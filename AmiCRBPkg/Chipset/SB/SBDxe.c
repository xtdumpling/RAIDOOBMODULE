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
/** @file SBDXE.C
    This file contains code for Template Southbridge initialization
    in the DXE stage.
*/

// Module specific Includes
#include <Efi.h>
#include <token.h>
#include <Setup.h>
#include <Dxe.h>
#include <AmiLib.h>
#include <AmiCspLib.h>

// Consumed Protocols
#include <Protocol\PciIo.h>
#include <Protocol\Cpu.h>
#include <Protocol\PciRootBridgeIo.h>
#include "Protocol\DevicePath.h"
#include "protocol\DriverBinding.h"
#include <Protocol\ComponentName.h>

// Support OEM USB per port disable function.
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#include <Protocol/AmiUsbController.h>
#include <Protocol/UsbPolicy.h>
#include <Include/Register/PchRegsLpc.h>
#endif

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
#include <Protocol/UsbIo.h>
#include <Library/MmPciBaseLib.h>
#endif

#include <Library/PchInfoLib.h>

#include <Guid/SetupVariable.h>
#include <Library/S3BootScriptLib.h>
#include <Library\PciLib.h>
#include <Platform.h>

#include <Include/Guid/PchRcVariable.h>  //  PCH Setup variables are moved to PCH_RC_CONFIGURATION
#include <Register/PchRegsUsb.h> //  Folder structure changed in 0.48
#include <Register/PchRegsPmc.h> //  Folder structure changed in 0.48

#include <Library/PciLib.h>

#define ICH_RCRB_FD2        0x3428 // Function Disable Register 2
#define PCILIB_TO_COMMON_ADDRESS(Address) \
        ((UINT64) ((((UINTN) ((Address>>20) & 0xff)) << 24) + (((UINTN) ((Address>>15) & 0x1f)) << 16) + (((UINTN) ((Address>>12) & 0x07)) << 8) + ((UINTN) (Address & 0xfff ))))
//----------------------------------------------------------------------------
// Constant definitions
//----------------------------------------------------------------------------
#define ONBOARD_RAID_GUID \
    { 0x5d206dd3, 0x516a, 0x47dc, 0xa1, 0xbc, 0x6d, 0xa2, 0x4, 0xaa, 0xbe, 0x8};

#define SB_SIGNATURE '__SB'
//----------------------------------------------------------------------------
//   Variable Prototypes
//----------------------------------------------------------------------------
typedef struct {
    UINT8 Bus;
    UINT8 Dev;
    UINT8 Fun;
    VOID *Process;
} DEVICES_AFTER_PCIIO;

DEVICES_AFTER_PCIIO gDevicesTable[] = {
    { 0x00, 0x1F, 0x02, NULL },
    { 0x00, 0x11, 0x04, NULL },// In RAID mode, the HDD info cannot be shown in SATA/sSATA Configuration page.
    { 0xFF, 0xFF, 0xFF, NULL }
};

//----------------------------------------------------------------------------
//   Variable Declaration
//----------------------------------------------------------------------------
// GUID Definitions
EFI_GUID gOnboardRaidGuid = ONBOARD_RAID_GUID;

UINTN gEventCount = sizeof(gDevicesTable) / sizeof(DEVICES_AFTER_PCIIO);
VOID *gSbInitNotifyReg = NULL;

EFI_RUNTIME_SERVICES *gRT;
EFI_BOOT_SERVICES *gBS;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo;
EFI_EVENT gEvtBootScript;
EFI_EVENT gEvtLegacyBoot;

// Support OEM USB per port disable function.
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
BOOLEAN                         gDisableAllUsbControllers = FALSE;
EFI_EVENT                       gEvtUsbProtocol  = NULL;
VOID                            *gRegUsbProtocol = NULL;
EFI_USB_PROTOCOL                *gUsbProtocol = NULL;

#define ACPI_PATH_MACRO \
 {{ACPI_DEVICE_PATH,ACPI_DP,ACPI_DEVICE_PATH_LENGTH}, EISA_PNP_ID(0x0A03),0}

#define PCI_PATH_MACRO(Device,Function) \
 {{HARDWARE_DEVICE_PATH, HW_PCI_DP, HW_PCI_DEVICE_PATH_LENGTH}, (Function),(Device)}

#define END_PATH_MACRO \
 {END_DEVICE_PATH,END_ENTIRE_SUBTYPE,END_DEVICE_PATH_LENGTH}

