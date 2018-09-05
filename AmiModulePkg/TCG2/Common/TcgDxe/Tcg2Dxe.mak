Prepare : Tcg2DxeFiles

Tcg2DxeFiles : $(BUILD_DIR)/Tcg2Dxe.inf 

$(BUILD_DIR)/Tcg2Dxe.inf : $(BUILD_DIR)/Token.mak $(TPM_20_TCG_DXE_DIR)/Tcg2Dxe.mak
	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = Tcg2Dxe$(EOL)\
  FILE_GUID                      = 39045756-FCA3-49bd-8DAE-C7BAE8389AFF$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = TreeDxeEntry$(EOL)\
$(EOL)\
[Sources]$(EOL)\
	../$(TPM_20_TCG_DXE_DIR)/Tcg2Dxe.c$(EOL)\
	../$(TPM_20_TCG_DXE_DIR)/Tcg2Dxe.h$(EOL)\
	../$(TPM_20_TCG_DXE_DIR)/Tpm2Sequences.c$(EOL)\
"> $(BUILD_DIR)/Tcg2Dxe.inf
#ifneq ($(TpmFunctionCallFiles),"")
#	@$(ECHO) \
#"	$(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(TpmFunctionCallFiles)))$(EOL)\
#">> $(BUILD_DIR)/Tcg2Dxe.inf
#endif
	@$(ECHO) \
"$(EOL)\
[Packages]$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  UefiCpuPkg/UefiCpuPkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  AmiTcgPkg/AmiTcgPkg.dec$(EOL)\
">> $(BUILD_DIR)/Tcg2Dxe.inf

	@$(ECHO) \
"$(EOL)\
[LibraryClasses]$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  BaseTisLib20$(EOL)\
  ShaLib$(EOL)\
  BaseCrbLibDxe$(EOL)\
  BaseLib$(EOL)\
  BaseMemoryLib$(EOL)\
  UefiLib$(EOL)\
  TcgAslLib$(EOL)\
  BaseAmiTcmlib$(EOL)\
  PeCoffLib$(EOL)\
  TpmReportStatusCodeWrapperLibDxe$(EOL)\
">> $(BUILD_DIR)/Tcg2Dxe.inf

	@$(ECHO) \
"$(EOL)\
[Protocols]$(EOL)\
  gTcgPlatformSetupPolicyGuid    	## CONSUMES$(EOL)\
  gAmiTpmSupportTypeProtocolguid    	## CONSUMES$(EOL)\
  gEfiBlockIoProtocolGuid    		## CONSUMES$(EOL)\
  gEfiFirmwareVolume2ProtocolGuid 	## CONSUMES$(EOL)\
  gEfiAcpiSupportGuid            	## CONSUMES$(EOL)\
  gEfiDiskIoProtocolGuid  	    	## PRODUCES$(EOL)\
  gEfiTrEEProtocolGuid			## CONSUMES$(EOL)\
  gEfiResetArchProtocolGuid    		## CONSUMES$(EOL)\
  gEfiSmmSwDispatch2ProtocolGuid      	## CONSUMES$(EOL)\
  gEfiTcgPlatformProtocolGuid		   ## CONSUMES$(EOL)\
  AmiProtocolInternalHlxeGuid		   ## CONSUMES$(EOL)\
  gAmiDxeHashInterfaceguid		   ## CONSUMES$(EOL)\
  gEfiSmmBase2ProtocolGuid      	## CONSUMES$(EOL)\
  gBdsAllDriversConnectedProtocolGuid	## CONSUMES$(EOL)\
">> $(BUILD_DIR)/Tcg2Dxe.inf

	@$(ECHO) \
"[Guids]$(EOL)\
  gEfiImageSecurityDatabaseGuid    ## CONSUMES$(EOL)\
  gEfiSmbiosTableGuid		   ## CONSUMES$(EOL)\
  AmiValidBootImageCertTblGuid		   ## CONSUMES$(EOL)\
  gTpm20HobGuid		   ## CONSUMES$(EOL)\
  gTcg2FinalEventsTableGuid		   ## CONSUMES$(EOL)\
  gEfiMemoryOverwriteControlDataGuid		   ## CONSUMES$(EOL)\
  ExtendedDataGuid		   ## CONSUMES$(EOL)\
  gSetupVariableGuid		   ## CONSUMES$(EOL)\
  gEfiTcgTreeLogHobGuid		   ## CONSUMES$(EOL)\
  gTcgInternalflagGuid		   ## CONSUMES$(EOL)\
  AmitcgefiOsVariableGuid		   ## CONSUMES$(EOL)\
  gEfiAcpiTableGuid     	## CONSUMES$(EOL)\
  ">> $(BUILD_DIR)/Tcg2Dxe.inf
ifeq ($(TCG2_SYSTEM_RESET_GUID_CORE), 1) 
	@$(ECHO) \
  "gAmiResetSystemEventGuid       	## CONSUMES$(EOL)\
">> $(BUILD_DIR)/Tcg2Dxe.inf
endif 

	@$(ECHO) \
"$(EOL)\
#//>>>$(EOL)\
[Depex]$(EOL)\
  gTcgPlatformSetupPolicyGuid$(EOL)\
#//<<<$(EOL)\
$(EOL)\
[BuildOptions]$(EOL)\
  *_*_*_BUILD_FLAGS=-s$(EOL)\
">> $(BUILD_DIR)/Tcg2Dxe.inf
