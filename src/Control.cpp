#include "stdafx.h"
#include "control.hpp"
#include "sdl_helpers.hpp"
#include "window.hpp"

namespace libsdlgui
{
    Control::Control(Window* pWindow, const SDL_Rect& location, Control* parent) :
        m_pWindow(pWindow), m_pParent(parent), m_flags(State::None), m_loc(location), m_borderSize(0), m_zOrder(0)
    {
        assert(m_pWindow != nullptr);
        m_borderColor = { 0, 0, 0, 0 };

        // inherit from window
        m_bColor = detail::GetBackgroundColor(m_pWindow);
        m_fColor = detail::GetForegroundColor(m_pWindow);

        detail::AddControl(m_pWindow, this);
    }

    Control::~Control()
    {
        detail::RemoveControl(m_pWindow, this);
    }

    bool Control::CanDrag() const
    {
        return CanDragImpl();
    }

    bool Control::CanDragImpl() const
    {
        return false;
    }

    bool Control::LeftMouseButtonDown(const SDL_MouseButtonEvent& buttonEvent)
    {
        // if the left mouse button was pressed return true
        return buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT;
    }

    bool Control::LeftMouseButtonUp(const SDL_MouseButtonEvent& buttonEvent)
    {
        return buttonEvent.state == SDL_RELEASED && buttonEvent.button == SDL_BUTTON_LEFT;
    }

    void Control::OnElapsedTime()
    {
        // empty
    }

    void Control::OnFocusAcquired()
    {
        // empty
    }

    void Control::OnFocusLost()
    {
        // empty
    }

    void Control::OnHiddenChanged(bool)
    {
        // empty
    }

    void Control::OnKeyboard(const SDL_KeyboardEvent&)
    {
        // empty
    }

    void Control::OnLeftClick(const SDL_Point&)
    {
        // empty
    }

    void Control::OnLocationChanged(int, int)
    {
        // empty
    }

    void Control::OnMiddleClick(const SDL_Point&)
    {
        // empty
    }

    bool Control::OnMouseButton(const SDL_MouseButtonEvent&)
    {
        // don't acquire focus by default
        return false;
    }

    void Control::OnMouseButtonExternal(const SDL_MouseButtonEvent&, Control*)
    {
        // empty
    }

    void Control::OnMouseEnter()
    {
        // empty
    }

    void Control::OnMouseExit()
    {
        // empty
    }

    void Control::OnMouseMotion(const SDL_MouseMotionEvent&)
    {
        // empty
    }

    void Control::OnMouseWheel(const SDL_MouseWheelEvent&)
    {
        // empty
    }

    void Control::OnResize(int, int)
    {
        // empty
    }

    void Control::OnRightClick(const SDL_Point&)
    {
        // empty
    }

    void Control::OnTextInput(const SDL_TextInputEvent&)
    {
        // empty
    }

    void Control::OnWindowChanged()
    {
        // empty
    }

    void Control::OnZOrderChanged()
    {
        // empty
    }

    void Control::Render()
    {
        if (!GetHidden())
        {
            RenderImpl();

            // border drawn last so it overlays the control's content
            if (m_borderSize > 0)
                m_pWindow->DrawRectangle(GetLocation(), m_borderColor, m_borderSize);
        }
    }

    void Control::SetBackgroundColor(const SDL_Color& color)
    {
        m_bColor = color;
    }

    void Control::SetBorderColor(const SDL_Color& color)
    {
        m_borderColor = color;
    }

    void Control::SetBorderSize(uint8_t size)
    {
        m_borderSize = size;
    }

    void Control::SetForegroundColor(const SDL_Color& color)
    {
        m_fColor = color;
    }

    void Control::SetHidden(bool isHidden)
    {
        if (isHidden && (m_flags & State::Hidden) != State::Hidden)
        {
            m_flags |= State::Hidden;
            OnHiddenChanged(true);
        }
        else if (!isHidden && (m_flags & State::Hidden) == State::Hidden)
        {
            m_flags ^= State::Hidden;
            OnHiddenChanged(false);
        }
    }

