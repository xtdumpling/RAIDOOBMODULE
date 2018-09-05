/** @file
  This is an implementation of the Library RAS module.

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

  Copyright (c) 2009-2013 Intel Corporation.  All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license.  Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/



#ifndef _MP_SYNC_DATA_LIB_H_
#define _MP_SYNC_DATA_LIB_H_

//
// Includes
//
//
// Consumed protocols
//
#include <Uefi.h>
#include <Library/BaseLib.h>
//#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <PiDxe.h>
#include <PiSmm.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/IioUds.h>
#include <Protocol/MpService.h>
#include <Protocol/SmmBase2.h>
#include <Library/SynchronizationLib.h>
#include <Library/PcdLib.h>
#include <Protocol/CpuCsrAccess.h>



typedef volatile UINTN              SPIN_LOCK;
#define CPUID_TOPO_INFO                       0x0b
#define EFI_CPUID_VERSION_INFO                0x1
#ifndef EFI_CPUID_EXTENDED_TOPOLOGY
#define EFI_CPUID_EXTENDED_TOPOLOGY           0x0B
#endif

#ifndef EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_INVALID
#define EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_INVALID 0x0
#endif

#ifndef EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_SMT
#define EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_SMT 0x1
#endif

#ifndef EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_CORE
#define EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_CORE    0x2
#endif

#define EFI_CPUID_SIGNATURE                   0x0
#define CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_INVALID 0x0
#define CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_CORE    0x2
#define EFI_CPUID_CACHE_PARAMS                0x4
#define GETTHECPUNUMBERFAILED -1


#define MAXCOREBITS (5)
#define MAXTHREADBITS (6)


#define MAXCOREID (1<<MAXCOREBITS)
#define MAXCOREID_SIZE MAXCOREID

#define MAXTHREADID (1<<MAXTHREADBITS)
#define MAXTHREADID_SIZE MAXTHREADID


#ifndef EFI_DEADLOOP 
#define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); } 
#endif

/**  
  
**/
// APTIOV_SERVER_OVERRIDE_RC_START : Disabled RAS debug messages in Release mode
#ifdef EFI_DEBUG
// APTIOV_SERVER_OVERRIDE_RC_END : Disabled RAS debug messages in Release mode
#ifndef SILENT_MODE
#define RAS_DEBUG_EN 1
#else
#define RAS_DEBUG_EN 0
#endif // SILENT_MODE
// APTIOV_SERVER_OVERRIDE_RC_START : Disabled RAS debug messages in Release mode
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Disabled RAS debug messages in Release mode

