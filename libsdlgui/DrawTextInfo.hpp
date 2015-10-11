#ifndef DRAWTEXTINFO_HPP
#define DRAWTEXTINFO_HPP

#include "Anchor.hpp"
#include "Font.hpp"
#include <SDL_ttf.h>
#include <string>
#include "TextAlignment.hpp"

//const SDL_Rect& location, const std::string& text, const SDL_Color& fColor, const SDL_Color& bColor, int size, TextAlignment textAlignment
struct DrawTextInfo
{
	std::string Text;
	Font const* Font;
	SDL_Rect Location;
	SDL_Color ForegroundColor;
	SDL_Color BackgroundColor;
	TextAlignment Alignment;
	Anchor Anchor;
};

#endif // DRAWTEXTINFO_HPP
