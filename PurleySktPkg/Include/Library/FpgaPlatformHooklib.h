/** @file
  @todo ADD DESCRIPTION

@copyright
 Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
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

/**
    
    Get Blue Bitstream index according to platform default policy  

    @param socket                          socket number
    @param FpgaBitStreamGuidIndex   return Ptr
    
    @retval EFI_SUCCESS       Operation completed successfully.
    @retval Otherwise            Operation failed.

**/
EFI_STATUS
EFIAPI
GetBBSIndexFromPlatformPolicy (
  IN   UINT8             Socket,
  OUT UINT8             *DefaultFpgaBitStreamGuidIndex
  );

/**
    
    Get Blue Bitstream index according to platform HSSI card design  

    @param socket                          socket number
    @param FpgaBitStreamGuidIndex          return BBS index
    @param FpgaHssiCardID                  return HSSI card ID
    
    @retval EFI_SUCCESS                    Operation completed successfully.
    @retval Otherwise                      Operation failed.

**/
EFI_STATUS
EFIAPI
GetBBSIndexFromHssiCardId (
  IN   UINT8             Socket,
  OUT UINT8             *FpgaBitStreamGuidIndex,
  OUT UINT8             *FpgaHssiCardID
  );

/**
    
    Enables/Disables Clock and Power for FPGA  

    @param socket               socket number
    
    @retval EFI_SUCCESS       Operation completed successfully.
    @retval Otherwise            Operation failed.

**/
EFI_STATUS
EFIAPI
FpgaPoweroff(
  IN  UINT8  Socket
);


