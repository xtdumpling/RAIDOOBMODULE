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
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/dbgcmd.h 2     1/23/14 5:44p Sudhirv $
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
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/dbgcmd.h $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     1/23/14 4:37p Sudhirv
// EIP I51926 - Serial Debugger - make Serial Debugger independent of
// AMIDebugRx and Debuger
// 
// 2     5/15/13 2:53p Sudhirv
// Updated for x86 DXE support patch binary generation.
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

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          dbgcmd.C
//
// Description:   Pei Debuglayer prototypes
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#pragma pack(1)

typedef struct DbgCmd
{	UINT8 Cmd;
}DbgCmd;

// Moved to xportcmd.h of DebugRx module.
/*
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

typedef struct {
	UINT8	Command;
	CPUInfo_T	CPUInfo;
	UINT8		ProjectTag[8]; 
	DateTime_T	RealTime;
	DateTime_T	BuildTime;
	UINT8		BiosCoreType;
}biosHandshake_S;

#define DBG_HANDSHAKE_ACK 0x1
#define DBG_HOST_TYPE_AMI_DEBUG_RX 			 0x01
#define DBG_HOST_TYPE_DEBUGGER_HOST_APP		 0x02

typedef struct DbgHandshakeAck	
{	UINT8 Cmd;
	UINT8 HostType;
	UINT8 Reserved[3];
}DbgHandshakeAck;
*/

#define DBG_PROCEED	0x2

typedef struct DbgProceed	
{	UINT8 Cmd;
}DbgProceed;

#define ATTR_MOVE_BYTE		1
#define ATTR_MOVE_WORD		2
#define ATTR_MOVE_DWORD		4

#define DBG_READ_DIR_IO_BLK	3
typedef struct DbgReadDirIoBlk	
{	UINT8 Cmd;
	UINT8 Attr;
	UINT16  StartPort;
	UINT16  BlkSize;
}DbgReadDirIoBlk;

#define BIOS_READ_DIR_IO_BLK_ACK 0x83
typedef struct BiosReadDirIoBlkAck	
{	UINT8 Cmd;
	UINT16 BlkSize;
}BiosReadDirIoBlkAck;

#define DBG_WRITE_DIR_IO_BLK	0x4	
typedef struct DbgWriteDirIoBlk	
{	UINT8 Cmd;
	UINT8 Attr;
	UINT16 StartPort;
	UINT16 BlkSize;
}DbgWriteDirIoBlk;

#define DBG_READ_MEM_BLK	0x5
typedef struct DbgReadMemBlk	
{	UINT8 Cmd;
	UINT8 Attr;
//	StartAddr	dq	?
	UINT32 StartOff;
	UINT32 StartSeg;
	UINT8 MemPage;
	UINT16 BlkSize;
}DbgReadMemBlk;

#define BIOS_READ_MEM_BLK_ACK	0x85
typedef struct BiosReadMemBlkAck	
{	UINT8 Cmd;
	UINT16 BlkSize;
}BiosReadMemBlkAck;

#define DBG_WRITE_MEM_BLK	6
typedef struct DbgWriteMemBlk
{	UINT8 Cmd;
	UINT8 Attr;
//	StartAddr	dq	?
	UINT32 StartOff;
	UINT32 StartSeg;
	UINT8 MemPage;
	UINT16 BlkSize;
}DbgWriteMemBlk;

#define BIOS_WRITE_MEM_BLK_ACK 	0x86		//new write mem. operation status from target - Ashrafj
typedef struct _BiosWriteMemBlkAck
{
	UINT8	Cmd;
	UINT8	WriteStatus;	// 0 - write mem success, 1 - write mem. fail
}BiosWriteMemBlkAck;

#define RT_GP_REG	0
#define RT_DBG_REG	1
#define RT_CR_REG	2
#define RT_MMX_REG  3

#define DBG_READ_CPU_REG	0x7	
typedef struct DbgReadCpuReg
{	UINT8 Cmd;
	UINT8 RegType;
}DbgReadCpuReg;

typedef struct CRReg_T	
{	UINT32 RegCR[5];
}CRReg_T;

typedef struct DbgReg_T
{	UINT32 RegDR[8];
}DbgReg_T;

