/** @file
  This file defines the PCH PCIE IoTrap Protocol.

@copyright
 Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_PCIE_IOTRAP_H_
#define _PCH_PCIE_IOTRAP_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                       gPchPcieIoTrapProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_PCIE_IOTRAP_PROTOCOL PCH_PCIE_IOTRAP_PROTOCOL;

///
/// Pcie Trap valid types
///
typedef enum {
  PciePmTrap,
  PcieTrapTypeMaximum
} PCH_PCIE_TRAP_TYPE;

/**
 This protocol is used to provide the IoTrap address to trigger PCH PCIE call back events
**/
struct _PCH_PCIE_IOTRAP_PROTOCOL {
  UINT16      PcieTrapAddress;
};

#endif
