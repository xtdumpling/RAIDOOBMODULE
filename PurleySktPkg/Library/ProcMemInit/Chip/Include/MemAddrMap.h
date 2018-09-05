//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
*
* Reference Code
*
* ESS - Enterprise Silicon Software
*
* INTEL CONFIDENTIAL
*
* Copyright (c) 2016..2017, Intel Corporation. All rights reserved.
*
* The source code contained or described herein and all documents
* related to the source code ("Material") are owned by Intel
* Corporation or its suppliers or licensors. Title to the Material
* remains with Intel Corporation or its suppliers and licensors.
* The Material contains trade secrets and proprietary and confidential
* information of Intel or its suppliers and licensors. The Material
* is protected by worldwide copyright and trade secret laws and treaty
* provisions.  No part of the Material may be used, copied, reproduced,
* modified, published, uploaded, posted, transmitted, distributed, or
* disclosed in any way without Intel's prior express written permission.
*
* No license under any patent, copyright, trade secret or other
* intellectual property right is granted to or conferred upon you
* by disclosure or delivery of the Materials, either expressly,
* by implication, inducement, estoppel or otherwise. Any license
* under such intellectual property rights must be express and
* approved by Intel in writing.
************************************************************************
*
*  PURPOSE:
*
*      This file contains memory mapping data for DDR4/DDRT
*
*
************************************************************************/


#ifndef _MEM_ADDR_MAP_H_
#define _MEM_ADDR_MAP_H_

#include "SysFunc.h"
#include "SysFuncChip.h"
#include "SysHostChip.h"
#include "MemFuncChip.h"
#include "FnvAccess.h"
#include "KtiSetupDefinitions.h"
#include "MaxSocket.h"

#ifdef MEMMAPSIM_BUILD
extern BOOLEAN MMS_UseNVMDIMMFWInterface;
#endif


//
// Disable warning for unused input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif //_MSC_VER

//
// Memory mapping defines
//
#define MAX_TWO_LM_COMBINATION  7
#define MAX_TWO_WAY_COMBINATION 3
#define MAX_ONE_WAY_COMBINATION 3

#define MEM_SIZE_16TB 16*1024
#define MEM_SIZE_32TB 32*1024

#define THREE_WAY 3
#define TWO_WAY 2
#define ONE_WAY 1

#define SOCKET_BITMAP_ALL 0xFF
#define INTER_BELOW_4GB_EN 1
#define INTER_BELOW_4GB_DIS 0

#define INTERLEAVE_LIST_BITS_CHANNEL  6
//align to GB boundary
#define GB_BOUNDARY_ALIGN 0xFFFFFFF0
#define GB_ALIGN_NVM 0xFFF0

// CR controller type (same present in CrystalRidge.h)
#define FNV 0x979
#define EKV 0x97A
#define BWV 0x97B

#define MAILBOX_BIOS_CMD_READ_SKU_LIMIT   0xBA  // New in SKX. Bios reads the current SKU limit for the socket

#define MC1 0x1  // to get to proper MC index by adding this value 

// memory map SKU limit 
typedef struct  {
  UINT32 SkuLimit;             // in 64GB units
  UINT32 MemoryAllocated;      // memory mapped to SPA(already allocated)
  UINT8  SkuLimitValid;        // 0-FALSE, 1-TRUE; error/success per socket
  UINT8  Violation;            // 0-FALSE, 1-TRUE
} MEMORY_MAP_SKU_LIMIT;

// private memory map DS for SKX source; This private data structure will be used more extensively in subsequent generations.
typedef struct {
  MEMORY_MAP_SKU_LIMIT MemMapSkuLimit[MAX_SOCKET];  // CPU SKU Limit
} MEMORY_MAP_DATA;

typedef struct socketNvram  SOCKET_NVRAM;
typedef struct SADTable     SAD_TABLE;

//
// Function declarations
//

// MemAddrMap.c
/**
  
  This routine is to get/retrieve SKU limit value per socket in the system from PCU
 
  @param host             - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  @retval void

**/
VOID InitializeSkuLimits (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData
);

/**
  
  This routine is to check whether there is a SKU limit  
  violation per socket basis
 
  @param host             - Pointer to sysHost  
  @param MemMapData  - Pointer to MemMapData (Private DS  
                      maintained by Memory decoder)
  @param MemType          - Type of memory  
  @param McList           - List of Memory controllers populated
  
  @retval BOOLEAN - TRUE (SKU Limit violation) FALSE (No SKU  
         Limit violation)

**/
BOOLEAN IsSkuLimitViolation(
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData,
  UINT32 MemoryToBeAllocated,
  UINT8  MemType,
  UINT8  McList[MAX_SOCKET*MAX_IMC]
);

/**
  
  This routine is to initilize value per socket for  
  known/desired values  
   
  @param host        - Pointer to sysHost  
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  
  @retval void

**/
VOID ResetSkuLimitsViolation (
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData
);

/**

  This routine initializes memory mapping parameters based on setup options and system capability.

  @param host  - Pointer to sysHost

  @retval VOID

**/
void
InitMemMapParams (
  PSYSHOST host
  );

/**

  This routine checks the values of the Memory address mapping setup options.

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
CheckMemAddrMapSetupOptionsValues (
  PSYSHOST host
  );

/**

  This routine initializes the memory size fields in the structures
  for DIMMs, Channels, and Nodes.  The sizes are calculated from the sizes in
  the rank structures.

  @param host  - Pointer to sysHost

  @retval status

**/
void
PopulateMemorySizeFields (
  PSYSHOST host
  );

/**

  This routine checks if partitioning is requested form setup
  or BIOS knobs and partitions the dimm as per the requested ration.

  @param host     - Pointer to sysHost
  @param sckt   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval status

**/
UINT32 PartitionDDRT (
  PSYSHOST host,
  UINT8 sckt,
  UINT8 ch,
  UINT8 dimm
  );

/**

  This routine initializes the memory size fields in the structures
  for DIMMs, Channels, and Nodes.  The sizes are calculated from the sizes in
  the rank structures.

  @param host  - Pointer to sysHost

  @retval void

**/
void
UpdateDdrtFields (
  PSYSHOST host
  );

/**

  This routine aligns the volCap of all the channels in every NVMCTLR dimm in the system to the nearest GB.
  This is to suffice a silicon requirement when SNC is enabled.

  @param host  - Pointer to sysHost

  @retval void

**/
void
AlignVolCaptoGB (
  PSYSHOST host
  );

