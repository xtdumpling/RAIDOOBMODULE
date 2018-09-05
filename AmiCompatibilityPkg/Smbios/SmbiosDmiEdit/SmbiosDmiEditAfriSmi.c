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

/** @file SmbiosDmiEditAfri.c
    This file contains AMI Firmware Runtime Interface (AFRI) registration
    and handler codes

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <FlashPart.h>
#include <Protocol/AmiFri.h>
#include <Protocol/AmiSmbios.h>
#include <Library/AmiBufferValidationLib.h>
#include "SmbiosDmiEdit.h"
#include "SmbiosDmiEditAfri.h"

//----------------------------------------------------------------------------
//  External Variables
//----------------------------------------------------------------------------
#if SMBIOS_2X_SUPPORT
extern  SMBIOS_TABLE_ENTRY_POINT    *SmbiosTableEntryPoint;
#endif                                          // SMBIOS_2X_SUPPORT
#if SMBIOS_3X_SUPPORT
extern  SMBIOS_3X_TABLE_ENTRY_POINT *SmbiosV3TableEntryPoint;
#endif                                          // SMBIOS_3X_SUPPORT
extern  UINT8                       *ScratchBufferPtr;
extern  UINT16                      MaximumBufferSize;

//----------------------------------------------------------------------------
//  External Function Declaration
//----------------------------------------------------------------------------
extern  VOID EnableShadowWrite();
extern  VOID DisableShadowWrite();
extern  VOID WriteOnceStatusInit(VOID);

extern  UINT16  GetStructureLength(
                        IN UINT8     *BufferStart
                        );

extern  UINT8*  GetStructureByHandleThenUpdateHandle(
                        IN OUT  UINT16    *Handle
                        );

//----------------------------------------------------------------------------
//  Function Declaration
//----------------------------------------------------------------------------
BOOLEAN
IsGotSmbiosPointers (VOID);

VOID
GetSmbiosPointers (VOID);

#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
EFI_STATUS
GetFlashDataLocation(VOID);
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC == 0

EFI_STATUS
AfriSmbiosSmiHandler (
    IN EFI_HANDLE   DispatchHandle,
    IN CONST VOID   *DispatchContext,
    IN OUT VOID     *CommBuffer,
    IN OUT UINTN    *CommBufferSize
);

UINT16
GetSmbiosInfo(
    IN OUT AFRI_GET_SMBIOS_INFO         *CommBuffer
);

UINT16
GetSmbiosStructure(
    IN OUT AFRI_GET_SMBIOS_STRUCTURE    *GetStructurePtr,
    IN      UINTN                       *CommBufferSize
);

UINT16
AfriSetSmbiosStructure(
    IN OUT AFRI_SET_SMBIOS_STRUCTURE    *CommBuffer
);

UINT16
GetSmbiosTable(
    IN OUT AFRI_GET_SMBIOS_TABLE        *CommBuffer,
    IN      UINTN                       *CommBufferSize
);

//----------------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------------
EFI_GUID    gAfriDmiEditGuid = AFRI_DMIEDIT_GUID;
UINT8       Buffer[2 * FLASH_BLOCK_SIZE];
UINT8       ScratchBuffer[2 * FLASH_BLOCK_SIZE];
UINT8       *gVirtBufferPtr = Buffer;
UINT8       *gFlashBlockPtr;
UINT8       *gVirtFlashBlockPtr;
UINTN       gDmiEditTableSize;
UINTN       gDmiEditTableOffset;                // Offset from start of flash block
UINTN       gCurrentDmiEditOffset;              // Offset from start of flash block
BOOLEAN     DmiEditTableIn2Blocks = FALSE;

#if (AMI_MODULE_PKG_VERSION < 25)
    EFI_PHYSICAL_ADDRESS            TsegStart = 0;
    EFI_PHYSICAL_ADDRESS            TsegEnd = 0;
#else
    #include <Library/AmiBufferValidationLib.h>
#endif                                          // (AMI_MODULE_PKG_VERSION < 25)

//----------------------------------------------------------------------------
//  Functions
//----------------------------------------------------------------------------

/**
    SmbiosDmiEditAfri support driver entry point

    @param  ImageHandle
    @param  SystemTable

    @return EFI_STATUS

**/

