//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Agent Module to load other modules to deploy SMM Entry Vector for X86 CPU.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#ifndef _CPU_PISMMCPUDXESMM_H_
#define _CPU_PISMMCPUDXESMM_H_
#include <PiSmm.h>
#include <AcpiCpuData.h>

#include <Protocol/MpService.h>
#include <Protocol/SmmConfiguration.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/SmmCpuSaveState.h>
#include <Protocol/SmmReadyToLock.h>
#include <Protocol/SmmCpuSync.h>
#include <Protocol/SmmCpuSync2.h>
#include <Protocol/SmmCpuService.h>

#include <Guid/AcpiS3Context.h>

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/SmmLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/MtrrLib.h>
#include <Library/SocketLga775Lib.h>
#include <Library/SocketLga1156Lib.h>
#include <Library/SmmCpuPlatformHookLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/HobLib.h>
#include <Library/CpuConfigLib.h>
#include <Library/LocalApicLib.h>
#include <Library/UefiCpuLib.h>
#include <Library/CpuExceptionHandlerLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Register/SmramSaveStateMap.h>
#include <Library/PeCoffGetEntryPointLib.h>

#include <CpuHotPlugData.h>

#include <Cpu/LockAlign.h>  // PURLEY_OVERRIDE_4929970
#include "SmmFeatures.h"
#include "CpuService.h"
#include "SmmProfile.h"
#include "SmmStateSaveRegister.h"

//
// Size of Task-State Segment defined in IA32 Manual
//
#define TSS_SIZE              104
#define TSS_X64_IST1_OFFSET   36
#define TSS_IA32_CR3_OFFSET   28
#define TSS_IA32_ESP_OFFSET   56

//
// Code select value
//
#define PROTECT_MODE_CODE_SEGMENT          0x08
#define SMMSEG_PROTECT_MODE_CODE_SEGMENT   0x10
#define LONG_MODE_CODE_SEGMENT             0x38

//
// The size 0x20 must be bigger than
// the size of template code of SmmInit. Currently,
// the size of SmmInit requires the 0x16 Bytes buffer
// at least.
// 
#define BACK_BUF_SIZE  0x20

#define EXCEPTION_VECTOR_NUMBER     0x20

#define INVALID_APIC_ID 0xFFFFFFFFFFFFFFFFULL

//
// SMM CPU synchronization features available on a processor
//
typedef struct {
  BOOLEAN          TargetedSmiSupported;
  BOOLEAN          DelayIndicationSupported;
  BOOLEAN          BlockIndicationSupported;
  //
  // This processor's LOG_PROC_EN bit used in SMM_ENABLE, SMM_DELAYED, and SMM_BLOCKED MSRs 
  // (introduced in Haswell processor). 
  // Value of "-1" indicates this field is invalid (i.e. LOG_PROC_EN bit is not
  // supported)
  //
  UINT64           HaswellLogProcEnBit; 
} SMM_CPU_SYNC_FEATURE;

typedef UINT32                              SMM_CPU_ARRIVAL_EXCEPTIONS;
#define ARRIVAL_EXCEPTION_BLOCKED           0x1
#define ARRIVAL_EXCEPTION_DELAYED           0x2
#define ARRIVAL_EXCEPTION_SMI_DISABLED      0x4

//
// Private structure for the SMM CPU module that is stored in DXE Runtime memory
// Contains the SMM Configuration Protocols that is produced.
// Contains a mix of DXE and SMM contents.  All the fields must be used properly.
//
#define SMM_CPU_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('s', 'c', 'p', 'u')

typedef struct {
  UINTN                           Signature;

  EFI_HANDLE                      SmmCpuHandle;

  EFI_PROCESSOR_INFORMATION       *ProcessorInfo;
  SMM_CPU_OPERATION               *Operation;
  UINTN                           *CpuSaveStateSize;
  VOID                            **CpuSaveState;
  UINT64                          *TstateMsr;
  SMM_CPU_SYNC_FEATURE            *SmmSyncFeature;
  UINT8                           *NonIioBusNum;

  EFI_SMM_RESERVED_SMRAM_REGION   SmmReservedSmramRegion[1];
  EFI_SMM_ENTRY_CONTEXT           SmmCoreEntryContext;
  EFI_SMM_ENTRY_POINT             SmmCoreEntry;

  EFI_SMM_CONFIGURATION_PROTOCOL  SmmConfiguration;
} SMM_CPU_PRIVATE_DATA;

