//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//    Rev.  1.00    Force PCIe OPROM only controlled by SmcOpromCtrl module.
//     Bug Fix:     
//     Reason:      Set CSM default OPROM policy to 3(all OpROMs are enabled)
//     Auditor:     Jimmy Chiu
//     Date:        Jul/21/2016
//
//***************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
/** @file CsmPolicy.c
    Implementation of CSM policy functions

**/
//**********************************************************************


//----------------------------------------------------------------------------

#include <Token.h>
#include <AmiDxeLib.h>
#include <BootOptions.h>
#include <Protocol/PciIo.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Setup.h>
#include <CsmElinkFunctions.h>
#include <Protocol/AmiCsmOpromPolicy.h>

#define DEFAULT_HANDLE_BUFFER_SIZE 0x40
typedef struct {
    UINTN TotalSize;
    UINTN CurrentSize;
    EFI_HANDLE *Array;
} HANDLE_BUFFER;

HANDLE_BUFFER ThunkHandles;
HANDLE_BUFFER NonThunkHandles;
static EFI_HANDLE SharedGopHandle = 0;
static BOOLEAN UefiGopDisconnected = FALSE;
static BOOLEAN ProceedToBoot = FALSE;

EFI_EVENT gLocateHanedlesPolicyEvent = NULL;
EFI_EVENT gProceedToBootPolicyEvent = NULL;
EFI_EVENT gPolicyEvent = NULL;

EFI_STATUS CsmPolicyGetDriver(
	IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
	IN EFI_HANDLE ControllerHandle,
	IN OUT EFI_HANDLE *DriverImageHandle
);

EFI_STATUS CsmPolicyGetDriverPath(
	IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
	IN EFI_HANDLE ControllerHandle,
	IN OUT EFI_DEVICE_PATH_PROTOCOL **DriverImagePath
) { return EFI_UNSUPPORTED; }

EFI_STATUS CsmPolicyDriverLoaded(
	IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
	IN EFI_HANDLE ControllerHandle,
	IN EFI_DEVICE_PATH_PROTOCOL *DriverImagePath,
	IN EFI_HANDLE DriverImageHandle
) { return EFI_UNSUPPORTED; }

static EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL CsmPolicyPlatformDriverOverride = {
    CsmPolicyGetDriver,
    CsmPolicyGetDriverPath,
    CsmPolicyDriverLoaded
};

//----------------------------------------------------------------------------

static EFI_GUID gSetupGuid = SETUP_GUID;
extern EFI_GUID gBdsAllDriversConnectedProtocolGuid;
extern EFI_GUID gEfiDriverBindingProtocolGuid;
extern EFI_GUID gAmiOpromPolicyProtocolGuid;
extern EFI_GUID gAmiOpromPolicyByPciHandleProtocolGuid;
extern EFI_GUID gAmiCsmThunkDriverGuid;

typedef EFI_STATUS (OEM_CHECK_UEFI_OPROM_POLICY) (EFI_HANDLE PciHandle);
extern OEM_CHECK_UEFI_OPROM_POLICY CSM_CHECK_UEFI_OPROM_POLICY EndOfCheckUefiOpromPolicyFunctions;
OEM_CHECK_UEFI_OPROM_POLICY* OemCheckUefiOpromPolicyList[] = { CSM_CHECK_UEFI_OPROM_POLICY NULL };

UINT8 CurrentCsmState;
VOID *ProcessOpRomRegistration;
VOID *DriverBindingRegistration;
static UINTN CurrentCount = 0;
static BOOLEAN ThunkFirst;
SETUP_DATA SetupData;

EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL *BusSpecific = NULL;

typedef struct CSM_POLICY_SETUP_DATA {
    UINT8  BootOptionFilter;
    UINT8  PxeOpRom;
    UINT8  MassStorageOpRom;
    UINT8  VideoOpRom;
/*
    We added the following field to preserve functionality existed in previous
    PciBus versions. For PCI devices other than Network,Mass Storage or Video
    value of following field will determine OpROM behavior as before.
*/
    UINT8  OldOpRom;
} CSM_POLICY_SETUP_DATA;

