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
		std::string appdata = getenv(crypt_str("APPDATA")) ? getenv(crypt_str("APPDATA")) : "";
		std::string path_resources = appdata + std::string(crypt_str("\\Unk\\"));

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImFontConfig fontcfg{};
		fontcfg.PixelSnapH = true;
		fontcfg.OversampleH = fontcfg.OversampleV = 4;
		
		// Load base font
		defaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 15.0f, &fontcfg, io.Fonts->GetGlyphRangesCyrillic());

		auto fonts_folder = path_resources + crypt_str("resources\\fonts\\");
		auto dinproF = fonts_folder + crypt_str("dinpro.ttf");
		
		// Check if custom font exists, otherwise fallback to default
		DWORD dwAttrib = GetFileAttributesA(dinproF.c_str());
		if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
			dinpro = io.Fonts->AddFontFromFileTTF(dinproF.c_str(), 20.0f, &fontcfg, io.Fonts->GetGlyphRangesCyrillic());
		} else {
			dinpro = defaultFont;
		}

		auto calibribF = "C:\\Windows\\Fonts\\calibrib.ttf";
		dwAttrib = GetFileAttributesA(calibribF);
		if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
			calibrib = io.Fonts->AddFontFromFileTTF(calibribF, 14.0f, &fontcfg, io.Fonts->GetGlyphRangesCyrillic());
		} else {
			calibrib = defaultFont;
		}

		if (!defaultFont) {
			// Extreme fallback if even arial is missing (unlikely on Windows)
			io.Fonts->AddFontDefault();
			defaultFont = io.Fonts->Fonts.back();
			if (!dinpro) dinpro = defaultFont;
			if (!calibrib) calibrib = defaultFont;
		}

		io.FontDefault = defaultFont;
	}
}