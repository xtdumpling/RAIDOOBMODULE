#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

#**********************************************************************
# $Header: $
#
# $Revision: $
#
# $Date: $
#**********************************************************************
# Revision History
# ----------------
# $Log:
# 
#
#**********************************************************************
#<AMI_FHDR_START>
#
# Name: 
#
# Description:
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : PrepareMEBin
PrepareMEBin:
ifeq ($(AUTO_ME_UPDATE_POLICY), 0)
ifneq ($(BUILD_OS), $(BUILD_OS_LINUX))
	if not exist $(BIOS_ME_FILE) $(CP) /y $(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwVersion$(PATH_SLASH)MeFwVersion.cbin $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin
else
	if [ ! -f "$(BIOS_ME_FILE)" ]; then $(CP) $(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwVersion$(PATH_SLASH)MeFwVersion.cbin $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin; fi
endif
endif
ifeq ($(AUTO_ME_UPDATE_POLICY), 1)
ifneq ($(BUILD_OS), $(BUILD_OS_LINUX))
	if exist $(BIOS_ME_FILE) $(CP) /y $(BIOS_ME_FILE) $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin
	if not exist $(BIOS_ME_FILE) $(CP) /y $(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwVersion$(PATH_SLASH)MeFwVersion.cbin $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin
else
	if [ -f "$(BIOS_ME_FILE)" ]; then $(CP) $(BIOS_ME_FILE) $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin; fi
	if [ ! -f "$(BIOS_ME_FILE)" ]; then $(CP) $(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwVersion$(PATH_SLASH)MeFwVersion.cbin $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin; fi
endif
endif
ifeq ($(AUTO_ME_UPDATE_POLICY), 2)
ifneq ($(BUILD_OS), $(BUILD_OS_LINUX))
	if exist $(BIOS_ME_FILE) $(CP) /y $(BIOS_ME_FILE) $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin
	if not exist $(BIOS_ME_FILE) $(CP) /y $(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwVersion$(PATH_SLASH)MeFwVersion.cbin $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin
	if exist $(BIOS_MEFW_CAPSULE_FILE) @del $(BIOS_MEFW_CAPSULE_FILE)
	if exist $(MEFW_CAPSULE_FILE) @del $(MEFW_CAPSULE_FILE)
else
	if [ -f "$(BIOS_ME_FILE)" ]; then $(CP) $(BIOS_ME_FILE) $(PATH_SLASH)$(BUILD_DIR)$(PATH_SLASH)MeRegion.bin; fi
	if [ ! -f "$(BIOS_ME_FILE)" ]; then $(CP) $(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwVersion$(PATH_SLASH)MeFwVersion.cbin $(BUILD_DIR)$(PATH_SLASH)MeRegion.bin; fi
	if [ -f "$(BIOS_MEFW_CAPSULE_FILE)" ]; then $(RM) $(BIOS_MEFW_CAPSULE_FILE); fi
	if [ -f "$(MEFW_CAPSULE_FILE)" ]; then $(RM) $(MEFW_CAPSULE_FILE); fi
endif
ifeq ($(ISH_FW_UPDATE_SUPPORT), 1)
ifneq ($(BUILD_OS), $(BUILD_OS_LINUX))
	if exist $(ISH_FW_CAPSULE_FILE) @del $(ISH_FW_CAPSULE_FILE)
	if exist $(BUILD_DIR)$(PATH_SLASH)IshFwFdfFileStatement.txt @del $(BUILD_DIR)$(PATH_SLASH)IshFwFdfFileStatement.txt
else
	if [ -f "$(ISH_FW_CAPSULE_FILE)" ]; then $(RM) $(ISH_FW_CAPSULE_FILE); fi
	if [ -f "$(BUILD_DIR)$(PATH_SLASH)IshFwFdfFileStatement.txt" ]; then $(RM) $(BUILD_DIR)$(PATH_SLASH)IshFwFdfFileStatement.txt; fi
endif
endif
endif

ifeq ($(AUTO_ME_UPDATE_POLICY), 2)
ifeq ($(FWCAPSULE_FILE_FORMAT), 0)
# FW Signature Block inside the BIOS ROM as a ROM Hole Ffs.
End : CombineFwCapsule
BUILD_ME : SignFwCapsule
else
# FW Signature Block is attached on top of BIOS Image.
BUILD_ME : CombineFwCapsule SignFwCapsule
endif

CombineFwCapsule:
ifneq ($(BUILD_OS), $(BUILD_OS_LINUX))
	$(CP) /b $(UNSIGNED_BIOS_ROM)+$(UNSIGNED_MEFW_CAPSULE) $(UNSIGNED_MEFW_CAPSULE)2
else
	$(CAT) $(UNSIGNED_BIOS_ROM) $(UNSIGNED_MEFW_CAPSULE) > $(UNSIGNED_MEFW_CAPSULE)2
endif
ifeq ($(FWCAPSULE_FILE_FORMAT), 1)
# Save Unsigned BIOS and ME capsule for later used.
ifneq ($(BUILD_OS), $(BUILD_OS_LINUX)) 
	$(CP) /b $(UNSIGNED_BIOS_ROM)+$(UNSIGNED_MEFW_CAPSULE) $(UNSIGNED_MEFW_CAPSULE)0
else
	$(CAT) $(UNSIGNED_BIOS_ROM) $(UNSIGNED_MEFW_CAPSULE) > $(UNSIGNED_MEFW_CAPSULE)0
endif
endif

EMPTY := 
SIGNED_FWCAP_HDR := $(BUILD_DIR)$(PATH_SLASH)FwCapsuleHdr.Sig
FWKEY_STATEMENT_FILE := $(FWKey_DIR)$(PATH_SLASH)FWkeyFileStatement.txt
MEFW_LAYOUT_BIN := $(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)$(DXE_ARCHITECTURE)$(PATH_SLASH)$(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwLayout$(PATH_SLASH)MeFwLayout$(PATH_SLASH)OUTPUT$(PATH_SLASH)MeFwLayout.bin
ISHFW_LAYOUT_BIN := $(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)$(DXE_ARCHITECTURE)$(PATH_SLASH)$(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwLayout$(PATH_SLASH)IshFwLayout$(PATH_SLASH)OUTPUT$(PATH_SLASH)MeFwLayout.bin
BIOSMEFW_LAYOUT_BIN := $(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)$(DXE_ARCHITECTURE)$(PATH_SLASH)$(AUTO_MEUD_DIR)$(PATH_SLASH)MeFwLayout$(PATH_SLASH)BiosMeFwLayout$(PATH_SLASH)OUTPUT$(PATH_SLASH)MeFwLayout.bin
UNSIGN_ISHFW_CAPSULE := $(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)FV$(PATH_SLASH)FV_ISH_FW_CAPSULE.Fv
ifndef CRYPTOCON_CMDLINE_MAP # SECURE_FLASH_MODULE_REVISION < 19
# substituted "-n" with empty for no FwKey file.
REP_CRYPTOCON_CMDLINE_0 := $(subst -n, $(EMPTY), $(CRYPTOCON_CMDLINE))
# substituted "-y" with empty for no FW Capsule Header file.
REP_CRYPTO_CMDLINE := $(subst -y, $(EMPTY), $(REP_CRYPTOCON_CMDLINE_0))
# substituted "-r2" with -r and particular Layout and -g and dummy guid for no FW Capsule Header file
REP_MEFW_CRYPTOCMD_0 := $(subst -r2, -g 58013D6D-71ED-4FA6-A1B5-51C865C20735 -r $(MEFW_LAYOUT_BIN), $(REP_CRYPTO_CMDLINE))
REP_ISHFW_CRYPTOCMD_0 := $(subst -r2,  -g 58013D6D-71ED-4FA6-A1B5-51C865C20735 -r $(ISHFW_LAYOUT_BIN), $(REP_CRYPTO_CMDLINE))
# substituted output file name and romLayout file name.
REP_BIOSMEFW_CRYPTOCMD := $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGNED_MEFW_CAPSULE)2, $(CRYPTOCON_CMDLINE))
REP_MEFW_CRYPTOCMD := $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGNED_MEFW_CAPSULE), $(REP_MEFW_CRYPTOCMD_0))
REP_ISHFW_CRYPTOCMD := $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGN_ISHFW_CAPSULE), $(REP_ISHFW_CRYPTOCMD_0))
else # SECURE_FLASH_MODULE_REVISION > 18
# substituted "-n" with empty for no FwKey file.
REP_CRYPTO_CMDLINE_SIG_0 := $(subst -n, $(EMPTY), $(CRYPTOCON_CMDLINE_SIG))
REP_CRYPTO_CMDLINE_SIG := $(subst -y, $(EMPTY), $(REP_CRYPTO_CMDLINE_SIG_0))
# substituted "-y" with empty for no FW Capsule Header file.
REP_CRYPTO_CMDLINE_MAP := $(subst -y, $(EMPTY), $(CRYPTOCON_CMDLINE_MAP))
# substituted output file name and romLayout file name.
REP_BIOSMEFW_CRYPTOCMD_MAP := $(subst $(ROM_LAYOUT_EX), $(BIOSMEFW_LAYOUT_BIN), $(CRYPTOCON_CMDLINE_MAP))
REP_BIOSMEFW_CRYPTOCMD_SIG := $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGNED_MEFW_CAPSULE)2, $(CRYPTOCON_CMDLINE_SIG))
REP_MEFW_CRYPTOCMD_SIG := $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGNED_MEFW_CAPSULE), $(REP_CRYPTO_CMDLINE_SIG))
REP_MEFW_CRTPTOCMD_MAP := $(subst $(ROM_LAYOUT_EX), $(MEFW_LAYOUT_BIN), $(REP_CRYPTO_CMDLINE_MAP))
REP_ISHFW_CRYPTOCMD_SIG := $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGN_ISHFW_CAPSULE), $(REP_CRYPTO_CMDLINE_SIG))
REP_ISHFW_CRYPTOCMD_MAP := $(subst $(ROM_LAYOUT_EX), $(ISHFW_LAYOUT_BIN), $(REP_CRYPTO_CMDLINE_MAP))
endif

