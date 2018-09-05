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


/** @file CmosManagerDxe.c
    Contains the routines that constitute the CMOS manager's
    DXE phase entry.

**/

#include <Protocol\Reset.h>
#include <Setup.h>
#include <CmosManagerHooks.h>
#include "CmosManagerDxe.h"
#include "CmosManagerHob.h"

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

static EFI_EVENT  gSetupEnterEvent;
static EFI_EVENT  gBootEvent;
static EFI_GUID   gCmosAccessGuid = EFI_DXE_CMOS_ACCESS_GUID;

/**
    Display a message string on the status code device.

    @param Manager private interface
    @param Format  pointer to a null-terminated format ASCII string
    @param ...
        -- variable parameter list of data/variables used within
        the format string

    @retval VOID

**/

VOID CmosTraceCallout(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    CHAR8                           *Format,
    ... )
{
#ifdef EFI_DEBUG
	va_list	ArgList;
//    ArgList = va_start(ArgList,Format);
    va_start(ArgList,Format);
	PrintDebugMessageVaList(-1, Format, ArgList);
	va_end(ArgList);
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
    return pBS->LocateProtocol( &gCmosAccessGuid, NULL, AccessInterface);
}


/**
    Locate the EFI_CMOS_ACCESS_INTERFACE for the current manager
    interface.

    @param PeiServices This is a pointer to the PEI Services structure.
                       (In the DXE phase it will be NULL.)
    @param CmosManager This is the private interface.

    @retval EFI_STATUS (Return Value)
        -- If successful, EFI_SUCCESS is returned.
        -- Otherwise, a valid EFI error code is returned.

**/

EFI_STATUS InstallAccessInterfaceCallout(
    IN   EFI_PEI_SERVICES             **PeiServices,
    OUT  EFI_CMOS_MANAGER_INTERFACE   *CmosManager )
{
	EFI_STATUS      Status;
    EFI_HANDLE      Handle = 0;

    Status = pBS->InstallMultipleProtocolInterfaces(
            &Handle, &CmosManager->AccessGuid, CmosManager, NULL);

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
    return pBS->AllocatePool( EfiBootServicesData, Size, Buffer);
}


/**
    This function locates the HOB containing the table of CMOS tokens
    and initializes token table information in the CMOS manager's
    interface.

    @param Manager The CMOS manager interface to initialize.

    @retval EFI_STATUS (Return value)
        = EFI_SUCCESS or valid EFI error code

    @note
      This function is used only in DXE phase, included by the build
      process via the absence of the macro PEI_COMPILE.

**/

EFI_STATUS InitializeCmosTablesFromHob(
    OUT EFI_CMOS_MANAGER_INTERFACE  *Manager )
{
    EFI_STATUS          Status;
    EFI_GUID            HobListGuid = HOB_LIST_GUID;
    EFI_GUID            CmosManagerHobGuid = CMOS_MANAGER_HOB_GUID;
    CMOS_MANAGER_HOB    *CmosManagerHob;

    CmosManagerHob =
        (CMOS_MANAGER_HOB*)GetEfiConfigurationTable(pST,&HobListGuid);
    if ( CmosManagerHob == NULL )
        return EFI_UNSUPPORTED;

    Status = FindNextHobByGuid( &CmosManagerHobGuid, (VOID**)&CmosManagerHob);
    if (EFI_ERROR( Status ))
        return Status;

    Manager->ManagerHob = NULL;      // NULL in DXE to prevent write attempts
    Manager->FirstManagedRegister = CmosManagerHob->FirstManagedRegister;
    Manager->LastManagedRegister = CmosManagerHob->LastManagedRegister;
    Manager->TokenTable = CmosManagerHob->TokenTable;
    Manager->TokenCount = CmosManagerHob->TokenCount;
    Manager->OptimalDefaultTable = CmosManagerHob->OptimalDefaultTable;
    Manager->OptimalDefaultCount = CmosManagerHob->OptimalDefaultCount;
    Manager->NoChecksumTable = CmosManagerHob->NoChecksumTable;
    Manager->NoChecksumCount = CmosManagerHob->NoChecksumCount;
    Manager->UnmanagedTable = CmosManagerHob->UnmanagedTable;
    Manager->UnmanagedTableCount = CmosManagerHob->UnmanagedTableCount;
    Manager->Checksum = CmosManagerHob->Checksum;
    Manager->ManagerStatus = CmosManagerHob->ManagerStatus;

    return EFI_SUCCESS;
}


