#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
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
# $Header: /Alaska/BIN/Board/Board.mak 15    2/04/11 8:05p Artems $
#
# $Revision: 15 $
#
# $Date: 2/04/11 8:05p $
#**********************************************************************

#<AMI_FHDR_START>
#-----------------------------------------------------------------------
# Name:			Board.MAK
#
# Description:	This make file builds board related DXE & PEI modules
#
#-----------------------------------------------------------------------
#<AMI_FHDR_END>

Prepare : $(BUILD_DIR)/OemPortList.c

$(BUILD_DIR)/OemPortList.c : $(BUILD_DIR)/Token.h $(BOARD_DIR)/Board.mak $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#include <Pei.h>$(EOL)\
typedef BOOLEAN (ONE_PARAM_FUNCTION)($(EOL)\
    IN EFI_PEI_SERVICES **PeiServices$(EOL)\
);$(EOL)\
$(EOL)\
extern ONE_PARAM_FUNCTION $(IsRecovery) EndOfOneParamList;$(EOL)\
$(EOL)\
ONE_PARAM_FUNCTION* IsRecoveryList[] = {$(IsRecovery) NULL};$(EOL)"\
>$(BUILD_DIR)/OemPortList.c
 
#---------------------------------------------------------------------------



#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
