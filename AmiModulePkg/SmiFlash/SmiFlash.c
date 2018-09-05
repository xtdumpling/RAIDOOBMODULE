//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.04
//    Bug Fix : N/A.
//    Reason  : Removed redundant "64MB BIOS flash support" token in source code.
//    Auditor : Joe Jhang
//    Date    : Jun/16/2017
//
//  Rev. 1.03
//    Bug Fix : N/A.
//    Reason  : Defended SMCI OOB in-band SW SMI 0xE6 with SMM communication 
//              mail box mechanism.
//    Auditor : Joe Jhang
//    Date    : Jun/06/2017
//
//  Rev. 1.02
//    Bug Fix:  Fix SMBIOS can't reserve when flashing BIOS which modify
//              setup item.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Apr/24/2017
//
//  Rev. 1.01
//    Bug Fix:  Add 64MB BIOS flash support.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/01/2017
//
//  Rev. 1.00
//    Bug Fix:  Fix building error when enabling DEBUG_MODE.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/01/2016
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  SmiFlash SMM Driver File.
**/

//----------------------------------------------------------------------
// header includes
#include <AmiDxeLib.h>
#include <Library/Debuglib.h>
#include <Token.h>
#include <Protocol/SmiFlash.h>
#include <SMIFlashELinks.h>
#if PI_SPECIFICATION_VERSION >= 0x1000A
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/HobLib.h>
#define RETURN(status) {return status;}
#else
#include <Protocol/SmmBase.h>
#include <Protocol/SmmSwDispatch.h>
#define RETURN(status) {return ;}
#endif
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/FlashProtocol.h>
#include <Flash.h>
#include <AmiSmm.h>
#include <Setup.h>
#include <RomLayout.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/SmmAccess2.h>
#include <Library/AmiBufferValidationLib.h>
#define ROM_LAYOUT_FFS_GUID \
    { 0x0DCA793A, 0xEA96, 0x42d8, 0xBD, 0x7B, 0xDC, 0x7F, 0x68, 0x4E, 0x38, 0xC1 }

#define NVRAM_HOB_GUID \
    { 0xc0ec00fd, 0xc2f8, 0x4e47, 0x90, 0xef, 0x9c, 0x81, 0x55, 0x28, 0x5b, 0xec }	

//----------------------------------------------------------------------
// component MACROs
#define FLASH_EMPTY_BYTE (UINT8)(-FLASH_ERASE_POLARITY)
#define STR(a) CONVERT_TO_STRING(a)

#if NVRAM_MIGRATION
#include <Protocol/AmiNvramUpdate.h>
#include <Guid/AmiNvram.h>
#define ROM_LAYOUT_SIGNATURE SIGNATURE_32('R', 'O', 'M', 'L')
#pragma pack(1)
// Define RomLayout structure here avoiding kernel "ROM_AREA" structure changed 
// in future.
typedef struct{
    UINT32  Signature;
    UINT32  Version;
    UINT32  DesriptorSize;
    UINT32  NumberOfDescriptors;
}ROM_LAYOUT_HDR;
typedef struct{
    EFI_GUID                Name;
    EFI_PHYSICAL_ADDRESS    Address;
    UINT32                  Offset;
    UINT32                  Size;
    ROM_AREA_TYPE           Type;
    UINT32                  Attributes;
}ROM_LAYOUT;
#pragma pack()

typedef struct {
    EFI_HOB_GUID_TYPE Header;
    EFI_PHYSICAL_ADDRESS NvramAddress;
    EFI_PHYSICAL_ADDRESS BackupAddress;
    UINT32 NvramSize;
    UINT32 HeaderLength;
    UINT32 NvramMode;
} NVRAM_HOB;

#endif // #if NVRAM_MIGRATION

//----------------------------------------------------------------------
// Module defined global variables

