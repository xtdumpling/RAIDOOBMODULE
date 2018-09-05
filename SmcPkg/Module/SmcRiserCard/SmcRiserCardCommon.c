//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
// Rev.  1.14
//   Bug Fix:   Fix incorrect NVMe port number string.
//   Reason:	
//   Auditor:	Kasber Chen
//   Date:  	Aug/21/2017
//
// Rev.  1.13
//   Bug Fix:   Fix incorrect pcie bifurcate setting when lane reverse.
//   Reason:	
//   Auditor:	Kasber Chen
//   Date:  	Mar/02/2017
//
// Rev. 1.12
//   Reason:	Support riser card M.2 slot.
//   Auditro:	Kasber Chen
//   Date:      Mar/02/2017
//
// Rev. 1.11
//   Reason:	Add riser card MUX channel change.
//   Auditro:	Kasber Chen
//   Date:      Feb/21/2017
//
// Rev. 1.10
//   Reason:	Support OEM type 130 for SXB slots.
//   Auditro:	Kasber Chen
//   Date:      Jan/25/2017
//
//  Rev. 1.09
//    Bug Fix:  Add riser card NVMe device location.
//    Auditor:  Kasber Chen
//    Date:     Jan/17/2017
//
//  Rev. 1.08
//    Bug Fix:  Add riser card redriver function.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/10/2017
//
//  Rev. 1.07
//    Bug Fix:  Support AOC NVMe card in riser card slots.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Nov/16/2016
//
//  Rev. 1.06
//    Bug Fix:  Fix AOC-URN2-i4XT only two LAN OPROM items in setup.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Nov/02/2016
//
//  Rev. 1.05
//    Bug Fix:  Fix some riser card pcie bifurcation change incorrectly.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Sep/30/2016
//
//  Rev. 1.04
//      Bug Fixed:  Support 12 NVMe.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/29/2016
//
//  Rev. 1.03
//      Bug Fixed:  Fix Bus dynamic change issue.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/06/2016
//
//  Rev. 1.02
//    Bug Fix:  Support 6 NVMe
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/14/2016
//
//  Rev. 1.01
//      Bug Fixed:  Add riser card name to IIO and OPROM control items.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/21/2016
//
//    Rev. 1.00
//    Reason:	Initialize revision.
//    Auditor:	Kasber Chen
//    Date:	04/13/2016
//
//**************************************************************************//
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MmPciBaseLib.h>
#include <PciBus.h>
#include "SmcLib.h"
#include "SmcRiserCardCommon.h"
#include "SmcRiserCard.h"
#include "SmcRiserCardDB.h"
#include "SmcRiserCardPei.h"

UINTN	SlotPcdNumTbl[] = {
		PcdToken(PcdSmcSLOT1BDF), PcdToken(PcdSmcSLOT2BDF), PcdToken(PcdSmcSLOT3BDF),
		PcdToken(PcdSmcSLOT4BDF), PcdToken(PcdSmcSLOT5BDF), PcdToken(PcdSmcSLOT6BDF),
		PcdToken(PcdSmcSLOT7BDF), PcdToken(PcdSmcSLOT8BDF), PcdToken(PcdSmcSLOT9BDF),
		PcdToken(PcdSmcSLOT10BDF), PcdToken(PcdSmcSLOT11BDF)};

UINTN	LanPcdNumTbl[] = {
		PcdToken(PcdSmcOBLan1BDF), PcdToken(PcdSmcOBLan2BDF), PcdToken(PcdSmcOBLan3BDF),
		PcdToken(PcdSmcOBLan4BDF)};

UINTN	NVMePcdNumTbl[] = {
		PcdToken(PcdSmcOBNVMe1BDF), PcdToken(PcdSmcOBNVMe2BDF), PcdToken(PcdSmcOBNVMe3BDF),
		PcdToken(PcdSmcOBNVMe4BDF), PcdToken(PcdSmcOBNVMe5BDF), PcdToken(PcdSmcOBNVMe6BDF),
		PcdToken(PcdSmcOBNVMe7BDF), PcdToken(PcdSmcOBNVMe8BDF), PcdToken(PcdSmcOBNVMe9BDF),
		PcdToken(PcdSmcOBNVMe10BDF), PcdToken(PcdSmcOBNVMe11BDF), PcdToken(PcdSmcOBNVMe12BDF)};