CSM_POLICY_SETUP_DATA CsmPolicySetupData = {
    DEFAULT_BOOT_OPTION_FILTERING_POLICY,
    DEFAULT_PXE_OPROM_POLICY,
    DEFAULT_MASS_STORAGE_OPROM_POLICY,
    DEFAULT_VIDEO_OPROM_POLICY,
    1
};

EFI_STATUS CheckUefiOpRomPolicy(
    IN	AMI_OPROM_POLICY_PROTOCOL *This,
    IN	UINT8                     PciClass
);

VOID ProcessOpRom(
    IN OPTIONAL CSM_PLATFORM_POLICY_DATA *OpRomStartEndProtocol
);

AMI_OPROM_POLICY_PROTOCOL AmiOpRomPolicyProtocol = {
    CheckUefiOpRomPolicy,
    ProcessOpRom
};

EFI_STATUS CheckUefiOpRomPolicyByPciHandle(
    IN  AMI_OPROM_POLICY_PROTOCOL *This,
    IN  EFI_HANDLE                PciHandle
);

AMI_CSM_OPROM_POLICY_PROTOCOL AmiOpRomPolicyByPciHandleProtocol = {
    CheckUefiOpRomPolicyByPciHandle,
    ProcessOpRom
};

EFI_STATUS AddHandle(
    IN OUT HANDLE_BUFFER *Target,
    EFI_HANDLE Handle
);

/**
    This function filters available boot options based on Setup
    control value
               
         
    @param Device Pointer to boot device

          
    @retval TRUE boot option will be removed from boot list
    @retval FALSE boot option won't be removed from boot list

**/
BOOLEAN CsmPolicyFilter( 
    IN BOOT_DEVICE *Device 
)
{
    BOOLEAN Result = FALSE;

    if(CurrentCsmState == 0)    //CSM is disabled on current boot - no need to filter boot options
        return Result;

    switch (CsmPolicySetupData.BootOptionFilter) {
        case 0: // Enable all boot options
            break;

        case 1: // Disable UEFI boot options
            Result = (Device->BbsEntry == NULL);
            break;

        case 2: // Disable Legacy boot options
            Result = (Device->BbsEntry != NULL);
            break;

        default:
            break;
    }
    return Result;
}

/**
    This function returns OpROM policy for given PCI class of the device

    @param 
        UINT8 PciClass - class of PCI device to check

         
    @retval UINT8 policy (0-disable all, 1-disable Legacy, 2-disable UEFI)

**/
UINT8 GetOpRomPolicy(
    IN UINT8 PciClass
)
{
    UINT8 Policy = 0xff;

    switch(PciClass) {
        case PCI_CL_NETWORK:
            Policy = CsmPolicySetupData.PxeOpRom;
            break;

        case PCI_CL_MASS_STOR:
            Policy = CsmPolicySetupData.MassStorageOpRom;
            break;

        case PCI_CL_DISPLAY:
            Policy = CsmPolicySetupData.VideoOpRom;
            break;

        default:
/*
    For PCI devices with PCI class different from Network, Mass Storage or Video
    behavior will be as in previous versions of PciBus driver - UEFI OpROM will
    be executed first or not executed at all
*/
            Policy = CsmPolicySetupData.OldOpRom;
#if SMCPKG_SUPPORT
            Policy = DEFAULT_OLD_OPROM_POLICY;
#endif
            break;
    }
    return Policy;
}

/**
    This function determines should OpROM be executed

    @param PciClass - class of PCI device to check
    @param UefiOprom - TRUE if verifying UEFI OpROM, FALSE for Legacy OpROM

         
    @retval EFI_SUCCESS OpROM should be executed
    @retval EFI_UNSUPPORTED OpROM should not be executed

**/
EFI_STATUS CheckOpRomExecution(
    IN UINT8 PciClass,
    IN BOOLEAN UefiOprom
)
{
    UINT8 Policy;
    BOOLEAN Result = TRUE;

    Policy = GetOpRomPolicy(PciClass);

    switch(Policy) {
        case 0:     //all OpROMs are disabled
            Result = FALSE;
            break;

        case 1:     //only UEFI OpROMs are enabled
            Result = (UefiOprom) ? TRUE : FALSE;
            break;

        case 2:     //only Legacy OpROMs are enabled
            Result = (UefiOprom) ? FALSE : TRUE;
            break;

        default:    //all OpROMs are enabled
            break;
    }
    return (Result) ? EFI_SUCCESS : EFI_UNSUPPORTED;       
}

