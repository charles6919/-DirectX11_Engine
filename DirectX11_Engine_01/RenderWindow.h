#pragma once
#include "ErrorLogger.h"

class RenderWindow
{
public:
	bool Initialize(HINSTANCE hInstance, string window_title, string window_class, int width, int height);
	bool ProcessMessages();
	~RenderWindow();
private:
	void RegisterWindowClass();

private:
	HWND handle = NULL;					//윈도우의 핸들
	HINSTANCE hInstance = NULL;			//인스턴스의 핸들
	string window_title = "";			
	wstring window_title_wide = L"";	
	string window_class = "";			
	wstring window_class_wide = L"";	
	int width;
	int height;

};

