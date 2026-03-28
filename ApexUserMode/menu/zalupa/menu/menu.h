#pragma once
#pragma execution_character_set("utf-8")
#include "styles.h"
#include "../../../protect/crypt_str.h"
#include "customgui.h"
#include "localization.h"
#include "tabs/esp.h"

namespace Render {

	const ImVec2 TabSize = ImVec2(336.0f, 260.0f);
	const ImVec2 DummyTabSize = ImVec2(10.0f, 16.0f);
	const float TabSpacing = 346.0f;

	bool first = true;

	enum Tabs {
		TAB_LEGIT = 0,
		TAB_RAGE = 1,
		TAB_PLAYER = 2,
		TAB_ITEMS = 3,
		TAB_HUD = 4,
		TAB_VIEW = 5,
		TAB_WORLD = 6,
		TAB_LOCAL = 7,
		TAB_MAIN = 8,
		TAB_MOVEMENT = 9,
		TAB_CFG = 10,
	};

	enum SubTab {
		SUBTAB_ESP_ENEMIES = 0,
		SUBTAB_ESP_TEAMMATES = 1,
		SUBTAB_VIEW_LOCAL = 11,
		SUBTAB_VIEW_WORLD = 12
	};

	// ----------

	static int currentTab = 0;
	static int currentSubTab = 0;
	ImVec4 current_button_col;

