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
    This provides a check on FFS files and FVs in the PEI

  This file contains the function that is eLinked into the PEI RomLayout eLink.
*/


#include <AmiPeiLib.h>
#include <FFS.h>
#include <Library/DebugLib.h>
#include "FfsIntegrityCheck.h"

//==============================================================================
// Function Definitions
/**
    Elinked function to verify the FV and the FFS files within it

    This function takes the FV passed into it and verifies the checksum 
    of the FV header and then verifies the checksum of each FFS file
    contained in the FV.

    @param   PeiServices   Pointer to the PeiServices Table
    @param   Area  Pointer to the a structure that describes the FV to be analyzed
    @param   FvType - Attributes of the FV described by the variable Area


    @retval TRUE the integrity check of the FV passed
    @retval FALSE  the integrity check of the FV failed
*/
BOOLEAN CheckFvAndFfsIntegrityPei(
    IN EFI_PEI_SERVICES **PeiServices, 
    IN AMI_ROM_AREA *Area,
    IN UINT32 FvType)
{
    EFI_STATUS Status;
    EFI_FFS_FILE_HEADER* pFile = NULL;
    EFI_FFS_FILE_HEADER* FfsFileHdr = NULL;
    EFI_FIRMWARE_VOLUME_HEADER *pFV;
    EFI_PHYSICAL_ADDRESS  FfsFileHeader;
    EFI_PHYSICAL_ADDRESS EndOfFv = Area->Address + Area->Size;
    EFI_BOOT_MODE  BootMode;
    UINT8  ErasePolarity;
    EFI_FFS_FILE_STATE  FileState;
    EFI_FFS_FILE_STATE  HighestBit;
    UINT32 FileLength;
    UINT32 FileOccupiedSize;
    UINTN  Index;
    UINTN  FvNum = 0;

    DEBUG((-1, "Enter CheckFvAndFfsIntegrityPei\n"));
    // error checking
    if ((Area == NULL) || (Area->Address == 0) || (Area->Address >= 0x100000000))
    {
        DEBUG((-1, "Invalid Value in ROM Area structure\n"));
        return FALSE;
    }
    // check for s3 resume state and skip this function if booting on S3 Resume 
    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    if ( (Status == EFI_SUCCESS) && (BootMode == BOOT_ON_S3_RESUME) )
    {
        DEBUG((-1, "S3 Resume path, exiting\n"));
        return TRUE;
    }

    pFV = (EFI_FIRMWARE_VOLUME_HEADER*)Area->Address;

    // check the Firmware volume's Header checksum, if not valid return false indicating
    //  the failed the integrity check
    if (!VerifyFvHeaderChecksum(pFV))
    {
        DEBUG((-1, "FV header checksum invalid, exiting\n"));
        return FALSE;
    }

    ErasePolarity = (pFV->Attributes & EFI_FVB2_ERASE_POLARITY) ? 0xFF : 0x00;

    // Now go through all FFS files and verify their checksums.  
    // Return FALSE if a file fails it's checksum
    while(TRUE)
    {
        
        // find the next file to check.  If there are no more files in the FV, then exit
        Status = AmiFindNextFile( ErasePolarity, pFV, &pFile);
        
        if (EFI_ERROR(Status))
            break;
        else
            FfsFileHdr = pFile; // store the pointer
        
        // check the attributes for the file, see if the FFS File has been checksummed
        // if yes, then start the step.  if no, go to the next file
        if ( !(pFile->Attributes & FFS_ATTRIB_CHECKSUM) )
            continue; // go to next file

        FileState = ErasePolarity ? ~(pFile->State) : pFile->State;
        //
        // Get file state set by its highest none zero bit.
        HighestBit = 0x80;
        while (HighestBit != 0 && (HighestBit & FileState) == 0) {
          HighestBit >>= 1;
        }
        FileState = HighestBit;
        
        // Check the file state to see if this a valid FFS file, and
        // check file checksum and return FALSE if invalid
        if ( (FileState == EFI_FILE_DATA_VALID) && (!FileChecksum(pFile)) )
        {
            DEBUG((-1, "FFS File state or checksum invalid, exiting\n"));
            return FALSE;
        }
    }
    
    if (Status == EFI_VOLUME_CORRUPTED)
    {
        // If FindNextFile returned EFI_VOLUME_CORRUPTED, then one of the following happened
        //  a) a corrupted header was found
        //  b) The pad file at the end of the FV was found

        // Calculate the beginning of the next file get the result as a UINT64 value
        // Length is 24 bits wide so mask upper 8 bits
        // FileLength is adjusted to FileOccupiedSize as it is 8 byte aligned.
        FileLength = (*(UINT32*)FfsFileHdr->Size) & FFS_FILE_SIZE_MASK;
        FileOccupiedSize = GETOCCUPIEDSIZE(FileLength, 8);
        FfsFileHeader = ((EFI_PHYSICAL_ADDRESS)(UINTN)FfsFileHdr) + FileOccupiedSize;

        // if the beginning of the next file is at or passed the end of the 
        //  FV then the checksum passed, otherwise a checksum failed.
        if (FfsFileHeader < EndOfFv)
        {
            // need to check the header for the next file in the list
            // loop through the space where the File header would be
            // if it is all 0xFF then we have hit the padfile
            // if not all 0xFFs then the area is corrupted
            for (Index = 0; Index < sizeof(EFI_FFS_FILE_HEADER); Index++)
                if ( ((UINT8*)FfsFileHeader)[Index] != ErasePolarity ) {
                    DEBUG((-1, "FFS Header is corrupted! Exiting \n\r"));
                    return FALSE;  
                }
        }
    }

    // if there are no more files in the FV, return TRUE
    DEBUG((-1, "All FV and FFS Files are cool.\n"));
    return TRUE;
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
