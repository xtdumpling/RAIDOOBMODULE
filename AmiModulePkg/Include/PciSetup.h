//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Add a function that limit the primary VGA's resources under
//              4G.(Refer to Grantley)
//    Reason:   To prevent form Display have no output when enabled Above 4G
//              and set VGA Priority to offboard.
//    Auditor:  Isaac Hsu
//    Date:     Dec/14/2016
//
//***************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/BIN/Core/Modules/PciBus/PciSetup.h 5     11/09/11 1:55p Yakovlevs $
//
// $Revision: 5 $
//
// $Date: 11/09/11 1:55p $
//**********************************************************************

//**********************************************************************
/** @file PciSetup.h
    Definition of SETUIP Data fields.

**/
//**********************************************************************
#ifndef __PCI_SETUP__H__
#define __PCI_SETUP__H__


#include <Token.h>

#define PCI_COMMON_VAR_NAME				L"PCI_COMMON"
#define PCI_DEV_DEV_VAR_NAME_FORMAT		L"PCI_DEV_%X_PCI"
#define PCI_DEV_HP_VAR_NAME_FORMAT		L"PCI_DEV_%X_HTP"
#define PCI_DEV_PE1_VAR_NAME_FORMAT		L"PCI_DEV_%X_PX1"
#define PCI_DEV_PE2_VAR_NAME_FORMAT		L"PCI_DEV_%X_PX2"
#define PCI_DEV_SHOW_VAR_NAME_FORMAT	L"PCI_DEV_%X_SHW"
//#define ROM_DISPATCH_VAR_NAME_FORMAT    L"PCI_ROM_%X"


//{ACA9F304-21E2-4852-9875-7FF4881D67A5}
#define PCI_FORM_SET_GUID   {0xACA9F304, 0x21E2, 0x4852, 0x98, 0x75, 0x7F, 0xF4, 0x88, 0x1D, 0x67, 0xA5}
#define PCI_VARSTORE_GUID	PCI_FORM_SET_GUID	
//{97CDC6BE-7792-4782-BA6D-E6E3BB56ACD8}
#define ROM_FORM_SET_GUID   {0x97cdc6be, 0x7792, 0x4782, 0xba, 0x6d, 0xe6, 0xe3, 0xbb, 0x56, 0xac, 0xd8}
#define ROM_VARSTORE_GUID   ROM_FORM_SET_GUID   



#define PCI_FORM_SET_CLASS 		0x88
#define ROM_FORM_SET_CLASS      0x99

#define PCI_MAIN_FORM_ID 		1
#define PCI_DEV_FORM_ID 		2
#define PCIE1_FORM_ID 			3
#define PCIE2_FORM_ID 			4
#define PCI_HOTPLUG_FORM_ID		5
#define ROM_MAIN_FORM_ID        1

#define PCI_GOTO_LABEL_START	0x0101
#define ROM_LIST_LABEL_START    0x0101
#define PCI_GOTO_LABEL_END		0x0102
#define ROM_LIST_LABEL_END      0x0102
#define PCI_DEV_LABEL_START		0x0201
#define PCI_DEV_LABEL_END		0x0202
#define PCIE1_LABEL_START		0x0301
#define PCIE1_LABEL_END			0x0302
#define PCIE2_LABEL_START		0x0401
#define PCIE2_LABEL_END			0x0402
#define PCI_HP_LABEL_START		0x0501
#define PCI_HP_LABEL_END		0x0502

#define PCI_GOTO_ID_BASE 		0x6000
#define ROM_GOTO_ID_BASE        0x8000

#define PCIE1_GOTO_ID 			0x6FF1
#define PCIE2_GOTO_ID 			0x6FF2
#define	PCI_HP_GOTO_ID			0x6FF3

