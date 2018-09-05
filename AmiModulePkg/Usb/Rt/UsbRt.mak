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
#<AMI_FHDR_START>
#
# Name:		UsbRt.mak
#
# Description:	Make file for the UsbRt component
#
#<AMI_FHDR_END>
#**********************************************************************

USB_RT_SOURCES := \
  ../$(USBRT_DIR)/AmiUsb.c$(EOL)\
  ../$(USBRT_DIR)/UsbKbd.c$(EOL)\
  ../$(USBRT_DIR)/Debug.c$(EOL)\
  ../$(USBRT_DIR)/Elib.c$(EOL)\
  ../$(USBRT_DIR)/Uhci.c$(EOL)\
  ../$(USBRT_DIR)/Usb.c$(EOL)\
  ../$(USBRT_DIR)/UsbHub.c$(EOL)\
  ../$(USBRT_DIR)/UsbMass.c$(EOL)\
  ../$(USBRT_DIR)/UsbMass.h$(EOL)\
  ../$(USBRT_DIR)/UsbCcid.c$(EOL)\
  ../$(USBRT_DIR)/UsbMs.c$(EOL)\
  ../$(USBRT_DIR)/UsbHid.c$(EOL)\
  ../$(USBRT_DIR)/UsbPoint.c$(EOL)\
  ../$(USBRT_DIR)/Uhci.h$(EOL)\
  ../$(USBRT_DIR)/UsbKbd.h$(EOL)\
  ../$(USBRT_DIR)/Ehci.c$(EOL)\
  ../$(USBRT_DIR)/Ehci.h$(EOL)\
  ../$(USBRT_DIR)/Ohci.c$(EOL)\
  ../$(USBRT_DIR)/Ohci.h$(EOL)\
  ../$(USBRT_DIR)/SysKbc.c$(EOL)\
  ../$(USBRT_DIR)/SysNoKbc.c$(EOL)\
  ../$(USBRT_DIR)/Xhci.h$(EOL)\
  ../$(USBRT_DIR)/Xhci.c$(EOL)\
  ../$(UHCD_DIR)/UsbPort.c$(EOL)
  
USB_RT_PACKAGES := \
  MdePkg/MdePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)
  
USB_RT_LIBRARYCLASSES := \
  BaseMemoryLib$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  DebugLib$(EOL)\
  IoLib$(EOL)\
  TimerLib$(EOL)
  
USB_RT_PROTOCOLS := \
  gEfiUsbProtocolGuid$(EOL)
  
USB_RT_SMM_SOURCES := ../$(UHCD_DIR)/UsbSb.c$(EOL)
USB_RT_SMM_PROTOCOLS := \
  gAmiUsbSmmProtocolGuid$(EOL)\
  gEfiUsbPolicyProtocolGuid$(EOL)\
  gEfiSmmUsbDispatch2ProtocolGuid$(EOL)\
  gEfiSmmSwDispatch2ProtocolGuid$(EOL)\
  gEfiSmmPeriodicTimerDispatch2ProtocolGuid$(EOL)\
  gEfiSmmGpiDispatch2ProtocolGuid$(EOL)\
  gEmul6064MsInputProtocolGuid$(EOL)\
  gEmul6064TrapProtocolGuid$(EOL)\
  gEmul6064KbdInputProtocolGuid$(EOL)
USB_RT_SMM_DEPEX := \
  gEfiUsbProtocolGuid AND$(EOL)\
  gEfiSmmBase2ProtocolGuid AND$(EOL)\
  gEfiSmmSwDispatch2ProtocolGuid$(EOL)
USB_RT_SMM_LIBRARYCLASSES := \
  AmiBufferValidationLib$(EOL)\
  SmmServicesTableLib$(EOL)\
  AmiUsbSmmGlobalDataValidationLib$(EOL)

USB_RT_DXE_SOURCES :=$(EOL)
USB_RT_DXE_PROTOCOLS :=$(EOL)
USB_RT_DXE_DEPEX := \
  gEfiUsbProtocolGuid$(EOL)
USB_RT_DXE_LIBRARYCLASSES :=$(EOL)

ifeq ($(USB_ACPI_ENABLE_WORKAROUND),1)
USB_RT_SMM_PROTOCOLS += \
  gEfiAcpiEnDispatchProtocolGuid$(EOL)
ifeq ($(AmiChipsetPkg_SUPPORT), 1)
USB_RT_SMM_PACKAGES := \
  AmiChipsetPkg/AmiChipsetPkg.dec$(EOL)
endif
endif

ifneq ($(USB_RT_SOURCES_LIST),"")
USB_RT_SOURCES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_SOURCES_LIST)))$(EOL)
endif
  
ifneq ($(USB_RT_PACKAGES_LIST),"")
USB_RT_PACKAGES += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_PACKAGES_LIST)))$(EOL)
endif
  
ifneq ($(USB_RT_LIBRARYCLASSES_LIST),"")
USB_RT_LIBRARYCLASSES += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_LIBRARYCLASSES_LIST)))$(EOL)
endif

ifneq ($(USB_RT_PROTOCOLS_LIST),"")
USB_RT_PROTOCOLS += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_PROTOCOLS_LIST)))$(EOL)
endif

ifneq ($(USB_RT_SMM_SOURCES_LIST),"")
USB_RT_SMM_SOURCES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_SMM_SOURCES_LIST)))$(EOL)
endif
  
ifneq ($(USB_RT_SMM_PACKAGES_LIST),"")
USB_RT_SMM_PACKAGES += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_SMM_PACKAGES_LIST)))$(EOL)
endif
  
ifneq ($(USB_RT_SMM_LIBRARYCLASSES_LIST),"")
USB_RT_SMM_LIBRARYCLASSES += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_SMM_LIBRARYCLASSES_LIST)))$(EOL)
endif

