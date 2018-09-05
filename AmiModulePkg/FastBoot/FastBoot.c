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

/** @file FastBoot.c
 *  Implementation of fast boot functionality.
 */

/// ===========================================================================
/// Includes
/// ===========================================================================
#include <Token.h>
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Protocol/BlockIo.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/ConsoleControl.h>
#include <Pci.h>
#include <Protocol/AMIPostMgr.h>
#include <Protocol/FastBootProtocol2.h>
#include <Protocol/AmiUsbController.h>    ///(EIP85135+)
#include <FastBootLinks.h>
#include <Protocol/UsbPolicy.h>
#include <FastBoot2.h>

#if CSM_SUPPORT
#include <Protocol/LegacyBios.h>
#endif

#include <Protocol/DiskInfo.h> 
#include <IndustryStandard/AmiAtaAtapi.h> 

#if IdeBusSrc_SUPPORT
#include <Protocol/AmiIdeBus.h> 
#endif

#include <Protocol/AmiAhciBus.h>

#if AMIUSB_SUPPORT
#include "Usb/Rt/UsbDef.h"
#endif

#include <Protocol/SimpleFileSystem.h>
#include <GenericSio.h>


/// ===========================================================================
/// Define
/// ===========================================================================

#define COM_PORT_EISA_ID 0x50141d0

                                        ///(EIP71257+)>
#if !defined(AMITSE_SUPPORT) || (!AMITSE_SUPPORT)
    #ifndef TSE_MAJOR
        #define TSE_MAJOR 0
    #endif
    #ifndef TSE_MINOR
        #define TSE_MINOR 0
    #endif     
#endif
                                        ///<(EIP71257+)
#define AMI_MEDIA_DEVICE_PATH_GUID \
    { 0x5023b95c, 0xdb26, 0x429b, {0xa6, 0x48, 0xbd, 0x47, 0x66, 0x4c, 0x80, 0x12 }}

#define BOOT_IA32  44     ///size in bytes of string L"EFI\\BOOT\\BOOTIA32.EFI"
#define BOOT_X64   42     ///size in bytes of string L"EFI\\BOOT\\BOOTx64.EFI"
#define BOOT_FLOW_CONDITION_FAST_BOOT 7
#define BOOT_FLOW_CONDITION_NORMAL  0

#define EFI_SHELL_PROTOCOL_GUID \
  {0x47C7B223, 0xC42A, 0x11D2, {0x8E, 0x57, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}

#if defined(TSE_CAPITAL_BOOT_OPTION) && (TSE_CAPITAL_BOOT_OPTION)
#define gBootName L"Boot%04X"
#else
#define gBootName L"Boot%04x"
#endif

/// ===========================================================================
/// External Variable Declaration
/// ===========================================================================
extern EFI_RUNTIME_SERVICES  *gRT;
extern EFI_BOOT_SERVICES     *gBS;
extern EFI_SYSTEM_TABLE      *gST;
extern EFI_GUID gEfiGlobalVariableGuid;
extern EFI_GUID AmiPostMgrProtocolGuid;
extern EFI_GUID ConInStartedProtocolGuid;
extern EFI_GUID ConOutStartedProtocolGuid;
extern EFI_GUID BdsAllDriversConnectedProtocolGuid;
extern EFI_GUID gEfiPciIoProtocolGuid;
extern EFI_GUID gEfiUsbPolicyProtocolGuid;
extern EFI_HANDLE ThisImageHandle;

/// ===========================================================================
/// External Function Declaration
/// ===========================================================================
VOID 
EFIAPI 
ReportConnectConOutProgressCode();

VOID 
EFIAPI 
ReportConnectConInProgressCode();

VOID 
EFIAPI 
InstallConOutStartedProtocol();

VOID 
EFIAPI 
InstallConInStartedProtocol();

VOID 
EFIAPI 
ConnectVgaConOut();

VOID 
EFIAPI 
ConnectPs2ConIn();

VOID 
EFIAPI 
ConnectUsbConIn();

VOID 
EFIAPI 
ConnectConInVariable();

VOID 
EFIAPI 
InstallConsoleStartedProtocol(
    CHAR16*   ConDevVar, 
    EFI_GUID* ProtocolGuid
);

VOID 
EFIAPI 
ReadyToBoot(UINT16 OptionNumber);

VOID 
EFIAPI 
ConnectDevicePath(
    IN EFI_DEVICE_PATH_PROTOCOL *Path
);

VOID 
EFIAPI 
InstallFwLoadFile(VOID);

EFI_DEVICE_PATH_PROTOCOL* 
EFIAPI 
DiscoverPartition(
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
);

#ifdef EFI_DXE_PERFORMANCE
VOID 
EFIAPI 
SavePerformanceData(
    IN EFI_EVENT Event, 
    IN VOID *Context
);
#endif

extern VOID 
EFIAPI 
RecoverTheMemoryAbove4Gb();

extern VOID 
EFIAPI 
CallTheDispatcher();

/// ===========================================================================
/// Golbal Variable Declaration
/// ===========================================================================
struct {
    EFI_DEVICE_PATH_PROTOCOL Header;
    CHAR16 FileName[22];
} FilePathNode = {
    {
        MEDIA_DEVICE_PATH,
        MEDIA_FILEPATH_DP,
#ifdef EFIx64
        BOOT_X64 + 4
#else
        BOOT_IA32 + 4
#endif
    },
#ifdef EFIx64
    { L"EFI\\BOOT\\BOOTx64.EFI"}
#else
    { L"EFI\\BOOT\\BOOTIA32.EFI" }
#endif
};

static EFI_GUID AmiMediaDevicePathGuid = AMI_MEDIA_DEVICE_PATH_GUID;
EFI_GUID FastBootVariableGuid = FAST_BOOT_VARIABLE_GUID;
static EFI_GUID GuidBootFlow = BOOT_FLOW_VARIABLE_GUID;
static EFI_GUID gLocalEfiHiiDatabaseProtocolGuid = EFI_HII_DATABASE_PROTOCOL_GUID;

#if FASTBOOT_NEED_RESTART
static EFI_GUID FastBootRestartGuid = FAST_BOOT_RESTART_GUID;
static EFI_GUID FastBootRestartCountGuid = FAST_BOOT_RESTART_COUNT_GUID;
#endif
FAST_BOOT_TSE_PROTOCOL2 *gFastBootTseProtocol=NULL;  ///(EIP63924+)
EFI_HANDLE      EfiBootImageHanlde = NULL;
EFI_HANDLE      LegacyBootDeviceHandle = NULL;
EFI_HANDLE      *RootHandles;
UINTN           NumberOfHandles;
SETUP_DATA      FbSetupData;
static BOOLEAN  Runtime = FALSE;
FAST_BOOT_POLICY2    *gFastBootPolicy;
BOOLEAN FbUsbSkipTableIsSet = FALSE;
UINT8   *BackupSkipTable = NULL;
EFI_USB_POLICY_PROTOCOL *gUsbPolicyProtocol = NULL;
BOOLEAN BackupUsbMassDriverSupport;

BOOLEAN    StopBlkIo = FALSE;
EFI_HANDLE gHandle = NULL;
EFI_HII_HANDLE  gHiiHandle = NULL;

static AMI_FAST_BOOT_PROTOCOL2 FastBootProtocol = { 
    NULL, 
    FbConnectInputDevices,
    IsRuntime
};

/// ===========================================================================
/// Function Definitions
/// ===========================================================================
BOOLEAN CapsulePresent(VOID);
EFI_STATUS ConnectFastEfiBootDevice();
EFI_STATUS ConnectFastLegacyBootDevice();
EFI_STATUS ConnectAllSataDevices();
EFI_STATUS ConnectHddDevices();
EFI_STATUS ConnectLastHddDevice();

///============================================================================
/// Elinks and Hooks
///============================================================================
typedef BOOLEAN (IsFastBootElink)(
    IN SETUP_DATA *Setupdata
);
extern IsFastBootElink IS_FAST_BOOT_LIST EndOfIsFastBootList;
IsFastBootElink* IsFastBootList[] = {IS_FAST_BOOT_LIST NULL};
                                        ///(EIP62683+)>
typedef VOID (AfterAllDrirverConnectElink)();
extern AfterAllDrirverConnectElink AFTER_ALL_DRIVER_CONNECT_HOOK EndOfList;
AfterAllDrirverConnectElink* AfterAllDriverConnectList[] = {AFTER_ALL_DRIVER_CONNECT_HOOK NULL};
                                        ///<(EIP62683+)
                                        ///(EIP63924+)>
typedef BOOLEAN (FastBootCheckModeChangeElink)();
extern FastBootCheckModeChangeElink FAST_BOOT_CHECK_MODE_CHANGE_HOOK EndOfFastBootModeChangeList;
FastBootCheckModeChangeElink* FastBootModeChange[] = {FAST_BOOT_CHECK_MODE_CHANGE_HOOK NULL};
                                        ///<(EIP63924+)
                                        ///<(EIP62845+)
typedef VOID (BeforeConnectFastBootDeviceElink)();    ///(EIP85135+)
extern BeforeConnectFastBootDeviceElink BEFORE_CONNECT_FAST_BOOT_DEVICE_HOOK EndOfBeforeConnectFastBootDeviceElink;
BeforeConnectFastBootDeviceElink* BeforeConnectFastBootDeviceHook[] = {BEFORE_CONNECT_FAST_BOOT_DEVICE_HOOK NULL};
                                        ///<(EIP62845+)
typedef VOID (ReturnNormalModeElink)();
extern ReturnNormalModeElink RETURN_NORMAL_MODE_HOOK EndOfReturnNormalModeElink;
ReturnNormalModeElink* ReturnNormalModeHook[] = {RETURN_NORMAL_MODE_HOOK NULL};

typedef EFI_STATUS (FastBootFlowListElink)();
extern FastBootFlowListElink FASTBOOT_FLOW_HOOK EndOfFastBootFlowListElink;
FastBootFlowListElink* FastBootFlowList[] = {FASTBOOT_FLOW_HOOK NULL};

CHAR8 *FastBootFlowListNames[] = {
        FASTBOOT_FLOW_HOOK_NAMES NULL
};

extern IS_VALID_FASTBOOT_BOOT_OPTION_FUNC_PTR  IS_VALID_FASTBOOT_BOOT_OPTION_FUNC;
IS_VALID_FASTBOOT_BOOT_OPTION_FUNC_PTR *IsValidFbBootOptionPtr = IS_VALID_FASTBOOT_BOOT_OPTION_FUNC;
#if FASTBOOT_NEED_RESTART
#ifndef AMITSE_EVENT_BEFORE_BOOT_GUID
#define AMITSE_EVENT_BEFORE_BOOT_GUID \
  { 0x3677770f, 0xefb2, 0x43b2, {0xb8, 0xae, 0xb3, 0x2, 0xe9, 0x60, 0x48, 0x82 }}
#endif

#define EFI_AMI_LEGACYBOOT_PROTOCOL_GUID            \
  {0x120d28aa, 0x6630, 0x46f0, {0x81, 0x57, 0xc0, 0xad, 0xc2, 0x38, 0x3b, 0xf5}};

EFI_GUID    FbBeforeBootProtocolGuid = AMITSE_EVENT_BEFORE_BOOT_GUID;
EFI_GUID    FbBeforeLegacyBootProtocolGuid = EFI_AMI_LEGACYBOOT_PROTOCOL_GUID;
#endif

///============================================================================
/// Procedures
///============================================================================

///<EIP118901+> 2013/06/21 >>>

/**
 *  Load string.
 *    
 */

VOID 
BdsLoadStrings()
{
    LoadStrings(TheImageHandle, &gHiiHandle);
}

/**
 *  Remove exist string when back to normal boot.
 *    
 */

EFI_STATUS 
RemoveStrings(
    EFI_HANDLE          ImageHandle, 
    EFI_HII_HANDLE      HiiHandle
)
{
    EFI_STATUS                      Status;
    EFI_HII_DATABASE_PROTOCOL       *HiiDatabase;
      
    Status = gBS->LocateProtocol (
        &gLocalEfiHiiDatabaseProtocolGuid,
        NULL,
        &HiiDatabase
    );
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    //
    // Remove HII package list to HII Database.
    //
    Status = HiiDatabase->RemovePackageList (
        HiiDatabase,
        HiiHandle
    );

    return Status;
}


/**
 *  Remove string.
 *    
 */

VOID 
EFIAPI 
BdsRemoveStrings()
{
    RemoveStrings(TheImageHandle, gHiiHandle);
}

///<EIP118901+> 2013/06/21 >>>

/**
 *  Makes a new device path with Dp1 as its first instance and Dp2 as its
 *     second instance, then returns the result in a newly allocated buffer.
 *    
 * @param[in]   Dp1                The first instance.
 * @param[in]   Dp2                The second instance.
 *
 * @retval     Dp2                     The new device path that includes Dp1 and DP2.
 */


EFI_DEVICE_PATH_PROTOCOL* 
FbAddDevicePath (
    IN EFI_DEVICE_PATH_PROTOCOL *Dp1, 
    IN EFI_DEVICE_PATH_PROTOCOL *Dp2 
)
{
    if (!Dp2) return Dp1;
    if (!Dp1)
    {
        return DPCopy(Dp2);
    }
    else
    {
        Dp2 = DPAddInstance(Dp1,Dp2);
        gBS->FreePool(Dp1);
        return Dp2;
    }
} 

/**
 *  Get PCI IO handle list that Calss code is match with input parameter 
 *     Class and SubClass.
 *    
 * @param[in]           Class                        Pci class code.
 * @param[in]           SubClass                    Pci sub class code.
 * @param[out]         NumberOfHandles    The number of handles that match the indicated class code.
 * @param[out]         HandleBuffer            The handle buffer of PCI IO handle.
 *
 * @retval     EFI_SUCCESS                         Get Pci handle and numbers.
 * @retval    EFI_NOT_FOUND                Can't get Pci handle and numbers.
 */

EFI_STATUS 
FbGetPciHandlesByClass (
    IN  UINT8         Class, 
    IN  UINT8         SubClass, 
    OUT UINTN         *NumberOfHandles, 
    OUT EFI_HANDLE    **HandleBuffer 
)
{
    EFI_STATUS Status;
    EFI_HANDLE *Handle;
    UINTN Number, Counter;

    if (!NumberOfHandles || !HandleBuffer) return EFI_INVALID_PARAMETER;
    ///Get a list of all PCI devices
    Status = gBS->LocateHandleBuffer(
        ByProtocol,&gEfiPciIoProtocolGuid, NULL, &Number, &Handle
    );
    if (EFI_ERROR(Status)) return Status;
    *NumberOfHandles = 0;
    for (Counter = 0; Counter < Number; Counter++)
    {
        EFI_PCI_IO_PROTOCOL *PciIo;
        UINT8 PciClass[4];
        
        Status=gBS->HandleProtocol(Handle[Counter], &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
        if (EFI_ERROR(Status)) continue;
        Status=PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_REV_ID_OFFSET, 1, &PciClass);
        if ( PciClass[3]==Class && PciClass[2]==SubClass)
            Handle[(*NumberOfHandles)++] = Handle[Counter];
    }
    if (*NumberOfHandles == 0){
        gBS->FreePool(Handle);
        return EFI_NOT_FOUND;
    }
    *HandleBuffer = Handle;
    return EFI_SUCCESS;
}

#if CSM_SUPPORT

/**
 *  Check and install pci oprom for VGA device.
 *    
 * @param[in]           HdlNum                    Number of Pci Io handle.
 * @param[in]           Handle                    Pci Io handle list.
 * @param[out]         LegacyBios                EFI_LEGACY_BIOS_PROTOCOL pointer.
 * @param[out]         OnBoard                    OnBoard (TRUE) or OffBoard (FALSE)
 *
 * @retval     TRUE                 Legacy Oprom is installed successfully.
 * @retval    FALSE                 Oprom is installed failures.
 */

BOOLEAN 
FbInstallOnBoardVgaOpRom (
    IN UINTN                      HdlNum,
    IN EFI_HANDLE                 *Handle,
    IN EFI_LEGACY_BIOS_PROTOCOL   *LegacyBios, 
    IN BOOLEAN                    OnBoard 
)
{
    UINTN   Counter;
    EFI_STATUS  Status;
    UINT64 PciAttributes;    
    UINTN   Flags;
    UINT64  Capabilities;    
    
    for (Counter=0; Counter < HdlNum; Counter++)    {
        EFI_PCI_IO_PROTOCOL *PciIo;
        EFI_DEVICE_PATH_PROTOCOL *Dp;
        UINT8 PciClass;
        
        Status = gBS->HandleProtocol(Handle[Counter], &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
        if (EFI_ERROR(Status)) continue;
        
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xB, 1, &PciClass);
        if (EFI_ERROR(Status)) continue;        
        if (PciClass != PCI_CL_DISPLAY) continue;
        
        Status = gBS->HandleProtocol(Handle[Counter], &gEfiDevicePathProtocolGuid, (VOID**)&Dp); 
        if (EFI_ERROR(Status)) continue;
        
        Status = PciIo->Attributes(
            PciIo, EfiPciIoAttributeOperationGet, 0, &PciAttributes
        );

        if ( !EFI_ERROR(Status) && (PciAttributes & EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE) && OnBoard)        
            continue;

                                        ///(EIP75718)>
        Status = PciIo->Attributes (PciIo,
                                    EfiPciIoAttributeOperationSupported, 0,
                                    &Capabilities);     /// Get device capabilities
        if (EFI_ERROR(Status)) continue;

        Status = PciIo->Attributes (
                        PciIo,
                        EfiPciIoAttributeOperationEnable,
                        ///Capabilities & EFI_PCI_DEVICE_ENABLE,
                        Capabilities & (EFI_PCI_DEVICE_ENABLE | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO),
                        NULL);              /// Enable device
        if (EFI_ERROR(Status)) goto done; 
              

        Status = LegacyBios->CheckPciRom ( LegacyBios,
                                           Handle[Counter],
                                           NULL,
                                           NULL,
                                           &Flags);
        if (EFI_ERROR(Status) || (Flags != 2)) goto done;
               
        Status = LegacyBios->InstallPciRom (
                              LegacyBios,
                              Handle[Counter],
                              NULL,
                              &Flags,
                              NULL,
                              NULL,
                              NULL,
                              NULL
                              );

done:        
        if (!EFI_ERROR(Status)) return TRUE;
        else {
            
            if (PciIo != NULL) {     
            ///
            /// Turn off the PCI device and disable forwarding of VGA cycles to this device
            ///
            PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationDisable,
                    Capabilities & EFI_PCI_DEVICE_ENABLE | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
                    NULL);
            }
        }
                                        ///<(EIP75718)
    }
    return FALSE;
}
#endif

