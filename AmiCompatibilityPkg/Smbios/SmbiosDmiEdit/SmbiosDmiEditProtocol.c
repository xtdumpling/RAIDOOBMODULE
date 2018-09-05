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

/** @file SmbiosDmiEditProtocol.c
    This file provides DmiEdit Protocols

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <FlashPart.h>
#include <Protocol/AmiSmbios.h>
#include "SmbiosDmiEdit.h"

EFI_HANDLE           			gImageHandle;
EFI_GUID    					gEfiAmiDmieditSmbiosProtocolGuid = AMI_DMIEDIT_SMBIOS_GUID;

EFI_SMBIOS_DMIEDIT_PROTOCOL     SmbiosDmieditProtocol = {DmiEditNonSmiHandler};

//----------------------------------------------------------------------------
//  External Variables
//----------------------------------------------------------------------------
#if (SMBIOS_2X_SUPPORT == 1)
extern SMBIOS_TABLE_ENTRY_POINT	*SmbiosTableEntryPoint;
#endif                                          // SMBIOS_2X_SUPPORT
#if (SMBIOS_3X_SUPPORT == 1)
extern SMBIOS_3X_TABLE_ENTRY_POINT *SmbiosV3TableEntryPoint;
#endif                                          // SMBIOS_3X_SUPPORT
extern UINT8                    *ScratchBufferPtr;
extern UINT16					MaximumBufferSize;

//----------------------------------------------------------------------------
//  External Function Declaration
//----------------------------------------------------------------------------
extern VOID EnableShadowWrite();
extern VOID DisableShadowWrite();

// For Smbios version 2.x
#if (SMBIOS_2X_SUPPORT == 1)
extern UINT16 GetSmbiosV2Info(
    IN OUT  GET_SMBIOS_INFO		*p
);

extern UINT16 GetSmbiosV2Structure(
    IN OUT  GET_SMBIOS_STRUCTURE    *p
);

extern UINT16 SetSmbiosV2Structure(
    IN SET_SMBIOS_STRUCTURE		*p
);
#endif                                          // SMBIOS_2X_SUPPORT

// For Smbios version 3.x
#if (SMBIOS_3X_SUPPORT == 1)
extern UINT16 GetSmbiosV3Info(
    IN OUT  GET_SMBIOS_V3_INFO	*p
);

extern UINT16 GetSmbiosV3Structure(
    IN OUT  GET_SMBIOS_V3_STRUCTURE    *p
);

extern UINT16 SetSmbiosV3Structure(
    IN SET_SMBIOS_V3_STRUCTURE	*p
);
#endif                                          // SMBIOS_3X_SUPPORT

//----------------------------------------------------------------------------
//  Function Declaration
//----------------------------------------------------------------------------
EFI_STATUS
GetFlashDataLocation (VOID);

//----------------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------------
UINT8       Buffer[2 * FLASH_BLOCK_SIZE];
UINT8       *gBufferPtr = Buffer;
UINT8       *gFlashBlockPtr;
UINTN       gDmiEditTableSize;
UINTN       gDmiEditTableOffset;            // Offset from start of flash block
UINTN       gCurrentDmiEditOffset;          // Offset from start of flash block
BOOLEAN     DmiEditTableIn2Blocks = FALSE;

/**
    SmbiosDmiEditProtocol driver entry point

    @param ImageHandle
    @param SystemTable

    @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
SmbiosDmiEditProtocolEntryPoint(
    IN EFI_HANDLE           ImageHandle,
	IN EFI_SYSTEM_TABLE     *SystemTable
)
{
	EFI_STATUS				Status;
    AMI_SMBIOS_PROTOCOL		*AmiSmbiosProtocol;

	InitAmiLib(ImageHandle, SystemTable);
	gImageHandle = ImageHandle;

	DEBUG((DEBUG_INFO, "=====  In SmbiosDmiEditProtocolEntryPoint  =====\n"));
    Status = pBS->LocateProtocol (&gAmiSmbiosProtocolGuid, NULL, (void**)&AmiSmbiosProtocol);
    if (EFI_ERROR (Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "AmiSmbiosProtocol not found!\n"));
#endif
        return Status;
    }

#if (SMBIOS_2X_SUPPORT == 1)
    SmbiosTableEntryPoint = AmiSmbiosProtocol->SmbiosGetTableEntryPoint(2);
#endif                                          // SMBIOS_2X_SUPPORT
#if (SMBIOS_3X_SUPPORT == 1)
    SmbiosV3TableEntryPoint = AmiSmbiosProtocol->SmbiosGetTableEntryPoint(3);
#endif                                          // SMBIOS_3X_SUPPORT
    ScratchBufferPtr = AmiSmbiosProtocol->SmbiosGetScratchBufferPtr();
	MaximumBufferSize = AmiSmbiosProtocol->SmbiosGetBufferMaxSize();

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
    DEBUG((DEBUG_INFO, "Calling GetFlashDataLocation\n"));
    Status = GetFlashDataLocation();
#endif                                  // SMBIOS_DMIEDIT_DATA_LOC

    if (Status == EFI_SUCCESS) {
        pBS->InstallMultipleProtocolInterfaces(
                                 &gImageHandle,
                                 &gEfiAmiDmieditSmbiosProtocolGuid,
                                 &SmbiosDmieditProtocol,
                                 NULL
                                 );
        DEBUG((DEBUG_INFO, "AmiDmieditSmbiosProtocol installed\n"));
    }

    DEBUG((DEBUG_INFO, "=====  Exiting SmbiosDmiEditProtocolEntryPoint  =====\n"));

    return Status;
}

/**
    Get pointers to Smbios Entry Point

    @param  None

    @return None

**/
VOID
GetSmbiosPointers (VOID)
{
    UINTN                   Size;

#if (SMBIOS_2X_SUPPORT == 1)
    Size = sizeof(SMBIOS_TABLE_ENTRY_POINT*);
    pRS->GetVariable(L"SmbiosEntryPointTable",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &SmbiosTableEntryPoint);
#endif                                          // SMBIOS_2X_SUPPORT

#if (SMBIOS_3X_SUPPORT == 1)
    Size = sizeof(SMBIOS_3X_TABLE_ENTRY_POINT*);
    pRS->GetVariable(L"SmbiosV3EntryPointTable",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &SmbiosV3TableEntryPoint);
#endif                                          // SMBIOS_3X_SUPPORT

    Size = sizeof(UINT8*);
    pRS->GetVariable(L"SmbiosScratchBuffer",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &ScratchBufferPtr);

    Size = sizeof(UINT16);
    pRS->GetVariable(L"MaximumTableSize",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &MaximumBufferSize);
}

