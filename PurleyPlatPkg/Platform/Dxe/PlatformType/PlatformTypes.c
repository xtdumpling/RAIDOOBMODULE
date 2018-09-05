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


  @file PlatformTypes.c
  
  Platform type driver to identify different platform.  

**/

//
// Statements that include other files
//
#include "PlatformTypes.h"
#include <Library/SerialPortLib/Ns16550.h>
#include <Library/IoLib.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/MpService.h>
#include <Library/SetupLib.h>

#define STRING_WIDTH_40    40

CHAR16                              PchName[40];
CHAR16                              IioName[40];
CHAR16                              *PlatformName;

//
// Instantiation of Driver's private data.
//
EFI_PLATFORM_DATA_DRIVER_PRIVATE    mPrivatePlatformData;
EFI_IIO_UDS_DRIVER_PRIVATE          mIioUdsPrivateData;
IIO_UDS                             *IioUdsData;          // Pointer to UDS in Allocated Memory Pool

SYSTEM_BOARD_PROTOCOL       *mSystemBoard;
EFI_GUID gEfiSmbusHcProtocolGuid = EFI_SMBUS_HC_PROTOCOL_GUID;
EFI_GUID gEfiMpServiceProtocolGuid = EFI_MP_SERVICES_PROTOCOL_GUID;
EFI_GUID mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;


/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
GetPchName (
  VOID
  )
{

  switch (mPrivatePlatformData.PlatformType.PchSku) {
  default:
    StrCpyS (PchName, STRING_WIDTH_40, L"Lewisburg");
  }

  //
  // Type
  //
  switch (mPrivatePlatformData.PlatformType.PchType) {
    default:
      StrCatS (PchName, STRING_WIDTH_40, L" SuperSku");
      break;
  }
  

  //
  // Revision
  //
  switch (mPrivatePlatformData.PlatformType.PchRevision) {
    default:
      StrCatS (PchName, STRING_WIDTH_40, L" - xx");
  }

  mPrivatePlatformData.PlatformType.PchStringPtr = (UINT64)PchName;
  return;
}


/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
GetIioName (
  VOID
  )
{

  StrCpyS(IioName, STRING_WIDTH_40, L"Unknown");

  mPrivatePlatformData.PlatformType.IioStringPtr = (UINT64)IioName;
  return ;
}


/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
SetAllMtrrsValid (
  VOID
)
{
  UINT32  Index;
  UINT64  MtrrValue;
  UINT32  VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64(EFI_IA32_MTRR_CAP) & B_EFI_IA32_MTRR_VARIABLE_SUPPORT);

  //
  // Check all Mtrrs to see if it is valid
  //
  for (Index = EFI_CACHE_VARIABLE_MTRR_BASE + 1; Index <= (EFI_CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit*2 - 1); Index+=2) {
    if (AsmReadMsr64 (Index - 1) == 0xFE000000) {
      AsmWriteMsr64 (Index - 1, 0);
      AsmWriteMsr64 (Index    , 0);
    } else {
      MtrrValue = AsmReadMsr64 (Index);
      if (MtrrValue && !(MtrrValue & B_EFI_CACHE_MTRR_VALID)) {
        AsmWriteMsr64 (Index, MtrrValue | B_EFI_CACHE_MTRR_VALID);
      }
    }
  } 
}