//----------------------------------------------------------------------
// Module specific global variable
EFI_GUID gSwSmiCpuTriggerGuid = SW_SMI_CPU_TRIGGER_GUID;
#if PI_SPECIFICATION_VERSION >= 0x1000A
//EFI_GUID gEfiSmmSwDispatchProtocolGuid = EFI_SMM_SW_DISPATCH2_PROTOCOL_GUID;
//EFI_GUID gEfiSmmBase2ProtocolGuid = EFI_SMM_BASE2_PROTOCOL_GUID;
EFI_GUID gEfiSmmCpuProtocolGuid = EFI_SMM_CPU_PROTOCOL_GUID;
EFI_SMM_BASE2_PROTOCOL          *gSmmBase2;
EFI_SMM_CPU_PROTOCOL            *gSmmCpu;
#else
EFI_GUID gEfiSmmSwDispatchProtocolGuid = EFI_SMM_SW_DISPATCH_PROTOCOL_GUID;
#endif
EFI_GUID gEfiSmiFlashProtocolGuid = EFI_SMI_FLASH_GUID;
EFI_GUID AmiNvramHobGuid = NVRAM_HOB_GUID;
#if NVRAM_MIGRATION
EFI_GUID AmiSmmNvramUpdateProtocolGuid = AMI_SMM_NVRAM_UPDATE_PROTOCOL_GUID;
#endif
// oem flash write enable/disable list creation code must be in this order
typedef VOID (SMIFLASH_UPDATE) (VOID);
extern SMIFLASH_UPDATE SMIFLASH_PRE_UPDATE_LIST EndOfSMIFlashList;
extern SMIFLASH_UPDATE SMIFLASH_END_UPDATE_LIST EndOfSMIFlashList;
SMIFLASH_UPDATE* SMIFlashPreUpdate[] = {SMIFLASH_PRE_UPDATE_LIST NULL};
SMIFLASH_UPDATE* SMIFlashEndUpdate[] = {SMIFLASH_END_UPDATE_LIST NULL};
#if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
typedef VOID (SMIFLASH_OEM_FUNC_HOOK) (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
extern SMIFLASH_OEM_FUNC_HOOK SMIFLASH_OEM_FUNC_HOOKS_LIST EndOfSMIFlashOemFuncHookList;
SMIFLASH_OEM_FUNC_HOOK* SMIFlashOemFuncHook[] = { SMIFLASH_OEM_FUNC_HOOKS_LIST NULL };
#endif
typedef VOID (SMIFLASH_HANDLER_HOOK) (UINT8 Date, UINT64 pInfoBlock);
extern SMIFLASH_HANDLER_HOOK SMIFLASH_PRE_HANDLER_LIST EndOfPreHandlerList;
extern SMIFLASH_HANDLER_HOOK SMIFLASH_END_HANDLER_LIST EndOfEndHandlerList;
SMIFLASH_HANDLER_HOOK* SMIFlashPreHandler[] = {SMIFLASH_PRE_HANDLER_LIST NULL};
SMIFLASH_HANDLER_HOOK* SMIFlashEndHandler[] = {SMIFLASH_END_HANDLER_LIST NULL};

typedef VOID (SMIFLASH_INIT) (VOID);
extern SMIFLASH_INIT SMIFLASH_IN_SMM_LIST EndOfInSmmList;
extern SMIFLASH_INIT SMIFLASH_NOT_IN_SMM_LIST EndOfNotInSmmList;
SMIFLASH_INIT* SMIFlashInSmm[] = {SMIFLASH_IN_SMM_LIST NULL};

FLASH_PROTOCOL *Flash = NULL;
#ifdef FLASH_PART_STRING_LENGTH
VOID GetFlashPartInfomation(UINT8 *pBlockAddress, UINT8 *Buffer);
// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
#define ExtraFlashInfoBlockSize FLASH_PART_STRING_LENGTH + 5 + 8
#else
#define ExtraFlashInfoBlockSize 5
// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
#endif
static EFI_GUID RomLayoutFfsGuid = ROM_LAYOUT_FFS_GUID;
ROM_AREA *RomLayout;
UINTN    NumberOfRomLayout = 0;
UINTN    NumberOfNcb = 0;
static EFI_SMRAM_DESCRIPTOR *mSmramRanges;
static UINTN                mSmramRangeCount;
UINT64 CurNvramAddress = 0; 
UINT64 CurNvramEndAddress = 0; 
UINT64 CurNvramBackupAddress = 0;
UINT32  CurNvramSize = 0;
BOOLEAN DoNvramMigration = FALSE;
//----------------------------------------------------------------------
// externally defined variables

//----------------------------------------------------------------------
// Function definitions
EFI_STATUS
GetFlashInfo(
    IN  OUT INFO_BLOCK  *pInfoBlock
);

EFI_STATUS
EnableFlashWrite(
    IN  OUT FUNC_BLOCK  *pFuncBlock
);

EFI_STATUS
DisableFlashWrite(
    IN  OUT FUNC_BLOCK  *pFuncBlock
);

EFI_STATUS
ReadFlash(
    IN  OUT FUNC_BLOCK  *pFuncBlock
);

EFI_STATUS
WriteFlash(
    IN  OUT FUNC_BLOCK  *pFuncBlock
);

EFI_STATUS
EraseFlash(
    IN  OUT FUNC_BLOCK  *pFuncBlock
);

EFI_SMI_FLASH_PROTOCOL  SmiFlash = {
    GetFlashInfo,
    EnableFlashWrite,
    DisableFlashWrite,
    ReadFlash,
    WriteFlash,
    EraseFlash,
    FLASH_SIZE
};

//APTIOV_SERVER_OVERRIDE_START: Fix added for SW SMI failure 
/// Defines format of the SMM Communication Mailbox
typedef struct {
    UINT64 Revision; ///< Revision of this data structure
    UINT64 PhysicalAddress; ///< Physical address of the SMM Communication buffer
    UINT32 Status; ///< Status of the last SMI call
    UINT32 Token; ///< Token that identifies SMI caller
} AMI_SMM_COMMUNICATION_MAILBOX;


typedef struct {
    EFI_PHYSICAL_ADDRESS Address;
    UINT64 Size;
} SMM_AMI_BUFFER_ADDRESS_RANGE;

#define AMI_SMM_COMMUNICATION_MAILBOX_REVISION 1
#define AMI_SMM_COMMUNICATION_MAILBOX_COMMAND_LOCK 0xC0000001
#define AMI_SMM_COMMUNICATION_MAILBOX_COMMAND_UNLOCK 0xC0000002
#define AMI_SMM_COMMUNICATION_MAILBOX_LOCK_FLAG_FORCE_LOCK 1
#define EFI_STATUS_TO_SMM_MAILBOX_STATUS(Status) (UINT32)( (EFI_ERROR(Status)) ? 0x80000000|(UINT32)(Status) : (UINT32)(Status) )
#define SMM_COMMUNICATION_BUFFER_SWSMI 0xD9

AMI_SMM_COMMUNICATION_MAILBOX *gSmmMailBox = NULL;
BOOLEAN gSmmBufferLocked = FALSE;
UINT32 gSmmTransactionId = 0;
EFI_PHYSICAL_ADDRESS gSmmCommunicationBuffer = 0;
UINT32 gSmmCommunicationBufferSize = 0;

/**
    Set Smi Status in ECX register.

    @param CpuIndex parameter that specifies which CPU to store SMI status.
    @param Status parameter that specifies SMI status.

    @retval VOID
**/

VOID SetSmiStatus(UINTN CpuIndex, EFI_STATUS Status){

    UINT32 SmiStatus = EFI_STATUS_TO_SMM_MAILBOX_STATUS(Status);
    EFI_SMM_CPU_SAVE_STATE  *pCpuSaveState = NULL;

#if PI_SPECIFICATION_VERSION >= 0x1000A  
    Status = gSmmCpu->WriteSaveState(
        gSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RCX, CpuIndex, &SmiStatus
    );
    ASSERT_EFI_ERROR(Status);
#else
    pCpuSaveState       = pSmst->CpuSaveState;
    pCpuSaveState[Cpu].Ia32SaveState.ECX = SmiStatus;
#endif
}

/**
    The SMI handler of SMM communication.
    
    @param DispatchHandle - Dispatch Handle
    @param Context - Pointer to the passed context
    @param CommBuffer - Pointer to the passed Communication Buffer
    @param CommBufferSize - Pointer to the Comm Buffer Size

    @return EFI_STATUS
    @retval EFI_SUCCESS Function executed successfully
*/ 

#if PI_SPECIFICATION_VERSION >= 0x1000A
EFI_STATUS 
SmmCommunicationBufferSwDispatch (
    IN  EFI_HANDLE                  DispatchHandle,
    IN CONST VOID                   *Context OPTIONAL,
    IN OUT VOID                     *CommBuffer OPTIONAL,
    IN OUT UINTN                    *CommBufferSize OPTIONAL
)
#else
VOID SmmCommunicationBufferSwDispatch (
    IN  EFI_HANDLE                  DispatchHandle,
    IN  EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
)
#endif
{
    EFI_SMM_CPU_SAVE_STATE  *pCpuSaveState = NULL;
    EFI_STATUS  Status = EFI_SUCCESS;
    UINTN       Cpu = (UINTN)-1, i = 0;
    EFI_PHYSICAL_ADDRESS MailboxAddress;
    UINT32      Command;

#if PI_SPECIFICATION_VERSION >= 0x1000A    
    if (CommBuffer != NULL && CommBufferSize != NULL) {
        Cpu = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
    }
    
    //
    // Found Invalid CPU number, return 
    //
    if(Cpu == (UINTN)-1) RETURN(Status);

    Status = gSmmCpu->ReadSaveState ( 
        gSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RCX, Cpu, &Command
    );
    if (EFI_ERROR(Status)) return EFI_SUCCESS;
#else // PI_SPECIFICATION_VERSION < 0x1000A
    SW_SMI_CPU_TRIGGER      *SwSmiCpuTrigger;

    for (i = 0; i < pSmst->NumberOfTableEntries; ++i) {
        if (guidcmp(&pSmst->SmmConfigurationTable[i].VendorGuid,
                                    &gSwSmiCpuTriggerGuid) == 0) {
            break;
        }
    }
    
    //If found table, check for the CPU that caused the software Smi.
    if (i != pSmst->NumberOfTableEntries) {
        SwSmiCpuTrigger = pSmst->SmmConfigurationTable[i].VendorTable;
        Cpu = SwSmiCpuTrigger->Cpu;
    }

    //
    // Found Invalid CPU number, return 
    //
    if(Cpu == (UINTN) -1) RETURN(Status);

    Data    = (UINT8)DispatchContext->SwSmiInputValue;
    
    pCpuSaveState       = pSmst->CpuSaveState;
    Command             = pCpuSaveState[Cpu].Ia32SaveState.ECX;
#endif
    
    if (Command == AMI_SMM_COMMUNICATION_MAILBOX_COMMAND_LOCK ){
        // Lock
        UINT32 BufferSize;
        UINT32 Flags;
#if PI_SPECIFICATION_VERSION >= 0x1000A  
        Status = gSmmCpu->ReadSaveState(
            gSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RSI, Cpu, &BufferSize
        );
        if (EFI_ERROR(Status)) return EFI_SUCCESS;
        Status = gSmmCpu->ReadSaveState(
            gSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RDI, Cpu, &Flags
        );
        if (EFI_ERROR(Status)) return EFI_SUCCESS;
#else // PI_SPECIFICATION_VERSION < 0x1000A
        BufferSize = pCpuSaveState[Cpu].Ia32SaveState.ESI;
        Flags = pCpuSaveState[Cpu].Ia32SaveState.EDI;
#endif
        if ( gSmmBufferLocked 
             && (Flags & AMI_SMM_COMMUNICATION_MAILBOX_LOCK_FLAG_FORCE_LOCK)==0){
            DEBUG ((DEBUG_ERROR, "SmiFlash ERROR: Attempt to lock application SMM communicatTRACE_ALWAYSion buffer that is already locked.\n"));
            Status = EFI_ACCESS_DENIED;
            SetSmiStatus(Cpu, Status);
            return EFI_SUCCESS;
        }
        
        if (BufferSize > gSmmCommunicationBufferSize){
            DEBUG ((DEBUG_ERROR, 
                "SmiFlash ERROR: Requested application SMM communication buffer size is not supported.\nRequested size=%X; Supported size=%X.\n",
                BufferSize, gSmmCommunicationBufferSize
            ));
            Status = EFI_BAD_BUFFER_SIZE;
            SetSmiStatus(Cpu, Status);
            BufferSize = gSmmCommunicationBufferSize;
#if PI_SPECIFICATION_VERSION >= 0x1000A 
            Status = gSmmCpu->WriteSaveState(
                gSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RSI, Cpu, &BufferSize
            );
            ASSERT_EFI_ERROR(Status);
#else // PI_SPECIFICATION_VERSION < 0x1000A
            pCpuSaveState[Cpu].Ia32SaveState.ESI = BufferSize;
#endif
            return EFI_SUCCESS;
        }
        gSmmTransactionId++;
        gSmmBufferLocked = TRUE;
        gSmmMailBox->Revision = AMI_SMM_COMMUNICATION_MAILBOX_REVISION;
        gSmmMailBox->PhysicalAddress = gSmmCommunicationBuffer;
        gSmmMailBox->Token = gSmmTransactionId;
        gSmmMailBox->Status = 0;
        
        MailboxAddress = (UINTN)gSmmMailBox;
#if PI_SPECIFICATION_VERSION >= 0x1000A 
        Status = gSmmCpu->WriteSaveState(
            gSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RSI, Cpu, &MailboxAddress
        );
        ASSERT_EFI_ERROR(Status);
        Status = gSmmCpu->WriteSaveState(
            gSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RDI, Cpu, (UINT32*)&MailboxAddress + 1
        );
        ASSERT_EFI_ERROR(Status);
#else // PI_SPECIFICATION_VERSION < 0x1000A
        pCpuSaveState[Cpu].Ia32SaveState.ESI = MailboxAddress;
        pCpuSaveState[Cpu].Ia32SaveState.EDI = (UINT32)MailboxAddress + 1;
#endif
    }else if (Command == AMI_SMM_COMMUNICATION_MAILBOX_COMMAND_UNLOCK ){
        // Unlock
        if (!gSmmBufferLocked){
            Status = EFI_ACCESS_DENIED;
            SetSmiStatus(Cpu,Status);
            DEBUG ((DEBUG_ERROR, "ERROR: Attempt to unlock application SMM communication buffer that is not locked.\n"));
            return EFI_SUCCESS;
        }
        if (    gSmmMailBox->Revision != AMI_SMM_COMMUNICATION_MAILBOX_REVISION
             || gSmmMailBox->PhysicalAddress != gSmmCommunicationBuffer
             || gSmmMailBox->Token != gSmmTransactionId
        ){
            Status = EFI_INVALID_PARAMETER;
            SetSmiStatus(Cpu,Status);
            DEBUG ((DEBUG_ERROR, "ERROR: Attempt to unlock SMM communication buffer with invalid mailbox content.\n"));
            return EFI_SUCCESS;
        }
        gSmmBufferLocked = FALSE;
        gSmmMailBox->Token = 0;
        gSmmMailBox->Status = 0;
    }else {
        SetSmiStatus(Cpu,EFI_UNSUPPORTED);
        return EFI_SUCCESS;
    }
    SetSmiStatus(Cpu,EFI_SUCCESS);
    return EFI_SUCCESS;
}

/**
    Validate the buffer address is within SPI image address range.

    @param Buffer parameter that specifies Buffer address to be Validated.
    @param BufferSize parameter that specifies Buffer size.

    @retval VOID
**/

EFI_STATUS EFIAPI ValidateFlashBufferAddress(CONST VOID* Buffer, CONST UINTN BufferSize){
    UINTN BufferAddress;
    EFI_PHYSICAL_ADDRESS SpiImageBase;

    if (Buffer == NULL) return EFI_INVALID_PARAMETER;

    BufferAddress = (UINTN)Buffer;
    if (BufferAddress - 1 + BufferSize < BufferAddress) return EFI_INVALID_PARAMETER; // overflow
    
    SpiImageBase = 0xFFFFFFFF - SPI_IMAGE_SIZE + 1;
    
    if (BufferAddress >= SpiImageBase && (BufferAddress + BufferSize - 1) <= 0xFFFFFFFF) 
        return EFI_SUCCESS; // Entire Buffer is in the white list region

    return EFI_ACCESS_DENIED;
}

/**
    Validate the buffer address is within smm communication buffer.

    @param Buffer parameter that specifies Buffer address to be Validated.
    @param BufferSize parameter that specifies Buffer size.

    @retval EFI_SUCCESS Validate pass.
**/
EFI_STATUS CheckSmmCommunicationBuffer(
    CONST VOID* Buffer, CONST UINTN BufferSize
){
    UINTN BufferAddress;
    EFI_PHYSICAL_ADDRESS EndOfCommBuffer;

    if (Buffer == NULL) return EFI_INVALID_PARAMETER;

    if (gSmmMailBox == NULL) return EFI_SUCCESS;
    BufferAddress = (UINTN)Buffer;
    if (BufferAddress + BufferSize < BufferAddress) return EFI_INVALID_PARAMETER; // overflow

    EndOfCommBuffer = gSmmCommunicationBuffer + gSmmCommunicationBufferSize;
    // If buffer is outside SMM Communication buffer, return EFI_ACCESS_DENIED
    if (    BufferAddress >= EndOfCommBuffer
         || BufferAddress+BufferSize <= gSmmCommunicationBuffer
    ) return EFI_ACCESS_DENIED;
    
    // If buffer overlaps with the SMM Communication buffer, but is not entirely
    // in the SMM Communication buffer, return an error
    if (    BufferAddress < gSmmCommunicationBuffer
         || BufferAddress+BufferSize > EndOfCommBuffer
    ){
        gSmmMailBox->Status = EFI_STATUS_TO_SMM_MAILBOX_STATUS(EFI_ACCESS_DENIED);
        return EFI_ACCESS_DENIED;
    }
    // If buffer is in the SMM Communication buffer, the size can't be zero
    if (BufferSize == 0){
        gSmmMailBox->Status = EFI_STATUS_TO_SMM_MAILBOX_STATUS(EFI_INVALID_PARAMETER);
        return EFI_INVALID_PARAMETER;
    }

    // Buffer must be locked before it's used.
    // Owner must initialized mailbox field Token before every call.
    if (    !gSmmBufferLocked
         || gSmmMailBox->Token != gSmmTransactionId
    ){
        if (!gSmmBufferLocked)
            DEBUG ((DEBUG_ERROR, "ERROR: Application SMM communication buffer is unlocked.\n"));
        else
            DEBUG ((DEBUG_ERROR, "ERROR: Application SMM communication buffer access token is invalid.\n"));
        gSmmMailBox->Status = EFI_STATUS_TO_SMM_MAILBOX_STATUS(EFI_ACCESS_DENIED);
        return EFI_ACCESS_DENIED;
    }
    
    gSmmMailBox->Status = 0;
    return EFI_SUCCESS;
}
//APTIOV_SERVER_OVERRIDE_END: Fix added for SW SMI failure

/**
    Procedure to report the Flash Blocks Information.

    @param pInfoBlock parameter that specifies the where the blocks infomation to be stored.

    @retval pInfoBlock parameter that specifies the blocks infomation
**/
EFI_STATUS
RomLayoutMigrationHandler(
    IN  OUT INFO_BLOCK  *pInfoBlock
)
{
#if NVRAM_MIGRATION
    BOOLEAN         IsNvramMigrated = FALSE;
    UINT8           *FvHeaderBuffer = NULL;
    UINT32          MaxBufferLength, Index;
    UINT64          NvramAddress, NvramBackupAddress;
    UINTN           NvramSize;
    EFI_GUID        NvramGuid = AMI_NVRAM_MAIN_ROM_AREA_GUID;
    EFI_GUID        NvramBackupGuid = AMI_NVRAM_BACKUP_ROM_AREA_GUID;
	AMI_NVRAM_UPDATE_PROTOCOL   *AmiNvramUpdate;
	ROM_LAYOUT_HDR  *RomLayoutHdr = (ROM_LAYOUT_HDR*)&pInfoBlock->Blocks[1];
	ROM_LAYOUT      *RomLayout = (ROM_LAYOUT*)((UINT8*)RomLayoutHdr + sizeof(ROM_LAYOUT_HDR));

    if (RomLayoutHdr->Signature != ROM_LAYOUT_SIGNATURE) return EFI_INVALID_PARAMETER;

    MaxBufferLength = pInfoBlock->Length - sizeof(BLOCK_DESC) - \
                EFI_FIELD_OFFSET(INFO_BLOCK, Blocks) - sizeof(ROM_LAYOUT_HDR);

    FvHeaderBuffer = (UINT8*)RomLayout + \
                        MaxBufferLength - sizeof(EFI_FIRMWARE_VOLUME_HEADER);
    if (FvHeaderBuffer <= ((UINT8*)RomLayout + RomLayoutHdr->DesriptorSize * \
                RomLayoutHdr->NumberOfDescriptors)) FvHeaderBuffer = NULL;

    // Reduce number of bytes used for header information
    for (Index = 0; Index < RomLayoutHdr->NumberOfDescriptors; Index++, RomLayout++) {
        if ((RomLayout >= (ROM_LAYOUT*)((UINT8*)RomLayoutHdr + MaxBufferLength)) || \
            !RomLayout->Address || !RomLayout->Size) return EFI_INVALID_PARAMETER;

        if (!guidcmp(&RomLayout->Name, &NvramGuid)) {
            NvramAddress = RomLayout->Address;
            NvramSize = RomLayout->Size;
            if (RomLayout->Size != NVRAM_SIZE) return EFI_INVALID_PARAMETER;
            if (RomLayout->Address != NVRAM_ADDRESS) IsNvramMigrated = TRUE;
        }
        if (!guidcmp(&RomLayout->Name, &NvramBackupGuid)) {
            NvramBackupAddress = RomLayout->Address;
            if (RomLayout->Size != NVRAM_SIZE) return EFI_INVALID_PARAMETER;
#if defined(FAULT_TOLERANT_NVRAM_UPDATE) && (FAULT_TOLERANT_NVRAM_UPDATE == 1)
            if( RomLayout->Address != NVRAM_BACKUP_ADDRESS ) IsNvramMigrated = TRUE;
#endif
        }
        // Check and restore the Firmware Volume Header Signature if missing.
        if (FvHeaderBuffer && (RomLayout->Type == RomAreaTypeFv)) {
            FUNC_BLOCK    FuncBlock;
            // Read the FV HEADER back by new RomLayout.
            FuncBlock.BufAddr   = (UINT64)FvHeaderBuffer;
            FuncBlock.BlockAddr = RomLayout->Offset;
            FuncBlock.BlockSize = sizeof(EFI_FIRMWARE_VOLUME_HEADER);
            if (!EFI_ERROR(ReadFlash(&FuncBlock))) {
                EFI_FIRMWARE_VOLUME_HEADER  *FvHdr;
                FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)FvHeaderBuffer;
                // Restore the FV Signature if missing.
                if (!guidcmp(&FvHdr->FileSystemGuid, &gEfiFirmwareFileSystem2Guid) && \
                    (FvHdr->Signature == (UINT32)-1)) {
                    FvHdr->Signature = EFI_FVH_SIGNATURE;    
                    FuncBlock.BufAddr   = (UINT64)&FvHdr->Signature;
                    FuncBlock.BlockSize = sizeof(EFI_FVH_SIGNATURE);
                    FuncBlock.BlockAddr = RomLayout->Offset + \
                        EFI_FIELD_OFFSET(EFI_FIRMWARE_VOLUME_HEADER, Signature);
                    WriteFlash(&FuncBlock);
                }    
            }
        }
    }

    if (IsNvramMigrated) {
        if (!EFI_ERROR(pSmst->SmmLocateProtocol (&AmiSmmNvramUpdateProtocolGuid, NULL, &AmiNvramUpdate))) {			
            DoNvramMigration = FALSE; //Don't do Nvram migration again						
            AmiNvramUpdate->MigrateNvram (AmiNvramUpdate, NvramAddress, NvramBackupAddress, NvramSize);
        }
    }
#endif  // #if NVRAM_MIGRATION
    return EFI_SUCCESS;
}