UINT8	RC_Table_Size = sizeof(RC_Table)/sizeof(RC_DATA_Table);
UINT8	IIO_Port_Index_Size = sizeof(IIO_Port_Index)/sizeof(SMC_IIO_PORT_INDEX);
UINT8	SlotPcdNumTblSize = sizeof(SlotPcdNumTbl)/sizeof(UINTN);
UINT8	LanPcdNumTblSize = sizeof(LanPcdNumTbl)/sizeof(UINTN);
UINT8	NVMePcdNumTblSize = sizeof(NVMePcdNumTbl)/sizeof(UINTN);

EFI_STATUS
SmcRiserCardRedriverData(
    IN  UINT8   SXB_Index,
    IN  UINT8   RC_ReD_Index,
    IN OUT      UINT8   *RC_MUX_Address,
    IN OUT      UINT8   *RC_MUX_Channel,
    IN OUT      UINT8   *RC_ReD_Add,
    IN OUT      RC_ReD  RC_ReD_Data[],
    IN OUT      UINT8   *RC_ReD_Data_Size
)
{
    DEBUG((-1, "SmcRiserCardRedriverData Start.\n"));
    
    if(!RC_Table[SXB_Index].ReD_Add[RC_ReD_Index])      return EFI_UNSUPPORTED;

    *RC_MUX_Address = RC_Table[SXB_Index].Mux_Add[RC_ReD_Index];
    *RC_MUX_Channel = RC_Table[SXB_Index].Mux_Ch[RC_ReD_Index];
    *RC_ReD_Add = RC_Table[SXB_Index].ReD_Add[RC_ReD_Index];
    *RC_ReD_Data_Size = RC_Table[SXB_Index].ReD_Data_Tbl_S[RC_ReD_Index] / sizeof(RC_ReD);
    MemCpy(RC_ReD_Data, RC_Table[SXB_Index].ReD_Data_Tbl[RC_ReD_Index],
                    RC_Table[SXB_Index].ReD_Data_Tbl_S[RC_ReD_Index]);

    DEBUG((-1, "SmcRiserCardRedriverData End.\n"));
    return EFI_SUCCESS;
}

VOID
SmcRiserCardIDUpdate(
    IN	UINT32	Board_CFG_GPIO[],
    IN	UINT8	Board_CFG_GPIO_Size,
    IN	UINT32	*temp_ID
)
{
    UINT8	index = 0;
    UINT32	temp = 0;

    *temp_ID = 0;
    for(index = 0; index < Board_CFG_GPIO_Size; index++){
	if(Board_CFG_GPIO[index] == 0xffffffff)	continue;
	GpioGetInputValue(Board_CFG_GPIO[index], &temp);
	*temp_ID |= temp << index;
    }
}

