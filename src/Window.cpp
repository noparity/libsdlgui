#include "stdafx.h"
#include "control.hpp"
#include "cursor_manager.hpp"
#include "exceptions.hpp"
#include "font_manager.hpp"
#include "window.hpp"

namespace libsdlgui
{
    Window::Window(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags) :
        m_flags(State::None), m_dims(dimentions), m_pCtrlWithFocus(nullptr), m_pCtrlUnderMouse(nullptr), m_subSystem(SDLSubSystem::Video), m_pFont(nullptr)
    {
        m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            m_dims.W, m_dims.H, windowFlags);

        if (m_window == nullptr)
            throw SDLException("SDL_CreateWindow failed with error '" + SDLGetError() + "'.");

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        if (m_renderer == nullptr)
            throw SDLException("SDL_CreateRenderer failed with error '" + SDLGetError() + "'.");

        if ((windowFlags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED)
            m_flags |= State::Minimized;

        // set default colors
        m_bColor = SDLColor(0, 0, 0, 0);
        m_fColor = SDLColor(255, 255, 255, 0);

        // set default draw color
        SDL_SetRenderDrawColor(m_renderer, m_bColor.r, m_bColor.g, m_bColor.b, m_bColor.a);

        detail::CursorManager::Initialize();
        FontManager::Initialize();

        SDL_StopTextInput();
        m_pFont = FontManager::GetInstance()->GetOrLoadFont("consola", 16);
    }

    Window::~Window()
    {
        detail::CursorManager::Destroy();
        FontManager::Destroy();
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
    }

    void Window::DrawLine(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color)
    {
        SDLColorHolder colorHolder(m_renderer, color);
        SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
    }

    void Window::DrawRectangle(const SDL_Rect& location, const SDL_Color& color, uint8_t thickness)
    {
        assert(thickness > 0);
        // must preserve previous draw color
        SDLColorHolder colorHolder(m_renderer, color);

        if (thickness == UINT8_MAX)
        {
            SDL_RenderFillRect(m_renderer, &location);
        }
        else
        {
            // create a copy of location so we can modify it
            auto myLoc = location;
            for (uint8_t i = 0; i < thickness; ++i)
            {
                myLoc.x = myLoc.x + i;
                myLoc.y = myLoc.y + i;
                myLoc.w = myLoc.w - i * 2;
                myLoc.h = myLoc.h - i * 2;

                SDL_RenderDrawRect(m_renderer, &myLoc);
            }
        }
    }

    void Window::DrawText(const SDL_Rect& location, const SDLTexture& texture, TextAlignment alignment)
    {
        // there are three possibilities
        //   the size of location is equal to that of textSurface
        //   the size of location is larger than that of textSurface
        //   the size of location is smaller than that of textSurface

        int xOffset = 0;
        int yOffset = 0;

        // calculate xOffset
        switch (alignment)
        {
        case TextAlignment::BottomCenter:
        case TextAlignment::MiddleCenter:
        case TextAlignment::TopCenter:
            xOffset = (location.w - texture.GetWidth()) / 2;
            break;

        case TextAlignment::BottomRight:
        case TextAlignment::MiddleRight:
        case TextAlignment::TopRight:
            xOffset = location.w - texture.GetWidth();
            break;
        }

        // calculate yOffset
        switch (alignment)
        {
        case TextAlignment::BottomCenter:
        case TextAlignment::BottomLeft:
        case TextAlignment::BottomRight:
            yOffset = location.h - texture.GetHeight();
            break;

        case TextAlignment::MiddleCenter:
        case TextAlignment::MiddleLeft:
        case TextAlignment::MiddleRight:
            yOffset = (location.h - texture.GetHeight()) / 2;
            break;
        }

        if (texture.GetWidth() == location.w && texture.GetHeight() == location.h)
        {
            SDL_RenderCopy(m_renderer, texture, nullptr, &location);
        }
        else if (texture.GetWidth() < location.w && texture.GetHeight() < location.h)
        {
            SDL_Rect myLoc = { location.x + xOffset, location.y + yOffset, texture.GetWidth(), texture.GetHeight() };
            SDL_RenderCopy(m_renderer, texture, nullptr, &myLoc);
        }
        else
        {
            // at least one dimension of the size of location is smaller than
            // textSurface so we need to compute the appropriate clipping rect.

            SDL_Rect myLoc = location;
            SDL_Rect clip = { 0, 0 };

            if (texture.GetWidth() > location.w)
            {
                // clip off the right side of the text
                clip.w = location.w;
            }
            else
            {
                clip.w = texture.GetWidth();
                myLoc.x += xOffset;
                myLoc.w = texture.GetWidth();
            }

            if (texture.GetHeight() > location.h)
            {
                clip.h = location.h;
            }
            else
            {
                clip.h = texture.GetHeight();
                myLoc.y += yOffset;
                myLoc.h = texture.GetHeight();
            }

            SDL_RenderCopy(m_renderer, texture, &clip, &myLoc);
        }
    }

