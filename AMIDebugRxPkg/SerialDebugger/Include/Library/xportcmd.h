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
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/xportcmd.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
//	dbgcmd.h
//
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/xportcmd.h $
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

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          xportcmd.h
//
// Description:   Pei Debuglayer prototypes
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#pragma pack(1)

typedef struct DbgCmd
{	UINT8 Cmd;
}DbgCmd;

#define  BIOS_HANDSHAKE		0x081

typedef struct {
	UINT16		Year;
	UINT8		Month;
	UINT8		Day;
	UINT8		Hour;
	UINT8		Minute;
	UINT8		Second;
} DateTime_T;

typedef struct CPUInfo_T
{	UINT8 Manufacturer[12];	//db	12 dup (?)
	UINT32 Version;			//dd	?
	UINT32 Feature;			//dd	?
}CPUInfo_T;

//EIP 59115 - Support to control Timestamp ON\OFF and show sec diff b/n Chkpts
typedef struct {
	UINT8	Command;
	CPUInfo_T	CPUInfo;
	UINT8		ProjectTag[8];
	DateTime_T	RealTime;
	DateTime_T	BuildTime;
	UINT8		BiosCoreType;
	UINT64		TimeStamp;	//Send to Target Timestamp for 10ms
	UINT64		InitialTs;
	UINT64		FinalTs;
}biosHandshake_S;

#define DBG_HANDSHAKE_ACK 0x1
#define DBG_HOST_TYPE_AMI_DEBUG_RX 			 0x01
#define DBG_HOST_TYPE_DEBUGGER_HOST_APP		 0x02
#define DBG_HOST_TYPE_DEBUGGER_HOST_NO_RX	 0x03

typedef struct DbgHandshakeAck
{	UINT8 Cmd;
	UINT8 HostType;
	UINT8 Reserved[3];
}DbgHandshakeAck;


#define BIOS_BOOT_PROCESS_START		0xD0
#define DBG_BOOT_PROCESS_START_ACK			0x50

typedef struct {
	UINT8	Command;
} biosBootProcessStart_S;


typedef struct {
	UINT8	Command;
} dbgBootProcessStartAck_S;


#define BIOS_SMM_BREAK_STATE 		0xD1
#define DBG_SMM_BREAK_STATE_ACK 	0x51

typedef struct {
	UINT8		Command;
} biosSmmBreakState_S;

typedef struct {
	UINT8	Command;
	UINT8	SmmEntryBreak;
	UINT8	SmmExitBreak;
} dbgSmmBreakStateAck_S;

#pragma pack()

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
