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

#ifndef MC_MAINEXT_h
#define MC_MAINEXT_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* MC_MAINEXT_DEV 10                                                          */
/* MC_MAINEXT_FUN 4                                                           */

/* VID_MC_MAINEXT_REG supported on:                                           */
/*       SKX_A0 (0x20254000)                                                  */
/*       SKX (0x20254000)                                                     */
/* Register default value:              0x8086                                */
#define VID_MC_MAINEXT_REG 0x07032000
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} VID_MC_MAINEXT_STRUCT;


/* DID_MC_MAINEXT_REG supported on:                                           */
/*       SKX_A0 (0x20254002)                                                  */
/*       SKX (0x20254002)                                                     */
/* Register default value:              0x2044                                */
#define DID_MC_MAINEXT_REG 0x07032002
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2044 
        
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
} DID_MC_MAINEXT_STRUCT;


/* PCICMD_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x20254004)                                                  */
/*       SKX (0x20254004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_MC_MAINEXT_REG 0x07032004
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} PCICMD_MC_MAINEXT_STRUCT;


/* PCISTS_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x20254006)                                                  */
/*       SKX (0x20254006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_MC_MAINEXT_REG 0x07032006
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} PCISTS_MC_MAINEXT_STRUCT;


/* RID_MC_MAINEXT_REG supported on:                                           */
/*       SKX_A0 (0x10254008)                                                  */
/*       SKX (0x10254008)                                                     */
/* Register default value:              0x00                                  */
#define RID_MC_MAINEXT_REG 0x07031008
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} RID_MC_MAINEXT_STRUCT;


/* CCR_N0_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x10254009)                                                  */
/*       SKX (0x10254009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_MC_MAINEXT_REG 0x07031009
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_MC_MAINEXT_STRUCT;


/* CCR_N1_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x2025400A)                                                  */
/*       SKX (0x2025400A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_MC_MAINEXT_REG 0x0703200A
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} CCR_N1_MC_MAINEXT_STRUCT;


/* CLSR_MC_MAINEXT_REG supported on:                                          */
/*       SKX_A0 (0x1025400C)                                                  */
/*       SKX (0x1025400C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_MC_MAINEXT_REG 0x0703100C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} CLSR_MC_MAINEXT_STRUCT;


/* PLAT_MC_MAINEXT_REG supported on:                                          */
/*       SKX_A0 (0x1025400D)                                                  */
/*       SKX (0x1025400D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_MC_MAINEXT_REG 0x0703100D
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} PLAT_MC_MAINEXT_STRUCT;


/* HDR_MC_MAINEXT_REG supported on:                                           */
/*       SKX_A0 (0x1025400E)                                                  */
/*       SKX (0x1025400E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_MC_MAINEXT_REG 0x0703100E
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} HDR_MC_MAINEXT_STRUCT;


/* BIST_MC_MAINEXT_REG supported on:                                          */
/*       SKX_A0 (0x1025400F)                                                  */
/*       SKX (0x1025400F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_MC_MAINEXT_REG 0x0703100F
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} BIST_MC_MAINEXT_STRUCT;


/* SVID_MC_MAINEXT_REG supported on:                                          */
/*       SKX_A0 (0x2025402C)                                                  */
/*       SKX (0x2025402C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_MC_MAINEXT_REG 0x0703202C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * generated by critter 19_2_0x02c
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_MC_MAINEXT_STRUCT;


/* SDID_MC_MAINEXT_REG supported on:                                          */
/*       SKX_A0 (0x2025402E)                                                  */
/*       SKX (0x2025402E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_MC_MAINEXT_REG 0x0703202E
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * generated by critter 19_2_0x02e
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_MC_MAINEXT_STRUCT;


/* CAPPTR_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x10254034)                                                  */
/*       SKX (0x10254034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_MC_MAINEXT_REG 0x07031034
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} CAPPTR_MC_MAINEXT_STRUCT;


/* INTL_MC_MAINEXT_REG supported on:                                          */
/*       SKX_A0 (0x1025403C)                                                  */
/*       SKX (0x1025403C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_MC_MAINEXT_REG 0x0703103C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} INTL_MC_MAINEXT_STRUCT;


/* INTPIN_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x1025403D)                                                  */
/*       SKX (0x1025403D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_MC_MAINEXT_REG 0x0703103D
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} INTPIN_MC_MAINEXT_STRUCT;


/* MINGNT_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x1025403E)                                                  */
/*       SKX (0x1025403E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_MC_MAINEXT_REG 0x0703103E
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} MINGNT_MC_MAINEXT_STRUCT;


/* MAXLAT_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x1025403F)                                                  */
/*       SKX (0x1025403F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_MC_MAINEXT_REG 0x0703103F
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} MAXLAT_MC_MAINEXT_STRUCT;


/* PXPCAP_MC_MAINEXT_REG supported on:                                        */
/*       SKX_A0 (0x40254040)                                                  */
/*       SKX (0x40254040)                                                     */
/* Register default value:              0x00910010                            */
#define PXPCAP_MC_MAINEXT_REG 0x07034040
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * generated by critter 19_2_0x040
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
} PXPCAP_MC_MAINEXT_STRUCT;


/* DIMMMTR_0_MC_MAINEXT_REG supported on:                                     */
/*       SKX_A0 (0x40254080)                                                  */
/*       SKX (0x40254080)                                                     */
/* Register default value:              0x0000000C                            */
#define DIMMMTR_0_MC_MAINEXT_REG 0x07034080
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Per-DIMM configurability for DIMM 0.
 */
