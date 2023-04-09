#ifndef PANEL_HPP
#define PANEL_HPP

#include "control.hpp"

namespace libsdlgui
{
    // represents a collection of controls whose
    // locations are relative to the panel
    class Panel : public Control
    {
    private:
        std::vector<Control*> m_controls;

        virtual void OnHiddenChanged(bool isHidden);
        virtual void OnLocationChanged(int deltaX, int deltaY);
        virtual void OnZOrderChanged();
        virtual void RenderImpl();

    public:
        Panel(Window* pWindow, const SDL_Rect& location);

        // adds a control to the panel.  the location of
        // the control must be within bounds of the panel
        void AddControl(Control* pControl);
    };

} // namespace libsdlgui

#endif // PANEL_HPP
