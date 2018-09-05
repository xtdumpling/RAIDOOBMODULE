#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
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
# $Header: /Alaska/SOURCE/CPU/Intel/NehalemMicrocode/Microcode.mak 5     12/20/11 2:43p Markw $
#
# $Revision: 5 $
#
# $Date: 12/20/11 2:43p $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name: MicroCode.mak
#
# Description:	Build the microcode file.
#
#<AMI_FHDR_END>
#**********************************************************************

ifeq ($(USE_NEW_MPACK), 1)

ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
MPACK=$(MPACK_WIN)
else
MPACK=$(MPACK_LINUX)
endif

ifeq ($(MICROCODE_SPLIT_BB_UPDATE), 0)
Prepare : $(BUILD_DIR)/MICROCODE.BIN
else
Prepare : $(BUILD_DIR)/MICROCODE.BIN $(BUILD_DIR)/MICROCODE_BLANK.BIN
endif

ifeq ($(PACK_MICROCODE), 0)
MPACK_ALIGN_FLAGS=-a $(MICROCODE_ALIGNMENT)
else
MPACK_ALIGN_FLAGS=-f -a 1
endif

$(BUILD_DIR)/MICROCODE.BIN : $(BUILD_DIR)/Token.mak $(MICROCODE_DIR)/Microcode.mak
ifeq ($(MICROCODE_SPLIT_BB_UPDATE), 0)
	$(MPACK) $(MPACK_ALIGN_FLAGS) -r $(MICROCODE_PAD_SIZE) -o $@ $(MICROCODE_FILES)
else
	$(MPACK) $(MPACK_ALIGN_FLAGS) -b -o $@ $(MICROCODE_FILES)
endif

$(BUILD_DIR)/MICROCODE_BLANK.BIN : $(BUILD_DIR)/Token.mak $(MICROCODE_DIR)/Microcode.mak
	$(MPACK) -f -r $(MICROCODE_PAD_SIZE) -o $@

else #====================== OLD IMPLEMENTATION ========================

#List of microcode files separated by spaces.
MICROCODE_FILES_LIST = $(strip $(MICROCODE_FILES))

#List of merge64 formated microcode files separted by spaces.
MERGE_64_MCODE_STRING = $(foreach Microcode, $(MICROCODE_FILES_LIST), \
   file $(Microcode) binfile=$(Microcode) align=$(MICROCODE_ALIGNMENT) end)

#List of merge64 formated microcode files separted by lines..
MERGE_64_MCODE_LIST = $(patsubst end$(SPACE), end$(EOL),$(MERGE_64_MCODE_STRING))

#Merge64 microcode pad.
MCODE_PAD = blank MICROCODE_PAD$(EOL)\
    size=$(MICROCODE_PAD_SIZE)$(EOL)\
    pattern=(0xff)$(EOL)\
end

#Set variable if to include microcode pad when split microcode disabled.
ifeq ($(MICROCODE_SPLIT_BB_UPDATE), 0)
MCODE_PAD_ECHO = $(MCODE_PAD)
endif

#Determine MPDT for microcode files.
ifeq ($(MICROCODE_SPLIT_BB_UPDATE), 0)
MCODE_MPDT = file $(BUILD_DIR)/MPDTable.bin binfile=$(BUILD_DIR)/MPDTable.bin end
else
MCODE_MPDT = file $(BUILD_DIR)/MPDTableBB.bin binfile=$(BUILD_DIR)/MPDTableBB.bin end
endif

#/////////////////////////////////////////////////////////////////////////////////////////////////////////////


ifeq ($(MICROCODE_SPLIT_BB_UPDATE), 0)
Prepare : $(BUILD_DIR)/MPDTable.bin  $(BUILD_DIR)/MICROCODE.BIN
else
Prepare : $(BUILD_DIR)/MPDTableBB.bin $(BUILD_DIR)/MPDTable.bin $(BUILD_DIR)/MICROCODE.BIN $(BUILD_DIR)/MICROCODE_BLANK.BIN
endif

$(BUILD_DIR)/MPDTable.bin : $(MICROCODE_DIR)/MPDTable.asm $(MICROCODE_DIR)/Microcode.mak
	ml /nologo /c /Cx /W3 /WX /IBuild /Fo$(BUILD_DIR)/MPDTable.obj  $(MICROCODE_DIR)/MPDTable.asm
	$(CCX86DIR)/link /NOLOGO /OPT:REF /OUT:$(BUILD_DIR)/MPDTable.dll /NODEFAULTLIB /DLL /ALIGN:32 /IGNORE:4108 /SUBSYSTEM:NATIVE /NOENTRY /FIXED $(BUILD_DIR)/MPDTable.obj
	genfw -b -o $(BUILD_DIR)/MPDTable.i $(BUILD_DIR)/MPDTable.dll
	split -f $(BUILD_DIR)/MPDTable.i -s 16 -o $(BUILD_DIR)/MPDTable.bin -t $(BUILD_DIR)/MPDTable.ii

$(BUILD_DIR)/MPDTableBB.bin : $(MICROCODE_DIR)/MPDTableBB.asm $(MICROCODE_DIR)/Microcode.mak
	ml /nologo /c /Cx /W3 /WX /IBuild /Fo$(BUILD_DIR)/MPDTableBB.obj  $(MICROCODE_DIR)/MPDTableBB.asm
	$(CCX86DIR)/link /NOLOGO /OPT:REF /OUT:$(BUILD_DIR)/MPDTableBB.dll /NODEFAULTLIB /DLL /ALIGN:32 /IGNORE:4108 /SUBSYSTEM:NATIVE /NOENTRY /FIXED $(BUILD_DIR)/MPDTableBB.obj
	genfw -b -o $(BUILD_DIR)/MPDTableBB.i $(BUILD_DIR)/MPDTableBB.dll
	split -f $(BUILD_DIR)/MPDTableBB.i -s 16 -o $(BUILD_DIR)/MPDTableBB.bin -t $(BUILD_DIR)/MPDTableBB.ii

$(BUILD_DIR)/MICROCODE.BIN : $(BUILD_DIR)/Token.mak $(MICROCODE_DIR)/Microcode.mak
	$(ECHO)  \
"output$(EOL)\
    MICROCODE_FILES($(BUILD_DIR)/Microcode.bin)$(EOL)\
end$(EOL)\
group MICROCODE_FILES$(EOL)\
    upper=0xffffffff$(EOL)\
components$(EOL)\
$(MCODE_MPDT)$(EOL)\
$(MCODE_PAD_ECHO)$(EOL)\
$(MERGE_64_MCODE_LIST)$(EOL)\
end end"\
>$(BUILD_DIR)/Microcode.ini
	Merge64 /s $(BUILD_DIR)/Microcode.ini

$(BUILD_DIR)/MICROCODE_BLANK.BIN : $(BUILD_DIR)/Token.mak $(MICROCODE_DIR)/Microcode.mak
	$(ECHO)  \
"output$(EOL)\
    MICROCODE_FILES($(BUILD_DIR)/Microcode_Blank.bin)$(EOL)\
end$(EOL)\
group MICROCODE_FILES$(EOL)\
    upper=0xffffffff$(EOL)\
components$(EOL)\
    file $(BUILD_DIR)/MPDTable.bin binfile=$(BUILD_DIR)/MPDTable.bin end$(EOL)\
$(MCODE_PAD)$(EOL)\
end end"\
>$(BUILD_DIR)/MicrocodeBlank.ini    
	Merge64 /s $(BUILD_DIR)/MicrocodeBlank.ini

endif #====================== OLD IMPLEMENTATION ========================

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
