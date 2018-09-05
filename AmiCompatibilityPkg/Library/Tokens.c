//**********************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
 
   This file contains constants and variables corresponding to some of the SDL tokens.
   The constants and variables are used by modules that are distributed in binary 
   and cannot directly consume token.h
*/

#include <Ffs.h>
#include <AmiLib.h>
#include <Token.h>

// Constants
#ifdef FLASH_ERASE_POLARITY
const UINT32 FlashBlockSize = FLASH_BLOCK_SIZE;
const UINTN FlashEmpty = (UINTN)(-FLASH_ERASE_POLARITY);
const UINT32 FlashEmptyNext = (FLASH_ERASE_POLARITY ? 0xffffff : 0);
#ifdef NVRAM_ADDRESS
const UINTN NvramAddress = NVRAM_ADDRESS;
#endif
#endif

#if FAULT_TOLERANT_NVRAM_UPDATE
const UINTN NvramBackupAddress = NVRAM_BACKUP_ADDRESS;
#else
const UINTN NvramBackupAddress = 0;
#endif

const BOOLEAN HideComPort = FALSE;

#ifdef FLASH_ERASE_POLARITY
#ifdef NVRAM_SIZE
const UINTN NvramSize = NVRAM_SIZE;
#endif
#ifdef NV_SIMULATION
const BOOLEAN NvSimulation = NV_SIMULATION;
#endif
#ifdef NVRAM_HEADER_SIZE
const UINT32 NvramHeaderLength = NVRAM_HEADER_SIZE;
#endif
#ifdef NVRAM_MONOTONIC_COUNTER_SUPPORT
const BOOLEAN NvramMonotonicCounterSupport = NVRAM_MONOTONIC_COUNTER_SUPPORT;
#endif
#ifdef NVRAM_RECORD_CHECKSUM_SUPPORT
const BOOLEAN NvramChecksumSupport = NVRAM_RECORD_CHECKSUM_SUPPORT;
#endif

// Variables
#define FLASH_DEVICE_BASE_ADDRESS (FLASH_BASE)
UINTN FlashDeviceBase = FLASH_DEVICE_BASE_ADDRESS;
UINTN FwhFeatureSpaceBase = FLASH_DEVICE_BASE_ADDRESS & ~(UINTN)0x400000;
#endif

#ifdef DEFAULT_LANGUAGE_CODE
const char *DefaultLanguageCode = CONVERT_TO_STRING(DEFAULT_LANGUAGE_CODE);
#endif


#if NVRAM_RT_GARBAGE_COLLECTION_SUPPORT
const BOOLEAN NvramRtGarbageCollectionSupport = NVRAM_RT_GARBAGE_COLLECTION_SUPPORT;
#else
const BOOLEAN NvramRtGarbageCollectionSupport = 0;
#endif

#if NO_MMIO_FLASH_ACCESS_DURING_UPDATE
const BOOLEAN FlashNotMemoryMapped = NO_MMIO_FLASH_ACCESS_DURING_UPDATE;
#else
const BOOLEAN FlashNotMemoryMapped = 0;
#endif

const UINT32 TraceLevelMask = 0xFFFFFFFF;
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
