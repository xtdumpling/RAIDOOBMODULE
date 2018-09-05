/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  OemErrorHandler.h

Abstract:
  This file will contain all definitions related to PCH Error Handler

------------------------------------------------------------------*/

#ifndef _PCH_ERROR_HANDLER_H
#define _PCH_ERROR_HANDLER_H
#include <PchAccess.h>
#include <Library/GpioLib.h>
#include <BackCompatible.h>
#include <Protocol/SmmGpiDispatch2.h>

#define PCI_DEVICE_NUMBER_PCH_LPC   31
#define PCI_FUNCTION_NUMBER_PCH_LPC 0
//
////
//// PM PCI registers
////
#define R_PCH_LPC_ACPI_BASE             0x40
//#define B_PCH_LPC_ACPI_BASE_BAR         0x0000FF80
#define B_PCH_LPC_ACPI_BASE_EN          BIT0
//
//#define R_PCH_LPC_ACPI_CNT              0x44
//#define B_PCH_LPC_ACPI_CNT_ACPI_EN      0x80
//#define B_PCH_LPC_ACPI_CNT_SCI_IRG_SEL  0x07
//
//#define R_PCH_OC_WDT_CTL                0x54
//#define B_PCH_OC_WDT_CTL_RLD            BIT31
//#define B_PCH_OC_WDT_CTL_ICCSURV_STS    BIT25
//#define B_PCH_OC_WDT_CTL_NO_ICCSURV_STS BIT24
//#define B_PCH_OC_WDT_CTL_EN             BIT14
//#define B_PCH_OC_WDT_CTL_ICCSURV        BIT13
//#define B_PCH_OC_WDT_CTL_LCK            BIT12
//#define B_PCH_OC_WDT_CTL_TOV_MASK       0x3FF
//#define B_PCH_OC_WDT_CTL_FAILURE_STS    BIT23
//#define B_PCH_OC_WDT_CTL_UNXP_RESET_STS BIT22
//#define B_PCH_OC_WDT_CTL_AFTER_POST     0x3F0000
//
//#define R_PCH_LPC_PM_INIT               0xAC
//#define B_PCH_LPC_PMIR_FIELD_1          (BIT31 | BIT30) // PMIR Field 1
//#define B_PCH_LPC_PMIR_FIELD_0          0x0FFFFF        // PMIR Field 0
//#define B_PCH_LPC_GLOBAL_RESET_CF9      0x00100000
//#define B_PCH_LPC_GLOBAL_RESET_LOCK     0x80000000
//
//#define R_PCH_LPC_BIOS_CNTL             0xDC
//#define S_PCH_LPC_BIOS_CNTL             1
//#define B_PCH_LPC_BIOS_CNTL_TSS         0x10
//#define V_PCH_LPC_BIOS_CNTL_SRC         0x0C
//#define V_PCH_SRC_PREF_EN_CACHE_EN      0x08
//#define V_PCH_SRC_PREF_DIS_CACHE_DIS    0x04
//#define V_PCH_SRC_PREF_DIS_CACHE_EN     0x00
//#define B_PCH_LPC_BIOS_CNTL_BLE         0x02
//#define B_PCH_LPC_BIOS_CNTL_BIOSWE      0x01
//#define N_PCH_LPC_BIOS_CNTL_BLE         1
//#define N_PCH_LPC_BIOS_CNTL_BIOSWE      0
//
//
//
#define INVALID_FLAG                    0xFF
//
////
//// PCH SMI enable and GPIO SMI enable/status
////
//#define R_ACPI_SMI_EN                  0x30
//#define R_ACPI_ALT_GP_SMI_EN           0x38
//#define  S_ACPI_ALT_GP_SMI_EN          2
//#define R_ACPI_ALT_GP_SMI_STS          0x3A
//#define  S_ACPI_ALT_GP_SMI_STS         2
//
////SMI_STS - SMI Status Register
//#define R_ACPI_SMI_STS                                 0x34
//#define  S_ACPI_SMI_EN                                 4
//#define   B_GPIO_UNLOCK_SMI_EN                         BIT27
//#define   B_EL_SMI_EN                                  BIT25
//#define   B_INTEL_USB2_EN                              BIT18
//#define   B_LEGACY_USB2_EN                             BIT17
//#define   B_PERIODIC_EN                                BIT14
#define   B_TCO_EN                                     BIT13
//#define   B_MCSMI_EN                                   BIT11
//#define   B_BIOS_RLS                                   BIT7
//#define   B_SWSMI_TMR_EN                               BIT6
//#define   B_APMC_EN                                    BIT5
//#define   B_SMI_ON_SLP_EN                              BIT4
//#define   B_LEGACY_USB_EN                              BIT3
//#define   B_BIOS_EN                                    BIT2
//#define   B_EOS                                        BIT1
#define   B_GBL_SMI_EN                                 BIT0
//
//
//#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8       7
//#define PCH_PCIE_RROT_PORT_AER_UES                     0x104
//
////
//// The structure for the light version of AHCI ROM
////
//#define DEFAULT_PCI_BUS_NUMBER_PCH                    0
//#define PCI_DEVICE_NUMBER_PCH_SATA                    31
//#define PCI_FUNCTION_NUMBER_PCH_SATA                   2
//#define SATA_CONTROLLER_STRUC_SIGNATURE               0x41325324
//#define SATA_CONTROLLER_STRUC_FIELDS_VALID            0xF
//#define SATA_CONTROLLER_IMPLEMENTED_PORTS             0x0000003F
//
//
////
//// PCI-Ex AER registers
////
#define R_PCIEXP_AER_UES              0x104   // Uncorrectable error status
#define V_PCIEXP_AER_UES              (BIT4+BIT12+BIT14+BIT15+BIT16+BIT17+BIT18+BIT20)   // Uncorrectable error status or value
#define R_PCIEXP_AER_UEM              0x108   // Uncorrectable error mask
#define V_PCIEXP_AER_UEM              (BIT4+BIT12+BIT14+BIT15+BIT16+BIT17+BIT18+BIT20)   // Uncorrectable error mask
#define R_PCIEXP_AER_UEV              0x10C   // Uncorrectable error severity
#define V_PCIEXP_AER_UEV              (BIT0+BIT4+BIT12+BIT13+BIT14+BIT15+BIT16+BIT17+BIT18+BIT19+BIT20)   // Uncorrectable error severity
//
#define R_PCIEXP_AER_CES              0x110   // Correctable error status
#define V_PCIEXP_AER_CES              (BIT0+BIT6+BIT7+BIT8+BIT12+BIT13) // Correctable error status or value
#define R_PCIEXP_AER_CEM              0x114   // Correctable error mask
#define V_PCIEXP_AER_CEM              (BIT0+BIT6+BIT7+BIT8+BIT12+BIT13)   // Correctable error mask
//
#define R_PCIEXP_AER_REC              0x12c   // Root error command
#define R_PCIEXP_AER_RES              0x130   // Root error status