UINT8
SmcRiserCardIDCmp(
    IN	UINT32	SMC_RC_ID
)
{
    UINT8	index = 0;

    DEBUG((-1, "SmcRiserCardIDCmp Entry.\n"));

    for(index = 0; index < RC_Table_Size; index++){
	if(RC_Table[index].ID_NAME.RC_ID == SMC_RC_ID){
	    DEBUG((-1, "RC_ID = %x\n", RC_Table[index].ID_NAME.RC_ID));
	    DEBUG((-1, "riser card : %s.\n", RC_Table[index].ID_NAME.L_RC_NAME));
	    return index;
	}
    }
    
    DEBUG((-1, "SmcRiserCardIDCmp End.\n"));
    return 0;	// No riser card searched.
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : GetRiserCardBifuracate
//
// Description : The function depend on how to set pcie bifurcate in system.
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
GetRiserCardBifuracate(
    IN	OUT	IIO_CONFIG	*SetupData,
    IN	UINT32	Board_Pcie_Table[],
    IN	UINT8	Board_Pcie_Table_Size,
    IN	UINT8	RC_Table_index,
    IN	UINT8	DefaultIOU0[],
    IN	UINT8	DefaultIOU1[],
    IN	UINT8	DefaultIOU2[]
)
{
    UINT8	i, j, RP_index;
    
    DEBUG((-1, "GetRiserCardBifuracate entry.\n"));
//program pcie port bifurcate, get board pcie port number then map to RC bifurcate
//-----------------------------------------------
//          |1A,2A,3A|1B,2B,3B|1C,2C,3C|1D,2D,3D|
//x16       |   x16  |    0   |    0   |    0   |
//x8  x8    |    x8  |    0   |   x8   |    0   |
//x4x4x8    |    x4  |   x4   |   x8   |    0   |
//x8  x4x4  |    x8  |    0   |   x4   |   x4   |
//x4x4x4x4  |    x4  |   x4   |   x4   |   x4   |
//-----------------------------------------------
    for(i = 0; i < Board_Pcie_Table_Size; i++){
	DEBUG((-1, "Board_Pcie_Table[%x] = %x.\n", i, Board_Pcie_Table[i]));
	RP_index = 0xff;
//Get root port index
	for(j = 0; j < IIO_Port_Index_Size; j++){
	    if(Board_Pcie_Table[i] == IIO_Port_Index[j].PORT){
		RP_index = IIO_Port_Index[j].INDEX;
		DEBUG((-1, "Board_Pcie_Table[%x] index = %x.\n", i, RP_index));
		break;
	    }
	}
	if(RP_index == 0xff){//unused port, goto next
	    DEBUG((-1, "Board_Pcie_Table[%x] skip.\n", i));
	    continue;
	}
//Set root port configuration
	if(RC_Table[RC_Table_index].SLOT_NUM[i])
	    SetupData->SLOTIMP[RP_index] = 1;
	if(RC_Table[RC_Table_index].SLOT_NUM[i] < 0x10)
	    SetupData->SLOTPSP[RP_index] = RC_Table[RC_Table_index].SLOT_NUM[i];
	else
	    SetupData->SLOTPSP[RP_index] = 0;

	if(Board_Pcie_Table[i] < P1PE0A)
	    RP_index = 0;	//CPU 0
	else
	    RP_index = 1;	//CPU 1
	
	switch(Board_Pcie_Table[i]){
	case P0PE1A:
	case P1PE1A:
	case P0PE2A:
	case P1PE2A:
	case P0PE3A:
	case P1PE3A:
	    if(RC_Table[RC_Table_index].RC_Bifurcate[i] == x16){
		if((Board_Pcie_Table[i] == P0PE1A) || (Board_Pcie_Table[i] == P1PE1A)){
		    if(DefaultIOU0[RP_index] != 0xff)	break;
		    SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_xxxxxx16;
		    DEBUG((-1, "P%xPE1A program to x16.\n", RP_index));
		}
		else if((Board_Pcie_Table[i] == P0PE2A) || (Board_Pcie_Table[i] == P1PE2A)){
		    if(DefaultIOU1[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_xxxxxx16;
		    DEBUG((-1, "P%xPE2A program to x16.\n", RP_index));
		}
		else if((Board_Pcie_Table[i] == P0PE3A) || (Board_Pcie_Table[i] == P1PE3A)){
		    if(DefaultIOU2[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_xxxxxx16;
		    DEBUG((-1, "P%xPE3A program to x16.\n", RP_index));
		}
		break;
	    }
	    else if(RC_Table[RC_Table_index].RC_Bifurcate[i] == x8){
		if((Board_Pcie_Table[i] == P0PE1A) || (Board_Pcie_Table[i] == P1PE1A)){
		    if(DefaultIOU0[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_xxx8x4x4)){
			SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_xxx8xxx8;
			DEBUG((-1, "P%xPE1A program to x8x8.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_x4x4x4x4){
			SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_x4x4xxx8;
			DEBUG((-1, "P%xPE1A program to x8x4x4.\n", RP_index));
		    }
		}
		else if((Board_Pcie_Table[i] == P0PE2A) || (Board_Pcie_Table[i] == P1PE2A)){
	    	    if(DefaultIOU1[RP_index] != 0xff)	break;	//user change pcie bifurcate
	    	    if((SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_AUTO) ||
	    		    (SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
	    		    (SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_xxx8x4x4)){
	    		SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_xxx8xxx8;
	    		DEBUG((-1, "P%xPE2A program to x8x8.\n", RP_index));
	    	    }
	    	    else if(SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_x4x4x4x4){
	    		SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_x4x4xxx8;
	    		DEBUG((-1, "P%xPE2A program to x8x4x4.\n", RP_index));
	    	    }
		}
		else if((Board_Pcie_Table[i] == P0PE3A) || (Board_Pcie_Table[i] == P1PE3A)){
		    if(DefaultIOU2[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_xxx8x4x4)){
			SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_xxx8xxx8;
			DEBUG((-1, "P%xPE3A program to x8x8.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_x4x4x4x4){
			SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_x4x4xxx8;
			DEBUG((-1, "P%xPE3A program to x8x4x4.\n", RP_index));
		    }
		}
		break;
	    }
	case P0PE1B:
	case P1PE1B:
	case P0PE2B:
	case P1PE2B:
	case P0PE3B:
	case P1PE3B:
	    if(RC_Table[RC_Table_index].RC_Bifurcate[i] == x4){
		if((Board_Pcie_Table[i] == P0PE1A) || (Board_Pcie_Table[i] == P0PE1B) ||
			(Board_Pcie_Table[i] == P1PE1A) || (Board_Pcie_Table[i] == P1PE1B)){
		    if(DefaultIOU0[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_xxx8xxx8)){
			SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_xxx8x4x4;
			DEBUG((-1, "P%xPE1B program to x4x4x8.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_x4x4xxx8){
			SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_x4x4x4x4;
			DEBUG((-1, "P%xPE1B program to x4x4x4x4.\n", RP_index));
		    }
		}
		else if((Board_Pcie_Table[i] == P0PE2A) || (Board_Pcie_Table[i] == P0PE2B) ||
			(Board_Pcie_Table[i] == P1PE2A) || (Board_Pcie_Table[i] == P1PE2B)){
		    if(DefaultIOU1[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_xxx8xxx8)){
			SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_xxx8x4x4;
			DEBUG((-1, "P%xPE2B program to x4x4x8.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_x4x4xxx8){
			SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_x4x4x4x4;
			DEBUG((-1, "P%xPE2B program to x4x4x4x4.\n", RP_index));
		    }
		}
		else if((Board_Pcie_Table[i] == P0PE3A) || (Board_Pcie_Table[i] == P0PE3B) ||
			(Board_Pcie_Table[i] == P1PE3A) || (Board_Pcie_Table[i] == P1PE3B)){
		    if(DefaultIOU2[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_xxx8xxx8)){
			SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_xxx8x4x4;
			DEBUG((-1, "P%xPE3B program to x4x4x8.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_x4x4xxx8){
			SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_x4x4x4x4;
			DEBUG((-1, "P%xPE3B program to x4x4x4x4.\n", RP_index));
		    }
		}
	    }
	    break;
	case P0PE1C:
	case P1PE1C:
	case P0PE2C:
	case P1PE2C:
	case P0PE3C:
	case P1PE3C:
	    if(RC_Table[RC_Table_index].RC_Bifurcate[i] == x8){
		if((Board_Pcie_Table[i] == P0PE1C) || (Board_Pcie_Table[i] == P1PE1C)){
		    if(DefaultIOU0[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_x4x4xxx8)){
			SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_xxx8xxx8;
			DEBUG((-1, "P%xPE1C program to x8x8.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_x4x4x4x4){
			SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_xxx8x4x4;
			DEBUG((-1, "P%xPE1C program to x4x4x8.\n", RP_index));
		    }
		}
		else if((Board_Pcie_Table[i] == P0PE2C) || (Board_Pcie_Table[i] == P1PE2C)){
		    if(DefaultIOU1[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_x4x4xxx8)){
			SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_xxx8xxx8;
			DEBUG((-1, "P%xPE2C program to x8x8.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_x4x4x4x4){
			SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_xxx8x4x4;
			DEBUG((-1, "P%xPE2C program to x4x4x8.\n", RP_index));
		    }
		}
		else if((Board_Pcie_Table[i] == P0PE3C) || (Board_Pcie_Table[i] == P1PE3C)){
		    if(DefaultIOU2[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_x4x4xxx8)){
			SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_xxx8xxx8;
			DEBUG((-1, "P%xPE3C program to x8x8.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_x4x4x4x4){
			SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_xxx8x4x4;
			DEBUG((-1, "P%xPE3C program to x4x4x8.\n", RP_index));
		    }
		}
		break;
	    }
	case P0PE1D:
	case P1PE1D:
	case P0PE2D:
	case P1PE2D:
	case P0PE3D:
	case P1PE3D:
	    if(RC_Table[RC_Table_index].RC_Bifurcate[i] == x4){
		if((Board_Pcie_Table[i] == P0PE1C) || (Board_Pcie_Table[i] == P0PE1D) ||
			(Board_Pcie_Table[i] == P1PE1C) || (Board_Pcie_Table[i] == P1PE1D)){
		    if(DefaultIOU0[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_xxx8xxx8)){
			SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_x4x4xxx8;
			DEBUG((-1, "P%xPE1D program to x8x4x4.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU0[RP_index] == IIO_BIFURCATE_xxx8x4x4){
			SetupData->ConfigIOU0[RP_index] = IIO_BIFURCATE_x4x4x4x4;
			DEBUG((-1, "P%xPE1D program to x4x4x4x4.\n", RP_index));
		    }
		}
		else if((Board_Pcie_Table[i] == P0PE2C) || (Board_Pcie_Table[i] == P0PE2D) ||
			(Board_Pcie_Table[i] == P1PE2C) || (Board_Pcie_Table[i] == P1PE2D)){
		    if(DefaultIOU1[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_xxx8xxx8)){
			SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_x4x4xxx8;
			DEBUG((-1, "P%xPE2D program to x8x4x4.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU1[RP_index] == IIO_BIFURCATE_xxx8x4x4){
			SetupData->ConfigIOU1[RP_index] = IIO_BIFURCATE_x4x4x4x4;
			DEBUG((-1, "P%xPE2D program to x4x4x4x4.\n", RP_index));
		    }
		}
		else if((Board_Pcie_Table[i] == P0PE3C) || (Board_Pcie_Table[i] == P0PE3D) ||
			(Board_Pcie_Table[i] == P1PE3C) || (Board_Pcie_Table[i] == P1PE3D)){
		    if(DefaultIOU2[RP_index] != 0xff)	break;	//user change pcie bifurcate
		    if((SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_AUTO) ||
			    (SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_xxxxxx16) || 
			    (SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_xxx8xxx8)){
			SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_x4x4xxx8;
			DEBUG((-1, "P%xPE3D program to x8x4x4.\n", RP_index));
		    }
		    else if(SetupData->ConfigIOU2[RP_index] == IIO_BIFURCATE_xxx8x4x4){
			SetupData->ConfigIOU2[RP_index] = IIO_BIFURCATE_x4x4x4x4;
			DEBUG((-1, "P%xPE3D program to x4x4x4x4.\n", RP_index));
		    }
		}
	    }
	    break;
	default:
	    break;
	}
    }
    DEBUG((-1, "GetRiserCardBifuracate end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : Swap
//
// Description : Swap two paramaters.
//
// Parameters  : 
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
Swap(
    IN	OUT	UINT32	*AAA,
    IN	OUT	UINT32	*BBB
)
{
    UINT32	Temp;
    
    Temp = *AAA;
    *AAA = *BBB;
    *BBB = Temp;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : RemapPcieTable
//
// Description : Some mother board design pcie port reversed.
//               In pcie port reversed case, we must remap pcie port or 
//               system riser card device and slot will map to wrong pcie port.
//
// Rule :        If in riser card is a X16 slot, it mean pcie port must be 2A or 3A
//               If in riser card is a X8 slot, it mean pcie port must be 1A, 2A, 2C, 3A, 3C
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
RemapPcieTable(
    IN	OUT	UINT32	Board_Pcie_Table[],
    IN	UINT8	Board_Pcie_Table_Size,
    IN	UINT8	RC_Table_index
)
{
    UINT8	i, j, Remap;
    UINT32	Remap_Tbl[] = {P0PE1B, P0PE1D, P0PE2B, P0PE2D, P0PE3B, P0PE3D,
		    	       P1PE1B, P1PE1D, P1PE2B, P1PE2D, P1PE3B, P1PE3D};

    DEBUG((-1, "RemapPcieTable entry.\n"));
    for(i = 0; i < Board_Pcie_Table_Size; i++){
//is it a reversed port
	Remap = 0;
    	if((!RC_Table[RC_Table_index].SLOT_NUM[i]) ||
    	        (RC_Table[RC_Table_index].SLOT_NUM[i] == 0xff))
    	    continue;

    	for(j = 0; j < sizeof(Remap_Tbl)/sizeof(UINT32); j++){
    	    if(Board_Pcie_Table[i] == Remap_Tbl[j]){
    		Remap = 1;    
    		break;
    	    }
    	}
    	if(Remap){
    	    if(RC_Table[RC_Table_index].RC_Bifurcate[i] == x16){	//lane reversed and x16
    		Swap(&Board_Pcie_Table[i], &Board_Pcie_Table[i + 3]);
    		Swap(&Board_Pcie_Table[i + 1], &Board_Pcie_Table[i + 2]);
    		i += 3;
    	    }
    	    else if(RC_Table[RC_Table_index].RC_Bifurcate[i] == x8){	//lane reversed and x8 or x4
    		Swap(&Board_Pcie_Table[i], &Board_Pcie_Table[i + 1]);
    		i += 1;
    	    } else
    	        i += 1;
    	}
    	else {
    	    if(RC_Table[RC_Table_index].RC_Bifurcate[i] == x16)
    		i += 3;
    	    else if(RC_Table[RC_Table_index].RC_Bifurcate[i] <= x8)
    		i += 1;
    	}
    }
    DEBUG((-1, "RemapPcieTable end.\n"));
}

VOID
CreateSlotInfoTable(
    IN  UINT32  Board_Pcie_Table[],
    IN  UINT8   Board_Pcie_Table_Size,
    IN  UINT8   RC_Table_index,
    IN OUT      SMC_SLOT_INFO   SXB_SLOT_INFO_Tbl[]
)
{
    UINT8       i, slot_num;

    DEBUG((-1, "CreateSlotInfoTable entry.\n"));

    for(i = 0; i < Board_Pcie_Table_Size; i++){  //search slot in riser card
        if((!RC_Table[RC_Table_index].SLOT_NUM[i]) ||
                (RC_Table[RC_Table_index].SLOT_NUM[i] == 0xff))
            continue;      //skip 0x00 and 0xff

        switch(RC_Table[RC_Table_index].SLOT_NUM[i] & 0xf0){
        case RC_SLOT:
            if(!RC_Table[RC_Table_index].SLOT_NUM[i])   break;
            slot_num = RC_Table[RC_Table_index].SLOT_NUM[i] - 1;
            SXB_SLOT_INFO_Tbl[slot_num].Address = RC_Table[RC_Table_index].Mux_Add[i];
            SXB_SLOT_INFO_Tbl[slot_num].Channel = RC_Table[RC_Table_index].Mux_Ch[i];
            SXB_SLOT_INFO_Tbl[slot_num].BDF = Board_Pcie_Table[i];
            SXB_SLOT_INFO_Tbl[slot_num].Bifurcate = RC_Table[RC_Table_index].RC_Bifurcate[i];
            break;
        }
    }
//    for(i = 0; i < MAX_SLOT; i++){
//        DEBUG((-1, "SXB_SLOT_INFO_Tbl[%d] = {%x, %x, %x, %x}\n", i, SXB_SLOT_INFO_Tbl[i].Address,
//            SXB_SLOT_INFO_Tbl[i].Channel, SXB_SLOT_INFO_Tbl[i].BDF, SXB_SLOT_INFO_Tbl[i].Bifurcate));
//    }
    DEBUG((-1, "CreateSlotInfoTable end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : RedefineDevBDF
//
// Description : 
//               
// Parameters  : Board_Pcie_Table[]         MB riser card slot pcie port table.
//               Board_Pcie_Table_Size      MB riser card slot pcie port table size number
//               RC_Table_index             riser card index in database
//
// Returns     : None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
RedefineDevBDF(
    IN	UINT32	Board_Pcie_Table[],
    IN	UINT8	Board_Pcie_Table_Size,
    IN	UINT8	RC_Table_index
)
{
    UINT8	i, j, /*TempBus,*/ PcdTblIndex;
//    UINT16	TempBDF;

    DEBUG((-1, "RedefineDevBDF Start.\n"));

    for(i = 0; i < Board_Pcie_Table_Size; i++){
        if((Board_Pcie_Table[i] == 0xffffffff) || (!RC_Table[RC_Table_index].SLOT_NUM[i]) ||
            (RC_Table[RC_Table_index].SLOT_NUM[i] == 0xff))
            continue;

        switch(RC_Table[RC_Table_index].SLOT_NUM[i] & 0xF0){
        case RC_SLOT:
        case RC_M2:
            for(PcdTblIndex = 0; PcdTblIndex < SlotPcdNumTblSize; PcdTblIndex++){
                if(LibPcdGet32(SlotPcdNumTbl[PcdTblIndex]) == 0x88888888){
                    LibPcdSet32(SlotPcdNumTbl[PcdTblIndex], Board_Pcie_Table[i]);
                    DEBUG((-1, "PcdSmcSLOT%xBDF = %x.\n", PcdTblIndex, Board_Pcie_Table[i]));
                    break;
                }
            }
            break;
        case RC_LAN:
//            if(PcdGet8(PcdSmcMultiFuncLanChip1) == 0x88){
                PcdSet8(PcdSmcMultiFuncLanChip1, RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f);
                DEBUG((-1, "PcdSmcMultiFuncLanChip1 = %x.\n", PcdGet8(PcdSmcMultiFuncLanChip1)));
                for(j = 0; j < (RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f); j++){
                    for(PcdTblIndex = 0; PcdTblIndex < LanPcdNumTblSize; PcdTblIndex++){
                	if(LibPcdGet32(LanPcdNumTbl[PcdTblIndex]) == 0x88888888){
                	    LibPcdSet32(LanPcdNumTbl[PcdTblIndex], Board_Pcie_Table[i]);
                	    DEBUG((-1, "PcdSmcOBLan%xBDF = %x.\n", PcdTblIndex, Board_Pcie_Table[i]));
                	    break;
                	}
                    }
                }
//            }
            break;
        case RC_SAS:
            break;
        case RC_NVME:
            for(PcdTblIndex = 0; PcdTblIndex < NVMePcdNumTblSize; PcdTblIndex++){
                if(LibPcdGet32(NVMePcdNumTbl[PcdTblIndex]) == 0x88888888){
                    LibPcdSet32(NVMePcdNumTbl[PcdTblIndex], Board_Pcie_Table[i]);
                    DEBUG((-1, "PcdSmcOBNVMe%xBDF = %x.\n", PcdTblIndex, Board_Pcie_Table[i]));
                    break;
                }
            }
            break;
        case RC_PLX_SLOT:
/*            TempBDF = SmcBDFTransfer(Board_Pcie_Table[i]);
            TempBus = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x19);
            TempBus++;	//next bus is slot root port in PLX
            for(j = 0; j <= 0x1f; j += 4){	// count how many slots in PLX
                if(*(UINT8*)MmPciAddress(0, TempBus, j, 0, 0) == 0xff)
                    continue;
                TempBDF = ((UINT16)TempBus << 8) + (j << 3);
                for(PcdTblIndex = 0; PcdTblIndex < SlotPcdNumTblSize; PcdTblIndex++){
                    if(LibPcdGet32(SlotPcdNumTbl[PcdTblIndex]) == 0x88888888){
                    	LibPcdSet32(SlotPcdNumTbl[PcdTblIndex], (UINT32)TempBDF);
                    	DEBUG((-1, "PcdSmcSLOT%xBDF = %x.\n", PcdTblIndex, (UINT32)TempBDF));
                        break;
                    }
                }
            }*/
            break;
        }
    }
    DEBUG((-1, "RedefineDevBDF End.\n"));
}

VOID
SetRiserCardNVMeLocation(
    IN  UINT32  Board_Pcie_Table[],
    IN  UINT8   Board_Pcie_Table_Size,
    IN  UINT8   RC_Table_index
)
{
    UINT8       i, j, CPU_Num, IIO_Num;
    UINT64      RiserCardNVMe, NVMeLocation = 0;

    DEBUG((-1, "SetRiserCardNVMeLocation Start.\n"));
    for(i = 0; i < Board_Pcie_Table_Size; i++){
        if((Board_Pcie_Table[i] == 0xffffffff) || (!RC_Table[RC_Table_index].SLOT_NUM[i]) ||
                (RC_Table[RC_Table_index].SLOT_NUM[i] == 0xff))
            continue;

        if(!(Board_Pcie_Table[i] >> 16))        continue;       //only support on CPU port

        switch(RC_Table[RC_Table_index].SLOT_NUM[i] & 0xF0){
        case RC_NVME:
            CPU_Num = ((UINT8)(Board_Pcie_Table[i] >> 20) & 0x0f) - 1;
            IIO_Num = ((UINT8)(Board_Pcie_Table[i] >> 16) & 0x0f) - 1;
            j = CPU_Num * 32 + IIO_Num * 8;                     //IIO A

            if((UINT8)Board_Pcie_Table[i] == 0x08)              //IIO B
                j += 2;
            else if((UINT8)Board_Pcie_Table[i] == 0x10)         //IIO C
                j += 4;
            else if((UINT8)Board_Pcie_Table[i] == 0x18)         //IIO D
                j += 6;
                    
            NVMeLocation |= LShiftU64(1, j);
            break;
        }
    }
    
    if(!NVMeLocation)   return;

    DEBUG((-1, "NVMeLocation = %lx.\n", NVMeLocation));
    RiserCardNVMe = PcdGet64(PcdSmcAOCLocation);
    RiserCardNVMe |= NVMeLocation;
    PcdSet64(PcdSmcAOCLocation, RiserCardNVMe);
    DEBUG((-1, "SetRiserCardNVMeLocation End.\n"));
    return;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  SmcRiserCardBifuracate
//
// Description :  the function depend on system pcie bifurcate changed function. 
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI
SmcRiserCardBifuracate(
    IN OUT	IIO_CONFIG	*SetupData,
    IN		UINT8		DefaultIOU0[],
    IN		UINT8		DefaultIOU1[],
    IN		UINT8		DefaultIOU2[]
)
{
    UINT32	SXB1_Tbl[] = BOARD_PCIE_PORT_SXB1;
    UINT32	SXB2_Tbl[] = BOARD_PCIE_PORT_SXB2;
    UINT32	SXB3_Tbl[] = BOARD_PCIE_PORT_SXB3;
    UINT8	SXB1_index = PcdGet8(PcdSmcSXB1Index);
    UINT8	SXB2_index = PcdGet8(PcdSmcSXB2Index);
    UINT8	SXB3_index = PcdGet8(PcdSmcSXB3Index);

    DEBUG((-1, "SmcRiserCardBifuracate entry.\n"));
    DEBUG((-1, "SXB1_index = %x.\n", SXB1_index));
    DEBUG((-1, "SXB2_index = %x.\n", SXB2_index));
    DEBUG((-1, "SXB3_index = %x.\n", SXB3_index));

    if(SXB1_index){
	RemapPcieTable(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
//Get left riser card bifurcate
	DEBUG((-1, "SmcRiserCardBifuracate SXB1, SXB1_index = %x.\n", SXB1_index));
	GetRiserCardBifuracate(SetupData, SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32),
			SXB1_index, DefaultIOU0, DefaultIOU1, DefaultIOU2);
	SetRiserCardNVMeLocation(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
    }
    if(SXB2_index){
	RemapPcieTable(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
//Get right riser card bifurcate
	DEBUG((-1, "SmcRiserCardBifuracate SXB2, SXB2_index = %x.\n", SXB2_index));
	GetRiserCardBifuracate(SetupData, SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32),
			SXB2_index, DefaultIOU0, DefaultIOU1, DefaultIOU2);
	SetRiserCardNVMeLocation(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
    }
    if(SXB3_index){
	RemapPcieTable(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
//Get ultra riser card bifurcate
	DEBUG((-1, "SmcRiserCardBifuracate SXB3, SXB3_index = %x.\n", SXB3_index));
	GetRiserCardBifuracate(SetupData, SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32),
			SXB3_index, DefaultIOU0, DefaultIOU1, DefaultIOU2);
	SetRiserCardNVMeLocation(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
    }
    DEBUG((-1, "SmcRiserCardBifuracate end.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI
SmcRiserCardSlotBDFTable(
    IN OUT      SMC_SLOT_INFO   SXB1_SLOT_INFO_Tbl[],
    IN OUT      SMC_SLOT_INFO   SXB2_SLOT_INFO_Tbl[],
    IN OUT      SMC_SLOT_INFO   SXB3_SLOT_INFO_Tbl[]
)
{
    UINT32      SXB1_Tbl[] = BOARD_PCIE_PORT_SXB1;
    UINT32      SXB2_Tbl[] = BOARD_PCIE_PORT_SXB2;
    UINT32      SXB3_Tbl[] = BOARD_PCIE_PORT_SXB3;
    UINT8       SXB1_index = PcdGet8(PcdSmcSXB1Index);
    UINT8       SXB2_index = PcdGet8(PcdSmcSXB2Index);
    UINT8       SXB3_index = PcdGet8(PcdSmcSXB3Index);

    DEBUG((-1, "SmcRiserCardSlotBDFTable entry.\n"));
    MemSet((UINT8*)SXB1_SLOT_INFO_Tbl, sizeof(SMC_SLOT_INFO) * MAX_SLOT, 0);
    MemSet((UINT8*)SXB2_SLOT_INFO_Tbl, sizeof(SMC_SLOT_INFO) * MAX_SLOT, 0);
    MemSet((UINT8*)SXB3_SLOT_INFO_Tbl, sizeof(SMC_SLOT_INFO) * MAX_SLOT, 0);

    if(SXB1_SLOT_INFO_Tbl && SXB1_index){
        DEBUG((-1, "SmcRiserCardSlotBDFTable SXB1\n"));
//        RemapPcieTable(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
        CreateSlotInfoTable(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index, SXB1_SLOT_INFO_Tbl);
    }
    if(SXB2_SLOT_INFO_Tbl && SXB2_index){
        DEBUG((-1, "SmcRiserCardSlotBDFTable SXB2\n"));
//        RemapPcieTable(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
        CreateSlotInfoTable(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index, SXB2_SLOT_INFO_Tbl);
    }
    if(SXB3_SLOT_INFO_Tbl && SXB3_index){
        DEBUG((-1, "SmcRiserCardSlotBDFTable SXB3\n"));
//        RemapPcieTable(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
        CreateSlotInfoTable(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index, SXB3_SLOT_INFO_Tbl);
    }
    DEBUG((-1, "SmcRiserCardSlotBDFTable end.\n"));
    return EFI_SUCCESS;
}

