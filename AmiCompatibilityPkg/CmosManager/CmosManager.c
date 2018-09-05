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


/** @file CmosManager.c
    Contains the routines that constitute the CMOS manager
    implementation.  This files is used to create object code
    for both PEI and DXE phases, based upon the build
    macro PEI_COMPILE.

**/

#include <Efi.h>
#include <CmosAccess.h>
#include "CmosManager.h"
#include "CmosManagerHob.h"
#include "SspTokens.h"
#include "CmosBoard.h"


//------------------------
// Phase-Specific Callouts
//------------------------

extern EFI_STATUS AllocatePoolCallout(
    IN  EFI_PEI_SERVICES  			**PeiServices,
    IN  UINTN             			Size,
    OUT VOID              			**Buffer );

extern EFI_STATUS LocateAccessInterfaceCallout(
    IN  EFI_PEI_SERVICES           	**PeiServices,
    OUT EFI_CMOS_ACCESS_INTERFACE  	**AccessInterface );

extern EFI_STATUS InstallAccessInterfaceCallout(
    IN   EFI_PEI_SERVICES          	**PeiServices,
    OUT  EFI_CMOS_MANAGER_INTERFACE *CmosManager );

extern VOID StatusUpdateCallout(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager);

extern EFI_STATUS ManagerInterfaceInitializeCallout(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager);


//----------------------
// CMOS access functions
//----------------------

extern EFI_STATUS CmosRead(
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    IN  UINT16                      CmosToken,
    OUT UINT8                       *CmosValue );
extern EFI_STATUS CmosWrite(
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    IN  UINT16                      CmosToken,
    IN  UINT8                       CmosValue );
extern UINT16 CmosGetTokenFromRegister(
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    IN  UINT16                      CmosRegister );
extern EFI_STATUS CalculateChecksum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager,
    OUT UINT16                      *ChecksumValue );
extern EFI_STATUS LoadOptimalDefaults(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager );
extern EFI_STATUS WriteChecksum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager,
    OUT UINT16                      ChecksumValue );
extern EFI_STATUS ReadChecksum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager,
    OUT UINT16                      *ChecksumValue );
extern EFI_STATUS ReadCmosStatusBytes(
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    OUT CMOS_STATUS_BYTES           *StatusBytes );
extern EFI_STATUS SynchronizeLegacyStatusRegisters(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager,
    IN CMOS_MANAGER_STATUS          BitMap,
    IN CMOS_BIT_ACCESS_TYPE         AccessType );
extern EFI_STATUS CmosPhysicalReadWrite(
    IN      EFI_CMOS_MANAGER_INTERFACE  *Manager,
    IN      CMOS_ACCESS_TYPE            AccessType,
    IN      UINT16                      CmosAddress,
    IN      UINT8                       BitsToWrite,
    IN OUT  UINT8                       *CmosParameterValue );
extern EFI_STATUS CmosGetDateTime (
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    OUT EFI_TIME                    *Time );
extern EFI_STATUS CmosSetDateTime (
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    IN  EFI_TIME                    *Time );
extern EFI_STATUS UpdateBatteryStatus (
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager );


/**
    Gets the EFI_CMOS_MANAGER_INTERFACE for the current access
    interface.

    @param Cmos This is the access interface pointer.

    @retval EFI_CMOS_MANAGER_INTERFACE * (Return Value)
        -- If successful, the CMOS manager interface is returned.
        -- Otherwise, a valid EFI error code

**/

EFI_CMOS_MANAGER_INTERFACE *GetCmosMangerInterface(
    IN EFI_CMOS_ACCESS_INTERFACE   *Cmos )
{
    // The installed Protocol type will be EFI_CMOS_ACCESS_INTERFACE
    // (converted as needed by the CMOS manager)

    EFI_CMOS_MANAGER_INTERFACE *Interface = (EFI_CMOS_MANAGER_INTERFACE*) Cmos;
    return Interface;
}


/**
    Gets the EFI_CMOS_ACCESS_INTERFACE for the current manager
    interface.

    @param Manager This is the manager interface pointer.

    @retval EFI_CMOS_ACCESS_INTERFACE * (Return Value)
        -- If successful, the CMOS access interface is returned.
        -- Otherwise, NULL is returned.

**/

