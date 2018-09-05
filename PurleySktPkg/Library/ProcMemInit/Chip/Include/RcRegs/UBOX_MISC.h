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

#ifndef UBOX_MISC_h
#define UBOX_MISC_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* UBOX_MISC_DEV 8                                                            */
/* UBOX_MISC_FUN 2                                                            */

/* VID_UBOX_MISC_REG supported on:                                            */
/*       SKX_A0 (0x20042000)                                                  */
/*       SKX (0x20042000)                                                     */
/* Register default value:              0x8086                                */
#define VID_UBOX_MISC_REG 0x13022000
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} VID_UBOX_MISC_STRUCT;


/* DID_UBOX_MISC_REG supported on:                                            */
/*       SKX_A0 (0x20042002)                                                  */
/*       SKX (0x20042002)                                                     */
/* Register default value:              0x2016                                */
#define DID_UBOX_MISC_REG 0x13022002
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2016 
        
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
} DID_UBOX_MISC_STRUCT;


/* PCICMD_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x20042004)                                                  */
/*       SKX (0x20042004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_UBOX_MISC_REG 0x13022004
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} PCICMD_UBOX_MISC_STRUCT;


/* PCISTS_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x20042006)                                                  */
/*       SKX (0x20042006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_UBOX_MISC_REG 0x13022006
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} PCISTS_UBOX_MISC_STRUCT;


/* RID_UBOX_MISC_REG supported on:                                            */
/*       SKX_A0 (0x10042008)                                                  */
/*       SKX (0x10042008)                                                     */
/* Register default value:              0x00                                  */
#define RID_UBOX_MISC_REG 0x13021008
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} RID_UBOX_MISC_STRUCT;


/* CCR_N0_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x10042009)                                                  */
/*       SKX (0x10042009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_UBOX_MISC_REG 0x13021009
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_UBOX_MISC_STRUCT;


/* CCR_N1_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x2004200A)                                                  */
/*       SKX (0x2004200A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_UBOX_MISC_REG 0x1302200A
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} CCR_N1_UBOX_MISC_STRUCT;


/* CLSR_UBOX_MISC_REG supported on:                                           */
/*       SKX_A0 (0x1004200C)                                                  */
/*       SKX (0x1004200C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_UBOX_MISC_REG 0x1302100C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} CLSR_UBOX_MISC_STRUCT;


/* PLAT_UBOX_MISC_REG supported on:                                           */
/*       SKX_A0 (0x1004200D)                                                  */
/*       SKX (0x1004200D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_UBOX_MISC_REG 0x1302100D
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} PLAT_UBOX_MISC_STRUCT;


/* HDR_UBOX_MISC_REG supported on:                                            */
/*       SKX_A0 (0x1004200E)                                                  */
/*       SKX (0x1004200E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_UBOX_MISC_REG 0x1302100E
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} HDR_UBOX_MISC_STRUCT;


/* BIST_UBOX_MISC_REG supported on:                                           */
/*       SKX_A0 (0x1004200F)                                                  */
/*       SKX (0x1004200F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_UBOX_MISC_REG 0x1302100F
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} BIST_UBOX_MISC_STRUCT;


/* SVID_UBOX_MISC_REG supported on:                                           */
/*       SKX_A0 (0x2004202C)                                                  */
/*       SKX (0x2004202C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_UBOX_MISC_REG 0x1302202C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_UBOX_MISC_STRUCT;


/* SDID_UBOX_MISC_REG supported on:                                           */
/*       SKX_A0 (0x2004202E)                                                  */
/*       SKX (0x2004202E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_UBOX_MISC_REG 0x1302202E
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_UBOX_MISC_STRUCT;


/* CAPPTR_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x10042034)                                                  */
/*       SKX (0x10042034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_UBOX_MISC_REG 0x13021034
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} CAPPTR_UBOX_MISC_STRUCT;


/* INTL_UBOX_MISC_REG supported on:                                           */
/*       SKX_A0 (0x1004203C)                                                  */
/*       SKX (0x1004203C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_UBOX_MISC_REG 0x1302103C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} INTL_UBOX_MISC_STRUCT;


/* INTPIN_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x1004203D)                                                  */
/*       SKX (0x1004203D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_UBOX_MISC_REG 0x1302103D
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} INTPIN_UBOX_MISC_STRUCT;


/* MINGNT_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x1004203E)                                                  */
/*       SKX (0x1004203E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_UBOX_MISC_REG 0x1302103E
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} MINGNT_UBOX_MISC_STRUCT;


/* MAXLAT_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x1004203F)                                                  */
/*       SKX (0x1004203F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_UBOX_MISC_REG 0x1302103F
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
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
} MAXLAT_UBOX_MISC_STRUCT;


/* PXPCAP_UBOX_MISC_REG supported on:                                         */
/*       SKX_A0 (0x40042040)                                                  */
/*       SKX (0x40042040)                                                     */
/* Register default value:              0x00920010                            */
#define PXPCAP_UBOX_MISC_REG 0x13024040
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * PCI Express Capability
 */
