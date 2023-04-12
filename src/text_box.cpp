#include "stdafx.h"
#include "cursor_manager.hpp"
#include "text_box.hpp"
#include "window.hpp"

namespace libsdlgui
{
    TextBox::TextBox(Window* pWindow, const SDL_Rect& location) :
        Control(pWindow, location),
        m_caret(pWindow, SDLRect(location.x + TextOffsetX, location.y + 8, CaretWidth, location.h - 16)),
        m_pPrevCursor(nullptr),
        m_position(0),
        m_clipOffset(0)
    {
        SetBorderColor(SDLColor(128, 128, 128, 0));
        SetBorderSize(1);

        // place the caret above the text box
        detail::SetZOrder(&m_caret, detail::GetZOrder(this) + 1);
    }

    void TextBox::KeydownBackspace()
    {
        if (m_position == 0)
            return;

        auto charWidth = static_cast<int>(detail::GetFont(GetWindow())->GetCharSize(m_text[m_position - 1]));
        assert(charWidth > 0);

        if (m_position == m_text.size())
            m_text.pop_back();
        else
            m_text.erase(m_position - 1, 1);

        m_texture = detail::CreateTextureForText(GetWindow(), m_text, detail::GetFont(GetWindow()), GetForegroundColor(), GetBackgroundColor());
        --m_position;

        // don't move the caret if the string is bigger than the text box
        if (m_texture.GetWidth() < GetLocation().w - (TextOffsetX * 2) + CaretWidth)
        {
            MoveCaret(-charWidth);
        }
        else if (m_clipOffset > 0)
        {
            if (m_clipOffset < charWidth)
            {
                // if there is a single character being clipped
                // move the caret by the number of visible pixels.
                MoveCaret(-(charWidth - m_clipOffset));
                m_clipOffset = 0;
            }
            else
            {
                m_clipOffset -= charWidth;
            }
        }
    }

    void TextBox::KeydownDelete()
    {
        if (m_position < m_text.size())
        {
            m_text.erase(m_position, 1);
            m_texture = detail::CreateTextureForText(GetWindow(), m_text, detail::GetFont(GetWindow()), GetForegroundColor(), GetBackgroundColor());
        }
    }

    void TextBox::KeydownLeft()
    {
        if (m_position > 0)
        {
            --m_position;
            auto charWidth = static_cast<int>(detail::GetFont(GetWindow())->GetCharSize(m_text[m_position]));
            assert(charWidth > 0);
            MoveCaret(-charWidth);
        }
    }

    void TextBox::KeydownRight()
    {
        if (m_position < m_text.size())
        {
            auto charWidth = detail::GetFont(GetWindow())->GetCharSize(m_text[m_position]);
            ++m_position;
            MoveCaret(charWidth);
        }
    }

    void TextBox::MoveCaret(int offset)
    {
        auto caretLoc = m_caret.GetLocation();
        caretLoc.x += offset;
        assert(caretLoc.x >= 0);

        // ensure the caret stays within bounds
        // the delta is the number of pixels past the boundary
        // of the text box.  if it's less than offset it means
        // there is part of a char being clipped, so instead of
        // adjusting m_clipOffset by the full offset adjust it
        // by the number of pixels being clipped.

        auto location = GetLocation();
        if (caretLoc.x > (location.x + location.w) - (TextOffsetX + (CaretWidth - 1)))
        {
            // caret tried to move past the right bound, adjust the offset based on the delta
            offset = caretLoc.x - ((location.x + location.w) - (TextOffsetX + (CaretWidth - 1)));
            caretLoc.x = (location.x + location.w) - (TextOffsetX + (CaretWidth - 1));

            m_clipOffset += offset;
        }
        else if (caretLoc.x < location.x + TextOffsetX)
        {
            // caret tried to move past the left bound, adjust the offset based on the delta
            offset = caretLoc.x - (location.x + TextOffsetX);
            caretLoc.x = location.x + TextOffsetX;

            if (m_clipOffset > 0)
                m_clipOffset += offset;
        }

        assert(m_clipOffset >= 0);
        m_caret.SetLocation(caretLoc);
    }

    void TextBox::OnFocusAcquired()
    {
        assert(SDL_IsTextInputActive() == SDL_FALSE);
        m_caret.StartAnimation();
        SetBorderColor(SDLColor(64, 64, 128, 0));
        auto loc = GetLocation();
        SDL_SetTextInputRect(&loc);
        SDL_StartTextInput();
    }

    void TextBox::OnFocusLost()
    {
        m_caret.StopAnimation();
        SetBorderColor(SDLColor(128, 128, 128, 0));
        SDL_StopTextInput();
    }

    void TextBox::OnKeyboard(const SDL_KeyboardEvent& keyboardEvent)
    {
        if (keyboardEvent.state == SDL_PRESSED)
        {
            m_caret.PauseAnimation();

            switch (keyboardEvent.keysym.sym)
            {
            case SDLK_BACKSPACE:
                KeydownBackspace();
                break;
            case SDLK_DELETE:
                KeydownDelete();
                break;
            case SDLK_LEFT:
                KeydownLeft();
                break;
            case SDLK_RIGHT:
                KeydownRight();
                break;
            }
        }
        else if (keyboardEvent.state == SDL_RELEASED)
        {
            m_caret.ResumeAnimation();
        }
    }

