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
# $Header: /AptioV/BIN/AMIDebugRx/DbgRxPortLib/DbgRxPortLib.mak 4     12/31/14 6:22p Sudhirv $
#
# $Revision: 4 $
#
# $Date: 12/31/14 6:22p $
#**********************************************************************
# Revision History
# ----------------
# $Log: /AptioV/BIN/AMIDebugRx/DbgRxPortLib/DbgRxPortLib.mak $
# 
# 4     12/31/14 6:22p Sudhirv
# Updated for AMI Debug for UEFI 3.01.0016 Label
# 
# 
# 
#**********************************************************************


#<AMI_FHDR_START>
#-----------------------------------------------------------------------
#
# Name:			DbgRxPortLib.mak
#
# Description:	
#				
#
#-----------------------------------------------------------------------
#<AMI_FHDR_END>
#**********************************************************************
# Make sure that Debugger and USBRedirection modules are not enabled togather
ifeq ("$(AMI_DEBUGGER_SUPPORT)", "1")
  ifeq ("$(USB_REDIRECTION_SUPPORT)", "1")
    $(error  AMI_DEBUGGER_SUPPORT requires the USB_REDIRECTION_SUPPORT SDL token to be 0)
  endif
endif

Prepare : $(BUILD_DIR)/PEIDbgRxPortELink.h

$(BUILD_DIR)/PEIDbgRxPortELink.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define PEI_DBG_INIT_LIST  $(PeiDebugInitialize)$(EOL)"\
> $(BUILD_DIR)/PEIDbgRxPortELink.h

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
