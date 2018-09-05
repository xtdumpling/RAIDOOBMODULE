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
## @file
#  Tool chain initialization file.
#**********************************************************************

# Detect tool chain family
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
GROUP1:=\\\\1
else
GROUP1:=\\1
endif
TOOL_CHAIN_FAMILY:=$(shell $(GAWK) "/.+_FAMILY[[:space:]]*=[[:space:]]*.+[[:space:]]*/{print gensub(/.+_FAMILY[[:space:]]*=[[:space:]]*(.+)[[:space:]]*/, \"$(GROUP1)\",\"g\");exit;}" $(TOOL_DEFINITION_FILE))
GROUP1:=
ifeq ($(TOOL_CHAIN_FAMILY),)
$(error Can't detect the tool chain family. Add "*_<Tool-Chain-Tag>_*_*_FAMILY = <Tool-Chain-Family-Tag>" into your tool chain definition file($(TOOL_DEFINITION_FILE)))
endif

#**********************************************************************
#                  Microsoft Tool Chain Initialization
#**********************************************************************
ifeq ($(TOOL_CHAIN_FAMILY),MSFT)
ifeq ($(wildcard $(TOOLS_DIR)/SetMicrosoftCompilerEnvironment.mak),$(wildcard $(TOOLS_DIR)/SetMicrosoftCompilerEnvironment.mak))
include $(TOOLS_DIR)/SetMicrosoftCompilerEnvironment.mak
else
  ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
    $(error Tool chain $(TOOL_CHAIN_TAG) is not supported on Linux. Update TOOL_CHAIN_TAG SDL token to change the active tool chain.)
  endif

  # Check if required variables are set. 
  # The variable can be either an environment variable or an SDL token.
  ifndef IA32_TOOLS_DIR
    # If IA32_TOOLS_DIR is not defined,let's use legacy CCX86DIR environment variable.
    # We can't use $(CCX86DIR) because Windows environment variables are case insensitive,
    # and make variables are case sensitive.
    # We are using shell function to read variable using Windows case conventions.
    IA32_TOOLS_DIR:=$(shell if not "%CCX86DIR%"=="" echo %CCX86DIR%)
  endif
  ifeq ($(IA32_TOOLS_DIR),)
    $(error Location of the 32-bit compiler is not defined! It must be specified using IA32_TOOLS_DIR env.variable or SDL token)
  endif
  export IA32_TOOLS_DIR
  export CCX86DIR:=$(IA32_TOOLS_DIR)
  ifeq ($(X64_SUPPORT), 1)
    ifndef X64_TOOLS_DIR
      # If X64_TOOLS_DIR is not defined,let's use legacy CCX64DIR environment variable.
      # We can't use $(CCX64DIR) because Windows environment variables are case insensitive,
      # and make variables are case sensitive.
      # We are using shell function to read variable using Windows case conventions.
      X64_TOOLS_DIR:=$(shell if not "%CCX64DIR%"=="" echo %CCX64DIR%)
    endif
    ifeq ($(X64_TOOLS_DIR),)
      $(error Location of the 64-bit compiler is not defined! It must be specified using X64_TOOLS_DIR env.variable or SDL token)
    endif
    export X64_TOOLS_DIR
    export CCX64DIR:=$(X64_TOOLS_DIR)
  endif #ifeq ($(X64_SUPPORT), 1)
endif # if SetMicrosoftCompilerEnvironment.mak exists
  # Set optimization flags for Microsoft tool chains
  ifeq ($(OPTIMIZATION), 0)
    EXTERNAL_CC_FLAGS +=  /Od 
  else
    EXTERNAL_CC_FLAGS +=  /O1ib2 
  endif
  # Set warnings level
  ifeq ($(STRICT_BUILD_MODE), 0)
    EXTERNAL_CC_FLAGS +=  /W3 /wd4091 /wd4311 /wd4312 
  else
    EXTERNAL_CC_FLAGS +=  /W4
  endif
endif #ifeq ($(TOOL_CHAIN_FAMILY),MSFT)

#**********************************************************************
#                  GCC Tool Chain Initialization
#**********************************************************************
ifeq ($(TOOL_CHAIN_FAMILY),GCC)
  ifeq ($(call __ge, $(BUILD_TOOLS_VERSION), 25),yes)
    ifeq ($(TARGET),RELEASE)
      export EXTERNAL_GENFW_FLAGS:=--nodebuginfo
    endif
  else
    # LINAROWIN requires Build Tools 25
    ifeq ($(TOOL_CHAIN_TAG),LINAROWIN)
      $(error LINAROWIN tool chain requires Build Tools 25 or newer)
    endif
  endif

  # Set default locations of Linux GCC tools
  ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
    ifeq ($(INTEL_ARCHITECTURE_SUPPORT), 1)
      ifndef IA32_TOOLS_DIR
        IA32_TOOLS_DIR=/usr/bin
      endif
      ifeq ($(X64_SUPPORT), 1)
        ifndef X64_TOOLS_DIR
          X64_TOOLS_DIR=/usr/bin
        endif
      endif # ifeq ($(X64_SUPPORT), 1)
    else  # ifeq ($(INTEL_ARCHITECTURE_SUPPORT), 1)
      ifeq ($(DXE_ARCHITECTURE), AARCH64)
        ifndef AARCH64_TOOL_PREFIX
          AARCH64_TOOL_PREFIX=aarch64-linux-gnu-
        endif
      else # ifeq ($(DXE_ARCHITECTURE), AARCH64)
        ifndef ARM_TOOLS_DIR
          ARM_TOOLS_DIR=/usr/bin
        endif
        ifndef ARM_TOOL_PREFIX
          ARM_TOOL_PREFIX=arm-linux-gnueabi-
        endif
      endif # ifeq ($(DXE_ARCHITECTURE), AARCH64)
    endif # ifeq ($(INTEL_ARCHITECTURE_SUPPORT), 1)
  endif # ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))

  # Check if required variables are set. 
  # The variable can be either an environment variable or an SDL token.
  ifeq ($(INTEL_ARCHITECTURE_SUPPORT), 1)
    ifndef IA32_TOOLS_DIR
      $(error Location of the 32-bit compiler is not defined! It must be specified using IA32_TOOLS_DIR and IA32_TOOL_PREFIX env.variables or SDL tokens)
    endif
    export IA32_TOOLS_DIR IA32_TOOL_PREFIX
    ifeq ($(X64_SUPPORT), 1)
      ifndef X64_TOOLS_DIR
        $(error Location of the 64-bit compiler is not defined! It must be specified using X64_TOOLS_DIR and X64_TOOL_PREFIX env.variables or SDL tokens)
      endif
      export X64_TOOLS_DIR X64_TOOL_PREFIX
    endif # ifeq ($(X64_SUPPORT), 1)
  else  # ifeq ($(INTEL_ARCHITECTURE_SUPPORT), 1)
    ifeq ($(DXE_ARCHITECTURE), AARCH64)
      ifndef AARCH64_TOOLS_DIR
        $(error Compiler location is not defined! It must be specified using AARCH64_TOOLS_DIR and AARCH64_TOOL_PREFIX env.variables or SDL tokens)
      endif
      export AARCH64_TOOLS_DIR AARCH64_TOOL_PREFIX
    else # ifeq ($(DXE_ARCHITECTURE), AARCH64)
      ifndef ARM_TOOLS_DIR
        $(error Compiler location is not defined! It must be specified using ARM_TOOLS_DIR and ARM_TOOL_PREFIX env.variables or SDL tokens)
      endif
      export ARM_TOOLS_DIR ARM_TOOL_PREFIX
    endif # ifeq ($(DXE_ARCHITECTURE), AARCH64)
  endif # ifeq ($(INTEL_ARCHITECTURE_SUPPORT), 1)
endif #ifeq ($(TOOL_CHAIN_FAMILY),GCC)

#**********************************************************************
#          Generic Initilization Shared By All Tool Chains
#**********************************************************************
ifeq ($(DEBUG_CODE), 1)
EXTERNAL_CC_FLAGS +=  -DEFI_DEBUG 
else
EXTERNAL_CC_FLAGS +=  -DMDEPKG_NDEBUG 
endif
export EXTERNAL_CC_FLAGS
export FONT_TOOL := FontTool -F 2.1 -C $(FONT_INI_FILE)
export FONT_TOOL_TMP_FILE:=$(ABS_OUTPUT_DIR)/$(TARGET)_$(TOOL_CHAIN_TAG)/font.tmp
export ASL_COMPILER

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