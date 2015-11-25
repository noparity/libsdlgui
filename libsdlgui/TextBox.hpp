#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "Caret.hpp"
#include "Control.hpp"
#include "SdlHelpers.hpp"
#include "SDL_mouse.h"

class TextBox : public Control
{
private:
	static const int TextOffsetX = 4;
	Caret m_caret;
	SDL_Cursor* m_pPrevCursor;
	std::string m_text;
	SDLTexture m_texture;

	void Init();

	virtual void OnFocusAcquired();
	virtual void OnFocusLost();
	virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnSdlEvent(const SDL_Event& event);
	virtual void RenderImpl();

public:
	TextBox(Window* pWindow, const SDL_Rect& location);
	TextBox(Control* pParent, const SDL_Rect& location);
};

#endif // TEXTBOX_HPP
