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

#ifndef _EFI_PEI_IPMI_BMC_H_
#define _EFI_PEI_IPMI_BMC_H_

//---------------------------------------------------------------------------

#include "Token.h"
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/PciLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/IPMITransportPpi.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/I2cMaster.h>
#include <Ppi/Smbus2.h>
#include "SsifAlertPinCheckHook.h"

//---------------------------------------------------------------------------

typedef PEI_IPMI_TRANSPORT_PPI EFI_IPMI_TRANSPORT;
typedef EFI_PEI_BMC_STATUS EFI_BMC_STATUS;

#include <Include/IpmiBmcCommon.h>
#include <Include/IpmiNetFnAppDefinitions.h>

//
// Delay Constants
//
#define DELAY_OF_THIRTY_SECOND  0x1C9C380  //30000000 Microseconds

//
// LPC bus, device, function constants
//
#define LPC_GEN_PMCON_3         0xA4
#define LPC_BUS                 0
#define LPC_DEVICE              31
#define LPC_FUNC                0
#define PWR_FLR_BIT             BIT1
#define CLEAR_BIT               BIT1

//
// IPMB Message Sequence Number
//
#define SEQ_NO 0
//
// Completion code related definitions
//
#define COMPLETION_CODE_SUCCESS                     0x00
#define COMPLETION_CODE_DEVICE_SPECIFIC_START       0x01
#define COMPLETION_CODE_DEVICE_SPECIFIC_END         0x7E
#define COMPLETION_CODE_COMMAND_SPECIFIC_START      0x80
#define COMPLETION_CODE_COMMAND_SPECIFIC_END        0xBE

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

#define INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS(a) \
  CR ( \
  a, \
  EFI_IPMI_BMC_INSTANCE_DATA, \
  IpmiTransport, \
  SM_IPMI_BMC_SIGNATURE \
  )

//
// SSIF Block length definition
//
#define SMBUS_BLOCK_LENGTH 0x20

//
// Hook list
//
typedef EFI_STATUS (INIT_LPC_BMC)(
  IN CONST EFI_PEI_SERVICES          **PeiServices
);

#if BMC_INIT_DELAY
typedef BOOLEAN (BMC_INIT_DELAY_HOOK)(
  IN CONST EFI_PEI_SERVICES          **PeiServices
);
#endif

//
// SMB alert pin hook function definition
//
typedef BOOLEAN (SSIF_ALERT_PIN_CHECK)(
  IN  VOID
);

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
//
// Ipmi OEM PEI initialization hook.
//
typedef EFI_STATUS (IPMI_OEM_PEI_INIT_HOOK)(
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       UINTN                        *MmioBaseAddress,
  IN       UINTN                        *BaseAddressRange
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
