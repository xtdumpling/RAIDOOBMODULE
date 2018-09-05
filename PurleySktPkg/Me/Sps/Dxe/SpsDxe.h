/**

Copyright (c) 1996 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file SpsDxe.h

  Definitions for SPS ME-BIOS support in DXE.

**/
#ifndef _SPSDXE_H_
#define _SPSDXE_H_

#include "Protocol/MeSpsPolicyProtocol.h"

//
// Data shared between SPS DXE files.
//
typedef struct
{
  SPS_INFO_HOB         *pSpsHob;
  SPS_POLICY_PROTOCOL  *pSpsPolicy;
} SPS_DXE_CONTEXT;

VOID SpsEndOfDxeCallback(IN EFI_EVENT Event, IN VOID *pContext);
VOID SpsReadyToBootCallback(IN EFI_EVENT Event, IN VOID *pContext);

EFI_STATUS SpsNmSendHostCfgInfo(SPS_DXE_CONTEXT*);

#endif // _SPSDXE_H_

