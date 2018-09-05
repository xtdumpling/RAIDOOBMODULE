//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file  DxeFrb.h  

    Header file for DXE FRB Modular part

**/

#ifndef _DXE_FRB_H_
#define _DXE_FRB_H_

//----------------------------------------------------------------------

#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Pi/PiSmmCis.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/HobList.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/FrbProtocol.h>
#include <Protocol/LegacyBiosExt.h>
#include <IpmiNetFnAppDefinitions.h>

//----------------------------------------------------------------------

#define EFI_SM_FRB_SIGNATURE EFI_SIGNATURE_32('f','r','b','x')
//APTIOV_SERVER_OVERRIDE_START
//
// EDK I Shell Guid
//
#define SHELL_ENVIRONMENT_PROTOCOL_GUID \
  { \
    0x47c7b221, 0xc42a, 0x11d2, {0x8e, 0x57, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b} \
  }

//
// EDK II Shell Guid 
//
#define EFI_SHELL_PROTOCOL_GUID \
  { \
    0x6302d008, 0x7f9b, 0x4f30, { 0x87, 0xac, 0x60, 0xc9, 0xfe, 0xf5, 0xda, 0x4e } \
  }
//APTIOV_SERVER_OVERRIDE_END

typedef struct {
  UINT16    TimeOutValue;           ///< Count down to start from 
  UINT8     PretimeoutInterval;     ///< PreTimeoutInterval
  UINT8     PreTimeOutInterrupt;    ///< Interrupt type to generate before timer expires
  UINT8     BootPolicy;             ///< Policy to do after timer expires like hard reset or power down.
} FRB_DATA_BUFFER;

VOID
EFIAPI
DisableTimersBeforeSetup (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
EFIAPI
DisableEnableFrb2DuringOpRomScan (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
EFIAPI
DisableFrb2BeforePassword (
  IN EFI_EVENT      Event,
  IN VOID           *Context
  );

VOID
EFIAPI
EnableFrb2AfterPassword (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

EFI_STATUS
EnableOsWdtTimer ( 
  VOID
  );

VOID
EFIAPI
DisableEnableOsWdtTimer (
  VOID
  );

VOID
EFIAPI
DisableOsWdtBeforeShell (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
EFIAPI
DisableFrb2BeforePromptTimeOut (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
EFIAPI
EnableFrb2AfterPromptTimeOut (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
EFIAPI
ClearFlag (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
