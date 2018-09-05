//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix:  Support default string for SPG.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Apr/18/2017
//
//  Rev. 1.05
//    Bug Fix:  Change HideItem policy to fix TXT can being enabled.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/07/2017
//
//  Rev. 1.04
//    Reason:	Support OEM type 130 for SXB slots.
//    Auditro:	Kasber Chen
//    Date:     Jan/25/2017
//
//  Rev. 1.03
//    Bug Fix:  Hide unused items when no any riser card.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/19/2017
//
//  Rev. 1.02
//    Bug Fix:  Correct IIO string.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Nov/25/2016
//
//  Rev. 1.01
//    Bug Fix:  Dynamic hide unused IIO and VMD items.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Nov/11/2016
//
//  Rev. 1.00
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//****************************************************************************
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <AmiLib.h>
#include "EFI.h"
#include <Token.h>
#include "Uefi\UefiInternalFormRepresentation.h"
#include "SmcRiserCardCommon.h"
#include "SmcSetupModify.h"
#include "SmcRiserCard.h"
#if DEBUG_MODE
#include <..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\PurleyPlatPkg\Platform\Dxe\SocketSetup\SocketSetup\DEBUG\SocketSetupStrDefs.h>
#else
#include <..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\PurleyPlatPkg\Platform\Dxe\SocketSetup\SocketSetup\DEBUG\SocketSetupStrDefs.h>
#endif

extern  SMC_SETUP_MODIFY_PROTOCOL       *SetupModifyProtocol;
extern	RC_DATA_Table	RC_Table[];
extern	SMC_IIO_PORT_INDEX	IIO_Port_Index[];
extern	UINT8	IIO_Port_Index_Size;
extern	UINT8	SXB1_index, SXB2_index, SXB3_index;
extern	UINTN	SlotPcdNumTbl[], LanPcdNumTbl[], NVMePcdNumTbl[];
extern	UINT8	SlotPcdNumTblSize, LanPcdNumTblSize, NVMePcdNumTblSize;
extern  UINT64  Intel_RC_IIO_HideItem;

EFI_STRING_ID   StringId[] = {
        STRING_TOKEN(STR_PCIE_PORT_0_PROMPT), STRING_TOKEN(STR_PCIE_PORT_1_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_2_PROMPT), STRING_TOKEN(STR_PCIE_PORT_3_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_4_PROMPT), STRING_TOKEN(STR_PCIE_PORT_5_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_6_PROMPT), STRING_TOKEN(STR_PCIE_PORT_7_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_8_PROMPT), STRING_TOKEN(STR_PCIE_PORT_9_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_10_PROMPT), STRING_TOKEN(STR_PCIE_PORT_11_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_12_PROMPT), STRING_TOKEN(STR_PCIE_PORT_13_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_14_PROMPT), STRING_TOKEN(STR_PCIE_PORT_15_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_16_PROMPT), STRING_TOKEN(STR_PCIE_PORT_17_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_18_PROMPT), STRING_TOKEN(STR_PCIE_PORT_19_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_20_PROMPT), STRING_TOKEN(STR_PCIE_PORT_21_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_22_PROMPT), STRING_TOKEN(STR_PCIE_PORT_23_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_24_PROMPT), STRING_TOKEN(STR_PCIE_PORT_25_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_26_PROMPT), STRING_TOKEN(STR_PCIE_PORT_27_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_28_PROMPT), STRING_TOKEN(STR_PCIE_PORT_29_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_30_PROMPT), STRING_TOKEN(STR_PCIE_PORT_31_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_32_PROMPT), STRING_TOKEN(STR_PCIE_PORT_33_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_34_PROMPT), STRING_TOKEN(STR_PCIE_PORT_35_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_36_PROMPT), STRING_TOKEN(STR_PCIE_PORT_37_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_38_PROMPT), STRING_TOKEN(STR_PCIE_PORT_39_PROMPT),
        STRING_TOKEN(STR_PCIE_PORT_40_PROMPT), STRING_TOKEN(STR_PCIE_PORT_41_PROMPT)};

EFI_STRING_ID       StringVMD[] = {0,   //reserve area for CPU1 DMI
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_0), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_1),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_2), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_3),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_4), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_5),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_6), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_7),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_8), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_9),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_10), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_11),
        0, 0, 0, 0, 0, 0, 0, 0, 0,      //reserve area for CPU1 MCP0, MCP1, CPU2 DMI
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_12), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_13),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_14), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_15),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_16), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_17),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_18), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_19),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_20), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_21),
        STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_22), STRING_TOKEN(STR_VMDPORTENABLE_PROMPT_23),
        0, 0, 0, 0, 0, 0, 0, 0};        //reserve area for CPU2 MCP0, MCP1

