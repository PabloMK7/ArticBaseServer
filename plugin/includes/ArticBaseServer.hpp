#pragma once
#include "Main.hpp"
#include "ArticBaseCommon.hpp"
#include "queue"
#include <tuple>
#include "CTRPluginFramework/CTRPluginFramework.hpp"

class ArticBaseServer {
public:
    ArticBaseServer(int socket_fd);
    ~ArticBaseServer();

    void Serve();
    void QueryStop();

    static bool SetNonBlock(int sockFD, bool blocking);
private:
    void Stop();
    static constexpr size_t MAX_WORK_BUF_SIZE = 4 * 1024 * 1024 + 512 * 1024; // 4.5MB
    static constexpr size_t MAX_PARAM_AMOUNT = 10;

    static bool Read(int& sockFD, void* buffer, size_t size);
    static bool Write(int& sockFD, void* buffer, size_t size);

    class RequestHandler {
    public:
        RequestHandler(ArticBaseServer* serv, int id);
        ~RequestHandler();
        ArticBaseServer* server;
        Thread thread;
        static void HandleThread(void* arg);
        void Serve();
        bool ready = false;
        bool run = true;
        int workBufferSize;
        void* workBuffer;
        int id;
        int listen_fd = -1;
        int accept_fd = -1;
    };

    class Request {
    public:
        ArticBaseCommon::RequestPacket reqPacket;
        std::vector<ArticBaseCommon::RequestParameter> reqParameters;
    };

    static constexpr const char* VERSION = "0";

    int socketFd;
    bool run = true;
    bool stopQueried = false;

    std::queue<Request> pendingRequests;
    CTRPluginFramework::Mutex pendingRequestsMutex;
    LightEvent newPendingRequest;
    std::array<RequestHandler*, 4> requestHandlers;
    
public:
    class MethodInterface {
    public:
        using MethodState = ArticBaseCommon::MethodState;

        MethodInterface(Request& _req, void* _workBuffer, size_t _workBufferSize, int& _socketFD) : req(_req), workBuffer(_workBuffer, _workBufferSize), socketFD(_socketFD)  {};
        
        bool GetParameterS8(s8& out);
        bool GetParameterS16(s16& out);
        bool GetParameterS32(s32& out);
        bool GetParameterS64(s64& out);
        bool GetParameterBuffer(void*& outBuff, size_t& outSize);

        bool FinishInputParameters() {
            if (state != MethodState::PARSING_INPUT)
                return false;
            if (currParameter != req.reqPacket.parameterCount) {
                state = MethodState::PARAMETER_COUNT_MISMATCH;
                return false;
            }
            state = MethodState::GENERATING_OUTPUT;
            workBuffer.Clear();
            return true;
        }

        ArticBaseCommon::Buffer* ReserveResultBuffer(u32 bufferID, size_t resultBuffSize);
        ArticBaseCommon::Buffer* ResizeLastResultBuffer(ArticBaseCommon::Buffer* buffer, size_t newSize);

        void FinishGood(int returnValue);
        void FinishInternalError();

        int GetMethodReturnValue() {
            return returnValue;
        }
        MethodState GetMethodState() {
            return state;
        }
        
    private:
        class WorkBufferHandler
        {
        private:
            void* workBuffer;
            size_t workBufferSize;
            size_t offset = 0;
        public:
            WorkBufferHandler(void* _workBuffer, size_t _workBufferSize) : workBuffer(_workBuffer), workBufferSize(_workBufferSize) {}
            
            void Clear() {
                offset = 0;
            }

            size_t Capacity() {
                return workBufferSize;
            }

            size_t Size() {
                return offset;
            }

            ArticBaseCommon::Buffer* Reserve(u32 bufferID, u32 bufferSize) {
                if (offset + sizeof(ArticBaseCommon::Buffer) + bufferSize > workBufferSize) {
                    logger.Error("o=0x%08X, bs=0x%08X, wbs=0x%08X", offset, bufferSize, workBufferSize);
                    return nullptr;
                }
                ArticBaseCommon::Buffer* buf = (ArticBaseCommon::Buffer*)((uintptr_t)workBuffer + offset);
                offset += sizeof(ArticBaseCommon::Buffer) + bufferSize;
                buf->bufferID = bufferID;
                buf->bufferSize = bufferSize;
                return buf;
            }
            
            enum class ResizeState {
                GOOD,
                INPUT_ERROR,
                OUT_OF_MEMORY,
            };
            ResizeState ResizeLast(ArticBaseCommon::Buffer* buffer, size_t newSize);

            std::pair<void*, size_t> GetRaw() {
                return std::make_pair(workBuffer, Size());
            }
        };
        
        friend class RequestHandler;
        std::pair<void*, size_t> GetOutputBuffer() {
            return workBuffer.GetRaw();
        }
        Request& req;
        int& socketFD;
        WorkBufferHandler workBuffer;
        int currParameter = 0;
        MethodState state = MethodState::PARSING_INPUT;
        int returnValue = 0;
    };
};