//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//*************************************************************************
/** @file CspMeud.c
**/
//**********************************************************************
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include "Efi.h"
#include "Token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Flash.h>
#include "CspMeud.h"
#include "Ofbd/Meud/Meud.h"
#include <Protocol/SmmSwDispatch.h>

#ifndef SB_RCBA ///RCBA is removed after Boardwell project 
#include <Sb.h>
#include <AmiCspLib.h>
#endif

// ME FW Capsule Update support.
#if defined(SecSMIFlash_SUPPORT) && (SecSMIFlash_SUPPORT == 1)
#include <Protocol/SecSmiFlash.h>
static EFI_GUID gEfiSecSmiFlashProtocolGuid = EFI_SEC_SMI_FLASH_GUID;
UINT32 *gFwCapsuleAddress = NULL;
#endif
//----------------------------------------------------------------------------
// Function Externs

//----------------------------------------------------------------------------
// Local prototypes
typedef struct {
    UINT32  SpiBar;         // 
    UINT32  SpiRegrFdoc;    // 
    UINT32  SpiRegrFdod;    // 
    UINT32  HeciBus;        // 
    UINT32  HeciDevice;     // 
    UINT32  HeciFunction;   // 
    UINT32  HeciRegrHfs;   // 
    UINT32  MeDevCtrlBase;  // 
    UINT32  MeDisableBit;   // 
} ME_CFG;

#if !defined ME_DEFINITIONS
#define ME_DEFINITIONS {0, 0, 0, 0, 0, 0, 0, 0, 0}
#endif

//----------------------------------------------------------------------------
// Local Variables
const ME_CFG                  MeCfg[] = ME_DEFINITIONS;
BOOLEAN                       ME_SECOVER = FALSE;
UINT64                        Nonce;
UINT32                        Factory_Base;
UINT32                        Factory_Limit;
EFI_PHYSICAL_ADDRESS          Phy_Address;
OFBD_TC_55_ME_PROCESS_STRUCT  *StructPtr;
UINT8                         MacAddr[6];
EFI_PHYSICAL_ADDRESS          RomBuffer;
EFI_PHYSICAL_ADDRESS          BlockBuffer;

FLASH_REGIONS_DESCRIPTOR
FlashRegionsDescriptor[] =
{
    { FDT_BLK, "FDR" },
    { BIOS_BLK, "BIOS" },
    { GBE_BLK, "GBER" },
    { PDR_BLK, "PDR" },
    { REG8_BLK, "EC" },
    { ME_BLK, "MER" },
    { MAX_BLK, "" },
};

