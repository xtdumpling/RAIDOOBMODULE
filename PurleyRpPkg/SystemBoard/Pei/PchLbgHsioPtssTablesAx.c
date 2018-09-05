/** @file
    LbgPchH Ax HSIO PTSS C File

@copyright
 Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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
#include "PchLbgHsioPtssTablesAx.h"

HSIO_PTSS_TABLES PchLbgHsioPtss_Ax[] = {
  {{0xA9, 14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 18, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 19, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 20, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 21, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeOpalCitySTHI},
  {{0xA9, 14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeNeonCityEPRP},
  {{0xA9, 15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeNeonCityEPRP},
  {{0xA9, 16, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeNeonCityEPRP},
  {{0xA9, 17, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeNeonCityEPRP},
  {{0xEB, 0, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchDmiTopoUnknown, TypePurleyLBGEPDVP},
  {{0xEB, 0, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchDmiTopoUnknown, TypePurleyLBGEPDVP},
  {{0xEB, 1, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchDmiTopoUnknown, TypePurleyLBGEPDVP},
  {{0xEB, 1, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchDmiTopoUnknown, TypePurleyLBGEPDVP},
  {{0xEB, 2, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchDmiTopoUnknown, TypePurleyLBGEPDVP},
  {{0xEB, 2, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchDmiTopoUnknown, TypePurleyLBGEPDVP},
  {{0xEB, 3, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchDmiTopoUnknown, TypePurleyLBGEPDVP},
  {{0xEB, 3, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchDmiTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 18, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 18, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 18, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 18, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 19, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 19, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 19, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 19, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 20, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 20, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 20, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 20, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 21, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 21, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 21, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 21, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xA9, 25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypePurleyLBGEPDVP},
  {{0xEB, 0, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchDmiTopoUnknown, TypeNeonCityEPECB},
  {{0xEB, 0, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchDmiTopoUnknown, TypeNeonCityEPECB},
  {{0xEB, 1, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchDmiTopoUnknown, TypeNeonCityEPECB},
  {{0xEB, 1, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchDmiTopoUnknown, TypeNeonCityEPECB},
  {{0xEB, 2, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchDmiTopoUnknown, TypeNeonCityEPECB},
  {{0xEB, 2, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchDmiTopoUnknown, TypeNeonCityEPECB},
  {{0xEB, 3, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchDmiTopoUnknown, TypeNeonCityEPECB},
  {{0xEB, 3, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchDmiTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x11c, 0x10000000, (UINT32) ~0xF8000000}, PchPcieTopoUnknown, TypeNeonCityEPECB},
  {{0xA9, 25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown, TypeNeonCityEPECB},
};

UINT32 PchLbgHsioPtss_Ax_Size = sizeof (PchLbgHsioPtss_Ax) / sizeof (PchLbgHsioPtss_Ax[0]);
