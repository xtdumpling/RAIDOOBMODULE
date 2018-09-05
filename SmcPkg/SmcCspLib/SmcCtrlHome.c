//****************************************************************************
//**                                                                        **
//**           Copyright(c) 1993-2017 Super Micro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fixed: Remove Ctrl home feature.
//    Reason   : 
//    Auditor  : Chen Lin
//    Date     : Jul/17/2017
//
//
//  Rev. 1.00
//    Bug Fixed: Adding the feature - Pressing Ctrl+Home to enter BIOS recovery mode.
//    Reason   : It is a SMC feature.
//    Auditor  : Yenhsin Cho
//    Date     : Apr/07/2017
//
//****************************************************************************

#include <Library/IoLib.h>  // Rev. 1.00 (Ctrl+Home) ++
#include <Token.h>          // Rev. 1.00 (Ctrl+Home) ++

// Rev. 1.00 (Ctrl+Home) ++ >>
//BOOLEAN
//SmcCtrlHomePressed (
//  EFI_PEI_SERVICES  **PeiServices
//  )
//{
//  IoWrite8(0x72, CMOS_CTRL_HOME_FLAG);
//  if(IoRead8(0x73) == 0x55) {
//    IoWrite8(0x73, 0xAA);
//    return TRUE;
//  }
//  
//  return FALSE;
//}
//// Rev. 1.00 (Ctrl+Home) ++ <<

