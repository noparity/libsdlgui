#ifndef VERTICALSCROLLBAR_HPP
#define VERTICALSCROLLBAR_HPP

#include <functional>
#include "control.hpp"
#include "scroll_event_data.hpp"

namespace libsdlgui::detail
{
    class VerticalScrollbar : public Control
    {
    public:
        using ScrollCallback = std::function<void(const ScrollEventData&)>;

    private:
        enum class ButtonClicked : uint8_t
        {
            None = 0,
            Down,
            Up,
            Slider
        };

        SDL_Rect m_sliderLoc;
        ScrollCallback m_scrollCallback;
        uint32_t m_current;
        uint32_t m_max;
        uint32_t m_min;
        uint8_t m_largeChange;
        uint8_t m_smallChange;
        ButtonClicked m_held;
        bool m_showSlider;
        bool m_dragSlider;

        SDL_Rect GetButtonBounds(bool isUp) const;
        ButtonClicked GetButtonClicked(const SDL_Point& clickLoc) const;
        ScrollDirection GetScrollDirForButton(ButtonClicked button);
        void MoveSlider();
        virtual void OnElapsedTime();
        virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
        virtual void OnMouseExit();
        virtual void OnMouseMotion(const SDL_MouseMotionEvent& motionEvent);
        virtual void OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent);
        virtual void RenderImpl();
        void ScrollContent(ScrollDirection direction, ScrollMagnitude magnitude);

    public:
        VerticalScrollbar(Window* pWindow, const SDL_Rect& location, Control* parent);

        // gets the current position of the scroll slider
        uint32_t Current() const { return m_current; }

        // gets the number of items to scroll for a large change
        uint8_t LargeChange() const { return m_largeChange; }

        // gets the upper bound of the scrollable range
        uint32_t Maximum() const { return m_max; }

        // gets the lower bound of the scrollable range
        uint32_t Minimum() const { return m_min; }

        // registers a callback to be invoked when the scroll bar has been moved.
        void RegisterForScrollCallback(ScrollCallback&& callback) { m_scrollCallback = callback; }

        // sets the current position of the scroll slider
        void SetCurrent(uint32_t current);

        // sets the upper bound of the scrollable range
        void SetMaximum(uint32_t max);

        // sets the lower bound of the scrollable range
        void SetMinimum(uint32_t min) { m_min = min; }

        // gets the number of items to scroll for a small change
        uint8_t SmallChange() const { return m_smallChange; }
    };

} // namespace libsdlgui::detail

#endif // VERTICALSCROLLBAR_HPP
