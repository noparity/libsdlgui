#include "stdafx.h"
#include "caret.hpp"
#include "window.hpp"

namespace libsdlgui::detail
{
    Caret::Caret(Window* pWindow, const SDL_Rect& location) :
        m_pause(false), Control(pWindow, location)
    {
        // caret is hidden until its containing control has focus
        SetHidden(true);
    }

    void Caret::OnElapsedTime()
    {
        // oscillate between hidden/visible for blinking effect
        if (!m_pause)
            SetHidden(!GetHidden());
    }

    void Caret::PauseAnimation()
    {
        m_pause = true;
        SetHidden(false);
    }

    void Caret::RenderImpl()
    {
        auto loc = GetLocation();
        if (loc.w == 1)
            GetWindow()->DrawLine(SDLPoint(loc.x, loc.y), SDLPoint(loc.x, loc.y + loc.h - 1), GetForegroundColor());
        else
            GetWindow()->DrawRectangle(loc, GetForegroundColor(), UINT8_MAX);
    }

    void Caret::ResumeAnimation()
    {
        m_pause = false;
    }

    void Caret::StartAnimation()
    {
        SetHidden(false);
        // blink every second
        detail::RegisterForElapsedTimeNotification(GetWindow(), this, 1000);
    }

    void Caret::StopAnimation()
    {
        detail::UnregisterForElapsedTimeNotification(GetWindow(), this);
        SetHidden(true);
    }

} // namespace libsdlgui::detail