    void Control::SetLocation(const SDL_Rect& location)
    {
        if (m_loc != location)
        {
            auto oldLoc = m_loc;
            m_loc = location;

            // check location
            int deltaX = location.x - oldLoc.x;
            int deltaY = location.y - oldLoc.y;

            if (deltaX != 0 || deltaY != 0)
                OnLocationChanged(deltaX, deltaY);

            // check size
            deltaX = location.h - oldLoc.h;
            deltaY = location.w - oldLoc.w;

            if (deltaX != 0 || deltaY != 0)
                OnResize(deltaX, deltaY);
        }
    }

    namespace detail
    {
        uint8_t GetZOrder(Control const* pControl)
        {
            return pControl->m_zOrder;
        }

        void NotificationElapsedTime(Control* pControl)
        {
            pControl->OnElapsedTime();
        }

        void NotificationFocusAcquired(Control* pControl)
        {
            pControl->m_flags |= Control::State::Focused;
            pControl->OnFocusAcquired();
        }

        void NotificationFocusLost(Control* pControl)
        {
            pControl->m_flags ^= Control::State::Focused;
            pControl->OnFocusLost();
        }

        void NotificationKeyboard(Control* pControl, const SDL_KeyboardEvent& keyboardEvent)
        {
            pControl->OnKeyboard(keyboardEvent);
        }

        bool NotificationMouseButton(Control* pControl, const SDL_MouseButtonEvent& buttonEvent)
        {
            // raise the mouse button event before the click event
            auto result = pControl->OnMouseButton(buttonEvent);

            if (buttonEvent.state == SDL_PRESSED)
            {
                pControl->m_flags |= Control::State::MouseDown;
            }
            else if (pControl->m_flags & Control::State::MouseDown && buttonEvent.state == SDL_RELEASED)
            {
                auto clickLoc = SDLPoint(buttonEvent.x, buttonEvent.y);
                if (buttonEvent.button == SDL_BUTTON_LEFT)
                    pControl->OnLeftClick(clickLoc);
                else if (buttonEvent.button == SDL_BUTTON_RIGHT)
                    pControl->OnRightClick(clickLoc);
                else if (buttonEvent.button == SDL_BUTTON_MIDDLE)
                    pControl->OnMiddleClick(clickLoc);

                pControl->m_flags ^= Control::State::MouseDown;
            }

            return result;
        }

        void NotificationMouseButtonExternal(Control* pControl, const SDL_MouseButtonEvent& buttonEvent, Control* pOtherControl)
        {
            pControl->OnMouseButtonExternal(buttonEvent, pOtherControl);
        }

        void NotificationMouseEnter(Control* pControl)
        {
            pControl->OnMouseEnter();
        }

        void NotificationMouseExit(Control* pControl)
        {
            // if the mouse leaves the control while a button is down
            // clear the button down flag.  this prevents clicks from
            // being triggered in the following scenario
            // 
            // button A has the mouse button down then the mouse is moved out
            // mouse is pressed down someplace else then moved back over button A
            // mouse button is released over button A
            //
            // this behavior is slightly different from desktop window managers
            // but IMO is a reasonable compromise.
            pControl->m_flags ^= Control::State::MouseDown;
            pControl->OnMouseExit();
        }

        void NotificationMouseMotion(Control* pControl, const SDL_MouseMotionEvent& motionEvent)
        {
            pControl->OnMouseMotion(motionEvent);
        }

        void NotificationMouseWheel(Control* pControl, const SDL_MouseWheelEvent& wheelEvent)
        {
            pControl->OnMouseWheel(wheelEvent);
        }

        void NotificationTextInput(Control* pControl, const SDL_TextInputEvent& textEvent)
        {
            pControl->OnTextInput(textEvent);
        }

        void NotificationWindowChanged(Control* pControl)
        {
            pControl->OnWindowChanged();
        }

        void SetZOrder(Control* pControl, uint8_t zOrder)
        {
            pControl->m_zOrder = zOrder;
            pControl->OnZOrderChanged();
        }

    } // namespace detail

} // namespace libsdlgui
