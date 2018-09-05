
Prepare : $(BUILD_DIR)/SspFileList.txt $(BUILD_DIR)/CmosManagerHooks.h

$(BUILD_DIR)/SspFileList.txt : $(BUILD_DIR)/Token.h $(CMOS_MANAGER_DIR)/CmosManager.mak
	$(ECHO) \
		$(subst \,//,$(CMOS_MANAGER_DIR)//Template.ssp) \
		$(subst \,//,$(CMOS_MANAGER_DIR)//CmosBoard.ssp) \
		$(subst \,//,$(ADDON_SSP_FILES)) > $(BUILD_DIR)/SspFileListElinks.txt
	gawk -f $(CMOS_MANAGER_DIR)/CmosSspScript.txt $(BUILD_DIR)/SspFileListElinks.txt > $(BUILD_DIR)/SspFileList.txt
	gawk -v BINMODE=2 -v ORS="\r\n" "{ print }" $(BUILD_DIR)/Token.equ > $(BUILD_DIR)/token_win.equ
	@$(SSP) $(BUILD_DIR)\\ -s2 -efi $(BUILD_DIR)/SspFileList.txt


$(BUILD_DIR)/CmosManagerHooks.h : $(BUILD_DIR)/Token.h $(CMOS_MANAGER_DIR)/CmosManager.mak
	$(ECHO) \
"#define CMOS_BATTERY_TEST_MAPPING $(CMOS_BATTERY_TEST_MAPPING)$(EOL)\
#define CMOS_IS_FIRST_BOOT_MAPPING $(CMOS_IS_FIRST_BOOT_MAPPING)$(EOL)\
#define CMOS_IS_BSP_MAPPING $(CMOS_IS_BSP_MAPPING)$(EOL)\
#define CMOS_IS_COLD_BOOT_MAPPING $(CMOS_IS_COLD_BOOT_MAPPING)$(EOL)\
#define CMOS_IS_USABLE_MAPPING $(CMOS_IS_USABLE_MAPPING)$(EOL)\
#define CMOS_PORT_MAPPING $(CMOS_PORT_MAPPING)$(EOL)"\
>$(BUILD_DIR)/CmosManagerHooks.h

AMI_CSP_LIB_INCLUDE_FILES := \
 $(CMOS_MANAGER_DIR)/CmosBoard.h \
$(AMI_CSP_LIB_INCLUDE_FILES)

