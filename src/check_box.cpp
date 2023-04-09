#include "stdafx.h"
#include "check_box.hpp"
#include "window.hpp"

namespace libsdlgui
{
    CheckBox::CheckBox(Window* pWindow, const SDL_Rect& location) :
        Control(pWindow, location), m_text(pWindow, location), m_checked(false)
    {
        // move the text to the right of the check box and set some default properties
        m_text.SetLocation(SDLRect(location.x + 32, location.y, location.w, location.h));
        m_text.SetAlignment(TextAlignment::MiddleLeft);
        m_text.SetText("check box");
    }

    void CheckBox::OnLeftClick(const SDL_Point&)
    {
        m_checked = !m_checked;
        if (m_callback)
            m_callback(m_checked);
    }

    bool CheckBox::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
    {
        return LeftMouseButtonDown(buttonEvent);
    }

    void CheckBox::RegisterForCheckedChangedCallback(const CheckedChangedCallback& callback)
    {
        m_callback = callback;
    }

    void CheckBox::RenderImpl()
    {
        uint8_t thickness = m_checked ? UINT8_MAX : 1;
        auto checkLoc = GetLocation();
        checkLoc.x += 8;
        checkLoc.w = 16;
        checkLoc.y += (checkLoc.h / 2) - 10;
        checkLoc.h = 16;
        GetWindow()->DrawRectangle(checkLoc, GetForegroundColor(), thickness);
    }

} // namespace libsdlgui
