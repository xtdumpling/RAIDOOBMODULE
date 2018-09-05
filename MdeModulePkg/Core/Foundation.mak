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

#**********************************************************************
#  @file
#  Processes PeiCore and DxeCore eLinks
#**********************************************************************
Prepare : $(BUILD_DIR)/PeiCoreHooks.h $(BUILD_DIR)/DxeCoreHooks.h $(BUILD_DIR)/GetPpiName.c $(BUILD_DIR)/GetProtocolName.c

$(BUILD_DIR)/PeiCoreHooks.h :  $(Foundation_DIR)/Foundation.mak $(TOKEN_MAK)
	$(ECHO) \
"#define PEI_CORE_INITIALIZE_HOOKS $(PeiCoreInitialize)\
$(EOL)#define PEI_CORE_MEMORY_INSTALLED_HOOKS $(PeiCoreMemoryInstalled)$(EOL)\
"\
> $@

$(BUILD_DIR)/DxeCoreHooks.h : $(Foundation_DIR)/Foundation.mak $(TOKEN_MAK)
	$(ECHO) "#define DXE_CORE_INITIALIZE_HOOKS $(DxeCoreInitialize)$(EOL)" > $@

ifeq ($(call __ge, $(BUILD_TOOLS_VERSION), 29),yes)
ifneq ($(PEI_ARCHITECTURE),)
ifneq ($(DXE_ARCHITECTURE),)
ENABLE_FOUNDATION_INTERFACE_NAME_TABLE_GENERATION=1
endif
endif
endif

$(BUILD_DIR)/GetPpiName.c $(BUILD_DIR)/GetProtocolName.c : $(Foundation_DIR)/Foundation.mak

ifdef ENABLE_FOUNDATION_INTERFACE_NAME_TABLE_GENERATION
$(BUILD_DIR)/GetPpiName.c:
	$(ECHO) \
"#include <$(PLATFORM_NAME)/$(TARGET)_$(TOOL_CHAIN_TAG)/PpiTable$(PEI_ARCHITECTURE).c>\
$(EOL)CHAR8 *GetPpiName(CONST EFI_GUID* Guid){ return GetPpiNameFromBuildToolGeneratedTable(Guid); }$(EOL)\
" > $@
$(BUILD_DIR)/GetProtocolName.c:
	$(ECHO) \
"#include <$(PLATFORM_NAME)/$(TARGET)_$(TOOL_CHAIN_TAG)/ProtocolTable$(DXE_ARCHITECTURE).c>\
$(EOL)CHAR8 *GetProtocolName(CONST EFI_GUID* Guid){ return GetProtocolNameFromBuildToolGeneratedTable(Guid); }$(EOL)\
" > $@
else
$(BUILD_DIR)/GetPpiName.c:
	$(ECHO) "CHAR8 *GetPpiName(CONST EFI_GUID* Guid){ return NULL; }" > $@
$(BUILD_DIR)/GetProtocolName.c:
	$(ECHO) "CHAR8 *GetProtocolName(CONST EFI_GUID* Guid){ return NULL; }" > $@
endif

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