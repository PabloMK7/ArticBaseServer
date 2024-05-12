#pragma once
#include "3ds/types.h"
#include <algorithm>
#include <string>

namespace CTRPluginFramework
{
    class Color
    {
    public:

        enum class BlendMode
        {
            Alpha,
            Add,
            Sub,
            Mul,
            None
        };

        constexpr Color(void) : a(255), b(0), g(0), r(0) {}
        constexpr Color(u32 color) : raw(color) {}
        constexpr Color(u8 red, u8 green, u8 blue, u8 alpha = 255) : a(alpha), b(blue), g(green), r(red) {}

        inline u32 ToU32(void) const { return raw; };
        Color   &Fade(float fading);
        Color   Blend(const Color &color, BlendMode mode) const;

        inline bool    operator == (const Color &right) const {return raw == right.raw;}
        inline bool    operator != (const Color &right) const {return raw != right.raw;}
        bool    operator < (const Color &right) const;
        bool    operator <= (const Color &right) const;
        bool    operator > (const Color &right) const;
        bool    operator >= (const Color &right) const;
        Color   operator + (const Color &right) const;
        Color   operator - (const Color &right) const;
        Color   operator * (const Color &right) const;
        Color   &operator += (const Color &right);
        Color   &operator -= (const Color &right);
        Color   &operator *= (const Color &right);

        operator std::string() const
        {
            char  strColor[5] = { 0 };

            strColor[0] = 0x1B;
            strColor[1] = std::max((u8)1, r);
            strColor[2] = std::max((u8)1, g);
            strColor[3] = std::max((u8)1, b);

            return strColor;
        }

        union
        {
            u32     raw;
            struct // Match raw byte order
            {
                u8      a;
                u8      b;
                u8      g;
                u8      r;
            };
        };
    };
}

