//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IpmiBmc.h
    IPMI BMC Controller functions.

**/

#ifndef _EFI_IPMI_BMC_H_
#define _EFI_IPMI_BMC_H_

//---------------------------------------------------------------------------

#include "Token.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>

#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/IPMISelfTestLogProtocol.h>
#include <Include/IpmiBmcCommon.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Include/IpmiNetFnSensorDefinitions.h>
#include <Protocol/I2cMaster.h>
#include <Protocol/SmbusHc.h>
#include "SsifAlertPinCheckHook.h"

//---------------------------------------------------------------------------

//
// Function Prototypes
//
BOOLEAN
IsLeapYear (
  IN UINT16   Year
  );

//
// Some Common Functions
//

EFI_STATUS
InitializeIpmiPhysicalLayerHook (
  VOID
  );

EFI_STATUS
IpmiSmbiosTypesSupport (
  VOID
  );

EFI_STATUS
SpmiSupport (
  VOID
  );

EFI_STATUS
IpmiCmosClear (
  VOID
  );

//
// Completion code related definitions
//
#define COMPLETION_CODE_SUCCESS                     0x00
#define COMPLETION_CODE_DEVICE_SPECIFIC_START       0x01
#define COMPLETION_CODE_DEVICE_SPECIFIC_END         0x7E
#define COMPLETION_CODE_COMMAND_SPECIFIC_START      0x80
#define COMPLETION_CODE_COMMAND_SPECIFIC_END        0xBE

//
// IPMB Message Sequence Number
//
#define SEQ_NO 0


//#define ERROR_COMPLETION_CODE(a) !( (a >= COMPLETION_CODE_SUCCESS) && (a <= COMPLETION_CODE_COMMAND_SPECIFIC_END) )

#define ERROR_COMPLETION_CODE(a)        !( (a == COMPLETION_CODE_SUCCESS) || \
                            ( (a >= COMPLETION_CODE_DEVICE_SPECIFIC_START) &&\
                                (a <= COMPLETION_CODE_DEVICE_SPECIFIC_END) )||\
                            ( (a >= COMPLETION_CODE_COMMAND_SPECIFIC_START) &&\
                                (a <= COMPLETION_CODE_COMMAND_SPECIFIC_END) ) )

//
// IPMI Instance signature
//
#define SM_IPMI_BMC_SIGNATURE SIGNATURE_32 ('i', 'p', 'm', 'i')

//
// SSIF Block length definition
//
#define SMBUS_BLOCK_LENGTH 0x20


#define INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS(a) \
  CR ( \
  a, \
  EFI_IPMI_BMC_INSTANCE_DATA, \
  IpmiTransport, \
  SM_IPMI_BMC_SIGNATURE \
  )

//
// SMB alert pin hook function definition
//
typedef BOOLEAN (SSIF_ALERT_PIN_CHECK)(
  IN VOID
);

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
//
// Ipmi OEM DXE and SMM initialization hooks and Smbios type 38 update hook.
//
typedef EFI_STATUS (IPMI_OEM_DXE_INIT_HOOK)(
  IN CONST EFI_BOOT_SERVICES            *BootServices,
  IN       UINTN                        *MmioBaseAddress,
  IN       UINTN                        *BaseAddressRange
);

typedef EFI_STATUS (IPMI_OEM_SMM_INIT_HOOK)(
  IN CONST EFI_BOOT_SERVICES            *BootServices,
  IN       UINTN                        *MmioBaseAddress,
  IN       UINTN                        *BaseAddressRange
);

typedef EFI_STATUS (IPMI_OEM_SMBIOS_TYPES_UPDATE_HOOK)(
  IN OUT   VOID                         **SmbiosTypeRecord
);
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
