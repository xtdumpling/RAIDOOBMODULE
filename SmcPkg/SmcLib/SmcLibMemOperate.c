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

#include "Token.h"
#include "AmiLib.h"
#include "SmcLibString.h"
#include "SmcLibMemOperate.h"


//-----------------------------------------------------------------------------

EFI_STATUS SmcInitMemOperateLib (SMCALLOCATEPOOL AlcPool, SMCFREEPOOL FrPool)
{
    //
    // Init functions.
    //

    SmcInitDynMemBuffer ();

    return EFI_SUCCESS;
}

EFI_STATUS SmcFreeMemOperateLib ()
{
    SmcFreeDynMemBuffer ();

    return EFI_SUCCESS;
}


SmcDynMemBuffer DynMemBufferList[MAX_DYN_MEM_BUFFER_LIST_SIZE];

EFI_STATUS SmcInitDynMemBuffer ()
{
    UINT32 ii = 0;

    for (ii = 0; ii < MAX_DYN_MEM_BUFFER_LIST_SIZE; ii++) {
        DynMemBufferList[ii].Buffer = NULL;
        DynMemBufferList[ii].Length = 0;
        DynMemBufferList[ii].MaxBufferLength = 0;
    }
    return EFI_SUCCESS;
}

EFI_STATUS SmcFreeDynMemBuffer ()
{
    UINT32 ii = 0;
    for (ii = 0; ii < MAX_DYN_MEM_BUFFER_LIST_SIZE; ii++) {
        if (DynMemBufferList[ii].Buffer != NULL) {
            SmcFreeMem (DynMemBufferList[ii].Buffer);
            DynMemBufferList[ii].Buffer = NULL;
            DynMemBufferList[ii].Length = 0;
            DynMemBufferList[ii].MaxBufferLength = 0;
        }
    }
    return EFI_SUCCESS;
}
SmcDynMemBuffer* NewDynMem (UINT8* Source, UINT32 Size)
{
    UINT32 ii = 0;

    for (ii = 0; ii < MAX_DYN_MEM_BUFFER_LIST_SIZE; ii++) {
        if (DynMemBufferList[ii].Buffer == NULL) {
            DynMemBufferList[ii].Length = Size;
            DynMemBufferList[ii].MaxBufferLength = Size * 2;
            DynMemBufferList[ii].Buffer = SmcAllocateMem (Size * 2);
            if (DynMemBufferList[ii].Buffer != NULL) {
                SmcMemSet (DynMemBufferList[ii].Buffer, 0, Size * 2);
                SmcMemCpy (DynMemBufferList[ii].Buffer, Source, Size);
            }
            return &(DynMemBufferList[ii]);
        }
    }
    return NULL;
}

SmcDynMemBuffer* NewDynMemReserve (UINT32 Size)
{
    UINT32 ii = 0;

    for (ii = 0; ii < MAX_DYN_MEM_BUFFER_LIST_SIZE; ii++) {
        if (DynMemBufferList[ii].Buffer == NULL) {
            DynMemBufferList[ii].Length = 0;
            DynMemBufferList[ii].MaxBufferLength = Size;
            DynMemBufferList[ii].Buffer = SmcAllocateMem (Size);
            SmcMemSet (DynMemBufferList[ii].Buffer, 0, Size);
            return &(DynMemBufferList[ii]);
        }
    }
    return NULL;
}

VOID DelDynMem (SmcDynMemBuffer* DynMemBuffer)
{
    if (DynMemBuffer->Buffer != NULL) SmcFreeMem (DynMemBuffer->Buffer);
    DynMemBuffer->Buffer = NULL;
    DynMemBuffer->Length = 0;
    DynMemBuffer->MaxBufferLength = 0;
}

VOID DynMemAppend (SmcDynMemBuffer* DynMemBuffer, UINT8* Source, UINT32 Size)
{
    UINT8* pTmp = NULL;
    UINT32 NewLength = DynMemBuffer->Length + Size;

    if (NewLength > DynMemBuffer->MaxBufferLength) {
        pTmp = SmcAllocateMem (NewLength * 2);
        SmcMemSet (pTmp, 0, NewLength * 2);
        SmcMemCpy (pTmp, DynMemBuffer->Buffer, DynMemBuffer->Length);
        SmcMemCpy (pTmp + DynMemBuffer->Length, Source, Size);
        SmcFreeMem (DynMemBuffer->Buffer);
        DynMemBuffer->Buffer = pTmp;
        DynMemBuffer->Length = NewLength;
        DynMemBuffer->MaxBufferLength = NewLength * 2;
    }
    else {
        SmcMemCpy (DynMemBuffer->Buffer + DynMemBuffer->Length, Source, Size);
        DynMemBuffer->Length = NewLength;
    }
}

VOID DynMemAppendDynMem (SmcDynMemBuffer* DynMemBuffer, SmcDynMemBuffer* Source)
{
    DynMemAppend (DynMemBuffer, Source->Buffer, Source->Length);
}

VOID DynMemAppendChar (SmcDynMemBuffer* DynMemBuffer, CHAR8 Chr)
{
    UINT8 Tmp[1];

    Tmp[0] = Chr;

    DynMemAppend (DynMemBuffer, Tmp, 1);
}

VOID DynMemAppendStr (SmcDynMemBuffer* DynMemBuffer, CHAR8* Source)
{
    DynMemAppend (DynMemBuffer, Source, SmcStrLen (Source));
}

SmcDynMemBuffer* SubDynMem (SmcDynMemBuffer* Source, UINT32 Pos, UINT32 Len)
{
    UINT32 ModifyLen = Len;

    if (Pos + Len > Source->Length) ModifyLen = Source->Length - Pos;

    return NewDynMem (Source->Buffer + Pos, ModifyLen);
}

INT32 DynMemRfind (SmcDynMemBuffer* Source, SmcDynMemBuffer* Buf)
{
    UINT32 ii = 0;
    UINT32 jj = 0;
    UINT32 Pos = Source->Length - 1;
    UINT8 NoMatch = 0;

    if (Buf->Length > Source->Length) return -1;

    for (ii = 0; ii < Source->Length; ii++) {
        NoMatch = 0;
        Pos = Source->Length - ii - 1;
        for (jj = 0; jj < Buf->Length; jj++) {
            if (Buf->Buffer[jj] != Source->Buffer[Pos + jj]) {
                NoMatch = 1;
                break;
            }
        }
        if (NoMatch == 0)
            break;
    }
    if (NoMatch == 0)
        return Pos;
    else
        return -1;
}

//
// The format string size limit is 1024.
//

UINTN DynMemAppendFs (SmcDynMemBuffer* Source, char *Format, ...)
{
    UINT32 Size = 1024;
    UINT8* pTmp = NULL;
    UINT32 ReSize = 0;
    va_list	ArgList;

    if (Format == NULL) return 0;

    ArgList = va_start (ArgList, Format);

    pTmp = SmcAllocateMem (Size + 1);
    SmcMemSet (pTmp, 0, Size + 1);

    ReSize = (UINT32)Sprintf_va_list (pTmp, Format, ArgList);

    DynMemAppend (Source, pTmp, ReSize);

    if (pTmp!= NULL) SmcFreeMem (pTmp);

    va_end(ArgList);

    return ReSize;
}


UINT32 SmcGuidIsNull (EFI_GUID* Source)
{
    UINT32 ii = 0;
    UINT8* pSource = (UINT8*)Source;
    for (ii = 0; ii < sizeof (EFI_GUID); ii++) {
        if (pSource[ii] != 0) return 0; // Not null
    }
    return 1; // Is null
}

