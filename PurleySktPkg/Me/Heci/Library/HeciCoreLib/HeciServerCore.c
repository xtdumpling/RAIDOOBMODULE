/**
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
**/

/**

Copyright (c) 2008 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file HeciServerCore.c

  Heci Server driver core for PEI Phase.

**/

#include <Library/DebugLib.h>
#include <IndustryStandard/Pci.h>
#include <Library/PlatformHooksLib.h>
#include <MeChipset.h>
#include <Library/HeciCoreLib.h>

#include "MeState.h"
#include "HeciRegs.h"
#include "MeAccess.h"


STATIC UINTN
Heci2MbarRead (VOID)
{
  union
  {
    UINT64   QWord;
    struct
    {
      UINT32 DWordL;
      UINT32 DWordH;
    } Bits;
  } Mbar;
  
  Mbar.QWord = 0;
  Mbar.Bits.DWordL = Heci2PciRead32(R_HECIMBAR);
  if (Mbar.Bits.DWordL == 0xFFFFFFFF)
  {
    DEBUG((DEBUG_ERROR, "[HECI2] ERROR: HECI-2 device disabled\n"));
    Mbar.Bits.DWordL = 0;
    goto GetOut;
  }
  Mbar.Bits.DWordL &= 0xFFFFFFF0; // clear address type bits
  Mbar.Bits.DWordH = Heci2PciRead32(R_HECIMBAR + 4);
  
  if (IsSimicsPlatform() && Mbar.Bits.DWordH != 0)
  {
    DEBUG((DEBUG_WARN, "[HECI2] Detected 64-bit MBAR in HECI-2 under SIMICS, force 32-bit\n"));
    Mbar.QWord = 0;
  }
  if (Mbar.QWord == 0)
  {
    Mbar.QWord = HECI2_PEI_DEFAULT_MBAR;
    DEBUG((DEBUG_WARN, "[HECI2] WARNING: MBAR not programmed, using default 0x%08X%08X\n",
           Mbar.Bits.DWordH, Mbar.Bits.DWordL));
    Heci2PciWrite32(R_HECIMBAR + 4, Mbar.Bits.DWordH);
    Heci2PciWrite32(R_HECIMBAR, Mbar.Bits.DWordL);
  }
 GetOut:
  return (UINTN)Mbar.QWord;
}


/**

    Return ME Status

    @param MeStatus    - Pointer for status report

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
Heci2GetMeStatus (
  IN  UINT32                 *MeStatus
  )
{
  HECI_FWS_REGISTER MeFirmwareStatus;

  if (MeStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *MeStatus           = ME_NOT_READY;

  MeFirmwareStatus.ul = Heci2PciRead32 (R_FWSTATE);

  *MeStatus           = MeFirmwareStatus.ul;

  DEBUG ((EFI_D_ERROR, "[HECI2] NMFS: %X\n", MeFirmwareStatus.ul));

  return EFI_SUCCESS;
}


/*****************************************************************************
 @brief
   Determines if the HECI-2 device is present and, if present, initializes it for
   use by the BIOS.

 @param  None

 @retval EFI_SUCCESS    The function completed successfully.
 @retval EFI_DEVICE_ERROR  HECI device not present.
**/
EFI_STATUS
EFIAPI
Heci2Initialize (
  VOID)
{
  HECI_HOST_CONTROL_REGISTER           HeciRegHCsr;
  VOLATILE HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  VOLATILE HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr;
  UINTN                                HeciMBar;

  HeciRegHCsrPtr    = NULL;
  HeciRegMeCsrHaPtr = NULL;

  //
  // Check for HECI-1 PCI device availability
  //
  if (Heci2PciRead16(PCI_VENDOR_ID_OFFSET) != 0x8086)
  {
    DEBUG((EFI_D_ERROR, "[HECI2] ERROR: Device not present\n"));
    return EFI_DEVICE_ERROR;
  }
  //
  // Get HECI_MBAR and see if it is programmed
  // to a useable value
  //
  HeciMBar = Heci2MbarRead();

  //
  // Set HECI-2 interrupt delivery mode to SCI interrupt
  //
  DEBUG((EFI_D_INFO, "[HECI2] Setting HIDM to SCI mode\n"));
  Heci2PciWrite8(R_HIDM, HECI_INTERRUPUT_GENERATE_SCI);
  //
  // Set HECI-2 interrupt delivery mode lock
  //
  Heci2PciWrite8 (R_HIDM, Heci2PciRead8 (R_HIDM) | HECI_INTERRUPUT_LOCK);

  //
  // Enable HECI BME and MSE
  //
  Heci2PciWrite8(PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
  //
  //  Setup H_CSR register as follows:
  //     a) Make sure H_RST is clear
  //     b) Set H_RDY = 0 (not ready)
  //     c) Set H_IG = 0 (no interrupt)
  // Thus HECI-2 is not enabled (not ready) till ASL code enable it.
  //
  HeciRegHCsrPtr      = (VOID *) ((UINTN)HeciMBar + H_CSR);
  HeciRegHCsr.ul      = HeciRegHCsrPtr->ul;

  HeciRegHCsr.r.H_RST = 0;
  HeciRegHCsr.r.H_RDY = 0;
  HeciRegHCsr.r.H_IS  = 1;
  HeciRegHCsr.r.H_IG  = 0;

  HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;

  return EFI_SUCCESS;
}// Heci2Initialize ()