/**

  This routine checks if the system configuration has capability to run in 2LM mode.
  If not, it sets the volMemMode variable to 1lm and returns an error.
  This error caus the system to restart the memory mapping code in 1LM mode.

  @param host  - Pointer to sysHost

  @retval SUCCESS - If 2lm mode can be supported
  @retval FAILURE - If 2lm mode cannot be supported and we need to restart the memory mapping in 1lm mode.

**/
UINT32
AdjustVolatileMemMode (
  PSYSHOST host
  );

/**

  Init memory size variables based on the memtype

  @param host  - Pointer to sysHost
  @param memType  - - Type of memory region to be mapped

  @retval N/A

**/
UINT8 InitBlkCtrlRegion (
  PSYSHOST host,
  UINT8 memType
  );

/**

  This routine Creates SAD rules for the memory to be mapped into the system address space.

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  @retval status

**/
UINT32
CreateSADRules (
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData
  );

/**

  Interleaves memory on the channel level across sockets. This

  routine figures out the mapping and fills in the structures for the SAD, TAD,
  and SAG tables.  It does not program any registers.

  @param host      - Pointer to sysHost  
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM      - Top of low memory

  @retval status

**/
UINT32
SocketInterleave (
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16 TOLM
  );

/**

  Interleaves memory across sockets. This routine figures out the mapping and
  fills in the structures for the SAD tables.  It does not program any
  registers.

  @param host      - Pointer to sysHost 
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM      - Top of low memory
  @param memType   - Memory type

  @retval status


**/
UINT32
SADInterleave (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16    TOLM,
  UINT8     memType
  );

/**

  Description: Check if near memory population matches between IMCs

  @param host  - Pointer to sysHost
  @param socketNum - socket id

  @retval True -  Near Memory population matches between IMCs
  @retval False - Near Memory population does not match between IMCs

**/
BOOLEAN
checkNMPopulation (
  PSYSHOST host,
  UINT8 socketNum
  );

/**

  This routine looks for NVM dimms with PMEM partition

  @param host         - Pointer to sysHost
  @param socketBitMap - BitMap of sockets to process

  @retval SUCCESS

**/
BOOLEAN
IsPmemPossible (
  PSYSHOST  host,
  UINT8     socketBitMap
  );

/**

  Description: This routine fills the 2LM matrix below based on the near memory population

---------------------------------------------------------------------------------------------------------
  NM population/IMC  ||   IMC0 |  IMC1 |  IMC2 |  IMC3 |  IMC4 |  IMC5 |  IMC6 |  IMC7  ||   Counter
---------------------------------------------------------------------------------------------------------
                     ||        |       |       |       |       |       |       |        ||
  0 0 1              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  0 1 0              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  0 1 1              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  1 0 0              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  1 0 1              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  1 1 0              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  1 1 1              ||        |       |       |       |       |       |       |        ||
---------------------------------------------------------------------------------------------------------

  @param host  - Pointer to sysHost
  @param matrix   -  structure to hold the above matrix
  @param counter   - Structure that holds the number of imcs that could participate in the interleave
  @param ddrtChInterleave  -  NVMDIMM channel interleave that is being verified
  @retval counter - number of imcs that can participate in this interleave

**/
UINT8
FillMatrix_2LM (
  PSYSHOST host,
  UINT16 matrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8 counter[MAX_TWO_LM_COMBINATION],
  UINT8 ddrtChInterleave
  );

/**

  Description: This routine check validaty of the imc ways and adjusts it to the nearest valid IMC ways.

  @param host  - Pointer to sysHost
  @param map   -  structure to hold the interleaveable size in each participating imc
  @param counter  -  Number of imcs capable of participating in the interleave.
  @retval counter - number of imcs that can participate in this interleave

**/
UINT8
AdjustSADWays (
  PSYSHOST host,
  UINT16 map[MAX_IMC * MAX_SOCKET],
  UINT8 counter
  );

/**

  Description: This rountine fill the two way interleave table

  @param host  - Pointer to sysHost
  @param TwoWayMap   -  structure to hold the interleaveable size in each participating imc
  @param TwoWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param TwoWayCombnationIndex - The index to the combination that is most efficient


  @retval VOID

**/
VOID
FillTwoWayInterTable_2lm (
  PSYSHOST host,
  UINT16  TwoWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   TwoWayCount[MAX_TWO_WAY_COMBINATION],
  UINT8   nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_TWO_WAY_COMBINATION],
  UINT8   TwoWayCombnationIndex
  );

/**

  Description: This rountine fill the two way interleave table

  @param host  - Pointer to sysHost
  @param TwoWayMap   -  structure to hold the interleaveable size in each participating imc
  @param TwoWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param TwoWayCombnationIndex - The index to the combination that is most efficient


  @retval VOID

**/
VOID
FillTwoWayInterTable (
  PSYSHOST host,
  UINT16  TwoWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   TwoWayCount[MAX_TWO_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_TWO_WAY_COMBINATION],
  UINT8   TwoWayCombnationIndex
  );

/**

  Description: This routine check if two way UMA interleave is possible

  @param Host - Pointer to sysHost
  @param MemType - memory type
  @param TwoWayMap -  structure to hold the interleaveable size in each participating imc
  @param TwoWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param InterleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param NmInterleaveBitMap - structure that holds the near memory interleave information

  @retval Counter - number of imcs participating in the interleave combination that yield maximum interleave.

**/
UINT8
IsTwoWayInterleave (
  SYSHOST 	*Host,
  UINT8   	MemType,
  UINT16  	TwoWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   	TwoWayCount[MAX_TWO_WAY_COMBINATION],
  UINT16  	InterleavableSize[MAX_TWO_WAY_COMBINATION],
  UINT8   	NmInterleaveBitMap[MAX_TWO_WAY_COMBINATION]
  );

/**

  Description: Do two way UMA interleave

  @param host  - Pointer to sysHost  
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM  - top of low memory
  @param TwoWayMap   -  structure to hold the interleaveable size in each participating imc
  @param TwoWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param interleaveOption - structure to hold the interleaveble size in each of the possible imc combination
  @param nodeLimit - Pointer to return Limit of the sad entry created.

  @retval status
**/
UINT32
DoTwoWayUMAInterleave (
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16    TOLM,
  UINT8   memType,
  UINT16  TwoWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   TwoWayCount[MAX_TWO_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_TWO_WAY_COMBINATION],
  UINT8   nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION],
  UINT8   *interleaveOption,
  UINT32  *nodeLimit
  );