extern SMM_CPU_PRIVATE_DATA  *gSmmCpuPrivate;
extern CPU_HOT_PLUG_DATA      mCpuHotPlugData;
extern UINTN                  mMaxNumberOfCpus;
extern UINTN                  mNumberOfCpus;
extern volatile BOOLEAN       *mRebased;
extern BOOLEAN                mSaveStateInMsr;
extern BOOLEAN                mSmmInitSaveStateInMsr;
extern UINT32                 gSmbase;
extern BOOLEAN                mRestoreSmmConfigurationInS3;
extern EFI_SMM_CPU_PROTOCOL   mSmmCpu;

//
// SMM CPU Protocol function prototypes.
//

/**
  Read information from the CPU save state.

  @param  This      EFI_SMM_CPU_PROTOCOL instance
  @param  Widthe    The number of bytes to read from the CPU save state.
  @param  Register  Specifies the CPU register to read form the save state.
  @param  CpuIndex  Specifies the zero-based index of the CPU save state
  @param  Buffer    Upon return, this holds the CPU register value read from the save state.

  @retval EFI_SUCCESS   The register was read from Save State
  @retval EFI_NOT_FOUND The register is not defined for the Save State of Processor  
  @retval EFI_INVALID_PARAMTER   This or Buffer is NULL.

**/
EFI_STATUS
EFIAPI
SmmReadSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL         *This,
  IN UINTN                              Width,
  IN EFI_SMM_SAVE_STATE_REGISTER        Register,
  IN UINTN                              CpuIndex,
  OUT VOID                              *Buffer
  );

/**
  Write data to the CPU save state.

  @param  This      EFI_SMM_CPU_PROTOCOL instance
  @param  Widthe    The number of bytes to read from the CPU save state.
  @param  Register  Specifies the CPU register to write to the save state.
  @param  CpuIndex  Specifies the zero-based index of the CPU save state
  @param  Buffer    Upon entry, this holds the new CPU register value.

  @retval EFI_SUCCESS   The register was written from Save State
  @retval EFI_NOT_FOUND The register is not defined for the Save State of Processor  
  @retval EFI_INVALID_PARAMTER   ProcessorIndex or Width is not correct

**/
EFI_STATUS
EFIAPI
SmmWriteSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL         *This,
  IN UINTN                              Width,
  IN EFI_SMM_SAVE_STATE_REGISTER        Register,
  IN UINTN                              CpuIndex,
  IN CONST VOID                         *Buffer
  );

//
// SMM CPU Sync Protocol and SMM CPU Sync2 Protocol function prototypes.
//

/**
  Given timeout constraint, wait for all APs to arrive. If this function returns EFI_SUCCESS, then
  no AP will execute normal mode code before entering SMM, so it is safe to access shared hardware resource 
  between SMM and normal mode. Note if there are SMI disabled APs, this function will return EFI_NOT_READY.


  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.

  @retval EFI_SUCCESS           No AP will execute normal mode code before entering SMM, so it is safe to access 
                                shared hardware resource between SMM and normal mode
  @retval EFI_NOT_READY         One or more CPUs may still execute normal mode code
  @retval EFI_DEVICE_ERROR      Error occured in obtaining CPU status.

**/
EFI_STATUS
EFIAPI
SmmCpuSyncCheckApArrival (
  IN    SMM_CPU_SYNC_PROTOCOL            *This
  );

/**
  Given timeout constraint, wait for all APs to arrive. If this function returns EFI_SUCCESS, then
  no AP will execute normal mode code before entering SMM, so it is safe to access shared hardware resource 
  between SMM and normal mode. Note if there are SMI disabled APs, this function will return EFI_NOT_READY.


  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.

  @retval EFI_SUCCESS           No AP will execute normal mode code before entering SMM, so it is safe to access 
                                shared hardware resource between SMM and normal mode
  @retval EFI_NOT_READY         One or more CPUs may still execute normal mode code
  @retval EFI_DEVICE_ERROR      Error occured in obtaining CPU status.

**/
EFI_STATUS
EFIAPI
SmmCpuSync2CheckApArrival (
  IN    SMM_CPU_SYNC2_PROTOCOL            *This
  );

