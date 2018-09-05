/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  PcatProtocol.h

@brief:

  Protocol for accessing ACPI PCAT (Platform Configuration Attributes Table)

**/

#ifndef _PCAT_PROTOCOL_H_
#define _PCAT_PROTOCOL_H_

extern EFI_GUID gAcpiPcatProtocolGuid;

//
// Type definitions
//

typedef
EFI_STATUS
(EFIAPI *EFI_PCAT_INITIALIZE_PCAT) (
  OUT UINT64* PcatTablePtr,
  IN  UINT64 PcatTableSize
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PCAT_CREATE_SAET) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PCAT_ADD_VLR_TO_SAET) (
  IN  UINT8   *Vlr,
  IN  UINTN   VlrSize
  );

typedef
BOOLEAN
(EFIAPI *EFI_PCAT_IS_INITIALIZED) (
  VOID
  );
///
/// Protocol definition
///
typedef struct {
  EFI_PCAT_INITIALIZE_PCAT  InitializeAcpiPcat;
  EFI_PCAT_CREATE_SAET      CreateAcpiPcatSaet;
  EFI_PCAT_ADD_VLR_TO_SAET  AddVlr2PcatSaet;
  EFI_PCAT_IS_INITIALIZED   IsInitialized;
} EFI_ACPI_PCAT_PROTOCOL;




#endif _PCAT_PROTOCOL_H_
