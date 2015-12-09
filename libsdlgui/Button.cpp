#include "stdafx.h"
#include "Button.hpp"

Button::Button(Window* pWindow, const SDL_Rect& location) :
	Label(pWindow, location)
{
	SetDefaultColorScheme();
	SetAlignment(TextAlignment::MiddleCenter);
	SetText("button");
}

void Button::OnFocusAcquired()
{
	SetBorderColor(SDLColor(64, 64, 128, 0));
}

void Button::OnFocusLost()
{
	SetBackgroundColor(SDLColor(64, 64, 64, 0));
	SetBorderColor(SDLColor(128, 128, 128, 0));
}

bool Button::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
	bool takeFocus = false;
	if (buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT)
	{
		takeFocus = true;
		SetBackgroundColor(SDLColor(32, 32, 64, 0));
	}
	else if (buttonEvent.state == SDL_RELEASED && buttonEvent.button == SDL_BUTTON_LEFT)
	{
		SetMouseOverColorScheme();
		if (m_onClick != nullptr)
			m_onClick();
	}

	return takeFocus;
}

void Button::OnMouseEnter()
{
	SetMouseOverColorScheme();
}

void Button::OnMouseExit()
{
	SetDefaultColorScheme();
}

void Button::RegisterForClickCallback(const ButtonClickCallback& callback)
{
	m_onClick = callback;
}

void Button::SetDefaultColorScheme()
{
	SetBackgroundColor(SDLColor(64, 64, 64, 0));
	if (!HasFocus())
		SetBorderColor(SDLColor(128, 128, 128, 0));
	SetBorderSize(2);
}

void Button::SetMouseOverColorScheme()
{
	SetBackgroundColor(SDLColor(64, 64, 128, 0));
	SetBorderColor(SDLColor(128, 128, 255, 0));
	SetBorderSize(1);
}