/**
  Set SMM synchronization mode starting from the next SMI run.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              The SMM synchronization mode to be set and effective from the next SMI run.

  @retval EFI_SUCCESS           The function completes successfully.
  @retval EFI_INVALID_PARAMETER SynMode is not valid.

**/
EFI_STATUS
EFIAPI
SmmCpuSyncSetMode (
  IN    SMM_CPU_SYNC_PROTOCOL            *This,
  IN    SMM_CPU_SYNC_MODE                SyncMode
  );

/**
  Set SMM synchronization mode starting from the next SMI run.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              The SMM synchronization mode to be set and effective from the next SMI run.

  @retval EFI_SUCCESS           The function completes successfully.
  @retval EFI_INVALID_PARAMETER SynMode is not valid.

**/
EFI_STATUS
EFIAPI
SmmCpuSync2SetMode (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This,
  IN    SMM_CPU_SYNC_MODE                SyncMode
  );

/**
  Get current effective SMM synchronization mode.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              Output parameter. The current effective SMM synchronization mode.

  @retval EFI_SUCCESS           The SMM synchronization mode has been retrieved successfully.
  @retval EFI_INVALID_PARAMETER SyncMode is NULL.

**/
EFI_STATUS
EFIAPI
SmmCpuSyncGetMode (
  IN    SMM_CPU_SYNC_PROTOCOL            *This,
  OUT   SMM_CPU_SYNC_MODE                *SyncMode
  );

/**
  Get current effective SMM synchronization mode.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              Output parameter. The current effective SMM synchronization mode.

  @retval EFI_SUCCESS           The SMM synchronization mode has been retrieved successfully.
  @retval EFI_INVALID_PARAMETER SyncMode is NULL.

**/
EFI_STATUS
EFIAPI
SmmCpuSync2GetMode (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This,
  OUT   SMM_CPU_SYNC_MODE                *SyncMode
  );

/**
  Checks CPU SMM synchronization state

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  CpuIndex              Index of the CPU for which the state is to be retrieved.
  @param  CpuState              The state of the CPU

  @retval EFI_SUCCESS           Returns EFI_SUCCESS if the CPU's state is retrieved.
  @retval EFI_INVALID_PARAMETER Returns EFI_INVALID_PARAMETER if CpuIndex is invalid or CpuState is NULL
  @retval EFI_DEVICE_ERROR      Error occured in obtaining CPU status.
**/
EFI_STATUS
EFIAPI
SmmCpuSync2CheckCpuState (
  IN    SMM_CPU_SYNC2_PROTOCOL            *This,
  IN    UINTN                             CpuIndex,
  OUT   SMM_CPU_SYNC2_CPU_STATE           *CpuState               
  );

/**
  Change CPU's SMM enabling state.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  CpuIndex              Index of the CPU to enable / disable SMM
  @param  Enable                If TRUE, enable SMM; if FALSE disable SMM

  @retval EFI_SUCCESS           The CPU's SMM enabling state is changed.
  @retval EFI_INVALID_PARAMETER Returns EFI_INVALID_PARAMETER if CpuIndex is invalid
  @retval EFI_UNSUPPORTED       Returns EFI_UNSUPPORTED the CPU does not support dynamically enabling / disabling SMI
  @retval EFI_DEVICE_ERROR      Error occured in changing SMM enabling state.
**/
EFI_STATUS
EFIAPI
SmmCpuSync2ChangeSmmEnabling (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This,
  IN    UINTN                            CpuIndex,
  IN    BOOLEAN                          Enable
  );

/**
  Send SMI IPI to a specific CPU.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  CpuIndex              Index of the CPU to send SMI to.

  @retval EFI_SUCCESS           The SMI IPI is sent successfully.
  @retval EFI_INVALID_PARAMETER Returns EFI_INVALID_PARAMETER if CpuIndex is invalid
  @retval EFI_DEVICE_ERROR      Error occured in sending SMI IPI.
**/
EFI_STATUS
EFIAPI 
SmmCpuSync2SendSmi (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This,
  IN    UINTN                            CpuIndex
  );

