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

#ifndef CHABC_SAD1_h
#define CHABC_SAD1_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* CHABC_SAD1_DEV 29                                                          */
/* CHABC_SAD1_FUN 1                                                           */

/* VID_CHABC_SAD1_REG supported on:                                           */
/*       SKX_A0 (0x201E9000)                                                  */
/*       SKX (0x201E9000)                                                     */
/* Register default value:              0x8086                                */
#define VID_CHABC_SAD1_REG 0x03012000
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} VID_CHABC_SAD1_STRUCT;


/* DID_CHABC_SAD1_REG supported on:                                           */
/*       SKX_A0 (0x201E9002)                                                  */
/*       SKX (0x201E9002)                                                     */
/* Register default value:              0x2055                                */
#define DID_CHABC_SAD1_REG 0x03012002
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2055 
        
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
} DID_CHABC_SAD1_STRUCT;


/* PCICMD_CHABC_SAD1_REG supported on:                                        */
/*       SKX_A0 (0x201E9004)                                                  */
/*       SKX (0x201E9004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_CHABC_SAD1_REG 0x03012004
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} PCICMD_CHABC_SAD1_STRUCT;


/* PCISTS_CHABC_SAD1_REG supported on:                                        */
/*       SKX_A0 (0x201E9006)                                                  */
/*       SKX (0x201E9006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_CHABC_SAD1_REG 0x03012006
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} PCISTS_CHABC_SAD1_STRUCT;


/* RID_CHABC_SAD1_REG supported on:                                           */
/*       SKX_A0 (0x101E9008)                                                  */
/*       SKX (0x101E9008)                                                     */
/* Register default value:              0x00                                  */
#define RID_CHABC_SAD1_REG 0x03011008
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} RID_CHABC_SAD1_STRUCT;


