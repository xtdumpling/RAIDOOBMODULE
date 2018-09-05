//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
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
OemGetPattern.c

This module is allow the BIOS side can through AFU to get the specific data of Rom file. AFU will set 
OFBD_TC_GET_PAT_REQ in FunSts field and send OFBD data to BIOS to check the "get pattern" function is 
required or not.
 
If "get pattern" function is requested, then BIOS should provide the search target in PatternSignature 
field, and filled its data range in SignatureDataSize field. 

Afterward AFU will according to the PatternSignature to search the entire Rom file. 

If found, AFU will set OFBD_TC_GET_PAT_CHK in FunSts field and provide the target's memory buffer 
after signature of OFBD_EXT_TBL_END for BIOS can to check or comparison. 

If not found, AFU will set OFBD_TC_GET_PAT_NOT_FOUND in FunSts field for BIOS can to know.

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "OemGetPattern.h"
#include "../Ofbd.h"

//---------------------------------------------------------------------------

#if OEM_GET_PATTERN_SAMPLE_TEST
#define ROM_FFS_GUID { 0x0DCA793A, 0xEA96, 0x42d8, 0xBD, 0x7B, 0xDC, 0x7F, 0x68, 0x4E, 0x38, 0xC1 }
static 	EFI_GUID LayoutFfsGuid = ROM_FFS_GUID;
#endif	// if OEM_GET_PATTERN_SAMPLE_TEST

/**
    Ofbd (get specific pattern code handle)

	Request Afu tool to search specific Ffs data by Guid
	
    @param pOFBDHdr - Ofbd header.

    @return EFI_STATUS
    @retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDGetSpecificPatternHandle( 
    IN OUT OFBD_HDR *pOFBDHdr)
{
	EFI_STATUS Status = EFI_SUCCESS;

#if OEM_GET_PATTERN_SAMPLE_TEST
	char  *DisOemStr = "[DBG] Get Search Target from BIOS !!";        
	UINT8 *pOFBDTblEnd;
	OFBD_TC_5C_GET_PATTERN_STRUCT *GetPatternStructPtr;
	UINT8 dbSearchSize = 0;

	pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
	GetPatternStructPtr = (OFBD_TC_5C_GET_PATTERN_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
	GetPatternStructPtr->RetSts =  OFBD_RS_GET_PAT_OK;
	GetPatternStructPtr->FunSts =  OFBD_TC_GET_PAT_LENGTH_CHECK;
	
	//
	// OEM add start here +>>>
	//
	
	//
	// Example: Request AFU to search Rom Layout Ffs data by Guid
	//
	
	// Step 01: Filled the Rom Layout Ffs Guid in PatternSignature filed.
	MemCpy(GetPatternStructPtr->PatternSignature, (UINT8*)&LayoutFfsGuid, sizeof(EFI_GUID));
	// Step 02: Tell the Afu the length of signature data
	dbSearchSize = sizeof(EFI_GUID);
	// Step 03: Tell the Afu need to extract how many size data
	GetPatternStructPtr->SignatureDataSize = 0x800;
	// Optional : Request AFU display the OemStr message
	pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
	Strcpy((char *)pOFBDTblEnd, DisOemStr);
    
	//
	// OEM add start here <<<+
	//
	
	GetPatternStructPtr->FunSts += dbSearchSize;	

#endif	// #if OEM_GET_PATTERN_SAMPLE_TEST

    return(Status);
}

/**
   	Ofbd (pattern check code handle)

	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully

*/ 

