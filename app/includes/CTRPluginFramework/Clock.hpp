#pragma once
#include "3ds.h"
#include "CTRPluginFramework/Time.hpp"

namespace CTRPluginFramework
{
    class Clock
    {
    public:
        Clock(void)  : _startTime(GetCurrentTime()) {}
        constexpr Clock(const Time& time) : _startTime(time) {}

        __always_inline Time GetElapsedTime(void) const {
            return (GetCurrentTime() - _startTime);
        }

        __always_inline bool HasTimePassed(const Time& time) const {
            return (GetElapsedTime() >= time);
        }

        __always_inline Time Restart(void) {
            const Time now = GetCurrentTime();

            const Time ret = now - _startTime;

            _startTime = now;
            return (ret);
        }
    private:
        Time    _startTime;

        static __always_inline Time GetCurrentTime(void)
        {
            return Ticks(svcGetSystemTick());
        }
    };
}
