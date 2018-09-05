//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/**

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


    @file RasMpLinkProtocol.h
    
  GUID for MPLINK RAS protocol

**/

#ifndef _RAS_MPLINK_PROTOCOL_H_
#define _RAS_MPLINK_PROTOCOL_H_

//#include "Tiano.h"
//#include "MpLinkRcParm.h"
#include <SysHost.h>

//
// Forward reference for pure ANSI compatability
//

#define EFI_RAS_MPLINK_PROTOCOL_GUID \
   { \
      0xbd1c3cc1, 0x22f6, 0x4274, 0xa1, 0xa1, 0x2e, 0x80, 0x1d, 0x60, 0x1c, 0x36 \
   }

//EFI_FORWARD_DECLARATION (EFI_RAS_MPLINK_PROTOCOL);

typedef enum {
  RAS_MPLINK_EVNT_ONLINE = 0,      // Logical/Physical Online event
  RAS_MPLINK_EVNT_OFFLINE,         // Logical/Physical Offline event
  RAS_MPLINK_EVNT_RESET_SM         // Reset MPLINK RAS state machine
}RAS_MPLINK_EVENT_TYPE;

#define  RAS_MPLINK_SOCKET_TYPE_CPU       0
#define  RAS_MPLINK_SOCKET_TYPE_IOH       1
#define  RAS_MPLINK_SOCKET_INFO_NA        0xFF

typedef enum {
  RAS_MPLINK_SUCCESS = 0,  // The event handling completed and register settings are committed
  RAS_MPLINK_PENDING,      // Some more work related to the event is pending (for Time sliced SMI RAS support)
  RAS_MPLINK_INT_DOMAIN_PENDING,      // Indicate CPU online is complete except for Int domain setup  (for Time sliced SMI RAS support)
  RAS_MPLINK_FAILURE_EVT_IN_PROGRESS,  // Error: An event is already in progress
  RAS_MPLINK_FAILURE_INVALID_EVTTYPE,  // Error: Invalid Event type
  RAS_MPLINK_FAILURE_CANNOT_OFFLINE,   // Error: Socket can not be offlined because it is needed to reach other sockets
  RAS_MPLINK_FAILURE_SOC_UNREACHABLE,  // Error: Hot Added Socket is unreachable
  RAS_MPLINK_FAILURE_CHKIN,            // Error: Hot Added Socket failed to check in
  RAS_MPLINK_FAILURE_SOC_MISMATCH,     // Error: Hot Added Socket mismatch to SBSP
  RAS_MPLINK_FAILURE_GENERAL           // Error: MPLINK RAS handler general failure
}RAS_MPLINK_STATUS;

// Protocol Interfaces

//
// To cehck if onlining/offlining a given socket can succeed. If the onlined socket is not reachable from
// (i.e none of the existing sockets are immediate neighbor of the onlined socket) 
// or if the socket to be offlined is part of a pass thru path and if it is the only path to reach other
// sockets in the system, then the socket online/offline request will be rejected. For online event,
// the caller should make sure the socket is powered on before calling this API.
//
typedef 
RAS_MPLINK_STATUS
//EFI_RUNTIMESERVICE 
(EFIAPI *EFI_RAS_MPLINK_CHECK_EVENT) (
  IN UINT8          EventType,
  IN UINT8          SocketInfo
);

//
// To be called for each O*L event that affects the MPLINK fabric. After successful handling of the event,
// the MPLINK RAS driver will Quiesce the system and commit the register settings.
// This API is time sliced; so it should be called repeatedly when the return code is "RAS_MPLINK_PENDING"
// or "RAS_MPLINK_INT_DOMAIN_PENDING"; return code "RAS_MPLINK_SUCCESS" indicates successful hanlding;
// all other codes indicate failure. In all failure cases, the RAS_MPLINK driver will reset the state machine and
// return the error code. The O*L handling can be aborted by calling this API with event type set to
// "RAS_MPLINK_EVNT_RESET_SM".
//
typedef 
RAS_MPLINK_STATUS
//EFI_RUNTIMESERVICE 
(EFIAPI *EFI_RAS_MPLINK_HANDLER) (
  IN UINT8          EventType,
  IN UINT8          SocketInfo
);

//
// To be called to include/exclude the onlined socket in IPI target list of the SBSP
//
typedef 
RAS_MPLINK_STATUS
//EFI_RUNTIMESERVICE 
(EFIAPI *EFI_RAS_MPLINK_ADJUST_IPI_LIST) (
  IN BOOLEAN        Include
);

//
// To be called for RAS event that affects system wide memory configuration. For example,
// if there is some change in system memory configuration including mirror mode, hemisphere setting, etc.
// this API needs to be called to change the System Coherency setting accordingly. Note that the caller must
// make sure this API can acquaire the Quiesce buffer. This API will release the Quiesce buffer before
// returning. Also, the caller is responsible for committing the register settings produced by this API.
//
typedef 
RAS_MPLINK_STATUS
//EFI_RUNTIMESERVICE 
(EFIAPI *EFI_RAS_MPLINK_COH_HANDLER) (
  IN BOOLEAN        HemiMode,
  IN UINT8          SlaveB0List,
  IN UINT8          SlaveB1List
);

// To get Ras Host pointer in MEM RAS drivers
typedef 
RAS_MPLINK_STATUS
(EFIAPI *EFI_RAS_MPLINK_GET_MEM_RAS_HOST) (
  IN OUT PSYSHOST     memRasHost
);

typedef struct _EFI_RAS_MPLINK_PROTOCOL {
  BOOLEAN                       HostParmValid; // Flag to indicate if the MPLINK system info is valid
  // BKL_PORTING
  SYSHOST                          *Host; // All the latest MPLINK info of the system
  EFI_RAS_MPLINK_CHECK_EVENT       RasMpLinkCheckEvent;
  EFI_RAS_MPLINK_HANDLER           RasMpLinkHandler;
  EFI_RAS_MPLINK_ADJUST_IPI_LIST   RasMpLinkAdjustIpiList;
  EFI_RAS_MPLINK_COH_HANDLER       RasMpLinkCohHandler;
  EFI_RAS_MPLINK_GET_MEM_RAS_HOST  RasMpLinkGetMemRasHost;
} EFI_RAS_MPLINK_PROTOCOL;

extern EFI_GUID gEfiRasMpLinkProtocolGuid;

//Porting for Platform reqd
// dummy definition for structure; to be later replaced by native definition
// for Platform

typedef struct{
  UINT8 dummy;
}MPLINK_PARM;
//BKL_PORTING

#endif