typedef union {
  struct {
    UINT32 ca_width : 2;
    /* ca_width - Bits[1:0], RWS_LB, default = 2'b00 
       00 - 10 bits
       01 - 11 bits
       10 - 12 bits
       11 - reserved
     */
    UINT32 ra_width : 3;
    /* ra_width - Bits[4:2], RWS_LB, default = 3'b011 
       000 - reserved 
       001 - 13 bits
       010 - 14 bits
       011 - 15 bits
       100 - 16 bits
       101 - 17 bits
       110 - 18 bits
       111: reserved
     */
    UINT32 ddr3_dnsty : 3;
    /* ddr3_dnsty - Bits[7:5], RWS_LB, default = 3'b000 
       000 - Reserved
       001 - 2Gb
       010 - 4Gb
       011 - 8Gb
       100 - 16Gb
       101 - 12Gb
       This field applies to ddr4.
       
     */
    UINT32 ddr3_width : 2;
    /* ddr3_width - Bits[9:8], RWS_LB, default = 2'b00 
       00 - x4
       01 - x8
       10 - x16
       11 - reserved
       This field applies to ddr4.
       
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank_cnt : 2;
    /* rank_cnt - Bits[13:12], RWS_LB, default = 2'b00 
       
       00 - SR
       01 - DR
       10 - QR
       11 - reserved
       
     */
    UINT32 rsvd_14 : 1;
    /* rsvd_14 - Bits[14:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dimm_pop : 1;
    /* dimm_pop - Bits[15:15], RWS_LB, default = 1'b0 
       
                   DDR4 DIMM populated if set; otherwise, unpopulated.
                   Should be set to 0 when this slot is populated with a DDR-T DIMM.
               
     */
    UINT32 rank_disable : 4;
    /* rank_disable - Bits[19:16], RWS_LB, default = 4'b0000 
       RANK Disable Control to disable refresh and ZQCAL operation. This bit setting 
       must be set consistently with TERM_RNK_MSK, i.e. both corresponding bits cannot 
       be set at the same time. In the other word, a disabled rank must not be selected 
       for the terminaton rank. 
       RANK_DISABLE[3], i.e. bit 19: rank 3 disable. Note DIMMMTR_2.RANK_DISABLE[3] is 
       don't care since DIMM 2 must not be quad-rank 
       RANK_DISABLE[2], i.e. bit 18: rank 2 disable. Note DIMMMTR_2.RANK_DISABLE[2] is 
       don't care since DIMM 2 must not be quad-rank 
       RANK_DISABLE[1], i.e. bit 17: rank 1 disable
       RANK_DISABLE[0], i.e. bit 16: rank 0 disable
       when set, no refresh will be perform on this rank. ODT termination is not 
       affected by this bit. 
       Note that patrols are disabled by dimm*_pat_rank_disable of amap register now 
       and not affected by this bit field. 
     */
    UINT32 rsvd_20 : 1;
    /* rsvd_20 - Bits[20:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 hdrl : 1;
    /* hdrl - Bits[21:21], RWS_LB, default = 1'b0 
       When set, will enable High Density Reduced Load mode which will transmit Row 
       address bits 17:16 on chip select lines 
       7:6 and 3:2.
     */
    UINT32 hdrl_parity : 1;
    /* hdrl_parity - Bits[22:22], RWS_LB, default = 1'b0 
       When set, will enable parity calculation to include address bits 17:16 which are 
       sent on chip select lines 
       7:6 and 3:2.
     */
    UINT32 ddr4_3dsnumranks_cs : 2;
    /* ddr4_3dsnumranks_cs - Bits[24:23], RWS_LB, default = 2'b00 
       Number of sub ranks per chip select per dimm of DDR4 3DS and non3ds_lrdimm (this 
       is a dimm 
       specific field. There are restriction on rank_cnt field if this field is nonzero 
       fro 3ds devices. 
       Can not mix 2 chip_select parts and 1 chip_select part on the same channel)
       00- 3ds/non3ds_lrdimm ddr4 multiple ranks/chip select disabled
       01- 2 ranks per chip select
       10- 4 ranks per chip select
       11- 8 ranks per chip select
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMMMTR_0_MC_MAINEXT_STRUCT;


/* DIMMMTR_1_MC_MAINEXT_REG supported on:                                     */
/*       SKX_A0 (0x40254084)                                                  */
/*       SKX (0x40254084)                                                     */
/* Register default value:              0x0000000C                            */
#define DIMMMTR_1_MC_MAINEXT_REG 0x07034084
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Option 1 indicates the option 1 mapping is automatically selected when 3rd DIMM 
 * slot (DS#2) is 
 * unpopulated (DIMMMTR2), including single, dual, and quad-rank DIMMs in any of 
 * the 2 slots. Default 
 * mapping is automatically selected any time a DIMM is loaded in the 3th slot 
 * (DS#2). 
 */
typedef union {
  struct {
    UINT32 ca_width : 2;
    /* ca_width - Bits[1:0], RWS_LB, default = 2'b00 
       00 - 10 bits
       01 - 11 bits
       10 - 12 bits
       11 - reserved
     */
    UINT32 ra_width : 3;
    /* ra_width - Bits[4:2], RWS_LB, default = 3'b011 
       000 - reserved 
       001 - 13 bits
       010 - 14 bits
       011 - 15 bits
       100 - 16 bits
       101 - 17 bits
       110 - 18 bits
       111: reserved
     */
    UINT32 ddr3_dnsty : 3;
    /* ddr3_dnsty - Bits[7:5], RWS_LB, default = 3'b000 
       000 - Reserved
       001 - 2Gb
       010 - 4Gb
       011 - 8Gb
       100 - 16Gb
       101 - 12Gb
       This field applies to ddr4.
       
     */
    UINT32 ddr3_width : 2;
    /* ddr3_width - Bits[9:8], RWS_LB, default = 2'b00 
       00 - x4
       01 - x8
       10 - x16
       11 - reserved
       This field applies to ddr4.
       
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank_cnt : 2;
    /* rank_cnt - Bits[13:12], RWS_LB, default = 2'b00 
       
       00 - SR
       01 - DR
       10 - QR
       11 - reserved
       
     */
    UINT32 rsvd_14 : 1;
    /* rsvd_14 - Bits[14:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dimm_pop : 1;
    /* dimm_pop - Bits[15:15], RWS_LB, default = 1'b0 
       
                   DDR4 DIMM populated if set; otherwise, unpopulated.
                   Should be set to 0 when this slot is populated with a DDR-T DIMM.
               
     */
    UINT32 rank_disable : 4;
    /* rank_disable - Bits[19:16], RWS_LB, default = 4'b0000 
       RANK Disable Control to disable refresh and ZQCAL operation. This bit setting 
       must be set consistently with TERM_RNK_MSK, i.e. both corresponding bits cannot 
       be set at the same time. In the other word, a disabled rank must not be selected 
       for the terminaton rank. 
       RANK_DISABLE[3], i.e. bit 19: rank 3 disable. Note DIMMMTR_2.RANK_DISABLE[3] is 
       don't care since DIMM 2 must not be quad-rank 
       RANK_DISABLE[2], i.e. bit 18: rank 2 disable. Note DIMMMTR_2.RANK_DISABLE[2] is 
       don't care since DIMM 2 must not be quad-rank 
       RANK_DISABLE[1], i.e. bit 17: rank 1 disable
       RANK_DISABLE[0], i.e. bit 16: rank 0 disable
       when set, no refresh will be perform on this rank. ODT termination is not 
       affected by this bit. 
       Note that patrols are disabled by dimm*_pat_rank_disable of amap register now 
       and not affected by this bit field. 
     */
    UINT32 rsvd_20 : 3;
    /* rsvd_20 - Bits[22:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddr4_3dsnumranks_cs : 2;
    /* ddr4_3dsnumranks_cs - Bits[24:23], RWS_LB, default = 2'b00  */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMMMTR_1_MC_MAINEXT_STRUCT;


/* DIMMMTR_2_MC_MAINEXT_REG supported on:                                     */
/*       SKX_A0 (0x40254088)                                                  */
/*       SKX (0x40254088)                                                     */
/* Register default value:              0x0000000C                            */
#define DIMMMTR_2_MC_MAINEXT_REG 0x07034088
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Option 1 mapping is automatically selected when 3rd DIMM slot (DS#2) is
 * unpopulated (DIMMMTR2), including single, dual, and quad-rank DIMMs in any of 
 * the 2 slots. Default 
 * mapping is automatically selected any time a DIMM is loaded in the 3th slot 
 * (DS#2). 
 */
typedef union {
  struct {
    UINT32 ca_width : 2;
    /* ca_width - Bits[1:0], RWS_LB, default = 2'b00 
       00 - 10 bits
       01 - 11 bits
       10 - 12 bits
       11 - reserved
     */
    UINT32 ra_width : 3;
    /* ra_width - Bits[4:2], RWS_LB, default = 3'b011 
       000 - reserved 
       001 - 13 bits
       010 - 14 bits
       011 - 15 bits
       100 - 16 bits
       101 - 17 bits
       110 - 18 bits
       111: reserved
     */
    UINT32 ddr3_dnsty : 3;
    /* ddr3_dnsty - Bits[7:5], RWS_LB, default = 3'b000 
       000 - Reserved
       001 - 2Gb
       010 - 4Gb
       011 - 8Gb
       100 - 16Gb
       101 - 12Gb
       This field applies to ddr4.
       
     */
    UINT32 ddr3_width : 2;
    /* ddr3_width - Bits[9:8], RWS_LB, default = 2'b00 
       00 - x4
       01 - x8
       10 - x16
       11 - reserved
       This field applies to ddr4.
       
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank_cnt : 2;
    /* rank_cnt - Bits[13:12], RWS_LB, default = 2'b00 
       
       00 - SR
       01 - DR
       10 - QR
       11 - reserved
       
     */
    UINT32 rsvd_14 : 1;
    /* rsvd_14 - Bits[14:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dimm_pop : 1;
    /* dimm_pop - Bits[15:15], RWS_LB, default = 1'b0 
       
                   DDR4 DIMM populated if set; otherwise, unpopulated.
                   Should be set to 0 when this slot is populated with a DDR-T DIMM.
               
     */
    UINT32 rank_disable : 4;
    /* rank_disable - Bits[19:16], RWS_LB, default = 4'b0000 
       RANK Disable Control to disable refresh and ZQCAL operation. This bit setting 
       must be set consistently with TERM_RNK_MSK, i.e. both corresponding bits cannot 
       be set at the same time. In the other word, a disabled rank must not be selected 
       for the terminaton rank. 
       RANK_DISABLE[3], i.e. bit 19: rank 3 disable. Note DIMMMTR_2.RANK_DISABLE[3] is 
       don't care since DIMM 2 must not be quad-rank 
       RANK_DISABLE[2], i.e. bit 18: rank 2 disable. Note DIMMMTR_2.RANK_DISABLE[2] is 
       don't care since DIMM 2 must not be quad-rank 
       RANK_DISABLE[1], i.e. bit 17: rank 1 disable
       RANK_DISABLE[0], i.e. bit 16: rank 0 disable
       when set, no refresh will be perform on this rank. ODT termination is not 
       affected by this bit. 
       Note that patrols are disabled by dimm*_pat_rank_disable of amap register now 
       and not affected by this bit field. 
     */
    UINT32 rsvd_20 : 3;
    /* rsvd_20 - Bits[22:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddr4_3dsnumranks_cs : 2;
    /* ddr4_3dsnumranks_cs - Bits[24:23], RWS_LB, default = 2'b00  */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMMMTR_2_MC_MAINEXT_STRUCT;


/* AMAP_MC_MAINEXT_REG supported on:                                          */
/*       SKX_A0 (0x4025408C)                                                  */
/*       SKX (0x4025408C)                                                     */
/* Register default value:              0x00000000                            */
#define AMAP_MC_MAINEXT_REG 0x0703408C
/* Struct format extracted from XML file SKX_A0\2.10.4.CFG.xml.
 * address map variations.
 */
typedef union {
  struct {
    UINT32 fine_grain_bank_interleaving : 1;
    /* fine_grain_bank_interleaving - Bits[0:0], RW_LB, default = 1'b0 
       Enable DDR4 fine grain bank interleaving. 
     */
    UINT32 force_lat : 1;
    /* force_lat - Bits[1:1], RW_LB, default = 1'b0 
       When setting to 1, force additional 1 cycle latency in decoding logic (same 
       latency as 3 channel mode); expected to set to 1 when channel mirroring is 
       enabled. 
     */
    UINT32 rsvd_2 : 13;
    /* rsvd_2 - Bits[14:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dimm0_pat_rank_disable : 4;
    /* dimm0_pat_rank_disable - Bits[18:15], RW_LB, default = 4'b0000 
       This field is similar to dimmmtr_*.rank_disable; but it affects only patrol 
       operations. 
       Each bit controls 1 Chip_select's patrol in a given dimm. If 1, patrol engine 
       will skip that 
       ChipSelect; other wise it will patrol it if dimm is populted and all other 
       patrol conditions are 
       true. This is intended to be used after rank_sparing to skip patrol of the 
       spared rank. 
     */
    UINT32 dimm1_pat_rank_disable : 4;
    /* dimm1_pat_rank_disable - Bits[22:19], RW_LB, default = 4'b0000 
       This field is similar to dimmmtr_*.rank_disable; but it affects only patrol 
       operations. 
       Each bit controls 1 Chip_select's patrol in a given dimm. If 1, patrol engine 
       will skip that 
       ChipSelect; other wise it will patrol it if dimm is populted and all other 
       patrol conditions are 
       true. This is intended to be used after rank_sparing to skip patrol of the 
       spared rank. 
     */
    UINT32 dimm2_pat_rank_disable : 4;
    /* dimm2_pat_rank_disable - Bits[26:23], RW_LB, default = 4'b0000 
       This field is similar to dimmmtr_*.rank_disable; but it affects only patrol 
       operations. 
       Each bit controls 1 Chip_select's patrol in a given dimm. If 1, patrol engine 
       will skip that 
       ChipSelect; other wise it will patrol it if dimm is populted and all other 
       patrol conditions are 
       true. This is intended to be used after rank_sparing to skip patrol of the 
       spared rank. 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} AMAP_MC_MAINEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * address map variations.
 */
typedef union {
  struct {
    UINT32 fine_grain_bank_interleaving : 1;
    /* fine_grain_bank_interleaving - Bits[0:0], RW_LB, default = 1'b0 
       Enable DDR4 fine grain bank interleaving. 
     */
    UINT32 force_lat : 1;
    /* force_lat - Bits[1:1], RW_LB, default = 1'b0 
       When setting to 1, force additional 1 cycle latency in decoding logic (same 
       latency as 3 channel mode); expected to set to 1 when channel mirroring is 
       enabled. 
     */
    UINT32 mirr_adddc_en : 1;
    /* mirr_adddc_en - Bits[2:2], RW_LB, default = 1'b0 
       Enabling special spare copy mode for ADDDC and mirroring enable. In this mode, 
       system addresses will be gone through 3 times, with first pass for non-mirror 
       addresses, second pass for mirror primary addresses, and third pass for mirror 
       secondary addresses. This bit should be set to 1 in the channel where spare copy 
       is done. This function will only work if sparing_control.mirr_adddc_en is set to 
       1. 
     */
    UINT32 rsvd_3 : 12;
    /* rsvd_3 - Bits[14:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dimm0_pat_rank_disable : 4;
    /* dimm0_pat_rank_disable - Bits[18:15], RW_LB, default = 4'b0000 
       This field is similar to dimmmtr_*.rank_disable; but it affects only patrol 
       operations. 
       Each bit controls 1 Chip_select's patrol in a given dimm. If 1, patrol engine 
       will skip that 
       ChipSelect; other wise it will patrol it if dimm is populted and all other 
       patrol conditions are 
       true. This is intended to be used after rank_sparing to skip patrol of the 
       spared rank. 
     */
    UINT32 dimm1_pat_rank_disable : 4;
    /* dimm1_pat_rank_disable - Bits[22:19], RW_LB, default = 4'b0000 
       This field is similar to dimmmtr_*.rank_disable; but it affects only patrol 
       operations. 
       Each bit controls 1 Chip_select's patrol in a given dimm. If 1, patrol engine 
       will skip that 
       ChipSelect; other wise it will patrol it if dimm is populted and all other 
       patrol conditions are 
       true. This is intended to be used after rank_sparing to skip patrol of the 
       spared rank. 
     */
    UINT32 dimm2_pat_rank_disable : 4;
    /* dimm2_pat_rank_disable - Bits[26:23], RW_LB, default = 4'b0000 
       This field is similar to dimmmtr_*.rank_disable; but it affects only patrol 
       operations. 
       Each bit controls 1 Chip_select's patrol in a given dimm. If 1, patrol engine 
       will skip that 
       ChipSelect; other wise it will patrol it if dimm is populted and all other 
       patrol conditions are 
       true. This is intended to be used after rank_sparing to skip patrol of the 
       spared rank. 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} AMAP_MC_MAINEXT_STRUCT;



/* TADCHNILVOFFSET_0_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x40254090)                                                  */
/*       SKX (0x40254090)                                                     */
/* Register default value:              0x00000000                            */
#define TADCHNILVOFFSET_0_MC_MAINEXT_REG 0x07034090
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           TAD Interleaves and Offsets
 *           This register should be programmned while training_mode=1 or else the 
 * register write will be missed.   
 *           Will be available 0x4F DCLK cycles after we enter normal mode.
 *       
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00 
        socket interleave wayness
                               00 = 1 way,
                               01 = 2 way,
                               10 = 4 way,
                               11 = 8 way.
                 
     */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00 
        Channel interleave wayness
                               00 = 1 way
                               01 = 2 way
                               10 = 3 way
                 
     */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity. When negative offset is programmed (2's complement), 
       this field only support +/-32T and the upper bit is used to indicate the 
       polarity of the offset. 
     */
    UINT32 rsvd_24 : 2;
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       This field should be removed for SKX.  
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the skt_way interleave
                     b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
                     b'01  256B (based off PA[8] and up)  (used for 1LM only)
                     b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and 
       Block/DDRT_CSR) 
                     b'11   1GB (based off PA[30] and up) {used for PMem only, though 
       the standard PMem mode will be 4KB) 
                 
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the ch_way interleave
                     b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM 
       only) 
                     b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, 
       DDRT Block, and DDRT 2LM.  the latter only in mirror mode) 
                     b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT 
       PMem) 
                     b'11   reserved
                     Note:  using the same ch_granularity encoding for both DDRT and 
       DDR4 even though 64B only applies for DDR4 and 4KB only applies for DDRT. 
                 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TADCHNILVOFFSET_0_MC_MAINEXT_STRUCT;


/* TADCHNILVOFFSET_1_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x40254094)                                                  */
/*       SKX (0x40254094)                                                     */
/* Register default value:              0x00000000                            */
#define TADCHNILVOFFSET_1_MC_MAINEXT_REG 0x07034094
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           TAD Interleaves and Offsets
 *           This register should be programmned while training_mode=1 or else the 
 * register write will be missed.   
 *           Will be available 0x4F DCLK cycles after we enter normal mode.
 *       
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00 
        socket interleave wayness
                               00 = 1 way,
                               01 = 2 way,
                               10 = 4 way,
                               11 = 8 way.
                 
     */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00 
        Channel interleave wayness
                               00 = 1 way
                               01 = 2 way
                               10 = 3 way
                 
     */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity. When negative offset is programmed (2's complement), 
       this field only support +/-32T and the upper bit is used to indicate the 
       polarity of the offset. 
     */
    UINT32 rsvd_24 : 2;
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       This field should be removed for SKX.  
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the skt_way interleave
                     b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
                     b'01  256B (based off PA[8] and up)  (used for 1LM only)
                     b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and 
       Block/DDRT_CSR) 
                     b'11   1GB (based off PA[30] and up) {used for PMem only, though 
       the standard PMem mode will be 4KB) 
                 
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the ch_way interleave
                     b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM 
       only) 
                     b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, 
       DDRT Block, and DDRT 2LM.  the latter only in mirror mode) 
                     b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT 
       PMem) 
                     b'11   reserved
                     Note:  using the same ch_granularity encoding for both DDRT and 
       DDR4 even though 64B only applies for DDR4 and 4KB only applies for DDRT. 
                 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TADCHNILVOFFSET_1_MC_MAINEXT_STRUCT;


/* TADCHNILVOFFSET_2_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x40254098)                                                  */
/*       SKX (0x40254098)                                                     */
/* Register default value:              0x00000000                            */
#define TADCHNILVOFFSET_2_MC_MAINEXT_REG 0x07034098
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           TAD Interleaves and Offsets
 *           This register should be programmned while training_mode=1 or else the 
 * register write will be missed.   
 *           Will be available 0x4F DCLK cycles after we enter normal mode.
 *       
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00 
        socket interleave wayness
                               00 = 1 way,
                               01 = 2 way,
                               10 = 4 way,
                               11 = 8 way.
                 
     */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00 
        Channel interleave wayness
                               00 = 1 way
                               01 = 2 way
                               10 = 3 way
                 
     */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity. When negative offset is programmed (2's complement), 
       this field only support +/-32T and the upper bit is used to indicate the 
       polarity of the offset. 
     */
    UINT32 rsvd_24 : 2;
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       This field should be removed for SKX.  
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the skt_way interleave
                     b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
                     b'01  256B (based off PA[8] and up)  (used for 1LM only)
                     b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and 
       Block/DDRT_CSR) 
                     b'11   1GB (based off PA[30] and up) {used for PMem only, though 
       the standard PMem mode will be 4KB) 
                 
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the ch_way interleave
                     b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM 
       only) 
                     b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, 
       DDRT Block, and DDRT 2LM.  the latter only in mirror mode) 
                     b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT 
       PMem) 
                     b'11   reserved
                     Note:  using the same ch_granularity encoding for both DDRT and 
       DDR4 even though 64B only applies for DDR4 and 4KB only applies for DDRT. 
                 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TADCHNILVOFFSET_2_MC_MAINEXT_STRUCT;


/* TADCHNILVOFFSET_3_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x4025409C)                                                  */
/*       SKX (0x4025409C)                                                     */
/* Register default value:              0x00000000                            */
#define TADCHNILVOFFSET_3_MC_MAINEXT_REG 0x0703409C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           TAD Interleaves and Offsets
 *           This register should be programmned while training_mode=1 or else the 
 * register write will be missed.   
 *           Will be available 0x4F DCLK cycles after we enter normal mode.
 *       
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00 
        socket interleave wayness
                               00 = 1 way,
                               01 = 2 way,
                               10 = 4 way,
                               11 = 8 way.
                 
     */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00 
        Channel interleave wayness
                               00 = 1 way
                               01 = 2 way
                               10 = 3 way
                 
     */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity. When negative offset is programmed (2's complement), 
       this field only support +/-32T and the upper bit is used to indicate the 
       polarity of the offset. 
     */
    UINT32 rsvd_24 : 2;
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       This field should be removed for SKX.  
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the skt_way interleave
                     b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
                     b'01  256B (based off PA[8] and up)  (used for 1LM only)
                     b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and 
       Block/DDRT_CSR) 
                     b'11   1GB (based off PA[30] and up) {used for PMem only, though 
       the standard PMem mode will be 4KB) 
                 
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the ch_way interleave
                     b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM 
       only) 
                     b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, 
       DDRT Block, and DDRT 2LM.  the latter only in mirror mode) 
                     b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT 
       PMem) 
                     b'11   reserved
                     Note:  using the same ch_granularity encoding for both DDRT and 
       DDR4 even though 64B only applies for DDR4 and 4KB only applies for DDRT. 
                 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TADCHNILVOFFSET_3_MC_MAINEXT_STRUCT;