/**
    Procedure to get Nvram and NvBackup address from HOB
**/
VOID 
GetCurrentNvramInfo (
    VOID
)
{
#if NVRAM_MIGRATION
	VOID                       *GuidHob;
	NVRAM_HOB                  *NvramHob;
	
	GuidHob = GetFirstGuidHob (&AmiNvramHobGuid);
	
	if (GuidHob != NULL) {
        NvramHob = (NVRAM_HOB *)GuidHob;
		CurNvramAddress = NvramHob->NvramAddress;
		CurNvramBackupAddress = NvramHob->BackupAddress;
		CurNvramSize = NvramHob->NvramSize;
		CurNvramEndAddress = CurNvramAddress + CurNvramSize;
	}
#endif	
}
/**
    Procedure to generate the Flash Blocks Information Descriptors.

    @param pInfoBlock parameter that specifies the where the blocks infomation to be stored.

    @retval pInfoBlock parameter that specifies the blocks infomation
**/
EFI_STATUS
GenBlockInfoDescriptors(
    IN  OUT INFO_BLOCK  *pInfoBlock
)
{
    UINT32          BuffLen = 0;
    UINT64          BlkOff;
    BLOCK_DESC      *pBlock;
    UINT16          Index;
    UINT32          Add;
    UINT32          Flash4GBMapStart;
    UINT16          NumBlocks;
    UINT16          LastNcb = 0xFFFF;
    UINT8           NcbType = NC_BLOCK;
    UINT8           i;
	UINT8           InRomLayout;
	UINT16          *StartBlock, *EndBlock;
	EFI_STATUS      Status;

    // Total buffer length
    // BuffLen = pInfoBlock->Length;       

    // Reduce number of bytes used for header information
    BuffLen = pInfoBlock->Length - EFI_FIELD_OFFSET(INFO_BLOCK, Blocks);

    // Get the total block count
    pInfoBlock->TotalBlocks = NumBlocks = NUMBER_OF_BLOCKS;

    // Calculate the flash mapping start address. This is calculated
    // as follows:
    //  1. Find the total size of the flash (FLASH_BLOCK_SIZE *
    //      pInfoBlock->TotalBlocks)
    //  2. Subtract the total flash size from 4GB
    Flash4GBMapStart = 0xFFFFFFFF - (FLASH_BLOCK_SIZE * NumBlocks);
    Flash4GBMapStart ++;

    BlkOff = (UINT64)&pInfoBlock->Blocks;

    // Chek whether we have enough buffer space
	// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
    if (BuffLen < (sizeof (BLOCK_DESC) * NumBlocks)) {
        pInfoBlock->TotalBlocks = 0;
        pInfoBlock->Implemented = 1;
        return EFI_BUFFER_TOO_SMALL;
    }
	// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.

	//Calculate RomLayout's start ant end block
	Status = pSmst->SmmAllocatePool (
						EfiRuntimeServicesData,
						NumberOfRomLayout * sizeof(UINT16),
						(VOID**)&StartBlock );
	if (EFI_ERROR(Status)) return Status;
	Status = pSmst->SmmAllocatePool (
						EfiRuntimeServicesData,
						NumberOfRomLayout * sizeof(UINT16),
						(VOID**)&EndBlock );
	if (EFI_ERROR(Status)) return Status;
	for( i = 0; i < NumberOfRomLayout; i++ )
	{
		StartBlock[i] = RomLayout[i].Offset / FLASH_BLOCK_SIZE;
		EndBlock[i] = StartBlock[i] + (RomLayout[i].Size / FLASH_BLOCK_SIZE) - 1;
	}

	//Assign types
	NcbType = NcbType + (UINT8)NumberOfNcb - 1;
    for (pBlock = NULL, Index = 0; Index < NumBlocks; Index++) {

        pBlock = (BLOCK_DESC *)BlkOff;
        Add = Index * FLASH_BLOCK_SIZE;

        pBlock->StartAddress = Add;
        Add |= Flash4GBMapStart;
        pBlock->BlockSize = FLASH_BLOCK_SIZE;
		
		InRomLayout = 0;
		for( i = 0; i < NumberOfRomLayout; i++ )
		{
			if( Index >= StartBlock[i] && Index <= EndBlock[i] )
			{
				pBlock->Type = RomLayout[i].Type;
				InRomLayout = 1;
				break;
			}
		}

		if ( InRomLayout == 0 )
		{
			if (LastNcb+1 != Index) NcbType++;
			pBlock->Type = NcbType;
			LastNcb=Index;
		}
        BlkOff += sizeof (BLOCK_DESC);
    }
	
	pSmst->SmmFreePool(StartBlock);
	pSmst->SmmFreePool(EndBlock);

    // Info AFU Project Tag length.
    if (((UINT64)pInfoBlock + pInfoBlock->Length) > (BlkOff + 5))
    {
        CHAR8*    ProjectTag = STR(PROJECT_TAG);
        UINTN     TagLength;
        UINT8     ProjectTagSign[4] = {'$','B','P','T'};

        TagLength = Strlen(ProjectTag);

        MemCpy ( (UINT8*)BlkOff, ProjectTagSign, 4 );
        BlkOff += sizeof (UINT32);
        *(UINT8*)BlkOff++ = (UINT8)TagLength;
        // Added for AFU to identify the Core Version.
        *(UINT8*)BlkOff = 'V';
    }

#if defined (FLASH_PART_STRING_LENGTH) && (FLASH_PART_STRING_LENGTH != 0)
    if (((UINT64)pInfoBlock + pInfoBlock->Length) > \
            (++BlkOff + FLASH_PART_STRING_LENGTH + 8))
        GetFlashPartInfomation ( (UINT8*)Flash4GBMapStart, (UINT8*)BlkOff );
#endif
    return EFI_SUCCESS;
}
/**
    Procedure to report the Flash Blocks Information.

    @param pInfoBlock parameter that specifies the where the blocks infomation to be stored.

    @retval pInfoBlock parameter that specifies the blocks infomation
**/
EFI_STATUS
GetFlashInfo(
    IN  OUT INFO_BLOCK  *pInfoBlock
)
{
	// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
    EFI_STATUS	Status;
	// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
    IoWrite8( 0x80,0x25 );
    pInfoBlock->Version = SMI_FLASH_INTERFACE_VERSION;
    //pInfoBlock->Implemented = 0;
#if SMI_FLASH_INTERFACE_VERSION > 10
#if EC_FIRMWARE_UPDATE_INTERFACE_SUPPORT
    pInfoBlock->ECVersionOffset = EC_VERSION_OFFSET;
    pInfoBlock->ECVersionMask = EC_VERSION_MASK;
#else
    pInfoBlock->ECVersionOffset = 0;
    pInfoBlock->ECVersionMask = 0;
#endif
#endif
// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
#if SMI_FLASH_INTERFACE_VERSION >= 14
    pInfoBlock->BaseBlockSize = 0;
    pInfoBlock->BiosRomSize = 0;
    pInfoBlock->Reserved[0] = 0;
    pInfoBlock->Reserved[1] = 0;
    pInfoBlock->Reserved[2] = 0;
    pInfoBlock->Reserved[3] = 0;	
#endif
// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
    switch( pInfoBlock->Implemented )
    {
        // Rom Layout migration handle.
        case 2 :
#if FAULT_TOLERANT_NVRAM_UPDATE == 1 // Supermicro
            Status = RomLayoutMigrationHandler( pInfoBlock );
#endif // #if FAULT_TOLERANT_NVRAM_UPDATE == 1 // Supermicro
            break;
        // Generate Block Infomation Descripters
        default :
			// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
            Status = GenBlockInfoDescriptors( pInfoBlock);
			// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
    }
	// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
	if (!EFI_ERROR(Status)) pInfoBlock->Implemented = 0;
    return Status;
	// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
}

