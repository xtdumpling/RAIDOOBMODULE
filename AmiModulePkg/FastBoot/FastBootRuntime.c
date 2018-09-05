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

/** @file FastBootRuntime.c
 *  Implementation of callback when variable services is enabled.
 */

///============================================================================
/// Includes
///============================================================================
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Protocol/Variable.h>
#include "FastBoot2.h"
#include <Protocol/FastBootProtocol2.h>
#include <Protocol/AmiUsbController.h>
#include <Token.h>
#include <AMIVfr.h>
///============================================================================
/// Define
///============================================================================

///============================================================================
/// External Global Variable Declaration
///============================================================================

///============================================================================
/// External Function Definitions
///============================================================================
extern EFI_RUNTIME_SERVICES  *gRT;
extern EFI_BOOT_SERVICES     *gBS;
///============================================================================
/// Golbal Variable Declaration
///============================================================================
static EFI_GUID EfiVariableGuid = EFI_GLOBAL_VARIABLE;
EFI_GUID FastBootVariableGuid = FAST_BOOT_VARIABLE_GUID;
static EFI_GUID FastBootPolicyGuid = FAST_BOOT_POLICY_PROTOCOL2_GUID;
FAST_BOOT_POLICY2    gFastBootPolicy;
USB_SKIP_LIST DefaultSkipTable[] = USB_SKIP_TABLE;
SKIP_PCI_LIST2 DeafultSkipPciList[] = FAST_BOOT_PCI_SKIP_LIST;
#if SUPPORT_RAID_DRIVER
SATA_DEVICE_DATA    SataDeviceData[MAX_SATA_DEVICE_COUNT];
#endif
///============================================================================
/// Function Definitions
///============================================================================

#if SUPPORT_RAID_DRIVER

/**
 * Get SATA port present HOB.
 *    
 * @param[in][out]   SataPresentHob    The Point to HOB. 
 *
 * @retval EFI_SUCCESS                 Get SATA HOB success.
 * @retval EFI_NOT_FOUND               Not found SATA HOB.
 */

EFI_STATUS 
GetSataPortPresentHob (
  IN OUT SATA_PRESENT_HOB **SataPresentHob    
)
{
    EFI_STATUS  Status;
    VOID        *HobList;
    EFI_GUID    GuidHob = HOB_LIST_GUID;    
    EFI_GUID    SataPresentHobGuid = AMI_SATA_PRESENT_HOB_GUID;
    
    HobList = GetEfiConfigurationTable(gST, &GuidHob);
    if (!HobList) return EFI_NOT_READY;

    *SataPresentHob = (SATA_PRESENT_HOB*)HobList;

    while (!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, SataPresentHob)))
    {
        if (guidcmp(&(*SataPresentHob)->EfiHobGuidType.Name, &SataPresentHobGuid) == 0)
            break;
    }


    if (EFI_ERROR(Status)) return EFI_NOT_FOUND;

    return  EFI_SUCCESS;
}

/**
 * Check whether SATA Port Present status is changed.
 *    
 * @param[in]   FbVariable    Fast Boot Variable pointer. 
 *
 * @retval TRUE               SATA port present status is changed.
 * @retval FALSE              SATA port present status is NOT changed.
 */

BOOLEAN 
IsSataPortPresentChanged(
    FAST_BOOT2 *FbVariable
)
{
    SATA_PRESENT_HOB *SataPresentHob;
    EFI_STATUS  Status;
    UINT8   Counter;
    
    Status = GetSataPortPresentHob(&SataPresentHob);
    
    if (EFI_ERROR(Status) && FbVariable->ControllerCount == 0)
    {
        DEBUG((DEBUG_ERROR,"FB: Sata port present HOB is not found, need CSP porting for it\n"));
        return FALSE; ///CSP didn't report SATA port present HOB, SATA support policy would not work.
    }
   
    /// check whether SATA port present HOB is as same as previous boot totally.
    if (!MemCmp(&SataPresentHob->ControllerCount, \
            &FbVariable->ControllerCount, \
            sizeof(SATA_PRESENT_HOB) - sizeof(EFI_HOB_GUID_TYPE))) {
        return FALSE;
    }

    DEBUG((DEBUG_INFO,"FB: Sata port present is changed\n"));
    DEBUG((DEBUG_INFO,"Current: Controller count %x\n", SataPresentHob->ControllerCount));
    for (Counter = 0; Counter < 4; Counter++)
        DEBUG((DEBUG_INFO,"ClassCode %x,SataInfo %x\n", SataPresentHob->SataInfo[Counter].ClassCode, \
                SataPresentHob->SataInfo[Counter].PresentPortBitMap));
    
    DEBUG((DEBUG_INFO,"Last: Controller count %x\n", FbVariable->ControllerCount));
    for (Counter = 0; Counter < 4; Counter++)
        DEBUG((DEBUG_INFO,"ClassCode %x,SataInfo %x\n", FbVariable->SataInfo[Counter].ClassCode, \
                FbVariable->SataInfo[Counter].PresentPortBitMap));


    return TRUE;
}
#endif

