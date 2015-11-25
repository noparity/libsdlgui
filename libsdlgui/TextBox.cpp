#include "stdafx.h"
#include "CursorManager.hpp"
#include "TextBox.hpp"

TextBox::TextBox(Window* pWindow, const SDL_Rect& location) :
	m_caret(this, SDLRect(TextOffsetX, 8, 1, location.h - 16)), Control(pWindow, location)
{
	Init();
}

TextBox::TextBox(Control* pParent, const SDL_Rect& location) :
	m_caret(this, SDLRect(TextOffsetX, 8, 1, location.h - 16)), Control(pParent, location)
{
	Init();
}

void TextBox::Init()
{
	m_pPrevCursor = nullptr;
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

void TextBox::OnSdlEvent(const SDL_Event& event)
{
	bool createNewTexture = false;
	if (event.type == SDL_TEXTINPUT)
	{
		GetWindow()->SetCursorHidden(true);
		m_caret.PauseAnimation();
		m_text.append(event.text.text);
		createNewTexture = true;
	}
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && m_text.length() > 0)
	{
		m_caret.PauseAnimation();
		m_text.pop_back();
		createNewTexture = true;
	}
	else if (event.type == SDL_KEYUP)
	{
		m_caret.ResumeAnimation();
	}

	if (createNewTexture)
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
