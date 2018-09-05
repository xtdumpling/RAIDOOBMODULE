Prepare : $(BUILD_DIR)/Tpm20PlatformElinks.h Tpm20Files

$(BUILD_DIR)/Tpm20PlatformElinks.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define TPM2SETUPFUNCTIONS_LIST $(TPM2SETUPFUNCTIONS)$(EOL)\
#define TPM2PHAUTHFUNCTIONS_LIST $(TPM2PHAUTHFUNCTIONS)$(EOL)\
#define TPM2PPIFUNCTIONS_LIST $(TPM2PPIFUNCTIONS)$(EOL)"\
> $(BUILD_DIR)/Tpm20PlatformElinks.h


Tpm20Files : $(BUILD_DIR)/Tpm20PlatformDxe.inf 

$(BUILD_DIR)/Tpm20PlatformDxe.inf : $(BUILD_DIR)/Token.mak $(TPM_20_PLATFORM_DXE_DIR)/Tpm20PlatformDxe.mak
	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = Tpm20PlatformDxe$(EOL)\
  FILE_GUID                      = 0718AD81-F26A-4850-A6EC-F268E309D707$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = Tpm20PlatformEntry$(EOL)\
#$(EOL)\
#  This flag specifies whether HII resource section is generated into PE image.$(EOL)\
#$(EOL)\
  UEFI_HII_RESOURCE_SECTION      = TRUE$(EOL)\
$(EOL)\
#$(EOL)\
# The following information is for reference only and not required by the build tools.$(EOL)\
#$(EOL)\
#  VALID_ARCHITECTURES           = IA32 X64 IPF EBC$(EOL)\
#$(EOL)\
$(EOL)\
[Sources]$(EOL)\
	../$(TPM_20_PLATFORM_DXE_DIR)/Tpm20PlatformDxe.c$(EOL)\
	../$(TPM_20_PLATFORM_DXE_DIR)/Tpm20PlatformDxe.h$(EOL)\
	../$(TPM_20_PLATFORM_DXE_DIR)/Tpm20PlatformDxeFakeTokens.c$(EOL)\
	../$(TPM_20_PLATFORM_DXE_DIR)/Tpm20PlatformDxeString.uni$(EOL)\
"> $(BUILD_DIR)/Tpm20PlatformDxe.inf
ifneq ($(TpmFunctionCallFiles),"")
	@$(ECHO) \
"	$(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(TpmFunctionCallFiles)))$(EOL)\
">> $(BUILD_DIR)/Tpm20PlatformDxe.inf
endif
	@$(ECHO) \
"$(EOL)\
[Packages]$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiTsePkg/AmiTsePkg.dec$(EOL)\
  AmiTcgPkg/AmiTcgPkg.dec$(EOL)\
  MdeModulePkg/MdeModulePkg.dec$(EOL)\
">> $(BUILD_DIR)/Tpm20PlatformDxe.inf

	@$(ECHO) \
"$(EOL)\
[LibraryClasses]$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  DevicePathLib$(EOL)\
  UefiLib$(EOL)\
  ShaLib$(EOL)\
  HiiLib$(EOL)\
  DebugLib$(EOL)\
  TimerLib$(EOL)\
  NetLib$(EOL)\
  TpmReportStatusCodeWrapperLibDxe$(EOL)\
  BaseCrbLibDxe$(EOL)\
  BaseTisLib20$(EOL)\
  AmiTcgLibDxe$(EOL)\
">> $(BUILD_DIR)/Tpm20PlatformDxe.inf

	@$(ECHO) \
