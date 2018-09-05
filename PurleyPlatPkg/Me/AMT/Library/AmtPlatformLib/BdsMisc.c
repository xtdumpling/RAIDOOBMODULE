/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2004 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file BdsMisc.c

  Misc BDS library function

**/
#if AMT_SUPPORT

#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Protocol/DevicePath.h>
#include <Guid/GlobalVariable.h>
#include "Library/BdsLib.h"
#include <Library/DebugLib.h>

extern UINT16 gPlatformBootTimeOutDefault;

/**

  Get the Option Number that does not used
  Try to locate the specific option variable one by one untile find a free number

  @param VariableName - Indicate if the boot#### or driver#### option

  @retval The Minimal Free Option Number

**/
UINT16
BdsLibGetFreeOptionNumber (
  IN  CHAR16    *VariableName
  )
{
  UINT16        Number;
  UINTN         Index;
  CHAR16        StrTemp[10];
  UINT16        *OptionBuffer;
  UINTN         OptionSize;

  //
  // Try to find the minimum free number from 0, 1, 2, 3....
  //
  Index = 0;
  do {
    if (*VariableName == 'B') {
      UnicodeSPrint (StrTemp, sizeof (StrTemp), L"Boot%04x", Index);
    } else {
      UnicodeSPrint (StrTemp, sizeof (StrTemp), L"Driver%04x", Index);
    }
    //
    // try if the option number is used
    //
    OptionBuffer = BdsLibGetVariableAndSize (
              StrTemp,
              &gEfiGlobalVariableGuid,
              &OptionSize
              );
    if (OptionBuffer == NULL) {
      break;
    }
    Index++;
  } while (1);

  Number = (UINT16) Index;
  return Number;
}

