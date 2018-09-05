//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.05
//      Bug Fixed:  Skip override SVID/SDID to all PCH SATA/SSATA RAID controller.
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Apr/12/2017
//
//  Rev. 1.04
//      Bug Fixed:  Fix issue that SATA controller port 5/6 with HDD connected with RAID 0 mode can't install OS.
//      Reason:     Intel doesn't recommemd override SSVID of RAID controler like Greenlow.
//      Auditor:    William Hsu
//      Date:       Mar/09/2017
//
//  Rev. 1.03
//      Bug Fixed:  Add override DMI settings HSIO settings support.
//      Reason:     
//      Auditor:    Jacker Yeh
//      Date:       Jan/26/2017
//
//  Rev. 1.02
//      Bug Fixed:  Update SMC SVID/SDID to PCH devices.
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Aug/17/2016
//
//  Rev. 1.01
//      Bug Fixed:  Temporary solution for USB 2.0 settings cannot be override problem.
//      Reason:     Need to review it when AMI add the override code in the future.
//      Auditor:    Jacker Yeh
//      Date:       Apr/10/2016
//
//  Rev. 1.00
//      Bug Fixed:  Implment IOBP Override.
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Mar/25/2016
//
//*****************************************************************************
//*****************************************************************************
/** @file
  The PCH Init PEIM implements the PCH PEI Init PPI.

@copyright
  Copyright (c) 2004 - 2017 Intel Corporation. All rights reserved
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

#include "PchInitPei.h"
#include "PchSmbus.h"
#include "PchReset.h"
#include "PchUsb.h"
#include "PchSpi.h"
#include "Wdt.h"
#include "PchInitAdr.h"
#include <Ppi/Dwr.h>
#include <IeHeciRegs.h>
#include <MeChipset.h>
#include <IncludePrivate/PchHsio.h>
#include <Library/PchPcrLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/HeciMsgLib.h>
#include <Ppi/PchPeiInitDone.h>
#include <Ppi/PchInit.h>
#include <IncludePrivate/Library/PchRcLib.h>
#include <Library/PchPmcLib.h>
//SMCPKG_SUPPORT
#include <Platform.h>
#include <Ppi/PchHsioPtssTable.h>
#include "Token.h"
//SMCPKG_SUPPORT

static PCH_INIT_PPI               mPchInitPpi = {
    PchDmiTcVcMapInit,
    PchDmiTcVcProg,
    PchDmiTcVcPoll,
    PchDmiGen2Prog,
    PchDmiGen3Prog,
    PchCpuStrapSet,
    PchCpuEpocSet,

};


/*************************************************************************************
 */
#define V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PSFID_PMT                                    5
#define V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PORTGROUPID_UPSTREAM                         0
#define V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PORTGROUPID_DOWNSTREAM                       1
#define V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PORTID                                       1
#define V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_CHANNELID                                    1

#define V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_PORTGROUPID_UPSTREAM   0
#define V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_PORTGROUPID_DOWNSTREAM 1
#define V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_PSFID_PMTIE            6
#define V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_PORTID_SPA             1
#define V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_PORTID_PMTIE           1
#define V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_CHANNELID_PMTIE        1

#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTGROUPID_UPSTREAM   0
#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTGROUPID_DOWNSTREAM 1

#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTID_DMI             0
#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTID_PMTIE           1
#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_CHANNELID_DMI          0
#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_CHANNELID_PMTIE        1
#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PSFID_SPA              1
#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PSFID_DMI              1
#define V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PSFID_PMTIE            6


/*************************************************************************************
 */
/**
  Internal function performs register programming for MCTP based on the
  input values. This function programs both MCTP owner
  and MCTP target registers as the bits layout for both is identical.

  @param[in] PsfDevAddress - Indicates the PSF Address we are programming
  @param[in] PsfId         - PSF ID
  @param[in] PortGroupId   - Indicates Downstream/Upstream
  @param[in] PortId        - Defines the port to which SPX is attached
  @param[in] ChannelId     - Defines the channel
**/

STATIC
VOID
EFIAPI
PchPsfSetTarget(
  IN  PCH_SBI_PID      Pid,
  IN  UINT16           PsfDevAddress,
  IN  UINT8            PsfId,
  IN  UINT8            PortGroupId,
  IN  UINT8            PortId,
  IN  UINT8            ChannelId)
{
  UINT32                Data32And;
  UINT32                Data32Or;

  PchPcrRead32(Pid, PsfDevAddress, &Data32Or);
  DEBUG((DEBUG_ERROR, "[MCTP] PSF[%X][%X] %08X ->", Pid, PsfDevAddress, Data32Or));

  Data32And = (UINT32)~(B_PCH_PCR_PSF1_TARGET_PSFID |
                        B_PCH_PCR_PSF1_TARGET_PORTGROUPID |
                        B_PCH_PCR_PSF1_TARGET_PORTID |
                        B_PCH_PCR_PSF1_TARGET_CHANNELID);
  Data32Or = (((UINT32) PsfId) << N_PCH_PCR_PSF1_TARGET_PSFID) |
             (((UINT32) PortGroupId) << N_PCH_PCR_PSF1_TARGET_PORTGROUPID) |
             (((UINT32) PortId) << N_PCH_PCR_PSF1_TARGET_PORTID) |
              ChannelId;
  PchPcrAndThenOr32(Pid, PsfDevAddress, Data32And, Data32Or);
  PchPcrRead32(Pid, PsfDevAddress, &Data32Or);
  DEBUG((DEBUG_ERROR, "%08X\n", Data32Or));
}


/**
  Internal function determing MCTP Target register base address and calling another
  function for register programming.

  @param[in] TargetIndex   - Indicates the target index
  @param[in] PsfId         - PSF ID
  @param[in] PortGroupId   - Indicates Downstream/Upstream
  @param[in] PortId        - Defines the port to which SPX is attached
  @param[in] ChannelId     - Defines the channel
**/

STATIC
VOID
EFIAPI
PchPsfConfigureMctpTarget (
  IN  PCH_SBI_PID      Pid,
  IN  UINT8            TargetIndex,
  IN  UINT8            PsfId,
  IN  UINT8            PortGroupId,
  IN  UINT8            PortId,
  IN  UINT8            ChannelId)
{
  UINT16               PsfDevAddress;

  switch (Pid) {
    case PID_PSF1:
      PsfDevAddress = R_PCH_H_PCR_PSF1_PSF_MC_AGENT_MCAST1_RS0_TGT0_MCTP1;
      break;
    case PID_PSF6:
      PsfDevAddress = R_PCH_H_PCR_PSF6_PSF_MC_AGENT_MCAST0_RS0_TGT0_MCTP0;
      break;
    default:
      ASSERT(FALSE);
      return;
  }

  PsfDevAddress += (TargetIndex * 4);

  PchPsfSetTarget (Pid, PsfDevAddress, PsfId, PortGroupId, PortId, ChannelId);
}


/**
  Internal function performs register programming determining MCTP Multicast
  Configuration - enables the communication and determines the number of targets.
  (PSF1 only for the time, this should be changed  in the future  to make the
  function more flexible).

  @param[in] NumberOfTargets - Indicates the number of multicast targets
**/

STATIC
VOID
EFIAPI
PchPsfConfigureMctpControl (
  IN  PCH_SBI_PID       Pid,
  IN  UINT8             NumberOfTargets)
{
  UINT16                PsfDevAddress;
  UINT32                Data32And;
  UINT32                Data32Or;

  switch (Pid) {
    case PID_PSF1:
      PsfDevAddress = R_PCH_H_PCR_PSF1_PSF_MC_CONTROL_MCAST1_RS0_MCTP1;
      break;
    case PID_PSF6:
      PsfDevAddress = R_PCH_H_PCR_PSF6_PSF_MC_CONTROL_MCAST0_RS0_MCTP0;
      break;
    default:
      ASSERT(FALSE);
      return;
  }

  PchPcrRead32(Pid, PsfDevAddress, &Data32Or);
  DEBUG((DEBUG_ERROR, "[MCTP] PSF[%X][%X] %08X ->", Pid, PsfDevAddress, Data32Or));

  Data32And = (UINT32)~(B_PCH_PCR_PSF1_PSF_MC_CONTROL_MCAST1_RS0_MCTP1_NUMMC |
                        B_PCH_PCR_PSF1_PSF_MC_CONTROL_MCAST1_RS0_MCTP1_MULTCEN);
  Data32Or = (NumberOfTargets << N_PCH_PCR_PSF1_PSF_MC_CONTROL_MCAST1_RS0_MCTP1_NUMMC) |
              B_PCH_PCR_PSF1_PSF_MC_CONTROL_MCAST1_RS0_MCTP1_MULTCEN;
  PchPcrAndThenOr32(Pid, PsfDevAddress, Data32And,  Data32Or);
  PchPcrRead32(Pid, PsfDevAddress, &Data32Or);
  DEBUG((DEBUG_ERROR, "%08X\n", Data32Or));
}