/* TADCHNILVOFFSET_4_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x402540A0)                                                  */
/*       SKX (0x402540A0)                                                     */
/* Register default value:              0x00000000                            */
#define TADCHNILVOFFSET_4_MC_MAINEXT_REG 0x070340A0
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           TAD Interleaves and Offsets
 *           This register should be programmned while training_mode=1 or else the 
 * register write will be missed.   
 *           Will be available 0x4F DCLK cycles after we enter normal mode.
 *       
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00 
        socket interleave wayness
                               00 = 1 way,
                               01 = 2 way,
                               10 = 4 way,
                               11 = 8 way.
                 
     */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00 
        Channel interleave wayness
                               00 = 1 way
                               01 = 2 way
                               10 = 3 way
                 
     */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity. When negative offset is programmed (2's complement), 
       this field only support +/-32T and the upper bit is used to indicate the 
       polarity of the offset. 
     */
    UINT32 rsvd_24 : 2;
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       This field should be removed for SKX.  
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the skt_way interleave
                     b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
                     b'01  256B (based off PA[8] and up)  (used for 1LM only)
                     b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and 
       Block/DDRT_CSR) 
                     b'11   1GB (based off PA[30] and up) {used for PMem only, though 
       the standard PMem mode will be 4KB) 
                 
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the ch_way interleave
                     b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM 
       only) 
                     b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, 
       DDRT Block, and DDRT 2LM.  the latter only in mirror mode) 
                     b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT 
       PMem) 
                     b'11   reserved
                     Note:  using the same ch_granularity encoding for both DDRT and 
       DDR4 even though 64B only applies for DDR4 and 4KB only applies for DDRT. 
                 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TADCHNILVOFFSET_4_MC_MAINEXT_STRUCT;


