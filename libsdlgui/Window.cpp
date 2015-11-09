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

Dimentions Window::DrawText(const DrawTextInfo& drawTextInfo)
{
	{
		SDLColorHolder colorHolder(m_renderer, drawTextInfo.BackgroundColor);
		SDL_RenderFillRect(m_renderer, &drawTextInfo.Location);
	}

	// if there is no text then bail
	if (drawTextInfo.Text.length() == 0)
		return Dimentions(0, 0);

	auto currentStyle = static_cast<Font::Attributes>(TTF_GetFontStyle(drawTextInfo.Font->GetTtf()));
	if (currentStyle != drawTextInfo.Font->GetAttributes())
		TTF_SetFontStyle(drawTextInfo.Font->GetTtf(), static_cast<int>(drawTextInfo.Font->GetAttributes()));

	auto textSurface = SDLSurface(TTF_RenderText_Shaded(drawTextInfo.Font->GetTtf(), drawTextInfo.Text.c_str(),
		drawTextInfo.ForegroundColor, drawTextInfo.BackgroundColor));

	auto textMessage = SDLTexture(SDL_CreateTextureFromSurface(m_renderer, textSurface));

	// there are three possibilities
	//   the size of location is equal to that of textSurface
	//   the size of location is larger than that of textSurface
	//   the size of location is smaller than that of textSurface

	int xOffset = 0;
	int yOffset = 0;

	// calculate xOffset
	switch (drawTextInfo.Alignment)
	{
	case TextAlignment::BottomCenter:
	case TextAlignment::MiddleCenter:
	case TextAlignment::TopCenter:
		xOffset = (drawTextInfo.Location.w - textSurface->w) / 2;
		break;

	case TextAlignment::BottomRight:
	case TextAlignment::MiddleRight:
	case TextAlignment::TopRight:
		xOffset = drawTextInfo.Location.w - textSurface->w;
		break;
	}

	// calculate yOffset
	switch (drawTextInfo.Alignment)
	{
	case TextAlignment::BottomCenter:
	case TextAlignment::BottomLeft:
	case TextAlignment::BottomRight:
		yOffset = drawTextInfo.Location.h - textSurface->h;
		break;

	case TextAlignment::MiddleCenter:
	case TextAlignment::MiddleLeft:
	case TextAlignment::MiddleRight:
		yOffset = (drawTextInfo.Location.h - textSurface->h) / 2;
		break;
	}

	if (textSurface->w == drawTextInfo.Location.w && textSurface->h == drawTextInfo.Location.h)
	{
		SDL_RenderCopy(m_renderer, textMessage, nullptr, &drawTextInfo.Location);
	}
	else if (textSurface->w < drawTextInfo.Location.w && textSurface->h < drawTextInfo.Location.h)
	{
		SDL_Rect myLoc = { drawTextInfo.Location.x + xOffset, drawTextInfo.Location.y + yOffset, textSurface->w, textSurface->h };
		SDL_RenderCopy(m_renderer, textMessage, nullptr, &myLoc);
	}
	else
	{
		// at least one dimension of the size of location is smaller than
		// textSurface so we need to compute the appropriate clipping rect.

		SDL_Rect myLoc = drawTextInfo.Location;
		SDL_Rect clip = { 0, 0 };

		if (textSurface->w > drawTextInfo.Location.w)
		{
			if (drawTextInfo.Anchor == Anchor::Left)
			{
				// clip off the right side of the text
				clip.w = drawTextInfo.Location.w;
			}
			else if (drawTextInfo.Anchor == Anchor::Right)
			{
				// clip off the left side of the text
				clip.w = drawTextInfo.Location.w;
				clip.x = textSurface->w - drawTextInfo.Location.w;
			}
			else
			{
				assert(!"unhandled anchor type");
			}
		}
		else
		{
			clip.w = textSurface->w;
			myLoc.x += xOffset;
			myLoc.w = textSurface->w;
		}

		if (textSurface->h > drawTextInfo.Location.h)
		{
			clip.h = drawTextInfo.Location.h;
		}
		else
		{
			clip.h = textSurface->h;
			myLoc.y += yOffset;
			myLoc.h = textSurface->h;
		}

		SDL_RenderCopy(m_renderer, textMessage, &clip, &myLoc);
	}

	return Dimentions(textSurface->w, textSurface->h);
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
		control->Invalidate();
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

bool Window::Render()
{
	bool didRender = false;
	for (auto control : m_controls)
		didRender |= control->Render();

	return didRender;
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
		if (m_pCtrlWithFocus != nullptr)
			m_pCtrlWithFocus->NotificationEvent(sdlEvent);
		break;
	}

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
	if (ShouldRender() && Render())
		SDL_RenderPresent(m_renderer);
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
			break;
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
