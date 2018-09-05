//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
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
  Flash update routines file.
**/

//----------------------------------------------------------------------------
// Includes
#include <Efi.h>
#include <AmiDxeLib.h>
#include <Library/TimerLib.h>
#include "FlashPart.h"
#include "SpiFlash.h"
#include "Token.h"
//----------------------------------------------------------------------------
// Local defines for transaction types
// SPI Register Configuration 
typedef struct {
    UINT32  BusDevFuncRegr; // SPBAR + 04h: Hardware Sequencing Flash Status Register (16bits)
    UINT8   HwSeqStsCtl;   // SPBAR + 04h: Hardware Sequencing Flash Status Register (16bits)
    UINT8   FlashAddr;  // SPBAR + 08h: Flash Address
    UINT8   FlashData;  // SPBAR + 10h: Flasg Data (64 bytes)
    UINT8   SwSeqSts;   // SPBAR + 90h: Software Sequencing Flash Status Register
    UINT8   SwSeqCtrl;  // SPBAR + 91h: Software Sequencing Flash Control Register
    UINT8   PreOpCode;  // SPBAR + 94h: Prefix Opcode Configuration
    UINT8   OpCodeType; // SPBAR + 96h: Opcode Type Configuration
    UINT8   OpCodeMenu; // SPBAR + 98h: Opcode Menu Configuration
    UINT8   BiosFdoc;   // SPBAR + 0B0h: Flash Descriptor Observability Control Register
    UINT8   BiosFdod;   // SPBAR + 0B4h: Flash Descriptor Observability Data Register   
    UINT8   BiosLVscc;  // SPBAR + 0C4h: Host Lower Vendor Specific Component Capabilities Register
    UINT8   BiosUVscc;  // SPBAR + 0C8h: Host Upper Vendor Specific Component Capabilities Register
    UINT8   BiosPTInx;  // SPBAR + 0CCh: Parameter Table Index
    UINT8   BiosPTData;  // SPBAR + 0D0h: Parameter Table Data
} SPI_REGS;

// SPI default opcode slots
#define     SPI_OPCODE_WRITE_INDEX      0x0
#define     SPI_OPCODE_READ_INDEX       0x1
#define     SPI_OPCODE_ERASE_INDEX      0x2
#define     SPI_OPCODE_READ_S_INDEX     0x3
#define     SPI_OPCODE_READ_ID_INDEX    0x4
#define     SPI_OPCODE_WRITE_S_INDEX    0x5
#define     SPI_OPCODE_WRITE_S_E_INDEX  0x6
#define     SPI_OPCODE_WRITE_E_INDEX    0x7
#define     SPI_OPCODE_READ_SFDP_INDEX  0x7
#define     SPI_OPCODE_AAI_INDEX        0x6
#define     SPI_OPCODE_WRITE_D_INDEX    0x7
#define     SPI_PREFIX_WRITE_S_EN       0x1
#define     SPI_PREFIX_WRITE_EN         0x0

// SPI Hardware Flash Cycle Type  
#define     SPI_HW_SEQ_READ             0x0
#define     SPI_HW_SEQ_WRITE            0x2
#define     SPI_HW_SEQ_ERASE_4K         0x3
// Below Hardware Flash Cycle Types are added from Skylake-PCH  
#define     SPI_HW_SEQ_ERASE_64K        0x4
#define     SPI_HW_SEQ_READ_SFDP        0x5
#define     SPI_HW_SEQ_READ_ID          0x6
#define     SPI_HW_SEQ_WRITE_STATUS     0x7
#define     SPI_HW_SEQ_READ_STATUS      0x8

#define     SPI_MAX_DATA_TRANSFER       0x40
#define     FLASH_VALID_SIGNATURE       0x0ff0a55a
#define     NO_ADDRESS_UPDATE           0
#ifndef SPI_OPCODE_TYPE_READ_NO_ADDRESS
#define     SPI_OPCODE_TYPE_READ_NO_ADDRESS     0x0
#define     SPI_OPCODE_TYPE_WRITE_NO_ADDRESS    0x1
#define     SPI_OPCODE_TYPE_READ_WITH_ADDRESS   0x2
#define     SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS  0x3
#endif  // #ifndef SPI_OPCODE_TYPE_READ_NO_ADDRESS
#ifndef SPI_USE_HARDWARE_SEQUENCE
#define     SPI_USE_HARDWARE_SEQUENCE  0
#endif  // #ifndef SPI_USE_HARDWARE_SEQUENCE
#ifndef NO_MMIO_FLASH_ACCESS_DURING_UPDATE
#define NO_MMIO_FLASH_ACCESS_DURING_UPDATE 0
#endif
//----------------------------------------------------------------------------
// Module level global data
extern UINT16       gFlashId;
extern FLASH_PART   *FlashAPI;
//----------------------------------------------------------------------------
// Function Externs
extern
VOID
SpiChipsetVirtualFixup  (
    IN EFI_RUNTIME_SERVICES *pRS
);
//----------------------------------------------------------------------------
// Local prototypes
static BOOLEAN
CommonSpiEraseCommand   (
    IN volatile UINT8       *pBlockAddress
);
static EFI_STATUS
CommonSpiEraseCommandEx   (
    IN volatile UINT8       *pBlockAddress,
    IN UINT32               Length
);
static BOOLEAN
CommonSpiProgramCommand (
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Byte,
    IN OUT  UINT32          *Length
);
static VOID
CommonSpiReadCommand    (
    IN      volatile UINT8  *pByteAddress,
    OUT     UINT8           *Byte,
    IN  OUT UINT32          *Length
);
static BOOLEAN
CommonSpiIsEraseCompleted   (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
);
static BOOLEAN
CommonSpiIsProgramCompleted (
    IN  volatile UINT8      *pByteAddress,
    IN  UINT8               *Byte,
    IN  UINT32              Length,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
);
static VOID
CommonSpiBlockWriteEnable   (
    IN volatile UINT8       *pBlockAddress
);
static VOID
CommonSpiBlockWriteDisable  (
    IN volatile UINT8       *pBlockAddress
);
static VOID
CommonSpiDeviceWriteEnable  (
    VOID
);
static VOID
CommonSpiDeviceWriteDisable (
    VOID
);
static VOID
CommonSpiDeviceVirtualFixup (
    IN EFI_RUNTIME_SERVICES *pRS
);
//----------------------------------------------------------------------------
// Local Variables
FLASH_PART mCommonSpiFlash ={
    CommonSpiReadCommand,
    CommonSpiEraseCommand,
    CommonSpiEraseCommandEx,
    CommonSpiProgramCommand,
//-    CommonSpiIsEraseCompleted,
//-    CommonSpiIsProgramCompleted,
    CommonSpiBlockWriteEnable,
    CommonSpiBlockWriteDisable,
    CommonSpiDeviceWriteEnable,
    CommonSpiDeviceWriteDisable,
    CommonSpiDeviceVirtualFixup,
    1,                      // default value, should be changed in Init function
    SECTOR_SIZE_4KB,        // default value, should be changed in Init function
    NULL                    // Flash Part Number Pointer
};
EX_FLASH_PART mExFlashPart = {
      {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},0,0},
      FLASH_SIZE,           // flash size, should be changed in Init function
      0,                    // flash part id, should be changed in Init function
      0                     // flash part string, should be changed in
                            // Init function
};
#if defined SPI_BASE && SPI_BASE != 0
volatile UINT8  *gSPIBASE = (UINT8*)(UINTN)SPI_BASE;
#else
volatile UINT8  *gSPIBASE = (UINT8*)(UINTN)(SB_RCBA + SPI_BASE_ADDRESS);
#endif
UINT8           gbDeviceVirtual = 0;
UINT8           gbDeviceWriteEnabled = 0;
static UINTN    gBiosRegionBase = 0, gBiosRegionLimit = 0, gMaxFlashLimit = 0;
const SPI_REGS  SPIRegs[] = SPI_REGISTERS;
//----------------------------------------------------------------------------
// Function Definitions
/**
 * The function delays for flash operation.
 *
 */
