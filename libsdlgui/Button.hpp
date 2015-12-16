#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "Control.hpp"

class Button : public Control
{
public:
	using ButtonClickCallback = std::function<void()>;

private:
	ButtonClickCallback m_onClick;

	virtual void OnFocusAcquired();
	virtual void OnFocusLost();
	virtual void OnLeftClick(const SDL_Point&);
	virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void RenderImpl();

	void SetDefaultColorScheme();
	void SetMouseOverColorScheme();

public:
	Button(Window* pWindow, const SDL_Rect& location);

	void RegisterForClickCallback(const ButtonClickCallback& callback);
};

#endif // BUTTON_HPP
