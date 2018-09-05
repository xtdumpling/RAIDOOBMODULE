//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgSmm/TCGSmm.c 1     10/08/13 12:05p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:05p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/TcgSmm/TCGSmm.c $
//
// 1     10/08/13 12:05p Fredericko
// Initial Check-In for Tpm-Next module
//
// 2     8/30/13 11:02p Fredericko
//
// 1     7/10/13 5:57p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 17    7/31/12 6:27p Fredericko
// [TAG]        EIP94589
// [Category]   Improvement
// [Description]    Use better variable names in TcgSmm.c
// [Files]          TcgSmm.c
//
// 16    5/20/12 2:12p Fredericko
//
// 15    5/09/12 5:31p Fredericko
// Change Port address to support 16bit port addresses. Some Chipsets
// require this.
//
// 14    3/19/12 6:37p Fredericko
// [TAG]        EIP82866
// [Description]    AMIUEFI: Implement the NoPPIClear flag and provide
// operations to set/clear the value or a BIOS config option -  Windows
// Partner Bug Management Bug #679996
// [Files]          AmiTcgNvFlagSample.c, AmiTcgNvFlagSample.sdl,
// AmiTcgPlatformDxe.c
//
// 13    12/12/11 1:08p Fredericko
// [TAG]        EIP59683
// [Category]   Improvement
// [Description]    Allow selection between writing to SMI port as a word
// or as a Byte.
// Some platforms might require word writes to the SMI Status port.
// [Files]          Tcg.cif, Tcg.sdl, Tcg_ppi1_2_Ex.asl, TcgSmm.mak, TcgSmm.c
//
// 12    12/07/11 4:27p Fredericko
//
// 11    12/07/11 4:26p Fredericko
// [TAG]        EIP59683
// [Category]   Improvement
// [Description]    Allow selection between writing to SMI port as a word
// or as a Byte.
// Some platforms might require word writes to the SMI Status port.
// [Files]          Tcg.cif, Tcg.sdl, Tcg_ppi1_2_Ex.asl, TcgSmm.mak, TcgSmm.c
//
// 10    8/10/11 4:30p Fredericko
// [TAG]        EIPEIP66468
// [Category]   Spec Update
// [Severity]   Minor
// [Description]    1. Added some more boundary checking for unsupported
// functions and for handling of Ppi 0
// [Files]          1. TcgSmm.c
//
// 9     8/09/11 6:29p Fredericko
// [TAG]        EIP66468
// [Category]   Spec Update
// [Severity]   Minor
// [Description]    1. Changes for Tcg Ppi 1.2 support.
// [Files]          1 TcgSmm.h
// 2.TcgSmm.c
// 3.Tcg_ppi1_2.asl
// 4. AmiTcgNvflagsSample.c
// 5. AmiTcgPlatformPeiLib.c
// 6. AmiTcgPlatformDxe.sdl
// 7. AmiTcgPlatformDxe.c
//
// 8     7/25/11 3:20a Fredericko
// [TAG]        EIP65177
// [Category]   Spec Update
// [Severity]   Minor
// [Description]    TCG Ppi Sec ver 1.2 update
//
// 7     2/16/11 10:37a Fredericko
//  [TAG]        EIP54014
//  [Category] BUG FIX
//  [Severity]   HIGH
//  [Symptom] TPM initialize failed using win7 tool tpm.msc after drive
// bitlocker test.
//  [RootCause]  wrong size used for getvariable
//  [Solution]   Use correct size
//  [Files] TCGSmm.c
//
// 6     8/23/10 4:21p Fredericko
// Code Clean up. Removed port 80 checkpoint writes from code.
//
// 5     8/09/10 2:34p Fredericko
// Added NVRAM writes functions for TCG PPI support. Moved from TcgBoard
// component
//
// 4     8/04/10 5:07p Fredericko
// Changed AMI interface to use only one SMI value instead of 3
//
// 3     5/20/10 8:54a Fredericko
// Included File Header
// Included File Revision History
// Code Beautification
// EIP 37653
//
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TCGSmm.c
//
// Description:
// Function definition file for TCGSMM subcomponent
//
//<AMI_FHDR_END>
//*************************************************************************

#include "TCGSmm.h"
#include <Setup.h>
#include <Library/IoLib.h>
#include <Protocol/SmmVariable.h>
#include<Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Token.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/MemoryOverwriteControl.h>
#include <AmiDxeLib.h>
#include <AmiTcg/tcg.h>
#include <Protocol/AmiTcgProtocols.h>

