/*++
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c) 1996 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


Module Name:

  NvDimmAccess.c

Abstract:

  Implementation of JEDEC NVDIMM access routines

--*/

#include "JedecNvDimm.h"

SMB_CMD_CFG_0_PCU_FUN5_STRUCT SmbCmdSave;

/**

  Disable TSOD

  @param dev            - Device address on SMBUS
  @param Socket         - Socket ID

**/
VOID
DisableTsod (
   struct smbDevice             dev,
   UINT8                        Socket
  )
{
  UINT32                          smbCmdReg;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT   smbCmd;

  if (dev.address.busSegment == 0) {
    smbCmdReg   = SMB_CMD_CFG_0_PCU_FUN5_REG;
  } else {
    smbCmdReg   = SMB_CMD_CFG_1_PCU_FUN5_REG;
  }

  // Disable TSOD polling
  SmbCmdSave.Data = mCpuCsrAccess->ReadCpuCsr (Socket, 0, smbCmdReg);
  smbCmd.Data = SmbCmdSave.Data;
  if (smbCmd.Bits.smb_tsod_poll_en) {
    smbCmd.Bits.smb_tsod_poll_en = 0;
    mCpuCsrAccess->WriteCpuCsr(Socket, 0, smbCmdReg, smbCmd.Data);
  }
}

/**

  Restore TSOD setting

  @param dev            - Device address on SMBUS
  @param Socket         - Socket ID

**/
VOID
RestoreTsod (
    struct smbDevice             dev,
    UINT8                        Socket
  )
{
  UINT32                            smbCmdReg;

  if (dev.address.busSegment == 0) {
    smbCmdReg   = SMB_CMD_CFG_0_PCU_FUN5_REG;
  } else {
    smbCmdReg   = SMB_CMD_CFG_1_PCU_FUN5_REG;
  }

  // Restore TSOD setting
  mCpuCsrAccess->WriteCpuCsr(Socket, 0, smbCmdReg, SmbCmdSave.Data);
}

/**

  Gets the ACPI timer value

  @param host  - Pointer to the system host (root) structure

  @retval 24-bit counter value

**/
UINT32
GetCount ()
{
  UINT16 PmBase = 0;

  // PM base address
  PmBase = MmioRead16 (MmPciBase (0, 31, 2) + REG_ACPI_BASE_ADDRESS);
  PmBase = (UINT16) (PmBase & 0xFF80);

  return IoRead32 ((UINT16) (PmBase + IO_REG_ACPI_TIMER)) & 0xFFFFFF;
}

/**

  determines delay since the GetCount was called

  @param host        - Pointer to the system host (root) structure
  @param startCount  - 24-bit counter value from GetCount()

  @retval delay timein 1 us units

**/
UINT32
GetDelay (
   UINT32   startCount
   )
{
  UINT32  stopCount;

  stopCount = GetCount ();
  if (startCount > stopCount) {
    startCount |= 0xFF000000;
  }

  return((stopCount - startCount) * 1000U) / 3579U;
  //
  // 3.579 MHz clock
  //
}

