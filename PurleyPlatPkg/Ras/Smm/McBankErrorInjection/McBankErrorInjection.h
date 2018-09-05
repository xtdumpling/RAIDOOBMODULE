
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2014 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file McaErrorInj.h

    This is an implementation of the Mca Support protocol.

---------------------------------------------------------------------------**/


#ifndef _MCEJ_FILE_
#define _MCEJ_FILE_

//
// Includes
//
#include <Uefi.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#include <AmiProtocol.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Library/SmmServicesTableLib.h>

#pragma pack (1)

#define MCEJ_SWSMI_VALUE                0x9F
#define MCEJ_SUPPORT_EN  1
#define MCEJ_SUPPORT_DIS  0

//
// MCEJ Definitions
//
#define EFI_ACPI_3_0_MCEJ_SIGNATURE                     0x4A45434D
#define EFI_ACPI_MCEJ_REVISION                          0x00000001

#define EFI_ACPI_MCEJ_ID                                0x8086A201      //???

//
// ACPI table information used to initialize tables.
//
#ifndef EFI_ACPI_OEM_ID
// OEMID 6 bytes long
#define EFI_ACPI_OEM_ID         'I', 'N', 'T', 'E', 'L', ' '
#endif
#ifndef EFI_ACPI_OEM_TABLE_ID
// OEM table id 8 bytes long
#define EFI_ACPI_OEM_TABLE_ID     EFI_SIGNATURE_64 ('B', 'R', 'I', 'C', 'K', 'L', 'A', 'N') 
#endif
#ifndef EFI_ACPI_OEM_REVISION
#define EFI_ACPI_OEM_REVISION     0x00000001
#endif
#ifndef EFI_ACPI_CREATOR_ID
//#define EFI_ACPI_CREATOR_ID     0x5446534D          // "MSFT"
#define EFI_ACPI_CREATOR_ID       0x4C544E49       // INTL
#endif
#ifndef EFI_ACPI_CREATOR_REVISION
#define EFI_ACPI_CREATOR_REVISION   0x0100000D
#endif
#ifndef EFI_ACPI_TABLE_VERSION_ALL
#define EFI_ACPI_TABLE_VERSION_ALL    (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_2_0|EFI_ACPI_TABLE_VERSION_3_0)
#endif
#define MCEJ_ACPI_VERSION_SUPPORT   (EFI_ACPI_TABLE_VERSION_ALL)

// MCA error injection MSRs
#define DEBUG_ERR_INJ_CTL                             0x1e3
  #define MCBANK_OVERWRITE_EN             0x01  // BIT0
  #define MCA_CMCI_SIGNAL_EN                        0x02  // BIT1
#define DEBUG_ERR_INJ_CTL2                            0x1e4
  #define GENERATE_MCA                0x01  // BIT1
  #define GENERATE_CMCI               0x02  // BIT0 


// Commands passed by OS in the set command action entry of MCEJ table
#define CMD_MCBANK_OVERWRITE_EN                       0x01
#define CMD_MCA_CMCI_SIGNAL_EN                        0x02
#define CMD_GENERATE_MCA                              0x04
#define CMD_GENERATE_CMCI                             0x08

// Capabilities supported in the GET capabilities action entry of MCEJ table
#define MCA_CAP_SUPPORTED               0x01
#define CMCI_CAP_SUPPORTED                0x02

// MCA error injection action entry types
#define MCA_ERR_INJ_BEGIN_INJECT_OP                   0x00
#define MCA_ERR_INJ_GET_TRIGGER_ACTION_TABLE          0x01
#define MCA_ERR_INJ_GET_CAP                           0x02
#define MCA_ERR_INJ_SET_CMD                           0x03
#define MCA_ERR_INJ_EXECUTE_INJECT_OP                 0x04
#define MCA_ERR_INJ_END_INJECT_OP                     0x05
#define MCA_ERR_INJ_CHECK_BUSY_STATUS                 0x06
#define MCA_ERR_INJ_GET_CMD_STATUS                    0x07
#define MCA_ERR_INJ_TRIGGER_ERROR_ACTION              0xFF

// Error injection operation definitions
#define MCA_ERR_INJ_BEGIN_OPERATION                   0x55AA55AA
#define MCA_ERR_INJ_END_OPERATION                     0x00000000

// Error injection operation status
#define MCA_ERR_INJ_OPERATION_BUSY                    0x01
// Error injection command status
#define MCA_ERR_INJ_CMD_SUCCESS                       0x00
#define MCA_ERR_INJ_CMD_UNKNOWN_FAILURE               0x01
#define MCA_ERR_INJ_CMD_INVALID_ACCESS                0x02

#define INSTRUCTION_READ_REGISTER                     0x00
#define INSTRUCTION_READ_REGISTER_VALUE               0x01
#define INSTRUCTION_WRITE_REGISTER                    0x02
#define INSTRUCTION_WRITE_REGISTER_VALUE              0x03
#define INSTRUCTION_NO_OPERATION                      0x04

#define FLAG_NOTHING                                  0x00
#define FLAG_PRESERVE_REGISTER                        0x01
#define SW_SMI_PORT_ADDR                              0xb2 //APM_CNT Register

