/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    IioInitPeiLib.c

Abstract:

    Hooks file for IioInit Module

--*/

//
// Do not include any of this functionality in sim builds
//

#include <Uefi.h>
#include <PiPei.h>
#include <IioPlatformData.h>
#include <IioMisc.h>
#include <OemIioInit.h>
#include "IioInitPei.h"
#include <Library/HobLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <IncludePrivate/Library/PchRcLib.h>
#include <Ppi/PchInit.h>

#include <Guid/IioPolicyHob.h>


/**

    This routine will program TC/VC settings for PCH side 

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
IioPchDmiTcVcMapInit (
  IN  IIO_GLOBALS               *IioGlobalData,
  IN  PCH_DMI_TC_VC_MAP         *PchDmiTcVcMap
 )
{
  EFI_PEI_SERVICES             **PeiServices;
  EFI_STATUS                   Status;
  PCH_INIT_PPI                 *PchInitPpi;
  UINT8                        i;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  
  //
  // Locate PchInitPpi
  //
  Status = (*PeiServices)->LocatePpi (PeiServices, &gPchInitPpiGuid, 0, NULL, &PchInitPpi);
  IIO_ASSERT (IioGlobalData, Status, Status);
  //
  // Get default PchDmiTcVcMap from PCH library
  //
  PchInitPpi->DmiTcVcMapInit(PchDmiTcVcMap);
  PchDmiTcVcMap->DmiVc[DmiVcTypeVc1].Enable = IioGlobalData->IioVar.IioVData.DmiVc[DmiVcTypeVc1];
  PchDmiTcVcMap->DmiVc[DmiVcTypeVcm].Enable = IioGlobalData->IioVar.IioVData.DmiVc[DmiVcTypeVcm];

  IioGlobalData->IioVar.IioVData.DmiVcId[DmiVcTypeVc1] = PchDmiTcVcMap->DmiVc[DmiVcTypeVc1].VcId;
  IioGlobalData->IioVar.IioVData.DmiVcId[DmiVcTypeVcm] = PchDmiTcVcMap->DmiVc[DmiVcTypeVcm].VcId;

  for (i = 0; i < MaxDmiTcType; i++) {

    IioGlobalData->IioVar.IioVData.DmiTc[i] =  DmiVcTypeVc0;
    if (PchDmiTcVcMap->DmiTc[i].Vc == DmiVcTypeVc1){
      if (PchDmiTcVcMap->DmiVc[DmiVcTypeVc1].Enable){
        IioGlobalData->IioVar.IioVData.DmiTc[i] =  DmiVcTypeVc1;
      } else {
        PchDmiTcVcMap->DmiTc[i].Vc = DmiVcTypeVc0;
      }
    }

    if (PchDmiTcVcMap->DmiTc[i].Vc == DmiVcTypeVcm){
        if (PchDmiTcVcMap->DmiVc[DmiVcTypeVcm].Enable){
          IioGlobalData->IioVar.IioVData.DmiTc[i] =  DmiVcTypeVcm;
        } else {
          PchDmiTcVcMap->DmiTc[i].Vc = DmiVcTypeVc0;
        }
    }
  }// for ..
}
/**

    This routine will program TC/VC settings for PCH side 

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
IioDmiTcVcProg (
  IN  IIO_GLOBALS *IioGlobalData
  )
{
  EFI_PEI_SERVICES       **PeiServices;  
  EFI_STATUS             Status;
  PCH_INIT_PPI          *PchInitPpi;
  PCH_DMI_TC_VC_MAP      PchDmiTcVcMap;
  
  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  //
  // Locate PchInitPpi
  //
  Status = (*PeiServices)->LocatePpi (PeiServices, &gPchInitPpiGuid, 0, NULL, &PchInitPpi);
  IIO_ASSERT (IioGlobalData, Status, Status);
  //
  // Initialize PchDmiTcVcMap based in IIO DMI Vc/Tc settings
  //
  ZeroMem (&PchDmiTcVcMap, sizeof(PCH_DMI_TC_VC_MAP));
  IioPchDmiTcVcMapInit(IioGlobalData, &PchDmiTcVcMap);
  Status = PchInitPpi->DmiTcVcProg (&PchDmiTcVcMap);
  IIO_ASSERT (IioGlobalData, Status, Status);
}

/**

    This routine will poll Tc/Vc in order to verify that 
    CPU/PCH negociation is completed.

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
IioDmiTcVcPoll (
  IIO_GLOBALS         *IioGlobalData
  )
{
  EFI_PEI_SERVICES       **PeiServices;  
  EFI_STATUS             Status;
  PCH_INIT_PPI           *PchInitPpi;
  PCH_DMI_TC_VC_MAP      PchDmiTcVcMap;
  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  //
  // Locate PchInitPpi
  //
  Status = (UINT32)(*PeiServices)->LocatePpi (PeiServices, &gPchInitPpiGuid, 0, NULL, &PchInitPpi);
  IIO_ASSERT (IioGlobalData, Status, Status);
  
  ZeroMem (&PchDmiTcVcMap, sizeof(PCH_DMI_TC_VC_MAP));
  IioPchDmiTcVcMapInit(IioGlobalData, &PchDmiTcVcMap);
  Status = PchInitPpi->DmiTcVcPoll (&PchDmiTcVcMap);
  IIO_ASSERT (IioGlobalData, Status, Status);

}

/**

    This routine will prograam  PCH side to be capable to use
    Gen3 speed 

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
IioDmiGen3Prog (
  IN  IIO_GLOBALS *IioGlobalData
  )
{
  EFI_PEI_SERVICES       **PeiServices;
  EFI_STATUS             Status;
  PCH_INIT_PPI           *PchInitPpi;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  //
  // Locate PchInitPpi
  //
  Status = (*PeiServices)->LocatePpi (PeiServices, &gPchInitPpiGuid, 0, NULL, &PchInitPpi);
  IIO_ASSERT (IioGlobalData, Status, Status);

  PchInitPpi->DmiGen3Prog();
}

/**

    This routine will prograam  PCH side to be capable to use
    Gen2 speed 

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
IioDmiGen2Prog (
  IN  IIO_GLOBALS   *IioGlobalData
  )
{
  EFI_PEI_SERVICES       **PeiServices;
  EFI_STATUS             Status;
  PCH_INIT_PPI           *PchInitPpi;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  //
  // Locate PchInitPpi
  //
  Status = (*PeiServices)->LocatePpi (PeiServices, &gPchInitPpiGuid, 0, NULL, &PchInitPpi);
  IIO_ASSERT (IioGlobalData, Status, Status);

  PchInitPpi->DmiGen2Prog();
}

EFI_STATUS
IioPublishHobData(
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
  VOID                                  *HobPtr;

  HobPtr = BuildGuidDataHob (&gIioPolicyHobGuid, IioGlobalData, sizeof (IIO_GLOBALS));
  if(HobPtr == NULL){
    IIO_ASSERT (IioGlobalData, IIO_ERROR, 0);
  }

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "IioHobGlobalData Published!!!\n");

  OemPublishOpaSocketMapHob(IioGlobalData);
  return EFI_SUCCESS;
}