typedef struct
{
 ACPI_HID_DEVICE_PATH      Acpi;
 PCI_DEVICE_PATH           Pci;
 EFI_DEVICE_PATH_PROTOCOL  End;
} SMM_USB_DEVICE_PATH;

SMM_USB_DEVICE_PATH gXhciDevicePath[] = 
{
    {
        ACPI_PATH_MACRO,
        PCI_PATH_MACRO(0x14, 0),
        END_PATH_MACRO
    }
};

AMI_USB_HW_SMI_HC_CONTEXT gXhciHwSmiContext[] =
{
    USB_HC_XHCI,
    (EFI_DEVICE_PATH_PROTOCOL*)gXhciDevicePath
};
#endif

//----------------------------------------------------------------------------
// Function Prototypes
//----------------------------------------------------------------------------
VOID SbCallbackReadyToBoot(IN EFI_EVENT Event, IN VOID *Context);
VOID SbCallbackLegacyBoot(IN EFI_EVENT Event, IN VOID *Context);

EFI_STATUS
SBSupported(

    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  EFI_HANDLE                      Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
);

EFI_STATUS
SBStart(

    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
);

EFI_STATUS
SBStop(

    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     Controller,
    IN  UINTN                          NumberOfChildren,
    IN  EFI_HANDLE                     *ChildHandleBuffer
);

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
VOID USBPerPortDisableCallback (
    IN EFI_EVENT    Event,
    IN VOID *Context
);
#endif

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
VOID SbUsbProtocolCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);
#endif

CHAR16  *gSBDriverName         = L"AMI SB Driver";

CHAR16  *gSBControllerName     = L"SB Controller";

EFI_DRIVER_BINDING_PROTOCOL     gSBDriverBindingProtocol =
{

    SBSupported,
    SBStart,
    SBStop,
    1,                      // Version
    0,                   // Image handle
    0                    // Driver Binding Handle
};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SBDXE_Init
//
// Description: This function is the entry point for this DXE. This function
//              initializes the chipset SB
//
// Input: ImageHandle Image handle
//        SystemTable Pointer to the system table
//
// Output: Return Status based on errors that occurred while waiting for
//         time to expire.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SBDXE_Init(
    IN EFI_HANDLE  ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{

    EFI_STATUS                              Status;
    VOID                                    *Registration = NULL;
#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
#if CSM_SUPPORT 
    EFI_EVENT                               LegacyBootEvent;
#endif
    EFI_EVENT                               Event;
#endif
    
    
    InitAmiLib(ImageHandle, SystemTable);
    
    PROGRESS_CODE(DXE_SB_INIT);

    Status = CreateReadyToBootEvent(TPL_NOTIFY, SbCallbackReadyToBoot, 0, &gEvtBootScript );
    ASSERT_EFI_ERROR(Status);

    Status = CreateLegacyBootEvent(TPL_NOTIFY, SbCallbackLegacyBoot, 0, &gEvtLegacyBoot );
    ASSERT_EFI_ERROR(Status);

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
    Status = RegisterProtocolCallback( &gEfiUsbProtocolGuid,\
                                       SbUsbProtocolCallback,\
                                       NULL,\
                                       &gEvtUsbProtocol,\
                                       &gRegUsbProtocol );
    
    pBS->SignalEvent(gEvtUsbProtocol);
#endif

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1 
    RegisterProtocolCallback(
        &gAmiTseEventBeforeBootGuid,
        USBPerPortDisableCallback,
        NULL, &Event, &Registration
    );
	
#if CSM_SUPPORT
    CreateLegacyBootEvent(TPL_CALLBACK, &USBPerPortDisableCallback, NULL, &LegacyBootEvent);
#endif
#endif

    gSBDriverBindingProtocol.ImageHandle         = ImageHandle;
    gSBDriverBindingProtocol.DriverBindingHandle = ImageHandle;

    Status = pBS->InstallMultipleProtocolInterfaces(&ImageHandle,
             &gEfiDriverBindingProtocolGuid, &gSBDriverBindingProtocol,
             0);

    return EFI_SUCCESS;
}


