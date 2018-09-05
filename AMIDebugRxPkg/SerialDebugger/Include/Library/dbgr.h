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
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/dbgr.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
//	dbgr.h
//		
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/dbgr.h $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     1/23/14 4:37p Sudhirv
// EIP I51926 - Serial Debugger - make Serial Debugger independent of
// AMIDebugRx and Debuger
// 
// 3     9/21/13 4:26a Sudhirv
// Updated for AMI Debug for UEFI 3.01.0006 Label.
// 
// 2     5/15/13 2:53p Sudhirv
// Updated for x86 DXE support patch binary generation.
// 
// 1     11/02/12 10:05a Sudhirv
// Debugger eModule for AMI Debug for UEFI 3.0
// 
// 3     8/08/12 4:02p Sudhirv
// Updated after Binaries Prepared.
// 
// 2     7/16/12 9:39p Sudhirv
// Updated
// 
// 1     7/15/12 8:16p Sudhirv
// Debugger Module for Aptio 5
// 
//*****************************************************************

#define	FXSTOR_EDI(Addr)\
	_asm	mov	edi, Addr\
	_asm	_emit	0fh\
	_asm	_emit	0aeh\
	_asm	_emit	07h

#define	FXRSTOR_ESI(Addr)\
	_asm	mov	esi, Addr\
	_asm	_emit	0fh\
	_asm	_emit	0aeh\
	_asm	_emit	0eh

/*
#ifdef	USB_DEBUGGER

#define	DISABLE_IRQS	\
	_asm	mov	al,0xFF	\
	_asm	out	0x21,al

#define	ENABLE_TIMER	\
	_asm	mov al,0xFE	\
	_asm	out 0x21,al

#else

#define	DISABLE_IRQS

#define	ENABLE_TIMER

#endif
*/

//#pragma pack(1)
/*
typedef struct {
	UINT8	m_BrkPtType;
	UINT16	m_CS;
	UINT32	m_EIP;
	UINT16	m_SS;
	UINT32	m_ESP;
	UINT8	m_TargetRunning;
	UINT8	m_DbgrBusy;
//	UINT8	m_DbgSendBufSize;		//error!!! cannot send more then 256 bytes
	UINT16	m_DbgSendBufSize;
//	UINT8	m_DbgRcvBufSize;
	UINT16	m_DbgRcvBufSize;
	VOID	(*PeiDbgrReportStatusCode)();
}DbgGblData_T;

typedef struct {
	UINT16	m_OffHigh;
	UINT16	m_Reserved;
	UINT16	m_SegSel;
	UINT16	m_OffLow;
}IDTEntry_T;
*/
typedef struct {
	UINT32	m_Dr0;
	UINT32	m_Dr1;
	UINT32	m_Dr2;
	UINT32	m_Dr3;
	UINT32	m_Dr4;
	UINT32	m_Dr5;
	UINT32	m_Dr6;
	UINT32	m_Dr7;
	UINT32	m_Cr0;
	UINT32	m_Cr1;
	UINT32	m_Cr2;
	UINT32	m_Cr3;
	UINT32	m_Cr4;
	UINT32	m_gs;
	UINT32	m_fs;
	UINT32	m_es;
	UINT32	m_ds;
	UINT32	m_ss;
	UINT32	m_Edi;
	UINT32	m_Esi;
	UINT32	m_Ebp;
	UINT32	m_Esp;	
	UINT32	m_Ebx;	
	UINT32	m_Edx;	
	UINT32	m_Ecx; 	
	UINT32	m_Eax;
	UINT32	m_EIP;
	UINT32	m_CS;
	UINT32	m_EFlags;
}ContextData_T;

//#pragma pack()

void InitDbgLayer (UINT32, UINT32);
void RegXportExptnHndlr (VOID*);
void SetBrkPtHndlr (UINT32 ); 
void SetDbgHndlr (UINT32 ); 
void SetBrkHndlr (UINT32 ); 
void SetTmrHndlr (UINT32 );

//Added for ReadMSR/WriteMSR exception handling support
//void SetGPHndlr (UINT32 );
void GpExptnHndlr (); 
void GpExptnHndlrx64(void); 
void MsrExptnHndlr();

void DbgInitGblData (VOID*); 
void DbgCopyIntoRAM (); 
void DbgExptnHndlr ();
void BreakpointHndlr ();
void fnBrkPtExptnHndlr();
void fnDbgExptnHndlr();


