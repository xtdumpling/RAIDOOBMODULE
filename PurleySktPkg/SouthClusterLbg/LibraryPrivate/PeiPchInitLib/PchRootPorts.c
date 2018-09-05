//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  X11DDW power saving.
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     Aug/04/2017
//
//  Rev. 1.00
//    Bug Fix:  Fixed SelfTest error.
//    Reason:   
//    Auditor:  Stephen Chen
//    Date:     Mar/06/2017
//
//****************************************************************************
/** @file
  This file contains functions that initializes PCI Express Root Ports of PCH.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by the license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#include "PchInitPei.h"
#include "PchHsioLaneLib.h"
#include <Uefi/UefiBaseType.h>
#include <Library/PlatformHooksLib.h>
#include <Ppi/Dwr.h>
#include <Token.h>	//SMCPKG_SUPPORT
#include <Library/SetupLib.h>	//SmcOptimize_SUPPORT
/**
  Represents PCIe controller configuration.
**/
typedef enum {
  Pcie4x1      = 0,
  Pcie1x2_2x1  = 1,
  Pcie2x2      = 2,
  Pcie1x4      = 3
} PCIE_CONTROLLER_CONFIG;

GLOBAL_REMOVE_IF_UNREFERENCED CONST CHAR8* mPcieControllerConfigName[] = {
  "4x1",
  "1x2-2x1",
  "2x2",
  "1x4"
};

/**
  Device information structure
**/
typedef struct {
  UINT16  Vid;
  UINT16  Did;
  UINT8   MaxLinkSpeed;
  UINT8   MaxLinkWidth;
} PCIE_DEVICE_INFO;

/**
  Software Equalization flow parameters
**/
typedef struct {
  PCH_PCIE_EQ_LANE_PARAM  CoeffList[PCH_PCIE_SWEQ_COEFFS_MAX];
  UINT8                   PresetList[5];
  UINT16                  DwellTimeUs;
  UINT16                  ErrorThreshold;
  UINT16                  RecThreshold;
  UINT16                  RetrainTimeoutMs;
  UINT16                  RecoveryWait;
} SW_EQ_PARAMS;

/**
  Gen3 Equalization lane context
**/
typedef struct {
  HSIO_LANE HsioLane;   //< HSIO lane structure
  UINT32    PcieLane;   //< PCIe lane number (0 based)
  UINT32    BestIndex;  //< Best coefficient found
  UINT32    BestMargin; //< Best margin
} EQ_LANE_CONTEXT;

/**
  Gen3 Equalization context
**/
typedef struct {
  UINT16           RpIndex;
  UINT8            MaxLinkWidth;
  UINT8            LinkWidth;
  UINTN            RpBase;
  EQ_LANE_CONTEXT  Lane[4];
  BOOLEAN          Ctoc;
  CONST PCH_PCIE_ROOT_PORT_CONFIG  *RpConfig;
} EQ_CONTEXT;

CONST EFI_PEI_PPI_DESCRIPTOR gDirtyWarmResetSignalPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gDirtyWarmResetSignalGuid,
  NULL
};

const UINT8 DwrTempPciBus = 2;

/**
  Translate PCIe Port/Lane pair to 0-based PCIe lane number.

  @param[in] RpIndex    Root Port index
  @param[in] RpLane     Root Port Lane (0-3)

  @return PCIe lane number (0-based)
**/
UINT32
PchPcieLane (
  UINT32 RpIndex, 
  UINT32 RpLane
  );

/**
  wrapper for PchGetLaneInfo() that returns the LaneOwner data directly instead of through pointer
  @param[in]  LaneNumber

  @retval     0 - V_PCH_PCR_FIA_LANE_OWN_PCIEDMI
              1 - V_PCH_PCR_FIA_LANE_OWN_USB3
              ...
              0xFF - error, illegal LaneNumber
**/
UINT8
GetLaneOwner (
  UINT32 LaneNumber
  );

/**
  Returns the PCIe controller configuration (4x1, 1x2-2x1, 2x2, 1x4)

  @param[in] ControllerIndex        Number of PCIe controller (0 based)

  @retval PCIe controller configuration
**/
PCIE_CONTROLLER_CONFIG
GetPcieControllerConfig (
  IN UINT32 ControllerIndex
  );

/**
  Calculates the index of the first port on the same controller.

  @param[in] RpIndex     Root Port Number (0-based)

  @return Index of the first port on the first controller.
**/
UINT32
PchGetPcieFirstPortIndex (
  IN     UINT32  RpIndex
  )
{
  UINT32  ControllerIndex;

  ControllerIndex = RpIndex / PCH_PCIE_CONTROLLER_PORTS;
  return ControllerIndex * PCH_PCIE_CONTROLLER_PORTS;
}

/**
  This function determines whether root port is configured in non-common clock mode.
  Result is based on the NCC soft-strap setting.

  @param[in] RpBase      Root Port base address

  @retval TRUE           Port in NCC SSC mode.
  @retval FALSE          Port not in NCC SSC mode.
**/
BOOLEAN
IsPcieNcc (
  IN     UINTN  RpBase
  )
{
  if (MmioRead16 (RpBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_SCC) {
    return FALSE;
  } else {
    return TRUE;
  }
}

BOOLEAN
IsPcieLaneReversalEnabled (
  IN     UINT32  RpIndex
  )
{
  UINT32  Data32;
  PchSbiRpPciRead32 (PchGetPcieFirstPortIndex (RpIndex), R_PCH_PCIE_PCIEDBG, &Data32);
  return !!(Data32 & B_PCH_PCIE_PCIEDBG_LR);
}

/**
  This function checks if de-emphasis needs to be changed from default for a given rootport

  @param[in] PortIndex    Root Port number
  @param[in] PcieConfig   Pointer to a PCH_PCIE_CONFIG that provides the platform setting
  @param[in] DevInfo      Information on device that is connected to rootport

  @retval TRUE            De-emphasis needs to be changed
  @retval FALSE           No need to change de-emphasis
**/
BOOLEAN
NeedDecreasedDeEmphasis (
  IN PCIE_DEVICE_INFO      DevInfo
  )
{
  //
  // Intel WiGig devices
  //
  if (DevInfo.Vid == V_PCH_INTEL_VENDOR_ID && DevInfo.Did == 0x093C) {
    return TRUE;
  }
  return FALSE;
}
/**
  This function checks if PCIE Gen3 training should be delayed.
  Delay should happen on software Gen3 training during S3 resume,
  because in that case it will be handled by bootscript.

  @param[in] PcieConfig   Pointer to a PCH_PCIE_CONFIG that provides the platform setting
  @param[in] RpIndex      Root Port number

  @retval TRUE            Delay Gen3 retrain
  @retval FALSE           Do not delay
**/
BOOLEAN
IsDelayedGen3Retrain (
  IN CONST PCH_PCIE_CONFIG *PcieConfig,
  IN UINTN                 RpIndex,
  IN BOOLEAN               Gen3DeviceFound
  )
{
#ifndef FSP_FLAG
  UINT8         EqMethod;
  EFI_BOOT_MODE BootMode;
  UINTN         RpDevice;
  UINTN         RpFunction;
  UINTN         RpBase;

  PeiServicesGetBootMode (&BootMode);
  EqMethod = PcieConfig->RootPort[RpIndex].Gen3EqPh3Method;

  GetPchPcieRpDevFun (RpIndex, &RpDevice, &RpFunction);
  RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDevice, (UINT32) RpFunction);
  if ((EqMethod == PchPcieEqDefault || EqMethod == PchPcieEqSoftware) &&
      (BootMode == BOOT_ON_S3_RESUME) &&
      (GetMaxLinkSpeed (RpBase) >= 3) &&
      (Gen3DeviceFound == TRUE)) {
    return TRUE;
  }
#endif
  return FALSE;
}

/**
  Reload default RP PSF device number and function number.
  The PSF register doesn't got reset after system reset, which will result in mismatch between
  PSF register setting and PCIE PCR PCD register setting. Reset PSF regsiter in early phase
  to avoid cycle decoding confusing.
**/
VOID
PchRpReloadDefaultDevFunc (
  VOID
  )
{
  UINT16   Psf1RpFuncCfgBase;
  UINTN    PortIndex;

  PortIndex = 0;

  if (GetPchSeries () == PchLp) {
    Psf1RpFuncCfgBase = R_PCH_LP_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_SPA_D28_F0;
    //
    // Reload D28 registers
    //
    for (; PortIndex < 8; PortIndex++) {
      PchPcrWrite32 (PID_PSF1, Psf1RpFuncCfgBase + 4 * (UINT16)PortIndex, 0x000001C0 + 2 * PortIndex);
    }
    //
    // Reload D29 registers
    //
    for (; PortIndex < 12; PortIndex++) {
      PchPcrWrite32 (PID_PSF1, Psf1RpFuncCfgBase + 4 * (UINT16)PortIndex, 0x000001D0 + 2 * (PortIndex % 8));
    }
  } else {
    Psf1RpFuncCfgBase = R_PCH_H_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_SPA_D28_F0;
    //
    // Reload D28 registers
    //
    for (; PortIndex < 8; PortIndex++) {
      PchPcrWrite32 (PID_PSF1, Psf1RpFuncCfgBase + 4 * (UINT16)PortIndex, 0x000001C0 + 2 * PortIndex);
    }
    //
    // Reload D29 registers
    //
    for (; PortIndex < 16; PortIndex++) {
      PchPcrWrite32 (PID_PSF1, Psf1RpFuncCfgBase + 4 * (UINT16)PortIndex, 0x000001D0 + 2 * (PortIndex % 8));
    }
    //
    // Reload D27 registers
    //
    for (; PortIndex < 20; PortIndex++) {
      PchPcrWrite32 (PID_PSF1, Psf1RpFuncCfgBase + 4 * (UINT16)PortIndex, 0x000001B0 + 2 * (PortIndex % 8));
    }
  }
}

/**
  Retrieves state of the CLKREQ# pad.

  This function will change pad mode to GPIO!

  @param[in]  ClkreqIndex       CLKREQ# number

  @retuns State of the CLKREQ# pad (1 == high).
**/
BOOLEAN
PchPciGetClkreqState (
  IN     UINT32   ClkreqIndex
  )
{
  GPIO_CONFIG    PadConfig;
  GPIO_PAD       ClkreqPad;
  EFI_STATUS     Status;
  UINT32         Value;

  ClkreqPad = GpioGetClkreqPad (ClkreqIndex);

  ZeroMem (&PadConfig, sizeof (PadConfig));
  PadConfig.PadMode      = GpioPadModeGpio;
  PadConfig.Direction    = GpioDirIn;
  GpioSetPadConfig (ClkreqPad, &PadConfig);

  MicroSecondDelay (5);
  Status = GpioGetInputValue (ClkreqPad, &Value);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }
  return !!Value;
}

/**
  Detect whether CLKREQ# is supported by the platform and device.

  Assumes device presence. Device will pull CLKREQ# low until CPM is enabled.
  Test pad state to verify that the signal is correctly connected.
  This function will change pad mode to GPIO!

  @param[in] RootPortConfig      Root port configuration
  @param[in] DevicePresent       Determines whether there is a device on the port

  @return TRUE if supported, FALSE otherwise.
**/
BOOLEAN
PchPcieDetectClkreq (
  IN     CONST PCH_PCIE_ROOT_PORT_CONFIG   *RootPortConfig
  )
{
  if (!RootPortConfig->ClkReqSupported) {
    return FALSE;
  }
  if (RootPortConfig->ClkReqDetect &&
      (PchPciGetClkreqState (RootPortConfig->ClkReqNumber) == 1)) {
    DEBUG ((DEBUG_INFO, "CLKREQ%d is high, disabling power management.\n",
            RootPortConfig->ClkReqNumber));
    return FALSE;
  }
  return TRUE;
}

/**
  Disables the root port in PSF.

  @param[in] PortIndex          Root Port Number
**/
VOID
PchPsfDisableRootPort (
  IN     UINT32  PortIndex
  )
{
  if (GetPchSeries () == PchLp) {
    PchPcrAndThenOr32 (
      PID_PSF1,
      (UINT16) (R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE - (0x100 * PortIndex)) + R_PCH_PCR_PSFX_T1_SHDW_PCIEN,
      ~0u,
      B_PCH_PCR_PSFX_T1_SHDW_PCIEN_FUNDIS
    );
  } else {
    PchPcrAndThenOr32 (
      PID_PSF1,
      (UINT16) (R_PCH_H_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE - (0x100 * PortIndex)) + R_PCH_PCR_PSFX_T1_SHDW_PCIEN,
      ~0u,
      B_PCH_PCR_PSFX_T1_SHDW_PCIEN_FUNDIS
    );
  }
}

/**
  Checks if a given rootport owns its PCIE lane(s).
  In controllers configured as 1x4, 2x2 etc some ports have more than 1 lane, so other ports own none

  @param[in] RpIndex   Root Port Number
  @return TRUE         This port owns 1 or more lane
  @return FALSE        This port doesn't own any lane
**/
BOOLEAN
IsStandaloneRp (
  IN UINT8 RpIndex
  )
{
   switch (GetPcieControllerConfig (RpIndex/PCH_PCIE_CONTROLLER_PORTS)) {
    case Pcie4x1 :
      return TRUE;
    case Pcie1x2_2x1 :
      return ((RpIndex % 4) != 1);
    case Pcie2x2 :
      return ((RpIndex % 2) != 1);
    case Pcie1x4 :
      return ((RpIndex % 4) == 0);
    default:
      ASSERT(FALSE);
      return FALSE;
  }
}

/**
  Disables the root port. Depending on 2nd param, port's PCI config space may be left visible
  to prevent function swapping

  Use sideband access unless the port is still available.

  @param[in] PortIndex          Root Port Number
  @param[in] KeepPortVisible    Should the port' interface be left visible on PCI Bus0
**/
VOID
PchDisableRootPort (
  IN UINT8   RpIndex,
  IN BOOLEAN KeepPortVisible
  )
{
  UINTN       RpBase;
  UINT32      PchPwrmBase;
  UINT32      Data32;
  UINT32      LoopTime;
  UINT32      TargetState;
  Data32 = 0;

  DEBUG ((DEBUG_INFO, "PchDisableRootPort(%d) Start\n", RpIndex + 1));

  Data32 = 0;
  RpBase = PchPcieBase (RpIndex);

    ///
  /// If port is enabled and device is present disable the link.
  /// Make sure Slot Implemented is set so PDS is valid.
  ///
  if ((MmioRead16 (RpBase) != 0xFFFF) &&
      (MmioRead16 (RpBase + R_PCH_PCIE_XCAP) & B_PCIE_XCAP_SI) &&
      (MmioRead16 (RpBase + R_PCH_PCIE_SLSTS) & B_PCIE_SLSTS_PDS)) {
    //
    // Set Link Disable
    //
    DEBUG ((DEBUG_INFO, "Disabling the link.\n"));
    MmioOr16 ((RpBase + R_PCH_PCIE_LCTL), (UINT16) B_PCIE_LCTL_LD);
  } else {
    ///
    /// Otherwise if device is not present perform the following steps using sideband access:
    /// 1.  Set B0:Dxx:Fn:338h[26] = 1b
    /// 2.  Poll B0:Dxx:Fn:328h[31:24] until 0x1 with 50ms timeout
    /// 3.  Set B0:Dxx:Fn +408h[27] =1b
    ///
    DEBUG ((DEBUG_INFO, "Stopping the port.\n"));
    PchSbiRpPciAndThenOr32 (RpIndex, R_PCH_PCIE_PCIEALC, ~0u, (UINT32) B_PCH_PCIE_PCIEALC_BLKDQDA);
    //
    // For first lane of a port owned by PCIE, poll until DETRDY
    // For all others (remaining lanes of a x2/x4 port and for lanes not owned by PCIE), wait intil DETRDYECINP1CG
    //
    if (IsStandaloneRp (RpIndex) && (GetLaneOwner (HsioPcieLane (PchPcieLane (RpIndex,0))) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI)) {
      TargetState = V_PCH_PCIE_PCIESTS1_LTSMSTATE_DETRDY;
    } else {
      TargetState = V_PCH_PCIE_PCIESTS1_LTSMSTATE_DETRDYECINP1CG;
    }

    if (IsSimicsPlatform() == FALSE) {
    Data32 = 0;
    for (LoopTime = 0; LoopTime < 5000; LoopTime++) {
      PchSbiRpPciRead32 (RpIndex, R_PCH_PCIE_PCIESTS1, &Data32);
      if (((Data32 & B_PCH_PCIE_PCIESTS1_LTSMSTATE) >> N_PCH_PCIE_PCIESTS1_LTSMSTATE) == TargetState) {
        break;
      }
      MicroSecondDelay (10);
      }
    }
  }
  ///
  /// Set offset 408h[27] to 1b to disable squelch.
  ///
  PchSbiRpPciAndThenOr32 (RpIndex, R_PCH_PCIE_PHYCTL4, ~0u, (UINT32) B_PCH_PCIE_PHYCTL4_SQDIS);

  ///
  /// Make port disappear from PCI bus
  ///
  if (!KeepPortVisible) {
    DEBUG ((DEBUG_INFO, "Hiding the port\n"));
    ///
    /// PCIe RP IOSF Sideband register offset 0x00[19:16], depending on the port that is Function Disabled
    /// Access it by offset 0x02[4:0] to avoid RWO bit
    ///
    PchPcrAndThenOr8 (
      GetRpSbiPid (RpIndex),
      R_PCH_PCR_SPX_PCD + 0x02,
      0x0F,
      (UINT8) (1 << (RpIndex % 4))
    );
    ///
    /// Then disable the port in PSF
    ///
    PchPsfDisableRootPort (RpIndex);
  }
  ///
  /// Set PWRMBASE + 0x628 [21]~[2] = 1b to function disable PCIE port in PMC
  /// No need to clear this non-static PG bit while IP re-enabled since it's reset to 0 when PLTRST.
  ///
  PchPwrmBaseGet (&PchPwrmBase);
  Data32 = B_PCH_PWRM_NST_PG_FDIS_1_PCIE_A0_FDIS_PMC << RpIndex;
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_NST_PG_FDIS_1, Data32);

  DEBUG ((DEBUG_INFO, "PchDisableRootPort() End\n"));
}

