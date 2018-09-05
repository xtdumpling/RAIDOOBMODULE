//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/**

Copyright (c)  2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PciIovPlatformPolicy.h

  Include file for PciIovPlatformPolicy.c

**/

#ifndef PCI_IOV_PLATFORM_POLICY_H_
#define PCI_IOV_PLATFORM_POLICY_H_

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
;


/**

    The GetPlatformPolicy() function retrieves the platform policy regarding PCI IOV.
    
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
;

#endif