// Support OEM USB per port disable function.
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
VOID CheckDisableUsbControllers(VOID)
{
    EFI_STATUS Status;
    EFI_GUID   EfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE;
    UINTN      BootOrderSize = 0;
    UINT16     *BootOrder = NULL;
    
    Status = pBS->LocateProtocol( &gEfiUsbProtocolGuid,
                                  NULL,
                                  &gUsbProtocol );
    if (EFI_ERROR(Status)) return;
    
    if (gDisableAllUsbControllers){
        Status = pRS->GetVariable( L"BootOrder", \
                                   &EfiGlobalVariableGuid, \
                                   NULL, \
                                   &BootOrderSize, \
                                   &BootOrder );
        if (Status == EFI_NOT_FOUND) return;
        
        // Shutdown legacy
        gUsbProtocol->UsbRtShutDownLegacy();
        
        PciWrite16(PCI_LIB_ADDRESS (PCI_BUS_NUMBER_PCH_XHCI, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI, PCI_COMMAND_OFFSET), 0);
        
        MmioOr32((PCH_PWRM_BASE_ADDRESS + R_PCH_PWRM_NST_PG_FDIS_1), B_PCH_PWRM_NST_PG_FDIS_1_XHCI_FDIS_PMC);
    }
}
#endif



#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
/**
    This callback function is called after USB Protocol is 
    installed.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID SbUsbProtocolCallback (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context )
{
    EFI_STATUS    Status = EFI_SUCCESS;
    USB_SKIP_LIST SkipMassTable[] = { {1, 0, 0xff, 0, 0, 0x8},
                                      {0, 0, 0,    0, 0, 0  }
                                    };
    EFI_USB_POLICY_PROTOCOL *EfiUsbPolicyProtocol;	
    PCH_RC_CONFIGURATION    PchRcConfiguration;
    UINTN                   VarSize;    
    
#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1	
    PCH_SERIES 		PchSeries = GetPchSeries();
    UINT8           Port = 0, IndexPort = 0, SsPortIndex;
    USB_SKIP_LIST   *SkipOemMassTable; 
    USB_SKIP_LIST   *SklSkipMassTable;
    
    USB_SKIP_LIST SkipPchHUSBMassTable[] = {
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 1, 0, 0x8},  // USB_P1
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 2, 0, 0x8},  // USB_P2
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 3, 0, 0x8},  // USB_P3
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 4, 0, 0x8},  // USB_P4
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 5, 0, 0x8},  // USB_P5
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 6, 0, 0x8},  // USB_P6
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 7, 0, 0x8},  // USB_P7
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 8, 0, 0x8},  // USB_P8             
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 9, 0, 0x8},  // USB_P9
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 10, 0, 0x8}, // USB_P10
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 11, 0, 0x8}, // USB_P11
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 12, 0, 0x8}, // USB_P12
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 13, 0, 0x8}, // USB_P13
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 14, 0, 0x8}, // USB_P14
            
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x11, 0, 0x8}, // USB3_P1
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x12, 0, 0x8}, // USB3_P2
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x13, 0, 0x8}, // USB3_P3
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x14, 0, 0x8}, // USB3_P4
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x15, 0, 0x8}, // USB3_P5
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x16, 0, 0x8}, // USB3_P6
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x17, 0, 0x8}, // USB3_P7
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x18, 0, 0x8}, // USB3_P8
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x19, 0, 0x8}, // USB3_P9
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x1A, 0, 0x8}  // USB3_P10               
    };
    
    USB_SKIP_LIST SkipPchLpUSBMassTable[] = {
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 1, 0, 0x8},  // USB_P1
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 2, 0, 0x8},  // USB_P2
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 3, 0, 0x8},  // USB_P3
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 4, 0, 0x8},  // USB_P4
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 5, 0, 0x8},  // USB_P5
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 6, 0, 0x8},  // USB_P6
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 7, 0, 0x8},  // USB_P7
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 8, 0, 0x8},  // USB_P8             
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 9, 0, 0x8},  // USB_P9
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 10, 0, 0x8}, // USB_P10
            
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 13, 0, 0x8}, // USB3_P1
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 14, 0, 0x8}, // USB3_P2
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 15, 0, 0x8}, // USB3_P3
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 16, 0, 0x8}, // USB3_P4
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 17, 0, 0x8}, // USB3_P5
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 18, 0, 0x8}  // USB3_P6             
    };
#endif

    VarSize = sizeof (PchRcConfiguration);
    Status = pRS->GetVariable (
        L"PchRcConfiguration",
        &gEfiPchRcVariableGuid,
        NULL,
        &VarSize,
        &PchRcConfiguration
        );
    ASSERT_EFI_ERROR(Status);

    if (PchStepping () > LbgA0) {
      Status = pBS->LocateProtocol(
		      &gEfiUsbPolicyProtocolGuid,
		      NULL,
		      &EfiUsbPolicyProtocol);
      ASSERT_EFI_ERROR(Status);

   
      Status = pBS->AllocatePool (EfiBootServicesData, 1 * sizeof(AMI_USB_HW_SMI_HC_CONTEXT*), 
		      (VOID**)&EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcContext);

      EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcContext[0] = gXhciHwSmiContext;

      EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcCount = 1;
    }

    Status = pBS->LocateProtocol( &gEfiUsbProtocolGuid,
                                  NULL,
                                  &gUsbProtocol );
    ASSERT_EFI_ERROR(Status);
    
    if ( gDisableAllUsbControllers ) {
        gUsbProtocol->UsbCopySkipTable(SkipMassTable, sizeof(SkipMassTable)/sizeof (USB_SKIP_LIST));
        pBS->CloseEvent(Event);
        return;
    }

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1   
    //Report the SKIP USB port table for USB per-port disable
    
    if(PchRcConfiguration.PchUsbPerPortCtl == 1){	   
        if (PchSeries == PchH) {
            SkipOemMassTable = MallocZ(sizeof(SkipPchHUSBMassTable) + sizeof (USB_SKIP_LIST));
            SklSkipMassTable = SkipPchHUSBMassTable;
        } else {
            SkipOemMassTable = MallocZ(sizeof(SkipPchLpUSBMassTable) + sizeof (USB_SKIP_LIST));
            SklSkipMassTable = SkipPchLpUSBMassTable;
        }
        ASSERT(SkipOemMassTable);

        // For USB per-port disable
        for (Port = 0; Port < GetPchUsbMaxPhysicalPortNum(); Port++) {
            if (PchRcConfiguration.PchUsbHsPort[Port] == PCH_DEVICE_DISABLE) {
                SkipOemMassTable[IndexPort++] = SklSkipMassTable[Port];
                DEBUG ((EFI_D_INFO, "Disable USB EHCI Port = %x\n", Port));
            }
        }
        
        SsPortIndex = GetPchUsbMaxPhysicalPortNum();
        
        // For USB3 per-port disable
        for (Port = 0; Port < GetPchXhciMaxUsb3PortNum(); Port++) {
            if (PchRcConfiguration.PchUsbSsPort[Port] == PCH_DEVICE_DISABLE) {
                SkipOemMassTable[IndexPort++] = SklSkipMassTable[Port + SsPortIndex];
                DEBUG ((EFI_D_INFO, "Disable USB XHCI Port = %x\n", Port));
            }
        }        

        if ( IndexPort != 0 ) {
            gUsbProtocol->UsbCopySkipTable(SkipOemMassTable, IndexPort +1);
        }

        pBS->FreePool( SkipOemMassTable );
    }
#endif

    pBS->CloseEvent(Event);
}
#endif




#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
UINT8 GetSbUsbMaxPhysicalPortNum ( VOID )
{
    PCH_SERIES  PchSeries;

    PchSeries = GetPchSeries();
    switch (PchSeries) {
      case PchLp:
        return PCH_LP_XHCI_MAX_USB2_PHYSICAL_PORTS;

      case PchH:
        return PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS;

      default:
        return 0;
    }
}

/**
    This function can disable USB PerPort before OS booting.             

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/
VOID USBPerPortDisableCallback (
    IN EFI_EVENT    Event,
    IN VOID *Context
)
{
    PCH_SERIES                  PchSeries = GetPchSeries();
    UINT8                       XhciPortDisableFlage[26];
    UINT32                      Index;
    UINT32                      XhciUsb2Pdo = 0;
    UINT32                      XhciUsb3Pdo = 0;
    UINT32                      XhciSspeReg = 0;    
    UINT64                      XhciBar;
    static BOOLEAN              USBPerPortDisableDone = FALSE;
    PCH_RC_CONFIGURATION        PchRcConfiguration; 
    UINTN                       VarSize;    
    UINTN                       XhciMemBarReg;
    EFI_STATUS                  Status = EFI_SUCCESS;

    //Make sure the processing is performed only once. 
    if (USBPerPortDisableDone) {
        pBS->CloseEvent(Event);
        return;
    }

    DEBUG ((EFI_D_INFO, __FUNCTION__" - Start\n"));
    VarSize = sizeof (PchRcConfiguration);
    Status = pRS->GetVariable (
                    L"PchRcConfiguration",
                    &gEfiPchRcVariableGuid,
                    NULL,
                    &VarSize,
                    &PchRcConfiguration
                    );
    
    // Read back Xhci MMIO addrss
    XhciMemBarReg = (UINTN)MmPciAddress( \
		    0, \
		    PCI_BUS_NUMBER_PCH_XHCI, \
		    PCI_DEVICE_NUMBER_PCH_XHCI, \
		    PCI_FUNCTION_NUMBER_PCH_XHCI, \
		    R_PCH_XHCI_MEM_BASE);

    if ((MmioRead32(XhciMemBarReg) & 0x6) == 0x4){
        XhciBar = (((UINT64) MmioRead32(XhciMemBarReg + 4)) << 32) | 
                  ((UINT64) MmioRead32(XhciMemBarReg) & (~0x0F));
    } else {
        XhciBar = MMIO_READ32(XhciMemBarReg) & (~0x0F);
    }
    DEBUG ((EFI_D_INFO, "Xhci Bar = %lx\n", XhciBar));
    
    // Initial Xhci Port Disable Flags
    // for PCH-LP
    // Index: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
    // SS/HS: HS HS HS HS HS HS HS HS HS HS HS HS SS SS SS SS SS SS
    // for PCH-H
    // SS/HS: HS HS HS HS HS HS HS HS HS HS HS HS HS HS HS HS SS SS SS SS SS SS SS SS SS SS
    for (Index=0;Index<26;Index++) XhciPortDisableFlage[Index] = 0;
    
// Intel_RC >>>
        ///
        /// XHCI PDO for HS
        ///
        if (PchSeries == PchLp) {
            XhciUsb2Pdo = MmioRead32 (XhciBar + R_PCH_XHCI_USB2PDO) & B_PCH_XHCI_LP_USB2PDO_MASK;
        } else {
            XhciUsb2Pdo = MmioRead32 (XhciBar + R_PCH_XHCI_USB2PDO) & B_PCH_XHCI_H_USB2PDO_MASK;
        }
        
        for (Index = 0; Index < GetSbUsbMaxPhysicalPortNum (); Index++) {
            if (PchRcConfiguration.PchUsbHsPort[Index] == 1) {
                XhciUsb2Pdo &= (UINT32)~(B_PCH_XHCI_USB2PDO_DIS_PORT0 << Index);
            } else {
                XhciUsb2Pdo |= (UINT32) (B_PCH_XHCI_USB2PDO_DIS_PORT0 << Index); ///< A '1' in a bit position prevents the corresponding USB2 port from reporting a Device Connection to the XHC
                XhciPortDisableFlage[Index] |= 1;
            }
        }
        
        ///
        /// XHCI PDO for SS
        ///
        if (PchSeries == PchLp) {
          XhciUsb3Pdo = MmioRead32 (XhciBar + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_LP_USB3PDO_MASK;
          //XhciSspeReg = MmioRead32 (XhciBar + R_PCH_XHCI_SSPE) & B_PCH_XHCI_LP_SSPE_MASK;
        } else {
          XhciUsb3Pdo = MmioRead32 (XhciBar + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_H_USB3PDO_MASK;
          //XhciSspeReg = MmioRead32 (XhciBar + R_PCH_XHCI_SSPE) & B_PCH_XHCI_H_SSPE_MASK;
        }

        if (PchRcConfiguration.PchDciEn == 0) {
          for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {
              if (PchRcConfiguration.PchUsbSsPort[Index] == 1) {
                  XhciUsb3Pdo &= (UINT32)~(B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index);
              } else {
                  XhciUsb3Pdo |= (UINT32) (B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index); ///< A '1' in a bit position prevents the corresponding USB3 port from reporting a Device Connection to the XHC

                  if (PchSeries == PchLp) XhciPortDisableFlage[Index + 12] |= 1;
                  else XhciPortDisableFlage[Index + 16] |= 1;
              }
          }

          if (PchSeries == PchLp) {
              XhciSspeReg = (~XhciUsb3Pdo) & B_PCH_XHCI_LP_SSPE_MASK;
          } else {
              XhciSspeReg = (~XhciUsb3Pdo) & B_PCH_XHCI_H_SSPE_MASK;
          }
        } else {
            //
            // Doc #560605
            // 4773006: xHCI Warm Reset to Unused USB 3.0 port Issue.
            // When USB 3.0 Hosting Direct Connect Interface (DCI) is enabled, BIOS should not set any of the USB3PDO bits.
            //
            if (PchSeries == PchLp) {
              XhciUsb3Pdo &= (UINT32)~B_PCH_XHCI_LP_USB3PDO_MASK;
              XhciSspeReg |= (UINT32) B_PCH_XHCI_LP_SSPE_MASK;
            } else {
              XhciUsb3Pdo &= (UINT32)~B_PCH_XHCI_H_USB3PDO_MASK;
              XhciSspeReg |= (UINT32) B_PCH_XHCI_H_SSPE_MASK;
            }
        }

        ///
        /// USB2PDO and USB3PDO are Write-Once registers and bits in them are in the SUS Well.
        ///
        DEBUG ((EFI_D_INFO, "Write back Xhci HS PDO value: %x to HS PDO register\n", XhciUsb2Pdo));
        MmioWrite32 (XhciBar + R_PCH_XHCI_USB2PDO, XhciUsb2Pdo);
        DEBUG ((EFI_D_INFO, "Write back Xhci SS PDO value: %x to SS PDO register\n", XhciUsb3Pdo));
        MmioWrite32 (XhciBar + R_PCH_XHCI_USB3PDO, XhciUsb3Pdo);
        DEBUG ((EFI_D_INFO, "Write back Xhci SSPE value: %x to SSPE register\n", XhciSspeReg));        
        MmioWrite32 (XhciBar + R_PCH_XHCI_SSPE, XhciSspeReg);

// Intel_RC <<<
    
// Disable usb port under Xhci controller >>>
{
    UINT32                      counter;
    UINT32                      RegVal;
    
    // Disable Xhci port which are disconnected
    for(Index=0; Index<26; Index++){
        if (XhciPortDisableFlage[Index] != 0){
            DEBUG ((EFI_D_INFO, "Disable port%d under Xhci controller(start number:1)\n", (Index + 1)));
            if (PchSeries ==PchLp){
                // for PCH-LP
                // Index: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
                // SS/HS: HS HS HS HS HS HS HS HS HS HS HS HS SS SS SS SS SS SS
                DEBUG ((EFI_D_INFO, "This is PCH-LP sku\n"));
                if (Index > 17) break;
                
                if (Index < 12){
                    //HS port
                    RegVal = MmioRead32(XhciBar + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Index);
                    if (RegVal & B_PCH_XHCI_PORTSCXUSB2_PED){
                        //RegVal = (B_PCH_XHCI_PORTSCXUSB2_PED | B_PCH_XHCI_PORTSCXUSB2_PP);    //(EIP242242-)
                        RegVal = (B_PCH_XHCI_PORTSCXUSB2_PR | B_PCH_XHCI_PORTSCXUSB2_PP);       //(EIP242242+)
                        MmioWrite32((XhciBar + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Index), RegVal);
                        for(counter=0;counter<200;counter++){
                            RegVal = MmioRead32(XhciBar + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Index);
                            if(~(RegVal & BIT0)) break;  // Connect status bit 0 = 0
                            //if(!(RegVal & B_PCH_XHCI_PORTSCXUSB2_PED)) break; //(EIP242242-)
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB2_PRC) break;   //(EIP242242+)
                            pBS->Stall(100);
                        } // for loop
                    } // PED bit is enable
                } else {
                    //SS port
                    RegVal = MmioRead32(XhciBar + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * (Index - 12));
                    if (RegVal & B_PCH_XHCI_PORTSCXUSB3_PED){
                        RegVal = (B_PCH_XHCI_PORTSCXUSB3_WPR | B_PCH_XHCI_PORTSCXUSB3_PP);
                        MmioWrite32((XhciBar + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * (Index - 12)), RegVal);
                        for(counter=0;counter<3000;counter++){
                            RegVal = MmioRead32(XhciBar + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * (Index - 12));
                            if(~(RegVal & BIT0)) break;  // Connect status bit 0 = 0
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB3_WRC) break;
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB3_PRC) break;
                            pBS->Stall(100);
                        } // for loop
                        
                        // Clear Warm Port Reset Change and Port Reset Change bits
                        //RegVal = (B_PCH_XHCI_PORTSCXUSB3_WRC | B_PCH_XHCI_PORTSCXUSB3_PRC | B_PCH_XHCI_PORTSCXUSB3_PP);
                        //MmioWrite32((XhciBar + R_PCH_LP_XHCI_PORTSC1USB3 + 0x10 * Index), RegVal);
                    } // PED bit is enable
                }// SS/HS port
            } else {
                // for PCH-H
                // Index: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
                // SS/HS: HS HS HS HS HS HS HS HS HS HS HS HS HS HS HS HS SS SS SS SS SS SS SS SS SS SS
                DEBUG ((EFI_D_INFO, "This is PCH-H sku\n"));
                if (Index < 16){
                    //HS port
                    RegVal = MmioRead32(XhciBar + R_PCH_H_XHCI_PORTSC01USB2 + 0x10 * Index);
                    if (RegVal & B_PCH_XHCI_PORTSCXUSB2_PED){
                        //RegVal = (B_PCH_XHCI_PORTSCXUSB2_PED | B_PCH_XHCI_PORTSCXUSB2_PP);    //(EIP242242-)
                        RegVal = (B_PCH_XHCI_PORTSCXUSB2_PR | B_PCH_XHCI_PORTSCXUSB2_PP);       //(EIP242242+)
                        MmioWrite32((XhciBar + R_PCH_H_XHCI_PORTSC01USB2 + 0x10 * Index), RegVal);
                        for(counter=0;counter<200;counter++){
                            RegVal = MmioRead32(XhciBar + R_PCH_H_XHCI_PORTSC01USB2 + 0x10 * Index);
                            if(~(RegVal & BIT0)) break;  // Connect status bit 0 = 0
                            //if(!(RegVal & B_PCH_XHCI_PORTSCXUSB2_PED)) break; //(EIP242242-)
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB2_PRC) break;   //(EIP242242+)
                            pBS->Stall(100);
                        } // for loop
                    } // PED bit is enable
                } else {
                    //SS port
                    RegVal = MmioRead32(XhciBar + R_PCH_H_XHCI_PORTSC01USB3 + 0x10 * (Index - 16));
                    if (RegVal & B_PCH_XHCI_PORTSCXUSB3_PED){
                        RegVal = (B_PCH_XHCI_PORTSCXUSB3_WPR | B_PCH_XHCI_PORTSCXUSB3_PP);
                        MmioWrite32((XhciBar + R_PCH_H_XHCI_PORTSC01USB3 + 0x10 * (Index - 16)), RegVal);
                        for(counter=0;counter<3000;counter++){
                            RegVal = MmioRead32(XhciBar + R_PCH_H_XHCI_PORTSC01USB3 + 0x10 * (Index - 16));
                            if(~(RegVal & BIT0)) break;  // Connect status bit 0 = 0
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB3_WRC) break;
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB3_PRC) break;
                            pBS->Stall(100);
                        } // for loop
                        
                        // Clear Warm Port Reset Change and Port Reset Change bits
                        //RegVal = (B_PCH_XHCI_PORTSCXUSB3_WRC | B_PCH_XHCI_PORTSCXUSB3_PRC | B_PCH_XHCI_PORTSCXUSB3_PP);
                        //MmioWrite32((XhciBar + R_PCH_H_XHCI_PORTSC1USB3 + 0x10 * Index), RegVal);
                    } // PED bit is enable
                } // SS/HS port
            } // PCH sku
        } // XhciPortDisableFlage[counter] != 0
    } // for loop
}
// Disable usb port under Xhci controller <<<

    USBPerPortDisableDone = TRUE;
    DEBUG ((EFI_D_INFO, __FUNCTION__" - End\n"));
    pBS->CloseEvent(Event);
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SbCallbackLegacyBoot
//
// Description: SB call back function before legacy boot.
//
// Input: IN EFI_EVENT Event
//        IN VOID  *Context
//
// Output: None
//
// Notes:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SbCallbackLegacyBoot(IN EFI_EVENT Event, IN VOID *Context)
{

    pBS->CloseEvent(Event);           
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SbCallbackReadyToBoot
//
// Description: SB call back function before boot.
//
// Input: IN EFI_EVENT Event
//        IN VOID   *Context
// Output: None
//
// Notes:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SbCallbackReadyToBoot(IN EFI_EVENT Event, IN VOID *Context)
{	
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
    CheckDisableUsbControllers();
#endif	

    pBS->CloseEvent(Event); 
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SBSupported
//
// Description: Checks whether the device/controller is a SB device (other than IDE)
//
// Input:       *This           Pointer to the EFI_DRIVER_BINDING_PROTOCOL instance
//              Controller      The handle of a controller that the driver specified by
//                              This is managing.  This handle specifies the controller
//                              that needs a driver
//              *RemainingDevicePath    Device path of the controller
//
// Output:      EFI_STATUS
//              EFI_SUCCES              The Unicode string for the Driver specified by This
//                                      and the language specified by Language was returned
//                                      in DriverName
//              EFI_INVALID_PARAMETER   Language is NULL
//              EFI_INVALID_PARAMETER   DriverName is NULL
//              EFI_UNSUPPORTED         The driver specified by This does not support the
//                                      language specified by Language
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
SBSupported(
    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  EFI_HANDLE                      Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
)
{
//####  UINT8                       PciConfig[0x40];
    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIO;
    UINTN         seg, bus, dev, fun;

    // Check whether PCI Protocol has been installed on this controller
    Status = pBS->OpenProtocol(Controller, &gEfiPciIoProtocolGuid, (VOID **) & PciIO,
                               This->DriverBindingHandle, Controller, EFI_OPEN_PROTOCOL_BY_DRIVER);
    if (EFI_ERROR(Status))
        return Status;

//####    // Read the PCI Config Space
//####    Status = PciIO->Pci.Read (PciIO, EfiPciIoWidthUint8, 0, sizeof (PciConfig), PciConfig );
//####    if (EFI_ERROR(Status)) {
//####        pBS->CloseProtocol (Controller, &gEfiPciIoProtocolGuid, This->DriverBindingHandle, Controller);
//####        return EFI_UNSUPPORTED;
//####    }
//####
//####   // Porting require for chipset SMBUS device
//####    // Check whether the PCI Controller is IDE or NOT?
//####    if ( PciConfig[0x2] != 0x30                 ||
//####         PciConfig[0x3] != 0x3a                 ||
//####        PciConfig[0x0] != (0x86  )     ||
//####         PciConfig[0x1] != (0x80  )    ) {
//####        pBS->CloseProtocol (Controller, &gEfiPciIoProtocolGuid, This->DriverBindingHandle, Controller);
//####        return EFI_UNSUPPORTED;
//####    }

    Status = PciIO->GetLocation(PciIO, &seg, &bus, &dev, &fun);

    //  Close PciIoProtocol and return SUCCESS
    pBS->CloseProtocol(Controller, &gEfiPciIoProtocolGuid, This->DriverBindingHandle, Controller);

    if (EFI_ERROR(Status))
        return EFI_UNSUPPORTED;

    return EFI_UNSUPPORTED;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SBStart
//
// Description: This function is called to start this controller driver
//
// Input:       *This           Pointer to the EFI_DRIVER_BINDING_PROTOCOL instance
//              Controller      The handle of a controller that the driver specified by
//                              This is managing.  This handle specifies the controller
//                              whose driver has to be started
//              *RemainingDevicePath    Device path of the controller
//
// Output:      EFI_SUCCESS     Always
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
SBStart(
    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIO;
//  EFI_DEVICE_PATH_PROTOCOL    *ParentDevicePath;
    UINT8                       PciConfig[0x40];

    //  Check whether PCI Protocol has been installed on this controller
    Status = pBS->OpenProtocol(Controller, &gEfiPciIoProtocolGuid, (VOID **) & PciIO,
                               This->DriverBindingHandle, Controller, EFI_OPEN_PROTOCOL_BY_DRIVER);
    if (EFI_ERROR(Status))
        return Status;

    //  Read the PCI Config Space
    Status = PciIO->Pci.Read(PciIO, EfiPciIoWidthUint8, 0, sizeof(PciConfig), PciConfig);
    if (EFI_ERROR(Status))
    {
        pBS->CloseProtocol(Controller, &gEfiPciIoProtocolGuid, This->DriverBindingHandle, Controller);
        return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SBStop
//
// Description: This function is called to stop this controller driver
//
// Input:       *This           Pointer to the EFI_DRIVER_BINDING_PROTOCOL instance
//              Controller      The handle of a controller that the driver specified by
//                              This is managing.  This handle specifies the controller
//                              whose driver has to be stopped
//              NumberOfChildren    Number of children associated with this driver
//              *ChildHandleBuffer  Pointers of all the children handles
//
// Output:      EFI_SUCCESS     Always
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
SBStop(
    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     Controller,
    IN  UINTN                          NumberOfChildren,
    IN  EFI_HANDLE                     *ChildHandleBuffer
)
{
    return      EFI_SUCCESS;
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
