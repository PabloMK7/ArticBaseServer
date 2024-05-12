#pragma once
#include "CTRPluginFramework/Vector.hpp"
#include <algorithm>

namespace CTRPluginFramework
{
    template <typename T>
    class Rect
    {
    public:
        Rect();
        Rect(const Vector<T>& leftTop, const Vector<T>& size);
        Rect(const Vector<T>& leftTop, T width, T height);
        Rect(T left, T top, const Vector<T>& size);
        Rect(T left, T top, T width, T height);
        template <typename U>
        explicit Rect(const Rect<U>& rect);

        bool Contains(T x, T y) const;
        bool Contains(const Vector<T>& point) const;
        bool Intersects(const Rect<T>& rect) const;
        bool Intersects(const Rect<T>& rect, Rect<T>& intersect) const;

        Vector<T> leftTop;
        Vector<T> size;
    };

    template <typename T>
    Rect<T>::Rect() : leftTop(0, 0), size(0, 0)
    {

    }

    template <typename T>
    Rect<T>::Rect(const Vector<T>& leftTopCorner, const Vector<T>& size)
    {
        leftTop = leftTopCorner;
        this->size = size;
    }

    template <typename T>
    Rect<T>::Rect(const Vector<T>& leftTopCorner, T width, T height)
    {
        leftTop = leftTopCorner;
        size.x = width;
        size.y = height;
    }

    template <typename T>
    Rect<T>::Rect(T left, T top, const Vector<T>& size)
    {
        leftTop.x = left;
        leftTop.y = top;
        this->size = size;
    }

    template <typename T>
    Rect<T>::Rect(T left, T top, T width, T height)
    {
        leftTop.x = left;
        leftTop.y = top;
        size.x = width;
        size.y = height;
    }

    template <typename T>
    template <typename U>
    Rect<T>::Rect(const Rect<U>& rect)
    {
        leftTop = reinterpret_cast<T>(rect.leftTop);
        size = reinterpret_cast<T>(rect.size);
    }

    template <typename T>
    bool Rect<T>::Contains(T x, T y) const
    {
        T minX = std::min(leftTop.x, leftTop.x + size.x);
        T maxX = std::max(leftTop.x, leftTop.x + size.x);
        T minY = std::min(leftTop.y, leftTop.y + size.y);
        T maxY = std::max(leftTop.y, leftTop.y + size.y);

        return (x >= minX && x < maxX
            && y >= minY && y < maxY);
    }

    template <typename T>
    bool Rect<T>::Contains(const Vector<T>& point) const
    {
        return (Contains(point.x, point.y));
    }

    template <typename T>
    bool Rect<T>::Intersects(const Rect<T>& rect) const
    {
        Rect<T> intersect;
        return (Intersects(rect, intersect));
    }

    template <typename T>
    bool Rect<T>::Intersects(const Rect<T> &rect, Rect<T> &intersect) const
    {
        T thisMinX = std::min(leftTop.x, leftTop.x + size.x);
        T thisMaxX = std::max(leftTop.x, leftTop.x + size.x);
        T thisMinY = std::min(leftTop.y, leftTop.y + size.y);
        T thisMaxY = std::max(leftTop.y, leftTop.y + size.y);
        T rectMinX = std::min(rect.leftTop.x, rect.leftTop.x + rect.size.x);
        T rectMaxX = std::max(rect.leftTop.x, rect.leftTop.x + rect.size.x);
        T rectMinY = std::min(rect.leftTop.y, rect.leftTop.y + rect.size.y);
        T rectMaxY = std::max(rect.leftTop.y, rect.leftTop.y + rect.size.y);

        T intersectLeftX = std::max(thisMinX, rectMinX);
        T intersectLeftY = std::max(thisMinY, rectMinY);
        T intersectRightX = std::min(thisMaxX, rectMaxX);
        T intersectRightY = std::min(thisMaxY, rectMaxY);

        if (intersectLeftX < intersectRightX && intersectLeftY < intersectRightY)
        {
            intersect = Rect<T>(intersectLeftX, intersectLeftY, intersectRightX - intersectLeftX,
                                intersectRightY - intersectLeftY);
            return (true);
        }
        intersect = Rect<T>(0, 0, 0, 0);
        return (false);
    }

    template <typename T>
    bool operator ==(Rect<T> &left, Rect<T> &right)
    {
        return (left.leftTop == right.leftTop
                && left.size == right.size);
    }

    template <typename T>
    bool operator !=(Rect<T> &left, Rect<T> &right)
    {
        return (left.leftTop != right.leftTop
            && left.size != right.size);
    }

    typedef Rect<unsigned int> UIntRect;
    typedef Rect<int> IntRect;
    typedef Rect<float> FloatRect;
}
