#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "Caret.hpp"
#include "Control.hpp"
#include "SdlHelpers.hpp"

// represents a text box control
class TextBox : public Control
{
private:
    static const int TextOffsetX = 4;
    static const int CaretWidth = 1;
    Caret m_caret;
    SDL_Cursor* m_pPrevCursor;
    std::string m_text;
    SDLTexture m_texture;
    size_t m_position;
    int m_clipOffset;

    void KeydownBackspace();
    void KeydownDelete();
    void KeydownLeft();
    void KeydownRight();
    void MoveCaret(int offset);
    virtual void OnFocusAcquired();
    virtual void OnFocusLost();
    virtual void OnKeyboard(const SDL_KeyboardEvent& keyboardEvent);
    virtual void OnLeftClick(const SDL_Point& clickLoc);
    virtual bool OnMouseButton(const SDL_MouseButtonEvent& buttonEvent);
    virtual void OnMouseEnter();
    virtual void OnMouseExit();
    virtual void OnTextInput(const SDL_TextInputEvent& textEvent);
    virtual void RenderImpl();

public:
    TextBox(Window* pWindow, const SDL_Rect& location);

    // gets the text that was entered
    std::string GetText() const { return m_text; }
};

#endif // TEXTBOX_HPP