/**
  This function creates Capability and Extended Capability List

  @param[in] PortIndex    Root Port Number
  @param[in] *PchPolicy   The PCH Policy PPI

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
InitCapabilityList (
  IN      UINT8                      PortIndex,
  IN      CONST PCH_POLICY_PPI       *PchPolicy
  )
{
  UINTN       RpDevice;
  UINTN       RpFunction;
  UINTN       RpBase;
  UINT32      Data32;
  UINT16      Data16;
  UINT8       Data8;
  BOOLEAN     Gen3Enabled;
  UINT8       RpLinkSpeed;
  UINT16      NextCap;

  RpDevice    = 0xFF;
  RpFunction  = 0xFF;

  GetPchPcieRpDevFun (PortIndex, &RpDevice, &RpFunction);
  RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, RpDevice, RpFunction);

  ///
  /// Build Capability linked list
  /// 1.  Read and write back to capability registers 34h, 41h, 81h and 91h using byte access.
  /// 2.  Program NSR, A4h[3] = 0b
  ///
  Data8 = MmioRead8 (RpBase + PCI_CAPBILITY_POINTER_OFFSET);
  MmioWrite8 (RpBase + PCI_CAPBILITY_POINTER_OFFSET, Data8);

  Data16 = MmioRead16 (RpBase + R_PCH_PCIE_CLIST);
  MmioWrite16 (RpBase + R_PCH_PCIE_CLIST, Data16);

  Data16 = MmioRead16 (RpBase + R_PCH_PCIE_MID);
  MmioWrite16 (RpBase + R_PCH_PCIE_MID, Data16);

  Data16 = MmioRead16 (RpBase + R_PCH_PCIE_SVCAP);
  MmioWrite16 (RpBase + R_PCH_PCIE_SVCAP, Data16);

  Data32 = MmioRead32 (RpBase + R_PCH_PCIE_PMCS);
  Data32 &= (UINT32)~(B_PCIE_PMCS_NSR);
  MmioWrite32 (RpBase + R_PCH_PCIE_PMCS, Data32);

  /*
  a. NEXT_CAP = 0h
  */
  NextCap     = V_PCIE_EXCAP_NCO_LISTEND;

  /*
  b. If the RP is GEN3 capable (by fuse and BIOS policy), enable Secondary PCI Express Extended Capability
    1. Set Next Capability Offset, Dxx:Fn +220h[31:20] = NEXT_CAP
    2. Set Capability Version, Dxx:Fn +220h[19:16] = 1h
    3. Set Capability ID, Dxx:Fn +220h[15:0] = 0019h
    4. NEXT_CAP = 220h
    ELSE, set Dxx:Fn +220h [31:0] = 0
  */

  Gen3Enabled = FALSE;
  //
  // Check PCIe Gen3 Fuse and Policy Bits
  //
  RpLinkSpeed   = MmioRead8 (RpBase + R_PCH_PCIE_LCAP) & B_PCIE_LCAP_MLS;
  if ((RpLinkSpeed == V_PCIE_LCAP_MLS_GEN3) &&
    ((PchPolicy->PcieConfig.RootPort[PortIndex].PcieSpeed == PchPcieAuto) ||
    (PchPolicy->PcieConfig.RootPort[PortIndex].PcieSpeed == PchPcieGen3)))
  {
    Gen3Enabled = TRUE;
  }
  Data32 = 0;
  if (Gen3Enabled) {
    Data32  = (V_PCH_PCIE_EX_SPEECH_CV << N_PCIE_EXCAP_CV) | V_PCIE_EX_SPE_CID;
    Data32 |= (NextCap << N_PCIE_EXCAP_NCO);
    NextCap = R_PCH_PCIE_EX_SPEECH;
  }
  MmioWrite32 (RpBase + R_PCH_PCIE_EX_SPEECH, Data32);

  /*
  c. If support L1 Sub-State
    1. Set Next Capability Offset, Dxx:Fn +200h[31:20] = NEXT_CAP
    2. Set Capability Version, Dxx:Fn +200h[19:16] = 1h
    3. Set Capability ID, Dxx:Fn +200h[15:0] = 001Eh
    4. Read and write back to Dxx:Fn +204h
    5. Refer to section 8.3 for other requirements (Not implemented here)
    6. NEXT_CAP = 200h
    ELSE, set Dxx:Fn +200h [31:0] = 0, and read and write back to Dxx:Fn +204h
  */

  Data32 = 0;
  if ((PchPolicy->PcieConfig.RootPort[PortIndex].ClkReqSupported) &&
      (PchPolicy->PcieConfig.RootPort[PortIndex].L1Substates != PchPcieL1SubstatesDisabled))
  {
    Data32  = (V_PCH_PCIE_EX_L1S_CV << N_PCIE_EXCAP_CV) | V_PCIE_EX_L1S_CID;
    Data32 |= (NextCap << N_PCIE_EXCAP_NCO);
    NextCap = R_PCH_PCIE_EX_L1SECH;
  }
  MmioWrite32 (RpBase + R_PCH_PCIE_EX_L1SECH, Data32);

  Data32 = MmioRead32 (RpBase + R_PCH_PCIE_EX_L1SCAP);
  if (PchPolicy->PcieConfig.RootPort[PortIndex].L1Substates == PchPcieL1SubstatesDisabled) {
    Data32 &= (UINT32)~(0x1F);
  } else if (PchPolicy->PcieConfig.RootPort[PortIndex].L1Substates == PchPcieL1SubstatesL1_1) {
    Data32 &= (UINT32)~(BIT0);
  } else if (PchPolicy->PcieConfig.RootPort[PortIndex].L1Substates == PchPcieL1SubstatesL1_2) {
    Data32 &= (UINT32)~(BIT1);
  }
  MmioWrite32 (RpBase + R_PCH_PCIE_EX_L1SCAP, Data32);

  /*
  d. If support ACS
    1. Set Next Capability Offset, Dxx:Fn +140h[31:20] = NEXT_CAP
    2. Set Capability Version, Dxx:Fn +140h[19:16] = 1h
    3. Set Capability ID, Dxx:Fn +140h[15:0] = 000Dh
    4. Read and write back to Dxx:Fn +144h
    5. NEXT_CAP = 140h
    ELSE, set Dxx:Fn +140h [31:0] = 0, and read and write back to Dxx:Fn +144h
  */

  Data32 = 0;
  if (PchPolicy->PcieConfig.RootPort[PortIndex].AcsEnabled == TRUE) {
    Data32 = (V_PCH_PCIE_EX_ACS_CV << N_PCIE_EXCAP_CV) | V_PCIE_EX_ACS_CID;
    Data32 |= (NextCap << N_PCIE_EXCAP_NCO);
    NextCap = R_PCH_PCIE_EX_ACSECH;
  }
  MmioWrite32 (RpBase + R_PCH_PCIE_EX_ACSECH, Data32);

  Data32 = MmioRead32 (RpBase + R_PCH_PCIE_EX_ACSCAPR);
  MmioWrite32 (RpBase + R_PCH_PCIE_EX_ACSCAPR, Data32);

  /*
  e. If support Advanced Error Reporting
    1. Set Next Capability Offset, Dxx:Fn +100h[31:20] = NEXT_CAP
    2. Set Capability Version, Dxx:Fn +100h[19:16] = 1h
    3. Set Capability ID, Dxx:Fn +100h[15:0] = 0001h
    ELSE
    1. Set Next Capability Offset, Dxx:Fn +100h[31:20] = NEXT_CAP
    2. Set Capability Version, Dxx:Fn +100h[19:16]  = 0h
    3. Set Capability ID, Dxx:Fn +100h[15:10]  = 0000h
  */

  Data32 = 0;
  if (PchPolicy->PcieConfig.RootPort[PortIndex].AdvancedErrorReporting) {
    Data32 = (V_PCH_PCIE_EX_AEC_CV << N_PCIE_EXCAP_CV) | V_PCIE_EX_AEC_CID;
  }
  Data32 |= (NextCap << N_PCIE_EXCAP_NCO);
  MmioWrite32 (RpBase + R_PCH_PCIE_EX_AECH, Data32);

  //
  // Mask Unexpected Completion uncorrectable error
  //
  MmioOr32 (RpBase + R_PCH_PCIE_EX_UEM, B_PCIE_EX_UEM_UC);

  return EFI_SUCCESS;
}

