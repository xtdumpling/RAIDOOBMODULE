#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
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
# $Header: $
#
# $Revision: $
#
# $Date: $
#**********************************************************************


#<AMI_FHDR_START>
#-----------------------------------------------------------------------
#
# Name:			StatusCodeAmi.mak
#
# Description:	
#				
#
#-----------------------------------------------------------------------
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/StatusCodeElinks.h

$(BUILD_DIR)/StatusCodeElinks.h :  $(TOKEN_MAK)
	$(ECHO) \
"#define PEI_INIT_LIST  $(PeiStatusCodeInitialize)$(EOL)\
#define DXE_INIT_LIST  $(DxeStatusCodeInitialize)$(EOL)\
#define PEI_STRING_LIST   $(PeiReportStringStatus)$(EOL)\
#define DXE_STRING_LIST   $(DxeReportStringStatus)$(EOL)\
#define RT_STRING_LIST   $(RtReportStringStatus)$(EOL)\
#define SMM_STRING_LIST   $(SmmReportStringStatus)$(EOL)\
#define PEI_SIMPLE_LIST   $(PeiReportSimpleStatus)$(EOL)\
#define DXE_SIMPLE_LIST   $(DxeReportSimpleStatus)$(EOL)\
#define RT_SIMPLE_LIST   $(RtReportSimpleStatus)$(EOL)\
#define SMM_SIMPLE_LIST   $(SmmReportSimpleStatus)$(EOL)\
#define PEI_MISC_LIST   $(PeiReportMiscStatus)$(EOL)\
#define DXE_MISC_LIST   $(DxeReportMiscStatus)$(EOL)\
#define RT_MISC_LIST   $(RtReportMiscStatus)$(EOL)\
#define SMM_MISC_LIST   $(SmmReportMiscStatus)$(EOL)\
#define PEI_CHECKPOINT_LIST   $(PeiReportCheckpoint)$(EOL)\
#define DXE_CHECKPOINT_LIST   $(DxeReportCheckpoint)$(EOL)\
#define RT_CHECKPOINT_LIST   $(RtReportCheckpoint)$(EOL)\
#define SMM_CHECKPOINT_LIST   $(SmmReportCheckpoint)$(EOL)\
#define PEI_ERROR_CODE_ACTIONS   $(PeiErrorCodeActions)$(EOL)\
#define RT_ERROR_CODE_ACTIONS   $(RtErrorCodeActions)$(EOL)\
#define SMM_ERROR_CODE_ACTIONS   $(SmmErrorCodeActions)$(EOL)\
#define DXE_ERROR_CODE_ACTIONS   $(DxeErrorCodeActions)$(EOL)" \
> $(BUILD_DIR)/StatusCodeElinks.h

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
