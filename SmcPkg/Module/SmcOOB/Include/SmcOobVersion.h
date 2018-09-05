//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//  Rev. 1.00
//    Bug fixed:
//    Reason   : For OOB module OBJ version control.
//    Auditor  :
//    Date     :
//
//****************************************************************************

#ifndef _SMC_OOB_VERVION_H_
#define _SMC_OOB_VERSION_H_

// Below string will insert into all EFI files.
// Modify this string have no effect, only for check use.
static const CHAR8	OOBVersion[] = "_SMCOOBV1.00.13SP_";


/*
 Rev 1.00.13
 1. Move Variable_GUID to PlatformPolicy.
 2. Add a protocol gSmcOobDataUploadBmcDoneGuid to inform OOB upload BMC done.
 3. Improve some functions procedure and add debug message.
 
 Rev 1.00.12
 1. Fix lost page problem to SUM.

 Rev 1.00.11
 1. Add a function hook before Update Bios Cfg to determine should update BIOS cfg.
 
 Rev 1.00.10
 1. Support Hide Form for SUM examine Form ID. <dynamic page problem.>
 
 Rev 1.00.09
 1. Add a function hook before LoadDefault.
 
 Rev 1.00.08
 1. Add SMCHttpBoot Support function.
 2. Modify AssertInfo read SATA controller type method.
 
 Rev 1.00.07
 1. 1. Roll back CalculateMD5 function from protocol to Library since unknow hang up problem.

 Rev 1.00.06
 1. Add other function in SMM and Dxe OobLibrary protocol.
 
 Rev 1.00.05
 1. Fix Inband Cannot write SMBIOS dmi into Romhole region in coffeelake.
 
 Rev 1.00.04
 1.  Add an interface to call platform specific SmmRomHoleLib.
 2.  Isolate IPMI support for no IPMI module platform InBand. 
 
 Rev 1.00.03
 1. Fix Inband SMM ram size not enough to suit coffeelake Bios Cfg.
 
 Rev 1.00.02
 1. Sync Purley OOB code to fix SUM ATT test case 306 failed problem when run BIOS ECO test.

 Rev 1.00.01
 1. Add a interface Smc_UpdatePlatformVariable to hook updatevariable in SmcInBand for platform special callback value.

 Rev 1.00.00
 1. First Version to add OOB version control.
*/

#endif

