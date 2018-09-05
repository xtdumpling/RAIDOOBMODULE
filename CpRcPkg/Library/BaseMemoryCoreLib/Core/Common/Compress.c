/** @file
  Main file for compression routine.

  Compression routine. The compression algorithm is a mixture of
  LZ77 and Huffman coding. LZ77 transforms the source data into a
  sequence of Original Characters and Pointers to repeated strings.
  This sequence is further divided into Blocks and Huffman codings
  are applied to each Block.

  Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


//
// 7-15-2015
// Original source from \GrangevilleTrunk\ShellPkg\Library\UefiShellDebug1CommandsLib\Compress.c
// Modified to put Globals on local stack and support EFI compress version 1 and version 2.
// Added Compress/Decompress test routine for debugging / validation.
//
//#if (!defined(SIM_BUILD) && !defined(HEADLESS_MRC)) || defined(RC_SIM)

#include "SysHost.h"
#include "SysFunc.h"
#include "Compression.h"

/**
  Make a CRC table.

**/
static
VOID
EFIAPI
MakeCrcTable (
  PSCRATCH_DATA Globals
  )
{
  UINT32  LoopVar1;

  UINT32  LoopVar2;

  UINT32  LoopVar4;

  for (LoopVar1 = 0; LoopVar1 <= UINT8_MAX; LoopVar1++) {
    LoopVar4 = LoopVar1;
    for (LoopVar2 = 0; LoopVar2 < UINT8_BIT; LoopVar2++) {
      if ((LoopVar4 & 1) != 0) {
        LoopVar4 = (LoopVar4 >> 1) ^ CRCPOLY;
      } else {
        LoopVar4 >>= 1;
      }
    }

    Globals->mCrcTable[LoopVar1] = (UINT16) LoopVar4;
  }
}

/**
  Put a dword to output stream

  @param[in] Data    The dword to put.
**/
static
VOID
EFIAPI
PutDword (
  IN PSCRATCH_DATA Globals,
  IN UINT32        Data
  )
{
  if (Globals->mDstBase < Globals->mDstUpperLimit) {
    *Globals->mDstBase++ = (UINT8) (((UINT8) (Data)) & 0xff);
  }

  if (Globals->mDstBase < Globals->mDstUpperLimit) {
    *Globals->mDstBase++ = (UINT8) (((UINT8) (Data >> 0x08)) & 0xff);
  }

  if (Globals->mDstBase < Globals->mDstUpperLimit) {
    *Globals->mDstBase++ = (UINT8) (((UINT8) (Data >> 0x10)) & 0xff);
  }

  if (Globals->mDstBase < Globals->mDstUpperLimit) {
    *Globals->mDstBase++ = (UINT8) (((UINT8) (Data >> 0x18)) & 0xff);
  }
}

/**
  Allocate memory spaces for data structures used in compression process.

  @retval EFI_SUCCESS           Memory was allocated successfully.
**/
static
EFI_STATUS
EFIAPI
AllocateMemory (
  PSCRATCH_DATA Globals
  )
{

  Globals->mBufSiz     = BLKSIZ;
  Globals->mBuf[0] = 0;

  return EFI_SUCCESS;
}

/**
  Called when compression is completed to free memory previously allocated.

**/
static
VOID
EFIAPI
FreeMemory (
  PSCRATCH_DATA Globals
  )
{
  SHELL_FREE_NON_NULL (Globals->mText);
  SHELL_FREE_NON_NULL (Globals->mLevel);
  SHELL_FREE_NON_NULL (Globals->mChildCount);
  SHELL_FREE_NON_NULL (Globals->mPosition);
  SHELL_FREE_NON_NULL (Globals->mParent);
  SHELL_FREE_NON_NULL (Globals->mPrev);
  SHELL_FREE_NON_NULL (Globals->mNext);
  SHELL_FREE_NON_NULL (Globals->mBuf);
}

/**
  Initialize String Info Log data structures.
**/
static
VOID
EFIAPI
InitSlide (
  PSCRATCH_DATA Globals
  )
{
  NODE  LoopVar1;

  SetMem (Globals->mLevel + WNDSIZ, (UINT8_MAX + 1) * sizeof (UINT8), 1);
  SetMem (Globals->mPosition + WNDSIZ, (UINT8_MAX + 1) * sizeof (NODE), 0);

  SetMem (Globals->mParent + WNDSIZ, WNDSIZ * sizeof (NODE), 0);

  Globals->mAvail = 1;
  for (LoopVar1 = 1; LoopVar1 < WNDSIZ - 1; LoopVar1++) {
    Globals->mNext[LoopVar1] = (NODE) (LoopVar1 + 1);
  }

  Globals->mNext[WNDSIZ - 1] = NIL;
  SetMem (Globals->mNext + WNDSIZ * 2, (MAX_HASH_VAL - WNDSIZ * 2 + 1) * sizeof (NODE), 0);
}

/**
  Find child node given the parent node and the edge character

  @param[in] LoopVar6       The parent node.
  @param[in] LoopVar5       The edge character.

  @return             The child node.
  @retval NIL(Zero)   No child could be found.

**/
static
NODE
EFIAPI
Child (
  IN PSCRATCH_DATA Globals,
  IN NODE          LoopVar6,
  IN UINT8         LoopVar5
  )
{
  NODE  LoopVar4;

  LoopVar4             = Globals->mNext[HASH (LoopVar6, LoopVar5)];
  Globals->mParent[NIL]  = LoopVar6;  /* sentinel */
  while (Globals->mParent[LoopVar4] != LoopVar6) {
    LoopVar4 = Globals->mNext[LoopVar4];
  }

  return LoopVar4;
}

