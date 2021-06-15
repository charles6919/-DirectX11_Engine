#include "../../stdafx.h"
#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	//Setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame()
{
	float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(),
		D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	 
	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	{//Pavement Cube
		this->model.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}

	//Drw Text
	static int fpsCounter = 0;
	static string fpsString = "FPS : 0";
	fpsCounter++;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0f)
	{
		fpsString = "FPS : " + to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringConverter::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	static int counter = 0;
	//Start the Dear ImGUI frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGUI Test Window
	ImGui::Begin("Test");
	ImGui::End();
	//Assemble Together Draw data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(0, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
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
			DXGI_SWAP_CHAIN_DESC desc = {0};
			ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

			desc.BufferDesc.Width = this->windowWidth;
			desc.BufferDesc.Height = this->windowHeight;
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


			hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter,
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

			COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

		{
			hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
			COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");
		}

		{
			hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");
		}

		//Descibe Depth/Stencil buffer
		{
			CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
			//D3D11_TEXTURE2D_DESC desc;

			desc.MipLevels = 1;
			desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;

			hr = this->device->CreateTexture2D(&desc, NULL, this->depthStencilBuffer.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

			hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");
		}

		//Create depth stencil state
		{
			CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
			//D3D11_DEPTH_STENCIL_DESC desc;
			ZeroMemory(&desc, sizeof(CD3D11_DEPTH_STENCIL_DESC));

			desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

			hr = this->device->CreateDepthStencilState(&desc, this->depthStencilState.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		}

		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Create Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f,  static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
		//D3D11_VIEWPORT viewport;
		this->deviceContext->RSSetViewports(1, &viewport);

		//Create Rasterizer State
		{
			CD3D11_RASTERIZER_DESC desc(D3D11_DEFAULT);

			hr = this->device->CreateRasterizerState(&desc, this->rasterizerState.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
		}

		//Create Rasterizer State for culling front.
		{
			CD3D11_RASTERIZER_DESC desc(D3D11_DEFAULT);
			desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

			hr = this->device->CreateRasterizerState(&desc, this->rasterizerState_CullFront.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
		}


		//Create Blend State
		{
			D3D11_BLEND_DESC blendDesc = {0};

			D3D11_RENDER_TARGET_BLEND_DESC  rtbDesc = { 0 };

			rtbDesc.BlendEnable = true;
			rtbDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
			rtbDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
			rtbDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			rtbDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			rtbDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
			rtbDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			rtbDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			blendDesc.RenderTarget[0] = rtbDesc;

			hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");
		}

		spriteBatch = make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		//Create sampler desc for sampler state
		{
			CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);

			desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

			hr = this->device->CreateSamplerState(&desc, this->samplerState.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
		}
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

bool Graphics::InitializeShaders()
{
	wstring shaderFolderPath = L"";
#pragma region 셰이더 경로 결정부분
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
		shaderFolderPath = L"../x64/Debug/";
#else //x86
		shaderFolderPath = L"../Debug/";
#endif
#else //Relesase Mode
#ifdef  _WIN32//x64
		shaderFolderPath = L"../x64/Release/";
#else//x86
		shaderFolderPath = L"../Release/";
#endif
#endif // DEBUG
	}
#pragma endregion 셰이더 경로 결정부분

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexShader.Initialize(this->device, shaderFolderPath + L"VertexShader.cso", layout, numElements))
		return false;

	if (!pixelShader.Initialize(this->device, shaderFolderPath + L"PixelShader.cso"))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	try
	{
		HRESULT hr = NULL;
		//Load texture
		{
			hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
		}

		{
			hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
		}

		{
			hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
		}

		//Initialize constant buffers
		{
			hr = this->cb_vs_vertexShader.Initialize(this->device.Get(), this->deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		}

		//Initialize constant buffers
		{
			hr = this->cb_ps_pixelShader.Initialize(this->device.Get(), this->deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		}

		//Initialize Model(s)
		{
			if (!model.Initialize(this->device.Get(), this->deviceContext.Get(), this->pavementTexture.Get(), this->cb_vs_vertexShader))
			{
				return false;
			}
		}

		camera.SetPosition(0.0f, 0.0f, -2.0f);
		camera.setprojectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}
