//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file
    Microcode Update SMI handler.
    This file contains code for processing Interrupt 15 function D042h,
     and for registering the callback that does the processing.
 **/

#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/FlashProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/AmiBufferValidationLib.h>
#include <Token.h>
#include "MicrocodeUpdate.h"


EFI_SMM_CPU_PROTOCOL    *SmmCpuProtocol;

#ifndef INT15_D042_SWSMI
#define INT15_D042_SWSMI 0x44
#endif

EFI_GUID gMcodeFfsguid =
    {0x17088572, 0x377F, 0x44ef, 0x8F,0x4E,0xB0,0x9F,0xFF,0x46,0xA0,0x70};

typedef struct {
    UINT64 Address;
    UINT32 Size:24; //Multiple of 16 bytes.
    UINT32 Rsv:8;
    UINT16 Ver;
    UINT8  Type:7;  //Optmization?
    UINT8  C_V:1;
    UINT8  Checksum;
} FIT_ENTRY;

FIT_ENTRY *pFit = NULL;
UINT32    gFitSize;

#if MICROCODE_SPLIT_BB_UPDATE
UINT8 *gMcodeFlashStartFixed = 0;
UINT32 gMcodeFlashSizeFixed = 0;
#endif

UINT8 *gMcodeFlashStartUpdate = 0;
UINT32 gMcodeFlashSizeUpdate = 0; 

typedef enum {
    MCODE_BLK_EMPTY = 0,
    MCODE_BLK_START,
    MCODE_BLK_CONT
} MCODE_BLK_TYPE;

typedef struct {
    UINT8           *Addr; 
    UINT32          Size;   //0 if less than 64k
    MCODE_BLK_TYPE  Type;    
} MCODE_BLK_MAP;

UINT16 gNumMcodeBlks = 0;
UINT16 gFirstEmptyBlk = 0xffff;
MCODE_BLK_MAP *gMcodeBlkMap = NULL;


UINT32 gCpuSignature;
UINT8  gCpuFlag;
UINT32 gUcRevision;

UINT8 *gFlashWriteBuffer;

#define MAX_MICROCODE_UPDATE_FUNCTIONS 4
VOID(*MicrocodeUpdate[4])(SMI_UC_DWORD_REGS *) = {
    PresenceTest, WriteUpdateData, UpdateControl, ReadUpdateData
};

#define MICROCODE_SIZE(Hdr) \
    (((MICROCODE_HEADER*)(Hdr))->TotalSize ? ((MICROCODE_HEADER*)Hdr)->TotalSize : 2048)

#if PACK_MICROCODE
#define MICROCODE_ALIGN_SIZE(Hdr) \
    ((MICROCODE_SIZE(Hdr) + 16 - 1) & ~(16 - 1))
#else
#define MICROCODE_ALIGN_SIZE(Hdr) \
    ((MICROCODE_SIZE(Hdr) + MICROCODE_BLOCK_SIZE - 1) & ~(MICROCODE_BLOCK_SIZE - 1))
#endif

#if PACK_MICROCODE
#define MICROCODE_BLOCKS(Hdr) \
    ((((MICROCODE_SIZE(Hdr) + MICROCODE_BLOCK_SIZE - 1) & ~(MICROCODE_BLOCK_SIZE - 1)))/MICROCODE_BLOCK_SIZE)
#else
    #define MICROCODE_BLOCKS(Hdr) (MICROCODE_ALIGN_SIZE(Hdr)/MICROCODE_BLOCK_SIZE)
#endif

/**
Check address range to avoid TSEG area.

@param Address      Starting Address.
@param Range        Length of Area.

@retval EFI_SUCCESS     		Access granted.
@retval EFI_INVALID_PARAMETER   Access denied.
@retval EFI_ACCESS_DENIED	    Access denied.
**/

EFI_STATUS CheckAddressRange(IN UINT8 *Address, IN UINT32 Range)
{
    return AmiValidateMemoryBuffer (Address, (UINTN)Range);
}


/**
Check if the header is valid.

@param MICROCODE_HEADER     Address of Microcode Header.
@retval TRUE                Microcode header is valid.
**/

BOOLEAN IsValidHeader(MICROCODE_HEADER *uHeader)
{
    if (uHeader->HeaderVersion != UC_HEADER_VERSION) return FALSE;
    if (uHeader->LoaderRevison != UC_LOADER_REVISION) return FALSE;
    return TRUE;
}

/**
Validate the checksum.

@param Mcode     Address of Microcode Header.
@param Size      Microcode Size.

@retval TRUE     Checksum valid.
**/

BOOLEAN IsValidChecksum(VOID *Mcode, UINT32 Size)
{
    UINT32  NumDwords = Size >> 2;
    UINT32 *p32 = (UINT32*)Mcode;
    UINT32 Checksum = 0;   
    UINT32 i;

    for(i = 0; i < NumDwords; ++i) Checksum += p32[i];      //Checksum is the summation dwords.

    return Checksum == 0 ? TRUE : FALSE;
}

