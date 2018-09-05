//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2004 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file NgnAcpiSmmProtocol.h

  Definition of the NGN ACPI SMM area protocol.  This protocol
  publishes the address and format of a NGN Inteface used as a communications
  buffer between SMM code and ASL code.

  Note:  Data structures defined in this protocol are not naturally aligned.

**/

#ifndef _EFI_NGN_ACPI_SMM_INTERFACE_H_
#define _EFI_NGN_ACPI_SMM_INTERFACE_H_

//
// Global NVS Area Protocol GUID
//
#define EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL_GUID \
{ 0x841dda73, 0xa8d, 0x4d49, 0xaf, 0x60, 0x2a, 0x14, 0x5d, 0x21, 0x76, 0xce }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiNgnAcpiSmmInterfaceProtocolGuid;
//
// If any of these 3 defines change below, the same changes need to be reflected in Ngn.asi
#define MAX_INPUT_BUF_SIZE        0x1020
#define MAX_OUTPUT_BUF_SIZE       0x1020
#define MAX_NFIT_TABLE_SIZE       0x8000
#pragma pack (1)
//
// This struct is used primarily for NFIT table and DSM Inteface
// (IMPORTANT NOTE: When updating this structure please update CRCM OperationRegion in Ngn.asi, this structure is shared between ACPI and SMM)
//
// NvDimmConfigBitmap0 - This is the bitmap for Aep Dimms preset in socket 0 thru 3. Let us say that there are Aep Dimms present in dimm0 of socket 0 & 1,
// the least signficant 32 bits would look like this: 00000101010101010000010101010101b. The least significant 16 bits would represent socket0, the
// 12 least significant bits are tells the bitmap of the nvdimms present while the last 4 bits are reserved. In the future Node Number will be
// encoded in these bits.
typedef struct NGN_ACPI_SMM_INTERFACE {
  UINT64       NvDimmConfigBitmap0;// This is the bitmap of Aep Dimms present in sockets 0 thru 3.
  UINT64       NvDimmConfigBitmap1;// This is the bitmap of Aep Dimms present in sockets 4 thru 7.
  UINT64       EnergySourcePolicy0;// Energy Source Policy in sockets 0 thru 3.
  UINT64       EnergySourcePolicy1;// Energy Source Policy in sockets 4 thru 7.
  UINT32       FunctionIndex;      // Function Index
  UINT32       DeviceHandle;       // Uniquely identifies the Dimm
  UINT32       InputOffset;        // Offset of the Input Nvdimm to which the data is to be written
  UINT32       InputOpCode;        // Opcode for Vendor Specific
  UINT32       InputLength;        // Size of the Input Buffer in bytes
  UINT32       OutputLength;       // Output Length in bytes
  UINT64       ArsStartAddress;    // ARS SPA Start Address
  UINT64       ArsLength;          // ARS Length
  UINT16       ArsType;            // ARS Start type
  UINT8        ArsFlags;           // ARS Start flags
  UINT8        IsAepDimmPresent;   // If AEP NVDIMM present, should be set to 1
  UINT32       Status;             // Status as defined above
  UINT8        InputBuffer[MAX_INPUT_BUF_SIZE];
  UINT8        OutputBuffer[MAX_OUTPUT_BUF_SIZE];
} NGN_ACPI_SMM_INTERFACE, *PNGN_ACPI_SMM_INTERFACE;

#pragma pack ()

typedef struct _EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL {
  NGN_ACPI_SMM_INTERFACE     *Area;
} EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL;

#endif
