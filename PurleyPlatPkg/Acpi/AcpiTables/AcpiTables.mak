#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
# $Revision:  $
#
# $Date:  $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	AcpiTables.mak
#
# Description:
#
#<AMI_FHDR_END>
#**********************************************************************

#This should match with ASL_FLAGS in MAKEFILE of AcpiTables.inf in Build folder.
#Add default flags from AmiPkg\Configuration\CommonDef.txt file
#No defaults defined
ACPITABLE_ASL_FLAGS = 
#Flag -so: Generates offset.h file
#Flag -ve: Report only errors (ignore warnings and remarks)
ACPITABLE_ASL_FLAGS += -ve -so
#Add additional flags here. Ex: Overrides done in DSC file and etc.
ACPITABLE_ASL_FLAGS += -oi

#This should match with ASL_OUTFLAGS in MAKEFILE of AcpiTables.inf in Build folder
#Add default flags from AmiPkg\Configuration\CommonDef.txt file
ACPITABLE_ASL_OUTFLAGS = -p
#Add additional flags here. Ex: Overrides done in DSC file and etc.
#ACPITABLE_ASL_OUTFLAGS += 

#This should match with ASLPP in MAKEFILE of AcpiTables.inf in Build folder
ACPITABLE_ASLPP =  $(CCX64DIR)\cl.exe

#This should match ASLPP_FLAGS in MAKEFILE of AcpiTables.inf in Build folder
#Add default flags from AmiPkg\Configuration\CommonDef.txt file
ACPITABLE_ASLPP_FLAGS = /nologo /E /TC
#Include EXTERNAL CC Flags for pre-processing
ACPITABLE_ASLPP_FLAGS += $(EXTERNAL_CC_FLAGS)
#Add additional flags here. Ex: Overrides done in DSC file and etc.
#ACPITABLE_ASLPP_FLAGS += 

#Include files for Trim and Iasl tools.
TRIM_INC_LIST = $(WORKSPACE)\$(AcpiTables_DIR)\TrimIncludes.lst
include $(WORKSPACE)\$(AcpiTables_DIR)\IaslIncludes.lst

export BUILD_DIR AcpiTables_DIR  AmlOffsetHeaderFile_DEST_DIR

CreateAmlOffsetHeaderFile:
	@echo ***************** Generate AML offset header file Start *****************
# CpuPm.offset.h is not consumed in the source.
ifeq (0, 1)
	@echo ----- Generating SSDT CpuPm.offset.h -----
	Trim --asl-file -o $(WORKSPACE)\$(BUILD_DIR)\CpuPm.i -i $(TRIM_INC_LIST) $(WORKSPACE)\$(AcpiTables_DIR)\Ssdt\CpuPm.asl
	"$(ACPITABLE_ASLPP)" $(ACPITABLE_ASLPP_FLAGS) $(ACPITABLE_ASL_INC_LIST) $(BUILD_DIR)\CpuPm.i > $(BUILD_DIR)\CpuPm.iii
	Trim --source-code -l -o $(WORKSPACE)\$(BUILD_DIR)\CpuPm.iiii $(WORKSPACE)\$(BUILD_DIR)\CpuPm.iii
	"$(PLATFORM_ASL_COMPILER)" $(ACPITABLE_ASL_FLAGS) $(ACPITABLE_ASL_OUTFLAGS) $(WORKSPACE)\$(BUILD_DIR)\CpuPm.aml $(WORKSPACE)\$(BUILD_DIR)\CpuPm.iiii
	@call $(AcpiTables_DIR)\FilterOutAmlOffsetHeaderFile.bat SSDT CpuPm
	@echo ----- SSDT CpuPm.offset.h file is generated -----
endif
	
	@echo ----- Generating DSDT DsdtAsl.offset.h -----
# Using DsdtAsl.asl from ACPI module.
	Trim --asl-file -o $(WORKSPACE)\$(BUILD_DIR)\DsdtAsl.i -i $(TRIM_INC_LIST) $(WORKSPACE)\$(ACPI_BOARD_DIR)\DsdtAsl.asl
	"$(ACPITABLE_ASLPP)" $(ACPITABLE_ASLPP_FLAGS) $(ACPITABLE_ASL_INC_LIST) $(BUILD_DIR)\DsdtAsl.i > $(BUILD_DIR)\DsdtAsl.iii
	Trim --source-code -l -o $(WORKSPACE)\$(BUILD_DIR)\DsdtAsl.iiii $(WORKSPACE)\$(BUILD_DIR)\DsdtAsl.iii
ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
	"$(PLATFORM_ASL_COMPILER)" $(ACPITABLE_ASL_FLAGS) $(ACPITABLE_ASL_OUTFLAGS) $(WORKSPACE)\$(BUILD_DIR)\DsdtAsl.aml $(WORKSPACE)\$(BUILD_DIR)\DsdtAsl.iiii
else
	"$(PLATFORM_ASL_GCC_COMPILER)" $(ACPITABLE_ASL_FLAGS) $(ACPITABLE_ASL_OUTFLAGS) $(WORKSPACE)$(PATH_SLASH)$(BUILD_DIR)$(PATH_SLASH)DsdtAsl.aml $(WORKSPACE)$(PATH_SLASH)$(BUILD_DIR)$(PATH_SLASH)DsdtAsl.iiii
endif
	@call $(AcpiTables_DIR)\FilterOutAmlOffsetHeaderFile.bat DSDT DsdtAsl
	@echo ----- DSDT DsdtAsl.offset.h file is generated -----


	@echo ***************** Generate AML offset header file End *****************
	