typedef struct GpReg_T
{	UINT16 RegGS; 	
	UINT16 RegFS;	
	UINT16 RegES;	
	UINT16 RegDS;	
	UINT32  RegEdi;	
	UINT32  RegEsi;
	UINT32  RegEbp;
	UINT32  RegEsp;
	UINT32  RegEbx;
	UINT32  RegEdx;
	UINT32  RegEcx;
	UINT32  RegEax;
}GpReg_T;

typedef struct MMXReg_T
{   UINT32 RegMM0[2];
	UINT32 RegMM1[2];
	UINT32 RegMM2[2];
	UINT32 RegMM3[2];
	UINT32 RegMM4[2];
	UINT32 RegMM5[2];
	UINT32 RegMM6[2];
	UINT32 RegMM7[2];
}MMXReg_T; 

typedef struct RegContextInStack
{	CRReg_T		CRReg;
	DbgReg_T	DbgReg;
	GpReg_T		GpReg;
	MMXReg_T	MMXReg;
}RegContextInStack;

typedef struct GpRegContext
{	GpReg_T GpReg;
	UINT16 RegSS;		
	UINT32 RegEIP;		
	UINT16 RegCS;
	UINT32 EFlag;
}GpRegContext;

typedef struct DbgRegContext
{	DbgReg_T DbgReg;
}DbgRegContext;

typedef struct CRRegContext 
{	CRReg_T CRReg;		
}CRRegContext;

typedef struct MMXRegContext
{	MMXReg_T MMXReg;
}MMXRegContext;

#define BIOS_READ_CPU_REG_ACK	0x87
typedef struct BiosReadCpuRegAck
{	UINT8 Cmd;
	UINT8 CmdEnd;
	UINT16 CPURegBufSize;
}BiosReadCpuRegAck;

#define DBG_WRITE_CPU_REG	8
typedef struct DbgWriteCPUReg
{	UINT8 Cmd;
	UINT8 RegType;
	UINT8 CmdEnd;
}DbgWriteCPUReg;

#define BIOS_CHECKPOINT		0x89
typedef struct BiosCheckPoint
{   UINT8  Cmd;
	UINT16 CheckPoint;
}BiosCheckPoint;

#define BIOS_BREAKPOINT		0x8a
#define UNHANDLED_EXCEPTION 0x8e
#define HW_BREAKPOINT		00
#define SW_BREAKPOINT		1
#define MDL_LOAD			2

#define	EXPTN_STOP		0
#define	EXPTN_RESET		1


typedef  struct BIOSBrkPoint
{	UINT8 Cmd;
	UINT8 BrkPtType;
	UINT32 BrkPtOff;
	UINT32 BrkPtSeg;
}BIOSBrkPoint;

#define DBG_READ_PCI	0x0b
typedef struct DbgReadPCI
{	UINT8 Cmd;
	UINT8 BusNum;
	UINT8 DevNum;
	UINT8 FuncNum;
	UINT8 RegNum;
	UINT16 BlkSize;
}DbgReadPCI;

#define BIOS_READ_PCI_ACK	0x8b
typedef  struct BIOSReadPCIAck
{	UINT8 Cmd;
	UINT16 BlkSize;  
} BIOSReadPCIAck;

#define DBG_WRITE_PCI	0x0c
typedef struct DbgWritePCI
{	UINT8 Cmd;
	UINT8 BusNum;
	UINT8 DevNum;
	UINT8 FuncNum;
	UINT8 RegNum;
	UINT8 BlkSize;
}DbgWritePCI;

#define BIOS_MODULE_INFO 0x0d
typedef struct BIOSMdlInfo
{	UINT8 Cmd;
	UINT8 MdlNum;
	UINT16 PCIVenNum;
	UINT16 PCIDevNum;
	UINT32 MdlStartAddr;
	UINT16 MdlSize;
}BIOSMdlInfo;

#define BIOS_CODE_MOVED 0x0e
typedef struct BIOSCodeMoved
{	UINT8 Cmd;
	UINT32 SrcAddr;
	UINT32 DestAddr;
	UINT16 BlkSize;
}BIOSCodeMoved;