EFI_CMOS_ACCESS_INTERFACE *GetCmosAccessInterface(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager )
{
    EFI_CMOS_ACCESS_INTERFACE *Interface = NULL;

    // The installed Protocol type will be EFI_CMOS_ACCESS_INTERFACE
    // (converted as needed by the CMOS manager)
    if (Manager != NULL &&
        Manager->CheckStatus(Manager, CMOS_VALID_INTERFACE))
            Interface = (EFI_CMOS_ACCESS_INTERFACE*) Manager;

    return Interface;
}


/**
    This function returns true if all the status bits are set as
    specified by the BitMap.

    @param Manager Pointer to the Manager's interface
    @param BitMap  Bits to check


    @retval BOOLEAN (Return value)
        = TRUE, if all bits in ManagerStatus specified by
        BitMap are set.

        = FALSE, otherwise.

**/

BOOLEAN CmosManagerCheckStatus(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN CMOS_MANAGER_STATUS          BitMap )
{
    if ((Manager->ManagerStatus & BitMap) == BitMap)
        return TRUE;
    else
        return FALSE;
}


/**
    This function returns true if any of the status bits are set as
    specified by the BitMap.

    @param Manager Pointer to the Manager's interface
    @param BitMap  Bits to check


    @retval BOOLEAN (Return value)
        = TRUE, if one or more of the bits in ManagerStatus
        specified by BitMap are set.

        = FALSE, otherwise.

**/

BOOLEAN CmosManagerCheckAnyStatus(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN CMOS_MANAGER_STATUS          BitMap )
{
    if ((Manager->ManagerStatus & BitMap) != 0)
        return TRUE;
    else
        return FALSE;
}


/**
    This function provides a single entry point into the code that
    actually updates status values.

    @param Manager    Pointer to the Manager's interface
    @param BitMap     Bits to set or clear
    @param AccessType SetType or ClearType depending upon whether
                      setting or clearing bits

    @retval EFI_STATUS (Return value)
        = EFI_SUCCESS or valid EFI error code

**/

VOID CmosManagerSetClearStatus(
    IN EFI_CMOS_MANAGER_INTERFACE   *Mgr,
    IN CMOS_MANAGER_STATUS          BitMap,
    IN CMOS_BIT_ACCESS_TYPE         AccessType )
{
    if (AccessType == SetType){
        Mgr->ManagerStatus = Mgr->ManagerStatus | BitMap;

        // A boot/setup event callback explicitly clears these bits each boot
        if ( Mgr->CheckStatus(Mgr, CMOS_IS_USABLE) ){
            if ( BitMap & CMOS_BAD_BATTERY )
                Mgr->Access.Write(&Mgr->Access, CMOS_MGR_BATTERY_BAD, 1);
            if ( BitMap & CMOS_BAD_CHECKSUM )
                Mgr->Access.Write(&Mgr->Access, CMOS_MGR_CHECKSUM_BAD, 1);
            if ( BitMap & CMOS_DEFAULTS_LOADED )
                Mgr->Access.Write(&Mgr->Access, CMOS_MGR_DEFAULTS_LOADED, 1);
            if ( BitMap & CMOS_FIRST_BOOT_DETECTED )
                Mgr->Access.Write(&Mgr->Access, CMOS_MGR_FIRST_BOOT_DETECTED, 1);
        }
    }
    else
        Mgr->ManagerStatus = Mgr->ManagerStatus & ~BitMap;

    // if the Checksum or Battery status is being modified, then
    // update the legacy status registers
    if ( (BitMap & (CMOS_BAD_CHECKSUM | CMOS_BAD_BATTERY)) != 0  ){
        SynchronizeLegacyStatusRegisters(Mgr, BitMap, AccessType);
    }

    if (Mgr->ManagerHob != NULL)
    	Mgr->ManagerHob->ManagerStatus = Mgr->ManagerStatus;

    StatusUpdateCallout(Mgr);
}


/**
    This function sets status bits according to the specified bitmap.


    @param Manager Pointer to the Manager's interface
    @param BitMap  Bits to set

    @retval EFI_STATUS (Return value)
        = EFI_SUCCESS or valid EFI error code

**/

VOID CmosManagerSetStatus(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN CMOS_MANAGER_STATUS          BitMap )
{
    CmosManagerSetClearStatus(Manager, BitMap, SetType);
}