/**
  Internal function performing registers programming required for
  Management Component Transport Protocol.

**/
VOID
PchMctpConfigure (VOID)
{
  UINT8               RpIndex;
  UINT8               TargetIndex = 0;
  UINT16              SlotSts;
  UINT16              LinkSts;
  UINTN               BaseAddress;
  ///
  /// PCIe root ports mapping to PSF#1 ports:
  ///  0- 3 -> 0 (SPA)
  ///  4- 7 -> 1 (SPB)
  ///  8-11 -> 3 (SPC)
  /// 12-15 -> 5 (SPD)
  /// 16-19 -> 7 (SPE)
  ///
  static CONST UINT8  RpPsfPortId[] = {0, 1, 3, 5, 7};

  if (PchIsDwrFlow()) {
    DEBUG ((DEBUG_INFO, "DWR: PchMctpConfigure() End\n"));
    return;
  }

  DEBUG((DEBUG_ERROR, "[MCTP] Configure MCTP for server\n"));
  /// SPS specific configuration
  /// Register 0x4008 to value 0x00050101 - PSF-1-RC-OWNER-RS0, PMT (PSF#5, North Port Group, Port#1, Channel#1)
  /// Register 0x40D0 to value 0x00010000 - PSF-1-PSF-MC-AGENT-MCAST1-RS0-TGT0-MCTP1, DMI (PSF#1, North Port Group, Port#0, Channel#0)
  /// Register 0x40D4 to value 0x00060101 -PSF-1-PSF-MC-AGENT-MCAST1-RS0-TGT2-MCTP1, PMT IE (PSF#6, North Port Group, Port#1, Channel#1))
  /// Register 0x40D8 to value 0x00018000 - PSF-1-PSF-MC-AGENT-MCAST1-RS0-TGT1-MCTP1, SPA (PSF#1, South Port Group, Port#0, Channel#0)
  /// Register 0x4060 to value 0x00000007 - PSF-1-PSF-MC-CONTROL-MCAST1-RS0-MCTP1, Enable MCAST with three TARGETS
  /// Register 0x4008 to value 0x00050101 - PSF-6-RC-OWNER-RS0, PMT (PSF#5, North Port Group, Port#1, Channel#1)
  /// Register 0x4050 to value 0x00060101 -PSF-6-PSF-MC-AGENT-MCAST0-RS0-TGT0-MCTP0, PMT IE (PSF#6, North Port Group, Port#1, Channel#1))
  /// Register 0x4044 to value 0x00000003 - PSF-6-PSF-MC-CONTROL-MCAST0-RS0-MCTP0, Enable MCAST with one TARGET

  ///
  /// Configure PSF1 RC Owner - PMT
  ///
  PchPsfSetTarget(PID_PSF1, R_PCH_PCR_PSF1_RC_OWNER_RS0,
                  V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PSFID_PMT,
                  V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PORTGROUPID_UPSTREAM,
                  V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PORTID,
                  V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_CHANNELID);

  ///
  /// Configure Broadcast to DMI
  ///
  PchPsfConfigureMctpTarget(PID_PSF1, TargetIndex++,
                            V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PSFID_DMI,
                            V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTGROUPID_UPSTREAM,
                            V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTID_DMI,
                            V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_CHANNELID_DMI);

  ///
  /// Configure Broadcast for IE
  ///
  PchPsfConfigureMctpTarget(PID_PSF1, TargetIndex++,
                            V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PSFID_PMTIE,
                            V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTGROUPID_UPSTREAM,
                            V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTID_PMTIE,
                            V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_CHANNELID_PMTIE);

  for (RpIndex = 0; RpIndex < GetPchMaxPciePortNum(); RpIndex++)
  {
    BaseAddress = PchPcieBase(RpIndex);
    LinkSts = MmioRead16(BaseAddress + R_PCH_PCIE_LSTS);
    SlotSts = MmioRead16(BaseAddress + R_PCH_PCIE_SLSTS);
    if ((MmioRead32(BaseAddress) != 0xFFFFFFFF) && (LinkSts & B_PCIE_LSTS_LA) && (SlotSts & B_PCIE_SLSTS_PDS))
    {
      DEBUG((DEBUG_ERROR, "[MCTP] Root port %d slot status: detected\n", RpIndex));
      PchPsfConfigureMctpTarget(PID_PSF1, TargetIndex++,
                          V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PSFID_SPA,
                          V_PCH_PCR_PSF1_SPS_PSF_MC_AGENT_MCAST1_RS0_TGTX_MCTP1_PORTGROUPID_DOWNSTREAM,
                          RpPsfPortId[RpIndex / 4], (RpIndex % 4));
    }
  }

  ///
  /// Configure MCTP Multicast Control
  ///
  PchPsfConfigureMctpControl(PID_PSF1, TargetIndex);


  TargetIndex = 0;
  ///
  /// Configure PSF6 RC Owner - PMT
  ///
  PchPsfSetTarget(PID_PSF6, R_PCH_PCR_PSF1_RC_OWNER_RS0,
                  V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PSFID_PMT,
                  V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PORTGROUPID_UPSTREAM,
                  V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_PORTID,
                  V_PCH_PCR_PSF1_SPS_RC_OWNER_RS0_CHANNELID);

  PchPsfConfigureMctpTarget(PID_PSF6, TargetIndex++,
                            V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_PSFID_PMTIE,
                            V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_PORTGROUPID_UPSTREAM,
                            V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_PORTID_PMTIE,
                            V_PCH_PCR_PSF6_SPS_PSF_MC_AGENT_MCAST0_RS0_TGTX_MCTP0_CHANNELID_PMTIE);

  ///
  /// Configure PSF6  MCTP Multicast Control
  ///
  PchPsfConfigureMctpControl(PID_PSF6, TargetIndex);
} // PchMctpConfigure()

#include <PchIntErrors.h>

extern EFI_GUID gPchPsfErrorHobGuid;


static EFI_PEI_PPI_DESCRIPTOR     mPpiListVariable = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPchInitPpiGuid,
  &mPchInitPpi
};

static EFI_PEI_PPI_DESCRIPTOR     mPpiPchPeiInitDone = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPchPeiInitDonePpiGuid,
  NULL
};



//
// Ports Group Number - total in each RSx
//
#define PCH_PSF_ERRORS_MAX_PORT_GROUP_NUMBER       6

//
// PSFx error registers
//
#define V_PCH_PSFX_EHCR_PG_OFFSET                       0x200
#define B_PCH_PSFX_EHCR_MASK                            (BIT2 | BIT1 | BIT0)   ///< Error bits mask
#define V_PCH_PSFX_EHCR_PORT_OFFSET                     0x20                   ///< The offset to be added to reach next port in given RSx and given Port Group

//
// PSF Error Segment
//
typedef struct  {
  UINT8                   Pid; // Port ID of given PSF
  UINT8                   PortsInGroup[PCH_PSF_ERRORS_MAX_PORT_GROUP_NUMBER]; //Number (1-based) of ports in each Port Group in each RSx
  UINT32                  PSFxEhcrRS0PG0Base; //The base offset of all error registers for all groups and all RSs fir given PSFx
} PCH_PSF_ERROR_SEGMENT;

//
// The list of the PSF Segment (PID with other parameters) to check for errors
// In the PortsInGroup table section in the structure below we store how many ports each RSx and PortGroup have
// Like in the example in the first low: RS0, PG0 - 2 ports; RS0, PG1 - 13 ports; RS1, PG0 - 2 ports; RS1, PG1 - 13 ports etc.
//
PCH_PSF_ERROR_SEGMENT mPSFsToCheckForErrors[] = {{PID_PSF1, {2, 13, 2, 13, 2, 13}, 0xA000}, //RS0, PG0 - 2 ports; RS0, PG1 - 13 ports; RS1, PG0 - 2 ports; RS1, PG1 - 13 ports etc.
                                                 {PID_PSF2, {1, 6,  1,  6, 1,  6}, 0xA000},
                                                 {PID_PSF3, {1, 14, 1, 14, 1, 14}, 0xA000},
                                                 {PID_PSF4, {1, 11, 1, 11, 1, 11}, 0xA000},
                                                 {PID_PSF5, {8, 2,  8,  2, 8,  2}, 0xA000},
                                                 {PID_PSF6, {9, 2,  9,  2, 9,  2}, 0xA000},
                                                 {PID_PSF7, {1, 8,  1,  8, 1,  8}, 0xA000},
                                                 {PID_PSF8, {1, 6,  0,  0, 0,  0}, 0xB000},
                                                 {PID_PSF9, {1, 4,  0,  0, 0,  0}, 0xB000},
                                                 {PID_PSF10, {1, 3, 0,  0, 0,  0}, 0xB000}};

static DWR_PPI  mDwrPpi = {
  DirtyWarmResetExecute
};

static EFI_PEI_PPI_DESCRIPTOR gDirtyWarmResetPpi[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gDirtyWarmResetGuid,
    &mDwrPpi
  }
};

/**
  When DirtyWarmReset is detested installs PPI to triger
  DirtyWarmReset stall point and reset.
**/
VOID
PchInstallDwrPpi (
  VOID
  )
{
  EFI_STATUS Status;

  if (PchIsDwrFlow() == FALSE) {
    return;
  }

  DEBUG((DEBUG_ERROR, "DWR: DWR detected - install PPI\n"));
  Status  = PeiServicesInstallPpi (gDirtyWarmResetPpi);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "ERROR: Can't install DWR PPI\n"));
  } else {
    DEBUG((DEBUG_ERROR, "DWR: DWR PPI has been installed\n"));
  }
}

/**
  Checks whether there is any PSF internal Fabric Errors Logged
  If Yes just log this on the serial console and store in the HOB to be restored later
  in DXE by any driver/RAS driver provided by OEM
  This is according to HSD request 4929460

  @param[in] None

**/

VOID
PchLogPsfInternalErrors (
  VOID
  )
{
  UINTN PsfIndex, Port, PortGroup;
  UINT32 Data32 = 0;
  UINT32 DataFirstRead, DataSecondRead;
  UINTN CurrentOffset = 0;
  VOID *HobPtr;
  UINT8 ElementsCount = sizeof(mPSFsToCheckForErrors)/sizeof(PCH_PSF_ERROR_SEGMENT);
  PCH_FABRIC_ERRORS *PsfErrors;
  EFI_STATUS Status;

  Status = PeiServicesAllocatePool(sizeof(PCH_FABRIC_ERRORS),(VOID *)&PsfErrors);
  ASSERT (Status == EFI_SUCCESS);
  PsfErrors->Data32 = 0;

  ///
  /// Check all errors in all PSF for all channels in all ports for each port Group and RS
  /// Loop each PSF
  ///
  for (PsfIndex = 0; PsfIndex < ElementsCount; PsfIndex++){
    // Loop each Port Group
    for (PortGroup = 0; PortGroup < PCH_PSF_ERRORS_MAX_PORT_GROUP_NUMBER; PortGroup++)
    {
      CurrentOffset = mPSFsToCheckForErrors[PsfIndex].PSFxEhcrRS0PG0Base + PortGroup * V_PCH_PSFX_EHCR_PG_OFFSET;
      // Loop each port in given Port Group
      for (Port = 0; Port < mPSFsToCheckForErrors[PsfIndex].PortsInGroup[PortGroup]; Port++){
        // Check only Channel 0 for this port
        PchPcrRead32 (mPSFsToCheckForErrors[PsfIndex].Pid,
          (UINT16)(CurrentOffset + Port * V_PCH_PSFX_EHCR_PORT_OFFSET), &DataFirstRead);
        PchPcrRead32 (mPSFsToCheckForErrors[PsfIndex].Pid,
          (UINT16)(CurrentOffset + Port * V_PCH_PSFX_EHCR_PORT_OFFSET), &DataSecondRead);
        if (DataFirstRead != DataSecondRead) {
          ///
          /// LBG A0 WA based on HSD request 4929460 we can't trust first error register read
          /// If the second read value is different we must read it back third time in order to make sure value is correct
          ///
          PchPcrRead32 (mPSFsToCheckForErrors[PsfIndex].Pid,
            (UINT16)(CurrentOffset + Port * V_PCH_PSFX_EHCR_PORT_OFFSET), &Data32);
        }
        else {
          Data32 = DataFirstRead;
        }
        if (Data32 != 0) {
          // Error has been detected for this port
          // Set global error bits in global error structure
          PsfErrors->Data32 |= (Data32 & B_PCH_PSFX_EHCR_MASK) << (PsfIndex * 3); // 3 error status bits are being used for each PSF
          DEBUG ((DEBUG_ERROR,
            "ERROR: Internal fabric error detected!! PSF PID:0x%x, RSx/PortGroup:0x%x, Port:0x%x, Channel:0, error bits:0x%x\n",
            mPSFsToCheckForErrors[PsfIndex].Pid, PortGroup, Port, Data32));
          break;
        }
      }
    }
  }
  ///
  /// Finaly store all data into HOB so it can be used later in DXE by the RAS modules
  ///
  HobPtr = BuildGuidDataHob (&gPchPsfErrorHobGuid, (VOID *)PsfErrors, sizeof (PCH_FABRIC_ERRORS));
  ASSERT (HobPtr != 0);

}




