
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

/** @file SmbiosElogFlashFuncs.c
    SmbiosElog Flash Functions Implementation

**/
//---------------------------------------------------------------------------

#include "SmbiosElog.h"
#include <Protocol/FlashProtocol.h>

//---------------------------------------------------------------------------

extern FLASH_PROTOCOL    *Flash;

/**

    Hook for doing an erase from the designated area in flash for the Smbios 
    Event Log.

    @param StartAddress - Starting address of the flash 
    @param EndAddress - Ending address of the flash
    @param SmbiosElogPrivateData - Gives access to Private Variables 

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/
EFI_STATUS
ErrorLoggingEraseCustomBlockRange (
  IN UINT8*                           StartAddress,
  IN UINT8*                           EndAddress,
  IN EFI_SMBIOS_ELOG_INSTANCE_DATA	  *SmbiosElogPrivateData )
{
  UINT8*    Address;
  UINTN     Count;
  UINTN     StartCount;
  UINTN     EndCount;

// Copy areas before and after area to be erased into buffer

  StartCount = Count = (UINTN)StartAddress % LOCAL_LOGICAL_BLOCK_SIZE;
  if (Count > 0) {
    Address = (UINT8*)(  ((UINTN)StartAddress / LOCAL_LOGICAL_BLOCK_SIZE) * LOCAL_LOGICAL_BLOCK_SIZE  );
    while (Count > 0) {
        SmbiosElogPrivateData->BufferForErasing[Count-1] = *(Address+Count-1);
        Count--;
     }
  }

  EndCount = Count = (LOCAL_LOGICAL_BLOCK_SIZE-1) - ( (UINTN)EndAddress % LOCAL_LOGICAL_BLOCK_SIZE );
  if (Count > 0) {
    Address = (UINT8*)( ((UINTN)EndAddress / LOCAL_LOGICAL_BLOCK_SIZE) * LOCAL_LOGICAL_BLOCK_SIZE );
    while (Count > 0) {
         SmbiosElogPrivateData->BufferForErasing[BUFFER_SIZE_NEEDED_FOR_ERASING-Count] = 
             *(Address+LOCAL_LOGICAL_BLOCK_SIZE-Count),
              Count--;
    }
  }

// Erase all blocks that are affected

  Count = ((UINTN)EndAddress / LOCAL_LOGICAL_BLOCK_SIZE) - ((UINTN)StartAddress / LOCAL_LOGICAL_BLOCK_SIZE) + 1;
  Address = (UINT8*)( ((UINTN)StartAddress / LOCAL_LOGICAL_BLOCK_SIZE) * LOCAL_LOGICAL_BLOCK_SIZE );
  while (Count > 0) {

// Erase Block

    Flash->Erase(Address,LOCAL_LOGICAL_BLOCK_SIZE);

// If First Block being Written and info needed to be written back, write
// back now (will happen for first loop only)

    if(  Count == (((UINTN)EndAddress / LOCAL_LOGICAL_BLOCK_SIZE) - ((UINTN)StartAddress / LOCAL_LOGICAL_BLOCK_SIZE) + 1) && StartCount != 0) {
       Flash->Write(Address,(UINT32)StartCount,SmbiosElogPrivateData->BufferForErasing);
    }

// If Last Block being Written and info needed to be written back, write 
// back now
/*
    if ( Count == 1 ) {
       FlashProgram( Address + LOCAL_LOGICAL_BLOCK_SIZE,
                        SmbiosElogPrivateData->BufferForErasing + BUFFER_SIZE_NEEDED_FOR_ERASING,
                      EndCount );
    }
*/
     if ( Count == 1 && EndCount != 0) {
       Flash->Write(Address + LOCAL_LOGICAL_BLOCK_SIZE - EndCount,
            (UINT32)EndCount,
            SmbiosElogPrivateData->BufferForErasing + BUFFER_SIZE_NEEDED_FOR_ERASING - EndCount
            );

    }

    Count--;
    Address += LOCAL_LOGICAL_BLOCK_SIZE;
  }
  return EFI_SUCCESS;
}

