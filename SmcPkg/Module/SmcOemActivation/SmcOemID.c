//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/09/2016
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//<AMI_FHDR_START>
//
// Name:  SmcOemID.c
//
// Description:
//  Get the raw data of SLP public and marker from flash part.
// 
//<AMI_FHDR_END>
//****************************************************************************
#include <Library/DebugLib.h>
#include <AmiLib.h>
#include "token.h"
#include "OemActivation/OA2/OA2.h"

UINT8	OemId[6] = "SUPERM";
UINT8	OemTblId[8] = "SUPERM";

EFI_STATUS
SmcOverrideAcpiTableId(
    IN OUT	ACPI_HDR	*AcpiHdr
)
{
    DEBUG((-1, "SmcOverrideAcpiTableId Entry.\n"));
    if(AcpiHdr->Signature == SLIC_SIG){
	DEBUG((-1, "AcpiHdr->Signature = SLIC_SIG.\n"));
	MemCpy(OemId, AcpiHdr->OemId, sizeof(OemId));
	MemCpy(OemTblId, AcpiHdr->OemTblId, sizeof(OemTblId));
    }
    if((AcpiHdr->Signature == RSDT_SIG) || (AcpiHdr->Signature == XSDT_SIG)){
	DEBUG((-1, "AcpiHdr->Signature = RSDT_SIG or XSDT_SIG.\n"));
	MemCpy(AcpiHdr->OemId, OemId, sizeof(OemId));
	MemCpy(AcpiHdr->OemTblId, OemTblId, sizeof(OemTblId));	
    }
    DEBUG((-1, "SmcOverrideAcpiTableId End.\n"));
    return EFI_SUCCESS;
}


