#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "Label.hpp"

// TODO: probably shouldn't subclass from label

class Button : public Label
{
public:
	using ButtonClickCallback = std::function<void()>;

private:
	ButtonClickCallback m_onClick;

	void Init();

	virtual void OnFocusAcquired();
	virtual void OnFocusLost();
	virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();

	void SetDefaultColorScheme();
	void SetMouseOverColorScheme();

public:
	Button(Window* pWindow, const SDL_Rect& location);
	Button(Control* pParent, const SDL_Rect& location);

	void RegisterForClickCallback(const ButtonClickCallback& callback);
};

#endif // BUTTON_HPP