typedef union {
  struct {
    UINT32 capability_id : 8;
    /* capability_id - Bits[7:0], RO, default = 8'h10 
       Identifies the PCI Express capability assigned by PCI-SIG.
     */
    UINT32 next_ptr : 8;
    /* next_ptr - Bits[15:8], RO, default = 8'h00 
       Pointer to the next capability. Set to 0 to indicate there are no more 
       capability structures. 
     */
    UINT32 capability_version : 4;
    /* capability_version - Bits[19:16], RO, default = 4'h2 
       PCI Express Capability is Compliant with Version 2.0 of the PCI Express Spec.
     */
    UINT32 device_port_type : 4;
    /* device_port_type - Bits[23:20], RO, default = 4'h9 
       Device type is Root Complex Integrated Endpoint
     */
    UINT32 slot_implemented : 1;
    /* slot_implemented - Bits[24:24], RO, default = 1'b0 
       Not valid for PCIe integrated Endpoints
     */
    UINT32 interrupt_message_number : 5;
    /* interrupt_message_number - Bits[29:25], RO, default = 5'b00000 
       Not valid for this device, since the device does not generate interrupts
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PXPCAP_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD7_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x4004207C)                                                  */
/*       SKX (0x4004207C)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD7_UBOX_MISC_REG 0x1302407C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD7_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD8_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x40042080)                                                  */
/*       SKX (0x40042080)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD8_UBOX_MISC_REG 0x13024080
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD8_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD9_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x40042084)                                                  */
/*       SKX (0x40042084)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD9_UBOX_MISC_REG 0x13024084
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD9_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG supported on:                      */
/*       SKX_A0 (0x40042088)                                                  */
/*       SKX (0x40042088)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG 0x13024088
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG supported on:                      */
/*       SKX_A0 (0x4004208C)                                                  */
/*       SKX (0x4004208C)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG 0x1302408C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD12_UBOX_MISC_REG supported on:                      */
/*       SKX_A0 (0x40042090)                                                  */
/*       SKX (0x40042090)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD12_UBOX_MISC_REG 0x13024090
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD12_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG supported on:                      */
/*       SKX_A0 (0x40042094)                                                  */
/*       SKX (0x40042094)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG 0x13024094
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_REG supported on:                      */
/*       SKX_A0 (0x40042098)                                                  */
/*       SKX (0x40042098)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_REG 0x13024098
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG supported on:                      */
/*       SKX_A0 (0x4004209C)                                                  */
/*       SKX (0x4004209C)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG 0x1302409C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_STRUCT;


/* BIOSSCRATCHPAD0_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x400420A0)                                                  */
/*       SKX (0x400420A0)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSCRATCHPAD0_UBOX_MISC_REG 0x130240A0
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Set by BIOS, sticky across RESET
     */
  } Bits;
  UINT32 Data;
} BIOSSCRATCHPAD0_UBOX_MISC_STRUCT;


/* BIOSSCRATCHPAD1_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x400420A4)                                                  */
/*       SKX (0x400420A4)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSCRATCHPAD1_UBOX_MISC_REG 0x130240A4
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Set by BIOS, sticky across RESET
     */
  } Bits;
  UINT32 Data;
} BIOSSCRATCHPAD1_UBOX_MISC_STRUCT;


/* BIOSSCRATCHPAD2_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x400420A8)                                                  */
/*       SKX (0x400420A8)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSCRATCHPAD2_UBOX_MISC_REG 0x130240A8
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Set by BIOS, sticky across RESET
     */
  } Bits;
  UINT32 Data;
} BIOSSCRATCHPAD2_UBOX_MISC_STRUCT;


/* BIOSSCRATCHPAD3_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x400420AC)                                                  */
/*       SKX (0x400420AC)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSCRATCHPAD3_UBOX_MISC_REG 0x130240AC
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Set by BIOS, sticky across RESET
     */
  } Bits;
  UINT32 Data;
} BIOSSCRATCHPAD3_UBOX_MISC_STRUCT;


/* BIOSSCRATCHPAD4_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x400420B0)                                                  */
/*       SKX (0x400420B0)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSCRATCHPAD4_UBOX_MISC_REG 0x130240B0
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Set by BIOS, sticky across RESET
     */
  } Bits;
  UINT32 Data;
} BIOSSCRATCHPAD4_UBOX_MISC_STRUCT;


/* BIOSSCRATCHPAD5_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x400420B4)                                                  */
/*       SKX (0x400420B4)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSCRATCHPAD5_UBOX_MISC_REG 0x130240B4
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Set by BIOS, sticky across RESET
     */
  } Bits;
  UINT32 Data;
} BIOSSCRATCHPAD5_UBOX_MISC_STRUCT;


/* BIOSSCRATCHPAD6_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x400420B8)                                                  */
/*       SKX (0x400420B8)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSCRATCHPAD6_UBOX_MISC_REG 0x130240B8
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Set by BIOS, sticky across RESET
     */
  } Bits;
  UINT32 Data;
} BIOSSCRATCHPAD6_UBOX_MISC_STRUCT;


/* BIOSSCRATCHPAD7_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x400420BC)                                                  */
/*       SKX (0x400420BC)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSCRATCHPAD7_UBOX_MISC_REG 0x130240BC
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Set by BIOS, sticky across RESET
     */
  } Bits;
  UINT32 Data;
} BIOSSCRATCHPAD7_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD0_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x400420C0)                                                  */
/*       SKX (0x400420C0)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD0_UBOX_MISC_REG 0x130240C0
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD0_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x400420C4)                                                  */
/*       SKX (0x400420C4)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_REG 0x130240C4
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x400420C8)                                                  */
/*       SKX (0x400420C8)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG 0x130240C8
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_STRUCT;


/* CPUBUSNO_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x400420CC)                                                  */
/*       SKX (0x400420CC)                                                     */
/* Register default value:              0x03020100                            */
#define CPUBUSNO_UBOX_MISC_REG 0x130240CC
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Bus Number Configuration
 */
