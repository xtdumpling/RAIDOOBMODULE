/**

Copyright (c) 2004-2016 Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @file Decompress.c

  Decompressor. Algorithm Ported from OPSD code (Decomp.asm)

**/

#include "SysFunc.h"
#include "Compression.h"

UINT32
DecompressRc (
             PSYSHOST host,
             UINT8    *Src,
             UINT8    *Dest
             )
/*++

  Decompress code at src to dest

  @param Src   - Pointer to the compressed code
  @param Dest  - Pointer to place the uncompressed code

  @retval 0 - Success
  @retval Other - Failure error code

--*/
{
  UINT8 scratchBuf[sizeof (SCRATCH_DATA)];

  ((SCRATCH_DATA *)scratchBuf)->host = host;

  return Decompress ((void *)Src, *(UINT32 *)(Src) + 8, (void *)Dest,
                     *(UINT32 *)(Src + 4), scratchBuf, sizeof (scratchBuf), 2);
}

void
FillBuf (
        SCRATCH_DATA  *Sd,
        UINT16        NumOfBits
        )
/*++

  Shift mBitBuf NumOfBits left. Read in NumOfBits of bits from source.

  @param Sd        - The global scratch data
  @param NumOfBits  - The number of bits to shift and read.

  @retval N/A

--*/
{
  Sd->mBitBuf = (UINT32) (Sd->mBitBuf << NumOfBits);

  while (NumOfBits > Sd->mBitCount) {

    Sd->mBitBuf |= (UINT32) (Sd->mSubBitBuf << (NumOfBits = (UINT16) (NumOfBits - Sd->mBitCount)));

    if (Sd->mCompSize > 0) {
      //
      // Get 1 byte into SubBitBuf
      //
      Sd->mCompSize--;
      Sd->mSubBitBuf  = 0;
      Sd->mSubBitBuf  = Sd->mSrcBase[Sd->mInBuf++];
      Sd->mBitCount   = 8;

    } else {
      //
      // No more bits from the source, just pad zero bit.
      //
      Sd->mSubBitBuf  = 0;
      Sd->mBitCount   = 8;

    }
  }

  Sd->mBitCount = (UINT16) (Sd->mBitCount - NumOfBits);
  Sd->mBitBuf |= Sd->mSubBitBuf >> Sd->mBitCount;
}

UINT32
GetBits (
        SCRATCH_DATA  *Sd,
        UINT16        NumOfBits
        )
/*++

  Get NumOfBits of bits out from mBitBuf. Fill mBitBuf with subsequent
  NumOfBits of bits from source. Returns NumOfBits of bits that are
  popped out.

  @param Sd            - The global scratch data.
  @param NumOfBits     - The number of bits to pop and read.

  @retval The bits that are popped out.

--*/
{
  UINT32  OutBits;

  OutBits = (UINT32) (Sd->mBitBuf >> (BITBUFSIZ - NumOfBits));

  FillBuf (Sd, NumOfBits);

  return OutBits;
}

UINT16
MakeTable (
          SCRATCH_DATA  *Sd,
          UINT16        NumOfChar,
          UINT8         *BitLen,
          UINT16        TableBits,
          UINT16        *Table
          )
