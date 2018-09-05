
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009-2015 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file WheaErrorInj.h

    This is an implementation of the Whea Support protocol.

---------------------------------------------------------------------**/


#ifndef _WHEA_CORE_EINJ_
#define _WHEA_CORE_EINJ_

//
// Includes
//
//
// Consumed protocols
//
#include <Uefi.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/WheaPlatformHooksLib.h>
//#include <Library/ProcMemInitLib.h>
#include <Protocol/CrystalRidge.h>
#include <Protocol/IioUds.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/WheaPlatformSupport.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/CpuCsrAccess.h>
#include <Guid/MemoryConfigData.h>
#include <Guid/SetupVariable.h>
#include <Library/SynchronizationLib.h>
#include <Library/ProcMemErrReporting.h>
#include <Protocol/SmmCpuService.h>
#include <Protocol/SmmCpu.h>
#include <Cpu/CpuBaseLib.h>
#include <Library/mpsyncdatalib.h>
#include "Library/WheaSiliconHooksLib/WheaSiliconHooksLib.h"
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Include/Library/MeUmaEinjLib.h>
#include <Library/MmPciBaseLib.h>
#include <Include/MeUma.h>

//
// Data & structure definition
//

// WHEA error injection action definitions
#define WHEA_EINJ_BEGIN_INJECT_OP                               0x00
#define WHEA_EINJ_GET_TRIGGER_ACTION_TABLE                      0x01
#define WHEA_EINJ_SET_ERROR_TYPE                                0x02
#define WHEA_EINJ_GET_ERROR_TYPE                                0x03
#define WHEA_EINJ_END_INJECT_OP                                 0x04
#define WHEA_EINJ_EXECUTE_INJECT_OP                             0x05
#define WHEA_EINJ_CHECK_BUSY_STATUS                             0x06
#define WHEA_EINJ_GET_CMD_STATUS                                0x07
#define WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRSS                    0x08
#define WHEA_INSTRUCTION_CFLUSH                                 0x05

// SSG requested to validate the plug-in with error type 0x1000(bit12 set) to inject memory error
#define WHEA_TEMP_MEM_ERROR_TYPE                                0x1000  // for IFU , DCU error injections
#define WHEA_VENDOR_EXT_TYPE                                    0x80000000  // Intel specified OEM defined errro types
  #define WHEA_MEM_ERROR_REC_TYPE                               BIT0
  #define WHEA_PATROL_SCRUB_ERROR                               BIT1
  #define WHEA_UMA_ERROR_TYPE                                   BIT2

  #define WHEA_MESEG_COR_ERR_INJ_CONSUME_NOW                    BIT0
  #define WHEA_MESEG_COR_ERR_INJ_CONSUME_1MS                    BIT1
  #define WHEA_MESEG_COR_ERR_INJ_CONSUME_G0_S1_RW               BIT2
  #define WHEA_MESEG_COR_ERR_INJ_CONSUME_RESET_WARN             BIT3
  #define WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_NOW                  BIT4
  #define WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_1MS                  BIT5
  #define WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_G0_S1_RW             BIT6
  #define WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_RESET_WARN           BIT7
  #define WHEA_MESEG_ICV_ERR_INJ_CONSUME_NOW                    BIT8
  #define WHEA_MESEG_ICV_ERR_INJ_CONSUME_1MS                    BIT9
  #define WHEA_MESEG_ICV_ERR_INJ_CONSUME_G0_S1_RW               BIT10
  #define WHEA_MESEG_ICV_ERR_INJ_CONSUME_RESET_WARN             BIT11
// PCI-EX error injection bits
#define CAUSE_CTOERR_BIT                04  // UnCorrectable error injection test
#define CAUSE_RCVERR_BIT                02  //  Correctable error injection test
#define ERRINJDIS_BIT                   01  //  Error injection disable bit

// Error consume types
#define WHEA_NO_CONSUME                 0x00
#define WHEA_REC_PATROLSCRUB_ERROR      0x01
#define WHEA_CORE_IFU_DCU_ERR           0x02
#define WHEA_MEM_COR_ERROR              0x03

// EINJ - SET_ERROR_TYPE_WITH_ADDRESS Data Structure flags
#define WHEA_SET_ERROR_TYPE_WITH_PROC_APIC_VALID      0x01
#define WHEA_SET_ERROR_TYPE_WITH_MEMORY_ADDRESS_VALID 0x02
#define WHEA_SET_ERROR_TYPE_WITH_SBDF_VALID           0x04

