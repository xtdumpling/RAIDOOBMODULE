/** @file
  Initializes PCH thermal controller.

@copyright
  Copyright (c) 2013 - 2015 Intel Corporation. All rights reserved
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
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci30.h>
#include <PchAccess.h>


#include <IncludePrivate/Library/GpioPrivateLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>

#include <Library/MmPciBaseLib.h>

#include <Ppi/PchPolicy.h>

#include <CpuRegs.h>
#include <Library/CpuPpmLib.h>

/**
  SATA throttling actions

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
**/
VOID
SataThrottlingConfiguration (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  UINTN                                 SataPciBase;
  PCH_SERIES                            PchSeries;
  UINT16                                PortShift0Detect;
  UINT16                                PortShift1Detect;
  UINT8                                 Index;
  UINTN                                 MaxSataPortNum;
  UINT32                                Data32And;
  UINT32                                Data32Or;
    UINTN                         sSataPciBase;

  SataPciBase    = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SATA,
                     PCI_FUNCTION_NUMBER_PCH_SATA
                     );

  PchSeries      = GetPchSeries ();
  MaxSataPortNum = GetPchMaxSataPortNum ();

  ///
  /// SATA throttling action
  /// Program SATA Indexed Register Index and Data:
  /// a. If port 0 is empty, set SATA PCI offset A0h = 0Ch (PCH-H) or A0h (PCH-LP) and SATA PCI offset A4h[15:00] = 0000h
  ///    else if Port 0 has a HDD, set SATA PCI offset A4h[15:00] = 0039h
  ///    else if Port 0 has a SSD, set SATA PCI offset A4h[15:00] = 0F39h
  /// b. If port 1 is empty, set SATA PCI offset A0h = 0Ch (PCH-H) or A0h (PCH-LP) and SATA PCI offset A4h[31:16] = 0000h
  ///    else if Port 1 has a HDD, set SATA PCI offset A4h[31:16] = 0039h
  ///    else if Port 1 has a SSD, set SATA PCI offset A4h[31:16] = 0F39h
  /// c. If port 2 is empty, set SATA PCI offset A0h = 1Ch (PCH-H) or A4h (PCH-LP) and SATA PCI offset A4h[15:00] = 0000h
  ///    else if Port 2 has a HDD, set SATA PCI offset A4h[15:00] = 0039h
  ///    else if Port 2 has a SSD, set SATA PCI offset A4h[15:00] = 0F39h
  /// d. If port 3 is empty, set SATA PCI offset A0h = 1Ch (PCH-H) or A4h (PCH-LP) and SATA PCI offset A4h[31:16] = 0000h
  ///    else if Port 3 has a HDD, set SATA PCI offset A4h[31:16] = 0039h
  ///    else if Port 3 has a SSD, set SATA PCI offset A4h[31:16] = 0F39h
  ///
  /// e. If port 4 is empty, set SATA PCI offset A0h = 2Ch (PCH-H only) and SATA PCI offset A4h[15:00] = 0000h
  ///    else if Port 4 has a HDD, set SATA PCI offset A4h[15:00] = 0039h
  ///    else if Port 4 has a SSD, set SATA PCI offset A4h[15:00] = 0F39h
  /// f. If port 5 is empty, set SATA PCI offset A0h = 2Ch (PCH-H only) and SATA PCI offset A4h[31:16] = 0000h
  ///    else if Port 5 has a HDD, set SATA PCI offset A4h[31:16] = 0039h
  ///    else if Port 5 has a SSD, set SATA PCI offset A4h[31:16] = 0F39h
  /// e. If port 6 is empty, set SATA PCI offset A0h = 3Ch (PCH-H only) and SATA PCI offset A4h[15:00] = 0000h
  ///    else if Port 6 has a HDD, set SATA PCI offset A4h[15:00] = 0039h
  ///    else if Port 6 has a SSD, set SATA PCI offset A4h[15:00] = 0F39h
  /// f. If port 7 is empty, set SATA PCI offset A0h = 3Ch (PCH-H only) and SATA PCI offset A4h[31:16] = 0000h
  ///    else if Port 7 has a HDD, set SATA PCI offset A4h[31:16] = 0039h
  ///    else if Port 7 has a SSD, set SATA PCI offset A4h[31:16] = 0F39h
  ///
  for (Index = 0; Index < ((MaxSataPortNum + 1) / 2); Index++) {
    Data32And     = 0;
    Data32Or      = 0;

    if (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.SuggestedSetting == TRUE) {
      if (PchSeries == PchH) {
        PortShift0Detect   = MmioRead16 (SataPciBase + R_PCH_H_SATA_PCS + 2) & (BIT0 << (Index * 2));
        PortShift1Detect   = MmioRead16 (SataPciBase + R_PCH_H_SATA_PCS + 2) & (BIT1 << (Index * 2));
      } else {
        PortShift0Detect   = MmioRead16 (SataPciBase + R_PCH_LP_SATA_PCS) & (B_PCH_LP_SATA_PCS_P0P << (Index * 2));
        PortShift1Detect   = MmioRead16 (SataPciBase + R_PCH_LP_SATA_PCS) & (B_PCH_LP_SATA_PCS_P1P << (Index * 2));
      }
      if (PortShift0Detect != 0) {
        Data32Or |= 0x00000039;
        if (PchPolicyPpi->SataConfig.PortSettings[0 + (Index * 2)].SolidStateDrive == TRUE) {
          Data32Or |= 0x00000F00;
        }
      }
      if (PortShift1Detect != 0) {
        Data32Or |= 0x00390000;
        if (PchPolicyPpi->SataConfig.PortSettings[1 + (Index * 2)].SolidStateDrive == TRUE) {
          Data32Or |= 0x0F000000;
        }
      }
    } else {
      Data32Or  = (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P1TDispFinit << 31) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P1Tinact << 26) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P1TDisp << 24) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P1T3M << 20) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P1T2M << 18) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P1T1M << 16) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P0TDispFinit << 15) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P0Tinact << 10) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P0TDisp << 8) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P0T3M << 4) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P0T2M << 2) |
                  (UINT32)  PchPolicyPpi->ThermalConfig.ThermalThrottling.SataTT.P0T1M;
    }

    if (PchSeries == PchH) {
      MmioWrite8 (SataPciBase + R_PCH_SATA_SIRI, (0x0C + (Index * 0x10)));
      MmioAndThenOr32 ((UINTN) (SataPciBase + R_PCH_SATA_STRD), Data32And, Data32Or);
    } else {
      MmioWrite8 (SataPciBase + R_PCH_SATA_SIRI, (0xA0 + (Index * 4)));
      MmioAndThenOr32 ((UINTN) (SataPciBase + R_PCH_SATA_STRD), Data32And, Data32Or);
    }
  }

    ///
    /// sSATA throttling action
    /// Program sSATA Indexed Register Index and Data exactly the same as for SATA
    ///
    sSataPciBase    = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_SSATA,
                       PCI_FUNCTION_NUMBER_PCH_SSATA
                       );

    for (Index = 0; Index < (GetPchMaxsSataPortNum () / 2); Index++) {
      Data32And     = 0;
      Data32Or      = 0;

      if (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.SuggestedSetting == TRUE) {
        if (PchSeries == PchH) {
          PortShift0Detect   = MmioRead16 (sSataPciBase + R_PCH_H_SATA_PCS + 2) & (BIT0 << (Index * 2));
          PortShift1Detect   = MmioRead16 (sSataPciBase + R_PCH_H_SATA_PCS + 2) & (BIT1 << (Index * 2));
        } else {
          PortShift0Detect   = MmioRead16 (sSataPciBase + R_PCH_LP_SATA_PCS) & (B_PCH_LP_SATA_PCS_P0P << (Index * 2));
          PortShift1Detect   = MmioRead16 (sSataPciBase + R_PCH_LP_SATA_PCS) & (B_PCH_LP_SATA_PCS_P1P << (Index * 2));
        }
        if (PortShift0Detect != 0) {
          Data32Or |= 0x00000039;
          if (PchPolicyPpi->sSataConfig.PortSettings[0 + (Index * 2)].SolidStateDrive == TRUE) {
            Data32Or |= 0x00000F00;
          }
        }
        if (PortShift1Detect != 0) {
          Data32Or |= 0x00390000;
          if (PchPolicyPpi->sSataConfig.PortSettings[1 + (Index * 2)].SolidStateDrive == TRUE) {
            Data32Or |= 0x0F000000;
          }
        }
      } else {
        Data32Or  = (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P1TDispFinit << 31) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P1Tinact << 26) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P1TDisp << 24) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P1T3M << 20) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P1T2M << 18) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P1T1M << 16) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P0TDispFinit << 15) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P0Tinact << 10) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P0TDisp << 8) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P0T3M << 4) |
                    (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P0T2M << 2) |
                    (UINT32)  PchPolicyPpi->ThermalConfig.ThermalThrottling.sSataTT.P0T1M;
      }

      if (PchSeries == PchH) {
        MmioWrite8 (sSataPciBase + R_PCH_SATA_SIRI, (0x0C + (Index * 0x10)));
        MmioAndThenOr32 ((UINTN) (sSataPciBase + R_PCH_SATA_STRD), Data32And, Data32Or);
      } else {
        MmioWrite8 (sSataPciBase + R_PCH_SATA_SIRI, (0xA0 + (Index * 4)));
        MmioAndThenOr32 ((UINTN) (sSataPciBase + R_PCH_SATA_STRD), Data32And, Data32Or);
      }
    }

}