typedef union {
  struct {
    UINT32 cpubusno0 : 8;
    /* cpubusno0 - Bits[7:0], RW_LB, default = 8'b00000000 
       Bus Number 0
     */
    UINT32 cpubusno1 : 8;
    /* cpubusno1 - Bits[15:8], RW_LB, default = 8'b00000001 
       Bus Number 1
     */
    UINT32 cpubusno2 : 8;
    /* cpubusno2 - Bits[23:16], RW_LB, default = 8'b00000010 
       Bus Number 2
     */
    UINT32 cpubusno3 : 8;
    /* cpubusno3 - Bits[31:24], RW_LB, default = 8'b00000011 
       Bus Number 3
     */
  } Bits;
  UINT32 Data;
} CPUBUSNO_UBOX_MISC_STRUCT;


/* CPUBUSNO1_UBOX_MISC_REG supported on:                                      */
/*       SKX_A0 (0x400420D0)                                                  */
/*       SKX (0x400420D0)                                                     */
/* Register default value:              0x00000504                            */
#define CPUBUSNO1_UBOX_MISC_REG 0x130240D0
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Bus Number Configuration 1
 */
typedef union {
  struct {
    UINT32 cpubusno4 : 8;
    /* cpubusno4 - Bits[7:0], RW_LB, default = 8'b00000100 
       Bus Number 4
     */
    UINT32 cpubusno5 : 8;
    /* cpubusno5 - Bits[15:8], RW_LB, default = 8'b00000101 
       Bus Number 5
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPUBUSNO1_UBOX_MISC_STRUCT;


/* CPUBUSNO_VALID_UBOX_MISC_REG supported on:                                 */
/*       SKX_A0 (0x400420D4)                                                  */
/*       SKX (0x400420D4)                                                     */
/* Register default value:              0x00000000                            */
#define CPUBUSNO_VALID_UBOX_MISC_REG 0x130240D4
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Bus Number Configuration Valid
 */
typedef union {
  struct {
    UINT32 rsvd : 31;
    /* rsvd - Bits[30:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 busno_programmed : 1;
    /* busno_programmed - Bits[31:31], RW_LB, default = 1'b0 
       Set by BIOS when the CSR is written with valid bus numbers. 
       or not
     */
  } Bits;
  UINT32 Data;
} CPUBUSNO_VALID_UBOX_MISC_STRUCT;


/* SMICTRL_UBOX_MISC_REG supported on:                                        */
/*       SKX_A0 (0x400420D8)                                                  */
/*       SKX (0x400420D8)                                                     */
/* Register default value:              0x04000000                            */
#define SMICTRL_UBOX_MISC_REG 0x130240D8
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * SMI generation control
 */
typedef union {
  struct {
    UINT32 threshold : 20;
    /* threshold - Bits[19:0], RW_LB, default = 20'b00000000000000000000 
       This is the  countdown that happens in the hardware before an SMI is generated 
       due to a UMC 
     */
    UINT32 rsvd_20 : 4;
    /* rsvd_20 - Bits[23:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 umcsmien : 1;
    /* umcsmien - Bits[24:24], RW_LB, default = 1'b0 
       This is the enable bit that enables SMI generation due to a UMC
       1 -> Generate SMI after the threshold counter expires.
       0 -> Disable generation of SMI
     */
    UINT32 smidis : 1;
    /* smidis - Bits[25:25], RW_LB, default = 1'b0 
       Disable generation of SMI
     */
    UINT32 smidis2 : 1;
    /* smidis2 - Bits[26:26], RW_LB, default = 1'b1 
       Disable generation of SMI forlocktimeout, cfg write mis-align access, and cfg 
       read mis-aslign access. 
     */
    UINT32 smidis3 : 1;
    /* smidis3 - Bits[27:27], RW_LB, default = 1'b0 
       Disable Generation of SMI from message channel
     */
    UINT32 smidis4 : 1;
    /* smidis4 - Bits[28:28], RW_LB, default = 1'b0 
       Disable Generation of SMI from CSMI from MsgCh
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMICTRL_UBOX_MISC_STRUCT;


/* MMCFG_RULE_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400420DC)                                                  */
/*       SKX (0x400420DC)                                                     */
/* Register default value:              0x00000000                            */
#define MMCFG_RULE_UBOX_MISC_REG 0x130240DC
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Use addr[27:26] to enforce maximum bus numbers
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this MMCFG rule.
     */
    UINT32 length : 2;
    /* length - Bits[2:1], RW_LB, default = 2'b00 
       
         This two bit field masks address 27:26 (BUS) to enforce MaxBusNumber as 
       following encodings; 
       
           2'b10 : MaxBusNumber = 63 (ie. 64MB MMCFG range)
           2'b01 : MaxBusNumber = 127 (ie 128MB MMCFG range)
           2'b00 : MaxBusNunber = 255 (ie >= 256MB MMCFG range)
       
        Minimum granularity of MMCFG range will be 64MB.
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MMCFG_RULE_UBOX_MISC_STRUCT;






/* BIOSNONSTICKYSCRATCHPAD3_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x400420E8)                                                  */
/*       SKX (0x400420E8)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD3_UBOX_MISC_REG 0x130240E8
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD3_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD4_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x400420EC)                                                  */
/*       SKX (0x400420EC)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD4_UBOX_MISC_REG 0x130240EC
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD4_UBOX_MISC_STRUCT;




/* BIOSNONSTICKYSCRATCHPAD5_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x400420F4)                                                  */
/*       SKX (0x400420F4)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD5_UBOX_MISC_REG 0x130240F4
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD5_UBOX_MISC_STRUCT;


/* BIOSNONSTICKYSCRATCHPAD6_UBOX_MISC_REG supported on:                       */
/*       SKX_A0 (0x400420F8)                                                  */
/*       SKX (0x400420F8)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSNONSTICKYSCRATCHPAD6_UBOX_MISC_REG 0x130240F8
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * BIOS Scratchpad register
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       BIOS Scratchpad register
     */
  } Bits;
  UINT32 Data;
} BIOSNONSTICKYSCRATCHPAD6_UBOX_MISC_STRUCT;


/* PXPENHCAP_UBOX_MISC_REG supported on:                                      */
/*       SKX_A0 (0x40042100)                                                  */
/*       SKX (0x40042100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_UBOX_MISC_REG 0x13024100
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * PCI Express Enhanced Capability
 */
typedef union {
  struct {
    UINT32 capability_id : 16;
    /* capability_id - Bits[15:0], RO, default = 16'h0000 
       There is no capability at this location
     */
    UINT32 capability_version : 4;
    /* capability_version - Bits[19:16], RO, default = 4'h0 
       There is no capability at this location.
     */
    UINT32 next_capability_offset : 12;
    /* next_capability_offset - Bits[31:20], RO, default = 12'h000 
       Pointer to the next capability in the enhanced configuration space. Set to 0 to 
       indicate there are no more capability structures. 
     */
  } Bits;
  UINT32 Data;
} PXPENHCAP_UBOX_MISC_STRUCT;


/* DEVHIDE0_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x40042104)                                                  */
/*       SKX (0x40042104)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE0_UBOX_MISC_REG 0x13024104
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 0
 */
typedef union {
  struct {
    UINT32 disfn : 9;
    /* disfn - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 23;
    /* disfn1 - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 9 - 31 are changed to RO because 9 - 15 
       are unused and 16 - 31 are PCH 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE0_UBOX_MISC_STRUCT;


/* DEVHIDE1_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x40042108)                                                  */
/*       SKX (0x40042108)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE1_UBOX_MISC_REG 0x13024108
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 0
 */
typedef union {
  struct {
    UINT32 disfn : 9;
    /* disfn - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 23;
    /* disfn1 - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 9 - 31 are changed to RO because 9 - 15 
       are unused and 16 - 31 are PCH 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE1_UBOX_MISC_STRUCT;


/* DEVHIDE2_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x4004210C)                                                  */
/*       SKX (0x4004210C)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE2_UBOX_MISC_REG 0x1302410C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 0
 */
typedef union {
  struct {
    UINT32 disfn : 9;
    /* disfn - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 23;
    /* disfn1 - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 9 - 31 are changed to RO because 9 - 15 
       are unused and 16 - 31 are PCH 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE2_UBOX_MISC_STRUCT;


/* DEVHIDE3_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x40042110)                                                  */
/*       SKX (0x40042110)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE3_UBOX_MISC_REG 0x13024110
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 0
 */
typedef union {
  struct {
    UINT32 disfn : 9;
    /* disfn - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 23;
    /* disfn1 - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 9 - 31 are changed to RO because 9 - 15 
       are unused and 16 - 31 are PCH 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE3_UBOX_MISC_STRUCT;


/* DEVHIDE4_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x40042114)                                                  */
/*       SKX (0x40042114)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE4_UBOX_MISC_REG 0x13024114
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 0
 */
typedef union {
  struct {
    UINT32 disfn : 9;
    /* disfn - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 23;
    /* disfn1 - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 9 - 31 are changed to RO because 9 - 15 
       are unused and 16 - 31 are PCH 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE4_UBOX_MISC_STRUCT;


/* DEVHIDE5_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x40042118)                                                  */
/*       SKX (0x40042118)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE5_UBOX_MISC_REG 0x13024118
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 0
 */
typedef union {
  struct {
    UINT32 disfn : 9;
    /* disfn - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 23;
    /* disfn1 - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 9 - 31 are changed to RO because 9 - 15 
       are unused and 16 - 31 are PCH 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE5_UBOX_MISC_STRUCT;


/* DEVHIDE6_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x4004211C)                                                  */
/*       SKX (0x4004211C)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE6_UBOX_MISC_REG 0x1302411C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 0
 */
typedef union {
  struct {
    UINT32 disfn : 9;
    /* disfn - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 23;
    /* disfn1 - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 9 - 31 are changed to RO because 9 - 15 
       are unused and 16 - 31 are PCH 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE6_UBOX_MISC_STRUCT;


/* DEVHIDE7_UBOX_MISC_REG supported on:                                       */
/*       SKX_A0 (0x40042120)                                                  */
/*       SKX (0x40042120)                                                     */
/* Register default value:              0xFFFFFE00                            */
#define DEVHIDE7_UBOX_MISC_REG 0x13024120
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 0
 */
typedef union {
  struct {
    UINT32 disfn : 9;
    /* disfn - Bits[8:0], RW_LB, default = 9'b000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 23;
    /* disfn1 - Bits[31:9], RO, default = 23'b11111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 9 - 31 are changed to RO because 9 - 15 
       are unused and 16 - 31 are PCH 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE7_UBOX_MISC_STRUCT;


/* DEVHIDE0_1_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042124)                                                  */
/*       SKX (0x40042124)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE0_1_UBOX_MISC_REG 0x13024124
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 1
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE0_1_UBOX_MISC_STRUCT;


/* DEVHIDE1_1_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042128)                                                  */
/*       SKX (0x40042128)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE1_1_UBOX_MISC_REG 0x13024128
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 1
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE1_1_UBOX_MISC_STRUCT;


/* DEVHIDE2_1_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x4004212C)                                                  */
/*       SKX (0x4004212C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE2_1_UBOX_MISC_REG 0x1302412C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 1
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE2_1_UBOX_MISC_STRUCT;


/* DEVHIDE3_1_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042130)                                                  */
/*       SKX (0x40042130)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE3_1_UBOX_MISC_REG 0x13024130
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 1
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE3_1_UBOX_MISC_STRUCT;


/* DEVHIDE4_1_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042134)                                                  */
/*       SKX (0x40042134)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE4_1_UBOX_MISC_REG 0x13024134
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 1
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE4_1_UBOX_MISC_STRUCT;


/* DEVHIDE5_1_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042138)                                                  */
/*       SKX (0x40042138)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE5_1_UBOX_MISC_REG 0x13024138
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 1
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE5_1_UBOX_MISC_STRUCT;


/* DEVHIDE6_1_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x4004213C)                                                  */
/*       SKX (0x4004213C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE6_1_UBOX_MISC_REG 0x1302413C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 1
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE6_1_UBOX_MISC_STRUCT;


/* DEVHIDE7_1_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042140)                                                  */
/*       SKX (0x40042140)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE7_1_UBOX_MISC_REG 0x13024140
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 1
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE7_1_UBOX_MISC_STRUCT;


/* DEVHIDE0_2_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042144)                                                  */
/*       SKX (0x40042144)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE0_2_UBOX_MISC_REG 0x13024144
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 2
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE0_2_UBOX_MISC_STRUCT;


/* DEVHIDE1_2_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042148)                                                  */
/*       SKX (0x40042148)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE1_2_UBOX_MISC_REG 0x13024148
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 2
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE1_2_UBOX_MISC_STRUCT;


/* DEVHIDE2_2_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x4004214C)                                                  */
/*       SKX (0x4004214C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE2_2_UBOX_MISC_REG 0x1302414C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 2
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE2_2_UBOX_MISC_STRUCT;


/* DEVHIDE3_2_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042150)                                                  */
/*       SKX (0x40042150)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE3_2_UBOX_MISC_REG 0x13024150
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 2
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE3_2_UBOX_MISC_STRUCT;


/* DEVHIDE4_2_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042154)                                                  */
/*       SKX (0x40042154)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE4_2_UBOX_MISC_REG 0x13024154
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 2
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE4_2_UBOX_MISC_STRUCT;


/* DEVHIDE5_2_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042158)                                                  */
/*       SKX (0x40042158)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE5_2_UBOX_MISC_REG 0x13024158
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 2
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE5_2_UBOX_MISC_STRUCT;


/* DEVHIDE6_2_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x4004215C)                                                  */
/*       SKX (0x4004215C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE6_2_UBOX_MISC_REG 0x1302415C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 2
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE6_2_UBOX_MISC_STRUCT;


/* DEVHIDE7_2_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042160)                                                  */
/*       SKX (0x40042160)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE7_2_UBOX_MISC_REG 0x13024160
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 2
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE7_2_UBOX_MISC_STRUCT;


/* DEVHIDE0_3_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042164)                                                  */
/*       SKX (0x40042164)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE0_3_UBOX_MISC_REG 0x13024164
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 3
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE0_3_UBOX_MISC_STRUCT;


/* DEVHIDE1_3_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042168)                                                  */
/*       SKX (0x40042168)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE1_3_UBOX_MISC_REG 0x13024168
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 3
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE1_3_UBOX_MISC_STRUCT;


/* DEVHIDE2_3_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x4004216C)                                                  */
/*       SKX (0x4004216C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE2_3_UBOX_MISC_REG 0x1302416C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 3
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE2_3_UBOX_MISC_STRUCT;


/* DEVHIDE3_3_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042170)                                                  */
/*       SKX (0x40042170)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE3_3_UBOX_MISC_REG 0x13024170
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 3
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE3_3_UBOX_MISC_STRUCT;


/* DEVHIDE4_3_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042174)                                                  */
/*       SKX (0x40042174)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE4_3_UBOX_MISC_REG 0x13024174
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 3
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE4_3_UBOX_MISC_STRUCT;


/* DEVHIDE5_3_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042178)                                                  */
/*       SKX (0x40042178)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE5_3_UBOX_MISC_REG 0x13024178
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 3
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE5_3_UBOX_MISC_STRUCT;


/* DEVHIDE6_3_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x4004217C)                                                  */
/*       SKX (0x4004217C)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE6_3_UBOX_MISC_REG 0x1302417C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 3
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE6_3_UBOX_MISC_STRUCT;


/* DEVHIDE7_3_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042180)                                                  */
/*       SKX (0x40042180)                                                     */
/* Register default value:              0x00000000                            */
#define DEVHIDE7_3_UBOX_MISC_REG 0x13024180
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 3
 */
typedef union {
  struct {
    UINT32 disfn : 32;
    /* disfn - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE7_3_UBOX_MISC_STRUCT;


/* DEVHIDE0_4_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042184)                                                  */
/*       SKX (0x40042184)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE0_4_UBOX_MISC_REG 0x13024184
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 4
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE0_4_UBOX_MISC_STRUCT;


/* DEVHIDE1_4_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042188)                                                  */
/*       SKX (0x40042188)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE1_4_UBOX_MISC_REG 0x13024188
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 4
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE1_4_UBOX_MISC_STRUCT;


/* DEVHIDE2_4_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x4004218C)                                                  */
/*       SKX (0x4004218C)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE2_4_UBOX_MISC_REG 0x1302418C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 4
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE2_4_UBOX_MISC_STRUCT;


/* DEVHIDE3_4_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042190)                                                  */
/*       SKX (0x40042190)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE3_4_UBOX_MISC_REG 0x13024190
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 4
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE3_4_UBOX_MISC_STRUCT;


/* DEVHIDE4_4_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042194)                                                  */
/*       SKX (0x40042194)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE4_4_UBOX_MISC_REG 0x13024194
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 4
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE4_4_UBOX_MISC_STRUCT;


/* DEVHIDE5_4_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x40042198)                                                  */
/*       SKX (0x40042198)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE5_4_UBOX_MISC_REG 0x13024198
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 4
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE5_4_UBOX_MISC_STRUCT;


/* DEVHIDE6_4_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x4004219C)                                                  */
/*       SKX (0x4004219C)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE6_4_UBOX_MISC_REG 0x1302419C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 4
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE6_4_UBOX_MISC_STRUCT;


/* DEVHIDE7_4_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421A0)                                                  */
/*       SKX (0x400421A0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE7_4_UBOX_MISC_REG 0x130241A0
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 4
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE7_4_UBOX_MISC_STRUCT;


/* DEVHIDE0_5_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421A4)                                                  */
/*       SKX (0x400421A4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE0_5_UBOX_MISC_REG 0x130241A4
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 5
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
               t in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE0_5_UBOX_MISC_STRUCT;


/* DEVHIDE1_5_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421A8)                                                  */
/*       SKX (0x400421A8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE1_5_UBOX_MISC_REG 0x130241A8
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 5
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE1_5_UBOX_MISC_STRUCT;


/* DEVHIDE2_5_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421AC)                                                  */
/*       SKX (0x400421AC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE2_5_UBOX_MISC_REG 0x130241AC
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 5
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE2_5_UBOX_MISC_STRUCT;


/* DEVHIDE3_5_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421B0)                                                  */
/*       SKX (0x400421B0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE3_5_UBOX_MISC_REG 0x130241B0
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 5
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE3_5_UBOX_MISC_STRUCT;


/* DEVHIDE4_5_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421B4)                                                  */
/*       SKX (0x400421B4)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE4_5_UBOX_MISC_REG 0x130241B4
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 5
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE4_5_UBOX_MISC_STRUCT;


/* DEVHIDE5_5_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421B8)                                                  */
/*       SKX (0x400421B8)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE5_5_UBOX_MISC_REG 0x130241B8
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 5
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE5_5_UBOX_MISC_STRUCT;


/* DEVHIDE6_5_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421BC)                                                  */
/*       SKX (0x400421BC)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE6_5_UBOX_MISC_REG 0x130241BC
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 5
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE6_5_UBOX_MISC_STRUCT;


/* DEVHIDE7_5_UBOX_MISC_REG supported on:                                     */
/*       SKX_A0 (0x400421C0)                                                  */
/*       SKX (0x400421C0)                                                     */
/* Register default value:              0xFFFFFF00                            */
#define DEVHIDE7_5_UBOX_MISC_REG 0x130241C0
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Device Hide Register in CSR space for Bus 5
 */
typedef union {
  struct {
    UINT32 disfn : 8;
    /* disfn - Bits[7:0], RW_LB, default = 8'b00000000 
       Disable Function(DisFn):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. 
     */
    UINT32 disfn1 : 24;
    /* disfn1 - Bits[31:8], RO, default = 24'b111111111111111111111111 
       Disable Function 1 (DisFn1):
       A bit set in this register implies that the appropriate device function is not 
       enabled. For instance, if bit 5 is set in DEVHIDEuCodeCtrl4, then it means that 
       in device 5, function 4 is disabled. Bit 8 - 31 are changed to RO because 8 - 15 
       are unused and 16 - 31 are MCP 
     */
  } Bits;
  UINT32 Data;
} DEVHIDE7_5_UBOX_MISC_STRUCT;


/* LOCALSEMAPHOREPEND0_UBOX_MISC_REG supported on:                            */
/*       SKX_A0 (0x40042200)                                                  */
/*       SKX (0x40042200)                                                     */
/* Register default value:              0x00000000                            */
#define LOCALSEMAPHOREPEND0_UBOX_MISC_REG 0x13024200
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Local semaphore0 pending bits used for debug.
 */
typedef union {
  struct {
    UINT32 pendingrequest : 32;
    /* pendingrequest - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Indicates the threads that have a pending request for Semaphore ownership for 
       cores 15:0. 
        bit 'i' in the field indicate a pending request for 2*CID+TID   (C0T0, C0T1, 
       C1T0,...C15T1) 
     */
  } Bits;
  UINT32 Data;
} LOCALSEMAPHOREPEND0_UBOX_MISC_STRUCT;


/* LOCALSEMAPHOREPEND0_CFG_2_UBOX_MISC_REG supported on:                      */
/*       SKX_A0 (0x40042204)                                                  */
/*       SKX (0x40042204)                                                     */
/* Register default value:              0x00000000                            */
#define LOCALSEMAPHOREPEND0_CFG_2_UBOX_MISC_REG 0x13024204
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Local semaphore0 pending bits used for debug.
 */
typedef union {
  struct {
    UINT32 pendingrequest : 32;
    /* pendingrequest - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Indicates the threads that have a pending request for Semaphore ownership for 
       cores 31:16. 
        bit 'i' in the field indicate a pending request for 2*CID+TID   (C16T0, C16T1, 
       C17T0,...C31T1) 
     */
  } Bits;
  UINT32 Data;
} LOCALSEMAPHOREPEND0_CFG_2_UBOX_MISC_STRUCT;


/* LOCALSEMAPHORE0_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x40042208)                                                  */
/*       SKX (0x40042208)                                                     */
/* Register default value:              0x00000000                            */
#define LOCALSEMAPHORE0_UBOX_MISC_REG 0x13024208
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * unCore Semaphore register is a resource shared by all threads even though it has 
 * access and view for each one of the threads. Each one of the fields is 
 * identified to  
 *  be a shared or a dedicated element.
 */
typedef union {
  struct {
    UINT32 aqu_override : 1;
    /* aqu_override - Bits[0:0], WO, default = 1'b0 
       The bit has different meanings for read and write.
        Write - Override acquisition :
         0 : no effect
         1 : Override. Take ownership of semaphore ignoring any other setting
             or requests.
       Note that this bit has no stat directly associated with it. There are different 
        operations associated with the read and write operations.
     */
    UINT32 bus_aqu_release : 1;
    /* bus_aqu_release - Bits[1:1], RW_V, default = 1'b0 
       Read - Bus status :
         0 : the semaphore s currently free.
         1 : the semaphore is currently busy by one of the threads.
        Write - Acquire request :
         0 : Release the ownership/request of the semaphore. It cause the pending bit 
       for the 
             thread to be cleared.
         1 : Request the ownership of the semaphore. It cause the pending bit for the 
       thread 
              to be set. 
              
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 currentthread : 1;
    /* currentthread - Bits[4:4], RO_V, default = 1'b0 
       This field indicates the TID of the thread that currently owns the semaphore.
        When the semaphore is free (Busy==0), this bit is undefined.
         0 : Thread 0 
         1 : Thread 1 
         
     */
    UINT32 currentcore : 6;
    /* currentcore - Bits[10:5], RO_V, default = 6'b000000 
       This field indicates the CID of the thread that currently owns the semaphore.
        When the semaphore is free (Busy==0), this bit is undefined.
         'n' - Core number
     */
    UINT32 rsvd_11 : 2;
    /* rsvd_11 - Bits[12:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 requesterthread : 1;
    /* requesterthread - Bits[13:13], WO, default = 1'b0 
       The thread id of the requester
     */
    UINT32 requestercore : 6;
    /* requestercore - Bits[19:14], WO, default = 6'b000000 
       Core ID of the requesting core
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LOCALSEMAPHORE0_UBOX_MISC_STRUCT;


/* LOCALSEMAPHORE1_UBOX_MISC_REG supported on:                                */
/*       SKX_A0 (0x4004220C)                                                  */
/*       SKX (0x4004220C)                                                     */
/* Register default value:              0x00000000                            */
#define LOCALSEMAPHORE1_UBOX_MISC_REG 0x1302420C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * unCore Semaphore register is a resource shared by all threads even though it has 
 * access and view for each one of the threads. Each one of the fields is 
 * identified to  
 *  be a shared or a dedicated element.
 */
typedef union {
  struct {
    UINT32 aqu_override : 1;
    /* aqu_override - Bits[0:0], WO, default = 1'b0 
       The bit has different meanings for read and write.
       Note that this bit has no stat directly associated with it. There are different 
        operations associated with the read and write operations.
        Write - Override acquisition :
         0 : no effect
         1 : Override. Take ownership of semaphore ignoring any other setting
             or requests.
     */
    UINT32 bus_aqu_release : 1;
    /* bus_aqu_release - Bits[1:1], RW_V, default = 1'b0 
       Read - Bus status :
         0 : the semaphore s currently free.
         1 : the semaphore is currently busy by one of the threads.
        Write - Acquire request :
         0 : Release the ownership/request of the semaphore. It cause the pending bit 
       for the 
             thread to be cleared.
         1 : Request the ownership of the semaphore. It cause the pending bit for the 
       thread 
              to be set. 
              
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 currentthread : 1;
    /* currentthread - Bits[4:4], RO_V, default = 1'b0 
       This field indicates the TID of the thread that currently owns the semaphore.
        When the semaphore is free (Busy==0), this bit is undefined.
         0 : Thread 0 
         1 : Thread 1
     */
    UINT32 currentcore : 6;
    /* currentcore - Bits[10:5], RO_V, default = 6'b000000 
       This field indicates the CID of the thread that currently owns the semaphore.
        When the semaphore is free (Busy==0), this bit is undefined.
         'n' - Core number
     */
    UINT32 rsvd_11 : 2;
    /* rsvd_11 - Bits[12:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 requesterthread : 1;
    /* requesterthread - Bits[13:13], WO, default = 1'b0 
       The thread id of the requester
     */
    UINT32 requestercore : 6;
    /* requestercore - Bits[19:14], WO, default = 6'b000000 
       Core ID of the requesting core
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LOCALSEMAPHORE1_UBOX_MISC_STRUCT;


/* SYSTEMSEMAPHORE0_UBOX_MISC_REG supported on:                               */
/*       SKX_A0 (0x40042210)                                                  */
/*       SKX (0x40042210)                                                     */
/* Register default value:              0x00000000                            */
#define SYSTEMSEMAPHORE0_UBOX_MISC_REG 0x13024210
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Uncore Semaphore register is a resource shared by all threads even though it has 
 * access and view for each one of the threads. Each one of the fields is 
 * identified to  
 *  be a shared or a dedicated element.
 */
typedef union {
  struct {
    UINT32 aqu_override : 1;
    /* aqu_override - Bits[0:0], WO, default = 1'b0 
       The bit has different meanings for read and write.
       Note that this bit has no stat directly associated with it. There are different 
        operations associated with the read and write operations.
        Write - Override acquisition :
         0 : no effect
         1 : Override. Take ownership of semaphore ignoring any other setting
             or requests.
     */
    UINT32 bus_aqu_release : 1;
    /* bus_aqu_release - Bits[1:1], RW_V, default = 1'b0 
       Read - Bus status :
         0 : the semaphore s currently free.
         1 : the semaphore is currently busy by one of the threads.
        Write - Acquire request :
         0 : Release the ownership/request of the semaphore. It cause the pending bit 
       for the 
             thread to be cleared.
         1 : Request the ownership of the semaphore. It cause the pending bit for the 
       thread 
              to be set. 
              
     */
    UINT32 currentnode : 3;
    /* currentnode - Bits[4:2], RO_V, default = 3'b000 
       This bit indicate the Node id of the node that currently owns the semaphore.
        When the semaphore is free (Busy==0), this bit is undefined.
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pendingrequestnode : 8;
    /* pendingrequestnode - Bits[15:8], RO_V, default = 8'b00000000 
       Pending request vector. Debug only usage
     */
    UINT32 rsvd_16 : 8;
    /* rsvd_16 - Bits[23:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 requesternode : 3;
    /* requesternode - Bits[26:24], WO, default = 3'b000 
       The requestor writes his own node id to the added into the pending vector
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SYSTEMSEMAPHORE0_UBOX_MISC_STRUCT;


/* SYSTEMSEMAPHORE1_UBOX_MISC_REG supported on:                               */
/*       SKX_A0 (0x40042214)                                                  */
/*       SKX (0x40042214)                                                     */
/* Register default value:              0x00000000                            */
#define SYSTEMSEMAPHORE1_UBOX_MISC_REG 0x13024214
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * unCore Semaphore register is a resource shared by all threads even though it has 
 * access and view for each one of the threads. Each one of the fields is 
 * identified to  
 *  be a shared or a dedicated element.
 */
typedef union {
  struct {
    UINT32 aqu_override : 1;
    /* aqu_override - Bits[0:0], WO, default = 1'b0 
       The bit has different meanings for read and write.
       Note that this bit has no stat directly associated with it. There are different 
        operations associated with the read and write operations. 
        Write - Override acquisition :
         0 : no effect
         1 : Override. Take ownership of semaphore ignoring any other setting
             or requests.
     */
    UINT32 bus_aqu_release : 1;
    /* bus_aqu_release - Bits[1:1], RW_V, default = 1'b0 
       Read - Bus status :
         0 : the semaphore s currently free.
         1 : the semaphore is currently busy by one of the threads.
        Write - Acquire request :
         0 : Release the ownership/request of the semaphore. It cause the pending bit 
       for the 
             thread to be cleared.
         1 : Request the ownership of the semaphore. It cause the pending bit for the 
       thread 
              to be set. 
              
     */
    UINT32 currentnode : 3;
    /* currentnode - Bits[4:2], RO_V, default = 3'b000 
       This bit indicate the Node id of the node that currently owns the semaphore.
        When the semaphore is free (Busy==0), this bit is undefined.
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pendingrequestnode : 8;
    /* pendingrequestnode - Bits[15:8], RO_V, default = 8'b00000000 
       Pending request vector. Debug only usage
     */
    UINT32 rsvd_16 : 8;
    /* rsvd_16 - Bits[23:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 requesternode : 3;
    /* requesternode - Bits[26:24], WO, default = 3'b000 
       The requestor writes his own node id to the added into the pending vector
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SYSTEMSEMAPHORE1_UBOX_MISC_STRUCT;


/* LOCALSEMAPHOREPEND1_UBOX_MISC_REG supported on:                            */
/*       SKX_A0 (0x40042228)                                                  */
/*       SKX (0x40042228)                                                     */
/* Register default value:              0x00000000                            */
#define LOCALSEMAPHOREPEND1_UBOX_MISC_REG 0x13024228
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Local semaphore1 pending bits used for debug.
 */
typedef union {
  struct {
    UINT32 pendingrequest : 32;
    /* pendingrequest - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Indicates the threads that have a pending request for Semaphore ownership for 
       cores 15:0. 
        bit 'i' in the field indicate a pending request for 2*CID+TID   (C0T0, C0T1, 
       C1T0,...C15T1) 
     */
  } Bits;
  UINT32 Data;
} LOCALSEMAPHOREPEND1_UBOX_MISC_STRUCT;


/* LOCALSEMAPHOREPEND1_CFG_2_UBOX_MISC_REG supported on:                      */
/*       SKX_A0 (0x4004222C)                                                  */
/*       SKX (0x4004222C)                                                     */
/* Register default value:              0x00000000                            */
#define LOCALSEMAPHOREPEND1_CFG_2_UBOX_MISC_REG 0x1302422C
/* Struct format extracted from XML file SKX\0.8.2.CFG.xml.
 * Local semaphore1 pending bits used for debug.
 */
typedef union {
  struct {
    UINT32 pendingrequest : 32;
    /* pendingrequest - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Indicates the threads that have a pending request for Semaphore ownership for 
       cores 31:16. 
        bit 'i' in the field indicate a pending request for 2*CID+TID   (C16T0, C16T1, 
       C17T0,...C31T1) 
     */
  } Bits;
  UINT32 Data;
} LOCALSEMAPHOREPEND1_CFG_2_UBOX_MISC_STRUCT;


#endif /* UBOX_MISC_h */
