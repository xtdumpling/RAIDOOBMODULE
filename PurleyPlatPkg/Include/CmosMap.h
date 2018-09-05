//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//
/**

Copyright (c) 2010 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file CmosMap.h

--*/

#define	CPU_HT_POLICY			0x50
#define	CPU_HT_POLICY_ENABLED   0x01

#define	TPM_POLICY				0x60
#define	TPM_POLICY_ENABLED		0x01

#define CMOS_OC_S3_SCRATCHPAD           0x98
#define CMOS_OC_SEND_BCLK_RAMP_MSG      0x99

//
// PC_RP_VARIABLE_SYNC_START
//
#ifdef PC_HOOK
#define CREATE_RP_VARIABLE_HOB_FLAG     0x68
#endif
//
// PC_RP_VARIABLE_SYNC_END
//