/* TADCHNILVOFFSET_5_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x402540A4)                                                  */
/*       SKX (0x402540A4)                                                     */
/* Register default value:              0x00000000                            */
#define TADCHNILVOFFSET_5_MC_MAINEXT_REG 0x070340A4
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           TAD Interleaves and Offsets
 *           This register should be programmned while training_mode=1 or else the 
 * register write will be missed.   
 *           Will be available 0x4F DCLK cycles after we enter normal mode.
 *       
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00 
        socket interleave wayness
                               00 = 1 way,
                               01 = 2 way,
                               10 = 4 way,
                               11 = 8 way.
                 
     */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00 
        Channel interleave wayness
                               00 = 1 way
                               01 = 2 way
                               10 = 3 way
                 
     */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity. When negative offset is programmed (2's complement), 
       this field only support +/-32T and the upper bit is used to indicate the 
       polarity of the offset. 
     */
    UINT32 rsvd_24 : 2;
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       This field should be removed for SKX.  
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the skt_way interleave
                     b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
                     b'01  256B (based off PA[8] and up)  (used for 1LM only)
                     b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and 
       Block/DDRT_CSR) 
                     b'11   1GB (based off PA[30] and up) {used for PMem only, though 
       the standard PMem mode will be 4KB) 
                 
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the ch_way interleave
                     b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM 
       only) 
                     b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, 
       DDRT Block, and DDRT 2LM.  the latter only in mirror mode) 
                     b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT 
       PMem) 
                     b'11   reserved
                     Note:  using the same ch_granularity encoding for both DDRT and 
       DDR4 even though 64B only applies for DDR4 and 4KB only applies for DDRT. 
                 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TADCHNILVOFFSET_5_MC_MAINEXT_STRUCT;


/* TADCHNILVOFFSET_6_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x402540A8)                                                  */
/*       SKX (0x402540A8)                                                     */
/* Register default value:              0x00000000                            */
#define TADCHNILVOFFSET_6_MC_MAINEXT_REG 0x070340A8
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           TAD Interleaves and Offsets
 *           This register should be programmned while training_mode=1 or else the 
 * register write will be missed.   
 *           Will be available 0x4F DCLK cycles after we enter normal mode.
 *       
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00 
        socket interleave wayness
                               00 = 1 way,
                               01 = 2 way,
                               10 = 4 way,
                               11 = 8 way.
                 
     */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00 
        Channel interleave wayness
                               00 = 1 way
                               01 = 2 way
                               10 = 3 way
                 
     */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity. When negative offset is programmed (2's complement), 
       this field only support +/-32T and the upper bit is used to indicate the 
       polarity of the offset. 
     */
    UINT32 rsvd_24 : 2;
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       This field should be removed for SKX.  
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the skt_way interleave
                     b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
                     b'01  256B (based off PA[8] and up)  (used for 1LM only)
                     b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and 
       Block/DDRT_CSR) 
                     b'11   1GB (based off PA[30] and up) {used for PMem only, though 
       the standard PMem mode will be 4KB) 
                 
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the ch_way interleave
                     b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM 
       only) 
                     b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, 
       DDRT Block, and DDRT 2LM.  the latter only in mirror mode) 
                     b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT 
       PMem) 
                     b'11   reserved
                     Note:  using the same ch_granularity encoding for both DDRT and 
       DDR4 even though 64B only applies for DDR4 and 4KB only applies for DDRT. 
                 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TADCHNILVOFFSET_6_MC_MAINEXT_STRUCT;


/* TADCHNILVOFFSET_7_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x402540AC)                                                  */
/*       SKX (0x402540AC)                                                     */
/* Register default value:              0x00000000                            */
#define TADCHNILVOFFSET_7_MC_MAINEXT_REG 0x070340AC
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           TAD Interleaves and Offsets
 *           This register should be programmned while training_mode=1 or else the 
 * register write will be missed.   
 *           Will be available 0x4F DCLK cycles after we enter normal mode.
 *       
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00 
        socket interleave wayness
                               00 = 1 way,
                               01 = 2 way,
                               10 = 4 way,
                               11 = 8 way.
                 
     */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00 
        Channel interleave wayness
                               00 = 1 way
                               01 = 2 way
                               10 = 3 way
                 
     */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity. When negative offset is programmed (2's complement), 
       this field only support +/-32T and the upper bit is used to indicate the 
       polarity of the offset. 
     */
    UINT32 rsvd_24 : 2;
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       This field should be removed for SKX.  
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the skt_way interleave
                     b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
                     b'01  256B (based off PA[8] and up)  (used for 1LM only)
                     b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and 
       Block/DDRT_CSR) 
                     b'11   1GB (based off PA[30] and up) {used for PMem only, though 
       the standard PMem mode will be 4KB) 
                 
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
                     Specifies the granularity of the ch_way interleave
                     b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM 
       only) 
                     b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, 
       DDRT Block, and DDRT 2LM.  the latter only in mirror mode) 
                     b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT 
       PMem) 
                     b'11   reserved
                     Note:  using the same ch_granularity encoding for both DDRT and 
       DDR4 even though 64B only applies for DDR4 and 4KB only applies for DDRT. 
                 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TADCHNILVOFFSET_7_MC_MAINEXT_STRUCT;


/* MEM_SIZE_CONTROL_MC_MAINEXT_REG supported on:                              */
/*       SKX_A0 (0x402540C0)                                                  */
/*       SKX (0x402540C0)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_SIZE_CONTROL_MC_MAINEXT_REG 0x070340C0
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Controls memory address generation for fast div-3
 */
typedef union {
  struct {
    UINT32 enable : 1;
    /* enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable address crop for fast div-3
     */
    UINT32 nm_chn_cap : 4;
    /* nm_chn_cap - Bits[4:1], RW_LB, default = 4'b0000 
       Per Channel Near Memory Capacity.
       4'h0 - 4GB
       4'h1 - 8GB
       4'h2 - 16GB
       4'h3 - 32GB
       4'h4 - 64GB
       4'h5 - 128GB
       4'h6 - 256GB
       4'h7 - 512GB
       4'b1xxx - Reserved
       This field can be programmed on the per-channel basis.
       
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MEM_SIZE_CONTROL_MC_MAINEXT_STRUCT;


/* PXPENHCAP_MC_MAINEXT_REG supported on:                                     */
/*       SKX_A0 (0x40254100)                                                  */
/*       SKX (0x40254100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_MC_MAINEXT_REG 0x07034100
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
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
} PXPENHCAP_MC_MAINEXT_STRUCT;


/* RIRWAYNESSLIMIT_0_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x40254108)                                                  */
/*       SKX (0x40254108)                                                     */
/* Register default value:              0x00000000                            */
#define RIRWAYNESSLIMIT_0_MC_MAINEXT_REG 0x07034108
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * There are 4 RIR ranges (represents how many rank interleave ranges supported to 
 * cover DIMM configurations). 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_limit : 11;
    /* rir_limit - Bits[11:1], RW_LB, default = 11'b00000000000 
       RIR[4:0].LIMIT[39:29] == highest address of the range in channel address space, 
       384GB in lock-step/192GB in independent channel, 512MB granularity.  
     */
    UINT32 rsvd_12 : 16;
    /* rsvd_12 - Bits[27:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_way : 2;
    /* rir_way - Bits[29:28], RW_LB, default = 2'b00 
       rank interleave wayness00 = 1 way,
                  01 = 2 way,
                  10 = 4 way,
                  11 = 8 way.
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_val : 1;
    /* rir_val - Bits[31:31], RW_LB, default = 1'b0 
       Range Valid when set; otherwise, invalid
     */
  } Bits;
  UINT32 Data;
} RIRWAYNESSLIMIT_0_MC_MAINEXT_STRUCT;


/* RIRWAYNESSLIMIT_1_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x4025410C)                                                  */
/*       SKX (0x4025410C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRWAYNESSLIMIT_1_MC_MAINEXT_REG 0x0703410C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * There are 4 RIR ranges (represents how many rank interleave ranges supported to 
 * cover DIMM configurations). 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_limit : 11;
    /* rir_limit - Bits[11:1], RW_LB, default = 11'b00000000000 
       RIR[4:0].LIMIT[39:29] == highest address of the range in channel address space, 
       384GB in lock-step/192GB in independent channel, 512MB granularity.  
     */
    UINT32 rsvd_12 : 16;
    /* rsvd_12 - Bits[27:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_way : 2;
    /* rir_way - Bits[29:28], RW_LB, default = 2'b00 
       rank interleave wayness00 = 1 way,
                  01 = 2 way,
                  10 = 4 way,
                  11 = 8 way.
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_val : 1;
    /* rir_val - Bits[31:31], RW_LB, default = 1'b0 
       Range Valid when set; otherwise, invalid
     */
  } Bits;
  UINT32 Data;
} RIRWAYNESSLIMIT_1_MC_MAINEXT_STRUCT;


