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

#ifndef MCDDC_DP_h
#define MCDDC_DP_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* MCDDC_DP_DEV 10                                                            */
/* MCDDC_DP_FUN 3                                                             */

/* VID_MCDDC_DP_REG supported on:                                             */
/*       SKX_A0 (0x20253000)                                                  */
/*       SKX (0x20253000)                                                     */
/* Register default value:              0x8086                                */
#define VID_MCDDC_DP_REG 0x0B012000
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} VID_MCDDC_DP_STRUCT;


/* DID_MCDDC_DP_REG supported on:                                             */
/*       SKX_A0 (0x20253002)                                                  */
/*       SKX (0x20253002)                                                     */
/* Register default value:              0x2043                                */
#define DID_MCDDC_DP_REG 0x0B012002
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2043 
        
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
} DID_MCDDC_DP_STRUCT;


/* PCICMD_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x20253004)                                                  */
/*       SKX (0x20253004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_MCDDC_DP_REG 0x0B012004
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} PCICMD_MCDDC_DP_STRUCT;


/* PCISTS_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x20253006)                                                  */
/*       SKX (0x20253006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_MCDDC_DP_REG 0x0B012006
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} PCISTS_MCDDC_DP_STRUCT;


/* RID_MCDDC_DP_REG supported on:                                             */
/*       SKX_A0 (0x10253008)                                                  */
/*       SKX (0x10253008)                                                     */
/* Register default value:              0x00                                  */
#define RID_MCDDC_DP_REG 0x0B011008
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} RID_MCDDC_DP_STRUCT;


/* CCR_N0_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x10253009)                                                  */
/*       SKX (0x10253009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_MCDDC_DP_REG 0x0B011009
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_MCDDC_DP_STRUCT;


/* CCR_N1_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x2025300A)                                                  */
/*       SKX (0x2025300A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_MCDDC_DP_REG 0x0B01200A
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} CCR_N1_MCDDC_DP_STRUCT;


/* CLSR_MCDDC_DP_REG supported on:                                            */
/*       SKX_A0 (0x1025300C)                                                  */
/*       SKX (0x1025300C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_MCDDC_DP_REG 0x0B01100C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} CLSR_MCDDC_DP_STRUCT;


/* PLAT_MCDDC_DP_REG supported on:                                            */
/*       SKX_A0 (0x1025300D)                                                  */
/*       SKX (0x1025300D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_MCDDC_DP_REG 0x0B01100D
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} PLAT_MCDDC_DP_STRUCT;


/* HDR_MCDDC_DP_REG supported on:                                             */
/*       SKX_A0 (0x1025300E)                                                  */
/*       SKX (0x1025300E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_MCDDC_DP_REG 0x0B01100E
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} HDR_MCDDC_DP_STRUCT;


/* BIST_MCDDC_DP_REG supported on:                                            */
/*       SKX_A0 (0x1025300F)                                                  */
/*       SKX (0x1025300F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_MCDDC_DP_REG 0x0B01100F
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} BIST_MCDDC_DP_STRUCT;


/* SVID_MCDDC_DP_REG supported on:                                            */
/*       SKX_A0 (0x2025302C)                                                  */
/*       SKX (0x2025302C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_MCDDC_DP_REG 0x0B01202C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x02c
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_MCDDC_DP_STRUCT;


/* SDID_MCDDC_DP_REG supported on:                                            */
/*       SKX_A0 (0x2025302E)                                                  */
/*       SKX (0x2025302E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_MCDDC_DP_REG 0x0B01202E
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x02e
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_MCDDC_DP_STRUCT;


/* CAPPTR_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x10253034)                                                  */
/*       SKX (0x10253034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_MCDDC_DP_REG 0x0B011034
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} CAPPTR_MCDDC_DP_STRUCT;


/* INTL_MCDDC_DP_REG supported on:                                            */
/*       SKX_A0 (0x1025303C)                                                  */
/*       SKX (0x1025303C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_MCDDC_DP_REG 0x0B01103C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} INTL_MCDDC_DP_STRUCT;


/* INTPIN_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x1025303D)                                                  */
/*       SKX (0x1025303D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_MCDDC_DP_REG 0x0B01103D
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} INTPIN_MCDDC_DP_STRUCT;


/* MINGNT_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x1025303E)                                                  */
/*       SKX (0x1025303E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_MCDDC_DP_REG 0x0B01103E
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} MINGNT_MCDDC_DP_STRUCT;


/* MAXLAT_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x1025303F)                                                  */
/*       SKX (0x1025303F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_MCDDC_DP_REG 0x0B01103F
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
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
} MAXLAT_MCDDC_DP_STRUCT;


/* PXPCAP_MCDDC_DP_REG supported on:                                          */
/*       SKX_A0 (0x40253040)                                                  */
/*       SKX (0x40253040)                                                     */
/* Register default value:              0x00910010                            */
#define PXPCAP_MCDDC_DP_REG 0x0B014040
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x040
 */
