#ifndef CURSORMANAGER_HPP
#define CURSORMANAGER_HPP

#include "SDL_mouse.h"

class CursorManager
{
private:
	static CursorManager* s_instance;
	std::vector<SDL_Cursor*> m_sysCursors;

public:
	CursorManager();
	~CursorManager();

	static void Destroy();

	SDL_Cursor* GetSystemCursor(SDL_SystemCursor systemCursor);

	static CursorManager* GetInstance()
	{
		assert(s_instance != nullptr);
		return s_instance;
	}
	static void Initialize();
};

#endif // CURSORMANAGER_HPP
