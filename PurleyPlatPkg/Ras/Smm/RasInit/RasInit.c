/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
  /*++

Copyright (c) 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  RasInit.c

Abstract:

Overall status of RAS event can be found in LIIO SPAD02 (B0:D20:F1:084h):

Here are the definitions for the lower word:

CpuRasCheckRequestValid     0x0001    "Checking to see if the CPU RAS request is valid."
CpuRasChangeMonarch         0x0002    "Requested CPU is the SMM monarch.  Changing monarch to next CPU package."
CpuRasOnlining              0x0003    "Checking QPI to see if requested onlining is possible."
CpuRasOnlineQpiCohUpdate    0x0050    "Adding CPU to coherency domain."
CpuRasOnlineWaitForMpInit   0x0051    "Waiting for hot added CPU to complete initialization."
CpuRasOnlineQpiDiscovery    0x0052    "Executing QPI topology discovery."
CpuRasOfflineQpiLinkUpdate  0x0053    "Severing QPI links to CPU."
CpuRasOfflinePowerOffFru    0x0054    "Cutting power to CPU."
CpuRasOnlineBringInPbsp     0x0055    "Send INIT/SIPI to hot added CPU."
CpuRasUpdateSmmList         0x0056    "Adding CPU to SMM list and notifying OS that new CPU is ready for OS to take control of it."
CpuRasOsNotification        0x0057    "Notifying OS of CPU changes."
CpuRasInitializeMemory      0x0060    "Initializing memory behind CPU."
CpuRasOnlinePowerOnFru      0x0061    "Powering on CPU socket."

EnumMemoryCopyExecute       0x010F    "Copying memory."
EnumMemoryOnlining          0x0114    "Onlining new memory."
EnumMemoryOfflining         0x0116    "Offlining memory."

CheckRequestValid           0x0201    "Checking to see if IIO RAS request is valid."
IohOnlineRequest            0x0220    "Powering on IIO."
IohOfflineRequest           0x0221    "Offlining IIO."
InitializeIoh               0x0222    "Initializing IIO and PCIe."
IohOnlineCheck              0x0223    "Waiting for power application to complete."
IohOnlineQpiDiscovery       0x0224    "Executing QPI topology discovery."
IohOfflineQpi               0x0225    "Offlining IIO."
IohOfflinePowerOff          0x0226    "Cutting power to IIO."

The upper word contains the overall status of InProgress or Completed.

InProgress                  0xFFFF
Completed                   0x0000-0xF000 If 0x000, then no error.  Else, contains the error code.

--*/
#include "RasInit.h"
#include <Library/PchCycleDecodingLib.h>

#pragma optimize("", off)

UINT32                  PmRegSmiEnSave;
UINT16                  PmRegAltSmiEnSave;
UINT16                  mAcpiBaseAddr;
BOOLEAN                 mIsSoftSmiRequest;
BOOLEAN                 mExitImmediately;
UINT32                  mSmiParam[4];
UINT8                   mFirstTimerEntry;
UINT8                   mRasTableIndex;
UINT8                   mQpiCpuSktHotPlugEn;
UINT8                   FirstCpuRasEntry;
UINT8                   FirstIioRasEntry;
UINT32                  *mStatusAddress;
BIOS_ACPI_PARAM         *mAcpiParameter;
UINT64                  mTsegUcBase = (UINT64)-1;
UINT8                   mHpRequest[6];
UINT32                  mSwSmiStatus;
BOOLEAN                 mSwSmiHandlerEnabled;
EFI_CPU_RAS_PROTOCOL                        *mCpuRas;
EFI_IIO_RAS_PROTOCOL                        *mIioRasProtocol;
EFI_RASF_SUPPORT_PROTOCOL                   *mRasfProtocol;
EFI_HP_IOX_ACCESS_PROTOCOL                  *mHpIOXAccess;
EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL   *mSmmPeriodicTimerDispatch;
static EFI_SMM_CPU_SERVICE_PROTOCOL                *mSmmCpuServiceProtocol;
EFI_HANDLE                                  mPeriodicTimerHandle;
EFI_ACPI_PCC_SHARED_MEMORY_REGION_RASF      *mRasfSharedMemoryRegion;
EFI_PLATFORM_RAS_POLICY_PROTOCOL            *mPlatformRasPolicyProtocol;
EFI_RAS_SYSTEM_TOPOLOGY                     *mRasTopology = NULL;
SYSTEM_RAS_SETUP                            *mRasSetup = NULL;
SYSTEM_RAS_CAPABILITY                       *mRasCapability = NULL;
EFI_IIO_UDS_PROTOCOL                        *mIioUds;
IIO_UDS                                     *mIioUdsData;
NEXT_STATE_ENTRY                            mNSE;
SPS_RAS_NOTIFY_PROTOCOL                     *mSpsRasNotifyProtocol;

