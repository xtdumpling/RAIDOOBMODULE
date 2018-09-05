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
## @file
# Generates time stamps.
#   Required parameters: TODAY - build date; NOW - build time
#    TODAY format: mm/dd/yyyy 
#    NOW format: hh:mm:ss
#**********************************************************************
include $(UTILITIES_MAK)

TIMESTAMPEQU="./$(BUILD_DIR)/TimeStamp.equ"
TIMESTAMPH="./$(BUILD_DIR)/TimeStamp.h"

TODAY_TWO_DIGIT_LIST := $(subst /, ,$(TODAY))
NOW_TWO_DIGIT_LIST := $(subst :, ,$(NOW))
TODAY_LIST := $(subst /, ,$(subst /0, ,/$(TODAY)))
NOW_LIST := $(subst :, ,$(subst :0, ,:$(NOW)))

FOUR_DIGIT_YEAR := $(word 3,$(TODAY_TWO_DIGIT_LIST))
TWO_DIGIT_MONTH := $(word 1,$(TODAY_TWO_DIGIT_LIST))
TWO_DIGIT_DAY := $(word 2,$(TODAY_TWO_DIGIT_LIST))
TWO_DIGIT_HOUR := $(word 1,$(NOW_TWO_DIGIT_LIST))
TWO_DIGIT_MINUTE := $(word 2,$(NOW_TWO_DIGIT_LIST))
TWO_DIGIT_SECOND := $(word 3,$(NOW_TWO_DIGIT_LIST))

MONTH := $(word 1,$(TODAY_LIST))
DAY := $(word 2,$(TODAY_LIST))
HOUR := $(word 1,$(NOW_LIST))
MINUTE := $(word 2,$(NOW_LIST))
SECOND := $(word 3,$(NOW_LIST))

all: timeequ timeh 

timeequ:
	@$(ECHO) "\
TODAY TEXTEQU <$(TODAY)> $(EOL)\
NOW TEXTEQU <$(NOW)> $(EOL)\
FOUR_DIGIT_YEAR TEXTEQU <$(FOUR_DIGIT_YEAR)> $(EOL)\
TWO_DIGIT_MONTH TEXTEQU <$(TWO_DIGIT_MONTH)> $(EOL)\
TWO_DIGIT_DAY TEXTEQU <$(TWO_DIGIT_DAY)> $(EOL)\
TWO_DIGIT_HOUR TEXTEQU <$(TWO_DIGIT_HOUR)> $(EOL)\
TWO_DIGIT_MINUTE TEXTEQU <$(TWO_DIGIT_MINUTE)> $(EOL)\
TWO_DIGIT_SECOND TEXTEQU <$(TWO_DIGIT_SECOND)> $(EOL)\
" > $(TIMESTAMPEQU)

timeh:
	@$(ECHO) "\
#ifndef __TIMESTAMP_H__ $(EOL)\
#define __TIMESTAMP_H__ $(EOL)\
#define TODAY \"$(TODAY)\" $(EOL)\
#define NOW \"$(NOW)\" $(EOL)\
#define FOUR_DIGIT_YEAR \"$(FOUR_DIGIT_YEAR)\" $(EOL)\
#define TWO_DIGIT_MONTH \"$(TWO_DIGIT_MONTH)\" $(EOL)\
#define TWO_DIGIT_DAY \"$(TWO_DIGIT_DAY)\" $(EOL)\
#define TWO_DIGIT_HOUR \"$(TWO_DIGIT_HOUR)\" $(EOL)\
#define TWO_DIGIT_MINUTE \"$(TWO_DIGIT_MINUTE)\" $(EOL)\
#define TWO_DIGIT_SECOND \"$(TWO_DIGIT_SECOND)\" $(EOL)\
#define L_TODAY L\"$(TODAY)\" $(EOL)\
#define L_NOW L\"$(NOW)\" $(EOL)\
#define L_FOUR_DIGIT_YEAR L\"$(FOUR_DIGIT_YEAR)\" $(EOL)\
#define L_TWO_DIGIT_MONTH L\"$(TWO_DIGIT_MONTH)\" $(EOL)\
#define L_TWO_DIGIT_DAY L\"$(TWO_DIGIT_DAY)\" $(EOL)\
#define L_TWO_DIGIT_HOUR L\"$(TWO_DIGIT_HOUR)\" $(EOL)\
#define L_TWO_DIGIT_MINUTE L\"$(TWO_DIGIT_MINUTE)\" $(EOL)\
#define L_TWO_DIGIT_SECOND L\"$(TWO_DIGIT_SECOND)\" $(EOL)\
#define THIS_YEAR $(FOUR_DIGIT_YEAR) $(EOL)\
#define THIS_MONTH $(MONTH) $(EOL)\
#define THIS_DAY $(DAY) $(EOL)\
#define THIS_HOUR $(HOUR) $(EOL)\
#define THIS_MINUTE $(MINUTE) $(EOL)\
#define THIS_SECOND $(SECOND) $(EOL)\
$(EOL)\
// Compatibility Definitions. $(EOL)\
// The definitions below are deprecated and will be removed in the future. $(EOL)\
#define FOUR_DIGIT_YEAR_INT THIS_YEAR $(EOL)\
#define TWO_DIGIT_MONTH_INT THIS_MONTH $(EOL)\
#define TWO_DIGIT_DAY_INT THIS_DAY $(EOL)\
#define TWO_DIGIT_HOUR_INT THIS_HOUR $(EOL)\
#define TWO_DIGIT_MINUTE_INT THIS_MINUTE $(EOL)\
#define TWO_DIGIT_SECOND_INT THIS_SECOND $(EOL)\
#endif $(EOL)\
" > $(TIMESTAMPH)

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