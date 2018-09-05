#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
# $Header:  $
#
# $Revision: $
#
# $Date:  $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	DefaultFixedBootOrder.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/DefaultFixedBootOrder.h\
          $(BUILD_DIR)/FixedBootOrderSetupData.h\
          $(BUILD_DIR)/DefaultFixedBootOrder.sd\
          $(BUILD_DIR)/FboGroupForm.dec\
          $(BUILD_DIR)/FboGroupForm.inf\
          $(BUILD_DIR)/FboGroupForm.vfr

# We are trying to convert a list of comma separated functions to a list of comma 
# separated function name strings.
# The original list of function may contain C comment blocks /* */

## Generate Build/DefaultFixedBootOrder.h ##
$(BUILD_DIR)/DefaultFixedBootOrder.h : $(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.mak \
                                       $(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.c \
                                       $(BUILD_DIR)\Token.h
	$(ECHO) \
"#include <$(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)$(AMITSE_TARGET_ARCH)\Build\Setup\DEBUG\SetupStrDefs.h>$(EOL)\
$(EOL)\
//--------------------------------------------------------------------------- $(EOL)\
// Constant and Variables declarations $(EOL)\
//--------------------------------------------------------------------------- $(EOL)\
typedef enum { $(EOL)\
//    BoTagLegacyXxx $(EOL)\
//    BoTagUefiXxx $(EOL)\
//    BoTagXxx $(EOL)\
//--standard device type don't modify--// $(EOL)\
    BoTagLegacyFloppy = 1,	//1 $(EOL)\
    BoTagLegacyHardDisk,	//2 $(EOL)\
    BoTagLegacyCdrom,		//3 $(EOL)\
    BoTagLegacyPcmcia,		//4 $(EOL)\
    BoTagLegacyUsb,		    //5 $(EOL)\
    BoTagLegacyEmbedNetwork,	//6 $(EOL)\
    BoTagLegacyBevDevice = 0x80,//7 $(EOL)\
//--standard device type don't modify--// end $(EOL)\
    BoTagLegacyUSBFloppy = 8, $(EOL)\
    BoTagLegacyUSBHardDisk, $(EOL)\
    BoTagLegacyUSBCdrom, $(EOL)\
    BoTagLegacyUSBKey, $(EOL)\
    BoTagLegacyUSBLan, $(EOL)\
    BoTagLegacyHardDisk1, $(EOL)\
    BoTagLegacyHardDisk2, $(EOL)\
    BoTagLegacyHardDisk3, $(EOL)\
    BoTagLegacyHardDisk4, $(EOL)\
    BoTagLegacyHardDisk5, $(EOL)\
    BoTagUefiUsbFloppy,				//(EIP96232+) $(EOL)\
    BoTagUefiHardDisk, $(EOL)\
    BoTagUefiHardDisk1, $(EOL)\
    BoTagUefiHardDisk2, $(EOL)\
    BoTagUefiHardDisk3, $(EOL)\
    BoTagUefiHardDisk4, $(EOL)\
    BoTagUefiHardDisk5, $(EOL)\
    BoTagUefiCdrom, $(EOL)\
    BoTagUefiUsbHardDisk, $(EOL)\
    BoTagUefiUsbKey, $(EOL)\
    BoTagUefiUsbLan, $(EOL)\
    BoTagUefiUsb, $(EOL)\
#if SHELL_GROUP_SUPPORT $(EOL)\
    BoTagUefiApplication,			//(EIP115686+) $(EOL)\
#endif $(EOL)\
    BoTagUefiUsbCdrom, $(EOL)\
    BoTagUefiNetWork, $(EOL)\
    BoTagUefi, $(EOL)\
    BoTagEmbeddedShell $(EOL)\
} FIXED_BOOT_OPTION_TAG; $(EOL)\
#define FBO_CHANGE_DEVICE_NAME_FUNCS $(FBO_ChangeDeviceNameFunctions)$(EOL)\
#define FBO_UEFI_DEVICE $(FBO_UEFI_DEVICE)$(EOL)\
#define FBO_LEGACY_DEVICE $(FBO_LEGACY_DEVICE)$(EOL)\
#define FBO_DUAL_DEVICE $(FBO_DUAL_DEVICE)$(EOL)"\
>$(BUILD_DIR)/DefaultFixedBootOrder.h


## Generate Build/FixedBootOrderSetupData.h ##
$(BUILD_DIR)/FixedBootOrderSetupData.h : $(BUILD_DIR)/token.mak $(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.mak
	$(ECHO) \
"$(if $(FIXED_BOOT_ORDER_SETUP_DEFINITIONS), $(foreach S_DEF, $(FIXED_BOOT_ORDER_SETUP_DEFINITIONS),#include<$(S_DEF)>$(EOL)))"\
>$(BUILD_DIR)/FixedBootOrderSetupData.h
	$(ECHO) "#define FIXED_BOOT_ORDER_SETUP_INFORM $(FIXED_BOOT_ORDER_SETUP_INFORM)"\
>>$(BUILD_DIR)/FixedBootOrderSetupData.h


## Generate Build/FboGroupForm.dec ##
$(BUILD_DIR)/FboGroupForm.dec : $(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.mak $(BUILD_DIR)/token.h
	$(ECHO)  \
"$(EOL)\
[Defines]$(EOL)\
  DEC_SPECIFICATION              = 0x00010005$(EOL)\
  PACKAGE_NAME                   = FboGroupFormLib$(EOL)\
  PACKAGE_GUID                   = 072a622d-0a5f-4d69-ab23-81222cc85c12$(EOL)\
  PACKAGE_VERSION                = 0.1$(EOL)\
$(EOL)\
[Guids]$(EOL)\
gFixedBootOrderSetupGuid = {0x7e07911a, 0x4807, 0x4b0f, {0xa4, 0x74, 0xf5, 0x43, 0x1c, 0xa9, 0x07, 0xb4}}$(EOL)\
"\
 >$@


#####################################################################################
## Below section for produce the Build/DefaultFixedBootOrder.sd, Build/FboGroupInfo.inf
## and Build/FboGroupInfo.vfr
## 
## It will auto generate FixedBootOrder Setup vfr Data, Form item, Goto item ...etc.
## 
## OEM/ODM can use Token to dynamic change it, please refer Token
## FBO_UEFI_DEVICE, FBO_LEGACY_DEVICE, FBO_DUAL_DEVICE in DefaultFixedBootOrder.sdl
## If anyone want to modify code, please study makfile and gawk that help to understand.
##
## Known Issue:
## Build Tool - make.exe(GNU Make version 3.78.1)
## $(if <condition>,<then-part>,<else-part>) <-This statement can't use "else part" 
#####################################################################################

## Define Match flag in DefaultFixedBootOrder.sd file ##
MATCH_UEFI_STRING=MAKEFILE_INSERT_UEFI_STRING_FLAG
MATCH_LEGACY_STRING=MAKEFILE_INSERT_LEGACY_STRING_FLAG
MATCH_DUAL_STRING=MAKEFILE_INSERT_DUAL_STRING_FLAG
MATCH_STR_LEGACY_BOOT_ORDER_STRING=MAKEFILE_INSERT_STR_LEGACY_BOOT_ORDER_STRING_FLAG
MATCH_STR_UEFI_BOOT_ORDER_STRING=MAKEFILE_INSERT_STR_UEFI_BOOT_ORDER_STRING_FLAG
MATCH_STR_DUAL_BOOT_ORDER_STRING=MAKEFILE_INSERT_STR_DUAL_BOOT_ORDER_STRING_FLAG
MATCH_FORM_SET=MAKEFILE_INSERT_FORM_SET_FLAG
MATCH_DEVICE_VALUE=MAKEFILE_INSERT_DEVICE_VALUE_FLAG
MATCH_UEFI_DEVICE_VALUE=MAKEFILE_INSERT_UEFI_DEVICE_VALUE_FLAG
MATCH_LEGACY_DEVICE_VALUE=MAKEFILE_INSERT_LEGACY_DEVICE_VALUE_FLAG
MATCH_DUAL_DEVICE_VALUE=MAKEFILE_INSERT_DUAL_DEVICE_VALUE_FLAG
MATCH_UEFI_FORM_LABEL_AUTO_ID=MAKEFILE_INSERT_UEFI_FORM_LABEL_AUTO_ID_FLAG
MATCH_LEGACY_FORM_LABEL_AUTO_ID=MAKEFILE_INSERT_LEGACY_FORM_LABEL_AUTO_ID_FLAG


## Define Match pattern in FboGroupForm.inf file ##
ABS_PATH_FBO_GROUP_FORM_FOLDER=$(subst \,/,$(FboGroupForm_DIR))
ABS_PATH_DEFAULT_FBO_FOLDER=$(subst \,/,$(DefaultFixedBootOrder_DIR))

FBO_GROUP_FORM_C_FILE=FboGroupForm.c
ABS_PATH_FBO_GROUP_FORM_C_FILE=../$(ABS_PATH_FBO_GROUP_FORM_FOLDER)/FboGroupForm.c

FIXED_BOOT_ORDER_HII_C_FILE=FixedBootOrderHii.c
ABS_PATH_FIXED_BOOT_ORDER_HII_C_FILE=../$(ABS_PATH_FBO_GROUP_FORM_FOLDER)/FixedBootOrderHii.c

FIXED_BOOT_ORDER_HII_H_FILE=FixedBootOrderHii.h
ABS_PATH_FIXED_BOOT_ORDER_HII_H_FILE=../$(ABS_PATH_FBO_GROUP_FORM_FOLDER)/FixedBootOrderHii.h

DEFAULT_FBO_UNI_FILE=\#\#FBO_TAG DefaultFixedBootOrder.uni\#\#
ABS_PATH_DEFAULT_FBO_UNI_FILE=../$(ABS_PATH_DEFAULT_FBO_FOLDER)/DefaultFixedBootOrder.uni


## Define flow condition ##
SUPPRESS_IF=suppressif
ID_EQ_VAL_LIST=ideqvallist
ONE_OF_VAR_ID=oneof varid
PROMPT=prompt
HELP=help
OPTION_TEXT=option text
OPTION_COND=MANUFACTURING OR_SYMBOL DEFAULT
END_ONE_OF=endoneof
END_IF=endif
GOTO=goto
FORM=form
FORM_ID=formid
TITLE=title
LABEL=label
END_FORM=endform

NUMBERS :=

#define ITERATE 
#$(if $(word ${1}, ${NUMBERS}),,\
# $(eval NUMBERS+=$(words ${NUMBERS}))\
#  $(call ITERATE,${1})\
#) 
#endef

#$(call ITERATE,5)

## For something reason, we can't use makefile to produce array... ##
## This array like as counter to use ##
INDEX_ARRAY=0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36
NONE_ZERO_INDEX_ARRAY := $(filter-out 0,$(INDEX_ARRAY))

## To replace Comma(,) to Space( ) 
FBO_UEFI_DEVICE := $(subst $(COMMA),$(SPACE),$(FBO_UEFI_DEVICE))
FBO_LEGACY_DEVICE := $(subst $(COMMA),$(SPACE),$(FBO_LEGACY_DEVICE))
FBO_DUAL_DEVICE := $(subst $(COMMA),$(SPACE),$(FBO_DUAL_DEVICE))
FBO_DUAL_DEVICE1 := $(addprefix DUAL_,$(FBO_DUAL_DEVICE)) 

## Get each mode(Uefi, Legacy, Dual) index array size.
FBO_UEFI_INDEX := $(wordlist 1,$(words $(FBO_UEFI_DEVICE)),$(NONE_ZERO_INDEX_ARRAY))
FBO_LEGACY_INDEX := $(wordlist 1,$(words $(FBO_LEGACY_DEVICE)),$(NONE_ZERO_INDEX_ARRAY))
FBO_DUAL_INDEX := $(wordlist 1,$(words $(FBO_DUAL_DEVICE1)),$(NONE_ZERO_INDEX_ARRAY))

## Prepare Uefi String ##
$(UEFI_STRING) : $(BUILD_DIR)/token.mak
UEFI_STRING := $(foreach INDEX, $(FBO_UEFI_DEVICE),\
STRING_TOKEN(STR_BOOT_$(INDEX))$(EOL))

## Prepare Legacy String ##
$(LEGACY_STRING) : $(BUILD_DIR)/token.mak
LEGACY_STRING := $(foreach INDEX, $(FBO_LEGACY_DEVICE),\
STRING_TOKEN(STR_BOOT_$(INDEX))$(EOL))

## Prepare Dual String ##
$(DUAL_STRING) : $(BUILD_DIR)/token.mak
DUAL_STRING := $(foreach INDEX, $(FBO_DUAL_DEVICE1),\
STRING_TOKEN(STR_BOOT_$(INDEX))$(EOL))

## Prepare StrBootOrder String ##
$(STR_LEGACY_BOOT_ORDER_STRING) : $(BUILD_DIR)/token.mak
STR_LEGACY_BOOT_ORDER_STRING := $(foreach INDEX, $(NONE_ZERO_INDEX_ARRAY),\
STRING_TOKEN(STR_LEGACY_BOOT_ORDER_$(INDEX))$(EOL))

## Prepare UEFI StrBootOrder String ##
$(STR_UEFI_BOOT_ORDER_STRING) : $(BUILD_DIR)/token.mak
STR_UEFI_BOOT_ORDER_STRING := $(foreach INDEX, $(NONE_ZERO_INDEX_ARRAY),\
STRING_TOKEN(STR_UEFI_BOOT_ORDER_$(INDEX))$(EOL))

## Prepare DUAL StrBootOrder String ##
$(STR_DUAL_BOOT_ORDER_STRING) : $(BUILD_DIR)/token.mak
STR_DUAL_BOOT_ORDER_STRING := $(foreach INDEX, $(NONE_ZERO_INDEX_ARRAY),\
STRING_TOKEN(STR_DUAL_BOOT_ORDER_$(INDEX))$(EOL))

## Prepare Uefi form label auto id ##
$(FBO_UEFI_FORM_LABEL_AUTO_ID) : $(BUILD_DIR)/token.mak
FBO_UEFI_FORM_LABEL_AUTO_ID := $(foreach INDEX, $(FBO_UEFI_DEVICE),\
AUTO_ID($(INDEX)_BOOT_FORM_LABEL)$(EOL))

## Prepare Legacy form label auto id ##
$(FBO_LEGACY_FORM_LABEL_AUTO_ID) : $(BUILD_DIR)/token.mak
FBO_LEGACY_FORM_LABEL_AUTO_ID := $(foreach INDEX, $(FBO_LEGACY_DEVICE),\
AUTO_ID($(INDEX)_BOOT_FORM_LABEL)$(EOL))

## Prepare Form Set ##
$(FORM_SET_DATA) : $(BUILD_DIR)/token.mak
FIXED_BOOT_ORDER_DISPLAY_FORM_SET := $(subst $(COMMA),$(SPACE),$(FIXED_BOOT_ORDER_DISPLAY_FORM_SET))
FIXED_BOOT_ORDER_DISPLAY_FORM_SET := $(foreach STR, $(FIXED_BOOT_ORDER_DISPLAY_FORM_SET),defined($(STR)))
FIXED_BOOT_ORDER_DISPLAY_FORM_SET := $(subst $(SPACE), D_OR_SYMBOL ,$(FIXED_BOOT_ORDER_DISPLAY_FORM_SET))
FORM_SET_DATA :=\#if $(FIXED_BOOT_ORDER_DISPLAY_FORM_SET)

## Prepare Device Value ##
$(DEVICE_VALUE) : $(BUILD_DIR)/token.mak
DEVICE_VALUE := $(foreach INDEX, $(FBO_DUAL_INDEX),\
\#define Device$(INDEX) $(word $(INDEX),$(INDEX_ARRAY))$(EOL))

## Prepare Uefi Device Value ##
$(UEFI_DEVICE_VALUE) : $(BUILD_DIR)/token.mak
UEFI_DEVICE_VALUE := $(foreach INDEX, $(FBO_UEFI_INDEX),\
\#define $(word $(INDEX),$(FBO_UEFI_DEVICE))_VALUE $(word $(INDEX),$(INDEX_ARRAY))$(EOL))

## Prepare Legacy Device Value ##
$(LEGACY_DEVICE_VALUE) : $(BUILD_DIR)/token.mak
LEGACY_DEVICE_VALUE := $(foreach INDEX, $(FBO_LEGACY_INDEX),\
\#define $(word $(INDEX),$(FBO_LEGACY_DEVICE))_VALUE $(word $(INDEX),$(INDEX_ARRAY))$(EOL))

## Prepare Dual Device Value ##
$(DUAL_DEVICE_VALUE) : $(BUILD_DIR)/token.mak
DUAL_DEVICE_VALUE := $(foreach INDEX, $(FBO_DUAL_INDEX),\
\#define $(word $(INDEX),$(FBO_DUAL_DEVICE1))_VALUE $(word $(INDEX),$(INDEX_ARRAY))$(EOL))

## Prepare Disable Value ##
ifeq ("1", "$(FIXED_BOOT_INCLUDE_DISABLED_OPTION)")
UEFI_DEVICE_VALUE += \
\#define UEFI_DISABLED_VALUE $(word $(words $(FBO_UEFI_DEVICE)), $(NONE_ZERO_INDEX_ARRAY))$(EOL)
UEFI_DISABLED_OPTION = $(OPTION_TEXT) = STRING_TOKEN(STR_DISABLED), value = UEFI_DISABLED_VALUE, flags = 0;$(EOL)

LEGACY_DEVICE_VALUE += \
\#define LEGACY_DISABLED_VALUE $(word $(words $(FBO_LEGACY_DEVICE)), $(NONE_ZERO_INDEX_ARRAY))$(EOL)
LEGACY_DISABLED_OPTION = $(OPTION_TEXT) = STRING_TOKEN(STR_DISABLED), value = LEGACY_DISABLED_VALUE, flags = 0;$(EOL)

DUAL_DEVICE_VALUE += \
\#define DUAL_DISABLED_VALUE $(word $(words $(FBO_DUAL_DEVICE1)), $(NONE_ZERO_INDEX_ARRAY))$(EOL)
DUAL_DISABLED_OPTION = $(OPTION_TEXT) = STRING_TOKEN(STR_DISABLED), value = DUAL_DISABLED_VALUE, flags = 0;$(EOL)
endif

## Prepare Gawk tool Uefi Data ##
$(GAWK_UEFI_DEVICE) : $(BUILD_DIR)/token.mak
GAWK_UEFI_DEVICE := $(subst $(SPACE)$(SPACE),+,$(FBO_UEFI_DEVICE))

## Prepare Gawk tool Legacy Data ##
$(GAWK_LEGACY_DEVICE) : $(BUILD_DIR)/token.mak
GAWK_LEGACY_DEVICE := $(subst $(SPACE)$(SPACE),+,$(FBO_LEGACY_DEVICE))

## Prepare Gawk tool Dual Data ##
$(GAWK_DUAL_DEVICE) : $(BUILD_DIR)/token.mak
GAWK_DUAL_DEVICE := $(subst $(SPACE),+,$(FBO_DUAL_DEVICE1))


## Generate Build/DefaultFixedBootOrder.sd by gawk.exe ##
$(BUILD_DIR)/DefaultFixedBootOrder.sd : $(BUILD_DIR)/token.mak \
										$(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.sd \
										$(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.mak \
										$(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk
	@cat $(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.sd \
		| gawk "{sub(/$(MATCH_UEFI_STRING)/, \"$(UEFI_STRING)\"); print}" \
	    | gawk "{sub(/$(MATCH_LEGACY_STRING)/, \"$(LEGACY_STRING)\"); print}" \
	    | gawk "{sub(/$(MATCH_DUAL_STRING)/, \"$(DUAL_STRING)\"); print}" \
	    | gawk "{sub(/$(MATCH_STR_LEGACY_BOOT_ORDER_STRING)/, \"$(STR_LEGACY_BOOT_ORDER_STRING)\"); print}" \
	    | gawk "{sub(/$(MATCH_STR_UEFI_BOOT_ORDER_STRING)/, \"$(STR_UEFI_BOOT_ORDER_STRING)\"); print}" \
   	    | gawk "{sub(/$(MATCH_STR_DUAL_BOOT_ORDER_STRING)/, \"$(STR_DUAL_BOOT_ORDER_STRING)\"); print}" \
	    | gawk "{sub(/$(MATCH_UEFI_FORM_LABEL_AUTO_ID)/, \"$(FBO_UEFI_FORM_LABEL_AUTO_ID)\"); print}" \
	    | gawk "{sub(/$(MATCH_LEGACY_FORM_LABEL_AUTO_ID)/, \"$(FBO_LEGACY_FORM_LABEL_AUTO_ID)\"); print}" \
	    | gawk "{sub(/$(MATCH_FORM_SET)/, \"$(FORM_SET_DATA)\"); print}" \
	    | gawk "{sub(/$(MATCH_DEVICE_VALUE)/, \"$(DEVICE_VALUE)\"); print}" \
	    | gawk "{sub(/$(MATCH_UEFI_DEVICE_VALUE)/, \"$(UEFI_DEVICE_VALUE)\"); print}" \
	    | gawk "{sub(/$(MATCH_LEGACY_DEVICE_VALUE)/, \"$(LEGACY_DEVICE_VALUE)\"); print}" \
	    | gawk "{sub(/$(MATCH_DUAL_DEVICE_VALUE)/, \"$(DUAL_DEVICE_VALUE)\"); print}" \
	>$(BUILD_DIR)/DefaultFixedBootOrder.sd
	gawk -f $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk \
		-v TYPE=UEFI -v DEVICE=$(GAWK_UEFI_DEVICE) -v DUAL_TOKEN=$(FBO_DUAL_MODE)$(CSM_SUPPORT)\
		-v DISABLED=$(FIXED_BOOT_INCLUDE_DISABLED_OPTION) \
		$(BUILD_DIR)/DefaultFixedBootOrder.sd
	@cat $(BUILD_DIR)/AutoGenFboData.sd > $(BUILD_DIR)/DefaultFixedBootOrder.sd
	gawk -f $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk \
		-v TYPE=LEGACY -v DEVICE=$(GAWK_LEGACY_DEVICE) -v DUAL_TOKEN=$(FBO_DUAL_MODE)$(CSM_SUPPORT)\
		-v DISABLED=$(FIXED_BOOT_INCLUDE_DISABLED_OPTION)\
		$(BUILD_DIR)/DefaultFixedBootOrder.sd
	@cat $(BUILD_DIR)/AutoGenFboData.sd > $(BUILD_DIR)/DefaultFixedBootOrder.sd
	gawk -f $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk \
		-v TYPE=DUAL -v DEVICE=$(GAWK_DUAL_DEVICE)\
		-v DISABLED=$(FIXED_BOOT_INCLUDE_DISABLED_OPTION)\
		$(BUILD_DIR)/DefaultFixedBootOrder.sd
	@cat $(BUILD_DIR)/AutoGenFboData.sd > $(BUILD_DIR)/DefaultFixedBootOrder.sd
	gawk -f $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk \
		-v TYPE=UEFI_GROUP_GOTO -v DEVICE=$(GAWK_UEFI_DEVICE)\
		$(BUILD_DIR)/DefaultFixedBootOrder.sd
	@cat $(BUILD_DIR)/AutoGenFboData.sd > $(BUILD_DIR)/DefaultFixedBootOrder.sd
	gawk -f $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk \
		-v TYPE=LEGACY_GROUP_GOTO -v DEVICE=$(GAWK_LEGACY_DEVICE)\
		$(BUILD_DIR)/DefaultFixedBootOrder.sd
	@cat $(BUILD_DIR)/AutoGenFboData.sd > $(BUILD_DIR)/DefaultFixedBootOrder.sd


## Generate Build/FboGroupForm.inf by gawk.exe ##
$(BUILD_DIR)/FboGroupForm.inf :  $(BUILD_DIR)/token.mak \
                                 $(FboGroupForm_DIR)/FboGroupForm.inf \
								 $(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.mak \
								 $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk						 
	@cat $(FboGroupForm_DIR)/FboGroupForm.inf \
	    | gawk "{sub(/$(FBO_GROUP_FORM_C_FILE)/, \"$(ABS_PATH_FBO_GROUP_FORM_C_FILE)\"); print}" \
	    | gawk "{sub(/$(DEFAULT_FBO_UNI_FILE)/, \"$(ABS_PATH_DEFAULT_FBO_UNI_FILE)\"); print}" \
	    | gawk "{sub(/$(FIXED_BOOT_ORDER_HII_C_FILE)/, \"$(ABS_PATH_FIXED_BOOT_ORDER_HII_C_FILE)\"); print}" \
	    | gawk "{sub(/$(FIXED_BOOT_ORDER_HII_H_FILE)/, \"$(ABS_PATH_FIXED_BOOT_ORDER_HII_H_FILE)\"); print}" \
	>$(BUILD_DIR)/FboGroupForm.inf
                      
## Generate Build/FboGroupForm.vfr by gawk.exe ##                              
$(BUILD_DIR)/FboGroupForm.vfr :  $(BUILD_DIR)/token.mak \
                                 $(FboGroupForm_DIR)/FboGroupForm.vfr \
								 $(DefaultFixedBootOrder_DIR)/DefaultFixedBootOrder.mak \
								 $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk
	gawk -f $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk \
		-v TYPE=UEFI_GROUP_FORM -v DEVICE=$(GAWK_UEFI_DEVICE)\
		$(FboGroupForm_DIR)/FboGroupForm.vfr
	@cat $(BUILD_DIR)/AutoGenFboData.sd > $(BUILD_DIR)/FboGroupForm.vfr
	gawk -f $(DefaultFixedBootOrder_DIR)/AutoGenFboData.gawk \
		-v TYPE=LEGACY_GROUP_FORM -v DEVICE=$(GAWK_LEGACY_DEVICE)\
		$(BUILD_DIR)/FboGroupForm.vfr
	@cat $(BUILD_DIR)/AutoGenFboData.sd > $(BUILD_DIR)/FboGroupForm.vfr
#######################################################################

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
