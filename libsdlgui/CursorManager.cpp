#include "stdafx.h"
#include "CursorManager.hpp"

CursorManager* CursorManager::s_instance = nullptr;

CursorManager::CursorManager()
{
	m_sysCursors.resize(SDL_NUM_SYSTEM_CURSORS);
}

CursorManager::~CursorManager()
{
	for (auto cursor : m_sysCursors)
		SDL_FreeCursor(cursor);
}

void CursorManager::Destroy()
{
	delete s_instance;
}

SDL_Cursor* CursorManager::GetSystemCursor(SDL_SystemCursor systemCursor)
{
	if (m_sysCursors[systemCursor] == nullptr)
		m_sysCursors[systemCursor] = SDL_CreateSystemCursor(systemCursor);

	return m_sysCursors[systemCursor];
}

void CursorManager::Initialize()
{
	if (s_instance == nullptr)
		s_instance = new CursorManager();
}
