#include "stdafx.h"
#include "label.hpp"
#include "window.hpp"

namespace libsdlgui
{
    Label::Label(Window* pWindow, const SDL_Rect& location) :
        Control(pWindow, location), m_text("label")
    {
        // default font is inherited from the window
        m_pFont = detail::GetFont(pWindow);
        assert(m_pFont != nullptr);
        m_texture = detail::CreateTextureForText(pWindow, m_text, m_pFont, GetForegroundColor(), GetBackgroundColor());
    }

    void Label::RenderImpl()
    {
        GetWindow()->DrawRectangle(GetLocation(), GetBackgroundColor(), UINT8_MAX);
        GetWindow()->DrawText(GetLocation(), m_texture, m_alignment);
    }

    void Label::SetAlignment(TextAlignment alignment)
    {
        m_alignment = alignment;
    }

    void Label::SetFont(Font* pFont)
    {
        if (pFont != m_pFont)
        {
            m_pFont = pFont;
            m_texture = detail::CreateTextureForText(GetWindow(), m_text, m_pFont, GetForegroundColor(), GetBackgroundColor());
        }
    }

    void Label::SetText(const std::string& text)
    {
        if (text != m_text)
        {
            m_text = text;
            m_texture = detail::CreateTextureForText(GetWindow(), m_text, m_pFont, GetForegroundColor(), GetBackgroundColor());
        }
    }

} // namespace libsdlgui
