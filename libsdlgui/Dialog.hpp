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

	SDL_Rect GetCloseButtonLoc() const;
	SDL_Rect GetTitleBarLoc() const;

protected:
	virtual void RenderImpl();

public:
	Dialog(Window* pWindow, const std::string& title, const SDL_Rect& location);

	virtual bool NotificationMouseButton(SDL_MouseButtonEvent buttonEvent);
};

#endif // DIALOG_HPP
