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

#ifndef M2MEM_MAIN_h
#define M2MEM_MAIN_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* M2MEM_MAIN_DEV 8                                                           */
/* M2MEM_MAIN_FUN 0                                                           */

/* VID_M2MEM_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x20240000)                                                  */
/*       SKX (0x20240000)                                                     */
/* Register default value:              0x8086                                */
#define VID_M2MEM_MAIN_REG 0x06002000
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} VID_M2MEM_MAIN_STRUCT;


/* DID_M2MEM_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x20240002)                                                  */
/*       SKX (0x20240002)                                                     */
/* Register default value:              0x2066                                */
#define DID_M2MEM_MAIN_REG 0x06002002
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2066 
        
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
} DID_M2MEM_MAIN_STRUCT;


/* PCICMD_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x20240004)                                                  */
/*       SKX (0x20240004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_M2MEM_MAIN_REG 0x06002004
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} PCICMD_M2MEM_MAIN_STRUCT;


/* PCISTS_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x20240006)                                                  */
/*       SKX (0x20240006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_M2MEM_MAIN_REG 0x06002006
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} PCISTS_M2MEM_MAIN_STRUCT;


/* RID_M2MEM_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x10240008)                                                  */
/*       SKX (0x10240008)                                                     */
/* Register default value:              0x00                                  */
#define RID_M2MEM_MAIN_REG 0x06001008
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} RID_M2MEM_MAIN_STRUCT;


/* CCR_N0_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x10240009)                                                  */
/*       SKX (0x10240009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_M2MEM_MAIN_REG 0x06001009
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_M2MEM_MAIN_STRUCT;


/* CCR_N1_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x2024000A)                                                  */
/*       SKX (0x2024000A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_M2MEM_MAIN_REG 0x0600200A
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} CCR_N1_M2MEM_MAIN_STRUCT;


/* CLSR_M2MEM_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x1024000C)                                                  */
/*       SKX (0x1024000C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_M2MEM_MAIN_REG 0x0600100C
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} CLSR_M2MEM_MAIN_STRUCT;


/* PLAT_M2MEM_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x1024000D)                                                  */
/*       SKX (0x1024000D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_M2MEM_MAIN_REG 0x0600100D
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} PLAT_M2MEM_MAIN_STRUCT;


/* HDR_M2MEM_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x1024000E)                                                  */
/*       SKX (0x1024000E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_M2MEM_MAIN_REG 0x0600100E
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} HDR_M2MEM_MAIN_STRUCT;


/* BIST_M2MEM_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x1024000F)                                                  */
/*       SKX (0x1024000F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_M2MEM_MAIN_REG 0x0600100F
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} BIST_M2MEM_MAIN_STRUCT;


/* SVID_M2MEM_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x2024002C)                                                  */
/*       SKX (0x2024002C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_M2MEM_MAIN_REG 0x0600202C
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * Subsystem Vendor Identification Number
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_M2MEM_MAIN_STRUCT;


/* SDID_M2MEM_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x2024002E)                                                  */
/*       SKX (0x2024002E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_M2MEM_MAIN_REG 0x0600202E
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * Subsystem Device Identification Number
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_M2MEM_MAIN_STRUCT;


/* CAPPTR_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x10240034)                                                  */
/*       SKX (0x10240034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_M2MEM_MAIN_REG 0x06001034
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} CAPPTR_M2MEM_MAIN_STRUCT;


/* INTL_M2MEM_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x1024003C)                                                  */
/*       SKX (0x1024003C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_M2MEM_MAIN_REG 0x0600103C
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} INTL_M2MEM_MAIN_STRUCT;


/* INTPIN_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x1024003D)                                                  */
/*       SKX (0x1024003D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_M2MEM_MAIN_REG 0x0600103D
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} INTPIN_M2MEM_MAIN_STRUCT;


/* MINGNT_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x1024003E)                                                  */
/*       SKX (0x1024003E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_M2MEM_MAIN_REG 0x0600103E
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} MINGNT_M2MEM_MAIN_STRUCT;


/* MAXLAT_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x1024003F)                                                  */
/*       SKX (0x1024003F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_M2MEM_MAIN_REG 0x0600103F
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} MAXLAT_M2MEM_MAIN_STRUCT;


/* PXPCAP_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x40240040)                                                  */
/*       SKX (0x40240040)                                                     */
/* Register default value:              0x00910010                            */
#define PXPCAP_M2MEM_MAIN_REG 0x06004040
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} PXPCAP_M2MEM_MAIN_STRUCT;


/* MODE_M2MEM_MAIN_REG supported on:                                          */
/*       SKX_A0 (0x40240080)                                                  */
/*       SKX (0x40240080)                                                     */
/* Register default value:              0x00000000                            */
#define MODE_M2MEM_MAIN_REG 0x06004080
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Configuration register for the main m2m modes.
 *       Important configuration restrictions:
 *         - Isoch is not supported by the m2m if DDRT is present on a channel.
 *         - DDRT scheduling and Isoch mode need to be mutually exclusive within 
 * the m2m. 
 *         - Isoch is not supported if any channel gets serviced by FNV.
 *         - Mirroring and Isoch mode are mutually exclusive.
 *       Whenever mirroring is enabled on a mesh2mem, then also following bits need 
 * to get set on that mesh2mem: 
 *        - SysFeatures[PcommitForceBroadcast]=1 (functional requirement)
 *        - Defeatures0[Wait4BothHalves]=1       (validation space reduction)
 *        - Defeatures0[IngBypDis]=1             (functional requirement for 
 * SysFeatures0[CfgImmediateFailoverActionEna]=1, otherwise validation space 
 * reduction) 
 *        - Sysfeatures0[PrefDisable]=1  (functional requirement for 
 * SysFeatures0[CfgImmediateFailoverActionEna]=1, otherwise validation space 
 * reduction) 
 *        - Defeatures0[EgrBypDis]=1             (validation space reduction)
 *       
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 4;
    /* rsvd_0 - Bits[3:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 isoch : 1;
    /* isoch - Bits[4:4], RWS_L, default = 1'h0 
       
               Isochronous Flows (Isoch):
               Enable Isochronous flows.
               It is forbidden to enable isochronous flows, as it might result in 
       system hangs,  
               when either mirror is enabled, or when DDRT is populated, e.g. DDRT is 
       present. 
               
     */
    UINT32 blockregion : 1;
    /* blockregion - Bits[5:5], RWS_L, default = 1'h0 
       
               Block Region (BlockRegion):
               Enable block region memory flows. Furthermore, 
       MmCapabilities[CapBlockRegionEn] needs  
               to be 1 for block region support.
               
     */
    UINT32 pmem : 1;
    /* pmem - Bits[6:6], RWS_L, default = 1'h0 
       
               Persistent Memory (PMem):
               Enable persistent memory flows. Furthermore, MmCapabilities[CapPMemEn] 
       needs to be 1 for PMem support. 
               
     */
    UINT32 pmemcaching : 1;
    /* pmemcaching - Bits[7:7], RWS_L, default = 1'h0 
       
               Persistent Memory Caching (PMemCaching):
               Enable persistent memory caching in the Near Memory (but still always 
       writes to persistent memory). 
               Persistent Mode needs to be enabled too. In SKX, this feature is only 
       supported as a Software Development Vehicle,  
               it is NOT supported for real usage system deployment.
               
     */
    UINT32 nmcaching : 1;
    /* nmcaching - Bits[8:8], RWS_L, default = 1'h0 
       
               Near Memory Caching (NMCaching):
               Enable Near Memory caching, i.e. two level memory with the first level 
       (i.e. the near memory)  
               having the capability to cache second level memory (i.e. far memory). 
       Setting this bit also  
               implies there is a second memory level present.
               Furthermore, in this implementation, near memory accesses get scheduled 
       by the DDR4 scheduler  
               while the far memory gets serviced by the DDRT scheduler.
               Note, also MmCapabilities[CapNMcachingEn] needs to be 1 for Near Memory 
       Caching. 
               
     */
    UINT32 mirrorddr4 : 1;
    /* mirrorddr4 - Bits[9:9], RWS_L, default = 1'h0 
       
               DDR4 Mirroring (MirrorDDR4):
               Enable DDR4 mirroring. It is illegal to set this bit when Near Memory 
       Caching is enabled. 
               MmCapabilities[CapMirrorEn] needs to be 1 for mirroring.
               
     */
    UINT32 mirrorddrt : 1;
    /* mirrorddrt - Bits[10:10], RWS_L, default = 1'h0 
       
               DDRT Mirroring (MirrorDDRT):
               Enable DDRT mirroring. MmCapabilities[CapMirrorEn] needs to be 1 for 
       mirroring. 
               
     */
    UINT32 adddc : 1;
    /* adddc - Bits[11:11], RWS_L, default = 1'h0 
       
               Unused: available for ECO if necessary.
               
     */
    UINT32 rsvd_12 : 20;
    /* rsvd_12 - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MODE_M2MEM_MAIN_STRUCT;


/* TOPOLOGY_M2MEM_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x40240088)                                                  */
/*       SKX (0x40240088)                                                     */
/* Register default value:              0x00018000                            */
#define TOPOLOGY_M2MEM_MAIN_REG 0x06004088
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Configuration register related to system/socket topology.
 *       The socket topology information is needed for PRE info in direct-to-core 
 * (D2C), and  
 *       PCLS info in BL MemData. More specifically, the PRE and PCLS fields 
 *       in data packets carry information about SNC cluster remoteness.
 *       
 */