VOID
SpiIoDelay (VOID)
{
    MicroSecondDelay(10);
    return ;
}


/**
 * This function Initial the SPI BASE Address
 *
 */
VOID InitializeSpiBase (VOID) 
{
    if (SPIRegs->BusDevFuncRegr != 0) {
        IoWrite32(0xcf8, 0x80000000 + SPIRegs->BusDevFuncRegr);
        gSPIBASE = (UINT8*)(UINTN)IoRead32(0xcfc);
    }
}


/*
 * This function retuns the Flash Descriptor Override Pin Strap Status (FDOPSS).
 *
 * @retval TRUE The Flash Descriptor Security Override strap is set
 * @retval FALSE The Flash Descriptor Security Override strap is not set
 */
BOOLEAN IsFlashDescriptorOverrided (VOID)
{
    return !(*(volatile UINT16*)(gSPIBASE + SPIRegs->HwSeqStsCtl) & BIT13);
}

/*
 * This function gets the Base and Limit from the specified Flash Region
 *
 * @param Type Type of Flash region.   
 * @param Base Base Address of region. 
 * @param Limit Limit Address of region.
 *
 * @retval EFI_SUCCESS Operation Completed
 * @retval EFI_INVALID_PARAMETER Operation not Completed
 */
EFI_STATUS
GetFlashRegion(
    FLASH_REGION_TYPE   Type,
    UINTN               *Base,
    UINTN               *Limit
)
{
    UINTN               Data;

    // First, check whether works in Flash Descriptor mode.
    *(volatile UINT32*)(gSPIBASE + SPIRegs->BiosFdoc) = 0;
    Data = *(volatile UINT32*)(gSPIBASE + SPIRegs->BiosFdod);
    if ((UINT32)Data != FLASH_VALID_SIGNATURE) return EFI_UNSUPPORTED;
    // Descriptor Mode, Get Region from SPI Memory Mapped Configuration Registers.
    switch (Type) {
        case FlashRegionDescriptor: 
            Data = *(volatile UINT32*)(gSPIBASE + 0x54); 
            break;
        case FlashRegionBios:       
            Data = *(volatile UINT32*)(gSPIBASE + 0x58); 
            break;
        case FlashRegionMe:         
            Data = *(volatile UINT32*)(gSPIBASE + 0x5C); 
            break;
        case FlashRegionGbE:        
            Data = *(volatile UINT32*)(gSPIBASE + 0x60); 
            break;
        case FlashRegionPlatformData: 
            Data = *(volatile UINT32*)(gSPIBASE + 0x64); 
            break;
        default : return EFI_INVALID_PARAMETER;
    }
    if (Data == 0x7FFF) return EFI_UNSUPPORTED;
    if (Base != NULL) *Base = (Data & 0xFFFF) << 12;
    if (Limit != NULL) *Limit = (Data >> 4) | 0xFFF;
    return EFI_SUCCESS;
}    
/*
 * This function gets the Maximum Limit from the specified Flash Region
 *
 * @param Limit Maximum Limit Address of region.
 *
 * @retval EFI_SUCCESS Operation Completed
 */
EFI_STATUS 
GetMaxRegionLimit(
    UINTN               *Limit
)
{
    UINTN   i, MaxRegionLimit;
    for (*Limit = MaxRegionLimit = i = 0; i < FlashRegionMax; i++) { 
        if (EFI_ERROR(GetFlashRegion(i, NULL, &MaxRegionLimit))) continue;
        if (MaxRegionLimit > *Limit) *Limit = MaxRegionLimit;
    }
    return EFI_SUCCESS;
}    
/*
 * This function reads data from the specified Flash Region
 *
 * @param Type Type of Flash region.   
 * @param Address Start Address of region.                      
 * @param Length number of bytes to read.                      
 * @param Buffer pointer to data to read from the flash region.
 *
 * @retval EFI_SUCCESS Operation Completed
 * @retval EFI_INVALID_PARAMETER Operation not Completed
 */
EFI_STATUS
FlashRegionOperation (
    FLASH_REGION_TYPE   Type,
    FLASH_OPERATION     Operation,
    IN UINT32           Address,
    IN UINT32           Length,
    IN UINT8            *Buffer
)
{
    UINTN               RegionBase, RegionLimit, BiosRegionLimit;
    volatile UINT8      *a;
    UINT32              TempLength = Length, i;
    EFI_STATUS          Status ;

    // Initial SPI Base.
    InitializeSpiBase();
#if SPI_USE_HARDWARE_SEQUENCE == 0
    // Check whether SPI registers have been initialed, return error if no.
    if (*(volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeMenu + 1) == 0) return EFI_NOT_READY;
#endif
    // Get RegionBase and RegioLimit.
    Status = GetFlashRegion(Type, &RegionBase, &RegionLimit);
    if (EFI_ERROR(Status) || ((RegionBase + Length) > (RegionLimit + 1))) return EFI_INVALID_PARAMETER;
    // Get Bios RegionLimit for patching "CommonConvertSpiAddress" hook of CommonSpiReadCommand.
    if (EFI_ERROR(GetFlashRegion(FlashRegionBios, NULL, &BiosRegionLimit))) return EFI_INVALID_PARAMETER;
    if (RegionBase >= (BiosRegionLimit + 1)) {
        a = (volatile UINT8*)((RegionBase - (BiosRegionLimit + 1)) + Address);
    } else { 
        a = (volatile UINT8*)((UINT32)~BiosRegionLimit + RegionBase + Address);
    } 
    switch (Operation) {  
        case FlashRegionErase:
            CommonSpiEraseCommand (a);
            break;
        case FlashRegionRead:
        case FlashRegionWrite:
            for (i = 0; i < Length; i = (Length - TempLength)) {
                if (Operation == FlashRegionRead)
                    CommonSpiReadCommand (a + i, Buffer + i, &TempLength);
                if (Operation == FlashRegionWrite) 
                    CommonSpiProgramCommand (a + i, Buffer + i, &TempLength);    
            }
        break;
        default: return EFI_INVALID_PARAMETER;
    }
    return EFI_SUCCESS;
}    


/**
 * This function verifies whether the command sent to the flash part has 
 * completed 
 *
 * @param HwCycle checking SPI Hardware Sequence 
 * @param SwCycle checking SPI Software Sequence 
 *                      
 */
VOID WaitForSpiCycleDone (UINT8 HwCycle, UINT8 SwCycle)
{
    UINT32              dTimeout;
    UINT8               bCyclyDone;
    
    if (HwCycle) {
        for (dTimeout = 0, bCyclyDone = 0; dTimeout < 0x4000000; dTimeout++) {
            bCyclyDone = *(volatile UINT8*)( gSPIBASE + SPIRegs->HwSeqStsCtl );
            if ((bCyclyDone & BIT00) || ((bCyclyDone & BIT05) == 0)) break;
        }
        // write BIT2 to clear CycleDone status
        *(volatile UINT8*)( gSPIBASE + SPIRegs->HwSeqStsCtl ) = BIT00 + BIT01 + BIT02;
    }
    if (SwCycle) {
        for (dTimeout = 0, bCyclyDone = 0; dTimeout < 0x4000000; dTimeout++) {
            bCyclyDone = *(volatile UINT8*)( gSPIBASE + SPIRegs->SwSeqSts );
            if ((bCyclyDone & BIT02) || ((bCyclyDone & BIT00) == 0)) break;
        }
        // write BIT2 to clear CycleDone status
        *(volatile UINT8*)( gSPIBASE + SPIRegs->SwSeqSts) = BIT02 + BIT03 + BIT04;
    }
}


/**
 * This function issuess SPI Write Disable if AAIWordProgram. 
 *                      
 */
VOID SpiWriteDisable (VOID)
{
    // Opcode menu slot 7 is configured as "Write Disable" if AAIWordProgram.
    *(volatile UINT16*)(gSPIBASE + SPIRegs->SwSeqCtrl) = \
                                ( SPI_OPCODE_WRITE_D_INDEX << 4) + BIT01;
    // Wait for spi software sequence cycle completed.
    WaitForSpiCycleDone (FALSE, TRUE);
    return ;
}


