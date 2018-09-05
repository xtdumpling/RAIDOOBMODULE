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
//  Rev. 1.03
//    Bug Fix:  Add WDT timer out value selection feature.
//    Reason:   
//    Auditor:  Jimmy Chiu (Refer Greenlow - Mark Chen)
//    Date:     May/05/2017
//
//  Rev. 1.02
//    Bug Fix:  Support SmcRomHole preserve in IPMI.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/12/2017
//
//  Rev. 1.01
//    Bug Fix : Clear OA2 when /OAD.
//    Reason  :
//    Auditor : Kasber Chen
//    Date    : Sep/01/2016
//
//  Rev. 1.00
//    Bug Fix : Renew.
//    Reason  : Move from Greenlow
//    Auditor : Kasber Chen
//    Date    : Aug/17/2016
//
//****************************************************************************

//<AMI_FHDR_START>
//
// Name:  SmcOFBD.c
//
// Description: Preserve and restore BIOS reserved 64kb data during AFU flash execution.
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Library/BaseMemoryLib.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiModulePkg/Ofbd/Ofbd.h>
#include <SspTokens.h>
#include <Setup.h>
#include "SmcLib.h"
#include "SmcRomHole.h"
#include "SmcOFBD.h"

EFI_GUID gSetupGuid = SETUP_GUID;

// Variable Declaration
SETUP_DATA gSetupData;
UINTN gSetupSize = sizeof(gSetupData);

//----------------------------------------------------------------------------
// Procedure:	PreserveSmcSMBIOSData
//
// Description:	Function to erase BIOS update command before write it.
//
// Input:
//      VOID
//
// Output:
//      VOID
//
//----------------------------------------------------------------------------
VOID
PreserveSmcRomHoleData(VOID)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	*TempBuffer;

    DEBUG((-1,"PreserveSmcSMBIOSData start.\n"));

    Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, SmcRomHoleSize(AFU_CMD_REGION), &TempBuffer);
    DEBUG((-1, "SmmAllocatePool = %r.\n", Status));
    SetMem(TempBuffer, SmcRomHoleSize(AFU_CMD_REGION), 0xff);
// Fill "Entry Flash."
    CopyMem(TempBuffer + 0x40, "Entry Flash", 11);
    Status = SmcRomHoleWriteRegion(AFU_CMD_REGION, TempBuffer);
    DEBUG((-1, "Fill Entry Flash Status = %r.\n", Status));
    pSmst->SmmFreePool(TempBuffer);

    SetCmosTokensValue(Q_SMC_FlashP_1, 0);
    SetCmosTokensValue(Q_SMC_FlashP_2, 0);
    SetCmosTokensValue(Q_SMC_FlashP_3, 0);
    SetCmosTokensValue(Q_SMC_FlashP_4, 0);
    SetCmosTokensValue(Q_SMC_FlashMEP, 0);
    DEBUG((-1,"PreserveSmcSMBIOSData end.\n"));
}

//----------------------------------------------------------------------------
// Procedure:	RestoreSmcSMBIOSData
//
// Description:	Function to restore BIOS update command data back if it been cleared by /k.
//              Also write UpdateComplete signature into BIOS reserved 64kb NCB offset 0x2040.
//              Note: In the special case we use same ROM hole address as OA3.0 key, so it will not been cleared by issue /k using AFUDOS.
//
// Input:
//      VOID
//
// Output:
//      VOID
//
//----------------------------------------------------------------------------
VOID
RestoreSmcRomHoleData(VOID)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	AFUCOMMAND[] = "PBNKEQrXSesphncROGkLlAD";
    UINT32	FlashP = 0;
    UINT8	FlashMEP = 0, i, *TempBuffer;

    DEBUG((-1, "RestoreSmcRomHoleData Start.\n"));
// Get flash parameters
    FlashP |= GetCmosTokensValue(Q_SMC_FlashP_1);
    FlashP |= (GetCmosTokensValue(Q_SMC_FlashP_2)) << 8;
    FlashP |= (GetCmosTokensValue(Q_SMC_FlashP_3)) << 16;
    FlashP |= (GetCmosTokensValue(Q_SMC_FlashP_4)) << 24;
    FlashMEP |= GetCmosTokensValue(Q_SMC_FlashMEP);
    DEBUG((-1,"FlashP = %x, FlashMEP = %x.\n", FlashP, FlashMEP));

    if(!FlashP && !FlashMEP){
        DEBUG((-1, "No any flash parameter.\n"));
        return;
    }
