//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  History
//
//  Rev. 1.00
//    Bug Fix : N/A
//    Reason  : Create a signal guid for OOB use after BDS connect all driver,
//    			since original guid not all platform signaled.
//    Auditor : Durant Lin
//    Date    : Jan/04/2018
///
//****************************************************************************
//****************************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <Library/DebugLib.h>

#define OOB_SIGNAL_EVENT_AFTER_BDS_CONNECT \
  {0x1A88a3b3, 0xBA70, 0x18A7, {0xAC, 0x55, 0x9a, 0x9c, 0xba, 0x9c, 0x9c, 0xa9}}

extern EFI_GUID gOobSignalEventAfterBdsConnect;


VOID OobSignalEventAfterBdsConnect (
  VOID
)
{
  EFI_HANDLE  Handle = NULL;
  
  DEBUG((-1,"Enter OobSignalEventAfterBdsConnect! \n"));

  pBS->InstallProtocolInterface (
        &Handle,
        &gOobSignalEventAfterBdsConnect,
        EFI_NATIVE_INTERFACE,
        NULL
        );
}

