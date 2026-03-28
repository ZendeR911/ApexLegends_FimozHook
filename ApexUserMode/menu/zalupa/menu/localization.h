#pragma once
#include "../../../protect/crypt_str.h"
#pragma execution_character_set("utf-8")

namespace Text {

	// Text


	//inline const char* Score_text[] = { "Score", "����", "Chinese" };
	inline const char* WMPH_text[] = { "�������� �����", "Waiting for match", "Chinese" };
	inline const char* Score_text[] = { "����", "Score", "Chinese" };
	inline const char* Ping_text[] = { "����", "Ping", "Chinese" };

	// General

	inline const char* thick_text[] = { "�������", "Thickness", "Chinsese" };
	inline const char* style_text[] = { "�����", "Style", "Chinese" };
	inline const char* enable_text[] = { "��������", "Enable", "Chinese" };
	inline const char* Hold_text[] = { "���������", "Hold", "Chinese" };
	inline const char* Toggle_text[] = { "������������", "Toggle", "Chinese" };
	inline const char* WaitingForInput_text[] = { "������� ����� ������", "Press any key", "Chinese" };

	// SubTabs
	inline const char* AimbotSubtab_text[] = { "���", "Aimbot", "Chinese" };
	inline const char* VisualSubtab_text[] = { "�������", "Visaul", "Chinese" };
	inline const char* MiscSubtab_text[] = { "������", "Misc", "Chinese" };

	// Tabs
	inline const char* Legittab_text[] = { "�����", "Legit", "Chinese" };
	inline const char* Playertab_text[] = { "�����", "Player", "Chinese" };
	inline const char* Viewtab_text[] = { "���", "View", "Chinese" };
	inline const char* Maintab_text[] = { "������", "Misc", "Chinese" };

	// ESP
	inline const char* ESP_text[] = { "��", "ESP", "Chinese" };
	inline const char* EnemiesESP_text[] = { "�����", "Enemies", "Chinese" };
	inline const char* TeamESP_text[] = { "�������", "Teammates", "Chinese" };
	inline const char* EnableESP_text[] = { "�������� ��", "Enable ESP", "Chinese" };
	inline const char* PlayerList_text[] = { "������ �������", "Playerlist", "Chinese" };
	inline const char* Box_text[] = { "�����", "Box", "Chinese" };
	inline const char* Health_text[] = { "������� ��������", "Health bar", "Chinese" };
	inline const char* Armor_text[] = { "������� ��������", "Armor bar", "Chinese" };
	inline const char* HealthChk_text[] = { "��������", "Health", "Chinese" };
	inline const char* ArmorChk_text[] = { "��������", "Armor", "Chinese" };
	inline const char* WeapChk_text[] = { "��������", "Weapon", "Chinese" };
	inline const char* Outline_text[] = { "�������", "Outline", "Chinese" };
	inline const char* Teammates_text[] = { "�������", "Teammates", "Chinese" };
	inline const char* Color_text[] = { "����", "Color", "Chinese" };
	inline const char* BoxThickness_text[] = { "������� ������", "Box Thickness", "Chinese" };
	inline const char* HealthThickness_text[] = { "������� �������", "Bar Thickness", "Chinese" };
	inline const char* Rounding_text[] = { "�����������", "Rounding", "Chinese" };
	inline const char* Flags_text[] = { "�����", "Flags", "Chinese" };
	inline const char* EnableHelathText_text[] = { "�����", "Text", "Chinese" };
	inline const char* BarColor_text[] = { "���� �������", "Bar color", "Chinese" };
	inline const char* EnableName_text[] = { "�����", "Names", "Chinese" };
	inline const char* EnablefillText_text[] = { "��� ������", "Text backgorund", "Chinese" };
	inline const char* NamesColor_text[] = { "����", "Color", "Chinese" };
	inline const char* WeaponColor_text[] = { "����", "Text Color", "Chinese" };
	inline const char* Ammo_text[] = { "����", "Ammo", "Chinese" };
	inline const char* AmmoBar_text[] = { "����", "Ammo Bar", "Chinese" };
	inline const char* Glow_text[] = { "���������", "Glow", "Chinese" };
	inline const char* Skeleton_text[] = { "������", "Skeleton", "Chinese" };

	inline const char* desc_esp_enenmies[] = { "�������� �� �� ������", "Toggles extra-sensory powers on enemies", "Chinese" };
	inline const char* desc_esp_teamates[] = { "�������� �� �� ��������", "Toggles extra-sensory powers on teammates", "Chinese" };

