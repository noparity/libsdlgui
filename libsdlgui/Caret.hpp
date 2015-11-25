#ifndef CARET_HPP
#define CARET_HPP

#include "Control.hpp"

class Caret : public Control
{
private:
	bool m_pause;

	virtual void OnElapsedTime();
	virtual void RenderImpl();

public:
	Caret(Control* pParent, const SDL_Rect& location);

	void PauseAnimation();
	void ResumeAnimation();
	void StartAnimation();
	void StopAnimation();
};

#endif // CARET_HPP
