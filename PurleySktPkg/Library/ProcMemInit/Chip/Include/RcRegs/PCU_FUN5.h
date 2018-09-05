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

#ifndef PCU_FUN5_h
#define PCU_FUN5_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* PCU_FUN5_DEV 30                                                            */
/* PCU_FUN5_FUN 5                                                             */

/* VID_PCU_FUN5_REG supported on:                                             */
/*       SKX_A0 (0x201F5000)                                                  */
/*       SKX (0x201F5000)                                                     */
/* Register default value:              0x8086                                */
#define VID_PCU_FUN5_REG 0x04052000
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} VID_PCU_FUN5_STRUCT;


/* DID_PCU_FUN5_REG supported on:                                             */
/*       SKX_A0 (0x201F5002)                                                  */
/*       SKX (0x201F5002)                                                     */
/* Register default value:              0x2085                                */
#define DID_PCU_FUN5_REG 0x04052002
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2085 
        
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
} DID_PCU_FUN5_STRUCT;


/* PCICMD_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x201F5004)                                                  */
/*       SKX (0x201F5004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_PCU_FUN5_REG 0x04052004
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} PCICMD_PCU_FUN5_STRUCT;


/* PCISTS_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x201F5006)                                                  */
/*       SKX (0x201F5006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_PCU_FUN5_REG 0x04052006
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} PCISTS_PCU_FUN5_STRUCT;


/* RID_PCU_FUN5_REG supported on:                                             */
/*       SKX_A0 (0x101F5008)                                                  */
/*       SKX (0x101F5008)                                                     */
/* Register default value:              0x00                                  */
#define RID_PCU_FUN5_REG 0x04051008
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} RID_PCU_FUN5_STRUCT;


/* CCR_N0_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x101F5009)                                                  */
/*       SKX (0x101F5009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_PCU_FUN5_REG 0x04051009
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_PCU_FUN5_STRUCT;


/* CCR_N1_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x201F500A)                                                  */
/*       SKX (0x201F500A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_PCU_FUN5_REG 0x0405200A
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} CCR_N1_PCU_FUN5_STRUCT;


/* CLSR_PCU_FUN5_REG supported on:                                            */
/*       SKX_A0 (0x101F500C)                                                  */
/*       SKX (0x101F500C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_PCU_FUN5_REG 0x0405100C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} CLSR_PCU_FUN5_STRUCT;


/* PLAT_PCU_FUN5_REG supported on:                                            */
/*       SKX_A0 (0x101F500D)                                                  */
/*       SKX (0x101F500D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_PCU_FUN5_REG 0x0405100D
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} PLAT_PCU_FUN5_STRUCT;


/* HDR_PCU_FUN5_REG supported on:                                             */
/*       SKX_A0 (0x101F500E)                                                  */
/*       SKX (0x101F500E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_PCU_FUN5_REG 0x0405100E
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} HDR_PCU_FUN5_STRUCT;


/* BIST_PCU_FUN5_REG supported on:                                            */
/*       SKX_A0 (0x101F500F)                                                  */
/*       SKX (0x101F500F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_PCU_FUN5_REG 0x0405100F
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} BIST_PCU_FUN5_STRUCT;


/* SVID_PCU_FUN5_REG supported on:                                            */
/*       SKX_A0 (0x201F502C)                                                  */
/*       SKX (0x201F502C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_PCU_FUN5_REG 0x0405202C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} SVID_PCU_FUN5_STRUCT;


/* SDID_PCU_FUN5_REG supported on:                                            */
/*       SKX_A0 (0x201F502E)                                                  */
/*       SKX (0x201F502E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_PCU_FUN5_REG 0x0405202E
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} SDID_PCU_FUN5_STRUCT;


/* CAPPTR_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x101F5034)                                                  */
/*       SKX (0x101F5034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_PCU_FUN5_REG 0x04051034
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} CAPPTR_PCU_FUN5_STRUCT;


/* INTL_PCU_FUN5_REG supported on:                                            */
/*       SKX_A0 (0x101F503C)                                                  */
/*       SKX (0x101F503C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_PCU_FUN5_REG 0x0405103C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} INTL_PCU_FUN5_STRUCT;


/* INTPIN_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x101F503D)                                                  */
/*       SKX (0x101F503D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_PCU_FUN5_REG 0x0405103D
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} INTPIN_PCU_FUN5_STRUCT;


/* MINGNT_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x101F503E)                                                  */
/*       SKX (0x101F503E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_PCU_FUN5_REG 0x0405103E
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} MINGNT_PCU_FUN5_STRUCT;


/* MAXLAT_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x101F503F)                                                  */
/*       SKX (0x101F503F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_PCU_FUN5_REG 0x0405103F
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
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
} MAXLAT_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_0_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F5040)                                                  */
/*       SKX (0x401F5040)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_0_PCU_FUN5_REG 0x04054040
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_0_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_1_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F5044)                                                  */
/*       SKX (0x401F5044)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_1_PCU_FUN5_REG 0x04054044
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_1_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_2_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F5048)                                                  */
/*       SKX (0x401F5048)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_2_PCU_FUN5_REG 0x04054048
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_2_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_3_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F504C)                                                  */
/*       SKX (0x401F504C)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_3_PCU_FUN5_REG 0x0405404C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_3_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_4_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F5050)                                                  */
/*       SKX (0x401F5050)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_4_PCU_FUN5_REG 0x04054050
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_4_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_5_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F5054)                                                  */
/*       SKX (0x401F5054)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_5_PCU_FUN5_REG 0x04054054
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_5_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_6_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F5058)                                                  */
/*       SKX (0x401F5058)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_6_PCU_FUN5_REG 0x04054058
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_6_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_7_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F505C)                                                  */
/*       SKX (0x401F505C)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_7_PCU_FUN5_REG 0x0405405C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_7_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_8_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F5060)                                                  */
/*       SKX (0x401F5060)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_8_PCU_FUN5_REG 0x04054060
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_8_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_9_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F5064)                                                  */
/*       SKX (0x401F5064)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_9_PCU_FUN5_REG 0x04054064
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_9_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_10_PCU_FUN5_REG supported on:                    */
/*       SKX_A0 (0x401F5068)                                                  */
/*       SKX (0x401F5068)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_10_PCU_FUN5_REG 0x04054068
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_10_PCU_FUN5_STRUCT;


/* DRAM_ENERGY_STATUS_CH_CFG_11_PCU_FUN5_REG supported on:                    */
/*       SKX_A0 (0x401F506C)                                                  */
/*       SKX (0x401F506C)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_CH_CFG_11_PCU_FUN5_REG 0x0405406C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * DRAM energy consumed by all the DIMMS in Channel0.  The counter will wrap around 
 * and continue counting when it reaches its limit. 
 *      ENERGY_UNIT for DRAM domain is 15.3uJ.
 *      The data is updated by PCODE and is Read Only for all SW.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_CH_CFG_11_PCU_FUN5_STRUCT;


/* SMB_PERIOD_CFG_0_PCU_FUN5_REG supported on:                                */
/*       SKX_A0 (0x401F5070)                                                  */
/*       SKX (0x401F5070)                                                     */
/* Register default value:              0x000003E8                            */
#define SMB_PERIOD_CFG_0_PCU_FUN5_REG 0x04054070
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains configuration for setting up the speed of the SMBUS. 
 * Duration for T_LOW and T_HIGH are configured through this register. Note that 
 * instance 0 is shared between SPD SMBus 0 and SPD SMBus 1 and instance 2 controls 
 * MCP SMBus.  
 */