/**
    Procedure to enable Flash Write.

    @param pFuncBlock parameter that specifies the Block information.

    @retval EFI_SUCCESS Flash Write enable successfully.
**/
EFI_STATUS
EnableFlashWrite(
    IN  OUT FUNC_BLOCK  *pFuncBlock
)
{
    return Flash->DeviceWriteEnable();
}

/**
    Procedure to disable Flash Write.

    @param pFuncBlock parameter that specifies the Block information.

    @retval EFI_SUCCESS Flash Write disable successfully.
**/
EFI_STATUS
DisableFlashWrite(
    IN  OUT FUNC_BLOCK  *pFuncBlock
)
{
    return Flash->DeviceWriteDisable();
}

/**
    Procedure to erase Flash block.

    @param pFuncBlock parameter that specifies the Block information.

    @retval EFI_SUCCESS Flash erase successfully.
**/
EFI_STATUS
EraseFlash(
    IN  OUT FUNC_BLOCK  *pFuncBlock
)
{
    EFI_STATUS  Status;
    UINT8       *BlockAddress;
    BlockAddress = (UINT8*)(UINTN)(pFuncBlock->BlockAddr + \
                                (0xFFFFFFFF - SmiFlash.FlashCapacity + 1));
    BlockAddress = (UINT8*)((UINTN)BlockAddress & (0xFFFFFFFF - FLASH_BLOCK_SIZE + 1));

    // Checking access rang for avoiding the privilege escalation into SMM 
    // via Smiflash driver.
//    if(EFI_ERROR(AmiValidateMmioBuffer(BlockAddress, FlashBlockSize))){
//		pFuncBlock->ErrorCode = 1;
//		return EFI_ACCESS_DENIED;
//	}
	//APTIOV_SERVER_OVERRIDE_Start: Fix added for SW SMI failure
    if(EFI_ERROR(ValidateFlashBufferAddress (BlockAddress, \
	//APTIOV_SERVER_OVERRIDE_END: Fix added for SW SMI failure
                                FlashBlockSize))) return EFI_ACCESS_DENIED;
#if NVRAM_MIGRATION	
    //Do Nvram migration if Nvram block was erased
	if (!DoNvramMigration && ((UINT64)BlockAddress >= CurNvramAddress) && ((UINT64)BlockAddress < CurNvramEndAddress))
		DoNvramMigration = TRUE;
#endif	
    Status = Flash->Erase(BlockAddress, FlashBlockSize);
    pFuncBlock->ErrorCode = EFI_ERROR(Status) != 0;
    return Status;
}

