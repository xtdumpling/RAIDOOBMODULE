  /** @file
  Header file for AMT Status Code support at PEI phase

@copyright
 Copyright (c) 2010 - 2016 Intel Corporation. All rights reserved
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
#ifndef _AMT_STATUS_CODE_PPI_H_
#define _AMT_STATUS_CODE_PPI_H_

///
/// Intel AMT PEI Status Code PPI GUID
/// This driver produces interface to let PEI Status Code generic driver report status to
/// Intel AMT, so that Intel AMT PET message can be sent out in PEI phase.
///
extern EFI_GUID gAmtStatusCodePpiGuid;

///
/// Revision
///
#define AMT_STATUS_CODE_PPI_REVISION_1  1

typedef struct _AMT_STATUS_CODE_PPI  AMT_STATUS_CODE_PPI;

/**
  Provides an interface that a software module can call to report an ASF PEI status code.
  The extension to report status code to Intel AMT, so that Intel AMT PET message will
  be sent out in PEI.

  @param[in] This                 This interface.
  @param[in] Type                 Indicates the type of status code being reported.
  @param[in] Value                Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is
                                  used to classify the entity as well as an operation.
  @param[in] Instance             The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in] CallerId             This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different
                                  rules to different callers.
  @param[in] Data                 This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_DEVICE_ERROR        The function should not be completed due to a device error.
**/
typedef
EFI_STATUS
(EFIAPI *AMT_STATUS_CODE_PPI_REPORT_STATUS_CODE) (
  IN  AMT_STATUS_CODE_PPI         *This,
  IN  EFI_STATUS_CODE_TYPE        Type,
  IN  EFI_STATUS_CODE_VALUE       Value,
  IN  UINT32                      Instance,
  IN  EFI_GUID                    *CallerId OPTIONAL,
  IN  EFI_STATUS_CODE_DATA        *Data OPTIONAL
  );

///
/// Intel AMT PEI Status Code PPI
/// The PEI platform status code driver should be responsible to use this interface to report
/// status code to Intel AMT. For example, memory init, memory init done, chassis intrusion.
/// So that Intel AMT driver will send corresponding PET message out.
///
struct _AMT_STATUS_CODE_PPI {
  ///
  /// The extension to report status code to Intel AMT, so that Intel AMT PET message will
  /// be sent out in PEI.
  ///
  AMT_STATUS_CODE_PPI_REPORT_STATUS_CODE  ReportStatusCode;
};

/// 1) If AMT driver is not ready yet, StatusCode driver can save information

#endif
