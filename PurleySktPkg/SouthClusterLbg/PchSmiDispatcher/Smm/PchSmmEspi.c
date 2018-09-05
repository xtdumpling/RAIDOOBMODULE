/** @file
  eSPI SMI implementation

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "PchSmmEspi.h"
#include <PiPei.h>
#include <Library/PchEspiLib.h>
#include <Library/PchInfoLib.h>
#include <Library/TimerLib.h>
#include <Ppi/PchReset.h>

GLOBAL_REMOVE_IF_UNREFERENCED ESPI_SMI_INSTANCE mEspiSmiInstance = {
  //
  // Signature
  //
  ESPI_SMI_INSTANCE_SIGNATURE,
  //
  // Handle
  //
  NULL,
  //
  // PchEspiSmiDispatchProtocol
  //
  {
    PCH_ESPI_SMI_DISPATCH_REVISION,
    EspiSmiUnRegister,
    BiosWrProtectRegister,
    BiosWrReportRegister,
    PcNonFatalErrRegister,
    PcFatalErrRegister,
    VwNonFatalErrRegister,
    VwFatalErrRegister,
    FlashNonFatalErrRegister,
    FlashFatalErrRegister,
    LnkType1ErrRegister,
    EspiSlaveSmiRegister
  },
  //
  // PchSmiEspiHandle[EspiTopLevelTypeMax]
  //
  {
    NULL, NULL, NULL
  },
  //
  // CallbackDataBase[EspiSmiTypeMax]
  //
  {
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}
  },
  //
  // EspiSmiEventCounter[EspiSmiTypeMax]
  //
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
  //
  // Barrier[EspiTopLevelTypeMax]
  //
  {
    {
      BiosWrProtect,
      BiosWrProtect
    },
    {
      BiosWrReport,
      LnkType1Err
    },
    {
      EspiSlaveSmi,
      EspiSlaveSmi
    }
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED CONST ESPI_DESCRIPTOR mEspiDescriptor[EspiSmiTypeMax] = {
  //
  // BiosWrProtect
  //
  {
    {
      PCIE_ADDR_TYPE,
      {(
        (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
        (PCI_DEVICE_NUMBER_PCH_LPC << 19) |
        (PCI_FUNCTION_NUMBER_PCH_LPC << 16) |
        R_PCH_LPC_BC
      )}
    },
    //
    // SourceIsActiveAndMask and SourceIsActiveValue
    //
    B_PCH_ESPI_PCBC_BWPDS | B_PCH_ESPI_PCBC_LE,
    B_PCH_ESPI_PCBC_BWPDS | B_PCH_ESPI_PCBC_LE,
    //
    // ClearStatusAndMask and ClearStatusOrMask
    //
    (UINT32) ~B_PCH_ESPI_PCBC_BWRS,
    B_PCH_ESPI_PCBC_BWPDS
  },
  //
  // BiosWrReport
  //
  {
    {
      PCIE_ADDR_TYPE,
      {(
        (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
        (PCI_DEVICE_NUMBER_PCH_LPC << 19) |
        (PCI_FUNCTION_NUMBER_PCH_LPC << 16) |
        R_PCH_LPC_BC
      )}
    },
    B_PCH_ESPI_PCBC_BWRS | B_PCH_ESPI_PCBC_BWRE,
    B_PCH_ESPI_PCBC_BWRS | B_PCH_ESPI_PCBC_BWRE,
    (UINT32) ~B_PCH_ESPI_PCBC_BWPDS,
    B_PCH_ESPI_PCBC_BWRS
  },
  //
  // PcNonFatalErr
  //
  {
    {
      PCR_ADDR_TYPE,
      {PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_ESPI_PCERR_SLV0)}
    },
    (B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XNFEE),
    (B_PCH_PCR_ESPI_XERR_XNFES | (V_PCH_PCR_ESPI_XERR_XNFEE_SMI << N_PCH_PCR_ESPI_XERR_XNFEE)),
    (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XFES),
    B_PCH_PCR_ESPI_XERR_XNFES
  },
  //
  // PcFatalErr
  //
  {
    {
      PCR_ADDR_TYPE,
      {PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_ESPI_PCERR_SLV0)}
    },
    (B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XFEE),
    (B_PCH_PCR_ESPI_XERR_XFES | (V_PCH_PCR_ESPI_XERR_XFEE_SMI << N_PCH_PCR_ESPI_XERR_XFEE)),
    (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES),
    B_PCH_PCR_ESPI_XERR_XFES
  },
  //
  // VwNonFatalErr
  //
  {
    {
      PCR_ADDR_TYPE,
      {PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_ESPI_VWERR_SLV0)}
    },
    (B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XNFEE),
    (B_PCH_PCR_ESPI_XERR_XNFES | (V_PCH_PCR_ESPI_XERR_XNFEE_SMI << N_PCH_PCR_ESPI_XERR_XNFEE)),
    (UINT32) ~B_PCH_PCR_ESPI_XERR_XFES,
    B_PCH_PCR_ESPI_XERR_XNFES
  },
  //
  // VwFatalErr
  //
  {
    {
      PCR_ADDR_TYPE,
      {PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_ESPI_VWERR_SLV0)}
    },
    (B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XFEE),
    (B_PCH_PCR_ESPI_XERR_XFES | (V_PCH_PCR_ESPI_XERR_XFEE_SMI << N_PCH_PCR_ESPI_XERR_XFEE)),
    (UINT32) ~B_PCH_PCR_ESPI_XERR_XNFES,
    B_PCH_PCR_ESPI_XERR_XFES
  },
  //
  // FlashNonFatalErr
  //
  {
    {
      PCR_ADDR_TYPE,
      {PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_ESPI_FCERR_SLV0)}
    },
    (B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XNFEE),
    (B_PCH_PCR_ESPI_XERR_XNFES | (V_PCH_PCR_ESPI_XERR_XNFEE_SMI << N_PCH_PCR_ESPI_XERR_XNFEE)),
    (UINT32) ~B_PCH_PCR_ESPI_XERR_XFES,
    B_PCH_PCR_ESPI_XERR_XNFES
  },
  //
  // FlashFatalErr
  //
  {
    {
      PCR_ADDR_TYPE,
      {PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_ESPI_FCERR_SLV0)}
    },
    (B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XFEE),
    (B_PCH_PCR_ESPI_XERR_XFES | (V_PCH_PCR_ESPI_XERR_XFEE_SMI << N_PCH_PCR_ESPI_XERR_XFEE)),
    (UINT32) ~B_PCH_PCR_ESPI_XERR_XNFES,
    B_PCH_PCR_ESPI_XERR_XFES
  },
  //
  // LnkType1Err
  //
  {
    {
      PCR_ADDR_TYPE,
      {PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_ESPI_LNKERR_SLV0)}
    },
    B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1S | B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1E,
    B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1S | (V_PCH_PCR_ESPI_LNKERR_SLV0_LFET1E_SMI << N_PCH_PCR_ESPI_LNKERR_SLV0_LFET1E),
    (UINT32) ~B_PCH_PCR_ESPI_LNKERR_SLV0_SLCRR,
    B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1S
  },
};

/**
  Enable eSPI SMI source

  @param[in]  EspiSmiType  Type based on ESPI_SMI_TYPE
**/
STATIC
VOID
EspiSmiEnableSource (
  IN CONST  ESPI_SMI_TYPE EspiSmiType
  )
{
  EFI_STATUS  Status;
  UINTN       PciBaseAddress;
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries ();

  switch (EspiSmiType) {
  case BiosWrProtect:
    PciBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_LPC,
                       PCI_FUNCTION_NUMBER_PCH_LPC
                       );
    MmioAndThenOr32 (
      PciBaseAddress + R_PCH_ESPI_PCBC,
      (UINT32) ~(B_PCH_ESPI_PCBC_BWRS | B_PCH_ESPI_PCBC_BWPDS),
      B_PCH_ESPI_PCBC_LE
      );
    break;

  case BiosWrReport:
    PciBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_LPC,
                       PCI_FUNCTION_NUMBER_PCH_LPC
                       );
    MmioAndThenOr32 (
      PciBaseAddress + R_PCH_ESPI_PCBC,
      (UINT32) ~(B_PCH_ESPI_PCBC_BWRS | B_PCH_ESPI_PCBC_BWPDS),
      B_PCH_ESPI_PCBC_BWRE
      );
    break;

  case PcNonFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_PCERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XNFEE);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_PCERR_SLV1,
               (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XNFEE);
      ASSERT_EFI_ERROR (Status);
    }
    break;

  case PcFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_PCERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XFEE);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_PCERR_SLV1,
               (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XFEE);
      ASSERT_EFI_ERROR (Status);
    }
    break;

  case VwNonFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_VWERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XNFEE);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_VWERR_SLV1,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XNFEE);
      ASSERT_EFI_ERROR (Status);
    }
    break;

  case VwFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_VWERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XFEE);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_VWERR_SLV1,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XFEE);
      ASSERT_EFI_ERROR (Status);
    }
    break;

  case FlashNonFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_FCERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XNFEE);
    ASSERT_EFI_ERROR (Status);
    break;

  case FlashFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_FCERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES),
               B_PCH_PCR_ESPI_XERR_XFEE);
    ASSERT_EFI_ERROR (Status);
    break;

  case LnkType1Err:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_LNKERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_LNKERR_SLV0_SLCRR | B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1S),
               B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1E);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_LNKERR_SLV1,
               (UINT32) ~(B_PCH_PCR_ESPI_LNKERR_SLV0_SLCRR | B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1S),
               B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1E);
      ASSERT_EFI_ERROR (Status);
    }
    break;
  default:
    DEBUG ((DEBUG_ERROR, "Unsupported EspiSmiType \n"));
    ASSERT (FALSE);
    break;
  }
}


