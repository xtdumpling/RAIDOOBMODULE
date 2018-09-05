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

#ifndef VCU_FUN2_h
#define VCU_FUN2_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* VCU_FUN2_DEV 31                                                            */
/* VCU_FUN2_FUN 2                                                             */

/* VID_VCU_FUN2_REG supported on:                                             */
/*       SKX_A0 (0x201FA000)                                                  */
/*       SKX (0x201FA000)                                                     */
/* Register default value:              0x8086                                */
#define VID_VCU_FUN2_REG 0x05022000
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} VID_VCU_FUN2_STRUCT;


/* DID_VCU_FUN2_REG supported on:                                             */
/*       SKX_A0 (0x201FA002)                                                  */
/*       SKX (0x201FA002)                                                     */
/* Register default value:              0x207A                                */
#define DID_VCU_FUN2_REG 0x05022002
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h207A 
        
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
} DID_VCU_FUN2_STRUCT;


/* PCICMD_VCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x201FA004)                                                  */
/*       SKX (0x201FA004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_VCU_FUN2_REG 0x05022004
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} PCICMD_VCU_FUN2_STRUCT;


/* PCISTS_VCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x201FA006)                                                  */
/*       SKX (0x201FA006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_VCU_FUN2_REG 0x05022006
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} PCISTS_VCU_FUN2_STRUCT;


/* RID_VCU_FUN2_REG supported on:                                             */
/*       SKX_A0 (0x101FA008)                                                  */
/*       SKX (0x101FA008)                                                     */
/* Register default value:              0x00                                  */
#define RID_VCU_FUN2_REG 0x05021008
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} RID_VCU_FUN2_STRUCT;


/* CCR_N0_VCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101FA009)                                                  */
/*       SKX (0x101FA009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_VCU_FUN2_REG 0x05021009
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_VCU_FUN2_STRUCT;


/* CCR_N1_VCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x201FA00A)                                                  */
/*       SKX (0x201FA00A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_VCU_FUN2_REG 0x0502200A
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} CCR_N1_VCU_FUN2_STRUCT;


/* CLSR_VCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x101FA00C)                                                  */
/*       SKX (0x101FA00C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_VCU_FUN2_REG 0x0502100C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} CLSR_VCU_FUN2_STRUCT;


/* PLAT_VCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x101FA00D)                                                  */
/*       SKX (0x101FA00D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_VCU_FUN2_REG 0x0502100D
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} PLAT_VCU_FUN2_STRUCT;


/* HDR_VCU_FUN2_REG supported on:                                             */
/*       SKX_A0 (0x101FA00E)                                                  */
/*       SKX (0x101FA00E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_VCU_FUN2_REG 0x0502100E
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} HDR_VCU_FUN2_STRUCT;


/* BIST_VCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x101FA00F)                                                  */
/*       SKX (0x101FA00F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_VCU_FUN2_REG 0x0502100F
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} BIST_VCU_FUN2_STRUCT;


/* CAPPTR_VCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101FA034)                                                  */
/*       SKX (0x101FA034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_VCU_FUN2_REG 0x05021034
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} CAPPTR_VCU_FUN2_STRUCT;


/* INTL_VCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x101FA03C)                                                  */
/*       SKX (0x101FA03C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_VCU_FUN2_REG 0x0502103C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} INTL_VCU_FUN2_STRUCT;


/* INTPIN_VCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101FA03D)                                                  */
/*       SKX (0x101FA03D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_VCU_FUN2_REG 0x0502103D
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} INTPIN_VCU_FUN2_STRUCT;


/* MINGNT_VCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101FA03E)                                                  */
/*       SKX (0x101FA03E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_VCU_FUN2_REG 0x0502103E
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} MINGNT_VCU_FUN2_STRUCT;


/* MAXLAT_VCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101FA03F)                                                  */
/*       SKX (0x101FA03F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_VCU_FUN2_REG 0x0502103F
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} MAXLAT_VCU_FUN2_STRUCT;


/* GLOBAL_NID_SOCKET_0_TO_3_MAP_VCU_FUN2_REG supported on:                    */
/*       SKX_A0 (0x401FA0D0)                                                  */
/*       SKX (0x401FA0D0)                                                     */
/* Register default value:              0x00000000                            */
#define GLOBAL_NID_SOCKET_0_TO_3_MAP_VCU_FUN2_REG 0x050240D0
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
 * This register is in the VCU CR space.  It contains NID information for sockets 
 * 0-3 in the platform.  Expectation is that BIOS will write this register during 
 * the Reset/Init flow. 
 */
