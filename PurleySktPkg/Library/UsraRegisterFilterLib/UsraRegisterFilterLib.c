//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
Implementation of UsraRegisterFilterLib class library.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef MINIBIOS_BUILD
#include <Library/UsraAccessApi.h>
#include <Library/IoLib.h>
#endif
#include <CoreApi.h>
#include <SysFunc.h>
#include <KtiPlatform.h>
#include "ScratchPadReg.h"
#include <RcRegs.h>
#include "CpuPciAccess.h"
#include "SysHostChip.h"

#ifdef MINIBIOS_BUILD
RETURN_STATUS RegAlignedRead (UINT32, UINTN, VOID*);
RETURN_STATUS RegAlignedWrite (UINT32, UINTN, VOID*);
#endif


#ifdef MRC_TRACE
#ifdef SERIAL_DBG_MSG
/**
    Capture the CSR access in format:

    *Socket, Type, Address, Direction, Size, Data

    Where:
     - Socket = processor socket # that executes the instruction
     - Type = 0 for IO, or 1 for Memory
     - Address = 16-bit IO or 32-bit Memory address (the MMIO address will encode bus, device, function, offset according to MMCFG format)
     - Direction = 0 for Rd, or 1 for Wr
     - Size = 1, 2, or 4 bytes
     - Data = hex data corresponding to the size

    For example:

     *0, 0, 0x80, 1, 1, 0xA0
     *0, 1, 0x100800dc, 1, 4, 0x00000055
     *0, 1, 0x10000000, 0, 4, 0x36008086
     *0, 1, 0x10000008, 0, 1, 0x00

    @param host      - pointer to the system host root structure
    @param SocId     - socket ID of the processor executing CSR access
    @param RegAddr   - CSR address in BDFO format
    @param RegOffset - CSR offset in Box/Type format
    @param Offset    - CSR offset in Box/Type format
    @param Data      - Data being read/written
    @param Size      - Size of the CSR being accessed
    @param RwFlag    - 0 for read, 1 for write

    @retval None

**/
VOID
CpuCsrAccessTraceCapEx (
  PSYSHOST host,
  UINT8 SocId,
  UINT8 *RegAddr,
  CSR_OFFSET RegOffset,
  UINT32 Offset,
  UINT32 Data,
  UINT8  Size,
  UINT8 RwFlag
)
{
  if (host == NULL) return;
  if (RwFlag == READ_ACCESS) {
    if (checkMsgLevel(host, SDBG_TRACE)) {
      if (!((RegOffset.Data  == SR_POST_CODE_CSR)||(RegOffset.Data  == SR_BIOS_SERIAL_DEBUG_CSR)||(RegOffset.Data  == SR_PRINTF_SYNC_CSR)\
            ||(RegOffset.Data == SMB_CMD_CFG_0_PCU_FUN5_REG) || (RegOffset.Data == SMB_CMD_CFG_1_PCU_FUN5_REG) || (RegOffset.Data == SMB_CMD_CFG_2_PCU_FUN5_REG) \
            ||(RegOffset.Data == BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG) ||(RegOffset.Data == BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_REG) ||(RegOffset.Data == BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG) \
            ||(RegOffset.Data == SYSTEMSEMAPHORE0_UBOX_MISC_REG) ||(RegOffset.Data == SYSTEMSEMAPHORE1_UBOX_MISC_REG) )) {
        getPrintFControl(host);
        rcPrintf ((host, "*%d, 1, 0x%x, 0, %d, ", SocId, RegAddr, Size));

        //
        // Determine register size
        //
        switch (Size) {
        case sizeof (UINT32):
          rcPrintf ((host, "0x%08x\n", (UINT32) Data));
          break;

        case sizeof (UINT16):
          rcPrintf ((host, "0x%04x\n", (UINT16) Data));
          break;

        case sizeof (UINT8):
          rcPrintf ((host, "0x%02x\n", (UINT8) Data));
          break;

        default:
          rcPrintf ((host, "\nInvalid Register Size in RegOffset = 0x%X\n", RegOffset.Data));
          RC_ASSERT(FALSE, ERR_INVALID_REG_ACCESS, RC_ASSERT_MINOR_CODE_5);
        }
        releasePrintFControl(host);
      }
    }
  } else {
    if (checkMsgLevel(host, SDBG_TRACE)) {
      if (!((RegOffset.Data  == SR_POST_CODE_CSR)||(RegOffset.Data  == SR_BIOS_SERIAL_DEBUG_CSR)||(RegOffset.Data  == SR_PRINTF_SYNC_CSR)\
            ||(RegOffset.Data == SMB_CMD_CFG_0_PCU_FUN5_REG) || (RegOffset.Data == SMB_CMD_CFG_1_PCU_FUN5_REG) || (RegOffset.Data == SMB_CMD_CFG_2_PCU_FUN5_REG) \
            ||(RegOffset.Data == BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG) ||(RegOffset.Data == BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_REG) ||(RegOffset.Data == BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG) \
            ||(RegOffset.Data == SYSTEMSEMAPHORE0_UBOX_MISC_REG) ||(RegOffset.Data == SYSTEMSEMAPHORE1_UBOX_MISC_REG) )) {
        getPrintFControl(host);
        rcPrintf ((host, "*%d, 1, 0x%x, 1, %d, ", SocId, RegAddr, Size));

        //
        // Determine register size
        //
        switch (Size) {
        case sizeof (UINT32):
          rcPrintf ((host, "0x%08x\n", (UINT32) Data));
          break;

        case sizeof (UINT16):
          rcPrintf ((host, "0x%04x\n", (UINT16) Data));
          break;

        case sizeof (UINT8):
          rcPrintf ((host, "0x%02x\n", (UINT8) Data));
          break;

        default:
          rcPrintf ((host, "\nInvalid Register Size in RegOffset = 0x%X\n", RegOffset.Data));
          RC_ASSERT(FALSE, ERR_INVALID_REG_ACCESS, RC_ASSERT_MINOR_CODE_6);
        }
        releasePrintFControl(host);
      }
    }
  }
  //
  // if SDBG_TRACE
  //
}
#endif //SERIAL_DBG_MSG
#endif //MRC_TRACE




