//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.10
//    Bug Fix:  Add detection rule for AOC-2UR6N4-IN001.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/21/2017
//
//  Rev. 1.09
//    Bug Fix:  Add riser card redriver function.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/10/2017
//
//  Rev. 1.08
//    Bug Fix:  Fix ultra IO index shift function fail.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Nov/24/2016
//
//  Rev. 1.07
//    Bug Fix:  Fix all OPROM item when no any riser card.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Nov/16/2016
//
//  Rev. 1.06
//    Bug Fix:  Create smbios type 9 after PCI bus protocol.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Nov/14/2016
//
//  Rev. 1.05
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//  Rev. 1.04
//      Bug Fixed:  Fix Bus dynamic change issue.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/06/2016
//
//  Rev. 1.03
//      Bug Fixed:  Fix AOC-UR-i2XT card show incorrectly.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jul/04/2016
//
//  Rev. 1.02
//      Bug Fixed:  Add riser card name to IIO and OPROM control items.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/21/2016
//
//  Rev. 1.01
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//    Rev. 1.00
//    Reason:	Initialize revision.
//    Auditor:	Kasber Chen
//    Date:	04/13/2016
//
//**************************************************************************//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MmPciBaseLib.h>
#include <PciBus.h>
#include "token.h"
#include "SmcLib.h"
#include "SmcCspLib.h"
#include "SmcRiserCardCommon.h"
#include "SmcRiserCard.h"

extern	RC_DATA_Table	RC_Table[];
extern	SMC_IIO_PORT_INDEX	IIO_Port_Index[];
UINT8	SXB1_index = 0, SXB2_index = 0, SXB3_index = 0;
UINT64  Intel_RC_IIO_HideItem;

SMC_SETUP_MODIFY_PROTOCOL       *SetupModifyProtocol;

EFI_GUID	SmcRiserCardProtocolGuid = EFI_SMC_RISERCARD_PROTOCOL_GUID;