/**
 * Fill default Fast Boot Policy.
 *    
 * @param[in]   SetupData                Pointer to SetupData. 
 * @param[in]   FbVariable                Pointer to FastBoot variable.
 *
 */

VOID 
SetDefaultFastBootPolicy(
    SETUP_DATA      *SetupData, 
    FAST_BOOT2      *FbVariable
)
{
    EFI_STATUS  Status;
    UINT8 *BootOption = NULL;
    UINTN Size = 0;
    AMITSESETUP AmiTseData;    
    EFI_GUID AmiTseSetupGuid = AMITSESETUP_GUID;
    UINTN    VariableSize;

    ///
    ///Information for previous boot
    ///
    gFastBootPolicy.UefiBoot = FbVariable->BootType;
    gFastBootPolicy.BootOptionNumber = FbVariable->BootOptionNumber;
    gFastBootPolicy.CheckBootOptionNumber = TRUE;
    gFastBootPolicy.DevStrCheckSum = FbVariable->DevStrCheckSum;    
    gFastBootPolicy.CheckDevStrCheckSum = TRUE;
    gFastBootPolicy.BootCount = FbVariable->BootCount;

    Status = GetEfiVariable(L"FastBootOption", &FastBootVariableGuid, NULL, &Size, &BootOption);     
    if (EFI_ERROR(Status)) {   
        return;
    }
    gFastBootPolicy.FastBootOption = (EFI_DEVICE_PATH_PROTOCOL*)BootOption;
#if SUPPORT_RAID_DRIVER
    MemCpy(SataDeviceData,FbVariable->SataDevice, sizeof(SATA_DEVICE_DATA) * MAX_SATA_DEVICE_COUNT);
    gFastBootPolicy.SataDevice = SataDeviceData;
#endif
    ///
    ///Config Behavior in fastboot path
    ///
    gFastBootPolicy.SataSupport = SetupData->FbSata;   
    gFastBootPolicy.VgaSupport = SetupData->FbVga;
    
    gFastBootPolicy.UsbSupport = SetupData->FbUsb;    
 
    gFastBootPolicy.UsbSkipTable = DefaultSkipTable;    
    gFastBootPolicy.UsbSkipTableSize = sizeof(DefaultSkipTable) / sizeof(USB_SKIP_LIST);

    gFastBootPolicy.Ps2Support = SetupData->FbPs2;
    gFastBootPolicy.NetworkStackSupport = SetupData->FbNetworkStack;
    gFastBootPolicy.RedirectionSupport = SetupData->FbRedirection;

    gFastBootPolicy.SkipPciList = DeafultSkipPciList;
    gFastBootPolicy.SkipPciListSize = sizeof(DeafultSkipPciList);
    gFastBootPolicy.SkipTseHandshake = SKIP_TSE_HANDSHAKE;
    gFastBootPolicy.FirstFastBootInS4 = ALLOW_FIRST_FASTBOOT_IN_S4;

    ///check password
    VariableSize = sizeof(AMITSESETUP);
    Status = gRT->GetVariable ( L"AMITSESetup", \
                                &AmiTseSetupGuid, \
                                NULL, \
                                &VariableSize, \
                                &AmiTseData );    

    if (!EFI_ERROR(Status)) {   
        if (AmiTseData.UserPassword[0] != 0) {
            DEBUG((DEBUG_INFO,"FB: User PW is set\n"));
            ///user password is set
            gFastBootPolicy.CheckPassword = TRUE;
        }
        
        if (AmiTseData.AdminPassword[0] != 0) {
            DEBUG((DEBUG_INFO,"FB: Admin PW is set\n"));                        
            ///Admin password is set, don't do anything now.
        }            
    }

}

/**
 * FastBoot runtime callback.
 *    
 * @param[in]   Event                The Event that is being processed.
 * @param[in]   Context            The Event Context.
 *
 */