// Action entry count
#define EFI_ACPI_MCEJ_ACTION_ENTRY_COUNT        0x08
#define EFI_ACPI_GENERIC_ADDRESS_STRUCTURE  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE

//
// Generic Address Space Address IDs
//
#define EFI_ACPI_SYSTEM_MEMORY              EFI_ACPI_3_0_SYSTEM_MEMORY
#define EFI_ACPI_SYSTEM_IO                  EFI_ACPI_3_0_SYSTEM_IO
#define EFI_ACPI_PCI_CONFIGURATION_SPACE    EFI_ACPI_3_0_PCI_CONFIGURATION_SPACE
#define EFI_ACPI_EMBEDDED_CONTROLLER        EFI_ACPI_3_0_EMBEDDED_CONTROLLER
#define EFI_ACPI_SMBUS                      EFI_ACPI_3_0_SMBUS
#define EFI_ACPI_FUNCTIONAL_FIXED_HARDWARE  EFI_ACPI_3_0_FUNCTIONAL_FIXED_HARDWARE

//
// Generic Address Space Access Sizes
//
#define EFI_ACPI_UNDEFINED  EFI_ACPI_3_0_UNDEFINED
#define EFI_ACPI_BYTE       EFI_ACPI_3_0_BYTE
#define EFI_ACPI_WORD       EFI_ACPI_3_0_WORD
#define EFI_ACPI_DWORD      EFI_ACPI_3_0_DWORD
#define EFI_ACPI_QWORD      EFI_ACPI_3_0_QWORD

//#define VALID_SET_CMD   ( MCBANK_OVERWRITE | MCA_CMCI_SIGNAL_EN | GENERATE_MCA | GENERATE_CMCI)

#pragma pack (1)

#define McejRegisterFiller  { \
  EFI_ACPI_SYSTEM_MEMORY, \
  64,                         \
  0,                          \
  EFI_ACPI_QWORD,         \
  0}

#define McejActionItemFiller  { \
  EFI_ACPI_SYSTEM_MEMORY, \
  8,                          \
  0,                          \
  EFI_ACPI_BYTE,          \
  0}

#define McejActionItemExecute { \
  EFI_ACPI_3_0_SYSTEM_IO,     \
  8,                          \
  0,                          \
  EFI_ACPI_BYTE,          \
  SW_SMI_PORT_ADDR }

// Parametre buffer used in updating the action entries of the MCEJ table.
typedef struct {
  UINT64                InjectionCap;            
  UINT64                CmdValue;            
  UINT64                TriggerActionTable;            
  UINT64                CmdStatus;            
  UINT64                BusyStatus;            
  UINT64                OpState;            
} MCA_ERR_INJ_PARAM_BUFFER;

// MCA Bank Injection Header in the MCEJ ACPI Table
typedef struct {
  UINT32                 InjectionHeaderSize;
  UINT32                 Resv1;
  UINT32                 InstructionEntryCount;
} EFI_ACPI_MCEJ_MCA_BANK_ERROR_INJECTION_HEADER;

// Trigger action table Header
typedef struct {
  UINT32        HeaderSize;
  UINT32                Revision;
  UINT32                TableSize;
  UINT32                EntryCount;
} EFI_ACPI_MCEJ_TRIGGER_ACTION_HEADER;

// MCEJ ACPI Table injection Action entry
typedef struct {
  UINT8                 Operation;
  UINT8                 Instruction;
  UINT8                 Flags;
  UINT8                 Reserved8;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  Register;
  UINT64                Value;
  UINT64                Mask;
} EFI_ACPI_MCEJ_ACTION_ITEM;

// MCEJ ACPI Trigger action entry table
typedef struct {
  EFI_ACPI_MCEJ_TRIGGER_ACTION_HEADER     Header;
  EFI_ACPI_MCEJ_ACTION_ITEM         Trigger[4];
} EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE;

// MCEJ ACPI Table
typedef struct {
  // ACPI Standard Header  
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  // MCA Bank Injection Header
  EFI_ACPI_MCEJ_MCA_BANK_ERROR_INJECTION_HEADER McBankInjHeader;
  // MCA bank Injection Action Table
  EFI_ACPI_MCEJ_ACTION_ITEM               actionEntryList[EFI_ACPI_MCEJ_ACTION_ENTRY_COUNT];
} EFI_ACPI_MCEJ_TABLE;

#pragma pack ()

extern EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;
extern EFI_RAS_SYSTEM_TOPOLOGY           *mRasTopology;
extern SYSTEM_RAS_SETUP                  *mRasSetup;
extern SYSTEM_RAS_CAPABILITY             *mRasCapability;

VOID
SetupInterface(
  );

EFI_STATUS
McejSwSmiCallBack (   
  IN  EFI_HANDLE                          DispatchHandle,
  IN  CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext, OPTIONAL
  IN OUT VOID                             *CommBuffer,     OPTIONAL
  IN OUT UINTN                            *CommBufferSize  OPTIONAL
  );

#endif 


/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2009-2010 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

---------------------------------------------------------------------------**/

