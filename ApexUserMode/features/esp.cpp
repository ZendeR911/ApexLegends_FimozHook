#include "esp.h"
#include <unordered_map>
#include <algorithm> 
#include "EntityManager.h"

extern uint64_t GameBase;
extern Vector2 ScreenSize;
extern uint64_t g_DbgRawHandle;
extern uint64_t g_DbgEntityListVal;
extern uint64_t g_DbgLocalPlayer;
extern int g_DbgEntityCount;


static void DrawHealthBar(ImDrawList* dL, float x, float y, float h, int hp, int max_hp)
{
    if (max_hp <= 0) max_hp = 100;
    float pct = ImClamp((float)hp / (float)max_hp, 0.f, 1.f);
    float barH = h * pct;
    float barW = 2.5f;
    float ox   = x - barW - 2.f;

    dL->AddRectFilled(ImVec2(ox, y), ImVec2(ox + barW, y + h), ImColor(0, 0, 0, 180));
    dL->AddRectFilled(ImVec2(ox, y + h - barH), ImVec2(ox + barW, y + h), ImColor(129, 255, 0, 255));
}

static void DrawShieldBar(ImDrawList* dL, float x, float y, float h, int shield, int max_shield)
{
    if (max_shield <= 0) return;
    float pct  = ImClamp((float)shield / (float)max_shield, 0.f, 1.f);
    float barH = h * pct;
    float barW = 2.5f;
    float ox   = x - barW * 2.f - 5.f;

    ImColor col = (max_shield <= 50)  ? ImColor(160, 160, 160, 255) :
                  (max_shield <= 75)  ? ImColor(30,  160, 255, 255) :
                  (max_shield <= 100) ? ImColor(180,  60, 255, 255) :
                                        ImColor(255,  50,  50, 255);

    dL->AddRectFilled(ImVec2(ox, y), ImVec2(ox + barW, y + h), ImColor(0, 0, 0, 180));
    dL->AddRectFilled(ImVec2(ox, y + h - barH), ImVec2(ox + barW, y + h), col);
}

//Full Box
static void DrawBoxFull(ImDrawList* dL, float x, float y, float w, float h,
                        ImColor color, int hp, int max_hp, int shield, int max_shield)
{
    dL->AddRect(ImVec2(x+1, y+1), ImVec2(x+w+1, y+h+1), ImColor(0,0,0,100), 0.f, 0, 1.5f);
    dL->AddRect(ImVec2(x, y), ImVec2(x+w, y+h), color, 0.f, 0, 1.25f);
    if (g_Config.shieldBars) { DrawHealthBar(dL, x, y, h, hp, max_hp); DrawShieldBar(dL, x, y, h, shield, max_shield); }
}

// Corner Box 
static void DrawBoxCorner(ImDrawList* dL, float x, float y, float w, float h,
                          ImColor color, int hp, int max_hp, int shield, int max_shield)
{
    float clW = w / 3.f;
    float clH = h / 3.f;
    ImColor shadow = ImColor(0, 0, 0, 100);
    auto S = [&](ImVec2 a, ImVec2 b){ dL->AddLine(ImVec2(a.x+1,a.y+1), ImVec2(b.x+1,b.y+1), shadow, 2.f); };
    S({x,y},{x+clW,y}); S({x,y},{x,y+clH});
    S({x+w,y},{x+w-clW,y}); S({x+w,y},{x+w,y+clH});
    S({x,y+h},{x+clW,y+h}); S({x,y+h},{x,y+h-clH});
    S({x+w,y+h},{x+w-clW,y+h}); S({x+w,y+h},{x+w,y+h-clH});

    dL->AddLine(ImVec2(x,y), ImVec2(x+clW,y), color, 2.f); dL->AddLine(ImVec2(x,y), ImVec2(x,y+clH), color, 2.f);
    dL->AddLine(ImVec2(x+w,y), ImVec2(x+w-clW,y), color, 2.f); dL->AddLine(ImVec2(x+w,y), ImVec2(x+w,y+clH), color, 2.f);
    dL->AddLine(ImVec2(x,y+h), ImVec2(x+clW,y+h), color, 2.f); dL->AddLine(ImVec2(x,y+h), ImVec2(x,y+h-clH), color, 2.f);
    dL->AddLine(ImVec2(x+w,y+h), ImVec2(x+w-clW,y+h), color, 2.f); dL->AddLine(ImVec2(x+w,y+h), ImVec2(x+w,y+h-clH), color, 2.f);
    if (g_Config.shieldBars) { DrawHealthBar(dL, x, y, h, hp, max_hp); DrawShieldBar(dL, x, y, h, shield, max_shield); }
}

