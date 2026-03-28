#include "glow.h"
#include "entity.h"
#include "EntityManager.h"

extern uint64_t GameBase;

void UpdateGlow(uint64_t gameBase)
{
    if (!g_Config.glowEnabled) return;
    if (!gameBase) return;

    static DWORD lastUpdate = 0;
    DWORD now = GetTickCount();
    if (now - lastUpdate < 500) return;
    lastUpdate = now;

    uint64_t lp = I::Read<uint64_t>(gameBase + OFFSET_LOCAL_PLAYER);
    if (!lp) return;

    int localTeam = I::Read<int>(lp + OFFSET_TEAM_NUMBER);
    if (localTeam <= 0) return;

    const auto& entities = EntityManager::GetEntities();
    for (const auto& d : entities)
    {
        if (d.ptr == lp) continue;
        if (d.teamNumber <= 0) continue;

        bool isEnemy = (d.teamNumber != localTeam);
        I::Write<uint32_t>(d.ptr + OFFSET_HIGHLIGHT_TEAM_BITS, 0xFFFFFFFF);
        I::Write<uint8_t>(d.ptr + OFFSET_HIGHLIGHT_TEAM_INDEX, isEnemy ? 0 : 1);
        I::Write<uint8_t>(d.ptr + OFFSET_HIGHLIGHT_CONTEXT, (uint8_t)g_Config.glowId);
    }
}