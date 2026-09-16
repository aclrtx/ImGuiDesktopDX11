#pragma once

#define WINDOW_WIDTH 250
#define WINDOW_HEIGHT 200
#define WINDOW_TITLE "ImGuiDesktopDX11"

#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <string>

static bool g_swapChainOccluded = false;

static HWND g_handle = nullptr;
static WNDCLASSEX g_windowClass = {};

static ID3D11Device* g_pD3DDevice = nullptr;
static ID3D11DeviceContext* g_pD3DDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND handle, UINT message, WPARAM data, LPARAM context );
