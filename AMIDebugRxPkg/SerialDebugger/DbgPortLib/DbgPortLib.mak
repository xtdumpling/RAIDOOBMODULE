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
# $Header: /AptioV/BIN/SerialDebugger/DbgPortLib/DbgPortLib.mak 2     12/31/14 6:38p Sudhirv $
#
# $Revision: 2 $
#
# $Date: 12/31/14 6:38p $
#**********************************************************************
# Revision History
# ----------------
# $Log: /AptioV/BIN/SerialDebugger/DbgPortLib/DbgPortLib.mak $
# 
# 2     12/31/14 6:38p Sudhirv
# Updated for AMI Debug for UEFI 3.01.0016 Label
# 
# 2     11/21/12 10:38a Sudhirv
# Changed Elink Name to avoid clash with SIO modules.
# 
# 1     11/19/12 7:42a Sudhirv
# [TAG]     EIP106836
# [Category] Function Request
# [Severity] Normal
# [Symptom] Make changes to AMIDebugRx\Debugger for chipset porting
# through ELink
# [Solution] Issue is fixed, Now chipset porting is done by ELink.
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
# Make sure that Serial Debugger and AMIDebugRx modules are enabled togather
ifeq ("$(SERIAL_DEBUGGER_SUPPORT)", "1")
  ifeq ("$(AMIDEBUG_RX_SUPPORT)", "1")
    $(error  SERIAL_DEBUGGER_SUPPORT requires the AMIDEBUG_RX_SUPPORT SDL token to be 0)
  endif
endif

# Make sure that Serial Debugger and Debugger modules are enabled togather
ifeq ("$(SERIAL_DEBUGGER_SUPPORT)", "1")
  ifeq ("$(AMI_DEBUGGER_SUPPORT)", "1")
    $(error  SERIAL_DEBUGGER_SUPPORT requires the AMI_DEBUGGER_SUPPORT SDL token to be 0)
  endif
endif

# Make sure that Serial Debugger and Usb Debug transport tokens are not enabled togather
ifeq ("$(SERIAL_DEBUGGER_SUPPORT)", "1")
  ifeq ("$(USB_DEBUG_TRANSPORT)", "1")
    $(error  SERIAL_DEBUGGER_SUPPORT requires the USB_DEBUG_TRANSPORT SDL token to be 0)
  endif
endif

Prepare : $(BUILD_DIR)/PEIDbgPortELink.h

$(BUILD_DIR)/PEIDbgPortELink.h :  $(BUILD_DIR)/token.mak
	$(ECHO) \
"#define PEI_DBG_INIT_LIST  $(PeiDebugInitialize)$(EOL)"\
> $(BUILD_DIR)/PEIDbgPortELink.h

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
