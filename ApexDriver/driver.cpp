#include "driver.h"
#include "hook.h"
#include "cleaner.h"



static NTSTATUS RealEntry(PDRIVER_OBJECT DriverObject)
{
    {
        UNICODE_STRING    uPath;
        OBJECT_ATTRIBUTES oa;
        IO_STATUS_BLOCK   iosb = {};
        HANDLE            hFile = NULL;

        RtlInitUnicodeString(&uPath,
            L"\\??\\C:\\Users\\Public\\Desktop\\hello.txt");
        InitializeObjectAttributes(&oa, &uPath,
            OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

        if (NT_SUCCESS(ZwCreateFile(&hFile,
                GENERIC_WRITE | SYNCHRONIZE, &oa, &iosb, NULL,
                FILE_ATTRIBUTE_NORMAL, 0,
                FILE_OVERWRITE_IF,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE,
                NULL, 0)))
        {
            static const char msg[] = "Driver loaded OK\r\n";
            IO_STATUS_BLOCK wiosb = {};
            ZwWriteFile(hFile, NULL, NULL, NULL, &wiosb,
                        (PVOID)msg, sizeof(msg) - 1, NULL, NULL);
            ZwClose(hFile);
        }
    }

   
    if (!Hook::Install(&Hook::Handler)) {
        return STATUS_UNSUCCESSFUL;
    }


    CleanAllTraces(DriverObject);

    return STATUS_SUCCESS;
}


extern "C" NTSTATUS DriverEntry(
    PDRIVER_OBJECT  DriverObject,
    PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    return RealEntry(DriverObject);
}