EFI_STATUS
SmbiosDmiEditAfriSmiInstall(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS              Status;
    EFI_HANDLE              AfriDmiEditHandle = NULL;
    EFI_SMM_SYSTEM_TABLE2   *Smst2;
    EFI_SMM_BASE2_PROTOCOL  *SmmBase2Protocol;

#if (AMI_MODULE_PKG_VERSION < 25)
    EFI_STATUS              Status;
    CPUINFO_HOB             *CpuInfoHob = NULL;
    EFI_GUID                CpuInfoHobGuid = AMI_CPUINFO_HOB_GUID;
    EFI_GUID                HobListGuid = HOB_LIST_GUID;

    InitAmiLib(ImageHandle, SystemTable);

    CpuInfoHob = (CPUINFO_HOB*)GetEfiConfigurationTable(pST, &HobListGuid);
    if (CpuInfoHob != NULL) {
        Status = FindNextHobByGuid(&CpuInfoHobGuid,(VOID**)&CpuInfoHob);
        if (Status == EFI_SUCCESS) {
            TsegStart = CpuInfoHob->TsegAddress;
            TsegEnd = CpuInfoHob->TsegAddress + CpuInfoHob->TsegSize;
        }
    }
#else
    InitAmiLib(ImageHandle, SystemTable);
#endif                                          // (AMI_MODULE_PKG_VERSION < 25)

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "In SmbiosDmiEditAfriSmiInstall\n"));
#endif

    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid,
                                 NULL,
                                 (VOID **)&SmmBase2Protocol
                                );
    if (EFI_ERROR(Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR, "SmmBase2Protocol not found!\n"));
#endif
        return Status;
    }

    SmmBase2Protocol->GetSmstLocation (SmmBase2Protocol, &Smst2);   // Save the system table pointer

    Status = Smst2->SmiHandlerRegister(AfriSmbiosSmiHandler, &gAfriDmiEditGuid, &AfriDmiEditHandle);
    if (Status == EFI_SUCCESS) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_INFO, "Successfully registering AfriSmbiosSmiHandler\n"));
#endif
        WriteOnceStatusInit();
    }

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "Exitting SmbiosDmiEditAfriSmiInstall\n"));
#endif
    return Status;
}

