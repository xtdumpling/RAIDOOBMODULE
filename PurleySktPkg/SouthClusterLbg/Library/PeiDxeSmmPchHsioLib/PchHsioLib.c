/** @file
  PCH HSIO Library.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

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

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <PchAccess.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchHsioLib.h>
#include <IncludePrivate/PchHsio.h>
#include <Library/HeciMsgLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/PchPolicy.h>
#include <IncludePrivate/PchPolicyHob.h>
#include <Ppi/PchPcieDeviceTable.h>
#include <Library/SpsPeiLib.h>
#include <Library/PchHsioLib.h>
#include <Library/MeTypeLib.h>

/**
  The function returns the Port Id and lane owner for the specified lane

  @param[in]  PhyMode             Phymode that needs to be checked
  @param[out] PortId              Common Lane End Point ID
  @param[out] LaneOwner           Lane Owner

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid lane number
**/
EFI_STATUS
EFIAPI
PchGetLaneInfo (
  IN  UINT32                            LaneNum,
  OUT UINT8                             *PortId,
  OUT UINT8                             *LaneOwner
  )
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;
  UINT32 Los4;

  Los1 = 0;
  Los2 = 0;
  Los3 = 0;
  Los4 = 0;

  PchSeries = GetPchSeries();

  if ((LaneNum > 25) && (PchSeries == PchH)) {
    return EFI_INVALID_PARAMETER;
  } else if (LaneNum < 8) {
    PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
  } else if (LaneNum < 16) {
    PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);
  } else if (LaneNum < 24) {
    PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
  } else {
    PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS4_REG_BASE, &Los4);
  }

  switch (LaneNum) {
    case 0:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L0O));
      *PortId = PID_MODPHY1;
      break;
    case 1:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L1O)>>4);
      *PortId = PID_MODPHY1;
      break;
    case 2:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L2O)>>8);
        *PortId = PID_MODPHY1;
        break;
    case 3:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L3O)>>12);
      *PortId = PID_MODPHY1;
      break;
    case 4:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L4O)>>16);
      *PortId = PID_MODPHY1;
      break;
    case 5:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L5O)>>20);
      *PortId = PID_MODPHY1;
      break;
    case 6:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L6O)>>24);
      *PortId = PID_MODPHY1;
      break;
    case 7:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L7O)>>28);
      *PortId = PID_MODPHY1;
      break;
    case 8:
      *LaneOwner = (UINT8) (Los2 & B_PCH_PCR_FIA_L8O);
      *PortId = PID_MODPHY1;
      break;
    case 9:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L9O)>>4);
      *PortId = PID_MODPHY1;
      break;
    case 10:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L10O)>>8);
      *PortId = PID_MODPHY2;
      break;
    case 11:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L11O)>>12);
      *PortId = PID_MODPHY2;
      break;
    case 12:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L12O)>>16);
      *PortId = PID_MODPHY2;
      break;
    case 13:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L13O)>>20);
      *PortId = PID_MODPHY2;
      break;
    case 14:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L14O)>>24);
      *PortId = PID_MODPHY2;
      break;
    case 15:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L15O)>>28);
      *PortId = PID_MODPHY2;
      break;
    case 16:
      *LaneOwner = (UINT8) (Los3 & B_PCH_PCR_FIA_L16O);
      *PortId = PID_MODPHY2;
      break;
    case 17:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L17O)>>4);
      *PortId = PID_MODPHY2;
      break;
    case 18:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L18O)>>8);
      *PortId = PID_MODPHY2;
      break;
    case 19:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L19O)>>12);
      *PortId = PID_MODPHY2;
      break;
    case 20:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L20O)>>16);
      *PortId = PID_MODPHY2;
      break;
    case 21:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L21O)>>20);
      *PortId = PID_MODPHY2;
      break;
    case 22:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L22O)>>24);
      *PortId = PID_MODPHY2;
      break;
    case 23:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L23O)>>28);
      *PortId = PID_MODPHY2;
      break;
    case 24:
      *LaneOwner = (UINT8) (Los4 & B_PCH_PCR_FIA_L24O);
      *PortId = PID_MODPHY2;
      break;
    case 25:
      *LaneOwner = (UINT8) ((Los4 & B_PCH_PCR_FIA_L25O)>>4);
      *PortId = PID_MODPHY2;
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


