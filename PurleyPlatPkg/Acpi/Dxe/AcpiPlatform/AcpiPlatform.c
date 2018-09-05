//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//     Rev. 1.01        Fixed CPU frequency can't keep maximum when always turbo mode enabled.
//       Bug Fix:
//       Reason:        Set ACPI parameters (CStateEnable, C3, C6, PStateEnable) to 0 when always turbo mode enabled.
//       Auditor:       Jimmy Chiu
//       Date:          Jun/30/2017
//
//     Rev. 1.00        Add always turbo mode function.
//       Bug Fix:
//       Reason:        Reference from Grantlley, control by TOKEN "MAX_PERFORMANCE_OPTION", default disabled.
//       Auditor:       Jimmy Chiu
//       Date:          May/16/2016
//
//****************************************************************************
  /**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file AcpiPlatform.c

  ACPI Platform Driver

**/

#include "AcpiPlatform.h"
#include "AcpiPlatformHooks.h"
// APTIOV_SERVER_OVERRIDE_RC_START : To get AmiPlatformAcpiLib functions.
#include <Library/AmiPlatformAcpiLib.h> 
// APTIOV_SERVER_OVERRIDE_RC_END : To get AmiPlatformAcpiLib functions.

#pragma optimize("", off)

// APTIOV_SERVER_OVERRIDE_RC_START : Update "PCIe ASPM Controls" flag of FACP table.
#define MAX_FADT 2
// APTIOV_SERVER_OVERRIDE_RC_END : Update "PCIe ASPM Controls" flag of FACP table.

extern BIOS_ACPI_PARAM                *mAcpiParameter;
extern SOCKET_IIO_CONFIGURATION       mSocketIioConfiguration;
extern SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;
extern SOCKET_PROCESSORCORE_CONFIGURATION   mSocketProcessorCoreConfiguration;
extern EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE *mSpcrTable;   
extern EFI_GUID gEfiGlobalVariableGuid;

BOOLEAN               mFirstNotify;
SYSTEM_CONFIGURATION  mSystemConfiguration;
PCH_RC_CONFIGURATION  mPchRcConfiguration;

UINT8                 mKBPresent = 0;
UINT8                 mMousePresent = 0;


/**

  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

  @param Protocol  -  The protocol to find.
  Instance  -  Return pointer to the first instance of the protocol.
  Type      -  The type of protocol to locate.

  @retval EFI_SUCCESS           -  The function completed successfully.
  @retval EFI_NOT_FOUND         -  The protocol could not be located.
  @retval EFI_OUT_OF_RESOURCES  -  There are not enough resources to find the protocol.

**/
EFI_STATUS
LocateSupportProtocol (
  IN   EFI_GUID       *Protocol,
  IN   EFI_GUID       gEfiAcpiMultiTableStorageGuid,
  OUT  VOID           **Instance,
  IN   UINT32         Type
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              *HandleBuffer;
  UINTN                   NumberOfHandles;
  EFI_FV_FILETYPE         FileType;
  UINT32                  FvStatus;
  EFI_FV_FILE_ATTRIBUTES  Attributes;
  UINTN                   Size;
  UINTN                   Index;

  FvStatus = 0;
  //
  // Locate protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Protocol,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    //
    // Defined errors at this time are not found and out of resources.
    //
    return Status;
  }
  //
  // Looking for FV with ACPI storage file
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    Protocol,
                    Instance
                    );
    ASSERT (!EFI_ERROR (Status));

    if (!Type) {
      //
      // Not looking for the FV protocol, so find the first instance of the
      // protocol.  There should not be any errors because our handle buffer
      // should always contain at least one or LocateHandleBuffer would have
      // returned not found.
      //
      break;
    }
    //
    // See if it has the ACPI storage file
    //
    Status = ((EFI_FIRMWARE_VOLUME2_PROTOCOL *) (*Instance))->ReadFile (
                                                              *Instance,
                                                              &gEfiAcpiMultiTableStorageGuid,
                                                              NULL,
                                                              &Size,
                                                              &FileType,
                                                              &Attributes,
                                                              &FvStatus
                                                              );

    //
    // If we found it, then we are done
    //
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //
  //
  // Free any allocated buffers
  //
  gBS->FreePool (HandleBuffer);

  return Status;
}



