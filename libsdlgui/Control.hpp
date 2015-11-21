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
		HiddenPending    = 0x1,
		Hidden           = 0x2,
		MouseOverControl = 0x4,
		Focused          = 0x8,
		DisableEffects   = 0x10
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
	bool EffectsDisabled() const { return (m_flags & State::DisableEffects) == State::DisableEffects; }
	virtual void FocusAcquired();
	virtual void FocusLost();
	Control* GetParent() const { return m_pParent; }
	Window* GetWindow() const { return m_pWindow; }
	bool HasFocus() const { return (m_flags & State::Focused) == State::Focused; }
	bool IsMouseOverControl() const { return (m_flags & State::MouseOverControl) == State::MouseOverControl; }
	virtual void RenderImpl() = 0;

	virtual void NotificationMouseEnter();
	virtual void NotificationMouseExit();

public:
	Control(Window* pWindow, const SDL_Rect& location);
	Control(Control* pParent, const SDL_Rect& location);
	virtual ~Control();

	void AddControl(Control* pControl);

	SDL_Color GetBackgroundColor() const { return m_bColor; }
	uint8_t GetBorderSize() const { return m_borderSize; }
	SDL_Color GetForegroundColor() const { return m_fColor; }
	bool GetHidden() const { return (m_flags & State::Hidden) == State::Hidden; }

	// gets the location of the control with respect to its parent
	SDL_Rect GetLocation() const { return m_loc; }

	// TODO: would be nice to take these out of public surface area
	virtual void NotificationElapsedTime();
	virtual void NotificationEvent(const SDL_Event& event);
	virtual bool NotificationMouseButton(SDL_MouseButtonEvent buttonEvent);
	void NotificationMouseMotion(SDL_MouseMotionEvent motionEvent);
	virtual void NotificationWindowChanged(Window* pWindow);

	void RemoveAllControls();
	void RemoveControl(Control* pControl);

	// render the control
	void Render();
	void SetBackgroundColor(const SDL_Color& color);
	void SetBorderColor(const SDL_Color& color);
	void SetBorderSize(uint8_t size);
	void SetFocus(bool hasFocus);
	void SetForegroundColor(const SDL_Color& color);

	virtual void SetHidden(bool isHidden);
	virtual void SetLocation(const SDL_Rect& location);
	void SetTransitionEffects(bool enabled);
};

#endif // CONTROL_HPP
