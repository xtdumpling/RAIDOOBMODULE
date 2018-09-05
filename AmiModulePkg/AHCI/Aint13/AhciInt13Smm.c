//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AhciInt13Smm.c
    This file contains code for SMI handler for AHCI INT13

**/

//---------------------------------------------------------------------------

#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/AmiAhciBus.h>
#include <Protocol/AhciSmmProtocol.h>
#include <Protocol/LegacyBios.h>
#include <Library/AmiBufferValidationLib.h>
#include "AhciInt13Smm.h"

// Naming conventions made to support AMI_COMPATIBILITY_PKG_VERSION version > 21
#if defined(AMI_COMPATIBILITY_PKG_VERSION) && (AMI_COMPATIBILITY_PKG_VERSION>=21)
  #define InitAmiSmmLibPi InitAmiSmmLib
  #define GetSmstConfigurationTablePi GetSmstConfigurationTable
  #define pSmmBasePi pSmmBase
  #define pSmstPi    pSmst
#endif


EFI_SMM_CPU_PROTOCOL        *gSmmCpuProtocol = NULL;
AMI_AHCI_BUS_SMM_PROTOCOL   *gAhciBusSmmProtocol = NULL;
DLIST                       gDriveInfoList;
EFI_GUID                    gAint13SmmDataGuid = AHCI_INT13_SMM_DATA_GUID;
EFI_GUID                    gAhciSmmProtocolGuid = AMI_AHCI_SMM_PROTOCOL_GUID;
UINT64                      PciExpressBaseAddress = 0;
UINT8                       *gBuffer = NULL;

//---------------------------------------------------------------------------

/**
        Check if input command is a LBA48 command

        @param    Command - AHCI command

        @retval TRUE  - LBA48 command
        @retval FALSE - Not a LBA48 command

**/
BOOLEAN
Is48BitCommand (
    IN UINT8                        Command
 )
{
    if ( Command == READ_SECTORS_EXT ||
         Command == READ_MULTIPLE_EXT ||
         Command == WRITE_SECTORS_EXT ||
         Command == WRITE_MULTIPLE_EXT ||
         Command == READ_DMA_EXT    ||
         Command == WRITE_DMA_EXT )
        return TRUE;
    else
        return FALSE;
}

/**
    Check if input command is a DMA command

    @param    Command - AHCI command

    @retval TRUE  - DMA command
            FALSE - Not a DMA command

**/
BOOLEAN
IsDmaCommand (
    IN UINT8                        Command
 )
{
    if ( Command == READ_DMA ||
         Command == READ_DMA_EXT ||
         Command == WRITE_DMA ||
         Command == WRITE_DMA_EXT )
        return TRUE;
    else
        return FALSE;
}

/**
    It maps EFI_STATUS code to corresponding INT13 error code

    @param    Status - EFI_STATUS code

    @retval UINT8 - INT13 error code

**/
UINT8
CheckErrorCode (
    IN  EFI_STATUS  Status
 )
{
    switch(Status){
        case EFI_SUCCESS:
            return  0x0;        // successful completion
            break;
        case EFI_INVALID_PARAMETER:
            return  0x01;       // invalid function in AH or invalid parameter
            break;
        case EFI_UNSUPPORTED:
            return  0x01;       // invalid function in AH or invalid parameter
            break;
        case EFI_NOT_READY:
            return  0xAA;       // drive not ready (hard disk)
            break;
        case EFI_DEVICE_ERROR:
            return  0xE0;       // status register error (hard disk)
            break;
        case EFI_WRITE_PROTECTED:
            return  0x03;       // disk write-protected
            break;
        case EFI_NO_MEDIA:
            return  0x31;       // no media in drive (IBM/MS INT 13 extensions)
            break;
        case EFI_MEDIA_CHANGED:
            return  0x06;       // disk changed
            break;
        case EFI_NOT_FOUND:
            return  0x01;       // invalid function in AH or invalid parameter
            break;
        case EFI_ACCESS_DENIED:
            return  0xB6;       // volume present but read protected (INT 13 extensions)
            break;
        case EFI_TIMEOUT:
            return  0x80;       // timeout (not ready)
            break;
        case EFI_ABORTED:
            return  0xBB;       // undefined error (hard disk)
            break;
        default:
            break;
    }
    return  0xBB;
}