VOID 
EFIAPI 
FastBootGetVarCallback(
    IN EFI_EVENT Event, 
    IN VOID *Context
)
{
    EFI_STATUS Status;
    UINT32 Variable = 0;
    FAST_BOOT2 FbVariable;
    UINTN Size = sizeof(SETUP_DATA);
    static EFI_GUID SetupVariableGuid = SETUP_GUID;
    SETUP_DATA SetupData;
    EFI_HANDLE  Handle = NULL;
#if LAST_BOOT_FAIL_MECHANISM    
    static UINT32 BootFlow = BOOT_FLOW_CONDITION_FIRST_BOOT;
    static EFI_GUID GuidBootFlow = BOOT_FLOW_VARIABLE_GUID;
#endif

    ///Initial fast boot policy 
    
    gBS->SetMem(&gFastBootPolicy, sizeof(FAST_BOOT_POLICY2), 0);
    
    Status=gBS->InstallProtocolInterface(
                                &Handle, 
                                &FastBootPolicyGuid, 
                                EFI_NATIVE_INTERFACE,
                                &gFastBootPolicy
                                );   
    ASSERT(!EFI_ERROR(Status));

    Status = gRT->GetVariable(L"Setup", &SetupVariableGuid, NULL, &Size, &SetupData);
    if (EFI_ERROR(Status) || SetupData.FastBoot == 0) 
        gFastBootPolicy.FastBootEnable = FALSE;
    else if (SetupData.FastBoot == 1)
        gFastBootPolicy.FastBootEnable = TRUE;

    Size = sizeof(FbVariable);
    Status = gRT->GetVariable(L"LastBoot", &FastBootVariableGuid, NULL, &Size, (VOID *)&FbVariable);
    if (EFI_ERROR(Status))
        gFastBootPolicy.LastBootVarPresence = FALSE;
    else 
        gFastBootPolicy.LastBootVarPresence = TRUE;
    
    SetDefaultFastBootPolicy(&SetupData, &FbVariable);

#if SUPPORT_RAID_DRIVER
    if (gFastBootPolicy.LastBootVarPresence == TRUE && IsSataPortPresentChanged(&FbVariable))
    {
        gRT->SetVariable(L"LastBoot", 
                         &FastBootVariableGuid,
                         EFI_VARIABLE_NON_VOLATILE,
                         0,
                         &Variable);

        gFastBootPolicy.LastBootVarPresence = FALSE;

    }
#endif

    if (gFastBootPolicy.FastBootEnable == FALSE || gFastBootPolicy.LastBootVarPresence == FALSE)
        return;    


    Size = sizeof(UINT32);
    Status = gRT->GetVariable(L"LastBootFailed", &FastBootVariableGuid, NULL, &Size, &Variable);
    if (EFI_ERROR(Status)) {
        Variable = 0x55aa55aa;
        gRT->SetVariable(L"LastBootFailed", 
                                  &FastBootVariableGuid, 
                                  EFI_VARIABLE_NON_VOLATILE |
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                  EFI_VARIABLE_RUNTIME_ACCESS,
                                  Size,
                                  &Variable);
    } else {
#if LAST_BOOT_FAIL_MECHANISM    
        ///reset LastBootFailed variable
        gRT->SetVariable(L"LastBootFailed", 
                                  &FastBootVariableGuid, 
                                  EFI_VARIABLE_NON_VOLATILE,
                                  0,
                                  &Variable);
        ///force setup
        gRT->SetVariable(L"BootFlow", 
                                  &GuidBootFlow, 
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                  sizeof(BootFlow),
                                  &BootFlow);

        gFastBootPolicy.LastBootFailure = TRUE;
#else
        ///check the fail count, if reach max count then perform a full boot
        if (Variable == 0x55aa55aa) 
            Variable = 0x01;
        else
            Variable++;

        if (Variable == MAX_LAST_BOOT_FAIL_COUNT) {

            gRT->SetVariable(L"LastBoot", 
                             &FastBootVariableGuid,
                             EFI_VARIABLE_NON_VOLATILE,
                             0,
                             &FbVariable);

            gFastBootPolicy.LastBootVarPresence = FALSE;

            Variable = 0x55aa55aa;
        }

        gRT->SetVariable(L"LastBootFailed", 
                                  &FastBootVariableGuid, 
                                  EFI_VARIABLE_NON_VOLATILE |
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                  EFI_VARIABLE_RUNTIME_ACCESS,
                                  Size,
                                  &Variable);
#endif
    }
}

/**
 * FastBoot runtime callback entry point.
 *    
 * @param[in]   ImageHandle                The firmware allocated handle for the EFI image. 
 * @param[in]   SystemTable                A pointer to the EFI System Table.
 *
 * @retval EFI_SUCCESS                    The entry point is executed successfully.
 */

EFI_STATUS 
EFIAPI 
FastBootEntry(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_EVENT Event;
    VOID *Registration;
    static EFI_GUID VariableArchProtocolGuid = EFI_VARIABLE_ARCH_PROTOCOL_GUID;
    VOID    *Protocol;
    EFI_STATUS  Status;
    
    InitAmiLib(ImageHandle,SystemTable);

    Status = gBS->LocateProtocol(&VariableArchProtocolGuid, NULL, &Protocol);
    
    if (EFI_ERROR(Status)) {
        RegisterProtocolCallback(&VariableArchProtocolGuid, FastBootGetVarCallback, NULL, &Event, &Registration);        
    } else {
        FastBootGetVarCallback(NULL, NULL);
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
