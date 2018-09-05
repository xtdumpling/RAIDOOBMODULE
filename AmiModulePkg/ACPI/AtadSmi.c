//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************


/** @file AtadSmi.c
    Interface to a subset of EFI Framework protocols using 
    legacy interfaces that will allow external software to access EFI 
    protocols in a legacy environment.

**/


#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include "AtadSmi.h"
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCommunication.h>
#include <Token.h>


EFI_HANDLE  VarSmiHandle = NULL;
#if ATAD_USE_COMMUNICATE == 1
EFI_SMM_BASE2_PROTOCOL      *InternalSmmBase2 = NULL;
#endif
EFI_GUID    AtadSmiGuid = ATAD_SMI_GUID;

VOID        *gAtadBuffer = NULL;
EFI_EVENT   EvtAtadSmi;
VOID        *RegAtadSmi;
UINT32 i=0;
UINT32 j=0;
/**
    The SMI handler for Nvram services.

    @param VOID


    @retval EFI_SUCCESS all the time.

**/

EFI_STATUS AtadSmiHandler ()
{
    EFI_STATUS              Status = EFI_DEVICE_ERROR;
    ATAD_VARIABLE            *SmmVarBuffer;
    EFI_GUID gEfiTimeVariableGuid  = EFI_TIME_VARIABLE_GUID;
    TIME_VARIABLE TimeVar;
    UINTN TimeVarSize = sizeof(TIME_VARIABLE);
    UINT32 TimeVarFlags = EFI_VARIABLE_NON_VOLATILE | 
                           EFI_VARIABLE_BOOTSERVICE_ACCESS | 
                           EFI_VARIABLE_RUNTIME_ACCESS;

    //checkpoint(0x24);
    if (gAtadBuffer == NULL) return EFI_SUCCESS;
    
    SmmVarBuffer = (ATAD_VARIABLE*)gAtadBuffer;

    	
    //checkpoint(0x25);
    switch (SmmVarBuffer->Signature){

        case ATAD_SW_SMI_UPDATE:

        	TimeVar.TimeZone = SmmVarBuffer->TimeZone;
        	TimeVar.Daylight = SmmVarBuffer->DayLight;
        	Status = pRS->SetVariable( L"EfiTime",
        												&gEfiTimeVariableGuid,
        												TimeVarFlags,
        												TimeVarSize,
        												&TimeVar );
        
        	if (!EFI_ERROR(Status)) SmmVarBuffer->Signature = ATAD_SW_SMI_OK;
        	else SmmVarBuffer->Signature = ATAD_SW_SMI_NOT_GOOD;
            //checkpoint(0x30+i++);
        	break;

        case ATAD_SW_SMI_GET:

		    Status = pRS->GetVariable( L"EfiTime",
		                               &gEfiTimeVariableGuid, 
		                               &TimeVarFlags, 
		                               &TimeVarSize, 
		                               &TimeVar );
		    if (!EFI_ERROR(Status))
		    {
			SmmVarBuffer->TimeZone = TimeVar.TimeZone;
			SmmVarBuffer->DayLight = TimeVar.Daylight;
			SmmVarBuffer->Signature = ATAD_SW_SMI_OK;
		    }
		    else SmmVarBuffer->Signature = ATAD_SW_SMI_NOT_GOOD;
		    //checkpoint(0x40+j++);
        break;
        
        default:
			SmmVarBuffer->Signature = ATAD_SW_SMI_NOT_GOOD;
	}
    //checkpoint(0x26);
    return EFI_SUCCESS;
}

#if ATAD_USE_COMMUNICATE == 1
EFI_STATUS AtadSmiHandler1 (
  IN     EFI_HANDLE                                DispatchHandle,
  IN     CONST VOID                                *Context,
  IN OUT VOID                                      *CommBuffer,
  IN OUT UINTN                                     *CommBufferSize
  )
{
    //TRACE((-1,"AtadSmiHandler1\n"));
    if (gAtadBuffer == NULL)
        return EFI_DEVICE_ERROR;
    //TRACE((-1,"AtadSmiHandler11\n"));
    return AtadSmiHandler();

}
#endif

EFI_STATUS AtadSmiHandler2 (
		IN EFI_HANDLE	DispatchHandle,
		IN CONST VOID	*Context OPTIONAL,
		IN OUT VOID		*CommBuffer OPTIONAL,
		IN OUT UINTN	*CommBufferSize OPTIONAL
)

