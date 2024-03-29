#include "stdafx.h"
#include "font_manager.hpp"
#include "helpers.hpp"

namespace libsdlgui
{
    FontManager::CountType FontManager::s_count = 0;
    FontManager::UnderlyingType FontManager::s_instance = nullptr;

    namespace detail
    {
        FontManager::FontManager()
        {
            // TODO: windows-only implementation
            m_fonts = std::filesystem::path(SafeGetEnv("windir")) /= "fonts";
        }

        FontManager::~FontManager()
        {
            // empty
        }

        Font* FontManager::GetOrLoadFont(const std::string& name, uint8_t size, Font::Attributes attributes)
        {
            // make a copy of name so we can normalize it
            auto lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](uint8_t c)
                {
                    return static_cast<uint8_t>(std::tolower(c));
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
    }

} // namespace libsdlgui
