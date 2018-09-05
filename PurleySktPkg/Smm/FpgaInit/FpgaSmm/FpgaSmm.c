/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2016 Intel Corporation.  All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaSmm.c

Abstract:

  This is the SMM driver for hidding FPGA device.

--*/

#include "FpgaSmm.h"

//
// Modular variables needed by this driver
//
FPGA_CONFIGURATION            FpgaConfiguration;
EFI_CPU_CSR_ACCESS_PROTOCOL   *mCpuCsrAccess;


/**
  Software SMI callback for hidding FPGA device.

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The interrupt was handled successfully.
**/
EFI_STATUS
EFIAPI
FpgaDeviceHideCallback (
  IN EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                  *Context,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
  )
{
  UINT8                         Socket;
  UINT32                        PcuDevhide;
  UINT32                        UboxDevhide;

  DEBUG ((DEBUG_ERROR, "FpgaDeviceHideCallback() software SMI handler enter.\n"));

  //
  // Loop thru all IIO stacks of all sockets that are present
  //
  DEBUG ((DEBUG_ERROR, "FpgaSktActive = 0x%X.\n", FpgaConfiguration.FpgaSktActive));
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (!(FpgaConfiguration.FpgaSktActive & (1 << Socket))) {
      continue;
    }

    UboxDevhide = mCpuCsrAccess->ReadCpuCsr(Socket, 0, DEVHIDE0_4_UBOX_MISC_REG);
    UboxDevhide |= (BIT5 | BIT3 | BIT2 | BIT1 | BIT0);  //UBOX Hide B(4) D0/1/2/3, F0 and Hide MCP0 VTD function @B(4) D5, F0
    mCpuCsrAccess->WriteCpuCsr(Socket, 0, DEVHIDE0_4_UBOX_MISC_REG, UboxDevhide);
    DEBUG ((DEBUG_ERROR, "UBOX Data = 0x%X.\n", mCpuCsrAccess->ReadCpuCsr (Socket, 0, DEVHIDE0_4_UBOX_MISC_REG)));

    PcuDevhide = mCpuCsrAccess->ReadCpuCsr(Socket, 0, DEVHIDE_B4F0_PCU_FUN6_REG);
    PcuDevhide |= (BIT5 | BIT3 | BIT2 | BIT1 | BIT0);  //PCU Hide B(4) D0/1/2/3, F0 and Hide MCP0 VTD function @B(4) D5, F0
    mCpuCsrAccess->WriteCpuCsr(Socket, 0, DEVHIDE_B4F0_PCU_FUN6_REG, PcuDevhide);
    DEBUG ((DEBUG_ERROR, "PCU Data = 0x%X.\n", mCpuCsrAccess->ReadCpuCsr (Socket, 0, DEVHIDE_B4F0_PCU_FUN6_REG)));
  }

  return EFI_SUCCESS;
}

/**
  The driver's entry point.

  It install callbacks for FPGA device hide function.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
FpgaSmmEntryPoint (
  IN EFI_HANDLE                  ImageHandle,
  IN EFI_SYSTEM_TABLE            *SystemTable
  )
{

  EFI_STATUS                     Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT    SwContext;
  EFI_HANDLE                     SwHandle;

  SwHandle = 0;
  DEBUG ((DEBUG_ERROR, "FpgaSmmEntryPoint() Enter\n"));

  //
  // BIOS detect FPGA SKU
  //
  Status = FpgaConfigurationGetValues (&FpgaConfiguration);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_ERROR, "FpgaSktActive = 0x%X.\n", FpgaConfiguration.FpgaSktActive));

  if (FpgaConfiguration.FpgaSktActive == 0) {
    DEBUG ((DEBUG_ERROR, "FpgaSmmEntryPoint() no FPGA activated.\n"));
    return EFI_SUCCESS;
  }

  //
  // Locate CSR Access Protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiCpuCsrAccessGuid,
                    NULL,
                    &mCpuCsrAccess
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Get the Sw dispatch protocol and register SMI callback functions.
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID**) &SwDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  SwContext.SwSmiInputValue = (UINTN) -1;
  Status = SwDispatch->Register (
                         SwDispatch,
                         FpgaDeviceHideCallback,
                         &SwContext,
                         &SwHandle
                         );
  ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FPGA software SMI number: 0x%X\n", (UINT8)SwContext.SwSmiInputValue));
    Status = PcdSet8S (PcdFpgaSwSmiInputValue, (UINT8)SwContext.SwSmiInputValue);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
