//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
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
 *      This file contains memory detection and initialization for
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

//The BDAT_MEMORY_DATA_STRUCTURE implemented here is as per BDAT 4.0, Memory Schema3

#ifndef _bdat_h
#define _bdat_h

#ifndef ASM_INC

#define MAX_SPD_BYTE_512         512 // Number of bytes in Serial EEPROM

#ifdef BDAT_SUPPORT
#pragma pack(1)


// Memory Schema 4B GUID  - {5B274DC7-4222-4033-BAC8-5F13A111A215}
#define BDAT_MEMORY_DATA_4B_GUID {0x5b274dc7, 0x4222, 0x4033, { 0xba, 0xc8, 0x5f, 0x13, 0xa1, 0x11, 0xa2, 0x15}}
// RMT Schema 5 GUID     - {1838678E-ED14-4e70-A90D-48572BF053D2}
#define RMT_SCHEMA_5_GUID       {0x1838678E, 0xED14, 0x4E70, { 0xA9, 0xD, 0x48, 0x57, 0x2B, 0xF0, 0x53, 0xD2}}

// MEMORY SCHEMA 4
typedef struct {
  UINT16                        recEnDelay[MAX_STROBE];  // Units = 1/64 QCLK Array of nible training results per rank
  UINT16                        wlDelay[MAX_STROBE];
  UINT8                         rxDqDelay[MAX_STROBE];
  UINT8                         txDqDelay[MAX_STROBE];
  UINT16                        clkDelay;
  UINT16                        ctlDelay;
  UINT16                        cmdDelay[3];
  UINT8                         ioLatency;
  UINT8                         roundtrip;
  UINT8                         txvref[MAX_STROBE]; //TxVref training values per rank and strobe
} BDAT_RANK_TRAINING_STRUCTURE;

typedef struct {
  UINT8                         rankEnabled;         // 0 = Rank disabled
  BDAT_RANK_TRAINING_STRUCTURE  rankTraining;        // Rank training settings
} BDAT_RANK_STRUCTURE;

typedef struct {
  UINT8                         valid[MAX_SPD_BYTE_512/8];  // Each valid bit maps to SPD byte
  UINT8                         spdData[MAX_SPD_BYTE_512];  // Array of raw SPD data bytes
} BDAT_SPD_STRUCTURE;

typedef struct {
  UINT8                         dimmEnabled;             // 0 = DIMM disabled
  BDAT_RANK_STRUCTURE           rankList[MAX_RANK_DIMM]; // Array of ranks per DIMM
  BDAT_SPD_STRUCTURE            spdBytes;                // SPD data per DIMM
} BDAT_DIMM_STRUCTURE;

typedef struct {
  UINT8                         chEnabled;               // 0 = Channel disabled
  UINT8                         numDimmSlot;             // Number of slots per channel on the board
  BDAT_DIMM_STRUCTURE           dimmList[MAX_DIMM];      // Array of DIMMs per channel
} BDAT_CHANNEL_STRUCTURE;


typedef struct {
  UINT8                         imcEnabled;           // 0 = MC disabled
  UINT16                        imcDid;               // MC device Id.
  UINT8                         imcRid;               // MC revision Id
  UINT16                        ddrFreq;              // DDR freq. in units of MHz/10
  UINT16                        ddrVoltage;           // Vdd in units of mV
  UINT8                         piStep;               // Step unit   = piStep * tCK/2048
                                                        // e.g. piStep = 16 for step = 11.7ps (1/128 tCK)
  UINT16                        rxVrefStep;           // Step unit = rxVrefStep * Vdd / 100
                                                        // e.g. rxVrefStep = 520 for step = 7.02 mV
  UINT16                        txVrefStep;           // Step unit = txVrefStep * Vdd / 100
  UINT16                        caVrefStep;           // Step unit = caVrefStep * Vdd / 100
  UINT8                         recvenStep;           // Step unit = recvenStep * tCK / 2048
  UINT8                         wrLevStep;            // Step unit = wrLevStep * tCK / 2048
  BDAT_CHANNEL_STRUCTURE        channelList[MAX_CH];
} BDAT_SOCKET_STRUCTURE;

typedef struct bdatSchemaHeader {
  EFI_GUID                      SchemaId;             //Schema 4 GUID
  UINT32                        DataSize;
  UINT16                        Crc16;
} BDAT_SCHEMA_HEADER_STRUCTURE;