/**
  Create a new child for a given parent node.

  @param[in] LoopVar6       The parent node.
  @param[in] LoopVar5       The edge character.
  @param[in] LoopVar4       The child node.
**/
static
VOID
EFIAPI
MakeChild (
  IN PSCRATCH_DATA Globals,
  IN NODE          LoopVar6,
  IN UINT8         LoopVar5,
  IN NODE          LoopVar4
  )
{
  NODE  LoopVar12;

  NODE  LoopVar10;

  LoopVar12          = (NODE) HASH (LoopVar6, LoopVar5);
  LoopVar10          = Globals->mNext[LoopVar12];
  Globals->mNext[LoopVar12]   = LoopVar4;
  Globals->mNext[LoopVar4]    = LoopVar10;
  Globals->mPrev[LoopVar10]   = LoopVar4;
  Globals->mPrev[LoopVar4]    = LoopVar12;
  Globals->mParent[LoopVar4]  = LoopVar6;
  Globals->mChildCount[LoopVar6]++;
}

/**
  Split a node.

  @param[in] Old     The node to split.
**/
static
VOID
EFIAPI
Split (
  IN PSCRATCH_DATA Globals,
  IN NODE          Old
  )
{
  NODE  New;

  NODE  LoopVar10;

  New               = Globals->mAvail;
  Globals->mAvail            = Globals->mNext[New];
  Globals->mChildCount[New]  = 0;
  LoopVar10                 = Globals->mPrev[Old];
  Globals->mPrev[New]        = LoopVar10;
  Globals->mNext[LoopVar10]          = New;
  LoopVar10                 = Globals->mNext[Old];
  Globals->mNext[New]        = LoopVar10;
  Globals->mPrev[LoopVar10]          = New;
  Globals->mParent[New]      = Globals->mParent[Old];
  Globals->mLevel[New]       = (UINT8) Globals->mMatchLen;
  Globals->mPosition[New]    = Globals->mPos;
  MakeChild (Globals, New, Globals->mText[Globals->mMatchPos + Globals->mMatchLen], Old);
  MakeChild (Globals, New, Globals->mText[Globals->mPos + Globals->mMatchLen], Globals->mPos);
}

/**
  Insert string info for current position into the String Info Log.

**/
static
VOID
EFIAPI
InsertNode (
  PSCRATCH_DATA Globals
  )
{
  NODE  LoopVar6;

  NODE  LoopVar4;

  NODE  LoopVar2;

  NODE  LoopVar10;
  UINT8 LoopVar5;
  UINT8 *TempString3;
  UINT8 *TempString2;

  PSYSHOST host;
  host = Globals->host;

  if (Globals->mMatchLen >= 4) {
    //
    // We have just got a long match, the target tree
    // can be located by MatchPos + 1. Travese the tree
    // from bottom up to get to a proper starting point.
    // The usage of PERC_FLAG ensures proper node deletion
    // in DeleteNode() later.
    //
    Globals->mMatchLen--;
    LoopVar4 = (NODE) ((Globals->mMatchPos + 1) | WNDSIZ);
    LoopVar6 = Globals->mParent[LoopVar4];
    while (LoopVar6 == NIL) {
      LoopVar4 = Globals->mNext[LoopVar4];
      LoopVar6 = Globals->mParent[LoopVar4];
    }

    while (Globals->mLevel[LoopVar6] >= Globals->mMatchLen) {
      LoopVar4 = LoopVar6;
      LoopVar6 = Globals->mParent[LoopVar6];
    }

    LoopVar10 = LoopVar6;
    while (Globals->mPosition[LoopVar10] < 0) {
      Globals->mPosition[LoopVar10]  = Globals->mPos;
      LoopVar10             = Globals->mParent[LoopVar10];
    }

    if (LoopVar10 < WNDSIZ) {
      Globals->mPosition[LoopVar10] = (NODE) (Globals->mPos | PERC_FLAG);
    }
  } else {
    //
    // Locate the target tree
    //
    LoopVar6 = (NODE) (Globals->mText[Globals->mPos] + WNDSIZ);
    LoopVar5 = Globals->mText[Globals->mPos + 1];
    LoopVar4 = Child (Globals, LoopVar6, LoopVar5);
    if (LoopVar4 == NIL) {
      MakeChild (Globals, LoopVar6, LoopVar5, Globals->mPos);
      Globals->mMatchLen = 1;
      return ;
    }

    Globals->mMatchLen = 2;
  }
  //
  // Traverse down the tree to find a match.
  // Update Position value along the route.
  // Node split or creation is involved.
  //
  for (;;) {
    if (LoopVar4 >= WNDSIZ) {
      LoopVar2         = MAXMATCH;
      Globals->mMatchPos = LoopVar4;
    } else {
      LoopVar2         = Globals->mLevel[LoopVar4];
      Globals->mMatchPos = (NODE) (Globals->mPosition[LoopVar4] & ~PERC_FLAG);
    }

    if (Globals->mMatchPos >= Globals->mPos) {
      Globals->mMatchPos -= WNDSIZ;
    }

    TempString3  = &Globals->mText[Globals->mPos + Globals->mMatchLen];
    if((Globals->mMatchPos + Globals->mMatchLen) < 0 ){
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_65);
      return;
    }
    TempString2  = &Globals->mText[Globals->mMatchPos + Globals->mMatchLen];
    while (Globals->mMatchLen < LoopVar2) {
      if (*TempString3 != *TempString2) {
        Split (Globals, LoopVar4);
        return ;
      }

      Globals->mMatchLen++;
      TempString3++;
      TempString2++;
    }

    if (Globals->mMatchLen >= MAXMATCH) {
      break;
    }

    Globals->mPosition[LoopVar4]  = Globals->mPos;
    LoopVar6             = LoopVar4;
    LoopVar4             = Child (Globals, LoopVar6, *TempString3);
    if (LoopVar4 == NIL) {
      MakeChild (Globals, LoopVar6, *TempString3, Globals->mPos);
      return ;
    }

    Globals->mMatchLen++;
  }

  LoopVar10             = Globals->mPrev[LoopVar4];
  Globals->mPrev[Globals->mPos]   = LoopVar10;
  Globals->mNext[LoopVar10]      = Globals->mPos;
  LoopVar10             = Globals->mNext[LoopVar4];
  Globals->mNext[Globals->mPos]   = LoopVar10;
  Globals->mPrev[LoopVar10]      = Globals->mPos;
  Globals->mParent[Globals->mPos] = LoopVar6;
  Globals->mParent[LoopVar4]    = NIL;

  //
  // Special usage of 'next'
  //
  Globals->mNext[LoopVar4] = Globals->mPos;

}

