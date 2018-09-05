/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2009-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file FnvAccess.c

  FalconValley Access mechanisms and mailbox commands.

**/
//
// Include files
//
#include "DataTypes.h"
#include "FnvAccess.h"
#include "SysFunc.h"
#include "SysFuncChip.h"
#include "MemFuncChip.h"
#include "Cpgc.h"
#include "CpgcChip.h"
#include "MemApiSkx.h"

#include "CpuPciAccess.h"


static UINT32 FnvMailboxInterface (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 inputPayload[32], UINT32 outputPayload[32], UINT32 opcode, UINT8 *mbStatus);
static UINT32 FnvMailboxInterfaceFinish (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 outputPayload[NUM_PAYLOAD_REG], UINT32 opcode, UINT8 *mbStatus);
static UINT32 FnvMailboxInterfaceStart (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 inputPayload[NUM_PAYLOAD_REG], UINT32 opcode);

static struct dimmInformation (*GetFnvCisBufferDimmInfo (
                                                   FNVCISBUFFER fnvBuffer
                                                  ))[MAX_SOCKET][MAX_CH][MAX_DIMM]
{
  return(&fnvBuffer->dimmInfo);
}

static struct platformConfiguration (*GetFnvCisBufferPlatformConfig (
                                                              FNVCISBUFFER fnvBuffer
                                                              )) [MAX_SOCKET][MAX_CH][MAX_DIMM]
{
  return (&fnvBuffer->platformConfig);
}

static struct errorPageLog (*GetFnvCisBufferErrorPageLog(
                                                  FNVCISBUFFER fnvBuffer
                                                  )) [MAX_SOCKET][MAX_CH][MAX_DIMM]
{
  return (&fnvBuffer->errorLog);
}

struct dimmPartition  (*GetFnvCisBufferDimmPartition (
                                                      FNVCISBUFFER fnvBuffer
                                                      )) [MAX_SOCKET][MAX_CH][MAX_DIMM]
{
  return(&fnvBuffer->dimmPart);
}



//-----------------------------------------------------------------------------------------------
/*
NVMCTLR address mapping

Function              AHB Slave Name  Base Address  End Address
DPA Mapped Registers  AHB_Slave_1     0xC0000000    0xC3FFFFFF
DA Unit CSRs          AHB_Slave_2     0xC4000000    0xC7FFFFFF
Dunit CSRs            AHB_Slave_3     0xC8000000    0xCBFFFFFF
Aunit CSRs            AHB_Slave_4     0xCC000000    0xCFFFFFFF
Punit CSR             AHB_Slave_5     0xD0000000    0xD3FFFFFF
Security CSRs         AHB_Slave_6     0xD4000000    0xD7FFFFFF
DFx/Misc CSRs         AHB_Slave_7     0xD8000000    0xDBFFFFFF
Reserved              AHB_Slave_8     0xDC000000    0xDFFFFFFF


AHB Addr Bit        31:29   28:26         25:22                     21:13           12:0
NVMCTLR CSR AHB Address 110b    Slave Number  Requestor ID              000000000b      12:0

AHB Addr Bit        31:29   28:26         25:22                     21:13           12    11:0
SMBus/JTAG (JCONFL) 110b    Dev Fun[2:1]  Req ID = SMBus or JTAG    000000000b      F[0]  11:0

AHB Addr Bit        31:29   28:26         25:22                     21:13           12:0
Extended MRS        110b    15:13         Req ID = NVMDIMM             000000000b      12:0
NOTE: Extended MRS Access is disabled by FW after NVMDIMM Initialization

AHB Addr Bit        31:29   28:26         25:22                     21:13           12:3    2:0
NVMDIMM DPA            110b    18:16         Req ID = NVMDIMM             000000000b      15:6    000b
NOTES: NVMDIMM DPA Starting Address A[42:0] = 000_0000_0000
Access to complete NVMCTLR Registers space is only enabled for debug
NVMCTLR Registers Occupy 512KByte NVMDIMM DPA Space + reserved (total 1MB)
NVMCTLR Reads/Writes Eight Bytes of NVMCTLR CSR AHB space per 64B NVMDIMM Access
(Data [63:0])

AHB Addr Bit        31:26                 25:22                     21:11           10:3    2:0
NVMDIMM Mailbox        110000b               Req ID = NVMDIMM             0100000000000b  13:6    000b
NOTES: NVMDIMM DPA Starting Address A[42:0] = 000_0010_0000
Mailbox Registers Occupy 12KByte NVMDIMM DPA Space + reserved (total 1MB)
NVMCTLR Reads/Writes Eight Bytes of NVMCTLR CSR AHB space per 64B NVMDIMM Access
(Data [63:0])
*/

#define FNV_SMB_REG_ADDR(Device,Function,Offset) \
  (((Offset) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1) << 15))

// Note: Mask bits[5:3] to ensure Quad word access (the first UI of the NVMDIMM data burst)
// CSR write size less than Quad word will need to use CSR read-modify-write unless caller handles caching
#define FNV_DPA_REG_ADDR(Device,Function,Offset) \
  ((((Offset) & 0xff8) << 3) | (((Function) & 0x07) << 15) | (((Device) & 0x1) << 18))

#define  FNV_SMB   0
#define  FNV_EMRS  1
#define  FNV_DPA   2

//-----------------------------------------------------------------------------------------------
/**

  Computes address of NVMCTLR configuration space

  @param host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param Offset    - Register offset; values come from the auto generated header file
  @param Size      - Register size in bytes (may be updated if pseudo-offset)

  @retval Address

**/
static UINT32
GetFnvCfgAddress (
                 PSYSHOST host,
                 UINT8    fnvInterface,
                 UINT32   Offset
                 )
{
  UINT32 Dev, Fun;
  UINT32 RegAddr = 0;
  UINT8 BoxType;
  UINT8 FuncBlk;
  CSR_OFFSET RegOffset;

  RegOffset.Data = Offset;

  BoxType = (UINT8)RegOffset.Bits.boxtype;
  FuncBlk = (UINT8)RegOffset.Bits.funcblk;

  // Assert NVMCTLR BoxType == 5
  RC_ASSERT(BoxType == 5, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_31);

  //
  // Identify the NVMCTLR Device/Function number for the access
  //
  Dev = FuncBlk >> 3;
  Fun = FuncBlk & 7;
  //
  // Form address
  //
  switch (fnvInterface) {
  case FNV_SMB:
  case FNV_EMRS:
    RegAddr = FNV_SMB_REG_ADDR(Dev, Fun, RegOffset.Bits.offset);
    break;
  case FNV_DPA:
    RegAddr = FNV_DPA_REG_ADDR(Dev, Fun, RegOffset.Bits.offset);
    break;
  default:
    //Assert on error
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_32);
    break;
  }
  return RegAddr;

} // GetFnvCfgAddress

UINT32
ReadFnvCfgSmbWorkerFunction(
                            PSYSHOST         host,
                            UINT8            socket,
                            UINT32           smbReg,
                            struct smbDevice dev,
                            UINT32           *localData,
                            UINT8            regSize
)
{
  UINT8  *pByte = (UINT8 *)localData;
  UINT8  devFunc = 0;
  UINT8  rsvd = 0;
  UINT8  data8 = 0;
  UINT32 status;



  //
  // Write command code 0x90 with Rsvd as data
  // ([7]Begin=1, [6]End=0, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  if ((status = WriteSmb(host, socket, dev, 0x80, &rsvd)) != SUCCESS)
    return status;

  //
  // Write command code 0x10 with DevFunc as data
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  devFunc = (UINT8)((smbReg >> 12) & 0xF);
  if ((status = WriteSmb(host, socket, dev, 0x00, &devFunc)) != SUCCESS)
    return status;

  //
  // Write command code 0x10 with Rsvd as data for Reg[15:8]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  data8 = (UINT8)((smbReg >> 8) & 0xF);
  if ((status = WriteSmb(host, socket, dev, 0x00, &data8)) != SUCCESS)
    return status;

  //
  // Write command code 0x50 with Reg as data for Reg[7:0]
  // ([7]Begin=0, [6]End=1, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  data8 = (UINT8)(smbReg & 0xFF);
  if ((status = WriteSmb(host, socket, dev, 0x40, &data8)) != SUCCESS)
    return status;

  //
  // Read command code 0x90 with Rsvd holding status data
  // ([7]Begin=1, [6]End=0, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  if ((status = ReadSmb(host, socket, dev, 0x80, &rsvd)) != SUCCESS)
    return status;

  //
  // Read command code 0x10 with Data[31:24]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  if ((status = ReadSmb(host, socket, dev, 0x00, pByte + 3)) != SUCCESS)
    return status;

  //
  // Read command code 0x10 with Data[23:16]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  if ((status = ReadSmb(host, socket, dev, 0x00, pByte + 2)) != SUCCESS)
    return status;


  //
  // Read command code 0x10 with Data[15:8]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  if ((status = ReadSmb(host, socket, dev, 0x00, pByte + 1)) != SUCCESS)
    return status;


  //
  // Read command code 0x50 with Data[7:0]
  // ([7]Begin=0, [6]End=1, [4]PEC_en=1, [3:2]=00-DWord RD, [1:0]=Byte)
  //
  if ((status = ReadSmb(host, socket, dev, 0x40, pByte)) != SUCCESS)
    return status;


  return SUCCESS;
  } //ReadFnvCfgSmbWorkerFunction
//-----------------------------------------------------------------------------------------------
/**

  Read NVMCTLR SMBUS dev/offset

  @param host        - Pointer to the system host (root) structure
  @param socket        - Processor socket ID
  @param ch          - DDR CHannel ID
  @param dev         - SMBUS device address
  @param reg         - byte offset for NVMCTLR
  @param data        - pointer to data to be read

  @retval 0 - success
  @retval 1 - failure

**/
UINT32
ReadFnvCfgSmb(
             PSYSHOST host,
             UINT8  socket,
             UINT8  ch,
             UINT8  dimm,
             UINT32 reg,
             UINT32 *data
             )

{
  UINT8 index = 0;
  UINT8 regSize = 0;
  UINT32 smbReg;
  UINT32 status = SUCCESS;
  UINT32 localData;
  struct smbDevice dev;

  CSR_OFFSET RegOffset;

#ifdef PRINT_FUNC
MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "ReadFnvCfgSmb Starts\n"));
#endif

#ifdef COSIM_EN
 if (C_IsFnvFeedbackPresent()) {
    C_delay_d_clk(50);
  }
#endif
  RegOffset.Data = reg;

  regSize = (UINT8)RegOffset.Bits.size;
  index = (UINT8)(RegOffset.Bits.offset & 3) << 3;
  smbReg = GetFnvCfgAddress (host, FNV_SMB, reg);
  smbReg &= 0xFFFFFFFC;             // Always Do Dword access
  *data = 0xFFFFFFFF;

  //
  // Initialize smbDevice structure
  //
  dev.compId = SPD;
  dev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  dev.address.deviceType = DTI_FNV;
  GetSmbAddress(host, socket, ch, dimm, &dev);

  do {
    localData = 0;
    status = ReadFnvCfgSmbWorkerFunction (host, socket, smbReg, dev, &localData, regSize);
  } while (status == FAILURE);



  localData >>= index;
  if (regSize == 4)
    *data = localData;
  else if (regSize == 2)
    *data = localData & 0x0FFFF;
  else
    *data = localData & 0x0FF;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "ReadFnvCfgSmb Ends\n"));
#endif

  return status;

}

