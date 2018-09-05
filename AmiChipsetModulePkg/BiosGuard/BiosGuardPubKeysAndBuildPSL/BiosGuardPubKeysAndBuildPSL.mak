#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
#<AMI_FHDR_START>
#
# Name: BiosGuardPubKey.mak
#
# Description: Make for BiosGuardPubKey
#
#<AMI_FHDR_END>
#*************************************************************************
Prepare: \
BIOS_GUARD_GENERATE_KEYS\
$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt\
$(BUILD_DIR)/BiosMap.bat\
$(BUILD_DIR)/BuildPSL.bat\
$(BUILD_DIR)/BuildBiosGuardVariable.bat

IS_BIOS_FLASH_SIZE_ALIGN_1MB:
ifeq ($(BiosGuard_SUPPORT), 1)
ifeq ($(IS_BIOS_FLASH_SIZE_ALIGNMENT1MB), 0)
	$(error FLASH_SIZE must to align 1MB for Bios Guard, this is limitation of CPU RC in the BiosGuardService.c)
endif
endif

BIOS_GUARD_PRIVATE_KEY = $(BIOS_GUARD_PUB_PATH)\BiosGuardPrivate.key
BIOS_GUARD_PUBLIC_KEY = $(BUILD_DIR)\BiosGuardPublic.key
BIOS_GUARD_VKEY_EXP = $(BIOS_GUARD_PUB_PATH)\.BiosGuardVkeyExp
BIOS_GUARD_PUBLIC_KEY_HASH = $(BUILD_DIR)\BiosGuardPubKeyhash.bin

BIOS_GUARD_PUB_KEY_FFS_FILE_GUID = 9C6B404D-5E6A-4DFB-96F3-64A063BA4FC4

BIOS_GUARD_GENERATE_KEYS:
	if exist AMI_BIOS_GUARD_*.* $(RM) AMI_BIOS_GUARD_*.*
	if exist $(BIOS_GUARD_PUBLIC_KEY_HASH) del $(BIOS_GUARD_PUBLIC_KEY_HASH)
	if not exist $(BIOS_GUARD_PRIVATE_KEY) $(KEYGEN) $(BIOS_GUARD_PRIVATE_KEY) $(BIOS_GUARD_PUBLIC_KEY)
	$(BIOS_GUARD_CRYPTOCON) -z -w -k $(BIOS_GUARD_PRIVATE_KEY) -f $(BIOS_GUARD_PUB_PATH)/dummy.bin -s
	copy /b .out_key + $(BIOS_GUARD_VKEY_EXP) $(BIOS_GUARD_PUBLIC_KEY_HASH)
	if exist .out_key del .out_key
	if exist .out_PfatCer del .out_PfatCer
	$(BIOS_GUARD_CRYPTOCON) -h2 -f $(BIOS_GUARD_PUBLIC_KEY_HASH) -o $(BIOS_GUARD_PUBLIC_KEY_HASH)

$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt: $(BIOS_GUARD_PUBLIC_KEY_HASH)
	$(ECHO)  \
	FILE FREEFORM = $(BIOS_GUARD_PUB_KEY_FFS_FILE_GUID) {$(EOL)\
	SECTION RAW = $< $(EOL)}> $@

$(BUILD_DIR)/BiosMap.bat: $(BUILD_DIR)/token.mak
	$(ECHO) "$(BUILD_BIOS_MAP_SET)" > $@
	call $@

$(BUILD_DIR)/BuildPSL.bat: $(BUILD_DIR)/token.mak
	$(ECHO) "$(BUILD_PSL_SET)" > $@
	call $@
	
$(BUILD_DIR)/BuildBiosGuardVariable.bat: $(BUILD_DIR)/token.mak
	$(ECHO) "$(BUILD_BIOS_GUARD_VARIABLE_SET)" > $@
	call $@	

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