/**
  Initialize R/WO Registers that described in PCH BIOS Spec

  @param[in] PchPolicy            The PCH Policy PPI instance

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PciERWORegInit (
  IN  CONST PCH_POLICY_PPI              *PchPolicy
  )
{
  UINTN       PortIndex;
  UINTN       RpDevice;
  UINTN       RpFunction;
  UINTN       RpBase;
  UINT32      Data32;
  UINT16      Data16;
  PCH_SERIES  PchSeries;

  DEBUG ((DEBUG_INFO, "PciERWORegInit() Start\n"));

  if (PchIsDwrFlow()) {
    DEBUG ((DEBUG_INFO, "DWR: PciERWORegInit() End\n"));
    return EFI_SUCCESS;
  }

  PchSeries       = GetPchSeries ();

  ///
  /// System BIOS must read the register and write the same value back to the register
  /// before passing control to the operating system.
  /// Dev:Func/Type Register Offset   Register Name                                     Bits
  /// Dxx:F0-F7     034h              Capabilities Pointer                              7:0
  /// Dxx:F0-F7     040h              Capabilities List                                 15:8
  /// Dxx:F0-F7     080h              Message Signaled Interrupt Capability ID          15:8
  /// Dxx:F0-F7     090h              Subsystem Vendor Capability                       15:8
  /// Dxx:F0-F7     0A4h              PCI Power Management Control And Status           3
  /// Dxx:F0-F7     100h              Advanced Error Extended Reporting                 31:0
  /// Dxx:F0-F7     140h              ACS Extended Capability Header                    31:0
  /// Dxx:F0-F7     144h              ACS Capability Register                           3:0
  /// Dxx:F0-F7     200h              L1 Sub-States Extended Capability Header          31:0
  /// Dxx:F0-F7     204h              L1 Sub-States Capabilities                        23:19, 17:16, 15:8, 4:0
  /// Dxx:F0-F7     220h              Secondary PCI Express Extended Capability Header  31:0
  /// ---- Above are done in InitCapabilityList()
  /// Dev:Func/Type Register Offset   Register Name                                     Bits
  /// Dxx:F0-F7     042h              PCI Express Capabilities                          8
  /// Dxx:F0-F7     050h              Link Control                                      3
  /// ---- Above are done in PchInitRootPorts()
  /// Dev:Func/Type Register Offset   Register Name                                     Bits
  /// Dxx:F0-F7     064h              Device Capabilities 2                             19:18, 11
  /// ---- Above are done in PchInitSingleRootPort()
  /// Dev:Func/Type Register Offset   Register Name                                     Bits
  /// Dxx:F0-F7     404h              Latency Tolerance Reporting Override 2            2
  /// ---- Above are done in PchPciExpressHelpersLibrary.c PcieSetPm ()
  /// Dev:Func/Type Register Offset   Register Name                                     Bits
  /// Dxx:F0-F7     094h              Subsystem Vendor ID                               31:0
  /// ---- Above are done in PchInitPreMem.c PchProgramSvidSid ()
  /// Dev:Func/Type Register Offset   Register Name                                     Bits
  /// Dxx:F0~F7     044h              Device Capabilities                               2:0
  /// Dxx:F0-F7     04Ch              Link Capabilities                                 22, 17:15, 11:10
  /// Dxx:F0-F7     054h              Slot Capabilities                                 31:19, 16:5
  /// Dxx:F0-F7     0D8h              Miscellaneous Port Configuration                  23, 2
  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
    GetPchPcieRpDevFun (PortIndex, &RpDevice, &RpFunction);
    RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, RpDevice, RpFunction);

    if (MmioRead16 (RpBase + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {
      Data16 = MmioRead16 (RpBase + R_PCH_PCIE_DCAP);
      MmioWrite16 (RpBase + R_PCH_PCIE_DCAP, Data16);

      Data32 = MmioRead32 (RpBase + R_PCH_PCIE_LCAP);
      MmioWrite32 (RpBase + R_PCH_PCIE_LCAP, Data32);

      Data32 = MmioRead32 (RpBase + R_PCH_PCIE_SLCAP);
      MmioWrite32 (RpBase + R_PCH_PCIE_SLCAP, Data32);

      Data32 = MmioRead32 (RpBase + R_PCH_PCIE_MPC);
      Data32 |= B_PCH_PCIE_MPC_SRL;
      MmioWrite32 (RpBase + R_PCH_PCIE_MPC, Data32);
    }
  }

  DEBUG ((DEBUG_INFO, "PciERWORegInit() End\n"));

  return EFI_SUCCESS;
}

/**
  The function to change the root port speed based on policy

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval EFI_SUCCESS             Succeeds.
**/
EFI_STATUS
PchPcieRpSpeedChange (
  IN CONST PCH_POLICY_PPI           *PchPolicyPpi,
  IN BOOLEAN                        *Gen3DeviceFound
  )
{
  EFI_STATUS            Status;
  UINTN                 PortIndex;
  UINTN                 PchMaxPciePortNum;
  UINTN                 PciRootPortRegBase[PCH_MAX_PCIE_ROOT_PORTS];
  UINTN                 RpDev;
  UINTN                 RpFunc;
  UINTN                 LinkRetrainedBitmap;
  UINTN                 TimeoutCount;
  UINT32                MaxLinkSpeed;

  if (PchIsDwrFlow()) {
    DEBUG ((DEBUG_INFO, "DWR: PchPcieRpSpeedChange() End\n"));
    return EFI_SUCCESS;
  }
  PchMaxPciePortNum = GetPchMaxPciePortNum ();
  //
  // Since we are using the root port base many times, it is best to cache them.
  //
  for (PortIndex = 0; PortIndex < PchMaxPciePortNum; PortIndex++) {
    PciRootPortRegBase[PortIndex] = 0;
    Status = GetPchPcieRpDevFun (PortIndex, &RpDev, &RpFunc);
    if (EFI_ERROR (Status)) {
      ASSERT (FALSE);
      continue;
    }
    PciRootPortRegBase[PortIndex] = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDev, (UINT32) RpFunc);
  }

  ///
  /// PCH BIOS Spec Section 8.14 Additional PCI Express* Programming Steps
  /// NOTE: Detection of Non-Complaint PCI Express Devices
  ///
  LinkRetrainedBitmap = 0;
  for (PortIndex = 0; PortIndex < PchMaxPciePortNum; PortIndex++) {
    if (PciRootPortRegBase[PortIndex] == 0) {
      continue;
    }
    if (MmioRead16 (PciRootPortRegBase[PortIndex] + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      continue;
    }

    MaxLinkSpeed = GetMaxLinkSpeed (PciRootPortRegBase[PortIndex]);

    if ((MaxLinkSpeed > 1) && (!IsDelayedGen3Retrain (&PchPolicyPpi->PcieConfig, PortIndex, Gen3DeviceFound[PortIndex]))) {
      MmioAndThenOr16 (
        PciRootPortRegBase[PortIndex] + R_PCH_PCIE_LCTL2,
        (UINT16) ~B_PCIE_LCTL2_TLS,
        (UINT16) MaxLinkSpeed
        );
      if (MmioRead16 (PciRootPortRegBase[PortIndex] + R_PCH_PCIE_SLSTS) & B_PCIE_SLSTS_PDS) {
        //
        // Retrain the link if device is present
        //
        MmioOr16 (PciRootPortRegBase[PortIndex] + R_PCH_PCIE_LCTL, B_PCIE_LCTL_RL);
        LinkRetrainedBitmap |= (1u << PortIndex);
      }
    }
  }

  //
  // 15 ms timeout while checking for link active on retrained link
  //
  for (TimeoutCount = 0; ((LinkRetrainedBitmap != 0) && (TimeoutCount < 150)); TimeoutCount++) {
    //
    // Delay 100 us
    //
    MicroSecondDelay (100);
    //
    // Check for remaining root port which was link retrained
    //
    for (PortIndex = 0; PortIndex < PchMaxPciePortNum; PortIndex++) {
      if ((LinkRetrainedBitmap & (1u << PortIndex)) != 0) {
        //
        // If the link is active, clear the bitmap
        //
        if (MmioRead16 (PciRootPortRegBase[PortIndex] + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA) {
          LinkRetrainedBitmap &= ~(1u << PortIndex);
        }
      }
    }
  }

  //
  // If 15 ms has timeout, and some link are not active, train to gen1
  //
  if (LinkRetrainedBitmap != 0) {
    for (PortIndex = 0; PortIndex < PchMaxPciePortNum; PortIndex++) {
      if ((LinkRetrainedBitmap & (1u << PortIndex)) != 0) {
        //
        // Set TLS to gen1
        //
        MmioAndThenOr16 (PciRootPortRegBase[PortIndex] + R_PCH_PCIE_LCTL2,
                         (UINT16) ~(B_PCIE_LCTL2_TLS),
                         V_PCIE_LCTL2_TLS_GEN1);
        //
        // Retrain link
        //
        MmioOr16 (PciRootPortRegBase[PortIndex] + R_PCH_PCIE_LCTL, B_PCIE_LCTL_RL);
      }
    }

    //
    // Wait for retrain completion or timeout in 15ms. Do not expect failure as
    // port was detected and trained as Gen1 earlier
    //
    for (TimeoutCount = 0; ((LinkRetrainedBitmap != 0) && (TimeoutCount < 150)); TimeoutCount++) {
      //
      // Delay 100 us
      //
      MicroSecondDelay (100);
      //
      // Check for remaining root port which was link retrained
      //
      for (PortIndex = 0; PortIndex < PchMaxPciePortNum; PortIndex++) {
        if ((LinkRetrainedBitmap & (1u << PortIndex)) != 0) {
          //
          // If the link is active, clear the bitmap
          //
          if (MmioRead16 (PciRootPortRegBase[PortIndex] + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA) {
            LinkRetrainedBitmap &= ~(1u << PortIndex);
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Get information about the endpoint

  @param[in]  RpBase      Root port base address
  @param[in]  TempPciBus  Temporary bus number
  @param[out] DeviceInfo  Device information structure

  @raturn TRUE if device was found, FALSE otherwise
**/
BOOLEAN
GetDeviceInfo (
  IN  UINTN             RpBase,
  IN  UINT8             TempPciBus,
  OUT PCIE_DEVICE_INFO  *DeviceInfo
  )
{
  UINTN                   EpBase;
  UINT32                  Data32;
  UINT8                   EpPcieCapPtr;
  UINT8                   EpLinkSpeed;
  UINT16                  EpMaxLinkWidth = 0;

  DeviceInfo->MaxLinkWidth = 0;
  DeviceInfo->Vid = 0xFFFF;
  DeviceInfo->Did = 0xFFFF;
  DeviceInfo->MaxLinkSpeed = 0;

  //
  // Check for device presence
  //
  if ((MmioRead16 (RpBase + R_PCH_PCIE_SLSTS) & B_PCIE_SLSTS_PDS) == 0) {
    return FALSE;
  }

  //
  // Assign temporary bus numbers to the root port
  //
  MmioAndThenOr32 (
    RpBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET,
    (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN,
    ((UINT32) (TempPciBus << 8)) | ((UINT32) (TempPciBus << 16))
  );

  //
  // A config write is required in order for the device to re-capture the Bus number,
  // according to PCI Express Base Specification, 2.2.6.2
  // Write to a read-only register VendorID to not cause any side effects.
  //
  EpBase  = MmPciBase (TempPciBus, 0, 0);
  MmioWrite16 (EpBase + PCI_VENDOR_ID_OFFSET, 0);

  Data32 = MmioRead32 (EpBase + PCI_VENDOR_ID_OFFSET);
  DeviceInfo->Vid = (UINT16) (Data32 & 0xFFFF);
  DeviceInfo->Did = (UINT16) (Data32 >> 16);

  EpLinkSpeed = 0;
  if (Data32 != 0xFFFFFFFF) {
    EpPcieCapPtr = PcieFindCapId (TempPciBus, 0, 0, EFI_PCI_CAPABILITY_ID_PCIEXP);
    if (EpPcieCapPtr != 0) {
      EpLinkSpeed = MmioRead8 (EpBase + EpPcieCapPtr + R_PCIE_LCAP_OFFSET) & B_PCIE_LCAP_MLS;
      EpMaxLinkWidth = MmioRead16 (EpBase + EpPcieCapPtr + R_PCIE_LCAP_OFFSET) & B_PCIE_LCAP_MLW;
      DeviceInfo->MaxLinkWidth = (UINT8) (EpMaxLinkWidth >> N_PCIE_LCAP_MLW);
    }
    DeviceInfo->MaxLinkSpeed = EpLinkSpeed;
  }

  //
  // Clear bus numbers
  //
  MmioAnd32 (RpBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN);

  DEBUG ((DEBUG_INFO, "VID: %04X DID: %04X  MLS: %d MLW: %d\n",
          DeviceInfo->Vid, DeviceInfo->Did, DeviceInfo->MaxLinkSpeed, DeviceInfo->MaxLinkWidth));
  return (Data32 != 0xFFFFFFFF);
}


/**
  Get information about the endpoint

  @param none

  @raturn nothing
**/
VOID
DirtyWarmResetExecute (
 VOID
)
{
  PCH_RESET_PPI   *PchResetPpi;
  EFI_STATUS       Status;
  UINT32           BiosSspad7Address;
  UINT32           BiosSspad7Data;
  PCIE_DEVICE_INFO DeviceInfo;
  UINTN            RpBase;
  UINTN            EpBase;

  if (PchIsDwrFlow() == FALSE) {
    //
    // Not DWR flow - exit
    //
    return;
  }

  DEBUG ((DEBUG_ERROR, "\nDirtyWarmResetExecute() - Start\n"));

  RpBase = PchPcieBase(GetBmcRootPort());
  if (GetDeviceInfo(RpBase, DwrTempPciBus, &DeviceInfo) == FALSE) {
    DEBUG ((DEBUG_ERROR, "ERROR: DirtyWarmReset: can't get end point device available...\n"));
  } else {
    //
    // Leave BMC PCI device available
    //
    MmioAndThenOr32 (
      RpBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET,
      (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN,
      ((UINT32) (DwrTempPciBus << 8)) | ((UINT32) (DwrTempPciBus << 16))
    );
    EpBase  = MmPciBase (DwrTempPciBus, 0, 0);
    //
    // A config write is required in order for the device to re-capture the Bus number,
    // according to PCI Express Base Specification, 2.2.6.2
    // Write to a read-only register VendorID to not cause any side effects.
    //
    MmioWrite16 (EpBase + PCI_VENDOR_ID_OFFSET, 0);
  }

  BiosSspad7Address = MmPciAddress (
                        0,
                        DEFAULT_PCI_BUS_NUMBER_PCH,
                        UBOX_SCRATCH_DEVICE,
                        UBOX_SCRATCH_FUNC2,
                        UBOX_BIOS_SCRATCHPAD7
                      );

  //
  // s4929724 Add debug ability to stall in the DWR flow prior to the reset
  //
  BiosSspad7Data = MmioRead32(BiosSspad7Address);
  if ((BiosSspad7Data & BIT25) && (BiosSspad7Data != 0xFFFFFFFF)) {
    //
    // BIT25 == 1 => BIOS is requested to stall for DWR debug/validation
    // Set bit26 to indicate we are in stalled flow
    //
    BiosSspad7Data |= BIT26;
    MmioWrite32(BiosSspad7Address, BiosSspad7Data);
  }

  //
  // Signal DWR guid
  //
  DEBUG ((DEBUG_INFO, "DWR: Sending DirtyWarmReset Notification ...\n"));
  PeiServicesInstallPpi (&gDirtyWarmResetSignalPpi);
  DEBUG ((DEBUG_INFO, "DWR: DirtyWarmReset Notification completed\n"));

  BiosSspad7Data = MmioRead32(BiosSspad7Address);
  if ((BiosSspad7Data & BIT25) && (BiosSspad7Data != 0xFFFFFFFF)) {
    //
    // Stall until 3rd party agent clears BIT26 indicating ready to reset
    //
    DEBUG ((DEBUG_INFO, "\nDWR: Stalling in DWR flow to allow error collection.\n"));
    while (MmioRead32(BiosSspad7Address) & BIT26) {
    }
  }

  Status = PeiServicesLocatePpi (
             &gPchResetPpiGuid,
             0,
             NULL,
             (VOID **)&PchResetPpi
             );
  if (!EFI_ERROR(Status)) {
    PchResetPpi->Reset (PchResetPpi, GlobalResetWithEc);
  } else {
    DEBUG ((DEBUG_ERROR, "\nDWR: ERROR: Can't get reset PPI\n"));
    ASSERT_EFI_ERROR (Status);
  }
  DEBUG ((DEBUG_ERROR, "\nDWR: Waiting for reset.\n"));
  CpuDeadLoop ();
}

/**
  Returns the PCIe controller configuration (4x1, 1x2-2x1, 2x2, 1x4)

  @param[in] ControllerIndex        Number of PCIe controller (0 based)

  @retval PCIe controller configuration
**/
PCIE_CONTROLLER_CONFIG
GetPcieControllerConfig (
  IN     UINT32        ControllerIndex
  )
{
  EFI_STATUS              Status;
  UINT32                  Data32;
  PCIE_CONTROLLER_CONFIG  Config;
  UINTN                   DevNum;
  UINTN                   DummyFn;
  UINT32                  FirstRp;

  FirstRp = ControllerIndex * PCH_PCIE_CONTROLLER_PORTS;

  Status = GetPchPcieRpDevFun (FirstRp, &DevNum, &DummyFn);
  ASSERT_EFI_ERROR (Status);

  Status = PchSbiRpPciRead32 (FirstRp, R_PCH_PCIE_STRPFUSECFG, &Data32);
  ASSERT_EFI_ERROR (Status);

  Config = ((Data32 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC);
  DEBUG ((DEBUG_INFO, "PCIe SP%c is %a\n", (UINTN)('A' + ControllerIndex), mPcieControllerConfigName[Config]));
  return Config;
}

/**
  Program PSF grant counts for PCI express depending on controllers configuration
**/
VOID
PchConfigurePsfGrantCountsForPcie (
  VOID
  )
{
  static CONST UINT8 DgcrPairsLp [PCH_LP_PCIE_MAX_CONTROLLERS][PCH_PCIE_CONTROLLER_PORTS][2] = {
    { {  5,  6 }, {  8,  9 }, { 11, 12 }, { 14, 15 } }, //SPA
    { { 19, 20 }, { 22, 23 }, { 25, 26 }, { 28, 29 } }, //SPB
    { { 33, 34 }, { 36, 37 }, { 39, 40 }, { 42, 43 } }, //SPC
  };

  static CONST UINT8 PgTgtLp [PCH_LP_PCIE_MAX_CONTROLLERS][PCH_PCIE_CONTROLLER_PORTS] = {
    {  4,  5,  6,  7 }, //SPA
    { 12, 13, 14, 15 }, //SPB
    { 20, 21, 22, 23 }  //SPC
  };

  static CONST UINT8 DgcrPairsH [PCH_H_PCIE_MAX_CONTROLLERS][PCH_PCIE_CONTROLLER_PORTS][2] = {
    { { 11, 12 }, { 14, 15 }, { 17, 18 }, { 20, 21 } }, //SPA
    { { 24, 25 }, { 27, 28 }, { 30, 31 }, { 33, 34 } }, //SPB
    { { 37, 38 }, { 40, 41 }, { 43, 44 }, { 46, 47 } }, //SPC
    { { 50, 51 }, { 53, 54 }, { 56, 57 }, { 59, 60 } }, //SPD
    { { 63, 64 }, { 66, 67 }, { 69, 70 }, { 72, 73 } }, //SPE
  };

  static CONST UINT8 PgTgtH [PCH_H_PCIE_MAX_CONTROLLERS][PCH_PCIE_CONTROLLER_PORTS] = {
    {  0,  1,  2,  3 }, //SPA
    {  4,  5,  6,  7 }, //SPB
    { 12, 13, 14, 15 }, //SPC
    { 20, 21, 22, 23 }, //SPD
    { 28, 29, 30, 31 }  //SPE
  };
  static CONST UINT8 DefaultGrantCount = 4;

  PCH_SERIES  PchSeries;
  UINT32      Dgcr0Addr;
  UINT32      PgTgt0Addr;
  UINT32      DgcrNo0;
  UINT32      DgcrNo1;
  UINT32      PgTgtNo;
  UINT32      Controller;
  UINT32      Channel;
  UINT32      ChannelGrant[PCH_PCIE_CONTROLLER_PORTS];
  UINT32      ControllerMax;

  DEBUG ((DEBUG_INFO, "PchConfigurePsfGrantCountsForPcie() Start\n"));

  PchSeries = GetPchSeries ();
  if (PchSeries == PchLp) {
    ControllerMax = PCH_LP_PCIE_MAX_CONTROLLERS;
    Dgcr0Addr     = R_PCH_LP_PSF1_DEV_GNTCNT_RELOAD_DGCR0;
    PgTgt0Addr    = R_PCH_LP_PSF1_TARGET_GNTCNT_RELOAD_PG1_TGT0;
  } else {
    ControllerMax = PCH_H_PCIE_MAX_CONTROLLERS;
    Dgcr0Addr     = R_PCH_H_PSF1_DEV_GNTCNT_RELOAD_DGCR0;
    PgTgt0Addr    = R_PCH_H_PSF1_TARGET_GNTCNT_RELOAD_PG1_TGT0;
  }

  for (Controller = 0; Controller < ControllerMax; ++Controller) {
    switch (GetPcieControllerConfig (Controller)) {
      case Pcie4x1:
        ChannelGrant[0] = 1;
        ChannelGrant[1] = 1;
        ChannelGrant[2] = 1;
        ChannelGrant[3] = 1;
        break;
      case Pcie2x2:
        ChannelGrant[0] = 2;
        ChannelGrant[1] = DefaultGrantCount;
        ChannelGrant[2] = 2;
        ChannelGrant[3] = DefaultGrantCount;
        break;
      case Pcie1x2_2x1:
        ChannelGrant[0] = 2;
        ChannelGrant[1] = DefaultGrantCount;
        ChannelGrant[2] = 1;
        ChannelGrant[3] = 1;
        break;
      case Pcie1x4:
        ChannelGrant[0] = 4;
        ChannelGrant[1] = DefaultGrantCount;
        ChannelGrant[2] = DefaultGrantCount;
        ChannelGrant[3] = DefaultGrantCount;
        break;
      default:
        ASSERT (FALSE);
        continue;
    }

    for (Channel = 0; Channel < PCH_PCIE_CONTROLLER_PORTS; ++Channel) {
      if (PchSeries == PchLp) {
        DgcrNo0 = DgcrPairsLp[Controller][Channel][0];
        DgcrNo1 = DgcrPairsLp[Controller][Channel][1];
        PgTgtNo = PgTgtLp[Controller][Channel];
      } else {
        DgcrNo0 = DgcrPairsH[Controller][Channel][0];
        DgcrNo1 = DgcrPairsH[Controller][Channel][1];
        PgTgtNo = PgTgtH[Controller][Channel];
      }

      DEBUG ((DEBUG_INFO, "DGCR%d = %d\n", DgcrNo0, ChannelGrant[Channel]));
      PchPcrAndThenOr32 (
        PID_PSF1,
        (UINT16) (Dgcr0Addr  + (DgcrNo0 * S_PCH_PSF_DEV_GNTCNT_RELOAD_DGCR)),
        (UINT32) ~B_PCH_PSF_DEV_GNTCNT_RELOAD_DGCR_GNT_CNT_RELOAD,
        ChannelGrant[Channel]
        );
      DEBUG ((DEBUG_INFO, "DGCR%d = %d\n", DgcrNo1, ChannelGrant[Channel]));
      PchPcrAndThenOr32 (
        PID_PSF1,
        (UINT16) (Dgcr0Addr  + (DgcrNo1 * S_PCH_PSF_DEV_GNTCNT_RELOAD_DGCR)),
        (UINT32) ~B_PCH_PSF_DEV_GNTCNT_RELOAD_DGCR_GNT_CNT_RELOAD,
        ChannelGrant[Channel]
        );
      DEBUG ((DEBUG_INFO, "PG1_TGT%d = %d\n", PgTgtNo, ChannelGrant[Channel]));
      PchPcrAndThenOr32 (
        PID_PSF1,
        (UINT16) (PgTgt0Addr + (PgTgtNo * S_PCH_PSF_TARGET_GNTCNT_RELOAD)),
        (UINT32) ~B_PCH_PSF_TARGET_GNTCNT_RELOAD_GNT_CNT_RELOAD,
        ChannelGrant[Channel]
        );
    }
  }
  DEBUG ((DEBUG_INFO, "PchConfigurePsfGrantCountsForPcie() End\n"));
}


/**
  Program controller power management settings.
  This settings are relevant to all ports including disabled ports.
  All registers are located in the first port of the controller.
  Use sideband access since primary may not be available.

  @param[in]  RpIndex               The root port to be initialized (zero based).
  @param[in]  TrunkClockGateEn      Indicates whether trunk clock gating is to be enabled,
                                    requieres all controller ports to have dedicated CLKREQ#
                                    or to be disabled.
**/
VOID
PchPcieConfigureControllerBasePowerManagement (
  IN  UINT32   RpIndex,
  IN  BOOLEAN  TrunkClockGateEn
)
{
  UINT32      Data32And;
  UINT32      Data32Or;

  DEBUG ((DEBUG_INFO, "PchPcieConfigureControllerBasePowerManagement(%d)\n", RpIndex + 1));

  ASSERT ((RpIndex % PCH_PCIE_CONTROLLER_PORTS) == 0);

  ///
  /// Set E1h[7,5,4,2] to 1111b   (R_PCH_PCIE_RPDCGEN)
  /// Set E2h[4] to 1b            (R_PCH_PCIE_RPPGEN)
  /// Set E1h[6] to 1b if all ports on the controller support CLKREQ#   (R_PCH_PCIE_RPDCGEN)
  ///
  Data32Or  = (B_PCH_PCIE_RPDCGEN_RPSCGEN | B_PCH_PCIE_RPDCGEN_LCLKREQEN |
               B_PCH_PCIE_RPDCGEN_BBCLKREQEN | B_PCH_PCIE_RPDCGEN_SRDBCGEN) << 8;
  Data32Or |= B_PCH_PCIE_RPPGEN_SEOSCGE << 16;
  if (TrunkClockGateEn) {
    DEBUG ((DEBUG_INFO, "Setting PTOCGE\n"));
    Data32Or |= (B_PCH_PCIE_RPDCGEN_PTOCGE << 8);
  }
  PchSbiRpPciAndThenOr32 (RpIndex, 0xE0, ~0u, Data32Or);

  ///
  /// Set E8h[17,15] to [1,1]
  ///
  Data32Or = B_PCH_PCIE_PWRCTL_WPDMPGEP | B_PCH_PCIE_PWRCTL_DBUPI;
  PchSbiRpPciAndThenOr32 (RpIndex, R_PCH_PCIE_PWRCTL, ~0u, Data32Or);
  ///
  /// Set F5h[1:0] to 11b  (R_PCH_PCIE_PHYCTL2)
  /// Set F7h[3:2] = 00b   (R_PCH_PCIE_IOSFSBCS)
  ///
  Data32And = (UINT32) ~(B_PCH_PCIE_IOSFSBCS_SIID << 24);
  Data32Or = (B_PCH_PCIE_PHYCTL2_PXPG3PLLOFFEN | B_PCH_PCIE_PHYCTL2_PXPG2PLLOFFEN) << 8;
  PchSbiRpPciAndThenOr32 (RpIndex, 0xF4, Data32And, Data32Or);


  ///
  /// Set 428h[5] to 1b
  /// Set 428h[0] to 0b
  Data32And = (UINT32) ~B_PCH_PCIE_PCE_PMCRE;
  Data32Or = B_PCH_PCIE_PCE_HAE;
  PchSbiRpPciAndThenOr32 (RpIndex, R_PCH_PCIE_PCE, Data32And, Data32Or);
}


/**
  Configure power management settings whcih are applicable to both enabled and disabled ports.
  This settings are relevant to all ports including disabled ports.
  Use sideband access since primary may not be available.

  @param[in]  RpIndex               The root port to be initialized (zero based).
  @param[in]  PhyLanePgEnable       Indicates whether PHY lane power gating is to be enabled,
                                    requires CLKREQ# to supported by the port or the port to be disabled.
**/
VOID
PchPcieConfigurePortBasePowerManagement (
  IN  UINT32   RpIndex,
  IN  BOOLEAN  PhyLanePgEnable
)
{

  ///
  ///
  DEBUG ((DEBUG_INFO, "Skip PchPcieConfigureBasePowerManagement(%d)\n", RpIndex + 1));
}

/**
  Get PCIe root port index based on device ID for enabled port.
  @param[in] RpBase    Root Port base address
  @return Root Port index (0 based)
**/
UINT32
PciePortIndex (
  IN     UINTN  RpBase
  )
{
  UINT16 RpDid;
  RpDid = MmioRead16 (RpBase + PCI_DEVICE_ID_OFFSET);
  ASSERT (RpDid != 0xFFFF);
  if ((RpDid & 0x00F0) == 0x00E0) {
    return (RpDid & 0x00FF) - 0xD7;
  } else if ((RpDid & 0x00F0) == 0x0060) {
    return (RpDid & 0x00FF) - 0x57;
  } else {
    return (RpDid & 0x00F);
  }
}

/**
  Get PCIe port number for enabled port.
  @param[in] RpBase    Root Port base address
  @return Root Port number (1 based)
**/
UINT32
PciePortNum (
  IN     UINTN  RpBase
  )
{
  return 1 + PciePortIndex (RpBase);
}

/**
  Set Gen3 coefficient list entry.

  @param[in] RpBase      Root Port base address
  @param[in] ListEntry   ListEntry (0-9)
  @param[in] Cm          C-1
  @param[in] Cp          C+1
**/
VOID
PcieSetCoeffList (
  UINTN  RpBase,
  UINT32 ListEntry,
  UINT32 Cm,
  UINT32 Cp
  )
{
  UINT32  PreReg;
  UINT32  PostReg;
  UINT32  PreField;
  UINT32  PostField;
  UINT32  Data32And;
  UINT32  Data32Or;

  ASSERT (ListEntry < 10);
  ASSERT ((Cm & ~0x3F) == 0);
  ASSERT ((Cp & ~0x3F) == 0);

  PreReg    = ((ListEntry * 2))     / 5;
  PreField  = ((ListEntry * 2))     % 5;
  PostReg   = ((ListEntry * 2) + 1) / 5;
  PostField = ((ListEntry * 2) + 1) % 5;

  ASSERT (PreReg  < 4);
  ASSERT (PostReg < 4);

  Data32And = (UINT32) ~(0x3F << (6 * PreField));
  Data32Or  = (Cm << (6 * PreField));
  ASSERT ((Data32And & Data32Or) == 0);
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_RTPCL1 + (PreReg  * 4), Data32And, Data32Or);

  Data32And = (UINT32) ~(0x3F << (6 * PostField));
  Data32Or  = (Cp << (6 * PostField));
  ASSERT ((Data32And & Data32Or) == 0);
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_RTPCL1 + (PostReg * 4), Data32And, Data32Or);

  DEBUG ((DEBUG_INFO, "Port %d list %d: (%d,%d)\n",
          PciePortNum (RpBase), ListEntry, Cm, Cp));
}

/**
  Gen3 Remote transmitter coefficient override

  @param[in] RpBase    Root Port base address
  @param[in] RpLane    Root Port Lane (0-3)
  @param[in] Cm        C-1
  @param[in] Cp        C+1
**/
STATIC
VOID
PcieSetCoefficients (
  UINTN  RpBase,
  UINT32 RpLane,
  UINT32 Cm,
  UINT32 Cp
  )
{
  UINT32  Mask[2];
  UINT32  Data[2];

  static UINT8 CmShift[] = { 0, 12, 24,  6 };
  static UINT8 CmReg  [] = { 0,  0,  0,  1 };
  static UINT8 CpShift[] = { 6, 18,  0, 12 };
  static UINT8 CpReg  [] = { 0,  0,  1,  1 };

  ASSERT (RpLane < 4);
  ASSERT ((Cm & ~0x3F) == 0);
  ASSERT ((Cp & ~0x3F) == 0);

  Mask[0] = 0;
  Mask[1] = 0;
  Data[0] = 0;
  Data[1] = 0;

  Mask[CmReg[RpLane]] |= 0x3Fu << CmShift[RpLane];
  Data[CmReg[RpLane]] |= Cm    << CmShift[RpLane];
  Mask[CpReg[RpLane]] |= 0x3Fu << CpShift[RpLane];
  Data[CpReg[RpLane]] |= Cp    << CpShift[RpLane];

  DEBUG ((DEBUG_INFO, "Port %d.%d use coefficients (%d, %d)\n",
          PciePortNum (RpBase), RpLane, Cm, Cp));

  MmioAndThenOr32 (RpBase + R_PCH_PCIE_RTPCL1, ~Mask[0], Data[0] | B_PCH_PCIE_RTPCL1_PCM);
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_RTPCL2, ~Mask[1], Data[1]);
  //
  // Remote Transmitter Preset Coefficient Override Enable
  //
  MmioOr32 (RpBase + R_PCH_PCIE_EQCFG1, B_PCH_PCIE_EQCFG1_RTPCOE);
}


/**
  Gen3 Remote transmitter preset override

  @param[in] RpBase    Root Port base address
  @param[in] RpLane    Root Port Lane (0-3)
  @param[in] Preset    Preset to use
**/
STATIC
VOID
PcieSetPreset (
  UINTN  RpBase,
  UINT32 RpLane,
  UINT32 Preset
  )
{
  UINT32 Mask32;
  UINT32 Data32;

  ASSERT (RpLane < 4);
  ASSERT ((Preset & ~0x3Fu) == 0);
  Mask32 = ~(0x3Fu << (RpLane * 6));
  Mask32 &= (UINT32) ~B_PCH_PCIE_RTPCL1_PCM;
  Data32 = Preset << (RpLane * 6);
  DEBUG ((DEBUG_INFO, "Port %d.%d use preset %d\n", PciePortNum (RpBase), RpLane, Preset));
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_RTPCL1, Mask32, Data32);
  //
  // Remote Transmitter Preset Coefficient Override Enable
  //
  MmioOr32 (RpBase + R_PCH_PCIE_EQCFG1, B_PCH_PCIE_EQCFG1_RTPCOE);
}

/**
  Gen3 Set Upstream Port Transmitter Preset

  @param[in] RpBase    Root Port base address
  @param[in] RpLane    Root Port Lane (0-3)
  @param[in] Preset    Preset to use
**/
STATIC
VOID
PcieSetUptp (
  UINTN  RpBase,
  UINT32 RpLane,
  UINT8  Preset
  )
{
  ASSERT (RpLane < 4);
  ASSERT (Preset < 11);
  DEBUG ((DEBUG_INFO, "Port %d.%d use UPTP %d\n", PciePortNum (RpBase), RpLane, Preset));
  MmioAndThenOr16 (RpBase + R_PCH_PCIE_EX_LECTL + (RpLane * 2), (UINT16) ~B_PCH_PCIE_EX_LECTL_UPTP, Preset << N_PCH_PCIE_EX_LECTL_UPTP);
}

/**
  Clear correctable error statuses.

  @param[in] RpBase    Root Port base address
**/
STATIC
VOID
ClearCorrectableErrors (
  UINTN RpBase
  )
{
  MmioWrite32 (RpBase + R_PCH_PCIE_EX_CES, B_PCH_PCIE_EX_CES_BD |
                                           B_PCH_PCIE_EX_CES_BT |
                                           B_PCH_PCIE_EX_CES_RE);
  MmioWrite16 (RpBase + R_PCH_PCIE_DSTS, B_PCIE_DSTS_CED);
  MmioWrite32 (RpBase + R_PCH_PCIE_EX_LES, 0xFFFFFFFF);
}

/**
  Reset and enable Recovery Entry and Idle Framing Error Count

  @param[in] RpBase    Root Port base address
**/
STATIC
VOID
ResetErrorCounts (
  UINTN RpBase
  )
{
  MmioAnd32 (RpBase + R_PCH_PCIE_EQCFG1, (UINT32) ~B_PCH_PCIE_EQCFG1_REIFECE);
  MmioOr32 (RpBase + R_PCH_PCIE_EQCFG1, B_PCH_PCIE_EQCFG1_REIFECE);
}

/**
  Get recovery error count

  @param[in] RpBase    Root Port base address
  @return Recovery Error Count
**/
STATIC
UINT32
GetRecoveryErrorCount (
  UINTN RpBase
  )
{
  return (MmioRead32 (RpBase + R_PCH_PCIE_EQCFG1) & B_PCH_PCIE_EQCFG1_REC) >> N_PCH_PCIE_EQCFG1_REC;
}

/**
  Get idle framing error count for given lane

  @param[in] RpBase    Root Port base address
  @param[in] RpLane    Root Port Lane (0-3)
  @return Recovery Error Count
**/
STATIC
UINT32
GetIdleFramingErrorCount (
  UINTN  RpBase,
  UINT32 RpLane
  )
{
  MmioWrite32 (RpBase + R_PCH_PCIE_MM, RpLane);
  return MmioRead32 (RpBase + R_PCH_PCIE_MM) >> N_PCH_PCIE_MM_MSST;
}

/**
  Get current PCIe link state.

  @param[in] RpBase    Root Port base address
  @return Link state
**/
STATIC
UINT32
GetLinkState (
  UINTN  RpBase
  )
{
  return MmioRead32 (RpBase + R_PCH_PCIE_PCIESTS1) >> N_PCH_PCIE_PCIESTS1_LTSMSTATE;
}

// Use MDEPKG_NDEBUG flag to W/A the complain of GCC compiler that IsLinkActive is not used.
#ifndef MDEPKG_NDEBUG
/**
  Determines whether PCIe link is active

  @param[in] RpBase    Root Port base address
  @return Link Active state
**/
STATIC
BOOLEAN
IsLinkActive (
  UINTN  RpBase
  )
{
  return !!(MmioRead16 (RpBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA);
}
#endif

/**
  Get negotiated link width.

  @param[in] RpBase    Root Port base address
  @return Negotiated link width
**/
STATIC
UINT8
GetNegotiatedLinkWidth (
  UINTN  RpBase
  )
{
  UINT8  LinkWidth;
  LinkWidth = (UINT8) ((MmioRead16 (RpBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_NLW) >> N_PCIE_LSTS_NLW);
  ASSERT (LinkWidth <= 4);
  if (LinkWidth > 4) LinkWidth = 4;
  return LinkWidth;
}

/**
  Get max link width.

  @param[in] RpBase    Root Port base address
  @return Max link width
**/
STATIC
UINT8
GetMaxLinkWidth (
  UINTN  RpBase
  )
{
  UINT8  LinkWidth;
  LinkWidth = (UINT8) ((MmioRead32 (RpBase + R_PCH_PCIE_LCAP) & B_PCIE_LCAP_MLW) >> N_PCIE_LCAP_MLW);
  ASSERT (LinkWidth <= 4);
  if (LinkWidth > 4) LinkWidth = 4;
  return LinkWidth;
}

/**
  Translate PCIe Port/Lane pair to 0-based PCIe lane number.

  @param[in] RpIndex    Root Port index
  @param[in] RpLane     Root Port Lane (0-3)

  @return PCIe lane number (0-based)
**/
UINT32
PchPcieLane (
  UINT32 RpIndex,
  UINT32 RpLane
  )
{
  UINT32  ControllerIndex;
  UINT32  ControllerLane;

  ASSERT (RpIndex < GetPchMaxPciePortNum ());
  ASSERT (((RpIndex % 4) + RpLane) < 4);

  ControllerIndex = (RpIndex / 4);
  ControllerLane  = (RpIndex % 4) + RpLane;
  if (IsPcieLaneReversalEnabled (RpIndex)) {
    ControllerLane = 3 - ControllerLane;
  }
  return (ControllerIndex * 4) + ControllerLane;
}

/**
  Reset best margin data

  @param[in]  Eq       Equaliztion context structure
**/
VOID
SwEqResetBestMargin (
  IN OUT EQ_CONTEXT    *Eq
  )
{
  UINT32 RpLane;
  for (RpLane = 0; RpLane < Eq->MaxLinkWidth; ++RpLane) {
    Eq->Lane[RpLane].BestIndex  = 0;
    Eq->Lane[RpLane].BestMargin = 0;
  }
}

/**
  Perform margining on the link using jitter insertion module.

  @param[in]  Eq       Equaliztion context structure
  @param[in]  Params   Software Equalization parameters
  @param[out] Margin   Margins for each lane

  @retval EFI_SUCCESS        Margining completed
  @retval EFI_ABORTED        Link no stable, margining interrupted
  @retval EFI_DEVICE_ERROR   Margininig stepped due to recovery threshold
**/
EFI_STATUS
SwEqDoMargin (
  CONST EQ_CONTEXT    *Eq,
  CONST SW_EQ_PARAMS  *Params,
  OUT   UINT32        Margin[4]
  )
{
  EFI_STATUS Status;
  UINT16  Step;
  UINT32  RpLane;
  UINTN   RpBase;
  UINT32  ErrCount[4];
  UINT32  ErrTab[HSIO_JIM_STEP_MAX][4];
  UINT32  RecErrTab[HSIO_JIM_STEP_MAX];
  UINT32  LaneMask;
  UINT32  Nlw;
  UINT32  Cls;
  UINT32  Rec;

  UINT32  Idx;


  RpBase = Eq->RpBase;

  ResetErrorCounts (RpBase);
  MicroSecondDelay (5);
  Rec = GetRecoveryErrorCount (RpBase);
  if (Rec > 0) {
    DEBUG ((DEBUG_INFO, "Link recovery, bad coefficients/preset. REC=%d\n", Rec));
    return EFI_ABORTED;
  }

  ZeroMem (ErrCount, sizeof (ErrCount));
  ZeroMem (ErrTab, sizeof (ErrTab));
  ZeroMem (RecErrTab, sizeof (RecErrTab));

  Status = EFI_SUCCESS;
  Cls = 3;
  Nlw = GetNegotiatedLinkWidth (RpBase);
  LaneMask = 0;
  for (RpLane = 0; RpLane < Nlw; ++RpLane) {
    LaneMask |= (1 << RpLane);
    Margin[RpLane] = HSIO_JIM_STEP_MAX;  // margin is max. if no error found
  }

  for (Step = 0; Step < HSIO_JIM_STEP_MAX; ++Step) {
    for (RpLane = 0; RpLane < Nlw; ++RpLane) {
      if ((LaneMask & (1u << RpLane)) == 0) continue;
      HsioLaneSetJimClp (&Eq->Lane[RpLane].HsioLane, Step, Params->RecoveryWait);
      HsioLaneFreeze (&Eq->Lane[RpLane].HsioLane, TRUE, Eq->Ctoc);
    }
    for (RpLane = 0; RpLane < Nlw; ++RpLane) {
      if ((LaneMask & (1u << RpLane)) == 0) continue;
      HsioLaneEnableJim (&Eq->Lane[RpLane].HsioLane, TRUE);
    }
    ResetErrorCounts (RpBase);

    MicroSecondDelay (Params->DwellTimeUs);

    for (RpLane = 0; RpLane < Nlw; ++RpLane) {
      if ((LaneMask & (1u << RpLane)) == 0) continue;
      HsioLaneEnableJim (&Eq->Lane[RpLane].HsioLane, FALSE);
    }
    Cls = GetLinkSpeed (RpBase);
    Rec = GetRecoveryErrorCount (RpBase);
    RecErrTab[Step] = Rec;
    for (RpLane = 0; RpLane < Nlw; ++RpLane) {
      if ((LaneMask & (1u << RpLane)) == 0) continue;
      ErrCount[RpLane] = GetIdleFramingErrorCount (RpBase, RpLane);
      ErrTab[Step][RpLane] = ErrCount[RpLane];
    }
    for (RpLane = 0; RpLane < Nlw; ++RpLane) {
      if ((LaneMask & (1u << RpLane)) == 0) continue;
      HsioLaneFreeze (&Eq->Lane[RpLane].HsioLane, FALSE, Eq->Ctoc);
    }

    if ((Cls != 3) || (Rec > Params->RecThreshold)) {
      Status = (Cls != 3) ? EFI_ABORTED : EFI_DEVICE_ERROR;
      LaneMask = 0;
      for (RpLane = 0; RpLane < Nlw; ++RpLane) {
        Margin[RpLane] = (Step > 0) ? (Step - 1) : 0;
      }
    } else {
      for (RpLane = 0; RpLane < Nlw; ++RpLane) {
        if ((LaneMask & (1u << RpLane)) == 0) continue;
        if (ErrCount[RpLane] >= Params->ErrorThreshold) {
          LaneMask &= ~(1u << RpLane);
          Margin[RpLane] = Step;
        }
      }
    }
    if (LaneMask == 0) break;
  }

DEBUG_CODE_BEGIN();
  DEBUG ((DEBUG_INFO, "# ", RpLane));
  for (Idx = 0; Idx < HSIO_JIM_STEP_MAX; ++Idx) {
    DEBUG ((DEBUG_INFO, " %3d", Idx));
  }
  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    DEBUG ((DEBUG_INFO, "\n%d:", RpLane));
    for (Idx = 0; Idx < HSIO_JIM_STEP_MAX; ++Idx) {
      DEBUG ((DEBUG_INFO, " %3d", ErrTab[Idx][RpLane]));
    }
  }
  DEBUG ((DEBUG_INFO, "\nR ", RpLane));
  for (Idx = 0; Idx < HSIO_JIM_STEP_MAX; ++Idx) {
    DEBUG ((DEBUG_INFO, " %3d", RecErrTab[Idx]));
  }
  DEBUG ((DEBUG_INFO, "\n"));

  if (Cls != 3) {
    DEBUG ((DEBUG_INFO, "Step %d: Downtrain CLS=%d!\n", Step, Cls));
  }
  if (Rec > Params->RecThreshold) {
    DEBUG ((DEBUG_INFO, "Step %d: REC = %d. (threshold=%d!)\n", Step, Rec, Params->RecThreshold));
  }

  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    DEBUG ((DEBUG_INFO, "Lane %d.%d margin=%02d\n",
            Eq->RpIndex + 1,
            RpLane,
            Margin[RpLane]));
  }
  DEBUG ((DEBUG_INFO, "SwEqDoMargin %r\n", Status));
DEBUG_CODE_END();
  return Status;
}

/**
  Update best margin for each lane in equalization context.

  @param[in]  Eq       Equaliztion context structure
  @param[in]  Params   Software Equalization parameters
  @param[out] Margin   Margins for each lane
**/
VOID
SwEqUpdateBestMargin (
  IN OUT EQ_CONTEXT    *Eq,
  IN     CONST UINT32  Margin[4],
  IN     UINT32        Index
  )
{
  UINT32 RpLane;

  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    if (Margin[RpLane] > Eq->Lane[RpLane].BestMargin) {
      Eq->Lane[RpLane].BestMargin = Margin[RpLane];
      Eq->Lane[RpLane].BestIndex  = Index;
    }
  }
}

/**
  Set coefficient during Software Equaliztion flow

  @param[in] Eq       Equaliztion context structure
  @param[in] Params   Software Equalization parameters
  @param[in] Index    Coefficient index
**/
VOID
SwEqSetCoefficients (
  CONST EQ_CONTEXT    *Eq,
  CONST SW_EQ_PARAMS  *Params,
  UINT32               Index
  )
{
  UINT32 RpLane;
  ASSERT (Index < PCH_PCIE_SWEQ_COEFFS_MAX);
  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    PcieSetCoefficients (
      Eq->RpBase,
      RpLane,
      Params->CoeffList[Index].Cm,
      Params->CoeffList[Index].Cp
      );
  }
}

/**
  Set Upstream Port Transmitter Preset.

  @param[in] Eq       Equaliztion context structure
  @param[in] Params   Software Equalization parameters
  @param[in] Index    Coefficient index
**/
VOID
SwEqSetUptp (
  CONST EQ_CONTEXT    *Eq,
  CONST SW_EQ_PARAMS  *Params,
  UINT32              Index
  )
{
  UINT32 RpLane;
  ASSERT (Index < 5);
  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    PcieSetUptp (Eq->RpBase, RpLane, Params->PresetList[Index]);
  }
}

/**
  Perform link retrain.

  @param[in] RpBase            Root Port base address
  @param[in] Speed             Target speed
  @param[in] RetrainTimeoutMs  Timeout value

  @return TRUE if success
**/
BOOLEAN
SwEqRetrain (
  UINTN       RpBase,
  UINT16      Speed,
  UINT32      RetrainTimeoutMs
  )
{
  UINT32  LinkState;
  UINT32  LinkSpeed;
  UINT32  Timeout;
  UINT32  TimeoutMax;

  DEBUG ((DEBUG_INFO, "Retrain to Gen%d\n", Speed));

  MmioAndThenOr16 (RpBase + R_PCH_PCIE_LCTL2, (UINT16) ~B_PCIE_LCTL2_TLS, Speed);

  //
  // Poll for link in L0
  //
  TimeoutMax = RetrainTimeoutMs * 100;
  Timeout    = 0;
  while (GetLinkState (RpBase) != V_PCH_PCIE_PCIESTS1_LTSMSTATE_L0) {
    MicroSecondDelay (10);
    ++Timeout;
    if (Timeout > TimeoutMax) {
      DEBUG ((DEBUG_INFO, "Link not in L0, cannot re-train!\n"));
      return FALSE;
    }
  }
  //
  // Hit Link Retrain
  //
  if (Speed > 2) {
    MmioOr32 (RpBase + R_PCH_PCIE_EX_LCTL3, B_PCIE_EX_LCTL3_PE);
  }
  MmioOr32 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_RL);

  MmioWrite32 (RpBase + R_PCH_PCIE_PCIESTS2, B_PCH_PCIE_PCIESTS2_CLRE); // Clear CLRE

  // suppress potentially uninitialized warning
  LinkState  = 0;
  LinkSpeed  = 0;

  TimeoutMax = RetrainTimeoutMs * 100;
  for (Timeout = 0; Timeout < TimeoutMax; ++Timeout) {
    LinkState  = GetLinkState (RpBase);
    LinkSpeed  = GetLinkSpeed (RpBase);

    if ((LinkState == V_PCH_PCIE_PCIESTS1_LTSMSTATE_L0) &&
        (LinkSpeed == Speed)) {
      DEBUG ((DEBUG_INFO, "Link in Gen%d\n", Speed));
      return TRUE;
    }
    MicroSecondDelay (10);
  }

// Use MDEPKG_NDEBUG flag to W/A the complain of GCC compiler that IsLinkActive is not used.
DEBUG_CODE_BEGIN();
  DEBUG ((DEBUG_INFO, "Failed to train the link to Gen%d (LTSM=0x%02x, CLS=%d, LA=%d, NLW=%d)\n",
          Speed, GetLinkState (RpBase), GetLinkSpeed (RpBase), IsLinkActive (RpBase), GetNegotiatedLinkWidth (RpBase)));
DEBUG_CODE_END();
  return FALSE;
}

/**
  Reset the device by asserting PERST# pin passed by policy.

  @param[in] Eq       Equaliztion context structure

  @retval TRUE        Reset complete
  @retval FALSE       No pin assigned
**/
BOOLEAN
SwEqDeviceReset (
  CONST EQ_CONTEXT   *Eq
  )
{
  UINT32  LinkState;
  UINTN RpBase;

  RpBase = Eq->RpBase;

  if (Eq->RpConfig->DeviceResetPad == 0) {
    return FALSE;
  }

  DEBUG ((DEBUG_INFO, "Resetting the device...\n"));
  MmioOr16 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_LD);
  do {
    LinkState = GetLinkState (RpBase);
  } while ((LinkState != V_PCH_PCIE_PCIESTS1_LTSMSTATE_DISWAIT) &&
           (LinkState != V_PCH_PCIE_PCIESTS1_LTSMSTATE_DISWAITPG));

  MmioOr32 (RpBase + R_PCH_PCIE_PCIEALC, B_PCH_PCIE_PCIEALC_BLKDQDA);
  MmioAnd16 (RpBase + R_PCH_PCIE_LCTL, (UINT16) ~B_PCIE_LCTL_LD);
  do {
    LinkState = GetLinkState (RpBase);
  } while (LinkState != V_PCH_PCIE_PCIESTS1_LTSMSTATE_DETRDY);

  DEBUG ((DEBUG_INFO, "Asserting PERST#\n"));
  GpioSetOutputValue (Eq->RpConfig->DeviceResetPad, Eq->RpConfig->DeviceResetPadActiveHigh);
  MicroSecondDelay (1000);

  DEBUG ((DEBUG_INFO, "De-asserting PERST#\n"));
  GpioSetOutputValue (Eq->RpConfig->DeviceResetPad, !Eq->RpConfig->DeviceResetPadActiveHigh);

  MmioAnd32 (RpBase + R_PCH_PCIE_PCIEALC, (UINT32) ~B_PCH_PCIE_PCIEALC_BLKDQDA);
  return TRUE;
}

/**
  Try to train link to desired speed. Reset the device if training fails.

  @param[in]  Eq                Equaliztion context structure
  @param[in]  Speed             Target speed
  @param[in]  RetrainTimeoutMs  Timeout value
  @param[out] DoneReset         Optional. TRUE if reset was tirggered, FALSE otherwise

  @return TRUE if success
**/
BOOLEAN
SwEqRetrainReset (
  CONST EQ_CONTEXT   *Eq,
  UINT16             Speed,
  UINT32             RetrainTimeoutMs,
  BOOLEAN            *DoneReset
  )
{
  UINTN RpBase;

  RpBase     = Eq->RpBase;
  if (DoneReset != NULL) {
    *DoneReset = FALSE;
  }
  if (SwEqRetrain (RpBase, Speed, RetrainTimeoutMs)) {
    return TRUE;
  } else {
    if (SwEqDeviceReset (Eq) == FALSE) {
      return FALSE;
    }
    if (DoneReset != NULL) {
      *DoneReset = TRUE;
    }
    return SwEqRetrain (RpBase, Speed, RetrainTimeoutMs);
  }
}

/**
  Initialize SW EQ parameters.

  @param[out] Params   Software Equalization parameters
**/
VOID
SwEqParamsInit (
  SW_EQ_PARAMS *Params
  )
{
  ZeroMem (Params, sizeof (SW_EQ_PARAMS));

  Params->PresetList[0] = 6;
  Params->PresetList[1] = 5;
  Params->PresetList[2] = 9;
  Params->PresetList[3] = 4;
  Params->PresetList[4] = 8;

  Params->DwellTimeUs       = 500;
  Params->ErrorThreshold    = 4;
  Params->RecThreshold      = 40;
  Params->RetrainTimeoutMs  = 50;
  Params->RecoveryWait      = 120;
}

VOID
SwEqParamsUpdate (
  SW_EQ_PARAMS            *Params,
  CONST PCH_PCIE_CONFIG2  *PcieConfig2
  )
{
  UINT32 Index;
  for (Index = 0; Index < PCH_PCIE_SWEQ_COEFFS_MAX; ++Index) {
    Params->CoeffList[Index].Cm = PcieConfig2->SwEqCoeffList[Index].Cm;
    Params->CoeffList[Index].Cp = PcieConfig2->SwEqCoeffList[Index].Cp;
  }
}

/**
  Dump SW EQ parameters.

  @param[out] Params   Software Equalization parameters
**/
VOID
SwEqParamsDump (
  CONST SW_EQ_PARAMS *Params
  )
{
DEBUG_CODE_BEGIN();
  UINT32 Index;
  DEBUG ((DEBUG_INFO, "SW EQ Parameters:\n"));

  DEBUG ((DEBUG_INFO, "  DwellTime         = %dus\n", Params->DwellTimeUs));
  DEBUG ((DEBUG_INFO, "  ErrorThreshold    = %d\n",   Params->ErrorThreshold));
  DEBUG ((DEBUG_INFO, "  RecThreshold      = %d\n",   Params->RecThreshold));
  DEBUG ((DEBUG_INFO, "  RetrainTimeoutMs  = %d\n",   Params->RetrainTimeoutMs));
  DEBUG ((DEBUG_INFO, "  RecoveryWait      = %d\n",   Params->RecoveryWait));

  DEBUG ((DEBUG_INFO, "  Coefficient list: "));
  for (Index = 0; Index < PCH_PCIE_SWEQ_COEFFS_MAX; ++Index) {
    DEBUG ((DEBUG_INFO, "%d:(%d,%d) ", Index, Params->CoeffList[Index].Cm, Params->CoeffList[Index].Cp));
  }
  DEBUG ((DEBUG_INFO, "\n  Preset list: "));
  for (Index = 0; Index < 5; ++Index) {
    DEBUG ((DEBUG_INFO, "%d:%d ", Index, Params->PresetList[Index]));
  }

  DEBUG ((DEBUG_INFO, "\n"));
DEBUG_CODE_END();
}

/**
  Dump best margins

  @param[in] Eq       Equaliztion context structure
**/
VOID
SwEqDumpBestMargin (
  CONST EQ_CONTEXT    *Eq
  )
{
DEBUG_CODE_BEGIN();
  UINT32 RpLane;
  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    DEBUG ((DEBUG_INFO, "  Lane %d\n", RpLane));
    DEBUG ((DEBUG_INFO, "    BestIndex  %d\n", Eq->Lane[RpLane].BestIndex));
    DEBUG ((DEBUG_INFO, "    BestMargin %d\n", Eq->Lane[RpLane].BestMargin));
  }
DEBUG_CODE_END();
}


/**
  Initialize SW context for given port.

  @param[out] Eq              Software Equalization parameters
  @param[in]  RpIndex         Port Index
  @param[in]  PciePortConfig  Root port configuration
**/
VOID
EqContextInit (
  OUT EQ_CONTEXT                    *Eq,
  UINT32                            RpIndex,
  CONST PCH_PCIE_ROOT_PORT_CONFIG   *PciePortConfig
  )
{
  UINTN        RpBase;
  UINT32       RpLane;
  UINT32       PcieLane;
  PCH_STEPPING PchStep;
  PCH_SERIES   PchSeries;

  ZeroMem (Eq, sizeof (EQ_CONTEXT));

  DEBUG ((DEBUG_INFO, "EqContextInit (%d)\n", RpIndex+1));

  Eq->RpConfig   = PciePortConfig;

  RpBase = PchPcieBase (RpIndex);
  Eq->RpBase       = RpBase;
  Eq->RpIndex      = (UINT16) RpIndex;
  Eq->MaxLinkWidth = GetMaxLinkWidth (RpBase);
  Eq->LinkWidth    = GetNegotiatedLinkWidth (RpBase);
  DEBUG ((DEBUG_INFO, "  MLW: %d  NLW: %d  CLS: %d  reversal: %d\n",
          Eq->MaxLinkWidth, Eq->LinkWidth, GetLinkSpeed (RpBase),
          IsPcieLaneReversalEnabled (RpIndex)));

  for (RpLane = 0; RpLane < Eq->MaxLinkWidth; ++RpLane) {
    PcieLane = PchPcieLane (RpIndex, RpLane);
    Eq->Lane[RpLane].PcieLane = PcieLane;
    HsioGetLane (HsioPcieLane (PcieLane), &Eq->Lane[RpLane].HsioLane);
    DEBUG ((DEBUG_INFO, "Port %d.%d (PCIE%d) -> HSIO%d (Pid=0x%02x, Base=0x%03x)\n",
            RpIndex+1,
            RpLane,
            PcieLane+1,
            Eq->Lane[RpLane].HsioLane.Index,
            Eq->Lane[RpLane].HsioLane.Pid,
            Eq->Lane[RpLane].HsioLane.Base));
  }

  PchStep   = PchStepping ();
  PchSeries = GetPchSeries ();

  if (((PchSeries == PchLp) && (PchStep <= PchLpC1)) ||
    ((PchSeries == PchH)  && (PchStep <= PchHD1))) {
    Eq->Ctoc = FALSE;
  } else {
    Eq->Ctoc = TRUE;
  }
}


/**
  Perform software link equaliztion using Upstream Port Transmitter Preset
  with phase 2 and 3 disabled.

  @param[in] Eq       Equaliztion context structure
  @param[in] Params   Software Equalization parameters
**/
VOID
SwEqDoPresetBypassPhase23 (
  EQ_CONTEXT          *Eq,
  CONST SW_EQ_PARAMS  *Params
  )
{
  UINTN      RpBase;
  UINT32     RpLane;
  UINT32     Index;
  BOOLEAN    DoneReset;
  UINT32     Count;
  BOOLEAN    RetryMax;
  UINT32     Margin[4];
  EFI_STATUS Status;
  RpBase = Eq->RpBase;

  DEBUG ((DEBUG_INFO, "SwEqDoPresetBypassPhase23 (%d)\n", Eq->RpIndex + 1));

  SwEqResetBestMargin (Eq);

  // Set JIM to soft EQ mode
  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    HsioLaneConfigSwEq (&Eq->Lane[RpLane].HsioLane);
  };

  DEBUG ((DEBUG_INFO, "Disable EQ Phase2/3\n"));
  MmioOr32 (RpBase + R_PCH_PCIE_EQCFG1, B_PCH_PCIE_EQCFG1_RTLEPCEB |
                                        B_PCH_PCIE_EQCFG1_LEP23B |
                                        B_PCH_PCIE_EQCFG1_LEP3B);

  RetryMax = 1;
  for (Index = 0; Index < 5; ++Index) {
    for (Count = 0; Count <= RetryMax; ++Count) {
      if (SwEqRetrainReset (Eq, 1, Params->RetrainTimeoutMs, NULL)) {
        SwEqSetUptp (Eq, Params, Index);
        if (SwEqRetrainReset (Eq, 3, Params->RetrainTimeoutMs, &DoneReset)) {
          Status = SwEqDoMargin (Eq, Params, Margin);
          if (Status == EFI_SUCCESS) {
            SwEqUpdateBestMargin (Eq, Margin, Index);
          }
          if ((Status == EFI_ABORTED) && (DoneReset == FALSE)) {
            RetryMax = 1;
          } else {
            RetryMax = 0;
          }
        }
      }
    }
  }
  DEBUG_CODE (
    SwEqDumpBestMargin (Eq);
  );

  SwEqRetrain (RpBase, 1, Params->RetrainTimeoutMs);
  DEBUG ((DEBUG_INFO, "Enable EQ Phase2/3\n"));
  MmioAnd32 (RpBase + R_PCH_PCIE_EQCFG1, (UINT32) ~(B_PCH_PCIE_EQCFG1_LEP23B |
                                                    B_PCH_PCIE_EQCFG1_LEP3B));
  DEBUG ((DEBUG_INFO, "***Best presets:\n"));
  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    Index = Eq->Lane[RpLane].BestIndex;
    ASSERT (Index < 5);
    PcieSetPreset (Eq->RpBase, RpLane, Params->PresetList[Index]);
  }
  SwEqRetrainReset (Eq, 3, Params->RetrainTimeoutMs, NULL);
}

