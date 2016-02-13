#ifndef TESTAPP_HPP
#define TESTAPP_HPP

#include "Button.hpp"
#include "CheckBox.hpp"
#include "Dialog.hpp"
#include "DropdownBox.hpp"
#include "Label.hpp"
#include "Panel.hpp"
#include "TextBox.hpp"
#include "Window.hpp"

class TestControl : public Panel
{
private:
    std::unique_ptr<Label> m_label1;
    std::unique_ptr<Label> m_label2;
    std::unique_ptr<Label> m_label3;
    std::unique_ptr<Label> m_label4;
    std::unique_ptr<Label> m_label5;

    virtual void RenderImpl();

public:
    TestControl(Window* pWindow, const SDL_Rect& location);
};

class TestDialog : public Dialog
{
private:
    Button m_buttonOK;
    Button m_buttonCancel;
    Label m_text;

public:
    TestDialog(Window* pWindow, const std::string& title, const Dimentions& dimentions);
};

class TestWindow : public Window
{
private:
    std::unique_ptr<TestControl> m_panel;
    std::unique_ptr<Label> m_label1;
    std::unique_ptr<Button> m_button1;
    std::unique_ptr<Button> m_button2;
    std::unique_ptr<Control> m_frame;
    std::unique_ptr<TextBox> m_textBox;
    std::unique_ptr<TestDialog> m_dialog;
    std::unique_ptr<DropdownBox> m_dropDown;
    std::unique_ptr<CheckBox> m_checkBox1;

    int m_frameNumber;

    void FrameOne();
    void FrameTwo();
    void FrameThree();
    void NextFrame();

public:
    TestWindow(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags);
};

#endif // TESTAPP_HPP
