/** @file
  This protocol provides the data referred by USB Precondition feature

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
#ifndef _USB_HC_PORT_PRECONDITION_H_
#define _USB_HC_PORT_PRECONDITION_H_



typedef struct {
  UINTN SegmentNumber;
  UINTN BusNumber;
  UINTN DeviceNumber;
  UINTN FunctionNumber;
} PCH_USB_HC_LOCATION;

typedef struct {
  UINTN ResetRecovery;
} PCH_USB_PORT_ENUM_TIMING_TABLE;

typedef struct _PCH_USB_HC_PORT_PRECONDITION PCH_USB_HC_PORT_PRECONDITION;

typedef
BOOLEAN
(EFIAPI *PCH_USB_HC_PORT_RESET_STATUS) (
  IN PCH_USB_HC_PORT_PRECONDITION *This,
  IN UINT8                        PortNumber
  );

/**
  This protocol provides the data referred by USB Precondition feature
**/
struct _PCH_USB_HC_PORT_PRECONDITION {
  UINT8                           Revision;
  PCH_USB_HC_PORT_PRECONDITION    *Next;
  PCH_USB_HC_LOCATION             Location;
  PCH_USB_HC_PORT_RESET_STATUS    IsRootPortReset;
  PCH_USB_PORT_ENUM_TIMING_TABLE  Timing;
};

#endif
