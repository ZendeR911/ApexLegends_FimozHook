#include "requiemRender.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image.h"
#include "../../../overlay.h"

namespace Render {

    ID3D11ShaderResourceView* Image(unsigned char* image_data, size_t image_size) {
        int width, height, channels;
        unsigned char* rgba_data = stbi_load_from_memory(image_data, image_size, &width, &height, &channels, 4);
        if (!rgba_data) {
            printf("Failed to load image: %s\n", stbi_failure_reason());
            return nullptr;
        }

        ID3D11ShaderResourceView* myTexture = nullptr;

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA subResource = {};
        subResource.pSysMem = rgba_data;
        subResource.SysMemPitch = width * 4;

        ID3D11Texture2D* pTexture = nullptr;
        if (SUCCEEDED(g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture))) {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = desc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = 1;
            g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &myTexture);
            pTexture->Release();
        }

        stbi_image_free(rgba_data);
        return myTexture;
    }

	// Simplebox
	void DrawBox(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float rounding, ImDrawFlags flags, float thickness) {
		ImGui::GetBackgroundDrawList()->AddRect({ p_min }, { p_max }, ImGui::GetColorU32(col), rounding, flags, thickness);
	}
	void DrawBoxFilled(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float rounding, ImDrawFlags flags) {
		ImGui::GetBackgroundDrawList()->AddRectFilled({ p_min }, { p_max }, ImGui::GetColorU32(col), rounding, flags);
	}

    void DrawBoxCorner(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float thickness) {
        const float x1 = p_min.x;
        const float y1 = p_min.y;
        const float x2 = p_max.x;
        const float y2 = p_max.y;

        const float w = x2 - x1;
        const float h = y2 - y1;

        const float cx = w / 3.0f;
        const float cy = h / 3.0f;

        const ImU32 color = ImGui::GetColorU32(col);

        // bottom left corner
        ImGui::GetBackgroundDrawList()->AddLine({ x1, y1 }, { x1 + cx, y1 }, color, thickness);
        ImGui::GetBackgroundDrawList()->AddLine({ x1, y1 }, { x1, y1 + cy }, color, thickness);

        // bottom right corner
        ImGui::GetBackgroundDrawList()->AddLine({ x2 - cx, y1 }, { x2, y1 }, color, thickness);
        ImGui::GetBackgroundDrawList()->AddLine({ x2, y1 }, { x2, y1 + cy }, color, thickness);

        // top left corner
        ImGui::GetBackgroundDrawList()->AddLine({ x1, y2 }, { x1 + cx, y2 }, color, thickness);
        ImGui::GetBackgroundDrawList()->AddLine({ x1, y2 - cy }, { x1, y2 }, color, thickness);

        // top right corner
        ImGui::GetBackgroundDrawList()->AddLine({ x2 - cx, y2 }, { x2, y2 }, color, thickness);
        ImGui::GetBackgroundDrawList()->AddLine({ x2, y2 - cy }, { x2, y2 }, color, thickness);
    }

	// Filled
	void DrawBoxFilledOutline(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float rounding, ImDrawFlags flags, float thickness) {
		ImGui::GetBackgroundDrawList()->AddRectFilled(p_min, p_max, ImGui::GetColorU32(col), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(p_min.x - 1, p_min.y - 1), ImVec2(p_max.x + 1, p_max.y + 1), IM_COL32(0, 0, 0, 220), rounding, flags, thickness);
	}
	void DrawBoxOutline(const ImVec2& p_min, const ImVec2& p_max, ImVec4 col, float rounding, ImDrawFlags flags, float thickness) {
		ImGui::GetBackgroundDrawList()->AddRect(p_min, p_max, ImGui::GetColorU32(col), rounding, flags, thickness);
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(p_min.x - 1, p_min.y - 1), ImVec2(p_max.x + 1, p_max.y + 1), IM_COL32(0, 0, 0, 220), rounding, flags, thickness);
	}

	// Text
	void DrawLabel(const ImVec2 pos, const ImVec4 col, const char* text) {
        ImDrawList* draw = ImGui::GetBackgroundDrawList();

        const ImU32 text_col = ImGui::GetColorU32(col);
        const ImU32 outline_col = IM_COL32(0, 0, 0, 255);

        static const ImVec2 offsets[] =
        {
            {-1,  0}, { 1,  0}, { 0, -1}, { 0,  1},
            {-1, -1}, { 1, -1}, {-1,  1}, { 1,  1}
        };

        for (const ImVec2& off : offsets)
            draw->AddText(ImVec2(pos.x + off.x, pos.y + off.y), outline_col, text);

        draw->AddText(pos, text_col, text);
	}

    void DrawLabelEx(ImFont* font, const float size, const ImVec2 pos, const ImVec4 col, const char* text) {
        ImDrawList* draw = ImGui::GetBackgroundDrawList();

        const ImU32 text_col = ImGui::GetColorU32(col);
        const ImU32 outline_col = IM_COL32(0, 0, 0, 255);

        static const ImVec2 offsets[] =
        {
            {-1,  0}, { 1,  0}, { 0, -1}, { 0,  1},
            {-1, -1}, { 1, -1}, {-1,  1}, { 1,  1}
        };

        for (const ImVec2& off : offsets)
            draw->AddText(font, size, ImVec2(pos.x + off.x, pos.y + off.y), outline_col, text);

        draw->AddText(font, size, pos, text_col, text);
    }

	void DrawLabelFilled(const ImVec2 pos, const ImVec4 label_col, const ImVec4 fill_col, const char* text, float rounding, ImDrawFlags flags) {
		ImVec2 p_min = ImVec2(pos.x - 4, pos.y);
		ImVec2 p_max = ImVec2(p_min.x + ImGui::CalcTextSize(text).x, p_min.y + ImGui::CalcTextSize(text).y);
		ImGui::GetBackgroundDrawList()->AddRectFilled(p_min, ImVec2(p_max.x + 2, p_max.y), ImGui::GetColorU32(fill_col), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddRect(p_min, ImVec2(p_max.x + 2, p_max.y ), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(p_min.x - 2, p_min.y), ImVec2(p_min.x + 2, p_max.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(p_min.x - 2, p_min.y), ImVec2(p_min.x + 2, p_max.y), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(pos.x - 2, pos.y), ImGui::GetColorU32(label_col), text);
	}

	void DrawLabelFilledEx(ImFont* font, const float size, const ImVec2 pos, const ImVec4 label_col, const ImVec4 fill_col, const char* text, float rounding, ImDrawFlags flags) {
		ImVec2 p_min = ImVec2(pos.x - 4, pos.y);
		ImVec2 p_max = ImVec2(p_min.x + ImGui::CalcTextSize(text).x, p_min.y + ImGui::CalcTextSize(text).y);
		ImGui::GetBackgroundDrawList()->AddRectFilled(p_min, ImVec2(p_max.x + 2, p_max.y), ImGui::GetColorU32(fill_col), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddRect(p_min, ImVec2(p_max.x + 2, p_max.y ), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(p_min.x - 2, p_min.y), ImVec2(p_min.x + 2, p_max.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(p_min.x - 2, p_min.y), ImVec2(p_min.x + 2, p_max.y), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), rounding, flags);
		ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(pos.x - 2, pos.y), ImGui::GetColorU32(label_col), text);
	}

}