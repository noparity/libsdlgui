#ifndef LABEL_HPP
#define LABEL_HPP

#include "Control.hpp"
#include "Font.hpp"
#include "SdlHelpers.hpp"
#include "TextAlignment.hpp"

class Label : public Control
{
private:
	std::string m_text;
	SDLTexture m_texture;
	Font* m_pFont;
	TextAlignment m_alignment;

	virtual void RenderImpl();

public:
	Label(Window* pWindow, const SDL_Rect& location);

	// sets the text alignment for the label
	void SetAlignment(TextAlignment alignment);

	// sets the font to use for the text
	void SetFont(Font* pFont);

	// sets the text to display
	void SetText(const std::string& text);
};

#endif // LABEL_HPP