UINTN RasDispatchTable[] = {

  (UINTN)&GetSourceOfGpi,
// APTIOV_SERVER_OVERRIDE_RC_START
#if  RAS_CPU_ONLINE_OFFLINE_ENABLE == 1
// APTIOV_SERVER_OVERRIDE_RC_END
  (UINTN)&CpuRasHandler,
// APTIOV_SERVER_OVERRIDE_RC_START
#endif
// APTIOV_SERVER_OVERRIDE_RC_END
  //(UINTN)&IioRasHandler,
  //(UINTN)&MemRasHandler,
  (UINTN)-1,

};

//
// Driver entry point
//

EFI_STATUS
EFIAPI
RasInitStart (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{

  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_GLOBAL_NVS_AREA_PROTOCOL *AcpiNvsProtocol = NULL;
    
  Status = gSmst->SmmLocateProtocol (&gEfiPlatformRasPolicyProtocolGuid, NULL, &mPlatformRasPolicyProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  mRasTopology = mPlatformRasPolicyProtocol->EfiRasSystemTopology;
  mRasSetup = mPlatformRasPolicyProtocol->SystemRasSetup;
  mRasCapability = mPlatformRasPolicyProtocol->SystemRasCapability; 

  if (mRasTopology->SystemInfo.SystemRasType != ADVANCED_RAS) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);
  mIioUdsData = (IIO_UDS *)mIioUds->IioUdsPtr;

  Status = gSmst->SmmLocateProtocol (&gEfiSmmPeriodicTimerDispatch2ProtocolGuid, NULL, (VOID **)&mSmmPeriodicTimerDispatch);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &AcpiNvsProtocol);
  ASSERT_EFI_ERROR (Status);
  mAcpiParameter = AcpiNvsProtocol->Area;
  ASSERT (mAcpiParameter);

  Status = gBS->LocateProtocol (&gEfiRasfSupportProtocolGuid, NULL, &mRasfProtocol);
  ASSERT_EFI_ERROR (Status);
  mRasfSharedMemoryRegion = mRasfProtocol->RasfSharedMemoryAddress;

  Status = gSmst->SmmLocateProtocol (&gEfiHpIoxAccessGuid, NULL, &mHpIOXAccess);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiCpuRasProtocolGuid, NULL, &mCpuRas);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiIioRasProtocolGuid, NULL, &mIioRasProtocol);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiSmmCpuServiceProtocolGuid, NULL, &mSmmCpuServiceProtocol);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gSpsRasNotifyProtocolGuid, NULL,&mSpsRasNotifyProtocol);

  mQpiCpuSktHotPlugEn = mRasSetup->Qpi.QpiCpuSktHotPlugEn;
    
  // Init Memory Ras
  InitMemoryRas ();

  InitHpGPI();

  EnableGpiChildHandler ();
  
  if (IsSimicsPlatform() == TRUE) {
    EnableSwSmiCpuRasHandler ();
  }

  return EFI_SUCCESS;
}

