/** @file

@copyright
 Copyright (c) 2009 - 2014 Intel Corporation. All rights reserved
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

#ifndef PCH_HDA_VERBTABLES_H_
#define PCH_HDA_VERBTABLES_H_

#include <Ppi/PchHdaVerbTable.h>


enum HDAUDIO_CODEC_SELECT {
  PchHdaCodecPlatformOnboard = 0,
  PchHdaCodecExternalKit     = 1
};

extern HDAUDIO_VERB_TABLE HdaVerbTableAlc286S;
extern HDAUDIO_VERB_TABLE HdaVerbTableAlc298;
extern HDAUDIO_VERB_TABLE HdaVerbTableAlc888S;
extern HDAUDIO_VERB_TABLE HdaVerbTableAlc892;
// APTIOV_SERVER_OVERRIDE_RC_START : Added Oem verb table data
extern HDAUDIO_VERB_TABLE OnBoardOemVertable;
extern HDAUDIO_VERB_TABLE ExtKitOemVertable;
// APTIOV_SERVER_OVERRIDE_RC_END : Added Oem verb table data

#endif // PCH_HDA_VERBTABLES_H_
