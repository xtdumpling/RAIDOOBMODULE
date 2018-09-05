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

//**********************************************************************//
// $Header:
//
// $Revision:
//
// $Date:
//**********************************************************************//
//**********************************************************************//

#include <Token.h>
#include <AmiDxeLib.h>
#include <FlashPart.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/AmiSmbios.h>
#include "SmbiosDmiEdit.h"
#include "SmbiosDmiEditSsi.h"

#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
#include <Protocol/SmbiosGetFlashDataProtocol.h>
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC

//----------------------------------------------------------------------------
//  External Variables
//----------------------------------------------------------------------------
#if SMBIOS_2X_SUPPORT
extern  SMBIOS_TABLE_ENTRY_POINT    *SmbiosTableEntryPoint;
extern  UINTN                       Smb2xTablePhysAddress;
#endif                                          // SMBIOS_2X_SUPPORT
#if SMBIOS_3X_SUPPORT
extern  SMBIOS_3X_TABLE_ENTRY_POINT *SmbiosV3TableEntryPoint;
extern  UINTN                       Smb3xTablePhysAddress;
#endif                                          // SMBIOS_3X_SUPPORT
extern  UINT8                       *ScratchBufferPtr;
extern  UINT16                      MaximumBufferSize;
extern  VOID                        *StringTable[];
extern  UINTN                       StringTableSize;
#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || (SMBIOS_DMIEDIT_DATA_LOC == 2)
extern  CHAR16                      *DmiArrayVar;
extern  UINT8                       *DmiData;
extern  CHAR16                      *Var;
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC

//----------------------------------------------------------------------------
//  External Function Declaration
//----------------------------------------------------------------------------
extern  VOID EnableShadowWrite();
extern  VOID DisableShadowWrite();
extern  VOID WriteOnceStatusInit(VOID);
extern  BOOLEAN FindStructureType(
                        IN OUT UINT8    **Buffer,
                        IN OUT UINT8    **StructureFoundPtr,
                        IN     UINT8    SearchType,
                        IN     UINT8    Instance        // 1-based
                        );

extern  BOOLEAN FindString(
                        IN OUT UINT8    **BufferPtr,
                        IN     UINT8    StringNumber    // 1-based
                        );

// For Smbios version 2.x
#if (SMBIOS_2X_SUPPORT == 1)
extern UINT16 GetSmbiosV2Info(
                        IN OUT  GET_SMBIOS_INFO     *p
                        );

extern UINT16 GetSmbiosV2Structure(
                        IN OUT  GET_SMBIOS_STRUCTURE    *p
                        );

extern UINT16 SetSmbiosV2Structure(
                        IN OUT  SET_SMBIOS_STRUCTURE     *p
                        );
#endif                                          // SMBIOS_2X_SUPPORT

// For Smbios version 3.x
#if (SMBIOS_3X_SUPPORT == 1)
extern UINT16 GetSmbiosV3Info(
                        IN OUT  GET_SMBIOS_V3_INFO  *p
                        );

extern UINT16 GetSmbiosV3Structure(
                        IN OUT  GET_SMBIOS_V3_STRUCTURE    *p
                        );

extern UINT16 SetSmbiosV3Structure(
                        IN SET_SMBIOS_V3_STRUCTURE  *p
                        );
#endif                                          // SMBIOS_3X_SUPPORT

//----------------------------------------------------------------------------
//  Function Declaration
//----------------------------------------------------------------------------
VOID
GetSmbiosPointerCallback (IN EFI_EVENT Event, IN VOID *Context);

BOOLEAN
IsGotSmbiosPointers (VOID);

VOID
GetSmbiosPointers (VOID);

#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
EFI_STATUS
GetFlashDataLocation(VOID);
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC

VOID
DmiEditSsiVirtAddressChange (
    IN EFI_EVENT Event,
    IN VOID *Context
);

EFI_STATUS
SsiHandler (
    IN  EFI_HANDLE  DispatchHandle,
    IN  CONST VOID  *DispatchContext,
    IN  OUT VOID    *CommBuffer,
    IN  OUT UINTN   *CommBufferSize
);

//----------------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------------
EFI_GUID    DmiEditSsiGuid = DMIEDIT_SSI_GUID;
UINT8       Buffer[2 * FLASH_BLOCK_SIZE];
UINT8       ScratchBuffer[2 * FLASH_BLOCK_SIZE];
UINT8       *gVirtBufferPtr = Buffer;
UINT8       *gFlashBlockPtr;
UINT8       *gVirtFlashBlockPtr;
UINTN       gDmiEditTableSize;
UINTN       gDmiEditTableOffset;            // Offset from start of flash block
UINTN       gCurrentDmiEditOffset;          // Offset from start of flash block
BOOLEAN     DmiEditTableIn2Blocks = FALSE;

