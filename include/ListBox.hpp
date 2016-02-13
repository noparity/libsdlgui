#ifndef LISTBOX_HPP
#define LISTBOX_HPP

#include "Control.hpp"
#include <string>
#include <vector>

// Represetns a list box control.
class ListBox : public Control
{
public:
    using SelectionChangedCallback = std::function<void(const std::string&)>;

private:
    std::vector<std::string> m_items;
    std::vector<std::tuple<SDLTexture, SDLTexture, bool>> m_textures;
    uint32_t m_highlighted;
    uint32_t m_selected;
    SelectionChangedCallback m_callback;

    uint32_t GetIndexForMouseLoc(const SDL_Point& mouseLoc);
    virtual void OnHiddenChanged(bool hidden);
    virtual void OnLeftClick(const SDL_Point& clickLoc);
    virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
    virtual void OnMouseMotion(const SDL_MouseMotionEvent& motionEvent);
    virtual void RenderImpl();
    void SelectedItemChanged(uint32_t item);

public:
    ListBox(Window* pWindow, const SDL_Rect& location);

    // Adds a new item to the drop-down box.
    // Items are displayed in the order they're added.
    void AddItem(const std::string& item);

    // Registers a callback to be invoked when the selected item changes.
    // The callback parameter contains the value of the selected item.
    void RegisterForSelectionChangedCallback(const SelectionChangedCallback& callback);
};

#endif // LISTBOX_HPP