"$(EOL)\
[Protocols]$(EOL)\
  gAmiTcgPlatformProtocolguid    ## CONSUMES$(EOL)\
  gEfiFirmwareVolume2ProtocolGuid    ## CONSUMES$(EOL)\
  gEfiBlockIoProtocolGuid    ## CONSUMES$(EOL)\
  gEfiFirmwareVolume2ProtocolGuid ## CONSUMES$(EOL)\
  gEfiAcpiSupportGuid            ## CONSUMES$(EOL)\
  gEfiDiskIoProtocolGuid  	    ## PRODUCES$(EOL)\
  gEfiHiiImageProtocolGuid		## CONSUMES$(EOL)\
  gOpromStartEndProtocolGuid    ## CONSUMES$(EOL)\
  gEfiHiiPackageListProtocolGuid      ## CONSUMES$(EOL)\
  gEfiTrEEProtocolGuid      ## CONSUMES$(EOL)\
  gTcgPlatformSetupPolicyGuid      ## CONSUMES$(EOL)\
  AmiProtocolInternalHlxeGuid      ## CONSUMES$(EOL)\
  gBdsAllDriversConnectedProtocolGuid      ## CONSUMES$(EOL)\
  gAmiPostManagerProtocolGuid      ## CONSUMES$(EOL)\
  gEdkiiVariableLockProtocolGuid      ## CONSUMES$(EOL)\
  gEfiResetArchProtocolGuid  ## CONSUMES$(EOL)\
  gEfiTrEEProtocolGuid      		  ## CONSUMES$(EOL)\
  AmiProtocolInternalHlxeGuid 		  ## CONSUMES$(EOL)\
  gAmiDxeHashInterfaceguid 			  ## CONSUMES$(EOL)\
  gEfiSimpleTextInProtocolGuid        ## CONSUMES$(EOL)\
">> $(BUILD_DIR)/Tpm20PlatformDxe.inf

	@$(ECHO) \
"[Guids]$(EOL)\
  gEfiImageSecurityDatabaseGuid    ## CONSUMES$(EOL)\
  gSetupVariableGuid  ## CONSUMES$(EOL)\
  gAmiTsePasswordPromptExitGuid  ## CONSUMES$(EOL)\
  gAmiTseAdminPasswordValidGuid  ## CONSUMES$(EOL)\
  gTcgEfiGlobalVariableGuid  ## CONSUMES$(EOL)\
  AmitcgefiOsVariableGuid   ## CONSUMES$(EOL)\
  AmiValidBootImageCertTblGuid   ## CONSUMES$(EOL)\
  FastBootVariableGuid   ## CONSUMES$(EOL)\
  gTcgInternalflagGuid   ## CONSUMES$(EOL)\
  gEfiSmbiosTableGuid   ## CONSUMES$(EOL)\
  FlagsStatusguid   ## CONSUMES$(EOL)\
  gAmiOsPpiConfirmationOverrideGuid      ## CONSUMES$(EOL)\
  TcgPasswordAuthenticationGuid   ## CONSUMES$(EOL)\
  gEfiMemoryOverwriteControlDataGuid   ## CONSUMES$(EOL)\
  gEfiPartTypeUnusedGuid  ## CONSUMES$(EOL)\
  gAmiTpm20MeasureConfigurationInfoGuid      ## CONSUMES$(EOL)\
  gAmiTsePasswordPromptExitGuid      ## CONSUMES$(EOL)\
  gAmiTseAdminPasswordValidGuid      ## CONSUMES$(EOL)\
  gMicrocodeGuid  ## CONSUMES$(EOL)\
  gTcgNvramHobGuid ## CONSUMES$(EOL)\
  gEfiSmbiosTableGuid  ## CONSUMES$(EOL)\
">> $(BUILD_DIR)/Tpm20PlatformDxe.inf

	@$(ECHO) \
"$(EOL)\
#//>>>$(EOL)\
[Depex]$(EOL)\
  gEfiTrEEProtocolGuid$(EOL)\
#//<<<$(EOL)\
$(EOL)\
[BuildOptions]$(EOL)\
  *_*_*_BUILD_FLAGS=-s$(EOL)\
">> $(BUILD_DIR)/Tpm20PlatformDxe.inf
