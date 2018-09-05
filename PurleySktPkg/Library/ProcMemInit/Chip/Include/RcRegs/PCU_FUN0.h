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

#ifndef PCU_FUN0_h
#define PCU_FUN0_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* PCU_FUN0_DEV 30                                                            */
/* PCU_FUN0_FUN 0                                                             */

/* VID_PCU_FUN0_REG supported on:                                             */
/*       SKX_A0 (0x201F0000)                                                  */
/*       SKX (0x201F0000)                                                     */
/* Register default value:              0x8086                                */
#define VID_PCU_FUN0_REG 0x04002000
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} VID_PCU_FUN0_STRUCT;


/* DID_PCU_FUN0_REG supported on:                                             */
/*       SKX_A0 (0x201F0002)                                                  */
/*       SKX (0x201F0002)                                                     */
/* Register default value:              0x2080                                */
#define DID_PCU_FUN0_REG 0x04002002
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2080 
        
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
} DID_PCU_FUN0_STRUCT;


/* PCICMD_PCU_FUN0_REG supported on:                                          */
/*       SKX_A0 (0x201F0004)                                                  */
/*       SKX (0x201F0004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_PCU_FUN0_REG 0x04002004
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} PCICMD_PCU_FUN0_STRUCT;


/* PCISTS_PCU_FUN0_REG supported on:                                          */
/*       SKX_A0 (0x201F0006)                                                  */
/*       SKX (0x201F0006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_PCU_FUN0_REG 0x04002006
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} PCISTS_PCU_FUN0_STRUCT;


/* RID_PCU_FUN0_REG supported on:                                             */
/*       SKX_A0 (0x101F0008)                                                  */
/*       SKX (0x101F0008)                                                     */
/* Register default value:              0x00                                  */
#define RID_PCU_FUN0_REG 0x04001008
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} RID_PCU_FUN0_STRUCT;


/* CCR_N0_PCU_FUN0_REG supported on:                                          */
/*       SKX_A0 (0x101F0009)                                                  */
/*       SKX (0x101F0009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_PCU_FUN0_REG 0x04001009
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_PCU_FUN0_STRUCT;


/* CCR_N1_PCU_FUN0_REG supported on:                                          */
/*       SKX_A0 (0x201F000A)                                                  */
/*       SKX (0x201F000A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_PCU_FUN0_REG 0x0400200A
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} CCR_N1_PCU_FUN0_STRUCT;


/* CLSR_PCU_FUN0_REG supported on:                                            */
/*       SKX_A0 (0x101F000C)                                                  */
/*       SKX (0x101F000C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_PCU_FUN0_REG 0x0400100C
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} CLSR_PCU_FUN0_STRUCT;


/* PLAT_PCU_FUN0_REG supported on:                                            */
/*       SKX_A0 (0x101F000D)                                                  */
/*       SKX (0x101F000D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_PCU_FUN0_REG 0x0400100D
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} PLAT_PCU_FUN0_STRUCT;


/* HDR_PCU_FUN0_REG supported on:                                             */
/*       SKX_A0 (0x101F000E)                                                  */
/*       SKX (0x101F000E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_PCU_FUN0_REG 0x0400100E
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} HDR_PCU_FUN0_STRUCT;


/* BIST_PCU_FUN0_REG supported on:                                            */
/*       SKX_A0 (0x101F000F)                                                  */
/*       SKX (0x101F000F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_PCU_FUN0_REG 0x0400100F
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} BIST_PCU_FUN0_STRUCT;


/* SVID_PCU_FUN0_REG supported on:                                            */
/*       SKX_A0 (0x201F002C)                                                  */
/*       SKX (0x201F002C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_PCU_FUN0_REG 0x0400202C
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} SVID_PCU_FUN0_STRUCT;


/* SDID_PCU_FUN0_REG supported on:                                            */
/*       SKX_A0 (0x201F002E)                                                  */
/*       SKX (0x201F002E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_PCU_FUN0_REG 0x0400202E
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} SDID_PCU_FUN0_STRUCT;


/* CAPPTR_PCU_FUN0_REG supported on:                                          */
/*       SKX_A0 (0x101F0034)                                                  */
/*       SKX (0x101F0034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_PCU_FUN0_REG 0x04001034
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} CAPPTR_PCU_FUN0_STRUCT;


/* INTL_PCU_FUN0_REG supported on:                                            */
/*       SKX_A0 (0x101F003C)                                                  */
/*       SKX (0x101F003C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_PCU_FUN0_REG 0x0400103C
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} INTL_PCU_FUN0_STRUCT;


/* INTPIN_PCU_FUN0_REG supported on:                                          */
/*       SKX_A0 (0x101F003D)                                                  */
/*       SKX (0x101F003D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_PCU_FUN0_REG 0x0400103D
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} INTPIN_PCU_FUN0_STRUCT;


/* MINGNT_PCU_FUN0_REG supported on:                                          */
/*       SKX_A0 (0x101F003E)                                                  */
/*       SKX (0x101F003E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_PCU_FUN0_REG 0x0400103E
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} MINGNT_PCU_FUN0_STRUCT;


/* MAXLAT_PCU_FUN0_REG supported on:                                          */
/*       SKX_A0 (0x101F003F)                                                  */
/*       SKX (0x101F003F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_PCU_FUN0_REG 0x0400103F
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
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
} MAXLAT_PCU_FUN0_STRUCT;


/* PRIP_NRG_STTS_PCU_FUN0_REG supported on:                                   */
/*       SKX_A0 (0x401F007C)                                                  */
/*       SKX (0x401F007C)                                                     */
/* Register default value:              0x00000000                            */
#define PRIP_NRG_STTS_PCU_FUN0_REG 0x0400407C
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Reports total energy consumed.  The counter will wrap around and continue 
 * counting when it reaches its limit.   
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} PRIP_NRG_STTS_PCU_FUN0_STRUCT;


