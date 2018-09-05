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
// $Header: /AptioV/BIN/AMIDebugRxPkg/Include/Library/AMIpeidebug.h 1     11/02/12 10:15a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:15a $
//*****************************************************************
//*****************************************************************
//
//	peidebug.h
//
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRxPkg/Include/Library/AMIpeidebug.h $
// 
//
//*****************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          AMIPeiDebug.h
//
// Description:   Contains 32 bit Pei Debugger prototypes
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef	_AMI_PEIDEBUG_H_
#define _AMI_PEIDEBUG_H_
#include "Token.h"


//#if USB_DEBUG_TRANSPORT
#include <Library/EHCI.h>
//#endif

#define EFI_PEI_DBG_BASEADDRESS_PPI_GUID \
  { 0x9bf4e141, 0xa4ec, 0x4d72,  {0xbc, 0xc4, 0x94, 0x2, 0x1d, 0x2b, 0x80, 0xbd }}

UINTN GetDbgDataBaseAddress();
BOOLEAN IsPMModeEnabled (UINT8);

#define	PEI_DEBUG_DATASECTION_BASEADDRESS	GetDbgDataBaseAddress()	//Starting address of PEI debugger data section
#define PEI_DEBUG_DATASECTION_SIZE		    0x2048
#define IRQ_VECTOR_BASE	0x8

#define PEI_DEBUG_DATA_BASEADDRESS  GetDbgDataBaseAddress() //(PeiDbgDataSection.DATABASEADDRESS)

#define IDT_ENTRIES					0x0	    // Number of Entries in IDT
#define IDT_SIZE					0x0        // Total Size = 160 Bytes
#define CONTEXT_SIZE				0x74	    // Total Size = 116 Bytes
#define XPORTLOCALVAR_SIZE			0x81	    // Total Size =  113 Bytes
#define XPORTGLOBALVAR_SIZE			0x30	    // Total Size =  24 Bytes
#define DBGRLOCALVAR_SIZE			0x85	    // Total Size =  69 Bytes
#define XPORTEXPTNBUF_SIZE			0x20	    // Total Size =  32 Bytes
#define GLOBALBUF_SIZE				0x210	    // Total Size = 528 Bytes
#define ACKBUF_SIZE					0x0a		// Total Size =  10 Bytes

//#define PEI_DEBUG_IDTBASEADDR			PEI_DEBUG_DATA_BASEADDRESS
#define PEI_DEBUG_CONTEXTADDR			PEI_DEBUG_DATA_BASEADDRESS //+ IDT_SIZE
#define PEI_DEBUG_XPORTLOCALVARADDR		PEI_DEBUG_CONTEXTADDR + CONTEXT_SIZE
#define PEI_DEBUG_XPORTGLOBALVARADDR	PEI_DEBUG_XPORTLOCALVARADDR + XPORTLOCALVAR_SIZE
#define PEI_DEBUG_DBGRLOCALVARADDR		PEI_DEBUG_XPORTGLOBALVARADDR + XPORTGLOBALVAR_SIZE
#define PEI_DEBUG_XPORTEXPTNADDR		PEI_DEBUG_DBGRLOCALVARADDR + DBGRLOCALVAR_SIZE
#define PEI_DEBUG_BUFADDR				PEI_DEBUG_XPORTEXPTNADDR + XPORTEXPTNBUF_SIZE
#define	PEI_DEBUG_ACKADDR				PEI_DEBUG_BUFADDR + GLOBALBUF_SIZE

#define	IN_PEI_BEFORE_MEMORY		0
#define	IN_PEI_AFTER_MEMORY			01
#define	IN_DXE_WITH_PEI_DBGR		02
#define	IN_DXE_WITH_DXE_DBGR		03
#define IN_SMM_WITH_SMM_DBGR		04

#pragma pack (push, pei_struct_data_pack, 1)

typedef struct {
	unsigned char	ContextReg[CONTEXT_SIZE];
	unsigned char	XportLocalVar[XPORTLOCALVAR_SIZE];
	unsigned char	XportGlobalVar[XPORTGLOBALVAR_SIZE];
	unsigned char	DbgrLocalVar[DBGRLOCALVAR_SIZE];
	unsigned char	XportExptn[XPORTEXPTNBUF_SIZE];
	unsigned char	GlobalBuf[GLOBALBUF_SIZE];
	unsigned char	AckBuf[ACKBUF_SIZE];
} PEIDebugData_T;

#pragma pack (pop, pei_struct_data_pack)

typedef struct _AMI_PEI_TRANSPORT_PROTOCOL	{
	UINT8	Signature[4];
	UINT16	(*Initialize)();
	UINT16	(*Restore)();
	UINT16	(*Send)();
	UINT16	(*Receive)();
	UINT16	(*SyncPacketNumber)();
	UINT16  (*RegisterHandler)();
	VOID	(*Callback)();
	UINT16	(*SendNoAck)();
	RETURN_STATUS	(*ConsoleIn)();
	VOID 	(*SendExptn)();
	RETURN_STATUS	(*ReInitialize)();
	RETURN_STATUS	(*SmmReInitialize)();
	RETURN_STATUS	(*UpdateTargetState)();
} AMI_PEI_TRANSPORT_PROTOCOL;

typedef	struct{
	UINT8	Signature[4];
	VOID	(*ExceptionCallback)();
	VOID	(*SendMessage)();
	UINTN	(*PeiDbgr_EnableDebugSupport)(UINTN BrkPtAddr);
	UINTN	(*PeiDbgr_SendPeiLoadImage)(UINTN FileHandle,UINTN ImageAddr,UINTN ImageSize,UINTN EntryPoint,void * PDBFileName);
	VOID	(*DoSpecialBreakPoint)(UINT32 EaxValue,UINT32 EdxValue);
}AMI_DEBUGGER_INTERFACE;