/**
  The function returns the Port Id and lane owner for the specified lane in WM20 FIA

  @param[in]  PhyMode             Phymode that needs to be checked
  @param[out] PortId              Common Lane End Point ID
  @param[out] LaneOwner           Lane Owner

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid lane number
**/
EFI_STATUS
EFIAPI
PchGetLaneInfoWM20 (
  IN  UINT32                            LaneNum,
  OUT UINT8                             *PortId,
  OUT UINT8                             *LaneOwner
  )
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;

  Los1 = 0;
  Los2 = 0;
  Los3 = 0;

  PchSeries = GetPchSeries();

  if (LaneNum > 20) {
    return EFI_INVALID_PARAMETER;
  } else if (LaneNum < 8) {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
  } else if (LaneNum < 16) {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);
  } else {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS4_REG_BASE, &Los3);
  }

  switch (LaneNum) {
    case 0:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L0O));
      *PortId = PID_MODPHY4;
      break;
    case 1:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L1O)>>4);
      *PortId = PID_MODPHY4;
      break;
    case 2:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L2O)>>8);
      *PortId = PID_MODPHY4;
      break;
    case 3:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L3O)>>12);
      *PortId = PID_MODPHY4;
      break;
    case 4:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L4O)>>16);
      *PortId = PID_MODPHY5;
      break;
    case 5:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L5O)>>20);
      *PortId = PID_MODPHY5;
      break;
    case 6:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L6O)>>24);
      *PortId = PID_MODPHY5;
      break;
    case 7:
      *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L7O)>>28);
      *PortId = PID_MODPHY5;
      break;
    case 8:
      *LaneOwner = (UINT8) (Los2 & B_PCH_PCR_FIA_L8O);
      *PortId = PID_MODPHY5;
      break;
    case 9:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L9O)>>4);
      *PortId = PID_MODPHY5;
      break;
    case 10:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L10O)>>8);
      *PortId = PID_MODPHY5;
      break;
    case 11:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L11O)>>12);
      *PortId = PID_MODPHY5;
      break;
    case 12:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L12O)>>16);
      *PortId = PID_MODPHY5;
      break;
    case 13:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L13O)>>20);
      *PortId = PID_MODPHY5;
      break;
    case 14:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L14O)>>24);
      *PortId = PID_MODPHY5;
      break;
    case 15:
      *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L15O)>>28);
      *PortId = PID_MODPHY5;
      break;
    case 16:
      *LaneOwner = (UINT8) (Los3 & B_PCH_PCR_FIA_L16O);
      *PortId = PID_MODPHY5;
      break;
    case 17:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L17O)>>4);
      *PortId = PID_MODPHY5;
      break;
    case 18:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L18O)>>8);
      *PortId = PID_MODPHY5;
      break;
    case 19:
      *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L19O)>>12);
      *PortId = PID_MODPHY5;
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


