#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/LoadedImage.h>
#include <Uefi.h>

EFI_STATUS openRootDir(IN EFI_HANDLE image_handle, OUT EFI_FILE_PROTOCOL **rootDir)
{
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

    gBS->OpenProtocol(image_handle, &gEfiLoadedImageProtocolGuid,
                      (VOID **)&loaded_image, image_handle, NULL,
                      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    gBS->OpenProtocol(loaded_image->DeviceHandle,
                      &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs,
                      image_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    fs->OpenVolume(fs, rootDir);

    return EFI_SUCCESS;
}

const CHAR16 *memoryTypeToString(EFI_MEMORY_TYPE type)
{
    switch (type)
    {
    case EfiReservedMemoryType:
        return L"EfiReservedMemoryType";
    case EfiLoaderCode:
        return L"EfiLoaderCode";
    case EfiLoaderData:
        return L"EfiLoaderData";
    case EfiBootServicesCode:
        return L"EfiBootServicesCode";
    case EfiBootServicesData:
        return L"EfiBootServicesData";
    case EfiRuntimeServicesCode:
        return L"EfiRuntimeServicesCode";
    case EfiRuntimeServicesData:
        return L"EfiRuntimeServicesData";
    case EfiConventionalMemory:
        return L"EfiConventionalMemory";
    case EfiUnusableMemory:
        return L"EfiUnusableMemory";
    case EfiACPIReclaimMemory:
        return L"EfiACPIReclaimMemory";
    case EfiACPIMemoryNVS:
        return L"EfiACPIMemoryNVS";
    case EfiMemoryMappedIO:
        return L"EfiMemoryMappedIO";
    case EfiMemoryMappedIOPortSpace:
        return L"EfiMemoryMappedIOPortSpace";
    case EfiPalCode:
        return L"EfiPalCode";
    case EfiPersistentMemory:
        return L"EfiPersistentMemory";
    case EfiMaxMemoryType:
        return L"EfiMaxMemoryType";
    default:
        return L"InvalidMemoryType";
    }
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE image_handle,
                           EFI_SYSTEM_TABLE *system_table)
{
    Print(L"hotaruOS by nikachu2012\n");

    EFI_MEMORY_DESCRIPTOR memoryMap[500]; // 40 * 500 bytes
    UINTN memoryMapKey, descriptorSize, memoryMapSize = sizeof(memoryMap);
    UINT32 descriptorVersion;

    // getMemoryMap
    gBS->GetMemoryMap(&memoryMapSize, memoryMap, &memoryMapKey, &descriptorSize,
                      &descriptorVersion);

    // get root directory
    EFI_FILE_PROTOCOL *rootDir;
    openRootDir(image_handle, &rootDir);

    // open memmap file
    EFI_FILE_PROTOCOL *memmapFile;
    rootDir->Open(
        rootDir, &memmapFile, L"\\memmap",
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);

    // write header
    UINTN len;
    CHAR8 *header =
        "Index, Type, Type(name), PhysicalStart, NumberOfPages, Attribute\n";
    len = AsciiStrLen(header);
    memmapFile->Write(memmapFile, &len, header);

    Print(L"bufferSize: 0x%08llx, mapSize: 0x%08llx, Key: 0x%08llx, "
          L"descSize: 0x%08llx, descVer: 0x%04lx\n",
          sizeof(memoryMap), memoryMapSize, memoryMapKey, descriptorSize,
          descriptorVersion);

    // write memmap
    for (int i = 0;
         ((EFI_PHYSICAL_ADDRESS)memoryMap + descriptorSize * i) < (EFI_PHYSICAL_ADDRESS)memoryMap + memoryMapSize;
         i++)
    {
        EFI_MEMORY_DESCRIPTOR *desc =
            (EFI_MEMORY_DESCRIPTOR *)(((EFI_PHYSICAL_ADDRESS)memoryMap) +
                                      descriptorSize * i);

        CHAR8 buf[256];

        len = AsciiSPrint(buf, sizeof(buf), "%u, %x, %ls, %08llx, %llx, %llx\n",
                          i, desc->Type, memoryTypeToString(desc->Type),
                          desc->PhysicalStart, desc->NumberOfPages,
                          desc->Attribute & 0xffffflu);

        memmapFile->Write(memmapFile, &len, buf);
    }

    // close file desc
    memmapFile->Close(memmapFile);

    Print(L"memoryMap wrote.");

    while (1)
        ;

    return EFI_SUCCESS;
}