#pragma optimize("",off)

#if WORD_ACCESS_SMI_PORT == 0x01
void DisablePlatformSMI();
#endif

//---------------------------------------------------------------
//---------------------------------------------------------------
//                  Porting BEGINS
//---------------------------------------------------------------
//---------------------------------------------------------------
VOID DisablePlatformSMI() {} // Porting routine. modify if required for platform
VOID EnablePlatformSMI() {} // Porting routine. modify if required for platform
//---------------------------------------------------------------
//---------------------------------------------------------------
//                  Porting ENDS
//---------------------------------------------------------------
//---------------------------------------------------------------


#if NVRAM_VERSION < 7
typedef EFI_STATUS (*SHOW_BOOT_TIME_VARIABLES)(BOOLEAN Show);

typedef struct
{
    SHOW_BOOT_TIME_VARIABLES ShowBootTimeVariables;
} AMI_NVRAM_CONTROL_PROTOCOL;

AMI_NVRAM_CONTROL_PROTOCOL *NvramControl = NULL;

#endif

EFI_RUNTIME_SERVICES         *ptrRuntimeServices = NULL;
EFI_PHYSICAL_ADDRESS         NvsMemoryAddress;
EFI_SMM_VARIABLE_PROTOCOL    *SmmVarProtocol;
AMI_ASL_PPI_NV_VAR           *TpmAcpiNvsFlags;

VOID NVOSWrite_PPI_request ( );

VOID NVOSWrite_MOR_request ();

VOID NVOSRead_PPI_request ( );


EFI_SMM_BASE2_PROTOCOL              *pSmmBase2;

