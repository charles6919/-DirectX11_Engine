#include "stdafx.h"
#include "WindowContainer.h"

LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OutputDebugStringA("Window Proc From Window Container.\n");
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
