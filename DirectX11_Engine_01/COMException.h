#pragma once
#include <comdef.h>
#include <string>
#include "StringConverter.h"
using namespace std;

#define COM_ERROR_IF_FAILED(hr, msg) if(FAILED(hr)) throw COMException(hr, msg, __FILE__, __FUNCTION__, __LINE__)

class COMException
{
public:
	COMException(HRESULT hr, const string& msg, const string& file, const string& function, int line)
	{
		_com_error error(hr);
		whatmsg = L"Msg: " + StringConverter::StringToWide(msg) + L"\n";
		whatmsg += error.ErrorMessage();
		whatmsg += L"\nFILE: " + StringConverter::StringToWide(file);
		whatmsg += L"\nFUCTION : " + StringConverter::StringToWide(function);
		whatmsg += L"\nLINE : " + StringConverter::StringToWide(std::to_string(line));
	}

	const wchar_t* what() const
	{
		return whatmsg.c_str();
	}

private:
	std::wstring whatmsg;
};