/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@copyright
  Copyright (c) 2013-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Platform.h

@brief
  This file contains platforms definitions.
**/
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef SSA_FLAG

#ifdef MAX_SOCKET
#define MAX_SOCKET_CNT   MAX_SOCKET  // maximum number of sockets per system
#else
#define MAX_SOCKET_CNT            (8)  // maximum number of sockets per system
#endif
#define MAX_CONTROLLER_CNT        (2)  // maximum number of controllers per socket
#define MAX_CHANNEL_CNT           (3)  // maximum number of channels per controller
#define MAX_DIMM_CNT              (2)  // maximum number of DIMMs per channel
#define MAX_RANK_CNT              (8)  // maximum number of ranks per DIMM (4 for RDIMM, 8 for LRDIMM)
#define MAX_DQDB_UNISEQ_CNT       (3)  // maximum number of DqDB unisequencers
#define DQDB_PATTERN_WIDTH        (64) // width of DqDB pattern
#define MAX_DQDB_LMN_UNISEQ_CNT   (1)  // maximum number of DqDB unisequencers that support LMN
#define MAX_DQDB_LMN_VALUES_CNT   (3)  // maximum number of DqDB LMN values, e.g., L, M, and N
#define CADB_PATTERN_ROW_CNT      (16) // maximum number of CADB pattern rows
#define CADB_PATTERN_WIDTH        (64) // width of CADB pattern
#define MAX_CADB_UNISEQ_CNT       (4)  // maximum number of CADB unisequencers
#define MAX_CADB_LMN_UNISEQ_CNT   (1)  // maximum number of CADB unisequencers that support LMN
#define MAX_CADB_LMN_VALUES_CNT   (3)  // maximum number of CADB LMN values, e.g., L, M, and N
#ifndef MAX_BUS_WIDTH
#define MAX_BUS_WIDTH             (72) // maximum bus width
#endif
#define CHUNKS_PER_CACHELINE      (8)  // number of chunks per cacheline
#define MAX_ERID_SIGNAL_CNT       (2)  // maximum number of ERID signals

#define MALLOC_HEADER_SIZE        (9)  // number of bytes in Malloc header
#define MALLOC_BUFFER_GRANULARITY (8)  // number of bytes in Malloc quantum buffer size

#endif // SSA_FLAG
#endif // __PLATFORM_H__

// end file Platform.h
