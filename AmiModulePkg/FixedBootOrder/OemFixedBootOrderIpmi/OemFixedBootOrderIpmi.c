//***************************************************************************
//**                                                                       **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.           **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  
//  History
//
//  Rev. 1.01
//    Bug Fix:  Patch SMCIPMITOOL/IPMICFG can't change the boot device to UEFI USB KEY and USB Hard-drive correctly(SMCIPMITOOL/IPMICFG viloate IPMI spec definition)
//    Reason:   SMCIPMITOOL bootoption command : 
//              UEFI Hard-drive :  9 (IPMI raw command : 0xA0 0x24), refer IPMI spec, the correct raw command is 0xA0 0x08
//              UEFI USB Key    : 11 (IPMI raw command : 0xA0 0x08), refer IPMI spec, the correct raw command is 0xA0 0x24
//    Auditor:  Jimmy Chiu
//    Date:     Mar/21/2017
//
//  Rev. 1.00
//    Bug Fix:  Add more IPMI boot device option(USB, UEFI-USB) (Refer IPMI 2.0 spec)
//    Reason:   (Refer the code from Jacker)
//    Auditor:  Jimmy Chiu
//    Date:     Jan/11/2017
//
//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file OemFixedBootOrderIpmi.c

    Adjust Boot Sequence form IPMI.
    
    Update Boot Priority or BootNext form IPMI setting.


*/


#include <AmiDxeLib.h>
#include <BootOptions.h>
#include <Setup.h>
#include <../FixedBootOrder.h>
#include <DefaultFixedBootOrder.h>
#include <Token.h> //SMCPKG_SUPPORT

//
// IPMI_BOOT_DEVICE definition should be replace by include Ipmiboot.h
//
// Parameter 5 data 3 Force Boot Device Selection
//
typedef enum {
  NoOverride,
  ForcePxe,
  ForceBootHardDrive,
  ForceBootHardDriveSafeMode,
  ForceBootDiagnosticPartition,
  ForceBootCdDvd,
  ForceBootBiosSetup,
//
//Newly added boot options - START
//
  ForceBootRemoteFloppy,
  ForceBootRemoteCdDvd,
  ForceBootPrimaryRemoteMedia,
  ForceBootRemoteHardDrive = 0xB,

//
//Newly added boot options - END
//
  ForceBootFloppy = 0xF
} IPMI_BOOT_DEVICE;

//
// extern from IpmiBoot.c
//
extern  BOOLEAN             gPersistentBoot;
extern  BOOLEAN             gUefiBoot;
extern  UINT8               gDeviceInstanceSelector;
extern  IPMI_BOOT_DEVICE    gIpmiForceBootDevice;

//
// extern from FboSetOrder.c
//
extern  FBODevMap           *pFBOLegacyDevMap;
extern  FBODevMap           *pFBOUefiDevMap;
extern  UINT16  GetDevMapDataCount(FBODevMap *pFBODevMap);
extern  UINT32  GetUefiBootOptionTag(IN BOOT_OPTION *Option);
extern  UINT16  SearchDevMapByType(FBODevMap *pFBODevMap, UINT16 DevType );



/**

    @param VOID

    @retval VOID

**/
BOOT_OPTION* SearchBootOptionInBootOptionListByType(UINT16 DevType)
{
    DLINK           *Link = NULL;
    BOOT_OPTION     *Option = NULL;
#if SMCPKG_SUPPORT
    BOOLEAN         FoundDevice = FALSE;
#endif
    
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        if(IsLegacyBootOption(Option)){ // Legacy
            BBS_BBS_DEVICE_PATH *BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
            if(BbsDp->Header.Type == BBS_DEVICE_PATH &&
               BbsDp->Header.SubType == BBS_BBS_DP &&
               BbsDp->DeviceType == DevType){
#if SMCPKG_SUPPORT
                FoundDevice = TRUE;
#endif
                break;
            }
        }
        else{    // Uefi
            if(GetUefiBootOptionTag(Option) == DevType){
#if SMCPKG_SUPPORT
                FoundDevice = TRUE;
#endif
                break;
            }
        }
    }