/**
    Handler for SmbiosDmiEditProtocol

    @param Data
    @param pCommBuff

    @return UINT32 Protocol status

**/
UINT32
DmiEditNonSmiHandler(
    IN UINT8    Data,
    IN UINT64   pCommBuff
)
{
    UINT32          eax;

#if (SMBIOS_2X_SUPPORT == 1) || (SMBIOS_3X_SUPPORT == 1)
    static BOOLEAN  ValidPointers = FALSE;

    DEBUG((DEBUG_INFO, "Non-SMI SMBIOSHandler Data: 0x%X \n",Data));

    pInterface = (void*)pCommBuff;

    if (!ValidPointers) {
        GetSmbiosPointers();
        ValidPointers = TRUE;
    }
#endif

    switch(Data) {
#if (SMBIOS_2X_SUPPORT == 1)
        case 0x50:  eax = GetSmbiosV2Info(pInterface);
                    break;
        case 0x51:  eax = GetSmbiosV2Structure(pInterface);
                    break;
        case 0x52:  EnableShadowWrite();
                    eax = SetSmbiosV2Structure(pInterface);
                    DisableShadowWrite();
                    break;
#endif                                          // SMBIOS_2X_SUPPORT
#if (SMBIOS_3X_SUPPORT == 1)
        case 0x58:  eax = GetSmbiosV3Info(pInterface);
                    break;
        case 0x59:  eax = GetSmbiosV3Structure(pInterface);
                    break;
        case 0x5a:  EnableShadowWrite();
                    eax = SetSmbiosV3Structure(pInterface);
                    DisableShadowWrite();
                    break;
#endif                                          // SMBIOS_3X_SUPPORT
        default:    eax = DMI_FUNCTION_NOT_SUPPORTED;
    }

    return eax;
}

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
/**
    Find the Flash Data file in the FV

    @param None

    @return UINT32 Protocol status

**/
EFI_STATUS
GetFlashDataLocation (VOID)
{
    EFI_STATUS      Status;
    UINTN           i;
    UINTN           index;
    UINT32          *BufferPtr;
    BOOLEAN         DmiEditDataFound = FALSE;

    DEBUG((DEBUG_INFO, "=====  In GetFlashDataLocation  =====\n"));
    for (i = 0; i < FV_BB_BLOCKS; i++) {
        gFlashBlockPtr = FLASHPART_BASE_ADDR + (UINT8*)(i * FLASH_BLOCK_SIZE);
        DEBUG((DEBUG_INFO, "gFlashBlockPtr = %x\n", gFlashBlockPtr));
        FlashRead(gFlashBlockPtr, Buffer, 2*FLASH_BLOCK_SIZE);

        for (index = 0; index < FLASH_BLOCK_SIZE; index += 4) {
            BufferPtr = (UINT32*)&Buffer[index];
            if (*BufferPtr == 0x5f415342) {     // "_ASB"
                gDmiEditTableSize = *(UINT16*)&Buffer[index + 4];
                gDmiEditTableOffset = index + 8;
                DEBUG((DEBUG_INFO, "Flash Data found at %x, offset = %x, size = %x\n", gFlashBlockPtr, gDmiEditTableOffset, gDmiEditTableSize));

                // if DmiEdit data table spans across block boundary, read next block also
                if ((gDmiEditTableOffset + gDmiEditTableSize) > FLASH_BLOCK_SIZE) {
                    DmiEditTableIn2Blocks = TRUE;
                    FlashRead(gFlashBlockPtr + FLASH_BLOCK_SIZE, &Buffer[FLASH_BLOCK_SIZE], FLASH_BLOCK_SIZE);
                }

                DmiEditDataFound = TRUE;
                break;
            }
        }

        if (DmiEditDataFound) {
            Status = EFI_SUCCESS;
            break;
        }

        Status = EFI_NOT_FOUND;
    }

    DEBUG((DEBUG_INFO, "=====  Exiting GetFlashDataLocation  =====\n"));
    return Status;
}