/**
  Delete outdated string info. (The Usage of PERC_FLAG
  ensures a clean deletion).

**/
static
VOID
EFIAPI
DeleteNode (
  PSCRATCH_DATA Globals
  )
{
  NODE  LoopVar6;

  NODE  LoopVar4;

  NODE  LoopVar11;

  NODE  LoopVar10;

  NODE  LoopVar9;

  if (Globals->mParent[Globals->mPos] == NIL) {
    return ;
  }

  LoopVar4             = Globals->mPrev[Globals->mPos];
  LoopVar11             = Globals->mNext[Globals->mPos];
  Globals->mNext[LoopVar4]      = LoopVar11;
  Globals->mPrev[LoopVar11]      = LoopVar4;
  LoopVar4             = Globals->mParent[Globals->mPos];
  Globals->mParent[Globals->mPos] = NIL;
  if (LoopVar4 >= WNDSIZ) {
    return ;
  }

  Globals->mChildCount[LoopVar4]--;
  if (Globals->mChildCount[LoopVar4] > 1) {
    return ;
  }

  LoopVar10 = (NODE) (Globals->mPosition[LoopVar4] & ~PERC_FLAG);
  if (LoopVar10 >= Globals->mPos) {
    LoopVar10 -= WNDSIZ;
  }

  LoopVar11 = LoopVar10;
  LoopVar6 = Globals->mParent[LoopVar4];
  LoopVar9 = Globals->mPosition[LoopVar6];
  while ((LoopVar9 & PERC_FLAG) != 0){
    LoopVar9 &= ~PERC_FLAG;
    if (LoopVar9 >= Globals->mPos) {
      LoopVar9 -= WNDSIZ;
    }

    if (LoopVar9 > LoopVar11) {
      LoopVar11 = LoopVar9;
    }

    Globals->mPosition[LoopVar6]  = (NODE) (LoopVar11 | WNDSIZ);
    LoopVar6             = Globals->mParent[LoopVar6];
    LoopVar9             = Globals->mPosition[LoopVar6];
  }

  if (LoopVar6 < WNDSIZ) {
    if (LoopVar9 >= Globals->mPos) {
      LoopVar9 -= WNDSIZ;
    }

    if (LoopVar9 > LoopVar11) {
      LoopVar11 = LoopVar9;
    }

    Globals->mPosition[LoopVar6] = (NODE) (LoopVar11 | WNDSIZ | PERC_FLAG);
  }

  LoopVar11           = Child (Globals, LoopVar4, Globals->mText[LoopVar10 + Globals->mLevel[LoopVar4]]);
  LoopVar10           = Globals->mPrev[LoopVar11];
  LoopVar9           = Globals->mNext[LoopVar11];
  Globals->mNext[LoopVar10]    = LoopVar9;
  Globals->mPrev[LoopVar9]    = LoopVar10;
  LoopVar10           = Globals->mPrev[LoopVar4];
  Globals->mNext[LoopVar10]    = LoopVar11;
  Globals->mPrev[LoopVar11]    = LoopVar10;
  LoopVar10           = Globals->mNext[LoopVar4];
  Globals->mPrev[LoopVar10]    = LoopVar11;
  Globals->mNext[LoopVar11]    = LoopVar10;
  Globals->mParent[LoopVar11]  = Globals->mParent[LoopVar4];
  Globals->mParent[LoopVar4]  = NIL;
  Globals->mNext[LoopVar4]    = Globals->mAvail;
  Globals->mAvail      = LoopVar4;
}

/**
  Read in source data

  @param[out] LoopVar7   The buffer to hold the data.
  @param[in] LoopVar8    The number of bytes to read.

  @return The number of bytes actually read.
**/
static
INT32
EFIAPI
FreadCrc (
  IN PSCRATCH_DATA Globals,
  OUT UINT8        *LoopVar7,
  IN  INT32        LoopVar8
  )
{
  INT32 LoopVar1;

  for (LoopVar1 = 0; Globals->mSrcBase < Globals->mSrcUpperLimit && LoopVar1 < LoopVar8; LoopVar1++) {
    *LoopVar7++ = *Globals->mSrcBase++;
  }

  LoopVar8 = LoopVar1;

  LoopVar7 -= LoopVar8;
  Globals->mOrigSize += LoopVar8;
  LoopVar1--;
  while (LoopVar1 >= 0) {
    UPDATE_CRC (*LoopVar7++);
    LoopVar1--;
  }

  return LoopVar8;
}