/**
  Determine the lane number of a specified port

  @param[out] LaneNum                   GBE Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetGbeLaneNum (
  UINT8               *LaneNum
  )
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;

  PchSeries = GetPchSeries ();
  // For LBG the GbE Lan can be configured on Lanes: 9,10,11,14,17
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);

  if (PchSeries == PchLp) {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
    if (((Los1 & B_PCH_PCR_FIA_L6O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 6;
      return EFI_SUCCESS;
    } else if (((Los1 & B_PCH_PCR_FIA_L7O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 7;
      return EFI_SUCCESS;
    } else if ((Los2 & B_PCH_PCR_FIA_L8O) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 8;
      return EFI_SUCCESS;
    } else if (((Los2 & B_PCH_PCR_FIA_L12O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 12;
      return EFI_SUCCESS;
    } else if (((Los2 & B_PCH_PCR_FIA_L13O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 13;
      return EFI_SUCCESS;
    }
  } else {
    PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
    if (((Los2 & B_PCH_PCR_FIA_L9O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 9;
      return EFI_SUCCESS;
    } else if (((Los2 & B_PCH_PCR_FIA_L10O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 10;
      return EFI_SUCCESS;
    } else if (((Los2 & B_PCH_PCR_FIA_L11O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 11;
      return EFI_SUCCESS;
    } else if (((Los2 & B_PCH_PCR_FIA_L14O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 14;
      return EFI_SUCCESS;
    } else if (((Los3 & B_PCH_PCR_FIA_L17O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 17;
      return EFI_SUCCESS;
    }
  }
  return EFI_UNSUPPORTED;
}

/**
  Determine the lane number of a specified port

  @param[in]  Usb3LaneIndex             USB3 Lane Index
  @param[out] LaneNum                   Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetUsb3LaneNum (
  UINT32              Usb3LaneIndex,
  UINT8               *LaneNum
  )
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;

  PchSeries = GetPchSeries ();
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);

  if (PchSeries == PchLp) {
    switch (Usb3LaneIndex) {
      case 0:
        if ((Los1 & B_PCH_PCR_FIA_L0O) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 0;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los1 & B_PCH_PCR_FIA_L1O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 1;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los1 & B_PCH_PCR_FIA_L2O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 2;
          return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los1 & B_PCH_PCR_FIA_L3O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 3;
          return EFI_SUCCESS;
        }
        break;
      case 4:
        if (((Los1 & B_PCH_PCR_FIA_L4O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 4;
          return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los1 & B_PCH_PCR_FIA_L5O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 5;
          return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported USB3 Lane Index"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
        break;
    }
  } else {
    switch (Usb3LaneIndex) {
      case 0:
        if ((Los1 & B_PCH_PCR_FIA_L0O) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 0;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los1 & B_PCH_PCR_FIA_L1O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 1;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los1 & B_PCH_PCR_FIA_L2O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 2;
          return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los1 & B_PCH_PCR_FIA_L3O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 3;
          return EFI_SUCCESS;
        }
        break;
      case 4:
        if (((Los1 & B_PCH_PCR_FIA_L4O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 4;
          return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los1 & B_PCH_PCR_FIA_L5O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 5;
          return EFI_SUCCESS;
        }
        break;
      case 6:
        if (((Los1 & B_PCH_PCR_FIA_L6O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 6;
          return EFI_SUCCESS;
        }
        break;
      case 7:
        if (((Los1 & B_PCH_PCR_FIA_L7O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 7;
          return EFI_SUCCESS;
        }
        break;
      case 8:
        if ((Los2 & B_PCH_PCR_FIA_L8O) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 8;
          return EFI_SUCCESS;
        }
        break;
      case 9:
        if (((Los2 & B_PCH_PCR_FIA_L9O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 9;
          return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported USB3 Lane Index"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
        break;
    }
  }

  DEBUG ((DEBUG_ERROR, "Specyfied Lane Index is not owned by USB3!\n"));

  return EFI_UNSUPPORTED;
}

/**
  Determine the lane number of a specified port

  @param[in]  SataLaneIndex             Sata Lane Index
  @param[out] LaneNum                   Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetSataLaneNum (
  UINT32              SataLaneIndex,
  UINT8               *LaneNum
  )
{
  PCH_SERIES  PchSeries;
  UINT32 Los2;
  UINT32 Los3;
  UINT32 Los4;

  PchSeries = GetPchSeries ();
  // For LBG SATA first controller can be configured on Lanes: 18-25
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);

  if (PchSeries == PchLp) {
    switch (SataLaneIndex) {
      case 0:
        if (((Los2 & B_PCH_PCR_FIA_L10O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 10;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los2 & B_PCH_PCR_FIA_L11O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 11;
          return EFI_SUCCESS;
        } else if (((Los2 & B_PCH_PCR_FIA_L14O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 14;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los2 & B_PCH_PCR_FIA_L15O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 15;
          return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported SATA Lane Index"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
        break;
    }
  } else {
    PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS4_REG_BASE, &Los4);
    switch (SataLaneIndex) {
      case 0:
        if (((Los3 & B_PCH_PCR_FIA_L18O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 18;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los3 & B_PCH_PCR_FIA_L19O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 19;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los3 & B_PCH_PCR_FIA_L20O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 20;
          return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los3 & B_PCH_PCR_FIA_L21O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 21;
          return EFI_SUCCESS;
        }
        break;
      case 4:
        if (((Los3 & B_PCH_PCR_FIA_L22O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 22;
          return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los3 & B_PCH_PCR_FIA_L23O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 23;
          return EFI_SUCCESS;
        }
        break;
      case 6:
        if ((Los4 & B_PCH_PCR_FIA_L24O) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 24;
          return EFI_SUCCESS;
        }
        break;
      case 7:
        if (((Los4 & B_PCH_PCR_FIA_L25O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
          *LaneNum = 25;
          return EFI_SUCCESS;
        }
        break;

      default:
        DEBUG ((DEBUG_ERROR, "Unsupported SATA Lane Index"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
        break;
    }
  }

  DEBUG ((DEBUG_ERROR, "Specyfied Lane Index is not owned by SATA!\n"));

  return EFI_UNSUPPORTED;
}

/**
  Determine the lane number of a specified port for secondary SATA

  @param[in]  sSataLaneIndex             sSata Lane Index
  @param[out] LaneNum                   Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetsSataLaneNum (
  UINT32              sSataLaneIndex,
  UINT8               *LaneNum
  )
{
  UINT32 Los2;
  UINT32 Los3;
  // For LBG SATA second controller can be configured on Lanes: 12-17
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);
  switch (sSataLaneIndex) {
  case 0:
    if (((Los2 & B_PCH_PCR_FIA_L12O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
      *LaneNum = 12;
      return EFI_SUCCESS;
    }
    break;
  case 1:
    if (((Los2 & B_PCH_PCR_FIA_L13O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
      *LaneNum = 13;
      return EFI_SUCCESS;
    }
    break;
  case 2:
    if (((Los2 & B_PCH_PCR_FIA_L14O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
      *LaneNum = 14;
      return EFI_SUCCESS;
    }
    break;
  case 3:
    if (((Los2 & B_PCH_PCR_FIA_L15O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
      *LaneNum = 15;
      return EFI_SUCCESS;
    }
    break;
  case 4:
    if ((Los3 & B_PCH_PCR_FIA_L16O) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
      *LaneNum = 16;
      return EFI_SUCCESS;
    }
    break;
  case 5:
    if (((Los3 & B_PCH_PCR_FIA_L17O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
      *LaneNum = 17;
      return EFI_SUCCESS;
    }
    break;
  default:
    DEBUG ((DEBUG_ERROR, "Unsupported secondary SATA Lane Index"));
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
    break;
  }
  return EFI_UNSUPPORTED;
}


/**
  Determine the lane number of a specified port

  @param[in]  PcieLaneIndex             PCIE Root Port Lane Index
  @param[out] LaneNum                   Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetPcieLaneNum (
  UINT32              PcieLaneIndex,
  UINT8               *LaneNum
  )
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;
  UINT32 Los4;

  PchSeries = GetPchSeries ();
  // Server LBG chipset can connect PCIe controllers to lanes: 6-25
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);

  if (PchSeries == PchLp) {
    switch (PcieLaneIndex) {
      case 0:
        if (((Los1 & B_PCH_PCR_FIA_L4O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 4;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los1 & B_PCH_PCR_FIA_L5O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 5;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los1 & B_PCH_PCR_FIA_L6O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 6;
          return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los1 & B_PCH_PCR_FIA_L7O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 7;
          return EFI_SUCCESS;
        }
        break;
      case 4:
        if ((Los2 & B_PCH_PCR_FIA_L8O) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 8;
          return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los2 & B_PCH_PCR_FIA_L9O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 9;
          return EFI_SUCCESS;
        }
        break;
      case 6:
        if (((Los2 & B_PCH_PCR_FIA_L10O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 10;
          return EFI_SUCCESS;
        }
        break;
      case 7:
        if (((Los2 & B_PCH_PCR_FIA_L11O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 11;
          return EFI_SUCCESS;
        }
        break;
      case 8:
        if (((Los2 & B_PCH_PCR_FIA_L12O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 12;
          return EFI_SUCCESS;
        }
        break;
      case 9:
        if (((Los2 & B_PCH_PCR_FIA_L13O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 13;
          return EFI_SUCCESS;
        }
        break;
      case 10:
        if (((Los2 & B_PCH_PCR_FIA_L14O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 14;
          return EFI_SUCCESS;
        }
        break;
      case 11:
        if (((Los2 & B_PCH_PCR_FIA_L15O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 15;
          return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported PCIE Root Port Lane Index"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
        break;
    }
  } else {
    PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
    PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS4_REG_BASE, &Los4);

    switch (PcieLaneIndex) {
      case 0:
        if (((Los1 & B_PCH_PCR_FIA_L6O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 6;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los1 & B_PCH_PCR_FIA_L7O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 7;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if ((Los2 & B_PCH_PCR_FIA_L8O) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 8;
          return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los2 & B_PCH_PCR_FIA_L9O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 9;
          return EFI_SUCCESS;
        }
        break;
      case 4:
        if (((Los2 & B_PCH_PCR_FIA_L10O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 10;
          return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los2 & B_PCH_PCR_FIA_L11O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 11;
          return EFI_SUCCESS;
        }
        break;
      case 6:
        if (((Los2 & B_PCH_PCR_FIA_L12O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 12;
          return EFI_SUCCESS;
        }
        break;
      case 7:
        if (((Los2 & B_PCH_PCR_FIA_L13O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 13;
          return EFI_SUCCESS;
        }
        break;
      case 8:
        if (((Los2 & B_PCH_PCR_FIA_L14O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 14;
          return EFI_SUCCESS;
        }
        break;
      case 9:
        if (((Los2 & B_PCH_PCR_FIA_L15O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 15;
          return EFI_SUCCESS;
        }
        break;
      case 10:
        if ((Los3 & B_PCH_PCR_FIA_L16O) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 16;
          return EFI_SUCCESS;
        }
        break;
      case 11:
        if (((Los3 & B_PCH_PCR_FIA_L17O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 17;
          return EFI_SUCCESS;
        }
        break;
      case 12:
        if (((Los3 & B_PCH_PCR_FIA_L18O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 18;
          return EFI_SUCCESS;
        }
        break;
      case 13:
        if (((Los3 & B_PCH_PCR_FIA_L19O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 19;
          return EFI_SUCCESS;
        }
        break;
      case 14:
        if (((Los3 & B_PCH_PCR_FIA_L20O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 20;
          return EFI_SUCCESS;
        }
        break;
      case 15:
        if (((Los3 & B_PCH_PCR_FIA_L21O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 21;
          return EFI_SUCCESS;
        }
        break;
      case 16:
        if (((Los3 & B_PCH_PCR_FIA_L22O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 22;
          return EFI_SUCCESS;
        }
        break;
      case 17:
        if (((Los3 & B_PCH_PCR_FIA_L23O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 23;
          return EFI_SUCCESS;
        }
        break;
      case 18:
        if ((Los4 & B_PCH_PCR_FIA_L24O) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 24;
          return EFI_SUCCESS;
        }
        break;
      case 19:
        if (((Los4 & B_PCH_PCR_FIA_L25O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 25;
          return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported PCIE Root Port Lane Index\n"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
        break;
    }
  }
  return EFI_UNSUPPORTED;
}

/**
  Get HSIO lane representation needed to perform any operation on the lane.

  @param[in]  LaneIndex  Number of the HSIO lane
  @param[out] HsioLane   HSIO lane representation
**/
VOID
HsioGetLane (
  IN   UINT8       LaneIndex,
  OUT  HSIO_LANE   *HsioLane
  )
{
  // SPT-LP
    // iolane  0 -  5 : 0xEA - 000, 200, 400, 600, 800, a00
    // iolane  6 -  9 : 0xE9 - 000, 200, 400, 600
    // iolane 10 - 15 : 0xA9 - 000, 200, 400, 600, 800, a00
  // SPT-H
    // iolane  0 -  5 : 0xEA - 000, 200, 400, 600, 800, a00
    // iolane  6 - 13 : 0xE9 - 000, 200, 400, 600, 800, a00, c00, e00
    // iolane 14 - 21 : 0xA9 - 000, 200, 400, 600, 800, a00, c00, e00
    // iolane 22 - 29 : 0xA8 - 000, 200, 400, 600, 800, a00, c00, e00

  // LBG
    // iolane  0 -  9 : 0xE9 - 000, 200, 400, 600, 800, a00, c00, e00, 1000, 1200
    // iolane 10 - 25 : 0xA9 - 000, 200, 400, 600, 800, a00, c00, e00, 1000, 1200 ... etc.

  static UINT8 IoLanesLp[]   = { 0, 6, 10, 16 };
  static UINT8 IoLanesH[]    = { 0, 6, 14, 22, 30 };
  static UINT8 IoLanesServer[]    = { 0, 10, 26 };
  static UINT8 Pids[]        = { PID_MODPHY1, PID_MODPHY2 };

  UINT8* IoLanes;
  UINT8  PidMax;
  UINT32 Index;

  if (HsioLane == NULL) {
    DEBUG ((DEBUG_ERROR, "HsioGetLane - Error. HsioLane Invalid Parameter!!!\n"));
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    return;
  }

  IoLanes = IoLanesServer;
  PidMax  = 2;

  ASSERT (LaneIndex < IoLanes[PidMax]);

  for (Index = 0; Index < PidMax; ++Index) {
     if (LaneIndex < IoLanes[Index + 1]) {
       HsioLane->Index = LaneIndex;
       HsioLane->Pid   = Pids[Index];
       HsioLane->Base  = (LaneIndex - IoLanes[Index]) * 0x200;
       return;
     }
  }
  ASSERT (FALSE);
}

