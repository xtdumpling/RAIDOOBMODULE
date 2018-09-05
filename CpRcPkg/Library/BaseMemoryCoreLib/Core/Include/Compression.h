//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

 Memory Reference Code

 ESS - Enterprise Silicon Software

 INTEL CONFIDENTIAL

@copyright
 Copyright 2006 - 2015 Intel Corporation All Rights Reserved.

 The source code contained or described herein and all documents
 related to the source code ("Material") are owned by Intel
 Corporation or its suppliers or licensors. Title to the Material
 remains with Intel Corporation or its suppliers and licensors.
 The Material contains trade secrets and proprietary and confidential
 information of Intel or its suppliers and licensors. The Material
 is protected by worldwide copyright and trade secret laws and treaty
 provisions.  No part of the Material may be used, copied, reproduced,
 modified, published, uploaded, posted, transmitted, distributed, or
 disclosed in any way without Intel's prior express written permission.

 No license under any patent, copyright, trade secret or other
 intellectual property right is granted to or conferred upon you
 by disclosure or delivery of the Materials, either expressly,
 by implication, inducement, estoppel or otherwise. Any license
 under such intellectual property rights must be express and
 approved by Intel in writing.

@file
  Compression.h

@brief
  This file contains reference code data structures

**/

#ifndef _compression_h
#define _compression_h
//
// Decompression algorithm begins here (move this to separate core header)
//
#define BITBUFSIZ 32
#define MAXMATCH  256
#define THRESHOLD 3
#define CODE_BIT  16
#ifndef UINT8_MAX
#define UINT8_MAX 0xff
#endif
#define BAD_TABLE - 1

//
// C: Char&Len Set; P: Position Set; T: exTra Set
//
#define NC      (0xff + MAXMATCH + 2 - THRESHOLD)
#define CBIT    9
#define MAXPBIT 5
#define TBIT    5
#define MAXNP   ((1U << MAXPBIT) - 1)
#define _NT      (CODE_BIT + 3)
#if _NT > MAXNP
#define NPT _NT
#else
#define NPT MAXNP
#endif
//
// C: the Char&Len Set; P: the Position Set; T: the exTra Set
//
#define CBIT              9
#define NP                (WNDBIT + 1)
// PBIT is Version dependent. Version 1 - PBIT = 4, Version 2 - PBIT = 5
//#define PBIT              4
#define NT                (CODE_BIT + 3)
#define TBIT              5

//
// Macro Definitions
//
#define SHELL_FREE_NON_NULL(x)
#define SetMem(dest, size, value)       MemSetLocal((UINT8 *)dest, value, size)
#define CopyMem(dest, source, size)     MemCopy ((UINT8 *)dest, (UINT8 *)source, size)
#ifndef EFI_STATUS
#define EFI_STATUS        UINT32
#endif
typedef INT16             NODE;
#define UINT8_MAX         0xff
#define UINT8_BIT         8
#define THRESHOLD         3
#define INIT_CRC          0
#define WNDBIT            10
#define WNDSIZ            (1U << WNDBIT)
#define MAXMATCH          256
#define BLKSIZ            (1U << 11)  // 11 * 1024U
#define PERC_FLAG         0x8000U
#define CODE_BIT          16
#define NIL               0
#define MAX_HASH_VAL      (3 * WNDSIZ + (WNDSIZ / 512 + 1) * UINT8_MAX)
#define HASH(LoopVar7, LoopVar5)        ((LoopVar7) + ((LoopVar5) << (WNDBIT - 9)) + WNDSIZ * 2)
#define CRCPOLY           0xA001
#define UPDATE_CRC(LoopVar5)     Globals->mCrc = Globals->mCrcTable[(Globals->mCrc ^ (LoopVar5)) & 0xFF] ^ (Globals->mCrc >> UINT8_BIT)

#define  INSUFFICIENT_SCRATCH_SIZE     1
#define  SRC_SIZE                      2
#define  SRC_LESS_COMP                 3
#define  BUFFER_SIZE_MISMATCH          4
#define  UNKNOWN_VERSION               5
#define  BAD_TABLE_FLAG                6

typedef struct Scratch_Data {
  PSYSHOST host;
  UINT8  *mSrcBase;  ///< Starting address of compressed data
  UINT8  *mDstBase;  ///< Starting address of decompressed data
  UINT32 mOutBuf;
  UINT32 mInBuf;
  UINT8  *mSrcUpperLimit;
  UINT8  *mDstUpperLimit;

  UINT8  mLevel[WNDSIZ + UINT8_MAX + 1];
  UINT8  mText[WNDSIZ * 2 + MAXMATCH];
  UINT8  mChildCount[WNDSIZ + UINT8_MAX + 1];
  UINT8  mBuf[BLKSIZ];
  UINT8  mCLen[NC];
  UINT8  mPTLen[NPT];
  UINT8  *mLen;
  INT16  mHeap[NC + 1];
  INT32  mRemainder;
  INT32  mMatchLen;
  INT32  mBitCount;
  INT32  mHeapSize;
  INT32  mTempInt32;
  UINT32 mBufSiz;
  UINT32 mOutputPos;
  UINT32 mOutputMask;
  UINT32 mBitBuf;
  UINT32 mSubBitBuf;
  UINT32 mCrc;
  UINT32 mCompSize;
  UINT32 mOrigSize;
  UINT32 CPos;

  UINT16 mBlockSize;
  UINT16 mBadTableFlag;
  UINT16 *mFreq;
  UINT16 *mSortPtr;
  UINT16 mLenCnt[17];
  UINT16 mLeft[2 * NC - 1];
  UINT16 mRight[2 * NC - 1];
  UINT16 mCrcTable[UINT8_MAX + 1];
  UINT16 mCFreq[2 * NC - 1];
  UINT16 mCCode[NC];
  UINT16 mPFreq[2 * NP - 1];
  UINT16 mPTCode[NPT];
  UINT16 mTFreq[2 * NT - 1];

  NODE   mPos;
  NODE   mMatchPos;
  NODE   mAvail;
  NODE   mPosition[WNDSIZ + UINT8_MAX + 1];
  NODE   mParent[WNDSIZ * 2];
  NODE   mPrev[WNDSIZ * 2];
  NODE   mNext[MAX_HASH_VAL + 1];
  INT32  mHuffmanDepth;
  UINT16 mCTable[4096];
  UINT16 mPTTable[256];
  UINT8  mPBit;          ///< The length of the field 'Position Set Code Length Array Size' in Block Header.\n
                         ///< For EFI 1.1 de/compression algorithm, mPBit = 4 \n
                         ///< For Tiano de/compression algorithm, mPBit = 5

} SCRATCH_DATA, *PSCRATCH_DATA;

#endif // _compression_h