typedef union {
  struct {
    UINT32 mynid : 4;
    /* mynid - Bits[3:0], RWS_L, default = 1'h0 
       
               My NID (MyNID):
               NodeID of this MC (more accurately, this mesh2mem).
               
     */
    UINT32 rsvd_4 : 6;
    /* rsvd_4 - Bits[9:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sncluster : 1;
    /* sncluster - Bits[10:10], RW_L, default = 1'h0 
       
               SNC cluster (SNCluster):
               SNC cluster this m2m belongs to: cluster 1 or 0.
               
     */
    UINT32 rsvd_11 : 2;
    /* rsvd_11 - Bits[12:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 basecluster1 : 6;
    /* basecluster1 - Bits[18:13], RW_L, default = 6'd12 
       
               Base of Cluster 1 (BaseCluster1):
               Core ID of the first core of cluster 1.
               Core IDs are assigned contiguously within a cluster.
               Cluster 0: 0...M-1, cluster 1: M...N. In this case 
               this field should be set to the value M.
               
     */
    UINT32 rsvd_19 : 13;
    /* rsvd_19 - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TOPOLOGY_M2MEM_MAIN_STRUCT;


/* NID2KTI_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x40240090)                                                  */
/*       SKX (0x40240090)                                                     */
/* Register default value:              0x00000000                            */
#define NID2KTI_M2MEM_MAIN_REG 0x06004090
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Configuration register mapping the requester NodeID to a Intel UPI port.
 *       Only used for direct-to-UPI (D2K) Data_Cmp return.
 *       
 */
typedef union {
  struct {
    UINT32 nid2ktimap : 16;
    /* nid2ktimap - Bits[15:0], RW_L, default = 16'h0 
       
               Requester NodeID to Intel UPI link map (NID2KTImap):
               Specifies which Intel UPI link a Data_Cmp needs to be sent to based on 
       the requester NodeID. 
               The NID picks a pair of adjacent bits to determine the Intel UPI link.
                 Intel UPI link = NID2KTImap[ 2*NID+1 : 2*NID ]
               With Intel UPI link encodings: 00=Intel UPI 0, 01=Intel UPI 1, 10=Intel 
       UPI 2. 
               
     */
    UINT32 kti2slinkintlvena : 1;
    /* kti2slinkintlvena - Bits[16:16], RW_L, default = 1'h0 
       
               Two socket Intel UPI link interleave Enable (KTI2SlinkIntlvEna):
               A one indicates that messages, between the two sockets in this 2S (or 
       2S+XNC) system, get interleaved across Intel UPI link0 and Intel UPI link1 based 
       on address bits (system address bits [12]XOR[6]). This configuration bit in M2M 
       is only relevant when D2K in M2M is enabled. 
               Only legal to enable in a 2S where the two sockets are interconnected 
       through a dual link, more specifically it needs to be link0 and link1. Other 
       agents on the socket (e.g. CHA) also need to be programmed to handle the 
       interleave. If an XNC is connected in this 2S system then it needs to use link2 
       to connect the XNC to a/the socket(s). Note, the NID2UPImap still maps NIDs to 
       link0/1 or link2. Both, the Intel UPI0 as well as the Intel UPI 1 encoding in 
       NID2KTImap will map to the link0/1 interleave.  
               
     */
    UINT32 swapktilinkintlv : 1;
    /* swapktilinkintlv - Bits[17:17], RW_L, default = 1'h0 
       
               Swap Two socket Intel UPI link interleave Enable (SwapKTILinkIntlv):
               This is required to support twisted board topologies where link0 on one 
       socket is connected to link1 on the other socket in 2S-2Link configurations and 
       with EnableXorBasedKTILink set. 
               
     */
    UINT32 d2k2s3ktiena : 1;
    /* d2k2s3ktiena - Bits[18:18], RW_L, default = 1'h0 
       
               2S 3 Intel UPI configuration D2K enable (D2K2S3KTIEna):
               Enable D2K in 2S 3 Intel UPI systems.
               
     */
    UINT32 rsvd : 13;
    /* rsvd - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} NID2KTI_M2MEM_MAIN_STRUCT;


/* ADD1_LUT_N0_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x40240098)                                                  */
/*       SKX (0x40240098)                                                     */
/* Register default value:              0x0F0F0F0F                            */
#define ADD1_LUT_N0_M2MEM_MAIN_REG 0x06004098
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Determines, for which destinations, BL egress needs to use the ADD1 CMS 
 * interface (vs. the ADD0 CMS interface). 
 *       Any value written to this register is expected to be legal.
 *       However for performance reasons one wants to tune this register so that 
 * destinations in the same mesh column  
 *       get spread across the ADD1 and ADD0 interfaces. Note that transgress 
 * traffic always takes the same ADD interface (lets say ADD1),  
 *       hence a slight bias (for CHA/Intel UPI destinations) towards ADD0 might be 
 * desirable if CPU is part of a multi-socket system. 
 *       
 */
typedef union {
  struct {
    UINT32 chatoadd1map : 28;
    /* chatoadd1map - Bits[27:0], RWS, default = 28'hf0f0f0f 
       
               CHA to CMS ADD1 map (ChaToAdd1Map):
               Bit position i maps to CHAi. A 1 means that BL egress traffic to that 
       CHA will  
               be sent through CMS ADD1. A 0 implies use of ADD0.
               
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ADD1_LUT_N0_M2MEM_MAIN_STRUCT;


/* ADD1_LUT_N1_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x4024009C)                                                  */
/*       SKX (0x4024009C)                                                     */
/* Register default value:              0x48000000                            */
#define ADD1_LUT_N1_M2MEM_MAIN_REG 0x0600409C
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Determines, for which destinations, BL egress needs to use the ADD1 CMS 
 * interface (vs. the ADD0 CMS interface). 
 *       Any value written to this register is expected to be legal.
 *       However for performance reasons one wants to tune this register so that 
 * destinations in the same mesh column  
 *       get spread across the ADD1 and ADD0 interfaces. Note that transgress 
 * traffic always takes the same ADD interface (lets say ADD1),  
 *       hence a slight bias (for CHA/Intel UPI destinations) towards ADD0 might be 
 * desirable if CPU is part of a multi-socket system. 
 *       
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 27;
    /* rsvd_0 - Bits[26:0], n/a, default = n/a 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Padding added by header generation tool.
     */
    UINT32 transgresstoadd1map : 1;
    /* transgresstoadd1map - Bits[27:27], RWS, default = 1'b1 
       
               Trangress to CMS ADD1 map (TgrToAdd1Map):
               A 1 means that all BL transgress traffic will take CMS ADD1. A 0 implies 
       use of ADD0. 
               For SKX, Transgress requests should always use add port 1.
               
     */
    UINT32 rsvd_28 : 1;
    /* rsvd_28 - Bits[28:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ktitoadd1map : 2;
    /* ktitoadd1map - Bits[30:29], RWS, default = 2'b10 
       
               Intel UPI to CMS ADD1 map (KtiToAdd1Map):
               Bit 61 is for Intel UPI 0/1, while bit 62 is for Intel UPI 2. A 1 means 
       that BL egress traffic to that an Intel UPI agent will  
               be sent through CMS ADD1. A 0 implies use of ADD0.
               For SKX, D2K responses should only use add port 0
               
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ADD1_LUT_N1_M2MEM_MAIN_STRUCT;


/* CREDITS_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x402400A0)                                                  */
/*       SKX (0x402400A0)                                                     */
/* Register default value:              0x00000000                            */
#define CREDITS_M2MEM_MAIN_REG 0x060040A0
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Register to configure and read credits.
 *       The values written when CrdtWrEn=1 take effect immediately and don't need 
 * any reset. 
 *       In fact, any warm or cold reset will erase the programmed values and hence 
 * they'll need to be reprogrammed after a reset. 
 *       Valid ranges (per channel):
 *         DDR4 RPQ: 0 - 48 (>1 if DDR4 present on that channel);
 *         DDR4 WPQ: 0 - 40 (<40 if patrol scrubbing enabled in that channel, (>1 
 * if DDR4 present on that channel)); 
 *         DDRT RPQ: 0 - 48 (>1 if DDRT present on that channel);
 *         DDRT WPQ: 0 - 32 (>1 if DDRT present on that channel);
 *         WDSB: 0 - 16; (>0 if DDRT or DDR4 present on that channel);
 *         Scoreboard: 4 - 125.  Of the 128 entries, 1 is for patrol, one is 
 * internally Score-Board reserved (for its anti-starvation), one for M2M 
 * anti-starvation (when M2M credit anti-starvation is enabled); 
 *       Valid ranges (per UPI agent):
 *         UPI agent's RSP ingress (for D2K data): 0 - 15 (>0 if D2K enabled);
 *       Valid ranges:
 *         PREF:   0 - 31;
 *         AD_TGR: 0 - 15; (>0 if NmCaching enabled);
 *         BL_TGR: 0 - 15; (>0 if dir updates enabled (>1S), or if NmCaching 
 * enabled); 
 *         BGF threshold: 0 - 10 (BGF has 12 entries);
 *       Ranges are further limited by following restrictions:
 *         If Isoch is enabled then RPQ > high priority rd threshold > critical 
 * priority rd threshold. 
 *         If Isoch is enabled then WPQ > high priority wr threshold > critical 
 * priority wr threshold. 
 *       Suggested performance settings (assuming patrol scubbing and D2K are 
 * enabled): 
 *         DDR4 RPQ: 48;
 *         DDR4 WPQ: 39;
 *         DDRT RPQ: 48;
 *         DDRT WPQ: 32;
 *         WDSB: 16 (Write Data Staging Buffer/wr-tracker);
 *         Scoreboard: 125;
 *         UPI agent's RSP ingress for D2K data: 4, or whatever is available from 
 * UPI agent. Needs performance tuning per SKU; 
 *         BL_TGR: 8 (=64-2x28). If less than 28 CHAs on the SKU then can set to 
 * higher value. 
 *       When NmCaching is disabled so no AD_TGR needed:
 *         PREF: 8 (=64-2x28). If less than 28 CHAs on the SKU then can set to 
 * higher value; 
 *         AD_TGR: 0 (there is no AD_TGR traffic when NmCaching is not enabled).
 *       When NmCaching is enabled then XPT/UPI prefetch is not supported:
 *         PREF: 0;
 *         AD_TGR: 8 (=64-2x28). If less than 28 CHAs on the SKU then can set to 
 * higher value. 
 *       
 */
typedef union {
  struct {
    UINT32 crdtcnt : 8;
    /* crdtcnt - Bits[7:0], RW_V, default = 1'h0 
       
               Credit Count (CrdtCnt):
               Credit value to be written into the credit counter (if CrdtWrEn), or 
               credit value read from the credit counter (if CrdtRdEn). 
               
     */
    UINT32 rsvd_8 : 2;
    /* rsvd_8 - Bits[9:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 crdttype : 6;
    /* crdttype - Bits[15:10], RW, default = 1'h0 
       
               Credit Type (CrdtType):
               Credit type to be written/read. In general the upper 2 bits indicate the 
       physical memory channel. 
               The four lsb's encode the credit type.
               CrdtType[5:4]:
                 00: Physical Memory Channel 0 / Intel UPI 0
                 01: Physical Memory Channel 1 / Intel UPI 1
                 10: Physical Memory Channel 2 / Intel UPI 2
               CrdtType[3:0] (* = minimum 2 credits if that channel populated):
                  0000: DDR4 RPQ *.
                  0001: DDR4 WPQ * (all wrs).
                  0100: DDRT RPQ *.
                  0101: DDRT WPQ * (all wrs).
                  1000: WDSB * (shared between DDR4 and DDRT).
                  1010: Scoreboard - min of 4 and max of 126 to account for patrol and 
       anti-starvation. 
                  1011: Prefetch entries in IngressQ * (min 2 credits if prefetch 
       enabled). 
                  1100: AD IngressQ Transgres entries.
                  1101: BL IngressQ Transgres entries.
                  1110: BGF thresholds (above which BGF is considered full).
                  1111: Intel UPI agent (D2K data).
               WDSB = Write Data Staging Buffer/write-tracker.
               
     */
    UINT32 rsvd_16 : 1;
    /* rsvd_16 - Bits[16:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 crdtrden : 1;
    /* crdtrden - Bits[17:17], RW_L, default = 1'h0 
       
               Credit Read Enable (CrdtRdEn):
               Read the credit counter as specified by the Credit Type field.
               The result of the read is available in the Credit Count field.
               The credit value returned will be the value programmed last time for 
       that credit type (or the default reset value for the credit type if reading 
       right after reset). 
               One gets no dynamic visibility through this mechanism of how many 
       credits are unconsumed while traffic is ongoing. 
               
     */
    UINT32 crdtwren : 1;
    /* crdtwren - Bits[18:18], RW_LV, default = 1'h0 
       
               Credit Write Enable (CrdtWrEn):
               Write the credit counter as specified by the Credit Type field with the 
       value  
               from the Credit Count field.
               Software must ensure that this MC tile is quiesced (i.e. all credits 
       were returned to  
               the credit counter) when setting this Write Enable bit. If not quiesced 
       then queue  
               overflows and queue state corruption can potentially occur.
               Whenever this field is 1, the credits take effect immediately without 
       waiting for any reset. 
               After writing all credits, make sure this field gets written with a 0.
               
     */
    UINT32 rsvd_19 : 13;
    /* rsvd_19 - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CREDITS_M2MEM_MAIN_STRUCT;


/* CREDITTHRESHOLDS_M2MEM_MAIN_REG supported on:                              */
/*       SKX_A0 (0x402400A4)                                                  */
/*       SKX (0x402400A4)                                                     */
/* Register default value:              0x44880C49                            */
#define CREDITTHRESHOLDS_M2MEM_MAIN_REG 0x060040A4
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register contains the thresholds for commands sent by Mesh2Mem to a 
 * memory channel. 
 *       It contains both thresholds for isochronous transactions, as well as BGF 
 * thresholds. 
 *       The isochronous thresholds are required to provide isochronous latency 
 * guarantees.  
 *       The BGF thresholds are required to prevent BGF overflows and might need to 
 * be set slightly  
 *       different for certain clock ratios.
 * 
 *       In case the system needs to handle critical priority isochronous 
 *       transactions, following programming restrictions hold (per channel):
 *         initial RPQ credits > CritRdThresh
 *         initial WPQ credits > CritWrThresh
 *       If these restrictions are not respected then deadlock will result.
 *       In case the system doesn't need to handle isochronous transactions, those 
 * credits which  
 *       would have been reserved for isoch transactions can be given to non-isoch 
 * transactions. 
 *       This can be accomplished by programming the isoch thresholds to 0 (for 
 * performance). 
 * 
 *       The critical priority threshold is the threshold of the number of credits 
 * reserved (per channel)  
 *       For example, when there are multiple outstanding memory read requests at 
 * the home agent,  
 *       the critical priority read transaction will have the highest priority to 
 * use the memory credits.  
 *       The m2m will issue the critical memory read transactions first. If the 
 * number of memory read  
 *       (RPQ) credits is larger than the critical read threshold, then normal, 
 * non-priority, read  
 *       transaction can be issued by the m2m to the memory channel.
 * 
 *       It is required the isoch requester implements guarantees so there is no 
 * sustained long period of  
 *       isoc transactions to cause other transactions to be starved at the m2m. 
 *       The same threshold is applied to all memory channels attached to the m2m.
 *       
 *       Isoch is not supported if DDRT is present on a channel. DDRT scheduling 
 * and Isoch enabling are  
 *       mutually exclusive.
 *       
 */
typedef union {
  struct {
    UINT32 critrdthresh : 3;
    /* critrdthresh - Bits[2:0], RW, default = 3'h1 
       
               Critical Priority Reads Threshold (CritRdThresh):
               This field defines the critical priority read transactions credit 
       threshold. 
               If the number of RPQ credits for a channel is lower than or equal to the 
       threshold then  
               the credits are reserved for critical priority reads and cannot be used 
       by any other  
               reads.
               
     */
    UINT32 critwrthresh : 3;
    /* critwrthresh - Bits[5:3], RW, default = 3'h1 
       
               Critical Priority Writes Threshold (CritWrThresh):
               This field defines the critical priority write transactions credit 
       threshold. 
               If the number of WPQ credits for a channel is lower than or equal to the 
       threshold then  
               the credits are reserved for critical priority writes and cannot be used 
       by any other  
               writes.
               
     */
    UINT32 critd2kthresh : 2;
    /* critd2kthresh - Bits[7:6], RW, default = 2'h1 
       
               Critical Priority D2K Threshold (CritD2KThresh):
               This field defines the critical priority D2K credit threshold.
               If the number of Intel UPI credits for a channel is lower than or equal 
       to the threshold those   
               credits (and buffer entries) are reserved for critical priority D2K 
       messages and cannot be  
               used by regular D2K messages.
               
     */
    UINT32 critpartialwdsbthresh : 2;
    /* critpartialwdsbthresh - Bits[9:8], RW, default = 2'h0 
       
               Critical Priority Partial Writes Data Staging Buffer Threshold 
       (CritPartialWDSBThresh): 
               This field defines the critical priority partial writes data staging 
       buffer (WDSB) credit threshold. 
               If the number of partial WDSB credits for a channel is lower than or 
       equal to the threshold those   
               credits (and buffer entries) are reserved for critical priority partial 
       writes and cannot be  
               used by any other writes.
               Not supported in SKX.
               
     */
    UINT32 sbcrdthresh : 4;
    /* sbcrdthresh - Bits[13:10], RW, default = 4'h3 
       
               Scoreboard Credit Threshold (SbCrdThresh):
               This field defines the scoreboard credit threshold.
               If the number of scoreboard credits for a channel is lower than the 
       threshold (where SbCrdThresh >= 3), 
               non-trangress queues are not allowed to bid.  Remaining credits are 
       reserved for the NM and 
               FM trangress queues to prevent deadlocks.
               
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_16 : 4;
    UINT32 rsvd_20 : 1;
    /* rsvd_20 - Bits[20:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_21 : 7;
    UINT32 ingbypbgfthreshold : 4;
    /* ingbypbgfthreshold - Bits[31:28], RW, default = 4'h4 
       
               This threshold controls when the bypass FSM is eligible to enable 
       ingress bypass from a disable state based on 
               the number of read commands that are already pending in the MC command 
       bgf.   
               If the number of commands in the command BGF is less-than-or-equal to 
       this threshold and the bypass FSM state is in disable  
               then the FSM state can move to bypass enabled state. Setting this 
       threshold to a higher value will yield more opportunities  
               to turn on bypass mode. However, too high a value can result in BGF 
       overflow and hence corruption. 
               
     */
  } Bits;
  UINT32 Data;
} CREDITTHRESHOLDS_M2MEM_MAIN_STRUCT;


/* SAD2TAD_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x402400A8)                                                  */
/*       SKX (0x402400A8)                                                     */
/* Register default value:              0x00000000                            */
#define SAD2TAD_M2MEM_MAIN_REG 0x060040A8
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       SAD-to-TAD mapping:
 *       This register allows indirect access to write or read each of the 24 
 * internal SAD2TAD table entries. 
 *       The reset value for all the fields in the internal SAD2TAD table is 0. 
 * This internal SAD2TAD table gets  
 *       reset on a cold reset (pwr pulse) only.
 * 
 *       To write a SAD2TAD entry: 
 *         1. Write 0 to the write-enable and read-enable bit.
 *         2. Configure all fields and at the same time set the write-enable bit.
 *         3. When done with writing the SAD2TAD entries, write 0 to the 
 * write-enable field. 
 * 
 *       To read a SAD2TAD entry: 
 *         0. Write-enable field should already be 0.
 *         1. Write 0 to the read-enable bit.
 *         2. Configure the SAD entry/index field and at the same time set the 
 * read-enable bit. 
 *         3. Now read this register which will have its fields populated with the 
 * information  
 *            of the particular SAD entry.
 * 
 *       Some fields are for EX only. In that case it is called out that the value 
 * should be kept 0  
 *       in an EP system.
 *       All the fields that are EX specific are DAD related, i.e. to support 
 * non-equal total capacities on the  
 *       two DDR channels behind a particular SMI link.
 *       
 */
typedef union {
  struct {
    UINT32 sad2tadwren : 1;
    /* sad2tadwren - Bits[0:0], RW_LB, default = 1'd0 
       
               SAD2TAD Write (Sad2TadWrEn):
               Write the SAD2TAD table entry with the information contained in this 
       register. 
               Hardware will write the SAD2TAD table entry when seeing a zero-to-one 
       transition on this bitfield. 
               
     */
    UINT32 sad2tadrden : 1;
    /* sad2tadrden - Bits[1:1], RWS, default = 1'd0 
       
               SAD2TAD Read (Sad2TadRdEn):
               Read the SAD2TAD table entry. The read information will get stored in 
       this register. 
               Hardware will read the SAD2TAD table entry when seeing a zero-to-one 
       transition on this bitfield. 
               
     */
    UINT32 sadid : 5;
    /* sadid - Bits[6:2], RWS, default = 5'd0 
       
               SAD2TAD Table SAD Entry/Index (SadId):
               Specifies what SAD entry in the SAD2TAD table to read or write 
       indirectly through this register.  
               There are 24 SAD2TAD entries so the legal range is 0-23.
               
     */
    UINT32 sadvld : 1;
    /* sadvld - Bits[7:7], RWS_LBV, default = 1'd0 
       
               SAD2TAD Table Entry Valid (SadVld):
               Specifies whether this SAD entry in the SAD2TAD table is valid.
               
     */
    UINT32 ddr4tadid : 4;
    /* ddr4tadid - Bits[11:8], RWS_LBV, default = 4'd0 
       
               DDR4 TAD entry (DDR4TadId):
               DDR4 TAD entry associated with the SAD2TAD table SAD entry. There are 8 
       DDR4 TAD entries,  
               hence the legal range is 0-7.
               The DDR4 TAD entry field is only relevant for regions allowing DDR4 
       access.  
               A block region will not use this field, nor will a PMem-non-cached 
       region. 
       
               Note, the SAD entry associated with a SAD2TAD entry is not 
               programmable; The SAD entry is simply used as an index 
               to directly access the SAD2TAD table.
               
     */
    UINT32 pmemvld : 1;
    /* pmemvld - Bits[12:12], RWS_LBV, default = 1'd0 
       
               PMem Region Valid (PmemVld):
               This SAD entry represents a persistent memory region.
               
     */
    UINT32 blkvld : 1;
    /* blkvld - Bits[13:13], RWS_LBV, default = 1'd0 
       
               Block Region Valid (BlkVld):
               This SAD entry represents a block region.
               
     */
    UINT32 ddrttadid : 5;
    /* ddrttadid - Bits[18:14], RWS_LBV, default = 4'd0 
       
               DDRT TAD entry (DDRTtadId):
               DDRT TAD entry associated with the SAD2TAD entry. There are 12 DDRT TAD 
       entries,  
               hence the legal range is 0-11.
               
     */
    UINT32 mirrorddr4 : 1;
    /* mirrorddr4 - Bits[19:19], RWS_LBV, default = 1'd0 
       
               Mirror region:
               This SAD entry represents a mirroring region.
               
     */
    UINT32 rsvd_20 : 1;
    /* rsvd_20 - Bits[20:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 secondary1st : 1;
    /* secondary1st - Bits[21:21], RWS_LBV, default = 1'd0 
       
               Read secondary mirror channel first (Secondary1st):
               For this memory region, read secondary mirror channel first.
               Used in mirroring to prevent (non error flow) accesses to a primary 
       mirror  
               channel for a bad primary region/address. Note, if an error flow does 
       get invoked  
               because of an error on secondary for an access to this region then the 
       primary storage  
               will still be accessed (when channel not failed over).
               
     */
    UINT32 rsvd_22 : 5;
    /* rsvd_22 - Bits[26:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wrapsddr4dad : 1;
    /* wrapsddr4dad - Bits[27:27], RWS_LBV, default = 1'd0 
       
               This SAD entry wraps the DDR4 2-way-to-1-way DAD boundary 
       (WrapsDDR4DAD=UseDynamicMAD): 
               Set this bit when a SAD entry contains/straddles the DDR4 2-way-to-1-way 
       DAD boundary and that DAD  
               boundary does not coincide with the SAD boundary. In this case two 
       TADIDs will have to be mapped to this single  
               SAD entry. The 2-way TADID is provided by the SAD2TAD table while the 
       1-way TADID is provided by the  
               S2M DDR4 DAD CSR.
                 1: Take DDR4 1-way/2-way DAD boundary into account to split up this 
       SAD region into two TAD regions. 
                 0: SAD region maps to a single TAD entry (default).
               Should always be 0 in EP systems. Only to be used on EX systems whenever 
       the total DDR4 memory population capacities 
               are different on the two channels connected to this SMI channel.
               ASSERTION: This bit should be set in max one SAD entry across the 
       SAD2TAD table. 
               
     */
    UINT32 wrapsddrtdad : 1;
    /* wrapsddrtdad - Bits[28:28], RWS_LBV, default = 1'd0 
       
               This SAD entry wraps the DDRT 2-way-to-1-way DAD boundary 
       (WrapsDDRTDAD=StaticMADrule): 
               Set this bit when a SAD entry contains/straddles the DDR4 2-way-to-1-way 
       DAD boundary and that DAD  
               boundary does not coincide with the SAD boundary. In this case two 
       TADIDs will have to be mapped to this single  
               SAD entry. The 2-way TADID is provided by the SAD2TAD table while the 
       1-way TADID is provided by the  
               S2M DDRT DAD CSR.
                 1: Take DDRT 1-way/2-way DAD boundary into account to split up this 
       SAD region into two TAD regions. 
                 0: SAD region maps to a single TAD entry (default).
               Should always be 0 in EP systems. Only to be used on EX systems whenever 
       the total DDRT memory population capacities 
               are different on the two channels connected to this SMI channel.
               ASSERTION: This bit should be set in max one SAD entry across the 
       SAD2TAD table. 
               
     */
    UINT32 ddrintlvbit : 3;
    /* ddrintlvbit - Bits[31:29], RWS_LBV, default = 3'd0 
       
               System address bit for DDR 2-way interleave 
       (DDRintlvBit=DynamicMADrule):  
               Selects which bit from the system address to pick to determine the DDR 
       channel  
               in case of two-way DDR channel interleave (behind this SMI link).
               System address[6+DDRintlvBit] will be the bit specifying the physical 
       DDR channel 0/1. 
               Legal range: 0...5. Should always be kept at 0 on EP systems. 
               
     */
  } Bits;
  UINT32 Data;
} SAD2TAD_M2MEM_MAIN_STRUCT;


/* DAD_M2MEM_MAIN_REG supported on:                                           */
/*       SKX_A0 (0x402400AC)                                                  */
/*       SKX (0x402400AC)                                                     */
/* Register default value:              0x00000000                            */
#define DAD_M2MEM_MAIN_REG 0x060040AC
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       S2M DAD (Destination/DDR-channel Address Decoder):
 *       This register allows indirect access to write or read each of the 2 
 * internal S2M DAD table entries. 
 *       Entry 0 is for DDR4, while entry 1 is associated with DDRT .
 *       The reset value for all the fields in the internal DAD table is 0. This 
 * internal DAD table gets  
 *       reset on a cold reset (pwr pulse) only.
 * 
 *       To write an S2M DAD entry: 
 *         1. Write 0 to the write-enable and read-enable bit.
 *         2. Configure all fields and at the same time set the write-enable bit.
 * 
 *       To read an S2M DAD entry: 
 *         1. Write 0 to the write-enable and read-enable bit.
 *         2. Configure the DAD entry (=index) field and at the same time set the 
 * read-enable bit. 
 *         3. Now read this register which will have its fields populated with the 
 * information  
 *            of the particular DAD entry.
 * 
 *       This S2M DAD programming is relevant for EX systems only. 
 * 
 *       This S2M DAD is necessary to support non-equal total memory capacities 
 * (DDR4 as well as DDRT) on the two DDR  
 *       channels behind this particular SMI link (including the limit case of for 
 * example no memory on DDR channel 0 and  
 *       all memory on DDR channel 1). In case only one channel is populated, 
 * DAD.OneWayDDR4chnl and DAD.OneWayDDRTchnl  
 *       specifies the populated channel. In case both DDR channels are populated, 
 * two-way DDR channel interleave takes  
 *       place up to the address right below DAD.DDR4OneWayStartAddr and 
 * DAD.DDRTOneWayStartAddr. The remaining 1-way (=no-interleave)  
 *       channel is again specified by DAD.OneWayDDR4chnl and DAD.OneWayDDRTchnl.
 * 
 *       Normally the SAD2TAD table associates a single TAD entry with a SAD entry. 
 * However, the 1-way/2-way system address  
 *       boundaries (DAD.DDR4OneWayStartAddr, DAD.DDRTOneWayStartAddr) might split 
 * a single SAD address space into two TAD ranges.  
 *       In that case, for that split SAD entry, the SAD2TAD provides the TADID for 
 * the 2-way interleave region and the  
 *       DAD.DDR4secondTADID (or the DAD.DDRTsecondTADID) provides the TADID for 
 * the 1-way interleave region. If a SAD entry is  
 *       indeed split in two then SAD2TAD.WrapsDDR4DAD (or SAD2TAD.WrapsDDRTDAD) 
 * needs to get set in the SAD2TAD table for that  
 *       SAD entry.
 * 
 *       In case only one channel is populated with DDR4 (or DDRT), 
 * DAD.DDR4OneWayStartAddr (or DAD.DDRTOneWayStartAddr) should be  
 *       set to 0. 
 *       In case both channels are populated with same total memory capacity, 
 * DAD.DDR4OneWayStartAddr (or DAD.DDRTOneWayStartAddr) should be  
 *       set above Top-Of-Memory (e.g. Top-Of-Memory+64MB).
 * 
 *       When DDR4 is used as memory cache and both DDR channels are populated with 
 * DDR4-as-cache memory, then, independent  
 *       of whether the DDR4 capacities on both channels are different or not, one 
 * could always set DAD.DDR4OneWayStartAddr above  
 *       Top-Of-Memory (e.g. Top-Of-Memory+64MB) and hence not use an extra DDR4 
 * TAD entry (to split a SAD entry in two). 
 *       
 */
typedef union {
  struct {
    UINT32 dadwren : 1;
    /* dadwren - Bits[0:0], RWS_LB, default = 1'd0 
       
               DAD Write (DadWrEn):
               Write the DAD table entry with the information contained in this 
       register. 
               Hardware will write the DAD table entry when seeing a zero-to-one 
       transition on this bitfield. 
               
     */
    UINT32 dadrden : 1;
    /* dadrden - Bits[1:1], RWS_LB, default = 1'd0 
       
               DAD Read (DadRdEn):
               Read the DAD table entry. The read information will get stored in this 
       register. 
               Hardware will read the DAD table entry when seeing a zero-to-one 
       transition on this bitfield. 
               
     */
    UINT32 dadid : 1;
    /* dadid - Bits[2:2], RWS_LB, default = 1'd0 
       
               DAD Table Entry/Index (DadId):
               Specifies what entry in the DAD table to read or write indirectly 
       through this register.  
                 DadID=0 is for DDR4.
                 DadID=1 is for DDRT .
               
     */
    UINT32 onewayphyschnl : 1;
    /* onewayphyschnl - Bits[3:3], RWS_LBV, default = 1'd0 
       
               Physical channel of the 1-way interleave region (OneWayPhysChnl):
               Physical DDR channel 0/1 associated with the DDR 1-way channel 
       interleave (i.e. no interleave) region.  
               Needs to be 0 in EP. In RG, in case only one channel is populated, this 
       bit needs to be the channel  
               number of the populated channel. In RG, in case of unequal memory 
       capacities, this bit needs to be  
               the channel number of the channel with the largest total capacity 
       (within that RG). 
               
     */
    UINT32 secondtadid : 4;
    /* secondtadid - Bits[7:4], RWS_LBV, default = 1'd0 
       
               Second TADID associated with the split SAD entry (SecondTadId):
               Specifies the 2nd TADID for the 1-way region associated with the split 
       SAD entry. 
               For the SAD entry which has two TAD entries mapped to it, the SAD2TAD 
       table specifies  
               the first (i.e. 2-way region) TADID and this field contains the second 
       TADID for the  
               non-interleaved region. For this field to be used, SAD2TAD.WrapsDDRTDAD 
       or WrapsDDR4DAD  
               needs to be set in one SAD2TAD entry.
               Keep as 0 in EP.
               
     */
    UINT32 onewaystartaddr : 22;
    /* onewaystartaddr - Bits[29:8], RWS_LBV, default = 1'd0 
       
               Starting address of the 1-way interleave region (OneWayStartAddr):
               Start system address[47:26] of the DDR 1-way channel interleave (i.e. no 
       interleave) region.  
               This is a 64MB aligned address. All system addresses less than this 
       start address belong to  
               the 2-way channel interleave region.
               Needs to be 0 in EP. Set to 0 in RG if only one channel is populated.
               
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DAD_M2MEM_MAIN_STRUCT;


/* PAD0_N0_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x402400B0)                                                  */
/*       SKX (0x402400B0)                                                     */
/* Register default value:              0x00000000                            */
#define PAD0_N0_M2MEM_MAIN_REG 0x060040B0
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       PAD (Pcommit Address Decoder) region 0:
 *       This register specifies the system address range for which mesh2mem should 
 * interpret incoming 
 *       BL writes as directed pcommits. Reads to this address range will just be 
 * propagated to memory. 
 * 
 *       When the PAD is enabled, any incoming write matching the PAD address 
 * region according to expression: 
 *         PAD region hit = (wr_sys_addr[45:6] and 
 * ~CONCAT(0,PAD.Mask[11:6]))==(PAD.Address[45:6] and ~CONCAT(0,PAD.Mask[11:6]) 
 *       will not write but instead get interpreted as a directed pcommit with the 
 * targeted channel provided by the CHA SAD. 
 * 
 *       The envisioned usage model is that an address range within the block 
 * region would be used as the PAD region. 
 *       
 */
typedef union {
  struct {
    UINT32 enable : 1;
    /* enable - Bits[0:0], RWS_LB, default = 1'd0 
       
               Enable the PAD, i.e. directed pcommits (PADenable):
               Use PAD[Address] and PAD[Mask] to determine that a Mesh2mem incoming 
       write needs  
               to be interpreted as a directed pcommit. If the PAD is disabled then 
       writes will  
               never get converted to directed pcommits and will just behave like 
       writes to memory. 
               Whenever a write gets converted to a directed pcommit, that write will 
       no longer take  
               place to memory.
               
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[5:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 address : 26;
    /* address - Bits[31:6], RWS_LB, default = 1'd0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Address specifying the directed pcommit region (PADaddress):
               PAD region system address [45:6] to compare against the incoming 
               writes' system addresses (masked comparison).
               
     */
  } Bits;
  UINT32 Data;
} PAD0_N0_M2MEM_MAIN_STRUCT;


/* PAD0_N1_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x402400B4)                                                  */
/*       SKX (0x402400B4)                                                     */
/* Register default value:              0x00000000                            */
#define PAD0_N1_M2MEM_MAIN_REG 0x060040B4
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       PAD (Pcommit Address Decoder) region 0:
 *       This register specifies the system address range for which mesh2mem should 
 * interpret incoming 
 *       BL writes as directed pcommits. Reads to this address range will just be 
 * propagated to memory. 
 * 
 *       When the PAD is enabled, any incoming write matching the PAD address 
 * region according to expression: 
 *         PAD region hit = (wr_sys_addr[45:6] and 
 * ~CONCAT(0,PAD.Mask[11:6]))==(PAD.Address[45:6] and ~CONCAT(0,PAD.Mask[11:6]) 
 *       will not write but instead get interpreted as a directed pcommit with the 
 * targeted channel provided by the CHA SAD. 
 * 
 *       The envisioned usage model is that an address range within the block 
 * region would be used as the PAD region. 
 *       
 */
typedef union {
  struct {
    UINT32 address : 14;
    /* address - Bits[13:0], RWS_LB, default = 1'd0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Address specifying the directed pcommit region (PADaddress):
               PAD region system address [45:6] to compare against the incoming 
               writes' system addresses (masked comparison).
               
     */
    UINT32 rsvd_14 : 9;
    /* rsvd_14 - Bits[22:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mask : 6;
    /* mask - Bits[28:23], RWS_LB, default = 1'd0 
       
               Mask specifying the directed pcommit region size (PADmask):
               This is a bit mask which tells which bits of the system address [11:6] 
       to mask out  
               when performing the comparison of the system address [45:6] to the PAD[ 
       Address[45:6] ]. 
               A one in a bit position means that bit will not be included in the 
       address comparison. 
               For example, Mask=0x7 results in 8 64B memory blocks mapping to the 
       directed pcommit region,  
               hence, 8 directed pcommits can be simultaneously outstanding in the 
       system to this M2M.  
               Basically, Mask+1 is the maximum allowed number of concurrent directed 
       pcommits that are  
               allowed to be outstanding in the system to this M2M.
               
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PAD0_N1_M2MEM_MAIN_STRUCT;


/* PAD1_N0_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x402400B8)                                                  */
/*       SKX (0x402400B8)                                                     */
/* Register default value:              0x00000000                            */
#define PAD1_N0_M2MEM_MAIN_REG 0x060040B8
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       PAD (Pcommit Address Decoder) region 1:
 *       This register specifies the system address range for which mesh2mem should 
 * interpret incoming 
 *       BL writes as directed pcommits. Reads to this address range will just be 
 * propagated to memory. 
 * 
 *       When the PAD is enabled, any incoming write matching the PAD address 
 * region according to expression: 
 *         PAD region hit = (wr_sys_addr[45:6] and 
 * ~CONCAT(0,PAD.Mask[11:6]))==(PAD.Address[45:6] and ~CONCAT(0,PAD.Mask[11:6]) 
 *       will not write but instead get interpreted as a directed pcommit with the 
 * targeted channel provided by the CHA SAD. 
 * 
 *       The envisioned usage model is that an address range within the block 
 * region would be used as the PAD region. 
 *       
 */
typedef union {
  struct {
    UINT32 enable : 1;
    /* enable - Bits[0:0], RWS_LB, default = 1'd0 
       
               Enable the PAD, i.e. directed pcommits (PADenable):
               Use PAD[Address] and PAD[Mask] to determine that a Mesh2mem incoming 
       write needs  
               to be interpreted as a directed pcommit. If the PAD is disabled then 
       writes will  
               never get converted to directed pcommits and will just behave like 
       writes to memory. 
               Whenever a write gets converted to a directed pcommit, that write will 
       no longer take  
               place to memory.
               
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[5:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 address : 26;
    /* address - Bits[31:6], RWS_LB, default = 1'd0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Address specifying the directed pcommit region (PADaddress):
               PAD region system address [45:6] to compare against the incoming 
               writes' system addresses (masked comparison).
               
     */
  } Bits;
  UINT32 Data;
} PAD1_N0_M2MEM_MAIN_STRUCT;


/* PAD1_N1_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x402400BC)                                                  */
/*       SKX (0x402400BC)                                                     */
/* Register default value:              0x00000000                            */
#define PAD1_N1_M2MEM_MAIN_REG 0x060040BC
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       PAD (Pcommit Address Decoder) region 1:
 *       This register specifies the system address range for which mesh2mem should 
 * interpret incoming 
 *       BL writes as directed pcommits. Reads to this address range will just be 
 * propagated to memory. 
 * 
 *       When the PAD is enabled, any incoming write matching the PAD address 
 * region according to expression: 
 *         PAD region hit = (wr_sys_addr[45:6] and 
 * ~CONCAT(0,PAD.Mask[11:6]))==(PAD.Address[45:6] and ~CONCAT(0,PAD.Mask[11:6]) 
 *       will not write but instead get interpreted as a directed pcommit with the 
 * targeted channel provided by the CHA SAD. 
 * 
 *       The envisioned usage model is that an address range within the block 
 * region would be used as the PAD region. 
 *       
 */
typedef union {
  struct {
    UINT32 address : 14;
    /* address - Bits[13:0], RWS_LB, default = 1'd0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Address specifying the directed pcommit region (PADaddress):
               PAD region system address [45:6] to compare against the incoming 
               writes' system addresses (masked comparison).
               
     */
    UINT32 rsvd_14 : 9;
    /* rsvd_14 - Bits[22:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mask : 6;
    /* mask - Bits[28:23], RWS_LB, default = 1'd0 
       
               Mask specifying the directed pcommit region size (PADmask):
               This is a bit mask which tells which bits of the system address [11:6] 
       to mask out  
               when performing the comparison of the system address [45:6] to the PAD[ 
       Address[45:6] ]. 
               A one in a bit position means that bit will not be included in the 
       address comparison. 
               For example, Mask=0x7 results in 8 64B memory blocks mapping to the 
       directed pcommit region,  
               hence, 8 directed pcommits can be simultaneously outstanding in the 
       system to this M2M.  
               Basically, Mask+1 is the maximum allowed number of concurrent directed 
       pcommits that are  
               allowed to be outstanding in the system to this M2M.
               
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PAD1_N1_M2MEM_MAIN_STRUCT;


/* PAD2_N0_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x402400C0)                                                  */
/*       SKX (0x402400C0)                                                     */
/* Register default value:              0x00000000                            */
#define PAD2_N0_M2MEM_MAIN_REG 0x060040C0
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       PAD (Pcommit Address Decoder) region 2:
 *       This register specifies the system address range for which mesh2mem should 
 * interpret incoming 
 *       BL writes as directed pcommits. Reads to this address range will just be 
 * propagated to memory. 
 * 
 *       When the PAD is enabled, any incoming write matching the PAD address 
 * region according to expression: 
 *         PAD region hit = (wr_sys_addr[45:6] and 
 * ~CONCAT(0,PAD.Mask[11:6]))==(PAD.Address[45:6] and ~CONCAT(0,PAD.Mask[11:6]) 
 *       will not write but instead get interpreted as a directed pcommit with the 
 * targeted channel provided by the CHA SAD. 
 * 
 *       The envisioned usage model is that an address range within the block 
 * region would be used as the PAD region. 
 *       
 */
typedef union {
  struct {
    UINT32 enable : 1;
    /* enable - Bits[0:0], RWS_LB, default = 1'd0 
       
               Enable the PAD, i.e. directed pcommits (PADenable):
               Use PAD[Address] and PAD[Mask] to determine that a Mesh2mem incoming 
       write needs  
               to be interpreted as a directed pcommit. If the PAD is disabled then 
       writes will  
               never get converted to directed pcommits and will just behave like 
       writes to memory. 
               Whenever a write gets converted to a directed pcommit, that write will 
       no longer take  
               place to memory.
               
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[5:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 address : 26;
    /* address - Bits[31:6], RWS_LB, default = 1'd0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Address specifying the directed pcommit region (PADaddress):
               PAD region system address [45:6] to compare against the incoming 
               writes' system addresses (masked comparison).
               
     */
  } Bits;
  UINT32 Data;
} PAD2_N0_M2MEM_MAIN_STRUCT;


/* PAD2_N1_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x402400C4)                                                  */
/*       SKX (0x402400C4)                                                     */
/* Register default value:              0x00000000                            */
#define PAD2_N1_M2MEM_MAIN_REG 0x060040C4
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       PAD (Pcommit Address Decoder) region 2:
 *       This register specifies the system address range for which mesh2mem should 
 * interpret incoming 
 *       BL writes as directed pcommits. Reads to this address range will just be 
 * propagated to memory. 
 * 
 *       When the PAD is enabled, any incoming write matching the PAD address 
 * region according to expression: 
 *         PAD region hit = (wr_sys_addr[45:6] and 
 * ~CONCAT(0,PAD.Mask[11:6]))==(PAD.Address[45:6] and ~CONCAT(0,PAD.Mask[11:6]) 
 *       will not write but instead get interpreted as a directed pcommit with the 
 * targeted channel provided by the CHA SAD. 
 * 
 *       The envisioned usage model is that an address range within the block 
 * region would be used as the PAD region. 
 *       
 */
typedef union {
  struct {
    UINT32 address : 14;
    /* address - Bits[13:0], RWS_LB, default = 1'd0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Address specifying the directed pcommit region (PADaddress):
               PAD region system address [45:6] to compare against the incoming 
               writes' system addresses (masked comparison).
               
     */
    UINT32 rsvd_14 : 9;
    /* rsvd_14 - Bits[22:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mask : 6;
    /* mask - Bits[28:23], RWS_LB, default = 1'd0 
       
               Mask specifying the directed pcommit region size (PADmask):
               This is a bit mask which tells which bits of the system address [11:6] 
       to mask out  
               when performing the comparison of the system address [45:6] to the PAD[ 
       Address[45:6] ]. 
               A one in a bit position means that bit will not be included in the 
       address comparison. 
               For example, Mask=0x7 results in 8 64B memory blocks mapping to the 
       directed pcommit region,  
               hence, 8 directed pcommits can be simultaneously outstanding in the 
       system to this M2M.  
               Basically, Mask+1 is the maximum allowed number of concurrent directed 
       pcommits that are  
               allowed to be outstanding in the system to this M2M.
               
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PAD2_N1_M2MEM_MAIN_STRUCT;


/* PREFETCHSAD_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402400D0)                                                  */
/*       SKX (0x402400D0)                                                     */
/* Register default value:              0x00000000                            */
#define PREFETCHSAD_M2MEM_MAIN_REG 0x060040D0
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This prefetch SAD (Source Address Decoder) register allows indirect access 
 * to write or read each of  
 *       the 4 internal PrefetchSAD table entries. 
 *       The PrefetchSAD table is a mini-SAD used to determine the physical memory 
 * channel for a Intel UPI/XPT prefetch. 
 *       The reset value for all the fields in the internal PrefetchSAD table is 0. 
 * This internal PrefetchSAD table gets  
 *       reset on a cold reset (pwr pulse) only.
 * 
 *       To write a PrefetchSAD entry: 
 *         1. Write 0 to the write-enable and read-enable bit.
 *         2. Configure all fields and at the same time set the write-enable bit.
 * 
 *       To read a PrefetchSAD entry: 
 *         1. Write 0 to the write-enable and read-enable bit.
 *         2. Configure the mini-SAD entry field and at the same time set the 
 * read-enable bit. 
 *         3. Now read this register which will have its fields populated with the 
 * information  
 *            of the particular mini-SAD entry.
 * 
 *       Along with this M2M PrefetchSAD, the mini-SADs in the CoreBo and M3UPI 
 * need to get programmed for  
 *       prefetches. In SKX, XPT prefetching is always to 1LM DDR4 and can only be 
 * enabled for 1LM DDR4 and 1LM DDRT configs.  
 *       It is illegal to enable XPT prefetching when Mode[NMcaching] is set.
 *       
 */
typedef union {
  struct {
    UINT32 prefsadwren : 1;
    /* prefsadwren - Bits[0:0], RWS_LB, default = 1'd0 
       
               PrefetchSAD Write (PrefSadWrEn):
               Write the PrefetchSAD table entry with the information contained in this 
       register. 
               Hardware will write the PrefetchSAD table entry when seeing a 
       zero-to-one transition on this bitfield. 
               
     */
    UINT32 prefsadrden : 1;
    /* prefsadrden - Bits[1:1], RWS_LB, default = 1'd0 
       
               PrefetchSAD Read (PrefSadRdEn):
               Read the PrefetchSAD table entry. The read information will get stored 
       in this register. 
               Hardware will read the PrefetchSAD table entry when seeing a zero-to-one 
       transition on this bitfield. 
               
     */
    UINT32 prefsadentry : 4;
    /* prefsadentry - Bits[5:2], RWS_LBV, default = 3'd0 
       
               PrefetchSAD Table Entry (PrefSadEntry):
               Index of the PrefetchSAD table entry.
               It specifies the entry in the PrefetchSAD table to read or write 
       indirectly through this register.  
               There are 4 PrefetchSAD entries (i.e. max 4 memory ranges) so the legal 
       range is 0-3. 
               
     */
    UINT32 prefsadvld : 1;
    /* prefsadvld - Bits[6:6], RWS_LBV, default = 1'd0 
       
               Prefetch-SAD Entry Valid (PrefSadVld)):
               The entry in the PrefetchSAD table is a valid entry and can be used
               by XPT/Intel UPI prefetches.
               
     */
    UINT32 prefsadinterlv : 3;
    /* prefsadinterlv - Bits[9:7], RWS_LBV, default = 2'd0 
       
               Interleave Grain (PrefSadInterlv):
               iMC target granularity for this PrefetchSAD entry:
                 0: 64B (with 64B channel granularity)
                 2: 256B (with 256B channel granularity)
                 1: 4KB (with 256B channel granularity)
               Specifies what the contiguous address granularity (interleave) is, 
               i.e. which contiguous address space maps to the same M2M (resp. to the 
       same channel). 
               
     */
    UINT32 prefsadchnls : 3;
    /* prefsadchnls - Bits[12:10], RWS_LBV, default = 3'd0 
       
               Interleave Channels (PrefSadChnls):
               Physical memory channels interleave for this PrefetchSAD entry.
               Bits [2:0] is a mask mapping to physical channels 2-0.
                 3'b111: Mod3 interleave among the 3 physical channels.
                 3'b101: 2-way interleave between physical channels 2 and 0.
                 3'b011: 2-way interleave between physical channels 1 and 0.
                 3'b101: 2-way interleave between physical channels 2 and 1.
                 3'b001: no interleave, only use physical channel 0.
                 3'b010: no interleave, only use physical channel 1.
                 3'b100: no interleave, only use physical channel 2.
               
     */
    UINT32 ddr4tadid : 4;
    /* ddr4tadid - Bits[16:13], RWS_LBV, default = 4'd0 
       
               DDR4 TAD Entry (Ddr4TadId):
               Specifies what TAD ID/entry the PrefetchSAD table entry maps to.
               There are 8 DDR4 TAD entries so legal range is 0-7.
               
     */
    UINT32 rsvd : 15;
    /* rsvd - Bits[31:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PREFETCHSAD_M2MEM_MAIN_STRUCT;


/* PREFSADCONFIG_M2MEM_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x402400D4)                                                  */
/*       SKX (0x402400D4)                                                     */
/* Register default value:              0x00000000                            */
#define PREFSADCONFIG_M2MEM_MAIN_REG 0x060040D4
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register contains two PREFETCH_TADID fields, and a single 256M 
 * granular range register field.  For any incoming prefetch, if the address is 
 * less-than the value in  
 *       the range register, use the first prefetch TADID, if the address is 
 * greater-than-or-eq the value in the range register, use the second prefetch 
 * TADID. 
 *       This is a limitation we will have to accept by requiring that memory be 
 * uniformly populated across the channels to get the full benefit of XPT/Intel UPI 
 *  
 *       prefetch, but one feature of this limitation that is problematic is the 
 * MMIO hole.  Even when memory is uniformly populated, the socket whose local  
 *       memory straddles the MMIO hole, requires a second TAD be used to recover 
 * the MMIO hole. 
 *       
 */
typedef union {
  struct {
    UINT32 firstpftadid : 3;
    /* firstpftadid - Bits[2:0], RWS_LBV, default = 1'd0 
       
               First TadID for when a prefetch read address is less-than 
       PrefMmioBaseAddr. 
               
     */
    UINT32 secondpftadid : 3;
    /* secondpftadid - Bits[5:3], RWS_LBV, default = 1'd0 
       
               Second TadID for when a prefetch read address is greater-than-or-eq 
       PrefMmioBaseAddr. 
               
     */
    UINT32 prefmmiobaseaddr : 18;
    /* prefmmiobaseaddr - Bits[23:6], RWS_LBV, default = 1'd0 
       
               Base address of prefetch MMIO range.  18 bits on 256MB boundary.
               
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PREFSADCONFIG_M2MEM_MAIN_STRUCT;


/* MEMORY_M2MEM_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x402400D8)                                                  */
/*       SKX (0x402400D8)                                                     */
/* Register default value:              0x00000000                            */
#define MEMORY_M2MEM_MAIN_REG 0x060040D8
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Information w.r.t. the memory population on the channels.
 *       
 */
typedef union {
  struct {
    UINT32 ddr4chnlpopulation : 3;
    /* ddr4chnlpopulation - Bits[2:0], RWS_LB, default = 3'h0 
       
               DDR4 population channel bitvector (DDR4ChnlPopulation):
               Specifies for each channel 2...0 whether that physical channel has DDR4 
       on it. 
               
     */
    UINT32 rsvd_3 : 1;
    /* rsvd_3 - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrtchnlpopulation : 3;
    /* ddrtchnlpopulation - Bits[6:4], RWS_LB, default = 3'h0 
       
               DDRT population channel bitvector (DDRTChnlPopulation):
               Specifies for each channel 2...0 (corresponding to bit 6...4) whether 
       that physical channel has DDRT on it. 
               
     */
    UINT32 rsvd_7 : 25;
    /* rsvd_7 - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MEMORY_M2MEM_MAIN_STRUCT;




/* SYSFEATURES0_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x402400E0)                                                  */
/*       SKX (0x402400E0)                                                     */
/* Register default value:              0x00000080                            */
#define SYSFEATURES0_M2MEM_MAIN_REG 0x060040E0
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       This register contains m2m features which BIOS might need to set 
 * differently based on  
 *       ear
 * he system topology/configuration or for performance/power reasons.
 * 
 *       A single socket system is expected to at least configure following 
 * (different from reset values): 
 *         CHA HA_COH_CFG[Dis_Directory] = 1 (functionality)
 *         M2M SysFeatures[FrcDirI]      = 1 (functionality)
 *         M2M SysFeatures[DirWrDisable] = 1 (performance)
 *         M2M SysFeatures[FrcD2c]       = 1 (performance)
 * 
 *       A multi-socket directory disabled system is expected to at least configure 
 * following (different from reset values): 
 *         CHA HA_COH_CFG[Dis_Directory] = 1 (functionality)
 *         M2M SysFeatures[D2kDisable]   = 1 (functionality)
 *         M2M SysFeatures[D2cDisable]   = 1 (functionality)
 *         M2M SysFeatures[DirWrDisable] = 1 (performance)
 *       
 */
typedef union {
  struct {
    UINT32 frcd2c : 1;
    /* frcd2c - Bits[0:0], RW, default = 1'd0 
       
               Force D2C (FrcD2c):  Do not use; FrcDirI will ultimately lead to D2C
               Force direct-to-core on every read request, even when CHA didn't set the 
       d2c bit on the request. 
                 0: Do not force d2c (default).
                 1: Force d2c on every read request from CHA, overriding the d2c 
       request bit from CHA. 
               BIOS should set this bit on a single socket system (for performance).
               It is illegal to set this bit to 1 outside of the single socket case.
               
     */
    UINT32 d2cdisable : 1;
    /* d2cdisable - Bits[1:1], RW, default = 1'd0 
       
               Direct-to-core disable (D2cDisable):
                 0: D2C enabled.
                 1: Disable direct-to-core data return. Data is sent to CHA instead, 
                    even if CHA set the d2c bit on the incoming request.
               BIOS has to disable D2C in multi-socket systems with dir-disabled (for 
       functionality). 
               It is illegal to enable D2C in multi-socket systems with dir-disabled.
               
     */
    UINT32 d2kdisable : 1;
    /* d2kdisable - Bits[2:2], RW, default = 1'd0 
       
               Direct-to-Intel UPI disable (D2kDisable):
                 0: D2K enabled.
                 1: Disable direct-to-Intel UPI data return. Data is sent to CHA 
       instead, even if CHA set the d2k bit on the incoming request. 
               BIOS has to disable D2K in multi-socket systems with dir-disabled (for 
       functionality). 
               It is illegal to enable D2K in multi-socket systems with dir-disabled.
               
     */
    UINT32 prefdisable : 1;
    /* prefdisable - Bits[3:3], RW, default = 1'd0 
       
               Prefetch disable (PrefDisable):
               Disable prefetching in M2M/MC: 
                 0: Prefetching enabled, i.e. MemSpecRd handling enabled.
                 1: An incoming MemSpecRd will be dropped.
               For performance, especially latency reduction, prefetch should be 
       enabled. 
               If power is more important than this incremental performance improvement 
        
               then disable this feature.  
     */
    UINT32 dirwrdisable : 1;
    /* dirwrdisable - Bits[4:4], RW_LB, default = 1'd0 
       
               Disable directory writes (DirWrDisable):
                 0: MC directory writes are enabled (default).
                 1: MC directory writes are disabled.
               Only legal configs where MC directory writes can be disabled are:
                 - when single socket system, or
                 - when directory disabled in the system.
               BIOS should set this bit to 1 for above configs (for performance).
               
     */
    UINT32 frcdiri : 1;
    /* frcdiri - Bits[5:5], RW_LB, default = 1'd0 
       
               Force dirI (FrcDirI):
               Force the directory state read from memory to Invalid, for 
               new dir calculation (exception: poison/uncorr stays poison) and for 
               dir delivery to CHA (here Invalid will override poison).
                 0: interpret directory from memory as-is.
                 1: override directory S/A/(P) read from memory with I.
               BIOS needs to set this bit to 1 if single socket.
               
     */
    UINT32 mapdirstodira : 1;
    /* mapdirstodira - Bits[6:6], RW_LB, default = 1'd0 
       
               Map dirS to dirA (MapDirStoDirA):
               Upgrade dirS to memory to dirA. It also prevents DataC_S_CmpO D2K (when 
       dirS no longer present in memory). 
                 0: don't override dirS with dirA (default).
                 1: dirS wr to memory gets replaced with dirA wr.
               Guideline is to keep at 0 for performance. Note that taking out 
       directory shared state  
               might result in less directory writes. Some usage models might prefer 
       less directory writes. 
               
     */
    UINT32 immediatefailoveractionena : 1;
    /* immediatefailoveractionena - Bits[7:7], RW, default = 1'd1 
       
               Immediately Failover Action Enable (ImmediateFailoverActionEna):
                 0: Do not failover a channel when mirror scrub read returns 
       uncorrectable data. 
                    Instead BIOS has the smarts and decision power to either (i) do 
       nothing, or  
                    to (ii) force accesses to primary first (SAD2TAD.Secondary1st), or 
                    to (iii) failover the complete channel through the MirrorFailover 
       register. 
                 1: Hardware will permanently failover the channel when a mirror scrub 
       read returns uncorrectable data. 
                    Hardware will set the appropriate failover bit in MirrorFailover. 
       When this bit is set,  
                    also XPT/Intel UPI prefetch needs to be disabled in the core and at 
       the Intel UPI agent respectively,  
                    and Defeatures0[IngBypDis] need to be set. In other words, in 
       mirroring hardware failover mode,  
                    both XPT prefetch and ingress bypass need to be disabled.
               
     */
    UINT32 ptlwrfailoverlogdis : 1;
    /* ptlwrfailoverlogdis - Bits[8:8], RW, default = 1'd0 
       
               Partial writes failover logging disable (PtlWrFailoverLogDis):
               Disable failover error logging and signaling in case of a partial write.
                 0: Allow m2m failover error logging and signaling for a partial write.
                 1: Disable m2m failover error logging and signaling for a partial 
       write. 
               Usage model: BIOS can choose to set this disable when 
       ImmediateFailoverActionEna==0 
               and SAD2TAD.Secondary1st==1. In this config, partial writes would still 
       go to both mirror channels  
               even though reads go to secondary channel. If BIOS determines there is a 
       frequent and undesired  
               signaling and logging storm of (known) failover events from partial 
       writes accessing the bad primary region,  
               then BIOS can set this defeature bit.
               Note, no partial write errors on ANY of the channels associated with the 
       m2m will be signaled nor  
               logged from that point onwards in m2m. 
               This defeature is failover event specific; other partial write errors 
       like uncorrectables will still get signaled. 
               
     */
    UINT32 frcwrni : 1;
    /* frcwrni - Bits[9:9], RW_V, default = 1'd0 
       
               Blindly forces inclusive writes to behave as non-inclusive writes, i.e. 
       forces a read of the NMC state to determine 
               whether NMC hit or miss before performing the write. No error is 
       generated if there is a miss for the inclusive write. 
               BIOS needs to set this bit after certain memory errors. If the patrol 
       scrubber reports an uncorrectable error in NMC 
               then this bit needs to be set.
               
     */
    UINT32 pcommitforcebroadcast : 1;
    /* pcommitforcebroadcast - Bits[10:10], RW, default = 1'd0 
       
               Force directed pcommits to broadcast to all channels connected to this 
       mesh2mem. 
               Needs to be set when mirroring is enabled on this mesh2mem.
               
     */
    UINT32 rsvd : 21;
    /* rsvd - Bits[31:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SYSFEATURES0_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register contains m2m features which BIOS might need to set 
 * differently based on  
 *       ear
 * he system topology/configuration or for performance/power reasons.
 * 
 *       A single socket system is expected to at least configure following 
 * (different from reset values): 
 *         CHA HA_COH_CFG[Dis_Directory]  = 1 (functionality)
 *         M2M SysFeatures0[FrcDirI]      = 1 (functionality)
 *         M2M SysFeatures0[DirWrDisable] = 1 (performance)
 * 
 *       A multi-socket directory-DISABLED system is expected to at least configure 
 * following (different from reset values): 
 *         CHA HA_COH_CFG[Dis_Directory]  = 1 (functionality)
 *         M2M SysFeatures0[D2kDisable]   = 1 (functionality)
 *         M2M SysFeatures0[D2cDisable]   = 1 (functionality)
 *         M2M SysFeatures0[DirWrDisable] = 1 (performance)
 *       
 */
typedef union {
  struct {
    UINT32 frcd2c : 1;
    /* frcd2c - Bits[0:0], RW, default = 1'd0 
       
               Force D2C (FrcD2c):  Do not use; FrcDirI will ultimately lead to D2C
               Force direct-to-core on every read request, even when CHA didn't set the 
       d2c bit on the request. 
                 0: Do not force d2c (default).
                 1: Force d2c on every read request from CHA, overriding the d2c 
       request bit from CHA. 
               It is illegal to set this bit to 1, as CHA cannot deal with d2c when it 
       doesn't expect it. Always keep it at 0. 
       	
     */
    UINT32 d2cdisable : 1;
    /* d2cdisable - Bits[1:1], RW, default = 1'd0 
       
               Direct-to-core disable (D2cDisable):
                 0: D2C enabled.
                 1: Disable direct-to-core data return. Data is sent to CHA instead, 
                    even if CHA set the d2c bit on the incoming request.
               BIOS has to disable D2C in multi-socket systems with dir-disabled (for 
       functionality). 
               It is illegal to enable D2C in multi-socket systems with dir-disabled.
               
     */
    UINT32 d2kdisable : 1;
    /* d2kdisable - Bits[2:2], RW, default = 1'd0 
       
               Direct-to-Intel UPI disable (D2kDisable):
                 0: D2K enabled.
                 1: Disable direct-to-Intel UPI data return. Data is sent to CHA 
       instead, even if CHA set the d2k bit on the incoming request. 
               BIOS has to disable D2K in multi-socket systems with dir-disabled (for 
       functionality). 
               It is illegal to enable D2K in multi-socket systems with dir-disabled.
               
     */
    UINT32 prefdisable : 1;
    /* prefdisable - Bits[3:3], RW, default = 1'd0 
       
               Prefetch disable (PrefDisable):
               Disable XPT prefetching in M2M/MC: 
                 0: Prefetching enabled, i.e. MemSpecRd handling enabled.
                 1: An incoming MemSpecRd will be dropped.
               For performance, especially latency reduction, XPT prefetch should be 
       enabled. 
               An XPT prefetch targets DDR4 and it is illegal to enable it in systems 
       with Near-Memory Cache (Mode[NMcaching]==1). 
               For any M2M that is in mirror mode, prefetching should be disabled by 
       setting this bit to 1 
               If power is more important than this incremental performance improvement 
        
               then disable this feature.  
     */
    UINT32 dirwrdisable : 1;
    /* dirwrdisable - Bits[4:4], RW_LB, default = 1'd0 
       
               Disable directory writes (DirWrDisable):
                 0: MC directory writes are enabled (default).
                 1: MC directory writes are disabled.
               Only legal configs where MC directory writes can be disabled are:
                 - when single socket system, or
                 - when directory disabled in the system.
               BIOS should set this bit to 1 for above configs (for performance).
               
     */
    UINT32 frcdiri : 1;
    /* frcdiri - Bits[5:5], RW_LB, default = 1'd0 
       
               Force dirI (FrcDirI):
               Force the directory state read from memory to Invalid, for 
               new dir calculation (exception: poison/uncorr stays poison) and for 
               dir delivery to CHA (here Invalid will override poison).
                 0: interpret directory from memory as-is.
                 1: override directory S/A/(P) read from memory with I.
               BIOS needs to set this bit to 1 if single socket.
               
     */
    UINT32 mapdirstodira : 1;
    /* mapdirstodira - Bits[6:6], RW_LB, default = 1'd0 
       
               Map dirS to dirA (MapDirStoDirA):
               Upgrade dirS to memory to dirA. It also prevents DataC_S_CmpO D2K (when 
       dirS no longer present in memory). 
                 0: don't override dirS with dirA (default).
                 1: dirS wr to memory gets replaced with dirA wr.
               Guideline is to keep at 0 for performance. Note that taking out 
       directory shared state  
               might result in less directory writes. Some usage models might prefer 
       less directory writes. 
               
     */
    UINT32 immediatefailoveractionena : 1;
    /* immediatefailoveractionena - Bits[7:7], RW, default = 1'd1 
       
               Immediately Failover Action Enable (ImmediateFailoverActionEna):
                 0: Do not failover a channel when mirror scrub read returns 
       uncorrectable data. 
                    Instead BIOS has the smarts and decision power to either (i) do 
       nothing, or  
                    to (ii) failover the complete channel through the MirrorFailover 
       register. 
                 1: Hardware will permanently failover the channel when a mirror scrub 
       read returns uncorrectable data. 
                    Hardware will set the appropriate failover bit in MirrorFailover. 
       When this bit is set,  
                    also XPT/Intel UPI prefetch needs to be disabled in the core and at 
       the Intel UPI agent respectively,  
                    and Defeatures0[IngBypDis] need to be set. In other words, in 
       mirroring hardware failover mode,  
                    both XPT prefetch and ingress bypass need to be disabled.
               
     */
    UINT32 ptlwrfailoverlogdis : 1;
    /* ptlwrfailoverlogdis - Bits[8:8], RW, default = 1'd0 
       
               Partial writes failover logging disable (PtlWrFailoverLogDis):
               Disable failover error logging and signaling in case of a partial write.
                 0: Allow m2m failover error logging and signaling for a partial write.
                 1: Disable m2m failover error logging and signaling for a partial 
       write. 
               Usage model: BIOS can choose to set this disable when 
       ImmediateFailoverActionEna==0 
               and SAD2TAD.Secondary1st==1. In this config, partial writes would still 
       go to both mirror channels  
               even though reads go to secondary channel. If BIOS determines there is a 
       frequent and undesired  
               signaling and logging storm of (known) failover events from partial 
       writes accessing the bad primary region,  
               then BIOS can set this defeature bit.
               Note, no partial write errors on ANY of the channels associated with the 
       m2m will be signaled nor  
               logged from that point onwards in m2m. 
               This defeature is failover event specific; other partial write errors 
       like uncorrectables will still get signaled. 
               
     */
    UINT32 frcwrni : 1;
    /* frcwrni - Bits[9:9], RW_V, default = 1'd0 
       
               Blindly forces inclusive writes to behave as non-inclusive writes, i.e. 
       forces a read of the NMC state to determine 
               whether NMC hit or miss before performing the write. No error is 
       generated if there is a miss for the inclusive write. 
               BIOS needs to set this bit after certain memory errors. If the patrol 
       scrubber reports an uncorrectable error in NMC 
               then this bit needs to be set.
               
     */
    UINT32 pcommitforcebroadcast : 1;
    /* pcommitforcebroadcast - Bits[10:10], RW, default = 1'd0 
       
               Force directed pcommits to broadcast to all channels connected to this 
       mesh2mem. 
               Needs to be set when mirroring is enabled on this mesh2mem.
               
     */
    UINT32 immediatefailoveractionenaddrt : 1;
    /* immediatefailoveractionenaddrt - Bits[11:11], RW, default = 1'd0 
       
               Immediately Failover Action Enable for DDRT 
       (ImmediateFailoverActionEnaDdrt): 
                 0: Do not failover a channel when mirror scrub read returns 
       uncorrectable data on DDRT. 
                    Instead BIOS has the smarts and decision power to either (i) do 
       nothing, or  
                    to (ii) failover the complete channel through the MirrorFailover 
       register. 
                 1: Hardware will permanently failover the channel when a mirror scrub 
       read returns uncorrectable data. 
                    Hardware will set the appropriate failover bit in MirrorFailover. 
       When this bit is set,  
                    also XPT/Intel UPI prefetch needs to be disabled in the core and at 
       the Intel UPI agent respectively,  
                    and Defeatures0[IngBypDis] need to be set. In other words, in 
       mirroring hardware failover mode,  
                    both XPT prefetch and ingress bypass need to be disabled.
                    
                    Note: this register will only take effect if 
       SysFeatures0[ImmediateFailoverActionEna] is also enabled.  
                    SysFeatures0[ImmediateFailoverActionEna] alone will enable HW 
       failover on DDR4 channels, setting this register 
                    enables HW failover for both DDR4 and DDRT mirrored channels.  
               
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SYSFEATURES0_M2MEM_MAIN_STRUCT;



/* DEFEATURES0_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402400E4)                                                  */
/*       SKX (0x402400E4)                                                     */
/* Register default value:              0x20800010                            */
#define DEFEATURES0_M2MEM_MAIN_REG 0x060040E4
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       This register contains m2m BIOS visible defeatures which are useful for 
 * system debug. 
 *       
 */
typedef union {
  struct {
    UINT32 discritchunk : 1;
    /* discritchunk - Bits[0:0], RW_LB, default = 1'd0 
       
               Disable DDR4 critical data chunk return (DisCritChunk):
                 0: critical chunk return enabled.
                 1: disable critical chunk return.
               When critical chunk return is enabled, address[5] indicates which 32B 
       half cache line  
               to return first. Critical chunk can only be returned from DDR4, when not 
       enabled as  
               NMC.
               BIOS has to disable critical chunk return if NMC enabled on the m2m. 
               It is illegal to enable critical chunk return when NMC is enabled on 
       that m2m. 
               
     */
    UINT32 wait4bothhalves : 1;
    /* wait4bothhalves - Bits[1:1], RW_LB, default = 1'd0 
       
               Wait for both Halves (Wait4BothHalves):
               Wait for error indication on 2nd half of the cache line before sending 
       out the first half. 
                 0: Send 1st cache line half to the mesh as soon as it arrives from MC 
       (default). 
                 1: Hold on to 1st cache line half and wait for 2nd half of cache line.
               Some of the MC ECC schemes only provide an error indication on the 2nd 
       half of the cache line. 
               In that case need to set this bit to 1. Not doing so is illegal.
               If NMC enabled then need to set this bit to 1.
               
     */
    UINT32 ingbypdis : 1;
    /* ingbypdis - Bits[2:2], RW, default = 1'd0 
       
               Ingress Bypass Disable (IngBypDis):
               CHA/Mesh read requests to DDR4 can bypass the m2m ingress if all m2m 
       ingressQs  
               to that DDR4 channel are empty.
                 0: Enable ingress bypass (default).
                 1: Disable DDR4 ingress bypass (debug).
               
     */
    UINT32 egrbypdis : 1;
    /* egrbypdis - Bits[3:3], RW, default = 1'd0 
       
               Egress Bypass Disable (EgrBypDis):
               DDR4 data to mesh can bypass the m2m egress if the egressQs are empty.
                 0: Enable egress bypass (default).
                 1: Disable egress bypass (debug).
               
     */
    UINT32 wciloptdis : 1;
    /* wciloptdis - Bits[4:4], RW_LB, default = 1'd1 
       
               Wcil optimization Disable (WcilOptDis):
               Wcil optimization consists of dropping the MemInvXtoI memory tag/dir NM 
       update (in case of a NM miss)  
               and not reading data from FM.
                 0: Drop NM tag/dir wr on NM miss (default).
                 1: Follow normal NM miss flow (debug).
               
     */
    UINT32 frcpmemdirupdate2ddrt : 1;
    /* frcpmemdirupdate2ddrt - Bits[5:5], RW_LB, default = 1'd0 
       
               Force NM Pmem dir updates to DDRT (FrcPmemDirUpdate2T
               By default, for multi-socket performance reasons, directory only updates 
       in NM for PMem Write-Through regions 
       are not propagated to DDRT.
                 0: Dont propagate PMem NM dir-only updates to DDRT (default).
                 1: Force propagation of NM PMem dir updates to DDRT (debug).
               This bit is only relevant in PMemCaching mode (Mode.PmemCaching==1).
               
     */
    UINT32 egrbypdirupddis : 1;
    /* egrbypdirupddis - Bits[6:6], RW, default = 1'd0 
       
               If egress bypass is enabled, this will disable directory update 
       bypasses.  For A-step this is not functional, B-step this  
               functionality will be added.
                 0: Directory update egress bypass is enabled.
                 1: Directory update egress bypass is disabled.
               
     */
    UINT32 ingbyprpqcrdfixdis : 1;
    /* ingbyprpqcrdfixdis - Bits[7:7], RW_LB, default = 1'd0 
       
               Ingress Bypass Credit fix Disable (IngBypRpqCrdFixDis):
               CHA/Mesh read requests to DDR4 can bypass the m2m ingress if all m2m 
       ingressQs  
               to that DDR4 channel are empty. This bit will disable a late bug fix in 
       case it adds new issues/complications. 
                 0: Enable ingress bypass credit fix b303006 (default).
                 1: Disable DDR4 ingress bypass credit fix b303006 (debug).
               
     */
    UINT32 egrbypdirupddisfix : 1;
    /* egrbypdirupddisfix - Bits[8:8], RW_LB, default = 1'd0 
       
               Egress Bypass directory update fix disable:
               Disable bug 303088 fix.
                 0: Enable bug fix for b303088 (default).
                 1: Disable bug fix for b303006 (debug).
               
     */
    UINT32 egrbypdirupdperffixdis : 1;
    /* egrbypdirupdperffixdis - Bits[9:9], RW_LB, default = 1'd0 
       
               Egress Bypass directory update performance enhancement fix.  This will 
       remove the early completion (trkfifo) requirement that this  
               be empty in order to enable bypass mode.
                 0: Enable bug fix for b303124 (default).
                 1: Disable bug fix for b303124 (debug).
               
     */
    UINT32 rsvd_10 : 10;
    /* rsvd_10 - Bits[19:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 demandscrubwrdis : 1;
    /* demandscrubwrdis - Bits[20:20], RW, default = 1'd0 
       
               Demand Scrub Disable (DemandScrubWrDis):
               Disable demand scrub write on a read error. Note that explicit writes or 
       directory writes will still happen. 
                 0: Demand scrub writes enabled (default).
                 1: Demand scrub writes disabled (debug).
               On a demand scrub write, in case of a correctable error, the corrected 
       data will be written back to memory,  
               and in case of an uncorrectable error, a poison indication will be 
       written to memory (no matter what the system  
               poison mode setting is).
               This disable gets only honored in 1LM DDR4 mode, as soon as for example 
       DDRT or 2LM are enabled this defeature will not be honored. 
               
     */
    UINT32 scrubcheckrddis : 1;
    /* scrubcheckrddis - Bits[21:21], RW, default = 1'd0 
       
               Scrub Check Read Disable (ScrubCheckRdDis):
               Disable scrub check read:
                 0: Scrub check read enabled (default).
                 1: Scrub check read disabled (debug).
               After a scrub write, the default flow is to perform a check read to 
       determine that the scrub  
               write was successful. This is relevant in the context of mirroring.
               During mirroring (before failover), on an uncorrectable/poison on 
       channel A,  
               the data will be read from the mirror channel B and written back to both 
       channels. 
               The mirror scrub has to write the good data read from channel B to the 
       erroneous channel A and  
               then read it back from channel A to check that the data is no longer 
       uncorrectable. In case the  
               data is still uncorrectable, as determined by the check read, a 
       (permanent) failover to channel B  
               might take place and channel A, which has the persistently uncorrectable 
       error, will no longer be  
               accessible.
               
     */
    UINT32 ddr4maxretries : 3;
    /* ddr4maxretries - Bits[24:22], RW, default = 3'd2 
       
               Max retries (MaxRetries):
               A transaction that is part of the error flow will always be retried at 
       least once. MaxRetries specifies  
               how many more times on top of this one retry a transaction is retried 
       before the error indication  
               from MC is considered an 'uncorrectable' error on that channel.
               Note, in mirror mode, the primary channel will be retried exactly once 
       before the secondary mirror  
               channel will be accessed.
               Default is 2 (to take care of ECC error with transient link error case). 
       Legal range: 0-7. It is  
               suggested to keep this low (2 or 3) so that during a long burst of 
       severe errors no false time-outs occur. 
               The combination EccOnRetry=0 with MaxRetries==0 is illegal because it 
       will result in ECC corrections 
               never to be invoked/applied.
               If trying to work around an unreliable link (e.g. during initial silicon 
       bring up), setting this to a  
               higher value (e.g. 5) might help.
               When NMcaching is enabled, never set this to a value smaller than 2 as 
       that might result in meta-data error  
               misinterpretation. In NMcaching mode, if one does require a value of 1 
       then it is required to set EccOnRetry=1 so 
               as to not get meta-data error misinterpretation but this goes at the 
       cost of error diagnostics (cannot detect  
               transient errors and instead will all be diagnosed as ECC corrected 
       errors). 
               
     */
    UINT32 ecconretry : 1;
    /* ecconretry - Bits[25:25], RW, default = 1'd0 
       
               ECC correction on first retry (EccOnRetry):
               In principle, from a reliability point of view, it is better to resolve 
       an  
               error by retry (just in case error might be transient) than by ECC. ECC 
       always has a probability, 
               however small it might be, that in severe (and possibly some transient) 
       error cases  
               the data gets miscorrected. Hence it is in general better to retry first 
       without  
               invoking ECC correction and only then invoke ECC correction.
                 0: retry without ECC first and only on second retry invoke ECC 
       correction (default). 
                 1: ECC correction on every retry.
               When this bit is 1 then MaxRetries should be set to 0 or higher.
               When this bit is 0 then MaxRetries should be set to 1 or higher, 
               otherwise ECC corrections (including SDDC/DDDC) would never be applied.
               It is recommended to use EccOnRetry=0 and MaxRetries > 1 so that the 
       error handling  
               FSM can distinguish transient errors from ECC corrected errors.
               
     */
    UINT32 badchnlfirstforrd : 1;
    /* badchnlfirstforrd - Bits[26:26], RW, default = 1'd0 
       
               Perform all retries for read/meminv on bad mirror channel first 
       (BadChnlFirst): 
                 1: Perform all rd retries on 'bad' mirror channel first before reading 
       from 'good' mirror channel (defeature). 
                 0: After first retry on 'bad' mirror channel, perform rd (and possibly 
       retries) of the 'good' mirror    
                    channel before any more retries on the 'bad' channel (default).
               For non-mirror regions, this register setting will have no effect.
               
     */
    UINT32 badchnlfirstforpwr : 1;
    /* badchnlfirstforpwr - Bits[27:27], RW, default = 1'd0 
       
               Perform all retries for readd/meminv on bad mirror channel first 
       (BadChnlFirst): 
                 1: Perform all rd retries on 'bad' mirror channel first before reading 
       from 'good' mirror channel (defeature). 
                 0: After first retry on 'bad' mirror channel, perform rd (and possibly 
       retries) of the 'good' mirror    
                    channel before any more retries on the 'bad' channel (default).
               For non-mirror regions, this register setting will have no effect.
               
     */
    UINT32 ddrtmaxretries : 3;
    /* ddrtmaxretries - Bits[30:28], RW, default = 3'd2 
       
               Max retries (MaxRetries):
               A transaction that is part of the error flow will always be retried at 
       least once. MaxRetries specifies  
               how many more times on top of this one retry a transaction is retried 
       before the error indication  
               from MC is considered an 'uncorrectable' error on that channel. 
        Also DDRTMaxRetries is the total no of retries in case NM retry issues a FM 
       transaction as all retries in this case are counted towards the threshold        
               Note, in mirror mode, the primary channel will be retried exactly once 
       before the secondary mirror  
               channel will be accessed.
               Default is 2. Legal range: 0-7. It is suggested to keep this low (2 or 
       3) so that during a  
               long burst of severe errors no false time-outs occur.
               The combination EccOnRetry=0 with MaxRetries==0 is illegal because it 
       will result in ECC corrections 
               never to be invoked/applied.
               
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DEFEATURES0_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register contains m2m BIOS visible defeatures which are useful for 
 * system debug. 
 *       
 */
typedef union {
  struct {
    UINT32 discritchunk : 1;
    /* discritchunk - Bits[0:0], RW_LB, default = 1'd0 
       
               Disable DDR4 critical data chunk return (DisCritChunk):
                 0: critical chunk return enabled.
                 1: disable critical chunk return.
               When critical chunk return is enabled, address[5] indicates which 32B 
       half cache line  
               to return first. Critical chunk can only be returned from DDR4, when not 
       enabled as  
               NMCache.
               BIOS has to disable critical chunk return when DDRT is present, i.e. as 
       soon as one of  
               NmCaching, Persistent memory or Block region is enabled on that m2m.
               It is illegal to enable critical chunk return when NMCaching, Persistent 
       memory or  
               Block region is enabled on that m2m.
               
     */
    UINT32 wait4bothhalves : 1;
    /* wait4bothhalves - Bits[1:1], RW_LB, default = 1'd0 
       
               Wait for both Halves (Wait4BothHalves):
               Wait for error indication on 2nd half of the cache line before sending 
       out the first half. 
                 0: Send 1st cache line half to the mesh as soon as it arrives from MC 
       (default). 
                 1: Hold on to 1st cache line half and wait for 2nd half of cache line.
               Some of the MC ECC schemes only provide an error indication on the 2nd 
       half of the cache line. 
               In that case need to set this bit to 1. Not doing so is illegal.
               If NMC enabled or DDRT is present on this M2M then need to set this bit 
       to 1. 
               
     */
    UINT32 ingbypdis : 1;
    /* ingbypdis - Bits[2:2], RW, default = 1'd0 
       
               Ingress Bypass Disable (IngBypDis):
               CHA/Mesh read requests to DDR4 can bypass the m2m ingress if all m2m 
       ingressQs  
               to that DDR4 channel are empty.
                 0: Enable ingress bypass (default).
                 1: Disable DDR4 ingress bypass (debug).
               
     */
    UINT32 egrbypdis : 1;
    /* egrbypdis - Bits[3:3], RW, default = 1'd0 
       
               Egress Bypass Disable (EgrBypDis):
               DDR4 data to mesh can bypass the m2m egress if the egressQs are empty.
                 0: Enable egress bypass (default).
                 1: Disable egress bypass (debug).
               
     */
    UINT32 rsvd_4 : 1;
    UINT32 frcpmemdirupdate2ddrt : 1;
    /* frcpmemdirupdate2ddrt - Bits[5:5], RW_LB, default = 1'd0 
       
               Force NM Pmem dir updates to DDRT (FrcPmemDirUpdate2DDRT)
               By default, for multi-socket performance reasons, directory only updates 
       in NM for PMem Write-Through regions 
       are not propagated to DDRT.
                 0: Dont propagate PMem NM dir-only updates to DDRT (default).
                 1: Force propagation of NM PMem dir updates to DDRT (debug).
               This bit is only relevant in PMemCaching mode (Mode.PmemCaching==1).
               
     */
    UINT32 egrbypdirupddis : 1;
    /* egrbypdirupddis - Bits[6:6], RW, default = 1'd0 
       
               If egress bypass is enabled, this will disable directory update 
       bypasses.  For A-step this is not functional, B-step this  
               functionality will be added.
                 0: Directory update egress bypass is enabled.
                 1: Directory update egress bypass is disabled.
               
     */
    UINT32 ingbyprpqcrdfixdis : 1;
    /* ingbyprpqcrdfixdis - Bits[7:7], RW_LB, default = 1'd0 
       
               Ingress Bypass Credit fix Disable (IngBypRpqCrdFixDis):
               CHA/Mesh read requests to DDR4 can bypass the m2m ingress if all m2m 
       ingressQs  
               to that DDR4 channel are empty. This bit will disable a late bug fix in 
       case it adds new issues/complications. 
                 0: Enable ingress bypass credit fix b303006 (default).
                 1: Disable DDR4 ingress bypass credit fix b303006 (debug).
               
     */
    UINT32 egrbypdirupddisfix : 1;
    /* egrbypdirupddisfix - Bits[8:8], RW_LB, default = 1'd0 
       
               Egress Bypass directory update fix disable:
               Disable bug 303088 fix.
                 0: Enable bug fix for b303088 (default).
                 1: Disable bug fix for b303006 (debug).
               
     */
    UINT32 egrbypdirupdperffixdis : 1;
    /* egrbypdirupdperffixdis - Bits[9:9], RW_LB, default = 1'd0 
       
               Egress Bypass directory update performance enhancement fix.  This will 
       remove the early completion (trkfifo) requirement that this  
               be empty in order to enable bypass mode.
                 0: Enable bug fix for b303124 (default).
                 1: Disable bug fix for b303124 (debug).
               
     */
    UINT32 prefetchddrtbypfixdis : 1;
    /* prefetchddrtbypfixdis - Bits[10:10], RW_LB, default = 1'd0 
       
               Disable bypass fix for 1lm transactions when prefetch and DDRT (non-2lm) 
       modes are enabled.  This will disable the fix for b305166 and 
               revert back to known A0 behavior.
                 0: Enable bug fix for b305166 (default).
                 1: Disable bug fix for b305166 (debug).
               
     */
    UINT32 enablewrcredsideband : 1;
    /* enablewrcredsideband - Bits[11:11], RWS_L, default = 1'd0 
       
               Enable sideband return of write credits to CHA
                 1: Write credits are returned to CHA via sideband bus.
                 0: Write credits are returned to CHA via AK ring.
               
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[19:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 demandscrubwrdis : 1;
    /* demandscrubwrdis - Bits[20:20], RW, default = 1'd0 
       
               Demand Scrub Disable (DemandScrubWrDis):
               Disable demand scrub write on a read error. Note that explicit writes or 
       directory writes will still happen. 
                 0: Demand scrub writes enabled (default).
                 1: Demand scrub writes disabled (debug).
               On a demand scrub write, in case of a correctable error, the corrected 
       data will be written back to memory,  
               and in case of an uncorrectable error, a poison indication will be 
       written to memory (no matter what the system  
               poison mode setting is).
               This disable gets only honored in 1LM DDR4 mode, as soon as for example 
       DDRT or 2LM are enabled this defeature will not be honored. 
               
     */
    UINT32 scrubcheckrddis : 1;
    /* scrubcheckrddis - Bits[21:21], RW, default = 1'd0 
       
               Scrub Check Read Disable (ScrubCheckRdDis):
               Disable scrub check read:
                 0: Scrub check read enabled (default).
                 1: Scrub check read disabled (debug).
               After a scrub write, the default flow is to perform a check read to 
       determine that the scrub  
               write was successful. This is relevant in the context of mirroring.
               During mirroring (before failover), on an uncorrectable/poison on 
       channel A,  
               the data will be read from the mirror channel B and written back to both 
       channels. 
               The mirror scrub has to write the good data read from channel B to the 
       erroneous channel A and  
               then read it back from channel A to check that the data is no longer 
       uncorrectable. In case the  
               data is still uncorrectable, as determined by the check read, a 
       (permanent) failover to channel B  
               might take place and channel A, which has the persistently uncorrectable 
       error, will no longer be  
               accessible.
               When mirroring DDRT volatile region (which implies 2LM/NM-caching mode) 
       it is illegal to disable the  
               scrub check read as it is an integral part of handling NM-cache misses 
       under Far-Memory errors correctly  
               when mirroring (would violate local inclusiveness). The exception to 
       this rule is disabling scrub check rd  
               together with SysFeatures0[FrcWrNI]=1; that is legal.
               
     */
    UINT32 ddr4maxretries : 3;
    /* ddr4maxretries - Bits[24:22], RW, default = 3'd2 
       
               Max retries (MaxRetries):
               A transaction that is part of the error flow will always be retried at 
       least once. MaxRetries specifies  
               how many more times on top of this one retry a transaction is retried 
       before the error indication  
               from MC is considered an 'uncorrectable' error on that channel.
               Note, in mirror mode, the primary channel will be retried exactly once 
       before the secondary mirror  
               channel will be accessed.
               Default is 2 (to take care of ECC error with transient link error case). 
       Legal range: 0-7. It is  
               suggested to keep this low (2 or 3) so that during a long burst of 
       severe errors no false time-outs occur. 
               The combination EccOnRetry=0 with MaxRetries==0 is illegal because it 
       will result in ECC corrections 
               never to be invoked/applied.
               If trying to work around an unreliable link (e.g. during initial silicon 
       bring up), setting this to a  
               higher value (e.g. 5) might help.
               When NMcaching is enabled, never set this to a value smaller than 2 as 
       that might result in meta-data error  
               misinterpretation. In NMcaching mode, if one does require a value of 1 
       then it is required to set EccOnRetry=1 so 
               as to not get meta-data error misinterpretation but this goes at the 
       cost of error diagnostics (cannot detect  
               transient errors and instead will all be diagnosed as ECC corrected 
       errors). 
               
     */
    UINT32 ecconretry : 1;
    /* ecconretry - Bits[25:25], RW, default = 1'd0 
       
               ECC correction on first retry (EccOnRetry):
               In principle, from a reliability point of view, it is better to resolve 
       an  
               error by retry (just in case error might be transient) than by ECC. ECC 
       always has a probability, 
               however small it might be, that in severe (and possibly some transient) 
       error cases  
               the data gets miscorrected. Hence it is in general better to retry first 
       without  
               invoking ECC correction and only then invoke ECC correction.
                 0: retry without ECC first and only on second retry invoke ECC 
       correction (default). 
                 1: ECC correction on every retry.
               When this bit is 1 then MaxRetries should be set to 0 or higher.
               When this bit is 0 then MaxRetries should be set to 1 or higher, 
               otherwise ECC corrections (including SDDC/DDDC) would never be applied.
               It is recommended to use EccOnRetry=0 and MaxRetries > 1 so that the 
       error handling  
               FSM can distinguish transient errors from ECC corrected errors.
               
     */
    UINT32 badchnlfirstforrd : 1;
    /* badchnlfirstforrd - Bits[26:26], RW, default = 1'd0 
       
               Perform all retries for read/meminv on bad mirror channel first 
       (BadChnlFirst): 
                 1: Perform all rd retries on 'bad' mirror channel first before reading 
       from 'good' mirror channel (defeature). 
                 0: After first retry on 'bad' mirror channel, perform rd (and possibly 
       retries) of the 'good' mirror    
                    channel before any more retries on the 'bad' channel (default).
               For non-mirror regions, this register setting will have no effect.
               
     */
    UINT32 badchnlfirstforpwr : 1;
    /* badchnlfirstforpwr - Bits[27:27], RW, default = 1'd0 
       
               Perform all retries for readd/meminv on bad mirror channel first 
       (BadChnlFirst): 
                 1: Perform all rd retries on 'bad' mirror channel first before reading 
       from 'good' mirror channel (defeature). 
                 0: After first retry on 'bad' mirror channel, perform rd (and possibly 
       retries) of the 'good' mirror    
                    channel before any more retries on the 'bad' channel (default).
               For non-mirror regions, this register setting will have no effect.
               
     */
    UINT32 ddrtmaxretries : 3;
    /* ddrtmaxretries - Bits[30:28], RW, default = 3'd2 
       
               Max retries (MaxRetries):
               A transaction that is part of the error flow will always be retried at 
       least once. MaxRetries specifies  
               how many more times on top of this one retry a transaction is retried 
       before the error indication  
               from MC is considered an 'uncorrectable' error on that channel. 
        Also DDRTMaxRetries is the total no of retries in case NM retry issues a FM 
       transaction as all retries in this case are counted towards the threshold        
               Note, in mirror mode, the primary channel will be retried exactly once 
       before the secondary mirror  
               channel will be accessed.
               Default is 2. Legal range: 0-7. It is suggested to keep this low (2 or 
       3) so that during a  
               long burst of severe errors no false time-outs occur.
               The combination EccOnRetry=0 with MaxRetries==0 is illegal because it 
       will result in ECC corrections 
               never to be invoked/applied.
               
     */
    UINT32 enablerdcredsideband : 1;
    /* enablerdcredsideband - Bits[31:31], RWS_L, default = 1'd0 
       
               Enable sideband return of read credits to CHA
                 1: Read credits are returned to CHA via sideband bus.
                 0: Read credits are returned to CHA via AK ring.
               
     */
  } Bits;
  UINT32 Data;
} DEFEATURES0_M2MEM_MAIN_STRUCT;



/* DEFEATURES1_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402400E8)                                                  */
/*       SKX (0x402400E8)                                                     */
/* Register default value on SKX_A0:    0x00000000                            */
/* Register default value on SKX:       0x00000200                            */
#define DEFEATURES1_M2MEM_MAIN_REG 0x060040E8
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       This register contains m2m BIOS visible defeatures which are useful for 
 * system debug. 
 *       
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 fatalonnmcacherducdata : 1;
    /* fatalonnmcacherducdata - Bits[2:2], RW, default = 1'd0 
       
               Go fatal for uncorrectable from Near-Memory Cache read 
       (FatalOnNmCachePwrUcData): 
                 1: In case of Near-Memory uncorrectable error, go fatal.
                 0: If Near-Memory data-only uncorrectable error then try to recover 
       from that by either  
                    getting the data from Far-Memory in case of clean NM cache state, 
       or by poisoning the data  
                    in case of modified NM cache state (default).
               
     */
    UINT32 fatalonnmcachepwrucdata : 1;
    /* fatalonnmcachepwrucdata - Bits[3:3], RW, default = 1'd0 
       
               Go fatal for uncorrectable from Near-Memory Cache partial write 
       (FatalOnNmCachePwrUcData): 
                 1: In case of Near-Memory uncorrectable error, go fatal.
                 0: If Near-Memory data-only uncorrectable error then try to recover 
       from that by (default). 
                    It might mean that for some classes of errors, the hardware will 
       force wrNI behavior from that 
                    point onwards for non-wrNI transactions from CHA.
               
     */
    UINT32 frcmetadataerr : 1;
    /* frcmetadataerr - Bits[4:4], RW_LB, default = 1'd0 
       
               Force meta data error on read error (FrcMetaDataErr):
                 1: Set meta data error on a read error from MC
                 0: Do nothing (default).
               
     */
    UINT32 rsvd_5 : 1;
    /* rsvd_5 - Bits[5:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrtmirrscrubwrdis : 1;
    /* ddrtmirrscrubwrdis - Bits[6:6], RW, default = 1'd0 
       
               Disable mirror scrub write to DDRT (DdrtMirrScrubWrDis):
                 1: Don't scrub write the corrected data back to the 'bad channel'. 
       Hence stored data still stays bad on that channel till a write comes back from 
       CHA. 
                 0: Scrub write the corrected data back to the 'bad channel' and then 
       perform a scrub check read to determine whether corrected data is now correct as 
       stored on the previously bad channel (default). 
               
     */
    UINT32 skipbadchnlforrd : 1;
    /* skipbadchnlforrd - Bits[7:7], RW, default = 1'd0 
       
               Skip bad mirror channel read/meminv retries (SkipBadChnl):
                 1: Skip the 'bad channel retry=1' state in the mirror error FSM.
                 0: Do take the 'bad channel retry=1' state after the 'good channel' 
       states in the mirror error FSM (default). 
               For non-mirror regions, this register setting will have no effect. 
       BadChnlFirstForRd==1 takes priority over this bit. 
               
     */
    UINT32 skipbadchnlforpwr : 1;
    /* skipbadchnlforpwr - Bits[8:8], RW, default = 1'd0 
       
               Skip bad mirror channel partial write retries (SkipBadChnl):
                 1: Skip the 'bad channel retry=1' state in the mirror error FSM.
                 0: Do take the 'bad channel retry=1' state after the 'good channel' 
       states in the mirror error FSM (default). 
               For non-mirror regions, this register setting will have no effect. 
       BadChnlFirstForPwr==1 takes priority over this bit. 
               
     */
    UINT32 rsvd_9 : 7;
    /* rsvd_9 - Bits[15:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ingeco : 8;
    /* ingeco - Bits[23:16], RW, default = 8'd0 
       
               Ingress ECO defeatures (IngEco):
                 8 configuration bits reserved for ingress ECOs.
                 bit 16:   setting this bit will disable the prefetch ECO fix for 
       b303685.  default behavior is to have it enabled and do not allow any  
                           demand reads to hit into the cam 1clk after a cam entry has 
       been deallocated. 
               
     */
    UINT32 egreco : 8;
    /* egreco - Bits[31:24], RW, default = 8'd0 
       
               Egress ECO defeatures (EgrEco):
                 8 configuration bits reserved for egress ECOs.
                 Bit 26: prevent NoRetry from forcing quiesce (b304049)
                 Bit 25: defeatures a 1lm retry fix where BL address array can get 
       corrupted (b303702) 
                 Bit 24: defeatures a 2lm retry fix where fills are getting blocked 
       during retry fsm leading to a deadlock (b303594) 
               
     */
  } Bits;
  UINT32 Data;
} DEFEATURES1_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register contains m2m BIOS visible defeatures which are useful for 
 * system debug. 
 *       
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 uconnmcacherducdata : 1;
    /* uconnmcacherducdata - Bits[2:2], RW, default = 1'd0 
       
               Go fatal/recov for uncorrectable from Near-Memory Cache read 
       (UcOnNmCachePwrUcData): 
                 1: In case of Near-Memory uncorrectable error, go 
       fatal/recov(depending on ignorepoison). 
                 0: If Near-Memory data-only uncorrectable error then try to recover 
       from that by either  
                    getting the data from Far-Memory in case of clean NM cache state, 
       or by poisoning the data  
                    in case of modified NM cache state (default).
               
     */
    UINT32 uconnmcachepwrucdata : 1;
    /* uconnmcachepwrucdata - Bits[3:3], RW, default = 1'd0 
       
               If we hit uncorrectable on data return from nmcache for a Underfill 
       read: 
       
               (UcOnNmCachePwrUcData == 1 AND IgnorePoisonOnNmCacheUcData == 1)    
       UC=1,PCC=1 
               (UcOnNmCachePwrUcData == 1 AND IgnorePoisonOnNmCacheUcData == 0 AND 
       CfgPoisonEn == 0)   UC=1,PCC=1 
               (UcOnNmCachePwrUcData == 1 AND IgnorePoisonOnNmCacheUcData == 0 AND 
       CfgPoisonEn == 1)  UC=1,PCC=0 
               (UcOnNmCachePwrUcData == 0 AND NM Miss AND CfgMcaLogCorrOnWrMissUc == 1) 
                      UC=0,PCC=0 
               (UcOnNmCachePwrUcData == 0 AND NM Miss AND CfgMcaLogCorrOnWrMissUc == 0) 
                      No logging 
               (UcOnNmCachePwrUcData == 0 AND NM Hit AND CfgPoisonEn == 0)              
                      UC=1,PCC=1 
               (UcOnNmCachePwrUcData == 0 AND NM Hit AND CfgPoisonEn == 1)              
                      UC=1,PCC=0 
               
     */
    UINT32 frcmetadataerr : 1;
    /* frcmetadataerr - Bits[4:4], RW_LB, default = 1'd0 
       
               Force meta data error on read error (FrcMetaDataErr):
                 1: Set meta data error on a read error from MC
                 0: Do nothing (default).
               
     */
    UINT32 rsvd_5 : 1;
    /* rsvd_5 - Bits[5:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrtmirrscrubwrdis : 1;
    /* ddrtmirrscrubwrdis - Bits[6:6], RW, default = 1'd0 
       
               Disable mirror scrub write to DDRT (DdrtMirrScrubWrDis):
                 1: Don't scrub write the corrected data back to the 'bad channel'. 
       Hence stored data still stays bad on that channel till a write comes back from 
       CHA. 
                 0: Scrub write the corrected data back to the 'bad channel' and then 
       perform a scrub check read to determine whether corrected data is now correct as 
       stored on the previously bad channel (default). 
               
     */
    UINT32 skipbadchnlforrd : 1;
    /* skipbadchnlforrd - Bits[7:7], RW, default = 1'd0 
       
               Skip bad mirror channel read/meminv retries (SkipBadChnl):
                 1: Skip the 'bad channel retry=1' state in the mirror error FSM.
                 0: Do take the 'bad channel retry=1' state after the 'good channel' 
       states in the mirror error FSM (default). 
               For non-mirror regions, this register setting will have no effect. 
       BadChnlFirstForRd==1 takes priority over this bit. 
               
     */
    UINT32 skipbadchnlforpwr : 1;
    /* skipbadchnlforpwr - Bits[8:8], RW, default = 1'd0 
       
               Skip bad mirror channel partial write retries (SkipBadChnl):
                 1: Skip the 'bad channel retry=1' state in the mirror error FSM.
                 0: Do take the 'bad channel retry=1' state after the 'good channel' 
       states in the mirror error FSM (default). 
                 For non-mirror regions, this register setting will have no effect. 
       BadChnlFirstForPwr==1 takes priority over this bit. 
                 In SKX, if BadChnlFirstForPwr==0, then SkipBadChnlForPwr must be set 
       to 1 for configs where DDRT is present. If ECC retry 
                 on the bad channel is desired, then set BadChnlFirstForPwr=1 instead.  
         
               
     */
    UINT32 ignorepoisononnmcacheucdata : 1;
    /* ignorepoisononnmcacheucdata - Bits[9:9], RW, default = 1'd1 
       
               If we hit uncorrectable on data return from nmcache for a Read:
       
               (UcOnNmCacheRdUcData == 1 AND IgnorePoisonOnNmCacheUcData == 1)  
       UC=1,PCC=1 
               (UcOnNmCacheRdUcData == 1 AND IgnorePoisonOnNmCacheUcData == 0 AND 
       CfgPoisonEn == 0)  UC=1,PCC=1 
               (UcOnNmCacheRdUcData == 1 AND IgnorePoisonOnNmCacheUcData == 0 AND 
       CfgPoisonEn == 1)  UC=1,PCC=0 
       
               If we hit uncorrectable on data return from nmcache for a Underfill 
       read: 
       
               (UcOnNmCachePwrUcData == 1 AND IgnorePoisonOnNmCacheUcData == 1)  
       UC=1,PCC=1 
               (UcOnNmCachePwrUcData == 1 AND IgnorePoisonOnNmCacheUcData == 0 AND 
       CfgPoisonEn == 0)  UC=1,PCC=1 
               (UcOnNmCachePwrUcData == 1 AND IgnorePoisonOnNmCacheUcData == 0 AND 
       CfgPoisonEn == 1)  UC=1,PCC=0 
       
               IgnorePoisonOnNmCacheUcData is ignored if: 
               UcOnNmCacheRdUcData == 0 (if the error is on a rd) OR
               UcOnNmCachePwrUcData == 0 (if the error is on a pwr)
               
     */
    UINT32 cfgfailoveronscrubchkrdcorr : 1;
    /* cfgfailoveronscrubchkrdcorr - Bits[10:10], RW, default = 1'd0 
       
                When set allow failover on every scrub read following a scrub write. 
       Has to be set with 
       		CfgImmediateFailoverActionEna to allow failover										       
               
     */
    UINT32 rsvd_11 : 5;
    /* rsvd_11 - Bits[15:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ingeco : 8;
    /* ingeco - Bits[23:16], RW, default = 8'd0 
       
               Ingress ECO defeatures (IngEco):
                 8 configuration bits reserved for ingress ECOs.
                 bit 16:   setting this bit will disable the prefetch ECO fix for 
       b303685.  default behavior is to have it enabled and do not allow any  
                           demand reads to hit into the cam 1clk after a cam entry has 
       been deallocated. 
                 bit 17:   when set, will use bit7 instead of bit6 for 64B prefetch 
       target channel interleave.  only applies in 2ch MC topologies. 
                 bit 18:   when set, will use bit9 instead of bit8 for 256B prefetch 
       target channel interleave.  only applies in 2ch MC topologies. 
       	  bit 19:   defeature bit for b310985
       	  bit 20:   defeature bit for b310986
       	  bit 21:   defeature bit for b310776
          bit 22:   set to disable timeout module for read tracker.Specifically needed 
       for cases where a bypassed demand read is a hit in  
                    prefetch cam.Not needed if either prefetch or ingress bypass is 
       disabled.        
               
     */
    UINT32 egreco : 8;
    /* egreco - Bits[31:24], RW, default = 8'd0 
       
               Egress ECO defeatures (EgrEco):
                8 configuration bits reserved for egress ECOs.
                 Bit 27: disable extra retry during errorFSM PWR scrub read state. Must 
       be set to 1'b1 in 1LM mirroring configs, must NOT be set in 2LM configs 
       (b307536) 
                 Bit 26: prevent NoRetry from forcing quiesce (b304049)
                 Bit 25: defeatures a 1lm retry fix where BL address array can get 
       corrupted (b303702) 
                 Bit 24: defeatures a 2lm retry fix where fills are getting blocked 
       during retry fsm leading to a deadlock (b303594) 
               
     */
  } Bits;
  UINT32 Data;
} DEFEATURES1_M2MEM_MAIN_STRUCT;



/* MIRRORCHNLS_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402400F0)                                                  */
/*       SKX (0x402400F0)                                                     */
/* Register default value:              0x00000249                            */
#define MIRRORCHNLS_M2MEM_MAIN_REG 0x060040F0
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Configuration register specifying the paired physical mirror channels.
 *       Only used for mirroring.
 *       In case of 3 channel interleave, only one map is legal:
 *         primary chnl0 -> secondary chnl1
 *         primary chnl1 -> secondary chnl2
 *         primary chnl2 -> secondary chnl0
 *       Mapping the primary and secondary channels to the same channel is 
 * unvalidated hence not supported. 
 *       
 */
typedef union {
  struct {
    UINT32 ddr4chnl0secondary : 2;
    /* ddr4chnl0secondary - Bits[1:0], RW_L, default = 2'h1 
       
               DDR4 Channel 0 secondary (Ddr4Chnl0Secondary):
               Secondary channel associated with primary channel 0.
               
     */
    UINT32 ddr4chnl1secondary : 2;
    /* ddr4chnl1secondary - Bits[3:2], RW_L, default = 2'h2 
       
               DDR4 Channel 1 secondary (Ddr4Chnl1Secondary):
               Secondary channel associated with primary channel 1.
               
     */
    UINT32 ddr4chnl2secondary : 2;
    /* ddr4chnl2secondary - Bits[5:4], RW_L, default = 2'h0 
       
               DDR4 Channel 2 secondary (Ddr4Chnl2Secondary):
               Secondary channel associated with primary channel 2.
               
     */
    UINT32 ddrtchnl0secondary : 2;
    /* ddrtchnl0secondary - Bits[7:6], RW_L, default = 2'h1 
       
               DDR4 Channel 0 secondary (DdrtChnl0Secondary):
               Secondary channel associated with primary channel 0.
               
     */
    UINT32 ddrtchnl1secondary : 2;
    /* ddrtchnl1secondary - Bits[9:8], RW_L, default = 2'h2 
       
               DDR4 Channel 1 secondary (DdrtChnl1Secondary):
               Secondary channel associated with primary channel 1.
               
     */
    UINT32 ddrtchnl2secondary : 2;
    /* ddrtchnl2secondary - Bits[11:10], RW_L, default = 2'h0 
       
               DDRT Channel 2 secondary (DDRTChnl2Secondary):
               Secondary channel associated with primary channel 2.
               
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MIRRORCHNLS_M2MEM_MAIN_STRUCT;


/* MIRRORFAILOVER_M2MEM_MAIN_REG supported on:                                */
/*       SKX_A0 (0x402400F4)                                                  */
/*       SKX (0x402400F4)                                                     */
/* Register default value:              0x00000000                            */
#define MIRRORFAILOVER_M2MEM_MAIN_REG 0x060040F4
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Configuration register providing observability and control as to which 
 * mirror channels failed 
 *       over. The granularity of a failover is all the DDR4 (mirror region) memory 
 * associated with a channel,  
 *       or all the DDRT (mirror region) memory associated with a channel.
 *       
 */
typedef union {
  struct {
    UINT32 ddr4chnlfailed : 3;
    /* ddr4chnlfailed - Bits[2:0], RW_LV, default = 3'h0 
       
               DDR4 Channel Failed (Ddr4ChnlFailed):
               Each bit is an status and control bit for a channel indicating whether 
       the DDR4 memory  
               associated with that channel failed over.
               Bits 2...0 map to channels 2...0.
               Channel failed over semantics: (i) reads will no longer access this 
       channel,  
               (ii) writes will no longer go to this channel (unless ForceWrsToFailed 
       is set). 
               Writing a bit to 1 will force that DDR4 channel to go to failed over 
       state. 
               
     */
    UINT32 rsvd_3 : 1;
    /* rsvd_3 - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrtchnlfailed : 3;
    /* ddrtchnlfailed - Bits[6:4], RW_LV, default = 3'h0 
       
               DDRT Channel Failed (DdrtChnlFailed):
               Each bit is a status and control bit for a channel indicating whether 
       DDRT memory  
               associated with that channel failed over.      
               Bits 6...4 map to channels 2...0.
               Channel failed over semantics: (i) reads will no longer access this 
       channel,  
               (ii) writes will no longer go to this channel (unless ForceWrsToFailed 
       is set). 
               Writing a bit to 1 will force that DDR4 channel to go to failed over 
       state. 
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddr4frcmirrwrstofailed : 1;
    /* ddr4frcmirrwrstofailed - Bits[8:8], RW_L, default = 1'h0 
       
               DDR4 Force Mirror Writes to Failed Channel (Ddr4FrcMirrWrsToFailed):
               Description is identical to above field, but in this case applicable to 
       DDR4 mirror channels. 
               
     */
    UINT32 ddrt4frcmirrwrstofailed : 1;
    /* ddrt4frcmirrwrstofailed - Bits[9:9], RW_L, default = 1'h0 
       
               DDRT Force Mirror Writes to Failed Channel (DDRT4FrcMirrWrsToFailed):
               When set, it forces channel mirror writes to both mirroring channels 
       even 
               after failover. This should only be set during system quiesce, i.e. when 
       no 
               mesh2mem traffic.
               0: Don't send writes to the failover channel. This is the default 
       production 
               setting.
               1: Force writes to also go to the failover channel. This is a debug 
       mode. 
       
       The usage model is to facilitate post silicon exercising of channel mirroring
       while injection uncorrectables on one channel. This will keep writes going to
       the failed channel so that later on one can undo the failover and continue
       testing channel mirroring with failover events.
       Limitations: Partial wrs will touch the failed channel which will still signal
       errors (correctable if data is good on one channel but bad on the other
       channel, fatal if data on both channels is bad). Also, because partial wrs
       touch both channels (even after failover), the data from the failover channel
       might fix the failed channel's data through the mirror scrub flow. Note, the
       failover state of a channel will be left untouched.
               
     */
    UINT32 rsvd_10 : 22;
    /* rsvd_10 - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MIRRORFAILOVER_M2MEM_MAIN_STRUCT;


/* PREFETCHTUNING_M2MEM_MAIN_REG supported on:                                */
/*       SKX_A0 (0x402400F8)                                                  */
/*       SKX (0x402400F8)                                                     */
/* Register default value:              0x00820358                            */
#define PREFETCHTUNING_M2MEM_MAIN_REG 0x060040F8
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This prefetch tuning register contains the thresholds to limit allocation 
 * of available bandwidth to  
 *       local (XPT) prefetches and remote (Intel UPI) prefetches and guarantee a 
 * minimal bandwidth  
 *       allocation to non-speculative reads (and writes). A prefetch in this 
 * context maps to   
 *       a MemSpecRd command.
 * 
 *       Both, XPT and Intel UPI, prefetch flows aim to save latency at the 
 * possible expense of fetching  
 *       unneeded data from memory. A Intel UPI/XPT prefetch will prefetch a cache 
 * line from memory before  
 *       even knowing whether there might be a cache hit in the cache associated 
 * with the home agent CHA. 
 *       The M2M prefetch CAM accomodates a maximum of 32 prefetches per M2M. This 
 * single M2M prefetch CAM is  
 *       shared across all memory channels and is also shared between Intel UPI and 
 * XPT prefetches. 
 *       
 *       It is allowed to reprogram this register on-the-fly, while memory traffic 
 * is flowing. 
 * 
 *       
 */
typedef union {
  struct {
    UINT32 prefrpqthreshold : 6;
    /* prefrpqthreshold - Bits[5:0], RW, default = 6'd24 
       
               Prefetch RPQ Threshold (PrefRPQThreshold):
               RPQ buffer entries threshold for dropping prefetches. 
               Prevents prefetches from grabbing bandwidth if read bandwidth is already 
       high. 
               If the number of RPQ entries occupied in (at least) one memory channel 
       is equal to or  
               greater than this threshold then incoming prefetch commands get dropped.
               
     */
    UINT32 prefwpqthreshold : 6;
    /* prefwpqthreshold - Bits[11:6], RW, default = 6'd13 
       
               Prefetch WPQ Threshold (PrefWPQThreshold)
               WPQ buffer entries threshold for dropping prefetches. 
               Prevents prefetches from grabbing bandwidth if write bandwidth is high.
               If the number of WPQ entries occupied in (at least) one memory channel 
       is  
               equal to or higher than this threshold then incoming prefetch commands 
       get dropped. 
               
     */
    UINT32 prefkticamthreshold : 6;
    /* prefkticamthreshold - Bits[17:12], RW, default = 6'd32 
       
               Intel UPI Prefetch CAM Threshold (PrefKTICamThreshold):
               Prefetch CAM valid entries threshold for dropping Intel UPI prefetches. 
       The prefetch CAM can only support up to 32 entries so never set this threshold 
       above 32, otherwise unexpected system behavior (resulting from CAM overflow) 
       might result. 
               This threshold prevents too many Intel UPI prefetches from outstanding 
       (i.e. throttle back speculative, unnecessary read bandwidth) and hence allows a 
       trade-off to be made between power and performance. 
               Also, by setting the XPT and Intel UPI thresholds differently, one can 
       allow a slight bias of one versus the other. 
               If the number of occupied CAM entries is larger than or equal to the 
       threshold then Intel UPI prefetches will not allocate in the prefetch CAM and 
       get dropped instead. 
               If power is the primary optimization vector (instead of performance) 
       then it  
               is suggested to program this threshold to a low value (e.g. in the range 
       0...3). 
               For absolute performance (at the cost of power) a value of 32 might be 
       good. 
               
     */
    UINT32 prefxptcamthreshold : 6;
    /* prefxptcamthreshold - Bits[23:18], RW, default = 6'd32 
       
               XPT Prefetch CAM Threshold (PrefXPTCamThreshold):
               Prefetch CAM valid entries threshold for dropping local (=XPT) 
       prefetches. The prefetch CAM can only support up to 32 entries so never set this 
       threshold above 32, otherwise unexpected system behavior (resulting from CAM 
       overflow) might result. 
               This threshold prevents too many XPT prefetches from outstanding (i.e. 
       throttle back speculative, unnecessary read bandwidth) and hence allows a 
       trade-off to be made  
               between power and performance.
               Also, by setting the XPT and Intel UPI thresholds differently, one can 
       allow a slight bias of one versus the other. 
               If the number of occupied CAM entries is larger than or equal to the 
       threshold then XPT prefetches will not allocate in the prefetch CAM and get 
       dropped instead. 
               If power is the primary optimization vector (instead of performance) 
       then it  
               is suggested to program this threshold to a low value (e.g. in the range 
       0...3). 
               For absolute performance (at the cost of power) a value of 32 might be 
       good. 
               
     */
    UINT32 pftuningen : 1;
    /* pftuningen - Bits[24:24], RW, default = 1'd0 
       
               Enable bit for prefetch tuning thresholds.
               
     */
    UINT32 cfgdirupdpfcamevictdisable : 1;
    /* cfgdirupdpfcamevictdisable - Bits[25:25], RW, default = 1'd0 
       
               Chicken bit to disable directory updates from invalidating prefetch cam 
       entries. 
               
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PREFETCHTUNING_M2MEM_MAIN_STRUCT;


/* BGFTUNING_M2MEM_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x402400FC)                                                  */
/*       SKX (0x402400FC)                                                     */
/* Register default value:              0x00000000                            */
#define BGFTUNING_M2MEM_MAIN_REG 0x060040FC
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This BGF tuning register contains programmable parameters related to BGF 
 * tuning during its initialization. 
 * 
 *       The flow accommodates BGF sync pulse frequencies of 100MHz, 50MHz, 33MHz, 
 * and 25MHz.  
 *       However, the memory channel frequency is likely to be a multiple of 33MHz. 
 *  
 *       The ratio would have to be programmed with respect to a 33MHz sync pulse, 
 * and the RatioType set  
 *       to use the pcode-programmed ratio exactly.
 *       The BGF sync pulse frequency is provided by a fuse. The fuse encoding is 
 * such that, if the ratio is 
 *       programmed with respect to a 100MHz sync pulse, the fuse value can be 
 * copied directly to the RatioType. 
 * 
 *       * If PCODE_MC_BOOT_CONTROL is 0, for testing purposes, the MC Ratio 
 * RatioType are set like all the 
 *         others.
 *       * If PCODE_MC_BOOT_CONTROL is 1, the MC Ratio RatioType are set w.r.t. 
 * 33MHz. Other BGF sync pulse 
 *         frequencies are not accomodated.
 *       
 */
typedef union {
  struct {
    UINT32 bgfoverride : 1;
    /* bgfoverride - Bits[0:0], RW_L, default = 1'd0 
       
               BGF Override (BgfOverride):
               When set, forces BgfRun to remain high when PMA deasserts BGF run. It
               overrides the PMA BGFrun signal.
               
     */
    UINT32 uratio : 8;
    /* uratio - Bits[8:1], RW_L, default = 8'd0 
       
               Uclock to BGF Sync pulse frequency Ratio (Uratio):
               This is Uclock to 33Mhz Bgf sync pulse frequency ratio. Uclock ratio at
               which tuning parameters take effect.
               
     */
    UINT32 ptrdist : 3;
    /* ptrdist - Bits[11:9], RW_L, default = 3'd0 
       
               Uclock vs Dclock Pointer Separation Distance (PtrDist):
               Value to set the U-D pointer separation distance to. Tune U to D
               pointer distance and overrides parameter from PCU when Uratio matches.
               
     */
    UINT32 cmdbubbleinit : 9;
    /* cmdbubbleinit - Bits[20:12], RW_L, default = 9'd0 
       
               Command Bubble Generator Initial Value (CmdBubbleInit):
               Tune Command BGF bubble generator initial value.
               Overrides default initial value when Uratio matches.
               
     */
    UINT32 bubbleinit : 9;
    /* bubbleinit - Bits[29:21], RW_L, default = 9'd0 
       
               BGF Bubble Generator Initial Value (BubbleInit):
               Tune bubble generator initial value for update BGF and data BGF bubble
               generator. It overrides the default initial value when Uratio matches.
       
               Contains the ratio signal bits of the initial value of BGF bubble 
       generator counter 
               value. It is the U clock bubble generator value to communicate to Dclock
               domain (under U>D condition). The initial counter value is set up by 
       pCode. 
               The width of the signals must be properly set to handle the arithmetic 
       requirement. 
               
     */
    UINT32 uratiomatchsts : 1;
    /* uratiomatchsts - Bits[30:30], RWS_LV, default = 1'd0 
       
               Uratio Match Event Status (UratioMatchSts):
               Records whether a Uratio match event occured for debug and performance 
       tuning. 
                 1: Uratio match event
                 0: Uratio does not match
               
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BGFTUNING_M2MEM_MAIN_STRUCT;


/* PXPENHCAP_M2MEM_MAIN_REG supported on:                                     */
/*       SKX_A0 (0x40240100)                                                  */
/*       SKX (0x40240100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_M2MEM_MAIN_REG 0x06004100
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
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
} PXPENHCAP_M2MEM_MAIN_STRUCT;


/* TIMEOUT_M2MEM_MAIN_REG supported on:                                       */
/*       SKX_A0 (0x40240104)                                                  */
/*       SKX (0x40240104)                                                     */
/* Register default value:              0x0000002C                            */
#define TIMEOUT_M2MEM_MAIN_REG 0x06004104
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Allows setting of time-out values. The timeout thresholds are set here by 
 * BIOS, and if a timeout is detected they will be reported in the MCA MSRs to the 
 * OS and the MCA shadow registers to OS. 
 *       It is suggested to set the time-out values to the highest possible values 
 * while respecting the time-out hierarchy:  
 *         core 3-strike time-out > Caching Agent CHA > Home Agent CHA > Memory 
 * Controller 
 *       As a rule of thumb, it is suggested to scale the absolute time-out values 
 * with a factor of 2 when moving by a level through the hierarchy.  
 *       Highest possible values are suggested to prevent false time-outs from 
 * happening in extreme conditions like extremely high error rates or compound 
 * error cases (e.g. Intel UPI and MC errors) in combination with very high memory 
 * bandwidths. 
 *       
 */
typedef union {
  struct {
    UINT32 timeouten : 1;
    /* timeouten - Bits[0:0], RW_L, default = 1'h0 
       
               Timeout Enable bit (TimeoutEn):
                 1: Enabled;
                 0: Timeouts are disabled.
               When time-out detection is disabled, it also implies that timeout 
       signaling (MCA/SMI/CMCI)  
               is disabled including its usage as a debug event/trigger.
               
     */
    UINT32 timeoutlock : 1;
    /* timeoutlock - Bits[1:1], RW_L, default = 1'h0 
       
               Timeout Lock bit (TimeoutLock):
               BIOS lock bit for the Timeout Base Unit and Timeout Multiplier.
                 1 = Timeout values are locked.
               
     */
    UINT32 timeoutbase : 2;
    /* timeoutbase - Bits[3:2], RW_L, default = 2'h3 
       
               Tracker Timeout Base Unit (TimeoutBase):
                0: Time base is 2^4=32 uncore cycles. A tracker
                   that exceeds (TimeoutMult+1)*2^4 cycles will timeout, but may not be 
       detected until 
                   (TimeoutMult+1)*2^(4+1) cycles later. Max programmable time-out of 
       33K uncore cycles. 
                1: Time base is 2^12=8K uncore cycles. A tracker
                   that exceeds (TimeoutMult+1)*2^12 cycles will timeout, but may not 
       be detected until 
                   (TimeoutMult+1)*2^(12+1) cycles later. Max programmable time-out of 
       8.4M uncore cycles). 
                2: Time base is 2^20=2.1M uncore cycles. A tracker 
                   that exceeds (TimeoutMult+1)*2^20 cycles will timeout, but may not 
       be detected until 
                   (TimeoutMult+1)*2^(20+1) cycles later. Max programmable time-out of 
       2.1G uncore cycles. 
                3: Time base is 2^28=537M uncore cycles. A tracker 
                   that exceeds (TimeoutMult+1)*2^28 cycles will timeout, but may not 
       be detected until 
                   (TimeoutMult+1)*2^(28+1) cycles later. Max programmable time-out of 
       549G uncore cycles. 
               
     */
    UINT32 timeoutmult : 9;
    /* timeoutmult - Bits[12:4], RW_L, default = 9'h2 
       
               Timeout Multiplier (TimeoutMult):
               The absolute time-out value (in seconds) will be: 
                 Multiplier x Base (in cycles) / mesh frequency (in Hz).
               There is variability in this absolute time because of dynamic 
               frequency changes (Turbo). Because of implementation details 
               there is also an uncertainty of plus a Base Unit on the 
               transaction that times out. All transactions timing out within 
               this Base Unit interval are treated as timing out at the same time 
               and only one of them will be reported (the one with the 
               lowest tracker index). The equation above presents the lower bound 
               of the time-out.
               
     */
    UINT32 rsvd : 19;
    /* rsvd - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TIMEOUT_M2MEM_MAIN_STRUCT;


/* DFXBGF0CTL_M2MEM_MAIN_REG supported on:                                    */
/*       SKX_A0 (0x40240108)                                                  */
/*       SKX (0x40240108)                                                     */
/* Register default value on SKX_A0:    0x00000010                            */
/* Register default value on SKX:       0x00000012                            */
#define DFXBGF0CTL_M2MEM_MAIN_REG 0x06004108
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Control for the DFX-BGF0.
 *       In parallel with the functional BGFs there is also a BGF, the DFX-BGF 
 * through which signatures are sent to detect BGF underflow/overflow. 
 *       
 */
typedef union {
  struct {
    UINT32 bgfstopovrd : 1;
    /* bgfstopovrd - Bits[0:0], RWS_LB, default = 1'h0 
       
               BGF Stop Override (BgfStopOvrd):
               The DFx BGF is primarily controlled by the BgfRun signal of primary, 
       functional BGFs in DFx BGF's neighborhood. Setting this field to 1, however, 
       forces the DFx BGF to stop. While the BGF is stopped, the pattern generator, 
       pattern checker and error counter are not running. When the BGF is restarted 
       (i.e. when this field is cleared to 0), the error counter (i.e. the ErrorCount 
       field) is reset. 
               If the BGF's configuration is changed while it is running, spurious 
       errors will be logged. To change configuration (pattern mode, read delay, read 
       back borrow, pointer separation offset), the standard procedure is to stop the 
       BGF (i.e. set this field 1), change the configuration, and restart the BGF (set 
       this field back to 0). All the other writable fields should be sticky through 
       reset. This field is the exception; it should clear during reset. 
               
     */
    UINT32 patternmode : 1;
    /* patternmode - Bits[1:1], RWS_LB, default = 1'h1 
       
               Pattern Mode (PatternMode):
               This field controls the type of test pattern that runs through the DFx 
       FIFO: 0 = toggle patter, 1 = pseudo-random bit sequence. (This setting should 
       not be changed while the BGF is running. See BgfStopOvrd.) Path from register to 
       pattern generator and checker may be multi-cycle. 
               
     */
    UINT32 enrddelay : 1;
    /* enrddelay - Bits[2:2], RWS_LB, default = 1'h0 
       
               Enable Read Delay Full Cycle (EnRdDelay):
               While this field (and EnRdBkBrw) is set to 0, BGF read timing follows 
       standard BGF behavior. If this field is set to 1, all reads are delayed by one 
       cycle. This setting may be combined with EnRdBkBrw.  (This setting should not be 
       changed while the BGF is running. See BgfStopOvrd.)       Path from register to 
       pattern checker may be multi-cycle. 
               
     */
    UINT32 enrdbkbrw : 1;
    /* enrdbkbrw - Bits[3:3], RWS_LB, default = 1'h0 
       
               Enable Read Delay Fine (EnRdBkBrw):
               While this field (and EnRdDelay) is set to 0, BGF read timing follows 
       standard BGF behavior. If this field is set to 1, read data is sampled half a 
       read clock earlier. This setting may be combined with EnRdDelay. (This setting 
       should not be changed while the BGF is running. See BgfStopOvrd.)    Path from 
       register to pattern checker may be multi-cycle. 
               
     */
    UINT32 ptrsepoffset : 3;
    /* ptrsepoffset - Bits[6:4], RWS_LB, default = 3'b001 
       
               Pointer Separation Offset (PtrSepOffset):
               This field specifies the offset between the pointer separation of nearby 
       primary, functional BGFs to the pointer separation of the DFx BGF. The DFx BGF's 
       pointer separation is the primary BGFs pointer separation minus this offset. 
       (This setting should not be changed while the BGF is running. See BgfStopOvrd.) 
       Path from register to DFx BGF's pointer separation calculation logic may be 
       multi-cycle. 
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ptrsepeff : 3;
    /* ptrsepeff - Bits[10:8], RO_V, default = 3'b000 
       
               Effective Pointer Separation (PtrSepEff):
               This field shows the effective pointer separation applied to the DFx BGF 
       (i.e. it is the primary BGF pointer separation minus PtrSepOffset). Note that 
       this value is actually the program value; the conceptual pointer separation is 
       the program value plus two. Shouldn't need any flip-flops at register. Value may 
       flow through from logic that computes the effective pointer separation. Path to 
       register may be multi-cycle. 
               
     */
    UINT32 rsvd_11 : 1;
    /* rsvd_11 - Bits[11:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_12 : 20;
  } Bits;
  UINT32 Data;
} DFXBGF0CTL_M2MEM_MAIN_STRUCT;


/* M2MBIOSQUIESCE_M2MEM_MAIN_REG supported on:                                */
/*       SKX_A0 (0x4024010C)                                                  */
/*       SKX (0x4024010C)                                                     */
/* Register default value:              0x00000040                            */
#define M2MBIOSQUIESCE_M2MEM_MAIN_REG 0x0600410C
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       This register supports a Lite Quiesce, i.e. a local temporary quiesce of 
 * the Mesh2mem transactions to its connected memory controllers. 
 * 
 *       The required usage sequence is a follows:
 *          1. BIOS writes ForceQuiesceM2m = 1,
 *          2. BIOS waits for IsQuiesced==1,
 *          3. BIOS executes whatever sequence of operations it needs to execute 
 * under this mesh2mem quiesce, 
 *          4. BIOS writes ForceQuiesceM2m = 0.
 *       During all these steps, it is forbidden for BIOS to access memory (even 
 * for fetching code) as this could lead to deadlock. 
 *       Also blocking for a very long time might lead to time-outs, so if a 
 * copious number of operations needs to be performed during step 3 then BIOS will 
 * have to time slice these operations and execute above sequence for each time 
 * slice. 
 *       It should be noted that only mesh2mem traffic to its connected memory 
 * controllers gets blocked. Patrol scrubs, rank sparing and memory maintenance 
 * commands (e.g. refreshed, closing pages) can still take place during this 
 * quiesce as they originate in MC. The heads of the ingress queues get blocked so 
 * transactions can still enter mesh2mem ingress till ingress fills up (and 
 * prefetches from core/Intel UPI can enter prefetchCAM till it fills up). 
 * 
 *       Typical usage model for this mechanism is when some feature needs to get 
 * dynamically switched on/off in M2M or MC. It will be used for ADDDC, +1 sparing 
 * and rank sparing flows. 
 * 
 *       
 */
typedef union {
  struct {
    UINT32 forcequiescem2m : 1;
    /* forcequiescem2m - Bits[0:0], RWS_LB, default = 1'b0 
       
               Force Quiesce in Mesh2mem control bit (ForceQuiesceM2m):
                 0: Do not quiesce M2m/Memory-controllers (default).
                 1: Force quiesce of this mesh2mem and its connected memory 
       controllers. 
                    Transactions from mesh2mem to its connected memory controllers will 
       get  
                    blocked in mesh2mem till this bit is 0.
               Note, forcing quiesce for too long a time continuously could result in 
       time-outs in the system (e.g. CHA time-outs or core 3-strike).  
               So if a copious amount of operations are required during quiesce then 
       BIOS time-sliced quiescing might become necessary. 
               
     */
    UINT32 isquiesced : 1;
    /* isquiesced - Bits[1:1], RWS_V, default = 1'b0 
       
               Is Quiesced status bit (IsQuiesced0):
                 0: Quiesce for this mesh2mem or its connected memory controllers has 
       not been reached yet. 
                 1: Quiesce for this mesh2mem and its connected memory controllers has 
       been reached. 
                    That means Egress, Transgress, Score-boards, Read Pending Queues 
       and Write Pending Queues are empty. 
                    It should be noted that only mesh2mem commands are being blocked. 
       So mc originating transactions  
                    like patrol reads or rank sparing or dimm maintenance traffic (e.g. 
       refreshes) might still take place. 
               
     */
    UINT32 drainrttimer : 14;
    /* drainrttimer - Bits[15:2], RWS_LB, default = 14'd16 
       
               MC round-trip-fifo drainage time (DrainRTtimer):
               Number of 32 TSVs (Time-Slot-Valids, i.e. slowest clock of M2M/MC) to 
       wait after MC RPQ/WPQ  
               and M2M transgress empty till asserting IsQuiesced0. This to guarantee 
       no in-flight data/acks  
               from the memory controller when IsQuiesced0 gets signalled.
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M2MBIOSQUIESCE_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register supports a Lite Quiesce, i.e. a local temporary quiesce of 
 * the Mesh2mem transactions to its connected memory controllers. 
 * 
 *       The required usage sequence is a follows. BIOS asks the pcode to perform 
 * following: 
 *          1. Pcode writes ForceQuiesceM2m = 1,
 *          2. Pcode waits for IsQuiesced==1,
 *          3. Pcode executes whatever sequence of operations it needs to execute 
 * under this mesh2mem quiesce, 
 *          4. Pcode writes ForceQuiesceM2m = 0.
 *       Pcode needs to perform these steps because the BIOS path to the CSR 
 * unquiesce (writing ForceQuiesceM2m=0) might be unavailable under Lite Quiesce. 
 * DMA will still send traffic into M2M and hence M2M ingressQ could fill up, back 
 * pressuring mesh and CHA. 
 * 
 *       Also blocking for a very long time might lead to time-outs, so if a 
 * copious number of operations needs to be performed during step 3 then BIOS will 
 * have to time slice these operations and execute above sequence for each time 
 * slice. 
 *       It should be noted that only mesh2mem traffic to its connected memory 
 * controllers gets blocked. Patrol scrubs, rank sparing and memory maintenance 
 * commands (e.g. refreshed, closing pages) can still take place during this 
 * quiesce as they originate in MC. The heads of the ingress queues get blocked so 
 * transactions can still enter mesh2mem ingress till ingress fills up (and 
 * prefetches from core/Intel UPI can enter prefetchCAM till it fills up). 
 * 
 *       Typical usage model for this mechanism is when some feature needs to get 
 * dynamically switched on/off in M2M or MC. It will be used for ADDDC, +1 sparing 
 * and rank sparing flows. 
 * 
 *       
 */
typedef union {
  struct {
    UINT32 forcequiescem2m : 1;
    /* forcequiescem2m - Bits[0:0], RW_LB, default = 1'b0 
       
               Force Quiesce in Mesh2mem control bit (ForceQuiesceM2m):
                 0: Do not quiesce M2m/Memory-controllers (default).
                 1: Force quiesce of this mesh2mem and its connected memory 
       controllers. 
                    Transactions from mesh2mem to its connected memory controllers will 
       get  
                    blocked in mesh2mem till this bit is 0.
               Note, forcing quiesce for too long a time continuously could result in 
       time-outs in the system (e.g. CHA time-outs or core 3-strike).  
               So if a copious amount of operations are required during quiesce then 
       BIOS time-sliced quiescing might become necessary. 
               
     */
    UINT32 isquiesced : 1;
    /* isquiesced - Bits[1:1], RW_V, default = 1'b0 
       
               Is Quiesced status bit (IsQuiesced0):
                 0: Quiesce for this mesh2mem or its connected memory controllers has 
       not been reached yet. 
                 1: Quiesce for this mesh2mem and its connected memory controllers has 
       been reached. 
                    That means Egress, Transgress, Score-boards, Read Pending Queues 
       and Write Pending Queues are empty. 
                    It should be noted that only mesh2mem commands are being blocked. 
       So mc originating transactions  
                    like patrol reads or rank sparing or dimm maintenance traffic (e.g. 
       refreshes) might still take place. 
               
     */
    UINT32 drainrttimer : 14;
    /* drainrttimer - Bits[15:2], RW_LB, default = 14'd16 
       
               MC round-trip-fifo drainage time (DrainRTtimer):
               Number of 32 TSVs (Time-Slot-Valids, i.e. slowest clock of M2M/MC) to 
       wait after MC RPQ/WPQ  
               and M2M transgress empty till asserting IsQuiesced0. This to guarantee 
       no in-flight data/acks  
               from the memory controller when IsQuiesced0 gets signalled.
               
     */
    UINT32 ncchunkerroroutstanding : 1;
    /* ncchunkerroroutstanding - Bits[16:16], RO_V, default = 1'd0 
       
               Error on Non-Crital Chunk Outstanding (NcChunkErrorOutstanding):
               An error was observed on a non-critical chunk while no error on the 
       critical chunk, and the transaction is still outstanding, i.e. not handled yet 
       by error flow. 
               When this bit is set, it is unsafe to set 
       Defeatures0[Wait4BothHalves]=1. If NcChunkErrorOutstanding is set while 
       M2mBiosQuiesce[IsQuiesced] is set and one wants to write 
       Defeatures0[Wait4BothHalves]=1 under quiesce, then one needs to get out of 
       quiesce (by M2mBiosQuiesce[ForceQuiesceM2m]=0) and try quiescing again till on 
       succeeds, i.e. find a condition where NcChunkErrorOutstanding==1 while 
       IsQuiesced is asserted.  
               NcChunkErrorOutstanding is a paranoia handle and there is presently no 
       known scenario where we need this, as any such scenario is presently handled by 
       hardware. Only when defeature bit IgnoreEgrNcErrOnQuiesce is set, i.e. the 
       hardware flow for handling this case is disabled, then this retry based 
       ForceQuiesceM2m flow becomes relevant (for M2M/MC mini-quiesce flows). 
               
     */
    UINT32 rsvd : 15;
    /* rsvd - Bits[31:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M2MBIOSQUIESCE_M2MEM_MAIN_STRUCT;



/* M2MDFXQUIESCE_M2MEM_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x40240110)                                                  */
/*       SKX (0x40240110)                                                     */
/* Register default value:              0x00100000                            */
#define M2MDFXQUIESCE_M2MEM_MAIN_REG 0x06004110
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register supports a Lite Quiesce, i.e. a local temporary quiesce of 
 * the Mesh2mem transactions to its connected memory controllers. 
 * 
 *       Typical usage model for this mechanism is when some feature needs to get 
 * dynamically switched on/off, or to induce stress or to break a deadlock case. 
 * 
 *       There are 3 independent groups of quiesce knobs available here (besides 
 * the BIOS knob in CSR M2mBiosQuiesce). The intention is that following usage 
 * model gets enforced: 
 *         - Triple (ForceQuiesce1M2m, WaitForDrainTimer1, IsQuiesced1) is 
 * available for ucode/pcode (Intel reserved). 
 *         - Triple (ForceQuiesce2M2m, WaitForDrainTimer2, IsQuiesced2) is 
 * available for DFD: e.g. volume exercising of defeatures/features/stress knobs 
 * (Intel reserved). 
 *         - Triple (ForceQuiesceOnLocRspRise, WaitForDrainTimerWhenLocRspRise, 
 * IsQuiescedFromLocRsp) is available for local response control 
 * LocalRspCntl1[FrcQuiesce (Intel reserved). 
 * 
 *       ForceQuiesce will result in the M2M/MC interface getting quiesced and MC 
 * structures (RPQ/WPQ/Score-board/RT-fifo) getting drained. Once that happened and 
 * WaitForDrainTimer is set then 'DrainTimer' TSVs (slowest(uncore-clk, mc-clk) 
 * pulses) need to be observed before IsQuiesced gets asserted. The M2M/MC 
 * interface will get blocked till ForceQuiesce becomes 0 again (except for the 
 * ForceQuiesceOnLocRspRise case). 
 * 
 *       
 */
typedef union {
  struct {
    UINT32 forcequiesce1m2m : 1;
    /* forcequiesce1m2m - Bits[0:0], RW_LB, default = 1'b0 
       
               Force Quiesce in Mesh2mem control bit (ForceQuiesce1M2m):
                 0: Do not quiesce M2m/Memory-controllers (default).
                 1: Force quiesce of this mesh2mem and its connected memory 
       controllers. 
                    Transactions from mesh2mem to its connected memory controllers will 
       get  
                    blocked in mesh2mem till this bit is 0.
               Note, forcing quiesce for too long a time continuously could result in 
       time-outs in the system (e.g. CHA time-outs or core 3-strike).  
               So if a copious amount of operations are required during quiesce then 
       BIOS time-sliced quiescing might become necessary. 
               
     */
    UINT32 waitfordraintimer1 : 1;
    /* waitfordraintimer1 - Bits[1:1], RWS_LB, default = 1'b0 
       
               Wait for drain timer (WaitForDrainTimer1):
                 0: Don't wait for drain timer, declare IsQuiesed as soon as drained.
                 1: After reaching drained state, wait extra time specified by 
       DrainTimer before declaring IsQuiesced. 
               
     */
    UINT32 isquiesced1 : 1;
    /* isquiesced1 - Bits[2:2], RW_V, default = 1'b0 
       
               Is Quiesced status bit (IsQuiesced1):
                 0: Quiesce for this mesh2mem or its connected memory controllers has 
       not been reached yet. 
                 1: Quiesce for this mesh2mem and its connected memory controllers has 
       been reached. 
                    That means Egress, Transgress, Score-boards, Read Pending Queues 
       and Write Pending Queues are empty. 
                    It should be noted that only mesh2mem commands are being blocked. 
       So mc originating transactions  
                    like patrol reads or rank sparing or dimm maintenance traffic (e.g. 
       refreshes) might still take place. 
                    In case of WaitForDrainTimer1 set, after draining, an extra time 
       specified by DrainTimer is  
                    enforced before asserting IsQuiesced1.
               
     */
    UINT32 rsvd_3 : 5;
    /* rsvd_3 - Bits[7:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 forcequiesce2m2m : 1;
    /* forcequiesce2m2m - Bits[8:8], RW_LB, default = 1'b0 
       
               Force Quiesce in Mesh2mem control bit (ForceQuiesce2M2m) set by NoRetry 
       logic for patrol deadlock: 
                 0: Do not quiesce M2m/Memory-controllers (default).
                 1: Force quiesce of this mesh2mem and its connected memory 
       controllers. 
                    Transactions from mesh2mem to its connected memory controllers will 
       get  
                    blocked in mesh2mem till this bit is 0.
               Note, forcing quiesce for too long a time continuously could result in 
       time-outs in the system (e.g. CHA time-outs or core 3-strike).  
               So if a copious amount of operations are required during quiesce then 
       BIOS time-sliced quiescing might become necessary. 
               
     */
    UINT32 waitfordraintimer2 : 1;
    /* waitfordraintimer2 - Bits[9:9], RW_LB, default = 1'b0 
       
               Wait for drain timer (WaitForDrainTimer2):
                 0: Don't wait for drain timer, declare IsQuiesed as soon as drained.
                 1: After reaching drained state, wait extra time specified by 
       DrainTimer before declaring IsQuiesced. 
               
     */
    UINT32 isquiesced2 : 1;
    /* isquiesced2 - Bits[10:10], RW_V, default = 1'b0 
       
               Is Quiesced status bit (IsQuiesced2):
                 0: Quiesce for this mesh2mem or its connected memory controllers has 
       not been reached yet. 
                 1: Quiesce for this mesh2mem and its connected memory controllers has 
       been reached. 
                    That means Egress, Transgress, Score-boards, Read Pending Queues 
       and Write Pending Queues are empty. 
                    It should be noted that only mesh2mem commands are being blocked. 
       So mc originating transactions  
                    like patrol reads or rank sparing or dimm maintenance traffic (e.g. 
       refreshes) might still take place. 
                    In case of WaitForDrainTimer2 set, after draining, an extra time 
       specified by DrainTimer is  
                    enforced before asserting IsQuiesced2.
               
     */
    UINT32 rsvd_11 : 5;
    /* rsvd_11 - Bits[15:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 forcequiesceonlocrsprise : 1;
    /* forcequiesceonlocrsprise - Bits[16:16], RW_L, default = 1'b0 
       
               Force Quiesce in Mesh2mem on rising edge of local response FrcQuiesce 
       (ForceQuiesceOnLocRspRise): 
                 0: Use level of FrcQuiesce local response signal to force quiesce 
       (default). 
                 1: Use rising edge of FrcQuiesce local response signal to force 
       quiesce (till IsQuiescedFromLocRsp asserts). 
               This bit only selects whether to look at signal level or rising edge of 
       signal. It will have no effect as long as  
               LocalRspCntl1[FrcQuiesce] is not set.
               
     */
    UINT32 waitfordraintimerwhenlocrsprise : 1;
    /* waitfordraintimerwhenlocrsprise - Bits[17:17], RWS_LB, default = 1'b0 
       
               Wait for drain timer (WaitForDrainTimer1):
                 0: Don't wait for drain timer, declare IsQuiesed as soon as drained.
                 1: After reaching drained state, wait extra time specified by 
       DrainTimer before declaring IsQuiesced. 
               
     */
    UINT32 isquiescedfromlocrsp : 1;
    /* isquiescedfromlocrsp - Bits[18:18], RW_V, default = 1'b0 
       
               Is Quiesced status bit (IsQuiesced3):
                 0: Quiesce for this mesh2mem or its connected memory controllers has 
       not been reached yet. 
                 1: Quiesce for this mesh2mem and its connected memory controllers has 
       been reached. 
                    That means Egress, Transgress, Score-boards, Read Pending Queues 
       and Write Pending Queues are empty. 
                    It should be noted that only mesh2mem commands are being blocked. 
       So mc originating transactions  
                    like patrol reads or rank sparing or dimm maintenance traffic (e.g. 
       refreshes) might still take place. 
                    In case of WaitForDrainTimerWhenLocRspRise set, after draining, an 
       extra time specified by DrainTimer is  
                    enforced before asserting IsQuiescedFromLocRsp.
               
     */
    UINT32 draintimer : 13;
    /* draintimer - Bits[31:19], RW_L, default = 13'h2 
       
               Extra drainage time (DrainTimer):
               Number of 32 TSVs (Time-Slot-Valid, i.e. slowest clock of M2M/MC) to 
       wait after MC RPQ/WPQ  
               and M2M transgress empty till asserting IsQuiesced1/2. This to guarantee 
       no in-flight data/acks  
               from the memory controller when IsQuiesced1/2 gets signalled.
               
     */
  } Bits;
  UINT32 Data;
} M2MDFXQUIESCE_M2MEM_MAIN_STRUCT;


/* EXRAS_CONFIG_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x4024011C)                                                  */
/*       SKX (0x4024011C)                                                     */
/* Register default value on SKX_A0:    0x00001210                            */
/* Register default value on SKX:       0x00007210                            */
#define EXRAS_CONFIG_M2MEM_MAIN_REG 0x0600411C
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * Reg descript.
 */
typedef union {
  struct {
    UINT32 cfgmcasmioncorr : 1;
    /* cfgmcasmioncorr - Bits[0:0], RWS_L, default = 1'b0 
       Configures SMI signaling for memory correctable error (retry corrected or 
       ECC/SDDC/DDDC corrected) . 
                              0: No SMI (default).
                              1: Enable SMI signaling.
               
     */
    UINT32 cfgmcacmcioncorr : 1;
    /* cfgmcacmcioncorr - Bits[1:1], RWS_L, default = 1'b0 
       Configures CMCI signaling for memory correctable error (retry corrected or 
       ECC/SDDC/DDDC corrected) . 
                              0: No CMCI (default).
                              1: Enable CMCI signaling.
               
     */
    UINT32 rsvd_2 : 1;
    /* rsvd_2 - Bits[2:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cfgmcasmioncorrcountthr : 1;
    /* cfgmcasmioncorrcountthr - Bits[3:3], RWS_L, default = 1'b0 
       Configures SMI signaling of correctable error count reaching threshold.
                              0: No SMI (default).
                              1: Enable SMI signaling.
               
     */
    UINT32 cfgmcacmcioncorrcountthr : 1;
    /* cfgmcacmcioncorrcountthr - Bits[4:4], RWS_L, default = 1'b1 
       Configures CMCI signaling of correctable error count reaching threshold.
                              0: No CMCI (default).
                              1: Enable CMCI signaling.
               
     */
    UINT32 cfgmcasmionmirrorcorr : 1;
    /* cfgmcasmionmirrorcorr - Bits[5:5], RWS_L, default = 1'b0 
       Configures SMI signaling for channel mirror-correctable error.
                              0: No SMI (default).
                              1: Enable SMI signaling.
                            A mirror-correctable error is an error on a mirror channel 
       that is corrected  
                            by fetching data from the alternate mirror channel followed 
       by a succesful write back to and  
                            reread from the erroneous channel.
               
     */
    UINT32 cfgmcacmcionmirrorcorr : 1;
    /* cfgmcacmcionmirrorcorr - Bits[6:6], RWS_L, default = 1'b0 
       Configures CMCI signaling for channel mirror-correctable error.
                              0: No CMCI (default).
                              1: Enable CMCI signaling.
                            A mirror-correctable error is an error on a mirror channel 
       that is corrected  
                            by fetching data from the alternate mirror channel followed 
       by a succesful write back to and  
                            reread from the erroneous channel.
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cfgmcasmionfailover : 1;
    /* cfgmcasmionfailover - Bits[8:8], RWS_L, default = 1'b0 
       Configures SMI signaling for mirror scrub reread failure.
                              0: No SMI (default).
                              1: Enable SMI signaling.
                            Note, if SysFeatures0[ImmediateFailoverActionEna] is set 
       then hardware  
                            will take care that a mirror scub reread fail will result 
       in a permanent channel failover. 
                            Otherwise, it is BIOS responsibility to interject permanent 
       channel failover. 
               
     */
    UINT32 cfgmcacmcionfailover : 1;
    /* cfgmcacmcionfailover - Bits[9:9], RWS_L, default = 1'b1 
       Configures CMCI signaling for mirror scrub reread failure.
                              0: No CMCI (default).
                              1: Enable CMCI signaling.
                            Note, if SysFeatures0[ImmediateFailoverActionEna] is set 
       then hardware  
                            will take care that a mirror scub reread fail will result 
       in a permanent channel failover. 
                            Otherwise, it is BIOS responsibility to interject permanent 
       channel failover. 
               
     */
    UINT32 rsvd_10 : 1;
    /* rsvd_10 - Bits[10:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cfgmcasmionrecov : 1;
    /* cfgmcasmionrecov - Bits[11:11], RWS_L, default = 1'b0 
       Configures SMI signaling for recoverable error.
                              0: No SMI (default).
                              1: Enable SMI signaling.
               
     */
    UINT32 cfgmcacmcionrecov : 1;
    /* cfgmcacmcionrecov - Bits[12:12], RWS_L, default = 1'b1 
       Recoverable error (UC=1,PCC=0) generates a CMCI type interrupt.
                              0: No interrupt;
                              1: Enable interrupt generation (default).
               
     */
    UINT32 rsvd_13 : 17;
    /* rsvd_13 - Bits[29:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 corr2corroverwritedis : 1;
    /* corr2corroverwritedis - Bits[30:30], RWS_L, default = 1'b0 
       
               Correctable-correctable Overwrite Disable (Corr2CorrOverwriteDis):
                 0: Correctable overwrites correctable in Machine Check bank and shadow 
       register (default). 
                 1: First correctable logged in Machine Check bank and also the shadow 
       register will not  
                    be overwritten by another correctable error.
               If this bit is set, the intention is that 
                 (i) also iMC retry_rd_err_log[NO_OVERRIDE] should be set so that the 
       iMC error logging  
                     of correctables in retry_rd_err_log, is aligned with the M2M error 
       log, and 
                 (ii) also iMC's dp_chkn_bit[ign_mca_ovrd] should be set so that iMC 
       error logging of  
                     correctables in the iMC machine check banks is aligned with the 
       M2M error logging. 
               It should be noted that the M2M and iMC error logging can still get out 
       of sync in case of  
               iMC patrol scrub (or iMC rank sparing) errors because M2M has no 
       observability into these  
               error types.
               
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} EXRAS_CONFIG_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * Reg descript.
 */
typedef union {
  struct {
    UINT32 cfgmcasmioncorr : 1;
    /* cfgmcasmioncorr - Bits[0:0], RWS_L, default = 1'b0 
       Configures SMI signaling for memory correctable error (retry corrected or 
       ECC/SDDC/DDDC corrected) . 
                              0: No SMI (default).
                              1: Enable SMI signaling.
               
     */
    UINT32 cfgmcacmcioncorr : 1;
    /* cfgmcacmcioncorr - Bits[1:1], RWS_L, default = 1'b0 
       Configures CMCI signaling for memory correctable error (retry corrected or 
       ECC/SDDC/DDDC corrected) . 
                              0: No CMCI (default).
                              1: Enable CMCI signaling.
               
     */
    UINT32 rsvd_2 : 1;
    /* rsvd_2 - Bits[2:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cfgmcasmioncorrcountthr : 1;
    /* cfgmcasmioncorrcountthr - Bits[3:3], RWS_L, default = 1'b0 
       Configures SMI signaling of correctable error count reaching threshold.
                              0: No SMI (default).
                              1: Enable SMI signaling.
               
     */
    UINT32 cfgmcacmcioncorrcountthr : 1;
    /* cfgmcacmcioncorrcountthr - Bits[4:4], RWS_L, default = 1'b1 
       Configures CMCI signaling of correctable error count reaching threshold.
                              0: No CMCI (default).
                              1: Enable CMCI signaling.
               
     */
    UINT32 cfgmcasmionmirrorcorr : 1;
    /* cfgmcasmionmirrorcorr - Bits[5:5], RWS_L, default = 1'b0 
       Configures SMI signaling for channel mirror-correctable error.
                              0: No SMI (default).
                              1: Enable SMI signaling.
                            A mirror-correctable error is an error on a mirror channel 
       that is corrected  
                            by fetching data from the alternate mirror channel followed 
       by a succesful write back to and  
                            reread from the erroneous channel.
               
     */
    UINT32 cfgmcacmcionmirrorcorr : 1;
    /* cfgmcacmcionmirrorcorr - Bits[6:6], RWS_L, default = 1'b0 
       Configures CMCI signaling for channel mirror-correctable error.
                              0: No CMCI (default).
                              1: Enable CMCI signaling.
                            A mirror-correctable error is an error on a mirror channel 
       that is corrected  
                            by fetching data from the alternate mirror channel followed 
       by a succesful write back to and  
                            reread from the erroneous channel.
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cfgmcasmionfailover : 1;
    /* cfgmcasmionfailover - Bits[8:8], RWS_L, default = 1'b0 
       Configures SMI signaling for mirror scrub reread failure.
                              0: No SMI (default).
                              1: Enable SMI signaling.
                            Note, if SysFeatures0[ImmediateFailoverActionEna] is set 
       then hardware  
                            will take care that a mirror scub reread fail will result 
       in a permanent channel failover. 
                            Otherwise, it is BIOS responsibility to interject permanent 
       channel failover. 
               
     */
    UINT32 cfgmcacmcionfailover : 1;
    /* cfgmcacmcionfailover - Bits[9:9], RWS_L, default = 1'b1 
       Configures CMCI signaling for mirror scrub reread failure.
                              0: No CMCI (default).
                              1: Enable CMCI signaling.
                            Note, if SysFeatures0[ImmediateFailoverActionEna] is set 
       then hardware  
                            will take care that a mirror scub reread fail will result 
       in a permanent channel failover. 
                            Otherwise, it is BIOS responsibility to interject permanent 
       channel failover. 
               
     */
    UINT32 rsvd_10 : 1;
    /* rsvd_10 - Bits[10:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cfgmcasmionrecov : 1;
    /* cfgmcasmionrecov - Bits[11:11], RWS_L, default = 1'b0 
       Configures SMI signaling for recoverable error.
                              0: No SMI (default).
                              1: Enable SMI signaling.
               
     */
    UINT32 cfgmcacmcionrecov : 1;
    /* cfgmcacmcionrecov - Bits[12:12], RWS_L, default = 1'b1 
       Recoverable error (UC=1,PCC=0) generates a CMCI type interrupt.
                              0: No interrupt;
                              1: Enable interrupt generation (default).
               
     */
    UINT32 cfgmcacorrondatauc : 1;
    /* cfgmcacorrondatauc - Bits[13:13], RWS_L, default = 1'b1 
       Correctable error(UC=0, PCC=0) for data uc errors corrected by FM fill
                              0: Log them as UC=1, PCC=0 
                              1: Log them as UC=0, PCC=0
       	Signalling behavior is synonymous to logging						 
               
     */
    UINT32 cfgmcalogcorronwrmissuc : 1;
    /* cfgmcalogcorronwrmissuc - Bits[14:14], RWS_L, default = 1'b1 
       Correctable error(UC=0, PCC=0) for data uc errors on nm ,not corrected but 
       skipped due to nm miss  
                              0: No Logging 
                              1: Log them as UC=0, PCC=0 and increment counter 
       everytime we skip to FM on miss 
       	Signalling behavior is synonymous to logging						 
               
     */
    UINT32 rsvd_15 : 15;
    /* rsvd_15 - Bits[29:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 corr2corroverwritedis : 1;
    /* corr2corroverwritedis - Bits[30:30], RWS_L, default = 1'b0 
       
               Correctable-correctable Overwrite Disable (Corr2CorrOverwriteDis):
                 0: Correctable overwrites correctable in Machine Check bank and shadow 
       register (default). 
                 1: First correctable logged in Machine Check bank and also the shadow 
       register will not  
                    be overwritten by another correctable error.
               If this bit is set, the intention is that 
                 (i) also iMC retry_rd_err_log[NO_OVERRIDE] should be set so that the 
       iMC error logging  
                     of correctables in retry_rd_err_log, is aligned with the M2M error 
       log, and 
                 (ii) also iMC's dp_chkn_bit[ign_mca_ovrd] should be set so that iMC 
       error logging of  
                     correctables in the iMC machine check banks is aligned with the 
       M2M error logging. 
               It should be noted that the M2M and iMC error logging can still get out 
       of sync in case of  
               iMC patrol scrub (or iMC rank sparing) errors because M2M has no 
       observability into these  
               error types.
               
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} EXRAS_CONFIG_M2MEM_MAIN_STRUCT;



/* MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG supported on:                          */
/*       SKX_A0 (0x40240120)                                                  */
/*       SKX (0x40240120)                                                     */
/* Register default value:              0x00000000                            */
#define MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG 0x06004120
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_STATUS MSR. This shadow register is 
 * under BIOS control while the MCi_STATUS  
 *       MSR is under OS control.
 *       Only the OtherInfo field definition is different from the MCi_STATUS MSR 
 * definition. 
 *       
 */
typedef union {
  struct {
    UINT32 mcacod : 16;
    /* mcacod - Bits[15:0], RWS_V, default = 1'b0 
       
               Specifies the machine check architecture defined error code for the 
       machine 
               check error condition detected:
                 bits[15:0] = 0000 0100 0000 0000: Time-out.
                 bits[15:0] = 0000 0000 0000 0101: Parity error on internal Mesh2mem 
       structures. 
                 bits[15:7] = 0000 0010 0: Near-Memory Cache controller error. Follow 
       below encodings 
                 bits[15:7] = 0000 0000 1: Last level memory controller error. Follow 
       below encodings 
               Bits[6:4] and bits[3:0] further sub-classify the cache/memory controller 
       errors: 
                 bits[6:4] = 000: Generic undefined request.
                 bits[6:4] = 001: Memory Read Error (MemRd*, MemSpecRd*, MemInv*).
                 bits[6:4] = 010: Memory Write Error.
                 bits[6:4] = 011: Address/Command Error (iMC Parity error, iMC BGF 
       error, bucket1 error). 
                 bits[6:4] = 100: Memory Scrubbing Error.
                 bits[6:4] = 101-111: reserved.
               Bits[3:0] expose physical channel information from where the erroneous 
       transaction 
               received error indications:
                 bits[3:0] = Physical Channel Number (0-2). So bits[3:2] always 0 in 
       this implementation. 
                 bits[3:0] = 1111: Channel not specified.
               If both 1st and 2nd levels of memory signaled an error then the channel 
               number logged here is the 1st level channel.
               
     */
    UINT32 mscoddatarderr : 1;
    /* mscoddatarderr - Bits[16:16], RWS_V, default = 1'b0 
       
               MSCOD bit for read error (MscodDataRdErr):
               Specifies a model specific error code that uniquely identifies the 
       machine 
               check error condition that has been detected.
                 0: No MC read data error logged;
                 1: Error logged is an MC read data error.
               In this implementation, 'Read' refers to one of 
                 {MemSpecRd, MemRd, MemRdData, MemRdXtoI/S/A, MemInv*}.
               
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[17:17], RWS_V, default = 1'b0 
       
               This bit is reserved for future use.
               
     */
    UINT32 mscodptlwrerr : 1;
    /* mscodptlwrerr - Bits[18:18], RWS_V, default = 1'b0 
       
               MSCOD bit for partial write error (MscodPtlWrErr):
               Specifies a model specific error code that uniquely identifies the 
       machine 
               check error condition that has been detected.
                 0: No MC partial write data error logged;
                 1: Error logged is an MC partial write data error.
               In this implementation, 'partial write' refers to one of 
                 {MemWrPtl_I/S/A, MemWrPtlNI_I/S/A}.
               The error is detected on the read data of the partial write command.
               
     */
    UINT32 mscodfullwrerr : 1;
    /* mscodfullwrerr - Bits[19:19], RWS_V, default = 1'b0 
       
               MSCOD bit for full write error (MscodFullWrErr):
               Specifies a model specific error code that uniquely identifies the 
       machine 
               check error condition that has been detected.
                 0: No MC full write data error logged;
                 1: Error logged is a full write data error.
               In this implementation, 'full write' refers to one of 
                 {MemWr_I/S/A, MemWrNI_I/S/A, MemWrFlush}.
               In case of MemWrFlush, the ADDRV bit will not get set because there is 
       no relevant  
               physical memory address associated with this command.
               In SKX, this bit will never be set as all full writes are posted to the 
       memory controller channel.  
               Any 'full write' errors would get logged in the memory channel 
       controller Machine Check banks instead of here. 
               
     */
    UINT32 mscodbgferr : 1;
    /* mscodbgferr - Bits[20:20], RWS_V, default = 1'b0 
       
               MSCOD bit for clock domains crossing error (MscodBgfErr):
               Specifies a model specific error code that uniquely identifies the 
       machine 
               check error condition that has been detected.
                 0: No MC cross-clock-domains buffer error logged;
                 1: Logged an M2M clock-domain-crossing buffer (BGF) error.
               This is a fatal error type.
               
     */
    UINT32 mscodtimeout : 1;
    /* mscodtimeout - Bits[21:21], RWS_V, default = 1'b0 
       
               MSCOD bit for time out (MscodTimeout):
               Specifies a model specific error code that uniquely identifies the 
       machine 
               check error condition that has been detected.
                 0: No M2M time-out logged;
                 1: Error logged is an M2M time out.
               This is a fatal error type.
               
     */
    UINT32 mscodparerr : 1;
    /* mscodparerr - Bits[22:22], RWS_V, default = 1'b0 
       
               MSCOD bit for M2M tracker parity error (MscodParErr):
               Specifies a model specific error code that uniquely identifies the 
       machine 
               check error condition that has been detected.
                 0: An M2M tracker parity error logged;
                 1: Error logged is an M2M tracker parity error.
               This is a fatal error type.
               
     */
    UINT32 mscodbucket1err : 1;
    /* mscodbucket1err - Bits[23:23], RWS_V, default = 1'b0 
       
               MSCOD bit for Bucket1 errors (MscodBucket1Err):
               Specifies a model specific error code that uniquely identifies the 
       machine 
               check error condition that has been detected.
                 0: No Bucket1 error logged;
                 1: Error logged is a Bucket1 error.
               This is a fatal error type. Bucket1 contains credit and buffer 
       underflows and overflows. 
               
     */
    UINT32 mscodddrtype : 2;
    /* mscodddrtype - Bits[25:24], RWS_V, default = 2'b0 
       
               MSCOD bits for DDR4/DDRT specific error (MscodDdrType):
               This is an attribute to MscodDataRdErr, MscodPtlWrErr, MscodFullWrErr 
       errors. 
                 00: Not logged whether error to DDR4 or DDRT.
                 01: Error specifically on DDR4.
                 10: Error specifically on DDRT.
                 11: Error for this transaction was detected on both DDR4 and DDRT.
               
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} MCI_STATUS_SHADOW_N0_M2MEM_MAIN_STRUCT;


/* MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG supported on:                          */
/*       SKX_A0 (0x40240124)                                                  */
/*       SKX (0x40240124)                                                     */
/* Register default value:              0x00000000                            */
#define MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG 0x06004124
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_STATUS MSR. This shadow register is 
 * under BIOS control while the MCi_STATUS  
 *       MSR is under OS control.
 *       Only the OtherInfo field definition is different from the MCi_STATUS MSR 
 * definition. 
 *       
 */
typedef union {
  struct {
    UINT32 otherinfo : 6;
    /* otherinfo - Bits[5:0], RWS_V, default = 6'h0 
       
               Other Info (OtherInfo):
               The functions of the bits in this field are implementation specific and 
       are not 
               part of the machine check architecture.
               In this implementation, this bitfield is used to deal with OS-BIOS race 
       cases and  
               logs the previous shadow error type (if the shadow status wasn't cleared 
       by BIOS). 
                 Other_Info[5:4] - Unused (2'b0)
                 Other_Info[3]: Trigger Event for History of Error: OS-BIOS race. 
                   Trigger event is defined as 0 default. Turns to 1 when M2M OS 
       Machine Check bank status register  
                   is being updated (more precisely: valid is being set when it wasn't 
       set before) while the M2M BIOS  
                   Shadow Machine Check register status's valid bit is still on 
       (indicating we have not read the 
                   previous error info from the shadow register yet) -> a.k.a. 
       triggering event. 
                 Other_Info[2]: History of Fatal Error: 0 is default. When triggering 
       event happens,  
                   turns to 1 if the previous error was a fatal error.
                 Other_Info[1]: History of recoverable Error: 0 is default. When 
       triggering event happens,  
                   turns to 1 if the previous error was a recoverable error
                 Other Info[0]: History of Corrected Error: 0 is default. When 
       triggering event happens,  
                   turns to 1 if the previous error was a corrected error.
               
     */
    UINT32 corrcount : 15;
    /* corrcount - Bits[20:6], RWS_V, default = 15'h0 
       
               Corrected error count (CorrCount):
               A 15 bit counter that is incremented each time a corrected
               error is observed by the MCA recording bank.
               Corrected in this context means either of:
                 retry corrected, ECC corrected, mirror scrub corrected.
               No failover events, nor recoverable events are counted.
               
     */
    UINT32 corrstatus : 2;
    /* corrstatus - Bits[22:21], RWS_V, default = 1'b0 
       
               Corrected Error Status Indicator (CorrStatus):
               Not used by the memory controller to reveal any information in this 
       implementation.  
               Will always get written with zeros (by hardware) in this implementation.
               
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[24:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pcc : 1;
    /* pcc - Bits[25:25], RWS_V, default = 1'b0 
       
               Indicates (when set) that the state of the processor might have been
               corrupted by the error condition detected and that reliable restarting 
       of the 
               processor may not be possible. 
               Interpretation:
                 UC=0: corrected error;
                 UC=1, PCC=0: recoverable error;
                 UC=1, PCC=1: fatal error.
               Recoverable error: Uncorrectable error from memory when poison enabled.
               Fatal error: (i) Uncorrectable error from memory when poison disabled, 
       or 
                            (ii) Non-poison error (i.e. control error, non-data error) 
       when  
                                 poison enabled.
               
     */
    UINT32 addrv : 1;
    /* addrv - Bits[26:26], RWS_V, default = 1'b0 
       
               This address-valid bit indicates (when set) that the MC5_ADDR register 
       contains  
               the address of the transaction for which the error occured.
               
     */
    UINT32 miscv : 1;
    /* miscv - Bits[27:27], RWS_V, default = 1'b0 
       
               This miscellaneous-valid bit indicates (when set) that the MCi_MISC 
       register contains  
               additional information regarding the error.
               
     */
    UINT32 en : 1;
    /* en - Bits[28:28], RWS_V, default = 1'b0 
       
               Indicates (when set) that signaling of the machine check exception for 
       the logged 
               error type was enabled by an associated flag bit of the MCi_CTL 
       register. 
               
     */
    UINT32 uc : 1;
    /* uc - Bits[29:29], RWS_V, default = 1'b0 
       
               Error logged in this bank is an uncorrected error.
                 UC=0: corrected error;
                 UC=1, PCC=0: recoverable error;
                 UC=1, PCC=1: fatal error.
               
     */
    UINT32 over : 1;
    /* over - Bits[30:30], RWS_V, default = 1'b0 
       
               Overflow occured. Overflow indicates a second machine check error 
       occured  
               while the information from the first error was still (valid) in the 
       machine  
               check register bank.
               
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RWS_V, default = 1'b0 
       
               VAL flag: Error logged in this bank is valid.
               
     */
  } Bits;
  UINT32 Data;
} MCI_STATUS_SHADOW_N1_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_STATUS MSR. This shadow register is 
 * under BIOS control while the MCi_STATUS  
 *       MSR is under OS control.
 *       Only the OtherInfo field definition is different from the MCi_STATUS MSR 
 * definition. 
 *       
 */
typedef union {
  struct {
    UINT32 otherinfo : 6;
    /* otherinfo - Bits[5:0], RWS_V, default = 6'h0 
       
               Other Info (OtherInfo):
               The functions of the bits in this field are implementation specific and 
       are not 
               part of the machine check architecture.
               In this implementation, this bitfield is used to deal with OS-BIOS race 
       cases and  
               logs the previous shadow error type (if the shadow status wasn't cleared 
       by BIOS). 
                 Other_Info[5:4] - Unused (2'b0)
                 Other_Info[3]: Trigger Event for History of Error: OS-BIOS race. 
                   Trigger event is defined as 0 default. Turns to 1 when M2M OS 
       Machine Check bank status register  
                   is being updated (more precisely: valid is being set when it wasn't 
       set before) while the M2M BIOS  
                   Shadow Machine Check register status's valid bit is still on 
       (indicating we have not read the 
                   previous error info from the shadow register yet) -> a.k.a. 
       triggering event. 
                 Other_Info[2]: History of Fatal Error: 0 is default. When triggering 
       event happens,  
                   turns to 1 if the previous error was a fatal error.
                 Other_Info[1]: History of recoverable Error: 0 is default. When 
       triggering event happens,  
                   turns to 1 if the previous error was a recoverable error
                 Other Info[0]: History of Corrected Error: 0 is default. When 
       triggering event happens,  
                   turns to 1 if the previous error was a corrected error.
               
     */
    UINT32 corrcount : 15;
    /* corrcount - Bits[20:6], RWS_V, default = 15'h0 
       
               Corrected error count (CorrCount):
               A 15 bit counter that is incremented each time a corrected
               error is observed by the MCA recording bank.
               Corrected in this context means either of:
                 retry corrected, ECC corrected, mirror scrub corrected.
               No failover events, nor recoverable events are counted.
        This count value will continue being incremented until cleared by software. The 
       most significant bit, 52, is a sticky count overflow bit. 
               
     */
    UINT32 corrstatus : 2;
    /* corrstatus - Bits[22:21], RWS_V, default = 1'b0 
       
               Corrected Error Status Indicator (CorrStatus):
               Not used by the memory controller to reveal any information in this 
       implementation.  
               Will always get written with zeros (by hardware) in this implementation.
               
     */
    UINT32 ar : 1;
    /* ar - Bits[23:23], RWS_V, default = 1'b0 
       
               When set, indicates immediate recovery action is required.  M2M always 
       sets this bit to 0. 
               
     */
    UINT32 s : 1;
    /* s - Bits[24:24], RWS_V, default = 1'b0 
       
               When set, indicates an MCE/MSMI was generated for a recoverable error; 
       when clear, indicates a CMCI/CSMI was generated.M2M always sets this bit to 0. 
               
     */
    UINT32 pcc : 1;
    /* pcc - Bits[25:25], RWS_V, default = 1'b0 
       
               Indicates (when set) that the state of the processor might have been
               corrupted by the error condition detected and that reliable restarting 
       of the 
               processor may not be possible. 
               Interpretation:
                 UC=0: corrected error;
                 UC=1, PCC=0: recoverable error;
                 UC=1, PCC=1: fatal error.
               Recoverable error: Uncorrectable error from memory when poison enabled.
               Fatal error: (i) Uncorrectable error from memory when poison disabled, 
       or 
                            (ii) Non-poison error (i.e. control error, non-data error) 
       when  
                                 poison enabled.
               
     */
    UINT32 addrv : 1;
    /* addrv - Bits[26:26], RWS_V, default = 1'b0 
       
               This address-valid bit indicates (when set) that the MC5_ADDR register 
       contains  
               the address of the transaction for which the error occured.
               
     */
    UINT32 miscv : 1;
    /* miscv - Bits[27:27], RWS_V, default = 1'b0 
       
               This miscellaneous-valid bit indicates (when set) that the MCi_MISC 
       register contains  
               additional information regarding the error.
               
     */
    UINT32 en : 1;
    /* en - Bits[28:28], RWS_V, default = 1'b0 
       
               Indicates (when set) that signaling of the machine check exception for 
       the logged 
               error type was enabled by an associated flag bit of the MCi_CTL 
       register. 
               
     */
    UINT32 uc : 1;
    /* uc - Bits[29:29], RWS_V, default = 1'b0 
       
               Error logged in this bank is an uncorrected error.
                 UC=0: corrected error;
                 UC=1, PCC=0: recoverable error;
                 UC=1, PCC=1: fatal error.
               
     */
    UINT32 over : 1;
    /* over - Bits[30:30], RWS_V, default = 1'b0 
       
               Overflow occured. Overflow indicates a second machine check error 
       occured  
               while the information from the first error was still (valid) in the 
       machine  
               check register bank.
               
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RWS_V, default = 1'b0 
       
               VAL flag: Error logged in this bank is valid.
               
     */
  } Bits;
  UINT32 Data;
} MCI_STATUS_SHADOW_N1_M2MEM_MAIN_STRUCT;



/* MCI_ADDR_SHADOW0_M2MEM_MAIN_REG supported on:                              */
/*       SKX_A0 (0x40240128)                                                  */
/*       SKX (0x40240128)                                                     */
/* Register default value:              0x00000000                            */
#define MCI_ADDR_SHADOW0_M2MEM_MAIN_REG 0x06004128
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_ADDR MSR (bits [31:0]). This register 
 * is under BIOS control while the MCi_ADDR  
 *       MSR is under OS control.
 *       
 */
typedef union {
  struct {
    UINT32 rsvd : 6;
    /* rsvd - Bits[5:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lophyaddr : 26;
    /* lophyaddr - Bits[31:6], RWS_V, default = 26'h0 
       
               Low-order Bits of the Physical Address (LoPhyAddr):
               Contains 26 bits of physical address [31:6] of the cache line.
               
     */
  } Bits;
  UINT32 Data;
} MCI_ADDR_SHADOW0_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_ADDR MSR (bits [31:0]). This register 
 * is under BIOS control while the MCi_ADDR  
 *       MSR is under OS control.
 *       
 */
typedef union {
  struct {
    UINT32 rsvdlo : 6;
    /* rsvdlo - Bits[5:0], RWS_V, default = 6'h0 
       
               Reserved for future address expansion and should always be set to zero 
       by the HW. 
               
     */
    UINT32 lophyaddr : 26;
    /* lophyaddr - Bits[31:6], RWS_V, default = 26'h0 
       
               Low-order Bits of the Physical Address (LoPhyAddr):
               Contains 26 bits of physical address [31:6] of the cache line.
               
     */
  } Bits;
  UINT32 Data;
} MCI_ADDR_SHADOW0_M2MEM_MAIN_STRUCT;



/* MCI_ADDR_SHADOW1_M2MEM_MAIN_REG supported on:                              */
/*       SKX_A0 (0x4024012C)                                                  */
/*       SKX (0x4024012C)                                                     */
/* Register default value:              0x00000000                            */
#define MCI_ADDR_SHADOW1_M2MEM_MAIN_REG 0x0600412C
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_ADDR MSR (bits [45:32]). This register 
 * is under BIOS control while the MCi_ADDR  
 *       MSR is under OS control.
 *       
 */
typedef union {
  struct {
    UINT32 hiphyaddr : 14;
    /* hiphyaddr - Bits[13:0], RWS_V, default = 14'h0 
       
               High-order Bits of the Physical Address (HiPhyAddr):
               Contains 14 bits of physical address [45:32] of the cache line. TBD: 
       DOES IT NEED TO GROW?? 
               
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCI_ADDR_SHADOW1_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_ADDR MSR (bits [45:32]). This register 
 * is under BIOS control while the MCi_ADDR  
 *       MSR is under OS control.
 *       
 */
typedef union {
  struct {
    UINT32 hiphyaddr : 14;
    /* hiphyaddr - Bits[13:0], RWS_V, default = 14'h0 
       
               High-order Bits of the Physical Address (HiPhyAddr):
               Contains 14 bits of physical address [45:32] of the cache line.
               
     */
    UINT32 rsvdhi : 18;
    /* rsvdhi - Bits[31:14], RWS_V, default = 18'h0 
       
               Reserved for future address expansion and should always be set to zero 
       by the HW. 
               
     */
  } Bits;
  UINT32 Data;
} MCI_ADDR_SHADOW1_M2MEM_MAIN_STRUCT;



/* MCI_MISC_SHADOW_N0_M2MEM_MAIN_REG supported on:                            */
/*       SKX_A0 (0x40240130)                                                  */
/*       SKX (0x40240130)                                                     */
/* Register default value:              0x00000086                            */
#define MCI_MISC_SHADOW_N0_M2MEM_MAIN_REG 0x06004130
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_MISC MSR. This register is under BIOS 
 * control while the MCi_MISC MSR   
 *       is under OS control.
 *       
 */
typedef union {
  struct {
    UINT32 addrlsb : 6;
    /* addrlsb - Bits[5:0], RWS_V, default = 6'h6 
       
               Address LSb (AddrLsb):
               Recoverable address LSb. The lowest valid recoverable address bit. 
       Indicates 
               the position of the least significant bit (LSb) of the recoverable error
               address. It is 6 for this implementation.
               
     */
    UINT32 addrmode : 3;
    /* addrmode - Bits[8:6], RWS_V, default = 3'h2 
       
               Address Mode (AddrMode):
               Address mode for the address logged in IA32_MCi_ADDR. The supported
               address modes are: 000 = Segment Offset; 001 = Linear Address; 010 =
               Physical Address; 011 = Memory Address; 100 to 110 = Reserved; 111 =
               Generic.
               It is 2 for this implementation, i.e. the physical (system) address gets
               logged.
               
     */
    UINT32 rsvd_9 : 2;
    /* rsvd_9 - Bits[10:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 errortype : 10;
    /* errortype - Bits[20:11], RWS_V, default = 10'b0 
       
               Error type attribute (ErrorType):
               Error type determined (as best as possible) by error flow:
                 bits[9:7]: reserved.
                 bit6: Uncorrectable NM$ access because persistently uncorrectable 
       meta-data. 
                 bit5: Txn caused permanent hardware channel failover during mirroring.
                 bit4: Scrub check rd returned uncorrected data (ECC correction is 
       enabled during scrub check rd); 
                 bit3: Scrub check rd returned good data (note that ECC correction is 
       enabled during scrub check rd); 
                 bit2: Txn could not be corrected by ECC;
                 bit1: Txn had an ECC corrected error (corrected by ECC during retry);
                 bit0: Txn had a transient error (corrected by retry without ECC).
               It is possible for multiple bits to be set in this bitvector.
               
     */
    UINT32 trkid : 9;
    /* trkid - Bits[29:21], RWS_V, default = 9'h0 
       
               Mesh2mem Tracker Identifier (TrkId):
                 bits[7:6] = physical channel associated with that tracker (i.e. 
       physical channel for initial access). 
                 {bit[8],bits[5:0]} = htid within that channel.
               
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 smiopcode : 1;
    /* smiopcode - Bits[31:31], RWS_V, default = 4'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               SMI Opcode (SmiOpcode):
                 Decoding in case of SMIAd==1 and SMIMsgClass==0(Request):
                  0=MemRd, 1=MemSpecRd, 2=MemRdData, 4=MemRdXtoS, 5=MemRdXtoI, 
       6=MemRdXtoA, 8=MemInv, 9=MemInvXtoI, 10=MemInvXtoA, 12=MemInvItoX. 
                 Decoding in case of SMIBl==1 and SMIMsgClass==5(Writeback):
                  0=MemWr, 3=MemWrNI, 4=MemWrPtl, 7=MemWrPtlNI, 11=MemWrFlush.
                 Any other encodings different from above are illegal/unexpected.
               
     */
  } Bits;
  UINT32 Data;
} MCI_MISC_SHADOW_N0_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_MISC MSR. This register is under BIOS 
 * control while the MCi_MISC MSR   
 *       is under OS control.
 *       
 */
typedef union {
  struct {
    UINT32 addrlsb : 6;
    /* addrlsb - Bits[5:0], RWS_V, default = 6'h6 
       
               Address LSb (AddrLsb):
               Recoverable address LSb. The lowest valid recoverable address bit. 
       Indicates 
               the position of the least significant bit (LSb) of the recoverable error
               address. It is 6 for this implementation.
               
     */
    UINT32 addrmode : 3;
    /* addrmode - Bits[8:6], RWS_V, default = 3'h2 
       
               Address Mode (AddrMode):
               Address mode for the address logged in IA32_MCi_ADDR. The supported
               address modes are: 000 = Segment Offset; 001 = Linear Address; 010 =
               Physical Address; 011 = Memory Address; 100 to 110 = Reserved; 111 =
               Generic.
               It is 2 for this implementation, i.e. the physical (system) address gets
               logged.
               
     */
    UINT32 rsvd0 : 2;
    /* rsvd0 - Bits[10:9], RWS_V, default = 2'h0 
       
               These bits are reserved for future use.
               
     */
    UINT32 errortype : 10;
    /* errortype - Bits[20:11], RWS_V, default = 10'b0 
       
               Error type attribute (ErrorType):
               Error type determined (as best as possible) by error flow:
                 bits[9:7]: reserved.
                 bit6: Uncorrectable NM$ access because persistently uncorrectable 
       meta-data. 
                 bit5: Txn caused permanent hardware channel failover during mirroring.
                 bit4: Scrub check rd returned uncorrected data (ECC correction is 
       enabled during scrub check rd); 
                 bit3: Scrub check rd returned good data (note that ECC correction is 
       enabled during scrub check rd); 
                 bit2: Txn could not be corrected by ECC;
                 bit1: Txn had an ECC corrected error (corrected by ECC during retry);
                 bit0: Txn had a transient error (corrected by retry without ECC).
               It is possible for multiple bits to be set in this bitvector.
               
     */
    UINT32 trkid : 9;
    /* trkid - Bits[29:21], RWS_V, default = 9'h0 
       
               Mesh2mem Tracker Identifier (TrkId):
                 bits[7:6] = physical channel associated with that tracker (i.e. 
       physical channel for initial access). 
                 {bit[8],bits[5:0]} = htid within that channel.
               
     */
    UINT32 rsvd1 : 1;
    /* rsvd1 - Bits[30:30], RWS_V, default = 1'h0 
       
               This bit is reserved for future use.
               
     */
    UINT32 smiopcode : 1;
    /* smiopcode - Bits[31:31], RWS_V, default = 4'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               SMI Opcode (SmiOpcode):
                 Decoding in case of SMIAd==1 and SMIMsgClass==0(Request):
                  0=MemRd, 1=MemSpecRd, 2=MemRdData, 4=MemRdXtoS, 5=MemRdXtoI, 
       6=MemRdXtoA, 8=MemInv, 9=MemInvXtoI, 10=MemInvXtoA, 12=MemInvItoX. 
                 Decoding in case of SMIBl==1 and SMIMsgClass==5(Writeback):
                  0=MemWr, 3=MemWrNI, 4=MemWrPtl, 7=MemWrPtlNI, 11=MemWrFlush.
                 Any other encodings different from above are illegal/unexpected.
               
     */
  } Bits;
  UINT32 Data;
} MCI_MISC_SHADOW_N0_M2MEM_MAIN_STRUCT;



/* MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG supported on:                            */
/*       SKX_A0 (0x40240134)                                                  */
/*       SKX (0x40240134)                                                     */
/* Register default value:              0x00000000                            */
#define MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG 0x06004134
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_MISC MSR. This register is under BIOS 
 * control while the MCi_MISC MSR   
 *       is under OS control.
 *       
 */
typedef union {
  struct {
    UINT32 smiopcode : 3;
    /* smiopcode - Bits[2:0], RWS_V, default = 4'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               SMI Opcode (SmiOpcode):
                 Decoding in case of SMIAd==1 and SMIMsgClass==0(Request):
                  0=MemRd, 1=MemSpecRd, 2=MemRdData, 4=MemRdXtoS, 5=MemRdXtoI, 
       6=MemRdXtoA, 8=MemInv, 9=MemInvXtoI, 10=MemInvXtoA, 12=MemInvItoX. 
                 Decoding in case of SMIBl==1 and SMIMsgClass==5(Writeback):
                  0=MemWr, 3=MemWrNI, 4=MemWrPtl, 7=MemWrPtlNI, 11=MemWrFlush.
                 Any other encodings different from above are illegal/unexpected.
               
     */
    UINT32 smimsgclass : 3;
    /* smimsgclass - Bits[5:3], RWS_V, default = 3'h0 
       
               SMI Message Class (SmiMsgClass):
               The logged transaction's message class. The expectation is to only see 
       following values: 
                 0: Request message class.
                 5: Write-back message class.
               
     */
    UINT32 smiad : 1;
    /* smiad - Bits[6:6], RWS_V, default = 1'h0 
       
               SMI AD message (SmiAD):
               The logged transaction came in on AD.
                 0: txn from BL.
                 1: txn from AD.
               
     */
    UINT32 mccmdvld : 1;
    /* mccmdvld - Bits[7:7], RWS_V, default = 1'h0 
       
               Memory Controller Command Valid (McCmdVld):
               Memory controller command got logged.
                 0: no McCmd logged, so don't interpret McCmd fields.
                 1: McCmd logged; can interpret McCmd fields.
               
     */
    UINT32 mccmdopcode : 6;
    /* mccmdopcode - Bits[13:8], RWS_V, default = 6'h0 
       
               Memory Controller Command Opcode (McCmdOpcode):
               Memory controller command for which the error got asserted (first error 
       during retries): 
                 0=MEMRD, 1=MEMSPECRD, 2=MEMRDDATA, 
                 4=MEMRDXTOS, 5=MEMRDXTOI, 6=MEMRDXTOA, 
                 8=MEMINV, 9=MEMINVXTOI, 10=MEMINVXTOA, 12=MEMINVITOX, 
                 16=MEMWR, 19=MEMWRNI, 20=MEMWRPTL, 23=MEMWRPTLNI, 
                 21=PATROL, 27=MEMWRFLUSH, 46=DEALLOCSBRD, 47=DEALLOCSBPF, 
                 40= FMFILLRD, 48=DIRUPD, 49=NMFILLWR, 51=FMWR4NM, 55=FMWRPTL4NM, 
       56=FMEVICTWR,  
                 62=DEALLOCSBWR, 63=DEALLOCSBALL. 
               
     */
    UINT32 mccmdmemregion : 4;
    /* mccmdmemregion - Bits[17:14], RWS_V, default = 4'h0 
       
               Memory region type (McCmdMemRegion):
                 bit 0: block region.
                 bit 1: pmem region.
                 bits [3:2] = {persistent, cacheable}.
                 { persistent, cacheable} encoding: 0 = 1LM DDR4 access, 1 = DDRT 
       access, 2 = DDR4 NM cache access (non-Pmem), 3 = DDR4 NM cache access for Pmem. 
               
     */
    UINT32 mccmdchnl : 2;
    /* mccmdchnl - Bits[19:18], RWS_V, default = 2'h0 
       
               Memory channel (McCmdChnl):
               Physical memory channel which had the error. Determined during retry.
               If no error on retry then this just captures the channel of the initial 
       retry.  
               If there was an error during the retry then this will hold the physical 
       channel on  
               which the error occured.
               
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[24:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mccmdmirrregion : 1;
    /* mccmdmirrregion - Bits[25:25], RWS_V, default = 1'h0 
       
               MC command was to a mirror region (McCmdMirrRegion).
               Which might or might not have failed over (see McCmdMirrFo bit).
               
     */
    UINT32 mccmdmirrsec : 1;
    /* mccmdmirrsec - Bits[26:26], RWS_V, default = 1'h0 
       
               Error on secondary mirror channel (McCmdMirrSec).
               
     */
    UINT32 mccmdmirrfo : 1;
    /* mccmdmirrfo - Bits[27:27], RWS_V, default = 1'h0 
       
               MC command was to an already permanently failed over channel/region 
       (MirrFo). 
               
     */
    UINT32 dataerrorchunk : 2;
    /* dataerrorchunk - Bits[29:28], RWS_V, default = 2'h0 
       
               Chunk containing Data Error (DataErrorChunk):
               Cache line half on which the data error occured:
                 00: no data error.
                 01: a data error signal came on 1st half of cache line (critical 
       chunk). 
                 10: a data error signal came on 2nd half of cache line (non-critical 
       chunk). 
                 11: a data error signal asserted both on 1st and 2nd half of cache 
       line. 
               
     */
    UINT32 mirrorcorrerr : 1;
    /* mirrorcorrerr - Bits[30:30], RWS_V, default = 1'h0 
       
               Mirror Channel Corrected Error (MirrorCorrErr):
               Error was corrected during mirroring and primary channel scrubbed 
       successfully.  
               A successful mirror scrub implies an error on the primary channel got 
               corrected by reading the data from the secondary, writing that data to 
       the primary  
               channel and successfully reading back the data from the primary channel.
               
     */
    UINT32 mirrorfailover : 1;
    /* mirrorfailover - Bits[31:31], RWS_V, default = 1'h0 
       
               Mirror Failover Error (MirrorFailover):
               An error occured on a memory channel of the mirror channel pair. The 
       error was corrected 
               by channel failover under mirroring. This implies an uncorrectable (i.e. 
       no correctable by  
               mirror scrub) error on a channel. After failover, the complete (failed) 
       channel is no longer  
               available as a mirror back-up.
               
     */
  } Bits;
  UINT32 Data;
} MCI_MISC_SHADOW_N1_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Contains information similar to MCi_MISC MSR. This register is under BIOS 
 * control while the MCi_MISC MSR   
 *       is under OS control.
 *       
 */
typedef union {
  struct {
    UINT32 smiopcode : 3;
    /* smiopcode - Bits[2:0], RWS_V, default = 4'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               SMI Opcode (SmiOpcode):
                 Decoding in case of SMIAd==1 and SMIMsgClass==0(Request):
                  0=MemRd, 1=MemSpecRd, 2=MemRdData, 4=MemRdXtoS, 5=MemRdXtoI, 
       6=MemRdXtoA, 8=MemInv, 9=MemInvXtoI, 10=MemInvXtoA, 12=MemInvItoX. 
                 Decoding in case of SMIBl==1 and SMIMsgClass==5(Writeback):
                  0=MemWr, 3=MemWrNI, 4=MemWrPtl, 7=MemWrPtlNI, 11=MemWrFlush.
                 Any other encodings different from above are illegal/unexpected.
               
     */
    UINT32 smimsgclass : 3;
    /* smimsgclass - Bits[5:3], RWS_V, default = 3'h0 
       
               SMI Message Class (SmiMsgClass):
               The logged transaction's message class. The expectation is to only see 
       following values: 
                 0: Request message class.
                 5: Write-back message class.
               
     */
    UINT32 smiad : 1;
    /* smiad - Bits[6:6], RWS_V, default = 1'h0 
       
               SMI AD message (SmiAD):
               The logged transaction came in on AD.
                 0: txn from BL.
                 1: txn from AD.
               
     */
    UINT32 mccmdvld : 1;
    /* mccmdvld - Bits[7:7], RWS_V, default = 1'h0 
       
               Memory Controller Command Valid (McCmdVld):
               Memory controller command got logged.
                 0: no McCmd logged, so don't interpret McCmd fields.
                 1: McCmd logged; can interpret McCmd fields.
               
     */
    UINT32 mccmdopcode : 6;
    /* mccmdopcode - Bits[13:8], RWS_V, default = 6'h0 
       
               Memory Controller Command Opcode (McCmdOpcode):
               Memory controller command for which the error got asserted (first error 
       during retries): 
                 0=MEMRD, 1=MEMSPECRD, 2=MEMRDDATA, 
                 4=MEMRDXTOS, 5=MEMRDXTOI, 6=MEMRDXTOA, 
                 8=MEMINV, 9=MEMINVXTOI, 10=MEMINVXTOA, 12=MEMINVITOX, 
                 16=MEMWR, 19=MEMWRNI, 20=MEMWRPTL, 23=MEMWRPTLNI, 
                 21=PATROL, 27=MEMWRFLUSH, 46=DEALLOCSBRD, 47=DEALLOCSBPF, 
                 40= FMFILLRD, 48=DIRUPD, 49=NMFILLWR, 51=FMWR4NM, 55=FMWRPTL4NM, 
       56=FMEVICTWR,  
                 62=DEALLOCSBWR, 63=DEALLOCSBALL. 
               
     */
    UINT32 mccmdmemregion : 4;
    /* mccmdmemregion - Bits[17:14], RWS_V, default = 4'h0 
       
               Memory region type (McCmdMemRegion):
                 bit 0: block region.
                 bit 1: pmem region.
                 bits [3:2] = {persistent, cacheable}.
                 { persistent, cacheable} encoding: 0 = 1LM DDR4 access, 1 = DDRT 
       access, 2 = DDR4 NM cache access (non-Pmem), 3 = DDR4 NM cache access for Pmem. 
               
     */
    UINT32 mccmdchnl : 2;
    /* mccmdchnl - Bits[19:18], RWS_V, default = 2'h0 
       
               Memory channel (McCmdChnl):
               Physical memory channel which had the error. Determined during retry.
               If no error on retry then this just captures the channel of the initial 
       retry.  
               If there was an error during the retry then this will hold the physical 
       channel on  
               which the error occured.
               
     */
    UINT32 rsvd2 : 5;
    /* rsvd2 - Bits[24:20], RWS_V, default = 5'h0 
       
               These bits are reserved for future use.
               
     */
    UINT32 mccmdmirrregion : 1;
    /* mccmdmirrregion - Bits[25:25], RWS_V, default = 1'h0 
       
               MC command was to a mirror region (McCmdMirrRegion).
               Which might or might not have failed over (see McCmdMirrFo bit).
               
     */
    UINT32 mccmdmirrsec : 1;
    /* mccmdmirrsec - Bits[26:26], RWS_V, default = 1'h0 
       
               Error on secondary mirror channel (McCmdMirrSec).
               
     */
    UINT32 mccmdmirrfo : 1;
    /* mccmdmirrfo - Bits[27:27], RWS_V, default = 1'h0 
       
               MC command was to an already permanently failed over channel/region 
       (MirrFo). 
               
     */
    UINT32 dataerrorchunk : 2;
    /* dataerrorchunk - Bits[29:28], RWS_V, default = 2'h0 
       
       	These bits are not used in SKX.
               Chunk containing Data Error (DataErrorChunk):
               Cache line half on which the data error occured:
                 00: no data error.
                 01: a data error signal came on 1st half of cache line (critical 
       chunk). 
                 10: a data error signal came on 2nd half of cache line (non-critical 
       chunk). 
                 11: a data error signal asserted both on 1st and 2nd half of cache 
       line. 
               
     */
    UINT32 mirrorcorrerr : 1;
    /* mirrorcorrerr - Bits[30:30], RWS_V, default = 1'h0 
       
               Mirror Channel Corrected Error (MirrorCorrErr):
               Error was corrected during mirroring and primary channel scrubbed 
       successfully.  
               A successful mirror scrub implies an error on the primary channel got 
               corrected by reading the data from the secondary, writing that data to 
       the primary  
               channel and successfully reading back the data from the primary channel.
               
     */
    UINT32 mirrorfailover : 1;
    /* mirrorfailover - Bits[31:31], RWS_V, default = 1'h0 
       
               Mirror Failover Error (MirrorFailover):
               An error occured on a memory channel of the mirror channel pair. The 
       error was corrected 
               by channel failover under mirroring. This implies an uncorrectable (i.e. 
       no correctable by  
               mirror scrub) error on a channel. After failover, the complete (failed) 
       channel is no longer  
               available as a mirror back-up.
               
     */
  } Bits;
  UINT32 Data;
} MCI_MISC_SHADOW_N1_M2MEM_MAIN_STRUCT;



/* ERR_CNTR_CTL_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240140)                                                  */
/*       SKX (0x40240140)                                                     */
/* Register default value:              0x00001401                            */
#define ERR_CNTR_CTL_M2MEM_MAIN_REG 0x06004140
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Saturating error counters (6b) in the M2M, track the number of 
 * transactions which had a memory controller error. 
 *       A transaction in this context is an SMI3 transaction from the mesh (NOT 
 * commands to iMC). So even if multiple  
 *       retries are sent to the memory controller, the retried commands to the iMC 
 * are not counted individually. 
 * 
 *       This control register allows indirect access to these M2M txn error 
 * counters: Reading the counters of a particular  
 *       error type, or clearing all the counters. ERR_CNTR_CTL.CntrIdx maps to 
 * following error types: 
 *         DDR4_CHNLS_LINK_TRANSIENT_ERR_CNTR  = 0;
 *         DDR4_CHNLS_ECC_CORRECTED_ERR_CNTR   = 1;
 *         DDR4_CHNLS_UNCORRECTABLE_ERR_CNTR   = 2;
 *         DDRT_CHNLS_LINK_TRANSIENT_ERR_CNTR  = 3;
 *         DDRT_CHNLS_ECC_CORRECTED_ERR_CNTR   = 4;
 *         DDRT_CHNLS_UNCORRECTABLE_ERR_CNTR   = 5;
 *         CHNLS_PRI_MIRRSCRUBRD_NOERR_CNTR    = 6;
 *         CHNLS_SEC_MIRRSCRUBRD_NOERR_CNTR    = 7;
 *         CHNLS_PRI_MIRRSCRUBRD_ERR_CNTR      = 8;
 *         CHNLS_SEC_MIRRSCRUBRD_ERR_CNTR      = 9;
 *       When performing a mirror scrub read, it can either return with error 
 * (=uncorrectable) or no error (=possibly ECC-corrected)  
 *       and hence will be counted respectively as either a MIRRSCRUBRD_NOERR or 
 * MIRRSCRUBRD_ERR type.   
 *       Because our mirroring implementation goes to the mirrored channel and gets 
 * the data from that channel (to minimize  
 *       the probability for miscorrections) even on an ECC correctable error, the 
 * ECC_CORRECTED type needs to be interpreted  
 *       as an ecc correctable type (as long as channel not failed over yet).
 * 
 *       A typical usage model would be for BIOS, on an SMI, to read the 
 * ERR_CNTR.ACntrNonZero or the ERR_CNTR.AThresholdReached  
 *       CR bit and when e.g. the AThresholdReached bit is set then use 
 * ERR_CNTR_CTL to read all the counters and add them to  
 *       BIOS internal cumulative (possibly leaky) error counts. Once that is done, 
 * BIOS clears all the counters. 
 * 
 *       BIOS can use these counters to gauge the health of the DDR4/DDRT-link, or 
 * the DDR4 DRAM.  
 *       BIOS can use this information to log/report this to their desire.
 * 
 *       Note, to determine which DDR4 dimm or rank is problematic, the iMC rank 
 * error counters should be consulted. 
 *       Similarly, to determine DDRT ECC errors, CRs need to be consulted. 
 * DDRT_ECC_CORRECTED error type in the M2M  
 *       only provides information w.r.t. the DDRT bus health.
 *       
 */
typedef union {
  struct {
    UINT32 cntena : 1;
    /* cntena - Bits[0:0], RW, default = 1'h1 
       
               Enable counting by all the M2M internal error counters.
               
     */
    UINT32 clrcntrs : 1;
    /* clrcntrs - Bits[1:1], RW_V, default = 1'h0 
       
               When written with a 1, will clear all the M2M internal error counters. 
               M2M hardware will automatically clear this bit a cycle after it is 
       written. 
               
     */
    UINT32 rsvd_2 : 1;
    /* rsvd_2 - Bits[2:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cntridx : 4;
    /* cntridx - Bits[6:3], RW, default = 4'h0 
       
               Index of the counter type to read.
               
     */
    UINT32 rsvd_7 : 2;
    /* rsvd_7 - Bits[8:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 smionthreshold : 1;
    /* smionthreshold - Bits[9:9], RW, default = 1'h0 
       
               Signal an SMI when the ERR_CNTR.AThresholdReached bit gets set by 
       hardware. 
               
     */
    UINT32 thresholdlog2 : 3;
    /* thresholdlog2 - Bits[12:10], RW_V, default = 3'h5 
       
               '1<<ThresholdLog2' defines the internal threshold 2^ThresholdLog2 at 
       which the ERR_CNTR.AThresholdReached bit will be set,  
               if at least one of the internal counters reaches this threshold. The 
       threshold is per error type and writing/reading the threshold is  
               through indirect access through ThresholdRd, ThresholdWr and 
       ThresholdIdx field. When ThresholdWr=1 then this ThesholdLog2 value will  
               get written to the internal threshold. When ThresholdRd=1 then this 
       ThresholdLog2 will contain the value read from the internal  
               ThresholdLog2 bits. On cold reset, the internal ThresholdLog2 get set to 
       5. 
               
     */
    UINT32 thresholdidx : 3;
    /* thresholdidx - Bits[15:13], RW, default = 3'h0 
       
               Index of the internal ThresholdLog2 type to read/write:
                 0=LINK_TRANSIENT_ERR_TYPE, 
                 1=ECC_CORRECTED_ERR_TYPE, 
                 2=UNCORRECTABLE_ERR_TYPE, 
                 3=MIRRSCRUBRD_NOERR_TYPE, 
                 4=MIRRSCRUBRD_ERR_TYPE
               
     */
    UINT32 thresholdwr : 1;
    /* thresholdwr - Bits[16:16], RW_V, default = 1'h0 
       
               Write the internal ThresdholdLog2 value for error type specified by 
       ThresholdIdx,  
               with the value specified by the ThresholdLog2 field of this register.
               This bit will automatically reset a cycle after it gets set.
               
     */
    UINT32 thresholdrd : 1;
    /* thresholdrd - Bits[17:17], RW_V, default = 1'h0 
       
               Read the internal ThresdholdLog2 value for error type specified by 
       ThresholdIdx,  
               and store the result in the ThresholdLog2 field of this register.
               This bit will automatically reset a cycle after it gets set.
               
     */
    UINT32 ptlwrcntdis : 1;
    /* ptlwrcntdis - Bits[18:18], RW, default = 1'h0 
       
               Disable counting of partial write errors. 
               Disables M2M ERR_CNTR counting of errors for partial writes. Can be 
       useful if the  
               defeature Chicken0.WrBothChnlsAfterFailover=1 is set, yet permanent 
       failover already took place. 
               Note that patrol might also still hit on the flaky physical 
               point on which this bit will have no effect.
               This bit will disable error counting of partial write errors altogether 
       (even if not in  
               mirroring mode or if already in failover mode).
               
     */
    UINT32 reservedforeco : 1;
    /* reservedforeco - Bits[19:19], RW, default = 1'h0 
       
               Reserved for future usage up to and including possible ECO.
               
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} ERR_CNTR_CTL_M2MEM_MAIN_STRUCT;


/* ERR_CNTR_M2MEM_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x40240144)                                                  */
/*       SKX (0x40240144)                                                     */
/* Register default value:              0x00000000                            */
#define ERR_CNTR_M2MEM_MAIN_REG 0x06004144
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Exposes the M2M internal error counts as they get indirectly read through 
 * the ERR_CNTR_CTL CR. 
 *       The ACntrNonZero and AThresholdReached are continuously kept up-to-date by 
 * hardware and do not  
 *       need to get read indirectly through the ERR_CNTR_CTL CR.
 * 
 * What we want to call out here specifically are the vastly differing 
 * interpretations of the MC DDR4 vs MC DDRT error counts for the 
 * transient/ecc-corrected types. 
 * 
 * Interpretation of the 'transient' count: transient from an MC perspective, in 
 * that a retry (might be without even applying MC ECC) resulted in good data. 
 *   DDR4: It implies a transient error e.g. most likely a link transient error. 
 *   DDRT: It might imply that the DDRT controller on the DDRT dimm fixed the data 
 * by ECC so in that case the DDRT controller itself might hold more accurate error 
 * information w.r.t. the DDRT storage devices. 
 * Interpretation of the 'ECC corrected' count: ECC in the MC was applied to 
 * correct the data. 
 *   DDR4: Implies a persistent (stored or simple link) error because a retry 
 * without ECC didn't correct the error and ECC was needed to get correct the data. 
 *   DDRT: Implies a (persistent and simple) link error because a simple retry to 
 * the DDRT controller did not correct the data (Note, the DDRT controller on the 
 * DDRT dimm would apply ECC internally, invisible to the MC). 
 * 
 *       
 */
typedef union {
  struct {
    UINT32 cntrchnl0 : 6;
    /* cntrchnl0 - Bits[5:0], RW_V, default = 6'h0 
       
               Chnl0 count associated with ERR_CNTR_CTL.CntrIdx.
               
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cntrchnl1 : 6;
    /* cntrchnl1 - Bits[13:8], RW_V, default = 6'h0 
       
               Chnl1 count associated with ERR_CNTR_CTL.CntrIdx.
               
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cntrchnl2 : 6;
    /* cntrchnl2 - Bits[21:16], RW_V, default = 6'h0 
       
               Chnl2 count associated with ERR_CNTR_CTL.CntrIdx.
               
     */
    UINT32 rsvd_22 : 8;
    /* rsvd_22 - Bits[29:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 acntrnonzero : 1;
    /* acntrnonzero - Bits[30:30], RW_V, default = 1'h0 
       
               At least one of the M2M internal error counters is non-zero. This field 
       is continuously updated by hardware. 
               Clear by writing a 1 to ERR_CNTR_CTL.ClrCntrs.
               
     */
    UINT32 athresholdreached : 1;
    /* athresholdreached - Bits[31:31], RW_V, default = 1'h0 
       
               At least one of the M2M internal error counters hit the threshold 
       '1<<ERR_CNTR_CTL.ThresholdLog2'. 
               Clear by writing a 1 to ERR_CNTR_CTL.ClrCntrs.
               
     */
  } Bits;
  UINT32 Data;
} ERR_CNTR_M2MEM_MAIN_STRUCT;


/* VIRAL_N0_M2MEM_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x40240150)                                                  */
/*       SKX (0x40240150)                                                     */
/* Register default value:              0x00000000                            */
#define VIRAL_N0_M2MEM_MAIN_REG 0x06004150
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Allows disabling individual viral sources so that viral would not assert 
 * for that error source. 
 *       The programming of this register is only relevant when viral is enabled, 
 * i.e. when M2M  
 *       MCi_MCG_Contain[ViralEnable] is 1.
 *       
 */
typedef union {
  struct {
    UINT32 disviralonmcafatal : 1;
    /* disviralonmcafatal - Bits[0:0], RW, default = 1'b0 
       
               Disable Viral on Fatal Memory Access error (DisViralOnMcaFatal):
                 0: Going viral is enabled for an M2M fatal memory access error (e.g. 
       uncorrectable rd). 
                 1: Going viral is disabled for an M2M fatal memory access error (e.g. 
       uncorrectable rd). 
               
     */
    UINT32 disviralonmcabucket1 : 1;
    /* disviralonmcabucket1 - Bits[1:1], RW, default = 1'b0 
       
               Disable Viral on Bucket1 Mca (DisViralOnMcaBucket1):
                 0: Going viral is enabled for an M2M bucket1 error.
                 1: Going viral is disabled for an M2M bucket1 error.
               
     */
    UINT32 disviralonmcabgf : 1;
    /* disviralonmcabgf - Bits[2:2], RW, default = 1'b0 
       
               Disable Viral on Bgf Mca (DisViralOnMcaBgf):
                 0: Going viral is enabled for an M2M BGF error.
                 1: Going viral is disabled for an M2M BGF error.
               
     */
    UINT32 disviralonmcaparity : 1;
    /* disviralonmcaparity - Bits[3:3], RW, default = 1'b0 
       
               Disable Viral on Parity Mca (DisViralOnMcaParity):
                 0: Going viral is enabled for an M2M address/cmd/state parity error.
                 1: Going viral is disabled for an M2M address/cmd/state parity error.
               
     */
    UINT32 disviralonmcatrkrtimeout : 1;
    /* disviralonmcatrkrtimeout - Bits[4:4], RW, default = 1'b0 
       
               Disable Viral on Tracker Timeout Mca (DisViralOnMcaTrkrTimeout):
                 0: Going viral is enabled for an M2M Tracker Timeout.
                 1: Going viral is disabled for an M2M Tracker Timeout.
               
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VIRAL_N0_M2MEM_MAIN_STRUCT;




/* PMONCNTR0_N0_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240200)                                                  */
/*       SKX (0x40240200)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR0_N0_M2MEM_MAIN_REG 0x06004200
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTR0_N0_M2MEM_MAIN_STRUCT;


/* PMONCNTR0_N1_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240204)                                                  */
/*       SKX (0x40240204)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR0_N1_M2MEM_MAIN_REG 0x06004204
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR0_N1_M2MEM_MAIN_STRUCT;


/* PMONCNTR1_N0_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240208)                                                  */
/*       SKX (0x40240208)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR1_N0_M2MEM_MAIN_REG 0x06004208
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTR1_N0_M2MEM_MAIN_STRUCT;


/* PMONCNTR1_N1_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x4024020C)                                                  */
/*       SKX (0x4024020C)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR1_N1_M2MEM_MAIN_REG 0x0600420C
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR1_N1_M2MEM_MAIN_STRUCT;


/* PMONCNTR2_N0_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240210)                                                  */
/*       SKX (0x40240210)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR2_N0_M2MEM_MAIN_REG 0x06004210
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTR2_N0_M2MEM_MAIN_STRUCT;


/* PMONCNTR2_N1_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240214)                                                  */
/*       SKX (0x40240214)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR2_N1_M2MEM_MAIN_REG 0x06004214
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR2_N1_M2MEM_MAIN_STRUCT;


/* PMONCNTR3_N0_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240218)                                                  */
/*       SKX (0x40240218)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR3_N0_M2MEM_MAIN_REG 0x06004218
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTR3_N0_M2MEM_MAIN_STRUCT;


/* PMONCNTR3_N1_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x4024021C)                                                  */
/*       SKX (0x4024021C)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR3_N1_M2MEM_MAIN_REG 0x0600421C
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR3_N1_M2MEM_MAIN_STRUCT;


/* PMONCNTR4_N0_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240220)                                                  */
/*       SKX (0x40240220)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR4_N0_M2MEM_MAIN_REG 0x06004220
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 32;
    /* countervalue - Bits[31:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTR4_N0_M2MEM_MAIN_STRUCT;


/* PMONCNTR4_N1_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240224)                                                  */
/*       SKX (0x40240224)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTR4_N1_M2MEM_MAIN_REG 0x06004224
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       This register is a perfmon counter. Software can both read it and write 
 * it. 
 *       
 */
typedef union {
  struct {
    UINT32 countervalue : 16;
    /* countervalue - Bits[15:0], RW_V, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
               Counter Value (CounterValue):
               Current value of the counter.
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONCNTR4_N1_M2MEM_MAIN_STRUCT;


/* PMONCNTRCFG0_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240228)                                                  */
/*       SKX (0x40240228)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG0_M2MEM_MAIN_REG 0x06004228
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 *  Perfmon Counter Control Register.  
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'h0 
       
               Event Select (EventSelect):
               This field is used to decode the PerfMon event which is selected. 
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'h0 
       
               Unit Mask (UnitMask):
               This mask selects the sub-events to be selected for creation of the 
       event. 
               The selected sub-events are bitwise OR-ed together to create event. At 
       least 
               one sub-event must be selected otherwise the PerfMon event signals will 
       not 
               ever get asserted. Events with no sub-events listed effectively have 
       only one 
               sub-event; bit 8 must be set to 1 in this case.
               
     */
    UINT32 queoccupreset : 1;
    /* queoccupreset - Bits[16:16], WO, default = 1'h0 
       
               Queue Occupancy Reset (QueOccupReset):
               This write only bit causes the queue occupancy counter of the PerfMon
               counter for which this Perf event select register is associated to be 
       cleared to 
               all zeroes when a '1' is written to it. No action is taken when a '0' is 
       written. 
               Note: Since the queue occupancy counters never drop below zero, it is
               possible for the counters to 'catch up' with the real occupancy of the 
       queue 
               in question when the real occupancy drop to zero.
               
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'h0 
       
               Counter Reset (Counter_Reset):
               When this bit is set, the corresponding counter will be reset to 0. This 
       allows 
               for a quick reset of the counter when changing event encodings.
               
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'h0 
       
               Edge Detect (EdgeDetect):
               Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a 
       given 
               event. For example, we have an event that counts the number of cycles in
               L0s mode in Intel UPI. By using edge detect, one can count the number of 
       times 
               that we entered L0s mode (by detecting the rising edge).
       
               Edge detect only works in conjunction with threshholding. This is true 
       even 
               for events that can only increment by 1 in a given cycle (like the L0s
               example above). In this case, one should set a threshhold of 1. One can 
       also 
               use Edge Detect with queue occupancy events. For example, if one wanted
               to count the number of times when the TOR occupancy was larger than 5,
               one would selet the TOR occupancy event with a threshold of 5 and set 
       the 
               Edge Detect bit.
       
               Edge detect can also be used with the invert. This is generally not
               particularly useful, as the count of falling edges compared to rising 
       edges 
               will either be the same or differ by 1.
               
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'h0 
       
               Overflow Enable (OverflowEnable):
               Setting this bit will enable the counter to send an overflow signal. If 
       this bit 
               is not set, the counter will wrap around when it overflows without 
       triggering 
               anything. If this bit is set and the Unit's configuration register has 
       Overflow 
               enabled, then a signal will be transmitted to the Ubox.
               
     */
    UINT32 internal : 1;
    /* internal - Bits[21:21], RW_V, default = 1'h0 
       
               This bit needs to be asserted if the event which needs to be selected is 
       an 
               internal event. 
       
               Internal events are events that are only accessible on unlocked parts. 
       One 
               can basically think of the internal bit as an extension of the event 
       select bits 
               [7:0], as it allows the use to gain access to additional events. It will 
       not be 
               possible to use these events on locked parts. For perfmons that exist in 
       MSR 
               space, the MSR write mask is set on this bit, and the ucode handler will
               trigger a GP fault if one attempts to write to these bits. Writing to 
       these bits 
               is controlled in other ways in the other units.
               
     */
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'h0 
       
               Counter Enable (CounterEnable):
               This field is the local enable for the PerfMon Counter. This bit must be
               asserted in order for the PerfMon counter to begin counting the events
               selected by the 'event select', 'unit mask', and 'internal' bits (see 
       the fields 
               below). There is one bit per PerfMon Counter. Note that if this bit is 
       set to 1 
               but the Unit Control Registers have determined that counting is 
       disabled, 
               then the counter will not count.
               
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'h0 
       
               This bit indicates how the threshold field will be compared to the 
       incoming 
               event. When 0, the comparison that will be done is threshold >= event.
               When set to 1, the comparison that will be done is inverted from the 
       case 
               where this bit is set to 0, i.e., threshold < event. The invert bit only 
       works 
               when Threshhold != 0. So, if one would like to invert a non-occupancy 
       event 
               (like LLC Hit), one needs to set the threshhold to 1.
               
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'h0 
       
               This field is compared directly against an incoming event value for 
       events 
               that can increment by 1 or more in a given cycle. Since the widest event
               from the UnCore is 7bits (queue occupancy), bit 31 is unused. The result 
       of 
               the comparison is effectively a 1 bit wide event, i.e., the counter will 
       be 
               incremented by 1 when the comparison is true (the type of comparison
               depends on the setting of the 'invert' bit - see bit 23 below) no matter 
       how 
               wide the original event was. When this field is zero, threshold 
       comparison is 
               disabled and the event is passed without modification.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG0_M2MEM_MAIN_STRUCT;


/* PMONCNTRCFG1_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240230)                                                  */
/*       SKX (0x40240230)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG1_M2MEM_MAIN_REG 0x06004230
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 *  Perfmon Counter Control Register.  
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'h0 
       
               Event Select (EventSelect):
               This field is used to decode the PerfMon event which is selected. 
               
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'h0 
       
               Unit Mask (UnitMask):
               This mask selects the sub-events to be selected for creation of the 
       event. 
               The selected sub-events are bitwise OR-ed together to create event. At 
       least 
               one sub-event must be selected otherwise the PerfMon event signals will 
       not 
               ever get asserted. Events with no sub-events listed effectively have 
       only one 
               sub-event; bit 8 must be set to 1 in this case.
               
     */
    UINT32 queoccupreset : 1;
    /* queoccupreset - Bits[16:16], WO, default = 1'h0 
       
               Queue Occupancy Reset (QueOccupReset):
               This write only bit causes the queue occupancy counter of the PerfMon
               counter for which this Perf event select register is associated to be 
       cleared to 
               all zeroes when a '1' is written to it. No action is taken when a '0' is 
       written. 
               Note: Since the queue occupancy counters never drop below zero, it is
               possible for the counters to 'catch up' with the real occupancy of the 
       queue 
               in question when the real occupancy drop to zero.
               
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'h0 
       
               Counter Reset (CounterReset):
               When this bit is set, the corresponding counter will be reset to 0. This 
       allows 
               for a quick reset of the counter when changing event encodings.
               
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'h0 
       
               Edge Detect (EdgeDetect):
               Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a 
       given 
               event. For example, we have an event that counts the number of cycles in
               L0s mode in Intel UPI. By using edge detect, one can count the number of 
       times 
               that we entered L0s mode (by detecting the rising edge).
       
               Edge detect only works in conjunction with threshholding. This is true 
       even 
               for events that can only increment by 1 in a given cycle (like the L0s
               example above). In this case, one should set a threshhold of 1. One can 
       also 
               use Edge Detect with queue occupancy events. For example, if one wanted
               to count the number of times when the TOR occupancy was larger than 5,
               one would selet the TOR occupancy event with a threshold of 5 and set 
       the 
               Edge Detect bit.
       
               Edge detect can also be used with the invert. This is generally not
               particularly useful, as the count of falling edges compared to rising 
       edges 
               will either be the same or differ by 1.
               
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'h0 
       
               Overflow Enable (OverflowEnable):
               Setting this bit will enable the counter to send an overflow signal. If 
       this bit 
               is not set, the counter will wrap around when it overflows without 
       triggering 
               anything. If this bit is set and the Unit's configuration register has 
       Overflow 
               enabled, then a signal will be transmitted to the Ubox.
               
     */
    UINT32 internal : 1;
    /* internal - Bits[21:21], RW_V, default = 1'h0 
       
               This bit needs to be asserted if the event which needs to be selected is 
       an 
               internal event. 
       
               Internal events are events that are only accessible on unlocked parts. 
       One 
               can basically think of the internal bit as an extension of the event 
       select bits 
               [7:0], as it allows the use to gain access to additional events. It will 
       not be 
               possible to use these events on locked parts. For perfmons that exist in 
       MSR 
               space, the MSR write mask is set on this bit, and the ucode handler will
               trigger a GP fault if one attempts to write to these bits. Writing to 
       these bits 
               is controlled in other ways in the other units.
               
     */
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'h0 
       
               Counter Enable (CounterEnable):
               This field is the local enable for the PerfMon Counter. This bit must be
               asserted in order for the PerfMon counter to begin counting the events
               selected by the 'event select', 'unit mask', and 'internal' bits (see 
       the fields 
               below). There is one bit per PerfMon Counter. Note that if this bit is 
       set to 1 
               but the Unit Control Registers have determined that counting is 
       disabled, 
               then the counter will not count.
               
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'h0 
       
               This bit indicates how the threshold field will be compared to the 
       incoming 
               event. When 0, the comparison that will be done is threshold >= event.
               When set to 1, the comparison that will be done is inverted from the 
       case 
               where this bit is set to 0, i.e., threshold < event. The invert bit only 
       works 
               when Threshhold != 0. So, if one would like to invert a non-occupancy 
       event 
               (like LLC Hit), one needs to set the threshhold to 1.
               
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'h0 
       
               This field is compared directly against an incoming event value for 
       events 
               that can increment by 1 or more in a given cycle. Since the widest event
               from the UnCore is 7bits (queue occupancy), bit 31 is unused. The result 
       of 
               the comparison is effectively a 1 bit wide event, i.e., the counter will 
       be 
               incremented by 1 when the comparison is true (the type of comparison
               depends on the setting of the 'invert' bit - see bit 23 below) no matter 
       how 
               wide the original event was. When this field is zero, threshold 
       comparison is 
               disabled and the event is passed without modification.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG1_M2MEM_MAIN_STRUCT;


/* PMONCNTRCFG2_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240238)                                                  */
/*       SKX (0x40240238)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG2_M2MEM_MAIN_REG 0x06004238
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 *  Perfmon Counter Control Register.  
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'h0 
       
               Event Select (EventSelect):
               This field is used to decode the PerfMon event which is selected. 
               
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'h0 
       
               Unit Mask (UnitMask):
               This mask selects the sub-events to be selected for creation of the 
       event. 
               The selected sub-events are bitwise OR-ed together to create event. At 
       least 
               one sub-event must be selected otherwise the PerfMon event signals will 
       not 
               ever get asserted. Events with no sub-events listed effectively have 
       only one 
               sub-event; bit 8 must be set to 1 in this case.
               
     */
    UINT32 queoccupreset : 1;
    /* queoccupreset - Bits[16:16], WO, default = 1'h0 
       
               Queue Occupancy Reset (QueOccupReset):
               This write only bit causes the queue occupancy counter of the PerfMon
               counter for which this Perf event select register is associated to be 
       cleared to 
               all zeroes when a '1' is written to it. No action is taken when a '0' is 
       written. 
               Note: Since the queue occupancy counters never drop below zero, it is
               possible for the counters to 'catch up' with the real occupancy of the 
       queue 
               in question when the real occupancy drop to zero.
               
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'h0 
       
               Counter Reset (CounterReset):
               When this bit is set, the corresponding counter will be reset to 0. This 
       allows 
               for a quick reset of the counter when changing event encodings.
               
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'h0 
       
               Edge Detect (EdgeDetect):
               Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a 
       given 
               event. For example, we have an event that counts the number of cycles in
               L0s mode in Intel UPI. By using edge detect, one can count the number of 
       times 
               that we entered L0s mode (by detecting the rising edge).
       
               Edge detect only works in conjunction with threshholding. This is true 
       even 
               for events that can only increment by 1 in a given cycle (like the L0s
               example above). In this case, one should set a threshhold of 1. One can 
       also 
               use Edge Detect with queue occupancy events. For example, if one wanted
               to count the number of times when the TOR occupancy was larger than 5,
               one would selet the TOR occupancy event with a threshold of 5 and set 
       the 
               Edge Detect bit.
       
               Edge detect can also be used with the invert. This is generally not
               particularly useful, as the count of falling edges compared to rising 
       edges 
               will either be the same or differ by 1.
               
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'h0 
       
               Overflow Enable (OverflowEnable):
               Setting this bit will enable the counter to send an overflow signal. If 
       this bit 
               is not set, the counter will wrap around when it overflows without 
       triggering 
               anything. If this bit is set and the Unit's configuration register has 
       Overflow 
               enabled, then a signal will be transmitted to the Ubox.
               
     */
    UINT32 internal : 1;
    /* internal - Bits[21:21], RW_V, default = 1'h0 
       
               This bit needs to be asserted if the event which needs to be selected is 
       an 
               internal event. 
       
               Internal events are events that are only accessible on unlocked parts. 
       One 
               can basically think of the internal bit as an extension of the event 
       select bits 
               [7:0], as it allows the use to gain access to additional events. It will 
       not be 
               possible to use these events on locked parts. For perfmons that exist in 
       MSR 
               space, the MSR write mask is set on this bit, and the ucode handler will
               trigger a GP fault if one attempts to write to these bits. Writing to 
       these bits 
               is controlled in other ways in the other units.
               
     */
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'h0 
       
               Counter Enable (CounterEnable):
               This field is the local enable for the PerfMon Counter. This bit must be
               asserted in order for the PerfMon counter to begin counting the events
               selected by the 'event select', 'unit mask', and 'internal' bits (see 
       the fields 
               below). There is one bit per PerfMon Counter. Note that if this bit is 
       set to 1 
               but the Unit Control Registers have determined that counting is 
       disabled, 
               then the counter will not count.
               
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'h0 
       
               This bit indicates how the threshold field will be compared to the 
       incoming 
               event. When 0, the comparison that will be done is threshold >= event.
               When set to 1, the comparison that will be done is inverted from the 
       case 
               where this bit is set to 0, i.e., threshold < event. The invert bit only 
       works 
               when Threshhold != 0. So, if one would like to invert a non-occupancy 
       event 
               (like LLC Hit), one needs to set the threshhold to 1.
               
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'h0 
       
               This field is compared directly against an incoming event value for 
       events 
               that can increment by 1 or more in a given cycle. Since the widest event
               from the UnCore is 7bits (queue occupancy), bit 31 is unused. The result 
       of 
               the comparison is effectively a 1 bit wide event, i.e., the counter will 
       be 
               incremented by 1 when the comparison is true (the type of comparison
               depends on the setting of the 'invert' bit - see bit 23 below) no matter 
       how 
               wide the original event was. When this field is zero, threshold 
       comparison is 
               disabled and the event is passed without modification.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG2_M2MEM_MAIN_STRUCT;


/* PMONCNTRCFG3_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240240)                                                  */
/*       SKX (0x40240240)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG3_M2MEM_MAIN_REG 0x06004240
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 *  Perfmon Counter Control Register.  
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'h0 
       
               Event Select (EventSelect):
               This field is used to decode the PerfMon event which is selected. 
               
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'h0 
       
               Unit Mask (UnitMask):
               This mask selects the sub-events to be selected for creation of the 
       event. 
               The selected sub-events are bitwise OR-ed together to create event. At 
       least 
               one sub-event must be selected otherwise the PerfMon event signals will 
       not 
               ever get asserted. Events with no sub-events listed effectively have 
       only one 
               sub-event; bit 8 must be set to 1 in this case.
               
     */
    UINT32 queoccupreset : 1;
    /* queoccupreset - Bits[16:16], WO, default = 1'h0 
       
               Queue Occupancy Reset (QueOccupReset):
               This write only bit causes the queue occupancy counter of the PerfMon
               counter for which this Perf event select register is associated to be 
       cleared to 
               all zeroes when a '1' is written to it. No action is taken when a '0' is 
       written. 
               Note: Since the queue occupancy counters never drop below zero, it is
               possible for the counters to 'catch up' with the real occupancy of the 
       queue 
               in question when the real occupancy drop to zero.
               
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'h0 
       
               Counter Reset (CounterReset):
               When this bit is set, the corresponding counter will be reset to 0. This 
       allows 
               for a quick reset of the counter when changing event encodings.
               
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'h0 
       
               Edge Detect (EdgeDetect):
               Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a 
       given 
               event. For example, we have an event that counts the number of cycles in
               L0s mode in Intel UPI. By using edge detect, one can count the number of 
       times 
               that we entered L0s mode (by detecting the rising edge).
       
               Edge detect only works in conjunction with threshholding. This is true 
       even 
               for events that can only increment by 1 in a given cycle (like the L0s
               example above). In this case, one should set a threshhold of 1. One can 
       also 
               use Edge Detect with queue occupancy events. For example, if one wanted
               to count the number of times when the TOR occupancy was larger than 5,
               one would selet the TOR occupancy event with a threshold of 5 and set 
       the 
               Edge Detect bit.
       
               Edge detect can also be used with the invert. This is generally not
               particularly useful, as the count of falling edges compared to rising 
       edges 
               will either be the same or differ by 1.
               
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'h0 
       
               Overflow Enable (OverflowEnable):
               Setting this bit will enable the counter to send an overflow signal. If 
       this bit 
               is not set, the counter will wrap around when it overflows without 
       triggering 
               anything. If this bit is set and the Unit's configuration register has 
       Overflow 
               enabled, then a signal will be transmitted to the Ubox.
               
     */
    UINT32 internal : 1;
    /* internal - Bits[21:21], RW_V, default = 1'h0 
       
               This bit needs to be asserted if the event which needs to be selected is 
       an 
               internal event. 
       
               Internal events are events that are only accessible on unlocked parts. 
       One 
               can basically think of the internal bit as an extension of the event 
       select bits 
               [7:0], as it allows the use to gain access to additional events. It will 
       not be 
               possible to use these events on locked parts. For perfmons that exist in 
       MSR 
               space, the MSR write mask is set on this bit, and the ucode handler will
               trigger a GP fault if one attempts to write to these bits. Writing to 
       these bits 
               is controlled in other ways in the other units.
               
     */
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'h0 
       
               Counter Enable (CounterEnable):
               This field is the local enable for the PerfMon Counter. This bit must be
               asserted in order for the PerfMon counter to begin counting the events
               selected by the 'event select', 'unit mask', and 'internal' bits (see 
       the fields 
               below). There is one bit per PerfMon Counter. Note that if this bit is 
       set to 1 
               but the Unit Control Registers have determined that counting is 
       disabled, 
               then the counter will not count.
               
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'h0 
       
               This bit indicates how the threshold field will be compared to the 
       incoming 
               event. When 0, the comparison that will be done is threshold >= event.
               When set to 1, the comparison that will be done is inverted from the 
       case 
               where this bit is set to 0, i.e., threshold < event. The invert bit only 
       works 
               when Threshhold != 0. So, if one would like to invert a non-occupancy 
       event 
               (like LLC Hit), one needs to set the threshhold to 1.
               
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'h0 
       
               This field is compared directly against an incoming event value for 
       events 
               that can increment by 1 or more in a given cycle. Since the widest event
               from the UnCore is 7bits (queue occupancy), bit 31 is unused. The result 
       of 
               the comparison is effectively a 1 bit wide event, i.e., the counter will 
       be 
               incremented by 1 when the comparison is true (the type of comparison
               depends on the setting of the 'invert' bit - see bit 23 below) no matter 
       how 
               wide the original event was. When this field is zero, threshold 
       comparison is 
               disabled and the event is passed without modification.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG3_M2MEM_MAIN_STRUCT;


/* PMONCNTRCFG4_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240248)                                                  */
/*       SKX (0x40240248)                                                     */
/* Register default value:              0x00000000                            */
#define PMONCNTRCFG4_M2MEM_MAIN_REG 0x06004248
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 *  Perfmon Counter Control Register.  
 */
typedef union {
  struct {
    UINT32 eventselect : 8;
    /* eventselect - Bits[7:0], RW_V, default = 8'h0 
       
               Event Select (EventSelect):
               This field is used to decode the PerfMon event which is selected. 
               
     */
    UINT32 unitmask : 8;
    /* unitmask - Bits[15:8], RW_V, default = 8'h0 
       
               Unit Mask (UnitMask):
               This mask selects the sub-events to be selected for creation of the 
       event. 
               The selected sub-events are bitwise OR-ed together to create event. At 
       least 
               one sub-event must be selected otherwise the PerfMon event signals will 
       not 
               ever get asserted. Events with no sub-events listed effectively have 
       only one 
               sub-event; bit 8 must be set to 1 in this case.
               
     */
    UINT32 queoccupreset : 1;
    /* queoccupreset - Bits[16:16], WO, default = 1'h0 
       
               Queue Occupancy Reset (QueOccupReset):
               This write only bit causes the queue occupancy counter of the PerfMon
               counter for which this Perf event select register is associated to be 
       cleared to 
               all zeroes when a '1' is written to it. No action is taken when a '0' is 
       written. 
               Note: Since the queue occupancy counters never drop below zero, it is
               possible for the counters to 'catch up' with the real occupancy of the 
       queue 
               in question when the real occupancy drop to zero.
               
     */
    UINT32 counterreset : 1;
    /* counterreset - Bits[17:17], WO, default = 1'h0 
       
               Counter Reset (CounterReset):
               When this bit is set, the corresponding counter will be reset to 0. This 
       allows 
               for a quick reset of the counter when changing event encodings.
               
     */
    UINT32 edgedetect : 1;
    /* edgedetect - Bits[18:18], RW_V, default = 1'h0 
       
               Edge Detect (EdgeDetect):
               Edge Detect allows one to count either 0 to 1 or 1 to 0 transitions of a 
       given 
               event. For example, we have an event that counts the number of cycles in
               L0s mode in Intel UPI. By using edge detect, one can count the number of 
       times 
               that we entered L0s mode (by detecting the rising edge).
       
               Edge detect only works in conjunction with threshholding. This is true 
       even 
               for events that can only increment by 1 in a given cycle (like the L0s
               example above). In this case, one should set a threshhold of 1. One can 
       also 
               use Edge Detect with queue occupancy events. For example, if one wanted
               to count the number of times when the TOR occupancy was larger than 5,
               one would selet the TOR occupancy event with a threshold of 5 and set 
       the 
               Edge Detect bit.
       
               Edge detect can also be used with the invert. This is generally not
               particularly useful, as the count of falling edges compared to rising 
       edges 
               will either be the same or differ by 1.
               
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[20:20], RW_V, default = 1'h0 
       
               Overflow Enable (OverflowEnable):
               Setting this bit will enable the counter to send an overflow signal. If 
       this bit 
               is not set, the counter will wrap around when it overflows without 
       triggering 
               anything. If this bit is set and the Unit's configuration register has 
       Overflow 
               enabled, then a signal will be transmitted to the Ubox.
               
     */
    UINT32 internal : 1;
    /* internal - Bits[21:21], RW_V, default = 1'h0 
       
               This bit needs to be asserted if the event which needs to be selected is 
       an 
               internal event. 
       
               Internal events are events that are only accessible on unlocked parts. 
       One 
               can basically think of the internal bit as an extension of the event 
       select bits 
               [7:0], as it allows the use to gain access to additional events. It will 
       not be 
               possible to use these events on locked parts. For perfmons that exist in 
       MSR 
               space, the MSR write mask is set on this bit, and the ucode handler will
               trigger a GP fault if one attempts to write to these bits. Writing to 
       these bits 
               is controlled in other ways in the other units.
               
     */
    UINT32 counterenable : 1;
    /* counterenable - Bits[22:22], RW_V, default = 1'h0 
       
               Counter Enable (CounterEnable):
               This field is the local enable for the PerfMon Counter. This bit must be
               asserted in order for the PerfMon counter to begin counting the events
               selected by the 'event select', 'unit mask', and 'internal' bits (see 
       the fields 
               below). There is one bit per PerfMon Counter. Note that if this bit is 
       set to 1 
               but the Unit Control Registers have determined that counting is 
       disabled, 
               then the counter will not count.
               
     */
    UINT32 invert : 1;
    /* invert - Bits[23:23], RW_V, default = 1'h0 
       
               This bit indicates how the threshold field will be compared to the 
       incoming 
               event. When 0, the comparison that will be done is threshold >= event.
               When set to 1, the comparison that will be done is inverted from the 
       case 
               where this bit is set to 0, i.e., threshold < event. The invert bit only 
       works 
               when Threshhold != 0. So, if one would like to invert a non-occupancy 
       event 
               (like LLC Hit), one needs to set the threshhold to 1.
               
     */
    UINT32 threshold : 8;
    /* threshold - Bits[31:24], RW_V, default = 8'h0 
       
               This field is compared directly against an incoming event value for 
       events 
               that can increment by 1 or more in a given cycle. Since the widest event
               from the UnCore is 7bits (queue occupancy), bit 31 is unused. The result 
       of 
               the comparison is effectively a 1 bit wide event, i.e., the counter will 
       be 
               incremented by 1 when the comparison is true (the type of comparison
               depends on the setting of the 'invert' bit - see bit 23 below) no matter 
       how 
               wide the original event was. When this field is zero, threshold 
       comparison is 
               disabled and the event is passed without modification.
               
     */
  } Bits;
  UINT32 Data;
} PMONCNTRCFG4_M2MEM_MAIN_STRUCT;


/* PMONUNITCTRL_M2MEM_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x40240258)                                                  */
/*       SKX (0x40240258)                                                     */
/* Register default value:              0x00030000                            */
#define PMONUNITCTRL_M2MEM_MAIN_REG 0x06004258
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 resetcounterconfigs : 1;
    /* resetcounterconfigs - Bits[0:0], WO, default = 1'h0 
       
               Reset Counter Configs (ResetCounterConfigs):
               When this bit is written to, the counter configuration registers will be 
       reset. 
               This does not effect the values in the counters. To reset the counters, 
       this 
               bit need only be set by one of the unit control registers.
               
     */
    UINT32 resetcounters : 1;
    /* resetcounters - Bits[1:1], WO, default = 1'h0 
       
               Reset Counters (ResetCounters):
               When this bit is written to, the counters data fields will be reset. The
               configuration values will not be reset. To reset the counters, this bit 
       need 
               only be set by one of the unit control registers.
               
     */
    UINT32 rsvd_2 : 6;
    /* rsvd_2 - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 freezecounters : 1;
    /* freezecounters - Bits[8:8], RW_V, default = 1'h0 
       
               Freeze Counters (FreezeCounters):
               This bit is written to when the counters should be frozen. If this bit 
       is written 
               to and freeze is enabled, the counters in the unit will stop counting. 
       To 
               freeze the counters, this bit need only be set by one of the unit 
       control 
               registers.
               
     */
    UINT32 rsvd_9 : 7;
    /* rsvd_9 - Bits[15:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 freezeenable : 1;
    /* freezeenable - Bits[16:16], RW_L, default = 1'h1 
       
               Freeze Enable (FreezeEnable):
               This bit controls what the counters in the unit will do when they 
       receive a 
               freeze signal. When set, the counters will be allowed to freeze. When 
       not 
               set, the counters will ignore the freeze signal. For freeze to be 
       enabled for a 
               given unit, all of the unit control registers must have this bit set.
               Lock bit: Mesh2Mem DfxLockCntl[DfxMiscLock].
               
     */
    UINT32 overflowenable : 1;
    /* overflowenable - Bits[17:17], RW_L, default = 1'h1 
       
               Overflow Enable (OverflowEnable):
               This bit controls the behavior of counters when they overflow. When set, 
       the 
               system will trigger the overflow handling process throughout the rest of 
       the 
               uncore, potentially triggering a PMI and freezing counters. When it is 
       not 
               set, the counters will simply wrap around and continue to count. For
               overflow to be enabled for a given unit, all of the unit control 
       registers must 
               have this bit set.
               Lock bit: Mesh2Mem DfxLockCntl[DfxMiscLock].
               
     */
    UINT32 rsvd_18 : 14;
    /* rsvd_18 - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONUNITCTRL_M2MEM_MAIN_STRUCT;


/* PMONUNITSTATUS_M2MEM_MAIN_REG supported on:                                */
/*       SKX_A0 (0x40240260)                                                  */
/*       SKX (0x40240260)                                                     */
/* Register default value:              0x00000000                            */
#define PMONUNITSTATUS_M2MEM_MAIN_REG 0x06004260
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 counteroverflowbitmask : 5;
    /* counteroverflowbitmask - Bits[4:0], RW1C, default = 4'h0 
       
               Counter Overflow Bitmask (CounterOverflowBitmask):
               This is a bitmask that specifies which counter (or counters) have 
       overflowed. 
               
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONUNITSTATUS_M2MEM_MAIN_STRUCT;


/* PMONADDRMATCH0_M2MEM_MAIN_REG supported on:                                */
/*       SKX_A0 (0x40240268)                                                  */
/*       SKX (0x40240268)                                                     */
/* Register default value:              0x00000000                            */
#define PMONADDRMATCH0_M2MEM_MAIN_REG 0x06004268
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Address match register: For each address bit which is not masked out, 
 * compare the incoming  
 *       (BL/AD) physical transaction address from the mesh and generate an event 
 * if a match.  
 *       This event can then be used as a pmon event or a debug trigger event.
 *       
 */
typedef union {
  struct {
    UINT32 rsvd : 6;
    /* rsvd - Bits[5:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lophyaddr : 26;
    /* lophyaddr - Bits[31:6], RWS, default = 26'h0 
       
               Lower Bits of the Physical Address (LoPhyAddr):
               Low physical address bits [31:6] to match on. Only those bits for which 
       the  
               corresponding maskout bit is zero will be compared.
               
     */
  } Bits;
  UINT32 Data;
} PMONADDRMATCH0_M2MEM_MAIN_STRUCT;


/* PMONADDRMATCH1_M2MEM_MAIN_REG supported on:                                */
/*       SKX_A0 (0x4024026C)                                                  */
/*       SKX (0x4024026C)                                                     */
/* Register default value:              0x00000000                            */
#define PMONADDRMATCH1_M2MEM_MAIN_REG 0x0600426C
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Address match register: For each address bit which is not masked out, 
 * compare the incoming  
 *       (BL/AD) transaction address from the mesh and generate an event. This 
 * event can be selected as a pmon  
 *       event, a debug trigger event or as a trace filtering event.
 *       
 */
typedef union {
  struct {
    UINT32 hiphyaddr : 14;
    /* hiphyaddr - Bits[13:0], RWS, default = 14'h0 
       
               High Bits of the Physical Address (HiPhyAddr):
               High physical address bits [45:32] to match on. Only those bits for 
       which the  
               corresponding maskout bit is zero will be compared.
               
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONADDRMATCH1_M2MEM_MAIN_STRUCT;


/* PMONADDRMASK0_M2MEM_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x40240270)                                                  */
/*       SKX (0x40240270)                                                     */
/* Register default value:              0x00000000                            */
#define PMONADDRMASK0_M2MEM_MAIN_REG 0x06004270
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Maskout bits used in conjunction with the address match register.
 *       
 */
typedef union {
  struct {
    UINT32 rsvd : 6;
    /* rsvd - Bits[5:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lophyaddrmask : 26;
    /* lophyaddrmask - Bits[31:6], RWS, default = 26'h0 
       
               Lower Maskout Bits of the Physical Address (LoPhyAddrMask):
               Maskout bits for physical address bits [31:6].
               When a maskout bit is set, the corresponding address match bit will be 
       masked out, i.e. will  
               not be compared against.
               
     */
  } Bits;
  UINT32 Data;
} PMONADDRMASK0_M2MEM_MAIN_STRUCT;


/* PMONADDRMASK1_M2MEM_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x40240274)                                                  */
/*       SKX (0x40240274)                                                     */
/* Register default value:              0x00000000                            */
#define PMONADDRMASK1_M2MEM_MAIN_REG 0x06004274
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Maskout bits used in conjunction with the address match register.
 *       
 */
typedef union {
  struct {
    UINT32 hiphyaddrmask : 14;
    /* hiphyaddrmask - Bits[13:0], RWS, default = 14'h0 
       
               High Maskout Bits of the Physical Address (HiPhyAddrMask):
               Maskout bits for physical address bits [45:32].
               When a maskout bit is set, the corresponding address match bit will be 
       masked out, i.e. will  
               not be compared against.
               
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONADDRMASK1_M2MEM_MAIN_STRUCT;


/* PMONOPCODEMASKMATCH_M2MEM_MAIN_REG supported on:                           */
/*       SKX_A0 (0x40240278)                                                  */
/*       SKX (0x40240278)                                                     */
/* Register default value:              0x000000F0                            */
#define PMONOPCODEMASKMATCH_M2MEM_MAIN_REG 0x06004278
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *       Opcode and message class mask/match register. For each opcode and message 
 * class bit which  
 *       is not masked out, compare the incoming (BL/AD) physical transaction 
 * address from the mesh  
 *       and generate an event if a match. This event can then be used as a pmon 
 * event or a debug trigger event. 
 *       
 */
typedef union {
  struct {
    UINT32 opcode : 4;
    /* opcode - Bits[3:0], RWS, default = 4'h0 
       
               Opcode:
               Opcode to match on of the incoming transaction from mesh.
               
     */
    UINT32 opcodemask : 4;
    /* opcodemask - Bits[7:4], RWS, default = 4'hf 
       
               Opcode Mask (OpcodeMask):
               Mask-out bits for the opcode match. When a maskout bit is set, 
               the corresponding opcode match bit will be masked out, i.e. will 
               not be compared against.
               
     */
    UINT32 msgclass : 4;
    /* msgclass - Bits[11:8], RWS, default = 4'h0 
       
               Message Class (MsgClass):
               Message class to match on in the incoming transaction from mesh.
               Message class encodings:
                 4'b0000: REQ (AD)
                 4'b0010: RSP (AD)
                 4'b1010: NCB (BL)
                 4'b1011: NCS (BL)
                 4'b1100: WB  (BL)
               
     */
    UINT32 msgclassmask : 4;
    /* msgclassmask - Bits[15:12], RWS, default = 4'h0 
       
               Message Class Mask (MsgClassMask):
               Mask-out bits for the message class match. When a maskout bit is set, 
               the corresponding message match bit will be masked out, i.e. will 
               not be compared against.
               
     */
    UINT32 mcopcode : 6;
    /* mcopcode - Bits[21:16], RWS, default = 6'h0 
       
               MC Opcode:
               Opcode to match on of the outgoing transaction to MC.
               The opcode encoding follows the MC encodings from the MC MAS.
               
     */
    UINT32 mcopcodemask : 6;
    /* mcopcodemask - Bits[27:22], RWS, default = 6'h0 
       
               MC Opcode Mask (MCOpcodeMask):
               Mask-out bits for the MC opcode match. When a maskout bit is set, 
               the corresponding opcode match bit will be masked out, i.e. will 
               not be compared against.
               
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PMONOPCODEMASKMATCH_M2MEM_MAIN_STRUCT;










/* ASC0LDVALHI_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402402A8)                                                  */
/*       SKX (0x402402A8)                                                     */
/* Register default value:              0x00000000                            */
#define ASC0LDVALHI_M2MEM_MAIN_REG 0x060042A8
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *     This register specifies the value to be loaded in Anti-Starvation Counter 0 
 * when the ASC state is 1. 
 *     The register can be locked by lock bit DFXAscLock in DfxLockCtl register. 
 * The register may be 
 *     readable with the lock bit set but no writes will take effect unless the 
 * lock bit is set to 0. 
 *     
 */
typedef union {
  struct {
    UINT32 hivalue : 32;
    /* hivalue - Bits[31:0], RWS_L, default = 32'h00000000 
       Value to be loaded in ASC counter 0 when ASC state is 1.
     */
  } Bits;
  UINT32 Data;
} ASC0LDVALHI_M2MEM_MAIN_STRUCT;


/* ASC1LDVALHI_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402402AC)                                                  */
/*       SKX (0x402402AC)                                                     */
/* Register default value:              0x00000000                            */
#define ASC1LDVALHI_M2MEM_MAIN_REG 0x060042AC
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *     This register specifies the value to be loaded in Anti-Starvation Counter 1 
 * when the ASC state is 1. 
 *     The register can be locked by lock bit DFXAscLock in DfxLockCtl register. 
 * The register may be 
 *     readable with the lock bit set but no writes will take effect unless the 
 * lock bit is set to 0. 
 *     
 */
typedef union {
  struct {
    UINT32 hivalue : 32;
    /* hivalue - Bits[31:0], RWS_L, default = 32'h00000000 
       Value to be loaded in ASC counter 1 when ASC state is 1.
     */
  } Bits;
  UINT32 Data;
} ASC1LDVALHI_M2MEM_MAIN_STRUCT;


/* ASC0LDVALLO_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402402B0)                                                  */
/*       SKX (0x402402B0)                                                     */
/* Register default value:              0x00000000                            */
#define ASC0LDVALLO_M2MEM_MAIN_REG 0x060042B0
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *     This register specifies the value to be loaded in Anti-Starvation Counter 0 
 * when the ASC state is 0. 
 *     The register can be locked by lock bit DFXAscLock in DfxLockCtl register. 
 * The register may be 
 *     readable with the lock bit set but no writes will take effect unless the 
 * lock bit is set to 0. 
 *     
 */
typedef union {
  struct {
    UINT32 lovalue : 32;
    /* lovalue - Bits[31:0], RWS_L, default = 32'h00000000 
       Value to be loaded in ASC counter 0 when ASC state is 0.
     */
  } Bits;
  UINT32 Data;
} ASC0LDVALLO_M2MEM_MAIN_STRUCT;


/* ASC1LDVALLO_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402402B4)                                                  */
/*       SKX (0x402402B4)                                                     */
/* Register default value:              0x00000000                            */
#define ASC1LDVALLO_M2MEM_MAIN_REG 0x060042B4
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *     This register specifies the value to be loaded in Anti-Starvation Counter 1 
 * when the ASC state is 0. 
 *     The register can be locked by lock bit DFXAscLock in DfxLockCtl register. 
 * The register may be 
 *     readable with the lock bit set but no writes will take effect unless the 
 * lock bit is set to 0. 
 *     
 */
typedef union {
  struct {
    UINT32 lovalue : 32;
    /* lovalue - Bits[31:0], RWS_L, default = 32'h00000000 
       Value to be loaded in ASC counter 1 when ASC state is 1.
     */
  } Bits;
  UINT32 Data;
} ASC1LDVALLO_M2MEM_MAIN_STRUCT;












/* ASC2LDVALHI_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402402CC)                                                  */
/*       SKX (0x402402CC)                                                     */
/* Register default value:              0x00000000                            */
#define ASC2LDVALHI_M2MEM_MAIN_REG 0x060042CC
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *     This register specifies the value to be loaded in Anti-Starvation Counter 2 
 * when the ASC state is 1. 
 *     The register can be locked by lock bit DFXAscLock in DfxLockCtl register. 
 * The register may be 
 *     readable with the lock bit set but no writes will take effect unless the 
 * lock bit is set to 0. 
 *     
 */
typedef union {
  struct {
    UINT32 hivalue : 32;
    /* hivalue - Bits[31:0], RWS_L, default = 32'h00000000 
       Value to be loaded in ASC counter 2 when ASC state is 1.
     */
  } Bits;
  UINT32 Data;
} ASC2LDVALHI_M2MEM_MAIN_STRUCT;


/* ASC2LDVALLO_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x402402D0)                                                  */
/*       SKX (0x402402D0)                                                     */
/* Register default value:              0x00000000                            */
#define ASC2LDVALLO_M2MEM_MAIN_REG 0x060042D0
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *     This register specifies the value to be loaded in Anti-Starvation Counter 2 
 * when the ASC state is 0. 
 *     The register can be locked by lock bit DFXAscLock in DfxLockCtl register. 
 * The register may be 
 *     readable with the lock bit set but no writes will take effect unless the 
 * lock bit is set to 0. 
 *     
 */
typedef union {
  struct {
    UINT32 lovalue : 32;
    /* lovalue - Bits[31:0], RWS_L, default = 32'h00000000 
       Value to be loaded in ASC counter 2 when ASC state is 1.
     */
  } Bits;
  UINT32 Data;
} ASC2LDVALLO_M2MEM_MAIN_STRUCT;


/* SCRATCHPAD_CFG_EGR_M2MEM_MAIN_REG supported on:                            */
/*       SKX_A0 (0x40240FDC)                                                  */
/*       SKX (0x40240FDC)                                                     */
/* Register default value:              0x00000000                            */
#define SCRATCHPAD_CFG_EGR_M2MEM_MAIN_REG 0x06004FDC
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * dummy register
 */
typedef union {
  struct {
    UINT32 bit_field_rws_v : 16;
    /* bit_field_rws_v - Bits[15:0], RWS_V, default = 16'b0000000000000000 
       scratchpad
     */
    UINT32 bit_field_rw_v : 16;
    /* bit_field_rw_v - Bits[31:16], RW_V, default = 16'b0000000000000000 
       scratchpad
     */
  } Bits;
  UINT32 Data;
} SCRATCHPAD_CFG_EGR_M2MEM_MAIN_STRUCT;


/* SCRATCHPAD_CFG_ING_M2MEM_MAIN_REG supported on:                            */
/*       SKX_A0 (0x40240FE4)                                                  */
/*       SKX (0x40240FE4)                                                     */
/* Register default value:              0x00000000                            */
#define SCRATCHPAD_CFG_ING_M2MEM_MAIN_REG 0x06004FE4
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * dummy register
 */
typedef union {
  struct {
    UINT32 bit_field_rws_v : 16;
    /* bit_field_rws_v - Bits[15:0], RWS_V, default = 16'b0000000000000000 
       scratchpad
     */
    UINT32 bit_field_rw_v : 16;
    /* bit_field_rw_v - Bits[31:16], RW_V, default = 16'b0000000000000000 
       scratchpad
     */
  } Bits;
  UINT32 Data;
} SCRATCHPAD_CFG_ING_M2MEM_MAIN_STRUCT;


/* SCRATCHPAD_CFG_BGF_M2MEM_MAIN_REG supported on:                            */
/*       SKX_A0 (0x40240FE8)                                                  */
/*       SKX (0x40240FE8)                                                     */
/* Register default value:              0x00000000                            */
#define SCRATCHPAD_CFG_BGF_M2MEM_MAIN_REG 0x06004FE8
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * dummy register
 */
typedef union {
  struct {
    UINT32 bit_field_rws_v : 16;
    /* bit_field_rws_v - Bits[15:0], RWS_V, default = 16'b0000000000000000 
       scratchpad
     */
    UINT32 bit_field_rw_v : 16;
    /* bit_field_rw_v - Bits[31:16], RW_V, default = 16'b0000000000000000 
       scratchpad
     */
  } Bits;
  UINT32 Data;
} SCRATCHPAD_CFG_BGF_M2MEM_MAIN_STRUCT;


/* WRTRKRALLOC_M2MEM_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x40240FF0)                                                  */
/*       SKX (0x40240FF0)                                                     */
/* Register default value on SKX_A0:    0x000B600C                            */
/* Register default value on SKX:       0x0DEB600C                            */
#define WRTRKRALLOC_M2MEM_MAIN_REG 0x06004FF0
/* Struct format extracted from XML file SKX_A0\2.8.0.CFG.xml.
 * 
 *          Hooks for controlling the reservation of store and mirror tracker 
 * entries for anti-deadlock purposes. 
 *       
 */
typedef union {
  struct {
    UINT32 sttrkrpwrthresh : 5;
    /* sttrkrpwrthresh - Bits[4:0], RW_LB, default = 5'd12 
       
                 Max number of partial writes allowed per channel store tracker. Must 
       be less than 14. 
                 A value of 0 disables this check.
               
     */
    UINT32 rsvd_5 : 5;
    /* rsvd_5 - Bits[9:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mirrtrkrthresh : 5;
    /* mirrtrkrthresh - Bits[14:10], RW_LB, default = 5'd24 
       
                 Max number of non-TGR mirror writes allowed. Should be less than 32 - 
       {BL transgress credits (set via Credits CSR)}. 
                 A value of 0 disables this check.
               
     */
    UINT32 mirrtrkrpwrthresh : 5;
    /* mirrtrkrpwrthresh - Bits[19:15], RW_LB, default = 5'd22 
       
                 Max number of partial mirror writes allowed. Should be less than 
       MirrTrkrThresh. 
                 A value of 0 disables this check.
               
     */
    UINT32 rsvd_20 : 12;
    /* rsvd_20 - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} WRTRKRALLOC_M2MEM_MAIN_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * 
 *          Hooks for controlling the reservation of store and mirror tracker 
 * entries for anti-deadlock purposes. 
 *       
 */
typedef union {
  struct {
    UINT32 sttrkrpwrthresh : 5;
    /* sttrkrpwrthresh - Bits[4:0], RW_LB, default = 5'd12 
       
                 Max number of partial writes allowed per channel store tracker. Must 
       be less than 14. 
                 A value of 0 disables this check.
               
     */
    UINT32 rsvd_5 : 5;
    /* rsvd_5 - Bits[9:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mirrtrkrthresh : 5;
    /* mirrtrkrthresh - Bits[14:10], RW_LB, default = 5'd24 
       
                 Max number of non-TGR mirror writes allowed. Should be less than 30 - 
       {BL transgress credits (set via Credits CSR)}. 
                 A value of 0 disables this check.
               
     */
    UINT32 mirrtrkrpwrthresh : 5;
    /* mirrtrkrpwrthresh - Bits[19:15], RW_LB, default = 5'd22 
       
                 Max number of partial mirror writes allowed. Should be less than 
       MirrTrkrThresh. 
                 A value of 0 disables this check.
               
     */
    UINT32 akwrcmphysthi : 4;
    /* akwrcmphysthi - Bits[23:20], RW_LB, default = 4'he 
       
                 Throttle BL ingress if the AK write completion egress occupancy 
       exceeds 4 * the programmed value. 
                 A value of 0 disables this check.
               
     */
    UINT32 akwrcmphystlo : 4;
    /* akwrcmphystlo - Bits[27:24], RW_LB, default = 4'hd 
       
                 Stop throttling BL ingress if the AK write completion egress occupancy 
       goes below 4 * the programmed value. 
                 A value of 0 disables this check.
               
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} WRTRKRALLOC_M2MEM_MAIN_STRUCT;



/* SCRATCHPAD_CFG_M2MEM_MAIN_REG supported on:                                */
/*       SKX_A0 (0x40240FFC)                                                  */
/*       SKX (0x40240FFC)                                                     */
/* Register default value:              0x00000000                            */
#define SCRATCHPAD_CFG_M2MEM_MAIN_REG 0x06004FFC
/* Struct format extracted from XML file SKX\2.8.0.CFG.xml.
 * TBD Dummy register so that regressions don't fail
 */
typedef union {
  struct {
    UINT32 tbd : 32;
    /* tbd - Bits[31:0], RW, default = 32'b0 
       R.
     */
  } Bits;
  UINT32 Data;
} SCRATCHPAD_CFG_M2MEM_MAIN_STRUCT;


#endif /* M2MEM_MAIN_h */
