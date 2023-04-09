#ifndef TEXTALIGNMENT_HPP
#define TEXTALIGNMENT_HPP

namespace libsdlgui
{
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

} // namespace libsdlgui

#endif // TEXTALIGNMENT_HPP
