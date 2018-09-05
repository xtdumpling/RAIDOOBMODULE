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
OemRomId.c

Oem Rom ID Check protocol is for Flash utility to send customer defined 
specific ROM ID for BIOS to ID version checking uses. 

BIOS should provide inserted ROM file's specific ROM ID location and then 
check it, when ID is given by Flash Utility after OFBD_EXT_TBL_END.
Moreover, Flash utility will have corresponding action, either stop flashing BIOS.

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "OemRomId.h"
#include "../Ofbd.h"


/**
   	Ofbd (Oem Rom Id Get handle)
	
	This function is used to request Afu to get the fixed address data of Rom file for BIOS to know.
	
	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDORIGetHandle( 
    IN OUT OFBD_HDR *pOFBDHdr)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if OEM_ROM_ID_SAMPLE_TEST
	OFBD_TC_53_ORI_STRUCT *ORIStructPtr;   
	ORIStructPtr = (OFBD_TC_53_ORI_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
	ORIStructPtr->dwRetSts = OFBD_TC_ORI_REQ + OFBD_TC_ORI_ROM;
	ORIStructPtr->ddStartAddrLow = OEM_ROM_ID_Address;
	ORIStructPtr->ddSize = OEM_ROM_ID_SIZE;
#endif

    return(Status);
}

/**
   	Ofbd (Oem Rom Id Check handle)
		
	This function is used to check the data get from Afu.
	
	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDORICheckHandle(
    IN OUT OFBD_HDR *pOFBDHdr)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if OEM_ROM_ID_SAMPLE_TEST
	UINT8 *pOFBDTblEnd;
    char *DisOemStrOK = "Sample Message: OEM ROM ID Check OK";
    char *DisOemStrFail = "Sample Message: OEM ROM ID Check Fail";
    UINT32 OIDLengthMajMin = 0x0c; // hard coded 

	pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
	if(*(UINT32 *)(pOFBDTblEnd + OIDLengthMajMin) >= OEM_ROM_ID_SUB)
	{
		pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
		//pOFBDHdr->OFBD_RS |= 0;	//comparison passed and do nothing
		Strcpy((char *)pOFBDTblEnd, DisOemStrOK);
	}
	else
	{
		pOFBDHdr->OFBD_RS |= OFBD_RS_ERR_OUT + OFBD_RS_DIS_OEMSTR;
		//pOFBDHdr->OFBD_RS |= 0;	//comparison failed and do nothing
		Strcpy((char *)pOFBDTblEnd, DisOemStrFail);
	}
#endif

    return(Status);
}

/**
   	This function is Ofbd Oem Rom Id Check function entry point

	@param Buffer - Ofbd header.
  	@param pOFBDDataHandled - handle value returns
  	
	@retval	0xFF means Function executed successfully
	@retval	0xFE means Function error occured
*/ 

VOID OEMROMIDEntry (
    IN VOID             *Buffer,
    IN OUT UINT8        *pOFBDDataHandled )
{
    OFBD_HDR *pOFBDHdr;
    OFBD_EXT_HDR *pOFBDExtHdr; 
    VOID *pOFBDTblEnd;
    OFBD_TC_53_ORI_STRUCT *ORIStructPtr;    

    if(*pOFBDDataHandled == 0)
    {
        pOFBDHdr = (OFBD_HDR *)Buffer;
        pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
        ORIStructPtr = (OFBD_TC_53_ORI_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR)); 
        pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));    

        //TRACE((-1,"pOFBDHdr address is:%x ------\n",pOFBDHdr));
        //TRACE((-1,"pOFBDTblEnd address is:%x ------\n",*(UINT64 *)((UINT8 *)pOFBDTblEnd -sizeof(OFBD_END))));
        
        if(pOFBDHdr->OFBD_FS & OFBD_FS_ORI)
        {   
            //Check Type Code ID
            if(pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_ORI)
            {  
                //IoWrite32(0x300, *(UINT32 *)((UINT8 *)pOFBDTblEnd -4)); //debug
				if(ORIStructPtr->dwGetCkSts == OFBD_TC_ORI_GET)
				{
					if(OFBDORIGetHandle(pOFBDHdr) == EFI_SUCCESS)
					{
						//OEM ROM ID Data Handled.
                    	*pOFBDDataHandled = 0xFF;      
                    	return;
                	}
				}
				else if(ORIStructPtr->dwGetCkSts == OFBD_TC_ORI_CHECK)
				{
					if(OFBDORICheckHandle(pOFBDHdr) == EFI_SUCCESS)
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