#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
/**
    Find the Flash Data file in the FV

    @param  None

    @return EFI_STATUS

**/
EFI_STATUS
GetFlashDataLocation (VOID)
{
    EFI_STATUS      Status;
    UINTN           i;
    UINTN           index;
    UINT32          *BufferPtr;
    BOOLEAN         DmiEditDataFound = FALSE;

    for (i = 0; i < FV_BB_BLOCKS; i++) {
        gFlashBlockPtr = FLASH_BB_BASE_ADDR + (UINT8*)(i * FLASH_BLOCK_SIZE);
        FlashRead(gFlashBlockPtr, Buffer, FLASH_BLOCK_SIZE);

        for (index = 0; index < FLASH_BLOCK_SIZE; index += 4) {
            BufferPtr = (UINT32*)&Buffer[index];
            if (*BufferPtr == 0x5f415342) {     // "_ASB"
                gDmiEditTableSize = *(UINT16*)&Buffer[index + 4];
                gDmiEditTableOffset = index + 8;

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

    return Status;
}
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC == 0

/**
    Get various pointers from NVRAM

    @param  None

    @return None

**/
VOID
GetSmbiosPointers (VOID)
{
    UINTN       Size;

#if SMBIOS_2X_SUPPORT
    Size = sizeof(SMBIOS_TABLE_ENTRY_POINT*);
    pRS->GetVariable(L"SmbiosEntryPointTable",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &SmbiosTableEntryPoint);
#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "SmbiosEntryPointTable at %016lx\n", SmbiosTableEntryPoint));
#endif
#endif                                          // SMBIOS_2X_SUPPORT

#if SMBIOS_3X_SUPPORT
    Size = sizeof(SMBIOS_3X_TABLE_ENTRY_POINT*);
    pRS->GetVariable(L"SmbiosV3EntryPointTable",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &SmbiosV3TableEntryPoint);
#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "SmbiosV3EntryPointTable at %016lx\n", SmbiosV3TableEntryPoint));
#endif
#endif                                          // SMBIOS_3X_SUPPORT

    Size = sizeof(UINT8*);
    pRS->GetVariable(L"SmbiosScratchBuffer",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &ScratchBufferPtr);
#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "Scratch Buffer at %016lx\n", ScratchBufferPtr));
#endif

    Size = sizeof(UINT16);
    pRS->GetVariable(L"MaximumTableSize",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &MaximumBufferSize);

#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
    GetFlashDataLocation();
    gVirtFlashBlockPtr = gFlashBlockPtr;
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC == 0
}

/**
    Check if all required pointers are valid

    @param  None

    @return Boolean
                TRUE  All pointers are valid
                FALSE Invalid pointer
**/
BOOLEAN
IsGotSmbiosPointers (VOID)
{
    UINT32 IsNotGot = 0;

#if SMBIOS_2X_SUPPORT
    if (SmbiosTableEntryPoint == NULL) {
        IsNotGot |= 0x1 << 0;
    }
#endif                                          // SMBIOS_2X_SUPPORT

#if SMBIOS_3X_SUPPORT
    if (SmbiosV3TableEntryPoint == NULL) {
        IsNotGot |= 0x1 << 1;
    }
#endif                                          // SMBIOS_3X_SUPPORT

    if (ScratchBufferPtr == NULL) {
        IsNotGot |= 0x1 << 2;
    }

    if (MaximumBufferSize == 0) {
        IsNotGot |= 0x1 << 3;
    }

    if (IsNotGot) {
        return FALSE;
    }

    return TRUE;
}

/**
    Handles the SMI through AFRI

    @param  EFI_HANDLE  DispatchHandle
            CONST VOID  *Context
            OUT VOID    *CommBuffer
            OUT UINT    *CommBufferSize

    @return EFI_STATUS

**/
EFI_STATUS
AfriSmbiosSmiHandler (
    IN       EFI_HANDLE DispatchHandle,
    IN CONST VOID       *Context,
    IN OUT   VOID       *CommBuffer,            // Pointer to input of FN60-63
    IN       UINTN      *CommBufferSize
)
{
    UINT16  RetStatus;

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "In AfriSmbiosSmiHandler\n"));
#endif

    if (!IsGotSmbiosPointers()) {               // Don't overwrite the pointer, if got before
        GetSmbiosPointers();
    }

    RetStatus = DMI_FUNCTION_NOT_SUPPORTED;
    switch(*(UINT8*)CommBuffer) {
        case 0x60:  RetStatus = GetSmbiosInfo((AFRI_GET_SMBIOS_INFO*)CommBuffer);
                    break;
        case 0x61:  RetStatus = GetSmbiosStructure((AFRI_GET_SMBIOS_STRUCTURE*)CommBuffer, CommBufferSize);
                    break;
        case 0x62:  EnableShadowWrite();
                    FlashDeviceWriteEnable();
                    RetStatus = AfriSetSmbiosStructure((AFRI_SET_SMBIOS_STRUCTURE*)CommBuffer);
                    FlashDeviceWriteDisable();
                    DisableShadowWrite();
                    break;
        case 0x63:  RetStatus = GetSmbiosTable((AFRI_GET_SMBIOS_TABLE*)CommBuffer, CommBufferSize);
                    break;
    }

    *(UINT16*)((UINT8*)CommBuffer + 1) = RetStatus;

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "Exiting Smbios SSI Handler - Status = %r\n", RetStatus));
#endif

    return EFI_SUCCESS;
}