/* CCR_N0_CHABC_SAD1_REG supported on:                                        */
/*       SKX_A0 (0x101E9009)                                                  */
/*       SKX (0x101E9009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_CHABC_SAD1_REG 0x03011009
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_CHABC_SAD1_STRUCT;


/* CCR_N1_CHABC_SAD1_REG supported on:                                        */
/*       SKX_A0 (0x201E900A)                                                  */
/*       SKX (0x201E900A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_CHABC_SAD1_REG 0x0301200A
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} CCR_N1_CHABC_SAD1_STRUCT;


/* CLSR_CHABC_SAD1_REG supported on:                                          */
/*       SKX_A0 (0x101E900C)                                                  */
/*       SKX (0x101E900C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_CHABC_SAD1_REG 0x0301100C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} CLSR_CHABC_SAD1_STRUCT;


/* PLAT_CHABC_SAD1_REG supported on:                                          */
/*       SKX_A0 (0x101E900D)                                                  */
/*       SKX (0x101E900D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_CHABC_SAD1_REG 0x0301100D
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} PLAT_CHABC_SAD1_STRUCT;


/* HDR_CHABC_SAD1_REG supported on:                                           */
/*       SKX_A0 (0x101E900E)                                                  */
/*       SKX (0x101E900E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_CHABC_SAD1_REG 0x0301100E
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} HDR_CHABC_SAD1_STRUCT;


/* BIST_CHABC_SAD1_REG supported on:                                          */
/*       SKX_A0 (0x101E900F)                                                  */
/*       SKX (0x101E900F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_CHABC_SAD1_REG 0x0301100F
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} BIST_CHABC_SAD1_STRUCT;


/* SVID_CHABC_SAD1_REG supported on:                                          */
/*       SKX_A0 (0x201E902C)                                                  */
/*       SKX (0x201E902C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_CHABC_SAD1_REG 0x0301202C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 * PCI Subsystem Vendor ID Register
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_CHABC_SAD1_STRUCT;


/* SDID_CHABC_SAD1_REG supported on:                                          */
/*       SKX_A0 (0x201E902E)                                                  */
/*       SKX (0x201E902E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_CHABC_SAD1_REG 0x0301202E
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 * PCI Subsystem device ID Register
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_CHABC_SAD1_STRUCT;


/* CAPPTR_CHABC_SAD1_REG supported on:                                        */
/*       SKX_A0 (0x101E9034)                                                  */
/*       SKX (0x101E9034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_CHABC_SAD1_REG 0x03011034
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} CAPPTR_CHABC_SAD1_STRUCT;


/* INTL_CHABC_SAD1_REG supported on:                                          */
/*       SKX_A0 (0x101E903C)                                                  */
/*       SKX (0x101E903C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_CHABC_SAD1_REG 0x0301103C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} INTL_CHABC_SAD1_STRUCT;


/* INTPIN_CHABC_SAD1_REG supported on:                                        */
/*       SKX_A0 (0x101E903D)                                                  */
/*       SKX (0x101E903D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_CHABC_SAD1_REG 0x0301103D
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} INTPIN_CHABC_SAD1_STRUCT;


/* MINGNT_CHABC_SAD1_REG supported on:                                        */
/*       SKX_A0 (0x101E903E)                                                  */
/*       SKX (0x101E903E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_CHABC_SAD1_REG 0x0301103E
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} MINGNT_CHABC_SAD1_STRUCT;


/* MAXLAT_CHABC_SAD1_REG supported on:                                        */
/*       SKX_A0 (0x101E903F)                                                  */
/*       SKX (0x101E903F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_CHABC_SAD1_REG 0x0301103F
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
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
} MAXLAT_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_0_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9040)                                                  */
/*       SKX (0x401E9040)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_0_N0_CHABC_SAD1_REG 0x03014040
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_0_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_0_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9044)                                                  */
/*       SKX (0x401E9044)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_0_N1_CHABC_SAD1_REG 0x03014044
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_0_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_1_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9048)                                                  */
/*       SKX (0x401E9048)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_1_N0_CHABC_SAD1_REG 0x03014048
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_1_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_1_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E904C)                                                  */
/*       SKX (0x401E904C)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_1_N1_CHABC_SAD1_REG 0x0301404C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_1_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_2_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9050)                                                  */
/*       SKX (0x401E9050)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_2_N0_CHABC_SAD1_REG 0x03014050
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_2_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_2_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9054)                                                  */
/*       SKX (0x401E9054)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_2_N1_CHABC_SAD1_REG 0x03014054
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_2_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_3_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9058)                                                  */
/*       SKX (0x401E9058)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_3_N0_CHABC_SAD1_REG 0x03014058
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_3_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_3_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E905C)                                                  */
/*       SKX (0x401E905C)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_3_N1_CHABC_SAD1_REG 0x0301405C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_3_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_4_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9060)                                                  */
/*       SKX (0x401E9060)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_4_N0_CHABC_SAD1_REG 0x03014060
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_4_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_4_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9064)                                                  */
/*       SKX (0x401E9064)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_4_N1_CHABC_SAD1_REG 0x03014064
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_4_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_5_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9068)                                                  */
/*       SKX (0x401E9068)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_5_N0_CHABC_SAD1_REG 0x03014068
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_5_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_5_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E906C)                                                  */
/*       SKX (0x401E906C)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_5_N1_CHABC_SAD1_REG 0x0301406C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_5_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_6_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9070)                                                  */
/*       SKX (0x401E9070)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_6_N0_CHABC_SAD1_REG 0x03014070
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_6_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_6_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9074)                                                  */
/*       SKX (0x401E9074)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_6_N1_CHABC_SAD1_REG 0x03014074
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_6_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_7_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9078)                                                  */
/*       SKX (0x401E9078)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_7_N0_CHABC_SAD1_REG 0x03014078
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_7_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_7_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E907C)                                                  */
/*       SKX (0x401E907C)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_7_N1_CHABC_SAD1_REG 0x0301407C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_7_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_8_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9080)                                                  */
/*       SKX (0x401E9080)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_8_N0_CHABC_SAD1_REG 0x03014080
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_8_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_8_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9084)                                                  */
/*       SKX (0x401E9084)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_8_N1_CHABC_SAD1_REG 0x03014084
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_8_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_9_N0_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E9088)                                                  */
/*       SKX (0x401E9088)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_9_N0_CHABC_SAD1_REG 0x03014088
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_9_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_9_N1_CHABC_SAD1_REG supported on:                            */
/*       SKX_A0 (0x401E908C)                                                  */
/*       SKX (0x401E908C)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_9_N1_CHABC_SAD1_REG 0x0301408C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_9_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_10_N0_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E9090)                                                  */
/*       SKX (0x401E9090)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_10_N0_CHABC_SAD1_REG 0x03014090
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_10_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_10_N1_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E9094)                                                  */
/*       SKX (0x401E9094)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_10_N1_CHABC_SAD1_REG 0x03014094
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_10_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_11_N0_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E9098)                                                  */
/*       SKX (0x401E9098)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_11_N0_CHABC_SAD1_REG 0x03014098
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_11_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_11_N1_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E909C)                                                  */
/*       SKX (0x401E909C)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_11_N1_CHABC_SAD1_REG 0x0301409C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_11_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_12_N0_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E90A0)                                                  */
/*       SKX (0x401E90A0)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_12_N0_CHABC_SAD1_REG 0x030140A0
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_12_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_12_N1_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E90A4)                                                  */
/*       SKX (0x401E90A4)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_12_N1_CHABC_SAD1_REG 0x030140A4
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_12_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_13_N0_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E90A8)                                                  */
/*       SKX (0x401E90A8)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_13_N0_CHABC_SAD1_REG 0x030140A8
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_13_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_13_N1_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E90AC)                                                  */
/*       SKX (0x401E90AC)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_13_N1_CHABC_SAD1_REG 0x030140AC
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_13_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_14_N0_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E90B0)                                                  */
/*       SKX (0x401E90B0)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_14_N0_CHABC_SAD1_REG 0x030140B0
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_14_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_14_N1_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E90B4)                                                  */
/*       SKX (0x401E90B4)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_14_N1_CHABC_SAD1_REG 0x030140B4
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_14_N1_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_15_N0_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E90B8)                                                  */
/*       SKX (0x401E90B8)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_15_N0_CHABC_SAD1_REG 0x030140B8
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_15_N0_CHABC_SAD1_STRUCT;


/* MMIO_RULE_CFG_15_N1_CHABC_SAD1_REG supported on:                           */
/*       SKX_A0 (0x401E90BC)                                                  */
/*       SKX (0x401E90BC)                                                     */
/* Register default value:              0x00000000                            */
#define MMIO_RULE_CFG_15_N1_CHABC_SAD1_REG 0x030140BC
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 baseaddress_vlsb_bits : 2;
    /* baseaddress_vlsb_bits - Bits[15:14], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 limitaddress_vlsb_bits : 2;
    /* limitaddress_vlsb_bits - Bits[17:16], RW_LB, default = 2'b00 
       This correspond to Addr[23:22] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIO_RULE_CFG_15_N1_CHABC_SAD1_STRUCT;


/* MMCFG_RULE_N0_CHABC_SAD1_REG supported on:                                 */
/*       SKX_A0 (0x401E90C0)                                                  */
/*       SKX (0x401E90C0)                                                     */
/* Register default value:              0x00003FC8                            */
#define MMCFG_RULE_N0_CHABC_SAD1_REG 0x030140C0
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMCFG interleave decoder.
     */
    UINT32 length : 3;
    /* length - Bits[3:1], RW_LB, default = 3'b100 
       This field document the max bus number supported by the interleave decoder.
       The following configurations are supported:
           3'b110 : MaxBusNumber = 63 (ie. 64MB MMCFG range)
           3'b101 : MaxBusNumber = 127 (ie 128MB MMCFG range)
           3'b100 : MaxBusNumber = 256 (ie 256MB MMCFG range)
           3'b010 : MaxBusNumber = 512 (ie 512MB MMCFG range)
           3'b001 : MaxBusNumber = 1048 (ie 1G MMCFG range)
           3'b000 : MaxBusNumber = 2048 (ie 2G MMCFG range)
       Minimum granularity of MMCFG range will be 64MB.
     */
    UINT32 rsvd_4 : 2;
    /* rsvd_4 - Bits[5:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 valid_targets : 8;
    /* valid_targets - Bits[13:6], RW_LB, default = 8'b11111111 
       Marks the valid targets. Request to an invalid target will result in an ABORT
     */
    UINT32 rsvd_14 : 12;
    /* rsvd_14 - Bits[25:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 base_address : 6;
    /* base_address - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMCFG rule base address. The granularity 
       of MMCFG rule is 64MB. Note that this interleave decoder can be used for higher 
       segments of the MMCFG and is not restricted to Segment 0 of MMCFG. 
       Check MMCFG_TargetList for Interleaved target list used by this decoder.
     */
  } Bits;
  UINT32 Data;
} MMCFG_RULE_N0_CHABC_SAD1_STRUCT;


