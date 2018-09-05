#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#**********************************************************************
# $Header: /Alaska/SOURCE/Modules/USB/ALASKA/uhcd.mak 11    10/25/11 8:26a Wilsonlee $
#
# $Revision: 11 $
#
# $Date: 10/25/11 8:26a $
#
#****************************************************************************

#**********************************************************************
#<AMI_FHDR_START>
#
# Name:		UHCD.MAK
#
# Description:	Make file for the UHCD component
#
#<AMI_FHDR_END>
#**********************************************************************

Prepare : $(BUILD_DIR)/Uhcd.inf

ifeq ("$(USB_IAD_PROTOCOL_SUPPORT)", "1")
USB_IAD = gAmiUsbIadProtocolGuid
endif

$(BUILD_DIR)/Uhcd.inf : $(BUILD_DIR)/Token.h $(UHCD_DIR)/Uhcd.mak
	$(ECHO) \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = Uhcd$(EOL)\
  FILE_GUID                      = 580DD900-385D-11d7-883A-00500473D4EB$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = AmiUsbDriverEntryPoint$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  ../$(UHCD_DIR)/Uhcd.c$(EOL)\
  ../$(UHCD_DIR)/AmiUsbHc.c$(EOL)\
  ../$(UHCD_DIR)/UsbBus.c$(EOL)\
  ../$(UHCD_DIR)/EfiUsbKb.c$(EOL)\
  ../$(UHCD_DIR)/Tree.c$(EOL)\
  ../$(UHCD_DIR)/EfiUsbKb.h$(EOL)\
  ../$(UHCD_DIR)/UsbBus.h$(EOL)\
  ../$(UHCD_DIR)/Tree.h$(EOL)\
  ../$(UHCD_DIR)/EfiUsbMass.c$(EOL)\
  ../$(UHCD_DIR)/EfiUsbMs.c$(EOL)\
  ../$(UHCD_DIR)/EfiUsbCcid.c$(EOL)\
  ../$(UHCD_DIR)/EfiUsbPoint.c$(EOL)\
  ../$(UHCD_DIR)/EfiUsbHid.c$(EOL)\
  ../$(UHCD_DIR)/ComponentName.c$(EOL)\
  ../$(UHCD_DIR)/UsbMisc.c$(EOL)\
  ../$(UHCD_DIR)/ComponentName.h$(EOL)\
  ../$(UHCD_DIR)/UsbPort.c$(EOL)\
  ../$(USBRT_DIR)/Debug.c$(EOL)\
$(EOL)\
[Packages]$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
$(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  MemoryAllocationLib$(EOL)\
  UefiLib$(EOL)\
  DebugLib$(EOL)\
$(EOL)\
[Protocols]$(EOL)\
  gEfiSimpleTextInputExProtocolGuid$(EOL)\
  gEfiUsbHcProtocolGuid$(EOL)\
  gEfiDriverBindingProtocolGuid$(EOL)\
  gEfiUsbPolicyProtocolGuid$(EOL)\
  gEfiUsbProtocolGuid$(EOL)\
  gEfiUsb2HcProtocolGuid$(EOL)\
  gEfiPciIoProtocolGuid$(EOL)\
  gUsbTimingPolicyProtocolGuid$(EOL)\
  gEfiSmmControl2ProtocolGuid$(EOL)\
  gEfiComponentName2ProtocolGuid$(EOL)\
  gNonSmmEmul6064TrapProtocolGuid$(EOL)\
  gEfiDevicePathProtocolGuid$(EOL)\
  gEfiUsbIoProtocolGuid$(EOL)\
  gEfiBlockIoProtocolGuid$(EOL)\
  gEfiSimpleTextInProtocolGuid$(EOL)\
  gAmiEfiKeycodeProtocolGuid$(EOL)\
  gEfiSimplePointerProtocolGuid$(EOL)\
  gEfiSmmPeriodicTimerDispatch2ProtocolGuid$(EOL)\
  gAmiBlockIoWriteProtectionProtocolGuid$(EOL)\
  gEfiLegacyBiosPlatformProtocolGuid$(EOL)\
  gEfiAbsolutePointerProtocolGuid$(EOL)\
  gAmiMultiLangSupportProtocolGuid$(EOL)\
  gAmiPciIrqProgramGuid$(EOL)\
  $(USB_IAD)$(EOL)\
$(EOL)\
[Pcd]$(EOL)\
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress$(EOL)\
$(EOL)\
[Depex]$(EOL)\
  TRUE$(EOL)\
$(EOL)\
[BuildOptions]$(EOL)\
  MSFT:*_*_*_CC_FLAGS = /D USB_RT_DXE_DRIVER$(EOL)\
  GCC:*_*_*_CC_FLAGS = -DUSB_RT_DXE_DRIVER$(EOL)"\
> $(BUILD_DIR)/Uhcd.inf

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