//Define Setup Question IDs 
#define PCI_PCI_LAT_QID			0x7001 
#define PCI_PCIX_LAT_QID		0x7002 
#define PCI_PCI_PERR_QID		0x7003 
#define PCI_PCI_SERR_QID		0x7004 
#define PCI_PAL_SNOOP_QID		0x7005 
#define PCI_CLOBAL_4G_QID		0x7006 
#define PCI_CLOBAL_SRIOV_QID	0x7007 
#define PCI_CLOBAL_HP_QID		0x7008 
#define PCI_LOCAL_4G_QID		0x7009
#define PCI_LOCAL_HP_QID		0x7026
#define PCI_LOCAL_PCIE1_QID		0x7027
#define PCI_LOCAL_PCIE2_QID		0x7028

#define PCI_PCIE1_MPL_QID		0x700A 
#define PCI_PCIE1_MRR_QID		0x700B 
#define PCI_PCIE1_ASPM_QID		0x700C 
#define PCI_GLOBAL_PCIE_BS_QID	0x700D 
#define PCI_GLOBAL_VC_QID       0x7029 


#define PCIE1_RELAXED_ORD_QID	0x700E 
#define PCIE1_EXT_TAG_QID		0x700F 
#define PCIE1_NO_SNOOP_QID		0x7010 
#define PCIE1_EXT_SYNC_QID		0x7011
#define PCIE1_CLOCK_PM_QID		0x7012
#define PCIE1_TR_RETRY_QID		0x7013
#define PCIE1_TR_TIMEOUT_QID	0x7014
#define PCIE1_EMPTY_LNK_QID		0x7015

#define PCIE2_COMPL_TIMEOUT_QID	0x7016
#define PCIE2_ARI_FWD_QID		0x7017
#define PCIE2_AOP_REQ_QID		0x7018
#define PCIE2_AOP_EGRES_QID		0x7019
#define PCIE2_IDO_REQ_QID		0x701A
#define PCIE2_IDO_COMPL_QID		0x701B
#define PCIE2_LTR_REP_QID		0x701C
#define PCIE2_E2E_TLP_QID		0x701D
#define PCIE2_LNK_SPEED_QID		0x701E
#define PCIE2_COMPL_SOS_QID		0x701F
#define PCIE2_HW_AW_QID			0x7020
#define PCIE2_HW_AS_QID			0x7021

#define PCIHP_BUS_PADD_QID		0x7022
#define PCIHP_IO_PADD_QID		0x7023
#define PCIHP_MMIO32_PADD_QID	0x7024
#define PCIHP_MMIO32PF_PADD_QID	0x7025
#define PCIHP_MMIO64_PADD_QID	0x7024
#define PCIHP_MMIO64PF_PADD_QID	0x7025


//next
//#define PCI_XXXX_QID			0x702B

#define ROM_VGA_SUPRESS_QID     0xFFF1
#define ROM_RESET_POLICY_QID    0xFFF2

#define ROM_DISPLAY_POLICY_QID  0xFFF3
#define ROM_DISPLAY_POLICY_QID1 0xFFF4
#define ROM_NETWORK_POLICY_QID  0xFFF5
#define ROM_NETWORK_POLICY_QID1 0xFFF6
#define ROM_STORAGE_POLICY_QID  0xFFF7
#define ROM_STORAGE_POLICY_QID1 0xFFF8
#define ROM_OTHER_POLICY_QID    0xFFF9
#define ROM_OTHER_POLICY_QID1   0xFFFA


#define ROM_CSM_ENABLE_QID2      0xEFF0
#define ROM_DISPLAY_POLICY_QID2  0xEFF1
#define ROM_NETWORK_POLICY_QID2  0xEFF2
#define ROM_STORAGE_POLICY_QID2  0xEFF3
#define ROM_OTHER_POLICY_QID2    0xEFF4

#define PCI_SETUP_AUTO_VALUE	55
#define PCI_SETUP_DONT_TOUCH    0xFE

#pragma pack(push, 1)

