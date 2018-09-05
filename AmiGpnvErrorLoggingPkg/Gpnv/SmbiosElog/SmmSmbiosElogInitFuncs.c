//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2014, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
//
//
//  Rev. 1.01
//    Bug Fix :fixed Multi event time count does not follow spec.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Dec/21/2016
//   
//  Rev. 1.00
//    Bug Fix : Add ECC/PCI SMBIOS event log function.
//    Reason  : 
//    Auditor : Ivern Yeh
//    Date    : Jun//2016
//  
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SmmSmbiosElogInitFuncs.c
    SmbiosElog Initialization Functions Implementation

**/
//---------------------------------------------------------------------------

#include "SmbiosElog.h"
#include <Protocol/FlashProtocol.h>
#include <Library/SmmServicesTableLib.h>
#include "DxeSmmRedirFuncs.h"
#include <Protocol/SmmReportStatusCodeHandler.h>
#include <Library/DebugLib.h>
#include "token.h"

// SMC R1.00 >>
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMBIOS Type 17 DIMM handles
#ifndef A2_MEMORY_SOCKETS
  #define A2_MEMORY_SOCKETS 0
#endif 
#ifndef A3_MEMORY_SOCKETS
  #define A3_MEMORY_SOCKETS 0
#endif 
#ifndef A4_MEMORY_SOCKETS
  #define A4_MEMORY_SOCKETS 0
#endif 
#define MAX_DIMMS NO_OF_PHYSICAL_MEMORY_ARRAY * \
(A1_MEMORY_SOCKETS + A2_MEMORY_SOCKETS + A3_MEMORY_SOCKETS + A4_MEMORY_SOCKETS)

UINT16  Type17Data[MAX_DIMMS];
UINT16  *Type17 = NULL;

UINT32 Type17Count = MAX_DIMMS;
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMC R1.00 <<


//---------------------------------------------------------------------------

// Global pointer to local information.  For Non-SMM version of component,
// Pointer should be updated when virtual memory mode is entered.

EFI_SMBIOS_ELOG_INSTANCE_DATA     *mRedirSmbiosPrivate;
EFI_GUID gEfiSetupVariableGuid = SETUP_GUID;
UINT64                            *mSmbiosRecordId;

// Prototypes for Functions Used in Init

FLASH_PROTOCOL    *Flash=NULL;
BOOLEAN IsSmbiosElogEnabled(VOID);
EFI_STATUS
EFIAPI
DxeSmmRedirFuncsHandler (
  IN     EFI_HANDLE                                DispatchHandle,
  IN     CONST VOID                                *RegisterContext,
  IN OUT VOID                                      *CommBuffer,
  IN OUT UINTN                                     *CommBufferSize
  );


// ENTRY POINT

/**

    Entry point of Smbios Elog Driver

    @param ImageHandle - The Image handle of this driver.
    @param SystemTable - The pointer of EFI_SYSTEM_TABLE.

    @return EFI_STATUS 
    @retval EFI_SUCCESS - The driver successfully initialized
    @retval EFI_NOT_FOUND - Needed protocols were not found

**/