/* MMCFG_RULE_N1_CHABC_SAD1_REG supported on:                                 */
/*       SKX_A0 (0x401E90C4)                                                  */
/*       SKX (0x401E90C4)                                                     */
/* Register default value:              0x00000000                            */
#define MMCFG_RULE_N1_CHABC_SAD1_REG 0x030140C4
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 base_address : 14;
    /* base_address - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMCFG rule base address. The granularity 
       of MMCFG rule is 64MB. Note that this interleave decoder can be used for higher 
       segments of the MMCFG and is not restricted to Segment 0 of MMCFG. 
       Check MMCFG_TargetList for Interleaved target list used by this decoder.
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMCFG_RULE_N1_CHABC_SAD1_STRUCT;


/* MMCFG_LOCAL_RULE_ADDRESS_CFG_0_CHABC_SAD1_REG supported on:                */
/*       SKX_A0 (0x401E90C8)                                                  */
/*       SKX (0x401E90C8)                                                     */
/* Register default value:              0x00000000                            */
#define MMCFG_LOCAL_RULE_ADDRESS_CFG_0_CHABC_SAD1_REG 0x030140C8
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limit0 : 8;
    /* limit0 - Bits[7:0], RW_LB, default = 8'b00000000 
       Maps out the bus base/limits for the locally mapped MMCFG addresses.  Any 
       locally mapped MMCFG 
       which has a bus_id greater than the last limit bus, will be sent to the ubox.
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_0.Limit0  base root bus for IIO Stack 0
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_0.Limit1  limit bus for IIO Stack 0
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_0.Limit2  limit bus for IIO Stack 1
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_0.Limit3  limit bus for IIO Stack 2
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_1.Limit0  limit bus for IIO Stack 3
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_1.Limit1  limit bus for IIO Stack 4
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_1.Limit2  limit bus for IIO Stack 5
     */
    UINT32 limit1 : 8;
    /* limit1 - Bits[15:8], RW_LB, default = 8'b00000000  */
    UINT32 limit2 : 8;
    /* limit2 - Bits[23:16], RW_LB, default = 8'b00000000  */
    UINT32 limit3 : 8;
    /* limit3 - Bits[31:24], RW_LB, default = 8'b00000000  */
  } Bits;
  UINT32 Data;
} MMCFG_LOCAL_RULE_ADDRESS_CFG_0_CHABC_SAD1_STRUCT;


