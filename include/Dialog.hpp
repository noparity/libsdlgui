#ifndef DIALOG_HPP
#define DIALOG_HPP

#include "Control.hpp"
#include "Dimentions.hpp"
#include "Panel.hpp"
#include "SdlHelpers.hpp"

// represents a dialog control
class Dialog : public Control
{
private:
    static const int TitleBarHeight = 32;
    static const int CloseButtonWidth = 48;

    SDLTexture m_titleTexture;
    Panel m_panel;
    bool m_canDrag;

    SDL_Rect GetCloseButtonLoc() const;
    SDL_Rect GetTitleBarLoc() const;

    virtual bool CanDragImpl() const;
    virtual void OnHiddenChanged(bool isHidden);
    virtual void OnLeftClick(const SDL_Point& clickLoc);
    virtual void OnLocationChanged(int deltaX, int deltaY);
    virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
    virtual void RenderImpl();

protected:
    // returns the location where content should be placed
    SDL_Rect GetContentLoc() const;

public:
    Dialog(Window* pWindow, const std::string& title, const Dimentions& dimentions);

    // adds a control to the dialog
    void AddControl(Control* pControl);

    // places the dialog in the center of its window
    void CenterDialog();
};

#endif // DIALOG_HPP