/**
  Check some PCH policies are valid for debugging unexpected problem if these
  values are not initialized or assigned incorrect resource.

  @param[in] PchPolicy    The PCH Policy PPI instance

**/
VOID
PchValidatePolicy (
  IN  PCH_POLICY_PPI *PchPolicy
  )
{
  ASSERT (PchPolicy->Revision == PCH_POLICY_REVISION);
  ASSERT (PchPolicy->AcpiBase != 0);
  ASSERT (PchPolicy->TempMemBaseAddr != 0);
}

/**
  Build Memory Mapped IO Resource which is used to build E820 Table in LegacyBios,
  the resource range should be preserved in ACPI as well.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
PchPreservedMmioResource (
  VOID
  )
{
  //
  // This function builds a HOB that describes a chunk of system memory.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PCH_PRESERVED_BASE_ADDRESS,
    PCH_PRESERVED_MMIO_SIZE
    );
  //
  // This function builds a HOB for the memory allocation.
  //
  BuildMemoryAllocationHob (
    PCH_PRESERVED_BASE_ADDRESS,
    PCH_PRESERVED_MMIO_SIZE,
    EfiMemoryMappedIO
    );

  return EFI_SUCCESS;
}

/**
  Perform DCI configuration.

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
  @param[in] PchPwrmBase                PCH Power Management Base address of this PCH device
**/
VOID
PchDciConfiguration (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi,
  IN  UINT32                            PchPwrmBase
  )
{
  UINT32                                Data32;
  BOOLEAN                               DciEnabled;
  BOOLEAN                               DciDetected;

  DciEnabled  = FALSE;
  DciDetected = FALSE;

  ///
  /// Set PCR[DCI] + 30h bit[5] to 1, and clear 30h [2,1,0] = 0
  ///
  ///
  /// BIOS need to set DCI PCR offset 0x08[19,18,17,16]=0
  ///
  PchPcrAndThenOr32 (PID_DCI, R_PCH_PCR_DCI_ECKPWRCTL, (UINT32)~(BIT19 | BIT18 | BIT17 | BIT16), 0);


  ///
  /// BWG 13.6 DCI support
  /// DCI enable configuration
  ///
  /// If (DCI mode enable) {
  ///   Enable flow
  /// } else {
  ///   If (auto mode enabled) {
  ///     Auto flow
  ///   } else {
  ///    Disable flow
  ///   }
  /// }
  ///

  DciEnabled = PchPolicyPpi->DciConfig.DciEn == TRUE;
  ///
  /// If DCI is not eanbled, and DCI AUTO MODE is enable, then detects DCI connected.
  ///
  if ((DciEnabled == FALSE) && (PchPolicyPpi->DciConfig.DciAutoDetect == TRUE)) {
    ///
    /// Detect DCI being avaiable (PCR[DCI] + 0x4[9] == 1 or PCR[DCI] + 0x4[10] == 1)
    ///
    PchPcrRead32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, &Data32);
    DciDetected = (Data32 & (BIT9 | BIT10)) != 0;
  }

  if (DciEnabled || DciDetected) {
    ///
    /// Disable SLP S0 and ModPHY sus power gating feature
    /// Handled in PchPm.c
    ///

    if (DciEnabled) {
      ///
      /// Set PCR[DCI] + 4h bit[4] to 1 if DCI is enabled.
      ///
      PchPcrAndThenOr32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, (UINT32)~0, (B_PCH_PCR_DCI_ECTRL_HDCIEN));
    }
    ///
    /// Set HSWPGCR1, PWRMBASE offset 0x5D0 to 0x80000000.
    /// This step has higher priority than the TraceHub mode disable HSWPGCR1 step.
    ///
    MmioWrite32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1, B_PCH_PWRM_SW_PG_CTRL_LOCK);
  } else {
    ///
    /// Set PCR[DCI] + 4h bit[4] to 0 and bit[0] to 1
    ///
    PchPcrAndThenOr32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, (UINT32) ~(B_PCH_PCR_DCI_ECTRL_HDCIEN), (B_PCH_PCR_DCI_ECTRL_HDCILOCK));
  }
}

/**
  Internal function performing miscellaneous init needed in early PEI phase

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
PchMiscInit (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{
  UINTN                 PciPmcRegBase;
  PCH_SERIES            PchSeries;

  if (PchIsDwrFlow()) {
    DEBUG ((DEBUG_INFO, "DWR: PchMiscInit() End\n"));
    return EFI_SUCCESS;
  }

  PchSeries        = GetPchSeries ();
  PciPmcRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_PMC,
                      PCI_FUNCTION_NUMBER_PCH_PMC
                      );
  ///
  /// Set PMC PCI offset ACh[20] = 0 at early boot
  ///
  MmioAnd32 (PciPmcRegBase + R_PCH_PMC_ETR3, (UINT32)~(B_PCH_PMC_ETR3_CF9GR));

  if (PchPolicyPpi->Port80Route == PchReservedPageToLpc) {
    PchPcrAndThenOr8 (
      PID_DMI, R_PCH_PCR_DMI_GCS + 1,
      (UINT8) (~(B_PCH_PCR_DMI_RPR >> 8)),
      0
      );
  } else {
    PchPcrAndThenOr8 (
      PID_DMI, R_PCH_PCR_DMI_GCS + 1,
      (UINT8)~0,
      (UINT8) (B_PCH_PCR_DMI_RPR >> 8)
      );
  }


    ///
    /// BIOS need to set LPC PCR 0x341C[13,3:0] to all 1's and [8,9.10] = 0's
    ///
  PchPcrAndThenOr32 (PID_LPC, R_PCH_PCR_LPC_CCE, (UINT32)~(BIT10 | BIT9 | BIT8), (BIT13 | BIT3 | BIT2 | BIT1 | BIT0));

  ///
  /// BIOS need to set FIA PCR offset 0[17:15] to 111b and offset 0x20[31] = 1b
  ///
  ///
  /// Set FIA PCR offset 0x20[31] = 1b
  //
  PchPcrAndThenOr32 (PID_FIA, R_PCH_PCR_FIA_PLLCTL, (UINT32)~0, (BIT31));

  ///
  /// Clear PCR PSF_1_PSF_PORT_CONFIG_PG1_PORT7[5] = 0b
  ///
  if (PchSeries == PchLp) {
    PchPcrAndThenOr8 (PID_PSF1, R_PCH_LP_PCR_PSF1_PSF_PORT_CONFIG_PG1_PORT7, (UINT8)~(BIT5), 0);
  } else {
    PchPcrAndThenOr8 (PID_PSF1, R_PCH_H_PCR_PSF1_PSF_PORT_CONFIG_PG1_PORT7, (UINT8)~(BIT5), 0);
  }

  return EFI_SUCCESS;
}

/**
  Internal function performing HPET initin early PEI phase

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
**/
VOID
PchHpetInit (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  PCH_HPET_CONFIG                       *HpetConfig;
  UINTN                                 P2sbBase;
  UINT16                                Data16;
  UINT32                                HpetBase;

  HpetConfig      = &PchPolicyPpi->HpetConfig;
  P2sbBase        = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_P2SB,
                      PCI_FUNCTION_NUMBER_PCH_P2SB
                      );

  //
  // Program this field accordingly if unique bus:device:function number is required for the
  // corresponding HPET
  //
  if (HpetConfig->BdfValid) {
    Data16 = ((UINT16) (HpetConfig->BusNumber) << 8) & B_PCH_P2SB_HBDF_BUF;
    Data16 |= ((UINT16) (HpetConfig->DeviceNumber) << 3) & B_PCH_P2SB_HBDF_DEV;
    Data16 |= (UINT16) (HpetConfig->FunctionNumber) & B_PCH_P2SB_HBDF_FUNC;
    MmioWrite16 ((UINTN) (P2sbBase + R_PCH_P2SB_HBDF), Data16);
  }
  //
  // Initial and enable HPET High Precision Timer memory address for basic usage
  // If HPET base is not set, the default would be 0xFED00000.
  //
  HpetBase = HpetConfig->Base;
  if (HpetBase == 0) {
    HpetBase = V_PCH_HPET_BASE0;
  }
  MmioAndThenOr8 (
    P2sbBase + R_PCH_P2SB_HPTC,
    (UINT8)~B_PCH_P2SB_HPTC_AS,
    (UINT8)(((HpetBase >> N_PCH_HPET_ADDR_ASEL) & B_PCH_P2SB_HPTC_AS) | B_PCH_P2SB_HPTC_AE)
    );
  //
  // Read back for posted write to take effect
  //
  MmioRead8 (P2sbBase + R_PCH_P2SB_HPTC);
  //
  // Set HPET Timer enable to start counter spinning
  //
  if (HpetConfig->Enable == TRUE) {
    MmioOr32 (HpetBase + 0x10, 0x1);
  }
  //
  // Build the resource descriptor hob for HPET address resource.
  // HPET only claims 0x400 in size, but the minimal size to reserve memory
  // is one page 0x1000.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    HpetBase,
    0x1000
    );
  BuildMemoryAllocationHob (
    HpetBase,
    0x1000,
    EfiMemoryMappedIO
    );
}

//
// SVID / SID init table entry
//
typedef struct {
  UINT8 DeviceNumber;
  UINT8 FunctionNumber;
  UINT8 SvidRegOffset;
} PCH_SVID_SID_INIT_ENTRY;