/* MMCFG_LOCAL_RULE_ADDRESS_CFG_1_CHABC_SAD1_REG supported on:                */
/*       SKX_A0 (0x401E90CC)                                                  */
/*       SKX (0x401E90CC)                                                     */
/* Register default value:              0x00000000                            */
#define MMCFG_LOCAL_RULE_ADDRESS_CFG_1_CHABC_SAD1_REG 0x030140CC
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limit0 : 8;
    /* limit0 - Bits[7:0], RW_LB, default = 8'b00000000 
       Maps out the bus base/limits for the locally mapped MMCFG addresses.  Any 
       locally mapped MMCFG 
       which has a bus_id greater than the last limit bus, will be sent to the ubox.
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_0.Limit0  base root bus for IIO Stack 0
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_0.Limit1  limit bus for IIO Stack 0
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_0.Limit2  limit bus for IIO Stack 1
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_0.Limit3  limit bus for IIO Stack 2
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_1.Limit0  limit bus for IIO Stack 3
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_1.Limit1  limit bus for IIO Stack 4
       		  MMCFG_LOCAL_RULE_ADDRESS_CFG_1.Limit2  limit bus for IIO Stack 5
     */
    UINT32 limit1 : 8;
    /* limit1 - Bits[15:8], RW_LB, default = 8'b00000000  */
    UINT32 limit2 : 8;
    /* limit2 - Bits[23:16], RW_LB, default = 8'b00000000  */
    UINT32 limit3 : 8;
    /* limit3 - Bits[31:24], RW_LB, default = 8'b00000000  */
  } Bits;
  UINT32 Data;
} MMCFG_LOCAL_RULE_ADDRESS_CFG_1_CHABC_SAD1_STRUCT;