/*++

  Creates Huffman Code mapping table according to code length array.

  @param Sd        - The global scratch data
  @param NumOfChar - Number of symbols in the symbol set
  @param BitLen    - Code length array
  @param TableBits - The width of the mapping table
  @param Table     - The table

  @retval 0         - OK.
  @retval BAD_TABLE - The table is corrupted.

--*/
{
  UINT16  Count[17];
  UINT16  Weight[17];
  UINT16  Start[18];
  UINT16  *Pointer;
  UINT16  Index3;
  UINT16  Index;
  UINT16  Len;
  UINT16  Char;
  UINT16  JuBits;
  UINT16  Avail;
  UINT16  NextCode;
  UINT16  Mask;

  for (Index = 1; Index <= 16; Index++) {
    Count[Index] = 0;
  }

  for (Index = 0; Index < NumOfChar; Index++) {
    Count[BitLen[Index]]++;
  }

  Start[1] = 0;

  for (Index = 1; Index <= 16; Index++) {
    Start[Index + 1] = (UINT16) (Start[Index] + (Count[Index] << (16 - Index)));
  }

  if (Start[17] != 0) {
    /*(1U << 16)*/
    return(UINT16) BAD_TABLE;
  }

  JuBits = (UINT16) (16 - TableBits);

  for (Index = 1; Index <= TableBits; Index++) {
    Start[Index] >>= JuBits;
    Weight[Index] = (UINT16) (1U << (TableBits - Index));
  }

  while (Index <= 16) {
    Weight[Index++] = (UINT16) (1U << (16 - Index));
  }

  Index = (UINT16) (Start[TableBits + 1] >> JuBits);

  if (Index != 0) {
    Index3 = (UINT16) (1U << TableBits);
    while (Index != Index3) {
      Table[Index++] = 0;
    }
  }

  Avail = NumOfChar;
  Mask  = (UINT16) (1U << (15 - TableBits));

  for (Char = 0; Char < NumOfChar; Char++) {

    Len = BitLen[Char];
    if (Len == 0 || Len > 16) {
      continue;
    }

    NextCode = (UINT16) (Start[Len] + Weight[Len]);

    if (Len <= TableBits) {

      for (Index = Start[Len]; Index < NextCode; Index++) {
        Table[Index] = Char;
      }

    } else {

      Index3  = Start[Len];
      Pointer = &Table[Index3 >> JuBits];
      Index   = (UINT16) (Len - TableBits);

      while (Index != 0) {
        if (*Pointer == 0) {
          Sd->mRight[Avail]                     = Sd->mLeft[Avail] = 0;
          *Pointer = Avail++;
        }

        if (Index3 & Mask) {
          Pointer = &Sd->mRight[*Pointer];
        } else {
          Pointer = &Sd->mLeft[*Pointer];
        }

        Index3 <<= 1;
        Index--;
      }

      *Pointer = Char;

    }

    Start[Len] = NextCode;
  }
  //
  // Succeeds
  //
  return 0;
}

UINT32
DecodeP (
        SCRATCH_DATA  *Sd
        )
/*++

  Decodes a position value.

  @param Sd      - the global scratch data

  @retval The position value decoded.

--*/
{
  UINT16  Val;
  UINT32  Mask;
  UINT32  Pos;

  Val = Sd->mPTTable[Sd->mBitBuf >> (BITBUFSIZ - 8)];

  if (Val >= MAXNP) {
    Mask = 1U << (BITBUFSIZ - 1 - 8);

    do {

      if (Sd->mBitBuf & Mask) {
        Val = Sd->mRight[Val];
      } else {
        Val = Sd->mLeft[Val];
      }

      Mask >>= 1;
    } while (Val >= MAXNP);
  }
  //
  // Advance what we have read
  //
  FillBuf (Sd, Sd->mPTLen[Val]);

  Pos = Val;
  if (Val > 1) {
    Pos = (UINT32) ((1U << (Val - 1)) + GetBits (Sd, (UINT16) (Val - 1)));
  }

  return Pos;
}

UINT16
ReadPTLen (
          SCRATCH_DATA  *Sd,
          UINT16        nn,
          UINT16        nbit,
          UINT16        Special
          )
/*++

  Reads code lengths for the Extra Set or the Position Set

  @param Sd        - The global scratch data
  @param nn        - Number of symbols
  @param nbit      - Number of bits needed to represent nn
  @param Special   - The special symbol that needs to be taken care of

  @retval 0         - OK.
  @retval BAD_TABLE - Table is corrupted.

--*/
{
  UINT16  Number;
  UINT16  CharC;
  UINT16  Index;
  UINT32  Mask;


//Ensure nn is within mPTTable[NPT] indexing bound
  if (nn > NPT) {
    nn = NPT;
  }

  Number = (UINT16) GetBits (Sd, nbit);

  if (Number == 0) {
    CharC = (UINT16) GetBits (Sd, nbit);

    for (Index = 0; Index < 256; Index++) {
      Sd->mPTTable[Index] = CharC;
    }

    for (Index = 0; Index < nn; Index++) {
      Sd->mPTLen[Index] = 0;
    }

    return 0;
  }

  Index = 0;

  while (Index < Number) {

    CharC = (UINT16) (Sd->mBitBuf >> (BITBUFSIZ - 3));

    if (CharC == 7) {
      Mask = 1U << (BITBUFSIZ - 1 - 3);
      while (Mask & Sd->mBitBuf) {
        Mask >>= 1;
        CharC += 1;
      }
    }

    FillBuf (Sd, (UINT16) ((CharC < 7) ? 3 : CharC - 3));

    Sd->mPTLen[Index++] = (UINT8) CharC;

    if (Index == Special) {
      CharC = (UINT16) GetBits (Sd, 2);
      while (((CharC--) >= 1) && (Index < nn)) {
        Sd->mPTLen[Index++] = 0;
      }
    }
  }

  while (Index < nn) {
    Sd->mPTLen[Index++] = 0;
  }

  return MakeTable (Sd, nn, Sd->mPTLen, 8, Sd->mPTTable);
}

