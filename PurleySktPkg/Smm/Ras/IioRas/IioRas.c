/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2008 - 2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
    IioRas.c

Abstract:


-----------------------------------------------------------------------------*/

//
// Includes
//
#include <SysFunc.h>
#include <Library/SmmServicesTableLib.h>                
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>           
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>                      
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IioInitLib.h>
#include <Library/IioInitDxeLib.h>
#include <Protocol/IioRasProtocol.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/IioSystem.h>
#include <Protocol/IioUds.h>
#include <Library/MmPciBaseLib.h>
#include <Library/IoLib.h>
#include <Include/SysRegs.h>
#include <Ras/RasStateMachine.h>
#include "UncoreCommonIncludes.h"
#include <Register/PchRegsPmc.h>

#define IIO_RAS_SIGNATURE  SIGNATURE_32 ('R', 'I', 'I', 'O')

EFI_IIO_RAS_PROTOCOL      *mIioRasProtocol;
BIOS_ACPI_PARAM           *mAcpiParameter;
EFI_CPU_CSR_ACCESS_PROTOCOL     *mCpuCsrAccess;
IIO_GLOBALS                     *mIioGlobalData;
static EFI_IIO_UDS_PROTOCOL            *mIioUds;

extern EFI_GUID gEfiIioRasProtocolGuid;

/**

  GenerateRasSci() generates an SCI interrupt for Ras OS Consumption

  @param  None.

  @retval EFI_STATUS EFI_SUCCESS - Success.
  @retval other - failure.

**/
EFI_STATUS
GenerateRasSci (
    VOID
)
{
  UINT8  GpeCtrl = 0;
  UINT16 PmBase = 0;

  // PM base address
  PmBase = MmioRead16 (MmPciBase (PMC_BUS, PMC_DEVICE, PMC_FUNC) + R_PCH_PMC_ACPI_BASE);
  PmBase = (UINT16) (PmBase & 0xFF80);

  // Generate SCI event
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (PmBase + R_PCH_ACPI_GPE_CNTL), 1, &GpeCtrl);
  GpeCtrl |= B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (PmBase + R_PCH_ACPI_GPE_CNTL), 1, &GpeCtrl);
  return EFI_SUCCESS;
}

/**

  IioOnline() is responsible for initializing the Incoming Socket's IIO.
  It utilizes the DXE IioInit Library.

  @param  Socket - Denotes the "Incoming Socket" number. i.e. The socket number that is "being online'd"

  @retval EFI_STATUS EFI_SUCCESS - Success.
  @retval other - failure.

**/
EFI_STATUS
EFIAPI
IioOnline (
  IN UINT8 Socket
  )
{
  UINT8           Iio;
  UINT8           SocketPresent[MaxIIO];
  EFI_STATUS      Status;

  Status = EFI_SUCCESS;
  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n  ************ IIO Online Flow In Progress. ************\n\n"));
  mIioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit = (UINT64) mCpuCsrAccess;
  mIioGlobalData->IioVar.IioVData.RasOperation = 1;
  mIioGlobalData->IioVar.IioVData.SocketUnderOnline = Socket;
  //
  // Temporarily modify the SocketPresent data just during this RAS Operation.
  // It will get restored before function exit.
  //
  for ( Iio = 0 ; Iio < MaxIIO ; Iio++ ) {
    if ( Iio == mIioGlobalData->IioVar.IioVData.SocketUnderOnline ) {
      SocketPresent[Iio] = 1;
      mIioGlobalData->IioVar.IioVData.SocketPresent[Iio] = 1;
    } else {
      SocketPresent[Iio] = mIioGlobalData->IioVar.IioVData.SocketPresent[Iio];
      mIioGlobalData->IioVar.IioVData.SocketPresent[Iio] = 0;
    }
  }

  //
  // Call IIO Early Entry (Link training, DFX init)
  //
  Status = (EFI_STATUS)IioEarlyInitializeEntry(mIioGlobalData);
  //
  // Call IIO Late Entry (PCIE port, IOAPIC, NTB, IOAT, VTD init)
  //
  Status = (EFI_STATUS)IioLateInitializeEntry(mIioGlobalData);
  IioCommonInitAfterResources(mIioGlobalData);

  //
  // Restore SocketPresent data
  //
  for ( Iio = 0 ; Iio < MaxIIO ; Iio++ ) {
    mIioGlobalData->IioVar.IioVData.SocketPresent[Iio] = SocketPresent[Iio];
  }
  mIioGlobalData->IioVar.IioVData.RasOperation = 0;
  // Update Acpi Parameters to communicate with ASL
  mAcpiParameter->IioPresentBitMask |= LShiftU64(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap, (Socket * 8));
  mAcpiParameter->IioChangeMask      = (1 << (Socket - 1));
  mAcpiParameter->IoApicEnable      |= (1 << (Socket + 1));
  // Request OS check the bus/device
  mAcpiParameter->SciRequestParam[0]   = SCI_CHECK;
  mAcpiParameter->SciRequestParam[1]   = 0;
  mAcpiParameter->SciRequestParam[2]   = 0;
  mAcpiParameter->SciRequestParam[3]   = 0;

  // Generate SCI
  GenerateRasSci();

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "    IIO Online Flow	Done.\n"));
  return Status;
}

