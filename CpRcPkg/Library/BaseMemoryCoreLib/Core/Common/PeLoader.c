/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
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
 *
 ************************************************************************/

//**************************************************************************
//*                                                                        *
//*  PURPOSE:                                                              *
//*                                                                        *
//*  This file contains code to place a PE executable image at a user-     *
//*  supplied flat 32-bit address in system BIOS.                          *
//*                                                                        *
//**************************************************************************
// Standard Include Files

// Tool Include Files
#include    "pecoff.h"
#include    "SysFunc.h"

#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif

void fUpdateImageBase(UINT8 *baseAddr, UINT32 u32PeSignOffset, PEHDR *dsPeHdr)
{
  long  lOffset;

  lOffset = u32PeSignOffset + 4 + sizeof(COFFHDR) + (long)&dsPeHdr->u32ImgBase - (long)&dsPeHdr->u16MagicNum;
  //fseek(outfile, lOffset, SEEK_SET);
  //fwrite(&u32PlacementAddress, sizeof(UINT32), 1, outfile);
  *(UINT32 *)(baseAddr + lOffset) =  (UINT32)baseAddr;
}

void fApplyFixupDelta(UINT8 *baseAddr, UINT32 u32Delta, UINT32 u16Offset, UINT8 u8Type)
{
  UINT32 u32Data;

  //fseek(outfile, (long)u16Offset, SEEK_SET);
  //fread(&u32Data, sizeof(UINT32), 1, outfile);
  //fseek(outfile, (long)(0-sizeof(UINT32)), SEEK_CUR);
  u32Data = *(UINT32 *)(baseAddr + u16Offset);

  switch (u8Type) {
  case 0:
    break;
  case 1:
    u32Data += (u32Delta >> 16) & 0x0000ffff;
    break;
  case 2:
    u32Data += u32Delta & 0x0000ffff;
    break;
  case 3:
    u32Data += u32Delta;
    break;
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
  default:
    break;
  }

  //fwrite(&u32Data, sizeof(UINT32), 1, outfile);
  *(UINT32 *)(baseAddr + u16Offset) = u32Data;
}

void fProcessRelocSection(UINT8 *baseAddr, PEHDR *dsPeHdr, UINT32 u32RelocSectionOffset, UINT32 u32RelocSectionVirtSize)
{
  UINT32   i;
  UINT32   u32FixupDelta;
  UINT32   u32PageRva;
  UINT32   u32BlockSize;
  UINT16   u16TypeOffset;
  UINT8    u8Type;
  UINT16   u16Offset;
  UINT32   u32Size;
  UINT8 *curPtr;

  // Calculate the fixup delta.
  u32FixupDelta = (UINT32)(baseAddr - dsPeHdr->u32ImgBase);

  // Seek to the start of the .reloc section.
  //fseek(infile, (long)(u32RelocSectionOffset), SEEK_SET);
  curPtr = baseAddr + u32RelocSectionOffset;
  u32Size = u32RelocSectionVirtSize;

  // This seems to be a bug in the way MS generates the reloc fixup blocks.
  // After we have gone thru all the fixup blocks in the .reloc section, the
  // variable u32Size should ideally go to zero. But I have found some orphan
  // data after all the fixup blocks that don't quite fit anywhere. So, I have
  // changed the check to a greater-than-eight. It should be at least eight
  // because the PageRva and the BlockSize together take eight bytes. If less
  // than 8 are remaining, then those are the orphans and we need to disregard them.
  while (u32Size >= 8) {
    //rcPrintf ((host, "u32Size = %08x\n", u32Size));
    // Read the Page RVA and Block Size for the current fixup block.
    //fread(&u32PageRva, sizeof(UINT32), 1, infile);
    //fread(&u32BlockSize, sizeof(UINT32), 1, infile);

    u32PageRva = *(UINT32 *)curPtr;
    curPtr += sizeof(UINT32);
    u32BlockSize = *(UINT32 *)curPtr;
    curPtr += sizeof(UINT32);

    if (u32BlockSize == 0) {
      //rcPrintf ((host, "u32BlockSize is 0, breaking out of while()...\n"));
      break;
    }

    u32Size -= sizeof(UINT32) * 2;

    // Extract the correct number of Type/Offset entries. This is given by:
    // Loop count = Number of relocation items =
    // (Block Size - 4 bytes (Page RVA field) - 4 bytes (Block Size field)) divided
    // by 2 (each Type/Offset entry takes 2 bytes).
    //rcPrintf ((host, "LoopCount = %04x\n", ((u32BlockSize - 2 * sizeof(UINT32)) / sizeof(UINT16)));

    for (i = 0; i < ((u32BlockSize - 2 * sizeof(UINT32)) / sizeof(UINT16)); i++) {
      //fread(&u16TypeOffset, sizeof(UINT16), 1, infile);
      u16TypeOffset = *(UINT16 *)curPtr;
      curPtr += sizeof(UINT16);

      u8Type = (UINT8)((u16TypeOffset & 0xf000) >> 12);
      u16Offset = (UINT16)((UINT16)u16TypeOffset & 0x0fff);
      u32Size -= sizeof(UINT16);

      //rcPrintf ((host, "%04x: PageRva: %08lx Offset: %04x Type: %x \n", i, u32PageRva, u16Offset, u8Type ));

      fApplyFixupDelta(baseAddr, u32FixupDelta, (u32PageRva + (UINT32)u16Offset), u8Type);
    }
  }
}


