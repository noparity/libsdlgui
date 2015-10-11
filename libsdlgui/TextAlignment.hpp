#ifndef TEXTALIGNMENT_HPP
#define TEXTALIGNMENT_HPP

#include "stdint.h"

enum class TextAlignment : uint8_t
{
	TopLeft,
	TopCenter,
	TopRight,
	MiddleLeft,
	MiddleCenter,
	MiddleRight,
	BottomLeft,
	BottomCenter,
	BottomRight
};

#endif // TEXTALIGNMENT_HPP
