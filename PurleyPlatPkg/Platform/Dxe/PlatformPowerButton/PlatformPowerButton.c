/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2011 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  PlatformPowerButton.c

@brief:

  Provide the Power Button Shutdown AP.

**/
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Library/BaseLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsLpc.h>
#include <PchAccess.h>
#include <Library/MmPciBaseLib.h>

/**
  When a power button event happens, it shutdown the machine
  
Arguments:

  @param[in] DispatchHandle       Dispatch handle
  @param[in] Context              Context information
  @param[in] CommBuffer           Buffer used for communication between caller/callback function
  @param[in] CommBufferSize       Size of communication buffer

  @retval Status.

**/
EFI_STATUS 
PowerButtonHandler(
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  UINTN   PmcBaseAddress;	
  UINT16  PchPmioBase;
  UINT16  Data16;
 // APTIOV_SERVER_OVERRIDE_RC_START : SMM Child Dispatcher will power off board      
  //UINT32  Data32;
 // APTIOV_SERVER_OVERRIDE_RC_END : SMM Child Dispatcher will power off board      

  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH, 
                     PCI_DEVICE_NUMBER_PCH_PMC, 
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );
  PchPmioBase = MmioRead16 (PmcBaseAddress + R_PCH_PMC_ACPI_BASE) & B_PCH_PMC_ACPI_BASE_BAR;
    
  //
  // First, GPE0_EN should be disabled to prevent any GPI waking up the system from S5
  // 
  Data16 = 0;         
  IoWrite16 ((UINTN)(PchPmioBase + R_PCH_ACPI_GPE0_EN_127_96), (UINT16)Data16);
  
  // APTIOV_SERVER_OVERRIDE_RC_START : SMM Child Dispatcher will power off board      
  #if 0
  //
  // Second, PwrSts register must be clear
  //  
  Data16 = IoRead16 ((UINTN)(PchPmioBase + R_PCH_ACPI_PM1_STS)); 
  Data16 |= B_PCH_ACPI_PM1_STS_PWRBTN;
  IoWrite16 ((UINTN)(PchPmioBase + R_PCH_ACPI_PM1_STS), (UINT16)Data16);
          
  //
  // Finally, transform system to S5 sleep state
  //
  Data32 = IoRead32 ((UINTN)(PchPmioBase + R_PCH_ACPI_PM1_CNT));
  Data32  = (UINT32) ((Data32 & ~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);

  DEBUG ((EFI_D_ERROR, "Write S5 to SLP_TYPE in PM1_CNT\n"));
  IoWrite32 ((UINTN) (PchPmioBase + R_PCH_ACPI_PM1_CNT), (UINT32)Data32);

  Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;
  DEBUG ((EFI_D_ERROR, "Write SLP_EN in PM1_CNT to go to S5\n"));
  IoWrite32 ((UINTN) (PchPmioBase + R_PCH_ACPI_PM1_CNT), (UINT32)Data32);
  #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : SMM Child Dispatcher will power off board      

  return EFI_SUCCESS;
}

/**
  The Driver Entry Point.
  
  The function is the driver Entry point.
  
  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES      Could not allocate needed resources

**/
EFI_STATUS 
EFIAPI
PowerButtonHandlerEntry(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              DispatchHandle;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *SmmPowerbuttonDispatch2;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT   Context = {EfiPowerButtonEntry};

  Status = gSmst->SmmLocateProtocol(
                    &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                    NULL,
                    &SmmPowerbuttonDispatch2
                    );
  if (!EFI_ERROR (Status)) {
    Status = SmmPowerbuttonDispatch2->Register(
                                      SmmPowerbuttonDispatch2,
                                      PowerButtonHandler,
                                      &Context,
                                      &DispatchHandle
                                      );    
  }

  return Status;
}