VOID
SmcRiserCardSetupModify(void)
{
    EFI_STATUS Status = EFI_SUCCESS;

    DEBUG((-1, "SmcRiserCardSetupModify Entry.\n"));

    Status = gBS->LocateProtocol(
                    &gSetupModifyProtocolGuid,
                    NULL,
                    &SetupModifyProtocol);

    if(Status)  return;

    SmcRiserCardSetupItem();
    SmcRiserCardSetupString();
    SmcRiserCardIioSetup();

    DEBUG((-1, "SmcRiserCardSetupModify end.\n"));
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : RCIndexShift
//
// Description : It is a special routine, in some ultra IO riser card.
//               There are two models using the same card ID.
//               So checking ID and confirm onboard device (Lan),
//               If LAN chip exist, set BDF. If no, shift one ID.
//
// Parameters  : Board_Pcie_Table[]         MB riser card slot pcie port table.
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
RCIndexShift(
    IN	UINT32	Board_Pcie_Table[],
    IN OUT	UINT8	*Temp_index
)
{
    UINT16       BDF;

    DEBUG((-1, "RCIndexShift entry.\n"));

    switch(RC_Table[*Temp_index].ID_NAME.RC_ID){
    case 0x00000000:	//AOC-UR-i2XT(R1.01) or AOC-UR-i4XT(R1.01)
    case 0x00001000:	//AOC-UR-i2XT+ or AOC-UR-i4XT+
	BDF = SmcBDFTransfer(Board_Pcie_Table[6]);
	BDF = *(UINT8*)MmPciAddress(0, BDF >> 8, (BDF >> 3) & 0x1F, BDF & 0x07, 0xaa);
    	if(((UINT8)BDF != 0xff) && ((UINT8)BDF & 0x40))	// is AOC-UR-i2XT(R1.01) or AOC-UR-i2XT+
    	    (*Temp_index)++;
    	break;
    case 0x00000100:    //AOC-2UR68-i2XT or AOC-2UR6-i4XT.
    	BDF = SmcBDFTransfer(Board_Pcie_Table[6]);
    	BDF = *(UINT8*)MmPciAddress(0, BDF >> 8, (BDF >> 3) & 0x1F, BDF & 0x07, 0x19);
    	if(*(UINT32*)MmPciAddress(0, (UINT8)BDF, 0, 0, 0x2C) == 0x084715D9)
    	    (*Temp_index)++;	
        break;
    case 0x00001100:    //AOC-2UR68-i2XT+ or AOC-2UR6-i4XT+
	BDF = SmcBDFTransfer(Board_Pcie_Table[6]);
	BDF = *(UINT8*)MmPciAddress(0, BDF >> 8, (BDF >> 3) & 0x1F, BDF & 0x07, 0x19);
	if(*(UINT32*)MmPciAddress(0, (UINT8)BDF, 0, 0, 0x2C) == 0x091A15D9)
	    (*Temp_index)++;	
        break;
    case 0x00000400:
    case 0x00000c00:
	BDF = SmcBDFTransfer(Board_Pcie_Table[2]);
	BDF = *(UINT8*)MmPciAddress(0, BDF >> 8, (BDF >> 3) & 0x1F, BDF & 0x07, 0xaa);
    	if(((UINT8)BDF != 0xff) && ((UINT8)BDF & 0x40))	// is AOC-URN2-i2XT(R1.00/R1.01)?
    	    (*Temp_index)++;
    	break;
    case 0x00000500:
    case 0x00000D00:
	BDF = SmcBDFTransfer(Board_Pcie_Table[6]);
	BDF = *(UINT8*)MmPciAddress(0, BDF >> 8, (BDF >> 3) & 0x1F, BDF & 0x07, 0x19);
    	if(*(UINT32*)MmPciAddress(0, (UINT8)BDF, 0, 0, 0x2C) == 0x084715D9)
    	    (*Temp_index)++;	
    	break;
    case 0x00000a00:	//1U XL710 NIC riser
	BDF = SmcBDFTransfer(Board_Pcie_Table[0]);
	BDF = *(UINT8*)MmPciAddress(0, BDF >> 8, (BDF >> 3) & 0x1F, BDF & 0x07, 0x19);
    	if(*(UINT32*)MmPciAddress(0, (UINT8)BDF, 0, 0, 0x2C) != 0x091815D9){//non-NVMe model?
    	    PcdSet32(PcdSmcOBNVMe1BDF, 0);
    	    PcdSet32(PcdSmcOBNVMe2BDF, 0);
    	}
    	else
    	    (*Temp_index)++;					//Shift to NVMe model
    	break;
    case 0x00000B00:    //AOC-2UR6N4-i4XT or AOC-2UR6N4-IN001
        BDF = SmcBDFTransfer(Board_Pcie_Table[0]);
        BDF = *(UINT8*)MmPciAddress(0, BDF >> 8, (BDF >> 3) & 0x1F, BDF & 0x07, 0xaa);
        if(((UINT8)BDF == 0xff) || !BDF) // is AOC-2UR6N4-i4XT?
            (*Temp_index)++;
        break;
    }
    DEBUG((-1, "RCIndexShift end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcRiserCardSmBiosUpdate
//
// Description : Create riser card smbios type 9.
//
// Parameters  : None
//
// Returns     : None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
SmcRiserCardSmBiosUpdate(
    IN	EFI_EVENT	Event,
    IN	VOID		*Context
)
{
    UINT32      SXB1_Tbl[] = BOARD_PCIE_PORT_SXB1;
    UINT32      SXB2_Tbl[] = BOARD_PCIE_PORT_SXB2;
    UINT32      SXB3_Tbl[] = BOARD_PCIE_PORT_SXB3;

    DEBUG((-1, "SmcRiserCardSmBiosUpdate Entry.\n"));

    gBS->CloseEvent(Event);

    if(SXB1_index){
        RemapPcieTable(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
        RedefineDevBDF(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
    }
    if(SXB2_index){
        RemapPcieTable(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
        RedefineDevBDF(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
    }
    if(SXB3_index){
        RemapPcieTable(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
#if ULIO_SUPPORT
        RCIndexShift(SXB3_Tbl, &SXB3_index);
#endif
        RedefineDevBDF(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
    }

    if(SXB1_index || SXB2_index || SXB3_index)
        RiserCardSmBiosUpdate();

    SmcRiserCardSetupModify();

    DEBUG((-1, "SmcRiserCardSmBiosUpdate end.\n"));
}

SMCRISERCARD_PROTOCOL	SmcRiserCardProtocol = {
    SmcRiserCardBifuracate
};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcRiserCardInit
//
// Description : 
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
SmcRiserCardInit(
    IN	EFI_HANDLE		ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_EVENT	SmbiosEvent;
    VOID        *SmbiosReg;
    EFI_HANDLE  Handle = NULL;

    DEBUG((-1, "SmcRiserCardInit Entry.\n"));

    SXB1_index = PcdGet8(PcdSmcSXB1Index);
    SXB2_index = PcdGet8(PcdSmcSXB2Index);
    SXB3_index = PcdGet8(PcdSmcSXB3Index);

//even no any riser card or riser card can't be supported. We still must set pcie bifurcate
//because there are some onbard device but type 9 can be skipped.
    Status = gBS->InstallProtocolInterface(
    			&Handle,
    			&SmcRiserCardProtocolGuid,
    			EFI_NATIVE_INTERFACE,
    			&SmcRiserCardProtocol);
    DEBUG((-1, "InstallProtocolInterface SmcRiserCardProtocolGuid %r.\n", Status));

    Status = gBS->CreateEvent(
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SmcRiserCardSmBiosUpdate,
                  NULL,
                  &SmbiosEvent);

    if(!Status){
    	Status = gBS->RegisterProtocolNotify(
                  &gAmiExtPciBusProtocolGuid,
                  SmbiosEvent,
                  &SmbiosReg);
    }

    DEBUG((-1, "SmcRiserCardInit end.\n"));
    return	EFI_SUCCESS;
}
