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
	HWND handle = NULL;					//�������� �ڵ�
	HINSTANCE hInstance = NULL;			//�ν��Ͻ��� �ڵ�
	string window_title = "";			
	wstring window_title_wide = L"";	
	string window_class = "";			
	wstring window_class_wide = L"";	
	int width;
	int height;

};

