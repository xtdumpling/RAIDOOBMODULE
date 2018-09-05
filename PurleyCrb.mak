#***************************************************************************
#**                                                                       **
#**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
#**                                                                       **
#**                                                                       **
#***************************************************************************
#***************************************************************************
#
#  File History
#
#  Rev. 1.02
#    Bug Fix:  Restore override files
#    Reason:   
#    Auditor:  Hartmann Hsieh
#    Date:     May/31/2016
#
#  Rev. 1.01
#    Bug Fix:  Fix build CRB BIOS fail after disable SMCPKG_Support
#    Reason:   
#    Auditor:  Salmon Chen
#    Date:     Apr/21/2016
#
#  Rev. 1.00
#    Bug Fix:  Initial revision.
#    Reason:   For board level build image
#    Auditor:  Salmon Chen
#    Date:     Feb/18/2016
#
#***************************************************************************
#***************************************************************************
#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#**********************************************************************
# $Header:  $
#
# $Revision:  $
#
# $Date:  $
#**********************************************************************
# Revision History
# ----------------
# $Log:  $
# 
# 
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	PurleyCrb.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************

#Do not change the order of Platform configurations.
EXTERNAL_CC_FLAGS +=  -D CRB_FLAG

ifeq ($(DEBUG_FLAGS_ENABLE), 1)
#EXTERNAL_CC_FLAGS += -D SERIAL_DBG_MSG=1  -D PLATFORM_VARIABLE_ATTRIBUTES=0x7  # Attributes are defined based on RT_ACCESS_SUPPORT_IN_HPKTOOL token.
EXTERNAL_CC_FLAGS += -D SERIAL_DBG_MSG=1
else
#EXTERNAL_CC_FLAGS += -D MDEPKG_NDEBUG -D SILENT_MODE  -D PLATFORM_VARIABLE_ATTRIBUTES=0x3 # Core takes care of this.
endif

#Define PLATFORM_VARIABLE_ATTRIBUTES based on RT_ACCESS_SUPPORT_IN_HPKTOOL token.
ifeq ($(RT_ACCESS_SUPPORT_IN_HPKTOOL), 1)
EXTERNAL_CC_FLAGS += -D PLATFORM_VARIABLE_ATTRIBUTES=0x7
else
EXTERNAL_CC_FLAGS += -D PLATFORM_VARIABLE_ATTRIBUTES=0x3
endif

ifeq ($(DEBUG_FLAGS_ENABLE), 1)
EXTERNAL_CC_FLAGS += -D SERIAL_DBG_MSG=1
else
#EXTERNAL_CC_FLAGS += -D MDEPKG_NDEBUG -D SILENT_MODE # Core takes care of this.
endif

ifeq ($(SDP_PLATFORM), 1)
EXTERNAL_CC_FLAGS += -D SDP_FLAG -D SV_OR_SDP_FLAG
endif


ifeq ($(SSAFD), 1)
EXTERNAL_CC_FLAGS += -D PURLEY_FLAG
else
 ifeq ($(SSALFD), 1)
 EXTERNAL_CC_FLAGS +=  -D PURLEY_FLAG -D SSA_FLAG
 else
 EXTERNAL_CC_FLAGS += -D PURLEY_FLAG -D SSA_FLAG -D SSA_LOADER_FLAG
 endif
endif


ifeq ($(SLE_FLAG_ENABLE), 1)
EXTERNAL_CC_FLAGS += -D HW_EMULATION -D SLE_FLAG
endif

