#pragma once
#include "RenderWindow.h"
#include "Systems/Keyboard/Keyboard.h"
#include "Systems/Mouse/Mouse.h"

class WindowContainer
{
public:
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	RenderWindow render_window;
	Keyboard keyboard;
	Mouse mouse;
private:

};