//----------------------------------------------------------------------------
// Function Definitions
/**
    Send Enable HECI message to ME FW.
    @param VOID
    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
HmrfpoEnableMsg(
    VOID
)
{
    return EFI_UNSUPPORTED;
}

/**
    Send LOCK HECI message and lock ME.
    @param VOID
    @retval VOID
    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
HmrfpoLockMsg(
    VOID
)
{
    return EFI_UNSUPPORTED;
}
/**
    Send Disable HECI message when boot in SECOVR_MEI_MSG.
    @param VOID
    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
HmrfpoDisable(
    VOID
)
{
    return EFI_UNSUPPORTED;
}
/**
    To Reset System.
    @param VOID
    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
SendMeiGlobalResetMsg(
    VOID
)
{
    return EFI_UNSUPPORTED;
}
/**
    Get ME Host FW Status.
    @param VOID
    @retval UINT32 Value of HFS

**/
UINT32
GetHfs(
    VOID
)
{
    UINT32  Data, HFSData;
    BOOLEAN DeviceHide = FALSE;
    UINT32  Address = MeCfg->MeDevCtrlBase;

#ifndef SB_RCBA  ///RCBA is removed after boardwell project
     AMI_SB_SERIES SbSeries = AmiGetSbSeries();
     if( SbSeries == SbLp )
     {
         Address = SB_PCR_ADDRESS(PID_PSF1, (R_SB_LP_PCR_PSF1_T0_SHDW_HECI1_REG_BASE + R_SB_PCR_PSFX_T0_SHDW_PCIEN));
     }
     else
     {
         Address = SB_PCR_ADDRESS(PID_PSF1, (R_SB_H_PCR_PSF1_T0_SHDW_HECI1_REG_BASE + R_SB_PCR_PSFX_T0_SHDW_PCIEN));
     }
#endif
    ///
    /// Read RCBA register for saving
    ///
    if( !Address ) return 0xFFFFFFFF; 

    Data = MmioRead32( Address );

    if( Data & MeCfg->MeDisableBit )
    {
        MmioWrite32( Address, (UINT32)(Data & ~MeCfg->MeDisableBit) );
        MmioRead32( Address ); // Take register effect.
        DeviceHide = TRUE;
    }

    HFSData = *(volatile UINT32*)ME_PCIE_CFG_ADDRESS( \
        MeCfg->HeciBus, MeCfg->HeciDevice, MeCfg->HeciFunction, MeCfg->HeciRegrHfs);
    
    if( DeviceHide )
    {
        MmioWrite32( Address, Data );
    }

    return HFSData;
}
/**
    Send a HECI message to lock ME.
    @param VOID
    @retval UINT32 Real Flash Size
**/
UINT32
GetFlashCapacity(
    VOID
)
{
    volatile UINT32 *FDOC;
    volatile UINT32 *FDOD;
    UINT32          FlashDescriptorSig = 0x0FF0A55A;
    static          FlashDensity = 0;
    UINT16          Components;
    UINT8           i,j;

    if( FlashDensity )
        return FlashDensity;

    FDOC = (UINT32*)(MeCfg->SpiBar + MeCfg->SpiRegrFdoc);
    FDOD = (UINT32*)(MeCfg->SpiBar + MeCfg->SpiRegrFdod);

    *FDOC = 0;

    if( *FDOD != FlashDescriptorSig )
        return 0;

    *FDOC = 0x04;
    Components = (*FDOD >> 8) & 0x03;

    *FDOC = 0x1000;
    j = *FDOD;

    for( i=0; i < (Components + 1); i++ )
    {
        switch( j & 0x07 )
        {
        case 0:
            FlashDensity += 0x80000;
            break;
        case 1:
            FlashDensity += 0x100000;
            break;
        case 2:
            FlashDensity += 0x200000;
            break;
        case 3:
            FlashDensity += 0x400000;
            break;
        case 4:
            FlashDensity += 0x800000;
            break;
        case 5:
            FlashDensity += 0x1000000;
            break;
        case 6:
            FlashDensity += 0x2000000;
            break;
        case 7:
            FlashDensity += 0x4000000;
            break;
        default:
            break;
        }
        j = j >> 4;
    }
    return FlashDensity;
}
/**
    Report ME Base address and Length to AFU
    @param BASE_Address address of ME region to be updated
    @param Length Length of ME region to be updated
    @param Func_Num 0 for Command /MEUF , 3 for Command /ME
    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
CspReportMeInfo(
    IN      UINT8   Func_Num,
    IN  OUT UINT32  *BASE_Address,
    IN  OUT UINT32  *Length
)
{
    switch( Func_Num )
    {
        // Only ME 6.0 support Me Ignition FW
        case 0:
            return EFI_UNSUPPORTED;
            break;

        case 3:
            // Flash the whole SPI but BIOS region
            *BASE_Address = 0;
            *Length = GetFlashCapacity() - FLASH_SIZE;
            if( ME_SECOVER )
                return EFI_SUCCESS;
            else
                return EFI_UNSUPPORTED;
            break;

        default:
            return EFI_UNSUPPORTED;
            break;
    }
    return EFI_SUCCESS;
}
/**
    Send Enable and Global reset MSG to ME FW.
    @param VOID
    @retval VOID
    @retval VOID
**/
VOID
MEUDSMIHandler(
    IN  EFI_HANDLE                  DispatchHandle,
    IN  EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
)
{
//      Moved sending HMRFPI MEI to Setup Callback.     
//      HMRFPO_ENABLE_MSG();
//      SEND_MEI_GLOBAL_RESET_MSG();
}
#if defined(SecSMIFlash_SUPPORT) && (SecSMIFlash_SUPPORT == 1)
/**
 * Callback funciotn on Secure SmiFlash Protocol for saving FW Capsule Buffer address.
 * @param Protocol OPTIONAL 
 * @param Interface OPTIONAL
 * @param Handle OPTIONAL
 * @retval EFI_SUCCESS Secure SmiFlash Protocol installed.
 * @retval EFI_NOT_FOUND Secure SmiFlash Protocol not install yet.
**/
static
EFI_STATUS
EFIAPI
SecSmiFlashProtocolCallback(
    IN  const EFI_GUID  *Protocol,
    IN  VOID            *Interface,
    IN  EFI_HANDLE      Handle
)
{
    EFI_SEC_SMI_FLASH_PROTOCOL *SecSmiFlash = NULL;
    if(EFI_ERROR(pSmst->SmmLocateProtocol( \
            &gEfiSecSmiFlashProtocolGuid, NULL, &SecSmiFlash)) ) return EFI_NOT_FOUND;
    gFwCapsuleAddress = SecSmiFlash->pFwCapsule;
    return EFI_SUCCESS;
}
#endif
/**
    Get Host Firmware Status.
    If needed, Send LOCK if needed in SMM.
    @param VOID
    @retval VOID
    @retval VOID
**/
VOID
CspMeudInSmm(
    VOID
)
{
    UINT32      MeHFS = GetHfs();
    EFI_STATUS  Status;
    UINT32      Offset, Length;

    //When ME region is not found, still can update PDR region
    Status = GetRegionOffset( ME_BLK, &Offset, &Length );
    if( Status == EFI_NOT_FOUND ) ME_SECOVER = TRUE;

    // According ME/TXE BWG, ME/TXE FW allows being updated in ME Disable[3]/
    // SECOVR_JMPR[4]/SECOVR_HECI_MSG[5]
    if( ((MeHFS & 0xF0000) == 0x30000) ||
        ((MeHFS & 0xF0000) == 0x40000) || 
        ((MeHFS & 0xF0000) == 0x50000) ) ME_SECOVER = TRUE;

    // Pre-Allocate Memory , just in case we can't get enough memory in SMM.
    if( ME_SECOVER )
    {
        Status = pBS->AllocatePages(
                        AllocateAnyPages,
                        EfiReservedMemoryType,
                        EFI_SIZE_TO_PAGES(GetFlashCapacity()),
                        &RomBuffer );
        if( EFI_ERROR(Status) )
            RomBuffer = 0;

        Status = pBS->AllocatePages(
                        AllocateAnyPages,
                        EfiReservedMemoryType,
                        EFI_SIZE_TO_PAGES(FLASH_BLOCK_SIZE),
                        &BlockBuffer);
        if( EFI_ERROR(Status) )
            BlockBuffer = 0;
    }
#if defined(SecSMIFlash_SUPPORT) && (SecSMIFlash_SUPPORT == 1)
    {
        VOID    *Registration;
        // Create SecSmiFlash Protocol Callback to get and save the FwCapsule 
        // Address, because of the address could be cleared after calling the 
        // functions of Secure SMI Flash protocol.
        pSmst->SmmRegisterProtocolNotify( &gEfiSecSmiFlashProtocolGuid, \
                                    SecSmiFlashProtocolCallback, &Registration );
        SecSmiFlashProtocolCallback( NULL, NULL, NULL );
    }    
#endif  // #if AUTO_ME_UPDATE_POLICY == 2
}
/**
    Handle ME Process
    @param Result 
    @param Message 
    @retval VOID
**/
VOID
MEProcessHandleResult(
    IN  UINT16  Result,
    IN  CHAR8   *Message
)
{
    StructPtr->UpdateResult = Result;
    MemCpy( (UINT8*)(StructPtr->ddMessageBuffer), Message, Strlen( Message ) );

    *(CHAR8*)(StructPtr->ddMessageBuffer + Strlen( Message )) = 0;
}
/**
    Init the Length and Offset need to be updated
    If needed, send ENABLE MESSAGE
    @param ProgramOffset 
    @param ProgramLength 
    @param Step 
    @param InSmm 
    @retval VOID
**/
EFI_STATUS
HandleBuffer(
    IN  OUT UINT32  *ProgramOffset,
    IN  OUT UINT32  *ProgramLength,
    IN  OUT UINT8   *Step,
    IN      BOOLEAN InSmm
)
{
    EFI_STATUS  Status;
    UINT32      Offset, Length;
    UINT32      HFS = GetHfs();

    switch( StructPtr->bBlockType )
    {
        case ME_BLK:
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            if( EFI_ERROR(Status) ) return Status;
            if( (HFS & BIT05) || (HFS & BIT10) )
                *Step = 2;
            else
                *Step = 1;
        break;

        case GBE_BLK:
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            if( Status == EFI_NOT_FOUND )
                return EFI_UNSUPPORTED;
            // Store Mac address
            if( Length )
            {
                UINT8   *Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);
                UINT8   ByteOffset = 0x27;  //Shared Init Control Word (Word 0x13)
                UINT8   SharedInitHigh;

                FlashRead( (Address + ByteOffset), &SharedInitHigh, 1 );

                //Bit 15:14, if the valid field doesn't equal 10b the NVM data is not valid
                if( (SharedInitHigh & 0xC0) != 0x80 )
                {
                    //Other MAC will be in offset 0x2000
                    Address = (UINT8*)FLASH_BASE_ADDRESS(Offset * 2);
                }

                FlashRead( Address, MacAddr, 6 );
            }
            *Step = 0;
        break;

        default:
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            if( EFI_ERROR(Status) )
                return EFI_UNSUPPORTED;
            *Step = 0;
        break;
    }

    *ProgramOffset = Offset;
    *ProgramLength = Length;

    return EFI_SUCCESS;
}
/**
    UpdateRegions
    @param Buffer 
    @param InSmm 
    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
UpdateRegions(
    IN  UINT8   *Buffer,
    IN  BOOLEAN InSmm
)
{
    static UINT32   Offset, Length;
    UINT8           *Address;
    EFI_STATUS      Status;
    BOOLEAN         FlashStatus = TRUE;
    static UINT8    Step = 0;
    static BOOLEAN  NewRegion;
    static UINT8    *ProgramBuffer;
    UINT8           *BufferForVerify = (UINT8*)BlockBuffer;
    UINT32          i;
    // Prepare Offset and Length to be updated
    // If BIT02 , update buffer
    if( (StructPtr->bHandleRequest & BIT02) )
    {
        Status = HandleBuffer( &Offset, &Length, &Step, InSmm );
        if( EFI_ERROR(Status) )
        {
            MEProcessHandleResult( BIT03, "UN SUPPORT TYPE" );
            return Status;
        }
        // Frist In
        NewRegion = TRUE;
        ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    // Set MAC address to buffer
    if( ((StructPtr->bBlockType) == GBE_BLK) && NewRegion )
    {
        UINT16  *GBEPtr;
        UINT16  CheckSum16;

        MemCpy( (Buffer + Offset), MacAddr, 6 );

        GBEPtr = (UINT16*)(Buffer + Offset);
        CheckSum16 = 0;
        // GBE checksum is from word offset 0x00 to 0x3F
        for( i = 0; i < 0x3F; i++ )
            CheckSum16 += GBEPtr[i];
        // Checksum should be 0xBABA, also include itself.
        CheckSum16 = 0xBABA - CheckSum16;
        // Checksum word is at offset 0x3F;
        MemCpy( &GBEPtr[0x3F], &CheckSum16, sizeof(UINT16) );
    }

    if( NewRegion )
    {
        NewRegion = FALSE;
    }

    Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);
    FlashBlockWriteEnable( Address );

    FlashEraseBlock( Address );

    // System hangs when using physical address.
    // So, verify erase complete or not.
    FlashStatus = TRUE;
    FlashRead( Address, BufferForVerify, FLASH_BLOCK_SIZE );

    for( i = 0; (i < FLASH_BLOCK_SIZE) && (*(BufferForVerify + i) == 0xFF); i++ );
    if( i != FLASH_BLOCK_SIZE )
        FlashStatus = FALSE;
    if( FlashStatus )
    {
        FlashProgram( Address, ProgramBuffer, FLASH_BLOCK_SIZE );
        FlashRead( Address, BufferForVerify, FLASH_BLOCK_SIZE );
        for( i = 0; (i < FLASH_BLOCK_SIZE) && (*(ProgramBuffer + i) == *(BufferForVerify + i)); i++ );
        if( i != FLASH_BLOCK_SIZE )
            FlashStatus = FALSE;
        if( FlashStatus )
            Status = EFI_SUCCESS;
        else
            Status = EFI_DEVICE_ERROR;
    }
    else
        Status = EFI_DEVICE_ERROR;
    FlashBlockWriteDisable( Address );
    ProgramBuffer = ProgramBuffer + FLASH_BLOCK_SIZE;
    Length -= FLASH_BLOCK_SIZE;
    Offset += FLASH_BLOCK_SIZE;

    // End of Region Update
    if( Length == 0 )
    {
        NewRegion = TRUE;
    }
    // OEM can output message here in every block updated.
    // Remember to Set BIT02
    else
    {
        MEProcessHandleResult( (BIT01), " ");
        return EFI_SUCCESS;
    }
    // Show Strings
    if( !EFI_ERROR(Status) )
    {
        UINT8   ResultString[25];
        UINT8   Index;

        for( Index = 0; Index < MAX_BLK; Index++ )
        {
            if( FlashRegionsDescriptor[Index].FlashRegion == MAX_BLK )
                break;

            if( FlashRegionsDescriptor[Index].FlashRegion == StructPtr->bBlockType )
            {
                Sprintf( ResultString, "Update success for %s", FlashRegionsDescriptor[Index].Command );
                    break;
            }
        }

        if( (Index == MAX_BLK) || (FlashRegionsDescriptor[Index].FlashRegion == MAX_BLK) )
            Sprintf( ResultString, "Update success" );

        MEProcessHandleResult( (BIT03 | BIT02), ResultString );
    }
    else
    {
        UINT8   ResultString[20];
        UINT8   Index;

        for( Index = 0; Index < MAX_BLK; Index++ )
        {
            if( FlashRegionsDescriptor[Index].FlashRegion == MAX_BLK )
                break;

            if( FlashRegionsDescriptor[Index].FlashRegion == StructPtr->bBlockType )
            {
                Sprintf( ResultString, "Update %s fail", FlashRegionsDescriptor[Index].Command );
                    break;
            }
        }

        if( (Index == MAX_BLK) || (FlashRegionsDescriptor[Index].FlashRegion == MAX_BLK) )
            Sprintf( ResultString, "Update fail" );
        
        MEProcessHandleResult( (BIT00 | BIT02), ResultString );
    }

    return EFI_SUCCESS;
}
/**
    Handle ME Process
    @param MEProcessStructPtr 
    @retval VOID
**/
VOID
MEProcessHandler
(
    IN  OUT OFBD_TC_55_ME_PROCESS_STRUCT    **MEProcessStructPtr
)
{
    EFI_STATUS                    Status;
    static UINTN                  NumberOfPages;
    static UINT32                 SizeCopied;
    static BOOLEAN                UseSmmMem = FALSE;
    static EFI_PHYSICAL_ADDRESS   SMM_Address;

    StructPtr = *MEProcessStructPtr;
    switch( StructPtr->bHandleRequest )
    {
        // Allocate Buffer
        case 1:
            NumberOfPages = StructPtr->TotalBlocks;
            Status = pSmst->SmmAllocatePages( AllocateAnyPages, 
                     EfiRuntimeServicesData, NumberOfPages, &SMM_Address );
            if( !EFI_ERROR(Status) )
            {
                UseSmmMem = TRUE;
                Phy_Address = SMM_Address;
            }
            else
                Phy_Address = RomBuffer;
            // No memory allocated
            if( !Phy_Address )
                MEProcessHandleResult( (BIT00 | BIT02), "Error : No Memory Allocated!!" );

            Status = pSmst->SmmAllocatePages( AllocateAnyPages, 
                     EfiRuntimeServicesData, EFI_SIZE_TO_PAGES(FLASH_BLOCK_SIZE), &BlockBuffer );
            if( EFI_ERROR(Status) ) BlockBuffer = 0;
            SizeCopied = 0;
        break;

#if defined(SecSMIFlash_SUPPORT) && (SecSMIFlash_SUPPORT == 1)
        // ME FW Capsule Update functions.
        case 3: // ME FW Capsule Update. (ME FW only) 
        case 5: // ME FW Capsule Update. (BIOS + ME FW) 
            if( !gFwCapsuleAddress )
            {
                MEProcessHandleResult( (BIT00 | BIT02), "Error : Functon Not Supported!!" );
                break;
            }
            // Initial local variables.
            Phy_Address = 0;
            SizeCopied = 0;
            if( (StructPtr->bHandleRequest == 3) && \
                (StructPtr->TotalBlocks <= EFI_SIZE_TO_PAGES(FWCAPSULE_IMAGE_SIZE)) )
            {
                // Function#3 : ME FW Update only, upload ME FW capsule from the beggining.
                Phy_Address = (EFI_PHYSICAL_ADDRESS)gFwCapsuleAddress;
            }
            if( (StructPtr->bHandleRequest == 5) && \
                (StructPtr->TotalBlocks <= \
                        EFI_SIZE_TO_PAGES(FWCAPSULE_IMAGE_SIZE - FLASH_SIZE)) )
            {
                // Function#5 : BIOS + ME FW Update, upload capsule from the end of BIOS.
#if defined(FWCAPSULE_FILE_FORMAT) && (FWCAPSULE_FILE_FORMAT == 1)
                Phy_Address = (EFI_PHYSICAL_ADDRESS)gFwCapsuleAddress + FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE;
#else                
                Phy_Address = (EFI_PHYSICAL_ADDRESS)gFwCapsuleAddress + FLASH_SIZE;
#endif                
            }
            if( !Phy_Address ) MEProcessHandleResult( (BIT00 | BIT02), "Error : Buffer Too Small!!" );
            break;
#endif  // #if SecSMIFlash_SUPPORT == 1
        // Recieve Data from AFU
        case 2:
            MemCpy(
                (UINT8*)(Phy_Address + SizeCopied),
                (UINT8*)StructPtr->ddMeDataBuffer,
                StructPtr->ddMeDataSize );
            SizeCopied += StructPtr->ddMeDataSize;
        break;

        // Update
        case 4:
            UpdateRegions( (UINT8*)Phy_Address, TRUE );
        break;

        // Continue....
        case 8:
            UpdateRegions( (UINT8*)Phy_Address, TRUE );
        break;

        // Free Buffer
        case 0x10:
            if( UseSmmMem )
                pSmst->SmmFreePages( Phy_Address, NumberOfPages );

        break;
    }
}

