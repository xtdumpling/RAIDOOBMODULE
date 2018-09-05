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

#
# @file  ViewSelLog.mak
#
#   Make file for ViewSelLog Modular part
#
Prepare : $(BUILD_DIR)/ViewSelLogOemDef.h 
$(BUILD_DIR)/ViewSelLogOemDef.h : $(BUILD_DIR)/Token.mak $(ViewSelLog_DIR)/ViewSelLog.mak
	$(ECHO) \
"$(if $(VIEW_SEL_LOG_OEM_FILE), $(foreach S_DEF, $(VIEW_SEL_LOG_OEM_FILE),#include <$(S_DEF)>$(EOL)))$(EOL)\
#define VIEW_SEL_LOG_OEM_SENSOR_NUMBER_LIST $(OEM_IPMI_SEL_SENSOR_NUMBER_LIST)$(EOL)$(EOL)\
#define VIEW_SEL_LOG_OEM_SENSOR_TYPE_LIST $(OEM_IPMI_SEL_SENSOR_TYPE_LIST)$(EOL)$(EOL)\
#define VIEW_SEL_LOG_OEM_SENSOR_TYPE_OFFSET_LIST $(OEM_IPMI_SEL_SENSOR_TYPE_OFFSET_LIST)$(EOL)$(EOL)"\
>$(BUILD_DIR)/ViewSelLogOemDef.h

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
