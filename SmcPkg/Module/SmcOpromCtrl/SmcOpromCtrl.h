//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Update module and support EFI driver control when CSM disabled.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Apr/01/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/09/2014
//
//****************************************************************************
//****************************************************************************

#ifndef	_H_SmcOpromCtrl_
#define	_H_SmcOpromCtrl_

// FIXME FIXME! Double check the GUID!!!

typedef struct {
  LIST_ENTRY              List;
  EFI_HANDLE             Handle;
} PCIIO_HANDLE_NODE;

#define _CR(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#define GET_NODE_FROM_LINK(Link) _CR(Link, PCIIO_HANDLE_NODE, List)

EFI_STATUS SmcUefiOpRomCtrl(
    IN  EFI_HANDLE      PciHandle
);

#endif
