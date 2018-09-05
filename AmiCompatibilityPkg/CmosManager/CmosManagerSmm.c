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


/** @file CmosManagerSmm.c
    Contains the routines that initialize the CMOS manager's
    SMM services.

**/

#include <Efi.h>
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLibInc.h>
#include <Protocol\Reset.h>
#include <Protocol\SmmBase.h>
#include <Protocol\SmmSwDispatch2.h>
#include <Protocol\AcpiSupport.h>
#include <CmosAccess.h>
#include <Acpi.h>
#include <AcpiRes.h>
#include <SspData.h>
#include <SspTokens.h>
#include <CmosManagerHooks.h>
#include "CmosManager.h"
#include "CmosManagerSmm.h"

extern CONST UINT8    gFirstManagedRegister;
extern CONST UINT8    gLastManagedRegister;
extern CMOS_TOKEN     gCmosTokenTable[];
extern UINT16         gCmosTokenTableSize;
extern CMOS_REGISTER  gCmosOptimalDefaultTable[];
extern UINT16         gCmosOptimalDefaultTableSize;
extern CMOS_REGISTER  gCmosNoCheckSumTable[];
extern UINT16         gCmosNoCheckSumTableSize;
extern CMOS_REGISTER  gUnmanagedTable[];
extern UINT16         gUnmanagedTableSize;

static EFI_CMOS_IS_FIRST_BOOT  gIsFirstBoot[] =
    {CMOS_IS_FIRST_BOOT_MAPPING NULL};
static EFI_CMOS_IS_BSP         gIsBsp[] =
    {CMOS_IS_BSP_MAPPING NULL};
static EFI_CMOS_IS_COLD_BOOT   gIsColdBoot[] =
    {CMOS_IS_COLD_BOOT_MAPPING NULL};
static EFI_CMOS_IS_CMOS_USABLE gCmosIsUsable[] =
    {CMOS_IS_USABLE_MAPPING NULL};
static EFI_CMOS_BATTERY_TEST   gCmosBatteryIsGood[] =
    {CMOS_BATTERY_TEST_MAPPING NULL};
static CMOS_PORT_MAP           gCmosBank[] =
    { {0,0,0,0,0}, CMOS_PORT_MAPPING  };
static UINT16 gCmosBankCount = sizeof(gCmosBank) / sizeof(CMOS_PORT_MAP);

static EFI_GUID   gCmosAccessGuid = EFI_SMM_CMOS_ACCESS_GUID;

EFI_CMOS_ACCESS_INTERFACE   *gCmos = NULL;
CMOS_SMM_CONTEXT            *gAslContext = NULL;  // ASL communication buffer

/**
    Display a message string on the status code device.

    @param Mgr    private interface
    @param Format pointer to a null-terminated format ASCII string
    @param ...    variable parameter list of data/variables used within
                  the format string

    @retval VOID

**/

VOID CmosTraceCallout(
    IN EFI_CMOS_MANAGER_INTERFACE   *Mgr,
    CHAR8                           *Format,
    ... )
{
#ifdef EFI_DEBUG
    CHAR8   Buffer[1024];
//    va_list ArgList = va_start( ArgList, Format );
    va_list ArgList;

    va_start( ArgList, Format );
    Sprintf_va_list( Buffer, Format, ArgList );
    DEBUG((EFI_D_ERROR, Buffer));
    va_end( ArgList );
#endif
}


/**
    Locate the EFI_CMOS_ACCESS_INTERFACE for the current manager
    interface.

    @param PeiServices     This is a pointer to the PEI Services structure.
                           (In the DXE phase it will be NULL.)
    @param AccessInterface This is the returned access interface.

    @retval EFI_STATUS (Return Value)
        -- If successful, EFI_SUCCESS is returned.
        -- Otherwise, a valid EFI error code is returned.

**/

EFI_STATUS LocateAccessInterfaceCallout(
    IN   EFI_PEI_SERVICES             **PeiServices,
    OUT  EFI_CMOS_ACCESS_INTERFACE    **AccessInterface )
{
    return pSmst->SmmLocateProtocol( &gCmosAccessGuid, NULL, AccessInterface);
}


/**
    Install the EFI_CMOS_ACCESS_INTERFACE for SMRAM CMOS access.

    @param PeiServices This is a pointer to the PEI Services structure.
                       (In the DXE phase it will be NULL.)
    @param Mgr         This is the private interface.

    @retval EFI_STATUS (Return Value)
        -- If successful, EFI_SUCCESS is returned.
        -- Otherwise, a valid EFI error code is returned.

    @note
      The Protocol is installed in both the Boot Services and System
      Management Services tables.

**/