/* LT_CONTROL_CHABC_SAD1_REG supported on:                                    */
/*       SKX_A0 (0x401E90D0)                                                  */
/*       SKX (0x401E90D0)                                                     */
/* Register default value:              0x00000008                            */
#define LT_CONTROL_CHABC_SAD1_REG 0x030140D0
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 lt_target : 4;
    /* lt_target - Bits[3:0], RW_L, default = 4'b1000 
       Target NodeID of the Intel TXT Target
       		  b'1xxx  Target is local IIO Stack (target is always the legacy stack)
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 lt_target_enable : 1;
    /* lt_target_enable - Bits[4:4], RW1S, default = 1'b0 
       when '1 is written, bit[3:0] below becomes Read-only.
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LT_CONTROL_CHABC_SAD1_STRUCT;


/* IOAPIC_TARGET_LIST_CFG_0_CHABC_SAD1_REG supported on:                      */
/*       SKX_A0 (0x401E90D4)                                                  */
/*       SKX (0x401E90D4)                                                     */
/* Register default value:              0x88888888                            */
#define IOAPIC_TARGET_LIST_CFG_0_CHABC_SAD1_REG 0x030140D4
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} IOAPIC_TARGET_LIST_CFG_0_CHABC_SAD1_STRUCT;


/* IOAPIC_TARGET_LIST_CFG_1_CHABC_SAD1_REG supported on:                      */
/*       SKX_A0 (0x401E90D8)                                                  */
/*       SKX (0x401E90D8)                                                     */
/* Register default value:              0x88888888                            */
#define IOAPIC_TARGET_LIST_CFG_1_CHABC_SAD1_REG 0x030140D8
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} IOAPIC_TARGET_LIST_CFG_1_CHABC_SAD1_STRUCT;


/* IOAPIC_TARGET_LIST_CFG_2_CHABC_SAD1_REG supported on:                      */
/*       SKX_A0 (0x401E90DC)                                                  */
/*       SKX (0x401E90DC)                                                     */
/* Register default value:              0x88888888                            */
#define IOAPIC_TARGET_LIST_CFG_2_CHABC_SAD1_REG 0x030140DC
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} IOAPIC_TARGET_LIST_CFG_2_CHABC_SAD1_STRUCT;


/* IOAPIC_TARGET_LIST_CFG_3_CHABC_SAD1_REG supported on:                      */
/*       SKX_A0 (0x401E90E0)                                                  */
/*       SKX (0x401E90E0)                                                     */
/* Register default value:              0x88888888                            */
#define IOAPIC_TARGET_LIST_CFG_3_CHABC_SAD1_REG 0x030140E0
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the IOAPIC target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} IOAPIC_TARGET_LIST_CFG_3_CHABC_SAD1_STRUCT;


