Prepare : SetupFiles

SetupFiles : $(BUILD_DIR)/Setup.inf $(BUILD_DIR)/SetupDefaults.inf $(BUILD_DIR)/AutoId.h 

$(BUILD_DIR)/AutoId.h : $(Setup_DIR)/Setup.vfr $(SETUP_DEFINITIONS) $(BUILD_DIR)/Token.mak
	$(VFRID) /s$(THE_LOWEST_AUTOID_NUMBER) /o$(BUILD_DIR)/AutoId.h $(Setup_DIR)/Setup.vfr $(SETUP_DEFINITIONS)
	@$(ECHO) \
"#ifndef AUTO_ID$(EOL)\
#define AUTO_ID(x) x$(EOL)\
#endif$(EOL)" >> $(BUILD_DIR)/AutoId.h 

$(BUILD_DIR)/Setup.inf : $(BUILD_DIR)/Token.mak $(Setup_DIR)/Setup.mak
	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
#   SETUP_DEFINITIONS, SETUP_FORMSETS, SetupStringFiles, and SetupCallbackFiles$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = Setup$(EOL)\
  FILE_GUID                      = 899407D7-99FE-43d8-9A21-79EC328CAC21$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = SetupEntry$(EOL)\
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
  ../$(Setup_DIR)/Setup.c$(EOL)\
  ../$(Setup_DIR)/SetupHiiAccess.c$(EOL)\
  ../$(Setup_DIR)/Setup.uni$(EOL)\
  ../$(Setup_DIR)/Setup.vfr$(EOL)\
  ../$(Setup_DIR)/SetupLibrary.c$(EOL)\
  ../$(Setup_DIR)/SetupPrivate.h$(EOL)\
"> $(BUILD_DIR)/Setup.inf
ifneq ($(SETUP_DEFINITIONS),"")
	@$(ECHO) \
"  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(SETUP_DEFINITIONS)))$(EOL)\
">> $(BUILD_DIR)/Setup.inf
endif
ifneq ($(SetupStringFiles),"")
	@$(ECHO) \
"  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(SetupStringFiles)))$(EOL)\
">> $(BUILD_DIR)/Setup.inf
endif
ifneq ($(SetupCallbackFiles),"")
	@$(ECHO) \
"	$(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(SetupCallbackFiles)))$(EOL)\
">> $(BUILD_DIR)/Setup.inf
endif
	@$(ECHO) \
"$(EOL)\
[Packages]$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  MdeModulePkg/MdeModulePkg.dec$(EOL)\
  AmiTsePkg/AmiTsePkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalSetupPackages))$(EOL)\
">> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"$(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  UefiBootServicesTableLib$(EOL)\
  UefiRuntimeServicesTableLib$(EOL)\
  DxeServicesTableLib$(EOL)\
  DevicePathLib$(EOL)\
  HiiLib$(EOL)\
  BaseLib$(EOL)\
  PrintLib$(EOL)\
  UefiLib$(EOL)\
  DebugLib$(EOL)\
">> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalSetupLibraryClasses))$(EOL)\
">> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"$(EOL)\
[Protocols]$(EOL)\
  gEfiHiiPackageListProtocolGuid    ## CONSUMES$(EOL)\
  gEfiHiiStringProtocolGuid    ## CONSUMES$(EOL)\
  gEfiHiiDatabaseProtocolGuid    ## CONSUMES$(EOL)\
  gEfiLoadedImageDevicePathProtocolGuid ## CONSUMES$(EOL)\
  gEfiDevicePathProtocolGuid            ## CONSUMES$(EOL)\
  gEfiHiiConfigAccessProtocolGuid  	    ## PRODUCES$(EOL)\
  gEfiHiiConfigRoutingProtocolGuid      ## CONSUMES$(EOL)\
">> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalSetupProtocols))$(EOL)"\
>> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"[Guids]$(EOL)\
  gEfiGlobalVariableGuid    ## CONSUMES$(EOL)\
  gAmiTseSetupEnterGuid\
">> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalSetupGuids))$(EOL)\
">> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"$(EOL)\
[Pcd]$(EOL)\
">> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalSetupPcds))$(EOL)\
">> $(BUILD_DIR)/Setup.inf

	@$(ECHO) \
"$(EOL)\
[Depex]$(EOL)\
  gAmiConInStartedProtocolGuid$(EOL)\
$(EOL)\
[BuildOptions]$(EOL)\
  *_*_*_BUILD_FLAGS=-s$(EOL)\
">> $(BUILD_DIR)/Setup.inf

$(BUILD_DIR)/SetupDefaults.inf : $(BUILD_DIR)/Token.mak $(Setup_DIR)/Setup.mak
	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
#   SETUP_DEFINITIONS, SETUP_FORMSETS, SetupStringFiles, and SetupCallbackFiles$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = SetupDefaults$(EOL)\
  FILE_GUID                      = C8300EA9-869E-42df-AEF2-5D939452A353$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = SetupDefaultsEntry$(EOL)\
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
[Sources]$(EOL)\
 ../AmiCompatibilityPkg/Setup/SetupDefaults.c$(EOL)\
 ../AmiCompatibilityPkg/Setup/Setup.uni$(EOL)\
 ../AmiCompatibilityPkg/Setup/SetupDefaults.vfr$(EOL)\
 "> $(BUILD_DIR)/SetupDefaults.inf
 
ifneq ($(SetupStringFiles),"")
	@$(ECHO) \
"  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(SetupStringFiles)))$(EOL)\
">> $(BUILD_DIR)/SetupDefaults.inf
endif

ifneq ($(SETUP_DEFINITIONS),"")
	@$(ECHO) \
"  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(SETUP_DEFINITIONS)))$(EOL)\
">> $(BUILD_DIR)/SetupDefaults.inf
endif

	@$(ECHO) \
"$(EOL)\
[Packages]$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  MdeModulePkg/MdeModulePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
">> $(BUILD_DIR)/SetupDefaults.inf
  
	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalSetupPackages))$(EOL)\
">> $(BUILD_DIR)/SetupDefaults.inf

	@$(ECHO) \
"$(EOL)\
[LibraryClasses]$(EOL)\
  UefiDriverEntryPoint$(EOL)\
$(EOL)\
[Protocols]$(EOL)\
$(EOL)\
[Guids]$(EOL)\
$(EOL)\
[Pcd]$(EOL)\
$(EOL)\
[Depex]$(EOL)\
  TRUE$(EOL)\
$(EOL)\
[BuildOptions]$(EOL)\
  *_*_*_BUILD_FLAGS=-s$(EOL)\
">> $(BUILD_DIR)/SetupDefaults.inf

