/** @file
  Firmware Status Smbios Interface implementation.

@copyright
 Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#if defined(AMT_SUPPORT) && AMT_SUPPORT

#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/Pci22.h>

#include <MeFwHob.h>
#include <FwStsSmbiosTable.h>
#include <Library/MmPciBaseLib.h>
#ifdef EFI_DEBUG
#include <Library/MeShowBufferLib.h>
#endif // EFI_DEBUG

/**
  Create the FWSTS info to the SMBIOS table using the SMBIOS protocol
  Invoke this routine to add the table entry when all the FWSTS data is finalized.

  - @pre
    - EFI_SMBIOS_PROTOCOL
  @param[in]  Event               - A pointer to the Event that triggered the callback.
                                    If the Event is not NULL, invoke DisableAllMeDevices () before exit
                                    because it is the event registered when failed to install HeciProtocol
                                    and locate SmbiosProtocol early
  @param[in]  Context             - A pointer to private data registered with the callback

  @retval EFI_SUCCESS             - if the data is successfully reported.
  @retval EFI_OUT_OF_RESOURCES    - if not able to get resources.
  @retval EFI_UNSUPPORTED         - if required DataHub or SMBIOS protocol is not available.
**/
VOID
EFIAPI
AddFwStsSmbiosEntry (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *Context
  )
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  FWSTS_SMBIOS_RECORD             *Record;
  UINT8                           Count;
  CHAR8                           *StringPtr;
  EFI_SMBIOS_PROTOCOL             *Smbios;
  ME_FW_HOB                       *MeFwHob;
  ME_FW_HOB                       *MeFwHobPool;
  UINT8                           Index;
  SMBIOS_TABLE_TYPE14             *SmbiosTableType14;
  static CHAR8                    T14String[sizeof (T14_FWSTS_STRING)] = T14_FWSTS_STRING;
  UINT32                          Function;
  UINTN                           DevicePciCfgBase;
  static MEI_DEV_STRING           DeviceList[] = {
    {HECI1_DEVICE, MEI1_FWSTS_STRING},
    {HECI2_DEVICE, MEI2_FWSTS_STRING},
    {HECI3_DEVICE, MEI3_FWSTS_STRING}
  };
  static UINT32                   FwStsOffsetTable[] = {
    R_ME_HFS,
    R_ME_GS_SHDW,
    R_ME_HFS_3,
    R_ME_HFS_4,
    R_ME_HFS_5,
    R_ME_HFS_6
  };

  DEBUG_CODE (
    DEBUG ((DEBUG_INFO, "AddFwStsSmbiosEntry () - Start\n"));
  );

  Record = NULL;
  SmbiosTableType14 = NULL;
  MeFwHobPool = NULL;

  ///
  /// gSiPkgTokenSpaceGuid.PcdFwStsSmbiosType determines the SMBIOS OEM type (0x80 to 0xFF) defined
  /// in SMBIOS, values 0-0x7F will be treated as disable FWSTS SMBIOS reporting. OEM structure type
  /// is dynamic type extracted from the "Type 14 - Group Associations Structure" to avoid conflicts
  /// introduced by hardcode OEM type.
  /// FWSTS structure uses it as SMBIOS OEM type to provide FWSTS information.
  ///

  if (PcdGet8(PcdFwStsSmbiosType) <= 0x7F) {
    DEBUG_CODE (
      DEBUG ((
        DEBUG_ERROR,
        "PcdFwStsSmbiosType value is 0x%x, it isn't defined as SMBIOS OEM type range(0x80 to 0xFF). FWSTS SMBIOS support is disabled now.\n",
        PcdGet8(PcdFwStsSmbiosType)
      ));
    );
    goto ErrExit;
  }

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
  if (Smbios == NULL) {
    goto ErrExit;
  }

  //
  // Get Mbp FW HOB
  //
  MeFwHob = GetFirstGuidHob (&gMeFwHobGuid);
  if (MeFwHob == NULL) {
    DEBUG_CODE (
      DEBUG ((DEBUG_ERROR, "MeFwHob is not available\n"));
    );
    MeFwHob = AllocateZeroPool (sizeof (ME_FW_HOB));
    if (MeFwHob != NULL) {
      for (Count = 0; Count < (sizeof (DeviceList)) / (sizeof (MEI_DEV_STRING)); Count++) {
        MeFwHob->Group[Count].FunNumber = DeviceList[Count].HeciDev;
      }
      MeFwHob->Count = Count;
      MeFwHobPool = MeFwHob;      // Free the temp pool before exit
    }
  }

  if (MeFwHob == NULL) {
    goto ErrExit;
  }

  ///
  /// Install SMBIOS Type14 - Group Associations Indicator structure
  /// Computed as 5 + (3 bytes for each item in the group).
  /// The user of this structure determines the number of items as (Length -5)/3.
  /// Add 1 bytes as table terminator.
  ///

  Size = sizeof (SMBIOS_TABLE_TYPE14) + AsciiStrSize (T14_FWSTS_STRING) + 1;

  SmbiosTableType14 = AllocateZeroPool (Size);
  if (SmbiosTableType14 == NULL) {
    goto ErrExit;
  }
  SmbiosTableType14->Hdr.Type = EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS;
  SmbiosTableType14->Hdr.Length = (UINT8) sizeof (SMBIOS_TABLE_TYPE14);
  SmbiosTableType14->Hdr.Handle = SMBIOS_HANDLE_PI_RESERVED;                // Assign an unused handle.
  SmbiosTableType14->GroupName  = 1;
  SmbiosTableType14->Group->ItemType = PcdGet8(PcdFwStsSmbiosType);
  StringPtr = ((CHAR8 *)SmbiosTableType14) + SmbiosTableType14->Hdr.Length;
  Status = AsciiStrCpyS (StringPtr, Size - SmbiosTableType14->Hdr.Length, T14String);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return;