/**

  GC_TODO: Add function description

  @param Event   - GC_TODO: add argument description
  @param Context - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
STATIC
VOID
EFIAPI
OnReadyToBoot (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  EFI_STATUS                  Status;
  // APTIOV_SERVER_OVERRIDE_RC_START
  //EFI_ACPI_DESCRIPTION_HEADER Table = {0};
  //EFI_ACPI_TABLE_VERSION      TableVersion;
  //UINTN                       TableHandle;
  //EFI_ACPI_TABLE_PROTOCOL     *AcpiTable;
  // APTIOV_SERVER_OVERRIDE_RC_END
  EFI_CPUID_REGISTER          CpuidRegisters;
  // APTIOV_SERVER_OVERRIDE_RC_START
  INTEL_SETUP_DATA                  SetupData; // To solve redefinition error
  // APTIOV_SERVER_OVERRIDE_RC_END
  UINT8                       ARIForward;

  if (mFirstNotify) {
    return ;
  }

  mFirstNotify = TRUE;
  
  Status = GetEntireConfig (&SetupData);
  ASSERT_EFI_ERROR (Status);
  CopyMem (&mSystemConfiguration, &(SetupData.SystemConfig), sizeof(SYSTEM_CONFIGURATION));
  CopyMem (&mSocketIioConfiguration, &(SetupData.SocketConfig.IioConfig), sizeof(SOCKET_IIO_CONFIGURATION));
  CopyMem (&mSocketPowermanagementConfiguration, &(SetupData.SocketConfig.PowerManagementConfig), sizeof(SOCKET_POWERMANAGEMENT_CONFIGURATION));
  CopyMem (&mSocketProcessorCoreConfiguration, &(SetupData.SocketConfig.SocketProcessorCoreConfiguration), sizeof(SOCKET_PROCESSORCORE_CONFIGURATION));
  CopyMem (&mPchRcConfiguration, &(SetupData.PchRcConfig), sizeof(PCH_RC_CONFIGURATION));

  mAcpiParameter->TpmEnable = mSystemConfiguration.TpmEnable;

  // CpuPm.Asl: External (CSEN, FieldUnitObj)
  mAcpiParameter->CStateEnable = !mSocketPowermanagementConfiguration.ProcessorAutonomousCstateEnable;
  // CpuPm.Asl: External (ACEN, FieldUnitObj)
  mAcpiParameter->AutoCstate = mSocketPowermanagementConfiguration.ProcessorAutonomousCstateEnable;
  // CpuPm.Asl: External (C3EN, FieldUnitObj)
  mAcpiParameter->C3Enable     = mSocketPowermanagementConfiguration.C3Enable;
  // CpuPm.Asl: External (C6EN, FieldUnitObj)
  if (mSocketPowermanagementConfiguration.C6Enable == PPM_AUTO || mAcpiParameter->AutoCstate) {
    mAcpiParameter->C6Enable     = 1;  //POR Default = Enabled
  } else {
    mAcpiParameter->C6Enable     = mSocketPowermanagementConfiguration.C6Enable;
  }
  if(mAcpiParameter->C6Enable && mAcpiParameter->C3Enable) {  //C3 and C6 enable are exclusive
    mAcpiParameter->C6Enable = 1;
    mAcpiParameter->C3Enable = 0;
  }
  // CpuPm.Asl: External (C7EN, FieldUnitObj)
  mAcpiParameter->C7Enable     = 0;
  // CpuPm.Asl: External (OSCX, FieldUnitObj)
  // If ACC enabled, leave OSCX set to 0 (C2)
  if(!mAcpiParameter->AutoCstate) {
    mAcpiParameter->OSCX         = mSocketPowermanagementConfiguration.OSCx;
  }
  // CpuPm.Asl: External (MWOS, FieldUnitObj)
  mAcpiParameter->MonitorMwaitEnable = 1;
  // CpuPm.Asl: External (PSEN, FieldUnitObj)
  mAcpiParameter->PStateEnable = mSocketPowermanagementConfiguration.ProcessorEistEnable;
  // CpuPm.Asl: External (HWAL, FieldUnitObj)
// APTIOV_SERVER_OVERRIDE_RC_START : HWAL value was not sync with the EIST PSD Setup Item
  mAcpiParameter->HWAllEnable = mSocketPowermanagementConfiguration.ProcessorEistPsdFunc; //Update in PatchGv3SsdtTable
// APTIOV_SERVER_OVERRIDE_RC_END : HWAL value was not sync with the EIST PSD Setup Item

  mAcpiParameter->KBPresent    = mKBPresent;
  mAcpiParameter->MousePresent = mMousePresent;
  mAcpiParameter->TStateEnable = mSocketPowermanagementConfiguration.TStateEnable;
  //Fine grained T state
  AsmCpuid (EFI_CPUID_POWER_MANAGEMENT_PARAMS,  &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  if ((CpuidRegisters.RegEax & EFI_FINE_GRAINED_CLOCK_MODULATION) && mSocketPowermanagementConfiguration.TStateEnable){
    mAcpiParameter->TStateFineGrained = 1;
  }
  if(CpuidRegisters.RegEax & B_CPUID_POWER_MANAGEMENT_EAX_HWP_LVT_INTERRUPT_SUPPORT) {
    mAcpiParameter->HwpInterrupt = 1;
  }
  // CpuPm.Asl: External (HWEN, FieldUnitObj)
  mAcpiParameter->HWPMEnable = DetectHwpFeature();

  mAcpiParameter->EmcaEn    = mSystemConfiguration.EmcaEn;
  mAcpiParameter->WheaSupportEn  = mSystemConfiguration.WheaSupportEn;

  mAcpiParameter->PcieAcpiHotPlugEnable = (UINT8) (BOOLEAN) (mSocketIioConfiguration.PcieAcpiHotPlugEnable != 0);
  //
  // Initialize USB3 mode from setup data
  //
  // If mode != manual control
  //  just copy mode from setup
  if (mPchRcConfiguration.PchUsbManualMode != 1) {
    mAcpiParameter->XhciMode = mPchRcConfiguration.PchUsbManualMode;
  }
  if (IsSimicsPlatform() == TRUE) {
    // SKX_TODO: Windows cannot boot with MWAIT and Cstates enabled
    mAcpiParameter->CStateEnable = 0; 
    mAcpiParameter->C6Enable     = 0; 
    mAcpiParameter->MonitorMwaitEnable = 0;
  }

#if SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION   //For always turbo feature
  if((mSocketPowermanagementConfiguration.PwrPerfTuning == 1) && (mSocketPowermanagementConfiguration.AltEngPerfBIAS == 3)){
    mAcpiParameter->CStateEnable = 0;
    mAcpiParameter->C3Enable     = 0;
    mAcpiParameter->C6Enable     = 0;
    mAcpiParameter->PStateEnable = 0;
    mAcpiParameter->MonitorMwaitEnable = 0;
  }
#endif //SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION

  //
  // Find the AcpiTable protocol
  //
// APTIOV_SERVER_OVERRIDE_RC_START
  #if 0
  Status = LocateSupportProtocol (
            &gEfiAcpiTableProtocolGuid,
            gEfiAcpiTableStorageGuid,
            &AcpiTable,
            FALSE
            );
  
  ASSERT_EFI_ERROR (Status);

  TableVersion    = EFI_ACPI_TABLE_VERSION_2_0;
  Table.Signature = EFI_ACPI_2_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE;
  Status = PlatformUpdateTables ((EFI_ACPI_COMMON_HEADER *)&Table, &TableVersion);
  if(!EFI_ERROR (Status)) {
    //
    // Add SPCR table
    //
    if (mSpcrTable != NULL) {
      DEBUG ((DEBUG_INFO, "mSpcrTable->Header.Length=%d\n", mSpcrTable->Header.Length));
      DEBUG ((DEBUG_INFO, "install=%x\n", &(AcpiTable->InstallAcpiTable)));
      DEBUG ((DEBUG_INFO, "acpit=%x\n",   AcpiTable));
      DEBUG ((DEBUG_INFO, "mSpcr=%x\n",   mSpcrTable));
      DEBUG ((DEBUG_INFO, "len   =%d\n",  mSpcrTable->Header.Length));

      TableHandle = 0;
      Status = AcpiTable->InstallAcpiTable (
        AcpiTable,
        mSpcrTable,
        mSpcrTable->Header.Length,
        &TableHandle
        );
      ASSERT_EFI_ERROR (Status);
    } else {
      DEBUG ((DEBUG_INFO, "Warning: mSpcrTable is NULL\n"));
    }
  }
  if (mSpcrTable != NULL) {
    gBS->FreePool (mSpcrTable);
  }

 #endif
// APTIOV_SERVER_OVERRIDE_RC_END
  VtdAcpiTablesUpdateFn();

  Status = GetOptionData (&gEfiSetupVariableGuid,  OFFSET_OF(SYSTEM_CONFIGURATION, ARIForward), &ARIForward, sizeof(UINT8));
  ASSERT_EFI_ERROR (Status);
  if (!ARIForward) {
    DisableAriForwarding();
  }

  // APTIOV_SERVER_OVERRIDE_RC_START : Close event so that is isn't called again.
  gBS->CloseEvent(Event);
  // APTIOV_SERVER_OVERRIDE_RC_END : Close event so that is isn't called again.
}


/**

  Entry point for Acpi platform driver.

  @param ImageHandle  -  A handle for the image that is initializing this driver.
  @param SystemTable  -  A pointer to the EFI system table.

  @retval EFI_SUCCESS           -  Driver initialized successfully.
  @retval EFI_LOAD_ERROR        -  Failed to Initialize or has been loaded.
  @retval EFI_OUT_OF_RESOURCES  -  Could not allocate needed resources.

**/
EFI_STATUS
EFIAPI
AcpiPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    AcpiStatus;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_FIRMWARE_VOLUME2_PROTOCOL  *FwVol;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINTN                         TableHandle;
  UINT32                        FvStatus;
  UINT32                        Size;
  EFI_EVENT                     Event;
  EFI_ACPI_TABLE_VERSION        TableVersion;
  VOID                          *HobList;

