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

#*************************************************************************
# $Header: /AptioV/Source/Modules/Ofbd/Ofbd.mak $
#
# $Revision: $
#
# $Date: $
#*************************************************************************
# Revision History
# ----------------
# $Log: $
# 
#*************************************************************************
#<AMI_FHDR_START>
#
# Name:	Ofbd.mak
#
# Description:	Make file for create Ofbd module inf file
#
#<AMI_FHDR_END>
#*************************************************************************
Prepare : $(BUILD_DIR)/OfbdFuncInc.h $(BUILD_DIR)/OfbdFuncElinks.h $(BUILD_DIR)/Ofbd.inf

#----------------------------------------------------------------------------
#           01. Create OfbdFuncInc header file
#----------------------------------------------------------------------------
OFBD_FUNC_INCLUDE_FILES1 = $(subst \,/,$(subst +,$(space),$(OFBD_FUNC_INCLUDE_FILES)))

$(BUILD_DIR)/OfbdFuncInc.h : $(OFBD_FUNC_INCLUDE_FILES1) $(BUILD_DIR)/Token.mak
	$(CAT) $(OFBD_FUNC_INCLUDE_FILES1) > $(BUILD_DIR)/OfbdFuncInc.h

#----------------------------------------------------------------------------
#           02. Create OfbdFuncElinks header file
#----------------------------------------------------------------------------
$(BUILD_DIR)/OfbdFuncElinks.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"// Don't delete this line$(EOL)\
#define OFBD_INIT_FUNC_LIST $(OFBD_INIT_FUNC_LIST)$(EOL)\
#define OFBD_IN_SMM_FUNC_LIST $(OFBD_IN_SMM_FUNC_LIST)$(EOL)\
#define OFBD_NOT_SMM_FUNC_LIST $(OFBD_NOT_SMM_FUNC_LIST)$(EOL)\
// Don't delete this line$(EOL)"\
> $(BUILD_DIR)/OfbdFuncElinks.h

#----------------------------------------------------------------------------
#           03. Create Ofbd module inf file
#----------------------------------------------------------------------------
$(BUILD_DIR)/Ofbd.inf : $(BUILD_DIR)/Token.h $(OFBD_DIR)/Ofbd.mak 
	$(ECHO)  \
" #**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
#**                                                                  **$(EOL)\
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **$(EOL)\
#**                                                                  **$(EOL)\
#**                       All Rights Reserved.                       **$(EOL)\
#**                                                                  **$(EOL)\
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **$(EOL)\
#**                                                                  **$(EOL)\
#**                       Phone: (770)-246-8600                      **$(EOL)\
#**                                                                  **$(EOL)\
#**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
$(EOL)\
$(EOL)\
 [Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = Ofbd$(EOL)\
  FILE_GUID                      = 57E56594-CE95-46ad-9531-3C49310CA7CE$(EOL)\
  MODULE_TYPE                    = $(OFBD_MODULE_TYPES)$(EOL)\
  VERSION_STRING                 = 2.0$(EOL)\
  PI_SPECIFICATION_VERSION       = 0x0001000A$(EOL)\
  ENTRY_POINT                    = AmiOFBDEntryPoint$(EOL)\
$(EOL)\
#$(EOL)\
# The following information is for reference only and not required by the build tools.$(EOL)\
#$(EOL)\
#  VALID_ARCHITECTURES           = IA32 X64 IPF EBC$(EOL)\
#$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(OFBD_FUNC_SOURCES_FILES)))$(EOL)\
  ../$(OFBD_DIR)/Ofbd.c$(EOL)\
$(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_OFBD_PACKAGES))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  FlashLib$(EOL)\
  UefiLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_OFBD_LIBRARIES))$(EOL)\
  $(EOL)\
[Guids]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_OFBD_GUIDS))$(EOL)\
  $(EOL)\
[Protocols]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_OFBD_PROTOCOLS))$(EOL)\
  $(EOL)\
[Depex]$(EOL)\
  $(subst $(SPACE),$(SPACE)AND$(EOL),$(AMI_OFBD_DEPEX))\
$(EOL)\
$(EOL)\
#**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
#**                                                                  **$(EOL)\
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **$(EOL)\
#**                                                                  **$(EOL)\
#**                       All Rights Reserved.                       **$(EOL)\
#**                                                                  **$(EOL)\
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **$(EOL)\
#**                                                                  **$(EOL)\
#**                       Phone: (770)-246-8600                      **$(EOL)\
#**                                                                  **$(EOL)\
#**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
$(EOL)"\
> $(BUILD_DIR)/Ofbd.inf
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