/**
 * This function checks whether need to issue the AAIWordProgram command. 
 *                      
 * @param dAddr Start address to be written.  
 * @param dLength Number of bytes to be written.
 *
 * @retval TRUE Need to issue the AAI command 
 * @retval FALSE No need to issue the AAI command
 */
BOOLEAN
CheckAaiWordProram (
    IN UINT32               dAddr,
    IN UINT32               dLength
)
{
    if ((mExFlashPart.AAIWordProgram != 0) && !(dAddr & 1) && (dLength >= 2))
        return TRUE;
    return FALSE;    
}


/**
 * This function reads the status register by specified flash address
 *                      
 * @param dUpdateAddr flash address to be read the status register.  
 *
 * @retval value of the status register
 */
UINT8
CommonSpiReadStatus   (
    IN UINT32               dUpdateAddr
)
{
    UINT16  wSpiCmd;

    if( dUpdateAddr ) *(volatile UINT32*)( gSPIBASE + SPIRegs->FlashAddr ) = dUpdateAddr;
#if SPI_USE_HARDWARE_SEQUENCE == 2
    // Flash SPI Cycle type 8 generated to "Read Status"
    wSpiCmd = ( SPI_HW_SEQ_READ_STATUS << 1 ) + BIT00;
    *(volatile UINT16*)( gSPIBASE + SPIRegs->HwSeqStsCtl + 2 ) = wSpiCmd;
    // Wait for spi hardware sequence cycle completed.
    WaitForSpiCycleDone (TRUE, FALSE);
#else    
    // Opcode menu slot 3 is configured as "Read Status Register"
    wSpiCmd = SPI_OPCODE_READ_S_INDEX << 4;
    // indicate that data phase is required
    wSpiCmd += (1 << 14);
    // Set BIT1 (Go)
    *(volatile UINT16*)( gSPIBASE + SPIRegs->SwSeqCtrl ) =  wSpiCmd + BIT01;
    // Wait for spi software sequence cycle completed.
    WaitForSpiCycleDone (FALSE, TRUE);
#endif
    // return status register.
    return ( *(volatile UINT8*)( gSPIBASE + SPIRegs->FlashData ) );
}


/**
 * This function verifies whether the command sent to the flash part has 
 * completed
 *
 */
VOID
WaitForWriteOperationCompleted (VOID)
{
    UINT16              wWaitStsRetry;
    UINT8               bStatus;

    for( wWaitStsRetry = 0; wWaitStsRetry < 0xFFFF; wWaitStsRetry++ ) {
        // read flash status register.
        bStatus = CommonSpiReadStatus( NO_ADDRESS_UPDATE );
        // Is operation busy ?
        if( !( bStatus & 0x1 ) ) break;
    }
}


/**
 * This function writes the status register by specified flash address
 *                      
 * @param bWriteData Data to be written to the status register.  
 * @param bOpcodeIndex Index of the OpCode
 * @param bIsDataPhase Data Phase need or not.
 * @param bPrefixOp Index of the Prefix OpCode
 * @param dSectorAddress flash address to be written to the status register
 *
 */
VOID
CommonSpiWriteStatus    (
    IN UINT8                bWriteData,
    IN UINT8                bOpcodeIndex,
    IN UINT8                bIsDataPhase,
    IN UINT8                bPrefixOp,
    IN UINT32               dSectorAddress
)
{
    UINT16  wSpiCmd;

    // Wait for spi hardware/software sequence cycle completed.
    WaitForSpiCycleDone (TRUE, TRUE);
    *(volatile UINT8*)( gSPIBASE + SPIRegs->FlashData ) = bWriteData;
    *(volatile UINT32*)( gSPIBASE + SPIRegs->FlashAddr ) = dSectorAddress;
#if SPI_USE_HARDWARE_SEQUENCE == 2
    // Flash SPI Cycle type 7 generated to "Write Status"
    wSpiCmd = ( SPI_HW_SEQ_WRITE_STATUS << 1 ) + BIT00;
    *(volatile UINT16*)( gSPIBASE + SPIRegs->HwSeqStsCtl + 2 ) = wSpiCmd;
    // Wait for spi hardware sequence cycle completed.
    WaitForSpiCycleDone (TRUE, FALSE);
#else    
    // Opcode menu slot 3 is configured as "Read Status Register"
    wSpiCmd = bOpcodeIndex << 4;
    // indicate that data phase is required
    wSpiCmd += ( bIsDataPhase << 14 );
    // BIT3(Preop 1)
    wSpiCmd += ( bPrefixOp << 3 );
    // Set BIT1 (Go), BIT2(Atomic w/ Prefix),
    *(volatile UINT16*)( gSPIBASE + SPIRegs->SwSeqCtrl ) =  wSpiCmd + BIT01 + BIT02;
    // Wait for spi software sequence cycle completed.
    WaitForSpiCycleDone (FALSE, TRUE);
#endif
    // wait for SPI flash operation completed.
    WaitForWriteOperationCompleted();
    // return status register.
    return ;
}


/**
 * Verifies if the device has been erased properly or if the current byte 
 * is the same as the byte to be written at this location.
 *
 * @param pDestination starting address of where the data will be written
 * @param pSource starting address of the data that is supposed to be written or 
 *        "NULL" for checking erase completed.
 * @param Length length of the data set to check
 * @param SpiCycle use the "SPI Cycle" or not 
 *                      
 * @retval TRUE the data is match or is clean.
 * @retval FALSE the data is not match or is not clean.
 */
static
BOOLEAN
IsEqualOrClean  (
    IN  volatile UINT8  *pDestination,
    IN  UINT8           *pSource,
    IN  UINT32          Length,
    IN  BOOLEAN         SpiCycle
)
{
    UINT8           Buffer[SPI_MAX_DATA_TRANSFER], Data;
    UINT32          i, j, TempLength = Length, RemainBytes = Length;
#if defined NO_MMIO_FLASH_ACCESS_DURING_UPDATE && NO_MMIO_FLASH_ACCESS_DURING_UPDATE == 0
    UINTN           FlashBase = 0xFFFFFFFF - gMaxFlashLimit;
    // Check the Top Swap enabled only if device is not converted to virtual for 
    // below reasons. 1) So Far TopSwap bit is set only if BIOS is in Recovery not 
    // in runtime. 2) The IsTopSwapOn() could not be converted to virtual then could
    // cause General Protection Fault in runtime.
    if (SpiCycle && gBiosRegionLimit && !gbDeviceVirtual && !IsTopSwapOn()) {
        if (((UINTN)pDestination >= (gBiosRegionBase + FlashBase)) && \
            ((UINTN)pDestination <= (gBiosRegionLimit + FlashBase))) SpiCycle = FALSE;
    }
#endif // #if defined NO_MMIO_FLASH_ACCESS_DURING_UPDATE && NO_MMIO_FLASH_ACCESS_DURING_UPDATE == 0
    for (i = 0; i < Length; TempLength = RemainBytes, i = (Length - RemainBytes)) {
        // Initial local read buffer.
       	MemSet(Buffer, SPI_MAX_DATA_TRANSFER, 0);
        // Read flash through SPI Cycle if SpiCycle is requested.
       	if (SpiCycle) CommonSpiReadCommand (pDestination + i, Buffer, &RemainBytes);
       	else {
            // Read flash through flash memory mapping
       	    (RemainBytes > SPI_MAX_DATA_TRANSFER) ? \
                        (RemainBytes -= SPI_MAX_DATA_TRANSFER) : (RemainBytes = 0);
       	    MemCpy(Buffer, pDestination + i, TempLength - RemainBytes);
       	}
        // Verify whether flash contents is the same as written Data or is clean.
        for (j = 0; j < (TempLength - RemainBytes); j++) {
            Data = (pSource != NULL) ? *(pSource + i + j) : (UINT8)(-FLASH_ERASE_POLARITY);
            if (Buffer[j] != Data) return FALSE;    
        }
    }
    return TRUE;
}

