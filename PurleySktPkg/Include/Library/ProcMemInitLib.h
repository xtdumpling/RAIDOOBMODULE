/** @file
Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PROC_MEMINIT_LIB_H__
#define __PROC_MEMINIT_LIB_H__

// Pull the MRC header files here
#ifdef IA32
#include "ProcMemInit.h"

EFI_STATUS
EFIAPI
PeimProcMemInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );
#else
#include "SysFunc.h"
#endif

#endif // __PROC_MEMINIT_LIB_H__
