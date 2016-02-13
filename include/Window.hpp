#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem.hpp>
#include "Dimentions.hpp"
#include "Flags.hpp"
#include "Font.hpp"
#include "SdlHelpers.hpp"
#include "TextAlignment.hpp"
#include <functional>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <string>
#include <vector>

// forward declaration to prevent circular reference
class Control;

// class that represents the app's window
class Window
{
private:
    using ControlElapsedTime = std::tuple<Control*, uint32_t, uint32_t>;

    enum State : uint32_t
    {
        None           = 0,
        Minimized      = 0x1,
        CursorHidden   = 0x2
    };

    Flags<State> m_flags;
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

    void OnKeyboard(const SDL_KeyboardEvent& keyboardEvent);
    void OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
    void OnMouseMotion(const SDL_MouseMotionEvent& motionEvent);
    void OnTextInput(const SDL_TextInputEvent& textEvent);
    void OnWindowResized(const SDL_WindowEvent& windowEvent);
    bool ShouldRender();

protected:
    // returns true if the cursor is hidden
    bool GetCursorHidden() const { return (m_flags & State::CursorHidden) == State::CursorHidden; }

public:
    Window(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags);
    virtual ~Window();

    // adds a control to the window so it can be rendered and receive events
    void AddControl(Control* pControl);

    // create an SDLTexture object for the specified text
    SDLTexture CreateTextureForText(const std::string& text, Font const* font, const SDL_Color& fgColor, const SDL_Color& bgColor);

    // draws a line of the specified color
    void DrawLine(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color);

    // draws a rectangle.  to draw a filled rectangle specify UINT8_MAX for thickness
    void DrawRectangle(const SDL_Rect& location, const SDL_Color& color, uint8_t thickness);

    // draws the specified text at the specified location
    void DrawText(const SDL_Rect& location, const SDLTexture& texture, TextAlignment alignment);

    // draws the specified texture at the specified location with an optional clipping rectangle
    void DrawTexture(const SDL_Rect& location, const SDLTexture& texture, SDL_Rect const* clip);

    // gets the window's background color
    SDL_Color GetBackgroundColor() const { return m_bColor; }

    // gets the window's dimentions
    Dimentions GetDimentions() const { return m_dims; }

    // gets the window's font
    Font* GetFont() const { return m_pFont; }

    // gets the window's foreground color
    SDL_Color GetForegroundColor() const { return m_fColor; }

    // removes all controls from the window
    void RemoveAllControls();

    // removes the specified control from the window
    void RemoveControl(Control* pControl);

    // render the window and its contents
    void Render();

    // registers a control to receive a callback on the specified interval
    void RegisterForElapsedTimeNotification(Control* pControl, uint32_t ticks);

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

    // unregisters the elapsed time callback for the specified control
    void UnregisterForElapsedTimeNotification(Control* pControl);
};

#endif // WINDOW_HPP
