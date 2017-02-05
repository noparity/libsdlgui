#include "stdafx.h"
#include "Control.hpp"
#include "SdlHelpers.hpp"

Control::Control(Window* pWindow, const SDL_Rect& location, Control* parent) :
    m_pWindow(pWindow), m_pParent(parent), m_flags(State::None), m_loc(location), m_borderSize(0), m_zOrder(0)
{
    assert(m_pWindow != nullptr);
    m_borderColor = { 0, 0, 0, 0 };

    // inherit from window
    m_bColor = m_pWindow->GetBackgroundColor();
    m_fColor = m_pWindow->GetForegroundColor();

    m_pWindow->AddControl(this);
}

Control::~Control()
{
    m_pWindow->RemoveControl(this);
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

void Control::NotificationElapsedTime()
{
    OnElapsedTime();
}

void Control::NotificationFocusAcquired()
{
    m_flags |= State::Focused;
    OnFocusAcquired();
}

void Control::NotificationFocusLost()
{
    m_flags ^= State::Focused;
    OnFocusLost();
}

void Control::NotificationKeyboard(const SDL_KeyboardEvent& keyboardEvent)
{
    OnKeyboard(keyboardEvent);
}

bool Control::NotificationMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
    // raise the mouse button event before the click event
    auto result = OnMouseButton(buttonEvent);

    if (buttonEvent.state == SDL_PRESSED)
    {
        m_flags |= State::MouseDown;
    }
    else if (m_flags & State::MouseDown && buttonEvent.state == SDL_RELEASED)
    {
        auto clickLoc = SDLPoint(buttonEvent.x, buttonEvent.y);
        if (buttonEvent.button == SDL_BUTTON_LEFT)
            OnLeftClick(clickLoc);
        else if (buttonEvent.button == SDL_BUTTON_RIGHT)
            OnRightClick(clickLoc);
        else if (buttonEvent.button == SDL_BUTTON_MIDDLE)
            OnMiddleClick(clickLoc);

        m_flags ^= State::MouseDown;
    }

    return result;
}

void Control::NotificationMouseButtonExternal(const SDL_MouseButtonEvent& buttonEvent, Control* pControl)
{
    OnMouseButtonExternal(buttonEvent, pControl);
}

void Control::NotificationMouseEnter()
{
    OnMouseEnter();
}

void Control::NotificationMouseExit()
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
    m_flags ^= State::MouseDown;
    OnMouseExit();
}

void Control::NotificationMouseMotion(const SDL_MouseMotionEvent& motionEvent)
{
    OnMouseMotion(motionEvent);
}

void Control::NotificationMouseWheel(const SDL_MouseWheelEvent& wheelEvent)
{
    OnMouseWheel(wheelEvent);
}

void Control::NotificationTextInput(const SDL_TextInputEvent& textEvent)
{
    OnTextInput(textEvent);
}

void Control::NotificationWindowChanged()
{
    OnWindowChanged();
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

void Control::SetZOrder(uint8_t zOrder)
{
    m_zOrder = zOrder;
    OnZOrderChanged();
}