//Side Lines
static void DrawBoxSide(ImDrawList* dL, float x, float y, float w, float h,
                        ImColor color, int hp, int max_hp, int shield, int max_shield)
{
    float cap = w * 0.18f;
    ImColor shadow = ImColor(0, 0, 0, 100);
    dL->AddLine(ImVec2(x+1, y+1), ImVec2(x+1, y+h+1), shadow, 2.5f);
    dL->AddLine(ImVec2(x+w+1, y+1), ImVec2(x+w+1, y+h+1), shadow, 2.5f);
    dL->AddLine(ImVec2(x, y), ImVec2(x, y+h), color, 2.f);
    dL->AddLine(ImVec2(x+w, y), ImVec2(x+w, y+h), color, 2.f);
    dL->AddLine(ImVec2(x, y), ImVec2(x+cap, y), color, 1.5f);
    dL->AddLine(ImVec2(x+w-cap, y), ImVec2(x+w, y), color, 1.5f);
    dL->AddLine(ImVec2(x, y+h), ImVec2(x+cap, y+h), color, 1.5f);
    dL->AddLine(ImVec2(x+w-cap, y+h), ImVec2(x+w, y+h), color, 1.5f);
    if (g_Config.shieldBars) { DrawHealthBar(dL, x, y, h, hp, max_hp); DrawShieldBar(dL, x, y, h, shield, max_shield); }
}

//Diamond Corners
static void DrawBoxDiamond(ImDrawList* dL, float x, float y, float w, float h,
                           ImColor color, int hp, int max_hp, int shield, int max_shield)
{
    ImColor dimColor = ImColor(color.Value.x, color.Value.y, color.Value.z, 0.35f);
    dL->AddRect(ImVec2(x+1,y+1), ImVec2(x+w+1,y+h+1), ImColor(0,0,0,100), 0.f, 0, 1.5f);
    dL->AddRect(ImVec2(x,y), ImVec2(x+w,y+h), dimColor, 0.f, 0, 1.f);
    float cx = x + w*0.5f, cy = y + h*0.5f, cs = 5.f;
    dL->AddLine(ImVec2(cx-cs,cy), ImVec2(cx+cs,cy), dimColor, 1.f); dL->AddLine(ImVec2(cx,cy-cs), ImVec2(cx,cy+cs), dimColor, 1.f);
    float d = 4.f;
    for (auto p : std::initializer_list<ImVec2>{{x,y}, {x+w,y}, {x,y+h}, {x+w,y+h}}) {
        ImVec2 pts[4] = {{p.x, p.y-d}, {p.x+d, p.y}, {p.x, p.y+d}, {p.x-d, p.y}};
        dL->AddPolyline(pts, 4, color, ImDrawFlags_Closed, 1.5f);
    }
    if (g_Config.shieldBars) { DrawHealthBar(dL, x, y, h, hp, max_hp); DrawShieldBar(dL, x, y, h, shield, max_shield); }
}

