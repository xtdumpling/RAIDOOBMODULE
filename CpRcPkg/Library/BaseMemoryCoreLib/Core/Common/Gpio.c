/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"

//
// Local Prototypes
//
UINT32 ReadGpio(PSYSHOST host, UINT16 address, UINT32 *data);
UINT32 WriteGpio(PSYSHOST host, UINT16 address, UINT32 data);


UINT32
InitGpio (
         PSYSHOST host
         )
/*++

  Initialize GPIOs

  @param host      - Pointer to the system host (root) structure

  @retval 0 - Success

--*/
{
  //
  // Call platform hooks to configure GPIO bits
  //
  PlatformInitGpio (host);

  return SUCCESS;
}

UINT32
UpdateGpioBit (
              PSYSHOST host,
              UINT16   address,
              UINT8    bitNumber,
              UINT8    bitValue
              )
/*++

  Updates the specified GPIO bit to the defined value

  @param host      - Pointer to the system host (root) structure
  @param address   - GPIO address offset from host->var.common.gpioBase
  @param bitNumber - Zero based bit number of GPIO within address
  @param bitValue  - Value to write to bit

  @retval 0 - Success

--*/
{
  UINT32  bitMask;
  UINT32  gpioData;

  bitMask = 1 << bitNumber;

  ReadGpio (host, address, &gpioData);
  if (bitValue) {
    gpioData |= bitMask;
  } else {
    gpioData &= ~bitMask;
  }

  WriteGpio (host, address, gpioData);

  return SUCCESS;
}

UINT32
ReadGpio (
         PSYSHOST host,
         UINT16   address,
         UINT32   *data
         )
/*++

  Reads the 16-bit GPIO address requested

  @param host    - Pointer to the system host (root) structure
  @param address - GPIO device address
  @param data    - pointer to location for data read

  @retval 0 - Success
  @retval Other - Failure

--*/
{
  //
  // Call the OEM hook to read the GPIO
  //
  switch (PlatformReadGpio (host, address, data)) {
  //
  // No hook available
  //
  case 0:
    *data = InPort16 (host, (UINT16) (host->var.common.gpioBase + address));
    return SUCCESS;

    //
    // GPIO read was successful
    //
  case 1:
    return SUCCESS;

    //
    // GPIO read failed
    //
  default:
    return FAILURE;
  }
}

UINT32
WriteGpio (
          PSYSHOST host,
          UINT16   address,
          UINT32   data
          )
/*++

  Writes the specified GPIO register with the provided data


  @param host    - Pointer to the system host (root) structure
  @param address - GPIO device address
  @param data    - data to write


  @retval 0 - Success
  @retval Other - Failure

--*/
{
  //
  // Call the OEM hook to write the GPIO
  //
  switch (PlatformWriteGpio (host, address, data)) {
  //
  // No hook available
  //
  case 0:
    OutPort16 (host, (UINT16) (host->var.common.gpioBase + address), (UINT16) data);
    return SUCCESS;

    //
    // GPIO write was successful
    //
  case 1:
    return SUCCESS;

    //
    // GPIO write failed
    //
  default:
    return FAILURE;
  }
}