/**
  Perform Thermal Management Support initialization

  @param[in] PchPolicyPpi         The PCH Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI
                                  devices to be used to initialize MMIO
                                  registers.

  @retval EFI_SUCCESS             Succeeds.
**/
EFI_STATUS
PchThermalInit (
  IN  PCH_POLICY_PPI            *PchPolicyPpi
  )
{
  UINTN                         ThermalPciBase;
  UINT32                        PchPwrmBase;
  UINT32                        ThermalBase;
  UINT32                        Data32And;
  UINT32                        Data32Or;
  UINT16                        Data16;
  UINT32                        PchTTLevels;
  MSR_REGISTER                  TempMsr;
  UINT32                        temperature;
  PCH_SERIES                    PchSeries;
  UINT8                         OrgCmd;
  UINT32                        OrgTbarLow;
  UINT32                        OrgTbarHigh;

  DEBUG ((DEBUG_INFO, "PchThermalInit () Start\n"));
  OrgCmd      = 0;
  OrgTbarLow  = 0;
  OrgTbarHigh = 0;

  ThermalPciBase = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_THERMAL,
                     PCI_FUNCTION_NUMBER_PCH_THERMAL
                     );

  PchSeries   = GetPchSeries ();
  if (PchPolicyPpi->ThermalConfig.ThermalDeviceEnable == 2) {
    ThermalBase = PCH_THERMAL_BASE_ADDRESS;
  } else {
  ThermalBase = PchPolicyPpi->TempMemBaseAddr;
  }
  PchPwrmBaseGet (&PchPwrmBase);

  ///
  /// Thermal PCI offset 10h[31:0], with a 64-bit BAR for BIOS.
  /// Enable MSE of Thermal PCI offset 04h[1] to enable memory decode
  ///
  MmioAnd8 (ThermalPciBase + PCI_COMMAND_OFFSET, (UINT8)~EFI_PCI_COMMAND_MEMORY_SPACE);
  MmioWrite32 (ThermalPciBase + R_PCH_THERMAL_TBAR, ThermalBase);
  MmioWrite32 (ThermalPciBase + R_PCH_THERMAL_TBARH, 0);
  MmioOr8 (ThermalPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// Thermal Subsystem Device Initialization
  /// The System BIOS must perform the following steps to initialize the PCH thermal subsystem device.
  /// Step 1
  /// Enable Thermal Subsystem device by making sure FD.TTD is cleared.
  /// The default value of FD.TTD is cleared.
  ///
  /// Step 2
  /// Optionally program Thermal controller Interrupt Pin/Route registers
  /// Left this to platform code
  ///
  /// Step 3
  /// Go through general PCI enumeration and assign standard PCI resource, etc.
  /// Left this to platform code
  ///
  /// Step 4
  /// Initialize relevant Thermal subsystems for the desired features.
  ///

  ///
  /// Step 5
  /// BIOS needs to perform the following steps prior to end of POST to free up the PCI resources
  /// and hide the thermal subsystem device(OPTIONAL).
  ///   Clear the memory and bus master enable bit of thermal controller
  ///   Release and clear memory resource assigned in TBAR (PCI offset 10h~13h)
  ///   Hide thermal controller PCI space by setting TCFD.TCD.
  ///
  if (PchPolicyPpi->ThermalConfig.ThermalDeviceEnable == 0) {
    ///
    /// Set Thermal Controller Function Disable (TCFD) TBAR + 0xF0 [0] to 1
    ///
    MmioOr8 (
      ThermalBase + R_PCH_TBAR_TCFD,
      B_PCH_TBAR_TCFD_TCD
      );

    ///
    /// set PCR[PSF2] TRH PCIEN[8] to 1
    ///
    PchPcrAndThenOr32 (
      PID_PSF2, R_PCH_PCR_PSF2_T0_SHDW_TRH_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
      (UINT32)~0,
      B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
      );

    goto PchThermalInitEnd;
  }

  ///
  /// Initializing Thermal Sensors
  /// Step 1
  /// - Set various trip points based on the particular usage model.  Note that Cat Trip must always be programmed.
  /// - CTT must be programmed for Cat Trip, CTT must never be changed while the TS enable is set.
  ///   This rule prevents a spurious trip from occurring and causing a system shutdown.
  ///   TSC must then be written to 0x81 to enable the power down and lock the register.
  ///   TSC is programmed later.
  /// - TAHV and TAHL may be programmed if the BIOS or driver wish to force a SW notification for PCH temperature
  ///    - If TAHL/TAHV programmed much later in the flow when a driver is loaded, this means that the TS had been
  ///      enabled long before this, the thermal sensor must be disabled when TAHL/TAHV are programmed, and then
  ///      re-enabled.
  ///    - TSPIEN or TSGPEN may be programmed to cause either an interrupt or SMI/SCI.
  ///    - It is recommended that TAHV, TALV, TSPIEN and TSGPEN be left at their default value, unless there is a
  ///      specific usage that requires these to be programmed.
  ///
  MmioWrite16 (ThermalBase + R_PCH_TBAR_CTT, 0x014A);

  ///
  /// Step 2
  /// Clear trip status from TSS/TAS. BIOS should write 0xFF to clear any bit that was inadvertently set while programming
  /// the TS. This write of 0xFF should be done before continuing to the next steps.
  ///
  MmioWrite8 (ThermalBase + R_PCH_TBAR_TSS, 0xFF);
  MmioWrite8 (ThermalBase + R_PCH_TBAR_TAS, 0xFF);

  ///
  /// Step 3
  /// Enable the desired thermal trip alert methods, i.e. GPE (TSGPEN), SMI (TSMIC) or Interrupt (TSPIEN).
  /// Only one of the methods should be enabled and the method will be depending on the platform implementation.
  /// - TSGPEN: BIOS should leave this as default 00h, unless it is required to enable GPE.
  /// - TSMIC: BIOS should leave TSMIC[7:0] as default 00h, unless the SMI handler is loaded
  ///   and it's safe to enable SMI for these events.
  /// - TSPIEN: BIOS should leave this as default 0x00, so that a driver can enable later
  ///
  //Leave the default settings.
  //MmioWrite8 (ThermalBase + R_PCH_TBAR_TSGPEN, 0x00);
  //MmioWrite8 (ThermalBase + R_PCH_TBAR_TSMIC, 0x00);
  //MmioWrite8 (ThermalBase + R_PCH_TBAR_TSPIEN, 0x00);

  ///
  /// Step 4
  /// If thermal reporting to an EC over SMBus is supported, then write 0x01 to TSREL, else leave at default.
  ///
  MmioWrite8 (ThermalBase + R_PCH_TBAR_TSREL, 0x01);

  ///
  /// Step 5
  /// If the PCH_Hot pin reporting is supported, then write the temperature value and set the enable in PHL.
  /// Note: For PCHHOT# support, we need to make sure if PCHHOT# pin is set to native mode.
  /// And the value in PHL register is valid only if it is between 00h and 1FFh.
  ///
  if ((GpioIsPchHotbPinInNativeMode ()) &&
      (PchPolicyPpi->ThermalConfig.PchHotLevel < 0x0200))
  {
    ///
    /// Program PHL register according to PchHotLevel setting.
    ///
    Data16 = (PchPolicyPpi->ThermalConfig.PchHotLevel | B_PCH_TBAR_PHLE);
    MmioWrite16 (ThermalBase + R_PCH_TBAR_PHL, Data16);
  }

  ///
  /// Step 6
  /// If thermal throttling is supported, then set the desired values in TL.
  ///
  if (PchPolicyPpi->ThermalConfig.ThermalThrottling.TTLevels.SuggestedSetting == FALSE) {
    ///
    /// Trip Point Temperature = (Trip Point Register [8:0]) / 2 - 50 centigrade degree
    /// If Trip Point Temperature <= T0Level, the system will be in T0 state.
    /// If T1Level >= Trip Point Temperature > T0Level, the system will be in T1 state.
    /// If T2Level >= Trip Point Temperature > T1Level, the system will be in T2 state.
    /// If Trip Point Temperature > T2Level, the system will be in T3 state.
    ///
    PchTTLevels = (UINT32) (((PchPolicyPpi->ThermalConfig.ThermalThrottling.TTLevels.T2Level + 50) * 2) << 20) |
                  (UINT32) (((PchPolicyPpi->ThermalConfig.ThermalThrottling.TTLevels.T1Level + 50) * 2) << 10) |
                  (UINT32)  ((PchPolicyPpi->ThermalConfig.ThermalThrottling.TTLevels.T0Level + 50) * 2);
    Data32Or    = (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.TTLevels.TTLock << 31) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.TTLevels.TTState13Enable << 30) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.TTLevels.TTEnable << 29);
  } else {
    PchTTLevels = 0;
    Data32Or    = 0;
    //
    // Check if PCH LP, it also indicates the ULT.
    //
    if ((PchSeries == PchLp) &&
        (PchPolicyPpi->ThermalConfig.ThermalThrottling.TTLevels.PchCrossThrottling == TRUE))
    {
      ///
      /// If processor is capable of cross throttling and it is enabled
      ///   Set TBAR + 40h[28:0] trip point temperature indicated by MSR 12Ah
      ///     If (RATL mode) // refer to SKL CPU BWG
      ///       T0L = ((MSR 1A2h[23:16] + 50) * 2)
      ///     Else
      ///       T0L = (((MSR 1A2h[23:16] - MSR 1A2h[29:24]) + 50) * 2)
      ///     T1L = T0L + 5C
      ///     T2L = T1L + 5C
      ///   Set TBAR + 40h[31:29] = 101b
      ///   Set TBAR + 50h[14] = 1b
      ///   Set PWRMBASE + C4h[26:24] = 101b
      ///
      TempMsr.Qword = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
      //
      // Check RATL mode by checking MSR 1A2h[6:0] != 0
      //
      if ((TempMsr.Bytes.FirstByte & B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_TIME_WINDOW) != 0) {
        temperature = TempMsr.Bytes.ThirdByte;
      } else {
        temperature = TempMsr.Bytes.ThirdByte - (TempMsr.Bytes.FouthByte & 0x3F);
      }
      PchTTLevels = (UINT32)(((temperature + 10 + 50) * 2) << 20) |
                    (UINT32)(((temperature +  5 + 50) * 2) << 10) |
                    (UINT32) ((temperature      + 50) * 2);
      ///
      /// Program TBAR + 40h[28:0]
      ///
      MmioWrite32 (ThermalBase + R_PCH_TBAR_TL, PchTTLevels);
      ///
      /// Program TBAR + 40h[31:29]
      /// TL [31] locks the thermal throttling registers
      ///
      MmioOr32 (ThermalBase + R_PCH_TBAR_TL, B_PCH_TBAR_TL_LOCK | B_PCH_TBAR_TL_TTEN);
      ///
      /// Set TBAR + 50h[14] = 1b
      ///
      MmioOr16 (ThermalBase + R_PCH_TBAR_TL2, R_PCH_TBAR_TL2_PMCTEN);
      ///
      /// Set PWRMBASE + C4h[26:24] = 101b
      ///
      MmioAndThenOr32 (PchPwrmBase + R_PCH_PWRM_PMSYNC_TPR_CONFIG,
        (UINT32)~(B_PCH_PWRM_PMSYNC_PCH2CPU_TT_STATE),
        (B_PCH_PWRM_PMSYNC_PCH2CPU_TT_EN | (V_PCH_PWRM_PMSYNC_PCH2CPU_TT_STATE_1 << N_PCH_PWRM_PMSYNC_PCH2CPU_TT_STATE))
        );
    } else {
      ///
      /// Set TBAR + 40h[28:20] = 148h
      /// Set TBAR + 40h[18:10] = 142h
      /// Set TBAR + 40h[8:0] = 13Ch
      /// Set TBAR + 40h[31:29] = 101b in separate write
      ///
      PchTTLevels = ((0x148 << 20) | (0x142 << 10) | (0x13C));
      MmioWrite32 (ThermalBase + R_PCH_TBAR_TL, PchTTLevels);
      MmioOr32 (ThermalBase + R_PCH_TBAR_TL, B_PCH_TBAR_TL_TTEN);
      MmioOr32 (ThermalBase + R_PCH_TBAR_TL, B_PCH_TBAR_TL_LOCK);
    }
  }

  ///
  /// Step 7
  /// Enable thermal sensor power management
  ///   Write C8h to TSPM[7:0]
  ///   Write 101b to TSPM[11:9]
  ///   Write 100b to TSPM[14:12]
  ///   Lock the register by writing '1' to TSPM[15], do it later
  ///
  MmioWrite16 (ThermalBase + R_PCH_TBAR_TSPM, 0x4AC8);


  ///
  /// Step 8
  /// PCH LP:
  ///   Program ThermalBar + 0xC0[0] = 1b
  ///   Program ThermalBar + 0xC4[12:8, 7:6] = 00111b, 11b
  ///   Program ThermalBar + 0xD0[25:21] = 00101b
  ///   Program ThermalBar + 0xE4[28:27, 26:24, 11:8, 4:1] = 11b, 001b, 1000b, 1000b
  ///   Program ThermalBar + 0xE8[15:8] = 80h
  ///   if ThermalBar + 0xCC[15:0] is 0x0000, then program ThermalBar + 0xE0 to 0x800002BE
  /// PCH H:
  ///   Program ThermalBAR + 0xC0 [0] = 1b
  ///   Program ThermalBAR + 0xC4[12:8, 7:6] = 00111b, 11b
  ///   Program ThermalBAR + 0xC8[31:16] = 0010h
  ///   Program ThermalBAR + 0xD0[25:21] = 01000b
  ///   Program ThermalBAR + 0xE4[28:27, 26:24, 11:8, 4:1] = 11b, 001b, 1000b, 1000b
  ///   Program ThermalBAR + 0xE8[15:8] = 80h
  ///   if ThermalBar + 0xCC[15:0] is 0x0000, then program ThermalBar + 0xE0 to 0x800002BE
  ///
  MmioOr8 (ThermalBase + R_PCH_TBAR_C0, BIT0);
  MmioAndThenOr32 (
    ThermalBase + R_PCH_TBAR_C4,
    (UINT32) ~(BIT12 | BIT11),
    BIT10 | BIT9 | BIT8 | BIT7 | BIT6
    );
  if (PchSeries == PchH) {
    MmioAndThenOr32 (
      ThermalBase + R_PCH_TBAR_C8,
      (UINT32) ~(0xFFFF0000),
      0x00100000
      );
  }
  if (PchSeries == PchLp) {
    MmioAndThenOr32 (
      ThermalBase + R_PCH_TBAR_D0,
      (UINT32) ~(BIT25 | BIT24 | BIT22),
      BIT23 | BIT21
      );
  } else {
    MmioAndThenOr32 (
      ThermalBase + R_PCH_TBAR_D0,
      (UINT32) ~(BIT25 | BIT23 | BIT22 | BIT21),
      BIT24
      );
  }
  MmioAndThenOr32 (
    ThermalBase + R_PCH_TBAR_E4,
    (UINT32) ~(BIT26 | BIT25 | BIT10 | BIT9 | BIT8 | BIT3 | BIT2 | BIT1),
    BIT28 | BIT27 | BIT24 | BIT11 | BIT4
    );
  MmioAndThenOr32 (
    ThermalBase + R_PCH_TBAR_E8,
    (UINT32) ~(0x0000FF00),
    0x00008000
    );

  if (MmioRead16 (ThermalBase + R_PCH_TBAR_CC) == 0) {
    MmioWrite32 (ThermalBase + R_PCH_TBAR_E0, 0x800002BE);
  }

  ///
  /// Step 9
  /// Program ThermalBar + 0xA4[2,1,0] = 1b,1b,1b
  ///
  MmioOr16 (ThermalBase + R_PCH_TBAR_A4, (BIT2 | BIT1 | BIT0));

  ///
  /// Step 10
  ///   Enable thermal sensor by programming TSEL register to 0x01.
  ///   This should be done after all thermal initialization steps are finished.
  ///
  MmioOr8 (ThermalBase + R_PCH_TBAR_TSEL, B_PCH_TBAR_TSEL_ETS);

  //
  // Per interface throttling actions
  //

  if (PchSeries == PchH) {
    ///
    /// DMI/OPI Throttling action
    /// Set PCR[DMI] + 2238h[11:10] = 10
    /// Set PCR[DMI] + 2238h[9:8] = 01
    /// Set PCR[DMI] + 2238h[7:6] = 00
    /// Set PCR[DMI] + 2238h[5:4] = 00
    /// Set PCR[DMI] + 2238h[0] = 1
    ///
    if (PchPolicyPpi->ThermalConfig.ThermalThrottling.DmiHaAWC.SuggestedSetting == TRUE) {
      Data32And = (UINT32)~(0x000006F0);
      Data32Or  = BIT11 | BIT8 | BIT0;
    } else {
      Data32And = (UINT32)~(0x00000FF1);
      Data32Or  = (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.DmiHaAWC.TS3TW << 10) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.DmiHaAWC.TS2TW << 8) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.DmiHaAWC.TS1TW << 6) |
                  (UINT32) (PchPolicyPpi->ThermalConfig.ThermalThrottling.DmiHaAWC.TS0TW << 4) |
                  (UINT32)  PchPolicyPpi->ThermalConfig.ThermalThrottling.DmiHaAWC.DmiTsawEn;
    }
    PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_DMIHWAWC, Data32And, Data32Or);
  }

  //
  // Sata throttling actions
  //
  SataThrottlingConfiguration (PchPolicyPpi);

  //
  // Security Lock
  //

  ///
  /// It tis recommended that TSC[7] set to '1' to lock the CAT Trip behavior
  /// TSC must then be written to 0x81 to enable the power down and lock the register.
  ///
  MmioOr8 (ThermalBase + R_PCH_TBAR_TSC, B_PCH_TBAR_TSC_PLD | B_PCH_TBAR_TSC_CPDE);

  ///
  /// TSMIC [7] locks SMI reporting of thermal events
  ///
  if (PchPolicyPpi->ThermalConfig.TsmicLock == TRUE) {
    MmioOr8 (ThermalBase + R_PCH_TBAR_TSMIC, B_PCH_TBAR_TSMIC_PLD);
  }

  ///
  /// PHLC[0] locks the PHL and PHLC registers for PCH_HOT#
  ///
  MmioOr8 (ThermalBase + R_PCH_TBAR_PHLC, B_PCH_TBAR_PHLC_LOCK);
  ///
  /// TL[31] locks the thermal throttling registers
  /// Done above.
  ///

  ///
  /// TL2[15] locks the thermal throttling 2 register
  ///
  MmioOr16 (ThermalBase + R_PCH_TBAR_TL2, R_PCH_TBAR_TL2_LOCK);

  ///
  /// TSEL [7] locks the thermal sensor enable, after TAHV and TAHL are
  /// programmed by BIOS or driver later in case
  ///
  MmioOr8 (ThermalBase + R_PCH_TBAR_TSEL, B_PCH_TBAR_TSEL_PLD);

  ///
  /// Lock TSPM the register by writing '1' to TSPM[15]
  ///
  MmioOr16 (ThermalBase + R_PCH_TBAR_TSPM, B_PCH_TBAR_TSPM_TSPMLOCK);