STATIC PCH_SVID_SID_INIT_ENTRY  SvidSidInitTable[] = {
  { PCI_DEVICE_NUMBER_PCH_LPC,                  PCI_FUNCTION_NUMBER_PCH_LPC,                 PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_P2SB,                 PCI_FUNCTION_NUMBER_PCH_P2SB,                PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_PMC,                  PCI_FUNCTION_NUMBER_PCH_PMC,                 PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_HDA,                  PCI_FUNCTION_NUMBER_PCH_HDA,                 PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_SATA,                 PCI_FUNCTION_NUMBER_PCH_SATA,                PCI_SVID_OFFSET},
#if SMCPKG_SUPPORT
  { PCI_DEVICE_NUMBER_PCH_SSATA,                PCI_FUNCTION_NUMBER_PCH_SSATA,               PCI_SVID_OFFSET},
#endif  
  { PCI_DEVICE_NUMBER_PCH_SMBUS,                PCI_FUNCTION_NUMBER_PCH_SMBUS,               PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_SPI,                  PCI_FUNCTION_NUMBER_PCH_SPI,                 PCI_SVID_OFFSET},
  //
  // Skip PCH LAN controller
  // PCH LAN SVID/SID may be loaded automatically from the NVM Word 0Ch/0Bh upon power up or reset
  // depending on the "Load Subsystem ID" bit field in NVM word 0Ah
  //
  //{ PCI_DEVICE_NUMBER_PCH_LAN,                  PCI_FUNCTION_NUMBER_PCH_LAN,                 PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_TRACE_HUB,            PCI_FUNCTION_NUMBER_PCH_TRACE_HUB,           PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_XHCI,                 PCI_FUNCTION_NUMBER_PCH_XHCI,                PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_XDCI,                 PCI_FUNCTION_NUMBER_PCH_XDCI,                PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_THERMAL,              PCI_FUNCTION_NUMBER_PCH_THERMAL,             PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9,    R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10,   R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11,   R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12,   R_PCH_PCIE_SVID},
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_13,   R_PCH_PCIE_SVID}, ///< PCI Express Root Port #13, SKL PCH H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_14,   R_PCH_PCIE_SVID}, ///< PCI Express Root Port #14, SKL PCH H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_15,   R_PCH_PCIE_SVID}, ///< PCI Express Root Port #15, SKL PCH H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_16,   R_PCH_PCIE_SVID}, ///< PCI Express Root Port #16, SKL PCH H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_17,   R_PCH_PCIE_SVID}, ///< PCI Express Root Port #17, SKL PCH H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_18,   R_PCH_PCIE_SVID}, ///< PCI Express Root Port #18, SKL PCH H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_19,   R_PCH_PCIE_SVID}, ///< PCI Express Root Port #19, SKL PCH H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3,        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_20,   R_PCH_PCIE_SVID}, ///< PCI Express Root Port #20, SKL PCH H only
  { ME_DEVICE_NUMBER,                           HECI_FUNCTION_NUMBER,             PCI_SVID_OFFSET},
  { ME_DEVICE_NUMBER,                           HECI2_FUNCTION_NUMBER,            PCI_SVID_OFFSET},
  { ME_DEVICE_NUMBER,                           IDER_FUNCTION_NUMBER,             PCI_SVID_OFFSET},
  { ME_DEVICE_NUMBER,                           SOL_FUNCTION_NUMBER,              PCI_SVID_OFFSET}
  ,
  { ME_DEVICE_NUMBER,                           HECI3_FUNCTION_NUMBER,            PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_EVA,                      PCI_FUNCTION_NUMBER_EVA_MROM0,    PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_EVA,                      PCI_FUNCTION_NUMBER_EVA_MROM1,    PCI_SVID_OFFSET},
  { PCI_DEVICE_NUMBER_EVA,                      PCI_FUNCTION_NUMBER_EVA_SSATA,    PCI_SVID_OFFSET}
#if IE_SUPPORT
  //
  // Do not assign Intel ids for IE devices. Let IE Policy in plafrom pkt assign ids for IE devices.
  // OEM may want to assign proprietary ids to allow proprietary driver binding for IE devices.
  //
#else
  ,
  { IE_DEV,                                     IE_FUN_HECI1,                     PCI_SVID_OFFSET},
  { IE_DEV,                                     IE_FUN_HECI2,                     PCI_SVID_OFFSET},
  { IE_DEV,                                     IE_FUN_IDER,                      PCI_SVID_OFFSET},
  { IE_DEV,                                     IE_FUN_KT,                        PCI_SVID_OFFSET},
  { IE_DEV,                                     IE_FUN_HECI3,                     PCI_SVID_OFFSET}
#endif // IE_SUPPORT
};

/**
  Program Pch devices Subsystem Vendor Identifier (SVID) and Subsystem Identifier (SID).

  @param[in] PchPolicy    The PCH Policy PPI instance

  @retval EFI_SUCCESS     The function completed successfully
**/
EFI_STATUS
PchProgramSvidSid (
  IN PCH_POLICY_PPI      *PchPolicy
  )
{
  UINT8                           Index;
  UINT8                           BusNumber;
  UINTN                           PciEAddressBase;
  UINT8                           DeviceNumber;
  UINT8                           FunctionNumber;
  UINT8                           SvidRegOffset;
  UINTN                           SvidSidInitTableSize;
  PCH_SERIES                      PchSeries;

  DEBUG ((DEBUG_INFO, "ProgramSvidSid() Start\n"));

  PchSeries            = GetPchSeries ();
  BusNumber            = DEFAULT_PCI_BUS_NUMBER_PCH;
  SvidSidInitTableSize = sizeof (SvidSidInitTable) / sizeof (PCH_SVID_SID_INIT_ENTRY);
  if (PchIsDwrFlow()) {
    DEBUG ((DEBUG_INFO, "DWR: ProgramSvidSid() End\n"));
    return EFI_SUCCESS;
  }

  if ((PchPolicy->PchConfig.SubSystemVendorId != 0) ||
      (PchPolicy->PchConfig.SubSystemId != 0))
  {
    for (Index = 0; Index < SvidSidInitTableSize; Index++) {
      DeviceNumber    = SvidSidInitTable[Index].DeviceNumber;
      FunctionNumber  = SvidSidInitTable[Index].FunctionNumber;
      SvidRegOffset   = SvidSidInitTable[Index].SvidRegOffset;
      PciEAddressBase = MmPciBase (
                          BusNumber,
                          DeviceNumber,
                          FunctionNumber
                          );
      //
      // Skip if the device is disabled
      //
      if (MmioRead16 (PciEAddressBase) != V_PCH_INTEL_VENDOR_ID) {
        continue;
      }
      //
      // Program Pch devices Subsystem Vendor Identifier (SVID) and Subsystem Identifier (SID)
      //
#if SMCPKG_SUPPORT == 0
      MmioWrite32 (
        (UINTN) (PciEAddressBase + SvidRegOffset),
        (UINT32) (PchPolicy->PchConfig.SubSystemVendorId |
                 (PchPolicy->PchConfig.SubSystemId << 16))
        );
#else
      switch( MmioRead32 ((UINTN)(PciEAddressBase)) ) {
        case 0xA1868086: //SATA Production SKU, 3rd Party RAID ID
        case 0xA2068086: //SATA Super SKU, 3rd Party RAID ID
        case 0x28228086: //SATA Production/Super SKU, Alternate ID
        case 0x28268086: //SATA Production/Super SKU, Alternate ID for Intel RSTe
        case 0xA1D68086: //SSATA Production SKU, 3rd Party RAID ID
        case 0xA2568086: //SSATA Super SKU, 3rd Party RAID ID
        case 0x28238086: //SSATA Production/Super SKU, Alternate ID
        case 0x28278086: //SSATA Production/Super SKU, Alternate ID for Intel RSTe
	        //Skip devices
	        MmioWrite32 (
	          (UINTN) (PciEAddressBase + SvidRegOffset),
	          (UINT32) (PchPolicy->PchConfig.SubSystemVendorId |
	          (PchPolicy->PchConfig.SubSystemId << 16))
	        );
        break;
	
        default:
	        MmioWrite32 (
	          (UINTN) (PciEAddressBase + SvidRegOffset), (UINT32)(CRB1_SSID));
        break;
      }
#endif
    }
  }

  DEBUG ((DEBUG_INFO, "ProgramSvidSid() End\n"));

  return EFI_SUCCESS;
}

/**
  This function assigns a CLKREQ# signal to PCI Express ports and GbE.
  Each enabled ports must have distinct CLKREQ# pin assigned before power management features are enabled.

  Assignment is based on the platform policy with assumption that unused ports will be disabled
  before enabling power management on remaining ports.

  @param[in] PchPolicy    The PCH Policy PPI instance
**/
VOID
PchConfigureClkreqMapping (
  IN  PCH_POLICY_PPI  *PchPolicyPpi
  )
{
  UINT32      MaxPciePortNum;
  UINT32      PortIndex;
  UINT32      Index;
  UINT32      Drcrm[PCH_MAX_FIA_DRCRM];
  UINT32      ClkReqNum;

  for (Index = 0; Index < PCH_MAX_FIA_DRCRM; ++Index) {
    Drcrm[Index] = 0;
  }
  ///
  /// PCH BIOS Spec Section 29.1.1 CLKREQ to Root Port Mapping
  /// The mapping of the PCIeExpress Ports to the CLKREQ# pins can be
  /// specified through the DRCRMx registers
  ///
  MaxPciePortNum  = GetPchMaxPciePortNum ();
  ASSERT ((MaxPciePortNum / 8) <= PCH_MAX_FIA_DRCRM);
  for (PortIndex = 0; PortIndex < MaxPciePortNum; PortIndex++) {
    if (PchPolicyPpi->PcieConfig.RootPort[PortIndex].ClkReqSupported) {
      ClkReqNum = PchPolicyPpi->PcieConfig.RootPort[PortIndex].ClkReqNumber;
    } else {
      ClkReqNum = 0;
    }
    Drcrm[PortIndex / 8] |= ClkReqNum << (4 * (PortIndex % 8));
  }

  ClkReqNum = PchPolicyPpi->LanConfig.ClkReqNumber;
  Drcrm[2] |= ClkReqNum << N_PCH_PCR_FIA_DRCRM3_GBEPCKRQM;

  DEBUG ((DEBUG_INFO, "DRCRM: 0x%08x 0x%08x 0x%08x\n", Drcrm[0], Drcrm[1], Drcrm[2]));

  for (Index = 0; Index < PCH_MAX_FIA_DRCRM; ++Index) {
    PchPcrWrite32 (
      PID_FIA,
      (UINT16) (R_PCH_PCR_FIA_DRCRM1 + (Index * S_PCH_PCR_FIA_DRCRM)),
      Drcrm[Index]
      );
  }

  ///
  /// Lock DRCRM registers and enable FIA power management
  /// PCR[FIA] + 0h bits [31, 17, 16, 15] to [1, 1, 1, 1]
  ///
  PchPcrAndThenOr32 (
    PID_FIA,
    R_PCH_PCR_FIA_CC,
    ~0u,
    B_PCH_PCR_FIA_CC_SRL | B_PCH_PCR_FIA_CC_PTOCGE | B_PCH_PCR_FIA_CC_OSCDCGE | B_PCH_PCR_FIA_CC_SCPTCGE
    );
}

