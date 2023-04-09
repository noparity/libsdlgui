#include "stdafx.h"
#include "drawing_routines.hpp"
#include "vertical_scrollbar.hpp"
#include "window.hpp"

namespace libsdlgui::detail
{
    VerticalScrollbar::VerticalScrollbar(Window* pWindow, const SDL_Rect& location, Control* parent) :
        Control(pWindow, location, parent),
        m_current(0),
        m_max(0),
        m_min(0),
        m_largeChange(10),
        m_smallChange(1),
        m_held(ButtonClicked::None),
        m_showSlider(false),
        m_dragSlider(false)
    {
        m_sliderLoc = { 0, 0, 0, 0 };
    }

    SDL_Rect VerticalScrollbar::GetButtonBounds(bool isUp) const
    {
        auto myLoc = GetLocation();
        auto height = 0;

        if (myLoc.h < (myLoc.w * 2))
            height = myLoc.h / 2;
        else
            height = myLoc.w;

        SDL_Rect buttonLoc = { 0, 0, 0, 0 };

        buttonLoc.x = myLoc.x;
        if (isUp)
            buttonLoc.y = myLoc.y;
        else
            buttonLoc.y = (myLoc.y + myLoc.h) - height;

        buttonLoc.h = height;
        buttonLoc.w = myLoc.w;

        return buttonLoc;
    }

    VerticalScrollbar::ButtonClicked VerticalScrollbar::GetButtonClicked(const SDL_Point& clickLoc) const
    {
        ButtonClicked buttonClicked = ButtonClicked::None;

        auto downButton = GetButtonBounds(false);
        auto upButton = GetButtonBounds(true);

        if (SDLPointInRect(clickLoc, downButton))
            buttonClicked = ButtonClicked::Down;
        else if (SDLPointInRect(clickLoc, upButton))
            buttonClicked = ButtonClicked::Up;
        else if (m_showSlider && SDLPointInRect(clickLoc, m_sliderLoc))
            buttonClicked = ButtonClicked::Slider;

        return buttonClicked;
    }

    ScrollDirection VerticalScrollbar::GetScrollDirForButton(VerticalScrollbar::ButtonClicked button)
    {
        assert(button != ButtonClicked::Slider && button != ButtonClicked::None);
        ScrollDirection dir = ScrollDirection::Increment;
        if (button == ButtonClicked::Up)
            dir = ScrollDirection::Decrement;

        return dir;
    }

    void VerticalScrollbar::MoveSlider()
    {
        if (m_showSlider)
        {
            // get the height of the slider
            auto height = m_sliderLoc.h;

            // get the full range of pixels available for sliding
            auto downButton = GetButtonBounds(false);
            auto upButton = GetButtonBounds(true);
            auto range = downButton.y - (upButton.y + upButton.h) - height;

            // normalize the min/max range to the range of available pixels

            // y = Min + (x - A) * (Max - Min) / (B - A)
            m_sliderLoc.y = (upButton.y + upButton.h) + (m_current - m_min) * (range) / (m_max - m_min);
        }
    }

    void VerticalScrollbar::OnElapsedTime()
    {
        bool accelerate = false;

        if ((static_cast<uint8_t>(m_held) & 0x80) == 0x80)
        {
            // strip the bit and accelerate the scrolling
            m_held = static_cast<ButtonClicked>(static_cast<uint8_t>(m_held) & 0xf);
            accelerate = true;
        }

        ScrollContent(GetScrollDirForButton(m_held), ScrollMagnitude::Small);

        if (accelerate)
            GetWindow()->RegisterForElapsedTimeNotification(this, 100);
    }

    bool VerticalScrollbar::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
    {
        if (LeftMouseButtonDown(buttonEvent))
        {
            ButtonClicked buttonClicked = GetButtonClicked(SDLPoint(buttonEvent.x, buttonEvent.y));
            if (buttonClicked != ButtonClicked::None)
            {
                if (buttonClicked != ButtonClicked::Slider)
                {
                    ScrollContent(GetScrollDirForButton(buttonClicked), ScrollMagnitude::Small);

                    // track the button being held down and start a timer, this
                    // allows the scrolling to continue if the button is held down.
                    // set the most significant bit indicating that the timer
                    // just started.

                    m_held = static_cast<ButtonClicked>(static_cast<uint8_t>(buttonClicked) | 0x80);
                    GetWindow()->RegisterForElapsedTimeNotification(this, 500);
                }
                else
                {
                    m_dragSlider = true;
                }
            }
        }
        else if (LeftMouseButtonUp(buttonEvent))
        {
            m_held = ButtonClicked::None;

            // if the slider is being drug there is no need
            // to unregister our button-down callback
            if (!m_dragSlider)
                GetWindow()->UnregisterForElapsedTimeNotification(this);

            m_dragSlider = false;
        }

        return false;
    }

