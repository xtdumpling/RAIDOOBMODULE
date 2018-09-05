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

#ifndef MC_2LM_h
#define MC_2LM_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* MC_2LM_DEV 10                                                              */
/* MC_2LM_FUN 1                                                               */

/* VID_MC_2LM_REG supported on:                                               */
/*       SKX_A0 (0x20251000)                                                  */
/*       SKX (0x20251000)                                                     */
/* Register default value:              0x8086                                */
#define VID_MC_2LM_REG 0x07012000
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} VID_MC_2LM_STRUCT;


/* DID_MC_2LM_REG supported on:                                               */
/*       SKX_A0 (0x20251002)                                                  */
/*       SKX (0x20251002)                                                     */
/* Register default value:              0x2041                                */
#define DID_MC_2LM_REG 0x07012002
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2041 
        
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
} DID_MC_2LM_STRUCT;


/* PCICMD_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x20251004)                                                  */
/*       SKX (0x20251004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_MC_2LM_REG 0x07012004
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} PCICMD_MC_2LM_STRUCT;


/* PCISTS_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x20251006)                                                  */
/*       SKX (0x20251006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_MC_2LM_REG 0x07012006
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} PCISTS_MC_2LM_STRUCT;


/* RID_MC_2LM_REG supported on:                                               */
/*       SKX_A0 (0x10251008)                                                  */
/*       SKX (0x10251008)                                                     */
/* Register default value:              0x00                                  */
#define RID_MC_2LM_REG 0x07011008
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} RID_MC_2LM_STRUCT;


/* CCR_N0_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x10251009)                                                  */
/*       SKX (0x10251009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_MC_2LM_REG 0x07011009
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_MC_2LM_STRUCT;


/* CCR_N1_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x2025100A)                                                  */
/*       SKX (0x2025100A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_MC_2LM_REG 0x0701200A
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} CCR_N1_MC_2LM_STRUCT;


/* CLSR_MC_2LM_REG supported on:                                              */
/*       SKX_A0 (0x1025100C)                                                  */
/*       SKX (0x1025100C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_MC_2LM_REG 0x0701100C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} CLSR_MC_2LM_STRUCT;


/* PLAT_MC_2LM_REG supported on:                                              */
/*       SKX_A0 (0x1025100D)                                                  */
/*       SKX (0x1025100D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_MC_2LM_REG 0x0701100D
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} PLAT_MC_2LM_STRUCT;


/* HDR_MC_2LM_REG supported on:                                               */
/*       SKX_A0 (0x1025100E)                                                  */
/*       SKX (0x1025100E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_MC_2LM_REG 0x0701100E
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} HDR_MC_2LM_STRUCT;


/* BIST_MC_2LM_REG supported on:                                              */
/*       SKX_A0 (0x1025100F)                                                  */
/*       SKX (0x1025100F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_MC_2LM_REG 0x0701100F
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} BIST_MC_2LM_STRUCT;


/* SVID_MC_2LM_REG supported on:                                              */
/*       SKX_A0 (0x2025102C)                                                  */
/*       SKX (0x2025102C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_MC_2LM_REG 0x0701202C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} SVID_MC_2LM_STRUCT;


/* SDID_MC_2LM_REG supported on:                                              */
/*       SKX_A0 (0x2025102E)                                                  */
/*       SKX (0x2025102E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_MC_2LM_REG 0x0701202E
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} SDID_MC_2LM_STRUCT;


/* CAPPTR_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x10251034)                                                  */
/*       SKX (0x10251034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_MC_2LM_REG 0x07011034
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} CAPPTR_MC_2LM_STRUCT;


/* INTL_MC_2LM_REG supported on:                                              */
/*       SKX_A0 (0x1025103C)                                                  */
/*       SKX (0x1025103C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_MC_2LM_REG 0x0701103C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} INTL_MC_2LM_STRUCT;


/* INTPIN_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x1025103D)                                                  */
/*       SKX (0x1025103D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_MC_2LM_REG 0x0701103D
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} INTPIN_MC_2LM_STRUCT;


/* MINGNT_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x1025103E)                                                  */
/*       SKX (0x1025103E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_MC_2LM_REG 0x0701103E
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} MINGNT_MC_2LM_STRUCT;


/* MAXLAT_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x1025103F)                                                  */
/*       SKX (0x1025103F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_MC_2LM_REG 0x0701103F
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} MAXLAT_MC_2LM_STRUCT;


/* PXPCAP_MC_2LM_REG supported on:                                            */
/*       SKX_A0 (0x40251040)                                                  */
/*       SKX (0x40251040)                                                     */
/* Register default value:              0x00910010                            */
#define PXPCAP_MC_2LM_REG 0x07014040
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} PXPCAP_MC_2LM_STRUCT;


/* MCNMCACHINGCFG2_MC_2LM_REG supported on:                                   */
/*       SKX_A0 (0x40251084)                                                  */
/*       SKX (0x40251084)                                                     */
/* Register default value:              0x00000000                            */
#define MCNMCACHINGCFG2_MC_2LM_REG 0x07014084
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * MC NMCaching Configuration Register.  A related register - McNMCachingCfg2 - is 
 * per channel and is located in MCMAINS block. 
 */