/**
  Disable eSPI SMI source

  @param[in]  EspiSmiType  Type based on ESPI_SMI_TYPE
**/
STATIC
VOID
EspiSmiDisableSource (
  IN CONST  ESPI_SMI_TYPE EspiSmiType
  )
{
  EFI_STATUS Status;
  PCH_SERIES PchSeries;

  PchSeries = GetPchSeries ();

  switch (EspiSmiType) {
  case BiosWrProtect:
  case BiosWrReport:
    DEBUG ((DEBUG_ERROR, "Bit is write lock, thus BWRE/BWPDS source cannot be disabled \n"));
    ASSERT (FALSE);
    break;

  case PcNonFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_PCERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XNFEE),
               0);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
        Status = PchPcrAndThenOr32 (
                 PID_ESPISPI,
                 (UINT16) R_PCH_PCR_ESPI_PCERR_SLV1,
                 (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XNFEE),
                 0);
        ASSERT_EFI_ERROR (Status);
      }

    break;

  case PcFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_PCERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XFEE),
               0);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_PCERR_SLV1,
               (UINT32) ~(B_PCH_PCR_ESPI_PCERR_SLV0_PCURD | B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XFEE),
               0);
      ASSERT_EFI_ERROR (Status);
    }
    break;

  case VwNonFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_VWERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XNFEE),
               0);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_VWERR_SLV1,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XNFEE),
               0);
      ASSERT_EFI_ERROR (Status);
    }
    break;

  case VwFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_VWERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XFEE),
               0);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
                 PID_ESPISPI,
                 (UINT16) R_PCH_PCR_ESPI_VWERR_SLV1,
                 (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XFEE),
                 0);
      ASSERT_EFI_ERROR (Status);
    }
    break;

  case FlashNonFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_FCERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XNFEE),
               0);
    ASSERT_EFI_ERROR (Status);
    break;

  case FlashFatalErr:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_FCERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_XERR_XNFES | B_PCH_PCR_ESPI_XERR_XFES | B_PCH_PCR_ESPI_XERR_XFEE),
               0);
    ASSERT_EFI_ERROR (Status);
    break;

  case LnkType1Err:
    Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_LNKERR_SLV0,
               (UINT32) ~(B_PCH_PCR_ESPI_LNKERR_SLV0_SLCRR | B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1S),
               0);
    ASSERT_EFI_ERROR (Status);
    if (PchSeries == PchH) {
      Status = PchPcrAndThenOr32 (
               PID_ESPISPI,
               (UINT16) R_PCH_PCR_ESPI_LNKERR_SLV1,
               (UINT32) ~(B_PCH_PCR_ESPI_LNKERR_SLV0_SLCRR | B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1S),
               0);
      ASSERT_EFI_ERROR (Status);
    }
    break;

  default:
    DEBUG ((DEBUG_ERROR, "Unsupported EspiSmiType \n"));
    ASSERT (FALSE);
    break;
  }
}

