//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/05/2014
//
//****************************************************************************
//****************************************************************************

#ifndef __ASTVBIOS_DXE_LIB__H__
#define __ASTVBIOS_DXE_LIB__H__
#ifdef  __cplusplus
extern "C" {
#endif

UINTN
CheckTheVideoSelect (VOID);

VOID 
TraceRootPortAndAddonVGABus (VOID);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef  __cplusplus
}
#endif
#endif

