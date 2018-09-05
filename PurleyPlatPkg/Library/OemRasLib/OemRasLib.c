/**
@file  Implementation of the OEM RAS LIBRARY.

@copyright
  Copyright (c) 2011 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/


#include "OemRasLib.h"

/**
  Override Ras Topology Structure

  This function overrides Ras Topology structure.

  @param[in out] *Topology          A structure pointer of Topology may need to be overridden

  @retval Status.

**/
EFI_STATUS
OemOverrideRasTopologyStructure (
  IN OUT   EFI_RAS_SYSTEM_TOPOLOGY *Topology
  )
{
  EFI_STATUS          Status;
  // APTIOV_SERVER_OVERRIDE_RC_START
  Topology->SystemInfo.LastBootErrorFlag = FALSE; // Added support for Last Boot Error Logging
  Topology->SystemInfo.SkipFillOtherMcBankInfoFlag = FALSE; // Collect all Mcbanks Info before logging error
  // APTIOV_SERVER_OVERRIDE_RC_END

  Status = EFI_SUCCESS;
  
  return Status;
}

/**
  Override Ras Capability Structure

  This function overrides Ras Capability structure.

  @param[in out] *Capability        A pointer of Capability structure may need to be overridden

  @retval Status.

**/
EFI_STATUS
OemOverrideRasCapabilityStructure (
  IN OUT      PLATFORM_RAS_CAPABILITY *Capability
  )
{
  EFI_STATUS          Status;
 
  Status = EFI_SUCCESS;
 
 
  return Status;
}


/**
  Override Pcie Error Logging 

  This function overrides Pcie Error Logging for a Pcie device

  @param[in out] *PciDevice         A pointer of PciDevice may need to be overridden

  @retval Status.

**/
EFI_STATUS
OemOverridePcieErrorLogging (
  IN OUT    PCIE_DEVICE_OVERRIDE *PciDevice
  )
{
  EFI_STATUS          Status;
  
  Status = EFI_SUCCESS;
  
  return Status;
}

/**
  Override Error Handling 

  This function overrides OS Notification

  @param[in] ErrorSeverity          Error Severity

  @retval Status.

**/
EFI_STATUS
OemNotifyOs (
  IN      UINT8     ErrorSeverity,
     OUT  BOOLEAN   *OsNotified
  )
{
  EFI_STATUS          Status;
  
  Status = EFI_SUCCESS;
  
  return Status;
}
  
/**
  Override RAS Setup Structure  

  This function overrides Setup Structure
  @param[in out] *SetupStructure         A pointer of Setup Stucture may need to be overridden

  @retval Status.

**/
EFI_STATUS
OemOverrideRasSetupStructure (
  IN OUT    SYSTEM_RAS_SETUP *SetupStructure
  )
{
  EFI_STATUS          Status;
  // APTIOV_SERVER_OVERRIDE_RC_START : Unlock Msr 790 for Error Injection Tools to work.
  SYSTEM_CONFIGURATION     *SystemConfiguration;
  INTEL_SETUP_DATA                  SetupData;

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (SYSTEM_CONFIGURATION), &SystemConfiguration);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (SystemConfiguration, sizeof (SYSTEM_CONFIGURATION));

  Status = GetEntireConfig (&SetupData);
  ASSERT_EFI_ERROR (Status);
  CopyMem (SystemConfiguration, &(SetupData.SystemConfig), sizeof(SYSTEM_CONFIGURATION));

  #if ENABLE_ERROR_INJECTOR_SUPPORT
    SetupStructure->UnlockMsr		            	= SystemConfiguration->UnlockMsr;
  #else
    SetupStructure->UnlockMsr		            	= 0;
  #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Unlock Msr 790 for Error Injection Tools to work.
  // bit c4 (Master Abort Error Enable) should be masked because of a known issue.
  SetupStructure->Iio.IioCoreErrorBitMap = (SetupStructure->Iio.IioCoreErrorBitMap) & 0xffffffef;
  // APTIOV_SERVER_OVERRIDE_RC_START : Unlock Msr 790 for Error Injection Tools to work.
  gBS->FreePool (SystemConfiguration);
  // APTIOV_SERVER_OVERRIDE_RC_END : Unlock Msr 790 for Error Injection Tools to work.
  Status = EFI_SUCCESS;

  return Status;
}