/**
 Get the installed microcode revision on the CPU.

 @retval Revision of microcode currently installed on CPU.
**/

UINT32 GetInstalledMicrocodeRevision()
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;

    //Clear IA32_BIOS_SIGN_ID of microcode loaded.
    AsmWriteMsr64(0x8b, 0); //IA32_BIOS_SIGN_ID

    //Reading CPU ID 1, updates the MSR to the microcode revision.
    AsmCpuid(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    return (UINT32) RShiftU64(AsmReadMsr64(0x8b), 32);
}


/**
Count blocks taken by microcode in FFS.

@param McodeStart   Start of microcode in FFS.
@param McodeSize    Size of microcode and empty space in FFS.
@param CountEmpty   TRUE if calculate blocks for empty space.

@retval TRUE        Number of blocks needed.
**/

UINT16 CountBlks(IN UINT8 *McodeStart, IN UINT32 McodeSize, IN BOOLEAN CountEmpty)
{
    UINT8 *p = McodeStart;
    UINT8 *EndOfMcode = p + McodeSize;
    UINT16 BlkIndex = 0;
    UINT16 TotBlks = 0;

    while(p < EndOfMcode) {
        if (*(UINT32*)p != 0xffffffff && *(UINT32*)p != 0) {            
            TotBlks += MICROCODE_BLOCKS(p);
            p += MICROCODE_ALIGN_SIZE(p);
        } else if (CountEmpty) {
            TotBlks += (UINT16)((EndOfMcode - p) / MICROCODE_BLOCK_SIZE);
            break;
        }
        else break;
    }

    return TotBlks;
}

/**
Update gMcodeBlkMap with data for microcode.

@param BlkStart     On Input: Start update with this block. Output: Next call use this value.
@param McodeStart   Start of microcode in FFS.
@param McodeSize    Size of microcode and empty space in FFS.
@param CountEmpty   TRUE if calculate blocks for empty space.
**/

VOID FillMicrocodeBlkMap(IN OUT UINT16 *BlkStart, IN UINT8 *McodeStart, IN UINT32 McodeSize, IN BOOLEAN CountEmpty)
{
    UINT8 *p = McodeStart;
    UINT8 *EndOfMcode = p + McodeSize;
    UINT16 BlkIndex = *BlkStart;

    while(p < EndOfMcode) {
        if (*(UINT32*)p != 0xffffffff && *(UINT32*)p != 0) {            
            UINT16 NumBlks = MICROCODE_BLOCKS(p);
            UINT32 McodeSize = MICROCODE_SIZE(p);
#if PACK_MICROCODE == 0
            UINT32 PackDiff = MICROCODE_ALIGN_SIZE(p) - McodeSize;
#endif

            gMcodeBlkMap[BlkIndex].Addr = p;
            gMcodeBlkMap[BlkIndex].Size =  MICROCODE_BLOCK_SIZE;
            gMcodeBlkMap[BlkIndex].Type = MCODE_BLK_START;
            if (NumBlks <= 1 && McodeSize < MICROCODE_BLOCK_SIZE) {
                gMcodeBlkMap[BlkIndex].Size =  McodeSize;
            }
            p += gMcodeBlkMap[BlkIndex].Size;
            McodeSize -= gMcodeBlkMap[BlkIndex].Size;
            ++BlkIndex;
            while(--NumBlks) {
                gMcodeBlkMap[BlkIndex].Addr = p;
                gMcodeBlkMap[BlkIndex].Size = MICROCODE_BLOCK_SIZE;
                gMcodeBlkMap[BlkIndex].Type = MCODE_BLK_CONT;
                if (NumBlks == 1 && McodeSize < MICROCODE_BLOCK_SIZE) {
                    gMcodeBlkMap[BlkIndex].Size =  McodeSize;
                }
                p += gMcodeBlkMap[BlkIndex].Size;
                McodeSize -= gMcodeBlkMap[BlkIndex].Size;
                ++BlkIndex;
            }
#if PACK_MICROCODE == 0
            p += PackDiff;
#endif
        } else if (CountEmpty) {
            gFirstEmptyBlk = BlkIndex;
            while (BlkIndex < gNumMcodeBlks) {
                gMcodeBlkMap[BlkIndex].Addr = p;
                gMcodeBlkMap[BlkIndex].Size = MICROCODE_BLOCK_SIZE;
                gMcodeBlkMap[BlkIndex].Type = MCODE_BLK_EMPTY;
                BlkIndex++;
            }
            break;
        } else break;
    }
    *BlkStart = BlkIndex;
}

/**
Initialize gMcodeBlkMap and related globals for all microcode FFS.
**/

