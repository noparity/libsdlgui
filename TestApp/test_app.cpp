#include "stdafx.h"
#include "font_manager.hpp"
#include "sdl_helpers.hpp"
#include "test_app.hpp"
#include <SDL_keycode.h>

TestControl::TestControl(libsdlgui::Window* pWindow, const SDL_Rect& location) :
    Panel(pWindow, location)
{
    m_label1 = std::make_unique<libsdlgui::Label>(pWindow, libsdlgui::SDLRect(0, 0, 64, 32, location));
    m_label1->SetBorderColor(libsdlgui::SDLColor(255, 0, 0, 0));
    m_label1->SetBorderSize(1);
    AddControl(m_label1.get());

    m_label2 = std::make_unique<libsdlgui::Label>(pWindow, libsdlgui::SDLRect(location.w - 64, 0, 64, 32, location));
    m_label2->SetBorderColor(libsdlgui::SDLColor(0, 0, 255, 0));
    m_label2->SetBorderSize(1);
    m_label2->SetAlignment(libsdlgui::TextAlignment::TopRight);
    AddControl(m_label2.get());

    m_label3 = std::make_unique<libsdlgui::Label>(pWindow, libsdlgui::SDLRect((location.w / 2) - 32, ((location.h / 2) - 32), 64, 32, location));
    m_label3->SetBorderColor(libsdlgui::SDLColor(0, 255, 255, 0));
    m_label3->SetBorderSize(1);
    m_label3->SetAlignment(libsdlgui::TextAlignment::MiddleCenter);
    AddControl(m_label3.get());

    m_label4 = std::make_unique<libsdlgui::Label>(pWindow, libsdlgui::SDLRect(0, location.h - 32, 64, 32, location));
    m_label4->SetBorderColor(libsdlgui::SDLColor(255, 0, 0, 0));
    m_label4->SetBorderSize(1);
    m_label4->SetAlignment(libsdlgui::TextAlignment::BottomLeft);
    AddControl(m_label4.get());

    m_label5 = std::make_unique<libsdlgui::Label>(pWindow, libsdlgui::SDLRect(location.w - 64, location.h - 32, 64, 32, location));
    m_label5->SetBorderColor(libsdlgui::SDLColor(0, 0, 255, 0));
    m_label5->SetBorderSize(1);
    m_label5->SetAlignment(libsdlgui::TextAlignment::BottomRight);
    AddControl(m_label5.get());

    SetBackgroundColor(libsdlgui::SDLColor(63, 63, 63, 0));
}

void TestControl::RenderImpl()
{
    GetWindow()->DrawRectangle(GetLocation(), GetBackgroundColor(), UINT8_MAX);
}

TestDialog::TestDialog(libsdlgui::Window* pWindow, const std::string& title, const libsdlgui::Dimentions& dimentions) :
    Dialog(pWindow, title, dimentions),
    m_buttonOK(pWindow, libsdlgui::SDLRect(dimentions.W - 68, dimentions.H - 36, 64, 32, GetContentLoc())),
    m_buttonCancel(pWindow, libsdlgui::SDLRect(dimentions.W - 68 - 70, dimentions.H - 36, 64, 32, GetContentLoc())),
    m_text(pWindow, libsdlgui::SDLRect(32, 64, 128, 32, GetContentLoc()))
{
    AddControl(&m_buttonOK);
    AddControl(&m_buttonCancel);
    AddControl(&m_text);
    m_text.SetText("Click one");

    m_buttonOK.SetText("OK", GetForegroundColor(), m_buttonOK.GetBackgroundColor());

    m_buttonCancel.SetText("Cancel", GetForegroundColor(), m_buttonCancel.GetBackgroundColor());

    m_buttonCancel.RegisterForClickCallback([this]()
    {
        this->SetHidden(true);
    });
}