SignFwCapsule:
	@echo ----------------------------------------------------------------
	@echo ----- create Signed BIOS + ME FW Capsule : "$(BIOS_MEFW_CAPSULE_FILE)" ------
	@echo ----------------------------------------------------------------
ifeq ($(wildcard $(FWpriv)),$(FWpriv))
ifndef CRYPTOCON_CMDLINE_MAP # SECURE_FLASH_MODULE_REVISION < 19
	$(CRYPTCON) -c2 -y -r$(ROM_LAYOUT_EX) -l$(FWCAPSULE_MAX_HDR_SIZE) -f $(UNSIGNED_MEFW_CAPSULE)2 -o $(UNSIGNED_MEFW_CAPSULE)2
	$(CRYPTCON) $(subst $(FWCAPSULE_FILE_NAME), $(BIOS_MEFW_CAPSULE_FILE), $(REP_BIOSMEFW_CRYPTOCMD))
else # SECURE_FLASH_MODULE_REVISION > 18 
#-	$(CRYPTCON) $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGNED_MEFW_CAPSULE)2, $(REP_BIOSMEFW_CRYPTOCMD_MAP))
#-	$(CRYPTCON) $(subst $(FWCAPSULE_FILE_NAME), $(BIOS_MEFW_CAPSULE_FILE), $(REP_BIOSMEFW_CRYPTOCMD_SIG))
#-	$(CRYPTCON) $(REP_BIOSMEFW_CRYPTOCMD_MAP) -f $(UNSIGNED_MEFW_CAPSULE)2 -o $(UNSIGNED_MEFW_CAPSULE)2
	$(CRYPTCON) $(CRYPTOCON_CMDLINE_MAP) -f $(UNSIGNED_MEFW_CAPSULE)2 -o $(UNSIGNED_MEFW_CAPSULE)2
	$(CRYPTCON) $(CRYPTOCON_CMDLINE_SIG) -f $(UNSIGNED_MEFW_CAPSULE)2 -o $(BIOS_MEFW_CAPSULE_FILE)