PchThermalInitEnd:

  if (PchPolicyPpi->ThermalConfig.ThermalDeviceEnable == 2) {
    ///
    /// Hide device when in ACPI mode
    /// Disable PciCfgSpace in PCR[PSF2]
    ///
    PchPcrAndThenOr32 (
      PID_PSF2, R_PCH_PCR_PSF2_T0_SHDW_TRH_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS,
      (UINT32)~0,
      B_PCH_PCR_PSFX_T0_SHDW_CFG_DIS_CFGDIS
      );
  } else {
  ///
    /// Restore BAR and command register and disable memory decode
  ///
  MmioAnd8 (ThermalPciBase + PCI_COMMAND_OFFSET, (UINT8)~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER));
    //
    // Restore original COMMAND and TBAR register
    // The TBAR might be assigned during S3 resume path.
    //
    MmioWrite32 (ThermalPciBase + R_PCH_THERMAL_TBAR, OrgTbarLow);
    MmioWrite32 (ThermalPciBase + R_PCH_THERMAL_TBARH, OrgTbarHigh);
    MmioWrite8 (ThermalPciBase + PCI_COMMAND_OFFSET, OrgCmd);
  }

  DEBUG ((DEBUG_INFO, "PchThermalInit () End\n"));

  return EFI_SUCCESS;
}

