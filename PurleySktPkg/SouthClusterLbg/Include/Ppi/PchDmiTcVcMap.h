/** @file
  This file defines the PCH DMI TC/VC mapping PPI

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
#ifndef _PCH_DMI_TC_VC_MAP_H_
#define _PCH_DMI_TC_VC_MAP_H_

//
// Extern the GUID for PPI users.
//
extern EFI_GUID                   gPchDmiTcVcMapPpiGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_DMI_TC_VC_PPI PCH_DMI_TC_VC_PPI;

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
  PCH_DMI_VC_TYPE Vc;    ///< The Virtual Channel to which the TC is mapped
  UINT8           TcId;  ///< Tc ID Encoding
} PCH_DMI_TC_CONFIG;

/**
  The data structure to describe DMI VC configuration setting
**/
typedef struct {
  BOOLEAN Enable;     ///< 0: Disable; 1: Enable
  UINT8   VcId;       ///< Vc ID Encoding for the Virtual Channel
} PCH_DMI_VC_CONFIG;

#define DmiTcTypeMax  8

///
/// PCH_DMI_TC_VC_PPI Structure Definition
/// Note: The default DMI TC/VC mapping will be used if it's not initialized
///
struct _PCH_DMI_TC_VC_PPI {
  PCH_DMI_TC_CONFIG DmiTc[DmiTcTypeMax];  ///< Configures PCH DMI Traffic class mapping.
  PCH_DMI_VC_CONFIG DmiVc[DmiVcTypeMax];  ///< Configures PCH DMI Virtual Channel setting.
};

#endif