#define DBG_GET_FIRST_DEV_IN_BUS 0x0f
typedef struct DbgGetFirstDevInBus
{	UINT8 Cmd;
	UINT8 BusNum;
}DbgGetFirstDevInBus;

#define BIOS_GET_FIRST_DEV_IN_BUS_ACK 0x8f
typedef struct BIOSGetFirstDevInBusAck
{	UINT8 Cmd;
	UINT8 BusNum;
	UINT8 DevNum;
	UINT8 FuncNum;
}BIOSGetFirstDevInBusAck;

#define DBG_GET_NEXT_DEV_IN_BUS	0x10
typedef struct DbgGetNextDevInBus
{	UINT8 Cmd;
	UINT8 BusNum;
	UINT8 DevNum;
	UINT8 FuncNum;
}DbgGetNextDevInBus;

#define BIOS_GET_NEXT_DEV_IN_BUS_ACK	0x90
typedef struct BIOSGetNextDevInBusAck
{	UINT8 Cmd;
	UINT8 BusNum;
	UINT8 DevNum;
	UINT8 FuncNum;
}BIOSGetNextDevInBusAck;

#define DBG_TRACE 0x11
typedef struct DbgTrace
{	UINT8 Cmd;
}DbgTrace;

#define DBG_READ_MSR		0x12
typedef struct DbgReadMsr
{	UINT8 Cmd;
	UINT8 NumMsr;
}DbgReadMsr;

#define BIOS_READ_MSR_ACK 	0x92
typedef struct BIOSReadMsrAck
{	UINT8 Cmd;
	UINT8 GPState;					//Added for handling exception
}BIOSReadMsrAck;

#define DBG_WRITE_MSR  		0x13
typedef struct DbgWriteMsr
{  UINT8 Cmd;
   UINT8 NumMsr;
}DbgWriteMsr;

#define BIOS_WRITE_MSR_ACK 	0x93
typedef struct BIOSWriteMsrAck
{	UINT8 Cmd;
	UINT8 GPState;					//Added for handling exception
}BIOSWriteMsrAck;


#define DBG_READ_FLOAT_REG  0x16
typedef struct DbgReadFloatReg
{ UINT8 Cmd;
}DbgReadFloatReg;

typedef struct DbgReadFloatRegContext
{      UINT16 Fcw;            // dw  ? 
       UINT16 Fsw;            // dw      ?
       UINT8  Reserved0;      // db      ?
       UINT8  Ftw;            // db      ?
       UINT16 Fop;            // dw      ?
       UINT32  FpuIp;          // dd      ?
       UINT16 _Cs;            // dw      ?
       UINT16 Reserved1;      // dw      ?
       UINT32  FpuDp;          // dd      ?
       UINT16 _Ds;            // dw      ?
       UINT16 Reserved2;      // dw      ?
       UINT32  Mxcsr;          // dd      ?
       UINT32   Reserved3;      // dd      ?
       UINT8  _ST0[10];        //    db      10 dup(0)
       UINT8  ReservedST0[6];  //   db      6  dup(0)
       UINT8  _ST1[10];        //    db      10 dup(0)
       UINT8  ReservedST1[6];  //   db      6  dup(0)
       UINT8  _ST2[10];        //    db      10 dup(0)
       UINT8  ReservedST2[6];  //   db      6  dup(0)
       UINT8  _ST3 [10];       //    db      10 dup(0)
       UINT8  ReservedST3[6];  //   db      6  dup(0)
       UINT8  _ST4[10];        //    db      10 dup(0)
       UINT8  ReservedST4[6];  //   db      6  dup(0)
       UINT8  _ST5[10];        //    db      10 dup(0)
       UINT8  ReservedST5[6];  //   db      6  dup(0)
       UINT8  _ST6[10];        //    db      10 dup(0)
       UINT8  ReservedST6[6];  //   db      6  dup(0)
       UINT8  _ST7[10];         //   db      10 dup(0)
       UINT8  ReservedST7[6];   //  db      6  dup(0)
       UINT8  _XMM0[16];        //   db      16 dup(0)
       UINT8  _XMM1[16];        //   db      16 dup(0)
       UINT8  _XMM2[16];        //   db      16 dup(0)
       UINT8  _XMM3[16];         //  db      16 dup(0)
       UINT8  _XMM4[16];         //  db      16 dup(0)
       UINT8  _XMM5[16];        //   db      16 dup(0)
       UINT8  _XMM6[16];        //   db      16 dup(0)
       UINT8  _XMM7[16];        //   db      16 dup(0)
       UINT8  Reserved8[224];    //   db      224 dup(0)
}DbgReadFloatRegContext;

