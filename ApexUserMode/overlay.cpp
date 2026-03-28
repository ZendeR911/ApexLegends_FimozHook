#include "overlay.h"
#include <string>
#include <thread>
#include <chrono>
#include <random>

#include "features/config.h"
#include "features/aim.h"
#include "features/camera.h"
#include "features/entity.h"
#include "features/esp.h"
#include "menu/menu.h"
#include "menu/zalupa/menu/menu.h"
#include "features/glow.h"
#include "features/strafe.h"
#include "features/EntityManager.h"
#include "apex_sdk.h"

// main
extern uint64_t GameBase;
extern bool g_Running;
extern Vector2 ScreenSize;
extern Config g_Config;

//ApexUserMode.cpp
extern uint64_t g_DbgViewRender;
extern uint64_t g_DbgVmPtr;
extern bool     g_DbgVMValid;
extern bool     g_DbgDriverOk;
extern uint64_t g_DbgLocalPlayer;
extern uint64_t g_DbgEntityListVal;

// Overlay Globals
HWND g_OverlayHwnd = nullptr;
HWND g_GameHwnd = nullptr;

static ID3D11DeviceContext* g_pd3dContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

// Matrix state
Matrix g_CachedVM = {};
static DWORD lastVMRead = 0;

namespace Util {
    std::wstring RandomString(int len) {
        static const wchar_t alph[] = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::wstring s;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(alph) / sizeof(wchar_t) - 2);
        for (int i = 0; i < len; i++) s += alph[dis(gen)];
        return s;
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
        return true;

    switch (Message) {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
            if (g_pRenderTargetView) { g_pRenderTargetView->Release(); g_pRenderTargetView = nullptr; }
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            
            ID3D11Texture2D* pBackBuffer;
            g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
            if (pBackBuffer) {
                g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
                pBackBuffer->Release();
            }
        }
        return 0;
    case WM_DESTROY:
        overlay::Shutdown();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, Message, wParam, lParam);
}

void HandleMessages(std::wstring lClassName, std::wstring lWindowName) {
    WNDCLASSEXW wClass = { sizeof(WNDCLASSEXW), CS_HREDRAW | CS_VREDRAW, WinProc, 0, 0, NULL, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW), NULL, NULL, lClassName.c_str(), NULL };
    RegisterClassExW(&wClass);

    g_OverlayHwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, 
                                   lClassName.c_str(), lWindowName.c_str(), WS_POPUP, 
                                   0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 
                                   NULL, NULL, NULL, NULL);

    SetLayeredWindowAttributes(g_OverlayHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(g_OverlayHwnd, &margin);
    
    SetWindowDisplayAffinity(g_OverlayHwnd, WDA_EXCLUDEFROMCAPTURE);

    ShowWindow(g_OverlayHwnd, SW_SHOW);
    UpdateWindow(g_OverlayHwnd);

    MSG msg;
    while (g_Running) {
        if (PeekMessageW(&msg, g_OverlayHwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (g_GameHwnd) {
            HWND hwnd_active = GetForegroundWindow();
            if (hwnd_active == g_GameHwnd) {
                HWND hwnd_prev = GetWindow(g_GameHwnd, GW_HWNDPREV);
                SetWindowPos(g_OverlayHwnd, hwnd_prev, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    DestroyWindow(g_OverlayHwnd);
    UnregisterClassW(lClassName.c_str(), NULL);
}

bool overlay::InitWindow() {
    std::wstring lClassName = Util::RandomString(12);
    std::wstring lWindowName = Util::RandomString(12);

    std::thread msgThread(HandleMessages, lClassName, lWindowName);
    msgThread.detach();

    auto start = std::chrono::steady_clock::now();
    while (!g_OverlayHwnd && std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
        Sleep(10);
    }

    return (g_OverlayHwnd != nullptr);
}

bool overlay::DirectXInit() {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = GetSystemMetrics(SM_CXSCREEN);
    sd.BufferDesc.Height = GetSystemMetrics(SM_CYSCREEN);
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_OverlayHwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_FEATURE_LEVEL fl;
    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &fl, &g_pd3dContext)))
        return false;

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
        pBackBuffer->Release();
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; 
    
    ImGui_ImplWin32_Init(g_OverlayHwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext);

    return true;
}

static void SetClickThrough(bool ct) {
    LONG_PTR ex = GetWindowLongPtrW(g_OverlayHwnd, GWL_EXSTYLE);
    if (ct) ex |= (WS_EX_TRANSPARENT | WS_EX_LAYERED);
    else ex &= ~WS_EX_TRANSPARENT;
    SetWindowLongPtrW(g_OverlayHwnd, GWL_EXSTYLE, ex);
}

void overlay::Render() {
    while (g_Running) {
        if (GetAsyncKeyState(VK_END) & 1) { g_Running = false; break; }
        
        // Toggle menu
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            g_Config.showMenu = !g_Config.showMenu;
            SetClickThrough(!g_Config.showMenu);
            if (g_Config.showMenu) {
                SetForegroundWindow(g_OverlayHwnd);
            } else {
                SetForegroundWindow(g_GameHwnd);
            }
        }

        // Update Matrix
        DWORD now = GetTickCount();
        if (now - lastVMRead > 16) {
            lastVMRead = now;
            uint64_t viewRender = I::Read<uint64_t>(GameBase + OFF_VIEW_RENDER);
            g_DbgViewRender = viewRender;

            uint64_t vmPtr = 0;
            if (viewRender) vmPtr = I::Read<uint64_t>(viewRender + OFF_VIEW_MATRIX);
            if (!vmPtr) vmPtr = I::Read<uint64_t>(GameBase + OFF_VIEW_MATRIX);
            g_DbgVmPtr = vmPtr;

            if (vmPtr) {
                g_CachedVM = I::Read<Matrix>(vmPtr);
                g_DbgVMValid = (g_CachedVM.matrix[0] != 0.f || g_CachedVM.matrix[5] != 0.f);
            } else {
                g_DbgVMValid = false;
            }
            g_DbgDriverOk = g_Driver.isConnected();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        uint64_t lp = I::Read<uint64_t>(GameBase + OFFSET_LOCAL_PLAYER);
        g_DbgLocalPlayer = lp;
        g_DbgEntityListVal = I::Read<uint64_t>(GameBase + OFFSET_ENTITY_LIST);
        EntityManager::Update(GameBase, lp);

        //ыныцылызацыя
        UpdateAimbot(g_CachedVM);
        RenderESP(g_CachedVM);
        RenderAimbotFOV();
        RenderCrosshair();
        RenderSpectatorCount();
        RenderMenu();

        if (g_Config.showMenu)
            Render::RenderMenu();

        UpdateBhop(GameBase);
        UpdateGlow(GameBase);

        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0, 0, 0, 0 };
        g_pd3dContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
        g_pd3dContext->ClearRenderTargetView(g_pRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }
}

void overlay::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pd3dContext) g_pd3dContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}