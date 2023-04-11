#ifndef CONTROL_HPP
#define CONTROL_HPP

#include <SDL_image.h>
#include "flags.hpp"

namespace libsdlgui
{
    // forward declaration
    class Window;

    // forward declaration for detail namespace
    class Control;

    namespace detail
    {
        // gets the Z-order of the control.  controls are rendered in ascending Z-order
        uint8_t GetZOrder(Control const* pControl);

        // notifies the control that its timer has elapsed
        void NotificationElapsedTime(Control* pControl);

        // notifies the control that it has acquired focus
        void NotificationFocusAcquired(Control* pControl);

        // notifies the control that it has lost focus
        void NotificationFocusLost(Control* pControl);

        // sends keyboard event data to the control
        void NotificationKeyboard(Control* pControl, const SDL_KeyboardEvent& keyboardEvent);

        // sends mouse button event data to the control (e.g. was clicked).  if
        // the function returns true it means the control should acquire focus.
        bool NotificationMouseButton(Control* pControl, const SDL_MouseButtonEvent& buttonEvent);

        // sends mouse button event data to the control when the event
        // happens outside the bounds of the control.  if the event happens
        // on a control a pointer to that control is provided.
        void NotificationMouseButtonExternal(Control* pControl, const SDL_MouseButtonEvent& buttonEvent, Control* pOtherControl);

        // notifies the control that the mouse has entered its bounding area
        void NotificationMouseEnter(Control* pControl);

        // notifies the control that the mouse has left its bounding area
        void NotificationMouseExit(Control* pControl);

        // sends mouse motion event data to the control (e.g. mouse is moving within the control)
        void NotificationMouseMotion(Control* pControl, const SDL_MouseMotionEvent& motionEvent);

        // sends mouse wheel event data to the control (e.g. scroll direction)
        void NotificationMouseWheel(Control* pControl, const SDL_MouseWheelEvent& wheelEvent);

        // sends text input events to the control (e.g. typing in a text box)
        void NotificationTextInput(Control* pControl, const SDL_TextInputEvent& textEvent);

        // notifies the control that its owning window has changed (e.g. was resized)
        void NotificationWindowChanged(Control* pControl);

        // sets the control's z-order. zero is the bottom and is the default value
        void SetZOrder(Control* pControl, uint8_t zOrder);
    }

    // base class from which all controls must derive
    class Control
    {
    private:
        enum State
        {
            None = 0,
            Hidden = 0x1,
            Focused = 0x2,
            MouseDown = 0x4
        };

        Window* m_pWindow;
        Control* m_pParent;
        detail::Flags<State> m_flags;
        SDL_Rect m_loc;
        SDL_Color m_bColor;
        SDL_Color m_fColor;
        SDL_Color m_borderColor;
        uint8_t m_borderSize;
        uint8_t m_zOrder;

        // extensibility points for derived classes (template method pattern)

        virtual bool CanDragImpl() const;
        virtual void OnElapsedTime();
        virtual void OnFocusAcquired();
        virtual void OnFocusLost();
        virtual void OnHiddenChanged(bool);
        virtual void OnKeyboard(const SDL_KeyboardEvent&);
        virtual void OnLeftClick(const SDL_Point&);
        virtual void OnLocationChanged(int, int);
        virtual void OnMiddleClick(const SDL_Point&);
        virtual bool OnMouseButton(const SDL_MouseButtonEvent&);
        virtual void OnMouseButtonExternal(const SDL_MouseButtonEvent&, Control* pControl);
        virtual void OnMouseEnter();
        virtual void OnMouseExit();
        virtual void OnMouseMotion(const SDL_MouseMotionEvent&);
        virtual void OnMouseWheel(const SDL_MouseWheelEvent&);
        virtual void OnResize(int, int);
        virtual void OnRightClick(const SDL_Point&);
        virtual void OnTextInput(const SDL_TextInputEvent&);
        virtual void OnWindowChanged();
        virtual void OnZOrderChanged();

        virtual void RenderImpl() = 0;

        friend uint8_t detail::GetZOrder(Control const* pControl);
        friend void detail::NotificationElapsedTime(Control* pControl);
        friend void detail::NotificationFocusAcquired(Control* pControl);
        friend void detail::NotificationFocusLost(Control* pControl);
        friend void detail::NotificationKeyboard(Control* pControl, const SDL_KeyboardEvent& keyboardEvent);
        friend bool detail::NotificationMouseButton(Control* pControl, const SDL_MouseButtonEvent& buttonEvent);
        friend void detail::NotificationMouseButtonExternal(Control* pControl, const SDL_MouseButtonEvent& buttonEvent, Control* pOtherControl);
        friend void detail::NotificationMouseEnter(Control* pControl);
        friend void detail::NotificationMouseExit(Control* pControl);
        friend void detail::NotificationMouseMotion(Control* pControl, const SDL_MouseMotionEvent& motionEvent);
        friend void detail::NotificationMouseWheel(Control* pControl, const SDL_MouseWheelEvent& wheelEvent);
        friend void detail::NotificationTextInput(Control* pControl, const SDL_TextInputEvent& textEvent);
        friend void detail::NotificationWindowChanged(Control* pControl);
        friend void detail::SetZOrder(Control* pControl, uint8_t zOrder);

    protected:
        // returns the window that owns the control
        Window* GetWindow() const { return m_pWindow; }

        // returns true if the control has focus
        bool HasFocus() const { return (m_flags & State::Focused) == State::Focused; }

        // returns true if the left mouse button was pressed on the control
        bool LeftMouseButtonDown(const SDL_MouseButtonEvent& buttonEvent);

        // returns true if the left mouse button was released on the control
        bool LeftMouseButtonUp(const SDL_MouseButtonEvent& buttonEvent);

    public:
        Control(Window* pWindow, const SDL_Rect& location, Control* parent = nullptr);
        virtual ~Control();

        // returns true if the control can be dragged
        bool CanDrag() const;

        // gets the background color for the control
        SDL_Color GetBackgroundColor() const { return m_bColor; }

        // gets the foreground color for the control
        SDL_Color GetForegroundColor() const { return m_fColor; }

        // returns true if the control is hidden and thus should not be rendered
        bool GetHidden() const { return (m_flags & State::Hidden) == State::Hidden; }

        // gets the location of the control
        SDL_Rect GetLocation() const { return m_loc; }

        // gets this control's parent, can be nullptr
        Control* GetParent() const { return m_pParent; }

        // render the control
        void Render();

        // sets the control's background color
        void SetBackgroundColor(const SDL_Color& color);

        // sets the control's border color
        void SetBorderColor(const SDL_Color& color);

        // sets the size of the control's border (zero for no border)
        void SetBorderSize(uint8_t size);

        // sets the control's foreground color
        void SetForegroundColor(const SDL_Color& color);

        // sets the control's hidden state
        void SetHidden(bool isHidden);

        // sets the control's location with respect to its owning window
        void SetLocation(const SDL_Rect& location);
    };

} // namespace libsdlgui

#endif // CONTROL_HPP