/**
 * This function converts the flash address to its physical address
 *                      
 * @param pAddress specified flash address need to be converted
 * @param pdCnvtAddress specified flash address after converted
 *
 * @retval EFI_SUCCESS Address converted successfully 
 * @retval EFI_INVALID_PARAMETER Invalid Address  
 */
EFI_STATUS
CommonConvertSpiAddress   (
    IN volatile UINT8       *pAddress,
    OUT UINT32              *pdCnvtAddress
)
{
    UINTN           BiosBase = gBiosRegionBase, BiosLimit = gBiosRegionLimit;
    UINTN           FlashLimit = gMaxFlashLimit;

    // if flash identified, here checks the BIOS region of Flash Descriptor Table.
    if (!gBiosRegionLimit) {
        // Do not call the GetFlashRegion() with "static" variable, the "static" 
        // variable can't be updated if Pre-Memory drivers of PEI phase.
        if (EFI_ERROR(GetFlashRegion(FlashRegionBios, &BiosBase, &BiosLimit))) {
            // No Flash Descriptor mode, use Flash Capacity as BiosRegion.
            BiosLimit = FlashLimit = (mExFlashPart.FlashCapacity - 1);
        } else GetMaxRegionLimit(&FlashLimit);
        gBiosRegionBase  = BiosBase;
        gBiosRegionLimit = BiosLimit;
        gMaxFlashLimit   = FlashLimit;
    }
    if (gbDeviceVirtual) {
        // pAddress - offset from Flash Device Base.
        pAddress -= FlashDeviceBase;
        // pAddress - 32bit memory mapping address.
        pAddress += (0xFFFFFFFF - FLASH_SIZE) + 1;
    }
    // pAddress - physical address in the flash.
    pAddress += (BiosLimit + 1);
    // check if address inside the flash region.
    if ((UINT32)pAddress > (UINT32)FlashLimit) return EFI_INVALID_PARAMETER;
    *pdCnvtAddress = (UINT32)pAddress;
    return EFI_SUCCESS;
}

/**
 * This function initials the SPI OpCode/Type to SPI registers. 
 *                      
 * @param FlashInfo SPI Flash Information Table.
 *
 */
VOID
InitializeSpiEnvironment (
    IN OUT  FLASH_INFO      *FlashInfo
)
{
    InitializeSpiBase();
#if SPI_USE_HARDWARE_SEQUENCE != 2
    // Using current SPI settings if dummy structure.
    if (FlashInfo->ReadId.Opcode == 0) return;

    //Program first DWORD of opcode commands
    *((volatile UINT32*)( gSPIBASE + SPIRegs->OpCodeMenu + 0 )) = (UINT32)
        // Write Byte (Clear to 0 for un-safe opcode if Hardware Sequence)
        ( (((SPI_USE_HARDWARE_SEQUENCE)? 0 : FlashInfo->Write.Opcode) << (SPI_OPCODE_WRITE_INDEX * 8)) | \
        // Read Data (Clear to 0 for un-safe opcode if Hardware Sequence)
          (((SPI_USE_HARDWARE_SEQUENCE)? 0 : FlashInfo->Read.Opcode) << (SPI_OPCODE_READ_INDEX * 8)) | \
        // Erase 64k Sector (Clear to 0 for un-safe opcode if Hardware Sequence)
          (((SPI_USE_HARDWARE_SEQUENCE)? 0 : FlashInfo->Erase.Opcode) << (SPI_OPCODE_ERASE_INDEX * 8)) | \
        // Read Device Status Reg
          (FlashInfo->ReadStatus.Opcode << (SPI_OPCODE_READ_S_INDEX * 8)) );

    //Program second DWORD of Opcode commands
    *((volatile UINT32*)( gSPIBASE + SPIRegs->OpCodeMenu + 4 )) = (UINT32)
        // Read device ID
      ((FlashInfo->ReadId.Opcode << ((SPI_OPCODE_READ_ID_INDEX - 4) * 8)) | \
        // Write Status Register
       (FlashInfo->WriteStatus.Opcode << \
                                    ((SPI_OPCODE_WRITE_S_INDEX - 4) * 8)) | \
        // Write Status Enable Register
       (FlashInfo->WriteStatusEnable.Opcode << \
                                    ((SPI_OPCODE_WRITE_S_E_INDEX - 4) * 8)) | \
        // Serial Flash Discoverable Paramenters Register
       (SPI_READ_SFDP_COMMAND << ((SPI_OPCODE_READ_SFDP_INDEX - 4) * 8)));

    //Program opcode types
    *((volatile UINT16*)( gSPIBASE + SPIRegs->OpCodeType )) = (UINT16)
        // write with address. (Clear to 0 for un-safe opcode if Hardware Sequence)
        (((SPI_USE_HARDWARE_SEQUENCE)? 0 : FlashInfo->Write.OpcodeType) << (SPI_OPCODE_WRITE_INDEX * 2) | \
        // read with address. (Clear to 0 for un-safe opcode if Hardware Sequence)
         ((SPI_USE_HARDWARE_SEQUENCE)? 0 : FlashInfo->Read.OpcodeType) << (SPI_OPCODE_READ_INDEX * 2) | \
        // write with address. (Clear to 0 for un-safe opcode if Hardware Sequence)
         ((SPI_USE_HARDWARE_SEQUENCE)? 0 : FlashInfo->Erase.OpcodeType) << (SPI_OPCODE_ERASE_INDEX * 2) | \
        // read w/o no adress.
         FlashInfo->ReadStatus.OpcodeType << (SPI_OPCODE_READ_S_INDEX * 2) | \
        // read with address.
         FlashInfo->ReadId.OpcodeType << (SPI_OPCODE_READ_ID_INDEX * 2) | \
        // write w/o address.
         FlashInfo->WriteStatus.OpcodeType << (SPI_OPCODE_WRITE_S_INDEX * 2) | \
        // write w/o address.
         FlashInfo->WriteStatusEnable.OpcodeType << (SPI_OPCODE_WRITE_S_E_INDEX * 2) | \
        // Read with address.
         SPI_OPCODE_TYPE_READ_WITH_ADDRESS << (SPI_OPCODE_READ_SFDP_INDEX * 2));

    //set up the prefix opcodes for commands
    *((volatile UINT16*)( gSPIBASE + SPIRegs->PreOpCode )) = (UINT16)
        ( ( FlashInfo->WriteStatusEnable.Opcode << 8 ) | \
                                    ( FlashInfo->WriteEnable.Opcode ) );

    //set up Program Opcode and Optype if AAIWordProgram.
    if (mExFlashPart.AAIWordProgram != 0) {
        UINT8  bOpType = 0;
        *((volatile UINT16*)(gSPIBASE + SPIRegs->OpCodeMenu + 6)) = 0x4ad; 
        bOpType = *((volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeType + 1));
        bOpType = ((bOpType & 0xf) | \
                 (SPI_OPCODE_TYPE_WRITE_NO_ADDRESS << 6) | \
                 (SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS << 4));
        *((volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeType + 1)) = bOpType;
    }
#if defined FAST_READ_SUPPORT && FAST_READ_SUPPORT != 0
#if defined SPI_FREQUENCY && SPI_FREQUENCY != 0
    // Here to sets frequency to use for all SPI software sequencing cycles.
    {
        UINT32  SwStatusRegr = 0;
        SwStatusRegr = *((volatile UINT32*)(gSPIBASE + SPIRegs->SwSeqSts));
        SwStatusRegr &= ~ (BIT26 | BIT25 | BIT24);
        SwStatusRegr |= (SPI_FREQUENCY << 24);
        *((volatile UINT32*)(gSPIBASE + SPIRegs->SwSeqSts)) = SwStatusRegr;
    }
#endif // #if defined SPI_FREQUENCY && SPI_FREQUENCY != 0   
#endif // #if defined FAST_READ_SUPPORT && FAST_READ_SUPPORT != 0   
#endif // #if SPI_USE_HARDWARE_SEQUENCE != 2
    return ;
}

