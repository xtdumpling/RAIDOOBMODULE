/** @file
  This file contains routines for eSPI

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <PchAccess.h>
#include <Library/PchEspiLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>

/**
  eSPI helper function to clear slave configuration register status

  @retval EFI_SUCCESS Write to private config space succeed
  @retval others      Read / Write failed
**/
STATIC
EFI_STATUS
EspiClearScrs (
  VOID
  )
{
  return PchPcrAndThenOr32 (
           PID_ESPISPI,
           R_PCH_PCR_ESPI_SLV_CFG_REG_CTL,
           (UINT32) ~0,
           B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRS
           );
}

/**
  eSPI helper function to poll slave configuration register enable for 0
  and to check for slave configuration register status

  @retval EFI_SUCCESS       Enable bit is zero and no error in status bits
  @retval EFI_DEVICE_ERROR  Error in SCRS
  @retval others            Read / Write to private config space failed
**/
STATIC
EFI_STATUS
EspiPollScreAndCheckScrs (
  VOID
  )
{
  EFI_STATUS Status;
  UINT32     Data32;
  UINT32     ScrStat;

  do {
    Status = PchPcrRead32 (
               PID_ESPISPI,
               R_PCH_PCR_ESPI_SLV_CFG_REG_CTL,
               &Data32
               );
    ASSERT_EFI_ERROR (Status);
  } while ((Data32 & B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRE) != 0);

  ScrStat = (Data32 & B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRS) >> N_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRS;
  if (ScrStat != V_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRS_NOERR) {
    DEBUG ((DEBUG_ERROR, "eSPI slave config register status (error) is %x \n", ScrStat));
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

typedef enum {
  EspiSlaveOperationConfigRead,
  EspiSlaveOperationConfigWrite,
  EspiSlaveOperationStatusRead,
  EspiSlaveOperationInBandReset
} ESPI_SLAVE_OPERATION;

/**
  Helper library to do all the operations regards to eSPI slave

  @param[in]      SlaveId         eSPI Slave ID
  @param[in]      SlaveAddress    Slave address to be put in R_PCH_PCR_ESPI_SLV_CFG_REG_CTL[11:0]
  @param[in]      SlaveOperation  Based on ESPI_SLAVE_OPERATION
  @param[in,out]  Data

  @retval EFI_SUCCESS           Operation succeed
  @retval EFI_INVALID_PARAMETER Slave ID is not supported or SlaveId 1 is used in PchLp
  @retval EFI_INVALID_PARAMETER Slave configuration register address exceed maximum allowed
  @retval EFI_INVALID_PARAMETER Slave configuration register address is not DWord aligned
  @retval EFI_ACCESS_DENIED     eSPI Slave write to address range 0 to 0x7FF has been locked
  @retval EFI_DEVICE_ERROR      Error in SCRS during polling stage of operation
**/
STATIC
EFI_STATUS
EspiSlaveOperationHelper (
  IN     UINT32               SlaveId,
  IN     UINT32               SlaveAddress,
  IN     ESPI_SLAVE_OPERATION SlaveOperation,
  IN OUT UINT32               *Data
  )
{
  EFI_STATUS  Status;
  UINT32      Data32;

  //
  // Check the SlaveId is 0 or 1
  //
  if (SlaveId >= PCH_ESPI_MAX_SLAVE_ID) {
    DEBUG ((DEBUG_ERROR, "eSPI Slave ID of %d or more is not accepted \n", PCH_ESPI_MAX_SLAVE_ID));
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check if SlaveId 1 is used, it is a PchH
  //
  if ((SlaveId == 1) && (GetPchSeries () != PchH)) {
    DEBUG ((DEBUG_ERROR, "eSPI Slave ID of 1 is only available on PchH \n"));
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check the address is not more then 0xFFF
  //
  if (SlaveAddress > B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRA) {
    DEBUG ((DEBUG_ERROR, "eSPI Slave address must be less than 0x%x \n", (B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRA + 1)));
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check the address is DWord aligned
  //
  if ((SlaveAddress & 0x3) != 0) {
    DEBUG ((DEBUG_ERROR, "eSPI Slave address must be DWord aligned \n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if write is allowed
  //
  if ((SlaveOperation == EspiSlaveOperationConfigWrite) && 
      (SlaveAddress <= 0x7FF)) {
    Status = PchPcrRead32 (
               PID_ESPISPI,
               (UINT16) (R_PCH_PCR_ESPI_LNKERR_SLV0 + (SlaveId * S_PCH_PCR_ESPI_LNKERR_SLV0)),
               &Data32
               );
    ASSERT_EFI_ERROR (Status);

    //
    // If the SLCRR is not set in corresponding slave, we will check the lock bit
    //
    if ((Data32 & B_PCH_PCR_ESPI_LNKERR_SLV0_SLCRR) == 0) {
      Status = PchPcrRead32 (
                 PID_ESPISPI,
                 (UINT16) R_PCH_PCR_ESPI_SLV_CFG_REG_CTL,
                 &Data32
                 );
      ASSERT_EFI_ERROR (Status);

      if ((Data32 & B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SBLCL) != 0) {
        DEBUG ((DEBUG_ERROR, "eSPI Slave write to address range 0 to 0x7FF has been locked \n"));
        return EFI_ACCESS_DENIED;
      }
    }
  }

  //
  // Input check done, now go through all the processes
  //
  Status = EspiClearScrs ();
  ASSERT_EFI_ERROR (Status);

  if (SlaveOperation == EspiSlaveOperationConfigWrite) {
    Status = PchPcrWrite32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_SLV_CFG_REG_DATA,
               *Data
               );
    ASSERT_EFI_ERROR (Status);
  }

  Status = PchPcrAndThenOr32 (
             PID_ESPISPI,
             (UINT16) R_PCH_PCR_ESPI_SLV_CFG_REG_CTL,
             (UINT32) ~(B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SID | B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRT | B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRA),
             (B_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRE | 
                (SlaveId << N_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SID) | 
                (((UINT32) SlaveOperation) << N_PCH_PCR_ESPI_SLV_CFG_REG_CTL_SCRT) |
                SlaveAddress
                )
             );
  ASSERT_EFI_ERROR (Status);

  Status = EspiPollScreAndCheckScrs ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((SlaveOperation == EspiSlaveOperationConfigRead) || (SlaveOperation == EspiSlaveOperationStatusRead)) {
    Status = PchPcrRead32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_SLV_CFG_REG_DATA,
               &Data32
               );
    if (SlaveOperation == EspiSlaveOperationStatusRead) {
      *Data = Data32 & 0xFFFF;
    } else {
      *Data = Data32;
    }
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

/**
  Get configuration from eSPI slave

  @param[in]  SlaveId       eSPI slave ID
  @param[in]  SlaveAddress  Slave Configuration Register Address
  @param[out] OutData       Configuration data read

  @retval EFI_SUCCESS           Operation succeed
  @retval EFI_INVALID_PARAMETER Slave ID is not supported
  @retval EFI_INVALID_PARAMETER Slave ID is not supported or SlaveId 1 is used in PchLp
  @retval EFI_INVALID_PARAMETER Slave configuration register address exceed maximum allowed
  @retval EFI_INVALID_PARAMETER Slave configuration register address is not DWord aligned
  @retval EFI_DEVICE_ERROR      Error in SCRS during polling stage of operation
**/
EFI_STATUS
PchEspiSlaveGetConfig (
  IN  UINT32 SlaveId,
  IN  UINT32 SlaveAddress,
  OUT UINT32 *OutData
  )
{
  //
  // 1. Clear status from previous transaction by writing 111b to status in SCRS, PCR[eSPI] + 4000h [30:28]
  // 2. Program SLV_CFG_REG_CTL with the right value (Bit[31]=01, Bit [20:19]=<SlvID>, Bit [17:16] = 00b, Bit[11:0] = <addr_xxx>. 
  // 3. Poll the SCRE (PCR[eSPI] +4000h [31]) to be set back to 0
  // 4. Check the transaction status in SCRS (bits [30:28])
  // 5. Read SLV_CFG_REG_DATA.
  //
  return EspiSlaveOperationHelper (SlaveId, SlaveAddress, EspiSlaveOperationConfigRead, OutData);
}

/**
  Set eSPI slave configuration

  @param[in]  SlaveId       eSPI slave ID
  @param[in]  SlaveAddress  Slave Configuration Register Address
  @param[in]  InData        Configuration data to write

  @retval EFI_SUCCESS           Operation succeed
  @retval EFI_INVALID_PARAMETER Slave ID is not supported or SlaveId 1 is used in PchLp
  @retval EFI_INVALID_PARAMETER Slave configuration register address exceed maximum allowed
  @retval EFI_INVALID_PARAMETER Slave configuration register address is not DWord aligned
  @retval EFI_ACCESS_DENIED     eSPI Slave write to address range 0 to 0x7FF has been locked
  @retval EFI_DEVICE_ERROR      Error in SCRS during polling stage of operation
**/
EFI_STATUS
PchEspiSlaveSetConfig (
  IN  UINT32 SlaveId,
  IN  UINT32 SlaveAddress,
  IN  UINT32 InData
  )
{
  //
  // 1. Clear status from previous transaction by writing 111b to status in SCRS, PCR[eSPI] + 4000h [30:28]
  // 2. Program SLV_CFG_REG_DATA with the write value.
  // 3. Program SLV_CFG_REG_CTL with the right value (Bit[31]=01, Bit [20:19]=<SlvID>, Bit [17:16] = 01b, Bit[11:0] = <addr_xxx>. 
  // 4. Poll the SCRE (PCR[eSPI] +4000h [31]) to be set back to 0
  // 5. Check the transaction status in SCRS (bits [30:28])
  //
  return EspiSlaveOperationHelper (SlaveId, SlaveAddress, EspiSlaveOperationConfigWrite, &InData);
}

/**
  Get status from eSPI slave

  @param[in]  SlaveId       eSPI slave ID
  @param[out] OutData       Configuration data read

  @retval EFI_SUCCESS           Operation succeed
  @retval EFI_INVALID_PARAMETER Slave ID is not supported or SlaveId 1 is used in PchLp
  @retval EFI_DEVICE_ERROR      Error in SCRS during polling stage of operation
**/
EFI_STATUS
PchEspiSlaveGetStatus (
  IN  UINT32 SlaveId,
  OUT UINT16 *OutData
  )
{
  EFI_STATUS  Status;
  UINT32      TempOutData;

  TempOutData = 0;

  //
  // 1. Clear status from previous transaction by writing 111b to status in SCRS, PCR[eSPI] + 4000h [30:28]
  // 2. Program SLV_CFG_REG_CTL with the right value (Bit[31]=01, Bit [20:19]=<SlvID>, Bit [17:16] = 10b, Bit[11:0] = <addr_xxx>. 
  // 3. Poll the SCRE (PCR[eSPI] +4000h [31]) to be set back to 0
  // 4. Check the transaction status in SCRS (bits [30:28])
  // 5. Read SLV_CFG_REG_DATA [15:0].
  //
  Status = EspiSlaveOperationHelper (SlaveId, 0, EspiSlaveOperationStatusRead, &TempOutData);
  *OutData = (UINT16) TempOutData;

  return Status;
}

/**
  eSPI slave in-band reset

  @param[in]  SlaveId       eSPI slave ID

  @retval EFI_SUCCESS           Operation succeed
  @retval EFI_INVALID_PARAMETER Slave ID is not supported or SlaveId 1 is used in PchLp
  @retval EFI_DEVICE_ERROR      Error in SCRS during polling stage of operation
**/
EFI_STATUS
PchEspiSlaveInBandReset (
  IN  UINT32 SlaveId
  )
{
  //
  // 1. Clear status from previous transaction by writing 111b to status in SCRS, PCR[eSPI] + 4000h [30:28]
  // 2. Program SLV_CFG_REG_CTL with the right value (Bit[31]=01, Bit [20:19]=<SlvID>, Bit [17:16] = 11b). 
  // 3. Poll the SCRE (PCR[eSPI] +4000h [31]) to be set back to 0
  // 4. Check the transaction status in SCRS (bits [30:28])
  //
  return EspiSlaveOperationHelper (SlaveId, 0, EspiSlaveOperationInBandReset, NULL);
}