void PlacePeImage(PSYSHOST host, UINT8 *baseAddr) {

  UINT32   u32PeSignOffset;
  UINT8   *curPtr;
  COFFHDR *dsCoffHdr;
  PEHDR   *dsPeHdr;
  SECTIONTBL *dsSectionTbl;
  UINT32 i;

  // --------------------------------------------------------
  // Get the PE Signature offset from the MS-DOS header.
  //fseek(infile, (long)0x3c, SEEK_SET);
  //fread(&u32PeSignOffset, sizeof(UINT32), 1, infile);
  //rcPrintf ((host, "u32PeSignOffset: %08x\n", u32PeSignOffset));
  u32PeSignOffset = *(UINT32 *)(baseAddr + 0x3C);

  // Seek to the COFF Header.
  //fseek(infile, (long)(u32PeSignOffset+4), SEEK_SET);
  // Read COFF Header from input file into dsCoffHdr.
  //fread(&dsCoffHdr, sizeof(COFFHDR), 1, infile);
  dsCoffHdr = (COFFHDR *)(baseAddr + u32PeSignOffset + 4);

  // Read PE Header from input file into dsPeHdr.
  //fread(&dsPeHdr, sizeof(PEHDR), 1, infile);
  dsPeHdr = (PEHDR *)(baseAddr + u32PeSignOffset + 4 + sizeof(COFFHDR));

  // Read as many Data Directories as are indicated in the PE Header u32NumDatDirs element.
  //for(i = 0; i < dsPeHdr->u32NumDatDirs; i++) {
  //   fread(&dsDatDirs[i], sizeof(DATADIR), 1, infile);
  //}

  // Read as many sections as are indicated by the COFF header.
  curPtr = (UINT8 *)dsPeHdr + sizeof(PEHDR) + (dsPeHdr->u32NumDatDirs * sizeof(DATADIR));
  for (i = 0; i < dsCoffHdr->u16NumSections; i++) {
    //fread(&dsSectionTbl, sizeof(SECTIONTBL), 1, infile);
    dsSectionTbl = (SECTIONTBL *)(curPtr + i * sizeof(SECTIONTBL));

    //fDumpSectionTable(&dsSectionTbl);

    if (StrCmpLocal(dsSectionTbl->cName, ".reloc") == 0) {
      // Store some crucial variables.
      //u32RelocSectionOffset = dsSectionTbl->u32RawDataPtr;
      //u32RelocSectionVirtSize = dsSectionTbl->u32VirtualSize;

      //  Got the .reloc section! Seek to the start of it.
      fProcessRelocSection(baseAddr, dsPeHdr, dsSectionTbl->u32RawDataPtr, dsSectionTbl->u32VirtualSize);
    }
  }

  // Finally, modify the image base field of the PE Header of the output file.
  fUpdateImageBase(baseAddr, u32PeSignOffset, dsPeHdr);

}

UINT32 GetPeEntrypoint(PSYSHOST host, UINT8 *baseAddr) {

  UINT32   u32PeSignOffset;
  PEHDR   *dsPeHdr;

  u32PeSignOffset = *(UINT32 *)(baseAddr + 0x3C);
  dsPeHdr = (PEHDR *)(baseAddr + u32PeSignOffset + 4 + sizeof(COFFHDR));

  return(dsPeHdr->u32EntryPoint + dsPeHdr->u32ImgBase);
}