INTN AMIReadMem(VOID*);
INTN AMIWriteMem(VOID*);
INTN ReadDirIO(VOID*);
INTN WriteDirIO(VOID*);
INTN ReadPCI(VOID*); 
INTN WritePCI(VOID*);
INTN ReadCPUReg(VOID*);
INTN WriteCPUReg(VOID*);
INTN DebugReadMsr(VOID*);
INTN DebugWriteMsr(VOID*);
INTN GetFirstPCIDevInBus(VOID*);
INTN GetNextPCIDevInBus(VOID*);
VOID AMICopyMem(UINT8*, UINT8*, UINT32, UINT8);
UINT32 ReadVenDevId(UINT8,UINT8);
UINT8 ReadPCIReg(UINT8,UINT8,UINT8);
VOID DbgWritePCIRegByte(UINT8 ,UINT8 ,UINT8 , UINT8);
VOID DbgWritePCIRegWord(UINT8 ,UINT8 ,UINT8 , UINT16);
VOID DbgWritePCIRegDWord(UINT8 ,UINT8 ,UINT8 , UINT32);
INTN ReadFloatReg(VOID*); 
INTN WriteFloatReg(VOID*);
INTN ReadDescTbl(VOID*);
INTN WriteDescTbl(VOID*);
INTN AppFileInfo(VOID*);
INTN Proceed(VOID*);
INTN DebugTrace(VOID*);
INTN SendDbgPkt(VOID*);
INTN RcvDbgPkt(VOID*);
INTN HandleDbgCmd(VOID*);
void CopyIntoContext(UINT32, UINT32);
VOID OnXPortExptn(UINT16);
VOID plSetContextBreakpoint(UINTN);
void DoSpecialBreakPoint(UINT32 EaxValue,UINT32 EdxValue);
INTN PciList(VOID*);
INTN PciCnt(VOID*); 
INTN ReadIIO(VOID*);
INTN WriteIIO(VOID*);
INTN CPUidInfo(VOID*);
INTN DbgSmmInfo(VOID*); 
INTN  GetHandles(VOID*);
INTN  DbgPerformanceData(VOID*);
INTN  DbgSmmBreakState(VOID*);

#ifndef	EFIx64
UINT16 ReadGPReg( VOID*,GPRegPacket * );
UINT16 ReadDbgReg(VOID*, DbgRegPacket * );
UINT16 ReadCRReg( VOID*,CRRegPacket * );
UINT16 ReadMMXReg( MMXRegPacket * );

UINT16 WriteGPReg( VOID*, GPRegPacket * );
UINT16 WriteDbgReg( VOID*, DbgRegPacket * );
UINT16 WriteCRReg( VOID*, CRRegPacket * );
UINT16 WriteMMXReg( MMXRegPacket * );
UINT16 ReadDescReg( DescRegPacket * );
#endif

//Procedures to perform ReadMSR/WriteMSR for x64 support - Sudhir
UINT64	ReadMSRQWord(UINT32 MsrIndex);
VOID	WriteMSRQWord(UINT32 MsrIndex,UINT64 Value);

// For IndirectIO Support
UINT8 	IIoReadByte(UINT16 DataPort, UINT16 IndexPort, UINT16 Port);
VOID 	IIoWriteByte(UINT16 DataPort, UINT16 IndexPort, UINT16 Port, UINT8 Value);
UINT16 	IIoReadWord(UINT16 DataPort, UINT16 IndexPort, UINT16 Port);
VOID 	IIoWriteWord(UINT16 DataPort, UINT16 IndexPort, UINT16 Port, UINT16 Value);
UINT32	IIoReadDword(UINT16 DataPort, UINT16 IndexPort, UINT16 Port);
VOID 	IIoWriteDword(UINT16 DataPort, UINT16 IndexPort, UINT16 Port, UINT32 Value);


//For CPUId Info Support
VOID GetCPUIIdnfo(UINT32,UINT32*);
UINT16 ReadFPUReg(UINT8*);
INTN ReadFloatReg();
INTN WriteFloatReg();
INTN ReadDescTbl();

VOID plSetHWBreakpoint(VOID*,UINTN);

INTN IsMemAddrValid (VOID*, UINTN);
INTN IsAddrValidInCAR (VOID*, UINTN); 
INTN IsAddrValidInCode (UINTN);
UINT32	IsDbgrInCAR(); 
UINTN	Get_CAR_EndAddr(VOID*);
UINTN	Get_Code_EndAddr();

void	ClearMemory(UINT8 * pMem,UINT16 size);
void	AMIMoveMemory(UINT8 *Dest, UINT8 * Src, UINT16 size);
void	ResetSystem();
//VOID	SendDebugText(UINT8 *DbgTxt,UINT16	StrSize);
VOID SendDebugText(UINT8 *DbgTxt,UINT16	StrSize, UINT16 CheckPoint);
VOID	UpdateDRState(VOID*);
void 	CallbackXport(VOID*);
UINT32	CompareMemory(UINT8 *b1, UINT8 *b2, UINT32 Size);
void 	EnableBreakpointOnIO();
void 	DisableBreakpointOnIO();
#ifndef EFIx64
void	UnHookDebugTextIfc(DbgGblData_T *DbgData);
void	HookupDebugTextIfc(DbgGblData_T *DbgData);
#else
void	UnHookDebugTextIfc(DbgGblData_Tx64 *DbgData);
void	HookupDebugTextIfc(DbgGblData_Tx64 *DbgData);
#endif
void	SetupDebugger(VOID*,UINTN BrkPtAddr);
UINTN	EnableDebugger(UINTN BrkPtAddr);

INTN SMRamOpen(VOID*);
INTN SMRamClose(VOID*);
UINTN	PeiDbgr_SendPeiLoadImage(UINTN FileHandle,UINTN ImageAddr,UINTN ImageSize,UINTN EntryPoint,void * PDBFileName);

INTN	GetTargetState(VOID*);
INTN	AllocMemory(VOID*);
INTN	SetupLoadedFV(VOID*);

//Added for StopDriver/StopNextDriver 
INTN NotifyAndBreak(VOID*);

/*
typedef	struct{
	UINT8	Signature[4];
	VOID	(*ExceptionCallback)();
	VOID	(*SendMessage)();
}AMI_DEBUGGER_INTERFACE;
*/

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