/**
  Get HSIO lane representation needed to perform any operation on the lane.

  @param[in]  LaneIndex  Number of the HSIO lane in WM20 FIA
  @param[out] HsioLane   HSIO lane representation
**/
VOID
HsioGetLaneWM20 (
  IN   UINT8       LaneIndex,
  OUT  HSIO_LANE   *HsioLane
  )
{
  // LBG WM20
  // iolane  0 -  3 : 0xEB - 000, 200, 400, 600, 800, a00, c00, e00, 1000, 1200
  // iolane  4 - 19 : 0x10 - 000, 200, 400, 600, 800, a00, c00, e00
  static UINT8 IoLanesServer[]    = { 0, 4, 20};
  static UINT8 Pids[]        = { PID_MODPHY4, PID_MODPHY5};

  UINT8* IoLanes;
  UINT8  PidMax;
  UINT32 Index;

  if (HsioLane == NULL) {
    DEBUG ((DEBUG_ERROR, "HsioGetLaneWM20 - Error. HsioLane Invalid Parameter!!!\n"));
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    return;
  }

  IoLanes = IoLanesServer;
  PidMax  = 2;

  ASSERT (LaneIndex < IoLanes[PidMax]);

  for (Index = 0; Index < PidMax; ++Index) {
     if (LaneIndex < IoLanes[Index + 1]) {
       HsioLane->Index = LaneIndex;
       HsioLane->Pid   = Pids[Index];
       HsioLane->Base  = (LaneIndex - IoLanes[Index]) * 0x200;
       return;
     }
  }
  ASSERT (FALSE);
}