/**

  This function will register the new boot#### or driver#### option base on
  the VariableName. The new registered boot#### or driver#### will be linked
  to BdsOptionList and also update to the VariableName. After the boot#### or
  driver#### updated, the BootOrder or DriverOrder will also be updated.

  @param BdsOptionList    - The header of the boot#### or driver#### link list

  DevicePath       - The device path which the boot####
                     or driver#### option present

  String           - The description of the boot#### or driver####

  VariableName     - Indicate if the boot#### or driver#### option

  @retval EFI_SUCCESS      - The boot#### or driver#### have been success registered

  EFI_STATUS       - Return the status of gRT->SetVariable ().

**/
EFI_STATUS
EFIAPI
BdsLibRegisterNewOption (
  IN  LIST_ENTRY                     *BdsOptionList,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *String,
  IN  CHAR16                         *VariableName
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    MaxOptionNumber;
  UINT16                    RegisterOptionNumber;
  UINT16                    *TempOptionPtr;
  UINTN                     TempOptionSize;
  UINT16                    *OptionOrderPtr;
  VOID                      *OptionPtr;
  UINTN                     OptionSize;
  UINT8                     *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  CHAR16                    *Description;
  CHAR16                    OptionName[10];
  BOOLEAN                   UpdateDescription;
  UINT16                    BootOrderEntry;
  UINTN                     OrderItemNum;


  OptionPtr             = NULL;
  OptionSize            = 0;
  TempPtr               = NULL;
  OptionDevicePath      = NULL;
  Description           = NULL;
  MaxOptionNumber       = 0;
  OptionOrderPtr        = NULL;
  UpdateDescription     = FALSE;
  ZeroMem (OptionName, sizeof (OptionName));

  TempOptionSize = 0;
  TempOptionPtr = BdsLibGetVariableAndSize (
                    VariableName,
                    &gEfiGlobalVariableGuid,
                    &TempOptionSize
                    );
  if (TempOptionPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Compare with current option variable
  //
  for (Index = 0; Index < TempOptionSize / sizeof (UINT16); Index++) {

    if (*VariableName == 'B') {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", TempOptionPtr[Index]);
    } else {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Driver%04x", TempOptionPtr[Index]);
    }

    OptionPtr = BdsLibGetVariableAndSize (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  &OptionSize
                  );
    if (OptionPtr == NULL) {
      continue;
    }
    TempPtr = OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    //
    // Notes: the description may will change base on the GetStringToken
    //
    if (CompareMem (OptionDevicePath, DevicePath, GetDevicePathSize (OptionDevicePath)) == 0) {
      if (CompareMem (Description, String, StrSize (Description)) == 0) {
        //
        // Got the option, so just return
        //
        FreePool (OptionPtr);
        FreePool (TempOptionPtr);
        return EFI_SUCCESS;
      } else {
        //
        // Option description changed, need update.
        //
        UpdateDescription = TRUE;
        FreePool (OptionPtr);
        break;
      }
    }

    FreePool (OptionPtr);
  }

  OptionSize          = sizeof (UINT32) + sizeof (UINT16) + StrSize (String) + GetDevicePathSize (DevicePath);
  OptionPtr           = AllocateZeroPool (OptionSize);
  if (OptionPtr == NULL) {
    ASSERT(OptionPtr != NULL);
    return EFI_BUFFER_TOO_SMALL;
  }
  TempPtr             = OptionPtr;
  *(UINT32 *) TempPtr = LOAD_OPTION_ACTIVE;
  TempPtr += sizeof (UINT32);
  *(UINT16 *) TempPtr = (UINT16) GetDevicePathSize (DevicePath);
  TempPtr += sizeof (UINT16);
  CopyMem (TempPtr, String, StrSize (String));
  TempPtr += StrSize (String);
  CopyMem (TempPtr, DevicePath, GetDevicePathSize (DevicePath));

  if (UpdateDescription) {
    //
    // The number in option#### to be updated
    //
    RegisterOptionNumber = TempOptionPtr[Index];
  } else {
    //
    // The new option#### number
    //
    RegisterOptionNumber = BdsLibGetFreeOptionNumber(VariableName);
  }

  if (*VariableName == 'B') {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", RegisterOptionNumber);
  } else {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Driver%04x", RegisterOptionNumber);
  }

  Status = gRT->SetVariable (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  OptionSize,
                  OptionPtr
                  );
  //
  // Return if only need to update a changed description or fail to set option.
  //
  if (EFI_ERROR (Status) || UpdateDescription) {
    FreePool (OptionPtr);
    FreePool (TempOptionPtr);
    return Status;
  }

  FreePool (OptionPtr);

  //
  // Update the option order variable
  //

  //
  // If no option order
  //
  if (TempOptionSize == 0) {
    BootOrderEntry = 0;
    Status = gRT->SetVariable (
                    VariableName,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof (UINT16),
                    &BootOrderEntry
                    );
    if (EFI_ERROR (Status)) {
      FreePool (TempOptionPtr);
      return Status;
    }
    return EFI_SUCCESS;
  }
  //
  // Append the new option number to the original option order
  //
  OrderItemNum = (TempOptionSize / sizeof (UINT16)) + 1 ;
  OptionOrderPtr = AllocateZeroPool ( OrderItemNum * sizeof (UINT16));
  if (OptionOrderPtr == NULL) {
    ASSERT(OptionOrderPtr != NULL);
    FreePool (TempOptionPtr);
    return EFI_BUFFER_TOO_SMALL;
  }
  CopyMem (OptionOrderPtr, TempOptionPtr, (OrderItemNum - 1) * sizeof (UINT16));

  OptionOrderPtr[Index] = RegisterOptionNumber;

  Status = gRT->SetVariable (
                  VariableName,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  OrderItemNum * sizeof (UINT16),
                  OptionOrderPtr
                  );
  if (EFI_ERROR (Status)) {
    FreePool (TempOptionPtr);
    FreePool (OptionOrderPtr);
    return Status;
  }

  FreePool (TempOptionPtr);
  FreePool (OptionOrderPtr);

  return EFI_SUCCESS;
}


/**

  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

  @param Name       - String part of EFI variable name

  VendorGuid - GUID part of EFI variable name

  VariableSize - Returns the size of the EFI variable that was read

  @retval Dynamically allocated memory that contains a copy of the EFI variable.
  @retval Caller is responsible freeing the buffer.

  NULL - Variable was not read

**/
VOID *
EFIAPI
BdsLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  VOID        *Buffer;

  Buffer = NULL;

  //
  // Pass in a zero size buffer to find the required buffer size.
  //
  BufferSize  = 0;
  Status      = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //
    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      return NULL;
    }
    //
    // Read variable into the allocated buffer.
    //
    Status = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      BufferSize = 0;
    }
  }

  *VariableSize = BufferSize;
  return Buffer;
}

/**

  Function compares a device path data structure to that of all the nodes of a
  second device path instance.

  @param Multi        - A pointer to a multi-instance device path data structure.

  Single       - A pointer to a single-instance device path data structure.

  @retval TRUE   - If the Single is contained within Multi

  FALSE  - The Single is not match within Multi


**/
BOOLEAN
EFIAPI
BdsLibMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;

  if (!Multi || !Single) {
    return FALSE;
  }

  DevicePath      = Multi;
  DevicePathInst  = GetNextDevicePathInstance (&DevicePath, &Size);

  //
  // Search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst != NULL) {
    if (CompareMem (Single, DevicePathInst, Size) == 0) {
      FreePool (DevicePathInst);
      return TRUE;
    }

    FreePool (DevicePathInst);
    DevicePathInst = GetNextDevicePathInstance (&DevicePath, &Size);
  }

  return FALSE;
}

#endif // AMT_SUPPORT