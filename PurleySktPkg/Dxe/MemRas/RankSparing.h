//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file RankSparing.h

  This file defines the macros and function prototypes used for Rank Sparing

**/

#ifndef _RANK_SPARING_H_
#define _RANK_SPARING_H_

#include <Protocol/MemRasProtocol.h>
#include "MemRas.h"
#include "SparingEngine.h"

//more than 1 rank can wait at this status, waiting for SSR free.
#define            NONSPARESTATUS_SCRUBNEXT 8
//only 1 rank per MC can set NONSPARESTATUS_SCRUBBING and NONSPARESTATUS_DOSPARECOPY at a time
#define            NONSPARESTATUS_SCRUBBING 9
#define            NONSPARESTATUS_DOSPARECOPY  10

#define            NONSPARESTATUS_SPARECOPYDONE  11
#define            NONSPARESTATUS_TRY_TAG    12

#define         SPARERANKSTATUS_AVAILABLE         0
//only 1 rank per MC can set SPARERANKSTATUS_SCRUBBING and SPARERANKSTATUS_INPROGRESS at a time.
#define         SPARERANKSTATUS_SCRUBBING        1
#define         SPARERANKSTATUS_INPROGRESS      2
#define         SPARERANKSTATUS_NOTAVAILALBE  3

//
//0 is highest priority, 3 is the lowest priority
//

typedef union {
  struct {
    UINT32 rank_exist : 1;
    /* rank_exist - Bits[0], 
    1, for spare rank, it exist; for non-spare rank, it means it CE overflow happened.
    0, for spare rank, it doesn't exist; for non-spare rank, it means it didn't CE overflow happened.
     */
    UINT32 rank_fail : 1;
    /* rank_fail - Bits[1], 
    1, CE overflow on this rank.
    0, CE is not overflow.
     */
    UINT32 rsvd_1 : 6;
    /* rsvd_8 - Bits[7:2], 
     */
    UINT32 non_spare_rank : 1;
    /* non_spare_rank - Bits[8],
    it is a non_spare_rank.
     */
    UINT32 non_spare_status : 4;
    /* non_spare_status - Bits[12:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_2 : 3;
    /* rsvd_8 - Bits[15:13], 
     */   
    UINT32 spare_rank:1;
    /* spare_rank - Bits[16], 
     */  
    UINT32 spare_rank_status:2;
    /* spare_rank_status - Bits[18:17], 
     */      
    UINT32 rsvd_3: 5;
    /* rsvd_3 - Bits[23:19], 
     */   
     UINT32 spare_rank_sequence:2;
    /* spare_rank_sequence - Bits[25:24], 
    0 is highest priority, 3 is the lowest priority
     */       
    UINT32 rsvd_4: 6;
    /* rsvd_4- Bits[31:26], 
     */        
  } Bits;
  UINT32 Data;
} RANK_STATUS_TYPE;

typedef enum{
	NEW_SPARE_COPY,
	EVENT_SPARE_COPY		
}SPARE_COPY_TYPE;



RANK_STATUS_TYPE
EFIAPI
GetRankState(
   IN  UINT8  McNodeId,
   IN  UINT8  Ch,
   IN  UINT8  LogicalRank
);


EFI_STATUS
EFIAPI
InitializeRankSparing(
 );


/**

   check if current rank is specific state of  rank.

   @param McNodeId -- Memory controller ID
   @param Ch -- channel number.
   @param LogicalRank -- logical rank.
   @param CheckState  -- check state

    @retval TRUE  -- available state of spare rank.
    @retval FALSE -- not available state of spare rank.

**/

BOOLEAN
EFIAPI
CheckRankState(
      IN UINT8 McNodeId,
      IN UINT8 Ch,
      IN UINT8 LogicalRank,
      IN UINT8 CheckState      
);



BOOLEAN
EFIAPI
CheckRankStatePerCh(
      IN     UINT8       McNodeId,
      IN     UINT8       Ch,
      IN     UINT8       state,
      IN OUT UINT8 *     pFoundRank
);



BOOLEAN
EFIAPI
CheckRankStatePerMC(
      IN     UINT8     McNodeId,
      IN     UINT8     state,
      IN OUT UINT8 *   pFoundCh,
      IN OUT UINT8 *   pFoundRank
);

