#include "stdafx.h"
#include "Control.hpp"
#include "CursorManager.hpp"
#include "Exceptions.hpp"
#include "FontManager.hpp"
#include "Window.hpp"
#include <algorithm>
#include <memory>

Window::Window(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags) :
	m_flags(State::None), m_dims(dimentions), m_pCtrlWithFocus(nullptr), m_pCtrlUnderMouse(nullptr), m_subSystem(SDLSubSystem::Video), m_pFont(nullptr)
{
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		m_dims.W, m_dims.H, windowFlags);

	if (m_window == nullptr)
		throw SDLException("SDL_CreateWindow failed with error '" + SDLGetError() + "'.");

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if (m_renderer == nullptr)
		throw SDLException("SDL_CreateRenderer failed with error '" + SDLGetError() + "'.");

	if ((windowFlags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED)
		m_flags |= State::Minimized;

	// set default colors
	m_bColor = SDLColor(0, 0, 0, 0);
	m_fColor = SDLColor(255, 255, 255, 0);

	// set default draw color
	SDL_SetRenderDrawColor(m_renderer, m_bColor.r, m_bColor.g, m_bColor.b, m_bColor.a);

	CursorManager::Initialize();
	FontManager::Initialize();

	SDL_StopTextInput();
	m_pFont = FontManager::GetInstance()->GetOrLoadFont("consola", 16);
}

Window::~Window()
{
	CursorManager::Destroy();
	FontManager::Destroy();
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
}

void Window::AddControl(Control* pControl)
{
	assert(std::find(m_controls.begin(), m_controls.end(), pControl) == m_controls.end());
	m_controls.push_back(pControl);

	// sort the list by descending z-order
	std::sort(m_controls.begin(), m_controls.end(), [](Control* lhs, Control* rhs)
	{
		return lhs->GetZOrder() > rhs->GetZOrder();
	});
}

SDLTexture Window::CreateTextureForText(const std::string& text, Font const* font, const SDL_Color& fgColor, const SDL_Color& bgColor)
{
	// if there is no text then bail
	if (text.length() == 0)
		return SDLTexture();

	auto currentStyle = static_cast<Font::Attributes>(TTF_GetFontStyle(font->GetTtf()));
	if (currentStyle != font->GetAttributes())
		TTF_SetFontStyle(font->GetTtf(), static_cast<int>(font->GetAttributes()));

	auto textSurface = SDLSurface(TTF_RenderText_Shaded(font->GetTtf(), text.c_str(),
		fgColor, bgColor));

	return SDLTexture(SDL_CreateTextureFromSurface(m_renderer, textSurface), textSurface->w, textSurface->h);
}

void Window::DrawLine(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color)
{
	SDLColorHolder colorHolder(m_renderer, color);
	SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
}

void Window::DrawRectangle(const SDL_Rect& location, const SDL_Color& color, uint8_t thickness)
{
	assert(thickness > 0);
	// must preserve previous draw color
	SDLColorHolder colorHolder(m_renderer, color);

	if (thickness == UINT8_MAX)
	{
		SDL_RenderFillRect(m_renderer, &location);
	}
	else
	{
		// create a copy of location so we can modify it
		auto myLoc = location;
		for (uint8_t i = 0; i < thickness; ++i)
		{
			myLoc.x = myLoc.x + i;
			myLoc.y = myLoc.y + i;
			myLoc.w = myLoc.w - i * 2;
			myLoc.h = myLoc.h - i * 2;

			SDL_RenderDrawRect(m_renderer, &myLoc);
		}
	}
}