/**

  Initialize Serial Port for OS boot

    The Baud Rate Divisor registers are programmed and the LCR
    is used to configure the communications format.

  @param None

  @retval None

**/
VOID
EFIAPI
SerialPortInitializeForOS (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  
  UINTN                   Divisor;
  UINT8                   OutputData;
  UINT8                   Data;
  UINT16                  ComBase  = 0x3f8;
  UINTN                   Bps      = 115200;
  UINT8                   DataBits = 8;
  UINT8                   Stop     = 1;
  UINT8                   Parity   = 0;
  UINT8                   BreakSet = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINT32                  SerialBaudRate;
  

  //
  // Get Baud Rate from Variable
  //
  Status = GetOptionData (&gEfiSetupVariableGuid,  OFFSET_OF(SYSTEM_CONFIGURATION, SerialBaudRate), &SerialBaudRate, sizeof(UINT32));
 
  if(!EFI_ERROR (Status)){
    Bps = SerialBaudRate;
  }
  
  //InitializeSio();
  //
  // Some init is done by the platform status code initialization.
  //
  //
  // Map 5..8 to 0..3
  //
    
  Data = (UINT8) (DataBits - (UINT8) 5);

  //
  // Calculate divisor for baud generator
  //
  Divisor = 115200 / Bps;

  //
  // Set communications format
  //
  OutputData = (UINT8) ((DLAB << 7) | ((BreakSet << 6) | ((Parity << 3) | ((Stop << 2) | Data))));
  IoWrite8 (ComBase + LCR_OFFSET, OutputData);

  //
  // Configure baud rate
  //
  IoWrite8 (ComBase + BAUD_HIGH_OFFSET, (UINT8) (Divisor >> 8));
  IoWrite8 (ComBase + BAUD_LOW_OFFSET, (UINT8) (Divisor & 0xff));

  //
  // Switch back to bank 0
  //
  OutputData = (UINT8) ((~DLAB << 7) | ((BreakSet << 6) | ((Parity << 3) | ((Stop << 2) | Data))));
  IoWrite8 (ComBase + LCR_OFFSET, OutputData);

  return;
}  

/**

  Assert the POST complete GPIO
  
  @param Event         Pointer to the event that triggered this Ccllback Function
  @param Context       VOID Pointer required for Ccllback functio

  @retval EFI_SUCCESS         - Assertion successfully


**/
VOID
AssertPostGpio (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
 // APTIOV_SERVER_OVERRIDE_RC_START : Check point does not display in Shell and OS
  //mSystemBoard->SystemAssertPostGpio();
 // APTIOV_SERVER_OVERRIDE_RC_END : Check point does not display in Shell and OS
  return;
}

/**

  Gets the CpuId and fills in the pointer with the value.
  Needed for executing CpuId on other APs.

  @param RegEax - Pointer to be used to pass the CpuId value

  @retval None

**/
VOID
EFIAPI
PlatformGetProcessorID (
  IN OUT UINT32  *RegEax
  )
{

  AsmCpuid (EFI_CPUID_VERSION_INFO, RegEax, NULL, NULL, NULL);
}

