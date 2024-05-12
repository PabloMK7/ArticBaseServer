#pragma once

#include "3ds/types.h"

namespace CTRPluginFramework
{
    class Time
    {
    public :

        constexpr Time(void) : _ticks(0) {}


        float           AsSeconds(void) const;

        int             AsMilliseconds(void) const;

        s64             AsMicroseconds(void) const;

        inline s64      AsTicks(void) const { return _ticks; }

        static const Time Zero; ///< Predefined "zero" time value

        static constexpr u32 TicksPerSecond = 268111856U;

    private :

        friend      constexpr Time Seconds(float amount);
        friend      constexpr Time Milliseconds(int amount);
        friend      constexpr Time Microseconds(s64 amount);
        friend      constexpr Time Ticks(s64 amount);

        constexpr Time(s64 ticks) : _ticks(ticks) {}

    private :


        s64     _ticks;
    };

    constexpr Time    Seconds(float amount)
    {
        return (Time(static_cast<s64>(amount * Time::TicksPerSecond)));
    }


    constexpr Time    Milliseconds(int amount)
    {
        return (Time(static_cast<s64>(amount * (Time::TicksPerSecond / 1000.f))));
    }


    constexpr Time    Microseconds(s64 amount)
    {
        return (Time(static_cast<s64>(amount * (Time::TicksPerSecond / 1000000.f))));
    }

    constexpr Time    Ticks(s64 amount)
    {
        return (Time(amount));
    }

    inline bool operator ==(const Time& left, const Time& right)
    {
        return (left.AsTicks() == right.AsTicks());
    }


    inline bool operator !=(const Time& left, const Time& right)
    {
        return (left.AsTicks() != right.AsTicks());
    }


    inline bool operator <(const Time& left, const Time& right)
    {
        return (left.AsTicks() < right.AsTicks());
    }


    inline bool operator >(const Time& left, const Time& right)
    {
        return (left.AsTicks() > right.AsTicks());
    }


    inline bool operator <=(const Time& left, const Time& right)
    {
        return (left.AsTicks() <= right.AsTicks());
    }

    inline bool operator >=(const Time& left, const Time& right)
    {
        return (left.AsTicks() >= right.AsTicks());
    }

    inline Time operator -(const Time& right)
    {
        return (Ticks(-right.AsTicks()));
    }


    inline Time operator +(const Time& left, const Time& right)
    {
        return (Ticks(left.AsTicks() + right.AsTicks()));
    }


    inline Time& operator +=(Time& left, const Time& right)
    {
        return (left = left + right);
    }


    inline Time operator -(const Time& left, const Time& right)
    {
        return (Ticks(left.AsTicks() - right.AsTicks()));
    }


    inline Time& operator -=(Time& left, const Time& right)
    {
        return left = left - right;
    }


    inline Time operator *(const Time& left, float right)
    {
        return (Seconds(left.AsSeconds() * right));
    }


    inline Time operator *(const Time& left, s64 right)
    {
        return (Microseconds(left.AsMicroseconds() * right));
    }


    inline Time operator *(float left, const Time& right)
    {
        return (right * left);
    }


    inline Time operator *(s64 left, const Time& right)
    {
        return (right * left);
    }


    inline Time& operator *=(Time& left, float right)
    {
        return (left = left * right);
    }


    inline Time& operator *=(Time& left, s64 right)
    {
        return (left = left * right);
    }


    inline Time operator /(const Time& left, float right)
    {
        return Seconds(left.AsSeconds() / right);
    }


    inline Time operator /(const Time& left, s64 right)
    {
        return (Microseconds(left.AsMicroseconds() / right));
    }


    inline Time& operator /=(Time& left, float right)
    {
        return (left = left / right);
    }


    inline Time& operator /=(Time& left, s64 right)
    {
        return (left = left / right);
    }


    inline float operator /(const Time& left, const Time& right)
    {
        return (left.AsSeconds() / right.AsSeconds());
    }


    inline Time operator %(const Time& left, const Time& right)
    {
        return (Ticks(left.AsTicks() % right.AsTicks()));
    }


    inline Time& operator %=(Time& left, const Time& right)
    {
        return (left = left % right);
    }

}

