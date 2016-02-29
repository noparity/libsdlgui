#ifndef DRAWINGROUTINES_HPP
#define DRAWINGROUTINES_HPP

#include "Window.hpp"

// draws a chevron pointing up or down (^ or v) within the specified bounds
void DrawChevron(Window* pWindow, const SDL_Rect& bounds, const SDL_Color& color, bool pointsUp);

// draws an X within the specified bounds
void DrawX(Window* pWindow, const SDL_Rect& bounds, const SDL_Color& color);

#endif // DRAWINGROUTINES_HPP
