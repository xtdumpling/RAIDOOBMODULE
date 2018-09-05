#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
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
# $Header: /Alaska/Tools/template.mak 6     1/13/10 2:13p Felixp $
#
# $Revision: 6 $
#
# $Date: 1/13/10 2:13p $
#**********************************************************************
# Revision History
# ----------------
# $Log: /Alaska/Tools/template.mak $
# 
# 6     1/13/10 2:13p Felixp
# 
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	<ComponentName>.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
all : <ComponentName>

<ComponentName> : $(BUILD_DIR)\<ComponentName>.mak <ComponentName>Bin

$(BUILD_DIR)\<ComponentName>.mak : $(<ComponentName>_DIR)\$(@B).cif $(<ComponentName>_DIR)\$(@B).mak $(BUILD_RULES)
	$(CIF2MAK) $(<ComponentName>_DIR)\$(@B).cif $(CIF2MAK_DEFAULTS)

<ComponentName>Bin : $(AMIDXELIB)
	$(MAKE) /$(MAKEFLAGS) $(BUILD_DEFAULTS)\
		/f $(BUILD_DIR)\<ComponentName>.mak all\
		GUID=<Component_GUID>\
		ENTRY_POINT=<Component_EntryPoint>\
		TYPE=BS_DRIVER \
		COMPRESS=1\
#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************