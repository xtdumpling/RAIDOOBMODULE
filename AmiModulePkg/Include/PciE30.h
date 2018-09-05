//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  @brief PCI-E 3.0/3.x Declarations.
**/
#ifndef _PCI_EXPRESS30_H
#define _PCI_EXPRESS30_H

#pragma pack(push,1)
//////////////////////////////////////////////////////////////////////
///Pci Express Gen 3.x Registers And Definitions
//////////////////////////////////////////////////////////////////////

//Different encoding for determining Link Speed used in V3.0.
//Added in V3.0 
///
///6/23/2015 updated with v4.0 spec
///
#define PCIE_LINK_SPEED_VECT_25G     0x01
#define PCIE_LINK_SPEED_VECT_50G     0x02
#define PCIE_LINK_SPEED_VECT_80G     0x04
//Added in V4.0 spec
#define PCIE_LINK_SPEED_VECT_16G     0x08

//This Definitions cahnged to reflect LNK SPEED VECTOR BITs
#define PCIE_LINK_SPEED_80G     	0x03
#define PCIE_LINK_SPEED_16G     	0x04

//For M-PCIe this has a different meaning
#define M_PCIE_HS_G1_G2    			PCIE_LINK_SPEED_VECT_25G
#define M_PCIE_HS_G3     			PCIE_LINK_SPEED_VECT_50G


//Pci Express GEN2 Link Capabilities 2 Register
//Just a place holder for now....in V2
//Defined  in Spec V3.0
typedef union _PCIE_LNK_CAP2_REG {	
	UINT32		LNK_CAP2;
	struct	{
        UINT32  Reserved1       :   1; //bit 0
        UINT32  SuppLnkSpeeds   :   7; //bit 1..7
        UINT32  CrossLnk        :   1; //bit 8  
///6/23/2015 updated with v4.0 spec
        UINT32  LowSkpGenSpVect :   7; //bit  9,10,11,12,13,14,15  	//added in v3.1
        UINT32  LowSkpResSpVect :   7; //bit 16,17,18,19,20,21,22 	//added in v3.1
        UINT32  RetimerPresDet	:   1; //bit 23 					//added in v4 0
        UINT32  Reserved2       :   7; //bit 24,25,26,27,28,29,30
        UINT32	DrsSupport		: 	1; //bit 31						//added in v3.1
//-------------------------------		
	};
} PCIE_LNK_CAP2_REG;

///Dfined in V4.0 Down Stream Component Presence
#define PCIE_CAP_ID_SEC_PCIE_CAP    0x19


// Extended capabilities IDs added in PCIe 3.0 spec                     
#define PCIE_CAP_ID_SEC_PCIE_CAP    0x19

#define PCIE_LNK_CNT3_OFFSET        0x04 //from Secondary PciE Ext Cap Header
#define PCIE_LANE_ERR_STA_OFFSET    0x08
#define PCIE_EQUALIZ_CNT_OFFSET     0x0C //Sized by MAX Link width.

//
//The Secondary PCI Express Extended Capability structure is required for all Ports and RCRBs that
//support a Link speed of 8.0 GT/s or higher. For Multi-Function Upstream Ports, this capability
//must be implemented in Function 0 and must not be implemented in other Functions.
//

//PCI Express GEN 3 Link Control 3 Register 
//Updated to PCIe v2.1
typedef union _PCIE_LNK_CNT3_REG {
	UINT32		LNK_CNT3;
	struct {
		UINT32	PerformEqualiz  :   1;  //bit 0
		UINT32	LnkEqReqIntEn   :   1;  //bit 1
//6/23/2015 updated with v4.0 spec
		UINT32	Reserved	    :	7;	//bit 2,3,4,5,6,7,8
		UINT32  LoSpkOsSpeedEn	:   7;  //bit 9,10,11,12,13,14,15, 
//-------------------------------
		UINT32	Reserved2	    :	16;	//bit 16..31
	};
} PCIE_LNK_CNT3_REG;



typedef union _PCIE_LANE_EQ_CNT_REG {
	UINT16		LANE_EQ_CNT;
	struct {
		UINT16	DnsPortTrPreset :   4;  //bit 0..3
		UINT16	DnsPortRcPrHint :   3;  //bit 4..6
		UINT16	Reserved1	    :	1;	//bit 7
		UINT16	UpsPortTrPreset :   4;  //bit 0..3
		UINT16	UpsPortRcPrHint :   3;  //bit 4..6
		UINT16	Reserved2	    :	1;	//bit 7
	};
} PCIE_LANE_EQ_CNT_REG;

#pragma pack(pop)
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
