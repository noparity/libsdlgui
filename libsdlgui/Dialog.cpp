#include "stdafx.h"
#include "Dialog.hpp"

Dialog::Dialog(Window* pWindow, const std::string& title, const SDL_Rect& location) :
	Control(pWindow, location), m_title(title)
{
	m_titleTexture = pWindow->CreateTextureForText(m_title, pWindow->GetFont(), SDLColor(0, 0, 0, 0), SDLColor(255, 255, 255, 0));
	SetBackgroundColor(SDLColor(128, 128, 128, 0));
	SetBorderColor(SDLColor(255, 255, 255, 0));
	SetBorderSize(1);
	SetHidden(true);
	SetZOrder(128);
}

SDL_Rect Dialog::GetCloseButtonLoc() const
{
	auto dialogLoc = GetLocation();

	SDL_Rect closeButtonLoc;
	closeButtonLoc.x = (dialogLoc.x + dialogLoc.w) - 48;
	closeButtonLoc.y = dialogLoc.y;
	closeButtonLoc.w = 48;
	closeButtonLoc.h = 31;
	return closeButtonLoc;
}

SDL_Rect Dialog::GetTitleBarLoc() const
{
	SDL_Rect titleBarLoc = GetLocation();
	titleBarLoc.h = 32;
	return titleBarLoc;
}

bool Dialog::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
	bool takeFocus = false;
	if (buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT)
	{
		takeFocus = true;
	}
	else if (buttonEvent.state == SDL_RELEASED && buttonEvent.button == SDL_BUTTON_LEFT)
	{
		auto clickLoc = SDLRect(buttonEvent.x, buttonEvent.y, 1, 1);
		auto closeLoc = GetCloseButtonLoc();
		SDL_Rect result;
		if (SDL_IntersectRect(&clickLoc, &closeLoc, &result) == SDL_TRUE)
			SetHidden(true);
	}

	return takeFocus;
}

void Dialog::RenderImpl()
{
	auto window = GetWindow();
	
	// draw the background
	auto dialogLoc = GetLocation();
	window->DrawRectangle(dialogLoc, GetBackgroundColor(), UINT8_MAX);
	
	// draw the title bar
	window->DrawRectangle(GetTitleBarLoc(), SDLColor(255, 255, 255, 0), UINT8_MAX);
	window->DrawText(GetTitleBarLoc(), m_titleTexture, TextAlignment::MiddleCenter, Anchor::Left);
	
	// draw the close button
	window->DrawRectangle(GetCloseButtonLoc(), SDLColor(255, 0, 0, 0), UINT8_MAX);
}
