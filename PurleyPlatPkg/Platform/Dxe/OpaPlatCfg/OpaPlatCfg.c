/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file OpaPlatCfg.c

 
Revision History:

**/

#include "OpaPlatCfg.h"

typedef struct {
    UINT8 SocketNumber;
    EFI_GUID ConfigFileGuid;
} OPA_PLAT_CONFIG_FILELIST;

CHAR16                          N1vVarName[] = L"hfi1-socket0-configuration";
  
OPA_PLAT_CONFIG_FILELIST SocketConfigFilesMapping[] = {
    0, { 0x4FD1BC5E, 0x0A53, 0x4501, { 0xB9, 0x13, 0x56, 0xD3, 0x62, 0x98, 0x9E, 0x13 } }, //Socket 0
    1, { 0x2CAD98FC, 0x1897, 0x4837, { 0xB3, 0x13, 0xE6, 0xF0, 0x95, 0xF4, 0xF8, 0x4C } }  //Socket 1
};


/**

  Locate given guid in any of FV

  @param EFI_GUID - Guid to be located

  @retval UINTN - Size of guid located data or file
          EFI_FIRMWARE_VOLUME2_PROTOCOL * - Instance of FV where guid exist
**/

EFI_FIRMWARE_VOLUME2_PROTOCOL *
LocateFV2Handle(
  IN EFI_GUID FileGuid,
  OUT UINTN *Size
  ) 
{
  EFI_STATUS                    Status;
  UINTN                         Count;
  UINTN                         NumHandles;
  EFI_HANDLE                    *HandleBuffer;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVolInstance;
  UINT32                        FvStatus;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;

  FwVolInstance = NULL;

  Status = gBS->LocateHandleBuffer(
    ByProtocol,
    &gEfiFirmwareVolume2ProtocolGuid,
    NULL,
    &NumHandles,
    &HandleBuffer
    );
  ASSERT_EFI_ERROR(Status);

  for (Count = 0; Count < NumHandles; Count++) {
    Status = gBS->HandleProtocol(
      HandleBuffer[Count],
      &gEfiFirmwareVolume2ProtocolGuid,
      (VOID **)&FwVolInstance
      );
    ASSERT_EFI_ERROR(Status);

    if (FwVolInstance != NULL){
      ///
      /// Locate Config file and get Size of file
      ///
      Status = FwVolInstance->ReadFile(
        FwVolInstance,
        &FileGuid,
        NULL,
        Size,
        &FileType,
        &Attributes,
        &FvStatus
        );
      if (Status == EFI_SUCCESS) {
        return FwVolInstance;
      }
    } //endif (FwVolInstance != NULL)
  } //endfor
  return FwVolInstance;
}


/**
  Form Variable name string and Create NV variable for OPA platform config file

  @param VOID * - File buffer pointer containing OPA platform config file
         UINTN  - Size of File buffer 
		 UINTN  - CPU socket number to which platform config file associated with

  @retval VOID

**/
VOID  
CreatePlatCfgNVVariable(
  VOID *FileBuffer,
  UINTN FileSize
  )
{
  EFI_STATUS                      Status;

  Status = gRT->SetVariable(
    N1vVarName,
    &gOPAPlatCfgNvVarGuid,
    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
    FileSize,
    FileBuffer
    );

  DEBUG((EFI_D_INFO, "OPA Platform config NV Variable = %s, Status = %r\n", N1vVarName, Status));

  //
  // String format L"hfi1-socketN-configuration"; Where N= 0, 1, 2, ....
  // Create Next VariableName
  //
  N1vVarName[11]++;
}

/**
  Driver Entry point acts as primary function to locate OPA platform config files from FV 
  and create NV varaible with platform config data  

  @param EFI_HANDLE
         EFI_SYSTEM_TABLE
         

  @retval VOID

**/
EFI_STATUS
EFIAPI
OpaPlatCfgEntryPoint(
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
    EFI_STATUS                    Status;
    UINTN                         Count;
    UINTN                         Size;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVolInstance;
    VOID                          *ConfigFileBuffer;
    //
    // Variables to SetVariable Parameter
    //
    EFI_FV_FILETYPE               FileType;
    EFI_FV_FILE_ATTRIBUTES        Attributes;
    UINT32                        FvStatus;

    //
    // Assign Default Values
    //
    FwVolInstance = NULL;
    ConfigFileBuffer = NULL;

    DEBUG((EFI_D_INFO, "OPA Platform config Data file Handle driver\n"));
    //
    // Loop through each Platform config file entry
    //
    for (Count = 0; Count < sizeof(SocketConfigFilesMapping) / sizeof(OPA_PLAT_CONFIG_FILELIST); Count++){

      //
      // get FWVolProtocol instance
      //
      FwVolInstance = LocateFV2Handle(SocketConfigFilesMapping[Count].ConfigFileGuid, &Size);
      if (FwVolInstance == NULL) {
        //
        // Config file does not present in any FV volume. continue to next config file
        //
        continue;
      }
    
      Status = gBS->AllocatePool(EfiBootServicesData, Size, (VOID **)&ConfigFileBuffer);

      //
      // Read Config file to memory buffer
      //
      Status = FwVolInstance->ReadFile(
        FwVolInstance,
        &SocketConfigFilesMapping[Count].ConfigFileGuid,
        ((VOID **)&ConfigFileBuffer),
        &Size,
        &FileType,
        &Attributes,
        &FvStatus
        );

      //
      // Truncate 4 bytes from buffer before sending 
      // 4 bytes are header and not part of config file. 
      //
      if (!EFI_ERROR(Status) && (Size > 4)) {
        CreatePlatCfgNVVariable((UINT8 *)ConfigFileBuffer+4, Size-4);
      }

      if (ConfigFileBuffer != NULL){
        Status = gBS->FreePool(ConfigFileBuffer);
      }
    } //for
    return EFI_SUCCESS;
}
