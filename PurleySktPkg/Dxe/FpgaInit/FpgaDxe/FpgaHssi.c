/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2017 Intel Corporation.  All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaHssi.c

Abstract:

  This driver for FPGA HSSI Control Interface

--*/

#include "FpgaDxe.h"
#include "FpgaHssi.h"

//
// Modular variables needed by this driver
//
extern FPGA_CONFIGURATION            FpgaConfiguration;
extern UINTN                         FmeBar[MAX_SOCKET];

EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL   *FpgaHssiConfigDataProtocol;

FPGA_HSSI_MODE_MAP  FpgaHssiModeMap[] = {
  {HSSI_CardID_4x10GbE, HSSI_MODE1},
  {HSSI_CardID_2x40GbE, HSSI_MODE2},
  {HSSI_CardID_4x25GbE, HSSI_MODE3}
};

/**
  Polling the HSSI_STAT register acknowledge bit.

  @param[in]  SocketId          The index of CPU socket.
  @param[in]  Acknowledge       The acknowledge setting state.

  @retval EFI_SUCCESS           The function is executed successfully.
  @retval EFI_TIMEOUT           Time out when polling acknowledge bit.

**/
EFI_STATUS
EFIAPI
WaitForAck (
  IN UINT8             SocketId,
  IN HSSI_ACKNOWLEDGE  Acknowledge
  )
{
  UINT32                            StartCount;
  HSSI_STAT_N1_FPGA_FME_STRUCT      HssiStat_N1;

  StartCount = 500;

  if (SocketId >= MAX_SOCKET || FmeBar[SocketId] == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Write HSSI control data to HSSI_CTRL register, and polling the HSSI status 
  // HSSI_STAT register until the acknowledge (bit [32]) gets set or de-asserted.
  //
  while (TRUE) {
    HssiStat_N1.Data = MmioRead32 (FmeBar[SocketId] + HSSI_STAT_N1_FPGA_FME_REG);
    if (HssiStat_N1.Bits.acknowledge == (UINT32) Acknowledge) {
      return EFI_SUCCESS;
    }

    MicroSecondDelay (10);
    StartCount--;
    if (StartCount == 0) {
      DEBUG ((DEBUG_ERROR, "WaitForAck failed. HssiStat_N1.Data = %X\n", HssiStat_N1.Data));
      return EFI_TIMEOUT;
    }
  }
}

/**
  FPGA HSSI acknowledge polling function for set and de-assert.

  @param[in]  SocketId          The index of CPU socket.

  @retval EFI_SUCCESS           The function is executed successfully.
  @retval EFI_TIMEOUT           Time out when polling acknowledge bit.

**/
EFI_STATUS
EFIAPI
HssiAck (
  IN UINT8             SocketId
  )
{
  EFI_STATUS           Status;

  if ((SocketId >= MAX_SOCKET) || (FmeBar[SocketId] == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = WaitForAck (SocketId, HSSI_ACK);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, 0);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, 0);

  Status = WaitForAck (SocketId, HSSI_NACK);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Initiate the "goto mode" based on the Add in Card ID.
  This puts the HSSI transceivers in the corresponding configuration.

  @param[in]  SocketId          The number of CPU socket.
  @param[in]  HssiMode          The HSSI mode need to be changed.

  @retval EFI_SUCCESS           The function is executed successfully.
  @retval other                 Some error occurs when executing this operation.

**/
EFI_STATUS
EFIAPI
ChangeHssiMode (
  IN  UINT8         SocketId,
  IN  UINT8         HssiMode
  )
{
  EFI_STATUS                        Status;
  HSSI_CTRL_N0_FPGA_FME_STRUCT      HssiCtrl_N0;  // HSSI_CTRL[63:0]: command[63:48], address[42:32], data[31:0]
  HSSI_CTRL_N1_FPGA_FME_STRUCT      HssiCtrl_N1;

  if ((SocketId >= MAX_SOCKET) || (FmeBar[SocketId] == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // 1. Provide the arg0 (mode) to the soft register 0x2:
  // a.Write the soft register address (0x2), and the value (0x#) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an SOFT reg WR (10)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;

  HssiCtrl_N1.Bits.address = SoftRegs2_Arg0;
  HssiCtrl_N0.Data = (UINT32) HssiMode;
  HssiCtrl_N1.Bits.command = Sw_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 2. Provide the soft command ID.
  // a.Write the soft command ID for change_hssi_mode (8) to the soft command address (0x1) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an SOFT reg WR (10)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;

  HssiCtrl_N1.Bits.address = SoftRegs1_SoftCmd;
  HssiCtrl_N0.Data = Change_Hssi_Mode;
  HssiCtrl_N1.Bits.command = Sw_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                        Status;
  HSSI_CTRL_N0_FPGA_FME_STRUCT      HssiCtrl_N0;  // HSSI_CTRL[63:0]: command[63:48], address[42:32], data[31:0]
  HSSI_CTRL_N1_FPGA_FME_STRUCT      HssiCtrl_N1;
  BUS_CMD_ADDR                      BusCmdAddr;
  UINT32                            ReconfigAddr;

  if ((SocketId >= MAX_SOCKET) || (FmeBar[SocketId] == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  ReconfigAddr = RegAddr + HSSI_XCVR_LANE_OFFSET * Lane;

  //
  // 1. Provide the ID information of the reconfigurable register to the AUX bus handshake registers.
  // a.Write the RCFG_ADDR, RCFG_CHNL (0), RCFG_CMD (2 -> read) and AUX_ADDR (8) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an AUX reg WR (80)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;
  BusCmdAddr.Data  = 0;


  HssiCtrl_N1.Bits.address = Local_Din;
  BusCmdAddr.Bits.addr = ReconfigAddr;
  BusCmdAddr.Bits.cmd = Rcfg_Read;
  HssiCtrl_N0.Data = BusCmdAddr.Data;
  HssiCtrl_N1.Bits.command = Aux_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 2. Send the request to the AUX bus handshake registers, to write the ID information that was provided in step 1, into RCFG bus.
  // a.Write the LOCAL_ADDR (8) LOCAL_CMD (1 -> write), and AUX_ADDR (7) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an AUX reg WR (80)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;
  BusCmdAddr.Data  = 0;

  HssiCtrl_N1.Bits.address = Local_Cmd;
  BusCmdAddr.Bits.addr = Recfg_Cmd_Addr;
  BusCmdAddr.Bits.cmd = Local_Write;
  HssiCtrl_N0.Data = BusCmdAddr.Data;
  HssiCtrl_N1.Bits.command = Aux_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 3. Read data from LOCAL bus to AUX bus.
  // a.Write the LOCAL_ADDR (a), LOCAL_CMD (0 -> read) and AUX_ADDR (7) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an AUX reg WR (80)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;
  BusCmdAddr.Data  = 0;

  HssiCtrl_N1.Bits.address = Local_Cmd;
  BusCmdAddr.Bits.addr = Recfg_Cmd_Rddata;
  BusCmdAddr.Bits.cmd = Local_Read;
  HssiCtrl_N0.Data = BusCmdAddr.Data;
  HssiCtrl_N1.Bits.command = Aux_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 4. Read data from AUX bus to HSSI_STAT
  // a.Write the AUX_ADDR (9) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an AUX reg RD (40)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;
  BusCmdAddr.Data  = 0;

  HssiCtrl_N1.Bits.address = Local_Dout;
  HssiCtrl_N1.Bits.command = Aux_Read;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Value = MmioRead32 (FmeBar[SocketId] + HSSI_STAT_N0_FPGA_FME_REG);

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                        Status;
  HSSI_CTRL_N0_FPGA_FME_STRUCT      HssiCtrl_N0;  // HSSI_CTRL[63:0]: command[63:48], address[42:32], data[31:0]
  HSSI_CTRL_N1_FPGA_FME_STRUCT      HssiCtrl_N1;
  BUS_CMD_ADDR                      BusCmdAddr;
  UINT32                            ReconfigAddr;

  if ((SocketId >= MAX_SOCKET) || (FmeBar[SocketId] == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  ReconfigAddr = RegAddr + HSSI_XCVR_LANE_OFFSET * Lane;

  //
  // 1. Provide the DATA to be written into the reconfigurable register to the AUX bus handshake registers.
  // a.Write the DATA and AUX_ADDR (8) to the HSSI_CTRL register.
  // b. Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an AUX reg WR (80)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;
  BusCmdAddr.Data  = 0;

  HssiCtrl_N1.Bits.address = Local_Din;
  HssiCtrl_N0.Data = Value;
  HssiCtrl_N1.Bits.command = Aux_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 2. Send the request to the AUX bus handshake registers, to write the ID information that was provided in step 1, into RCFG bus.
  // a.Write the LOCAL_ADDR (9) LOCAL_CMD (1 -> write), and AUX_ADDR (7) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an AUX reg WR (80)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;
  BusCmdAddr.Data  = 0;

  HssiCtrl_N1.Bits.address = Local_Cmd;
  BusCmdAddr.Bits.addr = Local_Dout;
  BusCmdAddr.Bits.cmd = Local_Write;
  HssiCtrl_N0.Data = BusCmdAddr.Data;
  HssiCtrl_N1.Bits.command = Aux_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 3. Provide the ID information of the reconfigurable register to the AUX bus handshake registers.
  // a.Write the RCFG_ADDR (2e1), RCFG_CHNL (0), RCFG_CMD (1 -> write) and AUX_ADDR (8) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an AUX reg WR (80)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;
  BusCmdAddr.Data  = 0;

  HssiCtrl_N1.Bits.address = Local_Din;
  BusCmdAddr.Bits.addr = ReconfigAddr;
  BusCmdAddr.Bits.cmd = Local_Write;
  HssiCtrl_N0.Data = BusCmdAddr.Data;
  HssiCtrl_N1.Bits.command = Aux_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 4. Send the request to the AUX bus handshake registers, to write the ID information that was provided in step 3, into RCFG bus.
  // a.Write the LOCAL_ADDR (8) LOCAL_CMD (1 -> write), and AUX_ADDR (7) to the HSSI_CTRL register.
  // b.Write the same value to HSSI_CTRL register than in step (a), but now indicate that the HSSI_CMD is an AUX reg WR (80)
  // c.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets set.
  // d.Write all 0's to the HSSI_CTRL register
  // e.Poll the HSSI_STAT register until the acknowledge (bit [32]) gets de-asserted.
  //
  HssiCtrl_N0.Data = 0;
  HssiCtrl_N1.Data = 0;
  BusCmdAddr.Data  = 0;

  HssiCtrl_N1.Bits.address = Local_Cmd;
  BusCmdAddr.Bits.addr = Local_Din;
  BusCmdAddr.Bits.cmd = Local_Write;
  HssiCtrl_N0.Data = BusCmdAddr.Data;
  HssiCtrl_N1.Bits.command = Aux_Write;
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, HssiCtrl_N0.Data);
  MmioWrite32 (FmeBar[SocketId] + HSSI_CTRL_N1_FPGA_FME_REG, HssiCtrl_N1.Data);

  Status = HssiAck (SocketId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Program FPGA HSSI config data.

  @param[in]  VOID

  @retval EFI_SUCCESS             The function is executed successfully.
  @retval other                   Some error occurs when executing this operation.

**/
EFI_STATUS
EFIAPI
ProgramHssiConfigData (
  VOID
  )
{
  EFI_STATUS                   Status;
  UINT8                        SocketId;
  HSSI_CONFIG_DATA_TABLE       *FpgaHssiConfigDataTable;
  UINTN                        NumOfHssiConfigData;
  UINTN                        Index;

  DEBUG ((DEBUG_ERROR, "ProgramHssiConfigData() ...\n"));

  if (FpgaConfiguration.HssiEqTuningEn == 0) {
    DEBUG ((DEBUG_ERROR, "FPGA HSSI EQ tuning disabled, skip program.\n"));
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                  &gEfiFpgaHssiConfigDataProtocol,
                  NULL,
                  (VOID **) &FpgaHssiConfigDataProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate FPGA HSSI Config Data Protocol failed, Status = %r\n", Status));
    return Status;
  }

  //
  // Loop thru all FPGA of all sockets that are present
  //
  for (SocketId = 0; SocketId < MAX_SOCKET; SocketId++) {
    if (!(FpgaConfiguration.FpgaSktActive & (1 << SocketId))) {
      continue;
    }

    DEBUG ((DEBUG_ERROR, "FPGA[%X] HSSI Card ID = %X\n", SocketId, FpgaConfiguration.FpgaHssiCardID[SocketId]));
    if ((FpgaConfiguration.FpgaHssiCardID[SocketId] == 0) || (FpgaConfiguration.FpgaHssiCardID[SocketId] == HSSI_CardID_PCIe)) {
      continue;
    }

    //
    // Update HSSI EQ Parameters and initiates the "goto mode".
    //
    Status = FpgaHssiConfigDataProtocol->FpgaConfigData (
                                           FpgaHssiConfigDataProtocol,
                                           SocketId,
                                           &FpgaHssiConfigDataTable,
                                           &NumOfHssiConfigData
                                           );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Get Socket[%X] HSSI config data failed, Status = %r\n", SocketId, Status));
      continue;
    }

    DEBUG ((DEBUG_ERROR, "Number of Config Data Table: %X\n", NumOfHssiConfigData));
    for (Index = 0; Index < NumOfHssiConfigData; Index++) {
      DEBUG ((DEBUG_ERROR, "FpgaHssiConfigDataTable[%X]: %lX\n", Index, FpgaHssiConfigDataTable[Index].Data));
      MmioWrite64 (FmeBar[SocketId] + HSSI_CTRL_N0_FPGA_FME_REG, FpgaHssiConfigDataTable[Index].Data);

      Status = HssiAck (SocketId);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Index[%X] write ACK failed.\n", Index));
        break;
      }
    }
  }

  return Status;
}
