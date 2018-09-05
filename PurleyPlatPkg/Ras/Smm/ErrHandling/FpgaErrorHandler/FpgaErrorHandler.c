/** @file
  Implementation of the FPGA Error Handler.

  Copyright (c) 2009-2016 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
#include "FpgaErrorHandler.h"
#include <FpgaRegs.h>
#include <Fpga.h>
#include <Cpu/CpuBaseLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/GpioLib.h>
#include <Library/ResetSystemLib.h>
#include <IncludePrivate/Library/GpioPrivateLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/mpsyncdatalib.h>
#include <Protocol/ErrorHandlingProtocol.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmVariable.h>
#include <CommonErrorHandlerDefs.h>
#include <UncoreCommonIncludes.h>
#include <PchAccess.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/WheaDefs.h>
#include <GpioPinsSklH.h>
#include <UsraAccessApi.h>

// Module data
EFI_ERROR_HANDLING_PROTOCOL           *mErrorHandlingProtocol;
EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;
EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccessProtocol;
EFI_SMM_VARIABLE_PROTOCOL             *mSmmVariable = NULL;
REPORTER_PROTOCOL                      mReporter;
SYSTEM_INFO                           *SystemInfo;

UINT32                                FMEBar[MAX_SOCKET];
CONST   UINT32                        FpgaErrorMask = (1 << 5) || (1 << 4);
CONST   UINT16                        KtiErrorEnRegs[] = {KTI_ERROR_EN0, KTI_ERROR_EN1, KTI_ERROR_EN2, KTI_ERROR_EN3, KTI_ERROR_EN4, KTI_ERROR_EN5, 0};
CONST   UINT16                        KtiErrorStsRegs[] = {KTI_ERROR_STAT0, KTI_ERROR_STAT1, KTI_ERROR_STAT2, KTI_ERROR_STAT3, KTI_ERROR_STAT4, KTI_ERROR_STAT5, 0};

#define PCH_CPU_HP_SMI_GPP_GROUP  GPIO_SKL_H_GROUP_GPP_E

/**
  Read from FME register
  @param[in]  Socket      Target socket
  @param[in]  Address     FME register address
  @param[out] Data        Data read from FME register

  @retval    Status.
**/
EFI_STATUS
ReadFmeCsr32 (
  UINT8   Socket,
  UINT16  Address,
  UINT32  *Data
  )
{
	  if (Socket >= MAX_SOCKET || FMEBar[Socket] == 0)
		return EFI_UNSUPPORTED;

	  *Data = MmioRead32((UINTN)(FMEBar[Socket] + Address));
	  return EFI_SUCCESS;
}

/**
  Write to FME register
  @param[in] Socket      Target socket
  @param[in] Address     FME register address
  @param[in] Data        Data to be written

  @retval    Status.
**/
EFI_STATUS
WriteFmeCsr32 (
  UINT8   Socket,
  UINT16  Address,
  UINT32  Data

  )
{
  if (Socket >= MAX_SOCKET || FMEBar[Socket] == 0)
	return EFI_UNSUPPORTED;

  MmioWrite32((UINTN)(FMEBar[Socket] + Address), Data);
  return EFI_SUCCESS;
}

/**
  Read from FME register
  @param[in]  Socket      Target socket
  @param[in]  Address     FME register address
  @param[out] Data        Data read from FME register

  @retval    Status.
**/
EFI_STATUS
ReadFmeCsr64 (
  UINT8   Socket,
  UINT16  Address,
  UINT64  *Data
  )
{
	  if (Socket >= MAX_SOCKET || FMEBar[Socket] == 0)
		return EFI_UNSUPPORTED;

	  *Data = MmioRead64((UINTN)(FMEBar[Socket] + Address));
	  return EFI_SUCCESS;
}

/**
  Write to FME register
  @param[in] Socket      Target socket
  @param[in] Address     FME register address
  @param[in] Data        Data to be written

  @retval    Status.
**/
EFI_STATUS
WriteFmeCsr64 (
  UINT8   Socket,
  UINT16  Address,
  UINT64  Data

  )
{
  if (Socket >= MAX_SOCKET || FMEBar[Socket] == 0)
	return EFI_UNSUPPORTED;

  MmioWrite64((UINTN)(FMEBar[Socket] + Address), Data);
  return EFI_SUCCESS;
}

