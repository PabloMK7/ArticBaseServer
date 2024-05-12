#pragma once
#include "3ds.h"
#include "map"
#include "ArticBaseCommon.hpp"
#include "ArticBaseServer.hpp"
#include "memory.h"
#include "string"

namespace ArticBaseFunctions {
    extern std::map<std::string, void(*)(ArticBaseServer::MethodInterface& out)> functionHandlers;
    extern std::vector<bool(*)()> setupFunctions;
    extern std::vector<bool(*)()> destructFunctions;

    enum class HandleType {
        FILE,
        DIR,
        ARCHIVE
    };
};