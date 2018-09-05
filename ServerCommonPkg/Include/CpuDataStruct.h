//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.          

Module Name:

  CpuDataStruct.h

Abstract:

--*/

#ifndef _CPU_DATA_STRUCT_H
#define _CPU_DATA_STRUCT_H

typedef struct {
  UINT32  RegEax;
  UINT32  RegEbx;
  UINT32  RegEcx;
  UINT32  RegEdx;
} EFI_CPUID_REGISTER;

typedef struct {
  UINT32  HeaderVersion;
  UINT32  UpdateRevision;
  UINT32  Date;
  UINT32  ProcessorId;
  UINT32  Checksum;
  UINT32  LoaderRevision;
  UINT32  ProcessorFlags;
  UINT32  DataSize;
  UINT32  TotalSize;
  UINT8   Reserved[12];
} EFI_CPU_MICROCODE_HEADER;

typedef struct {
  UINT32  ExtendedSignatureCount;
  UINT32  ExtendedTableChecksum;
  UINT8   Reserved[12];
} EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER;

typedef struct {
  UINT32  ProcessorSignature;
  UINT32  ProcessorFlag;
  UINT32  ProcessorChecksum;
} EFI_CPU_MICROCODE_EXTENDED_TABLE;

typedef struct {
  UINT32  Stepping : 4;
  UINT32  Model : 4;
  UINT32  Family : 4;
  UINT32  Type : 2;
  UINT32  Reserved1 : 2;
  UINT32  ExtendedModel : 4;
  UINT32  ExtendedFamily : 8;
  UINT32  Reserved2 : 4;
} EFI_CPU_VERSION;

#endif