void Window::DrawText(const SDL_Rect& location, const SDLTexture& texture, TextAlignment alignment)
{
	// there are three possibilities
	//   the size of location is equal to that of textSurface
	//   the size of location is larger than that of textSurface
	//   the size of location is smaller than that of textSurface

	int xOffset = 0;
	int yOffset = 0;

	// calculate xOffset
	switch (alignment)
	{
	case TextAlignment::BottomCenter:
	case TextAlignment::MiddleCenter:
	case TextAlignment::TopCenter:
		xOffset = (location.w - texture.GetWidth()) / 2;
		break;

	case TextAlignment::BottomRight:
	case TextAlignment::MiddleRight:
	case TextAlignment::TopRight:
		xOffset = location.w - texture.GetWidth();
		break;
	}

	// calculate yOffset
	switch (alignment)
	{
	case TextAlignment::BottomCenter:
	case TextAlignment::BottomLeft:
	case TextAlignment::BottomRight:
		yOffset = location.h - texture.GetHeight();
		break;

	case TextAlignment::MiddleCenter:
	case TextAlignment::MiddleLeft:
	case TextAlignment::MiddleRight:
		yOffset = (location.h - texture.GetHeight()) / 2;
		break;
	}

	if (texture.GetWidth() == location.w && texture.GetHeight() == location.h)
	{
		SDL_RenderCopy(m_renderer, texture, nullptr, &location);
	}
	else if (texture.GetWidth() < location.w && texture.GetHeight() < location.h)
	{
		SDL_Rect myLoc = { location.x + xOffset, location.y + yOffset, texture.GetWidth(), texture.GetHeight() };
		SDL_RenderCopy(m_renderer, texture, nullptr, &myLoc);
	}
	else
	{
		// at least one dimension of the size of location is smaller than
		// textSurface so we need to compute the appropriate clipping rect.

		SDL_Rect myLoc = location;
		SDL_Rect clip = { 0, 0 };

		if (texture.GetWidth() > location.w)
		{
			// clip off the right side of the text
			clip.w = location.w;
		}
		else
		{
			clip.w = texture.GetWidth();
			myLoc.x += xOffset;
			myLoc.w = texture.GetWidth();
		}

		if (texture.GetHeight() > location.h)
		{
			clip.h = location.h;
		}
		else
		{
			clip.h = texture.GetHeight();
			myLoc.y += yOffset;
			myLoc.h = texture.GetHeight();
		}

		SDL_RenderCopy(m_renderer, texture, &clip, &myLoc);
	}
}

void Window::DrawText(const SDL_Rect& location, const SDLTexture& texture, SDL_Rect const* clip)
{
	SDL_RenderCopy(m_renderer, texture, clip, &location);
}

void Window::OnKeyboard(const SDL_KeyboardEvent& keyboardEvent)
{
	if (m_pCtrlWithFocus != nullptr)
		m_pCtrlWithFocus->NotificationKeyboard(keyboardEvent);
}

void Window::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
	// if a control has focus and a click happens outside of that control
	// we need to send it a notification.  if the click happens on another
	// control pass the pointer to that control.
	bool notifyCtrl = true;
	Control* pClickedCtrl = nullptr;

	for (size_t i = 0; i < m_controls.size(); ++i)
	{
		if (m_occlusionMap[i] == 1 || m_controls[i]->GetHidden())
			continue;

		// find the control that was clicked on then dispatch the event
		auto clickPoint = SDLPoint(buttonEvent.x, buttonEvent.y);
		auto controlLoc = m_controls[i]->GetLocation();
		if (SDLPointInRect(clickPoint, controlLoc))
		{
			if (m_controls[i]->NotificationMouseButton(buttonEvent))
			{
				// remove focus from the previous control and give it to the selected one
				if (m_pCtrlWithFocus != nullptr && m_controls[i] != m_pCtrlWithFocus)
					m_pCtrlWithFocus->NotificationFocusLost();

				// if this control already has focus don't notify it again
				if (m_controls[i] != m_pCtrlWithFocus)
				{
					m_pCtrlWithFocus = m_controls[i];
					m_pCtrlWithFocus->NotificationFocusAcquired();
				}

				// a control that took focus was clicked, no need to notify
				// the previous control as it would have received a notification
				// that it lost focus.
				notifyCtrl = false;
			}

			// remember the control that was clicked but did not take focus
			pClickedCtrl = m_controls[i];

			break;
		}
	}

	if (m_pCtrlWithFocus != nullptr && notifyCtrl)
		m_pCtrlWithFocus->NotificationMouseButtonExternal(buttonEvent, pClickedCtrl);
}