/**
    Procedure to read Flash block.

    @param pFuncBlock parameter that specifies the Block information.

    @retval EFI_SUCCESS Flash read successfully.
**/
EFI_STATUS
ReadFlash(
    IN  OUT FUNC_BLOCK  *pFuncBlock
)
{
    UINT32      Flash4GBMapStart;
    EFI_STATUS  Status;

    Flash4GBMapStart = 0xFFFFFFFF - (FLASH_BLOCK_SIZE * NUMBER_OF_BLOCKS);
    Flash4GBMapStart ++;

    // Checking access rang for avoiding the privilege escalation into SMM 
    // via Smiflash driver.
//APTIOV_SERVER_OVERRIDE_Start: Fix added for SW SMI failure
    if (EFI_ERROR(ValidateFlashBufferAddress ((UINT8*)(Flash4GBMapStart + \
//APTIOV_SERVER_OVERRIDE_END: Fix added for SW SMI failure
            pFuncBlock->BlockAddr), pFuncBlock->BlockSize))) return EFI_ACCESS_DENIED;
//APTIOV_SERVER_OVERRIDE_Start: Fix added for SW SMI failure
    if (EFI_ERROR(CheckSmmCommunicationBuffer ((UINT8*)pFuncBlock->BufAddr, \
//APTIOV_SERVER_OVERRIDE_END: Fix added for SW SMI failure
                                    pFuncBlock->BlockSize))) return EFI_ACCESS_DENIED;
    Status = Flash->Read(
                 (UINT8*)(Flash4GBMapStart + pFuncBlock->BlockAddr),
                 pFuncBlock->BlockSize,
                 (UINT8*)pFuncBlock->BufAddr
             );
    pFuncBlock->ErrorCode = EFI_ERROR(Status) != 0;
    return Status;
}

/**
    Procedure to write Flash block.

    @param pFuncBlock parameter that specifies the Block information.

    @retval EFI_SUCCESS Flash write successfully.
**/
EFI_STATUS
WriteFlash(
    IN  OUT FUNC_BLOCK  *pFuncBlock
)
{
    EFI_STATUS  Status;
    UINT8       *FlashAddress;

    FlashAddress = (UINT8*)(UINTN)(pFuncBlock->BlockAddr + \
                                    (0xFFFFFFFF - SmiFlash.FlashCapacity + 1));
    // Checking access rang for avoiding the privilege escalation into SMM 
    // via Smiflash driver.
//APTIOV_SERVER_OVERRIDE_Start: Fix added for SW SMI failure
    if (EFI_ERROR(ValidateFlashBufferAddress ((UINT8*)FlashAddress, \
//APTIOV_SERVER_OVERRIDE_END: Fix added for SW SMI failure
                            pFuncBlock->BlockSize))) return EFI_ACCESS_DENIED;
//APTIOV_SERVER_OVERRIDE_Start: Fix added for SW SMI failure
    if (EFI_ERROR(CheckSmmCommunicationBuffer ((UINT8*)pFuncBlock->BufAddr, \
//APTIOV_SERVER_OVERRIDE_END: Fix added for SW SMI failure
                            pFuncBlock->BlockSize))) return EFI_ACCESS_DENIED;

    Status = Flash->Write(
                 FlashAddress, pFuncBlock->BlockSize, (UINT8*)pFuncBlock->BufAddr
             );
    pFuncBlock->ErrorCode = EFI_ERROR(Status) != 0;
    return Status;
}

// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
#if SMI_FLASH_INTERFACE_VERSION >= 14
/**
    Procedure to sync up the content of the new INFO_BLOCK buffer

    @param OrgInfoBlock   The original input buffer pointer that comes from flash tools
    @param NewInfoBlock   The new InfoBlock buffer pointer
    @param NewBufferSize  New InfoBlock buffer size

**/
VOID
SyncBufferContent (
    IN     INFO_BLOCK  *OrgInfoBlock,
    IN OUT INFO_BLOCK  *NewInfoBlock,
    IN     UINT32 NewBufferSize
  )
{
	MemCpy ( (UINT8*)NewInfoBlock, (UINT8*)OrgInfoBlock, OrgInfoBlock->Length);
	//Update Buffer length of New Buffer
	NewInfoBlock->Length = NewBufferSize;
}