EFI_STATUS InstallAccessInterfaceCallout(
    IN   EFI_PEI_SERVICES             **PeiServices,
    OUT  EFI_CMOS_MANAGER_INTERFACE   *Mgr )
{

    EFI_STATUS   Status = EFI_SUCCESS;
    EFI_STATUS   Status2;
    EFI_HANDLE   Handle = NULL;

    CMOS_TRACE_FULL((NULL, "--------- InstallAccessInterfaceCallout: Entry\n" ));
    CMOS_TRACE_FULL((NULL, "               Mgr = 0x%x\n", Mgr ));
    CMOS_TRACE_FULL((NULL, "  &Mgr->AccessGuid = 0x%x\n", &Mgr->AccessGuid ));
    CMOS_TRACE_FULL((NULL, "           &Handle = 0x%x\n", &Handle ));

    Status2 = pSmst->SmmInstallProtocolInterface(
            &Handle, &Mgr->AccessGuid, EFI_NATIVE_INTERFACE, Mgr);

    if (EFI_ERROR(Status2)){
        CMOS_TRACE((NULL, "  pSmst->InstallMultipleProtocolInterfaces failed (%r)\n", Status2 ));
        Status = EFI_UNSUPPORTED;
    }

    CMOS_TRACE_FULL((NULL, "--------- InstallAccessInterfaceCallout: Exit\n" ));

    return Status;
}


/**
    This function allocates memory.

    @param PeiServices pointer to the PEI Services data structure
                       (This parameter is NULL in the DXE phase.)
    @param Size        Number of bytes to allocate
    @param Buffer      Pointer to buffer for which memory is allocated

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

**/

EFI_STATUS AllocatePoolCallout(
    IN  EFI_PEI_SERVICES    **PeiServices,
    IN  UINTN               Size,
    OUT VOID                **Buffer )
{
    return pSmst->SmmAllocatePool(EfiRuntimeServicesData, Size, Buffer);
}


/**
    This function performs phase-specific initialization of the
    private EFI_CMOS_MANAGER_INTERFACE structure.

    @param Mgr Pointer to the EFI_CMOS_MANAGER_INTERFACE structure.

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS, if successful
        = valid EFI error code, otherwise

**/

EFI_STATUS ManagerInterfaceInitializeCallout(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr)
{
    Mgr->AccessGuid = gCmosAccessGuid;

    Mgr->FirstManagedRegister = gFirstManagedRegister;
    Mgr->LastManagedRegister = gLastManagedRegister;
    Mgr->TokenTable = (CMOS_TOKEN*) gCmosTokenTable;
    Mgr->TokenCount = gCmosTokenTableSize;
    Mgr->NoChecksumTable = (CMOS_REGISTER*) gCmosNoCheckSumTable;
    Mgr->NoChecksumCount = gCmosNoCheckSumTableSize;
    Mgr->UnmanagedTable = (CMOS_REGISTER*) gUnmanagedTable;
    Mgr->UnmanagedTableCount = gUnmanagedTableSize;
    Mgr->OptimalDefaultTable =
        (CMOS_REGISTER*) gCmosOptimalDefaultTable;
    Mgr->OptimalDefaultCount = gCmosOptimalDefaultTableSize;

    Mgr->IsFirstBoot = &gIsFirstBoot[0];
    Mgr->IsColdBoot = &gIsColdBoot[0];
    Mgr->IsBsp = &gIsBsp[0];
    Mgr->CmosIsUsable = &gCmosIsUsable[0];
    Mgr->CmosBatteryIsGood = &gCmosBatteryIsGood[0];
    Mgr->CmosBank = &gCmosBank[0];
    Mgr->CmosBankCount = gCmosBankCount;

    return EFI_SUCCESS;
}


/**
    This function performs additional tasks when the internal status
    value is updated. For the PEI phase, this task involves synchronizing
    the status value in the HOB. For the DXE phase, no tasks are required.

    @param Mgr Pointer to the EFI_CMOS_MANAGER_INTERFACE structure.

    @retval VOID

**/

VOID StatusUpdateCallout(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr)
{
    return;
}


/**
    This is called from SMM or ASL code to access CMOS during
    runtime.

    @param DispatchHandle
    @param DispatchContext

    @retval VOID

**/

EFI_STATUS SmiHandler (
    EFI_HANDLE    DispatchHandle,
    CONST VOID    *Context,
    VOID        *CommBuffer,
    UINTN        *CommBufferSize)
{
    UINT8               Function = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->CommandPort;
    EFI_STATUS          Status = EFI_NOT_FOUND;

    if (Function == CSMI){
        if (gCmos == NULL){
            gAslContext->Status = 0; // FALSE => operation failed
        }
        else {
            switch (gAslContext->Opcode) {
            case 0:  // Write from the communication buffer
                Status = gCmos->Write(gCmos, gAslContext->CmosToken,
                    gAslContext->CmosByte);
                break;
            case 1:  // Read into the communication buffer
                Status = gCmos->Read(gCmos, gAslContext->CmosToken,
                    &gAslContext->CmosByte);
                break;
            }
            if (EFI_ERROR(Status)){
               gAslContext->Status = 0; // FALSE => operation failed
            }
            else {
               gAslContext->Status = 1; // FALSE => operation success
            }
        }
    }

    return Status;
}


