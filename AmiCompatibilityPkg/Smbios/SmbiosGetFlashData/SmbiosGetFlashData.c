//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

/** @file SmbiosGetFlashData.c
    This file provides AMI Smbios GetFlashData Protocols

**/

#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/SmbiosGetFlashDataProtocol.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Library/BaseMemoryLib.h>

#define Align4(Value) (((Value)+3) & ~3)
#define Align8(Value) (((Value)+7) & ~7)

#pragma pack(1)

#define DMIEDIT_WRITE_ONCE      0x01
#define DMIEDIT_DELETE_STRUC    0x02
#define DMIEDIT_ADD_STRUC       0x04
#define DMIEDIT_EXTENDED_HDR    0x80

/**
    DMI data record
**/
typedef struct {
    UINT8   Type;
    UINT8   Offset;     // Structure field offset, or string number for Type 11 and 12
    UINT8   Reserved;   // Size of string including \0 or UUID (16)
    UINT8   Flags;      // Bit0 = Write Once
                        // Bit1 = Delete Structure
                        // Bit2 = Add structure
                        // Bit7 = Extended Header
    UINT8   HdrLength;
    UINT16  Size;
    UINT16  Handle;
} TABLE_INFO;

/**
    DMI Data Table Header
**/
typedef struct {
    UINT32  Signature;
    UINT32  Size;
} ROM_INFO;

#pragma pack()

EFI_GUID FlashDataFile  =  {0xfd44820b, 0xf1ab, 0x41c0, 0xae, 0x4e, 0x0c, 0x55, 0x55, 0x6e, 0xb9, 0xbd};

ROM_INFO    gRomInfo;
void        *gRomData;

/**
    Get Flash Data location and size.

    @param This     Pointer to EFI_SMBIOS_FLASH_DATA_PROTOCOL protocol
    @param Location Pointer to hold the location of flash data
    @param Size     Pointer to flash data size

    @return EFI_STATUS

**/
EFI_STATUS
GetFlashTableInfo (
    IN      EFI_SMBIOS_FLASH_DATA_PROTOCOL  *This,
    IN OUT  VOID                            **Location,
    IN OUT  UINT32                          *Size
)
{
    *Location = gRomData;
    *Size = gRomInfo.Size;
    return EFI_SUCCESS;
}

/**
    Get Flash Data Field

    @param This     Pointer to EFI_SMBIOS_FLASH_DATA_PROTOCOL protocol
    @param Table
    @param Offset
    @param Field

    @return EFI_STATUS

**/
EFI_STATUS
GetField (
    IN EFI_SMBIOS_FLASH_DATA_PROTOCOL  *This,
    IN UINT8                           Table,
    IN UINT8                           Offset,
    IN VOID                            **Field
)
{
    TABLE_INFO *p = gRomData;

    while ( p->Offset != 0xff && (p->Type != Table || p->Offset != Offset)) {
        p = (TABLE_INFO*)  ((UINT8*)(p+1) + p->Size);
    }

    if (p->Offset != 0xff) {
        *Field = p + 1;
        return EFI_SUCCESS;
    }

    *Field = 0;
    return EFI_NOT_FOUND;
}


EFI_SMBIOS_FLASH_DATA_PROTOCOL gSmbiosFlashDataProtocol = {
    GetFlashTableInfo,
    GetField
};

/**
    Find the RAW section

    @param Section
    @param End

    @return Pointer to Raw Section if found

**/
VOID*
FindRawSection (
    IN VOID *Section,
    IN VOID *End
)
{
    EFI_COMMON_SECTION_HEADER   *p = Section;
    while((UINTN)p < (UINTN)End)    //Use signed because 0 = 0x100000000
    {
        if (p->Type == EFI_SECTION_RAW) return (p+1);

        if (Align4(SECTION_SIZE(p)) == 0x00) {
            return 0;               // Section size = 0 indicates data is corrupted
        }

        p = (EFI_COMMON_SECTION_HEADER*)((UINT8*)p+Align4(SECTION_SIZE(p)));
    }
    return 0;
}

/**
    Find file in the FV with the input GUID

    @param Guid
    @param File
    @param EndOfFiles

    @return Pointer to File if found

**/
VOID*
FindFile (
    IN EFI_GUID *Guid,
    IN VOID     *File,
    IN VOID     *EndOfFiles
)
{
    EFI_FFS_FILE_HEADER *p = File;

    while(((UINTN)p < (UINTN)EndOfFiles) &&
          ((UINTN)(p+sizeof(EFI_FFS_FILE_HEADER)) < (UINTN)EndOfFiles)) // Use signed because 0 = 0x100000000
    {
        if (CompareGuid(Guid,&p->Name)) {
            // Found File.
            return FindRawSection(
                        p+1,
                        (UINT8*)p + (*(UINT32*)p->Size & 0xffffff) - sizeof(*p)
                        );
        }

        p = (EFI_FFS_FILE_HEADER*)((UINT8*)p + Align8((*(UINT32*)p->Size & 0xffffff)));
    }

    return NULL;
}

/**
    Find the Flash Data file in the FV.

    @param None

    @return EFI_STATUS

**/
EFI_STATUS
GetDataLocation (VOID)
{
    EFI_GUID                            gEfiFirmwareVolumeBlockProtocolGuid = EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL_GUID;
    EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock;
    EFI_PHYSICAL_ADDRESS                Address;
    EFI_HANDLE                          *HandleBuffer;
    EFI_STATUS                          Status;
    UINTN                               i;
    UINTN                               NumHandles;
    void                                *FirstFile;
    void                                *EndOfFiles;
    void                                *File;

    Status = pBS->LocateHandleBuffer(ByProtocol,
                                    &gEfiFirmwareVolumeBlockProtocolGuid,
                                    NULL,
                                    &NumHandles,
                                    &HandleBuffer);
    if (EFI_ERROR(Status)) return Status;

    for (i = 0; i < NumHandles; ++i) {
        Status = pBS->HandleProtocol(HandleBuffer[i],
                                    &gEfiFirmwareVolumeBlockProtocolGuid,
                                    (void **)&FvBlock);
        if (EFI_ERROR(Status)) continue;

        Status = FvBlock->GetPhysicalAddress(FvBlock, &Address);

        if (Status == EFI_SUCCESS) {
            FirstFile = (UINT8*)Address
                        + ((EFI_FIRMWARE_VOLUME_HEADER*)Address)->HeaderLength;

            EndOfFiles = (UINT8*)Address
                        + ((EFI_FIRMWARE_VOLUME_HEADER*)Address)->FvLength;

#if (SMBIOS_X64_BUILD == 0)
            if (EndOfFiles == 0) EndOfFiles = (UINT8*)0xffffffff;   // For x32, 0 = 0x100000000
#endif

            File = FindFile(&FlashDataFile, FirstFile, EndOfFiles);
            if (!File) {
                Status = EFI_NOT_FOUND;
                continue;
            }

            gRomInfo = *(ROM_INFO*)File;
            gRomData = (ROM_INFO*)File + 1;
            break;
        }
    }

    pBS->FreePool(HandleBuffer);
    return Status;
}

/**
    Driver entry point for SmbiosGetFlashData

    @param ImageHandle
    @param SystemTable

    @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
SmbiosGetFlashDataInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS  Status;

    InitAmiLib(ImageHandle, SystemTable);

    Status = GetDataLocation();
    if (EFI_ERROR(Status)) return Status;

    return pBS->InstallMultipleProtocolInterfaces(
                    &ImageHandle,
                    &gAmiSmbiosFlashDataProtocolGuid,&gSmbiosFlashDataProtocol,
                    NULL);
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
