#include "stdafx.h"
#include <cctype>
#include "Font.hpp"

Font::Font(TTFFont& ttfFont, const std::string& name, uint8_t size, Attributes attributes) :
	m_ttf(std::move(ttfFont)), m_name(name), m_size(size), m_attribs(attributes)
{
	// empty
}

size_t Font::GetCharSize(char c)
{
	int minx, maxx, miny, maxy, advance;
	auto result = TTF_GlyphMetrics(m_ttf, c, &minx, &maxx, &miny, &maxy, &advance);
	return advance;
}

size_t Font::GetTextSize(const char* text)
{
	int w, h;
	auto result = TTF_SizeText(m_ttf, text, &w, &h);
	return w;
}
