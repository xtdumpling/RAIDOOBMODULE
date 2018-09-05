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

#ifndef UBOX_CFG_h
#define UBOX_CFG_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* UBOX_CFG_DEV 8                                                             */
/* UBOX_CFG_FUN 0                                                             */

/* VID_UBOX_CFG_REG supported on:                                             */
/*       SKX_A0 (0x20040000)                                                  */
/*       SKX (0x20040000)                                                     */
/* Register default value:              0x8086                                */
#define VID_UBOX_CFG_REG 0x13002000
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} VID_UBOX_CFG_STRUCT;


/* DID_UBOX_CFG_REG supported on:                                             */
/*       SKX_A0 (0x20040002)                                                  */
/*       SKX (0x20040002)                                                     */
/* Register default value:              0x2014                                */
#define DID_UBOX_CFG_REG 0x13002002
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2014 
        
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
} DID_UBOX_CFG_STRUCT;


/* PCICMD_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x20040004)                                                  */
/*       SKX (0x20040004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_UBOX_CFG_REG 0x13002004
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} PCICMD_UBOX_CFG_STRUCT;


/* PCISTS_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x20040006)                                                  */
/*       SKX (0x20040006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_UBOX_CFG_REG 0x13002006
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} PCISTS_UBOX_CFG_STRUCT;


/* RID_UBOX_CFG_REG supported on:                                             */
/*       SKX_A0 (0x10040008)                                                  */
/*       SKX (0x10040008)                                                     */
/* Register default value:              0x00                                  */
#define RID_UBOX_CFG_REG 0x13001008
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} RID_UBOX_CFG_STRUCT;


/* CCR_N0_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x10040009)                                                  */
/*       SKX (0x10040009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_UBOX_CFG_REG 0x13001009
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_UBOX_CFG_STRUCT;


/* CCR_N1_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x2004000A)                                                  */
/*       SKX (0x2004000A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_UBOX_CFG_REG 0x1300200A
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} CCR_N1_UBOX_CFG_STRUCT;


/* CLSR_UBOX_CFG_REG supported on:                                            */
/*       SKX_A0 (0x1004000C)                                                  */
/*       SKX (0x1004000C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_UBOX_CFG_REG 0x1300100C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} CLSR_UBOX_CFG_STRUCT;


/* PLAT_UBOX_CFG_REG supported on:                                            */
/*       SKX_A0 (0x1004000D)                                                  */
/*       SKX (0x1004000D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_UBOX_CFG_REG 0x1300100D
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} PLAT_UBOX_CFG_STRUCT;


/* HDR_UBOX_CFG_REG supported on:                                             */
/*       SKX_A0 (0x1004000E)                                                  */
/*       SKX (0x1004000E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_UBOX_CFG_REG 0x1300100E
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} HDR_UBOX_CFG_STRUCT;


/* BIST_UBOX_CFG_REG supported on:                                            */
/*       SKX_A0 (0x1004000F)                                                  */
/*       SKX (0x1004000F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_UBOX_CFG_REG 0x1300100F
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} BIST_UBOX_CFG_STRUCT;


/* SVID_UBOX_CFG_REG supported on:                                            */
/*       SKX_A0 (0x2004002C)                                                  */
/*       SKX (0x2004002C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_UBOX_CFG_REG 0x1300202C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_UBOX_CFG_STRUCT;


/* SDID_UBOX_CFG_REG supported on:                                            */
/*       SKX_A0 (0x2004002E)                                                  */
/*       SKX (0x2004002E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_UBOX_CFG_REG 0x1300202E
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_UBOX_CFG_STRUCT;


/* CAPPTR_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x10040034)                                                  */
/*       SKX (0x10040034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_UBOX_CFG_REG 0x13001034
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} CAPPTR_UBOX_CFG_STRUCT;


/* INTL_UBOX_CFG_REG supported on:                                            */
/*       SKX_A0 (0x1004003C)                                                  */
/*       SKX (0x1004003C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_UBOX_CFG_REG 0x1300103C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} INTL_UBOX_CFG_STRUCT;


/* INTPIN_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x1004003D)                                                  */
/*       SKX (0x1004003D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_UBOX_CFG_REG 0x1300103D
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} INTPIN_UBOX_CFG_STRUCT;


/* MINGNT_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x1004003E)                                                  */
/*       SKX (0x1004003E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_UBOX_CFG_REG 0x1300103E
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} MINGNT_UBOX_CFG_STRUCT;


/* MAXLAT_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x1004003F)                                                  */
/*       SKX (0x1004003F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_UBOX_CFG_REG 0x1300103F
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} MAXLAT_UBOX_CFG_STRUCT;


/* PXPCAP_UBOX_CFG_REG supported on:                                          */
/*       SKX_A0 (0x40040040)                                                  */
/*       SKX (0x40040040)                                                     */
/* Register default value:              0x00920010                            */
#define PXPCAP_UBOX_CFG_REG 0x13004040
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} PXPCAP_UBOX_CFG_STRUCT;


/* BIOSSTICKYLOCKBYPASSSCRATCHPAD2_UBOX_CFG_REG supported on:                 */
/*       SKX_A0 (0x4004007C)                                                  */
/*       SKX (0x4004007C)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSTICKYLOCKBYPASSSCRATCHPAD2_UBOX_CFG_REG 0x1300407C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * BIOS SMM handler modifies the register contents during CPU HotAdd_Sparing RAS 
 * events. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS_LB, default = 32'b00000000000000000000000000000000 
       Usage - checkpoints to see where in the process the new socket is at.
     */
  } Bits;
  UINT32 Data;
} BIOSSTICKYLOCKBYPASSSCRATCHPAD2_UBOX_CFG_STRUCT;


/* BIOSSTICKYLOCKBYPASSSCRATCHPAD3_UBOX_CFG_REG supported on:                 */
/*       SKX_A0 (0x40040080)                                                  */
/*       SKX (0x40040080)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSTICKYLOCKBYPASSSCRATCHPAD3_UBOX_CFG_REG 0x13004080
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * BIOS SMM handler modifies the register contents during CPU HotAdd_Sparing RAS 
 * events. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS_LB, default = 32'b00000000000000000000000000000000 
       Usage - checkpoints to see where in the process the new socket is at.
     */
  } Bits;
  UINT32 Data;
} BIOSSTICKYLOCKBYPASSSCRATCHPAD3_UBOX_CFG_STRUCT;


/* BIOSSTICKYLOCKBYPASSSCRATCHPAD4_UBOX_CFG_REG supported on:                 */
/*       SKX_A0 (0x40040084)                                                  */
/*       SKX (0x40040084)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSTICKYLOCKBYPASSSCRATCHPAD4_UBOX_CFG_REG 0x13004084
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * BIOS SMM handler modifies the register contents during CPU HotAdd_Sparing RAS 
 * events. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS_LB, default = 32'b00000000000000000000000000000000 
       Usage - checkpoints to see where in the process the new socket is at.
     */
  } Bits;
  UINT32 Data;
} BIOSSTICKYLOCKBYPASSSCRATCHPAD4_UBOX_CFG_STRUCT;


/* BIOSSTICKYLOCKBYPASSSCRATCHPAD5_UBOX_CFG_REG supported on:                 */
/*       SKX_A0 (0x40040088)                                                  */
/*       SKX (0x40040088)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSTICKYLOCKBYPASSSCRATCHPAD5_UBOX_CFG_REG 0x13004088
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * BIOS SMM handler modifies the register contents during CPU HotAdd_Sparing RAS 
 * events. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS_LB, default = 32'b00000000000000000000000000000000 
       Usage - checkpoints to see where in the process the new socket is at.
     */
  } Bits;
  UINT32 Data;
} BIOSSTICKYLOCKBYPASSSCRATCHPAD5_UBOX_CFG_STRUCT;


/* BIOSSTICKYLOCKBYPASSSCRATCHPAD6_UBOX_CFG_REG supported on:                 */
/*       SKX_A0 (0x4004008C)                                                  */
/*       SKX (0x4004008C)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSTICKYLOCKBYPASSSCRATCHPAD6_UBOX_CFG_REG 0x1300408C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * BIOS SMM handler modifies the register contents during CPU HotAdd_Sparing RAS 
 * events. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS_LB, default = 32'b00000000000000000000000000000000 
       Usage - checkpoints to see where in the process the new socket is at.
     */
  } Bits;
  UINT32 Data;
} BIOSSTICKYLOCKBYPASSSCRATCHPAD6_UBOX_CFG_STRUCT;


