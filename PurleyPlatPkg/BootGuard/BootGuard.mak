#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1987-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30071           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

#**********************************************************************
#<AMI_FHDR_START>
#
# Name: BootGuard.mak
#
# Description: This make file updates the final ROM file
#
#<AMI_FHDR_END>
#**********************************************************************

End : Fit

.PHONY : Fit


Fit:
	$(ECHO) Inside BootGuard.mak

ifeq ($(INTEL_BOOT_GUARD_SUPPORT), 1)
ifeq ($(BOOT_GUARD_SIGNING_INTERLOCK_SUPPORT), 0)

ifneq ($(BOOT_GUARD_BPM_PEM_KEY_FILENAME),)
	@if not exist $(BOOT_GUARD_BPM_PEM_KEY_FILENAME) $(BOOTGUARD_DIR)\keygen.exe $(BOOT_GUARD_BPM_PEM_KEY_FILENAME) $(BOOT_GUARD_BPM_MODULUS_KEY_FILENAME) $(BOOT_GUARD_PBULIC_KEY_EXPONENT)
endif

ifeq ($(INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT), 1)
	$(BOOTGUARD_DIR)\BpmKmGen.exe -PFVMAIN $(FitFirstEndPatchBIOSRomPath) $(BOOTGUARD_DIR) $(BIOS_STARTING_ADDRESS) $(DXE_SEGMENT_BASE) $(DXE_SEGMENT_SIZE)
endif

ifneq ($(BOOT_GUARD_KM_PEM_KEY_FILENAME),)
	if not exist $(BOOT_GUARD_KM_PEM_KEY_FILENAME) $(BOOTGUARD_DIR)\keygen.exe $(BOOT_GUARD_KM_PEM_KEY_FILENAME) $(BOOT_GUARD_KM_MODULUS_KEY_FILENAME) $(BOOT_GUARD_PBULIC_KEY_EXPONENT)
endif
	$(BOOTGUARD_DIR)\BpmKmGen.exe -PBPMKM $(FitFirstEndPatchBIOSRomPath) $(BOOTGUARD_DIR) $(BOOT_GUARD_BPM_PEM_KEY_FILENAME) $(BOOT_GUARD_KM_PEM_KEY_FILENAME) $(BootGuardTokens)
endif

ifeq ($(BOOT_GUARD_SIGNING_INTERLOCK_SUPPORT), 1)
	
ifeq ($(INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT), 1)
	$(BOOTGUARD_DIR)\BpmKmGen.exe -PFVMAIN $(FitFirstEndPatchBIOSRomPath) $(BOOTGUARD_DIR) $(BIOS_STARTING_ADDRESS) $(DXE_SEGMENT_BASE) $(DXE_SEGMENT_SIZE)
endif
####
#### PUBLIC KEY passed as Input for sign tool
####
ifneq ($(BOOT_GUARD_BPM_MODULUS_KEY_FILENAME),)
	@if not exist $(BOOT_GUARD_BPM_MODULUS_KEY_FILENAME)  	@ECHO WARNING BPM Modulus is missing. BPMKMGEN tool won't work properly!!!
endif
ifneq ($(BOOT_GUARD_KM_MODULUS_KEY_FILENAME),)
	@if not exist $(BOOT_GUARD_KM_MODULUS_KEY_FILENAME) @ECHO WARNING KM Modulus is missing. BPMKMGEN tool won't work properly!!!
endif

ifneq ($(BOOT_GUARD_BPM_MODULUS_KEY_FILENAME),)
ifneq ($(BOOT_GUARD_KM_MODULUS_KEY_FILENAME),)
ifeq ($(wildcard $(BOOT_GUARD_BPM_MODULUS_KEY_FILENAME)),$(BOOT_GUARD_BPM_MODULUS_KEY_FILENAME))
ifeq ($(wildcard $(BOOT_GUARD_KM_MODULUS_KEY_FILENAME)),$(BOOT_GUARD_KM_MODULUS_KEY_FILENAME))
	$(BOOTGUARD_DIR)\BpmKmGen.exe -PBPMKMMDSING $(FitFirstEndPatchBIOSRomPath) $(BOOTGUARD_DIR) $(BOOT_GUARD_BPM_MODULUS_KEY_FILENAME) $(BOOT_GUARD_KM_MODULUS_KEY_FILENAME) $(BootGuardTokens) $(BootGuardSignInterLockTokens)
endif
endif
endif
endif
####
#### PRIVATE KEY passed as Input for sign tool
####
ifneq ($(BOOT_GUARD_BPM_PEM_KEY_FILENAME),)
	@if not exist $(BOOT_GUARD_BPM_PEM_KEY_FILENAME)  	@ECHO WARNING BPM PEM Key is missing. BPMKMGEN tool won't work properly!!!
endif
ifneq ($(BOOT_GUARD_KM_PEM_KEY_FILENAME),)
	@if not exist $(BOOT_GUARD_KM_PEM_KEY_FILENAME) @ECHO WARNING KM PEM Key is missing. BPMKMGEN tool won't work properly!!!
endif
ifneq ($(BOOT_GUARD_BPM_PEM_KEY_FILENAME),)
ifneq ($(BOOT_GUARD_KM_PEM_KEY_FILENAME),)
ifeq ($(wildcard $(BOOT_GUARD_BPM_PEM_KEY_FILENAME)),$(BOOT_GUARD_BPM_PEM_KEY_FILENAME))
ifeq ($(wildcard $(BOOT_GUARD_KM_PEM_KEY_FILENAME)),$(BOOT_GUARD_KM_PEM_KEY_FILENAME))
	$(BOOTGUARD_DIR)\BpmKmGen.exe -PBPMKMSING $(FitFirstEndPatchBIOSRomPath) $(BOOTGUARD_DIR) $(BOOT_GUARD_BPM_PEM_KEY_FILENAME) $(BOOT_GUARD_KM_PEM_KEY_FILENAME) $(BootGuardTokens) $(BootGuardSignInterLockTokens)
endif
endif
endif
endif
endif
endif

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1987-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30071           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************