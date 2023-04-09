#ifndef FONTMANAGER_HPP
#define FONTMANAGER_HPP

#include <filesystem>
#include "font.hpp"
#include "sdl_helpers.hpp"
#include "singleton.hpp"
#include <unordered_map>

namespace libsdlgui
{
    namespace detail
    {
        class FontManager
        {
        private:
            friend void Singleton<FontManager>::Initialize();
            friend void Singleton<FontManager>::Destroy();

            using FontPtr = std::unique_ptr<Font>;
            using FontCacheItem = std::pair<std::string, FontPtr>;
            using FontCache = std::unordered_map<std::string, FontPtr>;
            TTFInit m_init;
            std::filesystem::path m_fonts;
            FontCache m_cache;

            FontManager();
            ~FontManager();

        public:
            Font* GetOrLoadFont(const std::string& name, uint8_t size, Font::Attributes attributes = Font::Attributes::Normal);
        };
    }

    using FontManager = libsdlgui::detail::Singleton<detail::FontManager>;

} // namespace libsdlgui

#endif // FONTMANAGER_HPP
