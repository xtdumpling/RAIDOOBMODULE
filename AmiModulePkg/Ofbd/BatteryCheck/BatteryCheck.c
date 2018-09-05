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
BatteryCheck.c

This function is used for OEM can decided AFU flash behavior, when AC/Battery 
is under warning status and can output the OEM String message to the flash utility.

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "BatteryCheck.h"
#include "../Ofbd.h"

//#define CONVERT_TO_STRING(a) #a
#define STR(a) CONVERT_TO_STRING(a)


/**
   	Ofbd (Oem AC/Battery Check handle)
	
	This function is used for OEM can implement their AC/Battery check rule in 
	here and then request the Afu to stop the flash procedure or display the 
	error message when necessary.
	
	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDBatteryCheck( 
    IN OUT OFBD_HDR *pOFBDHdr)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if BATTERYCHECK_SAMPLE_TEST

	UINT8 *pOFBDTblEnd;
	OFBD_TC_5B_AC_BATTERY_STRUCT *AC_BatteryPtr;

	char *ErrorMsg_1 = "AC adapter is not connected.For an update, the AC adapter has to be connected.";
	char *ErrorMsg_2 = "There is no installed battery or the remaining charge level of the battery is lower than 60%%.Check your battery.";
	char *ErrorMsg_3 = "There is no installed battery or the remaining charge level of the battery is lower than 30%%.Check your battery.";
	char *ErrorMsg_4 = "No support this version of flash Common Interface.";

	pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
	AC_BatteryPtr = (OFBD_TC_5B_AC_BATTERY_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 

	switch(AC_BatteryPtr->dbGetSts)
	{
	//
    // Please add your code here +>>>
    //
		case OFBD_TC_AC_BAT_INFO_REQ:
			/*
			//+>>>if AC/Battery has an issue please add your message here
				Strcpy (pOFBDTblEnd , ErrorMsg_1);
            	pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
 				AC_BatteryPtr->dbRetSts |= OFBD_RS_AC_BAT_ERR ;
			//<<<
			*/
				AC_BatteryPtr->dbRetSts |= OFBD_RS_AC_BAT_OK;
		break;

		default:
			AC_BatteryPtr->dbRetSts |= OFBD_RS_AC_BAT_OK;
		break;
	//
    // Please add your code here <<<+
    //

	}

  
#endif  // #ifdef BATTERYCHECK_SAMPLE_TEST

    return(Status);
}

/**
   	This function is Ofbd Battery Check Header function entry point

	@param Buffer - Ofbd header.
  	@param pOFBDDataHandled - handle value returns
  	
	@retval	0xFF means Function executed successfully
	@retval	0xFE means Function error occured
*/ 

VOID BatteryCheckEntry (
    IN VOID             *Buffer,
    IN OUT UINT8        *pOFBDDataHandled )
{
	OFBD_HDR *pOFBDHdr;
	OFBD_EXT_HDR *pOFBDExtHdr; 
	VOID *pOFBDTblEnd;

	if(*pOFBDDataHandled == 0)
	{
		pOFBDHdr = (OFBD_HDR *)Buffer;
		pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
		pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));    
		
		if(pOFBDHdr->OFBD_FS & OFBD_FS_AC_BAT)
		{   
			//Check Type Code ID
			if(pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_AC_BAT)
			{  
				if(OFBDBatteryCheck(pOFBDHdr) == EFI_SUCCESS)
				{
					//Battery Check Handled.
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
