#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "Flags.hpp"
#include "Window.hpp"

class Control
{
private:
	enum State
	{
		None             = 0,
		Hidden           = 0x1,
		Focused          = 0x2,
		MouseDown        = 0x4
	};

	Window* m_pWindow;
	Flags<State> m_flags;
	SDL_Rect m_loc;
	SDL_Color m_bColor;
	SDL_Color m_fColor;
	SDL_Color m_borderColor;
	uint8_t m_borderSize;
	uint8_t m_zOrder;

	virtual bool CanDragImpl() const;
	virtual void OnElapsedTime();
	virtual void OnFocusAcquired();
	virtual void OnFocusLost();
	virtual void OnLeftClick();
	virtual void OnMiddleClick();
	virtual bool OnMouseButton(const SDL_MouseButtonEvent&);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMouseMotion(const SDL_MouseMotionEvent&);
	virtual void OnRightClick();
	virtual void OnSdlEvent(const SDL_Event&);
	virtual void OnWindowChanged(Window*);

	virtual void RenderImpl() = 0;

protected:
	Window* GetWindow() const { return m_pWindow; }
	bool HasFocus() const { return (m_flags & State::Focused) == State::Focused; }
	bool LeftMouseButtonDown(const SDL_MouseButtonEvent& buttonEvent);

public:
	Control(Window* pWindow, const SDL_Rect& location);
	virtual ~Control();

	bool CanDrag() const;
	SDL_Color GetBackgroundColor() const { return m_bColor; }
	uint8_t GetBorderSize() const { return m_borderSize; }
	SDL_Color GetForegroundColor() const { return m_fColor; }
	bool GetHidden() const { return (m_flags & State::Hidden) == State::Hidden; }

	// gets the location of the control with respect to its parent
	SDL_Rect GetLocation() const { return m_loc; }
	uint8_t GetZOrder() const { return m_zOrder; }

	void NotificationElapsedTime();
	void NotificationEvent(const SDL_Event& event);
	void NotificationFocusAcquired();
	void NotificationFocusLost();
	bool NotificationMouseButton(const SDL_MouseButtonEvent& buttonEvent);
	void NotificationMouseEnter();
	void NotificationMouseExit();
	void NotificationMouseMotion(const SDL_MouseMotionEvent& motionEvent);
	void NotificationWindowChanged(Window* pWindow);

	// render the control
	void Render();
	void SetBackgroundColor(const SDL_Color& color);
	void SetBorderColor(const SDL_Color& color);
	void SetBorderSize(uint8_t size);
	void SetForegroundColor(const SDL_Color& color);

	void SetHidden(bool isHidden);
	void SetLocation(const SDL_Rect& location);
	void SetZOrder(uint8_t zOrder);
};

#endif // CONTROL_HPP