static UINT8         Tpm20Device = 0;
EFI_STATUS TcgGetNextGuidHob(
    IN OUT VOID          **HobStart,
    IN EFI_GUID          * Guid,
    OUT VOID             **Buffer,
    OUT UINTN*BufferSize OPTIONAL )
{
    return EFI_SUCCESS;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  TcgGetSmstConfigurationTablePi
//
// Description: Gets a system configuration table based on tableguid
//
//
// Input:      IN EFI_GUID *TableGuid
//
// Output:      VOID*
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
VOID* TcgGetSmstConfigurationTablePi(IN EFI_GUID *TableGuid)
{
    EFI_CONFIGURATION_TABLE *Table;
    UINTN i;

    ASSERT(gSmst!=NULL); //pSmstFramework must be initialized with the call to InitSmiHandler
    if (gSmst==NULL) return NULL;
    Table = gSmst->SmmConfigurationTable;
    i = gSmst->NumberOfTableEntries;

    for (; i; --i,++Table)
    {
        if (CompareMem(&Table->VendorGuid,TableGuid, sizeof(EFI_GUID))==0)
            return Table->VendorTable;
    }
    return NULL;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  NVOSread_PPI_request
//
// Description: Returns TCG PPI variable values to the Operating system
//
//
// Input:       IN    EFI_HANDLE        DispatchHandle,
//              IN    EFI_SMM_SW_DISPATCH_CONTEXT    *DispatchContext
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
VOID NVOSRead_PPI_request()
{
    UINTN          Size = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR Temp;
    EFI_STATUS     Status;


#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
    Status = SmmVarProtocol->SmmGetVariable(L"AMITCGPPIVAR",
                                            &AmitcgefiOsVariableGuid,
                                            NULL,
                                            &Size,
                                            &Temp);
#else
    if (NvramControl == NULL)
        NvramControl = TcgGetSmstConfigurationTablePi(&gAmiNvramControlProtocolGuid);

    if (NvramControl) NvramControl->ShowBootTimeVariables(TRUE);
    Status = ptrRuntimeServices->GetVariable( L"AMITCGPPIVAR", \
             &AmitcgefiOsVariableGuid, \
             NULL, \
             &Size, \
             &Temp );

    if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
#endif

    if(EFI_ERROR(Status))
    {
    	TpmAcpiNvsFlags->RequestFuncResponse = PP_RETURN_TPM_OPERATION_RESPONSE_FAILURE;
        return;
    }

    switch (TpmAcpiNvsFlags->Flag )
    {
        case 3:
            TpmAcpiNvsFlags->RQST = Temp.RQST;
            break;
        case 5:
            TpmAcpiNvsFlags->RCNT = Temp.RCNT;
            TpmAcpiNvsFlags->ERROR = Temp.ERROR;
            break;
        default:
            TpmAcpiNvsFlags->ERROR = 0x0;
            break;
    }

    TpmAcpiNvsFlags->RequestFuncResponse = PP_RETURN_TPM_OPERATION_RESPONSE_SUCCESS;
}

//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure:  NVWrite_PPI_request
//
// Description: Writes TCG PPI variable values to NVRAM on SMI request the Operating system
//
//
// Input:       IN    EFI_HANDLE        DispatchHandle,
//              IN    EFI_SMM_SW_DISPATCH_CONTEXT    *DispatchContext
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
VOID NVOSWrite_PPI_request( )
{
    UINTN          Size = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR Temp;
    EFI_STATUS     Status;
    UINT32         Read_value = 0;
    UINT32          attrib =     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    AMI_ASL_PPI_NV_VAR   *TpmAcpiNvsFlags = ((AMI_ASL_PPI_NV_VAR *)(UINTN)NvsMemoryAddress);

    Read_value = TpmAcpiNvsFlags->RQST;

    if( (Read_value == TCPA_PPIOP_UNOWNEDFIELDUPGRADE && Tpm20Device == 1)
            || Read_value == TCPA_PPIOP_SETOPAUTH
            || (Read_value == TCPA_PPIOP_SETNOPPIMAINTENANCE_FALSE && Tpm20Device == 1)
            || (Read_value == TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE && Tpm20Device == 1)
            || Read_value == TCPA_PPIOP_CHANGE_EPS
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_ON_FALSE
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_ON_TRUE
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_OFF_FALSE
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_OFF_TRUE
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_EPS_FALSE
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_EPS_TRUE
            || Read_value == TCPA_PPIOP_LOG_ALL_DIGESTS
            || Read_value == TCPA_PPIOP_DISABLE_ENDORSEMENT_ENABLE_STORAGE_HIERARCHY
            || Read_value == TCPA_PPIOP_ENABLE_BLOCK_SID_FUNC
            || Read_value == TCPA_PPIOP_DISABLE_BLOCK_SID_FUNC
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_ENABLE_BLOCK_SID_FUNC_TRUE
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_ENABLE_BLOCK_SID_FUNC_FALSE
            || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_DISABLE_BLOCK_SID_FUNC_TRUE
            || Read_value >= TCPA_PPIOP_SET_PPREQUIRED_FOR_DISABLE_BLOCK_SID_FUNC_FALSE)
    {
        TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_SUBMIT_REQUEST_TO_PREOS_NOT_IMPLEMENTED;
        return;
    }

    if(Read_value >= 0 && Read_value < TCPA_PPIOP_VENDOR)
    {
        Temp.RQST  = Read_value;
        Temp.RCNT  = Read_value;
        Temp.ERROR = 0;
        Temp.Flag  = 0;
        Temp.AmiMisc = 0;
        Temp.OptionalData = TpmAcpiNvsFlags->OptionalRqstData;

#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
        attrib = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
        Status = SmmVarProtocol->SmmSetVariable(L"AMITCGPPIVAR",
                                                &AmitcgefiOsVariableGuid,
                                                attrib,
                                                Size,
                                                &Temp);
        if(Status == EFI_INVALID_PARAMETER)
        {
            Status = SmmVarProtocol->SmmSetVariable(L"AMITCGPPIVAR", \
                                                    &AmitcgefiOsVariableGuid, \
                                                    0, \
                                                    0, \
                                                    NULL);

            if(EFI_ERROR(Status))
            {
            	TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_SUBMIT_REQUEST_TO_PREOS_GENERAL_FAILURE;
                return;
            }

            Status = SmmVarProtocol->SmmSetVariable( L"AMITCGPPIVAR", \
                     &AmitcgefiOsVariableGuid, \
                     EFI_VARIABLE_NON_VOLATILE   \
                     | EFI_VARIABLE_BOOTSERVICE_ACCESS \
                     | EFI_VARIABLE_RUNTIME_ACCESS,\
                     Size, \
                     &Temp );
        }
#else
        if (NvramControl == NULL)
            NvramControl = TcgGetSmstConfigurationTablePi(&gAmiNvramControlProtocolGuid);

        if (NvramControl) NvramControl->ShowBootTimeVariables(TRUE);

        Status = ptrRuntimeServices->SetVariable ( L"AMITCGPPIVAR", \
                 &AmitcgefiOsVariableGuid, \
                 attrib, \
                 Size, \
                 &Temp );
        if(Status == EFI_INVALID_PARAMETER)
        {
            Status = ptrRuntimeServices->SetVariable(L"AMITCGPPIVAR", \
                     &AmitcgefiOsVariableGuid, \
                     0, \
                     0, \
                     NULL);

            if(EFI_ERROR(Status))
            {
                if(EFI_ERROR(Status))
                {
                	TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_SUBMIT_REQUEST_TO_PREOS_GENERAL_FAILURE;
                    return;
                }
                if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
                return;
            }

            Status = ptrRuntimeServices->SetVariable( L"AMITCGPPIVAR", \
                     &AmitcgefiOsVariableGuid, \
                     attrib, \
                     Size, \
                     &Temp );
        }

        if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
#endif

        if(EFI_ERROR(Status))
        {
        	TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_SUBMIT_REQUEST_TO_PREOS_GENERAL_FAILURE;
            return;
        }
    }
    else
    {
        TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_SUBMIT_REQUEST_TO_PREOS_NOT_IMPLEMENTED;
        return;
    }
}



//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: Read_User_Confirmation_Status
//
// Description: Reads the user confirmation satus for PPI requests
//
//
// Input:       IN    EFI_HANDLE        DispatchHandle,
//              IN    EFI_SMM_SW_DISPATCH_CONTEXT    *DispatchContext
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
VOID Read_User_Confirmation_Status( )
{
    UINTN                       Size = sizeof(PERSISTENT_BIOS_TPM_FLAGS);
    UINTN                       BiosSize = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR              Temp;
    PERSISTENT_BIOS_TPM_FLAGS   TpmNvFlags;
    UINT32                       Read_value = 0;
    EFI_STATUS                  Status;
    UINT32                      attrib =     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    AMI_ASL_PPI_NV_VAR              *TpmAcpiNvsFlags = ((AMI_ASL_PPI_NV_VAR *)(UINTN)NvsMemoryAddress);


    Read_value = TpmAcpiNvsFlags->RQST;

#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
    attrib = 0;
    Status = SmmVarProtocol->SmmGetVariable(
                 L"AMITCGPPIVAR",
                 &AmitcgefiOsVariableGuid,
                 &attrib,
                 &BiosSize,
                 &Temp);


#else
    if (NvramControl == NULL)
        NvramControl = TcgGetSmstConfigurationTablePi(&gAmiNvramControlProtocolGuid);

    if (NvramControl) NvramControl->ShowBootTimeVariables(TRUE);

    Status = ptrRuntimeServices->GetVariable( L"AMITCGPPIVAR", \
             &AmitcgefiOsVariableGuid, \
             NULL, \
             &BiosSize, \
             &Temp );
#endif

#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
    Status = SmmVarProtocol->SmmSetVariable(
                 L"AMITCGPPIVAR",
                 &AmitcgefiOsVariableGuid,
                 attrib,
                 BiosSize,
                 &Temp);

    if(Status == EFI_INVALID_PARAMETER)
    {
        Status = SmmVarProtocol->SmmSetVariable(L"AMITCGPPIVAR", \
                                                &AmitcgefiOsVariableGuid, \
                                                attrib, \
                                                0, \
                                                NULL);

        if(EFI_ERROR(Status))
        {
            TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_BLOCKED_BY_BIOS_CONFIGURATION;
            return;
        }

        Status = Status = SmmVarProtocol->SmmSetVariable( L"AMITCGPPIVAR", \
                          &AmitcgefiOsVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE   \
                          | EFI_VARIABLE_BOOTSERVICE_ACCESS \
                          | EFI_VARIABLE_RUNTIME_ACCESS, \
                          BiosSize, \
                          &Temp );
    }

#else
    Status = ptrRuntimeServices->SetVariable ( L"AMITCGPPIVAR", \
             &AmitcgefiOsVariableGuid, \
             EFI_VARIABLE_NON_VOLATILE   \
             | EFI_VARIABLE_BOOTSERVICE_ACCESS, \
             BiosSize, \
             &Temp );

    if(Status == EFI_INVALID_PARAMETER)
    {
        Status = ptrRuntimeServices->SetVariable(L"AMITCGPPIVAR", \
                 &AmitcgefiOsVariableGuid, \
                 0, \
                 0, \
                 NULL);

        if(EFI_ERROR(Status))
        {
            TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_BLOCKED_BY_BIOS_CONFIGURATION;
            if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
            return;

        }

        Status = ptrRuntimeServices->SetVariable( L"AMITCGPPIVAR", \
                 &AmitcgefiOsVariableGuid, \
                 attrib, \
                 BiosSize, \
                 &Temp );
    }
#endif



#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
    attrib = 0;
    Status = SmmVarProtocol->SmmGetVariable(
                 L"TPMPERBIOSFLAGS",
                 &FlagsStatusguid,
                 &attrib,
                 &Size,
                 &TpmNvFlags);

#else
    Status = ptrRuntimeServices->GetVariable( L"TPMPERBIOSFLAGS", \
             &FlagsStatusguid, \
             NULL, \
             &Size, \
             &TpmNvFlags );

    if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
#endif

    if(EFI_ERROR(Status))
    {
    	TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_BLOCKED_BY_BIOS_CONFIGURATION;
        return;
    }

    if(Read_value >= 0  && Read_value < TCPA_PPIOP_VENDOR)
    {
        if(Read_value == 0 )
        {

            TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
            return;
        }

        if( (Read_value == TCPA_PPIOP_UNOWNEDFIELDUPGRADE && Tpm20Device == 1)
                || Read_value == TCPA_PPIOP_SETOPAUTH
                || (Read_value == TCPA_PPIOP_SETNOPPIMAINTENANCE_FALSE && Tpm20Device == 1)
                || (Read_value == TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE && Tpm20Device == 1)
                || Read_value == TCPA_PPIOP_CHANGE_EPS
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_ON_FALSE
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_ON_TRUE
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_OFF_FALSE
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_OFF_TRUE
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_EPS_FALSE
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_EPS_TRUE
                || Read_value == TCPA_PPIOP_LOG_ALL_DIGESTS
                || Read_value == TCPA_PPIOP_DISABLE_ENDORSEMENT_ENABLE_STORAGE_HIERARCHY
                || Read_value == TCPA_PPIOP_ENABLE_BLOCK_SID_FUNC
                || Read_value == TCPA_PPIOP_DISABLE_BLOCK_SID_FUNC
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_ENABLE_BLOCK_SID_FUNC_TRUE
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_ENABLE_BLOCK_SID_FUNC_FALSE
                || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_DISABLE_BLOCK_SID_FUNC_TRUE
                || Read_value >= TCPA_PPIOP_SET_PPREQUIRED_FOR_DISABLE_BLOCK_SID_FUNC_FALSE)
        {
            TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_NOT_IMPLEMENTED;
            return;
        }
        else if(Read_value == TCPA_PPIOP_CLEAR || Read_value == TCPA_PPIOP_ENABLE_ACTV_CLEAR )
        {
            if(TpmNvFlags.NoPpiClear  == TRUE)
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
            }
            else
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED;
            }
            return;
        }
        else if(Read_value == TCPA_PPIOP_CLEAR_ENACT || Read_value == TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV)
        {
            if(TpmNvFlags.NoPpiClear  == TRUE  && TpmNvFlags.NoPpiProvision == TRUE )
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
            }
            else
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED;
            }
            return;
        }
        else if(Read_value == TCPA_PPIOP_SETNOPPIPROVISION_FALSE || Read_value == TCPA_PPIOP_SETNOPPIPROVISION_TRUE)
        {
            if(Read_value == TCPA_PPIOP_SETNOPPIPROVISION_TRUE )
            {
                if(Tpm20Device == 1)
                {
                    TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
                }
                else
                {
                    TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED;
                }
            }
            else
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
            }
            return;
        }
        else if(Read_value == TCPA_PPIOP_SETNOPPICLEAR_FALSE || Read_value == TCPA_PPIOP_SETNOPPICLEAR_TRUE)
        {
            if(Read_value == TCPA_PPIOP_SETNOPPICLEAR_TRUE )
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED;
            }
            else
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
            }
            return;
        }
        else if(Read_value == TCPA_PPIOP_SETNOPPIMAINTENANCE_FALSE || Read_value == TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE)
        {
            if(Read_value == TCPA_PPIOP_SETNOPPIMAINTENANCE_FALSE )
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
            }
            else
            {
                TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED;
            }
            return;
        }
        else if(Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_PCR_FALSE || Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_PCR_TRUE)
		{
			if(Read_value == TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_PCR_FALSE )
			{
				TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED;
			}
			else
			{
				TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
			}
			return;
		}
        else if(Read_value == TCPA_PPIOP_SET_PCR_BANKS)
		{
			if(TpmNvFlags.Ppi1_3_Flags.PpRequiredForChangePCRS)
			{
				TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED;
			}
			else
			{
				TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
			}
			return;
		}
        else if(TpmNvFlags.NoPpiProvision == TRUE)
        {
            TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED;
        }
        else
        {
            TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED;
        }
    }
    else
    {
        TpmAcpiNvsFlags->RequestFuncResponse = TREE_PP_GET_USER_CONFIRMATION_NOT_IMPLEMENTED;
    }
}



