/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PciIovPlatformPolicy.c
    
  Implementation of EFI PCI IOV PLATFORM Protocol.
    
Revision History:

**/


#include <PiDxe.h>
#include "PciPlatform.h"
#include <Guid/SetupVariable.h>
#include <Library/SetupLib.h>

#ifdef EFI_PCI_IOV_SUPPORT

/**

    The GetSystemLowestPageSize() function retrieves the system lowest page size.
    
    @param This                 - Pointer to the EFI_PCI_IOV_PLATFORM_PROTOCOL instance.
    @param SystemLowestPageSize - The system lowest page size. (This system supports a
                                  page size of 2^(n+12) if bit n is set.)
    
    @retval EFI_SUCCESS           - The function completed successfully.
    @retval EFI_INVALID_PARAMETER - SystemLowestPageSize is NULL.
    
**/
EFI_STATUS
EFIAPI
GetSystemLowestPageSize (
  IN  EFI_PCI_IOV_PLATFORM_PROTOCOL           *This,
  OUT UINT32                                  *SystemLowestPageSize
)
{
  EFI_STATUS                            Status;
  UINT8                                 SystemPageSize;  

  Status = GetOptionData (&gEfiSetupVariableGuid,  OFFSET_OF(SYSTEM_CONFIGURATION, SystemPageSize), &SystemPageSize, sizeof(UINT8));

  ASSERT_EFI_ERROR (Status);
  
  if (SystemLowestPageSize != NULL) {
    //
    // Page size is 4K
    //
    //*SystemLowestPageSize = 1;
    *SystemLowestPageSize = SystemPageSize;
  }
  return EFI_SUCCESS;
}

/**

    The GetIovPlatformPolicy() function retrieves the platform policy regarding PCI IOV.
    
    @param This         - Pointer to the EFI_PCI_IOV_PLATFORM_PROTOCOL instance.
    @param PciIovPolicy - The platform policy for PCI IOV configuration.
    
    @retval EFI_SUCCESS           - The function completed successfully.
    @retval EFI_INVALID_PARAMETER - PciPolicy is NULL.
    
**/
EFI_STATUS
EFIAPI 
GetIovPlatformPolicy (
  IN  EFI_PCI_IOV_PLATFORM_PROTOCOL           *This,
  OUT EFI_PCI_IOV_PLATFORM_POLICY             *PciIovPolicy
)
{
  EFI_STATUS                            Status;
  UINT8                                 PolicyEnable;
  UINT8                                 ARIEnable;
  UINT8                                 SRIOVEnable;
  UINT8                                 MRIOVEnable;

  PolicyEnable = 0;
  
  Status = GetOptionData (&gEfiSetupVariableGuid,  OFFSET_OF(SYSTEM_CONFIGURATION, ARIEnable), &ARIEnable, sizeof(UINT8));
  ASSERT_EFI_ERROR (Status);
  Status = GetOptionData (&gEfiSetupVariableGuid,  OFFSET_OF(SYSTEM_CONFIGURATION, SRIOVEnable), &SRIOVEnable, sizeof(UINT8));
  ASSERT_EFI_ERROR (Status);
  Status = GetOptionData (&gEfiSetupVariableGuid,  OFFSET_OF(SYSTEM_CONFIGURATION, MRIOVEnable), &MRIOVEnable, sizeof(UINT8));
  ASSERT_EFI_ERROR (Status);

  if (ARIEnable == TRUE) {
    PolicyEnable = PolicyEnable | EFI_PCI_IOV_POLICY_ARI;
  }

  if (SRIOVEnable == TRUE) {
    PolicyEnable = PolicyEnable | EFI_PCI_IOV_POLICY_SRIOV;
  }

  if (MRIOVEnable == TRUE) {
    PolicyEnable = PolicyEnable | EFI_PCI_IOV_POLICY_MRIOV;
  }
  
  if (PciIovPolicy != NULL) {
    //*PciIovPolicy = EFI_PCI_IOV_POLICY_ARI | EFI_PCI_IOV_POLICY_SRIOV;
    *PciIovPolicy = PolicyEnable;
  }
  return EFI_SUCCESS;
}

#endif

