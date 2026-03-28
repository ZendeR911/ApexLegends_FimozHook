#pragma once
#include "../../../Imgui/imgui.h"
#include "../../../Imgui/imgui_internal.h"
#include "../custom/requiemRender.h"
#include "../keybind.h"
#include "../../../protect/crypt_str.h"
#include <unordered_map>
#include <string>
#include <format>
#include <vector>
#include "localization.h"
#include <math.h>
#include "../textures.h"
#include "../../../features/config.h"

namespace ui {
	inline const char* hitboxes_list[] = { "Head", "Penis", "Legs" };
	inline const char* smooth_type[] = { "Increase", "Decrease", "Random" };
	inline const char* boxStyles[] = { "Deafult", "Outline", "Corner", "Filled" };

	struct Option {
		const char* name;
		bool selected = false;
	};

	inline void smallSpace() {
		ImGui::Spacing();
		ImGui::SameLine();
	}

	inline void space() {
		ImGui::Spacing(); // - Those allow for some spacing from the corners of BeginChild, idk if i wanna use them or keep no spacing
		ImGui::Spacing(); // - Those allow for some spacing from the corners of BeginChild, idk if i wanna use them or keep no spacing
		ImGui::SameLine(); // - Those allow for some spacing from the corners of BeginChild, idk if i wanna use them or keep no spacing
	}