/**
 *  Install and exectued VGA legacy Oprom. If Legacy Vga OpRom is not 
 *     installed successfully, then try to connect EFI VGA driver.
 */

VOID 
FbInstallVgaOpRom()
{
#if CSM_SUPPORT
    EFI_STATUS Status;
    EFI_HANDLE *Handle;
    UINTN Number;
    BOOLEAN VgaInstalled = FALSE;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios;

    ///Locate LegacyBios Protocol
    Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);    
    if (EFI_ERROR(Status)) return ;

    ///Get a list of all PCI devices
    Status = gBS->LocateHandleBuffer(
        ByProtocol,&gEfiPciIoProtocolGuid, NULL, &Number, &Handle
    );
    if (EFI_ERROR(Status)) return;

    VgaInstalled = FbInstallOnBoardVgaOpRom(Number,Handle,LegacyBios,FALSE);
    if (VgaInstalled == FALSE)    
        VgaInstalled = FbInstallOnBoardVgaOpRom(Number,Handle,LegacyBios,TRUE);        

    gBS->FreePool(Handle);

    ///If Legacy Vga OpRom is not installed successfully, then try to connect EFI VGA driver
    if (VgaInstalled == FALSE)    
         ConnectVgaConOut();    
#endif    
}
                                        ///(EIP85135+)>
/**
 *  Check whether this handle is bridge handle or not.
 *    
 * @param[in]           Handle        Handle that want to check.
 *
 * @retval     TRUE                      The input handle is pci root bridge handle.
 * @retval    FALSE                      The input handle is not pci root bridge handle.
 */

BOOLEAN 
IsRootBridgeHandle (
    IN EFI_HANDLE       Handle 
)
{
    EFI_STATUS  Status;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo;
    EFI_GUID PciRootBridgeIoProtocolGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;

    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&DevicePath
                    );
    if (EFI_ERROR(Status)) return FALSE;

    Status = gBS->HandleProtocol (
                    Handle,
                    &PciRootBridgeIoProtocolGuid,
                    (VOID**)&PciRootBridgeIo
                    );
    if (EFI_ERROR(Status)) return FALSE;

    return TRUE;

}

/**
 *  This function connects all PCI handles excpet PCI devices in 
 *  FAST_BOOT_PCI_SKIP_LIST.
 *    
 */

VOID 
ConnectEveryPciHandles()
{
    EFI_STATUS  Status;
    UINTN Number;
    EFI_HANDLE *Handle;    
    UINTN HandleCounter, SkipTableCounter;  
    SKIP_PCI_LIST2 *SkipPciList = gFastBootPolicy->SkipPciList;

    Status = gBS->LocateHandleBuffer(
        ByProtocol, &gEfiPciIoProtocolGuid, NULL, &Number, &Handle
    );
    if (EFI_ERROR(Status)) return;

     for ( HandleCounter = 0; HandleCounter < Number; HandleCounter++)
    {
        EFI_PCI_IO_PROTOCOL *PciIo;
        UINT8 PciClass[4];
        Status = gBS->HandleProtocol( Handle[HandleCounter], &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
        if (EFI_ERROR(Status)) continue;
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_REV_ID_OFFSET, 1, &PciClass);

        ///check skip table
        for (SkipTableCounter = 0; SkipTableCounter < (gFastBootPolicy->SkipPciListSize / (sizeof(SKIP_PCI_LIST2))); SkipTableCounter++) {

            if (SkipPciList[SkipTableCounter].SubClass == 0xFF && PciClass[3] == SkipPciList[SkipTableCounter].Class) 
                break;
                
            if (PciClass[3] == SkipPciList[SkipTableCounter].Class && PciClass[2] == SkipPciList[SkipTableCounter].SubClass)
                break;
        }

        if ( SkipTableCounter < (gFastBootPolicy->SkipPciListSize / (sizeof(SKIP_PCI_LIST2))))
            continue;
                
        ///check fastboot policy
        if (gFastBootPolicy->UsbSupport == 0 && PciClass[3] == PCI_CL_SER_BUS && PciClass[2] == PCI_CL_SER_BUS_SCL_USB)
            continue;    

        if (PciClass[3] == PCI_CL_NETWORK && gFastBootPolicy->NetworkStackSupport == 0) {
            gBS->ConnectController(Handle[HandleCounter], NULL, NULL, FALSE);
            continue;
        }
        
        ///all pass, then we connect this controller
        gBS->ConnectController(Handle[HandleCounter], NULL, NULL, TRUE);        
                
    }

    gBS->FreePool(Handle);      
}

/**
 *  This function connects handles in the system.
 *    
 */

VOID 
EFIAPI 
FbConnectEverything()
{
    UINTN HandleCounter;
       
    for (HandleCounter = 0; HandleCounter < NumberOfHandles; HandleCounter++) {
        if (IsRootBridgeHandle(RootHandles[HandleCounter]))
            ConnectEveryPciHandles();
        else     
            gBS->ConnectController(RootHandles[HandleCounter], NULL, NULL, TRUE);        
    }

    gBS->FreePool(RootHandles);

/// Signal it anyway for Consplitter to take care the ConIn/ConOut 
/// after everything is connected
    InstallConsoleStartedProtocol(NULL, &ConOutStartedProtocolGuid);
    InstallConsoleStartedProtocol(NULL, &ConInStartedProtocolGuid);
    
}

/**
 *  This procedure is executed before elink BDS_CONTROL_FLOW for getting all 
 *     system handles.
 */

VOID 
EFIAPI 
BeforeBdsFlow()
{
    gBS->LocateHandleBuffer(AllHandles, NULL, NULL, &NumberOfHandles, &RootHandles);
}
                                        ///<(EIP85135+)

                                        ///(EIP85135+)>

/**
 *  Skip certain usb port in fastboot path by setting skip table in usb 
 *     protocol. 
 */

#if AMIUSB_SUPPORT
VOID  
SetUsbSkipTable()
{
    EFI_STATUS Status;
    EFI_GUID EfiUsbProtocolGuid = EFI_USB_PROTOCOL_GUID;    
    EFI_USB_PROTOCOL *UsbProtocol = NULL;    
    USB_GLOBAL_DATA *UsbData = NULL;

    Status = gBS->LocateProtocol( &EfiUsbProtocolGuid, \
                                      NULL, \
                                      (VOID**)&UsbProtocol );
    if (EFI_ERROR(Status)) return;

///backup skip table pointer
    UsbData = (USB_GLOBAL_DATA*)UsbProtocol->USBDataPtr;

    BackupSkipTable = UsbData->gUsbSkipListTable;

///Set new skip table    
    UsbProtocol->UsbCopySkipTable( \
            (USB_SKIP_LIST*)gFastBootPolicy->UsbSkipTable, \
                     (UINT8)gFastBootPolicy->UsbSkipTableSize);  

}   
#endif
                                        ///<(EIP85135+)
                                        ///(EIP63924)>
#if PS2MOUSE_SUPPORT && PS2KBD_SUPPORT 

/**
 *  Change Key60_64 and Ps2Mouse value in variable SIO_DEV_STATUS_VAR
 *     that let ps2 device asl code return 0xf.
 */

VOID 
ChangePs2VarValue()
{

    UINTN SioDevStatusVarSize = sizeof(SIO_DEV_STATUS);
    EFI_GUID SioDevStatusVarGuid = SIO_DEV_STATUS_VAR_GUID;
    EFI_STATUS  Status;
    SIO_DEV_STATUS      SioDevStatus;
    UINT32              SioDevStatusVarAttributes = 0;
    
    Status = gRT->GetVariable(SIO_DEV_STATUS_VAR_NAME, &SioDevStatusVarGuid, NULL,
                              &SioDevStatusVarSize, &SioDevStatus);
    if (!EFI_ERROR(Status)) {
        SioDevStatus.Key60_64 = 1;
        SioDevStatus.Ps2Mouse = 1;
        SioDevStatusVarAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;

        Status = gRT->SetVariable(  SIO_DEV_STATUS_VAR_NAME, 
                                    &SioDevStatusVarGuid, 
                                    SioDevStatusVarAttributes,
                                    SioDevStatusVarSize, 
                                    &SioDevStatus);
    }
    
}
#endif
/**
    This function will locate PCI serial handles and connect them to 
    controller.

    @param  None
         
    @retval None
**/
VOID 
FbConnectSerialOffBoard()
{
    EFI_STATUS Status;
    EFI_HANDLE *Handle;
    UINTN      Number;
    UINT32     HandleCount;
    
    // Gets all PCI serial handles
    Status = FbGetPciHandlesByClass(
             PCI_CL_COMM, PCI_CL_COMM_CSL_SERIAL, &Number, &Handle);
    if (EFI_ERROR(Status)) {
        return;
    }
    
    for(HandleCount=0; HandleCount < Number; HandleCount++) {
        EFI_DEVICE_PATH_PROTOCOL *Dp;
        
        Status = gBS->HandleProtocol( Handle[HandleCount],
                                      &gEfiDevicePathProtocolGuid,
                                      &Dp );
        if (EFI_ERROR(Status)) {
            continue;
        }

        gBS->ConnectController(Handle[HandleCount], NULL, NULL, TRUE);
        ConnectDevicePath(Dp);
    }
    gBS->FreePool(Handle);
}

/**
    This function will locate all UART handles and connect them to 
    controller.

    @param  None
         
    @retval None
**/
VOID 
FbConnectSerialOnBoard()
{
    EFI_STATUS Status;
    EFI_HANDLE *Handle;
    UINTN      Number;
    UINTN      HandleCount;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    ACPI_HID_DEVICE_PATH     *Serial;
    
    Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiSioProtocolGuid, 
                                      NULL, 
                                      &Number, 
                                      &Handle );
    if (EFI_ERROR(Status)) {
        return;
    }
    
    for(HandleCount=0; HandleCount < Number; HandleCount++) {
        Status = gBS->HandleProtocol( Handle[HandleCount],
                                      &gEfiDevicePathProtocolGuid,
                                      &Dp );
        if (EFI_ERROR(Status)) {
            continue;
        }

        // Traversing between node to find ACPI Device Path node
        for ( ; !(IsDevicePathEnd(Dp)); Dp = NextDevicePathNode(Dp)) {
            if( Dp->Type == ACPI_DEVICE_PATH ) {
                Serial = (ACPI_HID_DEVICE_PATH *) Dp;
                if (Serial->HID == COM_PORT_EISA_ID) {
                    gBS->ConnectController(Handle[HandleCount], NULL, NULL, TRUE);
                    ConnectDevicePath(Dp);
                    break;
                }
            }
        }
    }
    gBS->FreePool(Handle);
}

/**
 *  Connect console in and console out device in fast boot path.
 *    
 */

VOID 
FastConnectConsoles()
{    
    DEBUG((DEBUG_INFO,"FB:Connect Console...\n"));

    /// Connect Console Out
    ReportConnectConOutProgressCode();              

    PERF_START(0, "ConnectVgaConOut", "FB", 0);  
    if (gFastBootPolicy->VgaSupport || gFastBootPolicy->UefiBoot == TRUE){  
        DEBUG((DEBUG_INFO,"FB:Connect Vga\n"));    
        ConnectVgaConOut();          
    } else {
        DEBUG((DEBUG_INFO,"FB:Install Vga OpRom Only\n"));            
        FbInstallVgaOpRom();        
    }
        
    PERF_END(0,"ConnectVgaConOut","FB",0);  
    PERF_START(0,"InstallConOutStartedProtocol","FB",0);          
    InstallConOutStartedProtocol();               
    PERF_END(0,"InstallConOutStartedProtocol","FB",0);  

    /// Connect Console In
    ReportConnectConInProgressCode();
    PERF_START(0, "ConnectUsbConIn", "FB" , 0);   
#if AMIUSB_SUPPORT                
                                        ///(EIP85135)>
    if (gFastBootPolicy->UsbSupport >= 1){
                       
        if (gFastBootPolicy->UsbSupport == 2){  

            ///Disable Usb storage driver support
            gBS->LocateProtocol(&gEfiUsbPolicyProtocolGuid, NULL, (VOID**)&gUsbPolicyProtocol);
            if (gUsbPolicyProtocol != NULL) {
                BackupUsbMassDriverSupport = gUsbPolicyProtocol->UsbDevPlcy->UsbMassDriverSupport;
#if SKIP_USB_STORAGE
                gUsbPolicyProtocol->UsbDevPlcy->UsbMassDriverSupport = FALSE;
#endif
            }
            
            DEBUG((DEBUG_INFO,"FB:Set Usb Skip Table\n"));           
            FbUsbSkipTableIsSet = TRUE;
            SetUsbSkipTable();
        }
        DEBUG((DEBUG_INFO,"FB:Connect Usb\n")); 
        ConnectUsbConIn();
    } 
#else 
    ConnectUsbConIn(); /// If AMIUSB_SUPPORT disable, always connect USB device
#endif  
    PERF_END(0, "ConnectUsbConIn", "FB", 0);           
                                        ///<(EIP85135)
    PERF_START(0, "ConnectPs2ConIn", "FB", 0);                                         
    if (gFastBootPolicy->Ps2Support){
        DEBUG((DEBUG_INFO,"FB:Connect Ps2\n"));                        
        ConnectPs2ConIn();
    }    
    PERF_END(0, "ConnectPs2ConIn", "FB", 0); 
    
    PERF_START(0, "ConnectSerialController","FB", 0); 
    if (gFastBootPolicy->RedirectionSupport){                       
        FbConnectSerialOffBoard();            
        FbConnectSerialOnBoard();            
    }
    PERF_END(0, "ConnectSerialController", "FB", 0);
    
    PERF_START(0, "InstallConInStartedProtocol","FB", 0);    
    InstallConInStartedProtocol();
    PERF_END(0, "InstallConInStartedProtocol", "FB", 0);        
    DEBUG((DEBUG_INFO,"FB:Connect Console...End\n"));
}
                                        ///<(EIP63924)
#if FASTBOOT_NEED_RESTART

/**
 * Reset system.
 *    
 * @param[in]   Event                The Event that is being processed.
 * @param[in]   Context            The Event Context.
 *
 */

VOID 
EFIAPI 
ResetForFastboot (
    IN EFI_EVENT        Event,
    IN VOID             *Context
)
{
    gRT->ResetSystem(EfiResetCold, 0, 0, NULL);
}

/**
 * Set reset flag on and create callback before boot to reset system.
 *    
 * @param[in]   Flag                Flag to device do reset or not.
 * @param[in]   Count                Reset system counter.
 *
 */