/**
    Returns the SMBIOS information

    @param[in, out] p - Pointer to AFRI_GET_SMBIOS_INFO structure

    @return UINT16 - 0 for success, input buffer p updated with
                     Smbios Table info
                   - DMI_FUNCTION_NOT_SUPPORTED for error

**/
UINT16
GetSmbiosInfo(
    IN OUT  AFRI_GET_SMBIOS_INFO   *CommBuffer
)
{
#if (SMBIOS_2X_SUPPORT == 1)
    if (!SmbiosTableEntryPoint) return DMI_FUNCTION_NOT_SUPPORTED;

    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->DmiBiosRevision, 1) == EFI_SUCCESS) {
        CommBuffer->DmiBiosRevision = (SMBIOS_2X_MAJOR_VERSION << 4) | SMBIOS_2X_MINOR_VERSION;
    }
    else {
        return DMI_BAD_PARAMETER;
    }
    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->NumberOfStructures, 2) == EFI_SUCCESS) {
        CommBuffer->NumberOfStructures = GetNumberOfStructures((UINT8*)SmbiosTableEntryPoint->TableAddress);
    }
    else {
        return DMI_BAD_PARAMETER;
    }
    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->LargestStructureSize, 2) == EFI_SUCCESS) {
        CommBuffer->LargestStructureSize = GetLargestStructureSize((UINT8*)SmbiosTableEntryPoint->TableAddress);
    }
    else {
        return DMI_BAD_PARAMETER;
    }
    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->DmiStorageBase, 4) == EFI_SUCCESS) {
        CommBuffer->DmiStorageBase = SmbiosTableEntryPoint->TableAddress;
    }
    else {
        return DMI_BAD_PARAMETER;
    }
    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->DmiStorageSize, 2) == EFI_SUCCESS) {
        CommBuffer->DmiStorageSize = MaximumBufferSize;
    }
    else {
        return DMI_BAD_PARAMETER;
    }

    return DMI_SUCCESS;
#endif                                          // SMBIOS_2X_SUPPORT

#if (SMBIOS_3X_SUPPORT == 1)
    if (!SmbiosV3TableEntryPoint) return DMI_FUNCTION_NOT_SUPPORTED;

    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->DmiBiosRevision, 1) == EFI_SUCCESS) {
        CommBuffer->DmiBiosRevision = (SMBIOS_3X_MAJOR_VERSION << 4) | SMBIOS_3X_MINOR_VERSION;
    }
    else {
        return DMI_BAD_PARAMETER;
    }
    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->NumberOfStructures, 2) == EFI_SUCCESS) {
        CommBuffer->NumberOfStructures = GetNumberOfStructures((UINT8*)SmbiosV3TableEntryPoint->TableAddress);
    }
    else {
        return DMI_BAD_PARAMETER;
    }
    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->LargestStructureSize, 2) == EFI_SUCCESS) {
        CommBuffer->LargestStructureSize = GetLargestStructureSize((UINT8*)SmbiosV3TableEntryPoint->TableAddress);
    }
    else {
        return DMI_BAD_PARAMETER;
    }
    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->DmiStorageBase, 8) == EFI_SUCCESS) {
        CommBuffer->DmiStorageBase = (UINT64)SmbiosV3TableEntryPoint->TableAddress;
    }
    else {
        return DMI_BAD_PARAMETER;
    }
    if (AmiValidateMemoryBuffer((VOID*)CommBuffer->DmiStorageSize, 2) == EFI_SUCCESS) {
        CommBuffer->DmiStorageSize = MaximumBufferSize;
    }
    else {
        return DMI_BAD_PARAMETER;
    }

    return DMI_SUCCESS;
#endif                                          // SMBIOS_3X_SUPPORT
}

