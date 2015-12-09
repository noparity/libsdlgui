#include "stdafx.h"
#include "Caret.hpp"

Caret::Caret(Window* pWindow, const SDL_Rect& location) :
	m_pause(false), Control(pWindow, location)
{
	// caret is hidden until its containing control has focus
	SetHidden(true);
}

void Caret::OnElapsedTime()
{
	if (!m_pause)
		SetHidden(!GetHidden());
}

void Caret::PauseAnimation()
{
	m_pause = true;
	SetHidden(false);
}

void Caret::RenderImpl()
{
	auto loc = GetLocation();
	if (loc.w == 1)
		GetWindow()->DrawLine(SDLPoint(loc.x, loc.y), SDLPoint(loc.x, loc.y + loc.h - 1), GetForegroundColor());
	else
		GetWindow()->DrawRectangle(loc, GetForegroundColor(), UINT8_MAX);
}

void Caret::ResumeAnimation()
{
	m_pause = false;
}

void Caret::StartAnimation()
{
	SetHidden(false);
	GetWindow()->RegisterForElapsedTimeNotification(this, 1000);
}

void Caret::StopAnimation()
{
	GetWindow()->UnregisterForElapsedTimeNotification(this);
	SetHidden(true);
}