FastBootEnableRestart(
    IN UINTN Flag,
    IN UINTN Count 
)
{
    EFI_STATUS Status;
    EFI_EVENT FunctionEvent;
    VOID *Registration;
                                        
    DEBUG((DEBUG_INFO,"FB:Normal boot need to restart.\n"));
    RegisterProtocolCallback(
                &FbBeforeBootProtocolGuid,
                ResetForFastboot,
                NULL, &FunctionEvent, &Registration
                );
    RegisterProtocolCallback(
                &FbBeforeLegacyBootProtocolGuid,
                ResetForFastboot,
                NULL, &FunctionEvent, &Registration
                );
                                        
    Status = gRT->SetVariable(FAST_BOOT_RESTART, 
                &FastBootRestartGuid, 
                EFI_VARIABLE_NON_VOLATILE |
                EFI_VARIABLE_BOOTSERVICE_ACCESS |
                EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof(Flag),
                &Flag);
    Status = gRT->SetVariable(FAST_BOOT_RESTART_COUNT, 
                &FastBootRestartCountGuid, 
                EFI_VARIABLE_NON_VOLATILE |
                EFI_VARIABLE_BOOTSERVICE_ACCESS |
                EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof(Count),
                &Count);        
}
#endif
                                        ///<(EIP63924)

/**
 *  FastBoot entry point.
 *    
 */

VOID 
EFIAPI 
FastBoot(VOID)
{
                                        ///(EIP63924+)>
    EFI_STATUS Status;
    EFI_GUID    FastBootTseGuid = FAST_BOOT_TSE_PROTOCOL2_GUID;    
    UINT16 Counter;
    
    Status = gBS->LocateProtocol(&FastBootTseGuid, NULL, (VOID**)&gFastBootTseProtocol);
    if (EFI_ERROR(Status)) return;
                                        ///<(EIP63924+)
    gFastBootPolicy->InFastBootPath = TRUE;        
    
    /// Run Fastboot.
    for (Counter = 0; FastBootFlowList[Counter]; Counter++) {
        Status = FastBootFlowList[Counter](); 
        if (EFI_ERROR(Status)) {
            DEBUG((DEBUG_INFO, "FastBootFlowList.%s (%p)= %r\n",
                FastBootFlowListNames[Counter], FastBootFlowList[Counter], Status ));
            break;
        } else {
            if (FastBootFlowListNames[Counter] != NULL) {
                DEBUG((DEBUG_INFO, "FastBootFlowList.%s (%p)= %r\n",
                        FastBootFlowListNames[Counter], FastBootFlowList[Counter], Status ));
            }
        }
    }

    /// if we're here - fast boot failed
    ReturnToNormalBoot();
    gFastBootPolicy->InFastBootPath = FALSE;                                        
    DEBUG((DEBUG_INFO,"FB: FastBoot Failure, return to BDS\n"));
}

/**
 *  Output the debug message to show current fast boot plicy setting.
 *     
 */

VOID 
EFIAPI 
ShowFastBootPolicy()
{
    EFI_DEVICE_PATH_PROTOCOL *Dp = gFastBootPolicy->FastBootOption;    
    UINTN   FastBootOptionSize;
    VOID    *LoadOptions;
    UINT32  LoadOptionsSize = 0;
    UINT8   *Ptr8 = NULL;  
    SKIP_PCI_LIST2 *PciSkipList = NULL;
#if SUPPORT_RAID_DRIVER    
    SATA_DEVICE_DATA *SataDevice = NULL;
#endif
    UINTN   Counter1, Counter2;
    USB_SKIP_LIST  *UsbSkip = NULL; ///(EIP96276.7)+

    DEBUG((DEBUG_INFO,"FB: ==================================================\n"));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->FastBootEnable = %x\n",gFastBootPolicy->FastBootEnable));

    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->UefiBoot = %x\n",gFastBootPolicy->UefiBoot));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->BootOptionNumber = %x\n",gFastBootPolicy->BootOptionNumber));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->DevStrCheckSum = %x\n",gFastBootPolicy->DevStrCheckSum));

    FastBootOptionSize = DPLength(Dp);
    if (gFastBootPolicy->UefiBoot) {
        LoadOptions = (UINT8*)Dp + DPLength(Dp);
        LoadOptionsSize = *(UINT32*)LoadOptions;  
        FastBootOptionSize += LoadOptionsSize+sizeof(UINT32);
    }

    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->FastBootOption\n"));         
    Ptr8 = (UINT8*)Dp;
    for (Counter1 = 0; Counter1 < FastBootOptionSize;)
    {
        for (Counter2 = 0; Counter2 < 0x10 && Counter1 < FastBootOptionSize; Counter2++)
        {
            DEBUG((DEBUG_INFO,"%02X ",Ptr8[Counter1]));
            Counter1++;
        }
        DEBUG((DEBUG_INFO," \n"));
    }
           
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->LastBootFailure = %x\n",gFastBootPolicy->LastBootFailure));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->LastBootVarPresence = %x\n",gFastBootPolicy->LastBootVarPresence));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->BootCount = %x\n",gFastBootPolicy->BootCount));

#if SUPPORT_RAID_DRIVER
    SataDevice = gFastBootPolicy->SataDevice;
    for (Counter1 = 0; Counter1 < MAX_SATA_DEVICE_COUNT && SataDevice[Counter1].Bdf != 0; Counter1++)
    {
     
        DEBUG((DEBUG_INFO,"FB: FastBootPolicy->SataDevice[%d],n",Counter1));
    
        DEBUG((DEBUG_INFO,"BDF:%x, CalssCode:%x, DveType:%x, PortNum:%x\n",\
            SataDevice[Counter1].Bdf,\
            SataDevice[Counter1].ClassCode,\
            SataDevice[Counter1].DevType,\
            SataDevice[Counter1].PortNum));  
        
        DEBUG((DEBUG_INFO,"Serial Number: "));
    
        for (Counter2 = 0; Counter2 < 20; Counter2++)
        {
            DEBUG((DEBUG_INFO,"%02X ",SataDevice[Counter1].SerialNum[Counter2]));
        }
        DEBUG((DEBUG_INFO," \n"));
    }
#endif

    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->CheckBootOptionNumber = %x\n",gFastBootPolicy->CheckBootOptionNumber));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->CheckDevStrCheckSum = %x\n",gFastBootPolicy->CheckDevStrCheckSum));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->SataSupport = %x\n",gFastBootPolicy->SataSupport));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->VgaSupport = %x\n",gFastBootPolicy->VgaSupport));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->UsbSupport = %x\n",gFastBootPolicy->UsbSupport));

                                        //(EIP96276.7)>
    UsbSkip = gFastBootPolicy->UsbSkipTable;
    for (Counter1 = 0; Counter1 < gFastBootPolicy->UsbSkipTableSize; Counter1++)
    {
        DEBUG((DEBUG_INFO,"FB: FastBootPolicy->UsbSkipTable[%d] = {%02X,%02X,%04X,%02X,%08X,%02X}\n", \
                Counter1, \
                UsbSkip->bSkipType, \
                UsbSkip->bFlag, \
                UsbSkip->wBDF, \
                UsbSkip->bRootPort, \
                UsbSkip->dRoutePath, \
                UsbSkip->bBaseClass));
        
        UsbSkip++;
    }
                                        ///<(EIP96276.7)

    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->Ps2Support = %x\n",gFastBootPolicy->Ps2Support));
    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->NetworkStackSupport = %x\n",gFastBootPolicy->NetworkStackSupport));

    for (Counter1 = 0, PciSkipList = gFastBootPolicy->SkipPciList; Counter1 < gFastBootPolicy->SkipPciListSize / sizeof(SKIP_PCI_LIST2); Counter1++)
        DEBUG((DEBUG_INFO,"FB: FastBootPolicy->SkipPciList[%d] = Class:%x,SubClass:%x\n", Counter1, PciSkipList[Counter1].Class, PciSkipList[Counter1].SubClass));

    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->CheckPassword = %x\n",gFastBootPolicy->CheckPassword));

    DEBUG((DEBUG_INFO,"FB: FastBootPolicy->SkipTseHandshake = %x\n",gFastBootPolicy->SkipTseHandshake));

    DEBUG((DEBUG_INFO,"FB: ==================================================\n"));

}
                                        ///(EIP96276.4)>>
/**
 * This function check the boot option and parse the device path to knwo this
 *     boot option is belong to LegacyType, UsbDevice, HardDrive, CDROM, NetWork,
 *     or WindowsBootManager.
 *     
 * @param[in]   BootOrderIndex        Index number of BootOrder.
 * @param[in]   BootOption            The boot option of BootOrder[BootOrderIndex].
 *
 */

BOOLEAN 
EFIAPI 
IsValidFastBootOption(
    IN UINTN              BootOrderIndex, 
    IN EFI_LOAD_OPTION    *BootOption 
)
{    
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    CHAR16 *Description;
    CHAR16 *WindowsBootManagerStr = L"Windows Boot Manager";    
    BOOLEAN LegacyType = FALSE;
    BOOLEAN UsbDevice = FALSE;
    BOOLEAN HardDrive = FALSE;
    BOOLEAN Cdrom = FALSE;
    BOOLEAN NetWork = FALSE;
    BOOLEAN WindowsBootManager = FALSE;
    
///check attribute

    if (!(BootOption->Attributes & LOAD_OPTION_ACTIVE)) return FALSE;

///check windows boot manager
    
    Description = (CHAR16 *)(BootOption + 1);
    if (!MemCmp(WindowsBootManagerStr, Description, Strlen((char*)WindowsBootManagerStr)))
        WindowsBootManager = TRUE;
    
///check device type
    
    Dp = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8*)Description + (Wcslen(Description) + 1) * sizeof(CHAR16));
    
    for ( ; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp)) {
        if (Dp->Type == BBS_DEVICE_PATH) LegacyType = TRUE; 
    
        if (Dp->Type == MESSAGING_DEVICE_PATH && Dp->SubType == MSG_USB_DP)
            UsbDevice = TRUE;
    
        if (Dp->Type == MESSAGING_DEVICE_PATH && Dp->SubType == MSG_MAC_ADDR_DP)
            NetWork = TRUE;        
    
        if (Dp->Type == MEDIA_DEVICE_PATH && Dp->SubType == MEDIA_HARDDRIVE_DP) 
            HardDrive = TRUE;
        
        if (Dp->Type == MEDIA_DEVICE_PATH && Dp->SubType == MEDIA_CDROM_DP) 
            Cdrom = TRUE;
    }
    
    DEBUG((DEBUG_INFO,"FB: BootOrder Index %x\n", BootOrderIndex));
    DEBUG((DEBUG_INFO,"FB: Windows Boot Manager: %x\n", WindowsBootManager));
    DEBUG((DEBUG_INFO,"FB: Legacy %x,Usb %x, HardDrive %x, CDROM %x, NetWork %x\n", \
        LegacyType, UsbDevice, HardDrive, Cdrom, NetWork));    
    
/// there are some samples for selecting a boot option as fast boot device.
/**
//Case 1. Fast boot with windows boot manager and it's the highest priority in boot order.
    if (BootOrderIndex == 0 && WindowsBootManager == 1) return TRUE;

//Case 2. don't support Legacy device
    if (LegacyType == TRUE) return FALSE;

//Case3. don't support usb type
    if (UsbDevice == TRUE) return FALSE;
**/

    return TRUE;   
}

/**
 * Determines whether a device path, PathAll, contains the device path, PathOne.  
 *     
 * @param[in]   PathAll        The device path to be scanned.
 * @param[in]   PathOne        The device path to locate within PathAll.
 *
 * @retval     TRUE            PathOne contains PathAll.
 * @retval    FALSE            PathOne doesn't contain PathAll.
 *
 */

BOOLEAN 
IsOneOfDp(
  IN EFI_DEVICE_PATH_PROTOCOL *PathAll, 
  IN EFI_DEVICE_PATH_PROTOCOL *PathOne 
)
{
    EFI_DEVICE_PATH_PROTOCOL *Path;
    
    Path = PathOne;

    do{
        if (isEndNode(PathAll)) return FALSE;        
        
        if (DPLength(PathAll) < DPLength(PathOne)) return FALSE;
    
        if (!(PathAll->Type == Path->Type && PathAll->SubType == Path->SubType))
        {
            PathAll = NEXT_NODE(PathAll);
            continue;
        }

        if (!MemCmp(PathAll, PathOne, DPLength(PathOne)))
            return TRUE;
        else
            PathAll = NEXT_NODE(PathAll);
        
    }while(TRUE);
    
}

/**
 * Search DevicePath and find out indicated device path node.
 *     
 * @param[in]           DevicePath        Searched device path.
 * @param[in]           Type                Indicated device path type.
 * @param[in]           SubType            Indicated device path sub type.
 *
 * @retval     TRUE                            Return indicated device path pointer.
 *
 */

EFI_DEVICE_PATH_PROTOCOL* 
FindDevicePath (
  IN OUT   EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN       UINT8                    Type,
  IN       UINT8                    SubType
)
{
    if (DevicePath == NULL) {
        return NULL;
    }
    
    while (!isEndNode (DevicePath)) {
        if (DevicePath->Type == Type && DevicePath->SubType== SubType)
            return DevicePath;
        
        DevicePath = NEXT_NODE (DevicePath);
    }
    
    return NULL;
}                                       

/**
 * Connect SATA controller in fast boot path.
 *     
 */

EFI_STATUS 
FastConnectSata()
{
    
    DEBUG((DEBUG_INFO,"FB:Connect Sata\n"));    

    switch(gFastBootPolicy->SataSupport)
    {
        case 0: /// only connect previous boot HDD?
            return ConnectLastHddDevice();        
        case 1: /// connect all SATA device
            ConnectAllSataDevices();        
            break;
#if SUPPORT_RAID_DRIVER            
        case 2: /// only connect HDDs, ODD & Port Multiplier will be skipped.
            return ConnectHddDevices();        
#endif            
    }
    
    return EFI_SUCCESS;
}
                                        ///<<(EIP96276.4)

/**
 * Check boot flow.
 *     
 * @retval EFI_SUCCESS            Boot flow doesn't change. 
 * @retval EFI_UNSUPPORTED        Boot flow changed.       
 */

EFI_STATUS 
EFIAPI 
FastbootCheckBootFlow()
{
    EFI_STATUS Status;
    UINT32     BootFlow;
    UINTN      Size;
#if FASTBOOT_NEED_RESTART    
    UINTN    Flag = 1;
    UINTN    Count = 0;
    EFI_EVENT  FastBootReadyToBootEvent;
#endif   
    
    DEBUG((DEBUG_INFO, "FB: FastBoot started\n"));
    Size = sizeof(BootFlow);
    Status = gRT->GetVariable(L"BootFlow", &GuidBootFlow, NULL, &Size, (VOID *)&BootFlow);
    if (!EFI_ERROR(Status) && (BootFlow != BOOT_FLOW_CONDITION_NORMAL)) {
        /// Some other driver wants different bootflow - abandon fast boot.
#if FASTBOOT_NEED_RESTART        
        DEBUG((DEBUG_INFO,"FB : Normal boot need to notify.\n"));
        ///Initial Reboot Flag
        Status = gRT->SetVariable(FAST_BOOT_RESTART, 
                &FastBootRestartGuid, 
                EFI_VARIABLE_NON_VOLATILE |
                EFI_VARIABLE_BOOTSERVICE_ACCESS |
                EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof(Flag),
                &Flag);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        Status = gRT->SetVariable(FAST_BOOT_RESTART_COUNT, 
                &FastBootRestartCountGuid, 
                EFI_VARIABLE_NON_VOLATILE |
                EFI_VARIABLE_BOOTSERVICE_ACCESS |
                EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof(Count),
                &Count);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        Status = CreateReadyToBootEvent(TPL_CALLBACK,
                                        FastBootReadyToBootNotify,
                                        NULL,
                                        &FastBootReadyToBootEvent);
        if (EFI_ERROR(Status)) {
            return Status;
        }
#endif                                        
            return EFI_UNSUPPORTED;        
    }
    
    return EFI_SUCCESS;
}

/**
 * Run CapsulePresent.
 *     
 * @retval EFI_SUCCESS            Capsule not Present. 
 * @retval EFI_UNSUPPORTED        Capsule Present.    
 */

EFI_STATUS 
EFIAPI 
FastbootCapsulePresent()
{
#if CAPSULE_SUPPORT
    if (CapsulePresent()) {
        return EFI_UNSUPPORTED; 
    }
#endif
    return EFI_SUCCESS;
}

/**
 * Run OemConfigurationChanged.
 *     
 * @retval EFI_SUCCESS            Configuration was not changed to fast boot flow. 
 * @retval EFI_UNSUPPORTED        Configuration was changed to fast boot flow.     
 */

