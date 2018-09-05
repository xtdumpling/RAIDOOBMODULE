/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2017 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
    MpSyncDatalib.c

Abstract:
    This is an implementation of the MpSyncDatalib Library for RAS module

-----------------------------------------------------------------------------*/

#include <Library/mpsyncdatalib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Protocol/SmmCpuService.h>
#include <Library/DebugLib.h>


MP_CORE_SYNC_DATA             mMpSyncData;
CPUINFO         mCpuInfo[MAX_SOCKET];

EFI_CPU_CSR_ACCESS_PROTOCOL  *mCpuCsrAccess_syncdata = NULL;
IIO_UDS                      *mIioUdsPtr_syncdata = NULL;
EFI_SMM_BASE2_PROTOCOL       *mSmmBase_syncdata = NULL;
EFI_SMM_SYSTEM_TABLE2        *mSmst_syncdata = NULL;
EFI_SMM_CPU_SERVICE_PROTOCOL *mSmmCpuServiceProtocol = NULL;


UINT8 
GetMaxThreads (
  VOID
  )
{
  UINTN   ThreadBits;
  UINTN   CoreBits;
  
  GetCoreAndSMTBits(&ThreadBits,&CoreBits);
  return (UINT8)(1 << ThreadBits);  
}

STATIC EFI_STATUS 
FindValidThread(
IN UINT8 skt,
IN UINT8 core,
OUT UINT8 *thread
)
{
  EFI_STATUS Status;
  UINT8 t;
  UINT8 MaxThread;
  
  if(thread == NULL || skt >= MAX_SOCKET || core >= MAXCOREID ) {
    return EFI_INVALID_PARAMETER;
  }
  Status = EFI_NOT_FOUND;
  MaxThread = GetMaxThreads();
  for(t=0; t < MaxThread;t++) {
    if(mCpuInfo[skt].Valid[core][t]) {
      *thread = t;
       Status = EFI_SUCCESS;
       break;
    }
  }
  return Status;
}

EFI_STATUS
SmmCntStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuIndex,
  IN OUT  VOID                      *ProcArguments OPTIONAL
  )

