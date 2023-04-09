#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "control.hpp"
#include "sdl_helpers.hpp"

namespace libsdlgui
{
    // represents a button control
    class Button : public Control
    {
    public:
        using ButtonClickCallback = std::function<void()>;

    private:
        SDLTexture m_texture;
        ButtonClickCallback m_onClick;

        virtual void OnFocusAcquired();
        virtual void OnFocusLost();
        virtual void OnLeftClick(const SDL_Point&);
        virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
        virtual void OnMouseEnter();
        virtual void OnMouseExit();
        virtual void RenderImpl();

        void SetDefaultColorScheme();
        void SetMouseOverColorScheme();

    public:
        Button(Window* pWindow, const SDL_Rect& location);

        // registers a callback to be invoked when the button is clicked
        void RegisterForClickCallback(const ButtonClickCallback& callback);

        // sets the texture to be rendered on the button
        void SetTexture(SDLTexture& texture);

        // sets the texture to be rendered on the button
        void SetTexture(SDLTexture&& texture);
    };

} // namespace libsdlgui

#endif // BUTTON_HPP