/**
    This function performs phase-specific initialization of the
    private EFI_CMOS_MANAGER_INTERFACE structure.

    @param Manager Pointer to the EFI_CMOS_MANAGER_INTERFACE structure.

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS, if successful
        = valid EFI error code, otherwise

**/

EFI_STATUS ManagerInterfaceInitializeCallout(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager)
{
    Manager->AccessGuid = gCmosAccessGuid;
    Manager->IsFirstBoot = &gIsFirstBoot[0];
    Manager->IsColdBoot = &gIsColdBoot[0];
    Manager->IsBsp = &gIsBsp[0];
    Manager->CmosIsUsable = &gCmosIsUsable[0];
    Manager->CmosBatteryIsGood = &gCmosBatteryIsGood[0];
    Manager->CmosBank = &gCmosBank[0];
    Manager->CmosBankCount = gCmosBankCount;

    InitializeCmosTablesFromHob(Manager);

    return EFI_SUCCESS;
}


/**
    This function performs additional tasks when the internal status
    value is updated. For the PEI phase, this task involves synchronizing
    the status value in the HOB. For the DXE phase, no tasks are required.

    @param Manager Pointer to the EFI_CMOS_MANAGER_INTERFACE structure.

    @retval VOID

**/

VOID StatusUpdateCallout(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager)
{
    return;
}


/**
    This function uses the CMOS Manager interface to clear the un-managed
    private status bits upon boot or setup entry.

    @param Event   Event handle
    @param Context Not used

    @retval VOID

**/

VOID ClearCmosBasedStatus(
    IN EFI_EVENT    Event,
    IN VOID         *Context )
{
    EFI_CMOS_ACCESS_INTERFACE   *Cmos;
    EFI_STATUS                  Status;
    CMOS_STATUS_BYTES           CmosInfo;
    BOOLEAN                     Usable;

    CMOS_TRACE_FULL((NULL, "ClearCmosBasedStatus: Enter\n" ));
    LOCATE_CMOS_ACCESS_PROTOCOL(Status, Cmos);

    if ( !EFI_ERROR(Status) ){

        // get CMOS-based info
        CMOS_TRACE_FULL((NULL, "Clear CMOS-based status values.\n" ));

        Cmos->ReadCmosStatusBytes(Cmos, &CmosInfo);
        Usable = CmosInfo.ConfigurationStatus.NotUsable == 1 ? FALSE : TRUE;

        // clear status values for next boot
        if ( Usable ){
            Cmos->Write(Cmos, CMOS_MGR_BATTERY_BAD,         0);
            Cmos->Write(Cmos, CMOS_MGR_CHECKSUM_BAD,        0);
            Cmos->Write(Cmos, CMOS_MGR_DEFAULTS_LOADED,     0);
            Cmos->Write(Cmos, CMOS_MGR_FIRST_BOOT_DETECTED, 0);
            CMOS_TRACE_FULL((NULL, "...Done\n" ));
        }
    }
    else
        TRACE((TRACE_ALWAYS, "ClearCmosBasedStatus...Failed\n" ));

    // This function only needs to be called one time to
    // get proper updates in setup and to clear the CMOS values.

#if CMOS_SETUP_SUPPORT
    pBS->CloseEvent(gSetupEnterEvent);
#endif
    pBS->CloseEvent(gBootEvent);
}


/**
    This function updates the CMOS setup variable using information
    from the CMOS-based status bits.

    @param Event   Event handle
    @param Context Not used

    @retval VOID

**/