//Neon Glow Box
static void DrawBoxNeon(ImDrawList* dL, float x, float y, float w, float h,
                        ImColor color, int hp, int max_hp, int shield, int max_shield)
{
    ImColor glow = ImColor(color.Value.x, color.Value.y, color.Value.z, 0.25f);
    dL->AddRect(ImVec2(x-1, y-1), ImVec2(x+w+1, y+h+1), glow, 0.f, 0, 4.f);
    dL->AddRect(ImVec2(x-2, y-2), ImVec2(x+w+2, y+h+2), ImColor(color.Value.x, color.Value.y, color.Value.z, 0.1f), 0.f, 0, 6.f);
    dL->AddRect(ImVec2(x, y), ImVec2(x+w, y+h), color, 0.f, 0, 1.f);
    if (g_Config.shieldBars) { DrawHealthBar(dL, x, y, h, hp, max_hp); DrawShieldBar(dL, x, y, h, shield, max_shield); }
}




struct SmoothBox { float x = 0, y = 0, w = 0, h = 0; bool alive = false; };
static std::unordered_map<uint64_t, SmoothBox> g_SmoothBoxes;
static void DrawSkeleton(ImDrawList* dL, const CachedEntity& ce,
                           const BoneMatrix* bones, const Matrix& vm, ImColor color)
{
    static const int links[][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 },            // Spine
        { 1, 6 }, { 6, 7 }, { 7, 8 },                                // Left arm
        { 1, 9 }, { 9, 10 }, { 10, 11 },                             // Right arm
        { 5, 12 }, { 12, 13 }, { 13, 14 },                           // Left leg
        { 5, 16 }, { 16, 17 }, { 17, 18 }                            // Right leg
    };

    for (auto& lk : links) {
        int bi1 = ce.boneIndices[lk[0]];
        int bi2 = ce.boneIndices[lk[1]];
        if (bi1 < 0 || bi2 < 0 || bi1 > 255 || bi2 > 255) continue;

        Vector3 a = GetBonePositionFromMatrix(bones[bi1], ce.position);
        Vector3 b = GetBonePositionFromMatrix(bones[bi2], ce.position);
        if (a.x == 0.f && a.y == 0.f && a.z == 0.f) continue;
        if (b.x == 0.f && b.y == 0.f && b.z == 0.f) continue;
        Vector3 sa = _WorldToScreen(a, vm, ScreenSize);
        Vector3 sb = _WorldToScreen(b, vm, ScreenSize);
        if (sa.z <= 0.f || sb.z <= 0.f) continue;
        dL->AddLine(ImVec2(sa.x + 1, sa.y + 1), ImVec2(sb.x + 1, sb.y + 1), ImColor(0, 0, 0, 150), 2.0f);
        dL->AddLine(ImVec2(sa.x, sa.y), ImVec2(sb.x, sb.y), color, 1.25f);
    }
}

#define ESP_BATCH    10
#define ESP_SAMPLES   8