/**

   get the lowest priority spare rank.

   @param McNodeId -- Memory controller ID
   @param Ch -- channel number.
   @param LogicalRank -- pointer to logical rank.

   @retval EFI_STATUS

Note: invoker should call IsAvailSpareRankExist first.
**/

VOID 
EFIAPI
GetLowestPrioritySpareRank(
      IN   UINT8     McNodeId,
      IN   UINT8     Ch,
      OUT  UINT8*    pLogicalRank
);


#if 0
  SKX Changes
  skip scratchpad use. spareLogicalRank[] in channelNvList in host will be posted as HOB
  RankSparing.c::InitializeRankSparing() can read the details directly from it.

/**

    Update scratch pad register after spare rank is mark dirty.
    
Suggestion on implementation of biosscratchpad register:	
   Proposed setup of the registerHSX:
 BIOSNonStickyScratchpad4 (MC0)/BIOSNonStickyScratchpad6 (MC1)
 bits 0-7     one-hot encoding of ch 0 spare ranks, each bit position represents a logical rank id
 bits 8-15   one-hot encoding of ch 1 spare ranks, each bit position represents a logical rank id
 bits 16-23 one-hot encoding of ch 2 spare ranks, each bit position represents a logical rank id
 bits 24-31 one-hot encoding of ch 3 spare ranks, each bit position represents a logical rank id


 BIOSSCRATCHPAD5:
 bit 16 ch0 sparing enabled
 bit 17 ch1 sparing enabled
 bit 18 ch2 sparing enabled
 bit 19 ch3 sparing enabled
 bit 20 ch4 sparing enabled
 bit 21 ch5 sparing enabled
 bit 22 ch6 sparing enabled
 bit 23 ch7 sparing enabled

    @param NodeId  -- MC id
    @param Ch -- when value is 0xff, do whole MC; other do specified channel.
    @retval None    

**/

VOID
UpdateScratchPadReg(
   UINT8 McNodeId,
   UINT8 Ch
);
#endif

/**

   set  rank new status. New status is  should downgrade, not upgrade here.

   @param McNodeId -- Memory controller ID
   @param Ch -- channel number.
   @param LogicalRank -- logical rank index.
   @param SpareStatus -- spare rank new status. 

   @retval none

**/
VOID
EFIAPI
SetRankState(
      IN UINT8 McNodeId,
      IN UINT8 Ch,
      IN UINT8 LogicalRank,
      IN UINT8 NewState
);

VOID
EFIAPI
MarkRankDirtyFlag(
      UINT8 McNodeId,
      UINT8 Ch,
      UINT8 LogicalRank	
);


EFI_STATUS
EFIAPI
CheckAndHandleAdddcSparing(
    IN  UINT8      NodeId,
    IN  EVENT_TYPE EventType,
    OUT BOOLEAN *  IsEventHandled
);

EFI_STATUS
EFIAPI
CheckAndHandleRankSparing(
    IN  UINT8      NodeId,
    IN  EVENT_TYPE EventType,
    OUT BOOLEAN *  IsEventHandled
);

EFI_STATUS
EFIAPI
CheckAndHandleSddcPlusOneSparing(
    IN  UINT8      NodeId,
    IN  EVENT_TYPE EventType,
    OUT BOOLEAN *  IsEventHandled
);

EFI_STATUS
EFIAPI
CheckAndHandleNewRankSparingEvent(
    IN UINT8 NodeId,
    OUT BOOLEAN *IsEventHandled
 );

EFI_STATUS
EFIAPI
CheckAndHandleRankSparingDone(
    IN UINT8 NodeId,
    OUT BOOLEAN *IsEventHandled
 );

BOOLEAN
EFIAPI
IsSpareRankAvailable(
   IN  UINT8  McNodeId,
   IN  UINT8  Ch,
   IN  UINT8  LogicalRank
);

VOID
EFIAPI
DoSpareCopy(
      IN UINT8           NodeId,
      IN UINT8           Ch,
      IN UINT8           Rank,
      IN SPARE_COPY_TYPE type,
      IN SPARING_MODE    SparingMode,
      IN UINT8  RegionNum
);

#endif //_RANK_SPARING_H_
