#include "EntityManager.h"
#include <algorithm>
#include <mutex>
#include <unordered_map>

std::vector<CachedEntity> EntityManager::m_Entities;
std::mutex                EntityManager::m_Mutex;
static std::unordered_map<uint64_t, std::vector<int>> g_ModelBoneCache;

struct BatchData {
    int     team;
    Vector3 pos;
    Vector3 vel;
    int     bleed;
    float   hp;
    int     maxHp;
    int     sh;
    int     maxSh;
    float   vis;
};

void EntityManager::Update(uint64_t gameBase, uint64_t localPlayer)
{
    if (!gameBase) return;

    int ltTeam = 0;
    Vector3 lpPos = { 0,0,0 };
    float curTime = 0;

    if (localPlayer) {
        ltTeam  = I::Read<int>(localPlayer + OFFSET_TEAM_NUMBER);
        lpPos   = I::Read<Vector3>(localPlayer + OFFSET_CAMERA_ORIGIN);
        curTime = I::Read<float>(localPlayer + OFFSET_TIME_BASE);
    }

    std::vector<CachedEntity> tempEntities;
    tempEntities.reserve(64);

    const int BATCH_SIZE = 10;
    for (int i = 0; i < 100; i += BATCH_SIZE) {
        uint64_t addrs[BATCH_SIZE] = { 0 };
        int count = 0;

        for (int j = 0; j < BATCH_SIZE; j++) {
            uint64_t e = GetEntityById(i + j, gameBase);
            if (e && e != localPlayer) {
                addrs[count++] = e;
            }
        }

        if (count == 0) continue;

        BatchData bd[BATCH_SIZE] = { 0 };
        DriverInterface::BatchReadEntry batch[BATCH_SIZE * 9];
        int si = 0;

        for (int k = 0; k < count; k++) {
            batch[si++] = { addrs[k] + OFFSET_TEAM_NUMBER,       &bd[k].team,  sizeof(int) };
            batch[si++] = { addrs[k] + OFFSET_ORIGIN,            &bd[k].pos,   sizeof(Vector3) };
            batch[si++] = { addrs[k] + OFFSET_VELOCITY,          &bd[k].vel,   sizeof(Vector3) };
            batch[si++] = { addrs[k] + OFFSET_BLEEDOUT_STATE,    &bd[k].bleed, sizeof(int) };
            batch[si++] = { addrs[k] + OFFSET_HEALTH,            &bd[k].hp,    sizeof(float) };
            batch[si++] = { addrs[k] + OFFSET_MAX_HEALTH,        &bd[k].maxHp, sizeof(int) };
            batch[si++] = { addrs[k] + OFFSET_SHIELD,            &bd[k].sh,    sizeof(int) };
            batch[si++] = { addrs[k] + OFFSET_SHIELD_MAX,        &bd[k].maxSh, sizeof(int) };
            batch[si++] = { addrs[k] + OFFSET_LAST_VISIBLE_TIME, &bd[k].vis,   sizeof(float) };
        }

        I::BatchRead(batch, si);

        for (int k = 0; k < count; k++) {
            if (bd[k].team <= 0 || bd[k].team > 150) continue;
            if (bd[k].hp <= 0 || bd[k].hp > 500)      continue;

            CachedEntity ce;
            ce.ptr             = addrs[k];
            ce.index           = i + k;
            ce.teamNumber      = bd[k].team;
            ce.position        = bd[k].pos;
            ce.velocity        = bd[k].vel;
            ce.bleedoutState   = bd[k].bleed;
            ce.health          = bd[k].hp;
            ce.maxHealth       = bd[k].maxHp;
            ce.shield          = bd[k].sh;
            ce.maxShield       = bd[k].maxSh;
            ce.lastVisibleTime = bd[k].vis;
            ce.isVisible       = (ce.lastVisibleTime > 0.f) && (curTime - ce.lastVisibleTime < 0.3f);
            
            float dx = ce.position.x - lpPos.x;
            float dy = ce.position.y - lpPos.y;
            float dz = ce.position.z - lpPos.z;
            ce.distance = sqrtf(dx*dx + dy*dy + dz*dz) / 39.62f; 

            uint64_t model = I::Read<uint64_t>(ce.ptr + OFFSET_STUDIO_HDR);
            if (model) {
                if (g_ModelBoneCache.find(model) == g_ModelBoneCache.end()) {
                    std::vector<int> indices(20, -1);
                    for (int h = 0; h < 20; h++) {
                        indices[h] = GetBoneFromHitbox(ce.ptr, h);
                    }
                    g_ModelBoneCache[model] = indices;
                }
                auto& cached = g_ModelBoneCache[model];
                for(int h=0; h<20 && h<cached.size(); h++) ce.boneIndices[h] = cached[h];
                ce.bonesInitialized = true;
            } else {
                ce.bonesInitialized = false;
            }

            tempEntities.push_back(ce);
        }
    }

    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Entities = std::move(tempEntities);
}

const std::vector<CachedEntity>& EntityManager::GetEntities()
{
    return m_Entities;
}

void EntityManager::Clear()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Entities.clear();
}
