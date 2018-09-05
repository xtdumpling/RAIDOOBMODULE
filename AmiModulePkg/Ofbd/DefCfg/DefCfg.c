//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//
//  History
//
//  Rev. 1.02
//    Bug Fix : Add SmcOFBD module.
//    Reason  : 
//    Auditor : Kasber Chen
//    Date    : Aug/18/2016
//
//  Rev. 1.01
//    Bug Fix : Add a SMC feature - JPME2_Jumpless_SUPPORT
//    Reason  : It is one of SMC features.
//    Auditor : Hartmann Hsieh
//    Date    : Jun/08/2016
//
//  Rev. 1.00	
//    Bug Fix : Initial version. (Refer from Grantley)
//    Reason  : 
//    Auditor : Jacker Yeh
//    Date    : Mar/18/2016
//
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
DefCfg.c

OFBD Default Command Configuration provides BIOS an opportunity to override 
end user issued command in AFU. For example, BIOS could disable /k command 
even user issued it in AFU, or automatically enable /b when /p is issued.

Please reference OFBDDEFCFGHandle for more details.

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "DefCfg.h"
#include "../Ofbd.h"
#if SMC_OFBD_SUPPORT
#include <SspTokens.h>
#include <SmcLib.h>
#endif

#define STR(a) CONVERT_TO_STRING(a)

extern EFI_GUID gAmiSmbiosNvramGuid;

#if JPME2_Jumpless_SUPPORT
UINT8 Parameter = 0;
#endif

/**
    Ofbd (default command configuration handle)

	Default Configure protocol is for BIOS can to adjust Afu tool flash command configuration.
	
    @param pOFBDHdr - Ofbd header.

    @return EFI_STATUS
    @retval EFI_SUCCESS Function executed successfully
*/ 

