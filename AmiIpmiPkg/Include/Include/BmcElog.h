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

/** @file BmcElog.h
    Baseboard Event Log functions.

**/

#ifndef _EFI_BMCELOG_H_
#define _EFI_BMCELOG_H_

//----------------------------------------------------------------------
#include "Token.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Include/IpmiNetFnSensorDefinitions.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/RedirElogProtocol.h>
#include "ServerMgmtSetupVariable.h"

//----------------------------------------------------------------------

#define EFI_ELOG_PHYSICAL                0
#define EFI_ELOG_VIRTUAL                 1
#define MAX_REDIR_DESCRIPTOR             10
#define MAX_TEMP_DATA                    100
#define SEL_RESERVE_ID_SUPPORT           0x02
#define DELETE_SEL_ENTRY_SUPPORT_MASK    0x08

#define IPMI_SPEC_VERSION_1_5           0x51
#define IPMI_SPEC_VERSION_2_0           0x02

//
// BMC Elog instance data
//
#pragma pack(1)
typedef struct {
  UINTN                         Signature;
  EFI_SM_ELOG_REDIR_TYPE        DataType;
  UINT8                         TempData[MAX_TEMP_DATA + 1];
  EFI_SM_ELOG_REDIR_PROTOCOL    BmcElog;
} EFI_BMC_ELOG_INSTANCE_DATA;
#pragma pack()

BOOLEAN
IsSelloggingEnabled(
  VOID
  );

EFI_STATUS
GetReservationId(
  UINT8* ReserveId
  );

//
// BMC Elog Instance signature
//
#ifndef _CR
#define _CR(Record, TYPE, Field)           ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#endif
#ifndef CR
#define CR(record, TYPE, field, signature) _CR(record, TYPE, field)
#endif
#ifndef EFI_SIGNATURE_16
#define EFI_SIGNATURE_16(A, B)             ((A) | (B << 8))
#endif
#ifndef EFI_SIGNATURE_32
#define EFI_SIGNATURE_32(A, B, C, D)       (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#endif

#define SM_ELOG_REDIR_SIGNATURE    EFI_SIGNATURE_32('e','l','o','f')

#define INSTANCE_FROM_EFI_SM_ELOG_REDIR_THIS(a) \
  CR(a, EFI_BMC_ELOG_INSTANCE_DATA, BmcElog, SM_ELOG_REDIR_SIGNATURE)
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
