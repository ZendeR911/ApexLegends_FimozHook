#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <cstdio>
#include <string>
#include <chrono>
#include <unordered_map>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")


#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"


#include "apex_sdk.h"
#include "features/esp.h"
#include "overlay.h"

#include "features/config.h"
#include "features/aim.h"
#include "features/camera.h"
#include "features/entity.h"
#include "menu/menu.h"
#include "features/glow.h"
#include "features/strafe.h"
#include "features/EntityManager.h"

Config g_Config;

uint64_t  GameBase   = 0;
HWND      g_hWnd     = nullptr;
Vector2   ScreenSize;
bool      g_ShowMenu = true;
bool      g_Running  = true;

int      g_DbgEntityCount   = 0;   // enemies found this frame
uint64_t g_DbgLocalPlayer   = 0;   // local player ptr
bool     g_DbgVMValid       = false; // view matrix non-zero
bool     g_DbgDriverOk      = false; // driver connected
uint64_t g_DbgRawHandle     = 0;   // raw handle from OFFSET_LOCAL_ENTITY_HANDLE
uint64_t g_DbgViewRender    = 0;   // ptr read from GameBase+OFF_VIEW_RENDER
uint64_t g_DbgVmPtr         = 0;   // ptr read from viewRender+OFF_VIEW_MATRIX
uint64_t g_DbgEntityListVal = 0;   // first entry of entity list (slot 0)

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (!I::installDriver("")) {
        MessageBoxA(nullptr, "Failed to connect to driver!\nLoad the driver first.",
                    "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    g_DbgDriverOk = true;

    DWORD PID = 0;
    for (int i = 0; i < 30 && !PID; i++) {
        PID = I::GetProcessIdByName(L"r5apex_dx12.exe");
        if (!PID) { MessageBoxA(nullptr, "Waiting for Apex Legends...", "Waiting", MB_OK); }
    }
    if (!PID) {
        MessageBoxA(nullptr, "Apex Legends not found.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    I::SetProcess(PID);

    for (int i = 0; i < 20 && !GameBase; i++) {
        GameBase = I::GetModuleBase("r5apex_dx12.exe");
        if (!GameBase) Sleep(3000);
    }
    if (!GameBase) {
        MessageBoxA(nullptr, "Failed to get game module base.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ScreenSize = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };
    g_GameHwnd = FindWindowA(NULL, "Apex Legends");
    if (!g_GameHwnd) {
        MessageBoxA(nullptr, "Could not find Apex Legends window.", "Error", MB_OK);
        return 1;
    }

    if (!overlay::InitWindow()) {
        MessageBoxA(nullptr, "Failed to initialize overlay window.", "Error", MB_OK);
        return 1;
    }

    if (!overlay::DirectXInit()) {
        MessageBoxA(nullptr, "Failed to initialize DirectX.", "Error", MB_OK);
        return 1;
    }

    overlay::Render();

    overlay::Shutdown();
    return 0;
}

