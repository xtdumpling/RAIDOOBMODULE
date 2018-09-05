//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/EHCI.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/EHCI.h $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     1/23/14 4:37p Sudhirv
// EIP I51926 - Serial Debugger - make Serial Debugger independent of
// AMIDebugRx and Debuger
// 
// 1     11/02/12 10:07a Sudhirv
// AMIDebugRxPkg modulepart for AMI Debug for UEFI 3.0
// 
// 2     8/03/12 6:32p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//*****************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:		ehci.h
//
// Description:	EHCI controller related declarations and #defines.
//
//<AMI_FHDR_END>
//**********************************************************************

/*
	USB Debug port resides in EHCI
	Following is specific to chipset (ICH4,ICH5)
*/
#ifndef	__EHCI_H__
#define __EHCI_H__

#define	EHCI_BUS_NUMBER					0
#define	EHCI_DEVICE_NUMBER				29
#define	EHCI_FUNCTION_NUMBER			7

#define EHCI_CAPABILITY_REG				0x34

#define	EHCI_POWERMGMT_CAP_REG			0x50	//ICH4 & ICH5(i875 MB)
#define EHCI_DEBUGPORT_CAP_REG			0x58	//ICH4 & ICH5(i875 MB)

#define ICH5_EHCI_BASE_ADDRESS_REGISTER	0x10

#define MEMORYMAP_BASE_ADDR				0xEFFFFFFF	//4GB - 256MB
#define	MEMORYMAP_WINDOW_SIZE			0x3FF		//1 KB relocateble mem space (ICH4/5)

//
// Turn off the PWE assertion and put the device into D0 State
//
#define	ICH5_PMCSR						0x8000
////////////////////////////////////////////////

#define ICH5_PCI_CAPABILITY_ID_PMI		0x01
#define ICH5_EHCI_COMMAND_REGISTER      0x04

#define	DEBUG_PORT						0x0A
#define PORTSC_CURRENT_CONNECT_STATUS	1
#define PORTSC_CONNECT_STATUS_CHANGE	2
#define PORTSC_PORT_ENABLE				4
#define PORTSC_PORT_DISABLE				0xFFFFFFFB
#define PORTSC_PORT_RESET				0x100
#define PORTSC_PORT_NOT_RESET			0xFFFFFEFF

#define IN_PID							0x69
#define OUT_PID							0xE1
#define SETUP_PID						0x2D
#define DATA0							0xC3
#define DATA1							0x4B

#define ACK								0xD2
#define NAK								0x5A
#define STALL							0x1E
#define NYET							0x96

#define DEBUG_PORT_DESCRIPTOR_TYPE		0x0A

#define MAX_USBDEBUGPORT_DATASIZE		8

#pragma pack(1)
typedef struct _CAP_REG_LIST{
	UINT8	CapID;
	UINT8	Nxt_Ptr;
	UINT16	Offset	:13;
	UINT16	BAR		:3;
}CAP_REG_LIST;

typedef struct _DEBUG_PORT_CONTROL_REGISTER{
	UINT32	DataLength	: 4;
	UINT32	WriteRead	: 1;
	UINT32	Go			: 1;
	UINT32	ErrorGood	: 1;
	UINT32	Exception	: 3;
	UINT32	InUse		: 1;
	UINT32	Reserved1	: 5;
	UINT32	Done		: 1;
	UINT32	Reserved2	: 11;
	UINT32	Enabled		: 1;
	UINT32	Reserved3	: 1;
	UINT32	Owner		: 1;
	UINT32	Reserved4	: 1;
}DEBUG_PORT_CONTROL_REGISTER;

typedef struct _USB_PID{
	UINT32	TokenPID	: 8;
	UINT32	SendPID		: 8;
	UINT32	RecievedPID	: 8;
	UINT32	Reserved	: 8;
}USB_PID;

typedef	struct _DATA_BUFFER{
	UINT32	LowDword;
	UINT32	HighDword;
}DATA_BUFFER;

typedef struct _DEVICE_ADDR_REGISTER{
	UINT32	USBEndPoint : 4;
	UINT32	Reserved1	: 4;
	UINT32	USBAddress	: 7;
	UINT32	Reserved2	: 17;
}DEVICE_ADDR_REGISTER;

typedef	struct	_HCSPARAMS{
	UINT32	N_PORTS		: 4;
	UINT32	PPC			: 1;
	UINT32	Resrvd		: 2;
	UINT32	PRR			: 1;
	UINT32	N_PCC		: 4;
	UINT32	N_CC		: 4;
	UINT32	P_INDICATOR	: 1;
	UINT32	Resrvd2		: 3;
	UINT32	DebugPortNum	: 4;
	UINT32	Resrvd3		: 8;
}HCSPARAMS;

typedef struct _GET_DESCRIPTOR_DEBUG_PORT{
	UINT8	bLength;
	UINT8	bDescriptorType;
	UINT8	bDebugInEndpoint;
	UINT8	bDebugOutEndpoint;
}GET_DESCRIPTOR_DEBUG_PORT;


typedef struct _USB_CMD{
	UINT32	RunStop		: 1;
	UINT32	HCRESET		: 1;
	UINT32	FLS			: 2;
	UINT32	PSE			: 1;
	UINT32	ASE			: 1;
	UINT32	IAAD		: 1;
	UINT32	LHCR		: 1;
	UINT32	ASPMC		: 3;
	UINT32	ASPME		: 1;
	UINT32	Resvd1		: 4;
	UINT32	ITC			: 8;
	UINT32	Resvd2		: 8;
}USBCMD;

typedef struct	_USB_STS{
	UINT32	USBINTR		: 1;
	UINT32	USBERRINT	: 1;
	UINT32	PCD			: 1;
	UINT32	FLR			: 1;
	UINT32	HSE			: 1;
	UINT32	INTASYNC	: 1;
	UINT32	Resvd1		: 6;
	UINT32	HCHalted	: 1;
	UINT32	Reclamation	: 1;
	UINT32	PSS			: 1;
	UINT32	ASS			: 1;
	UINT32	Resvd2		: 16;
}USBSTS;

typedef struct	_CONFIG_FLAG{
	UINT32	CF			: 1;
	UINT32	Reserved	: 31;
}CONFIGFLAG, *PCONFIGFLAG;

typedef struct	_OPERATIONAL_REGISTER{
	USBCMD	USBCommandReg;
	USBSTS	USBStatusReg;
}OPERATIONAL_REGISTER, *POPERATIONAL_REGISTER;

#pragma pack()

#endif	// __EHCI_H__

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
