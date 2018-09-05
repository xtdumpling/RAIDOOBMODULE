/** @file
  ME Firmware Version Info implementation.

@copyright
 Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _MBP_DATA_H
#define _MBP_DATA_H

#include <MeBiosPayloadData.h>
#include <Ppi/HeciPpi.h>


/**
  This routine returns ME-BIOS Payload information.

  @param[out] MbpPtr              ME-BIOS Payload information.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        Failed to consume MBP
**/
EFI_STATUS
PrepareMeBiosPayload (
  OUT  ME_BIOS_PAYLOAD                *MbpPtr,
  OUT  ME_BIOS_PAYLOAD_SENSITIVE      *MbpSensitivePtr
  );

/**
  Dump MBP_DATA_PROTOCOL

  @param[in] MbpData              Pointer to MBP_DATA_PROTOCOL

**/
VOID
MbpDebugPrint (
  IN UINT8                     MbpRevision,
  IN UINT8                     MbpSensitiveRevision,
  IN ME_BIOS_PAYLOAD           *MbpPtr,
  IN ME_BIOS_PAYLOAD_SENSITIVE *MbpSensitivePtr
  );

#endif
