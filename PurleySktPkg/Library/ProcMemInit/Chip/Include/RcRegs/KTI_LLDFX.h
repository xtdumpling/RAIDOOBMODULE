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

#ifndef KTI_LLDFX_h
#define KTI_LLDFX_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* KTI_LLDFX_DEV 14                                                           */
/* KTI_LLDFX_FUN 3                                                            */

/* VID_KTI_LLDFX_REG supported on:                                            */
/*       SKX_A0 (0x20373000)                                                  */
/*       SKX (0x20373000)                                                     */
/* Register default value:              0x8086                                */
#define VID_KTI_LLDFX_REG 0x09032000
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} VID_KTI_LLDFX_STRUCT;


/* DID_KTI_LLDFX_REG supported on:                                            */
/*       SKX_A0 (0x20373002)                                                  */
/*       SKX (0x20373002)                                                     */
/* Register default value:              0x205B                                */
#define DID_KTI_LLDFX_REG 0x09032002
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h205B 
        
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
} DID_KTI_LLDFX_STRUCT;


/* PCICMD_KTI_LLDFX_REG supported on:                                         */
/*       SKX_A0 (0x20373004)                                                  */
/*       SKX (0x20373004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_KTI_LLDFX_REG 0x09032004
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} PCICMD_KTI_LLDFX_STRUCT;


/* PCISTS_KTI_LLDFX_REG supported on:                                         */
/*       SKX_A0 (0x20373006)                                                  */
/*       SKX (0x20373006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_KTI_LLDFX_REG 0x09032006
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} PCISTS_KTI_LLDFX_STRUCT;


/* RID_KTI_LLDFX_REG supported on:                                            */
/*       SKX_A0 (0x10373008)                                                  */
/*       SKX (0x10373008)                                                     */
/* Register default value:              0x00                                  */
#define RID_KTI_LLDFX_REG 0x09031008
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} RID_KTI_LLDFX_STRUCT;


/* CCR_N0_KTI_LLDFX_REG supported on:                                         */
/*       SKX_A0 (0x10373009)                                                  */
/*       SKX (0x10373009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_KTI_LLDFX_REG 0x09031009
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_KTI_LLDFX_STRUCT;


/* CCR_N1_KTI_LLDFX_REG supported on:                                         */
/*       SKX_A0 (0x2037300A)                                                  */
/*       SKX (0x2037300A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_KTI_LLDFX_REG 0x0903200A
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} CCR_N1_KTI_LLDFX_STRUCT;


/* CLSR_KTI_LLDFX_REG supported on:                                           */
/*       SKX_A0 (0x1037300C)                                                  */
/*       SKX (0x1037300C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_KTI_LLDFX_REG 0x0903100C
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} CLSR_KTI_LLDFX_STRUCT;


/* PLAT_KTI_LLDFX_REG supported on:                                           */
/*       SKX_A0 (0x1037300D)                                                  */
/*       SKX (0x1037300D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_KTI_LLDFX_REG 0x0903100D
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} PLAT_KTI_LLDFX_STRUCT;


/* HDR_KTI_LLDFX_REG supported on:                                            */
/*       SKX_A0 (0x1037300E)                                                  */
/*       SKX (0x1037300E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_KTI_LLDFX_REG 0x0903100E
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} HDR_KTI_LLDFX_STRUCT;


/* BIST_KTI_LLDFX_REG supported on:                                           */
/*       SKX_A0 (0x1037300F)                                                  */
/*       SKX (0x1037300F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_KTI_LLDFX_REG 0x0903100F
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} BIST_KTI_LLDFX_STRUCT;


/* SVID_0_F3_KTI_LLDFX_REG supported on:                                      */
/*       SKX_A0 (0x2037302C)                                                  */
/*       SKX (0x2037302C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_0_F3_KTI_LLDFX_REG 0x0903202C
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} SVID_0_F3_KTI_LLDFX_STRUCT;


/* SDID_0_F3_KTI_LLDFX_REG supported on:                                      */
/*       SKX_A0 (0x2037302E)                                                  */
/*       SKX (0x2037302E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_0_F3_KTI_LLDFX_REG 0x0903202E
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} SDID_0_F3_KTI_LLDFX_STRUCT;


/* CAPPTR_KTI_LLDFX_REG supported on:                                         */
/*       SKX_A0 (0x10373034)                                                  */
/*       SKX (0x10373034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_KTI_LLDFX_REG 0x09031034
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} CAPPTR_KTI_LLDFX_STRUCT;


/* INTL_KTI_LLDFX_REG supported on:                                           */
/*       SKX_A0 (0x1037303C)                                                  */
/*       SKX (0x1037303C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_KTI_LLDFX_REG 0x0903103C
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} INTL_KTI_LLDFX_STRUCT;


/* INTPIN_KTI_LLDFX_REG supported on:                                         */
/*       SKX_A0 (0x1037303D)                                                  */
/*       SKX (0x1037303D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_KTI_LLDFX_REG 0x0903103D
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} INTPIN_KTI_LLDFX_STRUCT;


/* MINGNT_KTI_LLDFX_REG supported on:                                         */
/*       SKX_A0 (0x1037303E)                                                  */
/*       SKX (0x1037303E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_KTI_LLDFX_REG 0x0903103E
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} MINGNT_KTI_LLDFX_STRUCT;


/* MAXLAT_KTI_LLDFX_REG supported on:                                         */
/*       SKX_A0 (0x1037303F)                                                  */
/*       SKX (0x1037303F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_KTI_LLDFX_REG 0x0903103F
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
} MAXLAT_KTI_LLDFX_STRUCT;


/* PXPCAP_0_F3_KTI_LLDFX_REG supported on:                                    */
/*       SKX_A0 (0x40373040)                                                  */
/*       SKX (0x40373040)                                                     */
/* Register default value:              0x00910010                            */
#define PXPCAP_0_F3_KTI_LLDFX_REG 0x09034040
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
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
    /* capability_version - Bits[19:16], RO, default = 4'b1 
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
} PXPCAP_0_F3_KTI_LLDFX_STRUCT;


/* KTIMISCSTAT_KTI_LLDFX_REG supported on:                                    */
/*       SKX_A0 (0x403730D4)                                                  */
/*       SKX (0x403730D4)                                                     */
/* Register default value:              0x00000003                            */
#define KTIMISCSTAT_KTI_LLDFX_REG 0x090340D4
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 * Intel UPI Misc Status
 */
