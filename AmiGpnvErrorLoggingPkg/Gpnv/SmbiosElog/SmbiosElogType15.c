//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SmbiosElogType15.c
    SmbiosElog type 15 Functions Implementation

**/

//---------------------------------------------------------------------------

#include "SmbiosElog.h"

//---------------------------------------------------------------------------

AMI_SMBIOS_PROTOCOL *SmbiosProtocolPtr = NULL;

/**

    Adds the Type 15 Structure to the DataHub, which will later be retrieved 
    to add to  the SMBIOS Table.

    @param Event - EFI_EVENT_NOTIFY
    @param Context - The Event relative context

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/

EFI_STATUS
AddSMBIOSType15Structure (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{

  EFI_STATUS  Status;
  UINT8       LocalBuffer[sizeof(EFI_SYSTEM_EVENT_LOG_TYPE_15) + sizeof(SupportedEventLogTypesList) + 0x10];


// Check whether Log area is full or not. If it is full then set 
// "Log Area Full" bit (BIT1).
// Send the EventSize parameter in CheckIfFull function with  max size 
// possible. Currently OEM events has max size so sending this size.

  if (CheckIfFull(mRedirSmbiosPrivate, sizeof(MULTIPLE_EVENT_OEM_STATUS_CODE_DATA))) {
     SystemEventLogType15Table.LogStatus |= SMBIOS_EVENT_LOG_FULL_BIT;
  }

// Update the Log Change Token.

  SystemEventLogType15Table.LogChangeToken = mRedirSmbiosPrivate->RecordId;


// Locate DataHub and Log Smbios Data

  Status = gBS->LocateProtocol (
                                  &gAmiSmbiosProtocolGuid,
                                  NULL,
                                  (VOID **)&SmbiosProtocolPtr
                               );

  if(EFI_ERROR(Status)) {
      return Status;
  }


// Put "SystemEventLogType15Table" at beginning of information.

  gBS->CopyMem( &(LocalBuffer[0]),
                 (UINT8*)(&SystemEventLogType15Table),
                 sizeof(EFI_SYSTEM_EVENT_LOG_TYPE_15) 
              );
   
// Followed immediately by the supported event types to complete the Type 15 Table.

  gBS->CopyMem( &(LocalBuffer[0]) + sizeof(EFI_SYSTEM_EVENT_LOG_TYPE_15),
                 (UINT8*)(&SupportedEventLogTypesList[0]),
                 sizeof(SupportedEventLogTypesList) 
              );

  gBS->SetMem(&(LocalBuffer[0]) + sizeof(EFI_SYSTEM_EVENT_LOG_TYPE_15) + sizeof(SupportedEventLogTypesList), 0x10, 0);
  
  if (SmbiosProtocolPtr != NULL) {
    Status = SmbiosProtocolPtr->SmbiosAddStructure( &(LocalBuffer[0]), 
                                                    sizeof(EFI_SYSTEM_EVENT_LOG_TYPE_15) + sizeof(SupportedEventLogTypesList) + 0x02 
                                                  );
    if(EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR,"Add Smbios Type 15 Structure is failed\n"));
    }
  }

  return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