/**

  Description: This rountine fill the one way interleave table for 2lm

  @param host  - Pointer to sysHost
  @param OneWayMap   -  structure to hold the interleaveable size in each participating imc
  @param OneWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information
  @param oneWayCombnationIndex - The index to the combination that is most efficient


  @retval VOID

**/
VOID
FillOneWayInterTable_2lm (
  PSYSHOST host,
  UINT16  oneWayMap[MAX_ONE_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   oneWayCount[MAX_ONE_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_ONE_WAY_COMBINATION],
  UINT8  nmInterleaveBitMap[MAX_ONE_WAY_COMBINATION],
  UINT8   oneWayCombnationIndex
  );

/**

  Description: This rountine fill the one way interleave table

  @param host  - Pointer to sysHost
  @param OneWayMap   -  structure to hold the interleaveable size in each participating imc
  @param OneWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param OneWayCombnationIndex - The index to the combination that is most efficient


  @retval VOID

**/
VOID
FillOneWayInterTable (
  PSYSHOST host,
  UINT16  OneWayMap[MAX_ONE_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   OneWayCount[MAX_ONE_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_ONE_WAY_COMBINATION],
  UINT8   OneWayCombnationIndex
  );

/**

  Description: This routine check if one way UMA interleave is possible

  @param host  - Pointer to sysHost
  @param OneWayMap   -  structure to hold the interleaveable size in each participating imc
  @param OneWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information

  @retval counter - number of imcs participating in the interleave combination that yield maximum interleave.

**/
UINT8
IsOneWayInterleave (
  PSYSHOST host,
  UINT8   memType,
  UINT16  OneWayMap[MAX_ONE_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   OneWayCount[MAX_ONE_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_ONE_WAY_COMBINATION],
  UINT8   nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION]
  );

/**

  Description: Do one way UMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM  - top of low memory
  @param OneWayMap   -  structure to hold the interleaveable size in each participating imc
  @param OneWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param nodeLimit - Pointer to return Limit of the sad entry created.

  @retval status
**/
UINT32
DoOneWayUMAInterleave (
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16    TOLM,
  UINT8   memType,
  UINT16  OneWayMap[MAX_ONE_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   OneWayCount[MAX_ONE_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_ONE_WAY_COMBINATION],
  UINT8   nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION],
  UINT8   *interleaveOption,
  UINT32  *nodeLimit
  );

/**

  Description: This routine check if three way UMA interleave is possible

  @param host  - Pointer to sysHost
  @param matrix   -  structure to hold the interleaveable size in each participating imc

  @retval counter - number of imcs that can participate in this interleave

**/
UINT8
IsThreeWayInterleave (
  PSYSHOST host,
  UINT8 memType,
  UINT16 matrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8 *Index
  );

/**

  Description: This routine check if three way UMA interleave is possible

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData(Private DS maintained by Memory decoder)
  @param TOLM  - Top of Low Memory
  @param matrix   -  structure that holds the interleaveable size in each participating imc
  @param counter - number of imcs that can participate in this interleave
  @param nodeLimit - Pointer to return the limit of SAd entry created

  @retval status

**/
UINT32
DoThreeWayUMAInterleave (
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16    TOLM,
  UINT8     memType,
  UINT16    matrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8     Index,
  UINT8     counter,
  UINT32    *nodeLimit
  );

/**

 Interleave persistent memory on IMC and channel level but not across sockets for NUMA configuration.

  @param host                - Pointer to sysHost
  @param MemMapData     - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum           - Socket number
  @param TOLM                - Top of low memory

  @retval status

**/
UINT32
SocketNonInterleavePerMem (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8     socketNum,
  UINT16    TOLM
  );

/**

  Interleave volatile (DDR4/HBM) memory on IMC and channel level but not across sockets for NUMA configuration.

  @param host                - Pointer to sysHost
  @param MemMapData     - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum           - Socket number
  @param TOLM                - Top of low memory

  @retval status

**/
UINT32
SocketNonInterleaveVolMem (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8     socketNum,
  UINT16    TOLM
  );

/**

  Description: Interleaves memory on the channel level but not across sockets.
  This routine figures out the mapping and fills in the structures for the SAD, TAD,
  tables.  It does not program any registers.

  @param host      - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM      - Top of low memory

  @retval status

**/
UINT32
SocketNonInterleave (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16    TOLM
  );

/**

  This routine copies the volSize/DDR4 size into the remSize field of each channel in the sockets specified based on the volatile memory mode

  @param host         - Pointer to sysHost
  @param socketBitMap - BitMap of sockets to process

  @retval SUCCESS

**/
VOID
UpdateHostStructForVolMem(
  PSYSHOST   host,
  UINT8     socketBitMap
  );

/**

  Interleaves memory on the mc and channel level but not across sockets for NUMA configuration.
  This routine figures out the mapping and fills in the structures for the SAD and TAD
  tables.  It does not program any registers.

  @param host                - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum           - Socket number
  @param memType             - Type of memory being interleaved
  @param TOLM                - Top of low memory

  @retval status

**/
UINT32 SADNuma (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8     socketNum,
  UINT16    TOLM,
  UINT8     memType
  );

/**

  This routine copies the pmem size into the remSize field of each channel in the sockets specified

  @param host         - Pointer to sysHost
  @param socketBitMap - BitMap of sockets to process

  @retval void

**/
VOID
UpdateHostStructForPmem (
  PSYSHOST   host,
  UINT8     socketBitMap
  );

/**

  Description: Check if NUMA interleave is possible between the imcs in a socket

  @param host  - Pointer to sysHost
  @param socketNum - socket id
  @param way   - two way/threeway/one way interleave


  @retval True -  Interleave possible
  @retval False -  Interleave possible

**/
BOOLEAN
IsInterleavePossible (
  PSYSHOST host,
  UINT8 memType,
  UINT8 socketNum,
  UINT8 way
  );

/**

  Description: sort the channels in the socket based on the remsize value in descending order.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param chIndex - Index of first channel in the IMC that needs to be sorted
  @param channelSortList - structure to hold the sorted entries

  @retval sortnum - number of channels left with unallocated memory in the IMC

**/
UINT8
ChannelSort (
  PSYSHOST host,
  UINT8 socketNum,
  UINT8 chIndex,
  struct channelSort channelSortList[]
  );

/**

  Description: Do three way NUMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum - socket id
  @param TOLM  - Top of Low Memory


    @retval status
**/
UINT32
DoThreeWayInterleave (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8     socketNum,
  UINT16    TOLM,
  UINT8     memType
  );

/**

  Description: Do two way NUMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum - socket id
  @param TOLM  - Top of Low Memory


    @retval status
**/
UINT32
DoTwoWayInterleave (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8   socketNum,
  UINT16    TOLM,
  UINT8   memType
  );

/**

  Description: Do one way NUMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum - socket id
  @param TOLM  - Top of Low Memory


    @retval status
**/
UINT32
DoOneWayInterleave (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8   socketNum,
  UINT16    TOLM,
  UINT8   memType
  );

/**

  Description: Find the next SAD entry that needs to be filled for this Socket.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id


  @retval Index of Next available SAD entry

**/
UINT8
FindSadIndex (
  PSYSHOST  host,
  UINT8 sckt
  );

/**

  Description: Add a SAD entry in  socket that are not involved in the SAD interleave.

  @param host  - Pointer to sysHost
  @param map  - map of all the IMCs that are part of the interleave.
  @param nodeLimit - Limit value for the SAD entry

  @retval status

**/
UINT32
AddRemoteSadEntry (
  PSYSHOST  host,
  UINT16  map[MAX_SOCKET * MAX_IMC],
  UINT32  nodeLimit,
  UINT8   type
  );

/**

  Description: Find the NM population in the specified MC and fill the bitmap accordingly

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param mcIndex - mc id

  @retval Bit map of NM population in the given MC

**/
UINT8
FindNMBitMap (
  PSYSHOST  host,
  UINT8 sckt,
  UINT8 mc
  );

/**

  Description: Interleave the memory between the channels within an IMC of a socket

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum  - socket id
  @param TOLM - Top Of Low Memory
  @param below4GBInterFlag - Flag to check whether we are interleaving above or
  below TOLM

  @retval status
**/
UINT32
DoChannelInterleave (
  PSYSHOST  host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8     socketNum,
  UINT16    TOLM,
  UINT8     memType,
  UINT32    *nodeLimit,
  UINT8     below4GBInterFlag
  );

/**

  Description: Deteremine the target interleave granularity based on the memType

  @param host  - Pointer to sysHost
  @param memType - Memory type

  @retval Target interleave granularity
**/
UINT8
GetTargetInterleaveGranularity (
  PSYSHOST  host,
  UINT8     memType
  );

/**

  Check NVMDIMM presence in the socket

  @param host           - Pointer to sysHost
  @param socketBitMap   - BitMap of the sockets that need to be investigated.

  @retval TRUE /FALSE based on NVMDIMM presense

**/
BOOLEAN
IsDDRTPresent (
  PSYSHOST host,
  UINT8 socketBitMap
  );

/**

  Create SADs for Block or NVMCTLR CSR/Mailbox/Ctrl region

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM - Top Of Low Memory
  @param memType  - Type of memory region to be mapped

  @retval status

**/
UINT32
CreateNewRegionSAD (
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16 TOLM,
  UINT8 memType
  );

/**

  This degrades the memory mapping level and reruns the memory mapping algorithm.


  @param host  - Pointer to sysHost

  @retval ERROR_RESOURCE_CALCULATION_FAILURE

**/
UINT32
DegradeMemoryMapLevel (
  PSYSHOST host
  );

/**

  This routine Creates TAD rules in each IMC for the memory to be mapped into the system address space.

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
CreateTADRules (
  PSYSHOST host
  );

/**

  This routine Creates RIRs in each channel for the memory to be mapped into the system address space.

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
CreateRIRRules (
  PSYSHOST host
  );

/**

  This routine provides a hook for RAS module to patch memory map before being written to CSRs.

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
MemoryMapRASHook (
  PSYSHOST host
  );

/**

  This routine provides a hook to perform final adjustments to the internal strructure related to memory map.
  One important function that happens here is, that this is where we set the resourceCalculationFlagDone flag and rerun the memap algorithm.

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
FinalizeMemoryMap (
  PSYSHOST host
  );

/**

  Display Memory map Info

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
DisplayMemoryMapInfo (
  PSYSHOST host
  );

/**

  Computes a list of ranks sorted by size, largest to smallest.

  @param host        - Pointer to sysHost
  @param rkSortList  - Rank sort list
  @param sckt      - Socket Id
  @param ch          - Channel number (0-based)

  @retval sortNum - Number of ranks with memory remaining to be allocated
            @retval (sortNum may be 0 indicating all memory allocated)

**/
UINT8
RankSort (
  PSYSHOST host,
  struct rankSort rkSortList[],
  UINT8 sckt,
  UINT8 ch
  );


/**

  Interleaves memory on the rank level for DDR and NVM memory.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
RankInterleave (
  PSYSHOST host
  );

/**

  Interleaves memory on the rank level. This routine figures out the
   mapping and fills in the structures for the RIR tables for DDR.
  It does not program any registers.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
RankInterleaveDDR (
  PSYSHOST host
  );

/**

  Interleaves memory on the rank level. This routine figures out the
  mapping and fills in the structures for the RIR tables for DDRT.
  It does not program any registers.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
RankInterleaveDDRT (
  PSYSHOST host
  );

/**

  This routine initializes the dimm NVRAM struct for the given dimm with data read from the SPD of the dimm.

  @param host  - Pointer to sysHost

  @retval void

**/
void
GetDimmInfoFromSPD (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  DIMMINFORMATION  dimmInformation
  );

/**

Routine Description

  Description:  Encode target interleave granularity based on the target granularity

  @param host - Pointer to sysHost
  @param tgtGranularity - Target granularity

  @retval Encoded granularity
**/
UINT8
SADEncodeTargetInterleaveGranularity (
  PSYSHOST host,
  UINT8 tgtGranularity
  );

/**

Routine Description

  Description:  Encode interleave granularity based on the SAD entry type and granularity

  @param host - Pointer to sysHost

  type - SAD entry type
  granularity - SAD entry granularity

  @retval Encoded interleave

**/
EncodedInterleaveGranularity
SADEncodeInterleaveGranularity (
  PSYSHOST host,
  UINT8 type,
  UINT8 granularity
  );

/**

  Description: Encode Limit field for all DRAM rules
  Limit is encoded such that valid address range <= Limit

  @param Limit - Limit to encode

  @retval Encoded Limit

**/
UINT32
SADTADEncodeLimit (
  UINT32 Limit
  );

/**

  Description: Encode Mod3AsMod2 field for all DRAM rules

  @param ChannelInterleaveBitMap - Channel interleave bitmap to encode

  @retval Encoded Mod3AsMod2

**/
UINT32
SADEncodeMod3AsMod2 (
  UINT8 ChannelInterleaveBitmap
  );

/**

  Description: Encode interleave list for SAD rule

  @param Host - Pointer to sysHost
  @param Sckt  - Socket Id
  @param SadIndex - Index of SAD entry to encode
  @param MemType - memory type

  @retval Encoded InterleaveList

**/
UINT32
SADEncodeInterleaveList (
  SYSHOST *Host,
  UINT8   Sckt,
  UINT8   SadIndex,
  UINT8   MemType
  );

/**

Routine Description

  Description:  Encode target interleave granularity based on the target granularity

  @param host - Pointer to sysHost
  @param tgtGranularity - Target granularity

  @retval Encoded granularity
**/
UINT8
SADEncodeTargetInterleaveGranularity (
  PSYSHOST host,
  UINT8 tgtGranularity
  );

/**

  Find the MC index to use for calculating channel ways.
  imcInterBitmap must be a non-zero value in input. Routine assumes value has BIT0 and/or BIT1 set.

  @param imcInterBitmap : bitmap of IMCs in the interleave.
  @retval IMC number to use for calculating channel ways 

**/
UINT8
GetMcIndexFromBitmap (
  UINT8 imcInterBitmap
  );

/**

  Find the interleave ways from the bitmap

  @param bitmap : bitmap of the interleave
  @retval number of ways interleaved

**/
UINT8
CalculateWaysfromBitmap (
  UINT8 bitmap
  );

/**

  Fill in SAD/TAD/RIR CRSs for memory map from config info in host structure

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
WriteMemoryMapCSRs (
  PSYSHOST host
  );

/**

  Description: Fill in route table CRSs for memory map from config info in host
  structure.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
WriteRouteTableCSRs (
  PSYSHOST      host,
  UINT8         sckt
  );

/**

  Description: Fill in SAD CRSs for memory map from config info in host
  structure.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
WriteSADCSRs (
  PSYSHOST      host,
  UINT8         sckt
  );

/**

  Write the TAD CSRs

  @param host  - Pointer to sysHost
  @param sckt  - Socket number

  @retval N/A

**/
void
WriteTADCSRs (
  PSYSHOST  host,
  UINT8     sckt
  );

/**

  Find minimum NM size per channel  for this MC.

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC index

  @retval Min NM size per channel for this MC

**/
UINT16
FindMinNMSize (
  PSYSHOST  host,
  UINT8     sckt,
  UINT8     mc
  );

/**

  Encode TAD channel "ways" for TAD rule CSR in imc (memory controller)

  @param ways  - ways to encode

  @retval Encoded ways

**/
UINT8
imcTADEncodeChWays (
  UINT8 ways
  );

/**

  Encode TAD socket "ways" for TAD rule CSR.

  @param ways  - ways to encode

  @retval Encoded ways

**/
UINT8
TADEncodeSkWays (
  UINT8 ways
  );

/**

  Write the TAD Interleave Offset register

  @param host                    - Pointer to sysHost
  @param mc                      - mc/iMC number (0-based)
  @param ch                      - Channel number (0-based)
  @param rule                    - Rule number
  @param imcTADChnlIlvOffset0Reg - TAD Channel Interleave Offset Register
  @param TADOffsetList           - TAD Offset list
  @param TADChnIndexList         - TAD channel Index list


  @retval N/A

**/
void
WriteTADOffsetForChannel (
  PSYSHOST host,
  UINT8 sckt,
  UINT8 mc,
  UINT8 ch,
  UINT8 rule,
  struct TADTable *TADEntry,
  UINT32 *TADOffsetList,
  UINT8 *TADChnIndexList
  );

/**

  Description: Fill in SAD2TAD CRSs for memory map from config info in host
  structure.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
WriteMesh2MemCSRs (
  PSYSHOST      host,
  UINT8         sckt
  );

/**

  Check for the memmodes in this MC and update the variable memMode

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC Index

  @retval Bitmap of memory modes

**/
UINT8
CheckMemModes (
  PSYSHOST  host,
  UINT8     sckt,
  UINT8     mc
  );

/**

  Write the RIR CSRs

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
void
WriteRIRCSRs (
  PSYSHOST host,
  UINT8 sckt
  );

/**

  Write the RIR CSRs

  @param host                    - Pointer to sysHost
  @param sckt                  - Socket number
  @param ch                      - Channel number (0-based)

  @retval N/A

**/
void
WriteRIRForChannel (
  PSYSHOST host,
  UINT8 sckt,
  UINT8 ch
  );

/**

  Encode Limit field for RIR DRAM rules

  @param Limit - Limit to encode

  @retval Encoded Limit

**/
UINT32
RIREncodeLimit (
  UINT32 Limit
  );

/**

  Encode RIR "ways" for RIR rule CSR.

  @param ways  - ways to encode

  @retval Encoded ways

**/
UINT8
RIREncodeRIRWays (
  UINT8 ways
  );

/**

  Description: Fill in CHA CRSs for memory map from config info in host
  structure.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
WriteCHACSRs (
  PSYSHOST      host,
  UINT8         sckt
  );

/**

  Program CSRs that are suggested as workarounds.

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
WriteWorkAroundCSRs (
  PSYSHOST host,
  UINT8 sckt
  );

/**

  Fill in TOLM/TOHM CRSs for memory map from config info in host
  structure. For all sockets.

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
WriteTolmTohmCSRs (
  PSYSHOST host
  );


#ifdef NVMEM_FEATURE_EN
/**

Interleaves normal DDR4 memory (Non NVMem) on the channel level across sockets. This

routine figures out if there is any normal DDR4 memory present in the system and then calls SAD interleave function.
This function works for cases where normal dimms are mixed with Type 01 NVDIMMs and also when AEP dimms are present.
This function does not intereleave memory if the only memory presetn in the system is NVMem.

  @param host      - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  @param TOLM      - Top of low memory
  @param memType   - 1LM or 2LM.

  @retval status

**/
UINT32 SocketInterleaveNormalMem (
  PSYSHOST host, 
  MEMORY_MAP_DATA *MemMapData,
  UINT16 TOLM,
  UINT8 memType
  );

/**

Interleaves NVMem (Type01) on the channel level across sockets. This
routine figures out if there is any NVMem and then calls SAD interleave function.
If there no Type01 NVMEM in the system this function does nothing.

  @param host      - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  @param TOLM      - Top of low memory
  @param memType   - 1LM or 2LM.

  @retval status

**/
UINT32 SocketInterleaveNVMem (
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16 TOLM,
  UINT8 memType
  );

/**

Description: Interleaves normal DDR4 memory if present on the channel level but not across sockets.
This routine figures out the mapping and fills in the structures for the SAD, TAD,
tables.  It does not program any registers.

  @param host      - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  @param TOLM      - Top of low memory
  @param memType   - 1LM or 2LM

  @retval status

**/
UINT32 SocketNonInterleaveNormalMem (
  PSYSHOST host, 
  MEMORY_MAP_DATA *MemMapData,
  UINT8 socketNum,
  UINT16 TOLM, 
  UINT8 memType
  );

/**

Description: Interleaves Type01 NVMEM if present on the channel level but not across sockets.
This routine figures out the mapping and fills in the structures for the SAD, TAD,
tables.  It does not program any registers.

  @param host      - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  @param TOLM      - Top of low memory

  @retval status

**/
UINT32 SocketNonInterleaveNVMem (
  PSYSHOST host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8 socketNum,
  UINT16 TOLM,
  UINT8 memType
  );
#endif

// NGNDimmHandler.c

/**

This routine computes the checksum of a table of given size.

@param TblStart     -  Start of the table for which checksum needs to be computed
@param BytesCount   -  Total size of the table for which checksum needs to be computed

@retval Checksum computed
@retval VOID

**/
UINT8
ChsumTbl (
  UINT8 *TblStart,
  UINT32 BytesCount
  );

/**

  This routine translates the error codes related to the Partion requests to proper
  error responses defined in the spec.
  @param status - Status of the Partion Request

  @retval Value that needs to be programmed in the response field for this request
**/
UINT16
EncodePartitionResponse (
  UINT8 status
  );

/**

  This routine translates the error codes related to the Interleave  requests to proper
  error responses defined in the spec.
  @param status - Status of the Interleave Request

  @retval Value that needs to be programmed in the response field for this request
**/
UINT8
EncodeInterleaveResponse (
  UINT8 status
  );

/**

  This routine encodes the interelave granularity
  @param host             - Pointer to sysHost
  @param memType          - Memory type
  @param interleaveFormat - Interleave format

  @retval

**/
UINT8
EncodeInterleaveGranularity (
  PSYSHOST host,
  UINT8 memType,
  UINT32 interleaveFormat
  );

/**

  This routine clears the interleave information structure in CCUR table
  @param host     - Pointer to sysHost
  @param sckt     - socket number
  @param ch       - channel number
  @param dimm     - dimm number
  @param interleaveIndex     - Index to the Interleave requests

  @retval
**/
void
ClearCfgCurInterleaveStruct (
  PSYSHOST host,
  UINT8 sckt,
  UINT8 ch,
  UINT8 dimm,
  UINT8 interleaveIndex
  );

/**

  This routine clears the CCUR table and updates response parameters for new dimms.
  @param host     - Pointer to sysHost
  @param skt      - socket Number
  @param ch       - channel Number
  @param dimm     - Dimm Number


  @retval SUCCESS
**/
UINT32
UpdateCfgCurForNewDimm (
  PSYSHOST host,
  UINT8 sckt,
  UINT8 ch,
  UINT8 dimm
  );

/**

  This routine is to update CCUR tables after memory mapping to reflect the updated dimm configuration wherever applicable.
  @param host     - Pointer to sysHost

  @retval
**/
void
UpdateCfgCurAfterMemmap (
  PSYSHOST host
  );

/**

  This routine copies interleave request details from CfgIn to CfgOut for a dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param interleaveCounter     -  Count of interleave PCAT tables read.

  @retval SUCCESS
**/
UINT32
CopyInterleaveInformation (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8  interleaveCounter
  );

/**

  This routine is to update CfgOut tables after memory mapping to update the validation status.
  It copies the contents of CFGIN requests (Partition and Interleave) into CfgOut and updates the
  response status in the cfgOut structure.
  @param host     - Pointer to sysHost

  @retval
**/
void
UpdateCfgOutAfterMemmap (
  PSYSHOST host
  );

/**

  This routine is to parses through the buffer containing Current Configuration Body and updates the host structures accordingly
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param platformCurCfgBdy     -  Pointer to the Platform Current configuration table body buffer

  @retval SUCCESS
**/
UINT32
CopyCurrentCfgBody (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  NGN_DIMM_PLATFORM_CFG_CURRENT_BODY  *platformCurCfgBdy
  );

/**

  This routine is to update failure condition to all the request we have received from the NVMDIMM management software.
  This routine is called during memory mapping due to silicon resource deficit.
  MEM_MAP_LVL_NORMAL - NORMAL memory mapping
  MEM_MAP_LVL_IGNORE_CFGIN   - Will not honor requests from NVMDIMM management software.
  MEM_MAP_LVL_IGNORE_NEW_DIMMS   - MEM_MAP_LVL_IGNORE_CFGIN + Will not mapping volatile region of new dimms added to the system

  @param host     - Pointer to sysHost

  @retval SUCCESS
**/
UINT32
UpdateRequestforDegradedMemoryMapping (
  PSYSHOST host
  );

/**

  This routine is to parses through the buffer containing Partition Size PCAT table and updates the host structures accordingly
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param PCATPtr     -  Pointer to the PCAT table buffer
  @param RecordType     -  0-CFG_IN record  1- CFG_OUT record.

  @retval SUCCESS
**/
UINT32
UpdatePartitionRequest (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8  *PCATPtr,
  UINT8 RecordType
  );

/**

  This routine scans through the existing dimm information in the host
  NVRAm struct to find a match for the manufacturer id and serial id mentioned
  in the dimm interleave id table and fill the dimm lcoation in the host structs.

  @param host     - Pointer to sysHost
  @param destInterleaveIdPtr   - Pointer to the destination structure.
  @param DimmManufacturerId       - Manufacturer id mentiond in the dimm interleave id table.
  @param DimmSerialNumber     -  Serial number mentioned in the dimm interleave id table.

  @retval SUCCESS
**/
UINT32
FillDimmLocation (
  PSYSHOST host,
  NGN_DIMM_INTERLEAVE_ID_HOST *destInterleaveIdPtr,
  UINT8 *DimmManufacturerId,
  UINT8 *DimmSerialNumber,
  UINT8 *DimmModelNumber
  );

/**

  This routine is to parses through the buffer containing Interleave information PCAT table and updates the host structures accordingly
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param PCATPtr     -  Pointer to the PCAT table buffer
  @param RecordType     -  0- CFG_CUR record  1-CFG_IN record  2- CFG_OUT record.
  @param interleaveCounter     -  Count of interleave PCAT tables read.

  @retval SUCCESS
**/
UINT32
UpdateInterleaveInformation (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8  *PCATPtr,
  UINT8 RecordType,
  UINT8 interleaveCounter
  );

/**

  This routine to parses through the buffer looking for PCAT tables and updates the host structures accordingly
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param PCATPtr     -  Pointer to the PCAT table buffer
  @param partition     -  Partition from which the PCAT table is read from.
  @param RecordType     -  0- CFG_CUR record  1-CFG_IN record  2- CFG_OUT record.

  @retval SUCCESS
**/
UINT32
HandlePCATs (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8  *PCATPtr,
  UINT8 RecordType
  );

/**

  This routine to issue getplatformconfigdata command and read the current configuration data from the the dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number


  @retval TRUE - if a new dimm is detected
  @retval FALSE - if it is the same dimm form the previous boot
**/
UINT32
ReadCurrentCfgTable (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT32 CurCfgOffset,
  UINT32 CurCfgSize
  );

/**

  This routine to issue getplatformconfigdata command and reads the configuration header data from BIOS partition
  to get the offset of the current configuration data
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number


  @retval TRUE - if a new dimm is detected
  @retval FALSE - if it is the same dimm form the previous boot
**/
UINT32
ReadBIOSPartition (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
  );

/**

  This routine to issue getplatformconfigdata command and read the  configuration Input requests from the the dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param ConfRequestDataOffset     -  Offset of CfgIn table in Platform Config Data Region 2.
  @param ConfRequestDataSize     -  Offset of CfgIn table

  @retval SUCCESS
**/
UINT32
ReadCfgInTable (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT32 ConfRequestDataOffset,
  UINT32 ConfRequestDataSize
  );

/**

  This routine to issue getplatformconfigdata command and read the  configuration Input requests from the the dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param ConfResponseDataOffset     -  Offset of CfgOut table in Platform Config Data Region 2.
  @param ConfResponseDataSize     -  Offset of CfgOut table

  @retval SUCCESS
**/
UINT32
ReadCfgOutTable (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT32 ConfResponseDataOffset,
  UINT32 ConfResponseDataSize
  );

/**

  This routine to issue getplatformconfigdata command and read the  configuration Input requests from the the dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval SUCCESS
**/
UINT32
ReadOSPartition (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
  );

#ifndef MINIBIOS_BUILD
/**

  This routine compare the manufacturer id and serial number of the current
  dimm in the slot with the dimm information stored in the NVRAM from previous boot.

  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param *prevBootNGNDimmCfg     -  Pointer to the struct that stores the NGN dimm cfg from the previous boot


  @retval TRUE - if a new dimm is detected
  @retval FALSE - if it is the same dimm form the previous boot


**/
BOOLEAN
IsNewDimm (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  struct prevBootNGNDimmCfg  *prevBootNGNDimmCfg
  );
#endif

/**

  This routine is to compare the SKU information of each dimm to see if we have mismatched dimm SKUs  in the system.
  We currently do not support dimms of different SKU.
  @param host     - Pointer to sysHost


  @retval SUCCESS - if the SKU information of all the NVMDIMMs in the system match.
  @retval FAILURE - if the SKU information of all the NVMDIMMs in the system donot match
**/
UINT32
CheckForIncompatibleDimms (
  PSYSHOST host
  );

/**

  This routine is to issue identifydim and getplatformconfigdata command and read the current configuration data from all the NGN dimms in the system.
  @param host     - Pointer to sysHost


  @retval SUCCESS - if NGN DIMM PCD is read and the host struct is populated.
  @retval FAILURE - if dimm SKUs mismatch
**/
UINT32
GetDimmDeviceData (
  PSYSHOST host
  );

/**

Routine Description: SetDieSparingPolicy calls SetDieSparing per dimm.

  @param host           - Pointer to the system host (root) structure

  @retval 0 SUCCESS
  @retval 1 FAILURE

**/
UINT32
SetAepDieSparing (
  PSYSHOST       host,
  UINT8          socket
  );

/**

  This routine initializes the memory size fields in the structures
  for all the NGN dimms in the system

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
InitializeNGNDIMM (
  PSYSHOST host
  );

/**

  This routine validates  the Partition request records read from PCD1
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval PARTITION_TOO_BIG - if the partition size is greater than dimm size.
  @retval VALIDATION_SUCCESS - If validation succeeds
**/
UINT8
ValidatePartitionRequest (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
  );

/**

  This routine validates  a specific Interleave request read from PCD1
  @param host     - Pointer to sysHost
  @param sourceInterleaveId   - Pointer to the InterleaveId array of the source
  @param destInterleaveId   - Pointer to the InterleaveId array of the destination
  @param NumOfDimmsInInterleaveSet     -  Number of Dimms in the interleave

  @retval status
**/
UINT32
CompareDimmInterleaveId (
  PSYSHOST host,
  NGN_DIMM_INTERLEAVE_ID_HOST *sourceInterleaveId,
  NGN_DIMM_INTERLEAVE_ID_HOST *destInterleaveId,
  UINT8 NumOfDimmsInInterleaveSet
  );

/**

  This routine validates  a specific Interleave request read from PCD1
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param interleaveReqId     -  Interleave request id
  @param recordType     -  Type of the record validated CFG_CUR/CFG_IN

  @retval status
**/
UINT8
ValidateInterleaveRequest (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 interleaveReqId,
  UINT8 recordType
  );

/**

  This routine validates  the specific record read from PCD1 and stores the validation result in host struct
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval SUCCESS - Validation of the record successful
  @retval FAILURE - Validation of the record failed

**/
UINT32
ValidateCfgRecords (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 recordType
  );

/**

  This routine goes through bothe CfgCur and CfgIn and validates the current config and config requests.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
ValidateNGNDimmData (
  PSYSHOST host
  );

/**

  This routines clears the Interleaverequests related to a specific dimm

  @param host     - Pointer to sysHost
  @param sckt   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval SUCCESS

**/
UINT32
HandlePartitionFailure (
  PSYSHOST host,
  UINT8 sckt,
  UINT8 ch,
  UINT8 dimm,
  UINT32 nvmDimmStatusCode
  );

/**

  This routine goes through CfgIn, gets at the paririon requests and applies them to the dimm.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
HandlePartitionRequests (
  PSYSHOST host
  );

/**

  This gets the socket interleave information from the interleave request.

  @param host  - Pointer to sysHost
  @param interleavePtr  - Pointer to interleave table
  @param interleaveIdPtr  - pointer to the first dimm info struct for this interleave set
  @param scktInterleave  - pointer to the socket interleav structure
  @param sktWays  - pointer to the variable to store total IMC ways

  @retval SUCCESS

**/
UINT32
GetScktInterleaveInfo (
  PSYSHOST host,
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *interleavePtr,
  NGN_DIMM_INTERLEAVE_ID_HOST *interleaveIdPtr,
  UINT16 scktInterleave[MC_MAX_NODE],
  UINT8 *sktWays
  );

/**

  This routine creates a DRAM rule for a specific interleave request and updates the SADTable struct in corresponding sockets.

  @param host  - Pointer to sysHost
  @param sckt  - socket number
  @param ch  - chennal number
  @param dimm  - dimm number
  @param interleavePtr  - pointer to the first interleave set
  @param interleaveIdPtr  - pointer to the first dimm info struct for this interleave set

  @retval SUCCESS

**/
UINT32
GetChInterleaveInfo (
  PSYSHOST host,
  UINT8 sckt,
  UINT8 ch,
  UINT8 dimm,
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *interleavePtr,
  NGN_DIMM_INTERLEAVE_ID_HOST *interleaveIdPtr,
  UINT8 *ChInterleaveBitMap
  );

/**

  This routine helps to find the imc bitmap of MCs interleaveds for a SAD rule in a particular socket.

  @param host  - Pointer to sysHost
  @param scktInterleave  - structure with list of MCs that are part of this interleave
  @param socket  - Socket number

  @retval imcBitmap =- bit map of imcs interleaved in this SAD rule from thie socket.

**/
UINT8
FindImcBitmap (
  PSYSHOST host,
  UINT16 scktInterleave[MC_MAX_NODE],
  UINT8 socket
  );

/**

  This routine creates a DRAM rule for a specific interleave request and updates the SADTable struct in corresponding sockets.

  @param host  - Pointer to sysHost
  @param scktInterleave  - structure with list of MCs that are part of this interleave
  @param sktWays  - Number of IMCs involved in this interleave
  @param FMChInterleaveBitMap  - FM Channel Interleave bitmap
  @param InterSizePerCh  - Interleave size form each channel
  @param memType  - PMEM / PMEM$
  @param mirrorEnable  - Enable/Disable mirroring for this SAD rule
  @param interleaveFormat - Interleave granularities for this SAD rule
  @param nodeLimit  - Pointer to return the limit of SAD created.

  @retval status

**/
UINT32
AddSADRule (
  PSYSHOST host,
  UINT16 scktInterleave[MC_MAX_NODE],
  UINT8 sktWays,
  UINT8 FMChInterleaveBitMap,
  UINT8 NMBitmap,
  UINT16 InterSizePerCh,
  UINT8 memType,
  UINT8 mirrorEnable,
  UINT32 interleaveFormat,
  UINT32 *nodeLimit
  );

/**

  This routine compare the NM population of all the MC involved in the SAD rule and returns error if they dont match.

  @param host  - Pointer to sysHost
  @param scktInterleave  - sckt interleave struct that holds list MC  involved in the request.
  @param NMBitmap  - Pointer to variable to store the bitmap


  @retval SUCCESS - if NM population matches
  @retval Failure - if NM population doesnt match


**/
UINT32
CheckNMPopulation (
  PSYSHOST host,
  UINT16 scktInterleave[MC_MAX_NODE],
  UINT8 *NMBitmap
  );

/**

  This routine creates a DRAM rule for a specific interleave request and updates the SADTable struct in corresponding sockets.

  @param host  - Pointer to sysHost
  @param sckt  - socket number
  @param ch  - Channel number
  @param dimm  - dimm number
  @param interleavePtr  - Pointer to interleave table
  @param interleaveIdPtr  - pointer to the first dimm info struct for this interleave set

  @retval SADLimit

**/
UINT32
ApplyInterleave (
  PSYSHOST host,
  UINT8 sckt,
  UINT8 ch,
  UINT8 dimm,
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *interleavePtr,
  NGN_DIMM_INTERLEAVE_ID_HOST *interleaveIdPtr,
  UINT32 *nodeLimit
  );

/**

  Description: This routine sets the "processedFlag" & SadLimit variable in the interleave request
  struct in dimm structs if the requests are processed and SADs were created.

  @param host  - Pointer to sysHost
  @param sckt  - Socket Id
  @param ch  - channel id
  @param Dimm  - Dimm num
  @param *sourceInterleave  - Pointer to the interleave struct
  @param *sourceInterleaveId  - Pointer to the dimm id struct.
  @param recordType  - specifies if this is CFGIN or CFG_CUR
  @param nodeLimit   - Limit of SAD Rule related to the request

  @retval SUCCESS - if sucessfull
  @retval Failure - if not sucessfull


**/
UINT32
UpdateProcessedFlag (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *sourceInterleave,
  NGN_DIMM_INTERLEAVE_ID_HOST *sourceInterleaveId,
  UINT32 nodeLimit,
  UINT8 recordType,
  UINT32 responseStatus
  );

/**

  This routine goes through each NVMDIMM in the system and if there are no CFGIn requests, it will apply the current config
  based on CCUR data retreived from the dimm.

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
ApplyCCURRecords (
  PSYSHOST host
  );

/**

  This routine goes through each NVMDIMM in the system and if there are  CFGIn requests, it will apply Interleave requests retrieved form the dimm

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
ApplyInterleaveRequests (
  PSYSHOST host
  );

/**

This routine reads the gEfiPrevBootNGNDimmCfgVariable and updates the input variable with the data.
This will contain empty data in the first boot after flash

    @param host = pointer to sysHost structure
    @param prevBootNGNDimmCfg = pointer to the structure to be filled
**/
#ifndef MINIBIOS_BUILD
UINT32 UpdatePrevBootNGNDimmCfg (
  struct sysHost *host,
  struct prevBootNGNDimmCfg  *prevBootNGNDimmCfg
  );
#endif

#endif //_MEM_ADDR_MAP_H_
