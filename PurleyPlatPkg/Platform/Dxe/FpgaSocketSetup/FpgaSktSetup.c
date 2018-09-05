//
// Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
// This software and associated documentation (if any) is furnished
// under a license and may only be used or copied in accordance
// with the terms of the license. Except as permitted by such
// license, no part of this software or documentation may be
// reproduced, stored in a retrieval system, or transmitted in any
// form or by any means without the express written consent of
// Intel Corporation.
//

/**
  @file FpgaSktSetup.c

  FPGA Setup driver

 **/

#include "FpgaSktSetup.h"

extern UINT8                FpgaSktSetupFormsBin[];
extern EFI_GUID             gFpgaFormSetGuid;
EFI_HII_HANDLE              mFpgaHiiHandle = NULL;


#define FPGA_SETUP_SIGNATURE SIGNATURE_32 ('F', 'P', 'G', 'S')

typedef struct _INTEL_FPGA_SETUP_INSTANCE {
  UINTN                          Signature;
  EFI_HANDLE                     ImageHandle;
  EFI_HANDLE                     DeviceHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL ConfigAccess;
  VOID                           *StartOpCodeHandle;
  VOID                           *EndOpCodeHandle;
  EFI_HII_HANDLE                 HiiHandle;
  FPGA_SOCKET_CONFIGURATION      FpgaConfig;
} INTEL_FPGA_SETUP_INSTANCE;

INTEL_FPGA_SETUP_INSTANCE   mPrivateData;

typedef struct {
  VENDOR_DEVICE_PATH             Node;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

HII_VENDOR_DEVICE_PATH mHiiVendorDevicePath =
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        { (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
          (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8)
        }
      },
     FPGA_FORMSET_GUID
    },
    {
     END_DEVICE_PATH_TYPE,
     END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
      (UINT8)(END_DEVICE_PATH_LENGTH), (UINT8)((END_DEVICE_PATH_LENGTH) >> 8)
      }
    }
  };

/**
  Get FPGA BitStream ID

  @param SocketIndex  The index of socket

  @retval             The data of FPGA Bitstream ID

**/
UINT32
EFIAPI
GetFpgaBitstreamId (
  UINT8         SocketIndex
  )
{
  EFI_STATUS                Status;
  UINT8                     RCiEP0BusNumber;
  UINTN                     FMEBar;
  EFI_IIO_SYSTEM_PROTOCOL 	*IioSystemProtocol;
  IIO_GLOBALS               *IioGlobalData;

  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
  ASSERT_EFI_ERROR (Status);
  IioGlobalData = IioSystemProtocol->IioGlobalData;

  RCiEP0BusNumber = IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[SocketIndex][IIO_PSTACK3] - FPGA_PREAllOCATE_BUS_NUM + 0x01;
  FMEBar = (UINTN) IioPciExpressRead32 (IioGlobalData, (UINT8) SocketIndex, RCiEP0BusNumber, 0, 0, 0x10) & 0xFFFFFFF0;

  return MmioRead32 (FMEBar + BITSTREAM_ID_N1_FPGA_FME_REG);
}

/**
  Update the FPGA setup information

  @param Event       The Event need to be process
  @param Context     The context of this operation

  @retval None.

**/
VOID
EFIAPI
UpdateFpgaSocketDataEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                       Status;
  FPGA_CONFIGURATION               FpgaConfiguration;
  EFI_STRING                       NewString;
  EFI_STRING_ID                    TokenToUpdate;
  BITSTREAM_ID_N1_FPGA_FME_STRUCT  BitstreamId;

  DEBUG ((DEBUG_ERROR, "UpdateFpgaSocketDataEvent ()...\n"));

  //
  // Get the FPGA Configuration
  //
  Status = FpgaConfigurationGetValues (&FpgaConfiguration);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
  	return;
  }

  if (FpgaConfiguration.FpgaSktActive == 0) {
    return;
  }

  NewString = AllocateZeroPool (NEWSTRING_SIZE);
  ASSERT (NewString != NULL);
  if (NewString == NULL) {
    return;
  }

  if ((FpgaConfiguration.FpgaSktActive & 1) != 0) {
    BitstreamId.Data = GetFpgaBitstreamId (0);
    UnicodeSPrint (NewString, NEWSTRING_SIZE, L"%X.%X.%X.%X",
      BitstreamId.Bits.vermajor,
      BitstreamId.Bits.verminor,
      BitstreamId.Bits.verpatch,
      BitstreamId.Bits.verdebug);
    TokenToUpdate = (STRING_REF) STR_SOCKET0_BBS_VERSION_VALUE;
    HiiSetString (mFpgaHiiHandle, TokenToUpdate, NewString, NULL);
  }

  if ((FpgaConfiguration.FpgaSktActive & 2) != 0) {
    BitstreamId.Data = GetFpgaBitstreamId (1);
    UnicodeSPrint (NewString, NEWSTRING_SIZE, L"%X.%X.%X.%X",
      BitstreamId.Bits.vermajor,
      BitstreamId.Bits.verminor,
      BitstreamId.Bits.verpatch,
      BitstreamId.Bits.verdebug);
    TokenToUpdate = (STRING_REF) STR_SOCKET1_BBS_VERSION_VALUE;
    HiiSetString (mFpgaHiiHandle, TokenToUpdate, NewString, NULL);
  }

  gBS->FreePool (NewString);
}