/**
  Advance the current position (read in new data if needed).
  Delete outdated string info. Find a match string for current position.

  @retval TRUE      The operation was successful.
  @retval FALSE     The operation failed due to insufficient memory.
**/
static
BOOLEAN
EFIAPI
GetNextMatch (
  PSCRATCH_DATA Globals
  )
{
  INT32 LoopVar8;
  UINT8 Temp[WNDSIZ + MAXMATCH];

  Globals->mRemainder--;
  Globals->mPos++;
  if (Globals->mPos == WNDSIZ * 2) {
    CopyMem (Temp, &Globals->mText[WNDSIZ], WNDSIZ + MAXMATCH);
    CopyMem (&Globals->mText[0], Temp, WNDSIZ + MAXMATCH);

    LoopVar8 = FreadCrc (Globals, &Globals->mText[WNDSIZ + MAXMATCH], WNDSIZ);
    Globals->mRemainder += LoopVar8;
    Globals->mPos = WNDSIZ;
  }

  DeleteNode (Globals);
  InsertNode (Globals);

  return (TRUE);
}

/**
  Send entry LoopVar1 down the queue.

  @param[in] LoopVar1    The index of the item to move.
**/
static
VOID
EFIAPI
DownHeap (
  IN PSCRATCH_DATA Globals,
  IN INT32         i
  )
{
  INT32 LoopVar1;

  INT32 LoopVar2;

  //
  // priority queue: send i-th entry down heap
  //
  LoopVar2 = Globals->mHeap[i];
  LoopVar1 = 2 * i;
  while (LoopVar1 <= Globals->mHeapSize) {
    if (LoopVar1 < Globals->mHeapSize && Globals->mFreq[Globals->mHeap[LoopVar1]] > Globals->mFreq[Globals->mHeap[LoopVar1 + 1]]) {
      LoopVar1++;
    }

    if (Globals->mFreq[LoopVar2] <= Globals->mFreq[Globals->mHeap[LoopVar1]]) {
      break;
    }

    Globals->mHeap[i]  = Globals->mHeap[LoopVar1];
    i         = LoopVar1;
    LoopVar1         = 2 * i;
  }

  Globals->mHeap[i] = (INT16) LoopVar2;
}

/**
  Count the number of each code length for a Huffman tree.

  @param[in] LoopVar1      The top node.
**/
static
VOID
EFIAPI
CountLen (
  IN PSCRATCH_DATA Globals,
  IN INT32         LoopVar1
  )
{
  if (LoopVar1 < Globals->mTempInt32) {
    Globals->mLenCnt[(Globals->mHuffmanDepth < 16) ? Globals->mHuffmanDepth : 16]++;
  } else {
    Globals->mHuffmanDepth++;
    CountLen (Globals, Globals->mLeft[LoopVar1]);
    CountLen (Globals, Globals->mRight[LoopVar1]);
    Globals->mHuffmanDepth--;
  }
}

/**
  Create code length array for a Huffman tree.

  @param[in] Root   The root of the tree.
**/
static
VOID
EFIAPI
MakeLen (
  IN PSCRATCH_DATA Globals,
  IN INT32         Root
  )
{
  INT32   LoopVar1;

  INT32   LoopVar2;
  UINT32  Cum;

  for (LoopVar1 = 0; LoopVar1 <= 16; LoopVar1++) {
    Globals->mLenCnt[LoopVar1] = 0;
  }

  CountLen (Globals, Root);

  //
  // Adjust the length count array so that
  // no code will be generated longer than its designated length
  //
  Cum = 0;
  for (LoopVar1 = 16; LoopVar1 > 0; LoopVar1--) {
    Cum += Globals->mLenCnt[LoopVar1] << (16 - LoopVar1);
  }

  while (Cum != (1U << 16)) {
    Globals->mLenCnt[16]--;
    for (LoopVar1 = 15; LoopVar1 > 0; LoopVar1--) {
      if (Globals->mLenCnt[LoopVar1] != 0) {
        Globals->mLenCnt[LoopVar1]--;
        Globals->mLenCnt[LoopVar1 + 1] += 2;
        break;
      }
    }

    Cum--;
  }

  for (LoopVar1 = 16; LoopVar1 > 0; LoopVar1--) {
    LoopVar2 = Globals->mLenCnt[LoopVar1];
    LoopVar2--;
    while (LoopVar2 >= 0) {
      Globals->mLen[*Globals->mSortPtr++] = (UINT8) LoopVar1;
      LoopVar2--;
    }
  }
}

/**
  Assign code to each symbol based on the code length array.

  @param[in] LoopVar8      The number of symbols.
  @param[in] Len    The code length array.
  @param[out] Code  The stores codes for each symbol.
**/
static
VOID
EFIAPI
MakeCode (
  IN PSCRATCH_DATA Globals,
  IN  INT32        LoopVar8,
  IN  UINT8        Len[    ],
  OUT UINT16       Code[  ]
  )
{
  INT32   LoopVar1;
  UINT16  Start[18];

  Start[1] = 0;
  for (LoopVar1 = 1; LoopVar1 <= 16; LoopVar1++) {
    Start[LoopVar1 + 1] = (UINT16) ((Start[LoopVar1] + Globals->mLenCnt[LoopVar1]) << 1);
  }

  for (LoopVar1 = 0; LoopVar1 < LoopVar8; LoopVar1++) {
    Code[LoopVar1] = Start[Len[LoopVar1]]++;
  }
}