EFI_EVENT   SmbiosPointerEvent;
EFI_GUID    EfiSmbiosPointerGuid  = { 0xc4f26989, 0xc1cf, 0x438b, { 0x88, 0x7c, 0x53, 0xd0, 0x98, 0x7d, 0x67, 0x72 } };

//----------------------------------------------------------------------------
//  Functions
//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SmbiosDmiEditSsiInstall
//
// Description: DMIEdit support driver entry point
//
// Input:       IN EFI_HANDLE           ImageHandle,
//              IN EFI_SYSTEM_TABLE     *SystemTable
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
SmbiosDmiEditSsiInstall(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS              Status;
    EFI_HANDLE              SwHandle  = NULL;
    EFI_SMM_SYSTEM_TABLE2   *mSmst;
    EFI_SMM_BASE2_PROTOCOL  *SmmBase;

    InitAmiRuntimeLib(ImageHandle, SystemTable, NULL, DmiEditSsiVirtAddressChange);

    Status = pBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (void**)&SmmBase);
    if (EFI_ERROR(Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmmBase not found!\n"));
#endif
        return Status;
    }

    SmmBase->GetSmstLocation (SmmBase, &mSmst);   // Save the system table pointer

    Status = mSmst->SmiHandlerRegister(SsiHandler, &DmiEditSsiGuid, &SwHandle);
    if (EFI_ERROR(Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to register SsiHandler\n"));
#endif
    }

    WriteOnceStatusInit();

    Status = FlashInit (0x0);
    if (EFI_ERROR (Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "FlashInit failed!\n"));
#endif
        return Status;
    }

	Status = pBS->CreateEventEx(
					EVT_NOTIFY_SIGNAL,
					TPL_CALLBACK,
					GetSmbiosPointerCallback,
					NULL,
					&EfiSmbiosPointerGuid,
					&SmbiosPointerEvent);
    if (EFI_ERROR (Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosDmiEditSsiInstall - Failed to create SmbiosPointerEvent\n"));
#endif
    }

    return EFI_SUCCESS;
}

VOID
GetSmbiosPointerCallback(
    IN EFI_EVENT    Event,
    IN VOID         *Context
)
{
    GetSmbiosPointers();
#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
    GetFlashDataLocation();
    gVirtFlashBlockPtr = gFlashBlockPtr;
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC
    pBS->CloseEvent(Event);
}

// <AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: DmiEditSsiVirtAddressChange
//
// Description: Converts global pointers to virtual addresses
//
// Input:       IN EFI_EVENT    Event       Signalled event
//              IN VOID         *Context    Calling context
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//--------------------------------------------------------------------------
// <AMI_PHDR_END>
VOID
DmiEditSsiVirtAddressChange (
    IN EFI_EVENT    Event,
    IN VOID         *Context
)
{
    UINTN   i;
    VOID    *PtrArray[] = { // Make sure PtrArray is using un-converted pointer
#if (SMBIOS_2X_SUPPORT == 1)
                                &SmbiosTableEntryPoint->TableAddress,
                                &SmbiosTableEntryPoint,
#endif                                          // SMBIOS_2X_SUPPORT
#if (SMBIOS_3X_SUPPORT == 1)
                                &SmbiosV3TableEntryPoint->TableAddress,
                                &SmbiosV3TableEntryPoint,
#endif                                          // SMBIOS_3X_SUPPORT
                                &ScratchBufferPtr,
#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
                                &gFlashData,
                                &gVirtBufferPtr,
                                &gVirtFlashBlockPtr,
#else
                                &DmiArrayVar,
                                &DmiData,
                                &Var,
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC
                                &StringTable
                              };

    if (!IsGotSmbiosPointers()) {
        return;
    }

#if (SMBIOS_2X_SUPPORT == 1)
    Smb2xTablePhysAddress = SmbiosTableEntryPoint->TableAddress;
#endif
#if (SMBIOS_3X_SUPPORT == 1)
    Smb3xTablePhysAddress = SmbiosV3TableEntryPoint->TableAddress;
#endif

    FlashVirtualFixup(pRS);

    for (i = 0; i < sizeof(PtrArray)/sizeof(VOID*); i++) {
        pRS->ConvertPointer(0, PtrArray[i]);
    }

    for (i = 0; i < StringTableSize/sizeof(VOID*); i++) {
        pRS->ConvertPointer(0, StringTable[i]);
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetFlashDataLocation
//
// Description: Find the Flash Data file in the FV.
//
// Input:       None
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
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

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetSmbiosPointers
//
// Description: Get pointers to Smbios Entry Point
//
// Input:       None
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
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
    DEBUG((DEBUG_INFO, "SmbiosDataTable at %016lx\n", SmbiosTableEntryPoint));
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
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   IsGotSmbiosPointers
//
// Description: Check wether getting pointers to Smbios Entry Point
//
// Input:       None
//
// Output:      BOOLEAN
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
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

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SsiHandler
//
// Description: Handles the SMI through SSI
//
// Input:       IN EFI_HANDLE   DispatchHandle
//              IN CONST VOID   *DispatchContextGetFlashDataLocation
//              IN OUT VOID     *CommBuffer
//              IN OUT UINTN    *CommBufferSize
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SsiHandler (
    IN  EFI_HANDLE  DispatchHandle,
    IN  CONST VOID  *DispatchContext,
    IN  OUT VOID    *CommBuffer,        // Pointer to input of FN50-52, 58-5A
    IN  OUT UINTN   *CommBufferSize
)
{
    UINT8           SsiFn;
    VOID            *pInterface;
    UINT16          RetStatus;

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "In Smbios SSI Handler\n"));
#endif

    SsiFn = ((SSI_DMI_CTRL*)CommBuffer)->SsiFun;
    pInterface = (VOID*)((SSI_DMI_CTRL*)CommBuffer)->BuffAddr;   // Pointer to input of FN50-52, 58-5A

    RetStatus = DMI_FUNCTION_NOT_SUPPORTED;

    switch(SsiFn) {
#if (SMBIOS_2X_SUPPORT == 1)
        case 0x50:  if (IsGotSmbiosPointers()) RetStatus = GetSmbiosV2Info(pInterface);
                    ((GET_SMBIOS_INFO*)pInterface)->RetStatus = RetStatus;
                    break;
        case 0x51:  if (IsGotSmbiosPointers()) RetStatus = GetSmbiosV2Structure(pInterface);
                    ((GET_SMBIOS_STRUCTURE*)pInterface)->RetStatus = RetStatus;
                    break;
        case 0x52:  if (IsGotSmbiosPointers()) {
                        EnableShadowWrite();
                        RetStatus = SetSmbiosV2Structure(pInterface);
                        DisableShadowWrite();
                    }
                    ((SET_SMBIOS_V3_STRUCTURE*)pInterface)->RetStatus = RetStatus;
                    break;
#else
        case 0x50:  ((GET_SMBIOS_INFO*)pInterface)->RetStatus = RetStatus;
                    break;
        case 0x51:  ((GET_SMBIOS_STRUCTURE*)pInterface)->RetStatus = RetStatus;
                    break;
        case 0x52:  ((SET_SMBIOS_STRUCTURE*)pInterface)->RetStatus = RetStatus;
                    break;
#endif                                          // SMBIOS_2X_SUPPORT
#if (SMBIOS_3X_SUPPORT == 1)
        case 0x53:  switch(*(UINT8*)pInterface) {
                        case 0x58:  if (IsGotSmbiosPointers()) RetStatus = GetSmbiosV3Info(pInterface);
                        			((GET_SMBIOS_V3_INFO*)pInterface)->RetStatus = RetStatus;
                                    break;
                        case 0x59:  if (IsGotSmbiosPointers()) RetStatus = GetSmbiosV3Structure(pInterface);
                        			((GET_SMBIOS_V3_STRUCTURE*)pInterface)->RetStatus = RetStatus;
                                    break;
                        case 0x5a:  if (IsGotSmbiosPointers()) {
                                        EnableShadowWrite();
                                        FlashDeviceWriteEnable();
                                        RetStatus = SetSmbiosV3Structure(pInterface);
                                        FlashDeviceWriteDisable();
                                        DisableShadowWrite();
                                    }
                                    ((SET_SMBIOS_V3_STRUCTURE*)pInterface)->RetStatus = RetStatus;
                                    break;
                        case 0x5b:  if (IsGotSmbiosPointers()) RetStatus = GetSmbiosV3Table(pInterface);
                        			((GET_SMBIOS_V3_TABLE*)pInterface)->RetStatus = RetStatus;
                                    break;
                    }
#else
        case 0x53:  ((GET_SMBIOS_V3_INFO*)pInterface)->RetStatus = RetStatus;
                    break;
#endif                                          // SMBIOS_3X_SUPPORT
    }

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "Exiting Smbios SSI Handler - Status = %r\n", RetStatus));
#endif

    return EFI_SUCCESS;
}

#if defined(SMBIOS_DMIEDIT_DATA_LOC) && (SMBIOS_DMIEDIT_DATA_LOC == 0)
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   BlockWriteToFlash
//
// Description: Write to the flash part starting at "Address" for a length
//              of "Size".
//
// Input:       IN VOID    *Address,
//              IN VOID    *Data,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
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

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetFlashData
//
// Description: Searches the Flash Data Table for a record of Type and
//              Offset. If found, returns the location found, the data size,
//              and the end of data.
//
// Input:       IN TABLE_INFO   RecordInfo
//
// Output:      FLASH_DATA_INFO
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
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

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   UpdateSmbiosTable
//
// Description: Searches the Flash Data Table for a record of Type and
//              Offset. If found, the existing data will be replaced with
//              the new data, else the data will be added as a new record.
//
// Input:       IN TABLE_INFO  TableInfo,
//              IN UINT8       *Data
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
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
#endif							                // SMBIOS_DMIEDIT_DATA_LOC

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