/**
  Send SMI IPI to all CPUs excluding self

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.

  @retval EFI_SUCCESS           The SMI IPI is sent successfully.
  @retval EFI_INVALID_PARAMETER Returns EFI_INVALID_PARAMETER if CpuIndex is invalid
  @retval EFI_DEVICE_ERROR      Error occured in sending SMI IPI.
**/
EFI_STATUS
EFIAPI 
SmmCpuSync2SendSmiAllExcludingSelf (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This
  );

///
/// Structure used to describe a range of registers
///
typedef struct {
  EFI_SMM_SAVE_STATE_REGISTER  Start;
  EFI_SMM_SAVE_STATE_REGISTER  End;
  UINTN                        Length;
} CPU_SMM_SAVE_STATE_REGISTER_RANGE;

///
/// Structure used to build a lookup table to retrieve the widths and offsets
/// associated with each supported EFI_SMM_SAVE_STATE_REGISTER value
///

#define SMM_SAVE_STATE_REGISTER_SMMREVID_INDEX        1
#define SMM_SAVE_STATE_REGISTER_IOMISC_INDEX          2
#define SMM_SAVE_STATE_REGISTER_IOMEMADDR_INDEX       3
#define SMM_SAVE_STATE_REGISTER_MAX_INDEX             4

typedef struct {
  UINT8   Width32;
  UINT8   Width64;
  UINT16  Offset32;
  UINT16  Offset64Lo;
  UINT16  Offset64Hi;
  UINT32  MsrIndex;
  UINT8   OffsetInMsr;
  BOOLEAN Writeable;
} CPU_SMM_SAVE_STATE_LOOKUP_ENTRY;

///
/// Structure used to build a lookup table for the IOMisc width information
///
typedef struct {
  UINT8                        Width;
  EFI_SMM_SAVE_STATE_IO_WIDTH  IoWidth;
} CPU_SMM_SAVE_STATE_IO_WIDTH;

typedef struct {
  UINT32                      MsrIndex;
  UINT64                      MsrValue;
} MSR_INFO;

//
//
//
typedef struct {
  UINT32                            Offset;
  UINT16                            Segment;
  UINT16                            Reserved;
} IA32_FAR_ADDRESS;

#define SMM_PSD_OFFSET              0xfb00

typedef struct {
  UINT64                            Signature;              // Offset 0x00
  UINT16                            Reserved1;              // Offset 0x08
  UINT16                            Reserved2;              // Offset 0x0A
  UINT16                            Reserved3;              // Offset 0x0C
  UINT16                            SmmCs;                  // Offset 0x0E
  UINT16                            SmmDs;                  // Offset 0x10
  UINT16                            SmmSs;                  // Offset 0x12
  UINT16                            SmmOtherSegment;        // Offset 0x14
  UINT16                            Reserved4;              // Offset 0x16
  UINT64                            Reserved5;              // Offset 0x18
  UINT64                            Reserved6;              // Offset 0x20
  UINT64                            Reserved7;              // Offset 0x28
  UINT64                            SmmGdtPtr;              // Offset 0x30
  UINT32                            SmmGdtSize;             // Offset 0x38
  UINT32                            Reserved8;              // Offset 0x3C
  UINT64                            Reserved9;              // Offset 0x40
  UINT64                            Reserved10;             // Offset 0x48
  UINT16                            Reserved11;             // Offset 0x50
  UINT16                            Reserved12;             // Offset 0x52
  UINT32                            Reserved13;             // Offset 0x54
  UINT64                            MtrrBaseMaskPtr;        // Offset 0x58
} PROCESSOR_SMM_DESCRIPTOR;


extern IA32_FAR_ADDRESS             gSmmJmpAddr;

extern CONST UINT8                  gcSmmInitTemplate[];
extern CONST UINT16                 gcSmmInitSize;
extern UINT32                       gSmmCr0;
extern UINT32                       gSmmCr3;
extern UINT32                       gSmmCr4;
extern UINTN                        gSmmInitStack;