/**
    This function clears status bits according to the specified bitmap.


    @param Manager Pointer to the Manager's interface
    @param BitMap  Bits to clear

    @retval EFI_STATUS (Return value)
        = EFI_SUCCESS or valid EFI error code

**/

VOID CmosManagerClearStatus(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN CMOS_MANAGER_STATUS          BitMap )
{
    CmosManagerSetClearStatus(Manager, BitMap, ClearType);
}


/**
    This function configures the CMOS Manager.

    @param Manager Pointer to the Manager's interface
    @param Setting Configuration setting to invoke.

    @retval EFI_STATUS (Return value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS CmosConfigureManager(
    IN EFI_CMOS_MANAGER_INTERFACE   *Mgr,
    IN CMOS_CONFIGURATION_SETTING   Setting )
{
    EFI_STATUS                  Status = EFI_SUCCESS;

#define IS_REQUESTING(x) ((Setting & (x)) != 0)

    // Basic error checking
    if (    IS_REQUESTING(CMOS_OPTIMAL_DEFAULTS_ON)
         && IS_REQUESTING(CMOS_OPTIMAL_DEFAULTS_OFF) )
    {
        return Status = EFI_INVALID_PARAMETER;
    }

    //-----------------------------------------------------------------------
    // CMOS_OPTIMAL_DEFAULTS_ON
    //-----------------------------------------------------------------------
    // If configuring usage of the optimal defaults buffer and it's already
    // enabled, simply return.  Otherwise...
    //
    //  *  If executing in memory, enable the optimal defaults buffer
    //     and update the checksum.
    //  *  If not executing in memory, allocate the optimal defaults buffer,
    //     copy the values into it, and update the
    //     Manager->OptimalDefaultTable pointer to point to the read/write
    //     buffer.
    //  *  Set the manager status bit to indicate usage of the optimal
    //     defaults buffer.

    if ( IS_REQUESTING( CMOS_OPTIMAL_DEFAULTS_ON ) )
    {
        if (Mgr->CheckStatus(Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED))
            return EFI_SUCCESS;  // already configured

        // This will only occur in PEI phase!!
        if (!Mgr->CheckStatus(Mgr, CMOS_EXECUTING_IN_MEMORY)) {
            VOID *Temp;
            AllocatePoolCallout( Mgr->Access.PeiServices,
                Mgr->OptimalDefaultCount * sizeof(CMOS_REGISTER),
                &Temp);
            MemCpy( Temp , Mgr->OptimalDefaultTable,
                Mgr->OptimalDefaultCount * sizeof(CMOS_REGISTER) );
            Mgr->OptimalDefaultTable = Temp;
        }

        Mgr->SetStatus(Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED);
        Mgr->CalculateChecksum(Mgr, &Mgr->Checksum);
        Mgr->WriteChecksum(Mgr, Mgr->Checksum);
    }

    //-----------------------------------------------------------------------
    // CMOS_OPTIMAL_DEFAULTS_OFF
    //-----------------------------------------------------------------------
    //
    // Clear the status bit, and let the Manager interface user must decide
    // what to do next ...call LoadOptimalDefaults, enable buffered CMOS
    // mode, ....

    if ( IS_REQUESTING( CMOS_OPTIMAL_DEFAULTS_OFF ) ){
        Mgr->ClearStatus(Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED);
    }

#undef IS_REQUESTING

    return Status;
}


/**
    This function saves the specified pointer to a predefined 4-byte
    CMOS location.

    @param CmosManager Pointer to the manager's interface
    @param ApiPointer  API address

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS SaveApiPointerToCmos(
    IN EFI_CMOS_MANAGER_INTERFACE      *Mgr,
    IN EFI_CMOS_ACCESS_INTERFACE       *ApiPointer )
{
    UINT32   Offset;
    UINT8    Byte;  // Only used in PEI

    if (ApiPointer == NULL)
        ApiPointer = Mgr->GetAccessInterface(Mgr);

    if ( !Mgr->CheckAnyStatus(Mgr, CMOS_IS_USABLE ) ){
        CMOS_TRACE_FULL(( Mgr,
            "CMOS Not Usable: Could not save API pointer.\n"));
        return EFI_DEVICE_ERROR;
    }

    for (Offset = 0; Offset < 4; Offset++){
        Byte = (UINT8)((UINT32)ApiPointer >> (24 - (Offset * 8))) & 0xff;
        CmosPhysicalReadWrite(  Mgr,
                                WriteType,
                                CMOS_ACCESS_API_BYTE3 + Offset,
                                ALL_BITS,
                                &Byte );
    }

    return EFI_SUCCESS;
}


/**
    This function get the CMOS API pointer from a predefined 4-byte
    CMOS location and updates the output parameter with the pointer.

    @param CmosManager Pointer to the manager's interface to be installed
    @param ApiPointer  Address of the API pointer

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS GetApiPointerFromCmos(
    IN EFI_CMOS_MANAGER_INTERFACE   *Mgr,
    EFI_CMOS_ACCESS_INTERFACE       **ApiPointer )
{
    UINT8   Temp;
    UINT32  Ptr = 0;
    UINT8   Offset;

    if ( !Mgr->CheckAnyStatus(Mgr, CMOS_IS_USABLE ) ){
        CMOS_TRACE_FULL(( Mgr,
            "CMOS Not Usable: Could not read API pointer.\n"));
        return EFI_DEVICE_ERROR;
    }

    for (Offset = 0; Offset < 4; Offset++){
        CmosPhysicalReadWrite(  Mgr,
                                ReadType,
                                CMOS_ACCESS_API_BYTE3 + Offset,
                                0,
                                &Temp );
        Ptr =  (Ptr << 8) | Temp;
    }

    *ApiPointer = (EFI_CMOS_ACCESS_INTERFACE*) Ptr;

    return EFI_SUCCESS;
}


/**
    This function installs either the PEI or DXE phase interface
    (PPI or Protocol) for accessing CMOS.

    @param CmosManager Pointer to the interface to be installed

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code
        CmosManager->ManagerStatus
        = CMOS_VALID_MANAGER indicates successful
        installation of interface.

    @note
      This function is used for both PEI and DXE phase.  The build
      macro PEI_COMPILE will determine how the object code is
      created/linked.

**/

