//****************************************************************************
//**            (C)Copyright 1993-2013 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//  Rev. 1.11
//    Bug Fix:  Support default string for SPG.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Apr/18/2017
//
//  Rev. 1.10
//    Reason:   Support riser card M.2 slot.
//    Auditro:  Kasber Chen
//    Date:     Mar/02/2017
//
//  Rev. 1.09
//    Reason:   Support OEM type 130 for SXB slots.
//    Auditro:  Kasber Chen
//    Date:     Jan/25/2017
//
//  Rev. 1.08
//    Bug Fix:  Fix NVMe port VMD enable item lose issue.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Nov/17/2016
//
//  Rev. 1.07
//    Bug Fix:  Dynamic hide unused IIO and VMD items.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Nov/11/2016
//
//  Rev. 1.06
//    Bug Fix:  Support 12 NVMe.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Sep/29/2016
//
//  Rev. 1.05
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//  Rev. 1.04
//    Bug Fix:  Fix Bus dynamic change issue.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Sep/06/2016
//
//  Rev. 1.03
//    Bug Fix:  Support 6 NVMe
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/14/2016
//
//  Rev. 1.02
//    Bug Fix:  Add riser card name to IIO and OPROM control items.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Jun/21/2016
//
//  Rev. 1.01
//    Bug Fix:  Add PCIE bifurcation setting in PEI.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Jun/06/2016
//
//  Rev. 1.00
//    Reason:   Initialize revision.
//    Auditro:  Kasber Chen
//    Date:     12/09/2013
//
//****************************************************************************
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <AmiLib.h>
#include "EFI.h"
#include <token.h>
#include "SmcRiserCardCommon.h"
#include "SmcSetupModify.h"
#include "SmcRiserCard.h"
#if DEBUG_MODE
#include <..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#else
#include <..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#endif
#include <Library/PrintLib.h>

extern	SMCRISERCARD_PROTOCOL	SmcRiserCardProtocol;
extern  SMC_SETUP_MODIFY_PROTOCOL       *SetupModifyProtocol;
extern	RC_DATA_Table	RC_Table[];
extern	SMC_IIO_PORT_INDEX	IIO_Port_Index[];
extern	UINT8	IIO_Port_Index_Size;
extern	UINT8	SXB1_index, SXB2_index, SXB3_index;
extern	UINTN	SlotPcdNumTbl[], LanPcdNumTbl[], NVMePcdNumTbl[];
extern	UINT8	SlotPcdNumTblSize, LanPcdNumTblSize, NVMePcdNumTblSize;
extern  UINT64  Intel_RC_IIO_HideItem;

UINT16	STR_RC_OPROM_Table[] = {
        STR_SMC_PCI_SLOT_1_OPROM, STR_SMC_PCI_SLOT_2_OPROM,
        STR_SMC_PCI_SLOT_3_OPROM, STR_SMC_PCI_SLOT_4_OPROM,
        STR_SMC_PCI_SLOT_5_OPROM, STR_SMC_PCI_SLOT_6_OPROM,
        STR_SMC_PCI_SLOT_7_OPROM, STR_SMC_PCI_SLOT_8_OPROM,
        STR_SMC_PCI_SLOT_9_OPROM, STR_SMC_PCI_SLOT_10_OPROM,
        STR_SMC_PCI_SLOT_11_OPROM};

UINT16	STR_RC_LAN_Table[] = {
        STR_SMC_ONBOARD_LAN_1_SELECT, STR_SMC_ONBOARD_LAN_2_SELECT,
        STR_SMC_ONBOARD_LAN_3_SELECT, STR_SMC_ONBOARD_LAN_4_SELECT};

UINT16	STR_RC_NVMe_Table[] = {
        STR_SMC_ONBOARD_NVME_1_SELECT, STR_SMC_ONBOARD_NVME_2_SELECT,
        STR_SMC_ONBOARD_NVME_3_SELECT, STR_SMC_ONBOARD_NVME_4_SELECT,
        STR_SMC_ONBOARD_NVME_5_SELECT, STR_SMC_ONBOARD_NVME_6_SELECT,
        STR_SMC_ONBOARD_NVME_7_SELECT, STR_SMC_ONBOARD_NVME_8_SELECT,
        STR_SMC_ONBOARD_NVME_9_SELECT, STR_SMC_ONBOARD_NVME_10_SELECT,
        STR_SMC_ONBOARD_NVME_11_SELECT, STR_SMC_ONBOARD_NVME_12_SELECT};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcRiserCardSetupItem
