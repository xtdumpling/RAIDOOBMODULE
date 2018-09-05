#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
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
# $Header: $
#
# $Revision: $
#
# $Date: $
#*************************************************************************
# Revision History
# ----------------
# $Log: $
# 
# 
#*************************************************************************
#<AMI_FHDR_START>
#
# Name:         CRB.mak
#
# Description:
#
#<AMI_FHDR_END>
#*************************************************************************
Prepare : $(BUILD_DIR)/AmiCrbInc.h $(BUILD_DIR)/CrbPeiInitElink.h $(BUILD_DIR)/CrbDxeInitElink.h
#----------------------------------------------------------------------------
#           Create AmiCrbInc header file
#----------------------------------------------------------------------------
CRB_FUNC_INCLUDE_FILES1=$(subst \,/,$(subst +,$(space),$(CRB_FUNC_INCLUDE_FILES)))

$(BUILD_DIR)/AmiCrbInc.h : $(CRB_FUNC_INCLUDE_FILES1)
	$(CAT) $(CRB_FUNC_INCLUDE_FILES1) > $(BUILD_DIR)/AmiCrbInc.h
#    copy /b $(CRB_FUNC_INCLUDE_FILES) $(BUILD_DIR)\AmiCrbInc.h

$(BUILD_DIR)/CrbPeiInitElink.h : $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define CRB_PEI_INIT_FUNC_LIST $(CRB_PEI_INIT_FUNC_LIST)$(EOL)"\
> $@

#    	$(SILENT)type << >$(BUILD_DIR)\CrbPeiInitElink.h
##define CRB_PEI_INIT_FUNC_LIST $(CRB_PEI_INIT_FUNC_LIST)
#<<

$(BUILD_DIR)/CrbDxeInitElink.h : $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define CRB_DXE_INIT_FUNC_LIST $(CRB_DXE_INIT_FUNC_LIST)$(EOL)"\
> $@

#    	$(SILENT)type << >$(BUILD_DIR)\CrbDxeInitElink.h
##define CRB_DXE_INIT_FUNC_LIST $(CRB_DXE_INIT_FUNC_LIST)
#<<

AMI_CSP_LIB_INCLUDE_FILES := $(CRB_DIR)/CRB.h  \
$(CRB_DIR)/CRBLib.h  \
$(AMI_CSP_LIB_INCLUDE_FILES)

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