/**
    Procedure to check if we need to enlarge the INFO_BLOCK buffer and 

    @param pInfoBlock      The original input buffer pointer that comes from flash tools
    @param TempCommBuffer  The new InfoBlock buffer pointer
    @param TempBufferSize  New InfoBlock buffer size

**/
BOOLEAN
NeedPreEnlargeSyncBuffer(
    IN INFO_BLOCK  *pInfoBlock,
    OUT UINT64 *TempCommBuffer
)
{
    UINT32 		BuffLen;
    UINT8		Version;
    UINT32 		BufferSize;
    EFI_STATUS 	BufferStatus;
    BOOLEAN 	AfuNewFormat = FALSE;
    UINT64      *Buffer = NULL;
    static BOOLEAN RetryBufferTooSmall = FALSE;
	
    BuffLen = pInfoBlock->Length - EFI_FIELD_OFFSET(INFO_BLOCK, Blocks);
    Version = pInfoBlock->Version;

    // Use the SMI_FLASH_INTERFACE_VERSION to check if the AFU or SmiFlash 
    // support the Flash part that is equal or bigger than 32Mbyte.	
    if ((Version >=14) && RetryBufferTooSmall) {
        AfuNewFormat = TRUE;
    }else{
        AfuNewFormat = FALSE;
        if (BuffLen < (sizeof (BLOCK_DESC) * NUMBER_OF_BLOCKS))
            RetryBufferTooSmall = TRUE;
        return FALSE;
    }

    // Only support for the new version flash tool
    if ( RetryBufferTooSmall && AfuNewFormat ){
        BufferSize = sizeof (BLOCK_DESC) * NUMBER_OF_BLOCKS + EFI_FIELD_OFFSET(INFO_BLOCK, Blocks) + ExtraFlashInfoBlockSize;
        BufferStatus = pSmst->SmmAllocatePool (
            EfiRuntimeServicesData,
            BufferSize,
            (VOID**)&Buffer );
        if (EFI_ERROR(BufferStatus)){
            return FALSE;
        }
        *TempCommBuffer = (UINT64)Buffer;
        SyncBufferContent( pInfoBlock, (INFO_BLOCK *)Buffer, BufferSize);
        return TRUE;		
    }
    return FALSE;
}

/**
    Procedure to patch the return buffer for Tool Compatibility 

    @param pInfoBlock      The buffer pointer that will send back to flash tools
**/
VOID
PatchFlashToolCompatibility (
  IN INFO_BLOCK  *pInfoBlock
  )
{
    UINT32      TargetSize;
	// Checking the the return info_blocks is the fixed or unfixed info_blocks
	// BaseBlockSize, BiosRomSize are "0" in the fixed size formate
    if ((pInfoBlock->BaseBlockSize ==0) && (pInfoBlock->BiosRomSize == 0)){
        TargetSize = sizeof (BLOCK_DESC)* NUMBER_OF_BLOCKS + ExtraFlashInfoBlockSize;

        if (TargetSize >= pInfoBlock->Length)
            TargetSize = pInfoBlock->Length - EFI_FIELD_OFFSET(INFO_BLOCK, BiosRomSize);			
        MemCpy ( (UINT8*)&(pInfoBlock->BiosRomSize), &(pInfoBlock->Blocks[0]), TargetSize);
    }
}

/**
    Procedure to update the BLOCK_DESC

    @param Target      The destination INFO_BLOCK pointer 
    @param Source      The source INFO_BLOCK pointer
    @param SourceIdx   The source buffer index
    @param TargetIdx   The destination buffer index
**/
VOID
UpdateInfoBlocks (
    OUT	INFO_BLOCK  *Target,
    IN  INFO_BLOCK  *Source,
    IN  UINT32      SourceIdx,
    IN  UINT32      TargetIdx
)
{
	Target->Blocks[TargetIdx].StartAddress = Source->Blocks[SourceIdx].StartAddress;
	Target->Blocks[TargetIdx].BlockSize = Source->Blocks[SourceIdx].BlockSize;
	Target->Blocks[TargetIdx].Type = Source->Blocks[SourceIdx].Type;
}