//
// Define platform type check
//
/**

  Entry point for the driver.

  This routine reads the PlatformType GPI on FWH and produces a protocol 
  to be consumed by the chipset driver to effect those settings.

  @param ImageHandle  -  Image Handle.
  @param SystemTable  -  EFI System Table.
  
  @retval EFI_SUCCESS  -  Function has completed successfully.
  
**/
EFI_STATUS
EFIAPI
PlatformTypeInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_HOB_GUID_TYPE               *GuidHob;
  EFI_PLATFORM_INFO               *PlatformInfoHobData = NULL;
  IIO_UDS                         *UdsHobPtr;
  EFI_GUID                        UniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;
  EFI_EVENT   					  ReadyToBootEvent;

  //
  // Initialize driver private data.
  // Only one instance exists
  //
  ZeroMem (&mPrivatePlatformData, sizeof (mPrivatePlatformData));
  mPrivatePlatformData.Signature            = EFI_PLATFORM_TYPE_DRIVER_PRIVATE_SIGNATURE;


  PlatformName = AllocateZeroPool ((40) * sizeof (CHAR16));
  ASSERT (PlatformName != NULL);
  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfoHobData  = GET_GUID_HOB_DATA(GuidHob); 

  mPrivatePlatformData.PlatformType.BoardId     = PlatformInfoHobData->BoardId;
  mPrivatePlatformData.PlatformType.Type        = mPrivatePlatformData.PlatformType.BoardId;
  mPrivatePlatformData.PlatformType.IioSku      = PlatformInfoHobData->IioSku;
  mPrivatePlatformData.PlatformType.IioRevision = PlatformInfoHobData->IioRevision;
  mPrivatePlatformData.PlatformType.PchSku      = PlatformInfoHobData->PchSku;
  mPrivatePlatformData.PlatformType.PchRevision = PlatformInfoHobData->PchRevision;
  mPrivatePlatformData.PlatformType.PchType     = PlatformInfoHobData->PchType;  //Include PCH SKU type
  mPrivatePlatformData.PlatformType.CpuType     = PlatformInfoHobData->CpuType;
  mPrivatePlatformData.PlatformType.CpuStepping = PlatformInfoHobData->CpuStepping;
  mPrivatePlatformData.PlatformType.PchData.PchPciePortCfg1 = PlatformInfoHobData->PchData.PchPciePortCfg1;
  mPrivatePlatformData.PlatformType.PchData.PchPciePortCfg2 = PlatformInfoHobData->PchData.PchPciePortCfg2;
  mPrivatePlatformData.PlatformType.PchData.PchPciePortCfg3 = PlatformInfoHobData->PchData.PchPciePortCfg3;
  mPrivatePlatformData.PlatformType.PchData.PchPciePortCfg4 = PlatformInfoHobData->PchData.PchPciePortCfg4;
  mPrivatePlatformData.PlatformType.PchData.PchPciePortCfg5 = PlatformInfoHobData->PchData.PchPciePortCfg5;
  mPrivatePlatformData.PlatformType.PchData.PchPcieSBDE     = PlatformInfoHobData->PchData.PchPcieSBDE;
  mPrivatePlatformData.PlatformType.PchData.LomLanSupported = PlatformInfoHobData->PchData.LomLanSupported;
  mPrivatePlatformData.PlatformType.PchData.GbePciePortNum  = PlatformInfoHobData->PchData.GbePciePortNum;
  mPrivatePlatformData.PlatformType.PchData.GbeRegionInvalid  = PlatformInfoHobData->PchData.GbeRegionInvalid;
  mPrivatePlatformData.PlatformType.PchData.GbeEnabled      = PlatformInfoHobData->PchData.GbeEnabled;
  mPrivatePlatformData.PlatformType.PchData.PchStepping     = PlatformInfoHobData->PchData.PchStepping;

  //
  // Locate Dxe System Board Protocol
  //

  Status = gBS->LocateProtocol (&gEfiDxeSystemBoardProtocolGuid, NULL, (VOID**)&mSystemBoard); 
  ASSERT_EFI_ERROR (Status); 

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Cannot locate Dxe System Board protocol\n"));
    return Status;
  }
  mSystemBoard->InitPlatformName(PlatformName);
  mSystemBoard->UpdateMcuInfo();

  mPrivatePlatformData.PlatformType.TypeStringPtr = (UINT64)PlatformName;

  GetPchName();
  GetIioName();

  //
  // Install the PlatformType policy.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mPrivatePlatformData.Handle,
                  &gEfiPlatformTypeProtocolGuid,
                  &mPrivatePlatformData.PlatformType,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_ERROR, "%s platform is detected!\n", PlatformName));

  //
  // Time to get the IIO_UDS HOB data stored in the PEI driver
  //
  GuidHob    = GetFirstGuidHob (&UniversalDataGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  UdsHobPtr = GET_GUID_HOB_DATA(GuidHob); 

  //
  // Allocate Memory Pool for Universal Data Storage so that protocol can expose it
  //
  Status = gBS->AllocatePool ( EfiReservedMemoryType, sizeof (IIO_UDS), (VOID **) &IioUdsData );
  ASSERT_EFI_ERROR (Status);
  
  //
  // Initialize the Pool Memory with the data from the Hand-Off-Block
  //
  CopyMem(IioUdsData, UdsHobPtr, sizeof(IIO_UDS));

  //
  // Build the IIO_UDS driver instance for protocol publishing  
  //
  ZeroMem (&mIioUdsPrivateData, sizeof (mIioUdsPrivateData));
    
  mIioUdsPrivateData.Signature            = EFI_IIO_UDS_DRIVER_PRIVATE_SIGNATURE;
  mIioUdsPrivateData.IioUds.IioUdsPtr     = IioUdsData;
  mIioUdsPrivateData.IioUds.EnableVc      = NULL;

  //
  // Install the IioUds Protocol.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mIioUdsPrivateData.Handle,
                  &gEfiIioUdsProtocolGuid,
                  &mIioUdsPrivateData.IioUds,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);  


  //
  // Set up callback to assert the POST Complete GPIO to the iBMC
  //
  Status = EfiCreateEventReadyToBootEx(
             TPL_NOTIFY,
             AssertPostGpio,
             NULL,
             &ReadyToBootEvent
             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

