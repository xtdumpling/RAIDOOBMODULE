//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Support a feature that Update SMBIOS Type 1 and 3 with Fru0.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     May/12/2017
//
//****************************************************************************
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

/** @file IpmiRedirFru.h
    IPMI Redir FRU functions.

**/

#ifndef _EFI_IPMI_REDIR_FRU_H_
#define _EFI_IPMI_REDIR_FRU_H_

//----------------------------------------------------------------------

#include "Token.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/GenericFruProtocol.h>
#include <Protocol/RedirFruProtocol.h>
#include <Include/Fru.h>

//----------------------------------------------------------------------

//
// Macro definitions
//

#define  CHASSIS_TYPE_LENGTH            1
#define  CHASSIS_TYPE_OFFSET            2 
#define  CHASSIS_PART_NUMBER            3 

#define  PRODUCT_MFG_OFFSET             3 
#define  BOARD_MFG_OFFSET               6 

#define  MANAGEMENT_ACCESS_RECORD       0x03
#define  SYSTEM_UUID_SUB_RECORD_TYPE    0x07
#define  RECORD_CHECKSUM_SIZE           sizeof(UINT8)

#define  FRU_DATA_LENGTH_MASK           0x3F
#define  FRU_DATA_TYPE_MASK             0xC0

#define  IPMI_FRU_COMMAND_RETRY_DELAY      30000
#define  IPMI_FRU_COMMAND_RETRY_COUNTER    3

EFI_STATUS
EFIAPI
EfiGetFruRedirData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN UINTN                                FruSlotNumber,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData
  );

VOID
GenerateFruSmbiosData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN BOOLEAN                              GenerateDummySmbiosRecords
  );

#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
EFI_STATUS
EFIAPI
SmcEfiGetFruRedirData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN UINTN                                FruSlotNumber,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData
  );

VOID
SmcGenerateFruSmbiosData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN BOOLEAN                              GenerateDummySmbiosRecords
  );
#endif

EFI_STATUS
UpdateFruSmbiosTables (
  VOID
  );

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