VOID InitMcodeBlkMap()
{
    UINT16 BlkStart = 0;

    if (gMcodeBlkMap != NULL) FreePool(gMcodeBlkMap);
    gNumMcodeBlks = 0;
#if MICROCODE_SPLIT_BB_UPDATE
    gNumMcodeBlks += CountBlks(gMcodeFlashStartFixed, gMcodeFlashSizeFixed, FALSE);
#endif
    gNumMcodeBlks += CountBlks(gMcodeFlashStartUpdate, gMcodeFlashSizeUpdate, TRUE);

    gMcodeBlkMap = AllocatePool(gNumMcodeBlks * sizeof(MCODE_BLK_MAP));
    ASSERT(gMcodeBlkMap != NULL);

    gFirstEmptyBlk = 0xffff;
#if MICROCODE_SPLIT_BB_UPDATE
    FillMicrocodeBlkMap(&BlkStart, gMcodeFlashStartFixed, gMcodeFlashSizeFixed, FALSE);
#endif
    FillMicrocodeBlkMap(&BlkStart, gMcodeFlashStartUpdate, gMcodeFlashSizeUpdate, TRUE);
}

/**
Find Microcode FFS in FV.

@param FvHdr   Firmware volume to search.

@retval Pointer to FFS.
**/

UINT8 *FindMicrocodeFfs(IN EFI_FIRMWARE_VOLUME_HEADER *FvHdr)
{
    UINT8 *FvPtr = (UINT8*)FvHdr + FvHdr->HeaderLength;
    UINT8 *EndOfFv = (UINT8*)FvHdr + FvHdr->FvLength;

    //Search the FV_MAIN firmware volume for the microcode file.
    while (FvPtr < EndOfFv && *FvPtr != -1) {
        if (CompareGuid(&gMcodeFfsguid, &((EFI_FFS_FILE_HEADER*)FvPtr)->Name))
            return FvPtr;

        FvPtr += *(UINT32*)&((EFI_FFS_FILE_HEADER*)FvPtr)->Size & 0xffffff;
        FvPtr = (UINT8*)(((UINTN)FvPtr + 7) & ~7);   //8 byte alignment
    }
    return NULL;
}

/**
Initialize global variables used by the driver.

@retval TRUE  Initialized.
@retval FALSE Not initialized.
**/

BOOLEAN InitMicrocodeVariables()
{
    UINT8 *FfsPtr;
    UINT32 McodeFfsSize;
    UINT64 MsrValue;
    UINT32 RegEbx, RegEcx, RegEdx;
#if MICROCODE_SPLIT_BB_UPDATE
    UINT16 MPDTLengthFixed;
#endif
    UINT16 MPDTLengthUpdate;

#if MICROCODE_SPLIT_BB_UPDATE
    FfsPtr = FindMicrocodeFfs((EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)FV_MICROCODE_BASE);
    if (FfsPtr == NULL) return FALSE;

    gMcodeFlashStartFixed = FfsPtr + sizeof(EFI_FFS_FILE_HEADER);
    McodeFfsSize = ((*(UINT32*)((EFI_FFS_FILE_HEADER*)FfsPtr)->Size) & 0xffffff);
#if MPDTable_CREATED
    MPDTLengthFixed = *(UINT16*)(FfsPtr + McodeFfsSize - 2); //Last 2 bytes is table size.
#else 
    MPDTLengthFixed = 0;
#endif
    gMcodeFlashSizeFixed = McodeFfsSize - sizeof(EFI_FFS_FILE_HEADER) - MPDTLengthFixed;
#endif

#if MICROCODE_SPLIT_BB_UPDATE
    FfsPtr = FindMicrocodeFfs((EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)FV_MICROCODE_UPDATE_BASE);
#else
    FfsPtr = FindMicrocodeFfs((EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)FV_MICROCODE_BASE);
#endif
    if (FfsPtr == NULL) return FALSE;

    gMcodeFlashStartUpdate = FfsPtr + sizeof(EFI_FFS_FILE_HEADER);
    McodeFfsSize = ((*(UINT32*)((EFI_FFS_FILE_HEADER*)FfsPtr)->Size) & 0xffffff);
#if MPDTable_CREATED
    MPDTLengthUpdate = *(UINT16*)(FfsPtr + McodeFfsSize - 2); //Last 2 bytes is table size.
#else
    MPDTLengthUpdate = 0;
#endif
    gMcodeFlashSizeUpdate = McodeFfsSize - sizeof(EFI_FFS_FILE_HEADER) - MPDTLengthUpdate;

    InitMcodeBlkMap();

    //Clear revision value. CPUID of 1 will update this revision value.
    AsmWriteMsr64(0x8b, 0); //IA32_BIOS_SIGN_ID

    AsmCpuid(1, &gCpuSignature, &RegEbx, &RegEcx, &RegEdx);
    gCpuSignature &= 0x00ffffff;
    MsrValue = AsmReadMsr64(0x17);
    gCpuFlag = (UINT8)(RShiftU64(MsrValue, 50) & 7);            //Get the CPU flags.
    gUcRevision = (UINT32) RShiftU64(AsmReadMsr64(0x8b), 32);        //Get the current microcode revision loaded.

    pFit = *(FIT_ENTRY**)0xffffffc0;
    if ((UINT32)pFit == 0xffffffff || (UINT32)pFit == 0xeeeeeeee || *(UINT32*)pFit != 'TIF_') pFit = NULL;
    else gFitSize = pFit->Size * 16;

    return TRUE;
}

