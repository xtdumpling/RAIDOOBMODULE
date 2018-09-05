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

#ifndef PCU_FUN3_h
#define PCU_FUN3_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* PCU_FUN3_DEV 30                                                            */
/* PCU_FUN3_FUN 3                                                             */

/* VID_PCU_FUN3_REG supported on:                                             */
/*       SKX_A0 (0x201F3000)                                                  */
/*       SKX (0x201F3000)                                                     */
/* Register default value:              0x8086                                */
#define VID_PCU_FUN3_REG 0x04032000
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} VID_PCU_FUN3_STRUCT;


/* DID_PCU_FUN3_REG supported on:                                             */
/*       SKX_A0 (0x201F3002)                                                  */
/*       SKX (0x201F3002)                                                     */
/* Register default value:              0x2083                                */
#define DID_PCU_FUN3_REG 0x04032002
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2083 
        
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
} DID_PCU_FUN3_STRUCT;


/* PCICMD_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x201F3004)                                                  */
/*       SKX (0x201F3004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_PCU_FUN3_REG 0x04032004
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} PCICMD_PCU_FUN3_STRUCT;


/* PCISTS_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x201F3006)                                                  */
/*       SKX (0x201F3006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_PCU_FUN3_REG 0x04032006
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} PCISTS_PCU_FUN3_STRUCT;


/* RID_PCU_FUN3_REG supported on:                                             */
/*       SKX_A0 (0x101F3008)                                                  */
/*       SKX (0x101F3008)                                                     */
/* Register default value:              0x00                                  */
#define RID_PCU_FUN3_REG 0x04031008
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} RID_PCU_FUN3_STRUCT;


/* CCR_N0_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x101F3009)                                                  */
/*       SKX (0x101F3009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_PCU_FUN3_REG 0x04031009
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_PCU_FUN3_STRUCT;


/* CCR_N1_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x201F300A)                                                  */
/*       SKX (0x201F300A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_PCU_FUN3_REG 0x0403200A
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} CCR_N1_PCU_FUN3_STRUCT;


/* CLSR_PCU_FUN3_REG supported on:                                            */
/*       SKX_A0 (0x101F300C)                                                  */
/*       SKX (0x101F300C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_PCU_FUN3_REG 0x0403100C
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} CLSR_PCU_FUN3_STRUCT;


/* PLAT_PCU_FUN3_REG supported on:                                            */
/*       SKX_A0 (0x101F300D)                                                  */
/*       SKX (0x101F300D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_PCU_FUN3_REG 0x0403100D
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} PLAT_PCU_FUN3_STRUCT;


/* HDR_PCU_FUN3_REG supported on:                                             */
/*       SKX_A0 (0x101F300E)                                                  */
/*       SKX (0x101F300E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_PCU_FUN3_REG 0x0403100E
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} HDR_PCU_FUN3_STRUCT;


/* BIST_PCU_FUN3_REG supported on:                                            */
/*       SKX_A0 (0x101F300F)                                                  */
/*       SKX (0x101F300F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_PCU_FUN3_REG 0x0403100F
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} BIST_PCU_FUN3_STRUCT;


/* SVID_PCU_FUN3_REG supported on:                                            */
/*       SKX_A0 (0x201F302C)                                                  */
/*       SKX (0x201F302C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_PCU_FUN3_REG 0x0403202C
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * PCI Subsystem Vendor ID Register
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RWS_O, default = 16'h8086 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_PCU_FUN3_STRUCT;


/* SDID_PCU_FUN3_REG supported on:                                            */
/*       SKX_A0 (0x201F302E)                                                  */
/*       SKX (0x201F302E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_PCU_FUN3_REG 0x0403202E
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * PCI Subsystem device ID Register
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RWS_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_PCU_FUN3_STRUCT;


/* CAPPTR_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x101F3034)                                                  */
/*       SKX (0x101F3034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_PCU_FUN3_REG 0x04031034
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} CAPPTR_PCU_FUN3_STRUCT;


/* INTL_PCU_FUN3_REG supported on:                                            */
/*       SKX_A0 (0x101F303C)                                                  */
/*       SKX (0x101F303C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_PCU_FUN3_REG 0x0403103C
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} INTL_PCU_FUN3_STRUCT;


/* INTPIN_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x101F303D)                                                  */
/*       SKX (0x101F303D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_PCU_FUN3_REG 0x0403103D
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} INTPIN_PCU_FUN3_STRUCT;


/* MINGNT_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x101F303E)                                                  */
/*       SKX (0x101F303E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_PCU_FUN3_REG 0x0403103E
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} MINGNT_PCU_FUN3_STRUCT;


/* MAXLAT_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x101F303F)                                                  */
/*       SKX (0x101F303F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_PCU_FUN3_REG 0x0403103F
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
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
} MAXLAT_PCU_FUN3_STRUCT;


/* CONFIG_TDP_CONTROL_PCU_FUN3_REG supported on:                              */
/*       SKX_A0 (0x401F3060)                                                  */
/*       SKX (0x401F3060)                                                     */
/* Register default value:              0x00000000                            */
#define CONFIG_TDP_CONTROL_PCU_FUN3_REG 0x04034060
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * Rd/Wr register to allow platform SW to select TDP point and set lock via all 
 * three interfaces (MSR, MMIO and PECI/PCS) 
 */
typedef union {
  struct {
    UINT32 tdp_level : 2;
    /* tdp_level - Bits[1:0], RWS_L, default = 2'b00 
       Config TDP level selected
       0 = nominal TDP level (default)
       1 = Level from CONFIG_TDP_LEVEL_1
       2 = Level from CONFIG_TDP_LEVEL_2
       3 = reserved
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[30:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 config_tdp_lock : 1;
    /* config_tdp_lock - Bits[31:31], RWS_KL, default = 1'b0 
       Config TDP level select lock
       0 - unlocked.
       1 - locked till next reset.
       
     */
  } Bits;
  UINT32 Data;
} CONFIG_TDP_CONTROL_PCU_FUN3_STRUCT;


/* CONFIG_TDP_NOMINAL_PCU_FUN3_REG supported on:                              */
/*       SKX_A0 (0x401F3064)                                                  */
/*       SKX (0x401F3064)                                                     */
/* Register default value:              0x00000000                            */
#define CONFIG_TDP_NOMINAL_PCU_FUN3_REG 0x04034064
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 tdp_ratio : 8;
    /* tdp_ratio - Bits[7:0], RO_V, default = 8'b00000000 
       Pcode set based on SKU and factors in SSKU/softbin and flex impact.
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CONFIG_TDP_NOMINAL_PCU_FUN3_STRUCT;


/* CONFIG_TDP_LEVEL1_N0_PCU_FUN3_REG supported on:                            */
/*       SKX_A0 (0x401F3070)                                                  */
/*       SKX (0x401F3070)                                                     */
/* Register default value:              0x00000000                            */
#define CONFIG_TDP_LEVEL1_N0_PCU_FUN3_REG 0x04034070
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * Level 1 configurable TDP settings.
 */