/**
    This function disconnects native UEFI GOP driver before
    executing legacy video BIOS

          
    @param PciIo instance of PciIo protocol on handle,
        that have to be disconnected

         
    @retval EFI_SUCCESS driver was disconnected
    @retval EFI_UNSUPPORTED driver can't be disconnected

**/
EFI_STATUS DisconnectUefiGop(
    IN EFI_PCI_IO_PROTOCOL *PciIo
)
{
    EFI_STATUS Status;
    EFI_HANDLE *Handles;
    EFI_HANDLE TmpHandle;
    UINTN Count;
    UINTN i;
    VOID *Interface;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *Entries;
    UINT64 Capabilities;
    
/* first find corresponding handle */
    Status = pBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL, &Count, &Handles);
    if(EFI_ERROR(Status))
        return Status;

    for(i = 0; i < Count; i++) {
        Status = pBS->HandleProtocol(Handles[i], &gEfiPciIoProtocolGuid, &Interface);
        if(EFI_ERROR(Status))
            continue;

        if((VOID *)PciIo == Interface) {
            SharedGopHandle = Handles[i];
            break;
        }
    }
    pBS->FreePool(Handles);

    if(i == Count)              //no matches
        return EFI_NOT_FOUND;

/* now find child handle where Gop is installed */
    Status = pBS->OpenProtocolInformation(SharedGopHandle, &gEfiPciIoProtocolGuid, &Entries, &Count);
    if(EFI_ERROR(Status))
        return Status;

    for(i = 0; i < Count; i++) {
        if(!(Entries[i].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER))
            continue;

        Status = pBS->HandleProtocol(Entries[i].ControllerHandle, &gEfiGraphicsOutputProtocolGuid, &Interface);
        if(!EFI_ERROR(Status)) {
            TmpHandle = Entries[i].AgentHandle;
            break;
        }
    }
    pBS->FreePool(Entries);

    if(i == Count)              //no matches
        return EFI_NOT_FOUND;

/* now check if AgentHandle is our CSM thunk */
    Status = pBS->HandleProtocol(TmpHandle, &gAmiCsmThunkDriverGuid, &Interface);
    if(!EFI_ERROR(Status))
        return EFI_SUCCESS;

    Status = pBS->DisconnectController(SharedGopHandle, NULL, NULL);
/* we have to restore PCI attributes so video BIOS can be executed */
    Status = PciIo->Attributes (PciIo, EfiPciIoAttributeOperationSupported, 
                0, &Capabilities); 
    Status = PciIo->Attributes (PciIo, EfiPciIoAttributeOperationEnable,
                Capabilities & EFI_PCI_DEVICE_ENABLE, NULL);        
    return Status;
}

/**
    This function will be called before and after installing legacy OpROM

          
    @note  
  PolicyData is valid when this call is made before Option ROM execution, it
  is NULL when this function is called after Option ROM execution.

**/
VOID ProcessOpRom(
    IN OPTIONAL CSM_PLATFORM_POLICY_DATA *OpRomStartEndProtocol
)
{
    EFI_STATUS Status;
//    EFI_HANDLE Handle;
//    UINTN Size = sizeof(EFI_HANDLE);
//    CSM_PLATFORM_POLICY_DATA *OpRomStartEndProtocol;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINT8 PciClassCode;
/*
    Status = pBS->LocateHandle(ByRegisterNotify, NULL, ProcessOpRomRegistration, &Size, &Handle);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) 
        return;

    Status = pBS->HandleProtocol(Handle, &gOpromStartEndProtocolGuid, &OpRomStartEndProtocol);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) 
        return;
*/
    if(OpRomStartEndProtocol == NULL) { //post-process OpROM callback
        if(UefiGopDisconnected && !ProceedToBoot)
/* if this is last OpROM, and Gop was disconnected and we're not on legacy boot path */
            pBS->ConnectController(SharedGopHandle, NULL, NULL, TRUE);
        return;
    }

//check if someone already disabled execution of this OpROM
    if(OpRomStartEndProtocol->PciIo == NULL || OpRomStartEndProtocol->ExecuteThisRom == FALSE)
        return;

    PciIo = OpRomStartEndProtocol->PciIo;
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xB, 1, &PciClassCode);
    if (EFI_ERROR(Status)) 
        return;

    Status = CheckOpRomExecution(PciClassCode, FALSE);
    if(EFI_ERROR(Status)) {
        OpRomStartEndProtocol->ExecuteThisRom = FALSE;
        return;
    }