/**
  Generates Huffman codes given a frequency distribution of symbols.

  @param[in] NParm      The number of symbols.
  @param[in] FreqParm   The frequency of each symbol.
  @param[out] LenParm   The code length for each symbol.
  @param[out] CodeParm  The code for each symbol.

  @return The root of the Huffman tree.
**/
static
INT32
EFIAPI
MakeTree (
  IN  PSCRATCH_DATA  Globals,
  IN  INT32          NParm,
  IN  UINT16         FreqParm[ ],
  OUT UINT8          LenParm[  ],
  OUT UINT16         CodeParm[ ]
  )
{
  INT32 LoopVar1;

  INT32 LoopVar2;

  INT32 LoopVar3;

  INT32 Avail;

  //for (LoopVar1 = 0; LoopVar1 < NParm; LoopVar1++) {
  //  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
  //                  "MakeTree: NSymbols = %d, FreqParm[%d] = %d\n", NParm, LoopVar1, FreqParm[LoopVar1]));
  //}
  //
  // make tree, calculate len[], return root
  //
  Globals->mTempInt32        = NParm;
  Globals->mFreq     = FreqParm;
  Globals->mLen      = LenParm;
  Avail     = Globals->mTempInt32;
  Globals->mHeapSize = 0;
  Globals->mHeap[1]  = 0;
  for (LoopVar1 = 0; LoopVar1 < Globals->mTempInt32; LoopVar1++) {
    Globals->mLen[LoopVar1] = 0;
    if ((Globals->mFreq[LoopVar1]) != 0) {
      Globals->mHeapSize++;
      Globals->mHeap[Globals->mHeapSize] = (INT16) LoopVar1;
    }
  }

  if (Globals->mHeapSize < 2) {
    CodeParm[Globals->mHeap[1]] = 0;
    return Globals->mHeap[1];
  }

  for (LoopVar1 = Globals->mHeapSize / 2; LoopVar1 >= 1; LoopVar1--) {
    //
    // make priority queue
    //
    DownHeap (Globals, LoopVar1);
  }

  Globals->mSortPtr = CodeParm;
  do {
    LoopVar1 = Globals->mHeap[1];
    if (LoopVar1 < Globals->mTempInt32) {
      *Globals->mSortPtr++ = (UINT16) LoopVar1;
    }

    Globals->mHeap[1] = Globals->mHeap[Globals->mHeapSize--];
    DownHeap (Globals, 1);
    LoopVar2 = Globals->mHeap[1];
    if (LoopVar2 < Globals->mTempInt32) {
      *Globals->mSortPtr++ = (UINT16) LoopVar2;
    }

    LoopVar3         = Avail++;
    Globals->mFreq[LoopVar3]  = (UINT16) (Globals->mFreq[LoopVar1] + Globals->mFreq[LoopVar2]);
    Globals->mHeap[1]  = (INT16) LoopVar3;
    DownHeap (Globals, 1);
    Globals->mLeft[LoopVar3]  = (UINT16) LoopVar1;
    Globals->mRight[LoopVar3] = (UINT16) LoopVar2;
  } while (Globals->mHeapSize > 1);

  Globals->mSortPtr = CodeParm;
  MakeLen (Globals, LoopVar3);
  MakeCode (Globals, NParm, LenParm, CodeParm);

  //
  // return root
  //
  return LoopVar3;
}

/**
  Outputs rightmost LoopVar8 bits of x

  @param[in] LoopVar8   The rightmost LoopVar8 bits of the data is used.
  @param[in] x   The data.
**/
static
VOID
EFIAPI
PutBits (
  IN PSCRATCH_DATA Globals,
  IN INT32         LoopVar8,
  IN UINT32        x
  )
{
  UINT8 Temp;

  if (LoopVar8 < Globals->mBitCount) {
    Globals->mSubBitBuf |= x << (Globals->mBitCount -= LoopVar8);
  } else {

    Temp = (UINT8)(Globals->mSubBitBuf | (x >> (LoopVar8 -= Globals->mBitCount)));
    if (Globals->mDstBase < Globals->mDstUpperLimit) {
      *Globals->mDstBase++ = Temp;
    }
    Globals->mCompSize++;

    if (LoopVar8 < UINT8_BIT) {
      Globals->mSubBitBuf = x << (Globals->mBitCount = UINT8_BIT - LoopVar8);
    } else {

      Temp = (UINT8)(x >> (LoopVar8 - UINT8_BIT));
      if (Globals->mDstBase < Globals->mDstUpperLimit) {
        *Globals->mDstBase++ = Temp;
      }
      Globals->mCompSize++;

      Globals->mSubBitBuf = x << (Globals->mBitCount = 2 * UINT8_BIT - LoopVar8);
    }
  }
}

/**
  Encode a signed 32 bit number.

  @param[in] LoopVar5     The number to encode.
**/
static
VOID
EFIAPI
EncodeC (
  IN PSCRATCH_DATA Globals,
  IN INT32         LoopVar5
  )
{
  PutBits (Globals, Globals->mCLen[LoopVar5], Globals->mCCode[LoopVar5]);
}

/**
  Encode a unsigned 32 bit number.

  @param[in] LoopVar7     The number to encode.
**/
static
VOID
EFIAPI
EncodeP (
  IN PSCRATCH_DATA Globals,
  IN UINT32        LoopVar7
  )
{
  UINT32  LoopVar5;

  UINT32  LoopVar6;

  LoopVar5 = 0;
  LoopVar6 = LoopVar7;
  while (LoopVar6 != 0) {
    LoopVar6 >>= 1;
    LoopVar5++;
  }

  PutBits (Globals, Globals->mPTLen[LoopVar5], Globals->mPTCode[LoopVar5]);
  if (LoopVar5 > 1) {
    PutBits(Globals, LoopVar5 - 1, LoopVar7 & (0xFFFFU >> (17 - LoopVar5)));
  }
}