/**

  Read SMBUS device at a given device and offset

  @param NvDimm         - NvDimm structure
  @param dev            - Device address on SMBUS
  @param byteOffset     - Offset in Device
  @param data           - Pointer to store value

  @retval 0 - Success
  @retval 1 - failure

**/
UINT32
ReadSmbWorkerFunction (
   NVDIMM                       *NvDimm,
   struct smbDevice             dev,
   UINT8                        byteOffset,
   UINT8                        *data
   )
{
  UINT32                    startCount = 0;
  UINT32                    smbCmdReg;
  UINT32                    smbStatReg;
  UINT32                    smbDataReg;
  SMB_STATUS_CFG_0_PCU_FUN5_STRUCT  smbStat;
  SMB_DATA_CFG_0_PCU_FUN5_STRUCT    smbData;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT     smbCmd;
  UINT32                    rval;

  rval    = SUCCESS;

  // Ensure TSOD polling is disabled before continuing
  DisableTsod(dev, NvDimm->SocketId);

  if (dev.address.busSegment == 0) {
    smbCmdReg   = SMB_CMD_CFG_0_PCU_FUN5_REG;
    smbStatReg  = SMB_STATUS_CFG_0_PCU_FUN5_REG;
    smbDataReg  = SMB_DATA_CFG_0_PCU_FUN5_REG;
  } else {
    smbCmdReg   = SMB_CMD_CFG_1_PCU_FUN5_REG;
    smbStatReg  = SMB_STATUS_CFG_1_PCU_FUN5_REG;
    smbDataReg  = SMB_DATA_CFG_1_PCU_FUN5_REG;
  }

  //
  // Form read command
  //
  smbCmd.Data = 0;
  smbCmd.Bits.smb_ckovrd = 1;
  smbCmd.Bits.smb_ba = (UINT32) byteOffset;
  smbCmd.Bits.smb_dti = dev.address.deviceType;
  smbCmd.Bits.smb_sa = (UINT32) dev.address.strapAddress;
  smbCmd.Bits.smb_wrt = 0;
  smbCmd.Bits.smb_pntr_sel = 0;
  if (dev.compId == MTS) {
    smbCmd.Bits.smb_word_access = 1;
  } else {
    smbCmd.Bits.smb_word_access = 0;
  }
  smbCmd.Bits.smb_cmd_trigger = 1;

  //
  // Wait for host not busy
  //
  startCount = GetCount ();
  do {
    smbStat.Data = mCpuCsrAccess->ReadCpuCsr (NvDimm->SocketId, 0, smbStatReg);
    if (!smbStat.Bits.smb_busy) break;
    // Wait for timeout
  } while (GetDelay (startCount) < SMB_TIMEOUT);

  //
  // Send command
  //
  mCpuCsrAccess->WriteCpuCsr(NvDimm->SocketId, 0, smbCmdReg, smbCmd.Data);

  //
  // Wait for host not busy
  //
  startCount = GetCount ();
  do {
    smbStat.Data = mCpuCsrAccess->ReadCpuCsr (NvDimm->SocketId, 0, smbStatReg);
    if (!smbStat.Bits.smb_busy) break;

    // Wait for timeout
  } while (GetDelay (startCount) < SMB_TIMEOUT);

  //
  // Wait for the data
  //
  startCount = GetCount ();
  do {
    smbStat.Data = mCpuCsrAccess->ReadCpuCsr (NvDimm->SocketId, 0, smbStatReg);
  } while ( (!smbStat.Bits.smb_rdo && !smbStat.Bits.smb_sbe) && (GetDelay (startCount) < SMB_TIMEOUT) );

  //
  // If read data is valid
  //
  if (smbStat.Bits.smb_rdo && !smbStat.Bits.smb_sbe) {
    smbData.Data = mCpuCsrAccess->ReadCpuCsr (NvDimm->SocketId, 0, smbDataReg);
    if (dev.compId == MTS) {
      *data = (UINT8) smbData.Bits.smb_rdata;  //lsb
      *(data + 1) = (UINT8) (smbData.Bits.smb_rdata >> 8); //msb
    } else {
      *data = (UINT8) smbData.Bits.smb_rdata;
    }
  } else {
    rval = RETRY;
  }

  // Restore TSOD polling setting
  RestoreTsod(dev, NvDimm->SocketId);

  return rval;
}

