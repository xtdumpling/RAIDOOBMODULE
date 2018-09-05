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

#ifndef CHABC_PMA_h
#define CHABC_PMA_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* CHABC_PMA_DEV 29                                                           */
/* CHABC_PMA_FUN 3                                                            */

/* VID_CHABC_PMA_REG supported on:                                            */
/*       SKX_A0 (0x201EB000)                                                  */
/*       SKX (0x201EB000)                                                     */
/* Register default value:              0x8086                                */
#define VID_CHABC_PMA_REG 0x03032000
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} VID_CHABC_PMA_STRUCT;


/* DID_CHABC_PMA_REG supported on:                                            */
/*       SKX_A0 (0x201EB002)                                                  */
/*       SKX (0x201EB002)                                                     */
/* Register default value:              0x2057                                */
#define DID_CHABC_PMA_REG 0x03032002
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2057 
        
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
} DID_CHABC_PMA_STRUCT;


/* PCICMD_CHABC_PMA_REG supported on:                                         */
/*       SKX_A0 (0x201EB004)                                                  */
/*       SKX (0x201EB004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_CHABC_PMA_REG 0x03032004
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} PCICMD_CHABC_PMA_STRUCT;


/* PCISTS_CHABC_PMA_REG supported on:                                         */
/*       SKX_A0 (0x201EB006)                                                  */
/*       SKX (0x201EB006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_CHABC_PMA_REG 0x03032006
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} PCISTS_CHABC_PMA_STRUCT;


/* RID_CHABC_PMA_REG supported on:                                            */
/*       SKX_A0 (0x101EB008)                                                  */
/*       SKX (0x101EB008)                                                     */
/* Register default value:              0x00                                  */
#define RID_CHABC_PMA_REG 0x03031008
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} RID_CHABC_PMA_STRUCT;


/* CCR_N0_CHABC_PMA_REG supported on:                                         */
/*       SKX_A0 (0x101EB009)                                                  */
/*       SKX (0x101EB009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_CHABC_PMA_REG 0x03031009
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_CHABC_PMA_STRUCT;


/* CCR_N1_CHABC_PMA_REG supported on:                                         */
/*       SKX_A0 (0x201EB00A)                                                  */
/*       SKX (0x201EB00A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_CHABC_PMA_REG 0x0303200A
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} CCR_N1_CHABC_PMA_STRUCT;


/* CLSR_CHABC_PMA_REG supported on:                                           */
/*       SKX_A0 (0x101EB00C)                                                  */
/*       SKX (0x101EB00C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_CHABC_PMA_REG 0x0303100C
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} CLSR_CHABC_PMA_STRUCT;


/* PLAT_CHABC_PMA_REG supported on:                                           */
/*       SKX_A0 (0x101EB00D)                                                  */
/*       SKX (0x101EB00D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_CHABC_PMA_REG 0x0303100D
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} PLAT_CHABC_PMA_STRUCT;


/* HDR_CHABC_PMA_REG supported on:                                            */
/*       SKX_A0 (0x101EB00E)                                                  */
/*       SKX (0x101EB00E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_CHABC_PMA_REG 0x0303100E
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} HDR_CHABC_PMA_STRUCT;


/* BIST_CHABC_PMA_REG supported on:                                           */
/*       SKX_A0 (0x101EB00F)                                                  */
/*       SKX (0x101EB00F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_CHABC_PMA_REG 0x0303100F
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} BIST_CHABC_PMA_STRUCT;


/* SVID_CHABC_PMA_REG supported on:                                           */
/*       SKX_A0 (0x201EB02C)                                                  */
/*       SKX (0x201EB02C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_CHABC_PMA_REG 0x0303202C
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * PCI Subsystem Vendor ID Register
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_CHABC_PMA_STRUCT;


/* SDID_CHABC_PMA_REG supported on:                                           */
/*       SKX_A0 (0x201EB02E)                                                  */
/*       SKX (0x201EB02E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_CHABC_PMA_REG 0x0303202E
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * PCI Subsystem device ID Register
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_CHABC_PMA_STRUCT;


/* CAPPTR_CHABC_PMA_REG supported on:                                         */
/*       SKX_A0 (0x101EB034)                                                  */
/*       SKX (0x101EB034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_CHABC_PMA_REG 0x03031034
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} CAPPTR_CHABC_PMA_STRUCT;


/* INTL_CHABC_PMA_REG supported on:                                           */
/*       SKX_A0 (0x101EB03C)                                                  */
/*       SKX (0x101EB03C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_CHABC_PMA_REG 0x0303103C
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} INTL_CHABC_PMA_STRUCT;


/* INTPIN_CHABC_PMA_REG supported on:                                         */
/*       SKX_A0 (0x101EB03D)                                                  */
/*       SKX (0x101EB03D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_CHABC_PMA_REG 0x0303103D
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} INTPIN_CHABC_PMA_STRUCT;


/* MINGNT_CHABC_PMA_REG supported on:                                         */
/*       SKX_A0 (0x101EB03E)                                                  */
/*       SKX (0x101EB03E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_CHABC_PMA_REG 0x0303103E
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} MINGNT_CHABC_PMA_STRUCT;


/* MAXLAT_CHABC_PMA_REG supported on:                                         */
/*       SKX_A0 (0x101EB03F)                                                  */
/*       SKX (0x101EB03F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_CHABC_PMA_REG 0x0303103F
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
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
} MAXLAT_CHABC_PMA_STRUCT;


/* SNC_CONFIG_CHABC_PMA_REG supported on:                                     */
/*       SKX_A0 (0x401EB42C)                                                  */
/*       SKX (0x401EB42C)                                                     */
/* Register default value:              0x00000000                            */
#define SNC_CONFIG_CHABC_PMA_REG 0x0303442C
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * Sub Numa Clustering (SNC) Configuration
 */
