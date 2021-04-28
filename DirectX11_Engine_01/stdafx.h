#pragma once

//Includes
#include <Windows.h>
#include <comdef.h>
#include <string>
#include <memory>
#include <vector>
#include <wrl/client.h>
#include <assert.h>

//D3D Header
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

//Libs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D3DCompiler.lib")

//ImGUI
#include "Systems/Graphics/ImGui/imgui.h"
#include "Systems/Graphics/ImGui/imgui_impl_win32.h"
#include "Systems/Graphics/ImGui/imgui_impl_dx11.h"
//Namespaces
using namespace std;