// BIOS runtime Address range used for error injection
#define WHEA_EINJ_ADDR_RANGE                            0x4000
// Error injection operation definitions
#define EINJ_BEGIN_OPERATION                            0x55AA55AA
#define EINJ_END_OPERATION                              0x00000000

// Error injection operation status
#define WHEA_EINJ_OPERATION_BUSY                        0x01
#define WHEA_EINJ_CMD_SUCCESS                           0x00
#define WHEA_EINJ_CMD_UNKNOWN_FAILURE                   0x01
#define WHEA_EINJ_CMD_INVALID_ACCESS                    0x02

// Port for triggering SMI
#define R_APM_CNT                                         0xB2
// Error injection SW SMI defintions
#define EFI_WHEA_EINJ_IIO_ERR_INJ_LCK_SWSMI               0x99
#define EFI_WHEA_EINJ_EXECUTE_SWSMI                       0x9A
#define EFI_WHEA_EINJ_PATROL_SCRUB_SWSMI                  0x9B
#define EFI_WHEA_ME_EINJ_CONSUME_SWSMI                    0x98


//#define B0_REV_IVT  0x02
//#define B0_REV_HSX  0x01

#define   VID_1_16_2_CFG_REG   0x20182000
#define   RID_1_16_2_CFG_REG   0x10182008
#define   DID_1_16_2_CFG_REG   0x20182002
/********** NEED TO BE PORTED FOR PLATFORM ****
#define UBOX_DEV_NUM    0x00
#define WBOX_DEV_NUM    0x01

#define MBOX0_DEV_NUM   0x05
#define MBOX1_DEV_NUM   0x07
UINT8 MBOX_DEV_NUM[] =  {MBOX0_DEV_NUM, MBOX1_DEV_NUM};
**********/

#define MC_ERR_INJ_LCK          0x790
#define MC_PS_INT_DOWNGRADE     0x17f
#define V_PS_INT_DOWNGRADE     0x4  // BIT2
#define MSR_MCG_CONTAIN        0x178

#define   RSP_FUNC_ADDR_MATCH_LO_1_15_2_CFG_REG  0x4017a1c0
#define   RSP_FUNC_ADDR_MATCH_HI_1_15_2_CFG_REG  0x4017a1c4
#define   RSP_FUNC_ADDR_MASK_LO_1_15_2_CFG_REG   0x4017a1c8
#define   RSP_FUNC_ADDR_MASK_HI_1_15_2_CFG_REG   0x4017a1cc

#define   RSP_FUNC_ADDR_MATCH_LO_1_15_3_CFG_REG  0x4017b1c0
#define   RSP_FUNC_ADDR_MATCH_HI_1_15_3_CFG_REG  0x4017b1c4
#define   RSP_FUNC_ADDR_MASK_LO_1_15_3_CFG_REG   0x4017b1c8
#define   RSP_FUNC_ADDR_MASK_HI_1_15_3_CFG_REG   0x4017b1cc

#define   RSP_FUNC_ADDR_MATCH_LO_1_15_4_CFG_REG  0x4017c1c0
#define   RSP_FUNC_ADDR_MATCH_HI_1_15_4_CFG_REG  0x4017c1c4
#define   RSP_FUNC_ADDR_MASK_LO_1_15_4_CFG_REG   0x4017c1c8
#define   RSP_FUNC_ADDR_MASK_HI_1_15_4_CFG_REG   0x4017c1cc

#define   RSP_FUNC_ADDR_MATCH_LO_1_15_5_CFG_REG  0x4017d1c0
#define   RSP_FUNC_ADDR_MATCH_HI_1_15_5_CFG_REG  0x4017d1c4
#define   RSP_FUNC_ADDR_MASK_LO_1_15_5_CFG_REG   0x4017d1c8
#define   RSP_FUNC_ADDR_MASK_HI_1_15_5_CFG_REG   0x4017d1cc

#define   RSP_FUNC_CRC_ERR_INJ_EXTRA_1_16_2_CFG_REG   0x40182208
#define   RSP_FUNC_CRC_ERR_INJ_EXTRA_1_16_3_CFG_REG   0x40183208
#define   RSP_FUNC_CRC_ERR_INJ_EXTRA_1_16_6_CFG_REG   0x40186208
#define   RSP_FUNC_CRC_ERR_INJ_EXTRA_1_16_7_CFG_REG   0x40187208

