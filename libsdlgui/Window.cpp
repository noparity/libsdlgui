#include "stdafx.h"
#include "Control.hpp"
#include "CursorManager.hpp"
#include "Exceptions.hpp"
#include "FontManager.hpp"
#include "Window.hpp"
#include <algorithm>
#include <memory>

std::vector<Window*> Window::s_windows;

Window::Window(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags) :
	m_flags(State::None), m_dims(dimentions), m_pCtrlWithFocus(nullptr), m_subSystem(SDLSubSystem::Video), m_pFont(nullptr)
{
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		m_dims.W, m_dims.H, windowFlags);

	if (m_window == nullptr)
		throw SDLException("SDL_CreateWindow failed with error '" + SDLGetError() + "'.");

	m_id = SDL_GetWindowID(m_window);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if (m_renderer == nullptr)
		throw SDLException("SDL_CreateRenderer failed with error '" + SDLGetError() + "'.");

	if ((windowFlags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED)
		m_flags |= State::Minimized;
	else if ((windowFlags & SDL_WINDOW_HIDDEN) == SDL_WINDOW_HIDDEN)
		m_flags |= State::Closed;

	// set default colors
	m_bColor = SDLColor(0, 0, 0, 0);
	m_fColor = SDLColor(255, 255, 255, 0);

	// set default draw color
	SDL_SetRenderDrawColor(m_renderer, m_bColor.r, m_bColor.g, m_bColor.b, m_bColor.a);

	CursorManager::Initialize();
	FontManager::Initialize();

	SDL_StopTextInput();
	m_pFont = FontManager::GetInstance()->GetOrLoadFont("consola", 16);

	RegisterWindow(this);
}

Window::~Window()
{
	UnregisterWindow(this);
	CursorManager::Destroy();
	FontManager::Destroy();
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
}

void Window::AddControl(Control* pControl)
{
	assert(std::find(m_controls.begin(), m_controls.end(), pControl) == m_controls.end());
	m_controls.push_back(pControl);
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
	// must preserve previous draw color
	SDLColorHolder colorHolder(m_renderer, color);

	if (thickness == UINT8_MAX)
	{
		SDL_RenderFillRect(m_renderer, &location);
	}
	else
	{
		if (thickness == 1)
		{
			SDL_RenderDrawRect(m_renderer, &location);
		}
		else
		{
			// create an array of nested rectangles
			auto rects = std::make_unique<SDL_Rect[]>(thickness);
			for (uint8_t i = 0; i < thickness && (i < location.w / 2) && (i < location.h / 2); ++i)
			{
				rects[i].x = location.x + i;
				rects[i].y = location.y + i;
				rects[i].w = location.w - i * 2;
				rects[i].h = location.h - i * 2;
			}

			SDL_RenderDrawRects(m_renderer, rects.get(), thickness);
		}
	}
}

void Window::DrawText(const SDL_Rect& location, const SDLTexture& texture, TextAlignment alignment, Anchor anchor)
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
			if (anchor == Anchor::Left)
			{
				// clip off the right side of the text
				clip.w = location.w;
			}
			else if (anchor == Anchor::Right)
			{
				// clip off the left side of the text
				clip.w = location.w;
				clip.x = texture.GetWidth() - location.w;
			}
			else
			{
				assert(!"unhandled anchor type");
			}
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

void Window::OnMouseButton(SDL_MouseButtonEvent buttonEvent)
{
	for (auto control : m_controls)
	{
		if (control->GetHidden() == true)
			continue;

		// find the control that was clicked on then dispatch the event
		auto clickLoc = SDLRect(buttonEvent.x, buttonEvent.y, 1, 1);
		auto controlLoc = control->GetLocation();
		SDL_Rect result;
		if (SDL_IntersectRect(&clickLoc, &controlLoc, &result) == SDL_TRUE)
		{
			if (control->NotificationMouseButton(buttonEvent))
			{
				// remove focus from the previous control and give it to the selected one
				if (m_pCtrlWithFocus != nullptr)
					m_pCtrlWithFocus->SetFocus(false);

				m_pCtrlWithFocus = control;
				m_pCtrlWithFocus->SetFocus(true);
			}

			break;
		}
	}
}

