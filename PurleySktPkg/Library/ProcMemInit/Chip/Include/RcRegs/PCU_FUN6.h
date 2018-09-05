//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      Reference Code                                                    *
// *                                                                        *
// *      Copyright (c) 2007-2016, Intel Corporation.                       *
// *                                                                        *
// *      This software and associated documentation (if any) is furnished  *
// *      under a license and may only be used or copied in accordance      *
// *      with the terms of the license. Except as permitted by such        *
// *      license, no part of this software or documentation may be         *
// *      reproduced, stored in a retrieval system, or transmitted in any   *
// *      form or by any means without the express written consent of       *
// *      Intel Corporation.                                                *
// *                                                                        *
// *      This program has been developed by Intel Corporation.             *
// *      Licensee has Intel's permission to incorporate this source code   *
// *      into their product, royalty free.  This source code may NOT be    *
// *      redistributed to anyone without Intel's written permission.       *
// *                                                                        *
// *      Intel specifically disclaims all warranties, express or           *
// *      implied, and all liability, including consequential and other     *
// *      indirect damages, for the use of this code, including liability   *
// *      for infringement of any proprietary rights, and including the     *
// *      warranties of merchantability and fitness for a particular        *
// *      purpose.  Intel does not assume any responsibility for any        *
// *      errors which may appear in this code nor any responsibility to    *
// *      update it.                                                        *
// *                                                                        *
// **************************************************************************
// **************************************************************************
// *                                                                        *
// *  PURPOSE:                                                              *
// *                                                                        *
// *      This file contains Silicon register definitions.                  *
// *                                                                        *
// **************************************************************************
//
/* Date Stamp: 9/13/2016 */

#ifndef PCU_FUN6_h
#define PCU_FUN6_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* PCU_FUN6_DEV 30                                                            */
/* PCU_FUN6_FUN 6                                                             */

/* VID_PCU_FUN6_REG supported on:                                             */
/*       SKX_A0 (0x201F6000)                                                  */
/*       SKX (0x201F6000)                                                     */
/* Register default value:              0x8086                                */
#define VID_PCU_FUN6_REG 0x04062000
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Vendor ID Register
 */
typedef union {
  struct {
    UINT16 vendor_identification_number : 16;
    /* vendor_identification_number - Bits[15:0], RO, default = 16'h8086 
       The value is assigned by PCI-SIG to Intel.
     */
  } Bits;
  UINT16 Data;
} VID_PCU_FUN6_STRUCT;


/* DID_PCU_FUN6_REG supported on:                                             */
/*       SKX_A0 (0x201F6002)                                                  */
/*       SKX (0x201F6002)                                                     */
/* Register default value:              0x2086                                */
#define DID_PCU_FUN6_REG 0x04062002
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2086 
        
       Start (h)	End (h)	Device
       0x203F		0x203F	
       0x2040		0x204B	MC
       0x204C		0x204F	M3KTI
       0x2054		0x2054	CHASADALL
       0x2055		0x2055	CHAUTILALL
       0x2056		0x2056	CHAUTILALL1
       0x2057		0x2057	CHAPMAALL
       0x2058		0x205B	KTI
       0x2066		0x2067	M2MEM
       0x2068		0x206F	MC DDRIO
       0x2078		0x207F	VCU
       0x2080		0x2087	PCU
       0x2088		0x2088	
       0x2089		0x2089	
       0x208A		0x208A	
       0x208B		0x208B	
       0x208C		0x208C	
       0x208D		0x208D	CHAUTIL
       0x208E		0x208E	CHASAD
       0x208F		0x208F	CMSCHA
       0x2090		0x209F	RSVD FOR PCU FUNCTION 3
       	
     */
  } Bits;
  UINT16 Data;
} DID_PCU_FUN6_STRUCT;


/* PCICMD_PCU_FUN6_REG supported on:                                          */
/*       SKX_A0 (0x201F6004)                                                  */
/*       SKX (0x201F6004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_PCU_FUN6_REG 0x04062004
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Command Register
 */