VOID
InitHpGPI(
  VOID
  )
{
  UINT32 Data32;
  GPIO_CONFIG                 GpioPadConfig;
    
  ZeroMem (&GpioPadConfig, sizeof(GPIO_CONFIG));
  GpioGetPadConfig (PCH_CPU_HP_SMI_GPP_PAD, &GpioPadConfig);
  
  // set the GPIO PAD mode to GPIO
  GpioPadConfig.PadMode = GpioPadModeGpio;  

  // Set the GPIO PAD as IN and enable INV
  GpioPadConfig.Direction = GpioDirInInv;

  // Enable SMI Interrupt
  GpioPadConfig.InterruptConfig = GpioIntSmi | GpioIntLevel;

  // Set the Pad Ownership to ACPI
  GpioPadConfig.HostSoftPadOwn = GpioHostOwnAcpi;

  // Do the PAD configuration
  GpioSetPadConfig (PCH_CPU_HP_SMI_GPP_PAD, &GpioPadConfig);

  // Enable SMI generation at SMI_EN
  GpioSetGpiSmiPadEn(PCH_CPU_HP_SMI_GPP_PAD, V_PCH_GPIO_SMI_EN);

  // Get PM ACPI Base Address (Pmbase)
  PchAcpiBaseGet (&mAcpiBaseAddr);

  // Setup the GBL_SMI_EN=1 to only cause SMI.
  Data32 = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_EN));
  Data32 |= B_PCH_SMI_EN_GBL_SMI;
  IoWrite32 ((mAcpiBaseAddr + R_ACPI_SMI_EN), Data32);

  // clear the status
  GpioClearGpiSmiSts(PCH_CPU_HP_SMI_GPP_PAD);

     
}

EFI_STATUS
EnableGpiChildHandler (
  )
{

  EFI_STATUS                    Status = EFI_SUCCESS;

  EFI_SMM_GPI_DISPATCH2_PROTOCOL *mGpiDispatch;
  EFI_SMM_GPI_REGISTER_CONTEXT  mGpiContext;
  EFI_HANDLE                    GpiHandle;

  //
  // Register SMI handler through a child protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmGpiDispatch2ProtocolGuid, NULL, &mGpiDispatch); 
  ASSERT_EFI_ERROR (Status);


  GpioGetGpiSmiNum (PCH_CPU_HP_SMI_GPP_PAD, &mGpiContext.GpiNum);  

  Status = mGpiDispatch->Register (
                          mGpiDispatch,
                          RasInitGpiChildCallback,
                          &mGpiContext,
                          &GpiHandle
                          );
  ASSERT_EFI_ERROR (Status);

  return Status;
}


EFI_STATUS
RasInitGpiChildCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID     *mGpiContext,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
 
  EFI_SMM_GPI_REGISTER_CONTEXT *Context;
  UINT32                       GpioSmiStsValue;
    
  Context = (EFI_SMM_GPI_REGISTER_CONTEXT *)mGpiContext;

  GpioGetReg (PCH_CPU_HP_SMI_GPP_GROUP, R_PCH_PCR_GPIO_GPP_E_SMI_STS, &GpioSmiStsValue);
  
  if(GpioSmiStsValue & (1 << GpioGetPadNumberFromGpioPad(PCH_CPU_HP_SMI_GPP_PAD))) {
    HpGPIHandler ();
  }
  return EFI_SUCCESS;
}