/**
    It returns the drive information corresponding to input drive number, if found.

    @param    DriveNum  - INT13 drive number
    @param    DriveInfo - Filled with corresponding drive information

    @retval EFI_SUCCESS     - DriveInfo is valid
            EFI_UNSUPPORTED - Can't find the corresponding data.

**/
EFI_STATUS
GetDriveInfoByDriveNum(
    IN      UINT8       DriveNum,
    IN  OUT VOID        **DriveInfo
)
{
    DLINK                  *DriveInfoLink = NULL;
    SMM_AINT13_DRIVE_INFO  *pDriveInfo = NULL;

    // Look for drive information corresponding to DriveNum in gDriveInfoList
    DriveInfoLink = gDriveInfoList.pHead;
    for(;DriveInfoLink;DriveInfoLink=DriveInfoLink->pNext){
        pDriveInfo = OUTTER(DriveInfoLink, dLink, SMM_AINT13_DRIVE_INFO);
        if(DriveNum == pDriveInfo->DriveNum) {
            // Return the information if found, also set status as success
            *DriveInfo = pDriveInfo;
            return  EFI_SUCCESS;
        }
    }

    // No drive information corresponding to DriveNum in gDriveInfoList
    return  EFI_UNSUPPORTED;
}

/**
    Worker function to service AHCI INT13 request. Currently it supports
    Read/Write function only.
    Operation:
    1. Parse information passed as IA registers to parameter required
       by AMI_AHCI_BUS_SMM_PROTOCOL APIs.
    2. Call appropriate AMI_AHCI_BUS_SMM_PROTOCOL API.

    @param    ExRegs - Pointer of EFI_IA32_REGISTER_SET

    @retval EFI_SUCCESS     - Int13 request complete
            EFI_UNSUPPORTED - This Int13 request is unsupported

**/
EFI_STATUS
ProcessInt13Function(
    IN  EFI_IA32_REGISTER_SET      *ExRegs
)
{
    EFI_STATUS  Status;
    BOOLEAN     IsSupported = TRUE;
    VOID        *Buffer = NULL;
    UINT8       *BufferBackup = NULL;
    UINT32      ByteCount = 0;
    UINT32      TotalByteCount=0;
    UINT16      SectorCount = 0;
    UINT8       Command = 0;
    EFI_LBA     Lba = 0;
    UINT16      SkipBytesBefore = 0, SkipBytesAfter = 0;
    UINT16      Header = 0, Cylinder = 0, Sector = 0;
    UINT8       ReadWrite = 0;      // 0 : read, 1: write
    UINT8       *UserBuf = NULL;
    UINT32      bAhciBaseAddress = 0;
    UINTN       i = 0;
    UINT16      BlksPerTransfer;
    VOID        *AhciBuffer;
    BOOLEAN     UnalignedTransfer = FALSE;
    DISK_ADDRESS_PACKAGE  *Package = NULL;
    SMM_AINT13_DRIVE_INFO *pDriveInfo = NULL;
    COMMAND_STRUCTURE     CommandStructure;

    // Get drive information based on drive number
    Status = GetDriveInfoByDriveNum(ExRegs->H.DL,&pDriveInfo);

    if(!EFI_ERROR(Status)){
        if(pDriveInfo->DeviceType == ATA) {
            // Calculate AHCI parameter to be filled in COMMAND_STRUCTURE based on INT13 function
            if(ExRegs->H.AH == READ_SECTOR || ExRegs->H.AH == EXT_READ){
                Command = pDriveInfo->RCommand;
                ReadWrite = 0;  // read
            }else if(ExRegs->H.AH == WRITE_SECTOR || ExRegs->H.AH == EXT_WRITE){
                Command = pDriveInfo->WCommand;
                ReadWrite = 1;  // write
            }
            switch(ExRegs->H.AH){
                case READ_SECTOR:
                case WRITE_SECTOR:
                    Cylinder = ((UINT16)(ExRegs->H.CL & 0xC0 ) << 2) +ExRegs->H.CH; // cylinder: bit 6-7(CL) + CH
                    Header = (UINT16)ExRegs->H.DH;                                  // header  : DH
                    Sector = (UINT16)(ExRegs->H.CL & 0x3F);                         // sector  : bit 0-5(CL)
                    Lba = (Cylinder*(pDriveInfo->bMAXHN) + Header) * (pDriveInfo->bMAXSN) + Sector - 1;
                    SectorCount = ExRegs->H.AL;
                    Buffer = (VOID*)(((ExRegs->X.ES) << 4 ) + ExRegs->X.BX);
                    TotalByteCount = SectorCount*HDD_BLOCK_SIZE;
                    break;
                case EXT_READ:
                case EXT_WRITE:
                    Package = (DISK_ADDRESS_PACKAGE*)(((ExRegs->X.DS) << 4 ) + ExRegs->X.SI);
                    Lba = Package->StartLba;
                    SectorCount = Package->XferSector;
                    Buffer = (VOID*)(((Package->Buffer >> 16 & 0xFFFF) << 4) + (UINT16)Package->Buffer);
                    TotalByteCount = SectorCount*HDD_BLOCK_SIZE;
                    break;
                default:
                    IsSupported = FALSE;
                    break;
            }
        } else if(pDriveInfo->DeviceType == ATAPI){ // Only read command support is required
            Command = pDriveInfo->RCommand;
            ReadWrite = 0;  // read
            Buffer = (VOID*)((((ExRegs->E.EDI) >> 16 & 0xFFFF) << 4) + (UINT16)(ExRegs->E.EDI));
            Lba = ExRegs->E.EAX;
            SectorCount = ExRegs->X.CX; // CX
            SkipBytesAfter = ((ExRegs->E.ECX) >> 24) * 512; // CH+ (Higher byte of higher word of ECX)
            SkipBytesBefore = (((ExRegs->E.ECX) >> 16) & 0xFF) * 512; // CL+ (Lower byte of higher word of ECX)
            TotalByteCount = SectorCount * pDriveInfo->BlockSize; // 2048
            if(SkipBytesBefore || SkipBytesAfter) {
                Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, sizeof(UINT8)*(SkipBytesBefore + SkipBytesAfter), &BufferBackup);
                if (EFI_ERROR(Status)) {
                    ASSERT(TRUE);
                    IsSupported = FALSE;
                } else {
                    // Backup bytes to be preserved.
                    for(i = 0; i < (UINTN)(SkipBytesBefore + SkipBytesAfter); i++) {
                        BufferBackup[i] = *(((UINT8*)Buffer)+i + (TotalByteCount - SkipBytesBefore - SkipBytesAfter));
                    }
                }
            }
        } else {
            IsSupported = FALSE;
        }
    } // if(!EFI_ERROR(Status))
    else {
        IsSupported = FALSE;
    }

    if(IsSupported){
        // Validate Buffer is valid address and not reside in SMRAM region
        Status = AmiValidateMemoryBuffer(Buffer, (UINTN)TotalByteCount);
        if( EFI_ERROR(Status) ) {
            ExRegs->X.Flags.CF = 0x1;                // set on error
            ExRegs->H.AH = CheckErrorCode(EFI_INVALID_PARAMETER);   // return error code
            return EFI_SUCCESS;
        }

        // Backup AHCI base address from gAhciBusSmmProtocol
        bAhciBaseAddress = gAhciBusSmmProtocol->AhciBaseAddress;

        // Save current AHCI base address from AHCI controller.
        gAhciBusSmmProtocol->AhciBaseAddress = *(UINT32*)PCI_CFG_ADDR(pDriveInfo->BusNo, pDriveInfo->DevNo, pDriveInfo->FuncNo, PCI_ABAR);

        BlksPerTransfer =  SectorCount;
        ByteCount = TotalByteCount;
        AhciBuffer = Buffer;

        //If Buffer isn't aligned use internal buffer
        if(((UINT32)Buffer) & 0x1) {
            BlksPerTransfer = 1;
            AhciBuffer = gBuffer;
            UnalignedTransfer = TRUE;

            if(pDriveInfo->DeviceType == ATA) {
                ByteCount = BlksPerTransfer * HDD_BLOCK_SIZE;
            } else if(pDriveInfo->DeviceType == ATAPI){
                ByteCount = BlksPerTransfer * pDriveInfo->BlockSize;
            }
        }

        UserBuf = (UINT8*)Buffer;

        for (  ;  SectorCount; SectorCount -= BlksPerTransfer){

            if  (ReadWrite == 1 && UnalignedTransfer) {
                for(i = 0; i < ByteCount; i++) {
                    *(((UINT8*)AhciBuffer)+i) = *(((UINT8*)Buffer)+i);
                }
            }

            // clear Command structure
            MemSet (&CommandStructure, sizeof(COMMAND_STRUCTURE), 0);

            // Fill CommandStructure buffer.
            CommandStructure.Buffer = AhciBuffer;
            CommandStructure.ByteCount = ByteCount;
            if(pDriveInfo->DeviceType == ATA) { // ATA
                CommandStructure.Features = 0;
                CommandStructure.FeaturesExp = 0;
                CommandStructure.SectorCount = BlksPerTransfer;
                CommandStructure.LBALow = (UINT8)Lba;
                CommandStructure.LBAMid = (UINT8) (((UINT32)Lba >>8) & 0xff);
                CommandStructure.LBAHigh = (UINT8) (((UINT32)Lba >>16) & 0xff);
                if(Is48BitCommand(Command)){  // if support LBA48 feature?
                    CommandStructure.LBALowExp = (UINT8) (UINT8)Shr64(Lba,24);
                    CommandStructure.LBAMidExp = (UINT8) (UINT8)Shr64(Lba,32);
                    CommandStructure.LBAHighExp = (UINT8) (UINT8)Shr64(Lba,40);
                    CommandStructure.Device = 0x40;        // LBA48
                }else{
                    CommandStructure.Device = ((UINT8)Shr64(Lba,24) & 0x0f) | 0x40;    // LBA28
                }
                CommandStructure.Command = Command;
                CommandStructure.Control = 0;
            } else if(pDriveInfo->DeviceType == ATAPI) {
                CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = Command;
                CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = pDriveInfo->Lun << 5;
                CommandStructure.AtapiCmd.Ahci_Atapi_Command[2] = (UINT8)(((UINT32) Lba) >>  24);
                CommandStructure.AtapiCmd.Ahci_Atapi_Command[3] = (UINT8)(((UINT32) Lba) >> 16);
                CommandStructure.AtapiCmd.Ahci_Atapi_Command[4] = (UINT8)(((UINT16) Lba) >> 8);
                CommandStructure.AtapiCmd.Ahci_Atapi_Command[5] = (UINT8)(((UINT8) Lba) & 0xff);
                CommandStructure.AtapiCmd.Ahci_Atapi_Command[7] = (UINT8) (BlksPerTransfer >> 8);        // MSB
                CommandStructure.AtapiCmd.Ahci_Atapi_Command[8] = (UINT8) (BlksPerTransfer & 0xff);      // LSB
            }

            // Send ATA/ATAPI command in AHCI mode
            if(pDriveInfo->DeviceType == ATA) { // ATA
                if(IsDmaCommand(Command)) {
                    Status = gAhciBusSmmProtocol->AhciSmmExecuteDmaDataCommand( gAhciBusSmmProtocol,
                                                &CommandStructure,
                                                pDriveInfo->PortNum,
                                                pDriveInfo->PMPortNum,
                                                pDriveInfo->DeviceType,
                                                ReadWrite);
                } else {
                    Status = gAhciBusSmmProtocol->AhciSmmExecutePioDataCommand( gAhciBusSmmProtocol,
                                                &CommandStructure,
                                                pDriveInfo->PortNum,
                                                pDriveInfo->PMPortNum,
                                                pDriveInfo->DeviceType,
                                                ReadWrite);
                }
            } else { // ATAPI
                Status = gAhciBusSmmProtocol->AhciSmmExecutePacketCommand( gAhciBusSmmProtocol,
                                                &CommandStructure,
                                                ReadWrite,
                                                pDriveInfo->PortNum,
                                                pDriveInfo->PMPortNum,
                                                pDriveInfo->DeviceType);
            }

            if (EFI_ERROR(Status)) {
                break;
            }

            if  (ReadWrite == 0 && UnalignedTransfer) {
                for(i = 0; i < ByteCount; i++) {
                    *(((UINT8*)Buffer)+i) = *(((UINT8*)AhciBuffer)+i);
                }
            }

            (UINTN)Buffer = (UINTN)Buffer + ByteCount;
            Lba += BlksPerTransfer;

        }

        Buffer = UserBuf;

        // Restore base address to gAhciBusSmmProtocol
        gAhciBusSmmProtocol->AhciBaseAddress = bAhciBaseAddress;

        if(pDriveInfo->DeviceType == ATAPI){
            // fill output buffer with requested data only.
            if(SkipBytesBefore || SkipBytesAfter) {
                UserBuf = (UINT8*)Buffer;
                // Move requested data at start of the buffer
                if(SkipBytesBefore != 0)
                    for(i = 0; i < (UINTN)(TotalByteCount - SkipBytesBefore - SkipBytesAfter); i++) {
                        UserBuf[i] = UserBuf[i+SkipBytesBefore];
                    }
                // Keep rest of the buffer intact. Restore the backup.
                for(i = 0; i < (UINTN)(SkipBytesBefore + SkipBytesAfter); i++) {
                    UserBuf[i + (TotalByteCount - SkipBytesBefore - SkipBytesAfter)] = BufferBackup[i];
                }
                pSmst->SmmFreePool(BufferBackup);
            }
        }

        // update return register data whatever success or error!!
        if(!EFI_ERROR(Status)){
            // AHCI success
            ExRegs->X.Flags.CF = 0x0;               // clear if successful
            ExRegs->H.AH = CheckErrorCode(Status);  // successful completion
        }
        else{
            // AHCI error
            ExRegs->X.Flags.CF = 0x1;                // set on error
            ExRegs->H.AH = CheckErrorCode(Status);   // return error code
        }
    }

    // return EFI_SUCCESS: Int13 request is complete.
    // return EFI_UNSUPPORTED: This function isn't supported by this routine.
    return (IsSupported)? EFI_SUCCESS : EFI_UNSUPPORTED;
}