/**
  Clear a status for the SMI event

  @param[in]  EspiSmiType  Type based on ESPI_SMI_TYPE
**/
STATIC
VOID
EspiSmiClearStatus (
  IN CONST  ESPI_SMI_TYPE EspiSmiType
  )
{
  EFI_STATUS              Status;
  UINT32                  PciBus;
  UINT32                  PciDev;
  UINT32                  PciFun;
  UINT32                  PciReg;
  UINT32                  PciBaseAddress;
  CONST ESPI_DESCRIPTOR   *Desc;

  Desc = &mEspiDescriptor[EspiSmiType];

  switch (Desc->Address.Type) {
  case PCIE_ADDR_TYPE:
    PciBus  = Desc->Address.Data.pcie.Fields.Bus;
    PciDev  = Desc->Address.Data.pcie.Fields.Dev;
    PciFun  = Desc->Address.Data.pcie.Fields.Fnc;
    PciReg  = Desc->Address.Data.pcie.Fields.Reg;
    PciBaseAddress = (UINT32) MmPciBase (PciBus, PciDev, PciFun);
    MmioAndThenOr32 (PciBaseAddress + PciReg, Desc->ClearStatusAndMask, Desc->ClearStatusOrMask);
    break;
  case PCR_ADDR_TYPE:
    Status = PchPcrAndThenOr32 (
               Desc->Address.Data.Pcr.Fields.Pid,
               Desc->Address.Data.Pcr.Fields.Offset,
               Desc->ClearStatusAndMask,
               Desc->ClearStatusOrMask
               );
    ASSERT_EFI_ERROR (Status);
    break;
  default:
    DEBUG ((DEBUG_ERROR, "Address type for eSPI SMI is invalid \n"));
    ASSERT (FALSE);
    break;
  }
}

/**
  Checks if a source is active by looking at the enable and status bits

  @param[in]  EspiSmiType  Type based on ESPI_SMI_TYPE
**/
STATIC
BOOLEAN
EspiSmiSourceIsActive (
  IN CONST  ESPI_SMI_TYPE EspiSmiType
  )
{
  EFI_STATUS              Status;
  BOOLEAN                 Active;
  UINT32                  PciBus;
  UINT32                  PciDev;
  UINT32                  PciFun;
  UINT32                  PciReg;
  UINT32                  PciBaseAddress;
  UINT32                  Data32;
  CONST ESPI_DESCRIPTOR   *Desc;

  Desc = &mEspiDescriptor[EspiSmiType];

  Active = FALSE;
  switch (Desc->Address.Type) {
  case PCIE_ADDR_TYPE:
    PciBus  = Desc->Address.Data.pcie.Fields.Bus;
    PciDev  = Desc->Address.Data.pcie.Fields.Dev;
    PciFun  = Desc->Address.Data.pcie.Fields.Fnc;
    PciReg  = Desc->Address.Data.pcie.Fields.Reg;
    PciBaseAddress = (UINT32) MmPciBase (PciBus, PciDev, PciFun);
    Data32 = MmioRead32 (PciBaseAddress + PciReg);
    break;

  case PCR_ADDR_TYPE:
    Status = PchPcrRead32 (
               Desc->Address.Data.Pcr.Fields.Pid,
               Desc->Address.Data.Pcr.Fields.Offset,
               &Data32
               );
    ASSERT_EFI_ERROR (Status);
    break;

  default:
    Data32 = 0;
    DEBUG ((DEBUG_ERROR, "Address type for eSPI SMI is invalid \n"));
    ASSERT (FALSE);
    break;
  }

  if ((Data32 & Desc->SourceIsActiveAndMask) == Desc->SourceIsActiveValue) {
    Active = TRUE;
  }

  return Active;
}

