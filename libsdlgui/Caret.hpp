#ifndef CARET_HPP
#define CARET_HPP

#include "Control.hpp"

class Caret : public Control
{
private:
	uint16_t m_offset;
	bool m_pause;

protected:
	void RenderImpl();

public:
	Caret(Control* pParent, const SDL_Rect& location);

	virtual void NotificationElapsedTime();
	void PauseAnimation();
	void ResumeAnimation();
	virtual bool SetLocation(const SDL_Rect& location);
	void StartAnimation();
	void StopAnimation();
};

#endif // CARET_HPP