/**
  Init Memory Throttling.

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
**/
VOID
PchMemoryThrottlingInit (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  UINT32                                PwrmBase;
  UINT32                                Data32;
  PCH_MEMORY_THROTTLING                 *MemoryThrottling;

  MemoryThrottling = &(PchPolicyPpi->ThermalConfig.MemoryThrottling);

  if (MemoryThrottling->Enable == 0) {
    return;
  }

  PchPwrmBaseGet (&PwrmBase);
  //
  // Select 0: CPU_GP_0 (default) or 1: CPU_GP_1 for TsGpioC
  // Select 0: CPU_GP_3 (default) or 1: CPU_GP_2 for TsGpioD
  // Set PWRMBASE offset 0xC8 [11:10] accordingly.
  //
  Data32 = 0;
  if (MemoryThrottling->TsGpioPinSetting[TsGpioC].PinSelection) {
    Data32 |= B_PCH_PWRM_PMSYNC_GPIO_C_SEL;
  }
  if (MemoryThrottling->TsGpioPinSetting[TsGpioD].PinSelection) {
    Data32 |= B_PCH_PWRM_PMSYNC_GPIO_D_SEL;
  }
  MmioAndThenOr32 (
    PwrmBase + R_PCH_PWRM_PMSYNC_MISC_CFG,
    (UINT32) ~(B_PCH_PWRM_PMSYNC_GPIO_C_SEL | B_PCH_PWRM_PMSYNC_GPIO_D_SEL),
    Data32
    );

  //
  // Enable Enable GPIO_C/GPIO_D PMSYNC Pin Mode in C0
  // Set PWRMBASE offset 0xF4 [15:14] accordingly.
  //
  Data32 = 0;
  if (MemoryThrottling->TsGpioPinSetting[TsGpioC].C0TransmitEnable) {
    Data32 |= BIT14;
  }
  if (MemoryThrottling->TsGpioPinSetting[TsGpioD].C0TransmitEnable) {
    Data32 |= BIT15;
  }
  MmioAndThenOr32 (
    PwrmBase + R_PCH_PWRM_PM_SYNC_MODE_C0,
    (UINT32) ~(BIT15 | BIT14),
    Data32
    );

  //
  // Enable GPIO_C/GPIO_D PMSYNC pin mode.
  // Set PWRMBASE offset 0xD4 [15:14] accordingly.
  // Override to GPIO native mode.
  //
  Data32 = 0;
  if (MemoryThrottling->TsGpioPinSetting[TsGpioC].PmsyncEnable) {
    Data32 |= BIT14;
    if (MemoryThrottling->TsGpioPinSetting[TsGpioC].PinSelection == 0) {
      GpioSetCpuGpPinsIntoNativeMode (0);
    } else {
      GpioSetCpuGpPinsIntoNativeMode (1);
    }
  }
  if (MemoryThrottling->TsGpioPinSetting[TsGpioD].PmsyncEnable) {
    Data32 |= BIT15;
    if (MemoryThrottling->TsGpioPinSetting[TsGpioD].PinSelection == 0) {
      GpioSetCpuGpPinsIntoNativeMode (3);
    } else {
      GpioSetCpuGpPinsIntoNativeMode (2);
    }
  }
  MmioAndThenOr32 (
    PwrmBase + R_PCH_PWRM_PM_SYNC_MODE,
    (UINT32) ~(BIT15 | BIT14),
    Data32
    );
}