VOID
HpGPIHandler(
)
{
  BOOLEAN     OnlineOfflineRequestFlag;
  UINT32      Data32 = 0;
  UINTN       Idx;
  EFI_STATUS  Status;
  
  DEBUG_ACCESS(0x9FFF0);

  //
  // If it is the middle of another RAS service, do not response to the new request
  //
  if (mPeriodicTimerHandle) {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nAlready processing a RAS periodic timer event. Ignoring this event!\n"));
    return;
  }

  //
  // Check the cause of the SMI
  // It could be the real RAS HPGPI SMI triggered by hardware
  // or faked HPGPI SMI requested from OS or other RAS modules
  // The difference is the INV bit
  // If it is 1, then it is the real  HPGPI SMI
  // If it is 0, then it is the faked HPGPI SMI
  //
   
  DEBUG     ((EFI_D_INFO, "\n\n  RAS FLOW START\n  RAS FLOW START\n  RAS FLOW START\n\n"));

  Status = GpioGetInputInversion(PCH_CPU_HP_SMI_GPP_PAD, &Data32);

  if (mSwSmiHandlerEnabled == TRUE) {
    Data32 = 0;
  }

  if (!(Data32)) {//Soft SMI detected
    GpioSetInputInversion(PCH_CPU_HP_SMI_GPP_PAD, V_PCH_GPIO_RXINV_YES);

    if (mAcpiParameter->SmiRequestParam[0] > 0) {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nThe request is a Soft SMI Request\n"));
      mIsSoftSmiRequest = TRUE;
    } else {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nRogue Soft SMI Detected.\n"));
      for (Idx=0; Idx < 4; Idx++) { //Clear out these parameters or we'll continue to process Rogue SMIs.
        mAcpiParameter->SmiRequestParam[Idx] = 0;
      }
      mIsSoftSmiRequest = FALSE;
    }
  } else {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nThe request is a Real SMI Request\n"));
    mIsSoftSmiRequest = FALSE;
  }

  //
  // Check cause of GPI source
  //
  Status = IN_PROGRESS;
  OnlineOfflineRequestFlag  = FALSE;
  mStatusAddress = NULL;
  if (mIsSoftSmiRequest) {
    //
    // Zero out the request data structure so that caller will know it acknowledged the request
    //
    for (Idx = 0; Idx < 4; Idx++) {
      mSmiParam[Idx] = mAcpiParameter->SmiRequestParam[Idx];
    }
    if (mSmiParam[0]) {
      OnlineOfflineRequestFlag = TRUE;
    }
    if (mSmiParam[3]) {
       mStatusAddress  = (UINT32*)(UINTN)mSmiParam[3];
      //
      // All errors returned at this address need to be limited to 1 byte
      // for compatibility with various OS interfaces.
      //
      *mStatusAddress  = (UINT8) Status;
    }
  } else {
    mAcpiParameter->SmiRequestParam[0] = (UINT32)(-1);
    OnlineOfflineRequestFlag = mHpIOXAccess->CheckOnlineOfflineRequest();
  }

  mSmiParam[3] = (UINT32)Status;
  mAcpiParameter->SmiRequestParam[3] = (UINT32)Status;

  // If not related to RAS HP, return.
  if (OnlineOfflineRequestFlag == FALSE) {
    DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "    Confirmed that no RAS action detected. RAS FLOW COMPLETE.\n"));
    return;
  }

if (mSwSmiHandlerEnabled == FALSE) {
  // Setup periodic SMI handler and Disable all other sources SMI.
  Status  = gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (mAcpiBaseAddr + R_ACPI_SMI_EN), 1, &PmRegSmiEnSave);
  Data32  = PmRegSmiEnSave & (BIT31 + BIT30 + BIT0 + BIT1);
  Status  = gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (mAcpiBaseAddr + R_ACPI_SMI_EN), 1, &Data32);
  Data32  |= BIT14; //Periodic SMI Enable
  Status  = gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (mAcpiBaseAddr + R_ACPI_SMI_EN), 1, &Data32);
  
  //
  // TO DO:  Need to add code to save SMI setting on all GPIOs and disable SMI on all GPIO
  //

  // Register periodic SMI handler
  RegisterPeriodicTimer();
} else {
  RasSmiHandler ();
}

}

VOID
RegisterPeriodicTimer(
  VOID
  )
{
  EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT   PeriodicTimer;
  EFI_STATUS  Status;

  mFirstTimerEntry  = 01;
 
  if(!mPeriodicTimerHandle) {
    PeriodicTimer.Period = 160000;
    PeriodicTimer.SmiTickInterval = 160000;
    Status = mSmmPeriodicTimerDispatch->Register(mSmmPeriodicTimerDispatch, RasPeriodicChildCallback, &PeriodicTimer, &mPeriodicTimerHandle);
    ASSERT_EFI_ERROR (Status);
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nRAS Periodic Handler Registered\n"));
  }
}


EFI_STATUS
RasPeriodicChildCallback (
  IN  EFI_HANDLE    PeriodicTimerHandle,
  IN  CONST VOID    *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer  OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  )
{

  // Call the RAS periodic timer handler.
  RasPeriodicTimerSmiHandler();

  return EFI_SUCCESS;
}


