//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file SmbiosElogFlashFuncs.h
    SmbiosElogFlashFuncs definitions

**/

#ifndef _EFI_SMBIOS_ELOG_FLASH_FUNCS_H_
#define _EFI_SMBIOS_ELOG_FLASH_FUNCS_H_
//---------------------------------------------------------------------------

#include "Token.h"

//---------------------------------------------------------------------------

#define LOCAL_EVENT_LOG_PHY_ADDR    FV_ERROR_LOGGING_BASE
#define LOCAL_EVENT_LOG_LENGTH      FV_ERROR_LOGGING_SIZE
#define LOCAL_LOGICAL_BLOCK_SIZE    FLASH_BLOCK_SIZE

// kdg fix (Add Flash polarity into all programming)

#define LOCAL_FLASH_ERASE_POLARITY    FLASH_ERASE_POLARITY
#define LOCAL_FLASH_RETRIES           FLASH_RETRIES

// Assign 8-bit number for initializing flash

#if LOCAL_FLASH_ERASE_POLARITY != 0
  #define LOCAL_DEFAULT_BYTE_AFTER_ERASE    0xFF
#else
  #define LOCAL_DEFAULT_BYTE_AFTER_ERASE    0x00
#endif


// If Event Log Area spans from one block to another, 2 blocks are needed for 
// a buffer

#if (LOCAL_EVENT_LOG_PHY_ADDR / LOCAL_LOGICAL_BLOCK_SIZE) != ((LOCAL_EVENT_LOG_PHY_ADDR+LOCAL_EVENT_LOG_LENGTH-1) / LOCAL_LOGICAL_BLOCK_SIZE)
  #define BUFFER_SIZE_NEEDED_FOR_ERASING    2 * FLASH_BLOCK_SIZE
#else
  #define BUFFER_SIZE_NEEDED_FOR_ERASING    FLASH_BLOCK_SIZE
#endif



#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
