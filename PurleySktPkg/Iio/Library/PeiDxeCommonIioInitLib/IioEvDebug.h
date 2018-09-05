//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
/*++

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


 @file  IioEvDebug.h

 Header definition
--*/

#ifndef IIOEVDEBUG_H_
#define IIOEVDEBUG_H_

#define STOP_LOGGER   0
#define START_LOGGER  1
#define STOPLOG_MASK  0x3FF

#define G3DFRMERR_MASK  0xFFFFFFFF

#define LOGGER_DEPTH     64


VOID
PcieUnlockDfx (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT8            IioIndex,
  IN  UINT8            Lock
  );

VOID
LoggerStartStop (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex,
  IN  UINT8                               PortIndex,
  IN  UINT8                               Speed,
  IN  UINT8                               Stop,
  IN  UINT8                               Mask
  ); 

VOID
LogSnapShot (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex,
  IN  UINT8                               PortIndex
 ); 


UINT8
GetAbsLane (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT8           IioIndex,
  IN  UINT8           PortIndex,
  IN  UINT8           Lane
  );

VOID
ShowRxStats (
  IN  IIO_GLOBALS     *IioGlobalData
  );


UINT32
GetPcieCtle (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex,
  IN  UINT8         Lane,
  IN  UINT16        Speed
  );

VOID
GetPcieJitterModeMargin (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN UINT8               IioIndex,
  IN  UINT8               PortIndex,
  IN  UINT16              LinkWidth
  );

VOID
SetCdrPmodPeakStep (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT8           IioIndex,
  IN  UINT8           PortIndex,
  IN  UINT8           Lane,
  IN  UINT16          Peak,
  IN  UINT16          Step
  );

UINT32
GetPcieMonBus (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex,
  IN  UINT8         Lane,
  IN  UINT32        Param
  );

UINT32
GetPcieTraceBus (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex,
  IN  UINT8         Lane,
  IN  UINT32        Param
  );


#endif /* IIOEVDEBUG_H_ */
