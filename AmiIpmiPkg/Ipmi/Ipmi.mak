#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#
# @file  Ipmi.mak
#
#   Make file for Ipmi Module
#


ifeq ($(OEM_SEL_SUPPORT), 1)

Prepare : $(BUILD_DIR)/OemStatusCodeTable.h

$(BUILD_DIR)/OemStatusCodeTable.h : $(BUILD_DIR)/Token.mak $(IPMI_DIR)/Ipmi.mak
	@$(ECHO) \
"#define END_OF_OEM_STATUS_CODE_LIST {0xFFFF, 0xFF, 0xFF, 0xFF, 0xFF}$(EOL)\
EFI_STATUS_CODE_TO_SEL_ENTRY OemStatusCodeToSelTable [] = {$(EOL)\
$(OEM_STATUS_CODE_LIST)$(EOL)\
END_OF_OEM_STATUS_CODE_LIST };$(EOL)" > $(BUILD_DIR)/OemStatusCodeTable.h

endif	#ifeq ($(OEM_SEL_SUPPORT), 1)

ifeq ($(PeiIpmiInitialize_SUPPORT), 1)

Prepare : $(BUILD_DIR)/IpmiInitHooks.h

$(BUILD_DIR)/IpmiInitHooks.h : $(BUILD_DIR)/Token.mak $(IPMI_DIR)/Ipmi.mak
	$(ECHO) \
"#define INIT_LPC_BMC_HOOK_LIST $(INIT_LPC_BMC_HOOK)$(EOL)\
$(EOL)\
#if BMC_INIT_DELAY$(EOL)\
#define BMC_INIT_DELAY_HOOK_LIST $(BMC_INIT_DELAY_HOOK)$(EOL)\
#endif$(EOL)\
$(EOL)\
#if (!IPMI_DEFAULT_HOOK_SUPPORT)$(EOL)\
#define IPMI_OEM_PEI_INIT_HOOK_LIST $(IPMI_OEM_PEI_INIT_HOOK)$(EOL)\
#define IPMI_OEM_DXE_INIT_HOOK_LIST $(IPMI_OEM_DXE_INIT_HOOK)$(EOL)\
#define IPMI_OEM_SMM_INIT_HOOK_LIST $(IPMI_OEM_SMM_INIT_HOOK)$(EOL)\
#define IPMI_OEM_SPMI_UPDATE_HOOK_LIST $(IPMI_OEM_SPMI_UPDATE_HOOK)$(EOL)\
#define IPMI_OEM_SMBIOS_TYPES_UPDATE_HOOK_LIST $(IPMI_OEM_SMBIOS_TYPES_UPDATE_HOOK)$(EOL)\
#endif$(EOL)\
#define BMC_USB_HOOK_LIST $(BMC_USB_INTERFACE_HOOK)$(EOL)\
$(EOL)"\
>$(BUILD_DIR)/IpmiInitHooks.h

endif	#ifeq ($(PeiIpmiInitialize_SUPPORT), 1)

ifeq ($(COMMAND_SPECIFIC_COUNTER_UPDATE_SUPPORT), 1)

Prepare : $(BUILD_DIR)/CommandSpecificRetryCountTable.h

$(BUILD_DIR)/CommandSpecificRetryCountTable.h : $(BUILD_DIR)/Token.mak $(IPMI_DIR)/Ipmi.mak
	$(ECHO) \
"#pragma pack(1)$(EOL)\
typedef struct{$(EOL)\
  UINT8     NetFunction;$(EOL)\
  UINT8     Command;$(EOL)\
  union{$(EOL)\
  UINT32    BtDelay;$(EOL)\
  UINT16    RetryCounter;$(EOL)\
  };$(EOL)\
} COMMAND_SPECIFIC_RETRY_COUNT;$(EOL)\
#pragma pack()$(EOL)\
$(EOL)\
#define END_OF_COMMAND_SPECIFIC_RETRY_COUNT_TABLE {0xFF, 0xFF, 0xFFFFFFFF}$(EOL)\
COMMAND_SPECIFIC_RETRY_COUNT CommandSpecificRetryCountTable [] = {$(EOL)\
$(COMMAND_SPECIFIC_RETRY_COUNT_TABLE)$(EOL)\
END_OF_COMMAND_SPECIFIC_RETRY_COUNT_TABLE };$(EOL)"\
>$(BUILD_DIR)/CommandSpecificRetryCountTable.h

endif #ifeq ($(COMMAND_SPECIFIC_COUNTER_UPDATE_SUPPORT), 1)

Prepare : $(BUILD_DIR)/SpmiAslScope.asl

$(BUILD_DIR)/SpmiAslScope.asl : $(BUILD_DIR)/Token.mak $(IPMI_DIR)/Ipmi.mak
	$(ECHO) \
"#define SPMI_SCOPE $(SPMI_SCOPE)$(EOL)"\
>$(BUILD_DIR)/SpmiAslScope.asl

Prepare : $(BUILD_DIR)/SsifAlertPinCheckHook.h

$(BUILD_DIR)/SsifAlertPinCheckHook.h : $(BUILD_DIR)/Token.mak $(IPMI_DIR)/Ipmi.mak
	$(ECHO) \
"#define SSIF_ALERT_PIN_CHECK_HOOK_LIST $(SSIF_ALERT_PIN_CHECK_HOOK)$(EOL)"\
>$(BUILD_DIR)/SsifAlertPinCheckHook.h

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