UINT32
WriteFnvCfgSmbWorkerFunction (
                              PSYSHOST         host,
                              UINT8            socket,
                              UINT32           smbReg,
                              struct smbDevice dev,
                              UINT32           *localData,
                              UINT8            regSize,
                              UINT8            cmdCode
)
{
  UINT32 status;
  UINT8  *pByte = (UINT8 *)localData;
  UINT8 devFunc = 0;
  UINT8 rsvd = 0;
  UINT8 data8 = 0;
  //
  // Write command code 0x9C with Rsvd as data
  // ([7]Begin=1, [6]End=0, [4]PEC_en=1, [3:2]=11-DWord WR, [1:0]=Byte)
  //
  if ((status = WriteSmb(host, socket, dev, 0x80 | cmdCode, &rsvd)) != SUCCESS)
    return status;

  //
  // Write command code 0x1C with DevFunc as data
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=11-DWord WR, [1:0]=Byte)
  //
  devFunc = (UINT8)((smbReg >> 12) & 0xF);
  if ((status = WriteSmb(host, socket, dev, 0x00 | cmdCode, &devFunc)) != SUCCESS)
    return status;

  //
  // Write command code 0x1C with Rsvd as data for Reg[15:8]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=11-DWord WR, [1:0]=Byte)
  //
  data8 = (UINT8)((smbReg >> 8) & 0xF);
  if ((status = WriteSmb(host, socket, dev, 0x00 | cmdCode, &data8)) != SUCCESS)
    return status;

  //
  // Write command code 0x1C with Reg as data for Reg[7:0]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=11-DWord WR, [1:0]=Byte)
  //
  data8 = (UINT8)(smbReg & 0xFF);
  if ((status = WriteSmb(host, socket, dev, 0x00 | cmdCode, &data8)) != SUCCESS)
    return status;

  //
  // Read command code 0x1C with Data[31:24]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=11-DWord WR, [1:0]=Byte)
  //
  if (regSize == 4) {
    if ((status = WriteSmb(host, socket, dev, 0x00  | cmdCode, pByte + 3)) != SUCCESS)
      return status;
  }

  //
  // Read command code 0x1C with Data[23:16]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=11-DWord WR, [1:0]=Byte)
  //
  if (regSize == 4) {
    if ((status = WriteSmb(host, socket, dev, 0x00 | cmdCode, pByte + 2)) != SUCCESS)
      return status;
  }

  //
  // Read command code 0x1C with Data[15:8]
  // ([7]Begin=0, [6]End=0, [4]PEC_en=1, [3:2]=11-DWord WR, [1:0]=Byte)
  //
  if ((regSize == 4) || (regSize == 2)) {
    if ((status = WriteSmb(host, socket, dev, 0x00 | cmdCode, pByte + 1)) != SUCCESS)
      return status;
  }

  //
  // Read command code 0x5C with Data[7:0]
  // ([7]Begin=0, [6]End=1, [4]PEC_en=1, [3:2]=11-DWord WR, [1:0]=Byte)
  //
  if ((status = WriteSmb(host, socket, dev, 0x40 | cmdCode, pByte)) != SUCCESS)
    return status;

  return SUCCESS;
} // WriteFnvCfgSmbWorkerFunction
//-----------------------------------------------------------------------------------------------
/**

  Write NVMCTLR SMBUS dev/offset with specified data

  @param host        - Pointer to the system host (root) structure
  @param socket        - Processor socket ID
  @param ch          - DDR Channel ID
  @param reg         - byte offset for NVMCTLR
  @param data        - pointer to data to be read

  @retval 0 - success
  @retval 1 - failure

**/
UINT32
WriteFnvCfgSmb (
               PSYSHOST host,
               UINT8  socket,
               UINT8  ch,
               UINT8  dimm,
               UINT32 reg,
               UINT32 data
               )

{
  UINT8 regSize = 0;
  UINT32 smbReg;
  UINT32 status = SUCCESS;
  UINT32 localData = data;
  struct smbDevice dev;
  UINT8 cmdCode;
  CSR_OFFSET RegOffset;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteFnvCfgSmb Starts\n"));
#endif

#ifdef COSIM_EN
 if (C_IsFnvFeedbackPresent()) {
    C_delay_d_clk(50);
  }
#endif

  RegOffset.Data = reg;
  regSize = (UINT8)RegOffset.Bits.size;
  smbReg = GetFnvCfgAddress (host, FNV_SMB, reg);

  if (regSize == 4)
    cmdCode = 0x0C; // ([7]Begin=0, [6]End=0, [4]PEC_en=0, [3:2]=11-DWord WR, [1:0]=Byte)
  else if (regSize == 2)
    cmdCode = 0x08; // ([7]Begin=0, [6]End=0, [4]PEC_en=0, [3:2]=10-Word WR, [1:0]=Byte)
  else
    cmdCode = 0x04; // ([7]Begin=0, [6]End=0, [4]PEC_en=0, [3:2]=01-BYTE WR, [1:0]=Byte)
  //
  // Initialize smbDevice structure
  //
  dev.compId = SPD;
  dev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  dev.address.deviceType = DTI_FNV;
  GetSmbAddress(host, socket, ch, dimm, &dev);

  do {
    status = WriteFnvCfgSmbWorkerFunction (host, socket, smbReg, dev, &localData, regSize, cmdCode);
  } while (status == FAILURE);



#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteFnvCfgSmb Ends\n"));
#endif
  return status;
}

//-----------------------------------------------------------------------------------------------
/**

Arguments:

  host        - Pointer to the system host (root) structure
  ch          - DDR Channel ID

  @retval 0 - success
  @retval 1 - failure

**/
UINT32
WriteFnvCfgEmrs (
                PSYSHOST host,
                UINT8  socket,
                UINT8  ch,
                UINT8  dimm,
                UINT32 reg,
                UINT32 data
                )

{
  UINT8 regSize = 0;
  UINT32 smbReg;
  UINT32 status = SUCCESS;
  UINT32 localData = data;
  UINT8 *dataPtr = (UINT8 *)&localData;
  CSR_OFFSET RegOffset;
  UINT8 i;
  UINT32                      tCrapOrg;
  TCRAP_MCDDC_CTL_STRUCT      tCrap;
  struct channelNvram         (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteFnvCfgEmrs Starts\n"));
#endif

  //
  // Set 3N timing and save original setting
  //
  tCrap.Data = tCrapOrg = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);
  tCrap.Bits.cmd_stretch  = (*channelNvList)[ch].trainTimingMode;
  MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tCrap.Data);

  RegOffset.Data = reg;
  regSize = (UINT8)RegOffset.Bits.size;
  smbReg = GetFnvCfgAddress (host, FNV_SMB, reg);

  // Use little-endian ordering
  // Assume natural size alignment
  for (i = 0; i < regSize; i++) {

    // MRS command to bank 8 with address[11:0] = NVMCTLR CSR address [15:4]
    if (i == 0) {
      WriteMrsDdr4(host, socket, ch, dimm, 0, ((smbReg + i) >> 4) & 0xFFF, 8, A_SIDE);
    }

    // MRS command to bank 9 with address[11:8] = NVMCTLR CSR address [3:0], NVMCTLR CSR data [7:0]
    WriteMrsDdr4(host, socket, ch, dimm, 0, (((smbReg + i) & 0xF) << 8) | (*(dataPtr + i) & 0xFF), 9, A_SIDE);
  }
  //
  // Restore original CMD timing
  //
  MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tCrapOrg);


#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteFnvCfgEmrs Ends\n"));
#endif
  return status;
}

//-----------------------------------------------------------------------------------------------
/**

Arguments:

  host        - Pointer to the system host (root) structure
  ch          - DDR Channel ID

  @retval 0 - success
  @retval 1 - failure

**/
static UINT32
DpaWrite64 (
            PSYSHOST host,
            UINT8  socket,
            UINT8  ch,
            UINT8  dimm,
            UINT32 dpa,
            UINT64_STRUCT *data
            )

{
  UINT32 status = SUCCESS;


    // Disable assembly for NVMDIMM x'tor

  // Setup CPGC sequence
  CpgcSetup64(host, socket, ch, dimm, dpa, WRITE_ACCESS, 1, data);

  // IO reset
  IO_Reset(host, socket);

  // Start test
  CpgcGlobalTestStart (host, socket, (1 << ch));

  // Poll test completion
  CpgcPollGlobalTestDone (host, socket, (1 << ch));


    // Re-enable assembly for NVMDIMM x'tor

  return status;
}

//-----------------------------------------------------------------------------------------------
/**

Arguments:

  host        - Pointer to the system host (root) structure
  ch          - DDR Channel ID

  @retval 0 - success
  @retval 1 - failure

**/
static UINT32
DpaRead64 (
           PSYSHOST host,
           UINT8  socket,
           UINT8  ch,
           UINT8  dimm,
           UINT32 dpa,
           UINT64_STRUCT *data
           )

{
  UINT32 status = SUCCESS;
  UINT64_STRUCT localData;

  // Disable the assembly for NVMDIMM x'tor

  localData.lo = 0;
  localData.hi = 0;

  // Setup CPGC sequence
  CpgcSetup64(host, socket, ch, dimm, dpa, READ_ACCESS, 1, &localData);

  // IO reset
  IO_Reset(host, socket);

  // Start test
  CpgcGlobalTestStart (host, socket, (1 << ch));

  // Poll test completion
  CpgcPollGlobalTestDone (host, socket, (1 << ch));

  // Collect test results
  data->lo = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA0_S_MCDDC_CTL_REG);
  data->hi = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA1_S_MCDDC_CTL_REG);

    // Re-enable assembly for NVMDIMM x'tor

  return status;
}

//-----------------------------------------------------------------------------------------------
/**

Arguments:

  host        - Pointer to the system host (root) structure
  ch          - DDR Channel ID

**/
VOID
FnvBcwRead (
           PSYSHOST host,
           UINT8  socket,
           UINT8  ch,
           UINT8  dimm,
           UINT32 dpa,
           UINT8 data[MAX_STROBE/2]
           )
{
  UINT8 ui, byte;
  UINT8 uiBitMask;
  UINT8 temp = 0;
  UINT64_STRUCT localData;
  UINT32   cpgcErrDat0S;
  UINT32   cpgcErrDat1S;
  UINT32   cpgcErrDat2S;
  UINT32   cpgcErrDat3S;
  UINT32   cpgcErrEccS;
  UINT8     i;
  UINT8     wdbIndex;
  TWdbLine  WdbLines[32];

  for (wdbIndex = 0; wdbIndex < 8; wdbIndex++) {
    for (i = 0; i < MRC_WDB_LINE_SIZE; i++) {
      WdbLines[wdbIndex].WdbLine[i] = 0;
    } // i loop
  } // wdbIndex loop

  WDBFill (host, socket, ch, WdbLines, 1, 0);

  // Initialize results to 0
  for (byte = 0; byte < 9; byte++) {
    data[byte] = 0;
  }

  // For each UI of the burst
  for (ui = 0; ui < 8; ui++) {

    localData.lo = 0;
    localData.hi = 0;

    // UI0 = uiBitMask 0x01 and early
    // UI1 = uiBitMask 0x01 and late
    // UI2 = uiBitMask 0x02 and early
    // UI3 = uiBitMask 0x02 and late
    // UI4 = uiBitMask 0x04 and early
    // UI5 = uiBitMask 0x04 and late
    // UI6 = uiBitMask 0x08 and early
    // UI7 = uiBitMask 0x08 and late
    uiBitMask = (BIT0 << (ui >> 1));

    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA0_S_MCDDC_CTL_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA1_S_MCDDC_CTL_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA2_S_MCDDC_CTL_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA3_S_MCDDC_CTL_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_ECC_S_MCDDC_CTL_REG, 0);

    // Setup CPGC sequence
    CpgcSetup64(host, socket, ch, dimm, dpa, READ_ACCESS, uiBitMask, &localData);

    // IO reset
    IO_Reset(host, socket);

    // Start test
    CpgcGlobalTestStart (host, socket, (1 << ch));

    // Poll test completion
    CpgcPollGlobalTestDone (host, socket, (1 << ch));

    // Collect test results
    cpgcErrDat0S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA0_S_MCDDC_CTL_REG);
    cpgcErrDat1S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA1_S_MCDDC_CTL_REG);
    cpgcErrDat2S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA2_S_MCDDC_CTL_REG);
    cpgcErrDat3S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA3_S_MCDDC_CTL_REG);
    cpgcErrEccS  = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_ECC_S_MCDDC_CTL_REG);

    // Update the results
    for (byte = 0; byte < 9; byte++) {

      switch(byte >> 2) {
      case 0:
        if (ui % 2 == 0) {
          temp = (UINT8)(cpgcErrDat0S >> ((byte % 4) * 8));
        }
        else {
          temp = (UINT8)(cpgcErrDat2S >> ((byte % 4) * 8));
        }
        break;
      case 1:
        if (ui % 2 == 0) {
          temp = (UINT8)(cpgcErrDat1S >> ((byte % 4) * 8));
        }
        else {
          temp = (UINT8)(cpgcErrDat3S >> ((byte % 4) * 8));
        }
        break;
      case 2:
        if (ui % 2 == 0) {
          temp = (UINT8)(cpgcErrEccS);
        }
        else {
          temp = (UINT8)(cpgcErrEccS >> 8);
        }
        break;
      }
      data[byte] |= ((temp & BIT0) << (7 - ui));
    }
  }
} // FnvBcwRead

//-----------------------------------------------------------------------------------------------
/**

Arguments:

  host        - Pointer to the system host (root) structure
  ch          - DDR Channel ID

  @retval 0 - success
  @retval 1 - failure

**/
UINT32
ReadFnvCfgDpa (
              PSYSHOST host,
              UINT8  socket,
              UINT8  ch,
              UINT8  dimm,
              UINT32 reg,
              UINT64_STRUCT *data
              )

{
  UINT32 regSize = 0;
  CSR_OFFSET RegOffset;
  UINT64_STRUCT dataMask;
  UINT64_STRUCT localData;
  UINT32 dpa;
  UINT8 regIndex;
  UINT64_STRUCT temp;
  UINT32 status = SUCCESS;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "ReadFnvCfgDpa Starts\n"));
