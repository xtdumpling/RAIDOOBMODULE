//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    UBA INIT PEIM.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_INIT_PEI_H_
#define _UBA_INIT_PEI_H_

#include <PiPei.h>
#include <Uefi/UefiSpec.h>
#include <Ppi/UbaCfgDb.h>
#include <Ppi/UbaMakerPpi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>

#include <Guid/PlatformInfo.h> 
#include <Platform.h>
#include <BoardTypes.h>

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeNeonCityEPRPPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeNeonCityEPRPPpiGuid,
    NULL
  };

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeNeonCityEPECBPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeNeonCityEPECBPpiGuid,
    NULL
  };

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeOpalCitySTHIPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeOpalCitySTHIPpiGuid,
    NULL
  };

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypePurleyLBGEPDVPPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypePurleyLBGEPDVPPpiGuid,
    NULL
  };

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeWolfPassPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeWolfPassPpiGuid,
    NULL
  };

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeBuchananPassPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeBuchananPassPpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeCrescentCityPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeCrescentCityPpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeHedtEVPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeHedtEVPpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeHedtCRBPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeHedtCRBPpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeLightningRidgeEXRPPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeLightningRidgeEXRPPpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeLightningRidgeEXECB1Ppi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeLightningRidgeEXECB1PpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeLightningRidgeEXECB2Ppi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeLightningRidgeEXECB2PpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeLightningRidgeEXECB3Ppi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeLightningRidgeEXECB3PpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeLightningRidgeEXECB4Ppi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeLightningRidgeEXECB4PpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeLightningRidgeEX8S1NPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeLightningRidgeEX8S1NPpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeLightningRidgeEX8S2NPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeLightningRidgeEX8S2NPpiGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeKyanitePpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiPlatformTypeKyanitePpiGuid,
  NULL
};

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeNeonCityFPGAPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeNeonCityFPGAPpiGuid,
    NULL
  };

static EFI_PEI_PPI_DESCRIPTOR       mPlatformTypeOpalCityFPGAPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiPlatformTypeOpalCityFPGAPpiGuid,
    NULL
  };

#endif // _UBA_INIT_PEI_H_