/**
  Insert a handler into the corresponding linked list based on EspiSmiType

  @param[in]  DispatchFunction      The callback to execute
  @param[in]  EspiSmiType           Type based on ESPI_SMI_TYPE to determine which linked list to use
  @param[out] DispatchHandle        The link to the record in the database

  @retval     EFI_SUCCESS           Record was successfully inserted into master database
  @retval     EFI_OUT_OF_RESOURCES  Cannot allocate pool to insert record
**/
STATIC
EFI_STATUS
InsertEspiRecord (
  IN        PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  IN        ESPI_SMI_TYPE                   EspiSmiType,
  OUT       EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS      Status;
  ESPI_SMI_RECORD *Record;

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (ESPI_SMI_RECORD), (VOID **) &Record);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  SetMem (Record, sizeof (ESPI_SMI_RECORD), 0);

  Record->Callback  = DispatchFunction;
  Record->Signature = ESPI_SMI_RECORD_SIGNATURE;

  InsertTailList (&mEspiSmiInstance.CallbackDataBase[EspiSmiType], &Record->Link);
  EspiSmiClearStatus (EspiSmiType);
  EspiSmiEnableSource (EspiSmiType);

  ++mEspiSmiInstance.EspiSmiEventCounter[EspiSmiType];

  *DispatchHandle = (EFI_HANDLE) (&Record->Link);

  return EFI_SUCCESS;
}

/**
  This callback is registered to PchSmiDispatch

  @param[in]  DispatchHandle  Used to determine which source have been triggered
**/
VOID
EspiSmiCallback (
  IN  EFI_HANDLE                      DispatchHandle
  )
{
  DATABASE_RECORD     *PchSmiRecord;
  ESPI_TOP_LEVEL_TYPE EspiTopLevelType;
  ESPI_SMI_TYPE       EspiSmiType;
  ESPI_SMI_RECORD     *RecordInDb;
  LIST_ENTRY          *LinkInDb;

  PchSmiRecord = DATABASE_RECORD_FROM_LINK (DispatchHandle);

  if (PchSmiRecord->PchSmiType == PchTcoSmiLpcBiosWpType) {
    EspiTopLevelType = EspiBiosWrProtect;
  } else if (PchSmiRecord->PchSmiType == PchSmiSerialIrqType) {
    EspiTopLevelType = EspiSerialIrq;
  } else {
    DEBUG ((DEBUG_ERROR, "EspiSmiCallback was dispatched with a wrong DispatchHandle"));
    ASSERT (FALSE);
    return;
  }

  for (EspiSmiType = mEspiSmiInstance.Barrier[EspiTopLevelType].Start; EspiSmiType <= mEspiSmiInstance.Barrier[EspiTopLevelType].End; ++EspiSmiType) {
    if (!EspiSmiSourceIsActive (EspiSmiType)) {
      continue;
    }
    //
    // The source is active, so walk the callback database and dispatch
    //
    if (!IsListEmpty (&mEspiSmiInstance.CallbackDataBase[EspiSmiType])) {
      //
      // We have children registered w/ us -- continue
      //
      LinkInDb = GetFirstNode (&mEspiSmiInstance.CallbackDataBase[EspiSmiType]);

      while (!IsNull (&mEspiSmiInstance.CallbackDataBase[EspiSmiType], LinkInDb)) {
        RecordInDb = ESPI_RECORD_FROM_LINK (LinkInDb);

        //
        // RecordInDb->Link might be removed (unregistered) by Callback function, and then the
        // system will hang in ASSERT() while calling GetNextNode().
        // To prevent the issue, we need to get next record in DB here (before Callback function).
        //
        LinkInDb = GetNextNode (&mEspiSmiInstance.CallbackDataBase[EspiSmiType], &RecordInDb->Link);

        //
        // Callback
        //
        if (RecordInDb->Callback != NULL) {
          RecordInDb->Callback ((EFI_HANDLE) &RecordInDb->Link);
        } else {
          ASSERT (FALSE);
        }
      }
    }
    //
    // Finish walking the linked list for the EspiSmiType, so clear status
    //
    EspiSmiClearStatus (EspiSmiType);
  }
}

/**
  This function will register EspiSmiCallback with mSrcDescLpcBiosWp source decriptor
  This function make sure there is only one BIOS WP SMI handler is registered.
  While any ESPI sub BIOS WP SMI type is registered, all the BIOS WP SMI
  will go to callback function EspiSmiCallback first, and then dispatchs the callbacks
  recorded in mEspiSmiInstance.

  @retval EFI_SUCCESS Registration succeed
  @retval others      Registration failed
**/
STATIC
EFI_STATUS
RegisterBiosWrProtectIfNull (
  VOID
  )
{
  EFI_STATUS      Status;
  DATABASE_RECORD *Record;

  if (mEspiSmiInstance.PchSmiEspiHandle[EspiBiosWrProtect] == NULL) {
    Status = PchSmiRecordInsert (
               &mSrcDescLpcBiosWp,
               (PCH_SMI_CALLBACK_FUNCTIONS) EspiSmiCallback,
               PchTcoSmiLpcBiosWpType,
               &mEspiSmiInstance.PchSmiEspiHandle[EspiBiosWrProtect]
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Fail to register BIOS WP SMI handler \n"));
      return Status;
    }
    Record = DATABASE_RECORD_FROM_LINK (mEspiSmiInstance.PchSmiEspiHandle[EspiBiosWrProtect]);
    Record->ClearSource = PchTcoSmiClearSource;
  }

  return EFI_SUCCESS;
}