typedef union {
  struct {
    UINT32 pkg_tdp : 15;
    /* pkg_tdp - Bits[14:0], RO_V, default = 15'h0000 
       Power for this TDP level. Units defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT]
       Similar to PACKAGE_POWER_SKU[PKG_TDP]
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tdp_ratio : 8;
    /* tdp_ratio - Bits[23:16], RO_V, default = 8'h00 
       TDP ratio for config tdp level 1.
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CONFIG_TDP_LEVEL1_N0_PCU_FUN3_STRUCT;


/* CONFIG_TDP_LEVEL1_N1_PCU_FUN3_REG supported on:                            */
/*       SKX_A0 (0x401F3074)                                                  */
/*       SKX (0x401F3074)                                                     */
/* Register default value:              0x00000000                            */
#define CONFIG_TDP_LEVEL1_N1_PCU_FUN3_REG 0x04034074
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * Level 1 configurable TDP settings.
 */
typedef union {
  struct {
    UINT32 pkg_max_pwr : 15;
    /* pkg_max_pwr - Bits[14:0], RO_V, default = 16'h0000 
       Max pkg power setting allowed for this config TDP level1. Higher values will be 
       clamped down to this value. 
       Units defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT].
       Similar to PACKAGE_POWER_SKU[PKG_MAX_PWR].
     */
    UINT32 pkg_min_pwr : 16;
    /* pkg_min_pwr - Bits[30:15], RO_V, default = 15'h0000 
       Min pkg power setting allowed for this config TDP level. Lower values will be 
       clamped up to this value. 
       Units defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT].
       Similar to PACKAGE_POWER_SKU[PKG_MIN_PWR].
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CONFIG_TDP_LEVEL1_N1_PCU_FUN3_STRUCT;


/* CONFIG_TDP_LEVEL2_N0_PCU_FUN3_REG supported on:                            */
/*       SKX_A0 (0x401F3078)                                                  */
/*       SKX (0x401F3078)                                                     */
/* Register default value:              0x00000000                            */
#define CONFIG_TDP_LEVEL2_N0_PCU_FUN3_REG 0x04034078
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * Level 2 configurable TDP settings. On sku that does not support config tdp, 
 * these to registers will report out 0. 
 *      
 */
typedef union {
  struct {
    UINT32 pkg_tdp : 15;
    /* pkg_tdp - Bits[14:0], RO_V, default = 15'h0000 
       Power for this TDP level. Units defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT]
       Similar to PACKAGE_POWER_SKU[PKG_TDP].
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tdp_ratio : 8;
    /* tdp_ratio - Bits[23:16], RO_V, default = 8'h00 
       TDP ratio for level 2.
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CONFIG_TDP_LEVEL2_N0_PCU_FUN3_STRUCT;


/* CONFIG_TDP_LEVEL2_N1_PCU_FUN3_REG supported on:                            */
/*       SKX_A0 (0x401F307C)                                                  */
/*       SKX (0x401F307C)                                                     */
/* Register default value:              0x00000000                            */
#define CONFIG_TDP_LEVEL2_N1_PCU_FUN3_REG 0x0403407C
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * Level 2 configurable TDP settings. On sku that does not support config tdp, 
 * these to registers will report out 0. 
 *      
 */
typedef union {
  struct {
    UINT32 pkg_max_pwr : 15;
    /* pkg_max_pwr - Bits[14:0], RO_V, default = 15'h0000 
       Max pkg power setting allowed for config TDP level 2. Higher values will be 
       clamped down to this value. 
       Units defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT].
       Similar to PACKAGE_POWER_SKU[PKG_MAX_PWR].
     */
    UINT32 pkg_min_pwr : 16;
    /* pkg_min_pwr - Bits[30:15], RO_V, default = 16'h0000 
       Min pkg power setting allowed for this config TDP level 2. Lower values will be 
       clamped up to this value. 
       Units defined in PACKAGE_POWER_SKU_MSR[PWR_UNIT].
       Similar to PACKAGE_POWER_SKU[PKG_MIN_PWR].
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CONFIG_TDP_LEVEL2_N1_PCU_FUN3_STRUCT;


/* CAP_HDR_PCU_FUN3_REG supported on:                                         */
/*       SKX_A0 (0x401F3080)                                                  */
/*       SKX (0x401F3080)                                                     */
/* Register default value:              0x01200009                            */
#define CAP_HDR_PCU_FUN3_REG 0x04034080
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Header.It enumerates the CAPID registers 
 * available, and points to the next CAP_PTR. 
 */
typedef union {
  struct {
    UINT32 cap_id : 8;
    /* cap_id - Bits[7:0], RO_FW, default = 8'b00001001 
       This field has the value 1001b to identify the CAP_ID assigned by the PCI SIG 
       for vendor dependent capability pointers. 
     */
    UINT32 next_cap_ptr : 8;
    /* next_cap_ptr - Bits[15:8], RO_FW, default = 8'b00000000 
       This field is hardwired to 00h indicating the end of the capabilities linked 
       list. 
     */
    UINT32 capid_length : 8;
    /* capid_length - Bits[23:16], RO_FW, default = 8'b00100000 
       This field indicates the structure length including the header in Bytes.
     */
    UINT32 capid_version : 4;
    /* capid_version - Bits[27:24], RO_FW, default = 4'b0001 
       This field has the value 0001b to identify the first revision of the CAPID 
       register definition. 
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CAP_HDR_PCU_FUN3_STRUCT;


/* CAPID0_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x401F3084)                                                  */
/*       SKX (0x401F3084)                                                     */
/* Register default value:              0x00000000                            */
#define CAPID0_PCU_FUN3_REG 0x04034084
/* Struct format extracted from XML file SKX_A0\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 wayness : 2;
    /* wayness - Bits[1:0], RO_FW, default = 2'b00 
       Max wayness. '00=1S, '01=2S, '10 = 4S, '11 = 8S
     */
    UINT32 scalable : 1;
    /* scalable - Bits[2:2], RO_FW, default = 1'b0 
       If 1, implies S2S or S4S
     */
    UINT32 segment : 3;
    /* segment - Bits[5:3], RO_FW, default = 3'b000 
       111: EX; 101; EP with Storm Lake; 001: EP; 000: HEDT; Others: Reserved.
       	
     */
    UINT32 rsvd_6 : 1;
    /* rsvd_6 - Bits[6:6], RO_FW, default = 1'b0 
       Reserved for future use
               
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], RO_FW, default = 1'b0 
       Reserved for future use
               
     */
    UINT32 prg_tdp_lim_en : 1;
    /* prg_tdp_lim_en - Bits[8:8], RO_FW, default = 1'b0 
       TURBO_POWER_LIMIT MSRs usage enabled 
     */
    UINT32 llc_way_en : 3;
    /* llc_way_en - Bits[11:9], RO_FW, default = 3'b000 
       Enable LLC ways in active Cbo slice
        Value    Cache size
        '000:          0.5 M (4 lower ways)
        '001:          1 M (8 lower ways)
        '010:          1.5 M (12 lower ways)
        '011:          2 M (16 lower ways)
        '100:          2.5M (20 lower ways)
     */
    UINT32 ht_dis : 1;
    /* ht_dis - Bits[12:12], RO_FW, default = 1'b0 
       Multi threading disabled 
     */
    UINT32 rsvd_13 : 1;
    /* rsvd_13 - Bits[13:13], RO_FW, default = 1'b0 
       Reserved for future use
               
     */
    UINT32 rsvd_14 : 1;
    /* rsvd_14 - Bits[14:14], RO_FW, default = 1'b0 
       Reserved for future use 
     */
    UINT32 vt_x3_en : 1;
    /* vt_x3_en - Bits[15:15], RO_FW, default = 1'b0 
       VT-x3 enabled
     */
    UINT32 rsvd_16 : 1;
    UINT32 vmx_dis : 1;
    /* vmx_dis - Bits[17:17], RO_FW, default = 1'b0 
       VMX disabled
     */
    UINT32 smx_dis : 1;
    /* smx_dis - Bits[18:18], RO_FW, default = 1'b0 
       SMX disabled
     */
    UINT32 lt_production : 1;
    /* lt_production - Bits[19:19], RO_FW, default = 1'b0 
       Intel Trusted Execution Technology support
        1. Intel TXT enable == SMX enable
        2. Intel TXT enable == FIT boot enable
        3. Intel TXT production
        Legal combination (assume 0/1 == disable/enable)
        SMX enable / FIT boot enable / Intel TXT Production -- Remark           
                0 / 0 / 0 -- Intel TXT is disabled 
                1 / 1 / 1 -- Intel TXT is enabled 
                other values reserved 
     */
    UINT32 lt_sx_en : 1;
    /* lt_sx_en - Bits[20:20], RO_FW, default = 1'b0 
       Intel TXT and FIT-boot Enable
     */
    UINT32 lt_smm_inhibit : 1;
    /* lt_smm_inhibit - Bits[21:21], RO_FW, default = 1'b0 
       LT for handling of SMI inhibit with opt-out SMM enabled
     */
    UINT32 tsc_deadline_dis : 1;
    /* tsc_deadline_dis - Bits[22:22], RO_FW, default = 1'b0 
       APIC timer last tick relative mode:
        Support for TSC Deadline disabled
     */
    UINT32 aes_dis : 1;
    /* aes_dis - Bits[23:23], RO_FW, default = 1'b0 
       Advanced Encryption Standard (AES) disabled
     */
    UINT32 rsvd_24 : 1;
    /* rsvd_24 - Bits[24:24], RO_FW, default = 1'b0 
       Reserved for future use.
     */
    UINT32 xsaveopt_dis : 1;
    /* xsaveopt_dis - Bits[25:25], RO_FW, default = 1'b0 
       XSAVEOPT disabled.  XSAVEOPT will be disable if set.
     */
    UINT32 gsse256_dis : 1;
    /* gsse256_dis - Bits[26:26], RO_FW, default = 1'b0 
       All GSSE instructions and setting GSSE XFeatureEnabledMask[GSSE] disabled.  All 
       GSSE instructions will be disabled if set. 
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 art_dis : 1;
    /* art_dis - Bits[28:28], RO_FW, default = 1'b0 
       SparDisable support for Always Running APIC Timer.
        ART (Always Running APIC Timer) function in the APIC (enable legacy timer) 
       disabled 
     */
    UINT32 peci_en : 1;
    /* peci_en - Bits[29:29], RO_FW, default = 1'b0 
       PECI to the processor 
     */
    UINT32 dcu_mode : 1;
    /* dcu_mode - Bits[30:30], RO_FW, default = 1'b0 
       DCU mode (as set by MSR (31h) DCU_MODE) enabled
        0:        normal
        1:        16K 1/2 size ECC mode
     */
    UINT32 pclmulq_dis : 1;
    /* pclmulq_dis - Bits[31:31], RO_FW, default = 1'b0 
       PCLMULQ instructions disabled
     */
  } Bits;
  UINT32 Data;
} CAPID0_PCU_FUN3_A0_STRUCT;

/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 wayness : 2;
    /* wayness - Bits[1:0], RO_FW, default = 2'b00 
       Max wayness. '00=1S, '01=2S, '10 = 4S, '11 = 8S
     */
    UINT32 scalable : 1;
    /* scalable - Bits[2:2], RO_FW, default = 1'b0 
       If 1, implies S2S or S4S
     */
    UINT32 segment : 3;
    /* segment - Bits[5:3], RO_FW, default = 3'b000 
       111: Server; 011: FPGA; 001: Fabric; 010: Workstation; 000: HEDT; Others: 
       Reserved. 
       	
     */
    UINT32 basic_ras : 1;
    /* basic_ras - Bits[6:6], RO_FW, default = 1'b0 
       Combined with CAPID5.28 and CAPID0.7 to indicate RAS availability. {CAPID5.28, 
       CAPID0.[7:6]} == 3'b000, HEDT RAS; 3'b001, Standard RAS; 3'b011, Advanced RAS; 
       3'b101, 1SWS RAS; 3'b110, Core RAS; 3'b111, FPGA RAS. 
               
     */
    UINT32 advanced_ras : 1;
    /* advanced_ras - Bits[7:7], RO_FW, default = 1'b0 
       Combined with CAPID5.28 and CAPID0.7 to indicate RAS availability. {CAPID5.28, 
       CAPID0.[7:6]} == 3'b000, HEDT RAS; 3'b001, Standard RAS; 3'b011, Advanced RAS; 
       3'b101, 1SWS RAS; 3'b110, Core RAS; 3'b111, FPGA RAS. 
               
     */
    UINT32 prg_tdp_lim_en : 1;
    /* prg_tdp_lim_en - Bits[8:8], RO_FW, default = 1'b0 
       TURBO_POWER_LIMIT MSRs usage enabled 
     */
    UINT32 llc_way_en : 3;
    /* llc_way_en - Bits[11:9], RO_FW, default = 3'b000 
       Reserved
     */
    UINT32 ht_dis : 1;
    /* ht_dis - Bits[12:12], RO_FW, default = 1'b0 
       Multi threading disabled 
     */
    UINT32 rsvd_13 : 1;
    /* rsvd_13 - Bits[13:13], RO_FW, default = 1'b0 
       Reserved for future use
               
     */
    UINT32 rsvd_14 : 1;
    /* rsvd_14 - Bits[14:14], RO_FW, default = 1'b0 
       Reserved for future use 
     */
    UINT32 vt_x3_en : 1;
    /* vt_x3_en - Bits[15:15], RO_FW, default = 1'b0 
       VT-x3 enabled
     */
    UINT32 rsvd_16 : 1;
    UINT32 vmx_dis : 1;
    /* vmx_dis - Bits[17:17], RO_FW, default = 1'b0 
       VMX disabled
     */
    UINT32 smx_dis : 1;
    /* smx_dis - Bits[18:18], RO_FW, default = 1'b0 
       SMX disabled
     */
    UINT32 lt_production : 1;
    /* lt_production - Bits[19:19], RO_FW, default = 1'b0 
       Intel Trusted Execution Technology support is enabled in the platform.
               
     */
    UINT32 lt_sx_en : 1;
    /* lt_sx_en - Bits[20:20], RO_FW, default = 1'b0 
       Intel TXT and FIT-boot Enable
     */
    UINT32 lt_smm_inhibit : 1;
    /* lt_smm_inhibit - Bits[21:21], RO_FW, default = 1'b0 
       LT for handling of SMI inhibit with opt-out SMM enabled
     */
    UINT32 tsc_deadline_dis : 1;
    /* tsc_deadline_dis - Bits[22:22], RO_FW, default = 1'b0 
       APIC timer last tick relative mode:
        Support for TSC Deadline disabled
     */
    UINT32 aes_dis : 1;
    /* aes_dis - Bits[23:23], RO_FW, default = 1'b0 
       Advanced Encryption Standard (AES) disabled
     */
    UINT32 rsvd_24 : 1;
    /* rsvd_24 - Bits[24:24], RO_FW, default = 1'b0 
       Reserved for future use.
     */
    UINT32 xsaveopt_dis : 1;
    /* xsaveopt_dis - Bits[25:25], RO_FW, default = 1'b0 
        XSAVEOPT will be disable if set.
     */
    UINT32 gsse256_dis : 1;
    /* gsse256_dis - Bits[26:26], RO_FW, default = 1'b0 
       All GSSE instructions will be disabled if set.
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 art_dis : 1;
    /* art_dis - Bits[28:28], RO_FW, default = 1'b0 
       
        ART (Always Running APIC Timer) function in the APIC (enable legacy timer) 
       disabled 
     */
    UINT32 rsvd_29 : 1;
    UINT32 dcu_mode : 1;
    /* dcu_mode - Bits[30:30], RO_FW, default = 1'b0 
       DCU mode (as set by MSR (31h) DCU_MODE) enabled
        0:        normal
        1:        16K 1/2 size ECC mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} CAPID0_PCU_FUN3_STRUCT;



/* CAPID1_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x401F3088)                                                  */
/*       SKX (0x401F3088)                                                     */
/* Register default value:              0x00000000                            */
#define CAPID1_PCU_FUN3_REG 0x04034088
/* Struct format extracted from XML file SKX_A0\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 tph_en : 1;
    /* tph_en - Bits[0:0], RO_FW, default = 1'b0 
       TPH Enabled 
     */
    UINT32 core_ras_en : 1;
    /* core_ras_en - Bits[1:1], RO_FW, default = 1'b0 
       Data Poisoning, MCA recovery enabled
     */
    UINT32 rsvd_2 : 2;
    UINT32 gv3_dis : 1;
    /* gv3_dis - Bits[4:4], RO_FW, default = 1'b0 
       GV3 disabled. Does not allow for the writing of the IA32_PERF_CONTROL register 
       in order to change ratios 
     */
    UINT32 pwrbits_dis : 1;
    /* pwrbits_dis - Bits[5:5], RO_FW, default = 1'b0 
       0b     Power features activated during reset
        1b     Power features (especially clock gating) are not activated
     */
    UINT32 cpu_hot_add_en : 1;
    /* cpu_hot_add_en - Bits[6:6], RO_FW, default = 1'b0 
       Intel TXT - CPU HOT ADD enabled
     */
    UINT32 x2apic_en : 1;
    /* x2apic_en - Bits[7:7], RO_FW, default = 1'b0 
       Enable Extended APIC support.
        When set enables the support of x2APIC (Extended APIC) in the core and uncore.  
        
     */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_9 : 1;
    /* rsvd_9 - Bits[9:9], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 adddc_bank_rank_region_size : 1;
    /* adddc_bank_rank_region_size - Bits[10:10], RO_FW, default = 1'b0 
       0=Bank only, 1=Bank and/or Rank size
     */
    UINT32 max_adddc_regions : 2;
    /* max_adddc_regions - Bits[12:11], RO_FW, default = 2'b00 
       0=ADDDC disable, 1, 2, 4
     */
    UINT32 disable_2lm : 1;
    /* disable_2lm - Bits[13:13], RO_FW, default = 1'b0 
       To Disable 2LM
     */
    UINT32 disable_ddrt : 1;
    /* disable_ddrt - Bits[14:14], RO_FW, default = 1'b0 
       Disable the use of DDRT devices, once the fuse is blown to one, iMC will not 
       issue any DDRT commands or responding DDRT bus request. Internally, any request 
       targeting DDRT port is treated as programming error. System may hang. 
     */
    UINT32 disable_cr_pmem : 1;
    /* disable_cr_pmem - Bits[15:15], RO_FW, default = 1'b0 
       To Disable PMEM
     */
    UINT32 disable_cr_pmem_wt : 1;
    /* disable_cr_pmem_wt - Bits[16:16], RO_FW, default = 1'b0 
       To Disable PMEM WT
     */
    UINT32 disable_mc_mirror_mode : 1;
    /* disable_mc_mirror_mode - Bits[17:17], RO_FW, default = 1'b0 
       To Disable Mirror Mode
     */
    UINT32 rsvd_18 : 1;
    /* rsvd_18 - Bits[18:18], RO_FW, default = 1'b0 
       Reserved for future use.  
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], RO_FW, default = 1'b0 
       Reserved for future use.  
     */
    UINT32 rsvd_20 : 1;
    /* rsvd_20 - Bits[20:20], RO_FW, default = 1'b0 
       Reserved for future use.  
     */
    UINT32 rsvd_21 : 1;
    /* rsvd_21 - Bits[21:21], RO_FW, default = 1'b0 
       Reserved for future use.  
     */
    UINT32 rsvd_22 : 1;
    /* rsvd_22 - Bits[22:22], RO_FW, default = 1'b0 
       Reserved for future use.  
     */
    UINT32 rsvd_23 : 3;
    UINT32 dmfc : 4;
    /* dmfc - Bits[29:26], RO_FW, default = 4'b0000 
       This field controls which values may be written to the Memory Frequency Select 
       field 6:4 of the Clocking Configuration registers (MCHBAR Offset C00h). Any 
       attempt to write an unsupported value will be ignored.   
       
       [3:3] - If set, over-clocking is supported and bits [2:0] are ignored. 
       [2:0] - Maximum allowed memory frequency.
                 3b111 - up to DDR-1066 (4 x 266)
                 3b110 - up to DDR-1333 (5 x 266)
                 3b101 - up to DDR-1600 (6 x 266)
                 3b100 - up to DDR-1866 (7 x 266)
                 3b011 - up to DDR-2133 (8 x 266)
                 3b010 - up to DDR-2400 (9 x 266)
                 
     */
    UINT32 dis_mem_lt_support : 1;
    /* dis_mem_lt_support - Bits[30:30], RO_FW, default = 1'b0 
       Intel TXT support disabled
     */
    UINT32 dis_mem_mirror : 1;
    /* dis_mem_mirror - Bits[31:31], RO_FW, default = 1'b0 
       Disable memory channel mirroring mode. In the mirroring mode, the server 
       maintains two identical copies of all data in memory. The contents of branch 0 
       (ontaining channel 0/1) is duplicated in the DIMMs of branch 1 (containing 
       channel 2/3). In the event of an uncorrectable error in one of the copies, the 
       system can retrieve the mirrored copy of the data. The use of memory mirroring 
       means that only half of the installed memory is available to the operating 
       system. 
     */
  } Bits;
  UINT32 Data;
} CAPID1_PCU_FUN3_A0_STRUCT;

/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 tph_en : 1;
    /* tph_en - Bits[0:0], RO_FW, default = 1'b0 
       TLP Processing Hint Enabled 
     */
    UINT32 core_ras_en : 1;
    /* core_ras_en - Bits[1:1], RO_FW, default = 1'b0 
       Data Poisoning, MCA recovery enabled
     */
    UINT32 rsvd_2 : 2;
    UINT32 gv3_dis : 1;
    /* gv3_dis - Bits[4:4], RO_FW, default = 1'b0 
       GV3 disabled. Does not allow for the writing of the IA32_PERF_CONTROL register 
       in order to change ratios 
     */
    UINT32 pwrbits_dis : 1;
    /* pwrbits_dis - Bits[5:5], RO_FW, default = 1'b0 
       0b     Power features activated during reset
        1b     Power features (especially clock gating) are not activated
     */
    UINT32 cpu_hot_add_en : 1;
    /* cpu_hot_add_en - Bits[6:6], RO_FW, default = 1'b0 
       Intel TXT - CPU HOT ADD enabled
     */
    UINT32 x2apic_en : 1;
    /* x2apic_en - Bits[7:7], RO_FW, default = 1'b0 
       Enable Extended APIC support.
        When set enables the support of x2APIC (Extended APIC) in the core and uncore.  
        
     */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_9 : 1;
    /* rsvd_9 - Bits[9:9], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_10 : 3;
    UINT32 disable_2lm : 1;
    /* disable_2lm - Bits[13:13], RO_FW, default = 1'b0 
       Memory mode support with 3D-Xpoint memory is disabled
     */
    UINT32 disable_ddrt : 1;
    /* disable_ddrt - Bits[14:14], RO_FW, default = 1'b0 
       Disable the use of DDRT devices. When set, iMC will not issue any DDRT commands 
       or responding DDRT bus request. Internally, any request targeting DDRT port is 
       treated as programming error. System may hang. 
     */
    UINT32 disable_cr_pmem : 1;
    /* disable_cr_pmem - Bits[15:15], RO_FW, default = 1'b0 
       App Direct support with 3D-Xpoint memory is disabled.
     */
    UINT32 disable_cr_pmem_wt : 1;
    /* disable_cr_pmem_wt - Bits[16:16], RO_FW, default = 1'b0 
       Read caching for App Direct mode support with 3D-Xpoint memory is disabled.
     */
    UINT32 disable_mc_mirror_mode : 1;
    /* disable_mc_mirror_mode - Bits[17:17], RO_FW, default = 1'b0 
       To Disable Mirror Mode
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[22:18], RO_FW, default = 5'b00000 
       Reserved for future use.  
     */
    UINT32 rsvd_23 : 3;
    UINT32 dmfc : 4;
    /* dmfc - Bits[29:26], RO_FW, default = 4'b0000 
       This field controls which values may be written to the Memory Frequency Select 
       field 6:4 of the Clocking Configuration registers (MCHBAR Offset C00h). Any 
       attempt to write an unsupported value will be ignored.   
       		    
               [3:3] - If set, over-clocking is supported and bits [2:0] are ignored.   
          
               [2:0] - Maximum allowed memory frequency.		
       				
       	  3b110 - up to DDR-1600 (6 x 266)	    
       	  3b101 - up to DDR-1867 (7 x 266)	    
       	  3b100 - up to DDR-2133 (8 x 266)	    
       	  3b011 - up to DDR-2400 (9 x 266)	    
       	  3b010 - up to DDR-2667 (10 x 266)   
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} CAPID1_PCU_FUN3_STRUCT;



/* CAPID2_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x401F308C)                                                  */
/*       SKX (0x401F308C)                                                     */
/* Register default value:              0x00000000                            */
#define CAPID2_PCU_FUN3_REG 0x0403408C
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 pcie_dis_x16_port_1 : 1;
    /* pcie_dis_x16_port_1 - Bits[0:0], RO_FW, default = 1'b0 
       PCIe Port 1 x16 mode is disabled. Only x8's and x4's is supported. 
     */
    UINT32 pcie_dis_x16_port_2 : 1;
    /* pcie_dis_x16_port_2 - Bits[1:1], RO_FW, default = 1'b0 
       PCIe Port 2 x16 mode is disabled. Only x8's and x4's is supported. 
     */
    UINT32 pcie_dis_x16_port_3 : 1;
    /* pcie_dis_x16_port_3 - Bits[2:2], RO_FW, default = 1'b0 
       PCIe Port 3 x16 mode is disabled. Only x8's and x4's is supported. 
     */
    UINT32 pcie_disxpdev_port : 12;
    /* pcie_disxpdev_port - Bits[14:3], RO_FW, default = 12'b000000000000 
       Disable PCIe Port 1a, 1b, 1c, 1d, 2a, 2b, 2c, 2d, 3a, 3b, 3c, 3d, indicated by 
       bit [3:14] respectively. 
     */
    UINT32 pcie_disdmi : 1;
    /* pcie_disdmi - Bits[15:15], RO_FW, default = 1'b0 
       Disable PCIe Mode on the DMI Port
     */
    UINT32 pcie_disdma : 1;
    /* pcie_disdma - Bits[16:16], RO_FW, default = 1'b0 
       Disable the CBDMA Engine
     */
    UINT32 pcie_disvmd : 1;
    /* pcie_disvmd - Bits[17:17], RO_FW, default = 1'b0 
       Disable Volume Management Device
     */
    UINT32 pcie_disler_rpioerr : 1;
    /* pcie_disler_rpioerr - Bits[18:18], RO_FW, default = 1'b0 
       Disable Live Error Recovery and RP IO Error
     */
    UINT32 pcie_disntb : 1;
    /* pcie_disntb - Bits[19:19], RO_FW, default = 1'b0 
       Disable NT mode for all Ports
     */
    UINT32 pcie_disecrc : 1;
    /* pcie_disecrc - Bits[20:20], RO_FW, default = 1'b0 
       Disable ECRC Check/Generate
     */
    UINT32 pcie_dissris : 1;
    /* pcie_dissris - Bits[21:21], RO_FW, default = 1'b0 
       Disable SRIS for independent PCIe clocking
     */
    UINT32 pcie_dismulti_cast : 1;
    /* pcie_dismulti_cast - Bits[22:22], RO_FW, default = 1'b0 
       Disable Dual-Cast
     */
    UINT32 kti_link0_dis : 1;
    /* kti_link0_dis - Bits[23:23], RO_FW, default = 1'b0 
       When set Intel UPI link 0 will be disabled.
     */
    UINT32 kti_link1_dis : 1;
    /* kti_link1_dis - Bits[24:24], RO_FW, default = 1'b0 
       When set Intel UPI link 1 will be disabled.
     */
    UINT32 kti_link2_dis : 1;
    /* kti_link2_dis - Bits[25:25], RO_FW, default = 1'b0 
       When set Intel UPI link 2 will be disabled.
     */
    UINT32 rsvd_26 : 1;
    /* rsvd_26 - Bits[26:26], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 kti_allowed_cfclk_ratio_dis : 1;
    /* kti_allowed_cfclk_ratio_dis - Bits[28:28], RO_FW, default = 1'b0 
       Indicates the speed limit on UPI links:
       	1: Max UPI speed is limited 9.6GT/s;
       	0: Max UPI speed is limited 10.4GT/s.
               
     */
    UINT32 rsvd_29 : 1;
    /* rsvd_29 - Bits[29:29], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} CAPID2_PCU_FUN3_STRUCT;


/* CAPID3_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x401F3090)                                                  */
/*       SKX (0x401F3090)                                                     */
/* Register default value:              0x00000000                            */
#define CAPID3_PCU_FUN3_REG 0x04034090
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 chn_disable : 6;
    /* chn_disable - Bits[5:0], RO_FW, default = 6'b000000 
       Channel disabled. When set, the corresponding channel off the IMC's is disabled. 
       Bits 5:0 correspond to MC1, Channel 2/1/0, and MC0, Channel 2/1/0, respectively. 
     */
    UINT32 rsvd_6 : 1;
    /* rsvd_6 - Bits[6:6], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 disable_qr_dimm : 1;
    /* disable_qr_dimm - Bits[9:9], RO_FW, default = 1'b0 
       QR DIMM disabled. When set, CS signals for QR-DIMM in slot 0-1 is disabled. 
       Note: some CS may have multiplexed with address signal to support extended 
       addressing. The CS signal disabling is only applicable to CS not the being 
       multiplexed with address.  
     */
    UINT32 disable_ecc : 1;
    /* disable_ecc - Bits[10:10], RO_FW, default = 1'b0 
       ECC disabled. When set, ECC is disabled. 
     */
    UINT32 disable_dir : 1;
    /* disable_dir - Bits[11:11], RO_FW, default = 1'b0 
       DIR disabled. When set, directory is disabled. 
     */
    UINT32 rsvd_12 : 1;
    /* rsvd_12 - Bits[12:12], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 disable_rdimm : 1;
    /* disable_rdimm - Bits[13:13], RO_FW, default = 1'b0 
       RDIMM disabled. When set, RDIMM support is disabled. 
     */
    UINT32 disable_udimm : 1;
    /* disable_udimm - Bits[14:14], RO_FW, default = 1'b0 
       UDIMM disabled. When set, UDIMM support is disabled. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_16 : 1;
    /* rsvd_16 - Bits[16:16], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 disable_sparing : 1;
    /* disable_sparing - Bits[17:17], RO_FW, default = 1'b0 
       Sparing disabled. When set, rank sparing is disabled. 
     */
    UINT32 disable_patrol_scrub : 1;
    /* disable_patrol_scrub - Bits[18:18], RO_FW, default = 1'b0 
       Patrol scrub disabled. When set, rank patrol scrub is disabled.
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_20 : 1;
    UINT32 disable_asyncsr_adr : 1;
    /* disable_asyncsr_adr - Bits[21:21], RO_FW, default = 1'b0 
       RAID-On-LOAD disabled. When set, memory ignores ADR event.
     */
    UINT32 disable_smbus_wrt : 1;
    /* disable_smbus_wrt - Bits[22:22], RO_FW, default = 1'b0 
       SMBUS write capability disabled. When set, SMBus write is disabled. 
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_24 : 1;
    /* rsvd_24 - Bits[24:24], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_25 : 1;
    /* rsvd_25 - Bits[25:25], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_26 : 1;
    /* rsvd_26 - Bits[26:26], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_28 : 1;
    /* rsvd_28 - Bits[28:28], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_29 : 1;
    /* rsvd_29 - Bits[29:29], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_30 : 1;
    /* rsvd_30 - Bits[30:30], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} CAPID3_PCU_FUN3_STRUCT;


/* CAPID4_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x401F3094)                                                  */
/*       SKX (0x401F3094)                                                     */
/* Register default value:              0x00000000                            */
#define CAPID4_PCU_FUN3_REG 0x04034094
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 error_spoofing_dis : 1;
    /* error_spoofing_dis - Bits[0:0], RO_FW, default = 1'b0 
       Error spoofing disabled
     */
    UINT32 pfat_disable : 1;
    /* pfat_disable - Bits[1:1], RO_FW, default = 1'b0 
       PFAT disabled
     */
    UINT32 fit_boot_dis : 1;
    /* fit_boot_dis - Bits[2:2], RO_FW, default = 1'b0 
       FIT (Firmware Interface Table) boot disabled
     */
    UINT32 vmcs_shadowing_dis : 1;
    /* vmcs_shadowing_dis - Bits[3:3], RO_FW, default = 1'b0 
       VMCS shadowing disabled
     */
    UINT32 rsvd_4 : 1;
    /* rsvd_4 - Bits[4:4], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_5 : 1;
    /* rsvd_5 - Bits[5:5], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 physical_chop : 2;
    /* physical_chop - Bits[7:6], RO_FW, default = 2'b00 
       11:XCC; 10:HCC; 01:MCC; 00:LCC;
     */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 ocvolt_ovrd_dis : 1;
    /* ocvolt_ovrd_dis - Bits[9:9], RO_FW, default = 1'b0 
       Voltage override overclocking disabled
     */
    UINT32 llc_qos_mon_en : 1;
    /* llc_qos_mon_en - Bits[10:10], RO_FW, default = 1'b0 
       Cache Monitoring Technology (CMT) is enabled
     */
    UINT32 llc_qos_en : 1;
    /* llc_qos_en - Bits[11:11], RO_FW, default = 1'b0 
       Cache Allocation Technology (CAT) is enabled
     */
    UINT32 rtm_dis : 1;
    /* rtm_dis - Bits[12:12], RO_FW, default = 1'b0 
       Hardware Lock Elison + (HLE+) disabled
     */
    UINT32 hle_dis : 1;
    /* hle_dis - Bits[13:13], RO_FW, default = 1'b0 
       Hardware Lock Elison (HLE) disabled
     */
    UINT32 fma_dis : 1;
    /* fma_dis - Bits[14:14], RO_FW, default = 1'b0 
       FMA (Floatingpoint Multiple Add) instructions disabled
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_16 : 1;
    /* rsvd_16 - Bits[16:16], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_17 : 1;
    /* rsvd_17 - Bits[17:17], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_18 : 1;
    /* rsvd_18 - Bits[18:18], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 enhanced_mca_dis : 1;
    /* enhanced_mca_dis - Bits[19:19], RO_FW, default = 1'b0 
       Enhanced MCA disabled
     */
    UINT32 smm_code_chk_dis : 1;
    /* smm_code_chk_dis - Bits[20:20], RO_FW, default = 1'b0 
       Code access check disabled
     */
    UINT32 smm_cpu_svrstr_dis : 1;
    /* smm_cpu_svrstr_dis - Bits[21:21], RO_FW, default = 1'b0 
       SMM CPU save/restore disabled (part of enhanced SMM)
     */
    UINT32 targeted_smi_dis : 1;
    /* targeted_smi_dis - Bits[22:22], RO_FW, default = 1'b0 
       Targeted SMI disabled (part of enhanced SMM)
     */
    UINT32 sapm_dll_disable : 1;
    /* sapm_dll_disable - Bits[23:23], RO_FW, default = 1'b0  */
    UINT32 ufs_disable : 1;
    /* ufs_disable - Bits[24:24], RO_FW, default = 1'b0 
       If set, Uncore Frequency Scaling is disabled
     */
    UINT32 pcps_disable : 1;
    /* pcps_disable - Bits[25:25], RO_FW, default = 1'b0 
       Per-core P-state disabled
     */
    UINT32 eet_enable : 1;
    /* eet_enable - Bits[26:26], RO_FW, default = 1'b0 
       Energy efficient turbo enabled
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 oc_enabled : 1;
    /* oc_enabled - Bits[28:28], RO_FW, default = 1'b0 
       Overclocking enabled 
     */
    UINT32 rsvd_29 : 1;
    /* rsvd_29 - Bits[29:29], RO_FW, default = 1'b0 
       Memory Bandwidth Enforcement enable
     */
    UINT32 dram_rapl_disable : 1;
    /* dram_rapl_disable - Bits[30:30], RO_FW, default = 1'b0 
       DRAM RAPL disabled 
     */
    UINT32 dram_power_meter_disable : 1;
    /* dram_power_meter_disable - Bits[31:31], RO_FW, default = 1'b0 
        DRAM Power Meter disabled 
     */
  } Bits;
  UINT32 Data;
} CAPID4_PCU_FUN3_STRUCT;


/* CAPID5_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x401F3098)                                                  */
/*       SKX (0x401F3098)                                                     */
/* Register default value:              0x00000001                            */
#define CAPID5_PCU_FUN3_REG 0x04034098
/* Struct format extracted from XML file SKX_A0\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 second_imc : 1;
    /* second_imc - Bits[0:0], RO_FW, default = 1'b1 
       Indicates when second iMC is enabled
     */
    UINT32 iio_llcconfig_en : 1;
    /* iio_llcconfig_en - Bits[1:1], RO_FW, default = 1'b0 
       IIO to allocate in LLC enabled.
     */
    UINT32 timed_mwait_dis : 1;
    /* timed_mwait_dis - Bits[2:2], RO_FW, default = 1'b0 
       TIMED_MWAIT_DIS
     */
    UINT32 cap_mirror_ddr4_en : 1;
    /* cap_mirror_ddr4_en - Bits[3:3], RO_FW, default = 1'b0 
               When set, DDR4 MC channel mirroring mode capability is available. 
                  0: Channel mirroring mode capability is unavailable; 
                  1: Channel mirroring mode capability is available.
               If channel mirroring capability is unavailable and mirroring is 
       configured  
               then system results will be unpredictable.
     */
    UINT32 cap_mirror_ddrt_en : 1;
    /* cap_mirror_ddrt_en - Bits[4:4], RO_FW, default = 1'b0 
               When set, SXP MC channel mirroring mode capability is available. 
                  0: Channel mirroring mode capability is unavailable; 
                  1: Channel mirroring mode capability is available.
               If channel mirroring capability is unavailable and mirroring is 
       configured  
               then system results will be unpredictable.
               Fuse_TRUE.
     */
    UINT32 cap_block_region_en : 1;
    /* cap_block_region_en - Bits[5:5], RO_FW, default = 1'b0 
               When set, Block Region capability is available. 
               If Block Region capability is unavailable and block region is configured 
        
               then system results will be unpredictable.
                Fuse_TRUE.
     */
    UINT32 cap_pmem_en : 1;
    /* cap_pmem_en - Bits[6:6], RO_FW, default = 1'b0 
               When set, Persistent Memory capability is available. 
                  0: Capability is unavailable; 
                  1: Capability is available.
               If capability is unavailable and any feature related to the capability 
       is enabled  
               then system results will be unpredictable.
                Fuse_TRUE.
     */
    UINT32 cap_nm_caching_en : 1;
    /* cap_nm_caching_en - Bits[7:7], RO_FW, default = 1'b0 
        When set, two level memory caching (Near Memory caching for Far Memory) 
       capability  
               is available. 
                  0: Capability is unavailable; 
                  1: Capability is available.
               If capability is unavailable and any feature related to the capability 
       is enabled  
               then system results will be unpredictable.
                 Fuse_TRUE.
     */
    UINT32 cap_nm_caching_pm_en : 1;
    /* cap_nm_caching_pm_en - Bits[8:8], RO_FW, default = 1'b0 
       Near memory caching for persistent memory
     */
    UINT32 rsvd_9 : 1;
    /* rsvd_9 - Bits[9:9], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd_10 : 1;
    /* rsvd_10 - Bits[10:10], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[17:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 spare : 6;
    /* spare - Bits[23:18], RO_FW, default = 6'b000000 
       Reserved for future use
     */
    UINT32 rsvd_24 : 1;
    /* rsvd_24 - Bits[24:24], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 hwp_en : 1;
    /* hwp_en - Bits[25:25], RO_FW, default = 1'b0 
       HWPM Base feature
     */
    UINT32 acc_enable : 1;
    /* acc_enable - Bits[26:26], RO_FW, default = 1'b0 
       Autonomous C-State Control
     */
    UINT32 spare2 : 2;
    /* spare2 - Bits[28:27], RO_FW, default = 2'b00 
       Spare bits reserved for future use
     */
    UINT32 addr_based_mem_mirror : 1;
    /* addr_based_mem_mirror - Bits[29:29], RO_FW, default = 1'b0 
       Address based memory mirroring enabled
     */
    UINT32 hitme_enable : 1;
    /* hitme_enable - Bits[30:30], RO_FW, default = 1'b0 
       Directory Cache enabled.
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
  } Bits;
  UINT32 Data;
} CAPID5_PCU_FUN3_A0_STRUCT;

/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 second_imc : 1;
    /* second_imc - Bits[0:0], RO_FW, default = 1'b1 
       Indicates when second iMC is enabled
     */
    UINT32 iio_llcconfig_en : 1;
    /* iio_llcconfig_en - Bits[1:1], RO_FW, default = 1'b0 
       IIO to allocate in LLC enabled.
     */
    UINT32 timed_mwait_dis : 1;
    /* timed_mwait_dis - Bits[2:2], RO_FW, default = 1'b0 
       Timed Monitor Wait Disable
     */
    UINT32 cap_mirror_ddr4_en : 1;
    /* cap_mirror_ddr4_en - Bits[3:3], RO_FW, default = 1'b0 
               When set, DDR4 MC channel mirroring mode capability is available. 
                  0: Channel mirroring mode capability is unavailable; 
                  1: Channel mirroring mode capability is available.
               If channel mirroring capability is unavailable and mirroring is 
       configured  
               then system results will be unpredictable.
     */
    UINT32 cap_mirror_ddrt_en : 1;
    /* cap_mirror_ddrt_en - Bits[4:4], RO_FW, default = 1'b0 
        When set, NGNVM MC channel mirroring mode capability is available. 
       	   0: Channel mirroring mode capability is unavailable; 
       	   1: Channel mirroring mode capability is available.
       	 If channel mirroring capability is unavailable and mirroring is configured 
       	 then system results will be unpredictable.
               
     */
    UINT32 cap_block_region_en : 1;
    /* cap_block_region_en - Bits[5:5], RO_FW, default = 1'b0 
               When set, Block Region capability is available. 
       	    If Block Region capability is unavailable and block region is configured 
       	    then system results will be unpredictable.
               
     */
    UINT32 cap_pmem_en : 1;
    /* cap_pmem_en - Bits[6:6], RO_FW, default = 1'b0 
               When set, Persistent Memory capability is available. 
       	   0: Capability is unavailable; 
       	   1: Capability is available.
         If capability is unavailable and any feature related to the capability is 
       enabled  
       	 then system results will be unpredictable.
               
     */
    UINT32 cap_nm_caching_en : 1;
    /* cap_nm_caching_en - Bits[7:7], RO_FW, default = 1'b0 
        When set, two level memory caching (Near Memory caching for Far Memory) 
       capability  
       	      is available. 
       	    0: Capability is unavailable; 
       	    1: Capability is available.
          If capability is unavailable and any feature related to the capability is 
       enabled  
       	  then system results will be unpredictable.
               
     */
    UINT32 cap_nm_caching_pm_en : 1;
    /* cap_nm_caching_pm_en - Bits[8:8], RO_FW, default = 1'b0 
       Near memory caching for persistent memory
     */
    UINT32 rsvd_23_9 : 15;
    /* rsvd_23_9 - Bits[23:9], RO_FW, default = 15'b000000000000000 
       Reserved for future use. 
     */
    UINT32 rsvd_24 : 1;
    /* rsvd_24 - Bits[24:24], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
    UINT32 hwp_en : 1;
    /* hwp_en - Bits[25:25], RO_FW, default = 1'b0 
       HWPM Base feature
     */
    UINT32 acc_enable : 1;
    /* acc_enable - Bits[26:26], RO_FW, default = 1'b0 
       Autonomous C-State Control
     */
    UINT32 spare2 : 2;
    /* spare2 - Bits[28:27], RO_FW, default = 2'b00 
       Bit 27 (LSB) is for external SKU: when set, an external SKU. Bit 28 is combined 
       with CAPID0.7 and CAPID0.6 to indicate RAS availability. {CAPID5.28, 
       CAPID0.[7:6]} == 3'b000, HEDT RAS; 3'b001, Standard RAS; 3'b011, Advanced RAS; 
       3'b101, 1SWS RAS; 3'b110, Core RAS; 3'b111, FPGA RAS. 
       	
     */
    UINT32 addr_based_mem_mirror : 1;
    /* addr_based_mem_mirror - Bits[29:29], RO_FW, default = 1'b0 
       Address based memory mirroring enabled
     */
    UINT32 hitme_enable : 1;
    /* hitme_enable - Bits[30:30], RO_FW, default = 1'b0 
       Directory Cache enabled.
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], RO_FW, default = 1'b0 
       Reserved for future use. 
     */
  } Bits;
  UINT32 Data;
} CAPID5_PCU_FUN3_STRUCT;



/* CAPID6_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x401F309C)                                                  */
/*       SKX (0x401F309C)                                                     */
/* Register default value:              0x00000000                            */
#define CAPID6_PCU_FUN3_REG 0x0403409C
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 llc_slice_en : 28;
    /* llc_slice_en - Bits[27:0], RO_FW, default = 28'b0000000000000000000000000000 
       Enabled Cbo slices (Cbo with enabled LLC slice).
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CAPID6_PCU_FUN3_STRUCT;


/* FLEX_RATIO_N0_PCU_FUN3_REG supported on:                                   */
/*       SKX_A0 (0x401F30A0)                                                  */
/*       SKX (0x401F30A0)                                                     */
/* Register default value:              0x00000000                            */
#define FLEX_RATIO_N0_PCU_FUN3_REG 0x040340A0
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * The Flexible Boot register is written by BIOS in order to modify the maximum 
 * non-turbo ratio on the next reset. The DP/MP systems use this FLEX_RATIO in this 
 * register to configure the maximum common boot ratio for all physical processors 
 * in the system. The value in the FLEX_RATIO take effect on the next reset based 
 * on the value of FLEX_EN. 
 */
typedef union {
  struct {
    UINT32 oc_extra_voltage : 8;
    /* oc_extra_voltage - Bits[7:0], RWS, default = 8'b00000000 
       Extra voltage to be used for Over Clocking. The voltage is defined in units of 
       1/256 Volts. 
     */
    UINT32 flex_ratio : 8;
    /* flex_ratio - Bits[15:8], RWS, default = 8'b00000000 
       Desired Flex ratio. When enabled, the value in this field will set the maximum 
       non-turbo ratio available. If this value is greater than the maximum ratio set 
       by hardware, this field is ignored. 
     */
    UINT32 enable : 1;
    /* enable - Bits[16:16], RWS, default = 1'b0 
       Flex Ratio Enabled. When set to 1, indicates that the value programmed in 
       FLEX_RATIO field will be used to override the maximum non-turbo ratio on next 
       reboot. When this bit is cleared, all write to FLEX_RATIO field will be ignored. 
       However there will be no GP-fault# generated. 
     */
    UINT32 oc_bins : 3;
    /* oc_bins - Bits[19:17], RO_FW, default = 3'b000 
       This field indicates the maximum number of bins by which the part can be 
       overclocked 
     */
    UINT32 oc_lock : 1;
    /* oc_lock - Bits[20:20], RW1S, default = 1'b0 
       Once set to 1, the overclocking bitfields become locked from further writes are 
       ignored. A reset is required to clear the lock. 
       Note: System BIOS must set this bit to lock down overclocking limits.
     */
    UINT32 rsvd : 11;
    /* rsvd - Bits[31:21], n/a, default = n/a 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FLEX_RATIO_N0_PCU_FUN3_STRUCT;




/* CAPID7_PCU_FUN3_REG supported on:                                          */
/*       SKX_A0 (0x401F30A8)                                                  */
/*       SKX (0x401F30A8)                                                     */
/* Register default value:              0x00000000                            */
#define CAPID7_PCU_FUN3_REG 0x040340A8
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This register is a Capability Register used to expose feature support to BIOS 
 * for SKU differentiation. 
 */
typedef union {
  struct {
    UINT32 llc_ia_core_en : 28;
    /* llc_ia_core_en - Bits[27:0], RO_FW, default = 28'b0000000000000000000000000000 
       LLC_IA_CORE_EN
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CAPID7_PCU_FUN3_STRUCT;


/* PCU_MC_CTL_PCU_FUN3_REG supported on:                                      */
/*       SKX_A0 (0x401F30AC)                                                  */
/*       SKX (0x401F30AC)                                                     */
/* Register default value:              0x00000021                            */
#define PCU_MC_CTL_PCU_FUN3_REG 0x040340AC
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * This CSR is used to read a programmable counter value to interpret the incoming 
 * caterr pin as an IErr or UMCErr.  
 */
typedef union {
  struct {
    UINT32 caterrdetectcntr : 8;
    /* caterrdetectcntr - Bits[7:0], RW, default = 8'b00100001 
       If a socket counts the Caterr input pulse duration to be less than this value, 
       it will be classified as an UMCErr. Otherwise, if the duration is greater than 
       this threshold value, it will be classified as an Ierr. 
       Default Value = 0x33
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PCU_MC_CTL_PCU_FUN3_STRUCT;


/* SMT_CONTROL_PCU_FUN3_REG supported on:                                     */
/*       SKX_A0 (0x401F30B0)                                                  */
/*       SKX (0x401F30B0)                                                     */
/* Register default value:              0x00000000                            */
#define SMT_CONTROL_PCU_FUN3_REG 0x040340B0
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 8;
    /* rsvd_0 - Bits[7:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 thread_mask : 2;
    /* thread_mask - Bits[9:8], ROS_V, default = 2'b00 
       Thread Mask indicates which threads are enabled in the core.  The LSB is the 
       enable bit for Thread 0, whereas the MSB is the enable bit for Thread 1. 
       This field is determined by FW based on CSR_DESIRED_CORES[SMT_DISABLE]. 
     */
    UINT32 rsvd_10 : 14;
    /* rsvd_10 - Bits[23:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 fused_smt_capability : 1;
    /* fused_smt_capability - Bits[24:24], ROS_V, default = 1'b0 
       Enabled threads in the package.
       0b  1 thread
       1b  2 threads
       
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMT_CONTROL_PCU_FUN3_STRUCT;


/* RESOLVED_CORES_PCU_FUN3_REG supported on:                                  */
/*       SKX_A0 (0x401F30B4)                                                  */
/*       SKX (0x401F30B4)                                                     */
/* Register default value:              0x00000000                            */
#define RESOLVED_CORES_PCU_FUN3_REG 0x040340B4
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 core_mask : 28;
    /* core_mask - Bits[27:0], ROS_V, default = 28'b0000000000000000000000000000 
       The resolved IA core mask contains the functional (enabled in SKU) and 
       non-defeatured IA cores. 
       The mask is indexed by logical ID.  It is normally contiguous, unless BIOS 
       defeature is activated on a particular core. 
       BSP and APIC IDs will be set by the processor based on this value.
       This field is determined by FW based on CSR_DESIRED_CORES[CORE_OFF_MASK]. 
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} RESOLVED_CORES_PCU_FUN3_STRUCT;


/* FUSED_CORES_PCU_FUN3_REG supported on:                                     */
/*       SKX_A0 (0x401F30B8)                                                  */
/*       SKX (0x401F30B8)                                                     */
/* Register default value:              0x00000000                            */
#define FUSED_CORES_PCU_FUN3_REG 0x040340B8
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 fused_core_mask : 28;
    /* fused_core_mask - Bits[27:0], ROS_V, default = 28'b0000000000000000000000000000 
       Vector of SKU enabled IA cores in the package.
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} FUSED_CORES_PCU_FUN3_STRUCT;




























/* PCU_LT_CTRL_PCU_FUN3_REG supported on:                                     */
/*       SKX_A0 (0x401F30F0)                                                  */
/*       SKX (0x401F30F0)                                                     */
/* Register default value:              0x00000000                            */
#define PCU_LT_CTRL_PCU_FUN3_REG 0x040340F0
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 * PCU Intel TXT Control Attributes
 */
typedef union {
  struct {
    UINT32 bios_spare : 31;
    /* bios_spare - Bits[30:0], RW, default = 31'b0000000000000000000000000000000 
       Reserved for BIOS
     */
    UINT32 pit : 1;
    /* pit - Bits[31:31], RW_O, default = 1'b0 
       PECI is Trusted bit. Allows BIOS to specify whether PECI can be trusted. 
       
       Pcode uses this bit to allow write access to Intel TXT protected registers 
       initiated from PECI.  
       Pcode ignores this bit when in SSP boot mode. 
       Default value of 0 implies PECI is not Trusted.
       
       1 = PECI is Trusted
     */
  } Bits;
  UINT32 Data;
} PCU_LT_CTRL_PCU_FUN3_STRUCT;


/* PCU_BIOS_SPARE2_PCU_FUN3_REG supported on:                                 */
/*       SKX_A0 (0x401F30F4)                                                  */
/*       SKX (0x401F30F4)                                                     */
/* Register default value:              0x00000000                            */
#define PCU_BIOS_SPARE2_PCU_FUN3_REG 0x040340F4
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 bios_spare : 32;
    /* bios_spare - Bits[31:0], RW, default = 32'b00000000000000000000000000000000 
       
       [0:0] - Set by BIOS to request legacy APIC id assignment
       [11:01] - Reserved
       [13:12] - 2 bit encoding of connected DIMM type:
                 2'b00 - UDIMM
                 2'b01 - RDIMM
                 2'b10 - LRDIMM
                 2'b11 - Reserved
       [15:14] - 2 bit encoding of the Vddrq voltage level
                 2'b00 - 1.25 V
                 2'b01 - 1.35 V
                 2'b10 - 1.50 V
                 2'b11 - Reserved
       [16:16] - Set by Pcode to indicate that surprise reset was detected
       [17:17] - Reserved
       [18:18] - Used to select between Legacy ADR behavior and the new ADR flow; if 
       clear, use legacy behavior(default). 
       [19:19] - Set by BIOS to enable PECI to control Energy Performance BIAS out of 
       band 
       [20:20] - Set by BIOS to indicate that Wolfriver is present
       [21:21] - Set by pcode to indicate ADR_DONE for enhanced ADR flow
       [30:22] - Unused
       [31:31] - If set, enable GV block for memory sparing
               
     */
  } Bits;
  UINT32 Data;
} PCU_BIOS_SPARE2_PCU_FUN3_STRUCT;


/* PWR_LIMIT_MISC_INFO_PCU_FUN3_REG supported on:                             */
/*       SKX_A0 (0x401F30F8)                                                  */
/*       SKX (0x401F30F8)                                                     */
/* Register default value:              0x00000000                            */
#define PWR_LIMIT_MISC_INFO_PCU_FUN3_REG 0x040340F8
/* Struct format extracted from XML file SKX\1.30.3.CFG.xml.
 */
typedef union {
  struct {
    UINT32 pn_power_of_sku : 15;
    /* pn_power_of_sku - Bits[14:0], RO_FW, default = 15'b000000000000000 
       Skt power at Pn
               
     */
    UINT32 pbm_minimal_tau_allowed : 7;
    /* pbm_minimal_tau_allowed - Bits[21:15], RO_FW, default = 7'b0000000 
       Minimal Time window 
     */
    UINT32 rsvd : 10;
    /* rsvd - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PWR_LIMIT_MISC_INFO_PCU_FUN3_STRUCT;


#endif /* PCU_FUN3_h */
