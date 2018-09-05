#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	PciBus.mak
#
# Description:	Make file to build Pci Any Pci Device Porting Driver
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/PciBusDevHooksElink.h

$(BUILD_DIR)/PciBusDevHooksElink.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define AMI_ANY_DEV_HOOK_PCI_GET_SETUP_CONFIG_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCI_DEVICE_GET_SETUP_CONFIG) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCI_SKIP_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCI_PROGRAM_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCI_DEVICE_PROGRAM) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCI_GET_OPT_ROM_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCI_DEVICE_GET_OPT_ROM) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCI_ATTRIBUTES_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCI_DEVICE_ATTRIBUTES) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCI_QUERY_PHASE_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCI_DEVICE_QUERY_PHASE) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCI_READY_TO_BOOT_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCI_READY_TO_BOOT) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCIE_INIT_LINK_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCIE_INIT_LINK) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCIE_SET_ASPM_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCIE_DEVICE_SET_ASPM) $(EOL)\
#define AMI_ANY_DEV_HOOK_PCIE_SET_LNK_SPEED_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_PCIE_DEVICE_SET_LNK_SPEED) $(EOL)\
#define AMI_ANY_DEV_HOOK_HP_UPDATE_PADDING_CALLBACKS_LIST $(AMI_ANY_DEV_HOOK_HP_DEVICE_UPDATE_PADDING) $(EOL)"\
 > $@

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