void Window::OnMouseMotion(SDL_MouseMotionEvent motionEvent)
{
	if ((m_flags & State::EventsDisabled) == State::EventsDisabled)
		return;

	if (GetCursorHidden())
	{
		SDL_ShowCursor(SDL_ENABLE);
		SetCursorHidden(false);
	}
	// TODO: might make more sense for the Window class to track
	// the controls the mouse enters and leaves and just set a bit
	// on a control indicating either state.

	for (auto control : m_controls)
	{
		if (!control->GetHidden())
			control->NotificationMouseMotion(motionEvent);
	}
}

void Window::OnWindowResized(SDL_WindowEvent windowEvent)
{
	// resizing the window creates a new rendering context
	// which requires the rendering surface to be cleared.
	SDL_RenderClear(m_renderer);

	// update dimentions
	m_dims.W = windowEvent.data1;
	m_dims.H = windowEvent.data2;

	// notify all controls of the change then invalidate them
	for (auto control : m_controls)
	{
		control->NotificationWindowChanged(this);
	}
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

	// only render if the window is visible and only call present if any drawing was performed
	if (ShouldRender())
	{
		for (auto control : m_controls)
			control->Render();

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

void Window::Show()
{
	SDL_ShowWindow(m_window);
}

bool Window::ShouldRender()
{
	return ((m_flags & State::Minimized) != State::Minimized) &&
		((m_flags & State::Closed) != State::Closed);
}

void Window::TranslateEvent(const SDL_Event& sdlEvent)
{
	switch (sdlEvent.type)
	{
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		OnMouseButton(sdlEvent.button);
		break;
	case SDL_MOUSEMOTION:
		OnMouseMotion(sdlEvent.motion);
		break;
	case SDL_WINDOWEVENT:
		switch (sdlEvent.window.event)
		{
		case SDL_WINDOWEVENT_CLOSE:
			m_flags |= State::Closed;
			SDL_HideWindow(m_window);
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			m_flags |= State::Minimized;
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
		case SDL_WINDOWEVENT_RESTORED:
			m_flags ^= State::Minimized;
			break;
		case SDL_WINDOWEVENT_SHOWN:
			m_flags ^= State::Closed;
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			OnWindowResized(sdlEvent.window);
			break;
		}
		break;
	case SDL_QUIT:
		m_flags |= State::Closed;
		break;
	default:
		if (m_pCtrlWithFocus != nullptr && !m_pCtrlWithFocus->GetHidden())
			m_pCtrlWithFocus->NotificationEvent(sdlEvent);
		break;
	}
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

// static functions

// returns true if at least one window is active
bool Window::ActiveWindows()
{
	bool found = false;
	for (const auto window : s_windows)
	{
		if (window->IsActive())
		{
			found = true;
			window->Render();
		}
	}
	return found;
}

void Window::DispatchEvents()
{
	SDL_Event sdlEvent;
	if (SDL_PollEvent(&sdlEvent))
	{
		if (sdlEvent.window.windowID > 0)
			s_windows[sdlEvent.window.windowID - 1]->TranslateEvent(sdlEvent);
	}
}

void Window::RegisterWindow(Window* window)
{
	// window IDs are one-based, convert to zero-base to use as the index
	auto windowIndex = window->GetId() - 1;
	if (windowIndex >= s_windows.size())
	{
		s_windows.push_back(window);
	}
	else
	{
		assert(s_windows[windowIndex] == nullptr);
		s_windows[windowIndex] = window;
	}
}

void Window::UnregisterWindow(Window* window)
{
	s_windows[window->GetId() - 1] = nullptr;
}