typedef union {
  struct {
    UINT16 io_space_enable : 1;
    /* io_space_enable - Bits[0:0], RO, default = 1'b0 
       Hardwired to 0 since these devices don't decode any IO BARs
     */
    UINT16 memory_space_enable : 1;
    /* memory_space_enable - Bits[1:1], RO, default = 1'b0 
       Hardwired to 0 since these devices don't decode any memory BARs
     */
    UINT16 bus_master_enable : 1;
    /* bus_master_enable - Bits[2:2], RO, default = 1'b0 
       Hardwired to 0 since these devices don't generate any transactions
     */
    UINT16 special_cycle_enable : 1;
    /* special_cycle_enable - Bits[3:3], RO, default = 1'b0 
       Not applicable. Hardwired to 0.
     */
    UINT16 memory_write_and_invalidate_enable : 1;
    /* memory_write_and_invalidate_enable - Bits[4:4], RO, default = 1'b0 
       Not applicable to internal devices. Hardwired to 0.
     */
    UINT16 vga_palette_snoop_enable : 1;
    /* vga_palette_snoop_enable - Bits[5:5], RO, default = 1'b0 
       Not applicable to internal devices. Hardwired to 0.
     */
    UINT16 parity_error_response : 1;
    /* parity_error_response - Bits[6:6], RO, default = 1'b0 
       This bit has no impact on error reporting from these devices
     */
    UINT16 idsel_stepping_wait_cycle_control : 1;
    /* idsel_stepping_wait_cycle_control - Bits[7:7], RO, default = 1'b0 
       Not applicable to internal devices. Hardwired to 0.
     */
    UINT16 serr_enable : 1;
    /* serr_enable - Bits[8:8], RO, default = 1'b0 
       This bit has no impact on error reporting from these devices
     */
    UINT16 fast_back_to_back_enable : 1;
    /* fast_back_to_back_enable - Bits[9:9], RO, default = 1'b0 
       Not applicable to PCI Express and is hardwired to 0
     */
    UINT16 intx_disable : 1;
    /* intx_disable - Bits[10:10], RO, default = 1'b0 
       N/A for these devices
     */
    UINT16 rsvd : 5;
    /* rsvd - Bits[15:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT16 Data;
} PCICMD_PCU_FUN6_STRUCT;


/* PCISTS_PCU_FUN6_REG supported on:                                          */
/*       SKX_A0 (0x201F6006)                                                  */
/*       SKX (0x201F6006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_PCU_FUN6_REG 0x04062006
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Status Register
 */
typedef union {
  struct {
    UINT16 rsvd : 3;
    /* rsvd - Bits[2:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT16 intx_status : 1;
    /* intx_status - Bits[3:3], RO, default = 1'b0 
       Reflects the state of the INTA# signal at the input of the enable/disable 
       circuit.  This bit is set by HW to 1 when the INTA# is asserted.  This bit is 
       reset by HW to 0 after the interrupt is cleared (independent of the state of the 
       Interrupt Disable bit in the PCICMD register). 
       Hardwired to 0 on the processor
     */
    UINT16 capabilities_list : 1;
    /* capabilities_list - Bits[4:4], RO, default = 1'b0 
       This bit indicates the presence of a capabilities list structure. When set to 1, 
       indicates the register at 34h provides an offset into the function. 
     */
    UINT16 x66mhz_capable : 1;
    /* x66mhz_capable - Bits[5:5], RO, default = 1'b0 
       Not applicable to PCI Express. Hardwired to 0.
     */
    UINT16 reserved : 1;
    /* reserved - Bits[6:6], RO, default = 1'b0 
       Reserved
     */
    UINT16 fast_back_to_back : 1;
    /* fast_back_to_back - Bits[7:7], RO, default = 1'b0 
       Not applicable to PCI Express. Hardwired to 0.
     */
    UINT16 master_data_parity_error : 1;
    /* master_data_parity_error - Bits[8:8], RO, default = 1'b0 
       Hardwired to 0
     */
    UINT16 devsel_timing : 2;
    /* devsel_timing - Bits[10:9], RO, default = 2'b00 
       Not applicable to PCI Express. Hardwired to 0.
     */
    UINT16 signaled_target_abort : 1;
    /* signaled_target_abort - Bits[11:11], RO, default = 1'b0 
       Hardwired to 0
     */
    UINT16 received_target_abort : 1;
    /* received_target_abort - Bits[12:12], RO, default = 1'b0 
       Hardwired to 0
     */
    UINT16 received_master_abort : 1;
    /* received_master_abort - Bits[13:13], RO, default = 1'b0 
       Hardwired to 0
     */
    UINT16 signaled_system_error : 1;
    /* signaled_system_error - Bits[14:14], RO, default = 1'b0 
       Hardwired to 0
     */
    UINT16 detected_parity_error : 1;
    /* detected_parity_error - Bits[15:15], RO, default = 1'b0 
       This bit is set when the device receives a packet on the primary side with an 
       uncorrectable data error (including a packet with poison bit set) or an 
       uncorrectable address/control parity error. The setting of this bit is 
       regardless of the Parity Error Response bit (PERRE) in the PCICMD register. 
     */
  } Bits;
  UINT16 Data;
} PCISTS_PCU_FUN6_STRUCT;


/* RID_PCU_FUN6_REG supported on:                                             */
/*       SKX_A0 (0x101F6008)                                                  */
/*       SKX (0x101F6008)                                                     */
/* Register default value:              0x00                                  */
#define RID_PCU_FUN6_REG 0x04061008
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * "PCIe header Revision ID register"
 */
typedef union {
  struct {
    UINT8 revision_id : 8;
    /* revision_id - Bits[7:0], ROS_V, default = 8'b00000000 
       Reflects the Uncore Revision ID after reset.
       Reflects the Compatibility Revision ID after BIOS writes 0x69 to any RID 
       register in the processor uncore. 
               
     */
  } Bits;
  UINT8 Data;
} RID_PCU_FUN6_STRUCT;


/* CCR_N0_PCU_FUN6_REG supported on:                                          */
/*       SKX_A0 (0x101F6009)                                                  */
/*       SKX (0x101F6009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_PCU_FUN6_REG 0x04061009
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_PCU_FUN6_STRUCT;


/* CCR_N1_PCU_FUN6_REG supported on:                                          */
/*       SKX_A0 (0x201F600A)                                                  */
/*       SKX (0x201F600A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_PCU_FUN6_REG 0x0406200A
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT16 sub_class : 8;
    /* sub_class - Bits[7:0], RO_V, default = 8'b10000000 
       The value changes dependent upon the dev/func accessed. A table of the values 
       can be found in the Class-code tab of the msgch spread-sheet  
          Most dev-func will return 8'h80 for this field except for the following 
       dev-func0,func1,... combinations. The following exceptions will return 8'h01. 
       Please refer to /src/uncore/ncu/f_ccr_reg.vh for the full lookingup table. 
                 bus-0, dev-8 func-1 
                 bus-3, dev-14 to 16, func-0
                 bus-3, dev-18, func-0, 1, 4, 5
               
     */
    UINT16 base_class : 8;
    /* base_class - Bits[15:8], RO_V, default = 8'b00001000 
       The value changes dependent upon the dev-func accessed. A table of the values 
       can be found in the Class-code tab of the msgch spread-sheet  
          Most bus-dev-func will return 8'h08 for this field except for the following 
       bus-dev-func0,func1,... combinations. The following exceptions will return 
       8'h11. Please refer to /src/uncore/ncu/f_ccr_reg.vhfor the full lookingup table. 
                 bus-0, dev-8 func-1 
                 bus-3, dev-14 to 16, func-0
                 bus-3, dev-18, func-0, 1, 4, 5
               
     */
  } Bits;
  UINT16 Data;
} CCR_N1_PCU_FUN6_STRUCT;


