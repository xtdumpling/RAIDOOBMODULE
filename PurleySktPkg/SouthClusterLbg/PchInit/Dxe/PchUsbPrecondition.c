/** @file
  PCH USB precondition feature support in DXE phase

@copyright
 Copyright (c) 2012 - 2014 Intel Corporation. All rights reserved
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
#include <PchInit.h>
#include "PchUsbPrecondition.h"

//
// Data referred by XHCI
//
  GLOBAL_REMOVE_IF_UNREFERENCED UINTN         PortSCxUsb2Base = 0;
  GLOBAL_REMOVE_IF_UNREFERENCED UINTN         PortSCxUsb3Base = 0;

//
// Data referred by USB Precondition feature
//
GLOBAL_REMOVE_IF_UNREFERENCED PCH_USB_HC_PORT_PRECONDITION      *mPrivatePreConditionList = NULL;

//
// This flag set when 50ms root port reset duration is satisified (Tdrstr). It is countered from
// last root port reset.
//
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                           PchUsbRPortsRstDoneFlag = FALSE;

//
// All root ports reset continuously, so the reset starting time between first root port and last
// root port should not exceed PCH ACPI timer High-to-Low transition frequency - 2.3435 seconds.
//
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                             LastRPortResetTicks = 0;

//
// Tdrstr for all root portis satisfied as the following scenarios:
//
//  |
//  |-> Reset all root ports of 1st HC, save tick_1 to LastRPortResetTicks
//  |
//  |-> Reset all root ports of 2nd HC, save tick_2 to LastRPortResetTicks
//  |
//  |-> IsRootPortReset () for is invoked by first call, wait until if delay for tick_2 is enough
//  |   Set PchUsbRPortsRstDoneFlag = TRUE, return TRUE if the port is in the list
//  |
//  |-> IsRootPortReset () is invoked for the other HC, and PchUsbRPortsRstDoneFlag is set
//  |   Return TRUE if the port is in the list
//

/**
  Return current PCH PM1 timer value

  @param[in]                      None

  @retval                         PM1 timer value in 32 bit
**/
UINTN
PchGetPchTimerTick (
  VOID
  )
{
  UINT16                          AcpiBaseAddr;
  PchAcpiBaseGet (&AcpiBaseAddr);
  return IoRead32 ((UINTN) (AcpiBaseAddr + R_PCH_ACPI_PM1_TMR)) & B_PCH_ACPI_PM1_TMR_VAL;

}

/**
  Check if the required delay condition is satisified
  Note: The delay can't be larger than PCH ACPI timer High-to-Low
  transition frequency - 2.3435 seconds.

  @param[in]  InitialTicks        Initial PM1 tick value
  @param[in]  RequiredStallInUs   Required delay in us

  @retval     TRUE                The required delay is satisified
  @retval     FALSE               The required delay is not satisified
**/
BOOLEAN
UsbTimeout (
  IN UINTN                        InitialTicks,
  IN UINTN                        RequiredStallInUs
  )
{
  UINTN                           CurrentTick;
  UINTN                           ExpiredTick;

  //
  // The timer frequency is 3.579545 MHz, so 1 us corresponds 3.58 clocks
  //
  ExpiredTick = RequiredStallInUs * 358 / 100 + InitialTicks + 1;
  CurrentTick = PchGetPchTimerTick ();

  //
  // The High-to-Low transition will occur every 2.3435 seconds.
  //
  if (CurrentTick < InitialTicks) {
    CurrentTick += V_PCH_ACPI_PM1_TMR_MAX_VAL;
  }

  if (CurrentTick > ExpiredTick){
    return TRUE;
  }
  return FALSE;
}

