#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
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
Prepare : $(BUILD_DIR)$(PATH_SLASH)MEFwUpdLcl.inf

$(BUILD_DIR)$(PATH_SLASH)MEFwUpdLcl.inf : $(BUILD_DIR)$(PATH_SLASH)Token.h $(MEFwUpdLcl_DIR)$(PATH_SLASH)MEFwUpdLcl.mak 
	$(ECHO)  \
"$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = MEFwUpdLcl$(EOL)\
  FILE_GUID                      = A11585B7-8FA2-4F1C-AA6F-DD6309469613$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  PI_SPECIFICATION_VERSION       = 0x0001000A$(EOL)\
  ENTRY_POINT                    = MEFwUpdLclEntry$(EOL)\
$(EOL)\
#$(EOL)\
# The following information is for reference only and not required by the build tools.$(EOL)\
#$(EOL)\
#  VALID_ARCHITECTURES           = IA32 X64 IPF EBC$(EOL)\
#$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  ../$(MEFwUpdLcl_DIR)/MEFwUpdLcl.c$(EOL)\
$(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(MEFwUpdLcLPackages))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  MemoryAllocationLib$(EOL)\
  UefiLib$(EOL)\
  $(EOL)\
[Depex]$(EOL)\
	$(subst $(SPACE),$(EOL)$(SPACE),$(MEFwUpdLcLDepex))$(EOL)\
$(EOL)"\
> $(BUILD_DIR)$(PATH_SLASH)MEFwUpdLcl.inf

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************