#include "stdafx.h"
#include "CursorManager.hpp"
#include "TextBox.hpp"

TextBox::TextBox(Window* pWindow, const SDL_Rect& location) :
	m_caret(pWindow, SDLRect(location.x + TextOffsetX, location.y + 8, 1, location.h - 16)), Control(pWindow, location), m_pPrevCursor(nullptr)
{
	SetBorderColor(SDLColor(128, 128, 128, 0));
	SetBorderSize(1);
	// place the caret above the text box
	m_caret.SetZOrder(GetZOrder() + 1);
}

void TextBox::OnFocusAcquired()
{
	m_caret.StartAnimation();
	SetBorderColor(SDLColor(64, 64, 128, 0));
	auto loc = GetLocation();
	SDL_SetTextInputRect(&loc);
	SDL_StartTextInput();
}

void TextBox::OnFocusLost()
{
	m_caret.StopAnimation();
	SetBorderColor(SDLColor(128, 128, 128, 0));
	SDL_StopTextInput();
}

void TextBox::OnKeyboard(const SDL_KeyboardEvent& keyboardEvent)
{
	if (keyboardEvent.state == SDL_PRESSED)
	{
		m_caret.PauseAnimation();

		if (keyboardEvent.keysym.sym == SDLK_BACKSPACE && m_text.length() > 0)
		{
			m_text.pop_back();
			m_texture = GetWindow()->CreateTextureForText(m_text, GetWindow()->GetFont(), GetForegroundColor(), GetBackgroundColor());
		}
	}
	else if (keyboardEvent.state == SDL_RELEASED)
	{
		m_caret.ResumeAnimation();
	}
}

bool TextBox::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
	bool takeFocus = false;
	if (buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT && SDL_IsTextInputActive() == SDL_FALSE)
		takeFocus = true;

	return takeFocus;
}

void TextBox::OnMouseEnter()
{
	// only switch once
	if (m_pPrevCursor == nullptr)
	{
		// preserve old cursor then switch to I-beam
		m_pPrevCursor = SDL_GetCursor();
		SDL_SetCursor(CursorManager::GetInstance()->GetSystemCursor(SDL_SYSTEM_CURSOR_IBEAM));
	}
}

void TextBox::OnMouseExit()
{
	// restore previous cursor
	SDL_SetCursor(m_pPrevCursor);
	m_pPrevCursor = nullptr;
}

void TextBox::OnTextInput(const SDL_TextInputEvent& textEvent)
{
	GetWindow()->SetCursorHidden(true);
	m_text.append(textEvent.text);
	m_texture = GetWindow()->CreateTextureForText(m_text, GetWindow()->GetFont(), GetForegroundColor(), GetBackgroundColor());
}

void TextBox::RenderImpl()
{
	// create a buffer around the text
	auto location = GetLocation();
	location.x += TextOffsetX;
	location.w -= (TextOffsetX * 2);
	
	GetWindow()->DrawRectangle(GetLocation(), GetBackgroundColor(), UINT8_MAX);
	GetWindow()->DrawText(location, m_texture, TextAlignment::MiddleLeft, Anchor::Right);

	// move the caret with respect to our location
	auto caretLoc = m_caret.GetLocation();
	caretLoc.x = GetLocation().x + m_texture.GetWidth() + TextOffsetX;
	// ensure the caret stays within bounds
	location = GetLocation();
	if (caretLoc.x > (location.x + location.w) - (TextOffsetX + (caretLoc.w - 1)))
		caretLoc.x = (location.x + location.w) - (TextOffsetX + (caretLoc.w - 1));
	m_caret.SetLocation(caretLoc);
}
