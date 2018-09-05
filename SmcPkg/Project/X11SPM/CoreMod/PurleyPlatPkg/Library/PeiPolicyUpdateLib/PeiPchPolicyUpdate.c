//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev. 1.00
//      Bug Fixed:  Add port 61 item.
//      Reason:     For adaptec card hang on OPROM.
//      Auditor:    Kasber Chen
//      Date:       Jan/03/2017
//
//****************************************************************************
/** @file
  This file is SampleCode of the library for Intel PCH PEI Policy initialzation.

@copyright
 Copyright (c) 2004 - 2016 Intel Corporation. All rights reserved
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

#include <PeiPchPolicyUpdate.h>
#include <Register/PchRegsSata.h>
#include <Library/HobLib.h>
#include <Platform.h>
#include <Guid/PchRcVariable.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Guid/GlobalVariable.h>
#include <Ppi/PchHdaVerbTable.h>
#include "PchHdaVerbTables.h"
#include <Library/PchSerialIoLib.h>
#include <Library/SetupLib.h>
#include <Ppi/SystemBoard.h>
#include <Library/PchGbeLib.h>
#include <Library/PchInfoLib.h>
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
#include <Library/MeTypeLib.h>
#endif // ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_START
#include <Token.h>   // Based on sdl token emulating PORT 61h
#include <Library/AmiPlatformHooksLib.h>   //  Use OEM functions from AmiPlatformHooksLib.h
// APTIOV_SERVER_OVERRIDE_RC_END
#if SMCPKG_SUPPORT
#include "Setup.h"
#endif
//
// Haddock Creek
//
#define DIMM_SMB_SPD_P0C0D0_HC 0xA2
#define DIMM_SMB_SPD_P0C0D1_HC 0xA0
#define DIMM_SMB_SPD_P0C1D0_HC 0xA6
#define DIMM_SMB_SPD_P0C1D1_HC 0xA4
#define DIMM_SMB_SPD_P0C0D2_HC 0xAA
#define DIMM_SMB_SPD_P0C1D2_HC 0xA8

//
// Sawtooth Peak
// Single SPD EEPROM at 0xA2 serves both C0D0 and C1D0 (LPDDR is 1DPC only)
//
#define DIMM_SMB_SPD_P0C0D0_STP 0xA2
#define DIMM_SMB_SPD_P0C0D1_STP 0xA0
#define DIMM_SMB_SPD_P0C1D0_STP 0xA2
#define DIMM_SMB_SPD_P0C1D1_STP 0xA0

//
// Aden Hills
// DDR4 System (1DPC)
//
#define DIMM_SMB_SPD_P0C0D0_AH 0xA0
#define DIMM_SMB_SPD_P0C0D1_AH 0xA4
#define DIMM_SMB_SPD_P0C1D0_AH 0xA2
#define DIMM_SMB_SPD_P0C1D1_AH 0xA6


GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mSmbusHCRsvdAddresses[] = {
  DIMM_SMB_SPD_P0C0D0_HC,
  DIMM_SMB_SPD_P0C0D1_HC,
  DIMM_SMB_SPD_P0C1D0_HC,
  DIMM_SMB_SPD_P0C1D1_HC
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mSmbusSTPRsvdAddresses[] = {
  DIMM_SMB_SPD_P0C0D0_STP,
  DIMM_SMB_SPD_P0C0D1_STP,
  DIMM_SMB_SPD_P0C1D0_STP,
  DIMM_SMB_SPD_P0C1D1_STP
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mSmbusAHRsvdAddresses[] = {
  DIMM_SMB_SPD_P0C0D0_AH,
  DIMM_SMB_SPD_P0C0D1_AH,
  DIMM_SMB_SPD_P0C1D0_AH,
  DIMM_SMB_SPD_P0C1D1_AH
};

SYSTEM_CONFIGURATION            SetupVariables;

STATIC
EFI_STATUS
InstallVerbTable (
  IN      HDAUDIO_VERB_TABLE           *VerbTable
  )
{
  EFI_PEI_PPI_DESCRIPTOR  *VerbTablePpiDesc;
  EFI_STATUS               Status;

  VerbTablePpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (VerbTablePpiDesc != NULL);
  if (VerbTablePpiDesc == NULL) {
      return EFI_OUT_OF_RESOURCES;
  }
  VerbTablePpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  VerbTablePpiDesc->Guid  = &gPchHdaVerbTablePpiGuid;
  VerbTablePpiDesc->Ppi   = VerbTable;

  DEBUG ((DEBUG_INFO,
      "PCH HD-Audio installing verb table for vendor = 0x%04X devId = 0x%04X, "
      "revision = 0x%02X, (SDI=%X, size = %d DWORDS)\n",
      VerbTable->Header.VendorId,
      VerbTable->Header.DeviceId,
      VerbTable->Header.RevisionId,
      VerbTable->Header.SdiNo,
      VerbTable->Header.DataDwords));

  Status = PeiServicesInstallPpi (VerbTablePpiDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

static
VOID
InstallPlatformVerbTables (
    IN          UINTN                       CodecType
  )
{
  // APTIOV_SERVER_OVERRIDE_RC_START : Install Oem verbtable
  HDAUDIO_VERB_TABLE *OemVerbPtr;
  UINT8         i;

  if (CodecType == 0) { // PLATFORM_ONBOARD
    OemVerbPtr = &OnBoardOemVertable;
    
    DEBUG ((DEBUG_INFO, "Starting install onboard codec oem verb table: \n"));
    for (i = 0; i < sizeof(OnBoardOemVertable)/sizeof(HDAUDIO_VERB_TABLE); i++, OemVerbPtr++) {
      if (OemVerbPtr->Header.VendorId != 0xffff && OemVerbPtr->Header.DeviceId != 0xffff) {
        DEBUG ((DEBUG_INFO, "Index %d - \n", i));
        InstallVerbTable(OemVerbPtr);
      }
    }
    DEBUG ((DEBUG_INFO, "End install onboard codec oem verb table.\n"));
    if (i == 1) {
      DEBUG ((DEBUG_INFO, "No Oem verb table - Install default verb table from RC.\n"));
      InstallVerbTable(&HdaVerbTableAlc286S);
      InstallVerbTable(&HdaVerbTableAlc298);
      InstallVerbTable(&HdaVerbTableAlc888S);
    }
  } 
  else if (CodecType == 1) { // EXTERNAL_KIT
    OemVerbPtr = &ExtKitOemVertable;
        
    DEBUG ((DEBUG_INFO, "Starting install external kit codec oem verb table: \n"));
    for (i = 0; i < sizeof(ExtKitOemVertable)/sizeof(HDAUDIO_VERB_TABLE); i++, OemVerbPtr++) {
      if (OemVerbPtr->Header.VendorId != 0xffff && OemVerbPtr->Header.DeviceId != 0xffff) {
        DEBUG ((DEBUG_INFO, "Index %d - \n", i));
        InstallVerbTable(OemVerbPtr);
      }
    }
    if (i == 1)
      DEBUG ((DEBUG_INFO, "No Oem verb table here!\n"));
    DEBUG ((DEBUG_INFO, "End install extern kit codec oem verb table.\n"));
  }
  // APTIOV_SERVER_OVERRIDE_RC_END : Install Oem verbtable
}

EFI_STATUS
EFIAPI
UpdatePeiPchPolicy (
  IN OUT PCH_POLICY_PPI        *PchPolicy,
  IN SYSTEM_CONFIGURATION      *SetupVariables,
  IN PCH_RC_CONFIGURATION      *PchRcVariables
  )
/*++

Routine Description:

  This function performs PCH PEI Policy initialzation.

Arguments:

  PchPolicy               The PCH Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINT8                           Index;
  UINTN                           LpcBaseAddress;
  UINT8                           MaxSataPorts;
  UINT8                           BoardId;
  UINT8                           PciePort;
  UINT8                           *SmBusReservedTable;
  UINT8                           SmBusReservedNum;
#if AMT_SUPPORT
  ME_BIOS_EXTENSION_SETUP         MeBiosExtensionSetupData;
  UINTN                           VariableSize;
#endif
  PCH_USB_OVERCURRENT_PIN        *Usb20OverCurrentMappings=NULL;
  PCH_USB_OVERCURRENT_PIN        *Usb30OverCurrentMappings=NULL;
  SYSTEM_BOARD_PPI               *SystemBoard;
  PCH_USB20_AFE                  *Usb20AfeParams = NULL;
  UINT8                          VTdSupport;
  
  // APTIOV_SERVER_OVERRIDE_RC_START : PCH Slot mapping
  UINT8                          RootPortSlotNumber[] =  PCH_PtoSMap;
  // APTIOV_SERVER_OVERRIDE_RC_END : PCH Slot mapping
#if SMCPKG_SUPPORT
  EFI_GUID      SetupGuid = SETUP_GUID;
  UINTN         VarSize;
  SETUP_DATA    SetupData;
#endif
  //
  // Get SystemBoard PPI
  //
  Status = PeiServicesLocatePpi (
                            &gEfiPeiSystemBoardPpiGuid,   // GUID
                            0,                            // INSTANCE
                            NULL,                         // EFI_PEI_PPI_DESCRIPTOR
                            &SystemBoard                  // PPI
                            );
  ASSERT_EFI_ERROR (Status);

  BoardId = SystemBoard->SystemBoardIdValue();

  LpcBaseAddress = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_LPC,
                    PCI_FUNCTION_NUMBER_PCH_LPC
                    );

//
// Retrieve Setup variable
//
  Status = PeiServicesLocatePpi (
                            &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
                            0,                            // INSTANCE
                            NULL,                         // EFI_PEI_PPI_DESCRIPTOR
                            &VariableServices             // PPI
                            );
  ASSERT_EFI_ERROR (Status);

  PchPolicy->Port80Route          = PchRcVariables->IchPort80Route;

  //
  // DeviceEnables
  //
  if (PchIsGbeAvailable ()) {
    PchPolicy->LanConfig.Enable         = TRUE;
    PchPolicy->LanConfig.K1OffEnable    = PchRcVariables->PchLanK1Off;
  } else {
    PchPolicy->LanConfig.Enable         = FALSE;
  }

  PchPolicy->SataConfig.Enable          = PchRcVariables->PchSata;

  PchPolicy->sSataConfig.Enable          = PchRcVariables->PchsSata;
  PchPolicy->SmbusConfig.Enable         = TRUE;
  //
  // APTIOV_SERVER_OVERRIDE_RC_START
  // LBG and SPT Si WA s5330294 - CLOCKRUN in LPC has to be disabled
  // APTIOV_SERVER_OVERRIDE_RC_END
  //
  PchPolicy->PmConfig.PciClockRun       = FALSE;
  PchPolicy->PchConfig.Crid             = PchRcVariables->PchCrid;
  PchPolicy->PchConfig.Serm             = PchRcVariables->PchSerm;


  //
  // SMBUS reserved addresses
  //
  SmBusReservedTable = NULL;
  SmBusReservedNum   = 0;
  PchPolicy->SmbusConfig.SmbusIoBase = PCH_SMBUS_BASE_ADDRESS;
  SmBusReservedTable = mSmbusSTPRsvdAddresses;
  SmBusReservedNum   = sizeof (mSmbusSTPRsvdAddresses);

  if (SmBusReservedTable != NULL) {
    PchPolicy->SmbusConfig.NumRsvdSmbusAddresses = SmBusReservedNum;
    CopyMem (
      PchPolicy->SmbusConfig.RsvdSmbusAddressTable,
      SmBusReservedTable,
      SmBusReservedNum
      );
  }

  //
  // SATA Config
  //
  PchPolicy->SataConfig.SataMode  = PchRcVariables->SataInterfaceMode;
  MaxSataPorts = GetPchMaxSataPortNum ();

  for (Index = 0; Index < MaxSataPorts; Index++) {
    if (PchRcVariables->SataTestMode == TRUE)
    {
      PchPolicy->SataConfig.PortSettings[Index].Enable    = TRUE;
    } else {
      PchPolicy->SataConfig.PortSettings[Index].Enable = PchRcVariables->SataPort[Index];
    }
#ifdef PC_HOOK
    PchPolicy->SataConfig.PortSettings[Index].Enable           = PchRcVariables->SataPort[Index];
#endif
    PchPolicy->SataConfig.PortSettings[Index].HotPlug          = PchRcVariables->SataHotPlug[Index];
    PchPolicy->SataConfig.PortSettings[Index].SpinUp           = PchRcVariables->SataSpinUp[Index];
    PchPolicy->SataConfig.PortSettings[Index].External         = PchRcVariables->SataExternal[Index];
    PchPolicy->SataConfig.PortSettings[Index].DevSlp           = PchRcVariables->PxDevSlp[Index];
    PchPolicy->SataConfig.PortSettings[Index].EnableDitoConfig = PchRcVariables->EnableDitoConfig[Index];
    PchPolicy->SataConfig.PortSettings[Index].DmVal            = PchRcVariables->DmVal[Index];
    PchPolicy->SataConfig.PortSettings[Index].DitoVal          = PchRcVariables->DitoVal[Index];
    PchPolicy->SataConfig.PortSettings[Index].SolidStateDrive  = PchRcVariables->SataType[Index];
  }

  if (PchPolicy->SataConfig.SataMode == PchSataModeRaid) {
    PchPolicy->SataConfig.Rst.RaidAlternateId = PchRcVariables->SataAlternateId;
    PchPolicy->SataConfig.Rst.EfiRaidDriverLoad = PchRcVariables->SataRaidLoadEfiDriver;
  }
  PchPolicy->SataConfig.Rst.Raid0           = PchRcVariables->SataRaidR0;
  PchPolicy->SataConfig.Rst.Raid1           = PchRcVariables->SataRaidR1;
  PchPolicy->SataConfig.Rst.Raid10          = PchRcVariables->SataRaidR10;
  PchPolicy->SataConfig.Rst.Raid5           = PchRcVariables->SataRaidR5;
  PchPolicy->SataConfig.Rst.Irrt            = PchRcVariables->SataRaidIrrt;
  PchPolicy->SataConfig.Rst.OromUiBanner    = PchRcVariables->SataRaidOub;
  PchPolicy->SataConfig.Rst.HddUnlock       = PchRcVariables->SataHddlk;
  PchPolicy->SataConfig.Rst.LedLocate       = PchRcVariables->SataLedl;
  PchPolicy->SataConfig.Rst.IrrtOnly        = PchRcVariables->SataRaidIooe;
  PchPolicy->SataConfig.Rst.SmartStorage    = PchRcVariables->SataRaidSrt;
  PchPolicy->SataConfig.Rst.OromUiDelay     = PchRcVariables->SataRaidOromDelay;

  PchPolicy->SataConfig.EnclosureSupport    = TRUE;

  PchPolicy->SataConfig.SalpSupport     = PchRcVariables->SataSalp;
  PchPolicy->SataConfig.TestMode        = PchRcVariables->SataTestMode;

  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    if ((PchRcVariables->PchSata == TRUE) && (PchRcVariables->SataInterfaceMode == PchSataModeRaid)) {
      PchPolicy->SataConfig.RstPcieStorageRemap[Index].Enable               = PchRcVariables->RstPcieStorageRemap[Index];
      PchPolicy->SataConfig.RstPcieStorageRemap[Index].RstPcieStoragePort   = PchRcVariables->RstPcieStorageRemapPort[Index];
    } else {
      PchPolicy->SataConfig.RstPcieStorageRemap[Index].Enable               = FALSE;
    }
  }

  //
  // sSATA Config
  //
  PchPolicy->sSataConfig.SataMode  = PchRcVariables->sSataInterfaceMode;
  MaxSataPorts = GetPchMaxsSataPortNum ();

  for (Index = 0; Index < MaxSataPorts; Index++) {
    if (PchRcVariables->sSataTestMode == TRUE)
    {
      PchPolicy->sSataConfig.PortSettings[Index].Enable    = TRUE;
    } else {
      PchPolicy->sSataConfig.PortSettings[Index].Enable = PchRcVariables->sSataPort[Index];
    }
#ifdef PC_HOOK
    PchPolicy->sSataConfig.PortSettings[Index].Enable           = PchRcVariables->sSataPort[Index];
#endif
    PchPolicy->sSataConfig.PortSettings[Index].HotPlug          = PchRcVariables->sSataHotPlug[Index];
    PchPolicy->sSataConfig.PortSettings[Index].SpinUp           = PchRcVariables->sSataSpinUp[Index];
    PchPolicy->sSataConfig.PortSettings[Index].External         = PchRcVariables->sSataExternal[Index];
    PchPolicy->sSataConfig.PortSettings[Index].DevSlp           = PchRcVariables->sPxDevSlp[Index];
    PchPolicy->sSataConfig.PortSettings[Index].EnableDitoConfig = PchRcVariables->sEnableDitoConfig[Index];
    PchPolicy->sSataConfig.PortSettings[Index].DmVal            = PchRcVariables->sDmVal[Index];
    PchPolicy->sSataConfig.PortSettings[Index].DitoVal          = PchRcVariables->sDitoVal[Index];
    PchPolicy->sSataConfig.PortSettings[Index].SolidStateDrive  = PchRcVariables->sSataType[Index];
  }

  if (PchPolicy->sSataConfig.SataMode == PchSataModeRaid) {
    PchPolicy->sSataConfig.Rst.RaidAlternateId = PchRcVariables->sSataAlternateId;
    PchPolicy->sSataConfig.Rst.EfiRaidDriverLoad = PchRcVariables->sSataRaidLoadEfiDriver;
  }
  PchPolicy->sSataConfig.Rst.Raid0           = PchRcVariables->sSataRaidR0;
  PchPolicy->sSataConfig.Rst.Raid1           = PchRcVariables->sSataRaidR1;
  PchPolicy->sSataConfig.Rst.Raid10          = PchRcVariables->sSataRaidR10;
  PchPolicy->sSataConfig.Rst.Raid5           = PchRcVariables->sSataRaidR5;
  PchPolicy->sSataConfig.Rst.Irrt            = PchRcVariables->sSataRaidIrrt;
  PchPolicy->sSataConfig.Rst.OromUiBanner    = PchRcVariables->sSataRaidOub;
  PchPolicy->sSataConfig.Rst.HddUnlock       = PchRcVariables->sSataHddlk;
  PchPolicy->sSataConfig.Rst.LedLocate       = PchRcVariables->sSataLedl;
  PchPolicy->sSataConfig.Rst.IrrtOnly        = PchRcVariables->sSataRaidIooe;
  PchPolicy->sSataConfig.Rst.SmartStorage    = PchRcVariables->sSataRaidSrt;
  PchPolicy->sSataConfig.Rst.OromUiDelay     = PchRcVariables->sSataRaidOromDelay;

  PchPolicy->sSataConfig.EnclosureSupport    = TRUE;

  PchPolicy->sSataConfig.SalpSupport     = PchRcVariables->sSataSalp;
  PchPolicy->sSataConfig.TestMode        = PchRcVariables->sSataTestMode;
  //
  // Initiate DMI Configuration
  //
  if (SetupVariables->PcieDmiAspm != PLATFORM_POR) {
    if (SetupVariables->PcieDmiAspm != 0xFF) {
      PchPolicy->DmiConfig.DmiAspm = TRUE;
    } else {
      PchPolicy->DmiConfig.DmiAspm = FALSE;
    }
  }
  DEBUG((DEBUG_ERROR, "PchPolicy->DmiConfig.DmiAspm =%x\n", PchPolicy->DmiConfig.DmiAspm));
  //
  // PCI express config
  //
  PchPolicy->PcieConfig.DisableRootPortClockGating      = SetupVariables->PcieClockGating;
  PchPolicy->PcieConfig.EnablePort8xhDecode           = PchRcVariables->PcieRootPort8xhDecode;
  PchPolicy->PcieConfig.PchPciePort8xhDecodePortIndex = PchRcVariables->Pcie8xhDecodePortIndex;
  PchPolicy->PcieConfig.EnablePeerMemoryWrite         = PchRcVariables->PcieRootPortPeerMemoryWriteEnable;
  PchPolicy->PcieConfig.ComplianceTestMode            = PchRcVariables->PcieComplianceTestMode;

  /// Temporary WA: Force Link speed on BMC board to GEN1
  PciePort = SystemBoard->GetBmcPciePort();
  if (PciePort != 0xFF) {
    DEBUG((DEBUG_INFO, "WA Force Link Speed to GEN1: PciePort: %d", PciePort));
    PchRcVariables->PcieRootPortSpeed[PciePort] =1;
  }

  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    PchPolicy->PcieConfig.RootPort[Index].Enable                         = PchRcVariables->PcieRootPortEn[Index];

    //APTIOV_SERVER_OVERRIDE_RC_START: PCH Slot mapping
    PchPolicy->PcieConfig.RootPort[Index].PhysicalSlotNumber             = RootPortSlotNumber[Index];
    //APTIOV_SERVER_OVERRIDE_RC_END: PCH Slot mapping

    if (PchRcVariables->PchPcieGlobalAspm > PchPcieAspmDisabled) {
      // Disabled a.k.a. Per individual port
      PchPolicy->PcieConfig.RootPort[Index].Aspm                         = PchRcVariables->PchPcieGlobalAspm;
    } else {
      PchPolicy->PcieConfig.RootPort[Index].Aspm                         = PchRcVariables->PcieRootPortAspm[Index];
    }
#if X11SPM_SUPPORT
    // Disable ASPM for M.2 due to system get BSOD sometimes with M.2 installed
    if(Index==0)
        PchPolicy->PcieConfig.RootPort[Index].Aspm = PchPcieAspmDisabled;
#endif
    PchPolicy->PcieConfig.RootPort[Index].L1Substates                    = PchRcVariables->PcieRootPortL1SubStates[Index];
    PchPolicy->PcieConfig.RootPort[Index].AcsEnabled                     = PchRcVariables->PcieRootPortACS[Index];
    PchPolicy->PcieConfig.RootPort[Index].PmSci                          = PchRcVariables->PcieRootPortPMCE[Index];
    PchPolicy->PcieConfig.RootPort[Index].HotPlug                        = PchRcVariables->PcieRootPortHPE[Index];
    PchPolicy->PcieConfig.RootPort[Index].AdvancedErrorReporting         = PchRcVariables->PcieRootPortAER[Index];
    PchPolicy->PcieConfig.RootPort[Index].UnsupportedRequestReport       = PchRcVariables->PcieRootPortURE[Index];
    PchPolicy->PcieConfig.RootPort[Index].FatalErrorReport               = PchRcVariables->PcieRootPortFEE[Index];
    PchPolicy->PcieConfig.RootPort[Index].NoFatalErrorReport             = PchRcVariables->PcieRootPortNFE[Index];
    PchPolicy->PcieConfig.RootPort[Index].CorrectableErrorReport         = PchRcVariables->PcieRootPortCEE[Index];
    PchPolicy->PcieConfig.RootPort[Index].SystemErrorOnFatalError        = PchRcVariables->PcieRootPortSFE[Index];
    PchPolicy->PcieConfig.RootPort[Index].SystemErrorOnNonFatalError     = PchRcVariables->PcieRootPortSNE[Index];
    PchPolicy->PcieConfig.RootPort[Index].SystemErrorOnCorrectableError  = PchRcVariables->PcieRootPortSCE[Index];
    PchPolicy->PcieConfig.RootPort[Index].TransmitterHalfSwing           = PchRcVariables->PcieRootPortTHS[Index];
    PchPolicy->PcieConfig.RootPort[Index].CompletionTimeout              = PchRcVariables->PcieRootPortCompletionTimeout[Index];
    PchPolicy->PcieConfig.RootPort[Index].PcieSpeed                      = PchRcVariables->PcieRootPortSpeed[Index];

    PchPolicy->PcieConfig.RootPort[Index].MaxPayload                     = PchRcVariables->PcieRootPortMaxPayLoadSize[Index];
    PchPolicy->PcieConfig.RootPort[Index].Gen3EqPh3Method                = PchRcVariables->PcieRootPortEqPh3Method[Index];
  }

  for (Index = 0; Index < GetPchMaxPciePortNum (); ++Index) {
    PchPolicy->PcieConfig.EqPh3LaneParam[Index].Cm  = PchRcVariables->PcieLaneCm[Index];
    PchPolicy->PcieConfig.EqPh3LaneParam[Index].Cp  = PchRcVariables->PcieLaneCp[Index];
  }
  if (PchRcVariables->PcieSwEqOverride) {
    for (Index = 0; Index < PCH_PCIE_SWEQ_COEFFS_MAX; Index++) {
      PchPolicy->PcieConfig2.SwEqCoeffList[Index].Cm     = PchRcVariables->PcieSwEqCoeffCm[Index];
      PchPolicy->PcieConfig2.SwEqCoeffList[Index].Cp     = PchRcVariables->PcieSwEqCoeffCp[Index];
    }
  }

  PchPolicy->PcieConfig.MaxReadRequestSize                               = PchRcVariables->PcieRootPortMaxReadRequestSize;
  ///
  /// Update Competion Timeout settings for Upling ports for Server PCH
  ///
  PchPolicy->PcieConfig.PchPcieUX16CompletionTimeout                     = PchRcVariables->PchPcieUX16CompletionTimeout;
  PchPolicy->PcieConfig.PchPcieUX8CompletionTimeout                      = PchRcVariables->PchPcieUX8CompletionTimeout;
  ///
  /// Update Max Payload Size settings for Upling ports for Server PCH
  ///
  PchPolicy->PcieConfig.PchPcieUX16MaxPayload                            = PchRcVariables->PchPcieUX16MaxPayloadSize;
  PchPolicy->PcieConfig.PchPcieUX8MaxPayload                             = PchRcVariables->PchPcieUX8MaxPayloadSize;
  Status = GetOptionData(&gEfiSocketIioVariableGuid, OFFSET_OF (SOCKET_IIO_CONFIGURATION, VTdSupport), &VTdSupport, sizeof(VTdSupport));
  PchPolicy->PcieConfig.VTdSupport                                       = VTdSupport;

  ///
  /// Assign ClkReq signal to root port. (Base 0)
  /// For LP, Set 0 - 5
  /// For H,  Set 0 - 15
  /// Note that if GbE is enabled, ClkReq assigned to GbE will not be available for Root Port. (TODO for Purley)
  ///
  //
  // HdAudioConfig
  //
  PchPolicy->HdAudioConfig.Enable               = PchRcVariables->PchHdAudio;
  PchPolicy->HdAudioConfig.DspEnable            = FALSE;
  PchPolicy->HdAudioConfig.Pme                  = PchRcVariables->PchHdAudioPme;
  PchPolicy->HdAudioConfig.IoBufferOwnership    = PchRcVariables->PchHdAudioIoBufferOwnership;
  PchPolicy->HdAudioConfig.IoBufferVoltage      = PchRcVariables->PchHdAudioIoBufferVoltage;
  PchPolicy->HdAudioConfig.ResetWaitTimer       = 300;
  PchPolicy->HdAudioConfig.IDispCodecDisconnect = TRUE;  //iDisp is permanently disabled
  for(Index = 0; Index < HDAUDIO_FEATURES; Index++) {
    PchPolicy->HdAudioConfig.DspFeatureMask |= (UINT32)(PchRcVariables->PchHdAudioFeature[Index] ? (1 << Index) : 0);
  }

  for(Index = 0; Index < HDAUDIO_PP_MODULES; Index++) {
    PchPolicy->HdAudioConfig.DspPpModuleMask |= (UINT32)(PchRcVariables->PchHdAudioPostProcessingMod[Index] ? (1 << Index) : 0);
  }

  if (PchPolicy->HdAudioConfig.Enable) {
    InstallPlatformVerbTables (PchRcVariables->PchHdAudioCodecSelect);
  }

  PchPolicy->HdAudioConfig.VcType = PchRcVariables->DfxHdaVcType;
  //
  // LockDown
  //


    PchPolicy->LockDownConfig.RtcLock          = PchRcVariables->PchRtcLock;
#ifdef PC_HOOK
    PchPolicy->LockDownConfig.BiosLock         = TRUE;
    PchPolicy->LockDownConfig.SpiEiss          = FALSE;
#else
    PchPolicy->LockDownConfig.BiosLock         = PchRcVariables->PchBiosLock;
    PchPolicy->LockDownConfig.SpiEiss          = TRUE;
#endif
    PchPolicy->LockDownConfig.GlobalSmi        = TRUE;
    PchPolicy->LockDownConfig.BiosInterface    = TRUE;
    PchPolicy->LockDownConfig.EvaLockDown      = PchRcVariables->PchEvaLockDown;
    PchPolicy->LockDownConfig.GpioLockDown     = PchRcVariables->PchGpioLockDown;
    PchPolicy->LockDownConfig.TcoLock          = TRUE;

  if(PchRcVariables->PchP2sbUnlock) {
    PchPolicy->P2sbConfig.SbiUnlock = TRUE;
    PchPolicy->P2sbConfig.PsfUnlock = TRUE;
  } else {
    PchPolicy->P2sbConfig.SbiUnlock = FALSE;
    PchPolicy->P2sbConfig.PsfUnlock = FALSE;
  }
  PchPolicy->P2sbConfig.P2SbReveal = PchRcVariables->PchP2sbDevReveal;

  //
  // Update SPI policies
  //
  PchPolicy->SpiConfig.ShowSpiController = TRUE;

  //
  // DFX PMC Policy
  //
  PchPolicy->PmConfig.PmcReadDisable = PchRcVariables->PmcReadDisable;


  if (PchRcVariables->PchAdrEn != PLATFORM_POR) {
    PchPolicy->AdrConfig.PchAdrEn = PchRcVariables->PchAdrEn;
  }
  PchPolicy->AdrConfig.AdrGpioSel = PchRcVariables->AdrGpioSel;
  if (PchRcVariables->AdrHostPartitionReset != PLATFORM_POR) {
    PchPolicy->AdrConfig.AdrHostPartitionReset = PchRcVariables->AdrHostPartitionReset;
  }
  if (PchRcVariables->AdrTimerEn != PLATFORM_POR) {
    PchPolicy->AdrConfig.AdrTimerEn = PchRcVariables->AdrTimerEn;
  }
  if (PchRcVariables->AdrTimerVal != ADR_TMR_SETUP_DEFAULT_POR) {
    PchPolicy->AdrConfig.AdrTimerVal = PchRcVariables->AdrTimerVal;
  }
  if (PchRcVariables->AdrMultiplierVal != ADR_MULT_SETUP_DEFAULT_POR) {
    PchPolicy->AdrConfig.AdrMultiplierVal = PchRcVariables->AdrMultiplierVal;
  }
// APTIOV_SERVER_OVERRIDE_RC_START : EISS bit can't be set before NVRAMSMI
  PchPolicy->LockDownConfig.SpiEiss = PchRcVariables->PchSmmBwp;
// APTIOV_SERVER_OVERRIDE_RC_END : EISS bit can't be set before NVRAMSMI

  //
  // Thermal Config
  //
  if ((PchRcVariables->MemoryThermalManagement != FALSE) &&
      ((PchRcVariables->ExttsViaTsOnBoard != FALSE) || (PchRcVariables->ExttsViaTsOnDimm != FALSE)))
  {
    PchPolicy->ThermalConfig.MemoryThrottling.Enable                                     = TRUE;
    PchPolicy->ThermalConfig.MemoryThrottling.TsGpioPinSetting[TsGpioC].PmsyncEnable     = TRUE;
    PchPolicy->ThermalConfig.MemoryThrottling.TsGpioPinSetting[TsGpioD].PmsyncEnable     = TRUE;
    PchPolicy->ThermalConfig.MemoryThrottling.TsGpioPinSetting[TsGpioC].C0TransmitEnable = TRUE;
    PchPolicy->ThermalConfig.MemoryThrottling.TsGpioPinSetting[TsGpioD].C0TransmitEnable = TRUE;
    PchPolicy->ThermalConfig.MemoryThrottling.TsGpioPinSetting[TsGpioC].PinSelection     = 1;
    PchPolicy->ThermalConfig.MemoryThrottling.TsGpioPinSetting[TsGpioD].PinSelection     = 0;
  } else {
    PchPolicy->ThermalConfig.MemoryThrottling.Enable = FALSE;
  }

  //
  // IOAPIC Config
  //
  PchPolicy->IoApicConfig.IoApicEntry24_119 = PchRcVariables->PchIoApic24119Entries;
  PchPolicy->IoApicConfig.BdfValid          = 1;
  PchPolicy->IoApicConfig.BusNumber         = 0xF0;
  PchPolicy->IoApicConfig.DeviceNumber      = 0x1F;
  PchPolicy->IoApicConfig.FunctionNumber    = 0;


  //
  // Misc PM Config
  //
  PchPolicy->PmConfig.PchDeepSxPol                          = PchRcVariables->DeepSxMode;
  PchPolicy->PmConfig.WakeConfig.WolEnableOverride          = PchRcVariables->PchWakeOnLan;
  PchPolicy->PmConfig.WakeConfig.WoWlanEnable               = PchRcVariables->PchWakeOnWlan;
  PchPolicy->PmConfig.WakeConfig.WoWlanDeepSxEnable         = PchRcVariables->PchWakeOnWlanDeepSx;
  PchPolicy->PmConfig.WakeConfig.Gp27WakeFromDeepSx         = PchRcVariables->Gp27WakeFromDeepSx;
  PchPolicy->PmConfig.SlpLanLowDc                           = PchRcVariables->PchSlpLanLowDc;
  PchPolicy->PmConfig.PowerResetStatusClear.MeWakeSts       = TRUE;
  PchPolicy->PmConfig.PowerResetStatusClear.MeHrstColdSts   = TRUE;
  PchPolicy->PmConfig.PowerResetStatusClear.MeHrstWarmSts   = TRUE;
  PchPolicy->PmConfig.PciePllSsc                            = PchRcVariables->PciePllSsc;

  PchPolicy->PmConfig.DirtyWarmReset                        = PchRcVariables->Dwr_Enable;
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  if (MeTypeIsAmt()) {
    // Disable DirtyWarmReset for workstation
    PchPolicy->PmConfig.DirtyWarmReset = FALSE;
  }
#endif // ME_SUPPORT_FLAG
  PchPolicy->PmConfig.StallDirtyWarmReset                   = PchRcVariables->Dwr_Stall;
  PchPolicy->PmConfig.Dwr_BmcRootPort                       = PchRcVariables->Dwr_BmcRootPort;

  PchPolicy->PmConfig.PchGbl2HostEn.Bits.PMCGBL             = PchRcVariables->DwrEn_PMCGBL;
  PchPolicy->PmConfig.PchGbl2HostEn.Bits.MEWDT              = PchRcVariables->DwrEn_MEWDT;
  PchPolicy->PmConfig.PchGbl2HostEn.Bits.IEWDT              = PchRcVariables->DwrEn_IEWDT;

  PchPolicy->PmConfig.Dwr_MeResetPrepDone                   = PchRcVariables->Dwr_MeResetPrepDone;
  PchPolicy->PmConfig.Dwr_IeResetPrepDone                   = PchRcVariables->Dwr_IeResetPrepDone;

  //
  // DefaultSvidSid Config
  //
#ifdef PC_HOOK
  PchPolicy->PchConfig.SubSystemVendorId     = 0;
  PchPolicy->PchConfig.SubSystemId           = 0;
#else
  PchPolicy->PchConfig.SubSystemVendorId     = V_PCH_INTEL_VENDOR_ID;
  PchPolicy->PchConfig.SubSystemId           = V_PCH_DEFAULT_SID;
#endif
  PchPolicy->PchConfig.EnableClockSpreadSpec =  PchRcVariables->EnableClockSpreadSpec;
  //
  // Thermal Config
  //
  PchPolicy->ThermalConfig.ThermalThrottling.TTLevels.PchCrossThrottling = PchRcVariables->PchCrossThrottling;
  PchPolicy->ThermalConfig.ThermalThrottling.DmiHaAWC.SuggestedSetting   = TRUE;
  if (PchRcVariables->ThermalDeviceEnable == PchThermalDeviceAuto) {
    if (PchStepping () == LbgA0) {
      PchPolicy->ThermalConfig.ThermalDeviceEnable = PchThermalDeviceDisabled;
    } else {
#ifdef SKXD_EN
      PchPolicy->ThermalConfig.ThermalDeviceEnable = PchThermalDeviceDisabled;
#else
      PchPolicy->ThermalConfig.ThermalDeviceEnable = PchThermalDeviceEnabledPci;
#endif
    }
  } else {
    PchPolicy->ThermalConfig.ThermalDeviceEnable = PchRcVariables->ThermalDeviceEnable;
  }

  PchPolicy->ThermalConfig.ThermalThrottling.TTLevels.SuggestedSetting   = TRUE;
  PchPolicy->ThermalConfig.ThermalThrottling.SataTT.SuggestedSetting     = TRUE;
  PchPolicy->ThermalConfig.ThermalThrottling.sSataTT.SuggestedSetting     = TRUE;

#ifdef PC_HOOK
  ///Set PCHHOT# to 103C.
  ///This avoids fan boost due to PchThermTrip.
  ///
  PchPolicy->ThermalConfig.PchHotLevel = 0x132;
#endif
  //
  // DCI (EXI)
  //
  PchPolicy->DciConfig.DciEn         = PchRcVariables->PchDciEn;
  PchPolicy->DciConfig.DciAutoDetect = PchRcVariables->PchDciAutoDetect;


  //
  // Initialize Serial IRQ Config
  //
  PchPolicy->SerialIrqConfig.SirqEnable       = TRUE;
  PchPolicy->SerialIrqConfig.StartFramePulse  = PchSfpw4Clk;
  if (PchRcVariables->PchSirqMode == 0) {
    PchPolicy->SerialIrqConfig.SirqMode = PchQuietMode;
  } else {
    PchPolicy->SerialIrqConfig.SirqMode = PchContinuousMode;
  }

  //
  // Port 61h emulation
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Based on sdl token emulating PORT 61h
#if SMCPKG_SUPPORT
  VarSize = sizeof(SETUP_DATA);
  Status = VariableServices->GetVariable(
                  VariableServices,
                  L"Setup",
                  &SetupGuid,
                  NULL,
                  &VarSize,
                  &SetupData);
  
  if(!Status)
      PchPolicy->Port61hSmmConfig.Enable = SetupData.PchPort61hSmm;
  else
      PchPolicy->Port61hSmmConfig.Enable = PORT61H_EMULATION;
#else
  PchPolicy->Port61hSmmConfig.Enable = PORT61H_EMULATION;
#endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Based on sdl token emulating PORT 61h

#if AMT_SUPPORT
  VariableSize = sizeof (ME_BIOS_EXTENSION_SETUP);
  Status = VariableServices->GetVariable (
                              VariableServices,
                              ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
                              &gMeBiosExtensionSetupGuid,
                              NULL,
                              &VariableSize,
                              &MeBiosExtensionSetupData
                              );
#endif // AMT_SUPPORT

  //
  // DMI configuration
  //
  PchPolicy->DmiConfig.DmiLinkDownHangBypass = PchRcVariables->DmiLinkDownHangBypass;
  PchPolicy->DmiConfig.DmiStopAndScreamEnable = PchRcVariables->PcieDmiStopAndScreamEnable;

  //
  // Update Pch Usb Config
  //
  SystemBoard->GetUsbConfig(
    &Usb20OverCurrentMappings,
    &Usb30OverCurrentMappings,
    &Usb20AfeParams
    );
  UpdatePchUsbConfig (
    &PchPolicy->UsbConfig,
    SetupVariables,
    PchRcVariables,
    Usb20OverCurrentMappings,
    Usb30OverCurrentMappings,
    Usb20AfeParams
    );

  Status = InstallVerbTable (&HdaVerbTableAlc892);

#ifdef PC_HOOK
  DEBUG((DEBUG_INFO, "close USB 1.1 port\n"));
  switch (BoardId) {
  case TypeWolfPass:
    PchPolicy->UsbConfig.PortUsb20[0].Enable = FALSE;
    break;
  case TypeBuchananPass:
    PchPolicy->UsbConfig.PortUsb20[6].Enable = FALSE;
    break;
  case TypeSawtoothPass:
    PchPolicy->UsbConfig.PortUsb20[6].Enable = FALSE;
    break;
  default:
    break;
   }
#endif  // #ifdef PC_HOOK

#ifdef SKXD_EN
  DEBUG((DEBUG_INFO, "Disable USB 1.1 port\n"));
  PchPolicy->UsbConfig.PortUsb20[2].Enable = FALSE;
#endif  // #ifdef SKXD_EN


  //
  // Update TraceHub config based on setup options
  //
  PchPolicy->PchTraceHubConfig.EnableMode        = PchRcVariables->TraceHubEnableMode;

  switch (PchRcVariables->MemRegion0BufferSize) {
    case 0:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0; // No memory
       break;
    case 1:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x100000;  // 1MB
       break;
    case 2:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x800000;  // 8MB
       break;
    case 3:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x4000000; // 64MB
       break;
  }

  switch (PchRcVariables->MemRegion1BufferSize) {
    case 0:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0; // No memory
       break;
    case 1:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x100000;  // 1MB
       break;
    case 2:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x800000;  // 8MB
       break;
    case 3:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x4000000; // 64MB
       break;
  }



  //
  // Initialize PTSS board specyfic HSIO settings
  //
  SystemBoard->InstallPlatformHsioPtssTable(PchRcVariables, PchPolicy);

  PchPolicy->PchTraceHubConfig.PchTraceHubHide  =  PchRcVariables->PchTraceHubHide;
  // APTIOV_SERVER_OVERRIDE_RC_START : Oem Hook to Override PchPolicy
  OemPchPolicyOverride ((VOID *)PchPolicy, (VOID *) SetupVariables, (VOID *) PchRcVariables);
  // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hook to Override PchPolicy

  return Status;
}

