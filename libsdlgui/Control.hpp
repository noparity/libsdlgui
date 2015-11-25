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
		Focused          = 0x4
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
	uint8_t m_zOrder;

	virtual void OnElapsedTime();
	virtual void OnSdlEvent(const SDL_Event&);
	virtual void OnFocusAcquired();
	virtual void OnFocusLost();
	virtual bool OnMouseButton(const SDL_MouseButtonEvent&);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnWindowChanged(Window*);

	virtual void RenderImpl() = 0;

protected:
	Control* GetParent() const { return m_pParent; }
	Window* GetWindow() const { return m_pWindow; }
	bool HasFocus() const { return (m_flags & State::Focused) == State::Focused; }

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
	uint8_t GetZOrder() const { return m_zOrder; }

	void NotificationElapsedTime();
	void NotificationEvent(const SDL_Event& event);
	void NotificationFocusAcquired();
	void NotificationFocusLost();
	bool NotificationMouseButton(const SDL_MouseButtonEvent& buttonEvent);
	void NotificationMouseEnter();
	void NotificationMouseExit();
	void NotificationWindowChanged(Window* pWindow);

	void RemoveAllControls();
	void RemoveControl(Control* pControl);

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
