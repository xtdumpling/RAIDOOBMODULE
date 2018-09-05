//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2014 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.01
//    Bug Fix:  OOB Function : Rewrite some code. Remove getting data from build time.
//    Reason:   Add debug messages for parsing HII.
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/15/2015
//
//  Rev. 1.00
//    Bug Fix:  Optimize OOB code.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/10/14
//
//***************************************************************************

#ifndef __SMCLIBMEMOPERATE_H__
#define __SMCLIBMEMOPERATE_H__

#include "EFI.h"


#define MAX_DYN_MEM_BUFFER_LIST_SIZE 51200

typedef EFI_STATUS (EFIAPI *SMCALLOCATEPOOL)(EFI_MEMORY_TYPE PoolType, UINTN Size, VOID **Buffer);
typedef EFI_STATUS (EFIAPI *SMCFREEPOOL)(VOID *Buffer);

UINT8* SmcAllocateMem (UINT32 Size);
VOID SmcFreeMem (UINT8* Point);

EFI_STATUS SmcInitMemOperateLib (SMCALLOCATEPOOL AlcPool, SMCFREEPOOL FrPool);
EFI_STATUS SmcFreeMemOperateLib ();

typedef struct {
    UINT8* Buffer;
    UINT32 Length;
    UINT32 MaxBufferLength;
} SmcDynMemBuffer;

EFI_STATUS SmcInitDynMemBuffer ();
EFI_STATUS SmcFreeDynMemBuffer ();

SmcDynMemBuffer* NewDynMem (UINT8* Source, UINT32 Size);
SmcDynMemBuffer* NewDynMemReserve (UINT32 Size);
VOID DelDynMem (SmcDynMemBuffer* DynMemBuffer);
VOID DynMemAppend (SmcDynMemBuffer* DynMemBuffer, UINT8* Source, UINT32 Size);
VOID DynMemAppendDynMem (SmcDynMemBuffer* DynMemBuffer, SmcDynMemBuffer* Source);
VOID DynMemAppendChar (SmcDynMemBuffer* DynMemBuffer, CHAR8 Chr);
VOID DynMemAppendStr (SmcDynMemBuffer* DynMemBuffer, CHAR8* Source);
SmcDynMemBuffer* SubDynMem (SmcDynMemBuffer* Source, UINT32 Pos, UINT32 Len);
INT32 DynMemRfind (SmcDynMemBuffer* Source, SmcDynMemBuffer* Buf);
UINTN DynMemAppendFs (SmcDynMemBuffer* Source, char *Format, ...);

UINT32 SmcGuidIsNull (EFI_GUID* Source);

#endif // __SMCLIBMEMOPERATE_H__

//****************************************************************************
