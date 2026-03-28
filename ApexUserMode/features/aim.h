#pragma once
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../apex_sdk.h"
#include "config.h"

struct AimbotState {
    bool     isAiming;
    uint64_t currentTarget;
};
extern AimbotState g_AimbotState;

static Vector3 ApplyPrediction(const Vector3& bonePos, const Vector3& velocity, const Vector3& camPos, bool enabled)
{
    if (!enabled) return bonePos;

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

void UpdateAimbot(const Matrix& vm);
void RenderAimbotFOV();
void RenderCrosshair();