/* RIRWAYNESSLIMIT_2_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x40254110)                                                  */
/*       SKX (0x40254110)                                                     */
/* Register default value:              0x00000000                            */
#define RIRWAYNESSLIMIT_2_MC_MAINEXT_REG 0x07034110
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * There are 4 RIR ranges (represents how many rank interleave ranges supported to 
 * cover DIMM configurations). 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_limit : 11;
    /* rir_limit - Bits[11:1], RW_LB, default = 11'b00000000000 
       RIR[4:0].LIMIT[39:29] == highest address of the range in channel address space, 
       384GB in lock-step/192GB in independent channel, 512MB granularity.  
     */
    UINT32 rsvd_12 : 16;
    /* rsvd_12 - Bits[27:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_way : 2;
    /* rir_way - Bits[29:28], RW_LB, default = 2'b00 
       rank interleave wayness00 = 1 way,
                  01 = 2 way,
                  10 = 4 way,
                  11 = 8 way.
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_val : 1;
    /* rir_val - Bits[31:31], RW_LB, default = 1'b0 
       Range Valid when set; otherwise, invalid
     */
  } Bits;
  UINT32 Data;
} RIRWAYNESSLIMIT_2_MC_MAINEXT_STRUCT;


/* RIRWAYNESSLIMIT_3_MC_MAINEXT_REG supported on:                             */
/*       SKX_A0 (0x40254114)                                                  */
/*       SKX (0x40254114)                                                     */
/* Register default value:              0x00000000                            */
#define RIRWAYNESSLIMIT_3_MC_MAINEXT_REG 0x07034114
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * There are 4 RIR ranges (represents how many rank interleave ranges supported to 
 * cover DIMM configurations). 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_limit : 11;
    /* rir_limit - Bits[11:1], RW_LB, default = 11'b00000000000 
       RIR[4:0].LIMIT[39:29] == highest address of the range in channel address space, 
       384GB in lock-step/192GB in independent channel, 512MB granularity.  
     */
    UINT32 rsvd_12 : 16;
    /* rsvd_12 - Bits[27:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_way : 2;
    /* rir_way - Bits[29:28], RW_LB, default = 2'b00 
       rank interleave wayness00 = 1 way,
                  01 = 2 way,
                  10 = 4 way,
                  11 = 8 way.
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_val : 1;
    /* rir_val - Bits[31:31], RW_LB, default = 1'b0 
       Range Valid when set; otherwise, invalid
     */
  } Bits;
  UINT32 Data;
} RIRWAYNESSLIMIT_3_MC_MAINEXT_STRUCT;


/* RIRILV0OFFSET_0_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254120)                                                  */
/*       SKX (0x40254120)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV0OFFSET_0_MC_MAINEXT_REG 0x07034120
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv0offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset0 : 14;
    /* rir_offset0 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET0[39:26] == rank interleave 0 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt0 : 4;
    /* rir_rnk_tgt0 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 0 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV0OFFSET_0_MC_MAINEXT_STRUCT;


/* RIRILV0OFFSET_1_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254124)                                                  */
/*       SKX (0x40254124)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV0OFFSET_1_MC_MAINEXT_REG 0x07034124
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv0offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset0 : 14;
    /* rir_offset0 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET0[39:26] == rank interleave 0 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt0 : 4;
    /* rir_rnk_tgt0 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 0 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV0OFFSET_1_MC_MAINEXT_STRUCT;


/* RIRILV0OFFSET_2_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254128)                                                  */
/*       SKX (0x40254128)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV0OFFSET_2_MC_MAINEXT_REG 0x07034128
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv0offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset0 : 14;
    /* rir_offset0 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET0[39:26] == rank interleave 0 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt0 : 4;
    /* rir_rnk_tgt0 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 0 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV0OFFSET_2_MC_MAINEXT_STRUCT;


/* RIRILV0OFFSET_3_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x4025412C)                                                  */
/*       SKX (0x4025412C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV0OFFSET_3_MC_MAINEXT_REG 0x0703412C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv0offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset0 : 14;
    /* rir_offset0 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET0[39:26] == rank interleave 0 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt0 : 4;
    /* rir_rnk_tgt0 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 0 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV0OFFSET_3_MC_MAINEXT_STRUCT;


/* RIRILV1OFFSET_0_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254130)                                                  */
/*       SKX (0x40254130)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV1OFFSET_0_MC_MAINEXT_REG 0x07034130
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv1offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset1 : 14;
    /* rir_offset1 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET1[39:26] == rank interleave 1 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
               
     */
    UINT32 rir_rnk_tgt1 : 4;
    /* rir_rnk_tgt1 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 1 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV1OFFSET_0_MC_MAINEXT_STRUCT;


/* RIRILV1OFFSET_1_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254134)                                                  */
/*       SKX (0x40254134)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV1OFFSET_1_MC_MAINEXT_REG 0x07034134
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv1offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset1 : 14;
    /* rir_offset1 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET1[39:26] == rank interleave 1 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
               
     */
    UINT32 rir_rnk_tgt1 : 4;
    /* rir_rnk_tgt1 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 1 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV1OFFSET_1_MC_MAINEXT_STRUCT;


/* RIRILV1OFFSET_2_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254138)                                                  */
/*       SKX (0x40254138)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV1OFFSET_2_MC_MAINEXT_REG 0x07034138
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv1offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset1 : 14;
    /* rir_offset1 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET1[39:26] == rank interleave 1 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
               
     */
    UINT32 rir_rnk_tgt1 : 4;
    /* rir_rnk_tgt1 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 1 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV1OFFSET_2_MC_MAINEXT_STRUCT;


/* RIRILV1OFFSET_3_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x4025413C)                                                  */
/*       SKX (0x4025413C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV1OFFSET_3_MC_MAINEXT_REG 0x0703413C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv1offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset1 : 14;
    /* rir_offset1 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET1[39:26] == rank interleave 1 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
               
     */
    UINT32 rir_rnk_tgt1 : 4;
    /* rir_rnk_tgt1 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 1 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV1OFFSET_3_MC_MAINEXT_STRUCT;


/* RIRILV2OFFSET_0_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254140)                                                  */
/*       SKX (0x40254140)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV2OFFSET_0_MC_MAINEXT_REG 0x07034140
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv2offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset2 : 14;
    /* rir_offset2 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET2[39:26] == rank interleave 2 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt2 : 4;
    /* rir_rnk_tgt2 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 2 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV2OFFSET_0_MC_MAINEXT_STRUCT;


/* RIRILV2OFFSET_1_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254144)                                                  */
/*       SKX (0x40254144)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV2OFFSET_1_MC_MAINEXT_REG 0x07034144
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv2offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset2 : 14;
    /* rir_offset2 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET2[39:26] == rank interleave 2 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt2 : 4;
    /* rir_rnk_tgt2 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 2 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV2OFFSET_1_MC_MAINEXT_STRUCT;


/* RIRILV2OFFSET_2_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254148)                                                  */
/*       SKX (0x40254148)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV2OFFSET_2_MC_MAINEXT_REG 0x07034148
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv2offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset2 : 14;
    /* rir_offset2 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET2[39:26] == rank interleave 2 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt2 : 4;
    /* rir_rnk_tgt2 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 2 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV2OFFSET_2_MC_MAINEXT_STRUCT;


/* RIRILV2OFFSET_3_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x4025414C)                                                  */
/*       SKX (0x4025414C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV2OFFSET_3_MC_MAINEXT_REG 0x0703414C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv2offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset2 : 14;
    /* rir_offset2 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET2[39:26] == rank interleave 2 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt2 : 4;
    /* rir_rnk_tgt2 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 2 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV2OFFSET_3_MC_MAINEXT_STRUCT;


/* RIRILV3OFFSET_0_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254150)                                                  */
/*       SKX (0x40254150)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV3OFFSET_0_MC_MAINEXT_REG 0x07034150
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv3offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset3 : 14;
    /* rir_offset3 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET3[39:26] == rank interleave 3 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt3 : 4;
    /* rir_rnk_tgt3 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 3 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV3OFFSET_0_MC_MAINEXT_STRUCT;


/* RIRILV3OFFSET_1_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254154)                                                  */
/*       SKX (0x40254154)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV3OFFSET_1_MC_MAINEXT_REG 0x07034154
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv3offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset3 : 14;
    /* rir_offset3 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET3[39:26] == rank interleave 3 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt3 : 4;
    /* rir_rnk_tgt3 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 3 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV3OFFSET_1_MC_MAINEXT_STRUCT;


/* RIRILV3OFFSET_2_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254158)                                                  */
/*       SKX (0x40254158)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV3OFFSET_2_MC_MAINEXT_REG 0x07034158
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv3offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset3 : 14;
    /* rir_offset3 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET3[39:26] == rank interleave 3 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt3 : 4;
    /* rir_rnk_tgt3 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 3 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV3OFFSET_2_MC_MAINEXT_STRUCT;


/* RIRILV3OFFSET_3_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x4025415C)                                                  */
/*       SKX (0x4025415C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV3OFFSET_3_MC_MAINEXT_REG 0x0703415C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv3offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset3 : 14;
    /* rir_offset3 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET3[39:26] == rank interleave 3 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt3 : 4;
    /* rir_rnk_tgt3 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 3 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV3OFFSET_3_MC_MAINEXT_STRUCT;


/* RIRILV4OFFSET_0_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254160)                                                  */
/*       SKX (0x40254160)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV4OFFSET_0_MC_MAINEXT_REG 0x07034160
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv4offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset4 : 14;
    /* rir_offset4 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET4[39:26] == rank interleave 4 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt4 : 4;
    /* rir_rnk_tgt4 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 4 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV4OFFSET_0_MC_MAINEXT_STRUCT;


/* RIRILV4OFFSET_1_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254164)                                                  */
/*       SKX (0x40254164)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV4OFFSET_1_MC_MAINEXT_REG 0x07034164
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv4offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset4 : 14;
    /* rir_offset4 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET4[39:26] == rank interleave 4 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt4 : 4;
    /* rir_rnk_tgt4 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 4 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV4OFFSET_1_MC_MAINEXT_STRUCT;


/* RIRILV4OFFSET_2_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254168)                                                  */
/*       SKX (0x40254168)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV4OFFSET_2_MC_MAINEXT_REG 0x07034168
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv4offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset4 : 14;
    /* rir_offset4 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET4[39:26] == rank interleave 4 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt4 : 4;
    /* rir_rnk_tgt4 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 4 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV4OFFSET_2_MC_MAINEXT_STRUCT;


/* RIRILV4OFFSET_3_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x4025416C)                                                  */
/*       SKX (0x4025416C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV4OFFSET_3_MC_MAINEXT_REG 0x0703416C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv4offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset4 : 14;
    /* rir_offset4 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET4[39:26] == rank interleave 4 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt4 : 4;
    /* rir_rnk_tgt4 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 4 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV4OFFSET_3_MC_MAINEXT_STRUCT;


/* RIRILV5OFFSET_0_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254170)                                                  */
/*       SKX (0x40254170)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV5OFFSET_0_MC_MAINEXT_REG 0x07034170
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv5offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset5 : 14;
    /* rir_offset5 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET5[39:26] == rank interleave 5 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt5 : 4;
    /* rir_rnk_tgt5 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 5 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV5OFFSET_0_MC_MAINEXT_STRUCT;


/* RIRILV5OFFSET_1_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254174)                                                  */
/*       SKX (0x40254174)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV5OFFSET_1_MC_MAINEXT_REG 0x07034174
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv5offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset5 : 14;
    /* rir_offset5 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET5[39:26] == rank interleave 5 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt5 : 4;
    /* rir_rnk_tgt5 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 5 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV5OFFSET_1_MC_MAINEXT_STRUCT;


/* RIRILV5OFFSET_2_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254178)                                                  */
/*       SKX (0x40254178)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV5OFFSET_2_MC_MAINEXT_REG 0x07034178
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv5offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset5 : 14;
    /* rir_offset5 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET5[39:26] == rank interleave 5 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt5 : 4;
    /* rir_rnk_tgt5 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 5 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV5OFFSET_2_MC_MAINEXT_STRUCT;


/* RIRILV5OFFSET_3_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x4025417C)                                                  */
/*       SKX (0x4025417C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV5OFFSET_3_MC_MAINEXT_REG 0x0703417C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv5offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset5 : 14;
    /* rir_offset5 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET5[39:26] == rank interleave 5 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt5 : 4;
    /* rir_rnk_tgt5 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 5 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV5OFFSET_3_MC_MAINEXT_STRUCT;


/* RIRILV6OFFSET_0_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254180)                                                  */
/*       SKX (0x40254180)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV6OFFSET_0_MC_MAINEXT_REG 0x07034180
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv6offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset6 : 14;
    /* rir_offset6 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET6[39:26] == rank interleave 6 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt6 : 4;
    /* rir_rnk_tgt6 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 6 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV6OFFSET_0_MC_MAINEXT_STRUCT;


/* RIRILV6OFFSET_1_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254184)                                                  */
/*       SKX (0x40254184)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV6OFFSET_1_MC_MAINEXT_REG 0x07034184
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv6offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset6 : 14;
    /* rir_offset6 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET6[39:26] == rank interleave 6 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt6 : 4;
    /* rir_rnk_tgt6 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 6 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV6OFFSET_1_MC_MAINEXT_STRUCT;


/* RIRILV6OFFSET_2_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254188)                                                  */
/*       SKX (0x40254188)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV6OFFSET_2_MC_MAINEXT_REG 0x07034188
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv6offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset6 : 14;
    /* rir_offset6 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET6[39:26] == rank interleave 6 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt6 : 4;
    /* rir_rnk_tgt6 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 6 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV6OFFSET_2_MC_MAINEXT_STRUCT;


/* RIRILV6OFFSET_3_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x4025418C)                                                  */
/*       SKX (0x4025418C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV6OFFSET_3_MC_MAINEXT_REG 0x0703418C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv6offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset6 : 14;
    /* rir_offset6 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET6[39:26] == rank interleave 6 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt6 : 4;
    /* rir_rnk_tgt6 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 6 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV6OFFSET_3_MC_MAINEXT_STRUCT;


/* RIRILV7OFFSET_0_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254190)                                                  */
/*       SKX (0x40254190)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV7OFFSET_0_MC_MAINEXT_REG 0x07034190
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv7offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset7 : 14;
    /* rir_offset7 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET7[39:26] == rank interleave 0 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt7 : 4;
    /* rir_rnk_tgt7 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 7 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV7OFFSET_0_MC_MAINEXT_STRUCT;


/* RIRILV7OFFSET_1_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254194)                                                  */
/*       SKX (0x40254194)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV7OFFSET_1_MC_MAINEXT_REG 0x07034194
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv7offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset7 : 14;
    /* rir_offset7 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET7[39:26] == rank interleave 0 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt7 : 4;
    /* rir_rnk_tgt7 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 7 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV7OFFSET_1_MC_MAINEXT_STRUCT;


/* RIRILV7OFFSET_2_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x40254198)                                                  */
/*       SKX (0x40254198)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV7OFFSET_2_MC_MAINEXT_REG 0x07034198
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv7offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset7 : 14;
    /* rir_offset7 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET7[39:26] == rank interleave 0 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt7 : 4;
    /* rir_rnk_tgt7 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 7 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV7OFFSET_2_MC_MAINEXT_STRUCT;


/* RIRILV7OFFSET_3_MC_MAINEXT_REG supported on:                               */
/*       SKX_A0 (0x4025419C)                                                  */
/*       SKX (0x4025419C)                                                     */
/* Register default value:              0x00000000                            */
#define RIRILV7OFFSET_3_MC_MAINEXT_REG 0x0703419C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Four instances of ririlv7offset, one per RIR
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset7 : 14;
    /* rir_offset7 - Bits[15:2], RW_LB, default = 14'b00000000000000 
       RIR[5:0].RANKOFFSET7[39:26] == rank interleave 0 offset, 64MB granularity 
                   (The processor's minimum rank size is 512MB. 512MB/8 interleave = 
       64MB per 8-way interleave.) 
     */
    UINT32 rir_rnk_tgt7 : 4;
    /* rir_rnk_tgt7 - Bits[19:16], RW_LB, default = 4'b0000 
       target rank ID for rank interleave 7 (used for 1/2/4/8-way RIR interleaving).
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RIRILV7OFFSET_3_MC_MAINEXT_STRUCT;


/* RSP_FUNC_ADDR_MATCH_LO_MC_MAINEXT_REG supported on:                        */
/*       SKX_A0 (0x402541C0)                                                  */
/*       SKX (0x402541C0)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_ADDR_MATCH_LO_MC_MAINEXT_REG 0x070341C0
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Complete address match (Addr[45:3]) and mask is supported for all M2M writes. 
 * The error injection logic uses the address match mask logic output to determine 
 * which memory writes need to get error injection. Users can program up to two x4 
 * device masks (8-bits per chunk - 64 bits per cacheline).  
 */