/**
  Count the frequencies for the Extra Set.

**/
static
VOID
EFIAPI
CountTFreq (
  PSCRATCH_DATA Globals
  )
{
  INT32 LoopVar1;

  INT32 LoopVar3;

  INT32 LoopVar8;

  INT32 Count;

  for (LoopVar1 = 0; LoopVar1 < NT; LoopVar1++) {
    Globals->mTFreq[LoopVar1] = 0;
  }

  LoopVar8 = NC;
  while (LoopVar8 > 0 && Globals->mCLen[LoopVar8 - 1] == 0) {
    LoopVar8--;
  }

  LoopVar1 = 0;
  while (LoopVar1 < LoopVar8) {
    LoopVar3 = Globals->mCLen[LoopVar1++];
    if (LoopVar3 == 0) {
      Count = 1;
      while (LoopVar1 < LoopVar8 && Globals->mCLen[LoopVar1] == 0) {
        LoopVar1++;
        Count++;
      }

      if (Count <= 2) {
        Globals->mTFreq[0] = (UINT16) (Globals->mTFreq[0] + Count);
      } else if (Count <= 18) {
        Globals->mTFreq[1]++;
      } else if (Count == 19) {
        Globals->mTFreq[0]++;
        Globals->mTFreq[1]++;
      } else {
        Globals->mTFreq[2]++;
      }
    } else {
      //RC_ASSERT((LoopVar3+2)<(2 * NT - 1));
      Globals->mTFreq[LoopVar3 + 2]++;
    }
  }
}

/**
  Outputs the code length array for the Extra Set or the Position Set.

  @param[in] LoopVar8       The number of symbols.
  @param[in] nbit           The number of bits needed to represent 'LoopVar8'.
  @param[in] Special        The special symbol that needs to be take care of.

**/
static
VOID
EFIAPI
WritePTLen (
  IN PSCRATCH_DATA Globals,
  IN INT32         LoopVar8,
  IN INT32         nbit,
  IN INT32         Special
  )
{
  INT32 LoopVar1;

  INT32 LoopVar3;

  while (LoopVar8 > 0 && Globals->mPTLen[LoopVar8 - 1] == 0) {
    LoopVar8--;
  }

  PutBits (Globals, nbit, LoopVar8);
  LoopVar1 = 0;
  while (LoopVar1 < LoopVar8) {
    LoopVar3 = Globals->mPTLen[LoopVar1++];
    if (LoopVar3 <= 6) {
      PutBits (Globals, 3, LoopVar3);
    } else {
      PutBits (Globals, LoopVar3 - 3, (1U << (LoopVar3 - 3)) - 2);
    }

    if (LoopVar1 == Special) {
      while (LoopVar1 < 6 && Globals->mPTLen[LoopVar1] == 0) {
        LoopVar1++;
      }

      PutBits (Globals, 2, (LoopVar1 - 3) & 3);
    }
  }
}

/**
  Outputs the code length array for Char&Length Set.
**/
static
VOID
EFIAPI
WriteCLen (
  PSCRATCH_DATA Globals
  )
{
  INT32 LoopVar1;

  INT32 LoopVar3;

  INT32 LoopVar8;

  INT32 Count;

  LoopVar8 = NC;
  while (LoopVar8 > 0 && Globals->mCLen[LoopVar8 - 1] == 0) {
    LoopVar8--;
  }

  PutBits (Globals, CBIT, LoopVar8);
  LoopVar1 = 0;
  while (LoopVar1 < LoopVar8) {
    LoopVar3 = Globals->mCLen[LoopVar1++];
    if (LoopVar3 == 0) {
      Count = 1;
      while (LoopVar1 < LoopVar8 && Globals->mCLen[LoopVar1] == 0) {
        LoopVar1++;
        Count++;
      }

      if (Count <= 2) {
        for (LoopVar3 = 0; LoopVar3 < Count; LoopVar3++) {
          PutBits (Globals, Globals->mPTLen[0], Globals->mPTCode[0]);
        }
      } else if (Count <= 18) {
        PutBits (Globals, Globals->mPTLen[1], Globals->mPTCode[1]);
        PutBits (Globals, 4, Count - 3);
      } else if (Count == 19) {
        PutBits (Globals, Globals->mPTLen[0], Globals->mPTCode[0]);
        PutBits (Globals, Globals->mPTLen[1], Globals->mPTCode[1]);
        PutBits (Globals, 4, 15);
      } else {
        PutBits (Globals, Globals->mPTLen[2], Globals->mPTCode[2]);
        PutBits (Globals, CBIT, Count - 20);
      }
    } else {
      //ASSERT((LoopVar3+2)<NPT);
      PutBits (Globals, Globals->mPTLen[LoopVar3 + 2], Globals->mPTCode[LoopVar3 + 2]);
    }
  }
}