#if defined SPI_INITIALIZE_WITH_VSCC && SPI_INITIALIZE_WITH_VSCC == 1
/**
 * This function checks whether SPI Quad Read support through SFDP or not.
 *                      
 * @retval EFI_SUCCESS Flash part have the Quad Read support
 * @retval EFI_UNSUPPORTED Flash part have not the Quad Read support
 *
 */
EFI_STATUS
ProgramVsccByDescriptor(
    VOID
)
{
    UINT8           VsccBuffer[SPI_MAX_DATA_TRANSFER];
    UINT32          VsccTable = 0, VsccLength = 0, MaxRead, i, j;
    EFI_STATUS      Status;

    //Read Flash Upper Map
    MemSet( VsccBuffer, SPI_MAX_DATA_TRANSFER, 0 );
    Status = FlashRegionOperation(FlashRegionDescriptor, \
                            FlashRegionRead, 0xEFC, sizeof (UINT32), VsccBuffer);
    if (EFI_ERROR(Status) || \
        !(*(UINT32*)VsccBuffer) || (*(UINT32*)VsccBuffer == -1)) return Status;

    // VSCC Table Base Address represents address bits [11:4]
    VsccTable = (*(UINT32*)VsccBuffer & 0xFF) << 4;
    // VSCC Table Length is the 1-based number of DWORDs.
    VsccLength = (*(UINT32*)VsccBuffer & 0xFF00) >> 6;
    
    for (i = 0; i < VsccLength; i += SPI_MAX_DATA_TRANSFER) {
        MemSet( VsccBuffer, SPI_MAX_DATA_TRANSFER, 0 );
        MaxRead = ((VsccLength - i) >= SPI_MAX_DATA_TRANSFER) ? \
                                    SPI_MAX_DATA_TRANSFER : (VsccLength - i);
        // Read VSCC Table .                                
        Status = FlashRegionOperation (FlashRegionDescriptor, \
                    FlashRegionRead, VsccTable + i, MaxRead, VsccBuffer);
        if (EFI_ERROR(Status)) return Status;
        for (j = 0; j < MaxRead; j += 8) {
            if (*(UINT32*)(VsccBuffer + j) != mExFlashPart.FlashVenDevId) continue;
            VsccTable = *(UINT32*)(VsccBuffer + j + 0x04);    
            if (SPIRegs->BiosLVscc != 0) 
                *(volatile UINT32*)(gSPIBASE + SPIRegs->BiosLVscc) = VsccTable;
            if (SPIRegs->BiosUVscc != 0) 
                *(volatile UINT32*)(gSPIBASE + SPIRegs->BiosUVscc) = VsccTable;
            return EFI_SUCCESS;
        }
    }    
    return EFI_NOT_FOUND;
}

/**
 * This function checks whether SPI Quad Read support through SFDP or not.
 *                      
 * @retval EFI_SUCCESS Flash part have the Quad Read support
 * @retval EFI_UNSUPPORTED Flash part have not the Quad Read support
 *
 */
EFI_STATUS
SerialFlashDiscoveryForQuadRead (VOID)
{
    UINT8               SfdpData[256]; 
    UINT16              wSpiCmd, wOffset, j;
    UINT32              Index;

   	MemSet(SfdpData, 256, 0);
    for (Index = 0; Index < 256; Index += 64) {
    	// Set SPI read-address = 0, 64, 128, 256 
    	*(volatile UINT32*)(gSPIBASE + SPIRegs->FlashAddr) = Index;
#if SPI_USE_HARDWARE_SEQUENCE == 2
        // Flash SPI Cycle type 5 generated to "Read SFDP"
    	wSpiCmd = ( (64 - 1) << 8 );
        wSpiCmd += ( SPI_HW_SEQ_READ_SFDP << 1 ) + BIT00;
        *(volatile UINT16*)( gSPIBASE + SPIRegs->HwSeqStsCtl + 2 ) = wSpiCmd;
        // Wait for spi hardware sequence cycle completed.
        WaitForSpiCycleDone (TRUE, FALSE);
#else // #if SPI_USE_HARDWARE_SEQUENCE == 0 OR 1
    	// set opcode for "5A"
    	wSpiCmd = SPI_OPCODE_READ_SFDP_INDEX << 4;
    	// set transaction = 64 bytes
    	wSpiCmd += ((64 - 1) << 8);
    	// indicate that data phase is required
    	wSpiCmd += (1 << 14);
    	// Go (BIT1)
    	*(volatile UINT16*)(gSPIBASE + SPIRegs->SwSeqCtrl) =  wSpiCmd + BIT01;
        // Wait for spi software sequence cycle completed.
    	WaitForSpiCycleDone (FALSE, TRUE);
#endif // #if SPI_USE_HARDWARE_SEQUENCE == 2
    	SpiIoDelay();
    	for (j = 0; j < 64; j++)
    	    *(SfdpData + Index + j) = *(volatile UINT8*)(gSPIBASE + SPIRegs->FlashData + j);
  	    // Serial Flash Discoverable Parameters (SFDP) Signature = 50444653h
    	if (Index == 0) {
    	    wOffset = 0;
    	    if (*(UINT32*)&SfdpData[wOffset] != 0x50444653) {
                if (*(UINT32*)&SfdpData[++wOffset] != 0x50444653) {
                    if (SPIRegs->BiosPTInx == 0) return EFI_UNSUPPORTED;
                    // Try to read the "SFDP" from SPI Parameter Table Register
                    *(volatile UINT32*)(gSPIBASE + SPIRegs->BiosPTInx) = 0;
                    MicroSecondDelay(1);
                    Index = *(volatile UINT32*)(gSPIBASE + SPIRegs->BiosPTData + 4);
                    if(Index == 0x50444653) return EFI_SUCCESS;
                    return EFI_UNSUPPORTED;
    	        }
            }        
        }            
    }
  	// SFDP opode at address Ch bits 23:00 = Parameter ID 0 table Address
  	Index = (*(UINT32*)&SfdpData[wOffset + 0xC] & 0x00FFFFFF);
  	// SFDP opode at address 05h(SFPD Major Revisions) must = 0001h
  	// SFDP opode at address 0Ah(Serial Flash Basic Major Revisions) must = 0001h
  	if ((SfdpData[wOffset + 5] != 0x1) || (SfdpData[wOffset + 0xA] != 0x1) || \
        ((Index + 0x10) > 256)) return EFI_UNSUPPORTED;
    // Parameter ID 0 Table BIT[21] - Fast Read Quad I/O.
    // Parameter ID 0 Table BIT[22] - Fast Read Quad Output.
    if (*(UINT32*)&SfdpData[Index + 1] & (BIT21 + BIT22)) return EFI_SUCCESS;
    return EFI_UNSUPPORTED;
}

/**
 * This function re-initials the SPI OpCode/Type to SPI registers. 
 *                      
 * @param FlashInfo SPI Flash Information Table.
 *
 */