/**
  The function programs the Pcie Pll SSC registers for alternate clock.
**/
VOID
PchConfigureSscAlternate (
  VOID
  )
{
  EFI_STATUS                            Status;
  PCH_POLICY_PPI                        *PchPolicyPpi;
  
  Status = PeiServicesLocatePpi ( &gPchPlatformPolicyPpiGuid,
                                   0,
                                   NULL,
                                   (VOID **)&PchPolicyPpi
                                 );
  ASSERT_EFI_ERROR (Status);

  if (PchPolicyPpi->PmConfig.PciePllSsc == PCIE_PLL_SSC_DISABLED) {
    PeiHeciSetSSCAlternate (FALSE);
  } else if (PchPolicyPpi->PmConfig.PciePllSsc != PCIE_PLL_SSC_AUTO){
    PeiHeciSetSSCAlternate (TRUE);
  }
}

/**
  The function programs the Pcie Pll SSC registers for native and hybrid clock.
**/
VOID
PchConfigureSscWm20Gen2Pll (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT8                                 PciePllSsc;
  UINT32                                Data32;
  PCH_POLICY_PPI                        *PchPolicyPpi;
  UINT8                                 PciePllSscTable[] = {
                                          0x0,  // 0%
                                          0x6,  // 0.1%
                                          0xD,  // 0.2%
                                          0x14, // 0.3%
                                          0x1A, // 0.4%
                                          0x21, // 0.5%
                                          0x28, // 0.6%
                                          0x2E, // 0.7%
                                          0x35, // 0.8%
                                          0x3C, // 0.9%
                                          0x42, // 1.0%
                                          0x49, // 1.1%
                                          0x50, // 1.2%
                                          0x56, // 1.3%
                                          0x5D, // 1.4%
                                          0x64, // 1.5%
                                          0x6A, // 1.6%
                                          0x71, // 1.7%
                                          0x78, // 1.8%
                                          0x7E, // 1.9%
                                          0x1F  // POR for LBG
                                        };

  Status = PeiServicesLocatePpi ( &gPchPlatformPolicyPpiGuid,
                                   0,
                                   NULL,
                                   (VOID **)&PchPolicyPpi
                                 );
  ASSERT_EFI_ERROR (Status);
  if (PchPolicyPpi->PmConfig.PciePllSsc != PCIE_PLL_SSC_AUTO) {
    return;
  }
  if (PchPolicyPpi->PmConfig.PciePllSsc == PCIE_PLL_SSC_DISABLED) {
    ///
    /// The user choose to disable SSC
    ///
    PchPcrAndThenOr32 (
      PID_MODPHY4,
      R_PCH_HSIO_PLL_SSC_DWORD2,
      (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD2_SSCSEN,
      0
      );
    DEBUG ((DEBUG_INFO, "HSIO : PciePllSsc: SSC forced to disabled!\n"));
    return;
  }

  PciePllSsc = PchPolicyPpi->PmConfig.PciePllSsc;
  if (PciePllSsc > MAX_PCIE_PLL_SSC_PERCENT) {
    PciePllSsc = MAX_PCIE_PLL_SSC_PERCENT;
  }

  ///
  /// Step 1:  Clear i_sscen_h to 0b
  ///
  PchPcrAndThenOr32 ( PID_MODPHY4,
                      R_PCH_HSIO_PLL_SSC_DWORD2,
                      (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD2_SSCSEN,
                      0
                    );

  ///
  /// Step 2:  Write the desired Down Spread % bit values in i_sscstepsize_7_0
  ///
  PchPcrAndThenOr32 ( PID_MODPHY4,
                      R_PCH_HSIO_PLL_SSC_DWORD2,
                      (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD2_SSCSTEPSIZE_7_0,
                      (UINT32) (PciePllSscTable[PciePllSsc] << N_PCH_HSIO_PLL_SSC_DWORD2_SSCSTEPSIZE_7_0)
                    );

  ///
  /// Step 3:  Set i_ssc_propagate_h to 1b in the LCPLL_CFG1 register
  ///
  PchPcrAndThenOr32 ( PID_MODPHY4,
                      R_PCH_HSIO_PLL_SSC_DWORD3,
                      (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD3_SSC_PROPAGATE,
                      (UINT32) B_PCH_HSIO_PLL_SSC_DWORD3_SSC_PROPAGATE
                    );

  ///
  /// Step 4:  Set i_sscen_h to 1b
  ///
  PchPcrAndThenOr32 ( PID_MODPHY4,
                      R_PCH_HSIO_PLL_SSC_DWORD2,
                      (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD2_SSCSEN,
                      (UINT32) B_PCH_HSIO_PLL_SSC_DWORD2_SSCSEN
                    );

  PchPcrRead32 (PID_MODPHY4, R_PCH_HSIO_PLL_SSC_DWORD2, &Data32);
  DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, PciePllSsc = 0x%08x\n", PID_MODPHY4, R_PCH_HSIO_PLL_SSC_DWORD2, (UINT32) Data32));

}


