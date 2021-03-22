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
	if (adapters.size() > 0)//�ִٸ� ������ ���� ����
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

	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))//�׷���ī�带 �ν��Ͽ� ���Ϳ� Ǫ��
	{
		adapters.push_back(AdapterData(pAdapter));
		index++;
	}
	return adapters;
}
