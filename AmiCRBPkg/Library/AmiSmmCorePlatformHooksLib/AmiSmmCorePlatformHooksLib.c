//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiSmmCorePlatformHooksLib.c
    This file contains code for 

*/

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Cpu/CpuRegs.h>
#include <RcRegs/IIO_RAS.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SiliconRegAccess.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/CpuCsrAccess.h>

EFI_RAS_SYSTEM_TOPOLOGY               *mRasTopologyLib;
SYSTEM_RAS_SETUP                      *mRasSetupLib;
SYSTEM_RAS_CAPABILITY                 *mRasCapabilityLib;
EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccess;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocolLib = NULL;

/**
  This function checks if Socket is in Viral State.

  @param[in] Ioh            The IOH number.

  @retval    Status         return TRUE if viral error is reported.
**/

BOOLEAN
SocketInViralState (
  UINT8 Iio
)
{
  BOOLEAN                                   ViralState = FALSE;
  VIRAL_IIO_RAS_STRUCT                      ViralIioRas;
  UINT8                                     IioStack;

  for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    if (mRasTopologyLib->SystemInfo.SocketInfo[Iio].UncoreIioInfo.IioStackInfo[IioStack].Valid != 1) continue;
      
    ViralIioRas.Data = mCpuCsrAccess->ReadCpuCsr (Iio, IioStack, VIRAL_IIO_RAS_REG);

    if (ViralIioRas.Bits.iio_viral_state) {
      ViralState = TRUE;
      break;
    }
  }
   
  return ViralState;
}

/**
  Routine that goes through all the Sockets and checks if Socket is in 
  Viral State

  @param[in] VOID   Doesn't need any input parameter.

  @return Status Code
**/

EFI_STATUS
ProcessViralError (
   VOID
   )
{
  UINT8    Socket;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mRasTopologyLib->SystemInfo.SocketBitMap & (1 << Socket)) {
      if (SocketInViralState (Socket)) {
        CpuDeadLoop ();
      }
    }
  }
   return EFI_SUCCESS;
}

/**
  Routine to Initialse the needed Pointers

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @return Status Code
**/

EFI_STATUS 
InitializeAmiSmmCorePlatformHookLib (   
  IN      CONST EFI_GUID   *Protocol,
  IN      VOID             *Interface,
  IN      EFI_HANDLE        Handle
  )
{
  EFI_STATUS Status;  
  
  
  Status = gSmst->SmmLocateProtocol (&gEfiPlatformRasPolicyProtocolGuid, NULL, &mPlatformRasPolicyProtocolLib);
  ASSERT_EFI_ERROR (Status);
  ASSERT (mPlatformRasPolicyProtocolLib != NULL);

  mRasTopologyLib = mPlatformRasPolicyProtocolLib->EfiRasSystemTopology;
  mRasSetupLib = mPlatformRasPolicyProtocolLib->SystemRasSetup;
  mRasCapabilityLib = mPlatformRasPolicyProtocolLib->SystemRasCapability;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiCpuCsrAccessGuid, 
                    NULL, 
                    &mCpuCsrAccess
                    );
  ASSERT_EFI_ERROR (Status);
  
  return EFI_SUCCESS;
}

/**
  Checks if System is in Viral before invoking registered SMI handlers.
  
  This function performs platform specific tasks before invoking registered SMI handlers.
  
  @retval EFI_SUCCESS       The platform hook completes successfully.
  @retval Other values      The paltform hook cannot complete due to some error.

**/

EFI_STATUS
EFIAPI
RASProcessInViralBeforeSMM (
  VOID
  )
{
    EFI_STATUS  Status;
    VOID        *Registration;
    static BOOLEAN	RegisteredOnce = FALSE;
    
    if (mPlatformRasPolicyProtocolLib == NULL) {
	
	    if (RegisteredOnce == FALSE)
	    {
		    Status = gSmst->SmmRegisterProtocolNotify (
	                      &gEfiPlatformRasPolicyProtocolGuid,
	                      InitializeAmiSmmCorePlatformHookLib,
	                      &Registration
	                      );
		    RegisteredOnce = TRUE;
	    }
    }
    else{
      if (mRasTopologyLib->SystemInfo.SystemRasType == ADVANCED_RAS) {
        ProcessViralError();
      }  
  }
 
  return EFI_SUCCESS;
     
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
