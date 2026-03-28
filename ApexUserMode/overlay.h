#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"

extern HWND g_OverlayHwnd;
static ID3D11Device* g_pd3dDevice = nullptr;
extern HWND g_GameHwnd;

namespace overlay
{
    bool InitWindow();
    bool DirectXInit();
    void Render();
    void Shutdown();
}