void
ReadCLen (
         SCRATCH_DATA  *Sd
         )
/*++

  Reads code lengths for Char&Len Set.

  @param Sd    - the global scratch data

  @retval N/A

--*/
{
  UINT16  Number;
  UINT16  CharC;
  UINT16  Index;
  UINT32  Mask;

  Number = (UINT16) GetBits (Sd, CBIT);

  if (Number == 0) {
    CharC = (UINT16) GetBits (Sd, CBIT);

    for (Index = 0; Index < NC; Index++) {
      Sd->mCLen[Index] = 0;
    }

    for (Index = 0; Index < 4096; Index++) {
      Sd->mCTable[Index] = CharC;
    }

    return ;
  }

  Index = 0;
  while (Index < Number) {

    CharC = Sd->mPTTable[Sd->mBitBuf >> (BITBUFSIZ - 8)];
    if (CharC >= _NT) {
      Mask = 1U << (BITBUFSIZ - 1 - 8);

      do {

        if (Mask & Sd->mBitBuf) {
          CharC = Sd->mRight[CharC];
        } else {
          CharC = Sd->mLeft[CharC];
        }

        Mask >>= 1;

      } while (CharC >= _NT);
    }
    //
    // Advance what we have read
    //
    FillBuf (Sd, Sd->mPTLen[CharC]);

    if (CharC <= 2) {

      if (CharC == 0) {
        CharC = 1;
      } else if (CharC == 1) {
        CharC = (UINT16) (GetBits (Sd, 4) + 3);
      } else if (CharC == 2) {
        CharC = (UINT16) (GetBits (Sd, CBIT) + 20);
      }

      while ((short) (--CharC) >= 0) {
        Sd->mCLen[Index++] = 0;
      }

    } else {

      Sd->mCLen[Index++] = (UINT8) (CharC - 2);

    }
  }

  while (Index < NC) {
    Sd->mCLen[Index++] = 0;
  }

  MakeTable (Sd, NC, Sd->mCLen, 12, Sd->mCTable);

  return ;
}

UINT16
DecodeC (
        SCRATCH_DATA  *Sd
        )
/*++

  Decode a Character/length value.

  @param Sd    - The global scratch data.

  @retval The value decoded.

--*/
{
  UINT16  Index2;
  UINT32  Mask;

  if (Sd->mBlockSize == 0) {
    //
    // Starting a new block
    //
    Sd->mBlockSize    = (UINT16) GetBits (Sd, 16);
    Sd->mBadTableFlag = ReadPTLen (Sd, _NT, TBIT, 3);
    if (Sd->mBadTableFlag != 0) {
      return 0;
    }

    ReadCLen (Sd);

    Sd->mBadTableFlag = ReadPTLen (Sd, MAXNP, Sd->mPBit, (UINT16) (-1));
    if (Sd->mBadTableFlag != 0) {
      return 0;
    }
  }

  Sd->mBlockSize--;
  Index2 = Sd->mCTable[Sd->mBitBuf >> (BITBUFSIZ - 12)];

  if (Index2 >= NC) {
    Mask = 1U << (BITBUFSIZ - 1 - 12);

    do {
      if (Sd->mBitBuf & Mask) {
        Index2 = Sd->mRight[Index2];
      } else {
        Index2 = Sd->mLeft[Index2];
      }

      Mask >>= 1;
    } while (Index2 >= NC);
  }
  //
  // Advance what we have read
  //
  FillBuf (Sd, Sd->mCLen[Index2]);

  return Index2;
}

void
Decode (
       SCRATCH_DATA  *Sd
       )