/**
  Initialize usb global data and flag for reference

  @param[in]  None

  @retval     None
**/
VOID
UsbInitGlobalData (
  VOID
  )
{

  ///
  /// Set the flag to false and start to count time.
  ///
  PchUsbRPortsRstDoneFlag = FALSE;

  ///
  /// This is the latest root port reset, record it to ensure the Tdrstr is satisified.
  ///
  LastRPortResetTicks = PchGetPchTimerTick();
  return;
}

/**
  Check if the delay is enough since last root port reset

  @param[in]  None

  @retval     None
**/
VOID
UsbTdrstrDelayCheck (
  VOID
  )
{
  UINTN                           i;

  ///
  /// If the latest root port reset done, and then for all root ports reset by
  /// this protocol is ready. If we are in scenario#3, wait until delay time is enough. The flag
  /// is set either by timer event or the waitting loop.
  ///
  for (i = 0; (PchUsbRPortsRstDoneFlag != TRUE) && (i < USB_ROOT_PORT_RESET_STALL_US/ USB_TDRSTR_CHECK_INTERVAL_US); i++) {
    if (UsbTimeout (LastRPortResetTicks, USB_ROOT_PORT_RESET_STALL_US)) {
      PchUsbRPortsRstDoneFlag = TRUE;
      LastRPortResetTicks = 0;
      break;
    }
    MicroSecondDelay (USB_TDRSTR_CHECK_INTERVAL_US);
  }

  return ;
}