EFI_STATUS
InitializeSmmSmbiosElog (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{
  EFI_STATUS                           Status = EFI_SUCCESS;
  EFI_HANDLE                           NewHandle;
  AMI_SMBIOS_ELOG_SUPPORT_PROTOCOL    *SmbiosProtocol = NULL;
  UINT8                                *PointerToInstanceData;
  EFI_SMM_RSC_HANDLER_PROTOCOL         *SmmRscHandlerProtocol = NULL;
// SMC R1.00 >>
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
    AMI_SMBIOS_PROTOCOL    *AmiSmbiosProtocol = NULL;
    UINT8               *Structure;      // generic byte pointer
    UINT16              StructureSize;
    UINT32              i;
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMC R1.00 <<

  InitAmiSmmLib (ImageHandle, SystemTable);

  if (IsSmbiosElogEnabled() == FALSE)
   return EFI_SUCCESS;
  
// Locating the SMM flash protocol

  Status = gBS->LocateProtocol (&gFlashSmmProtocolGuid, NULL, &Flash);

  if(EFI_ERROR(Status)) {
      return Status;
  }

// Allocate Memory for SMM Private Data

  Status = gSmst->SmmAllocatePool (
                            EfiRuntimeServicesData,
                            sizeof (EFI_SMBIOS_ELOG_INSTANCE_DATA),
                            (void **)&mRedirSmbiosPrivate 
                            );

  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmAllocatePool (
                            EfiRuntimeServicesData,
                            sizeof(UINT64), 
                            (VOID **)&mSmbiosRecordId
                            );

  ASSERT_EFI_ERROR (Status);

// While in SMM Save Pointer of where Local Memory is so that we are able to
// see and read the local memory copy and all the variables describing the 
// current state. If not available from Regular memory version of component, 
// this SMM version can not run.

  Status = gBS->LocateProtocol (&gAmiSmbiosElogSupportGuid, NULL, (VOID **)&SmbiosProtocol);
  if (Status != EFI_SUCCESS) 
     return EFI_NOT_READY;


  PointerToInstanceData = SmbiosProtocol->Something;

// Copy the all the local data of non SMM into SMM

  MemCpy(mRedirSmbiosPrivate,PointerToInstanceData,sizeof(EFI_SMBIOS_ELOG_INSTANCE_DATA));

// Now install the SMM Redirect Protocol

  mRedirSmbiosPrivate->SmbiosElog.ActivateEventLog  = EfiActivateSmbiosElog;
  mRedirSmbiosPrivate->SmbiosElog.EraseEventLogData = EfiEraseSmbiosElogData;
  mRedirSmbiosPrivate->SmbiosElog.GetEventLogData   = EfiGetSmbiosElogData;
  mRedirSmbiosPrivate->SmbiosElog.SetEventLogData   = EfiSetSmbiosElogData;
  NewHandle = NULL;

  Status = gSmst->SmmInstallProtocolInterface (
                    &NewHandle,
                    &gEfiRedirElogProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mRedirSmbiosPrivate->SmbiosElog 
                    );

  ASSERT_EFI_ERROR (Status);

  if ( !EFI_ERROR(Status) ) {

// Register SMM DxeSmmRedirFuncsHandler SMI handler

      NewHandle = NULL;
      Status = gSmst->SmiHandlerRegister (DxeSmmRedirFuncsHandler, &gAmiSmmDummyProtocolRedirGuid, &NewHandle);
      ASSERT_EFI_ERROR (Status);


// Register the dummy protocol to notify that SMM Redir Protocol is installed

      NewHandle = NULL;
      Status = gBS->InstallProtocolInterface (
                           &NewHandle,
                           &gAmiSmmDummyProtocolRedirGuid,
                           EFI_NATIVE_INTERFACE,
                           NULL
                           );
      ASSERT_EFI_ERROR (Status);
  }
  
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
    Status = gBS->LocateProtocol (&gAmiSmbiosProtocolGuid,
                                  NULL,
                                  &AmiSmbiosProtocol);
    ASSERT_EFI_ERROR (Status);
    
    if (AmiSmbiosProtocol != NULL) {
    
      // update global Type17 ... handles for ECC errors
      Status = EFI_SUCCESS;
	  Type17 = &Type17Data[0];
      for (i = 1; !EFI_ERROR(Status) && (i - 1) < Type17Count; i++) {
    
        DEBUG ((DEBUG_INFO, "Get DIMM data from SMBIOS type 17\n"));  
        Status = AmiSmbiosProtocol->SmbiosReadStrucByType (17 /*Type 17*/,
                                                           i /*Instance is 1-based*/,
                                                           &Structure,
                                                           &StructureSize);
        if (!EFI_ERROR(Status)) {
    
          // only the structure data is copied ...no extended data
		   Type17[i - 1] = ((SMBIOS_STRUCTURE_HEADER*)Structure)->Handle;

          // free the memory
          gBS->FreePool(Structure);
        }
      }
    }
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMC R1.00 <<

      Status = gSmst->SmmLocateProtocol (
                           &gEfiSmmRscHandlerProtocolGuid,
                           NULL,
                           &SmmRscHandlerProtocol
                           );

      if (!EFI_ERROR (Status)) {

// Register the callback function for ReportStatusCode() notification

      Status = SmmRscHandlerProtocol->Register ((EFI_SMM_RSC_HANDLER_CALLBACK)GpnvStatusCodeReportWorker 
                                               );
      DEBUG ((-1, "Callback function registration for ReportStatusCode() notification Status = %r\n", Status));

  }


  return EFI_SUCCESS;
}

/**

    This handler get control when SMM communicate is called.This function 
    calls different API's depending on the header present.

    @param DispatchHandle - The unique handle assigned to this handler by
                            SmiHandlerRegister() 
    @param RegisterContext - Points to an optional handler context which was
                             specified when the handler was registered.
    @param CommBuffer - A pointer to a collection of data in memory
    @param CommBufferSize - The size of CommBuffer

    @return EFI_STATUS 
    @retval EFI_SUCCESS - API is called successfully.

**/
EFI_STATUS
EFIAPI
DxeSmmRedirFuncsHandler (
  IN     EFI_HANDLE                                DispatchHandle,
  IN     CONST VOID                                *RegisterContext,
  IN OUT VOID                                      *CommBuffer,
  IN OUT UINTN                                     *CommBufferSize
  )
{
    EFI_STATUS                                      Status = EFI_SUCCESS;
    SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA            *RedirFuncsSetCommunicateData;
    SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA            *RedirFuncsGetCommunicateData;
    SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA          *RedirFuncsEraseCommunicateData;
    SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA       *RedirFuncsActivateCommunicateData;

    ASSERT (CommBuffer != NULL);



     RedirFuncsSetCommunicateData = (SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA *)CommBuffer;

     switch (RedirFuncsSetCommunicateData->FunctionNumber) {

          case SMM_REDIR_FUNCS_SET_COMMUNICATE:
               Status = mRedirSmbiosPrivate->SmbiosElog.SetEventLogData (
                    &mRedirSmbiosPrivate->SmbiosElog,
                    RedirFuncsSetCommunicateData->ElogData,
                    RedirFuncsSetCommunicateData->DataType,
                    RedirFuncsSetCommunicateData->AlertEvent,
                    RedirFuncsSetCommunicateData->Size,
                    RedirFuncsSetCommunicateData->RecordId
                    );
            RedirFuncsSetCommunicateData->ReturnStatus = Status;
            break;

       case SMM_REDIR_FUNCS_GET_COMMUNICATE:
            RedirFuncsGetCommunicateData = (SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA *)CommBuffer;
            Status = mRedirSmbiosPrivate->SmbiosElog.GetEventLogData (
                    &mRedirSmbiosPrivate->SmbiosElog,
                    RedirFuncsGetCommunicateData->ElogData,
                    RedirFuncsGetCommunicateData->DataType,
                    RedirFuncsGetCommunicateData->Size,
                    RedirFuncsGetCommunicateData->RecordId
                    );
            RedirFuncsGetCommunicateData->ReturnStatus = Status;
            break;
       case SMM_REDIR_FUNCS_ERASE_COMMUNICATE:
            RedirFuncsEraseCommunicateData = (SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA *)CommBuffer;
            Status = mRedirSmbiosPrivate->SmbiosElog.EraseEventLogData (
                    &mRedirSmbiosPrivate->SmbiosElog,
                    RedirFuncsEraseCommunicateData->DataType,
                    RedirFuncsEraseCommunicateData->RecordId
                    );
            RedirFuncsEraseCommunicateData->ReturnStatus = Status;
            break;

        case SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE:
            RedirFuncsActivateCommunicateData = (SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA *)CommBuffer;
            Status = mRedirSmbiosPrivate->SmbiosElog.ActivateEventLog (
                    &mRedirSmbiosPrivate->SmbiosElog,
                    RedirFuncsActivateCommunicateData->DataType,
                    RedirFuncsActivateCommunicateData->EnableElog,
                    RedirFuncsActivateCommunicateData->ElogStatus
                    );
            RedirFuncsActivateCommunicateData->ReturnStatus = Status;
            break;
       default :
           break;


}
    return EFI_SUCCESS;
}

/**

    This will be called when ReportStatusCode() is called and here we log 
    the events.

    @param CodeType - Type of Status Code.
    @param Value - Value to output for Status Code.
    @param Instance - Instance Number of this status code.
    @param CallerId - ID of the caller of this status code.
    @param Data - Optional data associated with this status code.

    @return EFI_STATUS

**/
EFI_STATUS
GpnvStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
    EFI_STATUS                        Status = EFI_SUCCESS;
    EFI_STATUS_CODE_ELOG_INPUT        ToWrite;

// GpnvErrorLogging module reports only SMBIOS Event log types and 
// Error codes that are defined in Platform Initialization(PI) Specification

    if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) { 

    ToWrite.StatusCodeType = CodeType;
    ToWrite.StatusCodeValue = Value;

    ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE;

    *mSmbiosRecordId = 0;
    Status =mRedirSmbiosPrivate->SmbiosElog.SetEventLogData (
                                 &mRedirSmbiosPrivate->SmbiosElog,
                                 (UINT8*)&ToWrite,
                                 EfiElogRedirSmSMBIOS,
                                 FALSE,
                                 sizeof(EFI_STATUS_CODE_ELOG_INPUT),
                                 mSmbiosRecordId );

    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