EFI_STATUS
OFBDPatternCheckHandle(
    IN OUT OFBD_HDR *pOFBDHdr)
{
	EFI_STATUS Status = EFI_SUCCESS;

#if OEM_GET_PATTERN_SAMPLE_TEST
	char  *DisOemStr1 = "[DBG] Test is ok!!";        
	char  *DisOemStr2 = "[DBG] Test is failure!!";        
	VOID  *pOFBDTblEnd;
	UINT8 *DataPtr;
	UINT16 DataSize;
	OFBD_TC_5C_GET_PATTERN_STRUCT *GetPatternStructPtr;  
   
	GetPatternStructPtr = (OFBD_TC_5C_GET_PATTERN_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
	pOFBDTblEnd = (VOID *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
	
	//
	// Description:
	// 
	// 		DataPtr is a buffer pointer, which is contained the data that you want Afu to search.
	//
	// 		If you want Afu to search the Rom Layout FFS data by Guid. After the search, you can 
	//
	//		compare the DataPtr to make sure the Afu got the correct data.
	//
	//
	// Note: Data size may be changed by the Afu, if the search data is in the end of Rom 
	//
	//		 file, and remaining size is less than the setting.
	
	// 1. Get Data Size
	DataSize = GetPatternStructPtr->SignatureDataSize;	
	// 2. Get Data Pointer
	DataPtr  = (UINT8 *)(*(UINT64 *)((UINT8 *)pOFBDTblEnd - sizeof(OFBD_END)));

	//
	// OEM add start here +>>>
	//
	if(!MemCmp(DataPtr, (UINT8*)&LayoutFfsGuid, sizeof(EFI_GUID)))
	{
		GetPatternStructPtr->RetSts = OFBD_RS_GET_PAT_OK;		// Normal status
		//pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
		//Strcpy((char *)pOFBDTblEnd, DisOemStr1);
	}
	else
	{
		GetPatternStructPtr->RetSts |= OFBD_RS_GET_PAT_ERROR;	// If error occurs, SET OFBD_RS_GET_PAT_ERROR flag
		//pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
		//Strcpy((char *)pOFBDTblEnd, DisOemStr2);
	}
	//
	// OEM add start here <<<+
	//
	
#endif	// #if OEM_GET_PATTERN_SAMPLE_TEST

    return(Status);
}

/**
   	Ofbd (if not found the specific pattern code handle)
	
	If Afu can not find the data what you want, that Afu will call this routine for BIOS to know.
	
	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDNotFoundPatternHandle(
    IN OUT OFBD_HDR *pOFBDHdr)
{
	EFI_STATUS Status = EFI_SUCCESS;

#if OEM_GET_PATTERN_SAMPLE_TEST
	OFBD_TC_5C_GET_PATTERN_STRUCT *GetPatternStructPtr;  
   
	GetPatternStructPtr = (OFBD_TC_5C_GET_PATTERN_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR));     

	//
	// OEM add start here +>>>
	//

	//
	// OEM add start here <<<+
	//
	
	GetPatternStructPtr->RetSts = OFBD_RS_GET_PAT_OK;		// Normal status
#endif	// #if OEM_GET_PATTERN_SAMPLE_TEST

    return(Status);
}

/**
   	This function is Ofbd Oem Get Pattern function entry point

	@param Buffer - Ofbd header.
  	@param pOFBDDataHandled - handle value returns
  	
	@retval	0xFF means Function executed successfully
	@retval	0xFE means Function error occured
*/ 

VOID OemGetPatternEntry(
    IN VOID             *Buffer,
    IN OUT UINT8        *pOFBDDataHandled)
{
    OFBD_HDR *pOFBDHdr;
    OFBD_EXT_HDR *pOFBDExtHdr; 
    OFBD_TC_5C_GET_PATTERN_STRUCT *GetPatternStructPtr;    

    if(*pOFBDDataHandled == 0)
    {
        pOFBDHdr = (OFBD_HDR *)Buffer;
        pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
        GetPatternStructPtr = (OFBD_TC_5C_GET_PATTERN_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR)); 
        
        if(pOFBDHdr->OFBD_FS & OFBD_FS_GET_PAT)
        {
            //Check Type Code ID
            if(pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_GET_PATTERN)
            {              	
                if(GetPatternStructPtr->FunSts == OFBD_TC_GET_PAT_REQ)
                {
                    if(OFBDGetSpecificPatternHandle(pOFBDHdr) == EFI_SUCCESS)
                    {
                        //OEM Get Pattern Data Handled.
                        *pOFBDDataHandled = 0xFF;      
                        return;
                    }
                }
                else if(GetPatternStructPtr->FunSts == OFBD_TC_GET_PAT_CHK)
                {
                    if(OFBDPatternCheckHandle(pOFBDHdr) == EFI_SUCCESS)
                    {
                        //OEM Get Pattern Data Handled.
                        *pOFBDDataHandled = 0xFF;      
                        return;
                    }
                }
                else if(GetPatternStructPtr->FunSts == OFBD_TC_GET_PAT_NOT_FOUND)
                {
                    if(OFBDNotFoundPatternHandle(pOFBDHdr) == EFI_SUCCESS)
                    {
                        //OEM Get Pattern Data Handled.
                        *pOFBDDataHandled = 0xFF;      
                        return;
                    }
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
