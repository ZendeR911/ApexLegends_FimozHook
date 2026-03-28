#pragma once
#include <cstdint>
#include <vector>
#include "../apex_sdk.h"

struct EntityData {
    int     teamNumber;
    Vector3 position;
    int     bleedoutState;
    float   health;
    int     shield;
    int     maxHealth;
    int     maxShield;
    float   lastVisibleTime;
};

uint64_t              GetEntityById(uint64_t entityIndex, uint64_t gameBase);
std::vector<uint64_t> GetPlayerEntities(uint64_t gameBase, uint64_t localPlayer);