/* OK, we're decided to execute OpROM */
    OpRomStartEndProtocol->ExecuteThisRom = TRUE;

    if(PciClassCode == PCI_CL_DISPLAY) {
        Status = DisconnectUefiGop(PciIo);
    /* 
        we are executing video OpROM even if we can't disconnect
        native UEFI driver - possible bug
    */
        if(!EFI_ERROR(Status))
            UefiGopDisconnected = TRUE;
    }
    return;
}

/**
    This callback executed every time DriverBinding is installed

          
    @param Event Callback event
    @param Context pointer to calling context

**/
VOID LocateHandlesCallback(
    IN EFI_EVENT Event, 
    IN VOID *Context
)
{
    EFI_STATUS Status;
    EFI_STATUS Status1;
    UINTN Count = sizeof(EFI_HANDLE);
    EFI_HANDLE Handle;
    VOID *DummyInterface;

    do {
        Status = pBS->LocateHandle(ByRegisterNotify, 
                    NULL, 
                    DriverBindingRegistration, 
                    &Count, 
                    &Handle);
        if(!EFI_ERROR(Status)) {
            Status1 = pBS->HandleProtocol(Handle, 
                        &gAmiCsmThunkDriverGuid, 
                        &DummyInterface);
            if(EFI_ERROR(Status1)) { 
                Status = AddHandle(&NonThunkHandles, Handle);
            } else {
                Status = AddHandle(&ThunkHandles, Handle);
            }
        }
    } while (!EFI_ERROR(Status));
            
    return;
}

/**
    This function allocates buffer to store DriverBinding handles

          
    @param Target pointer to buffer to be initialized

**/
EFI_STATUS InitHandleBuffer(
    IN OUT HANDLE_BUFFER *Target
)
{
    EFI_STATUS Status;

    Status = pBS->AllocatePool(EfiBootServicesData, 
                    sizeof(EFI_HANDLE) * DEFAULT_HANDLE_BUFFER_SIZE, 
                    (VOID **)&(Target->Array));
    if(!EFI_ERROR(Status)) {
        Target->TotalSize = DEFAULT_HANDLE_BUFFER_SIZE;
        Target->CurrentSize = 0;
    }
    return Status;
}

/**
    This function increases and reallocates buffer to store 
    DriverBinding handles

          
    @param Target pointer to buffer to be increased

**/
EFI_STATUS IncreaseHandleBuffer(
    IN OUT HANDLE_BUFFER *Target
)
{
    EFI_STATUS Status;
    VOID *NewArray;

    Status = pBS->AllocatePool(EfiBootServicesData, 
                    sizeof(EFI_HANDLE) * (Target->TotalSize + DEFAULT_HANDLE_BUFFER_SIZE), 
                    &NewArray);
    if(!EFI_ERROR(Status)) {
        MemCpy(NewArray, Target->Array, sizeof(EFI_HANDLE) * Target->CurrentSize);
        Target->TotalSize += DEFAULT_HANDLE_BUFFER_SIZE;

        pBS->FreePool(Target->Array);
        Target->Array = (EFI_HANDLE *)NewArray;
    }
    return Status;
}