typedef union {
  struct {
    UINT32 skt0_nid : 3;
    /* skt0_nid - Bits[2:0], RW, default = 3'b000 
       Socket0 NID
     */
    UINT32 rsvd_3 : 1;
    UINT32 skt1_nid : 3;
    /* skt1_nid - Bits[6:4], RW, default = 3'b000 
       Socket1 NID
     */
    UINT32 rsvd_7 : 1;
    UINT32 skt2_nid : 3;
    /* skt2_nid - Bits[10:8], RW, default = 3'b000 
       Socket2 NID
     */
    UINT32 rsvd_11 : 1;
    UINT32 skt3_nid : 3;
    /* skt3_nid - Bits[14:12], RW, default = 3'b000 
       Socket3 NID
     */
    UINT32 rsvd_15 : 13;
    UINT32 skt_valid : 4;
    /* skt_valid - Bits[31:28], RW, default = 4'b0000 
       Valid bits indicating whether NID has been programmed by BIOS.  
     */
  } Bits;
  UINT32 Data;
} GLOBAL_NID_SOCKET_0_TO_3_MAP_VCU_FUN2_STRUCT;


/* GLOBAL_NID_SOCKET_4_TO_7_MAP_VCU_FUN2_REG supported on:                    */
/*       SKX_A0 (0x401FA0D4)                                                  */
/*       SKX (0x401FA0D4)                                                     */
/* Register default value:              0x00000000                            */
#define GLOBAL_NID_SOCKET_4_TO_7_MAP_VCU_FUN2_REG 0x050240D4
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
 * This reister is in the VCU CR space.  It contains NID information for sockets 
 * 4-7 in the platform.  Expectation is that BIOS will write this register during 
 * the Reset/Init flow. 
 */
typedef union {
  struct {
    UINT32 skt4_nid : 3;
    /* skt4_nid - Bits[2:0], RW, default = 3'b000 
       Socket4 NID
     */
    UINT32 rsvd_3 : 1;
    UINT32 skt5_nid : 3;
    /* skt5_nid - Bits[6:4], RW, default = 3'b000 
       Socket5 NID
     */
    UINT32 rsvd_7 : 1;
    UINT32 skt6_nid : 3;
    /* skt6_nid - Bits[10:8], RW, default = 3'b000 
       Socket6 NID
     */
    UINT32 rsvd_11 : 1;
    UINT32 skt7_nid : 3;
    /* skt7_nid - Bits[14:12], RW, default = 3'b000 
       Socket7 NID
     */
    UINT32 rsvd_15 : 13;
    UINT32 skt_valid : 4;
    /* skt_valid - Bits[31:28], RW, default = 4'b0000 
       Valid bits indicating whether NID has been programmed by BIOS. 
     */
  } Bits;
  UINT32 Data;
} GLOBAL_NID_SOCKET_4_TO_7_MAP_VCU_FUN2_STRUCT;