/* CLSR_PCU_FUN6_REG supported on:                                            */
/*       SKX_A0 (0x101F600C)                                                  */
/*       SKX (0x101F600C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_PCU_FUN6_REG 0x0406100C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Cache Line Size Register
 */
typedef union {
  struct {
    UINT8 cacheline_size : 8;
    /* cacheline_size - Bits[7:0], RO, default = 8'b00000000 
       Size of Cacheline
     */
  } Bits;
  UINT8 Data;
} CLSR_PCU_FUN6_STRUCT;


/* PLAT_PCU_FUN6_REG supported on:                                            */
/*       SKX_A0 (0x101F600D)                                                  */
/*       SKX (0x101F600D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_PCU_FUN6_REG 0x0406100D
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Latency Timer
 */
typedef union {
  struct {
    UINT8 primary_latency_timer : 8;
    /* primary_latency_timer - Bits[7:0], RO, default = 8'b00000000 
       Not applicable to PCI-Express. Hardwired to 00h.
     */
  } Bits;
  UINT8 Data;
} PLAT_PCU_FUN6_STRUCT;


/* HDR_PCU_FUN6_REG supported on:                                             */
/*       SKX_A0 (0x101F600E)                                                  */
/*       SKX (0x101F600E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_PCU_FUN6_REG 0x0406100E
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Header Type
 */
