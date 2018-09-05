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

/** @file PeiBmcBoard.c
    This file contains code for Initialize LPC BMC base
    address and SIO KCS in the PEI stage.

**/

#include "Token.h"
#include "IpmiBmc.h"
#include <PiPei.h>
#include <Library/PciLib.h>

// LPC Bridge Bus Device Function Definitions
#define LPC_BUS                 0
#define LPC_DEVICE              31
#define LPC_FUNC                0
#define REG_GEN2_DEC            0x88
#define LPC_DECODE_ENABLE       0x01
#define KCS_BASE_ADDRESS_MASK   0xFFF0
#define ENABLE_16_BYTE          0x000C0000

//#define REG_LPC_ENABLE          0x82
//#define REG_LPC_PM_BASE         0x40
//#define CNF2_LPC_EN             0x2000
//#define SIO_INDEX_PORT          0x4E
//#define SIO_DATA_PORT           0x4F
//#define LDN_REGISTER            0x07
//#define KCS4_LDN                0x09
//#define ACTIVATE_REGISTER       0x30

//#define BIT_MC_LPC_EN       0x400

/**
    Initializes Lpc to decode BMC IO ranges. This is platform specific Hook
    need to initialize chipset to decode and enable BMC IO ranges.

    @param PeiServices Pointer to the PEI Core data Structure

    @return EFI_STATUS
    @retval EFI_SUCCESS returned

**/
EFI_STATUS
InitLpcBmc (
  IN  CONST EFI_PEI_SERVICES          **PeiServices )
{
    //Commenting the SIO initialization code as PILOT3 module is available
    /*{

        #define SIO_CONFIG_INDEX    0x4e
        #define SIO_CONFIG_DATA        0x4f
        #define PILOT3_LD_SEL_REGISTER    0x7
        #define PILOT3_ACTIVATE_REGISTER    0x30
        #define PILOT3_BASE1_HI_REGISTER    0x60
        #define PILOT3_BASE1_LO_REGISTER    0x61
        #define PILOT3_BASE2_HI_REGISTER    0x62
        #define PILOT3_BASE2_LO_REGISTER    0x63

        #define PILOT3_LDN_KCS4    0x9
        #define SIO_KCS4_DATA_BASE_ADDRESS    0xca2
        #define SIO_KCS4_CMD_BASE_ADDRESS    0xca3
        #define PILOT3_ACTIVATE_VALUE    0x1

        IoWrite8 (SIO_CONFIG_INDEX, 0x5A); //UnLock SIO space

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_LD_SEL_REGISTER); //Point to logical device selection register
        IoWrite8 (SIO_CONFIG_DATA, PILOT3_LDN_KCS4); //Set logical device KCS4

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_BASE1_LO_REGISTER); //Point to Base Address LSB register
        IoWrite8 (SIO_CONFIG_DATA, (UINT8)((UINT16)SIO_KCS4_DATA_BASE_ADDRESS & 0x00FF)); //Write Base Address LSB register

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_BASE1_HI_REGISTER); //Point to Base Address MSB register
        IoWrite8 (SIO_CONFIG_DATA, (UINT8)((UINT16)SIO_KCS4_DATA_BASE_ADDRESS >> 8)); //Write Base Address MSB register

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_BASE2_LO_REGISTER); //Point to Activate Register
        IoWrite8 (SIO_CONFIG_DATA, (UINT8)((UINT16)SIO_KCS4_CMD_BASE_ADDRESS & 0x00FF)); //Enable serial port with Activation bit

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_BASE2_HI_REGISTER); //Point to Base Address LSB register
        IoWrite8 (SIO_CONFIG_DATA, (UINT8)((UINT16)SIO_KCS4_CMD_BASE_ADDRESS >> 8)); //Write Base Address LSB register

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_ACTIVATE_REGISTER); //Point to Activate Register
        IoWrite8 (SIO_CONFIG_DATA, PILOT3_ACTIVATE_VALUE); //enable serial port with Activation bit

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_LD_SEL_REGISTER); //Point to logical device selection register
        IoWrite8 (SIO_CONFIG_DATA, PILOT3_LDN_KCS4); //Set logical device KCS4

        IoWrite8 (SIO_CONFIG_INDEX, 0xA5); //Lock SIO space
    }*/


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
  //
  //Enable decoding Lpc BMC base address region.
  //
    PciWrite32 (((UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, REG_GEN2_DEC)), \
        ((IPMI_KCS_BASE_ADDRESS & KCS_BASE_ADDRESS_MASK) | LPC_DECODE_ENABLE) | ENABLE_16_BYTE);   // Enable 16 Byte

#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 1
/*
  //
  //Enable decoding LPC BMC base address region - KCS3.
  //Enable decoding LPC BMC base address region - KCS3.
  //
    PciOr16 ((UINTN) PCI_LIB_ADDRESS(LPC_BUS,LPC_DEVICE,LPC_FUNC,REG_LPC_ENABLE), BIT_MC_LPC_EN);	 
*/
#endif

/*
    PciWrite32 ((UINTN) PCI_LIB_ADDRESS(LPC_BUS,LPC_DEVICE,LPC_FUNC,REG_GEN2_DEC), \
        (BMC_ADDRESS_DECODE_MASK<<16) | (IPMI_KCS_BASE_ADDRESS & 0xFFFC) | LPC_DECODE_ENABLE);
  //
  //There are five KCS interfaces implemented on the Pilot. The KCS1 and KCS2 interfaces are 
  // disabled and will not be supported by the Pilot firmware. The remaining three KCS (
  // KCS3, 4, & 5) interfaces can be used for the communication between the host and the BMC.
  
  //
  //The BMC and the host processor communicate through KCS3, KCS4, KCS5, BT, SMIC or Mailboxes. 
  //Enable KCS4 in Pilot Sio to communicate with BMC.
  //By default base address is 0xCA2 and 0xCA3. just we need to enable in Activate Reg.
  //

  //
  //Enable decoding of SIO index ports 0x4E, 0x4F
  //
    PciOr16 ((UINTN) PCI_LIB_ADDRESS(LPC_BUS,LPC_DEVICE,LPC_FUNC,REG_LPC_ENABLE), CNF2_LPC_EN));

  //UnLock SIO space
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_INDEX_PORT, 0x5A);

  //Enable KCS4 for Host to BMC interface.
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_INDEX_PORT, LDN_REGISTER);
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_DATA_PORT, KCS4_LDN);
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_INDEX_PORT, ACTIVATE_REGISTER);
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_DATA_PORT, 0x01);
  
  //Lock SIO space
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_INDEX_PORT, 0xA5);
*/
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
  return EFI_SUCCESS;
}

#if BMC_INIT_DELAY
/**
    Checks the Power Failure PWR_FLR bit in Power Management Control register and Proper delay 
    is provided to get response from BMC when PWR_FLR bit is set and Wait for BMC Setup option 
    is enabled.

    @param PeiServices Pointer to the PEI Core data Structure

    @return BOOLEAN
    @retval TRUE  - If PWR_FLR bit is set.
    @retval FALSE - If PWR_FLR bit is not set.

**/
BOOLEAN
BmcInitDelay (
  IN CONST EFI_PEI_SERVICES          **PeiServices )
{

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Check for Power Failure PWR_FLR bit: %x \n", \
        PciRead8 ((UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, LPC_GEN_PMCON_3)) & PWR_FLR_BIT));
    //
    // Check Power Failure PWR_FLR bit for AC power loss.
    //
    if (PciRead8 ((UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, LPC_GEN_PMCON_3)) & PWR_FLR_BIT) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Power Failure PWR_FLR bit for AC power loss. \n"));
        return TRUE;
    }

    return FALSE;

}
#endif

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
