#pragma once
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../apex_sdk.h"
#include "config.h"
#include "entity.h"

void DrawCorneredBox(ImDrawList* dL, float x, float y, float w, float h, ImColor color, float thickness = 2.0f);
void RenderESP(const Matrix& vm);
