/** @file
  This file defines the PCH Info Protocol.

@copyright
 Copyright (c) 2008 - 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_INFO_H_
#define _PCH_INFO_H_
#include <SiVersion.h>

///
/// Define PCH INFO protocol GUID
///
#include "UsbHcPortPrecondition.h"

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                       gPchInfoProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_INFO_PROTOCOL PCH_INFO_PROTOCOL;

/**
  Protocol revision number
  Any backwards compatible changes to this protocol will result in an update in the revision number
  Major changes will require publication of a new protocol

  Revision 1:  Original version
**/
#define PCH_INFO_PROTOCOL_REVISION_1  1

/**
  RcVersion[7:0] is the release number.
  For example:
  PCH Framework 0.6.0-01 should be 00 06 00 01 (0x00060001)
**/
#define PCH_RC_BUILD    0x00
#define PCH_RC_VERSION  (SI_MAJOR_VERSION << 24) | (SI_MINOR_VERSION << 16) | (SI_REV_VERSION << 8) | (PCH_RC_BUILD)

/**
 This protocol is used to provide the information of PCH controller.
**/
struct _PCH_INFO_PROTOCOL {
  /**
    This member specifies the revision of the PCH Info protocol. This field is used
    to indicate backwards compatible changes to the protocol. Platform code that
    consumes this protocol must read the correct revision value to correctly interpret
    the content of the protocol fields.
  **/
  UINT8        Revision;
  /**
    Indicate the Usb precondition feature is working, and it links all the Usb HC
    precondition structures in the list.
  **/
  PCH_USB_HC_PORT_PRECONDITION  *Preconditioned;
  UINT8        PcieControllerCfg1;
  UINT8        PcieControllerCfg2;
  UINT8        PcieControllerCfg3;
  UINT8        PcieControllerCfg4;
  UINT8        PcieControllerCfg5;
};

#endif