EFI_STATUS
RasPeriodicTimerSmiHandler(
  VOID
  )
{
  EFI_STATUS  Status;
  BOOLEAN     UnRegister;
  UINT32      RegData;
  UINT32      Idx;
  UINT32      GpioSmiStsValue;

  Status      = EFI_SUCCESS;
  UnRegister  = FALSE;

  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nRasInit - RasPeriodicTimerSmiHandler \n"));

  if(mFirstTimerEntry)  {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nRasInit - First Periodic Timer SMI Handler entry\n"));
    mFirstTimerEntry  = 00;

    //
    // First time call initialize with the start of the RasDispatcherTable.
    //
    FirstCpuRasEntry      = 1;
    mRasTableIndex        = 0;
    mExitImmediately      = 0;
    mNSE.NextEntryPointer = (NEXT_ENTRY_POINTER)RasDispatchTable[mRasTableIndex];
  }

  //
  // Call the entry in the dispatcher table
  //
  Status  = mNSE.NextEntryPointer();

  if (Status != IN_PROGRESS) {
    //RasDevHideBack();
    if (Status != NO_REQUEST) {
      mSmiParam[3] = (UINT32)Status;
    }

    //
    // Goto the next entry in the dispatcher anyway.
    //
    mRasTableIndex++;
    mNSE.NextEntryPointer  = (NEXT_ENTRY_POINTER)RasDispatchTable[mRasTableIndex];
  }

  //
  // Unregister the periodic SMI handler if it is last entry.
  //
  if(((UINTN)RasDispatchTable[mRasTableIndex] == (UINTN)-1) || (mExitImmediately)) {
      UnRegister  = TRUE;
  }

  if(mPeriodicTimerHandle)  {
    if (UnRegister == TRUE) {

      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nUnregistering the Periodic Timer.\n"));

      Status = mSmmPeriodicTimerDispatch->UnRegister(mSmmPeriodicTimerDispatch, mPeriodicTimerHandle);
      mPeriodicTimerHandle  = NULL;
      ASSERT_EFI_ERROR (Status);

      //
      // Restore the PM SMI enable registers.
      //
      Status  = gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (mAcpiBaseAddr + R_ACPI_SMI_EN), 1, &PmRegSmiEnSave);

      //
      // Clear interrupts in IO Expander
      //
      mHpIOXAccess->ClearInterrupt(FALSE);
      
      GpioGetReg (PCH_CPU_HP_SMI_GPP_GROUP, R_PCH_PCR_GPIO_GPP_E_SMI_STS, &GpioSmiStsValue);
      GpioSmiStsValue |= (1 << GpioGetPadNumberFromGpioPad(PCH_CPU_HP_SMI_GPP_PAD));
      GpioSetReg (PCH_CPU_HP_SMI_GPP_GROUP, R_PCH_PCR_GPIO_GPP_E_SMI_STS, GpioSmiStsValue);
      
      // Indicate to OS that we've completed the RAS operation.  If Status == 0, then no error.
      if (mSmiParam[3] == IN_PROGRESS) {
        Status = ERROR_EXIT;
        mSmiParam[3] = (UINT32)Status;
      }

      mAcpiParameter->SmiRequestParam[3] = mSmiParam[3];

      if (mStatusAddress) {
        if (mSmiParam[3]) {
          *(UINT32 *)mStatusAddress = mSmiParam[3];
          mRasfSharedMemoryRegion->Status.Bits.CmdComplete = 1;
          mRasfSharedMemoryRegion->Status.Bits.Error = 1;
          mRasfSharedMemoryRegion->Status.Bits.Status = 4;
        } else {
          *(UINT32 *)mStatusAddress = 0;
          mRasfSharedMemoryRegion->Status.Bits.CmdComplete = 1;
          mRasfSharedMemoryRegion->Status.Bits.SciDoorBell = 1;
          mRasfSharedMemoryRegion->Status.Bits.Status = 0;
        }
      }

      // Clear out all the OS requests.
      for (Idx = 0 ;Idx < 3; Idx++) {
        mAcpiParameter->SmiRequestParam[Idx] = 0;
        mSmiParam[Idx] = 0;
      }

      // Clear the mHpRequest structure
      for (Idx=0; Idx < 6; Idx++) { 
        mHpRequest[Idx] = 0;
      }

      //Set to FALSE so the next Valid SMI starts fresh, or it will prematurely exit on the next try.
      UnRegister = FALSE; 
      mExitImmediately = FALSE;

      DEBUG ((EFI_D_INFO, "\n  RAS FLOW HAS COMPLETED!!\n  RAS FLOW HAS COMPLETED!!\n  RAS FLOW HAS COMPLETED!!\n\n"));
    
    } else {
      //
      // WA: SW periodic SMI timer will not work if the timer expires during the debug halt
      // Here try to reset the timer to trigger the next SMI
      //
      gSmst->SmmIo.Io.Read  (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_ACPI_SMI_EN, 1, &RegData);
      RegData = RegData & (~B_SWSMI_TMR_EN);
      gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_ACPI_SMI_EN, 1, &RegData);
      RegData = RegData | B_SWSMI_TMR_EN;
      gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_ACPI_SMI_EN, 1, &RegData);
    }
  }
  return EFI_SUCCESS;
}


