#ifndef CARET_HPP
#define CARET_HPP

#include "Control.hpp"

// represents a caret control
class Caret : public Control
{
private:
    bool m_pause;

    virtual void OnElapsedTime();
    virtual void RenderImpl();

public:
    Caret(Window* pWindow, const SDL_Rect& location);

    // pauses the caret animation
    void PauseAnimation();

    // resumes the caret animation
    void ResumeAnimation();

    // starts the caret animation
    void StartAnimation();

    // stops the caret animation
    void StopAnimation();
};

#endif // CARET_HPP