/**
  This function will register EspiSmiCallback with mSrcDescSerialIrq source decriptor
  This function make sure there is only one Serial IRQ SMI handler is registered.
  While any ESPI sub Serial IRQ SMI type is registered, all the Serial IRQ SMI
  will go to callback function EspiSmiCallback first, and then dispatchs the callbacks
  recorded in mEspiSmiInstance.

  @retval EFI_SUCCESS Registration succeed
  @retval others      Registration failed
**/
STATIC
EFI_STATUS
RegisterSerialIrqIfNull (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mEspiSmiInstance.PchSmiEspiHandle[EspiSerialIrq] == NULL) {
    Status = PchSmiRecordInsert (
               &mSrcDescSerialIrq,
               (PCH_SMI_CALLBACK_FUNCTIONS) EspiSmiCallback,
               PchSmiSerialIrqType,
               &mEspiSmiInstance.PchSmiEspiHandle[EspiSerialIrq]
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Fail to register Serial IRQ SMI handler \n"));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Installs and initialize this protocol

  @param[in]  ImageHandle   Not used

  @retval     EFI_SUCCESS   Installation succeed
  @retval     others        Installation failed
**/
EFI_STATUS
EFIAPI
InstallEspiSmi (
  IN EFI_HANDLE           ImageHandle
  )
{
  EFI_STATUS    Status;
  ESPI_SMI_TYPE EspiType;
  EFI_HANDLE    DispatchHandle;

  //
  // InitializeListHead for mEspiSmiInstance.CallBackDataBase[EspiTopLevelTypeMax]
  //
  for (EspiType = 0; EspiType < EspiSmiTypeMax; ++EspiType) {
    InitializeListHead (&mEspiSmiInstance.CallbackDataBase[EspiType]);
  }

  //
  // Install EfiEspiSmiDispatchProtocol
  //
  Status = gSmst->SmmInstallProtocolInterface (
                  &mEspiSmiInstance.Handle,
                  &gPchEspiSmiDispatchProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mEspiSmiInstance.PchEspiSmiDispatchProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to install eSPI SMI Dispatch Protocol \n"));
    ASSERT (FALSE);
    return Status;
  }

  // Register default Fatal error handler for eSPI
  Status = LnkType1ErrRegister (
              &mEspiSmiInstance.PchEspiSmiDispatchProtocol,
              EspiRecoverFromFatalError,
              &DispatchHandle
              );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to install eSPI SMI handler! \n"));
  }

  return EFI_SUCCESS;
}

/**
  eSPI SMI Dispatch Protocol instance to register a BIOS Write Protect event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
BiosWrProtectRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterBiosWrProtectIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, BiosWrProtect, DispatchHandle);
}

/**
  eSPI SMI Dispatch Protocol instance to register a BIOS Write Report event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
BiosWrReportRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterSerialIrqIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, BiosWrReport, DispatchHandle);
}

/**
  eSPI SMI Dispatch Protocol instance to register a Peripheral Channel Non Fatal Error event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
PcNonFatalErrRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterSerialIrqIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, PcNonFatalErr, DispatchHandle);
}

/**
  eSPI SMI Dispatch Protocol instance to register a Peripheral Channel Fatal Error event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
PcFatalErrRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterSerialIrqIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, PcFatalErr, DispatchHandle);
}

/**
  eSPI SMI Dispatch Protocol instance to register a Virtual Wire Non Fatal Error event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
VwNonFatalErrRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS  Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterSerialIrqIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, VwNonFatalErr, DispatchHandle);
}

/**
  eSPI SMI Dispatch Protocol instance to register a Virtual Wire Fatal Error event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
VwFatalErrRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS  Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterSerialIrqIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, VwFatalErr, DispatchHandle);
}

/**
  eSPI SMI Dispatch Protocol instance to register a Flash Channel Non Fatal Error event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
FlashNonFatalErrRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterSerialIrqIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, FlashNonFatalErr, DispatchHandle);
}

/**
  eSPI SMI Dispatch Protocol instance to register a Flash Channel Fatal Error event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
FlashFatalErrRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterSerialIrqIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, FlashFatalErr, DispatchHandle);
}

/**
  eSPI SMI Dispatch Protocol instance to register a Link Error event

  @param[in]  This              Not used
  @param[in]  DispatchFunction  The callback to execute
  @param[out] DispatchHandle    The handle for this callback registration

  @retval     EFI_SUCCESS       Registration succeed
  @retval     EFI_ACCESS_DENIED Return access denied if the SmmReadyToLock event has been triggered
  @retval     others            Registration failed
**/
EFI_STATUS
EFIAPI
LnkType1ErrRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK  DispatchFunction,
  OUT EFI_HANDLE                      *DispatchHandle
  )
{
  EFI_STATUS Status;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  Status = RegisterSerialIrqIfNull ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Insert a record
  //
  return InsertEspiRecord (DispatchFunction, LnkType1Err, DispatchHandle);
}

