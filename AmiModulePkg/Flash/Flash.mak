Prepare : $(BUILD_DIR)/FlashElink.h

ifeq ($(FWHFlash_SUPPORT),1)
FWHFLASHINCLUDE = \#include <FwhFlashParts.h>$(EOL)
endif #ifeq ($(FWHFlash_SUPPORT),"1")

ifeq ($(LPCFlash_SUPPORT),1)
LPCFLASHINCLUDE = \#include <LpcFlashParts.h>$(EOL)
endif #ifeq ($(LPCFlash_SUPPORT),"1")

ifeq ($(STDFlash_SUPPORT),1)
STDFLASHINCLUDE = \#include <StdFlashParts.h>$(EOL)
endif #ifeq ($(STDFlash_SUPPORT),"1")

ifeq ($(SPIFlash_SUPPORT),1)
SPIFLASHINCLUDE = \#include <SpiFlashParts.h>$(EOL)
endif #ifeq ($(SPIFlash_SUPPORT),"1")



$(BUILD_DIR)/FlashElink.h : $(BUILD_DIR)/Token.mak
	@$(ECHO) \
"#define FLASH_LIST $(FlashList)$(EOL)\
#define OEM_FLASH_WRITE_ENABLE_LIST $(OemFlashWriteEnableList)$(EOL)\
#define OEM_FLASH_WRITE_DISABLE_LIST $(OemFlashWriteDisableList)$(EOL)\
#define OEM_FLASH_VALID_TABLE_LIST $(OemFlashValidTableList)$(EOL)\
$(EOL)\
$(FWHFLASHINCLUDE)\
$(LPCFLASHINCLUDE)\
$(STDFLASHINCLUDE)\
$(SPIFLASHINCLUDE)"\
> $(BUILD_DIR)/FlashElink.h 

