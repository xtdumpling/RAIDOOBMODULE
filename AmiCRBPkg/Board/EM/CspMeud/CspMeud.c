//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix : 1.Skip update the REGB(10 GbE) and REGF(PTT) region.
//              2.Enable IsEqualOrClean function to fix flash ME show error message.
//              3.Skip the FPTB region update.
//              PS.Follow Intel ME BIOS interface spec #548530, ME flash has below steps.
//                 a.Write a new recovery (FTPR) image to OPR1 partition.
//                 b.Write a new factory presets (MFSB) partition image into MFSB partition.
//                 c.Write factory data partitions in Intel ME region that need to be updated
//                   (MFS, IVB1, IVB2, FLOG, UTOK) with their new images.
//                 e.Write new operational Intel Server Platform Services firmware image in operational partition OPR1.
//    Reason  :
//    Auditor : Jimmy Chiu
//    Date    : Feb/09/2017
//
//  Rev. 1.02
//    Bug Fix : Fixed ME can't be updated when FDT region is same.
//    Reason  : Refer Greenlow code base, skip the checking function IsEqualOrClean, spsFITC lock read access, no need to check
//    Auditor : Jimmy Chiu
//    Date    : Nov/01/2016
//
//  Rev. 1.01
//    Bug Fix : Skip update GBE/GBEA/GBEB region to avoid onboard GBE MAC address be update to default value when flash BIOS/ME.
//    Reason  : Don't update GBE region, whe we use AFUEFI to flash ME region.
//    Auditor : Jimmy Chiu
//    Date    : Aug/26/2016
//
//  Rev. 1.00
//    Bug Fix : Add SMC feature - JPME2_Jumpless_SUPPORT
//    Reason  : It is one of SMC features.
//    Auditor : Jimmy Chiu
//    Date    : Aug/01/2016
//
//****************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file CspMeud.c

    Meud related functions.
*/
#include <Uefi.h>
#include "Token.h"

#include <Protocol/SmmSwDispatch.h>

#include <Protocol/HeciSmm.h>
#include <Protocol/SpsSmmHmrfpoProtocol.h>

#include <Library/PciLib.h>

#include <Library/PchPcrLib.h>

#include <Sps.h>

#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Flash.h>
#include <Meud.h>

#include "CspMeud.h"

extern volatile UINT8   *gSPIBASE;

HECI_PROTOCOL           *SmmHeci = NULL;
SPS_SMM_HMRFPO_PROTOCOL *mSpsSmmHmrfpo = NULL;

UINT8   MacAddr[6];
UINT8   HMRFPO_STATUS;

EFI_PHYSICAL_ADDRESS   Phy_Address;
EFI_PHYSICAL_ADDRESS   OPRx_Buffer;
OFBD_TC_55_ME_PROCESS_STRUCT  *StructPtr;

UINT8   gMeStateBeforeFlashUpdate = 0xFF;

// APTIOV_SERVER_OVERRIDE_START : Me update changes.
UINT64  gNonce = 0;

#define SPS_INIT_TIMEOUT        2000000 // [us] Timeout when waiting for InitComplete
#define SPS_RESET_TIMEOUT       5000000
#define SPS_NORMAL_TIMEOUT		2000000
#define STALL_1US               1
#define STALL_1MS               1000
#define STALL_1S                1000000

UINTN
EFIAPI
MicroSecondDelay (
  IN      UINTN                     MicroSeconds
  );
// APTIOV_SERVER_OVERRIDE_END : Me update changes.

FLASH_REGIONS_DESCRIPTOR
FlashRegionsDescriptor[] =
{
    { FDT_BLK, "FDR" },
    { BIOS_BLK, "BIOS" },
    { GBE_BLK, "GBER" },
    { PDR_BLK, "PDR" },
    { DER_BLK, "DER" },
    { BIOS_2ND_BLK, "BIOS" },
    { REG7_BLK, "REG7" },
    { REG8_BLK, "BMC" },
    { REG9_BLK, "DER2" },
    { REGA_BLK, "IE" },
    { REGB_BLK, "GBEA" },
    { REGC_BLK, "REGC" },
    { REGD_BLK, "REGD" },
    { REGE_BLK, "REGE" },
    { REGF_BLK, "PTT" },
    { ME_BLK, "MER" },
    { MAX_BLK, "" }
};

/**
    Check Heci PCI device

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN
IsHeciHidden(
    IN  VOID
)
{
    UINT32  D22F0FunctionDisable;

    PchPcrRead32(
        PID_PSF1,
        R_PCH_H_PCR_PSF1_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
        &D22F0FunctionDisable );
    return (D22F0FunctionDisable & B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);
}
/**
    UnHide Heci PCI device

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN
UnHideHeci(
    IN  VOID 
)
{
    if( IsHeciHidden() )
    {
        PchPcrAndThenOr32(
            PID_PSF1,
            R_PCH_H_PCR_PSF1_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
            (UINT8)~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS,
            0 );
        return TRUE;
    }
    else
        return FALSE;
}
/**
    Hide Heci PCI device

    @param HeciHide

    @retval VOID
**/
VOID
HideHeci(
    IN  BOOLEAN HeciHide
)
{
    if( HeciHide )
    {
        PchPcrAndThenOr32(
            PID_PSF1,
            R_PCH_H_PCR_PSF1_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
            (UINT8)~0,
            B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS );
    }
}

// APTIOV_SERVER_OVERRIDE_START: Get Nonce Value for ME flash operations
/**
    Get Nonce Value for ME flash operations
    @param  VOID
    @retval EFI_STATUS    - Status of Locate Protocol
            EFI_NOT_FOUND - Nonce is zero
    
**/
EFI_STATUS
GetNonceFromSpsSmm ( 
   VOID ) {

    if ( gNonce == 0 ) {
         gNonce = mSpsSmmHmrfpo->GetNonce();
         if ( gNonce == 0 ) {
              DEBUG ((EFI_D_ERROR, "Nonce is zero! Returning from GetNonceFromSpsSmm()!\n"));
              return EFI_NOT_FOUND;
         }
    }
    return EFI_SUCCESS;
}
// APTIOV_SERVER_OVERRIDE_END: Get Nonce Value for ME flash operations

/**
    Get Host Firmware Status pass to MEUD

    @param VOID

    @retval UINT32
**/
UINT32
GetHfs(
    IN  VOID
)
{
    UINT32  Hfs = 0;
    BOOLEAN HeciIsHide;

    HeciIsHide = UnHideHeci();

    Hfs = HeciPciRead32( 0x40 );
    if( Hfs == 0xFFFFFFFF )
        Hfs = 0;

    HideHeci( HeciIsHide );

    return Hfs;
}