EFI_STATUS 
EFIAPI 
FastbootOemConfigurationChanged()
{
    if (OemConfigurationChanged()) {
        return EFI_UNSUPPORTED; 
    }
    
    return EFI_SUCCESS;
}

/**
 * Check last time boot data exist or not.
 *     
 * @retval EFI_SUCCESS            Last boot data exist.
 * @retval EFI_UNSUPPORTED        Can't find last boot data.    
 */

EFI_STATUS 
EFIAPI 
FastbootCheckLastBootVariablePresent()
{
    EFI_STATUS Status;
    EFI_EVENT  FastBootReadyToBootEvent;
    
    Status = CreateReadyToBootEvent(TPL_CALLBACK,
                                    FastBootReadyToBootNotify,
                                    NULL,
                                    &FastBootReadyToBootEvent);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    if (!gFastBootPolicy->LastBootVarPresence) {
        return EFI_UNSUPPORTED;
    }
    
    return EFI_SUCCESS;
}

/**
 * Show fastboot policy.
 * 
 * @retval EFI_SUCCESS            Function success.    
 */

EFI_STATUS 
EFIAPI 
FastbootUpdateAndShowPolicy()
{
    
    ShowFastBootPolicy(); 
    
    return EFI_SUCCESS;
}

/**
 * Check boot option change or not.
 * 
 * @retval EFI_SUCCESS            Boot option check pass.   
 * @retval EFI_UNSUPPORTED        Boot option changed and device path is not the same.       
 */

EFI_STATUS 
EFIAPI 
FastbootCheckBootOption()
{
    CHAR16     BootOptionName[9];
    EFI_LOAD_OPTION *BootOption = NULL;    
    UINTN      Size;
    EFI_STATUS Status;
    UINT16     *BootOrder = NULL;
    UINT8       Counter;
    BOOLEAN     VaildBootOption = FALSE;
    UINTN       BootOptionSize;         ///(EIP96276.4+)
    
    if (gFastBootPolicy->CheckBootOptionNumber) {
    
        Size = 0;
        Status = GetEfiVariable(L"BootOrder", &gEfiGlobalVariableGuid, NULL, &Size, &BootOrder);

        for (Counter = 0; Counter < (Size / sizeof(UINT16)); Counter++, BootOption = NULL) {    ///(EIP96276.4)

            Swprintf(BootOptionName, gBootName, BootOrder[Counter]);
            Status = GetEfiVariable(BootOptionName, &gEfiGlobalVariableGuid, NULL, &BootOptionSize, &BootOption);//(EIP96276.4)
            if (EFI_ERROR(Status))
                return Status;         
            
            VaildBootOption = IsValidFbBootOptionPtr(Counter, BootOption);
            if (VaildBootOption) break;

            gBS->FreePool(BootOption);

            if (VaildBootOption == FALSE && Counter == (Size / sizeof(UINT16) - 1 )) {
                DEBUG((DEBUG_ERROR, "FB: There is no valid boot option\n"));
                gBS->FreePool(BootOrder);
                return EFI_UNSUPPORTED;

            }
        }
        
        DEBUG((DEBUG_INFO, "FB: LastBootOption=%x, BootOrder[%d]=%x\n", gFastBootPolicy->BootOptionNumber, Counter, BootOrder[Counter]));
        
        if (gFastBootPolicy->BootOptionNumber != BootOrder[Counter]) 
        {
        ///
        /// BootOption Number check fail, then check device path.
        /// If device path is also wrong, abort the fast boot.
        ///                
            EFI_DEVICE_PATH_PROTOCOL *Dp;
            CHAR16                   *Description;   
            Description = (CHAR16 *)(BootOption + 1);                
            Dp = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8*)Description + (Wcslen(Description) + 1) * sizeof(CHAR16));

            if (!IsOneOfDp(gFastBootPolicy->FastBootOption, Dp)) {
                gBS->FreePool(BootOrder);
                gBS->FreePool(BootOption);
                return EFI_UNSUPPORTED;
            }
            
            DEBUG((DEBUG_INFO,"FB: Update new boot option number\n")); 
            gFastBootPolicy->BootOptionNumber = BootOrder[Counter];               
        }

        gBS->FreePool(BootOrder);
        gBS->FreePool(BootOption);
    }
    
    return EFI_SUCCESS;
}

/**
 * Check and connect console.
 *     
 * @retval EFI_SUCCESS            Function success.       
 */

EFI_STATUS 
EFIAPI 
FastbootConsoleCheck()
{
    PERF_START(0, "FastBootConnectConsole", "FB" ,0);
    FastConnectConsoles();
    PERF_END(0, "FastBootConnectConsole", "FB",0);
    
    return EFI_SUCCESS;
}

/**
 * Load string.
 *     
 * @retval EFI_SUCCESS            Function success.       
 */


EFI_STATUS 
EFIAPI 
FastbootBdsLoadStrings()
{
    ///<EIP118901+> 2013/06/21 >>>
    BdsLoadStrings();
    ///<EIP118901+> 2013/06/21 <<<
    return EFI_SUCCESS;
}

/**
 * BeforeConnectFastBootDeviceHook elink.
 *     
 * @retval EFI_SUCCESS            Function success.       
 */


EFI_STATUS 
EFIAPI 
FastbootBeforeConnectDeviceHook()
{
    
    UINT32 Counter;
    
    PERF_START(0,"BeforeConnectFbDevElink","FB",0);
                                        ///(EIP62683+)>
    /// before all driver connect elink 
    for (Counter = 0; BeforeConnectFastBootDeviceHook[Counter]; Counter++)
        BeforeConnectFastBootDeviceHook[Counter](); ///(EIP85135)

                                        ///<(EIP62683+)
    PERF_END(0,"BeforeConnectFbDevElink","FB",0);    

    return EFI_SUCCESS;
}

/**
 * SATA device check and connect.
 *     
 * @retval EFI_SUCCESS          Device check success.       
 */

EFI_STATUS 
EFIAPI 
FastbootSataCheck()
{
    EFI_STATUS Status;
    
    Status = FastConnectSata();
    if (EFI_ERROR(Status)) {
        return Status;   
    }
    
    return EFI_SUCCESS;
}

/**
 * Connect boot device.
 * 
 * @retval EFI_SUCCESS            Connect success.         
 */

EFI_STATUS 
EFIAPI 
FastbootConnectBootDevice()
{
    EFI_STATUS Status;
    
    PERF_START(0,"ConnectFbDev","FB",0);
    if (gFastBootPolicy->UefiBoot == TRUE)
        Status = ConnectFastEfiBootDevice();
    else
        Status = ConnectFastLegacyBootDevice();
    PERF_END(0,"ConnectFbDev","FB",0);

    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    return EFI_SUCCESS;
}

/**
 * FastBootAfterAllDriverConnctHook elink.
 *     
 * @retval EFI_SUCCESS            Function success.       
 */


EFI_STATUS 
EFIAPI 
FastbootAfterAllDriverConnct()
{
    UINT32 Counter;
    ///(EIP62683+)>
    PERF_START(0,"AfterConnectFbDevElink","FB",0);                                        
    /// after all driver connect elink 
    for (Counter = 0; AfterAllDriverConnectList[Counter]; Counter++) {
        AfterAllDriverConnectList[Counter]();
    }
    PERF_END(0,"AfterConnectFbDevElink","FB",0);    
    ///<(EIP62683+)

    return EFI_SUCCESS;
}

/**
 * Run CallTheDispatcher.
 * 
 * @retval EFI_SUCCESS            Function success.    
 */

EFI_STATUS 
EFIAPI 
FastbootCallTheDispatcher()
{
    CallTheDispatcher();
    return EFI_SUCCESS;
}


/**
 * Run RecoverTheMemoryAbove4Gb to make sure memory show correct in OS.
 *  
 * @retval EFI_SUCCESS            Function success.      
 */

EFI_STATUS 
EFIAPI 
FastbootRecoverTheMemory()
{
    
    RecoverTheMemoryAbove4Gb();
    return EFI_SUCCESS;
}

/**
 * Change ps2 variable to make sure ps2 device can work in OS.
 * 
 * @retval EFI_SUCCESS            Function success.    
 */

EFI_STATUS 
EFIAPI 
FastbootChangePs2VarValue()
{
#if PS2MOUSE_SUPPORT && PS2KBD_SUPPORT  
    if (!gFastBootPolicy->Ps2Support){
        DEBUG((DEBUG_INFO,"FB: Change Ps2 Variable Value\n"));   
        ChangePs2VarValue();
    }
#endif
    
    return EFI_SUCCESS;
}

/**
 * Run tsehandshake if there is no skip it.
 *     
 * @retval EFI_ABORTED            Boot fail, return to normal boot.
 * @retval EFI_SUCCESS            There have some problem so tse back to fastboot, 
 *                                use fastboot's boot function to boot.
 */

EFI_STATUS 
EFIAPI 
FastbootTseHandshake()
{
    EFI_STATUS Status;
    
    if ((gFastBootPolicy->SkipTseHandshake == 0) && (TSE_MAJOR >= 0x02) && (TSE_MINOR >= 0x10)) {
        PERF_START(0,"TSE","FB",0);        
        Status = TseHandshake();
        if (Status == EFI_ABORTED)   ///fast boot failed, proceed to full boot
            return Status;
    }
    
    return EFI_SUCCESS;
}

/**
 * Prepare boot to OS.
 * @retval EFI_ABORTED            Boot fail, return to normal boot.
 */

EFI_STATUS 
EFIAPI 
FastbootPrepareToBoot()
{
    ///Stop CheckForKey callback timer in TSE
    gFastBootTseProtocol->FastBootStopCheckForKeyTimer();
                                            ///<(EIP63924+)
    ///if we are here, somehow we failed launch fast boot through TSE - do it on our own
        
    if (gFastBootPolicy->UefiBoot == TRUE) {
        FastEfiBoot();
    } else {
        FastLegacyBoot();
    }
    
    /// Go to here means boot fail.
    return EFI_ABORTED;
}


/**
 * Function to determine if the boot path is to UEFI Shell.
 *     
 * @param[in]  Dp        Pointer to device path of bootable device.
 * 
 * @retval     TRUE        It's UEFI Shell boot path.
 * @retval     FALSE        It's not UEFI Shell boot path.
 *
 */

BOOLEAN 
EFIAPI 
IsBootToShell (
    IN EFI_DEVICE_PATH_PROTOCOL     *Dp 
)
{
    if (Dp->Type == MEDIA_DEVICE_PATH &&
       Dp->SubType == MEDIA_VENDOR_DP &&
       !guidcmp(&(((VENDOR_DEVICE_PATH *)Dp)->Guid), &AmiMediaDevicePathGuid))
       return TRUE;
    
    return FALSE;
}
                                        ///(EIP63924+)>

/**
 * Check TSE Post key is pressed or not.
 *     
 * 
 * @retval     TRUE        Post key is pressed.
 * @retval     FALSE        Post key is not pressed.
 *
 */

BOOLEAN 
EFIAPI 
CheckPostManagerKey()
{
    EFI_STATUS CheckForKeyStatus;
    BOOLEAN EnterSetup;
    UINT32  BootFlow;

    CheckForKeyStatus = gFastBootTseProtocol->FastBootCheckForKey(&EnterSetup, &BootFlow);
    if (CheckForKeyStatus == EFI_SUCCESS) return TRUE;
    return FALSE;  
}
                                        ///<(EIP63924+)

#if SUPPORT_RAID_DRIVER

/**
 * Check connected HDDs are as same as previous boot's.
 *     
 * @param[in]  Bdf            BDF of previous boot's HDD .
 * @param[in]  ClassCode    Class of previous boot's HDD .
 * @param[in]  PortNum        Port Number of previous boot's HDD .
 * 
 * @retval     EFI_NOT_FOUND            Can't find HDD or serial number.
 * @retval     EFI_SUCCESS                    HDD find and serial number match.
 * @retval    EFI_DEVICE_ERROR        Serial number doesn't match
 *
 */

EFI_STATUS 
CheckConnectedHdd(
  IN UINT16     Bdf,
  IN UINT16     ClassCode,
  IN UINT8      PortNum 
)
{
    EFI_STATUS  Status;       
    EFI_GUID    DiskInfoGuid = EFI_DISK_INFO_PROTOCOL_GUID;
    EFI_GUID    DiskInfoIdeGuid = EFI_DISK_INFO_IDE_INTERFACE_GUID;
    EFI_GUID    DiskInfoAhciGuid = EFI_DISK_INFO_AHCI_INTERFACE_GUID;    
    EFI_DISK_INFO_PROTOCOL  *DiskInfoProtocol;
    EFI_HANDLE  *Handle;
    UINTN   DiskInfoCount;
    UINTN   PciSeg,PciBus,PciDev,PciFun;    
    UINT16  DetectedHddBdf;
    UINT16  DetectedHddClassCode;
    UINT8   DetectedHddPortNum;
    UINT8   *DetectedHddSerialNum = NULL;
    BOOLEAN StrucFound = FALSE;
    UINT8   Counter;
    SATA_DEVICE_DATA *SataDevice = (SATA_DEVICE_DATA*)gFastBootPolicy->SataDevice;    
   
///
/// Find out SATA device structure the from last boot SATA device list
///
    for (Counter = 0; Counter < MAX_SATA_DEVICE_COUNT && SataDevice->Bdf != 0; SataDevice++) {
        if (Bdf == SataDevice->Bdf &&\
            ClassCode == SataDevice->ClassCode &&\
            PortNum == SataDevice->PortNum)
        {
            StrucFound = TRUE;
            break;
        }        
    }    
    if (StrucFound == FALSE) return EFI_NOT_FOUND;

///
/// Find out serial number of detected HDD.
///    

    Status=gBS->LocateHandleBuffer(ByProtocol, &DiskInfoGuid,NULL, &DiskInfoCount, &Handle);
    DEBUG((DEBUG_VERBOSE,"LocateHandleBuffer DiskInfoGuid %r\n", Status));
    if (EFI_ERROR(Status)) return Status;

    for (Counter = 0; Counter < DiskInfoCount; Counter++)
    {
        Status = gBS->HandleProtocol(Handle[Counter], &DiskInfoGuid, (VOID**)&DiskInfoProtocol);
        DEBUG((DEBUG_VERBOSE,"HandleProtocol DiskInfoGuid %r\n", Status));
        if (EFI_ERROR(Status)) continue;

        if (!guidcmp(&DiskInfoProtocol->Interface, &DiskInfoIdeGuid)) {
#if IdeBusSrc_SUPPORT
        ///            
        /// Ide diskInfo
        ///
            AMI_IDE_BUS_PROTOCOL    *IdeInterface = ((IDE_DISK_INFO*)DiskInfoProtocol)->IdeBusInterface;
            EFI_PCI_IO_PROTOCOL *PciIo = IdeInterface->PciIO;
            
            Status = PciIo->GetLocation(PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
            ASSERT_EFI_ERROR(Status);

            DetectedHddBdf = (UINT16)((PciBus << 8) | (PciDev << 3) | PciFun );
                
            Status = PciIo->Pci.Read (PciIo,\
                EfiPciIoWidthUint16,\
                0x0A,\
                1,\
                &DetectedHddClassCode);            
            ASSERT_EFI_ERROR(Status);

            DetectedHddPortNum = IdeInterface->IdeDevice.Channel | IdeInterface->IdeDevice.Device<<1;

            if (DetectedHddBdf != Bdf ||\
                DetectedHddClassCode != ClassCode ||\
                DetectedHddPortNum != PortNum)
                continue;                

            DetectedHddSerialNum = IdeInterface->IdeDevice.IdentifyData.Serial_Number_10;
            break;
#endif
        }else if (!guidcmp(&DiskInfoProtocol->Interface,&DiskInfoAhciGuid)) {
        ///
        ///Device is detected in AHCI mode
        ///  
            SATA_DEVICE_INTERFACE *SataInterface = ((SATA_DISK_INFO*)DiskInfoProtocol)->SataDevInterface;
            EFI_PCI_IO_PROTOCOL *PciIo = SataInterface->AhciBusInterface->PciIO;
                    
            Status = PciIo->GetLocation(PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
            ASSERT_EFI_ERROR(Status);
  
            DetectedHddBdf = (UINT16)((PciBus << 8) | (PciDev << 3) | PciFun );
            
            Status = PciIo->Pci.Read (PciIo,\
                EfiPciIoWidthUint16,\
                0x0A,\
                1,\
                &DetectedHddClassCode);            
            ASSERT_EFI_ERROR(Status);

            DetectedHddPortNum = SataInterface->PortNumber;

            if (DetectedHddBdf != Bdf ||\
                DetectedHddClassCode != ClassCode ||\
                DetectedHddPortNum != PortNum)
                continue;      

            DetectedHddSerialNum = SataInterface->IdentifyData.Serial_Number_10;
            break;           
            
        }    
    }

    gBS->FreePool(Handle);

    if (DetectedHddSerialNum == NULL) return EFI_NOT_FOUND;

///
/// Compare Serial number is as same as last boot.
///

    if (!MemCmp(DetectedHddSerialNum, SataDevice->SerialNum, 20))
        return EFI_SUCCESS;


    DEBUG((DEBUG_INFO,"FB: ConnectedHdd is match with previous boot\n"));

    DEBUG((DEBUG_INFO,"Detected BDS %x, ClassCode %x, PortNum %x\n", DetectedHddBdf, DetectedHddClassCode, DetectedHddPortNum));

    DEBUG((DEBUG_INFO,"Detected Serial Number: "));
    
    for (Counter = 0; Counter < 20; Counter++)
    {
        DEBUG((DEBUG_INFO,"%02x ",DetectedHddSerialNum[Counter]));
    }
    DEBUG((DEBUG_INFO," \n"));


    DEBUG((DEBUG_INFO,"Last BDS %x, ClassCode %x, PortNum %x\n",SataDevice->Bdf, SataDevice->ClassCode, SataDevice->PortNum));
    
    DEBUG((DEBUG_INFO,"Last Serial Number: "));
    
    for (Counter = 0; Counter < 20; Counter++)
    {
        DEBUG((DEBUG_INFO,"%02x ", SataDevice->SerialNum[Counter]));
    }
    DEBUG((DEBUG_INFO," \n"));

    return EFI_DEVICE_ERROR;
}
#endif

/**
 * Check device path is for SATA device or not.
 *     
 * @param[in]  Dp            Device path need to be checked.
 * 
 * @retval     TRUE            The device path is for SATA device.
 * @retval     FALSE            The device path is not for SATA device.
 *
 */

BOOLEAN 
IsSataDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL *Dp
)
{
    EFI_DEVICE_PATH_PROTOCOL *TempDp;
    
    TempDp = FindDevicePath(Dp, MESSAGING_DEVICE_PATH, MSG_SATA_DP);
    if (TempDp != NULL) return TRUE;

    TempDp = FindDevicePath(Dp, MESSAGING_DEVICE_PATH, MSG_ATAPI_DP);
    if (TempDp != NULL) return TRUE;

    return FALSE;
}

/**
 * Check device path is legacy raid device path or not.
 *     
 * @param[in]  Dp            Device path need to be checked.
 * 
 * @retval     TRUE            The device path is for  legacy raid device.
 * @retval     FALSE            The device path is not for legacy raid device.
 *
 */

BOOLEAN 
IsLegacyRaidDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL *Dp
)
{
    EFI_DEVICE_PATH_PROTOCOL *TempDp = NULL;
#if SUPPORT_RAID_DRIVER    
    PCI_DEVICE_PATH *PciDp = NULL;
    SATA_DEVICE_DATA *SataDevice = (SATA_DEVICE_DATA*)gFastBootPolicy->SataDevice;   
    UINT8    Counter;
    UINT16  Bdf;
#endif

    
    if (gFastBootPolicy->UefiBoot == TRUE)  return FALSE;

    TempDp = FindDevicePath(Dp, HARDWARE_DEVICE_PATH, HW_VENDOR_DP);
    if (TempDp == NULL) return FALSE;
    
    TempDp = FindDevicePath(Dp, HARDWARE_DEVICE_PATH, HW_PCI_DP);
    if (TempDp == NULL) return FALSE;
    
#if SUPPORT_RAID_DRIVER
    PciDp = (PCI_DEVICE_PATH*)TempDp;
    Bdf = (UINT16)((0 << 8) | (PciDp->Device << 3) | PciDp->Function );
    for (Counter = 0; Counter < MAX_SATA_DEVICE_COUNT && SataDevice[Counter].Bdf != 0; Counter++) {
        if (Bdf == SataDevice[Counter].Bdf)
            return TRUE;
    }
#endif
    return FALSE;    
}