#define   RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_1_16_2_CFG_REG   0x40182200
#define   RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_1_16_3_CFG_REG   0x40183200
#define   RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_1_16_6_CFG_REG   0x40186200
#define   RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_1_16_7_CFG_REG   0x40187200

#define   RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_1_16_2_CFG_REG   0x40182204
#define   RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_1_16_3_CFG_REG   0x40183204
#define   RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_1_16_6_CFG_REG   0x40186204
#define   RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_1_16_7_CFG_REG   0x40187204

#define   SADDBGMM0_1_12_0_CFG_REG   0x40160064
#define   SADDBGMM1_1_12_0_CFG_REG   0x40160068
#define   SADDBGMM2_1_12_0_CFG_REG   0x4016006c
#define   SADDBGMM0_1_12_1_CFG_REG   0x40161064
#define   SADDBGMM1_1_12_1_CFG_REG   0x40161068
#define   SADDBGMM2_1_12_1_CFG_REG   0x4016106c
#define   SADDBGMM0_1_12_2_CFG_REG   0x40162064
#define   SADDBGMM1_1_12_2_CFG_REG   0x40162068
#define   SADDBGMM2_1_12_2_CFG_REG   0x4016206c
#define   SADDBGMM0_1_12_3_CFG_REG   0x40163064
#define   SADDBGMM1_1_12_3_CFG_REG   0x40163068
#define   SADDBGMM2_1_12_3_CFG_REG   0x4016306c
#define   SADDBGMM0_1_13_0_CFG_REG   0x40168064
#define   SADDBGMM1_1_13_0_CFG_REG   0x40168068
#define   SADDBGMM2_1_13_0_CFG_REG   0x4016806c
#define   SADDBGMM0_1_13_1_CFG_REG   0x40169064
#define   SADDBGMM1_1_13_1_CFG_REG   0x40169068
#define   SADDBGMM2_1_13_1_CFG_REG   0x4016906c
#define   SADDBGMM0_1_13_2_CFG_REG   0x4016a064
#define   SADDBGMM1_1_13_2_CFG_REG   0x4016a068
#define   SADDBGMM2_1_13_2_CFG_REG   0x4016a06c
#define   SADDBGMM0_1_13_3_CFG_REG   0x4016b064
#define   SADDBGMM1_1_13_3_CFG_REG   0x4016b068
#define   SADDBGMM2_1_13_3_CFG_REG   0x4016b06c

#define   CBO_DBG_BUS_MATCH_1_12_0_CFG_REG   0x40160084
#define   CBO_DBG_BUS_MATCH_1_12_1_CFG_REG   0x40161084
#define   CBO_DBG_BUS_MATCH_1_12_2_CFG_REG   0x40162084
#define   CBO_DBG_BUS_MATCH_1_12_3_CFG_REG   0x40163084
#define   CBO_DBG_BUS_MATCH_1_13_0_CFG_REG   0x40168084
#define   CBO_DBG_BUS_MATCH_1_13_1_CFG_REG   0x40169084
#define   CBO_DBG_BUS_MATCH_1_13_2_CFG_REG   0x4016a084
#define   CBO_DBG_BUS_MATCH_1_13_3_CFG_REG   0x4016b084

#define   CBO_GLB_RSP_CNTRL_1_12_0_CFG_REG   0x40160094
#define   CBO_GLB_RSP_CNTRL_1_12_1_CFG_REG   0x40161094
#define   CBO_GLB_RSP_CNTRL_1_12_2_CFG_REG   0x40162094
#define   CBO_GLB_RSP_CNTRL_1_12_3_CFG_REG   0x40163094
#define   CBO_GLB_RSP_CNTRL_1_13_0_CFG_REG   0x40168094
#define   CBO_GLB_RSP_CNTRL_1_13_1_CFG_REG   0x40169094
#define   CBO_GLB_RSP_CNTRL_1_13_2_CFG_REG   0x4016a094
#define   CBO_GLB_RSP_CNTRL_1_13_3_CFG_REG   0x4016b094

#define ENTRY_INVALID   0xFF

#pragma pack (1)

#define EinjRegisterExecute { \
  EFI_ACPI_3_0_SYSTEM_IO,     \
  0x10,                        \
  0x00,                        \
  EFI_ACPI_3_0_WORD,          \
  R_APM_CNT \
  }

