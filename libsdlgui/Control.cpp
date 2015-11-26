#include "stdafx.h"
#include "Control.hpp"
#include "SdlHelpers.hpp"

Control::Control(Window* pWindow, const SDL_Rect& location) :
	m_pWindow(pWindow), m_pParent(nullptr), m_flags(State::None), m_loc(location), m_borderSize(0), m_zOrder(0)
{
	assert(m_pWindow != nullptr);
	m_borderColor = { 0, 0, 0, 0 };

	// inherit from window
	m_bColor = m_pWindow->GetBackgroundColor();
	m_fColor = m_pWindow->GetForegroundColor();

	m_pWindow->AddControl(this);
}

Control::Control(Control* pParent, const SDL_Rect& location) :
	Control(pParent->m_pWindow, location)
{
	assert(m_pParent == nullptr);
	m_pParent = pParent;
	m_pParent->AddControl(this);
}

Control::~Control()
{
	if (m_pParent != nullptr)
		m_pParent->RemoveControl(this);

	m_pWindow->RemoveControl(this);
}

void Control::AddControl(Control* pControl)
{
	// a control's location is relative to the panel
	// so ensure that it's within its bounds.
	auto panelLoc = GetLocation();
	auto controlLoc = pControl->GetLocation();
	if (controlLoc.x + controlLoc.w > panelLoc.w ||
		controlLoc.y + controlLoc.h > panelLoc.h)
		throw std::runtime_error("Control is out of bounds.");

	controlLoc.x += panelLoc.x;
	controlLoc.y += panelLoc.y;
	pControl->SetLocation(controlLoc);
	m_controls.push_back(pControl);
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

void Control::OnWindowChanged(Window*)
{
	// empty
}

void Control::RemoveAllControls()
{
	m_controls.clear();
}

void Control::RemoveControl(Control* pControl)
{
	auto controlIter = std::find(m_controls.begin(), m_controls.end(), pControl);
	assert(controlIter != m_controls.end());
	m_controls.erase(controlIter);
}

void Control::Render()
{
	if (!GetHidden())
	{
		if (m_flags & State::HiddenPending)
		{
			// transition from hidden pending to hidden
			m_flags ^= State::HiddenPending;
			m_flags |= State::Hidden;
		}
		else
		{
			RenderImpl();

			// border drawn last so it overlays the control's content
			if (m_borderSize > 0)
				m_pWindow->DrawRectangle(GetLocation(), m_borderColor, m_borderSize);
		}
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
		// must set hidden pending instead of hidden so
		// that the control can be rendered hidden.
		m_flags |= State::HiddenPending;
		changed = true;
	}
	else if (!isHidden && (m_flags & State::Hidden) == State::Hidden)
	{
		m_flags ^= State::Hidden;
		changed = true;
	}

	if (changed)
	{
		for (auto control : m_controls)
			control->SetHidden(isHidden);
	}
}

void Control::SetLocation(const SDL_Rect& location)
{
	// TODO: validate that location is within bounds of parent?
	if (location != m_loc)
	{
		for (auto control : m_controls)
		{
			auto controlLoc = control->GetLocation();
			// calculate delta x and y for the panel
			int deltax = location.x - m_loc.x;
			int deltay = location.y - m_loc.y;
			// adjust by delta x and y
			controlLoc.x += deltax;
			controlLoc.y += deltay;
			control->SetLocation(controlLoc);
		}

		m_loc = location;
	}
}

void Control::SetZOrder(uint8_t zOrder)
{
	m_zOrder = zOrder;
}
