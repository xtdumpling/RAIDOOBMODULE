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
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
$(error Microsoft tool chain is not supported on Linux. Update TOOL_CHAIN_TAG SDL token to change the active tool chain)
endif
ifneq ($(INTEL_ARCHITECTURE_SUPPORT),1)
$(error Microsoft tool chain is only support for Intel Architecture project. Update TOOL_CHAIN_TAG SDL token to change the active tool chain)
endif

ifeq ($(TOOL_CHAIN_TAG),VS2015)
  ifndef VSINSTALLDIR
    # Try to detect Visual Studio 2015 Location
    ifdef VS140COMNTOOLS
      VSINSTALLDIR:=$(subst Common7\Tools\,,$(VS140COMNTOOLS))
    else
      $(error VS2015 tool chiain is selected, but Visual Studio 2015 is not found. Update TOOL_CHAIN_TAG SDL token to change the active tool chain)
    endif
  endif
  ifdef VSINSTALLDIR
    IA32_TOOLS_DIR:=$(VSINSTALLDIR)VC\bin
    X64_TOOLS_DIR=$(VSINSTALLDIR)VC\bin\x86_amd64
    ifeq ($(subst Program Files (x86),,$(VSINSTALLDIR)),$(VSINSTALLDIR))
      PATH := C:\Program Files\Microsoft SDKs\Windows\v7.1A\Bin;$(PATH)
    else
      PATH := C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin;$(PATH)	
    endif
    export PATH := $(VSINSTALLDIR)VC\bin;$(VSINSTALLDIR)Common7\IDE;$(PATH)
  endif
endif # ifeq ($(TOOL_CHAIN_TAG),VS2015)

# Check if variables that define location of the Microsoft compiler are set. 
# The variable can be either an environment variable or an SDL token.
# If IA32_TOOLS_DIR/X64_TOOLS_DIR is not defined, we use legacy CCX86DIR/CCX64DIR environment variables.
# We can't use $(CCX86DIR) or  $(CCX64DIR) because Windows environment variables are case insensitive,
# whereas make variables are case sensitive.
# We are using shell function to read variable using Windows case conventions.
ifndef IA32_TOOLS_DIR
  IA32_TOOLS_DIR:=$(shell if not "%CCX86DIR%"=="" echo %CCX86DIR%)
endif
ifndef X64_TOOLS_DIR
  X64_TOOLS_DIR:=$(shell if not "%CCX64DIR%"=="" echo %CCX64DIR%)
endif

ifeq ($(IA32_TOOLS_DIR),)
  $(error Location of the 32-bit compiler is not defined! It must be specified using IA32_TOOLS_DIR env.variable or SDL token)
endif

export IA32_TOOLS_DIR
export CCX86DIR:=$(IA32_TOOLS_DIR)
ifeq ($(X64_SUPPORT), 1)
  ifeq ($(X64_TOOLS_DIR),)
    $(error Location of the 64-bit compiler is not defined! It must be specified using X64_TOOLS_DIR env.variable or SDL token)
  endif
  export X64_TOOLS_DIR
  export CCX64DIR:=$(X64_TOOLS_DIR)
endif #ifeq ($(X64_SUPPORT), 1)
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