/**

  IioOffline() is responsible for "taking offline" the Outgoing Socket's IIO.
  It utilizes the DXE IioInit Library.

  @param  Socket - Denotes the "Outgoing Socket" number. i.e. The socket number that is "being offline'd"

  @retval EFI_STATUS STATUS_DONE - Success.
  @retval other - failure.

**/
EFI_STATUS
EFIAPI
IioOffline (
  IN UINT8 Socket
  )
{
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "    IIO Offline Flow In Progress.\n"));
  //
  // Send Pcode command to disable IIO
  //
  //IioPortPowerDown(mIioGlobalData);
  //mAcpiParameter->IioPresentBitMask &= ~(1 << Socket);
  //mAcpiParameter->IioChangeMask &= (~1 << (Socket-1));
  //mAcpiParameter->IoApicEnable = 0;
  // Request OS check the bus/device
  //mAcpiParameter->SciRequestParam[0]   = SCI_CHECK;
  //mAcpiParameter->SciRequestParam[1]   = 0;
  //mAcpiParameter->SciRequestParam[2]   = 0;
  //mAcpiParameter->SciRequestParam[3]   = 0;

  // Generate SCI
  GenerateRasSci();

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "    IIO Offline Flow	Done.\n"));
  return STATUS_DONE;
}

//
//	Driver entry point
//
EFI_STATUS
EFIAPI
IioRasDriverEntry (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_HANDLE                    Handle = NULL;
  EFI_IIO_SYSTEM_PROTOCOL       *IioSystemProtocol;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *AcpiNvsProtocol = NULL;
  // If IIO RAS driver is already initialized, then no further action needed.
  Status = gSmst->SmmLocateProtocol (&gEfiIioRasProtocolGuid, NULL, &mIioRasProtocol);
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  //
  // Allocate the protocol instance and install the protocol
  //
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (EFI_IIO_RAS_PROTOCOL), &mIioRasProtocol);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (mIioRasProtocol, sizeof (EFI_IIO_RAS_PROTOCOL));

  //
  // Init Drive Protocols for other SMM driver to call
  //
  mIioRasProtocol->IioOnline = IioOnline;
  mIioRasProtocol->IioOffline = IioOffline;

  //
  // Install IIO RAS protocol
  //
  Status = gSmst->SmmInstallProtocolInterface (&Handle, &gEfiIioRasProtocolGuid, EFI_NATIVE_INTERFACE, mIioRasProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate needed protocols
  //
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &AcpiNvsProtocol);
  ASSERT_EFI_ERROR (Status);
  mAcpiParameter = AcpiNvsProtocol->Area;
  ASSERT (mAcpiParameter);

  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
  ASSERT_EFI_ERROR (Status);
  mIioGlobalData = (IIO_GLOBALS *)IioSystemProtocol->IioGlobalData;

  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid,NULL,&mIioUds);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

