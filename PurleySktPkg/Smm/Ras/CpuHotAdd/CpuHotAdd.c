/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2011 - 2015, Intel Corporation. All rights reserved. <BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
  CpuHotAdd.c

Abstract:
  Module to initialize hot added CPU and relocate SMBASE addresses for all threads.

--*/

#include "CpuHotAdd.h"
#include "ArchSpecificDef.h"
// APTIOV_SERVER_RC_OVERRIDE_START : Use the AMI sdl value for ACM Base.
#include <Token.h>
// APTIOV_SERVER_RC_OVERRIDE_END : Use the AMI sdl value for ACM Base.
#include "FlashMap.h"
#include <Library/mpsyncdatalib.h>

CPU_HOT_ADD_DATA                           mCpuHotAddData;
ACPI_CPU_DATA                              *mAcpiCpuData;
UINT8                                      mSaveState[0x10000 - SMM_CPU_STATE_OFFSET];

volatile HOT_ADD_CPU_EXCHANGE_INFO         *mExchangeInfo;
volatile UINTN                             mRelocatedCpu;


EFI_CPU_PPM_PROTOCOL                        *mCpuPpmProtocol;
EFI_CPU_PM_STRUCT                           *mPpmLib, mLocalPpmStruct;
PPM_FROM_PPMINFO_HOB                        mPpmInfoHob;
EFI_PPM_STRUCT                              mPpmStruct;
EFI_CPU_CSR_ACCESS_PROTOCOL                 *mCpuCsrAccess;
EFI_MP_SERVICES_PROTOCOL                    *mMpService;


VOID
SmmInitHandler (
  VOID
  )
/*++

Routine Description:

  This is funtion used by all threads to relocate the SMM base

Arguments:

  None  

Returns:
  None 

--*/   
{
  UINT32                            ApicId;
  UINTN                             Index;
  SOCKET_LGA_775_SMM_CPU_STATE      *CpuState;

  ApicId = GetApicId ();
  SmmInitializeSmrrEmrr ();

  CpuState = (SOCKET_LGA_775_SMM_CPU_STATE *)(UINTN)(SMM_DEFAULT_SMBASE + SMM_CPU_STATE_OFFSET);

  for (Index = 0; Index < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); Index++) {
      if (mExchangeInfo->CpuSmmInitData.ApicId[Index] == INVALID_APIC_ID) {       
        CpuState->x86.SMBASE  = (UINT32)(mExchangeInfo->CpuSmmInitData.SmBase[Index]);
        mExchangeInfo->CpuSmmInitData.ApicId[Index] = ApicId;
        break;
      }
  }

  AsmAcquireMPLock ((UINT8 *)&mExchangeInfo->Lock);  
  mRelocatedCpu++;
  AsmReleaseMPLock ((UINT8 *)&mExchangeInfo->Lock);
  
  AsmRsm ();
}

VOID
MPRendezvousProcedure (
  VOID 
  )
