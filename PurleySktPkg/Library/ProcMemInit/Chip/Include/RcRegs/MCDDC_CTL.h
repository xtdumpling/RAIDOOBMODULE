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

#ifndef MCDDC_CTL_h
#define MCDDC_CTL_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* MCDDC_CTL_DEV 10                                                           */
/* MCDDC_CTL_FUN 2                                                            */

/* VID_MCDDC_CTL_REG supported on:                                            */
/*       SKX_A0 (0x20252000)                                                  */
/*       SKX (0x20252000)                                                     */
/* Register default value:              0x8086                                */
#define VID_MCDDC_CTL_REG 0x0B002000
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} VID_MCDDC_CTL_STRUCT;


/* DID_MCDDC_CTL_REG supported on:                                            */
/*       SKX_A0 (0x20252002)                                                  */
/*       SKX (0x20252002)                                                     */
/* Register default value:              0x2042                                */
#define DID_MCDDC_CTL_REG 0x0B002002
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2042 
        
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
} DID_MCDDC_CTL_STRUCT;


/* PCICMD_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x20252004)                                                  */
/*       SKX (0x20252004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_MCDDC_CTL_REG 0x0B002004
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} PCICMD_MCDDC_CTL_STRUCT;


/* PCISTS_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x20252006)                                                  */
/*       SKX (0x20252006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_MCDDC_CTL_REG 0x0B002006
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} PCISTS_MCDDC_CTL_STRUCT;


/* RID_MCDDC_CTL_REG supported on:                                            */
/*       SKX_A0 (0x10252008)                                                  */
/*       SKX (0x10252008)                                                     */
/* Register default value:              0x00                                  */
#define RID_MCDDC_CTL_REG 0x0B001008
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} RID_MCDDC_CTL_STRUCT;


/* CCR_N0_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x10252009)                                                  */
/*       SKX (0x10252009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_MCDDC_CTL_REG 0x0B001009
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_MCDDC_CTL_STRUCT;


/* CCR_N1_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x2025200A)                                                  */
/*       SKX (0x2025200A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_MCDDC_CTL_REG 0x0B00200A
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} CCR_N1_MCDDC_CTL_STRUCT;


/* CLSR_MCDDC_CTL_REG supported on:                                           */
/*       SKX_A0 (0x1025200C)                                                  */
/*       SKX (0x1025200C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_MCDDC_CTL_REG 0x0B00100C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} CLSR_MCDDC_CTL_STRUCT;


/* PLAT_MCDDC_CTL_REG supported on:                                           */
/*       SKX_A0 (0x1025200D)                                                  */
/*       SKX (0x1025200D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_MCDDC_CTL_REG 0x0B00100D
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} PLAT_MCDDC_CTL_STRUCT;


/* HDR_MCDDC_CTL_REG supported on:                                            */
/*       SKX_A0 (0x1025200E)                                                  */
/*       SKX (0x1025200E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_MCDDC_CTL_REG 0x0B00100E
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} HDR_MCDDC_CTL_STRUCT;


/* BIST_MCDDC_CTL_REG supported on:                                           */
/*       SKX_A0 (0x1025200F)                                                  */
/*       SKX (0x1025200F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_MCDDC_CTL_REG 0x0B00100F
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} BIST_MCDDC_CTL_STRUCT;


/* SVID_MCDDC_CTL_REG supported on:                                           */
/*       SKX_A0 (0x2025202C)                                                  */
/*       SKX (0x2025202C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_MCDDC_CTL_REG 0x0B00202C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x02c
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_MCDDC_CTL_STRUCT;


/* SDID_MCDDC_CTL_REG supported on:                                           */
/*       SKX_A0 (0x2025202E)                                                  */
/*       SKX (0x2025202E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_MCDDC_CTL_REG 0x0B00202E
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x02e
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_MCDDC_CTL_STRUCT;


/* CAPPTR_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x10252034)                                                  */
/*       SKX (0x10252034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_MCDDC_CTL_REG 0x0B001034
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} CAPPTR_MCDDC_CTL_STRUCT;


/* INTL_MCDDC_CTL_REG supported on:                                           */
/*       SKX_A0 (0x1025203C)                                                  */
/*       SKX (0x1025203C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_MCDDC_CTL_REG 0x0B00103C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} INTL_MCDDC_CTL_STRUCT;


/* INTPIN_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x1025203D)                                                  */
/*       SKX (0x1025203D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_MCDDC_CTL_REG 0x0B00103D
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} INTPIN_MCDDC_CTL_STRUCT;


/* MINGNT_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x1025203E)                                                  */
/*       SKX (0x1025203E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_MCDDC_CTL_REG 0x0B00103E
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} MINGNT_MCDDC_CTL_STRUCT;


/* MAXLAT_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x1025203F)                                                  */
/*       SKX (0x1025203F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_MCDDC_CTL_REG 0x0B00103F
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} MAXLAT_MCDDC_CTL_STRUCT;


/* PXPCAP_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x40252040)                                                  */
/*       SKX (0x40252040)                                                     */
/* Register default value:              0x00910010                            */
#define PXPCAP_MCDDC_CTL_REG 0x0B004040
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x040
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
} PXPCAP_MCDDC_CTL_STRUCT;






/* PMONCNTR_0_N0_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520A0)                                                  */
/*       SKX (0x402520A0)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_0_N0_MCDDC_CTL_REG 0x0B0040A0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR_0_N0_MCDDC_CTL_STRUCT;


/* PMONCNTR_0_N1_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520A4)                                                  */
/*       SKX (0x402520A4)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_0_N1_MCDDC_CTL_REG 0x0B0040A4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
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
} PMONCNTR_0_N1_MCDDC_CTL_STRUCT;


/* PMONCNTR_1_N0_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520A8)                                                  */
/*       SKX (0x402520A8)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_1_N0_MCDDC_CTL_REG 0x0B0040A8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR_1_N0_MCDDC_CTL_STRUCT;


/* PMONCNTR_1_N1_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520AC)                                                  */
/*       SKX (0x402520AC)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_1_N1_MCDDC_CTL_REG 0x0B0040AC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
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
} PMONCNTR_1_N1_MCDDC_CTL_STRUCT;


/* PMONCNTR_2_N0_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520B0)                                                  */
/*       SKX (0x402520B0)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_2_N0_MCDDC_CTL_REG 0x0B0040B0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR_2_N0_MCDDC_CTL_STRUCT;


/* PMONCNTR_2_N1_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520B4)                                                  */
/*       SKX (0x402520B4)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_2_N1_MCDDC_CTL_REG 0x0B0040B4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
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
} PMONCNTR_2_N1_MCDDC_CTL_STRUCT;


/* PMONCNTR_3_N0_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520B8)                                                  */
/*       SKX (0x402520B8)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_3_N0_MCDDC_CTL_REG 0x0B0040B8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR_3_N0_MCDDC_CTL_STRUCT;


/* PMONCNTR_3_N1_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520BC)                                                  */
/*       SKX (0x402520BC)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_3_N1_MCDDC_CTL_REG 0x0B0040BC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
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
} PMONCNTR_3_N1_MCDDC_CTL_STRUCT;


/* PMONCNTR_4_N0_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520C0)                                                  */
/*       SKX (0x402520C0)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_4_N0_MCDDC_CTL_REG 0x0B0040C0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR_4_N0_MCDDC_CTL_STRUCT;


/* PMONCNTR_4_N1_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520C4)                                                  */
/*       SKX (0x402520C4)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_4_N1_MCDDC_CTL_REG 0x0B0040C4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
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
} PMONCNTR_4_N1_MCDDC_CTL_STRUCT;


/* PMONDBGCNTRESETVAL_N0_MCDDC_CTL_REG supported on:                          */
/*       SKX_A0 (0x402520C8)                                                  */
/*       SKX (0x402520C8)                                                     */
/* Register default value:              0x00000000                            */
#define PMONDBGCNTRESETVAL_N0_MCDDC_CTL_REG 0x0B0040C8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Perfmon counter reset value.  This is for debug only.  Whenever counter 4 is 
 * reset, it will load this value instead of resetting to zero. 
 */
typedef union {
  struct {
    UINT32 resetval : 32;
    /* resetval - Bits[31:0], RW_L, default = 48'b000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       The value to reset the counter to.
     */
  } Bits;
  UINT32 Data;
} PMONDBGCNTRESETVAL_N0_MCDDC_CTL_STRUCT;


/* PMONDBGCNTRESETVAL_N1_MCDDC_CTL_REG supported on:                          */
/*       SKX_A0 (0x402520CC)                                                  */
/*       SKX (0x402520CC)                                                     */
/* Register default value:              0x00000000                            */
#define PMONDBGCNTRESETVAL_N1_MCDDC_CTL_REG 0x0B0040CC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Perfmon counter reset value.  This is for debug only.  Whenever counter 4 is 
 * reset, it will load this value instead of resetting to zero. 
 */
typedef union {
  struct {
    UINT32 resetval : 16;
    /* resetval - Bits[15:0], RW_L, default = 48'b000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       The value to reset the counter to.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONDBGCNTRESETVAL_N1_MCDDC_CTL_STRUCT;


/* PMONCNTR_FIXED_N0_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402520D0)                                                  */
/*       SKX (0x402520D0)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_FIXED_N0_MCDDC_CTL_REG 0x0B0040D0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       This is the current value of the counter.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR_FIXED_N0_MCDDC_CTL_STRUCT;


/* PMONCNTR_FIXED_N1_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402520D4)                                                  */
/*       SKX (0x402520D4)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR_FIXED_N1_MCDDC_CTL_REG 0x0B0040D4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register is a perfmon counter.  Software can both read it and write it.
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'b000000000000000000000000000000000000000000000000 
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
} PMONCNTR_FIXED_N1_MCDDC_CTL_STRUCT;


/* PMONCNTRCFG_0_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520D8)                                                  */
/*       SKX (0x402520D8)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG_0_MCDDC_CTL_REG 0x0B0040D8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b00000000 
       This field is used to decode the PerfMon event which is selected. The encodings 
       for each of the valid UnCore PerfMon events can be found in the respective 
       performance monitoring sections. 
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b00000000 
       This mask selects the sub-events to be selected for creation of the event. The 
       selected sub-events are bitwise OR-ed together to create event. At least one 
       sub-event must be selected otherwise the PerfMon event signals will not ever get 
       asserted. Events with no sub-events listed effectively have only one sub-event 
       =event -1 bit 8 must be set to 1 in this case. 
     */
    UINT32 queueoccupancyreset : 1;
    /* queueoccupancyreset - Bits[16:16], WO, default = 1'b0 
       This write only bit causes the queue occupancy counter of the PerfMon counter 
       for which this Perf event select register is associated to be cleared to all 
       zeroes when a '1' is written to it. No action is taken when a '0' is written. 
       Note: Since the queue occupancy counters never drop below zero, it is possible 
       for the counters to 'catch up' with the real occupancy of the queue in question 
       when the real occupancy drop to zero. 
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a given 
       event. For example, we have an event that counts the number of cycles in L1 mode 
       in Intel UPI. By using edge detect, one can count the number of times that we 
       entered L1 mode (by detecting the rising edge). 
       Edge detect only works in conjunction with threshholding. This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above). In this case, one should set a threshhold of 1. One can also use Edge 
       Detect with queue occupancy events. For example, if one wanted to count the 
       number of times when the TOR occupancy was larger than 5, one would selet the 
       TOR occupancy event with a threshold of 5 and set the Edge Detect bit. 
       Edge detect can also be used with the invert. This is generally not particularly 
       useful, as the count of falling edges compared to rising edges will always on 
       differ by 1. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything. If this bit is set and the Unit's configuration register has Overflow 
       enabled, then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the PerfMon Counter. This bit must be 
       asserted in order for the PerfMon counter to begin counting the events selected 
       by the event select, unit mask, and internal bits (see the fields below). There 
       is one bit per PerfMon Counter. Note that if this bit is set to 1 but the Unit 
       Control Registers have determined that counting is disabled, then the counter 
       will not count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold >= event. When set 
       to 1, the comparison that will be done is inverted from the case where this bit 
       is set to 0, i.e., threshold < event. The invert bit only works when Threshhold 
       != 0. So, if one would like to invert a non-occupancy event (like LLC Hit), one 
       needs to set the threshhold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b00000000 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle. Since the widest event from the 
       UnCore is 7bits (queue occupancy), bit 31 is unused. The result of the 
       comparison is effectively a 1 bit wide event, i.e., the counter will be 
       incremented by 1 when the comparison is true (the type of comparison depends on 
       the setting of the 'invert' bit - see bit 23 below) no matter how wide the 
       original event was. When this field is zero, threshold comparison is disabled 
       and the event is passed without modification. 
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG_0_MCDDC_CTL_STRUCT;


/* PMONCNTRCFG_1_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520DC)                                                  */
/*       SKX (0x402520DC)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG_1_MCDDC_CTL_REG 0x0B0040DC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b00000000 
       This field is used to decode the PerfMon event which is selected. The encodings 
       for each of the valid UnCore PerfMon events can be found in the respective 
       performance monitoring sections. 
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b00000000 
       This mask selects the sub-events to be selected for creation of the event. The 
       selected sub-events are bitwise OR-ed together to create event. At least one 
       sub-event must be selected otherwise the PerfMon event signals will not ever get 
       asserted. Events with no sub-events listed effectively have only one sub-event 
       =event -1 bit 8 must be set to 1 in this case. 
     */
    UINT32 queueoccupancyreset : 1;
    /* queueoccupancyreset - Bits[16:16], WO, default = 1'b0 
       This write only bit causes the queue occupancy counter of the PerfMon counter 
       for which this Perf event select register is associated to be cleared to all 
       zeroes when a '1' is written to it. No action is taken when a '0' is written. 
       Note: Since the queue occupancy counters never drop below zero, it is possible 
       for the counters to 'catch up' with the real occupancy of the queue in question 
       when the real occupancy drop to zero. 
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a given 
       event. For example, we have an event that counts the number of cycles in L1 mode 
       in Intel UPI. By using edge detect, one can count the number of times that we 
       entered L1 mode (by detecting the rising edge). 
       Edge detect only works in conjunction with threshholding. This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above). In this case, one should set a threshhold of 1. One can also use Edge 
       Detect with queue occupancy events. For example, if one wanted to count the 
       number of times when the TOR occupancy was larger than 5, one would selet the 
       TOR occupancy event with a threshold of 5 and set the Edge Detect bit. 
       Edge detect can also be used with the invert. This is generally not particularly 
       useful, as the count of falling edges compared to rising edges will always on 
       differ by 1. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything. If this bit is set and the Unit's configuration register has Overflow 
       enabled, then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the PerfMon Counter. This bit must be 
       asserted in order for the PerfMon counter to begin counting the events selected 
       by the event select, unit mask, and internal bits (see the fields below). There 
       is one bit per PerfMon Counter. Note that if this bit is set to 1 but the Unit 
       Control Registers have determined that counting is disabled, then the counter 
       will not count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold >= event. When set 
       to 1, the comparison that will be done is inverted from the case where this bit 
       is set to 0, i.e., threshold < event. The invert bit only works when Threshhold 
       != 0. So, if one would like to invert a non-occupancy event (like LLC Hit), one 
       needs to set the threshhold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b00000000 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle. Since the widest event from the 
       UnCore is 7bits (queue occupancy), bit 31 is unused. The result of the 
       comparison is effectively a 1 bit wide event, i.e., the counter will be 
       incremented by 1 when the comparison is true (the type of comparison depends on 
       the setting of the 'invert' bit - see bit 23 below) no matter how wide the 
       original event was. When this field is zero, threshold comparison is disabled 
       and the event is passed without modification. 
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG_1_MCDDC_CTL_STRUCT;


/* PMONCNTRCFG_2_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520E0)                                                  */
/*       SKX (0x402520E0)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG_2_MCDDC_CTL_REG 0x0B0040E0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b00000000 
       This field is used to decode the PerfMon event which is selected. The encodings 
       for each of the valid UnCore PerfMon events can be found in the respective 
       performance monitoring sections. 
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b00000000 
       This mask selects the sub-events to be selected for creation of the event. The 
       selected sub-events are bitwise OR-ed together to create event. At least one 
       sub-event must be selected otherwise the PerfMon event signals will not ever get 
       asserted. Events with no sub-events listed effectively have only one sub-event 
       =event -1 bit 8 must be set to 1 in this case. 
     */
    UINT32 queueoccupancyreset : 1;
    /* queueoccupancyreset - Bits[16:16], WO, default = 1'b0 
       This write only bit causes the queue occupancy counter of the PerfMon counter 
       for which this Perf event select register is associated to be cleared to all 
       zeroes when a '1' is written to it. No action is taken when a '0' is written. 
       Note: Since the queue occupancy counters never drop below zero, it is possible 
       for the counters to 'catch up' with the real occupancy of the queue in question 
       when the real occupancy drop to zero. 
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a given 
       event. For example, we have an event that counts the number of cycles in L1 mode 
       in Intel UPI. By using edge detect, one can count the number of times that we 
       entered L1 mode (by detecting the rising edge). 
       Edge detect only works in conjunction with threshholding. This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above). In this case, one should set a threshhold of 1. One can also use Edge 
       Detect with queue occupancy events. For example, if one wanted to count the 
       number of times when the TOR occupancy was larger than 5, one would selet the 
       TOR occupancy event with a threshold of 5 and set the Edge Detect bit. 
       Edge detect can also be used with the invert. This is generally not particularly 
       useful, as the count of falling edges compared to rising edges will always on 
       differ by 1. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything. If this bit is set and the Unit's configuration register has Overflow 
       enabled, then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the PerfMon Counter. This bit must be 
       asserted in order for the PerfMon counter to begin counting the events selected 
       by the event select, unit mask, and internal bits (see the fields below). There 
       is one bit per PerfMon Counter. Note that if this bit is set to 1 but the Unit 
       Control Registers have determined that counting is disabled, then the counter 
       will not count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold >= event. When set 
       to 1, the comparison that will be done is inverted from the case where this bit 
       is set to 0, i.e., threshold < event. The invert bit only works when Threshhold 
       != 0. So, if one would like to invert a non-occupancy event (like LLC Hit), one 
       needs to set the threshhold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b00000000 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle. Since the widest event from the 
       UnCore is 7bits (queue occupancy), bit 31 is unused. The result of the 
       comparison is effectively a 1 bit wide event, i.e., the counter will be 
       incremented by 1 when the comparison is true (the type of comparison depends on 
       the setting of the 'invert' bit - see bit 23 below) no matter how wide the 
       original event was. When this field is zero, threshold comparison is disabled 
       and the event is passed without modification. 
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG_2_MCDDC_CTL_STRUCT;


/* PMONCNTRCFG_3_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520E4)                                                  */
/*       SKX (0x402520E4)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG_3_MCDDC_CTL_REG 0x0B0040E4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b00000000 
       This field is used to decode the PerfMon event which is selected. The encodings 
       for each of the valid UnCore PerfMon events can be found in the respective 
       performance monitoring sections. 
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b00000000 
       This mask selects the sub-events to be selected for creation of the event. The 
       selected sub-events are bitwise OR-ed together to create event. At least one 
       sub-event must be selected otherwise the PerfMon event signals will not ever get 
       asserted. Events with no sub-events listed effectively have only one sub-event 
       =event -1 bit 8 must be set to 1 in this case. 
     */
    UINT32 queueoccupancyreset : 1;
    /* queueoccupancyreset - Bits[16:16], WO, default = 1'b0 
       This write only bit causes the queue occupancy counter of the PerfMon counter 
       for which this Perf event select register is associated to be cleared to all 
       zeroes when a '1' is written to it. No action is taken when a '0' is written. 
       Note: Since the queue occupancy counters never drop below zero, it is possible 
       for the counters to 'catch up' with the real occupancy of the queue in question 
       when the real occupancy drop to zero. 
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a given 
       event. For example, we have an event that counts the number of cycles in L1 mode 
       in Intel UPI. By using edge detect, one can count the number of times that we 
       entered L1 mode (by detecting the rising edge). 
       Edge detect only works in conjunction with threshholding. This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above). In this case, one should set a threshhold of 1. One can also use Edge 
       Detect with queue occupancy events. For example, if one wanted to count the 
       number of times when the TOR occupancy was larger than 5, one would selet the 
       TOR occupancy event with a threshold of 5 and set the Edge Detect bit. 
       Edge detect can also be used with the invert. This is generally not particularly 
       useful, as the count of falling edges compared to rising edges will always on 
       differ by 1. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything. If this bit is set and the Unit's configuration register has Overflow 
       enabled, then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the PerfMon Counter. This bit must be 
       asserted in order for the PerfMon counter to begin counting the events selected 
       by the event select, unit mask, and internal bits (see the fields below). There 
       is one bit per PerfMon Counter. Note that if this bit is set to 1 but the Unit 
       Control Registers have determined that counting is disabled, then the counter 
       will not count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold >= event. When set 
       to 1, the comparison that will be done is inverted from the case where this bit 
       is set to 0, i.e., threshold < event. The invert bit only works when Threshhold 
       != 0. So, if one would like to invert a non-occupancy event (like LLC Hit), one 
       needs to set the threshhold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b00000000 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle. Since the widest event from the 
       UnCore is 7bits (queue occupancy), bit 31 is unused. The result of the 
       comparison is effectively a 1 bit wide event, i.e., the counter will be 
       incremented by 1 when the comparison is true (the type of comparison depends on 
       the setting of the 'invert' bit - see bit 23 below) no matter how wide the 
       original event was. When this field is zero, threshold comparison is disabled 
       and the event is passed without modification. 
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG_3_MCDDC_CTL_STRUCT;


/* PMONCNTRCFG_4_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x402520E8)                                                  */
/*       SKX (0x402520E8)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG_4_MCDDC_CTL_REG 0x0B0040E8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Perfmon Counter Control Register
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'b00000000 
       This field is used to decode the PerfMon event which is selected. The encodings 
       for each of the valid UnCore PerfMon events can be found in the respective 
       performance monitoring sections. 
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'b00000000 
       This mask selects the sub-events to be selected for creation of the event. The 
       selected sub-events are bitwise OR-ed together to create event. At least one 
       sub-event must be selected otherwise the PerfMon event signals will not ever get 
       asserted. Events with no sub-events listed effectively have only one sub-event 
       =event -1 bit 8 must be set to 1 in this case. 
     */
    UINT32 queueoccupancyreset : 1;
    /* queueoccupancyreset - Bits[16:16], WO, default = 1'b0 
       This write only bit causes the queue occupancy counter of the PerfMon counter 
       for which this Perf event select register is associated to be cleared to all 
       zeroes when a '1' is written to it. No action is taken when a '0' is written. 
       Note: Since the queue occupancy counters never drop below zero, it is possible 
       for the counters to 'catch up' with the real occupancy of the queue in question 
       when the real occupancy drop to zero. 
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'b0 
       When this bit is set, the corresponding counter will be reset to 0. This allows 
       for a quick reset of the counter when changing event encodings. 
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'b0 
       Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a given 
       event. For example, we have an event that counts the number of cycles in L1 mode 
       in Intel UPI. By using edge detect, one can count the number of times that we 
       entered L1 mode (by detecting the rising edge). 
       Edge detect only works in conjunction with threshholding. This is true even for 
       events that can only increment by 1 in a given cycle (like the L1 example 
       above). In this case, one should set a threshhold of 1. One can also use Edge 
       Detect with queue occupancy events. For example, if one wanted to count the 
       number of times when the TOR occupancy was larger than 5, one would selet the 
       TOR occupancy event with a threshold of 5 and set the Edge Detect bit. 
       Edge detect can also be used with the invert. This is generally not particularly 
       useful, as the count of falling edges compared to rising edges will always on 
       differ by 1. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'b0 
       Setting this bit will enable the counter to send an overflow signal. If this bit 
       is not set, the counter will wrap around when it overflows without triggering 
       anything. If this bit is set and the Unit's configuration register has Overflow 
       enabled, then a signal will be transmitted to the Ubox. 
     */
    UINT32 rsvd_21 : 1;
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'b0 
       This field is the local enable for the PerfMon Counter. This bit must be 
       asserted in order for the PerfMon counter to begin counting the events selected 
       by the event select, unit mask, and internal bits (see the fields below). There 
       is one bit per PerfMon Counter. Note that if this bit is set to 1 but the Unit 
       Control Registers have determined that counting is disabled, then the counter 
       will not count. 
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'b0 
       This bit indicates how the threshold field will be compared to the incoming 
       event. When 0, the comparison that will be done is threshold >= event. When set 
       to 1, the comparison that will be done is inverted from the case where this bit 
       is set to 0, i.e., threshold < event. The invert bit only works when Threshhold 
       != 0. So, if one would like to invert a non-occupancy event (like LLC Hit), one 
       needs to set the threshhold to 1. 
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'b00000000 
       This field is compared directly against an incoming event value for events that 
       can increment by 1 or more in a given cycle. Since the widest event from the 
       UnCore is 7bits (queue occupancy), bit 31 is unused. The result of the 
       comparison is effectively a 1 bit wide event, i.e., the counter will be 
       incremented by 1 when the comparison is true (the type of comparison depends on 
       the setting of the 'invert' bit - see bit 23 below) no matter how wide the 
       original event was. When this field is zero, threshold comparison is disabled 
       and the event is passed without modification. 
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG_4_MCDDC_CTL_STRUCT;


/* PMONDBGCTRL_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x402520EC)                                                  */
/*       SKX (0x402520EC)                                                     */
/* Register default value:              0x00000000                            */
#define PMONDBGCTRL_MCDDC_CTL_REG 0x0B0040EC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Control register for the specal debug wrapper around counter 4 in the Home 
 * Agent. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 othereventstopen : 1;
    /* othereventstopen - Bits[1:1], RW_L, default = 1'b0 
       When this is set, the counter's enable bit will be set to 0 whenever the partner 
       counter's event occurs. This should be used with the Pulse Width and A after B 
       usage models. 
     */
    UINT32 rsvd_2 : 3;
    /* rsvd_2 - Bits[4:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 othereventstarten : 1;
    /* othereventstarten - Bits[5:5], RW_L, default = 1'b0 
       When this bit is set, the counter's enable bit will be set to 1 whenever the 
       partner counter's event occurs. This is an optional event, which is generally 
       intended for cases when we need to use the queue occupancy counter, which only 
       exists on counter 3. 
     */
    UINT32 rsvd_6 : 1;
    /* rsvd_6 - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 myeventstarten : 1;
    /* myeventstarten - Bits[7:7], RW_L, default = 1'b0 
       When this is set, the counter's enable bit will automatically be set to 1 
       whenever the event occurs. It is generally used with the ClockedIncEn bit. It is 
       used in the Pulse Widge, A after B, and Inactivity usage models. 
     */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 othereventreseten : 1;
    /* othereventreseten - Bits[9:9], RW_L, default = 1'b0 
       When this bit is set, the counter will reset whenever the partner counter's 
       event occurs. This is used by the Pulse Width and A after B usage models. 
     */
    UINT32 rsvd_10 : 1;
    /* rsvd_10 - Bits[10:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 myeventreseten : 1;
    /* myeventreseten - Bits[11:11], RW_L, default = 1'b0 
       When this is enabled, the counter will reset whenever the counter's event is 
       triggered. This is used by the Inactivity usage model. 
     */
    UINT32 rsvd_12 : 1;
    /* rsvd_12 - Bits[12:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 clockedincenable : 1;
    /* clockedincenable - Bits[13:13], RW_L, default = 1'b0 
       Changes when the counter increments. Rather than incrementing based on the 
       event, the counter will increment by 1 in each cycle. This is used by the Pulse 
       Widge, A after B, and Inactivity usage models. 
     */
    UINT32 rsvd_14 : 18;
    /* rsvd_14 - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONDBGCTRL_MCDDC_CTL_STRUCT;




/* PMONUNITCTRL_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x402520F4)                                                  */
/*       SKX (0x402520F4)                                                     */
/* Register default value:              0x00030000                            */
#define PMONUNITCTRL_MCDDC_CTL_REG 0x0B0040F4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x0f4
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
    UINT32 freezeenable : 1;
    /* freezeenable - Bits[16:16], RW_LB, default = 1'b1 
       This bit controls what the counters in the unit will do when they receive a 
       freeze signal. When set, the counters will be allowed to freeze. When not set, 
       the counters will ignore the freeze signal. 
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[17:17], RW_LB, default = 1'b1 
       This bit controls the behavior of counters when they overflow. When set, the 
       system will trigger the overflow handling process throughout the rest of the 
       uncore, potentially triggering a PMI and freezing counters. When it is not set, 
       the counters will simply wrap around and continue to count. 
     */
    UINT32 rsvd_18 : 14;
    /* rsvd_18 - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONUNITCTRL_MCDDC_CTL_STRUCT;


/* PMONUNITSTATUS_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x402520F8)                                                  */
/*       SKX (0x402520F8)                                                     */
/* Register default value:              0x00000000                            */
#define PMONUNITSTATUS_MCDDC_CTL_REG 0x0B0040F8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This field shows which registers have overflowed in the unit.
 * Whenever a register overflows, it should set the relevant bit to 1.  An overflow 
 * should not effect the other status bits.  This status should only be cleared by 
 * software. 
 * We have defined 7 bits for this status.  This is overkill for many units.  See 
 * below for the bits that are used in the different units. 
 * In general, if the unit has a fixed counter, it will use bit 0.  Counter 0 would 
 * use the next LSB, and the largest counter would use the MSB. 
 * HA: [4:0] w/ [4] = Counter4 and [0] = Counter 0
 * IMC: [5:0] w/ [0] = Fixed; [1] = Counter0 and [5] = Counter4
 * Intel UPI: [4:0] (same as HA)
 * PCU: [3:0]: [0] = Counter0 and [3] = Counter 3
 * IO IRP0: [0] = Counter0; [1] = Counter1
 * IO IRP1: [2] = Counter0; [3] = Counter1
 */
typedef union {
  struct {
    UINT32 counteroverflowbitmask : 7;
    /* counteroverflowbitmask - Bits[6:0], RW1C, default = 7'b0000000 
       This is a bitmask that specifies which counter (or counters) have overflowed. If 
       the unit has a fixed counter, it's corresponding bitmask will be stored at 
       position 0. 
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONUNITSTATUS_MCDDC_CTL_STRUCT;


/* PXPENHCAP_MCDDC_CTL_REG supported on:                                      */
/*       SKX_A0 (0x40252100)                                                  */
/*       SKX (0x40252100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_MCDDC_CTL_REG 0x0B004100
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
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
} PXPENHCAP_MCDDC_CTL_STRUCT;


/* ET_CFG_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x40252104)                                                  */
/*       SKX (0x40252104)                                                     */
/* Register default value:              0x00000100                            */
#define ET_CFG_MCDDC_CTL_REG 0x0B004104
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x104
 */
typedef union {
  struct {
    UINT32 et_smpl_prd : 8;
    /* et_smpl_prd - Bits[7:0], RW_LB, default = 8'b00000000 
       Energy calculation sample period (in number of DCLK). This value is loaded onto 
       the corresponding ETSAMPLEPERIOD count-down counter. The counter is reload with 
       the ETSAMPLEPERIOD count after it counted zero. Here is the recommended setting 
       when ET_EN is enabled: 
       DCLK Setting
       400MHz 0x33
       533MHz 0x44
       667MHz 0x55
       800MHz 0x66
       933MHz 0x77
       However, the setting is subject to change per platform power delivery 
       recommendation. 
     */
    UINT32 et_div : 2;
    /* et_div - Bits[9:8], RW_LB, default = 2'b01 
       Energy equation divider control
       00: divider=2 (the energy counter is right shift by 1 bit)
       01: divider=4 (the energy counter is right shift by 2 bit)
       10: divider=8 (the energy counter is right shift by 3 bit)
       11: divider=16 (the energy counter is right shift by 4 bit)
     */
    UINT32 rsvd_10 : 5;
    /* rsvd_10 - Bits[14:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 et_en : 1;
    /* et_en - Bits[15:15], RW_LB, default = 1'b0 
       Enable Electrical Throttling
     */
    UINT32 rsvd_16 : 16;
    /* rsvd_16 - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ET_CFG_MCDDC_CTL_STRUCT;


/* CHN_TEMP_CFG_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x40252108)                                                  */
/*       SKX (0x40252108)                                                     */
/* Register default value:              0x980003FF                            */
#define CHN_TEMP_CFG_MCDDC_CTL_REG 0x0B004108
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x108
 */
typedef union {
  struct {
    UINT32 bw_limit_tf : 11;
    /* bw_limit_tf - Bits[10:0], RW, default = 11'b01111111111 
       (BW Throttle Window Size in DCLK) / 8 - description change from b307202
     */
    UINT32 rsvd_11 : 4;
    /* rsvd_11 - Bits[14:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 thrt_allow_isoch : 1;
    /* thrt_allow_isoch - Bits[15:15], RW, default = 1'b0 
       When this bit is zero, MC will lower CKE during Thermal Throttling, and ISOCH is 
       blocked. When this bit is one, MC will NOT lower CKE during Thermal Throttling, 
       and ISOCH will be allowed base on bandwidth throttling setting. However, setting 
       this bit would mean more power consumption due to CKE is asserted during thermal 
       or power throttling. 
       
     */
    UINT32 thrt_ext : 8;
    /* thrt_ext - Bits[23:16], RW, default = 8'b00000000 
       Max number of throttled transactions to be issued during BWLIMITTF due to 
       externally asserted MEMHOT#. 
     */
    UINT32 rsvd_24 : 3;
    /* rsvd_24 - Bits[26:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 bw_limit_thrt_en : 1;
    /* bw_limit_thrt_en - Bits[27:27], RW, default = 1'b1  */
    UINT32 cltt_debug_disable_lock : 1;
    /* cltt_debug_disable_lock - Bits[28:28], RW_O, default = 1'b1 
       lock bit of DIMMTEMPSTAT_[0:3][7:0]:Set this lock bit to disable configuration 
       write to DIMMTEMPSTAT_[0:3][7:0]. When this bit is clear, system debug/test 
       software can update the DIMMTEMPSTAT_[0:3][7:0] to verify various temperature 
       scenerios. 
     */
    UINT32 cltt_or_pcode_temp_mux_sel : 1;
    /* cltt_or_pcode_temp_mux_sel - Bits[29:29], RW, default = 1'b0 
       The TEMP_STAT byte update mux select control to direct the source to update 
       DIMMTEMPSTAT_[0:3][7:0]:0: Corresponding to the DIMM TEMP_STAT byte from 
       PCODE_TEMP_OUTPUT. 
       1: TSOD temperature reading from CLTT logic.
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 oltt_en : 1;
    /* oltt_en - Bits[31:31], RW, default = 1'b1 
       Enable OLTT temperature tracking
     */
  } Bits;
  UINT32 Data;
} CHN_TEMP_CFG_MCDDC_CTL_STRUCT;


/* CHN_TEMP_STAT_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x4025210C)                                                  */
/*       SKX (0x4025210C)                                                     */
/* Register default value:              0x00000000                            */
#define CHN_TEMP_STAT_MCDDC_CTL_REG 0x0B00410C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x10c
 */
typedef union {
  struct {
    UINT32 ev_asrt_dimm0 : 1;
    /* ev_asrt_dimm0 - Bits[0:0], RW1C, default = 1'b0 
       Event Asserted on DIMM ID 0
     */
    UINT32 ev_asrt_dimm1 : 1;
    /* ev_asrt_dimm1 - Bits[1:1], RW1C, default = 1'b0 
       Event Asserted on DIMM ID 1
     */
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CHN_TEMP_STAT_MCDDC_CTL_STRUCT;


/* DIMM_TEMP_OEM_0_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252110)                                                  */
/*       SKX (0x40252110)                                                     */
/* Register default value:              0x0000504B                            */
#define DIMM_TEMP_OEM_0_MCDDC_CTL_REG 0x0B004110
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x110
 */
typedef union {
  struct {
    UINT32 temp_oem_lo : 8;
    /* temp_oem_lo - Bits[7:0], RW, default = 8'b01001011 
       Lower Threshold Value - TCase threshold at which to Initiate System Interrupt 
       (SMI or MEMHOT#) at a - going rate. Note: the default value is listed in 
       decimal.valid range: 32 - 127 in degree C. 
       Others: reserved.
     */
    UINT32 temp_oem_hi : 8;
    /* temp_oem_hi - Bits[15:8], RW, default = 8'b01010000 
       Upper Threshold value - TCase threshold at which to Initiate System Interrupt 
       (SMI or MEMHOT#) at a + going rate. Note: the default value is listed in 
       decimal.valid range: 32 - 127 in degree C. 
       Others: reserved.
     */
    UINT32 temp_oem_lo_hyst : 3;
    /* temp_oem_lo_hyst - Bits[18:16], RW, default = 3'b000 
       Negative going Threshold Hysteresis Value. This value is added to TEMPOEMLO to 
       determine the point where the asserted status for that threshold will clear. Set 
       to 00h if sensor does not support negative-going threshold hysteresis. 
     */
    UINT32 rsvd_19 : 5;
    /* rsvd_19 - Bits[23:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 temp_oem_hi_hyst : 3;
    /* temp_oem_hi_hyst - Bits[26:24], RW, default = 3'b000 
       Positive going Threshold Hysteresis Value. This value is subtracted from 
       TEMPOEMHI to determine the point where the asserted status for that threshold 
       will clear. Set to 00h if sensor does not support positive-going threshold 
       hysteresis 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMM_TEMP_OEM_0_MCDDC_CTL_STRUCT;


/* DIMM_TEMP_OEM_1_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252114)                                                  */
/*       SKX (0x40252114)                                                     */
/* Register default value:              0x0000504B                            */
#define DIMM_TEMP_OEM_1_MCDDC_CTL_REG 0x0B004114
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x114
 */
typedef union {
  struct {
    UINT32 temp_oem_lo : 8;
    /* temp_oem_lo - Bits[7:0], RW, default = 8'b01001011 
       Lower Threshold Value - TCase threshold at which to Initiate System Interrupt 
       (SMI or MEMHOT#) at a - going rate. Note: the default value is listed in 
       decimal.valid range: 32 - 127 in degree C. 
       Others: reserved.
     */
    UINT32 temp_oem_hi : 8;
    /* temp_oem_hi - Bits[15:8], RW, default = 8'b01010000 
       Upper Threshold value - TCase threshold at which to Initiate System Interrupt 
       (SMI or MEMHOT#) at a + going rate. Note: the default value is listed in 
       decimal.valid range: 32 - 127 in degree C. 
       Others: reserved.
     */
    UINT32 temp_oem_lo_hyst : 3;
    /* temp_oem_lo_hyst - Bits[18:16], RW, default = 3'b000 
       Negative going Threshold Hysteresis Value. This value is added to TEMPOEMLO to 
       determine the point where the asserted status for that threshold will clear. Set 
       to 00h if sensor does not support negative-going threshold hysteresis. 
     */
    UINT32 rsvd_19 : 5;
    /* rsvd_19 - Bits[23:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 temp_oem_hi_hyst : 3;
    /* temp_oem_hi_hyst - Bits[26:24], RW, default = 3'b000 
       Positive going Threshold Hysteresis Value. This value is subtracted from 
       TEMPOEMHI to determine the point where the asserted status for that threshold 
       will clear. Set to 00h if sensor does not support positive-going threshold 
       hysteresis 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMM_TEMP_OEM_1_MCDDC_CTL_STRUCT;


/* DIMM_TEMP_TH_0_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252120)                                                  */
/*       SKX (0x40252120)                                                     */
/* Register default value:              0x005F5A55                            */
#define DIMM_TEMP_TH_0_MCDDC_CTL_REG 0x0B004120
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x120
 */
typedef union {
  struct {
    UINT32 temp_lo : 8;
    /* temp_lo - Bits[7:0], RW, default = 8'b01010101 
       TCase threshold at which to Initiate 2x refresh and/or THRTMID and initiate 
       Interrupt (MEMHOT#). 
       Note: the default value is listed in decimal.valid range: 32 - 127 in degree C.
       FF: Disabled
       Others: reserved.
       TEMP_LO should be programmed so it is less than TEMP_MID
     */
    UINT32 temp_mid : 8;
    /* temp_mid - Bits[15:8], RW, default = 8'b01011010 
       TCase threshold at which to Initiate THRTHI and assert valid range: 32 - 127 in 
       degree C. 
       Note: the default value is listed in decimal.
       FF: Disabled
       Others: reserved.
       TEMP_MID should be programmed so it is less than TEMP_HI
     */
    UINT32 temp_hi : 8;
    /* temp_hi - Bits[23:16], RW_LB, default = 8'b01011111 
       TCase threshold at which to Initiate THRTCRIT and assert THERMTRIP# valid range: 
       32 - 127 in degree C. Note: the default value is listed in decimal. 
       FF: Disabled
       Others: reserved.
       TEMP_HI should be programmed so it is greater than TEMP_MID
     */
    UINT32 temp_thrt_hyst : 3;
    /* temp_thrt_hyst - Bits[26:24], RW_LB, default = 3'b000 
       Positive going Threshold Hysteresis Value. Set to 00h if sensor does not support 
       positive-going threshold hysteresis. This value is subtracted from TEMP_THRT_XX 
       to determine the point where the asserted status for that threshold will clear. 
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMM_TEMP_TH_0_MCDDC_CTL_STRUCT;


/* DIMM_TEMP_TH_1_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252124)                                                  */
/*       SKX (0x40252124)                                                     */
/* Register default value:              0x005F5A55                            */
#define DIMM_TEMP_TH_1_MCDDC_CTL_REG 0x0B004124
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x124
 */
typedef union {
  struct {
    UINT32 temp_lo : 8;
    /* temp_lo - Bits[7:0], RW, default = 8'b01010101 
       TCase threshold at which to Initiate 2x refresh and/or THRTMID and initiate 
       Interrupt (MEMHOT#). 
       Note: the default value is listed in decimal.valid range: 32 - 127 in degree C.
       FF: Disabled
       Others: reserved.
       TEMP_LO should be programmed so it is less than TEMP_MID
     */
    UINT32 temp_mid : 8;
    /* temp_mid - Bits[15:8], RW, default = 8'b01011010 
       TCase threshold at which to Initiate THRTHI and assert valid range: 32 - 127 in 
       degree C. 
       Note: the default value is listed in decimal.
       FF: Disabled
       Others: reserved.
       TEMP_MID should be programmed so it is less than TEMP_HI
     */
    UINT32 temp_hi : 8;
    /* temp_hi - Bits[23:16], RW_LB, default = 8'b01011111 
       TCase threshold at which to Initiate THRTCRIT and assert THERMTRIP# valid range: 
       32 - 127 in degree C. Note: the default value is listed in decimal. 
       FF: Disabled
       Others: reserved.
       TEMP_HI should be programmed so it is greater than TEMP_MID
     */
    UINT32 temp_thrt_hyst : 3;
    /* temp_thrt_hyst - Bits[26:24], RW_LB, default = 3'b000 
       Positive going Threshold Hysteresis Value. Set to 00h if sensor does not support 
       positive-going threshold hysteresis. This value is subtracted from TEMP_THRT_XX 
       to determine the point where the asserted status for that threshold will clear. 
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMM_TEMP_TH_1_MCDDC_CTL_STRUCT;


/* DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_REG supported on:                           */
/*       SKX_A0 (0x40252130)                                                  */
/*       SKX (0x40252130)                                                     */
/* Register default value:              0x00000FFF                            */
#define DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_REG 0x0B004130
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * All three THRT_CRIT, THRT_HI and THRT_MID are per DIMM BW limit, i.e. all 
 * activities (ACT, READ, WRITE) from all ranks within a DIMM are tracked together 
 * in one DIMM activity counter. 
 * These throttle limits for hi and crit are also used during scalable memory 
 * buffer thermal throttling. 
 */
typedef union {
  struct {
    UINT32 thrt_mid : 8;
    /* thrt_mid - Bits[7:0], RW, default = 8'b11111111 
       Max number of throttled transactions (ACT, READ, WRITE) to be issued during 
       BWLIMITTF. 
     */
    UINT32 thrt_hi : 8;
    /* thrt_hi - Bits[15:8], RW_LB, default = 8'b00001111 
       Max number of throttled transactions (ACT, READ, WRITE) to be issued during 
       BWLIMITTF. 
     */
    UINT32 thrt_crit : 8;
    /* thrt_crit - Bits[23:16], RW_LB, default = 8'b00000000 
       Max number of throttled transactions (ACT, READ, WRITE) to be issued during 
       BWLIMITTF. 
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_STRUCT;


/* DIMM_TEMP_THRT_LMT_1_MCDDC_CTL_REG supported on:                           */
/*       SKX_A0 (0x40252134)                                                  */
/*       SKX (0x40252134)                                                     */
/* Register default value:              0x00000FFF                            */
#define DIMM_TEMP_THRT_LMT_1_MCDDC_CTL_REG 0x0B004134
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * All three THRT_CRIT, THRT_HI and THRT_MID are per DIMM BW limit, i.e. all 
 * activities (ACT, READ, WRITE) from all ranks within a DIMM are tracked together 
 * in one DIMM activity counter. 
 * These throttle limits for hi and crit are also used during scalable memory 
 * buffer thermal throttling. 
 */
typedef union {
  struct {
    UINT32 thrt_mid : 8;
    /* thrt_mid - Bits[7:0], RW, default = 8'b11111111 
       Max number of throttled transactions (ACT, READ, WRITE) to be issued during 
       BWLIMITTF. 
     */
    UINT32 thrt_hi : 8;
    /* thrt_hi - Bits[15:8], RW_LB, default = 8'b00001111 
       Max number of throttled transactions (ACT, READ, WRITE) to be issued during 
       BWLIMITTF. 
     */
    UINT32 thrt_crit : 8;
    /* thrt_crit - Bits[23:16], RW_LB, default = 8'b00000000 
       Max number of throttled transactions (ACT, READ, WRITE) to be issued during 
       BWLIMITTF. 
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMM_TEMP_THRT_LMT_1_MCDDC_CTL_STRUCT;


/* DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x40252140)                                                  */
/*       SKX (0x40252140)                                                     */
/* Register default value:              0x00002000                            */
#define DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG 0x0B004140
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x140
 */
typedef union {
  struct {
    UINT32 dimm_temp_offset : 4;
    /* dimm_temp_offset - Bits[3:0], RW, default = 4'b0000 
       Bit 3-0 - Temperature Offset Register
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ev_mh_tempoemlo_en : 1;
    /* ev_mh_tempoemlo_en - Bits[8:8], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPOEMLO
     */
    UINT32 ev_mh_tempoemhi_en : 1;
    /* ev_mh_tempoemhi_en - Bits[9:9], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPOEMHI
     */
    UINT32 ev_mh_templo_en : 1;
    /* ev_mh_templo_en - Bits[10:10], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPLO
     */
    UINT32 ev_mh_tempmid_en : 1;
    /* ev_mh_tempmid_en - Bits[11:11], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPMID
     */
    UINT32 ev_mh_temphi_en : 1;
    /* ev_mh_temphi_en - Bits[12:12], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPHI
     */
    UINT32 ev_2x_ref_templo_en : 1;
    /* ev_2x_ref_templo_en - Bits[13:13], RW, default = 1'b1 
       Initiate 2X refresh on TEMPLO
       DIMM with extended temperature range capability will need double refresh rate in 
       order to avoid data lost when DIMM temperature is above 85C but below 95C. 
       Warning: If the 2x refresh is disable with extended temperature range DIMM 
       configuration, system cooling and power thermal throttling scheme must guarantee 
       the DIMM temperature will not exceed 85C. 
     */
    UINT32 ev_thrtmid_templo : 1;
    /* ev_thrtmid_templo - Bits[14:14], RW, default = 1'b0 
       Initiate THRTMID on TEMPLO
     */
    UINT32 rsvd_15 : 9;
    /* rsvd_15 - Bits[23:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 temp_avg_intrvl : 8;
    /* temp_avg_intrvl - Bits[31:24], RO, default = 8'b00000000 
       Temperature data is averaged over this period. At the end of averaging period 
       (ms) , averaging process starts again. 0x1 - 0xFF = Averaging data is read via 
       TEMPDIMM STATUSREGISTER (Byte 1/2) as well as used for generating hysteresis 
       based interrupts. 
       00 = Instantaneous Data (non-averaged) is read via TEMPDIMM STATUSREGISTER (Byte 
       1/2) as well as used for generating hysteresis based interrupts. 
       Note: The processor does not support temp averaging.
     */
  } Bits;
  UINT32 Data;
} DIMM_TEMP_EV_OFST_0_MCDDC_CTL_STRUCT;


/* DIMM_TEMP_EV_OFST_1_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x40252144)                                                  */
/*       SKX (0x40252144)                                                     */
/* Register default value:              0x00002000                            */
#define DIMM_TEMP_EV_OFST_1_MCDDC_CTL_REG 0x0B004144
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x144
 */
typedef union {
  struct {
    UINT32 dimm_temp_offset : 4;
    /* dimm_temp_offset - Bits[3:0], RW, default = 4'b0000 
       Bit 3-0 - Temperature Offset Register
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ev_mh_tempoemlo_en : 1;
    /* ev_mh_tempoemlo_en - Bits[8:8], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPOEMLO
     */
    UINT32 ev_mh_tempoemhi_en : 1;
    /* ev_mh_tempoemhi_en - Bits[9:9], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPOEMHI
     */
    UINT32 ev_mh_templo_en : 1;
    /* ev_mh_templo_en - Bits[10:10], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPLO
     */
    UINT32 ev_mh_tempmid_en : 1;
    /* ev_mh_tempmid_en - Bits[11:11], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPMID
     */
    UINT32 ev_mh_temphi_en : 1;
    /* ev_mh_temphi_en - Bits[12:12], RW_LB, default = 1'b0 
       Assert MEMHOT# Event on TEMPHI
     */
    UINT32 ev_2x_ref_templo_en : 1;
    /* ev_2x_ref_templo_en - Bits[13:13], RW, default = 1'b1 
       Initiate 2X refresh on TEMPLO
       DIMM with extended temperature range capability will need double refresh rate in 
       order to avoid data lost when DIMM temperature is above 85C but below 95C. 
       Warning: If the 2x refresh is disable with extended temperature range DIMM 
       configuration, system cooling and power thermal throttling scheme must guarantee 
       the DIMM temperature will not exceed 85C. 
     */
    UINT32 ev_thrtmid_templo : 1;
    /* ev_thrtmid_templo - Bits[14:14], RW, default = 1'b0 
       Initiate THRTMID on TEMPLO
     */
    UINT32 rsvd_15 : 9;
    /* rsvd_15 - Bits[23:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 temp_avg_intrvl : 8;
    /* temp_avg_intrvl - Bits[31:24], RO, default = 8'b00000000 
       Temperature data is averaged over this period. At the end of averaging period 
       (ms) , averaging process starts again. 0x1 - 0xFF = Averaging data is read via 
       TEMPDIMM STATUSREGISTER (Byte 1/2) as well as used for generating hysteresis 
       based interrupts. 
       00 = Instantaneous Data (non-averaged) is read via TEMPDIMM STATUSREGISTER (Byte 
       1/2) as well as used for generating hysteresis based interrupts. 
       Note: The processor does not support temp averaging.
     */
  } Bits;
  UINT32 Data;
} DIMM_TEMP_EV_OFST_1_MCDDC_CTL_STRUCT;


/* DIMMTEMPSTAT_0_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252150)                                                  */
/*       SKX (0x40252150)                                                     */
/* Register default value:              0x00000055                            */
#define DIMMTEMPSTAT_0_MCDDC_CTL_REG 0x0B004150
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x150
 */
typedef union {
  struct {
    UINT32 dimm_temp : 8;
    /* dimm_temp - Bits[7:0], RW_LV, default = 8'b01010101 
       Current DIMM Temperature for thermal throttling
     */
    UINT32 rsvd_8 : 16;
    /* rsvd_8 - Bits[23:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ev_asrt_tempoemhi : 1;
    /* ev_asrt_tempoemhi - Bits[24:24], RW1C, default = 1'b0 
       Event Asserted on TEMPOEMHI Going High
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 ev_asrt_tempoemlo : 1;
    /* ev_asrt_tempoemlo - Bits[25:25], RW1C, default = 1'b0 
       Event Asserted on TEMPOEMLO Going Low
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 ev_asrt_templo : 1;
    /* ev_asrt_templo - Bits[26:26], RW1C, default = 1'b0 
       Event Asserted on TEMPLO Going High
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 ev_asrt_tempmid : 1;
    /* ev_asrt_tempmid - Bits[27:27], RW1C, default = 1'b0 
       Event Asserted on TEMPMID going High
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 ev_asrt_temphi : 1;
    /* ev_asrt_temphi - Bits[28:28], RW1C, default = 1'b0 
       Event Asserted on TEMPHI going HIGH
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMMTEMPSTAT_0_MCDDC_CTL_STRUCT;


/* DIMMTEMPSTAT_1_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252154)                                                  */
/*       SKX (0x40252154)                                                     */
/* Register default value:              0x00000055                            */
#define DIMMTEMPSTAT_1_MCDDC_CTL_REG 0x0B004154
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x154
 */
typedef union {
  struct {
    UINT32 dimm_temp : 8;
    /* dimm_temp - Bits[7:0], RW_LV, default = 8'b01010101 
       Current DIMM Temperature for thermal throttling
     */
    UINT32 rsvd_8 : 16;
    /* rsvd_8 - Bits[23:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ev_asrt_tempoemhi : 1;
    /* ev_asrt_tempoemhi - Bits[24:24], RW1C, default = 1'b0 
       Event Asserted on TEMPOEMHI Going High
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 ev_asrt_tempoemlo : 1;
    /* ev_asrt_tempoemlo - Bits[25:25], RW1C, default = 1'b0 
       Event Asserted on TEMPOEMLO Going Low
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 ev_asrt_templo : 1;
    /* ev_asrt_templo - Bits[26:26], RW1C, default = 1'b0 
       Event Asserted on TEMPLO Going High
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 ev_asrt_tempmid : 1;
    /* ev_asrt_tempmid - Bits[27:27], RW1C, default = 1'b0 
       Event Asserted on TEMPMID going High
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 ev_asrt_temphi : 1;
    /* ev_asrt_temphi - Bits[28:28], RW1C, default = 1'b0 
       Event Asserted on TEMPHI going HIGH
       It is assumed that each of the event assertion is going to trigger
       Configurable interrupt (Either MEMHOT# only or both SMI and
       MEMHOT#) defined in bit 30 of CHN_TEMP_CFG
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DIMMTEMPSTAT_1_MCDDC_CTL_STRUCT;


/* THRT_PWR_CHNL_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x2025215C)                                                  */
/*       SKX (0x2025215C)                                                     */
/* Register default value:              0x7FFF                                */
#define THRT_PWR_CHNL_MCDDC_CTL_REG 0x0B00215C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Max number of transactions (ACT, READ, WRITE) to be allowed during the 1 usec 
 * throttling timeframe per power throttling. 
 */
typedef union {
  struct {
    UINT16 chnl_thrt_pwr : 15;
    /* chnl_thrt_pwr - Bits[14:0], RW, default = 15'b111111111111111 
       Max amount of transaction energy to be allowed (per channel) during the 1 usec 
       throttling timeframe per power throttling. 
       PCODE can update this register dynamically. Note that the machine shifts this 
       field left by one so the number written to the register is multiplied by 2 
       before being used. 
     */
    UINT16 chnl_thrt_pwr_en : 1;
    /* chnl_thrt_pwr_en - Bits[15:15], RW, default = 1'b0 
       bit[15]: set to one to enable the power throttling for the channel.
       
     */
  } Bits;
  UINT16 Data;
} THRT_PWR_CHNL_MCDDC_CTL_STRUCT;




/* PM_CMD_PWR_0_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x40252160)                                                  */
/*       SKX (0x40252160)                                                     */
/* Register default value:              0x84000000                            */
#define PM_CMD_PWR_0_MCDDC_CTL_REG 0x0B004160
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x160
 */
typedef union {
  struct {
    UINT32 pwrcasr_dimm : 4;
    /* pwrcasr_dimm - Bits[3:0], RW, default = 4'b0000 
       Power contribution of CAS RD/RDS4 command in both OLTT and ET energy counters.
     */
    UINT32 pwrcasw_dimm : 4;
    /* pwrcasw_dimm - Bits[7:4], RW, default = 4'b0000 
       Power contribution of CAS WR/WRS4 command in both OLTT and ET energy counters.
     */
    UINT32 pwract_dimm : 6;
    /* pwract_dimm - Bits[13:8], RW, default = 6'b000000 
       Power contribution of ACT command in both OLTT and ET energy counters.
     */
    UINT32 pwrref_dimm : 8;
    /* pwrref_dimm - Bits[21:14], RW, default = 8'b00000000 
       Power contribution of 1x REF or SRE command. The 8b refresh weight defined here 
       is actually being multplied by 8 (shift left by 3 bits) before being accumulated 
       in the electrical throttling and OLTT counters. 
     */
    UINT32 active_idle_dimm : 5;
    /* active_idle_dimm - Bits[26:22], RW, default = 5'b10000 
       Defines number of DCLK of CKE-assertion to increase the OLTT and ET energy 
       counters (i.e. corresponding PMSUMPCCXRY, ET_DIMMSUM and ET_CH_SUM) by 4. 
       Hardware provides internal CKE counters (two per DIMM slot) to track each CKE. 
       When the internal count decrement to zero, the corresponding OLTT and ET energy 
       counters are increment by 4 and the internal CKE counter is loaded with the 
       content of this register field. 
       Valid Range of the register field : 1 - 31. Others: reserved.
       Due to the energy accumulator width limitation, we have imposed additional 
       programming limitation - this field must be programmed equal or greater than 4 
       DCLKs 
       Programming below 4 is not validated and may jeopardize missing thermal event or 
       proper electrical/power throttling during certain corner cases due to energy 
       accumulator over-flow. 
     */
    UINT32 pwrodt_cnt_dimm : 5;
    /* pwrodt_cnt_dimm - Bits[31:27], RW, default = 5'b10000 
       Defines number of DCLK of ODT-assertion to increase the OLTT and ET energy 
       counters (i.e. corresponding PMSUMPCCXRY, ET_DIMMSUM and ET_CH_SUM) by 16. 
       Hardware provides internal ODT counters (two per DIMM slot) to track each ODT. 
       When the internal count decrement to zero, the corresponding OLTT and ET energy 
       counters are incremented by 16 and the internal ODT counter is loaded with the 
       content of this register field. 
       Possible Valid Range of the register field : 1 - 31. Others: reserved.
       Due to the energy accumulator width limitation, we have imposed additional 
       programming limitation - this field must be programmed equal or greater than 4 
       DCLKs 
       Programming below 4 is not validated and may jeopardize missing thermal event or 
       proper electrical/power throttling during certain corner cases due to energy 
       accumulator over-flow. 
     */
  } Bits;
  UINT32 Data;
} PM_CMD_PWR_0_MCDDC_CTL_STRUCT;


/* PM_CMD_PWR_1_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x40252164)                                                  */
/*       SKX (0x40252164)                                                     */
/* Register default value:              0x84000000                            */
#define PM_CMD_PWR_1_MCDDC_CTL_REG 0x0B004164
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x164
 */
typedef union {
  struct {
    UINT32 pwrcasr_dimm : 4;
    /* pwrcasr_dimm - Bits[3:0], RW, default = 4'b0000 
       Power contribution of CAS RD/RDS4 command in both OLTT and ET energy counters.
     */
    UINT32 pwrcasw_dimm : 4;
    /* pwrcasw_dimm - Bits[7:4], RW, default = 4'b0000 
       Power contribution of CAS WR/WRS4 command in both OLTT and ET energy counters.
     */
    UINT32 pwract_dimm : 6;
    /* pwract_dimm - Bits[13:8], RW, default = 6'b000000 
       Power contribution of ACT command in both OLTT and ET energy counters.
     */
    UINT32 pwrref_dimm : 8;
    /* pwrref_dimm - Bits[21:14], RW, default = 8'b00000000 
       Power contribution of 1x REF or SRE command. The 8b refresh weight defined here 
       is actually being multplied by 8 (shift left by 3 bits) before being accumulated 
       in the electrical throttling and OLTT counters. 
     */
    UINT32 active_idle_dimm : 5;
    /* active_idle_dimm - Bits[26:22], RW, default = 5'b10000 
       Defines number of DCLK of CKE-assertion to increase the OLTT and ET energy 
       counters (i.e. corresponding PMSUMPCCXRY, ET_DIMMSUM and ET_CH_SUM) by 4. 
       Hardware provides internal CKE counters (two per DIMM slot) to track each CKE. 
       When the internal count decrement to zero, the corresponding OLTT and ET energy 
       counters are increment by 4 and the internal CKE counter is loaded with the 
       content of this register field. 
       Valid Range of the register field : 1 - 31. Others: reserved.
       Due to the energy accumulator width limitation, we have imposed additional 
       programming limitation - this field must be programmed equal or greater than 4 
       DCLKs 
       Programming below 4 is not validated and may jeopardize missing thermal event or 
       proper electrical/power throttling during certain corner cases due to energy 
       accumulator over-flow. 
     */
    UINT32 pwrodt_cnt_dimm : 5;
    /* pwrodt_cnt_dimm - Bits[31:27], RW, default = 5'b10000 
       Defines number of DCLK of ODT-assertion to increase the OLTT and ET energy 
       counters (i.e. corresponding PMSUMPCCXRY, ET_DIMMSUM and ET_CH_SUM) by 16. 
       Hardware provides internal ODT counters (two per DIMM slot) to track each ODT. 
       When the internal count decrement to zero, the corresponding OLTT and ET energy 
       counters are incremented by 16 and the internal ODT counter is loaded with the 
       content of this register field. 
       Possible Valid Range of the register field : 1 - 31. Others: reserved.
       Due to the energy accumulator width limitation, we have imposed additional 
       programming limitation - this field must be programmed equal or greater than 4 
       DCLKs 
       Programming below 4 is not validated and may jeopardize missing thermal event or 
       proper electrical/power throttling during certain corner cases due to energy 
       accumulator over-flow. 
     */
  } Bits;
  UINT32 Data;
} PM_CMD_PWR_1_MCDDC_CTL_STRUCT;


/* THRT_CHNL_MCDDC_CTL_REG supported on:                                      */
/*       SKX_A0 (0x4025216C)                                                  */
/*       SKX (0x4025216C)                                                     */
/* Register default value:              0x7FFFFFFF                            */
#define THRT_CHNL_MCDDC_CTL_REG 0x0B00416C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Max number of transactions (ACT, READ, WRITE) to be allowed during the 1 usec 
 * throttling timeframe per power throttling. 
 */
typedef union {
  struct {
    UINT32 pwr : 31;
    /* pwr - Bits[30:0], RW, default = 31'b1111111111111111111111111111111 
       Max amount of transaction energy to be allowed (per channel) during the 1 usec 
       throttling timeframe per power throttling. 
       PCODE can update this register dynamically. Note that the machine shifts this 
       field left by one so the number written to the register is multiplied by 2 
       before being used. 
     */
    UINT32 pwr_en : 1;
    /* pwr_en - Bits[31:31], RW, default = 1'b0 
       bit[31]: set to one to enable the power throttling for the channel.
       
     */
  } Bits;
  UINT32 Data;
} THRT_CHNL_MCDDC_CTL_STRUCT;


/* ET_DIMM_AVG_SUM_0_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252170)                                                  */
/*       SKX (0x40252170)                                                     */
/* Register default value:              0x00000000                            */
#define ET_DIMM_AVG_SUM_0_MCDDC_CTL_REG 0x0B004170
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x170
 */
typedef union {
  struct {
    UINT32 et_dimm_sum : 16;
    /* et_dimm_sum - Bits[15:0], RW_LBV, default = 16'b0000000000000000 
       DIMM energy current sum counter. When the ET_SAMPLE_PERIOD counter is counting 
       zero, the current sum (i.e. sum(i)) is used in the above Avg(i) calculation. The 
       ET_DIMM_SUM is reset in the next DCLK. This counter is sized to be sufficient 
       for all scenario and should not overlapped within valid ET_SAMPLE_PERIOD range. 
     */
    UINT32 et_dimm_avg : 16;
    /* et_dimm_avg - Bits[31:16], RW_LBV, default = 16'b0000000000000000 
       DIMM Average EnergyAvg(i)=Sum(i)/ET_DIV+Avg(i-1)-Avg(i-1)/ET_DIV 
     */
  } Bits;
  UINT32 Data;
} ET_DIMM_AVG_SUM_0_MCDDC_CTL_STRUCT;


/* ET_DIMM_AVG_SUM_1_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252174)                                                  */
/*       SKX (0x40252174)                                                     */
/* Register default value:              0x00000000                            */
#define ET_DIMM_AVG_SUM_1_MCDDC_CTL_REG 0x0B004174
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x174
 */
typedef union {
  struct {
    UINT32 et_dimm_sum : 16;
    /* et_dimm_sum - Bits[15:0], RW_LBV, default = 16'b0000000000000000 
       DIMM energy current sum counter. When the ET_SAMPLE_PERIOD counter is counting 
       zero, the current sum (i.e. sum(i)) is used in the above Avg(i) calculation. The 
       ET_DIMM_SUM is reset in the next DCLK. This counter is sized to be sufficient 
       for all scenario and should not overlapped within valid ET_SAMPLE_PERIOD range. 
     */
    UINT32 et_dimm_avg : 16;
    /* et_dimm_avg - Bits[31:16], RW_LBV, default = 16'b0000000000000000 
       DIMM Average EnergyAvg(i)=Sum(i)/ET_DIV+Avg(i-1)-Avg(i-1)/ET_DIV 
     */
  } Bits;
  UINT32 Data;
} ET_DIMM_AVG_SUM_1_MCDDC_CTL_STRUCT;


/* ET_DIMM_TH_0_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x40252180)                                                  */
/*       SKX (0x40252180)                                                     */
/* Register default value:              0xFFFFFFFF                            */
#define ET_DIMM_TH_0_MCDDC_CTL_REG 0x0B004180
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x180
 */
typedef union {
  struct {
    UINT32 et_dimm_lo_th : 16;
    /* et_dimm_lo_th - Bits[15:0], RW_LB, default = 16'b1111111111111111 
       Deassert electrical throttling when ET_DIMM_AVG is less than or equal to 
       ET_DIMM_LO_TH.Note: pcode may dynamically change and restore the programmed 
       threshold. Updating the threshold should take effect in the next DCLK. 
     */
    UINT32 et_dimm_hi_th : 16;
    /* et_dimm_hi_th - Bits[31:16], RW_LB, default = 16'b1111111111111111 
       Assert electrical throttling when ET_DIMM_AVG is greater than 
       ET_DIMM_HI_TH.Note: pcode may dynamically change and restore the programmed 
       threshold. Updating the threshold should take effect in the next DCLK. 
     */
  } Bits;
  UINT32 Data;
} ET_DIMM_TH_0_MCDDC_CTL_STRUCT;


/* ET_DIMM_TH_1_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x40252184)                                                  */
/*       SKX (0x40252184)                                                     */
/* Register default value:              0xFFFFFFFF                            */
#define ET_DIMM_TH_1_MCDDC_CTL_REG 0x0B004184
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x184
 */
typedef union {
  struct {
    UINT32 et_dimm_lo_th : 16;
    /* et_dimm_lo_th - Bits[15:0], RW_LB, default = 16'b1111111111111111 
       Deassert electrical throttling when ET_DIMM_AVG is less than or equal to 
       ET_DIMM_LO_TH.Note: pcode may dynamically change and restore the programmed 
       threshold. Updating the threshold should take effect in the next DCLK. 
     */
    UINT32 et_dimm_hi_th : 16;
    /* et_dimm_hi_th - Bits[31:16], RW_LB, default = 16'b1111111111111111 
       Assert electrical throttling when ET_DIMM_AVG is greater than 
       ET_DIMM_HI_TH.Note: pcode may dynamically change and restore the programmed 
       threshold. Updating the threshold should take effect in the next DCLK. 
     */
  } Bits;
  UINT32 Data;
} ET_DIMM_TH_1_MCDDC_CTL_STRUCT;


/* THRT_PWR_DIMM_0_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x20252190)                                                  */
/*       SKX (0x20252190)                                                     */
/* Register default value:              0x8FFF                                */
#define THRT_PWR_DIMM_0_MCDDC_CTL_REG 0x0B002190
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Max number of transactions (ACT, READ, WRITE) to be allowed during the 1 usec 
 * throttling timeframe per power throttling. 
 */
typedef union {
  struct {
    UINT16 thrt_pwr : 12;
    /* thrt_pwr - Bits[11:0], RW, default = 12'b111111111111 
       bit[11:0]: Max number of transactions (ACT, READ, WRITE) to be allowed (per 
       DIMM) during the 1 usec throttling timeframe per power throttling. 
       PCODE can update this register dynamically.
               
     */
    UINT16 rsvd : 3;
    /* rsvd - Bits[14:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT16 thrt_pwr_en : 1;
    /* thrt_pwr_en - Bits[15:15], RW, default = 1'b1 
       bit[15]: set to one to enable the power throttling for the DIMM.
       
     */
  } Bits;
  UINT16 Data;
} THRT_PWR_DIMM_0_MCDDC_CTL_STRUCT;


/* THRT_PWR_DIMM_1_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x20252192)                                                  */
/*       SKX (0x20252192)                                                     */
/* Register default value:              0x8FFF                                */
#define THRT_PWR_DIMM_1_MCDDC_CTL_REG 0x0B002192
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Max number of transactions (ACT, READ, WRITE) to be allowed during the 1 usec 
 * throttling timeframe per power throttling. 
 * 
 */
typedef union {
  struct {
    UINT16 thrt_pwr : 12;
    /* thrt_pwr - Bits[11:0], RW, default = 12'b111111111111 
       Max number of transactions (ACT, READ, WRITE) to be allowed (per DIMM) during 
       the 1 usec throttling timeframe per power throttling. 
       PCODE can update this register dynamically.  
     */
    UINT16 rsvd : 3;
    /* rsvd - Bits[14:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT16 thrt_pwr_en : 1;
    /* thrt_pwr_en - Bits[15:15], RW, default = 1'b1 
       bit[15]: set to one to enable the power throttling for the DIMM.
       
     */
  } Bits;
  UINT16 Data;
} THRT_PWR_DIMM_1_MCDDC_CTL_STRUCT;


/* THRT_COUNT_DIMM_0_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x20252198)                                                  */
/*       SKX (0x20252198)                                                     */
/* Register default value:              0x0000                                */
#define THRT_COUNT_DIMM_0_MCDDC_CTL_REG 0x0B002198
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Upper 16b of 23b per DIMM throttling demand counter, i.e. bit[22:7]: counting 
 * number of DCLK being throttled at this DIMM. This 23b counter is monotonic 
 * increment counter with auto wrap back to zero after overflow. PCODE calculate 
 * the delta change from the previous reading in order to determine how many cycles 
 * are being throttled regardless whether there is a transaction is pending or not. 
 *       
 */
typedef union {
  struct {
    UINT16 thrt_count : 16;
    /* thrt_count - Bits[15:0], RO_V, default = 16'b0000000000000000 
       Upper 16b of 23b per DIMM throttling demand counter, i.e. bit[22:7]: counting 
       number of DCLK being throttled at this DIMM. 
       This 23b counter is monotonic increment counter with auto wrap back to zero 
       after overflow. PCODE calculate the delta change 
       from the previous reading in order to determine how many cycles are being 
       throttled. 
               
     */
  } Bits;
  UINT16 Data;
} THRT_COUNT_DIMM_0_MCDDC_CTL_STRUCT;


/* THRT_COUNT_DIMM_1_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x2025219A)                                                  */
/*       SKX (0x2025219A)                                                     */
/* Register default value:              0x0000                                */
#define THRT_COUNT_DIMM_1_MCDDC_CTL_REG 0x0B00219A
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Upper 16b of 23b per DIMM throttling demand counter, i.e. bit[22:7]: counting 
 * number of DCLK being throttled at this DIMM. This 23b counter is monotonic 
 * increment counter with auto wrap back to zero after overflow. PCODE calculate 
 * the delta change from the previous reading in order to determine how many cycles 
 * are being throttled regardless whether there is a transaction is pending or not. 
 *       
 */
typedef union {
  struct {
    UINT16 thrt_count : 16;
    /* thrt_count - Bits[15:0], RO_V, default = 16'b0000000000000000 
       Upper 16b of 23b per DIMM throttling demand counter, i.e. bit[22:7]: counting 
       number of DCLK being throttled at this DIMM. This 23b counter is monotonic 
       increment counter with auto wrap back to zero after overflow. PCODE calculate 
       the delta change from the previous reading in order to determine how many cycles 
       are being throttled. 
               
     */
  } Bits;
  UINT16 Data;
} THRT_COUNT_DIMM_1_MCDDC_CTL_STRUCT;
























/* MC_TERM_RNK_MSK_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x402521D4)                                                  */
/*       SKX (0x402521D4)                                                     */
/* Register default value:              0x01FF0111                            */
#define MC_TERM_RNK_MSK_MCDDC_CTL_REG 0x0B0041D4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x1d4
 */
typedef union {
  struct {
    UINT32 term_rnk_msk : 10;
    /* term_rnk_msk - Bits[9:0], RW, default = 10'b0100010001 
       Physical Rank Mask to select which rank is used in the termination rank. BIOS 
       program the PHYSICAL rank select for the termination rank from each DIMM. It is 
       important to note that this is the PHYSICAL CS# mapping instead of the LOGICAL 
       rank mapping. 
       Here is the recommended programming method:
       Deciding and selecting the termination rank on each populated DIMM. For 
       simplicity, BIOS can always select rank 0 of each populated DIMM as the 
       termination rank unless rank 0 is marked as bad rank. 
       Note: BIOS may also optionally enable rank interleaving to separate the 
       termination ranks and non-termination ranks so OS can map more frequently used 
       address ranges into the RIR with termination ranks while mapping less frequently 
       used address ranges into RIR with non-termination ranks. This mapping enables a 
       better power optimization to exploit our PPD-S capability. 
       BIOS must also keep RD_ODT_TBL0-2 and WR_ODT_TBL0-2 consistent. Please refer to 
       those registers for further details. 
       This field CAN NOT be set as all zeros for populated channel. Min one 
       termination rank per DIMM. It has match rank occupancy for the ranks set as '1'. 
       
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[15:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ch_ppds_idle_timer : 16;
    /* ch_ppds_idle_timer - Bits[31:16], RW, default = 16'b0000000111111111 
       PPDS idle counter after all rank's rank idle counters (PDWN_IDEL_CNTR) have been 
       expired. 
     */
  } Bits;
  UINT32 Data;
} MC_TERM_RNK_MSK_MCDDC_CTL_STRUCT;


/* PM_ADR_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x402521E0)                                                  */
/*       SKX (0x402521E0)                                                     */
/* Register default value:              0x0000FF23                            */
#define PM_ADR_MCDDC_CTL_REG 0x0B0041E0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Controls aspects of the DDR ADR and DDR-T ADR flows
 */
typedef union {
  struct {
    UINT32 num_pwr_fail_cmds : 4;
    /* num_pwr_fail_cmds - Bits[3:0], RW, default = 4'b011 
        This should be set to 1 in order for MC to send correct number (3) of pwr fail 
       cmds after ADR. Configures the number of power fail commands are sent on DDR-T 
       to the FMC during a DDR-T ADR Event. 
     */
    UINT32 ddrt_adr_en : 1;
    /* ddrt_adr_en - Bits[4:4], RW_LB, default = 1'b0 
        When set, channel will enter DDR-T ADR flow when in 1LM Mode when ADR event 
       seen. Note: must be set when in 2LM Mode. Channel should always do DDR-T ADR in 
       2LM mode. 
     */
    UINT32 ddrt_early_warning_s_state : 3;
    /* ddrt_early_warning_s_state - Bits[7:5], RW, default = 3'b001 
        Specifies the S-state for DDR-T S-State command that should be sent as an early 
       warning to FMC. Default value is 001 = ADR.  
     */
    UINT32 drain_wpq_wait : 8;
    /* drain_wpq_wait - Bits[15:8], RW, default = 8'b11111111 
        Specifies how long to wait after wpq entries are stale and no wpq command has 
       won arbitration before considering the wpq drained. Used for both DDR ADR and 
       DDR-T ADR.  
     */
    UINT32 t_between_power_fail_cmds : 4;
    /* t_between_power_fail_cmds - Bits[19:16], RW, default = 4'b0000 
        Specifies the minimum amount of time to wait between scheduling power fail 
       commands on the same rank. A value of 0 disables this feature.  
     */
    UINT32 ddrt_adr_sref_timer : 5;
    /* ddrt_adr_sref_timer - Bits[24:20], RW, default = 5'b0000 
        This field defines number of DCLK of wait time after PCU asserts AsyncSR before 
       telling the refresh FSMs to start a DDR-T ADR flow.  
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PM_ADR_MCDDC_CTL_STRUCT;










/* ET_CH_AVG_MCDDC_CTL_REG supported on:                                      */
/*       SKX_A0 (0x402521F4)                                                  */
/*       SKX (0x402521F4)                                                     */
/* Register default value:              0x00000000                            */
#define ET_CH_AVG_MCDDC_CTL_REG 0x0B0041F4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x1f4
 */
typedef union {
  struct {
    UINT32 et_ch_avg : 18;
    /* et_ch_avg - Bits[17:0], RW_LBV, default = 18'b000000000000000000 
       Channel Average EnergyAvg(i)=Sum(i)/ET_DIV+Avg(i-1)-Avg(i-1)/ET_DIV 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ET_CH_AVG_MCDDC_CTL_STRUCT;


/* ET_CH_SUM_MCDDC_CTL_REG supported on:                                      */
/*       SKX_A0 (0x402521F8)                                                  */
/*       SKX (0x402521F8)                                                     */
/* Register default value:              0x00000000                            */
#define ET_CH_SUM_MCDDC_CTL_REG 0x0B0041F8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x1f8
 */
typedef union {
  struct {
    UINT32 et_ch_sum : 18;
    /* et_ch_sum - Bits[17:0], RW_LBV, default = 18'b000000000000000000 
       Channel energy current sum counter. When the ET_SAMPLE_PERIOD counter is 
       counting zero, the current sum (i.e. sum(i)) is used in the above Avg(i) 
       calculation. The ET_CH_SUM is reset in the next DCLK. The ET_CH_SUM is sized to 
       be sufficient for worst case scenario to avoid overflowing within valid 
       ET_SAMPLE_PERIOD range. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ET_CH_SUM_MCDDC_CTL_STRUCT;


/* ET_CH_TH_MCDDC_CTL_REG supported on:                                       */
/*       SKX_A0 (0x402521FC)                                                  */
/*       SKX (0x402521FC)                                                     */
/* Register default value:              0xFFFFFFFF                            */
#define ET_CH_TH_MCDDC_CTL_REG 0x0B0041FC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x1fc
 */
typedef union {
  struct {
    UINT32 et_ch_lo_th : 16;
    /* et_ch_lo_th - Bits[15:0], RW_LB, default = 16'b1111111111111111 
       The 16b ET_CH_LO_TH field is actually the high order 16b of the 18b threshold 
       value, i.e. ET_CH_LO_TH[17:2]. 00b bits are the two least significant bits of 
       the 18b threshold. 
       Channel energy low threshold.de-assert electrical throttling when 
       ET_CH_AVG[17:0] is less than or equal to ET_CH_LO_TH[17:0]. 
       Note: firmware may dynamically change and restore the programmed threshold. 
       Updating the threshold should take effect in the next DCLK. 
     */
    UINT32 et_ch_hi_th : 16;
    /* et_ch_hi_th - Bits[31:16], RW_LB, default = 16'b1111111111111111 
       The 16b ET_CH_HI_TH field is actually the high order 16b of the 18b threshold 
       value, i.e. ET_CH_HI_TH[17:2]. 00b bits are the two least significant bits of 
       the 18b threshold. 
       Channel energy high threshold.Assert electrical throttling when ET_CH_AVG[17:0] 
       is greater than ET_CH_HI_TH[17:0]. 
       Note: firmware may dynamically change and restore the programmed threshold. 
       Updating the threshold should take effect in the next DCLK. 
     */
  } Bits;
  UINT32 Data;
} ET_CH_TH_MCDDC_CTL_STRUCT;


/* TCDBP_MCDDC_CTL_REG supported on:                                          */
/*       SKX_A0 (0x40252200)                                                  */
/*       SKX (0x40252200)                                                     */
/* Register default value:              0x50E1D40A                            */
#define TCDBP_MCDDC_CTL_REG 0x0B004200
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x200
 */
typedef union {
  struct {
    UINT32 t_rcd : 5;
    /* t_rcd - Bits[4:0], RW, default = 5'b01010 
       tRCD. ACT to internal read or write delay time in DCLK (must be at least 5)
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[8:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_cl : 5;
    /* t_cl - Bits[13:9], RW, default = 5'b01010 
       CAS Read Latency (tCL) (must be at least 9) Note: RL=tAL+tCL, but AL is not 
       supported. 
     */
    UINT32 t_cwl : 5;
    /* t_cwl - Bits[18:14], RW, default = 5'b00111 
       CAS Write Latency (tCWL)  (must be at least 9) Note: WL=tAL+tCWL, but AL is not 
       supported  
     */
    UINT32 t_ras : 6;
    /* t_ras - Bits[24:19], RW, default = 6'b011100 
       tRAS.  ACT to PRE command period 
     */
    UINT32 cmd_oe_on : 1;
    /* cmd_oe_on - Bits[25:25], RW, default = 1'b0  */
    UINT32 cmd_oe_cs : 1;
    /* cmd_oe_cs - Bits[26:26], RW, default = 1'b0  */
    UINT32 t_rp : 5;
    /* t_rp - Bits[31:27], RW, default = 5'b01010 
       tRP.  PRE command period. 
     */
  } Bits;
  UINT32 Data;
} TCDBP_MCDDC_CTL_STRUCT;


/* TCRAP_MCDDC_CTL_REG supported on:                                          */
/*       SKX_A0 (0x40252204)                                                  */
/*       SKX (0x40252204)                                                     */
/* Register default value:              0x0C6063A5                            */
#define TCRAP_MCDDC_CTL_REG 0x0B004204
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x204
 */
typedef union {
  struct {
    UINT32 t_rrd : 3;
    /* t_rrd - Bits[2:0], RW, default = 3'b101 
       tRRD_S.  ACTIVE to ACTIVE command period.
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_rtp : 4;
    /* t_rtp - Bits[7:4], RW, default = 4'b1010 
       tRTP.  Internal READ Command to PRECHARGE Command delay. 
     */
    UINT32 t_cke : 4;
    /* t_cke - Bits[11:8], RW, default = 4'b0011 
       tCKE.  CKE minimum pulse width. 
     */
    UINT32 t_wtr : 4;
    /* t_wtr - Bits[15:12], RW, default = 4'b0110 
       tWTR_S.  DCLK delay from start of internal write transaction to internal read 
       command. 
       iMC's Write to Read Same Rank (T_WRSR) is automatically calulated based from 
       TCDBP.T_CWL + 4 + T_WTR. 
       For LRDIMM running in rank multiplication mode, IMC will continue to use the 
       above equation for T_WRSR even if the WRITE and READ are targeting same logical 
       rank but at different physical ranks behind the LRDIMM buffer, In the other 
       word, IMC will not be able to dynamically switch to T_WRDR timing. In order to 
       avoid timing violation in this scenario, BIOS must configure the T_WTR parameter 
       to be the MAX(T_WTR of LRDIMM, (T_WRDR' - T_CL + 2)). Note: Due to the lighter 
       electrical loading behind the LRDIMM buffer, further optimization can be tuned 
       during post-silicon to reduce the T_WRDR' paraemeter instead of directly using 
       the TCRWP.T_WRDR parameter. 
     */
    UINT32 t_faw : 6;
    /* t_faw - Bits[21:16], RW, default = 6'b100000 
       tFAW.  Four activate window (must be at least 4*tRRD and at most 63)
     */
    UINT32 t_prpden : 2;
    /* t_prpden - Bits[23:22], RW, default = 2'b01 
       tPRPDEN, tACTPDEN, tREFPDEN needs to have value of 2 for 2133+. All 3 values 
       will use this single value. 
     */
    UINT32 t_wr : 5;
    /* t_wr - Bits[28:24], RW, default = 5'b01100 
       tWR.  Bits 4:0 of WRITE recovery time (must be at least 15ns equivalent). Bit 
       6:5 is in tcothp2.t_wr_upper. 
     */
    UINT32 rsvd_29 : 1;
    UINT32 cmd_stretch : 2;
    /* cmd_stretch - Bits[31:30], RW, default = 2'b00 
       defines for how many cycles the command is stretched
       00: 1N operation
       01: Reserved
       10: 2N operation
       11: 3N operation
     */
  } Bits;
  UINT32 Data;
} TCRAP_MCDDC_CTL_STRUCT;


/* TCRWP_MCDDC_CTL_REG supported on:                                          */
/*       SKX_A0 (0x40252208)                                                  */
/*       SKX (0x40252208)                                                     */
/* Register default value:              0x02492492                            */
#define TCRWP_MCDDC_CTL_REG 0x0B004208
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x208
 */
typedef union {
  struct {
    UINT32 t_rrdr : 3;
    /* t_rrdr - Bits[2:0], RW, default = 3'b010 
       Back to back READ to READ from different RANK separation parameter. The actual 
       READ to READ command separation is T_RRDR + 5 DCLKs measured between the clock 
       assertion edges of the two corresponding asserted command CS#. Please note that 
       the minimum setting of the field must meet the DDRIO requirement for READ to 
       READ turnaround time to be at least 5 DClk at the DDRIO pin. 
       The maximum design range from the above calculation is 31.
     */
    UINT32 t_rrdd : 3;
    /* t_rrdd - Bits[5:3], RW, default = 3'b010 
       Back to back READ to READ from different DIMM separation parameter. The actual 
       READ to READ command separation is T_RRDD + 5 DCLKs measured between the clock 
       assertion edges of the two corresponding asserted command CS#. Please note that 
       the minimum setting of the field must meet the DDRIO requirement for READ to 
       READ turnaround time to be at least 5 DClk at the DDRIO pin. 
       The maximum design range from the above calculation is 31.
     */
    UINT32 t_wwdr : 3;
    /* t_wwdr - Bits[8:6], RW, default = 3'b010 
       Back to back WRITE to WRITE from different RANK separation parameter. The actual 
       WRITE to WRITE command separation is 
       T_WWDR + 5 DCLKs measured between the clock assertion edges of the two 
       corresponding asserted command CS#. Please note that the minimum setting of the 
       field must meet the DDRIO requirement for WRITE to WRITE turnaround time to be 
       at least 6 DClk at the DDRIO pin. 
       The maximum design range from the above calculation is 15.
     */
    UINT32 t_wwdd : 3;
    /* t_wwdd - Bits[11:9], RW, default = 3'b010 
       Back to back WRITE to WRITE from different DIMM separation parameter. The actual 
       WRITE to WRITE command separation is 
       T_WWDD + 5 DCLKs measured between the clock assertion edges of the two 
       corresponding asserted command CS#. Please note that the minimum setting of the 
       field must meet the DDRIO requirement for WRITE to WRITE turnaround time to be 
       at least 6 DClk at the DDRIO pin. 
       The maximum design range from the above calculation is 15.
     */
    UINT32 rsvd_12 : 6;
    UINT32 t_wrdr : 3;
    /* t_wrdr - Bits[20:18], RW, default = 3'b010 
       Back to back WRITE to READ from different RANK separation parameter.The actual 
       WRITE to READ command separation is 
       TCDBP.T_CWL - TCDBP.T_CL + T_WRDR + 6 DCLKs measured between the clock assertion 
       edges of the two corresponding asserted command CS#. 
     */
    UINT32 t_wrdd : 3;
    /* t_wrdd - Bits[23:21], RW, default = 3'b010 
       Back to back WRITE to READ from different DIMM separation parameter.The actual 
       WRITE to READ command separation is 
       TCDBP.T_CWL - TCDBP.T_CL + T_WRDD + 6 DCLKs measured between the clock assertion 
       edges of the two corresponding asserted command CS#. 
     */
    UINT32 t_rwsr : 3;
    /* t_rwsr - Bits[26:24], RW, default = 3'b010 
       This field is not used. Please refer to TCOTHP2 for the new register field 
       location. 
     */
    UINT32 t_ccd : 3;
    /* t_ccd - Bits[29:27], RW, default = 3'b000 
       back to back READ to READ CAS to CAS from same rank separation parameter.The 
       actual JEDEC CAS to CAS command separation is (T_CCD + 4) DCLKs measured between 
       the clock assertion edges of the two corresponding asserted command CS#. 
     */
    UINT32 t_wrdr_upper : 2;
    /* t_wrdr_upper - Bits[31:30], RW, default = 2'b00 
       Upper 2 bits (bits 4:3) of t_wrdr field.
     */
  } Bits;
  UINT32 Data;
} TCRWP_MCDDC_CTL_STRUCT;


/* TCOTHP_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x4025220C)                                                  */
/*       SKX (0x4025220C)                                                     */
/* Register default value:              0x66002003                            */
#define TCOTHP_MCDDC_CTL_REG 0x0B00420C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x20c
 */
typedef union {
  struct {
    UINT32 t_xp : 6;
    /* t_xp - Bits[5:0], RW, default = 3'b011 
       Exit Power Down with DLL on to any valid command; Exit Precharge Power Down with 
       DLL frozen to commands not requiring a locked DLL. 
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_cwl_adj : 3;
    /* t_cwl_adj - Bits[10:8], RW, default = 3'b000 
       This register defines additional WR data delay per channel in order to overcome 
       the WR-flyby issue. 
       The total CAS write latency that the DDR sees is the sum of T_CWL and the 
       T_CWL_ADJ minus T_CWL_ADJ_NEG. 
       000 - no added latency (default)
       001 to 111 - 1 to 7 Dclk of added latency
     */
    UINT32 shift_odt_early : 1;
    /* shift_odt_early - Bits[11:11], RW, default = 1'b0 
       This shifts the ODT waveform one cycle early relative to the timing set up in 
       the ODT_TBL2 register, when in 2N or 3N mode. 
       This bit has no effect in 1N mode.
     */
    UINT32 t_rwdr : 4;
    /* t_rwdr - Bits[15:12], RW, default = 4'b0010  */
    UINT32 t_rwdd : 4;
    /* t_rwdd - Bits[19:16], RW, default = 4'b0000 
       Do not use.
     */
    UINT32 t_rwsr : 4;
    /* t_rwsr - Bits[23:20], RW, default = 4'b0000 
       Do not use.
     */
    UINT32 t_odt_oe : 4;
    /* t_odt_oe - Bits[27:24], RW, default = 4'b0110 
       When t_odt_oe=0, ODT will not tri-state
       Otherwise, this field defines delay in Dclks to disable ODT output after all CKE 
       pins are low and either in self-refresh or in IBTOff mode 
     */
    UINT32 t_cs_oe : 4;
    /* t_cs_oe - Bits[31:28], RW, default = 4'b0110 
       When t_cs_oe=0, CS[9:0]# will not tri-state
       Otherwise, this field defines delay in Dclks to disable CS output after all CKE 
       pins are low 
     */
  } Bits;
  UINT32 Data;
} TCOTHP_MCDDC_CTL_STRUCT;


/* TCRFP_MCDDC_CTL_REG supported on:                                          */
/*       SKX_A0 (0x40252210)                                                  */
/*       SKX (0x40252210)                                                     */
/* Register default value:              0x4004883F                            */
#define TCRFP_MCDDC_CTL_REG 0x0B004210
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x210
 */
typedef union {
  struct {
    UINT32 orefni : 8;
    /* orefni - Bits[7:0], RW, default = 8'b00111111 
       Rank idle period that defines an opportunity for refresh, in DCLK cycles
     */
    UINT32 ref_hi_wm : 7;
    /* ref_hi_wm - Bits[14:8], RW, default = 7'b0001000 
       tREFI count level that turns the refresh priority to high (default is 8)
     */
    UINT32 ref_panic_wm : 7;
    /* ref_panic_wm - Bits[21:15], RW, default = 7'b0001001 
       tREFI count level in which the refresh priority is panic (default is 9)
       It is recommended to set the panic WM at least to 9, in order to utilize the 
       maximum no-refresh period possible 
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[29:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_30 : 1;
    UINT32 dis_reload_hi_wm_err_recov : 1;
    /* dis_reload_hi_wm_err_recov - Bits[31:31], RW, default = 1'b0 
       When set to 1, this field will disable the reload of the high water mark value 
       into the remaining refresh counter during native C/A parity recovery. 
     */
  } Bits;
  UINT32 Data;
} TCRFP_MCDDC_CTL_STRUCT;


/* TCRFTP_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x40252214)                                                  */
/*       SKX (0x40252214)                                                     */
/* Register default value:              0x1240062C                            */
#define TCRFTP_MCDDC_CTL_REG 0x0B004214
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x214
 */
typedef union {
  struct {
    UINT32 t_refi : 15;
    /* t_refi - Bits[14:0], RW, default = 15'b000011000101100 
       Defines the average period between refreshes in DCLK cycles. This register 
       defines the upper 15b of the 16b tREFI counter limit. The least significant bit 
       of the counter limit is always zero. 
       Here are the recommended T_REFI[14:0] setting for 7.8 usec:
       0800MT/s : 0C30h
       1067MT/s : 1040h
       1333MT/s : 1450h
       1600MT/s : 1860h
       1867MT/s : 1C70h
     */
    UINT32 t_rfc : 10;
    /* t_rfc - Bits[24:15], RW, default = 10'b0010000000 
       Time of refresh - from beginning of refresh until next ACT or refresh is allowed 
       (in DCLK cycles) 
       Here are the recommended T_RFC for 2Gb DDR3:
       0800MT/s : 040h
       1067MT/s : 056h
       1333MT/s : 06Bh
       1600MT/s : 080h
       1867MT/s : 096h
     */
    UINT32 t_refix9 : 7;
    /* t_refix9 - Bits[31:25], RW, default = 7'b0001001 
       period of min between 9*T_REFI and tRAS maximum (normally 70 us) in 1024 * DCLK 
       cycles.The default value will need to reduce 100 DCLK cycles - uncertainty on 
       timing of panic refresh 
     */
  } Bits;
  UINT32 Data;
} TCRFTP_MCDDC_CTL_STRUCT;


/* TCSRFTP_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x40252218)                                                  */
/*       SKX (0x40252218)                                                     */
/* Register default value:              0x6100B100                            */
#define TCSRFTP_MCDDC_CTL_REG 0x0B004218
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x218
 */
typedef union {
  struct {
    UINT32 t_xsdll : 12;
    /* t_xsdll - Bits[11:0], RW, default = 12'b000100000000 
       Exit Self Refresh to commands requiring a locked DLL in the range of 128 to 4095 
       DCLK cycles 
     */
    UINT32 t_xsoffset : 4;
    /* t_xsoffset - Bits[15:12], RW, default = 4'b1011 
       tXS = T_RFC+10ns. Setup of T_XSOFFSET is # of cycles for 10 ns. Range is between 
       3 and 11 DCLK cycles 
     */
    UINT32 t_zqoper : 10;
    /* t_zqoper - Bits[25:16], RW, default = 10'b0100000000 
       Normal operation Full calibration time
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[26:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_mod : 5;
    /* t_mod - Bits[31:27], RW, default = 5'b01100 
       Mode Register Set command update delay.
     */
  } Bits;
  UINT32 Data;
} TCSRFTP_MCDDC_CTL_STRUCT;


/* TCMR2SHADOW_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x4025221C)                                                  */
/*       SKX (0x4025221C)                                                     */
/* Register default value:              0x00020018                            */
#define TCMR2SHADOW_MCDDC_CTL_REG 0x0B00421C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x21c
 */
typedef union {
  struct {
    UINT32 mr2_shdw_a5to0 : 6;
    /* mr2_shdw_a5to0 - Bits[5:0], RW, default = 6'b011000 
       Copy of MR2 A[5:0] shadow
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mr2_shdw_a6_asr : 3;
    /* mr2_shdw_a6_asr - Bits[10:8], RW, default = 3'b000 
       Copy of MR2 A[6] shadow which defines per DIMM availability of ASR mode - set if 
       Auto Self-Refresh (ASR) is supported, otherwise cleared 
       Bit 10: Dimm 2
       Bit 9: Dimm 1
       Bit 8: Dimm 0
     */
    UINT32 rsvd_11 : 1;
    /* rsvd_11 - Bits[11:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mr2_shdw_a7_srt : 3;
    /* mr2_shdw_a7_srt - Bits[14:12], RW, default = 3'b000 
       Copy of MR2 A[7] shadow which defines per DIMM availability of SRT mode - set if 
       extended temperature range and ASR is not supported, otherwise cleared 
       Bit 14: Dimm 2
       Bit 13: Dimm 1
       Bit 12: Dimm 0
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mr2_shdw_a15to8 : 8;
    /* mr2_shdw_a15to8 - Bits[23:16], RW, default = 8'b00000010 
       Copy of MR2 A[15:8] shadow.
       Bit 23-19: zero, copy of MR2 A[15:11], reserved for future JEDEC use
       Bit 18-17: Rtt_WR, i.e. copy of MR2 A[10:9]
       Bit 16: zero, copy of MR2 A[8], reserved for future JEDEC use
     */
    UINT32 addr_bit_swizzle : 3;
    /* addr_bit_swizzle - Bits[26:24], RW_LV, default = 3'b000 
       Each bit is set in case of the corresponding 2-rank UDIMM or certain LRDIMM 
       requires address mirroring/swizzling. It indicates that some of the address bits 
       are swizzled for rank 1 (or rank 3), and this has to be considerred in MRS 
       command. The address swizzling bits: 
       A3 and A4
       A5 and A6
       A7 and A8
       A11 and A13 (ddr4 only)
       BA0 and BA1
       BG0 and BG1 (ddr4 only)
       Bit 24 refers to DIMM 0
       Bit 25 refers to DIMM 1
       Bit 26 refers to DIMM 2
     */
    UINT32 rsvd_27 : 4;
    /* rsvd_27 - Bits[30:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 two_mrs_en : 1;
    /* two_mrs_en - Bits[31:31], RW, default = 1'b0 
       THis bit is added for ddr4 rdimm,lrdimm,3ds support. Unlike ddr3 and udimm ddr4; 
       these devices use Address inversion which requires host 
       to issue 2 MRS to support these devices. THis bit only affects MR0/MR2/MR5 
       issued by RTL as a partof normal operation. Those MRS issued by 
       CPGC engine are not affected by this bit.
       If this bit =0, only one MRS will be sent to the target chip_select. If 1, two 
       back to back MRS will be sent to the traget chip_select once 
       for A-side and once for B-side.
     */
  } Bits;
  UINT32 Data;
} TCMR2SHADOW_MCDDC_CTL_STRUCT;


/* TCZQCAL_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x40252220)                                                  */
/*       SKX (0x40252220)                                                     */
/* Register default value:              0x00004080                            */
#define TCZQCAL_MCDDC_CTL_REG 0x0B004220
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x220
 */
typedef union {
  struct {
    UINT32 zqcsperiod : 8;
    /* zqcsperiod - Bits[7:0], RW, default = 8'b10000000 
       Time between ZQ-FSM initiated ZQCS operations in tREFI*128 (2 to 255, default is 
       128). 
       Note: ZQCx is issued at SRX.
     */
    UINT32 t_zqcs : 8;
    /* t_zqcs - Bits[15:8], RW, default = 8'b01000000 
       tZQCS in DCLK cycles (32 to 255, default is 64)
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCZQCAL_MCDDC_CTL_STRUCT;


/* TCSTAGGER_REF_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x40252224)                                                  */
/*       SKX (0x40252224)                                                     */
/* Register default value:              0x00020080                            */
#define TCSTAGGER_REF_MCDDC_CTL_REG 0x0B004224
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * tRFC like timing constraint parameter except it is a timing constraint 
 * applicable to REF-REF separation between different ranks on a channel. 
 */
typedef union {
  struct {
    UINT32 t_stagger_ref : 10;
    /* t_stagger_ref - Bits[9:0], RW, default = 10'b0010000000 
       tRFC like timing constraint parameter except it is a timing constraint 
       applicable to REF-REF separation between different ranks on a channel. 
       It is recommended to set T_STAGGER_REF equal or less than the TRFC parameter 
       which is defined as: 
       0800MT/s : 040h
       1067MT/s : 056h
       1333MT/s : 06Bh
       1600MT/s : 080h
       1867MT/s : 096h
       For 3DS DIMMs, it should be programmed with mimimum 3DCLKs
     */
    UINT32 t_shrtloop_stagger_ref : 10;
    /* t_shrtloop_stagger_ref - Bits[19:10], RW, default = 10'b0010000000 
       3DS specific field.tRFC like timing constraint parameter except it is a timing 
       constraint applicable to REF-REF separation between different sub-ranks/slices 
       within a stack of a 3DS DIMM. Typical value is tRFC/3 on a channel. 
       
     */
    UINT32 t_shrtloop_num : 3;
    /* t_shrtloop_num - Bits[22:20], RW, default = 3'b000 
       3DS specific field. Number of short loop with the time gap of 
       t_shrtloop_stagger_ref in a single full loop of refresh FSM. 
     */
    UINT32 maxnum_active_reffsm : 3;
    /* maxnum_active_reffsm - Bits[25:23], RW, default = 3'b000  */
    UINT32 rsvd : 6;
    /* rsvd - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCSTAGGER_REF_MCDDC_CTL_STRUCT;


/* TCMR4SHADOW_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252228)                                                  */
/*       SKX (0x40252228)                                                     */
/* Register default value:              0x00000000                            */
#define TCMR4SHADOW_MCDDC_CTL_REG 0x0B004228
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * MR4 Shadow Register
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 12;
    UINT32 write_preamble : 1;
    /* write_preamble - Bits[12:12], RW, default = 1'b0 
       If 0, write preamble=1tck. If 1, write_preamble=2tck. This bit field has to be 
       set consistantly in MC, ddrio and DDR devices. 
       When set to 1, MC modifies it behavior to send dqsoutenable to ddrio 1 clock 
       earlier as well as 
       increasing RD->WR timing by 1 clock in order to meet ddr4 requirements.
       When set to 1, MC modifies its behavior to block a command spacing of 5 clocks 
       between all combinations of CAS and/or GNT commands in addition 
       to meeting all other timing requirements.
     */
    UINT32 rsvd : 19;
    /* rsvd - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCMR4SHADOW_MCDDC_CTL_STRUCT;


/* TCMR0SHADOW_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x4025222C)                                                  */
/*       SKX (0x4025222C)                                                     */
/* Register default value:              0x00000000                            */
#define TCMR0SHADOW_MCDDC_CTL_REG 0x0B00422C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * MR0 Shadow Register
 */
typedef union {
  struct {
    UINT32 mr0_shadow : 13;
    /* mr0_shadow - Bits[12:0], RW, default = 13'b0000000000000 
       BIOS programs this field for MR0 register A12:A0 for all DIMMs in this channel. 
       IMC hardware is dynamically issuing MRS to MR0 to control the fast and slow exit 
       PPD (MRS MR0 A12). Other address bits (A[11:0]) are defined by this register 
       field. A15:A13 are always zero. 
     */
    UINT32 rsvd : 19;
    /* rsvd - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCMR0SHADOW_MCDDC_CTL_STRUCT;


/* TCMR5SHADOW_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252230)                                                  */
/*       SKX (0x40252230)                                                     */
/* Register default value:              0x00000000                            */
#define TCMR5SHADOW_MCDDC_CTL_REG 0x0B004230
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * MR5 Shadow Register
 */
typedef union {
  struct {
    UINT32 mr5_shadow : 13;
    /* mr5_shadow - Bits[12:0], RW, default = 13'b0000000000000 
       This CR holds a copy of MR5 register value programmed during MRC. This gets 
       written by BIOS. This is is to support the C/A parity error flag clearing in 
       DRAMs. In DDR4 mode, C/A Errors involves a error flow and end of that flow 
       requires MR5 to be updated with 0 in A[4]. A[12:5} and A[3:0] should remain what 
       it was during MRC. 
     */
    UINT32 rsvd : 19;
    /* rsvd - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCMR5SHADOW_MCDDC_CTL_STRUCT;


/* TCMR3SHADOW_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252234)                                                  */
/*       SKX (0x40252234)                                                     */
/* Register default value:              0x00000000                            */
#define TCMR3SHADOW_MCDDC_CTL_REG 0x0B004234
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * MR5 Shadow Register
 */
typedef union {
  struct {
    UINT32 mr3_shadow : 13;
    /* mr3_shadow - Bits[12:0], RW, default = 13'b0000000000000 
       This CR holds a copy of MR3 register (or F0BC1X register for ddr4 lrdimm 
       devices). This register is not 
       used by hardware directly. The intent is that MRC writes the value of MR3 or 
       F0B1CX into this register 
       such that EV params utilities can read this and modify only PDA bit (or PBA for 
       lrdimms) and write it 
       into CADB as part of Vref margining algorithms used while os is running. DDR4 
       and DDR4 buffer specs 
       should be referenced to get the latest bit position for PDA/PBA.
     */
    UINT32 rsvd : 19;
    /* rsvd - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCMR3SHADOW_MCDDC_CTL_STRUCT;


/* IDLETIME_MCDDC_CTL_REG supported on:                                       */
/*       SKX_A0 (0x40252238)                                                  */
/*       SKX (0x40252238)                                                     */
/* Register default value:              0x10C19008                            */
#define IDLETIME_MCDDC_CTL_REG 0x0B004238
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * At a high level, the goal of any page closing policy is to trade off some 
 * Premature Page Closes (PPCs) in order to avoid more Overdue Page Closes (OPCs).  
 * In other words, we want to avoid costly Page Misses and turn them into Page 
 * Empties at the expense of occasionally missing a Page Hit and instead getting a 
 * Page Empty.  The scheme achieves this by tracking the number of PPCs and OPCs 
 * over a certain configurable window (of requests).  It then compares the two 
 * values to configurable thresholds, and adjusts the amount of time before closing 
 * pages accordingly. 
 */
typedef union {
  struct {
    UINT32 idle_page_rst_val : 6;
    /* idle_page_rst_val - Bits[5:0], RW, default = 6'b001000 
       Idle Counter Reset Value (RV): This is the value that effectively adapts. It 
       determines what value the various ICs are set to whenever they are reset. It 
       therefore controls the number of cycles before an automatic page close is 
       triggered for an entire channel. 
                   
                   OPCs drive the RV down to 6'b000000, whereas PPCs drive the RV up to 
       6'b111111.  The RV value is sticky at these two lower and upper levels and does 
       not rollover.  Idle counter in page tables are loaded with ?[(idle_page_rst_val 
       * 4) + 3]? 
               
     */
    UINT32 win_size : 8;
    /* win_size - Bits[13:6], RW, default = 8'b01000000 
       Window Size (WS): The number of requests that we track before making a decision 
       to adapt the RV. 
                   The actual WS count (i.e. the number of requests in the window) is 
       equal to [(win_size * 16) + 15] 
               
     */
    UINT32 ppc_th : 7;
    /* ppc_th - Bits[20:14], RW, default = 7'b0000110 
       Premature Page Close (PPC) Threshold
                   If the number of PPCs in a given window is larger than this 
       threshold, we increase the RV 
                   The actual PPC threshold count (i.e. the number of PPC requests) is 
       equal to [(ppc_th * 16) + 15] 
               
     */
    UINT32 opc_th : 7;
    /* opc_th - Bits[27:21], RW, default = 7'b0000110 
       Overdue Page Close (OPC) Threshold
                   If the number of OPCs in a given window is larger than this 
       threshold, we decrease the RV. 
                   The actual OPC threshold count (i.e. the number of OPC requests) is 
       equal to [(opc_th * 16) + 15] 
               
     */
    UINT32 adapt_pg_clse : 1;
    /* adapt_pg_clse - Bits[28:28], RW, default = 1'b1 
       This register is programmed in conjunction with MCMTR.CLOSE_PG to enable three 
       different modes: 
       - Closed Page Mode =Mode -1 MCMTR.CLOSE_PG = 1 and ADAPT_PG_CLSE= 0. 
       - Adaptive Open =Open -1 MCMTR.CLOSE_PG = 0 and ADAPT_PG_CLSE= 1.
       - All others reserved.
       When ADAPT_PG_CLSE=0, the page close idle timer gets set with IDLE_PAGE_RST_VAL 
       times 4. 
     */
    UINT32 rsvd : 3;
    /* rsvd - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IDLETIME_MCDDC_CTL_STRUCT;


/* RDIMMTIMINGCNTL_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x4025223C)                                                  */
/*       SKX (0x4025223C)                                                     */
/* Register default value:              0x012C0008                            */
#define RDIMMTIMINGCNTL_MCDDC_CTL_REG 0x0B00423C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x23c
 */
typedef union {
  struct {
    UINT32 t_mrd : 4;
    /* t_mrd - Bits[3:0], RW, default = 4'b1000 
       Command word to command word programming delay in DCLK
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[11:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_stab : 20;
    /* t_stab - Bits[31:12], RW, default = 20'h012C0 
       Stablizing time in number of DCLK, i.e. the DCLK must be stable for T_STAB 
       before any access to the device take place. 
       tCKSRX is included in the T_STAB programming since there is not a separate 
       tCKSRX parameter control to delay self-refresh exit latency from clock stopped 
       conditions. 
       zero value in T_STAB is reserved and it is important to AVOID programming a zero 
       value in the T_STAB. 
       Recommended settings (Note: contains stretch goal and/or over-clock frequency 
       examples): 
       FREQ T_STAB for RDIMM (including tCKSRX value)
       0800 0960h+5h=0965h
       1067 0C80h+5h=0c85h
       1333 0FA0h+7h=0FA7h
       1600 12C0h+8h=12C8h
       1867 15E0h+Ah=15EAh
       2133 1900h+Bh=190Bh
       FREQ T_STAB for UDIMM (i.e. tCKSRX value)
       0800 5h
       1067 5h
       1333 7h
       1600 8h
       1867 Ah
       2133 Bh
     */
  } Bits;
  UINT32 Data;
} RDIMMTIMINGCNTL_MCDDC_CTL_STRUCT;


/* RDIMMTIMINGCNTL2_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x40252240)                                                  */
/*       SKX (0x40252240)                                                     */
/* Register default value:              0x00000504                            */
#define RDIMMTIMINGCNTL2_MCDDC_CTL_REG 0x0B004240
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x240
 */
typedef union {
  struct {
    UINT32 t_ckev : 8;
    /* t_ckev - Bits[7:0], RW, default = 8'd4 
       Input buffers (DCKE0 and DCKE1) disable time (float) after Ck/CK# = LOW
       (not needed since DDRIO cannot program the CKE to be tristated.
     */
    UINT32 t_ckoff : 8;
    /* t_ckoff - Bits[15:8], RW, default = 8'd5 
       tCKOFF timing parameter:
       Number of tCK required for both DCKE0 and DCKE1 to remain LOW before both CK/CK# 
       are driven Low 
       Old minimum setting is 2.
       With cmd/address parity error recovery enabled. THis shold be set a number large 
       enough to assure that parity error 
       on a self refresh entry has enough time to get back into MC and start the 
       recovery before clocks are turned off 
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RDIMMTIMINGCNTL2_MCDDC_CTL_STRUCT;


/* TCMRS_MCDDC_CTL_REG supported on:                                          */
/*       SKX_A0 (0x40252244)                                                  */
/*       SKX (0x40252244)                                                     */
/* Register default value:              0x00000008                            */
#define TCMRS_MCDDC_CTL_REG 0x0B004244
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x244
 */
typedef union {
  struct {
    UINT32 tmrd_ddr3 : 4;
    /* tmrd_ddr3 - Bits[3:0], RW, default = 4'b1000 
       DDR3 tMRD timing parameter. MRS to MRS minimum delay in number of DCLK.
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCMRS_MCDDC_CTL_STRUCT;


/* CPGC_PDA_SHADOW_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252248)                                                  */
/*       SKX (0x40252248)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PDA_SHADOW_MCDDC_CTL_REG 0x0B004248
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Shadow of main Per Dram Addressable register in mc_rrd
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 20;
    /* rsvd_0 - Bits[19:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank_select : 3;
    /* rank_select - Bits[22:20], RW_LB, default = 3'b000 
       This field is the logical rank select for PDA transaction and should match the 
       same field 
       in CPGC_PDA register and should be set to match Chip Select fields programmed 
       into 
       CADB for this transaction.
     */
    UINT32 rsvd_23 : 9;
    /* rsvd_23 - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PDA_SHADOW_MCDDC_CTL_STRUCT;


/* ERF_DDR4_CMD_REG0_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x4025224C)                                                  */
/*       SKX (0x4025224C)                                                     */
/* Register default value:              0x00000000                            */
#define ERF_DDR4_CMD_REG0_MCDDC_CTL_REG 0x0B00424C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Errof Flow DDR4 command register 0
 */
typedef union {
  struct {
    UINT32 ad : 13;
    /* ad - Bits[12:0], RW, default = 13'b0000000000000 
       This field holds the command field which goes through address(A) pins of DDR4 
       interface . 
     */
    UINT32 bank : 4;
    /* bank - Bits[16:13], RW, default = 4'b0000 
       This field holds the command field which goes through BA pins of DDR4 interface 
       . 
     */
    UINT32 rcw : 3;
    /* rcw - Bits[19:17], RW, default = 3'b000 
       This field holds the RAS#,CAS#,WE# fields of DDR4 interface
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[29:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdimm : 1;
    /* rdimm - Bits[30:30], RW, default = 1'b0 
       If this field is set to 1, the content of this register should be given to 
       register part of the dimm OR only one rank of the dimm. Other wise the command 
       should be given to all the ranks of the channel. 
     */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'b0 
       This field enables this register. The Error flow FSM uses the contents of this 
       register only if en bit is set. When DDR4 DIMM(s) is populated 
       (ddr4_ca_ctl.erf_en0 = 1), at least one of the erf_ddr4_cmd_reg* should have the 
       en bit set to 1. 
     */
  } Bits;
  UINT32 Data;
} ERF_DDR4_CMD_REG0_MCDDC_CTL_STRUCT;


/* ERF_DDR4_CMD_REG1_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252250)                                                  */
/*       SKX (0x40252250)                                                     */
/* Register default value:              0x00000000                            */
#define ERF_DDR4_CMD_REG1_MCDDC_CTL_REG 0x0B004250
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Errof Flow DDR4 command register 1
 */
typedef union {
  struct {
    UINT32 ad : 13;
    /* ad - Bits[12:0], RW, default = 13'b0000000000000 
       This field holds the command field which goes through address(A) pins of DDR4 
       interface . 
     */
    UINT32 bank : 4;
    /* bank - Bits[16:13], RW, default = 4'b0000 
       This field holds the command field which goes through BA pins of DDR4 interface 
       . 
     */
    UINT32 rcw : 3;
    /* rcw - Bits[19:17], RW, default = 3'b000 
       This field holds the RAS#,CAS#,WE# fields of DDR4 interface
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[29:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdimm : 1;
    /* rdimm - Bits[30:30], RW, default = 1'b0 
       If this field is set to 1, the content of this register should be given to 
       register part of the dimm OR only one rank of the dimm. Other wise the command 
       should be given to all the ranks of the channel. 
     */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'b0 
       This field enables this register. The Error flow FSM uses the contents of this 
       register only if en bit is set. When DDR4 DIMM(s) is populated 
       (ddr4_ca_ctl.erf_en0 = 1), at least one of the erf_ddr4_cmd_reg* should have the 
       en bit set to 1. 
     */
  } Bits;
  UINT32 Data;
} ERF_DDR4_CMD_REG1_MCDDC_CTL_STRUCT;


/* ERF_DDR4_CMD_REG2_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252254)                                                  */
/*       SKX (0x40252254)                                                     */
/* Register default value:              0x00000000                            */
#define ERF_DDR4_CMD_REG2_MCDDC_CTL_REG 0x0B004254
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Errof Flow DDR4 command register 2
 */
typedef union {
  struct {
    UINT32 ad : 13;
    /* ad - Bits[12:0], RW, default = 13'b0000000000000 
       This field holds the command field which goes through address(A) pins of DDR4 
       interface . 
     */
    UINT32 bank : 4;
    /* bank - Bits[16:13], RW, default = 4'b0000 
       This field holds the command field which goes through BA pins of DDR4 interface 
       . 
     */
    UINT32 rcw : 3;
    /* rcw - Bits[19:17], RW, default = 3'b000 
       This field holds the RAS#,CAS#,WE# fields of DDR4 interface
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[29:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdimm : 1;
    /* rdimm - Bits[30:30], RW, default = 1'b0 
       If this field is set to 1, the content of this register should be given to 
       register part of the dimm OR only one rank of the dimm. Other wise the command 
       should be given to all the ranks of the channel. 
     */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'b0 
       This field enables this register. The Error flow FSM uses the contents of this 
       register only if en bit is set. When DDR4 DIMM(s) is populated 
       (ddr4_ca_ctl.erf_en0 = 1), at least one of the erf_ddr4_cmd_reg* should have the 
       en bit set to 1. 
     */
  } Bits;
  UINT32 Data;
} ERF_DDR4_CMD_REG2_MCDDC_CTL_STRUCT;


/* ERF_DDR4_CMD_REG3_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252258)                                                  */
/*       SKX (0x40252258)                                                     */
/* Register default value:              0x00000000                            */
#define ERF_DDR4_CMD_REG3_MCDDC_CTL_REG 0x0B004258
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Errof Flow DDR4 command register 3
 */
typedef union {
  struct {
    UINT32 ad : 13;
    /* ad - Bits[12:0], RW, default = 13'b0000000000000 
       This field holds the command field which goes through address(A) pins of DDR4 
       interface . 
     */
    UINT32 bank : 4;
    /* bank - Bits[16:13], RW, default = 4'b0000 
       This field holds the command field which goes through BA pins of DDR4 interface 
       . 
     */
    UINT32 rcw : 3;
    /* rcw - Bits[19:17], RW, default = 3'b000 
       This field holds the RAS#,CAS#,WE# fields of DDR4 interface
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[29:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdimm : 1;
    /* rdimm - Bits[30:30], RW, default = 1'b0 
       If this field is set to 1, the content of this register should be given to 
       register part of the dimm OR only one rank of the dimm. Other wise the command 
       should be given to all the ranks of the channel. 
     */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'b0 
       This field enables this register. The Error flow FSM uses the contents of this 
       register only if en bit is set. When DDR4 DIMM(s) is populated 
       (ddr4_ca_ctl.erf_en0 = 1), at least one of the erf_ddr4_cmd_reg* should have the 
       en bit set to 1. 
     */
  } Bits;
  UINT32 Data;
} ERF_DDR4_CMD_REG3_MCDDC_CTL_STRUCT;


/* RPQAGE_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x4025225C)                                                  */
/*       SKX (0x4025225C)                                                     */
/* Register default value:              0x00000000                            */
#define RPQAGE_MCDDC_CTL_REG 0x0B00425C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Read Pending Queue Age Counters. 
 */
typedef union {
  struct {
    UINT32 cpugtcount : 10;
    /* cpugtcount - Bits[9:0], RW, default = 10'b0000000000 
       The name is misleading. Instead, it is RPQ Age Counter for the High priority 
       (VCP) transactions and Critical priority (VC1) isoch transactions issued from 
       M2M. The counter is increased by one every time there's a CAS command sent. When 
       the RPQ Age Counter is equal to this configured field value, the isoch 
       transaction is aged to the next priority level. BIOS must set this field to 
       non-zero value before setting the MCMTR.NORMAL=1. Recommended settings: 40h but 
       subject to revision based from post-silicon application specific performance 
       tuning. 
     */
    UINT32 rsvd_10 : 6;
    /* rsvd_10 - Bits[15:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 iocount : 10;
    /* iocount - Bits[25:16], RW, default = 10'b0000000000 
       The name is misleading. It is RPQ Age Counter for the Medium and Low priority 
       (VC0) non-isoch transactions issued from M2M. The counter is increased by one 
       every time there's a CAS command sent. When the RPQ Age Counter is equal to this 
       configured field value, the non-isoch transaction is aged to the next priority 
       level BIOS must set this field to non-zero value before setting the 
       MCMTR.NORMAL=1. Recommended settings: 100h but subject to revision based from 
       post-silicon application specific performance tuning. 
     */
    UINT32 rsvd_26 : 6;
    /* rsvd_26 - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RPQAGE_MCDDC_CTL_STRUCT;


/* RD_ODT_TBL0_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252260)                                                  */
/*       SKX (0x40252260)                                                     */
/* Register default value:              0x00000000                            */
#define RD_ODT_TBL0_MCDDC_CTL_REG 0x0B004260
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * One entry for each physical rank on each channel.  Each entry defines which ODT 
 * signals are asserted when accessing that rank.Also includes ODT timing control. 
 * Here is the recommended BIOS settings to keep the MC_TERM_RNK_MSK consistent:
 * Set Read ODT mapping - read ODT specifies all ODT pins assertion for a read 
 * targeting at this rank. Please clear read target DIMMs termination rank bit. 
 * The non-target DIMMs termination rank bits must be set. All non-termination 
 * rank in the ODT mapping table must be cleared. 
 */
typedef union {
  struct {
    UINT32 rd_odt_rank0 : 6;
    /* rd_odt_rank0 - Bits[5:0], RW, default = 6'b000000 
       Rank 0 Read ODT pins
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rd_odt_rank1 : 6;
    /* rd_odt_rank1 - Bits[13:8], RW, default = 6'b000000 
       Rank 1 Read ODT pins
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rd_odt_rank2 : 6;
    /* rd_odt_rank2 - Bits[21:16], RW, default = 6'b000000 
       Rank 2 Read ODT pins
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rd_odt_rank3 : 6;
    /* rd_odt_rank3 - Bits[29:24], RW, default = 6'b000000 
       Rank 3 Read ODT pins
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RD_ODT_TBL0_MCDDC_CTL_STRUCT;


/* RD_ODT_TBL1_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252264)                                                  */
/*       SKX (0x40252264)                                                     */
/* Register default value:              0x00000000                            */
#define RD_ODT_TBL1_MCDDC_CTL_REG 0x0B004264
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * One entry for each physical rank on each channel.  Each entry defines which ODT 
 * signals are asserted when accessing that rank.Also includes ODT timing control. 
 * Here is the recommended BIOS settings to keep the MC_TERM_RNK_MSK consistent:
 * Set Read ODT mapping - read ODT specifies all ODT pins assertion for a read 
 * targeting at this rank. Please clear read target DIMMs termination rank bit. 
 * The non-target DIMMs termination rank bits must be set. All non-termination 
 * rank in the ODT mapping table must be cleared. 
 */
typedef union {
  struct {
    UINT32 rd_odt_rank4 : 6;
    /* rd_odt_rank4 - Bits[5:0], RW, default = 6'b000000 
       Rank 4 Read ODT pins
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rd_odt_rank5 : 6;
    /* rd_odt_rank5 - Bits[13:8], RW, default = 6'b000000 
       Rank 5 Read ODT pins
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rd_odt_rank6 : 6;
    /* rd_odt_rank6 - Bits[21:16], RW, default = 6'b000000 
       Rank 6 Read ODT pins
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rd_odt_rank7 : 6;
    /* rd_odt_rank7 - Bits[29:24], RW, default = 6'b000000 
       Rank 7 Read ODT pins
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RD_ODT_TBL1_MCDDC_CTL_STRUCT;


/* RD_ODT_TBL2_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252268)                                                  */
/*       SKX (0x40252268)                                                     */
/* Register default value:              0x00000000                            */
#define RD_ODT_TBL2_MCDDC_CTL_REG 0x0B004268
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * One entry for each physical rank on each channel.  Each entry defines which ODT 
 * signals are asserted when accessing that rank.Also includes ODT timing control. 
 * Here is the recommended BIOS settings to keep the MC_TERM_RNK_MSK consistent:
 * Set Read ODT mapping - read ODT specifies all ODT pins assertion for a read 
 * targeting at this rank. Please clear read target DIMMs termination rank bit. 
 * The non-target DIMMs termination rank bits must be set. All non-termination 
 * rank in the ODT mapping table must be cleared. 
 */
typedef union {
  struct {
    UINT32 rd_odt_rank8 : 6;
    /* rd_odt_rank8 - Bits[5:0], RW, default = 6'b000000 
       Rank 8 Read ODT pins
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rd_odt_rank9 : 6;
    /* rd_odt_rank9 - Bits[13:8], RW, default = 6'b000000 
       Rank 9 Read ODT pins
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 extraleadingodt : 2;
    /* extraleadingodt - Bits[17:16], RW, default = 2'b00 
       Extra Leading ODT cycles
     */
    UINT32 rsvd_18 : 2;
    /* rsvd_18 - Bits[19:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 extratrailingodt : 2;
    /* extratrailingodt - Bits[21:20], RW, default = 2'b00 
       Extra Trailing ODT cycles
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RD_ODT_TBL2_MCDDC_CTL_STRUCT;


/* ERF_DDR4_CMD_REG4_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x4025226C)                                                  */
/*       SKX (0x4025226C)                                                     */
/* Register default value:              0x00000000                            */
#define ERF_DDR4_CMD_REG4_MCDDC_CTL_REG 0x0B00426C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 */
typedef union {
  struct {
    UINT32 ad : 13;
    /* ad - Bits[12:0], RW, default = 13'h0  */
    UINT32 bank : 4;
    /* bank - Bits[16:13], RW, default = 4'h0  */
    UINT32 rcw : 3;
    /* rcw - Bits[19:17], RW, default = 3'h0  */
    UINT32 rsvd : 10;
    /* rsvd - Bits[29:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdimm : 1;
    /* rdimm - Bits[30:30], RW, default = 1'h0  */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'h0 
       This field enables this register. The Error flow FSM uses the contents of this 
       register only if en bit is set. When DDR4 DIMM(s) is populated 
       (ddr4_ca_ctl.erf_en0 = 1), at least one of the erf_ddr4_cmd_reg* should have the 
       en bit set to 1. 
     */
  } Bits;
  UINT32 Data;
} ERF_DDR4_CMD_REG4_MCDDC_CTL_STRUCT;


/* WR_ODT_TBL0_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252270)                                                  */
/*       SKX (0x40252270)                                                     */
/* Register default value:              0x00000000                            */
#define WR_ODT_TBL0_MCDDC_CTL_REG 0x0B004270
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * One entry for each physical rank on each channel.  Each entry defines which ODT 
 * signals are asserted when accessing that rank.Also includes ODT timing control. 
 * Here is the recommended BIOS settings to keep the MC_TERM_RNK_MSK consistent:
 * Set Write ODT mapping - write ODT specified all ODT pins assertion for a write 
 * targeting at this rank. All DIMMs termination rank must have the ODT mask 
 * asserted. All non-termination rank in the ODT mapping table must be cleared. 
 */
typedef union {
  struct {
    UINT32 wr_odt_rank0 : 6;
    /* wr_odt_rank0 - Bits[5:0], RW, default = 6'b000000 
       Rank 0 Write ODT
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wr_odt_rank1 : 6;
    /* wr_odt_rank1 - Bits[13:8], RW, default = 6'b000000 
       Rank 1 Write ODT
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wr_odt_rank2 : 6;
    /* wr_odt_rank2 - Bits[21:16], RW, default = 6'b000000 
       Rank 2 Write ODT
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wr_odt_rank3 : 6;
    /* wr_odt_rank3 - Bits[29:24], RW, default = 6'b000000 
       Rank 3 Write ODT
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} WR_ODT_TBL0_MCDDC_CTL_STRUCT;


/* WR_ODT_TBL1_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252274)                                                  */
/*       SKX (0x40252274)                                                     */
/* Register default value:              0x00000000                            */
#define WR_ODT_TBL1_MCDDC_CTL_REG 0x0B004274
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * One entry for each physical rank on each channel.  Each entry defines which ODT 
 * signals are asserted when accessing that rank.Also includes ODT timing control. 
 * Here is the recommended BIOS settings to keep the MC_TERM_RNK_MSK consistent:
 * Set Write ODT mapping - write ODT specified all ODT pins assertion for a write 
 * targeting at this rank. All DIMMs termination rank must have the ODT mask 
 * asserted. All non-termination rank in the ODT mapping table must be cleared 
 */
typedef union {
  struct {
    UINT32 wr_odt_rank4 : 6;
    /* wr_odt_rank4 - Bits[5:0], RW, default = 6'b000000 
       Rank 4 Write ODT
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wr_odt_rank5 : 6;
    /* wr_odt_rank5 - Bits[13:8], RW, default = 6'b000000 
       Rank 5 Write ODT
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wr_odt_rank6 : 6;
    /* wr_odt_rank6 - Bits[21:16], RW, default = 6'b000000 
       Rank 6 Write ODT
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wr_odt_rank7 : 6;
    /* wr_odt_rank7 - Bits[29:24], RW, default = 6'b000000 
       Rank 7 Write ODT
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} WR_ODT_TBL1_MCDDC_CTL_STRUCT;


/* WR_ODT_TBL2_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252278)                                                  */
/*       SKX (0x40252278)                                                     */
/* Register default value:              0x00000000                            */
#define WR_ODT_TBL2_MCDDC_CTL_REG 0x0B004278
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * One entry for each physical rank on each channel.  Each entry defines which ODT 
 * signals are asserted when accessing that rank.Also includes ODT timing control. 
 * Here is the recommended BIOS settings to keep the MC_TERM_RNK_MSK consistent:
 * Set Write ODT mapping - write ODT specified all ODT pins assertion for a write 
 * targeting at this rank. All DIMMs termination rank must have the ODT mask 
 * asserted. All non-termination rank in the ODT mapping table must be cleared 
 */
typedef union {
  struct {
    UINT32 wr_odt_rank8 : 6;
    /* wr_odt_rank8 - Bits[5:0], RW, default = 6'b000000 
       Rank 8 Write ODT
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wr_odt_rank9 : 6;
    /* wr_odt_rank9 - Bits[13:8], RW, default = 6'b000000 
       Rank 9 Write ODT
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 extra_leading_odt : 2;
    /* extra_leading_odt - Bits[17:16], RW, default = 2'b00 
       Extra Leading ODT cycles. Only value of 0 or 1 should be used.
     */
    UINT32 rsvd_18 : 2;
    /* rsvd_18 - Bits[19:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 extra_trailing_odt : 2;
    /* extra_trailing_odt - Bits[21:20], RW, default = 2'b00 
       Extra Trailing ODT cycles
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} WR_ODT_TBL2_MCDDC_CTL_STRUCT;


/* ROWHAMCONFIG_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x40252280)                                                  */
/*       SKX (0x40252280)                                                     */
/* Register default value:              0x00180000                            */
#define ROWHAMCONFIG_MCDDC_CTL_REG 0x0B004280
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer configuration register
 */
typedef union {
  struct {
    UINT32 limit : 15;
    /* limit - Bits[14:0], RW_LB, default = 15'b000000000000000 
       Row activate Limit per timewindow.
     */
    UINT32 enable : 2;
    /* enable - Bits[16:15], RW_LB, default = 2'b00 
       Enable. 00 = disabled, other = enabled
     */
    UINT32 throttle_thresh : 5;
    /* throttle_thresh - Bits[21:17], RW_LB, default = 5'b01100 
       FIFO count at which the MC will throttle to avoid missing activates.
     */
    UINT32 rsvd_22 : 6;
    /* rsvd_22 - Bits[27:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 throttle_disable : 1;
    /* throttle_disable - Bits[28:28], RW_LB, default = 1'b0 
       If set to one will disable throttle of scheduler due to exceeding 
       throttle_thresh. 
     */
    UINT32 disable2 : 1;
    /* disable2 - Bits[29:29], RW_LB, default = 1'b0 
       If set to one will disable Pass Gate tracking but will allow inject_refresh to 
       be used. 
     */
    UINT32 inject_refresh : 1;
    /* inject_refresh - Bits[30:30], RW_LV, default = 1'b0 
       Set to one to inject a Pass Gate refresh. The low 13 bits of the limit field are 
       repurposed to provide the CID,rank,bank and upper row bits to inject.   
       [13:11] Cid
       [10:8] Rank
       [7:4] if DDR4 device then {Bank,BankGroup} otherwise {1'b0,Bank}
       [3:0] Row[17:14]
       Row[13:0] is hard coded to be 0x100. When using this feature, the Pass Gate 
       logic needs to be enabled by setting the enable field to 3 but the tracker needs 
       to be disabled by setting the disable2 bit to 1. There is no need to write the 
       Trigger bit to configure this operation. After the refresh has been injected, 
       this bit will clear back to zero. 
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ROWHAMCONFIG_MCDDC_CTL_STRUCT;


/* ROWHAMCONFIG2_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x40252284)                                                  */
/*       SKX (0x40252284)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAMCONFIG2_MCDDC_CTL_REG 0x0B004284
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer configuration register
 */
typedef union {
  struct {
    UINT32 trr_tmod : 8;
    /* trr_tmod - Bits[7:0], RW_LB, default = 8'b00000000 
       This field is used in TRR mode to specify the value of tMOD used at the end of 
       the refresh. This should be written with the value of (tcsrftp.t_mod + 
       tcdbp.t_rp). Since there is a delay in the logic it is possible to write a value 
       5 less than the result of the addition. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[16:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 half_t_ras : 6;
    /* half_t_ras - Bits[22:17], RW_LB, default = 6'b000000 
       One half the value of t_ras rounded up. This is only used for TRR mode Pass Gate 
       refresh. 
     */
    UINT32 safe_mode : 1;
    /* safe_mode - Bits[23:23], RW_LB, default = 1'b0 
       When this bit is set the machine will block the entire rank when doing the Pass 
       Gate refresh instead of just the banks required. 
     */
    UINT32 restart_delay : 8;
    /* restart_delay - Bits[31:24], RW_LB, default = 8'b00000000 
       The number of Dclk cycles to wait after finishing a Pass Gate refresh before 
       starting another. Note that this must be set to a value larger than PRE to ACT 
       to RD/WR to avoid forward progress issues. 
     */
  } Bits;
  UINT32 Data;
} ROWHAMCONFIG2_MCDDC_CTL_STRUCT;


/* ROWHAMCONFIG3_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x4025228C)                                                  */
/*       SKX (0x4025228C)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAMCONFIG3_MCDDC_CTL_REG 0x0B00428C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer configuration register
 */
typedef union {
  struct {
    UINT32 pass_mask0 : 4;
    /* pass_mask0 - Bits[3:0], RW_LB, default = 4'b0000 
       Which bits of the Row address should not be modified for DIMM0 when sweeping the 
       row. 3:0 corresponds to [17:14] of the row address. 
     */
    UINT32 pass_mask1 : 4;
    /* pass_mask1 - Bits[7:4], RW_LB, default = 4'b0000 
       Which bits of the Row address should not be modified for DIMM1 when sweeping the 
       row. 3:0 corresponds to [17:14] of the row address. 
     */
    UINT32 pass_mask2 : 4;
    /* pass_mask2 - Bits[11:8], RW_LB, default = 4'b0000 
       Which bits of the Row address should not be modified for DIMM1 when sweeping the 
       row. 3:0 corresponds to [17:14] of the row address. 
     */
    UINT32 mirror : 8;
    /* mirror - Bits[19:12], RW_LB, default = 8'b00000000 
       Specifies which ranks use mirroring on the address.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ROWHAMCONFIG3_MCDDC_CTL_STRUCT;


/* TCLRDP_MCDDC_CTL_REG supported on:                                         */
/*       SKX_A0 (0x40252290)                                                  */
/*       SKX (0x40252290)                                                     */
/* Register default value:              0x00001092                            */
#define TCLRDP_MCDDC_CTL_REG 0x0B004290
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Timing parameters for optimizing CAS-to-CAS turnarounds and LRDIMM.
 */
typedef union {
  struct {
    UINT32 t_rrds : 3;
    /* t_rrds - Bits[2:0], RW_LB, default = 3'b010 
       This register controls timing between different subranks of the same logical 
       rank. 
       Timings between CAS commands to the same subrank of a logical rank use the 
       existing non-LRDIMM same-rank timings. 
       Back to back READ to READ from different subranks within the same logical rank 
       separation parameter for LRDIMM. 
       The actual READ-READ command separation is (T_CCD + 4) + (T_RRDS + 1)) in DCLK 
       measured between the clock assertion edges of the two corresponding asserted 
       command CS#. 
       Note: T_RRDS + 1 is the actual separation and the T_CCD + 4 is the actual T_CCD 
       on JEDEC spec.dges of the two corresponding asserted command CS#. 
     */
    UINT32 t_wwds : 3;
    /* t_wwds - Bits[5:3], RW_LB, default = 3'b010 
       This register controls timing between different subranks of the same logical 
       rank. 
       Timings between CAS commands to the same subrank of a logical rank use the 
       existing non-LRDIMM same-rank timings. 
       Back to back WRITE to WRITE from different subranks within the same logical rank 
       separation parameter for LRDIMM. 
       The actual WRITE to WRITE command separation is (T_CCD + 4) + (T_WWDS + 1)) in 
       DCLK measured between the clock assertion edges of the two corresponding 
       asserted command CS#. 
       Note: T_WWDS + 1 is the actual separation and the T_CCD + 4 is the actual T_CCD 
       on JEDEC spec. 
     */
    UINT32 t_rwds : 5;
    /* t_rwds - Bits[10:6], RW_LB, default = 5'b00010 
       This register controls timing between different subranks of the same logical 
       rank. 
       Timings between CAS commands to the same subrank of a logical rank use the 
       existing non-LRDIMM same-rank timings. 
       Back to back READ to WRITE from different subranks within the same logical rank 
       separation parameter for LRDIMM. 
       The actual READ to WRITE command separation is (T_CCD + 4) + (T_RWDS + 1)) in 
       DCLK measured between the clock assertion edges of the two corresponding 
       asserted command CS#. 
       Note: T_RWDS + 1 is the actual separation and the T_CCD + 4 is the actual T_CCD 
       on JEDEC spec. 
     */
    UINT32 t_wrds : 5;
    /* t_wrds - Bits[15:11], RW_LB, default = 5'b00010 
       back to back WRITE to READ from different subranks within the same logical rank 
       separation parameter for LRDIMM. The actual WRITE to READ command separation is 
       (T_CCD + 4) + (T_WRDS + 1)) in DCLK measured between the clock assertion edges 
       of the two corresponding asserted command CS#. 
       This register controls timing between different subranks of the same logical 
       rank. 
       Timings between CAS commands to the same subrank of a logical rank use the 
       existing non-LRDIMM same-rank timings. 
       Back to back WRITE to READ from different subranks within the same logical rank 
       separation parameter for LRDIMM. 
       The actual WRITE to READ command separation is (T_CCD + 4) + (T_WRDS + 1)) in 
       DCLK measured between the clock assertion edges of the two corresponding 
       asserted command CS#. 
       Note: T_WRDS + 1 is the actual separation and the T_CCD + 4 is the actual T_CCD 
       on JEDEC spec. 
     */
    UINT32 rsvd_16 : 2;
    /* rsvd_16 - Bits[17:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 en_3ds : 1;
    /* en_3ds - Bits[18:18], RW_LB, default = 1'b0 
       Enable 3DS DIMM/LRDIMM support
     */
    UINT32 rm2x0 : 2;
    /* rm2x0 - Bits[20:19], RW_LB, default = 2'b00 
       This bit field is used for dimm0 subrank tracking timing support and it needs to 
       be programmed consistantly with respect to 
       ddr4_3dsnumranks_cs (for ddr4) and other fields which determine Rank 
       Multiplication for ddr3. 
       When subrank tracking is turned on, read->read, 
       read->write,write->read,write->write timings between subranks will be controlled 
       by 
       t_rrds,t_rwds,t_wrds,t_wwds timing registers for different bankgroups and *_l_ds 
       registers for same bankgroup 
       otherwise version of registers which are not *ds(different sub rank), 
       *dr(different rank), or *dd(different dimm) will be used 
       00: subrank tracking is turned off; this hould be used if there are no subranks 
       in this slot 
       01: subrank tracking is turned on assuming that number of subranks per chip 
       select is 2. 
       10: subrank tracking is turned on assuming that number of subranks per chip 
       select is 4. 
       11: subrank tracking is turned on assuming that number of subranks per chip 
       select is 8. 
     */
    UINT32 rm2x1 : 2;
    /* rm2x1 - Bits[22:21], RW_LB, default = 2'b00 
       Same as rm2x0 except this is for dimm slot 1.
       Since subrank tracking does not support 8 subranks per chip select in slot 1 in 
       a 3dim configuration, value of 11 for this filed is illegal for 3 dimm configs 
       and hardware will turn off sub rank tracking for all slots if this illegal value 
       is programmed. 
     */
    UINT32 rm2x2 : 2;
    /* rm2x2 - Bits[24:23], RW_LB, default = 2'b00 
       Reserved Unused. DIMM 2 not supported for SKX.
     */
    UINT32 en_pgta2 : 1;
    /* en_pgta2 - Bits[25:25], RW_LB, default = 1'b0 
       Enable page table aliasing on rank bit 2. If this bit is 1, page table entries 
       will alias on rank bit 2; if this bit is 0, page table entries will alias on 
       rank bit 1. (Aliasing on rank bit 2 will impact performance for 2DPC or 3DPC; 
       aliasing on rank bit 1 will impact performance for QR DIMMs or 3DPC.) 
     */
    UINT32 rsvd_26 : 1;
    /* rsvd_26 - Bits[26:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_ccd_wr : 3;
    /* t_ccd_wr - Bits[29:27], RW_LB, default = 3'b000 
       Back to back WRITE to WRITE CAS to CAS from same rank separation parameter.
       The actual JEDEC CAS to CAS command separation for writes is (T_CCD_WR + 4) 
       DCLKs measured between the clock assertion edges of the two corresponding 
       asserted command CS#. 
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 en_bl4 : 1;
    /* en_bl4 - Bits[31:31], RW_LB, default = 1'b0 
       Enable BL4 timing support. This is only intended to be used for Intel LINK 2 1:1 
       mode, where it allows 
       values for the CAS2CAS turnarounds that are smaller by 2 DCLKs.
     */
  } Bits;
  UINT32 Data;
} TCLRDP_MCDDC_CTL_STRUCT;


/* TCLRDP1_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x40252294)                                                  */
/*       SKX (0x40252294)                                                     */
/* Register default value:              0x00000012                            */
#define TCLRDP1_MCDDC_CTL_REG 0x0B004294
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Timing parameters for optimizing CAS-to-CAS turnarounds for ddr4 3ds
 */
typedef union {
  struct {
    UINT32 t_rr_l_ds : 3;
    /* t_rr_l_ds - Bits[2:0], RW_LB, default = 3'b010 
       In ddr4 mode when sub rank tracking is turned on, this field will determine 
       RD->RD spacing when accesses are to different sub_ranks but same_bankgroup; 
       where as tclrdp.t_rrds will be used when accesses are to different sub_ranks but 
       different_bankgroups. Spec is in flux so we add a register. 
     */
    UINT32 t_ww_l_ds : 3;
    /* t_ww_l_ds - Bits[5:3], RW_LB, default = 3'b010 
       In ddr4 mode when sub rank tracking is turned on, this field will determine 
       WR->WR spacing when accesses are to different sub_ranks but same_bankgroup; 
       where as tclrdp.t_wwds will be used when accesses are to different sub_ranks but 
       different_bankgroups. Spec is in flux so we add a register. 
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCLRDP1_MCDDC_CTL_STRUCT;


/* TCOTHP2_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x40252298)                                                  */
/*       SKX (0x40252298)                                                     */
/* Register default value:              0x00010842                            */
#define TCOTHP2_MCDDC_CTL_REG 0x0B004298
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x298
 */
typedef union {
  struct {
    UINT32 t_rwdr : 5;
    /* t_rwdr - Bits[4:0], RW_LB, default = 5'b00010 
       Read to write different rank dead cycles
       Back to back READ to WRITE from different RANK separation parameter. The actual 
       READ to WRITE command separation is 
       TCDBP.T_CL - TCDBP.T_CWL + T_RWDR + 6 DCLKs measured between the clock assertion 
       edges of the two corresponding asserted command CS#. 
       The maximum design range from the above calculation is 30.
     */
    UINT32 t_rwdd : 5;
    /* t_rwdd - Bits[9:5], RW_LB, default = 5'b00010 
       Read to write different dimm dead cycles
       Back to back READ to WRITE from different DIMM separation parameter. The actual 
       READ to WRITE command separation is 
       TCDBP.T_CL - TCDBP.T_CWL + T_RWDD + 6 DCLKs measured between the clock assertion 
       edges of the two corresponding asserted command CS#. 
       The maximum design range from the above calculation is 30.
     */
    UINT32 t_rwsr : 5;
    /* t_rwsr - Bits[14:10], RW_LB, default = 5'b00010 
       Read to write same rank dead cycles
       Back to back READ to WRITE from same rank separation parameter. The actual READ 
       to WRITE command separation targetting same rank is 
       TCDBP.T_CL - TCDBP.T_CWL + T_RWSR + 6 DCLKs measured between the clock assertion 
       edges of the two corresponding asserted command CS#. 
       The maximum design range from the above calculation is 30.
       For LRDIMM running in rank multiplication mode, IMC will continue to use the 
       above equation for T_RWSR even if the READ and WRITE are targeting same logical 
       rank but at different physical ranks behind the LRDIMM buffer, i.e. IMC will not 
       be able to dynamically switch to T_RWDR timing. In order to avoid timing 
       violation in this scenario, BIOS must configure the T_RWSR parameter to be the 
       MAX(T_RWSR of LRDIMM, T_RWDR'). Note: Due to the lighter electrical loading 
       behind the LRDIMM buffer, further optimization can be tuned during post-silicon 
       to reduce the T_RWDR' paraemeter instead of directly using the TCOTHP2.T_RWDR 
       parameter. 
       
     */
    UINT32 t_wrdd : 5;
    /* t_wrdd - Bits[19:15], RW_LB, default = 5'b00010 
       Back to back WRITE to READ from different DIMM separation parameter.The actual 
       WRITE to READ command separation is 
       TCDBP.T_CWL - TCDBP.T_CL + T_WRDD + 6 DCLKs measured between the clock assertion 
       edges of the two corresponding asserted command CS#. 
     */
    UINT32 t_cwl_adj_neg : 2;
    /* t_cwl_adj_neg - Bits[21:20], RW_LB, default = 2'b00 
       This register defines reduction of WR data delay per channel in order to 
       overcome the WR-flyby issue. 
       The total CAS write latency that the DDR sees is the sum of T_CWL and the 
       T_CWL_ADJ minus T_CWL_ADJ_NEG. 
       00 - no latency reduction (default)
       01 - reduce latency by 1 Dclk
       10 - reduce latency by 2 Dclk
       11 - reduce latency by 3 Dclk
       The total t_cwl (T_CWL + T_CWL_ADJ - T_CWL_ADJ_NEG) should not be less than 6
     */
    UINT32 t_wr_upper : 2;
    /* t_wr_upper - Bits[23:22], RW_LB, default = 2'b00 
       Bits 6:5 of WRITE recovery time (must be at least 15ns equivalent)
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TCOTHP2_MCDDC_CTL_STRUCT;


/* CAPARINJ_MCDDC_CTL_REG supported on:                                       */
/*       SKX_A0 (0x4025229C)                                                  */
/*       SKX (0x4025229C)                                                     */
/* Register default value:              0x00000001                            */
#define CAPARINJ_MCDDC_CTL_REG 0x0B00429C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 8;
    UINT32 enable : 1;
    /* enable - Bits[8:8], RW_LV, default = 1'b0  */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CAPARINJ_MCDDC_CTL_STRUCT;


/* TDDR4_MCDDC_CTL_REG supported on:                                          */
/*       SKX_A0 (0x402522A0)                                                  */
/*       SKX (0x402522A0)                                                     */
/* Register default value:              0x00000605                            */
#define TDDR4_MCDDC_CTL_REG 0x0B0042A0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x2a0
 */
typedef union {
  struct {
    UINT32 t_rrd_l : 3;
    /* t_rrd_l - Bits[2:0], RW, default = 3'b101 
       ACTIVE to ACTIVE command period, different bank group (must be at least the 
       larger value of 4 DCLK or 6ns) 
     */
    UINT32 rsvd_3 : 5;
    /* rsvd_3 - Bits[7:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_wtr_l : 4;
    /* t_wtr_l - Bits[11:8], RW, default = 4'b0110 
       DCLK delay from start of internal write transaction to internal read command 
       (must be at least the larger value of 4 DCLK or 7.5ns) 
       
     */
    UINT32 rsvd_12 : 4;
    /* rsvd_12 - Bits[15:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_ccd_l : 3;
    /* t_ccd_l - Bits[18:16], RW, default = 3'b000 
       back to back CAS to CAS (READ to READ), same bank group separation parameter.The 
       actual JEDEC CAS to CAS command separation is (T_CCD + 4) DCLKs measured between 
       the clock assertion edges of the two corresponding asserted command CS#. 
     */
    UINT32 rsvd_19 : 5;
    /* rsvd_19 - Bits[23:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_ccd_wr_l : 3;
    /* t_ccd_wr_l - Bits[26:24], RW, default = 3'b000 
       back to back CAS to CAS (WRITE to WRITE), same bank group separation 
       parameter.The actual JEDEC CAS to CAS command separation is (T_CCD + 4) DCLKs 
       measured between the clock assertion edges of the two corresponding asserted 
       command CS#. 
     */
    UINT32 rsvd_27 : 2;
    /* rsvd_27 - Bits[28:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 qrlrdimm2cs : 3;
    /* qrlrdimm2cs - Bits[31:29], RW, default = 3'b000 
       Set to one for DDR4 quad rank LRDIMM with 2 chip selects. Bit 2 is DIMM2. Bit 1 
       is DIMM1. Bit 0 is DIMM0. 
     */
  } Bits;
  UINT32 Data;
} TDDR4_MCDDC_CTL_STRUCT;


/* ERF_DDR4_CMD_REG5_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402522A4)                                                  */
/*       SKX (0x402522A4)                                                     */
/* Register default value:              0x00000000                            */
#define ERF_DDR4_CMD_REG5_MCDDC_CTL_REG 0x0B0042A4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 */
typedef union {
  struct {
    UINT32 ad : 13;
    /* ad - Bits[12:0], RW, default = 13'h0  */
    UINT32 bank : 4;
    /* bank - Bits[16:13], RW, default = 4'h0  */
    UINT32 rcw : 3;
    /* rcw - Bits[19:17], RW, default = 3'h0  */
    UINT32 rsvd : 10;
    /* rsvd - Bits[29:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdimm : 1;
    /* rdimm - Bits[30:30], RW, default = 1'h0  */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'h0 
       This field enables this register. The Error flow FSM uses the contents of this 
       register only if en bit is set. When DDR4 DIMM(s) is populated 
       (ddr4_ca_ctl.erf_en0 = 1), at least one of the erf_ddr4_cmd_reg* should have the 
       en bit set to 1. 
     */
  } Bits;
  UINT32 Data;
} ERF_DDR4_CMD_REG5_MCDDC_CTL_STRUCT;


/* ERF_DDR4_CMD_REG6_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402522A8)                                                  */
/*       SKX (0x402522A8)                                                     */
/* Register default value:              0x00000000                            */
#define ERF_DDR4_CMD_REG6_MCDDC_CTL_REG 0x0B0042A8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 */
typedef union {
  struct {
    UINT32 ad : 13;
    /* ad - Bits[12:0], RW, default = 13'h0  */
    UINT32 bank : 4;
    /* bank - Bits[16:13], RW, default = 4'h0  */
    UINT32 rcw : 3;
    /* rcw - Bits[19:17], RW, default = 3'h0  */
    UINT32 rsvd : 10;
    /* rsvd - Bits[29:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdimm : 1;
    /* rdimm - Bits[30:30], RW, default = 1'h0  */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'h0 
       This field enables this register. The Error flow FSM uses the contents of this 
       register only if en bit is set. When DDR4 DIMM(s) is populated 
       (ddr4_ca_ctl.erf_en0 = 1), at least one of the erf_ddr4_cmd_reg* should have the 
       en bit set to 1. 
     */
  } Bits;
  UINT32 Data;
} ERF_DDR4_CMD_REG6_MCDDC_CTL_STRUCT;


/* ERF_DDR4_CMD_REG7_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402522AC)                                                  */
/*       SKX (0x402522AC)                                                     */
/* Register default value:              0x00000000                            */
#define ERF_DDR4_CMD_REG7_MCDDC_CTL_REG 0x0B0042AC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 */
typedef union {
  struct {
    UINT32 ad : 13;
    /* ad - Bits[12:0], RW, default = 13'h0  */
    UINT32 bank : 4;
    /* bank - Bits[16:13], RW, default = 4'h0  */
    UINT32 rcw : 3;
    /* rcw - Bits[19:17], RW, default = 3'h0  */
    UINT32 rsvd : 10;
    /* rsvd - Bits[29:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rdimm : 1;
    /* rdimm - Bits[30:30], RW, default = 1'h0  */
    UINT32 en : 1;
    /* en - Bits[31:31], RW, default = 1'h0  */
  } Bits;
  UINT32 Data;
} ERF_DDR4_CMD_REG7_MCDDC_CTL_STRUCT;














/* ROWHAM_THRESHOLD0_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402522D0)                                                  */
/*       SKX (0x402522D0)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAM_THRESHOLD0_MCDDC_CTL_REG 0x0B0042D0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer threshold 0
 */
typedef union {
  struct {
    UINT32 thrshld0_enable : 1;
    /* thrshld0_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable the compare control defined in this register.
     */
    UINT32 thrshld0 : 15;
    /* thrshld0 - Bits[15:1], RW_LB, default = 15'b000000000000000 
       15 bit value to check to determine if TRR is injected or not.
     */
    UINT32 thrshld0_mask : 15;
    /* thrshld0_mask - Bits[30:16], RW_LB, default = 15'b000000000000000 
       15 bit mask to ignore each bit of thrshld0 field.
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ROWHAM_THRESHOLD0_MCDDC_CTL_STRUCT;


/* ROWHAM_THRESHOLD1_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402522D4)                                                  */
/*       SKX (0x402522D4)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAM_THRESHOLD1_MCDDC_CTL_REG 0x0B0042D4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer threshold 1
 */
typedef union {
  struct {
    UINT32 thrshld1_enable : 1;
    /* thrshld1_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable the compare control defined in this register.
     */
    UINT32 thrshld1 : 15;
    /* thrshld1 - Bits[15:1], RW_LB, default = 15'b000000000000000 
       15 bit value to check to determine if TRR is injected or not.
     */
    UINT32 thrshld1_mask : 15;
    /* thrshld1_mask - Bits[30:16], RW_LB, default = 15'b000000000000000 
       15 bit mask to ignore each bit of thrshld1 field.
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ROWHAM_THRESHOLD1_MCDDC_CTL_STRUCT;


/* ROWHAM_SEED_LO_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x402522D8)                                                  */
/*       SKX (0x402522D8)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAM_SEED_LO_MCDDC_CTL_REG 0x0B0042D8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer Seed
 */
typedef union {
  struct {
    UINT32 seed_lo : 32;
    /* seed_lo - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Lower 32b of initial value.
     */
  } Bits;
  UINT32 Data;
} ROWHAM_SEED_LO_MCDDC_CTL_STRUCT;


/* ROWHAM_SEED_HI_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x402522DC)                                                  */
/*       SKX (0x402522DC)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAM_SEED_HI_MCDDC_CTL_REG 0x0B0042DC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer Seed
 */
typedef union {
  struct {
    UINT32 seed_hi : 31;
    /* seed_hi - Bits[30:0], RW_LB, default = 31'b0000000000000000000000000000000 
       Upper 31b of initial value.
     */
    UINT32 seed_load : 1;
    /* seed_load - Bits[31:31], RW_LB, default = 1'b0 
       Intial value is loaded on a 0->1 transition in this field.
     */
  } Bits;
  UINT32 Data;
} ROWHAM_SEED_HI_MCDDC_CTL_STRUCT;


/* ROWHAM_CTL_MCDDC_CTL_REG supported on:                                     */
/*       SKX_A0 (0x402522E0)                                                  */
/*       SKX (0x402522E0)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAM_CTL_MCDDC_CTL_REG 0x0B0042E0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer Control
 */
typedef union {
  struct {
    UINT32 reset_lfsr : 1;
    /* reset_lfsr - Bits[0:0], RW_LB, default = 1'b0 
       If this bit is set to 1 then the hardware that triggers the TRR injection is 
       disabled. 
     */
    UINT32 rsvd_1 : 1;
    /* rsvd_1 - Bits[1:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reset_trr_fifo : 1;
    /* reset_trr_fifo - Bits[2:2], RW_LB, default = 1'b0 
       If this bit is set to 1, then all the outstanding TRR commands are flushed.
     */
    UINT32 dis_rhfifo_lastentry : 1;
    /* dis_rhfifo_lastentry - Bits[3:3], RW_LB, default = 1'b0 
       Disable use of last entry of TRR FIFO. Should set reset_trr_fifo before set this 
       bit. 
     */
    UINT32 rsvd_4 : 28;
    /* rsvd_4 - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ROWHAM_CTL_MCDDC_CTL_STRUCT;


/* ROWHAM_LOG_MCDDC_CTL_REG supported on:                                     */
/*       SKX_A0 (0x402522E4)                                                  */
/*       SKX (0x402522E4)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAM_LOG_MCDDC_CTL_REG 0x0B0042E4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer Log
 */
typedef union {
  struct {
    UINT32 enable_trr_log : 1;
    /* enable_trr_log - Bits[0:0], RW_LB, default = 1'b0 
       Enable logging of TRR information rowwam_log and rowham_log2.
     */
    UINT32 num_injects : 13;
    /* num_injects - Bits[13:1], RW_LV, default = 13'b0000000000000 
       Injections in window.
     */
    UINT32 num_between : 16;
    /* num_between - Bits[29:14], RW_LV, default = 16'b0000000000000000 
       Injections in window.
     */
    UINT32 fifo_empty : 1;
    /* fifo_empty - Bits[30:30], RW_LV, default = 1'b0 
       Injections in window.
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ROWHAM_LOG_MCDDC_CTL_STRUCT;


/* ROWHAM_LOG2_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x402522E8)                                                  */
/*       SKX (0x402522E8)                                                     */
/* Register default value:              0x00000000                            */
#define ROWHAM_LOG2_MCDDC_CTL_REG 0x0B0042E8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Row Hammer Log
 */
typedef union {
  struct {
    UINT32 num_drops : 13;
    /* num_drops - Bits[12:0], RW_LV, default = 13'b0000000000000 
       Dropped cmds in window.
     */
    UINT32 rsvd : 19;
    /* rsvd - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ROWHAM_LOG2_MCDDC_CTL_STRUCT;


/* RSP_FUNC_MCCTRL_ERR_INJ_MCDDC_CTL_REG supported on:                        */
/*       SKX_A0 (0x40252300)                                                  */
/*       SKX (0x40252300)                                                     */
/* Register default value:              0x00000004                            */
#define RSP_FUNC_MCCTRL_ERR_INJ_MCDDC_CTL_REG 0x0B004300
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Error Injection Response Function
 */
typedef union {
  struct {
    UINT32 thr_sel : 2;
    /* thr_sel - Bits[1:0], RWS_L, default = 2'b00 
       00 - Dont inject
       01 - Use Trigger-0
       10 - Use Trigger-1
       11 - Use Trigger-2
     */
    UINT32 thr_function : 2;
    /* thr_function - Bits[3:2], RWS_L, default = 2'b01 
       00 - RSVD
       01 - Throttle reads only
       10 - Throttle writes only
       11 - Throttle reads and writes
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 inj_addr_par_err_sel : 2;
    /* inj_addr_par_err_sel - Bits[9:8], RWS_L, default = 2'b00 
       00 - Dont inject
       01 - Use Trigger-0
       10 - Use Trigger-1
       11 - Use Trigger-2
     */
    UINT32 wr_crdt_thr_sel : 2;
    /* wr_crdt_thr_sel - Bits[11:10], RWS_L, default = 2'b00 
       00 - Don't throttle
       01 - Use Trigger-0
       10 - Use Trigger-1
       11 - Use Trigger-2
     */
    UINT32 rd_crdt_thr_sel : 2;
    /* rd_crdt_thr_sel - Bits[13:12], RWS_L, default = 2'b00 
       00 - Don't throttle
       01 - Use Trigger-0
       10 - Use Trigger-1
       11 - Use Trigger-2
     */
    UINT32 rd_retry_inj_sel : 2;
    /* rd_retry_inj_sel - Bits[15:14], RWS_L, default = 2'b00 
       This field is deprecated.
     */
    UINT32 rsvd_16 : 16;
    /* rsvd_16 - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_MCCTRL_ERR_INJ_MCDDC_CTL_STRUCT;


/* PWMM_STARV_CNTR_PRESCALER_MCDDC_CTL_REG supported on:                      */
/*       SKX_A0 (0x40252304)                                                  */
/*       SKX (0x40252304)                                                     */
/* Register default value:              0x1D008040                            */
#define PWMM_STARV_CNTR_PRESCALER_MCDDC_CTL_REG 0x0B004304
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Partial Write Starvation Counter Pre-scaler
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 24;
    UINT32 wpq_is : 6;
    /* wpq_is - Bits[29:24], RW, default = 6'b011101 
       When WDB level reaches this WM, the MC is in Isoch mode
       Value must be greater than WMM_ENTER and between (RPQ size + 5) and 29.
     */
    UINT32 wmm_exit_in_wim : 1;
    /* wmm_exit_in_wim - Bits[30:30], RW, default = 1'b0  */
    UINT32 dis_wim_exit_block : 1;
    /* dis_wim_exit_block - Bits[31:31], RW, default = 1'b0  */
  } Bits;
  UINT32 Data;
} PWMM_STARV_CNTR_PRESCALER_MCDDC_CTL_STRUCT;


/* WDBWM_MCDDC_CTL_REG supported on:                                          */
/*       SKX_A0 (0x40252308)                                                  */
/*       SKX (0x40252308)                                                     */
/* Register default value:              0x00201519                            */
#define WDBWM_MCDDC_CTL_REG 0x0B004308
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * This register configures the WMM behavior - watermarks and the starvation 
 * counter. 
 * Setup rules that must be kept are:
 * 1 <= WMM_EXIT < WMM_ENTER - 1
 *  WMM_ENTER < WPQ_IS
 *  RPQ_SIZE + 5 <= WPQ_IS
 *  WPQ_IS max value is 29
 */
typedef union {
  struct {
    UINT32 wmm_enter : 8;
    /* wmm_enter - Bits[7:0], RW_LB, default = 8'b00011001 
       When WDB reaches the level defined by this pointer, channel goes into WMM, value 
       must be between 2 and (WPQ_IS - 1). 
       // For JKT A0 bug#3078705 workaround/patch, please set the WMM_ENTER <= WPQ_IS - 
       6 instead. 
     */
    UINT32 wmm_exit : 8;
    /* wmm_exit - Bits[15:8], RW_LB, default = 8'b00010101 
       When channel is in WMM, when WDB level gets to this level the MC goes back to 
       RMM. 
       The value must be between 1 and (WMM_Enter - 1). Initial value is 22
     */
    UINT32 starve_cnt : 16;
    /* starve_cnt - Bits[31:16], RW_LB, default = 16'b0000000000100000 
       This count is used for the starvation switch. If after the WMM transaction count 
       reaches the value in this field and WDB did not go under WMMExit WM, MC returns 
       to RMM for the same number of DCLK and returns back to WMM. 
     */
  } Bits;
  UINT32 Data;
} WDBWM_MCDDC_CTL_STRUCT;
























/* SPARING_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x40252338)                                                  */
/*       SKX (0x40252338)                                                     */
/* Register default value:              0x00000500                            */
#define SPARING_MCDDC_CTL_REG 0x0B004338
/* Struct format extracted from XML file SKX_A0\2.10.2.CFG.xml.
 * Sparing Credit Register
 */
typedef union {
  struct {
    UINT32 sparecrdts : 6;
    /* sparecrdts - Bits[5:0], RW_LB, default = 6'b000000 
       Number of WPQ credits to withold from M2M while sparing is in progress. SKX: 
       always program to 0. 
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wrfifohwm : 6;
    /* wrfifohwm - Bits[13:8], RW_LB, default = 6'b000101 
       Maximum number of merged write isoch transactions allowed in a channel. When 
       this level is exceeded write credits will not be returned. A value of 0 disables 
       this feature and allows any number of merged write isoch transactions, which can 
       lead to unexpected behavior. 
     */
    UINT32 rsvd_14 : 18;
    /* rsvd_14 - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SPARING_MCDDC_CTL_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Sparing Credit Register
 */
typedef union {
  struct {
    UINT32 sparecrdts : 6;
    /* sparecrdts - Bits[5:0], RW_LB, default = 6'b000000 
       Number of WPQ credits to withold from M2M while sparing is in progress. SKX: 
       always program to 0. 
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wrfifohwm : 6;
    /* wrfifohwm - Bits[13:8], RW_LB, default = 6'b000101 
       Maximum number of merged write isoch transactions allowed in a channel. When 
       this level is exceeded write credits will not be returned. A value of 0 disables 
       this feature and allows any number of merged write isoch transactions, which can 
       lead to unexpected behavior. 
     */
    UINT32 rsvd_14 : 1;
    /* rsvd_14 - Bits[14:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 scrub_en : 1;
    /* scrub_en - Bits[15:15], RW_L, default = 1'b0 
       Scrub Enable when set.  
     */
    UINT32 rsvd_16 : 16;
    /* rsvd_16 - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SPARING_MCDDC_CTL_STRUCT;















/* DDR4_WR_STARVE_CTL_MCDDC_CTL_REG supported on:                             */
/*       SKX_A0 (0x40252354)                                                  */
/*       SKX (0x40252354)                                                     */
/* Register default value:              0x00040012                            */
#define DDR4_WR_STARVE_CTL_MCDDC_CTL_REG 0x0B004354
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * 
 *       This register controls a mechanism in the DDR4 scheduler to
 *       issue full writes when the number of writes is below the
 *       threshold to enter WMM, and causes the scheduler to not issue
 *       writes for a long interval. When this mechanism triggers entry
 *       into WMM, then the scheduler will enter WMM and issue all writes that are 
 * in WPQ for a long time. 
 *       
 */
typedef union {
  struct {
    UINT32 timer : 6;
    /* timer - Bits[5:0], RW_LB, default = 6'h12 
       Threshold for number of cycles of in base timer. The threshold field is 
       incremented every (2^timer) DCLK ticks. 
     */
    UINT32 rsvd_6 : 10;
    /* rsvd_6 - Bits[15:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 threshold : 8;
    /* threshold - Bits[23:16], RW_LB, default = 8'h4 
       Threshold for the base timer to wrap around. When the wrap around happens, if 
       any write that was in WPQ from last time the timer wrapped around is not issued, 
       entry into WMM is triggered. Program to 0 to disable mechanism. 
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDR4_WR_STARVE_CTL_MCDDC_CTL_STRUCT;


/* DDRT_TIMING_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252358)                                                  */
/*       SKX (0x40252358)                                                     */
/* Register default value:              0x18101010                            */
#define DDRT_TIMING_MCDDC_CTL_REG 0x0B004358
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Error Injection Response Function
 */
typedef union {
  struct {
    UINT32 ddrtrd_to_ddr4rd : 6;
    /* ddrtrd_to_ddr4rd - Bits[5:0], RW_LB, default = 5'h10 
       Controls turn-around from DDRT GNT to DDR4 Read
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrtrd_to_ddr4wr : 6;
    /* ddrtrd_to_ddr4wr - Bits[13:8], RW_LB, default = 5'h10 
       Controls turn-around from DDRT GNT to DDR4 Write
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrtwr_to_ddr4rd : 6;
    /* ddrtwr_to_ddr4rd - Bits[21:16], RW_LB, default = 5'h10 
       Controls turn-around from DDRT Write to DDR4 Read
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrtwr_to_ddr4wr : 6;
    /* ddrtwr_to_ddr4wr - Bits[29:24], RW_LB, default = 5'h18 
       Controls turn-around from DDRT Write to DDR4 Write
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_TIMING_MCDDC_CTL_STRUCT;






/* WMM_READ_CONFIG_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252400)                                                  */
/*       SKX (0x40252400)                                                     */
/* Register default value:              0x04010100                            */
#define WMM_READ_CONFIG_MCDDC_CTL_REG 0x0B004400
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Control logic to enable issuing RdCAS, Activates for reads and Precharges for 
 * reads from the RPQ in Write Major mode 
 */
typedef union {
  struct {
    UINT32 dis_opp_rd : 1;
    /* dis_opp_rd - Bits[0:0], RW_LB, default = 1'b0 
       Disable issuing read commands opportunistically during WMM.
     */
    UINT32 rsvd_1 : 7;
    /* rsvd_1 - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 act_enable : 1;
    /* act_enable - Bits[8:8], RW_LB, default = 1'b1 
       Disable issuing activate commands for reads opportunistically during WMM.
     */
    UINT32 rsvd_9 : 7;
    /* rsvd_9 - Bits[15:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pre_enable : 1;
    /* pre_enable - Bits[16:16], RW_LB, default = 1'b1 
       Disable issuing precharge commands for reads opportunistically during WMM.
     */
    UINT32 rsvd_17 : 7;
    /* rsvd_17 - Bits[23:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 max_rpq_cas : 4;
    /* max_rpq_cas - Bits[27:24], RW_LB, default = 4'b0100 
       Max number of Read CAS commands that may be issued from the RPQ during a single 
       visit to WMM. 
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} WMM_READ_CONFIG_MCDDC_CTL_STRUCT;


/* BL_EGRESS_CREDIT_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x40252404)                                                  */
/*       SKX (0x40252404)                                                     */
/* Register default value:              0x00221010                            */
#define BL_EGRESS_CREDIT_MCDDC_CTL_REG 0x0B004404
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * BL egress credit for M2M
 */
typedef union {
  struct {
    UINT32 ddr4_egcrdt : 6;
    /* ddr4_egcrdt - Bits[5:0], RW_LB, default = 6'b010000 
       
               Maximum number of BL egress credits reserved for DDR4 reads. 
               
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrt_egcrdt : 6;
    /* ddrt_egcrdt - Bits[13:8], RW_LB, default = 6'b010000 
       Maximum number of BL egress credits reserved for DDRT GNTs.
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 min_ddr4_egrcrdt : 3;
    /* min_ddr4_egrcrdt - Bits[18:16], RW_LB, default = 3'b010 
       
               Min number of BL egress credits that must be available to
               schedule a DDR4 read. This field can be set to 2 for 1LM mode
               but needs to be at least 3 for 2LM mode. 
               
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrt_egrcrdt_low : 3;
    /* ddrt_egrcrdt_low - Bits[22:20], RW_LB, default = 3'b010 
       
               Low threshold for DDRT BL egress credits. If the number of BL
               egress credits falls below this value then DDRT GNTs are
               scheduled with an additional 4 DCLK delay over the programmed
               CMD2GNT. 
               
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 du_slot_number : 3;
    /* du_slot_number - Bits[26:24], RW_LB, default = 3'b000 
       
               The slot number to allow read CAS for current channel. This feature is 
       only enabled when uclk is slower than 
       dclk, and DDRT DIMMs are used. When this feature is enabled 
       (mcmtr.enable_slot_use = 1), early completion for channel 0 should only be sent 
       in slot 0, and channel 1 should only use slot 2, while channel 2 should only use 
       slot 4. A value between 0 and 5 should be programmed in this field to allow the 
       above result. This field should be programmed such that (du_slot_number + read 
       CAS to early completion delay) mod 6 = (2 * current channel number). 
               
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BL_EGRESS_CREDIT_MCDDC_CTL_STRUCT;




/* CPGC_PATWDBCLCTL_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x402524A0)                                                  */
/*       SKX (0x402524A0)                                                     */
/* Register default value:              0x00270000                            */
#define CPGC_PATWDBCLCTL_MCDDC_CTL_REG 0x0B0044A0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern WDB Cache-Line Control -
 * Control logic used to select which cacheline will be tranmitted on writes and 
 * compared on reads. 
 */
typedef union {
  struct {
    UINT32 incr_rate : 5;
    /* incr_rate - Bits[4:0], RW_LB, default = 5'b00000 
       WDB_Increment_Rate indicates how many cachelines before the 
       WDB_Current_Read_Pointer is incremented. 
     */
    UINT32 incr_scale : 1;
    /* incr_scale - Bits[5:5], RW_LB, default = 1'b0 
       1= WDB Increment Rate is a linear number, 0 = WDB Increment Rate is an 
       exponential number 
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 start_pntr : 6;
    /* start_pntr - Bits[13:8], RW_LB, default = 6'b000000 
       Indicates the starting cacheline out of the WDB when a test is started.
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 end_pntr : 6;
    /* end_pntr - Bits[21:16], RW_LB, default = 6'b100111 
       Indicates the ending Cacheline out of the WDB where upon the next increment the 
       WDB_Pointer will be reset to the WDB_Start_Pointer. 
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCLCTL_MCDDC_CTL_STRUCT;


/* CPGC_INORDER_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x402524A4)                                                  */
/*       SKX (0x402524A4)                                                     */
/* Register default value:              0x20000000                            */
#define CPGC_INORDER_MCDDC_CTL_REG 0x0B0044A4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC INORDER Control -
 * This register was spli from cpgc_ckectl register because that register was moved 
 * to mcmnts for physivcal reasons . 
 */
typedef union {
  struct {
    UINT32 rsvd : 29;
    /* rsvd - Bits[28:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_29 : 1;
    UINT32 rpq1_inorder_en : 1;
    /* rpq1_inorder_en - Bits[30:30], RW_LB, default = 1'b0 
       If rpq1_Inorder_En is set to 1, RPQ0 CAS selection order will be in order 
       between entries 23 and 22 in CPGC mode. 
       THis should not be set to 1; for test only
       If rpq1_Inorder_En is set to 0, RPQ cas selection behaviour will not change.
     */
    UINT32 wpq_inorder_en : 1;
    /* wpq_inorder_en - Bits[31:31], RW_LB, default = 1'b0 
       If Wpq_Inorder_En is set to 1, WPQ allocation and write push, CAS selection 
       order will be round robin 
       If Wpq_Inorder_En is set to 0, WPQ allocation and write push, CAS selection will 
       be in normal operation mode; i.e., the first available entry will be selected. 
     */
  } Bits;
  UINT32 Data;
} CPGC_INORDER_MCDDC_CTL_STRUCT;


/* CMD_PAR_INJ_CTL_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x402524A8)                                                  */
/*       SKX (0x402524A8)                                                     */
/* Register default value:              0x00000000                            */
#define CMD_PAR_INJ_CTL_MCDDC_CTL_REG 0x0B0044A8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * DDR Command Parity Injection Control. Use with cmd_par_inj_match and 
 * cmd_par_inj_mask. Locked by epmcctrl_dfx_lck_cntl_rsplck.  
 *                    en_ddrt_ui0, en_ddrt_ui1, and en_ddr4 should always be mutex.
 */
typedef union {
  struct {
    UINT32 en_ddr4 : 1;
    /* en_ddr4 - Bits[0:0], RW_LV, default = 1'b0 
       Enable single injection on DDR4 Command. HW will self clear on injection.
     */
    UINT32 en_ddrt_ui0 : 1;
    /* en_ddrt_ui0 - Bits[1:1], RW_LV, default = 1'b0 
       Enable single injection on DDRT Comnmand, UI0. HW will self clear on injection.
     */
    UINT32 en_ddrt_ui1 : 1;
    /* en_ddrt_ui1 - Bits[2:2], RW_LV, default = 1'b0 
       Enable single injection on DDRT Command, UI1. HW will self clear on injection.
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[29:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddr4safe : 1;
    /* ddr4safe - Bits[30:30], RW, default = 1'b0 
       Enable safe command injection
     */
    UINT32 ddrtsafe : 1;
    /* ddrtsafe - Bits[31:31], RW, default = 1'b0 
       Enable safe command injection
     */
  } Bits;
  UINT32 Data;
} CMD_PAR_INJ_CTL_MCDDC_CTL_STRUCT;


/* CMD_PAR_INJ_MATCH_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402524AC)                                                  */
/*       SKX (0x402524AC)                                                     */
/* Register default value:              0x00000000                            */
#define CMD_PAR_INJ_MATCH_MCDDC_CTL_REG 0x0B0044AC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * DDR Command Parity Injection Match Value. Match will always be done on first UI 
 * for DDRT commands.  
 */
typedef union {
  struct {
    UINT32 cmd : 26;
    /* cmd - Bits[25:0], RW, default = 26'b0 
       
                   Match mapping for DDR4: ACT_b, C[2:0], BG[1:0], BA[1:0], A[17:0]
                   Match mapping for DDRT: ACT_b, RAS_b, CAS_b, WE_b, BG[1:0], BA[1:0], 
       A[17:0] 
               
     */
    UINT32 rank : 3;
    /* rank - Bits[28:26], RW, default = 3'b0 
       Match Value for Rank
     */
    UINT32 rsvd : 3;
    /* rsvd - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CMD_PAR_INJ_MATCH_MCDDC_CTL_STRUCT;


/* CMD_PAR_INJ_MASK_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x402524B0)                                                  */
/*       SKX (0x402524B0)                                                     */
/* Register default value:              0x1FFFFFFF                            */
#define CMD_PAR_INJ_MASK_MCDDC_CTL_REG 0x0B0044B0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * DDR Command Parity Injection Mask Value 
 */
typedef union {
  struct {
    UINT32 cmd : 26;
    /* cmd - Bits[25:0], RW, default = 26'h3ffffff 
       
                   Mask mapping for DDR4: ACT_b, C[2:0], BG[1:0], BA[1:0], A[17:0]
                   Mask mapping for DDRT: ACT_b, RAS_b, CAS_b, WE_b, BG[1:0], BA[1:0], 
       A[17:0] 
               
     */
    UINT32 rank : 3;
    /* rank - Bits[28:26], RW, default = 3'h7 
       Mask Value for Rank
     */
    UINT32 rsvd : 3;
    /* rsvd - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CMD_PAR_INJ_MASK_MCDDC_CTL_STRUCT;




/* CPGC_PATCADBCTL_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252500)                                                  */
/*       SKX (0x40252500)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATCADBCTL_MCDDC_CTL_REG 0x0B004500
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern Command Address Data Buffer Control -
 * Control logic used to select what portions of the CADB will be transmitted 
 * during deselects (when enabled) or compliance modes. 
 */
typedef union {
  struct {
    UINT32 enable_cadb_on_deselect : 1;
    /* enable_cadb_on_deselect - Bits[0:0], RW_LB, default = 1'b0 
       Enable_CADB_on_Deselect enables whether CADB will be driven out during all 
       deselects packets on the bus. 
     */
    UINT32 enable_cadb_always_on : 1;
    /* enable_cadb_always_on - Bits[1:1], RW_LB, default = 1'b0 
       Enable_CADB_Always_On enables the CADB to be driven out on all command and 
       address pins every Dclk while a test is Loopback.Pattern. 
     */
    UINT32 enable_cadb_parityntiming : 1;
    /* enable_cadb_parityntiming - Bits[2:2], RW_LB, default = 1'b0 
       If 1, cadb_parity will be driven on same clock as CMD/Address (i.e. N timing for 
       udimm 
       ddr4 only). If 0, cadb_parity will be driven on the clock after CMD/Address 
       (i.e. N+1 
       timing for all other devices)
     */
    UINT32 enable_onepasscadb_always_on : 1;
    /* enable_onepasscadb_always_on - Bits[3:3], RW_LB, default = 1'b0 
       If this bit is on, mdodifies the behaviour of cad_always_on mode by stopping the 
       test 
       automatically after all 16 entries have been issued out of CADB. If this bit is 
       zero, 
       cadb_always_on_mode behaviour doe snot change; i.e. once we enter this mode, the 
       only 
       exit criteria is if software stops the tese.
     */
    UINT32 enable_pda_doublelength : 1;
    /* enable_pda_doublelength - Bits[4:4], RW_LB, default = 1'b0 
       
                       If this bit is one, any PBA(Per Buffer Addressable)/PDA(Per dram 
       addressable) Writes issued from CADB will 
           be double length (i.e. if lockstep is not enabled, 16 UI instead of 8 UI. If 
       lockstep is enabled, 8 UI instead 
           of 4 UI). This is added to allow more setup up time for PBA accesses during 
       training. 
       				If 0, PBA/PDA writes will look exactly the saem as normal writes.
               
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBCTL_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBMRS_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252504)                                                  */
/*       SKX (0x40252504)                                                     */
/* Register default value:              0x000F0002                            */
#define CPGC_PATCADBMRS_MCDDC_CTL_REG 0x0B004504
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB MRS Control -
 * Used for programming MRS commands when the initialization mode is set to MRS.  
 * The CADB_Start_Pointer dictates where the test starts and after the 
 * CDB_End_Pointer entry is transmitted the test completes. 
 */
typedef union {
  struct {
    UINT32 mrs_gap : 4;
    /* mrs_gap - Bits[3:0], RW_LB, default = 4'b0010 
       2^MRS_Gap is the number of D clocks to wait before continuing to the next CADB 
       entry. 
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mrs_start_pointer : 4;
    /* mrs_start_pointer - Bits[11:8], RW_LB, default = 4'b0000 
       CADB_Start_Pointer describes which entry out of the Command Address Data Buffer 
       (CADB) will be used when a test is started that enables the CADB. 
     */
    UINT32 rsvd_12 : 4;
    /* rsvd_12 - Bits[15:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mrs_end_pointer : 4;
    /* mrs_end_pointer - Bits[19:16], RW_LB, default = 4'b1111 
       CADB_End_Pointer indicates which entry out of the Command Address Data Buffer 
       the Data generation will end the MRS test and will cause a trnisiton back to 
       Loopack.Marker. 
     */
    UINT32 rsvd_20 : 4;
    /* rsvd_20 - Bits[23:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mrs_current_pointer : 4;
    /* mrs_current_pointer - Bits[27:24], RO_V, default = 4'b0000 
       Indicates the current value of the CADB pointer.
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMRS_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBMUXCTL_MCDDC_CTL_REG supported on:                             */
/*       SKX_A0 (0x40252508)                                                  */
/*       SKX (0x40252508)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATCADBMUXCTL_MCDDC_CTL_REG 0x0B004508
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Mux Control -
 * Control Logic used to determine operation of the mux control bits.
 */
typedef union {
  struct {
    UINT32 mux0_control : 2;
    /* mux0_control - Bits[1:0], RW_LB, default = 2'b00 
       0x10=Select LFSR16 0x01=Select 16 bit buffer 0x00=Select LMN counter
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux1_control : 2;
    /* mux1_control - Bits[5:4], RW_LB, default = 2'b00 
       0x10=Select LFSR16 0x01=Select 16 bit buffer 0x00=Select LMN counter
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux2_control : 2;
    /* mux2_control - Bits[9:8], RW_LB, default = 2'b00 
       0x10=Select LFSR16 0x01=Select 16 bit buffer 0x00=Select LMN counter
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux3_control : 2;
    /* mux3_control - Bits[13:12], RW_LB, default = 2'b00 
       0x10=Select LFSR16 0x01=Select 16 bit buffer 0x00=Select LMN counter
     */
    UINT32 rsvd_14 : 18;
    /* rsvd_14 - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUXCTL_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBMUX0PB_MCDDC_CTL_REG supported on:                             */
/*       SKX_A0 (0x4025250C)                                                  */
/*       SKX (0x4025250C)                                                     */
/* Register default value:              0x00010000                            */
#define CPGC_PATCADBMUX0PB_MCDDC_CTL_REG 0x0B00450C
/* Struct format extracted from XML file SKX_A0\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Mux Control Bit #0 Programmable Buffer -
 * Control Logic used as select for the CADB buffer on reads.
 */
typedef union {
  struct {
    UINT32 pattern_buffer : 16;
    /* pattern_buffer - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Used to program the initial value of the CADB Mux# Pattern/LFSR Buffer.
     */
    UINT32 mptive_lfsr_seed : 1;
    /* mptive_lfsr_seed - Bits[16:16], RW_LB, default = 1'b1 
       0: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b0.
       1: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b1.
     */
    UINT32 vmse_cmd_trng_mode : 1;
    /* vmse_cmd_trng_mode - Bits[17:17], RW_LB, default = 1'b0 
       0: LFSR used as Classic Galois polynomial for CADB De-select and Always-on 
       operation 
       1: LFSR used as Fibonacci polynomial for Intel LINK 2 Fine Cmd training
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUX0PB_MCDDC_CTL_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Mux Control Bit #0 Programmable Buffer -
 * Control Logic used as select for the CADB buffer on reads.
 */
typedef union {
  struct {
    UINT32 pattern_buffer : 16;
    /* pattern_buffer - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Used to program the initial value of the CADB Mux# Pattern/LFSR Buffer.
     */
    UINT32 mptive_lfsr_seed : 1;
    /* mptive_lfsr_seed - Bits[16:16], RW_LB, default = 1'b1 
       0: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b0.
       1: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b1.
     */
    UINT32 link_cmd_trng_mode : 1;
    /* link_cmd_trng_mode - Bits[17:17], RW_LB, default = 1'b0 
       0: LFSR used as Classic Galois polynomial for CADB De-select and Always-on 
       operation 
       1: LFSR used as Fibonacci polynomial for Intel LINK 2 Fine Cmd training
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUX0PB_MCDDC_CTL_STRUCT;



/* CPGC_PATCADBMUX1PB_MCDDC_CTL_REG supported on:                             */
/*       SKX_A0 (0x40252510)                                                  */
/*       SKX (0x40252510)                                                     */
/* Register default value:              0x00010000                            */
#define CPGC_PATCADBMUX1PB_MCDDC_CTL_REG 0x0B004510
/* Struct format extracted from XML file SKX_A0\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Mux Control Bit #1 Programmable Buffer -
 * Control Logic used as select for the CADB buffer on reads.
 */
typedef union {
  struct {
    UINT32 pattern_buffer : 16;
    /* pattern_buffer - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Used to program the initial value of the CADB Mux# Pattern/LFSR Buffer.
     */
    UINT32 mptive_lfsr_seed : 1;
    /* mptive_lfsr_seed - Bits[16:16], RW_LB, default = 1'b1 
       0: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b0.
       1: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b1.
     */
    UINT32 vmse_cmd_trng_mode : 1;
    /* vmse_cmd_trng_mode - Bits[17:17], RW_LB, default = 1'b0 
       0: LFSR used as Classic Galois polynomial for CADB De-select and Always-on 
       operation 
       1: LFSR used as Fibonacci polynomial for Intel LINK 2 Fine Cmd training
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUX1PB_MCDDC_CTL_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Mux Control Bit #1 Programmable Buffer -
 * Control Logic used as select for the CADB buffer on reads.
 */
typedef union {
  struct {
    UINT32 pattern_buffer : 16;
    /* pattern_buffer - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Used to program the initial value of the CADB Mux# Pattern/LFSR Buffer.
     */
    UINT32 mptive_lfsr_seed : 1;
    /* mptive_lfsr_seed - Bits[16:16], RW_LB, default = 1'b1 
       0: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b0.
       1: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b1.
     */
    UINT32 link_cmd_trng_mode : 1;
    /* link_cmd_trng_mode - Bits[17:17], RW_LB, default = 1'b0 
       0: LFSR used as Classic Galois polynomial for CADB De-select and Always-on 
       operation 
       1: LFSR used as Fibonacci polynomial for Intel LINK 2 Fine Cmd training
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUX1PB_MCDDC_CTL_STRUCT;



/* CPGC_PATCADBMUX2PB_MCDDC_CTL_REG supported on:                             */
/*       SKX_A0 (0x40252514)                                                  */
/*       SKX (0x40252514)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATCADBMUX2PB_MCDDC_CTL_REG 0x0B004514
/* Struct format extracted from XML file SKX_A0\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Mux Control Bit #2 Programmable Buffer -
 * Control Logic used as select for the CADB buffer on reads.
 */
typedef union {
  struct {
    UINT32 pattern_buffer : 16;
    /* pattern_buffer - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Used to program the initial value of the CADB Mux# Pattern/LFSR Buffer.
     */
    UINT32 mptive_lfsr_seed : 1;
    /* mptive_lfsr_seed - Bits[16:16], RW_LB, default = 1'b0 
       0: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b0.
       1: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b1.
     */
    UINT32 vmse_cmd_trng_mode : 1;
    /* vmse_cmd_trng_mode - Bits[17:17], RW_LB, default = 1'b0 
       0: LFSR used as Classic Galois polynomial for CADB De-select and Always-on 
       operation 
       1: LFSR used as Fibonacci polynomial for Intel LINK 2 Fine Cmd training
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUX2PB_MCDDC_CTL_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Mux Control Bit #2 Programmable Buffer -
 * Control Logic used as select for the CADB buffer on reads.
 */
typedef union {
  struct {
    UINT32 pattern_buffer : 16;
    /* pattern_buffer - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Used to program the initial value of the CADB Mux# Pattern/LFSR Buffer.
     */
    UINT32 mptive_lfsr_seed : 1;
    /* mptive_lfsr_seed - Bits[16:16], RW_LB, default = 1'b0 
       0: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b0.
       1: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b1.
     */
    UINT32 link_cmd_trng_mode : 1;
    /* link_cmd_trng_mode - Bits[17:17], RW_LB, default = 1'b0 
       0: LFSR used as Classic Galois polynomial for CADB De-select and Always-on 
       operation 
       1: LFSR used as Fibonacci polynomial for Intel LINK 2 Fine Cmd training
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUX2PB_MCDDC_CTL_STRUCT;



/* CPGC_PATCADBMUX3PB_MCDDC_CTL_REG supported on:                             */
/*       SKX_A0 (0x40252518)                                                  */
/*       SKX (0x40252518)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATCADBMUX3PB_MCDDC_CTL_REG 0x0B004518
/* Struct format extracted from XML file SKX_A0\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Mux Control Bit #3 Programmable Buffer -
 * Control Logic used as select for the CADB buffer on reads.
 */
typedef union {
  struct {
    UINT32 pattern_buffer : 16;
    /* pattern_buffer - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Used to program the initial value of the CADB Mux# Pattern/LFSR Buffer.
     */
    UINT32 mptive_lfsr_seed : 1;
    /* mptive_lfsr_seed - Bits[16:16], RW_LB, default = 1'b0 
       0: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b0.
       1: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b1.
     */
    UINT32 vmse_cmd_trng_mode : 1;
    /* vmse_cmd_trng_mode - Bits[17:17], RW_LB, default = 1'b0 
       0: LFSR used as Classic Galois polynomial for CADB De-select and Always-on 
       operation 
       1: LFSR used as Fibonacci polynomial for Intel LINK 2 Fine Cmd training
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUX3PB_MCDDC_CTL_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Mux Control Bit #3 Programmable Buffer -
 * Control Logic used as select for the CADB buffer on reads.
 */
typedef union {
  struct {
    UINT32 pattern_buffer : 16;
    /* pattern_buffer - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Used to program the initial value of the CADB Mux# Pattern/LFSR Buffer.
     */
    UINT32 mptive_lfsr_seed : 1;
    /* mptive_lfsr_seed - Bits[16:16], RW_LB, default = 1'b0 
       0: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b0.
       1: Initial seed used for Multiplicative LFSR during Fine Cmd is 1'b1.
     */
    UINT32 link_cmd_trng_mode : 1;
    /* link_cmd_trng_mode - Bits[17:17], RW_LB, default = 1'b0 
       0: LFSR used as Classic Galois polynomial for CADB De-select and Always-on 
       operation 
       1: LFSR used as Fibonacci polynomial for Intel LINK 2 Fine Cmd training
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBMUX3PB_MCDDC_CTL_STRUCT;



/* CPGC_PATCADBCLMUX0LMN_MCDDC_CTL_REG supported on:                          */
/*       SKX_A0 (0x4025251C)                                                  */
/*       SKX (0x4025251C)                                                     */
/* Register default value:              0x01010100                            */
#define CPGC_PATCADBCLMUX0LMN_MCDDC_CTL_REG 0x0B00451C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Cache-Line Mux Control Bit0 LMN Counter Setting -
 * Template for the LMN Counter used for creating periodic pattern (M = HighQclk 
 * cycles + N= Low Qclk cycles with an initial delay = L Qclk cycles) 
 *   - Overlays in address over CPGC_CH#_PAT_CL_MUX#_PB
 */
typedef union {
  struct {
    UINT32 l_data_select : 1;
    /* l_data_select - Bits[0:0], RW_LB, default = 1'b0 
       0x0 = logic 0 must be driven during the initial L counter delay
       0x1=logic 1 must be driven during the initial L counter delay
     */
    UINT32 sweep_freq : 1;
    /* sweep_freq - Bits[1:1], RW_LB, default = 1'b0 
       After L_Counter Dclk cycles are driven at the beginning of a test then periodic 
       pattern is continuously repeated 
       depending on the value of Sweep Frequency.
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 l_counter : 8;
    /* l_counter - Bits[15:8], RW_LB, default = 8'b00000001 
       After L_Counter Dclk cycles are driven at the beginning of a test then periodic 
       pattern is continuously repeated 
       depending on the value of Sweep Frequency.
     */
    UINT32 m_counter : 8;
    /* m_counter - Bits[23:16], RW_LB, default = 8'b00000001 
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M) to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
    UINT32 n_counter : 8;
    /* n_counter - Bits[31:24], RW_LB, default = 8'b00000001 
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M)to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBCLMUX0LMN_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBCLMUX1LMN_MCDDC_CTL_REG supported on:                          */
/*       SKX_A0 (0x40252520)                                                  */
/*       SKX (0x40252520)                                                     */
/* Register default value:              0x01010100                            */
#define CPGC_PATCADBCLMUX1LMN_MCDDC_CTL_REG 0x0B004520
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Cache-Line Mux Control Bit1 LMN Counter Setting -
 * Template for the LMN Counter used for creating periodic pattern (M = HighQclk 
 * cycles + N= Low Qclk cycles with an initial delay = L Qclk cycles) 
 *   - Overlays in address over CPGC_CH#_PAT_CL_MUX#_PB
 */
typedef union {
  struct {
    UINT32 l_data_select : 1;
    /* l_data_select - Bits[0:0], RW_LB, default = 1'b0 
       0x0 = logic 0 must be driven during the initial L counter delay
       0x1=logic 1 must be driven during the initial L counter delay
     */
    UINT32 sweep_freq : 1;
    /* sweep_freq - Bits[1:1], RW_LB, default = 1'b0 
       After L_Counter Dclk cycles are driven at the beginning of a test then periodic 
       pattern is continuously repeated 
       depending on the value of Sweep Frequency.
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 l_counter : 8;
    /* l_counter - Bits[15:8], RW_LB, default = 8'b00000001 
       After L_Counter Dclk cycles are driven at the beginning of a test then periodic 
       pattern is continuously repeated 
       depending on the value of Sweep Frequency.
     */
    UINT32 m_counter : 8;
    /* m_counter - Bits[23:16], RW_LB, default = 8'b00000001 
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M) to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
    UINT32 n_counter : 8;
    /* n_counter - Bits[31:24], RW_LB, default = 8'b00000001 
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M)to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBCLMUX1LMN_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBCLMUX2LMN_MCDDC_CTL_REG supported on:                          */
/*       SKX_A0 (0x40252524)                                                  */
/*       SKX (0x40252524)                                                     */
/* Register default value:              0x01010100                            */
#define CPGC_PATCADBCLMUX2LMN_MCDDC_CTL_REG 0x0B004524
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Cache-Line Mux Control Bit2 LMN Counter Setting -
 * Template for the LMN Counter used for creating periodic pattern (M = HighQclk 
 * cycles + N= Low Qclk cycles with an initial delay = L Qclk cycles) 
 *   - Overlays in address over CPGC_CH#_PAT_CL_MUX#_PB
 */
typedef union {
  struct {
    UINT32 l_data_select : 1;
    /* l_data_select - Bits[0:0], RW_LB, default = 1'b0 
       0x0 = logic 0 must be driven during the initial L counter delay
       0x1=logic 1 must be driven during the initial L counter delay
     */
    UINT32 sweep_freq : 1;
    /* sweep_freq - Bits[1:1], RW_LB, default = 1'b0 
       After L_Counter Dclk cycles are driven at the beginning of a test then periodic 
       pattern is continuously repeated 
       depending on the value of Sweep Frequency.
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 l_counter : 8;
    /* l_counter - Bits[15:8], RW_LB, default = 8'b00000001 
       After L_Counter Dclk cycles are driven at the beginning of a test then periodic 
       pattern is continuously repeated 
       depending on the value of Sweep Frequency.
     */
    UINT32 m_counter : 8;
    /* m_counter - Bits[23:16], RW_LB, default = 8'b00000001 
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M) to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
    UINT32 n_counter : 8;
    /* n_counter - Bits[31:24], RW_LB, default = 8'b00000001 
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M)to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBCLMUX2LMN_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBCLMUX3LMN_MCDDC_CTL_REG supported on:                          */
/*       SKX_A0 (0x40252528)                                                  */
/*       SKX (0x40252528)                                                     */
/* Register default value:              0x01010100                            */
#define CPGC_PATCADBCLMUX3LMN_MCDDC_CTL_REG 0x0B004528
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Cache-Line Mux Control Bit3 LMN Counter Setting -
 * Template for the LMN Counter used for creating periodic pattern (M = HighQclk 
 * cycles + N= Low Qclk cycles with an initial delay = L Qclk cycles) 
 *   - Overlays in address over CPGC_CH#_PAT_CL_MUX#_PB
 */
typedef union {
  struct {
    UINT32 l_data_select : 1;
    /* l_data_select - Bits[0:0], RW_LB, default = 1'b0 
       0x0 = logic 0 must be driven during the initial L counter delay
       0x1=logic 1 must be driven during the initial L counter delay
     */
    UINT32 sweep_freq : 1;
    /* sweep_freq - Bits[1:1], RW_LB, default = 1'b0 
       After L_Counter Dclk cycles are driven at the beginning of a test then periodic 
       pattern is continuously repeated 
       depending on the value of Sweep Frequency.
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 l_counter : 8;
    /* l_counter - Bits[15:8], RW_LB, default = 8'b00000001 
       After L_Counter Dclk cycles are driven at the beginning of a test then periodic 
       pattern is continuously repeated 
       depending on the value of Sweep Frequency.
     */
    UINT32 m_counter : 8;
    /* m_counter - Bits[23:16], RW_LB, default = 8'b00000001 
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M) to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
    UINT32 n_counter : 8;
    /* n_counter - Bits[31:24], RW_LB, default = 8'b00000001 
       If Sweep_Frequency = 0 then a steady state frequency is driven (see 
       Sweep_Frequency for exact behavior) 
       If Sweep_Frequency = 1 then a frequency sweep is continuously generated ranging 
       from 2*(M)to 2*(N) driven (see Sweep_Frequency for exact behavior) 
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBCLMUX3LMN_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBWRPNTR_MCDDC_CTL_REG supported on:                             */
/*       SKX_A0 (0x4025252C)                                                  */
/*       SKX (0x4025252C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATCADBWRPNTR_MCDDC_CTL_REG 0x0B00452C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Write Pointer Control - Used to Enable Read Write Capability 
 * and select the entry within the CADB. 
 * CPGC_PATCADBWRPNTR, CPGC_PATCADBPROG[1:0] are used in conjunction to program 
 * CADB. Steps: 
 * - Keep CPGC_PATCADBWRPNTR at default address of 0x0.
 * - Write to CPGC_PATCADBPROG0. This programs address / bank fields of CADB 
 * Entry-0. 
 * - Write to CPGC_PATCADBPROG1. This programs cs / control / odt / cke / par 
 * fields of CADB Entry-0. Write to CPGC_PATCADBPROG1, results in 
 * CPGC_PATCADBWRPNTR auto-increment by 1. 
 * - Write to CPGC_PATCADBPROG0. This programs address / bank fields of CADB 
 * Entry-1. 
 * - Write to CPGC_PATCADBPROG1. This programs cs / control / odt / cke / par 
 * fields of CADB Entry-1. Write to CPGC_PATCADBPROG1, results in 
 * CPGC_PATCADBWRPNTR auto-increment by 1. 
 * - Repeat steps-2,3 for 16 CADB Entries for complete Buffer programming.
 */
typedef union {
  struct {
    UINT32 write_pointer : 4;
    /* write_pointer - Bits[3:0], RW_LBV, default = 4'b0000 
       Indicates what entry in the CADB is being indexed.
       In order to program CADB, this register should be programmed to a value followed 
       by a 32-bit write to cpgc_patcadbprog0 followed by a 32-bit write to 
       cpgc_patcadbprog1. Upon the last write in this sequence, this register will be 
       autoincrmented; so thoretically one can continue sequences of 
       write_to_cpgc_patcadbprog0 followed by write_to_cpgc_patcadbprog1 16 times if 
       initital value of this register is zero in order to program all entries in CADB. 
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBWRPNTR_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBPROG0_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252530)                                                  */
/*       SKX (0x40252530)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATCADBPROG0_MCDDC_CTL_REG 0x0B004530
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Programming 0 -
 * Buffer-0 for reading and writing of the CADB buffer.
 */
typedef union {
  struct {
    UINT32 address : 18;
    /* address - Bits[17:0], RW_LB, default = 18'b000000000000000000 
       Used to read or write the Address CADB data.
     */
    UINT32 rsvd_18 : 2;
    /* rsvd_18 - Bits[19:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cid : 3;
    /* cid - Bits[22:20], RW_LB, default = 3'b000 
       Used to read or write the Cid CADB data.
       This field is used for ddr4 devices which require Cbits;
       For HDRL ddr3 devices which require address[17], cid[0] maps to address[17]
       since address field is only [16:0].
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 bank : 4;
    /* bank - Bits[27:24], RW_LB, default = 4'b0000 
       Used to read or write the Bank CADB data. maps to {BG[1:0],BA[1:0]} for ddr4 and
       {dontcare,BA[2:0]} for ddr3.
     */
    UINT32 rsvd_28 : 2;
    /* rsvd_28 - Bits[29:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pdatag : 2;
    /* pdatag - Bits[31:30], RW_LB, default = 2'b00 
       Used to read or write the Bank CADB data. this field does not map to any pins. 
       Instead it controls entry and exit from PDA/PBA mode. 
       00: no change in pda/pba mode
       01: entr pda/pba mode.
       Should be used when cadb_entry= "MRS to MR3 with A4=1" (for PDA) or when 
       cadb_entry= "MRS to MR7 with A[12:4]=RC06, A[3:0]=write" (for PBA) after MRS to 
       MR7 with A[12:8]=RC6X,A[0]=1 and MRS to MR7 with A[12:8]=RC4X,A[7:0]=F0BC1X; 
       10: exit pda/pba mode.
       Should be used when cadb_entry= "MRS to MR3 with A4=0" (for PDA) or when 
       cadb_entry= "MRS to MR7 with A[12:4]=RC06, A[3:0]=write" (for PBA) after MRS to 
       MR7 with A[12:8]=RC6X,A[0]=0 and MRS to MR7 with A[12:8]=RC4X,A[7:0]=F0BC1X; 
       11: illegal
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBPROG0_MCDDC_CTL_STRUCT;


/* CPGC_PATCADBPROG1_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252534)                                                  */
/*       SKX (0x40252534)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATCADBPROG1_MCDDC_CTL_REG 0x0B004534
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Pattern CADB Programming 1 -
 * Buffer-1 for reading and writing of the CADB buffer.
 */
typedef union {
  struct {
    UINT32 cs : 10;
    /* cs - Bits[9:0], RW_LB, default = 10'b0000000000 
       Used to read or write the CS CADB data.
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 control : 4;
    /* control - Bits[15:12], RW_LB, default = 4'b0000 
       Used to read or write the Control CADB data.
       Bit-map: [15] - ACTb(ddr4), [14] - RASb, [13] - CASb, [12] - WEb
     */
    UINT32 odt : 6;
    /* odt - Bits[21:16], RW_LB, default = 6'b000000 
       Used to read or write the ODT CADB data.
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cke : 6;
    /* cke - Bits[29:24], RW_LB, default = 6'b000000 
       Used to read or write the CKE CADB data.
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 par : 1;
    /* par - Bits[31:31], RW_LB, default = 1'b0 
       Used to read or write the PAR CADB data.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATCADBPROG1_MCDDC_CTL_STRUCT;


/* CPGC_MISCREFCTL_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252538)                                                  */
/*       SKX (0x40252538)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_MISCREFCTL_MCDDC_CTL_REG 0x0B004538
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Refresh Control -
 * This register is used to control refresh operation in REUT mode.
 * Refreshes will be issued by the REUT block in a sequential circular rank order - 
 * Rank0, Rank1, Rank2, Rank3, Rank0, Rank1, etc. 
 */
typedef union {
  struct {
    UINT32 ref_rankmask : 8;
    /* ref_rankmask - Bits[7:0], RW_LB, default = 8'b00000000 
       Refresh_Rank_Mask is a 8-bit field, one bit per Rank.
       The LSB relates to Rank0 and the MSB relates to the highest Rank.
       If the per Rank bit of the Refresh_Rank_Mask is set to 1, then the REUT engine 
       will disable 
       executing any refresh to that rank while in REUT mode.
       If the per rank bit of the Refresh_Rank_Mask is set to 0 and then the REUT 
       engine will 
       enable executing a refresh (assuming the MC has refreshes enabled) to that Rank
       while in REUT mode based on the Panic_Refresh_Only value.
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[30:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 panic_ref_only : 1;
    /* panic_ref_only - Bits[31:31], RW_LB, default = 1'b0 
       If Panick_Refresh_Only is set to 1 then opportunistic and high priority auto 
       refreshes will be 
       disabled for all ranks and refreshes will only occur when the panic refresh 
       watermark has been exceeded. 
       If Panic_Refresh_Only is set to 0 then refreshes will occur using the existing 
       MC functional refresh logic. 
     */
  } Bits;
  UINT32 Data;
} CPGC_MISCREFCTL_MCDDC_CTL_STRUCT;


/* CPGC_MISCZQCTL_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x4025253C)                                                  */
/*       SKX (0x4025253C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_MISCZQCTL_MCDDC_CTL_REG 0x0B00453C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC ZQ Control -
 * This register is used to control rZQ operation in REUT mode. ZQ will be issued 
 * in REUT mode using the existing MC functional per-rank refresh FSM. 
 */
typedef union {
  struct {
    UINT32 zq_rankmask : 8;
    /* zq_rankmask - Bits[7:0], RW_LB, default = 8'b00000000 
       ZQ Mask is a per rank field. The LSB relates to Rank0 and the MSB relates to the 
       highest Rank. 
       If the per rank bit of the ZQ Mask is set to 1, then the REUT engine will 
       disable executing ZQ operations to that Rank. 
       Note: that if the Refresh_Rank_Mask for the same rank is set to 1 then the ZQ 
       Mask will have no effect and 
       ZQ will disabled to that rank.
       If the per rank bit of the ZQ mask is set to 0 and refreshes are being issued to 
       the corresponding Rank, 
       ZQ will be issued at a rate equal to 128*ZQCS_period programmed inside the 
       channel TC_ZQCAL CR, 
       always occurring after refreshes occur.
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[30:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 always_do_zq : 1;
    /* always_do_zq - Bits[31:31], RW_LB, default = 1'b0 
       If Always_Do_ZQ is set to 1 a ZQ operation will always take place after a 
       refresh occurs as long as 
       the ZQ_Rank_Mask for that rank is set to 0.
       If Always_Do_ZQ is set to 0 a ZQ operation will take place at the normal 
       programmed 128*ZQCS_period after 
       a refresh occurs as long as the ZQ_Rank_Mask for that rank is set to 0.
     */
  } Bits;
  UINT32 Data;
} CPGC_MISCZQCTL_MCDDC_CTL_STRUCT;


/* CPGC_MISCODTCTL_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252540)                                                  */
/*       SKX (0x40252540)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_MISCODTCTL_MCDDC_CTL_REG 0x0B004540
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC ODT Control -
 * This register is used to control CKE powerdown and ODT operation in REUT mode.
 * CKE powerdown will be issued by the normal CKE functional logic and must be 
 * induced by programming the appropriate delays between subsequences to initiate. 
 */
typedef union {
  struct {
    UINT32 odt_override : 6;
    /* odt_override - Bits[5:0], RW_LB, default = 6'b000000 
       ODT_Override is a 6-bit field, one bit per rank.
       The LSB relates to Rank0 and the MSB relates to the highest Rank. When 
       ODT_Override is set to 
       1 for a rank, the ODT functional logic will be bypassed and the ODT_On value for 
       that 
       rank will be used.
       When ODT_Override is set to 0 for a rank, the ODT functional logic will drive 
       the ODT value for that rank. 
     */
    UINT32 rsvd_6 : 9;
    /* rsvd_6 - Bits[14:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 odt_on : 6;
    /* odt_on - Bits[20:15], RW_LB, default = 6'b000000 
       ODT_on is a 6-bit field, one bit per rank.
       The LSB relates to Rank0 and the MSB relates to the highest Rank. When 
       ODT_Override is set to 1 
       for a rank the value in ODT_On will be used as the ODT value for that rank.
     */
    UINT32 rsvd_21 : 10;
    /* rsvd_21 - Bits[30:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mpr_train_ddr_on : 1;
    /* mpr_train_ddr_on - Bits[31:31], RW_LB, default = 1'b0 
       This bit is to be set during DDR training. The MPR pattern (Hardcoded 0101 
       pattern) from the DRAM will be used to initially train the read timing. This MPR 
       pattern is accessed via the MR3 DRAM register and, per the JEDEC spec, the mode 
       comes with two key limitations: 
       1. Before entering this mode, all pages must be precharges.
       2. Once in this mode, the MC can only issue RD or RDA commands.
       When this CR bit is set, the RAS# and WE# will always drive high. This will 
       morph any command from the scheduler into either READ or DESELECT. 
       The flow is as follows:
       1. MRS Mode to write MR3
       2. Set MPR_Train_DDR_On
       3. Set REUT Mode
       4. Wait for all pages to close (page table idle counter to expire)
       5. Run REUT Test
       6. Stop Test, clear MPR_Train_DDR_On bit, clear MR3 register
       7. Continue with other REUT tests
       Note that ZQ, CKE and Refresh should be disabled using the appropriate CR 
       settings during MPR_Train_DDR_On 
     */
  } Bits;
  UINT32 Data;
} CPGC_MISCODTCTL_MCDDC_CTL_STRUCT;


/* CPGC_MISCCKECTL_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252544)                                                  */
/*       SKX (0x40252544)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_MISCCKECTL_MCDDC_CTL_REG 0x0B004544
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC CKE Control -
 * This register is used to control CKE powerdown and ODT operation in REUT mode.
 * CKE powerdown will be issued by the normal CKE functional logic and must be 
 * induced by programming the appropriate delays between subsequences to initiate. 
 */
typedef union {
  struct {
    UINT32 cke_override : 8;
    /* cke_override - Bits[7:0], RW_LB, default = 8'b00000000 
       CKE_Override is a 8-bit field, one bit per logical rank. The LSB relates to 
       Rank0 and the MSB relates 
       to the highest Rank.
       When CKE_Override is set to 1 for a rank, the CKE functional logic will be 
       bypassed and the CKE_On 
       value for that rank will be used.
       When CKE_Override is set to 0 for a rank, the CKE functional logic will drive 
       the CKE value for that rank. 
     */
    UINT32 refresh_enable : 1;
    /* refresh_enable - Bits[8:8], RW_L, default = 1'b0 
       Per Channel Refresh enable
       If cold reset, this bit should be set by bios after
       1) Initializing the refresh timing parameters
       2) Running DDR through reset and init sequence
       If warm reset or S3 exit, this bit should be set immediately after SR exit
     */
    UINT32 rsvd_9 : 6;
    /* rsvd_9 - Bits[14:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cke_on : 8;
    /* cke_on - Bits[22:15], RW_LB, default = 8'b00000000 
       CKE_on is a 8-bit field, one bit per rank.
       The LSB relates to Rank0 and the MSB relates to the highest Rank.
       In REUT mode when the per rank bit is set CKE will be always on for that rank.
     */
    UINT32 rsvd_23 : 9;
    /* rsvd_23 - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_MISCCKECTL_MCDDC_CTL_STRUCT;


/* CPGC_ERR_CTL_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x40252548)                                                  */
/*       SKX (0x40252548)                                                     */
/* Register default value:              0x00000001                            */
#define CPGC_ERR_CTL_MCDDC_CTL_REG 0x0B004548
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Control -
 * This register defines the high level control signals for logging errs while in 
 * Loopback.Pattern and causing a tranisiton from Loopback.Pattern to 
 * Loopback.Marker. 
 */
typedef union {
  struct {
    UINT32 stop_on_ntherr : 6;
    /* stop_on_ntherr - Bits[5:0], RW_LB, default = 6'b000001 
       When Stop_On_Err_Control is set to 0x1 then the test will stop after the 
       Stop_on_Nth_Err. 
     */
    UINT32 cpgc_cntr_x8_mode : 1;
    /* cpgc_cntr_x8_mode - Bits[6:6], RW_LB, default = 1'b0 
       Controls if byte group counters are working in x4 mode or x8 mode.
       0: x4 mode
       1: x8 mode
     */
    UINT32 rsvd_7 : 5;
    /* rsvd_7 - Bits[11:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 stop_on_errcontrol : 3;
    /* stop_on_errcontrol - Bits[14:12], RW_LB, default = 3'h0 
       Defines what type of err is will Stop a test:
       00: Never Stop On Any Err - Prevents any err from causing a test to stop on this 
       channel. 
       01: Stop on the Nth Any Lane Err - If any lane(s) sees an err in a comparison 
       cycle for (Stop_on_Nth_Err) times then stop test. 
       10: Stop on All Byte Groups Err - If Byte_Group_Err_Status indicates an err has 
       happpened in all byte groups then a test is stopped. 
       11: Stop on All Lanes Err - If all data lanes see an err (Data_ Err_Status and 
       ECC_Err_Status) then a test is stopped. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 selective_err_enb_chunk : 8;
    /* selective_err_enb_chunk - Bits[23:16], RW_LB, default = 8'b00000000 
       Bit mask to enable any checking on any specific chunk within the Cache Line. 
       There are only 4 128 bit chunks in a cache line. A 1 in a position enables error 
       checking on that chunk of the cache line. So 0xf checks the whole cache line. 
     */
    UINT32 selective_err_enb_cl : 8;
    /* selective_err_enb_cl - Bits[31:24], RW_LB, default = 8'b00000000 
       Bit mask to enable what Cachelines are checked within each group of 8 received.
       Any 1 bit set indicates that the selected Cacheline must be checked.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_CTL_MCDDC_CTL_STRUCT;


/* CPGC_ERR_DATA0_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x4025254C)                                                  */
/*       SKX (0x4025254C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_DATA0_MCDDC_CTL_REG 0x0B00454C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Bitwise Err Mask -
 * This register defines a mask to block any errs from being logged for specific 
 * Data bits. [31:0] 
 */
typedef union {
  struct {
    UINT32 mask : 32;
    /* mask - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Each bit in Data_Err_Mask corresponds to a specific data lane (Bits 31:0).
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_DATA0_MCDDC_CTL_STRUCT;


/* CPGC_ERR_DATA1_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252550)                                                  */
/*       SKX (0x40252550)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_DATA1_MCDDC_CTL_REG 0x0B004550
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Bitwise Err Mask -
 * This register defines a mask to block any errs from being logged for specific 
 * Data bits. [63:32] 
 */
typedef union {
  struct {
    UINT32 mask : 32;
    /* mask - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Each bit in Data_Err_Mask corresponds to a specific data lane (Bits 63:32).
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_DATA1_MCDDC_CTL_STRUCT;


/* CPGC_ERR_ECC_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x40252554)                                                  */
/*       SKX (0x40252554)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_ECC_MCDDC_CTL_REG 0x0B004554
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Bitwise Err Mask -
 * This register defines a mask to block any errs from being logged for specific 
 * ECC bits. 
 */
typedef union {
  struct {
    UINT32 mask : 8;
    /* mask - Bits[7:0], RW_LB, default = 8'b00000000 
       Each bit in ECC_Err_Mask corresponds to a specific ECC lane (Bits 7:0).
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_ECC_MCDDC_CTL_STRUCT;


/* CPGC_ERR_DATA0_S_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x40252558)                                                  */
/*       SKX (0x40252558)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_DATA0_S_MCDDC_CTL_REG 0x0B004558
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Data Bitwise Err Status 
 */
typedef union {
  struct {
    UINT32 status : 32;
    /* status - Bits[31:0], RW_V, default = 32'b00000000000000000000000000000000 
       Each bit in Data_Err_Status corresponds to a specific data lane.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_DATA0_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_DATA1_S_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x4025255C)                                                  */
/*       SKX (0x4025255C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_DATA1_S_MCDDC_CTL_REG 0x0B00455C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Data Bitwise Err Status
 */
typedef union {
  struct {
    UINT32 status : 32;
    /* status - Bits[31:0], RW_V, default = 32'b00000000000000000000000000000000 
       Each bit in Data_Err_Status corresponds to a specific data lane.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_DATA1_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_DATA2_S_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x40252560)                                                  */
/*       SKX (0x40252560)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_DATA2_S_MCDDC_CTL_REG 0x0B004560
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Data Bitwise Err Status 
 */
typedef union {
  struct {
    UINT32 status : 32;
    /* status - Bits[31:0], RW_V, default = 32'b00000000000000000000000000000000 
       Each bit in Data_Err_Status corresponds to a specific data lane.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_DATA2_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_DATA3_S_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x40252564)                                                  */
/*       SKX (0x40252564)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_DATA3_S_MCDDC_CTL_REG 0x0B004564
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Data Bitwise Err Status 
 */
typedef union {
  struct {
    UINT32 status : 32;
    /* status - Bits[31:0], RW_V, default = 32'b00000000000000000000000000000000 
       Each bit in Data_Err_Status corresponds to a specific data lane.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_DATA3_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_CHUNK_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252568)                                                  */
/*       SKX (0x40252568)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_CHUNK_MCDDC_CTL_REG 0x0B004568
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Chunk Err Status -
 * This register indicates which Chunks within a cacheline have seen an err while 
 * in Loopback.Pattern. 
 */
typedef union {
  struct {
    UINT32 status : 8;
    /* status - Bits[7:0], RW_V, default = 8'b00000000 
       A one hot field with each bit corresponding to a specific Chunk
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_CHUNK_MCDDC_CTL_STRUCT;


/* CPGC_ERR_BYTE_MCDDC_CTL_REG supported on:                                  */
/*       SKX_A0 (0x4025256C)                                                  */
/*       SKX (0x4025256C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_BYTE_MCDDC_CTL_REG 0x0B00456C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Byte Group Err Status -
 * This register indicates the Byte groups that have seen an err while in 
 * Loopback.Pattern. 
 */
typedef union {
  struct {
    UINT32 grp_status : 18;
    /* grp_status - Bits[17:0], RW_V, default = 18'b000000000000000000 
       A one hot field with each bit corresponding to a specific Byte Group (Data byte 
       groups 0-7 and ECC = 8). 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_BYTE_MCDDC_CTL_STRUCT;


/* CPGC_ERR_ADDR0_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252570)                                                  */
/*       SKX (0x40252570)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_ADDR0_MCDDC_CTL_REG 0x0B004570
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Address-0 -
 * This register provides the memory location of the most current err Column/Row 
 * Address. 
 */
typedef union {
  struct {
    UINT32 row : 21;
    /* row - Bits[20:0], RW_V, default = 21'b00000000000000000000 
       Current c-bits (bits 20:18) and Row Address (bits 17:0) of the last cacheline to 
       experince an err. 
     */
    UINT32 rsvd_21 : 3;
    /* rsvd_21 - Bits[23:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank : 3;
    /* rank - Bits[26:24], RW_V, default = 3'b000 
       Current rank Address of the last cacheline to experince an err.
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_ADDR0_MCDDC_CTL_STRUCT;


/* CPGC_ERR_ADDR1_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252574)                                                  */
/*       SKX (0x40252574)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_ADDR1_MCDDC_CTL_REG 0x0B004574
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Address-1 -
 * This register provides the memory location of the most current err Bank/Rank 
 * Address. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 column : 12;
    /* column - Bits[13:2], RW_V, default = 12'b000000000000 
       Current Column Address of the last cacheline to experince an err.
     */
    UINT32 rsvd_14 : 10;
    /* rsvd_14 - Bits[23:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 bank : 4;
    /* bank - Bits[27:24], RW_V, default = 4'b0000 
       Current Bank Address of the last cacheline to experince an err.
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_ADDR1_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER_OVERFLOW_MCDDC_CTL_REG supported on:                      */
/*       SKX_A0 (0x40252578)                                                  */
/*       SKX (0x40252578)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER_OVERFLOW_MCDDC_CTL_REG 0x0B004578
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter Overflow Status -
 * This register indicates the current err counter value overflowed.
 */
typedef union {
  struct {
    UINT32 status : 9;
    /* status - Bits[8:0], RW_V, default = 9'b000000000 
       Counter_Overflow_Status contains the overflow flags for the various 
       REUT_CH#_ERR_COUNTER#_STATUS fileds. 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER_OVERFLOW_MCDDC_CTL_STRUCT;


/* CPGC_ERR_NTH_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x4025257C)                                                  */
/*       SKX (0x4025257C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_NTH_MCDDC_CTL_REG 0x0B00457C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Nth Err Status -
 * This register indicates that at least N errs have occurred while in 
 * Loopback.Pattern. 
 */
typedef union {
  struct {
    UINT32 status : 6;
    /* status - Bits[5:0], RO_V, default = 6'b000000 
       Nth_Err indicates the current Nth Err that has occurred while in 
       Loopback.Pattern. 
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_NTH_MCDDC_CTL_STRUCT;


/* CPGC_ERR_WDB_CAPTURE_STATUS_MCDDC_CTL_REG supported on:                    */
/*       SKX_A0 (0x40252580)                                                  */
/*       SKX (0x40252580)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_WDB_CAPTURE_STATUS_MCDDC_CTL_REG 0x0B004580
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err WDB Capture Status -
 * This register indicates the current pointer for where errs are being logged into 
 * the WDB. 
 */
typedef union {
  struct {
    UINT32 err_pntr : 6;
    /* err_pntr - Bits[5:0], RO_V, default = 6'b000000 
       The WDB_Current_Err_Pointer starts equal to the WDB_Starting_Pointer and every 
       cachline that experinces an err 
       will casue the WDB_Current_Err_Pointer to increment by 1 until it exceeds the 
       WDB_Ending_Err_Pointer where 
       the WDB_Current_Err_Pointer will become equal again tothe 
       WDB_Starting_Err_Pointer. 
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_WDB_CAPTURE_STATUS_MCDDC_CTL_STRUCT;


/* CPGC_ERR_WDB_CAPTURE_CTL_MCDDC_CTL_REG supported on:                       */
/*       SKX_A0 (0x40252584)                                                  */
/*       SKX (0x40252584)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_WDB_CAPTURE_CTL_MCDDC_CTL_REG 0x0B004584
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err WDB Capture Control -
 * This register enables and controls how errs are logged in the WDB.
 */
typedef union {
  struct {
    UINT32 enable : 1;
    /* enable - Bits[0:0], RW_LB, default = 1'b0 
       Setting Enable_WDB_Err_Capture will cause all cachelines that experience any err 
       to be written in the WDB. 
     */
    UINT32 rsvd_1 : 1;
    UINT32 rsvd_2 : 6;
    /* rsvd_2 - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 start_pntr : 6;
    /* start_pntr - Bits[13:8], RW_LB, default = 6'b000000 
       The WDB Starting Err Pointer is used to indicate where the first cacheline that 
       experiences a miscompare is written into the WDB. 
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 end_pntr : 6;
    /* end_pntr - Bits[21:16], RW_LB, default = 6'b000000 
       WDB Ending Pointer is used to indicate where the pointer will wrap around to the 
       WDB Starting Pointer after 
       writing an a cacheline that experiences a miscompare into the WDB using the WDB 
       Enfing Pointer. 
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_WDB_CAPTURE_CTL_MCDDC_CTL_STRUCT;


/* CPGC_PATWDBCL_RDSTS_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x4025258C)                                                  */
/*       SKX (0x4025258C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_PATWDBCL_RDSTS_MCDDC_CTL_REG 0x0B00458C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Read Data Buffer Read Cacheline Status
 */
typedef union {
  struct {
    UINT32 cur_rdpntr : 6;
    /* cur_rdpntr - Bits[5:0], RO_V, default = 6'b000000 
       Indicates the current value of the WDB_Pointer for Read Comparison.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_PATWDBCL_RDSTS_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER0_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402525C8)                                                  */
/*       SKX (0x402525C8)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER0_MCDDC_CTL_REG 0x0B0045C8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-0 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER0_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER0_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x402525CC)                                                  */
/*       SKX (0x402525CC)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER0_S_MCDDC_CTL_REG 0x0B0045CC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-0 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER0_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER1_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402525D0)                                                  */
/*       SKX (0x402525D0)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER1_MCDDC_CTL_REG 0x0B0045D0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-1 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER1_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER1_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x402525D4)                                                  */
/*       SKX (0x402525D4)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER1_S_MCDDC_CTL_REG 0x0B0045D4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-1 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER1_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER2_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402525D8)                                                  */
/*       SKX (0x402525D8)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER2_MCDDC_CTL_REG 0x0B0045D8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-2 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER2_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER2_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x402525DC)                                                  */
/*       SKX (0x402525DC)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER2_S_MCDDC_CTL_REG 0x0B0045DC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-2 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER2_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER3_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402525E0)                                                  */
/*       SKX (0x402525E0)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER3_MCDDC_CTL_REG 0x0B0045E0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-3 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER3_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER3_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x402525E4)                                                  */
/*       SKX (0x402525E4)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER3_S_MCDDC_CTL_REG 0x0B0045E4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-3 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER3_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER4_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402525E8)                                                  */
/*       SKX (0x402525E8)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER4_MCDDC_CTL_REG 0x0B0045E8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-4 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER4_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER4_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x402525EC)                                                  */
/*       SKX (0x402525EC)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER4_S_MCDDC_CTL_REG 0x0B0045EC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-4 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER4_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER5_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402525F0)                                                  */
/*       SKX (0x402525F0)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER5_MCDDC_CTL_REG 0x0B0045F0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-5 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER5_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER5_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x402525F4)                                                  */
/*       SKX (0x402525F4)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER5_S_MCDDC_CTL_REG 0x0B0045F4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-5 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER5_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER6_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x402525F8)                                                  */
/*       SKX (0x402525F8)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER6_MCDDC_CTL_REG 0x0B0045F8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-6 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER6_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER6_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x402525FC)                                                  */
/*       SKX (0x402525FC)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER6_S_MCDDC_CTL_REG 0x0B0045FC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-6 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER6_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER7_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252600)                                                  */
/*       SKX (0x40252600)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER7_MCDDC_CTL_REG 0x0B004600
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-7 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER7_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER7_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x40252604)                                                  */
/*       SKX (0x40252604)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER7_S_MCDDC_CTL_REG 0x0B004604
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-7 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER7_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER8_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252608)                                                  */
/*       SKX (0x40252608)                                                     */
/* Register default value:              0x0000007F                            */
#define CPGC_ERR_COUNTER8_MCDDC_CTL_REG 0x0B004608
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Err Counter-8 Control -
 * This register  is used to control a specific Err Counter.
 */
typedef union {
  struct {
    UINT32 pntr : 7;
    /* pntr - Bits[6:0], RW_LB, default = 7'b1111111 
       Counter_Pointer is used in conjunction with Counter_Control to indicate what 
       lane or byte group is desired for tracking err. 
     */
    UINT32 ctl : 2;
    /* ctl - Bits[8:7], RW_LB, default = 2'b00 
       Counter_Control determines what get counted by 
       REUT_CH#_ERR_COUNTER#_VALUE.Counter_Value 
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER8_MCDDC_CTL_STRUCT;


/* CPGC_ERR_COUNTER8_S_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x4025260C)                                                  */
/*       SKX (0x4025260C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_COUNTER8_S_MCDDC_CTL_REG 0x0B00460C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel Err Counter-8 Status -
 * This register  indicates the current err counter value.
 */
typedef union {
  struct {
    UINT32 status : 23;
    /* status - Bits[22:0], RW_V, default = 23'b00000000000000000000000 
       REUT_CH#_ERR_COUNTER_STATUS indicates the current err counter value which is 
       configured by REUT_CH#_ERR_COUNTER#_CTL. 
     */
    UINT32 rsvd : 9;
    /* rsvd - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_COUNTER8_S_MCDDC_CTL_STRUCT;


/* CPGC_ERR_ECC_S_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252610)                                                  */
/*       SKX (0x40252610)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERR_ECC_S_MCDDC_CTL_REG 0x0B004610
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * CPGC Channel ECC Err Status -
 * This register indicates which ECC lanes have seen an err while in 
 * Loopback.Pattern. 
 */
typedef union {
  struct {
    UINT32 status : 16;
    /* status - Bits[15:0], RW_V, default = 16'b0000000000000000 
       Each bit corresponds to a specific ECC lane.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERR_ECC_S_MCDDC_CTL_STRUCT;


/* DDR4_CBIT_PARITY_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x40252614)                                                  */
/*       SKX (0x40252614)                                                     */
/* Register default value:              0x00000000                            */
#define DDR4_CBIT_PARITY_MCDDC_CTL_REG 0x0B004614
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x614
 */
typedef union {
  struct {
    UINT32 enable : 3;
    /* enable - Bits[2:0], RW_LB, default = 3'b000 
       Per bit inclusion control for ddr4 Cbits in command/address parity generation.
       i.e. if enable[0]=1, C[0] will be included in parity calculation. if 
       enable[1]=1, C[1] 
       will be included; etc.
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDR4_CBIT_PARITY_MCDDC_CTL_STRUCT;


/* BLEGR_CRDT_CTL_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252618)                                                  */
/*       SKX (0x40252618)                                                     */
/* Register default value:              0x00000000                            */
#define BLEGR_CRDT_CTL_MCDDC_CTL_REG 0x0B004618
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Control fields for reserved BL egress credit
 */
typedef union {
  struct {
    UINT32 enable_tgr_cr : 1;
    /* enable_tgr_cr - Bits[0:0], RW_LB, default = 1'b0 
       Enable use of reserved credit for transgress underfill read. Enable this 
       reserved credit means the credits in bl_egress_credit needs to be reduced by 1. 
     */
    UINT32 enable_iot_cr : 1;
    /* enable_iot_cr - Bits[1:1], RW_LB, default = 1'b0 
       Enable use of reserved credit for IOT loopback write. Enable this reserved 
       credit means the credits in bl_egress_credit needs to be reduced by 1. 
     */
    UINT32 thresh_tgr_cr : 12;
    /* thresh_tgr_cr - Bits[13:2], RW_LB, default = 12'h0 
       Time (in number of DCLKs) after the reserved transgress credit is used that a 
       returned BL egress credit isn't destined for this reserved credit. Set to 0 to 
       disable the delay. 
     */
    UINT32 thresh_iot_cr : 12;
    /* thresh_iot_cr - Bits[25:14], RW_LB, default = 12'h0 
       Time (in number of DCLKs) after the reserved iot write credit is used that a 
       returned BL egress credit isn't destined for this reserved credit. Set to 0 to 
       disable the delay. 
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[30:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 iotlb_ddrt : 1;
    /* iotlb_ddrt - Bits[31:31], RW_LB, default = 1'b0 
       When set to 0, IOT DDRIO loopback is for tracing DDR4 write data; when set to 1, 
       IOT DDRIO loopback is for tracing DDRT write data. This field should be set the 
       same value as ddr_trace_throttle_ctl.iotlb_ddrt. 
     */
  } Bits;
  UINT32 Data;
} BLEGR_CRDT_CTL_MCDDC_CTL_STRUCT;


/* MCMNT_CHKN_BIT2_MCDDC_CTL_REG supported on:                                */
/*       SKX_A0 (0x40252620)                                                  */
/*       SKX (0x40252620)                                                     */
/* Register default value:              0x00000070                            */
#define MCMNT_CHKN_BIT2_MCDDC_CTL_REG 0x0B004620
/* Struct format extracted from XML file SKX_A0\2.10.2.CFG.xml.
 * generated by critter 20_0_0x700
 */
typedef union {
  struct {
    UINT32 dis_cke_pri_rcb_gate : 1;
    /* dis_cke_pri_rcb_gate - Bits[0:0], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in CKE
     */
    UINT32 dis_thr_pri_rcb_gate : 1;
    /* dis_thr_pri_rcb_gate - Bits[1:1], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in THR
     */
    UINT32 dis_ref_pri_rcb_gate : 1;
    /* dis_ref_pri_rcb_gate - Bits[2:2], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in REF
     */
    UINT32 dis_alias_preall_blk : 1;
    /* dis_alias_preall_blk - Bits[3:3], RW_LB, default = 1'b0 
       Disable blocking of aliased rank during PREALL states
     */
    UINT32 dis_hpref_blk : 1;
    /* dis_hpref_blk - Bits[4:4], RW_LB, default = 1'b1 
       Disable blocking of high priority refresh due to 
       tstagger_ref.maxnum_active_reffsm is reached 
     */
    UINT32 dis_panicref_blk : 1;
    /* dis_panicref_blk - Bits[5:5], RW_LB, default = 1'b1 
       Disable blocking of panic refresh due to tstagger_ref.maxnum_active_reffsm is 
       reached 
     */
    UINT32 dis_stolenref_blk : 1;
    /* dis_stolenref_blk - Bits[6:6], RW_LB, default = 1'b1 
       Disable blocking of stolen refresh due to tstagger_ref.maxnum_active_reffsm is 
       reached 
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCMNT_CHKN_BIT2_MCDDC_CTL_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x700
 */
typedef union {
  struct {
    UINT32 dis_cke_pri_rcb_gate : 1;
    /* dis_cke_pri_rcb_gate - Bits[0:0], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in CKE
     */
    UINT32 dis_thr_pri_rcb_gate : 1;
    /* dis_thr_pri_rcb_gate - Bits[1:1], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in THR
     */
    UINT32 dis_ref_pri_rcb_gate : 1;
    /* dis_ref_pri_rcb_gate - Bits[2:2], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in REF
     */
    UINT32 dis_alias_preall_blk : 1;
    /* dis_alias_preall_blk - Bits[3:3], RW_LB, default = 1'b0 
       Disable blocking of aliased rank during PREALL states
     */
    UINT32 dis_hpref_blk : 1;
    /* dis_hpref_blk - Bits[4:4], RW_LB, default = 1'b1 
       Disable blocking of high priority refresh due to 
       tstagger_ref.maxnum_active_reffsm is reached 
     */
    UINT32 dis_panicref_blk : 1;
    /* dis_panicref_blk - Bits[5:5], RW_LB, default = 1'b1 
       Disable blocking of panic refresh due to tstagger_ref.maxnum_active_reffsm is 
       reached 
     */
    UINT32 dis_stolenref_blk : 1;
    /* dis_stolenref_blk - Bits[6:6], RW_LB, default = 1'b1 
       Disable blocking of stolen refresh due to tstagger_ref.maxnum_active_reffsm is 
       reached 
     */
    UINT32 ddrt_linkfail_block_park : 1;
    /* ddrt_linkfail_block_park - Bits[7:7], RW_LB, default = 1'b0 
       Do not assert Park to DDRIO in linkfail 
     */
    UINT32 spare : 3;
    /* spare - Bits[10:8], RW_LB, default = 3'b0 
       Spare 
     */
    UINT32 dis_ddrt_srexit_adr : 1;
    /* dis_ddrt_srexit_adr - Bits[11:11], RW_LB, default = 1'b0 
       Set this bit for DDRT to remain in SR if ADR occurs 
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCMNT_CHKN_BIT2_MCDDC_CTL_STRUCT;



/* MCMNT_RCB_GATE_CTL_MCDDC_CTL_REG supported on:                             */
/*       SKX_A0 (0x40252624)                                                  */
/*       SKX (0x40252624)                                                     */
/* Register default value:              0x00000040                            */
#define MCMNT_RCB_GATE_CTL_MCDDC_CTL_REG 0x0B004624
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Control information for RCB clock gating in mcmnts
 */
typedef union {
  struct {
    UINT32 rcb_gate_mcmnt_timer : 10;
    /* rcb_gate_mcmnt_timer - Bits[9:0], RW_LB, default = 10'b0001000000 
       Time clock keeps running after triggering events
     */
    UINT32 rsvd : 22;
    /* rsvd - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCMNT_RCB_GATE_CTL_MCDDC_CTL_STRUCT;


/* MASK_RPQ0_MCDDC_CTL_REG supported on:                                      */
/*       SKX_A0 (0x40252630)                                                  */
/*       SKX (0x40252630)                                                     */
/* Register default value:              0x00000000                            */
#define MASK_RPQ0_MCDDC_CTL_REG 0x0B004630
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Mask_RPQ0 Register
 */
typedef union {
  struct {
    UINT32 mask : 24;
    /* mask - Bits[23:0], RW_LB, default = 24'b000000000000000000000000 
       Set a bit to 1 in this field to prevent the RPQ from allocating into the entry.
       Each bit in the 24b field corresponds to one of the RPQ entry in Entries 23 to 0 
       respectively. 
       Bit 0 blocks RPQ entry 0 and Bit 23 masks RPQ entry 23 and so on.
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MASK_RPQ0_MCDDC_CTL_STRUCT;


/* MASK_RPQ1_MCDDC_CTL_REG supported on:                                      */
/*       SKX_A0 (0x40252634)                                                  */
/*       SKX (0x40252634)                                                     */
/* Register default value:              0x00000000                            */
#define MASK_RPQ1_MCDDC_CTL_REG 0x0B004634
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Mask_Rpq1 Register
 */
typedef union {
  struct {
    UINT32 mask : 24;
    /* mask - Bits[23:0], RW_LB, default = 24'b000000000000000000000000 
       Set a bit to 1 in this field to prevent the RPQ from allocating into the entry.
       Each bit in the 24b field corresponds to one of the RPQ entry in Entries 47 to 
       24 respectively. 
       Bit 0 blocks RPQ entry 24 and Bit 23 masks RPQ entry 47 and so on.
     */
    UINT32 wpq_upper_mask : 8;
    /* wpq_upper_mask - Bits[31:24], RW_LB, default = 8'b00000000 
       Set a bit to 1 in this field to prevent the WPQ from allocating into the entry 
       plus 32. 
       Each bit in the 8 bit field corresponds to one of the upper WPQ Entries 39 to 32 
       respectively. 
       Bit 0 blocks WPQ entry 32 and Bit 7 masks WPQ entry 39 and so on.
     */
  } Bits;
  UINT32 Data;
} MASK_RPQ1_MCDDC_CTL_STRUCT;


/* MASK_WPQ_MCDDC_CTL_REG supported on:                                       */
/*       SKX_A0 (0x40252638)                                                  */
/*       SKX (0x40252638)                                                     */
/* Register default value:              0x00000000                            */
#define MASK_WPQ_MCDDC_CTL_REG 0x0B004638
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Mask_WPQ Register
 */
typedef union {
  struct {
    UINT32 mask : 32;
    /* mask - Bits[31:0], RW_LB, default = 32'b00000000000000000000000000000000 
       Set a bit to 1 in this field to prevent the WPQ from allocating into the entry.
       Each bit in the 32b field corresponds to one of the 32 WPQ entries.
       Bit 0 blocks WPQ entry 0 and Bit 31 masks WPQ entry 32 and so on. Note that
       mask_rpq1 has a field named wpq_upper_mask to control the upper 8 entries of
       the WPQ.
     */
  } Bits;
  UINT32 Data;
} MASK_WPQ_MCDDC_CTL_STRUCT;


/* SREF_STATIC2_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x4025263C)                                                  */
/*       SKX (0x4025263C)                                                     */
/* Register default value:              0x00070303                            */
#define SREF_STATIC2_MCDDC_CTL_REG 0x0B00463C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Second Control for Self Refresh.  Not impacted by PowerModeSelect signal from 
 * PMLink.   
 */
typedef union {
  struct {
    UINT32 idle_cmd_to_cke_drops_timer : 8;
    /* idle_cmd_to_cke_drops_timer - Bits[7:0], RW_LB, default = 8'd3 
       
                       This field defines delay from PM Idle to CKE low to make sure 
       FNV decodes the 
                       command. This CSR should cover the round trip latency to decode 
       the 
                       command, check for Error and drive the Error signal back to the 
       host. 
                       
     */
    UINT32 t_ddrt_cke : 4;
    /* t_ddrt_cke - Bits[11:8], RW_LB, default = 4'b0011 
       CKE minimum pulse width (must be at least the larger value of 3 DCLK or 5ns)
     */
    UINT32 rsvd_12 : 4;
    /* rsvd_12 - Bits[15:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sre_cmd_to_cke_drops_timer : 8;
    /* sre_cmd_to_cke_drops_timer - Bits[23:16], RW_LB, default = 8'd7 
       
                       This field defines delay from SREnter to CKE low to make sure 
       SREnter command doesn't have parity error. 
                       This CSR should cover the round trip latency to decode the
                       command, check for Error and drive the Error signal back to the 
       host. 
                       
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SREF_STATIC2_MCDDC_CTL_STRUCT;


/* SREF_STATIC_MCDDC_CTL_REG supported on:                                    */
/*       SKX_A0 (0x40252644)                                                  */
/*       SKX (0x40252644)                                                     */
/* Register default value:              0x407FFFFF                            */
#define SREF_STATIC_MCDDC_CTL_REG 0x0B004644
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Control for Self Refresh.  Not impacted by PowerModeSelect signal from PMLink.  
 *  
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 ufs_sref_idle_timer : 5;
    /* ufs_sref_idle_timer - Bits[9:5], RWS, default = 5'b11111 
       This is an idle timer (5-bit) to cover the corner case when Mesh2Mem may have no 
       pending transactions to iMC, but may be preparing to send something to the iMC, 
       but iMC enters SR seeing that it has nothing in its queues. In such a case, MC 
       could potentially enter SR and then exit temporarily and then enter into SR 
       again. This happens primarily because it is hard for Pcode to observe the 
       Mesh2Mem block_ack, since the GV block_ack goes to the hardware and does not 
       generate a fastpath to the Pcode. 
     */
    UINT32 mdll_timer : 12;
    /* mdll_timer - Bits[21:10], RW, default = 12'b111111111111 
       Master DLL wake up timer (delay in DCLK).  The MDLL lock time after the DLL 
       Enable is issued, approximately 500ns.  
                           This field is defaulted to 533MHz DCLK initial boot setting. 
       BIOS should reprogram this register for ~500ns equivalent at target speed. Here 
       is the set of the recommended setting for each DCLK speed: 
                           DDR Speed Setting
                           ddr4-1600 190h
                           ddr4-1867 1D3h
                           ddr4-2133 258h
                           ddr4-2667 29Ch
                       
     */
    UINT32 pdwn_rdimm_rc9_a4_a3 : 2;
    /* pdwn_rdimm_rc9_a4_a3 - Bits[23:22], RW, default = 2'b01 
       
                           Bit23: driven on DA4 during the RC9 control word access. 
       Reserved in non-LR-DIMM. In LR-DIMM, For LR-DIMM, DA4=1 for DQ clocking disable 
       in CKE power down. 
                           Bit22: driven on DA3 during the RC9 control word access. For 
       non-LR-DIMM, when set (default), register is in weak drive mode; otherwise, the 
       register is in float mode. 
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[25:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrt_idle_l : 4;
    /* ddrt_idle_l - Bits[29:26], RW, default = 4'b0000 
       Bits to send as part of the DDR-T PM Idle command to indicate the exit latency.  
       Encoding is as per the DDR-T spec.   
     */
    UINT32 pkgc_sref_en : 1;
    /* pkgc_sref_en - Bits[30:30], RW, default = 1'b1 
       Defeature to make sure we have a way to not go into Self Refresh during the PkgC 
       flow for survivability.  
     */
    UINT32 reserved : 1;
    /* reserved - Bits[31:31], RW, default = 1'b0 
        Reserved for future use.  
     */
  } Bits;
  UINT32 Data;
} SREF_STATIC_MCDDC_CTL_STRUCT;


/* POWER_STATUS_MCDDC_CTL_REG supported on:                                   */
/*       SKX_A0 (0x4025264C)                                                  */
/*       SKX (0x4025264C)                                                     */
/* Register default value:              0x00000000                            */
#define POWER_STATUS_MCDDC_CTL_REG 0x0B00464C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Status of Self Refresh and CKE power states
 */
typedef union {
  struct {
    UINT32 in_pkgc : 1;
    /* in_pkgc - Bits[0:0], RWS_V, default = 1'b0 
       Hardware sets this bit to 1 when PCU asserts ForceSrEnter and resets this bit to 
       0 when PCU asserts ForceSrExit bit. 
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} POWER_STATUS_MCDDC_CTL_STRUCT;










/* PMSUM_DDRT_WEIGHT_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252660)                                                  */
/*       SKX (0x40252660)                                                     */
/* Register default value:              0x00000000                            */
#define PMSUM_DDRT_WEIGHT_MCDDC_CTL_REG 0x0B004660
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Energy weight of DDRT commands tracked by pmsum_0, pmsum_1, and pmsum_channel 
 * registers 
 */
typedef union {
  struct {
    UINT32 wr_cmd_0 : 8;
    /* wr_cmd_0 - Bits[7:0], RW, default = 8'b00000000 
       Energy contribution of DDRT write command to DIMM 0.
     */
    UINT32 rd_cmd_0 : 8;
    /* rd_cmd_0 - Bits[15:8], RW, default = 8'b00000000 
       Energy contribution of DDRT read command to DIMM 0.
     */
    UINT32 wr_cmd_1 : 8;
    /* wr_cmd_1 - Bits[23:16], RW, default = 8'b00000000 
       Energy contribution of DDRT write command to DIMM 1.
     */
    UINT32 rd_cmd_1 : 8;
    /* rd_cmd_1 - Bits[31:24], RW, default = 8'b00000000 
       Energy contribution of DDRT read command to DIMM 1.
     */
  } Bits;
  UINT32 Data;
} PMSUM_DDRT_WEIGHT_MCDDC_CTL_STRUCT;


/* PMSUM_MCDDC_CTL_REG supported on:                                          */
/*       SKX_A0 (0x40252664)                                                  */
/*       SKX (0x40252664)                                                     */
/* Register default value:              0x00000000                            */
#define PMSUM_MCDDC_CTL_REG 0x0B004664
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Channel energy accumulator.
 */
typedef union {
  struct {
    UINT32 channel : 32;
    /* channel - Bits[31:0], RW_LV, default = 32'b00000000000000000000000000000000 
        Includes energy terms for DDR4 ACT, REF, Read CAS, and Write CAS, and also DDRT 
       read and write commands.  Also includes energy terms for ODT and CKE. 
       This register is paused after CH_FRZE_OLTT_CNTR is set and resume counting 
       during PSMI wipe. PSMI handler can clear this register after setting the 
       CH_FORCERW bit in EPMCCTRL_PSMI_QSC_CNTL register. 
       Lock is the inverse of the CH_FORCERW.
     */
  } Bits;
  UINT32 Data;
} PMSUM_MCDDC_CTL_STRUCT;


/* SREF_LL0_MCDDC_CTL_REG supported on:                                       */
/*       SKX_A0 (0x40252670)                                                  */
/*       SKX (0x40252670)                                                     */
/* Register default value:              0x080FFFFF                            */
#define SREF_LL0_MCDDC_CTL_REG 0x0B004670
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Self Refresh controls at Load Line point 0.  Used when PMLink 
 * PowerModeSelect[1][0]='00  
 */
typedef union {
  struct {
    UINT32 sref_idle_timer : 20;
    /* sref_idle_timer - Bits[19:0], RWS, default = 20'b11111111111111111111 
       
                           This field defines the rank idle period that causes 
       self-refresh entrance. This value is used when the 'SREF_EN' field is set. It 
       defines the number of idle cycles (in DCLKs) after the command issue that there 
       should not be any transaction in order to enter self-refresh for DDR4 DIMMs or 
       PM Idle for DDR-T DIMMs. It is programmable 1 to 1M-1 dynamically.  FFFFEh is a 
       reserved value and should not be used in normal operation. 
                           The min setting needs to allow for a refresh, a zqcal, a 
       retry read, and a handfull of cycles for Mesh2Mem to issue a demand scrub write: 
                           TCZQCAL.T_ZQCS + TCRFTP.T_RFC + 100 decimal. In reality, the 
       idle counter should be much larger in order avoid unnecessary SRE+SRX overhead. 
                           This register field can be updated dynamically.
       
                       
     */
    UINT32 opp_sref_en : 1;
    /* opp_sref_en - Bits[20:20], RW, default = 1'b0 
       
                           When set, this bit enables opportunistic Self Refresh entry 
       based on the SREF_IDLE_TIMER expiration.  
                           FORCE_SR and AsyncSR are other sources of SR entry 
       independent of the setting of SREF_EN. 
                           When 0, Opportunistic SR Entry is disabled and SR can only 
       be entered on Force_SR_Entry assertion or AsyncSR. 
       
                           A write to this register must be respected by iMC. If iMC 
       has entered self-refresh opportunistically, it must return to the state 
       specified by this register. PCU however needs to ensure that modifications 
       happen only under legal conditions. For example clocks must be available before 
       asking iMC to return to DLL on and SR exit state. 
       
                       
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[21:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mdll_off_en : 1;
    /* mdll_off_en - Bits[22:22], RW, default = 1'b0 
       
                           When 0 Master DLLs (MDLL) cannot be turned off. When 1 MDLLs 
       can be turned off in Self Refresh.  
                           A write to this register must be respected by IMC. If IMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to MDLL on and SR exit state. 
                       
     */
    UINT32 ibt_off_en : 1;
    /* ibt_off_en - Bits[23:23], RW, default = 1'b0 
       Enable IBT_OFF Register Power Down Mode when set; otherwise, IBT_ON is enabled.
     */
    UINT32 ck_mode : 2;
    /* ck_mode - Bits[25:24], RW, default = 2'b00 
       
                           The field defines how CK and CK# are turned off during Self 
       Refresh: 
       
                           CK_MODE=00, CK_ON: In this mode CK continues to be driven 
       during self-refresh. 
                           CK_MODE=01 is not supported. 
                           CK_MODE=10, CK_PULL_LOW_MODE: after tCKEoff timing delay 
       from SRE CKE de-assertion, IMC wait for tCKoff before dropping CK-ALIGN and 
       CK#-ALIGN (internal signal to DDRIO) to LOW thoughout the self-refresh.  
                           CK_MODE=11 is not supported. 
       
                           CKE tri-state is under separate control in DDRIO. 
                           All other signals (except DDR_RESET#) are tri-stated after 
       tCKEv delay. 
       
                           A write to this register must be respected by iMC. If iMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to DLL on and SR exit state. 
                       
     */
    UINT32 ddrt_idle_cmd : 2;
    /* ddrt_idle_cmd - Bits[27:26], RW, default = 2'b10 
       
                           Bits to send as part of the DDR-T PM Idle command to 
       indicate the specific command to be executed.  Encoding is as per the DDR-T 
       spec. 
                       
     */
    UINT32 reserved : 4;
    /* reserved - Bits[31:28], RW, default = 6'b000000 
       Reserved for future use.
     */
  } Bits;
  UINT32 Data;
} SREF_LL0_MCDDC_CTL_STRUCT;


/* SREF_LL1_MCDDC_CTL_REG supported on:                                       */
/*       SKX_A0 (0x40252674)                                                  */
/*       SKX (0x40252674)                                                     */
/* Register default value:              0x000FFFFF                            */
#define SREF_LL1_MCDDC_CTL_REG 0x0B004674
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Self Refresh controls at Load Line point 1.  Used when PMLink signal 
 * PowerModeSelect[1][0]='01  
 */
typedef union {
  struct {
    UINT32 sref_idle_timer : 20;
    /* sref_idle_timer - Bits[19:0], RWS, default = 20'b11111111111111111111 
       
                           This field defines the rank idle period that causes 
       self-refresh entrance. This value is used when the 'SREF_EN' field is set. It 
       defines the number of idle cycles (in DCLKs) after the command issue that there 
       should not be any transaction in order to enter self-refresh for DDR4 DIMMs or 
       PM Idle for DDR-T DIMMs. It is programmable 1 to 1M-1 dynamically.  FFFFEh is a 
       reserved value and should not be used in normal operation. 
                           The min setting needs to allow for a refresh, a zqcal, a 
       retry read, and a handfull of cycles for Mesh2Mem to issue a demand scrub write: 
                           TCZQCAL.T_ZQCS + TCRFTP.T_RFC + 100 decimal. In reality, the 
       idle counter should be much larger in order avoid unnecessary SRE+SRX overhead. 
                           This register field can be updated dynamically.
       
                       
     */
    UINT32 opp_sref_en : 1;
    /* opp_sref_en - Bits[20:20], RW, default = 1'b0 
       
                           When set, this bit enables opportunistic Self Refresh entry 
       based on the SREF_IDLE_TIMER expiration.  
                           FORCE_SR and AsyncSR are other sources of SR entry 
       independent of the setting of SREF_EN. 
                           When 0, Opportunistic SR Entry is disabled and SR can only 
       be entered on Force_SR_Entry assertion or AsyncSR. 
       
                           A write to this register must be respected by iMC. If iMC 
       has entered self-refresh opportunistically, it must return to the state 
       specified by this register. PCU however needs to ensure that modifications 
       happen only under legal conditions. For example clocks must be available before 
       asking iMC to return to DLL on and SR exit state. 
       
                       
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[21:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mdll_off_en : 1;
    /* mdll_off_en - Bits[22:22], RW, default = 1'b0 
       
                           When 0 Master DLLs (MDLL) cannot be turned off. When 1 MDLLs 
       can be turned off in Self Refresh.  
                           A write to this register must be respected by IMC. If IMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to MDLL on and SR exit state. 
                       
     */
    UINT32 ibt_off_en : 1;
    /* ibt_off_en - Bits[23:23], RW, default = 1'b0 
       Enable IBT_OFF Register Power Down Mode when set; otherwise, IBT_ON is enabled.
     */
    UINT32 ck_mode : 2;
    /* ck_mode - Bits[25:24], RW, default = 2'b00 
       
                           The field defines how CK and CK# are turned off during Self 
       Refresh: 
       
                           CK_MODE=00, CK_ON: In this mode CK continues to be driven 
       during self-refresh. 
                           CK_MODE=01 is not supported 
                           CK_MODE=10, CK_PULL_LOW_MODE: after tCKEoff timing delay 
       from SRE CKE de-assertion, IMC wait for tCKoff before dropping CK-ALIGN and 
       CK#-ALIGN (internal signal to DDRIO) to LOW thoughout the self-refresh.  
                           CK_MODE=11 is not supported. 
       
                           CKE tri-state is under separate control in DDRIO. 
                           All other signals (except DDR_RESET#) are tri-stated after 
       tCKEv delay. 
       
                           A write to this register must be respected by iMC. If iMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to DLL on and SR exit state. 
                       
     */
    UINT32 ddrt_idle_cmd : 2;
    /* ddrt_idle_cmd - Bits[27:26], RW, default = 2'b00 
       
                           Bits to send as part of the DDR-T PM Idle command to 
       indicate the specific command to be executed.  Encoding is as per the DDR-T 
       spec. 
                       
     */
    UINT32 reserved : 4;
    /* reserved - Bits[31:28], RW, default = 6'b000000 
       Reserved for future use.
     */
  } Bits;
  UINT32 Data;
} SREF_LL1_MCDDC_CTL_STRUCT;


/* SREF_LL2_MCDDC_CTL_REG supported on:                                       */
/*       SKX_A0 (0x40252678)                                                  */
/*       SKX (0x40252678)                                                     */
/* Register default value:              0x080FFFFF                            */
#define SREF_LL2_MCDDC_CTL_REG 0x0B004678
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Self Refresh controls at Load Line point 2.  Used when PMLink 
 * PowerModeSelect[1][0]='10  
 */
typedef union {
  struct {
    UINT32 sref_idle_timer : 20;
    /* sref_idle_timer - Bits[19:0], RWS, default = 20'b11111111111111111111 
       
                           This field defines the rank idle period that causes 
       self-refresh entrance. This value is used when the 'SREF_EN' field is set. It 
       defines the number of idle cycles (in DCLKs) after the command issue that there 
       should not be any transaction in order to enter self-refresh for DDR4 DIMMs or 
       PM Idle for DDR-T DIMMs. It is programmable 1 to 1M-1 dynamically.  FFFFEh is a 
       reserved value and should not be used in normal operation. 
                           The min setting needs to allow for a refresh, a zqcal, a 
       retry read, and a handfull of cycles for Mesh2Mem to issue a demand scrub write: 
                           TCZQCAL.T_ZQCS + TCRFTP.T_RFC + 100 decimal. In reality, the 
       idle counter should be much larger in order avoid unnecessary SRE+SRX overhead. 
                           This register field can be updated dynamically.
       
                       
     */
    UINT32 opp_sref_en : 1;
    /* opp_sref_en - Bits[20:20], RW, default = 1'b0 
       
                           When set, this bit enables opportunistic Self Refresh entry 
       based on the SREF_IDLE_TIMER expiration.  
                           FORCE_SR and AsyncSR are other sources of SR entry 
       independent of the setting of SREF_EN. 
                           When 0, Opportunistic SR Entry is disabled and SR can only 
       be entered on Force_SR_Entry assertion or AsyncSR. 
       
                           A write to this register must be respected by iMC. If iMC 
       has entered self-refresh opportunistically, it must return to the state 
       specified by this register. PCU however needs to ensure that modifications 
       happen only under legal conditions. For example clocks must be available before 
       asking iMC to return to DLL on and SR exit state. 
       
                       
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[21:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mdll_off_en : 1;
    /* mdll_off_en - Bits[22:22], RW, default = 1'b0 
       
                           When 0 Master DLLs (MDLL) cannot be turned off. When 1 MDLLs 
       can be turned off in Self Refresh.  
                           A write to this register must be respected by IMC. If IMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to MDLL on and SR exit state. 
                       
     */
    UINT32 ibt_off_en : 1;
    /* ibt_off_en - Bits[23:23], RW, default = 1'b0 
       Enable IBT_OFF Register Power Down Mode when set; otherwise, IBT_ON is enabled.
     */
    UINT32 ck_mode : 2;
    /* ck_mode - Bits[25:24], RW, default = 2'b00 
       
                           The field defines how CK and CK# are turned off during Self 
       Refresh: 
       
                           CK_MODE=00, CK_ON: In this mode CK continues to be driven 
       during self-refresh. 
                           CK_MODE=01 is not supported 
                           CK_MODE=10, CK_PULL_LOW_MODE: after tCKEoff timing delay 
       from SRE CKE de-assertion, IMC wait for tCKoff before dropping CK-ALIGN and 
       CK#-ALIGN (internal signal to DDRIO) to LOW thoughout the self-refresh.  
                           CK_MODE=11 is not supported. 
       
                           CKE tri-state is under separate control in DDRIO. 
                           All other signals (except DDR_RESET#) are tri-stated after 
       tCKEv delay. 
       
                           A write to this register must be respected by iMC. If iMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to DLL on and SR exit state. 
                       
     */
    UINT32 ddrt_idle_cmd : 2;
    /* ddrt_idle_cmd - Bits[27:26], RW, default = 2'b10 
       
                           Bits to send as part of the DDR-T PM Idle command to 
       indicate the specific command to be executed.  Encoding is as per the DDR-T 
       spec. 
                       
     */
    UINT32 reserved : 4;
    /* reserved - Bits[31:28], RW, default = 6'b000000 
       Reserved for future use.
     */
  } Bits;
  UINT32 Data;
} SREF_LL2_MCDDC_CTL_STRUCT;


/* SREF_LL3_MCDDC_CTL_REG supported on:                                       */
/*       SKX_A0 (0x4025267C)                                                  */
/*       SKX (0x4025267C)                                                     */
/* Register default value:              0x080FFFFF                            */
#define SREF_LL3_MCDDC_CTL_REG 0x0B00467C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Self Refresh controls at Load Line point 3.  Used when PMLink 
 * PowerModeSelect[1][0]='11  
 */
typedef union {
  struct {
    UINT32 sref_idle_timer : 20;
    /* sref_idle_timer - Bits[19:0], RWS, default = 20'b11111111111111111111 
       
                           This field defines the rank idle period that causes 
       self-refresh entrance. This value is used when the 'SREF_EN' field is set. It 
       defines the number of idle cycles (in DCLKs) after the command issue that there 
       should not be any transaction in order to enter self-refresh for DDR4 DIMMs or 
       PM Idle for DDR-T DIMMs. It is programmable 1 to 1M-1 dynamically.  FFFFEh is a 
       reserved value and should not be used in normal operation. 
                           The min setting needs to allow for a refresh, a zqcal, a 
       retry read, and a handfull of cycles for Mesh2Mem to issue a demand scrub write: 
                           TCZQCAL.T_ZQCS + TCRFTP.T_RFC + 100 decimal. In reality, the 
       idle counter should be much larger in order avoid unnecessary SRE+SRX overhead. 
                           This register field can be updated dynamically.
       
                       
     */
    UINT32 opp_sref_en : 1;
    /* opp_sref_en - Bits[20:20], RW, default = 1'b0 
       
                           When set, this bit enables opportunistic Self Refresh entry 
       based on the SREF_IDLE_TIMER expiration.  
                           FORCE_SR and AsyncSR are other sources of SR entry 
       independent of the setting of SREF_EN. 
                           When 0, Opportunistic SR Entry is disabled and SR can only 
       be entered on Force_SR_Entry assertion or AsyncSR. 
       
                           A write to this register must be respected by iMC. If iMC 
       has entered self-refresh opportunistically, it must return to the state 
       specified by this register. PCU however needs to ensure that modifications 
       happen only under legal conditions. For example clocks must be available before 
       asking iMC to return to DLL on and SR exit state. 
       
                       
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[21:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mdll_off_en : 1;
    /* mdll_off_en - Bits[22:22], RW, default = 1'b0 
       
                           When 0 Master DLLs (MDLL) cannot be turned off. When 1 MDLLs 
       can be turned off in Self Refresh.  
                           A write to this register must be respected by IMC. If IMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to MDLL on and SR exit state. 
                       
     */
    UINT32 ibt_off_en : 1;
    /* ibt_off_en - Bits[23:23], RW, default = 1'b0 
       Enable IBT_OFF Register Power Down Mode when set; otherwise, IBT_ON is enabled.
     */
    UINT32 ck_mode : 2;
    /* ck_mode - Bits[25:24], RW, default = 2'b00 
       
                           The field defines how CK and CK# are turned off during Self 
       Refresh: 
       
                           CK_MODE=00, CK_ON: In this mode CK continues to be driven 
       during self-refresh. 
                           CK_MODE=01 is not supported 
                           CK_MODE=10, CK_PULL_LOW_MODE: after tCKEoff timing delay 
       from SRE CKE de-assertion, IMC wait for tCKoff before dropping CK-ALIGN and 
       CK#-ALIGN (internal signal to DDRIO) to LOW thoughout the self-refresh.  
                           CK_MODE=11 is not supported. 
       
                           CKE tri-state is under separate control in DDRIO. 
                           All other signals (except DDR_RESET#) are tri-stated after 
       tCKEv delay. 
       
                           A write to this register must be respected by iMC. If iMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to DLL on and SR exit state. 
                       
     */
    UINT32 ddrt_idle_cmd : 2;
    /* ddrt_idle_cmd - Bits[27:26], RW, default = 2'b10 
       
                           Bits to send as part of the DDR-T PM Idle command to 
       indicate the specific command to be executed.  Encoding is as per the DDR-T 
       spec. 
                       
     */
    UINT32 reserved : 4;
    /* reserved - Bits[31:28], RW, default = 6'b000000 
       Reserved for future use.
     */
  } Bits;
  UINT32 Data;
} SREF_LL3_MCDDC_CTL_STRUCT;


/* CKE_LL0_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x402526A0)                                                  */
/*       SKX (0x402526A0)                                                     */
/* Register default value:              0x10000080                            */
#define CKE_LL0_MCDDC_CTL_REG 0x0B0046A0
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Control for CKE (DRAM powerdown modes) at Load Line point 0.  Used when PMLink 
 * signal PowerModeSelect[1][0]='00  
 */
typedef union {
  struct {
    UINT32 cke_idle_timer : 8;
    /* cke_idle_timer - Bits[7:0], RW, default = 8'b10000000 
       This defines the rank idle period that causes CKE power-down entrance. The 
       number of idle cycles (in DCLKs) are based from command CS assertion. It is 
       important to program this parameter to be greater than roundtrip latency 
       parameter in order to avoid the CKE de-assertion sooner than data return. 
                               This register field can be updated dynamically.
                           
     */
    UINT32 apd_en : 1;
    /* apd_en - Bits[8:8], RW, default = 1'b0 
       
                               CKE Active Power Down Mode for DDR4 DIMMs:
                               When 0, APD is disabled
                               When 1, APD is enabled
                               This register field can be updated dynamically.
                           
     */
    UINT32 ppd_en : 1;
    /* ppd_en - Bits[9:9], RW, default = 1'b0 
       
                               CKE Precharge Power Down (PPD):
                               When 0, PPD is disabled
                               When 1, PPD is enabled
                               This register field can be updated dynamically.
                               apd_en should also also be set to 1 if ppd_en = 1 with 
       3DS DIMMs. 
                           
     */
    UINT32 rsvd_10 : 14;
    /* rsvd_10 - Bits[23:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrt_cke_en : 1;
    /* ddrt_cke_en - Bits[24:24], RW, default = 1'b0 
       
                               CKE Active Power Down Mode for DDR-T DIMMs:
                               When 0, APD is disabled
                               When 1, APD is enabled
                               This register field can be updated dynamically.
                           
     */
    UINT32 rsvd_25 : 3;
    /* rsvd_25 - Bits[27:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reserved : 2;
    /* reserved - Bits[29:28], RW, default = 2'b01 
        Reserved for future use. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CKE_LL0_MCDDC_CTL_STRUCT;


/* CKE_LL1_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x402526A4)                                                  */
/*       SKX (0x402526A4)                                                     */
/* Register default value:              0x10000080                            */
#define CKE_LL1_MCDDC_CTL_REG 0x0B0046A4
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Control for CKE (DRAM powerdown modes) at Load Line point 1.  Used when PMLink 
 * signal PowerModeSelect[1][0]='01  
 */
typedef union {
  struct {
    UINT32 cke_idle_timer : 8;
    /* cke_idle_timer - Bits[7:0], RW, default = 8'b10000000 
       This defines the rank idle period that causes CKE power-down entrance. The 
       number of idle cycles (in DCLKs) are based from command CS assertion. It is 
       important to program this parameter to be greater than roundtrip latency 
       parameter in order to avoid the CKE de-assertion sooner than data return. 
                               This register field can be updated dynamically.
                           
     */
    UINT32 apd_en : 1;
    /* apd_en - Bits[8:8], RW, default = 1'b0 
       
                               CKE Active Power Down Mode for DDR4 DIMMs:
                               When 0, APD is disabled
                               When 1, APD is enabled
                               This register field can be updated dynamically.
                           
     */
    UINT32 ppd_en : 1;
    /* ppd_en - Bits[9:9], RW, default = 1'b0 
       
                               CKE Precharge Power Down (PPD):
                               When 0, PPD is disabled
                               When 1, PPD is enabled
                               This register field can be updated dynamically.
                               apd_en should also also be set to 1 if ppd_en = 1 with 
       3DS DIMMs. 
                           
     */
    UINT32 rsvd_10 : 14;
    /* rsvd_10 - Bits[23:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrt_cke_en : 1;
    /* ddrt_cke_en - Bits[24:24], RW, default = 1'b0 
       
                               CKE Active Power Down Mode for DDR-T DIMMs:
                               When 0, APD is disabled
                               When 1, APD is enabled
                               This register field can be updated dynamically.
                           
     */
    UINT32 rsvd_25 : 3;
    /* rsvd_25 - Bits[27:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reserved : 2;
    /* reserved - Bits[29:28], RW, default = 2'b01 
        Reserved for future use. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CKE_LL1_MCDDC_CTL_STRUCT;


/* CKE_LL2_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x402526A8)                                                  */
/*       SKX (0x402526A8)                                                     */
/* Register default value:              0x10000080                            */
#define CKE_LL2_MCDDC_CTL_REG 0x0B0046A8
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Control for CKE (DRAM powerdown modes) at Load Line point 2.  Used when PMLink 
 * signal PowerModeSelect[1][0]='10  
 */
typedef union {
  struct {
    UINT32 cke_idle_timer : 8;
    /* cke_idle_timer - Bits[7:0], RW, default = 8'b10000000 
       This defines the rank idle period that causes CKE power-down entrance. The 
       number of idle cycles (in DCLKs) are based from command CS assertion. It is 
       important to program this parameter to be greater than roundtrip latency 
       parameter in order to avoid the CKE de-assertion sooner than data return. 
                               This register field can be updated dynamically.
                           
     */
    UINT32 apd_en : 1;
    /* apd_en - Bits[8:8], RW, default = 1'b0 
       
                               CKE Active Power Down Mode for DDR4 DIMMs:
                               When 0, APD is disabled
                               When 1, APD is enabled
                               This register field can be updated dynamically.
                           
     */
    UINT32 ppd_en : 1;
    /* ppd_en - Bits[9:9], RW, default = 1'b0 
       
                               CKE Precharge Power Down (PPD):
                               When 0, PPD is disabled
                               When 1, PPD is enabled
                               This register field can be updated dynamically.
                               apd_en should also also be set to 1 if ppd_en = 1 with 
       3DS DIMMs. 
                           
     */
    UINT32 rsvd_10 : 14;
    /* rsvd_10 - Bits[23:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrt_cke_en : 1;
    /* ddrt_cke_en - Bits[24:24], RW, default = 1'b0 
       
                               CKE Active Power Down Mode for DDR-T DIMMs:
                               When 0, APD is disabled
                               When 1, APD is enabled
                               This register field can be updated dynamically.
                           
     */
    UINT32 rsvd_25 : 3;
    /* rsvd_25 - Bits[27:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reserved : 2;
    /* reserved - Bits[29:28], RW, default = 2'b01 
        Reserved for future use. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CKE_LL2_MCDDC_CTL_STRUCT;


/* CKE_LL3_MCDDC_CTL_REG supported on:                                        */
/*       SKX_A0 (0x402526AC)                                                  */
/*       SKX (0x402526AC)                                                     */
/* Register default value:              0x10000080                            */
#define CKE_LL3_MCDDC_CTL_REG 0x0B0046AC
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Control for CKE (DRAM powerdown modes) at Load Line point 3.  Used when PMLink 
 * signal PowerModeSelect[1][0]='11  
 */
typedef union {
  struct {
    UINT32 cke_idle_timer : 8;
    /* cke_idle_timer - Bits[7:0], RW, default = 8'b10000000 
       This defines the rank idle period that causes CKE power-down entrance. The 
       number of idle cycles (in DCLKs) are based from command CS assertion. It is 
       important to program this parameter to be greater than roundtrip latency 
       parameter in order to avoid the CKE de-assertion sooner than data return. 
                               This register field can be updated dynamically.
                           
     */
    UINT32 apd_en : 1;
    /* apd_en - Bits[8:8], RW, default = 1'b0 
       
                               CKE Active Power Down Mode for DDR4 DIMMs:
                               When 0, APD is disabled
                               When 1, APD is enabled
                               This register field can be updated dynamically.
                           
     */
    UINT32 ppd_en : 1;
    /* ppd_en - Bits[9:9], RW, default = 1'b0 
       
                               CKE Precharge Power Down (PPD):
                               When 0, PPD is disabled
                               When 1, PPD is enabled
                               This register field can be updated dynamically.
                               apd_en should also also be set to 1 if ppd_en = 1 with 
       3DS DIMMs. 
                           
     */
    UINT32 rsvd_10 : 14;
    /* rsvd_10 - Bits[23:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrt_cke_en : 1;
    /* ddrt_cke_en - Bits[24:24], RW, default = 1'b0 
       
                               CKE Active Power Down Mode for DDR-T DIMMs:
                               When 0, APD is disabled
                               When 1, APD is enabled
                               This register field can be updated dynamically.
                           
     */
    UINT32 rsvd_25 : 3;
    /* rsvd_25 - Bits[27:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reserved : 2;
    /* reserved - Bits[29:28], RW, default = 2'b01 
        Reserved for future use. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CKE_LL3_MCDDC_CTL_STRUCT;


/* MCMNT_CHKN_BIT_MCDDC_CTL_REG supported on:                                 */
/*       SKX_A0 (0x40252700)                                                  */
/*       SKX (0x40252700)                                                     */
/* Register default value:              0x000000C2                            */
#define MCMNT_CHKN_BIT_MCDDC_CTL_REG 0x0B004700
/* Struct format extracted from XML file SKX_A0\2.10.2.CFG.xml.
 * generated by critter 20_0_0x700
 */
typedef union {
  struct {
    UINT32 rdimm_cw_bdcast : 1;
    /* rdimm_cw_bdcast - Bits[0:0], RW_LB, default = 1'b0 
       RDIMM_CW_BDCAST
       When rdimm_cw_bdcast is set, dis_srl_rdimm_cw_issue must be ONE.
       When rdimm_cw_bdcast is zero, software can decided to stagger the CW issue by 
       tMRD (for control word issue, dis_srl_rdimm_cw_issue = 0), or allow CW to be 
       issue one after another one (dis_srl_rdimm_cw_issue = 1). 
       dis_srl_rdimm_cw_issue is more for DIMM to DIMM CW issue control, where 
       rdimm_cw_bdcast allows the whole channel to issue CW at the same time. 
     */
    UINT32 dis_ck_gate_mnts : 1;
    /* dis_ck_gate_mnts - Bits[1:1], RW_LB, default = 1'b1 
       Disable Clock Gating at MNTS.
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[2:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dis_opp_ref : 1;
    /* dis_opp_ref - Bits[3:3], RW_LB, default = 1'b0 
       When this bit is set, MC will never issue a refresh unless either HP or Panic 
       refresh is triggered. For example, if we only have panic refresh enabled, if IMC 
       has reached panic refresh watermark and issue one refresh to get out of panic. 
       When the next tREFI expires, the IMC will enter panic refresh again. The pattern 
       will repeat every tREFI and remain at constant panic mode. Therefore, this is 
       not very useful mode for normal usage. Setting this bit is recommended only for 
       bug workaround in opportunistic refresh flows. 
       Setting this bit will not affect AsyncSR or ForceSR since these signals are 
       OR'ed with opp_ref_enabled in RTL so that we will be able to drain all refreshes 
       and enter the power savings mode. 
       Limitations: If this bit is set, we will never be able to enter self refresh via 
       idle timer since the refreshes are not drained. 
     */
    UINT32 block_ckev_timer_srexit : 1;
    /* block_ckev_timer_srexit - Bits[4:4], RW_LB, default = 1'b0 
       
               Set this bit to 1 to disable turning off the tCKEV timer
               during SR exit. 
               
     */
    UINT32 en_clkgating_reffsm_srisd : 1;
    /* en_clkgating_reffsm_srisd - Bits[5:5], RW_LB, default = 1'b0 
       
                Default behavior is to enable clock gating of the refresh FSM
                only when it is in IDLE state. If this bit is set then
                refresh FSM will be clock gated when it is in SR issued state. 
               
     */
    UINT32 zqlsrl : 1;
    /* zqlsrl - Bits[6:6], RW_LB, default = 1'b1 
       Enabling the memory controller to issue two auto-refreshes upon exiting 
       self-refresh. It also serialize the ZQCx between ranks upon self-refresh exit 
       when set. 
     */
    UINT32 dis_2zq_qr_2cs : 1;
    /* dis_2zq_qr_2cs - Bits[7:7], RW_LB, default = 1'b1 
       This bit should be cleared to zero Only for QR lrdimm used in encoded_chip 
       select mode in order to issue ZQCS/L to all 4 chip selects. Otherwise, in encode 
       chip select mode, only chip_select[0] (cid 0 and 1) will get ZQCS/L. 
     */
    UINT32 dis_sdll_off : 1;
    /* dis_sdll_off - Bits[8:8], RW_LB, default = 1'b0 
       Disable slave dll off
     */
    UINT32 dis_zqcal : 1;
    /* dis_zqcal - Bits[9:9], RW_LB, default = 1'b0 
       Disable ZQCAL.
       Note: If DIS_ZQCAL=1 and DIS_SRX_EX_ZQ=1, IMC will not issue any ZQCx.
     */
    UINT32 long_zq : 1;
    /* long_zq - Bits[10:10], RW_LB, default = 1'b0 
       1: IMC issues ZQCL instead of ZQCS in the subsequent ZQ calibration after the 
       initial ZQ 
       0: IMC issues ZQCS instead of ZQCL in the subsequent ZQ calibration after the 
       initial ZQ 
     */
    UINT32 dis_srx_ex_zq : 1;
    /* dis_srx_ex_zq - Bits[11:11], RW_LB, default = 1'b0 
       Disable srx ex zq
       disable ZQ calibration upon exiting self-refresh if set
     */
    UINT32 fst_zq : 1;
    /* fst_zq - Bits[12:12], RW_LB, default = 1'b0 
       enable fast zq
       tczqcal_zqcsperiod is divided by 16 when fst_zq is set.
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW_LB, default = 2'b0 
       spare
     */
    UINT32 dis_srl_rdimm_cw_issue : 1;
    /* dis_srl_rdimm_cw_issue - Bits[15:15], RW_LB, default = 1'b0 
       DIS_SRL_RDIMM_CW_ISSUE
       When rdimm_cw_bdcast is set, dis_srl_rdimm_cw_issue must be zero.
       When rdimm_cw_bdcast is zero, software can decided to stagger the CW issue by 
       tMRD (for control word issue, dis_srl_rdimm_cw_issue = 1), or allow CW to be 
       issue one after another one (dis_srl_rdimm_cw_issue = 0). 
       dis_srl_rdimm_cw_issue is more for DIMM to DIMM CW issue control, where 
       rdimm_cw_bdcast allows the whole channel to issue CW at the same time. 
     */
    UINT32 dis_srl_mrs_issue : 1;
    /* dis_srl_mrs_issue - Bits[16:16], RW_LB, default = 1'b0 
       Do not set this defeature bit. MRS concurrent broadcast function is no longer 
       supported. 
     */
    UINT32 dis_cke_off_durg_thr : 1;
    /* dis_cke_off_durg_thr - Bits[17:17], RW_LB, default = 1'b0 
       Disable turnoff CKE during throttle
       Important Note: Setting DIS_CKE_OFF_DURG_THR=1will cause ACT to be issued during 
       throttling which will cause additional power consumption. 
     */
    UINT32 exit_cke_use_hp_refresh : 1;
    /* exit_cke_use_hp_refresh - Bits[18:18], RW_LB, default = 1'b0 
       0b: When IMC is in CKE IBT OFF mode or Slow CKE mode, only Panic Refresh will be 
       able to bring MC out of those modes. 
       1b: By setting this bit, MC will come out of those modes when High Priority 
       Refresh is due; however, it is important that the CHN_TEMP_CFG.THRT_ALLOW_ISOCH 
       must not be set to zero; otherw-lbise, panic refresh will be dropped!!! 
     */
    UINT32 stagger_ref_en : 1;
    /* stagger_ref_en - Bits[19:19], RW_LB, default = 1'b0 
       Stagger Refresh Enable, When set, the T_STAGGER_REF timing parameter becomes 
       effective to stagger the REF command 
     */
    UINT32 dis_wpq_sref_exit : 1;
    /* dis_wpq_sref_exit - Bits[20:20], RW_LB, default = 1'b0 
       Disable self-refresh exit due to WPQ going non-empty.
     */
    UINT32 defeature_12 : 1;
    /* defeature_12 - Bits[21:21], RW_LB, default = 1'b0 
       Reserved Unused, the IOSAV subseq repeat infinite mode bit is now in an IOSAV 
       register 
     */
    UINT32 frc_mdll_off : 1;
    /* frc_mdll_off - Bits[22:22], RW_LB, default = 1'b0 
       Force DDRIO master DLL off, and de-assert MCActive signal to the MC PMA.
     */
    UINT32 frc_mdll_on : 1;
    /* frc_mdll_on - Bits[23:23], RW_LB, default = 1'b0 
       Force DDRIO master DLL on, and assert MCActive signal to the MC PMA. Takes 
       priority over frc_mdll_off. 
     */
    UINT32 dis_ppds : 1;
    /* dis_ppds - Bits[24:24], RW_LB, default = 1'b0 
       Disables IBTOff and PPDS entry.
     */
    UINT32 dis_ppds_idle_cntr : 1;
    /* dis_ppds_idle_cntr - Bits[25:25], RW_LB, default = 1'b0 
       Disables channel idle counter for IBTOff and PPDS entry.
     */
    UINT32 defeature_13 : 1;
    /* defeature_13 - Bits[26:26], RW_LB, default = 1'b0 
       Reserved Unused
     */
    UINT32 defeature_14 : 1;
    /* defeature_14 - Bits[27:27], RW_LB, default = 1'b0 
       Reserved Unused
     */
    UINT32 defeature_15 : 1;
    /* defeature_15 - Bits[28:28], RW_LB, default = 1'b0 
       Reserved Unused
     */
    UINT32 dis_ch_thrt_done : 1;
    /* dis_ch_thrt_done - Bits[29:29], RW_LB, default = 1'b0  */
    UINT32 block_thr_1 : 1;
    /* block_thr_1 - Bits[30:30], RW_LB, default = 1'b0 
       Blocks throttle signal for panic refresh
     */
    UINT32 block_thr_2 : 1;
    /* block_thr_2 - Bits[31:31], RW_LB, default = 1'b0 
       Blocks throttle signal and throttle signal for isoch panic refresh
     */
  } Bits;
  UINT32 Data;
} MCMNT_CHKN_BIT_MCDDC_CTL_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x700
 */
typedef union {
  struct {
    UINT32 rdimm_cw_bdcast : 1;
    /* rdimm_cw_bdcast - Bits[0:0], RW_LB, default = 1'b0 
       RDIMM_CW_BDCAST
       When rdimm_cw_bdcast is set, dis_srl_rdimm_cw_issue must be ONE.
       When rdimm_cw_bdcast is zero, software can decided to stagger the CW issue by 
       tMRD (for control word issue, dis_srl_rdimm_cw_issue = 0), or allow CW to be 
       issue one after another one (dis_srl_rdimm_cw_issue = 1). 
       dis_srl_rdimm_cw_issue is more for DIMM to DIMM CW issue control, where 
       rdimm_cw_bdcast allows the whole channel to issue CW at the same time. 
     */
    UINT32 dis_ck_gate_mnts : 1;
    /* dis_ck_gate_mnts - Bits[1:1], RW_LB, default = 1'b1 
       Disable Clock Gating at MNTS.
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[2:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dis_opp_ref : 1;
    /* dis_opp_ref - Bits[3:3], RW_LB, default = 1'b0 
       When this bit is set, MC will never issue a refresh unless either HP or Panic 
       refresh is triggered. For example, if we only have panic refresh enabled, if IMC 
       has reached panic refresh watermark and issue one refresh to get out of panic. 
       When the next tREFI expires, the IMC will enter panic refresh again. The pattern 
       will repeat every tREFI and remain at constant panic mode. Therefore, this is 
       not very useful mode for normal usage. Setting this bit is recommended only for 
       bug workaround in opportunistic refresh flows. 
       Setting this bit will not affect AsyncSR or ForceSR since these signals are 
       OR'ed with opp_ref_enabled in RTL so that we will be able to drain all refreshes 
       and enter the power savings mode. 
       Limitations: If this bit is set, we will never be able to enter self refresh via 
       idle timer since the refreshes are not drained. 
     */
    UINT32 block_ckev_timer_srexit : 1;
    /* block_ckev_timer_srexit - Bits[4:4], RW_LB, default = 1'b0 
       
               Set this bit to 1 to disable turning off the tCKEV timer
               during SR exit. 
               
     */
    UINT32 en_clkgating_reffsm_srisd : 1;
    /* en_clkgating_reffsm_srisd - Bits[5:5], RW_LB, default = 1'b0 
       
                Default behavior is to enable clock gating of the refresh FSM
                only when it is in IDLE state. If this bit is set then
                refresh FSM will be clock gated when it is in SR issued state. 
               
     */
    UINT32 zqlsrl : 1;
    /* zqlsrl - Bits[6:6], RW_LB, default = 1'b1 
       Enabling the memory controller to issue two auto-refreshes upon exiting 
       self-refresh. It also serialize the ZQCx between ranks upon self-refresh exit 
       when set. 
     */
    UINT32 dis_2zq_qr_2cs : 1;
    /* dis_2zq_qr_2cs - Bits[7:7], RW_LB, default = 1'b1 
       This bit should be cleared to zero Only for QR lrdimm used in encoded_chip 
       select mode in order to issue ZQCS/L to all 4 chip selects. Otherwise, in encode 
       chip select mode, only chip_select[0] (cid 0 and 1) will get ZQCS/L. 
     */
    UINT32 dis_sdll_off : 1;
    /* dis_sdll_off - Bits[8:8], RW_LB, default = 1'b0 
       Disable slave dll off
     */
    UINT32 dis_zqcal : 1;
    /* dis_zqcal - Bits[9:9], RW_LB, default = 1'b0 
       Disable ZQCAL.
       Note: If DIS_ZQCAL=1 and DIS_SRX_EX_ZQ=1, IMC will not issue any ZQCx.
     */
    UINT32 long_zq : 1;
    /* long_zq - Bits[10:10], RW_LB, default = 1'b0 
       1: IMC issues ZQCL instead of ZQCS in the subsequent ZQ calibration after the 
       initial ZQ 
       0: IMC issues ZQCS instead of ZQCL in the subsequent ZQ calibration after the 
       initial ZQ 
     */
    UINT32 dis_srx_ex_zq : 1;
    /* dis_srx_ex_zq - Bits[11:11], RW_LB, default = 1'b0 
       Disable srx ex zq
       disable ZQ calibration upon exiting self-refresh if set
     */
    UINT32 fst_zq : 1;
    /* fst_zq - Bits[12:12], RW_LB, default = 1'b0 
       enable fast zq
       tczqcal_zqcsperiod is divided by 16 when fst_zq is set.
     */
    UINT32 en_ddrt_pc6_clkstp_fsm : 1;
    /* en_ddrt_pc6_clkstp_fsm - Bits[13:13], RW_LB, default = 1'b0 
       
               This bit must be set if the MC has any DDRT DIMM populated. It
               is needed for the correct operation of DDRT PC6 Clock Stop.
               This bit must be set to 0 for a MC in a DDR4 only config. 
               
     */
    UINT32 pc6_clkstp_fsm_block_pma_ack : 1;
    /* pc6_clkstp_fsm_block_pma_ack - Bits[14:14], RW_LB, default = 1'b0 
       
               Chicken bit to set to 1 if DDRT clkstop FSM should 
               control ACK to PMA during clock stop
               
     */
    UINT32 dis_srl_rdimm_cw_issue : 1;
    /* dis_srl_rdimm_cw_issue - Bits[15:15], RW_LB, default = 1'b0 
       DIS_SRL_RDIMM_CW_ISSUE
       When rdimm_cw_bdcast is set, dis_srl_rdimm_cw_issue must be zero.
       When rdimm_cw_bdcast is zero, software can decided to stagger the CW issue by 
       tMRD (for control word issue, dis_srl_rdimm_cw_issue = 1), or allow CW to be 
       issue one after another one (dis_srl_rdimm_cw_issue = 0). 
       dis_srl_rdimm_cw_issue is more for DIMM to DIMM CW issue control, where 
       rdimm_cw_bdcast allows the whole channel to issue CW at the same time. 
     */
    UINT32 dis_srl_mrs_issue : 1;
    /* dis_srl_mrs_issue - Bits[16:16], RW_LB, default = 1'b0 
       Do not set this defeature bit. MRS concurrent broadcast function is no longer 
       supported. 
     */
    UINT32 dis_cke_off_durg_thr : 1;
    /* dis_cke_off_durg_thr - Bits[17:17], RW_LB, default = 1'b0 
       Disable turnoff CKE during throttle
       Important Note: Setting DIS_CKE_OFF_DURG_THR=1will cause ACT to be issued during 
       throttling which will cause additional power consumption. 
     */
    UINT32 exit_cke_use_hp_refresh : 1;
    /* exit_cke_use_hp_refresh - Bits[18:18], RW_LB, default = 1'b0 
       0b: When IMC is in CKE IBT OFF mode or Slow CKE mode, only Panic Refresh will be 
       able to bring MC out of those modes. 
       1b: By setting this bit, MC will come out of those modes when High Priority 
       Refresh is due; however, it is important that the CHN_TEMP_CFG.THRT_ALLOW_ISOCH 
       must not be set to zero; otherw-lbise, panic refresh will be dropped!!! 
     */
    UINT32 stagger_ref_en : 1;
    /* stagger_ref_en - Bits[19:19], RW_LB, default = 1'b0 
       Stagger Refresh Enable, When set, the T_STAGGER_REF timing parameter becomes 
       effective to stagger the REF command 
     */
    UINT32 dis_wpq_sref_exit : 1;
    /* dis_wpq_sref_exit - Bits[20:20], RW_LB, default = 1'b0 
       Disable self-refresh exit due to WPQ going non-empty.
     */
    UINT32 defeature_12 : 1;
    /* defeature_12 - Bits[21:21], RW_LB, default = 1'b0 
       Reserved Unused, the IOSAV subseq repeat infinite mode bit is now in an IOSAV 
       register 
     */
    UINT32 frc_mdll_off : 1;
    /* frc_mdll_off - Bits[22:22], RW_LB, default = 1'b0 
       Force DDRIO master DLL off, and de-assert MCActive signal to the MC PMA.
     */
    UINT32 frc_mdll_on : 1;
    /* frc_mdll_on - Bits[23:23], RW_LB, default = 1'b0 
       Force DDRIO master DLL on, and assert MCActive signal to the MC PMA. Takes 
       priority over frc_mdll_off. 
     */
    UINT32 dis_ppds : 1;
    /* dis_ppds - Bits[24:24], RW_LB, default = 1'b0 
       Disables IBTOff and PPDS entry.
     */
    UINT32 dis_ppds_idle_cntr : 1;
    /* dis_ppds_idle_cntr - Bits[25:25], RW_LB, default = 1'b0 
       Disables channel idle counter for IBTOff and PPDS entry.
     */
    UINT32 defeature_13 : 1;
    /* defeature_13 - Bits[26:26], RW_LB, default = 1'b0 
       Reserved Unused
     */
    UINT32 defeature_14 : 1;
    /* defeature_14 - Bits[27:27], RW_LB, default = 1'b0 
       Reserved Unused
     */
    UINT32 defeature_15 : 1;
    /* defeature_15 - Bits[28:28], RW_LB, default = 1'b0 
       Disable bug fix for b305799
     */
    UINT32 dis_ch_thrt_done : 1;
    /* dis_ch_thrt_done - Bits[29:29], RW_LB, default = 1'b0  */
    UINT32 block_thr_1 : 1;
    /* block_thr_1 - Bits[30:30], RW_LB, default = 1'b0 
       Blocks throttle signal for panic refresh
     */
    UINT32 block_thr_2 : 1;
    /* block_thr_2 - Bits[31:31], RW_LB, default = 1'b0 
       Blocks throttle signal and throttle signal for isoch panic refresh
     */
  } Bits;
  UINT32 Data;
} MCMNT_CHKN_BIT_MCDDC_CTL_STRUCT;



/* MCSCHED_CHKN_BIT_MCDDC_CTL_REG supported on:                               */
/*       SKX_A0 (0x40252704)                                                  */
/*       SKX (0x40252704)                                                     */
/* Register default value:              0x00000000                            */
#define MCSCHED_CHKN_BIT_MCDDC_CTL_REG 0x0B004704
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x704
 */
typedef union {
  struct {
    UINT32 dis_2cyc_byp : 1;
    /* dis_2cyc_byp - Bits[0:0], RW_LB, default = 1'b0 
       Disable 2cyc bypass
     */
    UINT32 rsvd_1 : 1;
    UINT32 dis_opp_isoc_cas : 1;
    /* dis_opp_isoc_cas - Bits[2:2], RW_LB, default = 1'b0 
       Disable opp isoch cas
     */
    UINT32 dis_opp_isoc_ras : 1;
    /* dis_opp_isoc_ras - Bits[3:3], RW_LB, default = 1'b0 
       Disable opp isoch ras
     */
    UINT32 dis_opp_cas : 1;
    /* dis_opp_cas - Bits[4:4], RW_LB, default = 1'b0 
       Disable opp cas
     */
    UINT32 dis_opp_ras : 1;
    /* dis_opp_ras - Bits[5:5], RW_LB, default = 1'b0 
       Disable Opportunity RAS
     */
    UINT32 dis_prtl_wr_strvn : 1;
    /* dis_prtl_wr_strvn - Bits[6:6], RW_LB, default = 1'b0 
       Disable partial write starvation
     */
    UINT32 dis_blck_ph : 1;
    /* dis_blck_ph - Bits[7:7], RW_LB, default = 1'b0 
       Disable block ph
     */
    UINT32 dis_blck_pe : 1;
    /* dis_blck_pe - Bits[8:8], RW_LB, default = 1'b0 
       Disable block pe
     */
    UINT32 dis_blck_pm : 1;
    /* dis_blck_pm - Bits[9:9], RW_LB, default = 1'b0 
       Disable block pm
     */
    UINT32 cmd_oe_alwy_off : 1;
    /* cmd_oe_alwy_off - Bits[10:10], RW_LB, default = 1'b0 
       Command Output Enable is always off
     */
    UINT32 cmd_oe_alwy_on : 1;
    /* cmd_oe_alwy_on - Bits[11:11], RW_LB, default = 1'b0 
       Cmd output enable always on
     */
    UINT32 defeature_2 : 1;
    /* defeature_2 - Bits[12:12], RW_LB, default = 1'b0 
       Enable One-shot ECC injection for  LINK
     */
    UINT32 rsvd_13 : 2;
    UINT32 ovrd_odt_to_io : 1;
    /* ovrd_odt_to_io - Bits[15:15], RW_LB, default = 1'b0 
       Remove floor on safe timings
     */
    UINT32 dis_rdimm_par_gen : 1;
    /* dis_rdimm_par_gen - Bits[16:16], RW_LB, default = 1'b0 
       Disable rdimm DDR4 Command Parity Generation
     */
    UINT32 dis_pwmm : 1;
    /* dis_pwmm - Bits[17:17], RW_LB, default = 1'b0 
       Disable Partial Write Major Mode
       Note: this chicken bit should not be enabled when mirror mode is enabled - it 
       will cause a deadlock. 
     */
    UINT32 rsvd_18 : 3;
    UINT32 rpq_ring_cntr_dis : 1;
    /* rpq_ring_cntr_dis - Bits[21:21], RW_LB, default = 1'b0  */
    UINT32 rpq_sa_cntr_dis : 1;
    /* rpq_sa_cntr_dis - Bits[22:22], RW_LB, default = 1'b0 
       Disable RPQ SA age tracking counter
     */
    UINT32 dis_pwr_wmm_exit : 1;
    /* dis_pwr_wmm_exit - Bits[23:23], RW_LB, default = 1'b0 
       Set this bit to exit WMM when there are no further starved partial writes if we 
       entered WMM due to starved partials (as opposed to entering WMM due to a high 
       level of writes in the WPQ). 
     */
    UINT32 pri_starvation_wim : 1;
    /* pri_starvation_wim - Bits[24:24], RW_LB, default = 1'b0 
       Write Isoch and starvation priority. When set to 1, isoch is the hightest 
       priority write priority. When set to 0, partial write major mode and write major 
       mode starvation are higher priorities than write isoch. 
     */
    UINT32 dis_pgt_tmr : 1;
    /* dis_pgt_tmr - Bits[25:25], RW_LB, default = 1'b0 
       Disable page table idle counters.
     */
    UINT32 dis_rpq0_last_entry : 1;
    /* dis_rpq0_last_entry - Bits[26:26], RW_LB, default = 1'b0 
       Disable special code to use last entry of RPQ0
     */
    UINT32 dis_rpq1 : 1;
    /* dis_rpq1 - Bits[27:27], RW_LB, default = 1'b0 
       Disable use of RPQ1
     */
    UINT32 rsvd_28 : 1;
    UINT32 dis_ddrt_par_gen : 1;
    /* dis_ddrt_par_gen - Bits[29:29], RW_LB, default = 1'b0 
       Disable rdimm DDRT Command Parity Generation
     */
    UINT32 def_0 : 1;
    /* def_0 - Bits[30:30], RW_LB, default = 1'b0 
       Disable fix for b303371.
     */
    UINT32 def_1 : 1;
    /* def_1 - Bits[31:31], RW_LB, default = 1'b0 
       Set to 1 to enable write pointer for CADB lmn_fifo.
     */
  } Bits;
  UINT32 Data;
} MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT;


/* MCSCHED_CHKN_BIT2_MCDDC_CTL_REG supported on:                              */
/*       SKX_A0 (0x40252708)                                                  */
/*       SKX (0x40252708)                                                     */
/* Register default value:              0x0210000C                            */
#define MCSCHED_CHKN_BIT2_MCDDC_CTL_REG 0x0B004708
/* Struct format extracted from XML file SKX_A0\2.10.2.CFG.xml.
 * generated by critter 20_0_0x708
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 8;
    UINT32 dis_gs_pri_rcb_gate : 1;
    /* dis_gs_pri_rcb_gate - Bits[8:8], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in GS
     */
    UINT32 dis_pgt_pri_rcb_gate : 1;
    /* dis_pgt_pri_rcb_gate - Bits[9:9], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in PGT
     */
    UINT32 rsvd_10 : 1;
    UINT32 dis_safe_pri_rcb_gate : 1;
    /* dis_safe_pri_rcb_gate - Bits[11:11], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in Safe
     */
    UINT32 rsvd_12 : 3;
    UINT32 force_pcommit_ack : 1;
    /* force_pcommit_ack - Bits[15:15], RW_LB, default = 1'b0 
       
                Set to 1 to force DDR4 scheduler to issue an Ack for a pcommit 
               
     */
    UINT32 ignore_pcommit : 1;
    /* ignore_pcommit - Bits[16:16], RW_LB, default = 1'b0 
       
                Set to 1 to for DDR4 scheduler to ignore pcommit command
               
     */
    UINT32 pcommit_ack_for_linkfail : 1;
    /* pcommit_ack_for_linkfail - Bits[17:17], RW_LB, default = 1'b0 
       
                Default behavior is for pcommit acks to be blocked once a
                channel hits link fail condition. If this bit is set, MC will send an
                ACK for pcommit even in link fail condition. This bit is used
                for pcommits directed to NVDIMMs.
               
     */
    UINT32 adddc_wpq_dly : 4;
    /* adddc_wpq_dly - Bits[21:18], RW_LB, default = 4'b0100 
       Delay between primary and buddy command from WPQ in ADDDC mode; actual delay is 
       programmed value + 6 
     */
    UINT32 x8_wpq_dly : 4;
    /* x8_wpq_dly - Bits[25:22], RW_LB, default = 4'b1000 
       Delay between primary and buddy command from WPQ in x8 SDDC mode; actual delay 
       is programmed value + 6 
     */
    UINT32 enable_fwstarve : 1;
    /* enable_fwstarve - Bits[26:26], RW_LB, default = 1'b0 
       
                Set to 1 to enable full write starvation when pcommit is not pending
               
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dis_pat_pmrsblock : 1;
    /* dis_pat_pmrsblock - Bits[28:28], RW_LB, default = 1'b0 
       Set to 1 to not allow patrol to be issued during PmRsBlock
     */
    UINT32 clear_adddc_plusone_en : 1;
    /* clear_adddc_plusone_en - Bits[29:29], RW_LB, default = 1'b0 
       Set to 1 to clear the ADDDCPlusOneEn signal that results in 2 cycle delay in 
       read datapath; software need to clear this bit to 0 after setting it to 1 
     */
    UINT32 clear_dev_sub_en : 1;
    /* clear_dev_sub_en - Bits[30:30], RW_LB, default = 1'b0 
       Set to 1 to clear the dev_sub_en signal that results in 1 cycle delay in read 
       datapath; software need to clear this bit to 0 after setting it to 1 
     */
    UINT32 dis_force_bypass_cpgc : 1;
    /* dis_force_bypass_cpgc - Bits[31:31], RW_LB, default = 1'b0 
       Set to 1 to disable forced bypass for CAM match in CPGC mode
     */
  } Bits;
  UINT32 Data;
} MCSCHED_CHKN_BIT2_MCDDC_CTL_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x708
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 8;
    UINT32 dis_gs_pri_rcb_gate : 1;
    /* dis_gs_pri_rcb_gate - Bits[8:8], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in GS
     */
    UINT32 dis_pgt_pri_rcb_gate : 1;
    /* dis_pgt_pri_rcb_gate - Bits[9:9], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in PGT
     */
    UINT32 rsvd_10 : 1;
    UINT32 dis_safe_pri_rcb_gate : 1;
    /* dis_safe_pri_rcb_gate - Bits[11:11], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in Safe
     */
    UINT32 dis_misc_pri_rcb_gate : 1;
    /* dis_misc_pri_rcb_gate - Bits[12:12], RW_LB, default = 1'b0 
       Disable RCB Clock Gate in Misc
     */
    UINT32 dis_wpqpp_on_preempt_merge : 1;
    /* dis_wpqpp_on_preempt_merge - Bits[13:13], RW_LB, default = 1'b0 
       Disable merged write into a preempted WPQ entry from not setting pagetable 
       WPQPH. When this bit is one the machine will set the WPQPH bit in the page table 
       even though there may be no page hit in the WPQ. This will result in the rank 
       being locked up until a refresh event comes along. 
     */
    UINT32 dis_odt_pda_dl : 1;
    /* dis_odt_pda_dl - Bits[14:14], RW_LB, default = 1'b0 
       Disable the fix for ODT when enable_pda_doublelength is 1 (HSX HSD273201)
     */
    UINT32 force_pcommit_ack : 1;
    /* force_pcommit_ack - Bits[15:15], RW_LB, default = 1'b0 
       
                Set to 1 to force DDR4 scheduler to issue an Ack for a pcommit 
               
     */
    UINT32 ignore_pcommit : 1;
    /* ignore_pcommit - Bits[16:16], RW_LB, default = 1'b0 
       
                Set to 1 to for DDR4 scheduler to ignore pcommit command
               
     */
    UINT32 pcommit_ack_for_linkfail : 1;
    /* pcommit_ack_for_linkfail - Bits[17:17], RW_LB, default = 1'b0 
       
                Default behavior is for pcommit acks to be blocked once a
                channel hits link fail condition. If this bit is set, MC will send an
                ACK for pcommit even in link fail condition. This bit is used
                for pcommits directed to NVDIMMs.
               
     */
    UINT32 adddc_wpq_dly : 4;
    /* adddc_wpq_dly - Bits[21:18], RW_LB, default = 4'b0100 
       Delay between primary and buddy command from WPQ in ADDDC mode; actual delay is 
       programmed value + 6 
     */
    UINT32 x8_wpq_dly : 4;
    /* x8_wpq_dly - Bits[25:22], RW_LB, default = 4'b1000 
       Delay between primary and buddy command from WPQ in x8 SDDC mode; actual delay 
       is programmed value + 6 
     */
    UINT32 enable_fwstarve : 1;
    /* enable_fwstarve - Bits[26:26], RW_LB, default = 1'b0 
       
                Set to 1 to enable full write starvation when pcommit is not pending
               
     */
    UINT32 drop_writes_on_viral : 1;
    /* drop_writes_on_viral - Bits[27:27], RW_LB, default = 1'b0 
       
               Set to 1 to drop writes when viral condition occurs. This bit
               needs to be set when NVDIMMs are populated on the system. 
               
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} MCSCHED_CHKN_BIT2_MCDDC_CTL_STRUCT;



/* MCSCHED_RCB_GATE_CTL_MCDDC_CTL_REG supported on:                           */
/*       SKX_A0 (0x4025270C)                                                  */
/*       SKX (0x4025270C)                                                     */
/* Register default value:              0x00000040                            */
#define MCSCHED_RCB_GATE_CTL_MCDDC_CTL_REG 0x0B00470C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Control information for RCB clock gating in mcscheds
 */
typedef union {
  struct {
    UINT32 rcb_gate_mcsched_timer : 10;
    /* rcb_gate_mcsched_timer - Bits[9:0], RW_LB, default = 10'b0001000000 
       Time clock keeps running after triggering events
     */
    UINT32 rsvd : 22;
    /* rsvd - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCSCHED_RCB_GATE_CTL_MCDDC_CTL_STRUCT;


/* REFRESH_FSM_STATE_0_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x40252710)                                                  */
/*       SKX (0x40252710)                                                     */
/* Register default value:              0x3F3F3F3F                            */
#define REFRESH_FSM_STATE_0_MCDDC_CTL_REG 0x0B004710
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  This register contains current and previous state of refresh FSMs for rank 0 
 * and 1.   
 */
typedef union {
  struct {
    UINT32 rank0_current_state : 6;
    /* rank0_current_state - Bits[5:0], RO_V, default = 6'b111111 
       Current encoded state of rank 0 refresh FSM
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank0_previous_state : 6;
    /* rank0_previous_state - Bits[13:8], RO_V, default = 6'b111111 
       Previous encoded state of rank 0 refresh FSM
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank1_current_state : 6;
    /* rank1_current_state - Bits[21:16], RO_V, default = 6'b111111 
       Current encoded state of rank 1 refresh FSM
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank1_previous_state : 6;
    /* rank1_previous_state - Bits[29:24], RO_V, default = 6'b111111 
       Previous encoded state of rank 1 refresh FSM
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} REFRESH_FSM_STATE_0_MCDDC_CTL_STRUCT;


/* REFRESH_FSM_STATE_1_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x40252714)                                                  */
/*       SKX (0x40252714)                                                     */
/* Register default value:              0x3F3F3F3F                            */
#define REFRESH_FSM_STATE_1_MCDDC_CTL_REG 0x0B004714
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  This register contains current and previous state of refresh FSMs for rank 2 
 * and 3.   
 */
typedef union {
  struct {
    UINT32 rank2_current_state : 6;
    /* rank2_current_state - Bits[5:0], RO_V, default = 6'b111111 
       Current encoded state of rank 2 refresh FSM
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank2_previous_state : 6;
    /* rank2_previous_state - Bits[13:8], RO_V, default = 6'b111111 
       Previous encoded state of rank 2 refresh FSM
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank3_current_state : 6;
    /* rank3_current_state - Bits[21:16], RO_V, default = 6'b111111 
       Current encoded state of rank 3 refresh FSM
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank3_previous_state : 6;
    /* rank3_previous_state - Bits[29:24], RO_V, default = 6'b111111 
       Previous encoded state of rank 3 refresh FSM
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} REFRESH_FSM_STATE_1_MCDDC_CTL_STRUCT;


/* REFRESH_FSM_STATE_2_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x40252718)                                                  */
/*       SKX (0x40252718)                                                     */
/* Register default value:              0x3F3F3F3F                            */
#define REFRESH_FSM_STATE_2_MCDDC_CTL_REG 0x0B004718
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  This register contains current and previous state of refresh FSMs for rank 4 
 * and 5.   
 */
typedef union {
  struct {
    UINT32 rank4_current_state : 6;
    /* rank4_current_state - Bits[5:0], RO_V, default = 6'b111111 
       Current encoded state of rank 4 refresh FSM
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank4_previous_state : 6;
    /* rank4_previous_state - Bits[13:8], RO_V, default = 6'b111111 
       Previous encoded state of rank 4 refresh FSM
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank5_current_state : 6;
    /* rank5_current_state - Bits[21:16], RO_V, default = 6'b111111 
       Current encoded state of rank 5 refresh FSM
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank5_previous_state : 6;
    /* rank5_previous_state - Bits[29:24], RO_V, default = 6'b111111 
       Previous encoded state of rank 5 refresh FSM
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} REFRESH_FSM_STATE_2_MCDDC_CTL_STRUCT;


/* REFRESH_FSM_STATE_3_MCDDC_CTL_REG supported on:                            */
/*       SKX_A0 (0x4025271C)                                                  */
/*       SKX (0x4025271C)                                                     */
/* Register default value:              0x3F3F3F3F                            */
#define REFRESH_FSM_STATE_3_MCDDC_CTL_REG 0x0B00471C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  This register contains current and previous state of refresh FSMs for rank 6 
 * and 7.   
 */
typedef union {
  struct {
    UINT32 rank6_current_state : 6;
    /* rank6_current_state - Bits[5:0], RO_V, default = 6'b111111 
       Current encoded state of rank 6 refresh FSM
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank6_previous_state : 6;
    /* rank6_previous_state - Bits[13:8], RO_V, default = 6'b111111 
       Previous encoded state of rank 6 refresh FSM
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank7_current_state : 6;
    /* rank7_current_state - Bits[21:16], RO_V, default = 6'b111111 
       Current encoded state of rank 7 refresh FSM
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rank7_previous_state : 6;
    /* rank7_previous_state - Bits[29:24], RO_V, default = 6'b111111 
       Previous encoded state of rank 7 refresh FSM
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} REFRESH_FSM_STATE_3_MCDDC_CTL_STRUCT;


/* DDRT_MM_DDR4_CMD_THRESH_MCDDC_CTL_REG supported on:                        */
/*       SKX (0x4025235C)                                                     */
/* Register default value:              0x00001010                            */
#define DDRT_MM_DDR4_CMD_THRESH_MCDDC_CTL_REG 0x0B00435C
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * Error Injection Response Function
 */
typedef union {
  struct {
    UINT32 ddr4_wr_cmd_threshold : 8;
    /* ddr4_wr_cmd_threshold - Bits[7:0], RW_LB, default = 8'h10 
       Number of DDR4 reads to exceed to force switch from DDRT to DDR4 
     */
    UINT32 ddr4_rd_cmd_threshold : 8;
    /* ddr4_rd_cmd_threshold - Bits[15:8], RW_LB, default = 8'h10 
       Controls turn-around from DDRT GNT to DDR4 Read
     */
    UINT32 ignore_pwr_starve : 1;
    /* ignore_pwr_starve - Bits[16:16], RW_LB, default = 1'b0 
       Ignore DDR4 PWR starve to swtich from DDRT to DDR4 mode 
     */
    UINT32 ignore_pre : 1;
    /* ignore_pre - Bits[17:17], RW_LB, default = 1'b0 
       Ignore Pending PRE to swtich from DDRT to DDR4 mode 
     */
    UINT32 spare : 14;
    /* spare - Bits[31:18], RW_LB, default = 14'b0 
       Spare 
     */
  } Bits;
  UINT32 Data;
} DDRT_MM_DDR4_CMD_THRESH_MCDDC_CTL_STRUCT;


/* SREF_PKGC_MCDDC_CTL_REG supported on:                                      */
/*       SKX (0x40252680)                                                     */
/* Register default value:              0x0800000F                            */
#define SREF_PKGC_MCDDC_CTL_REG 0x0B004680
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  Self Refresh controls in PkgC.  Used when PCU/PMLink has asserted 
 * Force_SR_Enter and Force_SR_Exit.  Overrides equivalent fields in SREF_LL0 and 
 * SREF_LL1.  
 */
typedef union {
  struct {
    UINT32 sref_idle_timer : 20;
    /* sref_idle_timer - Bits[19:0], RWS, default = 20'b00000000000000001111 
       
                           This field defines the rank idle period that causes 
       self-refresh entrance. This value is used when the 'SREF_EN' field is set. It 
       defines the number of idle cycles (in DCLKs) after the command issue that there 
       should not be any transaction in order to enter self-refresh for DDR4 DIMMs or 
       PM Idle for DDR-T DIMMs. It is programmable 1 to 1M-1 dynamically.  FFFFEh is a 
       reserved value and should not be used in normal operation. 
                           The min setting needs to allow for a refresh, a zqcal, a 
       retry read, and a handfull of cycles for Mesh2Mem to issue a demand scrub write: 
                           TCZQCAL.T_ZQCS + TCRFTP.T_RFC + 100 decimal. In reality, the 
       idle counter should be much larger in order avoid unnecessary SRE+SRX overhead. 
                           This register field can be updated dynamically.
       
                       
     */
    UINT32 opp_sref_en : 1;
    /* opp_sref_en - Bits[20:20], RO, default = 1'b0 
       
                           When set, this bit enables opportunistic Self Refresh entry 
       based on the SREF_IDLE_TIMER expiration.  
                           FORCE_SR and AsyncSR are other sources of SR entry 
       independent of the setting of SREF_EN. 
                           When 0, Opportunistic SR Entry is disabled and SR can only 
       be entered on Force_SR_Entry assertion or AsyncSR. 
       
                           A write to this register must be respected by iMC. If iMC 
       has entered self-refresh opportunistically, it must return to the state 
       specified by this register. PCU however needs to ensure that modifications 
       happen only under legal conditions. For example clocks must be available before 
       asking iMC to return to DLL on and SR exit state. 
       
                       
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[21:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mdll_off_en : 1;
    /* mdll_off_en - Bits[22:22], RW, default = 1'b0 
       
                           When 0 Master DLLs (MDLL) cannot be turned off. When 1 MDLLs 
       can be turned off in Self Refresh.  
                           A write to this register must be respected by IMC. If IMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to MDLL on and SR exit state. 
                       
     */
    UINT32 ibt_off_en : 1;
    /* ibt_off_en - Bits[23:23], RW, default = 1'b0 
       Enable IBT_OFF Register Power Down Mode when set; otherwise, IBT_ON is enabled.
     */
    UINT32 ck_mode : 2;
    /* ck_mode - Bits[25:24], RW, default = 2'b00 
       
                           The field defines how CK and CK# are turned off during Self 
       Refresh: 
       
                           CK_MODE=00, CK_ON: In this mode CK continues to be driven 
       during self-refresh. 
                           CK_MODE=01 is not supported 
                           CK_MODE=10, CK_PULL_LOW_MODE: after tCKEoff timing delay 
       from SRE CKE de-assertion, IMC wait for tCKoff before dropping CK-ALIGN and 
       CK#-ALIGN (internal signal to DDRIO) to LOW thoughout the self-refresh.  
                           CK_MODE=11 is not supported. 
       
                           CKE tri-state is under separate control in DDRIO. 
                           All other signals (except DDR_RESET#) are tri-stated after 
       tCKEv delay. 
       
                           A write to this register must be respected by iMC. If iMC is 
       already in a deeper state, it must return to the state specified by this 
       register. PCU however needs to ensure that the modifications happen only under 
       legal conditions. For example clocks must be available, before asking MC to 
       return to DLL on and SR exit state. 
                       
     */
    UINT32 ddrt_idle_cmd : 2;
    /* ddrt_idle_cmd - Bits[27:26], RW, default = 2'b10 
       
                           Bits to send as part of the DDR-T PM Idle command to 
       indicate the specific command to be executed.  Encoding is as per the DDR-T 
       spec. 
                       
     */
    UINT32 reserved : 4;
    /* reserved - Bits[31:28], RW, default = 6'b000000 
       Reserved for future use.
     */
  } Bits;
  UINT32 Data;
} SREF_PKGC_MCDDC_CTL_STRUCT;


/* NO_BLEGR_CRD_STARVE_CTL_MCDDC_CTL_REG supported on:                        */
/*       SKX (0x40252684)                                                     */
/* Register default value:              0x00040012                            */
#define NO_BLEGR_CRD_STARVE_CTL_MCDDC_CTL_REG 0x0B004684
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * 
 *             this register will be used to break the deadlock case b/c of 
 * NoBlEgrCrd      
 *             this register has a threshold to detect we are out of BL egress 
 * credit for that much time and 
 *             trigger the error flow. The counter should be reset if we 
 * NoBlEgrCrdt = 0,  
 *             or when there is no reads in RPQ and no partial writes in WPQ.
 *             When the counter reaches the threshold, the trig signal for error 
 * flow should be set. 
 *       
 */
typedef union {
  struct {
    UINT32 timer : 6;
    /* timer - Bits[5:0], RW_LB, default = 6'h12 
       Threshold for number of cycles of in base timer. The threshold field is 
       incremented every (2^timer) DCLK ticks. 
     */
    UINT32 rsvd_6 : 10;
    /* rsvd_6 - Bits[15:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 threshold : 8;
    /* threshold - Bits[23:16], RW_LB, default = 8'h4 
       Threshold for the base timer to wrap around.
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} NO_BLEGR_CRD_STARVE_CTL_MCDDC_CTL_STRUCT;


/* MCSCHED_CHKN_BIT3_MCDDC_CTL_REG supported on:                              */
/*       SKX (0x40252720)                                                     */
/* Register default value:              0x00000000                            */
#define MCSCHED_CHKN_BIT3_MCDDC_CTL_REG 0x0B004720
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 * generated by critter 20_0_0x708
 */
typedef union {
  struct {
    UINT32 defeature : 32;
    /* defeature - Bits[31:0], RW_LB, default = 32'b0 
       
                   bit 0: disable fix for b304608; 
                   bit 1: disable fix for b304674; 
                   bit 2: disable fix for b304732; 
                   bits 5:3: disable fix for b305001; 
                   bit  7:6: disable fix for b304794
                   bit  8 : disable for NoBlEgrCrdt b304797
                   bit  9 : disable rpax parity error b306492
                   bit  10: disable wpax parity error b306492
                   bit  11: disable fix for b307105; allow exit of WMM due to MMStarve 
       when write is starved 
                   bit  12: disable fix for b307105; allow exit of WMM due to MMStarve 
       when no BL egress credit 
                   bit  13: disable fix for b307204; not increment RPQ age timeout 
       counter with ACT 
                   bit  14: always increment RPQ age timeout counter with ACT (even if 
       not starved) 
                   bit  15: enable 1-cycle datapath delay for device substitution
                   bit  16: enable 2-cycle datapath delay for ADDDC+1
                   bit  17: disable fix for b310908 
                   bit  23: disable fix for b311503, chose bit 23 instead of 18 as it 
       was timing friendly for manual ECO 
                   bits 31:17: spare 
     */
  } Bits;
  UINT32 Data;
} MCSCHED_CHKN_BIT3_MCDDC_CTL_STRUCT;


/* PC6_CLKSTP_FSM_MCDDC_CTL_REG supported on:                                 */
/*       SKX (0x40252724)                                                     */
/* Register default value:              0x00000000                            */
#define PC6_CLKSTP_FSM_MCDDC_CTL_REG 0x0B004724
/* Struct format extracted from XML file SKX\2.10.2.CFG.xml.
 *  This register contains current and previous state of refresh FSMs for rank 6 
 * and 7.   
 */
typedef union {
  struct {
    UINT32 pc6_clkstp_fsm : 7;
    /* pc6_clkstp_fsm - Bits[6:0], RO_V, default = 7'h0 
       Current state of PC6 clock stop fsm in MNT
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pcupwdn_fsm : 2;
    /* pcupwdn_fsm - Bits[9:8], RO_V, default = 2'h0 
       Current state of PCUPWDN fsm in MNT
     */
    UINT32 srck_fsm : 3;
    /* srck_fsm - Bits[12:10], RO_V, default = 3'h0 
       Current state of SRCK fsm in MNT
     */
    UINT32 rsvd_13 : 19;
    /* rsvd_13 - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PC6_CLKSTP_FSM_MCDDC_CTL_STRUCT;


#endif /* MCDDC_CTL_h */
