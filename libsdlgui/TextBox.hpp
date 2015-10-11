#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "Caret.hpp"
#include "Control.hpp"
#include "SDL_mouse.h"

class TextBox : public Control
{
private:
	static const int TextOffsetX = 4;
	Caret m_caret;
	SDL_Cursor* m_pPrevCursor;
	std::string m_text;

	void Init();

protected:
	virtual void RenderImpl();

public:
	TextBox(Window* pWindow, const SDL_Rect& location);
	TextBox(Control* pParent, const SDL_Rect& location);

	virtual void FocusAcquired();
	virtual void FocusLost();

	virtual void NotificationEvent(const SDL_Event& event);
	virtual bool NotificationMouseButton(SDL_MouseButtonEvent buttonEvent);
	virtual void NotificationMouseMotion(SDL_MouseMotionEvent motionEvent);
};

#endif // TEXTBOX_HPP
