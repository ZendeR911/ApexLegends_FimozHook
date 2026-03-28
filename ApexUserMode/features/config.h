#pragma once
#include "../Imgui/imgui.h"
#include <windows.h>

struct Config {
    bool  showMenu                = true;
    bool  espEnabled              = true;
    bool  snaplines               = false;
    bool  shieldBars              = true;
    int   espBoxMode              = 1;      // 0-4: Full, Corner, Side, Diamond, Neon
    int   snaplineMode            = 2;      // 0 = top, 1 = middle, 2 = bottom

    bool  aimbotEnabled           = false;
    float aimbotFov               = 60.0f;
    bool  aimbotVisibleOnly       = false;
    bool  aimbotDrawFov           = true;
    bool  aimbotDrawCrosshair     = true;
    int   aimbotBone              = 0;
    int   aimKey1                 = VK_RBUTTON;
    int   aimKey2                 = VK_LBUTTON;
    bool  aimbotHoldDisableVis    = false;
    int   holdDisableVisKey       = VK_LSHIFT;

    float maxDistance             = 5000.0f;

    ImVec4 colorFovCircle         = ImVec4(1.0f, 0.65f, 0.0f, 0.8f);
    ImVec4 colorFovCircleLocked   = ImVec4(0.0f, 1.0f,  0.0f, 0.8f);
    ImVec4 colorCrosshair         = ImVec4(1.0f, 1.0f,  1.0f, 0.6f);
    ImVec4 colorBoxVisible        = ImVec4(1.0f, 1.0f,  1.0f, 0.75f);
    ImVec4 colorBoxNotVisible     = ImVec4(1.0f, 0.0f,  0.0f, 0.75f);
    ImVec4 colorSnapVisible       = ImVec4(1.0f, 1.0f,  1.0f, 0.75f);
    ImVec4 colorSnapNotVisible    = ImVec4(1.0f, 0.0f,  0.0f, 0.75f);

    bool  showSpectatorCount      = false;

    // Bone ESP 
    bool   boneESP           = false;
    ImVec4 colorBone         = ImVec4(1.0f, 1.0f, 1.0f, 0.85f);

    // Glow
    bool   glowEnabled = false;
    int    glowId = 2;   // 0-7 не работет так что похуй

    // Aimbot bone 
    int    aimbotBoneTarget  = 1;   
    bool predictionEnabled = true;

    // ── Memory-write aim (VABot) ──
    float memAimSmoothing  = 5.0f;
    bool  recoilComp       = false;  // компенсация отдачи aims

    //// RCS
    //bool  rcsEnabled       = false;
    //float rcsPitchStr      = 1.0f;  // сила компенсации по pitch (вертикаль)
    //float rcsYawStr        = 0.5f;  // сила компенсации по yaw   (горизонталь)    это другая хуйня ее не трогай
    //int   rcsKey           = VK_LBUTTON; // клавиша стрельбы

    // Camera Features 
    /*bool  fovEnabled       = false;
    float fovValue         = 12000.0f;        тоже не работет эта хуйня, ну я не тестил так что потом сделаю
    bool  thirdPerson      = false;*/


    //movenet 
    bool bhopEnabled = false;
;
};

extern Config g_Config;