/**
    This function stores handles in allocated buffer

          
    @param Target pointer to buffer to store handle in
    @param Handle handle to be stored

    @retval 
        EFI_STATUS

**/
EFI_STATUS AddHandle(
    IN OUT HANDLE_BUFFER *Target,
    EFI_HANDLE Handle
)
{
    EFI_STATUS Status;

    if(Target->CurrentSize >= Target->TotalSize) {
        Status = IncreaseHandleBuffer(Target);
        if(EFI_ERROR(Status))
            return Status;
    }

    Target->Array[Target->CurrentSize] = Handle;
    Target->CurrentSize++;
    return EFI_SUCCESS;
}

/**
    This callback executed when all drivers are connected and system
    proceeds to boot

          
    @param Event Callback event
    @param Context pointer to calling context

**/
VOID ProceedToBootCallback(
    IN EFI_EVENT Event, 
    IN VOID *Context
)
{
    ProceedToBoot = TRUE;
}


/**
    Initialization of CsmPolicy data; this is a callback notification on
    gLegacyBiosProtocol installation

               
    @param Event Callback event
    @param Context pointer to calling context

**/
VOID CsmPolicyInit(
    IN EFI_EVENT Event, 
    IN VOID *Context
)
{
    EFI_STATUS Status;
    VOID *Registration;
    EFI_HANDLE Handle;
    UINTN Size = sizeof(EFI_HANDLE);

    CurrentCsmState = 1;

    Status = pBS->LocateHandle(ByProtocol, &gEfiLegacyBiosProtocolGuid, NULL, &Size, &Handle);

    if(EFI_ERROR(Status)) {
      return;
    }
	
    CsmPolicySetupData.BootOptionFilter = SetupData.BootOptionFilter;
    CsmPolicySetupData.VideoOpRom = SetupData.VideoOpRom;

/*        Status = RegisterProtocolCallback(&gOpromStartEndProtocolGuid, 
                     ProcessOpRomCallback, 
                     NULL, 
                     &PolicyEvent, 
                     &ProcessOpRomRegistration);
    if(EFI_ERROR(Status))
        return Status;
*/
    Status = RegisterProtocolCallback(&gBdsAllDriversConnectedProtocolGuid, 
                     ProceedToBootCallback, 
                     NULL, 
                     &gProceedToBootPolicyEvent, 
                     &Registration);
    ASSERT_EFI_ERROR(Status);

    /* Init platform driver override protocol */
    Status = InitHandleBuffer(&ThunkHandles);
    ASSERT_EFI_ERROR(Status);

    Status = InitHandleBuffer(&NonThunkHandles);
    ASSERT_EFI_ERROR(Status);

    Status = RegisterProtocolCallback(&gEfiDriverBindingProtocolGuid, 
                    LocateHandlesCallback, 
                    NULL, 
                    &gLocateHanedlesPolicyEvent, 
                    &DriverBindingRegistration);
    ASSERT_EFI_ERROR(Status);

    /*call callback manually to get already registered handles */
    LocateHandlesCallback(gLocateHanedlesPolicyEvent, NULL);
    
    Status = pBS->InstallMultipleProtocolInterfaces(&Handle,
                    &gAmiOpromPolicyProtocolGuid, 
                    &AmiOpRomPolicyProtocol,
                    &gAmiCsmOpromPolicyProtocolGuid, 
                    &AmiOpRomPolicyByPciHandleProtocol,
                    &gEfiPlatformDriverOverrideProtocolGuid, 
                    &CsmPolicyPlatformDriverOverride,
                    NULL);
    ASSERT_EFI_ERROR(Status);

}