/**
    Procedure to update the BLOCK_DESC

    @param Target      The destination INFO_BLOCK pointer 
    @param Source      The source INFO_BLOCK pointer
**/
VOID
MergeInfoBloksToUnfixedSize (
    OUT	INFO_BLOCK  *Target,
    IN  INFO_BLOCK  *Source
)
{
    UINT32      Counter = 0;
    UINT32      Base = 0;
    UINT32      Index;	
		
    for(Index = 1; Index < Source->TotalBlocks; Index++){
        if (Source->Blocks[Base].Type != Source->Blocks[Index].Type){
			UpdateInfoBlocks(Target, Source, Base, Counter);
            Counter++;
            Base = Index;
            // if the Base Block is the last one, then add to 
            if (Base == (Source->TotalBlocks-1)){
                UpdateInfoBlocks(Target, Source, Base, Counter);
                Counter++;
            }				
        }else{
            Source->Blocks[Base].BlockSize += Source->Blocks[Index].BlockSize;
            if (Index == (Source->TotalBlocks-1)){
                UpdateInfoBlocks(Target, Source, Base, Counter);
                Counter++;
            }
        }
    }

    // Update Header information of INFO_BLOCK
    MemCpy ( Target, Source, EFI_FIELD_OFFSET(INFO_BLOCK, Blocks));		
		
    // Update TotalBlocks, BiosRomSize, BaseBlockSize
    Target->TotalBlocks = Counter;
    Target->BiosRomSize = NUMBER_OF_BLOCKS * FLASH_BLOCK_SIZE;
    Target->BaseBlockSize = FLASH_BLOCK_SIZE;

    // Copy extra Flash infomation
    MemCpy ( &(Target->Blocks[Target->TotalBlocks]), 
		     &(Source->Blocks[Source->TotalBlocks]), 
		     ExtraFlashInfoBlockSize);
}
#endif
// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
/**
    Procedure to SmiFlash SMI handler.

    @param DispatchHandle   The unique handle assigned to this handler by SmiHandlerRegister().
    @param Context          Points to an optional handler context which was specified when the
                            handler was registered.
    @param CommBuffer       A pointer to a collection of data in memory that will
                            be conveyed from a non-SMM environment into an SMM environment.
    @param CommBufferSize   The size of the CommBuffer.

    @retval EFI_SUCCESS Command is handled successfully.
**/
#if PI_SPECIFICATION_VERSION >= 0x1000A
EFI_STATUS
EFIAPI
SMIFlashSMIHandler(
    IN      EFI_HANDLE              DispatchHandle,
    IN      CONST VOID              *Context OPTIONAL,
    IN  OUT VOID                    *CommBuffer OPTIONAL,
    IN  OUT UINTN                   *CommBufferSize OPTIONAL
)
#else
VOID SMIFlashSMIHandler(
    IN  EFI_HANDLE                  DispatchHandle,
    IN  EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
)
#endif
{

    EFI_SMM_CPU_SAVE_STATE  *pCpuSaveState = NULL;
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       Data;
    UINT64      pCommBuff;
// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
#if SMI_FLASH_INTERFACE_VERSION >= 14
    UINT64      OrgCommBuff = 0;	
    UINT32      OrgBufferSize;		
    UINT64      NewBuffer = 0;
    BOOLEAN     UseUnfixedSizeInfo = FALSE;	
#endif	
// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
    UINT32      HighBufferAddress = 0;
    UINT32      LowBufferAddress = 0;
    UINTN       Cpu = (UINTN)-1, i = 0;
#if PI_SPECIFICATION_VERSION < 0x1000A
    SW_SMI_CPU_TRIGGER      *SwSmiCpuTrigger;
#endif

#if PI_SPECIFICATION_VERSION >= 0x1000A

    if (CommBuffer != NULL && CommBufferSize != NULL) {
        Cpu = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
        Data = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->CommandPort;
    }
    //
    // Found Invalid CPU number, return 
    //
    if(Cpu == (UINTN)-1) RETURN(Status);

    Status = gSmmCpu->ReadSaveState ( gSmmCpu, \
                                      4, \
                                      EFI_SMM_SAVE_STATE_REGISTER_RBX, \
                                      Cpu, \
                                      &LowBufferAddress );
    Status = gSmmCpu->ReadSaveState ( gSmmCpu, \
                                      4, \
                                      EFI_SMM_SAVE_STATE_REGISTER_RCX, \
                                      Cpu, \
                                      &HighBufferAddress );

#else
    for (i = 0; i < pSmst->NumberOfTableEntries; ++i) {
        if (guidcmp(&pSmst->SmmConfigurationTable[i].VendorGuid,
                                    &gSwSmiCpuTriggerGuid) == 0) {
            break;
        }
    }

    //If found table, check for the CPU that caused the software Smi.
    if (i != pSmst->NumberOfTableEntries) {
        SwSmiCpuTrigger = pSmst->SmmConfigurationTable[i].VendorTable;
        Cpu = SwSmiCpuTrigger->Cpu;
    }

    //
    // Found Invalid CPU number, return 
    //
    if(Cpu == (UINTN) -1) RETURN(Status);

    Data    = (UINT8)DispatchContext->SwSmiInputValue;

    pCpuSaveState       = pSmst->CpuSaveState;
    HighBufferAddress   = pCpuSaveState[Cpu].Ia32SaveState.ECX;
    LowBufferAddress    = pCpuSaveState[Cpu].Ia32SaveState.EBX;
#endif    

    pCommBuff           = HighBufferAddress;
    pCommBuff           = Shl64(pCommBuff, 32);
    pCommBuff           += LowBufferAddress;
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 5 Data = 0x%x\n", Data));

    // Checking access rang for avoiding the privilege escalation into SMM 
    // via Smiflash driver.
    if(Data == SMIFLASH_GET_FLASH_INFO) {
//APTIOV_SERVER_OVERRIDE_Start: Fix added for SW SMI failure
        Status = CheckSmmCommunicationBuffer((UINT8*)pCommBuff, sizeof(INFO_BLOCK));
    } else Status = CheckSmmCommunicationBuffer((UINT8*)pCommBuff, sizeof(FUNC_BLOCK));
//APTIOV_SERVER_OVERRIDE_End: Fix added for SW SMI failure
    if(EFI_ERROR(Status)) RETURN(Status);

    // Initial Error code for blocking Flash Update from PreHandler eLink.
    if (Data != SMIFLASH_GET_FLASH_INFO) ((FUNC_BLOCK*)pCommBuff)->ErrorCode = 0;

    for (i = 0; SMIFlashPreHandler[i] != NULL; SMIFlashPreHandler[i++](Data, pCommBuff));
    // Process SmiFlash functions if GetFlashInfo call or No Error.
    if ((Data == SMIFLASH_GET_FLASH_INFO) || \
        (((FUNC_BLOCK*)pCommBuff)->ErrorCode == 0)) {

        switch (Data) {
            case 0x20:      // Enable Flash
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 6.0\n"));
                for (i = 0; SMIFlashPreUpdate[i] != NULL; SMIFlashPreUpdate[i++]()); 
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 6.1\n"));
                EnableFlashWrite((FUNC_BLOCK *)pCommBuff);
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 6.2\n"));
                break;

            case 0x24:      // Disable Flash
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 7.0\n"));
#if NVRAM_MIGRATION
			    if (DoNvramMigration) {
					AMI_NVRAM_UPDATE_PROTOCOL   *AmiNvramUpdate;
					DoNvramMigration = FALSE;
				    if (!EFI_ERROR(pSmst->SmmLocateProtocol (&AmiSmmNvramUpdateProtocolGuid, NULL, &AmiNvramUpdate))) {
						
						DEBUG((-1, " Do Nvram migration NvAddr= %x NvSize = %x NvBackupAddr = %x.\n", CurNvramAddress, CurNvramSize, CurNvramBackupAddress));
						
                        AmiNvramUpdate->MigrateNvram (AmiNvramUpdate, CurNvramAddress, CurNvramBackupAddress, CurNvramSize);
                    }
					
				}	
#endif				
                for (i = 0; SMIFlashEndUpdate[i] != NULL; SMIFlashEndUpdate[i++]());
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 7.1\n"));
                DisableFlashWrite((FUNC_BLOCK *)pCommBuff);
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 7.2\n"));
                break;

            case 0x22:      // Erase Flash
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 8.0\n"));
                EraseFlash((FUNC_BLOCK *)pCommBuff);
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 8.1\n"));
                break;

            case 0x21:      // Read Flash
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 9.0\n"));
                ReadFlash((FUNC_BLOCK *)pCommBuff);
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 9.1\n"));
                break;

            case 0x23:      // Write Flash
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 10.0\n"));
                WriteFlash((FUNC_BLOCK *)pCommBuff);
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 10.1\n"));
                break;

            case 0x25:      // Get Flash Info
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 11.0\n"));
// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
#if SMI_FLASH_INTERFACE_VERSION >= 14            
                //Checking see if we need to enlarge the buffer
				OrgBufferSize = ((INFO_BLOCK *)pCommBuff)->Length;				
                if (UseUnfixedSizeInfo = NeedPreEnlargeSyncBuffer((INFO_BLOCK *)pCommBuff, &NewBuffer)) {
					OrgCommBuff = pCommBuff;
			        pCommBuff = NewBuffer;
                }
#endif				
                GetFlashInfo((INFO_BLOCK *)pCommBuff);
                break;
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 11.0\n"));
// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
        }
    }
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 12.0\n"));
    for (i = 0; SMIFlashEndHandler[i] != NULL; SMIFlashEndHandler[i++](Data, pCommBuff));
DEBUG((DEBUG_INFO,"SMIFlashSMIHandler : 12.1\n"));
// APTIOV_SERVER_OVERRIDE_START : Support AFU flash when "FPGA_SUPPORT" token is enabled.
#if SMI_FLASH_INTERFACE_VERSION >= 14
    if (Data == SMIFLASH_GET_FLASH_INFO){
		if (UseUnfixedSizeInfo){
			//Merge the INFO_BLOCKs and convert it to new format
			MergeInfoBloksToUnfixedSize((INFO_BLOCK *)OrgCommBuff, (INFO_BLOCK *)pCommBuff);

			//Roll back the original buffer size before returnning back to the flash tool
			((INFO_BLOCK *)OrgCommBuff)->Length = OrgBufferSize;
			if (NewBuffer){
				pSmst->SmmFreePool((VOID *)NewBuffer);
				NewBuffer = 0;
				pCommBuff = OrgCommBuff;			
			}
		}		
		PatchFlashToolCompatibility((INFO_BLOCK *)pCommBuff);
    }
#endif
// APTIOV_SERVER_OVERRIDE_END : Support AFU flash when "FPGA_SUPPORT" token is enabled.
    RETURN(Status);
}

/**
    Procedure to prepare Rom Layout information for reporting Flash Infomatoin used.

    @retval EFI_SUCCESS Flash read successfully.
**/
EFI_STATUS
PrepareRomLayout(
    VOID
)
{
	EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
	EFI_STATUS  Status;
	UINT32      Authentication;
	UINTN       NumHandles;
	EFI_HANDLE  *HandleBuffer = NULL;
	UINT8       *Buffer = NULL;
	UINTN       BufferSize = 0;
	UINT8       *RomAreaBuffer = NULL;
	UINTN       RomAreaBufferSize;
	UINT8       i = 0, j = 0;

	Status = pBS->LocateHandleBuffer(
					ByProtocol,
					&gEfiFirmwareVolume2ProtocolGuid,
					NULL,
					&NumHandles,
					&HandleBuffer );
	if (EFI_ERROR(Status)) return Status;

	for ( i = 0; i < NumHandles; ++i )
	{
		Status = pBS->HandleProtocol(
						HandleBuffer[i],
						&gEfiFirmwareVolume2ProtocolGuid,
						&Fv );
		if (EFI_ERROR(Status)) continue;

        Status = Fv->ReadSection(Fv,
						&RomLayoutFfsGuid,
						EFI_SECTION_FREEFORM_SUBTYPE_GUID,
						0,
						(VOID**)&Buffer,
						&BufferSize,
						&Authentication );
		if ( Status == EFI_SUCCESS ) break;
	}

	RomAreaBuffer = Buffer + 0x10;
	RomAreaBufferSize = BufferSize - 0x10;
	NumberOfRomLayout = (RomAreaBufferSize / sizeof(ROM_AREA)) - 1;

	Status = pSmst->SmmAllocatePool (
						EfiRuntimeServicesData,
						NumberOfRomLayout * sizeof(ROM_AREA),
						(VOID**)&RomLayout );
	if (EFI_ERROR(Status)) return Status;
	MemCpy( RomLayout, RomAreaBuffer,
			NumberOfRomLayout * sizeof(ROM_AREA) );

	//Sorting
	for( i = 0; i < NumberOfRomLayout; i++ )
	{
		switch( RomLayout[i].Address  )
		{
			case FV_BB_BASE:
			case FV_MAIN_BASE:
			case NVRAM_ADDRESS:
#ifdef FAULT_TOLERANT_NVRAM_UPDATE
#if FAULT_TOLERANT_NVRAM_UPDATE
			case NVRAM_BACKUP_ADDRESS:
#endif
#endif
#if EC_FIRMWARE_UPDATE_INTERFACE_SUPPORT && SMI_FLASH_INTERFACE_VERSION > 10
			case AMI_EC_BASE:
#endif
				continue;
			default:
				break;
		}
		for( j = i + 1; j < NumberOfRomLayout; j++ )
		{
			if( RomLayout[j].Address < RomLayout[i].Address )
			{
				*(ROM_AREA*)RomAreaBuffer = RomLayout[j];
				RomLayout[j] = RomLayout[i];
				RomLayout[i] = *(ROM_AREA*)RomAreaBuffer;
			}
		}
	}

	//Prepare the FLASH_BLOCK_TYPE
	for( i = 0; i < NumberOfRomLayout; i++ )
	{
		switch( RomLayout[i].Address )
		{
			case FV_BB_BASE:
				RomLayout[i].Type = BOOT_BLOCK;
				break;
			case FV_MAIN_BASE:
				RomLayout[i].Type = MAIN_BLOCK;
				break;
			case NVRAM_ADDRESS:
#ifdef FAULT_TOLERANT_NVRAM_UPDATE
#if FAULT_TOLERANT_NVRAM_UPDATE
			case NVRAM_BACKUP_ADDRESS:
#endif
#endif
				RomLayout[i].Type = NV_BLOCK;
				break;
#if EC_FIRMWARE_UPDATE_INTERFACE_SUPPORT && SMI_FLASH_INTERFACE_VERSION > 10
			case AMI_EC_BASE:
				RomLayout[i].Type = EC_BLOCK;
				break;
#endif
			default:
				RomLayout[i].Type = NC_BLOCK + NumberOfNcb;
				NumberOfNcb++;
		}
	}

	pBS->FreePool(Buffer);
	pBS->FreePool(HandleBuffer);

	return Status;
}

