#include "stdafx.h"
#include "Dialog.hpp"

Dialog::Dialog(Window* pWindow, const std::string& title, const SDL_Rect& location) :
	Control(pWindow, location), m_title(title), m_canDrag(false)
{
	m_titleTexture = pWindow->CreateTextureForText(m_title, pWindow->GetFont(), SDLColor(0, 0, 0, 0), SDLColor(255, 255, 255, 0));
	SetBackgroundColor(SDLColor(128, 128, 128, 0));
	SetBorderColor(SDLColor(255, 255, 255, 0));
	SetBorderSize(1);
	SetHidden(true);
	SetZOrder(128);
}

bool Dialog::CanDragImpl() const
{
	return m_canDrag;
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

void Dialog::OnLeftClick(const SDL_Point& clickLoc)
{
	if (SDLPointInRect(clickLoc, GetCloseButtonLoc()))
		SetHidden(true);
}

bool Dialog::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
	bool takeFocus = false;
	auto clickLoc = SDLPoint(buttonEvent.x, buttonEvent.y);
	auto closeLoc = GetCloseButtonLoc();

	if (buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT)
	{
		takeFocus = true;
		auto titleLoc = GetTitleBarLoc();
		if (SDLPointInRect(clickLoc, titleLoc) && !SDLPointInRect(clickLoc, closeLoc))
			m_canDrag = true;
	}
	else if (buttonEvent.state == SDL_RELEASED)
	{
		m_canDrag = false;
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
