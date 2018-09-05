//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiFrb.h
    Header file for PeiFrb PEIM

**/

#ifndef _PEI_FRB_H_
#define _PEI_FRB_H_

//-----------------------------------------------------------------------
#include <PiPei.h>
#include <Pi/PiStatusCode.h>
#include <Library/DebugLib.h>
#include <Include/Frb.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Include/ServerMgmtSetupVariable.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/IPMITransportPpi.h>
#include <Ppi/ReadOnlyVariable2.h>
//-----------------------------------------------------------------------

VOID
GetFrbSetupOptions (
  IN CONST  EFI_PEI_SERVICES                        **PeiServices,
  IN OUT    EFI_BMC_SET_WATCHDOG_TIMER_STRUCTURE    *SetWatchDogTimerData,
  IN OUT    BOOLEAN                                 *FrbEnabled );

VOID
DisableFrb3Timer (
  IN CONST EFI_PEI_SERVICES  **PeiServices );

EFI_STATUS
EFIAPI
StopFrb2Timer (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi );
#endif

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