#ifdef EFI_DEBUG
  DEBUG ((DEBUG_INFO, "SmbiosTableType14 data printed for FWSTS SMBIOS support -\n"));
  ShowBuffer ((UINT8 *)SmbiosTableType14, (UINT32) Size);
#endif // EFI_DEBUG

  Status = Smbios->Add (
             Smbios,
             NULL,
             &SmbiosTableType14->Hdr.Handle,
             (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType14
             );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    goto ErrExit;
  }

  ///
  /// Install SMBIOS Type (PcdFwStsSmbiosType)
  /// Create full FWSTS SMBIOS record.
  /// Adjust size to include variable length strings,
  /// and set string numbers to point to the correct string.
  /// Add 1 bytes as table terminator.
  ///

  Size = sizeof (FWSTS_SMBIOS_RECORD) + (sizeof (FWSTS_SET_RECORD) * (MeFwHob->Count - 1)) +
        (AsciiStrSize (MEI1_FWSTS_STRING) * MeFwHob->Count) + 1;

  Record = AllocateZeroPool (Size);
  if (Record == NULL) {
    goto ErrExit;
  }
  Record->SmbiosHeader.Type = PcdGet8(PcdFwStsSmbiosType);
  Record->SmbiosHeader.Length = sizeof (FWSTS_SMBIOS_RECORD) +
                                  (sizeof (FWSTS_SET_RECORD) * (MeFwHob->Count - 1));

  Record->SmbiosHeader.Handle = SMBIOS_HANDLE_PI_RESERVED;                  // Assign an unused handle.
  Record->Version = 1;
  Record->Count = MeFwHob->Count;

  ///
  /// Copy the data from MeFwHob and update current FWSTS if the device is not disabled.
  /// If the device is disabled, report the FWSTS saved by MeFwHob directly
  ///

  StringPtr = ((CHAR8 *)Record) + Record->SmbiosHeader.Length;

  for (Count = 0; Count < MeFwHob->Count; Count++) {
    Record->Group[Count].ComponentName = (Count + 1);
    Function = (HECI_DEVICE) MeFwHob->Group[Count].FunNumber;
    DevicePciCfgBase = MmPciBase (ME_BUS, ME_DEVICE_NUMBER, Function);

    if (MmioRead16 (DevicePciCfgBase + PCI_DEVICE_ID_OFFSET) != 0xFFFF) {
      //
      // Read current FWSTS and report to SMBIOS directly
      //
      for (Index = 0; Index < (sizeof (FwStsOffsetTable) / sizeof (UINT32)); Index++) {
        Record->Group[Count].Reg[Index] =
        MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, Function) + FwStsOffsetTable[Index]);
      }
    } else {
      //
      // The device is disabled, report the FWSTS saved by MeFwHob directly
      //
      CopyMem (
        &(Record->Group[Count].Reg),
        &(MeFwHob->Group[Count].Reg),
        (sizeof (FWSTS_SET_RECORD) - 1));
    }
#ifdef EFI_DEBUG
    ShowBuffer ((UINT8 *)&(Record->Group[Count]), sizeof (FWSTS_SET_RECORD));
#endif // EFI_DEBUG
    ///
    /// Fill in corresponding string context for componentName
    ///

    for (Index = 0; Index < (sizeof (DeviceList)) / (sizeof (MEI_DEV_STRING)); Index++) {
      if ((UINT32) DeviceList[Index].HeciDev == Function) {
        Status = AsciiStrCpyS (StringPtr, ((CHAR8*)Record + Size - StringPtr), DeviceList[Index].String);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) return;
        StringPtr += AsciiStrSize (DeviceList[Index].String);
        break;
      }
    }
  }

#ifdef EFI_DEBUG
  DEBUG ((DEBUG_INFO, "FWSTS SMBIOS table OEM type 0x%x data printed - \n", Record->SmbiosHeader.Type));
  ShowBuffer ((UINT8 *)Record, (UINT32) Size);
#endif // EFI_DEBUG
  Status = Smbios->Add (
            Smbios,
            NULL,
            &Record->SmbiosHeader.Handle,
            (EFI_SMBIOS_TABLE_HEADER *) Record
            );
  ASSERT_EFI_ERROR (Status);

ErrExit:
  if (Event != NULL) {
    DEBUG ((DEBUG_ERROR, "[HECI1] Removing ME devices from config space!\n"));
    DisableAllMeDevices ();
    gBS->CloseEvent (Event);
  }

  //
  // Free all temp pools
  //
  if (MeFwHobPool != NULL) {
    FreePool (MeFwHobPool);
  }
  if (Record != NULL) {
    FreePool (Record);
  }
  if (SmbiosTableType14 != NULL) {
    FreePool (SmbiosTableType14);
  }
}

#endif // defined(AMT_SUPPORT) && AMT_SUPPORT