VOID ProgramVsccBySfdp (
    IN OUT  FLASH_INFO      *FlashInfo
)
{
    // Program UPPER/LOWER VSCC register.
    UINT32          dVSCC = 0;
    EFI_STATUS      Status;

    // Erase Opcode. 
    dVSCC = FlashInfo->Erase.Opcode << 8;
    // Block/Sector Erase Size.
    if (FlashInfo->SectorSize == SECTOR_SIZE_4KB) dVSCC |= BIT00;
    else if (FlashInfo->SectorSize == SECTOR_SIZE_8KB) dVSCC |= BIT01;
    else if (FlashInfo->SectorSize == SECTOR_SIZE_64KB) dVSCC |= (BIT00 + BIT01);
    // Write Granularity.
    if (FlashInfo->PageSize != 1) dVSCC |= BIT02;
    // Write Status Required.
    if (FlashInfo->WriteStatusEnable.Opcode == 0x50) dVSCC |= BIT03;
    // Write Enable On Write Status.
    if (FlashInfo->WriteStatusEnable.Opcode == 0x39) dVSCC |= BIT04;
    Status = SerialFlashDiscoveryForQuadRead();
    // Updated by Intel VSCCommn.bin Content Rev# 3.9 (2014)
    if (!EFI_ERROR(Status)) {
        switch ((UINT8)mExFlashPart.FlashVenDevId) {
            case 0xC2 : // MXIC
                dVSCC |= BIT6;  break;
            case 0x1F : // Atmel (BIT4 - Write Enable on Write to Status "06h")
                dVSCC |= BIT4;
                // Only DQ series (DID:0x8x) w/ Quad Mode support.
                if (((mExFlashPart.FlashVenDevId >> 12) & 0xf) != 8) break;
                dVSCC |= (BIT5 + BIT6); break;
            case 0xBF : // SST/Microchip
                dVSCC |= BIT7;  break;
            case 0x20 : // Numonyx/Micron
            case 0x1C : // EON
            case 0x8C : // ESMT
                break ;
            case 0x37 : // AMIC (Only LQ series (DID:0x40) w/ Quad Mode support)
                if ((UINT8)(mExFlashPart.FlashVenDevId >> 8) != 0x40) break;
            default :   // Winbond, Spansion, GigaDevice, Fidelix
                dVSCC |= BIT5;
                break;
        }
    }    
    if (Status != EFI_WRITE_PROTECTED) {
        if (SPIRegs->BiosLVscc != 0)
            *(volatile UINT32*)(gSPIBASE + SPIRegs->BiosLVscc) = dVSCC;
        if (SPIRegs->BiosUVscc != 0)
            *(volatile UINT32*)(gSPIBASE + SPIRegs->BiosUVscc) = dVSCC;
    }
}
#endif // #if defined SPI_INITIALIZE_WITH_VSCC && SPI_INITIALIZE_WITH_VSCC == 1

/**
 * This procedure unlocks "Global Block Protection" for Microchip/SST 26VF flash parts. 
 *                      
 */
VOID GlobalBlockProtectionUnlock (VOID)
{
    UINT8           SavedOpType, SavedOpCode;
    
    if (mExFlashPart.GlobalBlockUnlock != TRUE) return;
    SavedOpCode = *(volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeMenu + 7);
  	SavedOpType = *(volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeType + 1);
    // Microchip/SST "Global Block Protection" Command "0x98".
  	*(volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeMenu + 7) = 0x98;
  	// Do nothing if SPI Registers is locked.
  	if (*(volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeMenu + 7) != 0x98) return;
  	// Update SPI OpMenu[7] to "WRITE W/O ADDRESS".
    *(volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeType + 1) = \
                                                (SavedOpType & 0x3f) | BIT06;
    // Opcode menu slot 7 is configured as "Write Disable" if AAIWordProgram.
    *(volatile UINT16*)(gSPIBASE + SPIRegs->SwSeqCtrl) = \
                                    (SPI_OPCODE_WRITE_D_INDEX << 4) + BIT01;
    // wait for chipset SPI cycle completed.
    WaitForSpiCycleDone (FALSE, TRUE);
    *(volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeMenu + 7) = SavedOpCode;
  	*(volatile UINT8*)(gSPIBASE + SPIRegs->OpCodeType + 1) = SavedOpType;
    return ;
}
/**
 * This function re-initials the SPI OpCode/Type to SPI registers. 
 *                      
 * @param FlashInfo SPI Flash Information Table.
 *
 */
VOID
ReinitializeSpiEnvironment (
    IN OUT  FLASH_INFO      *FlashInfo
)
{
#if defined SPI_INITIALIZE_WITH_VSCC && SPI_INITIALIZE_WITH_VSCC == 1
    if (EFI_ERROR(ProgramVsccByDescriptor())) ProgramVsccBySfdp(FlashInfo);
#endif // #if defined SPI_INITIALIZE_WITH_VSCC && SPI_INITIALIZE_WITH_VSCC == 1
    GlobalBlockProtectionUnlock ();
    InitializeSpiEnvironment ( FlashInfo );
}


/**
 * This function read the Manufacturer/Device ID from flash part. 
 *                      
 * @param FlashInfo SPI Flash Information Table.
 * @param FlashAddress Flash address of the flash part. Can be used to 
 *                      send ID command
 *
 * @retval Value of the Manufacturer/Device ID to be read from flash part
 *
 */
UINT32
CommonSpiReadIdEx (
    IN      FLASH_INFO      *FlashInfo,
    IN      UINT32           FlashAddress
)
{
    UINT16              wSpiCmd;

    // Set SPI read-address = 0
    *(volatile UINT32*)( gSPIBASE + SPIRegs->FlashAddr ) = 0;
#if SPI_USE_HARDWARE_SEQUENCE == 2
    // Flash SPI Cycle type 6 generated to "Read JEDEC ID"
    wSpiCmd = ( ( 3 - 1 ) << 8 );
    wSpiCmd += ( SPI_HW_SEQ_READ_ID << 1 ) + BIT00;
    *(volatile UINT16*)(gSPIBASE + SPIRegs->HwSeqStsCtl + 2) = wSpiCmd;
    // Wait for spi hardware sequence cycle completed.
    WaitForSpiCycleDone (TRUE, FALSE);
#else // #if SPI_USE_HARDWARE_SEQUENCE == 0 OR 1
    // set opcode for "Read ID"
    wSpiCmd = SPI_OPCODE_READ_ID_INDEX << 4;
    // set transaction = 3 bytes
    wSpiCmd += ( ( 3 - 1 ) << 8 );
    // indicate that data phase is required
    wSpiCmd += ( 1 << 14 );
    // Go (BIT1)
    *(volatile UINT16*)( gSPIBASE + SPIRegs->SwSeqCtrl ) =  wSpiCmd + BIT01;
    // Wait for spi software sequence cycle completed.
    WaitForSpiCycleDone (FALSE, TRUE);
#endif // #if SPI_USE_HARDWARE_SEQUENCE == 2
    SpiIoDelay();
    return (*(volatile UINT32*)( gSPIBASE + SPIRegs->FlashData ) & 0x00FFFFFF);
}


/**
 * This function read the Manufacturer/Device ID from flash part. 
 *                      
 * @param FlashInfo SPI Flash Information Table.
 * @param FlashAddress Flash address of the flash part. Can be used to 
 *                      send ID command
 *
 * @retval TRUE Read Flash ID completed
 *
 */
BOOLEAN
CommonSpiReadId (
    IN      FLASH_INFO      *FlashInfo,
    IN OUT  UINT32          *dFlashId
)
{
    InitializeSpiEnvironment(FlashInfo);
    *dFlashId = CommonSpiReadIdEx(FlashInfo, 0);
    return TRUE;
}


/**
 * This API function erases a block in the flash. Flash model specific code
 * will branch out from this routine
 *
 * @param pBlockAddress Block that need to be erased
 * @param Length Number of bytes that need to be erased
 *
 * @retval EFI_SUCCESS erase complete
 * @retval EFI_DEVICE_ERROR erase not completed
 */