/**
Search the microcode in the firmware for the CPU signature or earlier stepping.

@param Mcode            Address of Microcode Header.
@param End              End of microcode updates. 
@param CpuSignature     Signature of CPU to find.

@retval Pointer to Microcode if found, otherwise 0.
**/

VOID *FindMicrocodeOfStepping(UINT8 *Mcode, UINT8 *End, UINT32 CpuSignature)
{
    UINT8 *ptr;
     for(ptr = Mcode; ptr < End; ptr += MICROCODE_ALIGN_SIZE(ptr)) {
        MICROCODE_HEADER* uC = (MICROCODE_HEADER*)ptr;

        if (*(UINT32*)ptr == 0xffffffff || *(UINT32*)ptr == 0) return 0;
        if (uC->CpuSignature == CpuSignature) return ptr;

        if (uC->TotalSize > (uC->DataSize + 48)) {        //Extended signature count.
            MICROCODE_EXT_PROC_SIG_TABLE *SigTable = (MICROCODE_EXT_PROC_SIG_TABLE*)(ptr + uC->DataSize + 48);
            UINT32 ExtSigCount = SigTable->Count;
            UINT8 i;

            for (i = 0; i < ExtSigCount; ++i) {
                if (SigTable->ProcSig[i].CpuSignature == CpuSignature) return ptr;
            }
        } 
    }
    return 0;
}

/**
Load the microcode onto the CPU.

@param Mcode            Address of Microcode Header.
**/

VOID LoadMicrocode(IN VOID *Mcode)
{
    AsmWriteMsr64(0x79, (UINT64)(UINTN)Mcode + 48); //IA32_BIOS_UPDT_TRIG
}

/**
Load the microcode on each CPU.

@param Mcode            Address of Microcode Header.
**/

VOID LoadMicrocodeEachCpu(IN VOID *Mcode)
{
    UINT8 i;
    //In for loop, BSP CPU will return error and continue for all APs.
    for (i = 0; i < gSmst->NumberOfCpus; ++i) {
        gSmst->SmmStartupThisAp(LoadMicrocode, i, Mcode);
    }
    LoadMicrocode(Mcode);
}

/**
Execute the presence test function for int 15h.

@param Regs Structure storing register values.
**/

VOID PresenceTest(SMI_UC_DWORD_REGS *Regs)
{

    //      Input:
    //          AX - D042h
    //          BL - 00h i.e., PRESCENCE_TEST
    //
    //      Output:
    //          CF  NC - All return values are valid
    //          CY - Failure, AH contains status.
    //
    //          AH  Return code
    //          AL  Additional OEM information
    //          EBX Part one of the signature 'INTE'.
    //          ECX Part two of the signature 'LPEP'.
    //          EDX Version number of the BIOS update loader
    //          SI  Number of update blocks system can record in NVRAM (1 based).

    
    Regs->EBX = 'INTE';     //Part 1 of the Signature
    Regs->ECX = 'LPEP';     //Part 2 of the Signature.
    Regs->EDX = UC_LOADER_VERSION;
    
    *(UINT16*)&Regs->ESI = gNumMcodeBlks;    //Number of blocks.
}

/**
Execute the update control for int 15h.

@param Regs Structure storing register values.
**/

VOID UpdateControl(SMI_UC_DWORD_REGS *Regs)
{
    //  Input:
    //      AX - D042h
    //      BL - 02h i.e., UPDATE_CONTROL
    //      BH - Task
    //      1 - Enable the update loading at initialization time.
    //      2 - Determine the current state of the update control without changing its status.
    // 
    //  Output:
    //      AH  Return code
    //      AL  Additional OEM information
    //      BL  Update status Disable or Enable.

    Regs->EBX = (Regs->EBX & 0xffffff00) | UC_INT15_ENABLE;    // Always enabled.
}


/**
Read microcode using int15h.

@param Regs Structure storing register values.
**/

