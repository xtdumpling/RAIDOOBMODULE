//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
  Internal header file for the Flash Driver. Used by both the Runtime and SMM drivers.
**/

#ifndef __FLASH_COMMON__H__
#define __FLASH_COMMON__H__

#include <Protocol/AmiFlash.h>
#include <Library/AmiCriticalSectionLib.h>
#include <Library/AmiRomLayoutLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

/// Private(internal) data structure associated with the flash protocol instance.
typedef struct{
    /// Protocol instance
	AMI_FLASH_PROTOCOL Flash;
	/// Write enable counter (a number of times Flash->DeviceWriteEnable have been called). 
	/// The number represents a number of concurrently running transactions.
	UINT32 WriteEnableCounter;
} FLASH_PROTOCOL_PRIVATE;

extern FLASH_PROTOCOL_PRIVATE FlashData;

/// The data structure is used to store information about pending FV area transactions.
typedef struct{
    /// Area pointer
    AMI_ROM_AREA *RomArea;
    /// Size of the RomArea. When update transaction starts, the field is initialized with RomArea->Size.
    /// Later on the filed is updated with the size the area will have once update is completed.
    /// New size is extracted from the data passed to a Flash->Write or Flash->Update function.
    UINT32 AreaSize ; 
    /// A flag indicating if the FV signature have to be restored once FV update is completed.
    BOOLEAN RestoreSignature;
} UPDATED_AREA_DESCRIPTOR;

/// Array of descriptors of the areas being updated (list of flash update transactions in progress).
extern UPDATED_AREA_DESCRIPTOR UpdatedArea[];
/// Number of elements in the UpdatedArea array.
extern INT32 NumberOfUpdatedAreas;
/// Critical section object.
extern CRITICAL_SECTION Cs;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
