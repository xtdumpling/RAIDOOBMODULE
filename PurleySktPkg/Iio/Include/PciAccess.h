//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PciAccess.h

  Macros to simplify and abstract the interface to PCI configuration.

**/

#ifndef _PCIACCESS_H_
#define _PCIACCESS_H_

VOID
IioPciExpressWrite32 (  
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset,
  IN UINT32            Data
  );
  
VOID
IioPciExpressWrite16 ( 
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset,
  IN UINT16            Data
  );

VOID
IioPciExpressWrite8 ( 
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset,
  IN  UINT8             Data
  );

UINT32
IioPciExpressRead32 ( 
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset
  );
  
UINT16
IioPciExpressRead16 ( 
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset
  );
  
UINT8
IioPciExpressRead8 ( 
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset
  );

VOID
IioWriteCpuCsr32 (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             SocId,
  IN  UINT8             BoxInst,
  IN  UINT32            RegOffset,
  IN  UINT32            Data
  );
  
VOID
IioWriteCpuCsr16 ( 
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             SocId,
  IN  UINT8             BoxInst,
  IN  UINT32            RegOffset,
  IN  UINT16            Data
  );

VOID
IioWriteCpuCsr8 ( 
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            RegOffset,
  IN  UINT8             Data
  );
  
UINT32
IioReadCpuCsr32 ( 
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             SocId,
  IN  UINT8             BoxInst,
  IN  UINT32            Cs
  );

UINT16
IioReadCpuCsr16 ( 
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            Csr
  );
  
UINT8
IioReadCpuCsr8 ( 
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            Csr
  );

UINT32
IioBios2PcodeMailBoxWrite (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8       Iio, 
  IN  UINT32      MailBoxCommand, 
  IN  UINT32      MailboxData
);

VOID
IioBreakAtCheckPoint (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8       MajorCode,
  IN  UINT8       MinorCode,
  IN  UINT16      Data
 );
#endif // _PCIACCESS_H_