/**
  Huffman code the block and output it.

**/
static
VOID
EFIAPI
SendBlock (
  PSCRATCH_DATA Globals
  )
{
  UINT32  LoopVar1;

  UINT32  LoopVar3;

  UINT32  Flags;

  UINT32  Root;

  UINT32  Pos;

  UINT32  Size;
  Flags = 0;

  Root  = MakeTree (Globals, NC, Globals->mCFreq, Globals->mCLen, Globals->mCCode);
  Size  = Globals->mCFreq[Root];
  PutBits (Globals, 16, Size);
  if (Root >= NC) {
    CountTFreq (Globals);
    Root = MakeTree (Globals, NT, Globals->mTFreq, Globals->mPTLen, Globals->mPTCode);
    if (Root >= NT) {
      WritePTLen (Globals, NT, TBIT, 3);
    } else {
      PutBits (Globals, TBIT, 0);
      PutBits (Globals, TBIT, Root);
    }

    WriteCLen (Globals);
  } else {
    PutBits (Globals, TBIT, 0);
    PutBits (Globals, TBIT, 0);
    PutBits (Globals, CBIT, 0);
    PutBits (Globals, CBIT, Root);
  }

  Root = MakeTree (Globals, NP, Globals->mPFreq, Globals->mPTLen, Globals->mPTCode);
  if (Root >= NP) {
    WritePTLen (Globals, NP, Globals->mPBit, -1);
  } else {
    PutBits (Globals, Globals->mPBit, 0);
    PutBits (Globals, Globals->mPBit, Root);
  }

  Pos = 0;
  for (LoopVar1 = 0; LoopVar1 < Size; LoopVar1++) {
    if (LoopVar1 % UINT8_BIT == 0) {
      Flags = Globals->mBuf[Pos++];
    } else {
      Flags <<= 1;
    }
    if ((Flags & (1U << (UINT8_BIT - 1))) != 0){
      EncodeC(Globals, Globals->mBuf[Pos++] + (1U << UINT8_BIT));
      LoopVar3 = Globals->mBuf[Pos++] << UINT8_BIT;
      LoopVar3 += Globals->mBuf[Pos++];

      EncodeP (Globals, LoopVar3);
    } else {
      EncodeC (Globals, Globals->mBuf[Pos++]);
    }
  }

  SetMem (Globals->mCFreq, NC * sizeof (UINT16), 0);
  SetMem (Globals->mPFreq, NP * sizeof (UINT16), 0);
}

/**
  Start the huffman encoding.

**/
static
VOID
EFIAPI
HufEncodeStart (
  PSCRATCH_DATA Globals
  )
{
  SetMem (Globals->mCFreq, NC * sizeof (UINT16), 0);
  SetMem (Globals->mPFreq, NP * sizeof (UINT16), 0);

  Globals->mOutputPos = Globals->mOutputMask = 0;

  Globals->mBitCount   = UINT8_BIT;
  Globals->mSubBitBuf  = 0;
}

/**
  Outputs an Original Character or a Pointer.

  @param[in] LoopVar5     The original character or the 'String Length' element of
                   a Pointer.
  @param[in] LoopVar7     The 'Position' field of a Pointer.
**/
static
VOID
EFIAPI
CompressOutput (
  IN PSCRATCH_DATA Globals,
  IN UINT32        LoopVar5,
  IN UINT32        LoopVar7
  )
{

  if ((Globals->mOutputMask >>= 1) == 0) {
    Globals->mOutputMask = 1U << (UINT8_BIT - 1);
    if (Globals->mOutputPos >= Globals->mBufSiz - 3 * UINT8_BIT) {
      SendBlock (Globals);
      Globals->mOutputPos = 0;
    }

    Globals->CPos        = Globals->mOutputPos++;
    Globals->mBuf[Globals->CPos]  = 0;
  }
  Globals->mBuf[Globals->mOutputPos++] = (UINT8) LoopVar5;
  Globals->mCFreq[LoopVar5]++;
  if (LoopVar5 >= (1U << UINT8_BIT)) {
    Globals->mBuf[Globals->CPos] = (UINT8)(Globals->mBuf[Globals->CPos]|Globals->mOutputMask);
    Globals->mBuf[Globals->mOutputPos++] = (UINT8)(LoopVar7 >> UINT8_BIT);
    Globals->mBuf[Globals->mOutputPos++] = (UINT8) LoopVar7;
    LoopVar5                  = 0;
    while (LoopVar7!=0) {
      LoopVar7 >>= 1;
      LoopVar5++;
    }
    Globals->mPFreq[LoopVar5]++;
  }
}

/**
  End the huffman encoding.

**/
static
VOID
EFIAPI
HufEncodeEnd (
  PSCRATCH_DATA Globals
  )
{
  SendBlock (Globals);

  //
  // Flush remaining bits
  //
  PutBits (Globals, UINT8_BIT - 1, 0);
}

/**
  The main controlling routine for compression process.

  @retval EFI_SUCCESS           The compression is successful.
  @retval EFI_OUT_0F_RESOURCES  Not enough memory for compression process.
**/
static
EFI_STATUS
EFIAPI
Encode (
  PSCRATCH_DATA Globals
  )
{
  EFI_STATUS  Status;
  INT32       LastMatchLen;
  NODE        LastMatchPos;

  Status = AllocateMemory (Globals);
  if (EFI_ERROR (Status)) {
    FreeMemory (Globals);
    return Status;
  }

  InitSlide (Globals);

  HufEncodeStart (Globals);

  Globals->mRemainder  = FreadCrc (Globals, &Globals->mText[WNDSIZ], WNDSIZ + MAXMATCH);

  Globals->mMatchLen   = 0;
  Globals->mPos        = WNDSIZ;
  InsertNode (Globals);
  if (Globals->mMatchLen > Globals->mRemainder) {
    Globals->mMatchLen = Globals->mRemainder;
  }

  while (Globals->mRemainder > 0) {
    LastMatchLen  = Globals->mMatchLen;
    LastMatchPos  = Globals->mMatchPos;
    if (!GetNextMatch (Globals)) {
      Status = (UINT32)EFI_OUT_OF_RESOURCES;
    }
    if (Globals->mMatchLen > Globals->mRemainder) {
      Globals->mMatchLen = Globals->mRemainder;
    }

    if (Globals->mMatchLen > LastMatchLen || LastMatchLen < THRESHOLD) {
      //
      // Not enough benefits are gained by outputting a pointer,
      // so just output the original character
      //
      CompressOutput(Globals, Globals->mText[Globals->mPos - 1], 0);
    } else {
      //
      // Outputting a pointer is beneficial enough, do it.
      //

      CompressOutput(Globals, LastMatchLen + (UINT8_MAX + 1 - THRESHOLD),
             (Globals->mPos - LastMatchPos - 2) & (WNDSIZ - 1));
      LastMatchLen--;
      while (LastMatchLen > 0) {
        if (!GetNextMatch (Globals)) {
          Status = (UINT32)EFI_OUT_OF_RESOURCES;
        }
        LastMatchLen--;
      }

      if (Globals->mMatchLen > Globals->mRemainder) {
        Globals->mMatchLen = Globals->mRemainder;
      }
    }
  }

  HufEncodeEnd (Globals);
  FreeMemory (Globals);
  return (Status);
}

