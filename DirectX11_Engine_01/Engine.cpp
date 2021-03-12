#include "stdafx.h"
#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, string window_title, string window_class, int width, int height)
{
	keyboard.EnableAutoRepeatChars();
	return this->render_window.Initialize(this, hInstance, window_title, window_class, width, height);
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update()
{
	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
		//string outMsg = "Char: ";
		//outMsg += ch;
		//outMsg += "\n";
		//OutputDebugStringA(outMsg.c_str());
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
		/*string outMsg = "";
		if (kbe.IsPress())
		{
			outMsg += "Key press: ";
		}
		else if (kbe.IsRelease())
		{
			outMsg += "Key release: ";
		}
		outMsg += keycode;
		outMsg += "\n";
		OutputDebugStringA(outMsg.c_str());*/
	}
}