/* MMCFG_LOCAL_RULE_CHABC_SAD1_REG supported on:                              */
/*       SKX_A0 (0x401E90E4)                                                  */
/*       SKX (0x401E90E4)                                                     */
/* Register default value:              0x00000000                            */
#define MMCFG_LOCAL_RULE_CHABC_SAD1_REG 0x030140E4
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 target0 : 4;
    /* target0 - Bits[3:0], RW_LB, default = 4'b0000 
       Target for the local bus decode of MMCFG.  Target physical stack ID for logical 
       IIO Stack 0. 
     */
    UINT32 target1 : 4;
    /* target1 - Bits[7:4], RW_LB, default = 4'b0000 
       Target for the local bus decode of MMCFG.  Target physical stack ID for logical 
       IIO Stack 1. 
     */
    UINT32 target2 : 4;
    /* target2 - Bits[11:8], RW_LB, default = 4'b0000 
       Target for the local bus decode of MMCFG.  Target physical stack ID for logical 
       IIO Stack 2. 
     */
    UINT32 target3 : 4;
    /* target3 - Bits[15:12], RW_LB, default = 4'b0000 
       Target for the local bus decode of MMCFG.  Target physical stack ID for logical 
       IIO Stack 3. 
     */
    UINT32 target4 : 4;
    /* target4 - Bits[19:16], RW_LB, default = 4'b0000 
       Target for the local bus decode of MMCFG.  Target physical stack ID for logical 
       IIO Stack 4. 
     */
    UINT32 target5 : 4;
    /* target5 - Bits[23:20], RW_LB, default = 4'b0000 
       Target for the local bus decode of MMCFG.  Target physical stack ID for logical 
       IIO Stack 5. 
     */
    UINT32 mode0 : 1;
    /* mode0 - Bits[24:24], RW_LB, default = 1'b0 
       Mode bit for managing the root bus for IIO Stack 0
       		  b'1    send all requests which match the root bus for this stack to the ubox
           b'0    send only requests which match the root bus with device ID less than 
       16 to the ubox 
     */
    UINT32 mode1 : 1;
    /* mode1 - Bits[25:25], RW_LB, default = 1'b0 
       Mode bit for managing the root bus for IIO Stack 1
       		  b'1    send all requests which match the root bus for this stack to the ubox
           b'0    send only requests which match the root bus with device ID less than 
       16 to the ubox 
     */
    UINT32 mode2 : 1;
    /* mode2 - Bits[26:26], RW_LB, default = 1'b0 
       Mode bit for managing the root bus for IIO Stack 2
       		  b'1    send all requests which match the root bus for this stack to the ubox
           b'0    send only requests which match the root bus with device ID less than 
       16 to the ubox 
     */
    UINT32 mode3 : 1;
    /* mode3 - Bits[27:27], RW_LB, default = 1'b0 
       Mode bit for managing the root bus for IIO Stack 3
       		  b'1    send all requests which match the root bus for this stack to the ubox
           b'0    send only requests which match the root bus with device ID less than 
       16 to the ubox 
     */
    UINT32 mode4 : 1;
    /* mode4 - Bits[28:28], RW_LB, default = 1'b0 
       Mode bit for managing the root bus for IIO Stack 4
       		  b'1    send all requests which match the root bus for this stack to the ubox
           b'0    send only requests which match the root bus with device ID less than 
       16 to the ubox 
     */
    UINT32 mode5 : 1;
    /* mode5 - Bits[29:29], RW_LB, default = 1'b0 
       Mode bit for managing the root bus for IIO Stack 5
       		  b'1    send all requests which match the root bus for this stack to the ubox
           b'0    send only requests which match the root bus with device ID less than 
       16 to the ubox 
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMCFG_LOCAL_RULE_CHABC_SAD1_STRUCT;


/* MMIO_TARGET_LIST_CFG_0_CHABC_SAD1_REG supported on:                        */
/*       SKX_A0 (0x401E90E8)                                                  */
/*       SKX (0x401E90E8)                                                     */
/* Register default value:              0x88888888                            */
#define MMIO_TARGET_LIST_CFG_0_CHABC_SAD1_REG 0x030140E8
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} MMIO_TARGET_LIST_CFG_0_CHABC_SAD1_STRUCT;


/* MMCFG_TARGET_LIST_CHABC_SAD1_REG supported on:                             */
/*       SKX_A0 (0x401E90EC)                                                  */
/*       SKX (0x401E90EC)                                                     */
/* Register default value:              0x88888888                            */
#define MMCFG_TARGET_LIST_CHABC_SAD1_REG 0x030140EC
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       MMCFG target.
           b'1xxx  Target is a local IIO Stack.  Actual stack ID target will be 
       determined by MMCFG_Local_Rule_CFG 
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       MMCFG target.
           b'1xxx  Target is a local IIO Stack.  Actual stack ID target will be 
       determined by MMCFG_Local_Rule_CFG 
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       MMCFG target.
           b'1xxx  Target is a local IIO Stack.  Actual stack ID target will be 
       determined by MMCFG_Local_Rule_CFG 
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       MMCFG target.
           b'1xxx  Target is a local IIO Stack.  Actual stack ID target will be 
       determined by MMCFG_Local_Rule_CFG 
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       MMCFG target.
           b'1xxx  Target is a local IIO Stack.  Actual stack ID target will be 
       determined by MMCFG_Local_Rule_CFG 
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       MMCFG target.
           b'1xxx  Target is a local IIO Stack.  Actual stack ID target will be 
       determined by MMCFG_Local_Rule_CFG 
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       MMCFG target.
           b'1xxx  Target is a local IIO Stack.  Actual stack ID target will be 
       determined by MMCFG_Local_Rule_CFG 
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       MMCFG target.
           b'1xxx  Target is a local IIO Stack.  Actual stack ID target will be 
       determined by MMCFG_Local_Rule_CFG 
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} MMCFG_TARGET_LIST_CHABC_SAD1_STRUCT;