EFI_STATUS InstallCmosAccessInterface(
    IN EFI_CMOS_MANAGER_INTERFACE      *CmosManager )
{
    EFI_STATUS          Status;

    Status = InstallAccessInterfaceCallout(CmosManager->Access.PeiServices,
    		CmosManager);

    if (!EFI_ERROR(Status))
        CmosManager->SetStatus(CmosManager, CMOS_VALID_MANAGER);
    else {
        CmosManager->SetStatus(CmosManager, CMOS_INSTALL_FAILED);
        CmosManager->ClearStatus(CmosManager, CMOS_VALID_MANAGER);
    }

    return Status;
}


/**
    This function initializes structure data and function pointer
    elements for a EFI_CMOS_MANAGER_INTERFACE.

    @param PeiServices This parameter will be NULL in the DXE phase
    @param CmosManager Pointer to the interface to be initialized

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code
        CmosManager->ManagerStatus
        = CMOS_VALID_INTERFACE indicates successful
        initialization of the structure.

    @note
      This function is used for both PEI and DXE phase.  The build
      macro PEI_COMPILE will determine how the object code is
      created.

**/


EFI_STATUS InitializeCmosManagerInterface(
    IN  EFI_PEI_SERVICES                **PeiServices,
    OUT EFI_CMOS_MANAGER_INTERFACE      *CmosManager )
{
    EFI_STATUS Status = EFI_SUCCESS;

    // PeiServices pointer must be initialized before calling CMOS_TRACE.
    CmosManager->Access.PeiServices = PeiServices;

    CMOS_TRACE_FULL(( CmosManager,
        "InitializeCmosManagerInterface Entry\n"));

    // Note: all pointers/values are already NULL/Zero

    // Not using an initializer list in order to avoid problems
    // if/when the interface declarations are modified.

    //-----------------------------------------------------------------------
    // Phase-independent initialization
    //-----------------------------------------------------------------------
    CmosManager->Access.Read = CmosRead;
    CmosManager->Access.Write = CmosWrite;
    CmosManager->Access.GetTokenFromRegister = CmosGetTokenFromRegister;
    CmosManager->Access.ReadCmosStatusBytes = ReadCmosStatusBytes;
    CmosManager->Access.GetTime = CmosGetDateTime;
    CmosManager->Access.SetTime = CmosSetDateTime;
    CmosManager->GetAccessInterface = GetCmosAccessInterface;
    CmosManager->InstallAccessInterface = InstallCmosAccessInterface;
    CmosManager->CalculateChecksum = CalculateChecksum;
    CmosManager->ReadChecksum = ReadChecksum;
    CmosManager->WriteChecksum = WriteChecksum;
    CmosManager->ConfigureManager = CmosConfigureManager;
    CmosManager->CheckStatus = CmosManagerCheckStatus;
    CmosManager->CheckAnyStatus = CmosManagerCheckAnyStatus;
    CmosManager->SetStatus = CmosManagerSetStatus;
    CmosManager->ClearStatus = CmosManagerClearStatus;
    CmosManager->LoadOptimalDefaults = LoadOptimalDefaults;
    CmosManager->SaveApiPointerToCmos = SaveApiPointerToCmos;
    CmosManager->GetApiPointerFromCmos = GetApiPointerFromCmos;

    CmosManager->SetStatus(CmosManager, CMOS_VALID_INTERFACE);

    return Status;
}