endif
ifeq ($(FWCAPSULE_FILE_FORMAT), 1)
# split the FW Signature Block from signed image.
	split -f $(BIOS_MEFW_CAPSULE_FILE) -s $(FWCAPSULE_MAX_HDR_SIZE) -o $(SIGNED_FWCAP_HDR)
# combine the FW Signature Block with unsigned image for identical Bios image.
	$(CP) /b /y $(SIGNED_FWCAP_HDR)+$(UNSIGNED_MEFW_CAPSULE)0 $(BIOS_MEFW_CAPSULE_FILE)
endif
else
	@echo ----- WARNING!!! Missing RSA private key FWpriv=$(FWpriv) to sign BIOS + ME FW Capsule image.
endif	
	@echo ----------------------------------------------------------------
	@echo ----- create Signed ME FW Capsule : "$(MEFW_CAPSULE_FILE)" -------------
	@echo ---------------------------------------------------------------- 
ifeq ($(wildcard $(FWpriv)),$(FWpriv))
ifndef CRYPTOCON_CMDLINE_MAP # SECURE_FLASH_MODULE_REVISION < 19
	$(CRYPTCON) $(subst $(FWCAPSULE_FILE_NAME), $(MEFW_CAPSULE_FILE), $(REP_MEFW_CRYPTOCMD))
