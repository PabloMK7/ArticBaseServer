#include "ArticBaseServer.hpp"
#include "ArticBaseFunctions.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <string_view>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern int transferedBytes;

ArticBaseServer::ArticBaseServer(int sock_fd) {
    socketFd = sock_fd;

    LightEvent_Init(&newPendingRequest, ResetType::RESET_ONESHOT);

    for (int i = 0; i < requestHandlers.size(); i++) {
        requestHandlers[i] = new RequestHandler(this, i);
    }
}

ArticBaseServer::~ArticBaseServer() {
    for (int i = 0; i < requestHandlers.size(); i++) {
        delete requestHandlers[i];
    }
    if (socketFd >= 0) {
        int fd = socketFd;
        socketFd = -1;
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}

void ArticBaseServer::Serve() {
    ArticBaseCommon::RequestPacket req;
    std::array<ArticBaseCommon::RequestParameter, MAX_PARAM_AMOUNT> params;
    int retryCount = 0;
    while (run && !stopQueried) {
        if (!Read(socketFd, &req, sizeof(req))) {
            if (stopQueried) {
                break;
            }
            if (run) {
                logger.Error("Server: Error reading from socket");
                svcSleepThread(1000000000);
                if (++retryCount == 3) {
                    break;
                }
            }
            continue;
        }

        std::array<char, sizeof(req.method) + 1> methodArray = {0};
        memcpy(methodArray.data(), req.method.data(), req.method.size());
        // Process special method now, delegate otherwise
        if (methodArray[0] == '$') {
            logger.Debug("Server: Processing %s (rID %d)", methodArray.data(), req.requestID);
            ArticBaseCommon::DataPacket resp{};
            resp.requestID = req.requestID;
            std::string_view method(methodArray.data());
            if (method == "$PING") {
                // Do nothing
            } else if (method == "$VERSION") {
                strcpy(resp.dataRaw, VERSION);
            } else if (method == "$PORTS") {
                snprintf(resp.dataRaw, sizeof(resp.dataRaw), "%d,%d,%d,%d", SERVER_PORT + 1, SERVER_PORT + 2, SERVER_PORT + 3, SERVER_PORT + 4);
            } else if (method == "$MAXSIZE") {
                snprintf(resp.dataRaw, sizeof(resp.dataRaw), "%d", ArticBaseServer::MAX_WORK_BUF_SIZE);
            } else if (method == "$MAXPARAM") {
                snprintf(resp.dataRaw, sizeof(resp.dataRaw), "%d", ArticBaseServer::MAX_PARAM_AMOUNT);
            } else if (method == "$READY") {
                bool ready = true;
                for (int i = 0; i < requestHandlers.size(); i++) {
                    if (!requestHandlers[i]->ready) {
                        ready = false;
                    }
                }
                snprintf(resp.dataRaw, sizeof(resp.dataRaw), "%d", ready ? 1 : 0);
            } else if (method == "$STOP") {
                stopQueried = true;
            } else {
                logger.Error("Server: Method not found: %s", methodArray.data());
            }
            if (!Write(socketFd, &resp, sizeof(resp))) {
                if (run) logger.Error("Server: Error writing to socket");
                continue;
            }
        } else {
            u32 readParams = 0;
            if (req.parameterCount) {
                if (req.parameterCount <= MAX_PARAM_AMOUNT) {
                    if (!Read(socketFd, params.data(), req.parameterCount * sizeof(ArticBaseCommon::RequestParameter))) {
                        if (run) logger.Error("Server: Error reading from socket");
                        continue;
                    }
                    readParams = req.parameterCount;
                } else {
                    logger.Error("Server: Too many parameters in request");
                    readParams = 0;
                }
            }
            
            {
                CTRPluginFramework::Lock l(pendingRequestsMutex);
                pendingRequests.push(Request{.reqPacket = req, .reqParameters = std::vector<ArticBaseCommon::RequestParameter>(params.begin(), params.begin() + readParams)});
            }
            LightEvent_Signal(&newPendingRequest);
        }
    }
    Stop();
}

void ArticBaseServer::QueryStop() {
    stopQueried = true;
    if (socketFd >= 0) {
        int fd = socketFd;
        socketFd = -1;
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}

bool ArticBaseServer::SetNonBlock(int sockFD, bool nonBlocking) {
    int flags = fcntl(sockFD, F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    if (nonBlocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    int res = fcntl(sockFD, F_SETFL, flags);
    if (res < 0) {
        return false;
    }
    return true;
}


void ArticBaseServer::Stop() {
    if (!run)
        return;
    run = false;
    if (socketFd >= 0) {
        int fd = socketFd;
        socketFd = -1;
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
    for (int i = 0; i < requestHandlers.size(); i++) {
        RequestHandler& handler = *requestHandlers[i];
        handler.run = false;
        if (handler.accept_fd >= 0) {
            int fd = handler.accept_fd;
            handler.accept_fd = -1;
            shutdown(fd, SHUT_RDWR);
            close(fd);
        }
        if (handler.listen_fd >= 0) {
            int fd = handler.listen_fd;
            handler.listen_fd = -1;
            shutdown(fd, SHUT_RDWR);
            close(fd);
        }
    }
    while(true) {
        bool allStopped = true;
        for (int i = 0; i < requestHandlers.size(); i++) {
            allStopped = allStopped && threadGetExitCode(requestHandlers[i]->thread) == 1;
        }
        if (allStopped)
            break;
        svcSleepThread(1000000);
        LightEvent_Signal(&newPendingRequest);
    } 
}

bool ArticBaseServer::Read(int& sockFD, void* buffer, size_t size) {
    size_t read_bytes = 0;
    while (read_bytes != size) {
        int new_read = recv(sockFD, (void*)((uintptr_t)buffer + read_bytes), size - read_bytes, 0);
        if (new_read < 0) {
            if (errno == EWOULDBLOCK) {
                svcSleepThread(1000000);
                continue;
            }
            read_bytes = 0;
            break;
        }
        transferedBytes += new_read;
        read_bytes += new_read;
    }
    return read_bytes == size;
}

bool ArticBaseServer::Write(int& sockFD, void* buffer, size_t size) {
    size_t write_bytes = 0;
    while (write_bytes != size)
    {
        int new_written = send(sockFD, (void*)((uintptr_t)buffer + write_bytes), size - write_bytes, 0);
        if (new_written < 0) {
            if (errno == EWOULDBLOCK) {
                svcSleepThread(1000000);
                continue;
            }
            write_bytes = 0;
            break;
        }
        transferedBytes += new_written;
        write_bytes += new_written;
    }
    return write_bytes == size;
}

size_t ArticBaseServer::RecvFrom(int& sockFD, void* buffer, size_t size, void* addr, void* addr_size) {
    while (true) {
        int new_read = recvfrom(sockFD, buffer, size, 0, (sockaddr*)addr, (socklen_t*)addr_size);
        if (new_read < 0) {
            if (errno == EWOULDBLOCK) {
                svcSleepThread(1000000);
                continue;
            }
            return 0;
        }
        transferedBytes += new_read;
        return new_read;
    }
}

size_t ArticBaseServer::SendTo(int& sockFD, void* buffer, size_t size, void* addr, void* addr_size) {
    socklen_t addr_len = *(socklen_t*)addr_size;
    while (true) {
        int new_written = sendto(sockFD, buffer, size, 0, (sockaddr*)addr, addr_len);
        if (new_written < 0) {
            if (errno == EWOULDBLOCK) {
                svcSleepThread(1000000);
                continue;
            }
            return 0;
        }
        transferedBytes += new_written;
        return new_written;
    }
}

ArticBaseServer::RequestHandler::RequestHandler(ArticBaseServer* serv, int id) {
    server = serv;
    this->id = id;

    workBufferSize = ArticBaseServer::MAX_WORK_BUF_SIZE;
    workBuffer = linearAlloc(workBufferSize);

    if (workBuffer == nullptr) {
        logger.Error("Worker %d: Failed to allocate work buffer", id);
    }

    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    thread = threadCreate(RequestHandler::HandleThread, this, 0x1000, prio - 1, -2, false);
}

ArticBaseServer::RequestHandler::~RequestHandler() {
    threadJoin(thread, U64_MAX);
    threadFree(thread);

    linearFree(workBuffer);
}

void ArticBaseServer::RequestHandler::HandleThread(void* arg) {
    RequestHandler* own = (RequestHandler*)arg;
    own->Serve();
    if (own->accept_fd >= 0) {
        int fd = own->accept_fd;
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
    logger.Debug("Worker %d: Exited", own->id);
    threadExit(1);
}

void ArticBaseServer::RequestHandler::Serve() {
    if (!workBuffer) {
        return;
    }

    struct sockaddr_in servaddr = {0};
    int res;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        logger.Error("Worker %d: Cannot create socket", id);
        return;
    }

    if (!ArticBaseServer::SetNonBlock(listen_fd, true)) {
        logger.Error("Worker %d: Cannot set non-block", id);
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERVER_PORT + id + 1);
    res = bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (res < 0) {
        logger.Error("Worker %d: Failed to bind() to port %d", id, SERVER_PORT + id + 1);
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    res = listen(listen_fd, 1);
    if (res < 0) {
        if (run) {
            logger.Error("Worker %d: Failed to listen()", id);
        }
        close(listen_fd);
        listen_fd = -1;
        return;
    }
    
    struct in_addr host_id;
    host_id.s_addr = gethostid();
    logger.Debug("Worker %d: Listening on: %s:%d", id, inet_ntoa(host_id), SERVER_PORT + id + 1);

    struct sockaddr_in peeraddr = {0};
    socklen_t peeraddr_len = sizeof(peeraddr);
    ready = true;
    while (true) {
        accept_fd = accept(listen_fd, (struct sockaddr *) &peeraddr, &peeraddr_len);
        if (accept_fd < 0 || peeraddr_len == 0) {
            if (errno == EWOULDBLOCK && run) {
                svcSleepThread(10000000);
                continue;
            }
            if (run) {
                logger.Error("Worker %d: Failed to accept()", id);
            }
            close(listen_fd);
            listen_fd = -1;
            return;
        }
        break;
    }
    close(listen_fd);
    listen_fd = -1;

    logger.Debug("Worker %d: Accepted %s:%d", id, inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

    if (!ArticBaseServer::SetNonBlock(accept_fd, true)) {
        logger.Error("Worker %d: Cannot set non-block", id);
        shutdown(accept_fd, SHUT_RDWR);
        close(accept_fd);
        accept_fd = -1;
        return;
    }

    while (true) {
        LightEvent_Wait(&server->newPendingRequest);
        if (!run) {
            break;
        }
        Request req;
        while (run) {
            {
                CTRPluginFramework::Lock l(server->pendingRequestsMutex);
                if (!server->pendingRequests.size())
                    break;
                req = server->pendingRequests.front();
                server->pendingRequests.pop();
            }

            ArticBaseCommon::DataPacket respPacket{};
            respPacket.requestID = req.reqPacket.requestID;

            std::array<char, sizeof(req.reqPacket.method) + 1> methodArray = {0};
            memcpy(methodArray.data(), req.reqPacket.method.data(), req.reqPacket.method.size());
            auto it = ArticBaseFunctions::functionHandlers.find(std::string(methodArray.data()));
            if (it == ArticBaseFunctions::functionHandlers.end()) {
                respPacket.resp.articResult = ArticBaseCommon::ResponseMethod::ArticResult::METHOD_NOT_FOUND;
                logger.Error("Worker %d: Method not found: %s", id, methodArray.data());
                if (!Write(accept_fd, &respPacket, sizeof(respPacket))) {
                    if (run)
                        logger.Error("Worker %d: Error writing to socket", id);
                    return;
                }
                continue;
            }

            logger.Debug("Worker %d: Processing %s (rID %d)", id, methodArray.data(), req.reqPacket.requestID);
            MethodInterface mi(req, workBuffer, workBufferSize, accept_fd);
            it->second(mi);

            ArticBaseCommon::MethodState mState = mi.GetMethodState();
            if (mState != ArticBaseCommon::MethodState::FINISHED) {
                respPacket.resp.articResult = ArticBaseCommon::ResponseMethod::ArticResult::METHOD_ERROR;
                logger.Error("Worker %d: %s method error: %d", id, methodArray.data(), mState);
                respPacket.resp.methodResult = (int)mState;
                if (!Write(accept_fd, &respPacket, sizeof(respPacket))) {
                    if (run)
                        logger.Error("Worker %d: Error writing to socket", id);
                    return;
                }
            } else {
                respPacket.resp.articResult = ArticBaseCommon::ResponseMethod::ArticResult::SUCCESS;
                respPacket.resp.methodResult = mi.GetMethodReturnValue();
                auto outBuffer = mi.GetOutputBuffer();
                respPacket.resp.bufferSize = outBuffer.second;
                if (!Write(accept_fd, &respPacket, sizeof(respPacket))) {
                    if (run)
                        logger.Error("Worker %d: Error writing to socket", id);
                    return;
                }
                if (outBuffer.second)
                    if (!Write(accept_fd, outBuffer.first, outBuffer.second)) {
                        if (run)
                            logger.Error("Worker %d: Error writing to socket", id);
                        return;
                    }
            }
        }
    }
}

bool ArticBaseServer::MethodInterface::GetParameterS8(s8& out) {
    if (state != MethodState::PARSING_INPUT)
        return false;

    if (currParameter >= req.reqPacket.parameterCount) {
        state = MethodState::PARAMETER_COUNT_MISMATCH;
        return false;
    }

    ArticBaseCommon::RequestParameter& currParam = req.reqParameters[currParameter++];

    if (currParam.type == ArticBaseCommon::RequestParameterType::IN_INTEGER_8) {
        out = *(s8*)currParam.data;
        return true;
    }

    state = MethodState::PARAMETER_TYPE_MISMATCH;
    return false;
}

bool ArticBaseServer::MethodInterface::GetParameterS16(s16& out) {
    if (state != MethodState::PARSING_INPUT)
        return false;

    if (currParameter >= req.reqPacket.parameterCount) {
        state = MethodState::PARAMETER_COUNT_MISMATCH;
        return false;
    }

    ArticBaseCommon::RequestParameter& currParam = req.reqParameters[currParameter++];

    if (currParam.type == ArticBaseCommon::RequestParameterType::IN_INTEGER_16) {
        out = *(s16*)currParam.data;
        return true;
    }

    state = MethodState::PARAMETER_TYPE_MISMATCH;
    return false;
}

bool ArticBaseServer::MethodInterface::GetParameterS32(s32& out) {
    if (state != MethodState::PARSING_INPUT)
        return false;

    if (currParameter >= req.reqPacket.parameterCount) {
        state = MethodState::PARAMETER_COUNT_MISMATCH;
        return false;
    }

    ArticBaseCommon::RequestParameter& currParam = req.reqParameters[currParameter++];

    if (currParam.type == ArticBaseCommon::RequestParameterType::IN_INTEGER_32) {
        out = *(s32*)currParam.data;
        return true;
    }

    state = MethodState::PARAMETER_TYPE_MISMATCH;
    return false;
}

bool ArticBaseServer::MethodInterface::GetParameterS64(s64& out) {
    if (state != MethodState::PARSING_INPUT)
        return false;

    if (currParameter >= req.reqPacket.parameterCount) {
        state = MethodState::PARAMETER_COUNT_MISMATCH;
        return false;
    }

    ArticBaseCommon::RequestParameter& currParam = req.reqParameters[currParameter++];

    if (currParam.type == ArticBaseCommon::RequestParameterType::IN_INTEGER_64) {
        out = *(s64*)currParam.data;
        return true;
    }

    state = MethodState::PARAMETER_TYPE_MISMATCH;
    return false;
}

bool ArticBaseServer::MethodInterface::GetParameterBuffer(void*& outBuff, size_t& outSize) {
    if (state != MethodState::PARSING_INPUT)
        return false;

    if (currParameter >= req.reqPacket.parameterCount) {
        state = MethodState::PARAMETER_COUNT_MISMATCH;
        return false;
    }

    ArticBaseCommon::RequestParameter& currParam = req.reqParameters[currParameter++];

    if (currParam.type == ArticBaseCommon::RequestParameterType::IN_SMALL_BUFFER) {
        outBuff = currParam.data;
        outSize = currParam.parameterSize;
        return true;
    } else if (currParam.type == ArticBaseCommon::RequestParameterType::IN_BIG_BUFFER) {
        // Perform request for buffer
        size_t bufferSize = *(size_t*)currParam.data;
        ArticBaseCommon::Buffer* buf = workBuffer.Reserve(currParam.bigBufferID, bufferSize);
        if (buf == nullptr) {
            state = MethodState::OUT_OF_MEMORY;
            return false;
        }

        ArticBaseCommon::DataPacket dataPacket{};
        dataPacket.requestID = req.reqPacket.requestID;
        dataPacket.resp.articResult = ArticBaseCommon::ResponseMethod::ArticResult::PROVIDE_INPUT;
        dataPacket.resp.provideInputBufferID = currParam.bigBufferID;
        dataPacket.resp.bufferSize = (int)bufferSize;
        
        if (!Write(socketFD, &dataPacket, sizeof(dataPacket))) {
            state = MethodState::BIG_BUFFER_WRITE_FAIL;
            return false;
        }

        if (!Read(socketFD, &dataPacket, sizeof(dataPacket))) {
            state = MethodState::BIG_BUFFER_READ_FAIL;
            return false;
        }
        
        if (dataPacket.requestID != req.reqPacket.requestID || 
            dataPacket.resp.articResult != ArticBaseCommon::ResponseMethod::ArticResult::PROVIDE_INPUT || 
            dataPacket.resp.provideInputBufferID != currParam.bigBufferID ||
            dataPacket.resp.bufferSize != (int)bufferSize) {

            state = MethodState::BIG_BUFFER_READ_FAIL;
            return false;
        }

        if (!Read(socketFD, buf->data, buf->bufferSize)) {
            state = MethodState::BIG_BUFFER_READ_FAIL;
            return false;
        }
        outBuff = buf->data;
        outSize = buf->bufferSize;
        return true;
    }
    state = MethodState::PARAMETER_TYPE_MISMATCH;
    return false;
}

ArticBaseCommon::Buffer* ArticBaseServer::MethodInterface::ReserveResultBuffer(u32 bufferID, size_t resultBuffSize) {
    if (state != MethodState::GENERATING_OUTPUT) {
        if (state == MethodState::PARSING_INPUT) {
            state = MethodState::UNEXPECTED_PARSING_INPUT;
        }
        return nullptr;
    }

    ArticBaseCommon::Buffer* buf = workBuffer.Reserve(bufferID, resultBuffSize);
    if (buf == nullptr) {
        state = MethodState::OUT_OF_MEMORY_OUTPUT;
        return nullptr;
    }
    return buf;
}

ArticBaseCommon::Buffer* ArticBaseServer::MethodInterface::ResizeLastResultBuffer(ArticBaseCommon::Buffer* buffer, size_t newSize) {
    if (state != MethodState::GENERATING_OUTPUT) {
        if (state == MethodState::PARSING_INPUT) {
            state = MethodState::UNEXPECTED_PARSING_INPUT;
        }
        return nullptr;
    }
    WorkBufferHandler::ResizeState resizeSate = workBuffer.ResizeLast(buffer, newSize);
    if (resizeSate != WorkBufferHandler::ResizeState::GOOD) {
        if (resizeSate == WorkBufferHandler::ResizeState::OUT_OF_MEMORY)
            state = MethodState::OUT_OF_MEMORY_OUTPUT;
        else
            state = MethodState::INTERNAL_METHOD_ERROR;
        return nullptr;
    }
    return buffer;
}

void ArticBaseServer::MethodInterface::FinishGood(int returnValue) {
    if (state == MethodState::GENERATING_OUTPUT)
        state = MethodState::FINISHED;
    if (state == MethodState::PARSING_INPUT)
        state = MethodState::UNEXPECTED_PARSING_INPUT;
    this->returnValue = returnValue;
}
void ArticBaseServer::MethodInterface::FinishInternalError() {
    if (state == MethodState::GENERATING_OUTPUT || state == MethodState::PARSING_INPUT)
        state = MethodState::INTERNAL_METHOD_ERROR;
    return;
}

ArticBaseServer::MethodInterface::WorkBufferHandler::ResizeState ArticBaseServer::MethodInterface::WorkBufferHandler::ResizeLast(ArticBaseCommon::Buffer* buffer, size_t newSize) {
    if (buffer->bufferSize == newSize)
        return ResizeState::GOOD;
    if ((uintptr_t)workBuffer + offset != (uintptr_t)buffer + buffer->bufferSize + sizeof(ArticBaseCommon::Buffer))
        return ResizeState::INPUT_ERROR;
    int sizeDiff = newSize - buffer->bufferSize;
    if (newSize == 0) {
        // Remove the buffer completely
        sizeDiff -= sizeof(ArticBaseCommon::Buffer);
    }
    if (offset + sizeDiff > workBufferSize)
        return ResizeState::OUT_OF_MEMORY;
    offset += sizeDiff;
    if (newSize != 0)
        buffer->bufferSize = newSize;
    return ResizeState::GOOD;
}