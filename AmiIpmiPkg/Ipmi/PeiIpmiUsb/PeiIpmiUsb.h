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

/** @file PeiIpmiUsb.h
    This file contains definitions and includes for PeiIpmiUsb 

**/

#ifndef _PEI_IPMI_USB_H
#define _PEI_IPMI_USB_H

//----------------------------------------------------------------------

#include <Token.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/ReportStatusCodeLib.h>

//#undef EFI_D_ERROR  Commented to build in debug mode if AmiUsb_16 is used.
#include "AmiModulePkg/Usb/Pei/UsbBotPeim.h"
#include <Ppi/IpmiUsbTransportPpi.h>
#include <Include/IpmiUsbPei.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <AmiStatusCodes.h>

//----------------------------------------------------------------------

//
// The Value field specifies the descriptor type in the high byte and
// the descriptor index in the low byte.
// Value = Type << 8 | Index
#define DESCRIPTOR_TYPE_INDEX_VALUE( Type )  ( (Type << 8) | 0x00 )

//
// Status code definition to report BMC Selftest fail.
//

#ifndef AMI_PEIM_EC_USB_BMC_SELFTEST_FAILED
#define AMI_PEIM_EC_USB_BMC_SELFTEST_FAILED    (AMI_STATUS_CODE_CLASS | 0x0000000C)
#endif

#ifndef PEI_USB_BMC_SELFTEST_FAILED
#define PEI_USB_BMC_SELFTEST_FAILED            (EFI_SOFTWARE_PEI_MODULE | AMI_PEIM_EC_USB_BMC_SELFTEST_FAILED)
#endif

//
// Function Prototypes
//

EFI_STATUS
EFIAPI
EfiSendIpmiUsbCommand (
  IN  IPMI_USB_TRANSPORT        *This,
  IN  UINT8                     NetFunction,
  IN  UINT8                     Lun,
  IN  UINT8                     Command,
  IN  UINT8                     *CommandData,
  IN  UINT8                     CommandDataSize,
  OUT UINT8                     *ResponseData,
  OUT UINT8                     *ResponseDataSize );

EFI_STATUS
EFIAPI
EfiSendOemIpmiUsbCommand (
  IN  IPMI_USB_TRANSPORT        *This,
  IN  UINT8                     NetFunction,
  IN  UINT8                     Lun,
  IN  UINT8                     Command,
  IN  UINT8                     *CommandData,
  IN  UINT32                    CommandDataSize,
  OUT UINT8                     *ResponseData,
  OUT UINT32                    *ResponseDataSize );

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
