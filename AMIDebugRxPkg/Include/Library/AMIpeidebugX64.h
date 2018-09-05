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
// $Header: /AptioV/BIN/AMIDebugRxPkg/Include/Library/AMIpeidebugX64.h 1     11/02/12 10:15a Sudhirv $
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
// $Log: /AptioV/SRC/AMIDebugRxPkg/Include/Library/AMIpeidebugX64.h $
// 
//
//*****************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          AmiPeiDebugx64.C
//
// Description:   Contains X64 Pei Debugger prototypes
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef	_AMI_PEIDEBUGx64_H_
#define _AMI_PEIDEBUGx64_H_
#include "token.h"

//#if USB_DEBUG_TRANSPORT
#include <Library/EHCI.h>
//#endif

#define EFI_PEI_DBG_BASEADDRESS_PPI_GUID \
  { 0x9bf4e141, 0xa4ec, 0x4d72,  {0xbc, 0xc4, 0x94, 0x2, 0x1d, 0x2b, 0x80, 0xbd }}

UINTN GetDbgDataBaseAddress();
BOOLEAN IsPMModeEnabled (UINT8);

#define	IN_PEI_BEFORE_MEMORY		0
#define	IN_PEI_AFTER_MEMORY			01
#define	IN_DXE_WITH_PEI_DBGR		02
#define	IN_DXE_WITH_DXE_DBGR		03
#define IN_SMM_WITH_SMM_DBGR		04

#define PEI_DEBUG_DATASECTION_SIZEx64 \
		(IDT_SIZEx64 + CONTEXT_SIZEx64 + XPORTLOCALVAR_SIZEx64 + XPORTGLOBALVAR_SIZEx64\
		+ DBGRLOCALVAR_SIZEx64 + XPORTEXPTNBUF_SIZEx64 + GLOBALBUF_SIZEx64 + ACKBUF_SIZEx64)

#define IRQ_VECTOR_BASE	0x8

typedef struct {
	UINT8	Signature[4];
	UINT32	DATABASEADDRESS;
}PEI_DBG_DATA_SECTIONx64;

#define PEI_DEBUG_DATA_BASEADDRESS  GetDbgDataBaseAddress()

#define IDT_ENTRIES						0x0	    // Number of Entries in IDT
#define IDT_SIZEx64						0x0     // Total Size = 352 Bytes
#define CONTEXT_SIZEx64					0x140	// Total Size = 320 Bytes
#define XPORTLOCALVAR_SIZEx64			0x91	// Total Size =  125 Bytes
#define XPORTGLOBALVAR_SIZEx64			0x40	// Total Size =  64 Bytes
#define DBGRLOCALVAR_SIZEx64			0xa3	// Total Size =  125 Bytes
#define XPORTEXPTNBUF_SIZEx64			0x20	// Total Size =  32 Bytes
#define GLOBALBUF_SIZEx64				0x235	// Total Size = 565 Bytes
#define ACKBUF_SIZEx64					0x0a	// Total Size =  10 Bytes



#define PEI_DEBUG_IDTBASEADDRx64			PEI_DEBUG_DATA_BASEADDRESS
#define PEI_DEBUG_CONTEXTADDRx64			PEI_DEBUG_DATA_BASEADDRESS + IDT_SIZEx64
#define PEI_DEBUG_XPORTLOCALVARADDRx64		PEI_DEBUG_CONTEXTADDRx64 + CONTEXT_SIZEx64
#define PEI_DEBUG_XPORTGLOBALVARADDRx64		PEI_DEBUG_XPORTLOCALVARADDRx64 + XPORTLOCALVAR_SIZEx64
#define PEI_DEBUG_DBGRLOCALVARADDRx64		PEI_DEBUG_XPORTGLOBALVARADDRx64 + XPORTGLOBALVAR_SIZEx64
#define PEI_DEBUG_XPORTEXPTNADDRx64			PEI_DEBUG_DBGRLOCALVARADDRx64 + DBGRLOCALVAR_SIZEx64
#define PEI_DEBUG_BUFADDRx64				PEI_DEBUG_XPORTEXPTNADDRx64 + XPORTEXPTNBUF_SIZEx64
#define	PEI_DEBUG_ACKADDRx64				PEI_DEBUG_BUFADDRx64 + GLOBALBUF_SIZEx64


#pragma pack (push, pei_struct_data_pack, 1)

typedef struct {
	unsigned char	ContextReg[CONTEXT_SIZEx64];
	unsigned char	XportLocalVar[XPORTLOCALVAR_SIZEx64];
	unsigned char	XportGlobalVar[XPORTGLOBALVAR_SIZEx64];
	unsigned char	DbgrLocalVar[DBGRLOCALVAR_SIZEx64];
	unsigned char	XportExptn[XPORTEXPTNBUF_SIZEx64];
	unsigned char	GlobalBuf[GLOBALBUF_SIZEx64];
	unsigned char	AckBuf[ACKBUF_SIZEx64];
}PEIDebugData_Tx64;


#pragma pack (pop, pei_struct_data_pack)


#pragma pack(1)
typedef struct {
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
} AMI_PEI_TRANSPORT_PROTOCOLx64;

//#pragma pack(1)
typedef struct {
	UINT64	m_EIP;
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
//For USB 3.0 Support
	UINT32	m_USB3HC;
	VOID*	m_DbgCapData;
}SerXPortGblData_Tx64;

typedef struct {
	AMI_PEI_TRANSPORT_PROTOCOLx64	*gTransportProtocol;
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
}SerXPortGblVar_Tx64;