    bool TextBox::OnMouseButton(const SDL_MouseButtonEvent& buttonEvent)
    {
        if (!LeftMouseButtonDown(buttonEvent))
            return false;

        if (m_text.size() > 0)
        {
            auto loc = GetLocation();
            auto caretLoc = m_caret.GetLocation();

            if (buttonEvent.x < m_texture.GetWidth() + loc.x + TextOffsetX && buttonEvent.x != caretLoc.x)
            {
                // move the caret to the location that was clicked

                auto pos = m_position;

                // direction of travel when iterating though m_text.
                // -1 means the click location was to the left of the caret.
                int direction = -1;
                if (caretLoc.x < buttonEvent.x)
                    direction = 1;

                // if a character is clicked track the x value on each side.  the
                // side closest to the clicked location is where the caret will go.
                int leftX = caretLoc.x;
                int rightX = leftX;

                while (true)
                {
                    if (direction < 0 && leftX < buttonEvent.x)
                        break;
                    else if (direction > 0 && rightX > buttonEvent.x)
                        break;
                    else if (pos > m_text.size())
                        break;

                    auto charWidth = detail::GetFont(GetWindow())->GetCharSize(m_text[pos]);
                    assert(charWidth > 0);
                    pos += direction;

                    if (direction < 0)
                    {
                        if (static_cast<int>(leftX - charWidth) < buttonEvent.x)
                            rightX = leftX;

                        leftX += (charWidth * direction);
                    }
                    else
                    {
                        if (static_cast<int>(rightX + charWidth) > buttonEvent.x)
                            leftX = rightX;

                        rightX += (charWidth * direction);
                    }
                }

                // check which is closest to the click location
                if (rightX - buttonEvent.x < buttonEvent.x - leftX)
                {
                    caretLoc.x = rightX;
                    m_position = pos;
                    if (direction < 0)
                        ++m_position;
                }
                else
                {
                    caretLoc.x = leftX;
                    m_position = pos;
                    if (direction > 0)
                        --m_position;
                }

                assert(m_position <= m_text.size());
                assert(caretLoc.x >= loc.x && caretLoc.x <= loc.x + loc.w);
                m_caret.SetLocation(caretLoc);
            }
            else if (buttonEvent.x > m_texture.GetWidth() && m_position < m_text.size())
            {
                // move the caret to the end of the text
                caretLoc.x = loc.x + TextOffsetX + m_texture.GetWidth();
                m_caret.SetLocation(caretLoc);
                m_position = m_text.size();
            }
        }

        return true;
    }

    void TextBox::OnMouseEnter()
    {
        // only switch once
        if (m_pPrevCursor == nullptr)
        {
            // preserve old cursor then switch to I-beam
            m_pPrevCursor = SDL_GetCursor();
            SDL_SetCursor(detail::CursorManager::GetInstance()->GetSystemCursor(SDL_SYSTEM_CURSOR_IBEAM));
        }
    }

    void TextBox::OnMouseExit()
    {
        // restore previous cursor
        SDL_SetCursor(m_pPrevCursor);
        m_pPrevCursor = nullptr;
    }

    void TextBox::OnTextInput(const SDL_TextInputEvent& textEvent)
    {
        GetWindow()->SetCursorHidden(true);
        if (m_position == m_text.size())
            m_text.append(textEvent.text);
        else
            m_text.insert(m_position, textEvent.text);

        auto charWidth = detail::GetFont(GetWindow())->GetTextSize(textEvent.text);
        MoveCaret(charWidth);

        ++m_position;
        m_texture = detail::CreateTextureForText(GetWindow(), m_text, detail::GetFont(GetWindow()), GetForegroundColor(), GetBackgroundColor());
    }

    void TextBox::OnZOrderChanged()
    {
        // our z-order changed, keep the caret above us
        detail::SetZOrder(&m_caret, detail::GetZOrder(this) + 1);
    }

    void TextBox::RenderImpl()
    {
        // create a buffer around the text
        auto location = GetLocation();
        location.x += TextOffsetX;

        // vertically center the text in the bounding rect
        location.y += (location.h - m_texture.GetHeight()) / 2;

        if (m_texture.GetHeight() <= location.h)
            location.h = m_texture.GetHeight();
        else
            assert(!"Font size is too tall for text box.");

        // if the text width is greater than the width of the text box the
        // text width will need to be clipped with respect to the position

        location.w -= (TextOffsetX * 2);
        SDL_Rect clip;

        clip.x = m_clipOffset;
        clip.y = 0;
        clip.h = location.h;
        clip.w = GetLocation().w - (TextOffsetX * 2);

        // if the portion of the texture being displayed doesn't completely fill
        // the location's width decrease the width so the texture isn't stretched

        auto deltaWidth = (clip.w - (m_texture.GetWidth() - m_clipOffset));
        if (deltaWidth > 0)
            location.w -= deltaWidth;

        GetWindow()->DrawRectangle(GetLocation(), GetBackgroundColor(), UINT8_MAX);
        GetWindow()->DrawTexture(location, m_texture, &clip);
    }

} // namespace libsdlgui