/**
 * Connect controller by device path recursively.
 *     
 * @param[in]  Path        Device path want to be connected.
 * 
 *
 */

VOID 
ConnectDevicePath_Recursive (
    IN EFI_DEVICE_PATH_PROTOCOL *Path)
{
    EFI_HANDLE Handle;
    EFI_STATUS Status;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath;

    while (TRUE)
    {
        EFI_DEVICE_PATH_PROTOCOL *LastPath = NULL;
        EFI_DEVICE_PATH_PROTOCOL *FirstNode = Path;

        if (isEndNode(Path))
        {
            if (Path->SubType == END_ENTIRE_SUBTYPE) break;
            Path++;
            continue;
        }

        while (TRUE) {
            EFI_DEVICE_PATH_PROTOCOL *Dp;
            UINT8 SubType;

            Path = FirstNode;

            /// LocateDevicePath can not work with multi-instance device paths.
            /// Prepare single instance device path and call LocateDevicePath.

            Dp = DPGetEndNode(Path);
            SubType = Dp->SubType;
            Dp->SubType = END_ENTIRE_SUBTYPE;
            Status = gBS->LocateDevicePath(&gEfiDevicePathProtocolGuid, \
                                                            &Path, &Handle);
            Dp->SubType = SubType;
            if (EFI_ERROR(Status)) break;

            if (isEndNode(Path))
            {
                /// Last time let's do it recursively.

                gBS->ConnectController(Handle, NULL, NULL, TRUE);  
                break;
            }
            if (Path == LastPath) break;
            LastPath = Path;

            if (StopBlkIo == TRUE) {
                gHandle = Handle;
                Status = gBS->OpenProtocol( Handle,
                            &gEfiDevicePathProtocolGuid,
                            (VOID **)&DevicePath,
                            ThisImageHandle,     
                            Handle,   
                            EFI_OPEN_PROTOCOL_BY_DRIVER);
            } else {
                Status = gBS->CloseProtocol ( Handle,
                                &gEfiDevicePathProtocolGuid,
                                ThisImageHandle,
                                Handle);
            }

            if (EFI_ERROR(gBS->ConnectController(Handle, NULL, Path, TRUE)))
                break;
        }

        while (!isEndNode(Path))
            Path = NEXT_NODE(Path);
    }
}

/**
 * Connect All Raid Controller.
 * 
 *
 */

VOID 
ConnectAllRaidController(){
    EFI_STATUS  Status;
    EFI_HANDLE  *Handle;
    UINTN       Number, Counter;

    DEBUG((DEBUG_INFO,"FB: ConnectAllRaidController\n"));

    ///Connect all SATA RAID Controllers
    Status = FbGetPciHandlesByClass(
        PCI_CL_MASS_STOR, PCI_CL_MASS_STOR_SCL_RAID, &Number, &Handle
    );
    if (EFI_ERROR(Status)){
        DEBUG((DEBUG_ERROR,"FB: There are no raid controller!\n"));
        return;
    }
    for (Counter = 0; Counter < Number; Counter++){
        gBS->ConnectController(Handle[Counter], NULL, NULL, TRUE);
    }        
    gBS->FreePool(Handle);
}

/**
 * Connect previous boot HDD.
 *     
 *
 */

EFI_STATUS 
ConnectLastHddDevice()
{
    EFI_DEVICE_PATH_PROTOCOL *Dp = gFastBootPolicy->FastBootOption;
    EFI_DEVICE_PATH_PROTOCOL *TempDp = Dp;
    EFI_GUID    PciIoProtocolGuid = EFI_PCI_IO_PROTOCOL_GUID;
    EFI_PCI_IO_PROTOCOL *PciIo; 
    EFI_STATUS  Status;
    EFI_HANDLE  PciHandle;
    UINT16  ClassCode;
    UINT16  Vid;

    if (IsLegacyRaidDevicePath(Dp))
    {
        /// there is no port information in Legacy Raid device, so decide to 
        /// connect all HDDs.        
        DEBUG((DEBUG_INFO, "FB: ConnectLastHddDevice - Find Legacy raid!\n"));
        DEBUG((DEBUG_INFO, "FB: Attempt to connect all Sata Device\n"));        
        ConnectAllSataDevices();
        return EFI_SUCCESS;
    }

    DEBUG((DEBUG_INFO, "FB: ConnectLastHddDevice\n"));

    TempDp = Dp;
    
    Status = gBS->LocateDevicePath(&PciIoProtocolGuid, &TempDp, &PciHandle);
    if (EFI_ERROR(Status)) return Status;
   
    Status = gBS->HandleProtocol(PciHandle, &PciIoProtocolGuid, (VOID**)&PciIo);
    if (EFI_ERROR(Status)) return Status;

    Status = PciIo->Pci.Read (PciIo,\
        EfiPciIoWidthUint16,\
        0x0A,\
        1,\
        &ClassCode);            
    ASSERT_EFI_ERROR(Status);

    Status = PciIo->Pci.Read (PciIo,\
        EfiPciIoWidthUint16,\
        0x00,\
        1,\
        &Vid);            
    ASSERT_EFI_ERROR(Status);
    
    if (ClassCode != 0x0104 && ClassCode != 0x0106 && ClassCode != 0x0101)
        return EFI_SUCCESS; ///is not SATA device, do Not try to connect it.

    if (ClassCode == 0x0104){    /// RAID Mode
        ConnectAllSataDevices();
    }
    else{
        ConnectDevicePath_Recursive(Dp);
    }

    return EFI_SUCCESS;
}

#if SUPPORT_RAID_DRIVER

/**
 * Connect all previous boot HDDs.
 *     
 *
 */

EFI_STATUS 
ConnectHddDevices()
{    
    UINT8 SataPort_DevPath[]= {
    /// ACPI Device Path
    0x02,0x01,0x0c,0x00,0xd0,0x41,0x03,0x0a,0x00,0x00,0x00,0x00,
    ///PCI Device Path
    0x01,0x01,0x06,0x00,0x02,0x1f,
    //SATA Device Path
    0x03,0x12,0x0a,0x00,0x00,0x00,0xff,0xff,0x00,0x00,
    ///Terminal
    0x7f,0xff,0x04,0x00
    };
    
    UINT8 IdePort_DevPath[]= {
    /// ACPI Device Path
    0x02,0x01,0x0c,0x00,0xd0,0x41,0x03,0x0a,0x00,0x00,0x00,0x00,
    ///PCI Device Path
    0x01,0x01,0x06,0x00,0x02,0x1f,
    ///SATA Device Path
    0x03,0x01,0x08,0x00,0x00,0x00,0x00,0x00,
    ///Terminal
    0x7f,0xff,0x04,0x00
    };

    EFI_STATUS  Status;   
    UINT8 Counter;
    EFI_DEVICE_PATH_PROTOCOL *TempDp;
    UINT8   Device, Function;
    SATA_DEVICE_DATA *SataDevice = (SATA_DEVICE_DATA*)gFastBootPolicy->SataDevice;
    EFI_DEVICE_PATH_PROTOCOL *Dp = gFastBootPolicy->FastBootOption;

    if (IsLegacyRaidDevicePath(Dp) && SataDevice[0].ClassCode == 0x0104)
    {
        DEBUG((DEBUG_INFO, "FB: Attempt to connect all Sata Device\n"));        
        ConnectAllSataDevices();
        return EFI_SUCCESS;
    }
    DEBUG((DEBUG_INFO, "FB: ConnectHddDevices\n"));
    
    for (Counter = 0; Counter < MAX_SATA_DEVICE_COUNT && SataDevice[Counter].Bdf != 0; Counter++)
    {
        if (SataDevice[Counter].DevType == ATAPI || SataDevice[Counter].DevType == PMPORT)
            continue;
        
        if (SataDevice[Counter].ClassCode == 0x0106 || SataDevice[Counter].ClassCode == 0x0104) 
        { /// AHCI or RAID
            if (SataDevice[Counter].ClassCode == 0x0104) {
                StopBlkIo = TRUE;
            }
            Function = SataDevice[Counter].Bdf & 0x03;
            Device = (SataDevice[Counter].Bdf >> 3) & 0x1F;            
            TempDp = FindDevicePath((EFI_DEVICE_PATH_PROTOCOL*)SataPort_DevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);
            if (TempDp == NULL) { 
                StopBlkIo = FALSE;
                continue;                 
            }
            ((PCI_DEVICE_PATH*)TempDp)->Device = Device;
            ((PCI_DEVICE_PATH*)TempDp)->Function = Function;            
    
            TempDp = FindDevicePath((EFI_DEVICE_PATH_PROTOCOL*)SataPort_DevPath, MESSAGING_DEVICE_PATH, MSG_SATA_DP);
            ((SATA_DEVICE_PATH*)TempDp)->HBAPortNumber = (UINT16)SataDevice[Counter].PortNum;

            ConnectDevicePath_Recursive((EFI_DEVICE_PATH_PROTOCOL*)SataPort_DevPath);
            if (SataDevice[Counter + 1].Bdf == 0) {
                StopBlkIo = FALSE;
                gBS->CloseProtocol ( gHandle,
                                &gEfiDevicePathProtocolGuid,
                                ThisImageHandle,
                                gHandle);
                Status = gBS->ConnectController(gHandle, NULL, NULL, TRUE); 
            }
            Status = CheckConnectedHdd(SataDevice[Counter].Bdf, SataDevice[Counter].ClassCode, SataDevice[Counter].PortNum);
            if (EFI_ERROR(Status)) {
                if (StopBlkIo == TRUE) {
                    StopBlkIo = FALSE;
                    gBS->CloseProtocol ( gHandle,
                                &gEfiDevicePathProtocolGuid,
                                ThisImageHandle,
                                gHandle);
                }
                return Status;            
            }             
        }
        else if (SataDevice[Counter].ClassCode == 0x0101)
        { ///IDE

            Function = SataDevice[Counter].Bdf & 0x03;
            Device = (SataDevice[Counter].Bdf >> 3) & 0x1F;            
            TempDp = FindDevicePath((EFI_DEVICE_PATH_PROTOCOL*)IdePort_DevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);
                if (TempDp == NULL) continue;                 
            ((PCI_DEVICE_PATH*)TempDp)->Device = Device;
            ((PCI_DEVICE_PATH*)TempDp)->Function = Function;   
            
            TempDp = FindDevicePath((EFI_DEVICE_PATH_PROTOCOL*)IdePort_DevPath, MESSAGING_DEVICE_PATH, MSG_ATAPI_DP);
            ((ATAPI_DEVICE_PATH*)TempDp)->PrimarySecondary = SataDevice[Counter].PortNum & BIT0;
            ((ATAPI_DEVICE_PATH*)TempDp)->SlaveMaster = SataDevice[Counter].PortNum & BIT1;

            ConnectDevicePath_Recursive((EFI_DEVICE_PATH_PROTOCOL*)IdePort_DevPath);

            Status = CheckConnectedHdd(SataDevice[Counter].Bdf, SataDevice[Counter].ClassCode, SataDevice[Counter].PortNum);
            if (EFI_ERROR(Status)) return Status;
        }
    }

    /// Up to here, all recorded SATA devices are connected.
    ///
    /// But AHCIBUS would NOT be run in AMD Raid mode, so we don't know whether all Hdd is connect on Raid controller or not.
    /// Find out all AMD Raid controller and connect it.

    ConnectAllRaidController();

    return EFI_SUCCESS;
}
#endif

                                        ///(EIP62845+)>
/**
 *Connect All SATA Devices.
 *     
 *
 */

EFI_STATUS 
ConnectAllSataDevices()            ///(EIP85135)
{
    EFI_STATUS Status;
    EFI_HANDLE *Handle;
    UINTN Number, Counter;
        
    DEBUG((DEBUG_INFO, "FB: ConnectAllSataDevices\n"));
    
    ///Connect all SATA IDE Controllers
    Status = FbGetPciHandlesByClass(
        PCI_CL_MASS_STOR, PCI_CL_MASS_STOR_SCL_IDE, &Number, &Handle
    );

    if (!EFI_ERROR(Status)){
        for (Counter = 0; Counter < Number; Counter++){
            gBS->ConnectController(Handle[Counter], NULL, NULL, TRUE);
        }        
        gBS->FreePool(Handle);
    }
    
    ///Connect all SATA AHCI Controllers
    Status = FbGetPciHandlesByClass(
        PCI_CL_MASS_STOR, 0x06, &Number, &Handle
    );

    if (!EFI_ERROR(Status)){
        for (Counter = 0; Counter < Number; Counter++){
            gBS->ConnectController(Handle[Counter], NULL, NULL, TRUE);
        }        
        gBS->FreePool(Handle);
    }

                                        ///(EIP95568+)>
    ///Connect all SATA RAID Controllers
    Status = FbGetPciHandlesByClass(
        PCI_CL_MASS_STOR, PCI_CL_MASS_STOR_SCL_RAID, &Number, &Handle
    );

    if (!EFI_ERROR(Status)){
        for (Counter = 0; Counter < Number; Counter++){
            gBS->ConnectController(Handle[Counter], NULL, NULL, TRUE);
        }        
        gBS->FreePool(Handle);
    }
                                        ///<(EIP95568+)
    return EFI_SUCCESS;                    ///(EIP85135)

}

