#include "stdafx.h"
#include "DrawingRoutines.hpp"
#include <iostream>
#include "VerticalScrollbar.hpp"

VerticalScrollbar::VerticalScrollbar(Window* pWindow, const SDL_Rect& location, uint32_t visible, Control* parent) :
    Control(pWindow, location, parent),
    m_count(0),
    m_visible(visible),
    m_range(0),
    m_held(ButtonClicked::None),
    m_sliderMoved(0),
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

uint8_t VerticalScrollbar::GetPixelsForScroll() const
{
    assert(m_showSlider);
    auto pixelsForScroll = std::ceil(static_cast<double>(m_range) / m_count);
    assert(pixelsForScroll > 0);
    assert(pixelsForScroll <= UINT8_MAX);
    return static_cast<uint8_t>(pixelsForScroll);
}

void VerticalScrollbar::MoveSlider(VerticalScrollbar::Direction direction)
{
    MoveSliderInternal(direction, false);
}

void VerticalScrollbar::MoveSliderImpl(int direction)
{
    assert(direction != 0);
    // keep the slider within bounds
    if (direction < 0)
    {
        auto upButton = GetButtonBounds(true);
        auto newYTop = m_sliderLoc.y + direction;
        if (newYTop < upButton.y + upButton.h)
            direction += (upButton.y + upButton.h) - newYTop;
    }
    else
    {
        auto downButton = GetButtonBounds(false);
        auto newYBottom = m_sliderLoc.y + m_sliderLoc.h + direction;
        if (newYBottom > downButton.y)
            direction -= newYBottom - downButton.y;
    }

    m_sliderLoc.y += direction;
}

void VerticalScrollbar::MoveSliderInternal(Direction direction, bool raiseEvent)
{
    if (m_showSlider)
    {
        auto pixelsToMove = static_cast<int>(GetPixelsForScroll());
        if (direction == VerticalScrollbar::Direction::TowardsBeginning)
            pixelsToMove = -pixelsToMove;

        MoveSliderImpl(pixelsToMove);
    }

    if (raiseEvent)
        RaiseScrollEvent(direction);
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

    VerticalScrollbar::Direction dir = VerticalScrollbar::Direction::TowardsEnd;
    if (m_held == ButtonClicked::Up)
        dir = VerticalScrollbar::Direction::TowardsBeginning;

    MoveSliderInternal(dir, true);

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
                VerticalScrollbar::Direction dir = VerticalScrollbar::Direction::TowardsEnd;
                if (buttonClicked == ButtonClicked::Up)
                    dir = VerticalScrollbar::Direction::TowardsBeginning;

                MoveSliderInternal(dir, true);

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
        // move the slider accordingly
        m_sliderMoved += static_cast<uint8_t>(std::abs(motionEvent.yrel));

        // track how far the scroll bar is moved, once it's moved
        // the requisite number of pixels raise the scroll event.
        if (m_sliderMoved >= GetPixelsForScroll())
        {
            m_sliderMoved = 0;
            VerticalScrollbar::Direction dir = VerticalScrollbar::Direction::TowardsEnd;
            if (motionEvent.yrel < 0)
                dir = VerticalScrollbar::Direction::TowardsBeginning;

            RaiseScrollEvent(dir);
        }

        MoveSliderImpl(motionEvent.yrel);
    }
}

void VerticalScrollbar::OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent)
{
    VerticalScrollbar::Direction dir = VerticalScrollbar::Direction::TowardsEnd;
    if (wheelEvent.y > 0)
        dir = VerticalScrollbar::Direction::TowardsBeginning;

    MoveSliderInternal(dir, true);
}

void VerticalScrollbar::RaiseScrollEvent(Direction direction)
{
    if (m_scrollCallback != nullptr)
        m_scrollCallback(direction);
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

void VerticalScrollbar::Resize(uint32_t count)
{
    m_count = count;

    // check the size of the control, if it's too
    // small then don't display the slider

    auto myLoc = GetLocation();
    auto downButton = GetButtonBounds(false);

    if (m_count > m_visible && myLoc.h > (downButton.h * 3))
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
        m_range = downButton.y - (upButton.y + upButton.h);
        
        double ratio = static_cast<double>(m_visible) / m_count;
        assert(ratio <= 1.0);

        m_sliderLoc.h = static_cast<int>(m_range * ratio);
        // don't let the slider get any smaller than a half button
        if (m_sliderLoc.h < downButton.h / 2)
            m_sliderLoc.h = downButton.h / 2;
    }
}
