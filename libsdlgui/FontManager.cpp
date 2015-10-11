#include "stdafx.h"
#include <cctype>
#include "FontManager.hpp"
#include "Helpers.hpp"

FontManager* FontManager::s_instance = nullptr;

FontManager::FontManager(boost::filesystem::path fontsLocation) :
	m_fonts(fontsLocation)
{
	// empty
}

FontManager::~FontManager()
{
	// empty
}

void FontManager::Destroy()
{
	delete s_instance;
	s_instance = nullptr;
}

void FontManager::Initialize()
{
	// TODO: windows-only implementation
	if (s_instance == nullptr)
		s_instance = new FontManager(boost::filesystem::path(SafeGetEnv("windir")) /= "fonts");
}

Font* FontManager::GetOrLoadFont(const std::string& name, uint8_t size, Font::Attributes attributes)
{
	// make a copy of name so we can normalize it
	auto lowerName = name;
	std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c)
	{
		return std::tolower(c);
	});

	auto fontIter = m_cache.find(lowerName);
	if (fontIter != m_cache.end())
		return fontIter->second.get();

	TTFFont ttf(m_fonts / (name + ".ttf"), size);
	auto font = std::make_unique<Font>(ttf, name, size, attributes);
	auto pFont = font.get();

	m_cache.insert(FontCacheItem(lowerName, std::move(font)));
	return pFont;
}