/**
  Perform software link equaliztion (coefficient search).
  @param[in] Eq       Equaliztion context structure
  @param[in] Params   Software Equalization parameters
**/
VOID
DoGen3SoftwareEq (
  EQ_CONTEXT          *Eq,
  CONST SW_EQ_PARAMS  *Params
  )
{
  UINTN   RpBase;
  UINT32  RpLane;
  UINT32  Index;
  UINT32  Margin[4];

  DEBUG ((DEBUG_INFO, "DoGen3SoftwareEq (%d)\n", Eq->RpIndex + 1));
  RpBase = Eq->RpBase;

  //
  // Bypass EQ Phase 3 evaluation
  //
  MmioOr32 (RpBase + R_PCH_PCIE_EQCFG1, B_PCH_PCIE_EQCFG1_RTLEPCEB);

  DEBUG_CODE (
    SwEqParamsDump (Params);
  );


  SwEqResetBestMargin (Eq);

  // Set JIM to soft EQ mode
  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    HsioLaneConfigSwEq (&Eq->Lane[RpLane].HsioLane);
  };

  for (Index = 0; Index < 5; ++Index) {
    if (SwEqRetrain (RpBase, 1, Params->RetrainTimeoutMs)) {
      SwEqSetCoefficients (Eq, Params, Index);
      if (SwEqRetrain (RpBase, 3, Params->RetrainTimeoutMs)) {
        if (SwEqDoMargin (Eq, Params, Margin) == EFI_SUCCESS) {
          SwEqUpdateBestMargin (Eq, Margin, Index);
        }
      }
    }
  }
  DEBUG_CODE (
    SwEqDumpBestMargin (Eq);
  );

  SwEqRetrain (RpBase, 1, Params->RetrainTimeoutMs);
  DEBUG ((DEBUG_INFO, "***Best coefficients:\n"));
  for (RpLane = 0; RpLane < Eq->LinkWidth; ++RpLane) {
    Index = Eq->Lane[RpLane].BestIndex;
    ASSERT (Index < PCH_PCIE_SWEQ_COEFFS_MAX);
    PcieSetCoefficients (
      Eq->RpBase,
      RpLane,
      Params->CoeffList[Index].Cm,
      Params->CoeffList[Index].Cp
      );
  }
  // If final Gen3 training succedded we are done
  if (SwEqRetrain (RpBase, 3, Params->RetrainTimeoutMs) == FALSE) {
    // Oterwise try with preset and phase 2 bypass.
    SwEqDoPresetBypassPhase23 (Eq, Params);
  }
}

