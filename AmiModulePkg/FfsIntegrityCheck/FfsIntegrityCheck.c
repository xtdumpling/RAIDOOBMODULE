//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
/** @file 
    This provides a check on FFS files and FVs

  This file contain the supporting code to implement the FV and FFS
  integrity check functionality.  This code Hooks into the 
  ProcessFvBeforePublishing eLink and is designed to be added to a project 
  and provide the functionality without having to be ported.

*/


#include <AmiPeiLib.h>
#include <FFS.h>
#include <RomLayout.h>
#include <Library/DebugLib.h>
#include "FfsIntegrityCheck.h"


//==============================================================================
// Function Definitions
/**
    Returns the file state set by the highest zero bit in the State field

    @param ErasePolarity   Erase Polarity  as defined by EFI_FVB2_ERASE_POLARITY
                         in the Attributes field.
    @param FfsHeader       Pointer to FFS File Header.

    @retval EFI_FFS_FILE_STATE File state is set by the highest none zero bit 
                             in the header State field.
*/
EFI_FFS_FILE_STATE
AmiGetFileState(
  IN UINT8                ErasePolarity,
  IN EFI_FFS_FILE_HEADER  *FfsHeader
  )
{
  EFI_FFS_FILE_STATE  FileState;
  EFI_FFS_FILE_STATE  HighestBit;

  FileState = FfsHeader->State;

  if (ErasePolarity != 0) {
    FileState = (EFI_FFS_FILE_STATE)~FileState;
  }
  
  // Get file state set by its highest none zero bit.
  HighestBit = 0x80;
  while (HighestBit != 0 && (HighestBit & FileState) == 0) {
    HighestBit >>= 1;
  }

  return HighestBit;
} 


/**
    Tries to determine if the FFS header is valid or not
  
    Currently this function just returns EFI_VOLUME_CORRUPTED.  The reason is that the 
    file states Header Under Construction and Header Invalid currently have no way to detemine 
    the state of the header what is in it and if anything else has been done, like writing the 
    FFS file.  Therefore, the system returns an error until a better solution is found

    @param  HeaderPointer  Pointer current header

    @retval EFI_SUCCESS  header determined to be valid
    @retval !=EFI_SUCCESS header not valid

*/
EFI_STATUS HeaderTest (UINT64 HeaderPointer)
{
    return EFI_VOLUME_CORRUPTED;
}


