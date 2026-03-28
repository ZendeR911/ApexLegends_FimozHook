#include "strafe.h"
#include "entity.h"
#include "../features/config.h"
#include <windows.h>

extern uint64_t GameBase;

void UpdateBhop(uint64_t gameBase)
{
    if (!g_Config.bhopEnabled) return;
    if (!gameBase) return;

    if (!(GetAsyncKeyState(VK_SPACE) & 0x8000)) return;

    uint64_t localPlayer = I::Read<uint64_t>(gameBase + OFFSET_LOCAL_PLAYER);
    if (!localPlayer) return;

    // 0x1 = на земле
    int flags = I::Read<int>(localPlayer + 0xc8);
    bool onGround = (flags & 0x1) != 0;

    if (onGround)
        I::Write<int>(gameBase + OFFSET_IN_JUMP + 0x8, 5);
    else
        I::Write<int>(gameBase + OFFSET_IN_JUMP + 0x8, 4);
}