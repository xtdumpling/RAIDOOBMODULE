/** @file
  Header file for the PCH USB Driver.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCH_USB_H_
#define _PCH_USB_H_
#include "PchInitPei.h"

#define USB_PR_CASE_0           0
#define USB_PR_CASE_1           1
#define USB_PR_CASE_2           2
#define TEN_MS_TIMEOUT          10000
#define PORT_RESET_TIMEOUT      12000  ///< 12000 * 10 us = 120 ms timeout for xHCI port reset, according to USB3 spec
#define XHCI_TEST_MODE_DURATION 100
#define XHCI_TEST_MODE_LOOP     160

typedef struct {
  UINT8   Device;
  UINT8   Function;
} USB_CONTROLLER;

/**
  Configures PCH USB controller
  
  @param[in]  PchPolicy           The PCH Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI 
                                  devices to be used to initialize MMIO 
                                  registers.

  @retval EFI_INVALID_PARAMETER   The parameter of PchPolicy is invalid
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EFIAPI
ConfigureXhciPreMem (
  IN  PCH_POLICY_PPI  *PchPolicy
  );

/**
  Performs configuration of PCH xHCI SSIC.

  @param[in] PchPolicy            The PCH Policy PPI
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
VOID
XhciSsicInit (
  IN  PCH_POLICY_PPI     *PchPolicy,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Performs basic configuration of PCH USB3 (xHCI) controller.

  @param[in] PchPolicy            The PCH Policy PPI
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in]  XhciPciMmBase       XHCI PCI Base Address

**/
VOID
XhciHcInit (
  IN  PCH_POLICY_PPI     *PchPolicy,
  IN  UINT32             XhciMmioBase,
  IN  UINTN              XhciPciMmBase
  );

/**
  Performs configuration of PCH xHCI post other initialization.

  @param[in] PchPolicy            The PCH Policy PPI
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
  VOID
  XhciPostInitDone (
    IN  PCH_POLICY_PPI     *PchPolicy,
    IN  UINT32             XhciMmioBase,
    IN  UINTN              XhciPciMmBase
    );



/**
  Setup XHCI Over-Current Mapping

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
VOID
XhciOverCurrentMapping (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Program Xhci Port Disable Override

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
VOID
XhciPortDisableOverride (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  );
/**
  Program XHCI USB ports to wake the system on USB connect/desconnetc

 **/
VOID
XhciWakeOnUsb (
  IN CONST PCH_USB_CONFIG          *UsbConfig,
  IN UINTN                         XhciMmioBase
  );
/**
  Program and enable XHCI Memory Space

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
VOID
XhciMemorySpaceOpen (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Clear and disable XHCI Memory Space

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
VOID
XhciMemorySpaceClose (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Lock USB registers before boot

  @param[in] UsbConfig      The PCH Policy for USB configuration
**/


/**
  Configure PCH xHCI, post-mem phase

  @param[in]  PchPolicy           The PCH Policy PPI instance
  @param[in]  TempMemBaseAddr     Temporary Memory Base Address for PCI 
                                  devices to be used to initialize MMIO 
                                  registers.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
ConfigureXhci (
  IN  PCH_POLICY_PPI   *PchPolicy
  );

/**
  Performs configuration of PCH USB3 (xHCI) controller.

  @param[in] XhciMmioBase        XHCI Mmio Base Address

  @retval None
**/
VOID
XhciInitPei (
  IN  UINT32    XhciMmioBase
  );

/**
  Tune the USB 2.0 high-speed signals quality.

  @param[in]  UsbConfig                 The PCH Policy for USB configuration
  @param[in]  TempMemBaseAddr           The temporary memory base address for PMC device
**/
VOID
Usb2AfeProgramming (
  IN  CONST PCH_USB_CONFIG    *UsbConfig,
  IN  UINT32                  TempMemBaseAddr
  );

/**
  Tune the USB 3.0 signals quality.

  @param[in]  UsbConfig                 The PCH Policy for USB configuration
**/
VOID
XhciUsb3Tune (
  IN  CONST PCH_USB_CONFIG    *UsbConfig
  );

#endif