/**
  Seamphore operation for all processor relocate SMMBase.
**/
VOID
EFIAPI
SmmRelocationSemaphoreComplete (
  VOID
  );

/**
  Hook return address of SMM Save State so that semaphore code
  can be executed immediately after AP exits SMM to indicate to
  the BSP that an AP has exited SMM after SMBASE relocation.

  @param CpuIndex  The processor index.
**/
VOID
SemaphoreHook (
  IN UINTN  CpuIndex
  );

///
/// The type of SMM CPU Information
///
typedef struct {
// PURLEY_OVERRIDE_BEGIN_4929970
  ALIGN128 SPIN_LOCK                Busy;
  ALIGN128 volatile UINT32          Run;
  ALIGN128 volatile BOOLEAN         Present;
  UINT8                             PadForAlign[128-sizeof(BOOLEAN)];
// PURLEY_OVERRIDE_END_4929970
  volatile EFI_AP_PROCEDURE         Procedure;
  volatile VOID                     *Parameter;
} SMM_CPU_DATA_BLOCK;

typedef struct {
  //
  // Pointer to an array. The array should be located immediately after this structure
  // so that UC cache-ability can be set together.
  //
  SMM_CPU_DATA_BLOCK            *CpuData;
// PURLEY_OVERRIDE_BEGIN_4929970
  ALIGN128 volatile UINT32      Counter;
  ALIGN128 volatile BOOLEAN     InsideSmm;
  ALIGN128 volatile BOOLEAN     AllCpusInSync;
  UINT8                         PadForAlign[128-sizeof(SPIN_LOCK)];
// PURLEY_OVERRIDE_END_4929970
  volatile UINT32               BspIndex;
  SMM_CPU_SYNC_MODE             SyncModeToBeSet;
  volatile SMM_CPU_SYNC_MODE    EffectiveSyncMode;
  volatile BOOLEAN              SwitchBsp;
  volatile BOOLEAN              *CandidateBsp;
} SMM_DISPATCHER_MP_SYNC_DATA;

typedef struct {
// PURLEY_OVERRIDE_BEGIN_4929970
  ALIGN128 SPIN_LOCK    SpinLock;
  UINT8                 PadForAlign[128-sizeof(SPIN_LOCK)];
  UINT32                MsrIndex;
// PURLEY_OVERRIDE_END_4929970
} MP_MSR_LOCK;

extern IA32_DESCRIPTOR                     gcSmiGdtr;
extern IA32_DESCRIPTOR                     gcSmiIdtr;
extern UINT32                              gSmiCr3;
extern volatile UINT32                     gSmiStack;
extern CONST PROCESSOR_SMM_DESCRIPTOR      gcPsd;
extern volatile UINT8                      gcSmiHandlerTemplate[];
extern CONST UINT16                        gcSmiHandlerSize;
extern UINT64                              gPhyMask;
extern ACPI_CPU_DATA                       mAcpiCpuData;
extern SMM_DISPATCHER_MP_SYNC_DATA         *mSmmMpSyncData;
extern VOID                                *mGdtForAp;
extern VOID                                *mIdtForAp;
extern VOID                                *mMachineCheckHandlerForAp;
extern UINTN                               mSmmStackArrayBase;
extern UINTN                               mSmmStackArrayEnd;
extern UINTN                               mSmmStackSize;
extern EFI_SMM_CPU_SERVICE_PROTOCOL        mSmmCpuService;
extern IA32_DESCRIPTOR                     gcSmiInitGdtr;
extern IA32_DESCRIPTOR                     mSmiPMIdtr;
extern UINT32                              gSmiExceptionCr3;
extern volatile UINT8                      gcSmiPMHandlerTemplate[];
extern CONST UINT16                        gcSmiPMHandlerSize;
extern UINT32                              gProtModeIdtr;
extern UINT32                              gProtModeSmbase;
extern BOOLEAN                             mSmmProtectedModeEnable;
extern BOOLEAN                             mSmmRuntimeCtlHooksEnable;


/**
  Create 4G PageTable in SMRAM.

  @param          ExtraPages       Additional page numbers besides for 4G memory
  @return         PageTable Address

**/
UINT32
Gen4GPageTable (
  IN      UINTN                     ExtraPages
  );