EFI_STATUS
GetSourceOfGpi(
  VOID
)
{
  UINT8       Idx;
  EFI_STATUS  Status;

  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n Executing first RAS handler function - GetSourceOfGpi\n"));

  Idx = *((volatile UINT8 *)(UINTN)0x9FFFF);

  if (mIsSoftSmiRequest) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "It is SoftSmiRequest\n"));
    if (mSmiParam[0] == SMI_CPU_ONLINE_OFFLINE || mSmiParam[0] == SMI_CPU_HOT_ADD_REMOVE) {
      //
      // Override the HpRequest with the user inputted value for socketID.
      //
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "It is SoftSmiRequest for CPU Online/Offline\n"));
      if (mSmiParam[1] & BIT0)  { // Cpu hot removal
        mHpRequest[0] = 0x0F & (UINT8)(~(1 << mSmiParam[2]) & (UINT8)mIioUdsData->SystemStatus.socketPresentBitMap);
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "It is SoftSmiRequest for CPU Offline\n"));
      } else {                 // Cpu hot add
        mHpRequest[0] = 0x0F & (UINT8)((1 << mSmiParam[2])  | (UINT8)mIioUdsData->SystemStatus.socketPresentBitMap);
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "It is SoftSmiRequest for CPU Online\n"));
      }
    } else {
      mHpRequest[0] = 0;
      mHpRequest[1] = 0;
      mHpRequest[2] = 0;
      mHpRequest[3] = 0;
      mHpRequest[4] = 0;
    }
  } else {
    Status = mHpIOXAccess->ReadIoxAndUpdateHpRequest(mHpRequest,TRUE);
    if(Status == IN_PROGRESS) {
      return IN_PROGRESS;
    }
  }

  return STATUS_DONE;
}

//
// The following functions are for debugging CPU RAS on Simics
// Since GPIO SMI generation and Periodic SMI are not working under Simics,
//  we are using SoftSMI based execution for CPU RAS

VOID
EnableSwSmiCpuRasHandler (
  VOID
 )
{
  EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT       SmmSwContext;
  EFI_HANDLE                         Handle = NULL;
  EFI_STATUS                        Status = EFI_SUCCESS;

  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
  ASSERT_EFI_ERROR(Status);

  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n Registering SwSmiCpuRasHandler\n"));

  // Register CPU RAS SW SMM Handler
  SmmSwContext.SwSmiInputValue = 0xAA;
  Handle = NULL;
  Status = SwDispatch->Register (SwDispatch, RasInitSwSmiChildCallback, &SmmSwContext, &Handle);
   

  return;

}