/**
    This is the SWSMI handler to service AHCI INT13 request.
    Operation:
    1. Take INT13 parameters stored on real mode stack from CPU save state.
    2. Call a sub-function to process INT13 request.
    3. Update output parameters (IA registers) on real mode stack.

    @param    CpuIndex  - Index of CPU which triggered SW SMI

    @retval EFI_STATUS

**/

EFI_STATUS
AhciInt13SmiHandler (
    IN UINTN       CpuIndex
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT16      StackSegment = 0;
    UINT16      StackOffset = 0;
    EFI_IA32_REGISTER_SET  ExRegs;
    INT13_TO_SMI_EXREGS    *Int13ToSmiExRegs = NULL;

    // Read SS/ESP from CPU save state
    gSmmCpuProtocol->ReadSaveState ( gSmmCpuProtocol,
                             2,
                             EFI_SMM_SAVE_STATE_REGISTER_RSP,
                             CpuIndex,
                             &StackOffset );

    gSmmCpuProtocol->ReadSaveState ( gSmmCpuProtocol,
                             2,
                             EFI_SMM_SAVE_STATE_REGISTER_SS,
                             CpuIndex,
                             &StackSegment );

    // Get base address of real mode stack
    Int13ToSmiExRegs = (INT13_TO_SMI_EXREGS*)(((StackSegment << 4) + StackOffset) + 2);

    MemSet (&ExRegs, sizeof(EFI_IA32_REGISTER_SET), 0);

    // Initialize the SMM THUNK registers
    ExRegs.E.EAX = Int13ToSmiExRegs->StackEAX;
    ExRegs.E.EBX = Int13ToSmiExRegs->StackEBX;
    ExRegs.E.ECX = Int13ToSmiExRegs->StackECX;
    ExRegs.E.EDX = Int13ToSmiExRegs->StackEDX;
    ExRegs.E.EDI = Int13ToSmiExRegs->StackEDI;
    ExRegs.E.ESI = Int13ToSmiExRegs->StackESI;
    ExRegs.E.EBP = Int13ToSmiExRegs->StackEBP;
    ExRegs.E.DS = Int13ToSmiExRegs->StackDS;
    ExRegs.E.ES = Int13ToSmiExRegs->StackES;
    ExRegs.E.FS = Int13ToSmiExRegs->StackFS;
    ExRegs.E.GS = Int13ToSmiExRegs->StackGS;
    ExRegs.X.Flags = Int13ToSmiExRegs->StackFlags;

    if(gAhciBusSmmProtocol && gDriveInfoList.pHead){
        // Execute Int13 function by AhciSmmProtocol and update ExRegs for return caller.
        // Note: Function will return non-EFI_SUCCESS value if Int13 function isn't
        // supported by ProcessInt13Function().
        Status = ProcessInt13Function(&ExRegs);
    }

    // Update the registers before go back caller.
    Int13ToSmiExRegs->StackEAX = ExRegs.E.EAX;
    Int13ToSmiExRegs->StackEBX = ExRegs.E.EBX;
    Int13ToSmiExRegs->StackECX = ExRegs.E.ECX;
    Int13ToSmiExRegs->StackEDX = ExRegs.E.EDX;
    Int13ToSmiExRegs->StackEDI = ExRegs.E.EDI;
    Int13ToSmiExRegs->StackESI = ExRegs.E.ESI;
    Int13ToSmiExRegs->StackEBP = ExRegs.E.EBP;
    Int13ToSmiExRegs->StackDS = ExRegs.E.DS;
    Int13ToSmiExRegs->StackES =  ExRegs.E.ES;
    Int13ToSmiExRegs->StackFS =  ExRegs.E.FS;
    Int13ToSmiExRegs->StackGS =  ExRegs.E.GS;
    Int13ToSmiExRegs->StackFlags = ExRegs.X.Flags;

    return Status;
}

