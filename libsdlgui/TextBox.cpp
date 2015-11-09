#include "stdafx.h"
#include "CursorManager.hpp"
#include "DrawTextInfo.hpp"
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

void TextBox::FocusAcquired()
{
	m_caret.StartAnimation();
	SetBorderColor(SDLColor(64, 64, 128, 0));
	auto loc = GetLocation();
	SDL_SetTextInputRect(&loc);
	SDL_StartTextInput();
}

void TextBox::FocusLost()
{
	m_caret.StopAnimation();
	SetBorderColor(SDLColor(128, 128, 128, 0));
	SDL_StopTextInput();
}

void TextBox::Init()
{
	m_pPrevCursor = nullptr;
	SetBorderColor(SDLColor(128, 128, 128, 0));
	SetBorderSize(1);
}

bool TextBox::NotificationMouseButton(SDL_MouseButtonEvent buttonEvent)
{
	bool takeFocus = false;
	if (buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT && SDL_IsTextInputActive() == SDL_FALSE)
		takeFocus = true;

	return takeFocus;
}

void TextBox::NotificationEvent(const SDL_Event& event)
{
	if (event.type == SDL_TEXTINPUT)
	{
		GetWindow()->SetCursorHidden(true);
		m_caret.PauseAnimation();
		m_text.append(event.text.text);
		Invalidate();
	}
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && m_text.length() > 0)
	{
		m_caret.PauseAnimation();
		m_text.pop_back();
		Invalidate();
	}
	else if (event.type == SDL_KEYUP)
	{
		m_caret.ResumeAnimation();
	}
}

void TextBox::NotificationMouseEnter()
{
	// only switch once
	if (m_pPrevCursor == nullptr)
	{
		// preserve old cursor then switch to I-beam
		m_pPrevCursor = SDL_GetCursor();
		SDL_SetCursor(CursorManager::GetInstance()->GetSystemCursor(SDL_SYSTEM_CURSOR_IBEAM));
	}
}

void TextBox::NotificationMouseExit()
{
	// restore previous cursor
	SDL_SetCursor(m_pPrevCursor);
	m_pPrevCursor = nullptr;
}

void TextBox::RenderImpl()
{
	DrawTextInfo dti;
	dti.Alignment = TextAlignment::MiddleLeft;
	dti.Anchor = Anchor::Right;
	dti.BackgroundColor = GetBackgroundColor();
	dti.Font = GetWindow()->GetFont();
	dti.ForegroundColor = GetForegroundColor();
	dti.Location = GetLocation();
	// create a buffer around the text
	dti.Location.x += TextOffsetX;
	dti.Location.w -= (TextOffsetX * 2);
	dti.Text = m_text;

	auto drawnDims = GetWindow()->DrawText(dti);
	// move the caret with respect to our location
	auto caretLoc = m_caret.GetLocation();
	caretLoc.x = GetLocation().x + drawnDims.W + TextOffsetX;
	m_caret.SetLocation(caretLoc);
}
