//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
OemOaHandle.c

This function is used for OEM can receive the OA data, and then decide what to do.

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "OemOaHandle.h"
#include "../Ofbd.h"

//#define CONVERT_TO_STRING(a) #a
#define STR(a) CONVERT_TO_STRING(a)

/**
   	Ofbd (Oem OA Data handle)
	
	This function is used for BIOS can to check or modify the OA 3 binary data, 
	before Afu to flash it.
	
	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDOemOADataHandle( 
    IN OUT OFBD_HDR *pOFBDHdr)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if OEM_OA_DATA_HANDLE_SAMPLE_TEST

    char *TestOemOAStr = " [DBG] OA function is called !!";
    
    UINT8 *pOFBDTblEnd;
    OFBD_TC_5A_OA_HANDLE_STRUCT *OAStructPtr;
    
    pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
    OAStructPtr = (OFBD_TC_5A_OA_HANDLE_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
    
    //
    // Please add your code here +>>>
    //
    if (OAStructPtr->dbOASts == OFBD_TC_OA_UPDATE_NOTIFY)
    {
        //
        //  Please set the dwRetSts for utility to know the process is success or fail
        //
            
        //
        //  Case 1: OA Data is invalid
        //
        
        //      OAStructPtr->dwRetSts = OFBD_RS_OA_DATA_IS_INVALID;
        

        //
        //  Case 2: OA Data has been modified, tell the utility use the new data to update
        //
        
        //      OAStructPtr->dwRetSts = OFBD_RS_OA_DATA_IS_MODIFIED;
        
        
        //
        //  Case 3: BIOS has updated the OA, so tell the utility doesn't to update
        //
        
        //      OAStructPtr->dwRetSts = OFBD_RS_OA_UPDATE_SKIPPED;
        
        
        //
        //  Case 4: BIOS doesn't allow the OA update, tell the utility stop the flash procedure
        //
        
        //      OAStructPtr->dwRetSts = OFBD_RS_OA_UPDATE_DECLINED;
                
        
        //
        //  Case X: OEM want to use their error code definition for /A update command status
        //
        
        
        //      Step 1: Set Bit 15 to 1
        //
        //      OAStructPtr->dwRetSts = OFBD_RS_OA_USE_OEM_ERROR_ID;
        //
        //      Step 2: Use dbErrorID field for utility return OEM specific error code
        //
        //      OAStructPtr->dbErrorID = 0~255;
        
        
        //   
        //  Tell the utility to show the OEM message, if needed. +>>>
        //
        pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
        Strcpy((char *)pOFBDTblEnd, TestOemOAStr);
        //
        // <<<+
        //
    
    }    
    
    //
    // Please add your code here <<<+
    //
    
#endif  // #ifdef OEM_OA_DATA_HANDLE_SAMPLE_TEST
    
    return(Status);
}

/**
   	This function is Ofbd Oem OA Data function entry point

	@param Buffer - Ofbd header.
  	@param pOFBDDataHandled - handle value returns
  	
	@retval	0xFF means Function executed successfully
	@retval	0xFE means Function error occured
*/ 

VOID OemOAHandleEntry (
    IN VOID             *Buffer,
    IN OUT UINT8        *pOFBDDataHandled )
{
	OFBD_HDR *pOFBDHdr;
	OFBD_EXT_HDR *pOFBDExtHdr; 
	VOID *pOFBDTblEnd;
	OFBD_TC_5A_OA_HANDLE_STRUCT *OAStructPtr;  

	if(*pOFBDDataHandled == 0)
	{
		pOFBDHdr = (OFBD_HDR *)Buffer;
		pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
		OAStructPtr = (OFBD_TC_5A_OA_HANDLE_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR)); 
		pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));    
		
		if(pOFBDHdr->OFBD_FS & OFBD_FS_OA)
		{   
			//Check Type Code ID
			if(pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_OA_HANDLE)
			{  
				if(OFBDOemOADataHandle(pOFBDHdr) == EFI_SUCCESS)
				{
					//Oem OA Data Handled.
					*pOFBDDataHandled = 0xFF;      
					return;
				}
			} 
			//Error occured
			*pOFBDDataHandled = 0xFE;          
			return;
		}  
	}

    return;
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
