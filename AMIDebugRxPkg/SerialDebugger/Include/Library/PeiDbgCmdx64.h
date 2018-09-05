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
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/PeiDbgCmdx64.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/PeiDbgCmdx64.h $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     1/23/14 4:37p Sudhirv
// EIP I51926 - Serial Debugger - make Serial Debugger independent of
// AMIDebugRx and Debuger
// 
// 1     11/02/12 10:05a Sudhirv
// Debugger eModule for AMI Debug for UEFI 3.0
// 
// 2     7/16/12 9:39p Sudhirv
// Updated
// 
// 1     7/15/12 8:16p Sudhirv
// Debugger Module for Aptio 5
// 
//*****************************************************************


#define HW_BREAKPOINT_x64		2	//changed from 00 to indicate it is EFIx64
#define SW_BREAKPOINT_x64		3	//changed from 1 to indicate it is EFIx64

#pragma pack(1)
typedef struct {
	UINT64  m_R8;
	UINT64  m_R9;
	UINT64  m_R10;
	UINT64  m_R11;
	UINT64  m_R12;
	UINT64  m_R13;
	UINT64  m_R14;
	UINT64  m_R15;
	UINT64	m_Cr8;

	UINT64	m_Dr0;
	UINT64	m_Dr1;
	UINT64	m_Dr2;
	UINT64	m_Dr3;
	UINT64	m_Dr4;
	UINT64	m_Dr5;
	UINT64	m_Dr6;
	UINT64	m_Dr7;
	UINT64	m_Cr0;
	UINT64	m_Cr1;
	UINT64	m_Cr2;
	UINT64	m_Cr3;
	UINT64	m_Cr4;
	UINT64	m_gs;
	UINT64	m_fs;
	UINT64	m_es;
	UINT64	m_ds;
//	UINT64	m_ss;
	UINT64	m_Edi;
	UINT64	m_Esi;
	UINT64	m_Ebp;
//	UINT64	m_Esp;		
	UINT64	m_Ebx;	
	UINT64	m_Edx;	
	UINT64	m_Ecx; 	
	UINT64	m_Eax;
	UINT64	m_EIP;
	UINT64	m_CS;
	UINT64	m_EFlags;
	UINT64	m_Esp;
	UINT64	m_ss;

}ContextDataX64_T;

typedef struct
{
	UINT8	Cmd;
	UINT8	BrkPtType;
	UINT64	BrkPtOff;
	UINT16	BrkPtSeg;
}
BIOSBrkPointX64;

typedef struct
{
	UINT64	DR0;
	UINT64	DR1;
	UINT64	DR2;
	UINT64	DR3;
	UINT64	DR4;
	UINT64	DR5;
	UINT64	DR6;
	UINT64	DR7;

	UINT16	GS;
	UINT16	FS;
	UINT16	ES;
	UINT16	DS;

	UINT64	RDI;
	UINT64	RSI;
	UINT64	RBP;
	UINT64	RSP;
	UINT64	RBX;
	UINT64	RDX;
	UINT64	RCX;
	UINT64	RAX;

	UINT16	SS;
	UINT64	RIP;
	UINT16	CS;
	UINT64	Rflags;

	UINT64  R8;
	UINT64  R9;
	UINT64  R10;
	UINT64  R11;
	UINT64  R12;
	UINT64  R13;
	UINT64  R14;
	UINT64  R15;
}
GPRegPacketX64;

typedef struct
{
	UINT64	MM0;
	UINT64	MM1;
	UINT64	MM2;
	UINT64	MM3;
	UINT64	MM4;
	UINT64	MM5;
	UINT64	MM6;
	UINT64	MM7;
}
MMXRegPacketX64;

typedef struct
{
	UINT64	DR0;
	UINT64	DR1;
	UINT64	DR2;
	UINT64	DR3;
	UINT64	DR4;
	UINT64	DR5;
	UINT64	DR6;
	UINT64	DR7;
}
DbgRegPacketX64;

typedef struct
{
	UINT64	CR0;
	UINT64	CR1;
	UINT64	CR2;
	UINT64	CR3;
	UINT64	CR4;
	UINT64  CR8;
}
CRRegPacketX64;

typedef struct
{
	UINT64	Gdtr[2];
	UINT64	Idtr[2];
	UINT64	Ldtr;
	UINT64	Tr;
}
DescRegPacketX64;

typedef struct {
        UINT8   Cmd;
        UINT8   ExptnType;
        UINT64  ExptnData;
        UINT16  CsVal;
        UINT64  EipVal;
}UnhandledExptnPacketX64;

typedef struct _DbgReadMemBlkx64	
{	UINT8 Cmd;
	UINT8 Attr;
	UINT64 StartAddr;
//	UINT32 StartOff;
//	UINT32 StartSeg;
	UINT8 MemPage;
	UINT16 BlkSize;
}DbgReadMemBlkX64;

typedef struct _DbgWriteMemBlkx64
{	UINT8 Cmd;
	UINT8 Attr;
	UINT64 StartAddr;
//	UINT32 StartOff;
//	UINT32 StartSeg;
	UINT8 MemPage;
	UINT16 BlkSize;
}DbgWriteMemBlkX64;

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
