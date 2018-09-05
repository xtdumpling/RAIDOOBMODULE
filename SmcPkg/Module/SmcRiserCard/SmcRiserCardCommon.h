//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
// Rev. 1.07
//   Reason:	Add riser card MUX channel change.
//   Auditro:	Kasber Chen
//   Date:      Feb/21/2017
//
//  Rev. 1.06
//    Reason:	Support OEM type 130 for SXB slots.
//    Auditro:	Kasber Chen
//    Date:      Jan/25/2017
//
//  Rev. 1.05
//    Reason:	Correct port D device and function number.
//    Auditro:	Kasber Chen
//    Date:     Jan/12/2017
//
//  Rev. 1.04
//    Bug Fix:  Add riser card redriver function.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/10/2017
//
//  Rev. 1.03
//    Bug Fixed:  Support AOC NVMe card in riser card slots.
//    Reason:     
//    Auditor:    Kasber Chen
//    Date:       Nov/16/2016
//
//  Rev. 1.02
//    Bug Fixed:  Add LAN string for ultra IO riser card.
//    Reason:     
//    Auditor:    Kasber Chen
//    Date:       Oct/19/2016
//
//  Rev. 1.01
//    Bug Fixed:  Fix Bus dynamic change issue.
//    Reason:     
//    Auditor:    Kasber Chen
//    Date:       Sep/06/2016
//
//  Rev. 1.00
//    Reason:	Initialize revision.
//    Auditro:	Kasber Chen
//    Date:	04/13/2016
//
//****************************************************************************
#ifndef	_H_SmcRiserCardCommon_
#define	_H_SmcRiserCardCommon_

#include <../CpRcPkg/Library/BaseMemoryCoreLib/Chip/Skx/Include/Iio/IioRegs.h>
#include <../CpRcPkg/Library/BaseMemoryCoreLib/Chip/Skx/Include/Iio/IioConfig.h>

#ifndef x16
#define x32     32
#define x24     24
#define x20     20
#define	x16     16
#define x8      8
#define x4      4

typedef enum{
    P0PE1A = 0x00110000, P0PE1B = 0x00110008, P0PE1C = 0x00110010, P0PE1D = 0x00110018,
    P0PE2A = 0x00120000, P0PE2B = 0x00120008, P0PE2C = 0x00120010, P0PE2D = 0x00120018,
    P0PE3A = 0x00130000, P0PE3B = 0x00130008, P0PE3C = 0x00130010, P0PE3D = 0x00130018,
    P1PE0A = 0x00200000,
    P1PE1A = 0x00210000, P1PE1B = 0x00210008, P1PE1C = 0x00210010, P1PE1D = 0x00210018,
    P1PE2A = 0x00220000, P1PE2B = 0x00220008, P1PE2C = 0x00220010, P1PE2D = 0x00220018,
    P1PE3A = 0x00230000, P1PE3B = 0x00230008, P1PE3C = 0x00230010, P1PE3D = 0x00230018
}SMC_IIO_PORT;

typedef struct{
    UINT8       Address;
    UINT8       Channel;
    UINT32      BDF;
    UINT8       Bifurcate;
}SMC_SLOT_INFO;
#endif

typedef enum{
    RC_SLOT = 0x00, RC_LAN = 0x10, RC_SAS = 0x20, RC_NVME = 0x30, RC_PLX_SLOT = 0x40,
    RC_M2 = 0x50
}SMC_RC_DEV_TYPE;

typedef enum{
    ONBOARD = 0, 
    SXB1, SXB2, SXB3, SXB4, SXB5, SXB6,
    STDSLOT1 = 220,
    STDSLOT2, STDSLOT3, STDSLOT4, STDSLOT5, STDSLOT6, STDSLOT7
}SMC_RISER_LOCATION;

typedef struct{
    UINT32	PORT;
    UINT8	INDEX;
}SMC_IIO_PORT_INDEX;

typedef struct{
    UINT32	RC_ID;
    CHAR16	*L_RC_NAME;
    CHAR8	*RC_NAME;
}RC_ID_NAME;

typedef struct{
    UINT8	Reg;
    UINT8	Data;
}RC_ReD;

typedef struct{
    RC_ID_NAME  ID_NAME;
    UINT8       RC_Bifurcate[12];
    UINT8       SLOT_NUM[12];		//bit0~3: slot number, 0x10: LAN, 0x20: SAS, 0x30: NVME
    	    	    	    		//if LAN, bit0~3 is number of lan port in one chip
    	    	    	    		//if NVMe, bit0~3 is NVMe port number
    UINT8       SLOT_TYPE[12];		//slot type in type 9
    CHAR8       *RC_SLOT_Str[4];	//slot string
    CHAR8       *RC_M2_Str[4];          //M.2 slot string
    CHAR8       *RC_LAN_Str[2];         //lan device string
    UINT8       Mux_Ch[12];		//Mux on riser card, Mux channel
    UINT8       Mux_Add[12];		//Mux on riser card, Mux address
    UINT8       ReD_Add[12];		//Redriver chip address
    RC_ReD      *ReD_Data_Tbl[12];	//Redriver data table
    UINT8       ReD_Data_Tbl_S[12];	//Redriver data table size
}RC_DATA_Table;

EFI_STATUS
SmcRiserCardRedriverData(
    IN  UINT8   SXB_Index,
    IN  UINT8   RC_ReD_Index,
    IN OUT      UINT8   *RC_MUX_Address,
    IN OUT      UINT8   *RC_MUX_Channel,
    IN OUT      UINT8   *RC_ReD_Add,
    IN OUT      RC_ReD  RC_ReD_Data[],
    IN OUT      UINT8   *RC_ReD_Data_Size
);

VOID
SmcRiserCardIDUpdate(
    IN	UINT32	Board_CFG_GPIO[],
    IN	UINT8	Board_CFG_GPIO_Size,
    IN	UINT32	*temp_ID
);

UINT8
SmcRiserCardIDCmp(
    IN	UINT32	SMC_RC_ID
);

VOID Swap(
    IN	OUT	UINT32	*AAA,
    IN	OUT	UINT32	*BBB
);
VOID RemapPcieTable(
    IN	OUT	UINT32	Board_Pcie_Table[],
    IN	UINT8	Board_Pcie_Table_Size,
    IN	UINT8	RC_Table_index
);

VOID RedefineDevBDF(
    IN	UINT32	Board_Pcie_Table[],
    IN	UINT8	Board_Pcie_Table_Size,
    IN	UINT8	RC_Table_index
);

EFI_STATUS EFIAPI
SmcRiserCardBifuracate(
    IN OUT      IIO_CONFIG      *SetupData,
    IN          UINT8           DefaultIOU0[],
    IN          UINT8           DefaultIOU1[],
    IN          UINT8           DefaultIOU2[]
);

EFI_STATUS EFIAPI
SmcRiserCardSlotBDFTable(
    IN OUT      SMC_SLOT_INFO   SXB1_SLOT_INFO_Tbl[],
    IN OUT      SMC_SLOT_INFO   SXB2_SLOT_INFO_Tbl[],
    IN OUT      SMC_SLOT_INFO   SXB3_SLOT_INFO_Tbl[]
);

#endif