//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: NVOSWrite_MOR_request
//
// Description: Writes TCG PPI MOR variable to NVRAM on SMI request the Operating system
//
//
// Input:       IN    EFI_HANDLE        DispatchHandle,
//              IN    EFI_SMM_SW_DISPATCH_CONTEXT    *DispatchContext
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
VOID NVOSWrite_MOR_request( )
{
    UINT8           mor  = 0;
    UINTN           MorSize = sizeof(mor);
    EFI_STATUS      Status;
    CHAR16          UefiMor[]   = L"MemoryOverwriteRequestControl";
    UINT32          Read_value  = 0;
    UINTN           PpiVarSize = sizeof(AMI_PPI_NV_VAR);
    UINT32          attrib =     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    AMI_ASL_PPI_NV_VAR   *TpmAcpiNvsFlags = ((AMI_ASL_PPI_NV_VAR *)(UINTN)NvsMemoryAddress);

    Read_value = TpmAcpiNvsFlags->RQST;

#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
    attrib = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    Status = SmmVarProtocol->SmmSetVariable(   UefiMor,
             &gEfiMemoryOverwriteControlDataGuid,
             attrib,
             MorSize,
             &Read_value);

#else
    Status = ptrRuntimeServices->SetVariable ( UefiMor, \
             &gEfiMemoryOverwriteControlDataGuid, \
             EFI_VARIABLE_NON_VOLATILE   \
             | EFI_VARIABLE_RUNTIME_ACCESS   \
             | EFI_VARIABLE_BOOTSERVICE_ACCESS, \
             MorSize, \
             &Read_value );
#endif

    if(Status)
    {
        TpmAcpiNvsFlags->RequestFuncResponse = MOR_REQUEST_GENERAL_FAILURE;
        return;
    }

    TpmAcpiNvsFlags->RequestFuncResponse = MOR_REQUEST_SUCCESS;
}



