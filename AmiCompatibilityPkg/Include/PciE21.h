//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/BIN/Core/Include/PCIE21.h 3     4/05/11 1:38p Yakovlevs $Revision: 6 $
//
// $Date: 4/05/11 1:38p $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  PCIE21.h
//
// Description:	PCI-E 2.0/2.1 Declarations.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef _PCI_EXPRESS20_H
#define _PCI_EXPRESS20_H

#ifdef __cplusplus
extern "C" {
#endif
/****** DO NOT WRITE ABOVE THIS LINE *******/

#pragma pack(push,1)
//////////////////////////////////////////////////////////////////////
//Pci Express Gen 2.0 Registers And Definitions
//////////////////////////////////////////////////////////////////////
//PCI Express Capability version
#define PCIE_CAP_VER1               1
#define PCIE_CAP_VER2               2


//New Dev/Port type defined in PCIe v.2 spec. 
#define	PCIE_TYPE_RC_INTEGR_EP      9
#define	PCIE_TYPE_RC_EVT_COLLECT    0xa

// Extended capabilities IDs                      
#define PCIE_CAP2_RANGE_A       0x1 // Range A supported (50us - 10ms)
#define PCIE_CAP2_RANGE_B       0x2 // Range B supported (10ms - 250ms)
#define PCIE_CAP2_RANGE_C       0x4 // Range C supported (250ms - 4s)
#define PCIE_CAP2_RANGE_D       0x8 // Range D supported (4s-64s)

//////////////////////////////////////////////////////////////////////
// PCI Express GEN 2 Device Register Block 2
//////////////////////////////////////////////////////////////////////

//Pci Express Device Capabilities Register
#define PCIE_DEV_CAP2_OFFSET		0x024
#define PCIE_DEV_CNT2_OFFSET		0x028
#define PCIE_DEV_STA2_OFFSET		0x02A

#define PCIE_CAP_TPH_SUPPORT        0x1
#define PCIE_CAP_EXT_TPH_SUPPORT    0x2

//PCI Express GEN2 Device Capability2 Register 
//Updated to PCIe v2.1
typedef union _PCIE_DEV_CAP2_REG {
	UINT32		DEV_CAP2;
	struct {
		UINT32	ComplToutRanges :   4; 	//bit 0..3
		UINT32	ComplToutDisable:   1; 	//bit 4	
        UINT32  AriForwarding   :   1;  //bit 5
        UINT32  AtomicOpRouting :   1;  //bit 6
        UINT32  AtomicOpCompl32 :   1;  //bit 7
        UINT32  AtomicOpCompl64 :   1;  //bit 8
        UINT32  Cas128Completer :   1;  //bit 9
        UINT32  NoRoPrPrPassing :   1;  //bit 10
        UINT32  LtrMechanism    :   1;  //bit 11
        UINT32  TphCompleter    :   2;  //bit 12,13
		UINT32  Reserved1       :   6;	//bit 14..19
        UINT32  ExtFmtField     :   1;  //bit 20
        UINT32  EndEndTlpPrefix :   1;  //bit 21
        UINT32  MaxEndEndPrefix :   2;  //bit 22,23
		UINT32  Reserved2       :   8;	//bit 24..31
	};
} PCIE_DEV_CAP2_REG;

//PCI Express GEN2 Device Control2 Register 
//Updated to PCIe v2.1
typedef union _PCIE_DEV_CNT2_REG {
	UINT16		DEV_CNT2;
	struct {
		UINT16	ComplToutRanges :   4; 	//bit 0..3
		UINT16	ComplToutDisable:   1; 	//bit 4	
        UINT16  AriForwarding   :   1;  //bit 5
        UINT16  AtomicOpRequer  :   1;  //bit 6
        UINT16  AtomicOpEgresBlk:   1;  //bit 7
        UINT16  IdoRequestEn    :   1;  //bit 8
        UINT16  IdoComplEn      :   1;  //bit 9
        UINT16  LtrEn           :   1;  //bit 10
		UINT16  Reserved        :   4;	//bit 11..14
        UINT16  EndEndTlpBlk    :   1;  //bit 15
	};
} PCIE_DEV_CNT2_REG;

//PCI Express GEN2 Device Status2 Register 
//Just a place holder for now....in V2

typedef union _PCIE_DEV_STA2_REG {
	UINT16		DEV_STA2;
	struct	{
        UINT16  PlaceHolder     :   16;
	};
} PCIE_DEV_STA2_REG;

//////////////////////////////////////////////////////////////////////
// PCI Express GEN 2 Link Register Block 2
//////////////////////////////////////////////////////////////////////
//#define PCIE_ASPM_DISABLE		0
//#define PCIE_ASPM_L0_ENABLE	1
//#define PCIE_ASPM_L1_ENABLE	2
//#define PCIE_ASPM_ALL_ENABLE	3

#define PCIE_LNK_CAP2_OFFSET    0x02C
#define PCIE_LNK_CNT2_OFFSET	0x030
#define PCIE_LNK_STA2_OFFSET	0x032
//Added in V2.0 
#define PCIE_LINK_SPEED_50G     0x02

//Pci Express GEN2 Link Capabilities 2 Register
//Just a place holder for now....in V2
//In V 3.0 bit definitions was added
/*typedef union _PCIE_LNK_CAP2_REG {	
	UINT32		LNK_CAP2;
	struct	{
        UINT32  Reserved        :   1;  //bit 0
        UINT32  SupLnkSpeedVect :   7;  //bit 1..7
        UINT32  CrossLnk        :   1;  //bit 8
//      UINT32  Reserved                //bit 9..31 
	};
} PCIE_LNK_CAP2_REG;
*/

//PCI Express GEN 2 Link Control2 Register 
//Updated to PCIe v2.1
typedef union _PCIE_LNK_CNT2_REG {
	UINT16		LNK_CNT2;
	struct {
		UINT16	TargetLnkSpeed  :   4;  //bit 0,1,2,3
		UINT16	EnterCompliance :   1;  //bit 4
		UINT16	HwAutoSpeedDis  :   1;  //bit 5	
		UINT16	SelDeEmphasis   :   1;  //bit 6
		UINT16	TrsMargin	    :   3;  //bit 7,8,9
		UINT16	EnterModCompl   :   1;  //bit 10
		UINT16	ComplianceSos   :   1;	//bit 11
		UINT16	ComplDeEmphasis :   4;  //bit 12, 13,14,15
	};
} PCIE_LNK_CNT2_REG;

//PCI Express GEN 2 Link Status2 Register 
//Updated to PCIe v2.1
typedef union _PCIE_LNK_STA2_REG {
	UINT16		LNK_STA2;
	struct {
		UINT16	SelDeEmphasis   :   1;  //bit 0
//PCIe Base v3.0 
        UINT16  EqComplete      :   1;  //bit 1
        UINT16  EqPhase1Ok      :   1;  //bit 2
        UINT16  EqPhase2Ok      :   1;  //bit 3
        UINT16  EqPhase3Ok      :   1;  //bit 4
        UINT16  EqRequest       :   1;  //bit 5
//PCIe Base v4.0 added bits use to be Reserved: bit 6..15
        UINT16	Reserved	    :	1;	//bit 6 RetimerDetected
        UINT16  Eq16Complete    :   1;  //bit 7
        UINT16  Eq16Phase1Ok    :   1;  //bit 8
        UINT16  Eq16Phase2Ok    :   1;  //bit 9
        UINT16  Eq16Phase3Ok    :   1;  //bit 10
        UINT16  Eq16Request     :   1;  //bit 11
        UINT16	DsCompPresence  :	3;	//bit 12..14
        UINT16	DrsMsgReceived  :	1;	//bit 15
	};
} PCIE_LNK_STA2_REG;


//////////////////////////////////////////////////////////////////////
// PCI Express Gen 2 Slot Register Block 2
//////////////////////////////////////////////////////////////////////
//Rest of registers reserved as place holders in V2.1 spec.

#define PCIE_SLT_CAP2_OFFSET		0x034
#define PCIE_SLT_CNT2_OFFSET		0x038
#define PCIE_SLT_STA2_OFFSET		0x03A


//Pci Express Slot Capabilities 2 Register
typedef union _PCIE_SLT_CAP2_REG {	
	UINT32		SLT_CAP2;
	struct	{
        UINT32 PlaceHolder      :   32;
	};
} PCIE_SLT_CAP2_REG;


//PCI Express Slot Control 2 Register 
typedef union _PCIE_SLT_CNT2_REG {
	UINT16		SLT_CNT2;
	struct {
        UINT16  PlaceHolder :   16;
	};
} PCIE_SLT_CNT2_REG;

//PCI Express Slot Status 2 Register 
typedef union _PCIE_SLT_STA2_REG {
	UINT16		SLT_STA2;
	struct {
        UINT16  PlaceHolder :   16;
	};
} PCIE_SLT_STA2_REG;


//////////////////////////////////////////////////////////////////////
//PCI Express Extended Capability ID added in V 2.0 and V 2.1 Spec
//////////////////////////////////////////////////////////////////////
#define PCIE_CAP_ID_LNK_DECL            0x0005  //Link Declaration Cap (Internal Link)
#define PCIE_CAP_ID_RC_ECEPA            0x0007  //Root Complex Event Collector End Point Association
#define PCIE_CAP_ID_MFVCH   			0x0008  //Multi Finction Virtual Channel    
#define PCIE_CAP_ID_VIRTUAL_CH2			0x0009  //Another Cap for VC   
#define PCIE_CAP_ID_VENDOR_SPEC         0x000B  //Vendor-Specific header

#define PCIE_CAP_ID_ACS                 0x000D  //Access Controll Services Cap.       
#define PCIE_CAP_ID_ATS                 0x000F  //Address translation Services

#define PCIE_CAP_ID_RESIZ_BAR           0x0015  //Resizable BAR Capability

//PCIE_CAP_ID_ARI defined in PCIe.h

//////////////////////////////////////////////////////////////////////
// Access Control Services (ACS) Extended Capability Structure
//////////////////////////////////////////////////////////////////////
#define PCIE_ACS_CAP_OFFSET		    0x004
#define PCIE_ACS_CNT_OFFSET		    0x006
#define PCIE_ACS_EVECT_OFFSET		0x008


//ACS Capability register
typedef union _PCIE_ACS_CAP_REG {
	UINT16		ACS_CAP;
	struct {
        UINT16  SrcValid_V      :   1;  //bit 0
        UINT16  TranslBlk_B     :   1;  //bit 1
        UINT16  P2PReqRedir_R   :   1;  //bit 2
        UINT16  P2PComplRedir_C :   1;  //bit 3
        UINT16  UpForwarding_U  :   1;  //bit 4
        UINT16  P2PEgressCtrl_E :   1;  //bit 5
        UINT16  DirTransP2P_T   :   1;  //bit 6
        UINT16  Reserved        :   1;  //bit 7
        UINT16  CtrlVectSize    :   8;  //bit 8..15
	};
} PCIE_ACS_CAP_REG;

//ACS Control Register.
typedef union _PCIE_ACS_CNT_REG {
	UINT16		ACS_CNT;
	struct {
        UINT16  SrcValid_V      :   1;  //bit 0
        UINT16  TranslBlk_B     :   1;  //bit 1
        UINT16  P2PReqRedir_R   :   1;  //bit 2
        UINT16  P2PComplRedir_C :   1;  //bit 3
        UINT16  UpForwarding_U  :   1;  //bit 4
        UINT16  P2PEgressCtrl_E :   1;  //bit 5
        UINT16  DirTransP2P_T   :   1;  //bit 6
        UINT16  Reserved        :   9;  //bit 7..15
	};
} PCIE_ACS_CNT_REG;


//////////////////////////////////////////////////////////////////////
// Resizable BAR Extended Capability Structure CAP_ID == 0x0015
//////////////////////////////////////////////////////////////////////

#define PCIE_RESIZ_BAR_CAP_OFFSET	0x004
#define PCIE_RESIZ_BAR_CNT_OFFSET	0x008

//Value for Resizable BAR Control Register
typedef enum {
    lenBar1M    =0,
    lenBar2M,       //1
    lenBar4M,       //2
    lenBar8M,       //3
    lenBar16M,      //4
    lenBar32M,      //5
    lenBar64M,      //6
    lenBar128M,     //7
    lenBar256M,     //8
    lenBar512M,     //9
    lenBar1G,       //10
    lenBar2G,       //11
    lenBar4G,       //12
    lenBar8G,       //13
    lenBar16G,      //14
    lenBar32G,      //15
    lenBar64G,      //16
    lenBar128G,     //17
    lenBar256G,     //18
    lenBar512G,     //19
    lenBarMaxLen    //20
} PCIE_BAR_LENGTH;
 


//Resizable BAR Capability Register.
typedef union _PCIE_RESIZ_BAR_CAP_REG {
    UINT32  RESIZ_BAR_CAP;
    struct {
        UINT8   Reserved1       : 4;    //bit 0..3
        UINT8   BarSize1M       : 1;    //bit 4
        UINT8   BarSize2M       : 1;    //bit 5
        UINT8   BarSize4M       : 1;    //bit 6
        UINT8   BarSize8M       : 1;    //bit 7
        UINT8   BarSize16M      : 1;    //bit 8
        UINT8   BarSize32M      : 1;    //bit 9
        UINT8   BarSize64M      : 1;    //bit 10
        UINT8   BarSize128M     : 1;    //bit 11
        UINT8   BarSize256M     : 1;    //bit 12
        UINT8   BarSize512M     : 1;    //bit 13
        UINT8   BarSize1G       : 1;    //bit 14
        UINT8   BarSize2G       : 1;    //bit 15
        UINT8   BarSize4G       : 1;    //bit 16
        UINT8   BarSize8G       : 1;    //bit 17
        UINT8   BarSize16G      : 1;    //bit 18
        UINT8   BarSize32G      : 1;    //bit 19
        UINT8   BarSize64G      : 1;    //bit 20
        UINT8   BarSize128G     : 1;    //bit 21
        UINT8   BarSize256G     : 1;    //bit 22
        UINT8   BarSize512G     : 1;    //bit 23
    };
} PCIE_RESIZ_BAR_CAP_REG;

//Resizable BAR Control Register.
typedef union _PCIE_RESIZ_BAR_CNT_REG {
    UINT16  RESIZ_BAR_CNT;
    struct {
        UINT16  BarIndex        : 3;    //bit 0..2
        UINT16  Reserved1       : 2;    //bit 3,4
        UINT16  NumBars         : 3;    //bit 5..7 valid values 1..6 applicablr only for first RESIZ_BAR_CNT_REG for all other -Reserved.
        UINT16  BarSize         : 5;    //bit 8..12
        UINT16  Reserved2       : 3;    //bit 13..15
    };
} PCIE_RESIZ_BAR_CNT_REG;


//////////////////////////////////////////////////////////////////////
// Alternative Routing ID (ARI) Extended Capability Structure CAP_ID == 0x0015
//////////////////////////////////////////////////////////////////////

#define PCIE_ARI_CAP_OFFSET	    0x004
#define PCIE_ARI_CNT_OFFSET	    0x006

//ARI Capability Register.
typedef union _PCIE_ARI_CAP_REG {
    UINT16  ARI_CAP;
    struct {
        UINT16 MFVCGroup       : 1;    //bit 0 MFVC Function Groups Capability (M)
        UINT16 ACSGroup        : 1;    //bit 1 ACS Function Groups Capability (A)
        UINT16 Reserved        : 6;    //bit 2..7
        UINT16 NextFuncN       : 8;
    };
}PCIE_ARI_CAP_REG;


//ARI Control Register.
typedef union _PCIE_ARI_CNT_REG {
    UINT16  ARI_CNT;
    struct {
        UINT16  MFVCGroup       : 1;    //bit 0 MFVC Function Groups Capability (M)
        UINT16  ACSGroup        : 1;    //bit 1 ACS Function Groups Capability (A)
        UINT16  Reserved1       : 2;    //bit 2..3
        UINT16  FuncGroup       : 3;    //bit 4..6
        UINT16  Reserved2       : 9;    //bit 7..15
    };
}PCIE_ARI_CNT_REG;

//////////////////////////////////////////////////////////////////////
// Address Translation Services (ATS) Extended Capability Structure CAP_ID == 0x000F
//////////////////////////////////////////////////////////////////////

//ATS Capability Register.
typedef union _PCIE_ATS_CAP_REG {
    UINT16  ATS_CAP;
    struct {
        UINT16  InvQueue        : 5;    //bit 0..4 
        UINT16  AlignReq        : 1;    //bit 5 
        UINT16  Reserved        : 10;    //bit 2..3
    };
}PCIE_ATS_CAP_REG;


//ATS Control Register.
typedef union _PCIE_ATS_CNT_REG {
    UINT16  ATS_CNT;
    struct {
        UINT16  InvQueue        : 5;    //bit 0..4 
        UINT16  AlignReq        : 1;    //bit 5 
        UINT16  Reserved        : 10;    //bit 2..3
    };
}PCIE_ATS_CNT_REG;


//////////////////////////////////////////////////////////////////////
// Root Complex Internal Link Control Extended
// Capability Header CAP_ID == 0x0006
//////////////////////////////////////////////////////////////////////
#define PCIE_CAP_ID_RC_INT_LNK		0x0006
//Pci Express RC Link Capabilities Register
typedef union _PCIE_RC_LNK_CAP_REG {	
	UINT32		RC_LNK_CAP;
	struct	{
		UINT32	MaxLnkSpeed	:	4; 	//bit 0..3
		UINT32	MaxLnkWidth	:	6; 	//bit 4,5,6,7,8,9	
		UINT32	AspmSupport	:	2; 	//bit 10,11
		UINT32	ExL0Latency	:	3; 	//bit 12,13,14
		UINT32	ExL1Latency	:	3;	//bit 15,16,17
	};
} PCIE_RC_LNK_CAP_REG;

//PCI Express RC Link Control Register 
typedef union _PCIE_RC_LNK_CNT_REG {
	UINT16		LNK_CNT;
	struct {
		UINT16	AspmControl	:	2; 	//bit 0,1
		UINT16	Reserved0	:	5;	//bit 2,3,4,5,6
		UINT16	ExtSynch	:	1;	//bit 7
	};
} PCIE_RC_LNK_CNT_REG;

//PCI Express RC Link Status Register 
typedef union _PCIE_RC_LNK_STA_REG {
	UINT16		LNK_STA;
	struct {
		UINT16	LnkSpeed	:	4; 	//bit 0,1,2,3
		UINT16	LnkWidth	:	6;	//bit 4,5,6,7,8,9
	};
} PCIE_RC_LNK_STA_REG;

//////////////////////////////////////////////////////////////////////
/// Root Complex Root Bridge Extended
/// Capability Header CAP_ID == 0x000A
//////////////////////////////////////////////////////////////////////
#define PCIE_CAP_ID_RCRB                0x000A  //RCRB Header    
#define PCIE_RCRB_VID_DID_OFFSET		0x4
#define PCIE_RCRB_CAP_OFFSET			0x8
#define PCIE_RCRB_CNT_OFFSET			0xC

//32 bit RCRB VID/DID RO register
typedef union _PCIE_RCRB_ID_REG {
	UINT32	DEV_VEN_ID;
	struct {
		UINT16		VenId;
		UINT16		DevId;
	};
} PCIE_RCRB_ID_REG;

//Pci Express RCRB Capabilities Register
typedef union _PCIE_RCRB_CAP_REG {	
	UINT32		RCRB_CAP;
	struct	{
		UINT32	CrsSwVisib	:	1; 	//bit 0
		UINT32	Reserved	:	31; //bit 1...31	
	};
} PCIE_RCRB_CAP_REG;

//PCI Express RCRB Control Register 
typedef union _PCIE_RCRB_CNT_REG {
	UINT32		RCRB_CNT;
	struct {
		UINT32	CrsSwVisib	:	1; 	//bit 0
		UINT32	Reserved	:	31;	//bit 1..31
	};
} PCIE_RCRB_CNT_REG;

//////////////////////////////////////////////////////////////////////
/// Multicast Extended Capability
/// Header CAP_ID == 0x0012
//////////////////////////////////////////////////////////////////////
/*
#define PCIE_CAP_ID_MULTICAST           0x0012  //Multicast Extended Capability 

#define PCIE_MULTICAST_CAP_OFFSET		0x04
#define PCIE_MULTICAST_CNT_OFFSET		0x06
#define PCIE_MULTICAST_BAR_OFFSET		0x04
#define PCIE_MULTICAST_RCV_OFFSET		0x10
#define PCIE_MULTICAST_BLK_ALL_OFFSET	0x18
#define PCIE_MULTICAST_BLK_UNT_OFFSET	0x20
#define PCIE_MULTICAST_OVR_BAR_OFFSET	0x28

*/








#pragma pack(pop)



/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
