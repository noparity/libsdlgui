#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <boost/filesystem.hpp>
#include "DrawTextInfo.hpp"
#include "Flags.hpp"
#include "Font.hpp"
#include "SdlHelpers.hpp"
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
		None = 0,
		Minimized    = 0x1,
		CursorHidden = 0x2
	};

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
	bool m_pumpMessages;

	void OnMouseButton(SDL_MouseButtonEvent buttonEvent);
	void OnMouseMotion(SDL_MouseMotionEvent motionEvent);
	void OnWindowResized(SDL_WindowEvent windowEvent);

protected:
	virtual bool Render();

public:
	Window(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags);
	~Window();

	void AddControl(Control* pControl);
	void DrawLine(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color);
	void DrawRectangle(const SDL_Rect& location, const SDL_Color& color, uint8_t thickness);
	Dimentions DrawText(const DrawTextInfo& drawTextInfo);
	SDL_Color GetBackgroundColor() const { return m_bColor; }
	Dimentions GetDimentions() const { return m_dims; }
	Font* GetFont() const { return m_pFont; }
	SDL_Color GetForegroundColor() const { return m_fColor; }
	void PumpMessages();
	void RemoveAllControls();
	void RemoveControl(Control* pControl);

	void RegisterForElapsedTimeNotification(Control* pControl, uint32_t ticks);

	void SetDefaultBackgroundColor(const SDL_Color& color) { m_bColor = color; }
	void SetDefaultFont(Font* pFont) { m_pFont = pFont; }
	void SetDefaultForegroundColor(const SDL_Color& color) { m_fColor = color; }

	void UnregisterForElapsedTimeNotification(Control* pControl);
};

#endif // WINDOW_HPP