/* UBOX_GL_ERR_CFG_UBOX_CFG_REG supported on:                                 */
/*       SKX_A0 (0x40040090)                                                  */
/*       SKX (0x40040090)                                                     */
/* Register default value:              0x03555570                            */
#define UBOX_GL_ERR_CFG_UBOX_CFG_REG 0x13004090
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * This is the viral/global fatal error config/status register in the Ubox. The 
 * mask bits in UBOXErrCtl_CFG are set, the mask bits of this register are don't 
 * care and no viral/fatal error are signaled. 
 */
typedef union {
  struct {
    UINT32 statusuboxfatalerror : 1;
    /* statusuboxfatalerror - Bits[0:0], RO_V, default = 1'b0 
       Read only global fatal error status in UBox (UMC or IERR)
     */
    UINT32 statusuboxviral : 1;
    /* statusuboxviral - Bits[1:1], RO_V, default = 1'b0 
       Read only global viralr status in UBox
     */
    UINT32 resetuboxfatalerror : 1;
    /* resetuboxfatalerror - Bits[2:2], RW, default = 1'b0 
       Set 1 to force clear Ubox fatal error status.
     */
    UINT32 resetuboxviral : 1;
    /* resetuboxviral - Bits[3:3], RW, default = 1'b0 
       Set 1 to force clear Ubox viral status.
     */
    UINT32 maskuboxumc : 1;
    /* maskuboxumc - Bits[4:4], RW, default = 1'b1 
       Set 1 to block fatal error status updated by UMC message received in UBox.
     */
    UINT32 maskuboxierr : 1;
    /* maskuboxierr - Bits[5:5], RW, default = 1'b1 
       Set 1 to block fatal error status updated by IERR message received in UBox.
     */
    UINT32 maskunsupportopcode : 2;
    /* maskunsupportopcode - Bits[7:6], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by unsupported opcode received 
       in UBox. 
     */
    UINT32 maskpoisonreception : 2;
    /* maskpoisonreception - Bits[9:8], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by poison received in UBox.
     */
    UINT32 maskcfgrdaddrmisalign : 2;
    /* maskcfgrdaddrmisalign - Bits[11:10], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by CFG read address 
       misalignment in UBox. 
     */
    UINT32 maskcfgwraddrmisalign : 2;
    /* maskcfgwraddrmisalign - Bits[13:12], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by CFG write address 
       misalignment in UBox. 
     */
    UINT32 masksmitimeout : 2;
    /* masksmitimeout - Bits[15:14], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by SMI timeout in UBox.
     */
    UINT32 maskmasterlocktimeout : 2;
    /* maskmasterlocktimeout - Bits[17:16], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by master lock timeout in UBox.
     */
    UINT32 maskpcuproxyierr : 2;
    /* maskpcuproxyierr - Bits[19:18], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by PCU proxy of IERR in UBox.
     */
    UINT32 maskpcuproxyumc : 2;
    /* maskpcuproxyumc - Bits[21:20], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by PCU proxy of UMC in UBox.
     */
    UINT32 maskpcuproxymsmi : 2;
    /* maskpcuproxymsmi - Bits[23:22], RW, default = 2'b01 
       Set 1 to block viral[0], fatal[1] status updated by PCU proxy of UMC in UBox.
     */
    UINT32 maskextviral : 1;
    /* maskextviral - Bits[24:24], RW, default = 1'b1 
       Set 1 to block viral status updated by CATERR pin assertion
     */
    UINT32 maskuboxmsmi : 1;
    /* maskuboxmsmi - Bits[25:25], RW, default = 1'b1 
       Set 1 to block fatal error status updated by MSMI message received in UBox.
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} UBOX_GL_ERR_CFG_UBOX_CFG_STRUCT;


/* UBOX_CTS_ERR_MASK_UBOX_CFG_REG supported on:                               */
/*       SKX_A0 (0x40040094)                                                  */
/*       SKX (0x40040094)                                                     */
/* Register default value:              0x000FFFFF                            */
#define UBOX_CTS_ERR_MASK_UBOX_CFG_REG 0x13004094
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * This is the mask register to filter Ubox errors sending to CTS. If the mask bits 
 * in UBOXErrCtl_CFG are set, the mask bits of this register are don't care and no 
 * error is sent. 
 */
typedef union {
  struct {
    UINT32 maskunsupportopcode : 1;
    /* maskunsupportopcode - Bits[0:0], RW, default = 1'b1 
       Set 1 to block CTS error updated by unsupported opcode received in UBox.
     */
    UINT32 maskpoisonreception : 1;
    /* maskpoisonreception - Bits[1:1], RW, default = 1'b1 
       Set 1 to block CTS error updated by poison received in UBox.
     */
    UINT32 maskcfgrdaddrmisalign : 1;
    /* maskcfgrdaddrmisalign - Bits[2:2], RW, default = 1'b1 
       Set 1 to block CTS error updated by CFG read address misalignment in UBox.
     */
    UINT32 maskcfgwraddrmisalign : 1;
    /* maskcfgwraddrmisalign - Bits[3:3], RW, default = 1'b1 
       Set 1 to block CTS error updated by CFG write address misalignment in UBox.
     */
    UINT32 masksmitimeout : 1;
    /* masksmitimeout - Bits[4:4], RW, default = 1'b1 
       Set 1 to block CTS error updated by SMI timeout in UBox.
     */
    UINT32 maskmasterlocktimeout : 1;
    /* maskmasterlocktimeout - Bits[5:5], RW, default = 1'b1 
       Set 1 to block CTS error updated by master lock timeout in UBox.
     */
    UINT32 maskm2mcerror : 1;
    /* maskm2mcerror - Bits[6:6], RW, default = 1'b1 
       Set 1 to block CTS error updated by strobe MC error from M2PCIE.
     */
    UINT32 rsvd_7 : 1;
    UINT32 maskuboxierr : 1;
    /* maskuboxierr - Bits[8:8], RW, default = 1'b1 
       Set 1 to block CTS error updated by IERR message received in UBox.
     */
    UINT32 maskuboxumc : 1;
    /* maskuboxumc - Bits[9:9], RW, default = 1'b1 
       Set 1 to block CTS error updated by UMC message received in UBox.
     */
    UINT32 maskuboxmsmi : 1;
    /* maskuboxmsmi - Bits[10:10], RW, default = 1'b1 
       Set 1 to block CTS error updated by MSMI message received in UBox.
     */
    UINT32 maskpcuproxyierr : 1;
    /* maskpcuproxyierr - Bits[11:11], RW, default = 1'b1 
       Set 1 to block CTS error updated by PCU proxy of IERR in UBox.
     */
    UINT32 maskpcuproxyumc : 1;
    /* maskpcuproxyumc - Bits[12:12], RW, default = 1'b1 
       Set 1 to block CTS error updated by PCU proxy of UMC in UBox.
     */
    UINT32 maskpcuproxymsmi : 1;
    /* maskpcuproxymsmi - Bits[13:13], RW, default = 1'b1 
       Set 1 to block CTS error updated by PCU proxy of MSMI in UBox.
     */
    UINT32 maskextviral : 1;
    /* maskextviral - Bits[14:14], RW, default = 1'b1 
       Set 1 to CTS error updated by CATERR pin assertion
     */
    UINT32 maskmsgchtrkerr : 1;
    /* maskmsgchtrkerr - Bits[15:15], RW, default = 1'b1 
       Set 1 to CTS error updated by Message Channel Tracker in UBox
     */
    UINT32 maskmsgchtrktimeout : 1;
    /* maskmsgchtrktimeout - Bits[16:16], RW, default = 1'b1 
       Set 1 to CTS error updated by Message Channel Tracker in UBox
     */
    UINT32 masksmisrcumc : 1;
    /* masksmisrcumc - Bits[17:17], RW, default = 1'b1 
       Set 1 to CTS error updated by SMI from UMC in UBox
     */
    UINT32 masksmisrcimc : 1;
    /* masksmisrcimc - Bits[18:18], RW, default = 1'b1 
       Set 1 to CTS error updated by SMI from iMC in UBox
     */
    UINT32 masksmidelivld : 1;
    /* masksmidelivld - Bits[19:19], RW, default = 1'b1 
       Set 1 to CTS error updated by SMI delivery status valid in UBox
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} UBOX_CTS_ERR_MASK_UBOX_CFG_STRUCT;


/* UBOX_EVENT_ERR_SEL_UBOX_CFG_REG supported on:                              */
/*       SKX_A0 (0x40040098)                                                  */
/*       SKX (0x40040098)                                                     */
/* Register default value:              0x00000000                            */
#define UBOX_EVENT_ERR_SEL_UBOX_CFG_REG 0x13004098
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * This is the select register to choose the error event to be sent out as triggers 
 * for CTS or MBP. 
 */
typedef union {
  struct {
    UINT32 triggersel0 : 2;
    /* triggersel0 - Bits[1:0], RW, default = 2'b00 
       Select which error event to output to CTS or MBP through Trigger Bit0.
       0 : PCU UMC
       1 : CTS Trigger selected by UBOX_CTS_ERR_MASK_CFG
       2 : IIO Error[0] 
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 triggersel1 : 2;
    /* triggersel1 - Bits[5:4], RW, default = 2'b00 
       Select which error event to output to CTS or MBP through Trigger Bit1.
       0 : PCU IERR
       1 : CTS Trigger selected by UBOX_CTS_ERR_MASK_CFG
       2 : IIO Error[1] 
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 triggersel2 : 2;
    /* triggersel2 - Bits[9:8], RW, default = 2'b00 
       Select which error event to output to CTS or MBP through Trigger Bit2.
       0 : MSMI
       1 : CTS Trigger selected by UBOX_CTS_ERR_MASK_CFG
       2 : IIO Error[2] 
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 triggersel3 : 2;
    /* triggersel3 - Bits[13:12], RW, default = 2'b00 
       Select which error event to output to CTS or MBP through Trigger Bit3.
       0 : CSMI
       1 : CTS Trigger selected by UBOX_CTS_ERR_MASK_CFG
       2 : IIO Error[0] 
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 triggersel4 : 2;
    /* triggersel4 - Bits[17:16], RW, default = 2'b00 
       Select which error event to output to CTS or MBP through Trigger Bit4.
       0 : CMC
       1 : CTS Trigger selected by UBOX_CTS_ERR_MASK_CFG
       2 : IIO Error[1] 
     */
    UINT32 rsvd_18 : 2;
    /* rsvd_18 - Bits[19:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 triggersel5 : 2;
    /* triggersel5 - Bits[21:20], RW, default = 2'b00 
       Select which error event to output to CTS or MBP through Trigger Bit5.
       0 : PMON Overflow
       1 : CTS Trigger selected by UBOX_CTS_ERR_MASK_CFG
       2 : IIO Error[2] 
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} UBOX_EVENT_ERR_SEL_UBOX_CFG_STRUCT;






/* IERRLOGGINGREG_UBOX_CFG_REG supported on:                                  */
/*       SKX_A0 (0x400400A4)                                                  */
/*       SKX (0x400400A4)                                                     */
/* Register default value:              0x00000000                            */
#define IERRLOGGINGREG_UBOX_CFG_REG 0x130040A4
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 *  IERR first/second logging error.
 */
typedef union {
  struct {
    UINT32 firstierrsrcid : 8;
    /* firstierrsrcid - Bits[7:0], RWS_V, default = 8'b00000000 
       
               msgCh portID of the end point with the first IERR.
        If FirstIerrSrcValid is 1, the block responsible for generating the first IERR 
       is decoded as follows: 
       	01000100 - PCU
       	{2'b11,6'bnnnnnn} - CHAID == 6'bnnnnnn
               
     */
    UINT32 firstierrsrcvalid : 1;
    /* firstierrsrcvalid - Bits[8:8], RWS_V, default = 1'b0 
        set to 1 if the FirstSrcID is valid
     */
    UINT32 firstierrsrcfromcbo : 1;
    /* firstierrsrcfromcbo - Bits[9:9], RWS_V, default = 1'b0 
       Set to 1 of the FirstIerrSrcID is from a Cbo or core.
     */
    UINT32 rsvd_10 : 6;
    /* rsvd_10 - Bits[15:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 secondierrsrcid : 8;
    /* secondierrsrcid - Bits[23:16], RWS_V, default = 8'b00000000 
       If SecondIerrSrcValid is 1, the block responsible for generating the second IERR 
       is logged here.  Refer to decode table in FirstIerrSrcID. 
     */
    UINT32 secondierrsrcvalid : 1;
    /* secondierrsrcvalid - Bits[24:24], RWS_V, default = 1'b0 
        set to 1 if the SecondIerrSrcID is valid
     */
    UINT32 secondierrsrcfromcbo : 1;
    /* secondierrsrcfromcbo - Bits[25:25], RWS_V, default = 1'b0 
       Set to 1 if the second SrcID is from a Cbo or core.
     */
    UINT32 rsvd_26 : 6;
    /* rsvd_26 - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IERRLOGGINGREG_UBOX_CFG_STRUCT;


/* MCERRLOGGINGREG_UBOX_CFG_REG supported on:                                 */
/*       SKX_A0 (0x400400A8)                                                  */
/*       SKX (0x400400A8)                                                     */
/* Register default value:              0x00000000                            */
#define MCERRLOGGINGREG_UBOX_CFG_REG 0x130040A8
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 *  MCERR first/second logging error.
 */
typedef union {
  struct {
    UINT32 firstmcerrsrcid : 8;
    /* firstmcerrsrcid - Bits[7:0], RWS_V, default = 8'b00000000 
       msgCh portID of the EP with the first MCERR.
        If FirstMCerrSrcValid is 1, the block responsible for generating the first 
       MCERR is decoded as follows: 
       	00000010 - Intel UPI 0
       	00000011 - Intel UPI 1
       	00000110 - Intel UPI 2
               01000100 - PCU
       	01100000 - IMC 0, channel by subPortID 
       	01100100 - IMC 1, channel by subPortID 
       	01100010 - M2MEM0
       	01100110 - M2MEM1
       	101000xx - IIO0
       	101001xx - IIO1
       	101010xx - IIO2
       	101011xx - CBDMA
       	101100xx - MCP0
       	101101xx - MCP1
       	{2'b11,6'bnnnnnn} - CHAID == 6'bnnnnnn
        	
     */
    UINT32 firstmcerrsrcvalid : 1;
    /* firstmcerrsrcvalid - Bits[8:8], RWS_V, default = 1'b0 
       Set to 1 if the FirstSrcID is valid.
     */
    UINT32 firstmcerrsrcfromcbo : 1;
    /* firstmcerrsrcfromcbo - Bits[9:9], RWS_V, default = 1'b0 
       Set to 1 of the FirstMCerrSrcID is from a Cbo or CORE. 
     */
    UINT32 firstmcerrsrcsubportid : 4;
    /* firstmcerrsrcsubportid - Bits[13:10], RWS_V, default = 4'b0000 
       msgCh Sub PortID of the EP with the first MCERR. The subPortID here is important 
       for MC to specify a channel, but not applied to other clusters. 
               0001 - MC MAIN
               0010 - MC 2LM0
               0011 - MC 2LM1
               0100 - MC 2LM2
               0101 - MC LMSCH0
               0110 - MC LMDP0
               0111 - MC LMSCH1
               1000 - MC LMDP1
               1001 - MC LMSCH2
               1010 - MC LMDP2
       
        	
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 secondmcerrsrcid : 8;
    /* secondmcerrsrcid - Bits[23:16], RWS_V, default = 8'b00000000 
       If SecondMCerrSrcValid is 1, the block responsible for generating the second 
       MCERR is logged here. Refer to decode table in FirstMCerrSrcID. 
     */
    UINT32 secondmcerrsrcvalid : 1;
    /* secondmcerrsrcvalid - Bits[24:24], RWS_V, default = 1'b0 
       Set to 1 if the SecondMCerrSrcID is valid.
     */
    UINT32 secondmcerrsrcfromcbo : 1;
    /* secondmcerrsrcfromcbo - Bits[25:25], RWS_V, default = 1'b0 
       Set to 1 if the second SrcID is from a Cbo or CORE. 
     */
    UINT32 secondmcerrsrcsubportid : 4;
    /* secondmcerrsrcsubportid - Bits[29:26], RWS_V, default = 4'b0000 
       msgCh Sub PortID of the EP with the first MCERR.
        	
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCERRLOGGINGREG_UBOX_CFG_STRUCT;


/* EMCA_CORE_CSMI_LOG_UBOX_CFG_REG supported on:                              */
/*       SKX_A0 (0x400400B0)                                                  */
/*       SKX (0x400400B0)                                                     */
/* Register default value:              0x00000000                            */
#define EMCA_CORE_CSMI_LOG_UBOX_CFG_REG 0x130040B0
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * This is a log of which cores have signalled a CSMI to ubox via the MCLK message. 
 *  
 */
typedef union {
  struct {
    UINT32 core0 : 1;
    /* core0 - Bits[0:0], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 0
               
     */
    UINT32 core1 : 1;
    /* core1 - Bits[1:1], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 1
               
     */
    UINT32 core2 : 1;
    /* core2 - Bits[2:2], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 2
               
     */
    UINT32 core3 : 1;
    /* core3 - Bits[3:3], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 3
               
     */
    UINT32 core4 : 1;
    /* core4 - Bits[4:4], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 4
               
     */
    UINT32 core5 : 1;
    /* core5 - Bits[5:5], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 5
               
     */
    UINT32 core6 : 1;
    /* core6 - Bits[6:6], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 6
               
     */
    UINT32 core7 : 1;
    /* core7 - Bits[7:7], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 7
               
     */
    UINT32 core8 : 1;
    /* core8 - Bits[8:8], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 8
               
     */
    UINT32 core9 : 1;
    /* core9 - Bits[9:9], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 9
               
     */
    UINT32 core10 : 1;
    /* core10 - Bits[10:10], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 10
               
     */
    UINT32 core11 : 1;
    /* core11 - Bits[11:11], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 11
               
     */
    UINT32 core12 : 1;
    /* core12 - Bits[12:12], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 12
               
     */
    UINT32 core13 : 1;
    /* core13 - Bits[13:13], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 13
               
     */
    UINT32 core14 : 1;
    /* core14 - Bits[14:14], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 14
               
     */
    UINT32 core15 : 1;
    /* core15 - Bits[15:15], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 15
               
     */
    UINT32 core16 : 1;
    /* core16 - Bits[16:16], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 16
               
     */
    UINT32 core17 : 1;
    /* core17 - Bits[17:17], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 17
               
     */
    UINT32 core18 : 1;
    /* core18 - Bits[18:18], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 18
               
     */
    UINT32 core19 : 1;
    /* core19 - Bits[19:19], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 19
               
     */
    UINT32 core20 : 1;
    /* core20 - Bits[20:20], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 20
               
     */
    UINT32 core21 : 1;
    /* core21 - Bits[21:21], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 21
               
     */
    UINT32 core22 : 1;
    /* core22 - Bits[22:22], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 22
               
     */
    UINT32 core23 : 1;
    /* core23 - Bits[23:23], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 23
               
     */
    UINT32 core24 : 1;
    /* core24 - Bits[24:24], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 24
               
     */
    UINT32 core25 : 1;
    /* core25 - Bits[25:25], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 25
               
     */
    UINT32 core26 : 1;
    /* core26 - Bits[26:26], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 26
               
     */
    UINT32 core27 : 1;
    /* core27 - Bits[27:27], RW1C, default = 1'b0 
       
                 CSMI received indicator for core 27
               
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} EMCA_CORE_CSMI_LOG_UBOX_CFG_STRUCT;


/* EMCA_CORE_MSMI_LOG_UBOX_CFG_REG supported on:                              */
/*       SKX_A0 (0x400400B8)                                                  */
/*       SKX (0x400400B8)                                                     */
/* Register default value:              0x00000000                            */
#define EMCA_CORE_MSMI_LOG_UBOX_CFG_REG 0x130040B8
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * This is a log of which cores have signalled an MSMI to ubox via the MCLK 
 * message.  
 */
typedef union {
  struct {
    UINT32 core0 : 1;
    /* core0 - Bits[0:0], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 0
               
     */
    UINT32 core1 : 1;
    /* core1 - Bits[1:1], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 1
               
     */
    UINT32 core2 : 1;
    /* core2 - Bits[2:2], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 2
               
     */
    UINT32 core3 : 1;
    /* core3 - Bits[3:3], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 3
               
     */
    UINT32 core4 : 1;
    /* core4 - Bits[4:4], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 4
               
     */
    UINT32 core5 : 1;
    /* core5 - Bits[5:5], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 5
               
     */
    UINT32 core6 : 1;
    /* core6 - Bits[6:6], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 6
               
     */
    UINT32 core7 : 1;
    /* core7 - Bits[7:7], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 7
               
     */
    UINT32 core8 : 1;
    /* core8 - Bits[8:8], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 8
               
     */
    UINT32 core9 : 1;
    /* core9 - Bits[9:9], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 9
               
     */
    UINT32 core10 : 1;
    /* core10 - Bits[10:10], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 10
               
     */
    UINT32 core11 : 1;
    /* core11 - Bits[11:11], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 11
               
     */
    UINT32 core12 : 1;
    /* core12 - Bits[12:12], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 12
               
     */
    UINT32 core13 : 1;
    /* core13 - Bits[13:13], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 13
               
     */
    UINT32 core14 : 1;
    /* core14 - Bits[14:14], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 14
               
     */
    UINT32 core15 : 1;
    /* core15 - Bits[15:15], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 15
               
     */
    UINT32 core16 : 1;
    /* core16 - Bits[16:16], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 16
               
     */
    UINT32 core17 : 1;
    /* core17 - Bits[17:17], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 17
               
     */
    UINT32 core18 : 1;
    /* core18 - Bits[18:18], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 18
               
     */
    UINT32 core19 : 1;
    /* core19 - Bits[19:19], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 19
               
     */
    UINT32 core20 : 1;
    /* core20 - Bits[20:20], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 20
               
     */
    UINT32 core21 : 1;
    /* core21 - Bits[21:21], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 21
               
     */
    UINT32 core22 : 1;
    /* core22 - Bits[22:22], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 22
               
     */
    UINT32 core23 : 1;
    /* core23 - Bits[23:23], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 23
               
     */
    UINT32 core24 : 1;
    /* core24 - Bits[24:24], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 24
               
     */
    UINT32 core25 : 1;
    /* core25 - Bits[25:25], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 25
               
     */
    UINT32 core26 : 1;
    /* core26 - Bits[26:26], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 26
               
     */
    UINT32 core27 : 1;
    /* core27 - Bits[27:27], RW1C, default = 1'b0 
       
                 MSMI received indicator for core 27
               
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} EMCA_CORE_MSMI_LOG_UBOX_CFG_STRUCT;


/* CPUNODEID_UBOX_CFG_REG supported on:                                       */
/*       SKX_A0 (0x400400C0)                                                  */
/*       SKX (0x400400C0)                                                     */
/* Register default value:              0x00000000                            */
#define CPUNODEID_UBOX_CFG_REG 0x130040C0
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Node ID Configuration Register
 */
typedef union {
  struct {
    UINT32 lclnodeid : 3;
    /* lclnodeid - Bits[2:0], RW_LB, default = 3'b000 
       Node Id of the local Socket
     */
    UINT32 rsvd_3 : 2;
    /* rsvd_3 - Bits[4:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 locknodeid : 3;
    /* locknodeid - Bits[7:5], RW_LB, default = 3'b000 
       NodeId of the lock master
     */
    UINT32 rsvd_8 : 2;
    /* rsvd_8 - Bits[9:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lgcnodeid : 3;
    /* lgcnodeid - Bits[12:10], RW_LB, default = 3'b000 
       NodeID of the legacy socket
     */
    UINT32 nodectrlid : 3;
    /* nodectrlid - Bits[15:13], RW_LB, default = 3'b000 
       Node ID of the Node Controller. Set by the BIOS. (EX processor only)
     */
    UINT32 rsvd_16 : 16;
    /* rsvd_16 - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPUNODEID_UBOX_CFG_STRUCT;


/* CPUENABLE_UBOX_CFG_REG supported on:                                       */
/*       SKX_A0 (0x400400C4)                                                  */
/*       SKX (0x400400C4)                                                     */
/* Register default value:              0x00000000                            */
#define CPUENABLE_UBOX_CFG_REG 0x130040C4
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Indicates which CPU is enabled
 */
typedef union {
  struct {
    UINT32 nodeiden : 8;
    /* nodeiden - Bits[7:0], RW_LB, default = 8'b00000000 
       Bit mask to indicate which node_id is enabled.
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[30:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RW_LB, default = 1'b0 
       Valid bit to indicate that the register has been initialized.
     */
  } Bits;
  UINT32 Data;
} CPUENABLE_UBOX_CFG_STRUCT;


/* INTCONTROL_UBOX_CFG_REG supported on:                                      */
/*       SKX_A0 (0x400400C8)                                                  */
/*       SKX (0x400400C8)                                                     */
/* Register default value:              0x00000001                            */
#define INTCONTROL_UBOX_CFG_REG 0x130040C8
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Interrupt Configuration Register
 */
typedef union {
  struct {
    UINT32 xapicen : 1;
    /* xapicen - Bits[0:0], RW_LB, default = 1'b1 
       Set this bit if you would like extended XAPIC configuration to be used. This bit 
       can be written directly, and can also be updated using XTPR messages 
     */
    UINT32 forcex2apic : 1;
    /* forcex2apic - Bits[1:1], RW_LB, default = 1'b0 
       Write:
       1: Forces the system to move into X2APIC Mode.
       0: No affect
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdrmodsel : 3;
    /* rdrmodsel - Bits[6:4], RW, default = 3'b000 
       Selects the redirection mode used for MSI interrupts with lowest-priority 
       delivery mode. The folowing schemes are used :  
       000 : Fixed Priority - select the first enabled APIC in the cluster. 
       001: Redirect last - last vector selected (applicable only in extended mode)
       010 : Hash Vector - select the first enabled APIC in round robin manner starting 
       form the hash of the vector number.  
       default: Fixed Priority
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 hashmodctr : 3;
    /* hashmodctr - Bits[10:8], RW, default = 3'b000 
       Indicates the hash mode control for the interrupt control.
       Select the hush function for the Vector based Hash Mode interrupt redirection 
       control :  
       000 select bits 7:4/5:4 for vector cluster/flat algorithm 
       001 select bits 6:3/4:3 
       010 select bits 4:1/2:1 
       011 select bits 3:0/1:0 
       other - reserved
     */
    UINT32 rsvd_11 : 5;
    /* rsvd_11 - Bits[15:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 clastchksmpmod : 1;
    /* clastchksmpmod - Bits[16:16], RW_LB, default = 1'b0 
       0: Disable checking for Logical_APICID[31:0] being non-zero when sampling flat/ 
       cluster mode bit in the IntPrioUpd message as part of setting bit 1 in this 
       register  
       1: Enable the above checking
     */
    UINT32 logfltclustmod : 1;
    /* logfltclustmod - Bits[17:17], RW_LBV, default = 1'b0 
       Set by bios to indicate if the OS is running logical flat or logical cluster 
       mode. This bit can also be updated by IntPrioUpd messages.  
       This bit reflects the setup of the filter at any given time. 
       0 - flat, 
       1 - cluster.
     */
    UINT32 logflatclustovren : 1;
    /* logflatclustovren - Bits[18:18], RW_LB, default = 1'b0 
       0 : IA32 Logical Flat or Cluster Mode bit is locked as Read only bit. 
       1 : IA32 Logical Flat or Cluster Mode bit may be written by SW, values written 
       by xTPR update are ignored.  
       For one time override of the IA32 Logical Flat or Cluster Mode value, return 
       this bit to it's default state after the bit is changed. Leaving this bit as '1' 
       will prevent automatic update of the filter. 
     */
    UINT32 rsvd_19 : 13;
    /* rsvd_19 - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} INTCONTROL_UBOX_CFG_STRUCT;


/* BCASTLIST_UBOX_CFG_REG supported on:                                       */
/*       SKX_A0 (0x400400CC)                                                  */
/*       SKX (0x400400CC)                                                     */
/* Register default value:              0x00000000                            */
#define BCASTLIST_UBOX_CFG_REG 0x130040CC
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Broadcast lists
 */
typedef union {
  struct {
    UINT32 intlist : 8;
    /* intlist - Bits[7:0], RW_LB, default = 8'b00000000 
       Broadcast list for interrupts and PMReq in an hot-add capable system
     */
    UINT32 stopstartlist : 8;
    /* stopstartlist - Bits[15:8], RW_LB, default = 8'b00000000 
       Broadcast list for StopReq/StartReq in a hot-add capable system
     */
    UINT32 misclist : 8;
    /* misclist - Bits[23:16], RW_LB, default = 8'b00000000 
       Broadcast list for WbInvdAck/InvdAck/EOI in a hot-add capable system
     */
    UINT32 intpriolist : 8;
    /* intpriolist - Bits[31:24], RW_LB, default = 8'b00000000 
       Broadcast list for IntPrioUpd in a hot-add capable system
     */
  } Bits;
  UINT32 Data;
} BCASTLIST_UBOX_CFG_STRUCT;


/* LOCKCONTROL_UBOX_CFG_REG supported on:                                     */
/*       SKX_A0 (0x400400D0)                                                  */
/*       SKX (0x400400D0)                                                     */
/* Register default value:              0x00000003                            */
#define LOCKCONTROL_UBOX_CFG_REG 0x130040D0
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Lock Control registers
 */
typedef union {
  struct {
    UINT32 lockdisable : 1;
    /* lockdisable - Bits[0:0], RW, default = 1'b1 
       Whether Locks are enabled in the system or not
     */
    UINT32 lockdelay : 3;
    /* lockdelay - Bits[3:1], RW, default = 3'b001 
       Delay between SysLock
       This may be used to prevent starvation on frequent Lock usage.
          000 - 0x0
          001 - 0x200 (1.2us)
          010 - 0x1000 (10us)
          011 - 0x2000 (20us)
          100 - 0x4000 (40us)
          101 - 0x8000 (80 us)
          110 - 0x10000 (160 us)
          111 - 0x20000 (320 us)
     */
    UINT32 compatibilitymode : 1;
    /* compatibilitymode - Bits[4:4], RO, default = 1'b0 
       Enable Compatibility Mode - disabled since no longer supported
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LOCKCONTROL_UBOX_CFG_STRUCT;


/* GIDNIDMAP_UBOX_CFG_REG supported on:                                       */
/*       SKX_A0 (0x400400D4)                                                  */
/*       SKX (0x400400D4)                                                     */
/* Register default value:              0x00000000                            */
#define GIDNIDMAP_UBOX_CFG_REG 0x130040D4
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Mapping between group id and nodeid
 */
typedef union {
  struct {
    UINT32 nodeid0 : 3;
    /* nodeid0 - Bits[2:0], RW_LB, default = 3'b000 
       Node Id for group 0
     */
    UINT32 nodeid1 : 3;
    /* nodeid1 - Bits[5:3], RW_LB, default = 3'b000 
       Node Id for group Id 1
     */
    UINT32 nodeid2 : 3;
    /* nodeid2 - Bits[8:6], RW_LB, default = 3'b000 
       Node Id for group Id 2
     */
    UINT32 nodeid3 : 3;
    /* nodeid3 - Bits[11:9], RW_LB, default = 3'b000 
       Node Id for group 3
     */
    UINT32 nodeid4 : 3;
    /* nodeid4 - Bits[14:12], RW_LB, default = 3'b000 
       Node Id for group id 4
     */
    UINT32 nodeid5 : 3;
    /* nodeid5 - Bits[17:15], RW_LB, default = 3'b000 
       Node Id for group 5
     */
    UINT32 nodeid6 : 3;
    /* nodeid6 - Bits[20:18], RW_LB, default = 3'b000 
       Node Id for group 6
     */
    UINT32 nodeid7 : 3;
    /* nodeid7 - Bits[23:21], RW_LB, default = 3'b000 
       NodeId for group id 7
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} GIDNIDMAP_UBOX_CFG_STRUCT;


/* EMCA_EN_CORE_IERR_TO_MSMI_UBOX_CFG_REG supported on:                       */
/*       SKX_A0 (0x400400D8)                                                  */
/*       SKX (0x400400D8)                                                     */
/* Register default value:              0x00000000                            */
#define EMCA_EN_CORE_IERR_TO_MSMI_UBOX_CFG_REG 0x130040D8
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * When this bit set the ubox will convert IERR's generated by the core to an 
 * MSMI's to allow firmware  
 *                    first model. In addition the Ubox will signal the PCU to 
 * assert the MSMI pin.   
 *        the MSMI socket pin
 */
typedef union {
  struct {
    UINT32 encoreierrtomsmi : 1;
    /* encoreierrtomsmi - Bits[0:0], RW_LB, default = 1'b0 
       setting this bit enables the Ubox to convert an IERR from the core to an MSMI. 
       Additionally the Ubox  
                            will send a NCU_PCU_MSG to the PCU with the IERR and MSMI 
       bit asserted.   
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} EMCA_EN_CORE_IERR_TO_MSMI_UBOX_CFG_STRUCT;


/* CORECOUNT_UBOX_CFG_REG supported on:                                       */
/*       SKX_A0 (0x400400E0)                                                  */
/*       SKX (0x400400E0)                                                     */
/* Register default value:              0x00000000                            */
#define CORECOUNT_UBOX_CFG_REG 0x130040E0
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Reflection of the LTCount2 register
 */
typedef union {
  struct {
    UINT32 corecount : 6;
    /* corecount - Bits[5:0], RO_V, default = 6'b000000 
       Reflection of the LTCount2 uCR
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CORECOUNT_UBOX_CFG_STRUCT;


/* UBOXERRSTS_UBOX_CFG_REG supported on:                                      */
/*       SKX_A0 (0x400400E4)                                                  */
/*       SKX (0x400400E4)                                                     */
/* Register default value:              0x00000000                            */
#define UBOXERRSTS_UBOX_CFG_REG 0x130040E4
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * This is error status register in the Ubox and covers most of the interrupt 
 * related errors 
 */
typedef union {
  struct {
    UINT32 smisrcumc : 1;
    /* smisrcumc - Bits[0:0], RWS_V, default = 1'b0 
       This is a bit that indicates that an SMI was caused due to a locally generated 
       UMC 
     */
    UINT32 smisrcimc : 1;
    /* smisrcimc - Bits[1:1], RWS_V, default = 1'b0 
       SMI is caused due to an imdication from the IMC
     */
    UINT32 poisonrsvd : 1;
    /* poisonrsvd - Bits[2:2], RWS_V, default = 1'b0 
       Ubox received a poisoned transaction
     */
    UINT32 unsupportedopcode : 1;
    /* unsupportedopcode - Bits[3:3], RWS_V, default = 1'b0 
       Unsupported opcode received by Ubox
     */
    UINT32 cfgrdaddrmisaligned : 1;
    /* cfgrdaddrmisaligned - Bits[4:4], RWS_V, default = 1'b0 
       MMCFG Read Address Misaligment received by Ubox
     */
    UINT32 cfgwraddrmisaligned : 1;
    /* cfgwraddrmisaligned - Bits[5:5], RWS_V, default = 1'b0 
       MMCFG Write Address Misaligment received by Ubox
     */
    UINT32 smitimeout : 1;
    /* smitimeout - Bits[6:6], RWS_V, default = 1'b0 
       SMI Timeout received by Ubox
     */
    UINT32 masterlocktimeout : 1;
    /* masterlocktimeout - Bits[7:7], RWS_V, default = 1'b0 
       Master Lock Timeout received by Ubox
     */
    UINT32 reserved : 8;
    /* reserved - Bits[15:8], RO_V, default = 8'h0 
       reserved
     */
    UINT32 smi_delivery_valid : 1;
    /* smi_delivery_valid - Bits[16:16], RW_V, default = 1'b0 
       SMI interrupt delivery status valid, write 1'b0 to clear valid status
     */
    UINT32 msg_ch_tkr_err : 1;
    /* msg_ch_tkr_err - Bits[17:17], RWS_V, default = 1'b0 
       Message Channel Tracker Error. This error occurs such case that illegal broad 
       cast port ID access to the message channel.  
     */
    UINT32 msg_ch_tkr_timeout : 6;
    /* msg_ch_tkr_timeout - Bits[23:18], RWS_V, default = 6'b000000 
       Message Channel Tracker TimeOut. This error occurs when any NP request doesn't 
       receive response in 4K cycles. The event is SV use and logging only, not 
       signaling as Ubox error. 
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} UBOXERRSTS_UBOX_CFG_STRUCT;


/* UBOXERRCTL_UBOX_CFG_REG supported on:                                      */
/*       SKX_A0 (0x400400E8)                                                  */
/*       SKX (0x400400E8)                                                     */
/* Register default value:              0x81FF7FC0                            */
#define UBOXERRCTL_UBOX_CFG_REG 0x130040E8
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Ubox per-error enable mask. If masked, a given Ubox error will not be signaled 
 * via SMI and will not be logged.  
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 poisonmask : 1;
    /* poisonmask - Bits[2:2], RWS, default = 1'b0 
       Mask SMI generation on receiving poison in Ubox.
     */
    UINT32 unsupportedmask : 1;
    /* unsupportedmask - Bits[3:3], RWS, default = 1'b0 
       Mask SMI generation on receiving unsupported opcodes.
     */
    UINT32 cfgrdaddrmisalignedmask : 1;
    /* cfgrdaddrmisalignedmask - Bits[4:4], RWS, default = 1'b0 
       Mask SMI generation on receiving MMCFG read address misalignment.
     */
    UINT32 cfgwraddrmisalignedmask : 1;
    /* cfgwraddrmisalignedmask - Bits[5:5], RWS, default = 1'b0 
       Mask SMI generation on receiving MMCFG write address misalignment.
     */
    UINT32 smitimeoutmask : 1;
    /* smitimeoutmask - Bits[6:6], RWS, default = 1'b1 
       Mask SMI generation on receiving SMI Timeout. (Mask on default)
     */
    UINT32 smitimeoutbasesizeselect : 2;
    /* smitimeoutbasesizeselect - Bits[8:7], RWS, default = 2'h3 
       Selecting the size of the bit base counter 00->6 bits, 01->15 bits, 10->20 bits, 
       11->24 bits 
     */
    UINT32 smitimeoutcountmatch : 6;
    /* smitimeoutcountmatch - Bits[14:9], RWS, default = 6'h3F 
       The value to match the time out counter [9:4]
     */
    UINT32 masterlocktimeoutmask : 1;
    /* masterlocktimeoutmask - Bits[15:15], RWS, default = 1'b0 
       Mask SMI generation on receiving Master Lock Timeout.
     */
    UINT32 masterlocktimeoutbasesizeselect : 2;
    /* masterlocktimeoutbasesizeselect - Bits[17:16], RWS, default = 2'h3 
       Selecting the size of the bit base counter 00->6 bits, 01->15 bits, 10->20 bits, 
       11->24 bits 
     */
    UINT32 masterlocktimeoutcountmatch : 6;
    /* masterlocktimeoutcountmatch - Bits[23:18], RWS, default = 6'h3F 
       The value to match the time out counter [9:4]
     */
    UINT32 uboxerrortopcumask : 1;
    /* uboxerrortopcumask - Bits[24:24], RWS, default = 1'b1 
       Set 1 to mask direct MCA from Ubox local error to PCU
     */
    UINT32 rsvd_25 : 6;
    /* rsvd_25 - Bits[30:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 nmibiosenable : 1;
    /* nmibiosenable - Bits[31:31], RWS, default = 1'b1 
       Set to 1 to enable signalling of pin-based NMI to BIOS
     */
  } Bits;
  UINT32 Data;
} UBOXERRCTL_UBOX_CFG_STRUCT;


/* UBOXERRMISC_UBOX_CFG_REG supported on:                                     */
/*       SKX_A0 (0x400400EC)                                                  */
/*       SKX (0x400400EC)                                                     */
/* Register default value:              0x00000000                            */
#define UBOXERRMISC_UBOX_CFG_REG 0x130040EC
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Logging of additional error information. 
 */
typedef union {
  struct {
    UINT32 opcode : 4;
    /* opcode - Bits[3:0], RWS_V, default = 4'b0000 
       Opcode(inbound BL ring) logged at event of poison reception/unsupport 
       opcode/misaligned CFG access. 
     */
    UINT32 rtid : 11;
    /* rtid - Bits[14:4], RWS_V, default = 11'b00000000000 
       RTID(inbound BL ring) logged at event of poison reception/unsupport 
       opcode/misaligned CFG access. 
     */
    UINT32 rnid : 6;
    /* rnid - Bits[20:15], RWS_V, default = 6'b000000 
       RNID(inbound BL ring) logged at event of poison reception/unsupport 
       opcode/misaligned CFG access. 
     */
    UINT32 src : 2;
    /* src - Bits[22:21], RWS_V, default = 6'b000000 
       Source(inbound BL ring) logged at event of poison reception/unsupport 
       opcode/misaligned CFG access. 
     */
    UINT32 address_8_0 : 9;
    /* address_8_0 - Bits[31:23], RWS_V, default = 9'b000000000 
       address[8:0](inbound BL ring) logged at event of poison reception/unsupport 
       opcode/misaligned CFG access. 
     */
  } Bits;
  UINT32 Data;
} UBOXERRMISC_UBOX_CFG_STRUCT;


/* UBOXERRMISC2_UBOX_CFG_REG supported on:                                    */
/*       SKX_A0 (0x400400F0)                                                  */
/*       SKX (0x400400F0)                                                     */
/* Register default value:              0x00000000                            */
#define UBOXERRMISC2_UBOX_CFG_REG 0x130040F0
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * Logging of additional error information. 
 */
typedef union {
  struct {
    UINT32 address_40_9 : 32;
    /* address_40_9 - Bits[31:0], RWS_V, default = 32'h00000000 
       address[40:9](inbound BL ring) logged at event of poison reception/unsupport 
       opcode/misaligned CFG access. 
     */
  } Bits;
  UINT32 Data;
} UBOXERRMISC2_UBOX_CFG_STRUCT;


/* BIOSSTICKYLOCKBYPASSSCRATCHPAD0_UBOX_CFG_REG supported on:                 */
/*       SKX_A0 (0x400400F4)                                                  */
/*       SKX (0x400400F4)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSTICKYLOCKBYPASSSCRATCHPAD0_UBOX_CFG_REG 0x130040F4
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * BIOS SMM handler modifies the register contents during CPU HotAdd_Sparing RAS 
 * events. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS_LB, default = 32'b00000000000000000000000000000000 
       Usage - checkpoints to see where in the process the new socket is at.
     */
  } Bits;
  UINT32 Data;
} BIOSSTICKYLOCKBYPASSSCRATCHPAD0_UBOX_CFG_STRUCT;


/* BIOSSTICKYLOCKBYPASSSCRATCHPAD1_UBOX_CFG_REG supported on:                 */
/*       SKX_A0 (0x400400F8)                                                  */
/*       SKX (0x400400F8)                                                     */
/* Register default value:              0x00000000                            */
#define BIOSSTICKYLOCKBYPASSSCRATCHPAD1_UBOX_CFG_REG 0x130040F8
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * BIOS SMM handler modifies the register contents during CPU HotAdd_Sparing RAS 
 * events. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RWS_LB, default = 32'b00000000000000000000000000000000 
       Usage - checkpoints to see where in the process the new socket is at.
     */
  } Bits;
  UINT32 Data;
} BIOSSTICKYLOCKBYPASSSCRATCHPAD1_UBOX_CFG_STRUCT;


/* PXPENHCAP_UBOX_CFG_REG supported on:                                       */
/*       SKX_A0 (0x40040100)                                                  */
/*       SKX (0x40040100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_UBOX_CFG_REG 0x13004100
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
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
} PXPENHCAP_UBOX_CFG_STRUCT;


/* IIO_GSYSMAP_UBOX_CFG_REG supported on:                                     */
/*       SKX_A0 (0x40040200)                                                  */
/*       SKX (0x40040200)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GSYSMAP_UBOX_CFG_REG 0x13004200
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 *  This register maps the error severity detected by the IIO to on of the system 
 * events. When an error is detected by the IIO, its corresponding error severity 
 * determines which system event to generate according to this register. 
 */
typedef union {
  struct {
    UINT32 severity0_map : 3;
    /* severity0_map - Bits[2:0], RWS_LB, default = 3'b000 
       
       010: Generate U2C NMI event to local cores only 
       001: Generate CSMI VLW
       000: No inband message
       Others: Reserved
               
     */
    UINT32 severity1_map : 3;
    /* severity1_map - Bits[5:3], RWS_LB, default = 3'b000 
       
       010: Generate U2C NMI event to local cores only 
       001: Generate CSMI VLW
       000: No inband message
       Others: Reserved
               
     */
    UINT32 severity2_map : 3;
    /* severity2_map - Bits[8:6], RWS_LB, default = 3'b000 
       
       010: Generate U2C NMI event to local cores only 
       001: Generate CSMI VLW
       000: No inband message
       Others: Reserved
               
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IIO_GSYSMAP_UBOX_CFG_STRUCT;


/* IIO_GSYSST_UBOX_CFG_REG supported on:                                      */
/*       SKX_A0 (0x40040204)                                                  */
/*       SKX (0x40040204)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GSYSST_UBOX_CFG_REG 0x13004204
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 *  
 * This register indicates the error severity signaled by the IIO global error 
 * logic. Setting of an individual error status bit indicates that the 
 * corresponding error severity has been detected by the IIO. 
 */
typedef union {
  struct {
    UINT32 severity0 : 1;
    /* severity0 - Bits[0:0], ROS_V, default = 1'b0 
       
       When set, IIO has detected an error of error severity 0
               
     */
    UINT32 severity1 : 1;
    /* severity1 - Bits[1:1], ROS_V, default = 1'b0 
       
       When set, IIO has detected an error of error severity 1
               
     */
    UINT32 severity2 : 1;
    /* severity2 - Bits[2:2], ROS_V, default = 1'b0 
       
       When set, IIO has detected an error of error severity 2
               
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IIO_GSYSST_UBOX_CFG_STRUCT;


/* IIO_GSYSCTL_UBOX_CFG_REG supported on:                                     */
/*       SKX_A0 (0x40040208)                                                  */
/*       SKX (0x40040208)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GSYSCTL_UBOX_CFG_REG 0x13004208
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * The system event control register controls/masks the reporting the errors 
 * indicated by the system event status register. When cleared, the error severity 
 * does not cause the generation of the system event. When set, detection of the 
 * error severity generates system event(s) according to system event map register 
 * (SYSMAP). 
 */
typedef union {
  struct {
    UINT32 severity0_en : 1;
    /* severity0_en - Bits[0:0], RW_LB, default = 1'b0 
       
       When set, IIO has enabled an error of error severity 0
               
     */
    UINT32 severity1_en : 1;
    /* severity1_en - Bits[1:1], RW_LB, default = 1'b0 
       
       When set, IIO has enabled an error of error severity 1
               
     */
    UINT32 severity2_en : 1;
    /* severity2_en - Bits[2:2], RW_LB, default = 1'b0 
       
       When set, IIO has enabled an error of error severity 2
               
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IIO_GSYSCTL_UBOX_CFG_STRUCT;


/* IIO_ERRPINCTL_UBOX_CFG_REG supported on:                                   */
/*       SKX_A0 (0x4004020C)                                                  */
/*       SKX (0x4004020C)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_ERRPINCTL_UBOX_CFG_REG 0x1300420C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This register provides the option to configure an error pin to either as a 
 * special purpose error pin which is asserted based on the detected error 
 * severity, or as a general purpose output which is asserted based on the value in 
 * the ERRPINDAT. The assertion of the error pins can also be completely disabled 
 * by this register. 
 */
typedef union {
  struct {
    UINT32 pin0 : 2;
    /* pin0 - Bits[1:0], RW_LB, default = 2'b00 
       
       11: Reserved.
       10: Assert Error Pin when error severity 0 is set in the system event status 
       reg. 
       01: Assert and Deassert Error pin according to error pin data register.
       00: Disable Error pin assertion
               
     */
    UINT32 pin1 : 2;
    /* pin1 - Bits[3:2], RW_LB, default = 2'b00 
       
       11: Reserved.
       10: Assert Error Pin when error severity 1 is set in the system event status 
       reg. 
       01: Assert and Deassert Error pin according to error pin data register.
       00: Disable Error pin assertion
               
     */
    UINT32 pin2 : 2;
    /* pin2 - Bits[5:4], RW_LB, default = 2'b00 
       
       11: Reserved.
       10: Assert Error Pin when error severity 2 is set in the system event status 
       reg. 
       01: Assert and Deassert Error pin according to error pin data register.
       00: Disable Error pin assertion
               
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IIO_ERRPINCTL_UBOX_CFG_STRUCT;


/* IIO_ERRPINSTS_UBOX_CFG_REG supported on:                                   */
/*       SKX_A0 (0x40040210)                                                  */
/*       SKX (0x40040210)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_ERRPINSTS_UBOX_CFG_REG 0x13004210
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * This register holds the error pin status.
 */
typedef union {
  struct {
    UINT32 err0 : 1;
    /* err0 - Bits[0:0], RW1CS_LBV, default = 1'b0 
       
       State of ERROR[0] pin (the register value is active high, the socket pin is 
       active VSS) 
               
     */
    UINT32 err1 : 1;
    /* err1 - Bits[1:1], RW1CS_LBV, default = 1'b0 
       
       State of ERROR[1] pin (the egister value is active high, the socket pin is 
       active VSS) 
               
     */
    UINT32 err2 : 1;
    /* err2 - Bits[2:2], RW1CS_LBV, default = 1'b0 
       
       State of ERROR[2] pin (the egister value is active high, the socket pin is 
       active VSS) 
               
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IIO_ERRPINSTS_UBOX_CFG_STRUCT;


/* IIO_ERRPINDAT_UBOX_CFG_REG supported on:                                   */
/*       SKX_A0 (0x40040214)                                                  */
/*       SKX (0x40040214)                                                     */
/* Register default value:              0x00000007                            */
#define IIO_ERRPINDAT_UBOX_CFG_REG 0x13004214
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * This register provides the data value when the error pin is configured as a 
 * general purpose output. 
 */
typedef union {
  struct {
    UINT32 pin0 : 1;
    /* pin0 - Bits[0:0], RW_LB, default = 1'b1 
       
       This bit acts as the general purpose output for the Error[0] pin. Software 
       sets/clears this bit to assert/deassert Error[0] pin. This bit applies only when 
       ERRPINCTL[5:4]=01; otherwise it is reserved. 
       0: Assert ERR#[0] pin (drive low)
       1: Deassert ERR#[0] pin (float high)
       Notes:
       This pin is open drain and must be pulled high by external resistor when 
       deasserted. 
       BIOS needs to write 1 to this bit for security reasons if this register is not 
       used. 
               
     */
    UINT32 pin1 : 1;
    /* pin1 - Bits[1:1], RW_LB, default = 1'b1 
       
       This bit acts as the general purpose output for the Error[1] pin. Software 
       sets/clears this bit to assert/deassert Error[1] pin. This bit applies only when 
       ERRPINCTL[5:4]=01; otherwise it is reserved. 
       0: Assert ERR#[1] pin (drive low)
       1: Deassert ERR#[1] pin (float high)
       Notes:
       This pin is open drain and must be pulled high by external resistor when 
       deasserted. 
       BIOS needs to write 1 to this bit for security reasons if this register is not 
       used. 
               
     */
    UINT32 pin2 : 1;
    /* pin2 - Bits[2:2], RW_LB, default = 1'b1 
       
       This bit acts as the general purpose output for the Error[2] pin. Software 
       sets/clears this bit to assert/deassert Error[2] pin. This bit applies only when 
       ERRPINCTL[5:4]=01; otherwise it is reserved. 
       0: Assert ERR#[2] pin (drive low)
       1: Deassert ERR#[2] pin (float high)
       Notes:
       This pin is open drain and must be pulled high by external resistor when 
       deasserted. 
       BIOS needs to write 1 to this bit for security reasons if this register is not 
       used. 
               
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IIO_ERRPINDAT_UBOX_CFG_STRUCT;


/* IIO_GF_ERRST_UBOX_CFG_REG supported on:                                    */
/*       SKX_A0 (0x40040218)                                                  */
/*       SKX (0x40040218)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GF_ERRST_UBOX_CFG_REG 0x13004218
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This register indicates the fatal error reported to the IIO global error logic. 
 * An individual error status bit that is set indicates that local IIO stack 
 * interface has detected an error. 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RW1CS_LBV, default = 32'h00000000 
       
       Bit map status of fatal error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GF_ERRST_UBOX_CFG_STRUCT;


/* IIO_GF_1ST_ERRST_UBOX_CFG_REG supported on:                                */
/*       SKX_A0 (0x4004021C)                                                  */
/*       SKX (0x4004021C)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GF_1ST_ERRST_UBOX_CFG_REG 0x1300421C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This field logs the global error status register content per iio stack when the 
 * first fatal error is reported. This has the same format as the global error 
 * status register (GF_ERRST). 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RO_V, default = 32'h00000000 
       
       Bit map status of 1st fatal error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GF_1ST_ERRST_UBOX_CFG_STRUCT;


/* IIO_GF_NXT_ERRST_UBOX_CFG_REG supported on:                                */
/*       SKX_A0 (0x40040220)                                                  */
/*       SKX (0x40040220)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GF_NXT_ERRST_UBOX_CFG_REG 0x13004220
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This filed logs the global error status register content per iio stack when the 
 * next fatal error is reported. This has the same format as the global error 
 * status register (GF_ERRST). 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RO_V, default = 32'h00000000 
       
       Bit map status of next fatal error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GF_NXT_ERRST_UBOX_CFG_STRUCT;


/* IIO_GNF_ERRST_UBOX_CFG_REG supported on:                                   */
/*       SKX_A0 (0x40040224)                                                  */
/*       SKX (0x40040224)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GNF_ERRST_UBOX_CFG_REG 0x13004224
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This register indicates the non-fatal error reported to the IIO global error 
 * logic. An individual error status bit that is set indicates that local IIO stack 
 * interface has detected an error. 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RW1CS_LBV, default = 32'h00000000 
       
       Bit map status of non-fatal error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GNF_ERRST_UBOX_CFG_STRUCT;


/* IIO_GNF_1ST_ERRST_UBOX_CFG_REG supported on:                               */
/*       SKX_A0 (0x40040228)                                                  */
/*       SKX (0x40040228)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GNF_1ST_ERRST_UBOX_CFG_REG 0x13004228
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This filed logs the global error status register content per iio stack when the 
 * first non-fatal error is reported. This has the same format as the global error 
 * status register (GNF_ERRST). 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RO_V, default = 32'h00000000 
       
       Bit map status of 1st non-fatal error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GNF_1ST_ERRST_UBOX_CFG_STRUCT;


/* IIO_GNF_NXT_ERRST_UBOX_CFG_REG supported on:                               */
/*       SKX_A0 (0x4004022C)                                                  */
/*       SKX (0x4004022C)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GNF_NXT_ERRST_UBOX_CFG_REG 0x1300422C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This filed logs the global error status register content per iio stack when the 
 * subsequent non-fatal error is reported. This has the same format as the global 
 * error status register (GNF_ERRST). 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RO_V, default = 32'h00000000 
       
       Bit map status of next non-fatal error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GNF_NXT_ERRST_UBOX_CFG_STRUCT;


/* IIO_GC_ERRST_UBOX_CFG_REG supported on:                                    */
/*       SKX_A0 (0x40040230)                                                  */
/*       SKX (0x40040230)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GC_ERRST_UBOX_CFG_REG 0x13004230
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This register indicates the correctable error reported to the IIO global error 
 * logic. An individual error status bit that is set indicates that local IIO stack 
 * interface has detected an error. 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RW1CS_LBV, default = 32'h00000000 
       
       Bit map status of correctable error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GC_ERRST_UBOX_CFG_STRUCT;


/* IIO_GC_1ST_ERRST_UBOX_CFG_REG supported on:                                */
/*       SKX_A0 (0x40040234)                                                  */
/*       SKX (0x40040234)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GC_1ST_ERRST_UBOX_CFG_REG 0x13004234
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This filed logs the global error status register content per iio stack when the 
 * first correctable error is reported. This has the same format as the global 
 * error status register (GC_ERRST). 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RO_V, default = 32'h00000000 
       
       Bit map status of 1st correctable error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GC_1ST_ERRST_UBOX_CFG_STRUCT;


/* IIO_GC_NXT_ERRST_UBOX_CFG_REG supported on:                                */
/*       SKX_A0 (0x40040238)                                                  */
/*       SKX (0x40040238)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GC_NXT_ERRST_UBOX_CFG_REG 0x13004238
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This filed logs the global error status register content per iio stack when the 
 * subsequent correctable error is reported. This has the same format as the global 
 * error status register (GC_ERRST). 
 *      00 Stack0/C-Stack 
 *      01 Stack1/P-Stack 
 *      02 Stack2/P-Stack 
 *      03 Stack3/P-Stack 
 *      04 Stack4/P-Stack
 * 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RO_V, default = 32'h00000000 
       
       Bit map status of next correctable error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GC_NXT_ERRST_UBOX_CFG_STRUCT;


/* IIO_GF_ERRMSK_UBOX_CFG_REG supported on:                                   */
/*       SKX_A0 (0x4004023C)                                                  */
/*       SKX (0x4004023C)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GF_ERRMSK_UBOX_CFG_REG 0x1300423C
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This register is bit mask of the fatal error reported to the IIO global error 
 * logic. 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RW_LB, default = 32'h00000000 
       
       Bit map mask of fatal error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GF_ERRMSK_UBOX_CFG_STRUCT;


/* IIO_GNF_ERRMSK_UBOX_CFG_REG supported on:                                  */
/*       SKX_A0 (0x40040240)                                                  */
/*       SKX (0x40040240)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GNF_ERRMSK_UBOX_CFG_REG 0x13004240
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This register is bit mask of the non-fatal error reported to the IIO global 
 * error logic. 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RW_LB, default = 32'h00000000 
       
       Bit map mask of non-fatal error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GNF_ERRMSK_UBOX_CFG_STRUCT;


/* IIO_GC_ERRMSK_UBOX_CFG_REG supported on:                                   */
/*       SKX_A0 (0x40040244)                                                  */
/*       SKX (0x40040244)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_GC_ERRMSK_UBOX_CFG_REG 0x13004244
/* Struct format extracted from XML file SKX\0.8.0.CFG.xml.
 * 
 * This register is bit mask of the non-fatal error reported to the IIO global 
 * error logic. 
 */
typedef union {
  struct {
    UINT32 bitmap : 32;
    /* bitmap - Bits[31:0], RW_LB, default = 32'h00000000 
       
       Bit map mask of correctable error 
               
     */
  } Bits;
  UINT32 Data;
} IIO_GC_ERRMSK_UBOX_CFG_STRUCT;


#endif /* UBOX_CFG_h */
