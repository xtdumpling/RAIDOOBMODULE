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

#ifndef KTI_LLPMON_h
#define KTI_LLPMON_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* KTI_LLPMON_DEV 14                                                          */
/* KTI_LLPMON_FUN 0                                                           */

/* VID_KTI_LLPMON_REG supported on:                                           */
/*       SKX_A0 (0x20370000)                                                  */
/*       SKX (0x20370000)                                                     */
/* Register default value:              0x8086                                */
#define VID_KTI_LLPMON_REG 0x09002000
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} VID_KTI_LLPMON_STRUCT;


/* DID_KTI_LLPMON_REG supported on:                                           */
/*       SKX_A0 (0x20370002)                                                  */
/*       SKX (0x20370002)                                                     */
/* Register default value:              0x2058                                */
#define DID_KTI_LLPMON_REG 0x09002002
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2058 
        
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
} DID_KTI_LLPMON_STRUCT;


/* PCICMD_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x20370004)                                                  */
/*       SKX (0x20370004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_KTI_LLPMON_REG 0x09002004
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} PCICMD_KTI_LLPMON_STRUCT;


/* PCISTS_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x20370006)                                                  */
/*       SKX (0x20370006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_KTI_LLPMON_REG 0x09002006
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
    /* capabilities_list - Bits[4:4], RO, default = 1'b1 
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
} PCISTS_KTI_LLPMON_STRUCT;


/* RID_KTI_LLPMON_REG supported on:                                           */
/*       SKX_A0 (0x10370008)                                                  */
/*       SKX (0x10370008)                                                     */
/* Register default value:              0x00                                  */
#define RID_KTI_LLPMON_REG 0x09001008
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} RID_KTI_LLPMON_STRUCT;


/* CCR_N0_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x10370009)                                                  */
/*       SKX (0x10370009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_KTI_LLPMON_REG 0x09001009
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_KTI_LLPMON_STRUCT;


/* CCR_N1_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x2037000A)                                                  */
/*       SKX (0x2037000A)                                                     */
/* Register default value:              0x1101                                */
#define CCR_N1_KTI_LLPMON_REG 0x0900200A
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT16 sub_class : 8;
    /* sub_class - Bits[7:0], RO_V, default = 8'b00000001 
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
    /* base_class - Bits[15:8], RO_V, default = 8'b00010001 
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
} CCR_N1_KTI_LLPMON_STRUCT;


/* CLSR_KTI_LLPMON_REG supported on:                                          */
/*       SKX_A0 (0x1037000C)                                                  */
/*       SKX (0x1037000C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_KTI_LLPMON_REG 0x0900100C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} CLSR_KTI_LLPMON_STRUCT;


/* PLAT_KTI_LLPMON_REG supported on:                                          */
/*       SKX_A0 (0x1037000D)                                                  */
/*       SKX (0x1037000D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_KTI_LLPMON_REG 0x0900100D
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} PLAT_KTI_LLPMON_STRUCT;


/* HDR_KTI_LLPMON_REG supported on:                                           */
/*       SKX_A0 (0x1037000E)                                                  */
/*       SKX (0x1037000E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_KTI_LLPMON_REG 0x0900100E
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} HDR_KTI_LLPMON_STRUCT;


/* BIST_KTI_LLPMON_REG supported on:                                          */
/*       SKX_A0 (0x1037000F)                                                  */
/*       SKX (0x1037000F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_KTI_LLPMON_REG 0x0900100F
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} BIST_KTI_LLPMON_STRUCT;


/* SVID_F0_KTI_LLPMON_REG supported on:                                       */
/*       SKX_A0 (0x2037002C)                                                  */
/*       SKX (0x2037002C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_F0_KTI_LLPMON_REG 0x0900202C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * PCIE Subsystem Vendor Identification Number
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'h8086 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_F0_KTI_LLPMON_STRUCT;


/* SDID_F0_KTI_LLPMON_REG supported on:                                       */
/*       SKX_A0 (0x2037002E)                                                  */
/*       SKX (0x2037002E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_F0_KTI_LLPMON_REG 0x0900202E
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * PCIE Subsystem Device ID
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0 
       Assigned by the subsystem vendor to uniquely identify the subsystem.
     */
  } Bits;
  UINT16 Data;
} SDID_F0_KTI_LLPMON_STRUCT;


/* CAPPTR_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x10370034)                                                  */
/*       SKX (0x10370034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_KTI_LLPMON_REG 0x09001034
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * PCI Capability Pointer Register
 */
typedef union {
  struct {
    UINT8 capability_pointer : 8;
    /* capability_pointer - Bits[7:0], RO, default = 8'h40 
       Points to the first capability structure for the device which is the PCIe 
       capability. 
     */
  } Bits;
  UINT8 Data;
} CAPPTR_KTI_LLPMON_STRUCT;


/* INTL_KTI_LLPMON_REG supported on:                                          */
/*       SKX_A0 (0x1037003C)                                                  */
/*       SKX (0x1037003C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_KTI_LLPMON_REG 0x0900103C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} INTL_KTI_LLPMON_STRUCT;


/* INTPIN_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x1037003D)                                                  */
/*       SKX (0x1037003D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_KTI_LLPMON_REG 0x0900103D
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} INTPIN_KTI_LLPMON_STRUCT;


/* MINGNT_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x1037003E)                                                  */
/*       SKX (0x1037003E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_KTI_LLPMON_REG 0x0900103E
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} MINGNT_KTI_LLPMON_STRUCT;


/* MAXLAT_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x1037003F)                                                  */
/*       SKX (0x1037003F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_KTI_LLPMON_REG 0x0900103F
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
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
} MAXLAT_KTI_LLPMON_STRUCT;


/* KTILCP_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x40370080)                                                  */
/*       SKX (0x40370080)                                                     */
/* Register default value:              0x00053000                            */
#define KTILCP_KTI_LLPMON_REG 0x09004080
/* Struct format extracted from XML file SKX_A0\3.14.0.CFG.xml.
 * Intel UPI Link Capability
 */
