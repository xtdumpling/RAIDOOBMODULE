/** @file
  This files contains Pch services for RCs usage

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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

#ifndef _PCH_RC_LIB_H_
#define _PCH_RC_LIB_H_

#include <PchAccess.h>

//
// Data structure definitions
//
typedef enum {
  DmiVcTypeVc0,
  DmiVcTypeVc1,
  DmiVcTypeVcm,
  DmiVcTypeMax
} PCH_DMI_VC_TYPE;

/**
  The data structure to describe DMI TC configuration setting
**/
typedef struct {
  PCH_DMI_VC_TYPE       Vc;       ///< The Virtual Channel to which the TC is mapped
  UINT8                 TcId;     ///< Tc ID Encoding
} PCH_DMI_TC_CONFIG;

/**
  The data structure to describe DMI VC configuration setting
**/
typedef struct {
  BOOLEAN               Enable;   ///< 0: Disable; 1: Enable
  UINT8                 VcId;     ///< Vc ID Encoding for the Virtual Channel
} PCH_DMI_VC_CONFIG;

#define DmiTcTypeMax  3

/**
  PCH_DMI_TC_VC_MAP Structure Definition
  Note: The default DMI TC/VC mapping will be used if it's not initialized
**/
typedef struct {
  PCH_DMI_TC_CONFIG DmiTc[DmiTcTypeMax];  ///< Configures PCH DMI Traffic class mapping.
  PCH_DMI_VC_CONFIG DmiVc[DmiVcTypeMax];  ///< Configures PCH DMI Virtual Channel setting.
} PCH_DMI_TC_VC_MAP;

/**
  Type definitions for CPU STRAP OPERATION.
**/
typedef enum _CPU_STRAP_OPERATION
{
  GetCpuStrapSetData,
  SetCpuStrapSetData,
  LockCpuStrapSetData
} CPU_STRAP_OPERATION;

/**
  Type definitions for CPU EPOC OPERATION.
**/
typedef enum _CPU_EPOC_OPERATION
{
  GetCpuEpocData,
  SetCpuEpocData
} CPU_EPOC_OPERATION;

/**
  List of PCH Cio2 Mode, PCIE mode or Hidden mode.
**/
typedef enum _PCH_CIO2_MODE
{
  PchCio2ModePCIe = 1,
  PchCio2ModeHidden
} PCH_CIO2_MODE;


/**
  Get PCH default TC VC Mapping settings. This funciton returns the default PCH setting 
  System Agent can update the settings according to polices.

  @param[in, out] PchDmiTcVcMap         Buffer for PCH_DMI_TC_VC_MAP instance.

**/
VOID
PchDmiTcVcMapInit (
  IN OUT  PCH_DMI_TC_VC_MAP             *PchDmiTcVcMap
  );

/**
  The function performing TC/VC mapping program, and poll all PCH Virtual Channel
  until negotiation completion.

  @param[in] PchDmiTcVcMap              Buffer for PCH_DMI_TC_VC_MAP instance.

  @retval EFI_SUCCESS                   The function completed successfully
  @retval Others                        All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
PchDmiTcVcProg (
  IN  PCH_DMI_TC_VC_MAP                 *PchDmiTcVcMap
  );
/**
  The function performing TC/VC mapping program, and poll all PCH Virtual Channel
  until negotiation completion.

  @param[in] PchDmiTcVcMap              Buffer for PCH_DMI_TC_VC_MAP instance.

  @retval EFI_SUCCESS                   The function completed successfully
  @retval Others                        All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
PchDmiTcVcPoll (
  IN  PCH_DMI_TC_VC_MAP                 *PchDmiTcVcMap
  );
/**
  The function set the Target Link Speed in PCH to DMI GEN 2.

**/
VOID
EFIAPI
PchDmiGen2Prog (
  VOID
  );

/**
  The function set the Target Link Speed in PCH to DMI GEN 3.

**/
VOID
EFIAPI
PchDmiGen3Prog (
  VOID
  );

/**
  The function is used while doing CPU Only Reset, where PCH may be required
  to initialize strap data before soft reset.

  @param[in] Operation                  Get/Set Cpu Strap Set Data
  @param[in, out] CpuStrapSet           Cpu Strap Set Data

  @retval EFI_SUCCESS                   The function completed successfully.
  @exception EFI_UNSUPPORTED            The function is not supported.
**/
EFI_STATUS
EFIAPI
PchCpuStrapSet (
  IN      CPU_STRAP_OPERATION           Operation,
  IN OUT  UINT16                        *CpuStrapSet
  );

/**
  Set Early Power On Configuration setting for feature change.

  @param[in] Operation                  Get or set EPOC data
  @param[in, out] CpuEPOCSet            Cpu EPOC Data

  @retval EFI_SUCCESS                   The function completed successfully.
  @exception EFI_UNSUPPORTED            The function is not supported.
**/
EFI_STATUS
EFIAPI
PchCpuEpocSet (
  IN     CPU_EPOC_OPERATION       Operation,
  IN OUT  UINT32                    *CpuEpocSet
  );


/**
  Enable VTd support in PSF.
  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
PchPsfEnableVtd (
  VOID
  );




#endif