#endif

  RegOffset.Data = reg;
  regSize = RegOffset.Bits.size;
  regIndex = (RegOffset.Bits.offset & 0x7) * 8;
  dpa = GetFnvCfgAddress (host, FNV_DPA, reg);

  // If size < 8, Read64, mask size, or data, Write64
  if (regSize) {

    //dataMask = ((1ULL << (8*regSize)) - 1) << regIndex;
    temp.lo = 1;
    temp.hi = 0;
    dataMask = LShiftUINT64(SubUINT64(LShiftUINT64(temp, 8 * regSize), temp), regIndex);

    DpaRead64(host, socket, ch, dimm, dpa, &localData);

    //localData = (localData & dataMask) >> regIndex;
    temp.lo = localData.lo & dataMask.lo;
    temp.hi = localData.hi & dataMask.hi;
    localData = RShiftUINT64(temp, regIndex);

    if (regSize == 4) {
      *(UINT32 *)data = localData.lo;
    } else if (regSize == 2) {
      *(UINT16 *)data = (UINT16)localData.lo;
    } else {
      *(UINT8 *)data = (UINT8)localData.lo;
    }
  } else {
    DpaRead64(host, socket, ch, dimm, dpa, data);
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "ReadFnvCfgDpa Ends\n"));
#endif

  return status;
}

//-----------------------------------------------------------------------------------------------
UINT32 WriteFnvCfgDpa (
                      PSYSHOST host,
                      UINT8  socket,
                      UINT8  ch,
                      UINT8  dimm,
                      UINT32 reg,
                      UINT64_STRUCT data
                      )
/**

Arguments:

  host        - Pointer to the system host (root) structure
  ch          - DDR Channel ID

  @retval 0 - success
  @retval 1 - failure

**/

{
  UINT32 regSize = 0;
  UINT32 regIndex = 0;
  CSR_OFFSET RegOffset;
  UINT64_STRUCT dataMask;
  UINT64_STRUCT localData;
  UINT32 dpa;
  UINT32 status = SUCCESS;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteFnvCfgDpa Starts\n"));
#endif

  RegOffset.Data = reg;
  regSize = RegOffset.Bits.size;
  regIndex = RegOffset.Bits.offset & 0x7;
  dpa = GetFnvCfgAddress (host, FNV_DPA, reg);

  // If size < 8, Read64, mask size, or data, Write64
  if (regSize) {
    dataMask.lo = 0xffffffff;
    dataMask.hi = 0xffffffff;
    // getting mask for the value being written
    dataMask = LShiftUINT64(dataMask, (8 * regSize));
    dataMask.lo = ~dataMask.lo;
    dataMask.hi = ~dataMask.hi;

    data.lo &= dataMask.lo; // masking LO of the value
    data.hi &= dataMask.hi; // masking HI of the value
    data = LShiftUINT64(data, (8 * regIndex)); // shifting the value to the offset of the CSR

    // getting mask for the value being read
    dataMask = LShiftUINT64(dataMask, (8 * regIndex));
    dataMask.lo = ~dataMask.lo;
    dataMask.hi = ~dataMask.hi;

    DpaRead64(host, socket, ch, dimm, dpa, &localData);
    localData.lo = (localData.lo & dataMask.lo) | data.lo;
    localData.hi = (localData.hi & dataMask.hi) | data.hi;

    DpaWrite64(host, socket, ch, dimm, dpa, &localData);
  } else {
    DpaWrite64(host, socket, ch, dimm, dpa, &data);
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteFnvCfgDpa Ends\n"));
#endif
  return status;
}

//#define RC_SIM_TEST  0
//-----------------------------------------------------------------------------------------------
/**

    NVMCTLR hook for NVMDIMM
    Enable NVMDIMM training mode, Test SMBus / DPA / CPGC interfaceS

    @param host   - Pointer to the system host (root) structure
    @param socket - Processor socket ID

    @retval None

**/
void
EarlyFnvConfigAccess (
                     PSYSHOST                  host,
                     UINT8                     socket
                     )
{
  UINT8 ch;
  UINT8 dimm;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "EarlyFnvConfigAccess Starts\n"));
#endif

  //
  // Assume that enough time has elapsed for NVMCTLR power-good
  // so that we can safely use SMBus interface
  //
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Update NVMCTLR access mode
    //
    (*channelNvList)[ch].fnvAccessMode = SMBUS;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

    } //dimm loop
  } //ch loop

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "EarlyFnvConfigAccess Ends\n"));
#endif
  return;
}

/**

    Falcon Valley hook after DDR training
    This routine disables NVMDIMM trainingmode, setup scramble seed

    @param host   - Pointer to the system host (root) structure
    @param socket - Processor socket ID

    @retval None

**/
void
LateFnvConfigAccess (
                     PSYSHOST                  host,
                     UINT8                     socket
                     )
{
  UINT8 ch;
  UINT8 dimm;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];
  DA_WR_SCRAMBLE_SEED_SEL_FNV_DA_UNIT_0_STRUCT  txSeedSel;
  DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_STRUCT         txSeedLo;
  DA_RD_SCR_LFSR_FNV_DA_UNIT_0_STRUCT           rxSeed;
  MCSCRAMBLECONFIG_MCDDC_DP_STRUCT              scrambleConfig;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "LateFnvConfigAccess Starts\n"));
#endif

  //
  // Disable NVMDIMM training mode
  //
  SetAepTrainingMode (host,socket, DISABLE_TRAINING_MODE);

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

      if (host->setup.mem.options & SCRAMBLE_EN_DDRT) {
        if (host->var.common.bootMode == NormalBoot && (host->var.mem.subBootMode == ColdBoot || host->var.mem.subBootMode == ColdBootFast)) {
          // Program Scrambling

          txSeedSel.Data = 0;
          txSeedLo.Data = 0;
          rxSeed.Data = 0;

          //Initialize seeds
          txSeedSel.Bits.ordersel = host->nvram.mem.scrambleSeed;
          rxSeed.Bits.seed = 0xFFFF;

          WriteFnvCfgEmrs(host, socket, ch, dimm, DA_WR_SCRAMBLE_SEED_SEL_FNV_DA_UNIT_0_REG, txSeedSel.Data);
          WriteFnvCfgEmrs(host, socket, ch, dimm, DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_REG, txSeedLo.Data);
          WriteFnvCfgEmrs(host, socket, ch, dimm, DA_RD_SCR_LFSR_FNV_DA_UNIT_0_REG, rxSeed.Data);

          // Ensure 0->1 transition to load seeds
          txSeedLo.Bits.lfsr_load = 1;
          rxSeed.Bits.lfsr_load = 1;

          WriteFnvCfgEmrs(host, socket, ch, dimm, DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_REG, txSeedLo.Data);
          WriteFnvCfgEmrs(host, socket, ch, dimm, DA_RD_SCR_LFSR_FNV_DA_UNIT_0_REG, rxSeed.Data);

          txSeedLo.Bits.en = 1;
          rxSeed.Bits.en = 1;

          WriteFnvCfgEmrs(host, socket, ch, dimm, DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_REG, txSeedLo.Data);
          WriteFnvCfgEmrs(host, socket, ch, dimm, DA_RD_SCR_LFSR_FNV_DA_UNIT_0_REG, rxSeed.Data);
        }

        scrambleConfig.Data = MemReadPciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG);
        scrambleConfig.Bits.rx_enable_ddrt = 1;
        // 4928558  Cloned From SKX Si Bug Eco: NVMDIMM Write Scrambling in 2N mode causes NVMCTLR ERROR
        if ((*channelNvList)[ch].timingMode != TIMING_2N)
          scrambleConfig.Bits.tx_enable_ddrt = 1;
        else
          scrambleConfig.Bits.tx_enable_ddrt = 0;
        scrambleConfig.Bits.ch_enable_ddrt = 1;
        MemWritePciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG, scrambleConfig.Data);
      }

    } //dimm loop




  } //ch loop

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "LateFnvConfigAccess Ends\n"));
#endif

  FnvDdrtIoInit(host, socket, DDRT_TRAINING_COMPLETE);

  return;
} // LateFnvConfigAccess

/**
*Updated 10/15/2014*
*NVMCTLR CIS 0.81*

Routine Description:
  @param host         - Pointer to the system host (root) structure
  @param socket       - Socket Number
  @param ch           - DDR Channel ID
  @param dimm         - DIMM Number
  @param mbStatus     - Pointer to 8 bit status code from status register
  @retval 0 SUCCESS
  @retval 1 FAILURE
**/
UINT32
SetFnvSystemTime ( PSYSHOST     host,
                   UINT8        socket,
                   UINT8        ch,
                   UINT8        dimm,
                   UINT8        *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG] = {0};
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Set System Time Starts\n"));
#endif

  inputPayload [0] = GetTimeInSecondsFrom1970();
  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, SET_SYSTEM_TIME, mbStatus);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Set System Time End\n"));
#endif

  return status;
}// Set System Time

/**
*Updated 8/19/2014*
*NVMCTLR CIS 0.70*

Routine Description: SetHostAlert allows the user to setup host alerts
  @param host         - Pointer to the system host (root) structure
  @param socket       - Socket Number
  @param ch           - DDR Channel ID
  @param dimm         - DIMM Number
  @param fnvBuffer    - structure containing everything NVMCTLR
  @param mbStatus     - Pointer to 8 bit status code from status register
  @retval 0 SUCCESS
  @retval 1 FAILURE
**/
static UINT32
GetErrorLogPage (PSYSHOST             host,
                 UINT8                socket,
                 UINT8                ch,
                 UINT8                dimm,
                 UINT8                accessType,
                 UINT8                logLevel,
                 UINT8                logType,
                 UINT8                offset,
                 UINT8                requestCount,
                 ERROR_PAGE_LOG       logPage,
                 UINT8                *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG];
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  ERROR_LOG_ENTRY errorLogEntry[MAX_LOG_ENTRY];
  UINT32 status;
  UINT8 i;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetErrorLogPage Starts\n"));
#endif

  (*errorLogEntry) = logPage->errorEntry;
  inputPayload[0]  = logLevel << 1 | logType << 2 |
                    (UINT32) (offset << 8) | (UINT32) (requestCount << 16);

  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, GET_ERROR_LOG, mbStatus);

  if (status == SUCCESS) {
    logPage->totalEntries    = (UINT8) outputPayload[0];
    logPage->newEntries      = (UINT8) (outputPayload[0] >> 8);
    logPage->overrunFlag     = (UINT8) (outputPayload[0] >> 16);
    logPage->returnCount     = (UINT8) (outputPayload[0] >> 24);
    for (i = 0; i < logPage->returnCount; i++) {
      (*errorLogEntry)[i].timeStamp.lo    = outputPayload[1 + (i * 6)];
      (*errorLogEntry)[i].timeStamp.hi    = outputPayload[2 + (i * 6)] & 0xFFFFFF;
      (*errorLogEntry)[i].timeOffset      = (outputPayload[2 + (i * 6)] >> 24) | ((outputPayload[3 + (i * 6)] & 0xFFFFFF) << 8);
      (*errorLogEntry)[i].dpaError.lo     = (outputPayload[3 + (i * 6)] >> 24) | ((outputPayload[4 + (i * 6)] & 0xFFFFFF) << 8);
      (*errorLogEntry)[i].dpaError.hi     = outputPayload[4 + (i * 6)] >> 24;
      (*errorLogEntry)[i].pdaError.lo     = outputPayload[5 + (i * 6)];
      (*errorLogEntry)[i].pdaError.hi     = outputPayload[6 + (i * 6)] & 0xFF;
      (*errorLogEntry)[i].range           = (UINT8) (outputPayload[6 + (i * 6)] >> 8);
      (*errorLogEntry)[i].errorID         = (UINT8) (outputPayload[6 + (i * 6)] >> 16);
      (*errorLogEntry)[i].transactionType = (UINT8) (outputPayload[6 + (i * 6)] >> 24);
    }
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetErrorLogPage Ends\n"));
#endif

  return status;
}

/**
  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param powerManagement   - structure containing DIMM power information
  @param mbStatus    - Pointer to 8 bit status code from status register

@retval 0 SUCCESS
@retval 1 FAILURE
**/
UINT32
GetPowerManagementPolicy (
                          PSYSHOST     host,
                          UINT8        socket,
                          UINT8        ch,
                          UINT8        dimm,
                          POWERMANAGEMENT  powerManagement,
                          UINT8        *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG];
  UINT32 outputPayload[NUM_PAYLOAD_REG];

  UINT32 status;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Get Power Management Policy Starts\n"));
#endif
  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, GET_POWER_MANAGEMENT, mbStatus);
  if (status == SUCCESS) {
    powerManagement->enable = (UINT8) outputPayload[0];
    powerManagement->TDP = (UINT8) (outputPayload[0] >> 8);
    powerManagement->peakPowerBudget = (UINT16) (outputPayload[0] >> 16);
    powerManagement->averagePowerBudget = (UINT16) (outputPayload[1]);
  }
#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Get Power Management Policy Ends\n"));
#endif

  return status;
}


/**
  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param powerManagement   - structure containing DIMM power information
  @param mbStatus    - Pointer to 8 bit status code from status register
@retval 0 SUCCESS
@retval 1 FAILURE
**/

