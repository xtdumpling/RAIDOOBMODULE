//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
//**************************************************************************
//*                                                                        *
//*      Intel Restricted Secret                                           *
//*                                                                        *
//*                                                                        *
//*      ESS - Enterprise Silicon Software                                 *
//*                                                                        *
//*      Copyright (c) 2004 - 2014 Intel Corp.                             *
//*                                                                        *
//*                                                                        *
//*      This program has been developed by Intel Corporation.             *
//*      Licensee has Intel's permission to incorporate this source code   *
//*      into their product, royalty free.  This source code may NOT be    *
//*      redistributed to anyone without Intel's written permission.       *
//*                                                                        *
//*      Intel specifically disclaims all warranties, express or           *
//*      implied, and all liability, including consequential and other     *
//*      indirect damages, for the use of this code, including liability   *
//*      for infringement of any proprietary rights, and including the     *
//*      warranties of merchantability and fitness for a particular        *
//*      purpose.  Intel does not assume any responsibility for any        *
//*      errors which may appear in this code nor any responsibility to    *
//*      update it.                                                        *
//*                                                                        *
//**************************************************************************
//**************************************************************************
//*                                                                        *
//*  PURPOSE:                                                              *
//*                                                                        *
//*  This file contains declarations for the header structures of a        *
//*  PE executable image.                                                  *
//*                                                                        *
//**************************************************************************

#ifndef _PECOFF_H
#define _PECOFF_H

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
#ifdef __GNUC__
typedef unsigned int    UINT32;
typedef int             INT32;
#else
// Disable warning for benign redefinition of type
#pragma warning(disable : 4142)
typedef unsigned long   UINT32;
typedef long            INT32;
#endif

#pragma pack(push,1)

typedef struct {
  UINT16   u16Machine;
  UINT16   u16NumSections;
  UINT32   u32DateTimeStamp;
  UINT32   u32SymTblPtr;
  UINT32   u32NumSymbols;
  UINT16   u16PeHdrSize;
  UINT16   u16Characs;
} COFFHDR, *PCOFFHDR;

typedef struct {
  UINT16   u16MagicNum;
  UINT8    u8LinkerMajVer;
  UINT8    u8LinkerMinVer;
  UINT32   u32CodeSize;
  UINT32   u32IDataSize;
  UINT32   u32UDataSize;
  UINT32   u32EntryPoint;
  UINT32   u32CodeBase;
  UINT32   u32DataBase;
  UINT32   u32ImgBase;
  UINT32   u32SectionAlignment;
  UINT32   u32FileAlignment;
  UINT16   u16OSMajVer;
  UINT16   u16OSMinVer;
  UINT16   u16ImgMajVer;
  UINT16   u16ImgMinVer;
  UINT16   u16SubMajVer;
  UINT16   u16SubMinVer;
  UINT32   u32Rsvd;
  UINT32   u32ImgSize;
  UINT32   u32HdrSize;
  UINT32   u32Chksum;
  UINT16   u16Subsystem;
  UINT16   u16DLLChars;
  UINT32   u32StkRsrvSize;
  UINT32   u32StkCmmtSize;
  UINT32   u32HeapRsrvSize;
  UINT32   u32HeapCmmtSize;
  UINT32   u32LdrFlags;
  UINT32   u32NumDatDirs;
} PEHDR, *PPEHDR;

typedef struct {
  UINT32   u32Rva;
  UINT32   u32DataDirSize;
} DATADIR, *PDATADIR;

typedef struct {
  char  cName[8];
  UINT32   u32VirtualSize;
  UINT32   u32VirtualAddress;
  UINT32   u32RawDataSize;
  UINT32   u32RawDataPtr;
  UINT32   u32RelocPtr;
  UINT32   u32LineNumPtr;
  UINT16   u16NumRelocs;
  UINT16   u16NumLineNums;
  UINT32   u32Characs;
} SECTIONTBL, *PSECTIONTBL;

#pragma pack(pop)

#endif   // _PECOFF_H
