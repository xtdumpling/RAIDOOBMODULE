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
#<AMI_FHDR_START>
#
# Name:	DsmAsl.mak
#
# Description:	Build rules for creating PciSlot.asl file
#   that creates _DSM methods for the given Slot instance and name.
#
#<AMI_FHDR_END>
#**********************************************************************

Prepare : DsmAsl

DsmAsl: $(BUILD_DIR)\PciSlot.asl	

$(BUILD_DIR)\PciSlot.asl : $(BUILD_DIR)\DsmAsl.lst
$(BUILD_DIR)\DsmAsl.lst : $(BUILD_DIR)\token.mak $(DsmAsl_DIR)\DsmAsl.bat
	if exist $(BUILD_DIR)\PciSlot.asl $(RM) $(BUILD_DIR)\PciSlot.asl
	$(ECHO) $(subst PCI_SLOT_DSM,$(EOL),$(DsmAsl:PCI_SLOT_DSM=^)) > $@
	$(DsmAsl_DIR)\DsmAsl.bat $@
	
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