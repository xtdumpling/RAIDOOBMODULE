//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.18
//    Bug Fix: Update Seg/Bus/Dev numbers to non-transfered values for PLX detection.
//    Reason:     
//    Auditor: Jason Hsu
//    Date:    Aug/10/2017
//
//  Rev. 1.09
//    Bug Fix:  Support default string for SPG.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Apr/18/2017
//
// Rev. 1.08
//   Reason:    Support riser card M.2 slot.
//   Auditro:   Kasber Chen
//   Date:      Mar/02/2017
//
//  Rev. 1.07
//    Reason:   Support OEM type 130 for SXB slots.
//    Auditro:  Kasber Chen
//    Date:     Jan/25/2017
//
//  Rev. 1.06
//    Bug Fix:  Dynamic create type 41 for ultra IO.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Oct/14/2016
//
//  Rev. 1.05
//    Bug Fix:  Fix Bus dynamic change issue.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Sep/06/2016
//
//  Rev. 1.04
//    Bug Fix:  Support 6 NVMe
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/14/2016
//
//  Rev. 1.03
//    Bug Fix:  Fix all riser card slot oprom items can't show in setup.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Jul/04/2016
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
//    Auditor:	Kasber Chen
//    Date:	04/13/2016      
//
//**************************************************************************//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <AmiLib.h>
#include "token.h"
#include <Protocol/AmiSmbios.h>
#include "SmcLib.h"
#include "SmcRiserCardCommon.h"
#include "SmcRiserCard.h"
#include "SmcRiserCardSmbios.h"

extern	UINT8	SXB1_index, SXB2_index, SXB3_index;
extern	RC_DATA_Table	RC_Table[];
extern	UINTN	SlotPcdNumTbl[], LanPcdNumTbl[], NVMePcdNumTbl[];
extern	UINT8	SlotPcdNumTblSize, LanPcdNumTblSize, NVMePcdNumTblSize;