typedef union {
  struct {
    UINT32 capability_id : 8;
    /* capability_id - Bits[7:0], RO, default = 8'b00010000 
       Provides the PCI Express capability ID assigned by PCI-SIG.
     */
    UINT32 next_ptr : 8;
    /* next_ptr - Bits[15:8], RO, default = 8'b00000000 
       Pointer to the next capability. Set to 0 to indicate there are no more 
       capability structures. 
     */
    UINT32 capability_version : 4;
    /* capability_version - Bits[19:16], RO, default = 4'b0001 
       PCI Express Capability is Compliant with Version 1.0 of the PCI Express Spec.
       Note:
       This capability structure is not compliant with Versions beyond 1.0, since they 
       require additional capability registers to be reserved. The only purpose for 
       this capability structure is to make enhanced configuration space available. 
       Minimizing the size of this structure is accomplished by reporting version 1.0 
       compliancy and reporting that this is an integrated root port device. As such, 
       only three Dwords of configuration space are required for this structure. 
     */
    UINT32 device_port_type : 4;
    /* device_port_type - Bits[23:20], RO, default = 4'b1001 
       Device type is Root Complex Integrated Endpoint
     */
    UINT32 slot_implemented : 1;
    /* slot_implemented - Bits[24:24], RO, default = 1'b0 
       N/A for integrated endpoints
     */
    UINT32 interrupt_message_number : 5;
    /* interrupt_message_number - Bits[29:25], RO, default = 5'b00000 
       N/A for this device
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PXPCAP_MCDDC_DP_STRUCT;


/* ROUNDTRIP0_MCDDC_DP_REG supported on:                                      */
/*       SKX_A0 (0x40253080)                                                  */
/*       SKX (0x40253080)                                                     */
/* Register default value:              0x0B0B0B0B                            */
#define ROUNDTRIP0_MCDDC_DP_REG 0x0B014080
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x080
 */
typedef union {
  struct {
    UINT32 rt_rank0 : 8;
    /* rt_rank0 - Bits[7:0], RW, default = 8'b00001011 
       Rank 0 round trip latency in QCLK, 88 max configurable value
     */
    UINT32 rt_rank1 : 8;
    /* rt_rank1 - Bits[15:8], RW, default = 8'b00001011 
       Rank 1 round trip latency in QCLK
     */
    UINT32 rt_rank2 : 8;
    /* rt_rank2 - Bits[23:16], RW, default = 8'b00001011 
       Rank 2 round trip latency in QCLK
     */
    UINT32 rt_rank3 : 8;
    /* rt_rank3 - Bits[31:24], RW, default = 8'b00001011 
       Rank 3 round trip latency in QCLK
     */
  } Bits;
  UINT32 Data;
} ROUNDTRIP0_MCDDC_DP_STRUCT;


/* ROUNDTRIP1_MCDDC_DP_REG supported on:                                      */
/*       SKX_A0 (0x40253084)                                                  */
/*       SKX (0x40253084)                                                     */
/* Register default value:              0x0B0B0B0B                            */
#define ROUNDTRIP1_MCDDC_DP_REG 0x0B014084
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x084
 */
typedef union {
  struct {
    UINT32 rt_rank4 : 8;
    /* rt_rank4 - Bits[7:0], RW, default = 8'b00001011 
       Rank 4 round trip latency in QCLK
     */
    UINT32 rt_rank5 : 8;
    /* rt_rank5 - Bits[15:8], RW, default = 8'b00001011 
       Rank 5 round trip latency in QCLK
     */
    UINT32 rt_rank6 : 8;
    /* rt_rank6 - Bits[23:16], RW, default = 8'b00001011 
       Rank 6 round trip latency in QCLK
     */
    UINT32 rt_rank7 : 8;
    /* rt_rank7 - Bits[31:24], RW, default = 8'b00001011 
       Rank 7 round trip latency in QCLK
     */
  } Bits;
  UINT32 Data;
} ROUNDTRIP1_MCDDC_DP_STRUCT;


/* IOLATENCY0_MCDDC_DP_REG supported on:                                      */
/*       SKX_A0 (0x4025308C)                                                  */
/*       SKX (0x4025308C)                                                     */
/* Register default value:              0x00000000                            */
#define IOLATENCY0_MCDDC_DP_REG 0x0B01408C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x08c
 */
typedef union {
  struct {
    UINT32 io_lat_rank0 : 4;
    /* io_lat_rank0 - Bits[3:0], RW, default = 4'b0000 
       Rank 0 IO latency in QCLK
     */
    UINT32 io_lat_rank1 : 4;
    /* io_lat_rank1 - Bits[7:4], RW, default = 4'b0000 
       Rank 1 IO latency in QCLK
     */
    UINT32 io_lat_rank2 : 4;
    /* io_lat_rank2 - Bits[11:8], RW, default = 4'b0000 
       Rank 2 IO latency in QCLK
     */
    UINT32 io_lat_rank3 : 4;
    /* io_lat_rank3 - Bits[15:12], RW, default = 4'b0000 
       Rank 3 IO latency in QCLK
     */
    UINT32 io_lat_rank4 : 4;
    /* io_lat_rank4 - Bits[19:16], RW, default = 4'b0000 
       Rank 4 IO latency in QCLK
     */
    UINT32 io_lat_rank5 : 4;
    /* io_lat_rank5 - Bits[23:20], RW, default = 4'b0000 
       Rank 5 IO latency in QCLK
     */
    UINT32 io_lat_rank6 : 4;
    /* io_lat_rank6 - Bits[27:24], RW, default = 4'b0000 
       Rank 6 IO latency in QCLK
     */
    UINT32 io_lat_rank7 : 4;
    /* io_lat_rank7 - Bits[31:28], RW, default = 4'b0000 
       Rank 7 IO latency in QCLK
     */
  } Bits;
  UINT32 Data;
} IOLATENCY0_MCDDC_DP_STRUCT;


/* IOLATENCY1_MCDDC_DP_REG supported on:                                      */
/*       SKX_A0 (0x40253090)                                                  */
/*       SKX (0x40253090)                                                     */
/* Register default value:              0x00500000                            */
#define IOLATENCY1_MCDDC_DP_REG 0x0B014090
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x090
 */
typedef union {
  struct {
    UINT32 io_lat_io_comp : 8;
    /* io_lat_io_comp - Bits[7:0], RW, default = 8'b00000000  */
    UINT32 cr_wrcrc_enable : 1;
    /* cr_wrcrc_enable - Bits[8:8], RW, default = 1'b0 
           1 = DDR4 write CRC is enabled
                   0 = DDR4 write CRC is disabled
     */
    UINT32 cr_crc_err_inj : 8;
    /* cr_crc_err_inj - Bits[16:9], RW, default = 8'b00000000 
       All the bits those are high, lead to inversion of correct CRC in the 8
       	 bit CRC generated per device (be it x4 or x8)
       	 Note that there is no provision for injecting error in the all-ones of the
                9th beat of X8 device CRC
     */
    UINT32 cr_oneshot_err_inj : 1;
    /* cr_oneshot_err_inj - Bits[17:17], RW, default = 1'b0 
           When this bit is set high, then the first write operation is injected with
       	 a CRC error as defined by cr_crc_err_inj field. The subsequent writes are 
                not injected with errors.
     */
    UINT32 wr_io_lat_io_comp : 8;
    /* wr_io_lat_io_comp - Bits[25:18], RW, default = 8'b00010100  */
    UINT32 rsvd_26 : 2;
    /* rsvd_26 - Bits[27:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cr_cont_wrcrc_err_inj : 1;
    /* cr_cont_wrcrc_err_inj - Bits[28:28], RW, default = 1'b0  */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IOLATENCY1_MCDDC_DP_STRUCT;


/* CMPL_TO_DATA_DELAY_MCDDC_DP_REG supported on:                              */
/*       SKX_A0 (0x402530A0)                                                  */
/*       SKX (0x402530A0)                                                     */
/* Register default value:              0x0000000C                            */
#define CMPL_TO_DATA_DELAY_MCDDC_DP_REG 0x0B0140A0
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * specify the number of qclk between early data return and data. legal values are 
 * decimal 12-24. 
 */
typedef union {
  struct {
    UINT32 rt_bit : 5;
    /* rt_bit - Bits[4:0], RW_LB, default = 5'd12 
       rt bit
     */
    UINT32 cmpl_program_delay : 3;
    /* cmpl_program_delay - Bits[7:5], RW_LB, default = 3'd0 
       cmpl_program_delay
     */
    UINT32 extra_cmpl_delay_rank0 : 2;
    /* extra_cmpl_delay_rank0 - Bits[9:8], RW_LB, default = 2'd0 
       Additional delay to rank 0 early completion; only used when extra_cmpl_delay_en 
       bit is set 
     */
    UINT32 extra_cmpl_delay_rank1 : 2;
    /* extra_cmpl_delay_rank1 - Bits[11:10], RW_LB, default = 2'd0 
       Additional delay to rank 1 early completion; only used when extra_cmpl_delay_en 
       bit is set 
     */
    UINT32 extra_cmpl_delay_rank2 : 2;
    /* extra_cmpl_delay_rank2 - Bits[13:12], RW_LB, default = 2'd0 
       Additional delay to rank 2 early completion; only used when extra_cmpl_delay_en 
       bit is set 
     */
    UINT32 extra_cmpl_delay_rank3 : 2;
    /* extra_cmpl_delay_rank3 - Bits[15:14], RW_LB, default = 2'd0 
       Additional delay to rank 3 early completion; only used when extra_cmpl_delay_en 
       bit is set 
     */
    UINT32 extra_cmpl_delay_rank4 : 2;
    /* extra_cmpl_delay_rank4 - Bits[17:16], RW_LB, default = 2'd0 
       Additional delay to rank 4 early completion; only used when extra_cmpl_delay_en 
       bit is set 
     */
    UINT32 extra_cmpl_delay_rank5 : 2;
    /* extra_cmpl_delay_rank5 - Bits[19:18], RW_LB, default = 2'd0 
       Additional delay to rank 5 early completion; only used when extra_cmpl_delay_en 
       bit is set 
     */
    UINT32 extra_cmpl_delay_rank6 : 2;
    /* extra_cmpl_delay_rank6 - Bits[21:20], RW_LB, default = 2'd0 
       Additional delay to rank 6 early completion; only used when extra_cmpl_delay_en 
       bit is set 
     */
    UINT32 extra_cmpl_delay_rank7 : 2;
    /* extra_cmpl_delay_rank7 - Bits[23:22], RW_LB, default = 2'd0 
       Additional delay to rank 7 early completion; only used when extra_cmpl_delay_en 
       bit is set 
     */
    UINT32 extra_cmpl_delay_en : 1;
    /* extra_cmpl_delay_en - Bits[24:24], RW_LB, default = 1'd0 
       Enable additional delay on a per-rank basis; only used when 
       mcmtr.enable_slot_use is set to 1 
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CMPL_TO_DATA_DELAY_MCDDC_DP_STRUCT;




/* MCWDB_CHKN_BIT_MCDDC_DP_REG supported on:                                  */
/*       SKX_A0 (0x402530F4)                                                  */
/*       SKX (0x402530F4)                                                     */
/* Register default value:              0x00000001                            */
#define MCWDB_CHKN_BIT_MCDDC_DP_REG 0x0B0140F4
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x0f4
 */
typedef union {
  struct {
    UINT32 dis_ddrio_earlywdata : 1;
    /* dis_ddrio_earlywdata - Bits[0:0], RW_LB, default = 1'b1 
       Disable sending write data early to ddrio for anti cross talk logic; If 0, data 
       will be sent 
       early one Dclock so ddrio has time to do cross talk cancelation logic.
       If set to 1, write data will be sent with normal timing and ddrio will have no 
       time to do cross 
       talk cancelation logic.
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCWDB_CHKN_BIT_MCDDC_DP_STRUCT;


/* PXPENHCAP_MCDDC_DP_REG supported on:                                       */
/*       SKX_A0 (0x40253100)                                                  */
/*       SKX (0x40253100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_MCDDC_DP_REG 0x0B014100
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This field points to the next Capability in extended configuration space.
 */
typedef union {
  struct {
    UINT32 capability_id : 16;
    /* capability_id - Bits[15:0], RO, default = 16'b0000000000000000 
       Indicates there are no capability structures in the enhanced configuration 
       space. 
     */
    UINT32 capability_version : 4;
    /* capability_version - Bits[19:16], RO, default = 4'b0000 
       Indicates there are no capability structures in the enhanced configuration 
       space. 
     */
    UINT32 next_capability_offset : 12;
    /* next_capability_offset - Bits[31:20], RO, default = 12'b000000000000  */
  } Bits;
  UINT32 Data;
} PXPENHCAP_MCDDC_DP_STRUCT;


/* CORRERRCNT_0_MCDDC_DP_REG supported on:                                    */
/*       SKX_A0 (0x40253104)                                                  */
/*       SKX (0x40253104)                                                     */
/* Register default value:              0x00000000                            */
#define CORRERRCNT_0_MCDDC_DP_REG 0x0B014104
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Per Rank corrected error counters.
 */
typedef union {
  struct {
    UINT32 cor_err_cnt_0 : 15;
    /* cor_err_cnt_0 - Bits[14:0], RW_LBV, default = 15'b000000000000000 
       The corrected error count for this rank. Hardware automatically clear this field 
       when the corresponding OVERFLOW_x bit is changing from 0 to 1. 
       This counter increments in number of cacheline accesses - not by codewords. On a 
       read access, if either of the codewords or both codewords have a corrected 
       error, this counter increments by 1. 
       Register: DEVTAG_CNTL<Rank>, Field FAILDEVICE: This field is updated once per 
       cacheline access not by codeword. On a read access, the device is logged as 
       follows 
       Corr_Err_On_CodeWord_0__Corr_Err_On_CoreWord_1__Device Logged
       Yes_____________________No______________________Corrected Device from CodeWord0
       No______________________Yes_____________________Corrected Device from CodeWord1
       Yes_____________________Yes_____________________Corrected Device from CodeWord0
     */
    UINT32 overflow_0 : 1;
    /* overflow_0 - Bits[15:15], RW1CS, default = 1'b0 
       The corrected error count for this rank has been overflowed. Once set it can 
       only be cleared via a write from BIOS. 
     */
    UINT32 cor_err_cnt_1 : 15;
    /* cor_err_cnt_1 - Bits[30:16], RW_LBV, default = 15'b000000000000000 
       The corrected error count for this rank. Hardware automatically clear this field 
       when the corresponding OVERFLOW_x bit is changing from 0 to 1. 
       This counter increments in number of cacheline accesses - not by codewords. On a 
       read access, if either of the codewords or both codewords have a corrected 
       error, this counter increments by 1. 
       Register: DEVTAG_CNTL<Rank>, Field FAILDEVICE: This field is updated once per 
       cacheline access not by codeword. On a read access, the device is logged as 
       follows 
       Corr_Err_On_CodeWord_0__Corr_Err_On_CoreWord_1__Device Logged
       Yes_____________________No______________________Corrected Device from CodeWord0
       No______________________Yes_____________________Corrected Device from CodeWord1
       Yes_____________________Yes_____________________Corrected Device from CodeWord0
     */
    UINT32 overflow_1 : 1;
    /* overflow_1 - Bits[31:31], RW1CS, default = 1'b0 
       The corrected error count for this rank has been overflowed. Once set it can 
       only be cleared via a write from BIOS. 
     */
  } Bits;
  UINT32 Data;
} CORRERRCNT_0_MCDDC_DP_STRUCT;


/* CORRERRCNT_1_MCDDC_DP_REG supported on:                                    */
/*       SKX_A0 (0x40253108)                                                  */
/*       SKX (0x40253108)                                                     */
/* Register default value:              0x00000000                            */
#define CORRERRCNT_1_MCDDC_DP_REG 0x0B014108
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Per Rank corrected error counters.
 */
typedef union {
  struct {
    UINT32 cor_err_cnt_2 : 15;
    /* cor_err_cnt_2 - Bits[14:0], RW_LBV, default = 15'b000000000000000 
       The corrected error count for this rank.
     */
    UINT32 overflow_2 : 1;
    /* overflow_2 - Bits[15:15], RW1CS, default = 1'b0 
       The corrected error count has crested over the limit for this rank. Once set it 
       can only be cleared via a write from BIOS. 
     */
    UINT32 cor_err_cnt_3 : 15;
    /* cor_err_cnt_3 - Bits[30:16], RW_LBV, default = 15'b000000000000000 
       The corrected error count for this rank.
     */
    UINT32 overflow_3 : 1;
    /* overflow_3 - Bits[31:31], RW1CS, default = 1'b0 
       The corrected error count has crested over the limit for this rank. Once set it 
       can only be cleared via a write from BIOS. 
     */
  } Bits;
  UINT32 Data;
} CORRERRCNT_1_MCDDC_DP_STRUCT;


/* CORRERRCNT_2_MCDDC_DP_REG supported on:                                    */
/*       SKX_A0 (0x4025310C)                                                  */
/*       SKX (0x4025310C)                                                     */
/* Register default value:              0x00000000                            */
#define CORRERRCNT_2_MCDDC_DP_REG 0x0B01410C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Per Rank corrected error counters.
 */
typedef union {
  struct {
    UINT32 cor_err_cnt_4 : 15;
    /* cor_err_cnt_4 - Bits[14:0], RW_LBV, default = 15'b000000000000000 
       The corrected error count for this rank.
     */
    UINT32 overflow_4 : 1;
    /* overflow_4 - Bits[15:15], RW1CS, default = 1'b0 
       The corrected error count has crested over the limit for this rank. Once set it 
       can only be cleared via a write from BIOS. 
     */
    UINT32 cor_err_cnt_5 : 15;
    /* cor_err_cnt_5 - Bits[30:16], RW_LBV, default = 15'b000000000000000 
       The corrected error count for this rank.
     */
    UINT32 overflow_5 : 1;
    /* overflow_5 - Bits[31:31], RW1CS, default = 1'b0 
       The corrected error count has crested over the limit for this rank. Once set it 
       can only be cleared via a write from BIOS. 
     */
  } Bits;
  UINT32 Data;
} CORRERRCNT_2_MCDDC_DP_STRUCT;


/* CORRERRCNT_3_MCDDC_DP_REG supported on:                                    */
/*       SKX_A0 (0x40253110)                                                  */
/*       SKX (0x40253110)                                                     */
/* Register default value:              0x00000000                            */
#define CORRERRCNT_3_MCDDC_DP_REG 0x0B014110
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Per Rank corrected error counters.
 */
typedef union {
  struct {
    UINT32 cor_err_cnt_6 : 15;
    /* cor_err_cnt_6 - Bits[14:0], RW_LBV, default = 15'b000000000000000 
       The corrected error count for this rank.
     */
    UINT32 overflow_6 : 1;
    /* overflow_6 - Bits[15:15], RW1CS, default = 1'b0 
       The corrected error count has crested over the limit for this rank. Once set it 
       can only be cleared via a write from BIOS. 
     */
    UINT32 cor_err_cnt_7 : 15;
    /* cor_err_cnt_7 - Bits[30:16], RW_LBV, default = 15'b000000000000000 
       The corrected error count for this rank.
     */
    UINT32 overflow_7 : 1;
    /* overflow_7 - Bits[31:31], RW1CS, default = 1'b0 
       The corrected error count has crested over the limit for this rank. Once set it 
       can only be cleared via a write from BIOS. 
     */
  } Bits;
  UINT32 Data;
} CORRERRCNT_3_MCDDC_DP_STRUCT;


/* RETRY_RD_ERR_LOG_ADDRESS2_MCDDC_DP_REG supported on:                       */
/*       SKX_A0 (0x40253114)                                                  */
/*       SKX (0x40253114)                                                     */
/* Register default value:              0x00000000                            */
#define RETRY_RD_ERR_LOG_ADDRESS2_MCDDC_DP_REG 0x0B014114
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Logs info on retried reads.  The contents of this register are valid with 
 * RETRY_RD_ERR_LOG. 
 *                    The values in this register indicate the address of failed 
 * device. Row and truncated column address 
 *       
 */
typedef union {
  struct {
    UINT32 row : 21;
    /* row - Bits[20:0], RWS_V, default = 21'h0 
       Row address for the last retry.this feild also include cbit so to get the row 
       address need to ignore the upper 3 bits. 
                            cbit = [20:18] 
                            row  = [17:0] 
     */
    UINT32 rsvd : 11;
    /* rsvd - Bits[31:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RETRY_RD_ERR_LOG_ADDRESS2_MCDDC_DP_STRUCT;




/* CORRERRTHRSHLD_0_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x4025311C)                                                  */
/*       SKX (0x4025311C)                                                     */
/* Register default value:              0x7FFF7FFF                            */
#define CORRERRTHRSHLD_0_MCDDC_DP_REG 0x0B01411C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This register holds the per rank corrected error thresholding value.
 */
typedef union {
  struct {
    UINT32 cor_err_th_0 : 15;
    /* cor_err_th_0 - Bits[14:0], RW_LB, default = 15'b111111111111111 
       The corrected error threshold for this rank that will be compared to the per 
       rank corrected error counter. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cor_err_th_1 : 15;
    /* cor_err_th_1 - Bits[30:16], RW_LB, default = 15'b111111111111111 
       The corrected error threshold for this rank that will be compared to the per 
       rank corrected error counter. 
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CORRERRTHRSHLD_0_MCDDC_DP_STRUCT;


/* CORRERRTHRSHLD_1_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x40253120)                                                  */
/*       SKX (0x40253120)                                                     */
/* Register default value:              0x7FFF7FFF                            */
#define CORRERRTHRSHLD_1_MCDDC_DP_REG 0x0B014120
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This register holds the per rank corrected error thresholding value.
 */
typedef union {
  struct {
    UINT32 cor_err_th_2 : 15;
    /* cor_err_th_2 - Bits[14:0], RW_LB, default = 15'b111111111111111 
       The corrected error threshold for this rank that will be compared to the per 
       rank corrected error counter. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cor_err_th_3 : 15;
    /* cor_err_th_3 - Bits[30:16], RW_LB, default = 15'b111111111111111 
       The corrected error threshold for this rank that will be compared to the per 
       rank corrected error counter. 
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CORRERRTHRSHLD_1_MCDDC_DP_STRUCT;


/* CORRERRTHRSHLD_2_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x40253124)                                                  */
/*       SKX (0x40253124)                                                     */
/* Register default value:              0x7FFF7FFF                            */
#define CORRERRTHRSHLD_2_MCDDC_DP_REG 0x0B014124
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This register holds the per rank corrected error thresholding value.
 */
typedef union {
  struct {
    UINT32 cor_err_th_4 : 15;
    /* cor_err_th_4 - Bits[14:0], RW_LB, default = 15'b111111111111111 
       The corrected error threshold for this rank that will be compared to the per 
       rank corrected error counter. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cor_err_th_5 : 15;
    /* cor_err_th_5 - Bits[30:16], RW_LB, default = 15'b111111111111111 
       The corrected error threshold for this rank that will be compared to the per 
       rank corrected error counter. 
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CORRERRTHRSHLD_2_MCDDC_DP_STRUCT;


/* CORRERRTHRSHLD_3_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x40253128)                                                  */
/*       SKX (0x40253128)                                                     */
/* Register default value:              0x7FFF7FFF                            */
#define CORRERRTHRSHLD_3_MCDDC_DP_REG 0x0B014128
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This register holds the per rank corrected error thresholding value.
 */
typedef union {
  struct {
    UINT32 cor_err_th_6 : 15;
    /* cor_err_th_6 - Bits[14:0], RW_LB, default = 15'b111111111111111 
       The corrected error threshold for this rank that will be compared to the per 
       rank corrected error counter. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cor_err_th_7 : 15;
    /* cor_err_th_7 - Bits[30:16], RW_LB, default = 15'b111111111111111 
       The corrected error threshold for this rank that will be compared to the per 
       rank corrected error counter. 
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CORRERRTHRSHLD_3_MCDDC_DP_STRUCT;






/* CORRERRORSTATUS_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253134)                                                  */
/*       SKX (0x40253134)                                                     */
/* Register default value:              0x00003000                            */
#define CORRERRORSTATUS_MCDDC_DP_REG 0x0B014134
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Per rank corrected error status. These bits are reset by bios.
 */
typedef union {
  struct {
    UINT32 err_overflow_stat : 8;
    /* err_overflow_stat - Bits[7:0], RW1C, default = 8'b00000000 
       This 8 bit field is the per rank error over-threshold status bits. The 
       organization is as follows: 
       Bit 0 : Rank 0
       Bit 1 : Rank 1
       Bit 2 : Rank 2
       Bit 3 : Rank 3
       Bit 4 : Rank 4
       Bit 5 : Rank 5
       Bit 6 : Rank 6
       Bit 7 : Rank 7
       Note: The register tracks which rank has reached or exceeded the corresponding 
       CORRERRTHRSHLD threshold settings. 
     */
    UINT32 dimm_alert : 3;
    /* dimm_alert - Bits[10:8], RW_V, default = 3'h0  */
    UINT32 dimm_alert_select : 3;
    /* dimm_alert_select - Bits[13:11], RW, default = 3'h6  */
    UINT32 ddr4crc_rank_log : 8;
    /* ddr4crc_rank_log - Bits[21:14], RW_V, default = 8'h0 
       This field get set with 1'b1 , if the corresponding rank detected ddr4 crc in 
       one of its write data. This will be cleared by BIOS 
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CORRERRORSTATUS_MCDDC_DP_STRUCT;


/* LEAKY_BKT_2ND_CNTR_REG_MCDDC_DP_REG supported on:                          */
/*       SKX_A0 (0x40253138)                                                  */
/*       SKX (0x40253138)                                                     */
/* Register default value:              0x00000000                            */
#define LEAKY_BKT_2ND_CNTR_REG_MCDDC_DP_REG 0x0B014138
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x138
 */
typedef union {
  struct {
    UINT32 leaky_bkt_2nd_cntr : 16;
    /* leaky_bkt_2nd_cntr - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Per rank secondary leaky bucket counter (2b per rank)
       bit 15:14: rank 7 secondary leaky bucket counter
       bit 13:12: rank 6 secondary leaky bucket counter
       bit 11:10: rank 5 secondary leaky bucket counter
       bit 9:8: rank 4 secondary leaky bucket counter
       bit 7:6: rank 3 secondary leaky bucket counter
       bit 5:4: rank 2 secondary leaky bucket counter
       bit 3:2: rank 1 secondary leaky bucket counter
       bit 1:0: rank 0 secondary leaky bucket counter
     */
    UINT32 leaky_bkt_2nd_cntr_limit : 16;
    /* leaky_bkt_2nd_cntr_limit - Bits[31:16], RW, default = 16'b0000000000000000 
       Secondary Leaky Bucket Counter Limit (2b per DIMM). This register defines 
       secondary leaky bucket counter limit for all 8 logical ranks within channel. The 
       counter logic will generate the secondary LEAK pulse to decrement the rank's 
       correctable error counter by 1 when the corresponding rank leaky bucket rank 
       counter roll over at the predefined counter limit. The counter increment at the 
       primary leak pulse from the LEAKY_BUCKET_CNTR_LO and LEAKY_BUCKET_CNTR_HI logic. 
       Bit[31:30]: Rank 7 Secondary Leaky Bucket Counter Limit
       Bit[29:28]: Rank 6 Secondary Leaky Bucket Counter Limit
       Bit[27:26]: Rank 5 Secondary Leaky Bucket Counter Limit
       Bit[25:24]: Rank 4 Secondary Leaky Bucket Counter Limit
       Bit[23:22]: Rank 3 Secondary Leaky Bucket Counter Limit
       Bit[21:20]: Rank 2 Secondary Leaky Bucket Counter Limit
       Bit[19:18]: Rank 1 Secondary Leaky Bucket Counter Limit
       Bit[17:16]: Rank 0 Secondary Leaky Bucket Counter Limit
       The value of the limit is defined as the following:
       0: the LEAK pulse is generated one DCLK after the primary LEAK pulse is 
       asserted. 
       1: the LEAK pulse is generated one DCLK after the counter roll over at 1.
       2: the LEAK pulse is generated one DCLK after the counter roll over at 2.
       3: the LEAK pulse is generated one DCLK after the counter roll over at 3.
     */
  } Bits;
  UINT32 Data;
} LEAKY_BKT_2ND_CNTR_REG_MCDDC_DP_STRUCT;




/* DEVTAG_CNTL_0_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253140)                                                  */
/*       SKX (0x10253140)                                                     */
/* Register default value:              0x3F                                  */
#define DEVTAG_CNTL_0_MCDDC_DP_REG 0x0B011140
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * SDDC Usage model -
 * When the number of correctable errors (CORRERRCNT_x) from a particular rank 
 * exceeds the corresponding threshold (CORRERRTHRSHLD_y), hardware 
 * will generate a LINK interrupt and log (and preserve) the failing device in the 
 * FailDevice field. SMM software will read the failing device on the particular 
 * rank. Software then set the EN bit to enable substituion of the failing 
 * device/rank with the parity from the rest of the devices inline. 
 * For independent channel configuration, each rank can tag once. Up to 8 ranks can 
 * be tagged. 
 * For lock-step channel configuration, only one x8 device can be tagged per 
 * rank-pair. SMM software must identify which channel should be tagged for this 
 * rank and only set the valid bit for the channel from the channel-pair. 
 * There is no hardware logic to report incorrect programming error. Unpredicable 
 * error and/or silent data corruption will be the consequence of such programming 
 * error. 
 * If the rank-sparing is enabled, it is recommend to prioritize the rank-sparing 
 * before triggering the device tagging due to the nature of the device tagging 
 * would drop the correction capability and any subsequent ECC error from this rank 
 * would cause uncorrectable error. 
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       Hardware will capture the fail device ID of the rank in the FailDevice field 
       upon successfull correction from the device correction engine. After SDDC is 
       enabled HW may not update this field. 
       Native DDR/Intel LINK 2 2:1/Intel LINK 2 1:1x8:
       Valid Range is decimal 0-17 to indicate which x4 device (independent channel) or 
       x8 device (lock-step mode) has failed. 
       Intel LINK 2 1:1x4(DDDC):
       Valid Range is decimal 0-35 to indicate which x4 device has failed.
       Note that when DDDC has been enabled on the non-spare device, and a subsequent 
       failure of the spare device occurs, the value logged here will be equal to the 
       DDDC faildevice. 
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       Device tagging (SDDC) enable for this rank. Once set, the parity device of the 
       rank is used for the replacement device content. After tagging, the rank will no 
       longer have the "correction" capability. ECC error "detection" capability will 
       not degrade after setting this bit. 
       Warning: For lock-step channel configuration, only one x8 device can be tagged 
       per rank-pair. SMM software must identify which channel should be tagged for 
       this rank and only set the corresponding DEVTAG_CNTL_x.EN bit for the channel 
       contains the fail device. The DEVTAG_CNTL_x.EN on the other channel of the 
       corresponding rank must not be set. 
       DDDC: (EX processor only)
       On DDDC supported systems, BIOS has the option to enable SDDC in conjuntion with 
       DDDC_CNTL:SPARING to enable faster sparing with SDDC substitution. This field is 
       cleared by HW on completion of DDDC sparing. 
     */
  } Bits;
  UINT8 Data;
} DEVTAG_CNTL_0_MCDDC_DP_STRUCT;


/* DEVTAG_CNTL_1_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253141)                                                  */
/*       SKX (0x10253141)                                                     */
/* Register default value:              0x3F                                  */
#define DEVTAG_CNTL_1_MCDDC_DP_REG 0x0B011141
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DEVTAG_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DEVTAG_CNTL_0 for description.
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DEVTAG_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DEVTAG_CNTL_1_MCDDC_DP_STRUCT;


/* DEVTAG_CNTL_2_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253142)                                                  */
/*       SKX (0x10253142)                                                     */
/* Register default value:              0x3F                                  */
#define DEVTAG_CNTL_2_MCDDC_DP_REG 0x0B011142
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DEVTAG_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DEVTAG_CNTL_0 for description.
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DEVTAG_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DEVTAG_CNTL_2_MCDDC_DP_STRUCT;


/* DEVTAG_CNTL_3_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253143)                                                  */
/*       SKX (0x10253143)                                                     */
/* Register default value:              0x3F                                  */
#define DEVTAG_CNTL_3_MCDDC_DP_REG 0x0B011143
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DEVTAG_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DEVTAG_CNTL_0 for description.
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DEVTAG_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DEVTAG_CNTL_3_MCDDC_DP_STRUCT;


/* DEVTAG_CNTL_4_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253144)                                                  */
/*       SKX (0x10253144)                                                     */
/* Register default value:              0x3F                                  */
#define DEVTAG_CNTL_4_MCDDC_DP_REG 0x0B011144
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DEVTAG_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DEVTAG_CNTL_0 for description.
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DEVTAG_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DEVTAG_CNTL_4_MCDDC_DP_STRUCT;


/* DEVTAG_CNTL_5_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253145)                                                  */
/*       SKX (0x10253145)                                                     */
/* Register default value:              0x3F                                  */
#define DEVTAG_CNTL_5_MCDDC_DP_REG 0x0B011145
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DEVTAG_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DEVTAG_CNTL_0 for description.
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DEVTAG_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DEVTAG_CNTL_5_MCDDC_DP_STRUCT;


/* DEVTAG_CNTL_6_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253146)                                                  */
/*       SKX (0x10253146)                                                     */
/* Register default value:              0x3F                                  */
#define DEVTAG_CNTL_6_MCDDC_DP_REG 0x0B011146
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DEVTAG_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DEVTAG_CNTL_0 for description.
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DEVTAG_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DEVTAG_CNTL_6_MCDDC_DP_STRUCT;


/* DEVTAG_CNTL_7_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253147)                                                  */
/*       SKX (0x10253147)                                                     */
/* Register default value:              0x3F                                  */
#define DEVTAG_CNTL_7_MCDDC_DP_REG 0x0B011147
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DEVTAG_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DEVTAG_CNTL_0 for description.
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DEVTAG_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DEVTAG_CNTL_7_MCDDC_DP_STRUCT;


/* RETRY_RD_ERR_LOG_LOCATOR_MCDDC_DP_REG supported on:                        */
/*       SKX_A0 (0x40253148)                                                  */
/* Register default value:              0x00000000                            */
#define RETRY_RD_ERR_LOG_LOCATOR_MCDDC_DP_REG 0x0B014148
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * Logs info on retried reads.  The contents of this register are valid with 
 * RETRY_RD_ERR_LOG 
 */
typedef union {
  struct {
    UINT32 loc_syn : 16;
    /* loc_syn - Bits[15:0], RWS_V, default = 16'h0 
        this register hold the value of locator syndrome from correction path. the 
       number of LS bits are the same for sddc and adddc ecc 
     */
    UINT32 plus1_loc_syn : 12;
    /* plus1_loc_syn - Bits[27:16], RWS_V, default = 12'h0 
       this register hold the value of plus1 locator syndrome from correction path. the 
       number of plus1 LS bits are the same for all the plus1 modes  
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RETRY_RD_ERR_LOG_LOCATOR_MCDDC_DP_STRUCT;




/* RETRY_RD_ERR_LOG_PARITY_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253150)                                                  */
/*       SKX (0x40253150)                                                     */
/* Register default value:              0x00000000                            */
#define RETRY_RD_ERR_LOG_PARITY_MCDDC_DP_REG 0x0B014150
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Logs info on retried reads.  The contents of this register are valid with 
 * RETRY_RD_ERR_LOG 
 */
typedef union {
  struct {
    UINT32 par_syn : 32;
    /* par_syn - Bits[31:0], RWS_V, default = 32'h0 
        this register hold the parity syndrome(mask) from correction path. for 
       sddc/x8/ddrt parity syndrome is 32 bits.  
                   but for 2LM adddc it is 16 bits so the 16 upper bits are always 0 
       for 2LM adddc: 16 bits 0 + Parity Syndrome (16 bits). 
                   in 1LM adddc reused the 8 upper bits to log the Parity Column 
       Syndrome. so the order for 1LM adddc will be: 
                   PC syn for upper half of CL (4bits)+ PC syn for lower half of CL 
       (4bits)+ 8 bits 0 + Parity Syndrome (16 bits) 
               
     */
  } Bits;
  UINT32 Data;
} RETRY_RD_ERR_LOG_PARITY_MCDDC_DP_STRUCT;


/* RETRY_RD_ERR_LOG_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x40253154)                                                  */
/*       SKX (0x40253154)                                                     */
/* Register default value:              0x0000A000                            */
#define RETRY_RD_ERR_LOG_MCDDC_DP_REG 0x0B014154
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * Logs info on retried reads
 */
typedef union {
  struct {
    UINT32 v : 1;
    /* v - Bits[0:0], RWS_V, default = 1'b0 
       The contents of this register have valid data as well as 
       RETRY_RD_ERR_LOG_PARITY, RETRY_RD_ERR_LOG_LOCATOR, RETRY_RD_ERR_LOG_ADDRESS1, 
       and RETRY_RD_ERR_LOG_ADDRESS2. 
     */
    UINT32 uc : 1;
    /* uc - Bits[1:1], RWS_V, default = 1'b0 
       Uncorrectable error. Logs cannot be overwritten while UC is set. This bit is not 
       controlled by MC, but by a lock indication from HA. 
     */
    UINT32 over : 1;
    /* over - Bits[2:2], RWS_V, default = 1'b0 
       The log has been overwritten.
     */
    UINT32 mode : 4;
    /* mode - Bits[6:3], RWS_V, default = 4'h0 
       indicate the ecc mode the register is logging for :
       		 4'b0000 => sddc 2LM
       		 4'b0001 => sddc 1LM
       		 4'b0010 => sddc +1  2LM
       		 4'b0011 => sddc +1 1LM
       		 4'b0100 => adddc 2LM
       		 4'b0101 => adddc 1LM
       		 4'b0110 => adddc +1 2LM
       		 4'b0111 => adddc +1 1LM
       		 4'b1000 => ddrt
       		 4'b1001 => x8 sddc
       		 4'b1010 => x8 sddc +1   
       		 4'b1011 => not a valid ecc mode                     
               
     */
    UINT32 corr_err : 1;
    /* corr_err - Bits[7:7], RWS_V, default = 1'b0 
       During the last log, entered correction but was not able to correct hte error => 
       DUE 
     */
    UINT32 corr_noerr : 1;
    /* corr_noerr - Bits[8:8], RWS_V, default = 1'b0 
       During the last log, entered correction but no ecc error was detected
     */
    UINT32 corr_correctable : 1;
    /* corr_correctable - Bits[9:9], RWS_V, default = 1'b0 
       During the last log, entered correction and detect a correctable ecc error and  
       corrected 
     */
    UINT32 corr_correctable_par_dev : 1;
    /* corr_correctable_par_dev - Bits[10:10], RWS_V, default = 1'b0 
       During the last log, entered correction, error was in parity device which 
       consider a correctable error  
     */
    UINT32 ecc_err : 1;
    /* ecc_err - Bits[11:11], RWS_V, default = 1'b0 
       During the last log, the ecc error indication sent to m2m
     */
    UINT32 patspr : 1;
    /* patspr - Bits[12:12], RWS_V, default = 1'b0 
       If en_patspr is set, this will indicate if the log is from patrol/spr or HA.
     */
    UINT32 en_patspr : 1;
    /* en_patspr - Bits[13:13], RWS, default = 1'b1 
       Default operation is to log only HA generated retries. Setting this bit will 
       enable logging of patrol and sparing retries. Note: Use of this bit may cause a 
       patrol or spare log to be locked by HA lock signal. 
     */
    UINT32 noover : 1;
    /* noover - Bits[14:14], RWS, default = 1'b0 
       Lock register after the first error, do not overflow.
     */
    UINT32 en : 1;
    /* en - Bits[15:15], RWS, default = 1'b1 
       Enable error logging. Will log on every retry.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RETRY_RD_ERR_LOG_MCDDC_DP_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Logs info on retried reads
 */
typedef union {
  struct {
    UINT32 v : 1;
    /* v - Bits[0:0], RWS_V, default = 1'b0 
       The contents of this register have valid data as well as 
       RETRY_RD_ERR_LOG_PARITY, RETRY_RD_ERR_LOG_MISC, RETRY_RD_ERR_LOG_ADDRESS1, and 
       RETRY_RD_ERR_LOG_ADDRESS2. 
     */
    UINT32 uc : 1;
    /* uc - Bits[1:1], RWS_V, default = 1'b0 
       Uncorrectable error. Logs cannot be overwritten while UC is set. This bit is not 
       controlled by MC, but by a lock indication from HA. 
     */
    UINT32 over : 1;
    /* over - Bits[2:2], RWS_V, default = 1'b0 
       The log has been overwritten.
     */
    UINT32 mode : 4;
    /* mode - Bits[6:3], RWS_V, default = 4'h0 
       indicate the ecc mode the register is logging for :
       		 4'b0000 => sddc 2LM
       		 4'b0001 => sddc 1LM
       		 4'b0010 => sddc +1  2LM
       		 4'b0011 => sddc +1 1LM
       		 4'b0100 => adddc 2LM
       		 4'b0101 => adddc 1LM
       		 4'b0110 => adddc +1 2LM
       		 4'b0111 => adddc +1 1LM
       		 4'b1000 => read is from ddrt dimm
       		 4'b1001 => x8 sddc
       		 4'b1010 => x8 sddc +1   
       		 4'b1011 => not a valid ecc mode                     
               
     */
    UINT32 corr_err : 1;
    /* corr_err - Bits[7:7], RWS_V, default = 1'b0 
       During the last log, entered correction but was not able to correct hte error => 
       DUE 
     */
    UINT32 corr_noerr : 1;
    /* corr_noerr - Bits[8:8], RWS_V, default = 1'b0 
       During the last log, entered correction but no ecc error was detected
     */
    UINT32 corr_correctable : 1;
    /* corr_correctable - Bits[9:9], RWS_V, default = 1'b0 
       During the last log, entered correction and detect a correctable ecc error and  
       corrected 
     */
    UINT32 corr_correctable_par_dev : 1;
    /* corr_correctable_par_dev - Bits[10:10], RWS_V, default = 1'b0 
       During the last log, entered correction, error was in parity device which 
       consider a correctable error  
     */
    UINT32 ecc_err : 1;
    /* ecc_err - Bits[11:11], RWS_V, default = 1'b0 
       During the last log, the ecc error indication sent to m2m
     */
    UINT32 patspr : 1;
    /* patspr - Bits[12:12], RWS_V, default = 1'b0 
       If en_patspr is set, this will indicate if the log is from patrol/spr or HA.
     */
    UINT32 en_patspr : 1;
    /* en_patspr - Bits[13:13], RWS, default = 1'b1 
       Default operation is to log only HA generated retries. Setting this bit will 
       enable logging of patrol and sparing retries. Note: Use of this bit may cause a 
       patrol or spare log to be locked by HA lock signal. 
     */
    UINT32 noover : 1;
    /* noover - Bits[14:14], RWS, default = 1'b0 
       Lock register after the first error, do not overflow.
     */
    UINT32 en : 1;
    /* en - Bits[15:15], RWS, default = 1'b1 
       Enable error logging. Will log on every retry.
     */
    UINT32 col_correction : 1;
    /* col_correction - Bits[16:16], RWS_V, default = 1'b0 
       This field indicate a column correction done instead of the device correction 
       and is only valid for 1LM adddc mode (retry_rd_err_log.mode = 4'b0101). In all 
       other modes, this field should be ignored. 
     */
    UINT32 rsvd : 15;
    /* rsvd - Bits[31:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RETRY_RD_ERR_LOG_MCDDC_DP_STRUCT;



/* VMSE_MCA_CTL_MCDDC_DP_REG supported on:                                    */
/*       SKX_A0 (0x40253158)                                                  */
/* Register default value:              0x00000000                            */
#define VMSE_MCA_CTL_MCDDC_DP_REG 0x0B014158
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * MC bank and signalling control. 
 * err4: Sb Persistent Counter Reached
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 16;
    UINT32 err4_en : 1;
    /* err4_en - Bits[16:16], RW_LB, default = 1'b0 
       Enable CSR logging. If set, the error will be logged in the corresponding log 
       field in this register. 
     */
    UINT32 err4_log : 1;
    /* err4_log - Bits[17:17], RW1CS, default = 1'b0 
       Error was logged by HW. BIOS clears by writing 0.
     */
    UINT32 err4_cmci : 1;
    /* err4_cmci - Bits[18:18], RW_LB, default = 1'b0 
       Enable/Disable CMCI signaling. When disabled, these errors will not be included 
       in the Corrected Error Count. Note: for corrected errors in general, CMCI is 
       only signaled when corr_err_cnt==corr_err_threshold. 
     */
    UINT32 err4_smi : 1;
    /* err4_smi - Bits[19:19], RW_LB, default = 1'b0 
       Enable/Disable SMI signaling each time error occurs.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VMSE_MCA_CTL_MCDDC_DP_STRUCT;


/* RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_REG supported on:                       */
/*       SKX_A0 (0x4025315C)                                                  */
/*       SKX (0x4025315C)                                                     */
/* Register default value:              0x00000000                            */
#define RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_REG 0x0B01415C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Logs info on retried reads.  The contents of this register are valid with 
 * RETRY_RD_ERR_LOG. 
 *         The values in this register indicate the address of failed device. 
 * failed device id, chip select, cbit, bank    
 *       
 */
typedef union {
  struct {
    UINT32 failed_dev : 6;
    /* failed_dev - Bits[5:0], RWS_V, default = 6'h0 
       Holds the failed device number: for sddc/ddrt/x8 the device number is between 
       0-17. for adddc device number between 0-16 and 18-35  dev 17 is the spare device 
        
     */
    UINT32 chip_select : 3;
    /* chip_select - Bits[8:6], RWS_V, default = 3'h0 
       Rank CS0-CS7 (encoded chip select) of retried read
     */
    UINT32 cbit : 3;
    /* cbit - Bits[11:9], RWS_V, default = 3'h0 
        C0-C2 (encoded subrank) for last retry. 
     */
    UINT32 bank : 4;
    /* bank - Bits[15:12], RWS_V, default = 4'h0 
        Bank ID for last retry.  Bottom two bits are Bank Group, top two bits are Bank 
       Address 
     */
    UINT32 col : 12;
    /* col - Bits[27:16], RWS_V, default = 12'h0 
       Column address for the last retry.to get the real column address from this field 
       remove bit12(burst chop bit) and bit10(close page bit) and then shift by 2 
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_STRUCT;




/* ALERTSIGNAL_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253164)                                                  */
/*       SKX (0x10253164)                                                     */
/* Register default value:              0x00                                  */
#define ALERTSIGNAL_MCDDC_DP_REG 0x0B011164
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * alert status
 */
typedef union {
  struct {
    UINT8 seen : 1;
    /* seen - Bits[0:0], RW1C, default = 1'b0 
       If 1, alert signal (DDR4 parity error) has been observed. This bit is intended 
       to be used as feedback during CMD/address training. A write 1 will clear this 
       field. 
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ALERTSIGNAL_MCDDC_DP_STRUCT;


/* CPGC_PDA_MCDDC_DP_REG supported on:                                        */
/*       SKX_A0 (0x40253168)                                                  */
/*       SKX (0x40253168)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PDA_MCDDC_DP_REG 0x0B014168
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Register for one Per Dram Addressable transaction 
 */
typedef union {
  struct {
    UINT32 device_select : 18;
    /* device_select - Bits[17:0], RW_LB, default = 18'b000000000000000000 
       This register will be driven on DQ lines during Per Dram Addressability Mode 
       Register Set 
       operations according to the following table. A 0 in a bit position will select 
       the 
       corresponding device for Per Dram addressability Transaction.
       for x8 devices:
       bit [0] will be driven on DQ[7:0]
       bit [1] will be driven on DQ[15:8]
       bit [2] will be driven on DQ[23:16]
       bit [3] will be driven on DQ[31:24]
       bit [4] will be driven on DQ[39:32]
       bit [5] will be driven on DQ[47:40]
       bit [6] will be driven on DQ[55:48]
       bit [7] will be driven on DQ[63:56]
       bit [8] will be driven on DQ[71:64]
       for x4 devices:
       bit [0] will be driven on DQ[3:0]
       bit [1] will be driven on DQ[7:4]
       bit [2] will be driven on DQ[11:8]
       bit [3] will be driven on DQ[15:12]
       bit [4] will be driven on DQ[19:16]
       bit [5] will be driven on DQ[23:20]
       bit [6] will be driven on DQ[27:24]
       bit [7] will be driven on DQ[31:28]
       bit [8] will be driven on DQ[35:32]
       bit [9] will be driven on DQ[39:36]
       bit [10] will be driven on DQ[43:40]
       bit [11] will be driven on DQ[47:44]
       bit [12] will be driven on DQ[51:48]
       bit [13] will be driven on DQ[55:52]
       bit [14] will be driven on DQ[59:56]
       bit [15] will be driven on DQ[63:60]
       bit [16] will be driven on DQ[67:64]
       bit [17] will be driven on DQ[71:68]
     */
    UINT32 rsvd_18 : 2;
    /* rsvd_18 - Bits[19:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank_select : 3;
    /* rank_select - Bits[22:20], RW_LB, default = 3'b000 
       This field is the logical rank select for PDA transaction and should match the 
       same field 
       in CPGC_PDA_SHADOW register and should be set to match Chip Select fields 
       programmed into 
       CADB for this transaction (see Rank Naming Convention in MAS for mapping of Chip 
       selects 
       to logical ranks)
     */
    UINT32 rsvd_23 : 9;
    /* rsvd_23 - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PDA_MCDDC_DP_STRUCT;








/* MR_READ_RESULT_DQ12TO0_MCDDC_DP_REG supported on:                          */
/*       SKX_A0 (0x40253178)                                                  */
/*       SKX (0x40253178)                                                     */
/* Register default value:              0x00000000                            */
#define MR_READ_RESULT_DQ12TO0_MCDDC_DP_REG 0x0B014178
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * saves information from last Mode Register read
 */
typedef union {
  struct {
    UINT32 dq : 32;
    /* dq - Bits[31:0], RO, default = 32'b00000000000000000000000000000000 
       For by 8 parts: device 0 result: MR_READ_RESULT_DQ12TO0[7:0]
       device 1 result: MR_READ_RESULT_DQ12TO0[23:16]
       For by 4 parts: device 0 result: MR_READ_RESULT_DQ12TO0[7:0]
       device 1 result: MR_READ_RESULT_DQ12TO0[15:8]
       device 2 result: MR_READ_RESULT_DQ12TO0[23:16]
       device 3 result: MR_READ_RESULT_DQ12TO0[31:24]
     */
  } Bits;
  UINT32 Data;
} MR_READ_RESULT_DQ12TO0_MCDDC_DP_STRUCT;


/* MR_READ_RESULT_DQ28TO16_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x4025317C)                                                  */
/*       SKX (0x4025317C)                                                     */
/* Register default value:              0x00000000                            */
#define MR_READ_RESULT_DQ28TO16_MCDDC_DP_REG 0x0B01417C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * saves information from last Mode Register read
 */
typedef union {
  struct {
    UINT32 dq : 32;
    /* dq - Bits[31:0], RO, default = 32'b00000000000000000000000000000000 
       For by 8 parts: device 2 result: MR_READ_RESULT_DQ28TO16[7:0]
       device 3 result: MR_READ_RESULT_DQ28TO16[23:16]
       For by 4 parts: device 4 result: MR_READ_RESULT_DQ28TO16[7:0]
       device 5 result: MR_READ_RESULT_DQ28TO16[15:8]
       device 6 result: MR_READ_RESULT_DQ28TO16[23:16]
       device 7 result: MR_READ_RESULT_DQ28TO16[31:24]
     */
  } Bits;
  UINT32 Data;
} MR_READ_RESULT_DQ28TO16_MCDDC_DP_STRUCT;


/* MR_READ_RESULT_DQ44TO32_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253180)                                                  */
/*       SKX (0x40253180)                                                     */
/* Register default value:              0x00000000                            */
#define MR_READ_RESULT_DQ44TO32_MCDDC_DP_REG 0x0B014180
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * saves information from last Mode Register read
 */
typedef union {
  struct {
    UINT32 dq : 32;
    /* dq - Bits[31:0], RO, default = 32'b00000000000000000000000000000000 
       For by 8 parts: device 4 result: MR_READ_RESULT_DQ44TO32[7:0]
       device 5 result: MR_READ_RESULT_DQ44TO32[23:16]
       For by 4 parts: device 8 result: MR_READ_RESULT_DQ44TO32[7:0]
       device 9 result: MR_READ_RESULT_DQ44TO32[15:8]
       device 10 result: MR_READ_RESULT_DQ44TO32[23:16]
       device 11 result: MR_READ_RESULT_DQ44TO32[31:24]
     */
  } Bits;
  UINT32 Data;
} MR_READ_RESULT_DQ44TO32_MCDDC_DP_STRUCT;


/* MR_READ_RESULT_DQ60TO48_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253184)                                                  */
/*       SKX (0x40253184)                                                     */
/* Register default value:              0x00000000                            */
#define MR_READ_RESULT_DQ60TO48_MCDDC_DP_REG 0x0B014184
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * saves information from last Mode Register read
 */
typedef union {
  struct {
    UINT32 dq : 32;
    /* dq - Bits[31:0], RO, default = 32'b00000000000000000000000000000000 
       For by 8 parts: device 6 result: MR_READ_RESULT_DQ60To48[7:0]
       device 7 result: MR_READ_RESULT_DQ60To48[23:16]
       For by 4 parts: device 12 result: MR_READ_RESULT_DQ60To48[7:0]
       device 13 result: MR_READ_RESULT_DQ60To48[15:8]
       device 14 result: MR_READ_RESULT_DQ60To48[23:16]
       device 15 result: MR_READ_RESULT_DQ60To48[31:24]
     */
  } Bits;
  UINT32 Data;
} MR_READ_RESULT_DQ60TO48_MCDDC_DP_STRUCT;


/* MR_READ_RESULT_DQ68TO64_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253188)                                                  */
/*       SKX (0x40253188)                                                     */
/* Register default value:              0x00000000                            */
#define MR_READ_RESULT_DQ68TO64_MCDDC_DP_REG 0x0B014188
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * saves information from last Mode Register read
 */
typedef union {
  struct {
    UINT32 dq : 16;
    /* dq - Bits[15:0], RO, default = 16'b0000000000000000 
       For by 8 parts: device 8 result: MR_READ_RESULT_DQ68TO64[7:0]
       For by 4 parts: device 16 result: MR_READ_RESULT_DQ68TO64[7:0]
       device 17 result: MR_READ_RESULT_DQ68TO64[15:8]
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MR_READ_RESULT_DQ68TO64_MCDDC_DP_STRUCT;


/* DDR4_VIRAL_CTL_MCDDC_DP_REG supported on:                                  */
/*       SKX_A0 (0x4025318C)                                                  */
/*       SKX (0x4025318C)                                                     */
/* Register default value on SKX_A0:    0x00001FFF                            */
/* Register default value on SKX:       0x00000000                            */
#define DDR4_VIRAL_CTL_MCDDC_DP_REG 0x0B01418C
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * Bits to control viral signalling for DDR4 Errors
 */
typedef union {
  struct {
    UINT32 dis_ha_wr_data_par_err : 1;
    /* dis_ha_wr_data_par_err - Bits[0:0], RWS, default = 1'b1 
       Disable viral triggering for M2Mem write data parity error for DDR4. Viral 
       signalling is Error signalling is disabled by default 
     */
    UINT32 dis_ddrt_ha_wr_data_par_err : 1;
    /* dis_ddrt_ha_wr_data_par_err - Bits[1:1], RWS, default = 1'b1 
       Disable viral triggering for M2Mem write data parity error for DDRT. Viral 
       signalling is Error signalling is disabled by default 
     */
    UINT32 dis_ha_wr_be_par_err : 1;
    /* dis_ha_wr_be_par_err - Bits[2:2], RWS, default = 1'b1 
       Disable viral triggering for M2Mem write data byte enable parity error for DDR4. 
       Viral signalling is Error signalling is disabled by default 
     */
    UINT32 dis_ddrt_ha_wr_be_par_err : 1;
    /* dis_ddrt_ha_wr_be_par_err - Bits[3:3], RWS, default = 1'b1 
       Disable viral triggering for M2Mem write data byte enable parity error for DDRT. 
       Viral signalling is Error signalling is disabled by default 
     */
    UINT32 dis_wdb_par_err : 1;
    /* dis_wdb_par_err - Bits[4:4], RWS, default = 1'b1 
       Disable viral triggering for DDR4 WDB parity error. Viral signalling is Error 
       signalling is disabled by default 
     */
    UINT32 dis_ddrt_wdb_par_err : 1;
    /* dis_ddrt_wdb_par_err - Bits[5:5], RWS, default = 1'b1 
       Disable viral triggering for DDRT WDB parity error. Viral signalling is Error 
       signalling is disabled by default 
     */
    UINT32 dis_rpq0_rpa_par_err : 1;
    /* dis_rpq0_rpa_par_err - Bits[6:6], RWS, default = 1'b1 
       Disable viral triggering for RPQ0 rpa parity error. Viral signalling is Error 
       signalling is disabled by default 
     */
    UINT32 dis_rpq1_rpa_par_err : 1;
    /* dis_rpq1_rpa_par_err - Bits[7:7], RWS, default = 1'b1 
       Disable viral triggering for RPQ1 rpa parity error. Viral signalling is Error 
       signalling is disabled by default 
     */
    UINT32 dis_wpq_wpa_par_err : 1;
    /* dis_wpq_wpa_par_err - Bits[8:8], RWS, default = 1'b1 
       Disable viral triggering for WPQ wpa parity error. Viral signalling is Error 
       signalling is disabled by default 
     */
    UINT32 dis_uncorr_pat_err : 1;
    /* dis_uncorr_pat_err - Bits[9:9], RWS, default = 1'b1 
       Disable viral triggering for uncorrectable patrol scurb error. Viral signalling 
       is Error signalling is disabled by default 
     */
    UINT32 dis_uncorr_spr_err : 1;
    /* dis_uncorr_spr_err - Bits[10:10], RWS, default = 1'b1 
       Disable viral triggering for uncorrectable sparing error. Viral signalling is 
       Error signalling is disabled by default 
     */
    UINT32 dis_appp_err : 1;
    /* dis_appp_err - Bits[11:11], RWS, default = 1'b1 
       Disable viral triggering for APPP error. Viral signalling is Error signalling is 
       disabled by default 
     */
    UINT32 dis_fv_viral_bit : 1;
    /* dis_fv_viral_bit - Bits[12:12], RWS, default = 1'b1 
       Disable viral triggering for viral bit send from FV. Viral signalling is Error 
       signalling is disabled by default 
     */
    UINT32 rsvd : 19;
    /* rsvd - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDR4_VIRAL_CTL_MCDDC_DP_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Bits to control viral signalling for DDR4 Errors
 */
typedef union {
  struct {
    UINT32 dis_ha_wr_data_par_err : 1;
    /* dis_ha_wr_data_par_err - Bits[0:0], RWS, default = 1'b0 
       Disable viral triggering for M2Mem write data parity error for DDR4.
     */
    UINT32 dis_ddrt_ha_wr_data_par_err : 1;
    /* dis_ddrt_ha_wr_data_par_err - Bits[1:1], RWS, default = 1'b0 
       Disable viral triggering for M2Mem write data parity error for DDRT.
     */
    UINT32 dis_ha_wr_be_par_err : 1;
    /* dis_ha_wr_be_par_err - Bits[2:2], RWS, default = 1'b0 
       Disable viral triggering for M2Mem write data byte enable parity error for DDR4.
     */
    UINT32 dis_ddrt_ha_wr_be_par_err : 1;
    /* dis_ddrt_ha_wr_be_par_err - Bits[3:3], RWS, default = 1'b0 
       Disable viral triggering for M2Mem write data byte enable parity error for DDRT.
     */
    UINT32 dis_wdb_par_err : 1;
    /* dis_wdb_par_err - Bits[4:4], RWS, default = 1'b0 
       Disable viral triggering for DDR4 WDB parity error.
     */
    UINT32 dis_ddrt_wdb_par_err : 1;
    /* dis_ddrt_wdb_par_err - Bits[5:5], RWS, default = 1'b0 
       Disable viral triggering for DDRT WDB parity error.
     */
    UINT32 dis_rpq0_rpa_par_err : 1;
    /* dis_rpq0_rpa_par_err - Bits[6:6], RWS, default = 1'b0 
       Disable viral triggering for RPQ0 rpa parity error.
     */
    UINT32 dis_rpq1_rpa_par_err : 1;
    /* dis_rpq1_rpa_par_err - Bits[7:7], RWS, default = 1'b0 
       Disable viral triggering for RPQ1 rpa parity error.
     */
    UINT32 dis_wpq_wpa_par_err : 1;
    /* dis_wpq_wpa_par_err - Bits[8:8], RWS, default = 1'b0 
       Disable viral triggering for WPQ wpa parity error.
     */
    UINT32 dis_uncorr_pat_err : 1;
    /* dis_uncorr_pat_err - Bits[9:9], RWS, default = 1'b0 
       Disable viral triggering for uncorrectable patrol scurb error.
     */
    UINT32 dis_uncorr_spr_err : 1;
    /* dis_uncorr_spr_err - Bits[10:10], RWS, default = 1'b0 
       Disable viral triggering for uncorrectable sparing error.
     */
    UINT32 dis_appp_err : 1;
    /* dis_appp_err - Bits[11:11], RWS, default = 1'b0 
       Disable viral triggering for APPP error.
     */
    UINT32 dis_fv_viral_bit : 1;
    /* dis_fv_viral_bit - Bits[12:12], RWS, default = 1'b0 
       Disable viral triggering for viral bit send from FV.
     */
    UINT32 dis_viral_link_fail : 1;
    /* dis_viral_link_fail - Bits[13:13], RWS, default = 1'b0 
       Disable viral triggering when Link fail condition occurs.
               
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDR4_VIRAL_CTL_MCDDC_DP_STRUCT;



/* WRROUNDTRIP0_MCDDC_DP_REG supported on:                                    */
/*       SKX_A0 (0x40253190)                                                  */
/*       SKX (0x40253190)                                                     */
/* Register default value:              0x1D1D1D1D                            */
#define WRROUNDTRIP0_MCDDC_DP_REG 0x0B014190
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x080
 */
typedef union {
  struct {
    UINT32 rt_rank0 : 8;
    /* rt_rank0 - Bits[7:0], RW, default = 8'b00011101 
       Rank 0 round trip latency in QCLK, 88 max configurable value
     */
    UINT32 rt_rank1 : 8;
    /* rt_rank1 - Bits[15:8], RW, default = 8'b00011101 
       Rank 1 round trip latency in QCLK
     */
    UINT32 rt_rank2 : 8;
    /* rt_rank2 - Bits[23:16], RW, default = 8'b00011101 
       Rank 2 round trip latency in QCLK
     */
    UINT32 rt_rank3 : 8;
    /* rt_rank3 - Bits[31:24], RW, default = 8'b00011101 
       Rank 3 round trip latency in QCLK
     */
  } Bits;
  UINT32 Data;
} WRROUNDTRIP0_MCDDC_DP_STRUCT;


/* WRROUNDTRIP1_MCDDC_DP_REG supported on:                                    */
/*       SKX_A0 (0x40253194)                                                  */
/*       SKX (0x40253194)                                                     */
/* Register default value:              0x1D1D1D1D                            */
#define WRROUNDTRIP1_MCDDC_DP_REG 0x0B014194
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x084
 */
typedef union {
  struct {
    UINT32 rt_rank4 : 8;
    /* rt_rank4 - Bits[7:0], RW, default = 8'b00011101 
       Rank 4 round trip latency in QCLK
     */
    UINT32 rt_rank5 : 8;
    /* rt_rank5 - Bits[15:8], RW, default = 8'b00011101 
       Rank 5 round trip latency in QCLK
     */
    UINT32 rt_rank6 : 8;
    /* rt_rank6 - Bits[23:16], RW, default = 8'b00011101 
       Rank 6 round trip latency in QCLK
     */
    UINT32 rt_rank7 : 8;
    /* rt_rank7 - Bits[31:24], RW, default = 8'b00011101 
       Rank 7 round trip latency in QCLK
     */
  } Bits;
  UINT32 Data;
} WRROUNDTRIP1_MCDDC_DP_STRUCT;


/* WRIOLATENCY_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x40253198)                                                  */
/*       SKX (0x40253198)                                                     */
/* Register default value:              0x00000000                            */
#define WRIOLATENCY_MCDDC_DP_REG 0x0B014198
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * generated by critter 20_2_0x08c
 */
typedef union {
  struct {
    UINT32 io_lat_rank0 : 4;
    /* io_lat_rank0 - Bits[3:0], RW, default = 4'b0000 
       Rank 0 IO latency in QCLK
     */
    UINT32 io_lat_rank1 : 4;
    /* io_lat_rank1 - Bits[7:4], RW, default = 4'b0000 
       Rank 1 IO latency in QCLK
     */
    UINT32 io_lat_rank2 : 4;
    /* io_lat_rank2 - Bits[11:8], RW, default = 4'b0000 
       Rank 2 IO latency in QCLK
     */
    UINT32 io_lat_rank3 : 4;
    /* io_lat_rank3 - Bits[15:12], RW, default = 4'b0000 
       Rank 3 IO latency in QCLK
     */
    UINT32 io_lat_rank4 : 4;
    /* io_lat_rank4 - Bits[19:16], RW, default = 4'b0000 
       Rank 4 IO latency in QCLK
     */
    UINT32 io_lat_rank5 : 4;
    /* io_lat_rank5 - Bits[23:20], RW, default = 4'b0000 
       Rank 5 IO latency in QCLK
     */
    UINT32 io_lat_rank6 : 4;
    /* io_lat_rank6 - Bits[27:24], RW, default = 4'b0000 
       Rank 6 IO latency in QCLK
     */
    UINT32 io_lat_rank7 : 4;
    /* io_lat_rank7 - Bits[31:28], RW, default = 4'b0000 
       Rank 7 IO latency in QCLK
     */
  } Bits;
  UINT32 Data;
} WRIOLATENCY_MCDDC_DP_STRUCT;




/* MCSCRAMBLECONFIG_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x402531E0)                                                  */
/*       SKX (0x402531E0)                                                     */
/* Register default value:              0x00000000                            */
#define MCSCRAMBLECONFIG_MCDDC_DP_REG 0x0B0141E0
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This register is used to scramble and unscramble the IMC to DDR Pad data using 
 * the DDR command address and the scramble seed. All the fields CH_ENABLE, 
 * TX_ENABLE and RX_ENABLE must be set to 1 to enable scrambling, and must be 
 * cleared to disable scrambling. 
 */
typedef union {
  struct {
    UINT32 tx_enable : 1;
    /* tx_enable - Bits[0:0], RWS_L, default = 1'b0 
       TX_ENABLE: DDR4 Tx data bus scrambling.
     */
    UINT32 rx_enable : 1;
    /* rx_enable - Bits[1:1], RWS_L, default = 1'b0 
       RX_ENABLE: DDR4 Rx data bus scrambling.
     */
    UINT32 ch_enable : 1;
    /* ch_enable - Bits[2:2], RWS_L, default = 1'b0 
       Channel enable
     */
    UINT32 seed_lock : 1;
    /* seed_lock - Bits[3:3], RWS_O, default = 1'b0 
       lock bit for the seed update.
       1b = lock
       0b = unlock
     */
    UINT32 rsvd_4 : 3;
    /* rsvd_4 - Bits[6:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tx_enable_ddrt : 1;
    /* tx_enable_ddrt - Bits[7:7], RWS_L, default = 1'b0 
       TX_ENABLE: DDR-T Tx data bus scrambling.
     */
    UINT32 rx_enable_ddrt : 1;
    /* rx_enable_ddrt - Bits[8:8], RWS_L, default = 1'b0 
       RX_ENABLE: DDR-T Rx data bus scrambling.
     */
    UINT32 ch_enable_ddrt : 1;
    /* ch_enable_ddrt - Bits[9:9], RWS_L, default = 1'b0 
       DDRT Channel enable
     */
    UINT32 rsvd_10 : 22;
    /* rsvd_10 - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCSCRAMBLECONFIG_MCDDC_DP_STRUCT;


/* MCSCRAMBLE_SEED_SEL_MCDDC_DP_REG supported on:                             */
/*       SKX_A0 (0x402531E4)                                                  */
/*       SKX (0x402531E4)                                                     */
/* Register default value:              0x00000000                            */
#define MCSCRAMBLE_SEED_SEL_MCDDC_DP_REG 0x0B0141E4
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This register is locked by SEED_LOCK bit in MCSCRAMBLECONFIG register.
 */
typedef union {
  struct {
    UINT32 scrb_lower_seed_sel : 16;
    /* scrb_lower_seed_sel - Bits[15:0], RWS_L, default = 16'b0000000000000000 
       Reordering the lower srambling seed select control.
     */
    UINT32 scrb_upper_seed_sel : 16;
    /* scrb_upper_seed_sel - Bits[31:16], RWS_L, default = 16'b0000000000000000 
       Reordering the upper srambling seed select control.
     */
  } Bits;
  UINT32 Data;
} MCSCRAMBLE_SEED_SEL_MCDDC_DP_STRUCT;


/* MCSCRAMBLE_SEED_SEL_DDRT_MCDDC_DP_REG supported on:                        */
/*       SKX_A0 (0x402531E8)                                                  */
/*       SKX (0x402531E8)                                                     */
/* Register default value:              0x00000000                            */
#define MCSCRAMBLE_SEED_SEL_DDRT_MCDDC_DP_REG 0x0B0141E8
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This register is locked by SEED_LOCK bit in MCSCRAMBLECONFIG register.
 */
typedef union {
  struct {
    UINT32 scrb_lower_seed_sel_ddrt : 16;
    /* scrb_lower_seed_sel_ddrt - Bits[15:0], RWS_L, default = 16'b0000000000000000 
       Reordering the lower srambling seed select control.
     */
    UINT32 scrb_upper_seed_sel_ddrt : 16;
    /* scrb_upper_seed_sel_ddrt - Bits[31:16], RWS_L, default = 16'b0000000000000000 
       Reordering the upper srambling seed select control.
     */
  } Bits;
  UINT32 Data;
} MCSCRAMBLE_SEED_SEL_DDRT_MCDDC_DP_STRUCT;


/* MCSCRAMBLE_RX_SEED_DDRT_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x402531EC)                                                  */
/*       SKX (0x402531EC)                                                     */
/* Register default value:              0x0000EE37                            */
#define MCSCRAMBLE_RX_SEED_DDRT_MCDDC_DP_REG 0x0B0141EC
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDRT Rx Data Scrambler Initial Seed. This register is locked by SEED_LOCK bit in 
 * MCSCRAMBLECONFIG register. 
 */
typedef union {
  struct {
    UINT32 seed : 16;
    /* seed - Bits[15:0], RWS_L, default = 16'hee37 
       
                DDRT Rx descramble seed. This seed is loaded at reset
                de-assertion and is the default seed that MC uses to unscramble DDRT
                read data. 
                If a different seed needs to be loaded, then it needs to be done under 
       quiesce.  
                CSRs in FNV and SKX should be loaded with the new seed and DDRT Error 
       FSM should  
                be triggered using CSR bit DDRT_RETRY_CTL.trigger_retry
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCSCRAMBLE_RX_SEED_DDRT_MCDDC_DP_STRUCT;




/* RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_MCDDC_DP_REG supported on:               */
/*       SKX_A0 (0x40253200)                                                  */
/*       SKX (0x40253200)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_MCDDC_DP_REG 0x0B014200
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Error Injection Response Function on Address Match Write Data Error Injection. 
 * Associating registers: RSP_FUNC_ADDR_MATCH_LO&HI, RSP_FUNC_ADDR_MATCH_LO&HI, 
 * RSP_FUNC_CRC_ERR_INJ_EXTRA.CRC_ERR_INJ_DEV0_5_BITS and CRC_ERR_INJ_DEV1_5_BITS 
 *       
 */
typedef union {
  struct {
    UINT32 dev0_xor_msk : 32;
    /* dev0_xor_msk - Bits[31:0], RW_L, default = 32'b00000000000000000000000000000000 
       device 0 data inversion mask for error injection. Eight 4-bit values specify 
       which bits of the nibble are inverted on each data cycle of a BL8 write. Bits 
       3:0 correspond to the first data cycle. 
       In addition to LT lock, this register is locked by EPMCMAIN_DFX_LCK_CNTL.RSPLCK 
       (uCR) AND MC_ERR_INJ_LCK.MC_ERR_INJ_LCK (MSR). 
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_MCDDC_DP_STRUCT;


/* RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_MCDDC_DP_REG supported on:               */
/*       SKX_A0 (0x40253204)                                                  */
/*       SKX (0x40253204)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_MCDDC_DP_REG 0x0B014204
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Error Injection Response Function on Address Match Write Data Error Injection. 
 * Associating registers: RSP_FUNC_ADDR_MATCH_LO&HI, RSP_FUNC_ADDR_MATCH_LO&HI, 
 * RSP_FUNC_CRC_ERR_INJ_EXTRA.CRC_ERR_INJ_DEV0_5_BITS and CRC_ERR_INJ_DEV1_5_BITS 
 * In addition to LT lock, this register is locked by EPMCMAIN_DFX_LCK_CNTL.RSPLCK 
 * (uCR) AND MC_ERR_INJ_LCK.MC_ERR_INJ_LCK (MSR). 
 */
typedef union {
  struct {
    UINT32 dev1_xor_msk : 32;
    /* dev1_xor_msk - Bits[31:0], RW_L, default = 32'b00000000000000000000000000000000 
       device 1 data inversion mask for error injection. Eight 4-bit values specify 
       which bits of the nibble are inverted on each data cycle of a BL8 write. Bits 
       3:0 correspond to the first data cycle. 
       In addition to LT lock, this register is locked by EPMCMAIN_DFX_LCK_CNTL.RSPLCK 
       (uCR) AND MC_ERR_INJ_LCK.MC_ERR_INJ_LCK (MSR). 
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_MCDDC_DP_STRUCT;


/* RSP_FUNC_CRC_ERR_INJ_EXTRA_MCDDC_DP_REG supported on:                      */
/*       SKX_A0 (0x40253208)                                                  */
/*       SKX (0x40253208)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_CRC_ERR_INJ_EXTRA_MCDDC_DP_REG 0x0B014208
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Error Injection Response Function
 *       
 */
typedef union {
  struct {
    UINT32 crc_err_inj_dev0_5_bits : 5;
    /* crc_err_inj_dev0_5_bits - Bits[4:0], RW_L, default = 5'b00000 
       Error Injection Response Function on Address Match Write Data Error Injection. 
       Associating registers: RSP_FUNC_ADDR_MATCH_LO&HI, RSP_FUNC_ADDR_MATCH_LO&HI, 
       RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK and RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK 
       Selects nibble of data bus for error injection. 0x0 selects DQ[3:0], 0x1 selects 
       DQ[7:4], 0x17 selects ECC[7:4] etc... 
       0x18 - 0x31 are reserved.
               
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 crc_err_inj_dev1_5_bits : 5;
    /* crc_err_inj_dev1_5_bits - Bits[12:8], RW_L, default = 5'b00000 
       Error Injection Response Function on Address Match Write Data Error Injection. 
       Associating registers: RSP_FUNC_ADDR_MATCH_LO&HI, RSP_FUNC_ADDR_MATCH_LO&HI, 
       RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK and RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK 
       Selects nibble of data bus for device 1 error injection. 0x0 selects DQ[3:0], 
       0x1 selects DQ[7:4], 0x17 selects ECC[7:4] etc... 
       0x18 - 0x31 are reserved.
       In addition to LT lock, this register is locked by EPMCMAIN_DFX_LCK_CNTL.RSPLCK 
       (uCR) AND MC_ERR_INJ_LCK.MC_ERR_INJ_LCK (MSR). 
     */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_16 : 2;
    UINT32 rsvd_18 : 6;
    /* rsvd_18 - Bits[23:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_24 : 2;
    UINT32 rsvd_26 : 6;
    /* rsvd_26 - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_CRC_ERR_INJ_EXTRA_MCDDC_DP_STRUCT;


/* RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_DDRT_MCDDC_DP_REG supported on:          */
/*       SKX_A0 (0x4025320C)                                                  */
/*       SKX (0x4025320C)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_DDRT_MCDDC_DP_REG 0x0B01420C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Error Injection Response Function on Address Match Write Data Error Injection. 
 * Associating registers: RSP_FUNC_ADDR_MATCH_LO&HI, RSP_FUNC_ADDR_MATCH_LO&HI, 
 * RSP_FUNC_CRC_ERR_INJ_EXTRA.CRC_ERR_INJ_DEV0_5_BITS and CRC_ERR_INJ_DEV1_5_BITS 
 *       
 */
typedef union {
  struct {
    UINT32 dev0_xor_msk : 32;
    /* dev0_xor_msk - Bits[31:0], RW_L, default = 32'b00000000000000000000000000000000 
       device 0 data inversion mask for error injection. Eight 4-bit values specify 
       which bits of the nibble are inverted on each data cycle of a BL8 write. Bits 
       3:0 correspond to the first data cycle. 
               
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_DDRT_MCDDC_DP_STRUCT;


/* RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_DDRT_MCDDC_DP_REG supported on:          */
/*       SKX_A0 (0x40253210)                                                  */
/*       SKX (0x40253210)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_DDRT_MCDDC_DP_REG 0x0B014210
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Error Injection Response Function on Address Match Write Data Error Injection. 
 * Associating registers: RSP_FUNC_ADDR_MATCH_LO&HI, RSP_FUNC_ADDR_MATCH_LO&HI, 
 * RSP_FUNC_CRC_ERR_INJ_EXTRA.CRC_ERR_INJ_DEV0_5_BITS and CRC_ERR_INJ_DEV1_5_BITS 
 *       
 */
typedef union {
  struct {
    UINT32 dev1_xor_msk : 32;
    /* dev1_xor_msk - Bits[31:0], RW_L, default = 32'b00000000000000000000000000000000 
       device 1 data inversion mask for error injection. Eight 4-bit values specify 
       which bits of the nibble are inverted on each data cycle of a BL8 write. Bits 
       3:0 correspond to the first data cycle. 
               
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_DDRT_MCDDC_DP_STRUCT;


/* RSP_FUNC_CRC_ERR_INJ_EXTRA_DDRT_MCDDC_DP_REG supported on:                 */
/*       SKX_A0 (0x40253214)                                                  */
/*       SKX (0x40253214)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_CRC_ERR_INJ_EXTRA_DDRT_MCDDC_DP_REG 0x0B014214
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Error Injection Response Function 
 */
typedef union {
  struct {
    UINT32 crc_err_inj_dev0_5_bits : 5;
    /* crc_err_inj_dev0_5_bits - Bits[4:0], RW_L, default = 5'b00000 
       Error Injection Response Function on Address Match Write Data Error Injection. 
       Associating registers: RSP_FUNC_ADDR_MATCH_LO&HI, RSP_FUNC_ADDR_MATCH_LO&HI, 
       RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK and RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK 
       Selects nibble of data bus for error injection. 0x0 selects DQ[3:0], 0x1 selects 
       DQ[7:4], 0x17 selects ECC[7:4] etc... 
       0x18 - 0x31 are reserved.
               
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 crc_err_inj_dev1_5_bits : 5;
    /* crc_err_inj_dev1_5_bits - Bits[12:8], RW_L, default = 5'b00000 
       Error Injection Response Function on Address Match Write Data Error Injection. 
       Associating registers: RSP_FUNC_ADDR_MATCH_LO&HI, RSP_FUNC_ADDR_MATCH_LO&HI, 
       RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK and RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK 
       Selects nibble of data bus for device 1 error injection. 0x0 selects DQ[3:0], 
       0x1 selects DQ[7:4], 0x17 selects ECC[7:4] etc... 
       0x18 - 0x31 are reserved.
               
     */
    UINT32 rsvd_13 : 19;
    /* rsvd_13 - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_CRC_ERR_INJ_EXTRA_DDRT_MCDDC_DP_STRUCT;


/* WDB_INIT_PSN_CTL_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x10253218)                                                  */
/*       SKX (0x10253218)                                                     */
/* Register default value:              0x00                                  */
#define WDB_INIT_PSN_CTL_MCDDC_DP_REG 0x0B011218
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * WDB init poison control bit
 */
typedef union {
  struct {
    UINT8 poisonall : 1;
    /* poisonall - Bits[0:0], RW_LB, default = 1'b0 
       Setting this bit will poison all writes coming from WDB for this channel. This 
       bit is meant to be used during memory initialization for memory space that is 
       set aside for memory migration. 
     */
    UINT8 poisonall_ddrt : 1;
    /* poisonall_ddrt - Bits[1:1], RW_LB, default = 1'b0 
       Setting this bit will poison all DDRT writes coming from WDB for this channel. 
       This bit is meant to be used during memory initialization for memory space that 
       is set aside for memory migration. 
     */
    UINT8 rsvd : 6;
    /* rsvd - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} WDB_INIT_PSN_CTL_MCDDC_DP_STRUCT;


/* WDB_DDRT_WR_CTL_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x1025321C)                                                  */
/*       SKX (0x1025321C)                                                     */
/* Register default value:              0x01                                  */
#define WDB_DDRT_WR_CTL_MCDDC_DP_REG 0x0B01121C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * WDB DDRT write delay control
 */
typedef union {
  struct {
    UINT8 sel : 5;
    /* sel - Bits[4:0], RW_LB, default = 5'b1 
       Delay selection for DDRT write in WDB. A value above 25 is not valid.
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} WDB_DDRT_WR_CTL_MCDDC_DP_STRUCT;








/* DDRT_DATAPATH_DELAY_MCDDC_DP_REG supported on:                             */
/*       SKX_A0 (0x40253248)                                                  */
/*       SKX (0x40253248)                                                     */
/* Register default value:              0x02431010                            */
#define DDRT_DATAPATH_DELAY_MCDDC_DP_REG 0x0B014248
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Delay for read and write datapath to switch from 
 *       DDRT mode to DDR4 mode after DDRT command is scheduled 
 *       
 */
typedef union {
  struct {
    UINT32 wr : 7;
    /* wr - Bits[6:0], RW_LB, default = 7'd16 
       
               CSR ddrt_datapath_delay2.en_wr_ddrt_mode controls delay to
               switch the write data path
               to DDRT mode after a DDRT Write is scheduled. After the write data
               path switches to DDRT mode, CSR ddrt_datapath_delay.wr controls
               how long should the Write data path be held in DDRT mode before
               it can switch back to DDR4 mode. 
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rd : 7;
    /* rd - Bits[14:8], RW_LB, default = 7'd16 
       
               CSR ddrt_datapath_delay2.en_rd_ddrt_mode controls delay to
               switch the read data path
               to DDRT mode after a GNT is scheduled. After the read data
               path switches to DDRT mode, CSR ddrt_datapath_delay.rd controls
               how long should the Read data path be held in DDRT mode before
               it can switch back to DDR4 mode. 
               
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 erid_to_rdvalid_delay : 5;
    /* erid_to_rdvalid_delay - Bits[20:16], RW_LB, default = 5'h3 
       Delay between ERID and RdValid in DDRT mode; the programmed value should be real 
       delay value minus 2 
     */
    UINT32 failed_nibble_number : 5;
    /* failed_nibble_number - Bits[25:21], RW_LB, default = 5'h12 
       for ddrt hard failure BIOS program the failed nibble number(device number)
     */
    UINT32 ddrt_force_correction : 1;
    /* ddrt_force_correction - Bits[26:26], RW_LB, default = 1'h0 
       for ddrt hard failure force to take the correction path all the time
     */
    UINT32 force_dp_ddrt_mode : 1;
    /* force_dp_ddrt_mode - Bits[27:27], RW_LB, default = 1'b0 
       
               Force Read and Write data path to be in DDRT mode whenever the
               scheduler switches to DDRT mode. The default behavior for DDRT
               is to switch the data path to DDRT mode just-in-time for a
               DDRT command that uses the datapath. If this bit is set, then
               the timing parameters that control DDRT-DDR4 major mode also
               need to adjusted. 
               
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW_LB, default = 4'h0 
       
               Spare
               
     */
  } Bits;
  UINT32 Data;
} DDRT_DATAPATH_DELAY_MCDDC_DP_STRUCT;


/* DDRT_DATAPATH_DELAY2_MCDDC_DP_REG supported on:                            */
/*       SKX_A0 (0x4025324C)                                                  */
/*       SKX (0x4025324C)                                                     */
/* Register default value:              0x05408844                            */
#define DDRT_DATAPATH_DELAY2_MCDDC_DP_REG 0x0B01424C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Delay for DDRT
 *       
 */
typedef union {
  struct {
    UINT32 erid_to_retry_delay : 5;
    /* erid_to_retry_delay - Bits[4:0], RW_LB, default = 5'h4 
       ERID to retry delay
     */
    UINT32 erid_to_earlyretry_delay : 5;
    /* erid_to_earlyretry_delay - Bits[9:5], RW_LB, default = 5'h2 
       ERID to earlier version of retry delay
     */
    UINT32 en_rd_ddrt_mode : 5;
    /* en_rd_ddrt_mode - Bits[14:10], RW_LB, default = 5'h2 
       
               CSR ddrt_datapath_delay2.en_rd_ddrt_mode controls delay to
               switch the read data path
               to DDRT mode after a GNT is scheduled. After the read data
               path switches to DDRT mode, CSR ddrt_datapath_delay.rd controls
               how long should the Read data path be held in DDRT mode before
               it can switch back to DDR4 mode. 
               This register should be programmed to 
               (((DDRT RoundTrip - 1)/2) - t_ddrt_gnt2erid - 5) 
               
     */
    UINT32 en_wr_ddrt_mode : 4;
    /* en_wr_ddrt_mode - Bits[18:15], RW_LB, default = 4'h1 
       
               CSR ddrt_datapath_delay2.en_wr_ddrt_mode controls delay to
               switch the write data path
               to DDRT mode after a DDRT Write is scheduled. After the write data
               path switches to DDRT mode, CSR ddrt_datapath_delay.wr controls
               how long should the Write data path be held in DDRT mode before
               it can switch back to DDR4 mode. 
               
     */
    UINT32 retry_delay : 4;
    /* retry_delay - Bits[22:19], RW_LB, default = 4'd8 
       
               This is the additional delay for which the read datapath must
               remain in DDRT mode in case the transaction is going through
               the ECC correction path
               
     */
    UINT32 rrd_gnt2erid : 7;
    /* rrd_gnt2erid - Bits[29:23], RW_LB, default = 7'd10 
       
               Delay from GNT to ERID. This should be programmed to 
               (t_ddrt_gnt2erid + 1)
               
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_DATAPATH_DELAY2_MCDDC_DP_STRUCT;


/* DDRT_FNV_INTR_CTL_MCDDC_DP_REG supported on:                               */
/*       SKX_A0 (0x40253250)                                                  */
/*       SKX (0x40253250)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_FNV_INTR_CTL_MCDDC_DP_REG 0x0B014250
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Delay for DDRT
 *       
 */
typedef union {
  struct {
    UINT32 slot0_lo_smi_en : 1;
    /* slot0_lo_smi_en - Bits[0:0], RW_LB, default = 1'b0 
       Enable SMI signalling for DDRT Lo priority interrupt from Slot 0
     */
    UINT32 slot1_lo_smi_en : 1;
    /* slot1_lo_smi_en - Bits[1:1], RW_LB, default = 1'b0 
       Enable SMI signalling for DDRT Lo priority interrupt from Slot 0
     */
    UINT32 slot0_hi_smi_en : 1;
    /* slot0_hi_smi_en - Bits[2:2], RW_LB, default = 1'b0 
       Enable SMI signalling for DDRT Lo priority interrupt from Slot 0
     */
    UINT32 slot1_hi_smi_en : 1;
    /* slot1_hi_smi_en - Bits[3:3], RW_LB, default = 1'b0 
       Enable SMI signalling for DDRT Lo priority interrupt from Slot 0
     */
    UINT32 slot0_lo_err0_en : 1;
    /* slot0_lo_err0_en - Bits[4:4], RW_LB, default = 1'b0 
       Enable ERR0 signalling for DDRT Lo priority interrupt from Slot 0
     */
    UINT32 slot1_lo_err0_en : 1;
    /* slot1_lo_err0_en - Bits[5:5], RW_LB, default = 1'b0 
       Enable ERR0 signalling for DDRT Lo priority interrupt from Slot 0
     */
    UINT32 slot0_hi_err0_en : 1;
    /* slot0_hi_err0_en - Bits[6:6], RW_LB, default = 1'b0 
       Enable ERR0 signalling for DDRT Lo priority interrupt from Slot 0
     */
    UINT32 slot1_hi_err0_en : 1;
    /* slot1_hi_err0_en - Bits[7:7], RW_LB, default = 1'b0 
       Enable ERR0 signalling for DDRT Lo priority interrupt from Slot 0
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_FNV_INTR_CTL_MCDDC_DP_STRUCT;






/* X4MODESEL_MCDDC_DP_REG supported on:                                       */
/*       SKX_A0 (0x40253268)                                                  */
/*       SKX (0x40253268)                                                     */
/* Register default value:              0x00000000                            */
#define X4MODESEL_MCDDC_DP_REG 0x0B014268
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * x4ModelSel - MCDP x4 Mode Select
 */
typedef union {
  struct {
    UINT32 dimm0_mode : 1;
    /* dimm0_mode - Bits[0:0], RW, default = 1'b0 
       Controls the DDRIO x4 (if set) / x8 (if cleared) DIMM0 DQS select.
     */
    UINT32 dimm1_mode : 1;
    /* dimm1_mode - Bits[1:1], RW, default = 1'b0 
       Controls the DDRIO x4 (if set) / x8 (if cleared) DIMM1 DQS select.
     */
    UINT32 dimm2_mode : 1;
    /* dimm2_mode - Bits[2:2], RW, default = 1'b0 
       Controls the DDRIO x4 (if set) / x8 (if cleared) DIMM2 DQS select.
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} X4MODESEL_MCDDC_DP_STRUCT;




/* VMSE_ERROR_MCDDC_DP_REG supported on:                                      */
/*       SKX_A0 (0x40253308)                                                  */
/* Register default value:              0x00000020                            */
#define VMSE_ERROR_MCDDC_DP_REG 0x0B014308
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * This register holds the status and configuration of the
 * link_ERR# signal.
 */
typedef union {
  struct {
    UINT32 vmse_err_latency : 7;
    /* vmse_err_latency - Bits[6:0], RWS_L, default = 7'b0100000 
       The maximum delay (in DCLKs) between a command or data with an error and the 
       observed assertion of SMI_ERR#. Without WrCRC enabled, the value should be 
       CA-parity-latency = DDRIO-pipeline-forward + Forward-board-delay + tPAR_alert + 
       return-board-delay + DDRIO-pipeline-return. With WrCRC enabled, the value should 
       be CRC-alert-latency = DDRIO-pipeline-forward + Forward-board-delay + tCWL + 
       5(data burst length) + tCRC_alert + return-board-delay + DDRIO-pipeline-return. 
       The maximum value of the CSR field should be less than 17 + tCWL + tWTR + 
       DDRIO-pipeline-forward + Forward-board-delay + tCL + return-board-delay + 
       DDRIO-pipeline-return or 17 + tCWL + tWTR + round trip latency. 
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VMSE_ERROR_MCDDC_DP_STRUCT;






/* CORRECTION_DEBUG_CORR_DATA_CNTL_MCDDC_DP_REG supported on:                 */
/*       SKX_A0 (0x40253318)                                                  */
/*       SKX (0x40253318)                                                     */
/* Register default value:              0x00000040                            */
#define CORRECTION_DEBUG_CORR_DATA_CNTL_MCDDC_DP_REG 0x0B014318
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * this register hold extra correction debug. it hold the vecotr for failed device 
 * number. 
 * 		    it should be a one-hot vector for the device number.
 * 		    it is a DUE case if more than one device is set. 
 *                     the contents of this register are valid with 
 * RETRY_RD_ERR_LOG 
 *                     this register is locked with dfx lock
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 6;
    UINT32 write_en : 1;
    /* write_en - Bits[6:6], RWS_V, default = 1'b1 
       write enable for the data register(correction_debug_corr_data)
                            for the default value (1) HW will write to this register 
       then HW will set it to 0 when done writing to it 
                            Software needs to set this bit when it is done reading from 
       it 
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CORRECTION_DEBUG_CORR_DATA_CNTL_MCDDC_DP_STRUCT;




/* VMSE_CFG_READ_1_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253324)                                                  */
/* Register default value:              0x00000000                            */
#define VMSE_CFG_READ_1_MCDDC_DP_REG 0x0B014324
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * Register does not hold any config read data. Instead it is used for various 
 * defeature bits for MC bug fixes. 
 */
typedef union {
  struct {
    UINT32 read_data : 32;
    /* read_data - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Bits used to enable bug fixes.
               
     */
  } Bits;
  UINT32 Data;
} VMSE_CFG_READ_1_MCDDC_DP_STRUCT;


/* DDR4_CA_CTL_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x402533FC)                                                  */
/*       SKX (0x402533FC)                                                     */
/* Register default value:              0x00000000                            */
#define DDR4_CA_CTL_MCDDC_DP_REG 0x0B0143FC
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * This register holds theControl bits for DDR4 C/A parity error flow logic
 */
typedef union {
  struct {
    UINT32 erf_en0 : 1;
    /* erf_en0 - Bits[0:0], RW_LB, default = 1'b0 
       This should be set to 1 to enable error flow
     */
    UINT32 erf_regpart0 : 1;
    /* erf_regpart0 - Bits[1:1], RW_LB, default = 1'b0 
       This should be set to 1 if RDIMM/LRDIMMs are populated in the channel
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tpar_recov_ch0 : 8;
    /* tpar_recov_ch0 - Bits[11:4], RW_LB, default = 8'b00000000 
       largest ( tPAR_ALERT_ON + tPAR_ALERT_PW ) value of a rank populated in the 
       channel in DCLK. 
     */
    UINT32 rsvd_12 : 20;
    /* rsvd_12 - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDR4_CA_CTL_MCDDC_DP_STRUCT;


/* VMSE_RETRY_SB_ERR_COUNT_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253400)                                                  */
/* Register default value:              0x00000000                            */
#define VMSE_RETRY_SB_ERR_COUNT_MCDDC_DP_REG 0x0B014400
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * Count of south bound errors.   
 */
typedef union {
  struct {
    UINT32 transient_err_cnt : 16;
    /* transient_err_cnt - Bits[15:0], RO_V, default = 16'b0000000000000000 
       Count of transient south bound errors.
       Cleared by hardware when the smi_retry_sb_err_limit.sb_err_limit is reached and 
       persistent_err_cnt is incremented. 
       Cleared by hardware when the smi_retry_timer.sb_err_observation_time timer 
       expires. 
     */
    UINT32 persistent_err_cnt : 8;
    /* persistent_err_cnt - Bits[23:16], RO_V, default = 8'b00000000 
       Count of persistent south bound errors. 
       Counter saturates at 0xff.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VMSE_RETRY_SB_ERR_COUNT_MCDDC_DP_STRUCT;




/* VMSE_RETRY_ERR_LIMITS_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x4025340C)                                                  */
/* Register default value:              0x00100000                            */
#define VMSE_RETRY_ERR_LIMITS_MCDDC_DP_REG 0x0B01440C
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * Thresholds at which transient errors are considered persistent and trigger a 
 * link width change.   
 */
typedef union {
  struct {
    UINT32 rsvd : 16;
    /* rsvd - Bits[15:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sb_err_limit : 15;
    /* sb_err_limit - Bits[30:16], RWS_L, default = 15'b000000000010000 
       Number of transient errors (link_retry_sb_err_count.transient_err_cnt) allowed 
       before the SB error is declared persistent. 
     */
    UINT32 sb_err_enable : 1;
    /* sb_err_enable - Bits[31:31], RWS_L, default = 1'b0 
       Enable NB persistent condition.
     */
  } Bits;
  UINT32 Data;
} VMSE_RETRY_ERR_LIMITS_MCDDC_DP_STRUCT;






/* VMSE_ERR_FSM_STATE_MCDDC_DP_REG supported on:                              */
/*       SKX_A0 (0x40253420)                                                  */
/* Register default value:              0x00000000                            */
#define VMSE_ERR_FSM_STATE_MCDDC_DP_REG 0x0B014420
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * State of the Intel LINK 2 Error flow FSM.
 */
typedef union {
  struct {
    UINT32 current_state : 4;
    /* current_state - Bits[3:0], RO_V, default = 4'b0000 
       The current state of the Intel LINK 2 Error flow FSM.
     */
    UINT32 rsvd_4 : 12;
    /* rsvd_4 - Bits[15:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 previous_state : 4;
    /* previous_state - Bits[19:16], RO_V, default = 4'b0000 
       The state of the Intel LINK 2 Error flow FSM before the current state.
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VMSE_ERR_FSM_STATE_MCDDC_DP_STRUCT;


/* DDR4_WRCRC2_DQ15_DQ0_MCDDC_DP_REG supported on:                            */
/*       SKX_A0 (0x4025343C)                                                  */
/*       SKX (0x4025343C)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC2_DQ15_DQ0_MCDDC_DP_REG 0x0B01443C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 15 to 0. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC2_DQ15_DQ0_MCDDC_DP_STRUCT;


/* DDR4_WRCRC2_DQ31_DQ16_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253440)                                                  */
/*       SKX (0x40253440)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC2_DQ31_DQ16_MCDDC_DP_REG 0x0B014440
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 31 to 16. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC2_DQ31_DQ16_MCDDC_DP_STRUCT;


/* DDR4_WRCRC2_DQ47_DQ32_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253444)                                                  */
/*       SKX (0x40253444)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC2_DQ47_DQ32_MCDDC_DP_REG 0x0B014444
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 47 to 32. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC2_DQ47_DQ32_MCDDC_DP_STRUCT;


/* DDR4_WRCRC2_DQ63_DQ48_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x4025344C)                                                  */
/*       SKX (0x4025344C)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC2_DQ63_DQ48_MCDDC_DP_REG 0x0B01444C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 63 to 48. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC2_DQ63_DQ48_MCDDC_DP_STRUCT;


/* DDR4_WRCRC2_CTL_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253450)                                                  */
/*       SKX (0x40253450)                                                     */
/* Register default value:              0xE4E40000                            */
#define DDR4_WRCRC2_CTL_MCDDC_DP_REG 0x0B014450
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 4;
    /* rsvd_0 - Bits[3:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 enable_nibble_swizzle : 9;
    /* enable_nibble_swizzle - Bits[12:4], RW_LB, default = 9'h0 
       Enable swizzling per nibble
     */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dq71_dq64_swizzle_ctl : 16;
    /* dq71_dq64_swizzle_ctl - Bits[31:16], RW_LB, default = 16'he4e4 
       Swizzling for DQ Bits 71 to 43. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC2_CTL_MCDDC_DP_STRUCT;


/* DDR4_WRCRC3_DQ15_DQ0_MCDDC_DP_REG supported on:                            */
/*       SKX_A0 (0x40253454)                                                  */
/*       SKX (0x40253454)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC3_DQ15_DQ0_MCDDC_DP_REG 0x0B014454
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 15 to 0. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC3_DQ15_DQ0_MCDDC_DP_STRUCT;


/* DDR4_WRCRC3_DQ31_DQ16_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253458)                                                  */
/*       SKX (0x40253458)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC3_DQ31_DQ16_MCDDC_DP_REG 0x0B014458
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 31 to 16. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC3_DQ31_DQ16_MCDDC_DP_STRUCT;


/* DDR4_WRCRC3_DQ47_DQ32_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x4025345C)                                                  */
/*       SKX (0x4025345C)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC3_DQ47_DQ32_MCDDC_DP_REG 0x0B01445C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 47 to 32. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC3_DQ47_DQ32_MCDDC_DP_STRUCT;


/* DDR4_WRCRC3_DQ63_DQ48_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253460)                                                  */
/*       SKX (0x40253460)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC3_DQ63_DQ48_MCDDC_DP_REG 0x0B014460
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 63 to 48. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC3_DQ63_DQ48_MCDDC_DP_STRUCT;


/* DDR4_WRCRC3_CTL_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253464)                                                  */
/*       SKX (0x40253464)                                                     */
/* Register default value:              0xE4E40000                            */
#define DDR4_WRCRC3_CTL_MCDDC_DP_REG 0x0B014464
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 4;
    /* rsvd_0 - Bits[3:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 enable_nibble_swizzle : 9;
    /* enable_nibble_swizzle - Bits[12:4], RW_LB, default = 9'h0 
       Enable swizzling per nibble
     */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dq71_dq64_swizzle_ctl : 16;
    /* dq71_dq64_swizzle_ctl - Bits[31:16], RW_LB, default = 16'he4e4 
       Swizzling for DQ Bits 71 to 43. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC3_CTL_MCDDC_DP_STRUCT;


/* DDR4_WRCRC0_DQ15_DQ0_MCDDC_DP_REG supported on:                            */
/*       SKX_A0 (0x40253468)                                                  */
/*       SKX (0x40253468)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC0_DQ15_DQ0_MCDDC_DP_REG 0x0B014468
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 15 to 0. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC0_DQ15_DQ0_MCDDC_DP_STRUCT;


/* DDR4_WRCRC0_DQ31_DQ16_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x4025346C)                                                  */
/*       SKX (0x4025346C)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC0_DQ31_DQ16_MCDDC_DP_REG 0x0B01446C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 31 to 16. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC0_DQ31_DQ16_MCDDC_DP_STRUCT;


/* DDR4_WRCRC0_DQ47_DQ32_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253470)                                                  */
/*       SKX (0x40253470)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC0_DQ47_DQ32_MCDDC_DP_REG 0x0B014470
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 47 to 32. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC0_DQ47_DQ32_MCDDC_DP_STRUCT;


/* DDR4_WRCRC0_DQ63_DQ48_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253474)                                                  */
/*       SKX (0x40253474)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC0_DQ63_DQ48_MCDDC_DP_REG 0x0B014474
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 63 to 48. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC0_DQ63_DQ48_MCDDC_DP_STRUCT;


/* DDR4_WRCRC0_CTL_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253478)                                                  */
/*       SKX (0x40253478)                                                     */
/* Register default value:              0xE4E40000                            */
#define DDR4_WRCRC0_CTL_MCDDC_DP_REG 0x0B014478
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 0
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 4;
    /* rsvd_0 - Bits[3:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 enable_nibble_swizzle : 9;
    /* enable_nibble_swizzle - Bits[12:4], RW_LB, default = 9'h0 
       Enable swizzling per nibble
     */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dq71_dq64_swizzle_ctl : 16;
    /* dq71_dq64_swizzle_ctl - Bits[31:16], RW_LB, default = 16'he4e4 
       Swizzling for DQ Bits 71 to 43. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC0_CTL_MCDDC_DP_STRUCT;


/* DDR4_WRCRC1_DQ15_DQ0_MCDDC_DP_REG supported on:                            */
/*       SKX_A0 (0x4025347C)                                                  */
/*       SKX (0x4025347C)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC1_DQ15_DQ0_MCDDC_DP_REG 0x0B01447C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 1
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 15 to 0. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC1_DQ15_DQ0_MCDDC_DP_STRUCT;


/* DDR4_WRCRC1_DQ31_DQ16_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253480)                                                  */
/*       SKX (0x40253480)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC1_DQ31_DQ16_MCDDC_DP_REG 0x0B014480
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 1
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 31 to 16. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC1_DQ31_DQ16_MCDDC_DP_STRUCT;


/* DDR4_WRCRC1_DQ47_DQ32_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253484)                                                  */
/*       SKX (0x40253484)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC1_DQ47_DQ32_MCDDC_DP_REG 0x0B014484
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 1
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 47 to 32. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC1_DQ47_DQ32_MCDDC_DP_STRUCT;


/* DDR4_WRCRC1_DQ63_DQ48_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253488)                                                  */
/*       SKX (0x40253488)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DDR4_WRCRC1_DQ63_DQ48_MCDDC_DP_REG 0x0B014488
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 1
 */
typedef union {
  struct {
    UINT32 swizzle_ctl : 32;
    /* swizzle_ctl - Bits[31:0], RW_LB, default = 32'he4e4e4e4 
       Swizzling for DQ Bits 63 to 48. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC1_DQ63_DQ48_MCDDC_DP_STRUCT;


/* DDR4_WRCRC1_CTL_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x4025348C)                                                  */
/*       SKX (0x4025348C)                                                     */
/* Register default value:              0xE4E40000                            */
#define DDR4_WRCRC1_CTL_MCDDC_DP_REG 0x0B01448C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 1
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 4;
    /* rsvd_0 - Bits[3:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 enable_nibble_swizzle : 9;
    /* enable_nibble_swizzle - Bits[12:4], RW_LB, default = 9'h0 
       Enable swizzling per nibble
     */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dq71_dq64_swizzle_ctl : 16;
    /* dq71_dq64_swizzle_ctl - Bits[31:16], RW_LB, default = 16'he4e4 
       Swizzling for DQ Bits 71 to 43. 2 CSR bits per DQ lane
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC1_CTL_MCDDC_DP_STRUCT;


/* DDR4_WRCRC_RANK_CTL_MCDDC_DP_REG supported on:                             */
/*       SKX_A0 (0x40253490)                                                  */
/*       SKX (0x40253490)                                                     */
/* Register default value:              0x00000000                            */
#define DDR4_WRCRC_RANK_CTL_MCDDC_DP_REG 0x0B014490
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDR4 Write CRC swizzling control for Rank 1
 */
typedef union {
  struct {
    UINT32 rank0 : 2;
    /* rank0 - Bits[1:0], RW_LB, default = 2'h0 
       DDR4 WR CRC mask to use for Rank0
     */
    UINT32 rank1 : 2;
    /* rank1 - Bits[3:2], RW_LB, default = 2'h0 
       DDR4 WR CRC mask to use for Rank1
     */
    UINT32 rank2 : 2;
    /* rank2 - Bits[5:4], RW_LB, default = 2'h0 
       DDR4 WR CRC mask to use for Rank2
     */
    UINT32 rank3 : 2;
    /* rank3 - Bits[7:6], RW_LB, default = 2'h0 
       DDR4 WR CRC mask to use for Rank3
     */
    UINT32 rank4 : 2;
    /* rank4 - Bits[9:8], RW_LB, default = 2'h0 
       DDR4 WR CRC mask to use for Rank4
     */
    UINT32 rank5 : 2;
    /* rank5 - Bits[11:10], RW_LB, default = 2'h0 
       DDR4 WR CRC mask to use for Rank5
     */
    UINT32 rank6 : 2;
    /* rank6 - Bits[13:12], RW_LB, default = 2'h0 
       DDR4 WR CRC mask to use for Rank6
     */
    UINT32 rank7 : 2;
    /* rank7 - Bits[15:14], RW_LB, default = 2'h0 
       DDR4 WR CRC mask to use for Rank7
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDR4_WRCRC_RANK_CTL_MCDDC_DP_STRUCT;




/* CPGC_PATWDBCLMUX_LMN_MCDDC_DP_REG supported on:                            */
/*       SKX_A0 (0x40253500)                                                  */
/*       SKX (0x40253500)                                                     */
/* Register default value:              0x01010100                            */
#define CPGC_PATWDBCLMUX_LMN_MCDDC_DP_REG 0x0B014500
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux LMN Control -
 * Template for the LMN Counter used for creating periodic pattern (M = HighChunk 
 * transfers + N= Low Chunk transfers with an initial delay = L Chunk transfers) 
 */
typedef union {
  struct {
    UINT32 l_datsel : 1;
    /* l_datsel - Bits[0:0], RW_LB, default = 1'b0 
       0x0 = logic 0 must be driven during the initial L counter delay
       0x1=logic 1 must be driven during the initial L counter delay
     */
    UINT32 sweep_freq : 1;
    /* sweep_freq - Bits[1:1], RW_LB, default = 1'b0 
       After L_Counter Chunk transfers are driven at the beginning of a test then 
       periodic pattern is continuously repeated depending on the value of Sweep 
       Frequency. 
       If Sweep_Frequency = 0 then the following pattern is continuously repeated:
       1) The opposite polarity as L (i.e. If L is high M is then Low) must be driven 
       for M+1 Qclk cycles 
       2) Followed immediately by the same polarity as L (i.e. If L is high N is then 
       High) must be driven for N+1 Qclk cycles 
       3) Repeat step 1)
       If Sweep_Frequency = 0 and and if either L, M, or N = 0 then the the state will 
       freeze in whoever is programmed to 0 starting with L then M and then N in terms 
       of priority. 
       If Sweep_Frequency =1 then the following pattern is continuously repeated:
       1) X = M
       2) X is driven opposite polarity as L (i.e. If L is high then the current 
       polarity is Low) for X Qclk cycles. 
       3) The same polarity as L (i.e. If L is high then the current polarity is High) 
       must be driven for X Qclk cycles. 
       4) The opposite polarity as L is driven for X Qclk cycles.
       5) X = X +1
       6) The Same polarity as L is driven for X Qclk cycles.
       7) If X = N then go to step 1 else go to step 2
       L, M, and N must never be programmed to 0 if Sweep_Frequency = 1 and is 
       considered undefined. 
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 l_cnt : 8;
    /* l_cnt - Bits[15:8], RW_LB, default = 8'b00000001 
       After L_Counter Chunk transfers are driven at the beginning of a test then 
       periodic pattern is continuously repeated depending on 
       the value of Sweep Frequency.
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M) to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
    UINT32 m_cnt : 8;
    /* m_cnt - Bits[23:16], RW_LB, default = 8'b00000001 
       After L_Counter Chunk transfers are driven at the beginning of a test then 
       periodic pattern is continuously repeated depending on 
       the value of Sweep Frequency.
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M) to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
    UINT32 n_cnt : 8;
    /* n_cnt - Bits[31:24], RW_LB, default = 8'b00000001 
       After L_Counter Chunk transfers are driven at the beginning of a test then 
       periodic pattern is continuously repeated depending on 
       the value of Sweep Frequency.
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M) to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCLMUX_LMN_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x4025350C)                                                  */
/*       SKX (0x4025350C)                                                     */
/* Register default value:              0x00AAAAAA                            */
#define CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG 0x0B01450C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux0 PB Write -
 * Template for the 24 Bit Buffer, also used for the LFSR seed.
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RW_V, default = 24'b101010101010101010101010 
       Used to program the initial value for of the Pattern/LFSR Buffer in the Read 
       comparison domain. 
       If the LFSR reload and save registrers are being used then this register will 
       not hold the initial value, but instead will hold the 
       last saved value.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253510)                                                  */
/*       SKX (0x40253510)                                                     */
/* Register default value:              0x00CCCCCC                            */
#define CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_REG 0x0B014510
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux1 PB Write -
 * Template for the 24 Bit Buffer, also used for the LFSR seed.
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RW_V, default = 24'b110011001100110011001100 
       Used to program the initial value for of the Pattern/LFSR Buffer in the Read 
       comparison domain. 
       If the LFSR reload and save registrers are being used then this register will 
       not hold the initial value, but instead will hold the 
       last saved value.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253514)                                                  */
/*       SKX (0x40253514)                                                     */
/* Register default value:              0x00F0F0F0                            */
#define CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_REG 0x0B014514
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux2 PB Write -
 * Template for the 24 Bit Buffer, also used for the LFSR seed.
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RW_V, default = 24'b111100001111000011110000 
       Used to program the initial value for of the Pattern/LFSR Buffer in the Read 
       comparison domain. 
       If the LFSR reload and save registrers are being used then this register will 
       not hold the initial value, but instead will hold the 
       last saved value.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253518)                                                  */
/*       SKX (0x40253518)                                                     */
/* Register default value:              0x00AAAAAA                            */
#define CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG 0x0B014518
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux0 PB Read -
 * Template for the 24 Bit Buffer, also used for the LFSR seed.
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RW_V, default = 24'b101010101010101010101010 
       Used to program the initial value for of the Pattern/LFSR Buffer in the Read 
       comparison domain. 
       If the LFSR reload and save registrers are being used then this register will 
       not hold the initial value, but instead will hold the 
       last saved value.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x4025351C)                                                  */
/*       SKX (0x4025351C)                                                     */
/* Register default value:              0x00CCCCCC                            */
#define CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG 0x0B01451C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux1 PB Read -
 * Template for the 24 Bit Buffer, also used for the LFSR seed.
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RW_V, default = 24'b110011001100110011001100 
       Used to program the initial value for of the Pattern/LFSR Buffer in the Read 
       comparison domain. 
       If the LFSR reload and save registrers are being used then this register will 
       not hold the initial value, but instead will hold the 
       last saved value.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG supported on:                         */
/*       SKX_A0 (0x40253520)                                                  */
/*       SKX (0x40253520)                                                     */
/* Register default value:              0x00F0F0F0                            */
#define CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG 0x0B014520
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux2 PB Read -
 * Template for the 24 Bit Buffer, also used for the LFSR seed.
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RW_V, default = 24'b111100001111000011110000 
       Used to program the initial value for of the Pattern/LFSR Buffer in the Read 
       comparison domain. 
       If the LFSR reload and save registrers are being used then this register will 
       not hold the initial value, but instead will hold the 
       last saved value.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_STRUCT;


/* CPGC_PATWDB_INV_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253524)                                                  */
/*       SKX (0x40253524)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDB_INV_MCDDC_DP_REG 0x0B014524
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Inversion and DC Control -
 * Control Logic used to program and control the WDB Inversion Mask
 */
typedef union {
  struct {
    UINT32 eccinv_or_dcenb : 8;
    /* eccinv_or_dcenb - Bits[7:0], RW_LB, default = 8'b00000000 
       Each bit in ECC_Inv_or_DC_Enable corresponds to a specific ECC Lane.
       When any bit is set this indicates that the selected data lane must be inverted 
       or drive a DC signal based on Data_INV_or_DC_Control. 
       When Enable_Inv_or_DC_Rotate is set the Data_Inv_or_DC_Enable and 
       ECC_Inv_or_DC_Enable are considered one continuous shift register with 
       Data_Inv_or_DC_Enable making up bits bits 0:63 and ECC_Inv_or_DC_Enable making 
       up bits 64:71 
       Bit 0 of ECC_Inv_or_DC_Enable corresponds to ECC 0.
     */
    UINT32 rsvd_8 : 8;
    /* rsvd_8 - Bits[15:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 invordc_shft_rate : 4;
    /* invordc_shft_rate - Bits[19:16], RW_LB, default = 4'b0000 
       If Inv_or_DC_Shift_Rate is enabled then the Data_Inv_or_DC_Enable and 
       ECC_Inv_or_DC_Enable will be left shifted every time 2^(Inv_or_DC_Shift_Rate) 
       cachelines are executed.
       When a rotation happens
       Bit 0 of Data_Inv_or_DC_Enable rotates into Bit 1 of Data_Inv_or_DC_Enable
       Bit 63 of Data_Inv_or_DC_Enable rotates into Bit 0 of the ECC_Inv_or_DC_Enable
       Bit 7 of ECC_Inv_or_DC_Enable rotates into Bit 0 of the Data_Inv_or_DC_Enable
     */
    UINT32 dc_polarity : 1;
    /* dc_polarity - Bits[20:20], RW_LB, default = 1'b0 
       DC_Polarity_Control indicates whether the polarity of the DC signal driven when 
       Inv_or_DC_Control = 1 will be logic high or logic low. 
       DC_Polarity_Control = 0 indicates a logic low will be driven.
       DC_Polarity_Control = 1 indicates a logic high will be driven.
     */
    UINT32 rsvd_21 : 9;
    /* rsvd_21 - Bits[29:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 invordc_ctl : 1;
    /* invordc_ctl - Bits[30:30], RW_LB, default = 1'b0 
       Inv_or_DC_Control controls whether a Lane is inverted in polarity or a DC signal 
       will be driven out for any bits set in the ECC_Inv_or_DC_Enable 
       and Lane_Inv_or_DC_Enable.
       Inv_or_DC_Control = 0 indicates the selected lane will be inverted
       Inv_or_DC_Control = 1 indicates the selected lane will drive a DC signal with a 
       logic value dictated by the DC_Polarity. 
     */
    UINT32 datainv_or_dcshftenb : 1;
    /* datainv_or_dcshftenb - Bits[31:31], RW_LB, default = 1'b0 
       When Inv_or_DC_Shift_Enable is set to 1 then the Data_Inv_or_DC_Enable and 
       ECC_Inv_or_DC_Enable will be left shifted 
       every time 2^(Inv_or_DC_Shift_Rate) cachelines are executed.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDB_INV_MCDDC_DP_STRUCT;


/* CPGC_PATWDB_INV0_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x40253528)                                                  */
/*       SKX (0x40253528)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDB_INV0_MCDDC_DP_REG 0x0B014528
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Inversion Mask Lower -
 * Control Logic used to program and control the WDB Inversion Mask - Bits 31:0
 */
typedef union {
  struct {
    UINT32 datainv_or_dcenb : 32;
    /* datainv_or_dcenb - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Each bit in Data_Inv_or_DC_Enable corresponds to a specific data Lane. [31:0]
       When any bit is set this indicates that the selected data lane must be inverted 
       or drive a DC signal based on Data_INV_or_DC_Control. 
       When Enable_Inv_or_DC_Rotate is set the Data_Inv_or_DC_Enable and 
       ECC_Inv_or_DC_Enable are considered one continuous shift register with 
       Data_Inv_or_DC_Enable making up bits bits 0:63 and ECC_Inv_or_DC_Enable making 
       up bits 64:71 
       Bit 0 of Data_Inv_or_DC_Enable corresponds to lane 0.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDB_INV0_MCDDC_DP_STRUCT;


/* CPGC_PATWDB_INV1_MCDDC_DP_REG supported on:                                */
/*       SKX_A0 (0x4025352C)                                                  */
/*       SKX (0x4025352C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDB_INV1_MCDDC_DP_REG 0x0B01452C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Inversion Mask Upper -
 * Control Logic used to program and control the WDB Inversion Mask - Bits 63:32
 */
typedef union {
  struct {
    UINT32 datainv_or_dcenb : 32;
    /* datainv_or_dcenb - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Each bit in Data_Inv_or_DC_Enable corresponds to a specific data Lane. [63:32]
       When any bit is set this indicates that the selected data lane must be inverted 
       or drive a DC signal based on Data_INV_or_DC_Control. 
       When Enable_Inv_or_DC_Rotate is set the Data_Inv_or_DC_Enable and 
       ECC_Inv_or_DC_Enable are considered one continuous shift register with 
       Data_Inv_or_DC_Enable making up bits bits 0:63 and ECC_Inv_or_DC_Enable making 
       up bits 64:71 
       Bit 0 of Data_Inv_or_DC_Enable corresponds to lane 0.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDB_INV1_MCDDC_DP_STRUCT;


/* CPGC_PATWDB_RDWR_PNTR_MCDDC_DP_REG supported on:                           */
/*       SKX_A0 (0x40253530)                                                  */
/*       SKX (0x40253530)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDB_RDWR_PNTR_MCDDC_DP_REG 0x0B014530
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Used to indicate what entry of the WDB is being read and written
 */
typedef union {
  struct {
    UINT32 rdwr_pntr : 6;
    /* rdwr_pntr - Bits[5:0], RW_V, default = 6'b000000 
       WDB_Read_Write_Pointer indicates what cacheline in the WDB that is being indexed 
       in the REUT_CH#_PAT_WDB_PROG#.WDB_Data 
       registers for writing of the contents of the WDB (See WDB_Data for more 
       details). 
       Programming sequence : write a value to
       PAT_WDB_RD_WR_PNTR;write a value to
       CPGC_PATWDB_WR0; write a value to
       CPGC_PATWDB_WR1; at this point the 64-bit value
       {CPGC_PATWDB_WR1, CPGC_PATWDB_WR0} will be
       transfered to the chunk Rdwr_Subpntr of
       WDB_entry rdwr_pntr. Rdwr_subpntr will be
       auto_incremented after each transfer like this
       and rdwr_pntr will be auto_incremented after all
       8 chunks of a cacheline have been updated.
     */
    UINT32 rdwr_subpntr : 3;
    /* rdwr_subpntr - Bits[8:6], RW_V, default = 3'b000 
       WDB_Read_Write_Sub_Pointer indicates what portion of a cacheline(64-bits) in the 
       WDB that is being indexed in the 
       REUT_CH#_PAT_WDB_READ.WDB_Data and REUT_CH#_PAT_WDB_WRITE.WDB_Data registers for 
       writing of the contents of the WDB. 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDB_RDWR_PNTR_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG supported on:                            */
/*       SKX_A0 (0x40253534)                                                  */
/*       SKX (0x40253534)                                                     */
/* Register default value:              0x00004049                            */
#define CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG 0x0B014534
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux Control -
 * Control Logic used to determine which chunk of cacheline data will be sent out 
 * each Q clk (8:1 Mux) 
 */
typedef union {
  struct {
    UINT32 mux0 : 2;
    /* mux0 - Bits[1:0], RW_LB, default = 2'b01 
       0x00 = Select LMN counter
       0x01 = Select 24 bit buffer
       0x10 = Select LFSR24
       0x11 = Reserved
     */
    UINT32 rsvd_2 : 1;
    /* rsvd_2 - Bits[2:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux1 : 2;
    /* mux1 - Bits[4:3], RW_LB, default = 2'b01 
       0x00 = Reserved
       0x01 = Select 24 bit buffer
       0x10 = Select LFSR24
       0x11 = Reserved
     */
    UINT32 rsvd_5 : 1;
    /* rsvd_5 - Bits[5:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux2 : 2;
    /* mux2 - Bits[7:6], RW_LB, default = 2'b01 
       0x00 = Reserved
       0x01 = Select 24 bit buffer
       0x10 = Select LFSR24
       0x11 = Reserved
     */
    UINT32 rsvd_8 : 6;
    /* rsvd_8 - Bits[13:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ecc_replace_byte : 1;
    /* ecc_replace_byte - Bits[14:14], RW_LB, default = 1'b1 
       ecc_replace_byte will allow the capture ECC cacheline errors into the WDB when 
       Enable_WDB_Error_Capture is set to 1. 
       ecc_replace_byte is used by the Read data path to Mux the received ECC byte into 
       the same byte group indicated by ecc_datasrc_sel. 
       If ecc_replace_byte is set to 0 then ECC XOR data (expected vs. received) will 
       never be captured in the WDB. 
       If ecc_replace_byte is set to 1 then ECC XOR data (expected vs. received) will 
       replace the XOR results of a selected data group 
       (indicated by ecc_datasrc_sel) when cacheline errors are written into the WDB.
     */
    UINT32 ecc_datasrc_sel : 1;
    /* ecc_datasrc_sel - Bits[15:15], RW_LB, default = 1'b0 
       Indicates whether byte 0 (Dq0-Dq7) or byte 7 (dq56-dq63) is transmitted and 
       compared for the ECC byte. 
       0= byte group 0 will be transmitted and compared for the ECC byte.
       1= byte group 7 will be transmitted and compared for the ECC byte.
       Hardware directs if ECC is included within Generation and Checking.
     */
    UINT32 save_lfsr_seedrate : 6;
    /* save_lfsr_seedrate - Bits[21:16], RW_LB, default = 6'b000000 
       save_lfsr_seedrate * ^(rld_lfsr_seedrate-1) defines the periodic cacheline 
       interval that the current LFSR value REUT#_CH#_PAT_CL_MUX#_PB_STATUS 
       is saved into the REUT#_CH#_PAT_CL_MUX#_PB register.
       When the save_lfsr_seedrate and rld_lfsr_seedrate is reached in the same cycle 
       only the saving of the LFSR seed takes place. 
       Examples:
       0 = Disable - the REUT#_CH#_PAT_CL_MUX#_PB_STATUS is never saved into the 
       REUT#_CH#_PAT_CL_MUX#_PB register. 
       1 = The REUT#_CH#_PAT_CL_MUX#_PB_STATUS is saved into the 
       REUT#_CH#_PAT_CL_MUX#_PB register every 2^(Reload_LFSR_Seed_Rate-1) cachelines. 
       2 = The REUT#_CH#_PAT_CL_MUX#_PB_STATUS is saved into the 
       REUT#_CH#_PAT_CL_MUX#_PB register every other 2^(Reload_LFSR_Seed_Rate-1) 
       cachelines. 
       3 = The REUT#_CH#_PAT_CL_MUX#_PB_STATUS is saved into the 
       REUT#_CH#_PAT_CL_MUX#_PB register every fourth 2^(Reload_LFSR_Seed_Rate-1) 
       cachelines. 
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rld_lfsr_seedrate : 3;
    /* rld_lfsr_seedrate - Bits[26:24], RW_LB, default = 3'b000 
       2^(rld_lfsr_seedrate-1) defines the periodic cacheline interval that the current 
       LFSR Seed is reloaded from REUT#_CH#_PAT_CL_MUX#_PB. 
       When the save_lfsr_seedrate and rld_lfsr_seedrate is reached in the same cycle 
       only the saving of the LFSR seed takes place. 
       Examples:
       0 = Disable - the REUT#_CH#_PAT_CL_MUX#_PB_STATUS is never reloaded into the 
       REUT#_CH#_PAT_CL_MUX#_PB register. 
       1 = The LFSR seed is reloaded every cacheline operation from 
       REUT#_CH#_PAT_CL_MUX#_PB. 
       2 = The LFSR seed is reloaded every other cacheline operation from 
       REUT#_CH#_PAT_CL_MUX#_PB 
       3 = The LFSR seed is reloaded every fourth cacheline operation from 
       REUT#_CH#_PAT_CL_MUX#_PB 
     */
    UINT32 cpgc_lfsr_2code_mode : 1;
    /* cpgc_lfsr_2code_mode - Bits[27:27], RW_LB, default = 1'b0 
       Set this bit fr CPGC training or DIMM testing under SMI 2:1 LFSR mode. This bit 
       will allow the LFSR to generate 
       4 LFSR codes per DCLK. The default mode is 2 codes per DCLK. The default mode is 
       used in native mode and SMI 1:1 
       modes.
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCLMUX0_PBSTS_WR_MCDDC_DP_REG supported on:                      */
/*       SKX_A0 (0x40253538)                                                  */
/*       SKX (0x40253538)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDBCLMUX0_PBSTS_WR_MCDDC_DP_REG 0x0B014538
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux0 PB Status Write -
 * Status of the final Pattern Buffer/LFSR/MN following a test. Not valid during a 
 * test. 
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       Current Status of the Pattern/LFSR Buffer in the Write domain.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCLMUX0_PBSTS_WR_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCLMUX1_PBSTS_WR_MCDDC_DP_REG supported on:                      */
/*       SKX_A0 (0x4025353C)                                                  */
/*       SKX (0x4025353C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDBCLMUX1_PBSTS_WR_MCDDC_DP_REG 0x0B01453C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux1 PB Status Write -
 * Status of the final Pattern Buffer/LFSR/MN following a test. Not valid during a 
 * test. 
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       Current Status of the Pattern/LFSR Buffer in the Write domain.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCLMUX1_PBSTS_WR_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCLMUX2_PBSTS_WR_MCDDC_DP_REG supported on:                      */
/*       SKX_A0 (0x40253540)                                                  */
/*       SKX (0x40253540)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDBCLMUX2_PBSTS_WR_MCDDC_DP_REG 0x0B014540
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux2 PB Status Write -
 * Status of the final Pattern Buffer/LFSR/MN following a test. Not valid during a 
 * test. 
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       Current Status of the Pattern/LFSR Buffer in the Write domain.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCLMUX2_PBSTS_WR_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCLMUX0_PBSTS_RD_MCDDC_DP_REG supported on:                      */
/*       SKX_A0 (0x40253544)                                                  */
/*       SKX (0x40253544)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDBCLMUX0_PBSTS_RD_MCDDC_DP_REG 0x0B014544
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux0 PB Status Read -
 * Status of the final Pattern Buffer/LFSR/MN following a test. Not valid during a 
 * test. 
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       Current Status of the Pattern/LFSR Buffer in the Read comparison domain.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCLMUX0_PBSTS_RD_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCLMUX1_PBSTS_RD_MCDDC_DP_REG supported on:                      */
/*       SKX_A0 (0x40253548)                                                  */
/*       SKX (0x40253548)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDBCLMUX1_PBSTS_RD_MCDDC_DP_REG 0x0B014548
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux1 PB Status Read -
 * Status of the final Pattern Buffer/LFSR/MN following a test. Not valid during a 
 * test. 
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       Current Status of the Pattern/LFSR Buffer in the Read comparison domain.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCLMUX1_PBSTS_RD_MCDDC_DP_STRUCT;


/* CPGC_PATWDBCLMUX2_PBSTS_RD_MCDDC_DP_REG supported on:                      */
/*       SKX_A0 (0x4025354C)                                                  */
/*       SKX (0x4025354C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDBCLMUX2_PBSTS_RD_MCDDC_DP_REG 0x0B01454C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * CPGC Pattern WDB Cache-Line Mux2 PB Status Read -
 * Status of the final Pattern Buffer/LFSR/MN following a test. Not valid during a 
 * test. 
 */
typedef union {
  struct {
    UINT32 patbuffer : 24;
    /* patbuffer - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       Current Status of the Pattern/LFSR Buffer in the Read comparison domain.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCLMUX2_PBSTS_RD_MCDDC_DP_STRUCT;


/* CPGC_PATWDB_WR0_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253550)                                                  */
/*       SKX (0x40253550)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDB_WR0_MCDDC_DP_REG 0x0B014550
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Buffer for writing the lower 32 bits of 1/8 of a  cacheline of the WDB buffer.
 */
typedef union {
  struct {
    UINT32 wdb_data : 32;
    /* wdb_data - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Writes can only happen while in Loopback.Marker (Test Done is set) or 
       (Enable_WDB_Error_Capture is not set while in Loopback.Pattern). 
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDB_WR0_MCDDC_DP_STRUCT;


/* CPGC_PATWDB_WR1_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253554)                                                  */
/*       SKX (0x40253554)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDB_WR1_MCDDC_DP_REG 0x0B014554
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Buffer for writing the upper 32 bits of 1/8 of a  cacheline of the WDB buffer.
 */
typedef union {
  struct {
    UINT32 wdb_data : 32;
    /* wdb_data - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Writes can only happen while in Loopback.Marker (Test Done is set) or 
       (Enable_WDB_Error_Capture is not set while in Loopback.Pattern). 
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDB_WR1_MCDDC_DP_STRUCT;






/* DDDC_CNTL_0_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253608)                                                  */
/*       SKX (0x10253608)                                                     */
/* Register default value:              0x3F                                  */
#define DDDC_CNTL_0_MCDDC_DP_REG 0x0B011608
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * DDDC Control Register for LINK1:1x4 Channels 0/2
 * Usage model - BIOS may only enable DDDC when operating in LINK1:1x4 mode.  The 
 * first device failure will enable DDDC, the second device failure may enable 
 * SDDC. 
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       Hardware will capture the fail device ID on a correctable error until EN field 
       has been set, after which hardware may no longer update. Any further fail device 
       ID will be captured in DEVTAG_CNTL. Valid range is 0-35 decimal. Above 35 means 
       no device failure has occurred on this rank. 
       Device ID mapping:
       35: Parity
       34: Spare
       33-18: Data
       17: CRC
       16: CRC
       15-0: Data
     */
    UINT8 sparing : 1;
    /* sparing - Bits[6:6], RWS_L, default = 1'b0 
       This field will be set by BIOS before the DDDC sparing flow begins to indicate 
       that this rank is currently in the sparing flow. The field is cleared by 
       hardware at the completion of DDDC sparing. SPARING and EN shall be mutually 
       exclusive. 
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       DDDC enable for this rank. When set, this rank will use the device mapping 
       specified by FAILDEVICE. This field is written by hardware at the completion of 
       DDDC sparing. SPARING and EN shall be mutually exclusive. 
     */
  } Bits;
  UINT8 Data;
} DDDC_CNTL_0_MCDDC_DP_STRUCT;


/* DDDC_CNTL_1_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253609)                                                  */
/*       SKX (0x10253609)                                                     */
/* Register default value:              0x3F                                  */
#define DDDC_CNTL_1_MCDDC_DP_REG 0x0B011609
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DDDC_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DDDC_CNTL_0 for description.
     */
    UINT8 sparing : 1;
    /* sparing - Bits[6:6], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DDDC_CNTL_1_MCDDC_DP_STRUCT;


/* DDDC_CNTL_2_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x1025360A)                                                  */
/*       SKX (0x1025360A)                                                     */
/* Register default value:              0x3F                                  */
#define DDDC_CNTL_2_MCDDC_DP_REG 0x0B01160A
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DDDC_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DDDC_CNTL_0 for description.
     */
    UINT8 sparing : 1;
    /* sparing - Bits[6:6], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DDDC_CNTL_2_MCDDC_DP_STRUCT;


/* DDDC_CNTL_3_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x1025360B)                                                  */
/*       SKX (0x1025360B)                                                     */
/* Register default value:              0x3F                                  */
#define DDDC_CNTL_3_MCDDC_DP_REG 0x0B01160B
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DDDC_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DDDC_CNTL_0 for description.
     */
    UINT8 sparing : 1;
    /* sparing - Bits[6:6], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DDDC_CNTL_3_MCDDC_DP_STRUCT;


/* DDDC_CNTL_4_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x1025360C)                                                  */
/*       SKX (0x1025360C)                                                     */
/* Register default value:              0x3F                                  */
#define DDDC_CNTL_4_MCDDC_DP_REG 0x0B01160C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DDDC_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DDDC_CNTL_0 for description.
     */
    UINT8 sparing : 1;
    /* sparing - Bits[6:6], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DDDC_CNTL_4_MCDDC_DP_STRUCT;


/* DDDC_CNTL_5_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x1025360D)                                                  */
/*       SKX (0x1025360D)                                                     */
/* Register default value:              0x3F                                  */
#define DDDC_CNTL_5_MCDDC_DP_REG 0x0B01160D
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DDDC_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DDDC_CNTL_0 for description.
     */
    UINT8 sparing : 1;
    /* sparing - Bits[6:6], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DDDC_CNTL_5_MCDDC_DP_STRUCT;


/* DDDC_CNTL_6_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x1025360E)                                                  */
/*       SKX (0x1025360E)                                                     */
/* Register default value:              0x3F                                  */
#define DDDC_CNTL_6_MCDDC_DP_REG 0x0B01160E
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DDDC_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DDDC_CNTL_0 for description.
     */
    UINT8 sparing : 1;
    /* sparing - Bits[6:6], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DDDC_CNTL_6_MCDDC_DP_STRUCT;


/* DDDC_CNTL_7_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x1025360F)                                                  */
/*       SKX (0x1025360F)                                                     */
/* Register default value:              0x3F                                  */
#define DDDC_CNTL_7_MCDDC_DP_REG 0x0B01160F
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * See DDDC_CNTL_0 for description.
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_V, default = 6'b111111 
       See DDDC_CNTL_0 for description.
     */
    UINT8 sparing : 1;
    /* sparing - Bits[6:6], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
    UINT8 en : 1;
    /* en - Bits[7:7], RWS_L, default = 1'b0 
       See DDDC_CNTL_0 for description.
     */
  } Bits;
  UINT8 Data;
} DDDC_CNTL_7_MCDDC_DP_STRUCT;












/* MCDP_RCB_GATE_CTL_MCDDC_DP_REG supported on:                               */
/*       SKX_A0 (0x40253634)                                                  */
/*       SKX (0x40253634)                                                     */
/* Register default value:              0x00000040                            */
#define MCDP_RCB_GATE_CTL_MCDDC_DP_REG 0x0B014634
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Control information for RCB clock gating in mcdp
 */
typedef union {
  struct {
    UINT32 rcb_gate_mcdp_timer : 10;
    /* rcb_gate_mcdp_timer - Bits[9:0], RW_LB, default = 10'b0001000000 
       Time clock keeps running after triggering events
     */
    UINT32 rsvd : 22;
    /* rsvd - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCDP_RCB_GATE_CTL_MCDDC_DP_STRUCT;


/* MC0_DP_CHKN_BIT_MCDDC_DP_REG supported on:                                 */
/*       SKX_A0 (0x40253700)                                                  */
/*       SKX (0x40253700)                                                     */
/* Register default value:              0x28000002                            */
#define MC0_DP_CHKN_BIT_MCDDC_DP_REG 0x0B014700
/* Struct format extracted from XML file SKX_A0\2.10.3.CFG.xml.
 * Note: some defeatures are only present in MC0 register
 */
typedef union {
  struct {
    UINT32 dis_ecc_chk : 1;
    /* dis_ecc_chk - Bits[0:0], RW_LB, default = 1'b0 
       Disable ECC checking
     */
    UINT32 dis_rdimm_par_chk : 1;
    /* dis_rdimm_par_chk - Bits[1:1], RW_LB, default = 1'b1 
       Disable rdimm par check
     */
    UINT32 dis_ha_par_chk : 1;
    /* dis_ha_par_chk - Bits[2:2], RW_LB, default = 1'b0 
       Disable HA par check
     */
    UINT32 dis_ha_par_gen : 1;
    /* dis_ha_par_gen - Bits[3:3], RW_LB, default = 1'b0 
       Disable HA par gen
     */
    UINT32 sw_qck_for_scn_data : 1;
    /* sw_qck_for_scn_data - Bits[4:4], RW_LB, default = 1'b0 
       Switch Qclk for scan data
     */
    UINT32 frc_odd_cyc_rt_dly : 1;
    /* frc_odd_cyc_rt_dly - Bits[5:5], RW_LB, default = 1'b0 
       Force odd cycle RT delay
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dis_err_det_prtl_wr_underfl : 1;
    /* dis_err_det_prtl_wr_underfl - Bits[7:7], RW_LB, default = 1'b0 
       Disable error detection for partial write underfills
     */
    UINT32 ovrd_trng_rank_to_io : 1;
    /* ovrd_trng_rank_to_io - Bits[8:8], RW_LB, default = 1'b0 
       Override training rank output to IO
     */
    UINT32 ena_err_when_poison : 1;
    /* ena_err_when_poison - Bits[9:9], RW_LB, default = 1'b0 
       Error signaling to Home Agent when MC sees poison line and applies correction 
       trials during retry. 
       1: sending both poison and error indication (for poison line and correction 
       trials). 
       0: Don't send error indication for poison line when correction trials were 
       applied. Only assert poison. This behavior is necessary to correctly 
       deal with poison lines (in poison mode) under LINK error flows or under channel 
       mirroring flows. 
       Value of this bit is only relevant when in poison mode.
     */
    UINT32 dis_ck_gate_dp : 1;
    /* dis_ck_gate_dp - Bits[10:10], RW_LB, default = 1'b0 
       Disable clock gating - read and write data path
     */
    UINT32 sel_rank0_training : 1;
    /* sel_rank0_training - Bits[11:11], RW_LB, default = 1'b0 
       Select training rank to be rank 0; not used yet
     */
    UINT32 cfglateerr : 1;
    /* cfglateerr - Bits[12:12], RW_LB, default = 1'b0 
       Configure Err return to HA one cycle late
     */
    UINT32 mcforcercvend3nnh : 1;
    /* mcforcercvend3nnh - Bits[13:13], RW_LB, default = 1'b0 
       Always force Receive enable to be on
     */
    UINT32 uselateqclkrfrdnnn : 1;
    /* uselateqclkrfrdnnn - Bits[14:14], RW_LB, default = 1'b0 
       Use one Qclk late for iscan read fifo read enable
     */
    UINT32 ign_ptrl_uc : 1;
    /* ign_ptrl_uc - Bits[15:15], RW_LB, default = 1'b0 
       Downgrades patrol scrubbing uncorrectable errors to correactable.
     */
    UINT32 ign_mca_ovrd : 1;
    /* ign_mca_ovrd - Bits[16:16], RW_LB, default = 1'b0 
       When set to 1, disables the MCA regular override mechanism and keeps current 
       logs 
     */
    UINT32 log_sec_err_dev : 1;
    /* log_sec_err_dev - Bits[17:17], RW_LB, default = 1'b0 
       When set to 1, Logs the error device from second codeword in a cacheline access 
       if both codewords are in error. By default, MC logs the first codeword device as 
       the failing device in a cacheline (because there are two code words in each 
       cacheline). 
     */
    UINT32 en_uc_err_log : 1;
    /* en_uc_err_log - Bits[18:18], RW_LB, default = 1'b0 
       Enables logging of uncorrectable read errors to MCA banks. When this field is 
       set to 1 along with en_corr_rd_err_log, MC is in mode-2 
     */
    UINT32 en_corr_err_log : 1;
    /* en_corr_err_log - Bits[19:19], RW_LB, default = 1'b0 
       Enables logging of correctable read errors to MCA banks. When set to 1, this 
       puts MC in mode-1. 
     */
    UINT32 dis_wdb_par_chk : 1;
    /* dis_wdb_par_chk - Bits[20:20], RW_LB, default = 1'b0 
       Disable Write Data Buffer parity checking
     */
    UINT32 dis_spare_dir : 1;
    /* dis_spare_dir - Bits[21:21], RW_LB, default = 1'b0 
       Disable spare device directory storage.
     */
    UINT32 no_over_ce : 1;
    /* no_over_ce - Bits[22:22], RW_LB, default = 1'b0 
       Set to 1 to avoid CE to override CE in MCA banks.
     */
    UINT32 dis_vmse_link_fail : 1;
    /* dis_vmse_link_fail - Bits[23:23], RW_LB, default = 1'b0  */
    UINT32 en_rdimm_par_err_log : 1;
    /* en_rdimm_par_err_log - Bits[24:24], RW_LB, default = 1'b0 
       Enable RDIMM parity error logging.
     */
    UINT32 dis_rrd_psn : 1;
    /* dis_rrd_psn - Bits[25:25], RW_LB, default = 1'b0 
       Disable poison detection in the RRD.
     */
    UINT32 dis_verr_ecc_trials : 1;
    /* dis_verr_ecc_trials - Bits[26:26], RW_LB, default = 1'b0  */
    UINT32 dis_vmse_nb_hw : 1;
    /* dis_vmse_nb_hw - Bits[27:27], RW_LB, default = 1'b1  */
    UINT32 dis_vmse_sb_hw : 1;
    /* dis_vmse_sb_hw - Bits[28:28], RW_LB, default = 1'b0  */
    UINT32 dis_verr_scramsync : 1;
    /* dis_verr_scramsync - Bits[29:29], RW_LB, default = 1'b1  */
    UINT32 dis_vmse_data_scram : 1;
    /* dis_vmse_data_scram - Bits[30:30], RW_LB, default = 1'b0  */
    UINT32 dis_vmse_cmd_scram : 1;
    /* dis_vmse_cmd_scram - Bits[31:31], RW_LB, default = 1'b0  */
  } Bits;
  UINT32 Data;
} MC0_DP_CHKN_BIT_MCDDC_DP_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Note: some defeatures are only present in MC0 register
 */
typedef union {
  struct {
    UINT32 dis_ecc_chk : 1;
    /* dis_ecc_chk - Bits[0:0], RW_LB, default = 1'b0 
       Disable ECC checking
     */
    UINT32 dis_rdimm_par_chk : 1;
    /* dis_rdimm_par_chk - Bits[1:1], RW_LB, default = 1'b1 
       Disable rdimm par check
     */
    UINT32 dis_ha_par_chk : 1;
    /* dis_ha_par_chk - Bits[2:2], RW_LB, default = 1'b0 
       Disable HA write data parity check
     */
    UINT32 dis_ha_par_gen : 1;
    /* dis_ha_par_gen - Bits[3:3], RW_LB, default = 1'b0 
       Disable HA par gen
     */
    UINT32 sw_qck_for_scn_data : 1;
    /* sw_qck_for_scn_data - Bits[4:4], RW_LB, default = 1'b0 
       Switch Qclk for scan data
     */
    UINT32 frc_odd_cyc_rt_dly : 1;
    /* frc_odd_cyc_rt_dly - Bits[5:5], RW_LB, default = 1'b0 
       Force odd cycle RT delay
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dis_err_det_prtl_wr_underfl : 1;
    /* dis_err_det_prtl_wr_underfl - Bits[7:7], RW_LB, default = 1'b0 
       Disable error detection for partial write underfills
     */
    UINT32 ovrd_trng_rank_to_io : 1;
    /* ovrd_trng_rank_to_io - Bits[8:8], RW_LB, default = 1'b0 
       Override training rank output to IO
     */
    UINT32 ena_err_when_poison : 1;
    /* ena_err_when_poison - Bits[9:9], RW_LB, default = 1'b0 
       Error signaling to Home Agent when MC sees poison line and applies correction 
       trials during retry. 
       1: sending both poison and error indication (for poison line and correction 
       trials). 
       0: Don't send error indication for poison line when correction trials were 
       applied. Only assert poison. This behavior is necessary to correctly 
       deal with poison lines (in poison mode) under LINK error flows or under channel 
       mirroring flows. 
       Value of this bit is only relevant when in poison mode.
     */
    UINT32 dis_ck_gate_dp : 1;
    /* dis_ck_gate_dp - Bits[10:10], RW_LB, default = 1'b0 
       Disable clock gating - read and write data path
     */
    UINT32 sel_rank0_training : 1;
    /* sel_rank0_training - Bits[11:11], RW_LB, default = 1'b0 
       Select training rank to be rank 0; not used yet
     */
    UINT32 cfglateerr : 1;
    /* cfglateerr - Bits[12:12], RW_LB, default = 1'b0 
       Configure Err return to HA one cycle late
     */
    UINT32 mcforcercvend3nnh : 1;
    /* mcforcercvend3nnh - Bits[13:13], RW_LB, default = 1'b0 
       Always force Receive enable to be on
     */
    UINT32 uselateqclkrfrdnnn : 1;
    /* uselateqclkrfrdnnn - Bits[14:14], RW_LB, default = 1'b0 
       Use one Qclk late for iscan read fifo read enable
     */
    UINT32 ign_ptrl_uc : 1;
    /* ign_ptrl_uc - Bits[15:15], RW_LB, default = 1'b0 
       Downgrades patrol scrubbing uncorrectable errors to correactable.
     */
    UINT32 ign_mca_ovrd : 1;
    /* ign_mca_ovrd - Bits[16:16], RW_LB, default = 1'b0 
       When set to 1, disables the MCA regular override mechanism and keeps current 
       logs 
     */
    UINT32 log_sec_err_dev : 1;
    /* log_sec_err_dev - Bits[17:17], RW_LB, default = 1'b0 
       When set to 1, Logs the error device from second codeword in a cacheline access 
       if both codewords are in error. By default, MC logs the first codeword device as 
       the failing device in a cacheline (because there are two code words in each 
       cacheline). 
     */
    UINT32 en_uc_err_log : 1;
    /* en_uc_err_log - Bits[18:18], RW_LB, default = 1'b0 
       Enables logging of uncorrectable read errors to MCA banks. When this field is 
       set to 1 along with en_corr_rd_err_log, MC is in mode-2 
     */
    UINT32 en_corr_err_log : 1;
    /* en_corr_err_log - Bits[19:19], RW_LB, default = 1'b0 
       Enables logging of correctable read errors to MCA banks. When set to 1, this 
       puts MC in mode-1. 
     */
    UINT32 dis_wdb_par_chk : 1;
    /* dis_wdb_par_chk - Bits[20:20], RW_LB, default = 1'b0 
       Disable Write Data Buffer parity checking
     */
    UINT32 dis_spare_dir : 1;
    /* dis_spare_dir - Bits[21:21], RW_LB, default = 1'b0 
       Disable spare device directory storage.
     */
    UINT32 no_over_ce : 1;
    /* no_over_ce - Bits[22:22], RW_LB, default = 1'b0 
       Set to 1 to avoid CE to override CE in MCA banks.
     */
    UINT32 dis_link_link_fail : 1;
    /* dis_link_link_fail - Bits[23:23], RW_LB, default = 1'b0  */
    UINT32 en_rdimm_par_err_log : 1;
    /* en_rdimm_par_err_log - Bits[24:24], RW_LB, default = 1'b0 
       Enable RDIMM parity error logging.
     */
    UINT32 dis_rrd_psn : 1;
    /* dis_rrd_psn - Bits[25:25], RW_LB, default = 1'b0 
       Disable poison detection in the RRD.
     */
    UINT32 dis_verr_ecc_trials : 1;
    /* dis_verr_ecc_trials - Bits[26:26], RW_LB, default = 1'b0  */
    UINT32 dis_link_nb_hw : 1;
    /* dis_link_nb_hw - Bits[27:27], RW_LB, default = 1'b1  */
    UINT32 dis_link_sb_hw : 1;
    /* dis_link_sb_hw - Bits[28:28], RW_LB, default = 1'b0  */
    UINT32 dis_verr_scramsync : 1;
    /* dis_verr_scramsync - Bits[29:29], RW_LB, default = 1'b1  */
    UINT32 dis_link_data_scram : 1;
    /* dis_link_data_scram - Bits[30:30], RW_LB, default = 1'b0  */
    UINT32 dis_link_cmd_scram : 1;
    /* dis_link_cmd_scram - Bits[31:31], RW_LB, default = 1'b0  */
  } Bits;
  UINT32 Data;
} MC0_DP_CHKN_BIT_MCDDC_DP_STRUCT;











/* PLUS1_RANK0_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253720)                                                  */
/*       SKX (0x10253720)                                                     */
/* Register default value:              0x00                                  */
#define PLUS1_RANK0_MCDDC_DP_REG 0x0B011720
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RW_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this rank, for portions of the rank not participating in an ADDDC region. 
                           Set by hardware at the beginning of a sparing flow.
       
                           Hardware will capture the fail device ID of the rank in the 
       FailDevice field upon successful correction from the ECC logic.  
                           After SDDC is enabled HW may not update this field.
                           Independent Channel x4: Valid Range is decimal 0-17 to 
       indicate which device has failed.  
                           Virtual Lockstep x8:    Valid Range is decimal 0-17 to 
       indicate which device has failed. 
                           Virtual Lockstep x4:    Valid Range is decimal 0-35 to 
       indicate which device has failed. 
       
                           Note that when DDDC has been enabled on the non-spare 
       device, and a subsequent failure of the spare device occurs, the value logged 
       here will be equal to the DDDC faildevice.  -- Is this still true for ADDDC? -- 
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} PLUS1_RANK0_MCDDC_DP_STRUCT;


/* PLUS1_RANK1_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253721)                                                  */
/*       SKX (0x10253721)                                                     */
/* Register default value:              0x00                                  */
#define PLUS1_RANK1_MCDDC_DP_REG 0x0B011721
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RW_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this rank, for portions of the rank not participating in an ADDDC region. 
                           Set by hardware at the beginning of a sparing flow.
       
                           Hardware will capture the fail device ID of the rank in the 
       FailDevice field upon successful correction from the ECC logic.  
                           After SDDC is enabled HW may not update this field.
                           Independent Channel x4: Valid Range is decimal 0-17 to 
       indicate which device has failed.  
                           Virtual Lockstep x8:    Valid Range is decimal 0-17 to 
       indicate which device has failed. 
                           Virtual Lockstep x4:    Valid Range is decimal 0-35 to 
       indicate which device has failed. 
       
                           Note that when DDDC has been enabled on the non-spare 
       device, and a subsequent failure of the spare device occurs, the value logged 
       here will be equal to the DDDC faildevice.  -- Is this still true for ADDDC? -- 
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} PLUS1_RANK1_MCDDC_DP_STRUCT;


/* PLUS1_RANK2_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253722)                                                  */
/*       SKX (0x10253722)                                                     */
/* Register default value:              0x00                                  */
#define PLUS1_RANK2_MCDDC_DP_REG 0x0B011722
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RW_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this rank, for portions of the rank not participating in an ADDDC region. 
                           Set by hardware at the beginning of a sparing flow.
       
                           Hardware will capture the fail device ID of the rank in the 
       FailDevice field upon successful correction from the ECC logic.  
                           After SDDC is enabled HW may not update this field.
                           Independent Channel x4: Valid Range is decimal 0-17 to 
       indicate which device has failed.  
                           Virtual Lockstep x8:    Valid Range is decimal 0-17 to 
       indicate which device has failed. 
                           Virtual Lockstep x4:    Valid Range is decimal 0-35 to 
       indicate which device has failed. 
       
                           Note that when DDDC has been enabled on the non-spare 
       device, and a subsequent failure of the spare device occurs, the value logged 
       here will be equal to the DDDC faildevice.  -- Is this still true for ADDDC? -- 
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} PLUS1_RANK2_MCDDC_DP_STRUCT;


/* PLUS1_RANK3_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253723)                                                  */
/*       SKX (0x10253723)                                                     */
/* Register default value:              0x00                                  */
#define PLUS1_RANK3_MCDDC_DP_REG 0x0B011723
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RW_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this rank, for portions of the rank not participating in an ADDDC region. 
                           Set by hardware at the beginning of a sparing flow.
       
                           Hardware will capture the fail device ID of the rank in the 
       FailDevice field upon successful correction from the ECC logic.  
                           After SDDC is enabled HW may not update this field.
                           Independent Channel x4: Valid Range is decimal 0-17 to 
       indicate which device has failed.  
                           Virtual Lockstep x8:    Valid Range is decimal 0-17 to 
       indicate which device has failed. 
                           Virtual Lockstep x4:    Valid Range is decimal 0-35 to 
       indicate which device has failed. 
       
                           Note that when DDDC has been enabled on the non-spare 
       device, and a subsequent failure of the spare device occurs, the value logged 
       here will be equal to the DDDC faildevice.  -- Is this still true for ADDDC? -- 
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} PLUS1_RANK3_MCDDC_DP_STRUCT;


/* PLUS1_RANK4_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253724)                                                  */
/*       SKX (0x10253724)                                                     */
/* Register default value:              0x00                                  */
#define PLUS1_RANK4_MCDDC_DP_REG 0x0B011724
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RW_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this rank, for portions of the rank not participating in an ADDDC region. 
                           Set by hardware at the beginning of a sparing flow.
       
                           Hardware will capture the fail device ID of the rank in the 
       FailDevice field upon successful correction from the ECC logic.  
                           After SDDC is enabled HW may not update this field.
                           Independent Channel x4: Valid Range is decimal 0-17 to 
       indicate which device has failed.  
                           Virtual Lockstep x8:    Valid Range is decimal 0-17 to 
       indicate which device has failed. 
                           Virtual Lockstep x4:    Valid Range is decimal 0-35 to 
       indicate which device has failed. 
       
                           Note that when DDDC has been enabled on the non-spare 
       device, and a subsequent failure of the spare device occurs, the value logged 
       here will be equal to the DDDC faildevice.  -- Is this still true for ADDDC? -- 
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} PLUS1_RANK4_MCDDC_DP_STRUCT;


/* PLUS1_RANK5_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253725)                                                  */
/*       SKX (0x10253725)                                                     */
/* Register default value:              0x00                                  */
#define PLUS1_RANK5_MCDDC_DP_REG 0x0B011725
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RW_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this rank, for portions of the rank not participating in an ADDDC region. 
                           Set by hardware at the beginning of a sparing flow.
       
                           Hardware will capture the fail device ID of the rank in the 
       FailDevice field upon successful correction from the ECC logic.  
                           After SDDC is enabled HW may not update this field.
                           Independent Channel x4: Valid Range is decimal 0-17 to 
       indicate which device has failed.  
                           Virtual Lockstep x8:    Valid Range is decimal 0-17 to 
       indicate which device has failed. 
                           Virtual Lockstep x4:    Valid Range is decimal 0-35 to 
       indicate which device has failed. 
       
                           Note that when DDDC has been enabled on the non-spare 
       device, and a subsequent failure of the spare device occurs, the value logged 
       here will be equal to the DDDC faildevice.  -- Is this still true for ADDDC? -- 
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} PLUS1_RANK5_MCDDC_DP_STRUCT;


/* PLUS1_RANK6_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253726)                                                  */
/*       SKX (0x10253726)                                                     */
/* Register default value:              0x00                                  */
#define PLUS1_RANK6_MCDDC_DP_REG 0x0B011726
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RW_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this rank, for portions of the rank not participating in an ADDDC region. 
                           Set by hardware at the beginning of a sparing flow.
       
                           Hardware will capture the fail device ID of the rank in the 
       FailDevice field upon successful correction from the ECC logic.  
                           After SDDC is enabled HW may not update this field.
                           Independent Channel x4: Valid Range is decimal 0-17 to 
       indicate which device has failed.  
                           Virtual Lockstep x8:    Valid Range is decimal 0-17 to 
       indicate which device has failed. 
                           Virtual Lockstep x4:    Valid Range is decimal 0-35 to 
       indicate which device has failed. 
       
                           Note that when DDDC has been enabled on the non-spare 
       device, and a subsequent failure of the spare device occurs, the value logged 
       here will be equal to the DDDC faildevice.  -- Is this still true for ADDDC? -- 
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} PLUS1_RANK6_MCDDC_DP_STRUCT;


/* PLUS1_RANK7_MCDDC_DP_REG supported on:                                     */
/*       SKX_A0 (0x10253727)                                                  */
/*       SKX (0x10253727)                                                     */
/* Register default value:              0x00                                  */
#define PLUS1_RANK7_MCDDC_DP_REG 0x0B011727
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RW_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this rank, for portions of the rank not participating in an ADDDC region. 
                           Set by hardware at the beginning of a sparing flow.
       
                           Hardware will capture the fail device ID of the rank in the 
       FailDevice field upon successful correction from the ECC logic.  
                           After SDDC is enabled HW may not update this field.
                           Independent Channel x4: Valid Range is decimal 0-17 to 
       indicate which device has failed.  
                           Virtual Lockstep x8:    Valid Range is decimal 0-17 to 
       indicate which device has failed. 
                           Virtual Lockstep x4:    Valid Range is decimal 0-35 to 
       indicate which device has failed. 
       
                           Note that when DDDC has been enabled on the non-spare 
       device, and a subsequent failure of the spare device occurs, the value logged 
       here will be equal to the DDDC faildevice.  -- Is this still true for ADDDC? -- 
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} PLUS1_RANK7_MCDDC_DP_STRUCT;


/* ADDDC_REGION0_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253730)                                                  */
/*       SKX (0x10253730)                                                     */
/* Register default value:              0x00                                  */
#define ADDDC_REGION0_MCDDC_DP_REG 0x0B011730
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each 
 * region using ADDDC. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this adddc region. 
                           Valid range is 0-35, with values >=18 indicating the buddy 
       rank. 
        
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ADDDC_REGION0_MCDDC_DP_STRUCT;


/* ADDDC_REGION1_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253731)                                                  */
/*       SKX (0x10253731)                                                     */
/* Register default value:              0x00                                  */
#define ADDDC_REGION1_MCDDC_DP_REG 0x0B011731
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each 
 * region using ADDDC. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this adddc region. 
                           Valid range is 0-35, with values >=18 indicating the buddy 
       rank. 
        
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ADDDC_REGION1_MCDDC_DP_STRUCT;


/* ADDDC_REGION2_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253732)                                                  */
/*       SKX (0x10253732)                                                     */
/* Register default value:              0x00                                  */
#define ADDDC_REGION2_MCDDC_DP_REG 0x0B011732
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each 
 * region using ADDDC. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this adddc region. 
                           Valid range is 0-35, with values >=18 indicating the buddy 
       rank. 
        
                       
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ADDDC_REGION2_MCDDC_DP_STRUCT;


/* ADDDC_REGION3_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253733)                                                  */
/*       SKX (0x10253733)                                                     */
/* Register default value:              0x00                                  */
#define ADDDC_REGION3_MCDDC_DP_REG 0x0B011733
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the current ECC configuration for each 
 * region using ADDDC. 
 *             
 */
typedef union {
  struct {
    UINT8 faildevice : 6;
    /* faildevice - Bits[5:0], RWS_LB, default = 6'b000000 
       
                           This field specifies the failed device to be mapped out for 
       this adddc region. 
                           Valid range is 0-35, with values >=18 indicating the buddy 
       rank. 
                        
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ADDDC_REGION3_MCDDC_DP_STRUCT;


/* ADDDC_ERR_INJ_MCDDC_DP_REG supported on:                                   */
/*       SKX_A0 (0x10253750)                                                  */
/*       SKX (0x10253750)                                                     */
/* Register default value:              0x00                                  */
#define ADDDC_ERR_INJ_MCDDC_DP_REG 0x0B011750
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * 
 *             This register specifies the error injection to devices in primary , 
 * buddy, or both half of CL . 
 *             
 */
typedef union {
  struct {
    UINT8 primary0 : 1;
    /* primary0 - Bits[0:0], RWS_LB, default = 1'b0 
       
                           This field specifies the error injection to device in 
       primary half of CL for mask0. 
                       
     */
    UINT8 buddy0 : 1;
    /* buddy0 - Bits[1:1], RWS_LB, default = 1'b0 
       
                           This field specifies the error injection to device in buddy 
       half of CL for mask0. 
                       
     */
    UINT8 primary1 : 1;
    /* primary1 - Bits[2:2], RWS_LB, default = 1'b0 
       
                           This field specifies the error injection to device in 
       primary half of CL for mask1. 
                       
     */
    UINT8 buddy1 : 1;
    /* buddy1 - Bits[3:3], RWS_LB, default = 1'b0 
       
                           This field specifies the error injection to device in buddy 
       half of CL for mask1. 
                       
     */
    UINT8 rsvd : 4;
    /* rsvd - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ADDDC_ERR_INJ_MCDDC_DP_STRUCT;


/* RETRY_RD_ERR_LOG_MISC_MCDDC_DP_REG supported on:                           */
/*       SKX (0x40253148)                                                     */
/* Register default value:              0x00000000                            */
#define RETRY_RD_ERR_LOG_MISC_MCDDC_DP_REG 0x0B014148
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Logs info on retried reads.  The contents of this register are valid with 
 * RETRY_RD_ERR_LOG 
 */
typedef union {
  struct {
    UINT32 plus1_loc_syn : 12;
    /* plus1_loc_syn - Bits[11:0], RWS_V, default = 12'h0 
       this register hold the value of plus1 locator syndrome from correction path. the 
       number of plus1 LS bits are the same for all the plus1 modes  
     */
    UINT32 inter_loc_syn : 12;
    /* inter_loc_syn - Bits[23:12], RWS_V, default = 12'h0 
       this register hold the value Intermediate Locator Syndrome from correction path 
       which are only valid when the mode is sddc 1lm. 
                   Only valid iff hard failure effect the IL bits.
                   The syndrome will indicate which bit is the failing one
     */
    UINT32 data_failed_col_num : 6;
    /* data_failed_col_num - Bits[29:24], RWS_V, default = 6'h0 
       this register indicate the failed column number over the data device.
                   columns over data are numbered from 0 to 63. this field is only 
       valid for 1LM ADDDC and when the column correction indication is set 
                   in the retry_rd_err_log register
     */
    UINT32 meta_failed_col_num : 2;
    /* meta_failed_col_num - Bits[31:30], RWS_V, default = 2'h0 
       this register indicate the failed column number over the metadata+locator 
       device. 
                   the valid columns are column 0 and 1. this field is only valid for 
       1LM ADDDC and when the column correction indication is set 
                   in the retry_rd_err_log register
     */
  } Bits;
  UINT32 Data;
} RETRY_RD_ERR_LOG_MISC_MCDDC_DP_STRUCT;


/* LINK_MCA_CTL_MCDDC_DP_REG supported on:                                    */
/*       SKX (0x40253158)                                                     */
/* Register default value:              0x00100000                            */
#define LINK_MCA_CTL_MCDDC_DP_REG 0x0B014158
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * MC bank and signalling control. 
 * err4: Sb Persistent Counter Reached
 */
typedef union {
  struct {
    UINT32 err0_en : 1;
    /* err0_en - Bits[0:0], RW_LB, default = 1'b0 
       Enable CSR logging for Link Fail condition. If set, the error will be logged in 
       the corresponding log field in this register. 
     */
    UINT32 err0_log : 1;
    /* err0_log - Bits[1:1], RW1CS, default = 1'b0 
       Link Fail Error was logged by HW. BIOS clears by writing 0.
     */
    UINT32 err0_cmci : 1;
    /* err0_cmci - Bits[2:2], RW_LB, default = 1'b0 
       Enable/Disable CMCI signaling for Link Fail condition. When disabled, these 
       errors will not be included in the Corrected Error Count. Note: for corrected 
       errors in general, CMCI is only signaled when corr_err_cnt==corr_err_threshold. 
     */
    UINT32 err0_smi : 1;
    /* err0_smi - Bits[3:3], RW_LB, default = 1'b0 
       Enable/Disable SMI signaling each time link fail error occurs.
     */
    UINT32 rsvd_4 : 12;
    UINT32 err4_en : 1;
    /* err4_en - Bits[16:16], RW_LB, default = 1'b0 
       Enable CSR logging. If set, the error will be logged in the corresponding log 
       field in this register. 
     */
    UINT32 err4_log : 1;
    /* err4_log - Bits[17:17], RW1CS, default = 1'b0 
       Error was logged by HW. BIOS clears by writing 0.
     */
    UINT32 err4_cmci : 1;
    /* err4_cmci - Bits[18:18], RW_LB, default = 1'b0 
       Enable/Disable CMCI signaling. When disabled, these errors will not be included 
       in the Corrected Error Count. Note: for corrected errors in general, CMCI is 
       only signaled when corr_err_cnt==corr_err_threshold. 
     */
    UINT32 err4_smi : 1;
    /* err4_smi - Bits[19:19], RW_LB, default = 1'b0 
       Enable/Disable SMI signaling each time error occurs.
     */
    UINT32 ptl_sa_mode : 1;
    /* ptl_sa_mode - Bits[20:20], RW_LBV, default = 1'b1 
       Patrol runs in system address generation mode if patrol is enabled.  If this 
       mode bit is not set, patrol runs in legacy mode if patrol is enabled. 
     */
    UINT32 rsvd : 11;
    /* rsvd - Bits[31:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LINK_MCA_CTL_MCDDC_DP_STRUCT;




/* LINK_CFG_READ_1_MCDDC_DP_REG supported on:                                 */
/*       SKX (0x40253324)                                                     */
/* Register default value:              0x00000000                            */
#define LINK_CFG_READ_1_MCDDC_DP_REG 0x0B014324
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Register does not hold any config read data. Instead it is used for various 
 * defeature bits for MC bug fixes. 
 */
typedef union {
  struct {
    UINT32 read_data : 32;
    /* read_data - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Bits used to enable bug fixes.
               
     */
  } Bits;
  UINT32 Data;
} LINK_CFG_READ_1_MCDDC_DP_STRUCT;


/* LINK_RETRY_SB_ERR_COUNT_MCDDC_DP_REG supported on:                         */
/*       SKX (0x40253400)                                                     */
/* Register default value:              0x00000000                            */
#define LINK_RETRY_SB_ERR_COUNT_MCDDC_DP_REG 0x0B014400
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Count of south bound errors.   
 */
typedef union {
  struct {
    UINT32 transient_err_cnt : 16;
    /* transient_err_cnt - Bits[15:0], RO_V, default = 16'b0000000000000000 
       Count of transient south bound errors.
       Cleared by hardware when the smi_retry_sb_err_limit.sb_err_limit is reached and 
       persistent_err_cnt is incremented. 
       Cleared by hardware when the smi_retry_timer.sb_err_observation_time timer 
       expires. 
     */
    UINT32 persistent_err_cnt : 8;
    /* persistent_err_cnt - Bits[23:16], RO_V, default = 8'b00000000 
       Count of persistent south bound errors. 
       Counter saturates at 0xff.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LINK_RETRY_SB_ERR_COUNT_MCDDC_DP_STRUCT;




/* LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG supported on:                           */
/*       SKX (0x4025340C)                                                     */
/* Register default value:              0x00100000                            */
#define LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG 0x0B01440C
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Thresholds at which transient errors are considered persistent and trigger a 
 * link width change.   
 */
typedef union {
  struct {
    UINT32 rsvd : 16;
    /* rsvd - Bits[15:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sb_err_limit : 15;
    /* sb_err_limit - Bits[30:16], RWS_L, default = 15'b000000000010000 
       Number of transient errors (link_retry_sb_err_count.transient_err_cnt) allowed 
       before the SB error is declared persistent. 
     */
    UINT32 sb_err_enable : 1;
    /* sb_err_enable - Bits[31:31], RWS_L, default = 1'b0 
       Enable NB persistent condition.
     */
  } Bits;
  UINT32 Data;
} LINK_RETRY_ERR_LIMITS_MCDDC_DP_STRUCT;


/* LINK_RETRY_TIMER_MCDDC_DP_REG supported on:                                */
/*       SKX (0x40253410)                                                     */
/* Register default value:              0x80000400                            */
#define LINK_RETRY_TIMER_MCDDC_DP_REG 0x0B014410
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Configuration settings for the timers for Intel SMI 2 Retry
 */
typedef union {
  struct {
    UINT32 link_err_flow_time_unit : 10;
    /* link_err_flow_time_unit - Bits[9:0], RWS_L, default = 10'b0000000000 
       Holds the 'tick' amount (specified in DCLKs) for the error observation window 
       timers. 
     */
    UINT32 tick_timer_en : 1;
    /* tick_timer_en - Bits[10:10], RWS_L, default = 1'b1 
       Enable the tick timer.
     */
    UINT32 sb_err_observation_time : 5;
    /* sb_err_observation_time - Bits[15:11], RWS_L, default = 5'b00000 
       Error observation window (in units oflink_err_flow_time_unit * 
       2^sb_err_observation_time) during which Intel LINK 2 Command and SB Data 
       transient errors are counted. 
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[25:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 link_fail_observation_time : 5;
    /* link_fail_observation_time - Bits[30:26], RWS_L, default = 5'b00000 
       Error observation window (in units of link_err_flow_time_unit * 
       2^link_fail_observation_time) during which link fail events are counted. 
     */
    UINT32 reset_verr_err_cnt : 1;
    /* reset_verr_err_cnt - Bits[31:31], RW_LB, default = 1'b1 
       Hardware resets the SB and NB error transient and persistent counters as well as 
       the link fail 
       counters when this bit changes from a 0 to 1.
     */
  } Bits;
  UINT32 Data;
} LINK_RETRY_TIMER_MCDDC_DP_STRUCT;

/* LINK_LINK_FAIL_MCDDC_DP_REG supported on:                                  */
/*       SKX (0x40253414)                                                     */
/* Register default value:              0x00000100                            */
#define LINK_LINK_FAIL_MCDDC_DP_REG 0x0B014414
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * Configuration settings for the Intel LINK 2 Retry link fail state
 */
typedef union {
  struct {
    UINT32 fail_count : 8;
    /* fail_count - Bits[7:0], RWS_V, default = 8'b00000000 
       Count of persistent southbound errors observed.
     */
    UINT32 fail_threshold : 8;
    /* fail_threshold - Bits[15:8], RWS_L, default = 8'b00000001 
       Threshold of link_link_fail.fail_count for which the link will go to the "Link 
       fail" state. 
       fail_threshold = 0 means the first persistent failure in the "Half Width C - SB" 
       state will cause "Link Fail". 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LINK_LINK_FAIL_MCDDC_DP_STRUCT;


/* LINK_ERR_FSM_STATE_MCDDC_DP_REG supported on:                              */
/*       SKX (0x40253420)                                                     */
/* Register default value:              0x00000000                            */
#define LINK_ERR_FSM_STATE_MCDDC_DP_REG 0x0B014420
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * State of the Intel LINK 2 Error flow FSM.
 */
typedef union {
  struct {
    UINT32 current_state : 4;
    /* current_state - Bits[3:0], RO_V, default = 4'b0000 
       The current state of the Intel LINK 2 Error flow FSM.
     */
    UINT32 rsvd_4 : 12;
    /* rsvd_4 - Bits[15:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 previous_state : 4;
    /* previous_state - Bits[19:16], RO_V, default = 4'b0000 
       The state of the Intel LINK 2 Error flow FSM before the current state.
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LINK_ERR_FSM_STATE_MCDDC_DP_STRUCT;


















/* DIS_CORR_ERR_LOG_MCDDC_DP_REG supported on:                                */
/*       SKX (0x40253704)                                                     */
/* Register default value:              0x00000000                            */
#define DIS_CORR_ERR_LOG_MCDDC_DP_REG 0x0B014704
/* Struct format extracted from XML file SKX\2.10.3.CFG.xml.
 * this register has one bit per rank to disable correctable error 
 * logging/signaling  
 */
typedef union {
  struct {
    UINT32 rank0 : 1;
    /* rank0 - Bits[0:0], RW_LB, default = 1'h0 
       disable correctable error logging/signaling for Rank0
     */
    UINT32 rank1 : 1;
    /* rank1 - Bits[1:1], RW_LB, default = 1'h0 
       disable correctable error logging/signaling for Rank1
     */
    UINT32 rank2 : 1;
    /* rank2 - Bits[2:2], RW_LB, default = 1'h0 
        disable correctable error logging/signaling for Rank2
     */
    UINT32 rank3 : 1;
    /* rank3 - Bits[3:3], RW_LB, default = 1'h0 
       disable correctable error logging/signaling for Rank3
     */
    UINT32 rank4 : 1;
    /* rank4 - Bits[4:4], RW_LB, default = 1'h0 
       disable correctable error logging/signaling for Rank4
     */
    UINT32 rank5 : 1;
    /* rank5 - Bits[5:5], RW_LB, default = 1'h0 
       disable correctable error logging/signaling for Rank5
     */
    UINT32 rank6 : 1;
    /* rank6 - Bits[6:6], RW_LB, default = 1'h0 
       disable correctable error logging/signaling for Rank6
     */
    UINT32 rank7 : 1;
    /* rank7 - Bits[7:7], RW_LB, default = 1'h0 
       disable correctable error logging/signaling for Rank7
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIS_CORR_ERR_LOG_MCDDC_DP_STRUCT;










#endif /* MCDDC_DP_h */