TestWindow::TestWindow(const std::string& title, const libsdlgui::Dimentions& dimentions, SDL_WindowFlags windowFlags) :
    m_frameNumber(0), Window(title, dimentions, windowFlags)
{
    m_label1 = std::make_unique<libsdlgui::Label>(this, libsdlgui::SDLRect(420, 128, 150, 32));
    m_label1->SetText("Nothing selected");
    m_label1->SetAlignment(libsdlgui::TextAlignment::MiddleCenter);
    m_dialog = std::make_unique<TestDialog>(this, "Test Dialog", libsdlgui::Dimentions(400, 300));
    m_button1 = std::make_unique<libsdlgui::Button>(this, libsdlgui::SDLRect(32, 32, 64, 32));
    m_button1->RegisterForClickCallback([this]()
    {
        this->m_dialog->CenterDialog();
        this->m_dialog->SetHidden(false);
    });
    m_button1->SetText("click", libsdlgui::SDLColor(255, 255, 255, 0), m_button1->GetBackgroundColor());

    m_button2 = std::make_unique<libsdlgui::Button>(this, libsdlgui::SDLRect(128, 64, 64, 32));
    m_button2->RegisterForClickCallback([this]()
    {
        this->NextFrame();
    });
    m_button2->SetText("next", libsdlgui::SDLColor(255, 255, 255, 0), m_button1->GetBackgroundColor());

    m_textBox = std::make_unique<libsdlgui::TextBox>(this, libsdlgui::SDLRect(32, 128, 128, 32));
    m_dropDown = std::make_unique<libsdlgui::DropdownBox>(this, libsdlgui::SDLRect(340, 200, 128, 32));

    for (int i = 0; i < 100; ++i)
        m_dropDown->AddItem(std::to_string(i));

    m_dropDown->RegisterForSelectionChangedCallback([this](const std::string& item)
    {
        m_label1->SetText(std::string("Selected ") + item);
    });

    m_checkBox1 = std::make_unique<libsdlgui::CheckBox>(this, libsdlgui::SDLRect(32, 178, 128, 32));
    m_checkBox1->Text()->SetText("Check me!");
    m_checkBox1->RegisterForCheckedChangedCallback([this](bool checked)
    {
        m_panel->SetHidden(checked);
    });

    SDL_Rect panelLoc;
    panelLoc.w = 320;
    panelLoc.h = 240;
    panelLoc.x = (dimentions.W / 2) - (panelLoc.w / 2);
    panelLoc.y = (dimentions.H / 2) - (panelLoc.h / 2);

    m_panel = std::make_unique<TestControl>(this, panelLoc);
    m_panel->SetBorderColor(libsdlgui::SDLColor(0, 255, 0, 0));
    m_panel->SetBorderSize(1);

    m_listBox = std::make_unique<libsdlgui::ListBox>(this, libsdlgui::SDLRect(40, 280, 128, 32), 3, 3);
    m_listBox->AddItem("some");
    m_listBox->AddItem("things");
    m_listBox->AddItem("to");
    m_listBox->AddItem("put");
    m_listBox->AddItem("in");
    m_listBox->AddItem("here");

    FrameOne();
}

void TestWindow::FrameOne()
{
    auto frame = std::make_unique<libsdlgui::Label>(this, libsdlgui::SDLRect((GetDimentions().W / 2) - 64, 0, 128, 32));
    frame->SetText("Frame One");
    frame->SetAlignment(libsdlgui::TextAlignment::BottomCenter);
    frame->SetBorderColor(libsdlgui::SDLColor(255, 0, 0, 0));
    frame->SetBorderSize(1);
    m_frame = std::move(frame);
}

void TestWindow::FrameTwo()
{
    auto frame = std::make_unique<libsdlgui::Label>(this, libsdlgui::SDLRect((GetDimentions().W / 2) - 64, 0, 128, 32));
    frame->SetText("Frame Two");
    frame->SetAlignment(libsdlgui::TextAlignment::BottomCenter);
    frame->SetBorderColor(libsdlgui::SDLColor(0, 255, 0, 0));
    frame->SetBorderSize(1);
    m_frame = std::move(frame);
}

void TestWindow::FrameThree()
{
    auto frame = std::make_unique<libsdlgui::Label>(this, libsdlgui::SDLRect((GetDimentions().W / 2) - 64, 0, 128, 32));
    frame->SetText("Frame Three");
    frame->SetAlignment(libsdlgui::TextAlignment::BottomCenter);
    frame->SetBorderColor(libsdlgui::SDLColor(0, 0, 255, 0));
    frame->SetBorderSize(1);
    m_frame = std::move(frame);
}

void TestWindow::NextFrame()
{
    ++m_frameNumber;
    if (m_frameNumber > 2)
        m_frameNumber = 0;

    switch (m_frameNumber)
    {
    case 0:
        FrameOne();
        break;
    case 1:
        FrameTwo();
        break;
    case 2:
        FrameThree();
        break;
    }
}

int main(int argc, char* argv[])
{
    libsdlgui::SDLInit sdlInit;

    TestWindow win("Test App", libsdlgui::Dimentions(1024, 768), SDL_WINDOW_RESIZABLE);

    bool quit = false;
    while (!quit)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
            quit = win.TranslateEvent(event);

        win.Render();
    }

    return 0;
}
