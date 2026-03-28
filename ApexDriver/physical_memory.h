

#pragma once
#include "definitions.h"
#include "spoof_call.h"
#include <intrin.h>


#define PTE_PRESENT      0x1ULL
#define PTE_LARGE_PAGE   0x80ULL
#define PTE_PHYS_MASK    0x0000FFFFFFFFF000ULL
#define PAGE_OFFSET_MASK 0xFFFULL


typedef struct _CR3_CACHE_ENTRY {
    volatile ULONG64 cr3;
    volatile HANDLE  pid;
    volatile ULONG   callCount;
    volatile BOOLEAN validated;
} CR3_CACHE_ENTRY;

static CR3_CACHE_ENTRY g_Cr3Cache = { 0, 0, 0, FALSE };

#define CR3_CACHE_THRESHOLD  500


static BOOLEAN ValidateCR3(ULONG64 cr3)
{
    if (cr3 == 0 || (cr3 & 0xFFF) != 0)
        return FALSE; 

    ULONG64 pml4e = 0;
    MM_COPY_ADDRESS addr;
    addr.PhysicalAddress.QuadPart = (LONGLONG)(cr3 & PTE_PHYS_MASK);
    SIZE_T bytesRead = 0;

    NTSTATUS status = MmCopyMemory(&pml4e, addr, sizeof(pml4e),
        MM_COPY_MEMORY_PHYSICAL, &bytesRead);

    if (!NT_SUCCESS(status) || bytesRead != sizeof(pml4e))
        return FALSE;

    return (pml4e & PTE_PRESENT) != 0;
}


static ULONG64 GetProcessCR3(HANDLE pid)
{
    if (!pid) return 0;

    if (g_Cr3Cache.pid == pid && g_Cr3Cache.validated && g_Cr3Cache.cr3)
        return g_Cr3Cache.cr3;

    if (g_Cr3Cache.pid != pid) {
        g_Cr3Cache.pid = pid;
        g_Cr3Cache.callCount = 0;
        g_Cr3Cache.cr3 = 0;
        g_Cr3Cache.validated = FALSE;
    }

    g_Cr3Cache.callCount++;

    PEPROCESS process = NULL;
    NTSTATUS lookupStatus;
    if (g_SpoofStub)
        lookupStatus = SpoofCall2(PsLookupProcessByProcessId, pid, &process);
    else
        lookupStatus = PsLookupProcessByProcessId(pid, &process);
    if (!NT_SUCCESS(lookupStatus))
        return 0;

    KAPC_STATE apc;
    if (g_SpoofStub)
        SpoofCall2(KeStackAttachProcess, process, &apc);
    else
        KeStackAttachProcess(process, &apc);

    ULONG64 cr3 = __readcr3();

    if (g_SpoofStub)
        SpoofCall1(KeUnstackDetachProcess, &apc);
    else
        KeUnstackDetachProcess(&apc);

    if (g_SpoofStub)
        SpoofCall1(ObfDereferenceObject, process);
    else
        ObfDereferenceObject(process);

    if (cr3 && ValidateCR3(cr3)) {
        g_Cr3Cache.cr3 = cr3;
        if (g_Cr3Cache.callCount >= CR3_CACHE_THRESHOLD)
            g_Cr3Cache.validated = TRUE;
    }

    return cr3;
}


static NTSTATUS ReadPhysicalAddress(ULONG64 physAddr, PVOID buffer, SIZE_T size)
{
    if (!buffer || !size || !physAddr)
        return STATUS_INVALID_PARAMETER;

    MM_COPY_ADDRESS addr;
    addr.PhysicalAddress.QuadPart = (LONGLONG)physAddr;
    SIZE_T bytesRead = 0;

    return MmCopyMemory(buffer, addr, size,
        MM_COPY_MEMORY_PHYSICAL, &bytesRead);
}


static NTSTATUS WritePhysicalAddress(ULONG64 physAddr, PVOID buffer, SIZE_T size)
{
    if (!buffer || !size || !physAddr)
        return STATUS_INVALID_PARAMETER;

    PHYSICAL_ADDRESS pa;
    pa.QuadPart = (LONGLONG)physAddr;

    PVOID mapped = MmMapIoSpace(pa, size, MmNonCached);
    if (!mapped)
        return STATUS_INSUFFICIENT_RESOURCES;

    memcpy(mapped, buffer, size);

    MmUnmapIoSpace(mapped, size);
    return STATUS_SUCCESS;
}


