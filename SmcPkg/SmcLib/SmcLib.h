//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Review BMC OEM command.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/11/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/12/2016
//
//**************************************************************************//
#ifndef __SMC_LIB__H__
#define __SMC_LIB__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Uefi.h>
#include "Token.h"
#include "SmcLibCommon.h"
#include "SmcLibSmBios.h"
#if IPMI_SUPPORT
#include "SmcLibBmc.h"
#endif

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