UINT32
SetPowerManagementPolicy (
                          PSYSHOST     host,
                          UINT8        socket,
                          UINT8        ch,
                          UINT8        dimm,
                          POWERMANAGEMENT  powerManagement,
                          UINT8        *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG];
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  UINT32 status;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Set Power Management Policy Starts\n"));
#endif
  inputPayload[0] = powerManagement->enable | (powerManagement->TDP << 8) | (powerManagement->peakPowerBudget << 16);
  inputPayload[1] = powerManagement->averagePowerBudget;

  dimmNvList = GetDimmNvList(host, socket, ch);
  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, SET_POWER_MANAGEMENT, mbStatus);
  if (status == SUCCESS) {
    (*dimmNvList)[dimm].TDP = (UINT8) (inputPayload[0] >> 8);
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Set Power Management Policy Ends\n"));
#endif
  return status;

}
/**
*Updated 8/19/2014*
*NVMCTLR CIS 0.70*

Routine Description: SetDDRTAlert allows the user to setup host alerts
  @param host         - Pointer to the system host (root) structure
  @param socket       - Socket Number
  @param ch           - DDR Channel ID
  @param dimm         - DIMM Number
  @param fnvBuffer    - structure containing everything NVMCTLR
  @param hostAlerts   - New Host Alert Settings
  @param mbStatus     - Pointer to 8 bit status code from status register

  @retval 0 SUCCESS
  @retval 1 FAILURE
**/
UINT32
SetDDRTAlert (
              PSYSHOST     host,
              UINT8        socket,
              UINT8        ch,
              UINT8        dimm,
              UINT16       hostAlerts[NUM_HOST_TRANSACTIONS],
              UINT8        *mbStatus
)
{
  UINT32   inputPayload[NUM_PAYLOAD_REG];
  UINT32   outputPayload[NUM_PAYLOAD_REG];
  UINT32   status;
  UINT16   *pInput;
  UINT32   i;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Set NVMDIMM Alert Starts\n"));
#endif

  pInput = (UINT16*)&inputPayload[0];

  for (i = 0; i < MAX_HOST_TRANSACTIONS; i++) {
  pInput[i] = i < NUM_HOST_TRANSACTIONS ? hostAlerts[i] : 0;
  }

  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, SET_DDRT_ALERTS, mbStatus);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Set NVMDIMM Alert Ends\n"));
#endif

  return status;
} //SetDDRTAlert

/**
*Updated 10/15/2014*
*NVMCTLR CIS 0.81*

Routine Description: GetDDRTAlert allows the user to retrieve host alerts
  @param host         - Pointer to the system host (root) structure
  @param socket       - Socket Number
  @param ch           - DDR Channel ID
  @param dimm         - DIMM Number
  @param fnvBuffer    - structure containing everything NVMCTLR
  @param mbStatus     - Pointer to 8 bit status code from status register
  @retval 0 SUCCESS
  @retval 1 FAILURE
**/
static UINT32
GetDDRTAlert (
              PSYSHOST     host,
              UINT8        socket,
              UINT8        ch,
              UINT8        dimm,
              UINT16       hostAlerts[NUM_HOST_TRANSACTIONS],
              UINT8        *mbStatus
)
{
  UINT32   inputPayload[NUM_PAYLOAD_REG];
  UINT32   outputPayload[NUM_PAYLOAD_REG];
  UINT32   status;
  UINT16   *pOutput;
  UINT32   i;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Get NVMDIMM Alert Starts\n"));
#endif

  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, GET_DDRT_ALERTS, mbStatus);

  if (status == SUCCESS) {
  pOutput = (UINT16*)&outputPayload[0];
  for (i = 0; i < NUM_HOST_TRANSACTIONS; i++) {
      hostAlerts[i] = pOutput[i];
  }
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "Get NVMDIMM Alert Ends\n"));
#endif

  return status;
} //GetDDRTAlert

/**
*Updated 10/15/2014*
*NVMCTLR CIS 0.81*

Routine Description: SetSecurityNonce locks the SMM/BIOS mailbox. Stores the valid nonce into fnvBuffer if successful

  @param host         - Pointer to the system host (root) structure
  @param socket       - Socket Number
  @param ch           - DDR Channel ID
  @param dimm         - DIMM Number
  @param nonce        - 64 bit security nonce to unlock the BIOS mailbox
  @param mbStatus     - Pointer to 8 bit status code from status register
@retval 0 SUCCESS
@retval 1 FAILURE

**/
UINT32
SetSecurityNonce (
                  PSYSHOST       host,
                  UINT8          socket,
                  UINT8          ch,
                  UINT8          dimm,
                  UINT64_STRUCT  nonce,
                  FNVCISBUFFER   fnvBuffer,
                  UINT8          *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG];
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "SetSecurityNonce Starts\n"));
#endif

  inputPayload[0] = nonce.lo;
  inputPayload[1] = nonce.hi;
  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, SET_SECURITY_NONCE, mbStatus);
  if (status == SUCCESS){
    fnvBuffer->securityNonce.lo = nonce.lo;
    fnvBuffer->securityNonce.hi = nonce.hi;
  }
#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "SetSecurityNonce Ends\n"));
#endif
  return SUCCESS;
}

/**
*Updated 10/15/2014*
*NVMCTLR CIS 0.81*

Routine Description: Small Input Payload implemented.
1. transfer information into input payload registers
2. Write to SMM/BIOS or SMBus mailbox

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param partitionID  - 0 BIOS - only accessible via the BIOS mailbox 1 - OEM 2 - Namespace Label Storage Area
  @param offset      - offset in bytes of the partition to start reading
  @param length      - length in bytes to read from the offset
  @param fnvBuffer   - structure containing everything NVMCTLR
  @param mbStatus    - Pointer to 8 bit status code from status register

@retval 0 SUCCESS
@retval 1 FAILURE

**/
UINT32
SetPlatformConfigData(
                      PSYSHOST      host,
                      UINT8         socket,
                      UINT8         ch,
                      UINT8         dimm,
                      UINT8         partitionID,
                      UINT32        offset,
                      FNVCISBUFFER  fnvBuffer,
                      UINT8         *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG] = {0};
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status;
  UINT8 payloadtype;
  UINT8 i;
  UINT32 fwStatusLo = 0;
  UINT8  mediaStatus = 0;

  payloadtype = SMALL_PAYLOAD;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "SetPlatformConfigData Starts\n"));
#endif

  fwStatusLo = ReadFnvCfg (host, socket, ch, dimm, D_FW_STATUS_FNV_D_UNIT_0_REG);

  mediaStatus = (UINT8)((fwStatusLo >> 16) & 0x3);

  inputPayload[0] = partitionID | (payloadtype << 8) | (offset << 16);
  inputPayload[1] = offset >> 16;

  for(i = 0; i < 16; i++){
    inputPayload[i+16] = fnvBuffer->platformData[i];
  }

  if (mediaStatus == MEDIA_READY) {  // media ready
    status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, SET_PLATFORM_CONFIG, mbStatus);
  } else if (mediaStatus == MEDIA_NOTREADY) { // media not ready
    status = WARN_NVMCTRL_MEDIA_NOTREADY;
  } else if (mediaStatus == MEDIA_INERROR) { // media in error
    status = WARN_NVMCTRL_MEDIA_INERROR;
  } else {
    status = WARN_NVMCTRL_MEDIA_RESERVED;
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "SetPlatformConfigData Ends\n"));
#endif
  return status;
}

/**
*Updated 10/15/2014*
*NVMCTLR CIS 0.81*

Routine Description: GetPlatformConfigData will get the size of the platform configuration or data of the platform configuration
1. Get pointer to dimmConfig
2. Write partition ID and offset to the input register
3. Read platform configuration data from the SMBus mailbox or Smm mailbox
4. Transfer output payload registers to platform data buffer
5. return status

  @param host             - Pointer to the system host (root) structure
  @param socket           - Socket Number
  @param ch               - DDR Channel ID
  @param dimm             - DIMM number
  @param partitionID      - 0 BIOS - only accessible via the BIOS mailbox 1 - OEM 2 - Namespace Label Storage Area
  @param retreive option  - 0-partition data (default) 1- partition size
  @param offset           - offset in bytes of the partition to start reading
  @param fnvBuffer        - structure containing everything NVMCTLR
  @param mbStatus         - Pointer to 8 bit status code from status register

@retval 0 SUCCESS
@retval 1 FAILURE

**/
UINT32
GetPlatformConfigData(
                      PSYSHOST      host,
                      UINT8         socket,
                      UINT8         ch,
                      UINT8         dimm,
                      UINT8         partitionID,
                      UINT8         retrieve_option,
                      UINT32        offset,
                      FNVCISBUFFER  fnvBuffer,
                      UINT8         *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG] = {0};
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT8 i;
  UINT8 payloadtype = SMALL_PAYLOAD;
  UINT32 status;
  UINT32 fwStatusLo = 0;
  UINT8  mediaStatus = 0;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetPlatformConfigData Starts\n"));
#endif

#ifndef MEMMAPSIM_BUILD
  fwStatusLo = ReadFnvCfg (host, socket, ch, dimm, D_FW_STATUS_FNV_D_UNIT_0_REG);
#else
  fwStatusLo = MEDIA_READY << 16; //simulate that media is ready
#endif

  mediaStatus = (UINT8)((fwStatusLo >> 16) & 0x3);
  // If retrieving size, use small payload Registers
  inputPayload[0] = partitionID | (payloadtype << 8) | (retrieve_option << 9) | (offset << 16);
  inputPayload[1] = offset >> 16;

  if (mediaStatus == MEDIA_READY) {  // media ready
    status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, GET_PLATFORM_CONFIG, mbStatus);

    if (status == SUCCESS){
      if (retrieve_option == 1) 
        fnvBuffer->platformSize = outputPayload[0];
      else {
        for(i = 0; i < NUM_PAYLOAD_REG; i++) {
          fnvBuffer->platformData[i] = outputPayload[i];
        }
      }
    }
  } else if (mediaStatus == MEDIA_NOTREADY) { // media not ready
    status = WARN_NVMCTRL_MEDIA_NOTREADY;
  } else if (mediaStatus == MEDIA_INERROR) { // media in error
    status = WARN_NVMCTRL_MEDIA_INERROR;
  } else {
    status = WARN_NVMCTRL_MEDIA_RESERVED;
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetPlatformConfigData Ends\n"));
#endif

  return status;
}

/**
*Updated 10/15/2014*
*NVMCTLR CIS 0.81*

Routine Description: GetSecurityState will get the security state of the dimm
1. Read Security from SMBus mailbox
2. If mailbox read is successful, transfer output Payload register to security
3. return status

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param security    - pass by reference to security
  @param mbStatus    - Pointer to 8 bit status code from status register
@retval 0 SUCCESS
@retval 1 FAILURE

**/
UINT32
GetSecurityState (
                  PSYSHOST      host,
                  UINT8         socket,
                  UINT8         ch,
                  UINT8         dimm,
                  UINT8         *security,
                  UINT8         *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG];
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetSecurityState Starts\n"));
#endif

  status = FnvMailboxInterface (host, socket, ch, dimm, inputPayload, outputPayload, GET_SECURITY_STATE, mbStatus);
  if (status == SUCCESS){
    *security = (UINT8) outputPayload[0];
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetSecurityState Ends\n"));
#endif
  return status;
}

/**
*NVMCTLR CIS 0.82*
Routine Description: Secure Erase Unit will Erase the dimm
1. Transfer passphrase to input payload registers
2. Write to SMBus mailbox
3. if mailbox read fails, return failure
4. return status

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param passphrase  - passphrase
  @param mbStatus    - Pointer to 8 bit status code from status register
@retval 0 SUCCESS
@retval 1 FAILURE
**/
UINT32
SecureEraseUnit (
                  PSYSHOST      host,
                  UINT8         socket,
                  UINT8         ch,
                  UINT8         dimm,
                  UINT8         *passphrase,
                  UINT8         *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG];
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status;
  UINT8  i,j;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "SecureEraseUnit Starts\n"));
#endif
 i = 0;
 for (j = 0; j < PASSPHRASE_COUNTER; j++) {
    inputPayload[j] =  (passphrase[i + 3] << 24) | (passphrase[i + 2] << 16) | (passphrase[i + 1] << 8) | (passphrase[i]) ;
    i +=4;
  }

  status = FnvMailboxInterface (host, socket, ch, dimm, inputPayload, outputPayload, SECURE_ERASE_UNIT, mbStatus);


#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "SecureEraseUnit Ends\n"));
#endif

  return status;
}

/**
*NVMCTLR CIS 0.82*
Routine Description: Used to set security passphrase
1. Check if the dimm is locked and frozen
2. Transfer passphrase to input payload registers
3. Write to SMBus mailbox
4. if mailbox read fails, return failure
5. return status

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param currentPassphrase - current passphrase array of size 32
  @param newPassphrase - new passphrase array of size 32
  @param mbStatus      - Pointer to 8 bit status code from status register
@retval 0 SUCCESS
@retval 1 FAILURE
**/
UINT32
SetPassphrase(
                 PSYSHOST      host,
                 UINT8         socket,
                 UINT8         ch,
                 UINT8         dimm,
                 UINT8         security,
                 UINT8        *currentPassphrase,
                 UINT8        *newPassphrase,
                 UINT8        *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG];
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status;
  UINT8  i,j;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "setPassphrase Starts\n"));