/**
  Populate HW EQ coefficient search list.
  @param[in] RpBase    Root Port base address
  @param[in] Params    Equalization parameters
**/
VOID
InitializeCoeffList (
  UINTN RpBase,
  CONST SW_EQ_PARAMS  *Params
  )
{
  UINT32 Index;

  for (Index = 0; Index < PCH_PCIE_SWEQ_COEFFS_MAX; ++Index) {
    PcieSetCoeffList (
      RpBase,
      Index,
      Params->CoeffList[Index].Cm,
      Params->CoeffList[Index].Cp
      );
  }

  MmioOr32 (RpBase + R_PCH_PCIE_RTPCL1, B_PCH_PCIE_RTPCL1_PCM);

  // Total number of coefficients
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_EQCFG2, (UINT32) ~B_PCH_PCIE_EQCFG2_HAPCSB, 4 << N_PCH_PCIE_EQCFG2_HAPCSB);
  // Number of coefficients per iteration
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_HAEQ,   (UINT32) ~B_PCH_PCIE_HAEQ_HAPCCPI,  4 << N_PCH_PCIE_HAEQ_HAPCCPI);

  for (Index = 0; Index < 4; ++Index) {
    DEBUG ((DEBUG_INFO, "RTPCL%d = 0x%08x\n", Index, MmioRead32 (RpBase + R_PCH_PCIE_RTPCL1 + (Index * 4))));
  }
}

/**
  Perform hardware link equaliztion.
  @param[in] Eq       Equaliztion context structure
  @param[in] Params   Equalization parameters
**/
VOID
DoGen3HardwareEq (
  EQ_CONTEXT          *Eq,
  CONST SW_EQ_PARAMS  *Params
  )
{
  UINTN  RpBase;
  UINT32 Data32Or;
  UINT32 Data32And;

  DEBUG ((DEBUG_INFO, "DoGen3HardwareEq (%d)\n", Eq->RpIndex + 1));

  RpBase = Eq->RpBase;

  DEBUG ((DEBUG_INFO, "LSTS2: 0x%04x\n", MmioRead16 (RpBase + R_PCH_PCIE_LSTS2)));

  //
  // Clear Remote Transmitter Preset Coefficient Override Enable
  //
  MmioAnd32 (RpBase + R_PCH_PCIE_EQCFG1, (UINT32) ~(B_PCH_PCIE_EQCFG1_RTPCOE |
                                                    B_PCH_PCIE_EQCFG1_TUPP));

  InitializeCoeffList (RpBase, Params);

  Data32And = (UINT32) ~(B_PCH_PCIE_EQCFG2_PCET |
                         B_PCH_PCIE_EQCFG2_REWMET);
  Data32Or  = (4 << N_PCH_PCIE_EQCFG2_PCET) | 0x02; // REWMET = 2 (4 errors)

  MmioAndThenOr32 (RpBase + R_PCH_PCIE_EQCFG2, Data32And, Data32Or);
  MmioAnd32 (RpBase + R_PCH_PCIE_HAEQ, (UINT32) ~B_PCH_PCIE_HAEQ_MACFOMC); // Clear MACFOMC

  DEBUG ((DEBUG_INFO, "EQCFG1: 0x%08x\n", MmioRead32 (RpBase + R_PCH_PCIE_EQCFG1)));
  DEBUG ((DEBUG_INFO, "EQCFG2: 0x%08x\n", MmioRead32 (RpBase + R_PCH_PCIE_EQCFG2)));
  DEBUG ((DEBUG_INFO, "HAEQ:   0x%08x\n", MmioRead32 (RpBase + R_PCH_PCIE_HAEQ)));
  DEBUG ((DEBUG_INFO, "L01EC:  0x%08x\n", MmioRead32 (RpBase + R_PCH_PCIE_EX_L01EC)));
  DEBUG ((DEBUG_INFO, "L23EC:  0x%08x\n", MmioRead32 (RpBase + R_PCH_PCIE_EX_L23EC)));

  MmioAndThenOr16 (RpBase + R_PCH_PCIE_LCTL2, (UINT16) ~B_PCIE_LCTL2_TLS, 3);
  MmioOr32 (RpBase + R_PCH_PCIE_EX_LCTL3, B_PCIE_EX_LCTL3_PE);
  MmioOr32 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_RL);
}

/**
  Static equaliztion flow
  @param[in] Eq       Equaliztion context structure
  @param[in] Param    Fixed coefficients to use
**/
VOID
DoGen3StaticEq (
  EQ_CONTEXT                    *Eq,
  CONST PCH_PCIE_EQ_LANE_PARAM  *Param
  )
{
  UINTN   RpBase;
  UINT32  RpLane;
  UINT8   Cm;
  UINT8   Cp;

  DEBUG((DEBUG_INFO, "DoGen3StaticEq (%d)\n", Eq->RpIndex + 1));
  RpBase = Eq->RpBase;

  //
  // Bypass EQ Phase 3
  //
  MmioOr32 (RpBase + R_PCH_PCIE_EQCFG1, B_PCH_PCIE_EQCFG1_RTLEPCEB);

  for (RpLane = 0; RpLane < Eq->MaxLinkWidth; ++RpLane) {
    Cm = Param[Eq->Lane[RpLane].PcieLane].Cm;
    Cp = Param[Eq->Lane[RpLane].PcieLane].Cp;
    PcieSetCoefficients (RpBase, RpLane, Cm, Cp);
  }

  MmioAndThenOr16 (RpBase + R_PCH_PCIE_LCTL2, (UINT16) ~B_PCIE_LCTL2_TLS, 3);
  MmioOr32 (RpBase + R_PCH_PCIE_EX_LCTL3, B_PCIE_EX_LCTL3_PE);
  MmioOr32 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_RL);
}

/**
  Force link speed to Gen1.

  Since root port TLS is persistent device may initiate Gen3 training after reset or S3.
  This is only observed with some devices.

  @param[in] RpBase    Root Port base address
**/
BOOLEAN
ForceGen1 (
  UINTN RpBase
  )
{
  UINT32  LinkState;
  UINT32  Timeout;

  if (SwEqRetrain (RpBase, 1, 50) == FALSE) {
    return FALSE;
  }

  DEBUG ((DEBUG_INFO, "Forcing link to Gen1\n"));

  MmioOr32 (RpBase + R_PCH_PCIE_PCIEALC, B_PCH_PCIE_PCIEALC_BLKDQDA);

  MmioOr16 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_LD);


  LinkState = GetLinkState (RpBase);
  Timeout = 0;
  while (Timeout < 50000){
   if ((LinkState != V_PCH_PCIE_PCIESTS1_LTSMSTATE_DISWAIT) &&
       (LinkState != V_PCH_PCIE_PCIESTS1_LTSMSTATE_DISWAITPG)) {
       MicroSecondDelay (2);
       LinkState = GetLinkState (RpBase);
    } else {
     // Exit from the loop since Link could be disabled successfully
      break; 
    }
    Timeout += 10;
  }

 if(Timeout >= 50000){
     DEBUG ((DEBUG_INFO, "ERROR: Link of Port %x could not be disabled before issue a retrain \n", RpBase));
 }
  MmioAndThenOr16 (RpBase + R_PCH_PCIE_LCTL2, (UINT16) ~B_PCIE_LCTL2_TLS, V_PCIE_LCTL2_TLS_GEN1);
  MmioAnd16 (RpBase + R_PCH_PCIE_LCTL, (UINT16) ~(B_PCIE_LCTL_LD));
  MmioAnd32 (RpBase + R_PCH_PCIE_PCIEALC, (UINT32) ~B_PCH_PCIE_PCIEALC_BLKDQDA);

  Timeout = 0;
  while (Timeout < 50000){
    if (MmioRead16 (RpBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA) {
      break;
    }
    MicroSecondDelay (10);
    Timeout += 10;
  }
  return TRUE;
}

/**
  Perform software link equaliztion (coefficient search).
  @param[in] RpIndex      Port index
  @param[in] PchPolicy    The PCH Policy
  @param[in] TempPciBus   Temp bus number
**/
VOID
Gen3LinkEqualize (
  UINT32                 RpIndex,
  CONST PCH_POLICY_PPI   *PchPolicy,
  UINT8                  TempPciBus,
  BOOLEAN                Gen3DeviceFound

  )
{
  EQ_CONTEXT               Eq;
  SW_EQ_PARAMS             SwEqParams;
  UINTN                    RpBase;
  PCH_PCIE_EQ_METHOD       EqMethod;
  CONST PCH_PCIE_CONFIG    *PcieConfig;
  CONST PCH_PCIE_CONFIG2   *PcieConfig2;

  DEBUG ((DEBUG_INFO, "Gen3LinkEqualize (%d)\n", RpIndex+1));

  PcieConfig  = &PchPolicy->PcieConfig;
  PcieConfig2 = &PchPolicy->PcieConfig2;

  EqContextInit (&Eq, RpIndex, &PcieConfig->RootPort[RpIndex]);
  RpBase = Eq.RpBase;

  SwEqParamsInit (&SwEqParams);

  SwEqParamsUpdate (
    &SwEqParams,
    PcieConfig2
    );

  ASSERT (GetLinkSpeed (RpBase) < 3);

  EqMethod = PcieConfig->RootPort[RpIndex].Gen3EqPh3Method;

  if (EqMethod == PchPcieEqHardware) {
    DoGen3HardwareEq (&Eq, &SwEqParams);
  } else if (EqMethod == PchPcieEqStaticCoeff) {
    DoGen3StaticEq (&Eq, PcieConfig->EqPh3LaneParam);
  } else {
    if (PcieConfig->RootPort[RpIndex].HotPlug) {
      DEBUG ((DEBUG_WARN, "Warning: Only static or hardware Gen3 equalization is supported with HotPlug\n"));
    }
    if (Gen3DeviceFound) {
      switch (EqMethod) {
      case PchPcieEqDefault:
      case PchPcieEqSoftware:
        //
        // Skip Gen3 Sw Eq on S3 resume flow to save time
        //
        if (!IsDelayedGen3Retrain (PcieConfig, RpIndex, Gen3DeviceFound)) {
          DoGen3SoftwareEq (&Eq, &SwEqParams);
          ClearCorrectableErrors (RpBase);
        }
        break;
      default:
        ASSERT(FALSE);
        break;
      }
    }
  }
  ResetErrorCounts (RpBase);

}

/**
  Initialize non-common clock port.

  @param[in] RpIndex    Root Port index
**/
VOID
PcieInitNccPort (
  IN UINT32   RpIndex
  )
{
  UINTN      RpBase;
  UINT32     RpLane;
  UINT32     PcieLane;
  UINT32     MaxLinkWidth;
  HSIO_LANE  HsioLane;

  DEBUG ((DEBUG_INFO, "PcieInitNccPort(%d)\n", RpIndex+1));

  RpBase = PchPcieBase (RpIndex);
  MaxLinkWidth = GetMaxLinkWidth (RpBase);
  for (RpLane = 0; RpLane < MaxLinkWidth; ++RpLane) {
    PcieLane = PchPcieLane (RpIndex, RpLane);
    HsioGetLane (HsioPcieLane (PcieLane), &HsioLane);
    HsioPcieNccLaneInit (&HsioLane);
  }
  MmioAnd32 (RpBase + R_PCH_PCIE_PCIEALC, (UINT32) ~B_PCH_PCIE_PCIEALC_BLKDQDA);
}

