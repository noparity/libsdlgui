#ifndef VERTICALSCROLLBAR_HPP
#define VERTICALSCROLLBAR_HPP

#include "Control.hpp"

class VerticalScrollbar : public Control
{
public:
    enum class Direction
    {
        TowardsBeginning,
        TowardsEnd
    };
    using ScrollCallback = std::function<void(Direction)>;

private:
    enum class ButtonClicked : uint8_t
    {
        None = 0,
        Down,
        Up,
        Slider
    };

    uint32_t m_count;
    uint32_t m_visible;
    ScrollCallback m_scrollCallback;
    SDL_Rect m_sliderLoc;
    uint32_t m_range;
    ButtonClicked m_held;
    uint8_t m_sliderMoved;
    bool m_showSlider;
    bool m_dragSlider;

    SDL_Rect GetButtonBounds(bool isUp) const;
    ButtonClicked GetButtonClicked(const SDL_Point& clickLoc) const;
    uint8_t GetPixelsForScroll() const;
    void MoveSliderImpl(int direction);
    void MoveSliderInternal(Direction direction, bool raiseEvent);
    virtual void OnElapsedTime();
    virtual bool OnMouseButton(const SDL_MouseButtonEvent&);
    virtual void OnMouseExit();
    virtual void OnMouseMotion(const SDL_MouseMotionEvent& motionEvent);
    virtual void OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent);
    void RaiseScrollEvent(Direction direction);
    virtual void RenderImpl();

public:
    VerticalScrollbar(Window* pWindow, const SDL_Rect& location, uint32_t visible, Control* parent);

    // moves the slider in the specified direction, does not raise the scroll event
    void MoveSlider(Direction direction);

    // registers a callback to be invoked when the scroll bar has been moved.
    // the callback parameter contains the direction of the scroll.
    void RegisterForScrollCallback(ScrollCallback&& callback) { m_scrollCallback = callback; }

    // resizes the scroll bar based on the max number of items
    void Resize(uint32_t count);
};

#endif // VERTICALSCROLLBAR_HPP