//
// EspiSlave srcdesc
//
PCH_SMM_SOURCE_DESC mSrcDescEspiSlave = {
  PCH_SMM_NO_FLAGS,
  {
    {
      {
        ACPI_ADDR_TYPE,
        {R_PCH_SMI_EN}
      },
      S_PCH_SMI_EN,
      N_PCH_SMI_EN_ESPI
    },
    NULL_BIT_DESC_INITIALIZER
  },
  {
    {
      {
        ACPI_ADDR_TYPE,
        {R_PCH_SMI_STS}
      },
      S_PCH_SMI_STS,
      N_PCH_SMI_STS_ESPI
    }
  },
  {
    {
      ACPI_ADDR_TYPE,
      {R_PCH_SMI_STS}
    },
    S_PCH_SMI_STS,
    N_PCH_SMI_STS_ESPI
  }
};

/**
  eSPI SMI Dispatch Protocol instance to register a eSPI slave SMI
  This routine will also lock down ESPI_SMI_LOCK bit after registration and prevent
  this handler from unregistration.
  On platform that supports more than 1 device through another chip select (SPT-H),
  the SMI handler itself needs to inspect both the eSPI devices' interrupt status registers
  (implementation specific for each Slave) in order to identify and service the cause.
  After servicing it, it has to clear the Slaves' internal SMI# status registers

  @param[in]  This                      Not used
  @param[in]  DispatchFunction          The callback to execute
  @param[out] DispatchHandle            The handle for this callback registration

  @retval     EFI_SUCCESS               Registration succeed
  @retval     EFI_ACCESS_DENIED         Return access denied if the SmmReadyToLock event has been triggered
  @retval     EFI_ACCESS_DENIED         The ESPI_SMI_LOCK is set and register is blocked.
  @retval     others                    Registration failed
**/
EFI_STATUS
EFIAPI
EspiSlaveSmiRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL    *This,
  IN  PCH_ESPI_SMI_DISPATCH_CALLBACK    DispatchFunction,
  OUT EFI_HANDLE                        *DispatchHandle
  )
{
  EFI_STATUS                            Status;
  UINT32                                GenPmConA;
  UINTN                                 PmcBaseAddress;

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "Register is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  //
  // If ESPI_SMI_LOCK is set, the register is blocked.
  //
  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );
  GenPmConA = MmioRead32 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_A);
  if (GenPmConA & B_PCH_PMC_GEN_PMCON_A_ESPI_SMI_LOCK) {
    return EFI_ACCESS_DENIED;
  }

  //
  // @note: This service doesn't utilize the data base of mEspiSmiInstance.
  //        While SMI is triggered it directly goes to the registing DispatchFunction
  //        instead of EspiSmiCallback.
  //
  Status = PchSmiRecordInsert (
             &mSrcDescEspiSlave,
             (PCH_SMI_CALLBACK_FUNCTIONS) DispatchFunction,
             PchEspiSmiEspiSlaveType,
             DispatchHandle
             );
  PchSmmClearSource (&mSrcDescEspiSlave);
  PchSmmEnableSource (&mSrcDescEspiSlave);

  //
  // Lock down the ESPI_SMI_LOCK after ESPI SMI is enabled.
  //
  MmioOr8 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_A, B_PCH_PMC_GEN_PMCON_A_ESPI_SMI_LOCK);

  //
  // Keep the DispatchHandle which will be used for unregister function.
  //
  mEspiSmiInstance.PchSmiEspiHandle[EspiPmc] = *DispatchHandle;

  return Status;
}