/**
    Read from AHCI MMIO address

    @param AhciBaseAddress - AHCI MMIO address
    @param UINT32* - Pointer to value read from AHCI MMIO address

    @retval EFI_STATUS

**/

EFI_STATUS
AhciMmioRead (
    IN  UINT32  AhciMmioAddress,
	OUT UINT32  *ReadValue
)
{
    EFI_STATUS    Status;

    // Validate AhciMmioAddress is valid MMIO address and not reside in SMRAM region
    Status = AmiValidateMmioBuffer( (VOID*)AhciMmioAddress, 4 );
    if( EFI_ERROR(Status) ) {
         return Status;
    }

    *ReadValue = *(UINT32*)(AhciMmioAddress);
    return Status;
}

/**
    Write to the AHCI MMIO Address

    @param AhciMmioAddress - AHCI MMIO address
    @param WriteValue - Value to be written

    @retval EFI_STATUS - EFI_NOT_FOUND: Invalid address, EFI_SUCCESS: Success


**/

EFI_STATUS
AhciMmioWrite (
    IN  UINT32  AhciMmioAddress,
    IN  UINT32  WriteValue
)
{
    EFI_STATUS    Status;

    // Validate AhciMmioAddress is valid MMIO address and not reside in SMRAM region
    Status = AmiValidateMmioBuffer( (VOID*)AhciMmioAddress, 4 );
    if( EFI_ERROR(Status) ) {
        return Status;
    }

    *(UINT32*)(AhciMmioAddress) = WriteValue;
    return Status;
}

