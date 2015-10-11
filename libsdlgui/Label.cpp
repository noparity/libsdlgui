#include "stdafx.h"
#include "DrawTextInfo.hpp"
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
}

void Label::RenderImpl()
{
	DrawTextInfo dti;
	dti.Alignment = m_alignment;
	dti.BackgroundColor = GetBackgroundColor();
	dti.Font = m_pFont;
	dti.ForegroundColor = GetForegroundColor();
	dti.Location = GetLocation();
	dti.Text = m_text;

	GetWindow()->DrawText(dti);
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
		Invalidate();
	}

	return IsDirty();
}
