/**
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file Abstract:


Revision History:

**/

#ifndef _IIOGEN3_H_
#define _IIOGEN3_H_


struct TxEqCoefficient
{
  UINT8 Precursor;
  UINT8 Cursor;
  UINT8 Postcursor;
};

typedef UINT32 GEN3_STATUS;

typedef struct  {
  UINT16  Vid;
  UINT16  Did;
  UINT8   Rsvd1;
  UINT8   Platform;
  UINT8   Socket;
  UINT16  Port;
  UINT8   UpstreamTxEqValid;
  struct  TxEqCoefficient UpstreamTxEq;
  UINT8   DownstreamTxEqValid;
  struct  TxEqCoefficient DownstreamTxEq;
} PORTPARAMS_STRUCT;

typedef union {
  struct {
    UINT16 Vid;
    UINT16 Did;
    UINT8  FsLfValid;
    UINT16 Fs;
    UINT16 Lf;
    UINT8  UpstreamTxEqValid;
    struct TxEqCoefficient UpstreamTxEq;
    UINT8  DownstreamTxEqValid;
    struct TxEqCoefficient DownstreamTxEq;
  } LinkParams;
  PORTPARAMS_STRUCT PortParams;
} GEN3OVERRIDE_STRUCT;

VOID
CheckGen3Override (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8 Socket
);

GEN3_STATUS
Gen3Override(
  IN  IIO_GLOBALS                      *IioGlobalData,
  IN  UINT8                             Socket,
  IN  UINT8                             Port,
  IN  UINT8                             Phase2EqMode,
  IN  UINT8                             Phase3EqMode
    );

GEN3_STATUS
Gen3PhyReset (
  IN  IIO_GLOBALS                      *IioGlobalData,
  IN  UINT8                             Socket,
  IN  UINT8                             Port,
  IN  UINT8                             ResetType 
  );


VOID
FixLinkDegrades (
  IN  IIO_GLOBALS                      *IioGlobalData,
  IN  UINT8                             Socket,
  IN  UINT8                             Port
  );

VOID
CheckGen3PreLinkOverride (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8 Socket
  );

VOID
MiscWA_PreLinkData(
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             Socket,
  IN  UINT8                             Port
  );


GEN3_STATUS
Gen3RetrainLink(
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             IioIndex,
  IN   UINT8                             PortIndex
  );

VOID
PresetOverride(
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8       IioIndex,
  IN  UINT8       PortIndex
);
#endif // _IIO_GEN3_H_