#define DBG_READ_FLOAT_REG_ACK  0x94
typedef struct DbgReadFloatRegAck
{  UINT8 Cmd;
   UINT16 Bufsize;
}DbgReadFloatRegAck;


#define DBG_WRITE_FLOAT_REG 0x17
typedef struct DbgWriteFloatReg
{  UINT8 Cmd;
   // ;;  512 Bytes of Data
}DbgWriteFloatReg;

#define DBG_READ_DESCRIPTOR_TABLE  0x18
typedef struct DbgReadDescriptorTable
{ UINT8 Cmd;
}DbgReadDescriptorTable;

typedef struct DbgReadDescriptorTableContext
{  UINT16 GDTLimit;
   UINT32  GDTBase; 
   UINT16 IDTLimit;        
   UINT32  IDTBase; 
   UINT16 LDTSelector;
   UINT16 TaskReg;    
}DbgReadDescriptorTableContext;

#define DBG_READ_DESCRIPTOR_TABLE_ACK  0x95
typedef struct DbgReadDescriptorTableAck
{    UINT8  Cmd;
     UINT16 Bufsize; 
}DbgReadDescriptorTableAck;

#define DBG_WRITE_DESCRIPTOR_TABLE 0x19
typedef struct DbgWriteDescriptorTable
{    UINT8  Cmd;
     UINT16 GDTLimit;
	 UINT32  GDTBase;
     UINT16 IDTLimit;        
     UINT32  IDTBase; 
     UINT16 LDTSelector;
     UINT16 TaskReg;        
}DbgWriteDescriptorTable;

#define DBG_READ_FILE_INFO	0x20
#define DBG_READ_FILE_INFO_ACK 0x0A0

typedef struct DbgFileInfoAck
{ UINT8 Cmd;
}DbgFileInfoAck;

typedef struct
{
	UINT64	Address;
	CHAR16	Name[1];
}
FileInfoPacket;


typedef struct DbgFileInfo
{	UINT32 Address[2];
	UINT16 Name;
}DbgFileInfo;

typedef struct {
        UINT8   Cmd;
        UINT32  ExptnType;
        UINT32  ExptnData;
        UINT32  CsVal;
        UINT32  EipVal;
}UnhandledExptnPacket;


typedef struct
{
	UINT32	DR0;
	UINT32	DR1;
	UINT32	DR2;
	UINT32	DR3;
	UINT32	DR4;
	UINT32	DR5;
	UINT32	DR6;
	UINT32	DR7;

	UINT16	GS;
	UINT16	FS;
	UINT16	ES;
	UINT16	DS;

	UINT32	EDI;
	UINT32	ESI;
	UINT32	EBP;
	UINT32	ESP;
	UINT32	EBX;
	UINT32	EDX;
	UINT32	ECX;
	UINT32	EAX;

	UINT16	SS;
	UINT32	EIP;
	UINT16	CS;
	UINT32	Eflags;
}
GPRegPacket;

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
MMXRegPacket;

typedef struct
{
	UINT32	DR0;
	UINT32	DR1;
	UINT32	DR2;
	UINT32	DR3;
	UINT32	DR4;
	UINT32	DR5;
	UINT32	DR6;
	UINT32	DR7;
}
DbgRegPacket;

typedef struct
{
	UINT32	CR0;
	UINT32	CR1;
	UINT32	CR2;
	UINT32	CR3;
	UINT32	CR4;
}
CRRegPacket;

typedef struct
{
	UINT32	Gdtr[2];
	UINT32	Idtr[2];
	UINT32	Ldtr;
	UINT32	Tr;
}
DescRegPacket;

