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
/** @file CspMeud10Gbe.c

    CspMeud related functions.
*/
#include <Uefi.h>

#include <AmiLib.h>
#include <Flash.h>

#include "CspMeud.h"

UINT8   mPfMacAddress[0x04][0x06];
UINT8   mPortMacAddress[0x04][0x06];
UINT8   mManageabilityMac[0x04][0x06];
UINT8   mPciMacAddress[0x06];

/**
    Memory access 10Gbe region

    @param Destination
    @param Source
    @param Length
    @param Restore

    @retval VOID
**/
VOID
FlashAccess10Gbe(
    IN  VOID    *Destination,
    IN  VOID    *Source,
    IN  UINTN   Length,
    IN  BOOLEAN Restore )
{
    FlashRead( Source, Destination, (UINT32)Length );
}
/**
    Memory access 10Gbe region

    @param Destination
    @param Source
    @param Length
    @param Restore This should be set if it's restore process

    @retval VOID
**/
VOID
MemAcess10Gbe(
    IN  VOID    *Destination,
    IN  VOID    *Source,
    IN  UINTN   Length,
    IN  BOOLEAN Restore )
{
    if( Restore == TRUE )
        MemCpy( Source, Destination, (UINT32)Length );
    else
        MemCpy( Destination, Source, (UINT32)Length );
}
/**
    Process 10Gbe pf mac address

    @param Region10Gbe Pointer to 10Gbe region
    @param Access Flash or memory access

    @retval EFI_STATUS
**/
EFI_STATUS
ProcessPfMac(
    IN  UINT16          *Region10Gbe,
    IN  UINT8           Behavior,
    IN  ACCESS_10GBE    Access
)
{
    UINT16                      U16Offset;
    UINT16                      U16PfMacOffset;
    UINT16                      *PfMacAddress;
    PF_MAC_ADDRESS_STRUCTURE    *PfMac;
    UINT8                       Index;

    //Read 16bit pointer at offset 0x48
    Access( &U16Offset, &Region10Gbe[0x48], sizeof(U16Offset), FALSE );

    //PF MAC address
    Access( &U16PfMacOffset, &Region10Gbe[0x18 + U16Offset], sizeof(U16PfMacOffset), FALSE );

    PfMacAddress = &Region10Gbe[0x18 + U16Offset + U16PfMacOffset];

    PfMac = (PF_MAC_ADDRESS_STRUCTURE*)PfMacAddress;

    for( Index = 0x00; Index < 0x04; Index++ )
    {
        Access(
            &mPfMacAddress[Index],
            &PfMac[Index].MacAddress,
            sizeof(PfMac->MacAddress),
            TRUE );
    }

    return EFI_SUCCESS;
}
/**
    Process 10Gbe port mac address

    @param Region10Gbe Pointer to 10Gbe region
    @param Access Flash or memory access

    @retval EFI_STATUS
**/
EFI_STATUS
ProcessPortMac(
    IN  UINT16          *Region10Gbe,
    IN  UINT8           Behavior,
    IN  ACCESS_10GBE    Access
)
{
    UINT16  U16Offset;
    UINT16  U16Offset1;
    UINT16  U16Offset2;
    UINT8   Index;

    //Read 16bit pointer at offset 0x07
    Access( &U16Offset, &Region10Gbe[0x07], sizeof(U16Offset), FALSE );
    Access( &U16Offset1, &Region10Gbe[0x0B + U16Offset], sizeof(U16Offset1), FALSE );
    Access( &U16Offset2, &Region10Gbe[0x0C + U16Offset], sizeof(U16Offset2), FALSE );

    for( Index = 0x00; Index < 0x04; Index++ )
    {
        Access(
            &mPortMacAddress[Index],
            &Region10Gbe[U16Offset1 + U16Offset2 + (Index * 0x02)],
            sizeof(UINT32),
            TRUE );
    }

    Access( &U16Offset1, &Region10Gbe[0x0D + U16Offset], sizeof(U16Offset1), FALSE );
    Access( &U16Offset2, &Region10Gbe[0x0E + U16Offset], sizeof(U16Offset2), FALSE );

    for( Index = 0x00; Index < 0x04; Index++ )
    {
        Access(
            &mPortMacAddress[Index][sizeof(UINT32)],
            &Region10Gbe[U16Offset1 + U16Offset2 + (Index * 0x02)],
            sizeof(UINT16),
            TRUE );
    }

    return EFI_SUCCESS;
}
/**
    Process 10Gbe manageability mac address

    @param Region10Gbe Pointer to 10Gbe region
    @param Access Flash or memory access

    @retval EFI_STATUS
**/
EFI_STATUS
ProcessManageabilityMac(
    IN  UINT16          *Region10Gbe,
    IN  UINT8           Behavior,
    IN  ACCESS_10GBE    Access
)
{
    UINT16  U16Offset;
    UINT16  U16Offset1;
    UINT8   Index;

    Access( &U16Offset, &Region10Gbe[0x48], sizeof(U16Offset), FALSE );
    Access( &U16Offset1, &Region10Gbe[0x17 + U16Offset], sizeof(U16Offset1), FALSE );

    for( Index = 0x00; Index < 0x04; Index++ )
    {
        Access(
            &mManageabilityMac[Index],
            &Region10Gbe[0x17 + U16Offset + U16Offset1 + 0x01 + (Index * 0x06)],
            sizeof(mManageabilityMac[Index]),
            TRUE );
    }

    return EFI_SUCCESS;
}
/**
    Process 10Gbe pci mac address

    @param Region10Gbe Pointer to 10Gbe region
    @param Access Flash or memory access

    @retval EFI_STATUS
**/
EFI_STATUS
ProcessPciMac(
    IN  UINT16          *Region10Gbe,
    IN  UINT8           Behavior,
    IN  ACCESS_10GBE    Access
)
{
    UINT16  U16Offset;
    UINT16  U16Offset1;
    UINT16  U16Offset2;

    //Read 16bit pointer at offset 0x07
    Access( &U16Offset, &Region10Gbe[0x07], sizeof(U16Offset), FALSE );
    Access( &U16Offset1, &Region10Gbe[0x07 + U16Offset], sizeof(U16Offset1), FALSE );
    Access( &U16Offset2, &Region10Gbe[0x08 + U16Offset], sizeof(U16Offset2), FALSE );

    Access(
        &mPciMacAddress,
        &Region10Gbe[U16Offset1 + U16Offset2],
        sizeof(UINT32),
        TRUE );

    Access( &U16Offset1, &Region10Gbe[0x09 + U16Offset], sizeof(U16Offset1), FALSE );
    Access( &U16Offset2, &Region10Gbe[0x0A + U16Offset], sizeof(U16Offset2), FALSE );

    Access(
        &mPciMacAddress[sizeof(UINT32)],
        &Region10Gbe[U16Offset1 + U16Offset2],
        sizeof(UINT16),
        TRUE );

    return EFI_SUCCESS;
}
/**
    Calculate 10Gbe region first 64K checksum

    @param Region10Gbe Pointer to 10Gbe region

    @retval EFI_STATUS
**/
EFI_STATUS
CalculateCheckSum(
    IN  UINT16  *Region10Gbe
)
{
    UINT16  VpdOffset;
    UINT16  CheckSum16;
    UINT16  Index;

    //VPD area pointer at 0x2F
    VpdOffset = Region10Gbe[0x2F];

    CheckSum16 = 0;

    for( Index = 0; Index < I40E_SR_SIZE_IN_WORDS; Index++ )
    {
        //Skip checksum
        if( Index == I40E_SR_SW_CHECKSUM_WORD_OFFSET )
            continue;

        //Skip Vpd area
        if( Index == VpdOffset )
        {
            Index += (I40E_SR_VPD_MODULE_MAX_SIZE_IN_WORDS - 0x01);
            continue;
        }

        //Skip PCIe ALT
        if( Index == I40E_SR_PCIE_ALT_MODULE_WORD_OFFSET )
            break;

        CheckSum16 += Region10Gbe[Index];
    }

    CheckSum16 = I40E_SR_SW_CHECKSUM_BASE - CheckSum16;
    Region10Gbe[I40E_SR_SW_CHECKSUM_WORD_OFFSET] = CheckSum16;

    return EFI_SUCCESS;
}
/**
    Process 10Gbe mac address

    @param Region10Gbe Pointer to 10Gbe region
    @param Behavior Save or restore the 10Gbe address

    @retval EFI_STATUS
**/
EFI_STATUS
Process10GbeMac(
    IN  VOID    *Region10Gbe,
    IN  UINT8   Behavior
)
{
    UINT16          *U16Region10Gbe;
    ACCESS_10GBE    *Access;
    UINT16          NvmControl;

    U16Region10Gbe = (UINT16*)Region10Gbe;

    switch( Behavior )
    {
        case Save10GbeMac:
            Access = FlashAccess10Gbe;
        break;

        case Restore10GbeMac:
            Access = MemAcess10Gbe;
        break;

        default:
            return EFI_UNSUPPORTED;
    }

    //NVM control word (Word 0x0000)
    ////Bit 7:6, if NVM validity doesn't equal 01b the NVM data is not present
    Access( &NvmControl, &U16Region10Gbe[0x00], sizeof(NvmControl), FALSE );
    if( (NvmControl & 0xC0) != 0x40 )
    {
        U16Region10Gbe = &U16Region10Gbe[0x8000];
    }

    ProcessPfMac( U16Region10Gbe, Behavior, Access );
    ProcessPortMac( U16Region10Gbe, Behavior, Access );
    ProcessManageabilityMac( U16Region10Gbe, Behavior, Access );

    if( Behavior == Restore10GbeMac )
        CalculateCheckSum( U16Region10Gbe );

    return EFI_SUCCESS;
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