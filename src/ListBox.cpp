#include "stdafx.h"
#include "ListBox.hpp"

ListBox::ListBox(Window* pWindow, const SDL_Rect& location, uint32_t minVisible, uint32_t maxVisible,
    Control* parent, bool highlightOnMouseMotion, bool scrollRequiresFocus) :
    Control(pWindow, location, parent),
    m_selected(UINT32_MAX),
    m_highlighted(UINT32_MAX),
    m_minVisible(minVisible),
    m_maxVisible(maxVisible),
    m_visStart(0),
    m_vertScrollbar(pWindow, location, maxVisible, this),
    m_scrollRequiredFocus(scrollRequiresFocus),
    m_highlightOnMouseMotion(highlightOnMouseMotion)
{
    // set the height of the control based on the item height and min visible count
    m_itemHeight = GetWindow()->GetFont()->GetHeight();
    auto myLoc = GetLocation();
    myLoc.h = m_itemHeight * m_minVisible;
    SetLocation(myLoc);
    SetBorderColor(SDLColor(0, 128, 0, 0));
    SetBorderSize(1);

    // now fix up the vertical scrollbar's dimentions
    AdjustVertScrollBarHeight();
    m_vertScrollbar.SetZOrder(GetZOrder() + 1);
    m_vertScrollbar.RegisterForScrollCallback([this](VerticalScrollbar::Direction dir)
    {
        if (dir == VerticalScrollbar::Direction::TowardsBeginning && m_visStart > 0)
            --m_visStart;
        else if (dir == VerticalScrollbar::Direction::TowardsEnd && m_visStart < (m_items.size() - m_maxVisible))
            ++m_visStart;
    });

    // hide the scrollbar by default
    m_vertScrollbar.SetHidden(true);
}

void ListBox::AddItem(const std::string& item)
{
    m_items.push_back(item);
    auto default = GetWindow()->CreateTextureForText(item, GetWindow()->GetFont(), GetForegroundColor(), GetBackgroundColor());
    auto highlight = GetWindow()->CreateTextureForText(item, GetWindow()->GetFont(), GetBackgroundColor(), GetForegroundColor());

    assert(static_cast<uint32_t>(default.GetHeight()) == m_itemHeight);
    m_textures.push_back(std::tuple<SDLTexture, SDLTexture, bool>(std::move(default), std::move(highlight), false));
    m_vertScrollbar.Resize(m_items.size());

    // if the count of items is greater than the minimum
    // number to display we need to resize the control up
    // to the maximum number of items.
    if (m_items.size() > m_minVisible && m_items.size() <= m_maxVisible)
    {
        auto myLoc = GetLocation();
        myLoc.h = m_itemHeight * m_items.size();
        SetLocation(myLoc);
        AdjustVertScrollBarHeight();
    }

    // if the count of items is greater than the maximum
    // to display enable the vertical scrollbar
    if (m_items.size() > m_maxVisible && !GetHidden())
        m_vertScrollbar.SetHidden(false);
}

void ListBox::AdjustVertScrollBarHeight()
{
    // keep the height of the scroll bar in sync with the height of the list box
    auto myLoc = GetLocation();
    m_vertScrollbar.SetLocation(SDLRect(myLoc.x + myLoc.w - (VertScrollbarWidth + 1), myLoc.y + 1, VertScrollbarWidth, myLoc.h - 2));
}

void ListBox::ChangeSelectedItem(uint32_t oldItem, uint32_t newItem)
{
    assert(newItem < m_items.size());
    assert(oldItem != newItem);

    SetHighlighted(newItem);

    if (m_callback != nullptr)
        m_callback(m_items[newItem]);

    // adjust the visible start index as required
    if (m_selected >= m_visStart + m_maxVisible)
    {
        ++m_visStart;
        m_vertScrollbar.MoveSlider(VerticalScrollbar::Direction::TowardsEnd);
    }
    else if (m_selected < m_visStart)
    {
        --m_visStart;
        m_vertScrollbar.MoveSlider(VerticalScrollbar::Direction::TowardsBeginning);
    }
}

uint32_t ListBox::GetIndexForMouseLoc(const SDL_Point& mouseLoc)
{
    if (m_items.size() == 0)
        return UINT32_MAX;

    auto myLoc = GetLocation();

    // select the highlighted texture for the item the mouse is over
    uint32_t index = ((mouseLoc.y - myLoc.y) / m_itemHeight) + m_visStart;
    if (index >= m_items.size())
        return UINT32_MAX;

    return index;
}