#if RAS_DEBUG_EN
  #define _RAS_DEBUG_PRINT(PrintLevel, ...)              \
      do {                                             \
        if (DebugPrintEnabled() && DebugPrintLevelEnabled (PrintLevel)) {     \
          SerialPrintBlocking (PrintLevel, ##__VA_ARGS__);      \
        }                                              \
      } while (FALSE)
  // APTIOV_SERVER_OVERRIDE_RC_START
//#define RASDEBUG(Expression) _RAS_DEBUG_PRINT Expression  DEBUG
#define RASDEBUG(Expression) DebugPrint Expression
  // APTIOV_SERVER_OVERRIDE_RC_END
#else
  #define RASDEBUG(Expression)
#endif



typedef struct _CPUINFO {
  BOOLEAN Valid[MAXCOREID_SIZE][MAXTHREADID_SIZE];
  UINTN cpuNumber[MAXCOREID_SIZE][MAXTHREADID_SIZE];
  UINT32 ApicId[MAXCOREID_SIZE][MAXTHREADID_SIZE];
  UINT8 ApicVer[MAXCOREID_SIZE][MAXTHREADID_SIZE];
} CPUINFO;

typedef struct {
  EFI_AP_PROCEDURE          Procedure;
  UINTN                     CpuIndex;
  IN OUT  VOID              *ProcArguments;
} MP_SYNC_WRAPPER_ARGS_STRUCT;

typedef struct {
  SPIN_LOCK ApBusy;
  volatile BOOLEAN Present;
  MP_SYNC_WRAPPER_ARGS_STRUCT Args;
} MP_CORE_CPU_DATA;

//Make sure this is the same value as PcdCpuMaxLogicalProcessorNumber
#define MAXCPUDATA 512
typedef struct {
  MP_CORE_CPU_DATA            CpuData[MAXCPUDATA];
  VOLATILE UINTN              CpuCnt;
  SPIN_LOCK                    LoggingLock;
  SPIN_LOCK                     SerialLock;
} MP_CORE_SYNC_DATA;


/**
  Get the apicid for logical processor location. (Socket, Core, thread).

  @return apicid

**/
UINT32
GetApicID(
  VOID
);
/**
  Get the apicid for logical processor location. (Socket, Core, thread).

  @return apicid

**/
UINT32
GetApicIdByLoc(
  IN EFI_CPU_PHYSICAL_LOCATION *Location
);


/**
  Constructor of the library, called automatically by Uefi driver entry point
  @param ImageHandle   - Pointer to the loaded image protocol for this driver
  @param SystemTable   - Pointer to the EFI System Table

  @return Status

**/
EFI_STATUS
EFIAPI
MpsyncDataLibConstructor (
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
);

/**
  Return linear cpu number(logical thread) as seen by MpService and as used by SmmApStartup
  @param Socket Sockeid from apicid
  @param Core   CoreId from apicid
  @param thread  ThreadId from apicid
  @return Number
  @return -1 is an invalid thread, it means the function didn't find a logical thread for socket, core,thread

**/
UINTN
GetTheCpuNumber (
  UINT8 Socket,
  UINT8 Core,
  UINT8 Thread
  );

/**
  Get Core and socket for currently executing thread

**/
VOID
ExtractingNodeCoreFromApicId (
  OUT UINT8 *Socket,
  OUT UINT8 *Core
  );

/**
  Return if the executing thread is part of socket and core.
  @param Socket Sockeid from apicid
  @param Core   CoreId from apicid
  @return boolean

**/
BOOLEAN
ExecutingOnLocalNodeCore (
  UINT8 Socket,
  UINT8 Core
  );

/**
  Return if the executing thread is part of socket.
  @param Socket Sockeid from apicid
  @return boolean TRUE if executing thread is part of the socket

**/
BOOLEAN
ExcutingOnLocalNode (
  UINT8 Socket
  );

/**
  Execute function on all cores within the socket
  @param skt            Sockeid from apicid
  @param SmmApCallback  The address of the procedure to run
  @param ProcArguments  The parameter to pass to the procedure
  @return STATUS
  @retval EFI_INVALID_PARAMETER skt is not valid
  @retval EFI_INVALID_PARAMETER SmmApCallback is NULL
  @retval EFI_SUCCESS The procedure has been successfully scheduled
**/
EFI_STATUS
SmmStartupCoreApBlocking(
    IN UINT8            skt,
    IN EFI_AP_PROCEDURE  SmmApCallback,
    IN OUT  VOID       *ProcArguments 	
);

EFI_STATUS
SmmStartupCoreApBlkCnt(
  IN UINT8             skt,
  IN EFI_AP_PROCEDURE  SmmApCallback,
  IN OUT  VOID        *ProcArguments     
);


/**
  Execute function on all socket and cores in the system
  @param SmmApCallback  The address of the procedure to run
  @param ProcArguments  The parameter to pass to the procedure
  @return STATUS
  @retval EFI_INVALID_PARAMETER SmmApCallback is NULL
  @retval EFI_SUCCESS The procedure has been successfully scheduled
**/
EFI_STATUS
SmmStartupAllSocketAllCoreApBlocking(
    IN EFI_AP_PROCEDURE  SmmApCallback,
    IN OUT  VOID       *ProcArguments 	    
);

/**
  Execute function on at least one thread for the socket
  @param skt            Sockeid from apicid
  @param SmmApCallback  The address of the procedure to run
  @param ProcArguments  The parameter to pass to the procedure
  @return STATUS
  @retval EFI_INVALID_PARAMETER skt is not valid
  @retval EFI_INVALID_PARAMETER SmmApCallback is NULL
  @retval EFI_SUCCESS The procedure has been successfully scheduled
**/
EFI_STATUS
SmmStartupPackageApBlocking(
    IN UINT8            skt,
    IN EFI_AP_PROCEDURE	SmmApCallback,
    IN OUT  VOID       *ProcArguments 		
);

/**
  Use CPUID topology algorithm to get APICID bits occupied by thread and core.

  @param  ThreadIdBits   Number of bits occupied by Thread ID portion.
  @param  CoreIdBits     Number of bits occupied by Core ID portion.

**/
EFI_STATUS 
GetCoreAndSMTBits (
  IN UINTN *ThreadBits,
  IN UINTN *CoreBits
);

/**
  Execute function on at least one thread for all the sockets in the system
  @param skt            Sockeid from apicid
  @param SmmApCallback  The address of the procedure to run
  @param ProcArguments  The parameter to pass to the procedure
  @return STATUS
  @retval EFI_INVALID_PARAMETER SmmApCallback is NULL
  @retval EFI_SUCCESS The procedure has been successfully scheduled
**/
EFI_STATUS
SmmStartupAllSocketPackageApBlocking(
  IN EFI_AP_PROCEDURE   SmmApCallback,
  IN OUT  VOID        *ProcArguments 		
);

/**
  Execute function On the specified thread and wait for the thread to finish
  @param Procedure      The address of the procedure to run
  @param CpuIndex       Target CPU Index
  @param ProcArguments  The parameter to pass to the procedure
  @return STATUS
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
  Used to release sync semaphore for specified Ap
  @param CpuIndex       Target CPU Index
**/
VOID
MpSyncReleaseAp
(
IN UINTN                     CpuIndex
);

/**
  Return linear cpu number(logical thread) as seen by MpService and as used by SmmApStartup.
  Internally get the apicid for running thread and transform to the logical thread.

  @return Number
  @return -1 is an invalid thread, it means the function didn't find a logical thread for socket, core,thread

**/
UINTN
GetAPCpuNumber(
   VOID
);

/**
  Collects physical location of the processor.

  This function gets Package ID/Core ID/Thread ID of the processor.

  @param  ApicId         APIC ID for determining processor topology.
  @param  Location       Pointer to EFI_CPU_PHYSICAL_LOCATION structure.
**/
EFI_STATUS
ExtractProcessorLocation(
  IN UINT32 ApicId,
  OUT EFI_CPU_PHYSICAL_LOCATION *Location
);

/**
  Used by SmmBlockingStartupThisAp to wrap Procedure with the blocking semaphore logic.
  This allows the procedure to be implemented without knowledge of the blocking algorithm.
  This will avoid calls to MpSyncReleaseAp and also allows hiding the blocking algorithm implementation.

  This function gets Package ID/Core ID/Thread ID of the processor.

  @param  Buffer Pointer to internal structure with smmstartup parameters
**/
VOID EFIAPI 
MpSyncApWrapper(
  IN OUT VOID  *Buffer
);

VOID EFIAPI 
MpSyncApWrapperCnt(
  IN OUT VOID  *Buffer
);


VOID
EFIAPI
SerialPrintBlocking (
  IN  UINTN        ErrorLevel,
  IN  CONST CHAR8  *Format,
  ...
);




#endif  //_EMCA_LIB_H_