typedef struct _PCI_COMMON_SETUP_DATA {
    UINT8   S3ResumeVideoRepost;	///[Disable]\ Enable
    UINT8   S3PciExpressScripts;    ///[Disable]\ Enable
    UINT8   HotPlug;                /// Disable \[Enable]
    UINT8   Above4gDecode;          ///[Disable]\ Enable
    UINT8	SriovSupport;			///[Disable]\ Enable
    UINT8   DontResetVcMapping;     ///[Disable]\ Enable
} PCI_COMMON_SETUP_DATA;


typedef struct _PCI_DEVICE_SETUP_DATA {
	///General PCI Settings: [] - default
	UINT8   PciLatency;				///[32]\ 64 \ 96 \ 128 \ 160 \ 192 \ 224 \ 248
	UINT8   PciXLatency;			/// 32 \[64]\ 96 \ 128 \ 160 \ 192 \ 224 \ 248
    UINT8   VgaPallete;             ///[Disable]\ Enable 
    UINT8   PerrEnable;             ///[Disable]\ Enable 
    UINT8   SerrEnable;             ///[Disable]\ Enable 
    ///Setup option override questions
    UINT8   Decode4gDisable;
	UINT8	Pcie1Disable;
	UINT8	Pcie2Disable;
	UINT8	HpDisable;
} PCI_DEVICE_SETUP_DATA;

typedef struct _PCI_HP_SETUP_DATA{
    ///Hotlpug Related Settings visible if PCI_HOTPLUG_SUPPORT == 1 && PCI_EXPRESS_SUPPORT == 1
    UINT64  BusPadd;                /// Disable \[1]\ 2 \ 3 \ 4 \ 5
    UINT64  IoPadd;                 /// Disable \[ 4K]\ 8K \ 16K \ 32K  
    UINT64  Io32Padd;				/// Not used in IA PC systems
    UINT64  Mmio32Padd;             /// Disable \  1M \ 4M \  8M \[16M]\ 32M \ 64M \ 128M  
    UINT64  Mmio32PfPadd;           /// Disable \  1M \ 4M \  8M \[16M]\ 32M \ 64M \ 128M  
    UINT64  Mmio64Padd;             ///[Disable]\  1M \ 4M \  8M \ 16M \ 32M \ 64M \ 128M \ 256M \ 512M \ 1G
    UINT64  Mmio64PfPadd;           ///[Disable]\  1M \ 4M \  8M \ 16M \ 32M \ 64M \ 128M \ 256M \ 512M \ 1G
} PCI_HP_SETUP_DATA;

typedef struct _PCI_SETUP_SHOW {
	UINT8	Above4GHide;
	UINT8	Pcie1Hide;
	UINT8	Pcie2Hide;
	UINT8	HotplugHide;
}PCI_SETUP_SHOW;


typedef struct _ROM_POLICY_VAR {
    UINT8   Option; //Enable/Disable
    UINT32  SdlIdx; //Device Index in SDL Database
} ROM_POLICY_VAR;

typedef struct _CSM_SETUP_DATA {
    UINT8               CsmPresent;
    UINT8               CsmNetwork;
    UINT8               CsmMassSt;
    UINT8               CsmDisplay;
    UINT8               CsmOther;
} CSM_SETUP_DATA;

#ifndef MAX_NUMBER_OF_ROM_ITEMS 
#define MAX_NUMBER_OF_ROM_ITEMS 1
#endif

typedef struct _ROM_SETUP_DATA {
    UINT32              PriVgaIdx;
    UINT8               PriVgaGroupValue;
    UINT8               PriVgaItemValue;
    UINT16              OptRomCount;   //64K should be enough?
    UINT8               ExtraRomCount; //This one will cover Additional Option ROMs of OnBoard devices w/o SDL Rom flag 
    UINT8               SdlScanned;
    UINT8               SupressVga;
    UINT8               ResetPolicy;
    ROM_POLICY_VAR      PolicyOption[MAX_NUMBER_OF_ROM_ITEMS];
} ROM_SETUP_DATA;