#define EinjRegisterFiller  { \
  EFI_ACPI_3_0_SYSTEM_MEMORY, \
  64,                         \
  0,                          \
  EFI_ACPI_3_0_QWORD,         \
  0}



VOID *
EFIAPI
AsmFlushCacheLine (
  IN      VOID                      *LinearAddress
  );

VOID *
EFIAPI
AsmCommitLine (
  IN VOID    *LinearAddress,
  IN UINT32  Value
  );


VOID
EFIAPI
AsmWbinvd (
  VOID
  );

VOID
AsmNop (
  VOID
  );

VOID
TurnOffCache (
  VOID
  );

VOID
TurnOnCache (
  VOID
  );

VOID
InjectionFlow (
  VOID
  );

EFI_STATUS
UpdateEinj (
  VOID
  );

VOID
DisablePsmi (
  VOID
  );

VOID
DisablePrefetch (
  VOID
  );

VOID
EnablePrefetch (
  VOID
  );


/**
  Set MC9_CTL[bit 5] to 0 before ME Error Injection
  
  @param NONE

  @retval NONE
**/
VOID
PreMC9CtlSetupForMEErrInj (
  VOID
  );

/**
  Set MC9_CTL[bit 5] to 1 after ME Error Injection
  
  @param NONE

  @retval NONE
**/
VOID
PostMC9CtlSetupForMEErrInj (
  VOID
  );

/**
  Do the pre-setup for ME Error Injection
  
  @param NONE

  @retval NONE
**/
VOID
PreMESegErrInjSetup (
  VOID
  );


/**
  Do the post-setup for ME Error Injection
  
  @param NONE

  @retval NONE
**/
VOID
PostMESegErrInjSetup (
  VOID
  );

  /**
  Corrupt the data the ME Seg Injection address offset
  
  @param NONE

  @retval NONE
**/
VOID
CorruptMESegInjOffset ( 
  VOID
);

/**
 Set up Trigger Action Table for ME Seg ERROr Injection
  
  @param NONE

  @retval NONE
**/
VOID
SetupMESegTriggerActionTable (
  VOID
  );

/**

  Call ME interface to get a page offset for ME Error Injection
  
  @param NONE

  @retval EFI_SUCCESS:              ME Page allocated Successfully
  @retval EFI_OUT_OF_RESOURCES:     ME Page cannot be allocated
**/
EFI_STATUS
GetMEPageForErrorInj (
  VOID
  );

/**

  SW SMI handler registered to call the ME libray to initiate the Error consumption

  @param DispatchHandle     - The unique handle assigned to this handler by SmiHandlerRegister().Type
                                            EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.1 Specification.
  @param DispatchContext    - Points to the optional handler context which was specified when the handler was registered.
  @param CommBuffer         - A pointer to a collection of data in memory that will be conveyed from a non-SMM
                                           environment into an SMM environment. The buffer must be contiguous, physically mapped, and be a physical address.
  @param CommBufferSize     - The size of the CommBuffer. 

  @retval EFI_SUCCESS - The interrupt was handled and quiesced. No other handlers should still be called.
    
**/
EFI_STATUS
WheaMeEinjSwSmiHanlder (
  IN  EFI_HANDLE                          DispatchHandle,
  IN  CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext, OPTIONAL
  IN OUT VOID                             *CommBuffer,     OPTIONAL
  IN OUT UINTN                            *CommBufferSize  OPTIONAL
  );


/**
  Periodic SMI handler to get the status from ME Seg Error consumption.
  ME will provide the status in around 5 ms after the error consumption is initiated

  @param Handle             - The unique handle assigned to this handler by SmiHandlerRegister().Type
                                            EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.1 Specification.
  @param mPeriodicTimer     -  Pointer to the DispatchContext

  @param CommBuffer         - A pointer to a collection of data in memory that will be conveyed from a non-SMM
                                           environment into an SMM environment. The buffer must be contiguous, physically mapped, and be a physical address.
  @param CommBufferSize     - The size of the CommBuffer. 

  @retval EFI_SUCCESS       - The interrupt was handled and quiesced. No other handlers should still be called.

**/
EFI_STATUS
GetMeEInjConsumeStatus (
  IN  EFI_HANDLE     Handle,
  IN  CONST VOID     *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer  OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  );

#pragma pack ()

#endif  //_WHEA_CORE_EINJ_