/*++

  Decode the source data and put the resulting data into the destination buffer.

  @param Sd            - The global scratch data

  @retval N/A

 --*/
{
  UINT16  BytesRemain;
  UINT32  DataIdx;
  UINT16  CharC;

  BytesRemain = (UINT16) (-1);

  DataIdx     = 0;

  for (;;) {
    CharC = DecodeC (Sd);
    if (Sd->mBadTableFlag != 0) {
      return ;
    }

    if (CharC < 256) {
      //
      // Process an Original Character
      //
      if (Sd->mOutBuf >= Sd->mOrigSize) {
        return ;
      } else {
        Sd->mDstBase[Sd->mOutBuf++] = (UINT8) CharC;
      }

    } else {
      //
      // Process a Pointer
      //
      CharC       = (UINT16) (CharC - (UINT8_MAX + 1 - THRESHOLD));

      BytesRemain = CharC;

      DataIdx     = Sd->mOutBuf - DecodeP (Sd) - 1;

      BytesRemain--;
      while ((short) (BytesRemain) >= 0) {
        Sd->mDstBase[Sd->mOutBuf++] = Sd->mDstBase[DataIdx++];
        if (Sd->mOutBuf >= Sd->mOrigSize) {
          return ;
        }

        BytesRemain--;
      }
    }
  }

  return ;
}

UINT32
GetInfo (
        void
        )
/*++

  Gets size of scratch data

  @param VOID

  @retval size of scratch data

--*/
{
  return(sizeof (SCRATCH_DATA));
}


UINT32
Decompress (
           void    *Source,
           UINT32  SrcSize,
           void    *Destination,
           UINT32  DstSize,
           void    *Scratch,
           UINT32  ScratchSize,
           UINT8   Version
           )
/*++

  The internal implementation of *_DECOMPRESS_PROTOCOL.Decompress().

  @param Source      - The source buffer containing the compressed data.
  @param SrcSize     - The size of source buffer
  @param Destination - The destination buffer to store the decompressed data
  @param DstSize     - The size of destination buffer.
  @param Scratch     - The buffer used internally by the decompress routine. This  buffer is needed to store intermediate data.
  @param ScratchSize - The size of scratch buffer.
  @param Version     - The version of de/compression algorithm.
                       Version 1 for EFI 1.1 de/compression algorithm.
                       Version 2 for Tiano de/compression algorithm.

  @retval 0 - Success
  @retval Other - Failure error code

--*/
{
  UINT32        Index;
  UINT32        CompSize;
  UINT32        OrigSize;
  UINT32        Status;
  SCRATCH_DATA  *Sd;
  UINT8         *Src;
  UINT8         *Dst;

  Status  = 0;
  Src     = Source;
  Dst     = Destination;

  if (ScratchSize < sizeof (SCRATCH_DATA)) {
    return INSUFFICIENT_SCRATCH_SIZE;
  }

  Sd = (SCRATCH_DATA *) Scratch;

  if (SrcSize < 8) {
    return SRC_SIZE;
  }

  CompSize  = Src[0] + (Src[1] << 8) + (Src[2] << 16) + (Src[3] << 24);
  OrigSize  = Src[4] + (Src[5] << 8) + (Src[6] << 16) + (Src[7] << 24);

  //
  // If compressed file size is 0, return
  //
  if (OrigSize == 0) {
    return Status;
  }

  if (SrcSize < CompSize + 8) {
    return SRC_LESS_COMP;
  }

  if (DstSize != OrigSize) {
    return BUFFER_SIZE_MISMATCH;
  }

  Src = Src + 8;

  for (Index = 0; Index < sizeof (SCRATCH_DATA); Index++) {
    ((UINT8 *) Sd)[Index] = 0;
  }
  //
  // The length of the field 'Position Set Code Length Array Size' in Block Header.
  // For EFI 1.1 de/compression algorithm(Version 1), mPBit = 4
  // For Tiano de/compression algorithm(Version 2), mPBit = 5
  //
  switch (Version) {
  case 1:
    Sd->mPBit = 4;
    break;

  case 2:
    Sd->mPBit = 5;
    break;

  default:
    //
    // Currently, only have 2 versions
    //
    return UNKNOWN_VERSION;
  }

  Sd->mSrcBase  = Src;
  Sd->mDstBase  = Dst;
  Sd->mCompSize = CompSize;
  Sd->mOrigSize = OrigSize;

  //
  // Fill the first BITBUFSIZ bits
  //
  FillBuf (Sd, BITBUFSIZ);

  //
  // Decompress it
  //
  Decode (Sd);

  if (Sd->mBadTableFlag != 0) {
    //
    // Something wrong with the source
    //
    Status = BAD_TABLE_FLAG;
  }

  return Status;
}