/**
    Finds the next file in the Firmware Volume

    This function uses the Firmware Volume header and the FFS file headers to parse through the 
    Firmware volume one file at a time   First call in should be with FileAddress = NULL, all subsequent
    calls should use the address that was passed out the previous time

    @param  ErasePolarity  Value to indicate what value, 0xff or 0x00, is the erased value of the flash part
    @param  FvStartingAddress   Pointer to the Starting address of the Firmware volume..
    @param  FileAddress Pointer to the next file in the Firmware volume 

    @retval EFI_SUCCESS  next file found and returned     
    @retval !=EFI_SUCCESS next file not found
*/
EFI_STATUS AmiFindNextFile(
        UINT8 ErasePolarity, 
        EFI_FIRMWARE_VOLUME_HEADER *FvStartingAddress, 
        EFI_FFS_FILE_HEADER **FileAddress)
{
    UINT64 Length = 0;
    UINT64 NewFileLocation = CONVERTTOUINT64 (*FileAddress);
    UINT8 FileState;
    BOOLEAN FileIsValid = 0;
    UINT64 FvEndAddress = CONVERTTOUINT64(FvStartingAddress) + FvStartingAddress->FvLength - 1;

    // Is this the first call for the Firmware Volume?
    // if so process this from the info in the FV header
    if (*FileAddress == 0)
    {
        // check for Extended headers
        if (FvStartingAddress->ExtHeaderOffset != 0)
        {
            UINT64 FvExtHeader;

            // The end of the Extended Headers is found in the Firmware Volume Extended Header
            //  So, the number of bytes to the beginning of the first FFS file is the offset to the extended header
            //  plus the size of the header
            Length = CONVERTTOUINT64(FvStartingAddress->ExtHeaderOffset);
             FvExtHeader =  (CONVERTTOUINT64( FvStartingAddress) + FvStartingAddress->ExtHeaderOffset);
             Length +=  ((EFI_FIRMWARE_VOLUME_EXT_HEADER *) FvExtHeader)->ExtHeaderSize;
        }
        else 
        {
            // This is the first call and the Extended Header Offset is zero.
            //  use the HeaderLength to find where the First FFS file begins
            Length = FvStartingAddress->HeaderLength;
        }
        // since this is the first time in for this firmware volume set the file pointer to point to the beginning of the 
        //  FV and use the Length variable to advance the pointer later on
        NewFileLocation = CONVERTTOUINT64 (FvStartingAddress);
    }
    else
    {
        // This is not the first call, check to see if we are using EFI_FFS_FILE_HEADER or EFI_FFS_FILE_HEADER2
        // and then get the correct length of the current file so the base address of the next file can be found
        if ( (( *FileAddress)->Attributes & FFS_ATTRIB_LARGE_FILE) 
                    && ((*FileAddress)->Size == 0) )
            Length = ((EFI_FFS_FILE_HEADER2 *)(*FileAddress))->ExtendedSize;
        else
            Length = FFS_FILE_SIZE((*FileAddress));
    }

    // add the length to the current NewFileLocation pointer which should already be pointing to the begining of 
    //  the previous file or the begining of the FV if FileAddress is NULL
    NewFileLocation = NewFileLocation + Length;

    // align the new file location.  The files are 8byte aligned
    NewFileLocation = GETOCCUPIEDSIZE(NewFileLocation, 8);

    // check for being beyond the end of the FV
    if (NewFileLocation > FvEndAddress)
        return EFI_NOT_FOUND;

    do {
        // Check for the File state
        FileState = AmiGetFileState (ErasePolarity, ((EFI_FFS_FILE_HEADER *)NewFileLocation));

        switch (FileState)
        {
            case EFI_FILE_HEADER_CONSTRUCTION:
            case EFI_FILE_HEADER_INVALID:
            {
                UINT64 HeaderPointer = NewFileLocation;
                EFI_STATUS Status;
                // This means that the header was in process or created, 
                // The data in the header cannot be relied upon.  Also, we do not know what type of header it is:
                //      EFI_FFS_FILE_HEADER2 or EFI_FFS_FILE_HEADER.  Start with EFI_FFS_FILE_HEADER
                //      and see if the header at the new location seems valid.  if not check move pointer forward
                //      another 4 bytes.  This is the difference between the two data structures
                
                // First Advance file pointer the sizeof(EFI_FFS_FILE_HEADER) and check the values
                Length = sizeof(EFI_FFS_FILE_HEADER);
                HeaderPointer += Length;

                // devise test for verifying header info
                Status = HeaderTest(HeaderPointer);
                if (EFI_ERROR(Status))
                {
                    // header determined to be invlaid, check if EFI_FFS_FILE_HEADER2 was the right size
                    HeaderPointer += sizeof(UINT32);
                    // run test again
                    Status = HeaderTest(HeaderPointer);
                    if (EFI_ERROR(Status))
                        return EFI_VOLUME_CORRUPTED;
                    
                    Length += 4;
                }
                break;
            }

            case EFI_FILE_DELETED:
            case EFI_FILE_HEADER_VALID:
            {
                // Deleted and Header valid mean that the header has valid information about the file, but the file is invlaid
                //      skip the file and try the next one
                //  
                if ( (((EFI_FFS_FILE_HEADER *)NewFileLocation)->Attributes & FFS_ATTRIB_LARGE_FILE) == FFS_ATTRIB_LARGE_FILE)
                {
                    Length = ((EFI_FFS_FILE_HEADER2 *)NewFileLocation)->ExtendedSize;
                }
                else
                {
                    Length = FFS_FILE_SIZE(((EFI_FFS_FILE_HEADER *)NewFileLocation));
                }
                break;
            }
            case EFI_FILE_DATA_VALID:
            case EFI_FILE_MARKED_FOR_UPDATE:
            {
                // Marked for update and File Data Valid mean that the header has valid information and that the file is valid 
                //      and should be returned for checking
                FileIsValid = 1;
                break;
            }

            default:
            {
                // FileState has an invalid state, exit and return an error
                return EFI_VOLUME_CORRUPTED;
                break;
            }
        } // end switch

        // only advance the NewFileLocation if we didnot find a valid file
        if ((FileState != EFI_FILE_DATA_VALID) && (FileState != EFI_FILE_MARKED_FOR_UPDATE))
        {
            // add the length to the current NewFileLocation pointer which should already be pointing to the begining of 
            //  the previous file or the begining of the FV if FileAddress is NULL
            NewFileLocation = NewFileLocation + Length;
    
            // align the new file location.  The files are 8byte aligned
            NewFileLocation = GETOCCUPIEDSIZE(NewFileLocation, 8);
        }
        
        // check for being beyond the end of the FV
        if (NewFileLocation > FvEndAddress)
            return EFI_NOT_FOUND;
        
    } while(!FileIsValid);

    *FileAddress = (EFI_FFS_FILE_HEADER *)NewFileLocation;
    return EFI_SUCCESS;
}


/**
    This function verifies the checksum of the firmware volume header

    @param  FvHeader pointer to the beginning of an FV header

    @retval TRUE Checksum verification passed
    @retval FALSE Checksum verification failed

*/
BOOLEAN VerifyFvHeaderChecksum (EFI_FIRMWARE_VOLUME_HEADER *FvHeader)
{
    UINT32  Index;
    UINT16  Checksum = 0;
    UINT16  *ptr = (UINT16*)FvHeader;

    for (Index = 0; Index < FvHeader->HeaderLength / sizeof(UINT16); Index++)
        Checksum = Checksum + ptr[Index];

    return (Checksum == 0);
}


/**
    This function verifies the checksum of the FFS File

    This function checks if it's a valid FFS file by checksumming the file
    and making sure it adds to 0.  The State byte in the Header is ignored 
    in this case

    @param   FfsHeader pointer to the header of an FFS File

    @retval TRUE  the checksum of the FFS file passed
    @retval FALSE the checksum of the FFS file failed
*/
BOOLEAN FileChecksum(EFI_FFS_FILE_HEADER* FfsHeader)
{
    UINT64  Index;
    UINT64  Length =(UINT64) (*(UINT32*)FfsHeader->Size & FFS_FILE_SIZE_MASK);
    UINT8   *ptr = (UINT8*)FfsHeader;
    UINT8   FileChecksum = 0;

    // check to see if EFI_FFS_FILE_HEADER2 is being used instead of EFI_FFS_FILE_HEADER
    // This is determined by checking the Attributes field for FFS_ATTRIB_LARGE_FILE bit and that 
    //  length = 0.  
    if ((FfsHeader->Attributes & FFS_ATTRIB_LARGE_FILE) && (Length == 0))
    {
        Length = ((EFI_FFS_FILE_HEADER2 *)FfsHeader)->ExtendedSize;
    }


    for (Index = 0; Index < Length; Index++)
        FileChecksum = FileChecksum + ptr[Index];
    FileChecksum = FileChecksum - FfsHeader->State;
    
    return (FileChecksum == 0);
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