static ULONG64 TranslateVirtualAddress(ULONG64 cr3, ULONG64 virtualAddress)
{
    if (!cr3 || !virtualAddress)
        return 0;

    ULONG64 pml4_idx = (virtualAddress >> 39) & 0x1FF;
    ULONG64 pdpt_idx = (virtualAddress >> 30) & 0x1FF;
    ULONG64 pd_idx   = (virtualAddress >> 21) & 0x1FF;
    ULONG64 pt_idx   = (virtualAddress >> 12) & 0x1FF;
    ULONG64 offset   = virtualAddress & PAGE_OFFSET_MASK;

    ULONG64 pte = 0;

    if (!NT_SUCCESS(ReadPhysicalAddress(
            (cr3 & PTE_PHYS_MASK) + pml4_idx * 8, &pte, 8)))
        return 0;
    if (!(pte & PTE_PRESENT)) return 0;

    if (!NT_SUCCESS(ReadPhysicalAddress(
            (pte & PTE_PHYS_MASK) + pdpt_idx * 8, &pte, 8)))
        return 0;
    if (!(pte & PTE_PRESENT)) return 0;
    if (pte & PTE_LARGE_PAGE)  
        return (pte & 0xFFFFC0000000ULL) + (virtualAddress & 0x3FFFFFFFULL);

    if (!NT_SUCCESS(ReadPhysicalAddress(
            (pte & PTE_PHYS_MASK) + pd_idx * 8, &pte, 8)))
        return 0;
    if (!(pte & PTE_PRESENT)) return 0;
    if (pte & PTE_LARGE_PAGE) 
        return (pte & 0xFFFFFE00000ULL) + (virtualAddress & 0x1FFFFFULL);

    if (!NT_SUCCESS(ReadPhysicalAddress(
            (pte & PTE_PHYS_MASK) + pt_idx * 8, &pte, 8)))
        return 0;
    if (!(pte & PTE_PRESENT)) return 0;

    return (pte & PTE_PHYS_MASK) + offset;
}



#define PHYS_STACK_BUF_SIZE  0x1000 

static BOOL PhysicalReadProcessMemory(
    HANDLE pid, ULONG64 virtualAddress, PVOID userBuffer, SIZE_T size)
{
    if (!userBuffer || !size || !virtualAddress)
        return FALSE;

    ULONG64 cr3 = GetProcessCR3(pid);
    if (!cr3) return FALSE;

    BOOLEAN usePool = (size > PHYS_STACK_BUF_SIZE);
    PUCHAR kernelBuf = NULL;

    if (usePool) {
        kernelBuf = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, size, 'rPhM');
        if (!kernelBuf) return FALSE;
    } else {
        kernelBuf = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, size, 'rPhM');
        if (!kernelBuf) return FALSE;
    }

    RtlZeroMemory(kernelBuf, size);

    BOOL success = TRUE;
    SIZE_T totalRead = 0;

    while (totalRead < size)
    {
        ULONG64 currentVA = virtualAddress + totalRead;
        SIZE_T pageRemaining = 0x1000 - (currentVA & PAGE_OFFSET_MASK);
        SIZE_T chunkSize = min(pageRemaining, size - totalRead);

        ULONG64 physAddr = TranslateVirtualAddress(cr3, currentVA);
        if (!physAddr) {
            success = FALSE;
            break;
        }

        if (!NT_SUCCESS(ReadPhysicalAddress(physAddr, kernelBuf + totalRead, chunkSize))) {
            success = FALSE;
            break;
        }

        totalRead += chunkSize;
    }

  
    if (success) {
        __try {
            RtlCopyMemory(userBuffer, kernelBuf, size);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            success = FALSE;
        }
    }

    ExFreePoolWithTag(kernelBuf, 'rPhM');
    return success;
}

static BOOL PhysicalWriteProcessMemory(
    HANDLE pid, ULONG64 virtualAddress, PVOID userBuffer, SIZE_T size)
{
    if (!userBuffer || !size || !virtualAddress)
        return FALSE;

    
    PUCHAR kernelBuf = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, size, 'wPhM');
    if (!kernelBuf) return FALSE;

    __try {
        RtlCopyMemory(kernelBuf, userBuffer, size);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        ExFreePoolWithTag(kernelBuf, 'wPhM');
        return FALSE;
    }

    PEPROCESS process = NULL;
    NTSTATUS lookupSt;
    if (g_SpoofStub)
        lookupSt = SpoofCall2(PsLookupProcessByProcessId, pid, &process);
    else
        lookupSt = PsLookupProcessByProcessId(pid, &process);
    if (!NT_SUCCESS(lookupSt)) {
        ExFreePoolWithTag(kernelBuf, 'wPhM');
        return FALSE;
    }

    BOOL success = FALSE;
    KAPC_STATE apc;
    if (g_SpoofStub)
        SpoofCall2(KeStackAttachProcess, process, &apc);
    else
        KeStackAttachProcess(process, &apc);

    __try {
        RtlCopyMemory((PVOID)virtualAddress, kernelBuf, size);
        success = TRUE;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        success = FALSE;
    }

    if (g_SpoofStub) {
        SpoofCall1(KeUnstackDetachProcess, &apc);
        SpoofCall1(ObfDereferenceObject, process);
    } else {
        KeUnstackDetachProcess(&apc);
        ObfDereferenceObject(process);
    }
    ExFreePoolWithTag(kernelBuf, 'wPhM');

    return success;
}