/**
    Write to the flash part starting at "Address" for a length of "Size"

    @param *FlashBlockAddress
    @param *Data

    @return UINT32 Protocol status

**/
EFI_STATUS
BlockWriteToFlash(
    IN VOID    *FlashBlockAddress,
    IN VOID    *Data
)
{
    EFI_STATUS  Status;

    FlashDeviceWriteEnable();
    FlashBlockWriteEnable(gFlashBlockPtr);

    FlashEraseBlock(gFlashBlockPtr);

    if (FlashProgram(gFlashBlockPtr, Data, FLASH_BLOCK_SIZE)) {
        Status = EFI_SUCCESS;
    }
    else {
        Status = EFI_INVALID_PARAMETER;
    }

    FlashBlockWriteDisable(gFlashBlockPtr);
    FlashDeviceWriteDisable();

    return Status;
}

/**
    Searches the Flash Data Table for a record of Type and
    Offset. If found, the existing data will be replaced with
    the new data, else the data will be added as a new record.

    @param TABLE_INFO

    @return FLASH_DATA_INFO

**/
FLASH_DATA_INFO
GetFlashData(
    IN TABLE_INFO   *RecordInfo
)
{

    TABLE_INFO          *FlashDataPtr;
    FLASH_DATA_INFO     FlashDataInfo = {0, 0, 0};

    FlashDataPtr = (TABLE_INFO*)&Buffer[gDmiEditTableOffset];
    gCurrentDmiEditOffset = 0;

    while (FlashDataPtr->Handle != 0xffff) {
        if (FlashDataPtr->Type == RecordInfo->Type &&
            FlashDataPtr->Handle == RecordInfo->Handle &&
            FlashDataPtr->Offset == RecordInfo->Offset &&
            FlashDataPtr->Flags == RecordInfo->Flags) {
            FlashDataInfo.Location = (UINT8*)FlashDataPtr;
            FlashDataInfo.Size = FlashDataPtr->Size;
            gCurrentDmiEditOffset = (UINT8*)FlashDataPtr - Buffer;
        }

        FlashDataPtr = (TABLE_INFO*)((UINT8*)(FlashDataPtr + 1) + FlashDataPtr->Size);
    }

    FlashDataInfo.EndOfData = (UINT8*)FlashDataPtr;
    if (gCurrentDmiEditOffset == 0) {
        gCurrentDmiEditOffset = (UINT8*)FlashDataPtr - Buffer;
    }

    return FlashDataInfo;
}

