//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Initial version.
//      Reason:     Function improvement.
//      Auditor:    Neumann Lin
//      Date:       Jan/30/2015
//
//*****************************************************************************
#ifndef __EARLYVIDEO_TSE_LIB__H__
#define __EARLYVIDEO_TSE_LIB__H__

#ifdef  __cplusplus
extern "C" {
#endif

VOID
TsePostProgressMessage (
  IN  EFI_BOOT_SERVICES   *pBS,
  IN  CHAR16              *PostMessage
  );

#ifdef  __cplusplus
}
#endif

#endif // __EARLYVIDEO_TSE_LIB__H__
