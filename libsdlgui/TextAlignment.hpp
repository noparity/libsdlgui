#ifndef TEXTALIGNMENT_HPP
#define TEXTALIGNMENT_HPP

#include "stdint.h"

// specifies how text should be aligned with respect to its container
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