    void Window::DrawTexture(const SDL_Rect& location, const SDLTexture& texture, SDL_Rect const* clip)
    {
        SDL_RenderCopy(m_renderer, texture, clip, &location);
    }

    void Window::OnKeyboard(const SDL_KeyboardEvent& keyboardEvent)
    {
        if (m_pCtrlWithFocus != nullptr)
            detail::NotificationKeyboard(m_pCtrlWithFocus, keyboardEvent);
    }

    void Window::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
    {
        // if a control has focus and a click happens outside of that control
        // we need to send it a notification.  if the click happens on another
        // control pass the pointer to that control.  only do this when the
        // button has been pressed, never on release.
        bool notifyCtrl = buttonEvent.state == SDL_PRESSED;
        Control* pClickedCtrl = nullptr;

        // must iterate in descending z-order
        for (auto i = static_cast<int>(m_controls.size()) - 1; i > -1; --i)
        {
            if (m_controls[i]->GetHidden())
                continue;

            // find the control that was clicked on then dispatch the event
            auto clickPoint = SDLPoint(buttonEvent.x, buttonEvent.y);
            auto controlLoc = m_controls[i]->GetLocation();
            if (SDLPointInRect(clickPoint, controlLoc))
            {
                if (detail::NotificationMouseButton(m_controls[i], buttonEvent))
                {
                    // remove focus from the previous control and give it to the selected one
                    if (m_pCtrlWithFocus != nullptr && m_controls[i] != m_pCtrlWithFocus)
                        detail::NotificationFocusLost(m_pCtrlWithFocus);

                    // if this control already has focus don't notify it again
                    if (m_controls[i] != m_pCtrlWithFocus)
                    {
                        m_pCtrlWithFocus = m_controls[i];
                        detail::NotificationFocusAcquired(m_pCtrlWithFocus);
                    }

                    // a control that took focus was clicked, no need to notify
                    // the previous control as it would have received a notification
                    // that it lost focus.
                    notifyCtrl = false;
                }

                // remember the control that was clicked but did not take focus
                pClickedCtrl = m_controls[i];

                break;
            }
        }

        if (m_pCtrlWithFocus != nullptr && notifyCtrl)
            detail::NotificationMouseButtonExternal(m_pCtrlWithFocus, buttonEvent, pClickedCtrl);
    }

    void Window::OnMouseMotion(const SDL_MouseMotionEvent& motionEvent)
    {
        if (GetCursorHidden())
        {
            SDL_ShowCursor(SDL_ENABLE);
            SetCursorHidden(false);
        }

        if (motionEvent.state == SDL_PRESSED && m_pCtrlWithFocus != nullptr && detail::CanDrag(m_pCtrlWithFocus))
        {
            auto controlLoc = m_pCtrlWithFocus->GetLocation();
            controlLoc.x += motionEvent.xrel;
            controlLoc.y += motionEvent.yrel;
            m_pCtrlWithFocus->SetLocation(controlLoc);
        }

        bool isOverControl = false;
        // must iterate in descending z-order
        for (auto i = static_cast<int>(m_controls.size()) - 1; i > -1; --i)
        {
            if (m_controls[i]->GetHidden())
                continue;

            auto clickPoint = SDLPoint(motionEvent.x, motionEvent.y);
            auto controlLoc = m_controls[i]->GetLocation();
            if (SDLPointInRect(clickPoint, controlLoc))
            {
                // notify the previous control the mouse has left it
                if (m_pCtrlUnderMouse != nullptr && m_pCtrlUnderMouse != m_controls[i])
                    detail::NotificationMouseExit(m_pCtrlUnderMouse);

                // if this control is already under the mouse don't notify it again
                if (m_pCtrlUnderMouse != m_controls[i])
                {
                    m_pCtrlUnderMouse = m_controls[i];
                    detail::NotificationMouseEnter(m_pCtrlUnderMouse);
                }

                detail::NotificationMouseMotion(m_pCtrlUnderMouse, motionEvent);
                isOverControl = true;

                // mouse can't be over more than one control so exit
                break;
            }
        }

        // mouse is no longer over any control, if it was earlier
        // then notify that control that it has exited it.
        if (!isOverControl && m_pCtrlUnderMouse != nullptr)
        {
            detail::NotificationMouseExit(m_pCtrlUnderMouse);
            m_pCtrlUnderMouse = nullptr;
        }
    }