#define DEBUG_TEXT_CMD 0x32		//For sending PEI status code
typedef struct DEBUG_TEXT
{
	UINT8 Cmd;
    UINT16 TxtSize;
}AMIDebugText;

#define DBG_ALIVE		0x0fe
#define BIOS_INVALID_COMMAND   0x0ff

//Commands for SMRAM
#define DBG_SM_RAM_OPEN      0x33
typedef struct {
	UINT8	Cmd;
}DbgSMRamOpen;

#define DBG_SM_RAM_OPEN_ACK   0xA3
typedef  struct {
	UINT8	Cmd;
	UINT8	Status;
	UINT8	NoSmmHandlers;
	UINT8	TotalSmmHandlers;
}DbgSMRamOpenAck;

#define DBG_SM_RAM_CLOSE      0x34
typedef struct {
	UINT8	Cmd;
} dbgSMRamClose;

#define DBG_SM_RAM_CLOSE_ACK   0xA4	
typedef struct {
	UINT8	Cmd; 
	UINT8	Status;
}DbgSMRamCloseAck;

#define LD_IMG_DATA	0x31
#define LD_IMG_DATA_ACK 0x0B1
typedef struct _LdImageData
{ UINT8 Cmd;
  UINT64 ImageBaseAddr;
  UINT64 ImageSize;
  UINT64 Handle;
  UINT16 ImageStrLen;
}LdImageData; 


#define DBG_GET_TARGET_STATE 	0x35
typedef struct 
{	UINT8 			Cmd;
} dbgGetTargetState;

#define DBG_GET_TARGET_STATE_ACK 0x0B5
typedef struct 
{	UINT8 			Cmd;
	UINT8			Status;
} dbgGetTargetStateAck;

#define DBG_ALLOCATE_MEM 		0x36
typedef struct 
{	UINT8 			Cmd;
	UINT32			MemSize;
} dbgAllocMem;

#define DBG_ALLOCATE_MEM_ACK	0x0B6
typedef struct 
{	UINT8 			Cmd;
	UINT8			Status;
	UINT64 			FVBase;
} dbgAllocMemAck;

#define DBG_SETUP_LOADED_FV		0x37
typedef struct 
{	UINT8 			Cmd;
	UINT64 			FVBase;
	UINT8			FVType;
} dbgSetupLoadedFV;

#define DBG_SETUP_LOADED_FV_ACK	0x0B7
typedef struct 
{	UINT8 			Cmd;
	UINT8			Status;
} dbgSetupLoadedFVAck;

// Added for StopDriver/StopNextDriver 

#define DBG_NOTIFY_AND_BREAK		0x38
typedef struct DbgNotifyAndBreak
{	UINT8	Cmd;
	UINT8	StatusFlag;
}DbgNotifyAndBreak;

#define BIOS_NOTIFY_AND_BREAK_ACK	0xb8
typedef struct BIOSNotifyAndBreakAck
{	UINT8	Cmd;
	UINT8	StatusFlag;
}BIOSNotifyAndBreakAck;

#define DBG_PCI_LIST		0x39
typedef struct DbgPciList
{	UINT8 Cmd;
	UINT8 BusNum;
	UINT8 DevNum;
	UINT8 FunNum;
	UINT16 devicecount;
	UINT16 BlkSize;
}DbgPciList;

#define BIOS_PCI_LIST_ACK	0xb9
typedef  struct BIOSPciListAck
{	UINT8 Cmd;
	UINT16 BlkSize;  
} BIOSPciListAck;

#define DBG_PCI_CNT		0x3a
typedef struct DbgPciCount
{	UINT8 Cmd;
}DbgPciCnt;

#define BIOS_PCI_CNT_ACK	0xba
typedef  struct BIOSPciCountAck
{	UINT8 Cmd;
	UINT16 count; 
}BIOSPciCntAck;


//Commands for ReadIIO and WriteIIO
#define DBG_READ_IIO_BLK		0x3b
typedef struct DbgReadIIoBlk	
{	UINT8 	Cmd;
	UINT8 	Attr;
	UINT16	DataPort;
	UINT16	IndexPort;
	UINT32 	StartPort;
	UINT8  	BlkSize;
}DbgReadIIoBlk;

