//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  History
//
//  Rev. 1.00
//    Bug Fix : Add searching config in order to search correctly.
//    Reason  : Function support after Afuefu 5.09
//    Auditor : Kasber Chen
//    Date    : Dec/22/2016
//
//****************************************************************************
#include <AmiDxeLib.h>
#include <AmiModulePkg\Ofbd\Ofbd.h>
#include <SspTokens.h>
#include "SmcGetPattern.h"
#include "SmcLib.h"

EFI_STATUS
OFBDGetSpecificPatternHandle( 
    IN OUT	OFBD_HDR	*pOFBDHdr
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT64	CPUID = 0;
    CHAR8	CPUID_Str[16];
    UINT8	*pOFBDTblEnd;
    OFBD_TC_5C_GET_PATTERN_STRUCT	*GetPatternStructPtr;
    UINT8       dbSearchSize = 0;
    
    TRACE((-1, "OFBDGetSpecificPatternHandle Start.\n"));
    pOFBDTblEnd = (UINT8*)((UINT8*)pOFBDHdr + (pOFBDHdr->OFBD_Size));
    GetPatternStructPtr = (OFBD_TC_5C_GET_PATTERN_STRUCT*)((UINT8*)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR)); 
    GetPatternStructPtr->RetSts = OFBD_RS_GET_PAT_OK;
    GetPatternStructPtr->FunSts = OFBD_TC_GET_PAT_LENGTH_CHECK;
    
    AsmCpuid(1, &((UINT32)CPUID), NULL, NULL, NULL);
    Sprintf(CPUID_Str, "CPUID = %lx", CPUID);

    if(GetCmosTokensValue(Q_SMC_Flash_Cancel) != 0x99)	//set 0x99 to skip CPUID check
	SetCmosTokensValue(Q_SMC_Flash_Cancel, 0x00);

    // Step 01: Filled the Rom Layout Ffs Guid in PatternSignature filed.
    MemCpy(GetPatternStructPtr->PatternSignature, (UINT8*)&CPUID, sizeof(CPUID));
    dbSearchSize = sizeof(CPUID);
    // Step 02: Tell the Afu need to extract how many size data
    GetPatternStructPtr->SignatureDataSize = 0x800;
    // Optional : Request AFU display the OemStr message
    pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
    Strcpy((CHAR8*)pOFBDTblEnd, CPUID_Str);
    GetPatternStructPtr->FunSts += dbSearchSize;
    TRACE((-1, "OFBDGetSpecificPatternHandle end.\n"));
    return(Status);
}

EFI_STATUS
OFBDNotFoundPatternHandle(
    IN OUT	OFBD_HDR	*pOFBDHdr
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    CHAR8	*DisOemStr1 = "The Flashed BIOS can't support your CPU !!";
    VOID	*pOFBDTblEnd;
    OFBD_TC_5C_GET_PATTERN_STRUCT	*GetPatternStructPtr;

    TRACE((-1, "OFBDNotFoundPatternHandle start.\n"));
    GetPatternStructPtr = (OFBD_TC_5C_GET_PATTERN_STRUCT*)((UINT8*)pOFBDHdr + pOFBDHdr->OFBD_HDR_SIZE + sizeof(OFBD_EXT_HDR));     
    pOFBDTblEnd = (VOID*)((UINT8*)pOFBDHdr + (pOFBDHdr->OFBD_Size));

    GetPatternStructPtr->RetSts = OFBD_RS_GET_PAT_OK;		// Normal status
    pOFBDHdr->OFBD_RS |= OFBD_RS_DIS_OEMSTR;
    Strcpy((CHAR8*)pOFBDTblEnd, DisOemStr1);

    if(GetCmosTokensValue(Q_SMC_Flash_Cancel) != 0x99)	//set 0x99 to skip CPUID check
	SetCmosTokensValue(Q_SMC_Flash_Cancel, 0xAA);

    TRACE((-1, "OFBDNotFoundPatternHandle end.\n"));
    return(Status);
}

VOID SmcGetPatternEntry(
    IN		VOID	*Buffer,
    IN OUT	UINT8	*pOFBDDataHandled
)
{
    OFBD_HDR	*pOFBDHdr;
    OFBD_EXT_HDR	*pOFBDExtHdr;
    OFBD_TC_5C_GET_PATTERN_STRUCT	*GetPatternStructPtr;

    if(*pOFBDDataHandled == 0){
        pOFBDHdr = (OFBD_HDR*)Buffer;
        pOFBDExtHdr = (OFBD_EXT_HDR*)((UINT8*)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
        GetPatternStructPtr = (OFBD_TC_5C_GET_PATTERN_STRUCT*)((UINT8*)pOFBDExtHdr + sizeof(OFBD_EXT_HDR)); 
        
        if(pOFBDHdr->OFBD_FS & OFBD_FS_GET_PAT){
            //Check Type Code ID
            if(pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_GET_PATTERN){              	
		if(GetPatternStructPtr->FunSts == OFBD_TC_GET_PAT_REQ){
		    if(OFBDGetSpecificPatternHandle(pOFBDHdr) == EFI_SUCCESS){
			//OEM Get Pattern Data Handled.
                    	*pOFBDDataHandled = 0xFF;      
                    	return;
		    }
		}
		else if(GetPatternStructPtr->FunSts == OFBD_TC_GET_PAT_NOT_FOUND){
		    if(OFBDNotFoundPatternHandle(pOFBDHdr) == EFI_SUCCESS){
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
