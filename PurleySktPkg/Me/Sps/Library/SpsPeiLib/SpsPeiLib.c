/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  SpsPeiLib.c

@brief:

  Implementation file for Sps Pei Library

**/
#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Library/MeTypeLib.h>
#include <Ppi/MePolicyPpi.h>
#include <Ppi/HeciPpi.h>
#include <Ppi/Spi.h>
#include "SpsPei.h"
#include <Library/PchInfoLib.h>

#define SS125_VMX20_MPHY0             125

#define CLOCK_MODE_NATIVE             0x00
#define CLOCK_MODE_HYBRID             0x01
#define CLOCK_MODE_NATIVE_ALTERNATE   0x10
#define CLOCK_MODE_HYBRID_ALTERNATE   0x11
#define CLOCK_MODE_EXTERNAL_STANDARD  0x02
#define CLOCK_MODE_EXTERNAL_ALTERNATE 0x12

#if SPS_SUPPORT
EFI_STATUS
SpsSetGetCurrenClockingMode (
   IN UINT8                                 Command,
   IN OUT ICC_SETGET_CLOCK_SETTINGS_BUFFER  *Msg
  );
#endif // SPS_SUPPORT

CLOCKING_MODES GetClockingModeFromSoftStraps( VOID )
{
  EFI_STATUS         Status;
  EFI_SPI_PROTOCOL  *SpiPpi;
  UINT32             SoftStrapValue;
  CLOCKING_MODES     retVal = InternalStandard;

  // In this case read SS125 bits 0,1
  //     0x0 -> internal clock
  //     0x2 -> external clock
  Status = PeiServicesLocatePpi (
             &gPeiSpiPpiGuid,
             0,
             NULL,
             (VOID **) &SpiPpi
             );
  if (!EFI_ERROR (Status)) {
    Status = SpiPpi->ReadPchSoftStrap (SpiPpi, SS125_VMX20_MPHY0 * sizeof(UINT32), sizeof (SoftStrapValue),(UINT8*)&SoftStrapValue);
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "(Hsio) GetClockingMode() SPI SoftStrap read = 0x%x", SoftStrapValue));
      SoftStrapValue &= 3;
      DEBUG ((DEBUG_INFO, " (0x%x)\n", SoftStrapValue));
      switch (SoftStrapValue) {
        case 0:
          if (PchStepping () == LbgA0) {  
            retVal = CLOCK_MODE_NATIVE;
          } else {
            retVal = CLOCK_MODE_NATIVE_ALTERNATE;
          }
        break;
        case 2:
          retVal = CLOCK_MODE_EXTERNAL_STANDARD;
        break;
        default:
          DEBUG ((DEBUG_WARN, "(Hsio) GetClockingMode() Warning: unsupported clocking mode choice, assuming internal\n"));
        break;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "(Hsio) GetClockingMode() returns %s",
          retVal==InternalStandard?"InternalStandard":"External"));

  return retVal;
}

/**
  Get current clocking mode via HECI.
  In case non SPS ME on board read it from soft straps

  @param[out] ClockingMode        Clocking mode read

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           Function called before DID
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         Unsupported Clocking Mode has been returned
**/
EFI_STATUS
PeiGetCurrenClockingMode (
   OUT CLOCKING_MODES *ClockingMode
  )
{
  EFI_STATUS                        Status = EFI_INVALID_PARAMETER;
  ICC_SETGET_CLOCK_SETTINGS_BUFFER  Msg;
  CONST EFI_PEI_SERVICES            **PeiServices = GetPeiServicesTablePointer ();

  DEBUG ((DEBUG_INFO, "[ME] PeiGetCurrenClockingMode\n"));
  if (ClockingMode != NULL) {
    (*PeiServices)->SetMem(&Msg, sizeof(ICC_SETGET_CLOCK_SETTINGS_BUFFER), 0);
#if SPS_SUPPORT
    if (MeTypeIsSps () && !MeTypeIsSpsInRecovery ()) {
      DEBUG ((DEBUG_INFO, "[ME] PeiGetCurrenClockingMode: For SPS ME use Heci message\n"));
      Status = SpsSetGetCurrenClockingMode( ICC_GET_CLOCK_SETTINGS_CMD,
                                            &Msg
                                          );
    } else {
      DEBUG ((DEBUG_INFO, "[ME] PeiGetCurrenClockingMode: Non SPS ME detected\n"));
      Status = EFI_NOT_READY;
    }
#else
    Status = EFI_NOT_READY;
#endif // SPS_SUPPORT
    if (Status == EFI_NOT_READY) {
      // Before DreamInitDone message read it from Soft Straps
      DEBUG ((DEBUG_INFO, "[ME] PeiGetCurrenClockingMode: Read clock mode from SoftStraps\n"));
      Status = EFI_SUCCESS;
      Msg.Response.ClockMode = GetClockingModeFromSoftStraps();
    }
    if (!EFI_ERROR(Status)) {
      switch (Msg.Response.ClockMode) {
        case CLOCK_MODE_NATIVE:
          *ClockingMode = InternalStandard;
          break;
        case CLOCK_MODE_HYBRID:
          *ClockingMode = HybridStandard;
          break;
        case CLOCK_MODE_NATIVE_ALTERNATE:
          *ClockingMode = InternalAlternate;
          break;
        case CLOCK_MODE_HYBRID_ALTERNATE:
          *ClockingMode = HybridAlternate;
          break;
        case CLOCK_MODE_EXTERNAL_STANDARD:
        case CLOCK_MODE_EXTERNAL_ALTERNATE:
          *ClockingMode = External;
          break;
        default:
          DEBUG ((DEBUG_ERROR,
                  "(ME) PeiGetCurrenClockingMode: Unknown Clocking Mode = 0x%x\n",
                  Msg.Response.ClockMode));
          Status =  EFI_UNSUPPORTED;

      }
    }
  }

  DEBUG ((DEBUG_INFO, "[ME] PeiGetCurrenClockingMode exit status = %r \n",
                      Status));
  return Status;
}