/**
    This function is CsmPolicy driver entry point 

         
    @param ImageHandle Image handle
    @param SystemTable pointer to system table

          
    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_ERROR Some error occurred during execution

**/
EFI_STATUS
EFIAPI
CsmPolicyEntry(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;
    EFI_HANDLE Handle;
    UINTN Size = sizeof(SETUP_DATA);
    VOID *Registration;

    InitAmiLib(ImageHandle, SystemTable);
    
    Status = pRS->GetVariable(L"Setup", &gSetupGuid, NULL, &Size, &SetupData);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    
    CsmPolicySetupData.BootOptionFilter = SetupData.BootOptionFilter;
    CsmPolicySetupData.PxeOpRom = SetupData.PxeOpRom;
    CsmPolicySetupData.MassStorageOpRom = SetupData.MassStorageOpRom;
    CsmPolicySetupData.VideoOpRom = SetupData.VideoOpRom;
    CsmPolicySetupData.OldOpRom = SetupData.OldOpRom;

    CurrentCsmState = 0;
    
// Determine current CSM state based on whether gEfiLegacyBiosProtocolGuid protocol is installed
    Size = sizeof(EFI_HANDLE);
    Status = pBS->LocateHandle(ByProtocol, &gEfiLegacyBiosProtocolGuid, NULL, &Size, &Handle);

    if(!EFI_ERROR(Status)) {
        CsmPolicyInit(NULL, NULL);
    } else {
        Status = RegisterProtocolCallback(&gEfiLegacyBiosProtocolGuid, 
                    CsmPolicyInit, 
                    NULL, 
                    &gPolicyEvent, 
                    &Registration);
//Reset Boot option filter to avoid situation, when no boot options are available
        CsmPolicySetupData.BootOptionFilter = 0;
//Set video policy to UEFI only
        CsmPolicySetupData.VideoOpRom = 1;
    }

    return EFI_SUCCESS;
}


/**
  Library destructor.
  Free resources, remove the callbacks registered in constructor.
**/

EFI_STATUS
EFIAPI
CsmPolicyDestructor(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    if (gLocateHanedlesPolicyEvent != NULL)
    {
        pBS->CloseEvent(gLocateHanedlesPolicyEvent);
        gLocateHanedlesPolicyEvent = NULL;
    }
    if (gProceedToBootPolicyEvent != NULL)
    {
        pBS->CloseEvent(gProceedToBootPolicyEvent);
        gProceedToBootPolicyEvent = NULL;
    }

    if (gPolicyEvent != NULL)
    {
        pBS->CloseEvent(gPolicyEvent);
        gPolicyEvent = NULL;
    }
    
    return EFI_SUCCESS;
}

/**
    This function is a part of AmiOpRomPolicyProtocol

         
    @param This -pointer to AMI_OPROM_POLICY_PROTOCOL
    @param PciClass -PCI device class

          
    @retval EFI_SUCCESS OpROM should be executed
    @retval EFI_UNSUPPORTED OpROM should not be executed

**/
EFI_STATUS CheckUefiOpRomPolicy(
    IN	AMI_OPROM_POLICY_PROTOCOL *This,
    IN	UINT8                     PciClass
)
{
    return CheckOpRomExecution(PciClass, TRUE);
}

/**
    This function is a part of AmiOpRomPolicyByPciHandleProtocol

         
    @param This -pointer to pointer to AMI_OPROM_POLICY_BY_PCIHANDLE_PROTOCOL
    @param PciHandle -PCI device handle

          
    @retval EFI_SUCCESS OpROM should be executed
    @retval EFI_UNSUPPORTED OpROM should not be executed

**/
EFI_STATUS CheckUefiOpRomPolicyByPciHandle(
    IN  AMI_OPROM_POLICY_PROTOCOL *This,
    IN  EFI_HANDLE                PciHandle
)
{
    UINT32 i;
    EFI_STATUS Status;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINT8 PciClass;

    for(i = 0; OemCheckUefiOpromPolicyList[i] != NULL; i++) {
        Status = OemCheckUefiOpromPolicyList[i](PciHandle);
        if(Status == EFI_SUCCESS || Status == EFI_UNSUPPORTED)
            return Status;
    }

    Status = pBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &PciIo);
    if(EFI_ERROR(Status))
        return Status;

    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xB, 1, &PciClass);

    return (EFI_ERROR(Status))? Status : CheckOpRomExecution(PciClass, TRUE);
}

