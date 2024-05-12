#include "CTRPluginFramework/Color.hpp"
#include <algorithm>

namespace CTRPluginFramework
{
    Color&   Color::Fade(float fading)
    {
        if (fading > 1.0f || fading < -1.0f)
            return *this;

        if (fading > 0.0f)
        {
            float tint = 1.f - fading;
            r = std::min((int)(255 - (255 - r) * tint), 255);
            g = std::min((int)(255 - (255 - g) * tint), 255);
            b = std::min((int)(255 - (255 - b) * tint), 255);
        }
        else
        {
            float shade = 1.f + fading;

            r *= shade;
            g *= shade;
            b *= shade;
        }
        return *this;
    }

    Color Color::Blend(const Color &color, BlendMode mode) const
    {
        // This is background, color is foreground
        Color           ret;
        unsigned int    _r;
        unsigned int    _g;
        unsigned int    _b;
        unsigned int    _a;
        float           forealpha = (float)color.a / 255.f;
        float           minusForeAlpha = 1.f - forealpha;

        switch (mode)
        {
        case BlendMode::Alpha:
            _r = (forealpha * (float)color.r) + ((float)r * minusForeAlpha);
            _g = (forealpha * (float)color.g) + ((float)g * minusForeAlpha);
            _b = (forealpha * (float)color.b) + ((float)b * minusForeAlpha);
            _a = color.a * a;
            ret.r = std::min(_r, 255u);
            ret.g = std::min(_g, 255u);
            ret.b = std::min(_b, 255u);
            ret.a = std::min(_a, 255u);
            break;
        case BlendMode::Add:
            _r = a * r / 255 + 255 * color.r / 255;
            _g = a * g / 255 + 255 * color.g / 255;
            _b = a * b / 255 + 255 * color.b / 255;
            _a = a + color.a;
            ret.r = std::min(_r, 255u);
            ret.g = std::min(_g, 255u);
            ret.b = std::min(_b, 255u);
            ret.a = std::min(_a, 255u);
            break;
        case BlendMode::Sub:
            _r = a * r / 255 - 255 * color.r / 255;
            _g = a * g / 255 - 255 * color.g / 255;
            _b = a * b / 255 - 255 * color.b / 255;
            _a = a - color.a;
            ret.r = std::max(_r, 0u);
            ret.g = std::max(_g, 0u);
            ret.b = std::max(_b, 0u);
            ret.a = std::max(_a, 0u);
            break;
        case BlendMode::Mul:
            ret = *this * color;
            break;
        default:
            ret = color;
            break;
        }
        return (ret);
    }

    bool Color::operator < (const Color &right) const
    {
        return (( r < right.r)
            && (b < right.b)
            && (g < right.g));
    }

    bool Color::operator <= (const Color &right) const
    {
        return (( r <= right.r)
            && (b <= right.b)
            && (g <= right.g));
    }

    bool Color::operator > (const Color &right) const
    {
        return (( r > right.r)
            && (b > right.b)
            && (g > right.g));
    }

    bool Color::operator >= (const Color &right) const
    {
        return (( r >= right.r)
            && (b >= right.b)
            && (g >= right.g));
    }

    Color Color::operator+(const Color& right) const
    {
        u8 _r(std::min(r + right.r, 255));
        u8 _g(std::min(g + right.g, 255));
        u8 _b(std::min(b + right.b, 255));
        u8 _a(std::min(a + right.a, 255));

        return (Color(_r, _g, _b, _a));

    }

    Color Color::operator-(const Color& right) const
    {
        u8 _r(std::max(r - right.r, 0));
        u8 _g(std::max(g - right.g, 0));
        u8 _b(std::max(b - right.b, 0));
        u8 _a(std::max(a - right.a, 0));

        return (Color(_r, _g, _b, _a));
    }

    Color Color::operator*(const Color& right) const
    {
        u8 _r(r * right.r / 255);
        u8 _g(g * right.g / 255);
        u8 _b(b * right.b / 255);
        u8 _a(a * right.a / 255);

        return (Color(_r, _g, _b, _a));
    }

    Color &Color::operator+=(const Color& right)
    {
        *this = *this + right;
        return (*this);
    }

    Color &Color::operator-=(const Color& right)
    {
        *this = *this - right;
        return (*this);
    }

    Color &Color::operator*=(const Color& right)
    {
        *this = *this * right;
        return (*this);
    }
 }
