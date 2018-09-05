/** @file
  PEI I2C Master module

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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

#ifndef _PEI_I2C_MASTER_LIB_H_
#define _PEI_I2C_MASTER_LIB_H_

#include <Ppi/I2cMaster.h>
#include <Library/PchSerialIoLib.h>

/*
  Installs I2cMaster PPIs for each I2c controller.
  
  @param[in] Controller          - SerialIo controller number

  @retval EFI_INVALID_PARAMETER  - wrong Controller number
  @retval EFI_ALREADY_STARTED    - I2cMaster Ppi was already installed on given controller
  @retval EFI_SUCCESS            - I2cMaster Ppi succesfully installed
  @retval any other return value - internal error of InstallPpi function
*/
EFI_STATUS 
InstallI2cMasterPpi ( 
  PCH_SERIAL_IO_CONTROLLER Controller 
  );

/*
  Finds I2cMasterPpi instance for a specified controller

  @param[in] Controller - SerialIo controller number

  @retval NULL          - couldn't locate I2cMaster Ppi for given controller
  @retval not-NULL      - pointer to I2cMaster Ppi
*/
EFI_PEI_I2C_MASTER_PPI* 
LocateI2cMasterPpi (
  PCH_SERIAL_IO_CONTROLLER Controller 
  );

#endif
