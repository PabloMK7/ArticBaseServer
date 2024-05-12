#include "3ds/types.h"

#include "CTRPluginFramework/Time.hpp"

namespace CTRPluginFramework
{
    const Time Time::Zero;

    float   Time::AsSeconds(void) const
    {
        return (_ticks / (float)TicksPerSecond);
    }


    int     Time::AsMilliseconds(void) const
    {
        return static_cast<int>(_ticks / (TicksPerSecond / 1000.f));
    }


    s64     Time::AsMicroseconds(void) const
    {
        return static_cast<s64>(_ticks / (TicksPerSecond / 1000000.f));
    }

}
