Prepare :  $(BUILD_DIR)/PlatformSpecificFuncLib.inf

$(BUILD_DIR)/PlatformSpecificFuncLib.inf : $(BUILD_DIR)/token.h 
	@$(ECHO)\
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = PlatformSpecificFuncLib$(EOL)\
  FILE_GUID                      = 107b2fa5-9abf-a13a-c1ab-a1a69a3b1a16$(EOL)\
  MODULE_TYPE                    = BASE$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  LIBRARY_CLASS                  = PlatformSpecificFuncLib$(EOL)\
$(EOL)\
[Packages]$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  MdeModulePkg/MdeModulePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  CryptoPkg/CryptoPkg.dec$(EOL)\
  SmcPkg/SmcPkg.dec$(EOL)\
  SmcPkg/Module/SmcOOB/SmcOOB.dec$(EOL)\
  AmiNetworkPkg/AmiNetworkPkg.dec$(EOL)\
$(EOL)\
[LibraryClasses]$(EOL)\
  IoLib$(EOL)\
  BaseCryptLib$(EOL)\
  UefiBootServicesTableLib$(EOL)\
  UefiRuntimeServicesTableLib$(EOL)\
  BaseMemoryLib$(EOL)\
  DebugLib$(EOL)\
  AmiDxeLib$(EOL)\
"> $(BUILD_DIR)/PlatformSpecificFuncLib.inf
ifeq ($(MEHLOW_SGX_EXPOSE_SUPPORT), 1)
	@$(ECHO)\
"  RngLib$(EOL)\
">> $(BUILD_DIR)/PlatformSpecificFuncLib.inf
endif
	@$(ECHO)\
"[Sources]$(EOL)\
../$(PlatformSpecificFuncLib_DIR)/PlatformSpecificFuncLib.c$(EOL)\
../$(PlatformSpecificFuncLib_DIR)/PlatformSpecificFuncLib.h$(EOL)\
$(EOL)\
[Protocols]$(EOL)\
$(EOL)\
[Guids]$(EOL)\
$(EOL)\
[Pcd]\
">> $(BUILD_DIR)/PlatformSpecificFuncLib.inf