ifeq ($(SOFTSIM_BIOS_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -D SOFTSIM_FLAG
endif

#Define based on INTEL_TXT_SUPPORT support
ifeq ($(INTEL_TXT_SUPPORT), 1)
EXTERNAL_CC_FLAGS +=  -D LT_FLAG
endif

ifeq ($(SPARING_SCRATCHPAD_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -D SPARING_SCRATCHPAD_SUPPORT
endif

ifeq ($(TRAD_BIOS_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DTRAD_FLAG=1
endif

ifeq ($(ULT_BIOS_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DULT_FLAG=1
endif

ifeq ($(SERIAL_IO_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DSERIAL_IO_FLAG=1
endif

ifeq ($(SUS_WELL_RESTORE_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DSUS_WELL_RESTORE=1
endif

ifeq ($(ADSP_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DADSP_FLAG=1
endif

ifeq ($(WDT_SUPPORT_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DWDT_SUPPORT_ENABLED=1
endif

ifeq ($(USB_PRECONDITION_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DUSB_PRECONDITION_ENABLE_FLAG=1
endif

ifeq ($(RSTE_TESTING), 1)
EXTERNAL_CC_FLAGS +=  -DRSTE_TEST=1
endif

ifeq ($(PCH_SERVER_BIOS_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DPCH_SERVER_BIOS_FLAG=1
endif

ifeq ($(BTG_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DBTG_FLAG
endif

ifeq ($(SERVER_BIOS_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -DSERVER_BIOS_FLAG=1
endif

ifeq ( $(BIOSGUARD_EC_ENABLE) , 1 )
EXTERNAL_CC_FLAGS += -D BIOSGUARD_FLAG=1 -D BIOSGUARD_EC_FLAG=1
else
EXTERNAL_CC_FLAGS += -D BIOSGUARD_FLAG=1
endif

ifeq ($(LIGHTNINGRIDGE_PPO_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -D LIGHTNINGRIDGE_PPO_FLAG
endif

ifeq ($(PURLEYEPDVP_PPO_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -D PURLEYEPDVP_PPO_FLAG
endif

ifeq ($(NEON_PPO_ENABLE), 1)
EXTERNAL_CC_FLAGS +=  -D NEON_PPO_FLAG
endif

ifeq ($(HEDT_PPO_ENABLE), 1)
EXTERNAL_CC_FLAGS += -D HEDT_PPO_FLAG
endif

ifeq ($(HEDTME_PPO_ENABLE), 1)
EXTERNAL_CC_FLAGS += -D HEDT_PPO_FLAG
endif

#AMI IPMI module is used in source. Not required to define IPMI flags for Intel.

ifeq ($(PPO_FLAG), 1)
EXTERNAL_CC_FLAGS += -D PPO_FLAG
endif

ifeq ($(NVDIMM_ENABLE), 1)
EXTERNAL_CC_FLAGS += -D MEM_NVDIMM_EN -D NVMEM_FEATURE_EN
endif

EXTERNAL_CC_FLAGS += -D MAX_SOCKET=$(MAX_SOCKET)

ifeq ($(IE_ENABLE), 1)
EXTERNAL_CC_FLAGS += -DIE_SUPPORT=1
endif

ifeq ($(ME_ENABLE), 1)
EXTERNAL_CC_FLAGS += -DME_SUPPORT_FLAG=1
	ifeq ($(SPS_ENABLE), 1)
  	EXTERNAL_CC_FLAGS += -DSPS_SUPPORT=1
   	endif
   	ifeq ($(ME_TESTMENU), 1)
  	EXTERNAL_CC_FLAGS += -DME_TESTMENU_FLAG=1
   	endif
    ifeq ($(AMT_ENABLE), 1) 
     EXTERNAL_CC_FLAGS += -DAMT_SUPPORT=1 -DAMT_SUPPORT_FLAG=1
        ifeq ($(ICC_ENABLE), 1)
         EXTERNAL_CC_FLAGS += -DICC_SUPPORT=1
        endif
        ifeq ($(ASF_PEI_ENABLE), 1)
         EXTERNAL_CC_FLAGS += -DASF_PEI=1
        endif
        ifeq ($(Terminal_SUPPORT), 1)
         EXTERNAL_CC_FLAGS += -DTerminal_SUPPORT=1
        endif    
    endif #ifeq ($(AMT_ENABLE), 1)
endif #ifeq ($(ME_ENABLE), 1)

# Include EFI_PCI_IOV_SUPPORT defines based on PCI_IOV_SUPPORT sdl token
ifeq ($(PCI_IOV_SUPPORT), 1)
EXTERNAL_CC_FLAGS += -D EFI_PCI_IOV_SUPPORT
endif


#RAS_SUPPORT support flag is provided as EXTERNAL flags based on RAS_SUPPORT token.
ifeq ($(RAS_SUPPORT), 1)
EXTERNAL_CC_FLAGS += -D RAS_SUPPORT
endif

#WHEA_SUPPORT support flag is provided as EXTERNAL flags based on WHEA_SUPPORT token.
ifeq ($(WHEA_SUPPORT), 1)
EXTERNAL_CC_FLAGS += -D WHEA_SUPPORT
endif

EXTERNAL_CC_FLAGS += -D SKX_HOST

ifeq ($(SMCPKG_SUPPORT), 0)
EXTERNAL_CC_FLAGS += -DENHANCED_WARNING_LOG=1
endif

ifeq ($(HOTPLUG_SUPPORT), 1)
EXTERNAL_CC_FLAGS += -D HOTPLUG_FLAG
endif

#Dynamic mmcfg base address change support is provided as EXTERNAL flags based on DYNAMIC_MMCFG_BASE_SUPPORT token.
ifeq ($(DYNAMIC_MMCFG_BASE_SUPPORT), 1)
EXTERNAL_CC_FLAGS += -D DYNAMIC_MMCFG_BASE_FLAG
endif

ifeq ($(WORKSTATION_SUPPORT),0)
BeforeGenFds: CopyPtuRomCertificate

CopyPtuRomCertificate:
	@echo ----- Copy PTU ROM Certificate to OEM Certificate folder -----"
	@copy /Y $(FITC_PTU_DIR)\Purley_SpsNMPTU_root.cer $(OemCert_DIR)\Purley_SpsNMPTU_root.cer
endif

Prepare: DeleteRomFile

Prepare: CreateAmlOffsetHeaderFile

DeleteRomFile: 

ifeq ($(SMCPKG_SUPPORT),1)
ifeq ($(DEBUG_MODE), 0)

	@copy /Y BuildTools\Smc\ChkBiosSetting.exe $(BUILD_DIR)
	@copy /Y BIOSSettings.cfg $(BUILD_DIR)
	@copy /Y BuildTools\Smc\ChkMEsetting.exe $(BUILD_DIR)
	@copy /Y BuildTools\Smc\ChkMEsetting.bat $(BUILD_DIR)
	@copy /Y ChkMEsettings.cfg $(BUILD_DIR)

	call BuildTools\Smc\RunPy.bat BuildTools\Smc\smcprj.py

	$(BUILD_DIR)\ChkBiosSetting.exe $(BUILD_DIR)\BIOSSettings.cfg -p $(BUILD_DIR)
	$(BUILD_DIR)\ChkMEsetting.bat
endif	
endif	

ifeq ($(SecureMod_SUPPORT),1)
	if exist $(FWCAPSULE_FILE_NAME) $(RM) $(FWCAPSULE_FILE_NAME)
endif	
	if exist $(PROJECT_TAG)*.CAP $(RM) $(PROJECT_TAG)*.CAP
	if exist $(PROJECT_TAG)*.ROM $(RM) $(PROJECT_TAG)*.ROM
	if exist OutImage*.bin $(RM) OutImage*.bin
	
#End : CreateBinFile

OUTPUT_DIR = $(OUTPUT_DIRECTORY)\$(TARGET)_$(TOOL_CHAIN_TAG)
export OUTPUT_DIR

#.PHONY : CreateBinFile
CreateBinFile:
ifeq ($(SecureMod_SUPPORT),1)
    ifeq ($(wildcard $(FWpriv)), $(FWpriv))
	@echo ----- Copy Signed BIOS Image file to AMIROM.fd for RomImage"
	@copy /Y $(FWCAPSULE_FILE_NAME) $(OUTPUT_DIR)\FV\AMIROM.fd
    else
	@echo ----- Copy Unsigned BIOS Image file from AMIROM.fd"
	@copy /Y $(OUTPUT_DIR)\FV\AMIROM.fd $(BIOS_TAG).ROM
    endif
else
	@copy /Y $(OUTPUT_DIR)\FV\AMIROM.fd $(BIOS_TAG).ROM
endif

# Supermicro + START

#
# Create the bat file of project name.
#
ifeq ($(SMCPKG_SUPPORT), 1)
	call BuildTools\Smc\RunPy.bat BuildTools\Smc\smcprj.py

#
# Create the bat file of token environment variables.
#

	@$(ECHO)\
"$(EOL)\
set OverrideSps_SUPPORT=$(OverrideSps_SUPPORT)$(EOL)\
set BuildTarget=$(TARGET)$(EOL)\
set ReleaseBuild=$(OFFICIAL_RELEASE)$(EOL)\
set IPMI_SUPPORT=$(IPMI_SUPPORT)$(EOL)\
set SMCPKG_SUPPORT=1$(EOL)\
set BIOS_SIZE=$(BIOS_SIZE)$(EOL)\
$(EOL)"\
> $(BUILD_DIR)\SetSmcEnv.bat

	if not exist $(OUTPUT_DIR)\ROM $(MKDIR) $(OUTPUT_DIR)\ROM
	@$(ECHO) Generating OutImage.bin
	@call PurleyPlatPkg\BuildImage.bat $(FTOOL_DIR) $(FITC_XML_DIR) $(FITC_FILE_NAME) $(FITC_NM_FILE_NAME)

#
# Restore override files
#
	xcopy Original .\ /K /E /Y /C /I /H
else
	@$(ECHO)\
"$(EOL)\
set OverrideSps_SUPPORT=0$(EOL)\
set SMCPKG_SUPPORT=0$(EOL)\
$(EOL)"\
> $(BUILD_DIR)\SetSmcEnv.bat
# Supermicro - End
	if not exist $(OUTPUT_DIR)\ROM $(MKDIR) $(OUTPUT_DIR)\ROM
ifeq ($(WORKSTATION_SUPPORT),1)
	@copy /Y $(ME_TOOL_DIR)\$(ME_BIN) $(OUTPUT_DIR)\ROM\ME_11.bin
	@copy /Y $(ME_TOOL_DIR)\$(GBE_NVM_BIN) $(OUTPUT_DIR)\ROM\GBE.bin
	@copy /Y $(ME_TOOL_DIR)\..\SPS\$(10GBE_NVM_BIN) $(OUTPUT_DIR)\ROM\10GBE_NVM.bin
	@$(ECHO) Generating OutImage.bin
	@call PurleyPlatPkg\BuildImage.bat $(ME_TOOL_DIR) $(ME_XML_FILE_NAME) ME
	@copy $(OUTPUT_DIR)\ROM\OutImage_ME.bin OutImage_ME.bin	
ifeq ($(BiosGuard_SUPPORT), 1)
	@copy $(OUTPUT_DIR)\ROM\OutImage_ME.map $(FITC_ME_MAP)
	@$(ECHO) OutImage_ME.bin OutImage.bin > $(BIOS_GUARD_NAME)
	call $(BUILD_BIOS_GUARD_PSL)
	@del $(FITC_ME_MAP)
endif
else
	@copy /Y $(FTOOL_DIR)\$(GBE_NVM_BIN) $(OUTPUT_DIR)\ROM\GBE_NVM.bin
	@copy /Y $(FTOOL_DIR)\$(10GBE_NVM_BIN) $(OUTPUT_DIR)\ROM\10GBE_NVM.bin
	@$(ECHO) Generating OutImage.bin
ifeq ($(FPGA_SUPPORT),1)
	@call PurleyPlatPkg\BuildImage.bat $(FTOOL_DIR) $(FITC_XML_DIR) $(FITC_FILE_NAME) $(FITC_NM_FILE_NAME) FPGA
else
	@call PurleyPlatPkg\BuildImage.bat $(FTOOL_DIR) $(FITC_XML_DIR) $(FITC_FILE_NAME) $(FITC_NM_FILE_NAME)
endif
ifeq ("$(FITC_NM_FILE_NAME)", "")
	@copy $(OUTPUT_DIR)\ROM\OutImage.bin OutImage.bin	
else
	@copy $(OUTPUT_DIR)\ROM\OutImage.bin OutImage.bin
	@copy $(OUTPUT_DIR)\ROM\OutImage_NM.bin OutImage_NM.bin
endif	
endif
endif
ifeq ($(SecureMod_SUPPORT),1)
    ifeq ($(wildcard $(FWpriv)), $(FWpriv))
	@$(ECHO) OutImage.bin and $(FWCAPSULE_FILE_NAME) binaries created.
    else
	@$(ECHO) OutImage.bin and $(BIOS_TAG).ROM binaries created.
    endif
else
	@$(ECHO) OutImage.bin and $(BIOS_TAG).ROM binaries created.
endif

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************