AMI_SMBIOS_PROTOCOL	*SmbiosProtocol;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : CreateRCSmbios
//
// Description : Create SMBIOS type 9 and OEM type 130.                
//
// Parameters  : Board_Pcie_Table[]         MB riser card slot pcie port table.
//               RC_Table_index             riser card index in database 
//
// Returns     : None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
CreateRCSmbios(
    IN	UINT32	Board_Pcie_Table[],
    IN	UINT8	Board_Pcie_Table_Size,
    IN	UINT8	RC_Table_index,
    IN	UINT16	SmcSlotID,
    IN	OUT	UINT8	*SkipPcdNum
)
{
    UINT8	i, *TempBuffer/*, PcdTblIndex*/;
    UINT8	x = 0, Lan_Ins = 0, LAN_Chip_Num = 0, Device_Count = 0, Str_Count = 1;
    UINT8       PCIE_Bif[SXB_Max_Device], Dev_Type[SXB_Max_Device], Dev_Num[SXB_Max_Device];
    UINT8       Slot_Type[SXB_Max_Device], Slot_Name[SXB_Max_Device];
    UINT8       Mux_Add[SXB_Max_Device], Mux_Channel[SXB_Max_Device];
    UINT16	    TempBufferSize;
    EFI_STATUS	Status = EFI_SUCCESS;

    DEBUG((-1, "CreateRCSmbios Entry.\n"));
    gBS->AllocatePool(EfiBootServicesData, 255, &TempBuffer);
    for(i = 0; i < Board_Pcie_Table_Size; i++){
        if((Board_Pcie_Table[i] == 0xffffffff) || (!RC_Table[RC_Table_index].SLOT_NUM[i]) ||
                (RC_Table[RC_Table_index].SLOT_NUM[i] == 0xff))
            continue;      //skip 0x00 and 0xff

        switch(RC_Table[RC_Table_index].SLOT_NUM[i] & 0xf0){
        case RC_SLOT:	//smbios type 9 information
            gBS->SetMem(TempBuffer, 255, 0x00);				//clear buffer
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Type = 0x09;
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Length = 0x11;
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Handle = 0xffff;
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDesig = 0x01;	//fixed.
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotChar_1 = 0x04;	//3.3v
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotChar_2 = 0x01;	//PME#
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = 0x00;
            //Set SLOT TYPE
            switch(RC_Table[RC_Table_index].SLOT_TYPE[i]){
            case x16:
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotType = 0xB6;	//PcieGen3X16
                break;
            case x8:
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotType = 0xB5;	//PcieGen3X8
                break;
            case x4:
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotType = 0xB4;	//PcieGen3X4
                break;
            }
            //choose by slot bifurcate
            switch(RC_Table[RC_Table_index].RC_Bifurcate[i]){
            case x16:
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = 0x0D;	//16X
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotLength = 0x04;	//LengthLong
                break;
            case x8:
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = 0x0B;	//8X
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotLength = 0x03;	//LengthShort
                break;
            case x4:
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = 0x0A;	//4X
                ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotLength = 0x03;	//LengthShort
                break;
            }
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotID = SmcSlotID | RC_Table[RC_Table_index].SLOT_NUM[i];
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SegGroupNumber = (UINT16)(Board_Pcie_Table[i] >> 16);
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->BusNumber = (UINT8)(Board_Pcie_Table[i] >> 8);
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = (UINT8)Board_Pcie_Table[i];

            //riser card string
            if(!AsciiStrCmp(RC_Table[RC_Table_index].ID_NAME.RC_NAME, "DEFAULT_STR")){
                Sprintf((UINT8*)&TempBuffer[0x11], "%s", RC_Table[RC_Table_index].RC_SLOT_Str[RC_Table[RC_Table_index].SLOT_NUM[i] - 1]);
            } else {
                Sprintf((UINT8*)&TempBuffer[0x11], "%s %s", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                        RC_Table[RC_Table_index].RC_SLOT_Str[RC_Table[RC_Table_index].SLOT_NUM[i] - 1]);
            }

            TempBufferSize = sizeof(SMBIOS_SYSTEM_SLOTS_INFO) + (UINT16)AsciiStrLen((UINT8*)&TempBuffer[0x11]) + 2;
            SmbiosProtocol->SmbiosAddStructure(TempBuffer, TempBufferSize);	//Add SMBIOS structure
            break;
        case RC_LAN:	//smbios type 41 information
	    gBS->SetMem(TempBuffer, 100, 0x00);				//clear buffer
            for(x = 0; x < (RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f); x++){
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->StructureType.Type = 0x29;  //type 41
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->StructureType.Length = 0x0b;
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->StructureType.Handle = 0xffff;
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->RefDesignation = 1;
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->DeviceType = 0x05;  //ethernet
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->DeviceTypeInstance = Lan_Ins + 1;
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->SegGroupNumber = (UINT16)(Board_Pcie_Table[i] >> 16);
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->BusNumber = (UINT8)(Board_Pcie_Table[i] >> 8);
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)TempBuffer)->DevFuncNumber = (UINT8)Board_Pcie_Table[i];
                Sprintf((UINT8*)&TempBuffer[0x0b], "%s #%x", RC_Table[RC_Table_index].RC_LAN_Str[LAN_Chip_Num], Lan_Ins + 1);
                DEBUG((-1, "%s #%x\n", RC_Table[RC_Table_index].RC_LAN_Str[LAN_Chip_Num], Lan_Ins + 1));
                TempBufferSize = sizeof(SMBIOS_ONBOARD_DEV_EXT_INFO) + (UINT16)AsciiStrLen((UINT8*)&TempBuffer[0x0b]) + 2;
                SmbiosProtocol->SmbiosAddStructure(TempBuffer, TempBufferSize);	//Add SMBIOS structure    
                Lan_Ins++;
            }
            LAN_Chip_Num++;	//point to next LAN chip string.
            break;
        case RC_SAS:
            break;
        case RC_NVME:
            break;
        case RC_PLX_SLOT:
/*            gBS->SetMem(TempBuffer, 100, 0x00);				//clear buffer
            for(PcdTblIndex = 0; PcdTblIndex <= SlotPcdNumTblSize; PcdTblIndex++){
                if((LibPcdGet32(SlotPcdNumTbl[PcdTblIndex]) != 0x88888888) &&
                    (LibPcdGet32(SlotPcdNumTbl[PcdTblIndex]) & 0xff00) &&
                    ((LibPcdGet32(SlotPcdNumTbl[PcdTblIndex]) & 0xff00) != 0x8000)){
                    if(x == *SkipPcdNum){
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Type = 0x09;
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Length = 0x11;
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Handle = 0xffff;
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDesig = 0x01;	//fixed.
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotChar_1 = 0x04;	//3.3v
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotChar_2 = 0x01;	//PME#
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = 0x00;
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotType = 0xB6;	//PcieGen3X16
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = 0x0D;	//16X
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotLength = 0x04;	//LengthLong
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotID = SmcSlotID | RC_Table[RC_Table_index].SLOT_NUM[i];
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->BusNumber = (UINT8)(LibPcdGet16(SlotPcdNumTbl[PcdTblIndex]) >> 8);
                        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = (UINT8)(LibPcdGet16(SlotPcdNumTbl[PcdTblIndex]) >> 8);

                        Sprintf(&TempBuffer[0x11], "%s SLOT%d PCI-E", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                            RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f);

                        TempBufferSize = sizeof(SMBIOS_SYSTEM_SLOTS_INFO) + (UINT16)AsciiStrLen((UINT8*)&TempBuffer[0x11]) + 2;
                        SmbiosProtocol->SmbiosAddStructure(TempBuffer, TempBufferSize);	//Add SMBIOS structure
                        (*SkipPcdNum)++;
                        x = 0;
                        break;
                    }
                    x++;
                }
            }*/
            break;
        case RC_M2:   //smbios type 9 information
            gBS->SetMem(TempBuffer, 255, 0x00);                         //clear buffer
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Type = 0x09;
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Length = 0x11;
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->StructureType.Handle = 0xffff;
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDesig = 0x01;  //fixed.
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotChar_1 = 0x04; //3.3v
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotChar_2 = 0x01; //PME#
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = 0x00;
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotType = 0x16;
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = 0x0A;       //4X
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotLength = 0x03;     //LengthShort
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotID = SmcSlotID | (RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f);
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SegGroupNumber = (UINT16)(Board_Pcie_Table[i] >> 16);
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->BusNumber = (UINT8)(Board_Pcie_Table[i] >> 8);
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = (UINT8)Board_Pcie_Table[i];

            //riser card string
            Sprintf((UINT8*)&TempBuffer[0x11], "%s %s", RC_Table[RC_Table_index].ID_NAME.RC_NAME,
                    RC_Table[RC_Table_index].RC_M2_Str[(RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f) - 1]);

            TempBufferSize = sizeof(SMBIOS_SYSTEM_SLOTS_INFO) + (UINT16)AsciiStrLen((UINT8*)&TempBuffer[0x11]) + 2;
            SmbiosProtocol->SmbiosAddStructure(TempBuffer, TempBufferSize);     //Add SMBIOS structure
            break;
        }
    }
