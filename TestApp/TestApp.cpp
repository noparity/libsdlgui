#include "stdafx.h"
#include "FontManager.hpp"
#include "SdlHelpers.hpp"
#include "TestApp.hpp"
#include <SDL_keycode.h>

TestControl::TestControl(Window* pWindow, const SDL_Rect& location) :
	Control(pWindow, location)
{
	m_label1 = std::make_unique<Label>(this, SDLRect(0, 0, 64, 32));
	m_label1->SetBorderColor(SDLColor(255, 0, 0, 0));
	m_label1->SetBorderSize(1);

	m_label2 = std::make_unique<Label>(this, SDLRect(location.w - 64, 0, 64, 32));
	m_label2->SetBorderColor(SDLColor(0, 0, 255, 0));
	m_label2->SetBorderSize(1);
	m_label2->SetAlignment(TextAlignment::TopRight);

	m_label3 = std::make_unique<Label>(this, SDLRect((location.w / 2) - 32, ((location.h / 2) - 32), 64, 32));
	m_label3->SetBorderColor(SDLColor(0, 255, 255, 0));
	m_label3->SetBorderSize(1);
	m_label3->SetAlignment(TextAlignment::MiddleCenter);

	m_label4 = std::make_unique<Label>(this, SDLRect(0, location.h - 64, 64, 32));
	m_label4->SetBorderColor(SDLColor(255, 0, 0, 0));
	m_label4->SetBorderSize(1);
	m_label4->SetAlignment(TextAlignment::BottomLeft);

	m_label5 = std::make_unique<Label>(this, SDLRect(location.w - 64, location.h - 64, 64, 32));
	m_label5->SetBorderColor(SDLColor(0, 0, 255, 0));
	m_label5->SetBorderSize(1);
	m_label5->SetAlignment(TextAlignment::BottomRight);
}

void TestControl::RenderImpl()
{
	GetWindow()->DrawRectangle(GetLocation(), GetBackgroundColor(), UINT8_MAX);
}

/*
SDL_Rect panelLoc;
panelLoc.w = 320;
panelLoc.h = 240;
panelLoc.x = (dimentions.W / 2) - (panelLoc.w / 2);
panelLoc.y = (dimentions.H / 2) - (panelLoc.h / 2);

m_panel = std::make_unique<TestControl>(this, panelLoc);
m_panel->SetBorderColor(SDLColor(0, 255, 0, 0));
m_panel->SetBorderSize(1);

SDL_Rect loc;
loc.w = 64;
loc.h = 32;
loc.x = 256;
loc.y = 0;

m_label1 = std::make_unique<Label>(this, loc);
m_label1->SetBorderColor(SDLColor(255, 0, 0, 0));
m_label1->SetBorderSize(1);
m_label1->SetText("test");
m_label1->SetAlignment(TextAlignment::MiddleCenter);
m_label1->SetFont(FontManager::GetInstance()->GetOrLoadFont("times", 16, Font::Attributes::Underline | Font::Attributes::Bold));

auto labelPtr = m_panel.get();
//RegisterForResizeEvent([labelPtr](Window* pWindow)
//{
//	// re-center the label
//	SDL_Rect loc = labelPtr->GetLocation();
//	auto dims = pWindow->GetDimentions();
//	loc.x = (dims.W / 2) - 32;
//	loc.y = (dims.H / 2) - 16;
//	labelPtr->SetLocation(loc);
//});
*/

TestWindow::TestWindow(const std::string& title, const Dimentions& dimentions, SDL_WindowFlags windowFlags) :
	m_frameNumber(0), Window(title, dimentions, windowFlags)
{
	m_dialog = std::make_unique<Dialog>(this, "Test Dialog", SDLRect(48, 34, 640, 480));
	m_button1 = std::make_unique<Button>(this, SDLRect(32, 32, 64, 32));
	m_button1->RegisterForClickCallback([this]()
	{
		this->m_dialog->SetHidden(false);
	});

	m_button2 = std::make_unique<Button>(this, SDLRect(128, 64, 64, 32));
	m_button2->RegisterForClickCallback([this]()
	{
		this->NextFrame();
	});

	m_textBox = std::make_unique<TextBox>(this, SDLRect(32, 128, 128, 32));

	FrameOne();
}

void TestWindow::FrameOne()
{
	auto frame = std::make_unique<Label>(this, SDLRect((GetDimentions().W / 2) - 64, 0, 128, 32));
	frame->SetText("Frame One");
	frame->SetAlignment(TextAlignment::BottomCenter);
	frame->SetBorderColor(SDLColor(255, 0, 0, 0));
	frame->SetBorderSize(1);
	m_frame = std::move(frame);
}

void TestWindow::FrameTwo()
{
	auto frame = std::make_unique<Label>(this, SDLRect((GetDimentions().W / 2) - 64, 0, 128, 32));
	frame->SetText("Frame Two");
	frame->SetAlignment(TextAlignment::BottomCenter);
	frame->SetBorderColor(SDLColor(0, 255, 0, 0));
	frame->SetBorderSize(1);
	m_frame = std::move(frame);
}

void TestWindow::FrameThree()
{
	auto frame = std::make_unique<Label>(this, SDLRect((GetDimentions().W / 2) - 64, 0, 128, 32));
	frame->SetText("Frame Three");
	frame->SetAlignment(TextAlignment::BottomCenter);
	frame->SetBorderColor(SDLColor(0, 0, 255, 0));
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
	SDLInit sdlInit;

	TestWindow win("Test App", Dimentions(1024, 768), static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE));

	while (Window::ActiveWindows())
		Window::DispatchEvents();

	return 0;
}
