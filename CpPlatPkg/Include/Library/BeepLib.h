//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Provides services to send progress/error codes to Beep device.

  Copyright (c) 2014 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef __BEEP_LIB_H__
#define __BEEP_LIB_H__

/**
  Sends a 32-bit value to Beep device.

  Sends the 32-bit value specified by Value to Beep device, and returns Value.  
  Some implementations of this library function may perform I/O operations 
  directly to Beep device.  Other implementations may send Value to 
  ReportStatusCode(), and the status code reporting mechanism will eventually 
  display the 32-bit value on the status reporting device.
  
  Beep() must actively prevent recursion.  If Beep() is called while 
  processing another Post Code Library function, then 
  Beep() must return Value immediately.

  @param  Value  Beep count.
**/
VOID
EFIAPI
Beep (
  IN UINT32  Value
  );

/**
  Sends a 32-bit value to Beep device.

  Sends the 32-bit value specified by Value to Beep device, and returns Value.  
  Some implementations of this library function may perform I/O operations 
  directly to Beep device.  Other implementations may send Value to 
  ReportStatusCode(), and the status code reporting mechanism will eventually 
  display the 32-bit value on the status reporting device.
  
  LongBeep() must actively prevent recursion.  If LongBeep() is called while 
  processing another Post Code Library function, then 
  LongBeep() must return Value immediately.

  @param  Value  Beep count.
**/
VOID
EFIAPI
LongBeep (
  IN UINT32  Value
  );

#endif
