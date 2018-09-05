/**
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights 
reserved This software and associated documentation (if any) is 
furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by 
such license, no part of this software or documentation may be 
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file UuidDxe.c
  
  Platform type driver to identify different platform.  

**/


#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/SetupVariable.h>
#include <Protocol/VariableWrite.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Removing IPMI feature code
//#include <Protocol/IpmiTransportProtocol.h>
//#include <Library/IpmiBaseLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Removing IPMI feature code

VOID                                   *mEfiVariableProtocolNotifyReg;
EFI_EVENT                              mEfiVariableProtocolEvent;
// APTIOV_SERVER_OVERRIDE_RC_START : Removing IPMI feature code
//EFI_IPMI_TRANSPORT                     *mIpmiTransport;
// APTIOV_SERVER_OVERRIDE_RC_END : Removing IPMI feature code

#define VarMfgUuid                     L"MfgSystemUuid"

#define EFI_SM_NETFN_INTEL_OEM         0x30
#define EFI_INTEL_OEM_SET_SYSTEM_GUID  0x41

extern EFI_GUID                        gEfiMfgUUIDHobGuid;

/**

  Get the system UUID from the UuidInfo Hob and send it to the BMC.
  
  @retval EFI_UNSUPPORTED       - Platform Info Protocol was not found.  UUID not sent to BMC
  @retval EFI_SUCCESS           - UUID was sent successfully to the BMC.

**/
// APTIOV_SERVER_OVERRIDE_RC_START : Removing IPMI feature code
#if 0
EFI_STATUS
SendUuidToBmc (
  VOID
  )
{

  EFI_STATUS              Status = EFI_SUCCESS;
  UINT8                   *SystemUuid=NULL;
  EFI_HOB_GUID_TYPE       *GuidHob;
  UINT8                   ResponseData;
  UINT8                   ResponseSize;

  GuidHob       = GetFirstGuidHob (&gEfiMfgUUIDHobGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  SystemUuid  = GET_GUID_HOB_DATA (GuidHob);

  Status = gBS->LocateProtocol (
             &gEfiIpmiTransportProtocolGuid,
             NULL,
             &mIpmiTransport
           );

  if ((Status == EFI_SUCCESS) && (mIpmiTransport != NULL)) {
    ResponseSize  = sizeof (ResponseData);
    Status = mIpmiTransport->SendIpmiCommand(
               mIpmiTransport,
               EFI_SM_NETFN_INTEL_OEM,
               0,
               EFI_INTEL_OEM_SET_SYSTEM_GUID,
               SystemUuid,
               16,
               (UINT8*) (&ResponseData),
               (UINT8*) &ResponseSize
             );

    if (Status != EFI_SUCCESS) {
      DEBUG((DEBUG_ERROR, "[IPMI] Set System GUID (Intel OEM Command 0x41) to BMC Failed.\n"));
    }
  }
  return Status;
}
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Removing IPMI feature code
VOID
EFIAPI
MFGUuidVariableDelCallback (
  IN  EFI_EVENT Event,
  IN  VOID      *Context
  )
{
  EFI_STATUS        Status;
  UINT8             Buffer[sizeof (EFI_GUID)];
  UINTN             SizeUUID;
  
  SizeUUID = sizeof (EFI_GUID);
  Status = gRT->GetVariable (
                  VarMfgUuid,
                  &gEfiSetupVariableGuid,
                  NULL,
                  &SizeUUID,
                  Buffer
                  );

  if(!EFI_ERROR (Status)){
  Status = gRT->SetVariable (
                      VarMfgUuid,
                      &gEfiSetupVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );
  }
}

/**

  Entry point for the driver.

  This routine check if MfgSystemUuid exists, if found, delete it to avoid duplicate write operation.

  @param ImageHandle  -  Image Handle.
  @param SystemTable  -  EFI System Table.
  
  @retval EFI_SUCCESS  -  Function has completed successfully.
  
**/
EFI_STATUS
EFIAPI
UuidDxeEntry (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
 
  EFI_STATUS                      Status;
  UINT8                           Buffer[sizeof (EFI_GUID)];
  VOID                        	  *VarArch;
  UINTN                           SizeUUID;
// APTIOV_SERVER_OVERRIDE_RC_START : Removing IPMI feature code
//  Status = SendUuidToBmc(); 
// APTIOV_SERVER_OVERRIDE_RC_END : Removing IPMI feature code

  Status = gBS->LocateProtocol (
                   &gEfiVariableWriteArchProtocolGuid,
                   NULL,
                   &VarArch
                   );
  if ( !EFI_ERROR (Status) ){
    SizeUUID = sizeof (EFI_GUID);
    Status = gRT->GetVariable (
                    VarMfgUuid,
                    &gEfiSetupVariableGuid,
                    NULL,
                    &SizeUUID,
                    Buffer
                    );

    if(!EFI_ERROR (Status)){
	   Status = gRT->SetVariable (
                        VarMfgUuid,
                        &gEfiSetupVariableGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        0,
                        NULL
                        );
     } 
  }else{
  
  //
  // Register to be notified when the variable write protocol has been produced.
  // Check for UUID in NV area.  If found or error, then delete it.
  //
    Status = gBS->CreateEvent (
                   EVT_NOTIFY_SIGNAL,
                   TPL_CALLBACK,
                   MFGUuidVariableDelCallback,
                   NULL,
                   &mEfiVariableProtocolEvent
                   );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (
                  &gEfiVariableWriteArchProtocolGuid,
                  mEfiVariableProtocolEvent,
                  &mEfiVariableProtocolNotifyReg
                  );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