typedef union {
  struct {
    UINT8 configuration_layout : 7;
    /* configuration_layout - Bits[6:0], RO, default = 7'b0 
       Type 0 header
     */
    UINT8 multi_function_device : 1;
    /* multi_function_device - Bits[7:7], RO, default = 1'b1 
       This bit defaults to 1b since all these devices are multi-function
     */
  } Bits;
  UINT8 Data;
} HDR_PCU_FUN6_STRUCT;


/* BIST_PCU_FUN6_REG supported on:                                            */
/*       SKX_A0 (0x101F600F)                                                  */
/*       SKX (0x101F600F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_PCU_FUN6_REG 0x0406100F
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI BIST Register
 */
typedef union {
  struct {
    UINT8 bist_tests : 8;
    /* bist_tests - Bits[7:0], RO, default = 8'b0 
       Not supported. Hardwired to 00h
     */
  } Bits;
  UINT8 Data;
} BIST_PCU_FUN6_STRUCT;


/* SVID_PCU_FUN6_REG supported on:                                            */
/*       SKX_A0 (0x201F602C)                                                  */
/*       SKX (0x201F602C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_PCU_FUN6_REG 0x0406202C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Subsystem Vendor ID Register
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RWS_O, default = 16'h8086 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_PCU_FUN6_STRUCT;


/* SDID_PCU_FUN6_REG supported on:                                            */
/*       SKX_A0 (0x201F602E)                                                  */
/*       SKX (0x201F602E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_PCU_FUN6_REG 0x0406202E
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Subsystem device ID Register
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RWS_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_PCU_FUN6_STRUCT;


/* CAPPTR_PCU_FUN6_REG supported on:                                          */
/*       SKX_A0 (0x101F6034)                                                  */
/*       SKX (0x101F6034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_PCU_FUN6_REG 0x04061034
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Capability Pointer Register
 */
typedef union {
  struct {
    UINT8 capability_pointer : 8;
    /* capability_pointer - Bits[7:0], RO, default = 8'h00 
       Points to the first capability structure for the device which is the PCIe 
       capability. 
     */
  } Bits;
  UINT8 Data;
} CAPPTR_PCU_FUN6_STRUCT;


/* INTL_PCU_FUN6_REG supported on:                                            */
/*       SKX_A0 (0x101F603C)                                                  */
/*       SKX (0x101F603C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_PCU_FUN6_REG 0x0406103C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Interrupt Line Register
 */
typedef union {
  struct {
    UINT8 interrupt_line : 8;
    /* interrupt_line - Bits[7:0], RO, default = 8'b0 
       N/A for these devices
     */
  } Bits;
  UINT8 Data;
} INTL_PCU_FUN6_STRUCT;


/* INTPIN_PCU_FUN6_REG supported on:                                          */
/*       SKX_A0 (0x101F603D)                                                  */
/*       SKX (0x101F603D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_PCU_FUN6_REG 0x0406103D
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Interrupt Pin Register
 */
typedef union {
  struct {
    UINT8 interrupt_pin : 8;
    /* interrupt_pin - Bits[7:0], RO, default = 8'b0 
       N/A since these devices do not generate any interrupt on their own
     */
  } Bits;
  UINT8 Data;
} INTPIN_PCU_FUN6_STRUCT;


/* MINGNT_PCU_FUN6_REG supported on:                                          */
/*       SKX_A0 (0x101F603E)                                                  */
/*       SKX (0x101F603E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_PCU_FUN6_REG 0x0406103E
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Min Grant Register
 */
typedef union {
  struct {
    UINT8 mgv : 8;
    /* mgv - Bits[7:0], RO, default = 8'b0 
       The device does not burst as a PCI compliant master.
     */
  } Bits;
  UINT8 Data;
} MINGNT_PCU_FUN6_STRUCT;


/* MAXLAT_PCU_FUN6_REG supported on:                                          */
/*       SKX_A0 (0x101F603F)                                                  */
/*       SKX (0x101F603F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_PCU_FUN6_REG 0x0406103F
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * PCI Max Latency Register
 */
typedef union {
  struct {
    UINT8 mlv : 8;
    /* mlv - Bits[7:0], RO, default = 8'b00000000 
       The device has no specific requirements for how often it needs to access the PCI 
       bus. 
     */
  } Bits;
  UINT8 Data;
} MAXLAT_PCU_FUN6_STRUCT;


