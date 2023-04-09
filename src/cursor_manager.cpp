#include "stdafx.h"
#include "cursor_manager.hpp"

namespace libsdlgui::detail
{
    CursorManager::CountType CursorManager::s_count = 0;
    CursorManager::UnderlyingType CursorManager::s_instance = nullptr;

    CursorManagerType::CursorManagerType()
    {
        m_sysCursors.resize(SDL_NUM_SYSTEM_CURSORS);
    }

    CursorManagerType::~CursorManagerType()
    {
        for (auto cursor : m_sysCursors)
            SDL_FreeCursor(cursor);

        m_sysCursors.clear();
    }

    SDL_Cursor* CursorManagerType::GetSystemCursor(SDL_SystemCursor systemCursor)
    {
        if (m_sysCursors[systemCursor] == nullptr)
            m_sysCursors[systemCursor] = SDL_CreateSystemCursor(systemCursor);

        return m_sysCursors[systemCursor];
    }

} // namespace libsdlgui::detail
