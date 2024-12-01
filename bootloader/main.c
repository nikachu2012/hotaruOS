#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#include "frameBufferConfig.hpp"
#include "elf.hpp"

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

EFI_STATUS openGOP(EFI_HANDLE image_handle, EFI_GRAPHICS_OUTPUT_PROTOCOL **gop)
{
    UINTN numGopHandles = 0;
    EFI_HANDLE *gopHandles = NULL;

    gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &numGopHandles,
        &gopHandles);

    gBS->OpenProtocol(
        gopHandles[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)gop,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    FreePool(gopHandles);

    return EFI_SUCCESS;
}

const CHAR16 *pixelFormatToString(EFI_GRAPHICS_PIXEL_FORMAT fmt)
{
    switch (fmt)
    {
    case PixelRedGreenBlueReserved8BitPerColor:
        return L"PixelRedGreenBlueReserved8BitPerColor";
    case PixelBlueGreenRedReserved8BitPerColor:
        return L"PixelBlueGreenRedReserved8BitPerColor";
    case PixelBitMask:
        return L"PixelBitMask";
    case PixelBltOnly:
        return L"PixelBltOnly";
    case PixelFormatMax:
        return L"PixelFormatMax";
    default:
        return L"InvalidPixelFormat";
    }
}

void Halt(void)
{
    while (1)
        __asm__("hlt");
}

void CalcLoadAddressRange(Elf64_Ehdr *fileHeader, UINT64 *first, UINT64 *last)
{
    Elf64_Phdr *programHeaderArray = (Elf64_Phdr *)((VOID *)fileHeader + fileHeader->e_phoff);

    *first = MAX_UINT64; // 最小値を求めるために最大値にする
    *last = 0;           // 最大値を求めるために最小値にする

    for (Elf64_Half i = 0; i < fileHeader->e_phnum; i++)
    {
        if (programHeaderArray[i].p_type != PT_LOAD)
            continue;

        *first = MIN(*first, programHeaderArray[i].p_vaddr);
        *last = MAX(*last, programHeaderArray[i].p_vaddr + programHeaderArray[i].p_memsz);
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
    EFI_STATUS status;

    EFI_FILE_PROTOCOL *kernelFile;
    rootDir->Open(rootDir, &kernelFile, L"\\kernel.elf", EFI_FILE_MODE_READ, 0);

    UINTN fileInfoSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 fileInfoBuffer[fileInfoSize];

    kernelFile->GetInfo(kernelFile, &gEfiFileInfoGuid, &fileInfoSize, fileInfoBuffer);
    EFI_FILE_INFO *fileInfo = (EFI_FILE_INFO *)fileInfoBuffer;
    UINTN kernelFileSize = fileInfo->FileSize;

    // カーネル保存用の一時領域を確保
    VOID *kernelBuffer;
    status = gBS->AllocatePool(EfiLoaderData, kernelFileSize, &kernelBuffer);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to allocate pool: %r\n", status);
        Halt();
    }

    status = kernelFile->Read(kernelFile, &kernelFileSize, kernelBuffer);
    if (EFI_ERROR(status))
    {
        Print(L"File load error: %r\n", status);
        Halt();
    }

    Elf64_Ehdr *kernelFileHeader = (Elf64_Ehdr *)kernelBuffer;
    UINT64 kernelFirstAddr, kernelLastAddr;

    Elf64_Phdr *kernelProgramHeaderArray = ((Elf64_Phdr *)((UINT64)kernelFileHeader + kernelFileHeader->e_phoff));
    CalcLoadAddressRange(kernelFileHeader, &kernelFirstAddr, &kernelLastAddr);

    Print(L"Kernel address: 0x%08llx - 0x%08llx\n", kernelFirstAddr, kernelLastAddr);

    // ページ数の計算 (ファイル全体のバイト数 / 4KiB の切り上げ)
    UINTN pages = (kernelLastAddr - kernelFirstAddr + (EFI_PAGE_SIZE - 1)) / EFI_PAGE_SIZE;
    status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, pages, &kernelFirstAddr); // 実際指定した番地(0x100000)にカーネルを配置
    if (EFI_ERROR(status))
    {
        Print(L"Failed to allocate pages: %r\n", status);
        Halt();
    }

    // copy segments
    for (Elf64_Half i = 0; i < kernelFileHeader->e_phnum; ++i)
    {
        if (kernelProgramHeaderArray[i].p_type != PT_LOAD)
            continue;

        UINT64 segmentInFile = (UINT64)kernelFileHeader + kernelProgramHeaderArray[i].p_offset;
        CopyMem((VOID *)kernelProgramHeaderArray[i].p_vaddr,
                (VOID *)segmentInFile,
                kernelProgramHeaderArray[i].p_filesz);

        UINTN remainBytes = kernelProgramHeaderArray[i].p_memsz - kernelProgramHeaderArray[i].p_filesz;
        SetMem((VOID *)kernelProgramHeaderArray[i].p_vaddr + kernelProgramHeaderArray[i].p_filesz,
               remainBytes,
               0);
    }

    Print(L"Kernel loaded!\n");

    // 確保した一時領域を開放
    status = gBS->FreePool(kernelBuffer);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to free pool: %r\n", status);
        Halt();
    }

    // Open Graphic Output Protocol(GOP)
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    openGOP(image_handle, &gop);

    Print(L"Resolution: %ux%u, Pixel format: %ls, %u pixels/line\n",
          gop->Mode->Info->HorizontalResolution,
          gop->Mode->Info->VerticalResolution,
          pixelFormatToString(gop->Mode->Info->PixelFormat),
          gop->Mode->Info->PixelsPerScanLine);

    Print(L"Framebuffer: 0x%0llx - 0x%0llx, size: %lu bytes\n",
          gop->Mode->FrameBufferBase,
          gop->Mode->FrameBufferBase + gop->Mode->FrameBufferSize,
          gop->Mode->FrameBufferSize);

    // EFIブートサービスを終了
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

    // カーネルの起動
    // エントリポイントのアドレスを取得
    UINT64 entryAddress = *(UINT64 *)(kernelFirstAddr + 24);

    struct frameBufferConfig config =
        {(UINT8 *)gop->Mode->FrameBufferBase,
         gop->Mode->Info->PixelsPerScanLine,
         gop->Mode->Info->HorizontalResolution,
         gop->Mode->Info->VerticalResolution,
         PixelFormat_undefined};

    switch (gop->Mode->Info->PixelFormat)
    {
    case PixelRedGreenBlueReserved8BitPerColor:
        config.pixelFormat = PixelFormat_RGBResv8BitPerColor;
        break;
    case PixelBlueGreenRedReserved8BitPerColor:
        config.pixelFormat = PixelFormat_BGRResv8BitPerColor;
        break;
    default:
        Print(L"!!!!ERRROR!!!! Unimplemented pixel format: %s\n", pixelFormatToString(gop->Mode->Info->PixelFormat));
        Halt();
        break;
    }

    // 関数プロトタイプを作ってメモリのアドレスから関数を実行
    // System V AMD64 ABIの形式に固定して引数を渡す (CLANGPDBでコンパイルするとき)
    typedef void __attribute__((sysv_abi)) EntryPointType(const struct frameBufferConfig *);
    EntryPointType *entry_point = (EntryPointType *)entryAddress;
    entry_point(&config);

    Print(L"ALL DONE\n");
    Halt();

    return EFI_SUCCESS;
}