void Window::OnMouseMotion(const SDL_MouseMotionEvent& motionEvent)
{
	if (GetCursorHidden())
	{
		SDL_ShowCursor(SDL_ENABLE);
		SetCursorHidden(false);
	}

	if (motionEvent.state == SDL_PRESSED && m_pCtrlWithFocus != nullptr && m_pCtrlWithFocus->CanDrag())
	{
		auto controlLoc = m_pCtrlWithFocus->GetLocation();
		controlLoc.x += motionEvent.xrel;
		controlLoc.y += motionEvent.yrel;
		m_pCtrlWithFocus->SetLocation(controlLoc);
	}
	
	bool isOverControl = false;
	// must iterate in descending z-order
	for (size_t i = 0; i < m_controls.size(); ++i)
	{
		if (m_occlusionMap[i] == 1 || m_controls[i]->GetHidden())
			continue;

		auto clickPoint = SDLPoint(motionEvent.x, motionEvent.y);
		auto controlLoc = m_controls[i]->GetLocation();
		if (SDLPointInRect(clickPoint, controlLoc))
		{
			// notify the previous control the mouse has left it
			if (m_pCtrlUnderMouse != nullptr && m_pCtrlUnderMouse != m_controls[i])
				m_pCtrlUnderMouse->NotificationMouseExit();

			// if this control is already under the mouse don't notify it again
			if (m_pCtrlUnderMouse != m_controls[i])
			{
				m_pCtrlUnderMouse = m_controls[i];
				m_pCtrlUnderMouse->NotificationMouseEnter();
			}

			m_pCtrlUnderMouse->NotificationMouseMotion(motionEvent);
			isOverControl = true;

			// mouse can't be over more than one control so exit
			break;
		}
	}

	// mouse is no longer over any control, if it was earlier
	// then notify that control that it has exited it.
	if (!isOverControl && m_pCtrlUnderMouse != nullptr)
	{
		m_pCtrlUnderMouse->NotificationMouseExit();
		m_pCtrlUnderMouse = nullptr;
	}
}

void Window::OnTextInput(const SDL_TextInputEvent& textEvent)
{
	// should have a control with focus (e.g. a text box)
	assert(m_pCtrlWithFocus != nullptr);
	m_pCtrlWithFocus->NotificationTextInput(textEvent);
}

void Window::OnWindowResized(const SDL_WindowEvent& windowEvent)
{
	// resizing the window creates a new rendering context
	// which requires the rendering surface to be cleared.
	SDL_RenderClear(m_renderer);

	// update dimentions
	m_dims.W = windowEvent.data1;
	m_dims.H = windowEvent.data2;

	// notify all controls of the change
	for (auto control : m_controls)
		control->NotificationWindowChanged();
}

void Window::RegisterForElapsedTimeNotification(Control* pControl, uint32_t ticks)
{
	m_ctrlsElapsedTime.push_back(ControlElapsedTime(pControl, ticks, SDL_GetTicks()));
}

void Window::RemoveAllControls()
{
	m_controls.clear();
	m_ctrlsElapsedTime.clear();
}

void Window::RemoveControl(Control* pControl)
{
	auto controlIter = std::find(m_controls.begin(), m_controls.end(), pControl);
	assert(controlIter != m_controls.end());
	if (controlIter != m_controls.end())
	{
		UnregisterForElapsedTimeNotification(*controlIter);
		m_controls.erase(controlIter);
	}
}

