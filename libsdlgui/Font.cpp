#include "stdafx.h"
#include <cctype>
#include "Font.hpp"

Font::Font(TTFFont& ttfFont, const std::string& name, uint8_t size, Attributes attributes) :
	m_ttf(std::move(ttfFont)), m_name(name), m_size(size), m_attribs(attributes)
{
	// empty
}