#if SMCPKG_SUPPORT
/**
  The function programs HSIO registers.

  @param[in]  PchPolicyPpi        The PCH Policy PPI instance
**/
VOID
AmiPchHsioProg (
  IN  PCH_POLICY_PPI      *PchPolicyPpi,
  IN  UINT32              TempMemBaseAddr,
  IN  UINTN               PmcBaseAddress
  )
{
  PCH_SBI_PTSS_HSIO_TABLE     OemPtssTable[] = {SB_OEM_PTSS_TABLE_LIST {0xff, 0xff, 0x0, 0x0, 0x0, 0x0}};
  UINT8                       PortId;
  UINT8                       LaneOwner;
  UINT8                       LaneNum;
  UINTN                       Index = 0;
  UINT16                      Offset;
  UINT32                      Data32;
  UINT8                       Response;
  
  DEBUG((EFI_D_INFO, "AmiPchHsioProg()\n"));
  while(OemPtssTable[Index].LaneNum != 0xff){
    DEBUG((EFI_D_INFO, "AmiPchHsioProg, Index = %x, LaneNum = %x, OemPtssTable[Index].PhyMode = %x, \n", Index, OemPtssTable[Index].LaneNum, OemPtssTable[Index].PhyMode));
  // LaneNum start from 0

    //if (GetPchSeries () == PchH) {  
    //    if (OemPtssTable[Index].LaneNum < 15) 
    //        LaneNum = OemPtssTable[Index].LaneNum - 1; // LaneNum start from 1 to match the spec.
    //    else 
    //        LaneNum = OemPtssTable[Index].LaneNum + 3; // Fix the LaneNum not match.
    //} else {
    //    LaneNum = OemPtssTable[Index].LaneNum - 1; // LaneNum start from 1 to match the spec.
    //}
	LaneNum = OemPtssTable[Index].LaneNum;
    Offset = OemPtssTable[Index].Offset;
    
    if (OemPtssTable[Index].PhyMode < V_PCH_PCR_FIA_LANE_OWN_UX8) {
        
        //if (OemPtssTable[Index].LaneNum < 1) {
        //    DEBUG((EFI_D_ERROR, "Error, LaneNum start from 1.\n"));
        //    continue;
        //}
        if(OemPtssTable[Index].PhyMode == 0){//For DMI 
            PchGetLaneInfoWM20 (LaneNum, &PortId, &LaneOwner);
        }
        else{
            PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);
        }
        DEBUG ((DEBUG_INFO, "HSIO : LaneNum = %d, PortID = 0x%02x, Offset = 0x%04x, LaneOwner = %d\n", LaneNum, (PCH_SBI_PID) PortId, (UINT16) Offset, LaneOwner));
        if (LaneOwner == OemPtssTable[Index].PhyMode) {

            PchPcrAndThenOr32 (
                      (PCH_SBI_PID) PortId,
                      (UINT16) Offset,
                      (UINT32) OemPtssTable[Index].BitMask,
                      (UINT32) OemPtssTable[Index].Value);
            DEBUG ((DEBUG_INFO, "HSIO : LaneNum = %d, PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x, BitMask = 0x%08x", LaneNum, (PCH_SBI_PID) PortId, (UINT16) Offset, OemPtssTable[Index].Value, OemPtssTable[Index].BitMask));
            PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
            DEBUG((EFI_D_INFO, " Register value = 0x%08x\n", Data32));
        } else {
            DEBUG((EFI_D_INFO, "Error!! LaneOwner not match. LaneOwner = %d\n", LaneOwner));
        }
    } else if (OemPtssTable[Index].PhyMode == 8){
        // For program USB2 U2ECR1 and U2ECR3
        
        DEBUG((EFI_D_INFO, "USB2: Offset = 0x%04x, Value = 0x%08x, BitMask = 0x%08x\n", Offset, OemPtssTable[Index].Value, OemPtssTable[Index].BitMask));
        
        PchSbiExecution (
          PID_USB2,
          Offset,
          PrivateControlRead,
          FALSE,
          &Data32,
          &Response
          );        
        DEBUG((EFI_D_INFO, "Original value = 0x%08x, ", Data32));
        
        Data32 &= (UINT32) OemPtssTable[Index].BitMask;
        Data32 |= (UINT32) OemPtssTable[Index].Value;
        DEBUG((EFI_D_INFO, "Program value = 0x%08x, ", Data32));
                
        PchSbiExecution (
          PID_USB2,
          Offset,
          PrivateControlWrite,
          TRUE,
          &Data32,
          &Response
          );
        PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) Offset, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);
        
#ifndef MDEPKG_NDEBUG                
        PchSbiExecution (
          PID_USB2,
          Offset,
          PrivateControlRead,
          FALSE,
          &Data32,
          &Response
          );
        DEBUG((EFI_D_INFO, "Actual value = 0x%08x\n", Data32));
#endif        

    } else {
       DEBUG((EFI_D_ERROR, "Error!! PhyMode not support. phyMode = %d", OemPtssTable[Index].PhyMode));
    }
    
    Index++;
  }
}
#endif

/**
  This function performs basic initialization for PCH in PEI phase after Policy produced.
  If any of the base address arguments is zero, this function will disable the corresponding
  decoding, otherwise this function will enable the decoding.
  This function locks down the AcpiBase.

  @param[in] SiPolicyPpi   The Silicon Policy PPI instance
  @param[in] PchPolicyPpi  The PCH Policy PPI instance
**/
VOID
EFIAPI
PchOnPolicyInstalled (
  IN  PCH_POLICY_PPI  *PchPolicyPpi
  )
{
  EFI_STATUS              Status;
  UINTN                   LpcBaseAddress;
  UINTN                   PmcBaseAddress;
  UINTN                   SpiBaseAddress;
#ifndef FSP_FLAG
  BOOLEAN                 RstPcieStorageRemapEnabled;
#endif //FSP_FLAG

  DEBUG ((DEBUG_INFO, "PchOnPolicyInstalled() - Start\n"));

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );

  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SPI,
                     PCI_FUNCTION_NUMBER_PCH_SPI
                     );


  ///
  /// Set ACPI Base in PMC device
  ///
  Status = PchAcpiBaseSet (PchPolicyPpi->AcpiBase);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Lock down the ACPI Base
  ///
  MmioOr8 (
    PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B + 2,
    (UINT8) (B_PCH_PMC_GEN_PMCON_B_ACPI_BASE_LOCK >> 16)
    );

  ///
  /// Set PWRM Base in PMC device
  ///
  Status = PchPwrmBaseSet (PCH_PWRM_BASE_ADDRESS);
  ASSERT_EFI_ERROR (Status);

  //
  // Check to disable and lock WDT according to Policy.
  //
  WdtOnPolicyInstalled ((BOOLEAN)PchPolicyPpi->WdtConfig.DisableAndLock);

#ifndef FSP_FLAG
  ///
  /// Tune the USB 2.0 high-speed signals quality.
  /// This is done before USB Initialization because the PMC XRAM has
  /// to be filled up with USB2 AFE settings followed by HSIO settings
  /// before it is locked
  ///
  Usb2AfeProgramming (&PchPolicyPpi->UsbConfig, PchPolicyPpi->TempMemBaseAddr);
#endif // FSP_FLAG
  //
  // Configure BIOS HSIO if ChipsetInit HSIO Settings in CSME are right
  // The HSIO PPI MUST be ready before this function.
  // The HSIO programming should be done before any IP configuration.
  //
#if ME_SUPPORT_FLAG
  if (PeiHeciInitialize() == EFI_SUCCESS) {
    PchHsioChipsetInitProg (FALSE);
  }
#endif // ME_SUPPORT_FLAG
  Status = PchHsioBiosProg (PchPolicyPpi);
  ASSERT_EFI_ERROR (Status);

#ifndef FSP_FLAG
  //
  // Configure USB3 ModPHY turning.
  //
  XhciUsb3Tune (&(PchPolicyPpi->UsbConfig));
#endif // FSP_FLAG
#if SMCPKG_SUPPORT
  AmiPchHsioProg(PchPolicyPpi, PchPolicyPpi->TempMemBaseAddr, PmcBaseAddress);
#endif
  ///
  /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
  /// BIOS needs to enable the "Enable in SMM.STS" (EISS) feature of the PCH by setting
  /// SPI PCI offset DCh[5] = 1b for SPI or setting eSPI PCI offset DCh[5] = 1b for eSPI.
  /// When this bit is set, the BIOS region is not writable until SMM sets the InSMM.STS bit,
  /// to ensure BIOS can only be modified from SMM. Please refer to CPU BWG for more details
  /// on InSMM.STS bit.
  /// Intel requires that BIOS enables the Lock Enable (LE) feature of the PCH to ensure
  /// SMM protection of flash.
  /// SPI PCI offset DCh[1] = 1b for SPI or setting eSPI PCI offset DCh[1] = 1b for eSPI.
  /// When this bit is set, EISS is locked down.
  ///

// APTIOV_SERVER_OVERRIDE_RC_START : EISS bit can't be set before NVRAMSMI
//  if (PchPolicyPpi->LockDownConfig.SpiEiss == 1) {
    //
    // Set SPI EISS (SPI PCI offset DCh[5]) and LE (SPI PCI offset DCh[1])
    // Set LPC/eSPI EISS (LPC/eSPI PCI offset DCh[5]) and LE (LPC/eSPI PCI offset DCh[1])
    //
//    MmioOr8 (SpiBaseAddress + R_PCH_SPI_BC, B_PCH_SPI_BC_EISS);
//    MmioOr8 (LpcBaseAddress + R_PCH_LPC_BC, B_PCH_LPC_BC_EISS);
//  } else {
// APTIOV_SERVER_OVERRIDE_RC_END : EISS bit can't be set before NVRAMSMI
    //
    // Clear SMM_EISS (SPI PCI offset DCh[5])
    // Clear LPC/eSPI EISS (LPC/eSPI PCI offset DCh[5])
    // Since the HW default is 1, need to clear it when disabled in policy
    //
// APTIOV_SERVER_OVERRIDE_RC_START : EISS bit can't be set before NVRAMSMI
    MmioAnd8 (SpiBaseAddress + R_PCH_SPI_BC, (UINT8)~B_PCH_SPI_BC_EISS);
    MmioAnd8 (LpcBaseAddress + R_PCH_LPC_BC, (UINT8)~B_PCH_LPC_BC_EISS);
//  }
// APTIOV_SERVER_OVERRIDE_RC_END : EISS bit can't be set before NVRAMSMI

  PchDciConfiguration (PchPolicyPpi, PCH_PWRM_BASE_ADDRESS);

  TraceHubInitialize (
    (UINT8) PchPolicyPpi->PchTraceHubConfig.EnableMode
    );

  //
  // The following function should be executed after PchDciConfiguration()
  //
  TraceHubManagePowerGateControl();

  //
  // Configure SATA controller
  //
#ifndef FSP_FLAG
    RstPcieStorageRemapEnabled  = IsRstPcieStorageRemapEnabled (&PchPolicyPpi->SataConfig, PCH_SATA_FIRST_CONTROLLER);
    Status = EarlyConfigurePchHSata (
               &PchPolicyPpi->SataConfig,
               RstPcieStorageRemapEnabled,
               PchPolicyPpi->TempMemBaseAddr,
               PCH_SATA_FIRST_CONTROLLER
               );

    ConfigurePchHSata (
               &PchPolicyPpi->SataConfig,
               PchPolicyPpi->TempMemBaseAddr,
               PCH_SATA_FIRST_CONTROLLER
               );

    if(GetIsPchsSataPresent() == TRUE){
      Status = EarlyConfigurePchHSata (
                 &PchPolicyPpi->sSataConfig,
                 FALSE,
                 PchPolicyPpi->TempMemBaseAddr,
                 PCH_SATA_SECOND_CONTROLLER
                 );
      ConfigurePchHSata (
                 &PchPolicyPpi->sSataConfig,
                 PchPolicyPpi->TempMemBaseAddr,
                 PCH_SATA_SECOND_CONTROLLER
                 );
    }
