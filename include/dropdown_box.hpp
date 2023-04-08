#ifndef DROPDOWNBOX_HPP
#define DROPDOWNBOX_HPP

#include "control.hpp"
#include "list_box.hpp"
#include "sdl_helpers.hpp"

// Represents a drop-down box control.
class DropdownBox : public Control
{
public:
    using SelectionChangedCallback = ListBox::SelectionChangedCallback;

private:
    static const uint32_t DropdownButtonWidth = 25;

    // a slightly specialized list box used to display the content of the drop-down box.
    class ContentBox : public ListBox
    {
    private:
        virtual bool OnMouseButton(const SDL_MouseButtonEvent&);

    public:
        ContentBox(Window* pWindow, const SDL_Rect& location, uint32_t minVisible, uint32_t maxVisible, Control* parent);
        bool IsSubControl(Control* pControl);
    };

    ContentBox m_content;
    SDLTexture m_texture;
    SelectionChangedCallback m_callback;

    virtual void OnFocusAcquired();
    virtual void OnFocusLost();
    virtual void OnKeyboard(const SDL_KeyboardEvent& keyboardEvent);
    virtual void OnLeftClick(const SDL_Point& clickLoc);
    virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
    virtual void OnMouseButtonExternal(const SDL_MouseButtonEvent& buttonEvent, Control* pControl);
    virtual void OnMouseWheel(const SDL_MouseWheelEvent& wheelEvent);
    virtual void RenderImpl();

public:
    DropdownBox(Window* pWindow, const SDL_Rect& location);

    // Adds a new item to the drop-down box.
    // Items are displayed in the order they're added.
    void AddItem(const std::string& item);

    // Registers a callback to be invoked when the selected item changes.
    // The callback parameter contains the value of the selected item.
    void RegisterForSelectionChangedCallback(const SelectionChangedCallback& callback);
};

#endif // DROPDOWNBOX_HPP