/**
  Initialize global data for MP synchronization.

  @param ImageHandle  File Image Handle.
  @param Stacks       Base address of SMI stack buffer for all processors.
  @param StackSize    Stack size for each processor in SMM.

**/
VOID
InitializeMpServiceData (
  IN EFI_HANDLE  ImageHandle,
  IN VOID        *Stacks,
  IN UINTN       StackSize
  );

/**
  Initialize Timer for Smm AP Sync.

**/
VOID
InitializeSmmTimer (
  VOID
  );

/**
  Start Timer for Smm AP Sync.

**/
UINT64
EFIAPI
StartSyncTimer (
  VOID
  );

/**
  Check if the Smm AP Sync timer is timeout.

  @param Timer  The start timer from the begin.

**/
BOOLEAN
EFIAPI
IsSyncTimerTimeout (
  IN      UINT64                    Timer
  );

/**
  Initialize IDT for SMM Stack Guard.

**/
VOID
EFIAPI
InitializeIDTSmmStackGuard (
  VOID
  );

/**

  Register the SMM Foundation entry point.

  @param          This              Pointer to EFI_SMM_CONFIGURATION_PROTOCOL instance
  @param          SmmEntryPoint     SMM Foundation EntryPoint

  @retval         EFI_SUCCESS       Successfully to register SMM foundation entry point

**/
EFI_STATUS
EFIAPI
RegisterSmmEntry (
  IN CONST EFI_SMM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_SMM_ENTRY_POINT                   SmmEntryPoint
  );

/**
  Create PageTable for SMM use.

  @return     PageTable Address

**/
UINT32
SmmInitPageTable (
  VOID
  );

/**
  Schedule a procedure to run on the specified CPU.

  @param   Procedure        The address of the procedure to run
  @param   CpuIndex         Target CPU number
  @param   ProcArguments    The parameter to pass to the procedure

  @retval   EFI_INVALID_PARAMETER    CpuNumber not valid
  @retval   EFI_INVALID_PARAMETER    CpuNumber specifying BSP
  @retval   EFI_INVALID_PARAMETER    The AP specified by CpuNumber did not enter SMM
  @retval   EFI_INVALID_PARAMETER    The AP specified by CpuNumber is busy
  @retval   EFI_SUCCESS - The procedure has been successfully scheduled

**/
EFI_STATUS
EFIAPI
SmmStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuIndex,
  IN OUT  VOID                      *ProcArguments OPTIONAL
  );

/**
  Schedule a procedure to run on the specified CPU in a blocking fashion.

  @param  Procedure                The address of the procedure to run
  @param  CpuIndex                 Target CPU Index
  @param  ProcArguments            The parameter to pass to the procedure

  @retval EFI_INVALID_PARAMETER    CpuNumber not valid
  @retval EFI_INVALID_PARAMETER    CpuNumber specifying BSP
  @retval EFI_INVALID_PARAMETER    The AP specified by CpuNumber did not enter SMM
  @retval EFI_INVALID_PARAMETER    The AP specified by CpuNumber is busy
  @retval EFI_SUCCESS              The procedure has been successfully scheduled

**/
EFI_STATUS
EFIAPI
SmmBlockingStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuIndex,
  IN OUT  VOID                      *ProcArguments OPTIONAL
  );

/**
  Initialize MP synchronization data.

**/
VOID
EFIAPI
InitializeMpSyncData (
  VOID
  );

/**

  Find out SMRAM information including SMRR base, SMRR size and IEDBase.

  @param          SmrrBase          SMRR base
  @param          SmrrSize          SMRR size
  @param          IedBase           IED Base
  
**/
VOID
FindSmramInfo (
  OUT UINT32   *SmrrBase,
  OUT UINT32   *SmrrSize,
  OUT UINT32   *IedBase
  );

/**
  The function is invoked before SMBASE relocation in S3 path to restors CPU status.

  The function is invoked before SMBASE relocation in S3 path. It does first time microcode load 
  and restores MTRRs for both BSP and APs.

**/
VOID
EarlyInitializeCpu (
  VOID
  );