/* DEVHIDE_B0F0_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6040)                                                  */
/*       SKX (0x401F6040)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F0_PCU_FUN6_REG 0x04064040
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 9;
    /* dis_dev - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 23;
    /* devunused - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B0F0_PCU_FUN6_STRUCT;


/* DEVHIDE_B0F1_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6044)                                                  */
/*       SKX (0x401F6044)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F1_PCU_FUN6_REG 0x04064044
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 9;
    /* dis_dev - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 23;
    /* devunused - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B0F1_PCU_FUN6_STRUCT;


/* DEVHIDE_B0F2_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6048)                                                  */
/*       SKX (0x401F6048)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F2_PCU_FUN6_REG 0x04064048
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 9;
    /* dis_dev - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 23;
    /* devunused - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B0F2_PCU_FUN6_STRUCT;


/* DEVHIDE_B0F3_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F604C)                                                  */
/*       SKX (0x401F604C)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F3_PCU_FUN6_REG 0x0406404C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 9;
    /* dis_dev - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 23;
    /* devunused - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B0F3_PCU_FUN6_STRUCT;


/* DEVHIDE_B0F4_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6050)                                                  */
/*       SKX (0x401F6050)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F4_PCU_FUN6_REG 0x04064050
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 9;
    /* dis_dev - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 23;
    /* devunused - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B0F4_PCU_FUN6_STRUCT;


/* DEVHIDE_B0F5_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6054)                                                  */
/*       SKX (0x401F6054)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F5_PCU_FUN6_REG 0x04064054
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 9;
    /* dis_dev - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 23;
    /* devunused - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B0F5_PCU_FUN6_STRUCT;


/* DEVHIDE_B0F6_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6058)                                                  */
/*       SKX (0x401F6058)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F6_PCU_FUN6_REG 0x04064058
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 9;
    /* dis_dev - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 23;
    /* devunused - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B0F6_PCU_FUN6_STRUCT;


/* DEVHIDE_B0F7_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F605C)                                                  */
/*       SKX (0x401F605C)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F7_PCU_FUN6_REG 0x0406405C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 9;
    /* dis_dev - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 23;
    /* devunused - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B0F7_PCU_FUN6_STRUCT;


/* DEVHIDE_B1F0_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6060)                                                  */
/*       SKX (0x401F6060)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F0_PCU_FUN6_REG 0x04064060
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 32;
    /* dis_dev - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B1F0_PCU_FUN6_STRUCT;


/* DEVHIDE_B1F1_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6064)                                                  */
/*       SKX (0x401F6064)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F1_PCU_FUN6_REG 0x04064064
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 32;
    /* dis_dev - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B1F1_PCU_FUN6_STRUCT;


/* DEVHIDE_B1F2_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6068)                                                  */
/*       SKX (0x401F6068)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F2_PCU_FUN6_REG 0x04064068
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 32;
    /* dis_dev - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B1F2_PCU_FUN6_STRUCT;


/* DEVHIDE_B1F3_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F606C)                                                  */
/*       SKX (0x401F606C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F3_PCU_FUN6_REG 0x0406406C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 32;
    /* dis_dev - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B1F3_PCU_FUN6_STRUCT;


/* DEVHIDE_B1F4_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6070)                                                  */
/*       SKX (0x401F6070)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F4_PCU_FUN6_REG 0x04064070
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 32;
    /* dis_dev - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B1F4_PCU_FUN6_STRUCT;


/* DEVHIDE_B1F5_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6074)                                                  */
/*       SKX (0x401F6074)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F5_PCU_FUN6_REG 0x04064074
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 32;
    /* dis_dev - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B1F5_PCU_FUN6_STRUCT;


/* DEVHIDE_B1F6_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6078)                                                  */
/*       SKX (0x401F6078)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F6_PCU_FUN6_REG 0x04064078
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 32;
    /* dis_dev - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B1F6_PCU_FUN6_STRUCT;


/* DEVHIDE_B1F7_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F607C)                                                  */
/*       SKX (0x401F607C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F7_PCU_FUN6_REG 0x0406407C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 32;
    /* dis_dev - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B1F7_PCU_FUN6_STRUCT;


/* DEVHIDE_B2F0_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6080)                                                  */
/*       SKX (0x401F6080)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F0_PCU_FUN6_REG 0x04064080
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 14;
    /* dis_dev - Bits[13:0], RW_LB, default = 14'b00000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 8;
    /* dis_dev1 - Bits[21:14], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 2;
    /* dis_dev2 - Bits[23:22], RW_LB, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 8;
    /* dis_dev3 - Bits[31:24], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B2F0_PCU_FUN6_STRUCT;


/* DEVHIDE_B2F1_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6084)                                                  */
/*       SKX (0x401F6084)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F1_PCU_FUN6_REG 0x04064084
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 14;
    /* dis_dev - Bits[13:0], RW_LB, default = 14'b00000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 8;
    /* dis_dev1 - Bits[21:14], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 2;
    /* dis_dev2 - Bits[23:22], RW_LB, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 8;
    /* dis_dev3 - Bits[31:24], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B2F1_PCU_FUN6_STRUCT;


/* DEVHIDE_B2F2_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6088)                                                  */
/*       SKX (0x401F6088)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F2_PCU_FUN6_REG 0x04064088
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 14;
    /* dis_dev - Bits[13:0], RW_LB, default = 14'b00000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 8;
    /* dis_dev1 - Bits[21:14], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 2;
    /* dis_dev2 - Bits[23:22], RW_LB, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 8;
    /* dis_dev3 - Bits[31:24], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B2F2_PCU_FUN6_STRUCT;


/* DEVHIDE_B2F3_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F608C)                                                  */
/*       SKX (0x401F608C)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F3_PCU_FUN6_REG 0x0406408C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 14;
    /* dis_dev - Bits[13:0], RW_LB, default = 14'b00000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 8;
    /* dis_dev1 - Bits[21:14], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 2;
    /* dis_dev2 - Bits[23:22], RW_LB, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 8;
    /* dis_dev3 - Bits[31:24], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B2F3_PCU_FUN6_STRUCT;


/* DEVHIDE_B2F4_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6090)                                                  */
/*       SKX (0x401F6090)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F4_PCU_FUN6_REG 0x04064090
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 14;
    /* dis_dev - Bits[13:0], RW_LB, default = 14'b00000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 8;
    /* dis_dev1 - Bits[21:14], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 2;
    /* dis_dev2 - Bits[23:22], RW_LB, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 8;
    /* dis_dev3 - Bits[31:24], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B2F4_PCU_FUN6_STRUCT;


/* DEVHIDE_B2F5_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6094)                                                  */
/*       SKX (0x401F6094)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F5_PCU_FUN6_REG 0x04064094
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 14;
    /* dis_dev - Bits[13:0], RW_LB, default = 14'b00000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 8;
    /* dis_dev1 - Bits[21:14], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 2;
    /* dis_dev2 - Bits[23:22], RW_LB, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 8;
    /* dis_dev3 - Bits[31:24], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B2F5_PCU_FUN6_STRUCT;


/* DEVHIDE_B2F6_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F6098)                                                  */
/*       SKX (0x401F6098)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F6_PCU_FUN6_REG 0x04064098
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 14;
    /* dis_dev - Bits[13:0], RW_LB, default = 14'b00000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 8;
    /* dis_dev1 - Bits[21:14], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 2;
    /* dis_dev2 - Bits[23:22], RW_LB, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 8;
    /* dis_dev3 - Bits[31:24], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B2F6_PCU_FUN6_STRUCT;


/* DEVHIDE_B2F7_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F609C)                                                  */
/*       SKX (0x401F609C)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F7_PCU_FUN6_REG 0x0406409C
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 14;
    /* dis_dev - Bits[13:0], RW_LB, default = 14'b00000000000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 8;
    /* dis_dev1 - Bits[21:14], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 2;
    /* dis_dev2 - Bits[23:22], RW_LB, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 8;
    /* dis_dev3 - Bits[31:24], RO, default = 8'b11111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B2F7_PCU_FUN6_STRUCT;


/* DEVHIDE_B3F0_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60A0)                                                  */
/*       SKX (0x401F60A0)                                                     */
/* Register default value:              0xFF183F00                            */
#define DEVHIDE_B3F0_PCU_FUN6_REG 0x040640A0
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 6;
    /* dis_dev1 - Bits[13:8], RO, default = 6'b111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 5;
    /* dis_dev2 - Bits[18:14], RW_LB, default = 5'b00000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 2;
    /* dis_dev3 - Bits[20:19], RO, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev4 : 3;
    /* dis_dev4 - Bits[23:21], RW_LB, default = 3'b000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 8;
    /* devunused - Bits[31:24], RO, default = 8'b11111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B3F0_PCU_FUN6_STRUCT;


/* DEVHIDE_B3F1_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60A4)                                                  */
/*       SKX (0x401F60A4)                                                     */
/* Register default value:              0xFF183F00                            */
#define DEVHIDE_B3F1_PCU_FUN6_REG 0x040640A4
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 6;
    /* dis_dev1 - Bits[13:8], RO, default = 6'b111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 5;
    /* dis_dev2 - Bits[18:14], RW_LB, default = 5'b00000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 2;
    /* dis_dev3 - Bits[20:19], RO, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev4 : 3;
    /* dis_dev4 - Bits[23:21], RW_LB, default = 3'b000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 8;
    /* devunused - Bits[31:24], RO, default = 8'b11111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B3F1_PCU_FUN6_STRUCT;


/* DEVHIDE_B3F2_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60A8)                                                  */
/*       SKX (0x401F60A8)                                                     */
/* Register default value:              0xFF183F00                            */
#define DEVHIDE_B3F2_PCU_FUN6_REG 0x040640A8
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 6;
    /* dis_dev1 - Bits[13:8], RO, default = 6'b111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 5;
    /* dis_dev2 - Bits[18:14], RW_LB, default = 5'b00000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 2;
    /* dis_dev3 - Bits[20:19], RO, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev4 : 3;
    /* dis_dev4 - Bits[23:21], RW_LB, default = 3'b000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 8;
    /* devunused - Bits[31:24], RO, default = 8'b11111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B3F2_PCU_FUN6_STRUCT;


/* DEVHIDE_B3F3_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60AC)                                                  */
/*       SKX (0x401F60AC)                                                     */
/* Register default value:              0xFF183F00                            */
#define DEVHIDE_B3F3_PCU_FUN6_REG 0x040640AC
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 6;
    /* dis_dev1 - Bits[13:8], RO, default = 6'b111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 5;
    /* dis_dev2 - Bits[18:14], RW_LB, default = 5'b00000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 2;
    /* dis_dev3 - Bits[20:19], RO, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev4 : 3;
    /* dis_dev4 - Bits[23:21], RW_LB, default = 3'b000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 8;
    /* devunused - Bits[31:24], RO, default = 8'b11111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B3F3_PCU_FUN6_STRUCT;


/* DEVHIDE_B3F4_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60B0)                                                  */
/*       SKX (0x401F60B0)                                                     */
/* Register default value:              0xFF1BFF00                            */
#define DEVHIDE_B3F4_PCU_FUN6_REG 0x040640B0
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 10;
    /* dis_dev1 - Bits[17:8], RO, default = 10'b1111111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 1;
    /* dis_dev2 - Bits[18:18], RW_LB, default = 1'b0 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 2;
    /* dis_dev3 - Bits[20:19], RO, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev4 : 3;
    /* dis_dev4 - Bits[23:21], RW_LB, default = 3'b000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 8;
    /* devunused - Bits[31:24], RO, default = 8'b11111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B3F4_PCU_FUN6_STRUCT;


/* DEVHIDE_B3F5_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60B4)                                                  */
/*       SKX (0x401F60B4)                                                     */
/* Register default value:              0xFF1BFF00                            */
#define DEVHIDE_B3F5_PCU_FUN6_REG 0x040640B4
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 10;
    /* dis_dev1 - Bits[17:8], RO, default = 10'b1111111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 1;
    /* dis_dev2 - Bits[18:18], RW_LB, default = 1'b0 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 2;
    /* dis_dev3 - Bits[20:19], RO, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev4 : 3;
    /* dis_dev4 - Bits[23:21], RW_LB, default = 3'b000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 8;
    /* devunused - Bits[31:24], RO, default = 8'b11111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B3F5_PCU_FUN6_STRUCT;


/* DEVHIDE_B3F6_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60B8)                                                  */
/*       SKX (0x401F60B8)                                                     */
/* Register default value:              0xFF1BFF00                            */
#define DEVHIDE_B3F6_PCU_FUN6_REG 0x040640B8
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 10;
    /* dis_dev1 - Bits[17:8], RO, default = 10'b1111111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 1;
    /* dis_dev2 - Bits[18:18], RW_LB, default = 1'b0 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 2;
    /* dis_dev3 - Bits[20:19], RO, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev4 : 3;
    /* dis_dev4 - Bits[23:21], RW_LB, default = 3'b000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 8;
    /* devunused - Bits[31:24], RO, default = 8'b11111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B3F6_PCU_FUN6_STRUCT;


/* DEVHIDE_B3F7_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60BC)                                                  */
/*       SKX (0x401F60BC)                                                     */
/* Register default value:              0xFF1BFF00                            */
#define DEVHIDE_B3F7_PCU_FUN6_REG 0x040640BC
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev1 : 10;
    /* dis_dev1 - Bits[17:8], RO, default = 10'b1111111111 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev2 : 1;
    /* dis_dev2 - Bits[18:18], RW_LB, default = 1'b0 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev3 : 2;
    /* dis_dev3 - Bits[20:19], RO, default = 2'b11 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 dis_dev4 : 3;
    /* dis_dev4 - Bits[23:21], RW_LB, default = 3'b000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 8;
    /* devunused - Bits[31:24], RO, default = 8'b11111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B3F7_PCU_FUN6_STRUCT;


/* DEVHIDE_B4F0_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60C0)                                                  */
/*       SKX (0x401F60C0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F0_PCU_FUN6_REG 0x040640C0
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B4F0_PCU_FUN6_STRUCT;


/* DEVHIDE_B4F1_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60C4)                                                  */
/*       SKX (0x401F60C4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F1_PCU_FUN6_REG 0x040640C4
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B4F1_PCU_FUN6_STRUCT;


/* DEVHIDE_B4F2_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60C8)                                                  */
/*       SKX (0x401F60C8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F2_PCU_FUN6_REG 0x040640C8
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B4F2_PCU_FUN6_STRUCT;


/* DEVHIDE_B4F3_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60CC)                                                  */
/*       SKX (0x401F60CC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F3_PCU_FUN6_REG 0x040640CC
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B4F3_PCU_FUN6_STRUCT;


/* DEVHIDE_B4F4_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60D0)                                                  */
/*       SKX (0x401F60D0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F4_PCU_FUN6_REG 0x040640D0
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B4F4_PCU_FUN6_STRUCT;


/* DEVHIDE_B4F5_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60D4)                                                  */
/*       SKX (0x401F60D4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F5_PCU_FUN6_REG 0x040640D4
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B4F5_PCU_FUN6_STRUCT;


/* DEVHIDE_B4F6_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60D8)                                                  */
/*       SKX (0x401F60D8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F6_PCU_FUN6_REG 0x040640D8
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B4F6_PCU_FUN6_STRUCT;


/* DEVHIDE_B4F7_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60DC)                                                  */
/*       SKX (0x401F60DC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F7_PCU_FUN6_REG 0x040640DC
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B4F7_PCU_FUN6_STRUCT;


/* DEVHIDE_B5F0_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60E0)                                                  */
/*       SKX (0x401F60E0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F0_PCU_FUN6_REG 0x040640E0
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B5F0_PCU_FUN6_STRUCT;


/* DEVHIDE_B5F1_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60E4)                                                  */
/*       SKX (0x401F60E4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F1_PCU_FUN6_REG 0x040640E4
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B5F1_PCU_FUN6_STRUCT;


/* DEVHIDE_B5F2_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60E8)                                                  */
/*       SKX (0x401F60E8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F2_PCU_FUN6_REG 0x040640E8
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B5F2_PCU_FUN6_STRUCT;


/* DEVHIDE_B5F3_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60EC)                                                  */
/*       SKX (0x401F60EC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F3_PCU_FUN6_REG 0x040640EC
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B5F3_PCU_FUN6_STRUCT;


/* DEVHIDE_B5F4_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60F0)                                                  */
/*       SKX (0x401F60F0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F4_PCU_FUN6_REG 0x040640F0
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B5F4_PCU_FUN6_STRUCT;


/* DEVHIDE_B5F5_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60F4)                                                  */
/*       SKX (0x401F60F4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F5_PCU_FUN6_REG 0x040640F4
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B5F5_PCU_FUN6_STRUCT;


/* DEVHIDE_B5F6_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60F8)                                                  */
/*       SKX (0x401F60F8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F6_PCU_FUN6_REG 0x040640F8
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B5F6_PCU_FUN6_STRUCT;


/* DEVHIDE_B5F7_PCU_FUN6_REG supported on:                                    */
/*       SKX_A0 (0x401F60FC)                                                  */
/*       SKX (0x401F60FC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F7_PCU_FUN6_REG 0x040640FC
/* Struct format extracted from XML file SKX\1.30.6.CFG.xml.
 * Device Hide Register in CSR space
 */
typedef union {
  struct {
    UINT32 dis_dev : 8;
    /* dis_dev - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Device(Dis_Dev):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDE4, then it means that in device 
       5, function 4 is disabled. 
     */
    UINT32 devunused : 24;
    /* devunused - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Device Unused: No device instantiated at this BDF so no Hide functionality 
       required. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE_B5F7_PCU_FUN6_STRUCT;


#endif /* PCU_FUN6_h */