VOID ReadUpdateData(SMI_UC_DWORD_REGS *Regs)
{

    //  Input:
    //      AX  - D042h
    //      BL  - 03h i.e., READ_UPDATE_DATA
    //      ES:DI - Real Mode Pointer to the Intel Update structure.
    //      SS:SP - Stack pointer (32K minimum)
    //      SI    - Update number, the index number of the update block to be read.
    //              This number is zero based and must be less than the update
    //              count returned from the prescence test function.
    //
    // Output:
    //      AH  Return code
    //      AL  Additional OEM information
    //      BL  Update status Disable or Enable.
    
    EFI_STATUS Status;
    UINT16 Index = (UINT16)Regs->ESI;
    UINT8   *UpdateBuffer;
    MICROCODE_HEADER *Header;

    if (Index >= gNumMcodeBlks) {
      Regs->EFLAGS |= CARRY_FLAG;
      *(UINT16*)&Regs->EAX = UC_UPDATE_NUM_INVALID;
      return;
    }

    if (gMcodeBlkMap[Index].Type == MCODE_BLK_CONT) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_NOT_EMPTY;
        return;
    }        

    UpdateBuffer = (UINT8*)(UINTN)(((UINT16)Regs->ES << 4) + (UINT16)Regs->EDI);
	
	Status = CheckAddressRange (UpdateBuffer, MICROCODE_BLOCK_SIZE);
    if(EFI_ERROR(Status)) return;

    if (gMcodeBlkMap[Index].Type == MCODE_BLK_EMPTY) {
        SetMem(UpdateBuffer, MICROCODE_BLOCK_SIZE, 0xff);
        return;
    }        

    Header = (MICROCODE_HEADER *)gMcodeBlkMap[Index].Addr;

    CopyMem(UpdateBuffer, Header, MICROCODE_SIZE(Header));
}


///////////////////////////////////////////////////////////////////////////////////

static UINT8 *gFlashBlk;          //Pointer to current flash block to write.
static UINT8 *gFlashBuffer;         //Pointer to beginning of buffer.
static UINT8 *gFlashBufferPos;      //Pointer to current posisiton.
static UINT32 gFlashBufferUsed;     //Number of bytes used in the buffer.


/**
Helper function to write the buffer to the flash and reset the buffer.
**/

VOID FlushBufferToFlash()
{
    FwhErase((UINTN)gFlashBlk, FLASH_BLOCK_SIZE);
    FwhWrite(gFlashBuffer, (UINTN)gFlashBlk, FLASH_BLOCK_SIZE);
    gFlashBlk += FLASH_BLOCK_SIZE;
    gFlashBufferPos = gFlashBuffer;
    gFlashBufferUsed = 0;

}

/**
Initialize the flash buffer before using buffer.

@param FirstFlashBlk            Address of first block to flash.
@param FlashBuffer              Flash Buffer.
**/

VOID InitializeFlashBuffer(IN UINT8* FirstFlashBlk, IN UINT8 *FlashBuffer)
{
    gFlashBlk = FirstFlashBlk;
    gFlashBuffer = FlashBuffer;
    gFlashBufferPos = gFlashBuffer;
    gFlashBufferUsed = 0;
}

/**
Copy data to the buffer. When the buffer is full, write to the flash, and continues to copy data.
 
@param Data     Start of data to write.
@param Size     Amount to Write.
**/

VOID CopyToFlashBuffer(IN UINT8 *Data, IN UINT32 Size)
{
    while (Size) {
        if (gFlashBufferUsed + Size <= FLASH_BLOCK_SIZE) {
            CopyMem(gFlashBufferPos, Data, Size);

            gFlashBufferPos += Size;
            gFlashBufferUsed += Size;

            if (gFlashBufferUsed == FLASH_BLOCK_SIZE) FlushBufferToFlash();
            return;
        }
		
		CopyMem(gFlashBufferPos, Data, FLASH_BLOCK_SIZE - gFlashBufferUsed);
        Data += FLASH_BLOCK_SIZE - gFlashBufferUsed;
		Size -= FLASH_BLOCK_SIZE - gFlashBufferUsed;
        FlushBufferToFlash();
    }
}

/**
Fill part of the buffer with a value. When the buffer is full, write to the flash, and continue to update the beginning of the buffer with a value. 

@param Value    Start of data to write.
@param Size     Amount to repeat write.
**/

VOID WriteValueToFlashBuffer(IN UINT8 Value, IN UINT32 Size)
{
    while (Size) {
        if (gFlashBufferUsed + Size <= FLASH_BLOCK_SIZE) {
            SetMem(gFlashBufferPos, Size, Value);

            gFlashBufferPos += Size;
            gFlashBufferUsed += Size;

            if (gFlashBufferUsed == FLASH_BLOCK_SIZE) FlushBufferToFlash();
            return;
        }
		
        SetMem(gFlashBufferPos, FLASH_BLOCK_SIZE - gFlashBufferUsed, Value);
		Size -= FLASH_BLOCK_SIZE - gFlashBufferUsed;
        FlushBufferToFlash();
    }
}

/**
Fill the rest of the buffer of a size of the flash block, then update the flash.

@param Data     Pointer to starting of data to write to the flash.
**/

VOID FillFlashBufferAndFlush(IN UINT8 *Data)
{
    if (gFlashBufferUsed != 0) {
        CopyMem(gFlashBufferPos, Data, FLASH_BLOCK_SIZE - gFlashBufferUsed);
        FwhErase((UINTN)gFlashBlk, FLASH_BLOCK_SIZE);
        FwhWrite(gFlashBuffer, (UINTN)gFlashBlk, FLASH_BLOCK_SIZE);
    }
}