EFI_STATUS
SpsSetGetCurrenClockingMode (
   IN UINT8                                 Command,
   IN OUT ICC_SETGET_CLOCK_SETTINGS_BUFFER  *Msg
  )
{
#if SPS_SUPPORT
  EFI_STATUS                    Status;
  EFI_STATUS                    RetVal = EFI_INVALID_PARAMETER;
  HECI_PPI                      *HeciPpi;
  UINT32                        CommandSize;
  UINT32                        ResponseSize;
#else
  EFI_STATUS                    RetVal = EFI_UNSUPPORTED;
#endif // SPS_SUPPORT

  DEBUG ((DEBUG_INFO, "[ICC] SpsSetGetCurrenClockingMode\n"));

#if SPS_SUPPORT
  if (!MeTypeIsSps () || MeTypeIsSpsInRecovery ()) {
    RetVal = EFI_UNSUPPORTED;
  } else if (Msg != NULL) {
    RetVal = EFI_DEVICE_ERROR;
    Status = PeiServicesLocatePpi (
              &gHeciPpiGuid,            // GUID
              0,                        // INSTANCE
              NULL,                     // EFI_PEI_PPI_DESCRIPTOR
              (VOID **) &HeciPpi        // PPI
              );
    if (!EFI_ERROR(Status)) {
      CommandSize                          = sizeof (ICC_SETGET_CLOCK_SETTINGS_REQ);
      ResponseSize                         = sizeof (ICC_SETGET_CLOCK_SETTINGS_RSP);

      Msg->Message.IccHeader.ApiVersion    = SKYLAKE_PCH_PLATFORM;
      Msg->Message.IccHeader.IccCommand    = Command;
      Msg->Message.IccHeader.IccResponse   = 0;
      Msg->Message.IccHeader.BufferLength  = CommandSize - sizeof (ICC_HEADER);
      Msg->Message.IccHeader.Reserved      = 0;
      Msg->Message.RequestedClock          = REQUESTED_CLOCK_IsCLK_BCLK;
      Msg->Message.SettingType             = 0;

      RetVal = HeciPpi->SendwAck (
                      HECI1_DEVICE,
                      (UINT32 *) &(Msg->Message),
                      CommandSize,
                      &ResponseSize,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_ICC_MESSAGE_ADDR
                    );

      if (!EFI_ERROR(RetVal) &&
          ((Msg->Response.IccHeader.IccCommand != Command) ||
           (Msg->Response.IccHeader.IccResponse != ICC_STATUS_SUCCESS))) {
        DEBUG ((DEBUG_ERROR,
                "(ICC) SpsSetGetCurrenClockingMode: Wrong response! IccHeader.IccResponse = 0x%x\n",
                Msg->Response.IccHeader.IccResponse));
        RetVal = EFI_DEVICE_ERROR;
      }
    }
  }
#endif // SPS_SUPPORT
  DEBUG ((DEBUG_INFO, "[ICC] SpsSetGetCurrenClockingMode exit status = %r \n",
                      RetVal));

  return RetVal;
}

/**
  Function sets SSC on/off

  @param[in] UseAlternate         Set Alternate SSC
                                  FALSE: turn off SSC for BCLK PLL
                                  TRUE:  turn on SSC 0.5% downspread for BCLK PLL

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           Function called before DID
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         Unsupported Clocking Mode has been returned
**/
EFI_STATUS
PeiHeciSetSSCAlternate (
   IN BOOLEAN UseAlternate
  )
{
#if SPS_SUPPORT
  EFI_STATUS                        Status = EFI_INVALID_PARAMETER;
  ICC_SETGET_CLOCK_SETTINGS_BUFFER  Msg;
  CONST EFI_PEI_SERVICES            **PeiServices = GetPeiServicesTablePointer ();
#else
  EFI_STATUS                        Status = EFI_UNSUPPORTED;
#endif // SPS_SUPPORT

#if SPS_SUPPORT
  DEBUG ((DEBUG_INFO, "[ME] PeiHeciSetSSCAlternate(%d)\n", UseAlternate));
  (*PeiServices)->SetMem(&Msg, sizeof(ICC_SETGET_CLOCK_SETTINGS_BUFFER), 0);

  if (UseAlternate) {
    Msg.Message.SSCsetting = 1; // turn on SSC 0.5% downspread for BCLK PLL
  } else {
    Msg.Message.SSCsetting = 0; // turn off SSC for BCLK PLL
  }
  DEBUG ((DEBUG_INFO, "[ME] PeiHeciSetSSCAlternate sets SSC settings to %d\n",
                       Msg.Message.SSCsetting));

  Status = SpsSetGetCurrenClockingMode( ICC_SET_CLOCK_SETTINGS_CMD,
                                        &Msg
                                       );
#endif // SPS_SUPPORT

  DEBUG ((DEBUG_INFO, "[ME] PeiHeciSetSSCAlternate exit status = %r \n",
                      Status));

  return Status;
}
