/**@file

@copyright
 Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include "MeSmbios.h"
#include <Guid/EventGroup.h>
#include "Library/MeTypeLib.h"

extern UINT8 SmbiosMiscStrings[];

EFI_STATUS
UpdateMeSmbiosTable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
/*++
Description:

  Parses the mMiscSubclassDataTable structure and adds tables 82 and 83 to SMBIOS

Arguments:

  Event                   Pointer to the event that triggered this Callback Function
  Context                 VOID Pointer required for Callback function

Returns:

  EFI_SUCCESS             The data was successfully reported to the Data Hub

--*/
{
  EFI_SMBIOS_TABLE_HEADER       *RecordDataHeader;
  EFI_SMBIOS_TABLE_HEADER       *TempRecordHeader;
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_SMBIOS_PROTOCOL           *Smbios;
  EFI_SMBIOS_HANDLE             SmbiosHandle;



  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status         = EFI_SUCCESS;

  DEBUG((EFI_D_INFO, "UpdateMeSmbiosTable enter\n"));

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, &Smbios);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Could not locate Smbios protocol: %r\n", Status));
    return Status;
  } else if (Smbios == NULL) {
    DEBUG((EFI_D_ERROR, "LocateProtocol(Smbios) returned NULL pointer!\n"));
    return EFI_DEVICE_ERROR;
  }

  for (Index = 0; Index < mSmbiosMiscDataTableEntries; Index++) {
    //
    // Initialize SMBIOS Record pointer
    //
    RecordDataHeader         = (EFI_SMBIOS_TABLE_HEADER *) mMiscSubclassDataTable[Index].RecordData;
    RecordDataHeader->Handle = SMBIOS_HANDLE_PI_RESERVED;

    //
    // Remove the table from SMBIOS tables for updating a new one
    //
    do {
      Status = Smbios->GetNext (
                           Smbios,
                           &SmbiosHandle,
                           NULL,
                           &TempRecordHeader,
                           NULL
                           );      
      if (EFI_ERROR(Status)) {
        break;
      }

      if (TempRecordHeader->Type == RecordDataHeader->Type) {        
        Status = Smbios->Remove (
                             Smbios,
                             SmbiosHandle
                             );
        ASSERT_EFI_ERROR (Status);
        DEBUG ((DEBUG_INFO, "UpdateMeSmbiosTable() SMBIOS Type %d has been removed!!\n", RecordDataHeader->Type));
      }
    } while (Status == EFI_SUCCESS);

    if (mMiscSubclassDataTable[Index].Function != NULL) {
      //
      // The entry has a valid function pointer.
      //
      Status = (*mMiscSubclassDataTable[Index].Function) (
                            mMiscSubclassDataTable[Index].RecordType,
                            &mMiscSubclassDataTable[Index].RecordLen,
                            (SMBIOS_STRUCTURE *)RecordDataHeader
                            );
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "Type %d table function failed - %r\n", RecordDataHeader->Type, Status));
        return Status;
      }
    }

    Status = Smbios->Add (
                      Smbios,
                      NULL,
                      &RecordDataHeader->Handle,
                      RecordDataHeader
                      );
    if (!EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "UpdateMeSmbiosTable: The type %d table is installed into SMBIOS records\n", RecordDataHeader->Type));
    } else {
      DEBUG ((DEBUG_ERROR, "UpdateMeSmbiosTable: The type %d table is not installed into SMBIOS records - %r\n", 
        RecordDataHeader->Type, Status));
    }
  }

  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "UpdateMeSmbiosTable exit\n"));

  return Status;
}

EFI_STATUS 
EFIAPI
MeSmbiosDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++
Description:

  Standard EFI driver point. This driver registers  the notify function to reports any generated
  data to the DataHub.

Arguments:

  ImageHandle             Handle for the image of this driver
  SystemTable             Pointer to the EFI System Table

Returns:

  EFI_SUCCESS             The function completed successfully

--*/
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;
  

#if defined(SPS_SUPPORT) && SPS_SUPPORT
  if (!MeTypeIsAmt()) {
    return EFI_UNSUPPORTED;
  }
#endif //SPS_SUPPORT

  DEBUG((EFI_D_INFO, "MeSmbiosDriverEntryPoint entrypoint\n"));

  //
  // Register End of DXE event 
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UpdateMeSmbiosTable,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &Event
                  );

  ASSERT_EFI_ERROR (Status);

  return Status;
}
