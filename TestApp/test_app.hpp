#ifndef TESTAPP_HPP
#define TESTAPP_HPP

#include "button.hpp"
#include "check_box.hpp"
#include "dialog.hpp"
#include "dropdown_box.hpp"
#include "label.hpp"
#include "list_box.hpp"
#include "panel.hpp"
#include "text_box.hpp"
#include "window.hpp"

class TestControl : public libsdlgui::Panel
{
private:
    std::unique_ptr<libsdlgui::Label> m_label1;
    std::unique_ptr<libsdlgui::Label> m_label2;
    std::unique_ptr<libsdlgui::Label> m_label3;
    std::unique_ptr<libsdlgui::Label> m_label4;
    std::unique_ptr<libsdlgui::Label> m_label5;

    virtual void RenderImpl();

public:
    TestControl(libsdlgui::Window* pWindow, const SDL_Rect& location);
};

class TestDialog : public libsdlgui::Dialog
{
private:
    libsdlgui::Button m_buttonOK;
    libsdlgui::Button m_buttonCancel;
    libsdlgui::Label m_text;

public:
    TestDialog(libsdlgui::Window* pWindow, const std::string& title, const libsdlgui::Dimentions& dimentions);
};

class TestWindow : public libsdlgui::Window
{
private:
    std::unique_ptr<TestControl> m_panel;
    std::unique_ptr<libsdlgui::Label> m_label1;
    std::unique_ptr<libsdlgui::Button> m_button1;
    std::unique_ptr<libsdlgui::Button> m_button2;
    std::unique_ptr<libsdlgui::Control> m_frame;
    std::unique_ptr<libsdlgui::TextBox> m_textBox;
    std::unique_ptr<TestDialog> m_dialog;
    std::unique_ptr<libsdlgui::DropdownBox> m_dropDown;
    std::unique_ptr<libsdlgui::CheckBox> m_checkBox1;
    std::unique_ptr<libsdlgui::ListBox> m_listBox;

    int m_frameNumber;

    void FrameOne();
    void FrameTwo();
    void FrameThree();
    void NextFrame();

public:
    TestWindow(const std::string& title, const libsdlgui::Dimentions& dimentions, SDL_WindowFlags windowFlags);
};

#endif // TESTAPP_HPP
