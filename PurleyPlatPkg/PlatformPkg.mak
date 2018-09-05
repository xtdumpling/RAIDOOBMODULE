#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
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
# $Revision:  $
#
# $Date:  $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:  PlatformPkg.mak
#
# Description:	Make file for Platform Package
#
#<AMI_FHDR_END>
#**********************************************************************

Prepare : $(BUILD_DIR)\ProgramOemSvidSid.h

$(BUILD_DIR)\ProgramOemSvidSid.h : $(BUILD_DIR)/token.mak $(PurleyPlatPkg_DIR)/PlatformPkg.mak
	@$(ECHO) \
" #define PROGRAM_IIO_OEM_SVID_SID_LIST $(PROGRAM_IIO_OEM_SVID_SID)$(EOL)\
$(EOL)\
#define PROGRAM_PCH_OEM_SVID_SID_LIST $(PROGRAM_PCH_OEM_SVID_SID)$(EOL)"\
>$(BUILD_DIR)/ProgramOemSvidSid.h

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