//create oem type 130
    DEBUG((-1, "Create OEM type 130.\n"));
    LAN_Chip_Num = 0;
    gBS->SetMem(TempBuffer, 255, 0);
    for(i = 0; i < SXB_Max_Device; i++){
        if((!RC_Table[RC_Table_index].RC_Bifurcate[i]) ||
                (RC_Table[RC_Table_index].RC_Bifurcate[i] == 0xff))
            continue;           //skip 0x00 and 0xff

        ((RC_SMBIOS_INFO*)TempBuffer)->Total_Lane += RC_Table[RC_Table_index].RC_Bifurcate[i];
        PCIE_Bif[Device_Count] = RC_Table[RC_Table_index].RC_Bifurcate[i];
        Dev_Type[Device_Count] = RC_Table[RC_Table_index].SLOT_NUM[i] >> 4;
        Dev_Num[Device_Count] = RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f;
        Slot_Type[Device_Count] = RC_Table[RC_Table_index].SLOT_TYPE[i];
        if(RC_Table[RC_Table_index].SLOT_NUM[i]){
            Str_Count++;
            Slot_Name[Device_Count] = Str_Count;
        } else {
            Slot_Name[Device_Count] = 0;
        }
        Mux_Add[Device_Count] = RC_Table[RC_Table_index].Mux_Add[i];
        Mux_Channel[Device_Count] = RC_Table[RC_Table_index].Mux_Ch[i];

        Device_Count++;
    }
    DEBUG((-1, "Device_Count = %x.\n", Device_Count));
    TempBufferSize =  Device_Count * sizeof(RC_DEVICE_GROUP) + sizeof(RC_SMBIOS_INFO);
    DEBUG((-1, "TempBufferSize = %x, x = %x.\n", TempBufferSize, x));

    ((RC_SMBIOS_INFO*)TempBuffer)->StructureType.Type = 130;  //type 130
    ((RC_SMBIOS_INFO*)TempBuffer)->StructureType.Length = (UINT8)TempBufferSize;
    ((RC_SMBIOS_INFO*)TempBuffer)->StructureType.Handle = 0xffff;
    ((RC_SMBIOS_INFO*)TempBuffer)->SXB_Location = (UINT8)(SmcSlotID >> 8);
    ((RC_SMBIOS_INFO*)TempBuffer)->Card_ID = 0;         //reserve for riser card FRU
    ((RC_SMBIOS_INFO*)TempBuffer)->Card_Ver = 0;        //reserve for riser card FRU
    ((RC_SMBIOS_INFO*)TempBuffer)->Card_Name = 0x01;    //card name is fixed 0x01

    AsciiStrCpy(&TempBuffer[TempBufferSize], RC_Table[RC_Table_index].ID_NAME.RC_NAME);
    TempBufferSize += (UINT16)AsciiStrSize(&TempBuffer[TempBufferSize]);
    for(i = 0; i < SXB_Max_Device; i++){
        if(!RC_Table[RC_Table_index].SLOT_NUM[i] || (RC_Table[RC_Table_index].SLOT_NUM[i] == 0xff))
            continue;

        switch(RC_Table[RC_Table_index].SLOT_NUM[i] & 0xf0){
        case RC_SLOT:
            AsciiStrCpy(&TempBuffer[TempBufferSize], RC_Table[RC_Table_index].RC_SLOT_Str[RC_Table[RC_Table_index].SLOT_NUM[i] - 1]);
            break;
        case RC_LAN:
            AsciiStrCpy(&TempBuffer[TempBufferSize], RC_Table[RC_Table_index].RC_LAN_Str[LAN_Chip_Num]);
            LAN_Chip_Num++;
            break;
        case RC_SAS:
            break;
        case RC_NVME:
            Sprintf(&TempBuffer[TempBufferSize], "NVME%d", RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f);
            break;
        case RC_M2:
            AsciiStrCpy(&TempBuffer[TempBufferSize], RC_Table[RC_Table_index].RC_M2_Str[(RC_Table[RC_Table_index].SLOT_NUM[i] & 0x0f) - 1]);
            break;
        }
        DEBUG((-1, "AsciiStrSize = %x.\n", (UINT16)AsciiStrSize(&TempBuffer[TempBufferSize])));
        TempBufferSize += (UINT16)AsciiStrSize(&TempBuffer[TempBufferSize]);
    }
    TempBufferSize++;   //add a 0x00 for end of type

    x = sizeof(RC_SMBIOS_INFO);
    ((RC_SMBIOS_INFO*)TempBuffer)->Dev_Group.PCIE_Bif_P = x;
    MemCpy(&TempBuffer[x], PCIE_Bif, Device_Count);

    x += Device_Count;
    ((RC_SMBIOS_INFO*)TempBuffer)->Dev_Group.Device_Type_P = x;
    MemCpy(&TempBuffer[x], Dev_Type, Device_Count);

    x += Device_Count;
    ((RC_SMBIOS_INFO*)TempBuffer)->Dev_Group.Device_Num_P = x;
    MemCpy(&TempBuffer[x], Dev_Num, Device_Count);

    x += Device_Count;
    ((RC_SMBIOS_INFO*)TempBuffer)->Dev_Group.Slot_Type_P = x;
    MemCpy(&TempBuffer[x], Slot_Type, Device_Count);

    x += Device_Count;
    ((RC_SMBIOS_INFO*)TempBuffer)->Dev_Group.Slot_Name_P = x;
    MemCpy(&TempBuffer[x], Slot_Name, Device_Count);

    x += Device_Count;
    ((RC_SMBIOS_INFO*)TempBuffer)->Dev_Group.MUX_Address_P = x;
    MemCpy(&TempBuffer[x], Mux_Add, Device_Count);

    x += Device_Count;
    ((RC_SMBIOS_INFO*)TempBuffer)->Dev_Group.MUX_Channel_P = x;
    MemCpy(&TempBuffer[x], Mux_Channel, Device_Count);
    
    DEBUG((-1, "TempBufferSize = %x.\n", TempBufferSize));
    Status = SmbiosProtocol->SmbiosAddStructure(TempBuffer, TempBufferSize);     //Add SMBIOS structure

    gBS->FreePool(TempBuffer);
    DEBUG((-1, "CreateRCSmbios end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : RiserCardSmBiosUpdate
//
// Description : Create SMBIOS type 9 data to system.                
//
// Parameters  :  
//
// Returns     : None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
RiserCardSmBiosUpdate(void)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT32	SXB1_Tbl[] = BOARD_PCIE_PORT_SXB1;
    UINT32	SXB2_Tbl[] = BOARD_PCIE_PORT_SXB2;
    UINT32	SXB3_Tbl[] = BOARD_PCIE_PORT_SXB3;
    UINT16	SmcSlotID = 0;
    UINT8	SkipPcdNum = 0;

    DEBUG((-1, "RiserCardSmBiosUpdate Entry.\n"));

    Status = gBS->LocateProtocol(
		    &gAmiSmbiosProtocolGuid,
		    NULL,
		    &SmbiosProtocol);

    if(Status)	return;

//Create left riser card smbios type9
    if(SXB1_index){
	DEBUG((-1, "Create WIO or GIO left riser card SMBIOS.\n"));
	RemapPcieTable(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32), SXB1_index);
#ifdef SXB1_SLOTID_OVERRIDE
	    SmcSlotID = SXB1_SLOTID_OVERRIDE << 8;
#else
	    SmcSlotID = SXB1 << 8;
#endif
    	CreateRCSmbios(SXB1_Tbl, sizeof(SXB1_Tbl)/sizeof(UINT32),
    			SXB1_index, SmcSlotID, &SkipPcdNum);
    }
//Create right riser card smbios type9
    if(SXB2_index){
	DEBUG((-1, "Create WIO or GIO right riser card SMBIOS.\n"));
	RemapPcieTable(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32), SXB2_index);
#ifdef SXB2_SLOTID_OVERRIDE
	    SmcSlotID = SXB2_SLOTID_OVERRIDE << 8;
#else
    	    SmcSlotID = SXB2 << 8;	// WIO right riser card
#endif
    	CreateRCSmbios(SXB2_Tbl, sizeof(SXB2_Tbl)/sizeof(UINT32),
    			SXB2_index, SmcSlotID, &SkipPcdNum);
    }
//Create ultra riser card smbios type9
    if(SXB3_index){
	DEBUG((-1, "Create ultra or GIO mid riser card SMBIOS.\n"));
	RemapPcieTable(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32), SXB3_index);
#ifdef SXB3_SLOTID_OVERRIDE
	    SmcSlotID = SXB3_SLOTID_OVERRIDE << 8;
#else
    	    SmcSlotID = SXB3 << 8;		// ultra riser card
#endif
    	CreateRCSmbios(SXB3_Tbl, sizeof(SXB3_Tbl)/sizeof(UINT32),
    			SXB3_index, SmcSlotID, &SkipPcdNum);
    }

    DEBUG((-1, "RiserCardSmBiosUpdate end.\n"));
}
