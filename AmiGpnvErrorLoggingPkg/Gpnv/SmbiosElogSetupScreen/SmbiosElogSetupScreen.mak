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


## @file SmbiosElogSetupScreen.mak
#    This is the make file for the SmbiosElogSetupScreen module this 
#    creates the files OemOperationTokenTable.h and OemSubClassTokenTable.h
##

ifeq ($(OemSmbiosErrorLogging), 1)

Prepare : $(BUILD_DIR)/OemOperationTokenTable.h $(BUILD_DIR)/OemSubClassTokenTable.h

$(BUILD_DIR)/OemOperationTokenTable.h : $(BUILD_DIR)/Token.mak $(SmbiosElogSetupScreen_DIR)/SmbiosElogSetupScreen.mak
	@$(ECHO) \
"#define OEM_Error_Op_Table $(OEM_ERROR_OPERATION_LOOKUP_TABLE) $(EOL)\
$(EOL)" > $(BUILD_DIR)/OemOperationTokenTable.h

$(BUILD_DIR)/OemSubClassTokenTable.h : $(BUILD_DIR)/Token.mak $(SmbiosElogSetupScreen_DIR)/SmbiosElogSetupScreen.mak
	@$(ECHO) \
"#define OEM_SubClass_Table $(OEM_SUBCLASS_LOOKUP_TABLE) $(EOL)\
$(EOL)" > $(BUILD_DIR)/OemSubClassTokenTable.h

endif	#ifeq ($(OemSmbiosErrorLogging), 1)


ifeq ($(CustomEventTypeAndStrings), 1)

Prepare : $(BUILD_DIR)/EventLogsCustomStringFunctions.h

$(BUILD_DIR)/EventLogsCustomStringFunctions.h : $(BUILD_DIR)/Token.mak $(SmbiosElogSetupScreen_DIR)/SmbiosElogSetupScreen.mak
	@$(ECHO) \
"#define EventLogCustomStringFunctions $(EVENT_LOGS_CUSTOM_STRING_FUNCTIONS) $(EOL)\
$(EOL)" > $(BUILD_DIR)/EventLogsCustomStringFunctions.h

endif	#ifeq ($(CustomEventTypeAndStrings), 1)

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