#ifndef MINIBIOS_BUILD
/**
  Perform MMIO read

  @param[in] AccessWidth          Access Width
  @param[in] AlignedAddress       An address to be read out
  @param[in] Buffer               A pointer of buffer contains the data to be read out

  @retval RETURN_SUCCESS          The function completed successfully.
**/
RETURN_STATUS
RegAlignedRead (
  IN UINT32                   AccessWidth,
  IN UINTN                    AlignedAddress,
  OUT VOID                    *Buffer
  )
{
  switch (AccessWidth)
  {
  case  UsraWidth8:
    *((UINT8*)Buffer) = MmioRead8 (AlignedAddress);
    break;
  case  UsraWidth16:
    *((UINT16*)Buffer) = MmioRead16 (AlignedAddress);
    break;
  case  UsraWidth32:
    *((UINT32*)Buffer) = MmioRead32 (AlignedAddress);
    break;
  default:
    *((UINT64*)Buffer) = MmioRead64 (AlignedAddress);
    break;
  }

  return RETURN_SUCCESS;
};

/**
  Perform MMIO write

  @param[in] AccessWidth          Access Width
  @param[in] AlignedAddress       An address to be written
  @param[in] Buffer               A pointer of buffer contains the data to be written

  @retval RETURN_SUCCESS          The function completed successfully.
**/
RETURN_STATUS
RegAlignedWrite (
  IN UINT32                   AccessWidth,
  IN UINTN                    AlignedAddress,
  OUT VOID                    *Buffer
  )
{
  switch (AccessWidth)
  {
  case  UsraWidth8:
    MmioWrite8 (AlignedAddress,*((UINT8*)Buffer));
    break;
  case  UsraWidth16:
    MmioWrite16 (AlignedAddress,*((UINT16*)Buffer));
    break;
  case  UsraWidth32:
    MmioWrite32 (AlignedAddress,*((UINT32*)Buffer));
    break;
  default:
    MmioWrite64 (AlignedAddress, *((UINT64*)Buffer));
    break;
  }
  return RETURN_SUCCESS;
}
#endif