/*++

Routine Description:

  This is the routine that all threads other than PBSP will be running after the hot add event

Arguments:

  None  

Returns:
  None 

--*/    
{
  InitialProcessorInit();

  // Check-in counter for threads 
  AsmAcquireMPLock ((UINT8 *)&mExchangeInfo->Lock);
  mExchangeInfo->NumCpuCheckedIn++;
  AsmReleaseMPLock ((UINT8 *)&mExchangeInfo->Lock);
  
  if (IsSimicsPlatform() == FALSE) {
    PpmMsrProgramming ((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  }

  return;
}

VOID
PackageBspCpuEntry (
  VOID
  )
{
  
  UINT32                      Index = 0;
  UINT32                      CpuIndex;
  UINT32                      ApicId;
  UINT8                       SktId;
  UINT32                      SocketLevelBitShift;
  
  
  ApicId = GetApicId ();
  // use SBSP data as we don't support different SKU CPU O*L
  AsmCpuidEx (EFI_CPUID_CORE_TOPOLOGY, 1, &SocketLevelBitShift, NULL, NULL, NULL);
  SocketLevelBitShift &= 0x1F;
  SktId = (UINT8)(ApicId >> SocketLevelBitShift);    
  
  mExchangeInfo = (HOT_ADD_CPU_EXCHANGE_INFO *)mCpuHotAddData.MpExchangeInfoPtr;

  InitialProcessorInit();

  // Power Management CSR Progoramming.
  if (IsSimicsPlatform() == FALSE) {
    PpmCsrProgramming(SktId);
  }

  // Set up the exchange data structure for APs' waking up
  mExchangeInfo->ApFunction      = (VOID *)(UINTN)MPRendezvousProcedure;
  mExchangeInfo->Lock            = 0;
  mExchangeInfo->InitFlag        = 3;
  mExchangeInfo->SignalFlag      = 0;

  // Send INIT IPI/SIPI to all APs, blocking them at entry

  //
  // In Simics,  the SIPI is broadcasted to SBSP as well
  // So, sending the targeted SIPI to S1C0T0
  //

  if (IsSimicsPlatform() == FALSE) {
    SendInitSipiSipiAllExcludingSelf ((UINT32)mCpuHotAddData.StartupVector);
  } else {
    SendInitSipiSipi (0x21, (UINT32)mCpuHotAddData.StartupVector);
  }

  // Wait for all APs to response to the IPI
  MicroSecondDelay  (2000);
  
#ifdef LT_FLAG
  LTSXHotPlug();
#endif

  // Assign index for all CPUs for stack allocation
  CpuIndex = 0;
  for (Index = 0; Index < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); Index++) {
    if (mExchangeInfo->BistBuffer[Index].Number != 0) {      
      mExchangeInfo->BistBuffer[Index].Number = CpuIndex + 1;
      CpuIndex++;
    }   
  }
  
  // Unblock APs, everybody shoud go to MPRendezvousProcedure with stack set properly
  mExchangeInfo->NumCpuCheckedIn = 1;
  mExchangeInfo->SignalFlag = 1;
  while (mExchangeInfo->NumCpuCheckedIn != CpuIndex) {
    CpuPause ();
  }

  if (IsSimicsPlatform() == FALSE) {
    PpmMsrProgramming ((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
    PStateTransition ((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);

    PpmSetMsrLockBit ((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);

    mLocalPpmStruct.Info->SocketNumber = SktId;
    PpmSetCsrLockBit((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
    PpmSetBiosInitDone((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
    EnableAutonomousCStateControl ((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
    PpmSetMsrCstConfigCtrlLockBit ((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);

  }

  // Notify SBSP the completion of MPINIT
  mExchangeInfo->SignalFlag = (UINT32)(-1);

  return;

}

VOID
PackageSmmRelocate (
  VOID
  )
/*++

Routine Description:

  This funtion will relocate the SMM base for all threads including PBSP
  by sending SMI IPI to everybody. The SMI IPI has to be sent in a serilaized 
  manner to avoid the SMM saving region overlap with each other at the default
  base

Arguments:

  None  

Returns:
  None 

--*/ 
{
  UINTN                       LastNumber; 
  UINTN                       Index;
  UINTN                       CpuIndex;
  UINT32                      ApicId;
  UINTN                       OldData;
  UINT8                      *U8Ptr;
  VOID                       *OldApFunction;

  OldApFunction  = mExchangeInfo->ApFunction;
  
  // Backup original contents @ 0x38000
  U8Ptr = (UINT8*)(UINTN)(SMM_DEFAULT_SMBASE + SMM_HANDLER_OFFSET);

  // Set SMM jmp to AP waking vector
  OldData                = *(UINT64 *)U8Ptr;
  *U8Ptr                 = 0xEA;   //jmp
  *(UINT32 *)(U8Ptr + 1) = (UINT32)( (mCpuHotAddData.StartupVector >> 4) << 16 );
  CopyMem (mSaveState, (UINT8*)(UINTN)(SMM_DEFAULT_SMBASE + SMM_CPU_STATE_OFFSET), sizeof(mSaveState));

  // Relocate SMM base for all threads including itself
  mExchangeInfo->InitFlag        = 0;
  mExchangeInfo->ApFunction      = (VOID *)(UINTN)SmmInitHandler;

  CpuIndex      = 0;
  mRelocatedCpu = 0;
  LastNumber    = mRelocatedCpu;
  
  // Send the SMI IPI one by one for all detected threads
  for (Index = 0; Index < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); Index++) {    
    if (mExchangeInfo->BistBuffer[Index].Number != 0) {      
      
      ApicId = (UINT8)Index;
      SendSmiIpi ((UINT32)ApicId);

      // Wait for its completion
      while (mRelocatedCpu == LastNumber) {
        CpuPause ();
      } 

      // Delay a little bit so that the processor can exit SMM
      MicroSecondDelay  (100);
      LastNumber = mRelocatedCpu;
      CpuIndex++;
    }
  }
  
  // Restore the 0x38000 contents
  *(UINT64 *)U8Ptr = OldData;
  CopyMem ((UINT8*)(UINTN)(SMM_DEFAULT_SMBASE + SMM_CPU_STATE_OFFSET), mSaveState, sizeof(mSaveState));
  mExchangeInfo->ApFunction  = OldApFunction;

  mExchangeInfo->ApFunction = (VOID *)((EFI_PHYSICAL_ADDRESS)PackageBspCpuEntry);
  
  return;
}

VOID
InitializeSmmCpuPpmStruct() 
{
 
  CopyMem(&(mLocalPpmStruct.Version), &(mPpmLib->Version), sizeof(PPM_VERSION));

  mLocalPpmStruct.Info = &(mPpmInfoHob);
  CopyMem(mLocalPpmStruct.Info, mPpmLib->Info, sizeof(PPM_FROM_PPMINFO_HOB));

  mLocalPpmStruct.Setup = &(mPpmStruct);
  CopyMem(mLocalPpmStruct.Setup, mPpmLib->Setup, sizeof(EFI_PPM_STRUCT));

  mLocalPpmStruct.CpuCsrAccess = mCpuCsrAccess;
   
}

EFI_STATUS
EFIAPI
CpuHotAddEntry (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
/*++

Routine Description:

  Entry point for Cpu Hot Add driver.

Arguments:

  ImageHandle  -  EFI_HANDLE
  SystemTable  -  EFI_SYSTEM_TABLE

Returns:
  EFI_SUCCESS      - Success.
  EFI_UNLOAD_IMAGE - Unload the image for the first loading 
  others           - Error occurred.

--*/    
{
  EFI_HANDLE                  Handle = NULL;
  EFI_LEGACY_BIOS_PROTOCOL   *LegacyBios;
  HOT_ADD_CPU_EXCHANGE_INFO  *ExchangeInfo;
  VOID                       *LegacyRegion, *SmmStartupDataArea;
  MP_ASSEMBLY_ADDRESS_MAP    AddressMap;
  EFI_PHYSICAL_ADDRESS       WorkingBuffer;
  CPU_INIT_INFO              *CpuInitInfo;
  EFI_STATUS                 Status = EFI_SUCCESS;

  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Hot add Driver\n"));

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, 0x1000, &SmmStartupDataArea);
  ASSERT_EFI_ERROR (Status);
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nThe address of SmmStartupData area is %x\n",SmmStartupDataArea));

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &mMpService);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiCpuPpmProtocolGuid, NULL, &mCpuPpmProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mPpmLib = mCpuPpmProtocol->CpuPpmStruct;
  
  // Copy the data from mCpuPpmProtocl to the local structure.
  InitializeSmmCpuPpmStruct();

  mAcpiCpuData = (ACPI_CPU_DATA *)(UINTN)PcdGet64 (PcdCpuS3DataAddress);

  // Allocate and Initialize the MP Exchange Info data in ACPI NVS Region.
  Status = gBS->AllocatePool (
             EfiACPIMemoryNVS,
             sizeof (HOT_ADD_CPU_EXCHANGE_INFO),
             &ExchangeInfo
             );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
  }
 

  ExchangeInfo->StackStart  = (VOID *) (UINTN) mAcpiCpuData->StackAddress;
  ExchangeInfo->StackSize   = PcdGet32 (PcdCpuApStackSize);
  ExchangeInfo->ApFunction  = (VOID *)((EFI_PHYSICAL_ADDRESS)PackageBspCpuEntry);

  // Need to program the following legacy MSRS on the incoming skt. The value is to be same as that of SBSP
  // So, saving the MSR offset and values of SBSP here, and will be programmed when the socket comes online
  ExchangeInfo->SbspMsrInfo[0].MsrOffset = MSR_PMG_CST_CONFIG_CONTROL;
  ExchangeInfo->SbspMsrInfo[0].MsrValue = AsmReadMsr64 (MSR_PMG_CST_CONFIG_CONTROL);;

  ExchangeInfo->SbspMsrInfo[1].MsrOffset = MSR_PMG_IO_CAPTURE_BASE;
  ExchangeInfo->SbspMsrInfo[1].MsrValue = AsmReadMsr64 (MSR_PMG_IO_CAPTURE_BASE);

  ExchangeInfo->SbspMsrInfo[2].MsrOffset = MSR_POWER_CTL;
  ExchangeInfo->SbspMsrInfo[2].MsrValue = AsmReadMsr64 (MSR_POWER_CTL);

  ExchangeInfo->SbspMsrInfo[3].MsrOffset = EFI_MSR_IA32_MISC_ENABLE;
  ExchangeInfo->SbspMsrInfo[3].MsrValue = AsmReadMsr64 (EFI_MSR_IA32_MISC_ENABLE);

  //
  // Allocate 4K aligned bytes from either 0xE0000 or 0xF0000.
  // Note some CSM16 does not satisfy alignment request, so allocate a buffer of 2*4K
  // and adjust the base address myself.
  //
  Status = LegacyBios->GetLegacyRegion (
                         LegacyBios,
                         0x2000,
                         0,
                         0x1000,
                         &LegacyRegion
                         );
  ASSERT_EFI_ERROR (Status);

  if ((((UINTN)LegacyRegion) & 0xfff) != 0) {
    LegacyRegion = (VOID *)(UINTN)((((UINTN)LegacyRegion) + 0xfff) & ~((UINTN)0xfff));
  }

  //
  // Get the address map of startup code for AP,
  // including code size, and offset of long jump instructions to redirect.
  //
  ZeroMem (&AddressMap, sizeof (AddressMap));
  AsmGetHotAddCodeAddressMap (&AddressMap);

  WorkingBuffer = (EFI_PHYSICAL_ADDRESS) AllocateZeroPool (0x1000);

  //
  // Copy AP startup code to a working buffer first and then patch the long jump
  // offsets for mode switching.
  //
  CopyMem ((VOID *)WorkingBuffer, AddressMap.RendezvousFunnelAddress, AddressMap.Size);
  *(UINT32 *) (UINTN) (WorkingBuffer + AddressMap.FlatJumpOffset + 3) = (UINT32) ((UINTN)LegacyRegion + AddressMap.PModeEntryOffset);
  if (AddressMap.LongJumpOffset != 0) {
    *(UINT32 *) (UINTN) (WorkingBuffer + AddressMap.LongJumpOffset + 2) = (UINT32) ((UINTN)LegacyRegion + AddressMap.LModeEntryOffset);
  }


  // Initialize the CPU Init data and place it right after the startup vector code.
  CpuInitInfo = (CPU_INIT_INFO *) (UINTN) (WorkingBuffer + AddressMap.Size);
  ZeroMem ((VOID *) CpuInitInfo, sizeof (CPU_INIT_INFO));
  CpuInitInfo->BufferStart = (UINT32) (UINTN) LegacyRegion;
  CpuInitInfo->Cr3         =   (UINTN)Gen4GPageTable ();;  /// 0x 7fcd0000 -> needs to be updated with new page table created outside smm
  CopyMem ((VOID *) (UINTN) &CpuInitInfo->GdtrProfile, (VOID *) (UINTN) mAcpiCpuData->GdtrProfile, sizeof (IA32_DESCRIPTOR));
  CopyMem ((VOID *) (UINTN) &CpuInitInfo->IdtrProfile, (VOID *) (UINTN) mAcpiCpuData->IdtrProfile, sizeof (IA32_DESCRIPTOR));
  CpuInitInfo->MpExchangeInfoPtr = ExchangeInfo;

  // Now, copy the working buffer to the Legacy Region using Legacy BIOS protocol
  Status = LegacyBios->CopyLegacyRegion (
                         LegacyBios,
                         0x1000,
                         LegacyRegion,
                         (VOID *)(UINTN)WorkingBuffer
                         );

  CopyMem(SmmStartupDataArea, (VOID *)(UINTN)WorkingBuffer, 0x1000);

  FreePool((VOID *)WorkingBuffer);

  mCpuHotAddData.StartupVector = (EFI_PHYSICAL_ADDRESS)LegacyRegion;
  mCpuHotAddData.MpExchangeInfoPtr = ExchangeInfo;
  mCpuHotAddData.PackageSmmRelocate = PackageSmmRelocate;
  mCpuHotAddData.CpuPmStruct = &mLocalPpmStruct;
  mCpuHotAddData.SmmStartupCodeArea = (EFI_PHYSICAL_ADDRESS)SmmStartupDataArea;


  // Finally indicate that the Startup Vector code and data has been initialized.
  mCpuHotAddData.MpExchangeInfoPtr->InitFlag    = 2;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiCpuHotAddDataProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mCpuHotAddData
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


EFI_STATUS
AllocateMemoryBelow4G (
  IN   EFI_MEMORY_TYPE MemoryType,
  IN   UINTN           Size,
  OUT  VOID           **Buffer
  )
/*++

Routine Description:

  Allocate MemoryType below 4G memory address.

Arguments:

  Size   - Size of memory to allocate.
  Buffer - Allocated address for output.

Returns:

  EFI_SUCCESS - Memory successfully allocated.
  Other       - Other errors occur.

--*/
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;

  Pages = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  = (gBS->AllocatePages) (
                   AllocateMaxAddress,
                   MemoryType,
                   Pages,
                   &Address
                   );

  *Buffer = (VOID *) (UINTN) Address;

  return Status;
};


UINT32
Gen4GPageTable (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINTN                             i, j;
  UINT64                            *PDE, *PTE;
  UINT64                            *PML4;
  EFI_PHYSICAL_ADDRESS              PageTable;

  Status = AllocateMemoryBelow4G (EfiReservedMemoryType,  6 * EFI_PAGE_SIZE, (VOID **)&PageTable);

  ASSERT_EFI_ERROR (Status);

  //
  // Zero out all page table entries first
  //
  ZeroMem ((VOID*)(UINTN)PageTable, EFI_PAGES_TO_SIZE (6));

  //
  // This step is needed only in 64-bit mode
  //
  PML4 = (UINT64*)(UINTN)PageTable;
  PageTable += EFI_PAGE_SIZE;
  *PML4 = PageTable | IA32_PG_P | IA32_PG_RW;
  

  PDE = (UINT64*)(UINTN)PageTable;
  PTE = PDE + EFI_PAGE_SIZE / sizeof (*PDE);

  for (i = 0; i < 4; i++) {
    *PDE = (UINTN)PTE | IA32_PG_P |  IA32_PG_RW;
    PDE++;

    for (j = 0; j < EFI_PAGE_SIZE / sizeof (*PTE); j++) {
      *PTE = (((i << 9) + j) << 21) |
        IA32_PG_PS | IA32_PG_RW | IA32_PG_P;
      PTE++;
    }
  }

  //
  // This step is needed only in 64-bit mode
  //
  PageTable = (EFI_PHYSICAL_ADDRESS)(UINTN)PML4;
  
  return (UINT32)PageTable;
}

#ifdef LT_FLAG
VOID
LTSXHotPlug (
  VOID 
  )
/*++

Routine Description:

 Do LTSX CPU Hot-Plug setting here, include

 1. set LT.SPAD bit 61 for microcode SIPI function in OS
 2. LockConfig


Arguments:

  None  

Returns:
  None 

--*/    
{

	EFI_CPUID_REGISTER  Register;
  UINT32              Data32;
  UINT64              Ia32FeatureControl;
  UINT64              Bus_No_Msr;
  UINT32              Stack;
  UINT32              IioDfxLockAddr;
  BOOLEAN             IsLTSXEnabled;

	AsmCpuid (EFI_CPUID_VERSION_INFO, &Register.RegEax, &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);

  Data32 = LT_EXIST_VALUE;              // Use LT_EXIST as LTSX strapping enable or not
  IioDfxLockAddr = IIO_DFX_LCK_GLOBAL;  // IIO_DFX_LCK GLOBAL at Bx:D7:F7:0x504

  if(  (Data32) ) {   // check if is IVT-EX and lt-sx enabled
      IsLTSXEnabled = TRUE;
  }else {
      IsLTSXEnabled = FALSE;
  }

  Data32  = LT_SPAD_HIGH_VALUE;         // Check if ACm report any error so far
  Data32 &= (BIT31+BIT30);
  Ia32FeatureControl  = AsmReadMsr64(MSR_IA32_FEATURE_CONTROL);  // Check if SMXE enabled

  // Suppose IA32_FEATURE shoud be copied from BSP at InitializeFeatures  ()
  // sometimes it failed, patch here first
  if( (Data32 == BIT31) && (Ia32FeatureControl==0) ) {
     AsmWriteMsr64 (MSR_IA32_FEATURE_CONTROL, 0xFF07);
     Ia32FeatureControl = 0xFF07;
  }

  Bus_No_Msr =  AsmReadMsr64(MSR_BUS_NO);       // get IIO bus number
  if(Bus_No_Msr & BIT63) {                      // check if bus data is valid
    //do IIO DFX lock
    for(Stack = 0; Stack < MAX_IIO_STACK; Stack++) {  
      IioDfxLockAddr &= 0xF00FFFFF;             // clear Bus field
      IioDfxLockAddr |= (UINT32)(((UINT32)(RShiftU64(Bus_No_Msr, (Stack * 8))) & 0xFF) << 20); 
      *(volatile UINT32 *) (UINTN) (IioDfxLockAddr) = 0x000003FF; // set IIO_DFX_Lock
    }
  } else {                                      //we have invalid Bus# data
    //SKX TODO: caller has no idea how to handle the error
    //Temp WA to softhang for dedbug 
    CpuDeadLoop();                        
  } 

  // Check if:
  // 1. LT-SX strapping ON
  // 2. ACM not report error so far
  // 3. SMXE in IA32_FEATURE_CONTROL enabled
  if((IsLTSXEnabled == TRUE) &&                
     (Data32 == BIT31) &&    
     ((Ia32FeatureControl & FEATURE_SENTER_ENABLE) == FEATURE_SENTER_ENABLE) &&
     ((Ia32FeatureControl & FEATURE_VMON_IN_SMX)   == FEATURE_VMON_IN_SMX) ) {

        //SmiEnSave = __inbyte (0x430);
        //Data8 = SmiEnSave & 0xFE;   //~B_GBL_SMI_EN; Ddisable SMI Here ?
        //__outbyte  (0x430, Data8);
        //AcpiStall (4000);

        SetApicBSPBit (TRUE);

        ProgramVirtualWireMode_RAS (TRUE, 0x0);

        SendInterrupt (
            BROADCAST_MODE_ALL_EXCLUDING_SELF, 
            0, 
            0, 
            DELIVERY_MODE_INIT, 
            TRIGGER_MODE_EDGE, 
            TRUE
        );

        // Wait for all APs to response to the IPI
        MicroSecondDelay  (10000);
        // APTIOV_SERVER_RC_OVERRIDE_START : Use the AMI sdl value for ACM Base.
        LaunchBiosAcm ((UINTN)FV_ACM_BASE, (UINTN)LT_LOCK_CONFIG);
        // APTIOV_SERVER_RC_OVERRIDE_END : Use the AMI sdl value for ACM Base.

        //
        // Start AP's, or SMI will not be delivered.
        //
        SendInterrupt (
            BROADCAST_MODE_ALL_EXCLUDING_SELF,
            0,
            (UINT32) RShiftU64 (mCpuHotAddData.StartupVector, 12),   //t_test, working fine but TBD
            DELIVERY_MODE_SIPI,
            TRIGGER_MODE_EDGE,
            TRUE
        );

        SendInterrupt (
            BROADCAST_MODE_ALL_EXCLUDING_SELF,
            0,
            (UINT32) RShiftU64 (mCpuHotAddData.StartupVector, 12),  //t_test, working fine but TBD
            DELIVERY_MODE_SIPI,
            TRIGGER_MODE_EDGE,
            TRUE
        );

        //
        // Wait for all APs to response to the IPI
        //
        MicroSecondDelay  (10000);

        SetApicBSPBit (FALSE);

        Data32 = LT_SPAD_TXT_CPU_HOT_PLUG;     // 
        LT_SPAD_SET_HIGH_VALUE = Data32;       // set CPU hot-plug bit LT.SPAD[61]

  }// if((IsLTSXEnabled == TRUE)...

}  
#endif //LT_FLAG

