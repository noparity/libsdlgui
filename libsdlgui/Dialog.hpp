#ifndef DIALOG_HPP
#define DIALOG_HPP

#include "SdlHelpers.hpp"
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include "Window.hpp"

class Dialog
{
public:
	Dialog(const std::string& title, Dimentions dimentions);
};

#endif // DIALOG_HPP
