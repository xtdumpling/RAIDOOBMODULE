/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c) 2016, Intel Corporation.

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


Module Name:

  XlateFunctions.h

Abstract:

  Header containing declaration of functions handling DPA/SPA conversions.

--*/

#ifndef _XLATEFUNCTIONS_H_
#define _XLATEFUNCTIONS_H_

#include "CrystalRidge.h"

typedef struct _DPA_ADDRESS_
{
	UINT64 Offset;
	UINT8  IntDimmNum;
	UINT8  Ch;
	UINT8  Imc;
	UINT8  Skt;
} DPA_ADDRESS;

/**

Routine Description: XlateSpaToDpa - This function converts
given Spa to Dpa in the case of the Dimm involved in all Channel
ways and up to 8 Imc Ways. Converts only PMEM and CTRL regions.

  @param[in] *NvmDimmPtr - Pointer to Dimm structure
  @param[in] SpaBaseAddr - SAD Base Address
  @param[in] Spa         - Spa for which we need the DPA
  @param[out] *Dpa       - Pointer to the Dpa that this function
                           returns.

  @return EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
XlateSpaToDpaOffset (
  IN  AEP_DIMM  *NvmDimmPtr,
  IN  UINT64    SpaBaseAddr,
  IN  UINT64    Spa,
  OUT UINT64    *Dpa
  );

/**

Routine Description: XlateDpaToSpa - This function converts given Dpa
to Spa in the case of the Dimm involved in all Channel ways and
up to 8 Imc Ways. Converts only PMEM and CTRL regions.

  @param[in] *NvmDimmPtr - Pointer to Dimm structure
  @param[in] SpaBaseAddr - Start Address of the Spa for Dpa Translation
  @param[in] Dpa         - Dpa offset for which we need the SPA
  @param[out] *Spa       - Pointer to the Spa.

  @return EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
XlateDpaOffsetToSpa (
  IN  AEP_DIMM  *NvmDimmPtr,
  IN  UINT64    SpaBaseAddr,
  IN  UINT64    Dpa,
  OUT UINT64    *Spa
  );

#endif // _XLATEFUNCTIONS_H_