#endif
  i = 0;
  for (j = 0; j < PASSPHRASE_COUNTER; j++) {
    inputPayload[j] =  (currentPassphrase[i + 3] << 24) | (currentPassphrase[i + 2] << 16) | (currentPassphrase[i + 1] << 8) | (currentPassphrase[i]) ;
    i +=4;
  }

  i = 0;
  for (j = NEW_PASSPHRASE_START; j < (NEW_PASSPHRASE_START + PASSPHRASE_COUNTER); j++) {
    inputPayload[j] =  (newPassphrase[i + 3] << 24) | (newPassphrase[i + 2] << 16) | (newPassphrase[i + 1] << 8) | (newPassphrase[i]) ;
    i +=4;

  }

  if ( !(security & SECURITY_LOCKED) && !(security & SECURITY_FROZEN) )
    status = FnvMailboxInterface (host, socket, ch, dimm, inputPayload, outputPayload, SET_PASSPHRASE, mbStatus);
  else
    status = FAILURE;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "setPassphrase Ends\n"));
#endif

  return status;
}



/**
*Updated 10/15/2014*
*NVMCTLR CIS 0.81*

Routine Description: SetDimmPartitionInfo will send in new partitions to NVMCTLR Dimm
1. Get the Pointer to dimmPartition
2. Get the security state of dimm
3. If mailbox read fails, return failure
4. If security state is frozen, then return failure
5. Transfer new partition values into input payload register
6. Write to SMBus mailbox
7. if mailbox read fails, return failure
8. Call GetDimmPartition to check if SetDimmPartitionInfo was successful. Older values will be store in FNVBuffer. Set Status accordingly.
9. return Status

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param twoLM       - 2LM volatile memory partition
  @param PM          - persistent memory partition
  @param fnvBuffer   - structure containing everything NVMCTLR
  @param mbStatus    - Pointer to 8 bit status code from status register
@retval 0 SUCCESS
@retval 1 FAILURE

**/
UINT32
SetDimmPartitionInfo(
                      PSYSHOST      host,
                      UINT8         socket,
                      UINT8         ch,
                      UINT8         dimm,
                      UINT32        twoLM,
                      UINT32        PM,
                      FNVCISBUFFER fnvBuffer,
                      UINT8        *mbStatus
)
{
  UINT32 inputPayload[NUM_PAYLOAD_REG] = {0};
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status;
  struct dimmPartition (*dimmPartitionList)[MAX_SOCKET][MAX_CH][MAX_DIMM];

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "SetDimmPartitionInfo Starts\n"));
#endif

  dimmPartitionList = GetFnvCisBufferDimmPartition(fnvBuffer);

  inputPayload[0] = twoLM;
  inputPayload[1] = PM;
  status = FnvMailboxInterface (host, socket, ch, dimm, inputPayload, outputPayload, SET_DIMM_PARTITION, mbStatus);

  //If mailbox read fails, return failure

  if (status == FAILURE) return status;

  // Get DIMM Partition Info
  status = GetDimmPartitionInfo(host, socket, ch, dimm, fnvBuffer, mbStatus);
  // if mailbox read fails, return failure
  if (status == FAILURE) return status;

  // If values are different, then write failed
  if (((*dimmPartitionList)[socket][ch][dimm].volatileCap == twoLM) &&
      ((*dimmPartitionList)[socket][ch][dimm].persistentCap == PM)) {
    status = SUCCESS;
  } else {
    status = FAILURE;
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "SetDimmPartitionInfo Ends\n"));
#endif
  return status;
}
/**
*Updated 10/15/2014*
*NVMCTLR CIS 0.81*

Routine Description: GetDimmPartitionInfo will store the partition info into the fnvBuffer.
1. Get the Pointer to dimmPartition
2. Read partition information from the SMBus mailbox
3. If read is succesful, then transfer all information from output payload registers into the fnvBuffer
4. return status

Manufacturer, Serial Number, and Model Number are dumped into a data buffer
  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param fnvBuffer   - structure containing everything NVMCTLR
  @param mbStatus     - Pointer to 8 bit status code from status register

@retval 0 SUCCESS
@retval 1 FAILURE

**/
UINT32
GetDimmPartitionInfo(
                       PSYSHOST     host,
                       UINT8        socket,
                       UINT8        ch,
                       UINT8        dimm,
                       FNVCISBUFFER fnvBuffer,
                       UINT8        *mbStatus
)
{
  UINT32 inputPayload [NUM_PAYLOAD_REG];
  UINT32 outputPayload [NUM_PAYLOAD_REG];
  struct dimmPartition (*dimmPartitionList)[MAX_SOCKET][MAX_CH][MAX_DIMM];
  UINT32 status;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetDimmPartitionInfo Starts\n"));
#endif

  dimmPartitionList = GetFnvCisBufferDimmPartition(fnvBuffer);

  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, GET_DIMM_PARTITION, mbStatus);
  if (status == SUCCESS) {
    (*dimmPartitionList)[socket][ch][dimm].volatileCap        = outputPayload[0]; // Volatile Capacity in 4KB granularity 3:0
    (*dimmPartitionList)[socket][ch][dimm].volatileStart.lo   = outputPayload[2]; // DPA start address of 2LM region Least Significant 11:8
    (*dimmPartitionList)[socket][ch][dimm].volatileStart.hi   = outputPayload[3]; // DPA start address of 2LM region Most Significant 15:12
    (*dimmPartitionList)[socket][ch][dimm].persistentCap      = outputPayload[4]; // Persisent Memory Capacitiy in 4KB granularity 19:16
    (*dimmPartitionList)[socket][ch][dimm].persistentStart.lo = outputPayload[6]; // DPA start address of PM region Least Significant 27:24
    (*dimmPartitionList)[socket][ch][dimm].persistentStart.hi = outputPayload[7]; // DPA start address of PM region Most Significant 31:28
    (*dimmPartitionList)[socket][ch][dimm].rc                 = outputPayload[8]; // Raw usable size of Volatile and Persisent in 4KB granularity 35:32
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetDimmPartitionInfo Ends\n"));
#endif
  return status;
}

/**
*Updated 10/13/2014*
*NVMCTLR CIS 0.81*

Routine Description: IdentifyDimm will store dimm information into the fnvBuffer.
1. Get the Pointer to dimmInfoList
2. Read dimm information from the SMBus mailbox
3. If read is succesful, then transfer all information from output payload registers into the fnvBuffer
4. return status

Manufacturer, Serial Number, and Model Number are dumped into a data buffer
  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param fnvBuffer   - structure containing everything NVMCTLR
  @param mbStatus    - Pointer to 8 bit status code from status register
@retval 0 SUCCESS
@retval 1 FAILURE

**/
UINT32
IdentifyDimm ( PSYSHOST            host,
               UINT8               socket,
               UINT8               ch,
               UINT8               dimm,
               DIMMINFORMATION     dimmInformation,
               UINT8               *mbStatus
)
{
  UINT32 inputPayload [NUM_PAYLOAD_REG];
  UINT32 outputPayload [NUM_PAYLOAD_REG];
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  UINT32 temp;
  UINT8 i;
  UINT32 status;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "IdentifyDimm Starts\n"));
#endif

  dimmNvList = GetDimmNvList(host, socket, ch);
  status = FnvMailboxInterface (host, socket, ch, dimm, inputPayload, outputPayload, IDENTIFY_DIMM, mbStatus);
  if (status == SUCCESS) {
    // Also save the VendorID & DeviceID also in the dimmNvList struct
    dimmInformation->vendorID = (*dimmNvList)[dimm].VendorIDIdentifyDIMM = (UINT16)outputPayload[0];       // Vendor ID Byte 1:0
    dimmInformation->deviceID = (*dimmNvList)[dimm].DeviceIDIdentifyDIMM = (UINT16)(outputPayload[0] >> 16);// Device ID Byte 3:2
    dimmInformation->revisionID        = (UINT16)outputPayload[1];                             // Revision ID Byte 5:4
    dimmInformation->ifc               = (UINT16)(outputPayload[1] >> 16);                     // Interface Format Code Byte 7:6
    dimmInformation->fwr.minorVersion  = (UINT8)outputPayload[2];                              // Firmware Revision Major Version Byte 8
    dimmInformation->fwr.majorVersion  = (UINT8)(outputPayload[2] >> 8);                       // Firmware Revision Minor Version Byte 9
    dimmInformation->fwr.hotfixVersion = (UINT8)(outputPayload[2] >> 16);                      // Firmware Revision Hot Fix Version Byte 10
    temp                                                = outputPayload[3] & 0x000000FF;
    dimmInformation->fwr.buildVersion  = (UINT16)((outputPayload[2] >> 24) | (temp << 8));     // Firmware Revision Build Version Byte 12:11
    dimmInformation->api               = (UINT8)(outputPayload[3] >> 8);                       // API Version Byte 13
    dimmInformation->fswr              = (UINT8)(outputPayload[3] >> 16);                      // Featured SW Required Mask Byte 14
  dimmInformation->ssid              = (UINT8)(outputPayload[3] >> 24);                      // Security SKU Id 15
    dimmInformation->nbw               = (UINT16)outputPayload[4];                             // Number of Block Windows Byte 17:16
    dimmInformation->nwfa              = (UINT16)(outputPayload[4] >> 16);                     // Number of Write Flush Addresses 19:18
    dimmInformation->wfas.lo           = outputPayload[5];                                     // Write Flush Address Start Least Significant 23:20
    dimmInformation->wfas.hi           = outputPayload[6];                                     // Write Flush Address Start Most Significant 27:24
    dimmInformation->ombcr             = outputPayload[7];                                     // Offset of Block Mode Control Region 31:28
    dimmInformation->rc                = outputPayload[8];                                     // Raw Capacity 35:32
    for (i = 0; i < MAX_MANUFACTURER_STRLEN; i++){
      dimmInformation->mf[i] = (UINT8) (outputPayload[9 + i /4] >> (8 * ((i) % 4)));  // Manufacturer 37:36
    }

    for (i = 0; i < MAX_SERIALNUMBER_STRLEN; i++){
      dimmInformation->sn[i] = (UINT8) (outputPayload[9 + (i + 2)/4] >> (8 * ((i + 2) % 4)));  // Serial Number 41:38
    }

    for (i = 0; i < MAX_MODELNUMBER_STRLEN; i++){
      dimmInformation->mn[i] = (UINT8) (outputPayload[10 + (i + 2)/4] >> (8 * ((i + 2) % 4)));  // Model Number 61:42
    }

    dimmInformation->dimmSku = (UINT8) (((outputPayload[15] >> 16) & 0x0000f) | ((outputPayload[16] << 4) & 0x0030)); //DimmSku 65:62
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "IdentifyDimm Ends\n"));
#endif
  return status;
}