EFI_STATUS
CommonSpiEraseCommandEx   (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    volatile UINT32     dSectorAddr, dPhyAddress;
    UINT16              i, wEraseRetry, wNumSectors;
    UINT16              wSpiCmd;
    EFI_STATUS          Status;

    // These parts only erase in 64K sectors
    InitializeSpiEnvironment (&mExFlashPart.FlashCommandMenu);
    if (Length % FlashAPI->FlashSectorSize) return EFI_INVALID_PARAMETER;
    wNumSectors = (Length / FlashAPI->FlashSectorSize);
	//APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	Status = EFI_SUCCESS;
	//APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    for ( i = 0; i < wNumSectors ; i++ ) {
        dSectorAddr = (UINT32)(pBlockAddress + (i * FlashAPI->FlashSectorSize));
        if (EFI_ERROR(CommonConvertSpiAddress((volatile UINT8*)dSectorAddr, \
                                    &dPhyAddress))) return EFI_INVALID_PARAMETER;
        if (IsEqualOrClean ((volatile UINT8*)dSectorAddr, \
                                    NULL, FlashAPI->FlashSectorSize, TRUE)) continue;
        for ( wEraseRetry = 0; wEraseRetry < FLASH_RETRIES; wEraseRetry++ ) {
            // Wait for spi hardware/software sequence cycle completed.
            WaitForSpiCycleDone (TRUE, TRUE);
            *(volatile UINT32*)( gSPIBASE + SPIRegs->FlashAddr ) = dPhyAddress;
#if SPI_USE_HARDWARE_SEQUENCE != 0
            // Flash SPI Cycle type 3 generated to "4K Block Erase"
            wSpiCmd = ( SPI_HW_SEQ_ERASE_4K << 1 ) + BIT00;
#if SPI_USE_HARDWARE_SEQUENCE == 2
            if (FlashAPI->FlashSectorSize == SECTOR_SIZE_64KB) {
                // Flash SPI Cycle type 4 generated to "64K Block Erase"
                wSpiCmd = ( SPI_HW_SEQ_ERASE_64K << 1 ) + BIT00;
            }  
#endif // #if SPI_USE_HARDWARE_SEQUENCE == 2
            *(volatile UINT16*)( gSPIBASE + SPIRegs->HwSeqStsCtl + 2 ) = wSpiCmd;
            // Wait for spi hardware sequence cycle completed.
            WaitForSpiCycleDone (TRUE, FALSE);
#else // #if SPI_USE_HARDWARE_SEQUENCE == 0
            // opcode index 2 is programmed for erase command.
            // Set BIT1 (Go), BIT2(Atomic w/ Prefix)
            wSpiCmd = ( SPI_OPCODE_ERASE_INDEX << 4) + BIT01 + BIT02;
            *(volatile UINT16*)( gSPIBASE + SPIRegs->SwSeqCtrl ) = wSpiCmd;
            // Wait for spi software sequence cycle completed.
            WaitForSpiCycleDone (FALSE, TRUE);
#endif // #if SPI_USE_HARDWARE_SEQUENCE != 0
            // wait for SPI flash operation completed.
            WaitForWriteOperationCompleted();
            // write operation appeared to succeed, now read back byte
            // and compare.
            CommonSpiIsEraseCompleted ((volatile UINT8*)dSectorAddr, NULL, &Status);
            if (!EFI_ERROR(Status)) break;
        }
        if (EFI_ERROR(Status)) return Status;
        // According to Intel Server Platform Services Integration Guide, 
        // Section#5.3.5, Waits 10 milliseconds delay while erase blocks.
        MicroSecondDelay(10);
    }
    return Status;
}


/**
 * This API function erases a block in the flash. Flash model specific code
 * will branch out from this routine
 *
 * @param pBlockAddress Block that need to be erased
 *                      
 */