typedef	struct{
	UINT8	Signature[4];
	VOID	(*ExceptionCallback)();
	VOID	(*SendMessage)();
	UINTN	(*PeiDbgr_EnableDebugSupport)(UINTN BrkPtAddr);
	VOID	(*INT1ExceptionHandler)();		//not to be use as API
	VOID	(*INT3ExceptionHandler)();		//not to be use as API
	VOID	(*INTnExceptionHandler)();		//not to be use as API
	UINTN	(*PeiDbgr_SendPeiLoadImage)(UINTN FileHandle,UINTN ImageAddr,UINTN ImageSize,UINTN EntryPoint,void * PDBFileName);
	VOID	(*INTFExceptionHandler)();		//not to be use as API
}AMI_DEBUGGER_INTERFACEx64;

typedef struct {
	UINT8	m_BrkPtType;
	UINT16	m_CS;
	UINT64	m_EIP;
	UINT16	m_SS;
	UINT64	m_ESP;
	UINT64	m_DR2;
	UINT64	m_DR7;
	UINT8	m_TargetRunning;

	UINT8	m_MSRState;
	UINT8	m_GPOccrd;

	UINT8	m_NotifyAndBreak;

	UINT8	m_DbgrBusy;
	UINT8	m_DR2Valid;
	UINT16	m_DbgSendBufSize;
	UINT16	m_DbgRcvBufSize;
	UINT64	m_BackupDR7;
	UINT8	m_PICMask;
	union{
	VOID	(*PeiDbgrReportStatusCode)();
	UINT64	res1;
	};
	UINT8	m_FlagToDebugAfterMemory;
	union{
	UINTN	(*PeiDbgr_EnableDebugSupport)(UINTN BrkPtAddr);
	UINT64	res2;
	};
	union{
	AMI_DEBUGGER_INTERFACEx64	*pPeiDbgDbgrIfc;
	UINT64	res3;
	};
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
	UINT8 	m_PerfDataAvailable;
	UINTN	m_SmmDebugDataAddress;
	UINT8	m_IsRuntime;
	UINT32	m_CarBaseAddress;
	UINT32	m_CarEndAddress;
	UINT8	m_isMemOpInProgress;
}DbgGblData_Tx64;
//#pragma pack()

//#pragma pack(1)
typedef struct {
	UINT16	m_OffLow;
	UINT16	m_SegSel;
	UINT16	m_Reserved;
	UINT16	m_OffHigh;
	UINT32	m_OffHighExt;
	UINT32	m_Reserved2;
}IDTEntry_Tx64;

typedef struct {
	UINT16	IDTLength;
	UINT64	IDTBaseAddr;	//should work for both 32 and 64 mode
}IDTBASEx64;
#pragma pack()

//=========================================================================
// Prototypes of structure definitions to be used handoff of 32 to 64 PEI
// Debugger
//=========================================================================
#pragma pack(1)
typedef struct {
	UINT64	gTransportProtocol;		//AMI_PEI_TRANSPORT_PROTOCOL	*gTransportProtocol;
	UINT64	Handler;				//UINT16 (*Handler)();

	//Additional parameters added in order to support DXE phase callback of
	//IRQ register handlers
	UINTN	InterruptType;
	UINTN	ContextData;
	UINT64	IRQRegisterHandler;			//VOID	(*IRQRegisterHandler)(UINTN,UINTN);

////#if USB_DEBUG_TRANSPORT
	UINT64	PeiDbgSIO_Init_USB_EHCI;	//void	(*PeiDbgSIO_Init_USB_EHCI)(VOID *);
//#endif
}SerXPortGblVar_Tx64_Proto;

typedef	struct{
	UINT8	Signature[4];
	UINT64	pExceptionCallback;				//VOID	(*ExceptionCallback)();
	UINT64	pSendMessage;					//VOID	(*SendMessage)();
	UINT64	pPeiDbgr_EnableDebugSupport;	//UINTN	(*PeiDbgr_EnableDebugSupport)(UINTN BrkPtAddr);
	UINT64	pINT1ExceptionHandler;			//VOID	(*INT1ExceptionHandler)();
	UINT64	pINT3ExceptionHandler;			//VOID	(*INT3ExceptionHandler)();
	UINT64	pINTnExceptionHandler;			//VOID	(*INTnExceptionHandler)();
	UINT64  pPeiDbgr_SendPeiLoadImage;		//UINTN	(*PeiDbgr_SendPeiLoadImage)(UINTN FileHandle,UINTN ImageAddr,UINTN ImageSize,UINTN EntryPoint,void * PDBFileName);
	UINT64	pINTFExceptionHandler;			//VOID	(*INTFExceptionHandler)();
}AMI_DEBUGGER_INTERFACEx64_Proto;

typedef struct {
	UINT8	Signature[4];
	UINT64	pInitialize;					//UINT16	(*Initialize)();
	UINT64	pRestore;						//UINT16	(*Restore)();
	UINT64	pSend;							//UINT16	(*Send)();
	UINT64	pReceive;						//UINT16	(*Receive)();
	UINT64	pSyncPacketNumber;				//UINT16	(*SyncPacketNumber)();
	UINT64	pRegisterHandler;				//UINT16  (*RegisterHandler)();
	UINT64	pCallback;						//VOID	(*Callback)();
	UINT64	pSendNoAck;						//UINT16	(*SendNoAck)();
	UINT64	pConsoleIn;						//	EFI_STATUS	(*ConsoleIn)();
	UINT64	pSendExptn;						//	VOID 	(*SendExptn)();
	UINT64	pIrqHandler;					//VOID	(*IrqHandler)();
} AMI_PEI_TRANSPORT_PROTOCOLx64__Proto;
#pragma pack()

//=========================================================================
//#define SET_SWBRKPT	_SetBrkPt:0xcc;
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