/**
  Verify whether the PCIe port does own all lanes according to the port width.
  @param[in] RpBase    Root Port base address
**/
BOOLEAN
IsPciePortOwningLanes (
  IN     UINTN   RpBase
  )
{
  UINT32     MaxLinkWidth;
  UINT32     RpLane;
  UINT32     RpIndex;
  UINT32     PcieLane;
  UINT32     Lane;
  UINT8      PortId;
  UINT8      LaneOwner;

  RpIndex      = PciePortIndex (RpBase);
  MaxLinkWidth = GetMaxLinkWidth (RpBase);
  for (RpLane = 0; RpLane < MaxLinkWidth; ++RpLane) {
    PcieLane = PchPcieLane (RpIndex, RpLane);
    Lane     = HsioPcieLane (PcieLane );
    PchGetLaneInfo (Lane, &PortId, &LaneOwner);
    if (LaneOwner != V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
  Check for device presence with timeout.

  @param[in]     RpBase      Root Port base address
  @param[in]     TimeoutUs   Timeout in microseconds
  @param[in,out] Timer       Timer value, must be initialized to zero
                             before the first call of this function.
**/
BOOLEAN
PchPcieIsDevicePresent (
  IN     UINTN   RpBase,
  IN     UINT32  TimeoutUs,
  IN OUT UINT32  *Timer
)
{
  while (TRUE) {
    if (MmioRead16 (RpBase + R_PCH_PCIE_SLSTS) & B_PCIE_SLSTS_PDS) {
      return TRUE;
    }
    if (*Timer < TimeoutUs) {
      MicroSecondDelay (10);
      *Timer += 10;
    } else {
      break;
    }
  }
  return FALSE;
}

/**
  Clear SMI_STS_PCI_EXP SMI and HOT_PLUG_STS SCI if they are present
  Check PMC SMI_STS[20] or GPE0_STS_127_96[1], if set, look for the "ONE" RP which has a device connected (means link active)
  1. Enable Hot-Plug SMI(MPC.HPME) and SCI (MPC.HPCE).
  2. Set Link Disabled bit to 1.
  3. Check LTSSM state register to ensure that the link is in Disabled state. (check link active)
  4. Disable HotPlug SMI(MPC.HPME) and SCI (MPC.HPCE).
  5. Clear Link Disabled bit.
  6. Poll for link active for 12ms and wait for 100ms if subsequent code invlove device access.
  7. Clear all status bits in SLSTS.
  8. Clear all status bits in SMSCS.
  9. Check to ensure that PCI_EXP_SMI_STS and HOT_PLUG_STS is cleared.

  @param[in] PciExpressConfig           PCIe policy configuration
  @param[in] ABase                      The PM I/O Base address of the PCH
**/
VOID
ClearPcieSmiSciSts (
  IN  CONST PCH_PCIE_CONFIG             *PciExpressConfig,
  IN  UINT16                            ABase
  )
{
  UINT8                                 RpIndex;
  UINTN                                 RpBase;
  UINT8                                 MaxPciePortNum;
  UINT32                                SmiSts;
  UINT32                                SciSts;

  if (PchIsDwrFlow()) {
    DEBUG ((DEBUG_INFO, "DWR: ClearPcieSmiSciSts() End\n"));
    return;
  }

  SmiSts = IoRead32 (ABase + R_PCH_SMI_STS);
  SciSts = IoRead32 (ABase + R_PCH_ACPI_GPE0_STS_127_96);

  if ((SmiSts & B_PCH_SMI_STS_PCI_EXP) ||
      (SciSts & B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG))
  {
    DEBUG ((DEBUG_INFO, "ClearPcieSmiSciSts begin SmiSts %x SciSts %x\n", SmiSts, SciSts));
    MaxPciePortNum   = GetPchMaxPciePortNum ();

    for (RpIndex = 0; RpIndex < MaxPciePortNum; RpIndex++) {
      RpBase = PchPcieBase (RpIndex);
      if (MmioRead16 (RpBase) == 0xFFFF) continue;

      if ((MmioRead16 (RpBase + R_PCH_PCIE_XCAP) & B_PCIE_XCAP_SI) &&
          (MmioRead16 (RpBase + R_PCH_PCIE_SLSTS) & B_PCIE_SLSTS_PDS) &&
          (PciExpressConfig->RootPort[RpIndex].HotPlug == TRUE) )
      {
        DEBUG ((DEBUG_INFO, "ClearPcieSmiSciSts applies w/a\n"));
        //
        // set MPC.HPME and MPC.HPCE
        // set LCTL.LD (SLSTS.DLLSC and SMSCS.HPLAS got set by this)
        // check LSTS.LA = 0
        // clear MPC.HPME and MPC.HPCE
        // clear LCTL.LD (SLSTS.PDC got set by this since SLSTS.PDS change from 1 to 0.)
        //               (SMSCS.HPPDM is not set since MPC.HPME is not set)
        // poll for LSTS.LA = 1 for 12ms and wait for 100ms. simply wait for 120ms (SLSTS.PDS got set to 1)
        // clear status of SLSTS
        // clear status of SMSCS
        //
        MmioOr8 (RpBase + R_PCH_PCIE_MPC, B_PCH_PCIE_MPC_HPME);
        MmioOr8 (RpBase + R_PCH_PCIE_MPC + 3, (B_PCH_PCIE_MPC_HPCE >> 24));
        MmioOr16 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_LD);
        while (MmioRead16 (RpBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA) {MicroSecondDelay (5);}
        MmioAnd8 (RpBase + R_PCH_PCIE_MPC, (UINT8)~B_PCH_PCIE_MPC_HPME);
        MmioAnd8 (RpBase + R_PCH_PCIE_MPC + 3, (UINT8)~(B_PCH_PCIE_MPC_HPCE >> 24));
        MmioAnd16 (RpBase + R_PCH_PCIE_LCTL, (UINT16)~B_PCIE_LCTL_LD);
        MicroSecondDelay ((12 + 100) * 1000);
        MmioWrite16 (RpBase + R_PCH_PCIE_SLSTS, 0x0108);
        MmioWrite32 (RpBase + R_PCH_PCIE_SMSCS, 0xC0000073);
        break;
      }
    }
    DEBUG ((DEBUG_INFO, "ClearPcieSmiSciSts end SmiSts %x SciSts %x\n", IoRead32 (ABase + R_PCH_SMI_STS), IoRead32 (ABase + R_PCH_ACPI_GPE0_STS_127_96)));
  }
}

/**
  Checks if given rootport should be left visible even though disabled, in order to avoid PCIE rootport swapping

  @param[in] RpIndex           rootport number
  @param[in] RpDisableMask     bitmask of all disabled rootports
  @param[in] PciExpressConfig  PCIe policy configuration

  @retval TRUE  port should be kept visible despite being disabled
  @retval FALSE port should be disabled and hidden

**/
BOOLEAN
IsPortForceVisible (
  IN UINT8                 RpIndex,
  IN UINT32                RpDisableMask,
  IN CONST PCH_PCIE_CONFIG *PciExpressConfig
  )
{
  UINT32 FunctionsEnabledPerDevice;
  UINT32 RpEnabledMask;

  //
  // only rootports mapped to Function0 are relevant for preventing rootport swap
  //
  if ((PciExpressConfig->RpFunctionSwap == 1) || (RpIndex % 8 != 0)) {
    return FALSE;
  }
  //
  // set a bit for each port that exists and isn't disabled
  //
  RpEnabledMask = (1u << GetPchMaxPciePortNum ()) - 1;
  RpEnabledMask &= (~RpDisableMask);

  FunctionsEnabledPerDevice = (RpEnabledMask >> ((RpIndex/8)*8) ) & 0xFF;
  if (FunctionsEnabledPerDevice != 0) {
    return TRUE;
  }
  return FALSE;
}
/**
  wrapper for PchGetLaneInfo() that returns the LaneOwner data directly instead of through pointer
  @param[in]  LaneNumber

  @retval     0 - V_PCH_PCR_FIA_LANE_OWN_PCIEDMI
              1 - V_PCH_PCR_FIA_LANE_OWN_USB3
              ...
              0xFF - error, illegal LaneNumber
**/
UINT8 GetLaneOwner (
  IN UINT32 LaneNumber
  )
{
  UINT8 LaneOwner;
  UINT8 PortId;
  
  if (!EFI_ERROR(PchGetLaneInfo (LaneNumber, &PortId, &LaneOwner))) {
    return LaneOwner;
  } else {
    ASSERT(FALSE);
    return 0xFF;
  }
}

/**
  If there is a PCIE port configured as x2 or x4, its 1st lane is not owned by PCIE but the following are,
  then following lanes would not be power gated. This function returns list of such lanes.

  @retval      bitmask of lanes to be power gated
**/
UINT32
GetPgLanes (
  VOID
  )
{
  UINT8  CtrlLanes[4];
  UINT32 LanesToPowerGate;
  UINT8  CtrlIndex;
  UINT8  LaneIndex;
  UINT8  MaxControllers;

  LanesToPowerGate = 0;
  if (GetPchSeries () == PchLp) {
    MaxControllers = PCH_LP_PCIE_MAX_CONTROLLERS;
  } else {
    MaxControllers = PCH_H_PCIE_MAX_CONTROLLERS;
  }

  for (CtrlIndex=0; CtrlIndex < MaxControllers; CtrlIndex++) {
    //
    // get the list of 4 lanes assigned to particular controller (SPA..SPE)
    //
    if (IsPcieLaneReversalEnabled (PCH_PCIE_CONTROLLER_PORTS * CtrlIndex)) {
      CtrlLanes[0] = HsioPcieLane (3 + PCH_PCIE_CONTROLLER_PORTS * CtrlIndex);
      CtrlLanes[1] = HsioPcieLane (2 + PCH_PCIE_CONTROLLER_PORTS * CtrlIndex);
      CtrlLanes[2] = HsioPcieLane (1 + PCH_PCIE_CONTROLLER_PORTS * CtrlIndex);
      CtrlLanes[3] = HsioPcieLane (0 + PCH_PCIE_CONTROLLER_PORTS * CtrlIndex);
    } else {
      CtrlLanes[0] = HsioPcieLane (0 + PCH_PCIE_CONTROLLER_PORTS * CtrlIndex);
      CtrlLanes[1] = HsioPcieLane (1 + PCH_PCIE_CONTROLLER_PORTS * CtrlIndex);
      CtrlLanes[2] = HsioPcieLane (2 + PCH_PCIE_CONTROLLER_PORTS * CtrlIndex);
      CtrlLanes[3] = HsioPcieLane (3 + PCH_PCIE_CONTROLLER_PORTS * CtrlIndex);
    }
    DEBUG ((DEBUG_INFO, "GetPgLanes SP%c: %d,%d,%d,%d\n",('A'+CtrlIndex),CtrlLanes[0],CtrlLanes[1],CtrlLanes[2],CtrlLanes[3] ));
    //
    // based on bifurcation and lane ownership of the 4 lanes decide which PCIE lanes are shadowed by non-PCIE ones
    //
    switch (GetPcieControllerConfig (CtrlIndex)) {
      case Pcie4x1 :
        break;
      case Pcie1x2_2x1 :
        if (GetLaneOwner (CtrlLanes[0]) != V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          if (GetLaneOwner (CtrlLanes[1]) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
            LanesToPowerGate |= BIT0 << CtrlLanes[1]; 
          }
        }
        break;
      case Pcie2x2 :
        if (GetLaneOwner (CtrlLanes[0]) != V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          if (GetLaneOwner (CtrlLanes[1]) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
            LanesToPowerGate |= BIT0 << CtrlLanes[1];
          }
        }
        if (GetLaneOwner (CtrlLanes[2]) != V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          if (GetLaneOwner (CtrlLanes[3]) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
            LanesToPowerGate |= BIT0 << CtrlLanes[3];
          }
        }
        break;
      case Pcie1x4 :
        if (GetLaneOwner (CtrlLanes[0]) != V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          for (LaneIndex = 1; LaneIndex <= 3; LaneIndex++) {
            if (GetLaneOwner (CtrlLanes[LaneIndex]) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) { 
              LanesToPowerGate |= BIT0 << CtrlLanes[LaneIndex];
            }
          }
        }
        break;
      default:
        ASSERT(FALSE);
    }
    DEBUG ((DEBUG_INFO, "GetPgLanes LanesToPowerGate %08x\n", LanesToPowerGate ));
  }
  
  return LanesToPowerGate;
}

/**
  Workaround for HSIO lanes power gating.
  BIOS will send power gating override messages to PMC telling it to power gate PCIE lanes which belong
  to a port which is not fully owned by PCIE. Primary use case is M.2 connector with x4 PCIE port and SATA
  drive connected, which causes some lanes from the x4 to be reconfigured from PCIE to SATA.
**/
VOID
FixPcieLanesPowerGating (
  VOID
  )
{
  UINT32 PchPwrmBase;
  UINT32 LanesToPowerGate;
  UINT16 LowerLanes;
  UINT16 HigherLanes;
  UINT32 Data32;

  //
  // Check which lanes need power gating workaround
  //
  LanesToPowerGate = GetPgLanes ();
  PchPwrmBaseGet (&PchPwrmBase);
  LowerLanes = (UINT16)(LanesToPowerGate & 0xFFFF);
  HigherLanes = (UINT16)((LanesToPowerGate & 0xFFFF0000)>>16);

  //
  // Send commands to PMC to power gate selected lanes
  //
  if (LowerLanes != 0) {
    Data32 = (LowerLanes << N_PCH_PWRM_MTPMC_PG_CMD_DATA) | V_PCH_PWRM_MTPMC_COMMAND_PG_LANE_0_15;
    MmioWrite32 (PchPwrmBase + R_PCH_PWRM_MTPMC, Data32);
    DEBUG ((DEBUG_INFO, "FixPcieLanesPowerGating: Command %08x sent to PMC\n", Data32 ));
  }
  if (HigherLanes != 0) {
    Data32 = (HigherLanes << N_PCH_PWRM_MTPMC_PG_CMD_DATA) | V_PCH_PWRM_MTPMC_COMMAND_PG_LANE_16_31;
    MmioWrite32 (PchPwrmBase + R_PCH_PWRM_MTPMC, Data32);
    DEBUG ((DEBUG_INFO, "FixPcieLanesPowerGating: Command %08x sent to PMC\n", Data32 ));
  }
  if (LanesToPowerGate == 0) {
    DEBUG ((DEBUG_INFO, "FixPcieLanesPowerGating: No message sent to PMC\n"));
  }
  //
  // Lock the message interface
  //
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG3, B_PCH_PWRM_CFG3_PWRG_LOCK);

}

/**
  Perform Initialization of the Downstream Root Ports.

  @param[in] PchPolicy            The PCH Policy PPI
  @param[in] TempPciBusMin        The temporary minimum Bus number for root port initialization
  @param[in] TempPciBusMax        The temporary maximum Bus number for root port initialization

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PchInitRootPorts (
  IN CONST PCH_POLICY_PPI    *PchPolicy,
  IN UINT8                   TempPciBusMin,
  IN UINT8                   TempPciBusMax
  )
{
  CONST PCH_PCIE_CONFIG   *PciExpressConfig;
  EFI_STATUS              Status;
  UINT32                  Data32Or;
  UINT8                   RpIndex;
  UINTN                   RpBase;
  UINT8                   MaxPciePortNum;
  UINTN                   PmcBaseAddress;
  UINT16                  ABase;
  UINT32                  RpDisableMask;
  UINT32                  RpClkreqMask;
  BOOLEAN                 PortWithMaxPayload128;
  UINT8                   RpLinkSpeed;
  UINT32                  Timer;
  UINT32                  DetectTimeoutUs;
  BOOLEAN                 Gen3DeviceFound[PCH_MAX_PCIE_ROOT_PORTS];
  BOOLEAN                 KeepPortVisible;

  DEBUG ((DEBUG_INFO, "PchInitRootPorts() Start\n"));

  if (PchIsDwrFlow()) {
    DEBUG ((DEBUG_INFO, "DWR: PchInitRootPorts() End\n"));
    return EFI_SUCCESS;
  }

  Status            = EFI_SUCCESS;
  PciExpressConfig  = &PchPolicy->PcieConfig;
  PmcBaseAddress    = MmPciBase (
                        DEFAULT_PCI_BUS_NUMBER_PCH,
                        PCI_DEVICE_NUMBER_PCH_PMC,
                        PCI_FUNCTION_NUMBER_PCH_PMC
                        );
  PchAcpiBaseGet (&ABase);
  PortWithMaxPayload128 = FALSE;
  Timer            = 0;
  DetectTimeoutUs  = PciExpressConfig->DetectTimeoutMs * 1000;
  MaxPciePortNum   = GetPchMaxPciePortNum ();
  RpDisableMask    = 0;
  RpClkreqMask     = 0;

  for (RpIndex = 0; RpIndex < MaxPciePortNum; RpIndex++) {
    RpBase = PchPcieBase (RpIndex);

    //
    // Enable Server Error Reporting Management feature by setting bit 29  (SERM) at offset 0xFC
    // in PCH PCIe root ports configuration space.
    // Note: Bit 29 (SERM) is writable only for first port in x4 configuration
    // and it enables SERM feature for entire x4 port. For remaining 3 ports SERM bit is RO.
    //
    if ((RpIndex % 4) == 0) {
      if (PchPolicy->PchConfig.Serm == TRUE){
        MmioOr32 (RpBase + R_PCH_PCIE_STRPFUSECFG, (UINT32)B_PCH_PCIE_STRPFUSECFG_SERM);
      } else {
        MmioAnd32 (RpBase + R_PCH_PCIE_STRPFUSECFG, (UINT32)~B_PCH_PCIE_STRPFUSECFG_SERM);
      }
    }

    //
    // Determine available ports based on lane ownership and port configuration (x1/x2/x4)
    // Root ports can be already disabled by PchEarlyDisabledDeviceHandling
    //
    if ((MmioRead16 (RpBase) == 0xFFFF) ||
        (IsPciePortOwningLanes (RpBase) == FALSE)) {
      RpDisableMask |= (BIT0 << RpIndex);
      continue;
    }

    ///
    /// Set the Slot Implemented Bit.  Note that this must be set before presence is valid.
    /// PCH BIOS Spec section 8.2.3, The System BIOS must
    /// initialize the "Slot Implemented" bit of the PCI Express* Capabilities Register,
    /// XCAP Dxx:Fn:42h[8] of each available and enabled downstream root port.
    ///
    /// Set SI also for slots disabled by policy since disabling flow depends on device presence.
    ///
    MmioOr16 (RpBase + R_PCH_PCIE_XCAP, B_PCIE_XCAP_SI);

    if (PciExpressConfig->RootPort[RpIndex].Enable == FALSE) {
      RpDisableMask |= (BIT0 << RpIndex);
      continue;
    }

    //
    // Initialize SRIS ports
    //
    if (IsPcieNcc (RpBase)) {
      PcieInitNccPort (RpIndex);
      if (DetectTimeoutUs == 0) {
        DetectTimeoutUs = 15000; // 15 ms
      }
    }
  }

  //
  // Clear SMI_STS_PCI_EXP and HOT_PLUG_STS if they are present.
  // Do this after XCAP_SI set.
  //
  ClearPcieSmiSciSts (PciExpressConfig, ABase);

  for (RpIndex = 0; RpIndex < MaxPciePortNum; RpIndex++) {
    RpBase = PchPcieBase (RpIndex);

    //
    // Enable CLKREQ# reagrdless of port being available/enabled to allow clock gating.
    //
    if (PciExpressConfig->RootPort[RpIndex].ClkReqSupported) {
      RpClkreqMask |= (BIT0 << RpIndex);
    }

    //
    // Skip unavailable ports
    //
    if (RpDisableMask & (BIT0 << RpIndex)) {
      continue;
    }

    ///
    /// For non-hotplug ports disable the port if there is no device present.
    ///
    if (PchPcieIsDevicePresent (RpBase, DetectTimeoutUs, &Timer)) {
      DEBUG ((DEBUG_INFO, "Port %d has a device attached.\n", RpIndex + 1));
      //
      // If device is present disable CLKREQ# if not connected.
      //
      if (PchPcieDetectClkreq (&PciExpressConfig->RootPort[RpIndex]) == FALSE) {
        RpClkreqMask &= ~(BIT0 << RpIndex);
      }
    } else {
      if (PciExpressConfig->RootPort[RpIndex].HotPlug == FALSE) {
        RpDisableMask |= (BIT0 << RpIndex);
      }
    }
  }

  for (RpIndex = 0; RpIndex < MaxPciePortNum; RpIndex++) {
    if (RpClkreqMask & (BIT0 << RpIndex)) {
      //
      // Enabled CLKREQ# pad if supported to allow clock gating regardless of port being enabled.
      //
      GpioEnableClkreq (PciExpressConfig->RootPort[RpIndex].ClkReqNumber);
    }

    ///
    /// Configure power management applicatble to all port including disabled ports.
    ///
    if (PciExpressConfig->DisableRootPortClockGating == FALSE) {
      if ((RpIndex % PCH_PCIE_CONTROLLER_PORTS) == 0) {
        //
        // TrunkClockGateEn depends on each of the controller ports supporting CLKREQ# or being disabled.
        //
        PchPcieConfigureControllerBasePowerManagement (
          RpIndex,
          (((RpClkreqMask | RpDisableMask) & (0xFu << RpIndex)) == (0xFu << RpIndex))
          );
      }
      ///
      ///
      DEBUG ((DEBUG_INFO, "Skip PchPcieConfigureBasePowerManagement(%d)\n", RpIndex + 1));
    }
  }

  ///
  /// For each controller set Initialize Transaction Layer Receiver Control on Link Down
  /// and Initialize Link Layer Receiver Control on Link Down.
  ///
  for (RpIndex = 0; RpIndex < MaxPciePortNum; ++RpIndex) {
    if ((RpIndex % PCH_PCIE_CONTROLLER_PORTS) == 0) {
      MmioOr32 (PchPcieBase (RpIndex) + R_PCH_PCIE_PCIEALC, B_PCH_PCIE_PCIEALC_ITLRCLD | B_PCH_PCIE_PCIEALC_ILLRCLD);
    }
  }

  for (RpIndex = 0; RpIndex < MaxPciePortNum; ++RpIndex) {
    RpBase = PchPcieBase (RpIndex);
    if (RpDisableMask & (BIT0 << RpIndex)) {
      KeepPortVisible = IsPortForceVisible (RpIndex, RpDisableMask, PciExpressConfig);
      PchDisableRootPort (RpIndex, KeepPortVisible);
    } else {
      /// PCH BIOS Spec Section 8.2.10 Completion Retry Status Replay Enable
      /// Following reset it is possible for a device to terminate the
      /// configuration request but indicate that it is temporarily unable to process it,
      /// but in the future. The device will return the Configuration Request Retry Status.
      /// By setting the Completion Retry Status Replay Enable, Dxx:Fn + 320h[22],
      /// the RP will re-issue the request on receiving such status.
      /// The BIOS shall set this bit before first configuration access to the endpoint.
      MmioOr32 (RpBase + R_PCH_PCIE_PCIECFG2, B_PCH_PCIE_PCIECFG2_CRSREN);

      ///
      /// Configure the rootports
      ///
      Status = PchInitSingleRootPort (
                RpIndex,
                PchPolicy,
                PmcBaseAddress,
                ABase,
                TempPciBusMin,
                TempPciBusMax,
                &Gen3DeviceFound[RpIndex]
                );
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, " Root Port %x device enabled. RpDisableMask: 0x%x\n",
          RpIndex + 1,
          RpDisableMask));
      } else {
        continue;
      }

      ///
      /// Perform equalization for Gen3 capable ports
      ///
      if (GetMaxLinkSpeed (RpBase) >= 3) {
        Gen3LinkEqualize (RpIndex, PchPolicy, TempPciBusMin, Gen3DeviceFound[RpIndex]);
      }

      if (!IsDelayedGen3Retrain (&PchPolicy->PcieConfig, RpIndex, Gen3DeviceFound[RpIndex])) {
        /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
        /// Set "Link Speed Training Policy", Dxx:Fn + D4h[6] to 1.
        /// Make sure this is after mod-PHY related programming is completed.
        MmioOr32 (RpBase + R_PCH_PCIE_MPC2, B_PCH_PCIE_MPC2_LSTP);
      }

      ///
      /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
      /// Step 29 If Transmitter Half Swing is enabled, program the following sequence
      /// a. Ensure that the link is in L0.
      /// b. Program the Link Disable bit (0x50[4]) to 1b.
      /// c. Program the Analog PHY Transmitter Voltage Swing bit (0xE8[13]) to set the transmitter swing to half/full swing
      /// d. Program the Link Disable bit (0x50[4]) to 0b.
      /// BIOS can only enable this on SKU where GEN3 capability is disabled on that port
      RpLinkSpeed   = MmioRead8 (RpBase + R_PCH_PCIE_LCAP) & B_PCIE_LCAP_MLS;
      if ((RpLinkSpeed < V_PCIE_LCAP_MLS_GEN3) &&
        ((PciExpressConfig->RootPort[RpIndex].TransmitterHalfSwing) &&
        (((MmioRead32 (RpBase + R_PCH_PCIE_PCIESTS1) & (B_PCH_PCIE_PCIESTS1_LNKSTAT)) >> N_PCH_PCIE_PCIESTS1_LNKSTAT) == V_PCH_PCIE_PCIESTS1_LNKSTAT_L0))) {
        MmioOr8 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_LD);

        MmioOr16 (RpBase + R_PCH_PCIE_PWRCTL, B_PCH_PCIE_PWRCTL_TXSWING);

        MmioAnd8 (RpBase + R_PCH_PCIE_LCTL, (UINT8) ~(B_PCIE_LCTL_LD));
      }
    }
  }

  for (RpIndex = 0; RpIndex < MaxPciePortNum; RpIndex++) {
    RpBase = PchPcieBase (RpIndex);
    if (MmioRead16 (RpBase + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      continue;
    }

    ///
    /// PCH BIOS Spec Section 8.10 PCI Bus Emulation & Port80 Decode Support
    /// The I/O cycles within the 80h-8Fh range can be explicitly claimed
    /// by the PCIe RP by setting MPC.P8XDE, PCI offset D8h[26] = 1 (using byte access)
    ///
    if ((PciExpressConfig->EnablePort8xhDecode) &&
      (PciExpressConfig->PchPciePort8xhDecodePortIndex == RpIndex)) {
      MmioOr8 (RpBase + R_PCH_PCIE_MPC + 3, (UINT8) (B_PCH_PCIE_MPC_P8XDE >> 24));
      ///
      /// PCH BIOS Spec Section 8.10 PCI Bus Emulation & Port80 Decode Support
      /// Set PCIE RP PCI offset ECh[2] = 1b
      ///
      Data32Or = B_PCH_PCIE_DC_PCIBEM;
      MmioOr32 (RpBase + R_PCH_PCIE_DC, Data32Or);
      ///
      /// PCH BIOS Spec Section 8.10 PCI Bus Emulation & Port80 Decode Support
      /// BIOS must also configure the corresponding DMI registers GCS.RPR and GCS.RPRDID
      /// to enable DMI to forward the Port8x cycles to the corresponding PCIe RP
      ///
      PchIoPort80DecodeSet (PciExpressConfig->PchPciePort8xhDecodePortIndex);
    }
  }

  ///
  /// PCH BIOS Spec Section 8.2.9
  /// Enable PCIe Relaxed Order to always allow downstream completions to pass posted writes. To enable feature set the following registers:
  /// 2)PCR[DMI] + 2314h[31, 7] = [1b, 1b]
  /// 3)PSF_1_PSF_PORT_CONFIG_PG0_PORT0 bit [1] = [1b]
  PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_2314, (UINT32) ~(BIT31 | BIT7), (UINT32) (BIT31 | BIT7));
  PchPcrAndThenOr32 (PID_PSF1, R_PCH_PCR_PSF_PORT_CONFIG_PG0_PORT0, ~0u, BIT1);

  //
  // Program the root port target link speed based on policy.
  //
  Status = PchPcieRpSpeedChange (PchPolicy, Gen3DeviceFound);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize R/WO Registers that described in PCH BIOS Spec
  //
  PciERWORegInit (PchPolicy);

  FixPcieLanesPowerGating ();

  DEBUG ((DEBUG_INFO, "PchInitRootPorts() End\n"));

  return EFI_SUCCESS;
}

/**
  Perform Root Port Initialization.

  @param[in]  PortIndex               The root port to be initialized (zero based)
  @param[in]  PchPolicy               The PCH Policy PPI
  @param[in]  PmcBaseAddress          The Power Management Controller Base Address
  @param[in]  ABase                   The PM I/O Base address of the PCH
  @param[in]  TempPciBusMin           The temporary minimum Bus number for root port initialization
  @param[in]  TempPciBusMax           The temporary maximum Bus number for root port initialization

  @retval EFI_SUCCESS             Device found. The root port must be enabled.
  @retval EFI_NOT_FOUND           No device is found on the root port. It may be disabled.
  @exception EFI_UNSUPPORTED      Unsupported operation.
**/
EFI_STATUS
PchInitSingleRootPort (
  IN  UINT8                                     PortIndex,
  IN  CONST PCH_POLICY_PPI                      *PchPolicy,
  IN  UINTN                                     PmcBaseAddress,
  IN  UINT16                                    ABase,
  IN  UINT8                                     TempPciBusMin,
  IN  UINT8                                     TempPciBusMax,
  OUT BOOLEAN                                   *Gen3DeviceFound
  )
{
  EFI_STATUS                        Status;
  UINTN                             RpDevice;
  UINTN                             RpFunction;
  UINTN                             RpBase;
  UINT8                             BusNumber;
  UINT32                            Data32Or;
  UINT32                            Data32And;
  UINT16                            Data16;
  UINT16                            Data16Or;
  UINT16                            Data16And;
  UINT8                             Data8Or;
  UINT8                             Data8And;
  CONST PCH_PCIE_ROOT_PORT_CONFIG   *RootPortConfig;
  BOOLEAN                           DeviceFound;
  PCH_SERIES                        PchSeries;
  PCH_STEPPING                      PchStep;
  UINT32                            Tls;
  PCIE_DEVICE_INFO                  DevInfo;
  UINT32                            Uptp;
  UINT32                            Dptp;

  PchSeries       = GetPchSeries ();
  PchStep         = PchStepping ();
  RootPortConfig  = &PchPolicy->PcieConfig.RootPort[PortIndex];
  BusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH;


  DEBUG ((DEBUG_INFO, "PchInitSingleRootPort (%d) Start \n", PortIndex + 1));
  GetPchPcieRpDevFun (PortIndex, &RpDevice, &RpFunction);
  RpBase = MmPciBase (BusNumber, RpDevice, RpFunction);

  Tls = MmioRead16 (RpBase + R_PCH_PCIE_LCTL2) & B_PCIE_LCTL2_TLS;

  DEBUG ((DEBUG_INFO, "LSTP: %d\n", (MmioRead32 (RpBase + R_PCH_PCIE_MPC2) & B_PCH_PCIE_MPC2_LSTP) != 0));
  DEBUG ((DEBUG_INFO, "TLS: %d\n",  Tls));
  DEBUG ((DEBUG_INFO, "CLS: %d\n",  GetLinkSpeed (RpBase)));

  //
  // Make sure TLS is 1 before any link retrain initated by host
  //
  MmioAndThenOr16 (RpBase + R_PCH_PCIE_LCTL2, (UINT16) ~B_PCIE_LCTL2_TLS, V_PCIE_LCTL2_TLS_GEN1);

  //
  // Since root port TLS is persistent device may initiate Gen3 training after reset or S3.
  // This is not observed with regular devices but some cards will proceed with Gen3.
  // Since EQ settings are not configured at this point link may not be stable, therefore test for
  // CLS or DID is not reliable. If TLS > 2 perform Link Disable w/a.
  //
  if (Tls > 2 && (MmioRead16 (RpBase + R_PCH_PCIE_SLSTS) & B_PCIE_SLSTS_PDS)) {
    if (ForceGen1 (RpBase) == FALSE)
        return EFI_ABORTED;
    DEBUG ((DEBUG_INFO, "TLS: %d\n",  MmioRead16 (RpBase + R_PCH_PCIE_LCTL2) & B_PCIE_LCTL2_TLS));
    DEBUG ((DEBUG_INFO, "CLS: %d\n",  GetLinkSpeed (RpBase)));
  }

  DeviceFound = GetDeviceInfo (RpBase, TempPciBusMin, &DevInfo);  
  *Gen3DeviceFound = (DeviceFound && (DevInfo.MaxLinkSpeed >= 3));

  if ( NeedDecreasedDeEmphasis (DevInfo)) {
    MmioOr32 (RpBase + R_PCH_PCIE_LCTL2, B_PCIE_LCTL2_SD);
  }


  ///
  /// If only 128B max payload is supported set CCFG.UNRS to 0.
  ///
  /// If peer writes are supported set max payload size supported to 128B, clear CCFG.UPMWPD
  /// and  program all the PCH Root Ports such that upstream posted writes and upstream non-posted requests
  /// are split at 128B boundary by setting CCFG fields: UPSD to 0, CCFG.UPRS to 000b and UNSD to 0, UNRS to 000b
  ///
  Data32And = ~0u;
  Data32Or  = 0;
  if (RootPortConfig->MaxPayload == PchPcieMaxPayload128 || 
      PchPolicy->PcieConfig.EnablePeerMemoryWrite) {

    MmioAnd16 (RpBase + R_PCH_PCIE_DCAP, (UINT16) ~B_PCIE_DCAP_MPS);

    Data32And &= (UINT32) ~(B_PCH_PCIE_CCFG_UNSD | B_PCH_PCIE_CCFG_UNRS);
    Data32Or  |= (UINT32)  (V_PCH_PCIE_CCFG_UNRS_128B << N_PCH_PCIE_CCFG_UNRS);

    if (PchPolicy->PcieConfig.EnablePeerMemoryWrite) {
      Data32And &= (UINT32) ~(B_PCH_PCIE_CCFG_UPMWPD |
                              B_PCH_PCIE_CCFG_UPSD | B_PCH_PCIE_CCFG_UPRS);
      Data32Or  |= (UINT32)  (V_PCH_PCIE_CCFG_UPRS_128B << N_PCH_PCIE_CCFG_UPRS);
    }
  }
  ASSERT (RootPortConfig->MaxPayload < PchPcieMaxPayloadMax);

  ///
  /// PCH BIOS Spec Section 8.15.1 Power Optimizer Configuration
  /// If B0:Dxx:Fn + 400h is programmed, BIOS will also program B0:Dxx:Fn + 404h [1:0] = 11b,
  /// to enable these override values.
  /// - Fn refers to the function number of the root port that has a device attached to it.
  /// - Default override value for B0:Dxx:Fn + 400h should be 880F880Fh
  /// - Also set 404h[2] to lock down the configuration
  /// - Refer to table below for the 404h[3] policy bit behavior.
  /// Done in PcieSetPm()
  ///
  /// PCH BIOS Spec Section 8.15.1 Power Optimizer Configuration
  /// Program B0:Dxx:Fn + 64h [11] = 1b
  ///
  Data32Or = 0;
  Data32And = ~0u;
  if (RootPortConfig->LtrEnable == TRUE) {
    Data32Or |= B_PCIE_DCAP2_LTRMS;
  } else {
    Data32And &= (UINT32) ~(B_PCIE_DCAP2_LTRMS);
  }
  ///
  /// PCH BIOS Spec Section 8.15.1 Power Optimizer Configuration
  /// Optimized Buffer Flush/Fill (OBFF) is not supported.
  /// Program B0:Dxx:Fn + 64h [19:18] = 0h
  ///
  Data32And &= (UINT32) ~B_PCIE_DCAP2_OBFFS;
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_DCAP2, Data32And, Data32Or);
  ///
  /// PCH BIOS Spec Section 8.15.1 Power Optimizer Configuration
  /// Program B0:Dxx:Fn + 68h [10] = 1b
  ///
  Data16 = MmioRead16 (RpBase + R_PCH_PCIE_DCTL2);
  if (RootPortConfig->LtrEnable == TRUE) {
    Data16 |= B_PCIE_DCTL2_LTREN;
  } else {
    Data16 &= (UINT16) ~(B_PCIE_DCTL2_LTREN);
  }
  MmioWrite16 (RpBase + R_PCH_PCIE_DCTL2, Data16);

  ///
  /// PCH BIOS Spec Section 8.15.1 Power Optimizer Configuration
  /// Step 3 done in PchPciExpressHelpersLibrary.c ConfigureLtr
  ///

  ///
  /// Set Dxx:Fn + 300h[23:00] = 0B75FA7h
  /// Set Dxx:Fn + 304h[11:00] = 0C97h
  ///
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_PCIERTP1, ~0x00FFFFFFu, 0x00B75FA7);
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_PCIERTP2, ~0x00000FFFu, 0x00000C97);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Set Dxx:Fn + 318h [31:16] = 1414h (Gen2 and Gen1 Active State L0s Preparation Latency)
  ///
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_PCIEL0SC, ~0xFFFF0000u, 0x14140000);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// 1.  Program Dxx:Fn + E8h[20, 1] to [1, 1]
  ///
  MmioOr32 (RpBase + R_PCH_PCIE_PWRCTL, B_PCH_PCIE_PWRCTL_LTSSMRTC |
                                        B_PCH_PCIE_PWRCTL_RPL1SQPOL);
  ///
  /// 2.  Program Dxx:Fn + 320h[27, 30] to [1,1]
  /// Enable PCIe Relaxed Order to always allow downstream completions to pass posted writes,
  /// 3.  Set B0:Dxx:Fn:320h[24:23] = 11b
  /// Set PME timeout to 10ms, by
  /// 4.  Set B0:Dxx:Fn:320h[21:20] = 01b
  ///

  Data32And = (UINT32) ~B_PCH_PCIE_PCIECFG2_PMET;
  Data32Or  = B_PCH_PCIE_PCIECFG2_LBWSSTE |
              B_PCH_PCIE_PCIECFG2_RLLG3R |
              B_PCH_PCIE_PCIECFG2_CROAOV |
              B_PCH_PCIE_PCIECFG2_CROAOE |
              (V_PCH_PCIE_PCIECFG2_PMET << N_PCH_PCIE_PCIECFG2_PMET);

  MmioAndThenOr32 (RpBase + R_PCH_PCIE_PCIECFG2, Data32And, Data32Or);

  ///
  /// Enable squelch by programming Dxx:Fn + 324h[25, 24, 5] to [0, 0, 1]
  /// For Gen3 device enable squelch in L0 by setting Dxx:Fn + 324h[7] to 1
  /// Enable Completion Time-Out Non-Fatal Advisory Error, Dxx:Fn + 324h[14] = 1b
  ///
  Data32And = (UINT32) ~(B_PCH_PCIE_PCIEDBG_LGCLKSQEXITDBTIMERS | B_PCH_PCIE_PCIEDBG_SQOL0);

