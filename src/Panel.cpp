#include "stdafx.h"
#include "panel.hpp"
#include "sdl_helpers.hpp"

namespace libsdlgui
{
    Panel::Panel(Window* pWindow, const SDL_Rect& location) :
        Control(pWindow, location)
    {
        // empty
    }

    void Panel::AddControl(Control* pControl)
    {
        // ensure the control is within the bounds of the panel
        if (!SDLRectOcclusion(GetLocation(), pControl->GetLocation()))
            throw new std::runtime_error("control is not within bounds of the panel");

        m_controls.push_back(pControl);

        // place the control just above the panel
        detail::SetZOrder(pControl, detail::GetZOrder(this) + 1);

        // sort the controls in ascending z-order
        std::sort(m_controls.begin(), m_controls.end(), [](Control* lhs, Control* rhs)
            {
                return detail::GetZOrder(lhs) < detail::GetZOrder(rhs);
            });

        // if the panel is hidden then hide the control too
        if (GetHidden())
            pControl->SetHidden(true);
    }

    void Panel::OnHiddenChanged(bool isHidden)
    {
        for (auto& control : m_controls)
            control->SetHidden(isHidden);
    }

    void Panel::OnLocationChanged(int deltaX, int deltaY)
    {
        for (auto& control : m_controls)
        {
            auto controlLoc = control->GetLocation();
            controlLoc.x += deltaX;
            controlLoc.y += deltaY;
            control->SetLocation(controlLoc);
        }
    }

    void Panel::OnZOrderChanged()
    {
        for (auto& control : m_controls)
            detail::SetZOrder(control, detail::GetZOrder(this) + 1);
    }

    void Panel::RenderImpl()
    {
        // empty
    }

} // namespace libsdlgui
