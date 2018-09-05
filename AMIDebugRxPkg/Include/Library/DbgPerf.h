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
// $Header: /AptioV/SRC/AMIDebugRxPkg/Include/Library/DbgPerf.h 1     11/02/12 10:07a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:07a $
//*****************************************************************
//*****************************************************************
//
//	DbgPerf.h
//
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRxPkg/Include/Library/xportcmd.h $
//
//*****************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          DbgPerf.h
//
// Description:   Debug performance Lib Structures
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include "Token.h"
#include <Protocol/Performance.h>

#if defined RECORD_VERSION && (RECORD_VERSION >= 2)
#define EFI_PERF_PEI_ENTRY_MAX_NUM      100  
#else
#define EFI_PERF_PEI_ENTRY_MAX_NUM      49  
#endif

UINT16 PerfPeiEntryMaxNum = EFI_PERF_PEI_ENTRY_MAX_NUM;

typedef struct {
    CHAR8   Token[EFI_PERF_PDBFILENAME_LENGTH];
    EFI_GUID GUID;
    UINT64  StartTime;
    UINT64  EndTime;
    UINT64  Duration;
} EFI_PERF_DATA;

typedef struct {
    UINT64        S3Resume;	// Total time
    UINT32        S3EntryNum;	// Module numbers
    UINT32        S3ResumeNum;	// S3 resume numbers
    UINT64        S3ResumeAve;	// Average time
    EFI_PERF_DATA S3Entry[EFI_PERF_PEI_ENTRY_MAX_NUM];
    UINT64        CpuFreq;	// CPU frequency
    UINT32        Count;	// Total module number
    UINT32        Signiture;	// Signiture,
} EFI_PERF_HEADER;

#pragma    pack(push, 1)
typedef struct _EFI_PERF_S3_INFO{
    UINT64        S3Resume;    /// Total time
    UINT32        S3EntryNum;    /// Module numbers
    UINT32        S3ResumeNum;    /// S3 resume numbers
    UINT64        S3ResumeAve;    /// Average time
    UINT16        S3EntryReservedSpaceNumber;   /// EFI_PERF_PEI_ENTRY_MAX_NUM number, AMIPerfRecordDump can easy to know how many space reseverved for S3 resume data.
} EFI_PERF_S3_INFO;
//    EFI_PERF_DATA S3Entry[EFI_PERF_PEI_ENTRY_MAX_NUM];
typedef struct _EFI_PERF_INFO{
    UINT64        CpuFreq;    // /CPU frequency
    UINT32        Count;    /// Total module number
    UINT32        Signiture;    /// Signiture,
} EFI_PERF_INFO;

typedef	struct {	
	UINT32			        Signature;		    //0
	UINT32			        Length;			    //4        
} PERF_TAB_HEADER;
#pragma pack(pop)

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
