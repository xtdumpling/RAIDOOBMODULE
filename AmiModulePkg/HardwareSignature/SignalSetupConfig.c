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
/**@file SignalSetupConfig.c

 This file will handle Setup Config changed. And it will report to HardwareSignature's final value..
 
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php.
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/
//---------------------------------------------------------------------------
#include <Efi.h>
#include <Dxe.h>
#include <AmiDxeLib.h>
#include <Token.h>
#include <HardwareChangeProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
EFI_GUID  gSetupUpdateCountGuid = SETUP_UPDATE_COUNT_GUID;

/**
  Set setup  count to NVRAM.
  
  This function saves the setup count into NVRAM.

  @param[in]  ChangeTime                        setup times.                 
  
  @retval         EFI_SUCCESS                     ChangeTime is written successfully
  @retval         EFI_INVALID_PARAMETER   ChangeTime is invalid
  
**/
EFI_STATUS 
SetSetupCountToNvram (
    IN UINT32 *ChangeTime )
{
    EFI_STATUS          Status;
    UINTN               VarSize = sizeof(UINT32);

    if(ChangeTime == NULL)
        return EFI_INVALID_PARAMETER;
    
    Status = gRT->SetVariable(
            SETUP_UPDATE_COUNT_VARIABLE, 
            &gSetupUpdateCountGuid,
            EFI_VARIABLE_NON_VOLATILE |
            EFI_VARIABLE_BOOTSERVICE_ACCESS |
            EFI_VARIABLE_RUNTIME_ACCESS,
            VarSize, 
            ChangeTime
            ); 
    
    return Status;    
}

/**
  Get the setup update count data.

  This function gets the setup update times from NVRAM.
  
  @param[in,out]  ChangeTime                        The setup update times.

  @retval              EFI_SUCCESS                      Get the setup update change times successfully.
  @retval              EFI_INVALID_PARAMETER    ChangeTime is NULL.

**/
EFI_STATUS 
GetSetupCountFromNvram(
    IN OUT UINT32 *ChangeTime )
{
    EFI_STATUS          Status;
    UINTN               VarSize = sizeof(UINT32);

    if(ChangeTime == NULL)
        return EFI_INVALID_PARAMETER;
    
    Status = gRT->GetVariable(
            SETUP_UPDATE_COUNT_VARIABLE, 
            &gSetupUpdateCountGuid,
            NULL, 
            &VarSize, 
            ChangeTime ); 

    if( EFI_ERROR(Status) )
    {
        MemSet(ChangeTime,sizeof(UINT32),0);
    }
    
    return Status;    
}

/**
  Count the setup change times.
  
  This function process the setup change count times.
  It  links function SavedConfigChanges in AMITSE module.

  @retval  None.
  
**/
VOID SetupChangeCount()
{
    UINT32 SetupCount;

    SetupCount=0;
    GetSetupCountFromNvram(&SetupCount);
    SetupCount++;
    SetSetupCountToNvram(&SetupCount);
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