typedef union {
  struct {
    UINT32 mcnmcachingnmchncap : 4;
    /* mcnmcachingnmchncap - Bits[3:0], RW_LB, default = 4'h0 
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
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_8 : 1;
    UINT32 rsvd_9 : 7;
    /* rsvd_9 - Bits[15:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_16 : 1;
    UINT32 rsvd_17 : 15;
    /* rsvd_17 - Bits[31:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCNMCACHINGCFG2_MC_2LM_STRUCT;


/* MCNMCACHINGINTLV_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251088)                                                  */
/*       SKX (0x40251088)                                                     */
/* Register default value:              0x00000000                            */
#define MCNMCACHINGINTLV_MC_2LM_REG 0x07014088
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * MC NMCaching Configuration Register
 */
typedef union {
  struct {
    UINT32 mcnmcaching_tad_il_granular_ximc : 1;
    /* mcnmcaching_tad_il_granular_ximc - Bits[0:0], RW_LB, default = 1'b0 
       If set, strip out bit 8 for 256b interleaving between near-memory channels in a 
       socket. This is only relevant if there are only 2 channels of NM behind the MC. 
       This field is programmed on the per channel basis, i.e. need to maintain 
       consistency with other populated channels in this iMC. 
     */
    UINT32 mcnmcachingchanintlvval : 1;
    /* mcnmcachingchanintlvval - Bits[1:1], RW_LB, default = 1'b0 
       The value of bit 8 for this channel. Used for reverse address decode. This field 
       is programmed on the per channel basis. 
     */
    UINT32 mcnmcachingsocketintlv : 2;
    /* mcnmcachingsocketintlv - Bits[3:2], RW_LB, default = 2'b00 
       4kb Socket interleave ways:
       2'b00 - no socket interleaving
       2'b01 - 2 way interleave: strip out bit 12 from index
       2'b10 - 4 way interleave: strip out bits 13:12 from index
       2'b11 - 8 way interleave: strip out bits 14:12 from index
       This field is programmed on the per iMC basis, i.e. need to maintain consistency 
       with other populated channels in this iMC. 
     */
    UINT32 mcnmcachingsocketintlvval : 3;
    /* mcnmcachingsocketintlvval - Bits[6:4], RW_LB, default = 3'b000 
       The value of bits 14:12 for this MC. Used for reverse address decode. This field 
       is programmed on the per iMC basis, i.e. need to maintain consistency with other 
       populated channels in this iMC. 
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCNMCACHINGINTLV_MC_2LM_STRUCT;


/* SYSFEATURES0_MC_2LM_REG supported on:                                      */
/*       SKX_A0 (0x4025108C)                                                  */
/*       SKX (0x4025108C)                                                     */
/* Register default value:              0x00000000                            */
#define SYSFEATURES0_MC_2LM_REG 0x0701408C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * 
 *       This register contains MC features which BIOS might need to set 
 * differently based on  
 *       the system topology/configuration or for performance/power reasons.
 * 
 *       A single socket system is expected to at least configure following 
 * (different from reset values): 
 *         CHA HA_COH_CFG[Dis_Directory] = 1 (functionality)
 *         M2M SysFeatures[FrcDirI]      = 1 (functionality)
 *         M2M SysFeatures[DirWrDisable] = 1 (performance)
 * 
 *       A multi-socket directory disabled system is expected to at least configure 
 * following (different from reset values): 
 *         CHA HA_COH_CFG[Dis_Directory] = 1 (functionality)
 *         M2M SysFeatures[DirWrDisable] = 1 (performance)
 * 
 *       These CSRs mirror SysFeatures0 in M2M and must be set consistently with 
 * that. 
 *       
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 3;
    /* rsvd_0 - Bits[2:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 frcpmemdirupdate2ddrt : 1;
    /* frcpmemdirupdate2ddrt - Bits[3:3], RW_LB, default = 1'd0 
       
               Force NM Pmem dir updates to DDR-T (FrcPmemDirUpdate2ddrt):
               By default, for multi-socket performance reasons, directory only updates 
       in NM for PMem Write-Through regions are not propagated to DDR-T. 
                 0: Dont propagate PMem NM dir-only updates to DDR-T (default).
                 1: Force propagation of NM PMem dir updates to DDR-T (debug).
               This bit is only relevant in PMemCaching mode (Mode.PmemCaching==1).
               
     */
    UINT32 dirwrdisable : 1;
    /* dirwrdisable - Bits[4:4], RW_LB, default = 1'd0 
       
               Must be set consistently with SysFeatures0.DirWrDisable in M2Mem.
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
       
               Must be set consistently with SysFeatures0.FrcDirI in M2Mem.
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
       
               Must be set consistently with SysFeatures0.MapDirStoDirA in M2Mem.
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
    UINT32 reconstructaddronhit : 1;
    /* reconstructaddronhit - Bits[7:7], RW_LB, default = 1'd0 
       
               Reconstruct the fill/evict address on all read returns, not just on a 
       miss. 
                 0: only reconstruct the address on a miss
                 1: reconstruct the address on all read returns (for debug)
               
     */
    UINT32 rsvd_8 : 24;
    /* rsvd_8 - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SYSFEATURES0_MC_2LM_STRUCT;


/* MCNMCACHINGOFFSET0_MC_2LM_REG supported on:                                */
/*       SKX_A0 (0x40251090)                                                  */
/*       SKX (0x40251090)                                                     */
/* Register default value:              0x00000000                            */
#define MCNMCACHINGOFFSET0_MC_2LM_REG 0x07014090
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * MC NMCaching Configuration Register 0.
 */
typedef union {
  struct {
    UINT32 mcnmcachingoffset0 : 16;
    /* mcnmcachingoffset0 - Bits[15:0], RW_LB, default = 16'h0000 
       This is the base address of the lower cacheable region from bits 47:32. This 
       field is programmed on the per iMC basis, i.e. need to maintain consistency with 
       other populated channels in this iMC. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MCNMCACHINGOFFSET0_MC_2LM_STRUCT;




/* SCRATCHPAD_2LMCNTL_MC_2LM_REG supported on:                                */
/*       SKX_A0 (0x402510FC)                                                  */
/*       SKX (0x402510FC)                                                     */
/* Register default value:              0x00000000                            */
#define SCRATCHPAD_2LMCNTL_MC_2LM_REG 0x070140FC
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * dummy register
 */
typedef union {
  struct {
    UINT32 bit_field : 32;
    /* bit_field - Bits[31:0], RWS_V, default = 32'b00000000000000000000000000000000 
       Reserved bits for ECOs.
                 Bit  0 - when set, DDR4 data returns in 1LM AG2 will be treated as 
       transgress returns. 
                 Bit  1 - when set, SB will signal that it is empty when no request is 
       allocated. When clear, SB will signal that it is empty when no non-patrol 
       request is allocated. 
                 Bit 22 - when set, DeallocAll commands from M2M will deallocate 
       patrols from the SB. 
               
     */
  } Bits;
  UINT32 Data;
} SCRATCHPAD_2LMCNTL_MC_2LM_STRUCT;


/* PXPENHCAP_MC_2LM_REG supported on:                                         */
/*       SKX_A0 (0x40251100)                                                  */
/*       SKX (0x40251100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_MC_2LM_REG 0x07014100
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
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
} PXPENHCAP_MC_2LM_STRUCT;


/* SBVIRALCTL_MC_2LM_REG supported on:                                        */
/*       SKX_A0 (0x40251104)                                                  */
/*       SKX (0x40251104)                                                     */
/* Register default value:              0x00000000                            */
#define SBVIRALCTL_MC_2LM_REG 0x07014104
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Enables for the SB viral triggering
 */
typedef union {
  struct {
    UINT32 reqerrdis : 1;
    /* reqerrdis - Bits[0:0], RWS, default = 1'b0 
       Disable viral triggering of bad request errors
     */
    UINT32 readrspmisserrdis : 1;
    /* readrspmisserrdis - Bits[1:1], RWS, default = 1'b0 
       Disable viral triggering of read data response miss errors
     */
    UINT32 ddr4cmperrdis : 1;
    /* ddr4cmperrdis - Bits[2:2], RWS, default = 1'b0 
       Disable viral triggering of DDR4 completion errors
     */
    UINT32 ddrtcmperrdis : 1;
    /* ddrtcmperrdis - Bits[3:3], RWS, default = 1'b0 
       Disable viral triggering of DDRt completion errors
     */
    UINT32 readrspopcodeerrdis : 1;
    /* readrspopcodeerrdis - Bits[4:4], RWS, default = 1'b0 
       Disable viral triggering of read data response opcode errors
     */
    UINT32 cmplfifooferrdis : 1;
    /* cmplfifooferrdis - Bits[5:5], RWS, default = 1'b0 
       Disable viral triggering of completion FIFO overflows
     */
    UINT32 cmplfifouferrdis : 1;
    /* cmplfifouferrdis - Bits[6:6], RWS, default = 1'b0 
       Disable viral triggering of completion FIFO underflows
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SBVIRALCTL_MC_2LM_STRUCT;




/* SBDEFEATURES0_MC_2LM_REG supported on:                                     */
/*       SKX_A0 (0x4025110C)                                                  */
/*       SKX (0x4025110C)                                                     */
/* Register default value:              0x00A2007F                            */
#define SBDEFEATURES0_MC_2LM_REG 0x0701410C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * 2LM scoreboard defeatures
 */
typedef union {
  struct {
    UINT32 numsbentries : 7;
    /* numsbentries - Bits[6:0], RWS_LB, default = 7'd127 
       Number of enabled SB entries. A value of 0 corresponds to all 128 entries being 
       enabled, otherwise entries 0...N-1 are enabled. 
               This must be set to 126 or less, as the upper 2 entries are statically 
       allotted to patrol and anti-starvation. 
     */
    UINT32 sbcrdtthrottleen : 2;
    /* sbcrdtthrottleen - Bits[8:7], RWS_LB, default = 1'b0 
       Enable throttling the return of scoreboard credits when the DFD response 
       function asserts. 
       00 - Dont throttle
       01 - Use Trigger-0
       10 - Use Trigger-1
       11 - Use Trigger-2
     */
    UINT32 sbfrcrejecten : 2;
    /* sbfrcrejecten - Bits[10:9], RWS_LB, default = 1'b0 
       Force new requests to be rejected when the DFD response function asserts.
       00 - Dont throttle
       01 - Use Trigger-0
       10 - Use Trigger-1
       11 - Use Trigger-2
     */
    UINT32 sbfrcrejectmode : 3;
    /* sbfrcrejectmode - Bits[13:11], RWS_LB, default = 1'b0 
       Used in conjunction with SbFrcRejectEn. Type of request to force reject when the 
       DFD response function asserts. 
       000 - All requests
       001 - NM requests
       010 - FM requests
       011 - Patrol requests
       100 - Reads
       101 - Writes
       110 - Non-patrol requests
     */
    UINT32 pwrdnovrd : 1;
    /* pwrdnovrd - Bits[14:14], RWS_LB, default = 1'b0 
       Disable clock-gating for 2LM scoreboard control
     */
    UINT32 msgchpwrdnovrd : 1;
    /* msgchpwrdnovrd - Bits[15:15], RWS_LB, default = 1'b0 
       Disable clock-gating for 2LM scoreboard MsgChnl
     */
    UINT32 maskmatchmodeen : 1;
    /* maskmatchmodeen - Bits[16:16], RW_LB, default = 1'b0 
       When set, each scoreboard entry stores the result of the mask/match operation in 
       M2M 
     */
    UINT32 addrantistarvedis : 1;
    /* addrantistarvedis - Bits[17:17], RWS_LB, default = 1'b1 
       When set, disables the anti-starvation mechanism for address conflicts.
               When not disabled, SbDefeatures0.NumSbEntries must be greater than 0
     */
    UINT32 addrantistarvethresh : 5;
    /* addrantistarvethresh - Bits[22:18], RWS_LB, default = 5'b01000 
       Number of times an entry must be rejected before anti-starvation will kick in.
     */
    UINT32 credantistarvedis : 1;
    /* credantistarvedis - Bits[23:23], RWS_LB, default = 1'b1 
       When set, disables the anti-starvation mechanism for SB credits.
               Must be set the same as M2Mem's Chicken0.DisCrdtAntiStarve.
               When not disabled, SbDefeatures0.NumSbEntries must be greater than 0
     */
    UINT32 clrfifoonsbdeallocall : 1;
    /* clrfifoonsbdeallocall - Bits[24:24], RWS_L, default = 1'b0 
       When set, clears the completion and data return FIFOs on an SbDeallocAll 
       command. 
     */
    UINT32 disdropnodata : 1;
    /* disdropnodata - Bits[25:25], RWS_L, default = 1'b0 
       When set, prevents SB from decoding and ignoring no-data packets
     */
    UINT32 rsvd : 6;
    /* rsvd - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SBDEFEATURES0_MC_2LM_STRUCT;


/* SBMCACTL_MC_2LM_REG supported on:                                          */
/*       SKX_A0 (0x40251110)                                                  */
/*       SKX (0x40251110)                                                     */
/* Register default value:              0x00000000                            */
#define SBMCACTL_MC_2LM_REG 0x07014110
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Enables for the SB error logging
 */
typedef union {
  struct {
    UINT32 reqerrdis : 1;
    /* reqerrdis - Bits[0:0], RWS, default = 1'b0 
       Disable logging and reporting of bad request errors
     */
    UINT32 readrspmisserrdis : 1;
    /* readrspmisserrdis - Bits[1:1], RWS, default = 1'b0 
       Disable logging and reporting of read data response miss errors
     */
    UINT32 ddr4cmperrdis : 1;
    /* ddr4cmperrdis - Bits[2:2], RWS, default = 1'b0 
       Disable logging and reporting of DDR4 completion errors
     */
    UINT32 ddrtcmperrdis : 1;
    /* ddrtcmperrdis - Bits[3:3], RWS, default = 1'b0 
       Disable logging and reporting of DDRt completion errors
     */
    UINT32 readrspopcodeerrdis : 1;
    /* readrspopcodeerrdis - Bits[4:4], RWS, default = 1'b0 
       Disable logging and reporting of read data response opcode errors
     */
    UINT32 cmplfifooferrdis : 1;
    /* cmplfifooferrdis - Bits[5:5], RWS, default = 1'b0 
       Disable logging and reporting of completion FIFO overflows
     */
    UINT32 cmplfifouferrdis : 1;
    /* cmplfifouferrdis - Bits[6:6], RWS, default = 1'b0 
       Disable logging and reporting of completion FIFO underflows
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SBMCACTL_MC_2LM_STRUCT;






/* DDRT_TIMING_MC_2LM_REG supported on:                                       */
/*       SKX_A0 (0x40251800)                                                  */
/*       SKX (0x40251800)                                                     */
/* Register default value:              0x00000600                            */
#define DDRT_TIMING_MC_2LM_REG 0x07014800
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDR-T Timing register.
 */
typedef union {
  struct {
    UINT32 du_slot_number : 3;
    /* du_slot_number - Bits[2:0], RW_LB, default = 3'b000 
       
               The slot number to allow GNT for current channel. This feature is only 
       enabled when uclk is slower than dclk, 
       and DDRT DIMMs are used. When this feature is enabled (mcmtr.enable_slot_use = 
       1), early completion for channel 0 should only be sent in slot 0, and channel 1 
       should only use slot 2, while channel 2 should only use slot 4. A value between 
       0 and 5 should be programmed in this field to allow the above result. This field 
       should be programmed such that (du_slot_number + GNT to early completion delay) 
       mod 6 = (2 * current channel number). 
               
     */
    UINT32 rsvd_3 : 5;
    /* rsvd_3 - Bits[7:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rid2dealloc : 8;
    /* rid2dealloc - Bits[15:8], RW, default = 8'd6 
       
                   This field controls the delay from when the MC sees RID to the 
       deallocation of the entry in the T-RPQ. 
               
     */
    UINT32 rsvd_16 : 16;
    /* rsvd_16 - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_TIMING_MC_2LM_STRUCT;


/* RSP_FUNC_ADDR_MATCH_LO_DDRT_MC_2LM_REG supported on:                       */
/*       SKX_A0 (0x40251880)                                                  */
/*       SKX (0x40251880)                                                     */
/* Register default value:              0x00000000                            */
#define RSP_FUNC_ADDR_MATCH_LO_DDRT_MC_2LM_REG 0x07014880
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Complete address match (Addr[45:3]) and mask is supported for all M2M writes. 
 * The error injection logic uses the address match mask logic output to determine 
 * which memory writes need to get error injection. Users can program up to two x4 
 * device masks (8-bits per chunk - 64 bits per cacheline). In Lockstep mode, only 
 * one device can be programmed in each lockstep channel. 
 */
typedef union {
  struct {
    UINT32 addr_match_lower : 32;
    /* addr_match_lower - Bits[31:0], RWS, default = 32'b00000000000000000000000000000000 
       Addr Match Lower: 32-bits (Match Addr[34:3])
     */
  } Bits;
  UINT32 Data;
} RSP_FUNC_ADDR_MATCH_LO_DDRT_MC_2LM_STRUCT;


/* RSP_FUNC_ADDR_MATCH_HI_DDRT_MC_2LM_REG supported on:                       */
/*       SKX_A0 (0x40251884)                                                  */
/*       SKX (0x40251884)                                                     */
/* Register default value:              0x00002000                            */
#define RSP_FUNC_ADDR_MATCH_HI_DDRT_MC_2LM_REG 0x07014884
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Complete address match (Addr[45:3]) and mask is supported for all M2M writes. 
 * The error injection logic uses the address match mask logic output to determine 
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
    UINT32 clear_addmatch_on_retry : 1;
    /* clear_addmatch_on_retry - Bits[13:13], RW, default = 1'b1 
       Clear the AddMatch bit in DDRT WPQ on DDRT Retry flow.
     */
    UINT32 rsvd_14 : 11;
    /* rsvd_14 - Bits[24:14], n/a, default = n/a 
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
} RSP_FUNC_ADDR_MATCH_HI_DDRT_MC_2LM_STRUCT;


/* RSP_FUNC_ADDR_MASK_LO_DDRT_MC_2LM_REG supported on:                        */
/*       SKX_A0 (0x40251888)                                                  */
/*       SKX (0x40251888)                                                     */
/* Register default value:              0xFFFFFFFF                            */
#define RSP_FUNC_ADDR_MASK_LO_DDRT_MC_2LM_REG 0x07014888
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Complete address match (Addr[45:3]) and mask is supported for all M2M writes. 
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
} RSP_FUNC_ADDR_MASK_LO_DDRT_MC_2LM_STRUCT;


/* RSP_FUNC_ADDR_MASK_HI_DDRT_MC_2LM_REG supported on:                        */
/*       SKX_A0 (0x4025188C)                                                  */
/*       SKX (0x4025188C)                                                     */
/* Register default value:              0x0E0007FF                            */
#define RSP_FUNC_ADDR_MASK_HI_DDRT_MC_2LM_REG 0x0701488C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Complete address match (Addr[45:3]) and mask is supported for all M2M writes. 
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
    UINT32 rsvd_11 : 14;
    /* rsvd_11 - Bits[24:11], n/a, default = n/a 
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
} RSP_FUNC_ADDR_MASK_HI_DDRT_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_0_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x40251890)                                                  */
/*       SKX (0x40251890)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_0_MC_2LM_REG 0x07014890
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_0_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_1_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x40251894)                                                  */
/*       SKX (0x40251894)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_1_MC_2LM_REG 0x07014894
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_1_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_2_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x40251898)                                                  */
/*       SKX (0x40251898)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_2_MC_2LM_REG 0x07014898
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_2_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_3_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x4025189C)                                                  */
/*       SKX (0x4025189C)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_3_MC_2LM_REG 0x0701489C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_3_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_4_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x402518A0)                                                  */
/*       SKX (0x402518A0)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_4_MC_2LM_REG 0x070148A0
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_4_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_5_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x402518A4)                                                  */
/*       SKX (0x402518A4)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_5_MC_2LM_REG 0x070148A4
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_5_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_6_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x402518A8)                                                  */
/*       SKX (0x402518A8)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_6_MC_2LM_REG 0x070148A8
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_6_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_7_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x402518AC)                                                  */
/*       SKX (0x402518AC)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_7_MC_2LM_REG 0x070148AC
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_7_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_8_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x402518B0)                                                  */
/*       SKX (0x402518B0)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_8_MC_2LM_REG 0x070148B0
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_8_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_9_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x402518B4)                                                  */
/*       SKX (0x402518B4)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_9_MC_2LM_REG 0x070148B4
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_9_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_10_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x402518B8)                                                  */
/*       SKX (0x402518B8)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_10_MC_2LM_REG 0x070148B8
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_10_MC_2LM_STRUCT;


/* FMTADCHNILVOFFSET_11_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x402518BC)                                                  */
/*       SKX (0x402518BC)                                                     */
/* Register default value:              0x00000000                            */
#define FMTADCHNILVOFFSET_11_MC_2LM_REG 0x070148BC
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  12 instances of TAD channel interleaves for DDR-T.  
 */
typedef union {
  struct {
    UINT32 skt_ways : 2;
    /* skt_ways - Bits[1:0], RW_LB, default = 2'b00  */
    UINT32 chn_ways : 2;
    /* chn_ways - Bits[3:2], RW_LB, default = 2'b00  */
    UINT32 tad_offset : 20;
    /* tad_offset - Bits[23:4], RW_LB, default = 20'b00000000000000000000 
       channel interleave 0 offset, i.e. CHANNELOFFSET[45:26] == channel interleave i 
       offset, 64MB granularity . 
     */
    UINT32 chn_idx_offset : 2;
    /* chn_idx_offset - Bits[25:24], RW_LB, default = 2'b00 
       Reverse Address Translation Channel Index OffsetBIOS programmed this field by 
       calculating: 
       (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
       where % is the modulo function.
       CHN_IDX_OFFSET can have a value of 0, 1, or 2
       In this equation, the BASE is the lowest address in the TAD range. The 
       TAD_SKT_WAY is 1, 2, 4, or 8, and TAD_CH_WAY is 1, 2, 3, or 4. CHN_IDX_OFFSET 
       will always end up being zero if TAD_CH_WAY is not equal to 3. If TAD_CH_WAY is 
       3, CHN_IDX_OFFSET can be 0, 1, or 2. 
     */
    UINT32 imc_wayness_x3 : 1;
    /* imc_wayness_x3 - Bits[26:26], RW_LB, default = 1'b0 
       when set, the skt_wayness will be treated as skt_way*3, with the div3 that this 
       implies on the system address at the granularity specified by imc_granularity 
       (used for EX only) 
                 
     */
    UINT32 skt_granularity : 2;
    /* skt_granularity - Bits[28:27], RW_LB, default = 2'b00 
       
       Specifies the granularity of the skt_way interleave
         b'00   64B (based off PA[6] and up)  (used for legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for 1LM only)
         b'10   4KB (based off PA[12] and up) (used for 2LM, PMem, and Block/DDRT_CSR)
         b'11   1GB (based off PA[30] and up) {used for PMem only, though the standard 
       PMem mode will be 4KB) 
               
     */
    UINT32 ch_granularity : 2;
    /* ch_granularity - Bits[30:29], RW_LB, default = 2'b00 
       
       Specifies the granularity of the ch_way interleave
         b'00   64B (based off PA[6] and up)  (used for DDR4 legacy 1LM only)
         b'01  256B (based off PA[8] and up)  (used for DDR4 1LM, DDR4 2LM, DDRT Block, 
       and DDRT 2LM.  the latter only in mirror mode) 
         b'10   4KB (based off PA[12] and up) (used for DDRT 2LM and DDRT PMem)
         b'11   reserved
       Note:  using the same ch_granularity encoding for both DDRT and DDR4 even though 
       64B only applies for DDR4 and 4KB only applies for DDRT. 
               
     */
    UINT32 tad_offset_sign : 1;
    /* tad_offset_sign - Bits[31:31], RW_LB, default = 1'b0 
       When 0, the tad_offset is subtracted from the system address. When 1, the 
       tad_offset is added to the system address. 
     */
  } Bits;
  UINT32 Data;
} FMTADCHNILVOFFSET_11_MC_2LM_STRUCT;


/* DDRT_SCRATCHPAD_MC_2LM_REG supported on:                                   */
/*       SKX_A0 (0x402518FC)                                                  */
/*       SKX (0x402518FC)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_SCRATCHPAD_MC_2LM_REG 0x070148FC
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * dummy register
 */
typedef union {
  struct {
    UINT32 scratchpad : 32;
    /* scratchpad - Bits[31:0], RWS_V, default = 32'b00000000000000000000000000000000 
       scratchpad
     */
  } Bits;
  UINT32 Data;
} DDRT_SCRATCHPAD_MC_2LM_STRUCT;


/* DDRT_RETRY_FSM_STATE_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x40251904)                                                  */
/*       SKX (0x40251904)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_RETRY_FSM_STATE_MC_2LM_REG 0x07014904
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  This register contains current and previous state of DDRT retry FSM.  
 */
typedef union {
  struct {
    UINT32 current_state : 5;
    /* current_state - Bits[4:0], RO_V, default = 5'b0 
       Current encoded state of DDRT retry FSM
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_8 : 5;
    UINT32 rsvd_13 : 19;
    /* rsvd_13 - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_RETRY_FSM_STATE_MC_2LM_STRUCT;


/* FMRIRWAYNESSLIMIT_0_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x40251908)                                                  */
/*       SKX (0x40251908)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRWAYNESSLIMIT_0_MC_2LM_REG 0x07014908
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * 4 Rank Interleave Ranges (RIR) for DDR-T 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_limit : 13;
    /* rir_limit - Bits[13:1], RW_LB, default = 13'b0000000000000 
       RIR[4:0].LIMIT[41:29] == highest address of the range in channel address space.  
       Needs to address up to 4TB at a 512MB granularity. 
     */
    UINT32 rsvd_14 : 14;
    /* rsvd_14 - Bits[27:14], n/a, default = n/a 
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
} FMRIRWAYNESSLIMIT_0_MC_2LM_STRUCT;


/* FMRIRWAYNESSLIMIT_1_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x4025190C)                                                  */
/*       SKX (0x4025190C)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRWAYNESSLIMIT_1_MC_2LM_REG 0x0701490C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * 4 Rank Interleave Ranges (RIR) for DDR-T 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_limit : 13;
    /* rir_limit - Bits[13:1], RW_LB, default = 13'b0000000000000 
       RIR[4:0].LIMIT[41:29] == highest address of the range in channel address space.  
       Needs to address up to 4TB at a 512MB granularity. 
     */
    UINT32 rsvd_14 : 14;
    /* rsvd_14 - Bits[27:14], n/a, default = n/a 
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
} FMRIRWAYNESSLIMIT_1_MC_2LM_STRUCT;


/* FMRIRWAYNESSLIMIT_2_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x40251910)                                                  */
/*       SKX (0x40251910)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRWAYNESSLIMIT_2_MC_2LM_REG 0x07014910
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * 4 Rank Interleave Ranges (RIR) for DDR-T 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_limit : 13;
    /* rir_limit - Bits[13:1], RW_LB, default = 13'b0000000000000 
       RIR[4:0].LIMIT[41:29] == highest address of the range in channel address space.  
       Needs to address up to 4TB at a 512MB granularity. 
     */
    UINT32 rsvd_14 : 14;
    /* rsvd_14 - Bits[27:14], n/a, default = n/a 
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
} FMRIRWAYNESSLIMIT_2_MC_2LM_STRUCT;


/* FMRIRWAYNESSLIMIT_3_MC_2LM_REG supported on:                               */
/*       SKX_A0 (0x40251914)                                                  */
/*       SKX (0x40251914)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRWAYNESSLIMIT_3_MC_2LM_REG 0x07014914
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * 4 Rank Interleave Ranges (RIR) for DDR-T 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_limit : 13;
    /* rir_limit - Bits[13:1], RW_LB, default = 13'b0000000000000 
       RIR[4:0].LIMIT[41:29] == highest address of the range in channel address space.  
       Needs to address up to 4TB at a 512MB granularity. 
     */
    UINT32 rsvd_14 : 14;
    /* rsvd_14 - Bits[27:14], n/a, default = n/a 
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
} FMRIRWAYNESSLIMIT_3_MC_2LM_STRUCT;


/* FMRIRILV0OFFSET_0_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251920)                                                  */
/*       SKX (0x40251920)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRILV0OFFSET_0_MC_2LM_REG 0x07014920
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  DDR-T Rank Interleave ranges 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset0 : 16;
    /* rir_offset0 - Bits[17:2], RW_LB, default = 16'b0000000000000000 
       RIR[].RANKOFFSET0[41:26] == rank interleave 0 offset
     */
    UINT32 rir_rnk_tgt0 : 4;
    /* rir_rnk_tgt0 - Bits[21:18], RW_LBV, default = 4'b0000 
       target rank ID for rank interleave 0 (used for 1/2-way RIR interleaving).
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FMRIRILV0OFFSET_0_MC_2LM_STRUCT;


/* FMRIRILV1OFFSET_0_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251924)                                                  */
/*       SKX (0x40251924)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRILV1OFFSET_0_MC_2LM_REG 0x07014924
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  DDR-T Rank Interleave ranges 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset1 : 16;
    /* rir_offset1 - Bits[17:2], RW_LB, default = 16'b0000000000000000 
       RIR[].RANKOFFSET1[41:26] == rank interleave 1 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt1 : 4;
    /* rir_rnk_tgt1 - Bits[21:18], RW_LBV, default = 4'b0000 
       target rank ID for rank interleave 1 (used for 1/2-way RIR interleaving).
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FMRIRILV1OFFSET_0_MC_2LM_STRUCT;


/* FMRIRILV0OFFSET_1_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251940)                                                  */
/*       SKX (0x40251940)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRILV0OFFSET_1_MC_2LM_REG 0x07014940
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  DDR-T Rank Interleave ranges 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset0 : 16;
    /* rir_offset0 - Bits[17:2], RW_LB, default = 16'b0000000000000000 
       RIR[].RANKOFFSET0[41:26] == rank interleave 0 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt0 : 4;
    /* rir_rnk_tgt0 - Bits[21:18], RW_LBV, default = 4'b0000 
       target rank ID for rank interleave 0 (used for 1/2-way RIR interleaving).
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FMRIRILV0OFFSET_1_MC_2LM_STRUCT;


/* FMRIRILV1OFFSET_1_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251944)                                                  */
/*       SKX (0x40251944)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRILV1OFFSET_1_MC_2LM_REG 0x07014944
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  DDR-T Rank Interleave ranges 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset1 : 16;
    /* rir_offset1 - Bits[17:2], RW_LB, default = 16'b0000000000000000 
       RIR[].RANKOFFSET1[41:26] == rank interleave 1 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt1 : 4;
    /* rir_rnk_tgt1 - Bits[21:18], RW_LBV, default = 4'b0000 
       target rank ID for rank interleave 1 (used for 1/2-way RIR interleaving).
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FMRIRILV1OFFSET_1_MC_2LM_STRUCT;


/* FMRIRILV0OFFSET_2_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251960)                                                  */
/*       SKX (0x40251960)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRILV0OFFSET_2_MC_2LM_REG 0x07014960
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  DDR-T Rank Interleave ranges 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset0 : 16;
    /* rir_offset0 - Bits[17:2], RW_LB, default = 16'b0000000000000000 
       RIR[].RANKOFFSET0[41:26] == rank interleave 0 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt0 : 4;
    /* rir_rnk_tgt0 - Bits[21:18], RW_LBV, default = 4'b0000 
       target rank ID for rank interleave 0 (used for 1/2-way RIR interleaving).
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FMRIRILV0OFFSET_2_MC_2LM_STRUCT;


/* FMRIRILV1OFFSET_2_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251964)                                                  */
/*       SKX (0x40251964)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRILV1OFFSET_2_MC_2LM_REG 0x07014964
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  DDR-T Rank Interleave ranges 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset1 : 16;
    /* rir_offset1 - Bits[17:2], RW_LB, default = 16'b0000000000000000 
       RIR[].RANKOFFSET1[41:26] == rank interleave 1 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt1 : 4;
    /* rir_rnk_tgt1 - Bits[21:18], RW_LBV, default = 4'b0000 
       target rank ID for rank interleave 1 (used for 1/2-way RIR interleaving).
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FMRIRILV1OFFSET_2_MC_2LM_STRUCT;


/* FMRIRILV0OFFSET_3_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251980)                                                  */
/*       SKX (0x40251980)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRILV0OFFSET_3_MC_2LM_REG 0x07014980
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  DDR-T Rank Interleave ranges 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset0 : 16;
    /* rir_offset0 - Bits[17:2], RW_LB, default = 16'b0000000000000000 
       RIR[].RANKOFFSET0[41:26] == rank interleave 0 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt0 : 4;
    /* rir_rnk_tgt0 - Bits[21:18], RW_LBV, default = 4'b0000 
       target rank ID for rank interleave 0 (used for 1/2-way RIR interleaving).
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FMRIRILV0OFFSET_3_MC_2LM_STRUCT;


/* FMRIRILV1OFFSET_3_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251984)                                                  */
/*       SKX (0x40251984)                                                     */
/* Register default value:              0x00000000                            */
#define FMRIRILV1OFFSET_3_MC_2LM_REG 0x07014984
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  DDR-T Rank Interleave ranges 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rir_offset1 : 16;
    /* rir_offset1 - Bits[17:2], RW_LB, default = 16'b0000000000000000 
       RIR[].RANKOFFSET1[41:26] == rank interleave 1 offset, 64MB granularity 
     */
    UINT32 rir_rnk_tgt1 : 4;
    /* rir_rnk_tgt1 - Bits[21:18], RW_LBV, default = 4'b0000 
       target rank ID for rank interleave 1 (used for 1/2-way RIR interleaving).
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FMRIRILV1OFFSET_3_MC_2LM_STRUCT;


/* T_DDRT_RD_CNSTRNT_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251A00)                                                  */
/*       SKX (0x40251A00)                                                     */
/* Register default value:              0x06060602                            */
#define T_DDRT_RD_CNSTRNT_MC_2LM_REG 0x07014A00
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Read Constraint Timing Register.
 */
typedef union {
  struct {
    UINT32 t_ddrt_rdrd_s : 8;
    /* t_ddrt_rdrd_s - Bits[7:0], RW_LB, default = 8'h02 
       Minimum READ to READ command DCLK separation from same DIMM separation parameter 
       (measured between the clock assertion edges of the two corresponding asserted 
       command CS#). The min:max program range is 2-31. Any value outside of the range 
       is RFU. 
     */
    UINT32 t_ddrt_rdrd_dd : 8;
    /* t_ddrt_rdrd_dd - Bits[15:8], RW_LB, default = 8'h06 
       Minimum READ to READ command DCLK separation from
                  different DIMM separation parameter (measured between the clock
                  assertion edges of the two corresponding asserted command CS#). The
                  min:max program range is 5-31.  Any value outside of the range is
                  RFU. This should be programmed to the same
                  value as t_ddrt_rdrd_s if the number of 1 DDRT DIMMs on the channel
                  is 1. 
     */
    UINT32 t_ddrt_wrrd_s : 8;
    /* t_ddrt_wrrd_s - Bits[23:16], RW_LB, default = 8'h06 
       Minimum WRITE to READ command DCLK separation from same DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 2-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_wrrd_dd : 8;
    /* t_ddrt_wrrd_dd - Bits[31:24], RW_LB, default = 8'h06 
       Minimum WRITE to READ command DCLK separation
                  from different DIMM separation parameter (measured between the clock
                  assertion edges of the two corresponding asserted command CS#). The
                  min:max program range is 5-31.  Any value outside of the range is
                  RFU.   This should be programmed to the same
                  value as t_ddrt_wrrd_s if the number of 1 DDRT DIMMs on the channel
                  is 1. 
     */
  } Bits;
  UINT32 Data;
} T_DDRT_RD_CNSTRNT_MC_2LM_STRUCT;


/* T_DDRT_WR_CNSTRNT_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251A04)                                                  */
/*       SKX (0x40251A04)                                                     */
/* Register default value:              0x06060606                            */
#define T_DDRT_WR_CNSTRNT_MC_2LM_REG 0x07014A04
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Write Constraint Timing Register.
 */
typedef union {
  struct {
    UINT32 t_ddrt_rdwr_s : 8;
    /* t_ddrt_rdwr_s - Bits[7:0], RW_LB, default = 8'h06 
       Minimum READ to WRITE command DCLK separation from same DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 2-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_rdwr_dd : 8;
    /* t_ddrt_rdwr_dd - Bits[15:8], RW_LB, default = 8'h06 
       Minimum READ to WRITE command DCLK separation from different DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 5-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_wrwr_s : 8;
    /* t_ddrt_wrwr_s - Bits[23:16], RW_LB, default = 8'h06 
       Minimum WRITE to WRITE command DCLK separation from same DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 4-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_wrwr_dd : 8;
    /* t_ddrt_wrwr_dd - Bits[31:24], RW_LB, default = 8'h06 
       Minimum WRITE to WRITE command DCLK separation from different DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 5-31.  Any value outside of 
       the range is RFU. 
     */
  } Bits;
  UINT32 Data;
} T_DDRT_WR_CNSTRNT_MC_2LM_STRUCT;


/* T_DDRT_GNT2RW_CNSTRNT_MC_2LM_REG supported on:                             */
/*       SKX_A0 (0x40251A08)                                                  */
/*       SKX (0x40251A08)                                                     */
/* Register default value:              0x06060606                            */
#define T_DDRT_GNT2RW_CNSTRNT_MC_2LM_REG 0x07014A08
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Grant to RW Timing Constraints
 */
typedef union {
  struct {
    UINT32 t_ddrt_gntrd_s : 8;
    /* t_ddrt_gntrd_s - Bits[7:0], RW_LB, default = 8'h06 
       Minimum GRANT to READ command DCLK separation from same DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 2-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_gntrd_dd : 8;
    /* t_ddrt_gntrd_dd - Bits[15:8], RW_LB, default = 8'h06 
       Minimum GRANT to READ command DCLK separation
                  from different DIMM separation parameter (measured between the clock
                  assertion edges of the two corresponding asserted command CS#). The
                  min:max program range is 2-31.  This should be programmed to the same
                  value as t_ddrt_gntrd_s if the number of 1 DDRT DIMMs on the channel
                  is 1. 
               
     */
    UINT32 t_ddrt_gntwr_s : 8;
    /* t_ddrt_gntwr_s - Bits[23:16], RW_LB, default = 8'h06 
       Minimum GRANT to WRITE command DCLK separation from same DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 5-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_gntwr_dd : 8;
    /* t_ddrt_gntwr_dd - Bits[31:24], RW_LB, default = 8'h06 
       Minimum GRANT to WRITE command DCLK separation from different DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 5-31.  Any value outside of 
       the range is RFU. 
     */
  } Bits;
  UINT32 Data;
} T_DDRT_GNT2RW_CNSTRNT_MC_2LM_STRUCT;


/* T_DDRT_RW2GNT_CNSTRNT_MC_2LM_REG supported on:                             */
/*       SKX_A0 (0x40251A0C)                                                  */
/*       SKX (0x40251A0C)                                                     */
/* Register default value:              0x06060606                            */
#define T_DDRT_RW2GNT_CNSTRNT_MC_2LM_REG 0x07014A0C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW to Grant Timing Constraints
 */
typedef union {
  struct {
    UINT32 t_ddrt_rdgnt_s : 8;
    /* t_ddrt_rdgnt_s - Bits[7:0], RW_LB, default = 8'h06 
       Minimum READ to GRANT command DCLK separation from same DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 2-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_rdgnt_dd : 8;
    /* t_ddrt_rdgnt_dd - Bits[15:8], RW_LB, default = 8'h06 
       Minimum READ to GRANT command DCLK separation from different DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 5-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_wrgnt_s : 8;
    /* t_ddrt_wrgnt_s - Bits[23:16], RW_LB, default = 8'h06 
       Minimum WRITE to GRANT command DCLK separation from same DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 5-31.  Any value outside of 
       the range is RFU. 
     */
    UINT32 t_ddrt_wrgnt_dd : 8;
    /* t_ddrt_wrgnt_dd - Bits[31:24], RW_LB, default = 8'h06 
       Minimum WRITE to GRANT command DCLK separation from different DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 5-31.  Any value outside of 
       the range is RFU. 
     */
  } Bits;
  UINT32 Data;
} T_DDRT_RW2GNT_CNSTRNT_MC_2LM_STRUCT;


/* T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG supported on:                            */
/*       SKX_A0 (0x20251A10)                                                  */
/*       SKX (0x20251A10)                                                     */
/* Register default value:              0x0606                                */
#define T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG 0x07012A10
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Grant to Grant Timing Constraint
 */
typedef union {
  struct {
    UINT16 t_ddrt_gntgnt_s : 8;
    /* t_ddrt_gntgnt_s - Bits[7:0], RW_LB, default = 8'h06 
       Minimum GRANT to GRANT command DCLK separation from same DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 4-31.  Any value outside of 
       the range is RFU. 
     */
    UINT16 t_ddrt_gntgnt_dd : 8;
    /* t_ddrt_gntgnt_dd - Bits[15:8], RW_LB, default = 8'h06 
       Minimum GRANT to GRANT command DCLK separation from different DIMM separation 
       parameter (measured between the clock assertion edges of the two corresponding 
       asserted command CS#). The min:max program range is 5-31.  Any value outside of 
       the range is RFU. 
     */
  } Bits;
  UINT16 Data;
} T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_STRUCT;


/* T_DDRT_PD_MC_2LM_REG supported on:                                         */
/*       SKX_A0 (0x40251A14)                                                  */
/*       SKX (0x40251A14)                                                     */
/* Register default value:              0x000F000F                            */
#define T_DDRT_PD_MC_2LM_REG 0x07014A14
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW to Power Down Timing Constraints
 */
typedef union {
  struct {
    UINT32 t_ddrt_rdpden : 8;
    /* t_ddrt_rdpden - Bits[7:0], RW_LB, default = 8'h0F 
       Minimum READ to PD command separation (in DCLK, measured between the clock 
       assertion edges of the two corresponding asserted command CS#). The min:max 
       program range is 2-31.  Any value outside of the range is RFU. 
               
     */
    UINT32 rsvd_8 : 8;
    /* rsvd_8 - Bits[15:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_ddrt_wrpden : 8;
    /* t_ddrt_wrpden - Bits[23:16], RW_LB, default = 8'h0F 
       Minimum WRITE to PD command separation (in DCLK measured between the clock 
       assertion edges of the two corresponding asserted command CS#). The min:max 
       program range is 2-31.  Any value outside of the range is RFU. 
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} T_DDRT_PD_MC_2LM_STRUCT;


/* T_DDRT_PD2_MC_2LM_REG supported on:                                        */
/*       SKX_A0 (0x40251A18)                                                  */
/*       SKX (0x40251A18)                                                     */
/* Register default value:              0x00000006                            */
#define T_DDRT_PD2_MC_2LM_REG 0x07014A18
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW to Power Down Timing Constraint Register 2
 */
typedef union {
  struct {
    UINT32 t_ddrt_gntpden : 8;
    /* t_ddrt_gntpden - Bits[7:0], RW_LB, default = 8'h06 
       Minimum GRANT to PD command separation (in DCLK measured between the clock 
       assertion edges of the two corresponding asserted command CS#). The min:max 
       program range is 2-31.  Any value outside of the range is RFU. 
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} T_DDRT_PD2_MC_2LM_STRUCT;


/* T_DDRT_PWRUP_MC_2LM_REG supported on:                                      */
/*       SKX_A0 (0x40251A1C)                                                  */
/*       SKX (0x40251A1C)                                                     */
/* Register default value:              0x30400008                            */
#define T_DDRT_PWRUP_MC_2LM_REG 0x07014A1C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Power Down Exit Timing Constraints
 */
typedef union {
  struct {
    UINT32 t_ddrt_xp : 8;
    /* t_ddrt_xp - Bits[7:0], RW_LB, default = 8'h08 
       Minimum Power Down Exit to first command DCLK separation. The min:max program 
       range is TBD.  Any value outside of the range is RFU. 
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[11:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_ddrt_xpdll : 16;
    /* t_ddrt_xpdll - Bits[27:12], RW_LB, default = 16'h400 
        Not used.  Remove. 
     */
    UINT32 t_ddrt_cke : 4;
    /* t_ddrt_cke - Bits[31:28], RW_LB, default = 4'b0011 
       CKE minimum pulse width.  Should be programmed consistent with CR_TCRAP.t_cke 
     */
  } Bits;
  UINT32 Data;
} T_DDRT_PWRUP_MC_2LM_STRUCT;


/* T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG supported on:                         */
/*       SKX_A0 (0x40251A20)                                                  */
/*       SKX (0x40251A20)                                                     */
/* Register default value:              0x00020606                            */
#define T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG 0x07014A20
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT DIMM0 Basic Timing Register
 */
typedef union {
  struct {
    UINT32 t_ddrt_gnt2erid : 8;
    /* t_ddrt_gnt2erid - Bits[7:0], RW_LB, default = 8'h06 
       Delay from GRANT to ERID. The min:max program range is TBD.  Any value outside 
       of the range is RFU. 
                   The value of this field should be programmed during training as part 
       of roundtrip training. 
               
     */
    UINT32 t_ddrt_tcl : 8;
    /* t_ddrt_tcl - Bits[15:8], RW_LB, default = 8'h06 
       Grant to 1st Data Delay. Not used by hardware.
     */
    UINT32 t_ddrt_twl : 8;
    /* t_ddrt_twl - Bits[23:16], RW_LB, default = 8'h02 
       Delay between write command (CS# assertion edge) and 1st burst of write data. 
                   Offset from programmed value of 9 DCLKs based on pipeline delays.
               
     */
    UINT32 t_ddrt_twl_adj : 3;
    /* t_ddrt_twl_adj - Bits[26:24], RW_LB, default = 3'h0 
       
                This register defines additional WR data delay per channel in order to
                overcome the WR-flyby issue. This change is applied only to DDRT DIMMs
                on the channel. 
                The total CAS write latency that the DDR sees is the sum of t_ddrt_twl 
       and the t_ddrt_twl_adj minus t_ddrt_twl_adj_neg. 
                000 - no added latency (default)
                001 to 111 - 1 to 7 Dclk of added latency
               
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_ddrt_twl_adj_neg : 2;
    /* t_ddrt_twl_adj_neg - Bits[29:28], RW_LB, default = 2'b00 
       
                 This register defines reduction of WR data delay per channel in order 
       to overcome the WR-flyby issue. 
                 The total CAS write latency that the DDR sees is the sum of t_ddrt_twl 
       and the t_ddrt_twl_adj minus t_ddrt_twl_adj_neg. 
                 This change is applied only to DDRT DIMMs on the channel.
                 00 - no latency reduction (default)
                 01 - reduce latency by 1 Dclk
                 10 - reduce latency by 2 Dclk
                 11 - reduce latency by 3 Dclk
                 The total t_cwl (t_ddrt_twl + t_ddrt_tWL_adj - t_ddrt_twl_adj_neg) 
       should not be less than 2  
               
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT;


/* T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG supported on:                         */
/*       SKX_A0 (0x40251A24)                                                  */
/*       SKX (0x40251A24)                                                     */
/* Register default value:              0x00020606                            */
#define T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG 0x07014A24
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT DIMM1 Basic Timing Register
 */
typedef union {
  struct {
    UINT32 t_ddrt_gnt2erid : 8;
    /* t_ddrt_gnt2erid - Bits[7:0], RW_LB, default = 8'h06 
       Delay from GRANT to ERID. The min:max program range is TBD.  Any value outside 
       of the range is RFU. 
                   The value of this field should be programmed during training as part 
       of roundtrip training. 
               
     */
    UINT32 t_ddrt_tcl : 8;
    /* t_ddrt_tcl - Bits[15:8], RW_LB, default = 8'h06 
       Grant to 1st Data Delay. Not used by hardware.
     */
    UINT32 t_ddrt_twl : 8;
    /* t_ddrt_twl - Bits[23:16], RW_LB, default = 8'h02 
       Not used.  DIMM0 and DIMM1 must use the same tWL. 
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_STRUCT;


/* T_PERSIST_TIMING_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251A28)                                                  */
/*       SKX (0x40251A28)                                                     */
/* Register default value:              0x10040020                            */
#define T_PERSIST_TIMING_MC_2LM_REG 0x07014A28
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Persistent Write Delay
 */
typedef union {
  struct {
    UINT32 t_persist_delay : 16;
    /* t_persist_delay - Bits[15:0], RW_LB, default = 16'h20 
       
                This is the delay that the TWPQ must wait after
                issuing write data before considering the written data has reached
                persistent domain in FNV. The scheduler must monitor the ERR# pin in
                this duration and mark the transaction as visible only if ERR# is not
                asserted for Error latency duration after write data is issued. Note:
                Need to document the additional delay for the write data thru DDRIO
                and the delay of sensing ERR# assertion in DDRIO. The maximum  program
                range is 128 Dclks.
                
     */
    UINT32 t_gnt_dq : 8;
    /* t_gnt_dq - Bits[23:16], RW_LB, default = 8'h4 
       This is the delay that the data bus must be blocked after a GNT is scheduled. 
       The min:max program range is TBD.  Any value outside of the range is RFU. 
     */
    UINT32 t_gnt_dq_retry : 8;
    /* t_gnt_dq_retry - Bits[31:24], RW_LB, default = 8'd16 
       
                This is the delay for read to read command timing when read data goes 
       through  
                through ECC correction. This CSR must must be programmed to
                value that is greater than max (DDR4_TO_DDRT_DELAY, DDRT_TO_DDR4_DELAY, 
       12)  
               
     */
  } Bits;
  UINT32 Data;
} T_PERSIST_TIMING_MC_2LM_STRUCT;


/* DDRT_CREDIT_LIMIT_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251A2C)                                                  */
/*       SKX (0x40251A2C)                                                     */
/* Register default value on SKX_A0:    0x00300028                            */
/* Register default value on SKX:       0x00000028                            */
#define DDRT_CREDIT_LIMIT_MC_2LM_REG 0x07014A2C
/* Struct format extracted from XML file SKX_A0\2.10.1.CFG.xml.
 * DDR-T Read and Write credits per DIMM
 */
typedef union {
  struct {
    UINT32 ddrt_rd_credit : 8;
    /* ddrt_rd_credit - Bits[7:0], RW_LB, default = 8'h28 
       Max 64B Pending Read Credit to FM (FNV RPQ Depth)
     */
    UINT32 ddrt_wr_credit : 8;
    /* ddrt_wr_credit - Bits[15:8], RW_LB, default = 8'h00 
       Max 64B Pending Write Credit to FM (FNV WPQ depth)
     */
    UINT32 reserved : 16;
    /* reserved - Bits[31:16], RW_LB, default = 8'h30 
       Reserved for future use.
     */
  } Bits;
  UINT32 Data;
} DDRT_CREDIT_LIMIT_MC_2LM_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDR-T Read and Write credits per DIMM
 */
typedef union {
  struct {
    UINT32 ddrt_rd_credit : 8;
    /* ddrt_rd_credit - Bits[7:0], RW_LB, default = 8'h28 
       Max 64B Pending Read Credit to FM (FNV RPQ Depth)
     */
    UINT32 ddrt_wr_credit : 8;
    /* ddrt_wr_credit - Bits[15:8], RW_LB, default = 8'h00 
       Max 64B Pending Write Credit to FM (FNV WPQ depth)
     */
    UINT32 min_rd_credit_partials : 4;
    /* min_rd_credit_partials - Bits[19:16], RW_LB, default = 4'h0 
       
               DDRT Reads from the Read Pending Queue are scheduled 
               only if the number of available SXP Read credits is greater than the
               value programmed in this field. This reserves a minimum number of DDRT
               read credits for the background underfill read for partial
               writes. 
               
     */
    UINT32 reserved : 12;
    /* reserved - Bits[31:20], RW_LB, default = 12'h0 
       
               Reserved 
               
     */
  } Bits;
  UINT32 Data;
} DDRT_CREDIT_LIMIT_MC_2LM_STRUCT;



/* DDRT_M2M_CREDIT_COUNT_MC_2LM_REG supported on:                             */
/*       SKX_A0 (0x40251A30)                                                  */
/*       SKX (0x40251A30)                                                     */
/* Register default value:              0x30300000                            */
#define DDRT_M2M_CREDIT_COUNT_MC_2LM_REG 0x07014A30
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT current credit count/state.  For debug. 
 */
typedef union {
  struct {
    UINT32 rsvd : 16;
    /* rsvd - Bits[15:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 m2m_rd_cr_cnt : 8;
    /* m2m_rd_cr_cnt - Bits[23:16], RO_V, default = 8'h30 
       Current 64B Pending Read Credit Count to M2M
     */
    UINT32 m2m_wr_cr_cnt : 8;
    /* m2m_wr_cr_cnt - Bits[31:24], RO_V, default = 8'h30 
       Current 64B Pending Write Credit Count to M2M
     */
  } Bits;
  UINT32 Data;
} DDRT_M2M_CREDIT_COUNT_MC_2LM_STRUCT;


/* DDRT_MAJOR_MODE_THRESHOLD1_MC_2LM_REG supported on:                        */
/*       SKX_A0 (0x40251A34)                                                  */
/*       SKX (0x40251A34)                                                     */
/* Register default value:              0x321E0818                            */
#define DDRT_MAJOR_MODE_THRESHOLD1_MC_2LM_REG 0x07014A34
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * configures the DDRT Write Major Mode behavior - Thresholds and starvation 
 * counters. 
 */
typedef union {
  struct {
    UINT32 wmm_enter : 8;
    /* wmm_enter - Bits[7:0], RW_LB, default = 8'h18 
       When channel is in DDRT RMM and the number of WDB entries gets to this level, 
       channel goes into DDRT WMM. The value must be at least 2. 
     */
    UINT32 wmm_exit : 8;
    /* wmm_exit - Bits[15:8], RW_LB, default = 8'h08 
       When channel is in DDRT WMM and the number of WDB entries gets to this level, 
       channel goes back to DDRT RMM. The value must be between 1 and (WMM_Enter - 1). 
     */
    UINT32 min_rmm_cycles : 8;
    /* min_rmm_cycles - Bits[23:16], RW_LB, default = 8'h1E 
       This count is used for the starvation switch. Channel will stay in DDRT RMM for 
       at least MIN_RMM_CYCLES number of cycles. Exception: DDRT ADR will force channel 
       into DDRT WMM. 
                            The threshold affects how quickly the scheduler issues 
       writes and it may be reduced to a lower value to allow pcommits to complete 
       faster. 
     */
    UINT32 min_wmm_cycles : 8;
    /* min_wmm_cycles - Bits[31:24], RW_LB, default = 8'h32 
       This count is used for the starvation switch. Channel will stay in DDRT WMM for 
       at most MAX_WMM_CYCLES number of cycles. Exception: DDRT ADR will force channel 
       into DDRT WMM. 
     */
  } Bits;
  UINT32 Data;
} DDRT_MAJOR_MODE_THRESHOLD1_MC_2LM_STRUCT;


/* DDRT_CREDIT_COUNT_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251A38)                                                  */
/*       SKX (0x40251A38)                                                     */
/* Register default value:              0x38283828                            */
#define DDRT_CREDIT_COUNT_MC_2LM_REG 0x07014A38
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT current credit count/state
 */
typedef union {
  struct {
    UINT32 ddrt_rd_cr_cnt_ddrt0 : 8;
    /* ddrt_rd_cr_cnt_ddrt0 - Bits[7:0], RO_V, default = 8'h28 
       Current count of Read credits to FM consumed on rank 0
     */
    UINT32 ddrt_wr_cr_cnt_ddrt0 : 8;
    /* ddrt_wr_cr_cnt_ddrt0 - Bits[15:8], RO_V, default = 8'h38 
       Current count of Write credits to FM consumed on rank 0
     */
    UINT32 ddrt_rd_cr_cnt_ddrt1 : 8;
    /* ddrt_rd_cr_cnt_ddrt1 - Bits[23:16], RO_V, default = 8'h28 
       Current count of Read credits to FM consumed on rank 1
     */
    UINT32 ddrt_wr_cr_cnt_ddrt1 : 8;
    /* ddrt_wr_cr_cnt_ddrt1 - Bits[31:24], RO_V, default = 8'h38 
       Current count of Write credits to FM consumed on rank 1
     */
  } Bits;
  UINT32 Data;
} DDRT_CREDIT_COUNT_MC_2LM_STRUCT;


/* DDRT_MAJOR_MODE_THRESHOLD3_MC_2LM_REG supported on:                        */
/*       SKX_A0 (0x40251A3C)                                                  */
/*       SKX (0x40251A3C)                                                     */
/* Register default value on SKX_A0:    0x00000008                            */
/* Register default value on SKX:       0x0228A108                            */
#define DDRT_MAJOR_MODE_THRESHOLD3_MC_2LM_REG 0x07014A3C
/* Struct format extracted from XML file SKX_A0\2.10.1.CFG.xml.
 * Configures the DDRT Major Mode behavior - Thresholds and starvation counters.
 */
typedef union {
  struct {
    UINT32 critical_starve : 5;
    /* critical_starve - Bits[4:0], RW_LB, default = 5'h8 
       
               Force the scheduler to switch from DDR4 to DDRT major mode even if a 
       single DDRT command is present in the DDRT queues. This transition is  
               triggered after the scheduler has remained in DDR4 major mode for a 
       critical starvation number of cycles and no DDRT transaction was scheduled.  
               The critical starvation threshold is calculated as 
       MODE_STARVE_CYCLE_THRESHOLD.DDRT X critical_starve. A value of 0 in this field 
       disables 
               the transition. 
               
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_MAJOR_MODE_THRESHOLD3_MC_2LM_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Configures the DDRT Major Mode behavior - Thresholds and starvation counters.
 */
typedef union {
  struct {
    UINT32 ddrt_critical_starve : 5;
    /* ddrt_critical_starve - Bits[4:0], RW_LB, default = 5'h8 
       
               Force the scheduler to switch from DDR4 to DDRT major mode even if a 
       single DDRT command is present in the DDRT queues. This transition is  
               triggered after the scheduler has remained in DDR4 major mode for a 
       critical starvation number of cycles and no DDRT transaction was scheduled.  
               The critical starvation threshold is calculated as 
       MODE_STARVE_CYCLE_THRESHOLD.DDRT X ddrt_critical_starve. A value of 0 in this 
       field disables 
               the transition. 
               
     */
    UINT32 ddr4_critical_starve : 5;
    /* ddr4_critical_starve - Bits[9:5], RW_LB, default = 5'h8 
       
               Force the scheduler to switch from DDRT to DDR4 major mode even if a 
       single DDR4 command is present in the DDR4 queues. This transition is  
               triggered after the scheduler has remained in DDRT major mode for a 
       critical starvation number of cycles and no DDRT transaction was scheduled.  
               The critical starvation threshold is calculated as 
       MODE_STARVE_CYCLE_THRESHOLD.DDR4 X ddr4_critical_starve. A value of 0 in this 
       field disables 
               the transition. 
               
     */
    UINT32 ddrt_mode_pcommit_thr : 6;
    /* ddrt_mode_pcommit_thr - Bits[15:10], RW_LB, default = 6'h28 
       
               B0 HSD b306217 - Force scheduler to switch from DDR4 to DDRT when min 
       DDR4 mode cycle time residency is met and pcommit is pending in WPQ with  
               number of pcommit writes greater than this CR value 
       (ddrt_mode_pcommit_thr). - default 'd40 
               
     */
    UINT32 ddrt_wmm_mode_pcommit_thr : 6;
    /* ddrt_wmm_mode_pcommit_thr - Bits[21:16], RW_LB, default = 6'h28 
       
               B0 HSD b306217 - Force DDRT WMM state machine to switch to WMM when min 
       residency in RMM is met and when pcommit is pending with writes greater 
               than this CR value (ddrt_wmm_mode_pcommit_thr) - default 'd40
               
     */
    UINT32 ddrt_wmm_mode_exit_pcommit_thr : 6;
    /* ddrt_wmm_mode_exit_pcommit_thr - Bits[27:22], RW_LB, default = 6'h08 
       
               B0 HSD b306217 - Force DDRT WMM state machine to switch to WMM when min 
       residency in RMM is met and when pcommit is pending with writes greater 
               than this CR value (ddrt_wmm_mode_exit_pcommit_thr) - default 'd8 
       leaving same as cr_ddrt_major_mode_threshold1_wmm_exit_DnnnH 
               
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_MAJOR_MODE_THRESHOLD3_MC_2LM_STRUCT;



/* DDRT_RPQL_MSK_MC_2LM_REG supported on:                                     */
/*       SKX_A0 (0x40251A40)                                                  */
/*       SKX (0x40251A40)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_RPQL_MSK_MC_2LM_REG 0x07014A40
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * TRPQ[31:0] entry mask. Masking off the entry when set to one.
 */
typedef union {
  struct {
    UINT32 ddrt_rpql_msk : 32;
    /* ddrt_rpql_msk - Bits[31:0], RW_LB, default = 32'h00000000 
       TRPQ[31:0] entry mask. Masking off the entry when set to one.
     */
  } Bits;
  UINT32 Data;
} DDRT_RPQL_MSK_MC_2LM_STRUCT;


/* DDRT_RPQH_MSK_MC_2LM_REG supported on:                                     */
/*       SKX_A0 (0x20251A44)                                                  */
/*       SKX (0x20251A44)                                                     */
/* Register default value:              0x0000                                */
#define DDRT_RPQH_MSK_MC_2LM_REG 0x07012A44
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * TRPQ[47:32] entry mask. Masking off the entry when set to one.
 */
typedef union {
  struct {
    UINT16 ddrt_rpqh_msk : 16;
    /* ddrt_rpqh_msk - Bits[15:0], RW_LB, default = 16'h0000 
       TRPQ[47:32] entry mask. Masking off the entry when set to one.
     */
  } Bits;
  UINT16 Data;
} DDRT_RPQH_MSK_MC_2LM_STRUCT;


/* DDRT_WPQ_MSK_MC_2LM_REG supported on:                                      */
/*       SKX_A0 (0x40251A48)                                                  */
/*       SKX (0x40251A48)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_WPQ_MSK_MC_2LM_REG 0x07014A48
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * TWPQ entry mask. Masking off the entry when set to one.
 */
typedef union {
  struct {
    UINT32 ddrt_wpq_msk : 32;
    /* ddrt_wpq_msk - Bits[31:0], RW_LB, default = 32'h00000000 
       TWPQ entry mask. Masking off the entry when set to one.
     */
  } Bits;
  UINT32 Data;
} DDRT_WPQ_MSK_MC_2LM_STRUCT;


/* DDRT_RETRY_TIMER2_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251A4C)                                                  */
/*       SKX (0x40251A4C)                                                     */
/* Register default value:              0x00007FFF                            */
#define DDRT_RETRY_TIMER2_MC_2LM_REG 0x07014A4C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Retry Thresholds
 */
typedef union {
  struct {
    UINT32 train_reset_time : 8;
    /* train_reset_time - Bits[7:0], RW_LB, default = 8'b11111111 
       Train reset duration. Program to the same value as link 
       retry_timer2.train_reset_time 
     */
    UINT32 retry_cmpl_time : 7;
    /* retry_cmpl_time - Bits[14:8], RW_LB, default = 7'b1111111 
       Wait time in completion state (SUCCESS or FAILURE). Program to the same value as 
       link retry_timer2.retry_cmpl_time 
     */
    UINT32 rsvd : 17;
    /* rsvd - Bits[31:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_RETRY_TIMER2_MC_2LM_STRUCT;


/* DDRT_DEFEATURE_MC_2LM_REG supported on:                                    */
/*       SKX_A0 (0x40251A50)                                                  */
/*       SKX (0x40251A50)                                                     */
/* Register default value on SKX_A0:    0x00008000                            */
/* Register default value on SKX:       0x00028000                            */
#define DDRT_DEFEATURE_MC_2LM_REG 0x07014A50
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * TWPQ entry mask. Masking off the entry when set to one.
 */
typedef union {
  struct {
    UINT32 mm_use_rt_empty : 1;
    /* mm_use_rt_empty - Bits[0:0], RW_LB, default = 1'b0 
       Use RT empty before switching major mode
     */
    UINT32 ignore_erid_parity_error : 1;
    /* ignore_erid_parity_error - Bits[1:1], RW_LB, default = 1'b0 
       
               Set to 1 to block the ERID parity error detection. Default is
               to leave the error condition enabled. 
               
     */
    UINT32 ignore_erid_fatal_error : 1;
    /* ignore_erid_fatal_error - Bits[2:2], RW_LB, default = 1'b0 
       
                Set to 1 to force MC to ignore the ERID Fatal error. This error fires
                when MC detects a RPQ or underfill Read data packet where RID did not 
                match the ERID. Default behavior is to treat the
                error as a fatal condition. If this bit is set, MC will block the
                fatal error and instead trigger the Error Flow FSM. 
               
     */
    UINT32 force_pcommit_ack : 1;
    /* force_pcommit_ack - Bits[3:3], RW_LB, default = 1'b0 
       
                Set to 1 to force DDRT scheduler to issue an Ack for a pcommit 
               
     */
    UINT32 ignore_pcommit : 1;
    /* ignore_pcommit - Bits[4:4], RW_LB, default = 1'b0 
       
                Set to 1 to for DDRT scheduler to ignore pcommit command. MC
                will send an ACK when a pcommit command is seen without
                draining the WPQ. 
               
     */
    UINT32 dis_thermal_event : 1;
    /* dis_thermal_event - Bits[5:5], RW_LB, default = 1'b0 
       disable DDRT Thermal event
     */
    UINT32 fnv_iot_trig_match : 2;
    /* fnv_iot_trig_match - Bits[7:6], RW_LB, default = 2'b0 
       
                This field is matched with the directory bits in the FNV IOT
                packet to generate a trigger. A value of 0 disables the
                trigger. 
               
     */
    UINT32 pcommit_ack_for_linkfail : 1;
    /* pcommit_ack_for_linkfail - Bits[8:8], RW_LB, default = 1'b0 
       
                Default behavior is for pcommit acks to be blocked once a
                channel hits link fail condition. If this bit is set, MC will send an
                ACK for pcommit even in link fail condition. This bit is used
                for pcommits directed to DDRT.
               
     */
    UINT32 pcommit_snapshot_all_wrs : 1;
    /* pcommit_snapshot_all_wrs - Bits[9:9], RW_LB, default = 1'b0 
       
                Default behavior of pcommit is to snapshot all outstanding
                writes to persistent or block region when a pcommit instruction is 
       seen.  
                If this bit is set then debug defeature is set then snapshot ALL wrs in 
       DDRT WPQ. 
               
     */
    UINT32 pkgc_wait_ddrt_wr_credit : 1;
    /* pkgc_wait_ddrt_wr_credit - Bits[10:10], RW_LB, default = 1'b0 
       
                Queue empty indication to PkGC flows will wait for all DDRT
                write credits to be returned by FNV
               
     */
    UINT32 ignore_ddrt_req : 2;
    /* ignore_ddrt_req - Bits[12:11], RW_LB, default = 1'b0 
       
               Set each bit in this field to 1 to force MC to ignore incoming
               REQ pin. Bit 11 is for Slot 0 and Bit 12 is for Slot 1 
               
     */
    UINT32 ignore_erid : 2;
    /* ignore_erid - Bits[14:13], RW_LB, default = 1'b0 
       
               Set each bit in this field to 1 to force MC to ignore incoming
               ERID pin. Bit 11 is for Slot 0 and Bit 12 is for Slot 1 
               
     */
    UINT32 ignore_viral : 1;
    /* ignore_viral - Bits[15:15], RW_LB, default = 1'b1 
       
               Set this bit to 1 to force the DDRT Retry FSM to ignore Viral
               condiion. When set, VIRAL command is not issued to FNV and DDRT FSM
               will not go to Link Fail.
               
     */
    UINT32 ignore_retry_cmd : 1;
    /* ignore_retry_cmd - Bits[16:16], RW_LB, default = 1'b0 
       
               Set this bit to 1 to force the DDRT to ignore the Retry bit
               from M2M. DDRT transactions will not go through the ECC
               correction path if this bit is set. 
               
     */
    UINT32 ignore_erid_notrd_err : 1;
    /* ignore_erid_notrd_err - Bits[17:17], RW_LB, default = 1'b1 
       
               Set this bit to 1 to force the DDRT to ignore the check when
               ERID does not match a read packet but RID does.
               
     */
    UINT32 ignore_retry_for_ndp : 1;
    /* ignore_retry_for_ndp - Bits[18:18], RW_LB, default = 1'b0 
       
               Set this bit to prevent No data packet transactions from using
               the correction path. 
               
     */
    UINT32 ignore_pending_retry_delay : 1;
    /* ignore_pending_retry_delay - Bits[19:19], RW_LB, default = 1'b0 
       
               Set this bit to ignore pending retry command check used to add
               the delay programmed in t_GNT_DQ_RETRY. 
               
     */
    UINT32 block_gnt2cmd_1cyc : 1;
    /* block_gnt2cmd_1cyc - Bits[20:20], RW_LB, default = 1'b0 
       
               Set this bit to prevent a DDRT command from being scheduling
               in the Dclk after a GNT. Blocking subsequent to this cycle
               will be handled by the safe logic blocking. 
               
     */
    UINT32 ddrt_defeature : 11;
    /* ddrt_defeature - Bits[31:21], RW_LB, default = 11'h0 
       
               Bit 21: If this bit is set to 1 then Idle to CKE FSM will never assert 
       for non-zero Req count 
               Bit 22: If this bit is set to 1 then it disables the fix for SKX HSD 
       b303440 (chicken bit). 
               Bit 23: If this bit is set to 1 then DDRT Retry flow will not prevent 
       CKE APD or PPD  
               Bit 24: If this bit is set to 1 then it disables the write credit 
       portion of the b303858 fix.  
               Bit 25: If this bit is set to 1 then it disables the read credit portion 
       of the b303858 fix.  
               Bit 26: Spare 
               Bit 27: If this bit is set to 1 then it disabled fix for b304239 
               Bits 31 to 28 are spare.
               
     */
  } Bits;
  UINT32 Data;
} DDRT_DEFEATURE_MC_2LM_STRUCT;


/* T_DDRT_MISC_DELAY_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251A54)                                                  */
/*       SKX (0x40251A54)                                                     */
/* Register default value:              0x1E260884                            */
#define T_DDRT_MISC_DELAY_MC_2LM_REG 0x07014A54
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 rpq_rid_to_credit_ret : 5;
    /* rpq_rid_to_credit_ret - Bits[4:0], RW_LB, default = 5'h4 
       Controls the delay to return RPQ credit to M2M after ERID is received 
     */
    UINT32 wpq_rid_to_fwr : 5;
    /* wpq_rid_to_fwr - Bits[9:5], RW_LB, default = 5'h4 
       Controls the delay to move WPQ Partial to Full Write credit after ERID is 
       received  
     */
    UINT32 wpq_dealloc_to_credit_ret : 4;
    /* wpq_dealloc_to_credit_ret - Bits[13:10], RW_LB, default = 4'h2 
       
               CSR controls the delay to retrun the WPQ credit back to M2M
               after the Write data is read Write data buffer for the memory
               write. The maximum value that this can be programmed to is 12. 
               
     */
    UINT32 force_ddrt_mode : 1;
    /* force_ddrt_mode - Bits[14:14], RW_LB, default = 1'b0  */
    UINT32 force_ddr4_mode : 1;
    /* force_ddr4_mode - Bits[15:15], RW_LB, default = 1'b0  */
    UINT32 invld_erid_retry : 4;
    /* invld_erid_retry - Bits[19:16], RW_LB, default = 4'h6 
       
               Control the delay to block Read Valid for non M2M read transaction
               when the transaction is using the correction path. 
               
     */
    UINT32 wpq_rid_to_rt_ufill : 5;
    /* wpq_rid_to_rt_ufill - Bits[24:20], RW_LB, default = 4'h2 
       
               Controls the delay from wpq ERID to underfill indication on read return 
       path.  
               The formula to program this is (t_ddrt_tcl - 13) 
               
     */
    UINT32 wpq_flush_to_dealloc : 5;
    /* wpq_flush_to_dealloc - Bits[29:25], RW_LB, default = 5'hf 
       This field is not used. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[30:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 enable_ddrt_2n_timing : 1;
    /* enable_ddrt_2n_timing - Bits[31:31], RW_LB, default = 1'b0 
       
               Set this field to 1 to enable DDRT 2N Timing. 
               
     */
  } Bits;
  UINT32 Data;
} T_DDRT_MISC_DELAY_MC_2LM_STRUCT;


/* DDRT_WDB_PAR_ERR_CTL_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x40251A58)                                                  */
/*       SKX (0x40251A58)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_WDB_PAR_ERR_CTL_MC_2LM_REG 0x07014A58
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Write Data Buffer error injection control
 */
typedef union {
  struct {
    UINT32 inj_mask : 16;
    /* inj_mask - Bits[15:0], RW_LB, default = 16'b0000000000000000 
       16 bit mask for injection on WDB read parity value
     */
    UINT32 enable : 1;
    /* enable - Bits[16:16], RW_LB, default = 1'b0 
       Parity error will be injected according to inj_mask on the next WDB read after 
       this bit is set. Enable bit will be unset after injection. 
     */
    UINT32 dis_wdb_par_chk : 1;
    /* dis_wdb_par_chk - Bits[17:17], RW_LB, default = 1'b0 
       Disable Write Data Buffer parity checking
     */
    UINT32 dis_ha_par_chk : 1;
    /* dis_ha_par_chk - Bits[18:18], RW_LB, default = 1'b0 
       Disable HA write data parity checking
     */
    UINT32 rsvd : 13;
    /* rsvd - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_WDB_PAR_ERR_CTL_MC_2LM_STRUCT;


/* DDRT_FNV0_EVENT0_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251A60)                                                  */
/*       SKX (0x40251A60)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_FNV0_EVENT0_MC_2LM_REG 0x07014A60
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT DIMM0 FNV low priority event log.
 */
typedef union {
  struct {
    UINT32 log : 24;
    /* log - Bits[23:0], RWS_V, default = 24'h000000 
       FNV event log
     */
    UINT32 id : 3;
    /* id - Bits[26:24], RWS_V, default = 1'b0 
       Packet ID of interrupt packet
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 err_select : 1;
    /* err_select - Bits[28:28], RWS_L, default = 1'b0 
       FNV event ERR0 assertion select. Upon logging a FNV low priority event, iMC to 
       generate a msgchn posted write to IIO to assert ERR0 pin if this select mask is 
       configured to one by BIOS during boot. Bit 29:28 can have all four combinations. 
       When both are set, both LINK and ERR# are armed. 
     */
    UINT32 smi_select : 1;
    /* smi_select - Bits[29:29], RWS_L, default = 1'b0 
       FNV event SMM interrupt select. Upon logging a FNV low priority event, iMC to 
       generate a msgchn posted write to UBox to generate LINK if this select mask is 
       configured to one by BIOS during boot. Bit 29:28 can have all four combinations. 
       When both are set, both LINK and ERR# are armed. 
     */
    UINT32 overflow : 1;
    /* overflow - Bits[30:30], RW1CS, default = 1'b0 
       FNV event overflow. 2nd event is detected before the EVNET_VALID is cleared by 
       interrupt handler. 
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RW1CS, default = 1'b0 
       FNV event valid. Event log is valid.
     */
  } Bits;
  UINT32 Data;
} DDRT_FNV0_EVENT0_MC_2LM_STRUCT;


/* DDRT_FNV0_EVENT1_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251A64)                                                  */
/*       SKX (0x40251A64)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_FNV0_EVENT1_MC_2LM_REG 0x07014A64
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT DIMM0 FNV high priority event log.
 */
typedef union {
  struct {
    UINT32 log : 24;
    /* log - Bits[23:0], RWS_V, default = 24'h000000 
       FNV event log
     */
    UINT32 id : 3;
    /* id - Bits[26:24], RWS_V, default = 1'b0 
       Packet ID of interrupt packet
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 err_select : 1;
    /* err_select - Bits[28:28], RWS_L, default = 1'b0 
       FNV event ERR0 assertion select. Upon logging a FNV low priority event, iMC to 
       generate a msgchn posted write to IIO to assert ERR0 pin if this select mask is 
       configured to one by BIOS during boot. Bit 29:28 can have all four combinations. 
       When both are set, both LINK and ERR# are armed. 
     */
    UINT32 smi_select : 1;
    /* smi_select - Bits[29:29], RWS_L, default = 1'b0 
       FNV event SMM interrupt select. Upon logging a FNV low priority event, iMC to 
       generate a msgchn posted write to UBox to generate LINK if this select mask is 
       configured to one by BIOS during boot. Bit 29:28 can have all four combinations. 
       When both are set, both LINK and ERR# are armed. 
     */
    UINT32 overflow : 1;
    /* overflow - Bits[30:30], RW1CS, default = 1'b0 
       FNV event overflow. 2nd event is detected before the EVNET_VALID is cleared by 
       interrupt handler. 
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RW1CS, default = 1'b0 
       FNV event valid. Event log is valid.
     */
  } Bits;
  UINT32 Data;
} DDRT_FNV0_EVENT1_MC_2LM_STRUCT;


/* DDRT_FNV1_EVENT0_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251A70)                                                  */
/*       SKX (0x40251A70)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_FNV1_EVENT0_MC_2LM_REG 0x07014A70
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT DIMM1 FNV low priority event log.
 */
typedef union {
  struct {
    UINT32 log : 24;
    /* log - Bits[23:0], RWS_V, default = 24'h000000 
       FNV event log
     */
    UINT32 id : 3;
    /* id - Bits[26:24], RWS_V, default = 1'b0 
       Packet ID of interrupt packet
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 err_select : 1;
    /* err_select - Bits[28:28], RWS_L, default = 1'b0 
       FNV event ERR0 assertion select. Upon logging a FNV low priority event, iMC to 
       generate a msgchn posted write to IIO to assert ERR0 pin if this select mask is 
       configured to one by BIOS during boot. Bit 29:28 can have all four combinations. 
       When both are set, both LINK and ERR# are armed. 
     */
    UINT32 smi_select : 1;
    /* smi_select - Bits[29:29], RWS_L, default = 1'b0 
       FNV event SMM interrupt select. Upon logging a FNV low priority event, iMC to 
       generate a msgchn posted write to UBox to generate LINK if this select mask is 
       configured to one by BIOS during boot. Bit 29:28 can have all four combinations. 
       When both are set, both LINK and ERR# are armed. 
     */
    UINT32 overflow : 1;
    /* overflow - Bits[30:30], RW1CS, default = 1'b0 
       FNV event overflow. 2nd event is detected before the EVNET_VALID is cleared by 
       interrupt handler. 
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RW1CS, default = 1'b0 
       FNV event valid. Event log is valid.
     */
  } Bits;
  UINT32 Data;
} DDRT_FNV1_EVENT0_MC_2LM_STRUCT;


/* DDRT_FNV1_EVENT1_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251A74)                                                  */
/*       SKX (0x40251A74)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_FNV1_EVENT1_MC_2LM_REG 0x07014A74
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT DIMM1 FNV high priority event log.
 */
typedef union {
  struct {
    UINT32 log : 24;
    /* log - Bits[23:0], RWS_V, default = 24'h000000 
       FNV event log
     */
    UINT32 id : 3;
    /* id - Bits[26:24], RWS_V, default = 1'b0 
       Packet ID of interrupt packet
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 err_select : 1;
    /* err_select - Bits[28:28], RWS_L, default = 1'b0 
       FNV event ERR0 assertion select. Upon logging a FNV low priority event, iMC to 
       generate a msgchn posted write to IIO to assert ERR0 pin if this select mask is 
       configured to one by BIOS during boot. Bit 29:28 can have all four combinations. 
       When both are set, both LINK and ERR# are armed. 
     */
    UINT32 smi_select : 1;
    /* smi_select - Bits[29:29], RWS_L, default = 1'b0 
       FNV event SMM interrupt select. Upon logging a FNV low priority event, iMC to 
       generate a msgchn posted write to UBox to generate LINK if this select mask is 
       configured to one by BIOS during boot. Bit 29:28 can have all four combinations. 
       When both are set, both LINK and ERR# are armed. 
     */
    UINT32 overflow : 1;
    /* overflow - Bits[30:30], RW1CS, default = 1'b0 
       FNV event overflow. 2nd event is detected before the EVNET_VALID is cleared by 
       interrupt handler. 
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RW1CS, default = 1'b0 
       FNV event valid. Event log is valid.
     */
  } Bits;
  UINT32 Data;
} DDRT_FNV1_EVENT1_MC_2LM_STRUCT;


/* MODE_STARVE_CYCLE_THRESHOLD_MC_2LM_REG supported on:                       */
/*       SKX_A0 (0x40251A78)                                                  */
/*       SKX (0x40251A78)                                                     */
/* Register default value:              0x01000100                            */
#define MODE_STARVE_CYCLE_THRESHOLD_MC_2LM_REG 0x07014A78
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT idle cycle threshold before releasing current major mode
 */
typedef union {
  struct {
    UINT32 ddrt : 16;
    /* ddrt - Bits[15:0], RW_LB, default = 16'h100 
       
               This field controls the minimum duration that the major mode FSM remains 
       in DDRT mode. This  
               field additionally is used to calculate the number of cycles since exit 
       from DDRT major mode before  
               DDRT mode is considered starved. See 
       DDRT_MAJOR_MODE_THRESHOLD3.critical_starve 
               
     */
    UINT32 ddr4 : 16;
    /* ddr4 - Bits[31:16], RW_LB, default = 16'h100 
       
               This field controls the minimum duration that the major mode FSM remains 
       in DDR4 mode. This  
               field additionally is used to calculate the number of cycles since exit 
       from DDR4 major mode before  
               DDR4 mode is considered starved. See 
       DDRT_MAJOR_MODE_THRESHOLD3.critical_starve 
               
     */
  } Bits;
  UINT32 Data;
} MODE_STARVE_CYCLE_THRESHOLD_MC_2LM_STRUCT;


/* MODE_DELAYS_MC_2LM_REG supported on:                                       */
/*       SKX_A0 (0x20251A7C)                                                  */
/*       SKX (0x40251A7C)                                                     */
/* Register default value on SKX_A0:    0x1010                                */
/* Register default value on SKX:       0x10201010                            */
#define MODE_DELAYS_MC_2LM_REG 0x0701C000
/* Struct format extracted from XML file SKX_A0\2.10.1.CFG.xml.
 * DDRT idle cycle threshold before releasing current major mode
 */
typedef union {
  struct {
    UINT16 ddrt_to_ddr4_delay : 8;
    /* ddrt_to_ddr4_delay - Bits[7:0], RW_LB, default = 8'h10 
       Delay while switching from DDRT to DDR4 major mode. During this period, the MC 
       will continue to remain in DDRT mode but no new DDRT transactions are scheduled 
     */
    UINT16 ddr4_to_ddrt_delay : 8;
    /* ddr4_to_ddrt_delay - Bits[15:8], RW_LB, default = 8'h10 
       Delay while switching from DDR4 to DDRT major mode. During this period, the MC 
       will continue to remain in DDR4 mode but no new DDR4 transactions are scheduled 
     */
  } Bits;
  UINT16 Data;
} MODE_DELAYS_MC_2LM_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT idle cycle threshold before releasing current major mode
 */
typedef union {
  struct {
    UINT32 ddrt_to_ddr4_delay : 8;
    /* ddrt_to_ddr4_delay - Bits[7:0], RW_LB, default = 8'h10 
       Delay while switching from DDRT to DDR4 major mode. During this period, the MC 
       will continue to remain in DDRT mode but no new DDRT transactions are scheduled 
     */
    UINT32 ddr4_to_ddrt_delay : 8;
    /* ddr4_to_ddrt_delay - Bits[15:8], RW_LB, default = 8'h10 
       Delay while switching from DDR4 to DDRT major mode. During this period, the MC 
       will continue to remain in DDR4 mode but no new DDR4 transactions are scheduled 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[16:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reset_timer : 1;
    /* reset_timer - Bits[17:17], RW_LB, default = 1'b0 
       
               A 0 to 1 transition on this register will reset
               the various timers used in the DDR4 - DDRT major mode FSM
               
     */
    UINT32 wmm_gnt2gnt_delay : 5;
    /* wmm_gnt2gnt_delay - Bits[22:18], RW_LB, default = 5'b01000 
       
               Additional delay to add in GNT to GNT scheduling in WMM. 
               
     */
    UINT32 spare : 1;
    /* spare - Bits[23:23], RW_LB, default = 2'b0 
       spare
     */
    UINT32 reqs : 8;
    /* reqs - Bits[31:24], RW_LB, default = 8'h10 
       Threshold of outstanding REQ in DDRT scheduler to switch from DDR4 to DDRT mode
     */
  } Bits;
  UINT32 Data;
} MODE_DELAYS_MC_2LM_STRUCT;



/* MODE_DDRT_CMD_STARVE_THRESHOLD_MC_2LM_REG supported on:                    */
/*       SKX_A0 (0x40251A80)                                                  */
/*       SKX (0x40251A80)                                                     */
/* Register default value:              0x10101010                            */
#define MODE_DDRT_CMD_STARVE_THRESHOLD_MC_2LM_REG 0x07014A80
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT idle cycle threshold before releasing current major mode
 */
typedef union {
  struct {
    UINT32 reads : 8;
    /* reads - Bits[7:0], RW_LB, default = 8'h10 
       Threshold of outstanding reads in DDRT RPQ to hit DDRT starvation condition
     */
    UINT32 writes : 8;
    /* writes - Bits[15:8], RW_LB, default = 8'h10 
       Threshold of outstanding writes in DDRT WPQ to hit DDRT starvation condition
     */
    UINT32 partials : 8;
    /* partials - Bits[23:16], RW_LB, default = 8'h10 
       Threshold of outstanding partials writes in DDRT WPQ to hit DDRT starvation 
       condition 
     */
    UINT32 gnts : 8;
    /* gnts - Bits[31:24], RW_LB, default = 8'h10 
       Threshold of outstanding GNTs in DDRT RPQ to hit DDRT starvation condition
     */
  } Bits;
  UINT32 Data;
} MODE_DDRT_CMD_STARVE_THRESHOLD_MC_2LM_STRUCT;


/* MODE_MIN_RESIDENCY_THRESHOLD_MC_2LM_REG supported on:                      */
/*       SKX_A0 (0x40251A84)                                                  */
/*       SKX (0x40251A84)                                                     */
/* Register default value:              0x00200020                            */
#define MODE_MIN_RESIDENCY_THRESHOLD_MC_2LM_REG 0x07014A84
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT idle cycle threshold before releasing current major mode
 */
typedef union {
  struct {
    UINT32 ddrt : 16;
    /* ddrt - Bits[15:0], RW_LB, default = 16'h20 
       This field is not used. Refer to MODE_STARVE_CYCLE_THRESHOLD.DDRT
     */
    UINT32 ddr4 : 16;
    /* ddr4 - Bits[31:16], RW_LB, default = 16'h20 
       This field is not used. Refer to MODE_STARVE_CYCLE_THRESHOLD.DDR4
     */
  } Bits;
  UINT32 Data;
} MODE_MIN_RESIDENCY_THRESHOLD_MC_2LM_STRUCT;


/* DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_REG supported on:                        */
/*       SKX_A0 (0x40251A88)                                                  */
/*       SKX (0x40251A88)                                                     */
/* Register default value:              0x05100540                            */
#define DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_REG 0x07014A88
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 pwmm_starv_cntr_prescaler : 8;
    /* pwmm_starv_cntr_prescaler - Bits[7:0], RW, default = 8'b01000000 
       DDR-T Partial Write Starvation Counter Pre-Scaler
       When the counter increment to the STARV_CNTR_PRESCALER value, the 
       STARV_CNTR_PRESCALER counter will be roll over back to zero and start counting. 
       There is a 3b partial write starvation counter for each partial write in the 
       write queue. 
       At the roll over, all 3b partial write starvation counters will be incremented 
       by 1. The 3b pwr starvation counter will saturated at its max value to indicate 
       the partial write starvation condition. The 3b counter will be cleared when the 
       partial write is served. 
     */
    UINT32 gnt_exit : 8;
    /* gnt_exit - Bits[15:8], RW_LB, default = 8'h05  */
    UINT32 pwr_enter : 8;
    /* pwr_enter - Bits[23:16], RW_LB, default = 8'h10  */
    UINT32 pwr_exit : 8;
    /* pwr_exit - Bits[31:24], RW_LB, default = 8'h05  */
  } Bits;
  UINT32 Data;
} DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_STRUCT;


/* DDRT_RETRY_TIMER_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251A8C)                                                  */
/*       SKX (0x40251A8C)                                                     */
/* Register default value:              0x40200119                            */
#define DDRT_RETRY_TIMER_MC_2LM_REG 0x07014A8C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT Retry Thresholds
 */
typedef union {
  struct {
    UINT32 retry_on_timer_limit : 6;
    /* retry_on_timer_limit - Bits[5:0], RW_LB, default = 6'h19 
       Sets the limit as (2^retry_on_timer_limit) DCLKs for duration when Retry FSM can 
       be 
                            active before an error is triggered.
               
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 disable_retry_timer : 1;
    /* disable_retry_timer - Bits[8:8], RW_LB, default = 1'b1 
       
               Set to 0 to enable watch dog timer for DDRT Retry FSM
               timer. The timer limit is programmed as (2^retry_on_timer_limit) DCLKs 
       for duration when Retry FSM can be 
               active before an error is triggered.
               
     */
    UINT32 rsvd_9 : 7;
    /* rsvd_9 - Bits[15:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 min_delay_limit : 8;
    /* min_delay_limit - Bits[23:16], RW_LB, default = 8'h20 
       
               Sets the limit in DCLKs for minimum duration that certain Retry
               states should remain before transitioning out of that state.  
               
     */
    UINT32 min_rt_delay : 8;
    /* min_rt_delay - Bits[31:24], RW_LB, default = 8'h40 
       
               Sets the limit in DCLKs for for certain Retry that check
               events that depend on the round trip channel delay. 
               
     */
  } Bits;
  UINT32 Data;
} DDRT_RETRY_TIMER_MC_2LM_STRUCT;


/* DDRT_RETRY_LINK_FAIL_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x40251A90)                                                  */
/*       SKX (0x40251A90)                                                     */
/* Register default value:              0x00200000                            */
#define DDRT_RETRY_LINK_FAIL_MC_2LM_REG 0x07014A90
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 threshold : 16;
    /* threshold - Bits[15:0], RW_LB, default = 16'h0 
       
                If the number of Error FSM interations within a DCLK interval 
                defined by DDRT_RETRY_LINK_FAIL.window exceeds the value
                programmed in DDRT_RETRY_LINK_FAIL.threshold, then a Link
                Fail condition is triggered. If the threshold is programed to 0
                then the Link Fail condition is disabled. 
               
     */
    UINT32 window : 6;
    /* window - Bits[21:16], RW_LB, default = 6'h20 
       
                If the number of Error FSM interations within a DCLK interval 
                defined by (2^DDRT_RETRY_LINK_FAIL.window) exceeds the value
                programmed in DDRT_RETRY_LINK_FAIL.threshold, then a Link
                Fail condition is triggered. If the threshold is programed to 0
                then the Link Fail condition is disabled. 
               
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_RETRY_LINK_FAIL_MC_2LM_STRUCT;


/* DDRT_RETRY_CTL_MC_2LM_REG supported on:                                    */
/*       SKX_A0 (0x40251A94)                                                  */
/*       SKX (0x40251A94)                                                     */
/* Register default value:              0x021D0160                            */
#define DDRT_RETRY_CTL_MC_2LM_REG 0x07014A94
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 trigger_retry : 1;
    /* trigger_retry - Bits[0:0], RW_LB, default = 1'b0 
       
               A 0 to 1 transition in this fields force the Retry FSM out of Idle. 
               
     */
    UINT32 reissue_err_ack : 1;
    /* reissue_err_ack - Bits[1:1], RW_LB, default = 1'b0 
       
               Set this bit to force the Retry FSM to keep injecting the Error Ack 
       Command until the Error  
               signal is de-asserted. If this bit is not set, then the FSM  will inject 
        
               Err Ack once and go to Link Fail is the DDR-T Error pin from any 
       populated DDR-T slot is not de-asserted. 
               
     */
    UINT32 reset_err_cnt : 1;
    /* reset_err_cnt - Bits[2:2], RW_LB, default = 1'b0 
       
               Set this bit to reset all error counts and timers related to DDRT Retry 
       FSM.  
               The count will remain at 0 as long as this bit is a 1
               
     */
    UINT32 rsvd_3 : 1;
    /* rsvd_3 - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 inj_cnt_limit : 4;
    /* inj_cnt_limit - Bits[7:4], RW_LB, default = 4'h6 
       
               Number of times Force Error command is issued unsuccessfully in each 
       DDRT 
               Retry flow iteration to trigger Link Fail condition. A value of 0 
               in this field disables the check. 
               
     */
    UINT32 queue_empty_chk : 1;
    /* queue_empty_chk - Bits[8:8], RW_LB, default = 1'b1 
       
               When set to 1, DDRT retry timer should always count when DDRT queues are 
       empty 
               When set to 0, DDRT retry timer will count when the MC is in DDRT mode 
       irrespective of whether the DDRT queues are empty or not. This is a debug option 
        
               
     */
    UINT32 force_write_drain : 1;
    /* force_write_drain - Bits[9:9], RW_LB, default = 1'b0 
       
                  Default behavior is to unblock DDRT reads as soon as FNV_ERR is 
       de-asserted and 
                  to not wait for writes to drain. If the CSR bit is set then Retry FSM 
       will 
                  wait for writes to drain before going back to Idle. 
               
     */
    UINT32 force_timer_non_ddrt_mode : 7;
    /* force_timer_non_ddrt_mode - Bits[16:10], RW_LB, default = 7'b1000000 
       
                  For wait timer in each state, force timer to count when not in DDRT 
       mode. 
                  Bit 10 - FORCE_ERROR state
                  Bit 11 - RD_FLUSH state
                  Bit 12 - RD_DRAIN state
                  Bit 13 - POST_RD_DRAIN state
                  Bit 14 - ISSUE_ERR_ACK state
                  Bit 15 - FAILURE state
                  Bit 16 - SUCCESS state
               
     */
    UINT32 link_fail_max_rd : 4;
    /* link_fail_max_rd - Bits[20:17], RW_LB, default = 4'b1110 
       
               Maximum number of DDRT reads allowed after link goes to link fail mode. 
               
     */
    UINT32 link_fail_block_width : 5;
    /* link_fail_block_width - Bits[25:21], RW_LB, default = 5'd16 
       
               Number of cycles to assert block to M2M for after a  DDRT read
               in DDRT Link Fail state. 
               
     */
    UINT32 link_fail_block_disable : 1;
    /* link_fail_block_disable - Bits[26:26], RW_LB, default = 1'b0 
       
               Set this bit so that DDRT Retry FSM does not assert Block to
               M2M for DDRT Reads in Retry Link Fail State. 
               
     */
    UINT32 rsvd_27 : 5;
    /* rsvd_27 - Bits[31:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_RETRY_CTL_MC_2LM_STRUCT;


/* DDRT_RETRY_STATUS_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251A98)                                                  */
/*       SKX (0x40251A98)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_RETRY_STATUS_MC_2LM_REG 0x07014A98
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 16;
    UINT32 current_retry_state : 4;
    /* current_retry_state - Bits[19:16], RO_V, default = 1'b0 
       
               Current state of the Retry FSM 
               
     */
    UINT32 rsvd_20 : 2;
    /* rsvd_20 - Bits[21:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} DDRT_RETRY_STATUS_MC_2LM_STRUCT;




/* DDRT_DIMM_INFO_DIMM0_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x40251AA0)                                                  */
/*       SKX (0x40251AA0)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_DIMM_INFO_DIMM0_MC_2LM_REG 0x07014AA0
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Log the DIMM info packet provided by FNV DIMM 0
 */
typedef union {
  struct {
    UINT32 base_address : 12;
    /* base_address - Bits[11:0], RO_V, default = 12'b0 
       
               Base address of the indirectly mapped region, in multiples of 64GB, 
       expressed as a DIMM address 
               
     */
    UINT32 length : 12;
    /* length - Bits[23:12], RO_V, default = 12'b0 
       
               Length of the indirectly mapped region, in multiples of 64GB
               
     */
    UINT32 base_addr_valid : 1;
    /* base_addr_valid - Bits[24:24], RO_V, default = 1'b0 
       
               Base address specified in bits 11:0 is valid
               
     */
    UINT32 length_valid : 1;
    /* length_valid - Bits[25:25], RO_V, default = 1'b0 
       
               Length specified in bits 23:12 is valid
               
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[30:26], RO_V, default = 5'b0 
       
               Reserved for future use
               
     */
    UINT32 intel_dimm : 1;
    /* intel_dimm - Bits[31:31], RO_V, default = 1'b0 
       
               0 - non-Intel DIMM
               1 - Intel DIMM
               
     */
  } Bits;
  UINT32 Data;
} DDRT_DIMM_INFO_DIMM0_MC_2LM_STRUCT;


/* DDRT_DIMM_INFO_DIMM1_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x40251AA4)                                                  */
/*       SKX (0x40251AA4)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_DIMM_INFO_DIMM1_MC_2LM_REG 0x07014AA4
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Log the DIMM info packet provided by FNV DIMM 1
 */
typedef union {
  struct {
    UINT32 base_address : 12;
    /* base_address - Bits[11:0], RO_V, default = 12'b0 
       
               Base address of the indirectly mapped region, in multiples of 64GB, 
       expressed as a DIMM address 
               
     */
    UINT32 length : 12;
    /* length - Bits[23:12], RO_V, default = 12'b0 
       
               Length of the indirectly mapped region, in multiples of 64GB
               
     */
    UINT32 base_addr_valid : 1;
    /* base_addr_valid - Bits[24:24], RO_V, default = 1'b0 
       
               Base address specified in bits 11:0 is valid
               
     */
    UINT32 length_valid : 1;
    /* length_valid - Bits[25:25], RO_V, default = 1'b0 
       
               Length specified in bits 23:12 is valid
               
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[30:26], RO_V, default = 5'b0 
       
               Reserved for future use
               
     */
    UINT32 intel_dimm : 1;
    /* intel_dimm - Bits[31:31], RO_V, default = 1'b0 
       
               0 - non-Intel DIMM
               1 - Intel DIMM
               
     */
  } Bits;
  UINT32 Data;
} DDRT_DIMM_INFO_DIMM1_MC_2LM_STRUCT;


/* DDRT_CLK_GATING_MC_2LM_REG supported on:                                   */
/*       SKX_A0 (0x40251AA8)                                                  */
/*       SKX (0x40251AA8)                                                     */
/* Register default value:              0x00000040                            */
#define DDRT_CLK_GATING_MC_2LM_REG 0x07014AA8
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * CSR to program clock gating for DDRT scheduler
 */
typedef union {
  struct {
    UINT32 reqcnt_timer : 10;
    /* reqcnt_timer - Bits[9:0], RW_LB, default = 10'h40 
       
               Timer to keep clocks running afer MC issues a GNT for last outstanding 
       REQ. This should more than the DDRT round trip 
               delay. Program to 0 to disable the timer, that is, timer will never 
       start and will not have any impact on the clock gating.  
               Program to all 1s to keep the timer always running. 
               
     */
    UINT32 rsvd_10 : 10;
    /* rsvd_10 - Bits[19:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 en_ddrt_dfd_clk : 1;
    /* en_ddrt_dfd_clk - Bits[20:20], RW_LB, default = 1'b0 
       
               This bit must be set to 1 to enable various debug features in
               the DDRT scheduler logic. This bit need not be set to 1 for normal
               functional operation. 
               
     */
    UINT32 dis_major_mode_clkgating : 1;
    /* dis_major_mode_clkgating - Bits[21:21], RW_LB, default = 1'b0 
       
               Disable clock gating for the DDRT major mode logic 
               
     */
    UINT32 dis_safe_clkgating : 1;
    /* dis_safe_clkgating - Bits[22:22], RW_LB, default = 1'b0 
       
               Disable clock gating for the DDRT safe logic 
               
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dis_retry_clkgating : 1;
    /* dis_retry_clkgating - Bits[24:24], RW_LB, default = 1'b0 
       
               Disable clock gating for the DDRT Retry FSM 
               
     */
    UINT32 dis_wdb_clkgating : 1;
    /* dis_wdb_clkgating - Bits[25:25], RW_LB, default = 1'b0 
       
               Disable clock gating for the DDRT WPQ
               
     */
    UINT32 dis_wpq_clkgating : 1;
    /* dis_wpq_clkgating - Bits[26:26], RW_LB, default = 1'b0 
       
               Disable clock gating for the DDRT WPQ
               
     */
    UINT32 dis_wpq_fsm_clkgating : 1;
    /* dis_wpq_fsm_clkgating - Bits[27:27], RW_LB, default = 1'b0 
       
               Disable clock gating for the DDRT WPQ FSM logic 
               
     */
    UINT32 dis_revaddr_log_clkgating : 1;
    /* dis_revaddr_log_clkgating - Bits[28:28], RW_LB, default = 1'b0 
       
               Disable clock gating used to store addresses for reverse address decode 
               
     */
    UINT32 dis_rpq_clkgating : 1;
    /* dis_rpq_clkgating - Bits[29:29], RW_LB, default = 1'b0 
       
               Disable clock gating for in the DDRT RPQ block 
               
     */
    UINT32 dis_cpgc_clkgating : 1;
    /* dis_cpgc_clkgating - Bits[30:30], RW_LB, default = 1'b0 
       
               Disable clock gating for CPGC logic in the DDRT block 
               
     */
    UINT32 dis_ddrts_clkgating : 1;
    /* dis_ddrts_clkgating - Bits[31:31], RW_LB, default = 1'b0 
       
               Disable clock gating in DDRTS block. 
               
     */
  } Bits;
  UINT32 Data;
} DDRT_CLK_GATING_MC_2LM_STRUCT;


/* DDRT_MISC_CTL_MC_2LM_REG supported on:                                     */
/*       SKX_A0 (0x40251AAC)                                                  */
/*       SKX (0x40251AAC)                                                     */
/* Register default value:              0x00000006                            */
#define DDRT_MISC_CTL_MC_2LM_REG 0x07014AAC
/* Struct format extracted from XML file SKX_A0\2.10.1.CFG.xml.
 * Misc defeatures for DDRt
 */
typedef union {
  struct {
    UINT32 en_dir_only_upd : 1;
    /* en_dir_only_upd - Bits[0:0], RW_LB, default = 1'b0 
       
                  0: Reads and writes will always be full cacheline
                  1: MemInvXto* reads will be directory-only reads, and DirUpd will be 
       directory-only writes. 
               
     */
    UINT32 dis_ddrt_opp_rd : 1;
    /* dis_ddrt_opp_rd - Bits[1:1], RW_LB, default = 1'b1 
       
                  1: DDRT RPQ Reads will not be scheduled in DDR4 mode
                  0: DDRT RPQ Reads will be scheduled in DDR4 mode. GNTs continue to be 
       blocked in DDR4 mode 
               This bit should be set for DDRT 2N mode. 
               
     */
    UINT32 dis_ddrt_opp_ufill_rd : 1;
    /* dis_ddrt_opp_ufill_rd - Bits[2:2], RW_LB, default = 1'b1 
       
                  1: DDRT Underfill Reads will not be scheduled in DDR4 mode
                  0: DDRT Underfill Reads will be scheduled in DDR4 mode. GNTs continue 
       to be blocked in DDR4 mode 
               This bit should be set for DDRT 2N mod.
               
     */
    UINT32 ddrt_dec_force_lat : 1;
    /* ddrt_dec_force_lat - Bits[3:3], RW_LB, default = 1'b0 
       
                  When setting to 1, force additional 1 cycle latency in decoding logic 
       (same latency as 3 channel mode). 
               
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_MISC_CTL_MC_2LM_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Misc defeatures for DDRt
 */
typedef union {
  struct {
    UINT32 en_dir_only_upd : 1;
    /* en_dir_only_upd - Bits[0:0], RW_LB, default = 1'b0 
       
                  0: Reads and writes will always be full cacheline
                  1: MemInvXto* reads will be directory-only reads, and DirUpd will be 
       directory-only writes. 
               
     */
    UINT32 dis_ddrt_opp_rd : 1;
    /* dis_ddrt_opp_rd - Bits[1:1], RW_LB, default = 1'b1 
       
                  1: DDRT RPQ Reads will not be scheduled in DDR4 mode
                  0: DDRT RPQ Reads will be scheduled in DDR4 mode. GNTs continue to be 
       blocked in DDR4 mode 
               This bit should be set for DDRT 2N mode. 
               
     */
    UINT32 dis_ddrt_opp_ufill_rd : 1;
    /* dis_ddrt_opp_ufill_rd - Bits[2:2], RW_LB, default = 1'b1 
       
                  1: DDRT Underfill Reads will not be scheduled in DDR4 mode
                  0: DDRT Underfill Reads will be scheduled in DDR4 mode. GNTs continue 
       to be blocked in DDR4 mode 
               This bit should be set for DDRT 2N mod.
               
     */
    UINT32 ddrt_dec_force_lat : 1;
    /* ddrt_dec_force_lat - Bits[3:3], RW_LB, default = 1'b0 
       
                  When setting to 1, force additional 1 cycle latency in decoding logic 
       (same latency as 3 channel mode). 
               
     */
    UINT32 ddr4_pipesys2chn : 1;
    /* ddr4_pipesys2chn - Bits[4:4], RW_LB, default = 1'b0 
       
                  When setting to 1, indicate additional 1 cycle latency is required in 
       DDR4 decoding logic (3 channel mode or forced). 
               
     */
    UINT32 viral_dummy_wr : 1;
    /* viral_dummy_wr - Bits[5:5], RW_LB, default = 1'b0 
       
               Viral condition will block DDRT writes. 
               
     */
    UINT32 link_fail_on_viral : 1;
    /* link_fail_on_viral - Bits[6:6], RW_LB, default = 1'b0 
       
               Viral condition will force DDRT FSM to go to Link fail condition. 
               
     */
    UINT32 rsvd : 17;
    /* rsvd - Bits[23:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 defeature : 8;
    /* defeature - Bits[31:24], RW_LB, default = 8'h0 
       
               Bit 29: Set to 1 to handle ERID default value as 00 
               Bit 28: Set to 1 to disable DDRT write byte enable parity check
               Bit 27: Set to 1 to disable checking DDRT write credits when
                       switching from DDR4 to DDRT mode at the minimum threshold. 
               Bit 26: Set to 1 for scheduler to return CGPC credit when DDRT Wr is 
       scheduled 
               Bit 25: WPQ can GNTs if reqCnt is non-zero in DDRT WMM and DDRT ADR is 
       active  
               Bit 24: Set to prevent WPQ from issuing GNTs if reqCnt is
                       non-zero in DDRT WMM while there are no FNV Write credits 
               
     */
  } Bits;
  UINT32 Data;
} DDRT_MISC_CTL_MC_2LM_STRUCT;



/* DDRT_VIRAL_CTL_MC_2LM_REG supported on:                                    */
/*       SKX_A0 (0x40251AB0)                                                  */
/*       SKX (0x40251AB0)                                                     */
/* Register default value:              0x00000085                            */
#define DDRT_VIRAL_CTL_MC_2LM_REG 0x07014AB0
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Bits to control viral signalling for DDRT Errors
 */
typedef union {
  struct {
    UINT32 dis_erid_par : 1;
    /* dis_erid_par - Bits[0:0], RWS, default = 1'b1 
       Disable viral triggering for ERID correctable Parity error. Viral signalling is 
       Error signalling is disabled by default 
     */
    UINT32 dis_erid_uc : 1;
    /* dis_erid_uc - Bits[1:1], RWS, default = 1'b0 
       Disable viral triggering for ERID fatal parity error
     */
    UINT32 dis_pending_intr_error : 1;
    /* dis_pending_intr_error - Bits[2:2], RWS, default = 1'b1 
       Spare unused bit
     */
    UINT32 dis_erid_fifo_error : 1;
    /* dis_erid_fifo_error - Bits[3:3], RWS, default = 1'b0 
       Disable viral triggering for ERID FIFO over flow or underflow error
     */
    UINT32 dis_wr_fnv_credit_error : 1;
    /* dis_wr_fnv_credit_error - Bits[4:4], RWS, default = 1'b0 
       Disable viral triggering when number of write credit returned exceeds number of 
       outstanding writes 
     */
    UINT32 dis_rd_fnv_credit_error : 1;
    /* dis_rd_fnv_credit_error - Bits[5:5], RWS, default = 1'b0 
       Disable viral triggering when number of read credit returned exceeds number of 
       outstanding reads 
     */
    UINT32 dis_ddrt_scheduler_mismatch : 1;
    /* dis_ddrt_scheduler_mismatch - Bits[6:6], RWS, default = 1'b0 
       Disable viral triggering when there is a mismatch is command scheduled at DDRT 
       and global scheduler 
     */
    UINT32 dis_fnv_error : 1;
    /* dis_fnv_error - Bits[7:7], RWS, default = 1'b1 
       Disable viral triggering when FNV Error pin is asserted
     */
    UINT32 dis_fnv_thermal_error : 1;
    /* dis_fnv_thermal_error - Bits[8:8], RWS, default = 1'b0 
       Disable viral triggering when FNV Error error is detected
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_VIRAL_CTL_MC_2LM_STRUCT;


/* DDRT_MCA_CTL_MC_2LM_REG supported on:                                      */
/*       SKX_A0 (0x40251AB4)                                                  */
/*       SKX (0x40251AB4)                                                     */
/* Register default value:              0x00000001                            */
#define DDRT_MCA_CTL_MC_2LM_REG 0x07014AB4
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Bits to control MCA signalling for DDRT Errors
 */
typedef union {
  struct {
    UINT32 dis_erid_par : 1;
    /* dis_erid_par - Bits[0:0], RWS, default = 1'b1 
       Disable MCA Bank logging for ERID correctable Parity error. Viral signalling is 
       Error signalling is disabled by default 
     */
    UINT32 dis_erid_uc : 1;
    /* dis_erid_uc - Bits[1:1], RWS, default = 1'b0 
       Disable MCA Bank logging for ERID fatal parity error
     */
    UINT32 dis_pending_intr_error : 1;
    /* dis_pending_intr_error - Bits[2:2], RWS, default = 1'b0 
       Disable MCA Bank logging for DDRT Interrupt
     */
    UINT32 dis_erid_fifo_error : 1;
    /* dis_erid_fifo_error - Bits[3:3], RWS, default = 1'b0 
       Disable MCA Bank logging for ERID FIFO over flow or underflow error
     */
    UINT32 dis_wr_fnv_credit_error : 1;
    /* dis_wr_fnv_credit_error - Bits[4:4], RWS, default = 1'b0 
       Disable MCA Bank logging when number of write credit returned exceeds number of 
       outstanding writes 
     */
    UINT32 dis_rd_fnv_credit_error : 1;
    /* dis_rd_fnv_credit_error - Bits[5:5], RWS, default = 1'b0 
       Disable MCA Bank logging when number of read credit returned exceeds number of 
       outstanding reads 
     */
    UINT32 dis_ddrt_scheduler_mismatch : 1;
    /* dis_ddrt_scheduler_mismatch - Bits[6:6], RWS, default = 1'b0 
       Disable MCA Bank logging when there is a mismatch is command scheduled at DDRT 
       and global scheduler 
     */
    UINT32 dis_fnv_error : 1;
    /* dis_fnv_error - Bits[7:7], RWS, default = 1'b0 
       Disable MCA Bank logging when FNV pin error is asserted
     */
    UINT32 dis_fnv_thermal_error : 1;
    /* dis_fnv_thermal_error - Bits[8:8], RWS, default = 1'b0 
       Disable MCA Bank logging when FNV thermal error is asserted
     */
    UINT32 rsvd_9 : 15;
    /* rsvd_9 - Bits[23:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 en_smi_fnv_err : 1;
    /* en_smi_fnv_err - Bits[24:24], RWS, default = 1'b0 
       Enable SMI for correctable ERID parity error
     */
    UINT32 en_smi_erid_par : 1;
    /* en_smi_erid_par - Bits[25:25], RWS, default = 1'b0 
       Enable LINK for correctable ERID parity error
     */
    UINT32 rsvd_26 : 6;
    /* rsvd_26 - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_MCA_CTL_MC_2LM_STRUCT;


/* DDRT_CLK_GATING2_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251AB8)                                                  */
/*       SKX (0x40251AB8)                                                     */
/* Register default value:              0x00000040                            */
#define DDRT_CLK_GATING2_MC_2LM_REG 0x07014AB8
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * Misc defeatures for DDRt
 */
typedef union {
  struct {
    UINT32 ddrtq_busy_timer : 12;
    /* ddrtq_busy_timer - Bits[11:0], RW_LB, default = 12'h40 
       
               Timer to keep clocks running for DDRT ReadQ or DDRT WriteQ
               after the queues drain all transactions. 
               
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_CLK_GATING2_MC_2LM_STRUCT;


/* DDRT_ECC_MODE_MC_2LM_REG supported on:                                     */
/*       SKX_A0 (0x40251ABC)                                                  */
/*       SKX (0x40251ABC)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_ECC_MODE_MC_2LM_REG 0x07014ABC
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * ECC mode encoding for DDRT write commands
 */
typedef union {
  struct {
    UINT32 em : 5;
    /* em - Bits[4:0], RW_LB, default = 5'b0 
       
               ECC mode bits to issue to FNV as part of the write command. It is the 
       responsibility of software to program these bits 
               correctly. Note that an encoding of 11111 is the default Flow A ECC 
       mode.  
               
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_ECC_MODE_MC_2LM_STRUCT;


/* CPGC_ERID_LFSR_CTL_MC_2LM_REG supported on:                                */
/*       SKX_A0 (0x40251D00)                                                  */
/*       SKX (0x40251D00)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERID_LFSR_CTL_MC_2LM_REG 0x07014D00
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 reset0 : 1;
    /* reset0 - Bits[0:0], RW_LB, default = 1'b0  */
    UINT32 load0 : 1;
    /* load0 - Bits[1:1], RW_LB, default = 1'b0  */
    UINT32 mode0 : 1;
    /* mode0 - Bits[2:2], RW_LB, default = 1'b0 
       1: Selects Pattern Buffer mode; 0: Selects LFSR mode 
     */
    UINT32 stop_on_err0 : 1;
    /* stop_on_err0 - Bits[3:3], RW_LB, default = 1'b0  */
    UINT32 mask_err0 : 1;
    /* mask_err0 - Bits[4:4], RW_LB, default = 1'b0  */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reset1 : 1;
    /* reset1 - Bits[8:8], RW_LB, default = 1'b0  */
    UINT32 load1 : 1;
    /* load1 - Bits[9:9], RW_LB, default = 1'b0  */
    UINT32 mode1 : 1;
    /* mode1 - Bits[10:10], RW_LB, default = 1'b0 
       1: Selects Pattern Buffer mode; 0: Selects LFSR mode 
     */
    UINT32 stop_on_err1 : 1;
    /* stop_on_err1 - Bits[11:11], RW_LB, default = 1'b0  */
    UINT32 mask_err1 : 1;
    /* mask_err1 - Bits[12:12], RW_LB, default = 1'b0  */
    UINT32 rsvd_13 : 3;
    /* rsvd_13 - Bits[15:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 save_lfsr_seedrate : 6;
    /* save_lfsr_seedrate - Bits[21:16], RW_LB, default = 6'b000000 
       
                    save_lfsr_seedrate * ^(reload_lfsr_seedrate-1) defines the
                    periodic cacheline interval that the current LFSR value
                    REUT#_CH#_PAT_CL_MUX#_PB_STATUS is saved into the
                    REUT#_CH#_PAT_CL_MUX#_PB register. 
                      When the save_lfsr_seedrate and reload_lfsr_seedrate is reached 
       in the same cycle only the saving of the LFSR seed takes place. 
                      Examples:
                      0 = Disable - the REUT#_CH#_PAT_CL_MUX#_PB_STATUS is never saved 
       into the REUT#_CH#_PAT_CL_MUX#_PB register. 
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
    UINT32 reload_lfsr_seedrate : 3;
    /* reload_lfsr_seedrate - Bits[26:24], RW_LB, default = 3'b000 
       
                   2^(reload_lfsr_seedrate-1) defines the periodic cacheline
                   interval that the current LFSR Seed is reloaded from
                   REUT#_CH#_PAT_CL_MUX#_PB. 
                   When the save_lfsr_seedrate and reload_lfsr_seedrate is reached in 
       the same cycle only the saving of the LFSR seed takes place. 
                      Examples:
                      0 = Disable - the REUT#_CH#_PAT_CL_MUX#_PB_STATUS is never 
       reloaded into the REUT#_CH#_PAT_CL_MUX#_PB register. 
                      1 = The LFSR seed is reloaded every cacheline operation from 
       REUT#_CH#_PAT_CL_MUX#_PB. 
                      2 = The LFSR seed is reloaded every other cacheline operation 
       from REUT#_CH#_PAT_CL_MUX#_PB 
                      3 = The LFSR seed is reloaded every fourth cacheline operation 
       from REUT#_CH#_PAT_CL_MUX#_PB 
     */
    UINT32 rsvd_27 : 4;
    /* rsvd_27 - Bits[30:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 enable_erid_lfsr_training : 1;
    /* enable_erid_lfsr_training - Bits[31:31], RW_LB, default = 1'b0 
       
               Enable ERID LFSR training 
               
     */
  } Bits;
  UINT32 Data;
} CPGC_ERID_LFSR_CTL_MC_2LM_STRUCT;


/* CPGC_ERID_LFSR_INIT0_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x40251D04)                                                  */
/*       SKX (0x40251D04)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERID_LFSR_INIT0_MC_2LM_REG 0x07014D04
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 seed : 24;
    /* seed - Bits[23:0], RW_LB, default = 24'h0  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERID_LFSR_INIT0_MC_2LM_STRUCT;


/* CPGC_ERID_LFSR_INIT1_MC_2LM_REG supported on:                              */
/*       SKX_A0 (0x40251D08)                                                  */
/*       SKX (0x40251D08)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERID_LFSR_INIT1_MC_2LM_REG 0x07014D08
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 seed : 24;
    /* seed - Bits[23:0], RW_LB, default = 24'h0  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERID_LFSR_INIT1_MC_2LM_STRUCT;


/* CPGC_ERID_LFSR_WR_STATUS0_MC_2LM_REG supported on:                         */
/*       SKX_A0 (0x40251D0C)                                                  */
/*       SKX (0x40251D0C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERID_LFSR_WR_STATUS0_MC_2LM_REG 0x07014D0C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 current_code : 24;
    /* current_code - Bits[23:0], RW_LB, default = 24'b0  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERID_LFSR_WR_STATUS0_MC_2LM_STRUCT;


/* CPGC_ERID_LFSR_WR_STATUS1_MC_2LM_REG supported on:                         */
/*       SKX_A0 (0x40251D10)                                                  */
/*       SKX (0x40251D10)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERID_LFSR_WR_STATUS1_MC_2LM_REG 0x07014D10
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 current_code : 24;
    /* current_code - Bits[23:0], RW_LB, default = 24'b0  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERID_LFSR_WR_STATUS1_MC_2LM_STRUCT;


/* CPGC_ERID_LFSR_RD_STATUS0_MC_2LM_REG supported on:                         */
/*       SKX_A0 (0x40251D14)                                                  */
/*       SKX (0x40251D14)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERID_LFSR_RD_STATUS0_MC_2LM_REG 0x07014D14
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 current_code : 24;
    /* current_code - Bits[23:0], RW_LB, default = 24'b0  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERID_LFSR_RD_STATUS0_MC_2LM_STRUCT;


/* CPGC_ERID_LFSR_RD_STATUS1_MC_2LM_REG supported on:                         */
/*       SKX_A0 (0x40251D18)                                                  */
/*       SKX (0x40251D18)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERID_LFSR_RD_STATUS1_MC_2LM_REG 0x07014D18
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 current_code : 24;
    /* current_code - Bits[23:0], RW_LB, default = 24'b0  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_ERID_LFSR_RD_STATUS1_MC_2LM_STRUCT;


/* CPGC_ERID_LFSR_ERR_MC_2LM_REG supported on:                                */
/*       SKX_A0 (0x40251D1C)                                                  */
/*       SKX (0x40251D1C)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_ERID_LFSR_ERR_MC_2LM_REG 0x07014D1C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT ERID training Error status
 */
typedef union {
  struct {
    UINT32 mask : 8;
    /* mask - Bits[7:0], RW_LB, default = 8'h0 
       8 bit mask for Error
     */
    UINT32 status : 8;
    /* status - Bits[15:8], RW_LB, default = 8'h0 
       8 bit status per ERID bit
     */
    UINT32 count : 16;
    /* count - Bits[31:16], RW_LB, default = 16'h0 
       count of errors
     */
  } Bits;
  UINT32 Data;
} CPGC_ERID_LFSR_ERR_MC_2LM_STRUCT;


/* CPGC_DDRT_MISC_CTL_MC_2LM_REG supported on:                                */
/*       SKX_A0 (0x40251D20)                                                  */
/*       SKX (0x40251D20)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_DDRT_MISC_CTL_MC_2LM_REG 0x07014D20
/* Struct format extracted from XML file SKX_A0\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 multi_credit_on : 1;
    /* multi_credit_on - Bits[0:0], RW_LB, default = 1'b0 
       
               Set this to 1 only when the DDRT schedulers need to handle multple 
               outstanding transactions 
               
     */
    UINT32 disable_max_credit_check : 1;
    /* disable_max_credit_check - Bits[1:1], RW_LB, default = 1'b0 
       
               If this bit is set during multi-credit mode, then the scheduler ignores 
       the value programmed cpgc_max_credit field. 
               No restriction to block grants until a certain threshold of reads are 
       scheduled is done. Reads and Grants are scheduled as they become available.    
               
     */
    UINT32 enable_erid_return : 1;
    /* enable_erid_return - Bits[2:2], RW_LB, default = 1'b0 
       
               If this bit is set during multi-credit mode, then the scheduler ignores 
       ERID returned from DDRIO and use internally generated ERID value to deallocate 
       from RPQ. 
               Should be set to 1 when ERID is expected to incorrect during CPGC 
       training. 
               
     */
    UINT32 rsvd_3 : 7;
    /* rsvd_3 - Bits[9:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cpgc_max_credit : 8;
    /* cpgc_max_credit - Bits[17:10], RW_LB, default = 8'b0 
       
               Maximum number of read credits allowed to CPGC sequencer. This limit is 
       used to block reads if the scheduler  
               accumulates reads with pending GNTs            
               
     */
    UINT32 rsvd_18 : 14;
    /* rsvd_18 - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_DDRT_MISC_CTL_MC_2LM_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT RW Major Mode Thresholds
 */
typedef union {
  struct {
    UINT32 multi_credit_on : 1;
    /* multi_credit_on - Bits[0:0], RW_LB, default = 1'b0 
       
               Set this to 1 only when the DDRT schedulers need to handle multple 
               outstanding transactions 
               
     */
    UINT32 disable_max_credit_check : 1;
    /* disable_max_credit_check - Bits[1:1], RW_LB, default = 1'b0 
       
               If this bit is set during multi-credit mode, then the scheduler ignores 
       the value programmed cpgc_max_credit field. 
               No restriction to block grants until a certain threshold of reads are 
       scheduled is done. Reads and Grants are scheduled as they become available.    
               
     */
    UINT32 enable_erid_return : 1;
    /* enable_erid_return - Bits[2:2], RW_LB, default = 1'b0 
       
               If this bit is set during multi-credit mode, then the scheduler ignores 
       ERID returned from DDRIO and use internally generated ERID value to deallocate 
       from RPQ. 
               Should be set to 1 when ERID is expected to incorrect during CPGC 
       training. 
               
     */
    UINT32 rsvd_3 : 7;
    /* rsvd_3 - Bits[9:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 cpgc_max_credit : 8;
    /* cpgc_max_credit - Bits[17:10], RW_LB, default = 8'b0 
       
               Maximum number of read credits allowed to CPGC sequencer. This limit is 
       used to block reads if the scheduler  
               accumulates reads with pending GNTs            
               
     */
    UINT32 cpgc_rpq_force_drain : 1;
    /* cpgc_rpq_force_drain - Bits[18:18], RW_LB, default = 1'b0 
       
               CR bit to clear valids of all RPQ entries. Meant to be used by BIOS 
       during training in CPGC mode. Set this CR bit to unconditionally clear all 
       valids.  
        Transactions will be dropped by clearing the valids without actually draining 
       them. 
        Level sensitive control, i.e., valids are cleared as long as this CR bit stays 
       high so BIOS must clear the bit before pending queues can be reused 
               
     */
    UINT32 cpgc_wpq_force_drain : 1;
    /* cpgc_wpq_force_drain - Bits[19:19], RW_LB, default = 1'b0 
       
               CR bit to clear valids of all WPQ entries. Meant to be used by BIOS 
       during training in CPGC mode. Set this CR bit to unconditionally clear all 
       valids.  
        Transactions will be dropped by clearing the valids without actually draining 
       them. 
        Level sensitive control, i.e., valids are cleared as long as this CR bit stays 
       high so BIOS must clear the bit before pending queues can be reused 
               
     */
    UINT32 cpgc_dis_rpq_fifo_ptr_rst : 1;
    /* cpgc_dis_rpq_fifo_ptr_rst - Bits[20:20], RW_LB, default = 1'b0 
       
               CR defeature bit, to disable cr_cpgc_rpq_force_drain_DnnnH from 
       resetting RPQ's fifo ptrs.  
               
     */
    UINT32 rsvd_21 : 11;
    /* rsvd_21 - Bits[31:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CPGC_DDRT_MISC_CTL_MC_2LM_STRUCT;



/* DDRT_ERROR_MC_2LM_REG supported on:                                        */
/*       SKX_A0 (0x40251D24)                                                  */
/*       SKX (0x40251D24)                                                     */
/* Register default value:              0x00000744                            */
#define DDRT_ERROR_MC_2LM_REG 0x07014D24
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT ERID training Error status
 */
typedef union {
  struct {
    UINT32 current_ddrt_err0 : 1;
    /* current_ddrt_err0 - Bits[0:0], RW_LBV, default = 1'b0 
       
               This bit reflects the current status of DDR-T Error from Slot 0  as seen 
       in the memory controller.  
               If the error pin is asserted, this field shows value 1. The logging in 
       this field is not affected by ignore_ddrt_err0 bit.  
               If the error condition is still in effect, then hardware will re-assert 
       the bit after software clears it.  
               
     */
    UINT32 observed_ddrt_err0_assert : 1;
    /* observed_ddrt_err0_assert - Bits[1:1], RW_LBV, default = 1'b0 
       
               Hardware will set this bit to a 1 when it observes that DDR-T Error from 
       Slot 0  has been active. 
               The logging in this field is not affected by ignore_ddrt_err0 bit.   
               Once set, hardware will keep the bit set even if DDR-T Error from Slot 0 
        is not active until the bit is cleared by software.  
               It is the responsibility of software to clear this bit. 
               If the error condition is still in effect, then hardware will re-assert 
       the bit after software clears it.  
               
     */
    UINT32 ignore_ddrt_err0 : 1;
    /* ignore_ddrt_err0 - Bits[2:2], RW_LB, default = 1'b1 
       
               Set this bit to 1 to mask the DDR-T Error from Slot 0 in the iMC.
               DDRT_ERROR.current_ddrt_err0 will still report the current state of 
       error signal, but the DDR-T retry flow will not be  
               initiated even if the error indication asserts. 
               
     */
    UINT32 rsvd_3 : 1;
    /* rsvd_3 - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 current_ddrt_err1 : 1;
    /* current_ddrt_err1 - Bits[4:4], RW_LBV, default = 1'b0 
       
               This bit reflects the current status of DDR-T Error from Slot 1  as seen 
       in the memory controller.  
               If the error pin is asserted, this field shows value 1. The logging in 
       this field is not affected by ignore_ddrt_err1 bit.  
               If the error condition is still in effect, then hardware will re-assert 
       the bit after software clears it.  
               
     */
    UINT32 observed_ddrt_err1_assert : 1;
    /* observed_ddrt_err1_assert - Bits[5:5], RW_LBV, default = 1'b0 
       
               Hardware will set this bit to a 1 when it observes that DDR-T Error from 
       Slot 1  has been active. 
               The logging in this field is not affected by ignore_ddrt_err1 bit.   
               Once set, hardware will keep the bit set even if DDR-T Error from Slot 1 
        is not active until the bit is cleared by software.  
               It is the responsibility of software to clear this bit. 
               If the error condition is still in effect, then hardware will re-assert 
       the bit after software clears it.  
               
     */
    UINT32 ignore_ddrt_err1 : 1;
    /* ignore_ddrt_err1 - Bits[6:6], RW_LB, default = 1'b1 
       
               Set this bit to 1 to mask the DDR-T Error from Slot 1 in the iMC.
               DDRT_ERROR.current_ddrt_err1 will still report the current state of 
       error signal, but the DDR-T retry flow will not be  
               initiated even if the error indication asserts. 
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ignore_ddrt_ecc_error : 1;
    /* ignore_ddrt_ecc_error - Bits[8:8], RW_LB, default = 1'b1 
       
               Set this bit to 1 to force the DDRT Retry FSM to ignore ECC errors.
               The correct behavior is to start  retry FSM on DDRT read data ECC 
       errors.  
               This bit should be set during CPGC mode where Read data uses training 
       patterns that do not have good ECC.  
               
     */
    UINT32 ignore_erid_parity_error : 1;
    /* ignore_erid_parity_error - Bits[9:9], RW_LB, default = 1'b1 
       
               Set this bit to 1 to force the DDRT Retry FSM to ignore ERID parity 
       errors. 
               The correct behavior is to start retry FSM on ERID parity errors. 
               This bit should be set during CPGC mode where ERID parity error is not 
       reliable. 
               
     */
    UINT32 ignore_ddr4_error : 1;
    /* ignore_ddr4_error - Bits[10:10], RW_LB, default = 1'b1 
       
               Set this bit to 1 to force the DDRT Retry FSM to ignore the DDR4 command 
       parity errors.  
               The correct behavior is to set this bit to 1 only when there is no DDRT 
       populated in the channel.  
               If the channel has a DDRT DIMM then the bit should be programmed to 0. 
               
     */
    UINT32 spare : 5;
    /* spare - Bits[15:11], RW_LB, default = 5'b0 
       
               Bit 11: Disable DIMM Info packets 
               Remaining bits are Spare
               
     */
    UINT32 rsvd_16 : 16;
    /* rsvd_16 - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_ERROR_MC_2LM_STRUCT;


/* CPGC_DDRT_PATWDB_RDWR_PNTR_MC_2LM_REG supported on:                        */
/*       SKX_A0 (0x40251D28)                                                  */
/*       SKX (0x40251D28)                                                     */
/* Register default value:              0x00000000                            */
#define CPGC_DDRT_PATWDB_RDWR_PNTR_MC_2LM_REG 0x07014D28
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * DDRT ERID training Error status
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
} CPGC_DDRT_PATWDB_RDWR_PNTR_MC_2LM_STRUCT;










































/* DDRT_ERR_LOG_1ST_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251D80)                                                  */
/*       SKX (0x40251D80)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_ERR_LOG_1ST_MC_2LM_REG 0x07014D80
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  
 *      This register shows the 1st error loggged by the DDRT logic.
 *      CSR bit en_ddrt_dfd_clk must be enabled to use this register  
 *     
 */
typedef union {
  struct {
    UINT32 error : 15;
    /* error - Bits[14:0], RW_V, default = 15'b0 
       
               Kick off the Retry FSM 
               
     */
    UINT32 info : 16;
    /* info - Bits[30:15], RW_V, default = 16'b0 
       
               Additional information about error 
               
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RW_V, default = 1'b0 
       
               Register contains valid information 
               
     */
  } Bits;
  UINT32 Data;
} DDRT_ERR_LOG_1ST_MC_2LM_STRUCT;


/* DDRT_ERR_LOG_NEXT_MC_2LM_REG supported on:                                 */
/*       SKX_A0 (0x40251D84)                                                  */
/*       SKX (0x40251D84)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_ERR_LOG_NEXT_MC_2LM_REG 0x07014D84
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  
 *      This register accumulates all the errors logged in the DDRT logic
 *      after the 1st error. 
 *      CSR bit en_ddrt_dfd_clk must be enabled to use this register  
 *     
 */
typedef union {
  struct {
    UINT32 error : 15;
    /* error - Bits[14:0], RW_V, default = 15'b0 
       
               Errors seen 
               
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[30:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 valid : 1;
    /* valid - Bits[31:31], RW_V, default = 1'b0 
       
               Register contains valid information 
               
     */
  } Bits;
  UINT32 Data;
} DDRT_ERR_LOG_NEXT_MC_2LM_STRUCT;


/* DDRT_ERR_LOG_CTL_MC_2LM_REG supported on:                                  */
/*       SKX_A0 (0x40251D88)                                                  */
/*       SKX (0x40251D88)                                                     */
/* Register default value:              0x00000000                            */
#define DDRT_ERR_LOG_CTL_MC_2LM_REG 0x07014D88
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 *  
 *      This register accumulates all the errors logged in the DDRT logic
 *      after the 1st error. 
 *      CSR bit en_ddrt_dfd_clk must be enabled to use this register  
 *     
 */
typedef union {
  struct {
    UINT32 mask : 15;
    /* mask - Bits[14:0], RW_LB, default = 15'b0 
       
               Mask Errors from being logged into the DDDRT Error log registers 
               
     */
    UINT32 rsvd : 17;
    /* rsvd - Bits[31:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_ERR_LOG_CTL_MC_2LM_STRUCT;


/* DDRT_WR_STARVE_CTL_MC_2LM_REG supported on:                                */
/*       SKX_A0 (0x40251D8C)                                                  */
/*       SKX (0x40251D8C)                                                     */
/* Register default value:              0x04000612                            */
#define DDRT_WR_STARVE_CTL_MC_2LM_REG 0x07014D8C
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * 
 *       This register controls a mechanism in the DDRT scheduler to
 *       issue full writes when the number of writes is below the
 *       threshold to enter WMM, and causes the scheduler to not issue
 *       writes for a long interval. When this mechanism triggers entry
 *       into WMM, then the scheduler will enter WMM and issue a
 *       programmable number of writes before exiing WMM. 
 *       
 */
typedef union {
  struct {
    UINT32 timer : 6;
    /* timer - Bits[5:0], RW_LB, default = 6'h12 
       
                Timer to control write starvation window. 
                The threshold field is incremented every (2^timer) DCLK ticks
               
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 threshold : 16;
    /* threshold - Bits[23:8], RW_LB, default = 16'h6 
       
                  If not write is issued in a (threshold x 2^timer) window
                  then DDRT WMM is forced. The scheduler will remain in DDRT
                  WMM until num_writes number of writes are issued. 
               
     */
    UINT32 num_writes : 4;
    /* num_writes - Bits[27:24], RW_LB, default = 4'h4 
       
                  Number of writes to issue in WWM before exiting when WMM is
                  entered as a result of this mechanism. 
               
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DDRT_WR_STARVE_CTL_MC_2LM_STRUCT;


/* DDRT_STARVE_CTL_MC_2LM_REG supported on:                                   */
/*       SKX_A0 (0x40251D90)                                                  */
/*       SKX (0x40251D90)                                                     */
/* Register default value:              0x0020020A                            */
#define DDRT_STARVE_CTL_MC_2LM_REG 0x07014D90
/* Struct format extracted from XML file SKX\2.10.1.CFG.xml.
 * 
 *       This register controls a mechanism in the DDRT scheduler to
 *       issue GNT when there is no BL egress credit to get write credit
 *       back from FNV, or to issue GNT for transgress underfill read 
 *       when there is no general purpose BL egress credit. This register
 *       controls the period after above condition occurs that such a GNT 
 *       can be issued.
 *       
 */
typedef union {
  struct {
    UINT32 timer : 6;
    /* timer - Bits[5:0], RW_LB, default = 6'hA 
       
                Timer to control write starvation window. 
                The threshold field is incremented every (2^timer) DCLK ticks
               
     */
    UINT32 rsvd_6 : 1;
    UINT32 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 wrcrdt_threshold : 12;
    /* wrcrdt_threshold - Bits[19:8], RW_LB, default = 12'h2 
       
                  If the condition of no BL egress credit and no write credit lasts for
                  (threshold x 2^timer) period, then a GNT is allowed to be issued. The
                  read data coming back will be dropped.
               
     */
    UINT32 tgr_threshold : 12;
    /* tgr_threshold - Bits[31:20], RW_LB, default = 12'h2 
       
                  If the condition of no general purpose BL egress credit and 
       transgress 
                  underfill read available lasts for (threshold x 2^timer) period, then 
        
                  a GNT is allowed to be issued. The read data coming back will be 
       dropped 
                  if it is not for any transgress underfill read.
               
     */
  } Bits;
  UINT32 Data;
} DDRT_STARVE_CTL_MC_2LM_STRUCT;


#endif /* MC_2LM_h */
