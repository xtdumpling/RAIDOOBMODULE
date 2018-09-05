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


/** @file CmosBoard.c
    Contains routines that are board-specific.

**/

#include <AmiCspLib.h>

#ifndef MSR_XAPIC_BASE
  #define MSR_XAPIC_BASE 0x1b
#endif
#ifndef XAPIC_BASE_BSP_BIT
  #define XAPIC_BASE_BSP_BIT 8
#endif
#ifndef XAPIC_BASE_BSP_BIT_MASK
  #define XAPIC_BASE_BSP_BIT_MASK  (1 << XAPIC_BASE_BSP_BIT)
#endif

#ifndef EFI_GLOBAL_VARIABLE_GUID
#define EFI_GLOBAL_VARIABLE_GUID \
  {0x8BE4DF61, 0x93CA, 0x11D2, 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C}
#endif


/**
    This function is used to access addresses in the Standard CMOS
    register range (0x0-0x7f), for PEI and DXE boot phases.


    @param PeiServices        PEI Services table pointer (NULL in DXE phase)
    @param AccessType         ReadType or WriteType to specify the type of access
    @param CmosRegister       The CMOS register to access
    @param CmosParameterValue Pointer to the data variable to be accessed

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

**/

EFI_STATUS ReadWriteStandardRange (
    IN      EFI_PEI_SERVICES      **PeiServices,  // NULL in DXE phase
    IN      CMOS_ACCESS_TYPE      AccessType,
    IN      UINT16                CmosRegister,
    IN OUT  UINT8                 *CmosParameterValue )
{
    UINT8   TempValue;

    if (CmosRegister > 0x7f)
        return EFI_INVALID_PARAMETER;

    // If reading an RTC register, then check the Update in Progress bit in
    // Status register A to ensure a read is allowed.
    if(CmosRegister <= 9 && AccessType == ReadType) {
        do {
            IoWrite8(CMOS_BANK0_INDEX, 0xa | CMOS_NMI_BIT_VALUE);
            TempValue = IoRead8(CMOS_BANK0_DATA);
        } while (TempValue & 0x80);
    }

    IoWrite8( CMOS_BANK0_INDEX, (UINT8)CmosRegister | CMOS_NMI_BIT_VALUE );

    if (AccessType == ReadType){
        *CmosParameterValue = IoRead8( CMOS_BANK0_DATA );
    }
    else {
        IoWrite8( CMOS_BANK0_DATA, *CmosParameterValue );
    }

    return EFI_SUCCESS;
}

/**
    This function is used to access addresses in the CMOS register range
    (0x80-0xff), for PEI and DXE boot phases.

    @param PeiServices        Pointer to the PEI services table.  (This pointer
                              is NULL in the DXE phase.)
    @param AccessType         ReadType or WriteType to specify the type of access
    @param CmosRegister       The CMOS register to access
    @param CmosParameterValue Pointer to the data variable to be accessed

    @retval EFI_STATUS (return value)
            EFI_SUCCESS - the access operation was successful.
            Otherwise - a valid EFI error code is returned.

    @note
      This function is used when a translation from logical
      address to index port value is required.  For example, the
      ICH10 only allows values 0-7f for port 72/73 (upper bank)
      access, so logical CMOS address 0x80 must be translated to
      index port value 0, 0x81 becomes index port value 1, and
      so on.

**/


/*
============================================================================
   Southbridge porting notes
============================================================================
   The following code is an example of how to implement code that
   translates from CMOS register number to access the physical
   CMOS location when the register number is not identical to the
   physical location.

   If access to the address range above 0x7f is required, and translation
   between CMOS register number and CMOS index port value must be made,
   then a function similar to this should be place in SBGeneric.c, SBCsplib.h,
   and the appropriate CMOS_PORT_MAPPING eLink should also be defined.
============================================================================
*/