//
// APTIOV_SERVER_OVERRIDE_RC_START : Update "PCIe ASPM Controls" flag of FACP table.
//
  UINT32                        Version = EFI_ACPI_TABLE_VERSION_1_0B;
  UINT8                         FadtTableCount;
//
// APTIOV_SERVER_OVERRIDE_RC_END : Update "PCIe ASPM Controls" flag of FACP table.
//

  mFirstNotify      = FALSE;

  TableVersion      = EFI_ACPI_TABLE_VERSION_NONE;
  Instance          = 0;
  CurrentTable      = NULL;
  TableHandle       = 0;

  //
  // Report Status Code to indicate Acpi Init
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_BS_ACPI_INIT)
    );

  //
  // Update HOB variable for PCI resource information
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  //
  // Find the AcpiTable protocol
  //
  Status = LocateSupportProtocol (
            &gEfiAcpiTableProtocolGuid,
            gEfiAcpiTableStorageGuid,
            &AcpiTable,
            FALSE
            );
  
  ASSERT_EFI_ERROR (Status);

  //
  // Locate the firmware volume protocol
  //
  Status = LocateSupportProtocol (
            &gEfiFirmwareVolume2ProtocolGuid,
            gEfiAcpiTableStorageGuid,
            &FwVol,
            TRUE
            );
  ASSERT_EFI_ERROR (Status);

  Status    = EFI_SUCCESS;
  Instance  = 0;

  PlatformHookInit ();
  UpdateVtdIntRemapFlag(mSocketIioConfiguration.VTdSupport, mSocketIioConfiguration.InterruptRemap);

  //
  // Read tables from the storage file.
  //
  while (!EFI_ERROR (Status)) {
    CurrentTable = NULL;

    Status = FwVol->ReadSection (
                      FwVol,
                      &gEfiAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &CurrentTable,
                      (UINTN *) &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      //
      // Allow platform specific code to reject the table or update it
      //
      AcpiStatus = AcpiPlatformHooksIsActiveTable (CurrentTable); //SystemBoard);

      if (!EFI_ERROR (AcpiStatus)) {
        //
        // Perform any table specific updates.
        //
        AcpiStatus = PlatformUpdateTables (CurrentTable, &TableVersion);
        if (!EFI_ERROR (AcpiStatus)) {
          //
          // Add the table
          //
          TableHandle = 0;
          if (TableVersion != EFI_ACPI_TABLE_VERSION_NONE) {
            AcpiStatus = AcpiTable->InstallAcpiTable (
                                      AcpiTable,
                                      CurrentTable,
                                      CurrentTable->Length,
                                      &TableHandle
                                      );
          }
          ASSERT_EFI_ERROR (AcpiStatus);
        }
      }
      //
      // Increment the instance
      //
      Instance++;
// APTIOV_SERVER_OVERRIDE_RC_START : To free the buffer allocated by ReadSection.
      FreePool(CurrentTable); 
// APTIOV_SERVER_OVERRIDE_RC_END : To free the buffer allocated by ReadSection.
    }
  }

  //
  //APTIOV_SERVER_OVERRIDE_RC_START: To read from AMI DSDT FFS
  //
      Status = AcpiLibGetAcpiTable(
                   EFI_ACPI_TABLE_VERSION_ALL,
                   EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
                   (EFI_ACPI_SDT_HEADER**) &CurrentTable,
                   &TableHandle
                   );
      if(EFI_ERROR(Status)){
          DEBUG ((EFI_D_ERROR,"AcpiPlatform: Failed to get AMI DSDT, Status: %r\n", Status));
          ASSERT_EFI_ERROR(Status);
      } else{
          //
          // Allow platform specific code to reject the table or update it
          //
          AcpiStatus = AcpiPlatformHooksIsActiveTable (CurrentTable); 
          if (!EFI_ERROR (AcpiStatus)) {
              //
              // Perform any table specific updates.
              //
              AcpiStatus = PlatformUpdateTables (CurrentTable, &TableVersion);
          }
          if (!EFI_ERROR(AcpiStatus)){
        	  Status = AcpiTable->UninstallAcpiTable(AcpiTable, TableHandle);
        	  if(!EFI_ERROR(Status)){
        		  TableHandle = 0;
                  Status = AcpiTable->InstallAcpiTable(
                              AcpiTable, 
                              CurrentTable,
                              CurrentTable->Length,
                              &TableHandle);
              }
              DEBUG ((EFI_D_INFO,"AcpiPlatform: ReinstallAcpiTable for AMI DSDT %r\n", Status));
          }
          FreePool(CurrentTable); // Freeing memory allocated by GetAcpiTable call for DSDT.
      }
      
  //
  //APTIOV_SERVER_OVERRIDE_RC_END: To read from AMI DSDT FFS
  //
  //
  // Install xHCI ACPI Table
  //
  InstallXhciAcpiTable ();
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Update "PCIe ASPM Controls" flag of FACP table.
  //
      for (FadtTableCount = 0; FadtTableCount < MAX_FADT; FadtTableCount++) {
          Status = AcpiLibGetAcpiTable (
                     Version,
                     EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
                     (EFI_ACPI_SDT_HEADER **) &CurrentTable,
                     &TableHandle
                   );
          DEBUG ((EFI_D_ERROR,"AcpiPlatform: AcpiLibGetAcpiTable Status: %r\n", Status));
          if (!EFI_ERROR(Status)) {
              //
              // Perform any table specific updates.
              //
              AcpiStatus = PlatformUpdateTables (CurrentTable, &TableVersion);
              if (!EFI_ERROR (AcpiStatus)) {
                  Status = AcpiLibReinstallAcpiTable (
                             (VOID *)CurrentTable,
                             &TableHandle
                           );
                  DEBUG ((EFI_D_ERROR,"AcpiPlatform: AcpiLibReinstallAcpiTable for AMI FADT %r\n", Status));
                  Version = EFI_ACPI_TABLE_VERSION_X;
              }
              FreePool(CurrentTable); // Freeing memory allocated by GetAcpiTable call for FADT.
          } else {
              if (Status != EFI_NOT_FOUND) ASSERT_EFI_ERROR (Status);
              break;
          }
      }
  //
  // APTIOV_SERVER_OVERRIDE_RC_END :  Update "PCIe ASPM Controls" flag of FACP table.
  //

  Status = EfiCreateEventReadyToBootEx(
             TPL_NOTIFY,
             OnReadyToBoot,
             NULL,
             &Event
             );

  //
  // Finished
  //
  return EFI_SUCCESS;
}