ifneq ($(USB_RT_SMM_PROTOCOLS_LIST),"")
USB_RT_SMM_PROTOCOLS += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_SMM_PROTOCOLS_LIST)))$(EOL)
endif

ifneq ($(USB_RT_DXE_SOURCES_LIST),"")
USB_RT_DXE_SOURCES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_DXE_SOURCES_LIST)))$(EOL)
endif
  
ifneq ($(USB_RT_DXE_PACKAGES_LIST),"")
USB_RT_DXE_PACKAGES += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_DXE_PACKAGES_LIST)))$(EOL)
endif
  
ifneq ($(USB_RT_DXE_LIBRARYCLASSES_LIST),"")
USB_RT_DXE_LIBRARYCLASSES += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_DXE_LIBRARYCLASSES_LIST)))$(EOL)
endif

ifneq ($(USB_RT_DXE_PROTOCOLS_LIST),"")
USB_RT_DXE_PROTOCOLS += $(patsubst %,%,$(subst $(SPACE),$(EOL)$(SPACE),$(USB_RT_DXE_PROTOCOLS_LIST)))$(EOL)
endif

Prepare : $(BUILD_DIR)/UsbDevDriverElinks.h $(BUILD_DIR)/UsbRtSmm.inf $(BUILD_DIR)/UsbRtDxe.inf

$(BUILD_DIR)/UsbDevDriverElinks.h :
	$(ECHO) \
"// Don't delete this line$(EOL)\
#define USB_DEV_EFI_DRIVER $(USB_DEV_EFI_DRIVER_LIST)$(EOL)\
#define USB_DEV_DELAYED_DRIVER $(USB_DEV_DELAYED_DRIVER_LIST)$(EOL)\
#define USB_DEV_DRIVER $(USB_DEV_DRIVER_LIST)$(EOL)\
#define KBD_BUFFER_CHECK_ELINK_LIST $(CheckKeyBoardBufferForSpecialChars)$(EOL)\
#define USB_HID_TYPE_CHECK_ELINK_LIST $(UsbHidTypeCheckFunctions)$(EOL)\
#define USB_HID_BUFFER_CHECK_ELINK_LIST $(UsbHidBufferCheckFunctions)$(EOL)\
#define EHCI_GET_ROOT_HUB_PORT_SPEED_ELINK_LIST $(EhciGetRootHubPortSpeedFunctions)$(EOL)\
// Don't delete this line$(EOL)"\
> $(BUILD_DIR)/UsbDevDriverElinks.h

$(BUILD_DIR)/UsbRtSmm.inf : $(BUILD_DIR)/Token.h $(USBRT_DIR)/UsbRt.mak
	$(ECHO) \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = UsbRtSmm$(EOL)\
  FILE_GUID                      = 04EAAAA1-29A1-11d7-8838-00500473D4EB$(EOL)\
  MODULE_TYPE                    = DXE_SMM_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  PI_SPECIFICATION_VERSION       = 0x00010014$(EOL)\
  ENTRY_POINT                    = USBDriverEntryPoint$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  $(USB_RT_SOURCES)\
  $(USB_RT_SMM_SOURCES)\
$(EOL)\
[Packages]$(EOL)\
  $(USB_RT_PACKAGES)\
  $(USB_RT_SMM_PACKAGES)\
$(EOL)\
[LibraryClasses]$(EOL)\
  $(USB_RT_LIBRARYCLASSES)\
  $(USB_RT_SMM_LIBRARYCLASSES)\
$(EOL)\
[Protocols]$(EOL)\
  $(USB_RT_PROTOCOLS)\
  $(USB_RT_SMM_PROTOCOLS)\
$(EOL)\
[Depex]$(EOL)\
  $(USB_RT_SMM_DEPEX)\
$(EOL)\
[BuildOptions]$(EOL)\
  MSFT:*_*_*_CC_FLAGS = /D USB_RT_DRIVER$(EOL)\
  GCC:*_*_*_CC_FLAGS = -DUSB_RT_DRIVER$(EOL)"\
> $(BUILD_DIR)/UsbRtSmm.inf

$(BUILD_DIR)/UsbRtDxe.inf : $(BUILD_DIR)/Token.h $(USBRT_DIR)/UsbRt.mak
	$(ECHO) \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = UsbRtDxe$(EOL)\
  FILE_GUID                      = 171F43DC-C4D9-47a6-9641-65DDCDD5AA30$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  PI_SPECIFICATION_VERSION       = 0x00010014$(EOL)\
  ENTRY_POINT                    = USBDriverEntryPoint$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  $(USB_RT_SOURCES)\
  $(USB_RT_DXE_SOURCES)\
$(EOL)\
[Packages]$(EOL)\
  $(USB_RT_PACKAGES)\
  $(USB_RT_DXE_PACKAGES)\
$(EOL)\
[LibraryClasses]$(EOL)\
  $(USB_RT_LIBRARYCLASSES)\
  $(USB_RT_DXE_LIBRARYCLASSES)\
$(EOL)\
[Protocols]$(EOL)\
  $(USB_RT_PROTOCOLS)\
  $(USB_RT_DXE_PROTOCOLS)\
$(EOL)\
[Depex]$(EOL)\
  $(USB_RT_DXE_DEPEX)\
$(EOL)\
[BuildOptions]$(EOL)\
  MSFT:*_*_*_CC_FLAGS = /D USB_RT_DRIVER /D USB_RT_DXE_DRIVER$(EOL)\
  GCC:*_*_*_CC_FLAGS = -DUSB_RT_DRIVER -DUSB_RT_DXE_DRIVER$(EOL)"\
> $(BUILD_DIR)/UsbRtDxe.inf

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
