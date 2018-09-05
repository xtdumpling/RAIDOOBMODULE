#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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
#<AMI_FHDR_START>
#
# Name:		NB.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/NbInc.h

AMI_CSP_LIB_INCLUDE_FILES := $(BUILD_DIR)/NbInc.h \
$(AMI_CSP_LIB_INCLUDE_FILES)

$(BUILD_DIR)/NbInc.h : $(NB_BOARD_DIR)/NB.h $(NB_CHIPSET_DIR)/NBCspLib.h $(NB_BOARD_DIR)/NB.mak
	$(ECHO) \
"// Don't delete this line$(EOL)\
#include <$(NB_BOARD_DIR)/NB.h>$(EOL)\
#include <$(NB_CHIPSET_DIR)/NBCspLib.h>$(EOL)\
// Don't delete this line$(EOL)"\
> $@


#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
