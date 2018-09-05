/**

Copyright (c) 2005-2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MMRC.h

  The external header file that all projects must include in order to port the MMRC.

**/

#ifndef _MMRC_H_
#define _MMRC_H_

#include "MmrcData.h"
#include "Printf.h"
#include "RegAccess.h"
#include "SysHost.h"
#include "MmrcLibraries.h"
//#include "MmrcHooks.h"

#ifndef _H2INC
#ifndef __GNUC__
//
// C intrinsic function definitions
//
#define size_t UINT32
//void * __cdecl memcpy(void * dst, void * src, size_t cnt);
//void * __cdecl memset(void *dst, char value, size_t cnt);
#endif
#endif

extern MMRC_STATUS MmrcExecuteTask (PSYSHOST MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex,UINT8 Channel);
extern VOID       SaveMrcHostStructureAddress (PSYSHOST MrcDataAddress);
extern PSYSHOST   GetMrcHostStructureAddress ();
extern VOID       MrcDeadLoop ();

extern INSTANCE_PORT_MAP              InstancePortMapGenerated[];
extern INSTANCE_PORT_OFFSET           InstancePortOffsetGenerated[];
extern UINT8                          ChannelToInstanceMapGenerated[MAX_BOXES][MAX_CHANNELS];
extern TASK_DESCRIPTOR                InitTasks[];
extern CONST FLOORPLAN                FloorPlanGenerated[MAX_CHANNELS][MAX_STROBES];
extern CONST INT16                    StrobeToRegisterGenerated[MAX_STROBES][2];
extern CONST INT16                    RankToRegisterGenerated[MAX_RANKS][2];
extern       EXTERNAL_GETSET_FUNCTION ExternalFunctions[];
extern       UINT16                   HalfClk[][NUM_FREQ];
extern       UINT8                    ADdll[];
extern CONST GET_SET_ELEMENTS         Elements[];
extern       SIGNAL_INFO              SignalInfo[];
extern       Register                 Registers[];
extern       UINT16                   Granularity[MAX_DLL_COUNT + 1][NUM_FREQ][MAX_NUM_DELAY_ELEMENTS + 1];
extern CLOCK_CROSSINGS                ClockCrossings[];
extern CONST UINT8                    InitData[];
extern       UINT16                   EarlyCommandPatternsRise[];
extern       UINT16                   EarlyCommandPatternsFall[];
extern       UINT32                   EarlyCommandResults41[];
extern       UINT32                   EarlyCommandResults48[];
extern       UINT8                    MrsCommandIndex[MAX_RANKS][MR_COUNT];
extern       char                    *OutputStrings[];
extern CONST UINT8                    UnitStringsGenerated[][MAX_BOXNAME_LENGTH];

#endif