//---------------------------------------------------------------------------------
///Structure Defines PCI Releated data stored in GLOBAL SETUP_DATA NVRAM variable
typedef struct _PCIE1_SETUP_DATA {
	///PCI Express Device Settings: [] - default
	UINT8	RelaxedOrdering; 		///[Disable]\ Enable
	UINT8	ExtTagField; 			///[Disable]\ Enable
	UINT8	NoSnoop;				/// Disable \[Enable]
	UINT8	MaxPayload;				///[Auto]\ 128 \ 256 \ 512 \ 1024 \ 2048 \ 4096 (in bytes)
	UINT8   MaxReadRequest;			///[Auto]\ 128 \ 256 \ 512 \ 1024 \ 2048 \ 4096 (in bytes)
	///PCI Express Link settings: [] - default
	UINT8   AspmMode; 				///[Disable]\ Auto \ Force L0
	UINT8   ExtendedSynch;			///[Disable]\ Enable
	UINT8	ClockPm;
    UINT8   LnkTrRetry;             ///[Disable]\ 2 \ 3 \ 5
    UINT16  LnkTrTimeout;           ///[1...1000] (Microseconds uS) 
    UINT8   LnkDisable;             ///[Keep ON == 0] / Disable ==1  
    UINT8   IgnoreCommonClock;      /// Enable /[Disable] == 0 by default.

    //Access controll
    UINT8   DevCtrDontTouch;         ///Used to protect RC settings.
    UINT8   LnkCtrDontTouch;         ///Used to protect RC settings.
    UINT8   SltCtrDontTouch;
} PCIE1_SETUP_DATA;

//PCI_EXPRESS_GEN2_SUPPORT
typedef struct _PCIE2_SETUP_DATA {
    ///Gen2 Device Settings
    UINT8   ComplTimeOut;           ///[Disable]\ Default \ 50 - 100 us \ 1ms - 10ms \ 16...
    UINT8   AriFwd;                 ///[Disable]\ Enable
    UINT8   AtomOpReq;              ///[Disable]\ Enable
    UINT8   AtomOpEgressBlk;        ///[Disable]\ Enable
    UINT8   IDOReq;                 ///[Disable]\ Enable
    UINT8   IDOCompl;               ///[Disable]\ Enable
    UINT8   LtrReport;              ///[Disable]\ Enable
    UINT8   E2ETlpPrBlk;            ///[Disable]\ Enable
    
    //Gen2 Link Settings
    UINT8   LnkSpeed;               ///[Auto]\ 5.0 GHz \ 2.5 GHz
    UINT8   ComplSos;               ///[Disable]\ Enable
    UINT8   HwAutoWidth;            ///[Enable]\ Disable    //LNK_CNT_REG #1
    UINT8   HwAutoSpeed;            ///[Enable]\ Disable

    //Access controll
    UINT8   DevCtr2DontTouch;        ///Used to protect RC settings.
    UINT8   LnkCtr2DontTouch;        ///Used to protect RC settings.

} PCIE2_SETUP_DATA;

#pragma pack(pop)

#ifndef VFRCOMPILE

typedef enum {
	dtNone=0,		
	dtDevice,
	dtPcie1,
	dtPcie2,
	dtHp,
	dtMaxType
} PCI_SETUP_TYPE;

typedef union _PCI_HOTPLUG_SETUP_DATA {
	UINT64 				ARRAY[7];
	PCI_HP_SETUP_DATA   ArrayField;
}PCI_HOTPLUG_SETUP_DATA;

typedef struct _PCI_SETUP_DATA {
//	PCI_COMMON_SETUP_DATA	CommonSettings;
	PCI_DEVICE_SETUP_DATA	PciDevSettings;
	PCIE1_SETUP_DATA		Pcie1Settings;
	PCIE2_SETUP_DATA		Pcie2Settings;
//	PCI_HOTPLUG_SETUP_DATA	HotplugSettings;
#if SMCPKG_SUPPORT
    UINT8   SmcPriorVgaBus;
#endif
}PCI_SETUP_DATA;

#endif


//Function Prototypes
//VOID BrdGetPciSetupData(PCI_SETUP_DATA *PciSetupData);

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
