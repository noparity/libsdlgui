#include "stdafx.h"
#include "ListBox.hpp"

ListBox::ListBox(Window* pWindow, const SDL_Rect& location) :
	Control(pWindow, location), m_highlighted(0), m_selected(UINT32_MAX)
{
	SetBorderColor(SDLColor(0, 128, 0, 0));
	SetBorderSize(1);
}

void ListBox::AddItem(const std::string& item)
{
	m_items.push_back(item);
	auto default = GetWindow()->CreateTextureForText(item, GetWindow()->GetFont(), GetForegroundColor(), GetBackgroundColor());
	auto highlight = GetWindow()->CreateTextureForText(item, GetWindow()->GetFont(), GetBackgroundColor(), GetForegroundColor());

	// update size
	auto myLoc = GetLocation();
	myLoc.h += default.GetHeight();
	SetLocation(myLoc);

	m_textures.push_back(std::tuple<SDLTexture, SDLTexture, bool>(std::move(default), std::move(highlight), false));
}

uint32_t ListBox::GetIndexForMouseLoc(const SDL_Point& mouseLoc)
{
	auto myLoc = GetLocation();

	// select the highlighted texture for the item the mouse is over
	uint32_t index = (mouseLoc.y - myLoc.y) / (myLoc.h / m_items.size());

	// if the mouse is over the very last line of the content rect
	// it will be equal to the number of items.  clamp it here.
	if (index == m_items.size())
		--index;

	return index;
}

void ListBox::OnHiddenChanged(bool hidden)
{
	// clear the highlighted bit on the highlighted item
	// and if an item is selected highlight that one.

	if (hidden && m_textures.size() > 0)
	{
		std::get<2>(m_textures[m_highlighted]) = false;

		if (m_selected != UINT32_MAX)
		{
			std::get<2>(m_textures[m_selected]) = true;
			m_highlighted = m_selected;
		}
	}
}

void ListBox::OnLeftClick(const SDL_Point& clickLoc)
{
	auto index = GetIndexForMouseLoc(clickLoc);
	SelectedItemChanged(index);
}

bool ListBox::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
	return LeftMouseButtonDown(buttonEvent);
}

void ListBox::OnMouseMotion(const SDL_MouseMotionEvent& motionEvent)
{
	// select the highlighted texture for the item the mouse is over
	auto index = GetIndexForMouseLoc(SDLPoint(motionEvent.x, motionEvent.y));

	if (index != m_highlighted)
	{
		std::get<2>(m_textures[m_highlighted]) = false;
		m_highlighted = index;
	}

	std::get<2>(m_textures[m_highlighted]) = true;
}

void ListBox::RenderImpl()
{
	auto itemLoc = GetLocation();

	for (const auto& texture : m_textures)
	{
		SDLTexture const* t;

		if (std::get<2>(texture) == false)
			t = &std::get<0>(texture);
		else
			t = &std::get<1>(texture);

		itemLoc.h = t->GetHeight();
		if (std::get<2>(texture))
			GetWindow()->DrawRectangle(itemLoc, GetForegroundColor(), UINT8_MAX);
		GetWindow()->DrawText(itemLoc, *t, TextAlignment::MiddleLeft);

		auto paddedHeight = t->GetHeight();
		itemLoc.y += paddedHeight;
	}
}

void ListBox::RegisterForSelectionChangedCallback(const SelectionChangedCallback& callback)
{
	m_callback = callback;
}

void ListBox::SelectedItemChanged(uint32_t item)
{
	assert(item < m_items.size());

	if (item != m_selected)
	{
		m_selected = item;

		if (m_callback != nullptr)
			m_callback(m_items[m_selected]);
	}
}