UINT16  STR_IIO_Table[] = {
        STR_PCIE_PORT_0_PROMPT, STR_PCIE_PORT_1_PROMPT, STR_PCIE_PORT_2_PROMPT,
        STR_PCIE_PORT_3_PROMPT, STR_PCIE_PORT_4_PROMPT, STR_PCIE_PORT_5_PROMPT,
        STR_PCIE_PORT_6_PROMPT, STR_PCIE_PORT_7_PROMPT, STR_PCIE_PORT_8_PROMPT,
        STR_PCIE_PORT_9_PROMPT, STR_PCIE_PORT_10_PROMPT, STR_PCIE_PORT_11_PROMPT,
        STR_PCIE_PORT_12_PROMPT, STR_PCIE_PORT_13_PROMPT, STR_PCIE_PORT_14_PROMPT,
        STR_PCIE_PORT_15_PROMPT, STR_PCIE_PORT_16_PROMPT, STR_PCIE_PORT_17_PROMPT,
        STR_PCIE_PORT_18_PROMPT, STR_PCIE_PORT_19_PROMPT, STR_PCIE_PORT_20_PROMPT,
        STR_PCIE_PORT_21_PROMPT, STR_PCIE_PORT_22_PROMPT, STR_PCIE_PORT_23_PROMPT,
        STR_PCIE_PORT_24_PROMPT, STR_PCIE_PORT_25_PROMPT, STR_PCIE_PORT_26_PROMPT,
        STR_PCIE_PORT_27_PROMPT, STR_PCIE_PORT_28_PROMPT, STR_PCIE_PORT_29_PROMPT,
        STR_PCIE_PORT_30_PROMPT, STR_PCIE_PORT_31_PROMPT, STR_PCIE_PORT_32_PROMPT,
        STR_PCIE_PORT_33_PROMPT, STR_PCIE_PORT_34_PROMPT, STR_PCIE_PORT_35_PROMPT,
        STR_PCIE_PORT_36_PROMPT, STR_PCIE_PORT_37_PROMPT, STR_PCIE_PORT_38_PROMPT,
        STR_PCIE_PORT_39_PROMPT, STR_PCIE_PORT_40_PROMPT, STR_PCIE_PORT_41_PROMPT};

UINT16  STR_VMD_Table[] = {0,   //reserve area for CPU1 DMI
        STR_VMDPORTENABLE_PROMPT_0, STR_VMDPORTENABLE_PROMPT_1,
        STR_VMDPORTENABLE_PROMPT_2, STR_VMDPORTENABLE_PROMPT_3,
        STR_VMDPORTENABLE_PROMPT_4, STR_VMDPORTENABLE_PROMPT_5,
        STR_VMDPORTENABLE_PROMPT_6, STR_VMDPORTENABLE_PROMPT_7,
        STR_VMDPORTENABLE_PROMPT_8, STR_VMDPORTENABLE_PROMPT_9,
        STR_VMDPORTENABLE_PROMPT_10, STR_VMDPORTENABLE_PROMPT_11,
        0, 0, 0, 0, 0, 0, 0, 0, 0,      //reserve area for CPU1 MCP0, MCP1, CPU2 DMI
        STR_VMDPORTENABLE_PROMPT_12, STR_VMDPORTENABLE_PROMPT_13,
        STR_VMDPORTENABLE_PROMPT_14, STR_VMDPORTENABLE_PROMPT_15,
        STR_VMDPORTENABLE_PROMPT_16, STR_VMDPORTENABLE_PROMPT_17,
        STR_VMDPORTENABLE_PROMPT_18, STR_VMDPORTENABLE_PROMPT_19,
        STR_VMDPORTENABLE_PROMPT_20, STR_VMDPORTENABLE_PROMPT_21,
        STR_VMDPORTENABLE_PROMPT_22, STR_VMDPORTENABLE_PROMPT_23,
        0, 0, 0, 0, 0, 0, 0, 0};        //reserve area for CPU2 MCP0, MCP1};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : RiserCardIioStringModify
