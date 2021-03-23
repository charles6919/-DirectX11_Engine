#include "../../stdafx.h"
#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	if (!InitializeDirectX(hwnd, width, height))
		return false;

	if (!InitializeShaders())
		return false;
	return true;
}

void Graphics::RenderFrame()
{
	float bgColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	this->swapchain->Present(1, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height)
{
	vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No DXGI Adapters found.");
		return false;
	}

	HRESULT hr;

	//Create Swapchain
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = hwnd;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		

		hr = D3D11CreateDeviceAndSwapChain(	adapters[0].pAdapter,
													D3D_DRIVER_TYPE_UNKNOWN,
													NULL,
													NULL,
													NULL,
													0,
													D3D11_SDK_VERSION,
													&desc,
													this->swapchain.GetAddressOf(),
													this->device.GetAddressOf(),
													NULL,
													this->deviceContext.GetAddressOf());

		assert(SUCCEEDED(hr));
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	{
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		assert(SUCCEEDED(hr));
	}

	{
		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), NULL);

	return true;
}

bool Graphics::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT hr;
	UINT numElements = ARRAYSIZE(layout);
	{
		hr = this->device->CreateInputLayout(layout, numElements, vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	return false;
}