/**
    Get GBE Region Offet of whole FlashPart
    @param Region - This is the ME_BLOCK_TYPE, need to mapping to programming guide region number
    @param Offset
    @param Length
    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
GetRegionOffset(
    UINT8   Region,
    UINT32  *Offset,
    UINT32  *Length
)
{
    volatile UINT32 *FDOC;
    volatile UINT32 *FDOD;
    UINT32          FlashDescriptorSig = 0x0FF0A55A;
    UINT32          Buffer32, RegionStart, RegionEnd;
    UINT8           EdsRegion;

    FDOC = (UINT32*)(MeCfg->SpiBar + MeCfg->SpiRegrFdoc);
    FDOD = (UINT32*)(MeCfg->SpiBar + MeCfg->SpiRegrFdod);

    *FDOC = 0;

    if( *FDOD != FlashDescriptorSig )
        return EFI_UNSUPPORTED;

    switch( Region )
    {
        //Mapping old definition to region number
        case PDR_BLK:
            EdsRegion = 4;
            break;

        case GBE_BLK:
            EdsRegion = 3;
            break;

        case ME_BLK:
            EdsRegion = 2;
            break;

        case ME_OPR_BLK:
            EdsRegion = 0xFF;
            break;

        case BIOS_BLK:
            EdsRegion = 1;
            break;

        case DER_BLK:
            EdsRegion = 5;
            break;
            
        case BIOS_2ND_BLK:
            EdsRegion = 6;
            break;

        default:
            EdsRegion = Region;
            break;
    }

    if( EdsRegion != 0xFF )
        *FDOC = (0x2000 + (EdsRegion * 0x04));
    else
        return EFI_UNSUPPORTED;

    Buffer32 = *FDOD;

    //If the region is unsued
    if( Buffer32 == 0x00007FFF )
        return EFI_UNSUPPORTED;

    RegionEnd = Buffer32 >> 16;
    RegionStart = Buffer32 & 0xFFFF;

    *Offset = RegionStart << 12;
    *Length = (RegionEnd - RegionStart + 1) << 12;
    
    if( (RegionEnd == 0) && Region )
    {
        *Length = 0;
        return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
