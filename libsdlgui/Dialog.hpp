#ifndef DIALOG_HPP
#define DIALOG_HPP

#include "Control.hpp"
#include "SdlHelpers.hpp"
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include "Window.hpp"

class Dialog : public Control
{
private:
	std::string m_title;
	SDLTexture m_titleTexture;
	bool m_canDrag;

	SDL_Rect GetCloseButtonLoc() const;
	SDL_Rect GetTitleBarLoc() const;

	virtual bool CanDragImpl() const;
	virtual void OnLeftClick(const SDL_Point& clickLoc);
	virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
	virtual void RenderImpl();

public:
	Dialog(Window* pWindow, const std::string& title, const SDL_Rect& location);
};

#endif // DIALOG_HPP