/**
  Register event handler to update FPGA setup data records.

  @param None.

  @retval EFI_SUCCESS    The event handler was registered.
  @retval Others         Internal error when register event.

**/
EFI_STATUS
EFIAPI
UpdateFpgaSocketData (
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_EVENT               DummyEvent;
  VOID                    *mFpgaSocketDummyRegistration;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  UpdateFpgaSocketDataEvent,
                  NULL,
                  &DummyEvent
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Register protocol notifications on this event.
  //
  Status = gBS->RegisterProtocolNotify (
                  // APTIOV_SERVER_OVERRIDE_RC_START : Use GUID for TSE.
                  //&gSignalBeforeEnterSetupGuid,
                  &gEfiSetupEnterGuid,
                  // APTIOV_SERVER_OVERRIDE_RC_END : Use GUID for TSE.
                  DummyEvent,
                  &mFpgaSocketDummyRegistration
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
 Entry point of the SOCKET Embedded controller screen setup driver.

 @param ImageHandle                A handle for the image that is initializing this driver
 @param SystemTable                A pointer to the EFI system table

 @retval EFI_SUCCESS:              Driver initialized successfully
 @retval EFI_UNSUPPORTED           Driver has unsupported needs

**/
EFI_STATUS
EFIAPI
InstallFpgaSocketSetupScreen (
  IN EFI_HANDLE              ImageHandle,
  IN EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_IFR_GUID_LABEL        *StartLabel;
  EFI_IFR_GUID_LABEL        *EndLabel;
  BOOLEAN                   FpgaGlobalEnable = FALSE;

  //
  //  APTIOV_SERVER_OVERRIDE_RC_START : Getting resources for Socket Configuration
  //
  EFI_HII_PACKAGE_LIST_HEADER *HiiPackageList = NULL;
  EFI_HII_DATABASE_PROTOCOL *HiiDatabase = NULL;
  //
  //  Get HiiPackageList header resource from handle.
  //
  Status = gBS->HandleProtocol(ImageHandle, &gEfiHiiPackageListProtocolGuid, (VOID**)&HiiPackageList);
  ASSERT_EFI_ERROR (Status);
  //
  //  APTIOV_SERVER_OVERRIDE_RC_END : Getting resources for Socket Configuration
  //
  //
  // First see if FPGA is active
  //
  Status = FpgaGlobalEnableGet (FPGA_FLOW_STAGE_SETUP, &FpgaGlobalEnable);

  if (!FpgaGlobalEnable) {
    return EFI_UNSUPPORTED;
  }
  mPrivateData.Signature = FPGA_SETUP_SIGNATURE;
  mPrivateData.ImageHandle = ImageHandle;

  mPrivateData.DeviceHandle = NULL;

  Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
                                        &mPrivateData.DeviceHandle,
                                        &gEfiDevicePathProtocolGuid,
                                        &mHiiVendorDevicePath,
                                        NULL
                                        );
  ASSERT_EFI_ERROR (Status);

  //
  //  APTIOV_SERVER_OVERRIDE_RC_START : Getting resources for Socket Configuration
  //
  #if 0
  mPrivateData.HiiHandle = HiiAddPackages (
                             &gFpgaFormSetGuid,
                             mPrivateData.DeviceHandle,
                             FpgaSktSetupFormsBin,
                             FpgaSocketSetupStrings,
                             NULL
                             );
  #endif
  //
  //  Locate HiiDataBase protocol
  //
  Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&HiiDatabase);
  ASSERT_EFI_ERROR (Status);

  //
  // Register SocketSetup package list with the HII Database
  //
  Status = HiiDatabase->NewPackageList (
                     HiiDatabase, 
                     HiiPackageList, 
                     mPrivateData.DeviceHandle, 
                     &mPrivateData.HiiHandle
                     );
  ASSERT_EFI_ERROR (Status);
  //
  //  APTIOV_SERVER_OVERRIDE_RC_END : Getting resources for Socket Configuration
  //
  ASSERT (mPrivateData.HiiHandle != NULL);

  //
  // Initialize the container for dynamic opcodes
  //
  mPrivateData.StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (mPrivateData.StartOpCodeHandle != NULL);

  mPrivateData.EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (mPrivateData.EndOpCodeHandle != NULL);

  if ((mPrivateData.StartOpCodeHandle == NULL) || (mPrivateData.EndOpCodeHandle == NULL)) {
    if (mPrivateData.StartOpCodeHandle != NULL) {
      HiiFreeOpCodeHandle (mPrivateData.StartOpCodeHandle);
    }
    if (mPrivateData.EndOpCodeHandle != NULL) {
      HiiFreeOpCodeHandle (mPrivateData.EndOpCodeHandle);
    }
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Create Hii Extend Label OpCode as the Top opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                                        mPrivateData.StartOpCodeHandle,
                                        &gEfiIfrTianoGuid,
                                        NULL,
                                        sizeof (EFI_IFR_GUID_LABEL)
                                        );
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number = VFR_FORMLABLE_FPGA_SOCKET_TOP;

  //
  // Create Hii Extend Label OpCode as the Bottom opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                                      mPrivateData.EndOpCodeHandle,
                                      &gEfiIfrTianoGuid,
                                      NULL,
                                      sizeof (EFI_IFR_GUID_LABEL)
                                      );
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number = VFR_FORMLABLE_FPGA_SOCKET_BOTTOM;

  mFpgaHiiHandle = mPrivateData.HiiHandle;

  Status = UpdateFpgaSocketData ();
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
