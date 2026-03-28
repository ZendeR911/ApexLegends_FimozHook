#pragma once
#include <cstdint>

const uint64_t OFFSET_LOCAL_PLAYER = 0x2562748;   // LocalPlayer
const uint64_t OFFSET_ENTITY_LIST = 0x612b428;   // EntityList
const uint64_t OFFSET_VIEW_RENDER = 0x3c72770;   // ViewRender ptr
const uint64_t OFFSET_VIEW_MATRIX = 0x11a350;    // ViewMatrix (world-to-screen) — unchanged
const uint64_t OFFSET_OBSERVER_LIST = 0x612d448;   // observerList
const uint64_t OFFSET_FLAGS = 0xc8; // m_fFlags
const uint64_t OFFSET_VELOCITY = 0x378; // m_vecVelocity
const uint64_t OFFSET_IN_ATTACK = 0x3c74198;   // +attack
const uint64_t OFFSET_IN_JUMP = 0x3c74290;   // +jump
const uint64_t OFFSET_IN_DUCK = 0x3c74388;   // +duck
const uint64_t OFFSET_IN_FORWARD = 0x3c743c8; // +forward
const uint64_t OFFSET_MANTLE_BOOST_STATE = 0x2c04; // m_mantleBoostState
const uint64_t OFFSET_ACTIVATE_GLIDE = 0x303c; // m_activateGlide
const uint64_t OFFSET_HIGHLIGHT_SETTING = 0x6875130;   // highlightSetting
const uint64_t OFFSET_GLOW_HIGHLIGHTS_TABLE = 0x6875130;   // same table


const uint64_t OFFSET_ORIGIN = 0x17C;       // m_vecAbsOrigin
const uint64_t OFFSET_VecAbsOrigin = 0x17C;
const uint64_t OFFSET_CAMERA_ORIGIN = 0x1fc4;      // camera_origin
const uint64_t OFFSET_TEAM_NUMBER = 0x334;       // m_iTeamNum
const uint64_t OFFSET_SQUAD_NUMBER = 0x340;       // m_squadID
const uint64_t OFFSET_HEALTH = 0x324;       // m_iHealth
const uint64_t OFFSET_MAX_HEALTH = 0x468;       // m_iMaxHealth
const uint64_t OFFSET_SHIELD = 0x1a0;       // m_shieldHealth
const uint64_t OFFSET_SHIELD_MAX = 0x1a4;       // m_shieldHealthMax
const uint64_t OFFSET_ARMOR_TYPE = 0x4854;      // m_armorType
const uint64_t OFFSET_LIFE_STATE = 0x690;       // m_lifeState
const uint64_t OFFSET_BLEEDOUT_STATE = 0x27c0;      // m_bleedoutState
const uint64_t OFFSET_LAST_VISIBLE_TIME = 0x1a64;      // lastVisibleTime
const uint64_t OFFSET_TIME_BASE = 0x2168;      // timeBase
const uint64_t OFFSET_VIEW_ANGLES = 0x2600;      // view angles (m_ammoPoolCapacity 0x2614 - 0x14)
const uint64_t OFFSET_PUNCH_ANGLES = 0x2518;      // m_vecPunchWeapon_Angle
const uint64_t OFFSET_ZOOM_FOV = 0x170C;      // m_targetZoomFOV
const uint64_t OFFSET_WEAPON_HANDLE = 0x19d4;      // weapon handle
const uint64_t OFFSET_VIEW_MODELS = 0x2e00;      // m_hViewModels
const uint64_t OFFSET_SIGNIFIER_NAME = 0x470;       // m_iSignifierName
const uint64_t OFFSET_NAME = 0x479;       // name
const uint64_t OFFSET_XP = 0x3824;      // m_xp
const uint64_t OFFSET_PITCH_Y = 0x24;      // m_kickScaleBasePitch
const uint64_t OFFSET_PITCH_X = 0x28;     //m_kickScaleBaseYaw 
const uint64_t OFFSET_FOV = 0x9c0;   //m_ifov


const uint64_t OFFSET_HIGHLIGHTSETTING = 0x6875130; //highlightSetting
const uint64_t OFFSET_HIGHLIGHT_DRAW = 0x1e30680; //highlight_draw
const uint64_t OFFSET_HIGHLIGHT_TEAM_BITS = 0x1f8; //m_highlightTeamBits
const uint64_t OFFSET_HIGHLIGHT_TEAM_INDEX = 0x1e8; //m_highlightTeamIndex     
const uint64_t OFFSET_HIGHLIGHT_CONTEXT = 0x299; //m_highlightGenericContexts
const uint64_t OFFSET_HIGHLIGHT_FADE_DURATION = 0x2a4; //m_highlightFadeDuration  


const uint64_t OFFSET_BONE = 0xE00;       
const uint64_t OFFSET_STUDIO_HDR = 0x1000;

const uint32_t BONE_HEAD = 0;
const uint32_t BONE_NECK = 1;
const uint32_t BONE_UPPER_CHEST = 2;
const uint32_t BONE_LOWER_CHEST = 3;
const uint32_t BONE_STOMACH = 4;
const uint32_t BONE_HIP = 5;
const uint32_t BONE_LEFT_SHOULDER = 6;
const uint32_t BONE_LEFT_ELBOW = 7;
const uint32_t BONE_LEFT_HAND = 8;
const uint32_t BONE_RIGHT_SHOULDER = 9;
const uint32_t BONE_RIGHT_ELBOW = 10;
const uint32_t BONE_RIGHT_HAND = 11;
const uint32_t BONE_LEFT_THIGH = 12;
const uint32_t BONE_LEFT_KNEE = 13;
const uint32_t BONE_LEFT_FOOT = 14;
const uint32_t BONE_RIGHT_THIGH = 16;
const uint32_t BONE_RIGHT_KNEE = 17;
const uint32_t BONE_RIGHT_FOOT = 18;

const uint32_t BONE_MATRIX_SIZE = 0x30;
const uint32_t BONE_X_OFFSET = 0xCC;
const uint32_t BONE_Y_OFFSET = 0xDC;
const uint32_t BONE_Z_OFFSET = 0xEC;

//Spectator 
const uint64_t OFFSET_OBSERVER_INDEX = 0x968;       // m_playerObserver

//Model 
const uint64_t OFFSET_MODEL_NAME = 0x30;

//PSilent
const uint64_t OFFSET_NET_CHANNEL = 0x1d0abc0;   // netChannel

//Legacy aliases
const uint64_t OFF_ENTITYLIST = OFFSET_ENTITY_LIST;
const uint64_t OFF_VIEW_RENDER = OFFSET_VIEW_RENDER;
const uint64_t OFF_VIEW_MATRIX = OFFSET_VIEW_MATRIX;
const uint64_t OFF_VecAbsOrigin = OFFSET_VecAbsOrigin;
const uint64_t OFF_iHealth = OFFSET_HEALTH;
const uint64_t OFF_iMaxHealth = OFFSET_MAX_HEALTH;
