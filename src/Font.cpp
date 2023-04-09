#include "stdafx.h"
#include "font.hpp"

namespace libsdlgui
{
    Font::Font(TTFFont& ttfFont, const std::string& name, uint8_t size, Attributes attributes) :
        m_ttf(std::move(ttfFont)), m_name(name), m_size(size), m_attribs(attributes)
    {
        // empty
    }

    uint32_t Font::GetCharSize(char c)
    {
        int minx, maxx, miny, maxy, advance;
        auto result = TTF_GlyphMetrics(m_ttf, c, &minx, &maxx, &miny, &maxy, &advance);
        assert(result == 0);

        return advance;
    }

    uint32_t Font::GetHeight()
    {
        return TTF_FontHeight(m_ttf);
    }

    uint32_t Font::GetLineSkipHeight()
    {
        return TTF_FontLineSkip(m_ttf);
    }

    uint32_t Font::GetTextSize(const char* text)
    {
        int w, h;
        auto result = TTF_SizeText(m_ttf, text, &w, &h);
        assert(result == 0);

        return w;
    }

} // namespace libsdlgui
