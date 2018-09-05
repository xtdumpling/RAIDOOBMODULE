//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file KcsBmc.h
    KCS BMC Controller functions.

**/

#ifndef _EFI_KCS_BMC_H
#define _EFI_KCS_BMC_H

#define KCS_WRITE_START         0x61
#define KCS_WRITE_END           0x62
#define KCS_READ                0x68
#define KCS_GET_STATUS          0x60  
#define KCS_ABORT               0x60

#define KCS_ABORT_RETRY_COUNT   10

//
//BT interface
//
#define CLR_WR_PTR_BIT    0x01
#define CLR_RD_PTR_BIT    0x02
#define H2B_ATN_BIT       0x04
#define B2H_ATN_BIT       0x08
#define H_BUSY            0x40
#define B_BUSY_BIT        0x80

typedef enum {
  KcsIdleState,
  KcsReadState,
  KcsWriteState,
  KcsErrorState
} KCS_STATE;

typedef union {
  UINT8     RawData;
  struct {
  UINT8     Obf:1;
  UINT8     Ibf:1;
  UINT8     SmAtn:1;
  UINT8     CD:1;
  UINT8     Oem1:1;
  UINT8     Oem2:1;
  UINT8     State:2;
  } Status;
} EFI_KCS_STATUS;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