/*
Routine Description: GetFnvInfo will store dimm information into the fnvBuffer.
1. Get the Pointer to dimmInfoList
2. Read dimm information from the SMBus mailbox
3. If read is succesful, then transfer all information from output payload registers into the fnvBuffer
4. return status

Manufacturer, Serial Number, and Model Number are dumped into a data buffer
  @param host           - Pointer to the system host (root) structure
  @param socket         - Socket Number
  @param ch             - DDR Channel ID
  @param dimm           - DIMM number
  @param FnvInformation - structure containing FNVINFORMATION
  @param mbStatus       - Pointer to 8 bit status code from status register
@retval 0 SUCCESS
@retval 1 FAILURE

*/
UINT32
GetFnvInfo (PSYSHOST            host,
            UINT8               socket,
            UINT8               ch,
            UINT8               dimm,
            FNVINFORMATION      FnvInformation,
            UINT8               *mbStatus
)
{
  UINT32 inputPayload [NUM_PAYLOAD_REG];
  UINT32 outputPayload [NUM_PAYLOAD_REG];
  UINT8  i;
  UINT32 status;

#ifdef PRINT_FUNC
  MemDebugPrint ((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetFnvInfo Starts\n"));
#endif

  status = FnvMailboxInterface (host, socket, ch, dimm, inputPayload, outputPayload, GET_FIRMWARE_IMAGE, mbStatus);
  if (status == SUCCESS) {
    FnvInformation->fwr.buildVersion   = (UINT16) outputPayload[0];                     // Firmware Revision Build Version Byte 1:0
    FnvInformation->fwr.hotfixVersion  = (UINT8) (outputPayload[0] >> 16);              // Firmware Revision Hot Fix Version Byte 2
    FnvInformation->fwr.minorVersion   = (UINT8) (outputPayload[0] >> 24);              // Firmware Revision Minor Version Byte 3
    FnvInformation->fwr.majorVersion   = (UINT8) outputPayload[1];                      // Firmware Revision Major Version Byte 4
    FnvInformation->fwrType            = (UINT8) (outputPayload[1] >> 8);               // Firmware Type Byte 5
    FnvInformation->sfwStatus          = (UINT8) outputPayload[4];
    FnvInformation->sfwr.buildVersion  = (UINT16) (outputPayload[4] >> 8);
    FnvInformation->sfwr.hotfixVersion = (UINT8) (outputPayload[4] >> 24);
    FnvInformation->sfwr.minorVersion  = (UINT8) outputPayload[5];
    FnvInformation->sfwr.majorVersion  = (UINT8) (outputPayload[5] >> 8);
    FnvInformation->sfwrType           = (UINT8) (outputPayload[5] >> 16);
    for (i = 0; i < COMMIT_ID; i++) {
      FnvInformation->commitId[i] = (UINT8) (outputPayload[8 + i/4] >> i % 4);
    }
  }

#ifdef PRINT_FUNC
  MemDebugPrint ((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                 "GetFnvInfo Ends\n"));
#endif
  return status;
}

#ifdef PRINT_FUNC
/**

Routine Description: Decodes bit mask to a string to make debug message easier to read

  @param bitmask     - Bit mask that identifies the Bit field in the Boot Status Register
  @retval string

**/
static char *GetBSRFieldStr(UINT32 bitmask, char *strBuf) {

  switch (bitmask) {
    case FNV_MB_READY:
      StrCpyLocal(strBuf, "Mailbox Ready");
      break;
    case FNV_DT_DONE:
      StrCpyLocal(strBuf, "NVMDIMM IO Init Complete");
      break;
    case FNV_MEDIA_DONE:
      StrCpyLocal(strBuf, "Media Ready");
      break;
    case FNV_CREDIT_READY:
      StrCpyLocal(strBuf, "Credit Ready");
      break;
    default:
      StrCpyLocal(strBuf, "Unknown BIT Field");
      break;
  }
  return strBuf;
}
#endif // PRINT_FUNC

/**

Routine Description: Decodes bit mask to a minor warning code

  @param bitmask     - Bit mask that identifies the Bit field in the Boot Status Register
  @retval minor Warning Code

**/
static UINT8
GetBSRMinorWarningCode (
                        UINT32 bitmask
)
{
  UINT8 minorWarning = 0;
  switch (bitmask) {
    case FNV_DT_DONE:
      minorWarning = WARN_DT_ERROR;
      break;
    case FNV_MEDIA_DONE:
      minorWarning = WARN_MEDIA_READY_ERROR;
      break;
    default:
      break;
  }
  return minorWarning;
}

/**

Routine Description: Returns simulation feedback for RcSim, CTE, and Simics

  @param bitmask     - Bit mask that identifies the Bit field in the Boot Status Register
  @retval feedback - Boot Status Register data

**/
static UINT32
GetBSRFeedback (
                UINT32 bitmask
)
{
  UINT32 feedback = 0;
  switch (bitmask) {
    case FNV_MB_READY:
      feedback = FNV_MB_READY;
      break;
    case FNV_DT_DONE:
      feedback = FNV_DT_DONE;
      break;
    case FNV_MEDIA_DONE:
      feedback = FNV_MEDIA_READY;
      break;
    case FNV_CREDIT_READY:
      feedback = FNV_CREDIT_READY;
      break;
    default:
      break;
  }
  return feedback;
}

/**

Routine Description: Determines if we need to continue polling

  @param bitmask     - Bit mask that identifies the Bit field in the Boot Status Register
  @retval status - 0 Stop Polling
  @retval status - 1 Continue Polling

**/
static UINT8
CheckforBSRError(
                 UINT32 bitmask,
                 UINT32 data,
                 UINT8  *error
)
{
  UINT8 status = 1;
  switch (bitmask) {
    case FNV_MB_READY:
      if (data == ((UINT32) -1)) {
        status = 1;
      } else if (data & FNV_MB_READY) {
        status = 0;
      } else {
        status = 1;
      }
      break;
    case FNV_DT_DONE:
      if (data & FNV_DT_DONE) {
        status = 0;
      } else {
        status = 1;
      }
      break;
    case FNV_CSR_UNLOCK:
      if (data & FNV_CSR_UNLOCK) {
        status = 1;
      } else {
        status = 0;
      }
      break;
    case FNV_MEDIA_DONE:
      if ((data & FNV_MEDIA_DONE) == FNV_MEDIA_READY) {
        status = 0;
      } else if ((data & FNV_MEDIA_DONE) ==  FNV_MEDIA_ERROR) {
        status = 0;
        *error = 1;
      } else {
        status = 1;
      }
      break;
    case FNV_CREDIT_READY:
      if (data & FNV_CREDIT_READY) {
        status = 0;
      } else {
        status = 1;
      }
      break;
    default:
      break;
  }
  return status;
}

/**

Routine Description: Returns Timeout required of each bit field

  @param bitmask     - Bit mask that identifies the Bit field in the Boot Status Register
  @retval delay - Returns the delay in microseconds

**/
static UINT32
GetDelayBSRTimeout (
                    UINT32 bitmask
)
{
  UINT32 delay = 0;

  switch (bitmask) {
    case FNV_MB_READY:
      delay = FNV_MB_READY_TIMEOUT;
      break;
    case FNV_DT_DONE:
    case FNV_CSR_UNLOCK:
      delay = FNV_DDRTIO_INIT_TIMEOUT;
      break;
    case FNV_MEDIA_DONE:
      delay = FNV_MEDIA_READY_TIMEOUT;
      break;
    case FNV_CREDIT_READY:
      delay = FNV_CREDIT_READY_TIMEOUT;
      break;
    default:
      break;
  }
  return delay;
}

/**

Routine Description: Converged Polling function of the Boot Status Register

@param host        - Pointer to the system host (root) structure
@param socket      - Socket Number
@param bitmask     - Bit mask that identifies the Bit field in the Boot Status Register
@retval 0 - success

**/
UINT32
FnvPollingBootStatusRegisterCh(
PSYSHOST host,
UINT8    socket,
UINT8    ch,
UINT32   bitmask
)
{
  UINT8               dimm;
  UINT32              startCount;
  UINT32              timeout;
  UINT8               error;
#ifdef PRINT_FUNC
#ifdef SERIAL_DBG_MSG
  char                strBuf[32] = {0};
#endif
#endif
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  D_FW_STATUS_FNV_D_UNIT_0_STRUCT fwStatusReg;

#ifdef PRINT_FUNC
    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "FnvPollingBootStatusRegister Starts: %s\n", GetBSRFieldStr(bitmask, strBuf)));
#endif

  dimmNvList = GetDimmNvList(host, socket, ch);
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

    //
    // Wait for mailbox ready with timeout
    //
    error = 0;
    startCount = GetCount (host);
    timeout = startCount + GetDelayBSRTimeout(bitmask); // Set Timeout
    do {
      fwStatusReg.Data = ReadFnvCfg (host, socket, ch, dimm, D_FW_STATUS_FNV_D_UNIT_0_REG);
   // SKX TODO: Use to break out of while loop for Simics
      if (host->var.common.emulation & SIMICS_FLAG) {
        fwStatusReg.Data |= GetBSRFeedback(bitmask);
      }

      // Check for TimeOut
      if (GetCount(host) > timeout) {
        OutputWarning (host, WARN_FNV_BSR, WARN_POLLING_LOOP_TIMEOUT, socket, ch, dimm, 0xFF);
        // Disable the channel on FW timeout
        DisableDIMM(host, socket, ch, dimm);
        break;
      }

    } while(CheckforBSRError(bitmask, fwStatusReg.Data, &error));

    if (error) {
      OutputWarning (host, WARN_FNV_BSR, GetBSRMinorWarningCode(bitmask), socket, ch, dimm, 0xFF);
      // Disable the channel on failure of BSR field
      DisableDIMM(host, socket, ch, dimm);
    }
  } //dimm

#ifdef PRINT_FUNC
    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "FnvPollingBootStatusRegister Ends: %s\n", GetBSRFieldStr(bitmask, strBuf)));
#endif

  return SUCCESS;
}
/**

Routine Description: Converged Polling function of the Boot Status Register

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param bitmask     - Bit mask that identifies the Bit field in the Boot Status Register
  @retval 0 - success

**/
UINT32
FnvPollingBootStatusRegister (
                              PSYSHOST host,
                              UINT8    socket,
                              UINT32   bitmask
)
{
  UINT8               ch;
  UINT8               dimm;
  UINT32              startCount;
  UINT32              timeout;
  UINT8               error;
#ifdef PRINT_FUNC
#ifdef SERIAL_DBG_MSG
  char                strBuf[32] = {0};
#endif
#endif
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  D_FW_STATUS_FNV_D_UNIT_0_STRUCT fwStatusReg;

#ifdef PRINT_FUNC
    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "FnvPollingBootStatusRegister Starts: %s\n", GetBSRFieldStr(bitmask, strBuf)));
#endif

    channelNvList = GetChannelNvList(host, socket);
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        //
        // Wait for mailbox ready with timeout
        //
        error = 0;
        startCount = GetCount (host);
        timeout = startCount + GetDelayBSRTimeout(bitmask); // Set Timeout
        do {
          fwStatusReg.Data = ReadFnvCfg (host, socket, ch, dimm, D_FW_STATUS_FNV_D_UNIT_0_REG);
   // SKX TODO: Use to break out of while loop for Simics
          if (host->var.common.emulation & SIMICS_FLAG) {
            fwStatusReg.Data |= GetBSRFeedback(bitmask);
          }

          // Check for TimeOut
          if (GetCount(host) > timeout) {
            OutputWarning (host, WARN_FNV_BSR, WARN_POLLING_LOOP_TIMEOUT, socket, ch, dimm, 0xFF);
            // Disable the channel on FW timeout
            DisableDIMM(host, socket, ch, dimm);
            break;
          }

        } while(CheckforBSRError(bitmask, fwStatusReg.Data, &error));

        if (error) {
          OutputWarning (host, WARN_FNV_BSR, GetBSRMinorWarningCode(bitmask), socket, ch, dimm, 0xFF);
          // Disable the channel on failure of BSR field
          DisableDIMM(host, socket, ch, dimm);
        }
      } //dimm
    } //ch

#ifdef PRINT_FUNC
    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "FnvPollingBootStatusRegister Ends: %s\n", GetBSRFieldStr(bitmask, strBuf)));
#endif

  return SUCCESS;
}

/**

Routine Description: Concurrently runs NVMCTLR NVMDIMM IO init on all NVMDIMMs attached to the socket

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @retval 0 - success

**/
UINT32
FnvDdrtIoInit(
              PSYSHOST      host,
              UINT8         socket,
              UINT8         payload
)
{
/*
    //
    // NVMCTLR NVMDIMM-IO init sequence
    //
    //   Assumptions:
    //   - Assume NVMCTLR data bus is isolated behind LR buffers, so no need to do DDR4 JEDEC init first.
    //     Need to test this assumption... e.g. has buffer been properly reset?
    //     What about IBT on CA signals?
    //   - Assume sufficient time has elapsed from NVMCTLR power-good until first SMBus transaction to SPD, RCD or NVMCTLR
    //
    //   Steps:
    //   1. Poll for NVMCTLR mailbox ready on each NVMDIMM per socket
    //   2. If NVMDIMM IO init not done, then issue mailbox command on each NVMDIMM per socket
    //   3. Poll for completion on each NVMDIMM and update command results
    //   4. Map-out any channels failing NVMCTLR NVMDIMM IO init command
    //
*/

  UINT8               ch;
  UINT8               dimm;
  UINT32              status;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  UINT32              outputPayload[NUM_PAYLOAD_REG];
  UINT32              inputPayload[NUM_PAYLOAD_REG];
  UINT32              opcode;
  UINT8               mbStatus = 0;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "FnvDdrtIoInit Starts\n"));
#endif

  channelNvList = GetChannelNvList(host, socket);

  //
  // Issue NVMDIMM IO init command
  // Note: NVMCTLR sense amp calibration does not run on CMD/CTL signals
  //
  opcode = DDRT_IO_INIT;
  if (payload == PLL_LOCK){
    inputPayload[0] = BIT7;
  } else if (payload == DDRT_FREQ){
    inputPayload[0] = host->nvram.mem.socket[socket].ddrtFreq;
  } else if (payload == DDRT_TRAINING_COMPLETE) {
    inputPayload[0] = BIT8;
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

      status = SUCCESS;

        status = FnvMailboxInterfaceStart (host, socket, ch, dimm, inputPayload, opcode);
      if (status != SUCCESS) {
        //Disable channel
      }
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                    "NVMCTL NVMDIMM IO Init started\n"));
    } //dimm
  } //ch

  // SKX TODO: need to enforce a timeout

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      status = FnvMailboxInterfaceFinish (host, socket, ch, dimm, outputPayload, opcode, &mbStatus);
      if (status != SUCCESS) {
        //Disable channel
      }
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                    "NVMCTL NVMDIMM IO Init completed\n"));
    } //dimm


  } //ch
#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "FnvDdrtIoInit Ends\n"));
#endif

  return SUCCESS;
}