/**
    This function is executed from SMM in order to install the SMRAM-based
    CMOS Access Interface pointer.

    @param ImageHandle Image handle
    @param SystemTable System table pointer

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

    @note
       *  Initializes/Installs the EFI_CMOS_ACCESS_INTERFACE
          Protocol for SMM.

**/

EFI_STATUS RegisterCmosSmiHandler (
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable )
{
    EFI_STATUS                      Status;
    EFI_CMOS_MANAGER_INTERFACE      *Mgr;
    EFI_SMM_SW_REGISTER_CONTEXT     SwContext;
    EFI_SMM_SW_DISPATCH2_PROTOCOL    *SwDispatch;
    EFI_HANDLE                      SwHandle  = NULL;

    CMOS_TRACE_FULL((NULL, "CmosManagerSMM Entry\n" ));

    //-----------------------------------------------------------------------
    // Initialize the manager interface and, at this point, only check for
    // interface initialization errors.
    //-----------------------------------------------------------------------
    Mgr = NewCmosManagerInterface(NULL);
    if ( Mgr == NULL || !Mgr->CheckStatus(Mgr, CMOS_VALID_INTERFACE) ) {
        return EFI_UNSUPPORTED;
    }
    if ( Mgr->CheckStatus( Mgr, CMOS_INTERFACE_ALREADY_INSTALLED) )
        return EFI_SUCCESS;

    //-----------------------------------------------------------------------
    // Handle specific reported errors.
    //
    // Note:    All boot time error conditions should have been handled
    //          before execution gets here.
    //
    //          All that is needed is to report the status for debugging.
    //-----------------------------------------------------------------------
    if (  Mgr->CheckStatus(Mgr, CMOS_BAD_BATTERY) ) {
        CMOS_TRACE_FULL((NULL,
            "CmosManagerSMM: CMOS_BAD_BATTERY = TRUE\n" ));

        // Default handler is to continue from DXE phase
    }
    else if ( Mgr->CheckAnyStatus(Mgr,   CMOS_BAD_CHECKSUM
                                       | CMOS_OPTIMAL_DEFAULTS_ENABLED) )
    {
#if (FULL_CMOS_MANAGER_DEBUG)
        if (  Mgr->CheckStatus(Mgr, CMOS_BAD_CHECKSUM) ) {
            CMOS_TRACE_FULL((NULL,
                "CmosManagerSMM: CMOS_BAD_CHECKSUM = TRUE\n" ));
        }

        if (  Mgr->CheckStatus(Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED) ) {
            CMOS_TRACE_FULL((NULL,
                "CmosManagerSMM: CMOS_OPTIMAL_DEFAULTS_ENABLED = TRUE\n" ));
        }
#endif
    }

    //-----------------------------------------------------------------------
    // Install the CMOS Access interface
    //-----------------------------------------------------------------------
    CMOS_TRACE_FULL((NULL,
        "CmosManagerSMM: Installing CMOS Access SMM Protocol @ 0x%x\n",
        Mgr ));
    Mgr->InstallAccessInterface(Mgr);
    if ( !Mgr->CheckStatus(Mgr, CMOS_VALID_MANAGER) ){
        CMOS_TRACE((NULL,
            "CmosManagerSMM: Invalid interface\n" ));
        return EFI_UNSUPPORTED;
    }

    // Update the module-scope interface pointer.
    gCmos = (EFI_CMOS_ACCESS_INTERFACE*) &Mgr->Access;

    //Register the SW SMI handler
    Status = pSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid,
          NULL, &SwDispatch);
    ASSERT_EFI_ERROR(Status);

    SwContext.SwSmiInputValue = CSMI;
    Status = SwDispatch->Register (SwDispatch, SmiHandler, &SwContext,
        &SwHandle);

    if (EFI_ERROR(Status)){
        CMOS_TRACE((NULL,
            "RegisterCmosSmiHandler: could not register...\n"));
    }
    ASSERT_EFI_ERROR(Status);

    CMOS_TRACE((NULL, "RegisterCmosSmiHandler Registered.\n"));

    return EFI_SUCCESS;
}


/**
    This function returns the 64-bit value assigned to the ASL named
    object.

    @param PDsdt   pointer to the DSDT table
    @param ObjName pointer to the name string
    @param Value   pointer to the value to be updated

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

**/

