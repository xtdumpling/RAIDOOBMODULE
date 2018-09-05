/**@file

@copyright
 Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
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

#include <Ppi/ReportStatusCodeHandler.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/AmtStatusCode.h>
#include <AmtForcePushPetHob.h>
#include <AmtPetQueueHob.h>

EFI_STATUS
AmtQueuePetMessage (
    IN  EFI_STATUS_CODE_TYPE    Type,
    IN  EFI_STATUS_CODE_VALUE   Value
    )
/*++
Routine Description: 

  This routine puts PET message to MessageQueue, which will be sent later.

Arguments:

  Type        - StatusCode message type.
  Value       - StatusCode message value.

Returns: 
  
  EFI_SUCCESS      - The function completed successfully

--*/
{
    AMT_PET_QUEUE_HOB     *PETQueueHob;
    EFI_STATUS            Status;

    // Create PET queue hob
    Status = PeiServicesCreateHob ( EFI_HOB_TYPE_GUID_EXTENSION,
                                    sizeof(AMT_PET_QUEUE_HOB),
                                    &PETQueueHob );
    if (EFI_ERROR (Status)) {
        return Status;
    }
    PETQueueHob->EfiHobGuidType.Name = gAmtPetQueueHobGuid;
    PETQueueHob->Type = Type;
    PETQueueHob->Value = Value;

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmtStatusCode(
  IN       EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_STATUS_CODE_TYPE        Type,
  IN       EFI_STATUS_CODE_VALUE       Value,
  IN       UINT32                      Instance,
  IN       EFI_GUID                    *CallerId,
  IN       EFI_STATUS_CODE_DATA        *Data
)
/*++
Routine Description: 

  Provides an interface that a software module can call to report an ASF PEI status code.

Arguments:

  PeiServices - PeiServices pointer.

  Type     - Indicates the type of status code being reported.
  
  Value    - Describes the current status of a hardware or software entity.
             This included information about the class and subclass that is 
             used to classify the entity as well as an operation.
  
  Instance - The enumeration of a hardware or software entity within 
             the system. Valid instance numbers start with 1.
  
  CallerId - This optional parameter may be used to identify the caller.
             This parameter allows the status code driver to apply different 
             rules to different callers.
  
  Data     - This optional parameter may be used to pass additional data.

Returns: 
  
  None

--*/
{
    AMT_STATUS_CODE_PPI             *AmtStatusCode;
    EFI_STATUS                      Status;

    Status = PeiServicesLocatePpi ( &gAmtStatusCodePpiGuid,
                                    0,
                                    NULL,
                                    &AmtStatusCode );
    if ( EFI_ERROR (Status) ) {
        if (((Type & EFI_STATUS_CODE_TYPE_MASK) != EFI_PROGRESS_CODE) &&
            ((Type & EFI_STATUS_CODE_TYPE_MASK) != EFI_ERROR_CODE)) {
            return EFI_SUCCESS;
        }

        // Register to Hob

        // Create PET queue hob
        AmtQueuePetMessage (Type, Value);

        return EFI_SUCCESS;
    }

    AmtStatusCode->ReportStatusCode (
                     AmtStatusCode,
                     Type,
                     Value,
                     Instance,
                     CallerId,
                     Data
                     );

    return EFI_SUCCESS;
}

EFI_STATUS
AmtStatusInit (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
/*++
Routine Description: 

  Init routine for PEI ASF StatusCode.

Arguments:

  FfsHeader   - FfsHeader pointer.
  PeiServices - PeiServices pointer.

Returns: 
  
  EFI_SUCCESS      - The function completed successfully

--*/
{
  EFI_STATUS                  Status;
  EFI_PEI_RSC_HANDLER_PPI     *RscHandlerPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **) &RscHandlerPpi
             );
  ASSERT_EFI_ERROR (Status);

  Status = RscHandlerPpi->Register ((EFI_PEI_RSC_HANDLER_CALLBACK) AmtStatusCode);                     
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
