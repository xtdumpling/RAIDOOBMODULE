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
# Used by makefile and Main.mak. Contains utility
#  definitions, autodetects OS, builds and exports path environment variables,
#  and some common character definitions (such as COMMA, SPACE, and EOL)
#**********************************************************************

# Detect OS, based on Windows WINDIR env variable
# 0 - Windows OS
# 1 - Linux OS
BUILD_OS_WINDOWS := 0
BUILD_OS_LINUX := 1

ifndef BUILD_OS
ifeq ($(OS),Windows_NT)
  # Windows OS
  BUILD_OS := $(BUILD_OS_WINDOWS)
else
  # Linux OS
  BUILD_OS := $(BUILD_OS_LINUX)
endif	#ifdef (windir)
export BUILD_OS
endif	#ifndef BUILD_OS

# Check paths 
# Make sure TOOLS_DIR is defined, otherwise error out!
ifdef TOOLS_DIR
  TOOLS_DIR := $(subst \,/,$(TOOLS_DIR))
else
  $(error TOOLS_DIR is not defined. Set TOOLS_DIR to an absolute path of the build tools directory.)
endif #ifdef (TOOLS_DIR)

ifndef BUILD_DIR
  BUILD_DIR = Build
export BUILD_DIR
endif #ifndef BUILD_DIR
export BUILD_ROOT=$(BUILD_DIR)

# If VEB isn't defined by VeB (or shell), try to guess which one
ifndef VEB
  __TMP_VEB_SUFFIX__:= $(suffix $(wildcard *.veb))
  ifneq ($(__TMP_VEB_SUFFIX__), .veb)
    ifndef __TMP_VEB_SUFFIX__
      $(error No VeB file found! Set VEB to a base name(no extention) of the project .veb file)
    endif
    $(error Multiple project VeB files found! Set VEB to a base name(no extention) of the project .veb file)
  endif
  VEB := $(basename $(wildcard *.veb) )
  $(warning VEB is not defined, using $(VEB).veb)
  export VEB
endif #ifndef VEB

# EDK2 variables
ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
  EFI_TOOLS_DIR := $(TOOLS_DIR)/Bin/Win32
else
  ifeq ("$(shell getconf LONG_BIT)","64")
    EFI_TOOLS_DIR = $(TOOLS_DIR)/Bin/Linux64
  else
    EFI_TOOLS_DIR = $(TOOLS_DIR)/Bin/Linux32
  endif
endif	#ifeq($(BUILD_OS),$(BUILD_OS_WINDOWS))
ifndef EDK_TOOLS_PATH
  WORKSPACE =$(CURDIR)
  EDK_TOOLS_PATH=$(TOOLS_DIR)
  BASE_TOOLS_PATH=$(TOOLS_DIR)
  export WORKSPACE EDK_TOOLS_PATH BASE_TOOLS_PATH
  # Set the paths correctly, based on OS
  ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
    PATH := $(TOOLS_DIR);$(EFI_TOOLS_DIR);$(PATH)
  else
    PATH := $(TOOLS_DIR):$(EFI_TOOLS_DIR):$(PATH)
  endif	#ifeq($(BUILD_OS),$(BUILD_OS_WINDOWS))
endif

# Useful build utilities
JAVA = java
MAKE = make
EDII_BUILD = $(EFI_TOOLS_DIR)/build

# Define some general utilities
ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
  UNIX_UTILS_DIR := $(TOOLS_DIR)/
  RM = del /F /Q
  RMDIR = cmd /C rd /S /Q
  CP = copy
# Can't use plain "PATH_SLASH=\" here because Make treats 
# back slash followed by the new line as a line continuation character.
  PATH_SLASH := $(subst /,\,/)
  PATH_SLASH_ESC = \\
  DOUBLEDOLLAR = $$
  FWBUILD = FWBuild
  FWBUILD_OPTION_CHAR = /
  MKDIR = mkdir
  __gt = $(shell if $1 GTR $2 (echo yes) else (echo no) )
  __ge = $(shell if $1 GEQ $2 (echo yes) else (echo no) )
else
  # Set default shell
  SHELL:=/bin/bash
  # This only needs to be defined on non-Windows OS. NUMBER_OF_PROCESSORS is a standard system variable on Windows.
  NUMBER_OF_PROCESSORS = $(shell cat /proc/cpuinfo | grep processor | wc -l)
  UNIX_UTILS_DIR = 
  RM = rm -fr
  RMDIR = rm -fr
  CP = cp -f
  PATH_SLASH = /
  PATH_SLASH_ESC = /
  DOUBLEDOLLAR = \$$
ifeq ($(call __ge, $(BUILD_TOOLS_VERSION), 27),yes)
  FWBUILD = FWBuild
endif  
  FWBUILD_OPTION_CHAR = -
  MKDIR = mkdir -p
  __gt = $(shell if [ $(1) -gt $(2) ] ; then echo yes ; else echo no ; fi)
  __ge = $(shell if [ $(1) -ge $(2) ] ; then echo yes ; else echo no ; fi)
endif #ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
# $(SPACE) at the end of the ECHO definitions is required to supports commands like "$(ECHO)abc"
# with no space between $(ECHO) and the argument
ECHO = @$(UNIX_UTILS_DIR)echo -e$(SPACE)
ECHO_NO_ESC = @$(UNIX_UTILS_DIR)echo -E$(SPACE)
DATE = $(UNIX_UTILS_DIR)date
CAT = $(UNIX_UTILS_DIR)cat
GAWK = $(UNIX_UTILS_DIR)gawk

# Useful definitions
EOL = \n
COMMA := ,
SPACE :=
SPACE +=
TAB := \t

# Export PATH variable
ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
  # Windows GNU Make Workaround.
  # Windows version of GNU make uses two path variables: PATH and Path.
  # PATH is inherited from the environment.
  # Path is created by make and initialized using value of the PATH variable.
  # However, if there is a path in the PATH that ends with the back slash(\),
  # the Path variable is not properly initialized.
  # It includes portion of the PATH up to the first '\;' occurrence.
  # To workaround the problems we are:

  # replacing '\;' with ';'
  PATH:=$(subst \;,;,$(PATH))
  # and updating the Path variable.
  Path:=$(PATH)
endif	#ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
export $(PATH)

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