{
  EFI_STATUS Status = EFI_SUCCESS;

  ASSERT(CpuIndex < MAXCPUDATA);

  if(CpuIndex < MAXCPUDATA) {  
    mMpSyncData.CpuData[CpuIndex].Args.CpuIndex = CpuIndex;
    mMpSyncData.CpuData[CpuIndex].Args.ProcArguments = ProcArguments;
    mMpSyncData.CpuData[CpuIndex].Args.Procedure = Procedure;
    
    InterlockedIncrement((UINT32 *)&mMpSyncData.CpuCnt);
    Status = mSmst_syncdata->SmmStartupThisAp (MpSyncApWrapperCnt,
                                      CpuIndex,
                                      &mMpSyncData.CpuData[CpuIndex].Args);
    if(Status != EFI_SUCCESS) {
      InterlockedDecrement((UINT32 *)&mMpSyncData.CpuCnt);      
    }
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  return Status;
}


UINTN
GetTheCpuNumber (
  UINT8 Socket,
  UINT8 Core,
  UINT8 Thread)
{
  UINT8 MaxThread;
  MaxThread = GetMaxThreads();
  
  if ((Socket < MAX_SOCKET) && 
      (Core < MAXCOREID)     && 
      (Thread < MaxThread) &&
      (mCpuInfo[Socket].Valid[Core][Thread])
      ) {
    return mCpuInfo[Socket].cpuNumber[Core][Thread];
  } else {
    return (UINTN)-1;
  }
}


UINT32
GetApicID (
  VOID
  )
/*++

Routine Description:
    Returns the apic ID  of the thread executing the code

Arguments:

  none
Returns:

    status - EFI_SUCCESS - no error
             non-zero code - if error
    IsEventHandled - Event handled status - TRUE/ FALSE

--*/
{
  UINT32 ApicId;
  UINT32 RegEax;
  
  AsmCpuid (EFI_CPUID_SIGNATURE, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0xB) {
    //
    // Is X2Apic capable.
    // Get ApicId from cpuid leaf B.
    //
    AsmCpuidEx (EFI_CPUID_EXTENDED_TOPOLOGY, 0, NULL, NULL, NULL, &ApicId);
  } else {
    //
    // Get ApicId from cpuid leaf 1.
    //
    AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, &ApicId, NULL, NULL);
    ApicId = ((ApicId >> 24) & 0xFF);
  }
  
  return ApicId;
}
UINT32
GetApicIdByLoc (
  IN EFI_CPU_PHYSICAL_LOCATION *Location
  )
{
  UINTN   ThreadBits;
  UINTN   CoreBits;
  UINT32  ApicId;

  ASSERT (Location != NULL);

  GetCoreAndSMTBits(&ThreadBits,&CoreBits);
  if(Location == NULL) {
    return 0;
  }
  ApicId = (UINT32) ((Location->Package << (ThreadBits + CoreBits)) | (Location->Core << ThreadBits) |  (Location->Thread));
  return ApicId;
}
EFI_STATUS
ExtractProcessorLocation(
  IN UINT32 ApicId,
  OUT EFI_CPU_PHYSICAL_LOCATION *Location
  ) 
{
  UINTN ThreadBits;
  UINTN CoreBits;

  if(Location == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  if(GetCoreAndSMTBits(&ThreadBits,&CoreBits) == EFI_SUCCESS) {
    Location->Thread = (UINT32)(ApicId & ~((~0) << ThreadBits));
    Location->Core = (UINT32)((ApicId >> ThreadBits) & ~((~0) << CoreBits));
    Location->Package = (ApicId >> (ThreadBits+CoreBits));
  } else {
    Location->Package = 0;
    Location->Core = 0;
    Location->Thread = 0;
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


UINTN
GetAPCpuNumber(
   VOID
)
{
    UINT32 ApicId;
    EFI_CPU_PHYSICAL_LOCATION Loc;
    UINTN cpunumber;
    UINT8 skt;

    ApicId = GetApicID();
    ExtractProcessorLocation(ApicId,&Loc);
    skt = (UINT8)Loc.Package;
    cpunumber = GetTheCpuNumber(skt, (UINT8)Loc.Core, (UINT8)Loc.Thread); 

    return cpunumber;
}



VOID
ExtractingNodeCoreFromApicId (
  OUT UINT8 *Socket,
  OUT UINT8 *Core
  )
{
  //
  // the APIC ID is composed of the Socket ID
  // and a combination of the Core ID and Thread ID, as illustrated
  // below:
  //
  //  ----------- -------------------
  // |  Bit 7-n  |      Bit n-0      |
  //  ----------- -------------------
  // | Socket ID | Core ID/Thread ID |
  //  ----------- -------------------
  // Bit "n" is Socket specific (defined via CPUID)
  //

  UINT8                coreShift;
  UINT8                packageShift;
  UINT8                coreTemp;
  UINT8                coreMask;
  UINT32               RegEax=0, RegEbx=0, RegEcx=0, RegEdx=0;

  //
  // Execute CPUID Leaf 0Bh, with ECX=0 (thread level), then
  // EAX[4:0]  = Number of bits to right-shift APIC ID to get CoreID
  // EDX[31:0] = Extended APIC ID -- Lower 8 bits identical to the legacy APIC ID
  //

  RegEax = CPUID_TOPO_INFO;
  RegEcx = 0;
 
  AsmCpuidEx (RegEax, RegEcx, &RegEax, &RegEbx, &RegEcx, &RegEdx);

  coreShift = (UINT8)(RegEax & 0x1f);
  coreTemp = (UINT8)((RegEdx & 0xff ) >> coreShift);

  //
  // Execute CPUID Leaf 0Bh, with ECX=1 (Core level), then
  // EAX[4:0]  = Number of bits to right-shift APIC ID to get SocketID (i.e., Packge-leval ID)
  // EDX[31:0] = Extended APIC ID -- Lower 8 bits identical to the legacy APIC ID
  //

  RegEax = CPUID_TOPO_INFO;
  RegEcx = 1;
 
  AsmCpuidEx (RegEax, RegEcx, &RegEax, &RegEbx, &RegEcx, &RegEdx);

  packageShift = (UINT8)RegEax & 0x1f;
  *Socket = (UINT8)(((UINT8)(RegEdx & 0xff )) >> packageShift);

  //
  // Mask package Id to get Core Id
  //
  coreMask = (1 << (packageShift - coreShift)) - 1;
  *Core = (UINT8) (coreTemp & coreMask);
  return; 
}


BOOLEAN
ExcutingOnLocalNode (
  UINT8 Socket
  )
{
  UINT8 localSocket = 0;
  UINT8 localCore = 0;

  ExtractingNodeCoreFromApicId (&localSocket, &localCore);

  return (BOOLEAN)(Socket == localSocket);
}


BOOLEAN
ExecutingOnLocalNodeCore (
  UINT8 Socket,
  UINT8 Core
  )
{
  UINT8 localSocket = 0;
  UINT8 localCore = 0;
  
  ExtractingNodeCoreFromApicId (&localSocket, &localCore);

  if ((Socket == localSocket) &&
      (Core == localCore)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

EFI_STATUS
EFIAPI
MpsyncDataLibConstructor (
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
)
{
  UINT8                       socket;
  EFI_STATUS                  Status;
  UINT32                      core;
  UINT32                      thread;
  UINTN                       numberOfCPUs;
  EFI_PROCESSOR_INFORMATION   processorInfoBuffer;
  UINTN                       currProcessor;
  UINTN                       CoreBits;
  UINTN                       ThreadBits;
  BOOLEAN                     InSmm = FALSE;
  EFI_IIO_UDS_PROTOCOL        *IioUdsProtocol = NULL;
  
  

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL,  &mSmmBase_syncdata);
  ASSERT_EFI_ERROR (Status);
  
  if (mSmmBase_syncdata != NULL) {
    mSmmBase_syncdata->GetSmstLocation (mSmmBase_syncdata, &mSmst_syncdata);
    mSmmBase_syncdata->InSmm (mSmmBase_syncdata, &InSmm);
  }
  ASSERT(mSmst_syncdata != NULL);
  if(mSmst_syncdata == NULL) {
    return EFI_NOT_READY;
  }

  // Locate the IIO Protocol Interface
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &IioUdsProtocol);
  ASSERT_EFI_ERROR (Status);

  // Save IioUds pointer for runtime use
  mIioUdsPtr_syncdata = IioUdsProtocol->IioUdsPtr;
  
  if(InSmm == TRUE) {
    Status = mSmst_syncdata->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess_syncdata);
    ASSERT_EFI_ERROR (Status);   
  }   

  Status = mSmst_syncdata->SmmLocateProtocol (&gEfiSmmCpuServiceProtocolGuid, NULL, (VOID **) &mSmmCpuServiceProtocol);
  ASSERT_EFI_ERROR (Status);
  for( socket = 0; socket < MAX_SOCKET; socket++) {
    for (core = 0; core < MAXCOREID; core++) {        
      for (thread = 0; thread < MAXTHREADID; thread++) {
        mCpuInfo[socket].Valid[core][thread] = FALSE;
        mCpuInfo[socket].cpuNumber[core][thread] = 0;
        mCpuInfo[socket].ApicId[core][thread] = 0;
        mCpuInfo[socket].ApicVer[core][thread] = 0;
      }
    }
  }

  numberOfCPUs = mSmst_syncdata->NumberOfCpus;
  
  GetCoreAndSMTBits(&ThreadBits,&CoreBits);
  InitializeSpinLock (&mMpSyncData.LoggingLock);
  InitializeSpinLock (&mMpSyncData.SerialLock);
  ASSERT(numberOfCPUs <= MAXCPUDATA);

  for(currProcessor = 0; currProcessor < MAXCPUDATA; currProcessor++) {
    InitializeSpinLock(&mMpSyncData.CpuData[currProcessor].ApBusy);
    mMpSyncData.CpuData[currProcessor].Present = FALSE;
  }
    
  for (currProcessor = 0; currProcessor < numberOfCPUs; currProcessor++) {
    Status = mSmmCpuServiceProtocol->GetProcessorInfo(mSmmCpuServiceProtocol,currProcessor, &processorInfoBuffer);
    if (Status == EFI_SUCCESS &&
      (processorInfoBuffer.StatusFlag & PROCESSOR_ENABLED_BIT) != 0) {
    
      mMpSyncData.CpuData[currProcessor].Present = TRUE;
    
      //
      // Save CpuNumber and APIC ID info
      //
      socket  = (UINT8)(((UINT32)processorInfoBuffer.ProcessorId) >> (CoreBits+ThreadBits));
      core  = (UINT32)((processorInfoBuffer.ProcessorId >> ThreadBits) & ~((~0) << CoreBits));
      thread  = (UINT32)(processorInfoBuffer.ProcessorId & ~((~0) << ThreadBits));
      RASDEBUG ((EFI_D_INFO, "MpSyncData: CoreBits: %lx, ThreadBits: %lx", CoreBits,ThreadBits));
         
      //ASSERT (core < MAXCOREID);
      //ASSERT(socket < MAX_SOCKET);
    
      if (socket < MAX_SOCKET && core < MAXCOREID && thread < MAXTHREADID) {
        //RASDEBUG ((EFI_D_ERROR, "MpSyncData: skt: %lx, core: %lx, thread: %lx, apicd: %lx, cpunumber: %lx \n",socket,core,thread,(UINT32)processorInfoBuffer.ProcessorId,currProcessor));
        mCpuInfo[socket].Valid[core][thread] = TRUE;
        mCpuInfo[socket].cpuNumber[core][thread] = currProcessor;
        mCpuInfo[socket].ApicId[core][thread] = ((UINT32)processorInfoBuffer.ProcessorId);
        mCpuInfo[socket].ApicVer[core][thread] = 2;
      } else {
        RASDEBUG ((EFI_D_ERROR, "MpSyncData: ERROR: Wrong socket,core,thread on init"));
        RASDEBUG ((EFI_D_ERROR, "MpSyncData: socket: %lx core: %lx thread: %lx",socket,core,thread));
        ASSERT(FALSE);
      }
    }
  } 
  
  return EFI_SUCCESS;
}


EFI_STATUS 
GetCoreAndSMTBits (
  IN UINTN *ThreadBits,
  IN UINTN *CoreBits
  )
{
  BOOLEAN  TopologyLeafSupported;
  UINT32   RegEax;
  UINT32   RegEbx;
  UINT32   RegEcx;
  UINT32   RegEdx;
  UINT32   MaxCpuIdIndex;
  UINT32   SubIndex;
  UINTN    LevelType;
  UINT32   MaxLogicProcessorsPerPackage;
  UINT32   MaxCoresPerPackage;

  ASSERT (ThreadBits != NULL);
  ASSERT (CoreBits != NULL);

  if (ThreadBits == NULL || CoreBits == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if the processor is capable of supporting more than one logical processor.
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, &RegEdx);
  if ((RegEdx & BIT28) == 0) {
    *ThreadBits = 0;
    *CoreBits = 0;
    return EFI_SUCCESS;
  } 

  *ThreadBits = 0;
  *CoreBits = 0;

   //
  // Assume three-level mapping of APIC ID: Package:Core:SMT.
  //

  TopologyLeafSupported = FALSE;

  //
  // Assume three-level mapping of APIC ID: Package:Core:SMT.
  //

  TopologyLeafSupported = FALSE;
  //
  // Get the max index of basic CPUID
  //
  AsmCpuid (EFI_CPUID_SIGNATURE, &MaxCpuIdIndex, NULL, NULL, NULL);

  //
  // If the extended topology enumeration leaf is available, it
  // is the preferred mechanism for enumerating topology.
  //
  if (MaxCpuIdIndex >= EFI_CPUID_EXTENDED_TOPOLOGY) {
    AsmCpuidEx (EFI_CPUID_EXTENDED_TOPOLOGY, 0, &RegEax, &RegEbx, &RegEcx, NULL);
    //
    // If CPUID.(EAX=0BH, ECX=0H):EBX returns zero and maximum input value for
    // basic CPUID information is greater than 0BH, then CPUID.0BH leaf is not
    // supported on that processor.
    //
    if (RegEbx != 0) {
      TopologyLeafSupported = TRUE;

      //
      // Sub-leaf index 0 (ECX= 0 as input) provides enumeration parameters to extract
      // the SMT sub-field of x2APIC ID.
      //
      LevelType = (RegEcx >> 8) & 0xff;
      ASSERT (LevelType == EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_SMT);

//
// PURLEY_OVERRIDE_BEGIN
//
      //
      //HSD: 5330351  for SKX PO, core location should right shift one on Purley whatever HT enable or not.
      //
      *ThreadBits = RegEax & 0x1f;
//
// PURLEY_OVERRIDE_END
//
      //
      // Software must not assume any "level type" encoding
      // value to be related to any sub-leaf index, except sub-leaf 0.
      //
      SubIndex = 1;
      do {
        AsmCpuidEx (EFI_CPUID_EXTENDED_TOPOLOGY, SubIndex, &RegEax, NULL, &RegEcx, NULL);
        LevelType = (RegEcx >> 8) & 0xff;
        if (LevelType == EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_CORE) {
          *CoreBits = (RegEax & 0x1f) - *ThreadBits;
          break;
        }
        SubIndex++;
      } while (LevelType != EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_INVALID);
    }
  }

  if (!TopologyLeafSupported) {
    AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, &RegEbx, NULL, NULL);
    MaxLogicProcessorsPerPackage = (RegEbx >> 16) & 0xff;
    if (MaxCpuIdIndex >= EFI_CPUID_CACHE_PARAMS) {
      AsmCpuidEx (EFI_CPUID_CACHE_PARAMS, 0, &RegEax, NULL, NULL, NULL);
      MaxCoresPerPackage = (RegEax >> 26) + 1;
    } else {
      //
      // Must be a single-core processor.
      //
      MaxCoresPerPackage = 1;
    }

    *ThreadBits = (UINTN) (HighBitSet32 (MaxLogicProcessorsPerPackage / MaxCoresPerPackage - 1) + 1);
    *CoreBits = (UINTN) (HighBitSet32 (MaxCoresPerPackage - 1) + 1);
  }
  
  return EFI_SUCCESS;
}

/* sugu_todo - below code sounds like NEVER executed*/
EFI_STATUS
SmmStartupAllSocketPackageApBlocking(
  IN EFI_AP_PROCEDURE  SmmApCallback,
  IN OUT  VOID       *ProcArguments 
)
{
  UINT8 skt;
  EFI_STATUS  Status;

  ASSERT(mSmst_syncdata != NULL);
  ASSERT(SmmApCallback != NULL);
  ASSERT(mIioUdsPtr_syncdata != NULL);

  if(mSmst_syncdata == NULL || SmmApCallback==NULL ||mIioUdsPtr_syncdata==NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for( skt = 0; skt < MAX_SOCKET; skt++) {
      //
      // Ignore disabled nodes
      //
      if ((mIioUdsPtr_syncdata->SystemStatus.socketPresentBitMap & (1 << skt)) == 0) {
        continue;
      }

      Status = SmmStartupPackageApBlocking(skt, SmmApCallback, ProcArguments);
      ASSERT_EFI_ERROR(Status); 
  }
  return EFI_SUCCESS;
}

EFI_STATUS
SmmStartupPackageApBlocking(
  IN UINT8            skt,
  IN EFI_AP_PROCEDURE  SmmApCallback,
  IN OUT  VOID       *ProcArguments   
)
{
  UINTN      cpuNumber;
  UINT8      core;
  UINT8      thread;

  thread     = 0;
  core       = 0;

  ASSERT(mSmst_syncdata != NULL);
  ASSERT(SmmApCallback != NULL);
  ASSERT(skt < MAX_SOCKET);

  if(mSmst_syncdata == NULL || SmmApCallback == NULL || skt >= MAX_SOCKET) {
    return EFI_INVALID_PARAMETER;
  }
  
  for (core = 0; core < MAXCOREID; core++) {
    if(FindValidThread(skt,core,&thread) == EFI_SUCCESS) {
      break;  
    }
  }
  if(core < MAXCOREID && mCpuInfo[skt].Valid[core][thread]) {
    if (ExcutingOnLocalNode (skt) == FALSE) {
      cpuNumber = GetTheCpuNumber (skt, core, thread);
      SmmBlockingStartupThisAp(SmmApCallback,
                               cpuNumber,
                               ProcArguments);
      //RASDEBUG((DEBUG_ERROR,"Startup package %lx\n",cpuNumber));
      
    } else {
       //RASDEBUG((DEBUG_ERROR,"Startup local skt %lx\n",skt));
       SmmApCallback(ProcArguments);
    }
  } else {
    return EFI_NOT_READY;
  }
  
  return EFI_SUCCESS;    
}


EFI_STATUS
SmmStartupAllSocketAllCoreApBlocking(
  IN EFI_AP_PROCEDURE  SmmApCallback,
  IN OUT  VOID       *ProcArguments   
)
{
  UINT8 skt;
  EFI_STATUS  Status;

  ASSERT(mSmst_syncdata != NULL);
  ASSERT(SmmApCallback != NULL);
  ASSERT(mIioUdsPtr_syncdata != NULL);
  
  if(mSmst_syncdata == NULL || mIioUdsPtr_syncdata == NULL || SmmApCallback == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  for( skt = 0; skt < MAX_SOCKET; skt++) {
    //
    // Ignore disabled nodes
    //
    if ((mIioUdsPtr_syncdata->SystemStatus.socketPresentBitMap & (1 << skt)) == 0) {
      continue;
    }

    Status = SmmStartupCoreApBlocking(skt, SmmApCallback, ProcArguments);
    ASSERT_EFI_ERROR(Status);
 }
    
  return EFI_SUCCESS;
}

EFI_STATUS
SmmStartupCoreApBlocking(
  IN UINT8             skt,
  IN EFI_AP_PROCEDURE  SmmApCallback,
  IN OUT  VOID        *ProcArguments     
)
{
  UINT8      core;
  UINT8      thread;
  UINTN     cpuNumber;
  EFI_STATUS Status = EFI_SUCCESS;

  if(mSmst_syncdata == NULL || skt >= MAX_SOCKET || SmmApCallback == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  for (core = 0; core < MAXCOREID; core++) {
    //RASDEBUG((DEBUG_ERROR,"Startup core %lx\n",core));
    if(FindValidThread(skt,core,&thread) != EFI_SUCCESS) {
      continue;  
    }
    //RASDEBUG((DEBUG_ERROR,"Startup thread %lx\n",thread));
    if (ExecutingOnLocalNodeCore (skt, core) == FALSE) {
      if(mCpuInfo[skt].Valid[core][thread] == TRUE) {
        cpuNumber = GetTheCpuNumber (skt, core, thread);
         //RASDEBUG((DEBUG_ERROR,"Startup cpunumber %lx\n",cpuNumber));
         if(cpuNumber == (UINTN)-1) {
          continue;
         }
        Status = SmmBlockingStartupThisAp(SmmApCallback,
                                 cpuNumber,
                                 ProcArguments);
        //RASDEBUG((DEBUG_ERROR,"Startup Status %lx\n",Status));
      } else {
         //RASDEBUG((DEBUG_ERROR,"Startup invalid skt %lx, core %lx thread %lx\n",skt, core,thread));
      }
    } else {
      //RASDEBUG((DEBUG_ERROR,"Startup local skt %lx, core %lx\n",skt, core));
      SmmApCallback(ProcArguments);      
    }
 }
  
 return EFI_SUCCESS;
}


EFI_STATUS
SmmStartupCoreApBlkCnt(
  IN UINT8             skt,
  IN EFI_AP_PROCEDURE  SmmApCallback,
  IN OUT  VOID        *ProcArguments     
)
{
  UINT8      core;
  UINT8      thread;
  UINTN     cpuNumber;
  EFI_STATUS Status = EFI_SUCCESS;

  if(mSmst_syncdata == NULL || skt >= MAX_SOCKET || SmmApCallback == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  mMpSyncData.CpuCnt = 0;

  for (core = 0; core < MAXCOREID; core++) {
    //RASDEBUG((DEBUG_ERROR,"Startup core %lx\n",core));
    if(FindValidThread(skt,core,&thread) != EFI_SUCCESS) {
      continue;  
    }
    //RASDEBUG((DEBUG_ERROR,"Startup thread %lx\n",thread));
    if (ExecutingOnLocalNodeCore (skt, core) == FALSE) {
      if(mCpuInfo[skt].Valid[core][thread] == TRUE) {
        cpuNumber = GetTheCpuNumber (skt, core, thread);
         //RASDEBUG((DEBUG_ERROR,"Startup cpunumber %lx\n",cpuNumber));
         if(cpuNumber == (UINTN)-1) {
          continue;
         }
        Status = SmmCntStartupThisAp(SmmApCallback,
                                 cpuNumber,
                                 ProcArguments);
        //RASDEBUG((DEBUG_ERROR,"Startup Status %lx\n",Status));
      } else {
        //RASDEBUG((DEBUG_ERROR,"Startup invalid skt %lx, core %lx thread %lx\n",skt, core,thread));
      }
    } else {
      //RASDEBUG((DEBUG_ERROR,"Startup local skt %lx, core %lx\n",skt, core));
      SmmApCallback(ProcArguments);      
    }
 }

 while (mMpSyncData.CpuCnt != 0) {
      //RASDEBUG((DEBUG_ERROR,"Startup cpucnt %lx\n",mMpSyncData.CpuCnt));
      CpuPause ();
 }
  
 return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
SmmBlockingStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuIndex,
  IN OUT  VOID                      *ProcArguments OPTIONAL
  )

{
  EFI_STATUS Status;

  ASSERT(CpuIndex < MAXCPUDATA);

  if(CpuIndex < MAXCPUDATA) {  
    mMpSyncData.CpuData[CpuIndex].Args.CpuIndex = CpuIndex;
    mMpSyncData.CpuData[CpuIndex].Args.ProcArguments = ProcArguments;
    mMpSyncData.CpuData[CpuIndex].Args.Procedure = Procedure;
    
    AcquireSpinLock(&mMpSyncData.CpuData[CpuIndex].ApBusy);
    Status = mSmst_syncdata->SmmStartupThisAp (MpSyncApWrapper,
                                      CpuIndex,
                                      &mMpSyncData.CpuData[CpuIndex].Args);
    if(Status == EFI_SUCCESS) {
      AcquireSpinLock(&mMpSyncData.CpuData[CpuIndex].ApBusy);      
    }
    
    ReleaseSpinLock(&mMpSyncData.CpuData[CpuIndex].ApBusy);
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  return Status;
}


VOID EFIAPI 
MpSyncApWrapper(
  IN OUT VOID  *Buffer
)
{
  MP_SYNC_WRAPPER_ARGS_STRUCT *Args;
  
  Args = (MP_SYNC_WRAPPER_ARGS_STRUCT *)Buffer;
  if(Args != NULL) {
    Args->Procedure(Args->ProcArguments);
    MpSyncReleaseAp(Args->CpuIndex);
  }
}

VOID EFIAPI 
MpSyncApWrapperCnt(
  IN OUT VOID  *Buffer
)
{
  MP_SYNC_WRAPPER_ARGS_STRUCT *Args;
  
  Args = (MP_SYNC_WRAPPER_ARGS_STRUCT *)Buffer;
  if(Args != NULL) {
    Args->Procedure(Args->ProcArguments);
  }
  InterlockedDecrement((UINT32 *)&mMpSyncData.CpuCnt);
}


VOID
MpSyncReleaseAp
(
IN UINTN                     CpuIndex
)
{
  //
  // Klocwork 1291: Array 'CpuData' of size 512 may use index value(s) UINT_MAX
  //
  ASSERT(CpuIndex < MAXCPUDATA);
  if (CpuIndex >= MAXCPUDATA) {
    CpuDeadLoop();
    return;
  }
  ReleaseSpinLock(&mMpSyncData.CpuData[CpuIndex].ApBusy); 
}

/**
  Prints a message to the serial port.

  @param  Format      Format string for the message to print.
  @param  ...         Variable argument list whose contents are accessed 
                      based on the format string specified by Format.

**/
VOID
EFIAPI
SerialPrintBlocking(
  IN  UINTN        ErrorLevel,
  IN  CONST CHAR8  *Format,
  ...
  )
{
  CHAR8    Buffer[200];
  VA_LIST  Marker;

  //
  // If Format is NULL, then ASSERT().
  //
  ASSERT (Format != NULL);

  if (Format == NULL)
	return;

  //
  // Convert the message to an ASCII String
  //
  VA_START (Marker, Format);
  AsciiVSPrint (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);

  //
  // Send the print string to a Serial Port
  //
  while (!AcquireSpinLockOrFail (&mMpSyncData.SerialLock)) {
      CpuPause ();
  }
  DebugPrint (ErrorLevel, Buffer);
  ReleaseSpinLock(&mMpSyncData.SerialLock);
}

