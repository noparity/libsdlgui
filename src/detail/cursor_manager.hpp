#ifndef CURSORMANAGER_HPP
#define CURSORMANAGER_HPP

#include <SDL_image.h>
#include "singleton.hpp"
#include <vector>

namespace libsdlgui::detail
{
    class CursorManagerType
    {
    private:
        friend void Singleton<CursorManagerType>::Initialize();
        friend void Singleton<CursorManagerType>::Destroy();

        std::vector<SDL_Cursor*> m_sysCursors;

        CursorManagerType();
        ~CursorManagerType();

    public:
        SDL_Cursor* GetSystemCursor(SDL_SystemCursor systemCursor);
    };

    using CursorManager = Singleton<CursorManagerType>;

} // namespace libsdlgui::detail

#endif // CURSORMANAGER_HPP