typedef union {
  struct {
    UINT32 kti_rate : 3;
    /* kti_rate - Bits[2:0], RO_V, default = 3'b011 
       This reflects the supported current Intel UPI rate setting into the PLL.
       100 - 9.6 GT/s
       101 - 10.4GT/s
       other - Reserved
       Note: The default value of 3'b011 does not reflect the actual Intel UPI rate.  
       Reads of this register field will always report one of the legal defined values 
       above. 
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIMISCSTAT_KTI_LLDFX_STRUCT;






































/* CC_DCC_OVRD_KTI_LLDFX_REG supported on:                                    */
/*       SKX_A0 (0x40373210)                                                  */
/*       SKX (0x40373210)                                                     */
/* Register default value:              0x00000000                            */
#define CC_DCC_OVRD_KTI_LLDFX_REG 0x09034210
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 txdcc_stopovrden : 1;
    /* txdcc_stopovrden - Bits[0:0], RWS_LB, default = 1'b0 
        
     */
    UINT32 txdcc_stopovrdval : 1;
    /* txdcc_stopovrdval - Bits[1:1], RWS_LB, default = 1'b0 
        
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 txdcc_startovrden : 1;
    /* txdcc_startovrden - Bits[4:4], RWS_LB, default = 1'b0 
        
     */
    UINT32 txdcc_startovrdval : 1;
    /* txdcc_startovrdval - Bits[5:5], RWS_LB, default = 1'b0 
        
     */
    UINT32 rsvd_6 : 1;
    /* rsvd_6 - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 txdcc_extovrden : 1;
    /* txdcc_extovrden - Bits[7:7], RWS_LB, default = 1'b0 
        
     */
    UINT32 txdcc_extval : 6;
    /* txdcc_extval - Bits[13:8], RWS_LB, default = 6'b0 
        
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rxdcc_stopovrden : 1;
    /* rxdcc_stopovrden - Bits[16:16], RWS_LB, default = 1'b0 
        
     */
    UINT32 rxdcc_stopovrdval : 1;
    /* rxdcc_stopovrdval - Bits[17:17], RWS_LB, default = 1'b0 
        
     */
    UINT32 rsvd_18 : 2;
    /* rsvd_18 - Bits[19:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rxdcc_startovrden : 1;
    /* rxdcc_startovrden - Bits[20:20], RWS_LB, default = 1'b0 
        
     */
    UINT32 rxdcc_startovrdval : 1;
    /* rxdcc_startovrdval - Bits[21:21], RWS_LB, default = 1'b0 
        
     */
    UINT32 rsvd_22 : 1;
    /* rsvd_22 - Bits[22:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rxdcc_extovrden : 1;
    /* rxdcc_extovrden - Bits[23:23], RWS_LB, default = 1'b0 
        
     */
    UINT32 rxdcc_extval : 6;
    /* rxdcc_extval - Bits[29:24], RWS_LB, default = 6'b0 
        
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CC_DCC_OVRD_KTI_LLDFX_STRUCT;




/* CC_DFX_MON0_KTI_LLDFX_REG supported on:                                    */
/*       SKX_A0 (0x40373218)                                                  */
/*       SKX (0x40373218)                                                     */
/* Register default value:              0x00000001                            */
#define CC_DFX_MON0_KTI_LLDFX_REG 0x09034218
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 dmon_sel : 7;
    /* dmon_sel - Bits[6:0], RWS_LB, default = 7'b1 
        
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dfxp_sel : 6;
    /* dfxp_sel - Bits[13:8], RWS_LB, default = 6'b0 
        
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dfxp0_function_sel : 2;
    /* dfxp0_function_sel - Bits[17:16], RWS_LB, default = 2'b0 
        
     */
    UINT32 dfxp1_function_sel : 2;
    /* dfxp1_function_sel - Bits[19:18], RWS_LB, default = 2'b0 
        
     */
    UINT32 dfxp0_dmon_module_sel : 3;
    /* dfxp0_dmon_module_sel - Bits[22:20], RWS_LB, default = 3'h0 
        
     */
    UINT32 dfxp1_dmon_module_sel : 3;
    /* dfxp1_dmon_module_sel - Bits[25:23], RWS_LB, default = 3'h0 
        
     */
    UINT32 dfxp0_module_sel : 2;
    /* dfxp0_module_sel - Bits[27:26], RWS_LB, default = 2'b0 
        
     */
    UINT32 dfxp1_module_sel : 2;
    /* dfxp1_module_sel - Bits[29:28], RWS_LB, default = 2'b0 
        
     */
    UINT32 dfx_lane_sel : 2;
    /* dfx_lane_sel - Bits[31:30], RWS_LB, default = 2'b0 
        
     */
  } Bits;
  UINT32 Data;
} CC_DFX_MON0_KTI_LLDFX_STRUCT;






