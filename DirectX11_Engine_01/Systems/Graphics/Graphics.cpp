#include "../../stdafx.h"
#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	if (!InitializeDirectX(hwnd, width, height))
	{
		return false;
	}

	return true;
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height)
{
	vector<AdapterData> adapters = AdapterReader::GetAdapters();

	return true;
}
