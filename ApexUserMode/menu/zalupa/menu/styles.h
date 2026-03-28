#pragma once
#include "../../../imgui/imgui.h"

namespace RenderHelper {

	ImVec4 ApplyStylex0Why() {
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.98f);
		style.Colors[ImGuiCol_Border] = ImColor(177, 146, 204, 128);

		style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(177, 146, 204, 128);
		style.Colors[ImGuiCol_CheckMark] = ImColor(150, 88, 161, 255);

		style.Colors[ImGuiCol_Button] = ImVec4(0.52f, 0.24f, 0.86f, 0.32f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.30f, 0.98f, 0.55f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.30f, 0.98f, 1.00f);

		style.Colors[ImGuiCol_Separator] = ImColor(177, 146, 204, 128);

		style.WindowBorderSize = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.TabBorderSize = 0.0f;

		style.ScrollbarRounding = 12.0f;
		style.FrameRounding = 4.0f;
		style.ChildRounding = 12.0f;
		style.PopupRounding = 12.0f;
		style.GrabRounding = 8.0f;

		style.SeparatorTextBorderSize = 1.0f;

		style.ScrollbarSize = 4.0f;
		style.ScrollbarRounding = 12.0f;

		style.FramePadding = ImVec2(2.0f, 2.0f);
		style.WindowPadding = ImVec2(8.0f, 8.0f);

		return ImVec4(0.70f, 0.30f, 0.98f, 0.82f); // <- Complete fucking shit i hate it i fucking hate it how the fuck am i that stupid that i can't do it without this fucking bullshit
	}



	ImVec4 ApplyStyleUnc() {
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.98f);
		style.Colors[ImGuiCol_Border] = ImColor(146, 146, 146, 128);

		style.Colors[ImGuiCol_Text] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

		style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(190, 190, 190, 128);
		style.Colors[ImGuiCol_CheckMark] = ImColor(220, 220, 220, 255);

		style.Colors[ImGuiCol_Button] = ImVec4(0.24f, 0.24f, 0.24f, 0.32f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.55f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);


		style.Colors[ImGuiCol_Separator] = ImColor(177, 177, 177, 128);

		style.WindowBorderSize = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.TabBorderSize = 0.0f;

		style.ScrollbarRounding = 12.0f;
		style.FrameRounding = 4.0f;
		style.ChildRounding = 12.0f;
		style.PopupRounding = 12.0f;
		style.GrabRounding = 8.0f;

		style.SeparatorTextBorderSize = 1.0f;

		style.ScrollbarSize = 4.0f;
		style.ScrollbarRounding = 12.0f;

		style.FramePadding = ImVec2(2.0f, 2.0f);
		style.WindowPadding = ImVec2(8.0f, 8.0f);

		// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
		return ImVec4(0.65f, 0.65f, 0.65f, 0.82f); // <- Complete fucking shit i hate it i fucking hate it how the fuck am i that stupid that i can't do it without this fucking bullshit
	}

	ImVec4 ApplyStyleTest() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.ChildBorderSize = 0.0f;
		style.FrameBorderSize = 0.0f;
		style.WindowBorderSize = 0.0f;
		style.PopupBorderSize = 0.0f;

		style.WindowRounding = 4.0f;
		style.ChildRounding = 4.0f;
		style.FrameRounding = 4.0f;
		

		style.ButtonTextAlign = ImVec2(0.0f, 0.5);

		style.FramePadding = ImVec2(4.0f, 4.0f);
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.ItemSpacing = ImVec2(8.0f, 8.0f);

		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.055f, 0.055f, 0.997f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
		style.Colors[ImGuiCol_Border] = ImColor(146, 146, 146, 92);

		// style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.18f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.52f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 0.85f);

		style.Colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 0.78f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.82f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);

		style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(190, 190, 190, 128);
		style.Colors[ImGuiCol_CheckMark] = ImColor(220, 220, 220, 255);

		style.Colors[ImGuiCol_Button] = ImVec4(0.04f, 0.04f, 0.04f, 0.08f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.20f, 0.20f, 0.32f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 0.52f);

		// DARK style.Colors[ImGuiCol_Text] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
		style.Colors[ImGuiCol_Text] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

		style.Colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.5f, 0.5f, 0.3f);

		// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
		return ImVec4(0.2f, 0.2f, 0.2f, 0.82f); // <- Complete fucking shit i hate it i fucking hate it how the fuck am i that stupid that i can't do it without this fucking bullshit
	}
} // ns