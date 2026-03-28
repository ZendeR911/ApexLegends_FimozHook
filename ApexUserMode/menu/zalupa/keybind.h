#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>

struct KeyBind {
    // --- STATIC REGISTRY ---
    // Every KeyBind object created anywhere will automatically sign up here.
    inline static std::vector<KeyBind*> registry;

    static void UpdateAll() {
        for (auto& kb : registry) {
            kb->Update();
        }
    }

    // Returns the keybind currently waiting for input in the UI
    static KeyBind* GetSelected() {
        for (auto& kb : registry) {
            if (kb->isSelected) return kb;
        }
        return nullptr;
    }

    // --- MEMBERS ---
    std::string bindName;
    int key = 0;
    bool isHoldMode = true;
    bool isActive = false;
    bool isSelected = false; // UI is waiting for input
    bool wasPressed = false;

    // Internal state for toggle logic
    bool wasKeyDown = false;

    // --- CONSTRUCTOR / DESTRUCTOR ---
    KeyBind(std::string name = "Empty") : bindName(name) {
        registry.push_back(this);
    }

    // Critical: remove from registry if the object is destroyed
    ~KeyBind() {
        registry.erase(std::remove(registry.begin(), registry.end(), this), registry.end());
    }

    // --- LOGIC ---
    void Clear() {
        key = 0;
        isActive = false;
    }

    void Update() {
        if (key == 0 || isSelected) return;

        bool isKeyDown = (GetAsyncKeyState(key) & 0x8000) != 0;

        // One-time trigger logic
        if (isKeyDown && !wasKeyDown) {
            wasPressed = true;
        }

        if (isHoldMode) {
            isActive = isKeyDown;
        }
        else {
            if (isKeyDown && !wasKeyDown) {
                isActive = !isActive;
            }
        }
        wasKeyDown = isKeyDown;
    }

    bool Pressed() {
        if (wasPressed) {
            wasPressed = false; // Reset so it only returns true once
            return true;
        }
        return false;
    }

    // Helper for your ImGui UI to show the key name
    std::string GetKeyName() const {
        if (key == 0) return "None";

        switch (key) {
        case VK_LBUTTON: return "Mouse 1";
        case VK_RBUTTON: return "Mouse 2";
        case VK_MBUTTON: return "Mouse 3";
        case VK_XBUTTON1: return "Mouse X1";
        case VK_XBUTTON2: return "Mouse X2";
        }

        char name[64];
        UINT scanCode = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
        if (GetKeyNameTextA(scanCode << 16, name, sizeof(name)))
            return std::string(name);

        return "Unknown";
    }
};