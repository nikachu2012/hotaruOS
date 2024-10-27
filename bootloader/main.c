#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Guid/FileInfo.h>

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
    Print(L"memoryMap wrote.\n");

    Print(L"Loading kernel...\n");

    // Load kernel
    EFI_FILE_PROTOCOL *kernelFile;
    rootDir->Open(rootDir, &kernelFile, L"\\kernel.elf", EFI_FILE_MODE_READ, 0);

    UINTN fileInfoSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 fileInfoBuffer[fileInfoSize];

    kernelFile->GetInfo(kernelFile, &gEfiFileInfoGuid, &fileInfoSize, fileInfoBuffer);
    EFI_FILE_INFO *fileInfo = (EFI_FILE_INFO *)fileInfoBuffer;
    UINTN kernelFileSize = fileInfo->FileSize;

    EFI_PHYSICAL_ADDRESS kernelBaseAddress = 0x100000;

    // ベースアドレスから kernelFileSize / EFI_PAGE_SIZE の切り上げ分のメモリを確保
    gBS->AllocatePages(AllocateAddress, EfiLoaderData, (kernelFileSize + 0xfff) / 0x1000, &kernelBaseAddress);

    kernelFile->Read(kernelFile, &kernelFileSize, (VOID *)kernelBaseAddress);
    Print(L"Kernel loaded!\nKernel address: 0x%0llx, size: %llu bytes\n", kernelBaseAddress, kernelFileSize);

    // EFIブートサービスを終了
    EFI_STATUS status;
    status = gBS->ExitBootServices(image_handle, memoryMapKey);

    if (EFI_ERROR(status))
    {
        // もう一度メモリマップの取得
        // (ブートサービスを終了するにはメモリマップのキーが最新に近いものでなければならない)
        memoryMapSize = sizeof(memoryMap);
        status = gBS->GetMemoryMap(&memoryMapSize, memoryMap, &memoryMapKey, &descriptorSize,
                                   &descriptorVersion);
        if (EFI_ERROR(status))
        {
            Print(L"Failed to get memory map. (status: %r)\n", status);
            while (1)
                ;
        }

        status = gBS->ExitBootServices(image_handle, memoryMapKey);
        if (EFI_ERROR(status))
        {
            Print(L"Could not exit boot service. (status: %r)\n", status);
            while (1)
                ;
        }
    }

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

    // Open Graphic Output Protocol(GOP)

    // カーネルの起動
    // エントリポイントのアドレスを取得
    UINT64 entryAddress = *(UINT64 *)(kernelBaseAddress + 24);

    // 関数プロトタイプを作ってメモリのアドレスから関数を実行
    typedef void EntryPointType(void);
    EntryPointType *entryPoint = (EntryPointType *)entryAddress;
    entryPoint();

    Print(L"ALL DONE\n");
    while (1)
        ;

    return EFI_SUCCESS;
}
