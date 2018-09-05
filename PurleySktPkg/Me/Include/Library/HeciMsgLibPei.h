/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  2006 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  HeciMsgLibPei.h

@brief
  Header file for Heci Message functionality

**/
#ifndef _PEI_HECI_MESSAGE_LIB_H_
#define _PEI_HECI_MESSAGE_LIB_H_

#include "Ppi/HeciPpi.h"

#define START_WDT_BYTE_COUNT                      0x0F
#define START_WDT_VERSION_NUMBER                  0x10
#define START_WDT_EVENT_SENSOR_TYPE               0x23
#define START_WDT_EVENT_TYPE                      0x6F
#define START_WDT_EVENT_OFFSET                    0x00
#define START_WDT_EVENT_SOURCE_TYPE_OS            0x48
#define START_WDT_EVENT_SOURCE_TYPE_BIOS          0x00
#define START_WDT_EVENT_SOURCE_TYPE               0x00  ///< 0x00 - BIOs, 0x48 - OS
#define START_WDT_EVENT_SEVERITY                  0x10  ///< critical
#define START_WDT_SENSOR_DEVICE                   0xFF  ///< unspecified
#define START_WDT_SENSOR_NUMBER                   0xFF  ///< unspecified
#define START_WDT_ENTITY                          0x00  ///< unspecified
#define START_WDT_ENTITY_INSTANCE                 0x00  ///< unspecified
#define START_WDT_EVENT_DATA_BYTE_0               0x40
#define START_WDT_EVENT_DATA_BYTE_1               0x02  ///< 0x02 BIOS POST WDT Timeout, 0x04 OS WDT timeout
#define START_WDT_EVENT_DATA_BYTE_1_BIOS_TIMEOUT  0x02
#define START_WDT_EVENT_DATA_BYTE_1_OS_TIMEOUT    0x04

#endif
