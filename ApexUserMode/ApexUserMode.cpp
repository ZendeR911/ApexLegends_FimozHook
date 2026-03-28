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
//#include "menu/zalupa/menu.h"
#include "menu/zalupa/textures.h"
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
    printf("[*] Starting Apex Legends FimozHook...\n");

    if (!I::installDriver("")) {
        MessageBoxA(nullptr, "Failed to connect to driver!\nLoad the driver first.",
                    "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    g_DbgDriverOk = true;

    DWORD PID = 0;
    const wchar_t* procNames[] = { L"r5apex.exe", L"r5apex_dx12.exe" };
    
    printf("[*] Waiting for Apex Legends (r5apex.exe or r5apex_dx12.exe)...\n");
    for (int i = 0; i < 30 && !PID; i++) {
        for (const auto& name : procNames) {
            PID = I::GetProcessIdByName(name);
            if (PID) {
                printf("[+] Found process: %ls (PID: %lu)\n", name, PID);
                break;
            }
        }
        if (!PID) Sleep(1000);
    }
    
    if (!PID) {
        MessageBoxA(nullptr, "Apex Legends not found.\nMake sure the game is running (r5apex.exe or r5apex_dx12.exe).", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    I::SetProcess(PID);

    for (int i = 0; i < 20 && !GameBase; i++) {
        GameBase = I::GetModuleBase("r5apex.exe");
        if (!GameBase) GameBase = I::GetModuleBase("r5apex_dx12.exe");
        if (!GameBase) {
            printf("[!] Failed to get game module base, retrying (%d/20)...\n", i + 1);
            Sleep(2000);
        }
    }
    if (!GameBase) {
        MessageBoxA(nullptr, "Failed to get game module base.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    printf("[+] Game Base: 0x%llX\n", (unsigned long long)GameBase);

    ScreenSize = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };
    g_GameHwnd = FindWindowA(NULL, "Apex Legends");
    if (!g_GameHwnd) {
        // Fallback for different languages or window names
        g_GameHwnd = FindWindowA("Respawn001", NULL);
    }
    
    if (!g_GameHwnd) {
        MessageBoxA(nullptr, "Could not find Apex Legends window.\nEnsure the game is not minimized and is in windowed/borderless mode.", "Error", MB_OK);
        return 1;
    }
    printf("[+] Found Game Window: 0x%p\n", g_GameHwnd);

    if (!overlay::InitWindow()) {
        MessageBoxA(nullptr, "Failed to initialize overlay window.", "Error", MB_OK);
        return 1;
    }

    if (!overlay::DirectXInit()) {
        MessageBoxA(nullptr, "Failed to initialize DirectX.", "Error", MB_OK);
        return 1;
    }

    printf("[+] Initialization complete. Hiding console and starting overlay.\n");
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    overlay::Render();

    overlay::Shutdown();
    return 0;
}

