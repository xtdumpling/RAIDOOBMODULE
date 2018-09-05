/**
Copyright (c) 2007 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file LastBootErrorLog.h

---------------------------------------------------------------------*/
#include "LastBootErrorLog.h"
#include <Guid/LastBootErrorLogHob.h>

EFI_WHEA_BOOT_PROTOCOL  *mWheaBootProtocol = NULL;
EFI_MEM_RAS_PROTOCOL    *mMemRas = NULL;
EFI_SMBIOS_PROTOCOL     *mSmbiosProtocol = NULL;
IIO_UDS                 *mIioUds = NULL;
EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;
 
static PREV_BOOT_ERR_GLOBAL_DATA mPrevBootErrData;

/*
  Initilizes the global data variables used in the driver

@param I/P and O/P : Updates the Previous boot error sorce pointer if gPrevBootErrSrcHobGuid HOB is located properly. 
                Else returns with NULL
  
Returns:    none;
      

*/
VOID
InitGlobalData(

  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_IIO_UDS_PROTOCOL        *iioUdsProtocol;
  EFI_IIO_SYSTEM_PROTOCOL     *mIioSystem;
  UINT8						  PciPort;
  UINT8						  RootBridgeLoop;

  // Initialize with null pointer

  // Locate the WheaBootprotocol.
  Status = gBS->LocateProtocol (
          &gEfiWheaBootProtocolGuid,
          NULL,
          &mWheaBootProtocol
          );
  if(EFI_ERROR(Status)) return;

  Status = mWheaBootProtocol->WheaBootStartElog(mWheaBootProtocol);
  if(EFI_ERROR(Status)) return;

  Status = gSmst->SmmLocateProtocol (&gEfiMemRasProtocolGuid, NULL, &mMemRas);
  if(EFI_ERROR(Status)) return;

  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid,NULL,&iioUdsProtocol);
  if(EFI_ERROR(Status)) return;
  mIioUds = iioUdsProtocol->IioUdsPtr;

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  if (EFI_ERROR(Status))return ;

	Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &mIioSystem);
  if(EFI_ERROR(Status)) return;

  mPrevBootErrData.SetupMcBankStsClear  = TRUE;
  mPrevBootErrData.PciexErrFound        = FALSE;

	//
	// Init legacy IohInfo structure 
	//
  for ( RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {

    if (mIioUds->PlatformData.IIO_resource[RootBridgeLoop].Valid == 1) {
      IohInfo[RootBridgeLoop].Valid = 1;
      IohInfo[RootBridgeLoop].SocketId = mIioUds->PlatformData.IIO_resource[RootBridgeLoop].SocketID;
      IohInfo[RootBridgeLoop].BusNum  = mIioUds->PlatformData.IIO_resource[RootBridgeLoop].BusBase;
      IohInfo[RootBridgeLoop].Core20BusNum = mIioUds->PlatformData.IIO_resource[RootBridgeLoop].BusBase;
      
      for(PciPort = 0; PciPort < NUMBER_PORTS_PER_SOCKET; PciPort++) {
        IohInfo[RootBridgeLoop].Port[PciPort].Bus = mIioSystem->IioGlobalData->IioVar.IioVData.SocketPortBusNumber[RootBridgeLoop][PciPort];
        IohInfo[RootBridgeLoop].Port[PciPort].Device = mIioSystem->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PciPort].Device;
        IohInfo[RootBridgeLoop].Port[PciPort].Function = mIioSystem->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PciPort].Function;
      }
    }
  }

}

//
// Driver Entry Point
//
/**

  Entry point of the Function. 
  This function collects previous boot errors if any of presents. This function is currentlyimplemented for McBank errors.

  @param ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  @param SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table        

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
LastBootErrorEntry(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
// APTIOV_SERVER_OVERRIDE_RC_START : Added to skip LastBoot Error Handler for cold boot.
  EFI_STATUS							Status;
  struct SystemMemoryMapHob             	*systemMemoryMap;
  EFI_HOB_GUID_TYPE                     		*GuidHob;
  // APTIOV_SERVER_OVERRIDE_RC_END : Added to skip LastBoot Error Handler for cold boot.
  // Initilizes the global data , prtocols and the mPrevBootErrSrcHob hoib used in for updating whea boot error records.
  InitGlobalData();
// APTIOV_SERVER_OVERRIDE_RC_START : Added to skip LastBoot Error Handler for cold boot.
  GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
  if ( GuidHob != NULL) {
      systemMemoryMap = (struct SystemMemoryMapHob *) GET_GUID_HOB_DATA (GuidHob);
  }
  else {
     systemMemoryMap = NULL;
     Status = EFI_DEVICE_ERROR;
  }

  if ( ( systemMemoryMap != NULL ) && ( systemMemoryMap->IsColdBoot != TRUE ) ) {
  // APTIOV_SERVER_OVERRIDE_RC_END : Added to skip LastBoot Error Handler for cold boot.
  // Collect if any previous boot fatal errors and update them in whea error boot record table that arew supported.
  ChecAndUpdatePrevBootErrors();
  
  if(mPrevBootErrData.PciexErrFound == FALSE) {
    CheckAndUpdatePciExError();
  }
  // APTIOV_SERVER_OVERRIDE_RC_START : Added to skip LastBoot Error Handler for cold boot.
  }
  // APTIOV_SERVER_OVERRIDE_RC_END : Added to skip LastBoot Error Handler for cold boot.
    
  return EFI_SUCCESS;
}

