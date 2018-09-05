/*++        

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  DeviceDriver.h

Abstract:

  Defines and prototypes for the DeviceDriver driver
  
--*/

#ifndef _DEVICEDRIVER_H_
#define _DEVICEDRIVER_H_

//
// Init function for our driver binding protocol
//
EFI_STATUS
DriverBindingProtocolInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN OUT EFI_HANDLE       *Handle
  );
 
#endif // #ifndef _DEVICEDRIVER_H_