typedef union {
  struct {
    UINT32 full_snc_en : 1;
    /* full_snc_en - Bits[0:0], RWS_LB, default = 1'b0 
       full snc enable
     */
    UINT32 snc_ind_en : 1;
    /* snc_ind_en - Bits[1:1], RWS_LB, default = 1'b0 
       SNC IND enable
     */
    UINT32 num_snc_clu : 2;
    /* num_snc_clu - Bits[3:2], RWS_LB, default = 2'b00 
       Num of SNC Clusters
     */
    UINT32 lowest_cbo_id_clus1 : 5;
    /* lowest_cbo_id_clus1 - Bits[8:4], RWS_LB, default = 5'b00000 
       Lowest cbo id or base of snc cluster 1
     */
    UINT32 lowest_cbo_id_clus2 : 5;
    /* lowest_cbo_id_clus2 - Bits[13:9], RWS_LB, default = 5'b00000 
       Lowest cbo id or base of snc cluster 2
     */
    UINT32 lowest_cbo_id_clus3 : 5;
    /* lowest_cbo_id_clus3 - Bits[18:14], RWS_LB, default = 5'b00000 
       Lowest cbo id or base of snc cluster 3
     */
    UINT32 rsvd : 13;
    /* rsvd - Bits[31:19], RWS_LB, default = 13'h0000 
       RSVD
     */
  } Bits;
  UINT32 Data;
} SNC_CONFIG_CHABC_PMA_STRUCT;


/* SNC_BASE_1_CHABC_PMA_REG supported on:                                     */
/*       SKX_A0 (0x401EB430)                                                  */
/*       SKX (0x401EB430)                                                     */
/* Register default value:              0x00000000                            */
#define SNC_BASE_1_CHABC_PMA_REG 0x03034430
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * SNC base 1
 */
