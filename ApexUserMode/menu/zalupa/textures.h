#pragma once
#include <d3d11.h>

#include "../../Imgui/imgui.h"
#include "../../protect/crypt_str.h"

#include "custom/requiemRender.h"
#include "icons.h"

namespace textures {
	inline ID3D11ShaderResourceView* test_tex;
	inline ID3D11ShaderResourceView* med_tex;
	inline ID3D11ShaderResourceView* arrows_tex;
	inline ID3D11ShaderResourceView* gear_tex;
	inline ID3D11ShaderResourceView* logo_tex;

	inline void load() {
		test_tex = Render::Image(test_data, sizeof(test_data));
		med_tex = Render::Image(medical_data, sizeof(medical_data));
		arrows_tex = Render::Image(arrows_data, sizeof(arrows_data));
		gear_tex = Render::Image(gear_data, sizeof(gear_data));
		logo_tex = Render::Image(logo_data, sizeof(logo_data));
	}
}

namespace fonts {

	inline ImFont* defaultFont = nullptr;
	inline ImFont* dinpro = nullptr;
	inline ImFont* calibrib = nullptr;

	inline void load() {
		std::string path_resources = getenv(crypt_str("APPDATA")) + std::string(crypt_str("\\Unk\\"));

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImFontConfig fontcfg{};
		fontcfg.PixelSnapH = true;
		fontcfg.OversampleH = fontcfg.OversampleV = 4;
		defaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 15.0f, &fontcfg, io.Fonts->GetGlyphRangesCyrillic());

		auto fonts_folder = path_resources + crypt_str("resources\\fonts\\");
		auto dinproF = fonts_folder + crypt_str("dinpro.ttf");

		dinpro = io.Fonts->AddFontFromFileTTF(dinproF.c_str(), 20.0f, &fontcfg, io.Fonts->GetGlyphRangesCyrillic());

		calibrib = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\calibrib.ttf", 14.0f, &fontcfg, io.Fonts->GetGlyphRangesCyrillic());
		io.FontDefault = defaultFont;
	}
}