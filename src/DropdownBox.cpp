#include "stdafx.h"
#include "DropdownBox.hpp"

DropdownBox::ContentBox::ContentBox(Window* pWindow, const SDL_Rect& location) :
    ListBox(pWindow, location)
{
    // empty
}

bool DropdownBox::ContentBox::OnMouseButton(const SDL_MouseButtonEvent&)
{
    // don't allow the content box to obtain focus
    return false;
}

DropdownBox::DropdownBox(Window* pWindow, const SDL_Rect& location) :
    Control(pWindow, location), m_contentBox(pWindow, SDLRect(location.x, location.y + location.h, location.w, 0))
{
    m_contentBox.SetHidden(true);
    m_contentBox.RegisterForSelectionChangedCallback([this](const std::string& item)
    {
        m_texture = GetWindow()->CreateTextureForText(item, GetWindow()->GetFont(), GetForegroundColor(), GetBackgroundColor());
        m_contentBox.SetHidden(true);

        if (m_callback != nullptr)
            m_callback(item);
    });

    SetBorderSize(1);
    SetBorderColor(SDLColor(0, 128, 0, 0));
    // set the default text
    m_texture = GetWindow()->CreateTextureForText("select...", GetWindow()->GetFont(), GetForegroundColor(), GetBackgroundColor());
}

void DropdownBox::AddItem(const std::string& item)
{
    m_contentBox.AddItem(item);
}

void DropdownBox::OnFocusAcquired()
{
    SetBorderColor(SDLColor(64, 64, 128, 0));
}

void DropdownBox::OnFocusLost()
{
    SetBorderColor(SDLColor(128, 128, 128, 0));
    m_contentBox.SetHidden(true);
}

void DropdownBox::OnLeftClick(const SDL_Point& clickLoc)
{
    auto myLoc = GetLocation();
    if (clickLoc.x >= (myLoc.x + myLoc.w) - (myLoc.h - 1))
        m_contentBox.SetHidden(!m_contentBox.GetHidden());
}

bool DropdownBox::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
    return LeftMouseButtonDown(buttonEvent);
}

void DropdownBox::OnMouseButtonExternal(const SDL_MouseButtonEvent& buttonEvent, Control* pControl)
{
    // if the left mouse button was pressed and it wasn't on the content box then hide it
    if (buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT && pControl != &m_contentBox)
        m_contentBox.SetHidden(true);
}

void DropdownBox::RegisterForSelectionChangedCallback(const SelectionChangedCallback& callback)
{
    m_callback = callback;
}

void DropdownBox::RenderImpl()
{
    auto myLoc = GetLocation();
    auto p1 = SDLPoint((myLoc.x + myLoc.w) - (myLoc.h - 1), myLoc.y);
    auto p2 = SDLPoint((myLoc.x + myLoc.w) - (myLoc.h - 1), myLoc.y + myLoc.h - 1);
    GetWindow()->DrawLine(p1, p2, SDLColor(128, 128, 128, 0));
    GetWindow()->DrawText(GetLocation(), m_texture, TextAlignment::MiddleLeft);
}