    void VerticalScrollbar::OnMouseExit()
    {
        GetWindow()->UnregisterForElapsedTimeNotification(this);
        m_held = ButtonClicked::None;
    }

    void VerticalScrollbar::OnMouseMotion(const SDL_MouseMotionEvent& motionEvent)
    {
        if (m_dragSlider && motionEvent.yrel != 0)
        {
            assert(m_showSlider);

            ScrollDirection dir = ScrollDirection::Increment;
            if (motionEvent.yrel < 0)
                dir = ScrollDirection::Decrement;

            ScrollContent(dir, ScrollMagnitude::Small);
        }
    }

    void VerticalScrollbar::OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent)
    {
        ScrollDirection dir = ScrollDirection::Increment;
        if (wheelEvent.y > 0)
            dir = ScrollDirection::Decrement;

        ScrollContent(dir, ScrollMagnitude::Small);
    }

    void VerticalScrollbar::RenderImpl()
    {
        auto myLoc = GetLocation();
        GetWindow()->DrawRectangle(myLoc, GetForegroundColor() / 2, UINT8_MAX);
        auto downButton = GetButtonBounds(false);
        GetWindow()->DrawRectangle(downButton, GetForegroundColor(), UINT8_MAX);
        DrawChevron(GetWindow(), downButton, GetBackgroundColor(), false);
        auto upButton = GetButtonBounds(true);
        GetWindow()->DrawRectangle(upButton, GetForegroundColor(), UINT8_MAX);
        DrawChevron(GetWindow(), upButton, GetBackgroundColor(), true);

        if (m_showSlider)
            GetWindow()->DrawRectangle(m_sliderLoc, SDLColor(230, 230, 230, 0), UINT8_MAX);
    }

    void VerticalScrollbar::ScrollContent(ScrollDirection direction, ScrollMagnitude magnitude)
    {
        // get the number of items to move based on the scroll type
        auto toMove = SmallChange();
        if (magnitude == ScrollMagnitude::Large)
            toMove = LargeChange();

        if (direction == ScrollDirection::Decrement)
        {
            if (m_current < toMove)
                toMove = static_cast<uint8_t>(m_current);

            m_current -= toMove;
        }
        else
        {
            auto remaining = m_current - (m_max);
            if (toMove > remaining)
                toMove = static_cast<uint8_t>(remaining);

            m_current += toMove;
        }

        MoveSlider();

        if (m_scrollCallback != nullptr)
        {
            ScrollEventData scrollEvent(m_current, direction, magnitude, ScrollOrientation::Vertical);
            m_scrollCallback(scrollEvent);
        }
    }

    void VerticalScrollbar::SetCurrent(uint32_t current)
    {
        m_current = current;
        MoveSlider();
    }

    void VerticalScrollbar::SetMaximum(uint32_t max)
    {
        m_max = max;

        // check the size of the control, if it's too
        // small then don't display the slider

        auto myLoc = GetLocation();
        auto downButton = GetButtonBounds(false);

        if (myLoc.h > (downButton.h * 3))
            m_showSlider = true;
        else
            m_showSlider = false;

        if (m_showSlider)
        {
            // scale the size of the slider based on the number
            // of items vs the height of the control.  the size
            // will be capped to keep it from getting too thin.

            auto upButton = GetButtonBounds(true);

            m_sliderLoc = GetLocation();
            m_sliderLoc.y = upButton.y + upButton.h;

            // get the range of pixels available
            auto range = downButton.y - (upButton.y + upButton.h);

            m_sliderLoc.h = static_cast<int>(range - m_max);
            // don't let the slider get any smaller than a half button
            if (m_sliderLoc.h < downButton.h / 2)
                m_sliderLoc.h = downButton.h / 2;
        }
    }

} // namespace libsdlgui::detail