/**
  Perform register read.

  @param[in]  Address              A pointer of the address of the USRA Address Structure to be read out.
  @param[in]  AlignedAddress       An address to be read out.
  @param[out] Buffer               A pointer of buffer contains the data to be read out.

  @retval RETURN_SUCCESS           Register access is issued.
**/
RETURN_STATUS
EFIAPI
UsraRegisterFilterRead (
  IN  USRA_ADDRESS             *Address,
  IN  UINTN                    AlignedAddress,
  OUT VOID                    *Buffer
  )
{
  PSYSHOST         Host;
  CSR_OFFSET       RegOffset;
  RegOffset.Data = Address->dwRawData[0];

  Host = (PSYSHOST) Address->Attribute.HostPtr;

#ifdef DEBUG_PERFORMANCE_STATS
  CpuCsrAccessCount (Host);
#endif //DEBUG_PERFORMANCE_STATS

  RegAlignedRead((UINT32)Address->Attribute.AccessWidth, AlignedAddress, Buffer);

#ifdef MRC_TRACE
#ifdef SERIAL_DBG_MSG

  CpuCsrAccessTraceCapEx(Host, (UINT8)Address->Csr.SocketId, (UINT8 *)AlignedAddress, RegOffset, (UINT32)RegOffset.Bits.offset, *(UINT32*) Buffer, (UINT8)RegOffset.Bits.size , READ_ACCESS);

#endif //SERIAL_DBG_MSG
#endif //MRC_TRACE

  return RETURN_SUCCESS;
}

/**
  Perform register write

  @param[in]  Address              A pointer of the address of the USRA Address Structure to be written.
  @param[in]  AlignedAddress       An address to be written.
  @param[in]  Buffer               A pointer of buffer contains the data to be written.

  @retval RETURN_SUCCESS           Register access is issued.
**/
RETURN_STATUS
EFIAPI
UsraRegisterFilterWrite (
  IN  USRA_ADDRESS             *Address,
  IN  UINTN                    AlignedAddress,
  IN  VOID                     *Buffer
  )
{
  PSYSHOST         Host;
  CSR_OFFSET       RegOffset;

  RegOffset.Data = Address->dwRawData[0];

  Host = (PSYSHOST) Address->Attribute.HostPtr;

#ifdef DEBUG_PERFORMANCE_STATS
  CpuCsrAccessCount (Host);
#endif //DEBUG_PERFORMANCE_STATS

  RegAlignedWrite((UINT32)Address->Attribute.AccessWidth, AlignedAddress, Buffer);

#ifdef MRC_TRACE
#ifdef SERIAL_DBG_MSG

    CpuCsrAccessTraceCapEx(Host, (UINT8)Address->Csr.SocketId, (UINT8 *)AlignedAddress, RegOffset, (UINT32)RegOffset.Bits.offset, *(UINT32*) Buffer, (UINT8)RegOffset.Bits.size, WRITE_ACCESS);

#endif
#endif


  return RETURN_SUCCESS;
}


/**
  Perform register modify

  @param[in]  Address              A pointer of the address of the USRA Address Structure to be modified.
  @param[in]  AlignedAddress       An address to be modified.
  @param[in]  AndBuffer            A pointer of buffer for the value used for AND operation.
                                   A NULL pointer means no AND operation. RegisterModify() equivalents to RegisterOr().
  @param[in]  OrBuffer             A pointer of buffer for the value used for OR operation.
                                   A NULL pointer means no OR operation. RegisterModify() equivalents to RegisterAnd().

  @retval RETURN_UNSUPPORTED       Register access is not issued.
**/
RETURN_STATUS
EFIAPI
UsraRegisterFilterModify (
  IN  USRA_ADDRESS             *Address,
  IN  UINTN                    AlignedAddress,
  IN  VOID                     *AndBuffer,
  IN  VOID                     *OrBuffer
  )
{
  return RETURN_UNSUPPORTED;
}

