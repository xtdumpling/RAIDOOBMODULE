//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.05
//    Bug Fix : N/A
//    Reason  : Read SATA device configuration by Devices protocol.
//    Auditor : Durant Lin
//    Date    : Jun/01/2018
//
//  Rev. 1.04
//    Bug Fix:  [Assetinfo] Fixed HDD information "Serial number", "Firmware Revision" and "Model Number" missing one ASCII character.
//    Reason:   Add 1 byte for ModelNumber, SerialNumber, FW_Rev 
//    Auditor:  Jimmy Chiu (Refer Greenlow - SVN_2191)
//    Date:     Mar/27/2017
//
//  Rev. 1.03
//    Bug Fix:  Enable SmcAssetInfo module and change IPMI command.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/10/2016
//
//  Rev. 1.02
//    Bug Fix:  Fix some data filed not meet HDD Asset spec
//    Reason:   
//    Auditor:  Salmon Chen
//    Date:     Jul/20/2015
//
//  Rev. 1.01
//    Bug Fix:  Fix some data filed not meet HDD Asset spec
//    Reason:   
//    Auditor:  Salmon Chen
//    Date:     Jul/20/2015
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:   
//    Auditor:  Salmon Chen
//    Date:     Jun/16/2015
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcAssetInfo.h
//
// Description: SMC Asset Information feature header file.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef	_SMCASSETINFODATA_H_
#define	_SMCASSETINFODATA_H_


//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: Asset_Head_Info
//
// 
//----------------------------------------------------------------------------
//<AMI_THDR_END>

#pragma pack(1)

typedef struct{
    UINT8	Major_Version;
    UINT16	Minor_Version;
    UINT32	Length;
    UINT16	Count;
    UINT8	Reserve[5];
    UINT8	Playload_Checksum;
    UINT8	Header_Checksum;
}Asset_Head_Info;

typedef struct{
    UINT8	Tier_ID;
    UINT16	Length;
    UINT8	Item1;
    CHAR8	Controller_Name[9];
}Tier1;

typedef struct{
    UINT8	Tier_ID;
    UINT16	Length;
    UINT8	Item1;
    CHAR8	Config_Type[5];
}Tier2;	

typedef struct{
    UINT8	Tier_ID;
    UINT16	Length;
    UINT8	Item2;
    UINT16	Slot_ID;
    UINT8	Item3;
    UINT8	Slot_Populated;
    UINT8	Item4;
    CHAR8	Model_Name[40+1];
    UINT8	Item5;
    CHAR8	SN[20+1];
    UINT8	Item6;
    CHAR8	FW_Rev[8+1];
    UINT8	Item7;
    UINT8	Smart_Support;
}Tier3;	

typedef struct{
    UINT16	Length;
    Tier1	HDD_Tier1;
    Tier2	HDD_Tier2;
    Tier3	HDD_Tier3;
}Instance;	

#pragma pack()

#define R_PCH_SATA_SATAGC                   0x9C

#endif // _SMCBUILDTIMEDATA_H_
