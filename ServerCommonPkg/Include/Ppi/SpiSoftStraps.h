//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SpiSoftStraps.h
    
Abstract:

  SPI SoftStraps PPI as defined in EFI 2.0

--*/

#ifndef _PEI_SPI_SOFTSTRAPS_PPI_H
#define _PEI_SPI_SOFTSTRAPS_PPI_H

#define PEI_SPI_SOFTSTRAPS_PPI_GUID \
  { \
    0x7F19E716, 0x419C, 0x4E79, { 0x8E, 0x37, 0xC2, 0xBD, 0x84, 0xEB, 0x65, 0x28 } \
  }

typedef	enum{
	EfiPeiSpiSoftStrapsWidthMaximum
} EFI_PEI_SPI_SOFTSTRAPS_PPI_WIDTH;

typedef struct _EFI_PEI_SPI_SOFTSTRAPS_PPI EFI_PEI_SPI_SOFTSTRAPS_PPI;

typedef
EFI_STATUS
(EFIAPI	*EFI_PEI_SPI_SOFTSTRAPS_READ) (
	IN EFI_PEI_SERVICES                   **PeiServices,
	IN EFI_PEI_SPI_SOFTSTRAPS_PPI         *This,
	IN EFI_PEI_SPI_SOFTSTRAPS_PPI_WIDTH   Width,
	IN UINT64                             Address,
	IN OUT VOID                           *Buffer
);

typedef
EFI_STATUS
(EFIAPI	*EFI_PEI_SPI_SOFTSTRAPS_WRITE) (
	IN EFI_PEI_SERVICES                   **PeiServices,
	IN EFI_PEI_SPI_SOFTSTRAPS_PPI         *This,
	IN EFI_PEI_SPI_SOFTSTRAPS_PPI_WIDTH   Width,
	IN UINT64                             Address,
	IN OUT VOID                           *Buffer
);

typedef struct _EFI_PEI_SPI_SOFTSTRAPS_PPI {
	EFI_PEI_SPI_SOFTSTRAPS_READ			  SoftStrapRead;
	EFI_PEI_SPI_SOFTSTRAPS_WRITE			SoftStrapWrite;
} EFI_PEI_SPI_SOFTSTRAPS_PPI;

extern EFI_GUID gPeiSpiSoftStrapsPpiGuid;

#endif
