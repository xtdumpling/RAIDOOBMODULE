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

/** @file
    Contains the functions to decode BMC KCS base address BMC.
*/

#include <Token.h>
#include <PiPei.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Register/PchRegsPmc.h>
#include <PchAccess.h>

#define KCS_BASE_ADDRESS_MASK   	0xFFF0
#define NUMBER_OF_BYTES_TO_DECODE	0x10

/**
    Initializes Lpc to decode BMC IO ranges. This is platform specific Hook
    need to initialize chipset to decode and enable BMC IO ranges.

    @param PeiServices Pointer to the PEI Core data Structure

    @return EFI_STATUS
    @retval EFI_SUCCESS returned

**/
EFI_STATUS
DecodeBmcBaseAddress (
  IN  CONST EFI_PEI_SERVICES	**PeiServices )
{
    #if defined(PILOT4_LPC_ESPI_SLAVE_CONNECTION) && PILOT4_LPC_ESPI_SLAVE_CONNECTION
        UINT8 SlaveDevice = ESPI_SECONDARY_SLAVE;
    #else
        UINT8 SlaveDevice = LPC_ESPI_FIRST_SLAVE;
    #endif
	
    DEBUG ((EFI_D_INFO, "DecodeBmcBaseAddress: Decoding 0x%X bytes from 0x%X base address \n", \
    		NUMBER_OF_BYTES_TO_DECODE, (IPMI_KCS_BASE_ADDRESS & KCS_BASE_ADDRESS_MASK)));

    //
    // Decode KCS BMC IO Ranges
    //
	PchLpcGenIoRangeSet ((IPMI_KCS_BASE_ADDRESS & KCS_BASE_ADDRESS_MASK), NUMBER_OF_BYTES_TO_DECODE, SlaveDevice );
	
	return EFI_SUCCESS;
}

/**
    Checks the Power Failure PWR_FLR bit status in Power Management Control register.

    @param PeiServices Pointer to the PEI Core data Structure

    @return BOOLEAN
    @retval TRUE  - If PWR_FLR bit is set.
    @retval FALSE - If PWR_FLR bit is not set.

**/
BOOLEAN
CheckPowerFailureStatus (
  IN CONST EFI_PEI_SERVICES          **PeiServices )
{

    DEBUG ((EFI_D_INFO, "Power Failure PWR_FLR bit: %x\n",\
      (( *(volatile UINT8*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
      (UINTN) PCI_LIB_ADDRESS( DEFAULT_PCI_BUS_NUMBER_PCH, 
                               PCI_DEVICE_NUMBER_PCH_PMC, 
                               PCI_FUNCTION_NUMBER_PCH_PMC, 
                               R_PCH_PMC_GEN_PMCON_B)) & B_PCH_PMC_GEN_PMCON_B_PWR_FLR ) >> 1 )));    

    //
    // Check Power Failure PWR_FLR bit for AC power loss.
    //
    if ( *(volatile UINT8*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
            (UINTN) PCI_LIB_ADDRESS( DEFAULT_PCI_BUS_NUMBER_PCH, 
                                     PCI_DEVICE_NUMBER_PCH_PMC,
                                     PCI_FUNCTION_NUMBER_PCH_PMC,
                                     R_PCH_PMC_GEN_PMCON_B)) & B_PCH_PMC_GEN_PMCON_B_PWR_FLR ) {
            DEBUG ((EFI_D_INFO, "Power Failure PWR_FLR bit is set due to AC power loss.\n"));
            return TRUE;
    }

    return FALSE;

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