/**
    SMI handler for the AHCI_MMIO_SWSMI SW SMI

    @param    CpuIndex  - Index of CPU which triggered SW SMI
    @param    FunctionNo - 1/2: Read or Write MMIO operation

    @retval

**/

EFI_STATUS
AhciMmioSmiHandler (
    IN     UINTN     CpuIndex,
    IN     UINT32    FunctionNo
)
{
    EFI_STATUS  Status;  
    UINT32      AhciMmioAddress;
    UINT32      WriteValue;
    UINT32      ReadValue;

    gSmmCpuProtocol->ReadSaveState (gSmmCpuProtocol,
                                    4,
                                    EFI_SMM_SAVE_STATE_REGISTER_RSI,
                                    CpuIndex,
                                    &AhciMmioAddress );

    if(FunctionNo == 1) {
        Status = AhciMmioRead(AhciMmioAddress, &ReadValue);

        if( !EFI_ERROR( Status ) ) {
        gSmmCpuProtocol->WriteSaveState(gSmmCpuProtocol,
                                        4,
                                        EFI_SMM_SAVE_STATE_REGISTER_RAX,
                                        CpuIndex,
                                        &ReadValue);

        FunctionNo = 0; // Update success
        }
        gSmmCpuProtocol->WriteSaveState(gSmmCpuProtocol,
                                        4,
                                        EFI_SMM_SAVE_STATE_REGISTER_RCX,
                                        CpuIndex,
                                        &FunctionNo);

    } else if(FunctionNo == 2) {

        gSmmCpuProtocol->ReadSaveState (gSmmCpuProtocol,
                                        4,
                                        EFI_SMM_SAVE_STATE_REGISTER_RBX,
                                        CpuIndex,
                                        &WriteValue );

        Status = AhciMmioWrite(AhciMmioAddress, WriteValue);

        if( !EFI_ERROR( Status ) ) {
            FunctionNo = 0; // Update success
        }
        gSmmCpuProtocol->WriteSaveState(gSmmCpuProtocol,
                                        4,
                                        EFI_SMM_SAVE_STATE_REGISTER_RCX,
                                        CpuIndex,
                                        &FunctionNo);
    }

    return EFI_SUCCESS;
}