/**
Find the empty entry in FIT for microcode update.

@retval Pointer to FIT entry.
**/

FIT_ENTRY * FindFirstMcodeEmptyFit() 
{
    FIT_ENTRY *Entry = pFit; //pFit points type 0.
    UINT8     *FitEnd = (UINT8*)pFit + gFitSize;

    ++Entry;

    //Must be type 1 (microcode) or unused entry (0x7f).
    if ((UINT8*)Entry >= FitEnd || (Entry->Type != 1 && Entry->Type != 0x7f)) return NULL;

    //Find end of microcode entries.
    while ((UINT8*)Entry < FitEnd && Entry->Type == 1 && Entry->Address != (UINT64)-1) ++Entry;
    
    //No entries
    if ((UINT8*)Entry >= FitEnd) return NULL;

    //Return if empty type 1.
    if (Entry->Type == 1 && Entry->Address == (UINT64)-1) return Entry;

    //Must be unused entry (0x7f).
    if (Entry->Type != 0x7f) return NULL;

    return Entry;
}

/**
Write Microcode FIT Entry to Flash.

@param FlashEntry   Flash entry address.
@param MCodeAddr    Microcode address.
**/

VOID AddMCodeFitEntryToFlash(IN FIT_ENTRY *FlashEntry, IN VOID *MCodeAddr)
{
    if (FlashEntry->Type == 0x7f) {
        FIT_ENTRY Entry;
        Entry.Address = (UINT64)(UINTN)MCodeAddr;
        Entry.Size = 0;
        Entry.Rsv = 0;
        Entry.Ver = 0x100;
        Entry.Type = 1;
        Entry.C_V = 0;
        Entry.Checksum = (UINT8)-1;
        FwhWrite((UINT8*)&Entry, (UINTN)FlashEntry, sizeof(FIT_ENTRY));
    } else if (FlashEntry->Type == 1 && FlashEntry->Address == (UINT64)-1) {
        FwhWrite((UINT8*)&MCodeAddr, (UINTN)&FlashEntry->Address, 8);
    }
}

/**
ReWrite FIT to new Microcode block map.
**/

VOID ReWriteFit()
{
    UINT8 *FitCopy;
    FIT_ENTRY *Entry;
    UINT8     *FitCopyEnd;
    UINT8     *FlashBlkStart;
    UINT16    BlkIndex;
    
    FitCopy = AllocatePool(gFitSize);
    FitCopyEnd = FitCopy + gFitSize;

    CopyMem(FitCopy, pFit, gFitSize); //Copy FIT into memory.

    Entry = (FIT_ENTRY*)FitCopy;
    ++Entry;

    //Update FIT microcode after microcode update. Exit if any inconsistencies.
    for (BlkIndex = 0; BlkIndex < gNumMcodeBlks && BlkIndex < gFirstEmptyBlk; ++BlkIndex) {
        if (gMcodeBlkMap[BlkIndex].Type == MCODE_BLK_CONT) continue;
        if (gMcodeBlkMap[BlkIndex].Type == MCODE_BLK_EMPTY) return; //Incorrect mapping if empty blocks in the middle.
        if (Entry->Type != 1) return; //Inconsisent FIT table and microcode table mappings.
        Entry->Address = (UINT64)(UINTN)gMcodeBlkMap[BlkIndex].Addr;
        ++Entry;
    }

    if (CompareMem(pFit, FitCopy, gFitSize) != 0) { //Check to see of the FIT was updated.
        //Write new Fit to flash.
        //1. Initialize buffer.
        //2. Write up to old FIT.
        //3. Write new fit.
        //4. Write rest of data.
    
        FlashBlkStart = (UINT8*)((UINTN)pFit & ~(FLASH_BLOCK_SIZE - 1)); //Must start at block boundary.
        InitializeFlashBuffer(FlashBlkStart, gFlashWriteBuffer);
        //Note: When Buffer is full, it will flush to flash.
        CopyToFlashBuffer(FlashBlkStart, (UINT32)((UINT8*)pFit - FlashBlkStart)); //Copy before old pFit.
        CopyToFlashBuffer(FitCopy, gFitSize); //Copy new FIT.
    	FillFlashBufferAndFlush((UINT8*)pFit + gFitSize); //Copy the rest of the data to flash.
    }
    FreePool(FitCopy);
}

/**
Write microcode to flash and load microcode into each CPU.

@param Regs Structure storing register values.
**/

