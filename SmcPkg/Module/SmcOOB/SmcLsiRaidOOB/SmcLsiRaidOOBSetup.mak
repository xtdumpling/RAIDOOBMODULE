Prepare : SMCLSIRAIDSETUPFILES

SMCLSIRAIDSETUPFILES : $(BUILD_DIR)/SMCLSIAutoId.h

$(BUILD_DIR)/SMCLSIAutoId.h : $(SmcLsiRaidOOBSetup_DIR)/SmcLsiRaidOOBSetupVfr.vfr $(BUILD_DIR)/Token.mak
	$(VFRID) /s3000 /o$(BUILD_DIR)/SMCLSIAutoId.h $(SmcLsiRaidOOBSetup_DIR)/SmcLsiRaidOOBSetupVfr.vfr 
	@$(ECHO) \
"#ifndef AUTO_ID$(EOL)\
#define AUTO_ID(x) x$(EOL)\
#endif$(EOL)" >> $(BUILD_DIR)/SMCLSIAutoId.h