/**
    This function locates (or allocates memory for) the
    EFI_CMOS_MANAGER_INTERFACE, calculates the checksum, checks the
    battery condition and updates the Manager's status bits.

    @param PeiServices This parameter will be NULL in the DXE phase

    @retval EFI_CMOS_MANAGER_INTERFACE* (Return Value)
        =  If successful, a pointer to the allocated/found
        structure is returned.
        =  NULL is returned on failure to allocate

        NewManager->ManagerStatus
        =  CMOS_INTERFACE_ALREADY_INSTALLED, if the interface is
        found prior to allocating
        =  CMOS_VALID_INTERFACE, on successful initialization
        of the access interface.

    @note
      This function is used for PEI, DXE and SMM interface initialization.
      The build macros PEI_COMPILE and SMM_COMPILE will determine how the
      object code is compiled.

      See CMOS_MANAGER_STATUS enum type for more information on manager
      status bits.

      Assumptions:

               1) This function should only be called once for each
                  entrypoint.

               2) It is up to the user of the Manager interface to determine
                  how to use the CMOS_MANAGER_STATUS bits upon return from
                  this function.

**/

EFI_CMOS_MANAGER_INTERFACE* NewCmosManagerInterface(
    IN EFI_PEI_SERVICES        **PeiServices )
{
    EFI_CMOS_MANAGER_INTERFACE    *NewManager;
    EFI_STATUS                    Status;
    EFI_CMOS_IS_FIRST_BOOT        *IsFirstBoot;
    EFI_CMOS_IS_COLD_BOOT         *IsColdBoot;
    EFI_CMOS_IS_BSP               *IsBsp;
    EFI_CMOS_IS_CMOS_USABLE       *CmosIsUsable;

    //-----------------------------------------------------------------------
    // If the interface is found then no initialization is done.
    //-----------------------------------------------------------------------
    Status = LocateAccessInterfaceCallout(PeiServices,
    		(EFI_CMOS_ACCESS_INTERFACE**)&NewManager);
    if (Status == EFI_SUCCESS)
    {
        NewManager->SetStatus(NewManager, CMOS_INTERFACE_ALREADY_INSTALLED);
        return NewManager;
    }

    //-----------------------------------------------------------------------
    // Return NULL on allocation error.
    //-----------------------------------------------------------------------
    if (EFI_ERROR(AllocatePoolCallout( PeiServices,
                                       sizeof(EFI_CMOS_MANAGER_INTERFACE),
                                       (VOID**)&NewManager ) )){
        return NULL;
    }

    //-----------------------------------------------------------------------
    // Zero the interface buffer (also ensures ManagerStatus bits are clear
    // and ensures pointers are NULL)
    //-----------------------------------------------------------------------
    MemSet( (void*)NewManager, sizeof(EFI_CMOS_MANAGER_INTERFACE), 0);

    //-----------------------------------------------------------------------
    // Initialize general structures and function pointers
    //-----------------------------------------------------------------------
    InitializeCmosManagerInterface(PeiServices, NewManager);

    //-----------------------------------------------------------------------
    // The following callout:
    //
    // In PEI phase:
    //      a) updates memory status
    //      b) initializes structure members in the private interface
    //
    // In DXE phase:
    //      a) gets the HOB
    //      b) completes the initialization of the private interface data
    //         structure pointers
    //
    // In SMM registration:
    //      a)  Use the original optimal defaults buffer, as the DXE buffer
    //          has already by flushed to the physical registers if the battery
    //          is good.  (If the battery is bad, it is unlikely CMOS is of
    //          any use in SMM.)
    //-----------------------------------------------------------------------
    ManagerInterfaceInitializeCallout(NewManager);

    IsFirstBoot = NewManager->IsFirstBoot;
    IsColdBoot = NewManager->IsColdBoot;
    IsBsp = NewManager->IsBsp;
    CmosIsUsable = NewManager->CmosIsUsable;

    CMOS_TRACE_FULL((NewManager, "Call CmosIsUsable: " ));
    while (*CmosIsUsable != NULL){
      if ( (*CmosIsUsable)( NewManager->Access.PeiServices ) ){
        NewManager->SetStatus(NewManager, CMOS_IS_USABLE);
        CMOS_TRACE_FULL((NewManager, "CMOS is usable\n" ));
      }
      else {
        NewManager->ClearStatus(NewManager, CMOS_IS_USABLE);
        CMOS_TRACE_FULL((NewManager, "CMOS is not usable\n" ));
      }
      ++CmosIsUsable;
    }

    //-----------------------------------------------------------------------
    // Update CMOS_FIRST_BOOT_DETECTED status bit before calculating
    // the checksum or determining the battery status.
    //-----------------------------------------------------------------------
    CMOS_TRACE_FULL((NewManager, "Call gIsFirstBoot: " ));
    while (*IsFirstBoot != NULL){
      if ( (*IsFirstBoot)( NewManager->Access.PeiServices ) ){
        NewManager->SetStatus(NewManager, CMOS_FIRST_BOOT_DETECTED);
        CMOS_TRACE_FULL((NewManager, "First boot detected\n" ));
      }
      else {
        CMOS_TRACE_FULL((NewManager, "First boot NOT detected\n" ));
      }
      ++IsFirstBoot;
    }

    // Update CMOS_COLD_BOOT_DETECTED status bit.
    CMOS_TRACE_FULL((NewManager, "Call gIsColdBoot: " ));
    while (*IsColdBoot != NULL){
      if ( (*IsColdBoot)( NewManager->Access.PeiServices ) ){
        NewManager->SetStatus(NewManager, CMOS_COLD_BOOT_DETECTED);
        CMOS_TRACE_FULL((NewManager, "Cold boot detected\n" ));
      }
      else {
        CMOS_TRACE_FULL((NewManager, "Cold boot NOT detected\n" ));
      }
      ++IsColdBoot;
    }

    // Update CMOS_BSP_IS_EXECUTING status bit.
    CMOS_TRACE_FULL((NewManager, "Call gIsBsp: " ));
    while (*IsBsp != NULL){
      if ( (*IsBsp)( NewManager->Access.PeiServices ) ){
        NewManager->SetStatus(NewManager, CMOS_BSP_IS_EXECUTING);
        CMOS_TRACE_FULL((NewManager, "BSP is executing\n" ));
      }
      else {
        CMOS_TRACE_FULL((NewManager, "BSP is NOT executing\n" ));
      }
      ++IsBsp;
    }

    // Configure incoherency recovery policy
#if CMOS_MGR_RECOVER_ONLY_CHECKUMMED
    NewManager->SetStatus(NewManager, CMOS_RECOVER_ONLY_CHECKSUMMED);
#endif

#if CMOS_MGR_RECOVER_IN_PEI
    NewManager->SetStatus(NewManager, CMOS_RECOVER_IN_PEI);
#endif

    //-----------------------------------------------------------------------
    // Update the NewManager->Checksum
    //
    // Note: This call only calculates and updates the checksum in
    //       the manager interface structure.  The CMOS_BAD_CHECKSUM bit
    //       is set in NewManager->ManagerStatus, if the calculated checksum
    //       does not match the current checksum in CMOS.
    //
    //       The user of the Manager interface will decide how to handle
    //       a bad checksum.
    //-----------------------------------------------------------------------
    NewManager->CalculateChecksum(NewManager, &NewManager->Checksum);

    UpdateBatteryStatus(NewManager);

    return NewManager;
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
