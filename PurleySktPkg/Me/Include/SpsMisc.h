/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file SpsMisc.h

 SPS definitions shared between PEI and DXE drivers.

**/
#ifndef _SPS_MISC_
#define _SPS_MISC_

#define CMOS_ADDR_PORT          0x70
#define CMOS_DATA_PORT          0x71
#define UPPER_CMOS_ADDR_PORT    0x72
#define UPPER_CMOS_DATA_PORT    0x73
#define CMOS_BAD_REG            0x0E
#ifdef SPS_CMOS_WRITE
#define SPSSETUP_SSP_FC_RECEIVED_REG   0x00
#endif


#endif  //_SPS_MISC_