/* SAD_TARGET_CHABC_SAD1_REG supported on:                                    */
/*       SKX_A0 (0x401E90F0)                                                  */
/*       SKX (0x401E90F0)                                                     */
/* Register default value:              0x00000808                            */
#define SAD_TARGET_CHABC_SAD1_REG 0x030140F0
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 flash_target : 4;
    /* flash_target - Bits[3:0], RW_LB, default = 4'b1000 
       Target NodeID of the Flash Target
       		  b'1---  Target is the local socket.  Target is always the legacy stack.
           b'0xxx  Target is remote socket with NodeID xxx.  Target will be the legacy 
       stack on that socket. 
     */
    UINT32 legacy_pch_target : 4;
    /* legacy_pch_target - Bits[7:4], RW_LB, default = 4'b0000 
       Target NodeID of the Legacy PCH Target
           b'1xxx Target is the local socket. Target is the local stack with stack id 
       xxx. 
       		  b'0xxx Target is remote socket with NodeID xxx.
     */
    UINT32 vga_target : 4;
    /* vga_target - Bits[11:8], RW_LB, default = 4'b1000 
       Target NodeID of the VGA Target
       		  b'1---  Target is the local socket.  Target is always the legacy stack.
           b'0xxx  Target is remote socket with NodeID xxx.  Target will be the legacy 
       stack on that socket. 
     */
    UINT32 sourceid : 3;
    /* sourceid - Bits[14:12], RW_LB, default = 3'b000 
       SourceID of the Socket. Programmable by BIOS. By default, the value should be 
       part of the APICID that represent the socket. 
     */
    UINT32 sourceiden : 1;
    /* sourceiden - Bits[15:15], RW_LB, default = 1'b0 
       Enable SourceID feature.
     */
    UINT32 enablemodulo3basedktilink : 1;
    /* enablemodulo3basedktilink - Bits[16:16], RW_LB, default = 1'b0 
       Use Mod3(PA) value to decide the Intel UPI link to use in 2S-3K case
     */
    UINT32 rsvd : 15;
    /* rsvd - Bits[31:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SAD_TARGET_CHABC_SAD1_STRUCT;


/* SAD_CONTROL_CHABC_SAD1_REG supported on:                                   */
/*       SKX_A0 (0x401E90F4)                                                  */
/*       SKX (0x401E90F4)                                                     */
/* Register default value:              0x00000000                            */
#define SAD_CONTROL_CHABC_SAD1_REG 0x030140F4
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 local_nodeid : 3;
    /* local_nodeid - Bits[2:0], RW_L, default = 4'b0000 
       NodeID of the local Socket. This register can not be overwritten (Read only) if 
       LT_Target_Enable ==1 (Under LT_Control). Same as LT_Target, the value can't 
       change to prevent any attack to Intel TXT. 
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SAD_CONTROL_CHABC_SAD1_STRUCT;


/* MMIO_TARGET_LIST_CFG_1_CHABC_SAD1_REG supported on:                        */
/*       SKX_A0 (0x401E90F8)                                                  */
/*       SKX (0x401E90F8)                                                     */
/* Register default value:              0x88888888                            */
#define MMIO_TARGET_LIST_CFG_1_CHABC_SAD1_REG 0x030140F8
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} MMIO_TARGET_LIST_CFG_1_CHABC_SAD1_STRUCT;


/* MMIOH_INTERLEAVE_RULE_N0_CHABC_SAD1_REG supported on:                      */
/*       SKX_A0 (0x401E9100)                                                  */
/*       SKX (0x401E9100)                                                     */
/* Register default value:              0x00000000                            */
#define MMIOH_INTERLEAVE_RULE_N0_CHABC_SAD1_REG 0x03014100
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIOH_INTERLEAVE_RULE_N0_CHABC_SAD1_STRUCT;