#pragma pack(1)
typedef struct {
	UINT32	m_EIP;
	UINT16	m_SendPktSize;
	UINT8	m_CurSendPktNum;
	UINT16	m_CurIxInRcvPkt;
	UINT8	m_CurRcvPktNum;
	UINT32  m_ExptnHndlr;
	UINT8	m_IsSending;
	UINT16	m_ExptnType;
	UINT8	m_PrevByteEsc;
	UINT8	m_RcvingPkt;
	UINT8	m_RcvPktReady;
	UINT16	m_RcvPktSize;
	UINT8	m_IsPortInListenMode;
	UINT8	m_IsHostConnected;
	UINT8	m_IsPeiDbgIsNotS3;
	UINT64	m_PeiDbgRxInitTimerVal;
	UINT8	m_IsPMModeEnabled;
//#if !USB_DEBUG_TRANSPORT
	UINT16  m_BaseAddr;
	//To obtain info from Porting template support, and use it to publish HOB
	UINT8	m_SIO_LDN_UART;			//Logical Device Number for COM port
//#else
	UINT32	USBBASE;
	UINT32	USB2_DEBUG_PORT_REGISTER_INTERFACE;
	UINT8	gDebugUSBAddress;
	UINT8	gDebugInEndpoint;
	UINT8	gDebugOutEndpoint;
	GET_DESCRIPTOR_DEBUG_PORT	Descriptor;
	UINT8	gUSBBulkTransferSetAddr;

	UINT8	gIN_DATAPID;
	UINT8	gOUT_DATAPID;

	UINT8	gIN_DATABUFFER[8];
	UINT8	gOUT_DATABUFFER[8];

	UINT8	gCurrentINDataBufferLength;
	UINT8	gCurrentINDataBufferIndex;
	UINT8	gCurrentOUTDataBufferIndex;

	UINT8 	lastdata;

	UINT8	gIsUsbDebugPortInUse;
//	UINT8	gIsUsbDebugPortInListenMode;

	//To obtain info from Porting template support, and use it to publish HOB
	UINT16	m_EHCI_MEMORY_SIZE;
	UINT8	m_PCI_EHCI_BUS_NUMBER;
	UINT8	m_PCI_EHCI_DEVICE_NUMBER;
	UINT8	m_PCI_EHCI_FUNCTION_NUMBER;
	UINT8	m_PCI_EHCI_BAR_OFFSET;
//#endif
// For USB 3.0 support
	UINT32	m_USB3HC;
	VOID*	m_DbgCapData;

}SerXPortGblData_T;

typedef struct {
	AMI_PEI_TRANSPORT_PROTOCOL	*gTransportProtocol;
	UINT16 (*Handler)();

	//Additional parameters added in order to support DXE phase callback of
	//IRQ register handlers
	UINTN	InterruptType;
	UINTN	ContextData;
	VOID	(*IRQRegisterHandler)(UINTN,UINTN);

//#if USB_DEBUG_TRANSPORT
//Additional interface for PeiDbgSIO module to reinit EHCI memory base
	void	(*PeiDbgSIO_Init_USB_EHCI)(VOID *);
//#endif
}SerXPortGblVar_T;

typedef struct {
	UINT8	m_BrkPtType;
	UINT16	m_CS;
	UINT32	m_EIP;
	UINT16	m_SS;
	UINT32	m_ESP;
	UINT32	m_DR2;
	UINT32	m_DR7;
	UINT8	m_TargetRunning;
	UINT8	m_DbgrBusy;
	UINT8	m_DR2Valid;

	UINT8	m_GPOccrd;
	UINT8   m_MSRState;

	UINT8	m_NotifyAndBreak;

	UINT16	m_DbgSendBufSize;
	UINT16	m_DbgRcvBufSize;
	VOID	(*PeiDbgrReportStatusCode)();
	UINT8	m_FlagToDebugAfterMemory;
	UINTN	(*PeiDbgr_EnableDebugSupport)(UINTN BrkPtAddr);
	AMI_DEBUGGER_INTERFACE	*pPeiDbgDbgrIfc;
	UINT8	m_TargetState;
	VOID*	m_Services;
	RETURN_STATUS (*UpdateFVHob)();
	UINT8	m_SMMEntryBreak;
	UINT8	m_SMMExitBreak;
	UINT8	m_SMMDispatchLoaded;
	UINTN 	m_INT1Hndlr;
	UINTN 	m_INT3Hndlr;
	UINTN 	m_TmrHndlr;
	UINT8 	m_DbgInitPending;
	UINT64 	m_DbgInitTimeStart;
	UINT64 	m_DbgInitTimeEnd;
	UINT8	m_PerfDataAvailable;
	UINTN	m_SmmDebugDataAddress;
	UINT8	m_IsRuntime;
	UINT32	m_CarBaseAddress;
	UINT32	m_CarEndAddress;
	UINT8	m_isMemOpInProgress;
}DbgGblData_T;

#pragma pack()

#pragma pack(1)
typedef struct {
	UINT16	m_OffLow;
	UINT16	m_SegSel;
	UINT16	m_Reserved;
	UINT16	m_OffHigh;
}IDTEntry_T;

typedef struct {
	UINT16	IDTLength;
	UINTN	IDTBaseAddr;	//should work for both 32 and 64 mode
}IDTBASE;

typedef UINT8 (DBG_IS_S3)(void);
#pragma pack()

void DriverSpecialBreakPoint(UINT32 EaxVal, UINT32 EdxVal);

#endif

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
