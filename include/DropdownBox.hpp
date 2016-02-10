#ifndef DROPDOWNBOX_HPP
#define DROPDOWNBOX_HPP

#include "Control.hpp"
#include "ListBox.hpp"
#include <memory>
#include "SdlHelpers.hpp"
#include <string>
#include <vector>

// Represents a drop-down box control.
class DropdownBox : public Control
{
public:
	using SelectionChangedCallback = std::function<void(const std::string&)>;

private:

	// Used to display the content of the drop-down box.
	class ContentBox : public ListBox
	{
	private:
		virtual bool OnMouseButton(const SDL_MouseButtonEvent&);

	public:
		ContentBox(Window* pWindow, const SDL_Rect& location);
	};

	ContentBox m_contentBox;
	SDLTexture m_texture;
	SelectionChangedCallback m_callback;

	virtual void OnFocusAcquired();
	virtual void OnFocusLost();
	virtual void OnLeftClick(const SDL_Point& clickLoc);
	virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
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