typedef union {
  struct {
    UINT32 addr_match_lower : 32;
    /* addr_match_lower - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Addr Match Lower: 32-bits (Match Addr[34:3])
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_ADDR_MATCH_LO_MC_MAINEXT_STRUCT;


/* RSP_FUNC_ADDR_MATCH_HI_MC_MAINEXT_REG supported on:                        */
/*       SKX_A0 (0x402541C4)                                                  */
/*       SKX (0x402541C4)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_ADDR_MATCH_HI_MC_MAINEXT_REG 0x070341C4
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Complete address match (Addr[45:3]) and mask is supported for all HA writes. The 
 * error injection logic uses the address match mask logic output to determine 
 * which memory writes need to get error injection. Users can program up to two x4 
 * device masks (8-bits per chunk - 64 bits per cacheline). In Lockstep mode, only 
 * one device can be programmed in each lockstep channel. 
 */
typedef union {
  struct {
    UINT32 addr_match_higher : 11;
    /* addr_match_higher - Bits[10:0], RWS, default = 11'b00000000000 
       Addr Match Higher : 11-Bits (Match Addr[45:35])
     */
    UINT32 rsp_func_addr_match_en : 1;
    /* rsp_func_addr_match_en - Bits[11:11], RWS_LV, default = 1'b0 
       Enabling the Address Match Response Function when set.
     */
    UINT32 rsvd_12 : 1;
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_16 : 8;
    UINT32 rsvd_24 : 1;
    /* rsvd_24 - Bits[24:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mirror_match : 3;
    /* mirror_match - Bits[27:25], RWS, default = 3'b000 
       Additional match configuration: Bit 0 matches with Mirror field; Bit 1 matches 
       with mirr_pri (primary channel); Bit 2 matches with demand scrub request 
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_ADDR_MATCH_HI_MC_MAINEXT_STRUCT;


/* RSP_FUNC_ADDR_MASK_LO_MC_MAINEXT_REG supported on:                         */
/*       SKX_A0 (0x402541C8)                                                  */
/*       SKX (0x402541C8)                                                     */
/* Register default value:              0xFFFFFFFF                            */
#define RSP_FUNC_ADDR_MASK_LO_MC_MAINEXT_REG 0x070341C8
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Complete address match (Addr[45:3]) and mask is supported for all HA writes. 
 * Error injection does not use the response logic triggers and uses the match mask 
 * logic output to determine which writes need to get error injection. Users can 
 * program up to two x4 device masks (8-bits per chunk - 64 bits per cacheline). In 
 * Lockstep mode, only one device can be programmed in each lockstep channel. 
 */
typedef union {
  struct {
    UINT32 addr_mask_lower : 32;
    /* addr_mask_lower - Bits[31:0], RWS, default = 32'hffffffff 
       Address Mask to deselect (when set) the corresponding Addr[34:3] for the address 
       match. 
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_ADDR_MASK_LO_MC_MAINEXT_STRUCT;


/* RSP_FUNC_ADDR_MASK_HI_MC_MAINEXT_REG supported on:                         */
/*       SKX_A0 (0x402541CC)                                                  */
/*       SKX (0x402541CC)                                                     */
/* Register default value:              0x0E0007FF                            */
#define RSP_FUNC_ADDR_MASK_HI_MC_MAINEXT_REG 0x070341CC
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Complete address match (Addr[45:3]) and mask is supported for all HA writes. 
 * Error injection does not use the response logic triggers and uses the match mask 
 * logic output to determine which writes need to get error injection. Users can 
 * program up to two x4 device masks (8-bits per chunk - 64 bits per cacheline). In 
 * Lockstep mode, only one device can be programmed in each lockstep channel. 
 */
typedef union {
  struct {
    UINT32 addr_mask_higher : 11;
    /* addr_mask_higher - Bits[10:0], RWS, default = 11'b11111111111 
       Address Mask to deselect (when set) the corresponding Addr[45:35] for the 
       address match. 
     */
    UINT32 rsvd_11 : 5;
    /* rsvd_11 - Bits[15:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_16 : 8;
    UINT32 rsvd_24 : 1;
    /* rsvd_24 - Bits[24:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mirror_mask : 3;
    /* mirror_mask - Bits[27:25], RWS, default = 3'b111 
       Additional mask to deselect (when set) the corresponding bits in mirror_mask 
       field. Bit 0 is mask for Mirror field; Bit 1 is mask for mirr_pri (primary 
       channel); Bit 2 is mask for demand scrub request 
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_ADDR_MASK_HI_MC_MAINEXT_STRUCT;


/* RSP_FUNC_RANK_BANK_MATCH_MC_MAINEXT_REG supported on:                      */
/*       SKX_A0 (0x402541D0)                                                  */
/*       SKX (0x402541D0)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_RANK_BANK_MATCH_MC_MAINEXT_REG 0x070341D0
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Enables targetting injection to a particular bank in a particular logical rank.  
 * When enabled, programmed rank and bank will be matched, system address match 
 * will not be used. 
 */
typedef union {
  struct {
    UINT32 bank : 4;
    /* bank - Bits[3:0], RW, default = 4'b0000 
       Bank to match
     */
    UINT32 rank : 3;
    /* rank - Bits[6:4], RW, default = 3'b000 
       Rank to match
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[29:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dis_patspr : 1;
    /* dis_patspr - Bits[30:30], RW, default = 1'b0 
       Disable any injection matching on Patrol or Spare transactions. This applies to 
       rank/bank match and system address match. 
     */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'b0 
       Enable rank and bank matching
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_RANK_BANK_MATCH_MC_MAINEXT_STRUCT;




/* ECC_MODE_RANK0_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x10254210)                                                  */
/*       SKX (0x10254210)                                                     */
/* Register default value:              0x00                                  */
#define ECC_MODE_RANK0_MC_MAINEXT_REG 0x07031210
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *       
 */
typedef union {
  struct {
    UINT8 mode : 1;
    /* mode - Bits[0:0], RW_LB, default = 1'b0 
       
                   This field specifies the ECC in use for this rank, for portions of 
       the rank not participating in an ADDDC region. 
                   0 = No failure
                   1 = Parity substitution.  EP:  Detect only, EX:  Single bit 
       correction 
                   This field is set by software at the beginning of a sparing flow.
               
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ECC_MODE_RANK0_MC_MAINEXT_STRUCT;


/* ECC_MODE_RANK1_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x10254211)                                                  */
/*       SKX (0x10254211)                                                     */
/* Register default value:              0x00                                  */
#define ECC_MODE_RANK1_MC_MAINEXT_REG 0x07031211
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *       
 */
typedef union {
  struct {
    UINT8 mode : 1;
    /* mode - Bits[0:0], RW_LB, default = 1'b0 
       
                   This field specifies the ECC in use for this rank, for portions of 
       the rank not participating in an ADDDC region. 
                   0 = No failure
                   1 = Parity substitution.  EP:  Detect only, EX:  Single bit 
       correction 
                   This field is set by software at the beginning of a sparing flow.
               
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ECC_MODE_RANK1_MC_MAINEXT_STRUCT;


/* ECC_MODE_RANK2_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x10254212)                                                  */
/*       SKX (0x10254212)                                                     */
/* Register default value:              0x00                                  */
#define ECC_MODE_RANK2_MC_MAINEXT_REG 0x07031212
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *       
 */
typedef union {
  struct {
    UINT8 mode : 1;
    /* mode - Bits[0:0], RW_LB, default = 1'b0 
       
                   This field specifies the ECC in use for this rank, for portions of 
       the rank not participating in an ADDDC region. 
                   0 = No failure
                   1 = Parity substitution.  EP:  Detect only, EX:  Single bit 
       correction 
                   This field is set by software at the beginning of a sparing flow.
               
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ECC_MODE_RANK2_MC_MAINEXT_STRUCT;


/* ECC_MODE_RANK3_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x10254213)                                                  */
/*       SKX (0x10254213)                                                     */
/* Register default value:              0x00                                  */
#define ECC_MODE_RANK3_MC_MAINEXT_REG 0x07031213
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *       
 */
typedef union {
  struct {
    UINT8 mode : 1;
    /* mode - Bits[0:0], RW_LB, default = 1'b0 
       
                   This field specifies the ECC in use for this rank, for portions of 
       the rank not participating in an ADDDC region. 
                   0 = No failure
                   1 = Parity substitution.  EP:  Detect only, EX:  Single bit 
       correction 
                   This field is set by software at the beginning of a sparing flow.
               
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ECC_MODE_RANK3_MC_MAINEXT_STRUCT;


/* ECC_MODE_RANK4_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x10254214)                                                  */
/*       SKX (0x10254214)                                                     */
/* Register default value:              0x00                                  */
#define ECC_MODE_RANK4_MC_MAINEXT_REG 0x07031214
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *       
 */
typedef union {
  struct {
    UINT8 mode : 1;
    /* mode - Bits[0:0], RW_LB, default = 1'b0 
       
                   This field specifies the ECC in use for this rank, for portions of 
       the rank not participating in an ADDDC region. 
                   0 = No failure
                   1 = Parity substitution.  EP:  Detect only, EX:  Single bit 
       correction 
                   This field is set by software at the beginning of a sparing flow.
               
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ECC_MODE_RANK4_MC_MAINEXT_STRUCT;


/* ECC_MODE_RANK5_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x10254215)                                                  */
/*       SKX (0x10254215)                                                     */
/* Register default value:              0x00                                  */
#define ECC_MODE_RANK5_MC_MAINEXT_REG 0x07031215
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *       
 */
typedef union {
  struct {
    UINT8 mode : 1;
    /* mode - Bits[0:0], RW_LB, default = 1'b0 
       
                   This field specifies the ECC in use for this rank, for portions of 
       the rank not participating in an ADDDC region. 
                   0 = No failure
                   1 = Parity substitution.  EP:  Detect only, EX:  Single bit 
       correction 
                   This field is set by software at the beginning of a sparing flow.
               
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ECC_MODE_RANK5_MC_MAINEXT_STRUCT;


/* ECC_MODE_RANK6_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x10254216)                                                  */
/*       SKX (0x10254216)                                                     */
/* Register default value:              0x00                                  */
#define ECC_MODE_RANK6_MC_MAINEXT_REG 0x07031216
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *       
 */
typedef union {
  struct {
    UINT8 mode : 1;
    /* mode - Bits[0:0], RW_LB, default = 1'b0 
       
                   This field specifies the ECC in use for this rank, for portions of 
       the rank not participating in an ADDDC region. 
                   0 = No failure
                   1 = Parity substitution.  EP:  Detect only, EX:  Single bit 
       correction 
                   This field is set by software at the beginning of a sparing flow.
               
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ECC_MODE_RANK6_MC_MAINEXT_STRUCT;


/* ECC_MODE_RANK7_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x10254217)                                                  */
/*       SKX (0x10254217)                                                     */
/* Register default value:              0x00                                  */
#define ECC_MODE_RANK7_MC_MAINEXT_REG 0x07031217
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the current ECC configuration for each rank, 
 * for portions of the rank not participating in an ADDDC region. 
 *       
 */
typedef union {
  struct {
    UINT8 mode : 1;
    /* mode - Bits[0:0], RW_LB, default = 1'b0 
       
                   This field specifies the ECC in use for this rank, for portions of 
       the rank not participating in an ADDDC region. 
                   0 = No failure
                   1 = Parity substitution.  EP:  Detect only, EX:  Single bit 
       correction 
                   This field is set by software at the beginning of a sparing flow.
               
     */
    UINT8 rsvd : 7;
    /* rsvd - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} ECC_MODE_RANK7_MC_MAINEXT_STRUCT;


/* ADDDC_REGION0_CONTROL_MC_MAINEXT_REG supported on:                         */
/*       SKX_A0 (0x40254230)                                                  */
/*       SKX (0x40254230)                                                     */
/* Register default value:              0x00000000                            */
#define ADDDC_REGION0_CONTROL_MC_MAINEXT_REG 0x07034230
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the configuration for the regions using ADDDC 
 * for operation after the sparing copy is finished for the region. 
 *       
 */
typedef union {
  struct {
    UINT32 region_enable : 1;
    /* region_enable - Bits[0:0], RW_LB, default = 1'b0 
       
                   This bit specifies whether the ADDDC region is in use.
                   0=ADDDC not enabled for this region
                   1=ADDDC enabled for this region
                   This bit should be set by software before the beginning of the ADDDC 
       sparing copy flow. 
               
     */
    UINT32 rsvd_1 : 1;
    /* rsvd_1 - Bits[1:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 region_size : 2;
    /* region_size - Bits[3:2], RW_LB, default = 1'b0 
       
                   Granularity for this ADDDC region:
                   00=bank
                   01=rank
                   10=reserved 
                   11=reserved
               
     */
    UINT32 failed_cs : 3;
    /* failed_cs - Bits[6:4], RW_LB, default = 1'b0 
       
                   This field specifies the Chip Select (rank) to match when 
       determining if this is the failed region participating in either ADDDC or Static 
       Virtual Lockstep.  The value in this field is encoded (Chip Selects are normally 
       1-hot), corresponding to CS0-CS7.  When the failed region is a DIMM, two region 
       registers should be programmed corresponding to both Chip Selects of the failed 
       DIMM. 
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 failed_c : 3;
    /* failed_c - Bits[10:8], RW_LB, default = 1'b0 
       
                   This field specifies the subrank to match when determining if this 
       is the failed region participating in ADDDC.  Only used when region_size=bank. 
               
     */
    UINT32 rsvd_11 : 1;
    /* rsvd_11 - Bits[11:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 failed_ba : 2;
    /* failed_ba - Bits[13:12], RW_LB, default = 1'b0 
       
                   This field specifies the bank address to match when determining if 
       this is the failed region participating in ADDDC.  Only used when 
       region_size=bank. 
               
     */
    UINT32 failed_bg : 2;
    /* failed_bg - Bits[15:14], RW_LB, default = 1'b0 
       
                   This field specifies the bank group to match when determining if 
       this is the failed region participating in ADDDC.  Only used when 
       region_size=bank. 
               
     */
    UINT32 nonfailed_cs : 3;
    /* nonfailed_cs - Bits[18:16], RW_LB, default = 1'b0 
       
                   This field specifies the Chip Select (rank) to match when 
       determining if this is the failed region participating in either ADDDC or Static 
       Virtual Lockstep.  The value in this field is encoded (Chip Selects are normally 
       1-hot), corresponding to CS0-CS7.  When the failed region is a DIMM, two region 
       registers should be programmed corresponding to both Chip Selects of the failed 
       DIMM. 
               
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 nonfailed_c : 3;
    /* nonfailed_c - Bits[22:20], RW_LB, default = 1'b0 
       
                   This field specifies the subrank to match when determining if this 
       is the failed region participating in ADDDC.  Only used when region_size=bank. 
               
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 nonfailed_ba : 2;
    /* nonfailed_ba - Bits[25:24], RW_LB, default = 1'b0 
       
                   This field specifies the bank address to match when determining if 
       this is the failed region participating in ADDDC.  Only used when 
       region_size=bank. 
               
     */
    UINT32 nonfailed_bg : 2;
    /* nonfailed_bg - Bits[27:26], RW_LB, default = 1'b0 
       
                   This field specifies the bank group to match when determining if 
       this is the failed region participating in ADDDC.  Only used when 
       region_size=bank. 
               
     */
    UINT32 rsvd_28 : 1;
    /* rsvd_28 - Bits[28:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 copy_in_progress : 1;
    /* copy_in_progress - Bits[29:29], RW_LBV, default = 1'b0 
       
                   This field specifies that ADDDC spare copy is on-going for this 
       region. 
                   This bit should be set by BIOS before starting the ADDDC sparing 
       copy flow. 
                   This bit should be cleared by hardware at the end of the sparing 
       copy flow. 
               
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ADDDC_REGION0_CONTROL_MC_MAINEXT_STRUCT;


/* ADDDC_REGION1_CONTROL_MC_MAINEXT_REG supported on:                         */
/*       SKX_A0 (0x40254234)                                                  */
/*       SKX (0x40254234)                                                     */
/* Register default value:              0x00000000                            */
#define ADDDC_REGION1_CONTROL_MC_MAINEXT_REG 0x07034234
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * 
 *           This register specifies the configuration for the regions using ADDDC 
 * for operation after the sparing copy is finished for the region. 
 *       
 */
typedef union {
  struct {
    UINT32 region_enable : 1;
    /* region_enable - Bits[0:0], RW_LB, default = 1'b0 
       
                   This bit specifies whether the ADDDC region is in use.
                   0=ADDDC not enabled for this region
                   1=ADDDC enabled for this region
               
     */
    UINT32 rsvd_1 : 1;
    /* rsvd_1 - Bits[1:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 region_size : 2;
    /* region_size - Bits[3:2], RW_LB, default = 1'b0 
       
                   Granularity for this ADDDC region:
                   00=bank
                   01=rank
                   10=reserved 
                   11=reserved
               
     */
    UINT32 failed_cs : 3;
    /* failed_cs - Bits[6:4], RW_LB, default = 1'b0 
       
                   This field specifies the Chip Select (rank) to match when 
       determining if this is the failed region participating in either ADDDC or Static 
       Virtual Lockstep.  The value in this field is encoded (Chip Selects are normally 
       1-hot), corresponding to CS0-CS7.  When the failed region is a DIMM, two region 
       registers should be programmed corresponding to both Chip Selects of the failed 
       DIMM. 
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 failed_c : 3;
    /* failed_c - Bits[10:8], RW_LB, default = 1'b0 
       
                   This field specifies the subrank to match when determining if this 
       is the failed region participating in either ADDDC or Static Virtual Lockstep.  
       Only used when region_size=bank. 
               
     */
    UINT32 rsvd_11 : 1;
    /* rsvd_11 - Bits[11:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 failed_ba : 2;
    /* failed_ba - Bits[13:12], RW_LB, default = 1'b0 
       
                   This field specifies the bank address to match when determining if 
       this is the failed region participating in either ADDDC or Static Virtual 
       Lockstep.  Only used when region_size=bank. 
               
     */
    UINT32 failed_bg : 2;
    /* failed_bg - Bits[15:14], RW_LB, default = 1'b0 
       
                   This field specifies the bank group to match when determining if 
       this is the failed region participating in either ADDDC or Static Virtual 
       Lockstep.  Only used when region_size=bank. 
               
     */
    UINT32 nonfailed_cs : 3;
    /* nonfailed_cs - Bits[18:16], RW_LB, default = 1'b0 
       
                   This field specifies the Chip Select (rank) to match when 
       determining if this is the failed region participating in either ADDDC or Static 
       Virtual Lockstep.  The value in this field is encoded (Chip Selects are normally 
       1-hot), corresponding to CS0-CS7.  When the failed region is a DIMM, two region 
       registers should be programmed corresponding to both Chip Selects of the failed 
       DIMM. 
               
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 nonfailed_c : 3;
    /* nonfailed_c - Bits[22:20], RW_LB, default = 1'b0 
       
                   This field specifies the subrank to match when determining if this 
       is the failed region participating in either ADDDC or Static Virtual Lockstep.  
       Only used when region_size=bank. 
               
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 nonfailed_ba : 2;
    /* nonfailed_ba - Bits[25:24], RW_LB, default = 1'b0 
       
                   This field specifies the bank address to match when determining if 
       this is the failed region participating in either ADDDC or Static Virtual 
       Lockstep.  Only used when region_size=bank. 
               
     */
    UINT32 nonfailed_bg : 2;
    /* nonfailed_bg - Bits[27:26], RW_LB, default = 1'b0 
       
                   This field specifies the bank group to match when determining if 
       this is the failed region participating in either ADDDC or Static Virtual 
       Lockstep.  Only used when region_size=bank. 
               
     */
    UINT32 rsvd_28 : 1;
    /* rsvd_28 - Bits[28:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 copy_in_progress : 1;
    /* copy_in_progress - Bits[29:29], RW_LB, default = 1'b0 
       
                 This field specifies that ADDDC spare copy is on-going for this region
               
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ADDDC_REGION1_CONTROL_MC_MAINEXT_STRUCT;










/* MCDECS_RCB_GATE_CTL_MC_MAINEXT_REG supported on:                           */
/*       SKX_A0 (0x40254248)                                                  */
/*       SKX (0x40254248)                                                     */
/* Register default value:              0x00000040                            */
#define MCDECS_RCB_GATE_CTL_MC_MAINEXT_REG 0x07034248
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Control information for RCB clock gating in mcdecs
 */
typedef union {
  struct {
    UINT32 rcb_gate_mcdecs_timer : 10;
    /* rcb_gate_mcdecs_timer - Bits[9:0], RW_LB, default = 10'b0001000000 
       Time clock keeps running after triggering events
     */
    UINT32 rsvd : 22;
    /* rsvd - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCDECS_RCB_GATE_CTL_MC_MAINEXT_STRUCT;


/* MCDECS_ERR_LOG_MC_MAINEXT_REG supported on:                                */
/*       SKX_A0 (0x4025424C)                                                  */
/*       SKX (0x4025424C)                                                     */
/* Register default value:              0x00000000                            */
#define MCDECS_ERR_LOG_MC_MAINEXT_REG 0x0703424C
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 * Error information in mcdecs
 */
typedef union {
  struct {
    UINT32 critical_chunk_err : 1;
    /* critical_chunk_err - Bits[0:0], RW_LB, default = 1'b0 
       Error indication when critical chunk bit is set for ADDDC or PlusOne transaction
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCDECS_ERR_LOG_MC_MAINEXT_STRUCT;




/* MCDDRTCFG_MC_MAINEXT_REG supported on:                                     */
/*       SKX_A0 (0x40254400)                                                  */
/*       SKX (0x40254400)                                                     */
/* Register default value:              0x00000000                            */
#define MCDDRTCFG_MC_MAINEXT_REG 0x07034400
/* Struct format extracted from XML file SKX\2.10.4.CFG.xml.
 */
typedef union {
  struct {
    UINT32 slot0 : 1;
    /* slot0 - Bits[0:0], RW_L, default = 1'b0 
       
                   DDR-T DIMM populated if set; otherwise, unpopulated.
                   Should be set to 0 when this slot is populated with a DDR4 DIMM.
               
     */
    UINT32 slot1 : 1;
    /* slot1 - Bits[1:1], RW_L, default = 1'b0 
       
                   DDR-T DIMM populated if set; otherwise, unpopulated.
                   Should be set to 0 when this slot is populated with a DDR4 DIMM.
               
     */
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCDDRTCFG_MC_MAINEXT_STRUCT;


#endif /* MC_MAINEXT_h */
