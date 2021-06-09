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
	this->deviceContext->OMSetBlendState(this->blendState.Get(), NULL, 0xFFFFFFFF);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	 
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	static float alpha = 0.1f;

	{//PinkSquare
		//Update Constant Buffer
		static float translationOffset[3] = { 0, 0, -1.0f };
		DirectX::XMMATRIX world = XMMatrixTranslation(translationOffset[0], translationOffset[1], translationOffset[2]);
		cb_vs_vertexShader.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
		cb_vs_vertexShader.data.mat = DirectX::XMMatrixTranspose(cb_vs_vertexShader.data.mat); //hlsl의 행렬의 행열이 반대기때문에 재배열 해줘야 한다.

		if (!cb_vs_vertexShader.ApplyChanges())
			return;
		this->deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexShader.GetAddressOf());


		this->cb_ps_pixelShader.data.alpha = alpha;
		this->cb_ps_pixelShader.ApplyChanges();
		this->deviceContext->PSSetConstantBuffers(0, 1, cb_ps_pixelShader.GetAddressOf());

		//Draw Square
		this->deviceContext->PSSetShaderResources(0, 1, this->pavementTexture.GetAddressOf());
		this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
		this->deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->RSSetState(this->rasterizerState_CullFront.Get());
		this->deviceContext->DrawIndexed(indexBuffer.BufferSize(), 0, 0);
		this->deviceContext->RSSetState(this->rasterizerState.Get());
		this->deviceContext->DrawIndexed(indexBuffer.BufferSize(), 0, 0);

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
	ImGui::DragFloat("Alpha", &alpha, 0.01f, 0.0f, 1.0f);
	ImGui::End();
	//Assemble Together Draw data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(0, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
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

	//Descibe Depth/Stencil buffer
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = this->windowWidth;
		desc.Height = this->windowHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = this->device->CreateTexture2D(&desc, NULL, this->depthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");
			return false;
		}

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create depth stencil view.");
			return false;
		}
	}

	//Create depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(CD3D11_DEPTH_STENCIL_DESC));

		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->device->CreateDepthStencilState(&desc, this->depthStencilState.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create depth stencil state.");
			return false;
		}
	}

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//Create Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)this->windowWidth;
	viewport.Height = (FLOAT)this->windowHeight;
	viewport.MaxDepth = 1.0f;


	this->deviceContext->RSSetViewports(1, &viewport);

	//Create Rasterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		hr = this->device->CreateRasterizerState(&desc, this->rasterizerState.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create rasterizer state.");
			return false;
		}
	}

	//Create Rasterizer State for culling front.
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

		hr = this->device->CreateRasterizerState(&desc, this->rasterizerState_CullFront.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create rasterizer state.");
			return false;
		}
	}
	

	//Create Blend State
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC  rtbDesc;
		ZeroMemory(&rtbDesc, sizeof(rtbDesc));

		rtbDesc.BlendEnable				= true;
		rtbDesc.SrcBlend				= D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbDesc.DestBlend				= D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbDesc.BlendOp					= D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbDesc.SrcBlendAlpha			= D3D11_BLEND::D3D11_BLEND_ONE;
		rtbDesc.DestBlendAlpha			= D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbDesc.BlendOpAlpha			= D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbDesc.RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = rtbDesc;

		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create blend state.");
			return false;
		}
	}

	spriteBatch = make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

	//Create sampler desc for sampler state
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));

		desc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = this->device->CreateSamplerState(&desc, this->samplerState.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create sampler state.");
			return false;
		}
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
	//Red Tri
	Vertex v[] =
	{
		Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),	//FRONT 왼쪽 아래
		Vertex(-0.5f,  0.5f, -0.5f, 0.0f, 0.0f),	//FRONT 왼쪽 위 
		Vertex(0.5f,  0.5f, -0.5f, 1.0f, 0.0f),		//FRONT 오른쪽 위
		Vertex(0.5f, -0.5f, -0.5f, 1.0f, 1.0f),		//FRONT 오른쪽 아래

		Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f),		//BACK 왼쪽 아래
		Vertex(-0.5f,  0.5f, 0.5f, 0.0f, 0.0f),		//BACK 왼쪽 위 
		Vertex(0.5f,  0.5f, 0.5f, 1.0f, 0.0f),		//BACK 오른쪽 위
		Vertex(0.5f, -0.5f, 0.5f, 1.0f, 1.0f),		//BACK 오른쪽 아래
	};

	DWORD indices[] =
	{
		0, 1, 2,	//FRONT
		2, 3, 0,	//FRONT

		4, 7, 6,	//BACK
		4, 6, 5,	//BACK

		3, 2, 6,	//RIGHT
		3, 6, 7,	//RIGHT

		4, 5, 1,	//LEFT
		4, 1, 0,	//LEFT

		1, 5, 6,	//TOP
		1, 6, 2,	//TOP

		0, 3, 7,	//BOTTOM
		0, 7, 4,	//BOTTOM


	};

	//Create vertex buffer & load vertex data.
	HRESULT hr;
	{
		hr = this->vertexBuffer.Initialize(this->device.Get(), v, ARRAYSIZE(v));
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create vertex buffer.");
			return false;
		}
	}

	//Create index buffer & load index data
	{
		hr = this->indexBuffer.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create index buffer.");
			return false;
		}
	}

	//Load texture
	{
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create wic texture from file.");
			return false;
		}
	}

	{
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create wic texture from file.");
			return false;
		}
	}

	{
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create wic texture from file.");
			return false;
		}
	}

	//Initialize constant buffers
	{
		hr = this->cb_vs_vertexShader.Initialize(this->device.Get(), this->deviceContext.Get());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
			return false;
		}
	}

	//Initialize constant buffers
	{
		hr = this->cb_ps_pixelShader.Initialize(this->device.Get(), this->deviceContext.Get());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
			return false;
		}
	}


	camera.SetPosition(0.0f, 0.0f, -2.0f);
	camera.setprojectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}
