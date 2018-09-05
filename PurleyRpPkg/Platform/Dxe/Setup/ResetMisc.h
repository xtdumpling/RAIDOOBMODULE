//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file ResetMisc.h

  Header file for Platform Initialization Driver.

Revision History:

**/

#ifndef _RESET_MISC_H_
#define _RESET_MISC_H_


// These have to match the setup options for ValidationResetType
#define V_RESET_NONE                        0x00
#define V_RESET_PWRGD_RST_SS                0x01
#define V_RESET_HARD_RST_SS                 0x02
#define V_RESET_HARD_RST_FF                 0x03
#define V_RESET_NORMAL_SF_FF_HRDRST         0x04
#define V_RESET_QPI_FREQ_WALK               0x05
#define V_RESET_G3_TEST                     0x06
#define V_RESET_INIT_TEST                   0x07
#define V_RESET_PWRGD_RST_SF                0x08

#endif
