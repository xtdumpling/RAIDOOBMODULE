//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  
//    Reason:   For platform handle special nvram callback.
//    Auditor:  Durant Lin
//    Date:     Jan/17/2018
//
//****************************************************************************

#ifndef	_H_PLATFORM_SPECIFIC_FUNC_LIB_
#define	_H_PLATFORM_SPECIFIC_FUNC_LIB_

#include "Token.h"
#include <AmiDxeLib.h>
#include <Setup.h>

#if defined(MEHLOW_SGX_EXPOSE_SUPPORT) && (MEHLOW_SGX_EXPOSE_SUPPORT == 1)
#include <Library/RngLib.h>
#endif

#endif // _H_OobLibrary_
