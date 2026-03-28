#pragma once
#include <cmath>
#include <cstdint>
#include "ApexOffsets.h"
#include "Memory/Interface.hpp"

struct Vector2 {
    float x, y;
    Vector2()                   : x(0.f), y(0.f) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}
};

struct Vector3 {
    float x, y, z;
    Vector3()                            : x(0.f), y(0.f), z(0.f) {}
    Vector3(float x_, float y_, float z_): x(x_), y(y_), z(z_) {}
    float   Length()             const { return sqrtf(x*x + y*y + z*z); }
    Vector3 operator-(const Vector3& o) const { return { x-o.x, y-o.y, z-o.z }; }
    Vector3 operator+(const Vector3& o) const { return { x+o.x, y+o.y, z+o.z }; }
};

struct Matrix {
    float matrix[16];
};


using Vec2 = Vector2;


static constexpr float K_PI = 3.14159265358979323846f;


inline float NormalizeAngle(float a) {
    while (a >  180.f) a -= 360.f;
    while (a < -180.f) a += 360.f;
    return a;
}


inline float AngleLerp(float from, float to, float t) {
    return from + NormalizeAngle(to - from) * t;
}


inline Vector3 CalcAngle(const Vector3& src, const Vector3& dst) {
    float dx     = dst.x - src.x;
    float dy     = dst.y - src.y;
    float dz     = dst.z - src.z;
    float dist2d = sqrtf(dx*dx + dy*dy);

    Vector3 a;
    a.x = -atan2f(dz, dist2d) * (180.f / K_PI);  // pitch
    a.y =  atan2f(dy, dx)     * (180.f / K_PI);   // yaw
    a.z = 0.f;
    return a;
}

inline Vector3 _WorldToScreen(const Vector3 pos, const Matrix& m, const Vector2 resolution)
{
    Vector3 out{};

    float _x = m.matrix[0]*pos.x + m.matrix[1]*pos.y + m.matrix[2]*pos.z  + m.matrix[3];
    float _y = m.matrix[4]*pos.x + m.matrix[5]*pos.y + m.matrix[6]*pos.z  + m.matrix[7];
    out.z    = m.matrix[12]*pos.x + m.matrix[13]*pos.y + m.matrix[14]*pos.z + m.matrix[15];

    if (out.z < 0.001f) { out.z = -1.f; return out; }

    _x *= 1.f / out.z;
    _y *= 1.f / out.z;

    int width  = (int)resolution.x;
    int height = (int)resolution.y;

    out.x = (float)(width  / 2) + 0.5f * _x * (float)width  + 0.5f;
    out.y = (float)(height / 2) - 0.5f * _y * (float)height + 0.5f;

    return out;
}


#define BONE_ARRAY_BASE_X  0x0C
#define BONE_ARRAY_BASE_Y  0x1C
#define BONE_ARRAY_BASE_Z  0x2C

inline int GetBoneFromHitbox(uint64_t entity, int hitboxId)
{
    uint64_t model = I::Read<uint64_t>(entity + OFFSET_STUDIO_HDR);
    if (!model) return -1;

    uint64_t studioHdr = I::Read<uint64_t>(model + 0x8);
    if (!studioHdr) return -1;

    uint16_t hitboxCache = I::Read<uint16_t>(studioHdr + 0x34);
    uint64_t hitboxArray = studioHdr + (uint64_t)((uint16_t)(hitboxCache & 0xFFFE) << (4 * (hitboxCache & 1)));

    uint16_t indexCache = I::Read<uint16_t>(hitboxArray + 0x4);
    int hitboxIndex = ((uint16_t)(indexCache & 0xFFFE) << (4 * (indexCache & 1)));

    uint16_t bone = I::Read<uint16_t>(hitboxArray + hitboxIndex + (hitboxId * 0x20));
    if (bone > 255) return -1;

    return (int)bone;
}

inline Vector3 GetBonePosition(uint64_t entity, uint32_t boneId, const Vector3& entityOrigin)
{
    uint64_t boneArray = I::Read<uint64_t>(entity + OFFSET_BONE);
    if (!boneArray) return {};

    float bx = I::Read<float>(boneArray + BONE_ARRAY_BASE_X + boneId * 0x30);
    float by = I::Read<float>(boneArray + BONE_ARRAY_BASE_Y + boneId * 0x30);
    float bz = I::Read<float>(boneArray + BONE_ARRAY_BASE_Z + boneId * 0x30);

    if (fabsf(bx) < 0.01f && fabsf(by) < 0.01f && fabsf(bz) < 0.01f)
        return {};

    return { bx + entityOrigin.x, by + entityOrigin.y, bz + entityOrigin.z };
}

struct BoneMatrix {
    float data[12]; // 3x4 matrix
};

inline bool ReadBoneMatrix(uint64_t entity, BoneMatrix* outBones, int count)
{
    uint64_t boneArray = I::Read<uint64_t>(entity + OFFSET_BONE);
    if (!boneArray) return false;

    return g_Driver.readProcessMemory(g_CurrentPID, boneArray, outBones, count * sizeof(BoneMatrix));
}

inline Vector3 GetBonePositionFromMatrix(const BoneMatrix& m, const Vector3& entityOrigin)
{
    return { m.data[3] + entityOrigin.x, m.data[7] + entityOrigin.y, m.data[11] + entityOrigin.z };
}

inline Vector3 GetBonePositionFromHitbox(uint64_t entity, int hitboxId,
                                         const Vector3& entityOrigin)
{
    int boneIndex = GetBoneFromHitbox(entity, hitboxId);
    if (boneIndex < 0) return {};
    return GetBonePosition(entity, (uint32_t)boneIndex, entityOrigin);
}