/**
    Common SMI handler for AHCI INT13 SMIs

    @param    DispatchHandle  - EFI Handle
    @param    DispatchContext - Pointer to the EFI_SMM_SW_REGISTER_CONTEXT
    @param    CommBuffer      - Pointer to Communication data
    @param    CommBufferSize  - Pointer to size of Communication data

    @retval EFI_STATUS

**/
EFI_STATUS
AhciCommonSmmHandler (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINTN       CpuIndex = (UINTN)-1;
    UINT32      FunctionNo = 0;

    // Get CPU number of CPU which generated this SWSMI
    if (CommBuffer != NULL && CommBufferSize != NULL) {
        CpuIndex = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
    }

    // Return if CPU number is invalid
    if(CpuIndex == (UINTN)-1) {
        return EFI_NOT_FOUND;
    }

    // Read ECX from CPU save state
    gSmmCpuProtocol->ReadSaveState ( gSmmCpuProtocol,
                                     4,
                                     EFI_SMM_SAVE_STATE_REGISTER_RCX,
                                     CpuIndex,
                                     &FunctionNo );

    switch(FunctionNo) {
        case 0x1:
        case 0x2:
            Status = AhciMmioSmiHandler(CpuIndex, FunctionNo);
            break;

        case 0x3:
            Status = AhciInt13SmiHandler(CpuIndex);
            break;

        default:
            // Invalid Function. Return Error.
            FunctionNo = 0xFF;
            gSmmCpuProtocol->WriteSaveState(gSmmCpuProtocol,
                                            4,
                                            EFI_SMM_SAVE_STATE_REGISTER_RCX,
                                            CpuIndex,
                                            &FunctionNo);
            Status = EFI_UNSUPPORTED;
            break;
    }

    return Status;
}