/**

    Function to read the area to place into the Memory Copy.

    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/
EFI_STATUS
ErrorLoggingReadFlashIntoMemoryCopy ( VOID )
{
  UINTN Count = 0;

  while (Count < LOCAL_EVENT_LOG_LENGTH) {
    mRedirSmbiosPrivate->CopyOfFlashArea[Count] = *( (mRedirSmbiosPrivate->StartAddress) + Count );
    Count++;
  }

  return EFI_SUCCESS;
}

/**

    Function checks to see if it is able to write the current section starting
    at StartOffset and ending at EndOffset, and if the section is not able to be
    written without erasing, the whole log is erased and fully rewritten with 
    the copy from memory.  If the section is able to be written without erasing,
    that section is written and we exit.

    @param SmbiosElogPrivateData - Pointer to local information so that local
                                   memory copy may also be altered.
    @param StartOffset - The starting offset in the memory copy of the first
                         byte to check
    @param EndOffset - The ending offset in the memory copy of the last byte
                       to check

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/
EFI_STATUS
RecordMemoryCopyDifferencesToFlash (
  IN EFI_SMBIOS_ELOG_INSTANCE_DATA    *SmbiosElogPrivateData,
  IN UINT32                            StartOffset,
  IN UINT32                            EndOffset )
{
  UINT32    CurrentOffsetInMemCopy;

  UINT8     FromCopy8;
  UINT8     FromFlash8;
  UINT32    FromCopy32;
  UINT32    FromFlash32;

  UINT8*    Address;


// Check if able to write without erasing


// Check enough bytes so that we can get to a 32-bit offset

  CurrentOffsetInMemCopy = StartOffset;
  while ( ((CurrentOffsetInMemCopy % sizeof(UINT32)) != 0)  &&  (CurrentOffsetInMemCopy <= EndOffset) ) {
        
    FromCopy8 = SmbiosElogPrivateData->CopyOfFlashArea[CurrentOffsetInMemCopy];
    FromFlash8 = *(SmbiosElogPrivateData->StartAddress + CurrentOffsetInMemCopy);
    if ( (FromCopy8 & FromFlash8) != FromCopy8 ) {

// Error Found, must erase all blocks that contain part of the flash table,
// and rewrite the information from the memory copy.

        goto EraseAllPoint;
     }
     CurrentOffsetInMemCopy++;
  }
  
// Check as many 32-bit chunks as possible

  while ( (CurrentOffsetInMemCopy + sizeof(UINT32)) <= EndOffset ) {
         
    FromCopy32  = *( (UINT32*)( &(SmbiosElogPrivateData->CopyOfFlashArea[CurrentOffsetInMemCopy]) ) );
    FromFlash32 = *( (UINT32*)(   SmbiosElogPrivateData->StartAddress + CurrentOffsetInMemCopy    ) );
    if ( (FromCopy32 & FromFlash32) != FromCopy32 ) {
	
    
// Error Found, must erase all blocks that contain part of the flash table,
// and rewrite the information from the memory copy.

       goto EraseAllPoint;
    }
     CurrentOffsetInMemCopy += sizeof(UINT32);
  }

// Check any remaining bytes

  while ( CurrentOffsetInMemCopy <= EndOffset ) {

    FromCopy8 = SmbiosElogPrivateData->CopyOfFlashArea[CurrentOffsetInMemCopy];
    FromFlash8 = *(SmbiosElogPrivateData->StartAddress + CurrentOffsetInMemCopy);
    if ( (FromCopy8 & FromFlash8) != FromCopy8 ) {
    
// Error Found, must erase all blocks that contain part of the flash table,
// and rewrite the information from the memory copy.
	
         goto EraseAllPoint;
    }
    CurrentOffsetInMemCopy++;
  }

// If reached, Write only the area designated by StartOffset/EndOffset, 
// with no erase needed.
// Enable all areas needed for writing event log on flash
// write data between two offsets
  
   CurrentOffsetInMemCopy = StartOffset;
  while (CurrentOffsetInMemCopy <= EndOffset) {
    if ( *(SmbiosElogPrivateData->StartAddress + CurrentOffsetInMemCopy)  != 
         (SmbiosElogPrivateData->CopyOfFlashArea[CurrentOffsetInMemCopy]) ) {

        Flash->Write( SmbiosElogPrivateData->StartAddress + CurrentOffsetInMemCopy,1,
             SmbiosElogPrivateData->CopyOfFlashArea + CurrentOffsetInMemCopy);
    }
    CurrentOffsetInMemCopy++;
  }

// Disable all areas needed for writing event log on flash
  return EFI_SUCCESS;



// If reached, Erase Needed, write all data in effected blocks

EraseAllPoint:

// Erase whole event log area, this also handles keeping intact information in
// effected blocks that are not part of the event log area.

  ErrorLoggingEraseCustomBlockRange( SmbiosElogPrivateData->StartAddress, 
                                     SmbiosElogPrivateData->EndAddress, 
                                     SmbiosElogPrivateData );

// Enable all areas needed for writing event log on flash

// Write all information from memory copy of event log back to Flash part.

  Address = SmbiosElogPrivateData->StartAddress;
  CurrentOffsetInMemCopy = 0;
  while (CurrentOffsetInMemCopy < LOCAL_EVENT_LOG_LENGTH) {
    if ( SmbiosElogPrivateData->CopyOfFlashArea[CurrentOffsetInMemCopy] != LOCAL_DEFAULT_BYTE_AFTER_ERASE ) {
        
         Flash->Write(Address + CurrentOffsetInMemCopy,
                 1,
                 SmbiosElogPrivateData->CopyOfFlashArea + CurrentOffsetInMemCopy);
        
     }
     CurrentOffsetInMemCopy++;
  }

  return EFI_SUCCESS;
}

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
