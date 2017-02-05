#ifndef LISTBOX_HPP
#define LISTBOX_HPP

#include "Control.hpp"
#include <string>
#include <vector>
#include "VerticalScrollbar.hpp"

// Represetns a list box control.
class ListBox : public Control
{
public:
    using SelectionChangedCallback = std::function<void(const std::string&)>;

private:
    static const uint32_t VertScrollbarWidth = 25;

    uint32_t m_selected;
    uint32_t m_highlighted;
    uint32_t m_minVisible;
    uint32_t m_maxVisible;
    uint32_t m_visStart;
    uint32_t m_itemHeight;
    VerticalScrollbar m_vertScrollbar;
    std::vector<std::string> m_items;
    std::vector<std::tuple<SDLTexture, SDLTexture, bool>> m_textures;
    SelectionChangedCallback m_callback;
    bool m_scrollRequiredFocus;
    bool m_highlightOnMouseMotion;

    uint32_t GetIndexForMouseLoc(const SDL_Point& mouseLoc);
    uint32_t GetVisCount() const;
    virtual void OnHiddenChanged(bool isHidden);
    virtual void OnKeyboard(const SDL_KeyboardEvent& keyboardEvent);
    virtual void OnLeftClick(const SDL_Point& clickLoc);
    virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
    virtual void OnMouseMotion(const SDL_MouseMotionEvent& motionEvent);
    virtual void OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent);
    virtual void OnResize(int deltaH, int);
    virtual void OnZOrderChanged();
    virtual void RenderImpl();
    void SelectedItemChanged();
    void SetHighlighted(uint32_t index);

public:
    ListBox(Window* pWindow, const SDL_Rect& location, uint32_t minVisible, uint32_t maxVisible,
        Control* parent = nullptr, bool highlightOnMouseMotion = false, bool scrollRequiresFocus = true);

    // Adds a new item to the drop-down box.
    // Items are displayed in the order they're added.
    void AddItem(const std::string& item);

    // returns the number of items in the list box
    size_t GetCountItems() const { return m_items.size(); }

    // returns the index of the item that's currently selected
    uint32_t GetSelectedIndex() const { return m_selected; }

    // Registers a callback to be invoked when the selected item changes.
    // The callback parameter contains the value of the selected item.
    void RegisterForSelectionChangedCallback(const SelectionChangedCallback& callback);
};

#endif // LISTBOX_HPP
