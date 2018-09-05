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

/** @file SmbiosDmiEdit.c
    This file contains SMI registration and handler codes

**/

#include <AmiDxeLib.h>
#include <Token.h>
#include <AmiHobs.h>
#include <Smm.h>
#include <AmiSmm.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/AmiSmbios.h>
#include "SmbiosDmiEdit.h"

#define FLASH_DEVICE_BASE (0xFFFFFFFF - FLASH_SIZE + 1)

EFI_GUID						gSwSmiCpuTriggerGuid = SW_SMI_CPU_TRIGGER_GUID;

EFI_SMM_SYSTEM_TABLE2			*mSmst;

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || (SMBIOS_DMIEDIT_DATA_LOC != 2)
#include <Protocol/SmbiosGetFlashDataProtocol.h>
#include <Protocol/SmiFlash.h>
#include <Protocol/FlashProtocol.h>

FLASH_PROTOCOL 					*mFlash = NULL;
EFI_SMBIOS_FLASH_DATA_PROTOCOL  *mSmbiosFlashDataProtocol;
EFI_SMI_FLASH_PROTOCOL          *mSmiFlash;
#endif                                          // SMBIOS_DMIEDIT_DATA_LOC

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
extern VOID EnableShadowWrite();
extern VOID DisableShadowWrite();
extern VOID GetSmbiosTableF000 (VOID);
extern VOID WriteOnceStatusInit(VOID);

// For Smbios version 2.x
#if (SMBIOS_2X_SUPPORT == 1)
extern UINT16 GetSmbiosV2Info(
    IN OUT  GET_SMBIOS_INFO     *p
);

extern UINT16 GetSmbiosV2Structure(
    IN OUT  GET_SMBIOS_STRUCTURE    *p
);