typedef union {
  struct {
    UINT32 kti_version_number : 4;
    /* kti_version_number - Bits[3:0], RO, default = 4'h0 
       Intel UPI Version number
       	0h - rev 1.0
       >0h - Reserved
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 crc_mode_supported : 2;
    /* crc_mode_supported - Bits[9:8], RO_V, default = 2'b0 
       CRC Mode Supported
       00 - CRC16
       01 - CRC16 and Rolling CRC
       10 - Reserved
       11 - Reserved
       Value set based on SKU.
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 llrq_wrap_value : 8;
    /* llrq_wrap_value - Bits[19:12], RO, default = 8'h53 
       Link Layer Retry Queue Wrap Value 
       Size of Queue - 1 
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILCP_KTI_LLPMON_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Link Capability
 */
typedef union {
  struct {
    UINT32 kti_version_number : 4;
    /* kti_version_number - Bits[3:0], RO, default = 4'h0 
       Intel UPI Version number
       	0h - rev 1.0
       >0h - Reserved
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 crc_mode_supported : 2;
    /* crc_mode_supported - Bits[9:8], RO_V, default = 2'b0 
       CRC Mode Supported
       00 - CRC16
       01 - CRC16 and Rolling CRC
       10 - Reserved
       11 - Reserved
       Value set based on SKU.
     */
    UINT32 vn1_supported : 1;
    /* vn1_supported - Bits[10:10], RO_V, default = 1'b0 
       VN1 Supported
       0 - VN1 Not Supported
       1 - VN1 Is Supported
     */
    UINT32 rsvd_11 : 1;
    /* rsvd_11 - Bits[11:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 llrq_wrap_value : 8;
    /* llrq_wrap_value - Bits[19:12], RO, default = 8'h53 
       Link Layer Retry Queue Wrap Value 
       Size of Queue - 1 
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILCP_KTI_LLPMON_STRUCT;



/* KTILCL_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x40370084)                                                  */
/*       SKX (0x40370084)                                                     */
/* Register default value:              0x00003F01                            */
#define KTILCL_KTI_LLPMON_REG 0x09004084
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Link Control
 */
typedef union {
  struct {
    UINT32 link_layer_reset : 1;
    /* link_layer_reset - Bits[0:0], RW_V, default = 1'b1 
       Re-initialize without resetting values in sticky registers. Write 1 to reset 
       link - this is a destructive reset to all link layer state. When link layer 
       reset completes, hardware will clear the bit to 0. 
       This reset should only be done when software can guarantee no valid protocol 
       traffic is flowing on the link. Only supported usage for this bit in processor 
       is hot-plug sequence. 
       Note: The default value of the register is 1. It is expected that, when a 
       platform is booting, the deassertion of the platform reset pin will allow link 
       layer reset to proceed and once that completes, hardware will clear the bit to 
       0. 
     */
    UINT32 enable_debug_flits : 1;
    /* enable_debug_flits - Bits[1:1], RWS, default = 1'b0 
       0 - No LLCTRL-DEBUG flits will be sent
       1 - LLCTRL-DEBUG flits may be sent
     */
    UINT32 link_layer_initialization_stall : 1;
    /* link_layer_initialization_stall - Bits[2:2], RWS, default = 1'b0 
       Link Layer 	Initialization Stall
       	0 - disable
        1 - enable on next initialization. Stall the transmission of the 
       LLCTRL-INIT.Param flit until this bit is cleared. 
        Note: this bit is set and cleared only by software (no hardware clearing is 
       supported). 
     */
    UINT32 link_layer_credit_refund_stall : 1;
    /* link_layer_credit_refund_stall - Bits[3:3], RWS, default = 1'b0 
       Link Layer Credit Refund Stall
       	0 - disable
       1 - Enable, on next initialization, stall the refund of initial link layer 
       credits until this bit is cleared. 
        Note: this bit is set and cleared only by software (no hardware clearing is 
       supported). 
     */
    UINT32 llr_retry_timeout : 3;
    /* llr_retry_timeout - Bits[6:4], RWS, default = 3'b0 
       Link Layer Retry 	Timeout value in terms of flits received
       000 - 4095 flits
       001 - 2047 flits
       010 - 1023 flits
       011 - 511 flits
       100 - 255 flits
       101 - 127 flits
       110 - 63 flits
       111 - 31 flits
        Note: This timeout value must be set higher than the round-trip delay between 
       this device and the remote device. 
       Note: Values below 4095 are intended for validation purposes and are not 
       expected for use in systems. 
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 max_num_retry : 4;
    /* max_num_retry - Bits[11:8], RWS, default = 4'hf 
       Consecutive LLRs to Physical Layer ReInit
       Note: This value is directly encoded which is different then Intel UPI 
       definition where values were encoded. 
     */
    UINT32 max_num_phy_reinit : 2;
    /* max_num_phy_reinit - Bits[13:12], RWS, default = 2'h3 
       Consecutive Phy ReInit?s to RETRY_ABORT
       Note: this value is directly encoded which is different then Intel UPI 
       definition where values were encoded. 
     */
    UINT32 next_crc_mode : 2;
    /* next_crc_mode - Bits[15:14], RWS, default = 2'b0 
       CRC mode for the Tx and Rx of the Link layer to take effect after the next Link 
       layer reset. 
       00 - Use CRC16
       01 - Use Rolling CRC
       10 - Reserved
       11 - Reserved
       Note: Setting Next CRC Mode to Rolling CRC will have no effect if that mode is 
       not supported by the device as defined in KTILCP.crc_mode_supported.  Software 
       must ensure the Next CRC Modes being set at both ends of the link are within the 
       capability of the devices and match each other. 
     */
    UINT32 rsvd_16 : 16;
    /* rsvd_16 - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILCL_KTI_LLPMON_STRUCT;


/* KTILCCL_KTI_LLPMON_REG supported on:                                       */
/*       SKX_A0 (0x40370088)                                                  */
/*       SKX (0x40370088)                                                     */
/* Register default value:              0x00924942                            */
#define KTILCCL_KTI_LLPMON_REG 0x09004088
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Link Credit Control
 * Register controls what credits are defined for each message class on VN0 and 
 * VNA. These credits are made visible on Intel UPI during the initialize phase of 
 * in the link layer. Incorrect programming can result in overflow of the receive 
 * queue. When returning credits on Intel UPI this register is used in conjunction 
 * with the Intel UPI standard register Intel UPI Link Control (KTILCL) to 
 * determine how many credits are returned. In other words, the values specified 
 * here act as the Max in the field descriptions for KTI[11:10] and KTI[9:8]. This 
 * value is captured and used by the Link Layer when exiting the parameter 
 * exchange. This state is referred to as Begin Normal Operation. 
 */
typedef union {
  struct {
    UINT32 vna_credits : 8;
    /* vna_credits - Bits[7:0], RWS, default = 8'h42 
       VNA credits to advertise at the next link init
       0-84 credits (total across all VNA/VN0/VN1 credits must be less-than-or-equal-to 
       84) 
     */
    UINT32 vn0_req_credits : 3;
    /* vn0_req_credits - Bits[10:8], RWS, default = 3'h1 
       VN0 - Req credits to advertise at the next link init
        Allowed values:0-7 credit (total across all VNA/VN0/VN1 credits must be 
       less-than-or-equal-to 84) 
     */
    UINT32 vn0_snp_credits : 3;
    /* vn0_snp_credits - Bits[13:11], RWS, default = 3'h1 
       VN0 - Snp credits to advertise at the next link init
        Allowed values:0-7 credits (total across all VNA/VN0/VN1 credits must be 
       less-than-or-equal-to 84) 
     */
    UINT32 vn0_rsp_credits : 3;
    /* vn0_rsp_credits - Bits[16:14], RWS, default = 3'h1 
       VN0 - RSP credits to advertise at the next link init
        Allowed values:0-7 credits (total across all VNA/VN0/VN1 credits must be 
       less-than-or-equal-to 84) 
     */
    UINT32 vn0_wb_credits : 3;
    /* vn0_wb_credits - Bits[19:17], RWS, default = 3'h1 
       VN0 - WB credits to advertise at the next link init
        Allowed values:0-7 credits (total across all VNA/VN0/VN1 credits must be 
       less-than-or-equal-to 84) 
     */
    UINT32 vn0_ncb_credits : 3;
    /* vn0_ncb_credits - Bits[22:20], RWS, default = 3'h1 
       VN0 - NCB credits to advertise at the next link init
        Allowed values:0-7 credits (total across all VNA/VN0/VN1 credits must be 
       less-than-or-equal-to 84) 
     */
    UINT32 vn0_ncs_credits : 3;
    /* vn0_ncs_credits - Bits[25:23], RWS, default = 3'h1 
       VN0 - NCS credits to advertise at the next link init
        Allowed values:0-7 credits (total across all VNA/VN0/VN1 credits must be 
       less-than-or-equal-to 84) 
     */
    UINT32 vn1_enable : 1;
    /* vn1_enable - Bits[26:26], RWS_L, default = 1'b0 
       Program to 1 to enable VN1 Link Credit Init.  Use VN0 credit values for VN1 
       channels.  VN0 values in the other fields of this register apply to the 
       corresponding channel in VN1. 
       VN1 must be supported and enabled.
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILCCL_KTI_LLPMON_STRUCT;


/* KTILS_KTI_LLPMON_REG supported on:                                         */
/*       SKX_A0 (0x4037008C)                                                  */
/*       SKX (0x4037008C)                                                     */
/* Register default value:              0x00000000                            */
#define KTILS_KTI_LLPMON_REG 0x0900408C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Link Status
 */
typedef union {
  struct {
    UINT32 link_layer_retry_queue_consumed : 8;
    /* link_layer_retry_queue_consumed - Bits[7:0], RO_V, default = 8'b0 
       Link Layer Retry Queue Consumed
       	Number of Retry Queue entries currently consumed while waiting for ACK.
     */
    UINT32 init_state : 2;
    /* init_state - Bits[9:8], RO_V, default = 2'b0 
       Reflects the current Initialization State of the link Layer, including any stall 
       conditions controlled by LL_LCL[3:2]. 
       00 - NOT_RDY_FOR_INIT
       01 - PARAM_EX
       10 - CRD_RETURN_STALL
       11 - INIT_DONE
     */
    UINT32 retry_state : 3;
    /* retry_state - Bits[12:10], RO_V, default = 3'b0 
       Reflects the current state of Local Retry State Machine
       000 - Retry_Local_Normal
       001 - Retry_LLRReq
       010 - Retry_Local_Idle
       011 - Retry_Phy_Reinit
       100 - Retry_Abort
     */
    UINT32 rx_normal_op_state : 1;
    /* rx_normal_op_state - Bits[13:13], RO_V, default = 1'b0 
       Reflects whether the LL Rx is in Normal Operation (i.e.. has received at least 
       one flit from the Phy).  Resets to 0 on cold reset, warm reset, or Link layer 
       LL_LCL[0] reset, and is set to 1 upon receipt of the first flit from the 
       Physical layer. 
       0 - Not Rx Normal Operation
       1 - Rx Normal Operation
     */
    UINT32 current_crc_mode : 2;
    /* current_crc_mode - Bits[15:14], RO_V, default = 2'b0 
       This field represents the current operating CRC mode of the Tx and Rx of the 
       Link layer. 
       00 - CRC16
       01 - Rolling CRC
       10 - Reserved
       11 - Reserved
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILS_KTI_LLPMON_STRUCT;


/* KTILES_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x40370090)                                                  */
/*       SKX (0x40370090)                                                     */
/* Register default value:              0x00000000                            */
#define KTILES_KTI_LLPMON_REG 0x09004090
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Link Error Status
 */
typedef union {
  struct {
    UINT32 worst_llr_numretry_count : 4;
    /* worst_llr_numretry_count - Bits[3:0], RW1CS, default = 4'b0 
       Worst Case Link Layer Retry NUM_RETRY
       Highest NUM_RETRY count achieved since last clear of this register or PHY reinit 
       initiated by the LLR LRSM. 
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 worst_llr_num_phyreinit_count : 2;
    /* worst_llr_num_phyreinit_count - Bits[9:8], RW1CS, default = 2'b0 
       Worst Case Retry NUM_PHY_REINIT
       Highest NUM_PHY_REINIT count achieved since last clear of this field.
     */
    UINT32 rsvd_10 : 22;
    /* rsvd_10 - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILES_KTI_LLPMON_STRUCT;


/* KTILP0_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x40370094)                                                  */
/*       SKX (0x40370094)                                                     */
/* Register default value:              0x09000000                            */
#define KTILP0_KTI_LLPMON_REG 0x09004094
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Link Parameter 0
 */
typedef union {
  struct {
    UINT32 kti_version : 4;
    /* kti_version - Bits[3:0], RO_V, default = 4'b0 
       Parameter bits from peer agent. Cleared on any LL initialization.
     */
    UINT32 rsvd_4 : 4;
    UINT32 sending_port : 5;
    /* sending_port - Bits[12:8], RO_V, default = 5'b0 
       Parameter bits from peer agent. Cleared on any LL initialization.
       The processor die port number of the sending port.  Legal values are 0-2.
     */
    UINT32 rsvd_13 : 1;
    /* rsvd_13 - Bits[13:13], RO_V, default = 1'b0 
       Parameter bits from peer agent. Cleared on any LL initialization.
     */
    UINT32 lt_agent : 1;
    /* lt_agent - Bits[14:14], RO_V, default = 1'b0 
       Parameter bits from peer agent. Cleared on any LL initialization.
       Set if the sending socket acts as an LT Agent.  Tied to the LT Strap (LTEnable).
     */
    UINT32 firmware : 1;
    /* firmware - Bits[15:15], RO_V, default = 1'b0 
       Parameter bits from peer agent. Cleared on any LL initialization.
       Set if the sending socket acts as a firmware agent.  Tied to the Firmware Agent 
       Strap (FrmAGENT). 
     */
    UINT32 base_nodeid : 4;
    /* base_nodeid - Bits[19:16], RO_V, default = 4'b0 
       Parameter bits from peer agent. Cleared on any LL initialization.
       The NodeID of the sending socket.
     */
    UINT32 rsvd_23_20 : 4;
    /* rsvd_23_20 - Bits[23:20], RO_V, default = 4'b0 
       Parameter bits from peer agent. Cleared on any LL initialization.
     */
    UINT32 llr_wrap : 8;
    /* llr_wrap - Bits[31:24], RO_V, default = 8'h9 
       Parameter bits from peer agent. Cleared on any LL initialization.
       Specifies the size of the link layer retry buffer -1.
     */
  } Bits;
  UINT32 Data;
} KTILP0_KTI_LLPMON_STRUCT;


/* KTILP1_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x40370098)                                                  */
/*       SKX (0x40370098)                                                     */
/* Register default value:              0x00000000                            */
#define KTILP1_KTI_LLPMON_REG 0x09004098
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Link Parameter 1
 */
typedef union {
  struct {
    UINT32 rsvd_63_32 : 32;
    /* rsvd_63_32 - Bits[31:0], RO_V, default = 32'b0 
       Parameter bits 63:32 from peer agent. Cleared on any LL initialization.
     */
  } Bits;
  UINT32 Data;
} KTILP1_KTI_LLPMON_STRUCT;


/* KTIVIRAL_KTI_LLPMON_REG supported on:                                      */
/*       SKX_A0 (0x4037009C)                                                  */
/*       SKX (0x4037009C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIVIRAL_KTI_LLPMON_REG 0x0900409C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Viral Control and Status Register
 * This register has control/status bits for Intel UPI Viral.
 */
typedef union {
  struct {
    UINT32 kti_viral_alert_enable : 1;
    /* kti_viral_alert_enable - Bits[0:0], RW, default = 1'b0 
       Enables assertion of Global Viral by Intel UPI Agent.
       0 - Disabled
       1 - Enabled
     */
    UINT32 kti_signal_global_fatal : 1;
    /* kti_signal_global_fatal - Bits[1:1], RW, default = 1'b0 
       Enables assertion of Global Fatal by Intel UPI Agent due to internal detection 
       of an uncorrectable error. 
       0 - Disabled
       1 - Enabled
     */
    UINT32 kti_global_viral_mask : 1;
    /* kti_global_viral_mask - Bits[2:2], RW, default = 1'b0 
       Enables generation of Tx (outgoing) Viral Flow
       0 - Block Injection of Viral on Intel UPI
       1 - Allow Injection of Viral on Intel UPI 
       Note: polarity of this bit is treated as an ""enable""
     */
    UINT32 rsvd_3 : 23;
    /* rsvd_3 - Bits[25:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti_unc_err_set : 1;
    /* kti_unc_err_set - Bits[26:26], RW1C, default = 1'b0 
       Status that indicates the Intel UPI Agent has detected an uncorrectable error.  
        Note 1 - if this bit is set, the Intel UPI Agent will continue to assert Global 
       Fatal/Global Viral (if enabled) 
        Note 2 - uncorrectable error used here includes KTIERRDIS masking, but does NOT 
       include KTI_MC_CTL masking 
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti_pkt_viral_set : 1;
    /* kti_pkt_viral_set - Bits[28:28], RW1C, default = 1'b0 
       Status that indicates the Intel UPI Agent has received Viral bit received in the 
       LLR.Ack message. 
        Note: If this bit is set, the Intel UPI Agent will continue to assert Global 
       Viral (if enabled) 
     */
    UINT32 rsvd_29 : 1;
    /* rsvd_29 - Bits[29:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti_viral_status : 1;
    /* kti_viral_status - Bits[30:30], RW1CS, default = 1'b0 
       Indicates previous detection of a Viral condition sent.
     */
    UINT32 kti_viral_state : 1;
    /* kti_viral_state - Bits[31:31], RW1C, default = 1'b0 
       Indicates that the Intel UPI Agent is currently asserting Viral flow outgoing 
       (Tx). 
     */
  } Bits;
  UINT32 Data;
} KTIVIRAL_KTI_LLPMON_STRUCT;


/* KTICSMITHRES_KTI_LLPMON_REG supported on:                                  */
/*       SKX_A0 (0x403700A0)                                                  */
/*       SKX (0x403700A0)                                                     */
/* Register default value:              0x00000000                            */
#define KTICSMITHRES_KTI_LLPMON_REG 0x090040A0
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI CSMI Threshold
 */
typedef union {
  struct {
    UINT32 bios_ce_threshold : 15;
    /* bios_ce_threshold - Bits[14:0], RWS, default = 15'b0 
       Threshold used in generation of CSMI.
     */
    UINT32 match_en : 1;
    /* match_en - Bits[15:15], RWS, default = 1'b0 
       If set it enables additional check for CSMI generation.
       - Generate CSMI (if enabled in KTI_MC_CTL2[32].CSMI_en)  if MATCH_EN is set and 
       KTICSMITHRES.bios_ce_threshold == BIOS_KTI_ERR_ST1[20:6].cor_err_cnt  
       - No CSMI generated if BIOS_CE_THRESHOLD == 0 even if MATCH_EN is set.
       When Match_en==0, we always generate CSMI on any correctable error without 
       checking the threshold if CSMI is enabled. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTICSMITHRES_KTI_LLPMON_STRUCT;


/* KTICERRLOGCTRL_KTI_LLPMON_REG supported on:                                */
/*       SKX_A0 (0x403700A4)                                                  */
/*       SKX (0x403700A4)                                                     */
/* Register default value:              0x00000000                            */
#define KTICERRLOGCTRL_KTI_LLPMON_REG 0x090040A4
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Log Control
 */
typedef union {
  struct {
    UINT32 dis_ce_log : 1;
    /* dis_ce_log - Bits[0:0], RWS, default = 1'b0 
       When DIS_CE_LOG is set, Corrected errors will not be logged in KTI_MC_ST (aka 
       MCi_STATUS).  CMCI will not be signaled for corrected errors when DIS_CE_LOG is 
       set, which happens naturally since they are not logged or counted. 
     */
    UINT32 dis_ucna_log : 1;
    /* dis_ucna_log - Bits[1:1], RWS, default = 1'b0 
       When  set, UCNA errors will not be logged (but will be logged in KTIERR_ST 
       CSRs).  CMCI will not be signaled for UCNA errors when DIS_UCNA_LOG is set. 
       Note: Intel UPI is not logging any UCNA cases in processor, so setting the bit 
       has no affect. 
     */
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTICERRLOGCTRL_KTI_LLPMON_STRUCT;


/* BIOS_KTI_ERR_ST_N0_KTI_LLPMON_REG supported on:                            */
/*       SKX_A0 (0x403700A8)                                                  */
/*       SKX (0x403700A8)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_KTI_ERR_ST_N0_KTI_LLPMON_REG 0x090040A8
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Status
 * BIOS Status CSR is a clone of KTI_MC_ST (aka MCi_STATUS). Only difference is 
 * this one logs Corrected errors and UCNA errors even when they are disabled via 
 * KTICERRLOGCTRL. 
 */
typedef union {
  struct {
    UINT32 mcacod_ll : 2;
    /* mcacod_ll - Bits[1:0], RWS_V, default = 2'b0 
       MSCOD Bus Interconnect Error LL
       Set to 1 for all Transmit Path Parity Protection errors (MSCOD_Code=0x12 ""UC LL 
       or Phy Control Error"" and KTIDBGERRST0.ctrl_txparerr=non-zero) such that 
       mcacod[15:0]=0x0405. 
       Set to 3 for all other errors.
       	Encoding is:
       0x0 - Level 0
       0x1 - Level 1
       0x2 - Level 2
       0x3 - Generic LL
     */
    UINT32 mcacod_ii : 2;
    /* mcacod_ii - Bits[3:2], RWS_V, default = 2'b0 
       MSCOD Bus Interconnect Error II
       Set to 1 for all Transmit Path Parity Protection errors (MSCOD_Code=0x12 ""UC LL 
       or Phy Control Error"" and KTIDBGERRST0.ctrl_txparerr=non-zero) such that 
       mcacod[15:0]=0x0405. 
       Set to 3 for all other errors.
       Encoding is:
       0x0 - Memory Access
       0x1 - Reserved II
       0x2 - I/O
       0x3 - Other Transaction
     */
    UINT32 mcacod_rrrr : 4;
    /* mcacod_rrrr - Bits[7:4], RWS, default = 4'b0 
       Field never set by hardware
     */
    UINT32 mcacod_t : 1;
    /* mcacod_t - Bits[8:8], RWS, default = 1'b0 
       Field never set by hardware
     */
    UINT32 mcacod_pp : 2;
    /* mcacod_pp - Bits[10:9], RWS_V, default = 2'b0 
       MCACOD - Bus Int Error PP
       Set to 2 for all Transmit Path Parity Protection errors (MSCOD_Code=0x12 ""UC LL 
       or Phy Control Error"" and KTIDBGERRST0.ctrl_txparerr=non-zero) such that 
       mcacod[15:0]=0x0405. 
       Set to 2 for all ""UC LL Rx Unsupported/Undefined Packet"" errors 
       (MSCOD_Code=0x11). 
       Set to 3 for all other errors.
       Encoding is:
       0x0 - Local Processor Originated
       0x1 - Local Processor Responded
       0x2 - Local Processor Observed Error
       0x3 - Generic PP
     */
    UINT32 mcacod_int : 1;
    /* mcacod_int - Bits[11:11], RWS_V, default = 1'b0 
       MCACOD - Bus Interconnect Error
       Set to 0 for all Transmit Path Parity Protection errors (MSCOD_Code=0x12 ""UC LL 
       or Phy Control Error"" and KTIDBGERRST0.ctrl_txparerr=non-zero) such that 
       mcacod[15:0]=0x0405. 
       Set to 1 for all other errors.
       Encoding:
       0 - Not Bus Interconnect Error
       1 - Bus Interconnect Error
     */
    UINT32 mcacod_rsvd : 4;
    /* mcacod_rsvd - Bits[15:12], RWS, default = 4'b0 
       Field never set by hardware
     */
    UINT32 mscod_code : 6;
    /* mscod_code - Bits[21:16], RWS_V, default = 6'b0 
       MSCOD Error Code
       Error Code from Intel UPI Agent. This field captures the error type based on the 
       standard MC_ST overwrite rules. 
       The Error Codes and handling are defined in the HAS.  The summary per ID is:
       Uncorrectable (UC)
       0x00 - UC Phy Initialization Failure (NumInit)
       0x01 - UC Phy Detected Drift Buffer Alarm 
       0x02 - UC Phy Detected Latency Buffer Rollover
       0x10 - UC LL Rx detected CRC error: unsuccessful LLR (entered Abort state)
       0x11 - UC LL Rx Unsupported/Undefined packet
       0x12 - UC LL or Phy Control Error
       0x13 - UC LL Rx Parameter Exception
       0x1F - UC LL Detected Control Error from M3UPI 
       Correctable (COR)
       0x20 - COR Phy Initialization Abort
       0x21 - COR Phy Reset
       0x22 - COR Phy Lane failure, recovery in x8 width
       0x23 - COR Phy L0c error corrected without Phy reset
       0x24 - COR Phy L0c error triggering Phy Reset
       0x25 - COR Phy L0p exit error triggering Phy Reset
       0x30 - COR LL Rx detected CRC error: successful LLR without Phy Reinit
       0x31 - COR LL Rx detected CRC error: successful LLR with Phy Reinit
     */
    UINT32 mscod_spare : 10;
    /* mscod_spare - Bits[31:22], RWS_V, default = 10'b0 
       MSCOD SPARE
       Field used for design specific sub-status fields for ""UC LL or Phy Control 
       Errors"" (mscode_code=0x12): 
       [22] - Phy Control Error (note: currently tied to 0 in processor)
       [23] - Unexpected Retry.Ack flit
       [24] - Unexpected Retry.Req flit
       [25] - RF parity error
       [26] - Routeback Table error
       [27] - unexpected Tx Protocol flit (EOP, Header or Data)
       [28] - Rx Header-or-Credit BGF credit overflow/underflow
       [29] - Link Layer Reset still in progress when Phy enters L0 (Phy training 
       should not be enabled until after LL reset is complete as indicated by 
       KTILCL.LinkLayerReset going back to 0). 
       [30] - Link Layer reset initiated while protocol traffic not idle
       [31] - Link Layer Tx Parity Error
     */
  } Bits;
  UINT32 Data;
} BIOS_KTI_ERR_ST_N0_KTI_LLPMON_STRUCT;


/* BIOS_KTI_ERR_ST_N1_KTI_LLPMON_REG supported on:                            */
/*       SKX_A0 (0x403700AC)                                                  */
/*       SKX (0x403700AC)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_KTI_ERR_ST_N1_KTI_LLPMON_REG 0x090040AC
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Status
 * BIOS Status CSR is a clone of KTI_MC_ST (aka MCi_STATUS). Only difference is 
 * this one logs Corrected errors and UCNA errors even when they are disabled via 
 * KTICERRLOGCTRL. 
 */
typedef union {
  struct {
    UINT32 other_info : 6;
    /* other_info - Bits[5:0], RWS_V, default = 6'b0 
       Other Info
       Field used for design specific sub-status fields for ""UC LL or Phy Control 
       Errors"" (mscode_code=0x12): 
       [32] - Link layer credit or queue (various) overflow/underflow
       [33] - Flow Queue credit overflow/underflow
       [37:34] - reserved 
     */
    UINT32 cor_err_cnt : 15;
    /* cor_err_cnt - Bits[20:6], RWS_V, default = 15'b0 
       Corrected Error Count
        15-bit counter which is incremented on every corrected error. MSB (bit 15) is a 
       sticky overflow bit. 
     */
    UINT32 corr_err_sts_ind : 2;
    /* corr_err_sts_ind - Bits[22:21], RWS, default = 2'b0 
       Field never set by hardware (but can be set by software).
       00 - No tracking - No hardware status tracking is provided for the structure 
       reporting this event. 
       01 - Green - Status tracking is provided for the structure posting the event; 
       the current status is green (below threshold).  For more information, see 
       Section ""Enhanced Cache Error reporting"" of the Software Developer Manual. 
       10 - Yellow - Status tracking is provided for the structure posting the event; 
       the current status is yellow (below threshold).  For more information, see 
       Section ""Enhanced Cache Error reporting"" of the Software Developer Manual. 
       11 - Reserved
     */
    UINT32 ar : 1;
    /* ar - Bits[23:23], RWS, default = 1'b0 
       Field never set by hardware
     */
    UINT32 s : 1;
    /* s - Bits[24:24], RWS, default = 1'b0 
       Field never set by hardware
     */
    UINT32 pcc : 1;
    /* pcc - Bits[25:25], RWS_V, default = 1'b0 
       Processor Context Corrupt Flag
       	Bit set on all uncorrectable errors
     */
    UINT32 addrv : 1;
    /* addrv - Bits[26:26], RWS_V, default = 1'b0 
       Address Valid in BIOS_KTI_ERR_AD.
       Bit set on error types which load Tx Address into BIOS_KTI_ERR_AD.  The 
       applicable error types are: 
       LL or Phy Generic Control (error code 0x12)
     */
    UINT32 miscv : 1;
    /* miscv - Bits[27:27], RWS_V, default = 1'b0 
       BIOS_KTI_ERR_MISC valid flag
       Indicates (when set) that the BIOS_KTI_ERR_MISC register contains additional 
       information regarding the error. See individual fields of BIOS_KTI_ERR_MISC for 
       details on which are valid for a given error. 
     */
    UINT32 en : 1;
    /* en - Bits[28:28], RWS_V, default = 1'b0 
       Error Enable Flag
       Set by hardware when MCA (uncorrectable) is signaled. 
     */
    UINT32 uc : 1;
    /* uc - Bits[29:29], RWS_V, default = 1'b0 
       Error Uncorrected
       	Set for error which was not corrected.
     */
    UINT32 over : 1;
    /* over - Bits[30:30], RWS_V, default = 1'b0 
       MCA Overflow Flag
       	Set on all second error cases
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RWS_V, default = 1'b0 
       Set by hardware when an uncorrectable or correctable error is logged.
     */
  } Bits;
  UINT32 Data;
} BIOS_KTI_ERR_ST_N1_KTI_LLPMON_STRUCT;


/* BIOS_KTI_ERR_MISC_N0_KTI_LLPMON_REG supported on:                          */
/*       SKX_A0 (0x403700B0)                                                  */
/*       SKX (0x403700B0)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_KTI_ERR_MISC_N0_KTI_LLPMON_REG 0x090040B0
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Misc
 * BIOS Misc CSR is a clone of KTI_MC_MISC (aka MCi_MISC). Only difference is this 
 * one logs Corrected errors and UCNA errors even when they are disabled via 
 * KTICERRLOGCTRL. 
 * 
 * 
 */
typedef union {
  struct {
    UINT32 flit_hdr : 20;
    /* flit_hdr - Bits[19:0], RWS_V, default = 20'b0 
       HDR bits captured:
       [0] - HIB (if not set other bits >4 in this log are not valid)
       [1] - VN
       [2] - VNA
       [3] - ACK
       [4] - CRD
       [8:5]- Slot 0 opcode
       [11:9] - Slot 0 MC
       [15:12] - Slot 1 Opcode
       [16] - Slot 1 MC
       [19:17] - Slot 2 Opcode
       Note: This field is valid for all Errors caused by flit decode errors 
       (mscod_code 0x11, 0x13, 0x22, 0x23, 0x30, 0x31).    For all other codes, this 
       field should be ignored. 
     */
    UINT32 syndrome_or_l0cbadlanes : 12;
    /* syndrome_or_l0cbadlanes - Bits[31:20], RWS_V, default = 16'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Captures Syndrome for  CRC check, or in L0c error captures 8-bit vector of bad 
       lanes. 
       For both conditions need to check ""Phy_state"" field to translate to a physical 
       lane. 
       Syndrome Applies for errors where crc error cause the error (mscod_code 0x30, 
       0x31, 0x10, 0x11) 
       L0c bad lane map applies in L0c errors (mscod_code 0x23, 0x24).
       For all other codes, this field should be ignored.
     */
  } Bits;
  UINT32 Data;
} BIOS_KTI_ERR_MISC_N0_KTI_LLPMON_STRUCT;


/* BIOS_KTI_ERR_MISC_N1_KTI_LLPMON_REG supported on:                          */
/*       SKX_A0 (0x403700B4)                                                  */
/*       SKX (0x403700B4)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_KTI_ERR_MISC_N1_KTI_LLPMON_REG 0x090040B4
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Misc
 * BIOS Misc CSR is a clone of KTI_MC_MISC (aka MCi_MISC). Only difference is this 
 * one logs Corrected errors and UCNA errors even when they are disabled via 
 * KTICERRLOGCTRL. 
 * 
 * 
 */
typedef union {
  struct {
    UINT32 syndrome_or_l0cbadlanes : 4;
    /* syndrome_or_l0cbadlanes - Bits[3:0], RWS_V, default = 16'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Captures Syndrome for  CRC check, or in L0c error captures 8-bit vector of bad 
       lanes. 
       For both conditions need to check ""Phy_state"" field to translate to a physical 
       lane. 
       Syndrome Applies for errors where crc error cause the error (mscod_code 0x30, 
       0x31, 0x10, 0x11) 
       L0c bad lane map applies in L0c errors (mscod_code 0x23, 0x24).
       For all other codes, this field should be ignored.
     */
    UINT32 illegal_flit_field : 1;
    /* illegal_flit_field - Bits[4:4], RWS_V, default = 1'b0 
       For Unexpected Flit Errors this bit is set if unsupported error was cause by 
       bits not captured in Flit_HDR fields. In processor some of the cases that  cause 
       this are: CTRL flit (RSVD Types and/or Sub-types), bit set in unsupported 
       payloads (Addr[51:46], RTID[10], HTID[10]). 
       Note: we still capture Flit_HDR information even if this bit is set.
       This field is valid for the following error codes (mscod_code): 0x11, 0x13, 
       0x30, 0x31.  For all other codes, this field should be ignored. 
     */
    UINT32 phy_state : 3;
    /* phy_state - Bits[7:5], RWS_V, default = 3'b0 
       Link State from Physical Layer on error used for isolating lane in CRC error 
       cases: 
       0x0 - 20 lane flit mux phase 0
       0x1 - 20 lane flit mux phase 1
       0x2 - 20 lane flit mux phase 2
       0x3 - 20 lane flit mux phase 3
       0x4 - 20 lane flit mux phase 4
       0x5 - Lower 8 logical lanes active in L0 (failover)
       0x6 - Upper 8 logical lanes active in L0 (failover)
       0x7 - Lower 8 logical lane active in L0p
       This field is valid for the following error codes (mscod_code): 0x11, 0x13, 
       0x22, 0x23, 0x30, 0x31.  For all other codes, this field should be ignored. 
       Note: This field is a compressed encoding of the Intel UPI spec ERxCLM (width) 
       and ERxPhase (flit phasing info) fields: 
       - (ERxCLM=0b111 and ERxPhase=0x0-0x4) is equal to PhyState=0x0-0x4
       - ERxCLM=0b001 is equal to PhyState 0x5(lower failover) or 0x7(L0p)
       - ERxCLM=0b100 is equal to PhyState=0x6(upper failover)
     */
    UINT32 rx_lane_reversal : 1;
    /* rx_lane_reversal - Bits[8:8], RWS_V, default = 1'b0 
       Indicates if physical layer Rx is reversed.
       This field is valid for the following error codes (mscod_code): 0x11, 0x13, 
       0x22, 0x23, 0x30, 0x31.  For all other codes, this field should be ignored. 
     */
    UINT32 rsvd0 : 1;
    /* rsvd0 - Bits[9:9], RWS, default = 1'b0  */
    UINT32 nerr : 12;
    /* nerr - Bits[21:10], RWS_V, default = 12'b0 
       Captures the Next Error, which is any later error which occurred after the 
       primary error was logged (in mscod_code).  Only Uncorrectable Errors (as listed 
       in mscod_code) are logged in a 12-bit one hot encoding format.  Note that in 
       cases where multiple errors (any mix of correctable and uncorrectable) occur in 
       the same cycle, all uncorrectable errors that occur on that cycle will be logged 
       in this field (in a multi-hot fashion). 
       Format: {bit} - {hex mscod_code} {description}
       [42] - 0x00 UC Phy Initialization Failure (NumInit)
       [43] - 0x01 UC Phy Detected Drift Buffer Alarm 
       [44] - 0x02 UC Phy Detected Latency Buffer Rollover
       [45] - Reserved
       [46] - Reserved
       [47] - 0x10 UC LL Rx detected CRC error: unsuccessful LLR (entered Abort state)
       [48] - 0x11 UC LL Rx Unsupported/Undefined packet
       [49] - 0x12 UC LL or Phy Control Error
       [50] - 0x13 UC LL Rx Parameter Exception
       [51] - Reserved
       [52] - Reserved
       [53] - 0x1F UC LL Detected Control Error from M3UPI 
     */
    UINT32 cor_err : 10;
    /* cor_err - Bits[31:22], RWS_V, default = 10'b0 
       Captures Correctable Errors, indicating all correctable error cases that have 
       occurred since last cleared.  Only Correctable Errors (as listed in mscod_code) 
       are logged in a 10-bit multi hot encoding format: 
       Format: {bit} - {hex mscod_code} {description}
       [54] - 0x20 COR Phy Initialization Abort
       [55] - 0x21 COR Phy Reset
       [56] - 0x22 COR Phy Lane failure, recovery in x8 width
       [57] - 0x23 COR Phy L0c error corrected without Phy reset
       [58] - 0x24 COR Phy L0c error triggering Phy Reset
       [59] - 0x25 COR Phy L0p exit error triggering Phy Reset
       [60] - Reserved
       [61] - 0x30 COR LL Rx detected CRC error: successful LLR without Phy Reinit
       [62] - 0x31 COR LL Rx detected CRC error: successful LLR with Phy Reinit
       [63] - Reserved
     */
  } Bits;
  UINT32 Data;
} BIOS_KTI_ERR_MISC_N1_KTI_LLPMON_STRUCT;


/* BIOS_KTI_ERR_AD_N0_KTI_LLPMON_REG supported on:                            */
/*       SKX_A0 (0x403700B8)                                                  */
/*       SKX (0x403700B8)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_KTI_ERR_AD_N0_KTI_LLPMON_REG 0x090040B8
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Misc
 * BIOS Addr CSR is a clone of KTI_MC_AD (aka MCi_ADDR). Only difference is this 
 * one logs Corrected errors and UCNA errors even when they are disabled via 
 * KTICERRLOGCTRL. 
 * 
 * 
 */
typedef union {
  struct {
    UINT32 rsvd0 : 6;
    /* rsvd0 - Bits[5:0], RWS, default = 6'b0  */
    UINT32 address_45_6 : 26;
    /* address_45_6 - Bits[31:6], RWS_V, default = 40'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Tx Address 45 to 6
       Note: this is only applicable if BIOS_KTI_ERR_ST.addrv=1.  If both flit slot0 
       and slot1 hit a Tx address parity error on the same cycle, slot0 is logged here. 
     */
  } Bits;
  UINT32 Data;
} BIOS_KTI_ERR_AD_N0_KTI_LLPMON_STRUCT;


/* BIOS_KTI_ERR_AD_N1_KTI_LLPMON_REG supported on:                            */
/*       SKX_A0 (0x403700BC)                                                  */
/*       SKX (0x403700BC)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_KTI_ERR_AD_N1_KTI_LLPMON_REG 0x090040BC
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Misc
 * BIOS Addr CSR is a clone of KTI_MC_AD (aka MCi_ADDR). Only difference is this 
 * one logs Corrected errors and UCNA errors even when they are disabled via 
 * KTICERRLOGCTRL. 
 * 
 * 
 */
typedef union {
  struct {
    UINT32 address_45_6 : 14;
    /* address_45_6 - Bits[13:0], RWS_V, default = 40'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Tx Address 45 to 6
       Note: this is only applicable if BIOS_KTI_ERR_ST.addrv=1.  If both flit slot0 
       and slot1 hit a Tx address parity error on the same cycle, slot0 is logged here. 
     */
    UINT32 rsvd1 : 18;
    /* rsvd1 - Bits[31:14], RWS, default = 17'b0  */
  } Bits;
  UINT32 Data;
} BIOS_KTI_ERR_AD_N1_KTI_LLPMON_STRUCT;


/* KTICRCERRCNT_KTI_LLPMON_REG supported on:                                  */
/*       SKX_A0 (0x403700C0)                                                  */
/*       SKX (0x403700C0)                                                     */
/* Register default value:              0x00000000                            */
#define KTICRCERRCNT_KTI_LLPMON_REG 0x090040C0
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI CRC Error Counter
 * Register used for counter CRC errors in Intel UPI.
 */
typedef union {
  struct {
    UINT32 counter : 31;
    /* counter - Bits[30:0], RW1CS, default = 31'b0 
       Counter increments every time the Link Layer detects a CRC error.
     */
    UINT32 overflow : 1;
    /* overflow - Bits[31:31], RWS_V, default = 1'b0 
       Overflow bit - set when the number of detected CRC errors exceeds the counter 
       width. 
     */
  } Bits;
  UINT32 Data;
} KTICRCERRCNT_KTI_LLPMON_STRUCT;


/* KTIERRDIS0_KTI_LLPMON_REG supported on:                                    */
/*       SKX_A0 (0x403700C4)                                                  */
/*       SKX (0x403700C4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIERRDIS0_KTI_LLPMON_REG 0x090040C4
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Disable
 * Allows BIOS to disable error report and logging for correctable and 
 * uncorrectable error cases. 
 */
typedef union {
  struct {
    UINT32 cor_mask : 10;
    /* cor_mask - Bits[9:0], RWS, default = 10'b0 
       Correctable Error Disable Mask
       If bit is set it disable MCA error logging signal for the given error code. See 
       Intel UPI MCA register for error code description. 
       Format: {bit} - {hex mscod_code} {description}
       [0] - 0x20 COR Phy Initialization Abort
       [1] - 0x21 COR Phy Reset
       [2] - 0x22 COR Phy Lane failure, recovery in x8 width
       [3] - 0x23 COR Phy L0c error corrected without Phy reset
       [4] - 0x24 COR Phy L0c error triggering Phy Reset
       [5] - 0x25 COR Phy L0p exit error triggering Phy Reset
       [6] - Reserved
       [7] - 0x30 COR LL Rx detected CRC error: successful LLR without Phy Reinit
       [8] - 0x31 COR LL Rx detected CRC error: successful LLR with Phy Reinit
       [9] - Reserved
     */
    UINT32 rsvd_10 : 6;
    /* rsvd_10 - Bits[15:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_16 : 10;
    UINT32 rsvd_26 : 5;
    /* rsvd_26 - Bits[30:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 smi_en : 1;
    /* smi_en - Bits[31:31], RW, default = 1'b0 
       smi enable
     */
  } Bits;
  UINT32 Data;
} KTIERRDIS0_KTI_LLPMON_STRUCT;


/* KTIERRDIS1_KTI_LLPMON_REG supported on:                                    */
/*       SKX_A0 (0x403700C8)                                                  */
/*       SKX (0x403700C8)                                                     */
/* Register default value:              0x00000000                            */
#define KTIERRDIS1_KTI_LLPMON_REG 0x090040C8
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Disable
 * Allows BIOS to disable error report and logging for correctable and 
 * uncorrectable error cases. 
 */
typedef union {
  struct {
    UINT32 unc_mask : 12;
    /* unc_mask - Bits[11:0], RWS, default = 12'b0 
       Uncorrectable Disable Mask
        Disable uncorrectable errors. See error code description in Intel UPI MCA 
       register for  error code description. 
       Format: {bit} - {hex mscod_code} {description}
       [0] - 0x00 UC Phy Initialization Failure (NumInit)
       [1] - 0x01 UC Phy Detected Drift Buffer Alarm 
       [2] - 0x02 UC Phy Detected Latency Buffer Rollover
       [3] - Reserved
       [4] - Reserved
       [5] - 0x10 UC LL Rx detected CRC error: unsuccessful LLR (entered Abort state)
       [6] - 0x11 UC LL Rx Unsupported/Undefined packet
       [7] - 0x12 UC LL or Phy Control Error
       [8] - 0x13 UC LL Rx Parameter Exception
       [9] - Reserved
       [10] - Reserved
       [11] - 0x1F UC LL Detected Control Error from M3UPI 
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIERRDIS1_KTI_LLPMON_STRUCT;


/* KTICORERRCNTDIS_KTI_LLPMON_REG supported on:                               */
/*       SKX_A0 (0x403700CC)                                                  */
/*       SKX (0x403700CC)                                                     */
/* Register default value:              0x00000000                            */
#define KTICORERRCNTDIS_KTI_LLPMON_REG 0x090040CC
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Correctable Error Count Disable
 * Allows BIOS to disable Correctable Error Counting (KTI_MC_ST.cor_err_cnt and 
 * BIOS_KTI_ERR_ST.cor_err_cnt) for each correctable error.  
 * NOTE: This has no affect on the KTIERRCNT0/1/2 error counters.
 * NOTE: Depending on how many MSCODs are disabled via this register, this may 
 * prevent CMCI triggered on KTI_MC_ST.cor_err_cnt, even if that feature is 
 * enabled. 
 */
typedef union {
  struct {
    UINT32 corerrcnt_mask : 10;
    /* corerrcnt_mask - Bits[9:0], RWS, default = 10'b0 
       Intel UPI Correctable Error Count Disable Mask
       NOTE: This has no affect on the KTIERRCNT0/1/2_CNTR error counters.
       If bit is set to 1 it disables incrementing of KTI_MC_ST.cor_err_cnt and/or 
       BIOS_KTI_ERR_ST.cor_err_cnt for a given error code.  See Intel UPI MCA register 
       for error code description. 
       Format: {bit} - {hex mscod_code} {description}
       [0] - 0x20 COR Phy Initialization Abort
       [1] - 0x21 COR Phy Reset
       [2] - 0x22 COR Phy Lane failure, recovery in x8 width
       [3] - 0x23 COR Phy L0c error corrected without Phy reset
       [4] - 0x24 COR Phy L0c error triggering Phy Reset
       [5] - 0x25 COR Phy L0p exit error triggering Phy Reset
       [6] - Reserved
       [7] - 0x30 COR LL Rx detected CRC error: successful LLR without Phy Reinit
       [8] - 0x31 COR LL Rx detected CRC error: successful LLR with Phy Reinit
       [9] - Reserved
     */
    UINT32 rsvd : 22;
    /* rsvd - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTICORERRCNTDIS_KTI_LLPMON_STRUCT;


/* KTIERRCNT0_MASK_KTI_LLPMON_REG supported on:                               */
/*       SKX_A0 (0x403700D0)                                                  */
/*       SKX (0x403700D0)                                                     */
/* Register default value:              0x00000000                            */
#define KTIERRCNT0_MASK_KTI_LLPMON_REG 0x090040D0
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Count 0 Mask
 * Register used for masking counter error cases in Intel UPI. Primary usage is to 
 * characterize correctable errors. 
 */
typedef union {
  struct {
    UINT32 mask : 22;
    /* mask - Bits[21:0], RWS, default = 22'b0 
       Error Mask
        This mask is used to select which bits are counted as part of error count. When 
       bit is set that error type is included in the Count. See Intel UPI MCA register 
       definition for  details on each error code. 
       Format: {bit} - {hex mscod_code} {description}
       [0] - 0x00 UC Phy Initialization Failure (NumInit)
       [1] - 0x01 UC Phy Detected Drift Buffer Alarm 
       [2] - 0x02 UC Phy Detected Latency Buffer Rollover
       [3] - Reserved
       [4] - Reserved
       [5] - 0x10 UC LL Rx detected CRC error: unsuccessful LLR (entered Abort state)
       [6] - 0x11 UC LL Rx Unsupported/Undefined packet
       [7] - 0x12 UC LL or Phy Control Error
       [8] - 0x13 UC LL Rx Parameter Exception
       [9] - Reserved
       [10] - Reserved
       [11] - 0x1F UC LL Detected Control Error from M3UPI 
       [12] - 0x20 COR Phy Initialization Abort
       [13] - 0x21 COR Phy Reset
       [14] - 0x22 COR Phy Lane failure, recovery in x8 width
       [15] - 0x23 COR Phy L0c error corrected without Phy reset
       [16] - 0x24 COR Phy L0c error triggering Phy Reset
       [17] - 0x25 COR Phy L0p exit error triggering Phy Reset
       [18] - Reserved
       [19] - 0x30 COR LL Rx detected CRC error: successful LLR without Phy Reinit
       [20] - 0x31 COR LL Rx detected CRC error: successful LLR with Phy Reinit
       [21] - Reserved
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIERRCNT0_MASK_KTI_LLPMON_STRUCT;


/* KTIERRCNT0_CNTR_KTI_LLPMON_REG supported on:                               */
/*       SKX_A0 (0x403700D4)                                                  */
/*       SKX (0x403700D4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIERRCNT0_CNTR_KTI_LLPMON_REG 0x090040D4
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Count 0 Counter
 * Register used for counting error cases in Intel UPI. Primary usage is to 
 * characterize correctable errors. 
 */
typedef union {
  struct {
    UINT32 counter : 15;
    /* counter - Bits[14:0], RWS_V, default = 15'b0 
       Counter increments based on error enabled in the Mask register.
       Counter is cleared by writing zeroes to both the 15-bit counter and 1-bit 
       overflow fields. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[30:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflow : 1;
    /* overflow - Bits[31:31], RWS_V, default = 1'b0 
       Overflow, remains sticky until cleared by software.
     */
  } Bits;
  UINT32 Data;
} KTIERRCNT0_CNTR_KTI_LLPMON_STRUCT;


/* KTIERRCNT1_MASK_KTI_LLPMON_REG supported on:                               */
/*       SKX_A0 (0x403700D8)                                                  */
/*       SKX (0x403700D8)                                                     */
/* Register default value:              0x00000000                            */
#define KTIERRCNT1_MASK_KTI_LLPMON_REG 0x090040D8
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Count 1 Mask
 * Register used for masking counter error cases in Intel UPI. Primary usage is to 
 * characterize correctable errors. 
 */
typedef union {
  struct {
    UINT32 mask : 22;
    /* mask - Bits[21:0], RWS, default = 22'b0 
       Error Mask
        This mask is used to select which bits are counted as part of error count. When 
       bit is set that error type is included in the Count. See Intel UPI MCA register 
       definition for  details on each error code. 
       Format: {bit} - {hex mscod_code} {description}
       [0] - 0x00 UC Phy Initialization Failure (NumInit)
       [1] - 0x01 UC Phy Detected Drift Buffer Alarm 
       [2] - 0x02 UC Phy Detected Latency Buffer Rollover
       [3] - Reserved
       [4] - Reserved
       [5] - 0x10 UC LL Rx detected CRC error: unsuccessful LLR (entered Abort state)
       [6] - 0x11 UC LL Rx Unsupported/Undefined packet
       [7] - 0x12 UC LL or Phy Control Error
       [8] - 0x13 UC LL Rx Parameter Exception
       [9] - Reserved
       [10] - Reserved
       [11] - 0x1F UC LL Detected Control Error from M3UPI 
       [12] - 0x20 COR Phy Initialization Abort
       [13] - 0x21 COR Phy Reset
       [14] - 0x22 COR Phy Lane failure, recovery in x8 width
       [15] - 0x23 COR Phy L0c error corrected without Phy reset
       [16] - 0x24 COR Phy L0c error triggering Phy Reset
       [17] - 0x25 COR Phy L0p exit error triggering Phy Reset
       [18] - Reserved
       [19] - 0x30 COR LL Rx detected CRC error: successful LLR without Phy Reinit
       [20] - 0x31 COR LL Rx detected CRC error: successful LLR with Phy Reinit
       [21] - Reserved
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIERRCNT1_MASK_KTI_LLPMON_STRUCT;


/* KTIERRCNT1_CNTR_KTI_LLPMON_REG supported on:                               */
/*       SKX_A0 (0x403700DC)                                                  */
/*       SKX (0x403700DC)                                                     */
/* Register default value:              0x00000000                            */
#define KTIERRCNT1_CNTR_KTI_LLPMON_REG 0x090040DC
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Count 1 Counter
 * Register used for counting error cases in Intel UPI. Primary usage is to 
 * characterize correctable errors. 
 */
typedef union {
  struct {
    UINT32 counter : 15;
    /* counter - Bits[14:0], RWS_V, default = 15'b0 
       Counter increments based on error enabled in the Mask register.
       Counter is cleared by writing zeroes to both the 15-bit counter and 1-bit 
       overflow fields. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[30:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflow : 1;
    /* overflow - Bits[31:31], RWS_V, default = 1'b0 
       Overflow, remains sticky until cleared by software.
     */
  } Bits;
  UINT32 Data;
} KTIERRCNT1_CNTR_KTI_LLPMON_STRUCT;


/* KTIERRCNT2_MASK_KTI_LLPMON_REG supported on:                               */
/*       SKX_A0 (0x403700E0)                                                  */
/*       SKX (0x403700E0)                                                     */
/* Register default value:              0x00000000                            */
#define KTIERRCNT2_MASK_KTI_LLPMON_REG 0x090040E0
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Count 2 Mask
 * Register used for masking counter error cases in Intel UPI. Primary usage is to 
 * characterize correctable errors. 
 */
typedef union {
  struct {
    UINT32 mask : 22;
    /* mask - Bits[21:0], RWS, default = 22'b0 
       Error Mask
        This mask is used to select which bits are counted as part of error count. When 
       bit is set that error type is included in the Count. See Intel UPI MCA register 
       definition for  details on each error code. 
       Format: {bit} - {hex mscod_code} {description}
       [0] - 0x00 UC Phy Initialization Failure (NumInit)
       [1] - 0x01 UC Phy Detected Drift Buffer Alarm 
       [2] - 0x02 UC Phy Detected Latency Buffer Rollover
       [3] - Reserved
       [4] - Reserved
       [5] - 0x10 UC LL Rx detected CRC error: unsuccessful LLR (entered Abort state)
       [6] - 0x11 UC LL Rx Unsupported/Undefined packet
       [7] - 0x12 UC LL or Phy Control Error
       [8] - 0x13 UC LL Rx Parameter Exception
       [9] - Reserved
       [10] - Reserved
       [11] - 0x1F UC LL Detected Control Error from M3UPI 
       [12] - 0x20 COR Phy Initialization Abort
       [13] - 0x21 COR Phy Reset
       [14] - 0x22 COR Phy Lane failure, recovery in x8 width
       [15] - 0x23 COR Phy L0c error corrected without Phy reset
       [16] - 0x24 COR Phy L0c error triggering Phy Reset
       [17] - 0x25 COR Phy L0p exit error triggering Phy Reset
       [18] - Reserved
       [19] - 0x30 COR LL Rx detected CRC error: successful LLR without Phy Reinit
       [20] - 0x31 COR LL Rx detected CRC error: successful LLR with Phy Reinit
       [21] - Reserved
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIERRCNT2_MASK_KTI_LLPMON_STRUCT;


/* KTIERRCNT2_CNTR_KTI_LLPMON_REG supported on:                               */
/*       SKX_A0 (0x403700E4)                                                  */
/*       SKX (0x403700E4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIERRCNT2_CNTR_KTI_LLPMON_REG 0x090040E4
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Error Count 2 Counter
 * Register used for counting error cases in Intel UPI. Primary usage is to 
 * characterize correctable errors. 
 */
typedef union {
  struct {
    UINT32 counter : 15;
    /* counter - Bits[14:0], RWS_V, default = 15'b0 
       Counter increments based on error enabled in the Mask register.
       Counter is cleared by writing zeroes to both the 15-bit counter and 1-bit 
       overflow fields. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[30:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflow : 1;
    /* overflow - Bits[31:31], RWS_V, default = 1'b0 
       Overflow, remains sticky until cleared by software.
     */
  } Bits;
  UINT32 Data;
} KTIERRCNT2_CNTR_KTI_LLPMON_STRUCT;


/* KTIAGCTRL_KTI_LLPMON_REG supported on:                                     */
/*       SKX_A0 (0x403700F0)                                                  */
/*       SKX (0x403700F0)                                                     */
/* Register default value on SKX_A0:    0x00101011                            */
/* Register default value on SKX:       0x00101010                            */
#define KTIAGCTRL_KTI_LLPMON_REG 0x090040F0
/* Struct format extracted from XML file SKX_A0\3.14.0.CFG.xml.
 * Intel UPI Agent Control
 */
typedef union {
  struct {
    UINT32 rbt_wr_enable : 1;
    /* rbt_wr_enable - Bits[0:0], RW, default = 1'b1  */
    UINT32 rbt_rd_enable : 1;
    /* rbt_rd_enable - Bits[1:1], RW, default = 1'b0 
       RBT Read Enable
       This mode enables the Rx RBT flow. It does not control writing to RBT on Tx 
       path. 
       0 - Disable
       1 - Enable
       By enabling RBT we cause the direct-to-core flow in Intel UPI. 
       
     */
    UINT32 remote_prefetch_mod3 : 2;
    /* remote_prefetch_mod3 - Bits[3:2], RW, default = 2'b00  */
    UINT32 tunnel_timeout : 3;
    /* tunnel_timeout - Bits[6:4], RW, default = 3'b001  */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 vna_threshold_llcrd : 8;
    /* vna_threshold_llcrd - Bits[15:8], RW, default = 8'd16 
       Threshold on number of VNA credits to accumulate before forcing a LLCRD message. 
        Recommended range: 16 to 66. 
     */
    UINT32 ack_threshold_llcrd : 8;
    /* ack_threshold_llcrd - Bits[23:16], RW, default = 8'd16 
       Threshold on number of ACK credits to accumulate before forcing a LLCRD message. 
        Recommended range: 16 to 66. 
     */
    UINT32 enableprefetch : 1;
    /* enableprefetch - Bits[24:24], RW, default = 1'b0 
       Prefetch enable for Remote Rd Types.
       When enabled this causes Remote Reads that match a SNC range to generate 
       MemSpecRd commands to the memory controller. The intent is to reduce latency for 
       remote reads. 
       Note: There is an identical copy of this field in M3UPI 
       M3KPRECTRL.EnablePrefetch that should be kept programmed identically to this. 
     */
    UINT32 fixed_field_reporting : 1;
    /* fixed_field_reporting - Bits[25:25], RW, default = 1'b0 
       The Intel UPI Spec defines some fields to have fixed values.  This register 
       controls how the link layer handles Rx detection of violations of these 
       definitions. 
       0: Trigger a CRC error (per Intel UPI Spec).
       1: Trigger a CRC error and an uncorrectable error.
     */
    UINT32 suppress_mem_rd_prefetch_rdcur : 1;
    /* suppress_mem_rd_prefetch_rdcur - Bits[26:26], RW, default = 1'b0 
       This bit defeatures Intel UPI generation of memory read prefetches for RdCur. 
       The default value is such that Intel UPI prefetch for RdCur is enabled. For 
       platforms that expect high speed and low latency IO devices consuming data where 
       data would be in the producer's cache (CPU or peer IO), this bit could be used 
       to disable Intel UPI prefetch for RdCur to reduce wasteful memory BW usage. For 
       platforms that do not have this tightly coupled IO behavior it is likely better 
       to allow the prefetch to save latency from memory. Customers should be able to 
       use a BIOS provided knob to experiment with this bit to determine how they want 
       to set it. 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIAGCTRL_KTI_LLPMON_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Agent Control
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], RW, default = 1'b0  */
    UINT32 rbt_rd_enable : 1;
    /* rbt_rd_enable - Bits[1:1], RW, default = 1'b0 
       RBT Read Enable
       This mode enables the Rx RBT flow. It does not control writing to RBT on Tx 
       path. 
       0 - Disable
       1 - Enable
       By enabling RBT reads, we cause the direct-to-core flow in Intel UPI. 
       
     */
    UINT32 remote_prefetch_mod3 : 2;
    /* remote_prefetch_mod3 - Bits[3:2], RW, default = 2'b00  */
    UINT32 tunnel_timeout : 3;
    /* tunnel_timeout - Bits[6:4], RW, default = 3'b001  */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 vna_threshold_llcrd : 8;
    /* vna_threshold_llcrd - Bits[15:8], RW, default = 8'h10 
       Threshold on number of VNA credits to accumulate before forcing a LLCRD message. 
        Recommended range: 16 to 66. 
     */
    UINT32 ack_threshold_llcrd : 8;
    /* ack_threshold_llcrd - Bits[23:16], RW, default = 8'h10 
       Threshold on number of ACK credits to accumulate before forcing a LLCRD message. 
        Recommended range: 16 to 66. 
     */
    UINT32 enableprefetch : 1;
    /* enableprefetch - Bits[24:24], RW, default = 1'b0 
       Prefetch enable for Remote Rd Types.
       When enabled this causes Remote Reads that match a SNC range to generate 
       MemSpecRd commands to the memory controller. The intent is to reduce latency for 
       remote reads. 
       Note: There is an identical copy of this field in M3UPI 
       M3KPRECTRL.EnablePrefetch that should be kept programmed identically to this. 
     */
    UINT32 fixed_field_reporting : 1;
    /* fixed_field_reporting - Bits[25:25], RW, default = 1'b0 
       The Intel UPI Spec defines some fields to have fixed values.  This register 
       controls how the link layer handles Rx detection of violations of these 
       definitions. 
       0 - Trigger a CRC error (per Intel UPI Spec).
       1 - Trigger a CRC error and an uncorrectable error.
     */
    UINT32 suppress_mem_rd_prefetch_rdcur : 1;
    /* suppress_mem_rd_prefetch_rdcur - Bits[26:26], RW, default = 1'b0 
       This bit defeatures Intel UPI generation of memory read prefetches for RdCur. 
       The default value is such that Intel UPI prefetch for RdCur is enabled. For 
       platforms that expect high speed and low latency IO devices consuming data where 
       data would be in the producer's cache (CPU or peer IO), this bit could be used 
       to disable Intel UPI prefetch for RdCur to reduce wasteful memory BW usage. For 
       platforms that do not have this tightly coupled IO behavior it is likely better 
       to allow the prefetch to save latency from memory. Customers should be able to 
       use a BIOS provided knob to experiment with this bit to determine how they want 
       to set it. 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIAGCTRL_KTI_LLPMON_STRUCT;



/* KTIA2RCRCTRL_KTI_LLPMON_REG supported on:                                  */
/*       SKX_A0 (0x403700F4)                                                  */
/*       SKX (0x403700F4)                                                     */
/* Register default value:              0x000104A5                            */
#define KTIA2RCRCTRL_KTI_LLPMON_REG 0x090040F4
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Agent to Ring Credit Control
 * Register used to control credits flow from Intel UPI agent to M3UPI targeting 
 * the ring. The values in this register take affect on next Link Layer 
 * initialization, which should normally occur as part of Warm Reset during the 
 * platform reset.  
 * The following agent-to-ring credits are hard-wired to 1 credit each for VN0 and, 
 * if VN1 is enabled, 1 credit each for VN1: ad_req, ad_wb, bl_wb, bl_ncb, bl_ncs. 
 * Current values are represented in KTIA2RCRCTRL_ACTIVE.
 */
typedef union {
  struct {
    UINT32 ad_vna : 6;
    /* ad_vna - Bits[5:0], RWS, default = 6'h25 
       Credits given to the Intel UPI link layer to send packets from any VN and a AD 
       message class to the M3UPI AD flow control queue.   
       	Link layer consumes these before using VN0/1 and AD MC-specific credits.
       Legal range of values: 6'd0 to 6'd37, but total across all applicable credit 
       types must not exceed the size of the respective FlowQ. 
     */
    UINT32 bl_vna : 5;
    /* bl_vna - Bits[10:6], RWS, default = 5'h12 
       Credits given to the Intel UPI link layer to send packets from any VN and a BL 
       message class to the M3UPI BL flow control queue.   
       	Link layer consumes these before using VN0/1 and BL MC-specific credits.
       Legal range of values: 5'd0 to 5'd18, but total across all applicable credit 
       types must not exceed the size of the respective FlowQ. 
     */
    UINT32 ak_vna : 6;
    /* ak_vna - Bits[16:11], RWS, default = 6'h20 
       Credits given to the Intel UPI link layer to send packets from any AK message 
       class to the M3UPI AK flow control queue.   
       Note that for AK messages VN0 and VN1 don't apply since all messages are 
       guaranteed to sink in local destinations, so no need to track VN0/1 credits. 
       Legal range of values: If VN1 enabled then range is 6'd4 to 6'd32.  If VN1 
       disabled then range is 6'd2 to 6'd32.  In either case, the total across all 
       applicable credit types must not exceed the size of the respective FlowQ. 
     */
    UINT32 rsvd_17 : 1;
    /* rsvd_17 - Bits[17:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ad_snp : 3;
    /* ad_snp - Bits[20:18], RWS, default = 3'h0 
       Credits +1 given to the Intel UPI link layer to send AD SNP VN0 packets to the 
       M3UPI AD flow control queue.   
        If VN1 is enabled, LL additionally gets this number of credits to send AD SNP 
       VN1 to AD flow control queue. 
       VN0/1 CRD encodings are ""value + 1"" (0=1CRD, 1=2CRD...). Value takes affect on 
       next Link Layer Reset. 
       Legal range of values: 3'd0 to 3'd6 (3'd7 is not allowed to avoid counts going 
       beyond 3-bits) but total across all applicable credit types must not exceed the 
       size of the respective FlowQ. 
     */
    UINT32 ad_rsp : 3;
    /* ad_rsp - Bits[23:21], RWS, default = 3'h0 
       Credits +1 given to the Intel UPI link layer to send AD RSP VN0 packets to the 
       M3UPI AD flow control queue.   
        If VN1 is enabled, LL additionally gets this number of credits to send AD RSP 
       VN1 to R3 AD flow control queue. 
       VN0/1 CRD encodings are ""value + 1"" (0=1CRD, 1=2CRD...). Value takes affect on 
       next Link Layer Reset. 
       Legal range of values: 3'd0 to 3'd6 (3'd7 is not allowed to avoid counts going 
       beyond 3-bits) but total across all applicable credit types must not exceed the 
       size of the respective FlowQ. 
     */
    UINT32 bl_rsp : 3;
    /* bl_rsp - Bits[26:24], RWS, default = 3'h0 
       Credits +1 given to the Intel UPI link layer to send BL RSP VN0 packets to the 
       M3UPI BL flow control queue.   
        If VN1 is enabled, LL additionally gets this number of credits to send BL RSP 
       VN1 to R3 BL flow control queue. 
       VN0/1 CRD encodings are ""value + 1"" (0=1CRD, 1=2CRD...). Value takes affect on 
       next Link Layer Reset. 
       Legal range of values: 3'd0 to 3'd6 (3'd7 is not allowed to avoid counts going 
       beyond 3-bits) but total across all applicable credit types must not exceed the 
       size of the respective FlowQ. 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIA2RCRCTRL_KTI_LLPMON_STRUCT;


/* KTIQRT_KTI_LLPMON_REG supported on:                                        */
/*       SKX_A0 (0x403700F8)                                                  */
/*       SKX (0x403700F8)                                                     */
/* Register default value:              0x0000FFFF                            */
#define KTIQRT_KTI_LLPMON_REG 0x090040F8
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Agent Intel UPI Routing Table
 * Control Routing from Intel UPI Local vs. Remote Determination
 */
typedef union {
  struct {
    UINT32 routeskt : 16;
    /* routeskt - Bits[15:0], RW_LB, default = 16'hFFFF  */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIQRT_KTI_LLPMON_STRUCT;


/* KTICTRTXPF_KTI_LLPMON_REG supported on:                                    */
/*       SKX_A0 (0x40370100)                                                  */
/*       SKX (0x40370108)                                                     */
/* Register default value:              0x00000000                            */
#define KTICTRTXPF_KTI_LLPMON_REG 0x0900C000
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Counter Tx Protocol Flits
 * Register used by P limit and power meter. 
 */
typedef union {
  struct {
    UINT32 count_tx_protocol_flit : 32;
    /* count_tx_protocol_flit - Bits[31:0], RO_V, default = 32'b0 
       Free running count of total Protocol flits in Tx. This count does not include 
       link layer only flits (NULL, Parameters, control, etc.).  
     */
  } Bits;
  UINT32 Data;
} KTICTRTXPF_KTI_LLPMON_STRUCT;


/* KTICTRRXPF_KTI_LLPMON_REG supported on:                                    */
/*       SKX_A0 (0x40370104)                                                  */
/*       SKX (0x4037010C)                                                     */
/* Register default value:              0x00000000                            */
#define KTICTRRXPF_KTI_LLPMON_REG 0x0900C001
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Counter Rx Protocol Flits
 * Register used by P limit and power meter. 
 */
typedef union {
  struct {
    UINT32 count_rx_protocol_flit : 32;
    /* count_rx_protocol_flit - Bits[31:0], RO_V, default = 32'b0 
       Free running count of total Protocol flits in Rx. This count does not include 
       link layer only flits (NULL, Parameters, control, etc.).  
     */
  } Bits;
  UINT32 Data;
} KTICTRRXPF_KTI_LLPMON_STRUCT;


/* KTICTRLINKCLK_KTI_LLPMON_REG supported on:                                 */
/*       SKX_A0 (0x40370108)                                                  */
/*       SKX (0x40370110)                                                     */
/* Register default value:              0x00000000                            */
#define KTICTRLINKCLK_KTI_LLPMON_REG 0x0900C002
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Link Clock (KFCLK) Counter
 * Counts KFCLK (8UI) cycles when not gated for L1+Vret.
 */
typedef union {
  struct {
    UINT32 count_linkclk : 32;
    /* count_linkclk - Bits[31:0], RO_V, default = 31'b0 
       Free running count of KFClk (8 UI link layer clock).
     */
  } Bits;
  UINT32 Data;
} KTICTRLINKCLK_KTI_LLPMON_STRUCT;


/* KTICTRL1_KTI_LLPMON_REG supported on:                                      */
/*       SKX_A0 (0x4037010C)                                                  */
/*       SKX (0x40370114)                                                     */
/* Register default value:              0x00000000                            */
#define KTICTRL1_KTI_LLPMON_REG 0x0900C003
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Counter L1
 * Register used by P limit and power meter.
 */
typedef union {
  struct {
    UINT32 count_l1 : 23;
    /* count_l1 - Bits[22:0], RO_V, default = 23'b0 
       Free running count of KFClk (8 UI link layer clock) in L1 link state.
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTICTRL1_KTI_LLPMON_STRUCT;


/* KTICTRRXL0P_KTI_LLPMON_REG supported on:                                   */
/*       SKX_A0 (0x40370110)                                                  */
/*       SKX (0x40370118)                                                     */
/* Register default value:              0x00000000                            */
#define KTICTRRXL0P_KTI_LLPMON_REG 0x0900C004
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Counter Rx L0p
 * Register used by P limit and power meter. 
 */
typedef union {
  struct {
    UINT32 count_rx_l0p : 23;
    /* count_rx_l0p - Bits[22:0], RO_V, default = 23'b0 
       Free running count of KFClk (8 UI link layer clock) in Rx side link in L0p 
       state. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTICTRRXL0P_KTI_LLPMON_STRUCT;


/* KTICTRTXL0P_KTI_LLPMON_REG supported on:                                   */
/*       SKX_A0 (0x40370114)                                                  */
/*       SKX (0x4037011C)                                                     */
/* Register default value:              0x00000000                            */
#define KTICTRTXL0P_KTI_LLPMON_REG 0x0900C005
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Counter Tx L0p
 * Register used by P limit and power meter. 
 */
typedef union {
  struct {
    UINT32 count_tx_l0p : 23;
    /* count_tx_l0p - Bits[22:0], RO_V, default = 23'b0 
       Free running count of KFClk (8 UI link layer clock) in Tx side link in L0p 
       state. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTICTRTXL0P_KTI_LLPMON_STRUCT;


/* KTIPCSTS_KTI_LLPMON_REG supported on:                                      */
/*       SKX_A0 (0x40370120)                                                  */
/*       SKX (0x40370120)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPCSTS_KTI_LLPMON_REG 0x09004120
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pcode Status
 * Register is used by PCode to store link training status.
 */
typedef union {
  struct {
    UINT32 ll_status : 4;
    /* ll_status - Bits[3:0], RW, default = 4'b0 
       Link Layer status registers that is written by Pcode. Encoding is the same as 
       KTILS.Init_State. 
     */
    UINT32 ll_status_valid : 1;
    /* ll_status_valid - Bits[4:4], RW, default = 4'b0 
       Bit indicates valid training status logged from PCode to BIOS.
       
     */
    UINT32 rsvd_5 : 27;
  } Bits;
  UINT32 Data;
} KTIPCSTS_KTI_LLPMON_STRUCT;


/* KTILK_TAD_MMIOH0_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370130)                                                  */
/*       SKX (0x40370130)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_MMIOH0_KTI_LLPMON_REG 0x09004130
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Defines base and limit of MMIOH ranges. Base for this range is hard coded to 
 * 4GB. Values are RW-LB (LB needed for security of all SAD/TAD/Routing related 
 * registers). Each limit corresponds to target IIO configured in the Target List. 
 */
typedef union {
  struct {
    UINT32 mmioh_target0 : 3;
    /* mmioh_target0 - Bits[2:0], RW_LB, default = 3'b0 
       Target IIO for range 0.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
     */
    UINT32 mmioh_target1 : 3;
    /* mmioh_target1 - Bits[5:3], RW_LB, default = 3'b0 
       Target IIO for range 1.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
     */
    UINT32 mmioh_target2 : 3;
    /* mmioh_target2 - Bits[8:6], RW_LB, default = 3'b0 
       Target IIO for range 2.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
     */
    UINT32 mmioh_target3 : 3;
    /* mmioh_target3 - Bits[11:9], RW_LB, default = 3'b0 
       Target IIO for range 3.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
     */
    UINT32 mmioh_target4 : 3;
    /* mmioh_target4 - Bits[14:12], RW_LB, default = 3'b0 
       Target IIO for range 4.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
     */
    UINT32 mmioh_target5 : 3;
    /* mmioh_target5 - Bits[17:15], RW_LB, default = 3'b0 
       Target IIO for range 5.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_MMIOH0_KTI_LLPMON_STRUCT;


/* KTILK_TAD_MMIOH1_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370134)                                                  */
/*       SKX (0x40370134)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_MMIOH1_KTI_LLPMON_REG 0x09004134
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see MMIOH0
 */
typedef union {
  struct {
    UINT32 mmioh_limit0 : 16;
    /* mmioh_limit0 - Bits[15:0], RW_LB, default = 16'b0 
       PA[45:30]
     */
    UINT32 mmioh_limit1 : 16;
    /* mmioh_limit1 - Bits[31:16], RW_LB, default = 16'b0 
       PA[45:30]
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_MMIOH1_KTI_LLPMON_STRUCT;


/* KTILK_TAD_MMIOH2_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370138)                                                  */
/*       SKX (0x40370138)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_MMIOH2_KTI_LLPMON_REG 0x09004138
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see MMIOH0
 */
typedef union {
  struct {
    UINT32 mmioh_limit2 : 16;
    /* mmioh_limit2 - Bits[15:0], RW_LB, default = 16'b0 
       PA[45:30]
     */
    UINT32 mmioh_limit3 : 16;
    /* mmioh_limit3 - Bits[31:16], RW_LB, default = 16'b0 
       PA[45:30]
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_MMIOH2_KTI_LLPMON_STRUCT;


/* KTILK_TAD_MMIOH3_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x4037013C)                                                  */
/*       SKX (0x4037013C)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_MMIOH3_KTI_LLPMON_REG 0x0900413C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see MMIOH0
 */
typedef union {
  struct {
    UINT32 mmioh_limit4 : 16;
    /* mmioh_limit4 - Bits[15:0], RW_LB, default = 16'b0 
       PA[45:30]
     */
    UINT32 mmioh_limit5 : 16;
    /* mmioh_limit5 - Bits[31:16], RW_LB, default = 16'b0 
       PA[45:30]
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_MMIOH3_KTI_LLPMON_STRUCT;


/* KTILK_TAD_MMIOL0_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370140)                                                  */
/*       SKX (0x40370140)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_MMIOL0_KTI_LLPMON_REG 0x09004140
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Defines targets and limits of MMIOL ranges. With base for this range hard coded 
 * to 0. Upper Addr bits 45:32 are assumed zero for MMIOL which is limited to less 
 * than 4GB.  Values are RW-LB (LB needed for security of all SAD/TAD/Routing 
 * related registers). Each limit corresponds to target IIO configured in the 
 * Target List. 
 */
typedef union {
  struct {
    UINT32 mmiol_target0 : 3;
    /* mmiol_target0 - Bits[2:0], RW_LB, default = 3'b0 
       Target IIO for range 0.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 mmiol_target1 : 3;
    /* mmiol_target1 - Bits[5:3], RW_LB, default = 3'b0 
       Target IIO for range 1.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 mmiol_target2 : 3;
    /* mmiol_target2 - Bits[8:6], RW_LB, default = 3'b0 
       Target IIO for range 2.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 mmiol_target3 : 3;
    /* mmiol_target3 - Bits[11:9], RW_LB, default = 3'b0 
       Target IIO for range 3.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 mmiol_target4 : 3;
    /* mmiol_target4 - Bits[14:12], RW_LB, default = 3'b0 
       Target IIO for range 4.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 mmiol_target5 : 3;
    /* mmiol_target5 - Bits[17:15], RW_LB, default = 3'b0 
       Target IIO for range 5.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_MMIOL0_KTI_LLPMON_STRUCT;


/* KTILK_TAD_MMIOL1_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370144)                                                  */
/*       SKX (0x40370144)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_MMIOL1_KTI_LLPMON_REG 0x09004144
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see MMIOHL0
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 6;
    /* rsvd_0 - Bits[5:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mmiol_limit0 : 10;
    /* mmiol_limit0 - Bits[15:6], RW_LB, default = 10'b0 
       PA[31:22]
     */
    UINT32 rsvd_16 : 6;
    /* rsvd_16 - Bits[21:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mmiol_limit1 : 10;
    /* mmiol_limit1 - Bits[31:22], RW_LB, default = 10'b0 
       PA[31:22]
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_MMIOL1_KTI_LLPMON_STRUCT;


/* KTILK_TAD_MMIOL2_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370148)                                                  */
/*       SKX (0x40370148)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_MMIOL2_KTI_LLPMON_REG 0x09004148
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see MMIOHL0
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 6;
    /* rsvd_0 - Bits[5:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mmiol_limit2 : 10;
    /* mmiol_limit2 - Bits[15:6], RW_LB, default = 10'b0 
       PA[31:22]
     */
    UINT32 rsvd_16 : 6;
    /* rsvd_16 - Bits[21:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mmiol_limit3 : 10;
    /* mmiol_limit3 - Bits[31:22], RW_LB, default = 10'b0 
       PA[31:22]
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_MMIOL2_KTI_LLPMON_STRUCT;


/* KTILK_TAD_MMIOL3_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x4037014C)                                                  */
/*       SKX (0x4037014C)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_MMIOL3_KTI_LLPMON_REG 0x0900414C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see MMIOHL0
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 6;
    /* rsvd_0 - Bits[5:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mmiol_limit4 : 10;
    /* mmiol_limit4 - Bits[15:6], RW_LB, default = 10'b0 
       PA[31:22]
     */
    UINT32 rsvd_16 : 6;
    /* rsvd_16 - Bits[21:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mmiol_limit5 : 10;
    /* mmiol_limit5 - Bits[31:22], RW_LB, default = 10'b0 
       PA[31:22]
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_MMIOL3_KTI_LLPMON_STRUCT;


/* KTILK_TAD_CFG0_KTI_LLPMON_REG supported on:                                */
/*       SKX_A0 (0x40370150)                                                  */
/*       SKX (0x40370150)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_CFG0_KTI_LLPMON_REG 0x09004150
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Defines targets, base, and limits of CFG ranges. Values are RW-LB (LB needed for 
 * security of all SAD/TAD/Routing related registers). Each limit corresponds to a 
 * target IIO which is controlled by the Target list. 
 */
typedef union {
  struct {
    UINT32 cfg_target0 : 3;
    /* cfg_target0 - Bits[2:0], RW_LB, default = 3'b0 
       Target IIO for range 0.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 cfg_target1 : 3;
    /* cfg_target1 - Bits[5:3], RW_LB, default = 3'b0 
       Target IIO for range 1.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 cfg_target2 : 3;
    /* cfg_target2 - Bits[8:6], RW_LB, default = 3'b0 
       Target IIO for range 2.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 cfg_target3 : 3;
    /* cfg_target3 - Bits[11:9], RW_LB, default = 3'b0 
       Target IIO for range 3.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 cfg_target4 : 3;
    /* cfg_target4 - Bits[14:12], RW_LB, default = 3'b0 
       Target IIO for range 4.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 cfg_target5 : 3;
    /* cfg_target5 - Bits[17:15], RW_LB, default = 3'b0 
       Target IIO for range 5.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 rsvd_18 : 6;
    /* rsvd_18 - Bits[23:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 enable_split_iio : 6;
    /* enable_split_iio - Bits[29:24], RW_LB, default = 6'b0 
       Enables Dev16-31 (1-bit x 6) for corresponding IIO target (IIO[5:0]) - When set 
       causes RootBus Dev16-31 to be sent to corresponding IIO instead of UBox. Default 
       value is 'b000000. Under normal conditions with DMI port active value should be 
       set to 'b000001. Bit 0 - IIO0, bit 1 - IIO1, etc.. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_CFG0_KTI_LLPMON_STRUCT;


/* KTILK_TAD_CFG1_KTI_LLPMON_REG supported on:                                */
/*       SKX_A0 (0x40370154)                                                  */
/*       SKX (0x40370154)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_CFG1_KTI_LLPMON_REG 0x09004154
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see KTILK_TAD_CFG0
 */
typedef union {
  struct {
    UINT32 cfg_base_bus_num : 8;
    /* cfg_base_bus_num - Bits[7:0], RW_LB, default = 8'b0 
       Base Bus # defined by Addr[27:20] in NcCfg*.
       Base value must be set accurately before any limit values are set. Note: Global 
       address map requires each socket CFG space be on granularity be 32 bus#, so base 
       must have lower 5 bits set to zero. 
     */
    UINT32 cfg_limit0_bus_num : 8;
    /* cfg_limit0_bus_num - Bits[15:8], RW_LB, default = 8'b0 
       Bus# limit0: Bus# is defined by Addr[27:20]
     */
    UINT32 cfg_limit1_bus_num : 8;
    /* cfg_limit1_bus_num - Bits[23:16], RW_LB, default = 8'b0 
       Bus# limit1: Bus# is defined by Addr[27:20]
     */
    UINT32 cfg_limit2_bus_num : 8;
    /* cfg_limit2_bus_num - Bits[31:24], RW_LB, default = 8'b0 
       Bus# limit2: Bus# is defined by Addr[27:20]
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_CFG1_KTI_LLPMON_STRUCT;


/* KTILK_TAD_CFG2_KTI_LLPMON_REG supported on:                                */
/*       SKX_A0 (0x40370158)                                                  */
/*       SKX (0x40370158)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_CFG2_KTI_LLPMON_REG 0x09004158
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see KTILK_TAD_CFG0
 */
typedef union {
  struct {
    UINT32 cfg_limit3_bus_num : 8;
    /* cfg_limit3_bus_num - Bits[7:0], RW_LB, default = 8'b0 
       Bus# limit3: Bus# is defined by Addr[27:20]
     */
    UINT32 cfg_limit4_bus_num : 8;
    /* cfg_limit4_bus_num - Bits[15:8], RW_LB, default = 8'b0 
       Bus# limit4: Bus# is defined by Addr[27:20]
     */
    UINT32 cfg_limit5_bus_num : 8;
    /* cfg_limit5_bus_num - Bits[23:16], RW_LB, default = 8'b0 
       Bus# limit5: Bus# is defined by Addr[27:20]
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_CFG2_KTI_LLPMON_STRUCT;


/* KTILK_TAD_LEG_KTI_LLPMON_REG supported on:                                 */
/*       SKX_A0 (0x4037015C)                                                  */
/*       SKX (0x4037015C)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_LEG_KTI_LLPMON_REG 0x0900415C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Decode for special non-coherent targets.
 */
typedef union {
  struct {
    UINT32 vga_enable : 1;
    /* vga_enable - Bits[0:0], RW_LB, default = 8'b0 
       Enables VGA range to be match (applies to both MMIO space and Legacy_IO space 
       ranges). 
     */
    UINT32 vga_target : 3;
    /* vga_target - Bits[3:1], RW_LB, default = 8'b0 
       3-bit value to indicate IIO 0-5. Values 6,7 are RSVD encodings
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_LEG_KTI_LLPMON_STRUCT;


/* KTILK_TAD_IO0_KTI_LLPMON_REG supported on:                                 */
/*       SKX_A0 (0x40370160)                                                  */
/*       SKX (0x40370160)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_IO0_KTI_LLPMON_REG 0x09004160
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Legacy IO TAD. Base Address is hard coded to zero.
 */
typedef union {
  struct {
    UINT32 legio_target0 : 3;
    /* legio_target0 - Bits[2:0], RW_LB, default = 3'b0 
       Target IIO for range 0.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 legio_target1 : 3;
    /* legio_target1 - Bits[5:3], RW_LB, default = 3'b0 
       Target IIO for range 1.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 legio_target2 : 3;
    /* legio_target2 - Bits[8:6], RW_LB, default = 3'b0 
       Target IIO for range 2.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 legio_target3 : 3;
    /* legio_target3 - Bits[11:9], RW_LB, default = 3'b0 
       Target IIO for range 3.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 legio_target4 : 3;
    /* legio_target4 - Bits[14:12], RW_LB, default = 3'b0 
       Target IIO for range 4.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 legio_target5 : 3;
    /* legio_target5 - Bits[17:15], RW_LB, default = 3'b0 
       Target IIO for range 5.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_IO0_KTI_LLPMON_STRUCT;


/* KTILK_TAD_IO1_KTI_LLPMON_REG supported on:                                 */
/*       SKX_A0 (0x40370164)                                                  */
/*       SKX (0x40370164)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_IO1_KTI_LLPMON_REG 0x09004164
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see KTILK_TAD_IO0
 */
typedef union {
  struct {
    UINT32 legio_base : 6;
    /* legio_base - Bits[5:0], RW_LB, default = 6'b0 
       Base Addr(15:10)
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 legio_limit0 : 6;
    /* legio_limit0 - Bits[13:8], RW_LB, default = 6'b0 
       Range 0 Limit Addr(15:10)
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 legio_limit1 : 6;
    /* legio_limit1 - Bits[21:16], RW_LB, default = 6'b0 
       Range 1 Limit Addr(15:10)
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 legio_limit2 : 6;
    /* legio_limit2 - Bits[29:24], RW_LB, default = 6'b0 
       Range 2 Limit Addr(15:10)
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_IO1_KTI_LLPMON_STRUCT;


/* KTILK_TAD_IO2_KTI_LLPMON_REG supported on:                                 */
/*       SKX_A0 (0x40370168)                                                  */
/*       SKX (0x40370168)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_IO2_KTI_LLPMON_REG 0x09004168
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see KTILK_TAD_IO0
 */
typedef union {
  struct {
    UINT32 legio_limit3 : 6;
    /* legio_limit3 - Bits[5:0], RW_LB, default = 6'b0 
       Range 3 Limit Addr(15:10)
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 legio_limit4 : 6;
    /* legio_limit4 - Bits[13:8], RW_LB, default = 6'b0 
       Range 4 Limit Addr(15:10)
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 legio_limit5 : 6;
    /* legio_limit5 - Bits[21:16], RW_LB, default = 6'b0 
       Range 5 Limit Addr(15:10)
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_IO2_KTI_LLPMON_STRUCT;


/* KTILK_TAD_IOAPIC0_KTI_LLPMON_REG supported on:                             */
/*       SKX_A0 (0x4037016C)                                                  */
/*       SKX (0x4037016C)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_IOAPIC0_KTI_LLPMON_REG 0x0900416C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * IOAPIC space interleave across the Ranges IOAPIC is a fixed ranged FEC0_0000 to 
 * FECF_FFFF. So base is fixed to the bottom of that range. 
 */
typedef union {
  struct {
    UINT32 ioapic_target0 : 3;
    /* ioapic_target0 - Bits[2:0], RW_LB, default = 3'b0 
       Target IIO for range 0.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 ioapic_target1 : 3;
    /* ioapic_target1 - Bits[5:3], RW_LB, default = 3'b0 
       Target IIO for range 1.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 ioapic_target2 : 3;
    /* ioapic_target2 - Bits[8:6], RW_LB, default = 3'b0 
       Target IIO for range 2.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 ioapic_target3 : 3;
    /* ioapic_target3 - Bits[11:9], RW_LB, default = 3'b0 
       Target IIO for range 3.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 ioapic_target4 : 3;
    /* ioapic_target4 - Bits[14:12], RW_LB, default = 3'b0 
       Target IIO for range 4.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 ioapic_target5 : 3;
    /* ioapic_target5 - Bits[17:15], RW_LB, default = 3'b0 
       Target IIO for range 5.
       3-bits for each target to indicate IIO0, IIO1, .. IIO5 (only 6 encodings are 
       defined). 
       
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_IOAPIC0_KTI_LLPMON_STRUCT;


/* KTILK_TAD_IOAPIC1_KTI_LLPMON_REG supported on:                             */
/*       SKX_A0 (0x40370170)                                                  */
/*       SKX (0x40370170)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_IOAPIC1_KTI_LLPMON_REG 0x09004170
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see KTILK_TAD_IOAPIC0
 */
typedef union {
  struct {
    UINT32 ioapic_base : 5;
    /* ioapic_base - Bits[4:0], RW_LB, default = 5'b0 
       IOAPIC Base Addr[19:15]
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ioapic_limit0 : 5;
    /* ioapic_limit0 - Bits[12:8], RW_LB, default = 5'b0 
       IIO Range 0 Limit Addr[19:15]
     */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ioapic_limit1 : 5;
    /* ioapic_limit1 - Bits[20:16], RW_LB, default = 5'b0 
       IIO Range 1 Limit Addr[19:15]
     */
    UINT32 rsvd_21 : 3;
    /* rsvd_21 - Bits[23:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ioapic_limit2 : 5;
    /* ioapic_limit2 - Bits[28:24], RW_LB, default = 5'b0 
       IIO Range 2 Limit Addr[19:15]
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_IOAPIC1_KTI_LLPMON_STRUCT;


/* KTILK_TAD_IOAPIC2_KTI_LLPMON_REG supported on:                             */
/*       SKX_A0 (0x40370174)                                                  */
/*       SKX (0x40370174)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_TAD_IOAPIC2_KTI_LLPMON_REG 0x09004174
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * see KTILK_TAD_IOAPIC0
 */
typedef union {
  struct {
    UINT32 ioapic_limit3 : 5;
    /* ioapic_limit3 - Bits[4:0], RW_LB, default = 5'b0 
       IIO Range 3 Limit Addr[19:15]
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ioapic_limit4 : 5;
    /* ioapic_limit4 - Bits[12:8], RW_LB, default = 5'b0 
       IIO Range 4 Limit Addr[19:15]
     */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ioapic_limit5 : 5;
    /* ioapic_limit5 - Bits[20:16], RW_LB, default = 5'b0 
       IIO Range 5 Limit Addr[19:15]
     */
    UINT32 rsvd_21 : 11;
    /* rsvd_21 - Bits[31:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_TAD_IOAPIC2_KTI_LLPMON_STRUCT;


/* KTILK_SNC_CONFIG_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370180)                                                  */
/*       SKX (0x40370180)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_SNC_CONFIG_KTI_LLPMON_REG 0x09004180
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Sub NUMA Clustering (SNC) Configuration.
 */
typedef union {
  struct {
    UINT32 full_snc_en : 1;
    /* full_snc_en - Bits[0:0], RWS_LB, default = 1'b0 
       Enables SNC.
        Each individual range requires legal address range setting prior to setting the 
       enable bit. 
     */
    UINT32 snc_ind_en : 1;
    /* snc_ind_en - Bits[1:1], RWS_LB, default = 1'b0 
       Enables SNC range matching. 
       For Intel UPI this is just AND'ed with ""Full_SNC_En"" to enable the SNC 
       feature. If either this bit or Full_SNC_en is not set then SNC will not be 
       enabled. 
       
     */
    UINT32 num_snc_clu : 2;
    /* num_snc_clu - Bits[3:2], RWS_LB, default = 5'b0 
       Number SNC Clusters
       Limits the number of cluster ranges that can be matched.
       00 - 2 SNC clusters
       01 - 3 SNC clusters
       10 - 4 SNC clusters
       11 - RSVD
     */
    UINT32 lowest_cbo_id_clus1 : 5;
    /* lowest_cbo_id_clus1 - Bits[8:4], RWS_LB, default = 5'b0 
       Defines the first CHA for the SNC range 1. 
       The CHA targets used in SNC range 1 is defined from this base value to 
       (Lowest_Cbo_ID_Clus2-1).  
       This value also implies the Max CHA value of SNC range 0, which is this value 
       minus 1. 
       Also note that the ""base"" value for SNC range 0 is hard coded to 0.
     */
    UINT32 lowest_cbo_id_clus2 : 5;
    /* lowest_cbo_id_clus2 - Bits[13:9], RWS_LB, default = 5'b0 
       Defines the first CHA for the SNC range 2. 
       The CHA targets used in SNC range 2 is defined from this base value to 
       (Lowest_Cbo_ID_Clus3-1).  
       This value also implies the Max CHA value of SNC range 1, which is this value 
       minus 1. 
     */
    UINT32 lowest_cbo_id_clus3 : 5;
    /* lowest_cbo_id_clus3 - Bits[18:14], RWS_LB, default = 5'b0 
       Defines the first CHA for the SNC range 3. 
       The CHA targets used in SNC range 3 is defined from this base value to 
       (cha_cnt-1). The cha_cnt is hard coded in every SKU. 
       This value also implies the Max CHA value of SNC range 2, which is this value 
       minus 1. 
     */
    UINT32 rsvd : 13;
    /* rsvd - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_SNC_CONFIG_KTI_LLPMON_STRUCT;


/* KTILK_SNC_BASE_1_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370184)                                                  */
/*       SKX (0x40370184)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_SNC_BASE_1_KTI_LLPMON_REG 0x09004184
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Address decode for Sub NUMA Clustering (SNC)
 */
typedef union {
  struct {
    UINT32 starting_addr_0 : 16;
    /* starting_addr_0 - Bits[15:0], RWS_LB, default = 16'b0 
       Base Address of SNC decode (PA[45:30])
     */
    UINT32 addrmask_45_40 : 6;
    /* addrmask_45_40 - Bits[21:16], RWS_LB, default = 6'b0 
       Address Hash mask bits [45:40]. 
       0 - causes corresponding address bit to be included in the hash
       1 - causes corresponding address bit to be forced to zero before entering the 
       hash function. 
       In 2LM mode of operation the setting must reflect the NM size (rounded to lower 
       power of 2) to ensure inclusive 2LM behavior. 
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_SNC_BASE_1_KTI_LLPMON_STRUCT;


/* KTILK_SNC_BASE_2_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370188)                                                  */
/*       SKX (0x40370188)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_SNC_BASE_2_KTI_LLPMON_REG 0x09004188
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Address decode for Sub NUMA Clustering (SNC)
 */
typedef union {
  struct {
    UINT32 starting_addr_1 : 16;
    /* starting_addr_1 - Bits[15:0], RWS_LB, default = 16'b0 
       Starting Address of SNC1 range (PA[45:30]). 
       SNC range 0 is considered disabled if the value is less-than-or-equal-to 
       SNC_Base. 
       If SNC range 0 is disabled it is not legal to enable ranges 1, 2, or 3.
     */
    UINT32 rsvd_16 : 4;
    /* rsvd_16 - Bits[19:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 addrmask_39_31 : 9;
    /* addrmask_39_31 - Bits[28:20], RWS_LB, default = 9'b0 
       Address Hash mask bits (39:31]. 
       0 - causes corresponding address bit to be included in the hash
       1 - causes corresponding address bit to be forced to zero before entering the 
       hash function. 
       In 2LM mode of operation the setting must reflect the NM size (rounded to lower 
       power of 2) to ensure inclusive 2LM behavior. 
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_SNC_BASE_2_KTI_LLPMON_STRUCT;


/* KTILK_SNC_BASE_3_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x4037018C)                                                  */
/*       SKX (0x4037018C)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_SNC_BASE_3_KTI_LLPMON_REG 0x0900418C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Address decode for Sub NUMA Clustering (SNC)
 */
typedef union {
  struct {
    UINT32 starting_addr_2 : 16;
    /* starting_addr_2 - Bits[15:0], RWS_LB, default = 16'b0 
       Starting Address of SNC2 range (PA[45:30])
       SNC range 1 is considered disabled if the value is less-than-or-equal-to 
       starting_addr_1. 
       If SNC range 1 is disabled it is not legal to enable range 2 or 3.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_SNC_BASE_3_KTI_LLPMON_STRUCT;


/* KTILK_SNC_BASE_4_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370190)                                                  */
/*       SKX (0x40370190)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_SNC_BASE_4_KTI_LLPMON_REG 0x09004190
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Address decode for Sub NUMA Clustering (SNC)
 */
typedef union {
  struct {
    UINT32 starting_addr_3 : 16;
    /* starting_addr_3 - Bits[15:0], RWS_LB, default = 16'b0 
       Starting Address of SNC3 range (PA[45:30]).
       SNC range 2 is considered disabled if this value is less-than-or-equal-to 
       starting_addr_2. 
       If SNC range 2 is disabled it is not legal to enable range 3.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_SNC_BASE_4_KTI_LLPMON_STRUCT;


/* KTILK_SNC_BASE_5_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370194)                                                  */
/*       SKX (0x40370194)                                                     */
/* Register default value:              0x00000000                            */
#define KTILK_SNC_BASE_5_KTI_LLPMON_REG 0x09004194
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Address decode for Sub NUMA Clustering (SNC)
 */
typedef union {
  struct {
    UINT32 starting_addr_4 : 16;
    /* starting_addr_4 - Bits[15:0], RWS_LB, default = 16'b0 
       Limit  of SNC3 range (PA[46:30]) (not inclusive of this address, aka less then 
       this value). 
       SNC range 3 is considered disabled if the value is less-than-or-equal-to 
       starting_addr_3. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTILK_SNC_BASE_5_KTI_LLPMON_STRUCT;




/* KTIPMONDBGCNTRESETVAL_N0_KTI_LLPMON_REG supported on:                      */
/*       SKX_A0 (0x40370308)                                                  */
/*       SKX (0x40370308)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONDBGCNTRESETVAL_N0_KTI_LLPMON_REG 0x09004308
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 4 Reset Value
 * Perfmon counter reset value. This is for debug only. Whenever counter 4 is 
 * reset, it will load this value instead of resetting to zero. 
 * This is locked by RSPLCK.
 */
typedef union {
  struct {
    UINT32 resetval : 32;
    /* resetval - Bits[31:0], RW_L, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       The value to reset counter 4 to (on a myeventreseten or othereventreseten 
       enabled event). 
     */
  } Bits;
  UINT32 Data;
} KTIPMONDBGCNTRESETVAL_N0_KTI_LLPMON_STRUCT;


/* KTIPMONDBGCNTRESETVAL_N1_KTI_LLPMON_REG supported on:                      */
/*       SKX_A0 (0x4037030C)                                                  */
/*       SKX (0x4037030C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONDBGCNTRESETVAL_N1_KTI_LLPMON_REG 0x0900430C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 4 Reset Value
 * Perfmon counter reset value. This is for debug only. Whenever counter 4 is 
 * reset, it will load this value instead of resetting to zero. 
 * This is locked by RSPLCK.
 */
typedef union {
  struct {
    UINT32 resetval : 16;
    /* resetval - Bits[15:0], RW_L, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       The value to reset counter 4 to (on a myeventreseten or othereventreseten 
       enabled event). 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONDBGCNTRESETVAL_N1_KTI_LLPMON_STRUCT;


/* KTIPMONDBGMBPSEL_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370310)                                                  */
/*       SKX (0x40370310)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONDBGMBPSEL_KTI_LLPMON_REG 0x09004310
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Debug mBP (micro-breakpoint) select
 * 
 */
typedef union {
  struct {
    UINT32 sel : 3;
    /* sel - Bits[2:0], RW_L, default = 3'b0 
       Selects the counter overflow that should be routed to the microbreakpoint (uBP) 
       in the miniTAP. 
       0 - Counter 0 Overflow
       1 - Counter 1 Overflow
       2 - Counter 2 Overflow
       3 - Counter 3 Overflow
       4 - Counter 4 Overflow
       5 - reserved
       6 - reserved
       7 - Any of the 5 Counter Overflow detects went high (ie. rising edge)
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONDBGMBPSEL_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_0_N0_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370318)                                                  */
/*       SKX (0x40370318)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_0_N0_KTI_LLPMON_REG 0x09004318
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 0
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_0_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_0_N1_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x4037031C)                                                  */
/*       SKX (0x4037031C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_0_N1_KTI_LLPMON_REG 0x0900431C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 0
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_0_N1_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_1_N0_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370320)                                                  */
/*       SKX (0x40370320)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_1_N0_KTI_LLPMON_REG 0x09004320
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 1
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_1_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_1_N1_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370324)                                                  */
/*       SKX (0x40370324)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_1_N1_KTI_LLPMON_REG 0x09004324
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 1
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_1_N1_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_2_N0_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370328)                                                  */
/*       SKX (0x40370328)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_2_N0_KTI_LLPMON_REG 0x09004328
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 2
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_2_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_2_N1_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x4037032C)                                                  */
/*       SKX (0x4037032C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_2_N1_KTI_LLPMON_REG 0x0900432C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 2
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_2_N1_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_3_N0_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370330)                                                  */
/*       SKX (0x40370330)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_3_N0_KTI_LLPMON_REG 0x09004330
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 3
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_3_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_3_N1_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370334)                                                  */
/*       SKX (0x40370334)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_3_N1_KTI_LLPMON_REG 0x09004334
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 3
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_3_N1_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_4_N0_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x40370338)                                                  */
/*       SKX (0x40370338)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_4_N0_KTI_LLPMON_REG 0x09004338
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 4
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_4_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTR_4_N1_KTI_LLPMON_REG supported on:                              */
/*       SKX_A0 (0x4037033C)                                                  */
/*       SKX (0x4037033C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTR_4_N1_KTI_LLPMON_REG 0x0900433C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Perfmon Counter 4
 * This register is a perfmon counter. Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTR_4_N1_KTI_LLPMON_STRUCT;


/* KTIPMONCNTRCFG_0_N0_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x40370350)                                                  */
/*       SKX (0x40370350)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_0_N0_KTI_LLPMON_REG 0x09004350
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 0
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b0 
       This field is used to decode the PerfMon event which is selected.
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b0 
       This mask selects the sub-events to be selected for creation of the event (for 
       applicable events).  See Pmon events for details. 
     */
    UINT32 rsvd_16 : 1;
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], RW_V, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows counting either 0 to 1 (if invert=0) or 1 to 0 (if invert=1) 
       transitions of a given event. For example, there is an event that counts the 
       number of cycles in L1 power state in Intel UPI. By using edge detect, one can 
       count the number of times that we entered L1 state (by detecting the rising edge 
       by setting invert=0).  
       Edge detect only works in conjunction with threshholding.  This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above).  In this case, one should set a threshold of 1 (with invert=0).  One can 
       also use Edge Detect with queue occupancy events.  For example, if one wanted to 
       count the number of times when the TXQ occupancy was greater than or equal to 5, 
       one would select the TXQ occupancy event with a threshold of 5 and set the Edge 
       Detect bit (and invert=0). 
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything.  
        If this bit is set and the Unit's configuration register has Overflow enabled, 
       then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the Perfmon Counter.  This bit must be 
       asserted in order for the Perfmon counter to begin counting the events selected 
       by the event select, unit mask, and extended unit mask.  There is one enable bit 
       per Perfmon Counter.  Note that if this bit is set to 1 but the Unit Control 
       Registers have determined that counting is disabled, then the counter will not 
       count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold greater than or 
       equal to event. When set to 1, the comparison that will be done is inverted from 
       the case where this bit is set to 0, i.e., threshold less than event. The invert 
       bit only works when Threshold != 0.  So, if one would like to invert a 
       non-occupancy event, one needs to set the threshold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b0 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle.  The result of the comparison is 
       effectively a 1 bit wide event, i.e., the counter will be incremented by 1 when 
       the comparison is true (the type of comparison depends on the setting of the 
       invert bit - see bit 23) no matter how wide the original event was.  When this 
       field is zero, threshold comparison is disabled and the event is passed without 
       modification. 
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_0_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTRCFG_0_N1_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x40370354)                                                  */
/*       SKX (0x40370354)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_0_N1_KTI_LLPMON_REG 0x09004354
/* Struct format extracted from XML file SKX_A0\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 0
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 8;
    /* unitmask_extended - Bits[7:0], RW_V, default = 8'b0 
       Extra bits for extended matching. See Pmon events
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_0_N1_KTI_LLPMON_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 0
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 32;
    /* unitmask_extended - Bits[31:0], RW_V, default = 32'b0 
       Extra bits for extended matching. See Pmon events for details.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_0_N1_KTI_LLPMON_STRUCT;



/* KTIPMONCNTRCFG_1_N0_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x40370358)                                                  */
/*       SKX (0x40370358)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_1_N0_KTI_LLPMON_REG 0x09004358
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 1
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b0 
       This field is used to decode the PerfMon event which is selected.
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b0 
       This mask selects the sub-events to be selected for creation of the event (for 
       applicable events).  See Pmon events for details. 
     */
    UINT32 rsvd_16 : 1;
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows counting either 0 to 1 (if invert=0) or 1 to 0 (if invert=1) 
       transitions of a given event. For example, there is an event that counts the 
       number of cycles in L1 power state in Intel UPI. By using edge detect, one can 
       count the number of times that we entered L1 state (by detecting the rising edge 
       by setting invert=0).  
       Edge detect only works in conjunction with threshholding.  This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above).  In this case, one should set a threshold of 1 (with invert=0).  One can 
       also use Edge Detect with queue occupancy events.  For example, if one wanted to 
       count the number of times when the TXQ occupancy was greater than or equal to 5, 
       one would select the TXQ occupancy event with a threshold of 5 and set the Edge 
       Detect bit (and invert=0). 
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything.  
        If this bit is set and the Unit's configuration register has Overflow enabled, 
       then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the Perfmon Counter.  This bit must be 
       asserted in order for the Perfmon counter to begin counting the events selected 
       by the event select, unit mask, and extended unit mask.  There is one enable bit 
       per Perfmon Counter.  Note that if this bit is set to 1 but the Unit Control 
       Registers have determined that counting is disabled, then the counter will not 
       count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold greater than or 
       equal to event. When set to 1, the comparison that will be done is inverted from 
       the case where this bit is set to 0, i.e., threshold less than event. The invert 
       bit only works when Threshold != 0.  So, if one would like to invert a 
       non-occupancy event, one needs to set the threshold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b0 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle.  The result of the comparison is 
       effectively a 1 bit wide event, i.e., the counter will be incremented by 1 when 
       the comparison is true (the type of comparison depends on the setting of the 
       invert bit - see bit 23) no matter how wide the original event was.  When this 
       field is zero, threshold comparison is disabled and the event is passed without 
       modification. 
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_1_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTRCFG_1_N1_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x4037035C)                                                  */
/*       SKX (0x4037035C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_1_N1_KTI_LLPMON_REG 0x0900435C
/* Struct format extracted from XML file SKX_A0\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 1
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 8;
    /* unitmask_extended - Bits[7:0], RW_V, default = 8'b0 
       Extra bits for extended matching. See Pmon events.
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_1_N1_KTI_LLPMON_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 1
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 32;
    /* unitmask_extended - Bits[31:0], RW_V, default = 32'b0 
       Extra bits for extended matching. See Pmon events for details.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_1_N1_KTI_LLPMON_STRUCT;



/* KTIPMONCNTRCFG_2_N0_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x40370360)                                                  */
/*       SKX (0x40370360)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_2_N0_KTI_LLPMON_REG 0x09004360
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 2
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b0 
       This field is used to decode the PerfMon event which is selected.
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b0 
       This mask selects the sub-events to be selected for creation of the event (for 
       applicable events).  See Pmon events for details. 
     */
    UINT32 rsvd_16 : 1;
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows counting either 0 to 1 (if invert=0) or 1 to 0 (if invert=1) 
       transitions of a given event. For example, there is an event that counts the 
       number of cycles in L1 power state in Intel UPI. By using edge detect, one can 
       count the number of times that we entered L1 state (by detecting the rising edge 
       by setting invert=0).  
       Edge detect only works in conjunction with threshholding.  This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above).  In this case, one should set a threshold of 1 (with invert=0).  One can 
       also use Edge Detect with queue occupancy events.  For example, if one wanted to 
       count the number of times when the TXQ occupancy was greater than or equal to 5, 
       one would select the TXQ occupancy event with a threshold of 5 and set the Edge 
       Detect bit (and invert=0). 
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything.  
        If this bit is set and the Unit's configuration register has Overflow enabled, 
       then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the Perfmon Counter.  This bit must be 
       asserted in order for the Perfmon counter to begin counting the events selected 
       by the event select, unit mask, and extended unit mask.  There is one enable bit 
       per Perfmon Counter.  Note that if this bit is set to 1 but the Unit Control 
       Registers have determined that counting is disabled, then the counter will not 
       count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold greater than or 
       equal to event. When set to 1, the comparison that will be done is inverted from 
       the case where this bit is set to 0, i.e., threshold less than event. The invert 
       bit only works when Threshold != 0.  So, if one would like to invert a 
       non-occupancy event, one needs to set the threshold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b0 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle.  The result of the comparison is 
       effectively a 1 bit wide event, i.e., the counter will be incremented by 1 when 
       the comparison is true (the type of comparison depends on the setting of the 
       invert bit - see bit 23) no matter how wide the original event was.  When this 
       field is zero, threshold comparison is disabled and the event is passed without 
       modification. 
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_2_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTRCFG_2_N1_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x40370364)                                                  */
/*       SKX (0x40370364)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_2_N1_KTI_LLPMON_REG 0x09004364
/* Struct format extracted from XML file SKX_A0\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 2
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 8;
    /* unitmask_extended - Bits[7:0], RW_V, default = 8'b0 
       Extra bits for extended matching. See Pmon events
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_2_N1_KTI_LLPMON_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 2
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 32;
    /* unitmask_extended - Bits[31:0], RW_V, default = 32'b0 
       Extra bits for extended matching. See Pmon events for details.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_2_N1_KTI_LLPMON_STRUCT;



/* KTIPMONCNTRCFG_3_N0_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x40370368)                                                  */
/*       SKX (0x40370368)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_3_N0_KTI_LLPMON_REG 0x09004368
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 3
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b0 
       This field is used to decode the PerfMon event which is selected.
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b0 
       This mask selects the sub-events to be selected for creation of the event (for 
       applicable events).  See Pmon events for details. 
     */
    UINT32 rsvd_16 : 1;
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], RW_V, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows counting either 0 to 1 (if invert=0) or 1 to 0 (if invert=1) 
       transitions of a given event. For example, there is an event that counts the 
       number of cycles in L1 power state in Intel UPI. By using edge detect, one can 
       count the number of times that we entered L1 state (by detecting the rising edge 
       by setting invert=0).  
       Edge detect only works in conjunction with threshholding.  This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above).  In this case, one should set a threshold of 1 (with invert=0).  One can 
       also use Edge Detect with queue occupancy events.  For example, if one wanted to 
       count the number of times when the TXQ occupancy was greater than or equal to 5, 
       one would select the TXQ occupancy event with a threshold of 5 and set the Edge 
       Detect bit (and invert=0). 
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything.  
        If this bit is set and the Unit's configuration register has Overflow enabled, 
       then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the Perfmon Counter.  This bit must be 
       asserted in order for the Perfmon counter to begin counting the events selected 
       by the event select, unit mask, and extended unit mask.  There is one enable bit 
       per Perfmon Counter.  Note that if this bit is set to 1 but the Unit Control 
       Registers have determined that counting is disabled, then the counter will not 
       count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold greater than or 
       equal to event. When set to 1, the comparison that will be done is inverted from 
       the case where this bit is set to 0, i.e., threshold less than event. The invert 
       bit only works when Threshold != 0.  So, if one would like to invert a 
       non-occupancy event, one needs to set the threshold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b0 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle.  The result of the comparison is 
       effectively a 1 bit wide event, i.e., the counter will be incremented by 1 when 
       the comparison is true (the type of comparison depends on the setting of the 
       invert bit - see bit 23) no matter how wide the original event was.  When this 
       field is zero, threshold comparison is disabled and the event is passed without 
       modification. 
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_3_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTRCFG_3_N1_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x4037036C)                                                  */
/*       SKX (0x4037036C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_3_N1_KTI_LLPMON_REG 0x0900436C
/* Struct format extracted from XML file SKX_A0\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 3
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 8;
    /* unitmask_extended - Bits[7:0], RW_V, default = 8'b0 
       Extra bits for extended matching. See Pmon events
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_3_N1_KTI_LLPMON_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 3
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 32;
    /* unitmask_extended - Bits[31:0], RW_V, default = 32'b0 
       Extra bits for extended matching. See Pmon events for details.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_3_N1_KTI_LLPMON_STRUCT;



/* KTIPMONCNTRCFG_4_N0_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x40370370)                                                  */
/*       SKX (0x40370370)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_4_N0_KTI_LLPMON_REG 0x09004370
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 4
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b0 
       This field is used to decode the PerfMon event which is selected.
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b0 
       This mask selects the sub-events to be selected for creation of the event (for 
       applicable events).  See Pmon events for details. 
     */
    UINT32 rsvd_16 : 1;
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], RW_V, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows counting either 0 to 1 (if invert=0) or 1 to 0 (if invert=1) 
       transitions of a given event. For example, there is an event that counts the 
       number of cycles in L1 power state in Intel UPI. By using edge detect, one can 
       count the number of times that we entered L1 state (by detecting the rising edge 
       by setting invert=0).  
       Edge detect only works in conjunction with threshholding.  This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above).  In this case, one should set a threshold of 1 (with invert=0).  One can 
       also use Edge Detect with queue occupancy events.  For example, if one wanted to 
       count the number of times when the TXQ occupancy was greater than or equal to 5, 
       one would select the TXQ occupancy event with a threshold of 5 and set the Edge 
       Detect bit (and invert=0). 
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything.  
        If this bit is set and the Unit's configuration register has Overflow enabled, 
       then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the Perfmon Counter.  This bit must be 
       asserted in order for the Perfmon counter to begin counting the events selected 
       by the event select, unit mask, and extended unit mask.  There is one enable bit 
       per Perfmon Counter.  Note that if this bit is set to 1 but the Unit Control 
       Registers have determined that counting is disabled, then the counter will not 
       count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold greater than or 
       equal to event. When set to 1, the comparison that will be done is inverted from 
       the case where this bit is set to 0, i.e., threshold less than event. The invert 
       bit only works when Threshold != 0.  So, if one would like to invert a 
       non-occupancy event, one needs to set the threshold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b0 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle.  The result of the comparison is 
       effectively a 1 bit wide event, i.e., the counter will be incremented by 1 when 
       the comparison is true (the type of comparison depends on the setting of the 
       invert bit - see bit 23) no matter how wide the original event was.  When this 
       field is zero, threshold comparison is disabled and the event is passed without 
       modification. 
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_4_N0_KTI_LLPMON_STRUCT;


/* KTIPMONCNTRCFG_4_N1_KTI_LLPMON_REG supported on:                           */
/*       SKX_A0 (0x40370374)                                                  */
/*       SKX (0x40370374)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONCNTRCFG_4_N1_KTI_LLPMON_REG 0x09004374
/* Struct format extracted from XML file SKX_A0\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 4
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 8;
    /* unitmask_extended - Bits[7:0], RW_V, default = 8'b0 
       Extra bits for extended matching. See Pmon events
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_4_N1_KTI_LLPMON_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Counter Config 4
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 unitmask_extended : 32;
    /* unitmask_extended - Bits[31:0], RW_V, default = 32'b0 
       Extra bits for extended matching. See Pmon events for details.
     */
  } Bits;
  UINT32 Data;
} KTIPMONCNTRCFG_4_N1_KTI_LLPMON_STRUCT;



/* KTIPMONUNITCTRL_KTI_LLPMON_REG supported on:                               */
/*       SKX_A0 (0x40370378)                                                  */
/*       SKX (0x40370378)                                                     */
/* Register default value:              0x00030000                            */
#define KTIPMONUNITCTRL_KTI_LLPMON_REG 0x09004378
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Unit Control
 */
typedef union {
  struct {
    UINT32 resetcounterconfigs : 1;
    /* resetcounterconfigs - Bits[0:0], WO, default = 1'b0 
       When this bit is written to, the counter configuration registers will be reset. 
       This does not effect the values in the counters. 
     */
    UINT32 resetcounters : 1;
    /* resetcounters - Bits[1:1], WO, default = 1'b0 
       When this bit is written to, the counters data fields will be reset. The 
       configuration values will not be reset. 
       Note: The counters should be frozen (KTIPMONUNITCTRL.freezecounters=1) before 
       writing resetcounters=1. 
     */
    UINT32 rsvd_2 : 6;
    /* rsvd_2 - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 freezecounters : 1;
    /* freezecounters - Bits[8:8], RW_V, default = 1'b0 
       This bit is written to when the counters should be frozen. If this bit is 
       written to and freeze is enabled, the counters in the unit will stop counting. 
     */
    UINT32 rsvd_9 : 7;
    /* rsvd_9 - Bits[15:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} KTIPMONUNITCTRL_KTI_LLPMON_STRUCT;


/* KTIPMONUNITSTATUS_KTI_LLPMON_REG supported on:                             */
/*       SKX_A0 (0x4037037C)                                                  */
/*       SKX (0x4037037C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPMONUNITSTATUS_KTI_LLPMON_REG 0x0900437C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Intel UPI Pmon Unit Status
 */
typedef union {
  struct {
    UINT32 counteroverflowbitmask : 5;
    /* counteroverflowbitmask - Bits[4:0], RW1C, default = 5'b0 
       This field shows which PMON counters have overflowed in the unit.
       Whenever a counter overflows, it will set the relevant bit to 1. An overflow 
       will not effect the other status bits. This status should only be cleared by 
       software.  
       [0] - KTIPMONCNTR_0 overflowed
       [1] - KTIPMONCNTR_1 overflowed
       [2] - KTIPMONCNTR_2 overflowed
       [3] - KTIPMONCNTR_3 overflowed
       [4] - KTIPMONCNTR_4 overflowed
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPMONUNITSTATUS_KTI_LLPMON_STRUCT;


/* SMI3TNLCTRL_KTI_LLPMON_REG supported on:                                   */
/*       SKX_A0 (0x40370400)                                                  */
/*       SKX (0x40370400)                                                     */
/* Register default value:              0x00000000                            */
#define SMI3TNLCTRL_KTI_LLPMON_REG 0x09004400
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * SMI3 Tunneling Control Register
 */
typedef union {
  struct {
    UINT32 quiesce_enable : 1;
    /* quiesce_enable - Bits[0:0], RW, default = 1'b0  */
    UINT32 ignore_agent_credit : 1;
    /* ignore_agent_credit - Bits[1:1], RW, default = 1'b0  */
    UINT32 ignore_link_credit : 1;
    /* ignore_link_credit - Bits[2:2], RW, default = 1'b0  */
    UINT32 inject_link_credit_return : 1;
    /* inject_link_credit_return - Bits[3:3], RW_V, default = 1'b0  */
    UINT32 duplicate_msg_to_tunnel_dbgmode : 1;
    /* duplicate_msg_to_tunnel_dbgmode - Bits[4:4], RW, default = 1'b0  */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rx_pma_status : 2;
    /* rx_pma_status - Bits[17:16], RO_V, default = 2'h0  */
    UINT32 rx_msg_status : 2;
    /* rx_msg_status - Bits[19:18], RO_V, default = 2'h0  */
    UINT32 tx_pma_status : 2;
    /* tx_pma_status - Bits[21:20], RO_V, default = 2'h0  */
    UINT32 tx_msg_status : 2;
    /* tx_msg_status - Bits[23:22], RO_V, default = 2'h0  */
    UINT32 rsvd_24 : 7;
    /* rsvd_24 - Bits[30:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 malformed_pkt : 1;
    /* malformed_pkt - Bits[31:31], RW1C, default = 1'b0  */
  } Bits;
  UINT32 Data;
} SMI3TNLCTRL_KTI_LLPMON_STRUCT;


/* SMI3TNLCTRL2_KTI_LLPMON_REG supported on:                                  */
/*       SKX_A0 (0x40370404)                                                  */
/*       SKX (0x40370404)                                                     */
/* Register default value:              0x00000000                            */
#define SMI3TNLCTRL2_KTI_LLPMON_REG 0x09004404
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * SMI3 Tunneling Control Register
 */
typedef union {
  struct {
    UINT32 rx_inject_pma_wake : 1;
    /* rx_inject_pma_wake - Bits[0:0], RW_V, default = 1'b0  */
    UINT32 rx_inject_pma_quiesce : 1;
    /* rx_inject_pma_quiesce - Bits[1:1], RW_V, default = 1'b0  */
    UINT32 rx_inject_pma_rst_agent : 1;
    /* rx_inject_pma_rst_agent - Bits[2:2], RW_V, default = 1'b0  */
    UINT32 rx_inject_pma_agtcdtret : 1;
    /* rx_inject_pma_agtcdtret - Bits[3:3], RW_V, default = 1'b0  */
    UINT32 rx_inject_pma_agent_status : 1;
    /* rx_inject_pma_agent_status - Bits[4:4], RW_V, default = 1'b0  */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rx_inject_msg_wake : 1;
    /* rx_inject_msg_wake - Bits[8:8], RW_V, default = 1'b0  */
    UINT32 rx_inject_msg_quiesce : 1;
    /* rx_inject_msg_quiesce - Bits[9:9], RW_V, default = 1'b0  */
    UINT32 rx_inject_msg_rst_agent : 1;
    /* rx_inject_msg_rst_agent - Bits[10:10], RW_V, default = 1'b0  */
    UINT32 rx_inject_msg_agtcdtret : 1;
    /* rx_inject_msg_agtcdtret - Bits[11:11], RW_V, default = 1'b0  */
    UINT32 rx_inject_msg_agent_status : 1;
    /* rx_inject_msg_agent_status - Bits[12:12], RW_V, default = 1'b0  */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tx_inject_pma_wake : 1;
    /* tx_inject_pma_wake - Bits[16:16], RW_V, default = 1'b0  */
    UINT32 tx_inject_pma_quiesce : 1;
    /* tx_inject_pma_quiesce - Bits[17:17], RW_V, default = 1'b0  */
    UINT32 tx_inject_pma_rst_agent : 1;
    /* tx_inject_pma_rst_agent - Bits[18:18], RW_V, default = 1'b0  */
    UINT32 tx_inject_pma_agtcdtret : 1;
    /* tx_inject_pma_agtcdtret - Bits[19:19], RW_V, default = 1'b0  */
    UINT32 tx_inject_pma_agent_status : 1;
    /* tx_inject_pma_agent_status - Bits[20:20], RW_V, default = 1'b0  */
    UINT32 rsvd_21 : 3;
    /* rsvd_21 - Bits[23:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tx_inject_msg_wake : 1;
    /* tx_inject_msg_wake - Bits[24:24], RW_V, default = 1'b0  */
    UINT32 tx_inject_msg_quiesce : 1;
    /* tx_inject_msg_quiesce - Bits[25:25], RW_V, default = 1'b0  */
    UINT32 tx_inject_msg_rst_agent : 1;
    /* tx_inject_msg_rst_agent - Bits[26:26], RW_V, default = 1'b0  */
    UINT32 tx_inject_msg_agtcdtret : 1;
    /* tx_inject_msg_agtcdtret - Bits[27:27], RW_V, default = 1'b0  */
    UINT32 tx_inject_msg_agent_status : 1;
    /* tx_inject_msg_agent_status - Bits[28:28], RW_V, default = 1'b0  */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMI3TNLCTRL2_KTI_LLPMON_STRUCT;


/* PXPCAP_F0_KTI_LLPMON_REG supported on:                                     */
/*       SKX (0x40370040)                                                     */
/* Register default value:              0x00920010                            */
#define PXPCAP_F0_KTI_LLPMON_REG 0x09004040
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * PCIE Capability
 */
typedef union {
  struct {
    UINT32 capability_id : 8;
    /* capability_id - Bits[7:0], RO, default = 8'h10 
       Provides the PCI Express capability ID assigned by PCI-SIG.
     */
    UINT32 next_ptr : 8;
    /* next_ptr - Bits[15:8], RO, default = 8'b0 
       Pointer to the next capability. Set to 0 to indicate there are no more 
       capability structures. 
     */
    UINT32 capability_version : 4;
    /* capability_version - Bits[19:16], RO, default = 4'h2 
       PCI Express Capability is Compliant with Version 1.0 of the PCI Express Spec.
        Note: This capability structure is not compliant with Versions beyond 1.0, 
       since they require additional capability registers to be reserved. The only 
       purpose for this capability structure is to make enhanced configuration space 
       available. Minimizing the size of this structure is accomplished by reporting 
       version 1.0 compliancy and reporting that this is an integrated root port 
       device. As such, only three Dwords of configuration space are required for this 
       structure. 
     */
    UINT32 device_port_type : 4;
    /* device_port_type - Bits[23:20], RO, default = 4'h9 
       Device type is Root Complex Integrated Endpoint
     */
    UINT32 slot_implemented : 1;
    /* slot_implemented - Bits[24:24], RO, default = 1'b0 
       N/A for integrated endpoints.
     */
    UINT32 interrupt_message_number : 5;
    /* interrupt_message_number - Bits[29:25], RO, default = 5'b0 
       N/A for this device.
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PXPCAP_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD0_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370044)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD0_F0_KTI_LLPMON_REG 0x09004044
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD0_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD1_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370048)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD1_F0_KTI_LLPMON_REG 0x09004048
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD1_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD2_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x4037004C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD2_F0_KTI_LLPMON_REG 0x0900404C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD2_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD3_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370050)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD3_F0_KTI_LLPMON_REG 0x09004050
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD3_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD4_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370054)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD4_F0_KTI_LLPMON_REG 0x09004054
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD4_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD5_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370058)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD5_F0_KTI_LLPMON_REG 0x09004058
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD5_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD6_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x4037005C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD6_F0_KTI_LLPMON_REG 0x0900405C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD6_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD7_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370060)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD7_F0_KTI_LLPMON_REG 0x09004060
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD7_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD8_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370064)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD8_F0_KTI_LLPMON_REG 0x09004064
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD8_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD9_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370068)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD9_F0_KTI_LLPMON_REG 0x09004068
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD9_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD10_F0_KTI_LLPMON_REG supported on:                                 */
/*       SKX (0x4037006C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD10_F0_KTI_LLPMON_REG 0x0900406C
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD10_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD11_F0_KTI_LLPMON_REG supported on:                                 */
/*       SKX (0x40370070)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD11_F0_KTI_LLPMON_REG 0x09004070
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD11_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD12_F0_KTI_LLPMON_REG supported on:                                 */
/*       SKX (0x40370074)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD12_F0_KTI_LLPMON_REG 0x09004074
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD12_F0_KTI_LLPMON_STRUCT;


/* PXP_RSVD13_F0_KTI_LLPMON_REG supported on:                                 */
/*       SKX (0x40370078)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD13_F0_KTI_LLPMON_REG 0x09004078
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD13_F0_KTI_LLPMON_STRUCT;


/* PXPENHCAP_F0_KTI_LLPMON_REG supported on:                                  */
/*       SKX (0x40370100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_F0_KTI_LLPMON_REG 0x09004100
/* Struct format extracted from XML file SKX\3.14.0.CFG.xml.
 * PCI Express Enhanced Capability Register
 */
typedef union {
  struct {
    UINT32 capability_id : 16;
    /* capability_id - Bits[15:0], RO, default = 16'b0 
       There is no capability at this location.
     */
    UINT32 capability_version : 4;
    /* capability_version - Bits[19:16], RO, default = 4'b0 
       There is no capability at this location.
     */
    UINT32 next_capability_offset : 12;
    /* next_capability_offset - Bits[31:20], RO, default = 12'b0 
       Pointer to the next capability in the enhanced configuraton space.  Set to 0 to 
       indicate there are no more capability structures. 
     */
  } Bits;
  UINT32 Data;
} PXPENHCAP_F0_KTI_LLPMON_STRUCT;




#endif /* KTI_LLPMON_h */
