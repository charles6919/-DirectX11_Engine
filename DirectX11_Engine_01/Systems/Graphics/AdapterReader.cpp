#include"../../stdafx.h"
#include "AdapterReader.h"

vector<AdapterData> AdapterReader::adapters;

AdapterData::AdapterData(IDXGIAdapter* pAdapter)
{
	this->pAdapter = pAdapter;
	{
		HRESULT hr = pAdapter->GetDesc(&this->desc);
		assert(SUCCEEDED(hr));
	}
}

vector<AdapterData> AdapterReader::GetAdapters()
{
	if (adapters.size() > 0)//있다면 만들지 말고 리턴
	{
		return adapters;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

	{
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create DXGIFactory for enumerating adapters.");
			exit(-1);
		}
	}

	IDXGIAdapter* pAdapter;
	UINT index = 0;

	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))//그래픽카드를 인식하여 벡터에 푸시
	{
		adapters.push_back(AdapterData(pAdapter));
		index++;
	}
	return adapters;
}
