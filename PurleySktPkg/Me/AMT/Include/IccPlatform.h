/**@file

@copyright
 Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef ICC_PLATFORM_H
#define ICC_PLATFORM_H


#define CLOCK0 0
#define CLOCK1 1

#define PROFILE_SELECTED_BY_BIOS  0
#define PROFILE_SELECTED_BY_ME    1

#define OPTION_DISABLED           0
#define OPTION_ENABLED            1

#define OPTION_USE_OEM            7
#define OPTION_OVERRIDE           8


#define ICC_MBP_DATA_NAME        L"IccMbpData"


#ifndef MAX_UI_CLOCKS
// MAX_UI_CLOCKS has to be defined in platform pkg
#endif // MAX_UI_CLOCKS

#define CLOCK0 0
#define CLOCK1 1

#define DONT_DISPLAY              0
#define DISPLAY                   1

#define OPTION_LOCK_DEFAULT       0
#define OPTION_LOCK_ALL           1
#define OPTION_UNLOCK_ALL         2

#define ICC_SSC_NONE              0
#define ICC_SSC_UP                1
#define ICC_SSC_CENTER            2
#define ICC_SSC_DOWN              4


#endif // ICC_PLATFORM_H