/* DEVHIDE_B0F0_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA110)                                                  */
/*       SKX (0x401FA110)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F0_VCU_FUN2_REG 0x05024110
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B0F0_VCU_FUN2_STRUCT;


/* DEVHIDE_B0F1_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA114)                                                  */
/*       SKX (0x401FA114)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F1_VCU_FUN2_REG 0x05024114
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B0F1_VCU_FUN2_STRUCT;


/* DEVHIDE_B0F2_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA118)                                                  */
/*       SKX (0x401FA118)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F2_VCU_FUN2_REG 0x05024118
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B0F2_VCU_FUN2_STRUCT;


/* DEVHIDE_B0F3_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA11C)                                                  */
/*       SKX (0x401FA11C)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F3_VCU_FUN2_REG 0x0502411C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B0F3_VCU_FUN2_STRUCT;


/* DEVHIDE_B0F4_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA120)                                                  */
/*       SKX (0x401FA120)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F4_VCU_FUN2_REG 0x05024120
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B0F4_VCU_FUN2_STRUCT;


/* DEVHIDE_B0F5_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA124)                                                  */
/*       SKX (0x401FA124)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F5_VCU_FUN2_REG 0x05024124
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B0F5_VCU_FUN2_STRUCT;


/* DEVHIDE_B0F6_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA128)                                                  */
/*       SKX (0x401FA128)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F6_VCU_FUN2_REG 0x05024128
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B0F6_VCU_FUN2_STRUCT;


/* DEVHIDE_B0F7_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA12C)                                                  */
/*       SKX (0x401FA12C)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE_B0F7_VCU_FUN2_REG 0x0502412C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B0F7_VCU_FUN2_STRUCT;


/* DEVHIDE_B1F0_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA130)                                                  */
/*       SKX (0x401FA130)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F0_VCU_FUN2_REG 0x05024130
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B1F0_VCU_FUN2_STRUCT;


/* DEVHIDE_B1F1_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA134)                                                  */
/*       SKX (0x401FA134)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F1_VCU_FUN2_REG 0x05024134
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B1F1_VCU_FUN2_STRUCT;


/* DEVHIDE_B1F2_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA138)                                                  */
/*       SKX (0x401FA138)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F2_VCU_FUN2_REG 0x05024138
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B1F2_VCU_FUN2_STRUCT;


/* DEVHIDE_B1F3_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA13C)                                                  */
/*       SKX (0x401FA13C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F3_VCU_FUN2_REG 0x0502413C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B1F3_VCU_FUN2_STRUCT;


/* DEVHIDE_B1F4_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA140)                                                  */
/*       SKX (0x401FA140)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F4_VCU_FUN2_REG 0x05024140
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B1F4_VCU_FUN2_STRUCT;


/* DEVHIDE_B1F5_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA144)                                                  */
/*       SKX (0x401FA144)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F5_VCU_FUN2_REG 0x05024144
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B1F5_VCU_FUN2_STRUCT;


/* DEVHIDE_B1F6_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA148)                                                  */
/*       SKX (0x401FA148)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F6_VCU_FUN2_REG 0x05024148
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B1F6_VCU_FUN2_STRUCT;


/* DEVHIDE_B1F7_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA14C)                                                  */
/*       SKX (0x401FA14C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE_B1F7_VCU_FUN2_REG 0x0502414C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B1F7_VCU_FUN2_STRUCT;


/* DEVHIDE_B2F0_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA150)                                                  */
/*       SKX (0x401FA150)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F0_VCU_FUN2_REG 0x05024150
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B2F0_VCU_FUN2_STRUCT;


/* DEVHIDE_B2F1_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA154)                                                  */
/*       SKX (0x401FA154)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F1_VCU_FUN2_REG 0x05024154
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B2F1_VCU_FUN2_STRUCT;


/* DEVHIDE_B2F2_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA158)                                                  */
/*       SKX (0x401FA158)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F2_VCU_FUN2_REG 0x05024158
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B2F2_VCU_FUN2_STRUCT;


/* DEVHIDE_B2F3_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA15C)                                                  */
/*       SKX (0x401FA15C)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F3_VCU_FUN2_REG 0x0502415C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B2F3_VCU_FUN2_STRUCT;


/* DEVHIDE_B2F4_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA160)                                                  */
/*       SKX (0x401FA160)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F4_VCU_FUN2_REG 0x05024160
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B2F4_VCU_FUN2_STRUCT;


/* DEVHIDE_B2F5_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA164)                                                  */
/*       SKX (0x401FA164)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F5_VCU_FUN2_REG 0x05024164
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B2F5_VCU_FUN2_STRUCT;


/* DEVHIDE_B2F6_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA168)                                                  */
/*       SKX (0x401FA168)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F6_VCU_FUN2_REG 0x05024168
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B2F6_VCU_FUN2_STRUCT;


/* DEVHIDE_B2F7_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA16C)                                                  */
/*       SKX (0x401FA16C)                                                     */
/* Register default value:              0xFFFFC000                            */
#define DEVHIDE_B2F7_VCU_FUN2_REG 0x0502416C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B2F7_VCU_FUN2_STRUCT;


/* DEVHIDE_B3F0_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA170)                                                  */
/*       SKX (0x401FA170)                                                     */
/* Register default value:              0xFF183F00                            */
#define DEVHIDE_B3F0_VCU_FUN2_REG 0x05024170
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B3F0_VCU_FUN2_STRUCT;


/* DEVHIDE_B3F1_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA174)                                                  */
/*       SKX (0x401FA174)                                                     */
/* Register default value:              0xFF183F00                            */
#define DEVHIDE_B3F1_VCU_FUN2_REG 0x05024174
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B3F1_VCU_FUN2_STRUCT;


/* DEVHIDE_B3F2_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA178)                                                  */
/*       SKX (0x401FA178)                                                     */
/* Register default value:              0xFF183F00                            */
#define DEVHIDE_B3F2_VCU_FUN2_REG 0x05024178
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B3F2_VCU_FUN2_STRUCT;


/* DEVHIDE_B3F3_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA17C)                                                  */
/*       SKX (0x401FA17C)                                                     */
/* Register default value:              0xFF183F00                            */
#define DEVHIDE_B3F3_VCU_FUN2_REG 0x0502417C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B3F3_VCU_FUN2_STRUCT;


/* DEVHIDE_B3F4_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA180)                                                  */
/*       SKX (0x401FA180)                                                     */
/* Register default value:              0xFF1BFF00                            */
#define DEVHIDE_B3F4_VCU_FUN2_REG 0x05024180
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B3F4_VCU_FUN2_STRUCT;


