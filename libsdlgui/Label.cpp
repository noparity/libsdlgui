#include "stdafx.h"
#include "Label.hpp"

Label::Label(Window* pWindow, const SDL_Rect& location) :
	Control(pWindow, location), m_text("label")
{
	// default font is inherited from the window
	m_pFont = pWindow->GetFont();
	assert(m_pFont != nullptr);
	m_texture = pWindow->CreateTextureForText(m_text, m_pFont, GetForegroundColor(), GetBackgroundColor());
}

void Label::RenderImpl()
{
	GetWindow()->DrawRectangle(GetLocation(), GetBackgroundColor(), UINT8_MAX);
	GetWindow()->DrawText(GetLocation(), m_texture, m_alignment);
}

void Label::SetAlignment(TextAlignment alignment)
{
	m_alignment = alignment;
}

void Label::SetFont(Font* pFont)
{
	if (pFont != m_pFont)
	{
		m_pFont = pFont;
		m_texture = GetWindow()->CreateTextureForText(m_text, m_pFont, GetForegroundColor(), GetBackgroundColor());
	}
}

void Label::SetText(const std::string& text)
{
	if (text != m_text)
	{
		m_text = text;
		m_texture = GetWindow()->CreateTextureForText(m_text, m_pFont, GetForegroundColor(), GetBackgroundColor());
	}
}
