#ifndef CHECKBOX_HPP
#define CHECKBOX_HPP

#include "Control.hpp"
#include "Label.hpp"

// Represents a check box control.
class CheckBox : public Control
{
public:
    using CheckedChangedCallback = std::function<void(bool)>;

private:
    Label m_text;
    CheckedChangedCallback m_callback;
    bool m_checked;

    virtual void OnLeftClick(const SDL_Point&);
    virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
    virtual void RenderImpl();

public:
    CheckBox(Window* pWindow, const SDL_Rect& location);

    // Returns the checked state of the control.
    bool Checked() const { return m_checked; }

    // Registers a callback to be invoked when the checked state changes.
    // The callback parameter contains the checked state of the control.
    void RegisterForCheckedChangedCallback(const CheckedChangedCallback& callback);

    // Returns a pointer to the control containing the text.
    Label* Text() { return &m_text; }
};

#endif // CHECKBOX_HPP
