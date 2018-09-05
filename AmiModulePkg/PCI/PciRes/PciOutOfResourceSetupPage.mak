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

# Support added for to display PCI resource error in the setup.
# 
# 
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	PciOutOfResourceSetupPage.mak
#
# Description:	Make file to display PCI resource error in the setup.
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/PciOutOfResourceSetupPageDefinitions.h

$(BUILD_DIR)/PciOutOfResourceSetupPageDefinitions.h : $(PciOutOfResourceSetupPage_DIR)/PciOutOfResourceSetupPage.mak
	@$(ECHO)\
"$(if $(PciOutOfResourceSetupPage_DEFINITIONS) ,\
$(foreach S_DEF, $(PciOutOfResourceSetupPage_DEFINITIONS),#include<$(S_DEF)>$(EOL)))\
$(EOL)\
#define PciOutOfResourceSetupPage_FUNCTION_LIST $(PciOutOfResourceSetupPageFunctions)$(EOL)\
" > $(BUILD_DIR)/PciOutOfResourceSetupPageDefinitions.h

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