#if CMOS_SETUP_SUPPORT
VOID UpdateCmosSetupVariable(
    IN EFI_EVENT    Event,
    IN VOID         *Context )
{
    EFI_CMOS_ACCESS_INTERFACE   *Cmos;
    EFI_STATUS                  Status;
    CMOS_STATUS_BYTES           CmosInfo;
    EFI_GUID                    SetupGuid = SETUP_GUID;
    UINTN                       VariableSize = sizeof(SETUP_DATA);
    UINT32                      Attributes;
    SETUP_DATA                  Sd;
    EFI_GUID                    CmosGuid = EFI_DXE_CMOS_ACCESS_GUID;
    BOOLEAN                     NotUsable;
    BOOLEAN                     BadBattery;
    BOOLEAN                     DefaultsLoaded;
    BOOLEAN                     FirstBoot;
    BOOLEAN                     BadChecksum;

    CMOS_TRACE_FULL(( NULL, "UpdateCmosSetupVariable: Enter\n" ));

    Status = pBS->LocateProtocol( &CmosGuid, NULL, &Cmos);
    if (!EFI_ERROR (Status)) {
        Status = Cmos->ReadCmosStatusBytes(Cmos, &CmosInfo);
        if (EFI_ERROR(Status)){
            CMOS_TRACE(( NULL,
                "UpdateCmosSetupVariable: ReadCmosStatusBytes failed\n" ));
            ASSERT_EFI_ERROR(EFI_NOT_FOUND);
            return;
        }
        NotUsable = (BOOLEAN)CmosInfo.ConfigurationStatus.NotUsable;
    }
    else {
        CMOS_TRACE(( NULL,
            "UpdateCmosSetupVariable: LocateProtocol failed\n" ));
        ASSERT_EFI_ERROR(Status);
        return;
    }

    // update setup data
    CMOS_TRACE_FULL((NULL,
                        "Updating setup data.\n" ));
    Status = pRS->GetVariable( L"Setup", &SetupGuid, &Attributes,
                               &VariableSize, &Sd );
    if (EFI_ERROR(Status)){
        CMOS_TRACE_FULL((NULL,
                        "Could not locate Setup variable.\n" ));
    }
    else {
        if ( NotUsable ) {
            CMOS_TRACE_FULL(( NULL,
                        "  Status: Cannot use CMOS-based status\n"));
            Sd.CmosBatteryIsBad = CmosInfo.Battery.Field.IsGood == 0 ?
                                      TRUE : FALSE;
            Sd.CmosNotUsable = TRUE;
        }
        else {
            Cmos->Read(Cmos, CMOS_MGR_BATTERY_BAD,         &BadBattery);
            Cmos->Read(Cmos, CMOS_MGR_CHECKSUM_BAD,        &BadChecksum);
            Cmos->Read(Cmos, CMOS_MGR_DEFAULTS_LOADED,     &DefaultsLoaded);
            Cmos->Read(Cmos, CMOS_MGR_FIRST_BOOT_DETECTED, &FirstBoot);

            Sd.CmosDefaultsLoaded = DefaultsLoaded;
            Sd.CmosFirstBootDetected = FirstBoot;
            Sd.CmosBatteryIsBad = BadBattery;
            Sd.CmosCheckSumIsBad = BadChecksum;
            Sd.CmosNotUsable = FALSE;
        }
        CMOS_TRACE_FULL(( NULL,
                "  Status: DefaultsLoaded=%X, FirstBoot=%X, BatteryBad=%X\n",
                Sd.CmosDefaultsLoaded, Sd.CmosFirstBootDetected,
                Sd.CmosBatteryIsBad ));
        CMOS_TRACE_FULL(( NULL,
                "          BadChecksum=%X, CmosNotUsable=%X\n",
                Sd.CmosCheckSumIsBad, Sd.CmosNotUsable ));
        pRS->SetVariable( L"Setup", &SetupGuid, Attributes,
                          VariableSize, &Sd );
    }

    // Clear the CMOS-based status bits after updating the setup variable
    ClearCmosBasedStatus(Event, Context);

    return;
}
#endif


/**
    This function creates a callback on the setup entry event and boot
    event to (respectively) update the CMOS setup variable and clear
    the CMOS-based status bits.


    @param VOID

    @retval VOID

**/