typedef union {
  struct {
    UINT32 smb_clk_prd : 16;
    /* smb_clk_prd - Bits[15:0], RWS_LB, default = 16'b0000001111101000 
       This field specifies SMBUS Clock period  in number of FXCLK. It is recommend to 
       program an even value in this field since the hardware is simply doing a right 
       shift for the divided by 2 operation. The value of this field must be less than 
       or equal to 0x2710 (i.e. 10KHz or faster). Note the 100KHz SMB_CLK_PRD default 
       value is calculated based on 100 MHz FXCLK.  Time high and time low of SMBUS 
       clock is defined as:  TIME_HIGH = SMB_CLK_PRD-SMB_CLK_OFFSET,  TIME_LOW = 
       SMB_CLK_PRD+SMB_CLK_OFFSET 
     */
    UINT32 smb_clk_offset : 8;
    /* smb_clk_offset - Bits[23:16], RWS_LB, default = 8'b00000000 
       Signed value in units of FXCLK to adjust the duty cycle of SMBUS clock. When 
       value =0, duty cycle = 50%. Time high and time low of SMBUS clock is defined as: 
       TIME_HIGH = SMB_CLK_PRD/2-SMB_CLK_OFFSET, TIME_LOW = 
       SMB_CLK_PRD/2+SMB_CLK_OFFSET. Must be within -(SMB_CLK_PRD/4-1) to 
       (SMB_CLK_PRD/4-1).  
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_PERIOD_CFG_0_PCU_FUN5_STRUCT;


/* SMB_PERIOD_CFG_1_PCU_FUN5_REG supported on:                                */
/*       SKX_A0 (0x401F5074)                                                  */
/*       SKX (0x401F5074)                                                     */
/* Register default value:              0x000003E8                            */
#define SMB_PERIOD_CFG_1_PCU_FUN5_REG 0x04054074
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains configuration for setting up the speed of the SMBUS. 
 * Duration for T_LOW and T_HIGH are configured through this register. Note that 
 * instance 0 is shared between SPD SMBus 0 and SPD SMBus 1 and instance 2 controls 
 * MCP SMBus.  
 */
typedef union {
  struct {
    UINT32 smb_clk_prd : 16;
    /* smb_clk_prd - Bits[15:0], RWS_LB, default = 16'b0000001111101000 
       This field specifies SMBUS Clock period  in number of FXCLK. It is recommend to 
       program an even value in this field since the hardware is simply doing a right 
       shift for the divided by 2 operation. The value of this field must be less than 
       or equal to 0x2710 (i.e. 10KHz or faster). Note the 100KHz SMB_CLK_PRD default 
       value is calculated based on 100 MHz FXCLK.  Time high and time low of SMBUS 
       clock is defined as:  TIME_HIGH = SMB_CLK_PRD-SMB_CLK_OFFSET,  TIME_LOW = 
       SMB_CLK_PRD+SMB_CLK_OFFSET 
     */
    UINT32 smb_clk_offset : 8;
    /* smb_clk_offset - Bits[23:16], RWS_LB, default = 8'b00000000 
       Signed value in units of FXCLK to adjust the duty cycle of SMBUS clock. When 
       value =0, duty cycle = 50%. Time high and time low of SMBUS clock is defined as: 
       TIME_HIGH = SMB_CLK_PRD/2-SMB_CLK_OFFSET, TIME_LOW = 
       SMB_CLK_PRD/2+SMB_CLK_OFFSET. Must be within -(SMB_CLK_PRD/4-1) to 
       (SMB_CLK_PRD/4-1).  
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_PERIOD_CFG_1_PCU_FUN5_STRUCT;


/* SMB_PERIOD_CNTR_CFG_0_PCU_FUN5_REG supported on:                           */
/*       SKX_A0 (0x401F5078)                                                  */
/*       SKX (0x401F5078)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_PERIOD_CNTR_CFG_0_PCU_FUN5_REG 0x04054078
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * Period counter definition for the SMBUS ports
 */
typedef union {
  struct {
    UINT32 smb_clk_prd_cntr : 16;
    /* smb_clk_prd_cntr - Bits[15:0], RO_V, default = 16'b0000000000000000 
       SMBus Clock Period Counter. This field is the current SMBus Clock Period Counter 
       Value. Advances by 1 every FXCLK. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_PERIOD_CNTR_CFG_0_PCU_FUN5_STRUCT;


/* SMB_PERIOD_CNTR_CFG_1_PCU_FUN5_REG supported on:                           */
/*       SKX_A0 (0x401F507C)                                                  */
/*       SKX (0x401F507C)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_PERIOD_CNTR_CFG_1_PCU_FUN5_REG 0x0405407C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * Period counter definition for the SMBUS ports
 */
typedef union {
  struct {
    UINT32 smb_clk_prd_cntr : 16;
    /* smb_clk_prd_cntr - Bits[15:0], RO_V, default = 16'b0000000000000000 
       SMBus Clock Period Counter. This field is the current SMBus Clock Period Counter 
       Value. Advances by 1 every FXCLK. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_PERIOD_CNTR_CFG_1_PCU_FUN5_STRUCT;


/* SMB_PERIOD_CNTR_CFG_2_PCU_FUN5_REG supported on:                           */
/*       SKX_A0 (0x401F5080)                                                  */
/*       SKX (0x401F5080)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_PERIOD_CNTR_CFG_2_PCU_FUN5_REG 0x04054080
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * Period counter definition for the SMBUS ports
 */
typedef union {
  struct {
    UINT32 smb_clk_prd_cntr : 16;
    /* smb_clk_prd_cntr - Bits[15:0], RO_V, default = 16'b0000000000000000 
       SMBus Clock Period Counter. This field is the current SMBus Clock Period Counter 
       Value. Advances by 1 every FXCLK. 
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_PERIOD_CNTR_CFG_2_PCU_FUN5_STRUCT;


/* SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_REG supported on:                        */
/*       SKX_A0 (0x401F5084)                                                  */
/*       SKX (0x401F5084)                                                     */
/* Register default value:              0x0003E800                            */
#define SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_REG 0x04054084
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * the SMBUS TSOD poll rate between consecutive TSOD accesses to the TSOD devices 
 * on the same SMBus segment 
 */
typedef union {
  struct {
    UINT32 smb_tsod_poll_rate : 18;
    /* smb_tsod_poll_rate - Bits[17:0], RWS, default = 18'b111110100000000000 
       TSOD poll rate configuration between consecutive TSOD accesses to the TSOD 
       devices on the same SMBUS port. This field specifies the TSOD poll rate in 
       number of 500ns per CNFG_500_NANOSEC register field definition. The value of 
       this field must be greater than 0x2400 (poll rate is less than ~217 TSODs polled 
       per second). Instance 0 is shared between SMBUS ports 0 and 1, and instance 1 is 
       used for SMBUS port 2 (MCP). 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_STRUCT;


/* SMB_TSOD_POLL_RATE_CFG_1_PCU_FUN5_REG supported on:                        */
/*       SKX_A0 (0x401F5088)                                                  */
/*       SKX (0x401F5088)                                                     */
/* Register default value:              0x0003E800                            */
#define SMB_TSOD_POLL_RATE_CFG_1_PCU_FUN5_REG 0x04054088
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * the SMBUS TSOD poll rate between consecutive TSOD accesses to the TSOD devices 
 * on the same SMBus segment 
 */
typedef union {
  struct {
    UINT32 smb_tsod_poll_rate : 18;
    /* smb_tsod_poll_rate - Bits[17:0], RWS, default = 18'b111110100000000000 
       TSOD poll rate configuration between consecutive TSOD accesses to the TSOD 
       devices on the same SMBUS port. This field specifies the TSOD poll rate in 
       number of 500ns per CNFG_500_NANOSEC register field definition. The value of 
       this field must be greater than 0x2400 (poll rate is less than ~217 TSODs polled 
       per second). Instance 0 is shared between SMBUS ports 0 and 1, and instance 1 is 
       used for SMBUS port 2 (MCP). 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TSOD_POLL_RATE_CFG_1_PCU_FUN5_STRUCT;


/* SMB_MUX_CONFIG_CFG_0_PCU_FUN5_REG supported on:                            */
/*       SKX_A0 (0x401F508C)                                                  */
/*       SKX (0x401F508C)                                                     */
/* Register default value:              0x76540000                            */
#define SMB_MUX_CONFIG_CFG_0_PCU_FUN5_REG 0x0405408C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains the configuration of SMBUS MUX if it is present in the 
 * platform.  
 */
typedef union {
  struct {
    UINT32 mux_exists : 1;
    /* mux_exists - Bits[0:0], RWS, default = 1'b0 
       Describes if a SMBUS Mux is connected to the SMBUS port.  Must be zero if no 
       SMBUS mux present. 
     */
    UINT32 branch_exists : 4;
    /* branch_exists - Bits[4:1], RWS, default = 4'b0000 
       Describes if hardware should poll the TSOD devices on a given branch.  Bit0 - 
       When 1, hardware will poll devices on Branch0.  Bit1 - When 1, hardware will 
       poll devices on Branch1.  Bit2 - When 1, hardware will poll devices on Branch2.  
       Bit3- RSVD and must be 0. 
     */
    UINT32 mux_address : 3;
    /* mux_address - Bits[7:5], RWS, default = 3'b000 
       Slave address for the SMBUS MUX
     */
    UINT32 rsvd_8 : 8;
    /* rsvd_8 - Bits[15:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux_branch0_config : 3;
    /* mux_branch0_config - Bits[18:16], RWS, default = 3'b100 
       Configuration for accessing Branch 0 sent to SMBUS MUX. Bit 2 is mux enable bit. 
       Bits[1:0] is the branch to select. 
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux_branch1_config : 3;
    /* mux_branch1_config - Bits[22:20], RWS, default = 3'b101 
       Configuration for accessing Branch 1 sent to SMBUS MUX. Bit 2 is mux enable bit. 
       Bits[1:0] is the branch to select. 
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux_branch2_config : 3;
    /* mux_branch2_config - Bits[26:24], RWS, default = 3'b110 
       Configuration for accessing Branch 2 sent to SMBUS MUX. Bit 2 is mux enable bit. 
       Bits[1:0] is the branch to select. 
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux_branch3_config : 3;
    /* mux_branch3_config - Bits[30:28], RWS, default = 3'b111 
       Reserved field for branch3_cfg. Configuration for accessing Branch 3 sent to 
       SMBUS MUX. Bit 2 is mux enable bit. Bits[1:0] is the branch to select. Note: 
       branch3 is not supported on the processor. This field is not used in HW. 
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_MUX_CONFIG_CFG_0_PCU_FUN5_STRUCT;


/* SMB_MUX_CONFIG_CFG_1_PCU_FUN5_REG supported on:                            */
/*       SKX_A0 (0x401F5090)                                                  */
/*       SKX (0x401F5090)                                                     */
/* Register default value:              0x76540000                            */
#define SMB_MUX_CONFIG_CFG_1_PCU_FUN5_REG 0x04054090
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains the configuration of SMBUS MUX if it is present in the 
 * platform.  
 */
typedef union {
  struct {
    UINT32 mux_exists : 1;
    /* mux_exists - Bits[0:0], RWS, default = 1'b0 
       Describes if a SMBUS Mux is connected to the SMBUS port.  Must be zero if no 
       SMBUS mux present. 
     */
    UINT32 branch_exists : 4;
    /* branch_exists - Bits[4:1], RWS, default = 4'b0000 
       Describes if hardware should poll the TSOD devices on a given branch.  Bit0 - 
       When 1, hardware will poll devices on Branch0.  Bit1 - When 1, hardware will 
       poll devices on Branch1.  Bit2 - When 1, hardware will poll devices on Branch2.  
       Bit3- RSVD and must be 0. 
     */
    UINT32 mux_address : 3;
    /* mux_address - Bits[7:5], RWS, default = 3'b000 
       Slave address for the SMBUS MUX
     */
    UINT32 rsvd_8 : 8;
    /* rsvd_8 - Bits[15:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux_branch0_config : 3;
    /* mux_branch0_config - Bits[18:16], RWS, default = 3'b100 
       Configuration for accessing Branch 0 sent to SMBUS MUX. Bit 2 is mux enable bit. 
       Bits[1:0] is the branch to select. 
     */
    UINT32 rsvd_19 : 1;
    /* rsvd_19 - Bits[19:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux_branch1_config : 3;
    /* mux_branch1_config - Bits[22:20], RWS, default = 3'b101 
       Configuration for accessing Branch 1 sent to SMBUS MUX. Bit 2 is mux enable bit. 
       Bits[1:0] is the branch to select. 
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux_branch2_config : 3;
    /* mux_branch2_config - Bits[26:24], RWS, default = 3'b110 
       Configuration for accessing Branch 2 sent to SMBUS MUX. Bit 2 is mux enable bit. 
       Bits[1:0] is the branch to select. 
     */
    UINT32 rsvd_27 : 1;
    /* rsvd_27 - Bits[27:27], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mux_branch3_config : 3;
    /* mux_branch3_config - Bits[30:28], RWS, default = 3'b111 
       Reserved field for branch3_cfg. Configuration for accessing Branch 3 sent to 
       SMBUS MUX. Bit 2 is mux enable bit. Bits[1:0] is the branch to select. Note: 
       branch3 is not supported on the processor. This field is not used in HW. 
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_MUX_CONFIG_CFG_1_PCU_FUN5_STRUCT;


/* SMB_TSOD_CONFIG_CFG_0_PCU_FUN5_REG supported on:                           */
/*       SKX_A0 (0x401F5094)                                                  */
/*       SKX (0x401F5094)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_TSOD_CONFIG_CFG_0_PCU_FUN5_REG 0x04054094
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains three groups of bits which indicate to the FSM whether a 
 * DIMM is present at an address or not. 
 */
typedef union {
  struct {
    UINT32 group0_dimm_present : 8;
    /* group0_dimm_present - Bits[7:0], RW_LB, default = 8'b00000000 
       Describes the presence of DIMMs on one SMBUS port.  
       Bit0 - Channel0, DIMM0.  
       Bit1 - Channel0, DIMM1.  
       Bit2 - Channel1, DIMM0.  
       Bit3 - Channel1, DIMM1.  
       Bit4 - Channel2, DIMM0.  
       Bit5 - Channel2, DIMM1.  
       Bit6 - RSVD.
       Bit7 - RSVD.
     */
    UINT32 group1_dimm_present : 8;
    /* group1_dimm_present - Bits[15:8], RW_LB, default = 8'b00000000 
       reserved and must be zero. 
     */
    UINT32 group2_dimm_present : 8;
    /* group2_dimm_present - Bits[23:16], RW_LB, default = 8'b00000000 
       reserved and must be zero. 
     */
    UINT32 group3_dimm_present : 8;
    /* group3_dimm_present - Bits[31:24], RW_LB, default = 8'b00000000 
       reserved and must be zero. 
     */
  } Bits;
  UINT32 Data;
} SMB_TSOD_CONFIG_CFG_0_PCU_FUN5_STRUCT;


/* SMB_TSOD_CONFIG_CFG_1_PCU_FUN5_REG supported on:                           */
/*       SKX_A0 (0x401F5098)                                                  */
/*       SKX (0x401F5098)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_TSOD_CONFIG_CFG_1_PCU_FUN5_REG 0x04054098
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains three groups of bits which indicate to the FSM whether a 
 * DIMM is present at an address or not. 
 */
typedef union {
  struct {
    UINT32 group0_dimm_present : 8;
    /* group0_dimm_present - Bits[7:0], RW_LB, default = 8'b00000000 
       Describes the presence of DIMMs on one SMBUS port.  
       Bit0 - Channel0, DIMM0.  
       Bit1 - Channel0, DIMM1.  
       Bit2 - Channel1, DIMM0.  
       Bit3 - Channel1, DIMM1.  
       Bit4 - Channel2, DIMM0.  
       Bit5 - Channel2, DIMM1.  
       Bit6 - RSVD.
       Bit7 - RSVD.
     */
    UINT32 group1_dimm_present : 8;
    /* group1_dimm_present - Bits[15:8], RW_LB, default = 8'b00000000 
       reserved and must be zero. 
     */
    UINT32 group2_dimm_present : 8;
    /* group2_dimm_present - Bits[23:16], RW_LB, default = 8'b00000000 
       reserved and must be zero. 
     */
    UINT32 group3_dimm_present : 8;
    /* group3_dimm_present - Bits[31:24], RW_LB, default = 8'b00000000 
       reserved and must be zero. 
     */
  } Bits;
  UINT32 Data;
} SMB_TSOD_CONFIG_CFG_1_PCU_FUN5_STRUCT;


/* SMB_CMD_CFG_0_PCU_FUN5_REG supported on:                                   */
/*       SKX_A0 (0x401F509C)                                                  */
/*       SKX (0x401F509C)                                                     */
/* Register default value:              0x20005000                            */
#define SMB_CMD_CFG_0_PCU_FUN5_REG 0x0405409C
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains the fields used by SW to issue a SMBUS command. Note: the 
 * third instance is reserved.  
 */
typedef union {
  struct {
    UINT32 smb_ba : 8;
    /* smb_ba - Bits[7:0], RWS, default = 8'b00000000 
       Bus Txn Address: This field identifies the bus transaction address to be 
       accessed.  Note: in WORD access, 23:16 specifies 2B access address. In Byte 
       access, 23:16 specified 1B access address. Also known as the command code in 
       SMBus terms.  
     */
    UINT32 smb_sa : 3;
    /* smb_sa - Bits[10:8], RWS, default = 3'b000 
       Slave Address: This field identifies the DIMM SPD/TSOD to be accessed.
     */
    UINT32 smb_dti : 4;
    /* smb_dti - Bits[14:11], RWS, default = 4'b1010 
       Device Type Identifier: This field specifies the device type identifier. Only 
       devices with this device-type will respond to commands.  '0011' specifies TSOD.  
       '1010' specifies EEPROM's.  '0110' specifies a write-protect operation for an 
       EEPROM.  '1110' specifies a 4-channel I2C-bus multiplexer.  Other identifiers 
       can be specified to target non-EEPROM devices on the SMBus.  Note: IMC based 
       hardware TSOD polling uses hardcoded DTI. Changing this field has no effect on 
       the hardware based TSOD polling. 
     */
    UINT32 smb_wrt : 2;
    /* smb_wrt - Bits[16:15], RWS, default = 2'b00 
       Bit[28:27]=00: SMBus Read.  Bit[28:27]=01: SMBus Write.  Bit[28:27]=10: illegal 
       combination.  Bit[28:27]=11: Write to pointer registerSMBus/I2C pointer update 
       (byte).  smb_pntr_sel and smb_word_access are ignored. Note: smb_dis_wrt will 
       NOT disable WrtPntr update command. 
     */
    UINT32 smb_word_access : 1;
    /* smb_word_access - Bits[17:17], RWS, default = 1'b0 
       word access: SMBus/I2C word (2B) access when set; otherwise, it is a byte 
       access. 
     */
    UINT32 smb_pntr_sel : 1;
    /* smb_pntr_sel - Bits[18:18], RWS, default = 1'b0 
       Pointer Selection: SMBus/I2C present pointer based access enable when set; 
       otherwise, use random access protocol. Hardware based TSOD polling will also use 
       this bit to enable the pointer word read.  Important Note: Hardware based TSOD 
       polling can be configured with pointer based access. If software manually issue 
       SMBus transaction to other address, i.e. changing the pointer in the slave 
       device, it is software's responsibility to restore the pointer in each TSOD 
       before returning to hardware based TSOD polling while keeping the 
       SMB_PNTR_SEL=1. 
     */
    UINT32 smb_cmd_trigger : 1;
    /* smb_cmd_trigger - Bits[19:19], RW_V, default = 1'b0 
       CMD trigger: After setting this bit to 1, the SMBus master will issue the SMBus 
       command using the other fields written in SMBCMD_[0:1] and SMBCntl_[0:1]. Note: 
       the '-V' in the attribute implies the hardware will reset this bit when the 
       SMBus command is being started 
     */
    UINT32 smb_tsod_poll_en : 1;
    /* smb_tsod_poll_en - Bits[20:20], RW_LB, default = 1'b0 
       Enables hardware-based TSOD Polling. This field does not affect the current 
       SMBus transaction in flight. A change takes effect once the bus is no longer 
       busy. If BIOS/BMC would like to use SMBus, it must first suspend TSOD polling by 
       setting this field to 0 and wait for the bus to become free. When BIOS/BMC is 
       done with the bus, It must resume TSOD polling by setting this field to 1.  
     */
    UINT32 rsvd_21 : 3;
    /* rsvd_21 - Bits[23:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 smb_soft_rst : 1;
    /* smb_soft_rst - Bits[24:24], RW, default = 1'b0 
       SMBus software reset strobe to graceful terminate pending transaction (after 
       ACK) and keep the SMB from issuing any transaction until this bit is cleared. If 
       slave device is hung, software can write this bit to 1 and the SMB_CKOVRD to 0 
       (for more than 35ms) to force hung the SMB slaves to time-out and put it in idle 
       state without using power good reset or warm reset.  Note: software need to set 
       the SMB_CKOVRD back to 1 after 35ms in order to force slave devices to time-out 
       in case there is any pending transaction. The corresponding SMB_STAT_x.SMB_SBE 
       error status bit may be set if there was such pending transaction time-out 
       (non-graceful termination). If the pending transaction was a write operation, 
       the slave device content may be corrupted by this clock overide operation. A 
       subsequent SMB command will automatically cleared the SMB_SBE. The IMC H/W will 
       perform SMB time-out (if the SMB_SBE_EN=1). Software should simply clear the 
       SMB_SBE and SMB_SOFT_RST sequentially after writing the SMB_CKOVRD=0 and 
       SMB_SOFT_RST=1 (asserting clock override and perform graceful txn termination). 
       Hardware will automatically de-assert the SMB_CKOVRD (update to 1) after the 
       pre-configured 35ms/65ms time-out.  
     */
    UINT32 smb_sbe_err0_en : 1;
    /* smb_sbe_err0_en - Bits[25:25], RW, default = 1'b0 
       Enable ERR0 assertion when SMB_SBE is 0 --> 1.
     */
    UINT32 smb_sbe_smi_en : 1;
    /* smb_sbe_smi_en - Bits[26:26], RW, default = 1'b0 
       Enable SMI generation when SMB_SBE is 0 --> 1. If enabled, sends an SMI 
       interrupt when smb_sbe is detected. 
     */
    UINT32 smb_sbe_en : 1;
    /* smb_sbe_en - Bits[27:27], RW, default = 1'b0 
       SMBus errer recovery enable . When set, hardware will automatically reset 
       smb_ckovrd to 1 after TLOW Timeout counter (SMB_TLOW_TIMEOUT_CNTR_CFG) expires. 
     */
    UINT32 smb_dis_wrt : 1;
    /* smb_dis_wrt - Bits[28:28], RW_LB, default = 1'b0 
       Disable SMBus Write.  Writing a '0' to this bit enables CMD write bit to be set 
       to 1; Writing a '1' forces CMD.write bit to be always 0, i.e. disabling SMBus 
       write. SMBus Read is not affected. I2C Write Pointer Update Command is not 
       affected.  Either SMB_DIS_WRT=1 or the FUSE_SHADOW_DIS_SMBUS_WRT=1 will disable 
       the SMBus Write Capability.  Note: SMBus write commands are silently converted 
       to SMBus read commands. No error is logged or signaled. 
     */
    UINT32 smb_ckovrd : 1;
    /* smb_ckovrd - Bits[29:29], RWS_V, default = 1'b1 
       Clock Override.  '0' = Clock signal is driven low, overriding writing a '1' to 
       CMD.  '1' = Clock signal is released high, allowing normal operation of CMD.  
       Toggling this bit can be used to 'budge' the port out of a 'stuck' state.  
       Software can write this bit to 0 and the SMB_SOFT_RST to 1 to force hung SMBus 
       controller and the SMB slaves to idle state without using power good reset or 
       warm reset.  Note: software need to set the SMB_CKOVRD back to 1 after 35ms in 
       order to force slave devices to time-out in case there is any pending 
       transaction. The corresponding SMB_STAT_x.SMB_SBE error status bit may be set if 
       there was such pending transaction time-out (non-graceful termination). If the 
       pending transaction was a write operation, the slave device content may be 
       corrupted by this clock overide operation. A subsequent SMB command will 
       automatically cleared the SMB_SBE.  Alternatively, if SMBUS error recovery is 
       enabled, the SMBus time-out control timer will start when ckovrd is pulled low. 
       When the time-out control timer expires, the SMB_CKOVRD# will "de-assert", i.e. 
       return to 1 value and clear the SMB_SBE=0. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_CMD_CFG_0_PCU_FUN5_STRUCT;


/* SMB_CMD_CFG_1_PCU_FUN5_REG supported on:                                   */
/*       SKX_A0 (0x401F50A0)                                                  */
/*       SKX (0x401F50A0)                                                     */
/* Register default value:              0x20005000                            */
#define SMB_CMD_CFG_1_PCU_FUN5_REG 0x040540A0
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains the fields used by SW to issue a SMBUS command. Note: the 
 * third instance is reserved.  
 */
typedef union {
  struct {
    UINT32 smb_ba : 8;
    /* smb_ba - Bits[7:0], RWS, default = 8'b00000000 
       Bus Txn Address: This field identifies the bus transaction address to be 
       accessed.  Note: in WORD access, 23:16 specifies 2B access address. In Byte 
       access, 23:16 specified 1B access address. Also known as the command code in 
       SMBus terms.  
     */
    UINT32 smb_sa : 3;
    /* smb_sa - Bits[10:8], RWS, default = 3'b000 
       Slave Address: This field identifies the DIMM SPD/TSOD to be accessed.
     */
    UINT32 smb_dti : 4;
    /* smb_dti - Bits[14:11], RWS, default = 4'b1010 
       Device Type Identifier: This field specifies the device type identifier. Only 
       devices with this device-type will respond to commands.  '0011' specifies TSOD.  
       '1010' specifies EEPROM's.  '0110' specifies a write-protect operation for an 
       EEPROM.  '1110' specifies a 4-channel I2C-bus multiplexer.  Other identifiers 
       can be specified to target non-EEPROM devices on the SMBus.  Note: IMC based 
       hardware TSOD polling uses hardcoded DTI. Changing this field has no effect on 
       the hardware based TSOD polling. 
     */
    UINT32 smb_wrt : 2;
    /* smb_wrt - Bits[16:15], RWS, default = 2'b00 
       Bit[28:27]=00: SMBus Read.  Bit[28:27]=01: SMBus Write.  Bit[28:27]=10: illegal 
       combination.  Bit[28:27]=11: Write to pointer registerSMBus/I2C pointer update 
       (byte).  smb_pntr_sel and smb_word_access are ignored. Note: smb_dis_wrt will 
       NOT disable WrtPntr update command. 
     */
    UINT32 smb_word_access : 1;
    /* smb_word_access - Bits[17:17], RWS, default = 1'b0 
       word access: SMBus/I2C word (2B) access when set; otherwise, it is a byte 
       access. 
     */
    UINT32 smb_pntr_sel : 1;
    /* smb_pntr_sel - Bits[18:18], RWS, default = 1'b0 
       Pointer Selection: SMBus/I2C present pointer based access enable when set; 
       otherwise, use random access protocol. Hardware based TSOD polling will also use 
       this bit to enable the pointer word read.  Important Note: Hardware based TSOD 
       polling can be configured with pointer based access. If software manually issue 
       SMBus transaction to other address, i.e. changing the pointer in the slave 
       device, it is software's responsibility to restore the pointer in each TSOD 
       before returning to hardware based TSOD polling while keeping the 
       SMB_PNTR_SEL=1. 
     */
    UINT32 smb_cmd_trigger : 1;
    /* smb_cmd_trigger - Bits[19:19], RW_V, default = 1'b0 
       CMD trigger: After setting this bit to 1, the SMBus master will issue the SMBus 
       command using the other fields written in SMBCMD_[0:1] and SMBCntl_[0:1]. Note: 
       the '-V' in the attribute implies the hardware will reset this bit when the 
       SMBus command is being started 
     */
    UINT32 smb_tsod_poll_en : 1;
    /* smb_tsod_poll_en - Bits[20:20], RW_LB, default = 1'b0 
       Enables hardware-based TSOD Polling. This field does not affect the current 
       SMBus transaction in flight. A change takes effect once the bus is no longer 
       busy. If BIOS/BMC would like to use SMBus, it must first suspend TSOD polling by 
       setting this field to 0 and wait for the bus to become free. When BIOS/BMC is 
       done with the bus, It must resume TSOD polling by setting this field to 1.  
     */
    UINT32 rsvd_21 : 3;
    /* rsvd_21 - Bits[23:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 smb_soft_rst : 1;
    /* smb_soft_rst - Bits[24:24], RW, default = 1'b0 
       SMBus software reset strobe to graceful terminate pending transaction (after 
       ACK) and keep the SMB from issuing any transaction until this bit is cleared. If 
       slave device is hung, software can write this bit to 1 and the SMB_CKOVRD to 0 
       (for more than 35ms) to force hung the SMB slaves to time-out and put it in idle 
       state without using power good reset or warm reset.  Note: software need to set 
       the SMB_CKOVRD back to 1 after 35ms in order to force slave devices to time-out 
       in case there is any pending transaction. The corresponding SMB_STAT_x.SMB_SBE 
       error status bit may be set if there was such pending transaction time-out 
       (non-graceful termination). If the pending transaction was a write operation, 
       the slave device content may be corrupted by this clock overide operation. A 
       subsequent SMB command will automatically cleared the SMB_SBE. The IMC H/W will 
       perform SMB time-out (if the SMB_SBE_EN=1). Software should simply clear the 
       SMB_SBE and SMB_SOFT_RST sequentially after writing the SMB_CKOVRD=0 and 
       SMB_SOFT_RST=1 (asserting clock override and perform graceful txn termination). 
       Hardware will automatically de-assert the SMB_CKOVRD (update to 1) after the 
       pre-configured 35ms/65ms time-out.  
     */
    UINT32 smb_sbe_err0_en : 1;
    /* smb_sbe_err0_en - Bits[25:25], RW, default = 1'b0 
       Enable ERR0 assertion when SMB_SBE is 0 --> 1.
     */
    UINT32 smb_sbe_smi_en : 1;
    /* smb_sbe_smi_en - Bits[26:26], RW, default = 1'b0 
       Enable SMI generation when SMB_SBE is 0 --> 1. If enabled, sends an SMI 
       interrupt when smb_sbe is detected. 
     */
    UINT32 smb_sbe_en : 1;
    /* smb_sbe_en - Bits[27:27], RW, default = 1'b0 
       SMBus errer recovery enable . When set, hardware will automatically reset 
       smb_ckovrd to 1 after TLOW Timeout counter (SMB_TLOW_TIMEOUT_CNTR_CFG) expires. 
     */
    UINT32 smb_dis_wrt : 1;
    /* smb_dis_wrt - Bits[28:28], RW_LB, default = 1'b0 
       Disable SMBus Write.  Writing a '0' to this bit enables CMD write bit to be set 
       to 1; Writing a '1' forces CMD.write bit to be always 0, i.e. disabling SMBus 
       write. SMBus Read is not affected. I2C Write Pointer Update Command is not 
       affected.  Either SMB_DIS_WRT=1 or the FUSE_SHADOW_DIS_SMBUS_WRT=1 will disable 
       the SMBus Write Capability.  Note: SMBus write commands are silently converted 
       to SMBus read commands. No error is logged or signaled. 
     */
    UINT32 smb_ckovrd : 1;
    /* smb_ckovrd - Bits[29:29], RWS_V, default = 1'b1 
       Clock Override.  '0' = Clock signal is driven low, overriding writing a '1' to 
       CMD.  '1' = Clock signal is released high, allowing normal operation of CMD.  
       Toggling this bit can be used to 'budge' the port out of a 'stuck' state.  
       Software can write this bit to 0 and the SMB_SOFT_RST to 1 to force hung SMBus 
       controller and the SMB slaves to idle state without using power good reset or 
       warm reset.  Note: software need to set the SMB_CKOVRD back to 1 after 35ms in 
       order to force slave devices to time-out in case there is any pending 
       transaction. The corresponding SMB_STAT_x.SMB_SBE error status bit may be set if 
       there was such pending transaction time-out (non-graceful termination). If the 
       pending transaction was a write operation, the slave device content may be 
       corrupted by this clock overide operation. A subsequent SMB command will 
       automatically cleared the SMB_SBE.  Alternatively, if SMBUS error recovery is 
       enabled, the SMBus time-out control timer will start when ckovrd is pulled low. 
       When the time-out control timer expires, the SMB_CKOVRD# will "de-assert", i.e. 
       return to 1 value and clear the SMB_SBE=0. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_CMD_CFG_1_PCU_FUN5_STRUCT;


/* SMB_CMD_CFG_2_PCU_FUN5_REG supported on:                                   */
/*       SKX_A0 (0x401F50A4)                                                  */
/*       SKX (0x401F50A4)                                                     */
/* Register default value:              0x20005000                            */
#define SMB_CMD_CFG_2_PCU_FUN5_REG 0x040540A4
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains the fields used by SW to issue a SMBUS command. Note: the 
 * third instance is reserved.  
 */
typedef union {
  struct {
    UINT32 smb_ba : 8;
    /* smb_ba - Bits[7:0], RWS, default = 8'b00000000 
       Bus Txn Address: This field identifies the bus transaction address to be 
       accessed.  Note: in WORD access, 23:16 specifies 2B access address. In Byte 
       access, 23:16 specified 1B access address. Also known as the command code in 
       SMBus terms.  
     */
    UINT32 smb_sa : 3;
    /* smb_sa - Bits[10:8], RWS, default = 3'b000 
       Slave Address: This field identifies the DIMM SPD/TSOD to be accessed.
     */
    UINT32 smb_dti : 4;
    /* smb_dti - Bits[14:11], RWS, default = 4'b1010 
       Device Type Identifier: This field specifies the device type identifier. Only 
       devices with this device-type will respond to commands.  '0011' specifies TSOD.  
       '1010' specifies EEPROM's.  '0110' specifies a write-protect operation for an 
       EEPROM.  '1110' specifies a 4-channel I2C-bus multiplexer.  Other identifiers 
       can be specified to target non-EEPROM devices on the SMBus.  Note: IMC based 
       hardware TSOD polling uses hardcoded DTI. Changing this field has no effect on 
       the hardware based TSOD polling. 
     */
    UINT32 smb_wrt : 2;
    /* smb_wrt - Bits[16:15], RWS, default = 2'b00 
       Bit[28:27]=00: SMBus Read.  Bit[28:27]=01: SMBus Write.  Bit[28:27]=10: illegal 
       combination.  Bit[28:27]=11: Write to pointer registerSMBus/I2C pointer update 
       (byte).  smb_pntr_sel and smb_word_access are ignored. Note: smb_dis_wrt will 
       NOT disable WrtPntr update command. 
     */
    UINT32 smb_word_access : 1;
    /* smb_word_access - Bits[17:17], RWS, default = 1'b0 
       word access: SMBus/I2C word (2B) access when set; otherwise, it is a byte 
       access. 
     */
    UINT32 smb_pntr_sel : 1;
    /* smb_pntr_sel - Bits[18:18], RWS, default = 1'b0 
       Pointer Selection: SMBus/I2C present pointer based access enable when set; 
       otherwise, use random access protocol. Hardware based TSOD polling will also use 
       this bit to enable the pointer word read.  Important Note: Hardware based TSOD 
       polling can be configured with pointer based access. If software manually issue 
       SMBus transaction to other address, i.e. changing the pointer in the slave 
       device, it is software's responsibility to restore the pointer in each TSOD 
       before returning to hardware based TSOD polling while keeping the 
       SMB_PNTR_SEL=1. 
     */
    UINT32 smb_cmd_trigger : 1;
    /* smb_cmd_trigger - Bits[19:19], RW_V, default = 1'b0 
       CMD trigger: After setting this bit to 1, the SMBus master will issue the SMBus 
       command using the other fields written in SMBCMD_[0:1] and SMBCntl_[0:1]. Note: 
       the '-V' in the attribute implies the hardware will reset this bit when the 
       SMBus command is being started 
     */
    UINT32 smb_tsod_poll_en : 1;
    /* smb_tsod_poll_en - Bits[20:20], RW_LB, default = 1'b0 
       Enables hardware-based TSOD Polling. This field does not affect the current 
       SMBus transaction in flight. A change takes effect once the bus is no longer 
       busy. If BIOS/BMC would like to use SMBus, it must first suspend TSOD polling by 
       setting this field to 0 and wait for the bus to become free. When BIOS/BMC is 
       done with the bus, It must resume TSOD polling by setting this field to 1.  
     */
    UINT32 rsvd_21 : 3;
    /* rsvd_21 - Bits[23:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 smb_soft_rst : 1;
    /* smb_soft_rst - Bits[24:24], RW, default = 1'b0 
       SMBus software reset strobe to graceful terminate pending transaction (after 
       ACK) and keep the SMB from issuing any transaction until this bit is cleared. If 
       slave device is hung, software can write this bit to 1 and the SMB_CKOVRD to 0 
       (for more than 35ms) to force hung the SMB slaves to time-out and put it in idle 
       state without using power good reset or warm reset.  Note: software need to set 
       the SMB_CKOVRD back to 1 after 35ms in order to force slave devices to time-out 
       in case there is any pending transaction. The corresponding SMB_STAT_x.SMB_SBE 
       error status bit may be set if there was such pending transaction time-out 
       (non-graceful termination). If the pending transaction was a write operation, 
       the slave device content may be corrupted by this clock overide operation. A 
       subsequent SMB command will automatically cleared the SMB_SBE. The IMC H/W will 
       perform SMB time-out (if the SMB_SBE_EN=1). Software should simply clear the 
       SMB_SBE and SMB_SOFT_RST sequentially after writing the SMB_CKOVRD=0 and 
       SMB_SOFT_RST=1 (asserting clock override and perform graceful txn termination). 
       Hardware will automatically de-assert the SMB_CKOVRD (update to 1) after the 
       pre-configured 35ms/65ms time-out.  
     */
    UINT32 smb_sbe_err0_en : 1;
    /* smb_sbe_err0_en - Bits[25:25], RW, default = 1'b0 
       Enable ERR0 assertion when SMB_SBE is 0 --> 1.
     */
    UINT32 smb_sbe_smi_en : 1;
    /* smb_sbe_smi_en - Bits[26:26], RW, default = 1'b0 
       Enable SMI generation when SMB_SBE is 0 --> 1. If enabled, sends an SMI 
       interrupt when smb_sbe is detected. 
     */
    UINT32 smb_sbe_en : 1;
    /* smb_sbe_en - Bits[27:27], RW, default = 1'b0 
       SMBus errer recovery enable . When set, hardware will automatically reset 
       smb_ckovrd to 1 after TLOW Timeout counter (SMB_TLOW_TIMEOUT_CNTR_CFG) expires. 
     */
    UINT32 smb_dis_wrt : 1;
    /* smb_dis_wrt - Bits[28:28], RW_LB, default = 1'b0 
       Disable SMBus Write.  Writing a '0' to this bit enables CMD write bit to be set 
       to 1; Writing a '1' forces CMD.write bit to be always 0, i.e. disabling SMBus 
       write. SMBus Read is not affected. I2C Write Pointer Update Command is not 
       affected.  Either SMB_DIS_WRT=1 or the FUSE_SHADOW_DIS_SMBUS_WRT=1 will disable 
       the SMBus Write Capability.  Note: SMBus write commands are silently converted 
       to SMBus read commands. No error is logged or signaled. 
     */
    UINT32 smb_ckovrd : 1;
    /* smb_ckovrd - Bits[29:29], RWS_V, default = 1'b1 
       Clock Override.  '0' = Clock signal is driven low, overriding writing a '1' to 
       CMD.  '1' = Clock signal is released high, allowing normal operation of CMD.  
       Toggling this bit can be used to 'budge' the port out of a 'stuck' state.  
       Software can write this bit to 0 and the SMB_SOFT_RST to 1 to force hung SMBus 
       controller and the SMB slaves to idle state without using power good reset or 
       warm reset.  Note: software need to set the SMB_CKOVRD back to 1 after 35ms in 
       order to force slave devices to time-out in case there is any pending 
       transaction. The corresponding SMB_STAT_x.SMB_SBE error status bit may be set if 
       there was such pending transaction time-out (non-graceful termination). If the 
       pending transaction was a write operation, the slave device content may be 
       corrupted by this clock overide operation. A subsequent SMB command will 
       automatically cleared the SMB_SBE.  Alternatively, if SMBUS error recovery is 
       enabled, the SMBus time-out control timer will start when ckovrd is pulled low. 
       When the time-out control timer expires, the SMB_CKOVRD# will "de-assert", i.e. 
       return to 1 value and clear the SMB_SBE=0. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_CMD_CFG_2_PCU_FUN5_STRUCT;


/* SMB_STATUS_CFG_0_PCU_FUN5_REG supported on:                                */
/*       SKX_A0 (0x401F50A8)                                                  */
/*       SKX (0x401F50A8)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_STATUS_CFG_0_PCU_FUN5_REG 0x040540A8
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains the fields to monitor the status
 */
typedef union {
  struct {
    UINT32 smb_busy : 1;
    /* smb_busy - Bits[0:0], ROS_V, default = 1'b0 
       SMBus Busy state. This bit is set by iMC while an SMBus/I2C command (including 
       TSOD command issued from IMC hardware) is executing. Any transaction that is 
       completed normally or gracefully will clear this bit automatically. By setting 
       the SMB_SOFT_RST will also clear this bit.  This register bit is sticky across 
       reset so any surprise reset during pending SMBus operation will sustain the bit 
       assertion across surprised warm-reset. BIOS reset handler can read this bit 
       before issuing any SMBus transaction to determine whether a slave device may 
       need special care to force the slave to idle state (e.g. via clock override 
       toggling (SMB_CKOVRD) and/or via induced time-out by asserting SMB_CKOVRD for 
       25-35ms). 
     */
    UINT32 smb_sbe : 1;
    /* smb_sbe - Bits[1:1], RO_V, default = 1'b0 
       SMBus Error.  This bit is set by H/W if an SMBus transaction (including the TSOD 
       polling or message channel initiated SMBus access) that does not complete 
       successfully (non-Ack has been received from slave at expected Ack slot of the 
       transferNote: Once SMB_SBE bit is set, H/W stops issuing hardware initiated TSOD 
       polling SMBUS transactions until the SMB_SBE is cleared. H/W will not increment 
       the SMB_STATUS_CFG.TSOD_SA or update SMB_STATUS_CFG.last_dti or 
       SMB_STATUS_CFG.last_branch_config until the SMB_SBE is cleared. Manual SMBus 
       command interface is not affected, i.e. new command issue will clear the 
       SMB_SBE.  
     */
    UINT32 smb_rdo : 1;
    /* smb_rdo - Bits[2:2], RO_V, default = 1'b0 
       Read Data Valid.  This bit is set by iMC when the Data field of this register 
       receives read data from the SPD/TSOD after completion of an SMBus read command. 
       It is cleared by iMC when a subsequent SMBus read command is issued. 
     */
    UINT32 smb_wod : 1;
    /* smb_wod - Bits[3:3], RO_V, default = 1'b0 
       Write Operation Done.  This bit is set by iMC when a SMBus Write command has 
       been completed on the SMBus. It is cleared by iMC when a subsequent SMBus Write 
       command is issued 
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tsod_sa : 3;
    /* tsod_sa - Bits[10:8], RO_V, default = 3'b000 
       Captures the slave address of the last TSOD device polled.
     */
    UINT32 last_dti : 4;
    /* last_dti - Bits[14:11], RO_V, default = 4'b0000 
       Captures the Device Type Identified code specified in the last issued BIOS/BMC 
       or HW command. BMC can use this information to determine if an error on SMBus is 
       caused by SMBus mux or by a DIMM. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 last_branch_cfg : 3;
    /* last_branch_cfg - Bits[18:16], RO_V, default = 3'b000 
       Captures the SMBus mux branch configuration in the last issued BIOS/BMC or HW 
       command to SMBUS mux. BMC can use this information to determine which DIMM on 
       which branch has caused an error on SMBus.  Bit 2 is defined as the mux enable 
       bit. Bits [1:0] are defined as the branch to be selected. 
     */
    UINT32 rsvd_19 : 13;
    /* rsvd_19 - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_STATUS_CFG_0_PCU_FUN5_STRUCT;


/* SMB_STATUS_CFG_1_PCU_FUN5_REG supported on:                                */
/*       SKX_A0 (0x401F50AC)                                                  */
/*       SKX (0x401F50AC)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_STATUS_CFG_1_PCU_FUN5_REG 0x040540AC
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains the fields to monitor the status
 */
typedef union {
  struct {
    UINT32 smb_busy : 1;
    /* smb_busy - Bits[0:0], ROS_V, default = 1'b0 
       SMBus Busy state. This bit is set by iMC while an SMBus/I2C command (including 
       TSOD command issued from IMC hardware) is executing. Any transaction that is 
       completed normally or gracefully will clear this bit automatically. By setting 
       the SMB_SOFT_RST will also clear this bit.  This register bit is sticky across 
       reset so any surprise reset during pending SMBus operation will sustain the bit 
       assertion across surprised warm-reset. BIOS reset handler can read this bit 
       before issuing any SMBus transaction to determine whether a slave device may 
       need special care to force the slave to idle state (e.g. via clock override 
       toggling (SMB_CKOVRD) and/or via induced time-out by asserting SMB_CKOVRD for 
       25-35ms). 
     */
    UINT32 smb_sbe : 1;
    /* smb_sbe - Bits[1:1], RO_V, default = 1'b0 
       SMBus Error.  This bit is set by H/W if an SMBus transaction (including the TSOD 
       polling or message channel initiated SMBus access) that does not complete 
       successfully (non-Ack has been received from slave at expected Ack slot of the 
       transferNote: Once SMB_SBE bit is set, H/W stops issuing hardware initiated TSOD 
       polling SMBUS transactions until the SMB_SBE is cleared. H/W will not increment 
       the SMB_STATUS_CFG.TSOD_SA or update SMB_STATUS_CFG.last_dti or 
       SMB_STATUS_CFG.last_branch_config until the SMB_SBE is cleared. Manual SMBus 
       command interface is not affected, i.e. new command issue will clear the 
       SMB_SBE.  
     */
    UINT32 smb_rdo : 1;
    /* smb_rdo - Bits[2:2], RO_V, default = 1'b0 
       Read Data Valid.  This bit is set by iMC when the Data field of this register 
       receives read data from the SPD/TSOD after completion of an SMBus read command. 
       It is cleared by iMC when a subsequent SMBus read command is issued. 
     */
    UINT32 smb_wod : 1;
    /* smb_wod - Bits[3:3], RO_V, default = 1'b0 
       Write Operation Done.  This bit is set by iMC when a SMBus Write command has 
       been completed on the SMBus. It is cleared by iMC when a subsequent SMBus Write 
       command is issued 
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tsod_sa : 3;
    /* tsod_sa - Bits[10:8], RO_V, default = 3'b000 
       Captures the slave address of the last TSOD device polled.
     */
    UINT32 last_dti : 4;
    /* last_dti - Bits[14:11], RO_V, default = 4'b0000 
       Captures the Device Type Identified code specified in the last issued BIOS/BMC 
       or HW command. BMC can use this information to determine if an error on SMBus is 
       caused by SMBus mux or by a DIMM. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 last_branch_cfg : 3;
    /* last_branch_cfg - Bits[18:16], RO_V, default = 3'b000 
       Captures the SMBus mux branch configuration in the last issued BIOS/BMC or HW 
       command to SMBUS mux. BMC can use this information to determine which DIMM on 
       which branch has caused an error on SMBus.  Bit 2 is defined as the mux enable 
       bit. Bits [1:0] are defined as the branch to be selected. 
     */
    UINT32 rsvd_19 : 13;
    /* rsvd_19 - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_STATUS_CFG_1_PCU_FUN5_STRUCT;


/* SMB_STATUS_CFG_2_PCU_FUN5_REG supported on:                                */
/*       SKX_A0 (0x401F50B0)                                                  */
/*       SKX (0x401F50B0)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_STATUS_CFG_2_PCU_FUN5_REG 0x040540B0
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains the fields to monitor the status
 */
typedef union {
  struct {
    UINT32 smb_busy : 1;
    /* smb_busy - Bits[0:0], ROS_V, default = 1'b0 
       SMBus Busy state. This bit is set by iMC while an SMBus/I2C command (including 
       TSOD command issued from IMC hardware) is executing. Any transaction that is 
       completed normally or gracefully will clear this bit automatically. By setting 
       the SMB_SOFT_RST will also clear this bit.  This register bit is sticky across 
       reset so any surprise reset during pending SMBus operation will sustain the bit 
       assertion across surprised warm-reset. BIOS reset handler can read this bit 
       before issuing any SMBus transaction to determine whether a slave device may 
       need special care to force the slave to idle state (e.g. via clock override 
       toggling (SMB_CKOVRD) and/or via induced time-out by asserting SMB_CKOVRD for 
       25-35ms). 
     */
    UINT32 smb_sbe : 1;
    /* smb_sbe - Bits[1:1], RO_V, default = 1'b0 
       SMBus Error.  This bit is set by H/W if an SMBus transaction (including the TSOD 
       polling or message channel initiated SMBus access) that does not complete 
       successfully (non-Ack has been received from slave at expected Ack slot of the 
       transferNote: Once SMB_SBE bit is set, H/W stops issuing hardware initiated TSOD 
       polling SMBUS transactions until the SMB_SBE is cleared. H/W will not increment 
       the SMB_STATUS_CFG.TSOD_SA or update SMB_STATUS_CFG.last_dti or 
       SMB_STATUS_CFG.last_branch_config until the SMB_SBE is cleared. Manual SMBus 
       command interface is not affected, i.e. new command issue will clear the 
       SMB_SBE.  
     */
    UINT32 smb_rdo : 1;
    /* smb_rdo - Bits[2:2], RO_V, default = 1'b0 
       Read Data Valid.  This bit is set by iMC when the Data field of this register 
       receives read data from the SPD/TSOD after completion of an SMBus read command. 
       It is cleared by iMC when a subsequent SMBus read command is issued. 
     */
    UINT32 smb_wod : 1;
    /* smb_wod - Bits[3:3], RO_V, default = 1'b0 
       Write Operation Done.  This bit is set by iMC when a SMBus Write command has 
       been completed on the SMBus. It is cleared by iMC when a subsequent SMBus Write 
       command is issued 
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tsod_sa : 3;
    /* tsod_sa - Bits[10:8], RO_V, default = 3'b000 
       Captures the slave address of the last TSOD device polled.
     */
    UINT32 last_dti : 4;
    /* last_dti - Bits[14:11], RO_V, default = 4'b0000 
       Captures the Device Type Identified code specified in the last issued BIOS/BMC 
       or HW command. BMC can use this information to determine if an error on SMBus is 
       caused by SMBus mux or by a DIMM. 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 last_branch_cfg : 3;
    /* last_branch_cfg - Bits[18:16], RO_V, default = 3'b000 
       Captures the SMBus mux branch configuration in the last issued BIOS/BMC or HW 
       command to SMBUS mux. BMC can use this information to determine which DIMM on 
       which branch has caused an error on SMBus.  Bit 2 is defined as the mux enable 
       bit. Bits [1:0] are defined as the branch to be selected. 
     */
    UINT32 rsvd_19 : 13;
    /* rsvd_19 - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_STATUS_CFG_2_PCU_FUN5_STRUCT;


/* SMB_DATA_CFG_0_PCU_FUN5_REG supported on:                                  */
/*       SKX_A0 (0x401F50B4)                                                  */
/*       SKX (0x401F50B4)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_DATA_CFG_0_PCU_FUN5_REG 0x040540B4
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains read and write data from SMBUS. Note: the third instance 
 * is reserved.  
 */
typedef union {
  struct {
    UINT32 smb_rdata : 16;
    /* smb_rdata - Bits[15:0], RO_V, default = 16'b0000000000000000 
       Read Data.  Holds data read from SMBus Read commands.  Since TSOD/EEPROM are I2C 
       devices and the byte order is MSByte first in a word read, reading of I2C using 
       word read should return SMB_RDATA[15:8]=I2C_MSB and SMB_RDATA[7:0]=I2C_LSB. If 
       reading of I2C using byte read, the SMB_RDATA[15:8]=don`t care; 
       SMB_RDATA[7:0]=read_byte.  If we have a SMB slave connected on the bus, reading 
       of the SMBus slave using word read should return SMB_RDATA[15:8]=SMB_LSB and 
       SMB_RDATA[7:0]=SMB_MSB.  If the software is not sure whether the target is I2C 
       or SMBus slave, please use byte access. 
     */
    UINT32 smb_wdata : 16;
    /* smb_wdata - Bits[31:16], RWS, default = 16'b0000000000000000 
       Write Data: Holds data to be written by SPDW commands.  Since TSOD/EEPROM are 
       I2C devices and the byte order is MSByte first in a word write, writing of I2C 
       using word write should use SMB_WDATA[15:8]=I2C_MSB and SMB_WDATA[7:0]=I2C_LSB. 
       If writing of I2C using byte write, the SMB_WDATA[15:8]=don`t care; 
       SMB_WDATA[7:0]=write_byte.  If we have a SMB slave connected on the bus, writing 
       of the SMBus slave using word write should use SMB_WDATA[15:8]=SMB_LSB and 
       SMB_WDATA[7:0]=SMB_MSB.  It is software responsibility to figure out the byte 
       order of the slave access. 
     */
  } Bits;
  UINT32 Data;
} SMB_DATA_CFG_0_PCU_FUN5_STRUCT;


/* SMB_DATA_CFG_1_PCU_FUN5_REG supported on:                                  */
/*       SKX_A0 (0x401F50B8)                                                  */
/*       SKX (0x401F50B8)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_DATA_CFG_1_PCU_FUN5_REG 0x040540B8
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains read and write data from SMBUS. Note: the third instance 
 * is reserved.  
 */
typedef union {
  struct {
    UINT32 smb_rdata : 16;
    /* smb_rdata - Bits[15:0], RO_V, default = 16'b0000000000000000 
       Read Data.  Holds data read from SMBus Read commands.  Since TSOD/EEPROM are I2C 
       devices and the byte order is MSByte first in a word read, reading of I2C using 
       word read should return SMB_RDATA[15:8]=I2C_MSB and SMB_RDATA[7:0]=I2C_LSB. If 
       reading of I2C using byte read, the SMB_RDATA[15:8]=don`t care; 
       SMB_RDATA[7:0]=read_byte.  If we have a SMB slave connected on the bus, reading 
       of the SMBus slave using word read should return SMB_RDATA[15:8]=SMB_LSB and 
       SMB_RDATA[7:0]=SMB_MSB.  If the software is not sure whether the target is I2C 
       or SMBus slave, please use byte access. 
     */
    UINT32 smb_wdata : 16;
    /* smb_wdata - Bits[31:16], RWS, default = 16'b0000000000000000 
       Write Data: Holds data to be written by SPDW commands.  Since TSOD/EEPROM are 
       I2C devices and the byte order is MSByte first in a word write, writing of I2C 
       using word write should use SMB_WDATA[15:8]=I2C_MSB and SMB_WDATA[7:0]=I2C_LSB. 
       If writing of I2C using byte write, the SMB_WDATA[15:8]=don`t care; 
       SMB_WDATA[7:0]=write_byte.  If we have a SMB slave connected on the bus, writing 
       of the SMBus slave using word write should use SMB_WDATA[15:8]=SMB_LSB and 
       SMB_WDATA[7:0]=SMB_MSB.  It is software responsibility to figure out the byte 
       order of the slave access. 
     */
  } Bits;
  UINT32 Data;
} SMB_DATA_CFG_1_PCU_FUN5_STRUCT;


/* SMB_DATA_CFG_2_PCU_FUN5_REG supported on:                                  */
/*       SKX_A0 (0x401F50BC)                                                  */
/*       SKX (0x401F50BC)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_DATA_CFG_2_PCU_FUN5_REG 0x040540BC
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register contains read and write data from SMBUS. Note: the third instance 
 * is reserved.  
 */
typedef union {
  struct {
    UINT32 smb_rdata : 16;
    /* smb_rdata - Bits[15:0], RO_V, default = 16'b0000000000000000 
       Read Data.  Holds data read from SMBus Read commands.  Since TSOD/EEPROM are I2C 
       devices and the byte order is MSByte first in a word read, reading of I2C using 
       word read should return SMB_RDATA[15:8]=I2C_MSB and SMB_RDATA[7:0]=I2C_LSB. If 
       reading of I2C using byte read, the SMB_RDATA[15:8]=don`t care; 
       SMB_RDATA[7:0]=read_byte.  If we have a SMB slave connected on the bus, reading 
       of the SMBus slave using word read should return SMB_RDATA[15:8]=SMB_LSB and 
       SMB_RDATA[7:0]=SMB_MSB.  If the software is not sure whether the target is I2C 
       or SMBus slave, please use byte access. 
     */
    UINT32 smb_wdata : 16;
    /* smb_wdata - Bits[31:16], RWS, default = 16'b0000000000000000 
       Write Data: Holds data to be written by SPDW commands.  Since TSOD/EEPROM are 
       I2C devices and the byte order is MSByte first in a word write, writing of I2C 
       using word write should use SMB_WDATA[15:8]=I2C_MSB and SMB_WDATA[7:0]=I2C_LSB. 
       If writing of I2C using byte write, the SMB_WDATA[15:8]=don`t care; 
       SMB_WDATA[7:0]=write_byte.  If we have a SMB slave connected on the bus, writing 
       of the SMBus slave using word write should use SMB_WDATA[15:8]=SMB_LSB and 
       SMB_WDATA[7:0]=SMB_MSB.  It is software responsibility to figure out the byte 
       order of the slave access. 
     */
  } Bits;
  UINT32 Data;
} SMB_DATA_CFG_2_PCU_FUN5_STRUCT;


/* SMB_TSOD_POLL_RATE_CNTR_CFG_0_PCU_FUN5_REG supported on:                   */
/*       SKX_A0 (0x401F50C0)                                                  */
/*       SKX (0x401F50C0)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_TSOD_POLL_RATE_CNTR_CFG_0_PCU_FUN5_REG 0x040540C0
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * SMBus Poll rate counter
 */
typedef union {
  struct {
    UINT32 smb_tsod_poll_rate_cntr : 18;
    /* smb_tsod_poll_rate_cntr - Bits[17:0], RW_LV, default = 18'b000000000000000000 
       TSOD poll rate counter. When the counter matches SMB_TSOD_POLL_RATE , the 
       counter is reset to zero. This counter increments every 500ns as configured by 
       CNFG_500_NANOSEC. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TSOD_POLL_RATE_CNTR_CFG_0_PCU_FUN5_STRUCT;


/* SMB_TSOD_POLL_RATE_CNTR_CFG_1_PCU_FUN5_REG supported on:                   */
/*       SKX_A0 (0x401F50C4)                                                  */
/*       SKX (0x401F50C4)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_TSOD_POLL_RATE_CNTR_CFG_1_PCU_FUN5_REG 0x040540C4
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * SMBus Poll rate counter
 */
typedef union {
  struct {
    UINT32 smb_tsod_poll_rate_cntr : 18;
    /* smb_tsod_poll_rate_cntr - Bits[17:0], RW_LV, default = 18'b000000000000000000 
       TSOD poll rate counter. When the counter matches SMB_TSOD_POLL_RATE , the 
       counter is reset to zero. This counter increments every 500ns as configured by 
       CNFG_500_NANOSEC. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TSOD_POLL_RATE_CNTR_CFG_1_PCU_FUN5_STRUCT;


/* SMB_TSOD_POLL_RATE_CNTR_CFG_2_PCU_FUN5_REG supported on:                   */
/*       SKX_A0 (0x401F50C8)                                                  */
/*       SKX (0x401F50C8)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_TSOD_POLL_RATE_CNTR_CFG_2_PCU_FUN5_REG 0x040540C8
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * SMBus Poll rate counter
 */
typedef union {
  struct {
    UINT32 smb_tsod_poll_rate_cntr : 18;
    /* smb_tsod_poll_rate_cntr - Bits[17:0], RW_LV, default = 18'b000000000000000000 
       TSOD poll rate counter. When the counter matches SMB_TSOD_POLL_RATE , the 
       counter is reset to zero. This counter increments every 500ns as configured by 
       CNFG_500_NANOSEC. 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TSOD_POLL_RATE_CNTR_CFG_2_PCU_FUN5_STRUCT;


/* SMB_TLOW_TIMEOUT_CNTR_CFG_0_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F50CC)                                                  */
/*       SKX (0x401F50CC)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_TLOW_TIMEOUT_CNTR_CFG_0_PCU_FUN5_REG 0x040540CC
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * SMBus timeout counter
 */
typedef union {
  struct {
    UINT32 smb_tlow_timeout_cntr : 24;
    /* smb_tlow_timeout_cntr - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       When Error recovery is enabled, the SMBus Time-out Timer Counter runs when BIOS 
       sets SMB_CMD_CFG.SMB_CKOVRD to 0. When SMB_TLOW_TIMEOUT_CFG.SMB_TLOW_TIME_OUT 
       matches the upper 16 bits of this 24-bit counter, SMB_CMD_CFG.SMB_CKOVRD is 
       reset back to 1 by H/W. Counter increments each FXCLK.  
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TLOW_TIMEOUT_CNTR_CFG_0_PCU_FUN5_STRUCT;


/* SMB_TLOW_TIMEOUT_CNTR_CFG_1_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F50D0)                                                  */
/*       SKX (0x401F50D0)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_TLOW_TIMEOUT_CNTR_CFG_1_PCU_FUN5_REG 0x040540D0
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * SMBus timeout counter
 */
typedef union {
  struct {
    UINT32 smb_tlow_timeout_cntr : 24;
    /* smb_tlow_timeout_cntr - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       When Error recovery is enabled, the SMBus Time-out Timer Counter runs when BIOS 
       sets SMB_CMD_CFG.SMB_CKOVRD to 0. When SMB_TLOW_TIMEOUT_CFG.SMB_TLOW_TIME_OUT 
       matches the upper 16 bits of this 24-bit counter, SMB_CMD_CFG.SMB_CKOVRD is 
       reset back to 1 by H/W. Counter increments each FXCLK.  
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TLOW_TIMEOUT_CNTR_CFG_1_PCU_FUN5_STRUCT;


/* SMB_TLOW_TIMEOUT_CNTR_CFG_2_PCU_FUN5_REG supported on:                     */
/*       SKX_A0 (0x401F50D4)                                                  */
/*       SKX (0x401F50D4)                                                     */
/* Register default value:              0x00000000                            */
#define SMB_TLOW_TIMEOUT_CNTR_CFG_2_PCU_FUN5_REG 0x040540D4
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * SMBus timeout counter
 */
typedef union {
  struct {
    UINT32 smb_tlow_timeout_cntr : 24;
    /* smb_tlow_timeout_cntr - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       When Error recovery is enabled, the SMBus Time-out Timer Counter runs when BIOS 
       sets SMB_CMD_CFG.SMB_CKOVRD to 0. When SMB_TLOW_TIMEOUT_CFG.SMB_TLOW_TIME_OUT 
       matches the upper 16 bits of this 24-bit counter, SMB_CMD_CFG.SMB_CKOVRD is 
       reset back to 1 by H/W. Counter increments each FXCLK.  
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TLOW_TIMEOUT_CNTR_CFG_2_PCU_FUN5_STRUCT;


/* MH_SENSE_500NS_PCU_FUN5_REG supported on:                                  */
/*       SKX_A0 (0x401F50D8)                                                  */
/*       SKX (0x401F50D8)                                                     */
/* Register default value:              0x00C84032                            */
#define MH_SENSE_500NS_PCU_FUN5_REG 0x040540D8
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * Input sense assertion and period configuration.
 */
typedef union {
  struct {
    UINT32 cnfg_500_nanosec : 10;
    /* cnfg_500_nanosec - Bits[9:0], RWS_LB, default = 10'b0000110010 
       500ns equivalent in FXCLK. BIOS calculate number of FXCLK to be equivalent to 
       500 nanoseconds. CNTR_500_NANOSEC will count up to this value, then reset to 
       zero. For pre-Si validation, minimum 2 can be set to speed up the simulation.  
       The recommended CNFG_500_NANOSEC value based on 100MHz FXCLK frequency is 50. 
     */
    UINT32 rsvd_10 : 3;
    /* rsvd_10 - Bits[12:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mh_in_sense_assert : 3;
    /* mh_in_sense_assert - Bits[15:13], RW, default = 3'b010 
       MEMHOT Input Sense Assertion Time in number of CNTR_500_NANOSEC. BIOS calculate 
       number of CNFG_500_NANOSEC for 1usec/2usec input_sense duration. This field 
       determines the duty cycle between sensing and asserting in the following manner: 
       mh_io_cntr <= mh_in_sense_assert: sense modemh_io_cntr > mh_in_sense_assert: 
       output mode.  Here is MH_IN_SENSE_ASSERT ranges:  0 or 1 Reserved, 2 - 7 1usec - 
       3.5usec sense assertion time in 500nsec increment. 
     */
    UINT32 mh_sense_period : 10;
    /* mh_sense_period - Bits[25:16], RW, default = 10'b0011001000 
       MEMHOT Input Sense Period in number of CNTR_500_NANOSEC. Determines how long a 
       complete memhot sense and output cycle is.  Must  be larger than 
       mh_in_sense_assert. 
     */
    UINT32 rsvd_26 : 6;
    /* rsvd_26 - Bits[31:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MH_SENSE_500NS_PCU_FUN5_STRUCT;


/* MH_IO_500NS_CNTR_PCU_FUN5_REG supported on:                                */
/*       SKX_A0 (0x401F50DC)                                                  */
/*       SKX (0x401F50DC)                                                     */
/* Register default value:              0x00000000                            */
#define MH_IO_500NS_CNTR_PCU_FUN5_REG 0x040540DC
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * IO counter configuration
 */
typedef union {
  struct {
    UINT32 cntr_500_nanosec : 10;
    /* cntr_500_nanosec - Bits[9:0], RW_LV, default = 10'b0000000000 
       500ns base counters used for the MEM_HOT counters and the SMBus counters. BIOS 
       calculate number of FXCLK to be equvalent to 500 nanoseconds. CNTR_500_NANOSEC 
       (hardware will increment this register by 1 every CNTR_500_NANOSEC). The counter 
       is  reset to zero when it matches CNFG_500_NANOSEC-1.  
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mh0_io_cntr : 10;
    /* mh0_io_cntr - Bits[21:12], RW_LV, default = 10'b0000000000 
       MEM_HOT[1:0]# Input Output Counter in number of CNTR_500_NANOSEC. When 
       MH_IO_CNTR is zero, the counter is loaded with MH_SENSE_PERIOD in the next 
       CNTR_500_NANOSEC. When count is greater than MH_IN_SENSE_ASSERT, the 
       MEM_HOT[1:0]# output driver may be turn on if the corresponding MEM_HOT#event is 
       asserted. The receiver is turned off during this time. When count is equal or 
       less than MH_IN_SENSE_ASSERT, MEM_HOT[1:0]# output is disabled and receiver is 
       turned on.  
     */
    UINT32 mh1_io_cntr : 10;
    /* mh1_io_cntr - Bits[31:22], RW_LV, default = 10'b0000000000 
       MEM_HOT[1:0]# Input Output Counter in number of CNTR_500_NANOSEC. When 
       MH1_IO_CNTR is zero, the counter is loaded with MH_SENSE_PERIOD in the next 
       CNTR_500_NANOSEC. When count is greater than MH_IN_SENSE_ASSERT, the MEM_HOT[1]# 
       output driver may be turn on if the corresponding MEM_HOT#event is asserted. The 
       receiver is turned off during this time. When count is equal or less than 
       MH_IN_SENSE_ASSERT, MEM_HOT[1:0]# output is disabled and receiver is turned on.  
     */
  } Bits;
  UINT32 Data;
} MH_IO_500NS_CNTR_PCU_FUN5_STRUCT;


/* MH_EXT_STAT_PCU_FUN5_REG supported on:                                     */
/*       SKX_A0 (0x401F50E0)                                                  */
/*       SKX (0x401F50E0)                                                     */
/* Register default value:              0x00000000                            */
#define MH_EXT_STAT_PCU_FUN5_REG 0x040540E0
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * Capture externally asserted MEM_HOT[1:0]# assertion detection.
 */
typedef union {
  struct {
    UINT32 mh_ext_stat_0 : 1;
    /* mh_ext_stat_0 - Bits[0:0], RO_V, default = 1'b0 
       MEM_HOT[0]# assertion status at this sense period.  Set if MEM_HOT[0]# is 
       asserted externally for this sense period, this running status bit will 
       automatically updated with the next sensed value in the next MEM_HOT input sense 
       phase. 
     */
    UINT32 mh_ext_stat_1 : 1;
    /* mh_ext_stat_1 - Bits[1:1], RO_V, default = 1'b0 
       MEM_HOT[1]# assertion status at this sense period.  Set if MEM_HOT[1]# is 
       asserted externally for this sense period, this running status bit will 
       automatically updated with the next sensed value in the next MEM_HOT input sense 
       phase. 
     */
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MH_EXT_STAT_PCU_FUN5_STRUCT;




/* MH_MAINCTL_PCU_FUN5_REG supported on:                                      */
/*       SKX_A0 (0x401F50E8)                                                  */
/*       SKX (0x401F50E8)                                                     */
/* Register default value:              0x00008000                            */
#define MH_MAINCTL_PCU_FUN5_REG 0x040540E8
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * MEMHOT control configuration
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 15;
    /* rsvd_0 - Bits[14:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mh_output_en : 1;
    /* mh_output_en - Bits[15:15], RW_LB, default = 1'b1 
       MEM_HOT output generation logic enable control.  When 0, the MEM_HOT output 
       generation logic is disabled, i.e. MEM_HOT[1:0]# outputs are in de-asserted 
       state, no assertion regardless of the memory temperature. Sensing of externally 
       asserted MEM_HOT[1:0]# is not affected by this bit. iMC will always reset the 
       MH1_DIMM_VAL and MH0_DIMM_VAL bits in the next DCLK so there is no impact to the 
       PCODE update to the MH_TEMP_STAT registers.  When 1, the MEM_HOT output 
       generation logic is enabled. 
     */
    UINT32 mh_sense_en : 1;
    /* mh_sense_en - Bits[16:16], RW_LB, default = 1'b0 
       Externally asserted MEM_HOTsense control enable bit.  When set, the MEM_HOT 
       sense logic is enabled. 
     */
    UINT32 mhot_smi_en : 1;
    /* mhot_smi_en - Bits[17:17], RW, default = 1'b0 
       Generate SMI during internal MEMHOT# event assertion.  When set, the MEM_HOT 
       sense logic is enabled. 
     */
    UINT32 mhot_ext_smi_en : 1;
    /* mhot_ext_smi_en - Bits[18:18], RW, default = 1'b0 
       Generate SMI event when either MEMHOT[1:0]# is externally asserted.  When set, 
       the MEM_HOT sense logic is enabled. 
     */
    UINT32 rsvd_19 : 13;
    /* rsvd_19 - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MH_MAINCTL_PCU_FUN5_STRUCT;


/* LBITMAP_PCU_FUN5_REG supported on:                                         */
/*       SKX_A0 (0x401F50EC)                                                  */
/*       SKX (0x401F50EC)                                                     */
/* Register default value:              0x00000007                            */
#define LBITMAP_PCU_FUN5_REG 0x040540EC
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register defines a position of the bitmap in the global error status. 
 */
typedef union {
  struct {
    UINT32 bit_select : 5;
    /* bit_select - Bits[4:0], RW, default = 5'b00111 
       selects which bit to set in the one-hot payload of the Global Error Message that 
       PCU sends to UBOX.  
     */
    UINT32 rsvd : 27;
    /* rsvd - Bits[31:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LBITMAP_PCU_FUN5_STRUCT;


/* VPPCTL_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x401F50F0)                                                  */
/*       SKX (0x401F50F0)                                                     */
/* Register default value:              0x00000000                            */
#define VPPCTL_PCU_FUN5_REG 0x040540F0
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register defines the control/command for PCA9555.
 */
typedef union {
  struct {
    UINT32 vpp_reset_mode : 1;
    /* vpp_reset_mode - Bits[0:0], RWS_LB, default = 1'b0 
       0: Power good reset will reset the VPP state machines and warm reset will cause 
       the VPP state machine to terminate at the next 'logical' VPP stream boundary and 
       then reset the VPP state machines  
       1: Both power good and warm reset will reset the VPP state machines
       
     */
    UINT32 vpp_error_severity : 2;
    /* vpp_error_severity - Bits[2:1], RWS_LB, default = 2'b0 
       
       	  0: Fatal
       	  1: Non Fatal
       	  2: Correctable
       
     */
    UINT32 vpp_mc_enable : 1;
    /* vpp_mc_enable - Bits[3:3], RW_LB, default = 1'b0 
       Message channel mastering enable. This bit enables sending slot status messages 
       to IIO and error messages UBOX.  
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VPPCTL_PCU_FUN5_STRUCT;


/* VPPSTS_PCU_FUN5_REG supported on:                                          */
/*       SKX_A0 (0x401F50F4)                                                  */
/*       SKX (0x401F50F4)                                                     */
/* Register default value:              0x00000000                            */
#define VPPSTS_PCU_FUN5_REG 0x040540F4
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register defines the status from PCA9555.
 */
typedef union {
  struct {
    UINT32 vpp_error : 1;
    /* vpp_error - Bits[0:0], RW1CS, default = 1'b0 
       VPP Port error happened i.e. an unexpected STOP or NACK was seen on the VPP 
       port. 
     */
    UINT32 vpp_paused : 1;
    /* vpp_paused - Bits[1:1], RO_V, default = 1'b0 
       When set, indicates that the VPP port has been paused by pCode at a SMBus 
       transaction boundary.  
     */
    UINT32 vpp_all_banks_serviced : 1;
    /* vpp_all_banks_serviced - Bits[2:2], RO_V, default = 1'b0 
       When set, indicates that the VPP port has sent a read and write command for 
       every enabled port/bank at least once since the last reset. If this bit is set 
       while a new SLOTCTL/SLOTSTS bank becomes enabled, then this bit might clear 
       until the new bank has also been serviced. 
       Note:
       If VPP_Paused is set, then the VPP port makes no progress toward setting this 
       bit until the VPP port is unpaused. If VPP is paused on entry to S3, it may 
       never be unpaused until power down occurs.  
     */
    UINT32 reserved : 29;
    /* reserved - Bits[31:3], RO, default = 29'b0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} VPPSTS_PCU_FUN5_STRUCT;


/* VPPFREQ_PCU_FUN5_REG supported on:                                         */
/*       SKX_A0 (0x401F50F8)                                                  */
/*       SKX (0x401F50F8)                                                     */
/* Register default value:              0x061E01F4                            */
#define VPPFREQ_PCU_FUN5_REG 0x040540F8
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * This register defines the status from PCA9555.
 */
typedef union {
  struct {
    UINT32 vpp_tsu_thd : 12;
    /* vpp_tsu_thd - Bits[11:0], RWS_LB, default = 12'h1F4 
       VPP Tsu and Thd
       Represents the high time and low time of the SCL pin. It should be set to 5uS 
       for a 100kHz SCL clock (5uS high time and 5uS low time). The default value 
       represents 5uS with an internal clock of 100MHz. 
       
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[15:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 vpp_thd_data : 8;
    /* vpp_thd_data - Bits[23:16], RWS_LB, default = 8'h1E 
       VPP Thd Data (Hold Time on Data)
       Hold time for Data is 300nS. The default value is set to 300nS when the internal 
       clock rate is 100MHz 
               
     */
    UINT32 vpp_tpf : 8;
    /* vpp_tpf - Bits[31:24], RWS_LB, default = 8'h6 
       VPP Tpf (Pulse Filter Time)
       Pulse Filter should be set to 60nS. The value used is dependent on the internal 
       clock frequency. In this case, internal clock frequency is 500MHz, so the 
       default value represents 60nS at that rate. 
               
     */
  } Bits;
  UINT32 Data;
} VPPFREQ_PCU_FUN5_STRUCT;


/* VPP_INVERTS_PCU_FUN5_REG supported on:                                     */
/*       SKX_A0 (0x401F50FC)                                                  */
/*       SKX (0x401F50FC)                                                     */
/* Register default value:              0x00000000                            */
#define VPP_INVERTS_PCU_FUN5_REG 0x040540FC
/* Struct format extracted from XML file SKX\1.30.5.CFG.xml.
 * VPP invertions.
 */
typedef union {
  struct {
    UINT32 dfr_inv_pwren : 1;
    /* dfr_inv_pwren - Bits[0:0], RWS_LB, default = 1'b0 
       Invert the power enable signal
     */
    UINT32 dfr_inv_emil : 1;
    /* dfr_inv_emil - Bits[1:1], RWS_LB, default = 1'b0 
       Invert the EMIL pin
     */
    UINT32 dfr_inv_mrl : 1;
    /* dfr_inv_mrl - Bits[2:2], RWS_LB, default = 1'b0 
       Invert the mrl
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VPP_INVERTS_PCU_FUN5_STRUCT;


#endif /* PCU_FUN5_h */
