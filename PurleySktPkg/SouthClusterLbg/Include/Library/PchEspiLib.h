/** @file
  Header file for PchEspiLib.
  All function in this library is available for PEI, DXE, and SMM,

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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

#ifndef _PCH_ESPI_LIB_H_
#define _PCH_ESPI_LIB_H_

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
  );

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
  );

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
  );

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
  );

#endif // _PEI_DXE_SMM_PCH_ESPI_LIB_H_
