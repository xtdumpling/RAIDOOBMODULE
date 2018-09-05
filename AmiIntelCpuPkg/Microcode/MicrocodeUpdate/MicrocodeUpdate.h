//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//****************************************************************************
// $Header: /Alaska/SOURCE/CPU/Intel/Nehalem/MicrocodeUpdate.h 4     5/05/09 7:40p Markw $
//
// $Revision: 4 $
//
// $Date: 5/05/09 7:40p $
//
//****************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		MicrocodeUpdate.h
//
// Description:	  Header file for Microcode Update SMI handler.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _MICROCODE_UPDATE_H_
#define _MICROCODE_UPDATE_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define SIG_PART_ONE        0x0494e5445 // 'INTE'
//#define SIG_PART_TWO        0x04c504550 // 'LPEP'


#define CARRY_FLAG 1

#define UC_LOADER_VERSION   1

#define UC_HEADER_VERSION   1
#define UC_LOADER_REVISION  1

#define UC_INT15_ENABLE         0x01

#define UC_PRESCENCE_TEST       0
#define UC_WRITE_UPDATE_DATA    1
#define UC_UPDATE_CONTROL       2
#define UC_READ_UPDATE_DATA     3

#define UC_SUCCESS              0x0000
#define UC_NOT_IMPLEMENTED      0x8600
#define UC_ERASE_FAILURE        0x9000
#define UC_WRITE_FAILURE        0x9100
#define UC_READ_FAILURE         0x9200
#define UC_STORAGE_FULL         0x9300
#define UC_CPU_NOT_PRESENT      0x9400
#define UC_INVALID_HEADER       0x9500
#define UC_INVALID_HEADER_CS    0x9600
#define UC_SECURITY_FAILURE     0x9700
#define UC_INVALID_REVISION     0x9800
#define UC_UPDATE_NUM_INVALID   0x9900
#define UC_NOT_EMPTY            0x9a00

typedef struct {
    UINT32  EAX;
    UINT32  EBX;
    UINT32  ECX;
    UINT32  EDX;
    UINT32  ESI;
    UINT32  EDI;
    UINT32  EFLAGS;
    UINT16  ES;
    UINT16  CS;
    UINT16  SS;
    UINT16  DS;
    UINT16  FS;
    UINT16  GS;
    UINT32  EBP;
} SMI_UC_DWORD_REGS;

#pragma pack(push, 1)

typedef struct {
    UINT32 HeaderVersion;
    UINT32 UpdateRevision;
    UINT32 Date;
    UINT32 CpuSignature;
    UINT32 Checksum;
    UINT32 LoaderRevison;
    UINT32 Flags:8;
    UINT32 RV3:24;
    UINT32 DataSize;
    UINT32 TotalSize;
    UINT32 RV4[3];
} MICROCODE_HEADER;

typedef struct {
    UINT32  CpuSignature;
    UINT32  Flags;
    UINT32  Checksum;
} PROC_SIG;

typedef struct {
    UINT32      Count;
    UINT32      Checksum;
    UINT8       Rsv[12];
    PROC_SIG    ProcSig[1];
} MICROCODE_EXT_PROC_SIG_TABLE;

#pragma pack(pop)

VOID PresenceTest(SMI_UC_DWORD_REGS *Regs);
VOID WriteUpdateData(SMI_UC_DWORD_REGS *Regs);
VOID UpdateControl(SMI_UC_DWORD_REGS *Regs);
VOID ReadUpdateData(SMI_UC_DWORD_REGS *Regs);

EFI_STATUS FwhErase(
  IN UINTN      Address,
  IN UINTN      NumberOfBytes
);

EFI_STATUS FwhWrite(
  IN UINT8      *Buffer, 
  IN UINTN      Address, 
  IN UINTN      NumberOfBytes
);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