/**
  Create the processor error record
  @param[in] ErrSts     - A pointer to the Processor Error Record.

  @retval Sev   Severity

**/
EFI_STATUS
LogFpgaError (
  UINT8  Socket
  )
{
  EFI_STATUS                       Status;
  FPGA_ERROR_LOG                   FpgaErrorLog;
  UINT8                            i;

  ZeroMem(&FpgaErrorLog, sizeof(FpgaErrorLog));

  RASDEBUG ((DEBUG_ERROR, "Logging FPGA error status registers...\n"));

  FpgaErrorLog.Valid = TRUE;
  ReadFmeCsr64(Socket, FME_ERROR, &(FpgaErrorLog.FmeErrorStatus));
  RASDEBUG ((DEBUG_ERROR, "FME_ERROR_STATUS: 0x%xL.\n", FpgaErrorLog.FmeErrorStatus));

  ReadFmeCsr32(Socket, KTI_FERR, &(FpgaErrorLog.KtiFerr));
  RASDEBUG ((DEBUG_ERROR, "FME_KTI_FERR: 0x%x.\n", FpgaErrorLog.KtiFerr));

  ReadFmeCsr32(Socket, KTI_NERR, &(FpgaErrorLog.KtiNerr));
  RASDEBUG ((DEBUG_ERROR, "FME_KTI_NERR: 0x%x.\n", FpgaErrorLog.KtiNerr));

  for(i = 0; KtiErrorStsRegs[i] != 0; i++) {
	ReadFmeCsr32(Socket, FME_ERROR, &(FpgaErrorLog.KtiErrorStat[i]));
	RASDEBUG ((DEBUG_ERROR, "KTI_ERR_STAT%n: 0x%x.\n", FpgaErrorLog.KtiErrorStat[i]));
  }

  Status = mSmmVariable->SmmSetVariable (
	  FPGA_ERR_VARIABLE_NAME,
      &gFpgaErrorRecordGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
      sizeof(FPGA_ERROR_LOG),
      &FpgaErrorLog
  );

  if (EFI_ERROR(Status)) {
	RASDEBUG ((DEBUG_ERROR, __FILE__" Saving FpgaErrorLog failed.\n"));
  }

  return EFI_SUCCESS;
}

/**
  Indicates if FPGA error report is supported

  @retval    BOOLEAN.
**/
BOOLEAN
FpgaErrorIsSupported (
  VOID
  )
 {
    return SystemInfo->FpgaSktActive != 0;
 }

/**
  Enable FPGA error reporting feature.

  @retval    Status.
**/
EFI_STATUS
FpgaErrorEnableReporting (
  VOID
  )
 {
    UINT8 Socket;
    UINT8 i;

    for (Socket = 0; Socket <MAX_SOCKET; Socket++) {
      if (!(SystemInfo->FpgaSktActive & (1 << Socket))) {
    	continue;
      }
   	  WriteFmeCsr64(Socket, FME_ERROR_MASK, 0x1L);
      for (i = 0; KtiErrorEnRegs[i] != 0; i++) {
    	WriteFmeCsr32(Socket, KtiErrorEnRegs[i], 0xFFFFFFFF);
      }
    }

    return EFI_SUCCESS;
 }

/**
  Disable FPGA error reporting feature.

  @retval    Status.
**/
EFI_STATUS 
FpgaErrorDisableReporting (
  UINT32 DeviceType,
  UINT32 EventType,
  UINT32 *ExtData
  )
 {
    UINT8 Socket;
    UINT8 i;

    for (Socket = 0; Socket <MAX_SOCKET; Socket++) {
      if (!(SystemInfo->FpgaSktActive & (1 << Socket))) {
      	continue;
      }
      WriteFmeCsr64 (Socket, FME_ERROR_MASK, 0x0);
      for (i = 0; KtiErrorEnRegs[i] != 0; i++) {
    	WriteFmeCsr32(Socket, KtiErrorEnRegs[i], 0);
      }
    }
   return EFI_SUCCESS;
}

/**
  Clear FPGA related error. This only way to clear FPGA error is power cycle. So, cold reset will be issued within this func.

  @retval    BOOLEAN.  Indicating if error is found on FPGA.
**/
BOOLEAN
FpgaErrorCheckStatus (
  VOID
  )
{
  EFI_STATUS Status;
  UINT32  GpioSmiSts;

  //if (SystemInfo->FpgaSktActive == 0)
  //	return FALSE;

  Status = GpioGetReg(PCH_CPU_HP_SMI_GPP_GROUP, R_PCH_PCR_GPIO_GPP_E_SMI_STS, &GpioSmiSts);

  if (EFI_ERROR(Status)) {
	RASDEBUG ((DEBUG_ERROR, __FILE__" Gpio read has failed\n"));
	return FALSE;
  }

  return (GpioSmiSts & FpgaErrorMask) != 0;
}

/**
  Clear FPGA related error. This only way to clear FPGA error is power cycle. So, cold reset will be issued within this func.

  @retval    Status.
**/

EFI_STATUS 
FpgaErrorClearStatus (
  VOID
  )
 {
   EFI_STATUS Status;
   UINT32  GpioSmiSts;

   Status = GpioGetReg(PCH_CPU_HP_SMI_GPP_GROUP, R_PCH_PCR_GPIO_GPP_E_SMI_STS, &GpioSmiSts);

   if (EFI_ERROR(Status)) {
		RASDEBUG ((DEBUG_ERROR, __FILE__" Gpio read has failed\n"));
   }

   GpioSetReg(PCH_CPU_HP_SMI_GPP_GROUP, R_PCH_PCR_GPIO_GPP_E_SMI_STS, GpioSmiSts & FpgaErrorMask); //The SMI_STS reg is W1CS

   //FPGA error status registers are stick and read-only. Perform cold reset to clear them.
   RASDEBUG ((DEBUG_ERROR, "Request code reboot for clearing FPGA error\n"));
   ResetCold();

   //Should not reach here.
   ASSERT (FALSE);
   EFI_DEADLOOP();
   return EFI_SUCCESS;
 }