/* CC_ICOMP_KTI_LLDFX_REG supported on:                                       */
/*       SKX_A0 (0x40373228)                                                  */
/*       SKX (0x40373228)                                                     */
/* Register default value:              0x00000001                            */
#define CC_ICOMP_KTI_LLDFX_REG 0x09034228
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 tx_icomp_rc_time : 2;
    /* tx_icomp_rc_time - Bits[1:0], RWS_LB, default = 2'b1 
        
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tx_icomp_gain_sel : 4;
    /* tx_icomp_gain_sel - Bits[7:4], RWS_LB, default = 4'b0 
        
     */
    UINT32 tx_icomp_votes : 2;
    /* tx_icomp_votes - Bits[9:8], RWS_LB, default = 2'b0 
        
     */
    UINT32 rsvd_10 : 22;
    /* rsvd_10 - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CC_ICOMP_KTI_LLDFX_STRUCT;












/* CC_MISC_CTL_1_KTI_LLDFX_REG supported on:                                  */
/*       SKX_A0 (0x40373244)                                                  */
/*       SKX (0x40373244)                                                     */
/* Register default value:              0x00200000                            */
#define CC_MISC_CTL_1_KTI_LLDFX_REG 0x09034244
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 8;
    /* rsvd_0 - Bits[7:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 icomp_track : 1;
    /* icomp_track - Bits[8:8], RWS_LB, default = 1'b0 
        
     */
    UINT32 rsvd_9 : 6;
    /* rsvd_9 - Bits[14:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 blnclegs_ctl : 6;
    /* blnclegs_ctl - Bits[20:15], RWS_LB, default = 6'h0 
        
     */
    UINT32 blnclegs_dis : 1;
    /* blnclegs_dis - Bits[21:21], RWS_LB, default = 1'b1 
        
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 hvm_mode : 8;
    /* hvm_mode - Bits[31:24], RWS_LB, default = 8'b0 
        
     */
  } Bits;
  UINT32 Data;
} CC_MISC_CTL_1_KTI_LLDFX_STRUCT;