#endif //PCH_SERVER_BIOS_FLAG

  Status = InitializePchSmbus (PchPolicyPpi);

  PchConfigureClkreqMapping (PchPolicyPpi);

  PchConfigurePsfGrantCountsForPcie ();

  //
  // Initialize LAN
  // This may cause platform reset if GbE enable policy changed
  //
  PchConfigureLan (PchPolicyPpi);
  Status = PchInitRootPorts (PchPolicyPpi, PchPolicyPpi->TempPciBusMin, PchPolicyPpi->TempPciBusMax);
  ASSERT_EFI_ERROR (Status);
  //
  // Program SVID and SID of PCH devices.
  // Program SVID and SID before most PCH device init since some device might be locked after init.
  //
  Status = PchProgramSvidSid (PchPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  Status = PchMiscInit (PchPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  PchHpetInit (PchPolicyPpi);

  //
  // Configure PCH OTG (xDCI), pre-mem phase
  //
  Status = ConfigureXdci (PchPolicyPpi);

  //
  // Configure PCH xHCI, pre-mem phase
  //
  Status = ConfigureXhciPreMem (PchPolicyPpi);

  ///
  /// Install the PCH PEI Init Done PPI
  ///
  Status = PeiServicesInstallPpi (&mPpiPchPeiInitDone);
  ASSERT_EFI_ERROR (Status);

  ConfigureLpcOnPolicy (PchPolicyPpi);


  InitAdrPreMem (PchPolicyPpi);

  if (PchPolicyPpi->PchConfig.Serm == TRUE){
    PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~0, B_PCH_PCR_ITSS_GIC_SERM);
  } else {
    PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~B_PCH_PCR_ITSS_GIC_SERM, 0);
  }
  DEBUG ((DEBUG_INFO, "PchOnPolicyInstalled() - End\n"));
}

/**
 Early init P2SB configuration
 For GPIO and ITSS that support sideband posted write, they can support
 back to back write after their correspoing bit sunder P2SB PCI Config
 80h-9fh are set.
**/
VOID
PchP2sbEarlyConfig (
  VOID
  )
{
  UINTN                   P2sbBase;

  P2sbBase  = MmPciBase (
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_P2SB,
                PCI_FUNCTION_NUMBER_PCH_P2SB
                );
  ///
  /// For GPIO and ITSS that support sideband posted write, they can support
  /// back to back write after their correspoing bit sunder P2SB PCI Config
  /// 80h-9fh are set.
  /// For SKL PCH-LP and SKL PCH-H, program the following at early PCH BIOS init
  /// 1. Set P2SB PCI offset 80h to 0
  /// 2. Set P2SB PCI offset 84h to 0
  /// 3. Set P2SB PCI offset 88h to 0
  /// 4. Set P2SB PCI offset 8Ch to 0
  /// 5. Set P2SB PCI offset 90h to 0
  /// 6. Set P2SB PCI offset 94h to 0000F000h
  /// 7. Set P2SB PCI offset 98h to 00000010h
  /// 8. Set P2SB PCI offset 9Ch to 0
  ///
  MmioWrite32 (P2sbBase + R_PCH_P2SB_80, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_84, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_88, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_8C, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_90, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_94, 0x0000F000);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_98, 0x00000010);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_9C, 0);
  ///
  /// Set P2SB PCI offset 0xF4[0] = 1
  ///
  MmioOr8 (P2sbBase + R_PCH_P2SB_F4, BIT0);
}

/**
  Function disable type 0 controller in PSF
**/
VOID
PsfT0FunctionDisable (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            ShdwRegBase
  )
{
  PchPcrAndThenOr32 (
    Pid,
    ShdwRegBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
    ~0u,
    B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
    );
}

/**
  Function disable type 1 controller in PSF
**/
VOID
PsfT1FunctionDisable (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            ShdwRegBase
  )
{
  PchPcrAndThenOr32 (
    Pid,
    ShdwRegBase + R_PCH_PCR_PSFX_T1_SHDW_PCIEN,
    ~0u,
    B_PCH_PCR_PSFX_T1_SHDW_PCIEN_FUNDIS
    );
}

