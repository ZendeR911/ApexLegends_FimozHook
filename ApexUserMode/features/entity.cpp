#include "entity.h"

uint64_t GetEntityById(uint64_t entityIndex, uint64_t gameBase)
{
    uint64_t entityList = gameBase + OFFSET_ENTITY_LIST;
    return I::Read<uint64_t>(entityList + (entityIndex << 5));
}
