#pragma once
#include "../customgui.h"

namespace tab {
	inline void esp() {
		ui::space();
		ImGui::BeginChild(crypt_str("esp tab"), { -1, 450 });

		ui::Checkbox(crypt_str("123"), &g_Config.espEnabled,  crypt_str("А когда чит на диеп.ио ???"));

		ImGui::EndChild();
	}
}