	inline const char* desc_names_enemies[] = { "�������� ����������� ���� � ������", "Toggle Name esp on enemies", "Chinese" };
	inline const char* desc_names_teammates[] = { "�������� ����������� ���� � ��������", "Toggle Name esp on teammates", "Chinese" };

	// Flags
	inline const char* fl_text[] = { "�����", "Flags", "Chinese" };
	inline const char* fl_Zoom_text[] = { "� �������", "Zoom", "Chinese" };
	inline const char* fl_Hk_text[] = { "����", "HK", "Chinese" };
	inline const char* fl_Defuse_text[] = { "������", "Defuse Kit", "Chinese" };
	inline const char* fl_Planting_text[] = { "�������", "Planting", "Chinese" };


	// Aim
	inline const char* AimWeapons_text[] = { "������", "Weapons", "Chinese" };

	inline const char* AimGlobal_text[] = { "������", "Default", "Chinese" };
	inline const char* AimKnife_text[] = { "������", "Knife", "Chinese" };
	inline const char* AimZeus_text[] = { "������", "Zeus", "Chinese" };
	inline const char* AimPistol_text[] = { "���������", "Pistols", "Chinese" };
	inline const char* AimSMGs_text[] = { "���������", "SMGs", "Chinese" };
	inline const char* AimRifles_text[] = { "���������", "Rifles", "Chinese" };
	inline const char* AimSinpers_text[] = { "���������", "Snipers", "Chinese" };
	inline const char* AimAuto_text[] = { "��������", "Auto-snipers", "Chinese" };
	inline const char* AimShotguns_text[] = { "��������", "Shotguns", "Chinese" };

	inline const char* AimBot_text[] = { "���", "Aimbot", "Chinese" };
	inline const char* aimHitboxes_text[] = { "��������", "Hitboxes", "Chinese" };
	inline const char* HeadHb_text[] = { "������", "Head", "Chinese" };
	inline const char* NeckHb_text[] = { "���", "Neck", "Chinese" };
	inline const char* TorsoHb_text[] = { "����", "Torso", "Chinese" };
	inline const char* PelvisHb_text[] = { "�����", "Pelvis", "Chinese" };
	inline const char* ArmsHb_text[] = { "����", "Arms", "Chinese" };
	inline const char* LegsHb_text[] = { "����", "Legs", "Chinese" };


	inline const char* AimSettings_text[] = { "���������", "Settings", "Chinese" };
	inline const char* EnableAim_text[] = { "Включить аим", "Enable aim", "Chinese" };
	inline const char* EnableAimTeam_text[] = { "На своих", "Team", "Chinese" };
	inline const char* AutoFire_text[] = { "Автовыстрел", "Autofire", "Chinese" };
	inline const char* AutoScope_text[] = { "Автоприцел", "Autoscope", "Chinese" };
	inline const char* VisibleCheck_text[] = { "Проверка видимости", "Visible Check", "Chinese" };
	inline const char* AimFov_text[] = { "Радиус", "Fov", "Chinese" };

	// Misc
	inline const char* MenuMisc_text[] = { "����", "Menu", "Chinese" };
	inline const char* Localization_text[] = { "����", "Language", "Chinese" };
	inline const char* Languages_text[] = { "�������", "English", "Chinese" };
	inline const char* Hotkeys_text[] = { "�������� �����", "Active hotkeys", "Chinese" };
	inline const char* Spectators_text[] = { "������ ������������", "Spectator list", "Chinese" };
	inline const char* Watermark_text[] = { "����������", "Watermark", "Chinese" };

	inline const char* Misc_text[] = { "������", "Miscellaneous", "Chinese" };

	// Config
	inline const char* ConfigTab_text[] = { "������", "Config", "Chinese" };
	inline const char* ConfigList_text[] = { "������ ��������", "Config List", "Chinese" };
	inline const char* ConfigActions_text[] = { "��������", "Actions", "Chinese" };
	inline const char* ConfigName_text[] = { "���", "Name", "Chinese" };
	inline const char* CreateConfig_text[] = { "�������", "Create", "Chinese" };
	inline const char* SaveConfig_text[] = { "���������", "Save", "Chinese" };
	inline const char* LoadConfig_text[] = { "���������", "Load", "Chinese" };
	inline const char* RefreshConfigs_text[] = { "��������", "Refresh", "Chinese" };

	enum languages {
		RUSSIAN = 0,
		ENGLISH = 1,
		CHINESE = 2
	};

	inline int lang = 1;

}