VOID GetTpmNVSFlags()
{
    UINTN       Size = sizeof (EFI_PHYSICAL_ADDRESS);
    static      BOOLEAN Init=0;
    EFI_STATUS  Status;


    if(Init == 1) return;
    Init = 1;
#if NVRAM_VERSION < 7
    if (NvramControl == NULL)
        NvramControl = TcgGetSmstConfigurationTablePi(&gAmiNvramControlProtocolGuid);

    if (NvramControl) NvramControl->ShowBootTimeVariables(TRUE);

    Status = ptrRuntimeServices->GetVariable( L"TpmServFlags", \
                                   &FlagsStatusguid, \
                                   NULL, \
                                   &Size, \
                                   &NvsMemoryAddress );

    if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
#else
    Status = SmmVarProtocol->SmmGetVariable(
                 L"TpmServFlags",
                 &FlagsStatusguid,
                 NULL,
                 &Size,
                 &NvsMemoryAddress);
#endif
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgSmiCommonHandler
//
// Description: Common function to handle TCG SMI's
//
//
// Input:       IN    EFI_HANDLE        DispatchHandle,
//              IN    EFI_SMM_SW_DISPATCH_CONTEXT    *DispatchContext
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS TcgSmiCommonHandler(
    IN EFI_HANDLE  DispatchHandle,
    IN CONST VOID  *Context         OPTIONAL,
    IN OUT VOID    *CommBuffer      OPTIONAL,
    IN OUT UINTN   *CommBufferSize  OPTIONAL)
{
    EFI_STATUS          Status;
    UINTN               Size = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR      Temp;
    UINT32              Attributes=0;

#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
    Status = SmmVarProtocol->SmmGetVariable(
                 L"AMITCGPPIVAR",
                 &AmitcgefiOsVariableGuid,
                 NULL,
                 &Size,
                 &Temp);
#else
    if (NvramControl == NULL)
        NvramControl = TcgGetSmstConfigurationTablePi(&gAmiNvramControlProtocolGuid);

    if (NvramControl) NvramControl->ShowBootTimeVariables(TRUE);

    Status = ptrRuntimeServices->GetVariable( L"AMITCGPPIVAR", \
             &AmitcgefiOsVariableGuid, \
             NULL, \
             &Size, \
             &Temp );

    if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
#endif

    GetTpmNVSFlags();
    if(NvsMemoryAddress == 0){
        TpmAcpiNvsFlags->RequestFuncResponse = (TCPA_PPI_RESPONSE_MASK | TCPA_PPI_BIOSFAIL);
        return EFI_NOT_FOUND;
    }

    TpmAcpiNvsFlags =  ((AMI_ASL_PPI_NV_VAR *)(UINTN)NvsMemoryAddress);

    if(Status)
    {
        TpmAcpiNvsFlags->RequestFuncResponse = (TCPA_PPI_RESPONSE_MASK | TCPA_PPI_BIOSFAIL);
        return Status;
    }

    if(TpmAcpiNvsFlags->Flag == ACPI_FUNCTION_GET_USER_CONFIRMATION_STATUS_FOR_REQUEST)
    {
        Read_User_Confirmation_Status();
    }
    else if(TpmAcpiNvsFlags->Flag == ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS || TpmAcpiNvsFlags->Flag == ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS_2)
    {
        NVOSWrite_PPI_request();
    }
    else if(TpmAcpiNvsFlags->Flag == ACPI_FUNCTION_GET_PENDING_REQUEST_BY_OS || TpmAcpiNvsFlags->Flag == ACPI_FUNCTION_RETURN_REQUEST_RESPONSE_TO_OS)
    {
        NVOSRead_PPI_request();
    }
    else if(TpmAcpiNvsFlags->Flag == ACPI_FUNCTION_SET_MOR_VALUE_FLAG)
    {
        NVOSWrite_MOR_request(  );
    }

    return EFI_SUCCESS;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   HelpRegisterPPISMI
//
// Description: Common function to handle TCG SMI's
//
//
// Input:       IN    EFI_HANDLE ImageHandle,
//              IN    EFI_SYSTEM_TABLE *SystemTable
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI
TCGSmmInit(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_SMM_SW_DISPATCH2_PROTOCOL *pSwDispatch2;
    EFI_SMM_SW_REGISTER_CONTEXT  SwContext;
    EFI_HANDLE                   Handle;
    EFI_STATUS                   Status;
    SETUP_DATA                   SetupDataBuffer;
    UINTN                        SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                       SetupVariableAttributes=0;
    UINT32                       attrib =     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;

    Status =  gSmst->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch2);
    if (EFI_ERROR(Status)) return Status;


#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
    Status =  gSmst->SmmLocateProtocol(&gEfiSmmVariableProtocolGuid, NULL, &SmmVarProtocol);
    if (EFI_ERROR(Status)) return Status;
#endif

    ptrRuntimeServices = (EFI_RUNTIME_SERVICES *)TcgGetSmstConfigurationTablePi(&SmmRsTableGuid);
    if(ptrRuntimeServices == NULL) return EFI_NOT_FOUND;

    Status = ptrRuntimeServices->GetVariable (L"Setup",
             &gSetupVariableGuid,
             &SetupVariableAttributes,
             &SetupVariableSize,
             &SetupDataBuffer);
    if ( EFI_ERROR( Status ))
    {
        return EFI_SUCCESS;
    }

//prevent _PTS (Method;\_SB.TPM.TPTS(Arg0)) call SMI as TpmSupport disabled.
//As TpmSupport disabled, TpmServFlags still keeps NvsMemoryAddress last boot but now this address is not used/allocated by TCG module.
    if (SetupDataBuffer.TpmSupport == 0)
    {
        return EFI_SUCCESS;
    }

    Tpm20Device = SetupDataBuffer.Tpm20Device;

    SwContext.SwSmiInputValue = PPI_OFFSET;
    Status                    = pSwDispatch2->Register( pSwDispatch2,
                                TcgSmiCommonHandler,
                                &SwContext,
                                &Handle );

    ASSERT_EFI_ERROR( Status );

    if ( EFI_ERROR( Status ))
    {
        return EFI_SUCCESS;
    }

    return Status;
}

#pragma optimize("",on)

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
