#ifndef LABEL_HPP
#define LABEL_HPP

#include "Control.hpp"
#include "Font.hpp"

class Label : public Control
{
private:
	std::string m_text;
	Font* m_pFont;
	TextAlignment m_alignment;

	void Init(Window* pWindow);

protected:
	virtual void RenderImpl();

public:
	Label(Window* pWindow, const SDL_Rect& location);
	Label(Control* pParent, const SDL_Rect& location);

	Font const* GetFont() const { return m_pFont; }
	bool SetAlignment(TextAlignment alignment);
	bool SetFont(Font* pFont);
	bool SetText(const std::string& text);
};

#endif // LABEL_HPP
