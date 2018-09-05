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
CleanErrorLog.c

This function is used to Clean Error Log. 

For example, AFU will call this function to clean error log when /CLNEVNLOG is issued.

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#if CLEAN_GPNV_ERROR_LOG_SAMPLE_TEST
#include <Protocol/SmiFlash.h>
#endif
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "CleanErrorLog.h"
#include "../Ofbd.h"

//#define CONVERT_TO_STRING(a) #a
#define STR(a) CONVERT_TO_STRING(a)

#if CLEAN_GPNV_ERROR_LOG_SAMPLE_TEST
EFI_GUID gEfiSmiFlashProtocolGuid = EFI_SMI_FLASH_GUID;
EFI_SMI_FLASH_PROTOCOL *mSmiFlash = NULL;
#endif

/**
    Clean Error Log Callback handle

	This callback function is used to locate SmiFlash protocol.
	
    @param Event - EFI Event.
    @param Context - Context.

*/

VOID CleanErrorLogCallback(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
#if CLEAN_GPNV_ERROR_LOG_SAMPLE_TEST
    EFI_GUID       gEfiSmiFlashProtocolGuid = EFI_SMI_FLASH_GUID;

    pBS->LocateProtocol (&gEfiSmiFlashProtocolGuid, NULL, &mSmiFlash);
#endif
}

/**
    Clean Error Log In Smm function handle

	This function is used to register CleanErrorLogCallback function.

*/

VOID CleanErrorLogInSmm(VOID)
{
#if CLEAN_GPNV_ERROR_LOG_SAMPLE_TEST
    EFI_STATUS     Status;
    EFI_GUID       gEfiSmiFlashProtocolGuid = EFI_SMI_FLASH_GUID;

    Status = pBS->LocateProtocol (&gEfiSmiFlashProtocolGuid, NULL, &mSmiFlash);
    if (EFI_ERROR(Status)){
        EFI_EVENT   SmiFlashCallbackEvt;
        VOID        *Reg;
        RegisterProtocolCallback(
             &gEfiSmiFlashProtocolGuid, CleanErrorLogCallback,
             NULL,&SmiFlashCallbackEvt, &Reg
        );
    }
#endif
}

/**
   	Ofbd (Clean Gpnv Error Log handle)
	
	This function is used to Clean Gpnv Error Logging data region.
	
	@param pOFBDHdr - Ofbd header.

  	@return EFI_STATUS
  	@retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDCleanErrorLogHandle( 
    IN OUT OFBD_HDR *pOFBDHdr)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if CLEAN_GPNV_ERROR_LOG_SAMPLE_TEST

	UINT8 *pOFBDTblEnd;
	OFBD_TC_58_CEL_STRUCT *CELStructPtr;
    #if defined(FV_ERROR_LOGGING_BASE) && defined(FV_ERROR_LOGGING_SIZE)
	FUNC_BLOCK FuncBlock;
	UINT8      pType1Header[16];
    #endif
	
	pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
	CELStructPtr = (OFBD_TC_58_CEL_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
	
    // Is need to Clean Gpnv Error Log
	if (CELStructPtr->dwFunSts == OFBD_TC_CEL_FUNC_REQ)
	{
		// Check Gpnv Error Logging module whether support or not
    #if !defined(FV_ERROR_LOGGING_BASE) || !defined(FV_ERROR_LOGGING_SIZE)
		CELStructPtr->dwRetSts = OFBD_TC_CEL_BIOS_NOT_SUPPORT;
    #else
		//
		// Reference from "ErrorLogging/SmiGpnv/SmiGpnv.c" file code +>>>
		//
		// SS: Alaska;$/Alaska/SOURCE/Modules/ErrorLogging;
		//
		// Label 4.6.3.6_GPNV_ERRLOG_2.1a
		//    
		// Enable Flash Write.    
        Status = mSmiFlash->EnableFlashWrite(&FuncBlock);
        // Read flash to preserve EFI_GPNV_ERROR_HEADER_TYPE1 header.
        FuncBlock.BufAddr = (UINTN)&pType1Header;
        FuncBlock.BlockSize = 0x10; // EFI_GPNV_ERROR_HEADER_TYPE1
        FuncBlock.BlockAddr = ((UINT32)FV_ERROR_LOGGING_BASE & \
                               (FLASH_SIZE - 1)); //0 starts at 0xfff0_0000
        FuncBlock.ErrorCode = 0;
        Status = mSmiFlash->ReadFlash(&FuncBlock);
        // Erase Error Logging GPNV.
        FuncBlock.BlockSize = FV_ERROR_LOGGING_SIZE; //max size 0xffff
        FuncBlock.BufAddr = 0;
        Status = mSmiFlash->EraseFlash(&FuncBlock); // Write Flash
        // Restore EFI_GPNV_ERROR_HEADER_TYPE1 header.
        FuncBlock.BlockSize = 0x10; //EFI_GPNV_ERROR_HEADER_TYPE1
        FuncBlock.BufAddr = (UINTN)&pType1Header;
        Status = mSmiFlash->WriteFlash(&FuncBlock); // Write Flash
        // Disable Flash Write.    
        Status = mSmiFlash->DisableFlashWrite(&FuncBlock);
        //
        // <<<+
        //
        if (EFI_ERROR (Status))
            CELStructPtr->dwRetSts = OFBD_TC_CEL_ERROR;
        else
            CELStructPtr->dwRetSts = OFBD_TC_CEL_OK;
    #endif
	}
		
#endif

    return(Status);
}

/**
   	This function is Ofbd Clean Gpnv Error Log function entry point

	@param Buffer - Ofbd header.
  	@param pOFBDDataHandled - handle value returns
  	
	@retval	0xFF means Function executed successfully
	@retval	0xFE means Function error occured
*/ 

VOID CleanErrorLogEntry (
    IN VOID             *Buffer,
    IN OUT UINT8        *pOFBDDataHandled )
{
	OFBD_HDR *pOFBDHdr;
	OFBD_EXT_HDR *pOFBDExtHdr; 
	VOID *pOFBDTblEnd;
	OFBD_TC_58_CEL_STRUCT *CELStructPtr;  

	if(*pOFBDDataHandled == 0)
	{
		pOFBDHdr = (OFBD_HDR *)Buffer;
		pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
		CELStructPtr = (OFBD_TC_58_CEL_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR)); 
		pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));    
		
		if(pOFBDHdr->OFBD_FS & OFBD_FS_ERR_LOG)
		{   
			//Check Type Code ID
			if(pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_CLN_ERR_LOG)
			{  
				if(OFBDCleanErrorLogHandle(pOFBDHdr) == EFI_SUCCESS)
				{
					//OEM Default Command Configuration Handled.
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
