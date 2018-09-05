//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++
 
Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaDxe.h

Abstract:

  Header file for the FpgaDxe Driver.

--*/

#ifndef _FPGA_DXE_H
#define _FPGA_DXE_H

#include <PiDxe.h>
#include <SysFunc.h>
#include <IndustryStandard/Pci.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/TimerLib.h>
#include <PciAccess.h>
#include <Protocol/IioSystem.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/IioUds.h>
#include <Protocol/MpService.h>
#include <Protocol/FpgaConfigProtocol.h>
#include <Cpu/CpuRegs.h>
#include <RcRegs.h>
#include <Fpga.h>
#include <Library/FpgaConfigurationLib.h>
#include <Guid/FpgaSocketVariable.h>
#include <FpgaRegs.h>
#include <SysHostChip.h>

#define EFI_MSR_SMRR2_PHYS_BASE                0x1F6
#define EFI_MSR_SMRR2_PHYS_MASK                0x1F7
#define  EFI_MSR_SMRR_PHYS_MASK_VALID          BIT11
#define  EFI_MSR_SMRR_MASK                     0xFFFFF000

#define EFI_PCI_FPGAID(Socket, Bus, Device, Function)  (((UINT32)Socket << 24) + ((UINT32)Bus << 16) + ((UINT32)Device << 8) + (UINT32)Function)
#define EFI_PCI_SOCKET_OF_FPGAID(FPGAID)               ((UINT8)((UINT32)FPGAID >> 24))
#define EFI_PCI_BUS_OF_FPGAID(FPGAID)                  ((UINT8)((UINT32)FPGAID >> 16))
#define EFI_PCI_DEV_OF_FPGAID(FPGAID)                  ((UINT8)((UINT32)FPGAID >> 8))
#define EFI_PCI_FUN_OF_FPGAID(FPGAID)                  ((UINT8)FPGAID)

//
// Define BIOS-Pcode MailBox SMB_ACCESS interface/data
//
typedef union {
  struct {
    UINT32 command    : 8;  // command - Bits[7:0], Data = 0xAE
    UINT32 read_write : 1;  // read_write - Bits[8:8], 0:read; 1:write
    UINT32 cr_index   : 8;
    UINT32 semaphore  : 2;  // 17:Request; 18:Release
    UINT32 rsvd_19    : 10; // Reserved in Address
    UINT32 rsvd_29    : 2;  // Reserved
    UINT32 run_busy   : 1;  // RUN_BUSY
  } Bits;
  UINT32 Data;
} BIOS_MAILBOX_INTERFACE_SMB_ACCESS_STRUCT;

typedef union {
  struct {
    UINT16 smb_rdata  : 16;  // read data in lower 16 bits
    UINT16 smb_wdata  : 16;  // write data in upper 16 bits
  } Bits;
  UINT32 Data;
} BIOS_MAILBOX_DATA_SMB_ACCESS_STRUCT;

#define FPGA_SMB_ACCESS_PCIE_CONFIG_CMD        0x200A9815  // Refer to SMB_CMD register via PCICfg

#endif
