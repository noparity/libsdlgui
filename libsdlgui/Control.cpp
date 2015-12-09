#include "stdafx.h"
#include "Control.hpp"
#include "SdlHelpers.hpp"

Control::Control(Window* pWindow, const SDL_Rect& location) :
	m_pWindow(pWindow), m_flags(State::None), m_loc(location), m_borderSize(0), m_zOrder(0)
{
	assert(m_pWindow != nullptr);
	m_borderColor = { 0, 0, 0, 0 };

	// inherit from window
	m_bColor = m_pWindow->GetBackgroundColor();
	m_fColor = m_pWindow->GetForegroundColor();

	m_pWindow->AddControl(this);
}

Control::~Control()
{
	m_pWindow->RemoveControl(this);
}

bool Control::CanDrag() const
{
	return CanDragImpl();
}

bool Control::CanDragImpl() const
{
	return false;
}

void Control::NotificationElapsedTime()
{
	OnElapsedTime();
}

void Control::NotificationEvent(const SDL_Event& event)
{
	OnSdlEvent(event);
}

void Control::NotificationFocusAcquired()
{
	m_flags |= State::Focused;
	OnFocusAcquired();
}

void Control::NotificationFocusLost()
{
	m_flags ^= State::Focused;
	OnFocusLost();
}

bool Control::NotificationMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
	return OnMouseButton(buttonEvent);
}

void Control::NotificationMouseEnter()
{
	OnMouseEnter();
}

void Control::NotificationMouseExit()
{
	OnMouseExit();
}

void Control::NotificationMouseMotion(const SDL_MouseMotionEvent& motionEvent)
{
	OnMouseMotion(motionEvent);
}

void Control::NotificationWindowChanged(Window* pWindow)
{
	OnWindowChanged(pWindow);
}

void Control::OnElapsedTime()
{
	// empty
}

void Control::OnSdlEvent(const SDL_Event&)
{
	// empty
}

void Control::OnFocusAcquired()
{
	// empty
}

void Control::OnFocusLost()
{
	// empty
}

bool Control::OnMouseButton(const SDL_MouseButtonEvent&)
{
	return false;
}

void Control::OnMouseEnter()
{
	// empty
}

void Control::OnMouseExit()
{
	// empty
}

void Control::OnMouseMotion(const SDL_MouseMotionEvent&)
{
	// empty
}

void Control::OnWindowChanged(Window*)
{
	// empty
}

void Control::Render()
{
	if (!GetHidden())
	{
		RenderImpl();

		// border drawn last so it overlays the control's content
		if (m_borderSize > 0)
			m_pWindow->DrawRectangle(GetLocation(), m_borderColor, m_borderSize);
	}
}

void Control::SetBackgroundColor(const SDL_Color& color)
{
	m_bColor = color;
}

void Control::SetBorderColor(const SDL_Color& color)
{
	m_borderColor = color;
}

void Control::SetBorderSize(uint8_t size)
{
	m_borderSize = size;
}

void Control::SetForegroundColor(const SDL_Color& color)
{
	m_fColor = color;
}

void Control::SetHidden(bool isHidden)
{
	bool changed = false;
	if (isHidden && (m_flags & State::Hidden) != State::Hidden)
	{
		m_flags |= State::Hidden;
		changed = true;
	}
	else if (!isHidden && (m_flags & State::Hidden) == State::Hidden)
	{
		m_flags ^= State::Hidden;
		changed = true;
	}
}

void Control::SetLocation(const SDL_Rect& location)
{
	m_loc = location;
}

void Control::SetZOrder(uint8_t zOrder)
{
	m_zOrder = zOrder;
}
