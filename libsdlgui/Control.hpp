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
		Dirty            = 0x1,
		HiddenPending    = 0x2,
		Hidden           = 0x4,
		MouseOverControl = 0x8,
		Focused          = 0x10
	};

	Window* m_pWindow;
	Control* m_pParent;
	std::vector<Control*> m_controls;
	Flags<State> m_flags;
	SDL_Rect m_loc;
	SDL_Color m_bColor;
	SDL_Color m_fColor;
	SDL_Color m_borderColor;
	uint8_t m_borderSize;

protected:
	virtual void FocusAcquired();
	virtual void FocusLost();
	Control* GetParent() const { return m_pParent; }
	Window* GetWindow() const { return m_pWindow; }
	bool HasFocus() const { return (m_flags & State::Focused) == State::Focused; }
	bool IsMouseOverControl() const { return (m_flags & State::MouseOverControl) == State::MouseOverControl; }
	virtual void RenderImpl() = 0;

public:
	Control(Window* pWindow, const SDL_Rect& location);
	Control(Control* pParent, const SDL_Rect& location);
	~Control();

	void AddControl(Control* pControl);

	// indicates that this control should be rendered on the next frame
	void Invalidate();

	// returns true if the control's state has changed
	bool IsDirty() const { return (m_flags & State::Dirty) == State::Dirty; }

	SDL_Color GetBackgroundColor() const { return m_bColor; }
	SDL_Color GetForegroundColor() const { return m_fColor; }
	bool GetHidden() const { return (m_flags & State::Hidden) == State::Hidden; }

	// gets the location of the control with respect to its parent
	SDL_Rect GetLocation() const { return m_loc; }

	// TODO: would be nice to take these out of public surface area
	virtual void NotificationElapsedTime();
	virtual void NotificationEvent(const SDL_Event& event);
	virtual bool NotificationMouseButton(SDL_MouseButtonEvent buttonEvent);
	virtual void NotificationMouseMotion(SDL_MouseMotionEvent motionEvent);
	virtual void NotificationWindowChanged(Window* pWindow);

	void RemoveAllControls();
	void RemoveControl(Control* pControl);

	// render the control
	bool Render();
	bool SetBackgroundColor(const SDL_Color& color);
	bool SetBorderColor(const SDL_Color& color);
	bool SetBorderSize(uint8_t size);
	bool SetFocus(bool hasFocus);
	bool SetForegroundColor(const SDL_Color& color);

	virtual bool SetHidden(bool isHidden);
	virtual bool SetLocation(const SDL_Rect& location);
};

#endif // CONTROL_HPP