/**
    Saves AINT13 information passed from Non-SMM mode using
    EFI_SMM_COMMUNICATION_PROTOCOL API. This is required for below reason:
     1)AhciSmmProtocol use port number, but Int13 service uses drive number.
     2)AhciSmmProtocol use LBA addressing on HDD, but Int13 Read/Write function
      uses Cylinder, Header and Sector addressing on HDD.

    @param    DispatchHandle  - EFI Handle
    @param    DispatchContext - Pointer to the EFI_SMM_SW_REGISTER_CONTEXT
    @param    CommBuffer      - Pointer to Communication data
    @param    CommBufferSize  - Pointer to size of Communication data

    @retval EFI_STATUS

**/
EFI_STATUS
GetAhciInt13SmmData (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL
)
{
    EFI_STATUS  Status;
    UINTN       i = 0, j = 0;
    AHCI_INT13_SMM_DATA      *AhciInt13SmmData = NULL;
    SMM_AINT13_DRIVE_INFO    *pDriveInfo = NULL;
    SMM_AINT13_DRIVE_INFO    *pSmmDriveInfo = NULL;

    // Confirm that communication buffer contains required data
    AhciInt13SmmData = (AHCI_INT13_SMM_DATA *)CommBuffer;
    if (!AhciInt13SmmData || AhciInt13SmmData->DriveCount == 0) {
        return EFI_SUCCESS;
    }

    // Save all information from AhciInt13SmmData to gDriveInfoList
    for(j=0;j<AhciInt13SmmData->DriveCount;j++){

        // Allocate SMM memory
        Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, sizeof(SMM_AINT13_DRIVE_INFO), &pSmmDriveInfo);
        if (EFI_ERROR(Status)) {
            continue;
        }
        // Copy all data
        pDriveInfo = &(AhciInt13SmmData->DriveInfo[j]);
        for(i=0;i<sizeof(SMM_AINT13_DRIVE_INFO);++i){
            *((UINT8*)pSmmDriveInfo + i) = *((UINT8*)pDriveInfo + i);
        }

        // Add data to list
        DListAdd(&gDriveInfoList, &pSmmDriveInfo->dLink);
    }

    // Locate AMI_AHCI_BUS_SMM_PROTOCOL
    if(gAhciBusSmmProtocol == NULL) {
        gAhciBusSmmProtocol = (AMI_AHCI_BUS_SMM_PROTOCOL*) GetSmstConfigurationTablePi(&gAhciSmmProtocolGuid);
    }

    return EFI_SUCCESS;
}

