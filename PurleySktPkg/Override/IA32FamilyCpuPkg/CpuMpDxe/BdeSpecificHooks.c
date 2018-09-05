#include "BdeSpecificHooks.h"
#include "MchkInit.h"
extern EFI_CPU_CSR_ACCESS_PROTOCOL         *mCpuCsrAccess;
extern EFI_IIO_UDS_PROTOCOL                *mIioUds;

MACHINE_CHECK_ATTRIBUTE MachineCheckAttribute[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

VOID 
BdeHookInitialize()
{
  EFI_STATUS    Status;
  
  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);
}

/**

   Get New SBSP processor number

    @param SocketNumber - CPU SocketID

    @retval ProcessorNumber - Processor Number for new SBSP

**/
UINTN
GetNewSbspProcessNumber (
  UINT32 SocketNumber
  )
{
  UINTN       i;
  UINTN       ProcessorNumber = (UINTN)-1;
  UINT32      CoreNumber = (UINT32)-1;     
  UINT32      ThreadNumber = (UINT32)-1;  
   

  for (i = 0; i < mCpuConfigContextBuffer.NumberOfProcessors; i++) {

    if(mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[i].ProcessorLocation.Package == SocketNumber) {
      if(mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[i].ProcessorLocation.Core >= CoreNumber) {
        continue;
      } else {
        CoreNumber = mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[i].ProcessorLocation.Core;
        if(mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[i].ProcessorLocation.Thread > ThreadNumber) {
          continue;
        } else {
          ThreadNumber = mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[i].ProcessorLocation.Thread;
          ProcessorNumber = i;
        }
      } //end of if CoreNumber
    } //end if SocketNumber
  }

  if(ProcessorNumber >= mCpuConfigContextBuffer.NumberOfProcessors) {  //don't find one
    ProcessorNumber = mCpuConfigLibConfigContextBuffer->BspNumber;     //deafult to current Sbsp
  }
  return ProcessorNumber;
}

//
// BIOS w/a for #3613974
// Please implement LLC init WA for all steppings != B0/B1
// C-step and higher will require BIOS workaround as it's removed from uCode again
//
// 
/**

    BIOS w/a for #3613974

    @param ProcessorNumber - Processor Number
    @retval None

**/
VOID
WA_InitializeLLC (
  UINTN      ProcessorNumber
  )
{
  UINT32              FamilyId;
  UINT32              ModelId;
  UINT32              SteppingId;
  UINT32              CoreId;
  UINT32              ThreadId;
  UINT32              data32;
  UINT8               *StartAddr;                         // start from 256MB
  UINT32              Stride = 64;                        // 64-Byte stride
  UINT32              LoopCount = (24*1024*1024)/Stride;  // LoopCount = 24MB/Stride
  UINT32              i;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);
  if (!(FamilyId == NEHALEM_FAMILY_ID && ModelId== SANDYBRIDGE_SERVER_MODEL_ID)) {
    return;
  }
  if (SteppingId > 1 && SteppingId < 5) return; // 2 is B0, 3 is B1

  //
  //Execute this workaround only on Core0/Thread0 of a CPU package
  //
  GetProcessorLocation (ProcessorNumber, NULL, &CoreId, &ThreadId);
  if (!(CoreId ==0 && ThreadId == 0)) {
    return;
  }

  //
  //  Read memory cache-lines into LLC
  //
  StartAddr = (UINT8 *)((UINT64)0x020000000); 
  for (i=0; i<LoopCount; i++) {
    data32 =   *((volatile UINT32*) StartAddr);
    StartAddr += Stride;
  }
  //
  // Clear MCi_Status related to CBo LLC (MCA banks 12 thru 19) 
  //  
  if (FeaturePcdGet (PcdCpuMachineCheckFlag)) {
    for (i= 12; (i < 12 + 8) && (i < MachineCheckAttribute[ProcessorNumber].NumberOfErrorReportingBank); ++i) {
      WriteRegisterTable (ProcessorNumber, Msr, EFI_IA32_MC0_STATUS + 4 * i, 0, 64, 0);
    }
  }
}

//
//  BIOS w/a for 3875167, 3614340, 3612728 (SNB Server System Agent BSU).
//  Transactions Issued from Socket 1
//  Appear Locked Due to an INVD from Socket 0
//  Permanent workaround for B0 and later steppings
// 

/**

     BIOS w/a for 3875167, 3614340, 3612728 (SNB Server System Agent BSU).

    @param None

    @retval None

**/
VOID WA_Lock (
  )
{
  UINT32     data;
  UINT8               i;
  UINT32   CpuSignature;

  if (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag)) {
    AsmCpuid (EFI_CPUID_VERSION_INFO, &CpuSignature, NULL, NULL, NULL);
    if ((CpuSignature & 0x0fff0ff0) == 0x206d0) {

      //
      // After all APs has loaded microcode, clear LockControl.LockDisable on 
      // all sockets present in the system.
      //
      for (i = 0; i < MAX_SOCKET; i++) {
        if (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[i].Valid) {
          data = mCpuCsrAccess->ReadCpuCsr(i, 0, LOCKCONTROL_UBOX_CFG_REG);
          data &= ~BIT0;
          mCpuCsrAccess->WriteCpuCsr(i, 0, LOCKCONTROL_UBOX_CFG_REG , data);

          
        }
      }
    }
  }
}