#if SMCPKG_SUPPORT
    if(FoundDevice == FALSE){
        Option = NULL;
    }
#endif
    return Option;
}

/**

    @param VOID

    @retval VOID

**/
VOID ApplyIpmiBootRequest(){
    
    //
    // Below sample doesn't consider about FBO DualMode.
    //
    
    UINT8           TargetType = 0;
    BOOLEAN         UpdateVariable = FALSE;
    UINT16          temp = 0;
    SETUP_DATA      *pSetupData = NULL;
    UINTN           SetupSize = 0;
    UINT32          SetupAttr = 0;
    EFI_GUID        gSetupGuid=SETUP_GUID;
    UINTN           i = 0;
    EFI_STATUS      Status;
    UINTN           DeviceMapCount =0;
    BOOT_OPTION     *Option = NULL;
    BOOLEAN         IsFound = FALSE;
    // Below switch case content shoud redefine according to different group definition.
    switch(gUefiBoot){
        case 0: // Legacy Boot
            switch(gIpmiForceBootDevice){
                case NoOverride: TargetType = 0; break;
                case ForcePxe: TargetType = BoTagLegacyEmbedNetwork; break;
                case ForceBootHardDrive: TargetType = BoTagLegacyHardDisk; break;
                //case ForceBootHardDriveSafeMode: TargetType = 0; break;
                //case ForceBootDiagnosticPartition: TargetType = 0; break;
                case ForceBootCdDvd: TargetType = BoTagLegacyCdrom; break;
                //case ForceBootBiosSetup: TargetType = 0; break;
                case ForceBootRemoteFloppy: TargetType = BoTagLegacyUSBFloppy; break;
                case ForceBootRemoteCdDvd: TargetType = BoTagLegacyUSBCdrom; break;
                case ForceBootPrimaryRemoteMedia: TargetType = BoTagLegacyUSBKey; break;
                case ForceBootRemoteHardDrive: TargetType = BoTagLegacyUSBHardDisk; break;
                case ForceBootFloppy: TargetType = BoTagLegacyUSBKey; break;
                default: TargetType = 0; break;
            }
            break;
        case 1: // Uefi Boot
            switch(gIpmiForceBootDevice){
                case NoOverride: TargetType = 0; break;
                case ForcePxe: TargetType = BoTagUefiNetWork; break;
                case ForceBootHardDrive: TargetType = BoTagUefiUsbKey; break;
                //case ForceBootHardDriveSafeMode: TargetType = 0; break;
                //case ForceBootDiagnosticPartition: TargetType = 0; break;
                case ForceBootCdDvd: TargetType = BoTagUefiCdrom; break;
                //case ForceBootBiosSetup: TargetType = 0; break;
                case ForceBootRemoteFloppy: TargetType = BoTagUefiUsbFloppy; break;
                case ForceBootRemoteCdDvd: TargetType = BoTagUefiUsbCdrom; break;
                case ForceBootPrimaryRemoteMedia: TargetType = BoTagUefiHardDisk; break;
                case ForceBootRemoteHardDrive: TargetType = BoTagUefiUsbHardDisk; break;
                case ForceBootFloppy: TargetType = BoTagUefiUsbKey; break;
                default: TargetType = 0; break;
            }
            break;
        default: 
            break;
    }

    if(!TargetType) return;

    if(gPersistentBoot){   // gPersistentBoot == '1'
        //
        // Only persistent type need to update LegacyPrioities and UefiPriorities.
        //
        Status = GetEfiVariable(L"Setup", &gSetupGuid, &SetupAttr, &SetupSize, &pSetupData);
        if(EFI_ERROR(Status)){
            return;
        }  
        if(!(Option = SearchBootOptionInBootOptionListByType(TargetType))){
            return;
        }
        if(!gUefiBoot){     // Legacy Boot?
            // Update SetupData->LegacyPriorities
            //
            // Below is sample, please implement how to match options and change proper priority order.  >>>>
            //
            DeviceMapCount = GetDevMapDataCount( pFBOLegacyDevMap );
            for(i=0 ;i<DeviceMapCount;i++){

                if(pSetupData->LegacyPriorities[i] == SearchDevMapByType(pFBOLegacyDevMap,TargetType)
                        && pSetupData->LegacyPriorities[i] != DeviceMapCount){
                   
                    temp = pSetupData->LegacyPriorities[0];
                    pSetupData->LegacyPriorities[0] = pSetupData->LegacyPriorities[i];
                    pSetupData->LegacyPriorities[i] = temp;

                    pSetupData->BootMode = gUefiBoot;
                    UpdateVariable = TRUE;
                    break;
                }
            }
            //
            // Above is sample, please implement how to match options and change boot priority order.  <<<<
            //
        }
        else{               // Uefi boot
            // Update SetupData->UefiPriorities
            //
            // Below is sample, please implement how to match options and change boot priority order.  >>>>
            //
            DeviceMapCount = GetDevMapDataCount( pFBOUefiDevMap );
            for(i=0 ;i<DeviceMapCount;i++){
                if(pSetupData->UefiPriorities[i] == SearchDevMapByType(pFBOUefiDevMap,TargetType)
                        && pSetupData->UefiPriorities[i] != DeviceMapCount){
                   
                    temp = pSetupData->UefiPriorities[0];
                    pSetupData->UefiPriorities[0] = pSetupData->UefiPriorities[i];
                    pSetupData->UefiPriorities[i] = temp;
                    
                    pSetupData->BootMode = gUefiBoot;
                    UpdateVariable = TRUE;
                    break;
                }
            }
            //
            // Above is sample, please implement how to match options and change proper priority order.  <<<<
            //
        }
        if(UpdateVariable){
            Status = pRS->SetVariable(L"Setup", &gSetupGuid, SetupAttr, SetupSize, pSetupData);
        }

        if(pSetupData){
            pBS->FreePool(pSetupData);
        }
        
    }else{    // gPersistentBoot == '0'
        //
        // Use BootNext solution for Next boot only request.
        //
        if(Option = SearchBootOptionInBootOptionListByType(TargetType)){
            IsFound = TRUE;
        }
       
        if(IsFound){
            Status = pRS->SetVariable (
                         L"BootNext", 
                         &gEfiGlobalVariableGuid,
                         EFI_VARIABLE_NON_VOLATILE| EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                         sizeof(Option->BootOptionNumber), 
                         &Option->BootOptionNumber);
        }
    } // if(gPersistentBoot){
    
}
/**
    Sync BootOptionTag for Group
    IpmiBoot Set Priority and Tag of the desired bootoption to 0.
    BDS will delete the Group Header because TAG is different.
    Then BootNext doesn't work property

    @param VOID

    @retval VOID

**/
VOID
FboIpmiBootTag
(
        VOID
)
{
    DLINK           *Link = NULL, *Link1 = NULL;
    BOOT_OPTION     *Option = NULL, *Option1 = NULL;
    
    if(gUefiBoot)
        return;
    
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        // Skip Uefi Boot Option
        if(!IsLegacyBootOption(Option))
            continue;
        
        if(Option->GroupHeader)
        {
            FOR_EACH_BOOT_OPTION(BootOptionList,Link1,Option1)
            {
                // Skip Uefi BootOption and GroupHeader
                if(Option1->GroupHeader || (!IsLegacyBootOption(Option1)) )
                    continue;
                
                if(Option->BootOptionNumber == Option1->BootOptionNumber)
                    Option1->Tag = Option->Tag;
            }
        } 
    }
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