typedef union {
  struct {
    UINT32 starting_addr_0 : 16;
    /* starting_addr_0 - Bits[15:0], RWS_LB, default = 16'h0000 
       starting address for cluster 0
     */
    UINT32 address_mask_45_40 : 6;
    /* address_mask_45_40 - Bits[21:16], RWS_LB, default = 6'b000000 
       Controls bits [45:40] of the 2LM hash address mask
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[31:22], RWS_LB, default = 10'b0000000000 
       RSVD
     */
  } Bits;
  UINT32 Data;
} SNC_BASE_1_CHABC_PMA_STRUCT;


/* SNC_BASE_2_CHABC_PMA_REG supported on:                                     */
/*       SKX_A0 (0x401EB434)                                                  */
/*       SKX (0x401EB434)                                                     */
/* Register default value:              0x00000000                            */
#define SNC_BASE_2_CHABC_PMA_REG 0x03034434
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * SNC base 2
 */
typedef union {
  struct {
    UINT32 starting_addr_1 : 16;
    /* starting_addr_1 - Bits[15:0], RWS_LB, default = 16'h0000 
       starting address for cluster 1
     */
    UINT32 rsvd1 : 4;
    /* rsvd1 - Bits[19:16], RWS_LB, default = 4'b0000 
       RSVD
     */
    UINT32 address_mask_39_31 : 9;
    /* address_mask_39_31 - Bits[28:20], RWS_LB, default = 9'b000000000 
       Controls bits [39:31] of the 2LM hash address mask
     */
    UINT32 rsvd2 : 3;
    /* rsvd2 - Bits[31:29], RWS_LB, default = 3'b000 
       RSVD
     */
  } Bits;
  UINT32 Data;
} SNC_BASE_2_CHABC_PMA_STRUCT;


/* SNC_BASE_3_CHABC_PMA_REG supported on:                                     */
/*       SKX_A0 (0x401EB438)                                                  */
/*       SKX (0x401EB438)                                                     */
/* Register default value:              0x00000000                            */
#define SNC_BASE_3_CHABC_PMA_REG 0x03034438
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * SNC base 3
 */
typedef union {
  struct {
    UINT32 starting_addr_2 : 16;
    /* starting_addr_2 - Bits[15:0], RWS_LB, default = 16'h0000 
       starting address for cluster 2
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], RWS_LB, default = 16'h0000 
       RSVD
     */
  } Bits;
  UINT32 Data;
} SNC_BASE_3_CHABC_PMA_STRUCT;


/* SNC_BASE_4_CHABC_PMA_REG supported on:                                     */
/*       SKX_A0 (0x401EB43C)                                                  */
/*       SKX (0x401EB43C)                                                     */
/* Register default value:              0x00000000                            */
#define SNC_BASE_4_CHABC_PMA_REG 0x0303443C
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * SNC base 4
 */
typedef union {
  struct {
    UINT32 starting_addr_3 : 16;
    /* starting_addr_3 - Bits[15:0], RWS_LB, default = 16'h0000 
       starting address for cluster 3
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], RWS, default = 16'h0000 
       RSVD
     */
  } Bits;
  UINT32 Data;
} SNC_BASE_4_CHABC_PMA_STRUCT;


/* SNC_BASE_5_CHABC_PMA_REG supported on:                                     */
/*       SKX_A0 (0x401EB440)                                                  */
/*       SKX (0x401EB440)                                                     */
/* Register default value:              0x00000000                            */
#define SNC_BASE_5_CHABC_PMA_REG 0x03034440
/* Struct format extracted from XML file SKX\1.29.3.CFG.xml.
 * SNC base 5
 */
typedef union {
  struct {
    UINT32 starting_addr_4 : 16;
    /* starting_addr_4 - Bits[15:0], RWS_LB, default = 16'h0000 
       starting address for cluster 4
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], RWS_LB, default = 16'h0000 
       RSVD
     */
  } Bits;
  UINT32 Data;
} SNC_BASE_5_CHABC_PMA_STRUCT;


#endif /* CHABC_PMA_h */