//
// Description : Update Iio string to BIOS setup, depend on how to update BIOS setup string.
//               You can set a default string, please refer TIO database.
//               If no any riser card, system show default string.                
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
RiserCardIioStringModify(
    IN	UINT32	Board_Pcie_Table[],
    IN	UINT8	Board_Pcie_Table_Size,
    IN	UINT8	RC_Table_index
)
{
    CHAR8	*TempString;
    UINTN	StrSize = 100;
    UINT8	x, y, index, PcdTblIndex;

    DEBUG((-1, "RiserCardIioStringModify entry.\n"));

    gBS->AllocatePool(EfiBootServicesData, 100, &TempString);
    for(x = 0; x < Board_Pcie_Table_Size; x++){
        if((Board_Pcie_Table[x] == 0xffffffff) || (!RC_Table[RC_Table_index].SLOT_NUM[x]) ||
                (RC_Table[RC_Table_index].SLOT_NUM[x] == 0xff))
            continue;      //skip 0x00 and 0xff

        gBS->SetMem(TempString, 100, 0x00);
        switch(RC_Table[RC_Table_index].SLOT_NUM[x] & 0xf0){
        case RC_SLOT:
            for(PcdTblIndex = 0; PcdTblIndex < SlotPcdNumTblSize; PcdTblIndex++){
                if(LibPcdGet32(SlotPcdNumTbl[PcdTblIndex]) == Board_Pcie_Table[x]){
                    for(y = 0; y < IIO_Port_Index_Size; y++){
                        if(Board_Pcie_Table[x] == IIO_Port_Index[y].PORT){
                            index = IIO_Port_Index[y].INDEX;
                            if(!AsciiStrCmp(RC_Table[RC_Table_index].ID_NAME.RC_NAME, "DEFAULT_STR")){
                                Sprintf(TempString, "%s",
                                        RC_Table[RC_Table_index].RC_SLOT_Str[RC_Table[RC_Table_index].SLOT_NUM[x] - 1]);
                                SetupModifyProtocol->SetString("Socket Configuration", StringId[index], TempString);
                                Sprintf(TempString, "%s VMD",
                                        RC_Table[RC_Table_index].RC_SLOT_Str[RC_Table[RC_Table_index].SLOT_NUM[x] - 1]);
                                SetupModifyProtocol->SetString("Socket Configuration", StringVMD[index], TempString);
                            } else {
                                Sprintf(TempString, "%s SLOT%d", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                                        (RC_Table[RC_Table_index].SLOT_NUM[x]));
                                SetupModifyProtocol->SetString("Socket Configuration", StringId[index], TempString);
                                Sprintf(TempString, "%s SLOT%d VMD", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                                        (RC_Table[RC_Table_index].SLOT_NUM[x]));
                                SetupModifyProtocol->SetString("Socket Configuration", StringVMD[index], TempString);
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        case RC_LAN:
            break;
        case RC_SAS:
            break;
        case RC_NVME:
            for(PcdTblIndex = 0; PcdTblIndex < NVMePcdNumTblSize; PcdTblIndex++){
                if(LibPcdGet32(NVMePcdNumTbl[PcdTblIndex]) == Board_Pcie_Table[x]){
                    for(y = 0; y < IIO_Port_Index_Size; y++){
                        if(Board_Pcie_Table[x] == IIO_Port_Index[y].PORT){
                            index = IIO_Port_Index[y].INDEX;
                            Sprintf(TempString, "%s NVME%d", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                                    (RC_Table[RC_Table_index].SLOT_NUM[x]) & 0x0f);
                            SetupModifyProtocol->SetString("Socket Configuration", StringId[index], TempString);
                            Sprintf(TempString, "%s NVME%d VMD", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                                    (RC_Table[RC_Table_index].SLOT_NUM[x]) & 0x0f);
                            SetupModifyProtocol->SetString("Socket Configuration", StringVMD[index], TempString);
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        case RC_PLX_SLOT:
            break;
        }
    }
    gBS->FreePool(TempString);
    
    DEBUG((-1, "RiserCardIioStringModify end.\n"));
    return;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcRiserCardIioSetup
//
// Description : The function depend on system how to set Iio.
//
// Parameters  : 
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI
SmcRiserCardIioSetup(void)
{
    UINT32	SXB1_Tbl[] = BOARD_PCIE_PORT_SXB1;
    UINT32	SXB2_Tbl[] = BOARD_PCIE_PORT_SXB2;
    UINT32	SXB3_Tbl[] = BOARD_PCIE_PORT_SXB3;
    UINT8       x;

    DEBUG((-1, "SmcRiserCardIioSetup entry.\n"));

    if(SXB1_index){
    	RemapPcieTable(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
    	RiserCardIioStringModify(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
    }
    if(SXB2_index){
    	RemapPcieTable(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
    	RiserCardIioStringModify(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
    }
    if(SXB3_index){
       	RemapPcieTable(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
       	RiserCardIioStringModify(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
    }
// hide IIO and VMD port
    DEBUG((-1, "Intel_RC_IIO_HideItem = %lx.\n", Intel_RC_IIO_HideItem));
    for(x = 0; x < sizeof(STR_IIO_Table)/sizeof(UINT16); x++){
        if(!(RShiftU64(Intel_RC_IIO_HideItem, x) & BIT0)){
            DEBUG((-1, "    Socket Configuration, STR_IIO_Table[%d]=0x%04x\n", x, STR_IIO_Table[x]));
            DEBUG((-1, "    Socket Configuration, STR_VMD_Table[%d]=0x%04x\n", x, STR_VMD_Table[x]));
            if (STR_IIO_Table[x] != 0) SetupModifyProtocol->Add2HideList("Socket Configuration", STR_IIO_Table[x]);
            if (STR_VMD_Table[x] != 0) SetupModifyProtocol->Add2HideList("Socket Configuration", STR_VMD_Table[x]);
        }
    }
    DEBUG((-1, "SmcRiserCardIioSetup end.\n"));
    return EFI_SUCCESS;
}

