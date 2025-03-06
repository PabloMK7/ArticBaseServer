#pragma once
#include "3ds.h"
#include "ArticProtocolCommon.hpp"
#include "ArticProtocolServer.hpp"

namespace ArticFunctions {
    enum class HandleType {
        FILE,
        DIR,
        ARCHIVE
    };

    // Controller_Start
    namespace ArticController {
        extern Thread thread;
        extern bool thread_run;
        extern int socket_fd;
        extern volatile bool socket_ready;
        void Handler(void* arg);
    };
}