//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmbAccess.h

Abstract:

  Header file for Whea generic implementation for error logging.

--*/

#ifndef _SMBACCESS_H_
#define _SMBACCESS_H_

#define SMB_READ        BIT0
#define SMB_WRITE       BIT1
#define MAX_RETRY       0x10
#define STALL_TIME      1000000 // 1,000,000 microseconds = 1 second
#define BUS_TRIES       3       // How many times to retry on Bus Errors

EFI_STATUS
AcquireSmBus (
  VOID
  );

EFI_STATUS
SmbusReadWrite (
  IN      UINT8     SlaveAddress,
  IN      UINT8     Operation,
  IN      UINT8     Command,
  IN OUT  VOID      *Buffer
  );

EFI_STATUS
CheckForAccessComplete (
  VOID
  );

EFI_STATUS
ReadWriteSmbus (
  VOID
  );


#endif