else # SECURE_FLASH_MODULE_REVISION > 18
#-	$(CRYPTCON) $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGNED_MEFW_CAPSULE), $(REP_MEFW_CRTPTOCMD_MAP))
#-	$(CRYPTCON) $(subst $(FWCAPSULE_FILE_NAME), $(MEFW_CAPSULE_FILE), $(REP_MEFW_CRYPTOCMD_SIG))
	$(CRYPTCON) $(REP_MEFW_CRTPTOCMD_MAP) -f $(UNSIGNED_MEFW_CAPSULE) -o $(UNSIGNED_MEFW_CAPSULE)
	$(CRYPTCON) $(REP_CRYPTO_CMDLINE_SIG) -f $(UNSIGNED_MEFW_CAPSULE) -o $(MEFW_CAPSULE_FILE)
endif

ifeq ($(ISH_FW_UPDATE_SUPPORT), 1)
	@echo ----------------------------------------------------------------
	@echo ----- create Signed ISH FW Capsule : "$(ISH_FW_CAPSULE_FILE)" -------------
	@echo ---------------------------------------------------------------- 
ifndef CRYPTOCON_CMDLINE_MAP # SECURE_FLASH_MODULE_REVISION < 19
	$(CRYPTCON) $(subst $(FWCAPSULE_FILE_NAME), $(ISH_FW_CAPSULE_FILE), $(REP_ISHFW_CRYPTOCMD))
else # SECURE_FLASH_MODULE_REVISION > 18 
#-	$(CRYPTCON) $(subst $(UNSIGNED_BIOS_ROM), $(UNSIGN_ISHFW_CAPSULE), $(REP_ISHFW_CRYPTOCMD_MAP))
#-	$(CRYPTCON) $(subst $(FWCAPSULE_FILE_NAME), $(ISH_FW_CAPSULE_FILE), $(REP_ISHFW_CRYPTOCMD_SIG))
	$(CRYPTCON) $(REP_ISHFW_CRYPTOCMD_MAP) -f $(UNSIGN_ISHFW_CAPSULE) -o $(UNSIGN_ISHFW_CAPSULE)
	$(CRYPTCON) $(REP_CRYPTO_CMDLINE_SIG) -f $(UNSIGN_ISHFW_CAPSULE) -o $(ISH_FW_CAPSULE_FILE)
