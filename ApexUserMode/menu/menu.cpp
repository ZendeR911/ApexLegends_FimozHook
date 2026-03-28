#include "menu.h"
#include "../features/entity.h"
#include <windows.h>
#include <cstdio>
#include <chrono>

extern uint64_t  GameBase;
extern Vector2   ScreenSize;
extern int       g_DbgEntityCount;
extern uint64_t  g_DbgLocalPlayer;
extern bool      g_DbgVMValid;
extern bool      g_DbgDriverOk;
extern uint64_t  g_DbgRawHandle;
extern uint64_t  g_DbgViewRender;
extern uint64_t  g_DbgVmPtr;
extern uint64_t  g_DbgEntityListVal;

static const char* KeyToString(int vk)
{
    if (vk >= 0x30 && vk <= 0x39) { static char b[2]; b[0]=(char)vk; return b; }
    if (vk >= 0x41 && vk <= 0x5A) { static char b[2]; b[0]=(char)vk; return b; }
    switch (vk) {
    case VK_LBUTTON:  return "LMB";
    case VK_RBUTTON:  return "RMB";
    case VK_MBUTTON:  return "MMB";
    case VK_XBUTTON1: return "Mouse 4";
    case VK_XBUTTON2: return "Mouse 5";
    case VK_SHIFT:    return "Shift";
    case VK_CONTROL:  return "Ctrl";
    case VK_MENU:     return "Alt";
    case VK_ESCAPE:   return "Escape";
    case VK_SPACE:    return "Space";
    case VK_INSERT:   return "Insert";
    case VK_DELETE:   return "Delete";
    case VK_END:      return "End";
    case VK_LSHIFT:   return "L Shift";
    case VK_RSHIFT:   return "R Shift";
    case VK_LCONTROL: return "L Ctrl";
    case VK_RCONTROL: return "R Ctrl";
    case VK_F1:       return "F1";  case VK_F2:  return "F2";
    case VK_F3:       return "F3";  case VK_F4:  return "F4";
    case VK_F5:       return "F5";  case VK_F6:  return "F6";
    case VK_F7:       return "F7";  case VK_F8:  return "F8";
    case VK_F9:       return "F9";  case VK_F10: return "F10";
    case VK_F11:      return "F11"; case VK_F12: return "F12";
    default:          return "Unknown";
    }
}

static int CountSpectators(uint64_t localPlayer)
{
    if (!localPlayer) return 0;
    int spectators = 0;

    auto observerList = I::Read<uint64_t>(GameBase + OFFSET_OBSERVER_LIST);
    if (!observerList) return 0;

    for (int i = 0; i < 61; i++) {
        auto entity = GetEntityById(i, GameBase);
        if (!entity) continue;

        int playerIndex = I::Read<int>(entity + 0x38);
        int specIndex   = I::Read<int>(observerList + playerIndex * 8 + OFFSET_OBSERVER_INDEX);
        auto specEntity = I::Read<uint64_t>(GameBase + OFFSET_ENTITY_LIST + ((specIndex & 0xFFFF) << 5));
        if (specEntity == localPlayer) spectators++;
    }
    return spectators;
}

void RenderSpectatorCount()
{
    if (!g_Config.showSpectatorCount) return;

    uint64_t lp = I::Read<uint64_t>(GameBase + OFFSET_LOCAL_PLAYER);
    if (!lp) return;

    int cnt = CountSpectators(lp);
    if (cnt <= 0) return;

    ImDrawList* dL = ImGui::GetBackgroundDrawList();
    char buf[64];
    sprintf_s(buf, "Spectators: %d", cnt);

    ImFont*  font     = ImGui::GetFont();
    float    fontSize = ImGui::GetFontSize() * 2.5f;
    ImVec2   ts       = font->CalcTextSizeA(fontSize, FLT_MAX, 0.f, buf);

    float pad = 20.f, bp = 15.f;
    ImVec2 tp  = ImVec2(ScreenSize.x - ts.x - pad - bp, pad + bp);
    ImVec2 bMin= ImVec2(tp.x - bp, tp.y - bp);
    ImVec2 bMax= ImVec2(tp.x + ts.x + bp, tp.y + ts.y + bp);

    dL->AddRectFilled(bMin, bMax, ImColor(10,10,15,200), 8.f);
    dL->AddRect      (bMin, bMax, ImColor(51,140,255,180), 8.f, 0, 2.f);
    dL->AddText(font, fontSize, tp, ImColor(80,180,255,255), buf);
}

