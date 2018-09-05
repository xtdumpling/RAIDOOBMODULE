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

/** @file AmiUsb.h
    AMI UsbRt driver definitions

**/

#ifndef _EFI_USB_H
#define _EFI_USB_H

#include <Token.h>

#include <Protocol/DevicePath.h>
#if !USB_RT_DXE_DRIVER
#include <Protocol/SmmBase.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmUsbDispatch2.h>
#endif
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/AmiUsbController.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/BlockIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/ComponentName.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/UsbPolicy.h>
#include <Protocol/Emul6064KbdInput.h>
#include <Protocol/Emul6064MsInput.h>
#include <Protocol/Emul6064Trap.h>
#include "UsbDef.h"

EFI_STATUS
EFIAPI
USBDriverEntryPoint(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );


#if !USB_RT_DXE_DRIVER
EFI_STATUS
EFIAPI
USBSWSMIHandler(
		IN EFI_HANDLE	DispatchHandle,
		IN CONST VOID	*Context OPTIONAL,
		IN OUT VOID		*CommBuffer OPTIONAL,
		IN OUT UINTN	*CommBufferSize OPTIONAL
);

EFI_STATUS
EFIAPI
UhciHWSMIHandler(
		IN EFI_HANDLE	DispatchHandle,
		IN CONST VOID	*Context OPTIONAL,
		IN OUT VOID		*CommBuffer OPTIONAL,
		IN OUT UINTN	*CommBufferSize OPTIONAL
);

EFI_STATUS
EFIAPI
OhciHWSMIHandler(
		IN EFI_HANDLE	DispatchHandle,
		IN CONST VOID	*Context OPTIONAL,
		IN OUT VOID		*CommBuffer OPTIONAL,
		IN OUT UINTN	*CommBufferSize OPTIONAL
);

EFI_STATUS
EFIAPI
EhciHWSMIHandler(
		IN EFI_HANDLE	DispatchHandle,
		IN CONST VOID	*Context OPTIONAL,
		IN OUT VOID		*CommBuffer OPTIONAL,
		IN OUT UINTN	*CommBufferSize OPTIONAL
);

EFI_STATUS
EFIAPI
XhciHwSmiHandler(
		IN EFI_HANDLE	DispatchHandle,
		IN CONST VOID	*Context OPTIONAL,
		IN OUT VOID		*CommBuffer OPTIONAL,
		IN OUT UINTN	*CommBufferSize OPTIONAL
);

EFI_STATUS
EFIAPI
Emul6064TrapCallback(
        IN CONST EFI_GUID  *Protocol,
        IN VOID            *Interface,
        IN EFI_HANDLE      Handle
);
#endif

VOID		EFIAPI UsbApiHandler(VOID*);
EFI_STATUS	InstallUsbProtocols(VOID);

EFI_STATUS	UsbInstallHwSmiHandler(HC_STRUC*);

UINT8   ByteReadIO (UINT16);
VOID    ByteWriteIO (UINT16, UINT8);
UINT32  ReadPCIConfig(UINT16, UINT8);
VOID    WordWritePCIConfig(UINT16, UINT8, UINT16);
UINT32  DwordReadMem(UINT32, UINT16);
VOID USBAPI_CheckPresence(URP_STRUC*);          // API 00h
VOID USBAPI_Start(URP_STRUC*);                  // API 20h
VOID USBAPI_Stop(URP_STRUC*);                   // API 21h
VOID USBAPI_DisableInterrupts(URP_STRUC*);      // API 22h
VOID USBAPI_EnableInterrupts(URP_STRUC*);       // API 23h
VOID USBAPI_MoveDataArea(URP_STRUC*);           // API 24h
VOID USBAPI_GetDeviceInfo(URP_STRUC*);          // API 25h
VOID USBAPI_CheckDevicePresence(URP_STRUC*);    // API 26h
VOID USBAPI_MassDeviceRequest(URP_STRUC*);      // API 27h
VOID USBAPI_PowerManageUSB(URP_STRUC*);         // API 28h
VOID USBAPI_PrepareForOS(URP_STRUC*);           // API 29h
VOID USBAPI_SecureInterface(URP_STRUC*);        // API 2Ah
VOID USBAPI_LightenKeyboardLEDs(URP_STRUC*);    // API 2Bh
VOID USBAPI_ChangeOwner(URP_STRUC*);            // API 2Ch
VOID USBAPI_HC_Proc(URP_STRUC*);                // API 2Dh
VOID USBAPI_Core_Proc(URP_STRUC*);              // API 2eh
VOID USBAPI_LightenKeyboardLEDs_Compatible(URP_STRUC*);    // API 2Fh
VOID USBAPI_KbcAccessControl(URP_STRUC*);       // API 30h      //(EIP29733+)
VOID USBAPI_LegacyControl(URP_STRUC*);       // API 31h      //
VOID USBAPI_GetDeviceAddress(URP_STRUC*);		// API 32h
VOID USBAPI_ExtDriverRequest(URP_STRUC*);		// API 33h
VOID USBAPI_CCIDRequest(URP_STRUC*);            // API 34h      
VOID USBAPI_UsbStopController(URP_STRUC*);      // API 35h	//(EIP74876+)
VOID USBAPI_HcStartStop(URP_STRUC *Urp);		// API 36h

