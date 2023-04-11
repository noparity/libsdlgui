#include "stdafx.h"
#include "button.hpp"
#include "window.hpp"

namespace libsdlgui
{
    Button::Button(Window* pWindow, const SDL_Rect& location) :
        Control(pWindow, location)
    {
        SetDefaultColorScheme();
    }

    void Button::OnFocusAcquired()
    {
        SetBorderColor(SDLColor(64, 64, 128, 0));
    }

    void Button::OnFocusLost()
    {
        SetBackgroundColor(SDLColor(64, 64, 64, 0));
        SetBorderColor(SDLColor(128, 128, 128, 0));
    }

    void Button::OnLeftClick(const SDL_Point&)
    {
        SetMouseOverColorScheme();
        if (m_onClick != nullptr)
            m_onClick();
    }

    bool Button::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
    {
        auto result = LeftMouseButtonDown(buttonEvent);
        if (result)
            SetBackgroundColor(SDLColor(32, 32, 64, 0));

        return result;
    }

    void Button::OnMouseEnter()
    {
        SetMouseOverColorScheme();
    }

    void Button::OnMouseExit()
    {
        SetDefaultColorScheme();
    }

    void Button::RenderImpl()
    {
        GetWindow()->DrawRectangle(GetLocation(), GetBackgroundColor(), UINT8_MAX);

        // center the texture within the button
        auto myLoc = GetLocation();
        auto textureLoc = myLoc;

        if (m_texture.GetWidth() < myLoc.w)
        {
            textureLoc.w = m_texture.GetWidth();
            textureLoc.x += ((myLoc.w - m_texture.GetWidth()) / 2);
        }

        if (m_texture.GetHeight() < myLoc.h)
        {
            textureLoc.h = m_texture.GetHeight();
            textureLoc.y += ((myLoc.h - m_texture.GetHeight()) / 2);
        }

        GetWindow()->DrawTexture(textureLoc, m_texture, nullptr);
    }

    void Button::RegisterForClickCallback(const ButtonClickCallback& callback)
    {
        m_onClick = callback;
    }

    void Button::SetDefaultColorScheme()
    {
        SetBackgroundColor(SDLColor(64, 64, 64, 0));
        if (!HasFocus())
            SetBorderColor(SDLColor(128, 128, 128, 0));
        SetBorderSize(2);
    }

    void Button::SetMouseOverColorScheme()
    {
        SetBackgroundColor(SDLColor(64, 64, 128, 0));
        SetBorderColor(SDLColor(128, 128, 255, 0));
        SetBorderSize(1);
    }

    void Button::SetText(const std::string& text, const SDL_Color& fgColor, const SDL_Color& bgColor)
    {
        SetTexture(detail::CreateTextureForText(GetWindow(), text, detail::GetFont(GetWindow()), fgColor, bgColor));
    }

    void Button::SetTexture(SDLTexture& texture)
    {
        auto myLoc = GetLocation();
        if (texture.GetWidth() > myLoc.w)
            throw std::runtime_error("texture is too wide for button");
        if (texture.GetHeight() > myLoc.h)
            throw std::runtime_error("texture is too tall for button");

        m_texture = texture;
    }

    void Button::SetTexture(SDLTexture&& texture)
    {
        SetTexture(texture);
    }

} // namespace libsdlgui