void RenderESP(const Matrix& vm)
{
    if (!g_Config.espEnabled) return;

    uint64_t lp = I::Read<uint64_t>(GameBase + OFFSET_LOCAL_PLAYER);
    if (!lp) return;

    int      ltTeam  = I::Read<int>(lp + OFFSET_TEAM_NUMBER);
    Vector3  localPos= I::Read<Vector3>(lp + OFFSET_CAMERA_ORIGIN);

    ImDrawList* dL = ImGui::GetBackgroundDrawList();
    int frameEntityCount = 0;

    const auto& entities = EntityManager::GetEntities();
    for (const auto& d : entities) {
        if (d.teamNumber == ltTeam) continue;
        if (d.distance * 39.62f > g_Config.maxDistance) continue;
        if (d.bleedoutState >= 2) continue;

        bool  isKnocked = (d.bleedoutState == 1);
        float health    = d.health;
        int   shield    = d.shield;

        bool isVisible = d.isVisible;

        ImColor boxColor = isVisible ? ImColor(g_Config.colorBoxVisible)
                                     : ImColor(g_Config.colorBoxNotVisible);

            Vector3 head3d = { 0,0,0 };
            if (d.bonesInitialized && d.boneIndices[BONE_HEAD] != -1) {
                head3d = GetBonePosition(d.ptr, d.boneIndices[BONE_HEAD], d.position);
            }

            if (head3d.x == 0.f) {
                head3d = d.position;
                head3d.z += isKnocked ? 35.f : 70.f;
            } else {
                head3d.z += 10.f;
            }

            Vector3 sf = _WorldToScreen(d.position, vm, ScreenSize);
            Vector3 sh = _WorldToScreen(head3d,    vm, ScreenSize);
            if (sf.z <= 0.f || sh.z <= 0.f) continue;

            float bh = sf.y - sh.y;
            float bw = bh * 0.55f;
            if (bh < 2.f) continue;

            float rawX = sh.x - bw / 2.0f;
            float rawY = sh.y;
            float rawW = bw, rawH = bh;

            float dt    = ImGui::GetIO().DeltaTime;
            float alpha = 1.0f - expf(-18.0f * dt); 
            auto& sb = g_SmoothBoxes[d.ptr];
            if (!sb.alive) {
                sb.x = rawX; sb.y = rawY; sb.w = rawW; sb.h = rawH; sb.alive = true;
            } else {
                sb.x += (rawX - sb.x) * alpha;
                sb.y += (rawY - sb.y) * alpha;
                sb.w += (rawW - sb.w) * alpha;
                sb.h += (rawH - sb.h) * alpha;
            }
            float x = sb.x, y = sb.y, w = sb.w, h = sb.h;

            frameEntityCount++;


            if (g_Config.boneESP && d.bonesInitialized) {
                BoneMatrix BM[100]; 
                if (ReadBoneMatrix(d.ptr, BM, 100)) {
                    ImColor boneCol = ImColor(g_Config.colorBone);
                    DrawSkeleton(dL, d, BM, vm, boneCol);
                }
            }


            switch (g_Config.espBoxMode) {
                case 0: DrawBoxFull   (dL, x, y, w, h, boxColor, (int)d.health, d.maxHealth, d.shield, d.maxShield); break;
                case 1: DrawBoxCorner (dL, x, y, w, h, boxColor, (int)d.health, d.maxHealth, d.shield, d.maxShield); break;
                case 2: DrawBoxSide   (dL, x, y, w, h, boxColor, (int)d.health, d.maxHealth, d.shield, d.maxShield); break;
                case 3: DrawBoxDiamond(dL, x, y, w, h, boxColor, (int)d.health, d.maxHealth, d.shield, d.maxShield); break;
                case 4: DrawBoxNeon   (dL, x, y, w, h, boxColor, (int)d.health, d.maxHealth, d.shield, d.maxShield); break;
                default: DrawBoxFull  (dL, x, y, w, h, boxColor, (int)d.health, d.maxHealth, d.shield, d.maxShield); break;
            }

            if (g_Config.snaplines) {
                ImColor snapCol = isVisible ? ImColor(g_Config.colorSnapVisible)
                                            : ImColor(g_Config.colorSnapNotVisible);
                Vec2 orig, tgt;
                if (g_Config.snaplineMode == 0) {
                    orig = Vec2(ScreenSize.x/2.f, 0.f);
                    tgt  = Vec2(x+w/2.f, y);
                } else if (g_Config.snaplineMode == 1) {
                    orig = Vec2(ScreenSize.x/2.f, ScreenSize.y/2.f);
                    tgt  = sf.x < ScreenSize.x/2.f ? Vec2(x, y+h/2.f) : Vec2(x+w, y+h/2.f);
                } else {
                    orig = Vec2(ScreenSize.x/2.f, ScreenSize.y);
                    tgt  = Vec2(x+w/2.f, y+h);
                }
                dL->AddLine(ImVec2(orig.x,orig.y), ImVec2(tgt.x,tgt.y), snapCol, 1.f);
            }
    }
    g_DbgEntityCount = frameEntityCount;
}