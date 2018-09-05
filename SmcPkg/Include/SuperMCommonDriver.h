//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/18/2014
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SuperMCommonDriver.h
//
// Description:
//  This file include SUPERMBOARDINFO data structure
//
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef	_SUPERM_COMMON_DRIVER_H_
#define	_SUPERM_COMMON_DRIVER_H_

//----------------------------------------------------------------------------
// Include File(s)
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Define(s)
//----------------------------------------------------------------------------
#define MAX_LAN_MAC_INFO_SIZE 19
#pragma warning ( disable : 4090 4028)

#pragma pack(1)

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: SUPERMBOARDINFO
//
// Description: AMI SMBUS driver private data structure
//
// Fields: Name                 Type                    Description
//----------------------------------------------------------------------------
// MBSVID                       UINT16                  Motherboard SVID
// GPU_AdjustVID_Support        UINT8					Support BMC adjust GPU VID
// BMCCMD70_SetGPU_DID          UINT8					BMC command 0x70 data
//                                                      BMCCMD70[n][0] : 0x32
//                                                         Sub command for this feature
//                                                      BMCCMD70[n][1] : Slot number
//                                                      BMCCMD70[n][2] : Low byte of GPU DID
//                                                      BMCCMD70[n][3] : High byte of GPU DID
//                                                                    07 Get/Set Lan mode
//                                                                    0D Get/Set SDR revision
//                                                                    0E  Get/Set FRU revision
// SystemConfiguration    		UINT8					BMC Command 0x99 data
//                                                      SystemConfiguration[0] : Bit 0, Onboard Mellanox IB
//                                                                               Bit 1, Analog Hotswap Controller
//                                                                               Bit 2, CPU Overheat LED Controller
//                                                      SystemConfiguration[1] ~ [9]: Reserve
//
// HardwareInfo     			UINT8					BMC command 20/21 data
//                                                      SystemConfigInfo[0~1] = SSID
//                                                      SystemConfigInfo[2] = Sensor type. Always 7 in current design
//                                                      SystemConfigInfo[3] =Bit 4, For CPU 0/CPU1 revise
//                                                                                          Bit 5, 1 for 83795-HF, 0 for 83795-F
//                                                                                          Bit 6, 10G lan support
//                                                                                          Bit 7, CPU temperature source.0 for H/W monitor,1 for ME
//                                                      SystemConfigInfo[4~9] = Onbard Lan1 MAC address
//
// LanMacInfo               	UINT8					System Lan Mac address
//                                                      LanMacInfo[0]= Lan Number(exclude Num lan of CMD 20)
//                                                      LanMacInfo[1~6] = Lan Mac
//                                                      LanMacInfo[7~12] = Lan Mac
//                                                      LanMacInfo[13~18] = Lan Mac
//
// BMC_Present                  UINT8					1 for BMC present
// SMCCheckTPMPresent           UINT8					1 for TPM present.
// PCB_Revision					UINT32					Record PCB revision
//                                                      Data format 100 == "1.00"
//                                                                  10a == "1.0a"
//                                                      			      .
//                                                      			      .
//                                                      			      .
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct _SUPERMBOARDINFO {
	UINT16	MBSVID;
	UINT8  GPU_AdjustVID_Support;
	UINT8  BMCCMD70_SetGPU_DID[4][4];
	UINT8  SystemConfiguration[10];
	UINT8  HardwareInfo[10];
	UINT8  LanMacInfo[MAX_LAN_MAC_INFO_SIZE];
	UINT8  BMC_Present;
	UINT8  SMCCheckTPMPresent;
	UINT32 PCB_Revision;
} SUPERMBOARDINFO;

#pragma pack()



#endif // _SUPERM_COMMON_DRIVER_H_

//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2013 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************

