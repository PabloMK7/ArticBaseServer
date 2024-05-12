#include "BCLIM.hpp"

namespace CTRPluginFramework {

    static inline int ColorClamp(int Color)
    {
        if (Color > 255) Color = 255;
        if (Color < 0) Color = 0;
        return Color;
    }

    const u8 BCLIM::textureTileOrder[] =
	{
		0,  1,   4,  5,
		2,  3,   6,  7,

		8,  9,   12, 13,
		10, 11,  14, 15
	};

    const u8 BCLIM::etc1Modifiers[][2] =
    {
        { 2, 8 },
        { 5, 17 },
        { 9, 29 },
        { 13, 42 },
        { 18, 60 },
        { 24, 80 },
        { 33, 106 },
        { 47, 183 }
    };

    Color BCLIM::OpaqueBlendFunc(const Color& dst, const Color& src) {
        return dst;
    }

    static Color ReadPixel(void* fb, int posX, int posY) {
        constexpr u32 _bytesPerPixel = 2;
        constexpr u32 _rowSize = 240;
        u32 offset = (_rowSize - 1 - posY + posX * _rowSize) * _bytesPerPixel;
        union
        {
            u16     u;
            u8      b[2];
        }           half;
        half.u = *reinterpret_cast<u16 *>((u32)fb + offset);
        Color c;
        c.r = (half.u >> 8) & 0xF8;
        c.g = (half.u >> 3) & 0xFC;
        c.b = (half.u << 3) & 0xF8;
        c.a = 255;
        return c;
    }
    static void WritePixel(void* fb, int posX, int posY, const Color& c) {
        constexpr u32 _bytesPerPixel = 2;
        constexpr u32 _rowSize = 240;
        u32 offset = (_rowSize - 1 - posY + posX * _rowSize) * _bytesPerPixel;
        union
        {
            u16     u;
            u8      b[2];
        }           half;
        half.u  = (c.r & 0xF8) << 8;
        half.u |= (c.g & 0xFC) << 3;
        half.u |= (c.b & 0xF8) >> 3;
        *reinterpret_cast<u16 *>((u32)fb + offset) = half.u;
    }

    void BCLIM::RenderInterfaceBackend(void* usrData, bool isRead, Color* c, int posX, int posY) {
        if (isRead) {
            *c = ReadPixel(gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL), posX, posY);
        } else {
            WritePixel(gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL), posX, posY, *c);
        }
    }

    static bool FastContains(const Rect<int>& rect, Vector<int> point) {
        return point.x >= rect.leftTop.x && point.x < (rect.leftTop.x + rect.size.x) &&
            point.y >= rect.leftTop.y && point.y < (rect.leftTop.y + rect.size.y);
    } 

    void BCLIM::Render(const Rect<int>& position, std::pair<void*, RenderBackend> backend, const Rect<int>& crop, const Rect<int>& limits, std::pair<bool, ColorBlendCallback> colorBlender) {
        auto mappingScale = [](const Rect<int>& position, int x, int y, int w, int h) {
            int posX = position.leftTop.x;
            int posY = position.leftTop.y;
            float progX = x/(float)w;
            float progY = y/(float)h;

            return Vector<int>(posX + position.size.x * progX, posY + position.size.y * progY);
        };
        auto mappingDirect = [](const Rect<int>& position, int x, int y, int w, int h) {
            return Vector<int>(position.leftTop.x + x, position.leftTop.y + y);
        };
        Vector<int>(*mapping)(const Rect<int>& position, int x, int y, int w, int h);
		Color current;

        if (position.size.x == header->imag.width && position.size.y == header->imag.height)
            mapping = mappingDirect;
        else
            mapping = mappingScale;

        switch (header->imag.format) {
            case TextureFormat::L8:
            case TextureFormat::A8:
            case TextureFormat::LA4:
            case TextureFormat::LA8:
            case TextureFormat::HILO8:
            case TextureFormat::RGB8:
            case TextureFormat::RGBA5551:
            case TextureFormat::RGBA8:
            case TextureFormat::ETC1:
            case TextureFormat::L4:
            case TextureFormat::A4:
            case TextureFormat::RGBA4:
            case TextureFormat::ETC1A4:
                break;
            case TextureFormat::RGB565:
            {
                int offs = 0;
                Vector<int> prevPos(-1, -1);
                for (int y = 0; y < header->imag.height; y+=8) {
                    for (int x = 0; x < header->imag.width; x+=8) {
                        for (int i = 0; i < 64; i++) {
                            int x2 = i % 8;
                            if (x + x2 >= crop.size.x || x + x2 >= header->imag.width) continue;
                            int y2 = i / 8;
                            if (y + y2 >= crop.size.y || y + y2 >= header->imag.height) continue;
                            auto drawPos = mapping(position, x + x2, y + y2, header->imag.width, header->imag.height);
                            if (!FastContains(limits, drawPos)) continue;
                            if (drawPos.x != prevPos.x || drawPos.y != prevPos.y) {
                                prevPos = drawPos;
                                int pos = textureTileOrder[x2 % 4 + y2 % 4 * 4] + 16 * (x2 / 4) + 32 * (y2 / 4);
                                u16 pixel = GetDataAt<u16>(offs + pos * 2);
                                u8 b = (u8)((pixel & 0x1F) << 3);
                                u8 g = (u8)((pixel & 0x7E0) >> 3);
                                u8 r = (u8)((pixel & 0xF800) >> 8);
                                if (colorBlender.first)
                                    backend.second(backend.first, true, &current, drawPos.x, drawPos.y);
                                Color finalcolor = colorBlender.second(Color(r, g, b), current);
                                backend.second(backend.first, false, &finalcolor, drawPos.x, drawPos.y);
                            }
                        }
                        offs += 64 * 2;
                    }
                }
            }
            break;
        }
    }
}