/**
    This function returns bus-specific handle for given controller

          
    @param ControllerHandle handle of the controller
    @param DriverImageHandle pointer to returned handle placeholder

         
    @retval EFI_SUCCESS handle returned
    @retval EFI_NOT_FOUND handle not found

**/
EFI_STATUS GetBusSpecificHandle(
	IN EFI_HANDLE ControllerHandle,
	IN OUT EFI_HANDLE *DriverImageHandle
)
{
    EFI_STATUS Status;

    if(BusSpecific == NULL) {
        Status = pBS->HandleProtocol(
                        ControllerHandle,  
                        &gEfiBusSpecificDriverOverrideProtocolGuid, 
                        &BusSpecific);
        if(EFI_ERROR(Status))
            return Status;
    }

    return BusSpecific->GetDriver(BusSpecific, DriverImageHandle);
}

/**
    This function returns if given PCI device class supports 
    controlled UEFI/Legacy OpROM execution sequence

          
    @param PciClass class of the device

         
    @retval TRUE controlled sequence supported
    @retval FALSE controlled sequence not supported

**/
BOOLEAN IsManageable(
    IN UINT8 PciClass
)
{
    if(PciClass == PCI_CL_DISPLAY ||
       PciClass == PCI_CL_NETWORK ||
       PciClass == PCI_CL_MASS_STOR)
        return TRUE;

    return FALSE;
}

/**
    Implementation of Platform Drive Override protocol
    GetDriver function

          
    @param This pointer to the instance of
        the protocol
    @param ControllerHandle handle of controller for which to retrieve
        DriverBinding handle
    @param DriverImageHandle pointer to returned handle placeholder

         
    @retval EFI_SUCCESS handle returned
    @retval EFI_NOT_FOUND handle not found

**/
EFI_STATUS CsmPolicyGetDriver(
	IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
	IN EFI_HANDLE ControllerHandle,
	IN OUT EFI_HANDLE *DriverImageHandle
)
{
    EFI_STATUS Status;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINT8 PciClass;
    UINT8 Policy;

    if(ControllerHandle == NULL)
        return EFI_INVALID_PARAMETER;

    if(DriverImageHandle == NULL)
        return EFI_INVALID_PARAMETER;

    if(*DriverImageHandle == NULL) { //This is first call
        Status = pBS->HandleProtocol(ControllerHandle, &gEfiPciIoProtocolGuid, &PciIo);
        if(EFI_ERROR(Status))
            return EFI_NOT_FOUND;      //we don't care about non pci handles

        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xB, 1, &PciClass);
        if(EFI_ERROR(Status))
            return EFI_NOT_FOUND; 

        Policy = GetOpRomPolicy(PciClass);
        if(Policy == 0)                 //OpROM execution is disabled
            return EFI_NOT_FOUND;

    /* check if this pci class supported for controlled OpROM
       execution sequence
    */
        if(!IsManageable(PciClass)) 
            return EFI_NOT_FOUND;

        ThunkFirst = (Policy == 2 || Policy == 3) ? TRUE : FALSE;
    }

    if(ThunkFirst) {
        if(CurrentCount < ThunkHandles.CurrentSize) {
            *DriverImageHandle = ThunkHandles.Array[CurrentCount];
            CurrentCount++;
            return EFI_SUCCESS;
        } else {
            CurrentCount = 0;               //reset counter
            return EFI_NOT_FOUND;
        }
    } else {
/* first get handles provided by bus specific override protocol */
        if((UINTN)BusSpecific != 0xfffffffa) {
            Status = GetBusSpecificHandle(ControllerHandle, DriverImageHandle);
            if(!EFI_ERROR(Status)) {
                return Status;
            }

            BusSpecific = (EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL *)(UINTN)0xfffffffa;    //set flag we're done with bus specific override
        }
/* now get other Driver binding handles except those with thunk protocol */
        if(CurrentCount < NonThunkHandles.CurrentSize) {
            *DriverImageHandle = NonThunkHandles.Array[CurrentCount];
            CurrentCount++;
            return EFI_SUCCESS;
        } else {
            CurrentCount = 0;
            BusSpecific = NULL;             //set flag we need to start from bus specific next time we're called
        }
    }
    return EFI_NOT_FOUND;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