extern UINT16 SetSmbiosV2Structure(
    IN SET_SMBIOS_STRUCTURE     *p
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

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || (SMBIOS_DMIEDIT_DATA_LOC != 2)
extern FLASH_DATA_INFO GetFlashDataInfo(
    IN TABLE_INFO   *RecordInfo
);
#endif

//----------------------------------------------------------------------------
//  Function Declaration
//----------------------------------------------------------------------------
EFI_STATUS
InSmmFunction(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
);

EFI_STATUS
SmiHandler (
    IN  EFI_HANDLE  DispatchHandle,
    IN  CONST VOID	*DispatchContext,
    IN  OUT	VOID	*CommBuffer,
    IN  OUT	UINTN	*CommBufferSize
);

/**
    DMIEdit support driver entry point

    @param ImageHandle
    @param SystemTable

    @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
SmbiosDmiEditSupportInstall(
    IN EFI_HANDLE           ImageHandle,
	IN EFI_SYSTEM_TABLE     *SystemTable
)
{
	InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

/**
    Initialize pointers and register SW SMI handlers for
    DMIEdit support.

    @param ImageHandle
    @param SystemTable

    @return EFI_STATUS

**/
EFI_STATUS
InSmmFunction(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS                      Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL   *SwDispatch;
    EFI_SMM_BASE2_PROTOCOL          *SmmBase;
#if REGISTER_SW_SMI_FN50 || REGISTER_SW_SMI_FN51 || REGISTER_SW_SMI_FN52 || REGISTER_SW_SMI_FN53
    EFI_HANDLE                      SwHandle = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT     SwContext;
#endif

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "In DmiEdit InSmmFunction\n"));
#endif
    Status = pBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (void**)&SmmBase);
    if(EFI_ERROR(Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmmBase not found!\n"));
#endif
        return Status;
    }

    SmmBase->GetSmstLocation (SmmBase, &mSmst);   // Save the system table pointer

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
    Status = pBS->LocateProtocol (&gAmiSmbiosFlashDataProtocolGuid, NULL, (void**)&mSmbiosFlashDataProtocol);
    if (EFI_ERROR(Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosFlashDataProtocol not found!\n"));
#endif
        return Status;
    }

    mSmbiosFlashDataProtocol->GetFlashTableInfo(mSmbiosFlashDataProtocol, &gFlashData, &gFlashDataSize);

    Status = mSmst->SmmLocateProtocol (&gEfiSmiFlashProtocolGuid, NULL, (void**)&mSmiFlash);
    if (EFI_ERROR(Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmiFlash not found!\n"));
#endif
        return Status;
    }

    Status = mSmst->SmmLocateProtocol (&gFlashSmmProtocolGuid, NULL, (void**)&mFlash);
    if (EFI_ERROR(Status)) return Status;
#endif

    WriteOnceStatusInit();

    // Register the SW SMI handler
    Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (void**)&SwDispatch);
    if (EFI_ERROR(Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SwDispatch not found!\n"));
#endif
        return Status;
    }

#if REGISTER_SW_SMI_FN50
    SwContext.SwSmiInputValue = 0x50;
    Status = SwDispatch->Register (SwDispatch, (EFI_SMM_HANDLER_ENTRY_POINT2)SmiHandler, &SwContext, &SwHandle);
    if (EFI_ERROR (Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to register SwSmi 0x50\n"));
#endif
        return Status;
    }
#endif

#if REGISTER_SW_SMI_FN51
    SwContext.SwSmiInputValue = 0x51;
    Status = SwDispatch->Register (SwDispatch, (EFI_SMM_HANDLER_ENTRY_POINT2)SmiHandler, &SwContext, &SwHandle);
    if (EFI_ERROR (Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to register SwSmi 0x51\n"));
#endif
        return Status;
    }
#endif

#if REGISTER_SW_SMI_FN52
    SwContext.SwSmiInputValue = 0x52;
    Status = SwDispatch->Register (SwDispatch, (EFI_SMM_HANDLER_ENTRY_POINT2)SmiHandler, &SwContext, &SwHandle);
    if (EFI_ERROR (Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to register SwSmi 0x52\n"));
#endif
        return Status;
    }
#endif

#if REGISTER_SW_SMI_FN53
    SwContext.SwSmiInputValue = 0x53;
    Status = SwDispatch->Register (SwDispatch, (EFI_SMM_HANDLER_ENTRY_POINT2)SmiHandler, &SwContext, &SwHandle);
    if (EFI_ERROR (Status)) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to register SwSmi 0x53\n"));
#endif
        return Status;
    }
#endif

    return EFI_SUCCESS;
}

/**
    Get pointers to Smbios Entry Point
**/
VOID
GetSmbiosPointers (VOID)
{
    UINTN                   Size;

#if SMBIOS_2X_SUPPORT
    Size = sizeof(SMBIOS_TABLE_ENTRY_POINT*);
    pRS->GetVariable(L"SmbiosEntryPointTable",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &SmbiosTableEntryPoint);
#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "SmbiosDataTable at %08x\n", SmbiosTableEntryPoint));
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
    DEBUG((DEBUG_INFO, "SmbiosV3EntryPointTable at %08x\n", SmbiosV3TableEntryPoint));
#endif
#endif                                          // SMBIOS_3X_SUPPORT

    Size = sizeof(UINT8*);
    pRS->GetVariable(L"SmbiosScratchBuffer",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &ScratchBufferPtr);
#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "Scratch Buffer at %08x\n", ScratchBufferPtr));
#endif

    Size = sizeof(UINT16);
    pRS->GetVariable(L"MaximumTableSize",
                            &gAmiSmbiosNvramGuid,
                            NULL,
                            &Size,
                            &MaximumBufferSize);
}

/**
    Handles the SMI

    @param DispatchHandle
    @param DispatchContext

    @return EFI_STATUS

**/
EFI_STATUS
SmiHandler (
    IN  EFI_HANDLE  DispatchHandle,
    IN  CONST VOID	*DispatchContext,
    IN  OUT	VOID	*CommBuffer,
    IN  OUT	UINTN	*CommBufferSize
)
{
    UINT8                   Data;
    VOID                    *pInterface;
    UINT16                  RetStatus;
    UINTN                   Cpu;
    EFI_GUID                SmmCpuProtocolGuid = EFI_SMM_CPU_PROTOCOL_GUID;
    EFI_SMM_CPU_PROTOCOL    *SmmCpuProtocol;
    EFI_SMM_SW_CONTEXT      *SwContext = (EFI_SMM_SW_CONTEXT*)CommBuffer;
    static BOOLEAN          ValidPointers = FALSE;

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "In SmiHandler\n"));
#endif

    if (!ValidPointers) {
#if DMIEDIT_DEBUG_TRACE
        DEBUG((DEBUG_INFO, "Getting Smbios pointers"));
#endif
        GetSmbiosPointers();
        ValidPointers = TRUE;
    }

    Cpu = SwContext->SwSmiCpuIndex;

    mSmst->SmmLocateProtocol(
                        &SmmCpuProtocolGuid,
                        NULL,
                        (void**)&SmmCpuProtocol
                        );

    pInterface = 0;         // Clear upper 64-bits.
    SmmCpuProtocol->ReadSaveState(
                            SmmCpuProtocol,
                            4,
                            EFI_SMM_SAVE_STATE_REGISTER_RBX,
                            Cpu,
                            &pInterface
                            );

#if (SMBIOS_2X_SUPPORT == 0) && (SMBIOS_3X_SUPPORT == 1)
{
    VOID    *pInterfaceHigh;

    SmmCpuProtocol->ReadSaveState(
                            SmmCpuProtocol,
                            4,
                            EFI_SMM_SAVE_STATE_REGISTER_RCX,
                            Cpu,
                            &pInterfaceHigh
                            );

    pInterface = (VOID*)((UINT64)pInterface | Shl64((UINT64)pInterfaceHigh, 32));
}
#endif

    Data = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->CommandPort;

#if DMIEDIT_DEBUG_TRACE
    DEBUG((DEBUG_INFO, "\nDmiEdit FN%x Start\n", Data));
#endif
    RetStatus = DMI_FUNCTION_NOT_SUPPORTED;
    switch(Data) {
#if (SMBIOS_2X_SUPPORT == 1)
        case 0x50:  RetStatus = GetSmbiosV2Info(pInterface);
                    break;
        case 0x51:  RetStatus = GetSmbiosV2Structure(pInterface);
                    break;
        case 0x52:  EnableShadowWrite();
                    RetStatus = SetSmbiosV2Structure(pInterface);
                    DisableShadowWrite();
                    break;
#endif                                          // SMBIOS_2X_SUPPORT
#if (SMBIOS_3X_SUPPORT == 1)
        case 0x53:  switch(*(UINT8*)pInterface) {
                        case 0x58:  RetStatus = GetSmbiosV3Info(pInterface);
                                    break;
                        case 0x59:  RetStatus = GetSmbiosV3Structure(pInterface);
                                    break;
                        case 0x5a:  EnableShadowWrite();
                                    RetStatus = SetSmbiosV3Structure(pInterface);
                                    DisableShadowWrite();
                                    break;
                    }
#endif                                          // SMBIOS_3X_SUPPORT
    }

#if DMIEDIT_DEBUG_TRACE
	DEBUG((DEBUG_INFO, " DmiEdit FN%x End - SMI return status = %04x\n", Data, RetStatus));
#endif
    SmmCpuProtocol->WriteSaveState(
                            SmmCpuProtocol,
                            2,
                            EFI_SMM_SAVE_STATE_REGISTER_RAX,
                            Cpu,
                            &RetStatus
                            );

    return EFI_SUCCESS;
}

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
/**
    Write to the flash part starting at "Address" for a length
    of "Size".

    @param Address
    @param Data
    @param Size

    @return EFI_STATUS

**/
EFI_STATUS
WriteToFlash(
    IN VOID    *Address,
    IN VOID    *Data,
    IN UINTN   Size
)
{
	EFI_STATUS	Status;

	Status = mFlash->DeviceWriteEnable();
	if (EFI_ERROR(Status)) return Status;

	Status = mFlash->Write(Address, Size, Data);

	mFlash->DeviceWriteDisable();

#if DMIEDIT_DEBUG_TRACE
	DEBUG((DEBUG_INFO, "WriteToFlash status = %r\n", Status));
#endif

	return Status;
}

/**
    Searches the Flash Data Table for a record of Type and
    Offset. If found, the existing data will be replaced with
    the new data, else the data will be added as a new record.

    @param TableInfo
    @param Data

    @retval UINT16 Status

**/
UINT16
UpdateSmbiosTable(
    IN TABLE_INFO  *TableInfo,
    IN UINT8       *Data
)
{
    UINT16              	i;
    UINT8                   *BufferPtr = NULL;
    UINT16               	BufferSize = 0;
    UINT16               	Count = 0;
    UINT32              	SpaceAvailable;
    EFI_STATUS          	Status;
    FLASH_DATA_INFO     	FlashDataInfo;
    UINT8               	*FlashDataPtr;
    FUNC_BLOCK          	FuncBlock[2];
    EFI_PHYSICAL_ADDRESS    SmmBuffer;
    EFI_PHYSICAL_ADDRESS    Buffer;

    FlashDataInfo = GetFlashDataInfo(TableInfo);

    // Check Size
    SpaceAvailable = (UINT32)((UINT8*)gFlashData + gFlashDataSize - FlashDataInfo.EndOfData);
    if (FlashDataInfo.Location) SpaceAvailable += FlashDataInfo.Size + sizeof(TABLE_INFO);

    if (sizeof(TABLE_INFO) + TableInfo->Size > SpaceAvailable) {
        return DMI_ADD_STRUCTURE_FAILED;
    }

    // Initialize FuncBlock
    for (i = 0; i < 2; i++) {
        FuncBlock[i].BufAddr = 0;
        FuncBlock[i].BlockAddr = 0;
        FuncBlock[i].BlockSize = 0;
        FuncBlock[i].ErrorCode = 0;
    }

    // Allocate 4K working buffer in SMM.
    Status = mSmst->SmmAllocatePages ( AllocateAnyPages, EfiRuntimeServicesData, 1, &Buffer);
    if (EFI_ERROR(Status)) return DMI_ADD_STRUCTURE_FAILED;
    BufferPtr = (UINT8*)Buffer;

    // Update String;
    *(TABLE_INFO *)BufferPtr = *TableInfo;
    BufferPtr += sizeof(TABLE_INFO);

    for(i = 0; i < TableInfo->Size; ++i) {
        *BufferPtr++ = Data[i];
    }

    if (FlashDataInfo.Location) {
        UINT32     FlashDataOffset;

        // Allocate 64K GetFlashInfo buffer in SMM.
        Status = mSmst->SmmAllocatePages ( AllocateAnyPages, \
                                           EfiRuntimeServicesData, \
                                           16, \
                                           &SmmBuffer);
        if (EFI_ERROR(Status)) {
            // Free buffer and return error.
            mSmst->SmmFreePages (Buffer, 1);
            return DMI_ADD_STRUCTURE_FAILED;
        }

        ((INFO_BLOCK*)SmmBuffer)->Length = 0x10000;

        Status = mSmiFlash->GetFlashInfo((INFO_BLOCK*)SmmBuffer);

        if (Status) {
            // Free buffers and return error.
            mSmst->SmmFreePages (Buffer, 1);
            mSmst->SmmFreePages (SmmBuffer, 16);
            return DMI_ADD_STRUCTURE_FAILED;
        }

        // Initialize FUNC_BLOCK structure for SMIFlash used.
        for (i = 0, Count = 1; i < ((INFO_BLOCK*)SmmBuffer)->TotalBlocks; i++) {
            if (((UINT32)(UINTN)FlashDataInfo.Location - FLASH_DEVICE_BASE) > \
                    (((INFO_BLOCK*)SmmBuffer)->Blocks[i].StartAddress + \
                     ((INFO_BLOCK*)SmmBuffer)->Blocks[i].BlockSize)) continue;
            FuncBlock[0].BlockSize = \
                            ((INFO_BLOCK*)SmmBuffer)->Blocks[i].BlockSize;
            FuncBlock[0].BlockAddr = \
                            ((INFO_BLOCK*)SmmBuffer)->Blocks[i].StartAddress;

            // Check whether SmbiosFlashData exceeds the block boundary.
            if (((UINT32)(UINTN)gFlashData + (UINT32)FLASHDATA_SIZE - FLASH_DEVICE_BASE) > \
                    (((INFO_BLOCK*)SmmBuffer)->Blocks[i+1].StartAddress)) {
                Count = 2;
                FuncBlock[1].BlockSize = \
                            ((INFO_BLOCK*)SmmBuffer)->Blocks[i+1].BlockSize;
                FuncBlock[1].BlockAddr = \
                            ((INFO_BLOCK*)SmmBuffer)->Blocks[i+1].StartAddress;
            }
            break;
        }

        // Free the GetFlashInfo buffer.
        Status = mSmst->SmmFreePages (SmmBuffer, 16);

        // Allocate the blocks buffer.
        Status = mSmst->SmmAllocatePages ( \
                            AllocateAnyPages, \
                            EfiRuntimeServicesData, \
                            (FuncBlock[0].BlockSize * Count) / 0x1000, \
                            &SmmBuffer);
        if (EFI_ERROR(Status)) {
            // Free buffer and return error.
            mSmst->SmmFreePages (Buffer, 1);
            return DMI_ADD_STRUCTURE_FAILED;
        }
        FuncBlock[0].BufAddr = SmmBuffer;
        FuncBlock[1].BufAddr = SmmBuffer + FuncBlock[0].BlockSize;

        // Read the whole SmbiosFlashData Blocks.
        for (i = 0; i < Count; i++) {
            Status = mFlash->Read((VOID*)(UINTN)(FuncBlock[i].BlockAddr + FLASH_DEVICE_BASE), \
                                FuncBlock[i].BlockSize, (VOID*)FuncBlock[i].BufAddr);
            if (Status) {
                // Free buffer and return error.
                mSmst->SmmFreePages (Buffer, 1);

                mSmst->SmmFreePages ( \
                            SmmBuffer, \
                            (FuncBlock[0].BlockSize * Count) / 0x1000);
                return DMI_ADD_STRUCTURE_FAILED;
            }
        }

        // Initialize SmbiosFlashData buffer.
        for (i = 0; i < FLASHDATA_SIZE; i++, *((UINT8*)Buffer + i) = 0xff);

        // Re-collect the Smbios structures to SmbiosFlashData buffer.
        FlashDataPtr = gFlashData;
        BufferPtr = (UINT8*)Buffer;

        while((((TABLE_INFO*)FlashDataPtr)->Size != 0xffff) &&
              (((TABLE_INFO*)FlashDataPtr)->Size != 0)) {
            if ((((TABLE_INFO*)FlashDataPtr)->Type == TableInfo->Type) && \
                (((TABLE_INFO*)FlashDataPtr)->Handle == TableInfo->Handle) && \
                (((TABLE_INFO*)FlashDataPtr)->Offset == TableInfo->Offset)) {
                // Replace the structure with updated data.
                MemCpy(BufferPtr, (UINT8*)TableInfo, sizeof(TABLE_INFO));
                BufferSize = TableInfo->Size;
                MemCpy (BufferPtr + sizeof(TABLE_INFO), Data, BufferSize);
                BufferSize += sizeof(TABLE_INFO);
            } else {
                // Copy the structure.
                BufferSize = (((TABLE_INFO*)FlashDataPtr)->Size + sizeof(TABLE_INFO));
                MemCpy (BufferPtr, FlashDataPtr, BufferSize);
            }

            BufferPtr += BufferSize;
            FlashDataPtr += (((TABLE_INFO*)FlashDataPtr)->Size + sizeof(TABLE_INFO));
        }

        // Copy the new SmbiosFlashData to read buffer.
        FlashDataOffset = ((UINT32)(UINTN)FlashDataInfo.Location - \
                                    FLASH_DEVICE_BASE - FuncBlock[0].BlockAddr);
        BufferPtr = (UINT8*)Buffer + (UINT32)(UINTN)FlashDataInfo.Location - (UINT32)(UINTN)gFlashData;
        MemCpy((UINT8*)(UINTN)(FuncBlock[0].BufAddr + FlashDataOffset),
                       (UINT8*)BufferPtr,
                       (UINT32)(UINTN)gFlashData + (UINT32)FLASHDATA_SIZE - (UINT32)(UINTN)FlashDataInfo.Location);

        // Write the block buffer with updated SmbiosFlashData back.
        if (!EFI_ERROR(Status)) {
            for (i = 0; i < Count; i++) {
                Status = mFlash->Update( \
										(VOID*)(UINTN)(FuncBlock[i].BlockAddr + FLASH_DEVICE_BASE), \
										FuncBlock[i].BlockSize, (VOID*)FuncBlock[i].BufAddr
										);
                if (EFI_ERROR(Status)) break;
            }
		}

        // Free the Block Buffer in SMM.
		mSmst->SmmFreePages ( SmmBuffer, \
							  (FuncBlock[0].BlockSize * Count) / 0x1000);
    }
    else {
        UINT32 EndOfData;

        EndOfData = (UINT32)(UINTN)FlashDataInfo.EndOfData & 0x0ffff;

        if ((EndOfData + (UINT32)(BufferPtr - (UINT8*)Buffer)) > 0x10000) {
            UINT32 NewOffestOfData;
            UINT32 ExtraSize;
            UINT32 DataLength;

            NewOffestOfData = (UINT32)(((UINT32)(UINTN)FlashDataInfo.EndOfData & 0xffff0000) + 0x10000);
            ExtraSize = EndOfData + (UINT32)(BufferPtr - (UINT8*)Buffer) - 0x10000;
            DataLength = (UINT32)(BufferPtr - (UINT8*)Buffer);

            Status = WriteToFlash(FlashDataInfo.EndOfData,
                                (UINT8*)Buffer,
                                DataLength - ExtraSize);

            Status = WriteToFlash( (VOID *)(UINTN)NewOffestOfData,
                                (UINT8*)(Buffer + DataLength - ExtraSize),
                                ExtraSize);
        }
        else {
            Status = WriteToFlash(FlashDataInfo.EndOfData,
                                (UINT8*)Buffer,
                                BufferPtr - (UINT8*)Buffer);
        }

        mSmst->SmmFreePages (Buffer, 1);

        if (Status) return DMI_ADD_STRUCTURE_FAILED;
    }

    return 0;
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
