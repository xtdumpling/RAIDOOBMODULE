/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement.
--*/
/*++
Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

 SpsHwChangePpi.h

Abstract:

 Node Manager in SPS firmware may support power measurement procedure using
 Power/Thermal Utility (PTU) application exposed as option ROM on MROM device
 on PCIe. To activate PTU OpROM this protocol can be used to signal that
 power measurement support is enabled and hardware configuration change
 was detected.
 
--*/
#ifndef _SPS_HW_CHANGE_PPI_H_
#define _SPS_HW_CHANGE_PPI_H_

extern EFI_GUID gSpsHwChangePpiGuid;

typedef struct _SPS_HW_CHANGE_PPI_ SPS_HW_CHANGE_PPI;


/*****************************************************************************
 @brief
  This function sets power measurement suport status and hardware configuration
  change status to be sent to ME. It is needed when platform power measurement
  using Power/Thermal Utility (PTU) Option ROM is supported.

  NOTE: This function can be used only when Node Manager is enabled in ME.

  @param[in] pThis            Pointer to this PPI
  @param[in] PwrMsmtSupport   True if power measurement using PTU is supported,
                              otherwise false.
  @param[in] HwCfgChanged     True if hardware configuration changed since
                              last boot, otherwise false.

  @return EFI_STATUS is returned.
**/
typedef EFI_STATUS (EFIAPI *SPS_HW_CHANGE_SET_STATUS)(
  IN     SPS_HW_CHANGE_PPI *pThis,
  IN     BOOLEAN            PwrMsmtSupport,
  IN     BOOLEAN            HwCfgChanged);

/*
 * EFI protocol for hardware configuration change monitoring for ME.
 *
 * This protocol defines operations needed to inform Node Manager in ME about:
 * - host support for power consumption measurement using NM PTU Option ROM and
 * - change in hardware configuration that may infuence power consumption.
 * The later triggers power measurement procedure when the first says that
 * measurement is supported.
 */
typedef struct _SPS_HW_CHANGE_PPI_
{
  SPS_HW_CHANGE_SET_STATUS  SpsHwChangeSetStatus;
} SPS_HW_CHANGE_PPI;

#endif // _SPS_HW_CHANGE_PPI_H_