/**
  The compression routine.
  @param[in]       SrcBuffer     The buffer containing the source data.
  @param[in]       SrcSize       The number of bytes in SrcBuffer.
  @param[in]       DstBuffer     The buffer to put the compressed image in.
  @param[in, out]  DstSize       On input the size (in bytes) of DstBuffer, on
                                 return the number of bytes placed in DstBuffer
  @param           Version       The version of de/compression algorithm.
                                      Version 1 for EFI 1.1 de/compression algorithm.
                                      Version 2 for Tiano de/compression algorithm.
  @param[in]       Sd           Scratch Data on stack
  @retval EFI_SUCCESS           The compression was sucessful.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.  DstSize is required.
**/
UINT32
CompressMrc (
  IN       VOID         *SrcBuffer,
  IN       UINT32       SrcSize,
  IN       VOID         *DstBuffer,
  IN OUT   UINT32       *DstSize,
  IN       UINT8        Version,
  IN       VOID         *Sd
  )
{
  EFI_STATUS     Status;
  PSCRATCH_DATA  Globals;

  Globals = (PSCRATCH_DATA)Sd;

  // Debug
  /*
  MemDebugPrint((Globals->host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
               "Compress: SrcSize = %d, WNDBIT = %d, WNDSIZ = %d, BLKSIZ = %d, size Globals = %d, Version = %d\n",
               SrcSize, WNDBIT, WNDSIZ, BLKSIZ, sizeof(*Globals), Version));
  */

  //
  // Initializations
  //
  Globals->mBufSiz         = 0;

  //
  // The length of the field 'Position Set Code Length Array Size' in Block Header.
  // For EFI 1.1 de/compression algorithm(Version 1), mPBit = 4
  // For Tiano de/compression algorithm(Version 2), mPBit = 5
  //
  switch (Version) {
  case 1:
    Globals->mPBit = 4;
    break;

  case 2:
    Globals->mPBit = 5;
    break;

  default:
    //
    // Currently, only have 2 versions
    //
    return UNKNOWN_VERSION;
  }

  Globals->mSrcBase        = (UINT8 *)SrcBuffer;
  Globals->mSrcUpperLimit  = Globals->mSrcBase + SrcSize;
  Globals->mDstBase        = (UINT8 *)DstBuffer;
  Globals->mDstUpperLimit  = Globals->mDstBase +*DstSize;

  PutDword (Globals, 0L);
  PutDword (Globals, 0L);

  MakeCrcTable (Globals);

  Globals->mOrigSize             = Globals->mCompSize = 0;
  Globals->mCrc                  = INIT_CRC;

  //
  // Compress it
  //
  Status = Encode (Globals);
  if (EFI_ERROR (Status)) {
    return (UINT32)EFI_OUT_OF_RESOURCES;
  }
  //
  // Null terminate the compressed data
  //
  if (Globals->mDstBase < Globals->mDstUpperLimit) {
    *Globals->mDstBase++ = 0;
  }
  //
  // Fill in compressed size and original size
  //
  Globals->mDstBase = (UINT8 *)DstBuffer;
  PutDword (Globals, Globals->mCompSize + 1);
  PutDword (Globals, Globals->mOrigSize);

  //
  // Return
  //
  if (Globals->mCompSize + 1 + 8 > *DstSize) {
    *DstSize = Globals->mCompSize + 1 + 8;
    return (UINT32)EFI_BUFFER_TOO_SMALL;
  } else {
    *DstSize = Globals->mCompSize + 1 + 8;
    return EFI_SUCCESS;
  }

}


/**
  The compression routine RC wrapper.
  @param[in]       host          Pointer to host struct.
  @param[in]       SrcBuffer     The buffer containing the source data.
  @param[in]       SrcSize       The number of bytes in SrcBuffer.
  @param[in]       DstBuffer     The buffer to put the compressed image in.
  @param[in, out]  DstSize       On input the size (in bytes) of DstBuffer, on
                                return the number of bytes placed in DstBuffer.

  @retval EFI_SUCCESS           The compression was sucessful.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.  DstSize is required.
**/
UINT32
CompressRc (
  IN       PSYSHOST host,
  IN       VOID     *SrcBuffer,
  IN       UINT32   SrcSize,
  IN       VOID     *DstBuffer,
  IN OUT   UINT32   *DstSize
  )
{
  SCRATCH_DATA Globals;

  // Clear globals
  MemSetLocal ((UINT8 *)&Globals, 0, sizeof(Globals));

  Globals.host = host;

  return CompressMrc (SrcBuffer, SrcSize, DstBuffer, DstSize, 2, &Globals);
}

//#endif // (!defined(SIM_BUILD) && !defined(HEADLESS_MRC)) || defined(RC_SIM)

