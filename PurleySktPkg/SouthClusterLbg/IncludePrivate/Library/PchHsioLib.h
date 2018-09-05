/** @file
  Header file for PchHsioLib.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_HSIO_LIB_H_
#define _PCH_HSIO_LIB_H_

typedef enum  {
  PchPcieP1,
  PchPcieP2,
  PchPcieP3,
  PchPcieP4,
  PchPcieP5,
  PchPcieP6,
  PchPcieP7,
  PchPcieP8,
  PchPcieP9,
  PchPcieP10,
  PchPcieP11,
  PchPcieP12,
  PchPcieP13,
  PchPcieP14,
  PchPcieP15,
  PchPcieP16,
  PchPcieP17,
  PchPcieP18,
  PchPcieP19,
  PchPcieP20,
  PchSataP0,
  PchSataP1,
  PchSataP2,
  PchSataP3,
  PchSataP4,
  PchSataP5,
  PchSataP6,
  PchSataP7,
  PchsSataP0,
  PchsSataP1,
  PchsSataP2,
  PchsSataP3,
  PchsSataP4,
  PchsSataP5,  
  PchUpx8P0,
  PchUpx8P1,
  PchUpx8P2,
  PchUpx8P3,
  PchUpx8P4,
  PchUpx8P5,
  PchUpx8P6, 
  PchUpx8P7,   
  PchUsbP0,
  PchUsbP1,
  PchUsbP2,
  PchUsbP3,
  PchUsbP4,
  PchUsbP5,
  PchUsbP6,
  PchUsbP7,
  PchUsbP8,
  PchUsbP9,
  PchGbeP0,
  PchUfsP0
} PCH_DEVICE_PORT;

/**
  Check if the device port is available on any lane

  @param[in]  DevicePort     Device Port Number

  @retval EFI_SUCCESS
**/
EFI_STATUS
IsPortAvailable (
  PCH_DEVICE_PORT DevicePort
  );

/**
  The function returns the Port Id and lane owner for the specified lane

  @param[in]  PhyMode             Phymode that needs to be checked
  @param[out] Pid                 Common Lane End Point ID
  @param[out] LaneOwner           Lane Owner

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid lane number
**/
EFI_STATUS
EFIAPI
PchGetLaneInfo (
  IN  UINT32                            LaneNum,
  OUT UINT8                             *PortId,
  OUT UINT8                             *LaneOwner
  );

#endif // _PCH_HSIO_LIB_H_