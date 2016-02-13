#include "stdafx.h"
#include "Button.hpp"

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