/**
    Searches the structure table for a record with its handle
    equal to the input Handle and copies its content into
    the provided buffer.

    @param[in, out] GetStructurePtr - Pointer to AFRI_GET_SMBIOS_STRUCTURE structure
    @param  Size

    @return UINT16 - 0 for success, input buffer p updated with
                     Smbios structure
                   - Error values:
                       DMI_FUNCTION_NOT_SUPPORTED
                       DMI_INVALID_HANDLE
                       DMI_BUFFER_TOO_SMALL

**/
UINT16
GetSmbiosStructure(
    IN OUT  AFRI_GET_SMBIOS_STRUCTURE   *CommBuffer,
    IN      UINTN                       *CommBufferSize
)
{
    UINT8       *SmbStructurePtr;
    UINT16      StructureSize;

#if (SMBIOS_2X_SUPPORT == 1)
    if (!SmbiosTableEntryPoint) return DMI_FUNCTION_NOT_SUPPORTED;
#elif (SMBIOS_3X_SUPPORT == 1)
    if (!SmbiosV3TableEntryPoint) return DMI_FUNCTION_NOT_SUPPORTED;
#else
    return DMI_FUNCTION_NOT_SUPPORTED;
#endif                                          // SMBIOS_2X_SUPPORT

    if (AmiValidateMemoryBuffer((VOID*)&CommBuffer->Handle, 2) == EFI_SUCCESS) {
        SmbStructurePtr = GetStructureByHandleThenUpdateHandle(&CommBuffer->Handle);
        if (!SmbStructurePtr) return DMI_INVALID_HANDLE;
    }
    else {
        return DMI_BAD_PARAMETER;
    }

    StructureSize = GetStructureLength(SmbStructurePtr);

    if ((*(UINT16*)CommBufferSize - sizeof(AFRI_GET_SMBIOS_STRUCTURE)) >= StructureSize) {
        UINT8       *dest;

        // Copy structure to input buffer
        dest = (UINT8*)CommBuffer + sizeof(AFRI_GET_SMBIOS_STRUCTURE);
        if (AmiValidateMemoryBuffer((VOID*)dest, (UINTN)StructureSize) == EFI_SUCCESS) {
            while(StructureSize--) *dest++ = *SmbStructurePtr++;
        }
        else {
            return DMI_BAD_PARAMETER;
        }
    }
    else {
        return DMI_BUFFER_TOO_SMALL;
    }

    return DMI_SUCCESS;
}

/**
    DMIEdit function to update the structures and saves the
    DMI data in the Flash Part for subsequent boot.

    @param Pointer to SMI function parameters

    @return Status

**/
UINT16
AfriSetSmbiosStructure(
    IN OUT  AFRI_SET_SMBIOS_STRUCTURE   *CommBuffer
)
{
    return SetSmbiosStructure(
                    (SET_SMBIOS_STRUCTURE_DATA*)&CommBuffer->Command,
                    CommBuffer->Control
                    );
}

/**
    Fills input buffer with contect of SMBIOS 3.x Table

    @param Pointer to SMI function parameters

    @return UINT16 Status (0 for success)

**/
UINT16
GetSmbiosTable(
    IN OUT  AFRI_GET_SMBIOS_TABLE   *CommBuffer,
    IN      UINTN                   *CommBufferSize
)
{
    UINT8       *BufferPtr;

    if ((*CommBufferSize - sizeof(AFRI_GET_SMBIOS_TABLE)) < MaximumBufferSize) return DMI_BUFFER_TOO_SMALL;

    BufferPtr = (UINT8*)CommBuffer + sizeof(AFRI_GET_SMBIOS_TABLE);

#if (SMBIOS_2X_SUPPORT == 1)
    if (!SmbiosTableEntryPoint) return DMI_FUNCTION_NOT_SUPPORTED;
    MemCpy(BufferPtr, (UINT8*)SmbiosTableEntryPoint->TableAddress, MaximumBufferSize);
#else
    if (!SmbiosV3TableEntryPoint) return DMI_FUNCTION_NOT_SUPPORTED;
    MemCpy(BufferPtr, (UINT8*)SmbiosV3TableEntryPoint->TableAddress, MaximumBufferSize);
#endif                                          // SMBIOS_2X_SUPPORT

    return DMI_SUCCESS;
}