#if JPME2_Jumpless_SUPPORT
UINT32 GetHFS2(VOID)
{
    UINT32  R_HFS2 = 0;
//AptioV Server Override Start: #RCBA - RCBA is obselete from Lewisburg PCH onwards
    BOOLEAN HeciIsHide;
	    
    HeciIsHide = UnHideHeci ();
//AptioV Server Override End: #RCBA - RCBA is obselete from Lewisburg PCH onwards

    R_HFS2 = HeciPciRead32(0x48);
    if(R_HFS2 == 0xFFFFFFFF)
        R_HFS2 = 0;

    HideHeci ( HeciIsHide ); //AptioV Server Override: #RCBA - RCBA is obselete from Lewisburg PCH onwards


    return R_HFS2;
}
#endif

/**
    Send HMRFPO MERESET by SpsSmmHmrfpoProtocol

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS
HmrfpoMeReset(
    IN  VOID
)
{
    EFI_STATUS  Status;
    BOOLEAN     HeciIsHide;

	// APTIOV_SERVER_OVERRIDE_START : ME Update Changes
    SPS_MEFS1   MeFs1;
    UINT32      Timeout;
    MKHI_MSG_HMRFPO_MERESET_REQ	HmrfpoMeResetReq;
	// APTIOV_SERVER_OVERRIDE_END : ME Update Changes

    if( mSpsSmmHmrfpo == NULL )
        return EFI_DEVICE_ERROR;

    HeciIsHide = UnHideHeci();

    // APTIOV_SERVER_OVERRIDE_START : ME Update Changes
    #if 0
    Status = mSpsSmmHmrfpo->MeReset();
    #endif

	//
	// Get nonce from Sps Smm Hmrfpo protocol
	//
    Status = GetNonceFromSpsSmm();
    if ( EFI_ERROR(Status) ) {
    	DEBUG ((EFI_D_ERROR, "Unable to get Nonce. Returning from HMRFPO_ENABLE_MSG!\n")); 
    	return Status;
    }

    HmrfpoMeResetReq.Mkhi.Data = 0x00000005;
    HmrfpoMeResetReq.Nonce = gNonce;

    Status = SmmHeci->SendMsg(HECI1_DEVICE, (UINT32*)&HmrfpoMeResetReq, sizeof(HmrfpoMeResetReq),
                                          SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);

    //
    // If Intel ME was not running in operational mode before 
    // flash update, return success.
    //        
    MeFs1.UInt32 = GetHfs();
    if ( gMeStateBeforeFlashUpdate == MeFs1.Bits.CurrentState ) {
        Status = EFI_SUCCESS;
        goto MERESET_END;
    }

    //
    // From ME-BIOS specification:
    // HMRFPO_MERESET message triggers Intel ME-only reset. The reset occurs immediately after the
    // HMRFPO_MERESET response is written to HECI, so if there was no error and the 
    // request has been accepted it is possible that BIOS will not have enough time to read 
    // the response. It is crucial to not repeat the request in such a case. MEFS1 register 
    // should be observed to confirm Intel ME is re-initializing.
    // So check for MEFS1.CurrentState to confirm that ME is re-initializing.
    // ME state transition: RECOVERY -> RESET -> INIT -> OPERATIONAL
    Timeout = SPS_RESET_TIMEOUT / STALL_1MS;
    MeFs1.UInt32 = GetHfs();
        
    while ( MeFs1.Bits.CurrentState != MEFS1_CURSTATE_INIT ) {
        if (!Timeout) {
            Status = EFI_DEVICE_ERROR;
            goto MERESET_END;
        } else if ( gMeStateBeforeFlashUpdate == MeFs1.Bits.CurrentState ) {
        	Status = EFI_SUCCESS;
        	goto MERESET_END;
        }

        MicroSecondDelay(STALL_1MS);
        MeFs1.UInt32 = GetHfs();
        Timeout--;
    }
        
    //
    // Wait for ME init complete. Timeout is 2 seconds.
    //
    Timeout = SPS_INIT_TIMEOUT / STALL_1MS;
    MeFs1.UInt32 = GetHfs();
    while ( !MeFs1.Bits.InitComplete ) {
        if (!Timeout) {
            Status = EFI_DEVICE_ERROR;
            goto MERESET_END;
        } else if ( gMeStateBeforeFlashUpdate == MeFs1.Bits.CurrentState ) {
        	Status = EFI_SUCCESS;
        	goto MERESET_END;
        }

        MicroSecondDelay(STALL_1MS);
        MeFs1.UInt32 = GetHfs();
        Timeout--;
    }
    //
    // Now, ME should be in normal operational State.
    //
        
    Timeout = SPS_NORMAL_TIMEOUT / STALL_1MS;
    MeFs1.UInt32 = GetHfs();
    while ( MeFs1.Bits.CurrentState != MEFS1_CURSTATE_NORMAL ) {
    	if (!Timeout) {
    		Status = EFI_DEVICE_ERROR;
    		goto MERESET_END;
    	} 
        	
    	MicroSecondDelay(STALL_1MS);
    	MeFs1.UInt32 = GetHfs();
    	Timeout--;
    }

MERESET_END:  
// APTIOV_SERVER_OVERRIDE_END : Me update changes.      

    HideHeci( HeciIsHide );

    return Status;
}
/**
    Send HMRFPO ENABLE by SpsSmmHmrfpoProtocol

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS
HmrfpoEnableMsg(
    IN  VOID
)
{
    EFI_STATUS  Status;
    BOOLEAN     HeciIsHide;

    if( mSpsSmmHmrfpo == NULL )
        return EFI_DEVICE_ERROR;

    HeciIsHide = UnHideHeci();

    Status = mSpsSmmHmrfpo->Enable();

    HideHeci( HeciIsHide );

    return Status;
}
/**
    Send HMRFPO LOCK by SpsSmmHmrfpoProtocol

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS
HmrfpoLockMsg(
    IN  VOID
)
{
    EFI_STATUS  Status;
    BOOLEAN     HeciIsHide;

    if( mSpsSmmHmrfpo == NULL )
        return EFI_DEVICE_ERROR;

    HeciIsHide = UnHideHeci();

    Status = mSpsSmmHmrfpo->Lock();

    HideHeci( HeciIsHide );

    return Status;
}
/**
    Mapping AMI defined region type to EDS defined

    @param AmiRegDef

    @retval UINT8
**/
UINT8
RegionMapping(
    IN  UINT8   AmiRegDef
)
{
    UINT8 EdsRegion;

    switch( AmiRegDef )
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
            //This type is only defined for SPS
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
            EdsRegion = AmiRegDef;
        break;
    }

    return EdsRegion;
}
/**
    Find index of FlashRegionsDescriptor

    @param AmiRegDef AMI defined region type

    @retval UINT8 Return MAX_BLK if not found match index
**/
UINT8
FindDescriptorIndex(
    IN  UINT8   AmiRegDef
)
{
    UINT8   Index;
    UINT8   Regions;

    Regions = sizeof(FlashRegionsDescriptor) / sizeof(FLASH_REGIONS_DESCRIPTOR);
    for( Index = 0; Index < Regions; Index++ )
    {
#if SMCPKG_SUPPORT
        if( (AmiRegDef == REGB_BLK) || (AmiRegDef == REGF_BLK) )
            break;
#endif

        if( FlashRegionsDescriptor[Index].FlashRegion == AmiRegDef )
            return Index;
    }

    return MAX_BLK;
}
/**
    Read the FLMSTR1 in descriptor for checking write access

    @param Region Region type

    @retval BOOLEAN TRUE region can be written, FALSE region can't be written
**/
BOOLEAN
CheckRegionWriteAccess(
    IN  UINT8   Region
)
{
    static UINT32   Flmstr1 = 0;
    UINT32          Value32;
    UINT8           *Address;
    UINT8           EdsRegion;
    UINT8           Shift;

    //If FDOPSS bit in SPI address is not set, the access is accepted
    if( ((*(UINT32*)(gSPIBASE + 0x04)) & BIT13) == 0 )
        return TRUE;

    if( Flmstr1 == 0 )
    {
        //Read FLMAP1 for Fmba
        Address = (UINT8*)FLASH_BASE_ADDRESS(0x18);
        FlashRead( Address, (UINT8*)&Value32, sizeof(UINT32) );

        //Read FLMSTR1 for write access
        Address = (UINT8*)FLASH_BASE_ADDRESS(((Value32 & (0x000000FF)) << 0x04));
        FlashRead( Address, (UINT8*)&Value32, sizeof(UINT32) );

        Flmstr1 = Value32;
    }
    else
    {
        //Since the descriptor region may be changed in the update process,
        //it should read onboard FLMSTR1 only once.
        Value32 = Flmstr1;
    }

// APTIOV_SERVER_OVERRIDE_START : Implement MEUD for ME11
#if defined(WORKSTATION_SUPPORT) && (WORKSTATION_SUPPORT == 0)
    //Me region register should be locked, but we will call HMRFPO to unlock
    //just retur true here.
    if( Region == ME_BLK )
        return TRUE;
#else
    //If onboard is ME firmware, check the HFSTS
    if( Region == ME_BLK )
    {
        //Get HFSTS
        Value32 = GetHfs();
        //bit [16:19]
        Value32 &= (BIT16 | BIT17 | BIT18 | BIT19);
        Value32 = Value32 >> 0x10; 
        switch( Value32 )
        {
            case 0x03: //Soft Temporary Disable
            case 0x04: //SECOVER_JMPR
            case 0x05: //SECOVER_MEI_MSG
                return TRUE;
            break;

            default:
                return FALSE;
            break;
        }
    }
#endif
// APTIOV_SERVER_OVERRIDE_END : Implement MEUD for ME11

    EdsRegion = RegionMapping( Region );

    if( (EdsRegion == 0xFF) || (EdsRegion == MAX_BLK) )
        return TRUE;
    
    if( EdsRegion >= 0x0C )
        Shift = EdsRegion - 0x08; //Region 12 to 15 is bit[4:7]
    else
        Shift = EdsRegion + 0x14; //Region 0 to 11 is bit[20:31]

    return (((Value32 >> Shift) & 0x01) == 0x01) ? TRUE : FALSE;
}
/**
    Compare the FREG register from SPI base and FLREG register
    in update image to determin wheter region is changed.
    If this function checks descriptor region, it will check all
    FREG and FLREG registers in order to avoiding region layout
    change.

    @param Region Region type

    @retval BOOLEAN TRUE region range is different, FALSE region range is same
**/
BOOLEAN
RegionRangeChanged(
    IN  UINT8   Region
)
{
    UINT8   EdsRegion;
    UINT8   Limit;
    UINT32  Offset;
    UINT32  Freg;
    UINT32  Flreg;

    EdsRegion = RegionMapping( Region );

    //Undefined region or OPR just assume not changed
    if( (EdsRegion == 0xFF) || (EdsRegion == MAX_BLK) )
        return FALSE;

    if( Region == FDT_BLK )
        Limit = 0x10;  //Check all registers if it's descriptor
    else
        Limit = EdsRegion + 0x01;  //Only check single region register

    for( EdsRegion; EdsRegion < Limit; EdsRegion++ )
    {
        //Get on-board FREG from SPI base
        if( EdsRegion >= 0x0C )
            //Region 12 to 15 is start from offset 0xE0
            Offset = 0xE0 + ((EdsRegion - 0x0C) * sizeof(UINT32));
        else
            //Region 0 to 11 is start from offset 0x54
            Offset = 0x54 + EdsRegion * sizeof(UINT32);

        Freg = *(UINT32*)(gSPIBASE + Offset);

        //Get FLREG from image's descriptor region
        Flreg = *(UINT32*)(Phy_Address + 0x40 + (EdsRegion * sizeof(UINT32)));

        if( Freg != Flreg )
            return TRUE;
    }

    return FALSE;
}
/**
    Compare the on-board FPT partition in ME region with update image's.

    @param MeRegion ME region in memory buffer
    @param IsIdentical Return ture if content is identical

    @retval EFI_STATUS
**/
EFI_STATUS
CheckFptPartition
(
    IN  VOID    *MeRegion,
    OUT BOOLEAN *IsIdentical
)
{
    EFI_STATUS  Status;
    UINTN       BufferSize;
    VOID        *SmmBuffer;
    UINT32      Offset;
    UINT32      Length;
    UINT8       *Address;

    //Send hmrfpo enable to get read access to ME region
    Status = HmrfpoEnableMsg();

    BufferSize = FPT_LENGTH;
    Status = pSmst->SmmAllocatePool(
                        EfiRuntimeServicesData,
                        BufferSize,
                        &SmmBuffer );
    if( EFI_ERROR(Status) )
    {
        HmrfpoLockMsg();
        return Status;
    }

    Status = GetRegionOffset( ME_BLK, &Offset, &Length );
    if( EFI_ERROR(Status) )
    {
        HmrfpoLockMsg();
        return Status;
    }

    //Read on-board FPT partition
    Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);
    FlashRead( Address, SmmBuffer, (UINT32)BufferSize );

    Length = (UINT32)MemCmp( MeRegion, SmmBuffer, BufferSize );

    Status = pSmst->SmmFreePool( SmmBuffer );

    if( Length != 0 )
        *IsIdentical = FALSE;
    else
        *IsIdentical = TRUE;

    Status = HmrfpoLockMsg();

    return EFI_SUCCESS;
}
/**
    Get regions' offset and length by FDOC register

    @param Region
    @param Offset
    @param Length

    @retval EFI_STATUS
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
    UINT8           EdsRegion;
    UINT32          Buffer32;
    UINT32          RegionStart;
    UINT32          RegionEnd;

    InitializeSpiBase();

    FDOC = (UINT32*)(gSPIBASE + 0xB4); 
    FDOD = (UINT32*)(gSPIBASE + 0xB8);
    *FDOC = 0;

    if( *FDOD != FLASH_VALID_SIGNATURE )
        return EFI_UNSUPPORTED;

#if SMCPKG_SUPPORT
    if( (StructPtr->bBlockType == REGB_BLK) || (StructPtr->bBlockType == REGF_BLK) )
        return EFI_NOT_FOUND;
#endif

    EdsRegion = RegionMapping( Region );

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

    return EFI_SUCCESS;
}
/**
    Get flash parts' capacity by FDOC register

    @param VOID

    @retval UINT32
**/
UINT32
GetFlashCapacity(
    IN  VOID
)
{
    volatile UINT32 *FDOC;
    volatile UINT32 *FDOD;
    UINT32          FlashDensity = 0;
    UINT16          Components;
    UINT8           i,j;

    InitializeSpiBase();

    FDOC = (UINT32*)(gSPIBASE + 0xB4);
    FDOD = (UINT32*)(gSPIBASE + 0xB8);

    *FDOC = 0;
    // Invalid !! Not Support ME
    if( *FDOD != FLASH_VALID_SIGNATURE )
        return 0;

    //Get flash part numbers
    *FDOC = 0x04;
    Components = (*FDOD >> 8) & 0x03;

    *FDOC = 0x1000;
    j = *FDOD;

    for( i = 0 ; i < ( Components + 1 ); i++ )
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
#if BITS_OF_SPI_DENSITY
        j = j >> 4;
#else
        j = j >> 3;
#endif
    }

    return FlashDensity;
}
/**
    Report ME Base address and Length to AFU

    @param Func_Num
    @param BASE_Address
    @param Length

    @retval EFI_STATUS
**/
EFI_STATUS
CspReportMeInfo
(
    IN      UINT8   Func_Num,
    IN  OUT UINT32  *BASE_Address,
    IN  OUT UINT32  *Length
)
{
    // Fill these 2 item for Afu can update BIOS with whole image input
    *BASE_Address = 0;
    *Length = GetFlashCapacity() - FLASH_SIZE;

    switch( Func_Num )
    {
        case 0:
            // Command 0 is for Ignition FW, only ME6 support this.
            return EFI_UNSUPPORTED;
        break;
        case 3:
            // Always rteturn SUCCESS here !!
        break;
        default:
            return EFI_UNSUPPORTED;
        break;
    }

    return EFI_SUCCESS;
}
/**
    Send Enable and Global reset MSG to ME FW.

    @param DispatchHandle
    @param DispatchContext

    @retval VOID
**/
VOID
EFIAPI
MEUDSMIHandler(
    IN  EFI_HANDLE                  DispatchHandle,
    IN  EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
)
{
    // This is not needed anymore, SPS can be disabled in runtime.
/*
    HmrfpoEnableMsg();
    SEND_MEI_GLOBAL_RESET_MSG();
*/
}
/**
    Locate Smm Heci protocol

    @param Protocol
    @param Interface
    @param Handle

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
LocateSmmHeciProtocol(
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
{
    EFI_STATUS  Status;

    Status = pSmst->SmmLocateProtocol(
                        &gSmmHeciProtocolGuid,
                        NULL,
                        &SmmHeci );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "Smm Heci Protocol locate Status: %r\n", Status));
    }

    return Status;
}
/**
    Locate Smm Hmrfpo protocol

    @param Protocol
    @param Interface
    @param Handle

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
LocateSmmHmrfpoProtocol(
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
{
    EFI_STATUS  Status;

    Status = pSmst->SmmLocateProtocol(
                        &gSpsSmmHmrfpoProtocolGuid,
                        NULL,
                        &mSpsSmmHmrfpo );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "Smm Hmrfpo locate Status: %r\n", Status));
    }

    return Status;
}
/**
    Elink in OFBD entry

    @param  VOID
    @retval VOID
**/
VOID
CspMeudInSmm(
    IN  VOID
)
{
    EFI_STATUS  Status;
    UINT8       Timer = 0;
    VOID        *ProtocolNotifyRegistration;

    //Locate the buffer for saving non-BIOS region image from AFU
    Status = pBS->AllocatePages(
                    AllocateAnyPages,
                    EfiRuntimeServicesData,
                    EFI_SIZE_TO_PAGES(RT_ME_BUFFER),
                    &Phy_Address );
    if( EFI_ERROR(Status) )
        return;

    //Locate the buffer for saving OPR partiton in ME region
    Status = pBS->AllocatePages(
                    AllocateAnyPages,
                    EfiRuntimeServicesData,
                    EFI_SIZE_TO_PAGES(RT_OPR_BUFFER),
                    &OPRx_Buffer );
    if( EFI_ERROR(Status) )
        return;

    Status = pSmst->SmmLocateProtocol(
                        &gSmmHeciProtocolGuid,
                        NULL,
                        &SmmHeci );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "Smm Heci Protocol locate Status: %r\n", Status));
        Status = pSmst->SmmRegisterProtocolNotify(
                            &gSmmHeciProtocolGuid,
                            LocateSmmHeciProtocol,
                            &ProtocolNotifyRegistration );
    }

    Status = pSmst->SmmLocateProtocol(
                        &gSpsSmmHmrfpoProtocolGuid,
                        NULL,
                        &mSpsSmmHmrfpo );
    if( EFI_ERROR(Status) )
    {
        ProtocolNotifyRegistration = NULL;
        Status = pSmst->SmmRegisterProtocolNotify(
                            &gSpsSmmHmrfpoProtocolGuid,
                            LocateSmmHmrfpoProtocol,
                            &ProtocolNotifyRegistration );
    }

    return;
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
    MemCpy( (UINT8*)(StructPtr->ddMessageBuffer), Message, Strlen(Message) );

    *(CHAR8*)(StructPtr->ddMessageBuffer + Strlen(Message)) = 0;
}
/**
    Send Hmrfpo enable message

    @param InSmm

    @retval EFI_STATUS
**/
EFI_STATUS
SendEnableMessage(
    IN  BOOLEAN InSmm
)
{
    UINT8       Counter;
    EFI_STATUS  Status;

    if( gMeStateBeforeFlashUpdate == 0xFF )
    {
        gMeStateBeforeFlashUpdate = (0x0F & GetHfs());
    }

    if( InSmm )
    {
        Counter = 0;
        do
        {
            Status = HmrfpoEnableMsg();
            if( !EFI_ERROR(Status) )
                break;
            Counter++;
            SmmHeci->ReInitHeci( HECI1_DEVICE );
        }while( Counter < 3 );
        GetHfs();
    }
    else
    {
        UINTN       VariableSize = 1;
        EFI_GUID    gMEUDErrorguid = MEUD_ERROR_GUID;

        IoWrite8( 0xB2, Disable_ME_SW_SMI );

        // Get Status
        Status = pRS->GetVariable(
                        L"ENABLESTATUS",
                        &gMEUDErrorguid,
                        NULL,
                        &VariableSize,
                        &HMRFPO_STATUS );

        // Error, if variable not exist
        if( Status == EFI_NOT_FOUND )
            return Status;

        return EFI_SUCCESS;
    }

    return Status;
}
/**
    Init the Length and Offset need to be updated. If needed, send ENABLE MESSAGE

    @param ProgramOffset
    @param ProgramLength
    @param Step
    @param InSmm

    @retval EFI_STATUS
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
    UINT32      Offset;
    UINT32      Length;
    UINT32      Hfs = GetHfs();

    switch( StructPtr->bBlockType )
    {
        case ME_BLK:
// APTIOV_SERVER_OVERRIDE_START : Implement MEUD for ME11
#if defined(WORKSTATION_SUPPORT) && (WORKSTATION_SUPPORT == 0)
            if( !(Hfs & BIT09) )
            {
                MEProcessHandleResult(
                    BIT03,
                    "ME FW not Initial done" );

                return EFI_NOT_READY;
            }
#endif
// APTIOV_SERVER_OVERRIDE_END : Implement MEUD for ME11
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            //If Fpt bad or recovery load fault
            if((Hfs & BIT05) || (Hfs & BIT10))
                *Step = UpdateMfsb;
            else
                *Step = UpdateFtprToOpr;
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

                FlashRead( Address, MacAddr, 0x06 );
            }
            *Step = UpdateNonMeRegion;
        break;

        case REGB_BLK:
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            if( Status == EFI_NOT_FOUND )
                return EFI_UNSUPPORTED;

            if( Length )
            {
                UINT8   *Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);
                Process10GbeMac( Address, Save10GbeMac );
            }
            *Step = UpdateNonMeRegion;
        break;

        case ME_OPR_BLK:
        {
            if( (Hfs & 0xF) != 2 )
            {
                Status = SendEnableMessage(InSmm);
                if( EFI_ERROR(Status) || (HMRFPO_STATUS != 0) )
                {
                    MEProcessHandleResult(
                        (BIT03 | BIT00),
                        "Error when sending Enable Message to ME !!" );
                    return Status;
                }
            }
            Offset = OPR1_START;
            if( OPR2_LENGTH )
            {
/*
                // If dual image system
                // 3.8.2.3
                if( !InSmm )
                {
                    // Update Factory_Limit.
                    UINTN   VariableSize = sizeof(UINT32);

                    Status = pRS->GetVariable(
                                    L"Factory_Limit",
                                    &gSmmHeciProtocolGuid,
                                    NULL,
                                    &VariableSize,
                                    &Factory_Limit );
                    if( EFI_ERROR(Status) )
                        Factory_Limit = 0;
                }
                if( Factory_Limit == (FPT_LENGTH + MFSB_LENGTH + FTPR_LENGTH +
                                     MFS_LENGTH + OPR1_LENGTH) )
                    Offset = OPR2_START;
*/
            }
            Length = OPR1_LENGTH;
            *Step = UpdateNonMeRegion;
        }
        break;
        
        default:
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            if( EFI_ERROR(Status) )
                return EFI_UNSUPPORTED;
            *Step = UpdateNonMeRegion;
        break;
    }

    *ProgramOffset = Offset;
    *ProgramLength = Length;

    return EFI_SUCCESS;
}
/**
    Handle Program Length and Offset , Steps and Program Buffer

    @param ProgramOffset
    @param ProgramLength
    @param Step
    @param InSmm
    @param ProgramBuffer

    @retval EFI_STATUS
**/
EFI_STATUS
HandleMEUpdateSteps(
    IN  OUT UINT32  *ProgramOffset,
    IN  OUT UINT32  *ProgramLength,
    IN      UINT8   Step,
    IN      BOOLEAN InSmm,
    IN  OUT UINT8   **ProgramBuffer
)
{
    static BOOLEAN          SmmPages = FALSE;
    EFI_PHYSICAL_ADDRESS    SmmBuffer;
    static UINTN            BufferPages;
    EFI_STATUS              Status;
    UINT32                  Offset = 0;
    UINT32                  Length = 0;

    if( Step == UpdateFtprToOpr )
    {
        //Program FTPR to OPR1 and OPR2
        //Send Enable to ME
        //Retry 3 times for Enable message.
        Status = SendEnableMessage( InSmm );
        if( EFI_ERROR(Status) || (HMRFPO_STATUS != 0) )
        {
            MEProcessHandleResult(
                (BIT03 | BIT02 | BIT00),
                "Error when sending Enable Message to ME !!" );
            return Status;
        }

        // Prepare buffer, the length and offset is defferent
        if( OPR2_LENGTH )
            BufferPages = EFI_SIZE_TO_PAGES(OPR1_LENGTH + OPR2_LENGTH);
        else
            BufferPages = EFI_SIZE_TO_PAGES(OPR1_LENGTH);

        if( InSmm && !SmmPages )
        {
            Status = pSmst->SmmAllocatePages(
                                AllocateAnyPages,
                                EfiRuntimeServicesData,
                                BufferPages,
                                &SmmBuffer );
            if( !EFI_ERROR(Status) )
            {
                SmmPages = TRUE;
                OPRx_Buffer = SmmBuffer;
            }
        }

        MemSet( (UINT8*)(OPRx_Buffer), EFI_PAGES_TO_SIZE(BufferPages), 0xFF );

        MemCpy(
            (UINT8*)(OPRx_Buffer),
            (UINT8*)(Phy_Address + FTPR_START),
            FTPR_LENGTH );

        // If OPR2 exist
        if( OPR2_LENGTH )
        {
            MemCpy(
                (UINT8*)(OPRx_Buffer + OPR1_LENGTH),
                (UINT8*)(Phy_Address + FTPR_START),
                FTPR_LENGTH );
        }

        // Update Buffer ,Length and Offset
        *ProgramBuffer = (UINT8*)OPRx_Buffer;

        Offset = OPR1_START;

        if( OPR2_LENGTH )
            Length = OPR1_LENGTH + OPR2_LENGTH;
        else
            Length = OPR1_LENGTH;
    }

    if( Step == UpdateMfsb )
    {
        //Update Mfsb
        Length = MFSB_LENGTH;
        Offset = MFSB_START;
        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    if( Step == UpdateFtpr )
    {
        //Update Ftpr
        Length = FTPR_LENGTH;
        Offset = FTPR_START;
        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    //Update factory data partitions
    if( Step == UpdateMfs )
    {
        // Free the memory we allocate last time.
        if( SmmPages )
        {
            pSmst->SmmFreePages( OPRx_Buffer, BufferPages );
            SmmPages = FALSE;
        }

        // Enable again !!
        Status = SendEnableMessage( InSmm );

        if( EFI_ERROR(Status) || (HMRFPO_STATUS != 0) )
        {
            MEProcessHandleResult(
                (BIT03 | BIT02 | BIT00),
                "Error when sending Enable Message to ME !!");
            if( !EFI_ERROR(Status) )
                Status = EFI_WRITE_PROTECTED;
            return Status;
        }

        Offset = MFS_START;

        // Just take care of length
        Length = MFS_LENGTH;

#if defined(IVB1_LENGTH)
        Length += (UINT32)IVB1_LENGTH;
#endif
#if defined(IVB2_LENGTH)
        Length += (UINT32)IVB2_LENGTH;
#endif

        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    if( Step == UpdateFlog )
    {
        //Update FLOG and UTOK
        Offset = FLOG_START;
        Length = FLOG_LENGTH;

#if defined(UTOK_LENGTH)
        Length += (UINT32)UTOK_LENGTH;
#endif
        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    //Update OPR1 and OPR2
    if( Step == UpdateOpr )
    {
        // Take care of length
        if( OPR2_LENGTH )
            Length = OPR1_LENGTH + OPR2_LENGTH;
        else
            Length = OPR1_LENGTH;

        Offset = OPR1_START;
        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    *ProgramOffset = Offset;
    *ProgramLength = Length;

    return EFI_SUCCESS;

}
/**
    Verifies if the device has been erased properly or if the current
    byte is the same as the byte to be written at this location

    @param pDestination Starting address of where the data will be written
    @param pSource Starting address of the data that is supposed to be written 
                    or "NULL" for checking erase completed
    @param Length Length of the data set to check

    @retval BOOLEAN
**/
static
BOOLEAN
IsEqualOrClean(
    IN  UINT8   *pDestination,
    IN  UINT8   *pSource,
    IN  UINT32  Length
)
{
    UINT8   Buffer[256];
    UINT8   Data;
    UINT32  TempLength = Length;
    UINT32  RemainBytes = Length;
    UINT32  i, j;

#if JPME2_Jumpless_SUPPORT
    //return TRUE;    //spsFITC lock read access, no need to check
#endif

    for( i = 0; i < Length; TempLength = RemainBytes, i = (Length - RemainBytes) )
    {
        // Initial local read buffer.
        MemSet( Buffer, 256, 0 );
        // Read flash through SPI Cycle if SpiCycle is requested.
        FlashRead( pDestination + i, Buffer, 256 );
        RemainBytes -= 256;
        // Verify whether flash contents is the same as written Data or is clean.
        for( j = 0; j < (TempLength - RemainBytes); j++ )
        {
            Data = (pSource != NULL) ? *(pSource + i + j) : 0xFF;
            if( Buffer[j] != Data )
                return FALSE;    
        }
    }
    return TRUE;
}

#if JPME2_Jumpless_SUPPORT

BOOLEAN
MeIsInManufacturingMode (
  VOID
  )
{
  SPS_MEFS1  MeFs1;
  SPS_MEFS2  MeFs2;
  
  //
  // Note: We are checking if ME is in Recovery Mode actually.
  //       This follows the current status of Purley CRB.
  //       This may change in the future.
  //
  
  MeFs1.UInt32 = GetHfs();
  if (MeFs1.Bits.CurrentState != MEFS1_CURSTATE_RECOVERY) return FALSE;
  MeFs2.UInt32 = GetHFS2();
  if (MeFs2.Bits.RecoveryCause != MEFS2_RCAUSE_MFGJMPR) return FALSE;
  
  return TRUE;
}
#endif

/**
    Update region

    @param Buffer
    @param InSmm

    @retval EFI_STATUS
**/
EFI_STATUS
UpdateRegions(
    IN  UINT8   *Buffer,
    IN  BOOLEAN InSmm
)
{
#if JPME2_Jumpless_SUPPORT
    BOOLEAN                MeInManuMode = FALSE;
#endif
    static UINT32   Offset;
    static UINT32   Length;
    UINT8           *Address;
    EFI_STATUS      Status;
    BOOLEAN         NeedToVerify = FALSE;
    static UINT8    Step = UpdateNonMeRegion;
    static BOOLEAN  NewRegion;
    UINTN           Counter = 0;
    static UINT8    RetryTimes = RETRY_OPR_COUNT;
    static UINT8    *ProgramBuffer;
    static BOOLEAN  FptIsIdentical;
    static BOOLEAN  WriteAccess;
    static BOOLEAN  RegionChanged;
    UINT8           Index;
    UINT8           Messages[80];
    UINT16          Result;
#if JPME2_Jumpless_SUPPORT
    MeInManuMode = MeIsInManufacturingMode();
#endif

    // Prepare Offset and Length to be updated
    // If BIT02 , update buffer
    if( (StructPtr->bHandleRequest & BIT02) )
    {
        if( !InSmm )
        {
            (UINT8*)Phy_Address = Buffer;
            Status = pBS->AllocatePages(
                            AllocateAnyPages,
                            EfiRuntimeServicesData,
                            EFI_SIZE_TO_PAGES(RT_OPR_BUFFER),
                            &OPRx_Buffer );
        }

        Status = HandleBuffer( &Offset, &Length, &Step, InSmm );

        if( EFI_ERROR(Status) )
        {
            MEProcessHandleResult( BIT03, "UN SUPPORT TYPE" );
            return Status;
        }
        // Frist In
        NewRegion = TRUE;
        WriteAccess = FALSE;
        RegionChanged = FALSE;
        ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

#if SMCPKG_SUPPORT
    if((((StructPtr->bBlockType) == GBE_BLK) || ((StructPtr->bBlockType) == REGB_BLK) || ((StructPtr->bBlockType) == REGC_BLK)) && NewRegion)
    {
        return EFI_SUCCESS; // Skip the flash GBE region
    }
#else
    if( ((StructPtr->bBlockType) == GBE_BLK) && NewRegion )
    {
        UINT16  *GBEPtr;
        UINT16  i, CheckSum16;

        // Set MAC address to buffer
        MemCpy( (Buffer + Offset), MacAddr, 0x06 );

        //Calculate the checksum
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

    if(((StructPtr->bBlockType) == REGB_BLK) && NewRegion)
    {
        VOID    *Region10Gbe;
        Region10Gbe = (VOID*)(Buffer + Offset);
        Process10GbeMac( Region10Gbe, Restore10GbeMac );
    }
#endif

    // Handle Step
    if( (StructPtr->bBlockType == ME_BLK) && NewRegion )
    {
// APTIOV_SERVER_OVERRIDE_START : Implement MEUD for ME11
#if defined(WORKSTATION_SUPPORT) && (WORKSTATION_SUPPORT == 0)
        //Compare FPT partition only one time before update ME region
        if( (StructPtr->bHandleRequest & BIT02) )
        {
            VOID *RegionMe;

            RegionMe = (VOID*)(Buffer + Offset);
            Status = CheckFptPartition( RegionMe, &FptIsIdentical );
            if( EFI_ERROR(Status) )
                return Status;
        }

        if( FptIsIdentical == TRUE )
        {
            //FPT partion is identical with update image's, process safe udpate
            Status = HandleMEUpdateSteps(
                        &Offset,
                        &Length,
                        Step,
                        InSmm,
                        &ProgramBuffer );
            if( EFI_ERROR(Status) )
                return Status;
        }
        else
        {
            //FPT partition is changed from update image, process simple update
            //Just take ME regions as normal region so send HmrfpoEnable here
            Status = SendEnableMessage(InSmm);
            if( EFI_ERROR(Status) || (HMRFPO_STATUS != 0) )
            {
                MEProcessHandleResult(
                    (BIT03 | BIT00),
                    "Error when sending Enable Message to ME !!" );
                return Status;
            }
            //Set step to UpdateNonMeRegion in order to avoid extra steps
            Step = UpdateNonMeRegion;
        }
#else
        Step = UpdateNonMeRegion;
#endif
// APTIOV_SERVER_OVERRIDE_END : Implement MEUD for ME11
    }

    if( NewRegion )
    {
        NewRegion = FALSE;
        WriteAccess = CheckRegionWriteAccess( StructPtr->bBlockType );
        RegionChanged = RegionRangeChanged( StructPtr->bBlockType );
    }

    Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);

    if( WriteAccess == TRUE )
    {
        FlashBlockWriteEnable( Address );
        if( IsEqualOrClean( Address, NULL, FLASH_BLOCK_SIZE ) )
        {
            //The flash content is clean
            FlashProgram( Address, ProgramBuffer, FLASH_BLOCK_SIZE );
            if( IsEqualOrClean( Address, ProgramBuffer, FLASH_BLOCK_SIZE ) )
                Status = EFI_SUCCESS;
            else
                Status = EFI_DEVICE_ERROR;
        }
        else
        {
            //The flash content is not clean
            if( IsEqualOrClean( Address, ProgramBuffer, FLASH_BLOCK_SIZE ) )
            {
                //The flash content is same
                Status = EFI_SUCCESS;
            }
            else
            {
                //The flash content is different
                FlashEraseBlock( Address );
                FlashProgram( Address, ProgramBuffer, FLASH_BLOCK_SIZE );
                if( IsEqualOrClean( Address, ProgramBuffer, FLASH_BLOCK_SIZE ) )
                    Status = EFI_SUCCESS;
                else
                    Status = EFI_DEVICE_ERROR;
            }
        }
        FlashBlockWriteDisable( Address );
    }
    else
    {
        //The region is locked.
        Index = FindDescriptorIndex( StructPtr->bBlockType );

        if( RegionChanged == TRUE )
        {
            //The region range changed, there should be layout changed.
            //Force to stop whole update process.
            if( Index != MAX_BLK )
            {
                Sprintf(
                    Messages,
                    "%s is locked and layout changed, block updating",
                    FlashRegionsDescriptor[Index].Command );
            }
            else
                Sprintf( Messages, "There is locked region and layout changed, block updating.");

            //Show the message and block whole updating process
            MEProcessHandleResult( (BIT00 | BIT02), Messages );
        }
        else
        {
#if SMCPKG_SUPPORT && JPME2_Jumpless_SUPPORT
            if( Index != MAX_BLK ) {
                if ((StructPtr->bBlockType == FDT_BLK) && !MeInManuMode)
                    Sprintf( Messages, "Update skipped for %s because ME is not in Manufacturing Mode.\n   Update continues...", FlashRegionsDescriptor[Index].Command);
           }
#else
            if( Index != MAX_BLK )
                Sprintf( Messages, "%s is locked, skip updating.", FlashRegionsDescriptor[Index].Command );
            else
                Sprintf( Messages, "There is locked region, skip updating.");
#endif

            //Show the message and just return the success bit to AFU in
            //order to updating other regions. If needs to block whole updating
            //process, change to set (BIT00 | BIT02)
            MEProcessHandleResult( (BIT03 | BIT02), Messages );
        }
        return EFI_SUCCESS;
    }

    ProgramBuffer = ProgramBuffer + FLASH_BLOCK_SIZE;
    Length -= FLASH_BLOCK_SIZE;
    Offset += FLASH_BLOCK_SIZE;

    // End of Region Update
    if( !EFI_ERROR(Status) )
    {
        if( Length == 0 )
        {
            NewRegion = TRUE;
// APTIOV_SERVER_OVERRIDE_START : Implement MEUD for ME11
#if defined(WORKSTATION_SUPPORT) && (WORKSTATION_SUPPORT == 0)
            // Only when Updating MER, the Step is lnot 0.
#if CHECK_FPTR_IN_OPR
            if( (Step == UpdateFtprToOpr) ||
                (Step == UpdateOpr) || (StructPtr->bBlockType == ME_OPR_BLK) )
#else
            if( (Step == UpdateOpr) || (StructPtr->bBlockType == ME_OPR_BLK) )
#endif
                NeedToVerify = TRUE;

            //It's simple update process
            if( (Step == UpdateNonMeRegion) && (StructPtr->bBlockType == ME_BLK) )
                NeedToVerify = TRUE;
#endif
// APTIOV_SERVER_OVERRIDE_END : Implement MEUD for ME11
        }
        else
        {
            // OEM can output message here in every block updated.
            // Remember to Set BIT02
            MEProcessHandleResult( (BIT01), " " );
            return EFI_SUCCESS;
        }

        if( NeedToVerify )
        {
            if( InSmm )
            {
                Counter = 0;
                do
                {
                    Status = HmrfpoLockMsg();
                    if( !EFI_ERROR(Status) )
                        break;
                    Counter++;
                    SmmHeci->ReInitHeci( HECI1_DEVICE );
                }while( Counter < 3 );
            }
            else
            {
                EFI_GUID   MEUDErrorguid = MEUD_ERROR_GUID;
                UINTN      VarianleSize;
                pRS->SetVariable(
                        L"LOCKMSG",
                        &MEUDErrorguid,
                        (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS),
                        sizeof(UINTN),
                        &Counter );

                IoWrite8( SW_SMI_IO_ADDRESS, Disable_ME_SW_SMI );

                VarianleSize = sizeof(UINT8);

                Status = pRS->GetVariable(
                                L"LOCKSTATUS",
                                &MEUDErrorguid,
                                NULL,
                                &VarianleSize,
                                &HMRFPO_STATUS );

                // Delete Flag
                pRS->SetVariable(
                        L"LOCKSTATUS",
                        &MEUDErrorguid,
                        (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS),
                        0,
                        &HMRFPO_STATUS );
            }

            // If success.
            if( !EFI_ERROR(Status) )
            {
                // Check Status
                if( HMRFPO_STATUS == 2 )
                {
                    MEProcessHandleResult( (BIT01 | BIT02), "Warming : HMRFPO_LOCK_NOT_VALID !!");
                    // Retry
                    if( RetryTimes > 0 )
                    {
                        Step = UpdateFtprToOpr;
                        RetryTimes -= 1;
                        return EFI_SUCCESS;
                    }
                }
//                else if( Step == UpdateOpr )
//                {
                    // Send MERESET Message.
                    Status = HmrfpoMeReset();
//                }
            }
        }
        else
            Status = EFI_SUCCESS;
    }

    // Show Strings
    if( !EFI_ERROR(Status) )
    {
        switch( Step )
        {
            case UpdateFtprToOpr:
                Result = (BIT01 | BIT02);
                Sprintf( Messages, "Successful Update Recovery Loader to OPRx!!" );
            break;

            case UpdateMfsb:
                Result = (BIT01 | BIT02);
                Sprintf( Messages, "Successful Update MFSB!!" );
            break;

            case UpdateFtpr:
                Result = (BIT01 | BIT02);
                Sprintf( Messages, "Successful Update FTPR!!" );
            break;

            case UpdateMfs:
                Result = (BIT01 | BIT02);
                Sprintf( Messages, "Successful Update MFS, IVB1 and IVB2!!" );
            break;

            case UpdateFlog:
                Result = (BIT01 | BIT02);
                Sprintf( Messages, "Successful Update FLOG and UTOK!!" );
            break;

            case UpdateOpr:
                Result = (BIT03 | BIT02);
                Sprintf( Messages, "ME Entire Image update success !!" );
            break;

            default:
            {
                Result = (BIT03 | BIT02);
                Index = FindDescriptorIndex( StructPtr->bBlockType );
                if( Index != MAX_BLK )
#if JPME2_Jumpless_SUPPORT
                    if (StructPtr->bBlockType == FDT_BLK && !MeInManuMode)
                    {
                      Sprintf( Messages, "Update skipped for %s because ME is not in Manufacturing Mode.\nUpdate continues...", FlashRegionsDescriptor[Index].Command);
                    } else {
                      Sprintf( Messages, "Update success for %s", FlashRegionsDescriptor[Index].Command);
                    }
#else
                    Sprintf( Messages, "Update success for %s", FlashRegionsDescriptor[Index].Command);
#endif
                else
                    Sprintf( Messages, "Update success");
            }
        }
        MEProcessHandleResult( Result, Messages );
        if( Step != UpdateNonMeRegion )
            Step += 1;
    }
    else
    {
        // check Flash verify result first.
        if( Status == EFI_DEVICE_ERROR )
        {
            Sprintf( Messages, "Flash Update Error at address 0x%08X !!", (UINT32)Address );
            MEProcessHandleResult( (BIT00 | BIT02), Messages );
            return EFI_SUCCESS;
        }
        switch( Step )
        {
            case UpdateFtprToOpr:
                Result = (BIT00 | BIT02);
                Sprintf( Messages, "Error when Update Recovery Loader to OPRx!!" );
            break;

            case UpdateMfsb:
                Result = (BIT00 | BIT02);
                Sprintf( Messages, "Error when Update MFSB!!" );
            break;

            case UpdateFtpr:
                Result = (BIT00 | BIT02);
                Sprintf( Messages, "Error when Update FTPR!!" );
            break;

            case UpdateMfs:
                Result = (BIT00 | BIT02);
                Sprintf( Messages, "Error when Update MFS, IVB1 and IVB2!!" );
            break;

            case UpdateFlog:
                Result = (BIT00 | BIT02);
                Sprintf( Messages, "Error when Update FLOG and UTOK!!" );
            break;

            case UpdateOpr:
                Result = (BIT00 | BIT02);
                Sprintf( Messages, "Error on ME Entire Image update!!" );
            break;

            default:
                StructPtr->UpdateResult = BIT03;
            break;
        }
        MEProcessHandleResult( Result, Messages );
        if( Step != UpdateNonMeRegion )
            Step += 1;
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
    EFI_STATUS                  Status;
    static UINTN                NumberOfPages;
    static UINT32               SizeCopied;
    static BOOLEAN              UseSmmMem = FALSE;
    static EFI_PHYSICAL_ADDRESS SMM_Address;
    static BOOLEAN              HeciIsHide;

    StructPtr = *MEProcessStructPtr;

    switch( StructPtr->bHandleRequest )
    {
        // Allocate Buffer
        case 1:
            HeciIsHide = UnHideHeci();
            // If Non-BIOS(ME/GbE/DER/PDR) region can't be identified 
            // by FDOC, the TotalBlocks could be less than Non-BIOS Region,
            // then, the SMM memroy will get destroyed when AFU upload
            // Non-BIOS region data to SMM. So, below change to use Non-BIOS 
            // region size instead of TotalBlocks size for allocating memory
            // to avoid such situation.
            NumberOfPages = EFI_SIZE_TO_PAGES(GetFlashCapacity() - FLASH_SIZE);
            Status = pSmst->SmmAllocatePages(
                                AllocateAnyPages,
                                EfiRuntimeServicesData,
                                NumberOfPages,
                                &SMM_Address );
            if( !EFI_ERROR(Status) )
            {
                UseSmmMem = TRUE;
                Phy_Address = SMM_Address;
            }
            // No memory allocated
            if( !Phy_Address )
                MEProcessHandleResult(
                    (BIT00 | BIT02),
                    "Error : No Memory Allocated!!" );

            SizeCopied = 0;
        break;

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

            HideHeci( HeciIsHide );
        break;
    }
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