BOOLEAN
CommonSpiEraseCommand   (
    IN volatile UINT8*      pBlockAddress
)
{
    return (EFI_ERROR(CommonSpiEraseCommandEx(pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
}


/**
 * This function programs a byte data to the specified location
 *
 * @param pByteAddress Location where the data to be written
 * @param Byte Byte to be written
 * @param Length Number of bytes to write
 *                      
 */
BOOLEAN
CommonSpiProgramCommand (
    IN volatile UINT8       *pByteAddress,
    IN UINT8                *Byte,
    IN OUT UINT32           *Length
)
{
    UINT8               bFlashRetry = 0;
    UINT16              wProgBytes = 0, i = 0;
    UINT16              wSpiCmd = 0, wRetry = 0, wMaxNumBytes = 0;
    UINT32              dPhyAddress = 0;


    InitializeSpiEnvironment( &mExFlashPart.FlashCommandMenu );
    wProgBytes = mCommonSpiFlash.FlashProgramSize;
    if ( mCommonSpiFlash.FlashProgramSize != 1 ) {
        // Limit the max transfer to the number of bytes the chipset can
        // transfer per cycle
        if (*Length >= SPI_MAX_DATA_TRANSFER) wProgBytes = SPI_MAX_DATA_TRANSFER;
        else wProgBytes = *Length;
        // this is currently for the WINBOND parts only
        // mask off lowest 8 bits of address so that we can determine how
        // many bytes we can write before we hit the end of a page
        wMaxNumBytes = 0x100 - ((UINT32)pByteAddress & 0xFF);
        if ( (UINT32)pByteAddress & 0x1 )   wProgBytes = 1;
        else if ( wProgBytes > wMaxNumBytes ) wProgBytes = wMaxNumBytes;
    } else if(CheckAaiWordProram((UINT32)pByteAddress, *Length)) wProgBytes = 2;

    for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
        // check if do the data need to be programmed ?
        if (IsEqualOrClean((volatile UINT8*)Byte, NULL, wProgBytes, FALSE)) break;
        for (i = 0; i < wProgBytes; i++) {
            for (wRetry = 0; wRetry < (FLASH_RETRIES * 100); wRetry++) {
                *(volatile UINT8*)(gSPIBASE + SPIRegs->FlashData + i) = *(Byte + i);
                if (*(Byte + i) == *(gSPIBASE + SPIRegs->FlashData + i)) break;
            } 
        }       
        // Wait for spi hardware/software sequence cycle completed.
        WaitForSpiCycleDone (TRUE, TRUE);
        if (EFI_ERROR(CommonConvertSpiAddress (pByteAddress, &dPhyAddress))) return FALSE;
        *(volatile UINT32*)( gSPIBASE + SPIRegs->FlashAddr ) = dPhyAddress;
#if SPI_USE_HARDWARE_SEQUENCE != 0
        // Flash SPI Cycle type 2 generated to "Write"
        wSpiCmd = ( SPI_HW_SEQ_WRITE << 1 ) + BIT00;
        // BIT[8..13] - update the number of bytes to be read.
        wSpiCmd += ( wProgBytes - 1 ) << 8;
        *(volatile UINT16*)( gSPIBASE + SPIRegs->HwSeqStsCtl + 2 ) = wSpiCmd;
        // Wait for spi hardware sequence cycle completed.
        WaitForSpiCycleDone (TRUE, FALSE);
#else // #if SPI_USE_HARDWARE_SEQUENCE = 0
        // BIT14 - indicate that it's data cycle.
        wSpiCmd = ( 1 << 14 );
        // BIT[8..13] - update the number of bytes to be written.
        wSpiCmd += ( wProgBytes - 1 ) << 8;
        // opcode index 0 is programmed for program command.
        // Set BIT1 (Go), BIT2(Atomic w/ Prefix)
        if (CheckAaiWordProram(dPhyAddress, wProgBytes))
            wSpiCmd += ( SPI_OPCODE_AAI_INDEX << 4) + BIT01 + BIT02;
        else wSpiCmd += ( SPI_OPCODE_WRITE_INDEX << 4) + BIT01 + BIT02;
        *(volatile UINT16*)( gSPIBASE + SPIRegs->SwSeqCtrl ) = wSpiCmd;
        // Wait for spi software sequence cycle completed.
        WaitForSpiCycleDone (FALSE, TRUE);
#endif // #if SPI_USE_HARDWARE_SEQUENCE != 0
        // wait for chipset SPI flash operation completed.
        WaitForWriteOperationCompleted();
        // Issue SPI Write Disable if SST AAIWordProgram supported.
        if (CheckAaiWordProram(dPhyAddress, wProgBytes)) SpiWriteDisable();
        // write operation appeared to succeed, now read back byte and compare
        // set control for 1-byte data read, no prefix
        if (IsEqualOrClean(pByteAddress, Byte, wProgBytes, TRUE)) break;
        if (FLASH_RETRIES == (bFlashRetry + 1)) return FALSE;
    }
    // Don't forget to return the number of bytes not written
    *Length = *Length - (UINT32)wProgBytes;
    return TRUE;
}

/**
 * This function read data from the specified location
 *
 * @param pByteAddress Location where the data to be read
 * @param Byte Data buffer to be read
 * @param Length Number of bytes to read
 *                      
 */
VOID 
CommonSpiReadCommand   (
    IN  volatile UINT8      *pByteAddress,
    OUT UINT8               *Byte,
    IN  OUT UINT32          *Length
)
{
    UINT32              dReadAddress = 0; 
    UINT16              wSpiCmd, i, wMaxRead = 0;

    InitializeSpiEnvironment( &mExFlashPart.FlashCommandMenu );
    if (EFI_ERROR(CommonConvertSpiAddress (pByteAddress, &dReadAddress))) return;
    wMaxRead = 0x100 - (dReadAddress & 0xff);
    if (wMaxRead > SPI_MAX_DATA_TRANSFER) wMaxRead = SPI_MAX_DATA_TRANSFER;
    if (wMaxRead > *Length) wMaxRead = (UINT16)*Length;
    // Wait for spi hardware/software sequence cycle completed.
    WaitForSpiCycleDone (TRUE, TRUE);
    // update the read address.
    *(volatile UINT32*)( gSPIBASE + SPIRegs->FlashAddr ) = dReadAddress;
#if SPI_USE_HARDWARE_SEQUENCE != 0            
    // Flash SPI Cycle type 0 generated to "Read"
    wSpiCmd = ( SPI_HW_SEQ_READ << 1 ) + BIT00;
    // BIT[8..13] - update the number of bytes to be read.
    wSpiCmd += (wMaxRead - 1) << 8;
    *(volatile UINT16*)( gSPIBASE + SPIRegs->HwSeqStsCtl + 2 ) = wSpiCmd;
    // Wait for spi hardware sequence cycle completed.
    WaitForSpiCycleDone (TRUE, FALSE);
#else // #if SPI_USE_HARDWARE_SEQUENCE = 0
    // Opcode menu slot 1 is configured as "Read Flash"
    wSpiCmd = ( SPI_OPCODE_READ_INDEX << 4 ) + BIT01;
    // indicate that data phase is required
    wSpiCmd += (1 << 14);
    // BIT[8..13] - update the number of bytes to be read.
    wSpiCmd += (wMaxRead - 1) << 8;
    *(volatile UINT16*)( gSPIBASE + SPIRegs->SwSeqCtrl ) = wSpiCmd;
    // Wait for spi software sequence cycle completed.
    WaitForSpiCycleDone (FALSE, TRUE);
#endif // #if SPI_USE_HARDWARE_SEQUENCE != 0
    // read data
    for (i = 0; i < wMaxRead; i++) 
        *(Byte + i) = *(volatile UINT8*)(gSPIBASE + SPIRegs->FlashData + i);
    *Length = *Length - (UINT32)wMaxRead;
}

/**
 * This function verifies whether the block erase command is completed and 
 * returns the status of the command
 *
 * @param pBlockAddress Address to access flash part
 * @param pError True on error and false on success
 * @param pStatus Error status code (0 - Success)
 *                      
 * @retval TRUE erase complete
 * @retval FALSE erase not completed
 */
BOOLEAN
CommonSpiIsEraseCompleted   (
    IN  volatile UINT8          *pBlockAddress,
    OUT BOOLEAN                 *pError,
    OUT UINTN                   *pStatus
)
{
    if (!IsEqualOrClean (pBlockAddress, NULL, FlashAPI->FlashSectorSize, TRUE)) {
        if ( pStatus ) *pStatus = EFI_NOT_READY;
        if ( pError ) *pError = TRUE;
        return FALSE;
    }
    if ( pError ) *pError = FALSE;
    if ( pStatus ) *pStatus = EFI_SUCCESS;
    return TRUE;
}

/**
 * This function verifies whether the program (page or byte) command is 
 * completed and returns the status of the command
 *
 * @param pByteAddress Location of the program command
 * @param Byte Last data byte written
 * @param Length Number of bytes to write
 * @param pError True on error and false on success
 * @param pStatus Error status code (0 - Success)
 *                      
 * @retval TRUE If operation completed successfully 
 *              *pError = FALSE, *pStatus = 0
 * @retval FALSE If operation failed                    
 *              *pError = TRUE, *pStatus = error status
 */
BOOLEAN
CommonSpiIsProgramCompleted (
    IN  volatile UINT8          *pByteAddress,
    IN  UINT8                   *Byte,
    IN  UINT32                  Length,
    OUT BOOLEAN                 *pError,
    OUT UINTN                   *pStatus
)
{
    if (!IsEqualOrClean (pByteAddress, Byte, Length, TRUE)) {
        if ( pStatus ) *pStatus = EFI_NOT_READY;
        if ( pError ) *pError = TRUE;
        return FALSE;
    }
    if ( pError ) *pError = FALSE;
    if ( pStatus ) *pStatus = EFI_SUCCESS;
    return TRUE;
}


/**
 * This function writes the status register by specified flash address
 *                      
 * @param pBlockAddress Address to access flash part
 * @param bStatusData Data to be written to the status register.  
 *
 */
VOID
SpiBlockProtectUpdate (
    IN volatile UINT8           *pBlockAddress,
    IN UINT8                    bStatusData
)
{
    UINT8                   bStatusReg = 0, bPrefixOp, bDataPhase = 1, bBPbits;
    UINT32                  dSectorAddr = 0;

    if (EFI_ERROR(CommonConvertSpiAddress (pBlockAddress, &dSectorAddr))) return;
    bStatusReg = CommonSpiReadStatus (dSectorAddr);
    bBPbits = (BIT02 + BIT03 + BIT04);
    switch ( (UINT8)mExFlashPart.FlashVenDevId ) {
        // if SST flash, prefix 1 w/o address
        case 0xBF :
            bPrefixOp = SPI_PREFIX_WRITE_S_EN;
            break;
        // if ATMEL flash, prefix 0 w/ address
        case 0x1F :
            bBPbits = (BIT02 + BIT03);
        default :
        // default flash, prefix 0 w/o address
            bPrefixOp = SPI_PREFIX_WRITE_EN;
    }
    bStatusReg &= bBPbits;              // keep valid bits.
    bStatusData &= bBPbits;             // keep valid bits.
    if (bStatusReg != bStatusData) {
        CommonSpiWriteStatus (  bStatusData, \
                                SPI_OPCODE_WRITE_S_INDEX, \
                                bDataPhase, \
                                bPrefixOp, \
                                dSectorAddr );
    }                                
}


/**
 * This function contains any flash specific code need to enable a 
 * particular flash block write
 *
 * @param pBlockAddress Location where the block register to be set
 *
 */
VOID
CommonSpiBlockWriteEnable   (
    IN volatile UINT8       *pBlockAddress
)
{
    InitializeSpiEnvironment( &mExFlashPart.FlashCommandMenu );
    SpiBlockProtectUpdate (pBlockAddress, 0);
}


/**
 * This function contains any flash specific code need to disable a 
 * particular flash block write
 *
 * @param pBlockAddress Location where the block register to be set
 *
 */
VOID
CommonSpiBlockWriteDisable  (
    IN volatile UINT8           *pBlockAddress
)
{
#if BLOCK_PROTECT_ENABLE
    SpiBlockProtectUpdate (pBlockAddress, 0xff);
#endif
}


/**
 * This function contains any flash specific code need to enable flash write
 *
 */
VOID
CommonSpiDeviceWriteEnable (VOID)
{
    // check is DeviceWrite enabled, if yes, don't enable it again,
    // else, enable it.
    if ( !gbDeviceWriteEnabled ) {
        gbDeviceWriteEnabled = 1;
    }
}


/**
 * This function contains any flash specific code need to disable flash write
 *
 */
VOID
CommonSpiDeviceWriteDisable (VOID)
{
    // check is DeviceWrite enabled, if yes, disable it,
    // if no, don't disable it.
    if ( gbDeviceWriteEnabled ) {
        gbDeviceWriteEnabled = 0;
    }
}


/**
 * This function will be invoked by the core to convert runtime pointers 
 * to virtual address
 *
 * @param pRS Pointer to runtime services
 *                      
 */
static
VOID
CommonSpiDeviceVirtualFixup (
    IN EFI_RUNTIME_SERVICES  *pRS
)
{

//  // Following is an example code for virtual address conversion
//  pRS->ConvertPointer(0, (VOID**)&FlashDeviceBase);

//-    SpiChipsetVirtualFixup(pRS);
    gbDeviceVirtual = 1;
    pRS->ConvertPointer(0, (VOID **)&gSPIBASE);

    return;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