#if SUPPORT_RAID_DRIVER

/**
 * Get SATA port present HOB.
 *    
 * @param[in]   SataPresentHob                SATA HOB data.
 *
 * @retval EFI_SUCCESS                        SATA HOB get success.
 */

EFI_STATUS 
GetSataPortPresentHOB (
    IN SATA_PRESENT_HOB **SataPresentHob    
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
 * Collect the SATA device information created by AHCI and IDE 
 * bus driver.
 *    
 * @param[in]   Var                FastBoot variable.
 *
 */

VOID 
CollectSataDeviceInfo(
  IN FAST_BOOT2 *Var
)
{
    EFI_GUID    DiskInfoGuid = EFI_DISK_INFO_PROTOCOL_GUID;
    EFI_GUID    DiskInfoIdeGuid = EFI_DISK_INFO_IDE_INTERFACE_GUID;
    EFI_GUID    DiskInfoAhciGuid = EFI_DISK_INFO_AHCI_INTERFACE_GUID;
    EFI_DISK_INFO_PROTOCOL  *DiskInfoProtocol;
    UINTN DiskInfoCount, Counter1, Counter2;
    EFI_HANDLE *Handle;
    EFI_STATUS  Status;
    static BOOLEAN Executed = FALSE;
    UINTN Seg, Bus, Dev, Fun;    
    UINT8   SataDevCount = 0;
    SATA_PRESENT_HOB *SataPresentHob = NULL;

    if (Executed == TRUE) return;
///
/// Collect the port present information reported by chipset
///

    Status = GetSataPortPresentHOB(&SataPresentHob);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR,"FB: Sata port present HOB is not found, need CSP porting for it\n"));
        return;
    }

    MemCpy(&Var->ControllerCount, &SataPresentHob->ControllerCount, sizeof(SATA_PRESENT_HOB) - sizeof(EFI_HOB_GUID_TYPE));
    
///
/// Collect the SATA device information created by AHCI and IDE bus driver
///
    Status=gBS->LocateHandleBuffer(ByProtocol, &DiskInfoGuid, NULL, &DiskInfoCount, &Handle);
    if (EFI_ERROR(Status)) return;

    for (Counter1 = 0; Counter1 < DiskInfoCount; Counter1++)
    {
        Status=gBS->HandleProtocol(Handle[Counter1], &DiskInfoGuid, (VOID**)&DiskInfoProtocol);
        if (EFI_ERROR(Status)) continue;

        if (!guidcmp(&DiskInfoProtocol->Interface,&DiskInfoIdeGuid)) {
#if IdeBusSrc_SUPPORT
        ///            
        ///Devie is detected in IDE mode
        ///
            AMI_IDE_BUS_PROTOCOL    *IdeInterface = ((IDE_DISK_INFO*)DiskInfoProtocol)->IdeBusInterface;
            EFI_PCI_IO_PROTOCOL *PciIo = IdeInterface->PciIO;
            
            Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
            ASSERT_EFI_ERROR(Status);

            Var->SataDevice[SataDevCount].Bdf = (UINT16)((Bus << 8) | (Dev << 3) | Fun );
            
            Status = PciIo->Pci.Read (PciIo,\
                EfiPciIoWidthUint16,\
                0x0A,\
                1,\
                &Var->SataDevice[SataDevCount].ClassCode);            
            ASSERT_EFI_ERROR(Status);


            Var->SataDevice[SataDevCount].DevType = IdeInterface->IdeDevice.DeviceType;
            Var->SataDevice[SataDevCount].PortNum = IdeInterface->IdeDevice.Channel | IdeInterface->IdeDevice.Device<<1;
                
            MemCpy( &Var->SataDevice[SataDevCount].SerialNum,
                    &IdeInterface->IdeDevice.IdentifyData.Serial_Number_10, 20);
            
            SataDevCount++;
        
            ASSERT(SataDevCount<MAX_SATA_DEVICE_COUNT);
#endif
        } else if (!guidcmp(&DiskInfoProtocol->Interface,&DiskInfoAhciGuid)) {
        ///
        ///Device is detected in AHCI mode
        ///  
            SATA_DEVICE_INTERFACE *SataInterface = ((SATA_DISK_INFO*)DiskInfoProtocol)->SataDevInterface;
            EFI_PCI_IO_PROTOCOL *PciIo = SataInterface->AhciBusInterface->PciIO;
                    
            Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
            ASSERT_EFI_ERROR(Status);
  
            Var->SataDevice[SataDevCount].Bdf = (UINT16)((Bus << 8) | (Dev << 3) | Fun );
            
            Status = PciIo->Pci.Read (PciIo,\
                EfiPciIoWidthUint16,\
                0x0A,\
                1,\
                &Var->SataDevice[SataDevCount].ClassCode);            
            ASSERT_EFI_ERROR(Status);

            Var->SataDevice[SataDevCount].DevType = SataInterface->DeviceType;
            Var->SataDevice[SataDevCount].PortNum = SataInterface->PortNumber;
            
            MemCpy( &Var->SataDevice[SataDevCount].SerialNum,
                    &SataInterface->IdentifyData.Serial_Number_10, 20);

            SataDevCount++;
            
            ASSERT(SataDevCount<MAX_SATA_DEVICE_COUNT);
            
        } else {
            continue;
        }
    
    }

///
///  Print out message for SATA Port&Device information.
///

    for (Counter1 = 0; Counter1 < MAX_SATA_DEVICE_COUNT && Var->SataDevice[Counter1].Bdf != 0; Counter1++)
    {
        DEBUG((DEBUG_INFO,"FB: SataDevice[%d] -- BDF:%x, CalssCode:%x, DveType:%x, PortNum:%x\n",\
                Counter1, Var->SataDevice[Counter1].Bdf, Var->SataDevice[Counter1].ClassCode,\
            Var->SataDevice[Counter1].DevType, Var->SataDevice[Counter1].PortNum));            
        
        DEBUG((DEBUG_INFO,"Serial Number: "));
        
        for (Counter2 = 0; Counter2 < 20; Counter2++)
        {
            DEBUG((DEBUG_INFO,"%02x ", Var->SataDevice[Counter1].SerialNum[Counter2]));
        }
        DEBUG((DEBUG_INFO," \n"));
    }

    gBS->FreePool(Handle);

    return;       
}
#endif

                                        ///<(EIP62845+)

/**
 * FastBoot ReadyToBoot callback.
 *    
 * @param[in]   Event                The Event that is being processed.
 * @param[in]   Context            The Event Context.
 *
 */

VOID 
EFIAPI 
FastBootReadyToBootNotify(
    IN EFI_EVENT Event, 
    IN VOID *Context
)
{
    EFI_STATUS          Status;
    UINTN                    BootOptionCounter, Size;
    FAST_BOOT2        Var;
    UINT16                   *BootOrder = NULL;
    CHAR16                   BootOptionName[9];
    EFI_LOAD_OPTION          *BootOption = NULL;
    UINTN                    BootOptionSize;    ///(EIP96276.4+)
    UINT32                   BbsPopup;
    BOOLEAN     VaildBootOption = FALSE;
#if FASTBOOT_NEED_RESTART    
    UINTN    RebootFlag;
    UINTN    RebootFlagSize = sizeof(RebootFlag);
    UINTN    RebootCount;
    UINTN    RebootCountSize = sizeof(RebootCount);
    UINTN    ResetFlag = 1;
    UINTN    ClearCount = 0;
#endif

///check if we are on bbs-popup boot path
    Size = sizeof(BbsPopup);
    Status = gRT->GetVariable(L"BbsPopupCalled", &FastBootVariableGuid, NULL, &Size, (VOID *)&BbsPopup);
    if (!EFI_ERROR(Status) && (BbsPopup == 0x55aa55aa))  {   ///we are on bbs-popup boot path
        ResetFastBootVariable();    ///clear variable if it was created on previous boots
        gBS->CloseEvent(Event);  
        return;
    }

    Size = sizeof(Var);
    gBS->SetMem(&Var, Size, 0);

    Status = gRT->GetVariable(L"LastBoot", &FastBootVariableGuid, NULL, &Size, (VOID *)&Var);
    if (EFI_ERROR(Status)) {       

#if FASTBOOT_NEED_RESTART    
 {
     EFI_STATUS               RestartStatus;
     EFI_STATUS               RestartCountStatus;
     
     RestartStatus = gRT->GetVariable(FAST_BOOT_RESTART,
                            &FastBootRestartGuid,
                            NULL,
                            &RebootFlagSize,
                            (VOID *)&RebootFlag
                            );
        
     RestartCountStatus = gRT->GetVariable(FAST_BOOT_RESTART_COUNT,
                            &FastBootRestartCountGuid,
                            NULL,
                            &RebootCountSize,
                            (VOID *)&RebootCount
                            );
        if (!EFI_ERROR(RestartStatus) && !EFI_ERROR(RestartCountStatus))
        {
            DEBUG((DEBUG_VERBOSE,"FB: ReBootFlag:%d\n",RebootFlag));
            DEBUG((DEBUG_VERBOSE,"FB: ReBootCount:%d\n",RebootCount));

            if (RebootFlag == 1 && RebootCount < 1 )
                FastBootEnableRestart(RebootFlag, RebootCount + 1);
        }
        else
            FastBootEnableRestart(ResetFlag, ClearCount + 1);
 }
#endif                       

        ///variable not found - create a new one
        
        ///get first actived boot option
        Status = GetEfiVariable(L"BootOrder", &gEfiGlobalVariableGuid, NULL, &Size, &BootOrder);
        if (EFI_ERROR(Status)) {
            gBS->CloseEvent(Event);  
            return;        
        }

        for (BootOptionCounter = 0; BootOptionCounter < Size / sizeof(UINT16); BootOptionCounter++, BootOption=NULL) {    ///(EIP96276.4)

            Swprintf(BootOptionName, gBootName, BootOrder[BootOptionCounter]);
           
            Status = GetEfiVariable(BootOptionName, &gEfiGlobalVariableGuid, NULL, &BootOptionSize, &BootOption);    ///(EIP96276.4)
            if (EFI_ERROR(Status)) {
                gBS->CloseEvent(Event);  
                return;
            }

            VaildBootOption = IsValidFbBootOptionPtr(BootOptionCounter, BootOption);
            if (VaildBootOption) break;
            
            gBS->FreePool(BootOption);
            
            if (VaildBootOption == FALSE && BootOptionCounter == (Size / sizeof(UINT16) - 1 )) {
                DEBUG((DEBUG_ERROR, "FB: There is no valid boot option\n"));
                gBS->FreePool(BootOrder);
                gBS->CloseEvent(Event);  
                return;
            }
        }

        Var.BootOptionNumber = BootOrder[BootOptionCounter];
        Var.BootCount = 0;
{
        CHAR16                   *Description;
        EFI_DEVICE_PATH_PROTOCOL *Dp;
        
        ///Retrieve device path
        Description = (CHAR16 *)(BootOption + 1);
        Dp = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8*)Description + (Wcslen(Description) + 1) * sizeof(CHAR16));

        if (Dp->Type == BBS_DEVICE_PATH) {
            Var.BootType =  FAST_BOOT_TYPE_LEGACY;
            Status = CreateLegacyFastBootOption(&Var);
            DEBUG((DEBUG_INFO,"FB: Create FastBoot Legacy Boot Option %r\n", Status));                                
        } else {
            Var.BootType = FAST_BOOT_TYPE_EFI;  
            Status = CreateEfiFastBootOption(BootOption, BootOptionSize, BootOrder[BootOptionCounter]);    ///(EIP96276.4)
            DEBUG((DEBUG_INFO,"FB: Create FastBoot EFI Boot Option %r\n", Status));                        
        }
}
        gBS->FreePool(BootOrder);
        gBS->FreePool(BootOption);

        if (EFI_ERROR(Status)) {
            gBS->CloseEvent(Event);  
            return;
        }
///
/// Collect SATA Port&Device information
///
#if SUPPORT_RAID_DRIVER
        CollectSataDeviceInfo(&Var);
#endif
        
    } else {
        Var.BootCount++;
        Var.BootOptionNumber = gFastBootPolicy->BootOptionNumber;
    }
   
    DEBUG((DEBUG_INFO,"FB: Create LastBoot Variable\n"));  
    gRT->SetVariable(L"LastBoot",
                     &FastBootVariableGuid,
                     EFI_VARIABLE_NON_VOLATILE | 
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | 
                     EFI_VARIABLE_RUNTIME_ACCESS,
                     sizeof(Var),
                     (VOID *)&Var);
    gBS->CloseEvent(Event);  
}

/**
 * Connect Legacy FastBoot Device.
 *    
 * @retval EFI_SUCCESS                    Connect legacy boot device success.
 * @retval EFI_NOT_FOUND                Connect legacy boot device fail or device type not support.
 */

EFI_STATUS 
ConnectFastLegacyBootDevice()
{
#if CSM_SUPPORT 
    EFI_STATUS Status;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    BOOLEAN UsbMassStorage = FALSE;
    BOOLEAN HddFilePath = FALSE;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios;
    UINT16 HddCount;
    UINT16 BbsCount;
    HDD_INFO *HddInfo;
    BBS_TABLE *BbsTable;
    EFI_HANDLE  Handle;
    UINT8 Counter;                              ///(EIP63924+)
    
    EFI_DISK_INFO_PROTOCOL *DiskInfo;
    EFI_GUID    DiskInfoGuid = EFI_DISK_INFO_PROTOCOL_GUID;
    IDENTIFY_DATA              IdentifyData;
    UINT32             IdentifyDataSize;
    UINT8   *DetectedHddSerialNum = NULL;
    UINTN   SerialNumberSize;
    UINT8   *LastHddSerialNum = NULL;
    EFI_GUID    DiskInfoIdeGuid = EFI_DISK_INFO_IDE_INTERFACE_GUID;
    EFI_GUID    DiskInfoAhciGuid = EFI_DISK_INFO_AHCI_INTERFACE_GUID; 
    
    DEBUG((DEBUG_INFO, "FB: Connect Fast LegacyBoot Device\n"));     
     
    Dp = gFastBootPolicy->FastBootOption;
    if (!IsSupportedDevice(Dp, &UsbMassStorage, &HddFilePath))         ///CD-ROM legacy boot not supported
        return EFI_NOT_FOUND;

    if ( UsbMassStorage && gFastBootPolicy->UsbSupport == 0)
        return EFI_NOT_FOUND;  ///If skip USB enable, don't boot into USB mass storage     
        
    if (!IsSataDevicePath(Dp) && !IsLegacyRaidDevicePath(Dp))
    {
        DEBUG((DEBUG_INFO, "FB: Dp connecting\n"));
        ConnectDevicePath(Dp);
    }

    Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &Dp, &Handle);
    if (EFI_ERROR(Status))
        return Status;
    DEBUG((DEBUG_VERBOSE, "FB: Dp connected Handle %x\n",Handle));
        
    LegacyBootDeviceHandle = Handle;
        
    Status = gBS->HandleProtocol(Handle, &gEfiBlockIoProtocolGuid, (VOID**)&BlockIo);
    if (EFI_ERROR(Status))
        return Status;
    DEBUG((DEBUG_VERBOSE, "FB: BlockIo found\n"));

    Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
    if (EFI_ERROR(Status)) 
        return Status;
    DEBUG((DEBUG_VERBOSE, "FB: Legacybios discovered\n"));

                                        ///(EIP63924)>
    /// check FastBootModeChange
    for (Counter = 0; FastBootModeChange[Counter]; Counter++) 
        if (FastBootModeChange[Counter]())return EFI_NOT_FOUND;
                                        ///<(EIP63924)
        
    Status = LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);  ///install int 13
    if (EFI_ERROR(Status)) return EFI_NOT_FOUND;

    if (gFastBootPolicy->SataSupport != 2) {
        Status = gBS->HandleProtocol(Handle, &DiskInfoGuid, (VOID**)&DiskInfo);
        if (!EFI_ERROR(Status)) {
            /// Only check serial number in ide or ahci mode.
            if (!guidcmp(&DiskInfo->Interface, &DiskInfoIdeGuid) || !guidcmp(&DiskInfo->Interface,&DiskInfoAhciGuid)) {
                Status = gBS->AllocatePool(EfiBootServicesData, sizeof(UINT8) * 20, &DetectedHddSerialNum);
                if (EFI_ERROR(Status)) return Status;
                IdentifyDataSize = sizeof(IDENTIFY_DATA);
                Status = DiskInfo->Identify(DiskInfo, &IdentifyData, &IdentifyDataSize);
                if (EFI_ERROR(Status)) {
                    gBS->FreePool(DetectedHddSerialNum);
                    return Status;
                }
                SerialNumberSize = sizeof(UINT8) * 20;
                MemCpy(DetectedHddSerialNum, IdentifyData.Serial_Number_10, SerialNumberSize);
                Status = GetEfiVariable(L"FastBootHddSerial", &FastBootVariableGuid, NULL, &SerialNumberSize, &LastHddSerialNum);     
                if (EFI_ERROR(Status)) {   
                    gBS->FreePool(DetectedHddSerialNum);
                    return Status;
                }
                if (MemCmp(DetectedHddSerialNum, LastHddSerialNum, SerialNumberSize)) {
                    DEBUG((DEBUG_ERROR,"DB: Serial number check fail\n"));
                    gBS->FreePool(DetectedHddSerialNum);
                    return EFI_NOT_FOUND;
                }
            }        
        }
    }
    
    if (DetectedHddSerialNum != NULL) {
        gBS->FreePool(DetectedHddSerialNum);
    }