void RenderMenu()
{
    if (!g_Config.showMenu) return;

    static float glowTime = 0.f;
    glowTime += ImGui::GetIO().DeltaTime * 2.f;
    float glow = (sinf(glowTime) + 1.f) * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(50,50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(700,750), ImGuiCond_Always);

    if (!ImGui::Begin("APHEL  //  APEX LEGENDS", &g_Config.showMenu,
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
                      ImGuiWindowFlags_NoResize))
    { ImGui::End(); return; }

    ImDrawList* dL  = ImGui::GetWindowDrawList();
    ImVec2 wPos     = ImGui::GetWindowPos();
    ImVec2 wSize    = ImGui::GetWindowSize();

    ImColor glowCol(51+(int)(glow*100), 140+(int)(glow*80), 255, (int)((0.3f+glow*0.4f)*255));
    dL->AddRect(ImVec2(wPos.x-2,wPos.y-2),
                ImVec2(wPos.x+wSize.x+2,wPos.y+wSize.y+2),
                glowCol, 12.f, 0, 3.f);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f,0.85f,1.f,1.f));
    ImGui::SetCursorPosX((wSize.x - ImGui::CalcTextSize("Press INSERT to toggle menu").x)*0.5f);
    ImGui::Text("Press INSERT to toggle menu");
    ImGui::PopStyleColor();
    ImGui::Spacing(); ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Visual
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.15f,0.45f,0.85f,0.9f));
    if (ImGui::CollapsingHeader("  VISUAL FEATURES", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent(10.f);
        ImGui::Checkbox("  ESP Enabled", &g_Config.espEnabled);
        ImGui::Spacing();

        float dm = g_Config.maxDistance / 100.f;
        if (ImGui::SliderFloat("  Max Range", &dm, 16.67f, 50.f, "%.1f m"))
            g_Config.maxDistance = dm * 100.f;

        if (g_Config.espEnabled) {
            ImGui::Indent(20.f);
            const char* boxModes[] = { "Full Box", "Corner Box", "Side Lines", "Diamond Corners", "Neon Glow" };
            ImGui::Combo("  Box Mode", &g_Config.espBoxMode, boxModes, 5);
            ImGui::Checkbox("  Shield Bars",       &g_Config.shieldBars);
            ImGui::Checkbox("  Snaplines",         &g_Config.snaplines);
            if (g_Config.snaplines) {
                ImGui::Indent(20.f);
                const char* sm[] = {"Top","Middle","Bottom"};
                ImGui::Combo("  Snapline Mode", &g_Config.snaplineMode, sm, 3);
                ImGui::Unindent(20.f);
            }
            ImGui::Checkbox("  Spectator Count", &g_Config.showSpectatorCount);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            //Bone ESP
            ImGui::Checkbox("  Bone ESP (Skeleton)", &g_Config.boneESP);
            if (g_Config.boneESP) {
                ImGui::Indent(20.f);
                ImGui::ColorEdit4("  Bone Color", (float*)&g_Config.colorBone, ImGuiColorEditFlags_AlphaBar);
                ImGui::Unindent(20.f);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();        


            ImGui::Spacing();
            ImGui::ColorEdit4("  Box Visible",     (float*)&g_Config.colorBoxVisible,    ImGuiColorEditFlags_AlphaBar);
            ImGui::ColorEdit4("  Box Not Visible", (float*)&g_Config.colorBoxNotVisible, ImGuiColorEditFlags_AlphaBar);
            if (g_Config.snaplines) {
                ImGui::ColorEdit4("  Snap Visible",     (float*)&g_Config.colorSnapVisible,    ImGuiColorEditFlags_AlphaBar);
                ImGui::ColorEdit4("  Snap Not Visible", (float*)&g_Config.colorSnapNotVisible, ImGuiColorEditFlags_AlphaBar);
            }
            ImGui::Unindent(20.f);
        }
        ImGui::Unindent(10.f);
    }
    ImGui::PopStyleColor();
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    //Combat
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.15f,0.45f,0.85f,0.9f));
    if (ImGui::CollapsingHeader("  COMBAT FEATURES", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent(10.f);
        ImGui::Checkbox("  Aimbot Enabled", &g_Config.aimbotEnabled);
        if (g_Config.aimbotEnabled) {
            ImGui::Indent(20.f);

            static bool lk1 = false, lk2 = false;
            static auto lt1 = std::chrono::steady_clock::now();
            static auto lt2 = std::chrono::steady_clock::now();

            auto makeKeyBtn = [&](const char* prefix, int& key, bool& listening,
                                  std::chrono::steady_clock::time_point& startTime) {
                char lbl[64];
                sprintf_s(lbl, "%s: %s", prefix, listening ? "Listening..." : KeyToString(key));
                if (ImGui::Button(lbl, ImVec2(200,22))) {
                    listening = !listening;
                    if (listening) startTime = std::chrono::steady_clock::now();
                }
                if (listening) {
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::chrono::steady_clock::now() - startTime).count();
                    if (ms > 200) {
                        for (int k=1; k<256; k++) {
                            if (GetAsyncKeyState(k) & 0x8000) {
                                key = k; listening = false; break;
                            }
                        }
                    }
                }
            };
            makeKeyBtn("Aim Key 1", g_Config.aimKey1, lk1, lt1);
            makeKeyBtn("Aim Key 2", g_Config.aimKey2, lk2, lt2);

            ImGui::Spacing();
            ImGui::SliderFloat("  FOV Radius", &g_Config.aimbotFov,  10.f,200.f, "%.0f px");

            //Boneaim
            ImGui::Spacing();
            {
                const char* boneNames[] = { "Head", "Neck", "Upper Chest", "Lower Chest", "Stomach", "Hip" };
                ImGui::Text("  Aim Bone:");
                ImGui::SameLine();
                ImGui::PushItemWidth(150.f);
                ImGui::Combo("##AimBone", &g_Config.aimbotBoneTarget, boneNames, 6);
                ImGui::PopItemWidth();
            }
            ImGui::Spacing();
            ImGui::Checkbox("  Draw FOV Circle",   &g_Config.aimbotDrawFov);
            ImGui::Checkbox("  Draw Crosshair",    &g_Config.aimbotDrawCrosshair);
            ImGui::Checkbox("  Visible Only",      &g_Config.aimbotVisibleOnly);
			ImGui::Checkbox("  Prediction (Move)", &g_Config.predictionEnabled);
            ImGui::Checkbox("  bhop 1337", &g_Config.bhopEnabled); 


            if (g_Config.aimbotVisibleOnly) {
                ImGui::Indent(20.f);
                ImGui::Checkbox("  Hold to Ignore Walls", &g_Config.aimbotHoldDisableVis);
                if (g_Config.aimbotHoldDisableVis) {
                    static bool lkh = false;
                    static auto lth = std::chrono::steady_clock::now();
                    makeKeyBtn("    Hold Key", g_Config.holdDisableVisKey, lkh, lth);
                }
                ImGui::Unindent(20.f);
            }

            if (g_Config.aimbotDrawFov) {
                ImGui::Spacing();
                ImGui::ColorEdit4("  FOV Circle",        (float*)&g_Config.colorFovCircle,       ImGuiColorEditFlags_AlphaBar);
                ImGui::ColorEdit4("  FOV Circle Locked", (float*)&g_Config.colorFovCircleLocked, ImGuiColorEditFlags_AlphaBar);
            }
            if (g_Config.aimbotDrawCrosshair)
                ImGui::ColorEdit4("  Crosshair", (float*)&g_Config.colorCrosshair, ImGuiColorEditFlags_AlphaBar);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.6f, 0.85f, 1.f, 1.f), "  Aim Settings (Memory Write):");
            ImGui::Indent(20.f);

            ImGui::SliderFloat("  Smoothing", &g_Config.memAimSmoothing, 0.0f, 10.0f, "%.1f");
            ImGui::Checkbox("  Recoil Compensation", &g_Config.recoilComp);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Вычитает punch angles (отдачу) при наводке");

            ImGui::Unindent(20.f);
            ImGui::Unindent(20.f);
        }
        ImGui::Unindent(10.f);
    }
    ImGui::PopStyleColor();

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
    
    ImGui::PopStyleColor();

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    //System info
    ImVec2 ib = ImGui::GetCursorScreenPos();
    dL->AddRectFilled(ib, ImVec2(ib.x+wSize.x-40, ib.y+160), ImColor(5,20,45,200), 8.f);
    dL->AddRect      (ib, ImVec2(ib.x+wSize.x-40, ib.y+160),
                      ImColor(40+(int)(glow*60), 120+(int)(glow*60), 255, 180), 8.f, 0, 1.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY()+2);
    ImGui::Indent(15.f);
    ImGui::TextColored(ImVec4(0.6f,0.85f,1.f,1.f), "SYSTEM INFO");
    ImGui::Spacing();

    ImGui::TextColored(g_DbgDriverOk ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f),
        "Driver:      %s", g_DbgDriverOk ? "Connected" : "Disconnected");
    ImGui::TextColored(GameBase ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f),
        "Game Base:   0x%llX", (unsigned long long)GameBase);
    ImGui::TextColored(g_DbgLocalPlayer ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(1.f,0.5f,0.3f,1.f),
        "LocalPlayer: 0x%llX", (unsigned long long)g_DbgLocalPlayer);
    ImGui::TextColored(g_DbgRawHandle ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f),
        "LP direct:   0x%llX", (unsigned long long)g_DbgRawHandle);
    ImGui::TextColored(g_DbgEntityListVal ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(1.f,0.5f,0.3f,1.f),
        "EntList[0]:  0x%llX", (unsigned long long)g_DbgEntityListVal);
    ImGui::TextColored(g_DbgViewRender ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f),
        "ViewRender:  0x%llX", (unsigned long long)g_DbgViewRender);
    ImGui::TextColored(g_DbgVmPtr ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f),
        "VM ptr:      0x%llX", (unsigned long long)g_DbgVmPtr);
    ImGui::TextColored(g_DbgVMValid ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(1.f,0.5f,0.3f,1.f),
        "View Matrix: %s", g_DbgVMValid ? "Valid" : "Invalid / Zero");
    ImGui::TextColored(g_DbgEntityCount > 0 ? ImVec4(0.4f,1.f,0.5f,1.f) : ImVec4(0.7f,0.9f,1.f,0.9f),
        "Enemies ESP: %d visible", g_DbgEntityCount);

    float fps = ImGui::GetIO().Framerate;
    ImVec4 fpsCol = fps>100 ? ImVec4(0.5f,1.f,0.7f,1.f)
                            : (fps>60 ? ImVec4(0.7f,0.9f,1.f,1.f) : ImVec4(1.f,0.5f,0.5f,1.f));
    ImGui::TextColored(fpsCol, "Frame Rate:  %.1f FPS", fps);
    ImGui::Unindent(15.f);

    ImGui::End();
}