/**
  Detect and Handle FPGA related error.

  @param[in] CurrentSev  Highest error severity detected.
  @param[in] Clear       Option for clear error. Does not apply to FPGA error handler.

  @retval    Status.
**/
 
EFI_STATUS
FpgaErrorDetectAndHandle (
  UINT32          *CurrentSev,
  ClearOption      Clear
  )
 {
   EFI_STATUS                Status;
   UINT8                     Socket;
   UINT32                    Data32;
   UINT64                    Data64;

   for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
	 if (! (SystemInfo->FpgaSktActive & (1 << Socket))){
	   continue;
	 }

	 //Check for FME error
     Status = ReadFmeCsr64(Socket, FME_ERROR, &Data64);
     if (EFI_ERROR(Status))
       return Status;

     if (Data64) {
       RASDEBUG ((DEBUG_ERROR, __FILE__"FPGA internal Error Detected on Socket %n. FME_ERROR = %x\n", Socket, Data64));
       LogFpgaError(Socket);
        *CurrentSev = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
     }

     //Check for KTI error
     Status = ReadFmeCsr32 (Socket, KTI_FERR,  &Data32);

     if (EFI_ERROR(Status))
       return Status;

     if (Data32) {
       RASDEBUG ((DEBUG_ERROR, __FILE__"FPGA UPI Error Detected on Socket %n. KTI_FERR = %x\n", Socket, Data32));
       LogFpgaError(Socket);
       *CurrentSev = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
     }
   }
   return EFI_SUCCESS;
 }

/**
  This function is a callback for SmmReadyToLock. It stores FME BAR for each socket for later use.

  @param[in] Protocol    Protocol unique ID.
  @param[in] Interface   Interface instance.
  @param[in] Handle      The handle on which the interface is installed..

  @retval    Status.
**/
EFI_STATUS
FpgaErrorEnableCallback (
  IN      CONST EFI_GUID   *Protocol,
  IN      VOID             *Interface,
  IN      EFI_HANDLE        Handle
  )
{
  UINT8                        Socket;
  UINT8                        RCiEP0BusNumber;
  UINT32                       Data;
  USRA_ADDRESS                 Address;

  ZeroMem (&FMEBar[0], sizeof(FMEBar));

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (!(SystemInfo->FpgaSktActive & (1 << Socket))) {
      continue;
    }

    RCiEP0BusNumber = SystemInfo->SocketInfo[Socket].UncoreIioInfo.IioStackInfo[IIO_PSTACK3].BusLimit - FPGA_PREAllOCATE_BUS_NUM + 0x01;
    USRA_PCIE_SEG_ADDRESS (Address, UsraWidth32, Socket, RCiEP0BusNumber, 0, 0, R_FPGA_FME_BAR);
    RegisterRead(&Address, &Data);
    FMEBar[Socket] = Data & 0xFFFFFFF0;
  }
  return EFI_SUCCESS;

}

/**
  Entry point for the FPGA Handler initialization.

  This function initializes the error handling and enables error
  sources for the FPGA.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/
EFI_STATUS
EFIAPI
InitializeFpagErrorHandler (
  IN      EFI_HANDLE          ImageHandle,
  IN      EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS     Status;
  VOID          *Registration;

  RASDEBUG ((DEBUG_ERROR, "InitializeFpgaErrHandler\n"));

  Status = gSmst->SmmLocateProtocol (&gEfiErrorHandlingProtocolGuid, NULL, &mErrorHandlingProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (&gEfiPlatformErrorHandlingProtocolGuid, NULL, &mPlatformErrorHandlingProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (&gEfiPlatformRasPolicyProtocolGuid, NULL, &mPlatformRasPolicyProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SystemInfo = &(mPlatformRasPolicyProtocol->EfiRasSystemTopology->SystemInfo);

  Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccessProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, &mSmmVariable);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
	return Status;
  }

  // Setup callback after pci space initialized. Save FME BAR before system boot to OS for later usage.
  Status = gSmst->SmmRegisterProtocolNotify (&gEfiSmmReadyToLockProtocolGuid, FpgaErrorEnableCallback, &Registration);

  if (FpgaErrorIsSupported()) {
    mReporter.IsSupported = FpgaErrorIsSupported;
    mReporter.EnableReporting = FpgaErrorEnableReporting;
    mReporter.DisableReporting = FpgaErrorDisableReporting;
    mReporter.CheckStatus = FpgaErrorCheckStatus;
    mReporter.ClearStatus = FpgaErrorClearStatus;
    mReporter.DetectAndHandle = FpgaErrorDetectAndHandle;

    mErrorHandlingProtocol->RegisterHandler (&mReporter, HighPriority);  
  }


  return EFI_SUCCESS;
}