/**
  The function is invoked after SMBASE relocation in S3 path to restors CPU status.

  The function is invoked after SMBASE relocation in S3 path. It restores configuration according to 
  data saved by normal boot path for both BSP and APs.

**/
VOID
InitializeCpu (
  VOID
  );

/**
  Page Fault handler for SMM use.

  @param  InterruptType    Defines the type of interrupt or exception that
                           occurred on the processor.This parameter is processor architecture specific.
  @param  SystemContext    A pointer to the processor context when
                           the interrupt occurred on the processor.
**/
VOID
EFIAPI
SmiPFHandler (
    IN EFI_EXCEPTION_TYPE   InterruptType,
    IN EFI_SYSTEM_CONTEXT   SystemContext
  );

/**
  Restore SMM Configuration for Haswell enhance SMM features in S3 boot path.
**/
VOID
RestoreSmmConfigurationInS3 (
  VOID
  );

/**
  Read a CPU Save State register on the target processor.

  This function abstracts the differences that whether the CPU Save State register is in the IA32 Cpu Save State Map
  or x64 Cpu Save State Map or a CPU Save State MSR.

  This function supports reading a CPU Save State register in SMBase relocation handler.

  @param[in]  CpuIndex       Specifies the zero-based index of the CPU save state.
  @param[in]  RegisterIndex  Index into mSmmCpuWidthOffset[] look up table.
  @param[in]  Width          The number of bytes to read from the CPU save state.
  @param[out] Buffer         Upon return, this holds the CPU register value read from the save state.

  @retval EFI_SUCCESS           The register was read from Save State.
  @retval EFI_NOT_FOUND         The register is not defined for the Save State of Processor.
  @retval EFI_INVALID_PARAMTER  This or Buffer is NULL.
  @retval EFI_UNSUPPORTED       The register has no corresponding MSR.

**/
EFI_STATUS
ReadSaveStateRegister (
  IN UINTN  CpuIndex,
  IN UINTN  RegisterIndex,
  IN UINTN  Width,
  OUT VOID  *Buffer
  );

/**
  Write value to a CPU Save State register on the target processor.

  This function abstracts the differences that whether the CPU Save State register is in the IA32 Cpu Save State Map
  or x64 Cpu Save State Map or a CPU Save State MSR.

  This function supports writing a CPU Save State register in SMBase relocation handler.

  @param[in] CpuIndex       Specifies the zero-based index of the CPU save state.
  @param[in] RegisterIndex  Index into mSmmCpuWidthOffset[] look up table.
  @param[in] Width          The number of bytes to read from the CPU save state.
  @param[in] Buffer         Upon entry, this holds the new CPU register value.

  @retval EFI_SUCCESS           The register was written to Save State.
  @retval EFI_NOT_FOUND         The register is not defined for the Save State of Processor.  
  @retval EFI_INVALID_PARAMTER  ProcessorIndex or Width is not correct.
  @retval EFI_UNSUPPORTED       The register is read-only, cannot be written, or has no corresponding MSR.

**/
EFI_STATUS
WriteSaveStateRegister (
  IN UINTN       CpuIndex,
  IN UINTN       RegisterIndex,
  IN UINTN       Width,
  IN CONST VOID  *Buffer
  );

/**
  Read information from the CPU save state.

  @param  Register  Specifies the CPU register to read form the save state.

  @retval 0   Register is not valid
  @retval >0  Index into mSmmCpuWidthOffset[] associated with Register

**/
UINTN
GetRegisterIndex (
  IN EFI_SMM_SAVE_STATE_REGISTER  Register
  );

/**
  Perform the remaining tasks.

**/
VOID
PerformRemainingTasks (
  VOID
  );

/**
  Initialize SMM Protected Mode IDT table.

**/
VOID
InitProtectedModeIdt (
  VOID
  );

/**
  Initialize MSR spin lock by MSR index.
  
  @param  MsrIndex       MSR index value.

**/
VOID
InitMsrSpinLockByIndex (
  IN UINT32      MsrIndex
  );

/**
  Search module name by input IP address and output it.

  @param CallerIpAddress   Caller instruction pointer.

**/
VOID
DumpModuleInfoByIp (
  IN  UINTN              CallerIpAddress
  );

#endif