//
// Description : Add setup item into hidden list
//
// Parameters  :
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI
SmcRiserCardSetupItem(void)
{
    UINT8	i, j;

    DEBUG((-1, "SmcRiserCardSetupItem entry.\n"));
// hide pcie slot root port
    for(i = 0; i < (sizeof(STR_RC_OPROM_Table)/sizeof(UINT16)); i++){
        if(!LibPcdGet32(SlotPcdNumTbl[i]) || (LibPcdGet32(SlotPcdNumTbl[i]) == 0x88888888))
            SetupModifyProtocol->Add2HideList("Setup", STR_RC_OPROM_Table[i]);
        else {
            for(j = 0; j < IIO_Port_Index_Size; j++){
                if(LibPcdGet32(SlotPcdNumTbl[i]) == IIO_Port_Index[j].PORT){
                    Intel_RC_IIO_HideItem |= LShiftU64(1, IIO_Port_Index[j].INDEX);
                    break;
                }
            }
        }
    }
// hide LAN port
    for(i = 0; i < (sizeof(STR_RC_LAN_Table)/sizeof(UINT16)); i++){
        if(!LibPcdGet32(LanPcdNumTbl[i]) || (LibPcdGet32(LanPcdNumTbl[i]) == 0x88888888))
            SetupModifyProtocol->Add2HideList("Setup", STR_RC_LAN_Table[i]);
    }
// hide NVMe port
    for(i = 0; i < (sizeof(STR_RC_NVMe_Table)/sizeof(UINT16)); i++){
        if(!LibPcdGet32(NVMePcdNumTbl[i]) || (LibPcdGet32(NVMePcdNumTbl[i]) == 0x88888888))
            SetupModifyProtocol->Add2HideList("Setup", STR_RC_NVMe_Table[i]);
        else {
            for(j = 0; j < IIO_Port_Index_Size; j++){
                if(LibPcdGet32(NVMePcdNumTbl[i]) == IIO_Port_Index[j].PORT){
                    Intel_RC_IIO_HideItem |= LShiftU64(1, IIO_Port_Index[j].INDEX);
                    break;
                }
            }
        }
    }
// hide SAS port
    if(!PcdGet32(PcdSmcOBSASBDF) || PcdGet32(PcdSmcOBSASBDF) == 0x88888888)
    	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_SAS_SELECT);

    DEBUG((-1, "SmcRiserCardSetupItem end.\n"));

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : RiserCardItemStringModify
//
// Description : 
//
// Parameters  : Board_Pcie_Table[]         MB riser card slot pcie port table.
//               Board_Pcie_Table_Size      MB riser card slot pcie port table size number
//               RC_Table_index             riser card index in database
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
RiserCardItemStringModify(
    IN  UINT32  Board_Pcie_Table[],
    IN  UINT8   Board_Pcie_Table_Size,
    IN  UINT8   RC_Table_index,
    IN  OUT     UINT8   *SkipPcdNum
)
{
    UINT8       x = 0, y, index;
    CHAR8       StrBuff[256];

    for(y = 0; y < Board_Pcie_Table_Size; y++){
        if((Board_Pcie_Table[y] == 0xffffffff) || (!RC_Table[RC_Table_index].SLOT_NUM[y]) ||
                (RC_Table[RC_Table_index].SLOT_NUM[y] == 0xff))
            continue;      //skip 0x00 and 0xff

        gBS->SetMem(StrBuff, sizeof(StrBuff), 0x00);
        switch(RC_Table[RC_Table_index].SLOT_NUM[y] & 0xf0){
        case RC_SLOT:
            for(index = 0; index < SlotPcdNumTblSize; index++){
                if(LibPcdGet32(SlotPcdNumTbl[index]) == Board_Pcie_Table[y]){
                    if(!AsciiStrCmp(RC_Table[RC_Table_index].ID_NAME.RC_NAME, "DEFAULT_STR")){
                        Sprintf(StrBuff, "%s OPROM",
                            RC_Table[RC_Table_index].RC_SLOT_Str[RC_Table[RC_Table_index].SLOT_NUM[y] - 1]);
                    } else {
                        Sprintf(StrBuff, "%s %s OPROM", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                            RC_Table[RC_Table_index].RC_SLOT_Str[RC_Table[RC_Table_index].SLOT_NUM[y] - 1]);
                    }
                    SetupModifyProtocol->SetString("Setup", STRING_TOKEN(STR_RC_OPROM_Table[index]), StrBuff);
                    break;
                }
            }
            break;
        case RC_LAN:
            break;
        case RC_SAS:
            break;
        case RC_NVME:
        	for(index = 0; index < NVMePcdNumTblSize; index++){
                if(LibPcdGet32(NVMePcdNumTbl[index]) == Board_Pcie_Table[y]){
                    Sprintf(StrBuff, "%s NVMe%d OPROM", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                	(RC_Table[RC_Table_index].SLOT_NUM[y] & 0x0f));
                    SetupModifyProtocol->SetString("Setup", STRING_TOKEN(STR_RC_NVMe_Table[index]), StrBuff);
        	            break;
        	    }
        	}
            break;
        case RC_PLX_SLOT:
/*            for(index = 0; index < SlotPcdNumTblSize; index++){
                if((LibPcdGet32(SlotPcdNumTbl[index]) != 0x8888) &&
                    (LibPcdGet32(SlotPcdNumTbl[index]) & 0xff00) &&
                    ((LibPcdGet32(SlotPcdNumTbl[index]) & 0xff00) != 0x8000)){
                    if(x == *SkipPcdNum){
                        Sprintf (StrBuff, "%s SLOT%d OPROM", (RC_Table[RC_Table_index].ID_NAME.L_RC_NAME),
                            (RC_Table[RC_Table_index].SLOT_NUM[y] & 0x0f));
                        SetupModifyProtocol.SetString ("Setup", STRING_TOKEN(STR_RC_OPROM_Table[index]), StrBuff);
                        (*SkipPcdNum)++;
                        x = 0;
                        break;
                    }
                    x++;
                }
            }*/
            break;
        case RC_M2:
            for(index = 0; index < SlotPcdNumTblSize; index++){
                if(LibPcdGet32(SlotPcdNumTbl[index]) == Board_Pcie_Table[y]){
                    Sprintf(StrBuff, "%s %s OPROM", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                        RC_Table[RC_Table_index].RC_M2_Str[(RC_Table[RC_Table_index].SLOT_NUM[y] & 0x0f) - 1]);
                    SetupModifyProtocol->SetString ("Setup", STRING_TOKEN(STR_RC_OPROM_Table[index]), StrBuff);
                    break;
                }
            }
            break;
        }
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcRiserCardSetupString
//
// Description :                
//
// Parameters  : 
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI
SmcRiserCardSetupString(void)
{
    UINT32	SXB1_Tbl[] = BOARD_PCIE_PORT_SXB1;
    UINT32	SXB2_Tbl[] = BOARD_PCIE_PORT_SXB2;
    UINT32	SXB3_Tbl[] = BOARD_PCIE_PORT_SXB3;
    UINT8	SkipPcdNum = 0;

    DEBUG((-1, "SmcRiserCardSetupString entry.\n"));
//re-mapping board pcie table.
    if(SXB1_index){
	RemapPcieTable(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
	RiserCardItemStringModify(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32),
	                SXB1_index, &SkipPcdNum);
    }
    if(SXB2_index){
	RemapPcieTable(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
	RiserCardItemStringModify(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32),
	                SXB2_index, &SkipPcdNum);
    }
    if(SXB3_index){
    	RemapPcieTable(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
    	RiserCardItemStringModify(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32),
    	                SXB3_index, &SkipPcdNum);
    }

    DEBUG((-1, "SmcRiserCardSetupString end.\n"));
    return EFI_SUCCESS;
}

