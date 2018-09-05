/**
@file  Implementation of the OEM RAS LIBRARY.

@copyright
  Copyright (c) 2011 - 2016 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
**/

#ifndef _OEM_RAS_LIB_H_
#define _OEM_RAS_LIB_H_

#include <PiDxe.h>
#include <Setup/IioUniversalData.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Library/DebugLib.h>

// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks Support
#include <Library\AmiErrReportingLib\AmiErrReportingLib.h>
#include <Library\SetupLib.h>
#include <Include\MemRas.h>
#include <ErrorRecords.h>
#include <Token.h>
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support
typedef struct {
  UINT8	    Bus;
  UINT8	    Device;   
  UINT8	    Function;
  BOOLEAN	IsRp;
  UINT8	    RpBus;
  UINT8	    RpDev;
  UINT8	    RpFunction;
  UINT32	VidDid;
  BOOLEAN	AerSupport;
  UINT32	ErrorConfigOverride;
} PCIE_DEVICE_OVERRIDE;

///
/// Function Prototypes
///

EFI_STATUS
OemOverrideRasTopologyStructure (
  IN OUT    EFI_RAS_SYSTEM_TOPOLOGY *Topology
  );  

EFI_STATUS
OemOverrideRasCapabilityStructure (
  IN OUT    PLATFORM_RAS_CAPABILITY *Capability
  );
  
EFI_STATUS
OemOverridePcieErrorLogging (
  IN OUT    PCIE_DEVICE_OVERRIDE *PciDevice
  );
  
EFI_STATUS
OemNotifyOs (
  IN      UINT8     ErrorSeverity,
     OUT  BOOLEAN   *OsNotified
  );
  
EFI_STATUS
OemOverrideRasSetupStructure (
  IN OUT    SYSTEM_RAS_SETUP *SetupStructure
  );

// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks Support
EFI_STATUS
OemMcaErrEnableDisableReporting (
  IN  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol
  );

EFI_STATUS
OemMemErrEnableDisableReporting (
  IN  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol
  );

EFI_STATUS
OemPciErrEnableDisableReporting (
  IN  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol
  );

EFI_STATUS
OemReadyToTakeAction (
	UEFI_MEM_ERROR_RECORD *MemoryErrorRecord, 
	BOOLEAN               *SkipEvent
	);

EFI_STATUS
OemCollectErrors (
	VOID *ErrorDataStructure
  );

EFI_STATUS
OemReportStatusCode (
  IN        EVENT_NAME  EventName,
  IN        UEFI_MEM_ERROR_RECORD *MemoryErrorRecord
  );

EFI_STATUS
OemInitializeAmiErrorHandler (
  IN  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol
  );
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support
#endif
