#ifndef CURSORMANAGER_HPP
#define CURSORMANAGER_HPP

#include <SDL_image.h>
#include "Singleton.hpp"

namespace detail
{
    class CursorManager
    {
    private:
        friend void Singleton<CursorManager>::Initialize();
        friend void Singleton<CursorManager>::Destroy();

        std::vector<SDL_Cursor*> m_sysCursors;

        CursorManager();
        ~CursorManager();

    public:
        SDL_Cursor* GetSystemCursor(SDL_SystemCursor systemCursor);
    };
}

using CursorManager = Singleton<detail::CursorManager>;

#endif // CURSORMANAGER_HPP
