#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include "../driver_comm.h"

inline DriverComm g_DriverComm;
inline DWORD      g_CurrentPID = 0;

class DriverInterface {
public:
    bool connect() {
        return g_DriverComm.Init();
    }

    bool isConnected() const {
        return g_DriverComm.IsConnected();
    }

    bool readProcessMemory(DWORD pid, ULONG64 address, void* buffer, SIZE_T size) {
        return g_DriverComm.ReadMemory(pid, address, buffer, size);
    }

    bool mouseMove(LONG x, LONG y) {
        INPUT input = {};
        input.type       = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        input.mi.dx      = x;
        input.mi.dy      = y;
        SendInput(1, &input, sizeof(INPUT));
        return true;
    }

    struct BatchReadEntry {
        ULONG64 address;
        void*   buffer;
        SIZE_T  size;
    };

    bool batchReadProcessMemory(DWORD pid, BatchReadEntry* entries, ULONG count) {
        for (ULONG i = 0; i < count; i++) {
            g_DriverComm.ReadMemory(pid, entries[i].address,
                                    entries[i].buffer, entries[i].size);
        }
        return true;
    }
};

inline DriverInterface g_Driver;
namespace I {

    template <class T>
    T Read(DWORD64 address) {
        T buffer{};
        g_Driver.readProcessMemory(g_CurrentPID, address, &buffer, sizeof(T));
        return buffer;
    }

    template <class T>
    bool Write(DWORD64 address, const T& value) {
        return g_DriverComm.WriteMemory(g_CurrentPID, address,
                                        (void*)&value, sizeof(T));
    }

    inline BOOL installDriver(const char*) {
        return g_Driver.connect() ? TRUE : FALSE;
    }

    inline BOOL isInstall() {
        return g_Driver.isConnected() ? TRUE : FALSE;
    }

    inline DWORD64 GetModuleBase(const char* Module) {
        wchar_t wModule[256] = {};
        MultiByteToWideChar(CP_ACP, 0, Module, -1, wModule, 256);
        return g_DriverComm.GetModuleBase(g_CurrentPID, wModule);
    }

    inline BOOL SetProcess(ULONG pid) {
        g_CurrentPID = pid;
        return TRUE;
    }

    inline DWORD GetProcessIdByName(const WCHAR* szProcessName) {
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE) return 0;

        PROCESSENTRY32W pe = {};
        pe.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(hSnap, &pe)) {
            do {
                if (!wcscmp(szProcessName, pe.szExeFile)) {
                    CloseHandle(hSnap);
                    return pe.th32ProcessID;
                }
            } while (Process32NextW(hSnap, &pe));
        }
        CloseHandle(hSnap);
        return 0;
    }

    inline BOOL BatchRead(DriverInterface::BatchReadEntry* entries, ULONG count) {
        return g_Driver.batchReadProcessMemory(g_CurrentPID, entries, count)
               ? TRUE : FALSE;
    }

} 