/* DEVHIDE_B3F5_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA184)                                                  */
/*       SKX (0x401FA184)                                                     */
/* Register default value:              0xFF1BFF00                            */
#define DEVHIDE_B3F5_VCU_FUN2_REG 0x05024184
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B3F5_VCU_FUN2_STRUCT;


/* DEVHIDE_B3F6_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA188)                                                  */
/*       SKX (0x401FA188)                                                     */
/* Register default value:              0xFF1BFF00                            */
#define DEVHIDE_B3F6_VCU_FUN2_REG 0x05024188
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B3F6_VCU_FUN2_STRUCT;


/* DEVHIDE_B3F7_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA18C)                                                  */
/*       SKX (0x401FA18C)                                                     */
/* Register default value:              0xFF1BFF00                            */
#define DEVHIDE_B3F7_VCU_FUN2_REG 0x0502418C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B3F7_VCU_FUN2_STRUCT;


/* DEVHIDE_B4F0_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA190)                                                  */
/*       SKX (0x401FA190)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F0_VCU_FUN2_REG 0x05024190
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B4F0_VCU_FUN2_STRUCT;


/* DEVHIDE_B4F1_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA194)                                                  */
/*       SKX (0x401FA194)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F1_VCU_FUN2_REG 0x05024194
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B4F1_VCU_FUN2_STRUCT;


/* DEVHIDE_B4F2_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA198)                                                  */
/*       SKX (0x401FA198)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F2_VCU_FUN2_REG 0x05024198
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B4F2_VCU_FUN2_STRUCT;


/* DEVHIDE_B4F3_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA19C)                                                  */
/*       SKX (0x401FA19C)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F3_VCU_FUN2_REG 0x0502419C
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B4F3_VCU_FUN2_STRUCT;


/* DEVHIDE_B4F4_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1A0)                                                  */
/*       SKX (0x401FA1A0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F4_VCU_FUN2_REG 0x050241A0
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B4F4_VCU_FUN2_STRUCT;


/* DEVHIDE_B4F5_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1A4)                                                  */
/*       SKX (0x401FA1A4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F5_VCU_FUN2_REG 0x050241A4
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B4F5_VCU_FUN2_STRUCT;


/* DEVHIDE_B4F6_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1A8)                                                  */
/*       SKX (0x401FA1A8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F6_VCU_FUN2_REG 0x050241A8
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B4F6_VCU_FUN2_STRUCT;


/* DEVHIDE_B4F7_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1AC)                                                  */
/*       SKX (0x401FA1AC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B4F7_VCU_FUN2_REG 0x050241AC
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B4F7_VCU_FUN2_STRUCT;


/* DEVHIDE_B5F0_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1B0)                                                  */
/*       SKX (0x401FA1B0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F0_VCU_FUN2_REG 0x050241B0
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B5F0_VCU_FUN2_STRUCT;


/* DEVHIDE_B5F1_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1B4)                                                  */
/*       SKX (0x401FA1B4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F1_VCU_FUN2_REG 0x050241B4
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B5F1_VCU_FUN2_STRUCT;


/* DEVHIDE_B5F2_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1B8)                                                  */
/*       SKX (0x401FA1B8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F2_VCU_FUN2_REG 0x050241B8
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B5F2_VCU_FUN2_STRUCT;


/* DEVHIDE_B5F3_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1BC)                                                  */
/*       SKX (0x401FA1BC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F3_VCU_FUN2_REG 0x050241BC
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B5F3_VCU_FUN2_STRUCT;


/* DEVHIDE_B5F4_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1C0)                                                  */
/*       SKX (0x401FA1C0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F4_VCU_FUN2_REG 0x050241C0
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B5F4_VCU_FUN2_STRUCT;


/* DEVHIDE_B5F5_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1C4)                                                  */
/*       SKX (0x401FA1C4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F5_VCU_FUN2_REG 0x050241C4
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B5F5_VCU_FUN2_STRUCT;


/* DEVHIDE_B5F6_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1C8)                                                  */
/*       SKX (0x401FA1C8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F6_VCU_FUN2_REG 0x050241C8
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B5F6_VCU_FUN2_STRUCT;


/* DEVHIDE_B5F7_VCU_FUN2_REG supported on:                                    */
/*       SKX_A0 (0x401FA1CC)                                                  */
/*       SKX (0x401FA1CC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE_B5F7_VCU_FUN2_REG 0x050241CC
/* Struct format extracted from XML file SKX\1.31.2.CFG.xml.
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
} DEVHIDE_B5F7_VCU_FUN2_STRUCT;


#endif /* VCU_FUN2_h */