endif
endif
else
	@echo ----- WARNING!!! Missing RSA private key FWpriv=$(FWpriv) to sign ME FW Capsule image.
endif
endif

# Create ME Firmware FID file for Secure Flash verification used.
Prepare : $(BUILD_DIR)/MeFwFid.inf
$(BUILD_DIR)/MeFwFid.inf : $(BUILD_DIR)/Token.h $(AUTO_MEUD_DIR)/AutoMeud.mak 
	$(ECHO)  \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = MeFwFid$(EOL)\
  FILE_GUID                      = 3FD1D3A2-99F7-420b-BC69-8BB1D492A332$(EOL)\
  MODULE_TYPE                    = USER_DEFINED$(EOL)\
  SECTION_GUID=2EBE0275-6458-4AF9-91ED-D3F4EDB100AA$(EOL)\
[Sources]$(EOL)\
  ../AmiModulePkg/Fid/Fid.cbin$(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)"\
> $(BUILD_DIR)/MeFwFid.inf

# Create ISH Firmware Capsule files for Secure Flash verification used.
ifeq ($(AUTO_ME_UPDATE_POLICY), 2)
ifeq ($(ISH_FW_UPDATE_SUPPORT), 1)
# Create ISH Firmware FID file for Secure Flash verification used.
Prepare : $(BUILD_DIR)/IshFwFid.inf
$(BUILD_DIR)/IshFwFid.inf: $(BUILD_DIR)/Token.h $(AUTO_MEUD_DIR)/AutoMeud.mak 
	$(ECHO)  \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = IshFwFid$(EOL)\
  FILE_GUID                      = 3FD1D3A2-99F7-420b-BC69-8BB1D492A332$(EOL)\
  MODULE_TYPE                    = USER_DEFINED$(EOL)\
  SECTION_GUID=2EBE0275-6458-4AF9-91ED-D3F4EDB100AA$(EOL)\
[Sources]$(EOL)\
  ../AmiModulePkg/Fid/Fid.cbin$(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)"\
> $(BUILD_DIR)/IshFwFid.inf

# Create ISH Firmware FDF File Statement file.
Prepare : $(BUILD_DIR)/IshFwFdfFileStatement.txt
$(BUILD_DIR)/IshFwFdfFileStatement.txt : $(BUILD_DIR)/Token.h $(AUTO_MEUD_DIR)/AutoMeud.mak 
ifeq ($(wildcard $(ME_ISH_FIRMEARE_FILE)),$(ME_ISH_FIRMEARE_FILE))
	$(ECHO)  \
"FILE FREEFORM = DD7E6038-3049-4E76-968F-A2C25B07C284 {$(EOL)\
  SECTION COMPRESS PI_STD {$(EOL)\
    SECTION RAW = $(ME_ISH_FIRMEARE_FILE)$(EOL)\
  }$(EOL)\
}$(EOL)\
">> $@
endif
ifeq ($(wildcard $(ME_PDT_DATA_FILE)),$(ME_PDT_DATA_FILE))
	$(ECHO)  \
"FILE FREEFORM = D6F5D3F2-96C0-4EA6-B200-C8B7E3636640 {$(EOL)\
  SECTION COMPRESS PI_STD {$(EOL)\
    SECTION RAW = $(ME_PDT_DATA_FILE)$(EOL)\
  }$(EOL)\
}$(EOL)\
">> $@
endif
	$(ECHO) " $(EOL)">> $@
endif
endif
#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************