EFI_STATUS
OFBDDEFCFGHandle( 
    IN OUT OFBD_HDR *pOFBDHdr)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if DEF_CFG_SAMPLE_TEST

	UINT8 *pOFBDTblEnd;
	OFBD_TC_51_DC_STRUCT *DCStructPtr;
	OFBD_TC_51_DC_EXT_STRUCT *DCExtStructPtr;
	UINT32 ExtAddr;
	
	pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size));
	DCStructPtr = (OFBD_TC_51_DC_STRUCT *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
	DCExtStructPtr = (OFBD_TC_51_DC_EXT_STRUCT *)((UINT8 *)pOFBDHdr + (pOFBDHdr->OFBD_Size) - sizeof(OFBD_END));
	ExtAddr = *(UINT32 *)((UINT8 *)pOFBDTblEnd - sizeof(OFBD_END));
	if (ExtAddr == OFBD_EXT_TBL_END)
	    DCExtStructPtr = NULL;
	
	#if (SMBIOS_DMIEDIT_DATA_LOC == 2)
		if (((DCStructPtr->ddRetSts & OFBD_TC_CFG_N) && (DCStructPtr->ddRetSts & OFBD_TC_CFG_R) && (DCStructPtr->ddRetSts & OFBD_TC_CFG_CAPSULE)) ||
		    ((DCStructPtr->ddRetSts & OFBD_TC_CFG_N) && (DCStructPtr->ddRetSts & OFBD_TC_CFG_R) && (DCStructPtr->ddRetSts & OFBD_TC_CFG_RECOVERY)))
		{
			UINT32	PreserveSmbiosNvram;
			// Set PreserveSmbiosNvram
			Status = pRS->SetVariable (
				L"PreserveSmbiosNvramVar",
				&gAmiSmbiosNvramGuid,
				EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
				 sizeof (UINT8),
				&PreserveSmbiosNvram
				);
			if (EFI_ERROR (Status)) {
				TRACE ((-1,  "Can't SetVariable of PreserveSmbiosNvramVar Status=%r\n",Status));
			} 
		}
	#endif
	//
	// OEM add
	//
#if SMCPKG_SUPPORT == 0 	
	// Sample : always cancel /K command
	if (DCStructPtr->ddRetSts & OFBD_TC_CFG_K)
	{
		DCStructPtr->ddExtCfg |= OFBD_TC_CFG_K;
	}
	
	// Sample : add new /B command when /P has issued
	if (DCStructPtr->ddRetSts & OFBD_TC_CFG_P)
	{
		DCStructPtr->ddRetSts |= OFBD_TC_CFG_B;
	}
	
	// Sample : For /Ln or /Kn command status setting
	if (DCExtStructPtr)
	{
		OFBD_TC_51_DC_EXT_STRUCT *NStructPtr = DCExtStructPtr;
		
		do
		{
			// Check current struct	is Ln command or not
			if (NStructPtr->ddIndexCfg == OFBD_TC_CFG_LN)
			{
				// Sample : add new /L1 command when /L0 command is issued
				if (NStructPtr->ddExtCfg & BIT00)
				{
					NStructPtr->ddExtCfg |=	BIT01;
				}
			}
			
			// Check current struct is Kn command or not
			if (NStructPtr->ddIndexCfg == OFBD_TC_CFG_KN)
			{
				// Sample : always cancel /K0 command
				if (NStructPtr->ddExtCfg & BIT00)
				{
					NStructPtr->ddIgnCfg |=	BIT00;
				}
			}
			// Ponit to next structure
			NStructPtr++;
			ExtAddr	= *(UINT32 *)NStructPtr;
		}while(ExtAddr && ExtAddr != 0xFFFFFFFF);
	}
#else
	// always cancel /X command
	if (DCStructPtr->ddRetSts & OFBD_TC_CFG_X)
	{
		DCStructPtr->ddExtCfg |= OFBD_TC_CFG_X;
	}		
	
	if (DCStructPtr->ddRetSts & OFBD_TC_CFG_P)
	{
		DCStructPtr->ddRetSts |= OFBD_TC_CFG_B;
		DCStructPtr->ddRetSts |= OFBD_TC_CFG_N;
		DCStructPtr->ddRetSts |= OFBD_TC_CFG_K;
#if JPME2_Jumpless_SUPPORT
		Parameter = 0x55;
#endif
	}

	if (DCStructPtr->ddRetSts & OFBD_TC_CFG_CLNEVNLOG)//if input command has /clnevnlog command, ignore other command.
	{
		DCStructPtr->ddExtCfg = DCStructPtr->ddRetSts & ~(OFBD_TC_CFG_CLNEVNLOG);
	}
#if SMC_OFBD_SUPPORT
    SetCmosTokensValue(Q_SMC_FlashP_1, (UINT8)(DCStructPtr->ddRetSts));
    SetCmosTokensValue(Q_SMC_FlashP_2, (UINT8)((DCStructPtr->ddRetSts) >> 8));
    SetCmosTokensValue(Q_SMC_FlashP_3, (UINT8)((DCStructPtr->ddRetSts) >> 16));
    SetCmosTokensValue(Q_SMC_FlashP_4, (UINT8)((DCStructPtr->ddRetSts) >> 24));
    if(GetCmosTokensValue(Q_SMC_Flash_Cancel) == 0xAA){
	DCStructPtr->ddRetSts |= OFBD_TC_CFG_R;
	DCStructPtr->ddRetSts |= OFBD_TC_CFG_LN;
	DCStructPtr->ddExtCfg = DCStructPtr->ddRetSts & ~(OFBD_TC_CFG_LN + OFBD_TC_CFG_R);
    }
#endif
#endif
#endif
    return(Status);
}

/**
   	This function is Ofbd Default Command Configuration function entry point

	@param Buffer - Ofbd header.
  	@param pOFBDDataHandled - handle value returns
  	
	@retval	0xFF means Function executed successfully
	@retval	0xFE means Function error occured
*/ 

VOID DEFCFGEntry (
    IN VOID             *Buffer,
    IN OUT UINT8        *pOFBDDataHandled )
{
	OFBD_HDR *pOFBDHdr;
	OFBD_EXT_HDR *pOFBDExtHdr; 
	VOID *pOFBDTblEnd;
	OFBD_TC_51_DC_STRUCT *DCStructPtr;  

	if(*pOFBDDataHandled == 0)
	{
		pOFBDHdr = (OFBD_HDR *)Buffer;
		pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
		DCStructPtr = (OFBD_TC_51_DC_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR)); 
		pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));    
		
		if(pOFBDHdr->OFBD_FS & OFBD_FS_CFG)
		{   
			//Check Type Code ID
			if(pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_AFUDEFCFG)
			{  
				if(OFBDDEFCFGHandle(pOFBDHdr) == EFI_SUCCESS)
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
