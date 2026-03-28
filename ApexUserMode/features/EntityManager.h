#pragma once
#include <vector>
#include <mutex>
#include "entity.h"

struct CachedEntity {
    uint64_t ptr;
    int      index;
    int      teamNumber;
    Vector3  position;
    Vector3  velocity;
    int      bleedoutState;
    float    health;
    int      maxHealth;
    int      shield;
    int      maxShield;
    float    lastVisibleTime;
    bool     isVisible;
    float    distance;
    int      boneIndices[128]; 
    bool     bonesInitialized;
};

class EntityManager {
public:
    static void Update(uint64_t gameBase, uint64_t localPlayer);
    static const std::vector<CachedEntity>& GetEntities();
    static void Clear();

private:
    static std::vector<CachedEntity> m_Entities;
    static std::mutex                m_Mutex;
};