/* MMIOH_INTERLEAVE_RULE_N1_CHABC_SAD1_REG supported on:                      */
/*       SKX_A0 (0x401E9104)                                                  */
/*       SKX (0x401E9104)                                                     */
/* Register default value:              0x00000000                            */
#define MMIOH_INTERLEAVE_RULE_N1_CHABC_SAD1_REG 0x03014104
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 interleave_mode : 3;
    /* interleave_mode - Bits[16:14], RW_LB, default = 3'b000 
       Specifies the interleave granularity of the MMIOH interleave decoder.  Encoded 
       as follows: 
       		  b'000     1G
       		  b'001     4G
       		  b'010    16G
       		  b'011    64G
       		  b'100   256G
       		  b'101  1024G
     */
    UINT32 rsvd : 15;
    /* rsvd - Bits[31:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIOH_INTERLEAVE_RULE_N1_CHABC_SAD1_STRUCT;


/* MMIOH_NONINTERLEAVE_RULE_N0_CHABC_SAD1_REG supported on:                   */
/*       SKX_A0 (0x401E9108)                                                  */
/*       SKX (0x401E9108)                                                     */
/* Register default value:              0x00000000                            */
#define MMIOH_NONINTERLEAVE_RULE_N0_CHABC_SAD1_REG 0x03014108
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMIO rule.
     */
    UINT32 baseaddress : 20;
    /* baseaddress - Bits[20:1], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 64MB. 
     */
    UINT32 baseaddress_lsb_bits : 2;
    /* baseaddress_lsb_bits - Bits[22:21], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule base address. Both base and 
       limit must match to declare a match to this MMIO rule. The granularity of MMIO 
       rule is 16MB. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 limitaddress_lsb_bits : 2;
    /* limitaddress_lsb_bits - Bits[25:24], RW_LB, default = 2'b00 
       This correspond to Addr[25:24] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 limitaddress : 6;
    /* limitaddress - Bits[31:26], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
  } Bits;
  UINT32 Data;
} MMIOH_NONINTERLEAVE_RULE_N0_CHABC_SAD1_STRUCT;


/* MMIOH_NONINTERLEAVE_RULE_N1_CHABC_SAD1_REG supported on:                   */
/*       SKX_A0 (0x401E910C)                                                  */
/*       SKX (0x401E910C)                                                     */
/* Register default value:              0x00800000                            */
#define MMIOH_NONINTERLEAVE_RULE_N1_CHABC_SAD1_REG 0x0301410C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 limitaddress : 14;
    /* limitaddress - Bits[13:0], RW_LB, default = 20'b00000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This correspond to Addr[45:26] of the MMIO rule top limit address. Both base and 
       limit must match to declare a match to this MMIO rule. 
     */
    UINT32 target : 4;
    /* target - Bits[17:14], RW_LB, default = 4'b1000 
       MMIOH Non-Interleave target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMIOH_NONINTERLEAVE_RULE_N1_CHABC_SAD1_STRUCT;


/* MMIO_TARGET_INTERLEAVE_LIST_CFG_0_CHABC_SAD1_REG supported on:             */
/*       SKX_A0 (0x401E9204)                                                  */
/*       SKX (0x401E9204)                                                     */
/* Register default value:              0x88888888                            */
#define MMIO_TARGET_INTERLEAVE_LIST_CFG_0_CHABC_SAD1_REG 0x03014204
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} MMIO_TARGET_INTERLEAVE_LIST_CFG_0_CHABC_SAD1_STRUCT;


/* MMIO_TARGET_INTERLEAVE_LIST_CFG_1_CHABC_SAD1_REG supported on:             */
/*       SKX_A0 (0x401E9208)                                                  */
/*       SKX (0x401E9208)                                                     */
/* Register default value:              0x88888888                            */
#define MMIO_TARGET_INTERLEAVE_LIST_CFG_1_CHABC_SAD1_REG 0x03014208
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} MMIO_TARGET_INTERLEAVE_LIST_CFG_1_CHABC_SAD1_STRUCT;


/* MMIO_TARGET_INTERLEAVE_LIST_CFG_2_CHABC_SAD1_REG supported on:             */
/*       SKX_A0 (0x401E920C)                                                  */
/*       SKX (0x401E920C)                                                     */
/* Register default value:              0x88888888                            */
#define MMIO_TARGET_INTERLEAVE_LIST_CFG_2_CHABC_SAD1_REG 0x0301420C
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} MMIO_TARGET_INTERLEAVE_LIST_CFG_2_CHABC_SAD1_STRUCT;


/* MMIO_TARGET_INTERLEAVE_LIST_CFG_3_CHABC_SAD1_REG supported on:             */
/*       SKX_A0 (0x401E9210)                                                  */
/*       SKX (0x401E9210)                                                     */
/* Register default value:              0x88888888                            */
#define MMIO_TARGET_INTERLEAVE_LIST_CFG_3_CHABC_SAD1_REG 0x03014210
/* Struct format extracted from XML file SKX\1.29.1.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       NodeID of the MMIO target.
       		  b'1xxx  Target is a local IIO Stack with local stack ID xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} MMIO_TARGET_INTERLEAVE_LIST_CFG_3_CHABC_SAD1_STRUCT;


#endif /* CHABC_SAD1_h */
