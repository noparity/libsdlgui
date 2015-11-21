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

bool Label::SetAlignment(TextAlignment alignment)
{
	if (m_alignment != alignment)
	{
		m_alignment = alignment;
		Invalidate();
	}

	return IsDirty();
}

bool Label::SetFont(Font* pFont)
{
	if (m_pFont != pFont)
	{
		m_pFont = pFont;
		Invalidate();
	}

	return IsDirty();
}

bool Label::SetText(const std::string& text)
{
	if (text != m_text)
	{
		m_text = text;
		m_texture = GetWindow()->CreateTextureForText(m_text, m_pFont, GetForegroundColor(), GetBackgroundColor());
		Invalidate();
	}

	return IsDirty();
}
