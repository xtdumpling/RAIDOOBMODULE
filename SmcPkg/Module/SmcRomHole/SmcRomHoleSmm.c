//***************************************************************************
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//***************************************************************************
//
//  File History
//
//  Rev. 1.04
//    Bug Fix:  Trigger a software SMI to flash ROM part after BIOS lock.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/30/2016
//
//  Rev. 1.03
//    Bug Fix:  Separate OA3 and SMC ROMHOLE define.
//    Auditor:  Kasber Chen
//    Date:     Sep/01/2016
//
//  Rev. 1.02
//    Bug Fix:  Change Flash to SmcFlash for redefine issue.
//    Auditor:  Kasber Chen
//    Date:     Aug/18/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix system hangs at Sum InBand command.
//    Reason:   Because of using Boot Service in SMM. Separate SmcRomHole into two libraries - DXE and SMM.
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/12/2016
//
//  Rev. 1.00
//    Bug Fix:  Change SMC ROM Hole map. Add a library to access the SMC ROM Hole.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/23/13
//
//****************************************************************************
#include <Token.h>
#include <SmcRomHole.h>
#include <AmiDxeLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/FlashProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmiFlash.h>

FLASH_PROTOCOL *SmcFlash = NULL;
#define SMC_ROM_HOLE_BLOCK_ADDRESS	(SMC_ROM_HOLE_LOCATION - FLASH_BASE)
#define SMC_ROM_HOLE_BLOCK_END		(SMC_ROM_HOLE_BLOCK_ADDRESS + SMC_ROM_HOLE_SIZE)

EFI_STATUS AllocateBuffer (VOID** Buffer, UINTN BufferSize)
{
    if (pSmst != NULL)
        return pSmst->SmmAllocatePool (EfiRuntimeServicesData, BufferSize, Buffer);
    else
        return EFI_NOT_READY;
}

EFI_STATUS FreeBuffer (UINT8* Buffer)
{
    if (pSmst != NULL)
        return pSmst->SmmFreePool(Buffer);
    else
        return EFI_NOT_READY;
}

EFI_STATUS
SmcRomHoleWriteRegion(
    IN	UINT8	Operation,
    IN	UINT8	*pBuffer
)
{
    EFI_STATUS	Status;
    UINT32	TempSize, Offset;
    UINT8	*TempBuffer;

    DEBUG((-1, "SmcRomHoleSmm.c - SmcRomHoleWriteRegion Start.\n"));
    DEBUG((-1, "Operation = 0x%x\n", Operation));

    if(SmcFlash == NULL)	return EFI_NOT_READY;

    //
    // Please use "AllocateBuffer" to alocate memory and use "FreeBuffer" to free memory.
    // Separate SmcRomHole into two libraries - DXE and SMM.
    // In DXE, "AllocateBuffer" uses "gBS->AllocatePool" to allocate memory.
    // In SMM, "AllocateBuffer" uses "pSmst->SmmAllocatePool" to allocate memory.
    // "FreeBuffer" is the same.
    //
    Status = AllocateBuffer (&TempBuffer, SMC_ROM_HOLE_SIZE);
    if(Status)	return Status;

    Status = SmcFlash->DeviceWriteEnable();
    if(Status){
	FreeBuffer(TempBuffer);
	return Status;
    }

    TempSize = SmcRomHoleSize(Operation);
    Offset = SmcRomHoleOffset(Operation);

    Status = SmcFlash->Read((UINT8*)SMC_ROM_HOLE_LOCATION, SMC_ROM_HOLE_SIZE, TempBuffer);
    if(Status){
	FreeBuffer(TempBuffer);
	return Status;
    }

    Status = SmcFlash->Erase((UINT8*)SMC_ROM_HOLE_LOCATION, SMC_ROM_HOLE_SIZE);
    if(Status){
	FreeBuffer(TempBuffer);
	return Status;
    }

    CopyMem(TempBuffer + Offset, pBuffer, TempSize);

    Status = SmcFlash->Write((UINT8*)SMC_ROM_HOLE_LOCATION, SMC_ROM_HOLE_SIZE, TempBuffer);
    if(Status){
	FreeBuffer(TempBuffer);
	return Status;
    }

    Status = SmcFlash->DeviceWriteDisable();
    FreeBuffer(TempBuffer);
    DEBUG((-1, "SmcRomHoleSmm.c - SmcRomHoleWriteRegion End.\n"));
    return Status;
}

EFI_STATUS
EFIAPI
SmcRomHoleSmmInit()
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_SMM_BASE2_PROTOCOL *gSmmBase2 = NULL;

    DEBUG((-1, "SmcRomHoleSmmInit Start.\n"));

    Status = gBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &gSmmBase2);

    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = gSmmBase2->GetSmstLocation (gSmmBase2, &pSmst);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = gBS->LocateProtocol(&gFlashSmmProtocolGuid, NULL, &SmcFlash);
    DEBUG((-1, "gFlashSmmProtocolGuid Status = %r.\n", Status));
    if (EFI_ERROR(Status)) SmcFlash = NULL;

    DEBUG((-1, "SmcRomHoleSmmInit End.\n"));
    return EFI_SUCCESS;
}

VOID
SmcRomHoleUpdateBlockTypeId(
    IN	UINT8	SwSmiNum,
    IN	UINT64	Buffer
)
{
    BLOCK_DESC	*BlockDesc;
    UINTN	i;

    // return if SW SMI value is not "Get Flash Info"
    if(SwSmiNum != SMIFLASH_GET_FLASH_INFO)
        return;

    BlockDesc = (BLOCK_DESC*)&((INFO_BLOCK*)Buffer)->Blocks;

    for(i = 0; i < ((INFO_BLOCK*)Buffer)->TotalBlocks; i++){

        DEBUG((-1, "SmcRomHoleUpdateBlockTypeId: %08X(%08X), Block %08X\n",SMC_ROM_HOLE_BLOCK_ADDRESS,SMC_ROM_HOLE_BLOCK_END,BlockDesc[i].StartAddress));

        if(BlockDesc[i].StartAddress < SMC_ROM_HOLE_BLOCK_ADDRESS)
            continue;

        if (BlockDesc[i].StartAddress >= SMC_ROM_HOLE_BLOCK_END)
            continue;

        DEBUG((-1, "SmcRomHoleUpdateBlockTypeId: Found Blocks %08X\n",BlockDesc[i].StartAddress));

        BlockDesc[i].Type = OA3_FLASH_BLOCK_DESC_TYPE;
    }
}
