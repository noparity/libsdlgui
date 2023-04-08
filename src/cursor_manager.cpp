#include "stdafx.h"
#include "cursor_manager.hpp"

CursorManager::CountType CursorManager::s_count = 0;
CursorManager::UnderlyingType CursorManager::s_instance = nullptr;

namespace detail
{
    CursorManager::CursorManager()
    {
        m_sysCursors.resize(SDL_NUM_SYSTEM_CURSORS);
    }

    CursorManager::~CursorManager()
    {
        for (auto cursor : m_sysCursors)
            SDL_FreeCursor(cursor);

        m_sysCursors.clear();
    }

    SDL_Cursor* CursorManager::GetSystemCursor(SDL_SystemCursor systemCursor)
    {
        if (m_sysCursors[systemCursor] == nullptr)
            m_sysCursors[systemCursor] = SDL_CreateSystemCursor(systemCursor);

        return m_sysCursors[systemCursor];
    }
}
