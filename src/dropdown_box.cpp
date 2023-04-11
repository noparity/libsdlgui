#include "stdafx.h"
#include "drawing_routines.hpp"
#include "dropdown_box.hpp"
#include "window.hpp"

namespace libsdlgui
{
    DropdownBox::ContentBox::ContentBox(Window* pWindow, const SDL_Rect& location, uint32_t minVisible, uint32_t maxVisible, Control* parent) :
        ListBox(pWindow, location, minVisible, maxVisible, parent, true, false)
    {
        // empty
    }

    bool DropdownBox::ContentBox::IsSubControl(Control* pControl)
    {
        // walk the parent hierachy to see if the control
        // that was clicked is a child of m_content

        while (pControl != nullptr)
        {
            if (pControl == this)
                return true;

            pControl = pControl->GetParent();
        }

        return false;
    }

    bool DropdownBox::ContentBox::OnMouseButton(const SDL_MouseButtonEvent&)
    {
        // don't allow the content box to obtain focus
        return false;
    }

    DropdownBox::DropdownBox(Window* pWindow, const SDL_Rect& location) :
        Control(pWindow, location), m_content(pWindow, SDLRect(location.x, location.y + location.h, location.w, 0), 1, 10, this)
    {
        m_content.SetHidden(true);
        m_content.RegisterForSelectionChangedCallback([this](auto item)
            {
                m_texture = detail::CreateTextureForText(GetWindow(), item, detail::GetFont(GetWindow()), GetForegroundColor(), GetBackgroundColor());
                m_content.SetHidden(true);

                if (m_callback != nullptr)
                    m_callback(item);
            });
        m_content.SetZOrder(32);

        SetBorderSize(1);
        SetBorderColor(SDLColor(0, 128, 0, 0));
        // set the default text
        m_texture = detail::CreateTextureForText(GetWindow(), "select...", detail::GetFont(GetWindow()), GetForegroundColor(), GetBackgroundColor());
    }

    void DropdownBox::AddItem(const std::string& item)
    {
        m_content.AddItem(item);
    }

    void DropdownBox::OnFocusAcquired()
    {
        SetBorderColor(SDLColor(64, 64, 128, 0));
    }

    void DropdownBox::OnFocusLost()
    {
        SetBorderColor(SDLColor(128, 128, 128, 0));
        m_content.SetHidden(true);
    }

    void DropdownBox::OnKeyboard(const SDL_KeyboardEvent& keyboardEvent)
    {
        // forward to the underlying ListBox to handle arrow keys
        detail::NotificationKeyboard(&m_content, keyboardEvent);
    }

    void DropdownBox::OnLeftClick(const SDL_Point& clickLoc)
    {
        auto myLoc = GetLocation();
        if (clickLoc.x >= (myLoc.x + myLoc.w) - static_cast<int>(DropdownButtonWidth))
        {
            // by default the content box will be drawn under the drop-down box.
            // if this would cause the content box not to be fully drawn then
            // place it above the drop-down (assumes this would be a better fit).
            auto windowDims = GetWindow()->GetDimentions();
            auto contentLoc = m_content.GetLocation();

            if (contentLoc.y + contentLoc.h > windowDims.H)
            {
                contentLoc.y = myLoc.y - contentLoc.h;
                m_content.SetLocation(contentLoc);
            }

            m_content.SetHidden(!m_content.GetHidden());
        }
    }

    bool DropdownBox::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
    {
        return LeftMouseButtonDown(buttonEvent);
    }

    void DropdownBox::OnMouseButtonExternal(const SDL_MouseButtonEvent& buttonEvent, Control* pControl)
    {
        // if the left mouse button was pressed and it wasn't on the content box then hide it
        if (buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT && !m_content.IsSubControl(pControl))
            m_content.SetHidden(true);
    }

    void DropdownBox::OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent)
    {
        if (HasFocus() && m_content.GetHidden())
        {
            // scrolling over the dropdown box will change the selected item.
            // we can piggyback on the keyboard event to get the desired behavior.

            SDL_KeyboardEvent keyboardEvent = { 0 };
            keyboardEvent.state = SDL_PRESSED;

            if (wheelEvent.y > 0)
                keyboardEvent.keysym.sym = SDLK_UP;
            else
                keyboardEvent.keysym.sym = SDLK_DOWN;

            OnKeyboard(keyboardEvent);
        }
    }

    void DropdownBox::RegisterForSelectionChangedCallback(const SelectionChangedCallback& callback)
    {
        m_callback = callback;
    }

    void DropdownBox::RenderImpl()
    {
        auto myLoc = GetLocation();

        auto buttonStart = (myLoc.x + myLoc.w) - DropdownButtonWidth;
        auto buttonLoc = SDLRect(buttonStart, myLoc.y, DropdownButtonWidth, myLoc.h);
        GetWindow()->DrawRectangle(buttonLoc, GetForegroundColor(), UINT8_MAX);
        detail::DrawChevron(GetWindow(), buttonLoc, GetBackgroundColor(), false);
        GetWindow()->DrawText(GetLocation(), m_texture, TextAlignment::MiddleLeft);
    }

} // namespace libsdlgui