/**
*Updated 8/19/2014*
*NVMCTLR CIS 0.70*

Routine Description: FnvMailboxInterface is a generic read/write to the Smm mailbox.
Smm mailbox will support SMBus or CPGC reads and SMbus, EMRS, or CPGC writes

Program Flow:
1. Look if Opcode has been implemented. If not, return failure
2. Setup Timeout for the doorbell
3. Read the Command Register for the Smm Mailbox. If Doorbell is idle, then breakout of while loop.
   If timeout has expired, log a warning and return failure.
4. Write the opcode into the Command Register
5. Fill in payload registers if necessary
6. Write Security nonce. (Not implemented yet. No nonce register defined in Headerfile 10/25)
7. Set Doorbell to busy
  @param host          - Pointer to the system host (root) structure
  @param socket        - Socket Number
  @param ch            - DDR Channel ID
  @param dimm          - DIMM number
  @param inputPayload  - input Payload Register
  @param opcode        - NVMCTLR command (subopcode | opcode)
@retval 0 SUCCESS
@retval 1 FAILURE
**/
static UINT32
FnvMailboxInterfaceStart (
                     PSYSHOST host,
                     UINT8    socket,
                     UINT8    ch,
                     UINT8    dimm,
                     UINT32   inputPayload[NUM_PAYLOAD_REG],
                     UINT32   opcode
)
{
  MB_SMBUS_CMD_FNV_D_UNIT_0_STRUCT                 SMBusCommandRegister;
  MB_SMM_CMD_FNV_DPA_MAPPED_0_STRUCT               SmmCommandRegister;
  UINT32                                           startCount;
  UINT32                                           timeout;
  UINT8                                            opcodeIndex;
  UINT32                                           payloadCount;
  UINT8                                            i;
  UINT32                                           doorbell;
  UINT8                                            smm = 0;
  UINT64_STRUCT                                    csrData64;
  CSR_OFFSET                                       RegOffset;
  struct channelNvram                              (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  // Choose appropriate CSRs
  switch ((*channelNvList)[ch].fnvAccessMode) {
    case SMBUS:
    case EMRS:
      smm = 0;
      break;
    case CPGC:
    case SAD:
      smm = 1;
      break;
    default:
      break;
  }

  // Find NVMCTLR Opcode
  for (i = 0; i < FNV_OPCODE_ARRAY_SIZE; i++){
    if (opcode == FnvPayloadLookupTable [i][OPCODE_INDEX]) break;
  }
  opcodeIndex = i;
  if (i >= FNV_OPCODE_ARRAY_SIZE || (FnvPayloadLookupTable[opcodeIndex][OPCODE_INDEX] == 0)){
    OutputWarning (host, WARN_INVALID_FNV_OPCODE, WARN_OPCODE_INDEX_LOOKUP, socket, ch, dimm, 0xFF);
    return FAILURE;
  }

  // Timeout Reading Doorbell 190 uS
  startCount = GetCount (host);
  timeout = startCount + DOORBELL_TIMEOUT; // Set Timeout

  // Enable Simics Call for NVMDIMM X'tor
  {

    // Check/Wait for doorbell idle status
    while (1) {
      if (smm) {
        SmmCommandRegister.Data = ReadFnvCfg (host, socket, ch, dimm, MB_SMM_CMD_FNV_DPA_MAPPED_0_REG);
        doorbell = SmmCommandRegister.Bits.doorbell;
      } else {
        SMBusCommandRegister.Data = ReadFnvCfg (host, socket, ch, dimm, MB_SMBUS_CMD_FNV_D_UNIT_0_REG);
        doorbell = SMBusCommandRegister.Bits.doorbell;
      }

  // SKX TODO: Simics support is missing
      if (host->var.common.emulation & SIMICS_FLAG) {
        doorbell = 0;
      }
      // Break out of polling loop
      if (doorbell == 0) break;

      // Check for TimeOut
      if (GetCount(host) > timeout) {
        OutputWarning (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_TIMEOUT, socket, ch, dimm, 0xFF);
        DisableChannel(host, socket, ch);
        return FAILURE;
      }
    }
  }
  //Send Opcode to Mailbox
  if (smm) {
    SmmCommandRegister.Bits.opcode = opcode;
    SmmCommandRegister.Bits.spares = 0;
    RegOffset.Data = MB_SMM_CMD_FNV_DPA_MAPPED_0_REG;
    // RegOffset.Bits.size = 0;      //select quadword
    csrData64.lo = SmmCommandRegister.Data;
    csrData64.hi = 0;
    WriteFnvCfgDpa (host, socket, ch, dimm, RegOffset.Data, csrData64);
  } else {
    SMBusCommandRegister.Bits.opcode = opcode;
    WriteFnvCfg (host, socket, ch, dimm, MB_SMBUS_CMD_FNV_D_UNIT_0_REG, SMBusCommandRegister.Data);
  }

  // Fill Input Payload Registers
  payloadCount = FnvPayloadLookupTable [opcodeIndex][INPUT_PAYLOAD_INDEX];
  for (i = 0; i < payloadCount; i++){
    if (smm) {
      RegOffset.Data = MB_SMM_INPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG + (i * 4);
      RegOffset.Bits.size = 0;      //select quadword
      csrData64.lo = inputPayload[i++];
      csrData64.hi = inputPayload[i];
      WriteFnvCfgDpa (host, socket, ch, dimm, RegOffset.Data, csrData64);
    } else {
      WriteFnvCfg (host, socket, ch, dimm, MB_SMBUS_INPUT_PAYLOAD_0_FNV_D_UNIT_0_REG + (i * 4) , inputPayload[i]);
    }
  }

  // Reg should be the address of the NONCE register
  // if (FnvPayloadLookupTable [opcodeIndex][RW_INDEX]){
  //   WriteFnvCfg(host, socket, ch, dimm, CPGC, reg, fnvBuffer->securityNonce.lo);
  //   WriteFnvCfg(host, socket, ch, dimm, CPGC, reg, fnvBuffer->securityNonce.hi);
  // }

  //Set doorbell to busy status
  if (smm) {
    SmmCommandRegister.Bits.doorbell = BUSY;
    SmmCommandRegister.Bits.spares = 0;
    RegOffset.Data = MB_SMM_CMD_FNV_DPA_MAPPED_0_REG;
    // RegOffset.Bits.size = 0;      //select quadword
    csrData64.lo = SmmCommandRegister.Data;
    csrData64.hi = 0;
    WriteFnvCfgDpa (host, socket, ch, dimm, RegOffset.Data, csrData64);
  } else {
    SMBusCommandRegister.Bits.doorbell = BUSY;
    WriteFnvCfg (host, socket, ch, dimm, MB_SMBUS_CMD_FNV_D_UNIT_0_REG, SMBusCommandRegister.Data);
  }

  return SUCCESS;
}

/**
*Updated 8/19/2014*
*NVMCTLR CIS 0.70*

Routine Description: FnvMailboxInterface is a generic read/write to the Smm mailbox.
Smm mailbox will support SMBus or CPGC reads and SMbus, EMRS, or CPGC writes

Program Flow:
1. Look if Opcode has been implemented. If not, return failure
2. Poll for Completion in the status register
3. Read the status code in the status register. Return if any failures occured.
4. Read output payload registers if necessary
5. Return SUCCESS
  @param host          - Pointer to the system host (root) structure
  @param socket        - Socket Number
  @param ch            - DDR Channel ID
  @param dimm          - DIMM number
  @param inputPayload  - input Payload Register
  @param outputPayload - Output Payload Register
  @param opcode        - NVMCTLR command (subopcode | opcode)
@retval 0 SUCCESS
@retval 1 FAILURE
**/
static UINT32
FnvMailboxInterfaceFinish (
                           PSYSHOST     host,
                           UINT8        socket,
                           UINT8        ch,
                           UINT8        dimm,
                           UINT32       outputPayload[NUM_PAYLOAD_REG],
                           UINT32       opcode,
                           UINT8        *mbStatus
)

{
  MB_SMBUS_STATUS_FNV_D_UNIT_0_STRUCT              SMBusStatusRegister;
  MB_SMM_STATUS_FNV_DPA_MAPPED_0_STRUCT            SmmStatusRegister;
  UINT32                                           payloadCount;
  UINT8                                            i;
  UINT32                                           startCount;
  UINT32                                           timeout;
  UINT8                                            opcodeIndex;
  UINT8                                            smm = 0;
  UINT32                                           comp;
  UINT32                                           stat = 0;
  UINT64_STRUCT                                    csrData64;
  CSR_OFFSET                                       RegOffset;
  struct channelNvram                              (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  // Choose appropriate CSRs
  switch ((*channelNvList)[ch].fnvAccessMode) {
    case SMBUS:
    case EMRS:
      smm = 0;
      break;
    case CPGC:
    case SAD:
      smm = 1;
      break;
    default:
      break;
  }

  // Find NVMCTLR Opcode
  for (i = 0; i < FNV_OPCODE_ARRAY_SIZE; i++){
    if (opcode == FnvPayloadLookupTable [i][OPCODE_INDEX]) break;
  }
  opcodeIndex = i;
  if (i >= FNV_OPCODE_ARRAY_SIZE || (FnvPayloadLookupTable[opcodeIndex][OPCODE_INDEX] == 0)){
    OutputWarning (host, WARN_INVALID_FNV_OPCODE, WARN_OPCODE_INDEX_LOOKUP , socket, ch, dimm, 0xFF);
    return FAILURE;
  }

  startCount = GetCount (host);
  timeout = startCount + COMP_TIMEOUT; // Set COMP Timeout
  if (host->var.common.emulation & SIMICS_FLAG) {
    timeout = startCount + 700;
  }
  // Enable Simics Call for NVMDIMM X'tor
  {
  // Poll for completion and status
    while (1){
      if (smm) {
        SmmStatusRegister.Data = ReadFnvCfg (host, socket, ch, dimm, MB_SMM_STATUS_FNV_DPA_MAPPED_0_REG);
        comp = SmmStatusRegister.Bits.comp;
        stat = SmmStatusRegister.Bits.stat;
        *mbStatus = (UINT8)stat;
      } else {
        SMBusStatusRegister.Data = ReadFnvCfg (host, socket, ch, dimm, MB_SMBUS_STATUS_FNV_D_UNIT_0_REG);
        comp = SMBusStatusRegister.Bits.comp;
        stat = SMBusStatusRegister.Bits.stat;
        *mbStatus = (UINT8)stat;
      }

  // SKX TODO: Simics support is missing
      if (host->var.common.emulation & SIMICS_FLAG) {
        comp = 1;
        stat = 0;
      }
      if (comp) break;

      if (GetCount(host) > timeout) {
        OutputWarning (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_TIMEOUT, socket, ch, dimm, 0xFF);
        DisableChannel(host, socket, ch);
        return FAILURE;
      }
    }
  }

  // Read Status
  if (StatusDecoder(host, socket, ch, dimm, stat) == FAILURE) {
    return FAILURE;
  }
  // Read Payload Registers
  payloadCount = FnvPayloadLookupTable [opcodeIndex][OUTPUT_PAYLOAD_INDEX];

  for (i = 0; i < payloadCount; i++){
    if (smm) {
      RegOffset.Data = MB_SMM_OUTPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG + (i * 4);
      RegOffset.Bits.size = 0;      //select quadword
      ReadFnvCfgDpa (host, socket, ch, dimm, RegOffset.Data, &csrData64);
      outputPayload [i++] = csrData64.lo;
      outputPayload [i] = csrData64.hi;
    } else {
      outputPayload [i] = ReadFnvCfg (host, socket, ch, dimm, MB_SMBUS_OUTPUT_PAYLOAD_0_FNV_D_UNIT_0_REG + (i * 4));
    }
  }

  return SUCCESS;
}

/**
*Updated 8/19/2014*
*NVMCTLR CIS 0.70*

Routine Description: FnvMailboxInterface is a generic read/write to the BIOS/SMM mailbox. BIOS mailbox will always use CPGC interface
Program Flow:

  @param host          - Pointer to the system host (root) structure
  @param socket        - Socket Number
  @param ch            - DDR Channel ID
  @param dimm          - DIMM number
  @param inputPayload  - input Payload Register
  @param outputPayload - Output Payload Register
  @param opcode        - NVMCTLR command (subopcode | opcode)
@retval 0 SUCCESS
@retval 1 FAILURE
**/
static UINT32
FnvMailboxInterface (
                   PSYSHOST     host,
                   UINT8        socket,
                   UINT8        ch,
                   UINT8        dimm,
                   UINT32       inputPayload[NUM_PAYLOAD_REG],
                   UINT32       outputPayload[NUM_PAYLOAD_REG],
                   UINT32       opcode,
                   UINT8        *mbStatus
)
{
  UINT32            status;

#ifdef MEMMAPSIM_BUILD
  status = MMS_FnvMailboxInterface( host, socket, ch, dimm, inputPayload, outputPayload, opcode );
  *mbStatus = 0;
#else
  status = FnvMailboxInterfaceStart (host, socket, ch, dimm, inputPayload, opcode);
  if (status == SUCCESS) {
    status = FnvMailboxInterfaceFinish (host, socket, ch, dimm, outputPayload, opcode, mbStatus);
  }
#endif
  return status;
}


/**

Routine Description: ReadFnvCfg will read a NVMCTLR register via SMBus or CPGC interface.
This will return the data read from a NVMCTLR register because ReadFnvCfgSmb and ReadFnvCfgDpa require different size of data types
If no interface is supported, then enter while(1)

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param mode        - Type of interface SMBus, EMRS, or CPGC
  @param reg         - NVMCTLR register Address
@retval Data from NVMCTLR register

**/
UINT32
ReadFnvCfg (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT32    reg
)
{
  UINT64_STRUCT          localData = {0,0};
  BUS_TYPE               mode;
  struct channelNvram    (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  mode = (*channelNvList)[ch].fnvAccessMode;

#ifdef COSIM_EN
  if (mode == CPGC) {
    mode = SMBUS;
  }
#endif
  switch (mode) {
    case SMBUS:
    case EMRS:
      ReadFnvCfgSmb (host, socket, ch, dimm, reg, &localData.lo);
      break;
    case CPGC:
      ReadFnvCfgDpa (host, socket, ch, dimm, reg, &localData);
      break;
    default:
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_33);
      break;
  }
  return localData.lo;
}


/**

Routine Description:WriteFnvCfg will write 32 bits of data into a NVMCTLR register.
If no interface is supported, then enter while(1)

  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param mode        - Type of interface SMBUS, EMRS, or CPGC
  @param reg         - NVMCTLR register Address
  @param data        - Data to be written to NVMCTLR register
  @retval 0 - success

**/
UINT32
WriteFnvCfg (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT32    reg,
              UINT32    data
)
{
  UINT64_STRUCT          localData;
  BUS_TYPE               mode;
  struct channelNvram    (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  mode = (*channelNvList)[ch].fnvAccessMode;
#ifdef COSIM_EN
  if (mode == CPGC) {
    mode = SMBUS;
  }
#endif
  localData.lo = data;
  switch (mode) {
    case SMBUS:
      WriteFnvCfgSmb (host, socket, ch, dimm, reg, data);
      break;
    case EMRS:
      WriteFnvCfgEmrs (host, socket, ch, dimm, reg, data);
      break;
    case CPGC:
      WriteFnvCfgDpa (host, socket, ch, dimm, reg, localData);
      break;
    default:
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_34);
      break;
  }
  return SUCCESS;
}

/**

Routine Description: Status Decoder will print status message after issuing a NVMCTLR Command


  @param host        - Pointer to the system host (root) structure
  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param mbstatus    - 8 bit status code from status register
  @retval 0 - success
  @retval 1 - failure

**/
UINT32
StatusDecoder (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT32    mbstatus
)
{
  UINT32 status;
  switch(mbstatus) {
    case 0x00:
      status = SUCCESS;
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox SUCCESS\n"));
      break;
    case 0x01:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Invalid Command Paramter\n"));
      break;
    case 0x02:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Data Transfer Error\n"));
      break;
    case 0x03:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Internal Device Error\n"));
      break;
    case 0x04:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Unsupported Command\n"));
      break;
    case 0x05:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Device Busy\n"));
      break;
    case 0x06:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Incorrect Passphrase/Security Nonce\n"));
      break;
    case 0x07:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Security Check Fail\n"));
      break;
    case 0x08:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Invalid Security State\n"));
      break;
    case 0x09:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure System Time Not Set\n"));
      break;
    case 0x0A:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Data Not Set\n"));
      break;
    case 0x0B:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure Aborted\n"));
    case 0x0C:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox No New FW to Execute\n"));
    case 0x0D:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Revision Failure\n"));
    case 0x0E:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Injection Not Enabled\n"));
    case 0x0F:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Config Locked Command Invalid\n"));
    default:
      status = FAILURE;
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "NVMCTL Mailbox Failure No Status Message\n"));
      break;
    }
  return status;
}