/**
    Driver entry point function. It does following tasks:
    1. Initializes global variables (gDriveInfoList, PciExpressBaseAddress etc.)
    2. Register SMI handler to get information passed through SmmCommunicationProtocol API.
    3. Register SW SMI handler to process AHCI INT13 requests.
    4. Locate EFI_SMM_CPU_PROTOCOL for Read/Write from/to CPU save state

    @param    EFI_HANDLE           ImageHandle,
    @param    EFI_SYSTEM_TABLE     *SystemTable

    @retval    EFI_STATUS

**/
EFI_STATUS
AhciInt13SmmEntry (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS                    Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch2;
    EFI_SMM_SW_REGISTER_CONTEXT   AhciInt13SwSmiContext = {AHCI_INT13_SMM_SWSMI_VALUE};
    EFI_HANDLE                    AhciInt13SmmHandle;
    EFI_HANDLE                    AhciInt13SmmDataHandle;

    InitAmiSmmLibPi(ImageHandle, SystemTable);

    // Initialize global drive info list
    DListInit(&gDriveInfoList);

    // Get the PCI Express Base Address from the PCD
    PciExpressBaseAddress = PcdGet64 (PcdPciExpressBaseAddress);

    // Return error if PSmstPi is NULL
    if(pSmmBasePi == NULL || pSmstPi == NULL) {
        ASSERT(TRUE);
        return EFI_NOT_FOUND;
    }

    // Register SMI handler to save AHCI_INT13_SMM_DATA passed from DXE through SmmCommunicationProtocol
    Status = pSmstPi->SmiHandlerRegister(
                                        (VOID *)GetAhciInt13SmmData,
                                        &gAint13SmmDataGuid,
                                        &AhciInt13SmmDataHandle
                                        );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    // Locate EFI_SMM_SW_DISPATCH2_PROTOCOL
    Status = pSmstPi->SmmLocateProtocol(
                                        &gEfiSmmSwDispatch2ProtocolGuid,
                                        NULL,
                                        &SwDispatch2
                                        );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    // Register SMI handler to handle AHCI INT13 operations
    Status = SwDispatch2->Register(
                                  SwDispatch2,
                                  AhciCommonSmmHandler,
                                  &AhciInt13SwSmiContext,
                                  &AhciInt13SmmHandle
                                  );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    // Locate EFI_SMM_CPU_PROTOCOL for Read/Write from/to CPU save state
    Status = pSmstPi->SmmLocateProtocol(
                                        &gEfiSmmCpuProtocolGuid,
                                        NULL,
                                        &gSmmCpuProtocol
                                        );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    // Use this buffer for unaligned read or write
    Status = pBS->AllocatePages (
                    AllocateAnyPages,
                    EfiReservedMemoryType,
                    EFI_SIZE_TO_PAGES(2048),  // 512 for ATA and 2048 for ATAPI, so taking 2048
                    (EFI_PHYSICAL_ADDRESS*)&(gBuffer));
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