/**

  Read SMBUS device at a given device and offset

  @param NvDimm         - NvDimm structure
  @param byteOffset  - Offset in Device
  @param data        - Pointer to store value

  @retval EFI_STATUS

**/
EFI_STATUS
ReadSmb (
   NVDIMM                       *NvDimm,
   UINT8                        byteOffset,
   UINT8                        *data
   )
{
  UINT32                          RetVal;
  UINT32                          smbCmdReg;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT   smbCmd;
  struct smbDevice                smbusdev;
  UINT8                           NodeId = SKT_TO_NODE(NvDimm->SocketId, NvDimm->ImcId);
  UINT8                           SktCh = NODECH_TO_SKTCH(NodeId, NvDimm->Ch);

  smbusdev.compId = SPD;
  smbusdev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  smbusdev.address.busSegment = SktCh / MAX_MC_CH;
  smbusdev.address.strapAddress = NvDimm->Dimm + ((SktCh % MAX_MC_CH) * MAX_DIMM);
  smbusdev.mcId = NvDimm->ImcId;
  smbusdev.address.deviceType = 0x08; // 0x08 for JEDEC

  RetVal  = ReadSmbWorkerFunction (NvDimm, smbusdev, byteOffset, data);

  if (RetVal == RETRY) {
    //
    // if SMBus is busy force a soft reset and retry transaction
    //
    if (smbusdev.address.busSegment == 0) {
      smbCmdReg  = SMB_CMD_CFG_0_PCU_FUN5_REG;
    } else {
      smbCmdReg  = SMB_CMD_CFG_1_PCU_FUN5_REG;
    }

    smbCmd.Data = mCpuCsrAccess->ReadCpuCsr (NvDimm->SocketId, 0, smbCmdReg);
    //
    // If slave device is hung, software can write this bit to 1 and
    // the SMB_CKOVRD to 0 (for more than 35ms) to force hung the SMB
    // slaves to time-out and put it in idle state without using power
    // good reset or warm reset.
    //

    //
    // Override the clock
    //
    smbCmd.Bits.smb_ckovrd = 0;
    smbCmd.Bits.smb_soft_rst = 1;
    mCpuCsrAccess->WriteCpuCsr(NvDimm->SocketId, 0, smbCmdReg, smbCmd.Data);
    //
    // Wait 35ms
    //
    MicroSecondDelay (35000);
    smbCmd.Bits.smb_ckovrd   = 1;
    smbCmd.Bits.smb_soft_rst = 0;
    mCpuCsrAccess->WriteCpuCsr(NvDimm->SocketId, 0, smbCmdReg, smbCmd.Data);

    RetVal  = ReadSmbWorkerFunction (NvDimm, smbusdev, byteOffset, data);
  }

  if (RetVal != SUCCESS)
    return EFI_DEVICE_ERROR;

  return EFI_SUCCESS;
}

