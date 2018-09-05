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
# Name:	EnableLsiIoDecoding.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
CSMCOREBin : $(BUILD_DIR)\EnableLsiDecoding.lib

$(BUILD_DIR)\EnableLsiDecoding.lib : $(BUILD_DIR)\EnableLsiDecoding.mak EnableLsiDecodingBin

$(BUILD_DIR)\EnableLsiDecoding.mak : $(EnableLsiDecoding_DIR)\$(@B).cif $(EnableLsiDecoding_DIR)\$(@B).mak $(BUILD_RULES)
	$(CIF2MAK) $(EnableLsiDecoding_DIR)\$(@B).cif $(CIF2MAK_DEFAULTS)

EnableLsiDecodingBin : $(AMIDXELIB)
	$(MAKE) /$(MAKEFLAGS) $(BUILD_DEFAULTS)\
		/f $(BUILD_DIR)\EnableLsiDecoding.mak all\
		TYPE=LIBRARY 
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