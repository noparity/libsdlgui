#ifndef FONT_HPP
#define FONT_HPP

#include "Flags.hpp"
#include "SdlHelpers.hpp"

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
    Flags<Attributes> m_attribs;

public:
    Font(TTFFont& ttfFont, const std::string& name, uint8_t size, Attributes attributes);

    Attributes GetAttributes() const { return m_attribs.Get(); }
    size_t GetCharSize(char c);
    size_t GetHeight();
    size_t GetLineSkipHeight();
    std::string GetName() const { return m_name; }
    size_t GetTextSize(const char* text);
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

namespace std
{
    template<>
    struct hash<Font>
    {
        size_t operator()(const Font& font) const
        {
            return std::hash<std::string>()(font.GetName() + std::to_string(font.GetSize()));
        }
    };
}

#endif // FONT_HPP