#define R_PCH_PCR_AECH                       0x2080      // Advanced Error Reporting Capabilities header (rsvd)
#define R_PCH_PCR_DMI_UES                    0x2084                      ///< Uncorrectable Error Status
#define R_PCH_PCR_DMI_UEM                    0x2088                      ///< Uncorrectable Error Mask
#define R_PCH_PCR_DMI_UEV                    0x208C                      ///< Uncorrectable Error Severity
#define R_PCH_PCR_DMI_CES                    0x2090                      ///< Correctable Error Status
#define R_PCH_PCR_DMI_CEM                    0x2094                      ///< Correctable Error Mask
#define R_PCH_PCR_DMI_AECC                   0x2098                      ///< Advanced Error Capabilities and Control
#define R_PCH_PCR_DMI_RES                    0x20B0                      ///< Root Error Status

#define B_PCH_DMI_UE_TE                            BIT0    // Training error
#define B_PCH_DMI_UE_DLPE                          BIT4    // Data Link Protocol Error
#define B_PCH_DMI_UE_PT                            BIT12   // Poisoned TLP
#define B_PCH_DMI_UE_FCPE                          BIT13   // Flow Control Protocol Error
#define B_PCH_DMI_UE_CT                            BIT14   // Completion Timeout
#define B_PCH_DMI_UE_CA                            BIT15   // Completer Abort
#define B_PCH_DMI_UE_UC                            BIT16   // Unexpected Completion
#define B_PCH_DMI_UE_RO                            BIT17   // Receiver Overflow
#define B_PCH_DMI_UE_MT                            BIT18   // Malformed TLP
#define B_PCH_DMI_UE_EE                            BIT19   // ECRC Error
#define B_PCH_DMI_UE_URE                           BIT20   // Unsupported Request Error

#define B_PCH_DMI_CE_RE                            BIT0    // Indicates a receiver error
#define B_PCH_DMI_CE_BT                            BIT6    // Bad TLP
#define B_PCH_DMI_CE_BD                            BIT7    // Bad DLLP
#define B_PCH_DMI_CE_RNR                           BIT8    // Replay Number Rollover

#define B_PCH_DMI_RES_CR                           BIT0
#define B_PCH_DMI_RES_ENR                          BIT2

#define B_PCH_PCI_PCI_BPC_PSE                       BIT6
#define R_PCH_PCI_PCI_BPC                           0x4C
#define B_PCH_PCI_PCI_BPC_RTAE                      BIT0

#define PCH_CATERR_GPI_SMI_GPP_PAD   GPIO_SKL_H_GPP_C23
#define PCH_CATERR_GPI_SMI_GPP_COM   2

// Used to override the AER mask registers
// Set bits that need to be masked due to known issues
#define PCH_AER_CEM_OVERRIDE    0x00000000
#define PCH_AER_UEM_OVERRIDE    0x00008000    // BIT15 must be set to prevent a reset during legacy Windows boot

//
// ICH device structure
//
typedef struct  {
  UINT8                   Segment;
  UINT8                   Bus;
  UINT8                   Device;
  UINT8                   Function;
  BOOLEAN                 Bridge;

} RAS_PCH_PCI_DEVICES;


////////////////////////////////////// ERROR HANDLING ROUTINES /////////////////////////////////////////

/**
  This function performs the PCH error checking and processing functions

  @retval EFI_SUCCESS.

**/
EFI_STATUS
ProcessPchErrors (
VOID
  );

/**
  This function checks if a PCI device is present on the specified bus.

  @param[in] Bus        Device's bus number

  @retval TRUE        A PCI device was found on the bus

**/
 BOOLEAN
PciErrLibDeviceOnBus (
  IN UINT8   Bus
  );

/**
  Notify the OS using PCH mechanisms

  This function takes the appropriate action for a particular error based on severity.

  @param[in] ErrorSeverity    The severity of the error to be handled

  @retval Status.

**/
EFI_STATUS
PchNotifyOs (
  IN      UINT8     ErrorSeverity,
     OUT  BOOLEAN   *OsNotified
  );


  ////////////////////////////////////// INITIALIZATION ROUTINES /////////////////////////////////////////

/**
  Function to clear PCH errors

  @retval None.

**/
VOID
ClearPchErrors (
 VOID
 );

#endif