#if SmcOptimize_SUPPORT
{
  INTEL_SETUP_DATA	IntelSetupData;
  DEBUG((-1, "SmcOptimize override\n"));
  GetEntireConfig (&IntelSetupData);
  if (IntelSetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == SMC_OPTIMIZE_SPECPOWER /*Max Power Efficient*/ && 
      IntelSetupData.SocketConfig.PowerManagementConfig.CpuPm == 2 /*custom*/&&
      IntelSetupData.SocketConfig.PowerManagementConfig.PwrPerfTuning == 1 /*BIOS*/ ) {
      DEBUG((-1, "1\n"));
      Data32Or  = B_PCH_PCIE_PCIEDBG_CTONFAE | B_PCH_PCIE_PCIEDBG_SPCE | B_PCH_PCIE_DMIL1EDM;  //SMC SpecPower
  }
  else {
    Data32Or  = B_PCH_PCIE_PCIEDBG_CTONFAE | B_PCH_PCIE_PCIEDBG_SPCE;
  }
}
#else
  Data32Or  = B_PCH_PCIE_PCIEDBG_CTONFAE | B_PCH_PCIE_PCIEDBG_SPCE;
#endif

  if (*Gen3DeviceFound) {
    Data32Or |= B_PCH_PCIE_PCIEDBG_SQOL0;
  }
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_PCIEDBG, Data32And, Data32Or);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// If Dxx:Fn + F5h[0] = 1b or step 3 is TRUE, set Dxx:Fn + 4Ch[17:15] = 4h
  /// Else set Dxx:Fn + 4Ch[17:15] = 010b
  ///
  Data32And = (UINT32) (~B_PCIE_LCAP_EL1);
  Data32Or = 4 << N_PCIE_LCAP_EL1;

  ///
  /// Set LCAP APMS according to platform policy.
  ///
  if (RootPortConfig->Aspm < PchPcieAspmAutoConfig) {
    Data32And &= (UINT32) ~B_PCIE_LCAP_APMS;
    Data32Or  |= RootPortConfig->Aspm << N_PCIE_LCAP_APMS;
  }
  //
  // For PCH LP steppings Bx and PCH H Bx/Cx if L1 capability is enabled L0s capability must be enabled
  // as well otherwise L1 will not be functional.
  // Since L0s must be disabled for the same steppings it will be achieved by disabling OS Native ASPM
  // and never enabling ASPM L0s.
  //
  if (((PchSeries == PchLp) && (PchStep < PchLpC0)) ||
      ((PchSeries == PchH)  && (PchStep < PchHD0))) {
    //
    // If only L1 is enabled by policy force L0s capability to enabled.
    //
    if (RootPortConfig->Aspm == PchPcieAspmL1) {
      Data32Or |= B_PCIE_LCAP_APMS_L0S;
    }
  } else {
    //
    // Force disable L0s for Gen3 devices
    //
      Data32And &= (UINT32) ~B_PCIE_LCAP_APMS_L0S;
      Data32Or  &= (UINT32) ~B_PCIE_LCAP_APMS_L0S;
  }
  //
  // The EL1, ASPMOC and APMS of LCAP are RWO, must program all together.
  //
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_LCAP, Data32And, Data32Or);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Configure PCI Express Number of Fast Training Sequence for each port
  /// 1.  Set Dxx:Fn + 314h [31:24, 23:16, 15:8, 7:0] to [7Eh, 70h, 3Fh, 38h]
  /// 2.  Set Dxx:Fn + 478h [31:24, 15:8, 7:0] to [28h, 3Dh, 2Ch]
  ///
  MmioWrite32 (RpBase + R_PCH_PCIE_PCIENFTS, 0x7E703F38);
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_G3L0SCTL, ~0xFF00FFFFu, 0x28003D2C);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Set Common Clock Exit Latency,      Dxx:Fn + D8h[17:15] = 4h
  /// Set Non-common Clock Exit Latency,  Dxx:Fn + D8h[20:18] = 7h
  ///
  Data32And =  ~(UINT32) (B_PCH_PCIE_MPC_UCEL |
                          B_PCH_PCIE_MPC_CCEL);
  Data32Or  = (7 << N_PCH_PCIE_MPC_UCEL) |
              (4<< N_PCH_PCIE_MPC_CCEL);
  switch (RootPortConfig->PcieSpeed) {
  case PchPcieGen1:
    Data32Or |= (V_PCH_PCIE_MPC_PCIESD_GEN1 << N_PCH_PCIE_MPC_PCIESD);
    break;
  case PchPcieGen2:
    Data32Or |= (V_PCH_PCIE_MPC_PCIESD_GEN2 << N_PCH_PCIE_MPC_PCIESD);
    break;
  case PchPcieGen3:
  case PchPcieAuto:
    break;
  }
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_MPC, Data32And, Data32Or);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Set MPC.IRRCE, Dxx:Fn + D8h[25] = 1b using byte access
  /// For system that support MCTP over PCIE set
  /// Set PCIE RP PCI offset D8h[27] = 1b
  /// Set PCIE RP PCI offset D8h[3] = 1b
  ///
#if SMCPKG == 0
  if (PchPolicy->PcieConfig.VTdSupport == FALSE) {
#endif
    Data8And = (UINT8) (~(B_PCH_PCIE_MPC_IRRCE | B_PCH_PCIE_MPC_MMBNCE) >> 24);
    Data8Or = (B_PCH_PCIE_MPC_IRRCE | B_PCH_PCIE_MPC_MMBNCE) >> 24;
    MmioAndThenOr8 (RpBase + R_PCH_PCIE_MPC + 3, Data8And, Data8Or);
#if SMCPKG == 0
  }