/**
  This code is intended to handle all cases where IP could be fuse disabled, static power gated
  or disabled due to soft strap lane assignment

  @param[in] PchPwrmBase       PCH Power Management Base address of this PCH device

  @retval EFI_SUCCESS
**/
VOID
PchEarlyDisabledDeviceHandling (
  IN  UINT32                      PchPwrmBase
  )
{
  UINT32                  FuseDis2State;
  UINTN                   PcieRp1StrpFuseCfg;
  UINTN                   PcieRp5StrpFuseCfg;
  UINTN                   PcieRp9StrpFuseCfg;
  UINTN                   PcieRp13StrpFuseCfg;
  UINTN                   PcieRp17StrpFuseCfg;
  UINT32                  Data32;
  UINT32                  SpaPcd;
  UINT32                  SpbPcd;
  UINT32                  SpcPcd;
  UINT32                  SpdPcd;
  UINT32                  SpePcd;
  EFI_STATUS              Status;
  PCH_SERIES              PchSeries;
  UINTN                   Mrom0RegBase;
  UINT32                  MsDevFuncHide;
  EFI_SPI_PROTOCOL        *SpiProtocol = NULL;
  UINT32                  SoftstrapVal;
  UINT16                  RpPsfBase[PCH_MAX_PCIE_ROOT_PORTS];
  SpdPcd =0;
  SpePcd =0;
  PchSeries = GetPchSeries ();


  DEBUG ((DEBUG_INFO, "PchEarlyDisabledDeviceHandling() - Start\n"));

  //
  // Get fuse info from PWRMBASE + FUSE_DIS_RD_2 (Offset 644h)
  //
  FuseDis2State = MmioRead32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_FUSE_DIS_RD_2));

  DEBUG ((DEBUG_INFO, "PchPwrmBase + R_PCH_PWRM_FUSE_DIS_RD_2 = %x\n", FuseDis2State));

  ///
  /// BWG 5.3.1 Fuse Disabled, Static Disabled, non-Static function disable Device Handling
  ///

  //
  // Get PCIE Port configuration and disable value for each RP
  // and perform PSF function disable
  //
  PchPcrRead32 (PID_SPA, R_PCH_PCR_SPX_PCD, &SpaPcd);
  DEBUG ((DEBUG_INFO, "SPA Controller PCD Value = %x\n", SpaPcd));
  PchPcrRead32 (PID_SPB, R_PCH_PCR_SPX_PCD, &SpbPcd);
  DEBUG ((DEBUG_INFO, "SPB Controller PCD Value = %x\n", SpbPcd));
  PchPcrRead32 (PID_SPC, R_PCH_PCR_SPX_PCD, &SpcPcd);
  DEBUG ((DEBUG_INFO, "SPC Controller PCD Value = %x\n", SpcPcd));

  if (PchSeries == PchLp) {
    RpPsfBase[0]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE;
    RpPsfBase[1]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE02_REG_BASE;
    RpPsfBase[2]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE03_REG_BASE;
    RpPsfBase[3]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE04_REG_BASE;
    RpPsfBase[4]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE05_REG_BASE;
    RpPsfBase[5]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE06_REG_BASE;
    RpPsfBase[6]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE07_REG_BASE;
    RpPsfBase[7]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE08_REG_BASE;
    RpPsfBase[8]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE09_REG_BASE;
    RpPsfBase[9]  = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE10_REG_BASE;
    RpPsfBase[10] = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE11_REG_BASE;
    RpPsfBase[11] = R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE12_REG_BASE;
  } else {
    PchPcrRead32 (PID_SPD, R_PCH_PCR_SPX_PCD, &SpdPcd);
    DEBUG ((DEBUG_INFO, "SPD Controller PCD Value = %x\n", SpdPcd));
    PchPcrRead32 (PID_SPE, R_PCH_PCR_SPX_PCD, &SpePcd);
    DEBUG ((DEBUG_INFO, "SPE Controller PCD Value = %x\n", SpePcd));

    RpPsfBase[0]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE;
    RpPsfBase[1]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE02_REG_BASE;
    RpPsfBase[2]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE03_REG_BASE;
    RpPsfBase[3]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE04_REG_BASE;
    RpPsfBase[4]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE05_REG_BASE;
    RpPsfBase[5]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE06_REG_BASE;
    RpPsfBase[6]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE07_REG_BASE;
    RpPsfBase[7]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE08_REG_BASE;
    RpPsfBase[8]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE09_REG_BASE;
    RpPsfBase[9]  = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE10_REG_BASE;
    RpPsfBase[10] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE11_REG_BASE;
    RpPsfBase[11] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE12_REG_BASE;
    RpPsfBase[12] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE13_REG_BASE;
    RpPsfBase[13] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE14_REG_BASE;
    RpPsfBase[14] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE15_REG_BASE;
    RpPsfBase[15] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE16_REG_BASE;
    RpPsfBase[16] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE17_REG_BASE;
    RpPsfBase[17] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE18_REG_BASE;
    RpPsfBase[18] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE19_REG_BASE;
    RpPsfBase[19] = R_PCH_H_PCR_PSF1_T1_SHDW_PCIE20_REG_BASE;
  }

    if ((SpaPcd & B_PCH_PCR_SPX_PCD_P1D) == B_PCH_PCR_SPX_PCD_P1D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller A Port 0 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[0]);
    }
    if ((SpaPcd & B_PCH_PCR_SPX_PCD_P2D) == B_PCH_PCR_SPX_PCD_P2D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller A Port 1 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[1]);
    }
    if ((SpaPcd & B_PCH_PCR_SPX_PCD_P3D) == B_PCH_PCR_SPX_PCD_P3D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller A Port 2 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[2]);
    }
    if ((SpaPcd & B_PCH_PCR_SPX_PCD_P4D) == B_PCH_PCR_SPX_PCD_P4D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller A Port 3 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[3]);
    }
    if ((SpbPcd & B_PCH_PCR_SPX_PCD_P1D) == B_PCH_PCR_SPX_PCD_P1D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller B Port 0 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[4]);
    }
    if ((SpbPcd & B_PCH_PCR_SPX_PCD_P2D) == B_PCH_PCR_SPX_PCD_P2D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller B Port 1 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[5]);
    }
    if ((SpbPcd & B_PCH_PCR_SPX_PCD_P3D) == B_PCH_PCR_SPX_PCD_P3D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller B Port 2 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[6]);
    }
    if ((SpbPcd & B_PCH_PCR_SPX_PCD_P4D) == B_PCH_PCR_SPX_PCD_P4D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller B Port 3 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[7]);
    }
    if ((SpcPcd & B_PCH_PCR_SPX_PCD_P1D) == B_PCH_PCR_SPX_PCD_P1D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller C Port 0 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[8]);
    }
    if ((SpcPcd & B_PCH_PCR_SPX_PCD_P2D) == B_PCH_PCR_SPX_PCD_P2D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller C Port 1 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[9]);
    }
    if ((SpcPcd & B_PCH_PCR_SPX_PCD_P3D) == B_PCH_PCR_SPX_PCD_P3D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller C Port 2 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[10]);
    }
    if ((SpcPcd & B_PCH_PCR_SPX_PCD_P4D) == B_PCH_PCR_SPX_PCD_P4D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller C Port 3 Fuse disabled - performing PSF disable\n"));
    PsfT1FunctionDisable (PID_PSF1, RpPsfBase[11]);
    }

  if (PchSeries == PchH) {
    if ((SpdPcd & B_PCH_PCR_SPX_PCD_P1D) == B_PCH_PCR_SPX_PCD_P1D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller D Port 0 Fuse disabled - performing PSF disable\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[12]);
    }
    if ((SpdPcd & B_PCH_PCR_SPX_PCD_P2D) == B_PCH_PCR_SPX_PCD_P2D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller D Port 1 Fuse disabled - performing PSF disable\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[13]);
    }
    if ((SpdPcd & B_PCH_PCR_SPX_PCD_P3D) == B_PCH_PCR_SPX_PCD_P3D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller D Port 2 Fuse disabled - performing PSF disable\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[14]);
    }
    if ((SpdPcd & B_PCH_PCR_SPX_PCD_P4D) == B_PCH_PCR_SPX_PCD_P4D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller D Port 3 Fuse disabled - performing PSF disable\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[15]);
    }
    if ((SpePcd & B_PCH_PCR_SPX_PCD_P1D) == B_PCH_PCR_SPX_PCD_P1D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller E Port 0 Fuse disabled - performing PSF disable\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[16]);
    }
    if ((SpePcd & B_PCH_PCR_SPX_PCD_P2D) == B_PCH_PCR_SPX_PCD_P2D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller E Port 1 Fuse disabled - performing PSF disable\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[17]);
    }
    if ((SpePcd & B_PCH_PCR_SPX_PCD_P3D) == B_PCH_PCR_SPX_PCD_P3D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller E Port 2 Fuse disabled - performing PSF disable\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[18]);
    }
    if ((SpePcd & B_PCH_PCR_SPX_PCD_P4D) == B_PCH_PCR_SPX_PCD_P4D) {
      DEBUG ((DEBUG_INFO, "PCIe Controller E Port 3 Fuse disabled - performing PSF disable\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[19]);
    }
  }

  //
  // Get PCIE PCI regsiter from SBI in case it's disabled.
  //
  Status = PchSbiRpPciRead32 (
             0,
             R_PCH_PCIE_STRPFUSECFG,
             &Data32
             );
  if (Status == EFI_SUCCESS) {
    PcieRp1StrpFuseCfg = (Data32 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC;
    switch (PcieRp1StrpFuseCfg) {
    case 3:
      //
      // Disable RP 2/3/4 when it's set to 1 x4
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 2/3/4\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[1]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[2]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[3]);
      break;
    case 2:
      //
      // Disable RP 2/4 when it's set to 2 x2
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 2/4\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[1]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[3]);
      break;
    case 1:
      //
      // Disable RP 2 when it's set to 1 x2
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 2\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[1]);
      break;
    case 0:
    default:
      break;
    }
  }

  //
  // Get PCIE PCI regsiter from SBI in case it's disabled.
  //
  Status = PchSbiRpPciRead32 (
             4,
             R_PCH_PCIE_STRPFUSECFG,
             &Data32
             );
  if(Status == EFI_SUCCESS) {
    PcieRp5StrpFuseCfg = (Data32 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC;
    switch (PcieRp5StrpFuseCfg) {
    case 3:
      //
      // Disable RP 6/7/8 when it's set to 1 x4
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 6/7/8\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[5]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[6]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[7]);
      break;
    case 2:
      //
      // Disable RP 6/8 when it's set to 2 x2
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 6/8\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[5]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[7]);
      break;
    case 1:
      //
      // Disable RP 6 when it's set to 1 x2
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 6\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[5]);
      break;
    case 0:
    default:
      break;
    }
  }

  //
  // Get PCIE PCI regsiter from SBI in case it's disabled.
  //
  Status = PchSbiRpPciRead32 (
             8,
             R_PCH_PCIE_STRPFUSECFG,
             &Data32
             );
  if(Status == EFI_SUCCESS) {
    PcieRp9StrpFuseCfg = (Data32 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC;
    switch (PcieRp9StrpFuseCfg) {
    case 3:
      //
      // Disable RP 10/11/12 when it's set to 1 x4
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 10/11/12\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[9]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[10]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[11]);
      break;
    case 2:
      //
      // Disable RP 10/12 when it's set to 2 x2
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 10/12\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[9]);
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[11]);
      break;
    case 1:
      //
      // Disable RP 10 when it's set to 1 x2
      //
      DEBUG ((DEBUG_INFO, "Disabling PCIE RP 10\n"));
      PsfT1FunctionDisable (PID_PSF1, RpPsfBase[9]);
      break;
    case 0:
    default:
      break;
    }
  }

  //
  // Additional PCIe device for SPT-H device 29, function 4 and device 27, function 0
  //
  if (PchSeries == PchH){
    //
    // Get PCIE PCI regsiter from SBI in case it's disabled.
    // Additional PCIe device for SPT-H device 29, function 4
    //
    Status = PchSbiRpPciRead32 (
               12,
               R_PCH_PCIE_STRPFUSECFG,
               &Data32
               );
    if(Status == EFI_SUCCESS) {
      PcieRp13StrpFuseCfg = (Data32 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC;
      switch (PcieRp13StrpFuseCfg) {
        case 3:
          //
          // Disable RP 14/15/16 when it's set to 1 x4
          //
          DEBUG ((DEBUG_INFO, "Disabling PCIE RP 14/15/16\n"));
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[13]);
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[14]);
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[15]);
          break;
        case 2:
          //
          // Disable RP 14/16 when it's set to 2 x2
          //
          DEBUG ((DEBUG_INFO, "Disabling PCIE RP 14/16\n"));
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[13]);
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[15]);
          break;
        case 1:
          //
          // Disable RP 14 when it's set to 1 x2
          //
          DEBUG ((DEBUG_INFO, "Disabling PCIE RP 14\n"));
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[13]);
          break;
        case 0:
        default:
          break;
      }
    }
    //
    // Get PCIE PCI regsiter from SBI in case it's disabled.
    // Additional PCIe device for SPT-H device 27, function 0
    //
    Status = PchSbiRpPciRead32 (
               16,
               R_PCH_PCIE_STRPFUSECFG,
               &Data32
               );
    if (Status == EFI_SUCCESS) {
      PcieRp17StrpFuseCfg = (Data32 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC;
      switch (PcieRp17StrpFuseCfg) {
        case 3:
          //
          // Disable RP 18/19/20 when it's set to 1 x4
          //
          DEBUG ((DEBUG_INFO, "Disabling PCIE RP 18/19/20\n"));
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[17]);
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[18]);
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[19]);
          break;
        case 2:
          //
          // Disable RP 18/20 when it's set to 2 x2
          //
          DEBUG ((DEBUG_INFO, "Disabling PCIE RP 18/20 \n"));
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[17]);
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[19]);
          break;
        case 1:
          //
          // Disable RP 18 when it's set to 1 x2
          //
          DEBUG ((DEBUG_INFO, "Disabling PCIE RP 18\n"));
          PsfT1FunctionDisable (PID_PSF1, RpPsfBase[17]);
          break;
        case 0:
        default:
          break;
      }
    }
  }

  //
  // OTG fused
  //
  if (FuseDis2State & B_PCH_PWRM_FUSE_DIS_RD_2_OTG_FUSE_SS_DIS) {
    DEBUG ((DEBUG_INFO, "xDCI (OTG) Fuse disabled - performing PSF disable\n"));
    if (PchSeries == PchLp) {
      PchPcrAndThenOr32 (PID_PSF2, R_PCH_LP_PCR_PSF2_T0_SHDW_OTG_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN, ~0u, B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);
    } else {
      PchPcrAndThenOr32 (PID_PSF2, R_PCH_H_PCR_PSF2_T0_SHDW_OTG_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN, ~0u, B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);
    }
  }
  //
  // SATA fused
  //
  if (FuseDis2State & B_PCH_PWRM_FUSE_DIS_RD_2_SATA_FUSE_SS_DIS) {
    DEBUG ((DEBUG_INFO, "SATA Fuse disabled - performing PSF disable\n"));
    if (PchSeries == PchLp) {
      PsfT0FunctionDisable (PID_PSF1, R_PCH_LP_PCR_PSF1_T0_SHDW_SATA_REG_BASE);
    } else {
      PsfT0FunctionDisable (PID_PSF1, R_PCH_H_PCR_PSF1_T0_SHDW_SATA_REG_BASE);
    }
  }
  Mrom0RegBase = MmPciBase (
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_EVA,
                  PCI_FUNCTION_NUMBER_EVA_MROM0
                 );
  MsDevFuncHide = MmioRead32(Mrom0RegBase + R_PCH_LBG_MSUINT_MSDEVFUNCHIDE);
  if (MsDevFuncHide != 0xFFFFFFFF) {
    //
    // MROM1 fused
    //
    if (MsDevFuncHide & B_PCH_EVA_MSUNIT_MSDEVFUNCHIDE_MROM1) {
      DEBUG ((DEBUG_INFO, "MROM1 Fuse disabled - performing PSF disable\n"));
      PsfT0FunctionDisable (PID_PSF3, R_PCH_PCR_PSF3_T0_SHDW_MROM1_REG_BASE);
    }
    //
    // sSATA fused
    //
    if (MsDevFuncHide & B_PCH_EVA_MSUNIT_MSDEVFUNCHIDE_SSATA) {
      DEBUG ((DEBUG_INFO, "sSATA Fuse disabled - performing PSF disable\n"));
      PsfT0FunctionDisable (PID_PSF1, R_PCH_H_PCR_PSF1_T0_SHDW_sSATA_REG_BASE);
    }
  }
  //
  // TraceHub fused
  // Hide TraceHub Phantom Device BDF 0/20/4 using PSF (Config Disable)
  //
  PchPcrAndThenOr32 (
    PID_PSF3,
    R_PCH_PCR_PSF3_T0_SHDW_TRACE_HUB_ACPI_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS,
    (UINT32)~BIT0,
    B_PCH_PCR_PSFX_T0_SHDW_CFG_DIS_CFGDIS
    );
  //
  // Gbe fused
  //
  Status = PeiServicesLocatePpi (
               &gPeiSpiPpiGuid,
               0,
               NULL,
               (VOID **) &SpiProtocol
               );
  ASSERT_EFI_ERROR (Status);

  Status = SpiProtocol->ReadPchSoftStrap(SpiProtocol, R_PCH_SPI_SS119_PMC, 4, &SoftstrapVal);
  if (!EFI_ERROR(Status)) {
    if ((SoftstrapVal & B_PCH_SPI_SS119_PMC_GBE_DIS) == B_PCH_SPI_SS119_PMC_GBE_DIS) {
      DEBUG ((DEBUG_INFO, "GBE Fuse disabled - performing PSF disable\n"));
      PsfT0FunctionDisable (PID_PSF3, R_PCH_H_PCR_PSF3_T0_SHDW_GBE_REG_BASE);
    }
  }

  DEBUG ((DEBUG_INFO, "PchEarlyDisabledDeviceHandling() - End\n"));
}

