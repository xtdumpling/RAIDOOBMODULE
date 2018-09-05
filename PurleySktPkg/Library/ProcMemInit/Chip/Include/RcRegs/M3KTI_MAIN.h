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

#ifndef M3KTI_MAIN_h
#define M3KTI_MAIN_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* M3KTI_MAIN_DEV 18                                                          */
/* M3KTI_MAIN_FUN 0                                                           */

/* VID_M3KTI_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x20390000)                                                  */
/*       SKX (0x20390000)                                                     */
/* Register default value:              0x8086                                */
#define VID_M3KTI_MAIN_REG 0x0A002000
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} VID_M3KTI_MAIN_STRUCT;


/* DID_M3KTI_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x20390002)                                                  */
/*       SKX (0x20390002)                                                     */
/* Register default value:              0x204C                                */
#define DID_M3KTI_MAIN_REG 0x0A002002
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h204C 
        
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
} DID_M3KTI_MAIN_STRUCT;


/* PCICMD_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x20390004)                                                  */
/*       SKX (0x20390004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_M3KTI_MAIN_REG 0x0A002004
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} PCICMD_M3KTI_MAIN_STRUCT;


/* PCISTS_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x20390006)                                                  */
/*       SKX (0x20390006)                                                     */
/* Register default value on SKX_A0:    0x0000                                */
/* Register default value on SKX:       0x0010                                */
#define PCISTS_M3KTI_MAIN_REG 0x0A002006
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} PCISTS_M3KTI_MAIN_STRUCT;


/* RID_M3KTI_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x10390008)                                                  */
/*       SKX (0x10390008)                                                     */
/* Register default value:              0x00                                  */
#define RID_M3KTI_MAIN_REG 0x0A001008
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} RID_M3KTI_MAIN_STRUCT;


/* CCR_N0_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x10390009)                                                  */
/*       SKX (0x10390009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_M3KTI_MAIN_REG 0x0A001009
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_M3KTI_MAIN_STRUCT;


/* CCR_N1_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x2039000A)                                                  */
/*       SKX (0x2039000A)                                                     */
/* Register default value:              0x1101                                */
#define CCR_N1_M3KTI_MAIN_REG 0x0A00200A
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} CCR_N1_M3KTI_MAIN_STRUCT;


/* CLSR_M3KTI_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x1039000C)                                                  */
/*       SKX (0x1039000C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_M3KTI_MAIN_REG 0x0A00100C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} CLSR_M3KTI_MAIN_STRUCT;


/* PLAT_M3KTI_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x1039000D)                                                  */
/*       SKX (0x1039000D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_M3KTI_MAIN_REG 0x0A00100D
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} PLAT_M3KTI_MAIN_STRUCT;


/* HDR_M3KTI_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x1039000E)                                                  */
/*       SKX (0x1039000E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_M3KTI_MAIN_REG 0x0A00100E
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} HDR_M3KTI_MAIN_STRUCT;


/* BIST_M3KTI_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x1039000F)                                                  */
/*       SKX (0x1039000F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_M3KTI_MAIN_REG 0x0A00100F
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} BIST_M3KTI_MAIN_STRUCT;


/* SVID_M3KTI_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x2039002C)                                                  */
/*       SKX (0x2039002C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_M3KTI_MAIN_REG 0x0A00202C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Subsystem_Vendor_Identification_Number
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
        
     */
  } Bits;
  UINT16 Data;
} SVID_M3KTI_MAIN_STRUCT;


/* SDID_M3KTI_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x2039002E)                                                  */
/*       SKX (0x2039002E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_M3KTI_MAIN_REG 0x0A00202E
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Subsystem_Device_Identification_Number
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
        
     */
  } Bits;
  UINT16 Data;
} SDID_M3KTI_MAIN_STRUCT;


/* CAPPTR_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x10390034)                                                  */
/*       SKX (0x10390034)                                                     */
/* Register default value on SKX_A0:    0x00                                  */
/* Register default value on SKX:       0x40                                  */
#define CAPPTR_M3KTI_MAIN_REG 0x0A001034
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} CAPPTR_M3KTI_MAIN_STRUCT;


/* INTL_M3KTI_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x1039003C)                                                  */
/*       SKX (0x1039003C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_M3KTI_MAIN_REG 0x0A00103C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} INTL_M3KTI_MAIN_STRUCT;


/* INTPIN_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x1039003D)                                                  */
/*       SKX (0x1039003D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_M3KTI_MAIN_REG 0x0A00103D
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} INTPIN_M3KTI_MAIN_STRUCT;


/* MINGNT_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x1039003E)                                                  */
/*       SKX (0x1039003E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_M3KTI_MAIN_REG 0x0A00103E
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} MINGNT_M3KTI_MAIN_STRUCT;


/* MAXLAT_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x1039003F)                                                  */
/*       SKX (0x1039003F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_M3KTI_MAIN_REG 0x0A00103F
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
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
} MAXLAT_M3KTI_MAIN_STRUCT;


/* PXPCAP_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x40390040)                                                  */
/*       SKX (0x40390040)                                                     */
/* Register default value on SKX_A0:    0x00000000                            */
/* Register default value on SKX:       0x00920010                            */
#define PXPCAP_M3KTI_MAIN_REG 0x0A004040
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * PCI Express Capability Register
 */
