//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Add function GetPurleyCPUType
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     Jun/08/2017
//
//  Rev. 1.02
//    Bug Fix:  Review BMC OEM command.
//    Reason:   Add function GetSystemConfig
//    Auditor:  Jimmy Chiu
//    Date:     Jul/28/2016
//
//  Rev. 1.01
//    Bug Fix:  Review BMC OEM command.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/11/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/26/2016
//
//****************************************************************************
//****************************************************************************

#ifndef __SMC_LIB_BMC_PRIVATE__H__
#define __SMC_LIB_BMC_PRIVATE__H__
#ifdef __cplusplus
extern "C" {
#endif
//
// SmcBmc private Prototype
//
EFI_STATUS
SMC_IPMICmd(
  IN  UINT8          NetFunction,
  IN  UINT8          Lun,
  IN  UINT8          Command,
  IN  UINT8          *CommandData,
  IN  UINT8          CommandDataSize,
  OUT UINT8         *ResponseData,
  OUT UINT8         *ResponseDataSize
);

EFI_STATUS
GetLanMacInfo(
  IN OUT UINT8         *LanMacData
);

EFI_STATUS
GetHardwareInfo(
  IN OUT UINT8         *HardWareData
);

EFI_STATUS
GetSystemConfig(
  IN OUT UINT8         *SysConfigData
);

UINT32
GetPurleyCPUType();

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************