VOID CreateEventHandlers( VOID )
{
    EFI_STATUS  Status;

#if CMOS_SETUP_SUPPORT
    EFI_GUID    SetupEnterProtocolGuid = AMITSE_SETUP_ENTER_GUID;
    VOID        *Registration;

    CMOS_TRACE_FULL((NULL, "CreateEventHandlers: UpdateCmosSetupVariable\n" ));
    Status = RegisterProtocolCallback( &SetupEnterProtocolGuid,
        UpdateCmosSetupVariable, NULL, &gSetupEnterEvent, &Registration );
    ASSERT_EFI_ERROR(Status);
#endif

    CMOS_TRACE_FULL((NULL, "CreateEventHandlers: ClearCmosBasedStatus\n" ));
    Status = CreateReadyToBootEvent( TPL_CALLBACK, ClearCmosBasedStatus, NULL,
        &gBootEvent );
    ASSERT_EFI_ERROR(Status);
}


//---------------------------------------------------------------------------
//  DXE entry point function
//
/**
    This function is the main DXE phase entry point for the CMOS
    manager module.

    @param ImageHandle Image handle
    @param SystemTable System table pointer

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

    @note
       *  Initializes/Installs the EFI_CMOS_ACCESS_INTERFACE
          Protocol.

**/

EFI_STATUS CmosManagerDxeEntry (
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable )
{
    EFI_CMOS_MANAGER_INTERFACE  *Mgr;

    InitAmiLib(ImageHandle, SystemTable);

    CMOS_TRACE_FULL((NULL, "CmosManagerDXE Entry\n" ));

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
    // If there is a bad battery, continue using the Optimal Defaults Table.
    //
    // Otherwise, if there is a bad checksum or the Optimal Defaults table
    // is in use from PEI, then flush the table to physical CMOS and
    // discontinue its use.
    //
    // Note: the Optimal Defaults Table is enabled only in PEI
    //-----------------------------------------------------------------------
    if (  !Mgr->CheckStatus(Mgr, CMOS_IS_USABLE) ) {
        CMOS_TRACE_FULL((NULL,
            "CmosManagerDXE: CMOS_IS_USABLE = FALSE\n" ));

        // Default handler is to continue using the Optimal Defaluts Table
        // if the CMOS is unusable.
    }
    else if ( Mgr->CheckStatus(Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED) )
    {
#if (FULL_CMOS_MANAGER_DEBUG)
        CMOS_TRACE_FULL((NULL,
                "CmosManagerDXE: CMOS_OPTIMAL_DEFAULTS_ENABLED = TRUE\n" ));
        if (  Mgr->CheckStatus(Mgr, CMOS_BAD_CHECKSUM) ) {
            CMOS_TRACE_FULL((NULL,
                "CmosManagerDXE: CMOS_BAD_CHECKSUM = TRUE\n" ));
        }
#endif
        CMOS_TRACE_FULL((NULL,
            "CmosManagerDXE: Calling LoadOptimalDefaults()\n" ));
        Mgr->LoadOptimalDefaults(Mgr);
        if ( Mgr->CheckStatus(Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED) ) {
            CMOS_TRACE_FULL((NULL,
                "CmosManagerDXE: CMOS_OPTIMAL_DEFAULTS_OFF\n" ));
            Mgr->ConfigureManager( Mgr, CMOS_OPTIMAL_DEFAULTS_OFF );
        }
    }

    //-----------------------------------------------------------------------
    // Create handlers for CMOS Manager
    //-----------------------------------------------------------------------
    CreateEventHandlers();

    //-----------------------------------------------------------------------
    // Install the CMOS Access interface
    //-----------------------------------------------------------------------
    CMOS_TRACE_FULL((NULL,
        "CmosManagerDXE: Installing CMOS Access Protocol\n" ));
    Mgr->InstallAccessInterface(Mgr);
    if ( !Mgr->CheckStatus(Mgr, CMOS_VALID_MANAGER) ){
    	TRACE((TRACE_ALWAYS,
            "CmosManagerDXE: Invalid interface\n" ));
        return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
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