#endif  
  Data8And = (UINT8) ~(B_PCH_PCIE_MPC_MCTPSE);
  Data8Or  = B_PCH_PCIE_MPC_MCTPSE ;
  MmioAndThenOr8 (RpBase + R_PCH_PCIE_MPC, Data8And, Data8Or);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Set PCIE RP PCI offset F5h[7:4] = 0000b
  ///
  MmioAnd8 (RpBase + R_PCH_PCIE_PHYCTL2, (UINT8) ~(B_PCH_PCIE_PHYCTL2_TDFT | B_PCH_PCIE_PHYCTL2_TXCFGCHGWAIT));

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Enable PME_TO Time-Out Policy, Dxx:Fn + E2h[6] =1b
  ///
  MmioOr8 (RpBase + R_PCH_PCIE_RPPGEN, B_PCH_PCIE_RPPGEN_PTOTOP);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Configure Transmitter Preset for each Upstream and Downstream Port Lane:
  /// 1.  Set L01EC.DPL0TP, Dxx:Fn + 22Ch[3:0]    = 7
  /// 2.  Set L01EC.UPL0TP, Dxx:Fn + 22Ch[11:8]   = 5
  /// 3.  Set L01EC.DPL1TP, Dxx:Fn + 22Ch[19:16]  = 7
  /// 4.  Set L01EC.UPL1TP, Dxx:Fn + 22Ch[27:24]  = 5
  /// 5.  Set L23EC.DPL2TP, Dxx:Fn + 230h[3:0]    = 7
  /// 6.  Set L23EC.UPL2TP, Dxx:Fn + 230h[11:8]   = 5
  /// 7.  Set L23EC.DPL3TP, Dxx:Fn + 230h[19:16]  = 7
  /// 8.  Set L23EC.UPL3TP, Dxx:Fn + 230h[27:24]  = 5
  ///
  Uptp = RootPortConfig->Uptp;
  Dptp = RootPortConfig->Dptp;
  Data32And = (UINT32) ~(B_PCIE_EX_L01EC_UPL1TP | B_PCIE_EX_L01EC_DPL1TP | B_PCIE_EX_L01EC_UPL0TP | B_PCIE_EX_L01EC_DPL0TP);
  Data32Or = ((Uptp << N_PCIE_EX_L01EC_UPL1TP) |
              (Dptp << N_PCIE_EX_L01EC_DPL1TP) |
              (Uptp << N_PCIE_EX_L01EC_UPL0TP) |
              (Dptp << N_PCIE_EX_L01EC_DPL0TP));
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_EX_L01EC, Data32And, Data32Or);

  Data32And = (UINT32) ~(B_PCIE_EX_L23EC_UPL3TP | B_PCIE_EX_L23EC_DPL3TP | B_PCIE_EX_L23EC_UPL2TP | B_PCIE_EX_L23EC_DPL2TP);
  Data32Or = ((Uptp << N_PCIE_EX_L23EC_UPL3TP) |
              (Dptp << N_PCIE_EX_L23EC_DPL3TP) |
              (Uptp << N_PCIE_EX_L23EC_UPL2TP) |
              (Dptp << N_PCIE_EX_L23EC_DPL2TP));
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_EX_L23EC, Data32And, Data32Or);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Enable EQ TS2 in Recovery Receiver Config, Dxx:Fn + 450h[7]= 1b
  ///
  MmioOr32 (RpBase + R_PCH_PCIE_EQCFG1, B_PCH_PCIE_EQCFG1_EQTS2IRRC);

  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// If there is no IOAPIC behind the root port, set EOI Forwarding Disable bit (PCIE RP PCI offset D4h[1]) to 1b.
  /// Done in PchPciExpressHelpersLibrary.c PcieSetEoiFwdDisable ()
  /// For Empty Hot Plug Slot, set is done in PchInitSingleRootPort ()
  ///

  ///
  /// System bios should initiate link retrain for all slots that has card populated after register restoration.
  /// Done in PchPciExpressHelpersLibrary.c PchPcieInitRootPortDownstreamDevices ()
  ///

  ///
  /// Configure Extended Synch
  ///
  if (RootPortConfig->ExtSync) {
    Data16And = (UINT16) ~0;
    Data16Or  = B_PCIE_LCTL_ES;
  } else {
    Data16And = (UINT16) (~B_PCIE_LCTL_ES);
    Data16Or  = 0;
  }
  MmioAndThenOr16 (RpBase + R_PCH_PCIE_LCTL, Data16And, Data16Or);

  ///
  /// Configure Completion Timeout
  ///
  Data16And = (UINT16)~(B_PCIE_DCTL2_CTD | B_PCIE_DCTL2_CTV);
  Data16Or  = 0;
  if (RootPortConfig->CompletionTimeout == PchPcieCompletionTO_Disabled) {
    Data16Or = B_PCIE_DCTL2_CTD;
  } else {
    switch (RootPortConfig->CompletionTimeout) {
    case PchPcieCompletionTO_Default:
      Data16Or = V_PCIE_DCTL2_CTV_DEFAULT;
      break;

    case PchPcieCompletionTO_16_55ms:
      Data16Or = V_PCIE_DCTL2_CTV_40MS_50MS;
      break;

    case PchPcieCompletionTO_65_210ms:
      Data16Or = V_PCIE_DCTL2_CTV_160MS_170MS;
      break;

    case PchPcieCompletionTO_260_900ms:
      Data16Or = V_PCIE_DCTL2_CTV_400MS_500MS;
      break;

    case PchPcieCompletionTO_1_3P5s:
      Data16Or = V_PCIE_DCTL2_CTV_1P6S_1P7S;
      break;

    default:
      Data16Or = 0;
      break;
    }
  }

  MmioAndThenOr16 (RpBase + R_PCH_PCIE_DCTL2, Data16And, Data16Or);

  ///
  /// Slot Implemented enabled earlier. Here will only save this register for enabled ports
  ///
  Data16Or  = BIT8;
  Data16And = 0xFFFF;
  ///
  /// For Root Port Slots Numbering on the CRBs.
  ///
  Data32Or  = 0;
  Data32And = (UINT32) (~(B_PCIE_SLCAP_SLV | B_PCIE_SLCAP_SLS | B_PCIE_SLCAP_PSN));
  ///
  /// PCH BIOS Spec section 8.8.2.1
  /// Note: If Hot Plug is supported, then write a 1 to the Hot Plug Capable (bit6) and Hot Plug
  /// Surprise (bit5) in the Slot Capabilities register, PCIE RP PCI offset 54h. Otherwise,
  /// write 0 to the bits PCIe Hot Plug SCI Enable
  ///
  Data32And &= (UINT32) (~(B_PCIE_SLCAP_HPC | B_PCIE_SLCAP_HPS));
  if (RootPortConfig->HotPlug) {
    Data32Or |= B_PCIE_SLCAP_HPC | B_PCIE_SLCAP_HPS;
  }
  ///
  /// Get the width from LCAP
  /// Slot Type  X1  X2/X4/X8/X16
  /// Default     10W   25W
  /// The slot power consumption and allocation is platform specific. Please refer to the
  /// "PCI Express* Card Electromechanical (CEM) Spec" for details.
  ///
  if (RootPortConfig->SlotPowerLimitValue != 0) {
      Data32Or |= (UINT32) (RootPortConfig->SlotPowerLimitValue << 7);
      Data32Or |= (UINT32) (RootPortConfig->SlotPowerLimitScale << 15);
  } else {
    if ((((MmioRead32 (RpBase + R_PCH_PCIE_LCAP)) & B_PCIE_LCAP_MLW) >> N_PCIE_LCAP_MLW) == 0x01) {
      Data32Or |= (UINT32) (100 << 7);
      Data32Or |= (UINT32) (1 << 15);
    } else if ((((MmioRead32 (RpBase + R_PCH_PCIE_LCAP)) & B_PCIE_LCAP_MLW) >> N_PCIE_LCAP_MLW) >= 0x02) {
      Data32Or |= (UINT32) (250 << 7);
      Data32Or |= (UINT32) (1 << 15);
    }
  }

  ///
  /// PCH BIOS Spec section 8.2.4
  /// Initialize Physical Slot Number for Root Ports
  ///
  Data32Or |= (UINT32) (RootPortConfig->PhysicalSlotNumber << 19);
  MmioAndThenOr32 (RpBase + R_PCH_PCIE_SLCAP, Data32And, Data32Or);

  InitCapabilityList (PortIndex, PchPolicy);

  //
  // As per SV request 4926630 - add menu option to control MPL supported
  //
  if (!RootPortConfig->HotPlug) {
    switch (PchPolicy->PcieConfig.RootPort[PortIndex].MaxPayload) {
    case PchPcieMaxPayLoad128B:
      MmioAnd16 (RpBase + R_PCH_PCIE_DCAP, (UINT16) ~B_PCIE_DCAP_MPS);
      break;
    case PchPcieMaxPayLoad256B:
      Data16And = (UINT16)~(BIT2 | BIT1);
      Data16Or  = BIT0;
      MmioAndThenOr16 (RpBase + R_PCH_PCIE_DCAP, Data16And, Data16Or);
      break;
    case PchPcieMaxPayLoad512B:
      Data16And = (UINT16)~(BIT2 | BIT0);
      Data16Or  = BIT1;
      MmioAndThenOr16 (RpBase + R_PCH_PCIE_DCAP, Data16And, Data16Or);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "PchInitSingleRootPort(): Unsupported Max Payload Size! \n"));
    }
  }
  ///
  /// Initialize downstream devices
  ///
  Status = PchPcieInitRootPortDownstreamDevices (
             BusNumber,
             (UINT8) RpDevice,
             (UINT8) RpFunction,
             TempPciBusMin,
             TempPciBusMax,
             PchPolicy->PcieConfig.MaxReadRequestSize
           );
  if (Status == EFI_SUCCESS) {
    DeviceFound = TRUE;
  }
  ///
  /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
  /// Set "Poisoned TLP Non-Fatal Advisory Error Enable", Dxx:Fn + D4h[12] to 1
  ///
  /// Disable the forwarding of EOI messages if no device is present.
  /// Set PCIE RP PCI offset D4h [1] = 1b
  ///
  Data32Or = B_PCH_PCIE_MPC2_PTNFAE;
  if (!DeviceFound) {
    Data32Or |= B_PCH_PCIE_MPC2_EOIFD;
  }
  MmioOr32 (RpBase + R_PCH_PCIE_MPC2, Data32Or);
  ///
  /// Additional configurations
  ///
  ///
  /// Configure Error Reporting policy in the Device Control Register
  ///
  Data16And = (UINT16) (~(B_PCIE_DCTL_URE | B_PCIE_DCTL_FEE | B_PCIE_DCTL_NFE | B_PCIE_DCTL_CEE));
  Data16Or  = 0;

  if (RootPortConfig->UnsupportedRequestReport) {
    Data16Or |= B_PCIE_DCTL_URE;
  }

  if (RootPortConfig->FatalErrorReport) {
    Data16Or |= B_PCIE_DCTL_FEE;
  }

  if (RootPortConfig->NoFatalErrorReport) {
    Data16Or |= B_PCIE_DCTL_NFE;
  }

  if (RootPortConfig->CorrectableErrorReport) {
    Data16Or |= B_PCIE_DCTL_CEE;
  }

  MmioAndThenOr16 (RpBase + R_PCH_PCIE_DCTL, Data16And, Data16Or);

  ///
  /// Configure Interrupt / Error reporting in R_PCH_PCIE_RCTL
  ///
  Data16And = (UINT16) (~(B_PCIE_RCTL_SFE | B_PCIE_RCTL_SNE | B_PCIE_RCTL_SCE));
  Data16Or  = 0;

  if (RootPortConfig->SystemErrorOnFatalError) {
    Data16Or |= B_PCIE_RCTL_SFE;
  }

  if (RootPortConfig->SystemErrorOnNonFatalError) {
    Data16Or |= B_PCIE_RCTL_SNE;
  }

  if (RootPortConfig->SystemErrorOnCorrectableError) {
    Data16Or |= B_PCIE_RCTL_SCE;
  }

  MmioAndThenOr16 (RpBase + R_PCH_PCIE_RCTL, Data16And, Data16Or);

  ///
  /// Root PCI-E Powermanagement SCI Enable
  ///
  if (RootPortConfig->PmSci) {
    ///
    /// PCH BIOS Spec section 8.7.3 BIOS Enabling of Intel PCH PCI Express* PME SCI Generation
    /// Step 1
    /// Make sure that PME Interrupt Enable bit, Dxx:Fn:Reg 5Ch[3] is cleared
    ///
    Data16And = (UINT16) (~B_PCIE_RCTL_PIE);
    Data16Or  = 0;
    MmioAnd16 (RpBase + R_PCH_PCIE_RCTL, Data16And);

    ///
    /// Step 2
    /// Program Misc Port Config (MPC) register at PCI config space offset
    /// D8h as follows:
    /// Set Power Management SCI Enable bit, Dxx:Fn:Reg D8h[31]
    /// Clear Power Management SMI Enable bit, Dxx:Fn:Reg D8h[0]
    /// Use Byte Access to avoid RWO bit
    ///
    MmioAnd8 (RpBase + R_PCH_PCIE_MPC, (UINT8) (~B_PCH_PCIE_MPC_PMME));
    MmioOr8 ((RpBase + R_PCH_PCIE_MPC + 3), (UINT8) (B_PCH_PCIE_MPC_PMCE >> 24));

    ///
    /// Step 3
    /// Make sure GPE0 Register (ABase+8Ch[9]), PCI_EXP_STS is 0, clear it if not zero
    ///
    Data32Or = IoRead32 (ABase + R_PCH_ACPI_GPE0_STS_127_96);
    if ((Data32Or & B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP) != 0) {
      Data32Or = B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP;
      IoWrite32 (ABase + R_PCH_ACPI_GPE0_STS_127_96, Data32Or);
    }
    ///
    /// Step 4
    /// Set BIOS_PCI_EXP_EN bit, PMC PCI offset A0h[10],
    /// to globally enable the setting of the PCI_EXP_STS bit by a PCI Express* PME event.
    ///
    MmioOr8 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_A + 1, B_PCH_PMC_GEN_PMCON_A_BIOS_PCI_EXP_EN >> 8);
  }
  if (RootPortConfig->HotPlug) {
    ///
    /// PCH BIOS Spec section 8.8.2.1
    /// Step 1
    /// Clear following status bits, by writing 1b to them, in the Slot
    /// Status register at offset 1Ah of PCI Express Capability structure:
    /// Presence Detect Changed (bit3)
    ///
    Data16Or  = B_PCIE_SLSTS_PDC;
    MmioOr16 (RpBase + R_PCH_PCIE_SLSTS, Data16Or);
    ///
    /// Step 2
    /// Program the following bits in Slot Control register at offset 18h
    /// of PCI Express* Capability structure:
    /// Presence Detect Changed Enable (bit3) = 1b
    /// Hot Plug Interrupt Enable (bit5) = 0b
    ///
    Data16And = (UINT16) (~B_PCIE_SLCTL_HPE);
    Data16Or  = B_PCIE_SLCTL_PDE;
    MmioAndThenOr16 (RpBase + R_PCH_PCIE_SLCTL, Data16And, Data16Or);
    ///
    /// Step 3
    /// Program Misc Port Config (MPC) register at PCI config space offset
    /// D8h as follows:
    /// Hot Plug SCI Enable (HPCE, bit30) = 1b
    /// Hot Plug SMI Enable (HPME, bit1) = 0b
    ///
    Data32And = (UINT32) (~B_PCH_PCIE_MPC_HPME);
    Data32Or  = B_PCH_PCIE_MPC_HPCE;
    MmioAndThenOr32 (RpBase + R_PCH_PCIE_MPC, Data32And, Data32Or);
    ///
    /// Step 4
    /// Clear GPE0 Register HOT_PLUG_STS by writing 1
    ///
    IoWrite32 (ABase + R_PCH_ACPI_GPE0_STS_127_96, (UINT32) B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG);

    ///
    /// PCH BIOS Spec section 8.9
    /// BIOS should mask the reporting of Completion timeout (CT) errors byerrors by setting
    /// the uncorrectable Error Mask register PCIE RP PCI offset 108h[14].
    ///
    MmioOr32 (RpBase + R_PCH_PCIE_EX_UEM, B_PCIE_EX_UEM_CT);
  }
  DEBUG ((DEBUG_INFO, "PchInitSingleRootPort () End \n"));
  if (DeviceFound == TRUE || (RootPortConfig->HotPlug == TRUE)) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
PchSetLinkWidth(
  IN  PCH_POLICY_PPI  *PchPolicyPpi
)
{
  UINTN                             RpDevice;
  UINTN                             RpFunction;
  UINTN                             RpBase;
  UINT8                             RootPortMLW;
  UINT8                             RootPortNLW;
  UINT32                            LoopTime;
  BOOLEAN                           WALinkDisabled = FALSE;
  BOOLEAN                           WALinkInDetect = FALSE;
  UINT32                            LinkState;
  UINT16                            SlotStatus;
  UINT8                             PortIndex;
  UINT8                             MaxPciePortNum;
  BOOLEAN                           DeviceFound;
  PCIE_DEVICE_INFO                  DevInfo;
  UINT8                             TempPciBusMin;

  if (PchIsDwrFlow()) {
    DEBUG ((DEBUG_INFO, "DWR: PchSetLinkWidth() End\n"));
    return EFI_SUCCESS;
  }

  MaxPciePortNum   = GetPchMaxPciePortNum ();
  TempPciBusMin =  PchPolicyPpi->TempPciBusMin;

  for (PortIndex = 0; PortIndex < MaxPciePortNum; ++PortIndex) {
    DEBUG ((DEBUG_INFO, "PchSetLinkWidth (%d) \n", PortIndex + 1));
    GetPchPcieRpDevFun (PortIndex, &RpDevice, &RpFunction);
    RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, RpDevice, RpFunction);
    DeviceFound = GetDeviceInfo (RpBase, TempPciBusMin, &DevInfo);
    if (DeviceFound) {
      //
      // s5330590 - LBG Si WA for low Rx off state impedance on PCIE PHY
      // WA shall not be applied for BMC connected on board to one PCIe Lane
      //
      RootPortMLW = MIN(DevInfo.MaxLinkWidth, GetMaxLinkWidth(RpBase));
      RootPortNLW = GetNegotiatedLinkWidth(RpBase);
      if ((RootPortMLW != RootPortNLW) && ((DevInfo.Vid != 0x19A2) || (DevInfo.Did != 0x0120))) {
        //
        //Applying WA
        //
        DEBUG ((DEBUG_INFO, "WARNING! Applying LBG Si WA for low Rx off state impedance...\n"));
        //
        // Set Link Disable
        //
        DEBUG ((DEBUG_INFO, "Disabling the link...\n"));
        MmioOr16 ((RpBase + R_PCH_PCIE_LCTL), (UINT16) B_PCIE_LCTL_LD);
        //
        // Wait for the link to be disabled
        //
        for (LoopTime = 0; LoopTime < 100; LoopTime++){
          LinkState = GetLinkState (RpBase);
          if ((LinkState == V_PCH_PCIE_PCIESTS1_LTSMSTATE_DISWAIT) ||
               (LinkState == V_PCH_PCIE_PCIESTS1_LTSMSTATE_DISWAITPG)){
            WALinkDisabled = TRUE;
            break;
          } else {
            MicroSecondDelay (10);
          }
        }
        //
        // Enable the link again
        //
        MmioAnd8 (RpBase + R_PCH_PCIE_LCTL, (UINT8) ~(B_PCIE_LCTL_LD));
        if (WALinkDisabled == TRUE){
          //
          // The link was disabled and now is reenabled. Wait for the link's LTSSM to be in Detect.Ready
          //
          for (LoopTime = 0; LoopTime < 100; LoopTime++){
            LinkState = GetLinkState (RpBase);
            if (LinkState == V_PCH_PCIE_PCIESTS1_LTSMSTATE_DETRDY){
              WALinkInDetect = TRUE;
              break;
            } else {
              MicroSecondDelay (10);
            }
          }
          if (WALinkInDetect == TRUE) {
            //
            // Wait for the device to be re detected again and device info again
            //
            for (LoopTime = 0; LoopTime < 1000; LoopTime++){
              SlotStatus = MmioRead16 (RpBase + R_PCH_PCIE_SLSTS);
              if ((SlotStatus & B_PCIE_SLSTS_PDS) != 0) {
                DeviceFound = GetDeviceInfo (RpBase, TempPciBusMin, &DevInfo);
                break;
              } else {
                MicroSecondDelay (50);
              }
            }
          } else {
            DEBUG ((DEBUG_ERROR, "The link's LTSSM can't enter DETECT state! The WA can't be applied\n"));
          }
        } else {
          DEBUG ((DEBUG_ERROR, "The link could not be disabled! The WA can't be applied\n"));
        }
        if (DeviceFound == FALSE) {
          DEBUG ((DEBUG_ERROR, "ERROR: During applying the Si WA after re-enbling the link the slot can't go into Presence Detect!\n"));
        }
      }
    }
  }
  return EFI_SUCCESS;
}

/**
  Get BMC RootPort

  @param none

  @return BMC root port number
**/
UINTN
GetBmcRootPort (
  VOID
  )
{
  PCH_POLICY_PPI       *PchPolicy;
  EFI_STATUS            Status;

  Status = PeiServicesLocatePpi (
             &gPchPlatformPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &PchPolicy
             );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: DWR Can't find PCH Policy (Status: %r)\n", Status));
    ASSERT_EFI_ERROR (Status);
    CpuDeadLoop();
  }

  return PchPolicy->PmConfig.Dwr_BmcRootPort;
}

