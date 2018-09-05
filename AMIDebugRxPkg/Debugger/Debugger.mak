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
# $Header: /AptioV/BIN/Debugger/Debugger.mak 2     12/31/14 6:26p Sudhirv $
#
# $Revision: 2 $
#
# $Date: 12/31/14 6:26p $
#**********************************************************************
# Revision History
# ----------------
# $Log: /AptioV/BIN/Debugger/Debugger.mak $
# 
# 2     12/31/14 6:26p Sudhirv
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
# Make sure that Debugger and AMIDebugRx modules are enabled togather
ifeq ("$(AMI_DEBUGGER_SUPPORT)", "1")
  ifneq ("$(AMIDEBUG_RX_SUPPORT)", "1")
    $(error  AMI_DEBUGGER_SUPPORT requires the AMIDEBUG_RX_SUPPORT SDL token to be 1)
  endif
endif

# Make sure that Debugger and USBRedirection modules are not enabled togather
ifeq ("$(AMI_DEBUGGER_SUPPORT)", "1")
  ifeq ("$(USB_REDIRECTION_SUPPORT)", "1")
    $(error  AMI_DEBUGGER_SUPPORT requires the USB_REDIRECTION_SUPPORT SDL token to be 0)
  endif
endif

# Make sure that Generic USB Debugger and PeiDebugger_Support SDL tokens are not enabled togather
ifeq ("$(GENERIC_USB_CABLE_SUPPORT)", "1")
  ifeq ("$(PeiDebugger_SUPPORT)", "1")
    $(error  GENERIC_USB_CABLE_SUPPORT requires the PeiDebugger_SUPPORT SDL token to be 0)
  endif
endif

# Make sure that Generic USB Debugger and USB_DEBUG_TRANSPORT SDL tokens are not enabled togather
ifeq ("$(GENERIC_USB_CABLE_SUPPORT)", "1")
  ifeq ("$(USB_DEBUG_TRANSPORT)", "1")
    $(error  GENERIC_USB_CABLE_SUPPORT requires the USB_DEBUG_TRANSPORT SDL token to be 0)
  endif
endif

# Make sure that Generic USB Debugger and USB_3_DEBUG_SUPPORT SDL tokens are not enabled togather
ifeq ("$(GENERIC_USB_CABLE_SUPPORT)", "1")
  ifeq ("$(USB_3_DEBUG_SUPPORT)", "1")
    $(error  GENERIC_USB_CABLE_SUPPORT requires the USB_3_DEBUG_SUPPORT SDL token to be 0)
  endif
endif

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
