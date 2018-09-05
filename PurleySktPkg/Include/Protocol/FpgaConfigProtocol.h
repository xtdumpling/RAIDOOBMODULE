//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaConfigProtocol.h

Abstract:

  This file provides the header of FPGA HSSI Config data protocol

--*/

#ifndef _FPGA_CONFIG_PROTOCOL_H_
#define _FPGA_CONFIG_PROTOCOL_H_

//
// FPGA HSSI Card ID Type
//
typedef enum {
  HSSI_CardID_Rsvd0   = 0,
  HSSI_CardID_2x25GbE = 1 ,
  HSSI_CardID_Rsvd2   = 2,
  HSSI_CardID_4x25GbE = 3,
  HSSI_CardID_2x40GbE = 4,
  HSSI_CardID_2x50GbE = 5,
  HSSI_CardID_4x10GbE = 6,
  HSSI_CardID_PCIe    = 7
} HSSI_CARD_ID;

typedef struct {
  UINT64       Data;
} HSSI_CONFIG_DATA_TABLE;

//
// Global ID for EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL
//
#define EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL_GUID \
  { \
    0x332b3586, 0xf0b4, 0x46ee, { 0xa3, 0x9c, 0x78, 0x63, 0x30, 0xe2, 0xd3, 0x28 } \
  }

//
// Forward declaration for EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL
//
typedef struct _EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL;

//
// Protocol Data Structures
//
/**
  Load FPGA HSSI config data according to add-in card detection.

  @param This                       Pointer to the EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL instance.
  @param FPGAHssiConfigDataTable    The index of CPU socket.
  @param FPGAHssiConfigDataTable    The HSSI config data table.
  @param NumOfHssiConfigData        A pointer to number of HSSI config data table.

  @retval EFI_SUCCESS:              The function executed successfully.
  @retval Others                    Internal error when load HSSI config data.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_FPGA_CONFIG_DATA) (
  IN EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL   *This,
  IN      UINT8                           SocketId,
  IN OUT  HSSI_CONFIG_DATA_TABLE          **FPGAHssiConfigDataTable,
  IN OUT  UINTN                           *NumOfHssiConfigData
  );

//
// Interface structure for the FPGA HSSI Config Data Protocol
//
struct _EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL {
  //
  //  Returns the data structure of FPGA HSSI config data
  //
  EFI_FPGA_CONFIG_DATA  FpgaConfigData;
};

extern EFI_GUID gEfiFpgaHssiConfigDataProtocol;

#endif