#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
/**
    Write to the flash part starting at "Address" for a length
    of "Size"

    @param  VOID    *FlashBlockAddress
            VOID    *Data

    @return EFI_STATUS

**/
EFI_STATUS
BlockWriteToFlash(
    IN VOID    *FlashBlockAddress,
    IN VOID    *Data
)
{
    EFI_STATUS  Status;

    FlashDeviceWriteEnable();
    FlashBlockWriteEnable(FlashBlockAddress);

    FlashEraseBlock(FlashBlockAddress);

    if (FlashProgram(FlashBlockAddress, Data, FLASH_BLOCK_SIZE)) {
        Status = EFI_SUCCESS;
    }
    else {
        Status = EFI_INVALID_PARAMETER;
    }

    FlashBlockWriteDisable(FlashBlockAddress);
    FlashDeviceWriteDisable();

    return Status;
}

/**
    Searches the Flash Data Table for a record of Type and
    Offset. If found, returns the location found, the data size,
    and the end of data

    @param  TABLE_INFO  *RecordInfo

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
    the new data, else the data will be added as a new record

    @param  TABLE_INFO  *RecordInfo
    @param  UINT8       *Data

    @return UINT16      Status

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

    FlashDataInfo = GetFlashData(TableInfo);

    // Check Size
    SpaceAvailable = gDmiEditTableSize - (FlashDataInfo.EndOfData - Buffer);
    if (FlashDataInfo.Location) SpaceAvailable += FlashDataInfo.Size + sizeof(TABLE_INFO);
    if (sizeof(TABLE_INFO) + TableInfo->Size > SpaceAvailable) {
        return DMI_ADD_STRUCTURE_FAILED;
    }

    // Copy TableInfo header into input Buffer
    BufferPtr = InputDataBuffer;
    *(TABLE_INFO *)BufferPtr = *TableInfo;
    BufferPtr += sizeof(TABLE_INFO);

    // Add data to input buffer
    for (i = 0; i < TableInfo->Size; ++i) {
        *BufferPtr++ = Data[i];
    }

    // Copy flash block (2 blocks) to scratch buffer
    MemCpy(ScratchBuffer, Buffer, 2 * FLASH_BLOCK_SIZE);

    // Copy input data to current location
    for (i = 0, j = gCurrentDmiEditOffset; i < TableInfo->Size + sizeof(TABLE_INFO); i++, j++) {
        ScratchBuffer[j] = InputDataBuffer[i];
    }
    LastDataIndex = j;

    // Clear the rest of DmiEdit data in scratch buffer to 0xFF
    for (i = gDmiEditTableOffset + gDmiEditTableSize - j; i < 0; i--, j++) {
        ScratchBuffer[j] = 0xff;
    }

    if (FlashDataInfo.Location) {
        // Skip over old data
        BufferPtr = FlashDataInfo.Location;
        BufferPtr += ((TABLE_INFO*)BufferPtr)->Size + sizeof(TABLE_INFO);
        while (BufferPtr < FlashDataInfo.EndOfData) {
            ScratchBuffer[LastDataIndex++] = *BufferPtr++;
        }
    }

    // Copy ScratchBuffer to Buffer (2 blocks)
    MemCpy(Buffer, ScratchBuffer, 2 * FLASH_BLOCK_SIZE);

    Status = BlockWriteToFlash(gVirtFlashBlockPtr, gVirtBufferPtr);

    // if DmiEdit data table spans across block boundary, flash the next block
    if ((Status == EFI_SUCCESS) && DmiEditTableIn2Blocks) {
        Status = BlockWriteToFlash(gVirtFlashBlockPtr + FLASH_BLOCK_SIZE, gVirtBufferPtr + FLASH_BLOCK_SIZE);
    }

    if (EFI_ERROR(Status)) {
        return DMI_BAD_PARAMETER;
    }
    else {
        return DMI_SUCCESS;
    }
}
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC == 0

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
