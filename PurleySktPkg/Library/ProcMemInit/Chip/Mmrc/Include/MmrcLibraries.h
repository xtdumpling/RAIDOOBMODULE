/**

Copyright (c) 2005-2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MMRCLibraries.h

  Library specific macros and function declarations used within the MMRC.

**/

#ifndef _MMRCLIBRARIES_H_
#define _MMRCLIBRARIES_H_

#include "Mmrc.h"
#include "MmrcProjectData.h"
#if JTAG
#include <stdio.h>
#endif

#ifdef __GNUC__
#define __cdecl __attribute__((__cdecl__))
#endif

#define MIN_CMD_DELAY            0
#define MAX_CMD_DELAY            0x36F
#define CMD_HEADER_ENTRY         0
#define CMD_HEADER_INIT          1
#define CMD_HEADER_COMMON        2
#define CMD_HEADER_INDEPENDENT   3
#define CMD_HEADER_TETHER        4
#define CMD_HEADER_FINAL         5

#define RESULTS_PASS 0
#define RESULTS_FAIL 1
#define RESULTS_INIT 2
#define CURRENT_SAMPLE 0
#define PREVIOUS_SAMPLE 1

enum {
  CPGC_CONFIG_MEMINIT    
};
enum {
  CPGC_CMDSEQ_WRITE, 
  CPGC_CMDSEQ_READ, 
  CPGC_CMDSEQ_WRITE_READ
};

//
// Macro definitions for Early Command Training.
//
#define NUMBER_EARLY_COMMAND_PATTERNS 2
#define NUMBER_EARLY_COMMAND_RETRIES  1
#define MRW41                         41 << 4
#define MRW42                         42 << 4
#define MRW48                         48 << 4

#if FAULTY_PART_TRACKING
//
// Faulty part tracking support
//
#define LOOKUP_TABLE_1             0x1001001001001L
#define LOOKUP_TABLE_2             0x84210842108421L
#define MASK_LOWER_12BIT           0xfff
#define MODULE31                   0x1f
#define FAULTY_PART_NO_ERROR       0
#define FAULTY_PART_CORRECTABLE    1
#define FAULTY_PART_UNCORRECTABLE  2
#define BYTE_MASK                  0xFF
#define NIBBLE_MASK                0xF
#define MAX_STROBE_X8_DEVICES      9
#define MAX_STROBE_X4_DEVICES      18
#define STROBE_BITS_X8_DEVICES     8
#define STROBE_BITS_X4_DEVICES     4
#endif

//
// Extern variables required for early command training.
//
#if defined SIM || defined JTAG
extern FILE *gLogFile;
#endif
extern UINT32 GetAddress (MMRC_DATA *, UINT8, UINT8);
extern VOID MySimStall(UINT32 count);

extern VOID OemMemoryErrorCode (UINT8);

typedef struct {
  UINT16  Sample[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];  
} SAMPLE_SWEEP;


MMRC_STATUS
DynamicAssignmentGenerated (
  IN  OUT   UINTX         DynamicVars[MAX_CHANNELS][DYNVAR_MAX],
  IN        UINT8         Channel,
  IN        UINT8         Index,
  IN        UINTX        *Value
)
;

/*
  This routine dumps all registers in the MMRC spreadsheet. On the cover sheet of the SS, the user
  can control the debug level output:

  DUMP_REGISTER_NAMES = 1 means all register names, ports, offsets, values, and default values will be printed.
  DUMP_REGISTER_FIELDS = 1 means all register field names, values, default values, and access types will be printed.
  DUMP_REGISTER_DESCRIPTIONS = 1 will display the description for each register field.

  If DUMP_REGISTER_NAMES is set to 0, the other enables are don't-cares as this is the global enable for this routine.
*/
MMRC_STATUS
  DumpAllRegisters (
)
;

/**

  Single entry point to MMRC. MRC calls this function
  and then MMRC acts as a black box, performing the requested
  stage of DDRIO init and returns when complete or when
  an error is encountered.
 
  @param MrcData:       Host structure for all data related to MMRC
  @param Action:           Phy init, training, FIFO reset, post-training, etc
  @param Parameters:       A structure used to pass data between MRC and MMRC.

  @retval Success
  @retval Failure

**/
MMRC_STATUS
MmrcEntry (
  IN  OUT   PSYSHOST    MrcData
)
;

MMRC_STATUS
CreatePFCTSel (
               IN  OUT   PSYSHOST        MrcData,
               IN        UINT8            Channel,
               IN        PFCT_VARIATIONS *PFCTSelect
)
;

UINTX
ByteEnableToUintX (
                    IN     UINT8  Be, 
                    IN     UINTX  Value, 
                    IN     UINT8 *ValueLength
) 
;

VOID
SaveMrcHostStructureAddress (
  IN    PSYSHOST   MrcDataAddress
)
;

PSYSHOST
GetMrcHostStructureAddress (
)
;
extern PSYSHOST MyMrcData[MAX_SOCKET];

#endif