#endif 
    return EFI_SUCCESS;
}

/**
 * FastBoot with legacy device.
 *    
 * @retval EFI_DEVICE_ERROR                Legacy boot fail.
 */

EFI_STATUS 
EFIAPI 
FastLegacyBoot(VOID)
{
#if CSM_SUPPORT 
    EFI_STATUS Status;
    UINT8 Index;    
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios;
    UINT16 HddCount;
    UINT16 BbsCount;
    HDD_INFO *HddInfo;
    BBS_TABLE *BbsTable;
    EFI_EVENT  FastBootLegacyBootEvent;
    UINT8   *DeviceString;
    UINT8   Counter;
    UINT16   StringCheckSum;

    DEBUG((DEBUG_INFO, "FB: Fast legacy boot started\n"));

                                        ///(EIP87390+)>
    gBS->RaiseTPL( TPL_HIGH_LEVEL );    /// guarantees that RestoreTPL won't ASSERT
    gBS->RestoreTPL( TPL_APPLICATION );
                                        ///<(EIP87390+)
    Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
    if (EFI_ERROR(Status)) 
        return Status;
    DEBUG((DEBUG_VERBOSE, "FB: Legacybios discovered\n"));
        
    LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);  //install int 13

                                        ///(EIP87390+)>
    ///Set USB device BootPriority as BBS_DO_NOT_BOOT_FROM, Install USB INT13 service for TCG Bitlocker Driver
    for (Index = 0; Index < BbsCount; Index++){       

                                        ///(EIP103422+)>
        /// if BBS table priority have been arranged already 
        /// increase one priority to all prioritized BBS table entry
        if (BbsTable[Index].BootPriority < BBS_DO_NOT_BOOT_FROM) { 
            BbsTable[Index].BootPriority = BbsTable[Index].BootPriority + 1;
            continue;
        }
                                        ///<(EIP103422+)         

        /// If BBS table priority have not been arranged yet, set device 
        /// priority as BBS_DO_NOT_BOOT_FROM.
        /// It means don't boot from this device but BIOS still need to install
        /// INT13 service for it.

        if (BbsTable[Index].BootPriority != BBS_IGNORE_ENTRY)
            BbsTable[Index].BootPriority = BBS_DO_NOT_BOOT_FROM;                    
            
    }
                                        ///<(EIP87390+)

    for (Index=0;Index<BbsCount;Index++){
    
        if (BbsTable[Index].IBV1 == (UINT32)LegacyBootDeviceHandle) break;
    
        if (Index == (BbsCount - 1)) return Status; ///can't find fast device in bbstable 
    }
    
    if (gFastBootPolicy->CheckDevStrCheckSum) {
        
        DeviceString = (UINT8*)((UINTN)((UINTN)BbsTable[Index].DescStringSegment << 4) + BbsTable[Index].DescStringOffset);
        StringCheckSum = 0;    
        for (Counter = 0; Counter < 50; Counter++) {        
            if (DeviceString[Counter] == 0) break;
            StringCheckSum += DeviceString[Counter];
        }        
         
        if (StringCheckSum != gFastBootPolicy->DevStrCheckSum) {
            return EFI_DEVICE_ERROR;
        }
  
    }

    BbsTable[Index].BootPriority = 0;

    Status = CreateLegacyBootEvent(TPL_NOTIFY,
                                    FastBootClearBootFailFlag,
                                    NULL,
                                    &FastBootLegacyBootEvent);
    if (EFI_ERROR(Status)) 
        return Status; 

    DEBUG((DEBUG_INFO,"FB: LEGACY BOOT Singal Ready to Boot\n"));

    ReadyToBoot(0xffff);

                                        ///(EIP68329)>
    if ((SKIP_TSE_HANDSHAKE == 0) && (TSE_MAJOR >= 0x02) && (TSE_MINOR >= 0x10)) {            
    } else {
        gFastBootTseProtocol->FastBootPasswordFreezeDevices();
    }
                                        ///<(EIP68329)

///Set Runtime as TRUE before pass control to OS.
    Runtime = TRUE;

    LegacyBios->LegacyBoot(LegacyBios, 0, 0, 0);
    
    return Status;
#else
    return EFI_SUCCESS;
#endif    
}

/**
 * This function use file system protocol to checks if OS loader present on given device.
 *    
 * @param[in]   Handle        Block io handle, used to get file system protocol. 
 * @param[in]   Dp                Pointer to device path of bootable device.
 *
 * @retval EFI_SUCCESS    OS loader exist.
 */

EFI_STATUS 
CheckLoaderEx( 
    IN EFI_HANDLE               Handle,
    IN EFI_DEVICE_PATH_PROTOCOL *Dp
)
{
    EFI_STATUS  Status;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SimpleFileSystem = NULL;
    EFI_FILE_PROTOCOL *FileProtocol = NULL;
    EFI_FILE_PROTOCOL* NewFileProtocol = NULL;
    FILEPATH_DEVICE_PATH   *BootOptionFilePath = NULL;

    for ( ; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp)) {
        if (Dp->Type == MEDIA_DEVICE_PATH && Dp->SubType == MEDIA_FILEPATH_DP) { 
            BootOptionFilePath = (FILEPATH_DEVICE_PATH*) (Dp);
            break;
        }
    }
    if (BootOptionFilePath == NULL) {
        return EFI_DEVICE_ERROR;
    }

    Status = gBS->HandleProtocol (
            Handle,
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID**)&SimpleFileSystem
        );
    if (EFI_ERROR(Status)) {          
        return Status;
    }

    Status = SimpleFileSystem->OpenVolume (
            SimpleFileSystem,
            &FileProtocol
        );
    if (EFI_ERROR(Status)) {          
        return Status;
    }

    Status = FileProtocol->Open (
                FileProtocol,
                &NewFileProtocol,
                BootOptionFilePath->PathName,
                EFI_FILE_MODE_READ,
                0
            );
    if (EFI_ERROR(Status)) {          
        return Status;
    }

    NewFileProtocol->Close(NewFileProtocol);
    
    return Status;
}

/**
 * Connect UEFI FastBoot Device.
 *    
 *
 * @retval EFI_SUCCESS    Connect UEFI boot device success.
 */

EFI_STATUS 
ConnectFastEfiBootDevice()
{
    EFI_STATUS  Status;
    EFI_DEVICE_PATH_PROTOCOL *Dp = gFastBootPolicy->FastBootOption;
    EFI_HANDLE  Handle;    
    UINT8   Counter;                          ///(EIP63924+)
    
    DEBUG((DEBUG_INFO, "FB: Connect Fast EFIBoot Device\n"));
    
    if (IsBootToShell(Dp))
        InstallFwLoadFile();
    else {
        if (!IsSataDevicePath(Dp))
        {  
            DEBUG((DEBUG_INFO, "FB: Dp connecting\n"));
            ConnectDevicePath(Dp);    
        }   

        Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &Dp, &Handle);
        if (EFI_ERROR(Status)) return Status;
        DEBUG((DEBUG_VERBOSE, "FB: Dp connected Handle %x\n",Handle));
        
        Status = CheckLoaderEx(Handle, gFastBootPolicy->FastBootOption);        
        if (EFI_ERROR(Status)) {
            DEBUG((DEBUG_ERROR, "FB: Check loader %r\n",Status));            
            return Status;
        }

    }
                                           ///(EIP63924)>
    for (Counter = 0; FastBootModeChange[Counter]; Counter++) 
        if (FastBootModeChange[Counter]()) return EFI_NOT_FOUND;
                                        ///<(EIP63924)

    return EFI_SUCCESS;
}

/**
 * Clears the screen for shell boot.
 *    
 * @param[in]   Event                The Event that is being processed.
 * @param[in]   Context            The Event Context.
 *
 */

VOID 
EFIAPI 
_ShellClearScreen (EFI_EVENT Event, VOID *Context)
{
    gST->ConOut->ClearScreen (gST->ConOut);
    gBS->CloseEvent (Event);
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
}

/**
 * Registers the shell guid.
 *    
 *
 */

VOID 
_RegisterShellGuid (VOID)
{
    EFI_STATUS     Status;
    VOID         *Registration = NULL;
    EFI_GUID     EfiShellInterfaceGuid = EFI_SHELL_PROTOCOL_GUID;
    EFI_EVENT   ShellLaunchEvent;
    Status = gBS->CreateEvent (
                EFI_EVENT_NOTIFY_SIGNAL, 
                TPL_CALLBACK,
                _ShellClearScreen,
                NULL,
                &ShellLaunchEvent);
    if (!EFI_ERROR (Status))
    {
        gBS->RegisterProtocolNotify(
                &EfiShellInterfaceGuid,
                ShellLaunchEvent,
                &Registration
                );
    }
}

/**
 * UEFI FastBoot path.
 *    
 *    @retval Boot fail if return a value.
 *
 */

EFI_STATUS 
EFIAPI 
FastEfiBoot(VOID) 
{
    EFI_STATUS Status;
    UINTN DpSize;
    UINT32 LoadOptionsSize;
    VOID *LoadOptions;
    EFI_LOADED_IMAGE_PROTOCOL *Image;

    DEBUG((DEBUG_INFO, "FB: Fast EFI boot started\n"));

    DpSize = DPLength(gFastBootPolicy->FastBootOption);
    LoadOptionsSize = *(UINT32 *)((UINT8 *)gFastBootPolicy->FastBootOption + DpSize);
    LoadOptions = (VOID *)((UINT8 *)gFastBootPolicy->FastBootOption + DpSize + sizeof(UINT32));                         

    /// this *MUST* be run a EFI_TPL_APPLICATION
    gBS->RaiseTPL( TPL_HIGH_LEVEL );    /// guarantees that RestoreTPL won't ASSERT (EIP87390)+
    gBS->RestoreTPL( TPL_APPLICATION );

    DEBUG((DEBUG_INFO,"FB: EFI BOOT Signal Ready to Boot\n"));

///
///  ReadyToBoot have signaled already in TSE path.
///
    if (gFastBootPolicy->SkipTseHandshake)
        ReadyToBoot(gFastBootPolicy->BootOptionNumber);

    
    Status = gBS->LoadImage(TRUE, ThisImageHandle, gFastBootPolicy->FastBootOption, NULL, 0, &EfiBootImageHanlde);

    DEBUG((DEBUG_INFO,"FB: load image %r\n",Status));
    
    if (EFI_ERROR(Status)) return Status;        
    if (LoadOptionsSize != 0) {
        Status = gBS->HandleProtocol(EfiBootImageHanlde, &gEfiLoadedImageProtocolGuid, (VOID**)&Image);
        if (EFI_ERROR(Status)) 
            return Status;

        Image->LoadOptionsSize = LoadOptionsSize;   
        Image->LoadOptions = LoadOptions;
    }    
                                        ///(EIP68329)>
    if ((gFastBootPolicy->SkipTseHandshake == 0) && (TSE_MAJOR >= 0x02) && (TSE_MINOR >= 0x10)) {            
    } else {
        gFastBootTseProtocol->FastBootPasswordFreezeDevices();
    }
                                        ///<(EIP68329)
    PERF_END(0, "FastBoot", "FB",0);  
    PERF_END(0, "TSE", "FB",0);            
#ifdef EFI_DXE_PERFORMANCE
    SavePerformanceData(NULL, NULL);
#endif

    FastBootClearBootFailFlag(NULL, NULL);

                                        ///(EIP60794+)>
    if (gST->ConOut != NULL) {
        gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    }

    _RegisterShellGuid();    
                                        ///<(EIP60794+)
                                        
    DEBUG((DEBUG_INFO,"FB: EFI BOOT start image\n"));        
///enabld usb mass driver, so that OS have chance to connect usb mass storage.
#if SKIP_USB_STORAGE
    if (gUsbPolicyProtocol != NULL) 
        gUsbPolicyProtocol->UsbDevPlcy->UsbMassDriverSupport = TRUE;
#endif
///Set Runtime as TRUE before pass control to OS.
    Runtime = TRUE;

    Status = gBS->StartImage(EfiBootImageHanlde, NULL, NULL);
     
    return Status;     
}

/**
 * This function creates Legacy fast boot option and save it to NVRAM.
 *    
 * @param[in]   Var                            The Variable contains the fast boot information  for next boot.
 *
 * @retval EFI_SUCCESS                Boot option setting done.
 * @retval EFI_UNSUPPORTED        Boot option is not fastboot support type.
 * 
 */

EFI_STATUS 
EFIAPI 
CreateLegacyFastBootOption ( 
  IN FAST_BOOT2 *Var )
{
#if CSM_SUPPORT
    EFI_STATUS Status;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios;
    UINT16 HddCount;
    UINT16 BbsCount;
    HDD_INFO *HddInfo;
    BBS_TABLE *BbsTable;
    EFI_HANDLE Handle;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    UINT8 BootOption[200];
    UINTN DpSize;
    UINT8 Counter;
    UINT8 Index = 0;                    ///(EIP73019+)
    UINT16 Priority = 0xfffc;
    UINT8   *DeviceString;
    UINT16   StringCheckSum = 0;

    EFI_DISK_INFO_PROTOCOL *DiskInfo;
    EFI_GUID    DiskInfoGuid = EFI_DISK_INFO_PROTOCOL_GUID;
    IDENTIFY_DATA              IdentifyData;
    UINT32             IdentifyDataSize;
    UINT8   *DetectedHddSerialNum = NULL;
    UINTN   SerialNumberSize;
    EFI_GUID    DiskInfoIdeGuid = EFI_DISK_INFO_IDE_INTERFACE_GUID;
    EFI_GUID    DiskInfoAhciGuid = EFI_DISK_INFO_AHCI_INTERFACE_GUID; 
    
    Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
    if (EFI_ERROR(Status))
        return Status;

    LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);

///find record with highest priority
    for (Counter = 0; Counter < BbsCount; Counter++) {
        if (BbsTable[Counter].BootPriority < Priority) {
            Priority = BbsTable[Counter].BootPriority;
            Index = Counter;
        }
    }

    if (BbsTable[Index].DeviceType != BBS_HARDDISK)
        return EFI_UNSUPPORTED;

    Handle = (EFI_HANDLE) *(UINTN*)(&(BbsTable[Index].IBV1));
    Status = gBS->HandleProtocol(Handle, &gEfiDevicePathProtocolGuid, (VOID**)&Dp);
    if (EFI_ERROR(Status))
        return Status;

    ///save device string check sum    
    DeviceString = (UINT8*)((UINTN)((UINTN)BbsTable[Index].DescStringSegment << 4) + BbsTable[Index].DescStringOffset);
    
    for (Counter = 0; Counter< 50; Counter++) {        
        if (DeviceString[Counter] == 0) break;
        StringCheckSum += DeviceString[Counter];
    }

    Var->DevStrCheckSum = StringCheckSum;

    DpSize = DPLength(Dp);
    MemCpy(BootOption, Dp, DpSize);

    Status = gRT->SetVariable(L"FastBootOption",
                              &FastBootVariableGuid,
                              EFI_VARIABLE_NON_VOLATILE | 
                              EFI_VARIABLE_BOOTSERVICE_ACCESS | 
                              EFI_VARIABLE_RUNTIME_ACCESS,
                              DpSize,
                              BootOption);
    if (EFI_ERROR(Status)) return Status;
    
    if (gFastBootPolicy->SataSupport != 2) {
        Status = gBS->HandleProtocol(Handle, &DiskInfoGuid, (VOID**)&DiskInfo);
        if (!EFI_ERROR(Status)) {
            if (!guidcmp(&DiskInfo->Interface, &DiskInfoIdeGuid) || !guidcmp(&DiskInfo->Interface,&DiskInfoAhciGuid)) {
                SerialNumberSize = sizeof(UINT8) * 20;
                Status = gBS->AllocatePool(EfiBootServicesData, SerialNumberSize, &DetectedHddSerialNum);
                if (EFI_ERROR(Status)) return Status;
                IdentifyDataSize = sizeof(IDENTIFY_DATA);
                Status = DiskInfo->Identify(DiskInfo, &IdentifyData, &IdentifyDataSize);
                if (EFI_ERROR(Status)) {
                    gBS->FreePool(DetectedHddSerialNum);
                    return Status;
                }
                MemCpy(DetectedHddSerialNum, IdentifyData.Serial_Number_10, SerialNumberSize);
                Status = gRT->SetVariable(L"FastBootHddSerial",
                                  &FastBootVariableGuid,
                                  EFI_VARIABLE_NON_VOLATILE | 
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS | 
                                  EFI_VARIABLE_RUNTIME_ACCESS,
                                  SerialNumberSize,
                                  DetectedHddSerialNum);
                gBS->FreePool(DetectedHddSerialNum);   
            }
        }
    }
    
    return Status;