	void LabelDivider(const char* label) {
		float baseFontSize = ImGui::GetFontSize(); // Usually 13.0f - 18.0f
		float targetSize = 12.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10)); // Extra space before label
		ImGui::SetWindowFontScale(targetSize / baseFontSize);

		ImGui::TextColored({ 0.4f, 0.4f, 0.4f, 1.0f }, label);

		ImGui::SetWindowFontScale(1.0f); // Reset scale immediately
		ImGui::PopStyleVar();
	};

	bool tabBtn(const char* label, int tabIdx, void* tex = nullptr, bool topBar = false) {
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiStorage* storage = ImGui::GetStateStorage();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		float dt = ImGui::GetIO().DeltaTime;
		if (dt >= 0.1)
			dt = 0.01;

		ImGuiID anim_min_id = ImGui::GetID(topBar ? "##tab_min_x" : "##tab_min_y");
		ImGuiID anim_max_id = ImGui::GetID(topBar ? "##tab_max_x" : "##tab_max_y");

		ImGui::PushID(label);

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 base_sz = topBar ? ImVec2(80.0f, 24.0f) : ImVec2(206.0f, 35.0f);
		bool isSelectedTab = topBar ? (currentSubTab == tabIdx) : (currentTab == tabIdx);
		bool isClicked = ImGui::InvisibleButton("##btn", base_sz);
		bool isHovered = ImGui::IsItemHovered();

		if (isClicked) {
			if (topBar) currentSubTab = tabIdx;
			else currentTab = tabIdx;
		}

		// 2. ANIMATIONS
		// Existing Hover animation for scaling/BG
		ImGuiID hover_id = ImGui::GetID("##h_val");
		float hover_t = storage->GetFloat(hover_id, 0.0f);
		hover_t = ImLerp(hover_t, isHovered ? 1.0f : 0.0f, dt * 12.0f);
		storage->SetFloat(hover_id, hover_t);

		// --- NEW: Text Highlight Animation ---
		ImGuiID text_hl_id = ImGui::GetID("##text_highlight_t");
		float text_hl_t = storage->GetFloat(text_hl_id, 0.0f);
		// Condition: Highlight if hovered OR selected
		bool shouldHighlight = (isHovered || isSelectedTab);
		// We use a slightly slower multiplier (e.g., 12.0f) for that "slow highlight" feel
		text_hl_t = ImLerp(text_hl_t, shouldHighlight ? 1.0f : 0.0f, dt * 12.0f);
		storage->SetFloat(text_hl_id, text_hl_t);

		float scale_pad = hover_t * 1.2f;
		if (scale_pad <= 0) scale_pad = 1;

		// BG Color (Original hover logic)
		ImU32 bg_col = ImGui::GetColorU32(ImLerp(style.Colors[ImGuiCol_Button], current_button_col, hover_t));

		// --- UPDATED: Text Color Lerp ---
		ImVec4 base_text = style.Colors[ImGuiCol_Text];
		ImVec4 bright_text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Pure White highlight
		ImU32 text_col = ImGui::GetColorU32(ImLerp(base_text, bright_text, text_hl_t));

		// Calculate scaled font size
		ImFont* font = ImGui::GetFont();
		float current_font_size = ImGui::GetFontSize() + scale_pad;
		ImVec2 textSz = ImGui::CalcTextSize(label);

		// 3. DRAWING
		if (topBar) {
			drawList->AddRectFilled(ImVec2(p.x - scale_pad, p.y - scale_pad),
				ImVec2(p.x + base_sz.x + scale_pad, p.y + base_sz.y + scale_pad),
				bg_col, style.FrameRounding);

			ImVec2 textPos = ImVec2(p.x + (base_sz.x - textSz.x) * 0.5f, p.y + (base_sz.y - textSz.y) * 0.5f);
			drawList->AddText(font, current_font_size, textPos, text_col, label);
		}
		else {
			float bar_w = 3.0f;
			float gap = 6.0f;
			ImVec2 bg_min = ImVec2(p.x + bar_w + gap, p.y);
			ImVec2 bg_max = ImVec2(p.x + base_sz.x, p.y + base_sz.y);

			drawList->AddRectFilled(ImVec2(bg_min.x - scale_pad, bg_min.y - scale_pad),
				ImVec2(bg_max.x + scale_pad, bg_max.y + scale_pad),
				bg_col, style.FrameRounding);

			float text_x = bg_min.x + 12.0f;
			if (tex != nullptr) {
				float icon_base_sz = 18.0f;
				float current_icon_sz = icon_base_sz + scale_pad;
				float icon_center_x = bg_min.x + 10.0f + (icon_base_sz * 0.5f);
				float icon_center_y = bg_min.y + (base_sz.y * 0.5f);

				ImVec2 icon_p_min = ImVec2(icon_center_x - (current_icon_sz * 0.5f), icon_center_y - (current_icon_sz * 0.5f));
				ImVec2 icon_p_max = ImVec2(icon_center_x + (current_icon_sz * 0.5f), icon_center_y + (current_icon_sz * 0.5f));

				// Optional: apply text_col/alpha to icon if it's a tintable texture
				drawList->AddImage(tex, icon_p_min, icon_p_max, ImVec2(0, 0), ImVec2(1, 1), text_col);
				text_x = bg_min.x + 35.0f + (scale_pad * 0.5f);
			}

			drawList->AddText(font, current_font_size, ImVec2(text_x, bg_min.y + (base_sz.y - textSz.y) * 0.5f), text_col, label);
		}

		// 4. THE GOOEY BAR
		if (isSelectedTab) {
			float pad = 8.0f;
			float target_min = topBar ? (p.x + pad) : (p.y + pad);
			float target_max = topBar ? (p.x + base_sz.x - pad) : (p.y + base_sz.y - pad);

			float cur_min = storage->GetFloat(anim_min_id, target_min);
			float cur_max = storage->GetFloat(anim_max_id, target_max);

			// Determine direction: faster speed for the "lead" edge, slower for "trail"
			float lead_speed = 25.0f;
			float trail_speed = 10.0f; // Slightly slower for a more "gooey" lag

			float min_speed = (target_min < cur_min) ? lead_speed : trail_speed;
			float max_speed = (target_max > cur_max) ? lead_speed : trail_speed;

			cur_min = ImLerp(cur_min, target_min, dt * min_speed);
			cur_max = ImLerp(cur_max, target_max, dt * max_speed);

			storage->SetFloat(anim_min_id, cur_min);
			storage->SetFloat(anim_max_id, cur_max);

			ImU32 bar_col = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (topBar) {
				drawList->AddRectFilled(ImVec2(cur_min, p.y + base_sz.y + 1.0f),
					ImVec2(cur_max, p.y + base_sz.y + 3.0f), bar_col, 10.0f);
			}
			else {
				drawList->AddRectFilled(ImVec2(p.x, cur_min),
					ImVec2(p.x + 3.0f, cur_max), bar_col, 10.0f);
			}
		}

		ImGui::PopID();
		return isClicked;
	}

	void tabText(const char* name, const char* description) {
		ImGui::SameLine();
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::Text(name);
		ui::smallSpace();
		ImGui::SetCursorScreenPos({ p.x ,p.y + ImGui::CalcTextSize(description).y });
		ImGui::TextColored({ 0.5f, 0.5f, 0.5f, 1.0f }, description);
	}

	void RenderMenu() {
		
		current_button_col = RenderHelper::ApplyStyleTest();
		ImGui::SetNextWindowSize(ImVec2(898.f, 600.f), ImGuiCond_Once);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;

		ImGui::Begin(crypt_str("Hello"), nullptr, flags);

		ImGui::BeginChild(crypt_str("##sidebar"), ImVec2(212.0f, 0));
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);
			ImGui::Spacing();
			ImGui::ImageWithBg(textures::logo_tex, ImVec2(145.0f, 72.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0));
			ImGui::Separator();

			LabelDivider(crypt_str("AIMBOT"));
			tabBtn(Text::Legittab_text[Text::lang], TAB_LEGIT, textures::gear_tex);
			LabelDivider(crypt_str("VISUALS"));
			if (tabBtn(Text::Playertab_text[Text::lang], TAB_PLAYER, textures::gear_tex))
			{
				if (currentSubTab > 3)
					currentSubTab = 0;
			}
			if (tabBtn(Text::Viewtab_text[Text::lang], TAB_VIEW, textures::logo_tex)) {
				if (currentSubTab < 3)
					currentSubTab = 11;
			}
			LabelDivider(crypt_str("SETTINGS"));
			tabBtn(Text::Maintab_text[Text::lang], TAB_MAIN, textures::logo_tex);
			tabBtn(Text::ConfigTab_text[Text::lang], TAB_CFG, textures::med_tex);

			ImGui::SetCursorPosY(512.0f);

			ImGui::Separator();
			ImVec2 sz_user = ImVec2(148, 65);
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImVec4 col = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
			ImVec4 col_text = ImGui::GetStyle().Colors[ImGuiCol_Text];

			ImGui::GetWindowDrawList()->AddRectFilled({ p.x + 8, p.y }, { p.x + 204, p.y + 56 }, ImGui::GetColorU32(col), 10.0f);
			ui::smallSpace();
			ImGui::SetCursorScreenPos({ p.x + 14, p.y + 8 });
			ImGui::ImageWithBg(textures::gear_tex, { 40, 40 });

			ImGui::GetWindowDrawList()->AddText(fonts::defaultFont, 15.0f, { p.x + 48 + 16, p.y + 6 + 4 }, IM_COL32_WHITE, "DmitriyUlyanov");
			ImGui::GetWindowDrawList()->AddText(fonts::defaultFont, 12.0f, { p.x + 48 + 16, p.y + 6 + 18 + 4 }, ImGui::GetColorU32(col_text), "Sub: 52");
			char buf_session[32]; // Buffer to hold the formatted string
			snprintf(buf_session, sizeof(buf_session), "Session %d:%02d", (int)ImGui::GetTime() / 60, (int)ImGui::GetTime() % 60);
			ImGui::GetWindowDrawList()->AddText(fonts::defaultFont, 12.0f, { p.x + 48 + 16, p.y + 6 + 30 + 4 }, ImGui::GetColorU32(col_text), buf_session);

		}

		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		ImGui::BeginChild(crypt_str("##TopBar"), ImVec2(0.0f, 96.0f), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
			if (currentTab == TAB_PLAYER) {
				ui::space();
				ImGui::ImageWithBg(textures::gear_tex, { 32, 32 });
				tabText(crypt_str("Wallhack"), crypt_str("Proivdes extra-sextory powers to help you dominate in game"));

				tabBtn(crypt_str("Enemies"), SUBTAB_ESP_ENEMIES, nullptr, true);
				ImGui::SameLine();
				tabBtn(crypt_str("Team"), SUBTAB_ESP_TEAMMATES, nullptr, true);
			}
			else if (currentTab == TAB_VIEW) {
				ui::space();
				ImGui::ImageWithBg(textures::gear_tex, { 32, 32 });
				tabText(crypt_str("Visuals"), crypt_str("Make game look better"));

				tabBtn(crypt_str("Local"), SUBTAB_VIEW_LOCAL, nullptr, true);
				ImGui::SameLine();
				tabBtn(crypt_str("World"), SUBTAB_VIEW_WORLD, nullptr, true);
			}
			ImGui::Separator();

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
		ImGui::SetCursorPos({ 232.0f, 108.0f });

		ImGui::BeginChild(crypt_str("##content"), ImVec2(0, 0));
		{

			tab::esp();

		}
		ImGui::EndChild();

		ImGui::End();
	}
}