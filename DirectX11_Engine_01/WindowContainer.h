#pragma once
#include "RenderWindow.h"
#include "Systems/Keyboard/Keyboard.h"
class WindowContainer
{
public:
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	RenderWindow render_window;
	Keyboard keyboard;

private:
};