EFI_STATUS
RasInitSwSmiChildCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID     *mGpiContext,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{

  UINT8 Data;
  
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nDebugging CPU online in SIMICS\n"));

  Data = IoRead8 (R_PCH_APM_STS);

  if (Data == 0xFF) { 
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n SwSMI parameter is %x\n", Data));

    // The following need to be set for the CPU RAS handler to initiate CPU1 online
    mAcpiParameter->SmiRequestParam[0] = SMI_CPU_ONLINE_OFFLINE;
    mAcpiParameter->SmiRequestParam[1] = 0;  // Online
    mAcpiParameter->SmiRequestParam[2] = 1;  // Skt 1
    mAcpiParameter->SmiRequestParam[3] = mSwSmiStatus;

    mFirstTimerEntry  = 01;
    mHpRequest[1] = 3;

    mSwSmiHandlerEnabled = TRUE;
         
    HpGPIHandler();

  }

  return EFI_SUCCESS;
}

EFI_STATUS
RasSmiHandler(
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  BOOLEAN     UnRegister = FALSE;
  UINT32      Idx;

  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nRasInit - RasSmiHandler \n"));

  //
  // First time call initialize with the start of the RasDispatcherTable.
  //
  FirstCpuRasEntry      = 1;
  mRasTableIndex        = 0;
  mExitImmediately      = 0;
  mNSE.NextEntryPointer = (NEXT_ENTRY_POINTER)RasDispatchTable[mRasTableIndex];
  
  while (1) {
    //
    // Call the entry in the dispatcher table
    //
    Status  = mNSE.NextEntryPointer();

    if (Status != IN_PROGRESS) {
      //RasDevHideBack();
      if (Status != NO_REQUEST) {
        mSmiParam[3] = (UINT32)Status;
      }

      //
      // Goto the next entry in the dispatcher anyway.
      //
      mRasTableIndex++;
      mNSE.NextEntryPointer  = (NEXT_ENTRY_POINTER)RasDispatchTable[mRasTableIndex];
    }

    //
    // Unregister the periodic SMI handler if it is last entry.
    //
    if(((UINTN)RasDispatchTable[mRasTableIndex] == (UINTN)-1) || (mExitImmediately)) {
       UnRegister  = TRUE;
    }

    if (UnRegister == TRUE) {

      //
      // Restore the PM SMI enable registers.
      //
      Status  = gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (mAcpiBaseAddr + R_ACPI_SMI_EN), 1, &PmRegSmiEnSave);

      mHpIOXAccess->ClearInterrupt(FALSE);
      
      // Indicate to OS that we've completed the RAS operation.  If Status == 0, then no error.
      if (mSmiParam[3] == IN_PROGRESS) {
        Status = ERROR_EXIT;
        mSmiParam[3] = (UINT32)Status;
      }

      mAcpiParameter->SmiRequestParam[3] = mSmiParam[3];

      if (mStatusAddress) {
        if (mSmiParam[3]) {
          *(UINT32 *)mStatusAddress = mSmiParam[3];
          mRasfSharedMemoryRegion->Status.Bits.CmdComplete = 1;
          mRasfSharedMemoryRegion->Status.Bits.Error = 1;
          mRasfSharedMemoryRegion->Status.Bits.Status = 4;
        } else {
          *(UINT32 *)mStatusAddress = 0;
          mRasfSharedMemoryRegion->Status.Bits.CmdComplete = 1;
          mRasfSharedMemoryRegion->Status.Bits.SciDoorBell = 1;
          mRasfSharedMemoryRegion->Status.Bits.Status = 0;
        }
      }

      // Clear out all the OS requests.
      for (Idx = 0 ;Idx < 3; Idx++) {
        mAcpiParameter->SmiRequestParam[Idx] = 0;
        mSmiParam[Idx] = 0;
      }

      // Clear the mHpRequest structure
      for (Idx=0; Idx < 6; Idx++) { 
        mHpRequest[Idx] = 0;
      }

      //Set to FALSE so the next Valid SMI starts fresh, or it will prematurely exit on the next try.
      UnRegister = FALSE; 
      mExitImmediately = FALSE;

      DEBUG ((EFI_D_INFO, "\n  RAS FLOW HAS COMPLETED!!\n  RAS FLOW HAS COMPLETED!!\n  RAS FLOW HAS COMPLETED!!\n\n"));
      break;
    
    } 
  }
  return EFI_SUCCESS;
}

