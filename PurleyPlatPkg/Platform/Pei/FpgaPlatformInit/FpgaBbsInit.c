/*++
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
--*/

/*++

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


--*/

#include "FpgaPlatformEarlyInit.h"


#define  FPGA_FV_BASE        FixedPcdGet32 (PcdFlashFdFpgaBase)
#define  FPGA_FV_SIZE        FixedPcdGet32 (PcdFlashFdFpgaSize)

#define  SPI_FLASH_64MB_BASE  0xFC000000
#define  SPI_FLASH_32MB_BASE  0xFE000000

EFI_STATUS
LoadFpgaFv (
  IN  EFI_SPI_PROTOCOL  *SpiPpi,
  OUT UINT8             *Buffer,
  IN  UINTN             FvBaseAddr,
  IN  UINTN             FvLength
  )
{
  EFI_STATUS            Status;

  Status = SpiPpi->FlashRead (
                     SpiPpi,
                     FlashRegionAll,
                     FvBaseAddr - SPI_FLASH_64MB_BASE,
                     (UINT32) FvLength,
                     Buffer
                     );
  DEBUG ((EFI_D_ERROR, "[FpgaBbsInit] Load FV from BIOS offset %x status: %r\n", FvBaseAddr - SPI_FLASH_64MB_BASE, Status));

  return Status;
}


/**
  This function is initialize FPGA BBS binaries file

  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS   FPGA BBS binaries file initialize successfully.

**/
EFI_STATUS
EFIAPI
FpgaBbsInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS                  Status;
  EFI_SPI_PROTOCOL            *SpiPpi;
  UINT8                       *Buffer;
  EFI_PHYSICAL_ADDRESS        PhyAddress;
  UINT32                      Size;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader = NULL;

  DEBUG ((EFI_D_ERROR, "[FpgaBbsInit] Entry\n"));

  Buffer = NULL;

  Status = PeiServicesLocatePpi (&gPeiSpiPpiGuid, 0, NULL, (VOID **) &SpiPpi);
  ASSERT_EFI_ERROR (Status);

  Size = FPGA_FV_SIZE;
  Status = (*PeiServices)->AllocatePages (
                             PeiServices,
                             EfiBootServicesCode,
                             EFI_SIZE_TO_PAGES(Size),
                             &PhyAddress
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[FpgaBbsInit] Allocate Memory failed.\n"));
    return Status;
  }
  Buffer = (UINT8 *) (UINTN) PhyAddress;

  Status = LoadFpgaFv (SpiPpi, Buffer, FPGA_FV_BASE, FPGA_FV_SIZE);
  if ((EFI_ERROR (Status)) || (Buffer == NULL)) {
    if (Buffer != NULL) {
      FreePool(Buffer);
    }
    DEBUG ((EFI_D_ERROR, "[FpgaBbsInit] Load FPGA FV failed.\n"));
    return Status;
  }

  Status = ValidateCommonFvHeader ((EFI_FIRMWARE_VOLUME_HEADER *) Buffer);
  if (EFI_ERROR (Status)) {
    FreePool(Buffer);
    DEBUG ((EFI_D_ERROR, "[FpgaBbsInit] FPGA FV at address %x is corrupted\n", FPGA_FV_BASE));
    return Status;
  }

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)Buffer;
  PeiServicesInstallFvInfoPpi (
    &FvHeader->FileSystemGuid,
    (VOID *) (UINTN) Buffer,
    (UINT32) FPGA_FV_SIZE,
    NULL,
    NULL
    );

  BuildFvHob (
    (UINTN) Buffer,
    (UINT32) FPGA_FV_SIZE
    );

  return Status;
}
