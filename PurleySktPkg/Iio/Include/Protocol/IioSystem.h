//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2012 - 2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file IioSystem.h

    Header file for IIO driver specific Interface.

---------------------------------------------------------------------------**/
#ifndef _IIO_SYSTEM_PROTOCOL_H_
#define _IIO_SYSTEM_PROTOCOL_H_

#include <IioPlatformData.h>
#include <IioSetupDefinitions.h>

//
// IIO System driver Protocol GUID
//
// {DDC3080A-2740-4ec2-9AA5-A0ADEFD6FF9C}
#define EFI_IIO_SYSTEM_GUID \
  { \
    0xDDC3080A, 0x2740, 0x4ec2, 0x9A, 0xA5, 0xA0, 0xAD, 0xEF, 0xD6, 0xFF, 0x9C \
  }

extern EFI_GUID         gEfiIioSystemProtocolGuid;

typedef struct _PORT_DESCRIPTOR{
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
}PORT_DESCRIPTOR;

typedef struct _PORT_ATTRIB{
  UINT8   PortWidth;
  UINT8   PortSpeed;
}PORT_ATTRIB;

EFI_STATUS
IioGetCpuUplinkPort (
    UINT8             IioIndex,
    PORT_DESCRIPTOR   *PortDescriptor,    //Bus, Device, function
    BOOLEAN           *PortStatus,        //TRUE if enabled else disabled
    PORT_ATTRIB       *PortAttrib         //width and speed
);

typedef
EFI_STATUS
(EFIAPI *IIO_GET_CPU_UPLINK_PORT) (
  IN  UINT8             IioIndex,
  OUT PORT_DESCRIPTOR   *PortDescriptor,
  OUT BOOLEAN           *PortStatus,
  OUT PORT_ATTRIB       *PortAttrib
);

typedef struct _EFI_IIO_SYSTEM_PROTOCOL{
  IIO_GLOBALS               *IioGlobalData;
  IIO_GET_CPU_UPLINK_PORT   IioGetCpuUplinkPort;
} EFI_IIO_SYSTEM_PROTOCOL;

#endif //_IIO_SYSTEM_PROTOCOL_H_