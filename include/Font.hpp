#ifndef FONT_HPP
#define FONT_HPP

#include "flags.hpp"
#include "sdl_helpers.hpp"

namespace libsdlgui
{
    class Font
    {
    public:
        enum class Attributes : uint8_t
        {
            Normal = 0,
            Bold = TTF_STYLE_BOLD,
            Italic = TTF_STYLE_ITALIC,
            Underline = TTF_STYLE_UNDERLINE,
            Strikethrough = TTF_STYLE_STRIKETHROUGH
        };

    private:
        TTFFont m_ttf;
        std::string m_name;
        uint8_t m_size;
        detail::Flags<Attributes> m_attribs;

    public:
        Font(TTFFont& ttfFont, const std::string& name, uint8_t size, Attributes attributes);

        Attributes GetAttributes() const { return m_attribs.Get(); }
        uint32_t GetCharSize(char c);
        uint32_t GetHeight();
        uint32_t GetLineSkipHeight();
        std::string GetName() const { return m_name; }
        uint32_t GetTextSize(const char* text);
        TTF_Font* GetTtf() const { return m_ttf; }
        uint8_t GetSize() const { return m_size; }
    };

    inline bool operator==(const Font& lhs, const Font& rhs)
    {
        return lhs.GetName() == rhs.GetName() && lhs.GetSize() == rhs.GetSize() && lhs.GetAttributes() == rhs.GetAttributes();
    }

    inline bool operator!=(const Font& lhs, const Font& rhs)
    {
        return !(lhs == rhs);
    }

    inline Font::Attributes operator|(Font::Attributes lhs, Font::Attributes rhs)
    {
        return static_cast<Font::Attributes>(static_cast<std::underlying_type<Font::Attributes>::type>(lhs) |
            static_cast<std::underlying_type<Font::Attributes>::type>(rhs));
    }

    /*namespace std
    {
        template<>
        struct hash<Font>
        {
            size_t operator()(const Font& font) const
            {
                return std::hash<std::string>()(font.GetName() + std::to_string(font.GetSize()));
            }
        };
    }*/

} // namespace libsdlgui

#endif // FONT_HPP
