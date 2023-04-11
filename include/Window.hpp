#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <boost/dynamic_bitset.hpp>
#include <filesystem>
#include "dimentions.hpp"
#include "flags.hpp"
#include "font.hpp"
#include <functional>
#include "sdl_helpers.hpp"
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <string>
#include "text_alignment.hpp"
#include <vector>

namespace libsdlgui
{
    // forward declaration to prevent circular reference
    class Control;

    // forward declaration for detail namespace
    class Window;

    namespace detail
    {
        // adds a control to the window so it can be rendered and receive events
        void AddControl(Window* pWindow, Control* pControl);

        // create an SDLTexture object for the specified text
        SDLTexture CreateTextureForText(Window const* pWindow, const std::string& text, Font const* font, const SDL_Color& fgColor, const SDL_Color& bgColor);

        // gets the window's background color
        SDL_Color GetBackgroundColor(Window const* pWindow);

        // gets the window's font
        Font* GetFont(Window const* pWindow);

        // gets the window's foreground color
        SDL_Color GetForegroundColor(Window const* pWindow);

        // registers a control to receive a callback on the specified interval.
        // doing subsequent calls with the same control will change the interval.
        void RegisterForElapsedTimeNotification(Window* pWindow, Control* pControl, uint32_t ticks);

        // removes the specified control from the window
        void RemoveControl(Window* pWindow, Control* pControl);

        // unregisters the elapsed time callback for the specified control
        void UnregisterForElapsedTimeNotification(Window* pWindow, Control* pControl);
    }

    // class that represents the app's window
    class Window
    {
    private:
        using ControlElapsedTime = std::tuple<Control*, uint32_t, uint32_t>;

        enum State : uint32_t
        {
            None = 0,
            Minimized = 0x1,
            CursorHidden = 0x2
        };

        detail::Flags<State> m_flags;
        SDL_Color m_bColor;
        SDL_Color m_fColor;
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        Dimentions m_dims;
        std::vector<Control*> m_controls;
        Control* m_pCtrlWithFocus;
        Control* m_pCtrlUnderMouse;
        SDLSubSystem m_subSystem;
        Font* m_pFont;
        std::vector<ControlElapsedTime> m_ctrlsElapsedTime;
        boost::dynamic_bitset<> m_occlusionMap;

        // returns true if the cursor is hidden
        bool GetCursorHidden() const { return (m_flags & State::CursorHidden) == State::CursorHidden; }

        void OnKeyboard(const SDL_KeyboardEvent& keyboardEvent);
        void OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
        void OnMouseMotion(const SDL_MouseMotionEvent& motionEvent);
        void OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent);
        void OnTextInput(const SDL_TextInputEvent& textEvent);
        void OnWindowResized(const SDL_WindowEvent& windowEvent);
        bool ShouldRender();

        friend void detail::AddControl(Window* pWindow, Control* pControl);
        friend SDLTexture detail::CreateTextureForText(Window const* pWindow, const std::string& text, Font const* font, const SDL_Color& fgColor, const SDL_Color& bgColor);
        friend SDL_Color detail::GetBackgroundColor(Window const* pWindow);
        friend Font* detail::GetFont(Window const* pWindow);
        friend SDL_Color detail::GetForegroundColor(Window const* pWindow);
        friend void detail::RegisterForElapsedTimeNotification(Window* pWindow, Control* pControl, uint32_t ticks);
        friend void detail::RemoveControl(Window* pWindow, Control* pControl);
        friend void detail::UnregisterForElapsedTimeNotification(Window* pWindow, Control* pControl);

    public:
        Window(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags);
        virtual ~Window();

        // draws a line of the specified color
        void DrawLine(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color);

        // draws a rectangle.  to draw a filled rectangle specify UINT8_MAX for thickness
        void DrawRectangle(const SDL_Rect& location, const SDL_Color& color, uint8_t thickness);

        // draws the specified text at the specified location
        void DrawText(const SDL_Rect& location, const SDLTexture& texture, TextAlignment alignment);

        // draws the specified texture at the specified location with an optional clipping rectangle
        void DrawTexture(const SDL_Rect& location, const SDLTexture& texture, SDL_Rect const* clip);

        // gets the window's dimentions
        Dimentions GetDimentions() const { return m_dims; }

        // removes all controls from the window
        void RemoveAllControls();

        // render the window and its contents
        void Render();

        // sets the cursor's hidden state
        void SetCursorHidden(bool hidden);

        // sets the window's background color
        void SetBackgroundColor(const SDL_Color& color) { m_bColor = color; }

        // sets the window's font
        void SetFont(Font* pFont) { m_pFont = pFont; }

        // sets the window's foreground color
        void SetForegroundColor(const SDL_Color& color) { m_fColor = color; }

        // processes the specified SDL_Event and should be called in the
        // app's main loop.  returns true if the quit event has been posted.
        bool TranslateEvent(const SDL_Event& sdlEvent);
    };

} // namespace libsdlgui

#endif // WINDOW_HPP