#else
    return EFI_SUCCESS;    
#endif 
}

/**
 * This function detects if FastBoot is supported for given device.
 *    
 * @param[in]     Dp                    Pointer to device path of bootable device.
 * @param[out]    UsbMassStorage        TRUE if device is USB device, FALSE otherwise.
 * @param[out]    HddFilePath           TRUE if device is HDD device, FALSE otherwise.
 *
 * @retval TRUE                         FastBoot is supported for given device.
 * @retval FALSE                        FastBoot is not supported for given device.
 * 
 */
BOOLEAN 
EFIAPI 
IsSupportedDevice(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp,
    OUT BOOLEAN *UsbMassStorage,
    OUT BOOLEAN *HddFilePath )
{

    for ( ; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp)) {
        if (Dp->Type == BBS_DEVICE_PATH)
            return FALSE;

        if (Dp->Type == MESSAGING_DEVICE_PATH) {
            if (Dp->SubType == MSG_USB_DP) 
                *UsbMassStorage = TRUE;
    
            if (Dp->SubType == MSG_MAC_ADDR_DP)
                return FALSE;        

            continue;
        }

        if (Dp->Type == MEDIA_DEVICE_PATH) {
            
            if (Dp->SubType == MEDIA_HARDDRIVE_DP) {                
                Dp = NEXT_NODE(Dp);
                ///check whether there is a file path behind hard drive path.
                if (Dp->SubType == MEDIA_FILEPATH_DP)
                    *HddFilePath = TRUE;  

                return TRUE;
            }

            if (Dp->SubType == MEDIA_CDROM_DP)
                return FALSE;              
        }
    }
    
    return TRUE;    
}


/**
 * This function creates UEFI fast boot option and save it to NVRAM.
 *    
 * @param[in]   BootOption            Pointer to regular boot option to boot to. 
 * @param[in]   OptionSize                Size of option additional parameters.
 * @param[in]   OptionNumber        Number of boot option in BootOrder variable.
 *
 * @retval EFI_SUCCESS                Create efi boot option success.
 * @retval EFI_UNSUPPORTED        Boot option is not fastboot support type or 
 *                                                     boot loader doesn't exist.
 */

EFI_STATUS 
EFIAPI 
CreateEfiFastBootOption(
  IN EFI_LOAD_OPTION *BootOption,
  IN UINTN           OptionSize,
  IN UINT16          OptionNumber
)
{
    EFI_STATUS Status;
    UINTN Size;
    UINTN DpSize;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    CHAR16 *Description;
    UINT32 LoadOptionsSize;
    VOID *LoadOptions;
    UINT8 *FastBootOption;
    VOID *SavePtr;
    BOOLEAN UpdatedDp = FALSE;
    BOOLEAN UsbMassStorage = FALSE;
    BOOLEAN HddFilePath = FALSE;
    EFI_HANDLE  Handle;  

///retrieve device path
    Description = (CHAR16 *)(BootOption + 1);
    Dp = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8*)Description + (Wcslen(Description) + 1) * sizeof(CHAR16));
    
    if (!IsSupportedDevice(Dp, &UsbMassStorage,&HddFilePath))
        return EFI_UNSUPPORTED;

    ///only hard drive or USB can come here

    LoadOptions = (UINT8*)Dp + BootOption->FilePathListLength;
    LoadOptionsSize = (UINT32)((UINT8*)BootOption + OptionSize - (UINT8 *)LoadOptions);

    if (Dp->Type == MEDIA_DEVICE_PATH && Dp->SubType == MEDIA_HARDDRIVE_DP) {    ///Windowns boot manager?
{
        EFI_DEVICE_PATH_PROTOCOL *PartitionDp;
        
        PartitionDp = DiscoverPartition(Dp);
        if (PartitionDp == NULL)
            return EFI_UNSUPPORTED;
        Dp = DPAdd(PartitionDp, NEXT_NODE(Dp));
        UpdatedDp = TRUE;
}
    } else if (Dp->Type == MEDIA_DEVICE_PATH && Dp->SubType == MEDIA_VENDOR_DP) { /// bulit-in shell?

    } else if (HddFilePath == FALSE) { /// add file path for SATA HDD or USB HDD without file path
        EFI_DEVICE_PATH_PROTOCOL *TempDp;
        
        Dp = DPAddNode(Dp, &FilePathNode.Header);
        TempDp = Dp;
        
        Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &TempDp, &Handle);
        if (EFI_ERROR(Status)) {
            gBS->FreePool(TempDp);
            return Status;
        }
        Status = CheckLoaderEx(Handle, TempDp);
        if (EFI_ERROR(Status)) {
            gBS->FreePool(Dp);
            return EFI_UNSUPPORTED;
        }
        UpdatedDp = TRUE;
    } 

    DpSize = DPLength(Dp);

/// prepare data
    Size = DpSize + sizeof(UINT32) + LoadOptionsSize;
    Status = gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&FastBootOption);
    if (EFI_ERROR(Status)) {
        gBS->FreePool(Dp);
        return EFI_UNSUPPORTED;
    }
    SavePtr = FastBootOption;
    MemCpy(FastBootOption, Dp, DpSize);
    FastBootOption += DpSize;
    *(UINT32 *)FastBootOption = LoadOptionsSize;
    FastBootOption += sizeof(UINT32);
    MemCpy(FastBootOption, LoadOptions, LoadOptionsSize);

    Status = gRT->SetVariable(L"FastBootOption", 
                              &FastBootVariableGuid,
                              EFI_VARIABLE_NON_VOLATILE | 
                              EFI_VARIABLE_BOOTSERVICE_ACCESS | 
                              EFI_VARIABLE_RUNTIME_ACCESS,
                              Size,
                              SavePtr);

    gBS->FreePool(SavePtr);
    if (UpdatedDp)
        gBS->FreePool(Dp);
    return Status;
}

/**
 * This function execute e-links "ReturnNormalMode".
 *    
 */

VOID 
EFIAPI 
ReturnToNormalBoot(VOID)
{
    UINT8   Counter;
    
    for (Counter = 0; ReturnNormalModeHook[Counter]; Counter++)
        ReturnNormalModeHook[Counter]();
}

/**
 * This function resets FastBoot variable if FastBoot path failed.
 *    
 */

VOID 
EFIAPI 
ResetFastBootVariable(VOID)
{
    FAST_BOOT2 Var;

    Runtime = FALSE;

///clear LastBootFailed variable if present
    FastBootClearBootFailFlag(NULL, NULL);

    if (!gFastBootPolicy->LastBootVarPresence) return;

    gRT->SetVariable(L"LastBoot", 
                     &FastBootVariableGuid,
                     EFI_VARIABLE_NON_VOLATILE,
                     0,
                     &Var);
}

/**
 * This function resets skip table for Usb driver and disconnect 
 * all usb controllers.
 *    
 */

VOID 
EFIAPI 
DisconnectUsbController(VOID)
{
    EFI_STATUS    Status;
    EFI_HANDLE *Handle;    
    UINTN Number, HandleCounter;    
#if AMIUSB_SUPPORT
    EFI_GUID EfiUsbProtocolGuid = EFI_USB_PROTOCOL_GUID;
    EFI_USB_PROTOCOL *UsbProtocol = NULL;
    USB_GLOBAL_DATA   *UsbData = NULL;
#endif

#if AMIUSB_SUPPORT
    if (gFastBootPolicy->UsbSupport != 2) return;
    
    ///Restore Usb storage driver support
    if (gUsbPolicyProtocol != NULL) 
        gUsbPolicyProtocol->UsbDevPlcy->UsbMassDriverSupport = BackupUsbMassDriverSupport;
    
    if (FbUsbSkipTableIsSet == FALSE) return;
          
    Status = gBS->LocateProtocol( &EfiUsbProtocolGuid, \
                                      NULL, \
                                      (VOID**)&UsbProtocol );
    if (EFI_ERROR(Status)) return;

    ///restore backup skip table pointer
    UsbData = (USB_GLOBAL_DATA*)UsbProtocol->USBDataPtr;    
    UsbData->gUsbSkipListTable = BackupSkipTable;
#endif

    ///Get a list of all USB Controllers
    Status = FbGetPciHandlesByClass(
        PCI_CL_SER_BUS, PCI_CL_SER_BUS_SCL_USB, &Number, &Handle
    );
    if (EFI_ERROR(Status)) return;
    
    for (HandleCounter = 0; HandleCounter < Number; HandleCounter++)
    {
        gBS->DisconnectController(Handle[HandleCounter], NULL, NULL);
    }
    gBS->FreePool(Handle);     
}

/**
 * This function checks if configuration was changed since last boot.
 *    
 *
 * @retval TRUE        Configuration was changed to fast boot flow.
 * @retval FALSE    Configuration was not changed to fast boot flow.
 */

BOOLEAN 
EFIAPI 
OemConfigurationChanged(VOID)
{
    EFI_STATUS Status;
    UINT32 BootFlow;
    UINTN Size = sizeof(UINT32);

    Status = gRT->GetVariable(L"BootFlow", &GuidBootFlow, NULL, &Size, &BootFlow);

    return (!EFI_ERROR(Status) && BootFlow == BOOT_FLOW_CONDITION_FIRST_BOOT) ? TRUE : FALSE;
}

/**
 * FastBoot clear boot fail flag callback.
 *    
 * @param[in]   Event                The Event that is being processed.
 * @param[in]   Context            The Event Context.
 *
 */

VOID 
EFIAPI 
FastBootClearBootFailFlag (
  IN EFI_EVENT Event, 
  IN VOID *Context )
{
    EFI_STATUS Status;
    UINT32 LastBootFailed;
    UINTN Size = sizeof(UINT32);

    Status = gRT->GetVariable(L"LastBootFailed", &FastBootVariableGuid, NULL, &Size, &LastBootFailed);
    if (!EFI_ERROR(Status)) {
        gRT->SetVariable(L"LastBootFailed", 
                                  &FastBootVariableGuid, 
                                  EFI_VARIABLE_NON_VOLATILE,
                                  0,
                                  &LastBootFailed);
    }
}

/**
 * This function FastBoot is enabled or disabled by checking  elink "IsFastBootList".
 *    
 *
 * @retval TRUE                    Fast boot is enabled.
 * @retval FALSE                Fast boot is not enabled.
 */

BOOLEAN 
EFIAPI 
IsFastBoot(VOID)
{
    static EFI_GUID SetupVariableGuid = SETUP_GUID;    
    EFI_STATUS  Status;
    UINTN         Size;
    BOOLEAN Result = TRUE;
    UINTN   Counter;
    EFI_GUID FastBootPolicyGuid = FAST_BOOT_POLICY_PROTOCOL2_GUID;

    Status = gBS->LocateProtocol(&FastBootPolicyGuid, NULL, (VOID**)&gFastBootPolicy);
    if (EFI_ERROR(Status) || gFastBootPolicy->FastBootEnable == FALSE)
        return FALSE;

    Size = sizeof(FbSetupData);
    Status = gRT->GetVariable(L"Setup", &SetupVariableGuid, NULL, &Size, &FbSetupData);
    if (EFI_ERROR(Status)) return FALSE;


    /// check IsFastBootList
    for (Counter = 0; IsFastBootList[Counter] && Result; Counter++)
        Result = IsFastBootList[Counter](&FbSetupData);


    if (!Result)
        ReturnToNormalBoot();    ///return to normal boot

    return Result;
}

/**
 * This function returns the status of Fast boot setup option.
 *    
 * @param[in]   FbSetupData        Point of SetupData. 
 *
 * @retval TRUE                            Fast boot is enabled.
 * @retval FALSE                        Fast boot is not enabled.
 */

BOOLEAN 
EFIAPI 
IsFastBootDefault (
  IN SETUP_DATA *FbSetupData 
)
{
    UINT16        BootNext;
    UINTN         Size;
    EFI_STATUS    Status;
    EFI_BOOT_MODE BootMode;
    BOOLEAN       FastBoot;

    if (!gFastBootPolicy->FastBootEnable) return FALSE;

    BootMode = GetBootMode();
    
    if (BootMode == BOOT_WITH_MINIMAL_CONFIGURATION) 
    return TRUE;        

    if (!gFastBootPolicy->FirstFastBootInS4 && BootMode == BOOT_ON_S4_RESUME) {
        if (gFastBootPolicy->LastBootVarPresence)
            return (gFastBootPolicy->BootCount > 0) ? TRUE : FALSE;        
    }

///Check for "BootNext" variable
    Size = sizeof(BootNext);
    Status = gRT->GetVariable(L"BootNext", &gEfiGlobalVariableGuid, NULL, &Size, &BootNext);
    if (!EFI_ERROR(Status)) {
        FastBoot = FALSE;
    } else {
        FastBoot = gFastBootPolicy->FastBootEnable;
    }

    return FastBoot;
}

/**
 * Changea Setup Boot Flow.
 *    
 * @param[in]   BootFlow                Boot flow data want to change. 
 *
 * @retval EFI_SUCCESS                Set variable success.
 */

EFI_STATUS 
ChangeSetupBootFlow (
    IN UINT32 BootFlow 
)
{
    EFI_STATUS  Status;
    EFI_GUID GuidBootFlow = BOOT_FLOW_VARIABLE_GUID;    

    Status = gRT->SetVariable(L"BootFlow", 
                               &GuidBootFlow, 
                               EFI_VARIABLE_BOOTSERVICE_ACCESS,
                               sizeof(BootFlow),
                               &BootFlow);
    return Status;
}

/**
 * This function executes FastBoot path via AMI TSE boot manager.
 *    
 *
 * @retval EFI_ABORTED        Fast boot via AMI TSE cannot be executed.
 */

EFI_STATUS 
EFIAPI 
TseHandshake()
{
    EFI_STATUS Status;
    AMI_POST_MANAGER_PROTOCOL *AmiPostMgr;
    EFI_HANDLE Handle = NULL;

    Status = gBS->LocateProtocol(&AmiPostMgrProtocolGuid, NULL, (VOID**)&AmiPostMgr);
    if (EFI_ERROR(Status)) return Status;

    if ( gFastBootPolicy->UefiBoot == TRUE)
        FastBootProtocol.Launch =  FastEfiBoot;
    else
        FastBootProtocol.Launch =  FastLegacyBoot;

    Status = gBS->InstallMultipleProtocolInterfaces(
                                &Handle,
                                &gAmiFastBootProtocol2Guid,
                                &FastBootProtocol,
                                NULL);
    if (EFI_ERROR(Status)) return Status;

    Status = ChangeSetupBootFlow(BOOT_FLOW_CONDITION_FAST_BOOT);
    if (EFI_ERROR(Status)) return Status;
    
    DEBUG((DEBUG_INFO,"FB: TseHandshake\n"));
        
    AmiPostMgr->Handshake();

///if we're here fast boot failed, change Boot flow back to BOOT_FLOW_CONDITION_NORMAL

    ChangeSetupBootFlow(BOOT_FLOW_CONDITION_NORMAL);

    return EFI_ABORTED;
}

/**
 * Return the status system in runtime or not.
 *    
 *
 * @retval TRUE                        System is in runtime.
 * @retval FALSE                    System is in Post.
 */

BOOLEAN 
EFIAPI 
IsRuntime(VOID)
{
    return Runtime;
}

/**
 * Connect console in device by ConIn variable.
 *    
 *
 */

VOID 
EFIAPI 
FbConnectInputDevices(VOID)
{
    ConnectConInVariable();
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
