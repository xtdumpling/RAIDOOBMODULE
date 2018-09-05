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
OemPwdCk.c

Password Check protocol is for BIOS to indicate AFU that BIOS password exists and 
needs to initiate password checking before flashing BIOS. AFU will set OEM_PC_CK 
in dbGetCkSts field and send OFBD data to BIOS to report password checking is 
required or not.
 
If password check function is requested then BIOS should fill out password length 
in dwPwdLen field. Afterward AFU asks user to input password with specified password
length given from BIOS in field dwPwdLen and fill user input password in memory 
buffer after signature of OFBD_EXT_TBL_END for BIOS to do the comparison. 
*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "OemPwdCk.h"
#include "../Ofbd.h"

static int ErrorNum = 0;

/**
   	Ofbd (Oem Password Get handle)
	
	This function is used to tell the Afu, the BIOS password length.
	
	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDPwdGetHandle( 
    IN OUT OFBD_HDR *pOFBDHdr)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if OEM_PWDCK_SAMPLE_TEST
    char *DisOemStrPwd1 = "Password";
    char *DisOemStrPwd2 = "Password Retry count exceeded!";
    UINT8 *pOFBDTblEnd;
	OFBD_TC_52_PC_STRUCT *PwdCkStructPtr; 

    pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
	PwdCkStructPtr = (OFBD_TC_52_PC_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
	PwdCkStructPtr->dbRetSts =  OEM_RS_PC_REQ;
	PwdCkStructPtr->dwPwdLen = 7;
    if(ErrorNum < PASSWORD_RETRY_NUM)
    {
        //pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
        //Strcpy((char *)pOFBDTblEnd, DisOemStrPwd1);
    }
    else
    {
        PwdCkStructPtr->dbRetSts |= OEM_RS_PC_TIMES_EXCEEDED;
        //pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
        //Strcpy((char *)pOFBDTblEnd, DisOemStrPwd2);
    }
#endif

    return(Status);
}

/**
   	Ofbd (Oem Password Check handle)
	
	This function is used to check the user input password, which get from Afu.
	
	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDPwdCheckHandle(
    IN OUT OFBD_HDR *pOFBDHdr)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if OEM_PWDCK_SAMPLE_TEST
    char *DisOemStrPwdOK = "Password Comparison successes!";
    char *DisOemStrPwdErr = "Password Comparison error!";
    char *DisOemStrPwdErr2 = "Password Retry count exceeded!";
    char *Password = "0123456";
	UINT8 *pOFBDTblEnd;
	OFBD_TC_52_PC_STRUCT *PwdCkStructPtr;  
   
	PwdCkStructPtr = (OFBD_TC_52_PC_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
	pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
	if(!MemCmp(pOFBDTblEnd, Password, PwdCkStructPtr->dwPwdLen))
	{
		PwdCkStructPtr->dbRetSts |= OEM_RS_PC_CK_OK;
        //pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
		//Strcpy((char *)pOFBDTblEnd, DisOemStrPwdOK);
	}
	else
	{
        ErrorNum++;
		PwdCkStructPtr->dbRetSts |= 0;
        if(ErrorNum < PASSWORD_RETRY_NUM)
        {
            //pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
		    //Strcpy((char *)pOFBDTblEnd,  DisOemStrPwdErr);
        }
        else
        {
            PwdCkStructPtr->dbRetSts |= OEM_RS_PC_TIMES_EXCEEDED;
            //pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
		    //Strcpy((char *)pOFBDTblEnd, DisOemStrPwdErr2);
        }
	}

#endif

    return(Status);
}

/**
   	This function is Ofbd Oem Password Check function entry point

	@param Buffer - Ofbd header.
  	@param pOFBDDataHandled - handle value returns
  	
	@retval	0xFF means Function executed successfully
	@retval	0xFE means Function error occured
*/ 

VOID OEMPWDCKEntry (
    IN VOID             *Buffer,
    IN OUT UINT8        *pOFBDDataHandled )
{
    OFBD_HDR *pOFBDHdr;
    OFBD_EXT_HDR *pOFBDExtHdr; 
    VOID *pOFBDTblEnd;
    OFBD_TC_52_PC_STRUCT *PwdCkStructPtr;    

    if(*pOFBDDataHandled == 0)
    {
        pOFBDHdr = (OFBD_HDR *)Buffer;
        pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
        PwdCkStructPtr = (OFBD_TC_52_PC_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR)); 
        pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));    

        //TRACE((-1,"pOFBDHdr address is:%x ------\n",pOFBDHdr));
        //TRACE((-1,"pOFBDTblEnd address is:%x ------\n",*(UINT64 *)((UINT8 *)pOFBDTblEnd -sizeof(OFBD_END))));
        
        if(pOFBDHdr->OFBD_FS & OFBD_FS_PWD)
        {   
            //Check Type Code ID
            if(pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_PWD)
            {  
                //IoWrite32(0x300, *(UINT32 *)((UINT8 *)pOFBDTblEnd -4)); //debug
				if(PwdCkStructPtr->dbGetCkSts == OFBD_TC_OPC_GET)
				{
					if(OFBDPwdGetHandle(pOFBDHdr) == EFI_SUCCESS)
					{
						//OEM ROM ID Data Handled.
                    	*pOFBDDataHandled = 0xFF;      
                    	return;
                	}
				}
				else if(PwdCkStructPtr->dbGetCkSts == OFBD_TC_OPC_CHECK)
				{
					if(OFBDPwdCheckHandle(pOFBDHdr) == EFI_SUCCESS)
					{
						//OEM ROM ID Data Handled.
                    	*pOFBDDataHandled = 0xFF;      
                    	return;
                	}
				}
                /*//Not OEM ROM ID Data.
                *pOFBDDataHandled = 0;          
                return;*/
            } 
            //else
            //{
                 //Error occured
                *pOFBDDataHandled = 0xFE;          
                return;
            //}
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