/**
  Check if the queried port is reset by USB precondition feature or not. This service must be called when
  XHC is in Run(R/S = '1') mode per XHCI specification requirement.

  @param[in]  This                PCH_USB_HC_PORT_PRECONDITION instance
  @param[in]  PortNumber          The root port number (started by zero) to be queried

  @retval     TRUE                The root port is reset done
  @retval     FALSE               The root port is not reset
**/
BOOLEAN
EFIAPI
IsXhcRootPortReset (
  IN PCH_USB_HC_PORT_PRECONDITION *This,
  IN UINT8                        PortNumber
  )
{
  UINT32                          UsbPort;
  UINT32                          Data32;
  UINT32                          XhciMmioBase;
  UINT32                          XhciPciMmBase;
  USB_XHCI_PRECONDITION_DEV       *XhcPreCondition;
  BOOLEAN                         ResumeFlag;

  XhcPreCondition = XHC_PRECONDITION_FROM_THIS (This);
  PortSCxUsb2Base = R_PCH_LP_XHCI_PORTSC01USB2;

  ///
  /// If the signature, PortNumber, or PortResetBitMap is invalid, return
  /// FALSE directly. Otherwise, return TRUE when required reset signal delay
  /// is satisified.
  ///
  if ((XhcPreCondition->PortResetBitMap == 0) || (XhcPreCondition->Signature != XHCI_PRECONDITION_DEV_SIGN)) {
    return FALSE;
  }

  ///
  /// Resume all USB2 protocol ports by first call
  ///
  if (XhcPreCondition->PORTSCxResumeDoneFlag != TRUE) {
    //
    // Drive the reset signal on root port for at least 50ms(Tdrstr). Check USB 2.0 Spec
    // section 7.1.7.5 for timing requirements.
    //
    if (!PchUsbRPortsRstDoneFlag) {
      UsbTdrstrDelayCheck ();
    }

    XhciPciMmBase = (UINT32) MmPciBase (
                      (UINT8) XhcPreCondition->Protocol.Location.BusNumber,
                      (UINT8) XhcPreCondition->Protocol.Location.DeviceNumber,
                      (UINT8) XhcPreCondition->Protocol.Location.FunctionNumber
                      );

    XhciMmioBase = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE) & (~0xF);
    ResumeFlag = FALSE;

    ///
    /// For USB2 protocol port on XHCI, the reset done port will enter U3 state once the HC is halted
    /// To recovery the USB2 protocol port from U3 to U0, SW should:
    /// 1. SW shall ensure that the XHC is in Run mode prior to transitioning a root hub port from Resume to
    ///    the U0 state.
    /// 2. Write a "15" (Resume) to the PLS, XHC shall transmit the resume signaling within 1ms (Tursm)
    /// 3. SW shall ensure that resume is signaled for at least 20 ms (Tdrsmdn) from the write of Resume
    /// 4. After Tdrsmdn is complete, SW shall write a "0"(U0) to the PLS field
    ///
    for (UsbPort = 0; UsbPort < XhcPreCondition->HsPortCount; UsbPort++) {
      if (((UINT32)(1 << UsbPort) & XhcPreCondition->PortResetBitMap) != 0) {
        Data32 = MmioRead32 (XhciMmioBase + (PortSCxUsb2Base + UsbPort * 0x10));
        if ((Data32 & B_PCH_XHCI_PORTSCXUSB2_CCS) != 0) {
          Data32 &= ~B_PCH_XHCI_PORT_CHANGE_ENABLE_MASK;
          Data32 |= (B_PCH_XHCI_USB2_U3_EXIT + B_PCH_XHCI_PORTSCXUSB2_PP + B_PCH_XHCI_PORTSCXUSB2_LWS);
          MmioWrite32 (
            XhciMmioBase + (PortSCxUsb2Base + UsbPort * 0x10),
            Data32
            );
          ResumeFlag = TRUE;
        } else {
          //
          // The CCS bit of this port disappears, it may be caused by the following reasons:
          // 1. Link training is successfully now, the CCS shows on correct USB speed port, i.e. USB3
          //    speed if it is USB3 device.
          // 2. The device is removed.
          // Ignore this port due to there is no device on it now.
          //
          XhcPreCondition->PortResetBitMap &= ~(UINTN) (1u << UsbPort);
        }
      }
    }

    if (ResumeFlag) {
      //
      // There is one root port resuming from U3 at least.
      //
      MicroSecondDelay (20 * 1000);
      for (UsbPort = 0; UsbPort < XhcPreCondition->HsPortCount; UsbPort++) {
        if (((UINT32)(1 << UsbPort) & XhcPreCondition->PortResetBitMap) != 0 ) {
          Data32 = (B_PCH_XHCI_PORTSCXUSB2_PP + B_PCH_XHCI_PORTSCXUSB2_LWS + B_PCH_XHCI_PORTSCXUSB2_CCS);
          MmioWrite32 (
            XhciMmioBase + (PortSCxUsb2Base+ UsbPort*0x10),
            Data32
            );
        }
      }
    }
    XhcPreCondition->PORTSCxResumeDoneFlag = TRUE;
  }

  if (XhcPreCondition->PORTSCxResumeDoneFlag == TRUE) {
    //
    // If the signature, PortNumber, or PortResetBitMap is invalid, return
    // FALSE directly. Otherwise, return TRUE when required reset signal delay
    // is satisified.
    //
    if (((UINT32)(1 << PortNumber) & XhcPreCondition->PortResetBitMap) != 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Perform USB precondition on XHCI, it is the root port reset on
  installed USB device in DXE phase

  @retval     None
**/
VOID
XhciPrecondition (
  VOID
  )
{
  UINT32                          UsbPort;
  UINT32                          Data32;
  USB_XHCI_PRECONDITION_DEV       *XhcPreCondition;
  PCH_USB_HC_LOCATION             XhcLocation = {0, 0, 0, 0};

  EFI_PHYSICAL_ADDRESS            XhciMemBaseAddress;
  EFI_STATUS                      Status;
  UINT32                          XhciMmioBase;
  UINTN                           HsPortCount;
  UINTN                           XhciPciMmBase;
  // APTIOV_SERVER_OVERRIDE_RC_START : For XHCI BAR allocation based on MMIOL ranges  
  EFI_IIO_UDS_PROTOCOL  	  *IohUds;
  // APTIOV_SERVER_OVERRIDE_RC_END : For XHCI BAR allocation based on MMIOL ranges  

  if (mPchPolicyHob->UsbConfig.UsbPrecondition == 0) {
    //
    // Precondition disabled
    //
    return;
  }

  PortSCxUsb2Base     = R_PCH_LP_XHCI_PORTSC01USB2;
  // APTIOV_SERVER_OVERRIDE_RC_START : Allocating XHCI BAR based on IIO UDS CPU0 MMIOL range  
/*
  if ( (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchBottomUp) || (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchTopDown) ){
    XhciMemBaseAddress  = 0x0ffffffff;
  }

  */
  Status = gBS->LocateProtocol(
				&gEfiIioUdsProtocolGuid,
				NULL,
				&IohUds);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)){
    return;
  }
	
  XhciMemBaseAddress = IohUds->IioUdsPtr->PlatformData.IIO_resource[0].StackRes[0].PciResourceMem32Limit;

  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchTopDown,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  N_PCH_XHCI_MEM_ALIGN,
                  V_PCH_XHCI_MEM_LENGTH,
                  &XhciMemBaseAddress,
                  mImageHandle,
                  NULL
                  );
  DEBUG ((DEBUG_ERROR, "XHCI base address  %lx\n", XhciMemBaseAddress));  
  ASSERT_EFI_ERROR (Status);
  // APTIOV_SERVER_OVERRIDE_RC_END : For XHCI BAR allocation based on MMIOL ranges
  if (EFI_ERROR (Status)) {
    return;
  }

  XhciPciMmBase   = MmPciBase (
                      0,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI
                      );

  ///
  /// Assign memory resources
  ///
  XhciMmioBase = (UINT32) XhciMemBaseAddress;

  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, XhciMmioBase);

  MmioOr16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16) (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );

  XhcPreCondition = AllocateZeroPool (sizeof (USB_XHCI_PRECONDITION_DEV));
  if (XhcPreCondition == NULL) {
    goto EndOfXhciPrecondition;
  }

  HsPortCount = (MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB2_2) & 0x0000FF00) >> 8;

  XhcPreCondition->Signature = XHCI_PRECONDITION_DEV_SIGN;

  XhcPreCondition->HsPortCount = HsPortCount;

  for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
    Data32 = MmioRead32 (XhciMmioBase + (PortSCxUsb2Base+ UsbPort*0x10));
    if ((Data32 & B_PCH_XHCI_PORTSCXUSB2_CCS) != 0) {
      Data32 &= ~B_PCH_XHCI_PORTSCXUSB2_PED;
      Data32 |= B_PCH_XHCI_PORTSCXUSB2_PR | B_PCH_XHCI_PORTSCXUSB2_PP;
      MmioWrite32 (
        XhciMmioBase + (PortSCxUsb2Base+ UsbPort*0x10),
        Data32
        );
      //
      // PortSC registers in PCH XHCI is counted from HS ports
      //
      XhcPreCondition->PortResetBitMap |= (UINTN) (1 << UsbPort);
    }
  }
  UsbInitGlobalData ();
  XhcLocation.DeviceNumber = (UINTN) PCI_DEVICE_NUMBER_PCH_XHCI;
  XhcLocation.FunctionNumber = (UINTN) PCI_FUNCTION_NUMBER_PCH_XHCI;
  CopyMem (&(XhcPreCondition->Protocol.Location), &XhcLocation, sizeof (PCH_USB_HC_LOCATION));
  XhcPreCondition->Protocol.IsRootPortReset = IsXhcRootPortReset;
  XhcPreCondition->PORTSCxResumeDoneFlag = FALSE;
  mPrivatePreConditionList = &(XhcPreCondition->Protocol);

EndOfXhciPrecondition:

  MmioAnd16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16) ~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, 0);

  //
  // Free the GCD pool
  //
  gDS->FreeMemorySpace (
         XhciMemBaseAddress,
         V_PCH_XHCI_MEM_LENGTH
         );
}