EFI_STATUS  GetAslNameObject(
    IN  ACPI_HDR  *PDsdt,
    IN  UINT8     *ObjName,
    OUT UINT64    *Value)
{
    ASL_OBJ_INFO    Obj;
    EFI_STATUS      Status;

    if (PDsdt->Signature != DSDT_SIG){
        return EFI_INVALID_PARAMETER;
    }

    Status = GetAslObj((UINT8*)(PDsdt+1), PDsdt->Length-sizeof(ACPI_HDR)-1,
        ObjName, otName, &Obj);

    if (!EFI_ERROR(Status)){
        *Value = *((UINT64*)((UINT8*)Obj.DataStart + 1));
    }

    return Status;
}


/**
    This function is executed in non-SMM in order to allocate the
    runtime communication buffer used by the \CMOS.ACCE asl method
    to use CMOS Manager services at runtime.

    @param N/A

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

**/

EFI_STATUS UpdateAslCommunicationBuffer ( VOID )
{
    EFI_PHYSICAL_ADDRESS  DsdtAddress = 0;
    ACPI_HDR              *Dsdt = NULL;
    UINT64                CurrentValue;
    EFI_STATUS            Status;

    // Get a pointer to the DSDT.
    Status = LibGetDsdt(&DsdtAddress, EFI_ACPI_TABLE_VERSION_ALL);
    if (EFI_ERROR(Status)){
        CMOS_TRACE((NULL, "CMOS Manager: failed to get DSDT\n"));
        goto EXIT;
    }
    Dsdt = (ACPI_HDR*)DsdtAddress;

    // Get the current value of \CCOM.
    Status = GetAslNameObject(Dsdt, "CCOM", &CurrentValue);
    if (EFI_ERROR(Status)){
        CMOS_TRACE((NULL, "Could not get \\CCOM ASL object.\n"));
        goto EXIT;
    }
    CMOS_TRACE_FULL((NULL, "CurrentValue of \\CCOM is 0x%lX\n",
        CurrentValue));

    // If \CCOM has not been updated yet, allocate the runtime communication
    // buffer and update the ASL object.  Otherwise, save the value of
    // \CCOM to the global variable in SMRAM scope.
    if (CurrentValue == 0x0123456789ABCDEF){
        Status = pBS->AllocatePool(EfiRuntimeServicesData,
            sizeof(CMOS_SMM_CONTEXT), &gAslContext);
        if (EFI_ERROR(Status)){
            CMOS_TRACE((NULL,
                "failed to allocate CMOS_SMM_CONTEXT\n"));
            goto EXIT;
        }
        CMOS_TRACE_FULL((NULL,
            "CMOS_SMM_CONTEXT allocated to 0x%lX\n", gAslContext ));

        Status = UpdateAslNameObject(Dsdt, "CCOM", (UINT64)gAslContext);
        if (EFI_ERROR(Status)){
            CMOS_TRACE((NULL, "failed to update \\CCOM\n"));
            goto EXIT;
        }
        Dsdt->Checksum = 0;
        Dsdt->Checksum = ChsumTbl((UINT8*)Dsdt, Dsdt->Length);

        CMOS_TRACE_FULL((NULL, "Updated \\CCOM ASL object.\n"));

        Status = GetAslNameObject(Dsdt, "CCOM", &CurrentValue);
        if (EFI_ERROR(Status)){
            CMOS_TRACE((NULL, "Could not get \\CCOM ASL object.\n"));
            goto EXIT;
        }
        CMOS_TRACE_FULL((NULL, "CurrentValue of \\CCOM is 0x%lX\n",
            CurrentValue));
    }
    else {
        gAslContext = (CMOS_SMM_CONTEXT*)CurrentValue;
        CMOS_TRACE_FULL((NULL, "gAslContext was already updated\n"));
    }

EXIT:
    return Status;
}


/**
    This function is the entry point for installing the SMM driver for
    CMOS Manager.

    @param ImageHandle Image handle
    @param SystemTable System table pointer

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

    @note
       *  Initializes/Installs the EFI_CMOS_ACCESS_INTERFACE
          Protocol for SMM.

**/

EFI_STATUS CmosManagerSmmEntry (
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable )
{
#if AMI_COMPATIBILITY_PKG_VERSION < 21
    InitAmiSmmLibPi(ImageHandle, SystemTable);
#else
    InitAmiSmmLib(ImageHandle, SystemTable);
#endif

#if CMOS_MANAGER_ASL_SUPPORT
{
    EFI_STATUS      Status;

    Status = UpdateAslCommunicationBuffer();
    if (EFI_ERROR(Status)){
        CMOS_TRACE((NULL,
            "CMOS Manager failed to update ASL communication buffer\n"));
        ASSERT_EFI_ERROR(Status);
    }
}
#endif

    return RegisterCmosSmiHandler(ImageHandle, SystemTable);
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
