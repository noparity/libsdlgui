#include "stdafx.h"
#include "dialog.hpp"
#include "drawing_routines.hpp"
#include "window.hpp"

namespace libsdlgui
{
    Dialog::Dialog(Window* pWindow, const std::string& title, const Dimentions& dimentions) :
        Control(pWindow, SDLRect(0, 0, dimentions.W, dimentions.H + TitleBarHeight)),
        m_panel(pWindow, SDLRect(0, TitleBarHeight, dimentions.W, dimentions.H)),
        m_canDrag(false)
    {
        m_titleTexture = pWindow->CreateTextureForText(title, pWindow->GetFont(), SDLColor(0, 0, 0, 0), SDLColor(255, 255, 255, 0));
        SetBackgroundColor(SDLColor(128, 128, 128, 0));
        SetBorderColor(SDLColor(255, 255, 255, 0));
        SetBorderSize(1);
        SetHidden(true);
        SetZOrder(128);
        m_panel.SetZOrder(GetZOrder() + 1);
    }

    void Dialog::AddControl(Control* pControl)
    {
        m_panel.AddControl(pControl);
    }

    bool Dialog::CanDragImpl() const
    {
        return m_canDrag;
    }

    void Dialog::CenterDialog()
    {
        auto windowDims = GetWindow()->GetDimentions();
        auto myLoc = GetLocation();

        myLoc.x = (windowDims.W / 2) - (myLoc.w / 2);
        myLoc.y = (windowDims.H / 2) - (myLoc.h / 2);
        SetLocation(myLoc);
    }

    SDL_Rect Dialog::GetCloseButtonLoc() const
    {
        auto dialogLoc = GetLocation();

        SDL_Rect closeButtonLoc;
        closeButtonLoc.x = (dialogLoc.x + dialogLoc.w) - CloseButtonWidth;
        closeButtonLoc.y = dialogLoc.y;
        closeButtonLoc.w = CloseButtonWidth;
        closeButtonLoc.h = TitleBarHeight - 1;
        return closeButtonLoc;
    }

    SDL_Rect Dialog::GetContentLoc() const
    {
        return m_panel.GetLocation();
    }

    SDL_Rect Dialog::GetTitleBarLoc() const
    {
        SDL_Rect titleBarLoc = GetLocation();
        titleBarLoc.h = TitleBarHeight;
        return titleBarLoc;
    }

    void Dialog::OnHiddenChanged(bool isHidden)
    {
        m_panel.SetHidden(isHidden);
    }

    void Dialog::OnLeftClick(const SDL_Point& clickLoc)
    {
        if (SDLPointInRect(clickLoc, GetCloseButtonLoc()))
            SetHidden(true);
    }

    void Dialog::OnLocationChanged(int deltaX, int deltaY)
    {
        auto panelLoc = m_panel.GetLocation();
        panelLoc.x += deltaX;
        panelLoc.y += deltaY;
        m_panel.SetLocation(panelLoc);
    }

    bool Dialog::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
    {
        bool takeFocus = false;
        auto clickLoc = SDLPoint(buttonEvent.x, buttonEvent.y);
        auto closeLoc = GetCloseButtonLoc();

        if (buttonEvent.state == SDL_PRESSED && buttonEvent.button == SDL_BUTTON_LEFT)
        {
            takeFocus = true;
            auto titleLoc = GetTitleBarLoc();
            if (SDLPointInRect(clickLoc, titleLoc) && !SDLPointInRect(clickLoc, closeLoc))
                m_canDrag = true;
        }
        else if (buttonEvent.state == SDL_RELEASED)
        {
            m_canDrag = false;
        }

        return takeFocus;
    }

    void Dialog::RenderImpl()
    {
        auto window = GetWindow();

        // draw the background
        auto dialogLoc = GetLocation();
        window->DrawRectangle(dialogLoc, GetBackgroundColor(), UINT8_MAX);

        // draw the title bar
        window->DrawRectangle(GetTitleBarLoc(), SDLColor(255, 255, 255, 0), UINT8_MAX);
        window->DrawText(GetTitleBarLoc(), m_titleTexture, TextAlignment::MiddleCenter);

        // draw the close button
        window->DrawRectangle(GetCloseButtonLoc(), SDLColor(255, 0, 0, 0), UINT8_MAX);
        detail::DrawX(GetWindow(), GetCloseButtonLoc(), SDLColor(0, 0, 0, 0));
    }

} // namespace libsdlgui
