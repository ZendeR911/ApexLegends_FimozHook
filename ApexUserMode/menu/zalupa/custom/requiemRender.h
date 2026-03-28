#pragma once
#include "../../../imgui/imgui.h"
#include "../../../imgui/imgui_internal.h"
#include <cmath>
#include "d3d11.h"

namespace Render {

	ID3D11ShaderResourceView* Image(unsigned char* image_data, size_t image_size);

	void DrawBox(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None, float thickness = 0);
	void DrawBoxFilled(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None);
	void DrawBoxFilledOutline(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None, float thickness = 0);
	void DrawBoxOutline(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None, float thickness = 0);

	void DrawBoxCorner(const ImVec2& p_min, const ImVec2& p_max, ImVec4 color, float thickness);
	void DrawBoxPartial(const ImVec2& p_min, const ImVec2& p_max, ImVec4 color, float thickness);

	void DrawLabel(const ImVec2 pos, const ImVec4 col, const char* text);
	void DrawLabelEx(ImFont* font, const float size, const ImVec2 pos, const ImVec4 col, const char* text);

	void DrawLabelFilled(const ImVec2 pos, const ImVec4 label_col, const ImVec4 fill_col, const char* text, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None);
	void DrawLabelFilledEx(ImFont* font, const float size, const ImVec2 pos, const ImVec4 label_col, const ImVec4 fill_col, const char* text, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None);

	inline float EaseOutCubic(float x)
	{
		return 1.f - std::pow(1.f - x, 3.f);
	}

	inline float EaseOutExpo(float x) {
		return 1.f - std::pow(2, - 10 * x);
	}

	inline auto Fade = [&](ImU32 col, float alpha)
		{
			ImVec4 c = ImGui::ColorConvertU32ToFloat4(col);
			c.w *= alpha;
			return ImGui::GetColorU32(c);
		};

}