void Window::Render()
{
	// notify any controls for elapsed time
	for (auto& control : m_ctrlsElapsedTime)
	{
		auto timeRequested = std::get<1>(control);
		auto timeElapsed = std::get<2>(control);
		auto currentTime = SDL_GetTicks();
		if (currentTime - timeElapsed >= timeRequested)
		{
			std::get<2>(control) = currentTime;
			std::get<0>(control)->NotificationElapsedTime();
		}
	}

	// only render if the window is visible
	if (ShouldRender())
	{
		// resize occlusion map as required
		if (m_occlusionMap.size() != m_controls.size())
			m_occlusionMap.resize(m_controls.size());

		// calculate all of the z-order partitions.  each tuple
		// represents a range of descending z-orders [beginning, end).
		std::vector<std::tuple<size_t, size_t>> partitions;
		{
			size_t beginning = 0;
			uint8_t currentZ = 0;
			for (size_t i = beginning; i < m_controls.size(); ++i)
			{
				auto zOrder = m_controls[i]->GetZOrder();
				if (zOrder > currentZ)
				{
					currentZ = m_controls[i]->GetZOrder();
				}
				else if (zOrder < currentZ)
				{
					partitions.push_back(std::tuple<size_t, size_t>(beginning, i));
					beginning = i;
					currentZ = m_controls[i]->GetZOrder();
				}

				// if we hit zero no need to process any further
				if (currentZ == 0)
					break;
			}
			// don't add the last partition as it can't occlude anything
			//partitions.push_back(std::tuple<size_t, size_t>(beginning, m_controls.size()));
		}

		if (partitions.size() > 1)
		{
			// iterate over the partitions finding all occluded controls
			for (auto partition : partitions)
			{
				auto beginning = std::get<0>(partition);
				auto end = std::get<1>(partition);
				assert(beginning < end);
				
				// for each control in the partition check if it
				// occludes any controls in the lower partitions
				for (auto top = beginning; top < end; ++top)
				{
					// don't skip hidden controls, it prevents us from
					// properly clearing the occlusion bit when a control
					// was occluded but no longer is.
					//if (m_controls[top]->GetHidden())
					//	continue;
					
					// if this control is occluded then don't bother testing if it
					// occludes any controls; the control that occluded it would have
					// occluded any controls under this one.
					if (m_occlusionMap[top] == 1)
						continue;
					
					auto topControlLoc = m_controls[top]->GetLocation();
					for (auto bottom = end; bottom < m_controls.size(); ++bottom)
					{
						auto bottomControlLoc = m_controls[bottom]->GetLocation();

						// if the bottom control is fully occluded then set the bit
						// in the map else clear it.  this handles the case when the
						// control was occluded but now isn't.
						if (SDLRectOcclusion(topControlLoc, bottomControlLoc) && !m_controls[top]->GetHidden())
							m_occlusionMap[bottom] = 1;
						else
							m_occlusionMap[bottom] = 0;
					}
				}
			}
		}

		SDL_RenderClear(m_renderer);

		// render controls in ascending z-order
		for (size_t i = m_controls.size() - 1; i < m_controls.size(); --i)
		{
			if (m_occlusionMap[i] == 0)
				m_controls[i]->Render();
		}

		SDL_RenderPresent(m_renderer);
	}
}

void Window::SetCursorHidden(bool hidden)
{
	if (hidden && !GetCursorHidden())
	{
		SDL_ShowCursor(SDL_DISABLE);
		m_flags = State::CursorHidden;
	}
	else if (!hidden && GetCursorHidden())
	{
		SDL_ShowCursor(SDL_ENABLE);
		m_flags ^= State::CursorHidden;
	}
}

bool Window::ShouldRender()
{
	return ((m_flags & State::Minimized) != State::Minimized);
}

bool Window::TranslateEvent(const SDL_Event& sdlEvent)
{
	bool quit = false;

	switch (sdlEvent.type)
	{
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		OnKeyboard(sdlEvent.key);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		OnMouseButton(sdlEvent.button);
		break;
	case SDL_MOUSEMOTION:
		OnMouseMotion(sdlEvent.motion);
		break;
	case SDL_TEXTINPUT:
		OnTextInput(sdlEvent.text);
		break;
	case SDL_WINDOWEVENT:
		switch (sdlEvent.window.event)
		{
		case SDL_WINDOWEVENT_MINIMIZED:
			m_flags |= State::Minimized;
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
		case SDL_WINDOWEVENT_RESTORED:
			m_flags ^= State::Minimized;
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			OnWindowResized(sdlEvent.window);
			break;
		}
		break;
	case SDL_QUIT:
		quit = true;
		break;
	}

	return quit;
}

void Window::UnregisterForElapsedTimeNotification(Control* pControl)
{
	for (auto iter = m_ctrlsElapsedTime.begin(); iter != m_ctrlsElapsedTime.end(); ++iter)
	{
		if (std::get<0>(*iter) == pControl)
		{
			m_ctrlsElapsedTime.erase(iter);
			break;
		}
	}
}
