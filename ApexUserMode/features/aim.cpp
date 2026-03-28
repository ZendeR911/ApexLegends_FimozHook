#include "aim.h"
#include "entity.h"
#include <windows.h>
#include <algorithm>

#include "EntityManager.h"

AimbotState g_AimbotState;

extern uint64_t GameBase;
extern Vector2 ScreenSize;

static float RandomFloat(float mn, float mx)
{
    return mn + (float)rand() / ((float)RAND_MAX / (mx - mn));
}

static float Distance2D(const Vector2& a, const Vector2& b)
{
    float dx = a.x - b.x, dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

struct AimbotTargetData {
    int     teamNumber;
    int     bleedoutState;
    Vector3 position;
    float   lastVisibleTime;
    Vector3 velocity; // для предикта
};

#define AIMBOT_BATCH     10
#define AIMBOT_SAMPLES   5 // было 4, добавили velocity

static uint64_t FindBestTarget(uint64_t localPlayer, const Matrix& viewMatrix,
    float currentTime, bool checkVisibility)
{
    if (!localPlayer) return 0;

    Vector3 localPos = I::Read<Vector3>(localPlayer + OFFSET_CAMERA_ORIGIN);
    int     localTeam = I::Read<int>(localPlayer + OFFSET_TEAM_NUMBER);
    Vector2 center = { ScreenSize.x / 2.f, ScreenSize.y / 2.f };

    uint64_t bestTarget = 0;
    float    bestDist = FLT_MAX;

    const auto& entities = EntityManager::GetEntities();
    for (const auto& d : entities) {
        if (d.teamNumber == localTeam)  continue;
        if (d.bleedoutState >= 2)       continue;
        if (checkVisibility && !d.isVisible) continue;
        if (d.position.x == 0.f && d.position.y == 0.f && d.position.z == 0.f) continue;
        if (d.distance * 39.62f > g_Config.maxDistance) continue;

        static const int k_AimHitboxes[] = { 0, 1, 2, 3, 4, 5 };
        int selHitbox = k_AimHitboxes[ImClamp(g_Config.aimbotBoneTarget, 0, 5)];
        
        Vector3 bonePos = { 0,0,0 };
        if (d.bonesInitialized && d.boneIndices[selHitbox] != -1) {
            bonePos = GetBonePosition(d.ptr, d.boneIndices[selHitbox], d.position);
        }

        if (bonePos.x == 0.f && bonePos.y == 0.f && bonePos.z == 0.f) {
            bonePos = d.position; bonePos.z += 50.f;
        }

        Vector3 sp = _WorldToScreen(bonePos, viewMatrix, ScreenSize);
        if (sp.z <= 0.f) continue;

        float fov = bestTarget ? g_Config.aimbotFov : g_Config.aimbotFov * 1.25f;
        float dist2d = Distance2D(Vector2{ sp.x, sp.y }, center);
        if (dist2d > fov) continue;

        if (dist2d < bestDist) { bestDist = dist2d; bestTarget = d.ptr; }
    }
    return bestTarget;
}

static Vector3 ApplyPrediction(const Vector3& bonePos, const Vector3& velocity, const Vector3& camPos)
{
    if (!g_Config.predictionEnabled) return bonePos;

    float dx = bonePos.x - camPos.x;
    float dy = bonePos.y - camPos.y;
    float dz = bonePos.z - camPos.z;
    float distance = sqrtf(dx * dx + dy * dy + dz * dz);

    float projectileSpeed = 28000.f;
    float timeToHit = distance / projectileSpeed;

    if (distance < 600.f || timeToHit > 0.8f) return bonePos;

    return {
        bonePos.x + velocity.x * timeToHit,
        bonePos.y + velocity.y * timeToHit,
        bonePos.z + velocity.z * timeToHit
    };
}

static void SmoothAimMemory(uint64_t localPlayer,
    const Vector3& camPos,
    const Vector3& targetBone)
{
    Vector3 targetAngle = CalcAngle(camPos, targetBone);

    if (g_Config.recoilComp) {
        Vector3 punch = I::Read<Vector3>(localPlayer + OFFSET_PUNCH_ANGLES);
        targetAngle.x -= punch.x * 2.0f;
        targetAngle.y -= punch.y * 2.0f;
    }

    targetAngle.x = ImClamp(targetAngle.x, -89.f, 89.f);
    targetAngle.y = NormalizeAngle(targetAngle.y);

    Vector3 cur = I::Read<Vector3>(localPlayer + OFFSET_VIEW_ANGLES);

    Vector3 delta;
    delta.x = NormalizeAngle(targetAngle.x - cur.x);
    delta.y = NormalizeAngle(targetAngle.y - cur.y);
    delta.z = 0.f;

    float s = g_Config.memAimSmoothing;
    if (s > 0.0f) {
        float divider = 1.0f + (s * 25.0f);
        delta.x /= divider;
        delta.y /= divider;
    }

    Vector3 out;
    out.x = ImClamp(cur.x + delta.x, -89.f, 89.f);
    out.y = NormalizeAngle(cur.y + delta.y);
    out.z = 0.f;

    I::Write<Vector3>(localPlayer + OFFSET_VIEW_ANGLES, out);
}


void UpdateAimbot(const Matrix& vm)
{
    if (!g_Config.aimbotEnabled) {
        g_AimbotState.isAiming = false;
        return;
    }

    static int      scanTick = 0;
    static uint64_t target   = 0;

    bool k1 = (GetAsyncKeyState(g_Config.aimKey1) & 0x8000) != 0;
    bool k2 = (GetAsyncKeyState(g_Config.aimKey2) & 0x8000) != 0;
    if (!k1 && !k2) {
        g_AimbotState.isAiming = false;
        g_AimbotState.currentTarget = 0;
        target = 0; 
        return;
    }

    bool holdKey = g_Config.aimbotHoldDisableVis && g_Config.aimbotVisibleOnly
        ? (GetAsyncKeyState(g_Config.holdDisableVisKey) & 0x8000) != 0
        : false;
    bool visOnly = g_Config.aimbotVisibleOnly && !holdKey;

    uint64_t lp = I::Read<uint64_t>(GameBase + OFFSET_LOCAL_PLAYER);
    if (!lp) return;
    if (!target || scanTick % 5 == 0) {
        float currentTime = I::Read<float>(lp + OFFSET_TIME_BASE);
        target = FindBestTarget(lp, vm, currentTime, visOnly);
    }
    scanTick++;

    if (!target) {
        g_AimbotState.isAiming = false;
        g_AimbotState.currentTarget = 0;
        return;
    }

    static float    yOffset = 0.f;
    static uint64_t lastTarget = 0;
    if (target != lastTarget) {
        yOffset = (RandomFloat(0.f, 1.f) < 0.17f) ? RandomFloat(-20.f, -10.f)
            : RandomFloat(-10.f, 10.f);
        lastTarget = target;
    }
    else {
        yOffset += RandomFloat(-1.2f, 1.2f);
        if (yOffset > 20.f) yOffset = 20.f;
        if (yOffset < -30.f) yOffset = -30.f;
    }

    Vector3 origin = { 0,0,0 };
    Vector3 velocity = { 0,0,0 };

    for (const auto& d : EntityManager::GetEntities()) {
        if (d.ptr == target) {
            origin = d.position;
            velocity = d.velocity;
            break;
        }
    }

    if (origin.x == 0.f) return; 

    static const int k_AimHitboxesUpd[] = { 0, 1, 2, 3, 4, 5 };
    int selHitboxUpd = k_AimHitboxesUpd[ImClamp(g_Config.aimbotBoneTarget, 0, 5)];

    Vector3 bonePos = { 0,0,0 };
    int bi = -1;
    for (const auto& d : EntityManager::GetEntities()) {
        if (d.ptr == target && d.bonesInitialized) {
            bi = d.boneIndices[selHitboxUpd];
            break;
        }
    }

    if (bi != -1) {
        bonePos = GetBonePosition(target, (uint32_t)bi, origin);
    }

    if (bonePos.x == 0.f && bonePos.y == 0.f && bonePos.z == 0.f)
    {
        bonePos = origin; bonePos.z += 50.f;
    }
    bonePos.z += yOffset;

    g_AimbotState.isAiming = true;
    g_AimbotState.currentTarget = target;

    Vector3 camPos = I::Read<Vector3>(lp + OFFSET_CAMERA_ORIGIN);

    // предикт
    bonePos = ApplyPrediction(bonePos, velocity, camPos);

    SmoothAimMemory(lp, camPos, bonePos);
}

void RenderAimbotFOV()
{
    if (!g_Config.aimbotEnabled || !g_Config.aimbotDrawFov) return;
    ImDrawList* dL = ImGui::GetBackgroundDrawList();
    Vector2 c = { ScreenSize.x / 2.f, ScreenSize.y / 2.f };
    ImVec4 col = (g_AimbotState.isAiming && g_AimbotState.currentTarget)
        ? g_Config.colorFovCircleLocked
        : g_Config.colorFovCircle;
    dL->AddCircle(ImVec2(c.x, c.y), g_Config.aimbotFov, ImColor(col), 64, 2.f);
}

void RenderCrosshair()
{
    if (!g_Config.aimbotDrawCrosshair) return;
    ImDrawList* dL = ImGui::GetBackgroundDrawList();
    Vector2 c = { ScreenSize.x / 2.f, ScreenSize.y / 2.f };
    ImColor col = ImColor(g_Config.colorCrosshair);
    float s = 5.f;
    dL->AddLine(ImVec2(c.x - s, c.y), ImVec2(c.x + s, c.y), col, 1.5f);
    dL->AddLine(ImVec2(c.x, c.y - s), ImVec2(c.x, c.y + s), col, 1.5f);
}