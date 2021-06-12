#pragma once
#include "COMException.h"

class ErrorLogger
{
public:
	static void Log(string message);
	static void Log(HRESULT hr, string message);
	static void Log(HRESULT hr, wstring message);
	static void Log(COMException& exception);
};

