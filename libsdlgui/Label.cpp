#include "stdafx.h"
#include "Anchor.hpp"
#include "Label.hpp"

Label::Label(Window* pWindow, const SDL_Rect& location) :
	Control(pWindow, location)
{
	Init(pWindow);
}

Label::Label(Control* pParent, const SDL_Rect& location) :
	Control(pParent, location)
{
	Init(GetWindow());
}

void Label::Init(Window* pWindow)
{
	// default font is inherited from the window
	m_pFont = pWindow->GetFont();
	assert(m_pFont != nullptr);
	m_text = "label";
	m_texture = pWindow->CreateTextureForText(m_text, m_pFont, GetForegroundColor(), GetBackgroundColor());
}

void Label::RenderImpl()
{
	GetWindow()->DrawRectangle(GetLocation(), GetBackgroundColor(), UINT8_MAX);
	GetWindow()->DrawText(GetLocation(), m_texture, m_alignment, Anchor::Right);
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