typedef struct {
  BDAT_SCHEMA_HEADER_STRUCTURE  schemaHeader;
  UINT32                        refCodeRevision;         // Matches JKT scratchpad definition
  UINT8                         maxNode;                 // Max processors per system, e.g. 4
  UINT8                         maxCh;                   // Max channels per socket, e.g. 4
  UINT8                         maxDimm;                 // Max DIMM per channel, e.g. 3
  UINT8                         maxRankDimm;             // Max ranks per DIMM, e.g. 4
  UINT8                         maxStrobe;               // Number of Dqs used by the rank, e.g. 18
  BDAT_SOCKET_STRUCTURE         socketList[MAX_SOCKET];  // Array of sockets per system
} BDAT_MEMORY_DATA_STRUCTURE;


//RMT SCHEMA 5
typedef struct {
  UINT8                         rxDqLeft;       // Units = piStep
  UINT8                         rxDqRight;
  UINT8                         txDqLeft;
  UINT8                         txDqRight;
  UINT8                         rxVrefLow;      // Units = rxVrefStep
  UINT8                         rxVrefHigh;
  UINT8                         txVrefLow;      // Units = txVrefStep
  UINT8                         txVrefHigh;
} BDAT_DQ_MARGIN_STRUCTURE;

typedef struct {
  UINT8                         rxDqLeft;       // Units = piStep = 1/64 QCLK piStep
  UINT8                         rxDqRight;
  UINT8                         txDqLeft;
  UINT8                         txDqRight;
  UINT8                         cmdLeft;
  UINT8                         cmdRight;
  UINT8                         ctlLeft;
  UINT8                         ctlRight;
  UINT8                         rxVrefLow;      // Units = rxVrefStep
  UINT8                         rxVrefHigh;
  UINT8                         txVrefLow;      // Units = txVrefStep
  UINT8                         txVrefHigh;
  UINT8                         cmdVrefLow;     // Units = caVrefStep
  UINT8                         cmdVrefHigh;
} BDAT_RANK_MARGIN_STRUCTURE;

typedef struct {
  UINT8                         rankEnabled;         // 0 = Rank disabled
  UINT8                         rankMarginEnabled;   // 0 = Rank margin disabled
  UINT8                         dqMarginEnabled;     // 0 = Rank margin disabled
  BDAT_RANK_MARGIN_STRUCTURE    rankMargin;          // Rank margin data
  BDAT_DQ_MARGIN_STRUCTURE      dqMargin[MAX_BITS];  // Array of Dq margin data per rank
} BDAT_RMT_RANK_STRUCTURE;

typedef struct {
  UINT8                         dimmEnabled;                 // 0 = DIMM disabled
  BDAT_RMT_RANK_STRUCTURE       rankList[MAX_RANK_DIMM];     // Array of ranks per DIMM
} BDAT_RMT_DIMM_STRUCTURE;

typedef struct {
  UINT8                         chEnabled;                  // 0 = Channel disabled
  UINT8                         numDimmSlot;                // Number of slots per channel on the board
  BDAT_RMT_DIMM_STRUCTURE       dimmList[MAX_DIMM];         // Array of DIMMs per channel
} BDAT_RMT_CHANNEL_STRUCTURE;

typedef struct {
  UINT8                         imcEnabled;                 // 0 = MC disabled
  BDAT_RMT_CHANNEL_STRUCTURE    channelList[MAX_CH];        // Array of channels per socket
} BDAT_RMT_SOCKET_STRUCTURE;

typedef struct {
  BDAT_SCHEMA_HEADER_STRUCTURE  schemaHeader;
  UINT32                        refCodeRevision;           // Matches JKT scratchpad definition
  UINT8                         maxNode;                   // Max processors per system, e.g. 4
  UINT8                         maxCh;                     // Max channels per socket, e.g. 4
  UINT8                         maxDimm;                   // Max DIMM per channel, e.g. 3
  UINT8                         maxRankDimm;               // Max ranks per DIMM, e.g. 4
  UINT8                         maxDq;                     // Number of Dq bits used by the rank, e.g. 72
  UINT32                        marginLoopCount;           // Units of cache line
  BDAT_RMT_SOCKET_STRUCTURE     socketList[MAX_SOCKET];    // Array of sockets per system
} BDAT_RMT_STRUCTURE;

#pragma pack()
#endif //BDAT_SUPPORT


#endif // ASM_INC
#endif // _bdat_h