/* PACKAGE_POWER_SKU_N0_PCU_FUN0_REG supported on:                            */
/*       SKX_A0 (0x401F0080)                                                  */
/*       SKX (0x401F0080)                                                     */
/* Register default value:              0x00600118                            */
#define PACKAGE_POWER_SKU_N0_PCU_FUN0_REG 0x04004080
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Defines allowed SKU power and timing parameters.
 */
typedef union {
  struct {
    UINT32 pkg_tdp : 15;
    /* pkg_tdp - Bits[14:0], RO_V, default = 15'b000000100011000 
       The TDP package power setting allowed for the SKU.  The TDP setting is typical 
       (not guaranteed). 
       The units for this value are defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT].
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pkg_min_pwr : 15;
    /* pkg_min_pwr - Bits[30:16], ROS_V, default = 15'b000000001100000 
       The minimal package power setting allowed for this part.  Lower values will be 
       clamped to this value.  The minimum setting is typical (not guaranteed). 
       The units for this value are defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT].
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PACKAGE_POWER_SKU_N0_PCU_FUN0_STRUCT;


/* PACKAGE_POWER_SKU_N1_PCU_FUN0_REG supported on:                            */
/*       SKX_A0 (0x401F0084)                                                  */
/*       SKX (0x401F0084)                                                     */
/* Register default value:              0x00120240                            */
#define PACKAGE_POWER_SKU_N1_PCU_FUN0_REG 0x04004084
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Defines allowed SKU power and timing parameters.
 */
typedef union {
  struct {
    UINT32 pkg_max_pwr : 15;
    /* pkg_max_pwr - Bits[14:0], ROS_V, default = 15'b000001001000000 
       The maximal package power setting allowed for the SKU.  Higher values will be 
       clamped to this value.  The maximum setting is typical (not guaranteed). 
       The units for this value are defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT].
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pkg_max_win : 7;
    /* pkg_max_win - Bits[22:16], ROS_V, default = 7'b0010010 
       The maximal time window allowed for the SKU.  Higher values will be clamped to 
       this value. 
       x = PKG_MAX_WIN[54:53]
       y = PKG_MAX_WIN[52:48]
       The timing interval window is Floating Point number given by 1.x * power(2,y).
       The unit of measurement is defined in PACKAGE_POWER_SKU_UNIT_MSR[TIME_UNIT].
     */
    UINT32 rsvd_23 : 9;
    /* rsvd_23 - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PACKAGE_POWER_SKU_N1_PCU_FUN0_STRUCT;


/* PKGC_LTR_LATENCY_MEASUREMENTS_PCU_FUN0_REG supported on:                   */
/*       SKX_A0 (0x401F0088)                                                  */
/*       SKX (0x401F0088)                                                     */
/* Register default value:              0x00000000                            */
#define PKGC_LTR_LATENCY_MEASUREMENTS_PCU_FUN0_REG 0x04004088
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * To measure memory transaction exit latencies, there are two hardware timers of 
 * 160 ns granularity. 
 *         These timers are in the unfiltered bclk domain, and one measures CLM 
 * exit latency and one DDR exit latency. 
 *         Pcode reads the timers, compares them to various thresholds, and if the 
 * timer value exceeds a threshold, pcode 
 *         sets the corresponding bit in this IO register.
 */
typedef union {
  struct {
    UINT32 ddr_access_time : 14;
    /* ddr_access_time - Bits[13:0], RO_V, default = 14'b0 
       DDR Access Time Value (in 160ns resolution)
     */
    UINT32 reserved : 1;
    /* reserved - Bits[14:14], RO_V, default = 1'b0 
       Reserved for future use.
     */
    UINT32 clm_access_time : 14;
    /* clm_access_time - Bits[28:15], RO_V, default = 14'b0 
       CLM Access Time Value (in 160ns resolution)
     */
    UINT32 non_snoop_threshold_resolution : 1;
    /* non_snoop_threshold_resolution - Bits[29:29], RO_V, default = 1'b0 
       Asserted if last wakeup time was greater than the non-snoop threshold for CLM 
       (when valid) and 
                 DDR.
     */
    UINT32 slow_snoop_threshold_resolution : 1;
    /* slow_snoop_threshold_resolution - Bits[30:30], RO_V, default = 1'b0 
       Asserted if last wakeup time was greater than the slow snoop threshold for CLM 
       (when valid) and 
                 DDR.
     */
    UINT32 fast_snoop_threshold_resolution : 1;
    /* fast_snoop_threshold_resolution - Bits[31:31], RO_V, default = 1'b0 
       Asserted if last wakeup time was greater than the fast snoop threshold for CLM 
       (when valid) and 
                 DDR.
     */
  } Bits;
  UINT32 Data;
} PKGC_LTR_LATENCY_MEASUREMENTS_PCU_FUN0_STRUCT;


/* PACKAGE_POWER_SKU_UNIT_PCU_FUN0_REG supported on:                          */
/*       SKX_A0 (0x401F008C)                                                  */
/*       SKX (0x401F008C)                                                     */
/* Register default value:              0x000A0E03                            */
#define PACKAGE_POWER_SKU_UNIT_PCU_FUN0_REG 0x0400408C
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Defines units for calculating SKU power and timing parameters.
 */
typedef union {
  struct {
    UINT32 pwr_unit : 4;
    /* pwr_unit - Bits[3:0], RO_V, default = 4'b0011 
       Power Units used for power control registers.
       The actual unit value is calculated by 1 W / Power(2,PWR_UNIT).
       The default value of 0011b corresponds to 1/8 W.
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 energy_unit : 5;
    /* energy_unit - Bits[12:8], RO_V, default = 5'h0e 
       Energy Units used for power control registers.
       The actual unit value is calculated by 1 J / Power(2,ENERGY_UNIT).
       The default value of 14 corresponds to Ux.14 number.
     */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 time_unit : 4;
    /* time_unit - Bits[19:16], RO_V, default = 4'b1010 
       Time Units used for power control registers.
       The actual unit value is calculated by 1 s / Power(2,TIME_UNIT).
       The default value of Ah corresponds to 976 usec.
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PACKAGE_POWER_SKU_UNIT_PCU_FUN0_STRUCT;


/* PACKAGE_ENERGY_STATUS_PCU_FUN0_REG supported on:                           */
/*       SKX_A0 (0x401F0090)                                                  */
/*       SKX (0x401F0090)                                                     */
/* Register default value:              0x00000000                            */
#define PACKAGE_ENERGY_STATUS_PCU_FUN0_REG 0x04004090
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Package energy consumed by the entire CPU (including IA, Uncore).  The counter 
 * will wrap around and continue counting when it reaches its limit.   
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} PACKAGE_ENERGY_STATUS_PCU_FUN0_STRUCT;


/* MEM_TRML_TEMPERATURE_REPORT_0_PCU_FUN0_REG supported on:                   */
/*       SKX_A0 (0x401F0094)                                                  */
/*       SKX (0x401F0094)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_TRML_TEMPERATURE_REPORT_0_PCU_FUN0_REG 0x04004094
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * This register is used to report the thermal status of the memory.   The channel 
 * max temperature field is used to report the maximal temperature of all ranks. 
 */
typedef union {
  struct {
    UINT32 channel0_max_temperature : 8;
    /* channel0_max_temperature - Bits[7:0], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 channel1_max_temperature : 8;
    /* channel1_max_temperature - Bits[15:8], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 channel2_max_temperature : 8;
    /* channel2_max_temperature - Bits[23:16], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} MEM_TRML_TEMPERATURE_REPORT_0_PCU_FUN0_STRUCT;


/* MEM_TRML_TEMPERATURE_REPORT_1_PCU_FUN0_REG supported on:                   */
/*       SKX_A0 (0x401F0098)                                                  */
/*       SKX (0x401F0098)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_TRML_TEMPERATURE_REPORT_1_PCU_FUN0_REG 0x04004098
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * This register is used to report the thermal status of the memory.   The channel 
 * max temperature field is used to report the maximal temperature of all ranks. 
 */
typedef union {
  struct {
    UINT32 channel0_max_temperature : 8;
    /* channel0_max_temperature - Bits[7:0], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 channel1_max_temperature : 8;
    /* channel1_max_temperature - Bits[15:8], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 channel2_max_temperature : 8;
    /* channel2_max_temperature - Bits[23:16], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} MEM_TRML_TEMPERATURE_REPORT_1_PCU_FUN0_STRUCT;


/* MEM_TRML_TEMPERATURE_REPORT_2_PCU_FUN0_REG supported on:                   */
/*       SKX_A0 (0x401F009C)                                                  */
/*       SKX (0x401F009C)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_TRML_TEMPERATURE_REPORT_2_PCU_FUN0_REG 0x0400409C
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * This register is used to report the thermal status of the memory.   The channel 
 * max temperature field is used to report the maximal temperature of all ranks. 
 */
typedef union {
  struct {
    UINT32 channel0_max_temperature : 8;
    /* channel0_max_temperature - Bits[7:0], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 channel1_max_temperature : 8;
    /* channel1_max_temperature - Bits[15:8], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 channel2_max_temperature : 8;
    /* channel2_max_temperature - Bits[23:16], RO_V, default = 8'b00000000 
       Temperature in Degrees (C).
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} MEM_TRML_TEMPERATURE_REPORT_2_PCU_FUN0_STRUCT;




/* PLATFORM_ID_N1_PCU_FUN0_REG supported on:                                  */
/*       SKX_A0 (0x401F00A4)                                                  */
/*       SKX (0x401F00A4)                                                     */
/* Register default value:              0x00000000                            */
#define PLATFORM_ID_N1_PCU_FUN0_REG 0x040040A4
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Indicates the platform that the processor is intended for.  NOTE: For the 
 * processor family this MSR is used for microcode update loading purpose only. For 
 * all other device identification purposes it is recommended that System BIOS use 
 * PCI based Device Identification register.  
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    /* rsvd_0 - Bits[17:0], n/a, default = n/a 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Padding added by header generation tool.
     */
    UINT32 platformid : 3;
    /* platformid - Bits[20:18], ROS_V, default = 3'b000 
       The field gives information concerning the intended platform for the processor. 
       Bits 52, 51, 50 are concatenated to form the platform ID as shown below: 
     */
    UINT32 rsvd_21 : 11;
    /* rsvd_21 - Bits[31:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PLATFORM_ID_N1_PCU_FUN0_STRUCT;


/* PLATFORM_INFO_N0_PCU_FUN0_REG supported on:                                */
/*       SKX_A0 (0x401F00A8)                                                  */
/*       SKX (0x401F00A8)                                                     */
/* Register default value:              0xF0010000                            */
#define PLATFORM_INFO_N0_PCU_FUN0_REG 0x040040A8
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Platform Info Register contains read-only package-level ratio information.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 8;
    /* rsvd_0 - Bits[7:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 max_non_turbo_lim_ratio : 8;
    /* max_non_turbo_lim_ratio - Bits[15:8], ROS_V, default = 8'b00000000 
       Contains the max non-turbo ratio.
       Contains the max non-turbo ratio. This is the ratio of the frequency that 
       invariant TSC runs at. Frequency = ratio * 100 MHz. 
       Note: The Maximum Non-Turbo Ratio is adjusted to the flexible limit ratio (as 
       specified in the FLEX_RATIO MSR 194h FLEX_RATIO field bits [15:8]) if flexible 
       limit is enabled (by setting the FLEX_RATIO MSR 194h FLEX_EN field bit[16]=1. 
       Note: In case of Configurable TDP feature, the maximum of the available TDP 
       levels is reported in this field. 
     */
    UINT32 smm_save_cap : 1;
    /* smm_save_cap - Bits[16:16], RO_V, default = 1'b1 
       When set to '1' indicates this feature exists and is configured by 
       SMM_SAVE_CONTROL 
     */
    UINT32 tapunlock : 1;
    /* tapunlock - Bits[17:17], RO_V, default = 1'b0 
       Current Tap Unlock Status
     */
    UINT32 rsvd_18 : 5;
    /* rsvd_18 - Bits[22:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ppin_cap : 1;
    /* ppin_cap - Bits[23:23], RO_V, default = 1'b0 
       When set to 1, indicates that this part supports the Protected Processor 
       Inventory Number (PPIN) feature. 
     */
    UINT32 ocvolt_ovrd_avail : 1;
    /* ocvolt_ovrd_avail - Bits[24:24], RO_V, default = 1'b0 
       0b  Indicates that the part does not support Voltage override overclocking.
                            1b  Indicates that the part supports Voltage override 
       overclocking. 
                            
     */
    UINT32 rsvd_25 : 2;
    UINT32 sample_part : 1;
    /* sample_part - Bits[27:27], ROS_V, default = 1'b0 
       Indicates if this part is a sample
     */
    UINT32 prg_turbo_ratio_en : 1;
    /* prg_turbo_ratio_en - Bits[28:28], RO_V, default = 1'b1 
       Programmable Turbo Ratios per number of Active Cores
     */
    UINT32 prg_tdp_lim_en : 1;
    /* prg_tdp_lim_en - Bits[29:29], ROS_V, default = 1'b1 
       Programmable TDP Limits for Turbo Mode.
       When set to 1, indicates that TDP Limits for Turbo mode are programmable, and 
       when set to 0, indicates TDP Limits for Turbo mode are not programmable. When 
       this bit is 0, an attempt to write to PP0_POWER_LIMIT, PP1_POWER_LIMIT and 
       PACKAGE_POWER_LIMIT MSR will result in a GP fault. 
     */
    UINT32 prg_tj_offset_en : 1;
    /* prg_tj_offset_en - Bits[30:30], ROS_V, default = 1'b1 
       Programmable TJ Offset Enable.
       When set to 1, indicates that the TCC Activation Offset field in 
       IA32_TEMPERATURE_TARGET MSR is valid and programmable. When set to 0, indicates 
       it's not programmable. When this bit is 0, an attempt to write to 
       MSR_TEMPERATURE_TARGET bits [27:24] will result in a GP fault. 
     */
    UINT32 cpuid_faulting_en : 1;
    /* cpuid_faulting_en - Bits[31:31], ROS_V, default = 1'b1 
       When set to 1, indicates that the processor supports raising a #GP if CPUID is 
       executed when not in SMM and the CPL > 0. When this bit is set, it indicates 
       that (140h) MISC_FEATURE_ENABLES bit 0 can be written by a VMM. 
     */
  } Bits;
  UINT32 Data;
} PLATFORM_INFO_N0_PCU_FUN0_STRUCT;


/* PLATFORM_INFO_N1_PCU_FUN0_REG supported on:                                */
/*       SKX_A0 (0x401F00AC)                                                  */
/*       SKX (0x401F00AC)                                                     */
/* Register default value:              0x00080000                            */
#define PLATFORM_INFO_N1_PCU_FUN0_REG 0x040040AC
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Platform Info Register contains read-only package-level ratio information.
 */
typedef union {
  struct {
    UINT32 lpm_support : 1;
    /* lpm_support - Bits[0:0], ROS_V, default = 1'b0 
       When set to 1, indicates that BIOS may program IA32_PERF_CTL to levels below the 
       max efficiency ratio downto Minimum Ratio bits [55:48]. 
       0 - Low Power mode not supported
       1 - Low Power mode supported
     */
    UINT32 config_tdp_levels : 2;
    /* config_tdp_levels - Bits[2:1], ROS_V, default = 2'b00 
       Indicates the number of configurable TDP levels supported.
       00 - Config TDP not supported
       01 - One additional TDP level supported
       10 - Two additional TDP levels supported
     */
    UINT32 pfat_enable : 1;
    /* pfat_enable - Bits[3:3], ROS_V, default = 1'b0 
       0 indicates that the Platform Firmware Armoring Technology (PFAT) feature is not 
       available, 1 indicates that the PFAT feature is available.  
     */
    UINT32 rsvd_4 : 1;
    /* rsvd_4 - Bits[4:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 timed_mwait_enable : 1;
    /* timed_mwait_enable - Bits[5:5], ROS_V, default = 1'b0 
       0 indicates that Timed MWAIT feature is not available, 1 indicates that Timed 
       MWAIT feature is available. 
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 max_efficiency_ratio : 8;
    /* max_efficiency_ratio - Bits[15:8], ROS_V, default = 8'b00000000 
       Contains the maximum efficiency ratio in units of 100 MHz. System BIOS must use 
       this as the minimum ratio in the _PSS table. 
     */
    UINT32 min_operating_ratio : 8;
    /* min_operating_ratio - Bits[23:16], RO_V, default = 8'b00001000 
       Indicates the minimum ratio supported by the processor (in units of 100MHz)
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PLATFORM_INFO_N1_PCU_FUN0_STRUCT;


/* TURBO_ACTIVATION_RATIO_PCU_FUN0_REG supported on:                          */
/*       SKX_A0 (0x401F00B0)                                                  */
/*       SKX (0x401F00B0)                                                     */
/* Register default value:              0x00000000                            */
#define TURBO_ACTIVATION_RATIO_PCU_FUN0_REG 0x040040B0
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Read/write register to allow MSR/MMIO access to ACPI P-state notify (PCS 33).
 */
typedef union {
  struct {
    UINT32 max_non_turbo_ratio : 8;
    /* max_non_turbo_ratio - Bits[7:0], RWS_L, default = 8'h00 
       CPU will treat any P-state request above this ratio as a request for max turbo 0 
       is special encoding which disables the feature. 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[30:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 turbo_activation_ratio_lock : 1;
    /* turbo_activation_ratio_lock - Bits[31:31], RWS_KL, default = 1'b0 
       Lock this MSR  until next reset
       0 - unlocked
       1 - locked
     */
  } Bits;
  UINT32 Data;
} TURBO_ACTIVATION_RATIO_PCU_FUN0_STRUCT;


/* PP0_ANY_THREAD_ACTIVITY_PCU_FUN0_REG supported on:                         */
/*       SKX_A0 (0x401F00B4)                                                  */
/*       SKX (0x401F00B4)                                                     */
/* Register default value:              0x00000000                            */
#define PP0_ANY_THREAD_ACTIVITY_PCU_FUN0_REG 0x040040B4
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * This register will count the BCLK cycles in which at least one of the IA cores 
 * was active. 
 * This is a 32 bit accumulation done by PCU HW.  Values exceeding 32b will wrap 
 * around. 
 * This value can be used in conjunction with PP0_EFFICIENT_CYCLES and 
 * PP0_THREAD_ACTIVITY to generate statistics for SW. 
 *       
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Number of Cycles
     */
  } Bits;
  UINT32 Data;
} PP0_ANY_THREAD_ACTIVITY_PCU_FUN0_STRUCT;


/* PP0_EFFICIENT_CYCLES_PCU_FUN0_REG supported on:                            */
/*       SKX_A0 (0x401F00B8)                                                  */
/*       SKX (0x401F00B8)                                                     */
/* Register default value:              0x00000000                            */
#define PP0_EFFICIENT_CYCLES_PCU_FUN0_REG 0x040040B8
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * This register will store a value equal to the product of the number of BCLK 
 * cycles in which at least one of the IA cores was active and the efficiency score 
 * calculated by the PCODE.  The efficiency score is a number between 0 and 1 that 
 * indicates the IA's efficiency. 
 * This is a 32 bit accumulation done by P-code to this register out of the 
 * PUSH-BUS.  Values exceeding 32b will wrap around. 
 * This value is used in conjunction with PP0_ANY_THREAD_ACTIVITY to generate 
 * statistics for SW. 
 *       
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Number of Cycles
     */
  } Bits;
  UINT32 Data;
} PP0_EFFICIENT_CYCLES_PCU_FUN0_STRUCT;


/* PP0_THREAD_ACTIVITY_PCU_FUN0_REG supported on:                             */
/*       SKX_A0 (0x401F00BC)                                                  */
/*       SKX (0x401F00BC)                                                     */
/* Register default value:              0x00000000                            */
#define PP0_THREAD_ACTIVITY_PCU_FUN0_REG 0x040040BC
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * This register will store a value equal to the product of the number of BCLK 
 * cycles and the number of IA threads that are running.  This is a 32 bit 
 * accumulation done by PCU HW.  Values exceeding 32b will wrap around. 
 * This value is used in conjunction with PP0_ANY_THREAD_ACTIVITY to generate 
 * statistics for SW. 
 * 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Number of Cycles
     */
  } Bits;
  UINT32 Data;
} PP0_THREAD_ACTIVITY_PCU_FUN0_STRUCT;


/* PACKAGE_TEMPERATURE_PCU_FUN0_REG supported on:                             */
/*       SKX_A0 (0x401F00C8)                                                  */
/*       SKX (0x401F00C8)                                                     */
/* Register default value:              0x00000000                            */
#define PACKAGE_TEMPERATURE_PCU_FUN0_REG 0x040040C8
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Package temperature in degrees (C).   This field is updated by FW.
 */
typedef union {
  struct {
    UINT32 data : 8;
    /* data - Bits[7:0], RO_V, default = 8'b00000000 
       Package temperature in degrees (C).
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PACKAGE_TEMPERATURE_PCU_FUN0_STRUCT;


/* PP0_TEMPERATURE_PCU_FUN0_REG supported on:                                 */
/*       SKX_A0 (0x401F00CC)                                                  */
/*       SKX (0x401F00CC)                                                     */
/* Register default value:              0x00000000                            */
#define PP0_TEMPERATURE_PCU_FUN0_REG 0x040040CC
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * PP0 temperature in degrees (C).   This field is updated by FW.
 */
typedef union {
  struct {
    UINT32 data : 8;
    /* data - Bits[7:0], RO_V, default = 8'b00000000 
       Temperature in degrees (C).
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PP0_TEMPERATURE_PCU_FUN0_STRUCT;


/* MRC_ODT_POWER_SAVING_PCU_FUN0_REG supported on:                            */
/*       SKX_A0 (0x401F00D0)                                                  */
/*       SKX (0x401F00D0)                                                     */
/* Register default value:              0x00000000                            */
#define MRC_ODT_POWER_SAVING_PCU_FUN0_REG 0x040040D0
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Each field represents a percentage ODT power saving for
 *         the DDR command type. The range is 0 to ~100% with an increment of 1/256 
 * ~= 0.391%. 
 */
typedef union {
  struct {
    UINT32 mrc_saving_rd : 8;
    /* mrc_saving_rd - Bits[7:0], RW_LB, default = 8'b0 
       Percentage ODT power saving for the DDR command. The range is 0 to ~100% with an 
       increment of 1/256 ~= 0.391%. 
     */
    UINT32 mrc_saving_wt : 8;
    /* mrc_saving_wt - Bits[15:8], RW_LB, default = 8'b0 
       Percentage ODT power saving for the DDR command. The range is 0 to ~100% with an 
       increment of 1/256 ~= 0.391%. 
     */
    UINT32 mrc_saving_cmd : 8;
    /* mrc_saving_cmd - Bits[23:16], RW_LB, default = 8'b0 
       Percentage ODT power saving for the DDR command. The range is 0 to ~100% with an 
       increment of 1/256 ~= 0.391%. 
     */
    UINT32 reserved : 8;
    /* reserved - Bits[31:24], RW_LB, default = 8'b0  */
  } Bits;
  UINT32 Data;
} MRC_ODT_POWER_SAVING_PCU_FUN0_STRUCT;




/* P_STATE_LIMITS_PCU_FUN0_REG supported on:                                  */
/*       SKX_A0 (0x401F00D8)                                                  */
/*       SKX (0x401F00D8)                                                     */
/* Register default value:              0x000000FF                            */
#define P_STATE_LIMITS_PCU_FUN0_REG 0x040040D8
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * This register allows SW to limit the maximum frequency allowed during run-time.  
 * PCODE will sample this register in slow loop. 
 */
typedef union {
  struct {
    UINT32 pstt_lim : 8;
    /* pstt_lim - Bits[7:0], RW_L, default = 8'b11111111 
       This field indicates the maximum IA frequency limit allowed during run-time.
     */
    UINT32 rsvd_8 : 8;
    UINT32 rsvd : 15;
    /* rsvd - Bits[30:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lock : 1;
    /* lock - Bits[31:31], RW_KL, default = 1'b0 
       This bit will lock all settings in this register.
     */
  } Bits;
  UINT32 Data;
} P_STATE_LIMITS_PCU_FUN0_STRUCT;


/* PACKAGE_THERM_MARGIN_PCU_FUN0_REG supported on:                            */
/*       SKX_A0 (0x401F00E0)                                                  */
/*       SKX (0x401F00E0)                                                     */
/* Register default value:              0x00000000                            */
#define PACKAGE_THERM_MARGIN_PCU_FUN0_REG 0x040040E0
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * DTS2.0 Thermal Margin
 */
typedef union {
  struct {
    UINT32 therm_margin : 16;
    /* therm_margin - Bits[15:0], RO_V, default = 16'b0000000000000000 
       Thermal margin in 8.8 format
     */
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} PACKAGE_THERM_MARGIN_PCU_FUN0_STRUCT;


/* TEMPERATURE_TARGET_PCU_FUN0_REG supported on:                              */
/*       SKX_A0 (0x401F00E4)                                                  */
/*       SKX (0x401F00E4)                                                     */
/* Register default value:              0x00000000                            */
#define TEMPERATURE_TARGET_PCU_FUN0_REG 0x040040E4
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Legacy register holding temperature related constants for Platform use.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 8;
    /* rsvd_0 - Bits[7:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 fan_temp_target_ofst : 8;
    /* fan_temp_target_ofst - Bits[15:8], RO_V, default = 8'b00000000 
       Fan Temperature Target Offset (a.k.a. T-Control) indicates the relative offset 
       from the Thermal Monitor Trip Temperature at which fans should be engaged.  
     */
    UINT32 ref_temp : 8;
    /* ref_temp - Bits[23:16], RO_V, default = 8'b00000000 
       This field indicates the maximum junction temperature, also referred to as the 
       Throttle 
                 Temperature, TCC Activation Temperature or Prochot Temperature. This 
       is the temperature at which the Adaptive Thermal Monitor is activated.  
     */
    UINT32 tj_max_tcc_offset : 4;
    /* tj_max_tcc_offset - Bits[27:24], RW, default = 4'b0000 
       Temperature offset in degrees (C) from the TJ Max. Used for throttling 
       temperature. Will not impact temperature reading. If offset is allowed and set - 
       the throttle will occur and reported at lower then Tj_max. 
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TEMPERATURE_TARGET_PCU_FUN0_STRUCT;


/* PACKAGE_RAPL_LIMIT_N0_PCU_FUN0_REG supported on:                           */
/*       SKX_A0 (0x401F00E8)                                                  */
/*       SKX (0x401F00E8)                                                     */
/* Register default value:              0x00000000                            */
#define PACKAGE_RAPL_LIMIT_N0_PCU_FUN0_REG 0x040040E8
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * The Integrated Graphics driver, CPM driver, BIOS and OS can balance the power 
 * budget between the Primary Power Plane (IA) and the Secondary Power Plane (GT) 
 * via PRIMARY_PLANE_TURBO_POWER_LIMIT_MSR and 
 * SECONDARY_PLANE_TURBO_POWER_LIMIT_MSR. 
 */
typedef union {
  struct {
    UINT32 pkg_pwr_lim_1 : 15;
    /* pkg_pwr_lim_1 - Bits[14:0], RW_L, default = 15'b000000000000000 
       This field indicates the power limitation #1.
       The unit of measurement is defined in PACKAGE_POWER_SKU_UNIT_MSR[PWR_UNIT].
     */
    UINT32 pkg_pwr_lim_1_en : 1;
    /* pkg_pwr_lim_1_en - Bits[15:15], RW_L, default = 1'b0 
       This bit enables/disables PKG_PWR_LIM_1.
       0b  Package Power Limit 1 is Disabled
       1b  Package Power Limit 1 is Enabled
     */
    UINT32 pkg_clmp_lim_1 : 1;
    /* pkg_clmp_lim_1 - Bits[16:16], RW_L, default = 1'b0 
       Package Clamping limitation #1 - Allow going below P1.
       0b     PBM is limited between P1 and P0.
       1b     PBM can go below P1.
     */
    UINT32 pkg_pwr_lim_1_time : 7;
    /* pkg_pwr_lim_1_time - Bits[23:17], RW_L, default = 7'b0000000 
       x = PKG_PWR_LIM_1_TIME[23:22]
       y = PKG_PWR_LIM_1_TIME[21:17]
       The timing interval window is Floating Point number given by 1.x * power(2,y).
       The unit of measurement is defined in PACKAGE_POWER_SKU_UNIT_MSR[TIME_UNIT].
       The maximal time window is bounded by PACKAGE_POWER_SKU_MSR[PKG_MAX_WIN].  The 
       minimum time window is 1 unit of measurement (as defined above). 
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PACKAGE_RAPL_LIMIT_N0_PCU_FUN0_STRUCT;


/* PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG supported on:                           */
/*       SKX_A0 (0x401F00EC)                                                  */
/*       SKX (0x401F00EC)                                                     */
/* Register default value:              0x00000000                            */
#define PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG 0x040040EC
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * The Integrated Graphics driver, CPM driver, BIOS and OS can balance the power 
 * budget between the Primary Power Plane (IA) and the Secondary Power Plane (GT) 
 * via PRIMARY_PLANE_TURBO_POWER_LIMIT_MSR and 
 * SECONDARY_PLANE_TURBO_POWER_LIMIT_MSR. 
 */
typedef union {
  struct {
    UINT32 pkg_pwr_lim_2 : 15;
    /* pkg_pwr_lim_2 - Bits[14:0], RW_L, default = 15'b000000000000000 
       This field indicates the power limitation #2.
       The unit of measurement is defined in PACKAGE_POWER_SKU_UNIT_MSR[PWR_UNIT].
     */
    UINT32 pkg_pwr_lim_2_en : 1;
    /* pkg_pwr_lim_2_en - Bits[15:15], RW_L, default = 1'b0 
       This bit enables/disables PKG_PWR_LIM_2.
       0b  Package Power Limit 2 is Disabled
       1b  Package Power Limit 2 is Enabled
     */
    UINT32 pkg_clmp_lim_2 : 1;
    /* pkg_clmp_lim_2 - Bits[16:16], RW_L, default = 1'b0 
       Package Clamping limitation #2 - Allow going below P1.
       0b     PBM is limited between P1 and P0.
       1b     PBM can go below P1.
     */
    UINT32 pkg_pwr_lim_2_time : 7;
    /* pkg_pwr_lim_2_time - Bits[23:17], RW_L, default = 7'b0000000 
       x = PKG_PWR_LIM_2_TIME[55:54]
       y = PKG_PWR_LIM_2_TIME[53:49]
       The timing interval window is Floating Point number given by 1.x * power(2,y).
       The unit of measurement is defined in PACKAGE_POWER_SKU_UNIT_MSR[TIME_UNIT].
       The maximal time window is bounded by PACKAGE_POWER_SKU_MSR[PKG_MAX_WIN].
       The minimum time window is 1 unit of measurement (as defined above).
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[30:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pkg_pwr_lim_lock : 1;
    /* pkg_pwr_lim_lock - Bits[31:31], RW_KL, default = 1'b0 
       When set, all settings in this register are locked and are treated as Read Only.
       This bit will typically set by BIOS during boot time or resume from Sx.
     */
  } Bits;
  UINT32 Data;
} PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_STRUCT;


/* PMAX_PCU_FUN0_REG supported on:                                            */
/*       SKX_A0 (0x401F00F0)                                                  */
/*       SKX (0x401F00F0)                                                     */
/* Register default value:              0x00000000                            */
#define PMAX_PCU_FUN0_REG 0x040040F0
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * BIOS supplied parameters for the on-chip Pmax detector circuit.  Must be locked 
 * by BIOS before RESET CPL4 
 */
typedef union {
  struct {
    UINT32 offset : 5;
    /* offset - Bits[4:0], RWS_L, default = 5'b0 
       Offset applied to reference value for Pmax trigger condition.  This will 
       increase or decrease the Pmax event threshold.  Encoding = 5bit, 2's complement. 
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[30:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lock : 1;
    /* lock - Bits[31:31], RWS_KL, default = 1'b0 
       When set, the contents of this register are locked and cannot be modified.  BIOS 
       is required to lock this register before RESET CPL4 
     */
  } Bits;
  UINT32 Data;
} PMAX_PCU_FUN0_STRUCT;


/* VR_CURRENT_CONFIG_N0_PCU_FUN0_REG supported on:                            */
/*       SKX_A0 (0x401F00F8)                                                  */
/*       SKX (0x401F00F8)                                                     */
/* Register default value:              0x00000500                            */
#define VR_CURRENT_CONFIG_N0_PCU_FUN0_REG 0x040040F8
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Limitation on the maximum current consumption of the primary power plane.
 */
typedef union {
  struct {
    UINT32 current_limit : 13;
    /* current_limit - Bits[12:0], RO_V, default = 13'b0010100000000 
       Current limitation in 0.125 A increments.  This field is locked by 
       VR_CURRENT_CONFIG[LOCK].  When the LOCK bit is set to 1b, this field becomes 
       Read Only. 
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[30:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lock : 1;
    /* lock - Bits[31:31], ROS_V, default = 1'b0 
       This bit will lock the CURRENT_LIMIT settings in this register and will also 
       lock this setting.  This means that once set to 1b, the CURRENT_LIMIT setting 
       and this bit become Read Only until the next Cold Reset. 
     */
  } Bits;
  UINT32 Data;
} VR_CURRENT_CONFIG_N0_PCU_FUN0_STRUCT;


/* VR_CURRENT_CONFIG_N1_PCU_FUN0_REG supported on:                            */
/*       SKX_A0 (0x401F00FC)                                                  */
/*       SKX (0x401F00FC)                                                     */
/* Register default value:              0x00000000                            */
#define VR_CURRENT_CONFIG_N1_PCU_FUN0_REG 0x040040FC
/* Struct format extracted from XML file SKX\1.30.0.CFG.xml.
 * Limitation on the maximum current consumption of the primary power plane.
 */
typedef union {
  struct {
    UINT32 psi1_threshold : 10;
    /* psi1_threshold - Bits[9:0], RO_V, default = 10'b0000000000 
       Maximum current in 1A units supported at external voltage regulator PS1
     */
    UINT32 psi2_threshold : 10;
    /* psi2_threshold - Bits[19:10], RO_V, default = 10'b0000000000 
       Maximum current in 1A units supported at external voltage regulator PS2
     */
    UINT32 psi3_threshold : 10;
    /* psi3_threshold - Bits[29:20], RO_V, default = 10'b0000000000 
       Maximum current in 1A units supported at external voltage regulator PS3
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} VR_CURRENT_CONFIG_N1_PCU_FUN0_STRUCT;


#endif /* PCU_FUN0_h */