/**
  eSPI SMI Dispatch Protocol instance to unregister a callback based on handle

  @param[in]  This                    Not used
  @param[in]  DispatchHandle          Handle acquired during registration

  @retval     EFI_SUCCESS             Unregister successful
  @retval     EFI_INVALID_PARAMETER   DispatchHandle is null
  @retval     EFI_INVALID_PARAMETER   DispatchHandle's forward link has bad pointer
  @retval     EFI_INVALID_PARAMETER   DispatchHandle does not exist in database
  @retval     EFI_ACCESS_DENIED       Unregistration is done after end of DXE
  @retval     EFI_ACCESS_DENIED       DispatchHandle is not allowed to unregistered
**/
EFI_STATUS
EFIAPI
EspiSmiUnRegister (
  IN  PCH_ESPI_SMI_DISPATCH_PROTOCOL  *This,
  IN  EFI_HANDLE                      DispatchHandle
  )
{
  EFI_STATUS          Status;
  ESPI_TOP_LEVEL_TYPE EspiTopLevelType;
  ESPI_SMI_TYPE       EspiSmiType;
  BOOLEAN             SafeToDisable;
  LIST_ENTRY          *LinkInDb;
  ESPI_SMI_RECORD     *RecordPointer;

  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Return access denied if the SmmReadyToLock event has been triggered
  //
  if (mReadyToLock == TRUE) {
    DEBUG ((DEBUG_ERROR, "UnRegister is not allowed if the SmmReadyToLock event has been triggered! \n"));
    return EFI_ACCESS_DENIED;
  }

  if (((LIST_ENTRY *) DispatchHandle)->ForwardLink == (LIST_ENTRY *) EFI_BAD_POINTER) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // For DispatchHandle belongs to Espi Slave SMI, refuses the request of unregistration.
  //
  if (mEspiSmiInstance.PchSmiEspiHandle[EspiPmc] == DispatchHandle) {
    DEBUG ((DEBUG_ERROR, "UnRegister is not allowed for ESPI Slave SMI handle! \n"));
    return EFI_ACCESS_DENIED;
  }

  //
  // Iterate through all the database to find the record
  //
  for (EspiSmiType = 0; EspiSmiType < EspiSmiTypeMax; ++EspiSmiType) {
    LinkInDb = GetFirstNode (&mEspiSmiInstance.CallbackDataBase[EspiSmiType]);

    while (!IsNull (&mEspiSmiInstance.CallbackDataBase[EspiSmiType], LinkInDb)) {
      if (LinkInDb != (LIST_ENTRY *) DispatchHandle) {
        LinkInDb = GetNextNode (&mEspiSmiInstance.CallbackDataBase[EspiSmiType], LinkInDb);

      } else {
        //
        // Found the source to be from this list
        //
        RemoveEntryList (LinkInDb);
        RecordPointer = (ESPI_RECORD_FROM_LINK (LinkInDb));

        if (mEspiSmiInstance.EspiSmiEventCounter[EspiSmiType] != 0) {
          if (--mEspiSmiInstance.EspiSmiEventCounter[EspiSmiType] == 0) {
            EspiSmiDisableSource (EspiSmiType);
          }
        }

        Status = gSmst->SmmFreePool (RecordPointer);
        if (EFI_ERROR (Status)) {
          ASSERT (FALSE);
        }

        goto EspiSmiUnRegisterEnd;
      }
    }
  }
  //
  // If the code reach here, the handle passed in cannot be found
  //
  DEBUG ((DEBUG_ERROR, "eSPI SMI handle is not in record database \n"));
  ASSERT (FALSE);
  return EFI_INVALID_PARAMETER;

EspiSmiUnRegisterEnd:

  //
  // Unregister and clear the handle from PchSmiDispatch
  //
  for (EspiTopLevelType = 0; EspiTopLevelType < EspiTopLevelTypeMax; ++EspiTopLevelType) {
    SafeToDisable = TRUE;
    //
    // Checks all the child events that belongs to a top level status in PMC
    //
    for (EspiSmiType = mEspiSmiInstance.Barrier[EspiTopLevelType].Start; EspiSmiType <= mEspiSmiInstance.Barrier[EspiTopLevelType].End; ++EspiSmiType) {
      if (mEspiSmiInstance.EspiSmiEventCounter[EspiSmiType] != 0) {
        SafeToDisable = FALSE;
      }
    }
    //
    // Finally, disable the top level event in PMC
    //
    if (SafeToDisable) {
      if (mEspiSmiInstance.PchSmiEspiHandle[EspiTopLevelType] != NULL) {
        Status = PchSmmCoreUnRegister (NULL, mEspiSmiInstance.PchSmiEspiHandle[EspiTopLevelType]);
        ASSERT_EFI_ERROR (Status);
        mEspiSmiInstance.PchSmiEspiHandle[EspiTopLevelType] = NULL;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  eSPI SMI handler for Fatal Error recovery flow
**/
VOID
EspiRecoverFromFatalError (
  IN  EFI_HANDLE    DispatchHandle
  )
{
  UINT32        Data32;
  EFI_STATUS    Status;
  UINT32        SlaveId;
  UINT32        MaxSlavesCount;
  UINT32        Timeout;

  DEBUG ((DEBUG_ERROR, "EspiRecoverFromFatalError - entry \n"));

  MaxSlavesCount = GetPchSeries () == PchH ? 2 : 1;

  for (SlaveId = 0; SlaveId < MaxSlavesCount; ++SlaveId) {
    //
    // Check if slave has SLCRR bit set. If it does it means it needs recovery.
    //
    Status = PchPcrRead32 (
      PID_ESPISPI,
      (UINT16) (R_PCH_PCR_ESPI_LNKERR_SLV0 + (SlaveId * S_PCH_PCR_ESPI_LNKERR_SLV0)),
      &Data32
      );
    ASSERT_EFI_ERROR (Status);

    //
    // If SLCRR[31] bit is set we need to recover that slave
    //
    if ((Data32 & B_PCH_PCR_ESPI_LNKERR_SLV0_SLCRR) != 0) {
      // 1. Perform in-band reset
      Status = PchEspiSlaveInBandReset (SlaveId);
      DEBUG ((DEBUG_ERROR, "EspiRecoverFromFatalError: PchEspiSlaveInBandReset Status= %r\n", Status));

      // 2. [Optional] Reconfigure link to slave

      // 3. Channels reset

      // Checking Peripheral channel for error
      Status = PchPcrRead32 (
        PID_ESPISPI,
        (UINT16) (R_PCH_PCR_ESPI_PCERR_SLV0 + (SlaveId * S_PCH_PCR_ESPI_PCERR_SLV0)),
        &Data32
        );
      ASSERT_EFI_ERROR (Status);

      if ((Data32 & B_PCH_PCR_ESPI_PCERR_PCFES) != 0) {
        // Disable and enable channel to perform channel reset
        PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_PCREG, &Data32);
        Data32 &= ~(B_PCH_PCR_ESPI_SLAVE_CHEN);
        PchEspiSlaveSetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_PCREG, Data32);

        // TODO: Maybe some delay should be added between enable/disable

        PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_PCREG, &Data32);
        Data32 |= B_PCH_PCR_ESPI_SLAVE_CHEN;
        PchEspiSlaveSetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_PCREG, Data32);

        Timeout = 100;
        while (~(Data32 & B_PCH_PCR_ESPI_SLAVE_CHRDY) && Timeout) {
          PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_PCREG, &Data32);
          MicroSecondDelay(1);
          Timeout--;
        }
        // TODO: Make sure PC Channel is configured correctly
      }

      // Checking Virtual Wire channel for error
      Status = PchPcrRead32 (
        PID_ESPISPI,
        (UINT16) (R_PCH_PCR_ESPI_VWERR_SLV0 + (SlaveId * S_PCH_PCR_ESPI_VWERR_SLV0)),
        &Data32
        );
      ASSERT_EFI_ERROR (Status);
      DEBUG ((DEBUG_ERROR, "[kris] EspiRecoverFromFatalError: Slv%d - VWERR= 0x%08x\n", SlaveId, Data32));

      if ((Data32 & B_PCH_PCR_ESPI_VWERR_VWFES) != 0) {
        // Disable and enable channel to perform channel reset
        PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_VWREG, &Data32);
        Data32 &= ~(B_PCH_PCR_ESPI_SLAVE_CHEN);
        PchEspiSlaveSetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_VWREG, Data32);

        // TODO: Maybe some delay should be added between enable/disable

        PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_VWREG, &Data32);
        Data32 |= B_PCH_PCR_ESPI_SLAVE_CHEN;
        PchEspiSlaveSetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_VWREG, Data32);

        Timeout = 100;
        while (~(Data32 & B_PCH_PCR_ESPI_SLAVE_CHRDY) && Timeout) {
          PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_VWREG, &Data32);
          MicroSecondDelay(1);
          Timeout--;
        }
        // TODO: Make sure VW Channel is configured correctly
      }

      // Flash Access channel is not supported for CS1#
      if (SlaveId == 0) {
        // Checking Flash Access channel for error
        Status = PchPcrRead32 (
          PID_ESPISPI,
          R_PCH_PCR_ESPI_FCERR_SLV0,
          &Data32
          );
        ASSERT_EFI_ERROR (Status);

        // Flash Access Channel Fatal Error Status
        if ((Data32 & B_PCH_PCR_ESPI_FCERR_FCFES) != 0) {
          // Disable and enable channel to perform channel reset
          PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_FCREG, &Data32);
          Data32 &= ~(B_PCH_PCR_ESPI_SLAVE_CHEN);
          PchEspiSlaveSetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_FCREG, Data32);

          // TODO: Maybe some delay should be added between enable/disable

          PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_FCREG, &Data32);
          Data32 |= B_PCH_PCR_ESPI_SLAVE_CHEN;
          PchEspiSlaveSetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_FCREG, Data32);

          Timeout = 100;
          while (~(Data32 & B_PCH_PCR_ESPI_SLAVE_CHRDY) && Timeout) {
            PchEspiSlaveGetConfig (SlaveId, R_PCH_PCR_ESPI_SLAVE_FCREG, &Data32);
            MicroSecondDelay(1);
            Timeout--;
          }
          // TODO: Make sure Flash Access Channel is configured correctly
        }
        // Flash Access Channel Non-Fatal Error Status
        Status = PchPcrRead32 (
          PID_ESPISPI,
          R_PCH_PCR_ESPI_FCERR_SLV0,
          &Data32
          );
        ASSERT_EFI_ERROR (Status);
        DEBUG ((DEBUG_ERROR, "EspiRecoverFromFatalError: Slv%d - FCERR= 0x%08x\n", SlaveId, Data32));
        if ((Data32 & B_PCH_PCR_ESPI_FCERR_FCNFES) != 0) {
          Data32 >>= N_PCH_PCR_ESPI_PCERR_FCNFEC;
          if (Data32 == V_PCH_PCR_ESPI_PCERR_FCNFEC_SRC_NFE ||
            Data32 == V_PCH_PCR_ESPI_PCERR_FCNFEC_SCR_UC ||
            Data32 == V_PCH_PCR_ESPI_PCERR_FCNFEC_UCT ||
            Data32 == V_PCH_PCR_ESPI_PCERR_FCNFEC_UA) {

            DEBUG ((DEBUG_ERROR, "EspiRecoverFromFatalError: Performing Warm Reset\n"));
            IoWrite8 ((UINTN) R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET);
            ///
            /// Waiting for system reset
            ///
            CpuDeadLoop ();
          } else {
            // Clear FCNFES bit of slave
            Status = PchPcrAndThenOr32 (
                        PID_ESPISPI,
                        (UINT16) (R_PCH_PCR_ESPI_FCERR_SLV0),
                        (UINT32) ~0,
                        (UINT32) (B_PCH_PCR_ESPI_FCERR_FCNFES)
                        );
            ASSERT_EFI_ERROR (Status);
          }
        }
      }

      // Clear SLCRR bit of slave
      Status = PchPcrAndThenOr32 (
                  PID_ESPISPI,
                  (UINT16) (R_PCH_PCR_ESPI_LNKERR_SLV0 + (SlaveId * S_PCH_PCR_ESPI_LNKERR_SLV0)),
                  (UINT32)~(B_PCH_PCR_ESPI_LNKERR_SLV0_LFET1S),
                  (UINT32) (B_PCH_PCR_ESPI_LNKERR_SLV0_SLCRR)
                  );
      ASSERT_EFI_ERROR (Status);
    }
  }
}