VOID WriteUpdateData(SMI_UC_DWORD_REGS *Regs)
{
    //  Input:
    //      AX    - D042h
    //      BL    - 01h i.e., WRITE_UPDATE_DATA
    //      ES:DI - Real Mode Pointer to the Intel Update structure.
    //      CX    - Scratch Pad1 (Real Mode Scratch segment 64K in length)
    //      DX    - Scratch Pad2 (Real Mode Scratch segment 64K in length)
    //      SI    - Scratch Pad3 (Real Mode Scratch segment 64K in length)
    //      SS:SP - Stack pointer (32K minimum)
    //
    //  Output:
    //      CF  NC - All return values are valid
    //          CY - Failure, AH contains status.
    //
    //      AH  Return code
    //      AL  Additional OEM information
    //
    
    EFI_STATUS Status;
    FIT_ENTRY *Entry;
    UINT8 *NewMcode = (UINT8*)(UINTN)((Regs->ES << 4) + (Regs->EDI & 0xFFFF));
    MICROCODE_HEADER *Header = (MICROCODE_HEADER*)NewMcode;
    INT32  NewMcodeAlignSize = MICROCODE_ALIGN_SIZE(NewMcode);
    INT32  NewMcodeSize = MICROCODE_SIZE(NewMcode);
    BOOLEAN Compact = FALSE;

    UINT8  *OldMcode;
    INT32  OldMcodeAlignSize;
    UINT8  *BlkStart;
    UINT8  *McodeUpdateStart;
	
    Status = CheckAddressRange(NewMcode, NewMcodeAlignSize);
    if(EFI_ERROR(Status)) return;
    
    if (!IsValidHeader(Header)) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_INVALID_HEADER;
        return;
    }

    if (!IsValidChecksum(NewMcode, NewMcodeSize)) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_INVALID_HEADER_CS;
        return;
    }

    //Only update if microcode is for the installed CPU.
    if (Header->CpuSignature != gCpuSignature || !(Header->Flags & (1<<gCpuFlag))) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_CPU_NOT_PRESENT;
        return;
    }
    
    //Only update a different revision.
    if (Header->UpdateRevision == gUcRevision) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_INVALID_REVISION;
        return;
    }
    
    //Check to make sure it is not an older version. This check only applies to non debug versions.
    if ((INT32)gUcRevision > 0 && (INT32)Header->UpdateRevision < (INT32)gUcRevision) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_INVALID_REVISION;
        return;
    }
    
    //If no microcode installed, then the revision is 0.
    if (gUcRevision != 0) { //Quick check.
        if(FindMicrocodeOfStepping(gMcodeFlashStartUpdate,
             gMcodeFlashStartUpdate + gMcodeFlashSizeUpdate,
             gCpuSignature
           ) != NULL) {
            Compact = TRUE;     //Remove old version.
        }
    }
    if (gFirstEmptyBlk == 0xffff || NewMcodeAlignSize > 
        (gMcodeFlashStartUpdate + gMcodeFlashSizeUpdate - gMcodeBlkMap[gFirstEmptyBlk].Addr)
    ) Compact = TRUE; //Volume is full.

    if (!Compact) {
        //Append blocks.
        UINT8  *pEmptyBlk = gMcodeBlkMap[gFirstEmptyBlk].Addr;

        LoadMicrocodeEachCpu(NewMcode);                //Install new microcode.

        //Check to see if new microcode is installed.
        if (Header->UpdateRevision != GetInstalledMicrocodeRevision()) {
            Regs->EFLAGS |= CARRY_FLAG;
            *(UINT16*)&Regs->EAX = UC_SECURITY_FAILURE;
            return;
        }

        FwhWrite(NewMcode, (UINTN)pEmptyBlk, NewMcodeSize);    //Currently ignoring status
        
        InitMcodeBlkMap();

        if (pFit != NULL) {
            Entry = FindFirstMcodeEmptyFit();
            if (Entry) {
                AddMCodeFitEntryToFlash(Entry, pEmptyBlk);
            }
        }

        return;
    }

    //***Compact Flash Part***

    //Currently restrict FLASH_BLOCK_SIZE to 64k or smaller
    if (FLASH_BLOCK_SIZE > 64 * 1024) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_NOT_IMPLEMENTED;
    }

    //Set up Buffer.
    gFlashWriteBuffer = (UINT8*)(UINTN)((UINT16)Regs->ECX << 4);
	
	Status = CheckAddressRange(gFlashWriteBuffer, FLASH_BLOCK_SIZE);
    if(EFI_ERROR(Status)) return;

    //Find existing microcode of same revision.
    McodeUpdateStart = gFirstEmptyBlk == 0xffff ? 
        gMcodeFlashStartUpdate + gMcodeFlashSizeUpdate : gMcodeBlkMap[gFirstEmptyBlk].Addr;

    OldMcode = FindMicrocodeOfStepping(
        gMcodeFlashStartUpdate,
        McodeUpdateStart,
        gCpuSignature
    );

    //Old Microcode not available to remove?
	if (!OldMcode) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_STORAGE_FULL;
        return;
    }
    OldMcodeAlignSize = MICROCODE_ALIGN_SIZE(OldMcode);

    //Check to see if space big enough for new microcode.
    if ((gMcodeFlashStartUpdate + gMcodeFlashSizeUpdate - McodeUpdateStart + OldMcodeAlignSize) <  NewMcodeAlignSize) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_STORAGE_FULL;
        return;
    }

    //Load new microcode, if can't load exit.
    LoadMicrocodeEachCpu(NewMcode);    //Install new microcode.
    if (Header->UpdateRevision != GetInstalledMicrocodeRevision()) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_SECURITY_FAILURE;
        return;
    }
    
    //Start compacting at block with Old Microcode to remove.
    BlkStart = (UINT8*)((UINTN)OldMcode & ~(FLASH_BLOCK_SIZE - 1)); //Must start at block boundary.

    //Note: When Buffer is full, it will flush to flash.
    InitializeFlashBuffer(BlkStart, gFlashWriteBuffer);
    CopyToFlashBuffer(BlkStart, (UINT32)(OldMcode - BlkStart)); //Copy before old microcode.

	CopyToFlashBuffer(OldMcode + OldMcodeAlignSize, (UINT32)(McodeUpdateStart - (OldMcode + OldMcodeAlignSize)));  //Copy after old microcode.

	CopyToFlashBuffer(NewMcode, NewMcodeSize);     //Copy new microcode.

	WriteValueToFlashBuffer(0xff, NewMcodeAlignSize - NewMcodeSize);	//Fill block space after microcode.

	if (NewMcodeAlignSize < OldMcodeAlignSize) {
        WriteValueToFlashBuffer(0xff, OldMcodeAlignSize - NewMcodeAlignSize);   //Write 0xff over reclaimed space. Polarity?
	} else {
		McodeUpdateStart += NewMcodeAlignSize - OldMcodeAlignSize;
	}
	FillFlashBufferAndFlush(McodeUpdateStart);
        
    InitMcodeBlkMap();
    if (pFit != NULL) ReWriteFit();
}