/**
    Searches the Flash Data Table for a record of Type and
    Offset. If found, the existing data will be replaced with
    the new data, else the data will be added as a new record.

    @param *TableInfo
    @param *Data

    @retval UINT16 Status

**/
UINT16
UpdateSmbiosTable(
    IN TABLE_INFO  *TableInfo,
    IN UINT8       *Data
)
{
    EFI_STATUS          Status;
    FLASH_DATA_INFO     FlashDataInfo;
    UINTN               SpaceAvailable;
    UINT8               *BufferPtr = NULL;
    UINTN               i;
    UINTN               j;
    UINTN               LastDataIndex;
    UINT8               InputDataBuffer[0x80];

    DEBUG((DEBUG_INFO, "===  In UpdateSmbiosTable  ===\n"));

    FlashDataInfo = GetFlashData(TableInfo);
    DEBUG((DEBUG_INFO, "FlashDataInfo.Location = 0x%016lx\n", FlashDataInfo.Location));
    DEBUG((DEBUG_INFO, "FlashDataInfo.Size = 0x%016lx\n", FlashDataInfo.Size));
    DEBUG((DEBUG_INFO, "FlashDataInfo.EndOfData = 0x%016lx\n", FlashDataInfo.EndOfData));

    // Check Size
    SpaceAvailable = gDmiEditTableSize - (FlashDataInfo.EndOfData - Buffer);
    if (FlashDataInfo.Location) SpaceAvailable += FlashDataInfo.Size + sizeof(TABLE_INFO);
    if (sizeof(TABLE_INFO) + TableInfo->Size > SpaceAvailable) {
        return DMI_ADD_STRUCTURE_FAILED;
    }

    DEBUG((DEBUG_INFO, "Copying TableInfo header into input buffer\n"));
    // Copy TableInfo header into input Buffer
    BufferPtr = InputDataBuffer;
    *(TABLE_INFO *)BufferPtr = *TableInfo;
    BufferPtr += sizeof(TABLE_INFO);

    DEBUG((DEBUG_INFO, "Adding data to input buffer\n"));
    // Add data to input buffer
    for (i = 0; i < TableInfo->Size; ++i) {
        *BufferPtr++ = Data[i];
    }

    DEBUG((DEBUG_INFO, "Copy input data to current location\n"));
    // Copy input data to current location
    for (i = 0, j = gCurrentDmiEditOffset; i < TableInfo->Size + sizeof(TABLE_INFO); i++, j++) {
        Buffer[j] = InputDataBuffer[i];
    }
    LastDataIndex = j;

    DEBUG((DEBUG_INFO, "Clear the rest of DmiEdit data in scratch buffer to 0xFF\n"));
    // Clear the rest of DmiEdit data in scratch buffer to 0xFF
    for (i = gDmiEditTableOffset + gDmiEditTableSize - j; i < 0; i--, j++) {
        Buffer[j] = 0xff;
    }

    if (FlashDataInfo.Location) {
        // Skip over old data
        BufferPtr = FlashDataInfo.Location;
        BufferPtr += ((TABLE_INFO*)BufferPtr)->Size + sizeof(TABLE_INFO);
        while (BufferPtr < FlashDataInfo.EndOfData) {
            Buffer[LastDataIndex++] = *BufferPtr++;
        }
    }

    DEBUG((DEBUG_INFO, "Writing to flash - 1st block\n"));
    Status = BlockWriteToFlash(gFlashBlockPtr, gBufferPtr);

    // if DmiEdit data table spans across block boundary, flash the next block
    if ((Status == EFI_SUCCESS) && DmiEditTableIn2Blocks) {
        DEBUG((DEBUG_INFO, "Writing to flash - 2nd block\n"));
        Status = BlockWriteToFlash(gFlashBlockPtr + FLASH_BLOCK_SIZE, gBufferPtr + FLASH_BLOCK_SIZE);
    }

    if (EFI_ERROR(Status)) {
        return DMI_BAD_PARAMETER;
    }
    else {
        return DMI_SUCCESS;
    }
}
#endif							// SMBIOS_DMIEDIT_DATA_LOC

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