    void Window::OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent)
    {
        if (m_pCtrlUnderMouse != nullptr)
            detail::NotificationMouseWheel(m_pCtrlUnderMouse, wheelEvent);
    }

    void Window::OnTextInput(const SDL_TextInputEvent& textEvent)
    {
        // should have a control with focus (e.g. a text box)
        assert(m_pCtrlWithFocus != nullptr);
        detail::NotificationTextInput(m_pCtrlWithFocus, textEvent);
    }

    void Window::OnWindowResized(const SDL_WindowEvent& windowEvent)
    {
        // resizing the window creates a new rendering context
        // which requires the rendering surface to be cleared.
        SDL_RenderClear(m_renderer);

        // update dimentions
        m_dims.W = windowEvent.data1;
        m_dims.H = windowEvent.data2;

        // notify all controls of the change
        for (auto control : m_controls)
            detail::NotificationWindowChanged(control);
    }

    void Window::RemoveAllControls()
    {
        m_controls.clear();
        m_ctrlsElapsedTime.clear();
    }

    void Window::Render()
    {
        // notify any controls for elapsed time
        for (auto& control : m_ctrlsElapsedTime)
        {
            auto timeRequested = std::get<1>(control);
            auto timeElapsed = std::get<2>(control);
            auto currentTime = SDL_GetTicks();
            if (currentTime - timeElapsed >= timeRequested)
            {
                std::get<2>(control) = currentTime;
                detail::NotificationElapsedTime(std::get<0>(control));
            }
        }

        // only render if the window is visible
        if (ShouldRender())
        {
            SDL_RenderClear(m_renderer);

            for (auto const control : m_controls)
            {
                detail::Render(control);
            }

            SDL_RenderPresent(m_renderer);
        }
    }

    void Window::SetCursorHidden(bool hidden)
    {
        if (hidden && !GetCursorHidden())
        {
            SDL_ShowCursor(SDL_DISABLE);
            m_flags = State::CursorHidden;
        }
        else if (!hidden && GetCursorHidden())
        {
            SDL_ShowCursor(SDL_ENABLE);
            m_flags ^= State::CursorHidden;
        }
    }

    bool Window::ShouldRender()
    {
        return ((m_flags & State::Minimized) != State::Minimized);
    }

    bool Window::TranslateEvent(const SDL_Event& sdlEvent)
    {
        bool quit = false;

        switch (sdlEvent.type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            OnKeyboard(sdlEvent.key);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            OnMouseButton(sdlEvent.button);
            break;
        case SDL_MOUSEWHEEL:
            OnMouseWheel(sdlEvent.wheel);
            break;
        case SDL_MOUSEMOTION:
            OnMouseMotion(sdlEvent.motion);
            break;
        case SDL_TEXTINPUT:
            OnTextInput(sdlEvent.text);
            break;
        case SDL_WINDOWEVENT:
            switch (sdlEvent.window.event)
            {
            case SDL_WINDOWEVENT_MINIMIZED:
                m_flags |= State::Minimized;
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_RESTORED:
                m_flags ^= State::Minimized;
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                OnWindowResized(sdlEvent.window);
                break;
            }
            break;
        case SDL_QUIT:
            quit = true;
            break;
        }

        return quit;
    }

    namespace detail
    {
        void AddControl(Window* pWindow, Control* pControl)
        {
            assert(std::find(pWindow->m_controls.begin(), pWindow->m_controls.end(), pControl) == pWindow->m_controls.end());
            pWindow->m_controls.push_back(pControl);

            // sort the controls in ascending z-order
            std::sort(pWindow->m_controls.begin(), pWindow->m_controls.end(), [](Control* lhs, Control* rhs)
                {
                    return detail::GetZOrder(lhs) < detail::GetZOrder(rhs);
                });
        }

        SDL_Color GetBackgroundColor(Window const* pWindow)
        {
            return pWindow->m_bColor;
        }

        Font* GetFont(Window const* pWindow)
        {
            return pWindow->m_pFont;
        }

        SDL_Color GetForegroundColor(Window const* pWindow)
        {
            return pWindow->m_fColor;
        }

        void RegisterForElapsedTimeNotification(Window* pWindow, Control* pControl, uint32_t ticks)
        {
            // check if the control is already registered, if
            // it is then update the frequency and start ticks.

            std::vector<Window::ControlElapsedTime>::iterator iter;
            for (iter = pWindow->m_ctrlsElapsedTime.begin(); iter != pWindow->m_ctrlsElapsedTime.end(); ++iter)
            {
                if (std::get<0>(*iter) == pControl)
                    break;
            }

            if (iter == pWindow->m_ctrlsElapsedTime.end())
            {
                pWindow->m_ctrlsElapsedTime.push_back(Window::ControlElapsedTime(pControl, ticks, SDL_GetTicks()));
            }
            else
            {
                // update the value and start time
                std::get<1>(*iter) = ticks;
                std::get<2>(*iter) = SDL_GetTicks();
            }
        }

        void RemoveControl(Window* pWindow, Control* pControl)
        {
            auto controlIter = std::find(pWindow->m_controls.begin(), pWindow->m_controls.end(), pControl);
            assert(controlIter != pWindow->m_controls.end());
            if (controlIter != pWindow->m_controls.end())
            {
                detail::UnregisterForElapsedTimeNotification(pWindow, *controlIter);
                pWindow->m_controls.erase(controlIter);
            }
        }

        void UnregisterForElapsedTimeNotification(Window* pWindow, Control* pControl)
        {
            for (auto iter = pWindow->m_ctrlsElapsedTime.begin(); iter != pWindow->m_ctrlsElapsedTime.end(); ++iter)
            {
                if (std::get<0>(*iter) == pControl)
                {
                    pWindow->m_ctrlsElapsedTime.erase(iter);
                    break;
                }
            }
        }

    } // namespace detail

} // namespace libsdlgui
