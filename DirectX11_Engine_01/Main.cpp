#include "stdafx.h"
#include "RenderWindow.h"

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance, 
					   _In_ LPWSTR lpCmdLine, 
					   _In_ int nCmdShow )
{
	RenderWindow rw;
	rw.Initialize(hInstance, "Title", "MyWindowClass", 800, 600);
	while (rw.ProcessMessages() == true)
	{
		int a = 10;
	}
	return 0;
}