uint32_t ListBox::GetVisCount() const
{
    // return the number of items that are visible
    // with respect to the min and max display bounds.

    auto visCount = m_items.size();
    if (visCount > m_maxVisible)
        visCount = m_maxVisible;
    else if (visCount < m_minVisible)
        visCount = m_minVisible;

    return visCount;
}

void ListBox::OnHiddenChanged(bool isHidden)
{
    // if the number of items is less than the max visible
    // items don't toggle the scroll bar as it isn't visible
    if (m_items.size() > m_maxVisible)
        m_vertScrollbar.SetHidden(isHidden);

    // fix up the highlighted entry.  this can happen when
    // m_highlightOnMouseMotion is true which can cause the
    // highlighted item to not necessarily be what's selected
    if (!isHidden && m_highlighted != m_selected)
        SetHighlighted(m_selected);
}

void ListBox::OnKeyboard(const SDL_KeyboardEvent& keyboardEvent)
{
    if (keyboardEvent.state == SDL_PRESSED)
    {
        switch (keyboardEvent.keysym.sym)
        {
        case SDLK_DOWN:
            if (m_selected == UINT32_MAX || m_selected < (m_items.size() - 1))
            {
                auto old = m_selected;
                ++m_selected;
                ChangeSelectedItem(old, m_selected);
            }
            break;
        case SDLK_UP:
            if (m_selected != UINT32_MAX && m_selected > 0)
            {
                auto old = m_selected;
                --m_selected;
                ChangeSelectedItem(old, m_selected);
            }
            break;
        }
    }
}

void ListBox::OnLeftClick(const SDL_Point& clickLoc)
{
    auto index = GetIndexForMouseLoc(clickLoc);
    if (index != UINT32_MAX && index != m_selected)
    {
        auto old = m_selected;
        m_selected = index;
        ChangeSelectedItem(old, m_selected);
    }
}

bool ListBox::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
{
    return LeftMouseButtonDown(buttonEvent);
}

void ListBox::OnMouseMotion(const SDL_MouseMotionEvent& motionEvent)
{
    if (m_highlightOnMouseMotion)
    {
        auto index = GetIndexForMouseLoc(SDLPoint(motionEvent.x, motionEvent.y));
        if (index == UINT32_MAX || index >= GetCountItems())
            return;

        if (index != m_highlighted)
            SetHighlighted(index);
    }
}

void ListBox::OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent)
{
    if (HasFocus() || !m_scrollRequiredFocus)
        m_vertScrollbar.NotificationMouseWheel(wheelEvent);
}

void ListBox::OnZOrderChanged()
{
    m_vertScrollbar.SetZOrder(GetZOrder() + 1);
}

void ListBox::RenderImpl()
{
    auto itemLoc = GetLocation();
    auto visCount = GetVisCount();

    // fill the background so nothing leaks through on lines
    // where the item texture doesn't fill the width of the control.
    GetWindow()->DrawRectangle(itemLoc, GetBackgroundColor(), UINT8_MAX);

    for (size_t i = m_visStart; i < m_visStart + visCount; ++i)
    {
        SDLTexture const* t;

        if (std::get<2>(m_textures[i]) == false)
            t = &std::get<0>(m_textures[i]);
        else
            t = &std::get<1>(m_textures[i]);

        itemLoc.h = t->GetHeight();

        // extend the highlighted appearance to the end of the control
        if (std::get<2>(m_textures[i]))
            GetWindow()->DrawRectangle(SDLRect(itemLoc.x, itemLoc.y, itemLoc.w, itemLoc.h), GetForegroundColor(), UINT8_MAX);

        GetWindow()->DrawText(itemLoc, *t, TextAlignment::MiddleLeft);

        itemLoc.y += m_itemHeight;
    }
}

void ListBox::RegisterForSelectionChangedCallback(const SelectionChangedCallback& callback)
{
    m_callback = callback;
}

void ListBox::SetHighlighted(uint32_t index)
{
    // unhighlight the current selection then highlight the new one
    if (m_highlighted != UINT32_MAX)
        std::get<2>(m_textures[m_highlighted]) = false;

    m_highlighted = index;
    std::get<2>(m_textures[m_highlighted]) = true;
}
