/** @file
  String support

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "String.h"

//*** AMI PORTING BEGIN ***//
#include <Library/MemoryAllocationLib.h>
// Bug fix: Memory leak.
// BdsLibGetStringById() returns a string pointer that has to be freed by the caller.
// The pointer to used to free last string returned by BdsLibGetStringById.
CHAR16 *BdsLibGetStrinByIdStringPointer = NULL;

VOID BdsLibFreeStringByIdBuffer(VOID){
  if (BdsLibGetStrinByIdStringPointer == NULL ) return;
  FreePool(BdsLibGetStrinByIdStringPointer);
  BdsLibGetStrinByIdStringPointer = NULL;
}
//*** AMI PORTING END *****//

/**
  Get string by string id from HII Interface


  @param Id              String ID.

  @retval  CHAR16 *  String from ID.
  @retval  NULL      If error occurs.

**/
CHAR16 *
BdsLibGetStringById (
  IN  EFI_STRING_ID   Id
  )
{
  //*** AMI PORTING BEGIN ***//
  // Bug fix: Memory leak.
  //return HiiGetString (gBdsLibStringPackHandle, Id, NULL);
  return BdsLibGetStrinByIdStringPointer = HiiGetString (gBdsLibStringPackHandle, Id, NULL);
  //*** AMI PORTING END *****//
}
