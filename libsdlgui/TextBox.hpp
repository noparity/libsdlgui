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

	virtual void OnFocusAcquired();
	virtual void OnFocusLost();
	virtual void OnKeyboard(const SDL_KeyboardEvent& keyboardEvent);
	virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnTextInput(const SDL_TextInputEvent& textEvent);
	virtual void RenderImpl();

public:
	TextBox(Window* pWindow, const SDL_Rect& location);

	std::string GetText() const { return m_text; }
};

#endif // TEXTBOX_HPP