/**
  Internal function performing miscellaneous init needed in very early PEI phase

  @param[in] PchPwrmBase       PCH Power Management Base address of this PCH device

**/
VOID
PchEarlyInit (
  IN  UINT32                      PchPwrmBase
  )
{
  UINTN                   PciPmcRegBase;
  UINT8                   Data8;

  DEBUG ((DEBUG_INFO, "PchEarlyInit() - Start\n"));

  //
  // Perform RP PSF device number and function number reset.
  //
  PchRpReloadDefaultDevFunc ();

  ///
  /// Perform PCH SKU detection to check if the RST PCIe Storage Remapping feature is supported
  ///
  PchRstPcieStorageCreEarlyInit ();

  ///
  /// Before any interrupt is enabled, set ITSS.MMC, PCR[ITSS] + 3334h[0] = 1b.
  ///
  PchPcrWrite16 (PID_ITSS, R_PCH_PCR_ITSS_MMC, (UINT16)B_PCH_PCR_ITSS_MMC_MSTRMSG_EN);

  ///
  /// SKL PCH BWG 7.2.14. Additional PCH DMI and OP-DMI Programming Steps
  /// Step 1.1
  /// Set PCR[DMI] + 2234[17,16] = 1b,1b (using byte access)
  /// This programming must be done before any upstream bus master transactions are enabled.
  ///
  PchPcrAndThenOr8 (PID_DMI, R_PCH_PCR_DMI_DMIC + 0x2, 0xFF, (BIT0 | BIT1));

  ///
  /// According to bios spec,
  /// Setup "uCode Patch Region Enable", PCR [DMI] + 2748h[0] to '0b'
  ///
  PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_UCPR, (UINT32)~B_PCH_PCR_DMI_UCPR_UPRE, 0);

  ///
  /// For GPIO and ITSS that support sideband posted write, they can support
  /// back to back write after their correspoing bit sunder P2SB PCI Config
  /// 80h-9fh are set.
  ///
  PchP2sbEarlyConfig ();

  ///
  /// PCH BIOS Spec Section 19.3 Power Failure Considerations
  /// RTC_PWR_STS bit, GEN_PMCON_B (PMC PCI offset A4h[2])
  /// When the RTC_PWR_STS bit is set, it indicates that the RTCRST# signal went low.
  /// Software should clear this bit. For example, changing the RTC battery sets this bit.
  /// System BIOS should reset CMOS to default values if the RTC_PWR_STS bit is set.
  /// The System BIOS should execute the sequence below if the RTC_PWR_STS bit is set
  /// before memory initialization. This will ensure that the RTC state machine has been
  /// initialized.
  /// 1. If the RTC_PWR_STS bit is set which indicates a new coin-cell battery insertion or a
  ///    battery failure, steps 2 through 5 should be executed.
  /// 2. Set RTC Register 0Ah[6:4] to 110b or 111b
  /// 3. Set RTC Register 0Bh[7].
  /// 4. Set RTC Register 0Ah[6:4] to 010b
  /// 5. Clear RTC Register 0Bh[7].
  ///
  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );

  if ((MmioRead8 (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B) &
      (UINT8) B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) != 0) {
    ///
    /// 2. Set RTC Register 0Ah[6:4] to 110b or 111b
    ///
    IoWrite8 (R_PCH_RTC_INDEX_ALT, (UINT8) R_PCH_RTC_REGA);
    Data8 = IoRead8 (R_PCH_RTC_INDEX_ALT) & (UINT8) ~(BIT6 | BIT5 | BIT4);
    Data8 |= (UINT8) (BIT6 | BIT5);
    IoWrite8 (R_PCH_RTC_TARGET_ALT, Data8);
    ///
    /// 3. Set RTC Register 0Bh[7].
    ///
    IoWrite8 (R_PCH_RTC_INDEX_ALT, (UINT8) R_PCH_RTC_REGB);
    IoOr8 (R_PCH_RTC_TARGET_ALT, (UINT8) B_PCH_RTC_REGB_SET);
    ///
    /// 4. Set RTC Register 0Ah[6:4] to 010b
    ///
    IoWrite8 (R_PCH_RTC_INDEX_ALT, (UINT8) R_PCH_RTC_REGA);
    Data8 = IoRead8 (R_PCH_RTC_TARGET_ALT) & (UINT8) ~(BIT6 | BIT5 | BIT4);
    Data8 |= (UINT8) (BIT5);
    IoWrite8 (R_PCH_RTC_TARGET_ALT, Data8);
    ///
    /// 5. Clear RTC Register 0Bh[7].
    ///
    IoWrite8 (R_PCH_RTC_INDEX_ALT, (UINT8) R_PCH_RTC_REGB);
    IoAnd8 (R_PCH_RTC_TARGET_ALT, (UINT8) ~B_PCH_RTC_REGB_SET);
  }
  ///
  /// PCH BIOS Spec Section 19.1 Handling Status Registers
  /// System BIOS must set 1b to clear the following registers during power-on
  /// and resuming from Sx sleep state.
  /// - PWRMBASE + Offset 10h[0] = 1b
  ///   Done in ConfigureMiscPm ()
  /// - PWRMBASE + Offset 10h[4] = 1b, needs to be done as early as possible during PEI
  /// - PWRMBASE + Offset 10h[5] = 1b
  ///   Done in ConfigureMiscPm ()
  ///
  MmioWrite32 (
    (UINTN) (PchPwrmBase + R_PCH_PWRM_PRSTS),
    (UINT32) (B_PCH_PWRM_PRSTS_FIELD_1)
    );

  //
  // Clear CF9GR if it's set in previous boot.
  //
  MmioAnd32 (PciPmcRegBase + R_PCH_PMC_ETR3, (UINT32)~B_PCH_PMC_ETR3_CF9GR);

  // Clear R_PCH_PWRM_PM_SYNC_MODE and R_PCH_PWRM_PM_SYNC_MODE_C0 to accommodate incremental updates (e.g. ADR)
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_PM_SYNC_MODE, 0x00000000);
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_PM_SYNC_MODE_C0, 0x00000000);


  PchEarlyDisabledDeviceHandling (PchPwrmBase);

  DEBUG_CODE (
    PrintPchPciConfigSpace ();
  );


  DEBUG ((DEBUG_INFO, "PchEarlyInit() - End\n"));

  return;
}

/**
  Performing Pch early init before PchPlatfromPolicy PPI produced

  @retval EFI_SUCCESS             The function completes successfully
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database
**/
EFI_STATUS
EFIAPI
PchInitPreMem (
  VOID
  )
{
  EFI_STATUS              Status;
  UINT32                  PchPwrmBase;
  UINTN                   P2sbBase;
  UINT32                  PcrBase;

  DEBUG ((DEBUG_INFO, "PchInitPreMem() - Start\n"));
  //
  // Check if Pch is supported
  //
  if (!IsPchSupported ()) {
    DEBUG ((DEBUG_ERROR, "PCH SKU is not supported due to no proper PCH LPC found!\n"));
    ASSERT (FALSE);
  }

  ///
  /// Check if SBREG has been set.
  /// If not, program it.
  ///
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );
  PcrBase  = MmioRead32 (P2sbBase + R_PCH_P2SB_SBREG_BAR);
  if ((PcrBase & B_PCH_P2SB_SBREG_RBA) == 0) {
    DEBUG ((DEBUG_INFO, "SBREG should be programmed before here\n"));
    //
    // Set SBREG base address.
    //
    MmioWrite32 (P2sbBase + R_PCH_P2SB_SBREG_BAR, PCH_PCR_BASE_ADDRESS);
    //
    // Enable the MSE bit for MMIO decode.
    //
    MmioOr8 (P2sbBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
  }

  ///
  /// Program ACPI BASE
  ///
  PchAcpiBaseSet(PcdGet16 (PcdPchAcpiIoPortBaseAddress));

  ///
  /// Program PWRM BASE
  ///
  PchPwrmBaseSet(PCH_PWRM_BASE_ADDRESS);

  ///
  /// Program TCO BASE
  ///
  PchTcoBaseSet(PcdGet16 (PcdTcoBaseAddress));

  ///
  /// Use method to get PCH PWRM Base
  ///
  Status = PchPwrmBaseGet (&PchPwrmBase);
  DEBUG ((DEBUG_INFO, "PCH PWRM Base needs to be programmed before here\n"));
  ASSERT (PchPwrmBase != 0);

  PchInstallDwrPpi();
  PchLogPsfInternalErrors();

  ///
  /// Perform miscellaneous init needed in very early PEI phase
  ///
  PchEarlyInit (PchPwrmBase);

  ///
  /// Install the PCH PEI Init PPI
  ///
  Status = PeiServicesInstallPpi (&mPpiListVariable);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "PchInitPreMem() - End\n"));

  return Status;
}

/**
  Show PCH related information
**/
VOID
PchShowInfomation (
  VOID
  )
{
DEBUG_CODE_BEGIN();
  CHAR8           Buffer[32];
  UINT32          BufferSize;
  UINTN           LpcBaseAddress;

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  BufferSize = sizeof (Buffer);
  PchGetSeriesStr (GetPchSeries (), Buffer, &BufferSize);
  DEBUG ((DEBUG_INFO, "PCH Series   : %a\n", Buffer));
  BufferSize = sizeof (Buffer);
  PchGetSteppingStr (PchStepping (), Buffer, &BufferSize);
  DEBUG ((DEBUG_INFO, "PCH Stepping : %a\n", Buffer));
  BufferSize = sizeof (Buffer);
  PchGetSkuStr (MmioRead16 (LpcBaseAddress + PCI_DEVICE_ID_OFFSET), Buffer, &BufferSize);
  DEBUG ((DEBUG_INFO, "PCH SKU      : %a\n", Buffer));
DEBUG_CODE_END();
}

/**
  PCH init pre-memory entry point
**/
VOID
EFIAPI
PchInitPrePolicy (
  VOID
  )
{
  EFI_STATUS Status;


  //
  // Dump PCH information
  //
  DEBUG_CODE (
    PchShowInfomation ();
  );


  DEBUG ((DEBUG_INFO, "PchInitPrePolicy() - Start\n"));

  //
  // Build Memory Mapped IO Resource which is used to build E820 Table in LegacyBios.
  // the resource range should be preserved in ACPI as well.
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Do not publish HOB for PCH MMIO resources here. Use RSVCHIPSET to add this memory to GCD map.
  //Status = PchPreservedMmioResource ();
  // APTIOV_SERVER_OVERRIDE_RC_END : Do not publish HOB for PCH MMIO resources here. Use RSVCHIPSET to add this memory to GCD map.

  //
  // Initialize WDT and install WDT PPI
  //
  Status = WdtPeiEntryPoint ();

  //
  // Install PCH RESET PPI
  //
  Status = InstallPchReset ();

  //
  // Installs PCH SPI PPI
  //
  Status = InstallPchSpi ();

  //
  // Perform PCH early init
  //
  Status = PchInitPreMem ();

  GpioUnlockAllGpios ();

  DEBUG ((DEBUG_INFO, "PchInitPrePolicy() - End\n"));
}

