#include "hook.h"
#include "pte_hook.h"
#include "spoof_call.h"
#include "physical_memory.h"


BOOL Hook::Install(void* handlerAddr)
{
    if (!handlerAddr)
        return FALSE;

    PVOID hookTarget = GetSystemModuleExport(
        "dxgkrnl.sys",
        "NtQueryCompositionSurfaceStatistics"
    );

    if (!hookTarget)
        return FALSE;

    InitSpoofCall();

   
    if (InstallPteHook(hookTarget, handlerAddr))
        return TRUE;

  
    BYTE patch[12] = { 0 };
    patch[0] = 0x48; 
    patch[1] = 0xB8; 
    uintptr_t addr = reinterpret_cast<uintptr_t>(handlerAddr);
    memcpy(&patch[2], &addr, sizeof(void*));
    patch[10] = 0xFF;
    patch[11] = 0xE0;

    WriteReadOnlyMemory(hookTarget, patch, sizeof(patch));

    return TRUE;
}



NTSTATUS Hook::Handler(PVOID callParam)
{
    if (!callParam || !MmIsAddressValid(callParam))
        return STATUS_SUCCESS;

    PREQUEST_DATA req = (PREQUEST_DATA)callParam;

    if (req->magic != REQUEST_MAGIC)
        return STATUS_SUCCESS;

    switch (req->command) {

    case CMD_READ:
        req->result = PhysicalReadProcessMemory(
            (HANDLE)req->pid,
            req->address,
            (PVOID)req->buffer,
            (SIZE_T)req->size
        ) ? 1 : 0;
        break;

    case CMD_WRITE:
        req->result = PhysicalWriteProcessMemory(
            (HANDLE)req->pid,
            req->address,
            (PVOID)req->buffer,
            (SIZE_T)req->size
        ) ? 1 : 0;
        break;

    case CMD_READ64:
        myReadProcessMemory(
            (HANDLE)req->pid,
            (PVOID)req->address,
            (PVOID)req->buffer,
            (DWORD)req->size
        );
        break;

    case CMD_WRITE64:
        myWriteProcessMemory(
            (HANDLE)req->pid,
            (PVOID)req->address,
            (PVOID)req->buffer,
            (DWORD)req->size
        );
        break;

    case CMD_MODULE_BASE:
        req->result = (unsigned __int64)GetProcessModuleBase(
            (HANDLE)req->pid,
            req->module_name
        );
        break;

    case CMD_ALLOC:
        req->result = (unsigned __int64)AllocateVirtualMemory(
            (HANDLE)req->pid,
            req->size,
            req->protect
        );
        break;

    case CMD_FREE:
        FreeVirtualMemory(
            (HANDLE)req->pid,
            (PVOID)req->result
        );
        break;

    case CMD_PROTECT:
        ProtectVirtualMemory(
            (HANDLE)req->pid,
            req->address,
            req->size,
            req->protect
        );
        break;

    case CMD_PING:
        req->result = g_PteHook.active ? 0x50544548 : 0x4B524E4C;
        break;

    case CMD_VERIFY_PTE:
    {       
        if (!req->buffer || !MmIsAddressValid((PVOID)req->buffer)) {
            req->result = 0;
            break;
        }

        PUCHAR out = (PUCHAR)req->buffer;
        RtlZeroMemory(out, 64);

        *(PULONG64)(out + 0)  = g_PteHook.active ? 1ULL : 0ULL;
        *(PULONG64)(out + 8)  = g_PteHook.originalPfn;
        *(PULONG64)(out + 16) = g_PteHook.newPfn;
        *(PULONG64)(out + 24) = (ULONG64)g_PteHook.targetVA;

        if (g_PteHook.active && g_PteHook.targetVA) {
            ULONG pageOffset = (ULONG)((ULONG64)g_PteHook.targetVA & 0xFFF);
       
            PHYSICAL_ADDRESS origPA;
            origPA.QuadPart = (LONGLONG)(g_PteHook.originalPfn << 12);
            PVOID mapped = MmMapIoSpace(origPA, PAGE_SIZE, MmCached);
            if (mapped) {
                RtlCopyMemory(out + 32, (PUCHAR)mapped + pageOffset, 16);
                MmUnmapIoSpace(mapped, PAGE_SIZE);
            }
        
            if (MmIsAddressValid(g_PteHook.targetVA)) {
                RtlCopyMemory(out + 48, g_PteHook.targetVA, 16);
            }
        }

        req->result = 1;
        break;
    }

    case CMD_VERIFY_SPOOF:
    {
        if (req->buffer && MmIsAddressValid((PVOID)req->buffer)) {
            PUCHAR out = (PUCHAR)req->buffer;
            RtlZeroMemory(out, 16);
            *(PULONG64)(out + 0) = (ULONG64)g_SpoofGadget;
            *(PULONG64)(out + 8) = (ULONG64)g_SpoofStub;
        }
        req->result = (g_SpoofGadget && g_SpoofStub) ? 1 : 0;
        break;
    }

    default:
        break;
    }

    return STATUS_SUCCESS;
}
