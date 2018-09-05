//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**              5555 Oakbrook Pkwy, Norcross, GA 30093              **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file


 */

#ifndef _PCI_EXPRESS_H
#define _PCI_EXPRESS_H

#ifdef __cplusplus
extern "C" {
#endif
/****** DO NOT WRITE ABOVE THIS LINE *******/

//How to assemble PCI Express Address 
#define PCIE_CFG_ADDR(bus,dev,func,reg) \
  ((VOID*) (UINTN)(PCIEX_BASE_ADDRESS + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))


#ifndef PCI_CAP_ID_PCIEXP
#define PCI_CAP_ID_PCIEXP		0x10
#endif

// Extended capabilities IDs                      
#define PCIE_CAP_ID_AER      0x0001            // Advanced error reporting
#define PCIE_CAP_ID_VC       0x0002            // Virtual channel
#define PCIE_CAP_ID_DSN      0x0003            // Device serial number
#define PCIE_CAP_ID_PB       0x0004            // Power budgeting

#pragma pack(push,1)
//////////////////////////////////////////////////////////////////////
//Pci Express Standard Registers
//////////////////////////////////////////////////////////////////////

#define	PCIE_TYPE_ENDPOINT			0
#define	PCIE_TYPE_LEGACYEP			1
#define	PCIE_TYPE_ROOTPORT			4
#define	PCIE_TYPE_UPS_PORT			5
#define	PCIE_TYPE_DNS_PORT			6
#define	PCIE_TYPE_PCIE_PCI			7
#define	PCIE_TYPE_PCI_PCIE			8

#define	PCIE_MAXPL_128B				0
#define	PCIE_MAXPL_256B				1
#define	PCIE_MAXPL_512B				2
#define	PCIE_MAXPL_1024B			3
#define	PCIE_MAXPL_2048B			4
#define	PCIE_MAXPL_4096B			5

#define	PCIE_LAT0_LESS_64NS			0
#define	PCIE_LAT0_64_128NS			1
#define	PCIE_LAT0_128_256NS			2
#define	PCIE_LAT0_256_512NS			3
#define	PCIE_LAT0_512_1024NS		4
#define	PCIE_LAT0_1_2MS				5
#define	PCIE_LAT0_2_4MS				6
#define	PCIE_LAT0_MORE_4MS			7

#define	PCIE_LAT1_LESS_1MS			0
#define	PCIE_LAT1_1_2MS				1
#define	PCIE_LAT1_2_4MS				2
#define	PCIE_LAT1_4_8MS				3
#define	PCIE_LAT1_8_16MS			4
#define	PCIE_LAT1_16_32MS			5
#define	PCIE_LAT1_32_64MS			6
#define	PCIE_LAT1_MORE_64MS			7

//////////////////////////////////////////////////////////////////////
// PCI Express Capabilities Register 
//////////////////////////////////////////////////////////////////////

#define PCIE_CAP_OFFSET			2

typedef union _PCIE_PCIE_CAP_REG {
	UINT16		PCIE_CAP;
	struct {
		UINT16	CapVersion	:	4;
		UINT16	PortType	:	4;	
		UINT16	SlotImpl	:	1;	
		UINT16	IntMsgNo	:	5;
        UINT16  Undefined   :   1;
		UINT16	Reserved	:	1;
	};
} PCIE_PCIE_CAP_REG;

//////////////////////////////////////////////////////////////////////
// PCI Express Device Register Block
//////////////////////////////////////////////////////////////////////

//Pci Express Device Capabilities Register
#define PCIE_DEV_CAP_OFFSET		0x004
#define PCIE_DEV_CNT_OFFSET		0x008
#define PCIE_DEV_STA_OFFSET		0x00A

//Updated to Rev 2.0
typedef union _PCIE_DEV_CAP_REG {
	UINT32		DEV_CAP;
	struct {
		UINT32	MaxPayload	:	3; 	//bit 0..2
		UINT32	PhantomFunc	:	2; 	//bit 3..4	
		UINT32	ExtTagFld	:	1; 	//bit 5
		UINT32	EpL0Latency	:	3; 	//bit 6..8
		UINT32	EpL1Latency	:	3;	//bit 9..11
		UINT32  AttnBtn		:	1;	//bit 12 undefined in V2
		UINT32  AttnInd		:	1; 	//bit 13 undefined in V2
		UINT32  PowerInd	:	1;	//bit 14 undefined in V2
        UINT32  RoleBasedErr:   1;  //bit 15 Added in V2
        UINT32  Reserved1   :   2;  //bit 16..17
		UINT32  SlotPwrLimV	:	8;	//bit 18..25
		UINT32  SlotPwrLimS	:	2;	//bit 26..27
        UINT32  FuncResetCap:   1;  //bit 28 Added in V2
		UINT32  Reserved2	:	3;	//bit 29..31
	};
} PCIE_DEV_CAP_REG;

//PCI Express Device Control Register 
//Updated to Rev 2.0
typedef union _PCIE_DEV_CNT_REG {
	UINT16		DEV_CNT;
	struct {
		UINT16	CorrErrRep	:	1; 	//bit 0
		UINT16	NfErrRep	:	1;	//bit 1
		UINT16	FatalErrRep	:	1;	//bit 2	
		UINT16	UsupReqRep	:	1;	//bit 3
		UINT16	RelaxOrder	:	1;	//bit 4
		UINT16	MaxPayload	:	3;	//bit 5..7
		UINT16	ExtTagFld	:	1;	//bit 8
		UINT16	PhantomFunc	:	1;	//bit 9
		UINT16	AuxPwrPm	:	1;	//bit 10
		UINT16	NoSnoop		:	1;	//bit 11
		UINT16	MaxReadReq	:	3;	//bit 12..14
		UINT16	FnRstBrgRtry:	1;	//bit 15 Added in V2
//------------------
	};
} PCIE_DEV_CNT_REG;

//PCI Express Device Status Register 
//Updated to Rev 2.0 matches with 1.1
typedef union _PCIE_DEV_STA_REG {
	UINT16		DEV_STA;
	struct {
		UINT16	CorrErrRep	:	1; 	//bit 0
		UINT16	NfErrRep	:	1;	//bit 1
		UINT16	FatalErrRep	:	1;	//bit 2	
		UINT16	UsupReqRep	:	1;	//bit 3
		UINT16	AuxPower	:	1;	//bit 4
		UINT16	TrasactPend	:	1;	//bit 5
		UINT16	Reserved	:	10;	//bit 6..15
	};
} PCIE_DEV_STA_REG;

//////////////////////////////////////////////////////////////////////
// PCI Express Link Register Block
//////////////////////////////////////////////////////////////////////
#define PCIE_ASPM_DISABLE		0
#define PCIE_ASPM_L0_ENABLE		1
#define PCIE_ASPM_L1_ENABLE		2
#define PCIE_ASPM_ALL_ENABLE	3

#define PCIE_LNK_CAP_OFFSET		0x00C
#define PCIE_LNK_CNT_OFFSET		0x010
#define PCIE_LNK_STA_OFFSET		0x012

#define PCIE_LINK_SPEED_25G     0x01

#define PCIE_LINK_WIDTHx1       0x01
#define PCIE_LINK_WIDTHx2       0x02
#define PCIE_LINK_WIDTHx4       0x04
#define PCIE_LINK_WIDTHx8       0x08
#define PCIE_LINK_WIDTHx12      0x0c
#define PCIE_LINK_WIDTHx16      0x10
#define PCIE_LINK_WIDTHx32      0x20

//Pci Express Link Capabilities Register
//Updated to Rev 2.0 
typedef union _PCIE_LNK_CAP_REG {	
	UINT32		LNK_CAP;
	struct	{
		UINT32	MaxLnkSpeed	:	4; 	//bit 0..3
		UINT32	MaxLnkWidth	:	6; 	//bit 4,5,6,7,8,9	
		UINT32	AspmSupport	:	2; 	//bit 10,11
		UINT32	ExL0Latency	:	3; 	//bit 12,13,14
		UINT32	ExL1Latency	:	3;	//bit 15,16,17
        UINT32  ClockPm     :   1;  //bit 18 Added in V2
        UINT32  SurpDownErr :   1;  //bit 19 Added in V2
        UINT32  DllLnkActive:   1;  //bit 20 Added in V2
        UINT32  BandwNotify :   1;  //bit 21 Added in V2
		UINT32  Reserved	:	2;	//bit 22,23
		UINT32  PortNum		:	8; 	//bit 24,25,26,27,28,29,30,31
	};
} PCIE_LNK_CAP_REG;


//PCI Express Link Control Register 
//Updated to Rev 2.0 
typedef union _PCIE_LNK_CNT_REG {
	UINT16		LNK_CNT;
	struct {
		UINT16	AspmControl	:	2; 	//bit 0,1
		UINT16	Reserved0	:	1;	//bit 2
		UINT16	RdComplBound:	1;	//bit 3	
		UINT16	LnkDisable	:	1;	//bit 4
		UINT16	RetrainLnk	:	1;	//bit 5
		UINT16	CommonClk 	:	1;	//bit 6
		UINT16	ExtSynch	:	1;	//bit 7
		UINT16	ClockPm     :   1;  //bit 8     Added in V2
		UINT16	HwAutoWdtDis:   1;  //bit 9     Added in V2
		UINT16  BandwMgmtInt:   1;  //bit 10    Added in V2
		UINT16	AutoBandwInt:   1;  //bit 11    Added in V2
		UINT16	Reserved1	:	4;	//bit 12,13,14,15
	};
} PCIE_LNK_CNT_REG;

//PCI Express Link Status Register 
//Updated to Rev 2.0 
typedef union _PCIE_LNK_STA_REG {
	UINT16		LNK_STA;
	struct {
		UINT16	LnkSpeed	:	4; 	//bit 0,1,2,3
		UINT16	LnkWidth	:	6;	//bit 4,5,6,7,8,9
		UINT16	TrainingErr	:	1;	//bit 10    Undefined in V2
		UINT16	LnkTraining	:	1;	//bit 11    
		UINT16	CommonClk	:	1;	//bit 12
        UINT16  DllLnkActive:   1;  //bit 13     Added in V2
        UINT16  BandwMgmtSts:   1;  //bit 14     Added in V2
		UINT16	AutoBandwSts:	1;	//bit 15     Added in V2
	};
} PCIE_LNK_STA_REG;

//////////////////////////////////////////////////////////////////////
// PCI Express Slot Register Block
//////////////////////////////////////////////////////////////////////

#define PCIE_SLT_CAP_OFFSET		0x014
#define PCIE_SLT_CNT_OFFSET		0x018
#define PCIE_SLT_STA_OFFSET		0x01A


//Pci Express Slot Capabilities Register
//Updated to Rev 2.0 
typedef union _PCIE_SLT_CAP_REG {	
	UINT32		SLT_CAP;
	struct	{
		UINT32	AttnBtn		:	1; 	//bit 0
		UINT32	PwrCntler	:	1; 	//bit 1
		UINT32	MrlSensor	:	1; 	//bit 2
		UINT32	AttnInd		:	1;  //bit 3
		UINT32	PowerInd	:	1;  //bit 4
		UINT32	HpSurprise	:	1;  //bit 5
		UINT32	HpCapable	:	1;  //bit 6
		UINT32	PwrLimVal	:	8; 	//bit 7,8,9,10,11,12,13,14
		UINT32	PwrLimScale	:	2; 	//bit 15,16
		UINT32  EmInterlock	:	1;	//bit 17     Added in V2
        UINT32  NoCmdCompl  :   1;  //bit 18     Added in V2
		UINT32  PhisSlotNum	:	13;	//bit 19,20,21,22,23,24,25,26,27,28,29,30,31
	};
} PCIE_SLT_CAP_REG;

//HP Slot Indicator Settings
#define PCIE_SLT_INDICATOR_ON		1
#define PCIE_SLT_INDICATOR_BLINK	2
#define PCIE_SLT_INDICATOR_OFF		3

#define PCIE_SLT_PWR_OFF			1
#define PCIE_SLT_PWR_ON				0

//Different settings for Power Value Defaults.
//Updated to Rev 2.0 Added 
#define PCIE_SLT_PWR_MAX            0xef
#define PCIE_SLT_PWR_250W           0xf0
#define PCIE_SLT_PWR_275W           0xf1
#define PCIE_SLT_PWR_300W           0xf3


//PCI Express Slot Control Register 
//Updated to Rev 2.0 
typedef union _PCIE_SLT_CNT_REG {
	UINT16		SLT_CNT;
	struct {
		UINT16	AttnBtnPress:	1; 	//bit 0
		UINT16	PowerFaul	:	1;	//bit 1
		UINT16	MrlSensor	:	1;	//bit 2
		UINT16	PresenceDet	:	1;	//bit 3
		UINT16	CmdCompleted:	1;	//bit 4	
		UINT16	HpIntEnable	:	1;	//bit 5
		UINT16	AttnIndCnt	:	2;	//bit 6,7
		UINT16	PowerIndCnt :	2;	//bit 8,9
		UINT16	PowerOff	:	1;	//bit 10
		UINT16  InterlockCnt:	1;	//bit 11     Added in V2
        UINT16  DllStatChEn :   1;  //bit 12     Added in V2
		UINT16	Reserved	:	3;	//bit 13,14,15
	};
} PCIE_SLT_CNT_REG;

//PCI Express Slot Status Register 
//Updated to Rev 2.0 
typedef union _PCIE_SLT_STA_REG {
	UINT16		SLT_STA;
	struct {
		UINT16	AttnBtnPress:	1; 	//bit 0
		UINT16	PowerFaul	:	1;	//bit 1
		UINT16	MrlSensor	:	1;	//bit 2
		UINT16	PresenceDet	:	1;	//bit 3
		UINT16	CmdCompleted:	1;	//bit 4	
		UINT16	MrlSensOpen	:	1;	//bit 5
		UINT16	CardPresent	:	1;	//bit 6
		UINT16  InterlockOn :	1;	//bit 7     Added in V2
        UINT16  DllStateChg :   1;  //bit 8     Added in V2
		UINT16	Reserved	:	7;	//bit 9,10,11,12,13,14,15
	};
} PCIE_SLT_STA_REG;

//////////////////////////////////////////////////////////////////////
// PCI Express Root Register Block
//////////////////////////////////////////////////////////////////////

#define PCIE_ROOT_CNT_OFFSET	0x01C
#define PCIE_ROOT_CAP_OFFSET    0x01E   //was reserved - Added in V2 
#define PCIE_ROOT_STA_OFFSET	0x020

//PCI Express Root Control Register 
//Updated to Rev 2.0 
typedef union _PCIE_ROOT_CNT_REG {
	UINT16		ROOT_CNT;
	struct {
		UINT16	SysErrCorErr:	1; 	//bit 0
		UINT16	SysErrNfErr	:	1;	//bit 1
		UINT16	SysErrFatErr:	1;	//bit 2
		UINT16	PmeIntEnable:	1;	//bit 3
        UINT16  CrsSwVisib  :   1;  //bit 4     Added in V2
		UINT16	Reserved	:	11;	//bit 5..15
	};
} PCIE_ROOT_CNT_REG;

//PCI Express Root Capabilities Register
//Register was ADDED in V2 spec
typedef union _PCIE_ROOT_CAP_REG {
	UINT16		ROOT_CAP;
	struct {
        UINT16  CrsSwVisib  :   1;  //bit 0
		UINT16	Reserved	:	15;	//bit 1..15
	};
} PCIE_ROOT_CAP_REG;


//PCI Express Root Status Register 
//Updated to Rev 2.0 nothing has cahnged.
typedef union _PCIE_ROOT_STA_REG {	
	UINT32		ROOT_STA;
	struct	{
		UINT32	PmeRequesterId :	16; //bit 0..15
		UINT32	PmeAsserted	:	1; 	//bit 16
		UINT32	PmePending	:	1; 	//bit 17
		UINT32  Reserved	:	14;	//bit 18..31
	};
} PCIE_ROOT_STA_REG;


//////////////////////////////////////////////////////////////////////
//PCI Express Extended Capability Pointer structure
//////////////////////////////////////////////////////////////////////

//First one (if any) located at adderss 0x100 in ext cfg space.
typedef union _PCIE_EXT_CAP_HDR {
	UINT32		EXT_CAP_HDR;
	struct {
		UINT16		ExtCapId;
		UINT16		CapVersion	: 4;
		UINT16		NextItemPtr	: 12;
	};
}PCIE_EXT_CAP_HDR;

//PCI Express Spec Defined Capabilities ID 
#define	PCIE_CAP_ID_NO_EXT_CAPS			0x0000
#define	PCIE_CAP_ID_ADV_ERR_REP			0x0001
#define PCIE_CAP_ID_VIRTUAL_CH			0x0002
#define PCIE_CAP_ID_SERIAL_NUM			0x0003
#define PCIE_CAP_ID_POWER_BUDG			0x0004
//this one is not defined in PCI Express Base Spec 
//but ROOT COMPLEX ROOT PORT must have this Capability
#define PCIE_CAP_ID_RC_LNK_DECL 		0x0005	
//after this cap pointer follows PCI Express Link Register Block
#define PCIE_CAP_ID_INTERNAL_LNK		0x0006
#define PCIE_CAP_ID_ARI                 0x000E
#define PCIE_CAP_ID_SRIOV               0x0010

//--------------------------------------------------------------------
//PCI Express extended Capabilities Definitions 
//--------------------------------------------------------------------
//All Extended Headers Resides in Ext Cfg Space - above offset 0x100

//////////////////////////////////////////////////////////////////////
// PCI Express Virtual Cahnnel Register Block
//////////////////////////////////////////////////////////////////////

//Offsets from the beginning of corresponded Ext Cap Header
#define PCIE_PORTVC_CAP1_OFFSET		0x004 
#define PCIE_PORTVC_CAP2_OFFSET		0x008
#define PCIE_PORTVC_CNT_OFFSET 		0x00C
#define PCIE_PORTVC_STS_OFFSET 		0x00E

//PCI Express Virtual Channel Capability Register 1
typedef union _PCIE_PORTVC_CAP_REG1 {
	UINT32	PORTVC_CAP1;
	struct {
		UINT32	ExtVcCnt		: 3;	//bit 0..2
		UINT32	Reserved1		: 1;	//bit 3
		UINT32	LPExtVcCnt		: 3;	//bit 4..6
		UINT32	Reserved2		: 1;	//bit 7
		UINT32	RefClk			: 2;	//bit 8..9
		UINT32	ArbTblEntrySz	: 2;	//bit 10..11
		UINT32  Reserved		: 20;   //bit 12..31
	};
} PCIE_PORTVC_CAP_REG1;


//PCI Express Virtual Channel Capability Register 2
typedef union _PCIE_PORTVC_CAP_REG2 {
	UINT32	PORTVC_CAP2;
	struct {
		UINT8	VcArbCap;				//bit 0..7
		UINT16	Reserved;				//bit 8..23
		UINT8	ArbTblOffset;			//bit 24..31
	};
} PCIE_PORTVC_CAP_REG2;

//PCI Express Virtual Channel Control Register
typedef union _PCIE_PORTVC_CNT_REG {
	UINT16	PORTVC_CNT;
	struct {
		UINT8	LoadArbTbl		: 1;	//bit 0
		UINT8	VcArbSelect		: 3;	//bit 1..3
		UINT8	Reserved1		: 4;	//bit 4..7
		UINT8	Reserved		: 8;	//bit 8..16
	};
} PCIE_PORTVC_CNT_REG;

//PCI Express Virtual Channel Status Register
typedef union _PCIE_PORTVC_STS_REG {
	UINT16	PORTVC_STS;
	struct {
		UINT8	ArbTblSts		: 1;	//bit 0
		UINT8	Reserved1		: 7;	//bit 1..7
		UINT8	Reserved		: 8;	//bit 8..16
	};
} PCIE_PORTVC_STS_REG;

//Macro to determine offset of VC Resource Block for particular "Channel"
//If Port supports N Virtual Channels where n = 0..7 
// n = PCIE_VC_CAP_REG1.ExtVcCnt + PCIE_VC_CAP_REG1.LoPriExtVcCnt
//it must be same number of VC Resource Register Blocks
#define PCIE_VC_RES_CAP_OFFSET(Channel)	(0x010+(Channel*0x0C))
#define PCIE_VC_RES_CNT_OFFSET(Channel)	(0x014+(Channel*0x0C))
#define PCIE_VC_RES_STS_OFFSET(Channel)	(0x018+(Channel*0x0C)+2)

//PCI Express Virtual Channel Resource Capabiliy Register
typedef union _PCIE_VC_RESCAP_REG {
	UINT32	VC_RESCAP;
	struct {
		UINT8	PortArbCap		: 8;	//bit 0..7
		UINT8	Reserved1		: 6;	//bit 8..13
		UINT8	AdvPackSw		: 1;	//bit 14
		UINT8	RejectSnoop		: 1;	//bit 15
		UINT8	MaxTimeSlot		: 7;	//bit 16..22
		UINT8	Reserved2		: 1;	//bit 23
		UINT8	PortArbTbl		: 8;	//bit 24..31
	};
} PCIE_VC_RESCAP_REG;

//PCI Express Virtual Channel Resource Control Register
typedef union _PCIE_VC_RESCNT_REG {
	UINT32	VC_RESCNT;
	struct {
		UINT8	Tc_VcMap		: 8;	//bit 0..7
		UINT8	Reserved1		: 8;	//bit 8..15
		UINT8	LdPortArbTbl	: 1;	//bit 16
		UINT8   PortArbSel		: 3;	//bit 17..19		
		UINT8	Reserved2		: 4;	//bit 20..23
		UINT8	VcId			: 3;	//bit 24..26
		UINT8	Reserved3		: 4;	//bit 27..30
		UINT8	VcEnable		: 1;	//bit 31
	};
} PCIE_VC_RESCNT_REG;

//PCI Express Virtual Channel Resource Status Register
typedef union _PCIE_VC_RESSTS_REG {
	UINT16	VC_RESSTS;
	struct {
		UINT8	ArbTblSts		: 1;	//bit 0
		UINT8	VcNegatPend		: 1;	//bit 1
		UINT8	Reserved1		: 6;	//bit 8..15
		UINT8	Reserved2		: 8;	//bit 20..23
	};
} PCIE_VC_RESSTS_REG;


//////////////////////////////////////////////////////////////////////
// Root Complex Link Declaration Enhanced Capability Header
//////////////////////////////////////////////////////////////////////

#define PCIE_RCT_ESD_OFFSET			0x004
#define PCIE_LNK_DSC_OFFSET(Link)	(0x010+(Link*0x10))
#define PCIE_LNK_BAR_OFFSET(Link)	(0x018+(Link*0x10))

//definitions of Element Type Field
#define EL_TYPE_CFG_SPACE 			0x00	//Configuration Space Element
#define EL_TYPE_SYSMEM_PORT			0x01	//System egress port or internal sink (memory)
#define EL_TYPE_INTERNAL			0x02	//Internal Root Complex Link

//Element Self Description register (ESD)
typedef union _PCIE_RCT_ESD_REG {
	UINT32	RCT_ESD;
	struct{
		UINT8	ElementType		: 4;	//bit 0..3
		UINT8	Reserved1		: 4;	//bit 4..7
		UINT8	NumbOfLinks		: 8;	//bit 8..15
		UINT8	ComponentId		: 8;	//bit 16..23 Starts at 1 - 00 is invalid value
		UINT8	PortNumber		: 8;	//bit 24..31		
	};
} PCIE_RCT_ESD_REG;

#define LNK_TYPE_INT	0	//means link pointd to Memory Mapped space
#define LNK_TYPE_EXT	1	//means link pointd to PCI Bus Device 

//Link Description Register 
typedef union _PCIE_LNK_DSC_REG {
	UINT32	LNK_DSC;
	struct{
		UINT8	LinkValid		: 1;	//bit 0
		UINT8	LinkType		: 1;	//bit 1
        UINT8   AssocRCRBHdr    : 1;    //bit 2  //Added in V2.1
		UINT8	Reserved1		: 5;	//bit 3..7	
		UINT8	Reserved2		: 8;	//bit 8..15	
		UINT8	TargetCompId	: 8;	//bit 16..23
		UINT8	PortNumb		: 8;	//bit 24..31
	};
} PCIE_LNK_DSC_REG;

typedef union _PCIE_LNK_BAR_REG {
	UINT64	LNK_BAR;
	struct	{
		UINT32	BitsInBus       : 3;	//bit 0..3 Encoded number of Bus Number bits (Added in V2.1)
		UINT32	Reserved1		: 9;	//bit 0..11
		UINT32	Function		: 3;	//bit 12..14
		UINT32	Device			: 5;	//bit 15..19
		UINT32	Bus_CfgAddrx31	: 12;	//bit 20..31 (Added in V2.1) see 7.13.3.2.2 of PCIe Base 2.1
		UINT32	CfgAddr3263		: 32;	//bit 32..64
	};
} PCIE_LNK_BAR_REG;


//////////////////////////////////////////////////////////////////////
// PCI Express SRIOV Register Block
//////////////////////////////////////////////////////////////////////
#define PCIE_SRIOV_CAP_OFFSET       4
#define PCIE_SRIOV_CNT_OFFSET       8


typedef union _PCIE_SRIOV_CAP_REG{
    UINT32  SRIOV_CAP;
    struct  {
        UINT32 VfMigr           : 1;    //bit 0
        UINT32 Reserved         : 20;   //bit 1..20
        UINT32 VfMgrInt         : 11;   //bit 21..31
    };
}PCIE_SRIOV_CAP_REG;

typedef union _PCIE_SRIOV_CNT_REG{
    UINT16  SRIOV_CNT;
    struct  {
        UINT16 VfEnable         : 1;    //bit 0
        UINT16 VfMigrEnable     : 1;    //bit 1
        UINT16 VfMgrIntEnable   : 1;    //bit 2
        UINT16 VfMse            : 1;    //bit 3
        UINT16 AreCap           : 1;    //bit 4
        UINT16 Reserved         : 11;   //bit 5..15
    };
}PCIE_SRIOV_CNT_REG;

#define PCIE_SRIOV_INITIAL_VF_OFFSET            0x0C
#define PCIE_SRIOV_FIRST_VF_OFFSET              0x14
#define PCIE_SRIOV_VF_STRIDE_OFFSET             0x16
#define PCIE_SRIOV_SUPPORTED_PAGE_SIZES_OFFSET  0x1C
#define PCIE_SRIOV_SYSTEM_PAGE_SIZE_OFFSET      0x20
#define PCIE_SRIOV_VF_BAR0_OFFSET               0x24



#pragma pack(pop)

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**              5555 Oakbrook Pkwy, Norcross, GA 30093              **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