// APTIOV_SERVER_OVERRIDE_RC_START : OEM hooks support
/**
  OemMcaErrEnableDisableReporting - OEM hook to Enable/Disable PCI Error Reporting

  This function provides a NULL method that can be overridden to set/override OEM error reporting parameters
  @param[in] *PlatformRasPolicyProtocol         A pointer of Ras Policy protocol containing Ras Setup, Capabilities and Topology information

  @retval Status.

**/
EFI_STATUS
OemMcaErrEnableDisableReporting (
  IN  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol
  ) 
{
    return EFI_SUCCESS;
}

/**
  OemMemErrEnableDisableReporting - OEM hook to Enable/Disable Memory Error Reporting

  This function provides a NULL method that can be overridden to set/override OEM error reporting parameters
  @param[in] *PlatformRasPolicyProtocol         A pointer of Ras Policy protocol containing Ras Setup, Capabilities and Topology information

  @retval Status.

**/
EFI_STATUS
OemMemErrEnableDisableReporting (
  IN  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol
  ) 
{
//APTIOV_SERVER_OVERRIDE_RC_START : Enable Mode1 Memory Error Reporting
#if MODE1_MEM_ERROR_REPORTING
    EnableMode1MemErrorReporting();
#endif
//APTIOV_SERVER_OVERRIDE_RC_END : Enable Mode1 Memory Error Reporting
    return EFI_SUCCESS;
}

/**
  OemPciErrEnableDisableReporting - OEM hook to Enable/Disable PCI Error Reporting

  This function provides a NULL method that can be overridden to set/override OEM error reporting parameters
  @param[in] *PlatformRasPolicyProtocol         A pointer of Ras Policy protocol containing Ras Setup, Capabilities and Topology information

  @retval Status.

**/
EFI_STATUS
OemPciErrEnableDisableReporting (
  IN  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol
  ) 
{
    return EFI_SUCCESS;
}

// Runtime Hooks 
/**
  OemReadyToTakeAction - OEM hook for modifying runtime handler execution.

  This function provides mechanism for following : 1. Decide action based on event ErrorEvent 
                                                   2. Decide to enter or exit handler code by clearing/setting SkipEvent variable
  @param[in] *MemoryErrorRecord      A pointer to memory error section.
  @param[in]  SkipEvent        Boolean variable holding handler skip or execute status.

  @retval Status.

**/
EFI_STATUS
OemReadyToTakeAction (
	UEFI_MEM_ERROR_RECORD *MemoryErrorRecord, 
	BOOLEAN *SkipEvent
  ) {
    return EFI_SUCCESS;
}

/**
  OemCollectErrors - NULL library Hook to collect more information about the errors

  This function provides mechanism for collecting extra information on errors logged 
  using methods like LogReportedError().
  @param[in] *ErrorDataStructure      A pointer to memory error section.

  @retval Status.

**/
EFI_STATUS
OemCollectErrors (
	VOID *ErrorDataStructure
  ) {
	return EFI_SUCCESS;
}

/**
  OemReportStatusCode - NULL library Hook to report error status codes

  This function provides mechanism for reporting error status codes .
  @param[in] EventName      Event name
  @param[in] *MemoryErrorRecord      A pointer to memory error section.

  @retval Status.

**/
EFI_STATUS
OemReportStatusCode (
  IN        EVENT_NAME  EventName,
  IN        UEFI_MEM_ERROR_RECORD *MemoryErrorRecord
  ) {
  
  // add OEM code to report status code
	return EFI_SUCCESS;
}

/**
  OemInitializeAmiErrorHandler - OEM hook in InitializeAmiErrorHandler

  This function provides a NULL method in InitializeAmiErrorHandler
  @param[in] *PlatformRasPolicyProtocol         A pointer of Ras Policy protocol containing Ras Setup, Capabilities and Topology information

  @retval Status.

**/
EFI_STATUS
OemInitializeAmiErrorHandler (
  IN  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol
  ) 
{
    return EFI_SUCCESS;
}
// APTIOV_SERVER_OVERRIDE_RC_END : OEM hooks support