#define BIOS_READ_IIO_BLK_ACK	 	0xbb
typedef struct BiosReadIIoBlkAck	
{	UINT8 	Cmd;
	UINT8 	BlkSize;
}BiosReadIIoBlkAck;

#define DBG_WRITE_IIO_BLK		0x3c
typedef struct DbgWriteIIoBlk	
{	UINT8 	Cmd;
	UINT8 	Attr;
	UINT16	DataPort;
	UINT16	IndexPort;
	UINT32  StartPort;
	UINT8   BlkSize;
}DbgWriteIIoBlk;

#define BIOS_WRITE_IIO_BLK_ACK	 	0xbc
typedef struct BiosWriteIIoBlkAck	
{	UINT8 	Cmd;
	UINT8 	BlkSize;
}BiosWriteIIoBlkAck;

#define DBG_CPUID_INFO 		    0x41

typedef struct BiosCPUIdInfo_S
{	UINT8 	Command;
}BiosCPUIdInfo_S;

typedef struct CPUIdInfo_T
{ 	UINT8 CpuName[48];
	UINT32 L2cache;	
	UINT32 Address;
}CPUIdInfo_T;

#define DBG_CPUID_INFO_ACK 	    0xc1 

typedef struct dbgCPUIdInfoAck_S
{	UINT8  Command;
        UINT8 	BlkSize;
	//CPUIdInfo_T  CpuIdInfo;
}dbgCPUIdInfoAck_S;

#define DBG_SMM_INFO_BLK		0x42
typedef struct DbgSmmInfoBlk	
{	UINT8 	Cmd;
}DbgSmmInfoBlk;

typedef struct SMMOutCtxt_T
{ 	UINT16  gSMIIOAddress;
    UINTN   gSMMBaseSaveState;
    UINTN   gSMMSaveStateSize;
    UINTN   gSMMBspBase;
    UINTN   gTsegSize;
}SMMOutCtxt_T;

#define BIOS_SMM_INFO_BLK_ACK	 	0xC2
typedef struct BiosSmmInfoBlkAck	
{	UINT8 	Cmd;
	UINT8 	BlkSize;
    //SMMOutCtxt_T    SMMInfo;
}BiosSmmInfoBlkAck;

#define DBG_GET_HANDLES 	0x40
typedef struct DbgGetHandles
{
	UINT8 Cmd;
	UINT8 NoofHandles;
	UINT8 HandleNo;
	EFI_GUID Guid;
}DbgGetHandles;

//EIP 81423 - Improve the EFI tab's Handle Display feature
#define BIOS_GET_HANDLES_ACK    0xC0
typedef struct BiosGetHandlesAck
{	UINT8 Cmd;
	UINT8 TargetType;
	UINT8 NoofHandles;
	UINT8 GuidData[900];
}BiosGetHandlesAck;

#define DBG_GET_PERF_DATA		0x43
#define BIOS_GET_PERF_DATA_ACK 	0xC3
typedef struct DbgPerfData
{	UINT8 Cmd;
}DbgPerfData; 

typedef struct BiosPerfDataAck
{
	UINT8 	Cmd;
	UINT8	Status;			//Error Status
	UINT8	Phase;			//PEI or DXE
	UINT32	TotalRecords;	//Total No of Records
	UINT64	StartAddress;	//Address of PM Records
	UINT64	CpuFreq;  		//CpuFrequency 
}BiosPerfDataAck; 

//Dbg Perf Error Status Definitions
#define DBG_PERF_SUCCESS	0x00
#define DBG_PERF_BS_NA    	0x01
#define	DBG_PERF_PROC_ERR	0x02
#define DBG_PERF_FREQ_ERR	0x03

#define DBG_SMM_BREAK_STATE		0x44
typedef struct DbgSmmBreakStatus
{	UINT8	Cmd;
	UINT8	SmmEntryFlag;
	UINT8	SmmExitFlag;
}DbgSmmBreakStatus;

#define BIOS_SMM_BREAK_STATE_ACK	0xC4
typedef struct BIOSSmmBreakStatusAck
{	UINT8	Cmd;
	UINT8	StatusFlag;
}BIOSSmmBreakStatusAck;

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
