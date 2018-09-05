//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaHssi.h

Abstract:

  Header file for the FPGA HSSI (High Speed Serial Interface) interface.

--*/

#ifndef _FPGA_HSSI_H_
#define _FPGA_HSSI_H_

#include "FpgaDxe.h"

typedef enum {
  Rcfg_Write        = 1,
  Rcfg_Read         = 2,
  Local_Read        = 0,
  Local_Write       = 1
} Bus_Cmd;

typedef enum {
  Led_R = 0,
  Prmgmt_Cmd        = 4,
  Prmgmt_Din        = 5,
  Prmgmt_Dout       = 6,
  Local_Cmd         = 7,
  Local_Din         = 8,
  Local_Dout        = 9
} Aux_Bus;

typedef enum {
  Tsd               = 0,
  Pll_Rst_Control   = 1,
  Pll_Locked_Status = 2,
  Prmgmt_Ram_Ena    = 3,
  Clk_Mon_Sel       = 4,
  Clk_Mon_Out       = 5,
  Recfg_Cmd_Addr    = 8,
  Recfg_Cmd_Wrdata  = 9,
  Recfg_Cmd_Rddata  = 10,
  Pll_Cmd_Addr      = 11,
  Pll_Write         = 12,
  Pll_T_Read        = 13,
  Pll_R_Read        = 14
} Local_Bus;

typedef enum {
  None              = 0x00,
  Sw_Read           = 0x08,
  Sw_Write          = 0x10,
  Aux_Read          = 0x40,
  Aux_Write         = 0x80
} Hssi_Cmd;

typedef enum {
  Change_Hssi_Mode  = 0x08,
  Tx_Eq_Write       = 0x09,
  Tx_Eq_Read        = 0x0A,
  Hssi_Init         = 0x0B,
  Hssi_Init_Done    = 0x0C,
  Fatal_Err         = 0x0D,
  Set_Hssi_Enable   = 0x0E,
  Get_Hssi_Enable   = 0x0F,
  Get_Hssi_Mode     = 0x10,
  Gbs_Service_Ena   = 0x11,
  Firmware_Version  = 0xFF
} Nios_Cmd;

typedef enum {
  SoftRegs0_SoftScratch = 0x00,
  SoftRegs1_SoftCmd,
  SoftRegs2_Arg0,
  SoftRegs3_Arg1,
  SoftRegs4_Arg2,
  SoftRegs5_Arg3,
  SoftRges6_Rval
} NIOS_Software_Registers;

typedef enum {
  HSSI_NACK         = 0,
  HSSI_ACK
} HSSI_ACKNOWLEDGE;

typedef union {
  struct {
    UINT32 addr  : 16;
    UINT32 cmd   : 16;
  } Bits;
  UINT32 Data;
} BUS_CMD_ADDR;

typedef enum {
  HSSI_MODE1        = 1,
  HSSI_MODE2        = 2,
  HSSI_MODE3        = 3
} FPGA_HSSI_MODE;

typedef struct {
  HSSI_CARD_ID      HssiCardId;
  FPGA_HSSI_MODE    HssiMode;
} FPGA_HSSI_MODE_MAP;

#define HSSI_XCVR_LANE_OFFSET        0x400

/**
  HSSI Transceiver (XCVR) Read Access Operation.

  @param[in]  SocketId          The index of CPU socket.
  @param[in]  Lane              The number of HSSI Lane.
  @param[in]  RegAddr           HSSI reconfigurable register address to read.
  @param[out] Value             The value of HSSI reconfigurable register.

  @retval EFI_SUCCESS           The function is executed successfully.
  @retval other                 Some error occurs when executing this access operation.

**/
EFI_STATUS
EFIAPI
HssiXcvrRead (
  IN  UINT8         SocketId,
  IN  UINT32        Lane,
  IN  UINT32        RegAddr,
  OUT UINT32        *Value
  );

/**
  HSSI Transceiver (XCVR) Write Access Operation.

  @param[in]  SocketId          The index of CPU socket.
  @param[in]  Lane              The number of HSSI Lane.
  @param[in]  RegAddr           HSSI reconfigurable register address to read.
  @param[in]  Value             The value of HSSI reconfigurable register.

  @retval EFI_SUCCESS           The function is executed successfully.
  @retval other                 Some error occurs when executing this access operation.

**/
EFI_STATUS
EFIAPI
HssiXcvrWrite (
  IN  UINT8         SocketId,
  IN  UINT32        Lane,
  IN  UINT32        RegAddr,
  IN  UINT32        Value
  );

/**
  Program FPGA HSSI congig data.

  @param[in]  VOID

  @retval EFI_SUCCESS             The function is executed successfully.
  @retval other                   Some error occurs when executing this operation.

**/
EFI_STATUS
EFIAPI
ProgramHssiConfigData (
  VOID
  );

#endif