/**
    In SMM Function for SmiFlash SMM driver.

    @param[in] ImageHandle  Image handle of this driver.
    @param[in] SystemTable  A Pointer to the EFI System Table.

    @retval EFI_SUCEESS     
    @return Others          Some error occurs.
**/
EFI_STATUS
InSmmFunction(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_HANDLE  Handle;
    UINTN       Index;
    EFI_HANDLE  DummyHandle = NULL;
    EFI_STATUS  Status;
#if PI_SPECIFICATION_VERSION >= 0x1000A
    EFI_SMM_SW_DISPATCH2_PROTOCOL    *pSwDispatch = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT      SwContext;

#else
    EFI_SMM_SW_DISPATCH_PROTOCOL    *pSwDispatch;
    EFI_SMM_SW_DISPATCH_CONTEXT     SwContext;
#endif
    EFI_SMM_ACCESS2_PROTOCOL        *SmmAccess2;
    UINTN                           Size;

#if PI_SPECIFICATION_VERSION >= 0x1000A

    Status = InitAmiSmmLib( ImageHandle, SystemTable );

    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &gSmmBase2);

    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmmBase->GetSmstLocation (gSmmBase2, &pSmst);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmst->SmmLocateProtocol( \
                        &gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmst->SmmLocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, &gSmmCpu);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

#else
    VERIFY_EFI_ERROR(pBS->LocateProtocol(
                          &gEfiSmmSwDispatchProtocolGuid, NULL, &pSwDispatch));
#endif

    Status = pBS->LocateProtocol( &gEfiSmmAccess2ProtocolGuid, NULL,
                                  (VOID**)&SmmAccess2 );
    if(EFI_ERROR(Status)) return EFI_SUCCESS;
    Size = 0;
    Status = SmmAccess2->GetCapabilities( SmmAccess2, &Size, mSmramRanges );
    if( Size == 0 ) return EFI_SUCCESS;
    Status = pSmst->SmmAllocatePool( EfiRuntimeServicesData, Size,
                                     (VOID**)&mSmramRanges );
    if(EFI_ERROR(Status)) return EFI_SUCCESS;
    Status = SmmAccess2->GetCapabilities( SmmAccess2, &Size, mSmramRanges );
    if(EFI_ERROR(Status)) return EFI_SUCCESS;
    mSmramRangeCount = Size / sizeof(EFI_SMRAM_DESCRIPTOR);

    VERIFY_EFI_ERROR(pBS->LocateProtocol(&gFlashSmmProtocolGuid,
                                                         NULL, &Flash));
    DEBUG((DEBUG_INFO,"SmiFlash: Flash Protocol %X\n",Flash));
//APTIOV_SERVER_OVERRIDE_Start: Fix added for SW SMI failure
    if (gSmmMailBox == NULL) {
        gSmmCommunicationBufferSize = SMM_COMMUNICATION_BUFFER_SIZE;
        Status = pBS->AllocatePages(
        AllocateAnyPages, EfiRuntimeServicesData, 
        EFI_SIZE_TO_PAGES(gSmmCommunicationBufferSize),
        &gSmmCommunicationBuffer
        );
        ASSERT_EFI_ERROR(Status);
        
        if (!EFI_ERROR(Status)) {
            Status = pBS->AllocatePool(
            EfiRuntimeServicesData, sizeof(AMI_SMM_COMMUNICATION_MAILBOX),
            (VOID **)&gSmmMailBox
            );
            ASSERT_EFI_ERROR(Status);
            
            if (EFI_ERROR(Status)){
                pBS->FreePages(
                    gSmmCommunicationBuffer,
                    EFI_SIZE_TO_PAGES(gSmmCommunicationBufferSize)
                );
                gSmmMailBox = NULL;
            } else {
                SwContext.SwSmiInputValue = SMM_COMMUNICATION_BUFFER_SWSMI;     
                Status  = pSwDispatch->Register(pSwDispatch, SmmCommunicationBufferSwDispatch,
                                                &SwContext, &Handle);
                ASSERT_EFI_ERROR(Status);
                if (EFI_ERROR(Status)){
                    pBS->FreePages(
                        gSmmCommunicationBuffer,
                        EFI_SIZE_TO_PAGES(gSmmCommunicationBufferSize)
                    );
                    gSmmCommunicationBuffer = 0;
                    
                    pBS->FreePool(gSmmMailBox);
                    gSmmMailBox = NULL;
                }
            }
        }
    }
//APTIOV_SERVER_OVERRIDE_End: Fix added for SW SMI failure
    for (Index = 0x20; Index < 0x26; Index++) {

        SwContext.SwSmiInputValue = Index;
        Status  = pSwDispatch->Register(pSwDispatch, SMIFlashSMIHandler,
                                                      &SwContext, &Handle);

        if (EFI_ERROR(Status)) return EFI_SUCCESS;

        //If any errors, unregister any registered SwSMI by this driver.
        //If error, and driver is unloaded, then a serious problem would exist.
    }

#if PI_SPECIFICATION_VERSION >= 0x1000A
    Status = pSmst->SmmInstallProtocolInterface(
                 &DummyHandle,
                 &gEfiSmiFlashProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &SmiFlash
             );
#else
    Status = pBS->InstallMultipleProtocolInterfaces(
                 &DummyHandle,
                 &gEfiSmiFlashProtocolGuid,&SmiFlash,
                 NULL
             );
#endif
    ASSERT_EFI_ERROR(Status);

	Status = PrepareRomLayout();
	ASSERT_EFI_ERROR(Status);

	GetCurrentNvramInfo();
	
	DEBUG((-1, " NVRAM Info!! Nvaddr = %x size = %x NvBackupAddr = %x.\n", CurNvramAddress, CurNvramSize, CurNvramBackupAddress));
	
    for (Index = 0; SMIFlashInSmm[Index] != NULL; SMIFlashInSmm[Index++]());
    return EFI_SUCCESS;
}

/**
    Entry Point for SmiFlash SMM driver.

    @param[in] ImageHandle  Image handle of this driver.
    @param[in] SystemTable  A Pointer to the EFI System Table.

    @retval EFI_SUCEESS     
    @return Others          Some error occurs.
**/
EFI_STATUS
EFIAPI
SMIFlashDriverEntryPoint(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    InitAmiLib(ImageHandle, SystemTable);
#if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
    {
      UINTN i = 0;
      for (i = 0; SMIFlashOemFuncHook[i] != NULL; SMIFlashOemFuncHook[i++](ImageHandle, SystemTable));
    }
#endif //#if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
    return InitSmmHandler (ImageHandle, \
                            SystemTable, InSmmFunction, NULL);
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