/**

  Write SMBUS dev/offset with specified data

  @param NvDimm         - NvDimm structure
  @param byteOffset  - byte offset within device
  @param data        - pointer to data to write

  @retval EFI_STATUS

**/
EFI_STATUS
WriteSmb (
   NVDIMM                       *NvDimm,
   UINT8                        byteOffset,
   UINT8                        *data
   )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT32                    startCount = 0;
  UINT32                    smbCmdReg;
  UINT32                    smbStatReg;
  UINT32                    smbDataReg;
  SMB_STATUS_CFG_0_PCU_FUN5_STRUCT    smbStat;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT       smbCmd;
  SMB_DATA_CFG_0_PCU_FUN5_STRUCT      smbData;
  struct smbDevice          smbusdev;
  UINT8                     NodeId = SKT_TO_NODE(NvDimm->SocketId, NvDimm->ImcId);
  UINT8                     SktCh = NODECH_TO_SKTCH(NodeId, NvDimm->Ch);

  smbusdev.compId = SPD;
  smbusdev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  smbusdev.address.busSegment = SktCh / MAX_MC_CH;
  smbusdev.address.strapAddress = NvDimm->Dimm + ((SktCh % MAX_MC_CH) * MAX_DIMM);
  smbusdev.mcId = NvDimm->ImcId;
  smbusdev.address.deviceType = 0x08; // 0x08 for JEDEC

  // Ensure TSOD polling is disabled before continuing
  DisableTsod(smbusdev, NvDimm->SocketId);

  if (smbusdev.address.busSegment == 0) {
    smbCmdReg   = SMB_CMD_CFG_0_PCU_FUN5_REG;
    smbStatReg  = SMB_STATUS_CFG_0_PCU_FUN5_REG;
    smbDataReg  = SMB_DATA_CFG_0_PCU_FUN5_REG;
  } else {
    smbCmdReg   = SMB_CMD_CFG_1_PCU_FUN5_REG;
    smbStatReg  = SMB_STATUS_CFG_1_PCU_FUN5_REG;
    smbDataReg  = SMB_DATA_CFG_1_PCU_FUN5_REG;
  }

  //
  // Form write command
  //
  smbCmd.Data = 0;
  smbCmd.Bits.smb_ckovrd = 1;
  smbCmd.Bits.smb_dis_wrt = 0;
  smbCmd.Bits.smb_dti = smbusdev.address.deviceType;
  smbCmd.Bits.smb_ba = (UINT32) byteOffset;
  smbCmd.Bits.smb_sa = (UINT32) smbusdev.address.strapAddress;
  smbCmd.Bits.smb_wrt = 1;
  if (smbusdev.compId == MTS) {
    smbCmd.Bits.smb_word_access = 1;
  } else {
    smbCmd.Bits.smb_word_access = 0;
  }
  smbCmd.Bits.smb_pntr_sel = 0;
  smbCmd.Bits.smb_cmd_trigger = 1;

  //
  // data to write
  //
  smbData.Data = 0;
  if (smbusdev.compId == MTS) {
    smbData.Bits.smb_wdata = (UINT32)*data + ((UINT32)*(data + 1) << 8);
  } else {
    smbData.Bits.smb_wdata = (UINT32) *data;
  }

  //
  // Wait for host not busy
  //
  startCount = GetCount ();
  do {
    smbStat.Data = mCpuCsrAccess->ReadCpuCsr (NvDimm->SocketId, 0, smbStatReg);
    if (!smbStat.Bits.smb_busy) break;
    // Wait for timeout
  } while (GetDelay (startCount) < SMB_TIMEOUT);

  //
  // Send command
  //
  mCpuCsrAccess->WriteCpuCsr(NvDimm->SocketId, 0, smbDataReg, smbData.Data);
  mCpuCsrAccess->WriteCpuCsr(NvDimm->SocketId, 0, smbCmdReg, smbCmd.Data);

  //
  // Wait for the write to complete
  //
  startCount = GetCount ();
  do {
    smbStat.Data = mCpuCsrAccess->ReadCpuCsr (NvDimm->SocketId, 0, smbStatReg);
  } while ( (!smbStat.Bits.smb_wod && !smbStat.Bits.smb_sbe) && (GetDelay (startCount) < SMB_TIMEOUT) );

  // Restore TSOD polling setting
  RestoreTsod(smbusdev, NvDimm->SocketId);

  return Status;
}

EFI_STATUS
JedecSwitchPage(
   NVDIMM                       *NvDimm,
   UINT8                        Page
   )
{
  EFI_STATUS    Status;
  UINT8         CurrentPage;
  UINTN         Retries = 0;

  Status = ReadSmb(NvDimm, OPEN_PAGE, &CurrentPage);
  ASSERT_EFI_ERROR (Status);
#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "Current Page = 0x%x\n", CurrentPage));
#endif

  // Avoid the Page Switch if we are already on the new Page
  if (CurrentPage == Page) return EFI_SUCCESS;

  WriteSmb(NvDimm, OPEN_PAGE, &Page);

  do {
    MicroSecondDelay(10000);
    Status = ReadSmb(NvDimm, OPEN_PAGE, &CurrentPage);
    ASSERT_EFI_ERROR (Status);
    Retries++;
  } while ((Retries < 5) || (CurrentPage != Page));

  if (CurrentPage == Page) {
#if VERBOSE_DEBUG
    DEBUG((EFI_D_ERROR, "New Page = 0x%x\n", CurrentPage));
#endif
    return EFI_SUCCESS;
  }
  else {
    return EFI_DEVICE_ERROR;
  }
}