/**
*Updated 8/19/2014*
*NVMCTL CIS 0.70*

Routine Description: SetDieSparingPolicy Sets policy for NGN die sparing.

  @param host           - Pointer to the system host (root) structure
  @param socket         - Socket Number
  @param ch             - DDR Channel ID
  @param dimm           - DIMM Number
  @param Enable         - Enable Die sparing
  @param Aggressiveness - Level of Aggressiveness
  @param mbStatus       - Pointer to 8 bit status code from status register
  @retval 0 SUCCESS
  @retval 1 FAILURE

**/

UINT32
SetDieSparingPolicy (
                     PSYSHOST       host,
                     UINT8          socket,
                     UINT8          ch,
                     UINT8          dimm,
                     UINT8          Enable,
                     UINT8          Aggressiveness,
                     UINT8          *mbStatus
)
{

  UINT32 inputPayload[NUM_PAYLOAD_REG] = {0};
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status = SUCCESS;

#ifdef PRINT_FUNC
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                      "Setting Die Sparing Policy for dimm %d on channel %d\n", dimm, ch));
#endif
  inputPayload[0] = Enable | (Aggressiveness << 8);
  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, SET_DIE_SPARING, mbStatus);

  return status;
}

/**
*Updated 8/19/2014*
*NVMCTLR CIS 0.70*

Routine Description: GetDieSparingPolicy Gets status of NGN die sparing.

  @param host           - Pointer to the system host (root) structure
  @param socket         - Socket Number
  @param ch             - DDR Channel ID
  @param dimm           - DIMM Number
  @param Enable         - Enable Die sparing
  @param Aggressiveness - Level of Aggressiveness
  @param mbStatus     - Pointer to 8 bit status code from status register

  @retval 0 SUCCESS
  @retval 1 FAILURE

**/

static UINT32
GetDieSparingPolicy (
                     PSYSHOST       host,
                     UINT8          socket,
                     UINT8          ch,
                     UINT8          dimm,
                     UINT8          *Enable,
                     UINT8          *spareAggressiveness,
                     UINT8          *supported,
                     UINT8          *mbStatus
)
{

  UINT32 inputPayload[NUM_PAYLOAD_REG];
  UINT32 outputPayload[NUM_PAYLOAD_REG];
  UINT32 status;

  status = FnvMailboxInterface(host, socket, ch, dimm, inputPayload, outputPayload, GET_DIE_SPARING, mbStatus);
  if (status == SUCCESS){
    *Enable               = (UINT8)(outputPayload[0]);
    *spareAggressiveness  = (UINT8)(outputPayload[0] >> 8);
    *supported = (UINT8)(outputPayload[0] >> 16);
  }
  return status;
}


/**

Routine Description: This is a generic Write to IO

  1. Write Data into Window Register MC_WDATA for Write only
  2. Write Bus, Device, and Function into Account Info
     Bits   [11]   - IA
     Bits   [10:8] – Bus – (Contrary to what the documentation may say, this is 2 for both NGN and NVMDIMM IO)
     Bits   [7:3]  – Device – Found in CSR description document (CSPEC/XML).
     Bits   [2:0]  – Function – Found in CSR description document (CSPEC/XML).
  3. Configure control Register
  4. Wait for completion for reads and nonposted writes
  @param host        - Pointer to the system host (root) structure
  @retval Data or Status depending on Read or Write

**/
UINT32
GetSetFnvIO (
              PSYSHOST host,
              UINT8    socket,
              UINT8    ch,
              UINT8    dimm,
              UINT32   reg,
              UINT32   *data,
              UINT8    IA,
              UINT8    mode,
              UINT8    byteMask // 0xF
)
{
  CSR_OFFSET                          RegOffset;
  UINT8                               func;
  UINT8                               device;
  D_MC_ACC_INFO_FNV_D_UNIT_0_STRUCT   windowAccountInfo;
  D_MC_CTRL_FNV_D_UNIT_0_STRUCT       windowControl;
  D_MC_STAT_FNV_D_UNIT_0_STRUCT       windowStatus;
  UINT32 startCount;
  UINT32 timeout;

  //
  // 1. Write Data into Window Register MC_WDATA for Write only
  //
  if (mode == FNV_IO_WRITE) {
    WriteFnvCfg (host, socket, ch, dimm, D_MC_WDATA_FNV_D_UNIT_0_REG, *data);
  }

  //
  // 2. Write IA, Bus, Device, and Function into Account Info
  //
  RegOffset.Data = reg;
  windowAccountInfo.Data = 0;
  device = 22;
  switch(RegOffset.Bits.funcblk) {
    case 0x0E:
      func = 0;
      break;
    case 0x0F:
      func = 1;
      break;
    case 0x10:
    default:
      func = 6;
      break;
  }
  windowAccountInfo.Bits.mc_acc_info = func| (device << 3) | (2 << 8) | (IA << 11); // Added place holder
  WriteFnvCfg (host, socket, ch, dimm, D_MC_ACC_INFO_FNV_D_UNIT_0_REG, windowAccountInfo.Data);

  //
  // 3. Configure Control CSR
  //
  windowControl.Data = 0;
  windowControl.Bits.mc_opcode = mode; // Read = 0 Write = 1
  windowControl.Bits.mc_port = 0x71;   // Port ID for IO CSR is 0x71
  if (mode == FNV_IO_WRITE) {
    windowControl.Bits.mc_posted = 1;  // Write posted set to 0 for now. Add lookup Table Later
  } else {
    windowControl.Bits.mc_posted = 0;  // Reads are always nonposted
  }
  windowControl.Bits.mc_addr = (RegOffset.Bits.offset >> 2) | (func << 10); // Address bits <11:2>
  windowControl.Bits.mc_be = byteMask; // Byte Enable Mask Field
  windowControl.Bits.mc_start = 0x01;  // Kick off the command
  WriteFnvCfg (host, socket, ch, dimm, D_MC_CTRL_FNV_D_UNIT_0_REG, windowControl.Data);

  //
  // 4. Wait for completion for reads and nonposted writes
  // TODO: add real tmeout
  // Processor timestamp counter
  if (windowControl.Bits.mc_posted == 0) {
    startCount = GetCount (host);
    timeout = startCount + WINDOW_TIMEOUT; // Set Timeout
    if (host->var.common.emulation & SIMICS_FLAG) {
      timeout = startCount + 100;
    }
    if (mode == FNV_IO_READ) {

      do {
        windowStatus.Data = ReadFnvCfg (host, socket, ch, dimm, D_MC_STAT_FNV_D_UNIT_0_REG);
        if (GetCount(host) >= timeout) {
          // Log Warning
          return FAILURE;
        }

      } while (!windowStatus.Bits.mc_rdata_vld); // Poll for Read Data Valid Bit

      if (windowStatus.Bits.mc_rdata_src != 0) {
        // Log Warning if you read from DA register
      }
      *data = ReadFnvCfg (host, socket, ch, dimm, D_MC_RDATA_FNV_D_UNIT_0_REG);
      return SUCCESS;
    } else {
      do {
        windowStatus.Data = ReadFnvCfg (host, socket, ch, dimm, D_MC_STAT_FNV_D_UNIT_0_REG);
        if (GetCount(host) >= timeout) {
          // Log Warning
          return FAILURE;
        }
      } while (!windowStatus.Bits.mc_resp); // Poll for a response

    }
  } // nonposted

  if (mode == FNV_IO_WRITE) {
    IODdrtResetPerCh(host, socket, ch, dimm);
  }
  return SUCCESS;
} // GetSetFnvIO

/**

  Routine Description: This function reads the Major and Minor Checkpoint of NGN training
  Bits  7 - 0 represent the Major Checkpoint
  Bits 15 - 8 represent the Minor Checkpoint
  @param host        - Pointer to the system host (root) structure
  @retval Data or Status depending on Read or Write
**/
static UINT32
GetFnvTrainingStatus (
                      PSYSHOST host,
                      UINT8    socket,
                      UINT8    ch,
                      UINT8    dimm
)
{
  D_FW_STATUS_FNV_D_UNIT_0_STRUCT fwStatusReg;

  fwStatusReg.Data = ReadFnvCfg(host, socket, ch, dimm, D_FW_STATUS_FNV_D_UNIT_0_REG);
  return (fwStatusReg.Data & (MAJOR_CHECKPOINT | MINOR_CHECKPOINT));
} // GetFnvTrainingStatus


/**
  Routine Description: This function returns a random 32 bit number
  @param host    - pointer to System Host (root) structure
  @param EaxReg  - data
  1. Check if processor supports RDRAND
  2. Call rdrand
  3. Check Carry Flag to see if rdrand failed
  @retval status
**/
UINT32
RdRand32(
       PSYSHOST host,
       UINT32*  EaxReg
)
{
#ifdef IA32
#ifdef __GNUC__
  asm (
            "movl    %0, %%esi\n\t"
    "repeat:\n\t"
            ".byte   $0x0f\n\t"
            ".byte   $0xc7\n\t"
            ".byte   $0xf0\n\t"
            "jnc     repeat\n\t"
            "movl    %%eax, (%%esi)\n\t"
            : : "r"(EaxReg) : "%esi"
  );
#else
  _asm {
            mov     esi, EaxReg
    repeat:
            _emit   0x0f
            _emit   0xc7
            _emit   0xf0
            jnc     repeat
            mov     dword ptr [esi], eax
  }
#endif
#endif
  return SUCCESS;
}