VOID USBMassAPIGetDeviceInformation(URP_STRUC*);    // USB Mass API Sub-Func 00h
VOID USBMassAPIGetDeviceGeometry(URP_STRUC*);       // USB Mass API Sub-Func 01h
VOID USBMassAPIResetDevice(URP_STRUC*);             // USB Mass API Sub-Func 02h
VOID USBMassAPIReadDevice(URP_STRUC*);              // USB Mass API Sub-Func 03h
VOID USBMassAPIWriteDevice(URP_STRUC*);             // USB Mass API Sub-Func 04h
VOID USBMassAPIVerifyDevice(URP_STRUC*);            // USB Mass API Sub-Func 05h
VOID USBMassAPIFormatDevice(URP_STRUC*);            // USB Mass API Sub-Func 06h
VOID USBMassAPICommandPassThru(URP_STRUC*);         // USB Mass API Sub-Func 07h
VOID USBMassAPIAssignDriveNumber(URP_STRUC*);       // USB BIOS API function 08h
VOID USBMassAPICheckDevStatus(URP_STRUC*);          // USB BIOS API function 09h
VOID USBMassAPIGetDevStatus(URP_STRUC*);            // USB BIOS API function 0Ah
VOID USBMassAPIGetDeviceParameters(URP_STRUC*);     // USB BIOS API function 0Bh
VOID USBMassAPIEfiReadDevice(URP_STRUC*);           // USB Mass API Sub-Func 0Ch
VOID USBMassAPIEfiWriteDevice(URP_STRUC*);          // USB Mass API Sub-Func 0Dh
VOID USBMassAPIEfiVerifyDevice(URP_STRUC*);         // USB Mass API Sub-Func 0Eh

DEV_INFO*   USBWrap_GetnthDeviceInfoStructure(UINT8);
VOID        USBWrap_GetDeviceCount(URP_STRUC*);
UINT8       USBWrapGetATAErrorCode(UINT32);
UINT16      USBMassRWVCommand(DEV_INFO*, UINT8, VOID*);
UINT8*      USB_GetDescriptor(HC_STRUC*, DEV_INFO*, UINT8*, UINT16, UINT8, UINT8);
UINT8		UsbSetInterface(HC_STRUC*, DEV_INFO*, UINT8);

UINT32      USB_ReConfigDevice(HC_STRUC*, DEV_INFO*);
UINT32      USB_ReConfigDevice2(HC_STRUC*, DEV_INFO*, CNFG_DESC*, INTRF_DESC*);
DEV_INFO*   UsbAllocDevInfo();
VOID        PrepareForLegacyOs(VOID);
UINT32      USB_ResetAndReconfigDev(HC_STRUC*, DEV_INFO*);
UINT32		USB_DevDriverDisconnect(HC_STRUC*, DEV_INFO*);
VOID        USBKB_LEDOn();

UINT8        USB_StartHostControllers(USB_GLOBAL_DATA*);
UINT8        USB_StopHostControllers(USB_GLOBAL_DATA*);
EFI_STATUS	 InitializeUsbGlobalData(VOID);
UINT8        UsbHcStart(HC_STRUC*);
UINT8        UsbHcStop(HC_STRUC*);
VOID         FixedDelay(UINTN);
DEV_INFO*    USB_GetDeviceInfoStruc(UINT8, DEV_INFO*, UINT8, HC_STRUC*);
UINT8        USBMassGetDeviceInfo(MASS_GET_DEV_INFO*);
UINT8        USBMassGetDeviceGeometry(MASS_GET_DEV_GEO*);

UINT16       USBMassStartUnitCommand (DEV_INFO*);
UINT8        USBMassCmdPassThru (MASS_CMD_PASS_THRU*);
UINT8        USBMassGetDeviceStatus (MASS_GET_DEV_STATUS*);
UINT32       USBMassCheckDeviceReady(DEV_INFO*);
MASS_INQUIRY *USBMassGetDeviceParameters(DEV_INFO*);
VOID         InitSysKbc(EFI_EMUL6064KBDINPUT_PROTOCOL**, EFI_EMUL6064MSINPUT_PROTOCOL**);
EFI_STATUS   InSmmFunction(EFI_HANDLE, EFI_SYSTEM_TABLE*);
VOID         UsbPeriodicEvent();
DEV_INFO*    USB_DetectNewDevice(HC_STRUC*, UINT8, UINT8, UINT8);
UINT8        USB_EnableEndpointsDummy (HC_STRUC*, DEV_INFO*, UINT8*);
UINT8        USB_InitDeviceDataDummy (HC_STRUC*,DEV_INFO*,UINT8,UINT8**);
UINT8        USB_DeinitDeviceDataDummy (HC_STRUC*,DEV_INFO*);
VOID*        USB_MemAlloc(UINT16);
UINT8        USB_MemFree(VOID*, UINT16);
UINT8        USBCheckPortChange (HC_STRUC*, UINT8, UINT8);
VOID         EFIAPI UsbKbcAccessControl(UINT8);
EFI_STATUS   EFIAPI USBRT_LegacyControl (VOID *fpURP);
VOID         EFIAPI USB_StopUnsupportedHc();
UINT8        UsbControlTransfer(HC_STRUC*, DEV_INFO*, DEV_REQ, UINT16, VOID*);
UINT8        UsbInterruptTransfer(HC_STRUC*, DEV_INFO*, UINT8, UINT16, VOID*, UINT16, UINT16);
VOID         CheckBiosOwnedHc(VOID);
DEV_DRIVER*  UsbFindDeviceDriverEntry(DEV_DRIVER*);

#endif

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