/* CC_SPARE_KTI_LLDFX_REG supported on:                                       */
/*       SKX_A0 (0x40373250)                                                  */
/*       SKX (0x40373250)                                                     */
/* Register default value:              0x0000000C                            */
#define CC_SPARE_KTI_LLDFX_REG 0x09034250
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 cc_spare : 16;
    /* cc_spare - Bits[15:0], RWS_LB, default = 16'hC 
        
     */
    UINT32 cc_spare1 : 16;
    /* cc_spare1 - Bits[31:16], RWS_LB, default = 16'h0 
        
     */
  } Bits;
  UINT32 Data;
} CC_SPARE_KTI_LLDFX_STRUCT;






/* KTIMISC_LBC_CONTROL_KTI_LLDFX_REG supported on:                            */
/*       SKX_A0 (0x40373268)                                                  */
/*       SKX (0x40373268)                                                     */
/* Register default value:              0x00000000                            */
#define KTIMISC_LBC_CONTROL_KTI_LLDFX_REG 0x09034268
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 * Load Bus control register
 */
typedef union {
  struct {
    UINT32 lbc_start : 1;
    /* lbc_start - Bits[0:0], RW_LBV, default = 1'b0 
       The SA sets this bit when a new request is sent out to the load bus controller. 
       The Ack from the load bus controller resets this bit once the operation is 
       completed. 
     */
    UINT32 lbc_req : 2;
    /* lbc_req - Bits[2:1], RW_LB, default = 2'b0 
       00 - Reserved
       01 - read
       10 - write
       11 - read-modify-write
     */
    UINT32 lbc_ld_sel : 7;
    /* lbc_ld_sel - Bits[9:3], RW_LB, default = 7'b0 
       Selects the EP on the load bus or monitor bus.
     */
    UINT32 lbc_ln_sel : 22;
    /* lbc_ln_sel - Bits[31:10], RW_LB, default = 22'b0 
       For writes, reads, and read-modify-writes this acts as the selection of which 
       lanes have to go through the respective transaction.  
       [10] - UPICOM
       [31:11] - Reserved
     */
  } Bits;
  UINT32 Data;
} KTIMISC_LBC_CONTROL_KTI_LLDFX_STRUCT;


/* KTIMISC_LBC_DATA_KTI_LLDFX_REG supported on:                               */
/*       SKX_A0 (0x4037326C)                                                  */
/*       SKX (0x4037326C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIMISC_LBC_DATA_KTI_LLDFX_REG 0x0903426C
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 * Load Bus data register
 */
typedef union {
  struct {
    UINT32 lbc_data_valid : 1;
    /* lbc_data_valid - Bits[0:0], RW_LBV, default = 1'b0 
       The LBC sets this bit in a read transaction when the data is ready to be read.
     */
    UINT32 lbc_data : 14;
    /* lbc_data - Bits[14:1], RW_LBV, default = 14'b0 
       Reads: This is the data that is read out from the particular lane with the 
       monitor bus. 
        Writes: This is the data which needs to be written into the selected lanes 
       using the load bus. 
        Read-modify-Writes: This is the data that needs to be used as an offset for the 
       RMW on the lanes specified by the lane select. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[30:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lbc_double_tmr : 1;
    /* lbc_double_tmr - Bits[31:31], RWS_LB, default = 1'b0 
       Set to 1 to double the load bus controller wait timer.
     */
  } Bits;
  UINT32 Data;
} KTIMISC_LBC_DATA_KTI_LLDFX_STRUCT;






/* KTIMISCMODLCK_KTI_LLDFX_REG supported on:                                  */
/*       SKX_A0 (0x40373300)                                                  */
/*       SKX (0x40373300)                                                     */
/* Register default value:              0x00000000                            */
#define KTIMISCMODLCK_KTI_LLDFX_REG 0x09034300
/* Struct format extracted from XML file SKX\3.14.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 iodfxlck_and_glblcsrlcken : 1;
    /* iodfxlck_and_glblcsrlcken - Bits[0:0], ROS_V, default = 1'b0 
       IO DFX lock and global csr lock enable status
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIMISCMODLCK_KTI_LLDFX_STRUCT;




#endif /* KTI_LLDFX_h */