	inline void TextDescription(const char* label, const char* description, bool spacing = true) {
		ImVec2 p_t = ImGui::GetCursorScreenPos();
		ImGui::Text(label);
		smallSpace();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 - ImGui::CalcTextSize(description).y);
		ImGui::TextColored({ 0.65f, 0.65f, 0.65f, 1.0f }, description);
	}

	inline bool Checkbox(const char* label, bool* v, const char* description = nullptr, bool spacing = true, bool menu = true) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiID id = window->GetID(label);
		ImGui::PushID(label);

		float height = ImGui::GetFrameHeight();
		float width = height * 1.25f;
		float radius = height * 0.25f;

		// --- DYNAMIC ALIGNMENT ---
		const float RIGHT_OFFSET = 15.0f;
		float right_edge_abs = ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x;

		ImVec2 p = ImVec2(right_edge_abs - width - RIGHT_OFFSET, ImGui::GetCursorScreenPos().y + 1.0f);
		if (!menu) {
			p = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 1.0f);
			p.x += 180; // Preserved your previous sub-menu offset logic
		}
		if (description != nullptr) p.y += 6;

		ImGui::Text("%s", label);
		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(p.x, p.y));

		ImGuiStorage* storage = ImGui::GetStateStorage();
		ImGuiID frame_id = ImGui::GetID("frame_c");

		float& t = *storage->GetFloatRef(id, *v ? 1.0f : 0.0f);
		int& last_frame = *storage->GetIntRef(frame_id, 0);

		if (last_frame < ImGui::GetFrameCount() - 1) t = 0.0f;
		last_frame = ImGui::GetFrameCount();

		float animation_speed = g.IO.DeltaTime * 8.0f;
		if (*v && t < 1.0f) t = ImMin(t + animation_speed, 1.0f);
		if (!*v && t > 0.0f) t = ImMax(t - animation_speed, 0.0f);

		ImVec4 col_bg_off = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
		ImVec4 col_bg_on = ImVec4(0.97f, 0.97f, 0.97f, 1.0f);
		ImVec4 col_knob_off = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		ImVec4 col_knob_on = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

		auto LerpU32 = [](ImVec4 a, ImVec4 b, float t) {
			return ImGui::GetColorU32(ImVec4(
				a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t
			));
			};

		ImU32 mixed_bg = LerpU32(col_bg_off, col_bg_on, t);
		ImU32 mixed_knob = LerpU32(col_knob_off, col_knob_on, t);

		bool result = false;
		if (ImGui::InvisibleButton(label, ImVec2(width, height))) {
			*v = !*v;
			result = true;
		}

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + width + 4.0f, p.y + height * 0.7f), mixed_bg, height);
		draw_list->AddCircleFilled(ImVec2(p.x + 2.0f + radius + t * (width - radius * 2.0f), p.y + height / 2.75f), radius, mixed_knob);

		if (description != nullptr) {
			smallSpace();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 - ImGui::CalcTextSize(description).y);
			ImGui::TextColored({ 0.5f, 0.5f, 0.5f, 1.0f }, description);
		}

		if (spacing) {
			ImGui::Spacing();
			ImGui::SameLine();
		}

		ImGui::PopID();
		return result;
	}

	inline void NewChildHeader(const char* label, ImVec2 size, void* tex = nullptr) {

		ImGuiStyle& style = ImGui::GetStyle();

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImU32 color_bg = ImGui::GetColorU32(ImVec4(0.45f, 0.45f, 0.45f, 0.7f));
		ImU32 color_header = ImGui::GetColorU32(ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

		ImU32 color_text = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		// draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), color_bg, style.FrameRounding);

		draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + size.x, p.y + 24), color_header, 0.0f);
		draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), color_bg, 0.0f);

		ImVec2 text_size = ImGui::CalcTextSize(label);
		ImVec2 text_pos = ImVec2(p.x + (size.x - text_size.x) * 0.02f, p.y + (24 - text_size.y) * 0.5f);

		if (tex != nullptr) {
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 8, ImGui::GetCursorPos().y + 4));
			ImGui::Image(tex, ImVec2(16, 16));
			ImGui::SameLine();
			text_pos = ImVec2(p.x + (size.x - text_size.x) * 0.02f + 24, p.y + (24 - text_size.y) * 0.5f);
		}


		draw_list->AddText(text_pos, color_text, label);

		ImGui::Dummy(ImVec2(size.x, 20));
		// ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.5f);

	}

	inline bool Slider(const char* label, float* v, float v_min, float v_max, const char* description = nullptr, bool menu = true) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		ImGui::PushID(label);

		// Label goes on the left
		ImGui::Text("%s", label);
		ImGui::SameLine();

		// Format text value so we can measure its width precisely
		char val_buf[32];
		snprintf(val_buf, sizeof(val_buf), "%.1f", *v);
		float text_width = ImGui::CalcTextSize(val_buf).x;

		// --- DYNAMIC ALIGNMENT ---
		const float RIGHT_OFFSET = 15.0f;
		const float TEXT_SPACING = 8.0f;
		float right_edge_abs = ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x;
		float middle_abs = ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x * 0.5f;

		// Dynamic width calculation guarantees it stretches perfectly between the middle and the right side text
		float slider_width = right_edge_abs - middle_abs - TEXT_SPACING - RIGHT_OFFSET - 24.0f;
		if (!menu) {
			middle_abs = ImGui::GetWindowPos().x + ImGui::CalcTextSize(label).x + 16.0f;
			slider_width = 180.0f; // Extra offset if outside standard menus
		}
		float height = 24.0f;
		ImVec2 p = ImVec2(middle_abs, ImGui::GetCursorScreenPos().y);
		p.y += 3;

		ImU32 color_bg = ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImU32 color_fill = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImU32 color_handle = ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));

		ImGui::SetCursorScreenPos(p);
		ImGui::InvisibleButton(label, ImVec2(slider_width, height));
		bool is_active = ImGui::IsItemActive();

		if (is_active) {
			float mouse_pos = ImGui::GetMousePos().x - p.x;
			*v = ImClamp(v_min + (v_max - v_min) * (mouse_pos / slider_width), v_min, v_max);
		}

		ImGuiID anim_id = ImGui::GetID("anim_v");
		ImGuiID frame_id = ImGui::GetID("frame_v");
		ImGuiStorage* storage = ImGui::GetStateStorage();

		float& v_anim = *storage->GetFloatRef(anim_id, v_min);
		int& last_frame = *storage->GetIntRef(frame_id, 0);

		if (last_frame < ImGui::GetFrameCount() - 1) v_anim = v_min;
		last_frame = ImGui::GetFrameCount();

		float animation_speed = g.IO.DeltaTime * 15.0f;
		v_anim += (*v - v_anim) * ImClamp(animation_speed, 0.0f, 1.0f);

		ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// --- Drawing the Slider Track ---
		draw_list->AddRectFilled(ImVec2(p.x, p.y + height * 0.25f),
			ImVec2(p.x + slider_width, p.y + height * 0.85f - 10.f),
			color_bg, style.FrameRounding);

		float fill_width = (v_anim - v_min) / (v_max - v_min) * slider_width;

		// Fill
		draw_list->AddRectFilled(ImVec2(p.x, p.y + height * 0.25f),
			ImVec2(p.x + fill_width, p.y + height * 0.85f - 10.f),
			color_fill, style.FrameRounding);

		// Handle
		float handle_x = p.x + fill_width;
		draw_list->AddCircleFilled(ImVec2(handle_x, p.y + height * 0.5f - 3.5f),
			height * 0.25f, color_handle);

		// --- Draw Value Text to the right ---
		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(p.x + slider_width + TEXT_SPACING, p.y));
		ImGui::Text("%s", val_buf);

		ImGui::PopID();
		return is_active;
	}

	inline bool Combo(const char* str_id, std::vector<Option>& options, bool multiselect, const char* description = nullptr)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		ImGuiID id = window->GetID(str_id);

		bool value_changed = false;
		std::string preview;

		if (multiselect) {
			for (const auto& option : options) {
				if (option.selected) preview.append(std::format("{} ", option.name));
			}
		}
		else {
			for (const auto& option : options) {
				if (option.selected) { preview = option.name; break; }
			}
		}
		if (preview.empty()) preview = "None";

		// --- DYNAMIC ALIGNMENT ---
		// Render the text label FIRST on the left to allow the combo box to snap to the middle cleanly
		if (str_id[0] != '#' || (str_id[0] == '#' && str_id[1] == '#')) {
			ImGui::Text("%s", str_id);
			ImGui::SameLine();
		}

		const float RIGHT_OFFSET = 15.0f;
		float right_edge_abs = ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x;
		float middle_abs = ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x * 0.5f;

		float width = right_edge_abs - middle_abs - RIGHT_OFFSET;
		float height = ImGui::GetFrameHeight();

		// Snap exact X coord to the middle of the pane
		ImVec2 p = ImVec2(middle_abs, ImGui::GetCursorScreenPos().y);
		ImRect bb(p, ImVec2(p.x + width, p.y + height));

		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id)) return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		char popup_name[64];
		sprintf_s(popup_name, "##p_%08X", id);
		ImGuiID popup_id = ImGui::GetID(popup_name);

		if (pressed) ImGui::OpenPopup(popup_name);

		// Draw Frame
		ImU32 frame_col = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		window->DrawList->AddRectFilled(bb.Min, bb.Max, frame_col, style.FrameRounding);

		ImVec2 pad_min = ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y);
		ImVec2 pad_max = ImVec2(bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y);

		ImGui::RenderTextClipped(pad_min, pad_max, preview.c_str(), NULL, NULL, ImVec2(0.0f, 0.5f));

		// Animation Logic
		float anim_speed = g.IO.DeltaTime * 12.0f;
		ImGuiStorage* storage = ImGui::GetStateStorage();
		float& p_t = *storage->GetFloatRef(popup_id, 0.0f);
		bool is_open = ImGui::IsPopupOpen(popup_name);

		if (is_open) p_t = ImMin(p_t + anim_speed, 1.0f);
		else p_t = ImMax(p_t - anim_speed, 0.0f);

		// Render Dropdown
		if (p_t > 0.005f) {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, p_t);
			float slide_offset = 5.0f * (1.0f - p_t);

			ImGui::SetNextWindowPos(ImVec2(bb.Min.x, bb.Max.y + 1.0f + slide_offset));
			ImGui::SetNextWindowSize(ImVec2(width, 0.0f));

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.WindowPadding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, style.PopupRounding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, style.PopupBorderSize);

			ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;

			bool should_draw = false;
			if (is_open) should_draw = ImGui::BeginPopupEx(popup_id, flags | ImGuiWindowFlags_Popup);
			else should_draw = ImGui::Begin(popup_name, nullptr, flags | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav);

			if (should_draw) {
				for (size_t i = 0; i < options.size(); i++) {
					if (ImGui::Selectable(options[i].name, options[i].selected, multiselect ? ImGuiSelectableFlags_DontClosePopups : 0)) {
						value_changed = true;
						if (multiselect) options[i].selected = !options[i].selected;
						else {
							for (auto& opt : options) opt.selected = false;
							options[i].selected = true;
							ImGui::CloseCurrentPopup();
						}
					}
				}
				if (is_open) ImGui::EndPopup();
				else ImGui::End();
			}
			ImGui::PopStyleVar(4);
		}

		return value_changed;
	}

	inline void Keybind(KeyBind& keybind, bool menu = true)
	{
		// 1. Get localized mode string
		std::string mode_text = keybind.isHoldMode ?
			Text::Hold_text[Text::lang] :
			Text::Toggle_text[Text::lang];

		if (menu) {
			// --- Left side text label ---
			ImGui::Text("%s", keybind.bindName.c_str());
			ImGui::SameLine();

			// --- Dynamic alignment (Snaps button to the middle of the pane) ---
			const float RIGHT_OFFSET = 15.0f;
			float right_edge_abs = ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x;
			float middle_abs = ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x * 0.5f;
			float width = right_edge_abs - middle_abs - RIGHT_OFFSET;

			ImVec2 p = ImVec2(middle_abs, ImGui::GetCursorScreenPos().y);
			ImGui::SetCursorScreenPos(p);

			// --- Button Text Logic ---
			std::string btn_text;
			if (keybind.isSelected)
				btn_text = Text::WaitingForInput_text[Text::lang];
			else
				btn_text = std::format("{} | {}", keybind.GetKeyName(), mode_text);

			// Append bindName to ID to prevent collisions if multiple keys have same text (e.g., "None")
			std::string btn_id = std::format("{}##{}", btn_text, keybind.bindName);

			if (ImGui::Button(btn_id.c_str(), ImVec2(width, 0))) {
				keybind.isSelected = !keybind.isSelected;
			}
		}
		else {
			// --- MENU = FALSE: Combined style (used in popups/submenus) ---
			std::string a1;
			if (keybind.isSelected)
				a1 = Text::WaitingForInput_text[Text::lang];
			else
				a1 = std::format("{0} : {1}, {2}", keybind.bindName, keybind.GetKeyName(), mode_text);

			if (ImGui::Button(a1.c_str())) {
				keybind.isSelected = !keybind.isSelected;
			}
		}

		// --- Interaction Logic (Works for both Menu and Non-Menu modes) ---

		// Right Click: Toggle between Hold and Toggle modes
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			keybind.isHoldMode = !keybind.isHoldMode;
		}

		// Middle Click: Reset the key to "None"
		if (ImGui::IsItemClicked(ImGuiMouseButton_Middle)) {
			keybind.Clear();
		}
	}

	inline bool Popup(const char* label, bool* v, const char* popup, const char* btn_label, bool check = true, const char* description = nullptr, bool spacing = true) {

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		ImGuiID id = window->GetID(label);
		ImGuiID actual_popup_id = ImGui::GetID(popup);

		ImGui::PushID(label);

		float height = ImGui::GetFrameHeight();
		float width = height * 1.25f;
		float radius = height * 0.25f;
		float anim_speed = g.IO.DeltaTime * 8.0f;
		void* tex = textures::arrows_tex;

		// --- DYNAMIC ALIGNMENT ---
		const float RIGHT_OFFSET = 15.0f;
		float right_edge_abs = ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x;

		// Set `p` aligned to the right edge minus offset and element width
		ImVec2 p = ImVec2(right_edge_abs - width - RIGHT_OFFSET, ImGui::GetCursorScreenPos().y + 1.0f);
		// Place the popup gear/button to the left of the main toggle
		ImVec2 p_p = ImVec2(p.x - 24.0f, ImGui::GetCursorScreenPos().y - 1.0f);

		if (description != nullptr) {
			p.y += 6;
			p_p.y += 6;
		}

		ImGui::Text("%s", label);
		ImGui::SameLine();
		ImGui::SetCursorScreenPos(p);
		bool result = false;

		// --- Persistent Animation & Tab Switch Logic ---
		ImGuiStorage* storage = ImGui::GetStateStorage();
		ImGuiID frame_id = ImGui::GetID("frame_state");

		float& t = *storage->GetFloatRef(id, *v ? 1.0f : 0.0f);
		ImGuiID icon_id = ImGui::GetID("##icon_hover");
		float& h_t = *storage->GetFloatRef(icon_id, 0.0f);
		int& last_frame = *storage->GetIntRef(frame_id, 0);

		if (last_frame < ImGui::GetFrameCount() - 1) {
			t = 0.0f;
			h_t = 0.0f;
		}
		last_frame = ImGui::GetFrameCount();

		// --- 1. Toggle Animation ---
		if (*v && t < 1.0f) t = ImMin(t + anim_speed, 1.0f);
		if (!*v && t > 0.0f) t = ImMax(t - anim_speed, 0.0f);

		if (check) {
			tex = textures::gear_tex;
			ImGui::InvisibleButton(label, ImVec2(width, height));
			if (ImGui::IsItemClicked()) {
				result = true;
				*v = !*v;
			}
		}

		// --- 2. Gear/Arrow Hover Lerp ---
		auto LerpVec4 = [](ImVec4 a, ImVec4 b, float t) {
			return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
			};

		ImU32 mixed_bg = ImGui::GetColorU32(LerpVec4(ImVec4(0.12f, 0.12f, 0.12f, 1.0f), ImVec4(0.97f, 0.97f, 0.97f, 1.0f), t));
		ImU32 mixed_knob = ImGui::GetColorU32(LerpVec4(ImVec4(0.35f, 0.35f, 0.35f, 1.0f), ImVec4(0.15f, 0.15f, 0.15f, 1.0f), t));

		if (!check) p_p = p;

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(p_p);

		if (ImGui::InvisibleButton(btn_label, ImVec2(20, 20))) {
			ImGui::OpenPopup(crypt_str("##shit"));
		}

		bool p_hov = ImGui::IsItemHovered();
		bool p_act = ImGui::IsItemActive();
		if (p_hov) h_t = ImMin(h_t + anim_speed, 1.0f);
		else h_t = ImMax(h_t - anim_speed, 0.0f);

		ImVec4 icon_tint = p_act ? ImVec4(1, 1, 1, 1) : LerpVec4(ImVec4(0.75f, 0.75f, 0.75f, 1.0f), ImVec4(0.87f, 0.87f, 0.87f, 1.0f), h_t);

		ImGui::SetCursorScreenPos(p_p);
		ImGui::ImageWithBg(tex, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), icon_tint);

		if (check) {
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(p, ImVec2(p.x + width + 4.0f, p.y + height * 0.7f), mixed_bg, height);
			draw_list->AddCircleFilled(ImVec2(p.x + 2.0f + radius + t * (width - radius * 2.0f), p.y + height / 2.75f), radius, mixed_knob);
		}

		// --- 3. Smooth Popup Animation ---
		ImGuiID popup_internal_id = ImGui::GetID(crypt_str("##shit"));
		float& p_t = *ImGui::GetStateStorage()->GetFloatRef(popup_internal_id, 0.0f);
		bool is_open = ImGui::IsPopupOpen(crypt_str("##shit"));

		if (is_open) p_t = ImMin(p_t + anim_speed, 1.0f);
		else p_t = ImMax(p_t - anim_speed, 0.0f);

		if (p_t > 0.01f) {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, p_t);
			ImGui::SetNextWindowPos(ImVec2(p_p.x, p_p.y + 25 + (8.0f * (1.0f - p_t))), ImGuiCond_Always);

			if (ImGui::BeginPopup(crypt_str("##shit"), ImGuiWindowFlags_NoMove)) {
				// Keeping all your custom popups identical internally
				ImGuiColorEditFlags flgs = ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoDragDrop;

				// strcmp here

				ImGui::EndPopup();
			}
			ImGui::PopStyleVar();
		}

		if (description != nullptr) {
			smallSpace();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2 - ImGui::CalcTextSize(description).y);
			ImGui::TextColored({ 0.5f, 0.5f, 0.5f, 1.0f }, description);
		}

		if (spacing) {
			ImGui::Spacing();
			ImGui::SameLine();
		}

		ImGui::PopID();
		return result;
	}

} // ns