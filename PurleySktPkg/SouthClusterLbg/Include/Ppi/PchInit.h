/** @file
  This file defines the PCH Init PPI

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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

#ifndef _PCH_INIT_H_
#define _PCH_INIT_H_

#include <IncludePrivate/Library/PchRcLib.h>
//
// Extern the GUID for PPI users.
//
extern EFI_GUID               gPchInitPpiGuid;


//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_INIT_PPI  PCH_INIT_PPI;

/**
  Get PCH default TC VC Mapping settings. This function returns the default PCH setting
  System Agent can update the settings according to polices.

  @param[in, out] PchDmiTcVcMap         Buffer for PCH_DMI_TC_VC_MAP instance.

**/
typedef
VOID
(EFIAPI *PCH_DMI_TCVC_MAP_INITIALIZE) (
    IN OUT PCH_DMI_TC_VC_MAP *PchDmiTcVcMap
);

/**
  The function performing TC/VC mapping program, and poll all PCH Virtual Channel
  until negotiation completion

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_DMI_TCVC_PROG) (
  IN  PCH_DMI_TC_VC_MAP        *PchDmiTcVcMap
  );
/**
  The function performing TC/VC mapping program, and poll all PCH Virtual Channel
  until negotiation completion

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_DMI_TCVC_POLL) (
  IN  PCH_DMI_TC_VC_MAP        *PchDmiTcVcMap
  );
/**
  The function set the Target Link Speed in PCH to DMI GEN 2.

**/
typedef
VOID
(EFIAPI *PCH_DMI_GEN2_PROG) (
  VOID
  );

/**
  The function set the Target Link Speed in PCH to DMI GEN 3.

  @retval None
**/
typedef
VOID
(EFIAPI *PCH_DMI_GEN3_PROG) (
  VOID
  );


/**
  The function is used while doing CPU Only Reset, where PCH may be required
  to initialize strap data before soft reset.

  @param[in] Operation            Get/Set Cpu Strap Set Data
  @param[in] CpuStrapSet          Cpu Strap Set Data

  @retval EFI_SUCCESS             The function completed successfully.
  @exception EFI_UNSUPPORTED      The function is not supported.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_CPU_STRAP_SET) (
  IN      CPU_STRAP_OPERATION         Operation,
  IN OUT  UINT16                      *CpuStrapSet
  );

/**
  Set Early Power On Configuration setting for feature change.

  @param[in] Operation           Get/Set Cpu EPOC Data
  @param[in] CpuEPOCSet          Cpu EPOC Data

  @retval EFI_SUCCESS             The function completed successfully.
  @exception EFI_UNSUPPORTED      The function is not supported.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_CPU_EPOC_SET) (
  IN      CPU_EPOC_OPERATION        Operation,
  IN OUT  UINT32                     *CpuEPOCSet
  );

/**
  This PPI provides several services for PCH init
**/
struct _PCH_INIT_PPI {
  ///
  /// The function to initialize TC/VC mapping default values
  /// IIO will override the TC VC map with the policy values.
  ///
  PCH_DMI_TCVC_MAP_INITIALIZE DmiTcVcMapInit;
  ///
  /// The function performing TC/VC mapping program, and poll all PCH Virtual Channel
  /// until negotiation completion.
  ///
  PCH_DMI_TCVC_PROG DmiTcVcProg;
  ///
  /// The function performing TC/VC mapping program, and poll all PCH Virtual Channel
  /// until negotiation completion.
  ///
  PCH_DMI_TCVC_POLL DmiTcVcPoll;
  ///
  ///  The function changes the PCH target link speed to DMI Gen 2
  ///
  PCH_DMI_GEN2_PROG     DmiGen2Prog;
  ///
  ///  The function changes the PCH target link speed to DMI Gen 3
  ///
  PCH_DMI_GEN3_PROG     DmiGen3Prog;
  ///
  /// The function provides a way to initialize PCH strap data before soft reset
  /// while doing CPU Only Reset
  ///
  PCH_CPU_STRAP_SET     CpuStrapSet;
  ///
  /// The function sets up CPU EPOC data
  ///
  PCH_CPU_EPOC_SET      CpuEpocSet;
};

#endif