// Erase OA3 area
    if(FlashP & OFBD_TC_CFG_OAD){
	Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, SmcRomHoleSize(OA3_REGION), &TempBuffer);
	DEBUG((-1, "SmmAllocatePool = %r.\n", Status));
	SetMem(TempBuffer, SmcRomHoleSize(OA3_REGION), 0xff);
	Status = SmcRomHoleWriteRegion(OA3_REGION, TempBuffer);
	pSmst->SmmFreePool(TempBuffer);
	DEBUG((-1, "Erase OA3 area Status = %r.\n", Status));
	Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, SmcRomHoleSize(OA2_REGION), &TempBuffer);
	DEBUG((-1, "SmmAllocatePool = %r.\n", Status));
	SetMem(TempBuffer, SmcRomHoleSize(OA2_REGION), 0xff);
	Status = SmcRomHoleWriteRegion(OA2_REGION, TempBuffer);
	pSmst->SmmFreePool(TempBuffer);
	DEBUG((-1, "Erase OA2 area Status = %r.\n", Status));
    }

// Erase $SMC area
    if(!(FlashP & OFBD_TC_CFG_R) && !(FlashP & OFBD_TC_CFG_CLNEVNLOG)){
	Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, SmcRomHoleSize(SMBIOS_FOR_BMC_REGION), &TempBuffer);
	DEBUG((-1, "SmmAllocatePool = %r.\n", Status));
	SetMem(TempBuffer, SmcRomHoleSize(SMBIOS_FOR_BMC_REGION), 0xff);
	MemCpy(TempBuffer, "$SMC", 4);
	Status = SmcRomHoleWriteRegion(SMBIOS_FOR_BMC_REGION, TempBuffer);
	pSmst->SmmFreePool(TempBuffer);
	DEBUG((-1, "Erase $SMC area Status = %r.\n", Status));
    }

    if((FlashP & OFBD_TC_CFG_P)){
	Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, SmcRomHoleSize(INBAND_REGION), &TempBuffer);
	DEBUG((-1, "SmmAllocatePool = %r.\n", Status));
	SetMem(TempBuffer, SmcRomHoleSize(INBAND_REGION), 0xff);
	Status = SmcRomHoleWriteRegion(INBAND_REGION, TempBuffer);
	pSmst->SmmFreePool(TempBuffer);
    }    

    Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, SmcRomHoleSize(AFU_CMD_REGION), &TempBuffer);
    DEBUG((-1, "SmmAllocatePool = %r.\n", Status));
    SetMem(TempBuffer, SmcRomHoleSize(AFU_CMD_REGION), 0xff);
// BIOS update command, Fill flash parameters
    for(i = 0; i < sizeof(AFUCOMMAND); i++){
	if((FlashP & 0x01) == 0)
	    AFUCOMMAND[i] = 0x00;
	FlashP = FlashP >> 1;
    }
    CopyMem(TempBuffer + 0x20, AFUCOMMAND, sizeof(AFUCOMMAND));
// Fill "Write successful"
    CopyMem(TempBuffer + 0x40, "Write successful", 16);
// ME update command.
    CopyMem(TempBuffer + 0x50, &FlashMEP, 1);
    Status = SmcRomHoleWriteRegion(AFU_CMD_REGION, TempBuffer);
    pSmst->SmmFreePool(TempBuffer);
    DEBUG((-1, "Fill BIOS update command Status = %r.\n", Status));

    DEBUG((-1, "RestoreSmcRomHoleData End.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	SmcOfbdDisableWdtInSmm
//
// Description:	Disable Watch Dog function while flashing BIOS.
//
// Input:
//      VOID
//
// Output:
//      VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
SmcOfbdDisableWdt(VOID)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    Status = pRS->GetVariable(
	      L"Setup",
	      &gSetupGuid,
	      NULL,
	      &gSetupSize,
	      &gSetupData);

    if(gSetupData.SmcWatchDogFn)
	SMC_IPMICmd30_97(0, 0);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	SmcOfbdRestoreWdt
//
// Description:	Restore Watch Dog setting after flashing BIOS.
//
// Input:
//      VOID
//
// Output:
//      VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
SmcOfbdRestoreWdt(VOID)
{
    if(gSetupData.SmcWatchDogFn)
	SMC_IPMICmd30_97(1, gSetupData.SmcWatchDogTime);
}
//****************************************************************************