/*
EFI_STATUS ReadWriteSecondBankTranslated (
    IN      EFI_PEI_SERVICES      **PeiServices,  // NULL in DXE phase
    IN      CMOS_ACCESS_TYPE      AccessType,
    IN      UINT16                CmosRegister,
    IN OUT  UINT8                 *CmosParameterValue )
{
    if (CmosRegister < 0x80 || CmosRegister > 0xff)
        return EFI_INVALID_PARAMETER;

    IoWrite8( CMOS_BANK1_INDEX, (UINT8)(CmosRegister-0x80));

    if (AccessType == ReadType){
        *CmosParameterValue = IoRead8( CMOS_BANK1_DATA );
    }
    else {
        IoWrite8( CMOS_BANK1_DATA, *CmosParameterValue );
    }

    return EFI_SUCCESS;
}
*/


/**
    This function is used to determing whether or not the CMOS battery is
    good.


    @param PeiServices Pointer to the PEI services table.  (This pointer
                       is NULL in the DXE phase.)

    @retval BOOLEAN (Return Value)
        = TRUE if the battery is good
        = TRUE if the battery is bad

    @note  Some chipsets need to check a PCI configuration space
              register to determine the RTC power status.

**/

BOOLEAN CmosMgrBatteryIsGood (
    IN  EFI_PEI_SERVICES      **PeiServices  /* NULL in DXE */  )
{
    // PORTING PORTING PORTING

    IoWrite8( CMOS_BANK0_INDEX,
        CMOS_RTC_STATUS_REGISTER + CMOS_NMI_BIT_VALUE );

    return ((IoRead8( CMOS_BANK0_DATA ) & 0x80) != 0) ? TRUE : FALSE;

    // PORTING PORTING PORTING
}

/**
    This function determines whether or not the CMOS is usable. It may
    be possible to use the CMOS even if the battery is bad.

    @param Manager Pointer to the Manager's interface

    @retval TRUE CMOS is usable
    @retval FALSE CMOS is not usable

    @note
      Typically, register 0xd can be used to determine whether or not
      CMOS is usable.  It may be possible to use the CMOS for reads/writes
      if the battery is bad.

**/

BOOLEAN CmosIsUsable (
    IN  EFI_PEI_SERVICES      **PeiServices  /* NULL in DXE */  )
{
    IoWrite8( CMOS_BANK0_INDEX,
        CMOS_RTC_STATUS_REGISTER + CMOS_NMI_BIT_VALUE );

    return ((IoRead8( CMOS_BANK0_DATA ) & 0x80) != 0) ? TRUE : FALSE;
}


/**
    This function determines whether or not this is the first boot after
    programming the boot device (flash part, ROM emulator, ...).

    @param PeiServices Pointer to the PEI services table.  (This pointer
                       is NULL in the DXE phase.)

    @retval BOOLEAN (Return Value)
        = TRUE, if this is the first boot
        = FALSE, otherwise

    @note
      For some chipsets, there may be additional issues to consider,
      such as warm reboot during the PEI phase, multiple processor threads
      and so on.  Therefore, this function can be implemented by an external
      module (typically the Southbridge module).

**/

BOOLEAN CmosMgrIsFirstBoot(
    IN  EFI_PEI_SERVICES        **PeiServices )
{
    // In the DXE phase, FALSE is always returned.
    return FALSE;
}


/**
    This function returns TRUE if the currently executing CPU core
    is the boot strap processor (BSP).

    @param PeiServices Pointer to the PEI services table.  (This pointer
                       is NULL in the DXE phase.)

    @retval BOOLEAN (Return Value)
        = TRUE, the BSP is executing
        = FALSE, otherwise

    @note
      For some chipsets, this functions may need to be replaced.

**/


BOOLEAN CmosMgrIsBsp(
    IN  EFI_PEI_SERVICES  **PeiServices )
{
    // if bit 8 of the APIC base register is set, then the current
    // core is the BSP (for most chipsets)

    //if ( ReadMsr(MSR_XAPIC_BASE) & XAPIC_BASE_BSP_BIT_MASK )
        return TRUE;
    //else
    //    return FALSE;
}


/**
    This function returns TRUE to specify the platform is always perfoming
    a cold boot.

    @param PeiServices Pointer to the PEI services table.  (This pointer
                       is NULL in the DXE phase.)

    @retval BOOLEAN (Return Value)
        = TRUE

    @note
      For some chipsets, this functions may need to be replaced.

**/

BOOLEAN CmosMgrIsColdBoot(
    IN  EFI_PEI_SERVICES  **PeiServices )
{
    return TRUE;
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