typedef union {
  struct {
    UINT32 capability_id : 8;
    /* capability_id - Bits[7:0], RO, default = 8'h10 
       Identifies the PCI Express capability assigned by PCI-SIG
     */
    UINT32 next_ptr : 8;
    /* next_ptr - Bits[15:8], RO, default = 8'b0 
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
    /* interrupt_message_number - Bits[29:25], RO, default = 5'b0 
       Not valid for this device, since the device does not generate interrupts
     */
    UINT32 reserved : 2;
    /* reserved - Bits[31:30], RO, default = 2'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXPCAP_M3KTI_MAIN_STRUCT;


/* PXP_RSVD0_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390044)                                                  */
/*       SKX (0x40390044)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD0_M3KTI_MAIN_REG 0x0A004044
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD0_M3KTI_MAIN_STRUCT;


/* PXP_RSVD1_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390048)                                                  */
/*       SKX (0x40390048)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD1_M3KTI_MAIN_REG 0x0A004048
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD1_M3KTI_MAIN_STRUCT;


/* PXP_RSVD2_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x4039004C)                                                  */
/*       SKX (0x4039004C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD2_M3KTI_MAIN_REG 0x0A00404C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD2_M3KTI_MAIN_STRUCT;


/* PXP_RSVD3_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390050)                                                  */
/*       SKX (0x40390050)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD3_M3KTI_MAIN_REG 0x0A004050
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD3_M3KTI_MAIN_STRUCT;


/* PXP_RSVD4_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390054)                                                  */
/*       SKX (0x40390054)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD4_M3KTI_MAIN_REG 0x0A004054
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD4_M3KTI_MAIN_STRUCT;


/* PXP_RSVD5_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390058)                                                  */
/*       SKX (0x40390058)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD5_M3KTI_MAIN_REG 0x0A004058
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD5_M3KTI_MAIN_STRUCT;


/* PXP_RSVD6_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x4039005C)                                                  */
/*       SKX (0x4039005C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD6_M3KTI_MAIN_REG 0x0A00405C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD6_M3KTI_MAIN_STRUCT;


/* PXP_RSVD7_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390060)                                                  */
/*       SKX (0x40390060)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD7_M3KTI_MAIN_REG 0x0A004060
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD7_M3KTI_MAIN_STRUCT;


/* PXP_RSVD8_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390064)                                                  */
/*       SKX (0x40390064)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD8_M3KTI_MAIN_REG 0x0A004064
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD8_M3KTI_MAIN_STRUCT;


/* PXP_RSVD9_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390068)                                                  */
/*       SKX (0x40390068)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD9_M3KTI_MAIN_REG 0x0A004068
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD9_M3KTI_MAIN_STRUCT;


/* PXP_RSVD10_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x40390070)                                                  */
/*       SKX (0x40390070)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD10_M3KTI_MAIN_REG 0x0A004070
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD10_M3KTI_MAIN_STRUCT;


/* PXP_RSVD11_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x40390074)                                                  */
/*       SKX (0x40390074)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD11_M3KTI_MAIN_REG 0x0A004074
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD11_M3KTI_MAIN_STRUCT;


/* PXP_RSVD12_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x40390078)                                                  */
/*       SKX (0x40390078)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD12_M3KTI_MAIN_REG 0x0A004078
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 32'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD12_M3KTI_MAIN_STRUCT;


/* M3KKRT_M3KTI_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x40390080)                                                  */
/*       SKX (0x40390080)                                                     */
/* Register default value:              0xFFFFFFFF                            */
#define M3KKRT_M3KTI_MAIN_REG 0x0A004080
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Intel UPI Routing Configuration for both Ports
 */
typedef union {
  struct {
    UINT32 port0routeskt : 16;
    /* port0routeskt - Bits[15:0], RW_LB, default = 16'hFFFF 
       8-deep indexed by DNID. The output is 2-bits each indicating target Port.  
       Encoding is: 00-Intel UPI Port 0, 01-Intel UPI port 1, 10-Intel UPI Port 2, 
       11-Local Socket 
       Note: It is illegal to program port source to target itself.
     */
    UINT32 port1routeskt : 16;
    /* port1routeskt - Bits[31:16], RW_LB, default = 16'hFFFF 
       8-deep indexed by DNID. The output is 2-bits each indicating target Port.  
       Encoding is: 00-Intel UPI Port 0, 01-Intel UPI port 1, 10-Intel UPI Port 2, 
       11-Local Socket 
       Note: It is illegal to program port source to target itself.
     */
  } Bits;
  UINT32 Data;
} M3KKRT_M3KTI_MAIN_STRUCT;


/* M3KRTE0CR_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390084)                                                  */
/*       SKX (0x40390084)                                                     */
/* Register default value:              0x05500150                            */
#define M3KRTE0CR_M3KTI_MAIN_REG 0x0A004084
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Route Through Credits from Port 0 to all active peer Intel UPI ports. These 
 * values are only used in systems that enable Route-Through or for credits used 
 * from SMI3 ports for D2K flow. 
 */
typedef union {
  struct {
    UINT32 vna_rt_ad : 4;
    /* vna_rt_ad - Bits[3:0], RWS, default = 4'b0000 
       VNA Route Through Credits For AD Messages
     */
    UINT32 vn0_rt_snp : 2;
    /* vn0_rt_snp - Bits[5:4], RWS, default = 2'b01 
       VN0 Route Through SNP Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_req : 2;
    /* vn0_rt_req - Bits[7:6], RWS, default = 2'b01 
       VN0 Route Through REQ Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_rsp_ad : 2;
    /* vn0_rt_rsp_ad - Bits[9:8], RWS, default = 2'b01 
       VN0 Route Through RSP Credits for AD Messages
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 rsvd_10 : 6;
    /* rsvd_10 - Bits[15:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 vna_rt_bl : 4;
    /* vna_rt_bl - Bits[19:16], RWS, default = 4'b0000 
       VNA Route Through Credits For BL Messages
     */
    UINT32 vn0_rt_rsp_bl : 2;
    /* vn0_rt_rsp_bl - Bits[21:20], RWS, default = 2'b01 
       VN0 Route Through RSP Credits For BL Messages
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_ncs : 2;
    /* vn0_rt_ncs - Bits[23:22], RWS, default = 2'b01 
       VN0 Route Through NCS Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_ncb : 2;
    /* vn0_rt_ncb - Bits[25:24], RWS, default = 2'b01 
       VN0 Route Through NCB Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_wb : 2;
    /* vn0_rt_wb - Bits[27:26], RWS, default = 2'b01 
       VN0 Route Through WB Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KRTE0CR_M3KTI_MAIN_STRUCT;


/* M3KRTE1CR_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390088)                                                  */
/*       SKX (0x40390088)                                                     */
/* Register default value:              0x05500150                            */
#define M3KRTE1CR_M3KTI_MAIN_REG 0x0A004088
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Route Through Credits from Port 1 to all active peer Intel UPI ports. These 
 * values are only used in systems that enable Route-Through or for credits used 
 * from SMI3 ports for D2K flow. 
 */
typedef union {
  struct {
    UINT32 vna_rt_ad : 4;
    /* vna_rt_ad - Bits[3:0], RWS, default = 4'b0000 
       VNA Route Through Credits For AD Messages
     */
    UINT32 vn0_rt_snp : 2;
    /* vn0_rt_snp - Bits[5:4], RWS, default = 2'b01 
       VN0 Route Through SNP Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_req : 2;
    /* vn0_rt_req - Bits[7:6], RWS, default = 2'b01 
       VN0 Route Through REQ Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_rsp_ad : 2;
    /* vn0_rt_rsp_ad - Bits[9:8], RWS, default = 2'b01 
       VN0 Route Through RSP Credits for AD Messages
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 rsvd_10 : 6;
    /* rsvd_10 - Bits[15:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 vna_rt_bl : 4;
    /* vna_rt_bl - Bits[19:16], RWS, default = 4'b0000 
       VNA Route Through Credits For BL Messages
     */
    UINT32 vn0_rt_rsp_bl : 2;
    /* vn0_rt_rsp_bl - Bits[21:20], RWS, default = 2'b01 
       VN0 Route Through RSP Credits For BL Messages
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_ncs : 2;
    /* vn0_rt_ncs - Bits[23:22], RWS, default = 2'b01 
       VN0 Route Through NCS Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_ncb : 2;
    /* vn0_rt_ncb - Bits[25:24], RWS, default = 2'b01 
       VN0 Route Through NCB Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 vn0_rt_wb : 2;
    /* vn0_rt_wb - Bits[27:26], RWS, default = 2'b01 
       VN0 Route Through WB Credits
       Value also applies for VN1 credits when VN1 is enabled.
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KRTE1CR_M3KTI_MAIN_STRUCT;


/* M3KEGRCTRL0_M3KTI_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x40390090)                                                  */
/*       SKX (0x40390090)                                                     */
/* Register default value:              0x00000000                            */
#define M3KEGRCTRL0_M3KTI_MAIN_REG 0x0A004090
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Egress Control From Port 0
 */
typedef union {
  struct {
    UINT32 rpq_vna : 5;
    /* rpq_vna - Bits[4:0], RWS, default = 5'b0000 
       Shared Credits to RPQ in all CHA.
       RPQ size is 24. 3 RSVD RPQ per enabled Intel UPI Port. The remander are 
       substraction of the reserved should be split evenly between all active Intel UPI 
       ports (and programmed into this register). 
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pre_mode : 1;
    /* pre_mode - Bits[8:8], RWS, default = 1'b0 
       PRE Mode encoding for D2C messages.
       0: 1-hop encoding of PRE[4:3]
       1: 2-hop encoding of PRE[4:3]
       D2C are only supported from directly connected socket, so only expected usage 
       case for 2-hop encoding is for Node Controller topologies. 
     */
    UINT32 rsvd_9 : 23;
    /* rsvd_9 - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KEGRCTRL0_M3KTI_MAIN_STRUCT;


/* M3KEGRCTRL1_M3KTI_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x40390094)                                                  */
/*       SKX (0x40390094)                                                     */
/* Register default value:              0x00000000                            */
#define M3KEGRCTRL1_M3KTI_MAIN_REG 0x0A004094
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Egress Control From Port 1
 */
typedef union {
  struct {
    UINT32 rpq_vna : 5;
    /* rpq_vna - Bits[4:0], RWS, default = 5'b0000 
       Shared Credits to RPQ in all CHA.
       RPQ size is 24. 3 RSVD RPQ per enabled Intel UPI Port. The remander are 
       substraction of the reserved should be split evenly between all active Intel UPI 
       ports (and programmed into this register). 
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pre_mode : 1;
    /* pre_mode - Bits[8:8], RWS, default = 1'b0 
       PRE Mode encoding for D2C messages.
       0: 1-hop encoding of PRE[4:3]
       1: 2-hop encoding of PRE[4:3]
       D2C are only supported from directly connected socket, so only expected usage 
       case for 2-hop encoding is for Node Controller topologies. 
     */
    UINT32 rsvd_9 : 23;
    /* rsvd_9 - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KEGRCTRL1_M3KTI_MAIN_STRUCT;


/* M3KPCIR0CR_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x40390098)                                                  */
/*       SKX (0x40390098)                                                     */
/* Register default value:              0x12491249                            */
#define M3KPCIR0CR_M3KTI_MAIN_REG 0x0A004098
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 iio0_iio1_ncb : 3;
    /* iio0_iio1_ncb - Bits[2:0], RWS, default = 3'b001 
       IIO0 and IIO1 NCB Credits. IIO0 and IIO1 share a ring stop, so credits are 
       combined. 
     */
    UINT32 iio2_ubox_ncb : 3;
    /* iio2_ubox_ncb - Bits[5:3], RWS, default = 3'b001 
       IIO2 and UBox NCB Credits. IIO2 and UBox share a ring stop, so credits on the 
       mesh are combined 
     */
    UINT32 iio3_ncb : 3;
    /* iio3_ncb - Bits[8:6], RWS, default = 3'b001 
       IIO3 NCB Credits.
     */
    UINT32 iio4_ncb : 3;
    /* iio4_ncb - Bits[11:9], RWS, default = 3'b001 
       IIO4 NCB Credits.
     */
    UINT32 iio5_ncb : 3;
    /* iio5_ncb - Bits[14:12], RWS, default = 3'b001 
       IIO5 NCB Credits.
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 iio0_iio1_ncs : 3;
    /* iio0_iio1_ncs - Bits[18:16], RWS, default = 3'b001 
       IIO0 and IIO1 NCS Credits. IIO0 and IIO1 share a ring stop, so credits are 
       combined. 
     */
    UINT32 iio2_ubox_ncs : 3;
    /* iio2_ubox_ncs - Bits[21:19], RWS, default = 3'b001 
       IIO2 and UBox NCS Credits. IIO2 and UBox share a ring stop, so credits on the 
       mesh are combined 
     */
    UINT32 iio3_ncs : 3;
    /* iio3_ncs - Bits[24:22], RWS, default = 3'b001 
       IIO3 NCS Credits.
     */
    UINT32 iio4_ncs : 3;
    /* iio4_ncs - Bits[27:25], RWS, default = 3'b001 
       IIO4 NCS Credits.
     */
    UINT32 iio5_ncs : 3;
    /* iio5_ncs - Bits[30:28], RWS, default = 3'b001 
       IIO5 NCS Credits.
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KPCIR0CR_M3KTI_MAIN_STRUCT;


/* M3KPCIR1CR_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x4039009C)                                                  */
/*       SKX (0x4039009C)                                                     */
/* Register default value:              0x12491249                            */
#define M3KPCIR1CR_M3KTI_MAIN_REG 0x0A00409C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 iio0_iio1_ncb : 3;
    /* iio0_iio1_ncb - Bits[2:0], RWS, default = 3'b001 
       IIO0 and IIO1 NCB Credits. IIO0 and IIO1 share a ring stop, so credits are 
       combined. 
     */
    UINT32 iio2_ubox_ncb : 3;
    /* iio2_ubox_ncb - Bits[5:3], RWS, default = 3'b001 
       IIO2 and UBox NCB Credits. IIO2 and UBox share a ring stop, so credits on the 
       mesh are combined 
     */
    UINT32 iio3_ncb : 3;
    /* iio3_ncb - Bits[8:6], RWS, default = 3'b001 
       IIO3 NCB Credits.
     */
    UINT32 iio4_ncb : 3;
    /* iio4_ncb - Bits[11:9], RWS, default = 3'b001 
       IIO4 NCB Credits.
     */
    UINT32 iio5_ncb : 3;
    /* iio5_ncb - Bits[14:12], RWS, default = 3'b001 
       IIO5 NCB Credits.
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 iio0_iio1_ncs : 3;
    /* iio0_iio1_ncs - Bits[18:16], RWS, default = 3'b001 
       IIO0 and IIO1 NCS Credits. IIO0 and IIO1 share a ring stop, so credits are 
       combined. 
     */
    UINT32 iio2_ubox_ncs : 3;
    /* iio2_ubox_ncs - Bits[21:19], RWS, default = 3'b001 
       IIO2 and UBox NCS Credits. IIO2 and UBox share a ring stop, so credits on the 
       mesh are combined 
     */
    UINT32 iio3_ncs : 3;
    /* iio3_ncs - Bits[24:22], RWS, default = 3'b001 
       IIO3 NCS Credits.
     */
    UINT32 iio4_ncs : 3;
    /* iio4_ncs - Bits[27:25], RWS, default = 3'b001 
       IIO4 NCS Credits.
     */
    UINT32 iio5_ncs : 3;
    /* iio5_ncs - Bits[30:28], RWS, default = 3'b001 
       IIO5 NCS Credits.
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KPCIR1CR_M3KTI_MAIN_STRUCT;


/* M3KCTRL_M3KTI_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x403900A0)                                                  */
/*       SKX (0x403900A0)                                                     */
/* Register default value:              0x00002000                            */
#define M3KCTRL_M3KTI_MAIN_REG 0x0A0040A0
/* Struct format extracted from XML file SKX_A0\3.18.0.CFG.xml.
 * M3UPI Control
 */
typedef union {
  struct {
    UINT32 socketid : 3;
    /* socketid - Bits[2:0], RWS, default = 3'b000  */
    UINT32 rsvd_3 : 5;
    /* rsvd_3 - Bits[7:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 d2kthreshold : 4;
    /* d2kthreshold - Bits[11:8], RWS, default = 4'b0000  */
    UINT32 d2cthreshold : 4;
    /* d2cthreshold - Bits[15:12], RWS_LB, default = 4'b0010 
       4b value indicating how many txns must be in the BL Egress before a D2c message 
       is spawned (if requested from the Agent). This is needed to allow BL bandwidth 
       injection to scale down to 1 BL packet per message 
       under load instead of 2. Value of 9 and above means disable D2c and a value of 0 
       is not allowed 
       Note: this value is per Cacheline (2 pumps) in BL Egress
       Expected value programmed by BIOS is 4
     */
    UINT32 rsvd_16 : 8;
    /* rsvd_16 - Bits[23:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 port0clkdisable : 1;
    /* port0clkdisable - Bits[24:24], RWS, default = 1'b0  */
    UINT32 port1clkdisable : 1;
    /* port1clkdisable - Bits[25:25], RWS, default = 1'b0  */
    UINT32 meshcreditpush : 1;
    /* meshcreditpush - Bits[26:26], RWS, default = 1'b0 
       Mesh Credit Push. When set all VNA credits from one of the 2 Intel UPI ports 
       will be pushed to the adjacent M2PCI agent to reduce AK ring congestions. This 
       bit should only be set on M3UPI with Intel UPI 0 and Intel UPI 1 connected. It 
       should only be enabled if both Intel UPI 0 and Intel UPI 1 are enabled.  Note: 
       that pushing of credits always occurs from physical port 1, whos definition is 
       based on Si layout and may not correspond to Intel UPI port 1 on the package. 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KCTRL_M3KTI_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * M3UPI Control
 */
typedef union {
  struct {
    UINT32 socketid : 3;
    /* socketid - Bits[2:0], RWS, default = 3'b000  */
    UINT32 rsvd_3 : 5;
    /* rsvd_3 - Bits[7:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 d2kthreshold : 4;
    /* d2kthreshold - Bits[11:8], RWS, default = 4'b0000  */
    UINT32 d2cthreshold : 4;
    /* d2cthreshold - Bits[15:12], RWS_LB, default = 4'b0010 
       4b value indicating how many txns must be in the BL Egress before a D2c message 
       is spawned (if requested from the Agent). This is needed to allow BL bandwidth 
       injection to scale down to 1 BL packet per message 
       under load instead of 2. Value of 9 and above means disable D2c and a value of 0 
       is not allowed 
       Note: this value is per Cacheline (2 pumps) in BL Egress
       Expected value programmed by BIOS is 4
     */
    UINT32 socketidforsnp : 3;
    /* socketidforsnp - Bits[18:16], RWS_LB, default = 3'b000  */
    UINT32 rsvd_19 : 5;
    /* rsvd_19 - Bits[23:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 port0clkdisable : 1;
    /* port0clkdisable - Bits[24:24], RWS, default = 1'b0  */
    UINT32 port1clkdisable : 1;
    /* port1clkdisable - Bits[25:25], RWS, default = 1'b0  */
    UINT32 meshcreditpush : 1;
    /* meshcreditpush - Bits[26:26], RWS, default = 1'b0 
       Mesh Credit Push. When set all VNA credits from one of the 2 Intel UPI ports 
       will be pushed to the adjacent M2PCI agent to reduce AK ring congestions. This 
       bit should only be set on M3UPI with Intel UPI 0 and Intel UPI 1 connected. It 
       should only be enabled if both Intel UPI 0 and Intel UPI 1 are enabled.  Note: 
       that pushing of credits always occurs from physical port 1, whos definition is 
       based on Si layout and may not correspond to Intel UPI port 1 on the package. 
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KCTRL_M3KTI_MAIN_STRUCT;



/* M3SNPFANOUT_P0_M3KTI_MAIN_REG supported on:                                */
/*       SKX_A0 (0x403900A4)                                                  */
/*       SKX (0x403900A4)                                                     */
/* Register default value:              0x00000000                            */
#define M3SNPFANOUT_P0_M3KTI_MAIN_REG 0x0A0040A4
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Snoop Fanout Table From Port 0.
 */
typedef union {
  struct {
    UINT32 fanouttable : 24;
    /* fanouttable - Bits[23:0], RW_LB, default = 24'b000000000000000000000000 
       Table is a 8-deep x 3-bit wide indicating Snoop Fanout Destination.
       Index - Result Bits
       0 - 2:0
       1 - 5:3
       2 - 8:6
       3 - 11:9
       4 - 14:12
       5 - 17:15
       6 - 20:18
       7 - 23:21
       Result bits indicate Fanout Target:
       0 - Port 0
       1 - Port 1
       2 - Port 2
       It is not legal to set a bit corresponding to the source (same rule as Routing 
       Table) 
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3SNPFANOUT_P0_M3KTI_MAIN_STRUCT;


/* M3SNPFANOUT_P1_M3KTI_MAIN_REG supported on:                                */
/*       SKX_A0 (0x403900A8)                                                  */
/*       SKX (0x403900A8)                                                     */
/* Register default value:              0x00000000                            */
#define M3SNPFANOUT_P1_M3KTI_MAIN_REG 0x0A0040A8
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Snoop Fanout Table From Port 1.
 */
typedef union {
  struct {
    UINT32 fanouttable : 24;
    /* fanouttable - Bits[23:0], RW_LB, default = 24'b000000000000000000000000 
       Table is a 8-deep x 3-bit wide indicating Snoop Fanout Destination.
       Index - Result Bits
       0 - 2:0
       1 - 5:3
       2 - 8:6
       3 - 11:9
       4 - 14:12
       5 - 17:15
       6 - 20:18
       7 - 23:21
       Result bits indicate Fanout Target:
       0 - Port 0
       1 - Port 1
       2 - Port 2
       It is not legal to set a bit corresponding to the source (same rule as Routing 
       Table) 
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3SNPFANOUT_P1_M3KTI_MAIN_STRUCT;


/* M3KTXTRH_M3KTI_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x403900AC)                                                  */
/*       SKX (0x403900AC)                                                     */
/* Register default value:              0x30000000                            */
#define M3KTXTRH_M3KTI_MAIN_REG 0x0A0040AC
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * M3UPI Tx Queue Threshold Register (L0)
 */
typedef union {
  struct {
    UINT32 txqcreditsreserved0duringl0 : 6;
    /* txqcreditsreserved0duringl0 - Bits[5:0], RWS, default = 6'd0 
       TxQ Credit Reserved0 when in L0 mode.  The Reserved 0/1/2/3 register set is 
       selected by PCU through virtual wires to PMA .  PowerModeSelect value 0 (perf 
       mode) 
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 txqcreditsreserved1duringl0 : 6;
    /* txqcreditsreserved1duringl0 - Bits[13:8], RWS, default = 6'd0 
       TxQ Credit Reserved1 when in L0 mode.  The Reserved 0/1/2/3 register set is 
       selected by PCU through virtual wires to PMA .  PowerModeSelect value 1 (Power 
       saving mode1) 
     */
    UINT32 txqcreditsreserved2duringl0 : 6;
    /* txqcreditsreserved2duringl0 - Bits[19:14], RWS, default = 6'd0 
       TxQ Credit Reserved2 when in L0 mode.  The Reserved 0/1/2/3 register set is 
       selected by PCU through virtual wires to PMA .  PowerModeSelect value 2 (Power 
       saving mode2) 
     */
    UINT32 txqcreditsreserved3duringl0 : 6;
    /* txqcreditsreserved3duringl0 - Bits[25:20], RWS, default = 6'd0 
       TxQ Credit Reserved3 when in L0 mode.  The Reserved 0/1/2/3 register set is 
       selected by PCU through virtual wires to PMA .  PowerModeSelect value 3 (Power 
       saving mode3) 
     */
    UINT32 bgfmaxcreditcount : 4;
    /* bgfmaxcreditcount - Bits[29:26], RWS, default = 4'd12 
       For normal operation, the domain crossing FIFO between the Intel UPI ingress 
       logic and link layer has 12 entries. This field may be used to artificially 
       limit the size of this FIFO. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KTXTRH_M3KTI_MAIN_STRUCT;


/* M3KTXTRH1_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x403900B0)                                                  */
/*       SKX (0x403900B0)                                                     */
/* Register default value:              0x00000000                            */
#define M3KTXTRH1_M3KTI_MAIN_REG 0x0A0040B0
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * M3UPI Tx Queue Threshold Register (L0p)
 */
typedef union {
  struct {
    UINT32 txqcreditsreserved0duringl0p : 6;
    /* txqcreditsreserved0duringl0p - Bits[5:0], RWS, default = 6'd0 
       TxQ Credit Reserved0 when in L0p mode.  The Reserved 0/1/2/3  register set is 
       selected by PCU through virtual wires to PMA .  PowerModeSelect value 0 (perf 
       mode) 
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 txqcreditsreserved1duringl0p : 6;
    /* txqcreditsreserved1duringl0p - Bits[13:8], RWS, default = 6'd0 
       TxQ Credit Reserved1 when in L0p mode.  The Reserved 0/1/2/3 register set is 
       selected by PCU through virtual wires to PMA .  PowerModeSelect value 1 (Power 
       saving mode1) 
     */
    UINT32 txqcreditsreserved2duringl0p : 6;
    /* txqcreditsreserved2duringl0p - Bits[19:14], RWS, default = 6'd0 
       TxQ Credit Reserved2 when in L0p mode.  The Reserved 0/1/2/3  register set is 
       selected by PCU through virtual wires to PMA .  PowerModeSelect value 2 (Power 
       saving mode2) 
     */
    UINT32 txqcreditsreserved3duringl0p : 6;
    /* txqcreditsreserved3duringl0p - Bits[25:20], RWS, default = 6'd0 
       TxQ Credit Reserved3 when in L0p mode.  The Reserved 0/1/2/3  register set is 
       selected by PCU through virtual wires to PMA .  PowerModeSelect value 3 (Power 
       saving mode3) 
     */
    UINT32 rsvd_26 : 6;
    /* rsvd_26 - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KTXTRH1_M3KTI_MAIN_STRUCT;


/* M3GLERRCFG_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x403900B4)                                                  */
/*       SKX (0x403900B4)                                                     */
/* Register default value:              0x1003E7E0                            */
#define M3GLERRCFG_M3KTI_MAIN_REG 0x0A0040B4
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * M3UPI global viral/fatal error configuration register.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 3;
    /* rsvd_0 - Bits[2:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 resetglobalfatalerror : 1;
    /* resetglobalfatalerror - Bits[3:3], RW, default = 1'b0  */
    UINT32 resetglobalviral : 1;
    /* resetglobalviral - Bits[4:4], RW, default = 1'b0  */
    UINT32 maskm3fatalerror : 4;
    /* maskm3fatalerror - Bits[8:5], RW, default = 4'b1111  */
    UINT32 maskktifatalerrorin : 2;
    /* maskktifatalerrorin - Bits[10:9], RW, default = 2'b11  */
    UINT32 rsvd_11 : 2;
    /* rsvd_11 - Bits[12:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 maskktiviralout : 2;
    /* maskktiviralout - Bits[14:13], RW, default = 2'b11  */
    UINT32 maskktiiviralin : 2;
    /* maskktiiviralin - Bits[16:15], RW, default = 2'b11  */
    UINT32 maskpcufatalerrorout : 1;
    /* maskpcufatalerrorout - Bits[17:17], RW, default = 1'b1  */
    UINT32 localerrorstatus : 1;
    /* localerrorstatus - Bits[18:18], RO, default = 1'b0  */
    UINT32 viralstatustokti0 : 1;
    /* viralstatustokti0 - Bits[19:19], RO, default = 1'b0  */
    UINT32 viralstatustokti1 : 1;
    /* viralstatustokti1 - Bits[20:20], RO, default = 1'b0  */
    UINT32 viralstatusfromkti0 : 1;
    /* viralstatusfromkti0 - Bits[21:21], RO, default = 1'b0  */
    UINT32 viralstatusfromkti1 : 1;
    /* viralstatusfromkti1 - Bits[22:22], RO, default = 1'b0  */
    UINT32 fatalstatusfromkti0 : 1;
    /* fatalstatusfromkti0 - Bits[23:23], RO, default = 1'b0  */
    UINT32 fatalstatusfromkti1 : 1;
    /* fatalstatusfromkti1 - Bits[24:24], RO, default = 1'b0  */
    UINT32 fatalstatustopcu : 1;
    /* fatalstatustopcu - Bits[25:25], RO, default = 1'b0  */
    UINT32 rsvd_26 : 2;
    /* rsvd_26 - Bits[27:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ktirxerrtocts : 1;
    /* ktirxerrtocts - Bits[28:28], RW, default = 1'b1  */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3GLERRCFG_M3KTI_MAIN_STRUCT;


/* M3KIGRVNSEL0_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x403900B8)                                                  */
/*       SKX (0x403900B8)                                                     */
/* Register default value:              0x00000000                            */
#define M3KIGRVNSEL0_M3KTI_MAIN_REG 0x0A0040B8
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * The table assigns the locally generated Intel UPI packets to VN0 or VN1 (Port0)
 */
typedef union {
  struct {
    UINT32 igrvnswtchtable0 : 8;
    /* igrvnswtchtable0 - Bits[7:0], RW, default = 8'h00 
       
       This table assigns the locally generated Intel UPI packets to VN0 or VN1 
       dependant on DNID.  Table is indexed based on DNID[2:0] and output in VN0 or 
       VN1. 
       Bit - DNID[2:0] - VN
       0 - 0x0 - VN0 or VN1
       1 - 0x1 - VN0 or VN1
       ...
       7 - 0x7 - VN0 or VN1
       Note: in past products this able also assigned Route-Through traffic VN, but 
       that functionality was removed in SKX. 
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KIGRVNSEL0_M3KTI_MAIN_STRUCT;


/* M3KIGRVNSEL1_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x403900BC)                                                  */
/*       SKX (0x403900BC)                                                     */
/* Register default value:              0x00000000                            */
#define M3KIGRVNSEL1_M3KTI_MAIN_REG 0x0A0040BC
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * The table assigns the locally generated Intel UPI packets to VN0 or VN1 (Port1)
 */
typedef union {
  struct {
    UINT32 igrvnswtchtable1 : 8;
    /* igrvnswtchtable1 - Bits[7:0], RW, default = 8'h00 
       
       This table assigns the locally generated Intel UPI packets to VN0 or VN1 
       dependant on DNID.  Table is indexed based on DNID[2:0] and output in VN0 or 
       VN1. 
       Bit - DNID[2:0] - VN
       0 - 0x0 - VN0 or VN1
       1 - 0x1 - VN0 or VN1
       ...
       7 - 0x7 - VN0 or VN1
       Note: in past products this able also assigned Route-Through traffic VN, but 
       that functionality was removed in the processor 
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KIGRVNSEL1_M3KTI_MAIN_STRUCT;






/* M3EFQPRIVC_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x403900C8)                                                  */
/*       SKX (0x403900C8)                                                     */
/* Register default value:              0x00162A00                            */
#define M3EFQPRIVC_M3KTI_MAIN_REG 0x0A0040C8
/* Struct format extracted from XML file SKX_A0\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 disable_5flit : 1;
    /* disable_5flit - Bits[0:0], RWS_L, default = 1'b0  */
    UINT32 rsvd_1 : 7;
    /* rsvd_1 - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 adflowqueuedepth : 6;
    /* adflowqueuedepth - Bits[13:8], RWS_L, default = 6'd42  */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 blflowqueuedepth : 5;
    /* blflowqueuedepth - Bits[20:16], RWS_L, default = 5'd22  */
    UINT32 adusepurerrforarb : 1;
    /* adusepurerrforarb - Bits[21:21], RWS_L, default = 1'b0  */
    UINT32 blusepurerrforarb : 1;
    /* blusepurerrforarb - Bits[22:22], RWS_L, default = 1'b0  */
    UINT32 akusepurerrforarb : 1;
    /* akusepurerrforarb - Bits[23:23], RWS_L, default = 1'b0  */
    UINT32 crittercsrckgatedis : 1;
    /* crittercsrckgatedis - Bits[24:24], RWS, default = 1'b0  */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3EFQPRIVC_M3KTI_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 disable_5flit : 1;
    /* disable_5flit - Bits[0:0], RWS_L, default = 1'b0  */
    UINT32 rsvd_1 : 7;
    /* rsvd_1 - Bits[7:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 adflowqueuedepth : 6;
    /* adflowqueuedepth - Bits[13:8], RWS_L, default = 6'd42  */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 blflowqueuedepth : 5;
    /* blflowqueuedepth - Bits[20:16], RWS_L, default = 5'd22  */
    UINT32 adusepurerrforarb : 1;
    /* adusepurerrforarb - Bits[21:21], RWS_L, default = 1'b0  */
    UINT32 blusepurerrforarb : 1;
    /* blusepurerrforarb - Bits[22:22], RWS_L, default = 1'b0  */
    UINT32 akusepurerrforarb : 1;
    /* akusepurerrforarb - Bits[23:23], RWS_L, default = 1'b0  */
    UINT32 crittercsrckgatedis : 1;
    /* crittercsrckgatedis - Bits[24:24], RWS, default = 1'b0  */
    UINT32 disableclkgatingbgfcrdtrk : 1;
    /* disableclkgatingbgfcrdtrk - Bits[25:25], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgatingblfqwb : 1;
    /* kti0disableclkgatingblfqwb - Bits[26:26], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgatingblfqwb : 1;
    /* kti1disableclkgatingblfqwb - Bits[27:27], RWS_L, default = 1'd0  */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3EFQPRIVC_M3KTI_MAIN_STRUCT;



/* M3EFQPRIVC1_M3KTI_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x403900CC)                                                  */
/*       SKX (0x403900CC)                                                     */
/* Register default value:              0x0000000C                            */
#define M3EFQPRIVC1_M3KTI_MAIN_REG 0x0A0040CC
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 bgfmaxcreditcount : 4;
    /* bgfmaxcreditcount - Bits[3:0], RWS, default = 4'd12 
       For normal operation, the domain crossing FIFO between the UPI ingress logic and 
       link layer has 12 entries. This field may be used to artificially limit the size 
       of this FIFO. 
     */
    UINT32 flitsinbgfifothreshold : 4;
    /* flitsinbgfifothreshold - Bits[7:4], RWS, default = 4'd0  */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3EFQPRIVC1_M3KTI_MAIN_STRUCT;


/* M3EFQPRIVC2_M3KTI_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x403900D0)                                                  */
/*       SKX (0x403900D0)                                                     */
/* Register default value:              0x00000000                            */
#define M3EFQPRIVC2_M3KTI_MAIN_REG 0x0A0040D0
/* Struct format extracted from XML file SKX_A0\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti0disableclkgated2kcmpfifo : 1;
    /* kti0disableclkgated2kcmpfifo - Bits[0:0], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgateakegrq : 1;
    /* kti0disableclkgateakegrq - Bits[1:1], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgatecreditreturn : 1;
    /* kti0disableclkgatecreditreturn - Bits[2:2], RWS_L, default = 1'd0  */
    UINT32 kti0disableadclkgatedecode : 1;
    /* kti0disableadclkgatedecode - Bits[3:3], RWS_L, default = 1'd0  */
    UINT32 kti0disableadclkgatearb : 1;
    /* kti0disableadclkgatearb - Bits[4:4], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingadegrlistcoarse : 1;
    /* kti0disablepwrgatingadegrlistcoarse - Bits[5:5], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingadegrlistfine : 1;
    /* kti0disablepwrgatingadegrlistfine - Bits[6:6], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingadegrflistfine : 1;
    /* kti0disablepwrgatingadegrflistfine - Bits[7:7], RWS_L, default = 1'd0  */
    UINT32 rsvd_8 : 4;
    /* rsvd_8 - Bits[11:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti0disableblclkgatedecode : 1;
    /* kti0disableblclkgatedecode - Bits[12:12], RWS_L, default = 1'd0  */
    UINT32 kti0disableblclkgatearb : 1;
    /* kti0disableblclkgatearb - Bits[13:13], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingblegrlistcoarse : 1;
    /* kti0disablepwrgatingblegrlistcoarse - Bits[14:14], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingblegrlistfine : 1;
    /* kti0disablepwrgatingblegrlistfine - Bits[15:15], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgated2kcmpfifo : 1;
    /* kti1disableclkgated2kcmpfifo - Bits[16:16], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgateakegrq : 1;
    /* kti1disableclkgateakegrq - Bits[17:17], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgatecreditreturn : 1;
    /* kti1disableclkgatecreditreturn - Bits[18:18], RWS_L, default = 1'd0  */
    UINT32 kti1disableadclkgatedecode : 1;
    /* kti1disableadclkgatedecode - Bits[19:19], RWS_L, default = 1'd0  */
    UINT32 kti1disableadclkgatearb : 1;
    /* kti1disableadclkgatearb - Bits[20:20], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingadegrlistcoarse : 1;
    /* kti1disablepwrgatingadegrlistcoarse - Bits[21:21], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingadegrlistfine : 1;
    /* kti1disablepwrgatingadegrlistfine - Bits[22:22], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingadegrflistfine : 1;
    /* kti1disablepwrgatingadegrflistfine - Bits[23:23], RWS_L, default = 1'd0  */
    UINT32 rsvd_24 : 4;
    /* rsvd_24 - Bits[27:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti1disableblclkgatedecode : 1;
    /* kti1disableblclkgatedecode - Bits[28:28], RWS_L, default = 1'd0  */
    UINT32 kti1disableblclkgatearb : 1;
    /* kti1disableblclkgatearb - Bits[29:29], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingblegrlistcoarse : 1;
    /* kti1disablepwrgatingblegrlistcoarse - Bits[30:30], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingblegrlistfine : 1;
    /* kti1disablepwrgatingblegrlistfine - Bits[31:31], RWS_L, default = 1'd0  */
  } Bits;
  UINT32 Data;
} M3EFQPRIVC2_M3KTI_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti0disableclkgated2kcmpfifo : 1;
    /* kti0disableclkgated2kcmpfifo - Bits[0:0], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgateakegrq : 1;
    /* kti0disableclkgateakegrq - Bits[1:1], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgatecreditreturn : 1;
    /* kti0disableclkgatecreditreturn - Bits[2:2], RWS_L, default = 1'd0  */
    UINT32 kti0disableadclkgatedecode : 1;
    /* kti0disableadclkgatedecode - Bits[3:3], RWS_L, default = 1'd0  */
    UINT32 kti0disableadclkgatearb : 1;
    /* kti0disableadclkgatearb - Bits[4:4], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingadegrlistcoarse : 1;
    /* kti0disablepwrgatingadegrlistcoarse - Bits[5:5], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingadegrlistfine : 1;
    /* kti0disablepwrgatingadegrlistfine - Bits[6:6], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingadegrflistfine : 1;
    /* kti0disablepwrgatingadegrflistfine - Bits[7:7], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgating2scmsadcrdtrk : 1;
    /* kti0disableclkgating2scmsadcrdtrk - Bits[8:8], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgating2scmsakcrdtrk : 1;
    /* kti0disableclkgating2scmsakcrdtrk - Bits[9:9], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgating2scmsblcrdtrk : 1;
    /* kti0disableclkgating2scmsblcrdtrk - Bits[10:10], RWS_L, default = 1'd0  */
    UINT32 kti0disableclkgating2sblm2pcrdtrk : 1;
    /* kti0disableclkgating2sblm2pcrdtrk - Bits[11:11], RWS_L, default = 1'd0  */
    UINT32 kti0disableblclkgatedecode : 1;
    /* kti0disableblclkgatedecode - Bits[12:12], RWS_L, default = 1'd0  */
    UINT32 kti0disableblclkgatearb : 1;
    /* kti0disableblclkgatearb - Bits[13:13], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingblegrlistcoarse : 1;
    /* kti0disablepwrgatingblegrlistcoarse - Bits[14:14], RWS_L, default = 1'd0  */
    UINT32 kti0disablepwrgatingblegrlistfine : 1;
    /* kti0disablepwrgatingblegrlistfine - Bits[15:15], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgated2kcmpfifo : 1;
    /* kti1disableclkgated2kcmpfifo - Bits[16:16], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgateakegrq : 1;
    /* kti1disableclkgateakegrq - Bits[17:17], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgatecreditreturn : 1;
    /* kti1disableclkgatecreditreturn - Bits[18:18], RWS_L, default = 1'd0  */
    UINT32 kti1disableadclkgatedecode : 1;
    /* kti1disableadclkgatedecode - Bits[19:19], RWS_L, default = 1'd0  */
    UINT32 kti1disableadclkgatearb : 1;
    /* kti1disableadclkgatearb - Bits[20:20], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingadegrlistcoarse : 1;
    /* kti1disablepwrgatingadegrlistcoarse - Bits[21:21], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingadegrlistfine : 1;
    /* kti1disablepwrgatingadegrlistfine - Bits[22:22], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingadegrflistfine : 1;
    /* kti1disablepwrgatingadegrflistfine - Bits[23:23], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgating2scmsadcrdtrk : 1;
    /* kti1disableclkgating2scmsadcrdtrk - Bits[24:24], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgating2scmsakcrdtrk : 1;
    /* kti1disableclkgating2scmsakcrdtrk - Bits[25:25], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgating2scmsblcrdtrk : 1;
    /* kti1disableclkgating2scmsblcrdtrk - Bits[26:26], RWS_L, default = 1'd0  */
    UINT32 kti1disableclkgating2sblm2pcrdtrk : 1;
    /* kti1disableclkgating2sblm2pcrdtrk - Bits[27:27], RWS_L, default = 1'd0  */
    UINT32 kti1disableblclkgatedecode : 1;
    /* kti1disableblclkgatedecode - Bits[28:28], RWS_L, default = 1'd0  */
    UINT32 kti1disableblclkgatearb : 1;
    /* kti1disableblclkgatearb - Bits[29:29], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingblegrlistcoarse : 1;
    /* kti1disablepwrgatingblegrlistcoarse - Bits[30:30], RWS_L, default = 1'd0  */
    UINT32 kti1disablepwrgatingblegrlistfine : 1;
    /* kti1disablepwrgatingblegrlistfine - Bits[31:31], RWS_L, default = 1'd0  */
  } Bits;
  UINT32 Data;
} M3EFQPRIVC2_M3KTI_MAIN_STRUCT;



/* M3EFQCTRL_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x403900D4)                                                  */
/*       SKX (0x403900D4)                                                     */
/* Register default value on SKX_A0:    0x00000000                            */
/* Register default value on SKX:       0x00000100                            */
#define M3EFQCTRL_M3KTI_MAIN_REG 0x0A0040D4
/* Struct format extracted from XML file SKX_A0\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 pe_mode : 1;
    /* pe_mode - Bits[0:0], RW, default = 1'b0 
       If set, we copy PE bit to and from DNID[3]. 
       
     */
    UINT32 ak_arb_mode : 2;
    /* ak_arb_mode - Bits[2:1], RWS, default = 2'b00  */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3EFQCTRL_M3KTI_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 pe_mode : 1;
    /* pe_mode - Bits[0:0], RW, default = 1'b0 
       If set, we copy PE bit to and from DNID[3]. 
       
     */
    UINT32 ak_arb_mode : 2;
    /* ak_arb_mode - Bits[2:1], RWS, default = 2'b00  */
    UINT32 blfqwbroutethruwaitforwbvncrd : 1;
    /* blfqwbroutethruwaitforwbvncrd - Bits[3:3], RWS, default = 1'b0  */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ktinonsnprdpfdis : 1;
    /* ktinonsnprdpfdis - Bits[8:8], RWS_LB, default = 1'b1  */
    UINT32 rsvd_9 : 23;
    /* rsvd_9 - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3EFQCTRL_M3KTI_MAIN_STRUCT;



/* M3EFQPRIVC3_M3KTI_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x403900D8)                                                  */
/*       SKX (0x403900D8)                                                     */
/* Register default value:              0x00000000                            */
#define M3EFQPRIVC3_M3KTI_MAIN_REG 0x0A0040D8
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 spare3 : 32;
    /* spare3 - Bits[31:0], RWS_L, default = 32'b0  */
  } Bits;
  UINT32 Data;
} M3EFQPRIVC3_M3KTI_MAIN_STRUCT;


/* M3EFQPRIVC4_M3KTI_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x403900DC)                                                  */
/*       SKX (0x403900DC)                                                     */
/* Register default value:              0x00000000                            */
#define M3EFQPRIVC4_M3KTI_MAIN_REG 0x0A0040DC
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 spare4 : 32;
    /* spare4 - Bits[31:0], RWS_L, default = 32'b0  */
  } Bits;
  UINT32 Data;
} M3EFQPRIVC4_M3KTI_MAIN_STRUCT;


/* M3KPRECTRL_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x403900E0)                                                  */
/*       SKX (0x403900E0)                                                     */
/* Register default value:              0x00000040                            */
#define M3KPRECTRL_M3KTI_MAIN_REG 0x0A0040E0
/* Struct format extracted from XML file SKX_A0\3.18.0.CFG.xml.
 * Prefetch CTRL.
 */
typedef union {
  struct {
    UINT32 mini_sad_mode : 3;
    /* mini_sad_mode - Bits[2:0], RWS, default = 3'b000  */
    UINT32 mini_sad_mod3_enable : 1;
    /* mini_sad_mod3_enable - Bits[3:3], RWS, default = 1'b0  */
    UINT32 rsvd_4 : 1;
    /* rsvd_4 - Bits[4:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 remspecrdthreshold : 4;
    /* remspecrdthreshold - Bits[8:5], RWS, default = 4'b0010 
       4b value indicating the AD egress credit threshold that is used to determine if 
       we can spawn a remote speculative read message to memory. The value indicates 
       the number of AD txns that must be in the AD egress before allowing a remote 
       speculative read spawn. Value of 9 and above disables spawning prefetches and a 
       value of 0 and 1 are not allowed 
     */
    UINT32 rsvd_9 : 23;
    /* rsvd_9 - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KPRECTRL_M3KTI_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Prefetch CTRL.
 */
typedef union {
  struct {
    UINT32 mini_sad_mode : 3;
    /* mini_sad_mode - Bits[2:0], RWS_LB, default = 3'b000  */
    UINT32 mini_sad_mod3_enable : 1;
    /* mini_sad_mod3_enable - Bits[3:3], RWS_LB, default = 1'b0  */
    UINT32 rsvd_4 : 1;
    /* rsvd_4 - Bits[4:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 remspecrdthreshold : 4;
    /* remspecrdthreshold - Bits[8:5], RWS_LB, default = 4'b0010 
       4b value indicating the AD egress credit threshold that is used to determine if 
       we can spawn a remote speculative read message to memory. The value indicates 
       the number of AD txns that must be in the AD egress before allowing a remote 
       speculative read spawn. Value of 9 and above disables spawning prefetches and a 
       value of 0 and 1 are not allowed 
     */
    UINT32 rsvd_9 : 3;
    /* rsvd_9 - Bits[11:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 disslot1pfwhenslot0pktvalid : 1;
    /* disslot1pfwhenslot0pktvalid - Bits[12:12], RWS_LB, default = 1'b0 
       Disable the bug fix for b305779 (revert back to A0 behavior). Note: with the fix 
       for b307074 this de-feature doesn't really work anymore as intended 
     */
    UINT32 rsvd_13 : 19;
    /* rsvd_13 - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KPRECTRL_M3KTI_MAIN_STRUCT;



/* M3KPRETL_M3KTI_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x403900E4)                                                  */
/*       SKX (0x403900E4)                                                     */
/* Register default value:              0x00000000                            */
#define M3KPRETL_M3KTI_MAIN_REG 0x0A0040E4
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Prefetch Target List
 */
typedef union {
  struct {
    UINT32 mini_sad_targets : 32;
    /* mini_sad_targets - Bits[31:0], RWS_LB, default = 32'h00000000  */
  } Bits;
  UINT32 Data;
} M3KPRETL_M3KTI_MAIN_STRUCT;


/* M3KLL_TX_CRD_P0_M3KTI_MAIN_REG supported on:                               */
/*       SKX_A0 (0x403900F0)                                                  */
/*       SKX (0x403900F0)                                                     */
/* Register default value:              0x00000000                            */
#define M3KLL_TX_CRD_P0_M3KTI_MAIN_REG 0x0A0040F0
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Intel UPI Link Transmit Credit Status Port0. This registers is defined in Intel 
 * UPI specification 
 */
typedef union {
  struct {
    UINT32 vna_crd : 8;
    /* vna_crd - Bits[7:0], RO_V, default = 8'b0 
       VNA Credits Available for use on Tx
     */
    UINT32 vn0_req_crd : 1;
    /* vn0_req_crd - Bits[8:8], RO_V, default = 1'b0 
       VN0 Request Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_snp_crd : 1;
    /* vn0_snp_crd - Bits[9:9], RO_V, default = 1'b0 
       VN0 Snoop Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_rsp_crd : 1;
    /* vn0_rsp_crd - Bits[10:10], RO_V, default = 1'b0 
       VN0 Response Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_wb_crd : 1;
    /* vn0_wb_crd - Bits[11:11], RO_V, default = 1'b0 
       VN0 Write Back Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_ncb_crd : 1;
    /* vn0_ncb_crd - Bits[12:12], RO_V, default = 1'b0 
       VN0 Non-Coherent Bypass Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_ncs_crd : 1;
    /* vn0_ncs_crd - Bits[13:13], RO_V, default = 1'b0 
       VN0 Non-Coherent Standard Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_req_crd : 1;
    /* vn1_req_crd - Bits[14:14], RO_V, default = 1'b0 
       VN1 Request Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_snp_crd : 1;
    /* vn1_snp_crd - Bits[15:15], RO_V, default = 1'b0 
       VN1 Snoop Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_rsp_crd : 1;
    /* vn1_rsp_crd - Bits[16:16], RO_V, default = 1'b0 
       VN1 Response Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_wb_crd : 1;
    /* vn1_wb_crd - Bits[17:17], RO_V, default = 1'b0 
       VN1 Write Back Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_ncb_crd : 1;
    /* vn1_ncb_crd - Bits[18:18], RO_V, default = 1'b0 
       VN1 Non-Coherent Bypass Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_ncs_crd : 1;
    /* vn1_ncs_crd - Bits[19:19], RO_V, default = 1'b0 
       VN1 Non-Coherent Standard Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KLL_TX_CRD_P0_M3KTI_MAIN_STRUCT;


/* M3KLL_TX_CRD_P1_M3KTI_MAIN_REG supported on:                               */
/*       SKX_A0 (0x403900F4)                                                  */
/*       SKX (0x403900F4)                                                     */
/* Register default value:              0x00000000                            */
#define M3KLL_TX_CRD_P1_M3KTI_MAIN_REG 0x0A0040F4
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Intel UPI Link Transmit Credit Status Port1. This registers is defined in Intel 
 * UPI specification 
 */
typedef union {
  struct {
    UINT32 vna_crd : 8;
    /* vna_crd - Bits[7:0], RO_V, default = 8'b0 
       VNA Credits Available for use on Tx
     */
    UINT32 vn0_req_crd : 1;
    /* vn0_req_crd - Bits[8:8], RO_V, default = 1'b0 
       VN0 Request Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_snp_crd : 1;
    /* vn0_snp_crd - Bits[9:9], RO_V, default = 1'b0 
       VN0 Snoop Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_rsp_crd : 1;
    /* vn0_rsp_crd - Bits[10:10], RO_V, default = 1'b0 
       VN0 Response Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_wb_crd : 1;
    /* vn0_wb_crd - Bits[11:11], RO_V, default = 1'b0 
       VN0 Write Back Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_ncb_crd : 1;
    /* vn0_ncb_crd - Bits[12:12], RO_V, default = 1'b0 
       VN0 Non-Coherent Bypass Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn0_ncs_crd : 1;
    /* vn0_ncs_crd - Bits[13:13], RO_V, default = 1'b0 
       VN0 Non-Coherent Standard Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_req_crd : 1;
    /* vn1_req_crd - Bits[14:14], RO_V, default = 1'b0 
       VN1 Request Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_snp_crd : 1;
    /* vn1_snp_crd - Bits[15:15], RO_V, default = 1'b0 
       VN1 Snoop Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_rsp_crd : 1;
    /* vn1_rsp_crd - Bits[16:16], RO_V, default = 1'b0 
       VN1 Response Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_wb_crd : 1;
    /* vn1_wb_crd - Bits[17:17], RO_V, default = 1'b0 
       VN1 Write Back Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_ncb_crd : 1;
    /* vn1_ncb_crd - Bits[18:18], RO_V, default = 1'b0 
       VN1 Non-Coherent Bypass Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 vn1_ncs_crd : 1;
    /* vn1_ncs_crd - Bits[19:19], RO_V, default = 1'b0 
       VN1 Non-Coherent Standard Credits Available for use on Tx
       0 - 0 Credits
       1 - 1+ Credits
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KLL_TX_CRD_P1_M3KTI_MAIN_STRUCT;


/* PXPENHCAP_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390100)                                                  */
/*       SKX (0x40390100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_M3KTI_MAIN_REG 0x0A004100
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * PCI Express Enhanced Capability Register
 */
typedef union {
  struct {
    UINT32 capability_id : 16;
    /* capability_id - Bits[15:0], RO, default = 16'b0 
       There is no capability at this location
     */
    UINT32 capability_version : 4;
    /* capability_version - Bits[19:16], RO, default = 4'b0 
       There is no capability at this location
     */
    UINT32 next_capability_offset : 12;
    /* next_capability_offset - Bits[31:20], RO, default = 12'b0 
       Pointer to the next capability in the enhanced configuration space. Set to 0 to 
       indicate there are no more capability structures. 
     */
  } Bits;
  UINT32 Data;
} PXPENHCAP_M3KTI_MAIN_STRUCT;








/* M3EGRERRMSK0123_M3KTI_MAIN_REG supported on:                               */
/*       SKX_A0 (0x40390118)                                                  */
/*       SKX (0x40390118)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRMSK0123_M3KTI_MAIN_REG 0x0A004118
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 akflowqunderflowmask : 1;
    /* akflowqunderflowmask - Bits[0:0], RWS, default = 1'b0 
        
     */
    UINT32 akflowqoverflowmask : 1;
    /* akflowqoverflowmask - Bits[1:1], RWS, default = 1'b0 
        
     */
    UINT32 blflowqunderflowmask : 1;
    /* blflowqunderflowmask - Bits[2:2], RWS, default = 1'b0 
        
     */
    UINT32 blflowqoverflowmask : 1;
    /* blflowqoverflowmask - Bits[3:3], RWS, default = 1'b0 
        
     */
    UINT32 adflowqunderflowmask : 1;
    /* adflowqunderflowmask - Bits[4:4], RWS, default = 1'b0 
        
     */
    UINT32 adflowqoverflowmask : 1;
    /* adflowqoverflowmask - Bits[5:5], RWS, default = 1'b0 
        
     */
    UINT32 blm2pncscreditunderflowmask : 1;
    /* blm2pncscreditunderflowmask - Bits[6:6], RWS, default = 1'b0 
        
     */
    UINT32 blm2pncbcreditunderflowmask : 1;
    /* blm2pncbcreditunderflowmask - Bits[7:7], RWS, default = 1'b0 
        
     */
    UINT32 blm2pncscreditoverflowmask : 1;
    /* blm2pncscreditoverflowmask - Bits[8:8], RWS, default = 1'b0 
        
     */
    UINT32 blm2pncbcreditoverflowmask : 1;
    /* blm2pncbcreditoverflowmask - Bits[9:9], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn1port1creditunderflowmask : 1;
    /* blrtvn1port1creditunderflowmask - Bits[10:10], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn1port1creditoverflowmask : 1;
    /* blrtvn1port1creditoverflowmask - Bits[11:11], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn1port20creditunderflowmask : 1;
    /* blrtvn1port20creditunderflowmask - Bits[12:12], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn1port20creditoverflowmask : 1;
    /* blrtvn1port20creditoverflowmask - Bits[13:13], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn0port1creditunderflowmask : 1;
    /* blrtvn0port1creditunderflowmask - Bits[14:14], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn0port1creditoverflowmask : 1;
    /* blrtvn0port1creditoverflowmask - Bits[15:15], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn0port20creditunderflowmask : 1;
    /* blrtvn0port20creditunderflowmask - Bits[16:16], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn0port20creditoverflowmask : 1;
    /* blrtvn0port20creditoverflowmask - Bits[17:17], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn1port1creditunderflowmask : 1;
    /* adrtvn1port1creditunderflowmask - Bits[18:18], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn1port1creditoverflowmask : 1;
    /* adrtvn1port1creditoverflowmask - Bits[19:19], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn1port20creditunderflowmask : 1;
    /* adrtvn1port20creditunderflowmask - Bits[20:20], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn1port20creditoverflowmask : 1;
    /* adrtvn1port20creditoverflowmask - Bits[21:21], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn0port1creditunderflowmask : 1;
    /* adrtvn0port1creditunderflowmask - Bits[22:22], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn0port1creditoverflowmask : 1;
    /* adrtvn0port1creditoverflowmask - Bits[23:23], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn0port20creditunderflowmask : 1;
    /* adrtvn0port20creditunderflowmask - Bits[24:24], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn0port20creditoverflowmask : 1;
    /* adrtvn0port20creditoverflowmask - Bits[25:25], RWS, default = 1'b0 
        
     */
    UINT32 d2kcmpfifounderflowmask : 1;
    /* d2kcmpfifounderflowmask - Bits[26:26], RWS, default = 1'b0 
        
     */
    UINT32 d2kcmpfifooverflowmask : 1;
    /* d2kcmpfifooverflowmask - Bits[27:27], RWS, default = 1'b0 
        
     */
    UINT32 cmscreditunderflowmask : 1;
    /* cmscreditunderflowmask - Bits[28:28], RWS, default = 1'b0 
        
     */
    UINT32 cmscreditoverflowmask : 1;
    /* cmscreditoverflowmask - Bits[29:29], RWS, default = 1'b0 
        
     */
    UINT32 adchacreditunderflowmask : 1;
    /* adchacreditunderflowmask - Bits[30:30], RWS, default = 1'b0 
        
     */
    UINT32 adchacreditoverflowmask : 1;
    /* adchacreditoverflowmask - Bits[31:31], RWS, default = 1'b0 
        
     */
  } Bits;
  UINT32 Data;
} M3EGRERRMSK0123_M3KTI_MAIN_STRUCT;


/* M3EGRERRMSK4567_M3KTI_MAIN_REG supported on:                               */
/*       SKX_A0 (0x4039011C)                                                  */
/*       SKX (0x4039011C)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRMSK4567_M3KTI_MAIN_REG 0x0A00411C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 akflowqunderflowmask : 1;
    /* akflowqunderflowmask - Bits[0:0], RWS, default = 1'b0 
        
     */
    UINT32 akflowqoverflowmask : 1;
    /* akflowqoverflowmask - Bits[1:1], RWS, default = 1'b0 
        
     */
    UINT32 blflowqunderflowmask : 1;
    /* blflowqunderflowmask - Bits[2:2], RWS, default = 1'b0 
        
     */
    UINT32 blflowqoverflowmask : 1;
    /* blflowqoverflowmask - Bits[3:3], RWS, default = 1'b0 
        
     */
    UINT32 adflowqunderflowmask : 1;
    /* adflowqunderflowmask - Bits[4:4], RWS, default = 1'b0 
        
     */
    UINT32 adflowqoverflowmask : 1;
    /* adflowqoverflowmask - Bits[5:5], RWS, default = 1'b0 
        
     */
    UINT32 blm2pncscreditunderflowmask : 1;
    /* blm2pncscreditunderflowmask - Bits[6:6], RWS, default = 1'b0 
        
     */
    UINT32 blm2pncbcreditunderflowmask : 1;
    /* blm2pncbcreditunderflowmask - Bits[7:7], RWS, default = 1'b0 
        
     */
    UINT32 blm2pncscreditoverflowmask : 1;
    /* blm2pncscreditoverflowmask - Bits[8:8], RWS, default = 1'b0 
        
     */
    UINT32 blm2pncbcreditoverflowmask : 1;
    /* blm2pncbcreditoverflowmask - Bits[9:9], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn1port0creditunderflowmask : 1;
    /* blrtvn1port0creditunderflowmask - Bits[10:10], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn1port0creditoverflowmask : 1;
    /* blrtvn1port0creditoverflowmask - Bits[11:11], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn1port2creditunderflowmask : 1;
    /* blrtvn1port2creditunderflowmask - Bits[12:12], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn1port2creditoverflowmask : 1;
    /* blrtvn1port2creditoverflowmask - Bits[13:13], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn0port0creditunderflowmask : 1;
    /* blrtvn0port0creditunderflowmask - Bits[14:14], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn0port0creditoverflowmask : 1;
    /* blrtvn0port0creditoverflowmask - Bits[15:15], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn0port2creditunderflowmask : 1;
    /* blrtvn0port2creditunderflowmask - Bits[16:16], RWS, default = 1'b0 
        
     */
    UINT32 blrtvn0port2creditoverflowmask : 1;
    /* blrtvn0port2creditoverflowmask - Bits[17:17], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn1port0creditunderflowmask : 1;
    /* adrtvn1port0creditunderflowmask - Bits[18:18], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn1port0creditoverflowmask : 1;
    /* adrtvn1port0creditoverflowmask - Bits[19:19], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn1port2creditunderflowmask : 1;
    /* adrtvn1port2creditunderflowmask - Bits[20:20], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn1port2creditoverflowmask : 1;
    /* adrtvn1port2creditoverflowmask - Bits[21:21], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn0port0creditunderflowmask : 1;
    /* adrtvn0port0creditunderflowmask - Bits[22:22], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn0port0creditoverflowmask : 1;
    /* adrtvn0port0creditoverflowmask - Bits[23:23], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn0port2creditunderflowmask : 1;
    /* adrtvn0port2creditunderflowmask - Bits[24:24], RWS, default = 1'b0 
        
     */
    UINT32 adrtvn0port2creditoverflowmask : 1;
    /* adrtvn0port2creditoverflowmask - Bits[25:25], RWS, default = 1'b0 
        
     */
    UINT32 d2kcmpfifounderflowmask : 1;
    /* d2kcmpfifounderflowmask - Bits[26:26], RWS, default = 1'b0 
        
     */
    UINT32 d2kcmpfifooverflowmask : 1;
    /* d2kcmpfifooverflowmask - Bits[27:27], RWS, default = 1'b0 
        
     */
    UINT32 cmscreditunderflowmask : 1;
    /* cmscreditunderflowmask - Bits[28:28], RWS, default = 1'b0 
        
     */
    UINT32 cmscreditoverflowmask : 1;
    /* cmscreditoverflowmask - Bits[29:29], RWS, default = 1'b0 
        
     */
    UINT32 adchacreditunderflowmask : 1;
    /* adchacreditunderflowmask - Bits[30:30], RWS, default = 1'b0 
        
     */
    UINT32 adchacreditoverflowmask : 1;
    /* adchacreditoverflowmask - Bits[31:31], RWS, default = 1'b0 
       Port1
     */
  } Bits;
  UINT32 Data;
} M3EGRERRMSK4567_M3KTI_MAIN_STRUCT;


/* M3EGRERRLOG0_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390120)                                                  */
/*       SKX (0x40390120)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRLOG0_M3KTI_MAIN_REG 0x0A004120
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Egress Error Logging.
 */
typedef union {
  struct {
    UINT32 d2kcmpfifounderflow : 1;
    /* d2kcmpfifounderflow - Bits[0:0], RW1CS, default = 1'b0  */
    UINT32 d2kcmpfifooverflow : 1;
    /* d2kcmpfifooverflow - Bits[1:1], RW1CS, default = 1'b0  */
    UINT32 akcmscreditunderflow : 1;
    /* akcmscreditunderflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 blcmscreditunderflow : 1;
    /* blcmscreditunderflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 adcmscreditunderflow : 1;
    /* adcmscreditunderflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 akcmscreditoverflow : 1;
    /* akcmscreditoverflow - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 blcmscreditoverflow : 1;
    /* blcmscreditoverflow - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 adcmscreditoverflow : 1;
    /* adcmscreditoverflow - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 adchasliceidcreditunderflow : 5;
    /* adchasliceidcreditunderflow - Bits[12:8], RW1CS, default = 5'b0  */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 adchawbcreditunderflow : 1;
    /* adchawbcreditunderflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 adchasnpcreditunderflow : 1;
    /* adchasnpcreditunderflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 adchareqcreditunderflow : 1;
    /* adchareqcreditunderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 adchashrcreditunderflow : 1;
    /* adchashrcreditunderflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 adchasliceidcreditoverflow : 5;
    /* adchasliceidcreditoverflow - Bits[24:20], RW1CS, default = 5'b0  */
    UINT32 rsvd_25 : 3;
    /* rsvd_25 - Bits[27:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 adchawbcreditoverflow : 1;
    /* adchawbcreditoverflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 adchasnpcreditoverflow : 1;
    /* adchasnpcreditoverflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 adchareqcreditoverflow : 1;
    /* adchareqcreditoverflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 adchashrcreditoverflow : 1;
    /* adchashrcreditoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3EGRERRLOG0_M3KTI_MAIN_STRUCT;


/* M3EGRERRLOG1_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390124)                                                  */
/*       SKX (0x40390124)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRLOG1_M3KTI_MAIN_REG 0x0A004124
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Egress Error Logging.
 */
typedef union {
  struct {
    UINT32 kti0parerrhdrrfing : 1;
    /* kti0parerrhdrrfing - Bits[0:0], RW1CS, default = 1'b0  */
    UINT32 kti0parerre2ebeing : 1;
    /* kti0parerre2ebeing - Bits[1:1], RW1CS, default = 1'b0  */
    UINT32 kti0parerre2edataing : 1;
    /* kti0parerre2edataing - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 rsvd : 1;
    /* rsvd - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 adrtvn1port1rspcreditunderflow : 1;
    /* adrtvn1port1rspcreditunderflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port1snpcreditunderflow : 1;
    /* adrtvn1port1snpcreditunderflow - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port1reqcreditunderflow : 1;
    /* adrtvn1port1reqcreditunderflow - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port20rspcreditunderflow : 1;
    /* adrtvn1port20rspcreditunderflow - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port20snpcreditunderflow : 1;
    /* adrtvn1port20snpcreditunderflow - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port20reqcreditunderflow : 1;
    /* adrtvn1port20reqcreditunderflow - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port1rspcreditoverflow : 1;
    /* adrtvn1port1rspcreditoverflow - Bits[10:10], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port1snpcreditoverflow : 1;
    /* adrtvn1port1snpcreditoverflow - Bits[11:11], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port1reqcreditoverflow : 1;
    /* adrtvn1port1reqcreditoverflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port20rspcreditoverflow : 1;
    /* adrtvn1port20rspcreditoverflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port20snpcreditoverflow : 1;
    /* adrtvn1port20snpcreditoverflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port20reqcreditoverflow : 1;
    /* adrtvn1port20reqcreditoverflow - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port1rspcreditunderflow : 1;
    /* adrtvn0port1rspcreditunderflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port1snpcreditunderflow : 1;
    /* adrtvn0port1snpcreditunderflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port1reqcreditunderflow : 1;
    /* adrtvn0port1reqcreditunderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 adrtport1shrcreditunderflow : 1;
    /* adrtport1shrcreditunderflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port20rspcreditunderflow : 1;
    /* adrtvn0port20rspcreditunderflow - Bits[20:20], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port20snpcreditunderflow : 1;
    /* adrtvn0port20snpcreditunderflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port20reqcreditunderflow : 1;
    /* adrtvn0port20reqcreditunderflow - Bits[22:22], RW1CS, default = 1'b0  */
    UINT32 adrtport20shrcreditunderflow : 1;
    /* adrtport20shrcreditunderflow - Bits[23:23], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port1rspcreditoverflow : 1;
    /* adrtvn0port1rspcreditoverflow - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port1snpcreditoverflow : 1;
    /* adrtvn0port1snpcreditoverflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port1reqcreditoverflow : 1;
    /* adrtvn0port1reqcreditoverflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 adrtport1shrcreditoverflow : 1;
    /* adrtport1shrcreditoverflow - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port20rspcreditoverflow : 1;
    /* adrtvn0port20rspcreditoverflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port20snpcreditoverflow : 1;
    /* adrtvn0port20snpcreditoverflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port20reqcreditoverflow : 1;
    /* adrtvn0port20reqcreditoverflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 adrtport20shrcreditoverflow : 1;
    /* adrtport20shrcreditoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3EGRERRLOG1_M3KTI_MAIN_STRUCT;


/* M3EGRERRLOG2_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390128)                                                  */
/*       SKX (0x40390128)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRLOG2_M3KTI_MAIN_REG 0x0A004128
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd : 1;
    /* rsvd - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rbtfifounderflow : 1;
    /* rbtfifounderflow - Bits[1:1], RW1CS, default = 1'b0  */
    UINT32 rbtfifooverflow : 1;
    /* rbtfifooverflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port20rspcreditunderflow : 1;
    /* blrtvn1port20rspcreditunderflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port1ncscreditoverflow : 1;
    /* blrtvn1port1ncscreditoverflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port1ncbcreditoverflow : 1;
    /* blrtvn1port1ncbcreditoverflow - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port1wbcreditoverflow : 1;
    /* blrtvn1port1wbcreditoverflow - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port1rspcreditoverflow : 1;
    /* blrtvn1port1rspcreditoverflow - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port20ncscreditoverflow : 1;
    /* blrtvn1port20ncscreditoverflow - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port20ncbcreditoverflow : 1;
    /* blrtvn1port20ncbcreditoverflow - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port20wbcreditoverflow : 1;
    /* blrtvn1port20wbcreditoverflow - Bits[10:10], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port20rspcreditoverflow : 1;
    /* blrtvn1port20rspcreditoverflow - Bits[11:11], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port1ncscreditunderflow : 1;
    /* blrtvn0port1ncscreditunderflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port1ncbcreditunderflow : 1;
    /* blrtvn0port1ncbcreditunderflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port1wbcreditunderflow : 1;
    /* blrtvn0port1wbcreditunderflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port1rspcreditunderflow : 1;
    /* blrtvn0port1rspcreditunderflow - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 blrtport1shrcreditunderflow : 1;
    /* blrtport1shrcreditunderflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port20ncscreditunderflow : 1;
    /* blrtvn0port20ncscreditunderflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port20ncbcreditunderflow : 1;
    /* blrtvn0port20ncbcreditunderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port20wbcreditunderflow : 1;
    /* blrtvn0port20wbcreditunderflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port20rspcreditunderflow : 1;
    /* blrtvn0port20rspcreditunderflow - Bits[20:20], RW1CS, default = 1'b0  */
    UINT32 blrtport20shrcreditunderflow : 1;
    /* blrtport20shrcreditunderflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port1ncscreditoverflow : 1;
    /* blrtvn0port1ncscreditoverflow - Bits[22:22], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port1ncbcreditoverflow : 1;
    /* blrtvn0port1ncbcreditoverflow - Bits[23:23], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port1wbcreditoverflow : 1;
    /* blrtvn0port1wbcreditoverflow - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port1rspcreditoverflow : 1;
    /* blrtvn0port1rspcreditoverflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 blrtport1shrcreditoverflow : 1;
    /* blrtport1shrcreditoverflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port20ncscreditoverflow : 1;
    /* blrtvn0port20ncscreditoverflow - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port20ncbcreditoverflow : 1;
    /* blrtvn0port20ncbcreditoverflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port20wbcreditoverflow : 1;
    /* blrtvn0port20wbcreditoverflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port20rspcreditoverflow : 1;
    /* blrtvn0port20rspcreditoverflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 blrtport20shrcreditoverflow : 1;
    /* blrtport20shrcreditoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3EGRERRLOG2_M3KTI_MAIN_STRUCT;


/* M3EGRERRLOG3_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x4039012C)                                                  */
/*       SKX (0x4039012C)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRLOG3_M3KTI_MAIN_REG 0x0A00412C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd : 1;
    /* rsvd - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 bgfcreditunderflow : 1;
    /* bgfcreditunderflow - Bits[1:1], RW1CS, default = 1'b0  */
    UINT32 bgfcreditoverflow : 1;
    /* bgfcreditoverflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 akflowqunderflow : 1;
    /* akflowqunderflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 akflowqoverflow : 1;
    /* akflowqoverflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 blflowqmcunderflow : 3;
    /* blflowqmcunderflow - Bits[7:5], RW1CS, default = 3'b0  */
    UINT32 blflowqunderflow : 1;
    /* blflowqunderflow - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 blflowqoverflow : 1;
    /* blflowqoverflow - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 adflowqmcunderflow : 3;
    /* adflowqmcunderflow - Bits[12:10], RW1CS, default = 3'b0  */
    UINT32 adflowqunderflow : 1;
    /* adflowqunderflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 adflowqoverflow : 1;
    /* adflowqoverflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 blm2pidcreditunderflow : 3;
    /* blm2pidcreditunderflow - Bits[17:15], RW1CS, default = 3'b0  */
    UINT32 blm2pidcreditoverflow : 3;
    /* blm2pidcreditoverflow - Bits[20:18], RW1CS, default = 3'b0  */
    UINT32 blm2pncscreditunderflow : 1;
    /* blm2pncscreditunderflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 blm2pncbcreditunderflow : 1;
    /* blm2pncbcreditunderflow - Bits[22:22], RW1CS, default = 1'b0  */
    UINT32 blm2pncscreditoverflow : 1;
    /* blm2pncscreditoverflow - Bits[23:23], RW1CS, default = 1'b0  */
    UINT32 blm2pncbcreditoverflow : 1;
    /* blm2pncbcreditoverflow - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port1ncscreditunderflow : 1;
    /* blrtvn1port1ncscreditunderflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port1ncbcreditunderflow : 1;
    /* blrtvn1port1ncbcreditunderflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port1wbcreditunderflow : 1;
    /* blrtvn1port1wbcreditunderflow - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port1rspcreditunderflow : 1;
    /* blrtvn1port1rspcreditunderflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port20ncscreditunderflow : 1;
    /* blrtvn1port20ncscreditunderflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port20ncbcreditunderflow : 1;
    /* blrtvn1port20ncbcreditunderflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port20wbcreditunderflow : 1;
    /* blrtvn1port20wbcreditunderflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3EGRERRLOG3_M3KTI_MAIN_STRUCT;


/* M3EGRERRLOG4_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390130)                                                  */
/*       SKX (0x40390130)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRLOG4_M3KTI_MAIN_REG 0x0A004130
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 d2kcmpfifounderflow : 1;
    /* d2kcmpfifounderflow - Bits[0:0], RW1CS, default = 1'b0  */
    UINT32 d2kcmpfifooverflow : 1;
    /* d2kcmpfifooverflow - Bits[1:1], RW1CS, default = 1'b0  */
    UINT32 akcmscreditunderflow : 1;
    /* akcmscreditunderflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 blcmscreditunderflow : 1;
    /* blcmscreditunderflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 adcmscreditunderflow : 1;
    /* adcmscreditunderflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 akcmscreditoverflow : 1;
    /* akcmscreditoverflow - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 blcmscreditoverflow : 1;
    /* blcmscreditoverflow - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 adcmscreditoverflow : 1;
    /* adcmscreditoverflow - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 adchasliceidcreditunderflow : 5;
    /* adchasliceidcreditunderflow - Bits[12:8], RW1CS, default = 5'b0  */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 adchawbcreditunderflow : 1;
    /* adchawbcreditunderflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 adchasnpcreditunderflow : 1;
    /* adchasnpcreditunderflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 adchareqcreditunderflow : 1;
    /* adchareqcreditunderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 adchashrcreditunderflow : 1;
    /* adchashrcreditunderflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 adchasliceidcreditoverflow : 5;
    /* adchasliceidcreditoverflow - Bits[24:20], RW1CS, default = 5'b0  */
    UINT32 rsvd_25 : 3;
    /* rsvd_25 - Bits[27:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 adchawbcreditoverflow : 1;
    /* adchawbcreditoverflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 adchasnpcreditoverflow : 1;
    /* adchasnpcreditoverflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 adchareqcreditoverflow : 1;
    /* adchareqcreditoverflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 adchashrcreditoverflow : 1;
    /* adchashrcreditoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3EGRERRLOG4_M3KTI_MAIN_STRUCT;


/* M3EGRERRLOG5_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390134)                                                  */
/*       SKX (0x40390134)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRLOG5_M3KTI_MAIN_REG 0x0A004134
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti1parerrhdrrfing : 1;
    /* kti1parerrhdrrfing - Bits[0:0], RW1CS, default = 1'b0  */
    UINT32 kti1parerre2ebeing : 1;
    /* kti1parerre2ebeing - Bits[1:1], RW1CS, default = 1'b0  */
    UINT32 kti1parerre2edataing : 1;
    /* kti1parerre2edataing - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 rsvd : 1;
    /* rsvd - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 adrtvn1port0rspcreditunderflow : 1;
    /* adrtvn1port0rspcreditunderflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port0snpcreditunderflow : 1;
    /* adrtvn1port0snpcreditunderflow - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port0reqcreditunderflow : 1;
    /* adrtvn1port0reqcreditunderflow - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port2rspcreditunderflow : 1;
    /* adrtvn1port2rspcreditunderflow - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port2snpcreditunderflow : 1;
    /* adrtvn1port2snpcreditunderflow - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port2reqcreditunderflow : 1;
    /* adrtvn1port2reqcreditunderflow - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port0rspcreditoverflow : 1;
    /* adrtvn1port0rspcreditoverflow - Bits[10:10], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port0snpcreditoverflow : 1;
    /* adrtvn1port0snpcreditoverflow - Bits[11:11], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port0reqcreditoverflow : 1;
    /* adrtvn1port0reqcreditoverflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port2rspcreditoverflow : 1;
    /* adrtvn1port2rspcreditoverflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port2snpcreditoverflow : 1;
    /* adrtvn1port2snpcreditoverflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 adrtvn1port2reqcreditoverflow : 1;
    /* adrtvn1port2reqcreditoverflow - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port0rspcreditunderflow : 1;
    /* adrtvn0port0rspcreditunderflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port0snpcreditunderflow : 1;
    /* adrtvn0port0snpcreditunderflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port0reqcreditunderflow : 1;
    /* adrtvn0port0reqcreditunderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 adrtport0shrcreditunderflow : 1;
    /* adrtport0shrcreditunderflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port2rspcreditunderflow : 1;
    /* adrtvn0port2rspcreditunderflow - Bits[20:20], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port2snpcreditunderflow : 1;
    /* adrtvn0port2snpcreditunderflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port2reqcreditunderflow : 1;
    /* adrtvn0port2reqcreditunderflow - Bits[22:22], RW1CS, default = 1'b0  */
    UINT32 adrtport2shrcreditunderflow : 1;
    /* adrtport2shrcreditunderflow - Bits[23:23], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port0rspcreditoverflow : 1;
    /* adrtvn0port0rspcreditoverflow - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port0snpcreditoverflow : 1;
    /* adrtvn0port0snpcreditoverflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port0reqcreditoverflow : 1;
    /* adrtvn0port0reqcreditoverflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 adrtport0shrcreditoverflow : 1;
    /* adrtport0shrcreditoverflow - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port2rspcreditoverflow : 1;
    /* adrtvn0port2rspcreditoverflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port2snpcreditoverflow : 1;
    /* adrtvn0port2snpcreditoverflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 adrtvn0port2reqcreditoverflow : 1;
    /* adrtvn0port2reqcreditoverflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 adrtport2shrcreditoverflow : 1;
    /* adrtport2shrcreditoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3EGRERRLOG5_M3KTI_MAIN_STRUCT;


/* M3EGRERRLOG6_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390138)                                                  */
/*       SKX (0x40390138)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRLOG6_M3KTI_MAIN_REG 0x0A004138
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd : 1;
    /* rsvd - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rbtfifounderflow : 1;
    /* rbtfifounderflow - Bits[1:1], RW1CS, default = 1'b0  */
    UINT32 rbtfifooverflow : 1;
    /* rbtfifooverflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port2rspcreditunderflow : 1;
    /* blrtvn1port2rspcreditunderflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port0ncscreditoverflow : 1;
    /* blrtvn1port0ncscreditoverflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port0ncbcreditoverflow : 1;
    /* blrtvn1port0ncbcreditoverflow - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port0wbcreditoverflow : 1;
    /* blrtvn1port0wbcreditoverflow - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port0rspcreditoverflow : 1;
    /* blrtvn1port0rspcreditoverflow - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port2ncscreditoverflow : 1;
    /* blrtvn1port2ncscreditoverflow - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port2ncbcreditoverflow : 1;
    /* blrtvn1port2ncbcreditoverflow - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port2wbcreditoverflow : 1;
    /* blrtvn1port2wbcreditoverflow - Bits[10:10], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port2rspcreditoverflow : 1;
    /* blrtvn1port2rspcreditoverflow - Bits[11:11], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port0ncscreditunderflow : 1;
    /* blrtvn0port0ncscreditunderflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port0ncbcreditunderflow : 1;
    /* blrtvn0port0ncbcreditunderflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port0wbcreditunderflow : 1;
    /* blrtvn0port0wbcreditunderflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port0rspcreditunderflow : 1;
    /* blrtvn0port0rspcreditunderflow - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 blrtport0shrcreditunderflow : 1;
    /* blrtport0shrcreditunderflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port2ncscreditunderflow : 1;
    /* blrtvn0port2ncscreditunderflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port2ncbcreditunderflow : 1;
    /* blrtvn0port2ncbcreditunderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port2wbcreditunderflow : 1;
    /* blrtvn0port2wbcreditunderflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port2rspcreditunderflow : 1;
    /* blrtvn0port2rspcreditunderflow - Bits[20:20], RW1CS, default = 1'b0  */
    UINT32 blrtport2shrcreditunderflow : 1;
    /* blrtport2shrcreditunderflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port0ncscreditoverflow : 1;
    /* blrtvn0port0ncscreditoverflow - Bits[22:22], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port0ncbcreditoverflow : 1;
    /* blrtvn0port0ncbcreditoverflow - Bits[23:23], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port0wbcreditoverflow : 1;
    /* blrtvn0port0wbcreditoverflow - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port0rspcreditoverflow : 1;
    /* blrtvn0port0rspcreditoverflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 blrtport0shrcreditoverflow : 1;
    /* blrtport0shrcreditoverflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port2ncscreditoverflow : 1;
    /* blrtvn0port2ncscreditoverflow - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port2ncbcreditoverflow : 1;
    /* blrtvn0port2ncbcreditoverflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port2wbcreditoverflow : 1;
    /* blrtvn0port2wbcreditoverflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 blrtvn0port2rspcreditoverflow : 1;
    /* blrtvn0port2rspcreditoverflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 blrtport2shrcreditoverflow : 1;
    /* blrtport2shrcreditoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3EGRERRLOG6_M3KTI_MAIN_STRUCT;


/* M3EGRERRLOG7_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x4039013C)                                                  */
/*       SKX (0x4039013C)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRLOG7_M3KTI_MAIN_REG 0x0A00413C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd : 1;
    /* rsvd - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 bgfcreditunderflow : 1;
    /* bgfcreditunderflow - Bits[1:1], RW1CS, default = 1'b0  */
    UINT32 bgfcreditoverflow : 1;
    /* bgfcreditoverflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 akflowqunderflow : 1;
    /* akflowqunderflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 akflowqoverflow : 1;
    /* akflowqoverflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 blflowqmcunderflow : 3;
    /* blflowqmcunderflow - Bits[7:5], RW1CS, default = 3'b0  */
    UINT32 blflowqunderflow : 1;
    /* blflowqunderflow - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 blflowqoverflow : 1;
    /* blflowqoverflow - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 adflowqmcunderflow : 3;
    /* adflowqmcunderflow - Bits[12:10], RW1CS, default = 3'b0  */
    UINT32 adflowqunderflow : 1;
    /* adflowqunderflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 adflowqoverflow : 1;
    /* adflowqoverflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 blm2pidcreditunderflow : 3;
    /* blm2pidcreditunderflow - Bits[17:15], RW1CS, default = 3'b0  */
    UINT32 blm2pidcreditoverflow : 3;
    /* blm2pidcreditoverflow - Bits[20:18], RW1CS, default = 3'b0  */
    UINT32 blm2pncscreditunderflow : 1;
    /* blm2pncscreditunderflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 blm2pncbcreditunderflow : 1;
    /* blm2pncbcreditunderflow - Bits[22:22], RW1CS, default = 1'b0  */
    UINT32 blm2pncscreditoverflow : 1;
    /* blm2pncscreditoverflow - Bits[23:23], RW1CS, default = 1'b0  */
    UINT32 blm2pncbcreditoverflow : 1;
    /* blm2pncbcreditoverflow - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port0ncscreditunderflow : 1;
    /* blrtvn1port0ncscreditunderflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port0ncbcreditunderflow : 1;
    /* blrtvn1port0ncbcreditunderflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port0wbcreditunderflow : 1;
    /* blrtvn1port0wbcreditunderflow - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port0rspcreditunderflow : 1;
    /* blrtvn1port0rspcreditunderflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port2ncscreditunderflow : 1;
    /* blrtvn1port2ncscreditunderflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port2ncbcreditunderflow : 1;
    /* blrtvn1port2ncbcreditunderflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 blrtvn1port2wbcreditunderflow : 1;
    /* blrtvn1port2wbcreditunderflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3EGRERRLOG7_M3KTI_MAIN_STRUCT;


/* M3KINGSMIC_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x40390140)                                                  */
/*       SKX (0x40390140)                                                     */
/* Register default value:              0x00003C80                            */
#define M3KINGSMIC_M3KTI_MAIN_REG 0x0A004140
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 disablesmi3prefetchaging : 1;
    /* disablesmi3prefetchaging - Bits[0:0], RWS, default = 1'b0  */
    UINT32 rsvd_1 : 3;
    /* rsvd_1 - Bits[3:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 smi3prefetchqueuedepth : 5;
    /* smi3prefetchqueuedepth - Bits[8:4], RWS, default = 5'd8  */
    UINT32 maxsmi3prefetchage : 6;
    /* maxsmi3prefetchage - Bits[14:9], RWS, default = 6'd30  */
    UINT32 rsvd_15 : 17;
    /* rsvd_15 - Bits[31:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KINGSMIC_M3KTI_MAIN_STRUCT;


/* M3KINGCTL_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40390144)                                                  */
/*       SKX (0x40390144)                                                     */
/* Register default value:              0x10260844                            */
#define M3KINGCTL_M3KTI_MAIN_REG 0x0A004144
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Ingress Control
 */
typedef union {
  struct {
    UINT32 adbypassexitthrqueued : 4;
    /* adbypassexitthrqueued - Bits[3:0], RWS, default = 4'h4  */
    UINT32 adbypassexitthrprior : 4;
    /* adbypassexitthrprior - Bits[7:4], RWS, default = 4'h4  */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 validtxcyclemode : 2;
    /* validtxcyclemode - Bits[10:9], RWS, default = 2'b0  */
    UINT32 idlebypassexitmodeselector : 2;
    /* idlebypassexitmodeselector - Bits[12:11], RWS, default = 2'b1  */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 flitsinbgfifothresholdunnnh : 4;
    /* flitsinbgfifothresholdunnnh - Bits[19:16], RWS, default = 4'h6  */
    UINT32 priorflittxthreshold : 3;
    /* priorflittxthreshold - Bits[22:20], RWS, default = 3'h2  */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 slot2blocktimer : 6;
    /* slot2blocktimer - Bits[29:24], RWS, default = 6'h10  */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KINGCTL_M3KTI_MAIN_STRUCT;


/* M3EGRERRMSK8_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390148)                                                  */
/*       SKX (0x40390148)                                                     */
/* Register default value:              0x00000000                            */
#define M3EGRERRMSK8_M3KTI_MAIN_REG 0x0A004148
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Mask
 */
typedef union {
  struct {
    UINT32 kti0parerrhdrrfingmask : 1;
    /* kti0parerrhdrrfingmask - Bits[0:0], RWS, default = 1'b0 
        
     */
    UINT32 kti0parerre2ebeingmask : 1;
    /* kti0parerre2ebeingmask - Bits[1:1], RWS, default = 1'b0 
        
     */
    UINT32 kti0parerre2edataingmask : 1;
    /* kti0parerre2edataingmask - Bits[2:2], RWS, default = 1'b0 
        
     */
    UINT32 rsvd_3 : 1;
    /* rsvd_3 - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti0bgfcrdunderflowmask : 1;
    /* kti0bgfcrdunderflowmask - Bits[4:4], RWS, default = 1'b0 
        
     */
    UINT32 kti0bgfcrdoverflowmask : 1;
    /* kti0bgfcrdoverflowmask - Bits[5:5], RWS, default = 1'b0 
        
     */
    UINT32 kti0rbtfifounderflowmask : 1;
    /* kti0rbtfifounderflowmask - Bits[6:6], RWS, default = 1'b0 
        
     */
    UINT32 kti0rbtfifooverflowmask : 1;
    /* kti0rbtfifooverflowmask - Bits[7:7], RWS, default = 1'b0 
        
     */
    UINT32 rsvd_8 : 8;
    /* rsvd_8 - Bits[15:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti1parerrhdrrfingmask : 1;
    /* kti1parerrhdrrfingmask - Bits[16:16], RWS, default = 1'b0 
        
     */
    UINT32 kti1parerre2ebeingmask : 1;
    /* kti1parerre2ebeingmask - Bits[17:17], RWS, default = 1'b0 
        
     */
    UINT32 kti1parerre2edataingmask : 1;
    /* kti1parerre2edataingmask - Bits[18:18], RWS, default = 1'b0 
        
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti1bgfcrdunderflowmask : 1;
    /* kti1bgfcrdunderflowmask - Bits[20:20], RWS, default = 1'b0 
        
     */
    UINT32 kti1bgfcrdoverflowmask : 1;
    /* kti1bgfcrdoverflowmask - Bits[21:21], RWS, default = 1'b0 
        
     */
    UINT32 kti1rbtfifounderflowmask : 1;
    /* kti1rbtfifounderflowmask - Bits[22:22], RWS, default = 1'b0 
        
     */
    UINT32 kti1rbtfifooverflowmask : 1;
    /* kti1rbtfifooverflowmask - Bits[23:23], RWS, default = 1'b0 
        
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3EGRERRMSK8_M3KTI_MAIN_STRUCT;




/* M3KLL_LDCL_M3KTI_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x40390154)                                                  */
/*       SKX (0x40390154)                                                     */
/* Register default value:              0x00000000                            */
#define M3KLL_LDCL_M3KTI_MAIN_REG 0x0A004154
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 disableflitslot1 : 1;
    /* disableflitslot1 - Bits[0:0], RWS, default = 1'b0 
       By default, all flit slots may be used. If this bit is set to 1, no messages 
       will be placed into flit slot 1. (In some cases, a single message occupies flit 
       slots 0 and 1. Such a message is technically a slot 0 message and the associated 
       use of slot 1 is always allowed and not disabled by this bit.) 
     */
    UINT32 disableflitslot2 : 1;
    /* disableflitslot2 - Bits[1:1], RWS, default = 1'b0 
       By default, all flit slots may be used. If this bit is set to 1, no protocol 
       messages will be placed into flit slot 2. (The link layer may still use flit 
       slot 2 for credit return messages.) 
     */
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KLL_LDCL_M3KTI_MAIN_STRUCT;


/* M3INGERRMASK0_M3KTI_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x40390158)                                                  */
/*       SKX (0x40390158)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGERRMASK0_M3KTI_MAIN_REG 0x0A004158
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 blp1pcmpbufoverflowmask : 1;
    /* blp1pcmpbufoverflowmask - Bits[0:0], RWS, default = 1'b0 
        
     */
    UINT32 blp1pcmpbufunderflowmask : 1;
    /* blp1pcmpbufunderflowmask - Bits[1:1], RWS, default = 1'b0 
        
     */
    UINT32 blpumperrorsecondnot1mask : 1;
    /* blpumperrorsecondnot1mask - Bits[2:2], RWS, default = 1'b0 
        
     */
    UINT32 blpumperrorfirstnot0mask : 1;
    /* blpumperrorfirstnot0mask - Bits[3:3], RWS, default = 1'b0 
        
     */
    UINT32 llresettxnotidlemask : 1;
    /* llresettxnotidlemask - Bits[4:4], RWS, default = 1'b0 
        
     */
    UINT32 llresetrxnotidlemask : 1;
    /* llresetrxnotidlemask - Bits[5:5], RWS, default = 1'b0 
        
     */
    UINT32 blingrlistdeferbufunderflowmask : 1;
    /* blingrlistdeferbufunderflowmask - Bits[6:6], RWS, default = 1'b0 
        
     */
    UINT32 blingrlistdeferbufoverflowmask : 1;
    /* blingrlistdeferbufoverflowmask - Bits[7:7], RWS, default = 1'b0 
        
     */
    UINT32 bld2kmarkerfifounderflowmask : 1;
    /* bld2kmarkerfifounderflowmask - Bits[8:8], RWS, default = 1'b0 
        
     */
    UINT32 bld2kmarkerfifooverflowmask : 1;
    /* bld2kmarkerfifooverflowmask - Bits[9:9], RWS, default = 1'b0 
        
     */
    UINT32 bld2kinfofifounderflowmask : 1;
    /* bld2kinfofifounderflowmask - Bits[10:10], RWS, default = 1'b0 
        
     */
    UINT32 bld2kinfofifooverflowmask : 1;
    /* bld2kinfofifooverflowmask - Bits[11:11], RWS, default = 1'b0 
        
     */
    UINT32 blcrdtypefifounderflowmask : 1;
    /* blcrdtypefifounderflowmask - Bits[12:12], RWS, default = 1'b0 
        
     */
    UINT32 blcrdtypefifooverflowmask : 1;
    /* blcrdtypefifooverflowmask - Bits[13:13], RWS, default = 1'b0 
        
     */
    UINT32 blsrcidfifounderflowmask : 1;
    /* blsrcidfifounderflowmask - Bits[14:14], RWS, default = 1'b0 
        
     */
    UINT32 blsrcidfifooverflowmask : 1;
    /* blsrcidfifooverflowmask - Bits[15:15], RWS, default = 1'b0 
        
     */
    UINT32 txqcrdunderflowmask : 1;
    /* txqcrdunderflowmask - Bits[16:16], RWS, default = 1'b0 
        
     */
    UINT32 txqcrdoverflowmask : 1;
    /* txqcrdoverflowmask - Bits[17:17], RWS, default = 1'b0 
        
     */
    UINT32 blingressunderflowmask : 1;
    /* blingressunderflowmask - Bits[18:18], RWS, default = 1'b0 
        
     */
    UINT32 blingressoverflowmask : 1;
    /* blingressoverflowmask - Bits[19:19], RWS, default = 1'b0 
        
     */
    UINT32 adingressunderflowmask : 1;
    /* adingressunderflowmask - Bits[20:20], RWS, default = 1'b0 
        
     */
    UINT32 adingressoverflowmask : 1;
    /* adingressoverflowmask - Bits[21:21], RWS, default = 1'b0 
        
     */
    UINT32 d2kcmpcreditunderflowmask : 1;
    /* d2kcmpcreditunderflowmask - Bits[22:22], RWS, default = 1'b0 
        
     */
    UINT32 d2kcmpcreditoverflowmask : 1;
    /* d2kcmpcreditoverflowmask - Bits[23:23], RWS, default = 1'b0 
        
     */
    UINT32 bgfcreditunderflowmask : 1;
    /* bgfcreditunderflowmask - Bits[24:24], RWS, default = 1'b0 
        
     */
    UINT32 bgfcreditoverflowmask : 1;
    /* bgfcreditoverflowmask - Bits[25:25], RWS, default = 1'b0 
        
     */
    UINT32 remotevn1creditunderflowmask : 1;
    /* remotevn1creditunderflowmask - Bits[26:26], RWS, default = 1'b0 
        
     */
    UINT32 remotevn1creditoverflowmask : 1;
    /* remotevn1creditoverflowmask - Bits[27:27], RWS, default = 1'b0 
        
     */
    UINT32 remotevn0creditunderflowmask : 1;
    /* remotevn0creditunderflowmask - Bits[28:28], RWS, default = 1'b0 
        
     */
    UINT32 remotevn0creditoverflowmask : 1;
    /* remotevn0creditoverflowmask - Bits[29:29], RWS, default = 1'b0 
        
     */
    UINT32 remotevnacreditunderflowmask : 1;
    /* remotevnacreditunderflowmask - Bits[30:30], RWS, default = 1'b0 
        
     */
    UINT32 remotevnacreditoverflowmask : 1;
    /* remotevnacreditoverflowmask - Bits[31:31], RWS, default = 1'b0 
        
     */
  } Bits;
  UINT32 Data;
} M3INGERRMASK0_M3KTI_MAIN_STRUCT;


/* M3INGERRMASK1_M3KTI_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x4039015C)                                                  */
/*       SKX (0x4039015C)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGERRMASK1_M3KTI_MAIN_REG 0x0A00415C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 blp1pcmpbufoverflowmask : 1;
    /* blp1pcmpbufoverflowmask - Bits[0:0], RWS, default = 1'b0 
        
     */
    UINT32 blp1pcmpbufunderflowmask : 1;
    /* blp1pcmpbufunderflowmask - Bits[1:1], RWS, default = 1'b0 
        
     */
    UINT32 blpumperrorsecondnot1mask : 1;
    /* blpumperrorsecondnot1mask - Bits[2:2], RWS, default = 1'b0 
        
     */
    UINT32 blpumperrorfirstnot0mask : 1;
    /* blpumperrorfirstnot0mask - Bits[3:3], RWS, default = 1'b0 
        
     */
    UINT32 llresettxnotidlemask : 1;
    /* llresettxnotidlemask - Bits[4:4], RWS, default = 1'b0 
        
     */
    UINT32 llresetrxnotidlemask : 1;
    /* llresetrxnotidlemask - Bits[5:5], RWS, default = 1'b0 
        
     */
    UINT32 blingrlistdeferbufunderflowmask : 1;
    /* blingrlistdeferbufunderflowmask - Bits[6:6], RWS, default = 1'b0 
        
     */
    UINT32 blingrlistdeferbufoverflowmask : 1;
    /* blingrlistdeferbufoverflowmask - Bits[7:7], RWS, default = 1'b0 
        
     */
    UINT32 bld2kmarkerfifounderflowmask : 1;
    /* bld2kmarkerfifounderflowmask - Bits[8:8], RWS, default = 1'b0 
        
     */
    UINT32 bld2kmarkerfifooverflowmask : 1;
    /* bld2kmarkerfifooverflowmask - Bits[9:9], RWS, default = 1'b0 
        
     */
    UINT32 bld2kinfofifounderflowmask : 1;
    /* bld2kinfofifounderflowmask - Bits[10:10], RWS, default = 1'b0 
        
     */
    UINT32 bld2kinfofifooverflowmask : 1;
    /* bld2kinfofifooverflowmask - Bits[11:11], RWS, default = 1'b0 
        
     */
    UINT32 blcrdtypefifounderflowmask : 1;
    /* blcrdtypefifounderflowmask - Bits[12:12], RWS, default = 1'b0 
        
     */
    UINT32 blcrdtypefifooverflowmask : 1;
    /* blcrdtypefifooverflowmask - Bits[13:13], RWS, default = 1'b0 
        
     */
    UINT32 blsrcidfifounderflowmask : 1;
    /* blsrcidfifounderflowmask - Bits[14:14], RWS, default = 1'b0 
        
     */
    UINT32 blsrcidfifooverflowmask : 1;
    /* blsrcidfifooverflowmask - Bits[15:15], RWS, default = 1'b0 
        
     */
    UINT32 txqcrdunderflowmask : 1;
    /* txqcrdunderflowmask - Bits[16:16], RWS, default = 1'b0 
        
     */
    UINT32 txqcrdoverflowmask : 1;
    /* txqcrdoverflowmask - Bits[17:17], RWS, default = 1'b0 
        
     */
    UINT32 blingressunderflowmask : 1;
    /* blingressunderflowmask - Bits[18:18], RWS, default = 1'b0 
        
     */
    UINT32 blingressoverflowmask : 1;
    /* blingressoverflowmask - Bits[19:19], RWS, default = 1'b0 
        
     */
    UINT32 adingressunderflowmask : 1;
    /* adingressunderflowmask - Bits[20:20], RWS, default = 1'b0 
        
     */
    UINT32 adingressoverflowmask : 1;
    /* adingressoverflowmask - Bits[21:21], RWS, default = 1'b0 
        
     */
    UINT32 d2kcmpcreditunderflowmask : 1;
    /* d2kcmpcreditunderflowmask - Bits[22:22], RWS, default = 1'b0 
        
     */
    UINT32 d2kcmpcreditoverflowmask : 1;
    /* d2kcmpcreditoverflowmask - Bits[23:23], RWS, default = 1'b0 
        
     */
    UINT32 bgfcreditunderflowmask : 1;
    /* bgfcreditunderflowmask - Bits[24:24], RWS, default = 1'b0 
        
     */
    UINT32 bgfcreditoverflowmask : 1;
    /* bgfcreditoverflowmask - Bits[25:25], RWS, default = 1'b0 
        
     */
    UINT32 remotevn1creditunderflowmask : 1;
    /* remotevn1creditunderflowmask - Bits[26:26], RWS, default = 1'b0 
        
     */
    UINT32 remotevn1creditoverflowmask : 1;
    /* remotevn1creditoverflowmask - Bits[27:27], RWS, default = 1'b0 
        
     */
    UINT32 remotevn0creditunderflowmask : 1;
    /* remotevn0creditunderflowmask - Bits[28:28], RWS, default = 1'b0 
        
     */
    UINT32 remotevn0creditoverflowmask : 1;
    /* remotevn0creditoverflowmask - Bits[29:29], RWS, default = 1'b0 
        
     */
    UINT32 remotevnacreditunderflowmask : 1;
    /* remotevnacreditunderflowmask - Bits[30:30], RWS, default = 1'b0 
        
     */
    UINT32 remotevnacreditoverflowmask : 1;
    /* remotevnacreditoverflowmask - Bits[31:31], RWS, default = 1'b0 
        
     */
  } Bits;
  UINT32 Data;
} M3INGERRMASK1_M3KTI_MAIN_STRUCT;


/* M3INGERRLOG0_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390160)                                                  */
/*       SKX (0x40390160)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGERRLOG0_M3KTI_MAIN_REG 0x0A004160
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Ingress Error Logging.
 */
typedef union {
  struct {
    UINT32 rsvd : 2;
    /* rsvd - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 d2kcmpcreditunderflow : 1;
    /* d2kcmpcreditunderflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 d2kcmpcreditoverflow : 1;
    /* d2kcmpcreditoverflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 bgfcreditunderflow : 1;
    /* bgfcreditunderflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 bgfcreditoverflow : 1;
    /* bgfcreditoverflow - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 remotevn1ncscreditunderflow : 1;
    /* remotevn1ncscreditunderflow - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 remotevn1ncbcreditunderflow : 1;
    /* remotevn1ncbcreditunderflow - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 remotevn1wbcreditunderflow : 1;
    /* remotevn1wbcreditunderflow - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 remotevn1rspcreditunderflow : 1;
    /* remotevn1rspcreditunderflow - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 remotevn1snpcreditunderflow : 1;
    /* remotevn1snpcreditunderflow - Bits[10:10], RW1CS, default = 1'b0  */
    UINT32 remotevn1reqcreditunderflow : 1;
    /* remotevn1reqcreditunderflow - Bits[11:11], RW1CS, default = 1'b0  */
    UINT32 remotevn1ncscreditoverflow : 1;
    /* remotevn1ncscreditoverflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 remotevn1ncbcreditoverflow : 1;
    /* remotevn1ncbcreditoverflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 remotevn1wbcreditoverflow : 1;
    /* remotevn1wbcreditoverflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 remotevn1rspcreditoverflow : 1;
    /* remotevn1rspcreditoverflow - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 remotevn1snpcreditoverflow : 1;
    /* remotevn1snpcreditoverflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 remotevn1reqcreditoverflow : 1;
    /* remotevn1reqcreditoverflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 remotevn0ncscreditunderflow : 1;
    /* remotevn0ncscreditunderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 remotevn0ncbcreditunderflow : 1;
    /* remotevn0ncbcreditunderflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 remotevn0wbcreditunderflow : 1;
    /* remotevn0wbcreditunderflow - Bits[20:20], RW1CS, default = 1'b0  */
    UINT32 remotevn0rspcreditunderflow : 1;
    /* remotevn0rspcreditunderflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 remotevn0snpcreditunderflow : 1;
    /* remotevn0snpcreditunderflow - Bits[22:22], RW1CS, default = 1'b0  */
    UINT32 remotevn0reqcreditunderflow : 1;
    /* remotevn0reqcreditunderflow - Bits[23:23], RW1CS, default = 1'b0  */
    UINT32 remotevn0ncscreditoverflow : 1;
    /* remotevn0ncscreditoverflow - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 remotevn0ncbcreditoverflow : 1;
    /* remotevn0ncbcreditoverflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 remotevn0wbcreditoverflow : 1;
    /* remotevn0wbcreditoverflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 remotevn0rspcreditoverflow : 1;
    /* remotevn0rspcreditoverflow - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 remotevn0snpcreditoverflow : 1;
    /* remotevn0snpcreditoverflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 remotevn0reqcreditoverflow : 1;
    /* remotevn0reqcreditoverflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 remotevnacreditunderflow : 1;
    /* remotevnacreditunderflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 remotevnacreditoverflow : 1;
    /* remotevnacreditoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3INGERRLOG0_M3KTI_MAIN_STRUCT;


/* M3INGERRLOG1_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390164)                                                  */
/*       SKX (0x40390164)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGERRLOG1_M3KTI_MAIN_REG 0x0A004164
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Ingress Error Logging.
 */
typedef union {
  struct {
    UINT32 rsvd : 2;
    /* rsvd - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 blp1pcmpbufoverflow : 1;
    /* blp1pcmpbufoverflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 blp1pcmpbufunderflow : 1;
    /* blp1pcmpbufunderflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 blpumperrorsecondnot1 : 1;
    /* blpumperrorsecondnot1 - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 blpumperrorfirstnot0 : 1;
    /* blpumperrorfirstnot0 - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 llresetwhiletxqcreditbusy : 1;
    /* llresetwhiletxqcreditbusy - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 llresetwhileslotnotidle : 1;
    /* llresetwhileslotnotidle - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 llresetwhilelistnotempty : 1;
    /* llresetwhilelistnotempty - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 llresetrxnotidle : 1;
    /* llresetrxnotidle - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 blingrlistdeferbufunderflow : 1;
    /* blingrlistdeferbufunderflow - Bits[10:10], RW1CS, default = 1'b0  */
    UINT32 blingrlistdeferbufoverflow : 1;
    /* blingrlistdeferbufoverflow - Bits[11:11], RW1CS, default = 1'b0  */
    UINT32 bld2kmarkerfifounderflow : 1;
    /* bld2kmarkerfifounderflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 bld2kmarkerfifooverflow : 1;
    /* bld2kmarkerfifooverflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 bld2kinfofifounderflow : 1;
    /* bld2kinfofifounderflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 bld2kinfofifooverflow : 1;
    /* bld2kinfofifooverflow - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 blcrdtypefifounderflow : 1;
    /* blcrdtypefifounderflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 blcrdtypefifooverflow : 1;
    /* blcrdtypefifooverflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 blsrcidfifounderflow : 1;
    /* blsrcidfifounderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 blsrcidfifooverflow : 1;
    /* blsrcidfifooverflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 txqcrdunderflow : 1;
    /* txqcrdunderflow - Bits[20:20], RW1CS, default = 1'b0  */
    UINT32 txqcrdoverflow : 1;
    /* txqcrdoverflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 blingressvcidunderflow : 3;
    /* blingressvcidunderflow - Bits[24:22], RW1CS, default = 3'b0  */
    UINT32 blingressunderflow : 1;
    /* blingressunderflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 blingressoverflow : 1;
    /* blingressoverflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 adingressvcidunderflow : 3;
    /* adingressvcidunderflow - Bits[29:27], RW1CS, default = 3'b0  */
    UINT32 adingressunderflow : 1;
    /* adingressunderflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 adingressoverflow : 1;
    /* adingressoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3INGERRLOG1_M3KTI_MAIN_STRUCT;


/* M3INGERRLOG2_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390168)                                                  */
/*       SKX (0x40390168)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGERRLOG2_M3KTI_MAIN_REG 0x0A004168
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Ingress Error Logging.
 */
typedef union {
  struct {
    UINT32 rsvd : 2;
    /* rsvd - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 d2kcmpcreditunderflow : 1;
    /* d2kcmpcreditunderflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 d2kcmpcreditoverflow : 1;
    /* d2kcmpcreditoverflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 bgfcreditunderflow : 1;
    /* bgfcreditunderflow - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 bgfcreditoverflow : 1;
    /* bgfcreditoverflow - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 remotevn1ncscreditunderflow : 1;
    /* remotevn1ncscreditunderflow - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 remotevn1ncbcreditunderflow : 1;
    /* remotevn1ncbcreditunderflow - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 remotevn1wbcreditunderflow : 1;
    /* remotevn1wbcreditunderflow - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 remotevn1rspcreditunderflow : 1;
    /* remotevn1rspcreditunderflow - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 remotevn1snpcreditunderflow : 1;
    /* remotevn1snpcreditunderflow - Bits[10:10], RW1CS, default = 1'b0  */
    UINT32 remotevn1reqcreditunderflow : 1;
    /* remotevn1reqcreditunderflow - Bits[11:11], RW1CS, default = 1'b0  */
    UINT32 remotevn1ncscreditoverflow : 1;
    /* remotevn1ncscreditoverflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 remotevn1ncbcreditoverflow : 1;
    /* remotevn1ncbcreditoverflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 remotevn1wbcreditoverflow : 1;
    /* remotevn1wbcreditoverflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 remotevn1rspcreditoverflow : 1;
    /* remotevn1rspcreditoverflow - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 remotevn1snpcreditoverflow : 1;
    /* remotevn1snpcreditoverflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 remotevn1reqcreditoverflow : 1;
    /* remotevn1reqcreditoverflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 remotevn0ncscreditunderflow : 1;
    /* remotevn0ncscreditunderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 remotevn0ncbcreditunderflow : 1;
    /* remotevn0ncbcreditunderflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 remotevn0wbcreditunderflow : 1;
    /* remotevn0wbcreditunderflow - Bits[20:20], RW1CS, default = 1'b0  */
    UINT32 remotevn0rspcreditunderflow : 1;
    /* remotevn0rspcreditunderflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 remotevn0snpcreditunderflow : 1;
    /* remotevn0snpcreditunderflow - Bits[22:22], RW1CS, default = 1'b0  */
    UINT32 remotevn0reqcreditunderflow : 1;
    /* remotevn0reqcreditunderflow - Bits[23:23], RW1CS, default = 1'b0  */
    UINT32 remotevn0ncscreditoverflow : 1;
    /* remotevn0ncscreditoverflow - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 remotevn0ncbcreditoverflow : 1;
    /* remotevn0ncbcreditoverflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 remotevn0wbcreditoverflow : 1;
    /* remotevn0wbcreditoverflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 remotevn0rspcreditoverflow : 1;
    /* remotevn0rspcreditoverflow - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 remotevn0snpcreditoverflow : 1;
    /* remotevn0snpcreditoverflow - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 remotevn0reqcreditoverflow : 1;
    /* remotevn0reqcreditoverflow - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 remotevnacreditunderflow : 1;
    /* remotevnacreditunderflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 remotevnacreditoverflow : 1;
    /* remotevnacreditoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3INGERRLOG2_M3KTI_MAIN_STRUCT;


/* M3INGERRLOG3_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x4039016C)                                                  */
/*       SKX (0x4039016C)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGERRLOG3_M3KTI_MAIN_REG 0x0A00416C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd : 2;
    /* rsvd - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 blp1pcmpbufoverflow : 1;
    /* blp1pcmpbufoverflow - Bits[2:2], RW1CS, default = 1'b0  */
    UINT32 blp1pcmpbufunderflow : 1;
    /* blp1pcmpbufunderflow - Bits[3:3], RW1CS, default = 1'b0  */
    UINT32 blpumperrorsecondnot1 : 1;
    /* blpumperrorsecondnot1 - Bits[4:4], RW1CS, default = 1'b0  */
    UINT32 blpumperrorfirstnot0 : 1;
    /* blpumperrorfirstnot0 - Bits[5:5], RW1CS, default = 1'b0  */
    UINT32 llresetwhiletxqcreditbusy : 1;
    /* llresetwhiletxqcreditbusy - Bits[6:6], RW1CS, default = 1'b0  */
    UINT32 llresetwhileslotnotidle : 1;
    /* llresetwhileslotnotidle - Bits[7:7], RW1CS, default = 1'b0  */
    UINT32 llresetwhilelistnotempty : 1;
    /* llresetwhilelistnotempty - Bits[8:8], RW1CS, default = 1'b0  */
    UINT32 llresetrxnotidle : 1;
    /* llresetrxnotidle - Bits[9:9], RW1CS, default = 1'b0  */
    UINT32 blingrlistdeferbufunderflow : 1;
    /* blingrlistdeferbufunderflow - Bits[10:10], RW1CS, default = 1'b0  */
    UINT32 blingrlistdeferbufoverflow : 1;
    /* blingrlistdeferbufoverflow - Bits[11:11], RW1CS, default = 1'b0  */
    UINT32 bld2kmarkerfifounderflow : 1;
    /* bld2kmarkerfifounderflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 bld2kmarkerfifooverflow : 1;
    /* bld2kmarkerfifooverflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 bld2kinfofifounderflow : 1;
    /* bld2kinfofifounderflow - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 bld2kinfofifooverflow : 1;
    /* bld2kinfofifooverflow - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 blcrdtypefifounderflow : 1;
    /* blcrdtypefifounderflow - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 blcrdtypefifooverflow : 1;
    /* blcrdtypefifooverflow - Bits[17:17], RW1CS, default = 1'b0  */
    UINT32 blsrcidfifounderflow : 1;
    /* blsrcidfifounderflow - Bits[18:18], RW1CS, default = 1'b0  */
    UINT32 blsrcidfifooverflow : 1;
    /* blsrcidfifooverflow - Bits[19:19], RW1CS, default = 1'b0  */
    UINT32 txqcrdunderflow : 1;
    /* txqcrdunderflow - Bits[20:20], RW1CS, default = 1'b0  */
    UINT32 txqcrdoverflow : 1;
    /* txqcrdoverflow - Bits[21:21], RW1CS, default = 1'b0  */
    UINT32 blingressvcidunderflow : 3;
    /* blingressvcidunderflow - Bits[24:22], RW1CS, default = 3'b0  */
    UINT32 blingressunderflow : 1;
    /* blingressunderflow - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 blingressoverflow : 1;
    /* blingressoverflow - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 adingressvcidunderflow : 3;
    /* adingressvcidunderflow - Bits[29:27], RW1CS, default = 3'b0  */
    UINT32 adingressunderflow : 1;
    /* adingressunderflow - Bits[30:30], RW1CS, default = 1'b0  */
    UINT32 adingressoverflow : 1;
    /* adingressoverflow - Bits[31:31], RW1CS, default = 1'b0  */
  } Bits;
  UINT32 Data;
} M3INGERRLOG3_M3KTI_MAIN_STRUCT;


/* M3INGDBGREMCRE0KTI0_M3KTI_MAIN_REG supported on:                           */
/*       SKX_A0 (0x40390170)                                                  */
/*       SKX (0x40390170)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGDBGREMCRE0KTI0_M3KTI_MAIN_REG 0x0A004170
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti0remotevnacreditreferencecount : 7;
    /* kti0remotevnacreditreferencecount - Bits[6:0], RWS, default = 7'b0 
       For most aspects of normal operation, the UPI ingress logic gets its VNA credit 
       allocation for the remote socket via initial credit returns transmitted by the 
       remote socket. However, for error detection (e.g. credit overflow) to work and 
       for clock gating to be more efficient, the ingress logic needs an independent 
       reference for what the total credit VNA count is supposed to be. The value of 
       this field is this reference. Once this field is programmed, the value needs to 
       be activated by toggling the LoadVnaCreditReference bit. 
     */
    UINT32 kti0loadremotevnacreditreference : 1;
    /* kti0loadremotevnacreditreference - Bits[7:7], RWS, default = 1'b0 
       To activate a value programmed into the RemVnaCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnaCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn0reqcreditreferencecount : 3;
    /* kti0remotevn0reqcreditreferencecount - Bits[10:8], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn0reqcreditreference : 1;
    /* kti0loadremotevn0reqcreditreference - Bits[11:11], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn0snpcreditreferencecount : 3;
    /* kti0remotevn0snpcreditreferencecount - Bits[14:12], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn0snpcreditreference : 1;
    /* kti0loadremotevn0snpcreditreference - Bits[15:15], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn0rspcreditreferencecount : 3;
    /* kti0remotevn0rspcreditreferencecount - Bits[18:16], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn0rspcreditreference : 1;
    /* kti0loadremotevn0rspcreditreference - Bits[19:19], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn0wbcreditreferencecount : 3;
    /* kti0remotevn0wbcreditreferencecount - Bits[22:20], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn0wbcreditreference : 1;
    /* kti0loadremotevn0wbcreditreference - Bits[23:23], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn0ncbcreditreferencecount : 3;
    /* kti0remotevn0ncbcreditreferencecount - Bits[26:24], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn0ncbcreditreference : 1;
    /* kti0loadremotevn0ncbcreditreference - Bits[27:27], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn0ncscreditreferencecount : 3;
    /* kti0remotevn0ncscreditreferencecount - Bits[30:28], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn0ncscreditreference : 1;
    /* kti0loadremotevn0ncscreditreference - Bits[31:31], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
  } Bits;
  UINT32 Data;
} M3INGDBGREMCRE0KTI0_M3KTI_MAIN_STRUCT;


/* M3INGDBGREMCRE1KTI0_M3KTI_MAIN_REG supported on:                           */
/*       SKX_A0 (0x40390174)                                                  */
/*       SKX (0x40390174)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGDBGREMCRE1KTI0_M3KTI_MAIN_REG 0x0A004174
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd : 8;
    /* rsvd - Bits[7:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti0remotevn1reqcreditreferencecount : 3;
    /* kti0remotevn1reqcreditreferencecount - Bits[10:8], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn1reqcreditreference : 1;
    /* kti0loadremotevn1reqcreditreference - Bits[11:11], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn1snpcreditreferencecount : 3;
    /* kti0remotevn1snpcreditreferencecount - Bits[14:12], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn1snpcreditreference : 1;
    /* kti0loadremotevn1snpcreditreference - Bits[15:15], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn1rspcreditreferencecount : 3;
    /* kti0remotevn1rspcreditreferencecount - Bits[18:16], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn1rspcreditreference : 1;
    /* kti0loadremotevn1rspcreditreference - Bits[19:19], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn1wbcreditreferencecount : 3;
    /* kti0remotevn1wbcreditreferencecount - Bits[22:20], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn1wbcreditreference : 1;
    /* kti0loadremotevn1wbcreditreference - Bits[23:23], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn1ncbcreditreferencecount : 3;
    /* kti0remotevn1ncbcreditreferencecount - Bits[26:24], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn1ncbcreditreference : 1;
    /* kti0loadremotevn1ncbcreditreference - Bits[27:27], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti0remotevn1ncscreditreferencecount : 3;
    /* kti0remotevn1ncscreditreferencecount - Bits[30:28], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti0loadremotevn1ncscreditreference : 1;
    /* kti0loadremotevn1ncscreditreference - Bits[31:31], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
  } Bits;
  UINT32 Data;
} M3INGDBGREMCRE1KTI0_M3KTI_MAIN_STRUCT;


/* M3INGDBGREMCRE0KTI1_M3KTI_MAIN_REG supported on:                           */
/*       SKX_A0 (0x40390178)                                                  */
/*       SKX (0x40390178)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGDBGREMCRE0KTI1_M3KTI_MAIN_REG 0x0A004178
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti1remotevnacreditreferencecount : 7;
    /* kti1remotevnacreditreferencecount - Bits[6:0], RWS, default = 7'b0 
       For most aspects of normal operation, the UPI ingress logic gets its VNA credit 
       allocation for the remote socket via initial credit returns transmitted by the 
       remote socket. However, for error detection (e.g. credit overflow) to work and 
       for clock gating to be more efficient, the ingress logic needs an independent 
       reference for what the total credit VNA count is supposed to be. The value of 
       this field is this reference. Once this field is programmed, the value needs to 
       be activated by toggling the LoadVnaCreditReference bit. 
     */
    UINT32 kti1loadremotevnacreditreference : 1;
    /* kti1loadremotevnacreditreference - Bits[7:7], RWS, default = 1'b0 
       To activate a value programmed into the RemVnaCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnaCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn0reqcreditreferencecount : 3;
    /* kti1remotevn0reqcreditreferencecount - Bits[10:8], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn0reqcreditreference : 1;
    /* kti1loadremotevn0reqcreditreference - Bits[11:11], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn0snpcreditreferencecount : 3;
    /* kti1remotevn0snpcreditreferencecount - Bits[14:12], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn0snpcreditreference : 1;
    /* kti1loadremotevn0snpcreditreference - Bits[15:15], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn0rspcreditreferencecount : 3;
    /* kti1remotevn0rspcreditreferencecount - Bits[18:16], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn0rspcreditreference : 1;
    /* kti1loadremotevn0rspcreditreference - Bits[19:19], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn0wbcreditreferencecount : 3;
    /* kti1remotevn0wbcreditreferencecount - Bits[22:20], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn0wbcreditreference : 1;
    /* kti1loadremotevn0wbcreditreference - Bits[23:23], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn0ncbcreditreferencecount : 3;
    /* kti1remotevn0ncbcreditreferencecount - Bits[26:24], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn0ncbcreditreference : 1;
    /* kti1loadremotevn0ncbcreditreference - Bits[27:27], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn0ncscreditreferencecount : 3;
    /* kti1remotevn0ncscreditreferencecount - Bits[30:28], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn0ncscreditreference : 1;
    /* kti1loadremotevn0ncscreditreference - Bits[31:31], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
  } Bits;
  UINT32 Data;
} M3INGDBGREMCRE0KTI1_M3KTI_MAIN_STRUCT;


/* M3INGDBGREMCRE1KTI1_M3KTI_MAIN_REG supported on:                           */
/*       SKX_A0 (0x4039017C)                                                  */
/*       SKX (0x4039017C)                                                     */
/* Register default value:              0x00000000                            */
#define M3INGDBGREMCRE1KTI1_M3KTI_MAIN_REG 0x0A00417C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd : 8;
    /* rsvd - Bits[7:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti1remotevn1reqcreditreferencecount : 3;
    /* kti1remotevn1reqcreditreferencecount - Bits[10:8], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn1reqcreditreference : 1;
    /* kti1loadremotevn1reqcreditreference - Bits[11:11], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn1snpcreditreferencecount : 3;
    /* kti1remotevn1snpcreditreferencecount - Bits[14:12], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn1snpcreditreference : 1;
    /* kti1loadremotevn1snpcreditreference - Bits[15:15], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn1rspcreditreferencecount : 3;
    /* kti1remotevn1rspcreditreferencecount - Bits[18:16], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn1rspcreditreference : 1;
    /* kti1loadremotevn1rspcreditreference - Bits[19:19], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn1wbcreditreferencecount : 3;
    /* kti1remotevn1wbcreditreferencecount - Bits[22:20], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn1wbcreditreference : 1;
    /* kti1loadremotevn1wbcreditreference - Bits[23:23], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn1ncbcreditreferencecount : 3;
    /* kti1remotevn1ncbcreditreferencecount - Bits[26:24], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn1ncbcreditreference : 1;
    /* kti1loadremotevn1ncbcreditreference - Bits[27:27], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
    UINT32 kti1remotevn1ncscreditreferencecount : 3;
    /* kti1remotevn1ncscreditreferencecount - Bits[30:28], RWS, default = 3'b000 
       For most aspects of normal operation, the UPI ingress logic gets its non-VNA 
       credit allocation (i.e. credits for each of VN0/VN1 crossed with 
       REQ/SNP/RSP/WB/NCS/NCB) for the remote socket via initial credit returns 
       transmitted by the remote socket. However, for error detection (e.g. credit 
       overflow) to work and for clock gating to be more efficient, the ingress logic 
       needs an independent reference for what the maximum credit non-VNA count is 
       supposed to be. The value of this field is this reference. Once this field is 
       programmed, the value needs to be activated by toggling the 
       LoadVnxCreditReference bit. (The same reference value is applied to each of the 
       12 non-VNA credit pools.) 
     */
    UINT32 kti1loadremotevn1ncscreditreference : 1;
    /* kti1loadremotevn1ncscreditreference - Bits[31:31], RWS, default = 1'b0 
       To activate a value programmed into the RemVnxCreditRefValue field, this bit 
       needs to be switched from 0 to 1 to 0. (If this bit is left at 1, then any 
       change in RemVnxCreditRefValue may cause undesired behavior.) 
     */
  } Bits;
  UINT32 Data;
} M3INGDBGREMCRE1KTI1_M3KTI_MAIN_STRUCT;






/* M3BGFTUNE_M3KTI_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x403901A0)                                                  */
/*       SKX (0x403901A0)                                                     */
/* Register default value:              0x00000000                            */
#define M3BGFTUNE_M3KTI_MAIN_REG 0x0A0041A0
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 bgfoverride : 1;
    /* bgfoverride - Bits[0:0], RW_L, default = 1'b0  */
    UINT32 uratio : 8;
    /* uratio - Bits[8:1], RW_L, default = 8'b00000000  */
    UINT32 rxptrdist : 3;
    /* rxptrdist - Bits[11:9], RW_L, default = 3'b000  */
    UINT32 rxbubbleinit : 9;
    /* rxbubbleinit - Bits[20:12], RW_L, default = 9'b000000000  */
    UINT32 txbubbleinit : 9;
    /* txbubbleinit - Bits[29:21], RW_L, default = 9'b000000000  */
    UINT32 uratiomatchsts : 1;
    /* uratiomatchsts - Bits[30:30], RW_LV, default = 1'b0  */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3BGFTUNE_M3KTI_MAIN_STRUCT;
















/* M3KUTLPRIVC0_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x403901D0)                                                  */
/*       SKX (0x403901D0)                                                     */
/* Register default value:              0x00000000                            */
#define M3KUTLPRIVC0_M3KTI_MAIN_REG 0x0A0041D0
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 msgchepckgatedis : 1;
    /* msgchepckgatedis - Bits[0:0], RWS, default = 1'b0 
       Message channl End point clock gating disable.
     */
    UINT32 crittercsrckgatedis : 1;
    /* crittercsrckgatedis - Bits[1:1], RWS, default = 1'b0 
       CSR register clock gating disable.
     */
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KUTLPRIVC0_M3KTI_MAIN_STRUCT;
















/* M3KEGRRSPGROUPBAGT1_M3KTI_MAIN_REG supported on:                           */
/*       SKX_A0 (0x403901F0)                                                  */
/*       SKX (0x403901F0)                                                     */
/* Register default value:              0x00000000                            */
#define M3KEGRRSPGROUPBAGT1_M3KTI_MAIN_REG 0x0A0041F0
/* Struct format extracted from XML file SKX_A0\3.18.0.CFG.xml.
 * Response function Group B Port1
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 14;
    UINT32 rsprestblockcrdretagt1 : 2;
    /* rsprestblockcrdretagt1 - Bits[15:14], RWS_L, default = 2'b0 
       RspRestBlockCrdRetAgt1
     */
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} M3KEGRRSPGROUPBAGT1_M3KTI_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 * Response function Group B Port1
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 14;
    UINT32 rsprestblockcrdretagt1 : 2;
    /* rsprestblockcrdretagt1 - Bits[15:14], RWS_L, default = 2'b0 
       RspRestBlockCrdRetAgt1
     */
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} M3KEGRRSPGROUPBAGT1_M3KTI_MAIN_STRUCT;



/* M3KINGPRIVC0_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390200)                                                  */
/*       SKX (0x40390200)                                                     */
/* Register default value:              0x00004060                            */
#define M3KINGPRIVC0_M3KTI_MAIN_REG 0x0A004200
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 adingressqueuedepth : 7;
    /* adingressqueuedepth - Bits[6:0], RWS_L, default = 7'd96  */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 blingressqueuedepth : 7;
    /* blingressqueuedepth - Bits[14:8], RWS_L, default = 7'd64  */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 disableadbypassidle : 1;
    /* disableadbypassidle - Bits[16:16], RWS_L, default = 1'b0  */
    UINT32 disableadbypassbusy0 : 1;
    /* disableadbypassbusy0 - Bits[17:17], RWS_L, default = 1'b0  */
    UINT32 disableadbypassbusy1 : 1;
    /* disableadbypassbusy1 - Bits[18:18], RWS_L, default = 1'b0  */
    UINT32 disableadbypassbusy2 : 1;
    /* disableadbypassbusy2 - Bits[19:19], RWS_L, default = 1'b0  */
    UINT32 usepurerrforarb : 1;
    /* usepurerrforarb - Bits[20:20], RWS_L, default = 1'b0  */
    UINT32 allowadarbwith1vna : 1;
    /* allowadarbwith1vna - Bits[21:21], RWS_L, default = 1'b0  */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KINGPRIVC0_M3KTI_MAIN_STRUCT;


/* M3KINGPRIVC1_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390204)                                                  */
/*       SKX (0x40390204)                                                     */
/* Register default value:              0x00000000                            */
#define M3KINGPRIVC1_M3KTI_MAIN_REG 0x0A004204
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 disableparalleladblarb : 1;
    /* disableparalleladblarb - Bits[0:0], RWS_L, default = 1'b0  */
    UINT32 disableparallelslotting : 1;
    /* disableparallelslotting - Bits[1:1], RWS_L, default = 1'b0  */
    UINT32 disableslottingrunahead : 1;
    /* disableslottingrunahead - Bits[2:2], RWS_L, default = 1'b0  */
    UINT32 disablevnswitchbeforedrain : 1;
    /* disablevnswitchbeforedrain - Bits[3:3], RWS_L, default = 1'b0  */
    UINT32 disableparalleladblarbbias : 1;
    /* disableparalleladblarbbias - Bits[4:4], RWS_L, default = 1'b0  */
    UINT32 disableratematchstalls : 1;
    /* disableratematchstalls - Bits[5:5], RWS_L, default = 1'b0  */
    UINT32 forceratematchstallon : 1;
    /* forceratematchstallon - Bits[6:6], RWS_L, default = 1'b0  */
    UINT32 forceratematchstallmax : 1;
    /* forceratematchstallmax - Bits[7:7], RWS_L, default = 1'b0  */
    UINT32 forcebothpumpsforallblmsg : 1;
    /* forcebothpumpsforallblmsg - Bits[8:8], RWS_L, default = 1'b0  */
    UINT32 require4creditsforheadertransmit : 1;
    /* require4creditsforheadertransmit - Bits[9:9], RWS_L, default = 1'b0  */
    UINT32 crittercsrckgatedis : 1;
    /* crittercsrckgatedis - Bits[10:10], RWS, default = 1'b0  */
    UINT32 rsvd_11 : 2;
    /* rsvd_11 - Bits[12:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 forcewaitforpump1beforebldealloc : 1;
    /* forcewaitforpump1beforebldealloc - Bits[13:13], RWS_L, default = 1'b0  */
    UINT32 disableserialadblarbonvna : 1;
    /* disableserialadblarbonvna - Bits[14:14], RWS_L, default = 1'b0  */
    UINT32 allowratematchingstallwhenuclkslower : 1;
    /* allowratematchingstallwhenuclkslower - Bits[15:15], RWS_L, default = 1'b0  */
    UINT32 disableblp1plimbo : 1;
    /* disableblp1plimbo - Bits[16:16], RWS_L, default = 1'b0  */
    UINT32 powermodeselect_ovr_enable : 1;
    /* powermodeselect_ovr_enable - Bits[17:17], RWS_L, default = 1'b0  */
    UINT32 powermodeselect_ovr_value : 2;
    /* powermodeselect_ovr_value - Bits[19:18], RWS_L, default = 2'b0  */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M3KINGPRIVC1_M3KTI_MAIN_STRUCT;


/* M3KINGPRIVC2_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390208)                                                  */
/*       SKX (0x40390208)                                                     */
/* Register default value:              0x00000000                            */
#define M3KINGPRIVC2_M3KTI_MAIN_REG 0x0A004208
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti0disableclkgatepredecode : 1;
    /* kti0disableclkgatepredecode - Bits[0:0], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatedecode : 1;
    /* kti0disableclkgatedecode - Bits[1:1], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatearbitration : 1;
    /* kti0disableclkgatearbitration - Bits[2:2], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgateslotting : 1;
    /* kti0disableclkgateslotting - Bits[3:3], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgateslotdrain : 1;
    /* kti0disableclkgateslotdrain - Bits[4:4], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgateadlistcoarse : 1;
    /* kti0disableclkgateadlistcoarse - Bits[5:5], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatebllistcoarse : 1;
    /* kti0disableclkgatebllistcoarse - Bits[6:6], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgateadlistfine : 1;
    /* kti0disableclkgateadlistfine - Bits[7:7], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatebllistfine : 1;
    /* kti0disableclkgatebllistfine - Bits[8:8], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgateprefetchlist : 1;
    /* kti0disableclkgateprefetchlist - Bits[9:9], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgate2scredit : 1;
    /* kti0disableclkgate2scredit - Bits[10:10], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatebgfcredit : 1;
    /* kti0disableclkgatebgfcredit - Bits[11:11], RWS_L, default = 1'b0  */
    UINT32 kti0disalbeclkgatemisc : 1;
    /* kti0disalbeclkgatemisc - Bits[12:12], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatebld2k : 1;
    /* kti0disableclkgatebld2k - Bits[13:13], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatemeshcrdret : 1;
    /* kti0disableclkgatemeshcrdret - Bits[14:14], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatingblp1p : 1;
    /* kti0disableclkgatingblp1p - Bits[15:15], RWS_L, default = 1'b0  */
    UINT32 kti0disableclkgatingparity : 1;
    /* kti0disableclkgatingparity - Bits[16:16], RWS_L, default = 1'b0  */
    UINT32 spare2 : 15;
    /* spare2 - Bits[31:17], RWS_L, default = 15'b0  */
  } Bits;
  UINT32 Data;
} M3KINGPRIVC2_M3KTI_MAIN_STRUCT;


/* M3KINGPRIVC3_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x4039020C)                                                  */
/*       SKX (0x4039020C)                                                     */
/* Register default value:              0x00000000                            */
#define M3KINGPRIVC3_M3KTI_MAIN_REG 0x0A00420C
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti1disableclkgatepredecode : 1;
    /* kti1disableclkgatepredecode - Bits[0:0], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatedecode : 1;
    /* kti1disableclkgatedecode - Bits[1:1], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatearbitration : 1;
    /* kti1disableclkgatearbitration - Bits[2:2], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgateslotting : 1;
    /* kti1disableclkgateslotting - Bits[3:3], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgateslotdrain : 1;
    /* kti1disableclkgateslotdrain - Bits[4:4], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgateadlistcoarse : 1;
    /* kti1disableclkgateadlistcoarse - Bits[5:5], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatebllistcoarse : 1;
    /* kti1disableclkgatebllistcoarse - Bits[6:6], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgateadlistfine : 1;
    /* kti1disableclkgateadlistfine - Bits[7:7], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatebllistfine : 1;
    /* kti1disableclkgatebllistfine - Bits[8:8], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgateprefetchlist : 1;
    /* kti1disableclkgateprefetchlist - Bits[9:9], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgate2scredit : 1;
    /* kti1disableclkgate2scredit - Bits[10:10], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatebgfcredit : 1;
    /* kti1disableclkgatebgfcredit - Bits[11:11], RWS_L, default = 1'b0  */
    UINT32 kti1disalbeclkgatemisc : 1;
    /* kti1disalbeclkgatemisc - Bits[12:12], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatebld2k : 1;
    /* kti1disableclkgatebld2k - Bits[13:13], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatemeshcrdret : 1;
    /* kti1disableclkgatemeshcrdret - Bits[14:14], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatingblp1p : 1;
    /* kti1disableclkgatingblp1p - Bits[15:15], RWS_L, default = 1'b0  */
    UINT32 kti1disableclkgatingparity : 1;
    /* kti1disableclkgatingparity - Bits[16:16], RWS_L, default = 1'b0  */
    UINT32 spare3 : 15;
    /* spare3 - Bits[31:17], RWS_L, default = 15'b0  */
  } Bits;
  UINT32 Data;
} M3KINGPRIVC3_M3KTI_MAIN_STRUCT;


/* M3KINGPRIVC4_M3KTI_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40390210)                                                  */
/*       SKX (0x40390210)                                                     */
/* Register default value:              0x00000000                            */
#define M3KINGPRIVC4_M3KTI_MAIN_REG 0x0A004210
/* Struct format extracted from XML file SKX\3.18.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 spare4 : 32;
    /* spare4 - Bits[31:0], RWS_L, default = 32'b0  */
  } Bits;
  UINT32 Data;
} M3KINGPRIVC4_M3KTI_MAIN_STRUCT;


#endif /* M3KTI_MAIN_h */
