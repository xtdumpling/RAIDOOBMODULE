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
# $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/PatsburgPCH/Chipset/AcpiModeEnable.mak 1     12/09/11 1:19p Archanaj $
#
# $Revision: 1 $
#
# $Date: 12/09/11 1:19p $
#**********************************************************************
# Revision History
# ----------------
# $Log: /Alaska/BIN/Chipset/Intel/SouthBridge/PatsburgPCH/Chipset/AcpiModeEnable.mak $
# 
# 1     12/09/11 1:19p Archanaj
# 
# 2     7/07/10 2:56p Bhimanadhunik
# Intial check-in for Patsburg.
# 
# 2     3/18/10 4:53a Tonywu
# Add "AcpiEnableCallbackList" and "AcpiDisableCallbackList" elink in the
# AcpiModeEnable driver.
# 
# 1     2/24/10 11:04p Tonywu
# Intel Cougar Point/SB eChipset initially releases.
# 
#**********************************************************************

#**********************************************************************
#<AMI_FHDR_START>
#
# Name:		AcpiModeEnable.mak
#
# Description:	Make file to create the driver that handles ACPI mode
#               enable/disable SW SMI
#
#<AMI_FHDR_END>
#**********************************************************************

Prepare : $(BUILD_DIR)/AcpiModeElinks.h

$(BUILD_DIR)/AcpiModeElinks.h :  $(BUILD_DIR)/token.mak
	$(ECHO) \
"// Don't delete this line$(EOL)\
#define ACPIMODE_ENABLE_LIST $(AcpiEnableCallbackList)$(EOL)\
#define ACPIMODE_DISABLE_LIST $(AcpiDisableCallbackList)$(EOL)\
// Don't delete this line$(EOL)"\
> $(BUILD_DIR)/AcpiModeElinks.h

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
