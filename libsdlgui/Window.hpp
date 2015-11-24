#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Anchor.hpp"
#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem.hpp>
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

struct Dimentions
{
	int W;
	int H;
	Dimentions(int w, int h) : W(w), H(h) {}
};

class Control;

class Window
{
private:
	using ControlElapsedTime = std::tuple<Control*, uint32_t, uint32_t>;

	enum State : uint32_t
	{
		None           = 0,
		Minimized      = 0x1,
		CursorHidden   = 0x2,
		Closed         = 0x4,
		EventsDisabled = 0x8
	};

	uint32_t m_id;
	Flags<State> m_flags;
	SDL_Color m_bColor;
	SDL_Color m_fColor;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	Dimentions m_dims;
	std::vector<Control*> m_controls;
	Control* m_pCtrlWithFocus;
	SDLSubSystem m_subSystem;
	Font* m_pFont;
	std::vector<ControlElapsedTime> m_ctrlsElapsedTime;
	boost::dynamic_bitset<> m_occlusionMap;

	// static fields
	static std::vector<Window*> s_windows;

	void OnMouseButton(SDL_MouseButtonEvent buttonEvent);
	void OnMouseMotion(SDL_MouseMotionEvent motionEvent);
	void OnWindowResized(SDL_WindowEvent windowEvent);
	bool ShouldRender();

	// static functions
	static void RegisterWindow(Window* window);
	static void UnregisterWindow(Window* window);

protected:
	bool GetCursorHidden() const { return (m_flags & State::CursorHidden) == State::CursorHidden; }
	SDL_Window* GetWindow() const { return m_window; }
	void Show();
	virtual void TranslateEvent(const SDL_Event& sdlEvent);

public:
	Window(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags);
	virtual ~Window();

	void AddControl(Control* pControl);
	SDLTexture CreateTextureForText(const std::string& text, Font const* font, const SDL_Color& fgColor, const SDL_Color& bgColor);
	void DisableEvents() { m_flags |= State::EventsDisabled; }
	void EnableEvents() { m_flags ^= State::EventsDisabled; }
	void DrawLine(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color);
	void DrawRectangle(const SDL_Rect& location, const SDL_Color& color, uint8_t thickness);
	void DrawText(const SDL_Rect& location, const SDLTexture& texture, TextAlignment alignment, Anchor anchor);
	SDL_Color GetBackgroundColor() const { return m_bColor; }
	Dimentions GetDimentions() const { return m_dims; }
	Font* GetFont() const { return m_pFont; }
	SDL_Color GetForegroundColor() const { return m_fColor; }
	uint32_t GetId() const { return m_id; }
	bool IsActive() const { return (m_flags & State::Closed) != State::Closed; }
	void RemoveAllControls();
	void RemoveControl(Control* pControl);
	void Render();

	void RegisterForElapsedTimeNotification(Control* pControl, uint32_t ticks);

	void SetCursorHidden(bool hidden);
	void SetDefaultBackgroundColor(const SDL_Color& color) { m_bColor = color; }
	void SetDefaultFont(Font* pFont) { m_pFont = pFont; }
	void SetDefaultForegroundColor(const SDL_Color& color) { m_fColor = color; }

	void UnregisterForElapsedTimeNotification(Control* pControl);

	// static functions

	static bool ActiveWindows();
	static void DispatchEvents();
};

#endif // WINDOW_HPP
