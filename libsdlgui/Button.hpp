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
	void SetDefaultColorScheme();
	void SetMouseOverColorScheme();

public:
	Button(Window* pWindow, const SDL_Rect& location);
	Button(Control* pParent, const SDL_Rect& location);

	virtual void FocusAcquired();
	virtual void FocusLost();
	virtual bool NotificationMouseButton(SDL_MouseButtonEvent buttonEvent);
	virtual void NotificationMouseMotion(SDL_MouseMotionEvent motionEvent);

	void RegisterForClickCallback(ButtonClickCallback callback);
};

#endif // BUTTON_HPP