/**
This function processes Interrupt 15h, function D042h to update microcode in flash.

@param DispatchHandle   EFI Handle
@param Context          Unused
@param CommBuffer       Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
@param CommBufferSize

@retval EFI_SUCCESS     Always returned.
**/

EFI_STATUS MicrocodeUpdateCallback(
	IN EFI_HANDLE DispatchHandle,
	IN CONST VOID *Context OPTIONAL,
	IN OUT VOID *CommBuffer OPTIONAL,
	IN OUT UINTN *CommBufferSize OPTIONAL
)
{
    SMI_UC_DWORD_REGS   *Regs = NULL;
    UINTN   Cpu;
    EFI_STATUS Status;

    UINT8   Function;
    static  BOOLEAN IsInitError = FALSE;
    static  BOOLEAN IsInit = FALSE;
        
    if (!IsInit) {
        IsInit = TRUE;
        IsInitError = !InitMicrocodeVariables();
    }

   Cpu =( (EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;

    Status = SmmCpuProtocol->ReadSaveState(SmmCpuProtocol,
        sizeof(UINT32),
        EFI_SMM_SAVE_STATE_REGISTER_RSI,
        Cpu,
        (VOID*)&Regs
    );
    ASSERT_EFI_ERROR(Status);    

    if ((UINT16)Regs->EAX != 0xD042) goto Exit;

    Function = (UINT8)Regs->EBX; //BL

    //Initialize return as successful.
    Regs->EFLAGS &= ~CARRY_FLAG;
    Regs->EAX &= 0xffff0000;

    if (IsInitError || Function >= MAX_MICROCODE_UPDATE_FUNCTIONS) {
        Regs->EFLAGS |= CARRY_FLAG;
        *(UINT16*)&Regs->EAX = UC_NOT_IMPLEMENTED;
        goto Exit;
    }

    MicrocodeUpdate[Function](Regs);
Exit:

    return EFI_SUCCESS;
}

/**
Initialize service to processes Interrupt 15h, function D042h to update microcode in flash.

@param ImageHandle   Driver Image Handle.
@param SystemTable   Pointer to the EFI System Table.

@retval EFI_SUCCESS  Services installed successfully.
**/

EFI_STATUS EFIAPI InitializeMicrocodeSmm(
	IN EFI_HANDLE           ImageHandle,
	IN EFI_SYSTEM_TABLE     *SystemTable
	)

{
    EFI_SMM_SW_DISPATCH2_PROTOCOL   *pSwDispatch;
    EFI_SMM_SW_REGISTER_CONTEXT     SwContext;
    EFI_STATUS  Status;
    EFI_HANDLE  Handle;

    BOOLEAN IsInit = InitMicrocodeVariables();
    if (!IsInit) return EFI_UNSUPPORTED;

    Status = gSmst->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch);
	ASSERT_EFI_ERROR(Status);

    Status = gSmst->SmmLocateProtocol(&	gEfiSmmCpuProtocolGuid, NULL, &SmmCpuProtocol);
	ASSERT_EFI_ERROR(Status);

    SwContext.SwSmiInputValue = INT15_D042_SWSMI;

	Status	= pSwDispatch->Register(pSwDispatch, MicrocodeUpdateCallback, &SwContext, &Handle);
	ASSERT_EFI_ERROR(Status);

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
