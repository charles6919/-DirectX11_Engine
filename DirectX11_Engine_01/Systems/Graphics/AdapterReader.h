#pragma once
#include "../../ErrorLogger.h"

class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter = nullptr;
	DXGI_ADAPTER_DESC desc;
};

class AdapterReader
{
public:
	static vector<AdapterData> GetAdapters();
private:
	static vector<AdapterData> adapters;
};