{

	EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext = Context;
	//checkpoint(0x22);
    if ((gAtadBuffer == NULL) || ((UINT8)DispatchContext->SwSmiInputValue != ACPI_TIME_AND_ALARM_SW_SMI))
        return EFI_DEVICE_ERROR;
    //checkpoint(0x23);
    return AtadSmiHandler();

}

/**
    This function registers SMI Handler for Nvram services

    @param Event signalled event
    @param Context calling context

    @retval EFI_SUCCESS all the time.

**/
EFI_STATUS  AtadSmiSmmSwDispatchCallback(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_SMM_SW_REGISTER_CONTEXT     SwContext;
	EFI_SMM_SW_DISPATCH2_PROTOCOL    *pSwDispatch;
    EFI_HANDLE                      Handle;
    EFI_STATUS                      Status;
    UINTN                           VariableSize = sizeof(gAtadBuffer);


    if(gAtadBuffer) return EFI_SUCCESS; // already came here once...
    Status = pSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch);

    if (EFI_ERROR(Status)) return Status;
    TRACE((-1, "AtadSmiSmmSwDispatchCallback \n"));
    Status = pRS->GetVariable ( L"AtadSmiBuffer",
                                &AtadSmiGuid,
                                NULL,
                                &VariableSize,
                                &gAtadBuffer );
    if ((EFI_ERROR(Status)) || (gAtadBuffer == NULL)) return EFI_NOT_FOUND;
    TRACE((-1, "AtadSmiSmmSwDispatchCallback2 \n"));
    SwContext.SwSmiInputValue = ACPI_TIME_AND_ALARM_SW_SMI;
    Status  = pSwDispatch->Register ( pSwDispatch,
                                      AtadSmiHandler2,
                                      &SwContext,
                                      &Handle );
    if (EFI_ERROR(Status)) return Status;
    TRACE((-1, "AtadSmiSmmSwDispatchCallback3 \n"));
    if (Event != NULL) pBS->CloseEvent (Event);
    return EFI_SUCCESS;
}
/**
    Registration of the SMI function.

    @param EFI_HANDLE          - ImageHandle
        EFI_SYSTEM_TABLE*   - SystemTable

    @retval EFI_STATUS, based on the result.

**/
EFI_STATUS AtadSmiInSmmFunction(
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS                      Status;
#if ATAD_USE_COMMUNICATE == 1
    EFI_SMM_SYSTEM_TABLE2       *pSmst2           = NULL;
#endif

    TRACE((-1, "In AtadSmi InSmmFunction\n"));
    Status = AtadSmiSmmSwDispatchCallback (NULL, NULL);
    if (EFI_ERROR(Status))
    {
    	Status = RegisterProtocolCallback ( &gEfiSmmSwDispatch2ProtocolGuid,
                                        	AtadSmiSmmSwDispatchCallback,
                                        	NULL,
                                        	&EvtAtadSmi,
                                        	&RegAtadSmi );
    	TRACE((-1, "In AtadSmi InSmmFunction RegisterProtocolCallback\n"));
    	ASSERT_EFI_ERROR (Status);
    	
    }
#if ATAD_USE_COMMUNICATE == 1
    Status = SystemTable->BootServices->LocateProtocol (
                                        &gEfiSmmBase2ProtocolGuid,
                                        NULL,
                                        (VOID **)&InternalSmmBase2
                                        );
    ASSERT_EFI_ERROR (Status);

    Status = InternalSmmBase2->GetSmstLocation (InternalSmmBase2, &pSmst2);
    ASSERT_EFI_ERROR (Status);
    ASSERT (pSmst2 != NULL);
    Status = pSmst2->SmiHandlerRegister(AtadSmiHandler1, &AtadSmiGuid, &VarSmiHandle);
    ASSERT_EFI_ERROR (Status);
#endif

    return Status;
}

/**
    This function is a driver entry point

         
    @param ImageHandle Image handle
    @param SystemTable pointer to system table

          
    @retval EFI_STATUS, based on the result.

**/

EFI_STATUS
AtadSmiEntry(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS Status;

    Status = InitAmiSmmLib (ImageHandle, SystemTable);
    TRACE((-1,"AtadSmiEntry\n"));
    AtadSmiInSmmFunction (ImageHandle, SystemTable);
    ASSERT_EFI_ERROR(Status);

    return Status;
}


//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
