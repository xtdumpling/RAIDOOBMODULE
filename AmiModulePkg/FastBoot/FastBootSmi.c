//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file FastBootSmi.c
 *  Implementation of fast boot smi functionality.
 */

#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <Token.h>
#include <Setup.h>

#if PI_SPECIFICATION_VERSION >= 0x1000A
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#define RETURN(status) {return status;}
#else
#include <Protocol/SmmBase.h>
#include <Protocol/SmmSwDispatch.h>
#define RETURN(status) {return ;}
#endif

extern EFI_GUID gEfiSmmSwDispatch2ProtocolGuid;

#if PI_SPECIFICATION_VERSION >= 0x1000A

/**
 * SwSmiHandler.
 *    
 * @param[in]   DispatchHandle        Handle of this dispatch function. 
 * @param[in]   Context                    Points to an optional handler context which was specified when the handler was registered.
 * @param[in]   CommBuffer            A pointer to a collection of data in memory that 
 *                                                     will be conveyed from a non-SMM environment into an SMM environment.
 * @param[in]   CommBufferSize    The size of the CommBuffer.
 *
 * @retval EFI_SUCCESS                Set variable success.
 */

EFI_STATUS 
EFIAPI 
SwSmiHandler (
  IN EFI_HANDLE DispatchHandle,
  IN CONST VOID  *Context OPTIONAL,
  IN OUT VOID    *CommBuffer OPTIONAL,
  IN OUT UINTN   *CommBufferSize OPTIONAL )
#else

/**
 * SwSmiHandler.
 *    
 * @param[in]   DispatchHandle            Handle of this dispatch function.
 * @param[in]   DispatchContext            Pointer to the dispatch function's context.
 *
 * @retval EFI_SUCCESS                    Set variable success.
 */

VOID 
EFIAPI 
SwSmiHandler (
  IN EFI_HANDLE                  DispatchHandle,
  IN EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext )
#endif
{

    EFI_STATUS  Status;
    EFI_GUID    SetupGuid = SETUP_GUID;
    UINTN       SetupDataSize = sizeof(SETUP_DATA);    
    SETUP_DATA  SetupData;
    UINT32      Attributes;
    
    Status = gRT->GetVariable(
                    L"Setup", 
                    &SetupGuid, 
                    &Attributes, 
                    &SetupDataSize, 
                    &SetupData);
    if (EFI_ERROR(Status)) return Status;
    
    SetupData.FastBoot = 0;      

    Status = gRT->SetVariable(
                    L"Setup", 
                    &SetupGuid,
                    Attributes,
                    SetupDataSize, 
                    &SetupData);
    return Status;
}

/**
 * InSmmFunction.
 *    
 * @param[in]   ImageHandle                The firmware allocated handle for the EFI image. 
 * @param[in]   SystemTable                A pointer to the EFI System Table.
 *
 * @retval EFI_SUCCESS                    Register SWSMI success.
 */

EFI_STATUS 
EFIAPI 
InSmmFunction( 
    IN EFI_HANDLE       ImageHandle, 
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    EFI_SMM_SW_REGISTER_CONTEXT     SwSmiContext;
    EFI_SMM_SW_DISPATCH2_PROTOCOL    *SwSmiDispatch;
#else
    EFI_SMM_SW_DISPATCH_CONTEXT     SwSmiContext;
    EFI_SMM_SW_DISPATCH_PROTOCOL    *SwSmiDispatch;
#endif

    EFI_HANDLE    SwSmiHandle;
    EFI_STATUS    Status;

    SwSmiContext.SwSmiInputValue = FAST_BOOT_DISABLE_SWSMI;

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;

    Status = pSmstPi->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwSmiDispatch);
#else
    Status = gBS->LocateProtocol (&gEfiSmmSwDispatchProtocolGuid, NULL, (VOID**)&SwSmiDispatch);
#endif

    if (EFI_ERROR(Status)) return Status;

    Status = SwSmiDispatch->Register(
        SwSmiDispatch,
        SwSmiHandler,
        &SwSmiContext,
        &SwSmiHandle
    );
   
    return Status;
    
}

/**
 * Set PM under AMITSE_EVENT_BEFORE_BOOT_GUID.
 *    
 * @param[in]   ImageHandle                The firmware allocated handle for the EFI image. 
 * @param[in]   SystemTable                A pointer to the EFI System Table.
 *
 * @retval EFI_SUCCESS                    The entry point is executed successfully.
 */

EFI_STATUS 
EFIAPI 
FastBootSmiEntryPoint(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    InitAmiLib(ImageHandle,SystemTable);

    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************


