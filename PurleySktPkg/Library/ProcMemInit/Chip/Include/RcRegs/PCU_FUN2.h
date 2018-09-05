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

#ifndef PCU_FUN2_h
#define PCU_FUN2_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* PCU_FUN2_DEV 30                                                            */
/* PCU_FUN2_FUN 2                                                             */

/* VID_PCU_FUN2_REG supported on:                                             */
/*       SKX_A0 (0x201F2000)                                                  */
/*       SKX (0x201F2000)                                                     */
/* Register default value:              0x8086                                */
#define VID_PCU_FUN2_REG 0x04022000
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} VID_PCU_FUN2_STRUCT;


/* DID_PCU_FUN2_REG supported on:                                             */
/*       SKX_A0 (0x201F2002)                                                  */
/*       SKX (0x201F2002)                                                     */
/* Register default value:              0x2082                                */
#define DID_PCU_FUN2_REG 0x04022002
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2082 
        
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
} DID_PCU_FUN2_STRUCT;


/* PCICMD_PCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x201F2004)                                                  */
/*       SKX (0x201F2004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_PCU_FUN2_REG 0x04022004
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} PCICMD_PCU_FUN2_STRUCT;


/* PCISTS_PCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x201F2006)                                                  */
/*       SKX (0x201F2006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_PCU_FUN2_REG 0x04022006
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} PCISTS_PCU_FUN2_STRUCT;


/* RID_PCU_FUN2_REG supported on:                                             */
/*       SKX_A0 (0x101F2008)                                                  */
/*       SKX (0x101F2008)                                                     */
/* Register default value:              0x00                                  */
#define RID_PCU_FUN2_REG 0x04021008
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} RID_PCU_FUN2_STRUCT;


/* CCR_N0_PCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101F2009)                                                  */
/*       SKX (0x101F2009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_PCU_FUN2_REG 0x04021009
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_PCU_FUN2_STRUCT;


/* CCR_N1_PCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x201F200A)                                                  */
/*       SKX (0x201F200A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_PCU_FUN2_REG 0x0402200A
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} CCR_N1_PCU_FUN2_STRUCT;


/* CLSR_PCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x101F200C)                                                  */
/*       SKX (0x101F200C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_PCU_FUN2_REG 0x0402100C
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} CLSR_PCU_FUN2_STRUCT;


/* PLAT_PCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x101F200D)                                                  */
/*       SKX (0x101F200D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_PCU_FUN2_REG 0x0402100D
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} PLAT_PCU_FUN2_STRUCT;


/* HDR_PCU_FUN2_REG supported on:                                             */
/*       SKX_A0 (0x101F200E)                                                  */
/*       SKX (0x101F200E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_PCU_FUN2_REG 0x0402100E
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} HDR_PCU_FUN2_STRUCT;


/* BIST_PCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x101F200F)                                                  */
/*       SKX (0x101F200F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_PCU_FUN2_REG 0x0402100F
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} BIST_PCU_FUN2_STRUCT;


/* SVID_PCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x201F202C)                                                  */
/*       SKX (0x201F202C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_PCU_FUN2_REG 0x0402202C
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} SVID_PCU_FUN2_STRUCT;


/* SDID_PCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x201F202E)                                                  */
/*       SKX (0x201F202E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_PCU_FUN2_REG 0x0402202E
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} SDID_PCU_FUN2_STRUCT;


/* CAPPTR_PCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101F2034)                                                  */
/*       SKX (0x101F2034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_PCU_FUN2_REG 0x04021034
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} CAPPTR_PCU_FUN2_STRUCT;


/* INTL_PCU_FUN2_REG supported on:                                            */
/*       SKX_A0 (0x101F203C)                                                  */
/*       SKX (0x101F203C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_PCU_FUN2_REG 0x0402103C
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} INTL_PCU_FUN2_STRUCT;


/* INTPIN_PCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101F203D)                                                  */
/*       SKX (0x101F203D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_PCU_FUN2_REG 0x0402103D
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} INTPIN_PCU_FUN2_STRUCT;


/* MINGNT_PCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101F203E)                                                  */
/*       SKX (0x101F203E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_PCU_FUN2_REG 0x0402103E
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} MINGNT_PCU_FUN2_STRUCT;


/* MAXLAT_PCU_FUN2_REG supported on:                                          */
/*       SKX_A0 (0x101F203F)                                                  */
/*       SKX (0x101F203F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_PCU_FUN2_REG 0x0402103F
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
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
} MAXLAT_PCU_FUN2_STRUCT;


/* DRAM_ENERGY_STATUS_PCU_FUN2_REG supported on:                              */
/*       SKX_A0 (0x401F207C)                                                  */
/*       SKX (0x401F207C)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_ENERGY_STATUS_PCU_FUN2_REG 0x0402407C
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * DRAM energy consumed by all the DIMMS in all the Channels.  The counter will 
 * wrap around and continue counting when it reaches its limit.   
 *   ENERGY_UNIT for DRAM domain is 15.3uJ.
 *   The data is updated by PCODE and is Read Only for all SW.  
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Energy Value
     */
  } Bits;
  UINT32 Data;
} DRAM_ENERGY_STATUS_PCU_FUN2_STRUCT;


/* CORE_FIVR_ERR_LOG_PCU_FUN2_REG supported on:                               */
/*       SKX_A0 (0x401F2080)                                                  */
/*       SKX (0x401F2080)                                                     */
/* Register default value:              0x00000000                            */
#define CORE_FIVR_ERR_LOG_PCU_FUN2_REG 0x04024080
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * Reports Core FIVR Faults - Bits correspond to cores which have a core IVR fault. 
 * Cores with a core IVR fault will not come out of reset. This field has the same 
 * field mapping as the CSR RESOLVED_CORES and results are reported in CSR 
 * RESOLVED_CORES.  
 */
typedef union {
  struct {
    UINT32 fault_vector : 32;
    /* fault_vector - Bits[31:0], ROS_V, default = 32'b000000 
       Fault vector
     */
  } Bits;
  UINT32 Data;
} CORE_FIVR_ERR_LOG_PCU_FUN2_STRUCT;


/* UNCORE_FIVR_ERR_LOG_PCU_FUN2_REG supported on:                             */
/*       SKX_A0 (0x401F2084)                                                  */
/*       SKX (0x401F2084)                                                     */
/* Register default value:              0x00000000                            */
#define UNCORE_FIVR_ERR_LOG_PCU_FUN2_REG 0x04024084
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * Reports Uncore FIVR Faults - If any bit is set in 31:0 an uncore FIVR fault has 
 * occured, and firmware should FRB the socket. An IERR will also occur on this 
 * condition.   
 */
typedef union {
  struct {
    UINT32 fault_vector : 32;
    /* fault_vector - Bits[31:0], ROS_V, default = 32'b000000 
       Fault vector
     */
  } Bits;
  UINT32 Data;
} UNCORE_FIVR_ERR_LOG_PCU_FUN2_STRUCT;


/* PACKAGE_RAPL_PERF_STATUS_PCU_FUN2_REG supported on:                        */
/*       SKX_A0 (0x401F2088)                                                  */
/*       SKX (0x401F2088)                                                     */
/* Register default value:              0x00000000                            */
#define PACKAGE_RAPL_PERF_STATUS_PCU_FUN2_REG 0x04024088
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is used by Pcode to report Package Power limit violations in the 
 * Platform PBM. 
 * 
 * Provides information on the performance impact of the RAPL power limit. 
 * Provides the total time for which PACKAGE was throttled because of the RAPL 
 * power limit. Throttling here is defined as going below O.S requested P-state.  
 * Total time = Reg value * 1s *1 /2^(time unit)
 * Usage model: Enables the O.S/Driver to learn about PACKAGE throttling as a 
 * result of RAPL limit. 
 * Can be used by other S/W components that control the PACKAGE power.
 *  This register is mapped as an MSR with the name Package_Perf_Status
 */
typedef union {
  struct {
    UINT32 pwr_limit_throttle_ctr : 32;
    /* pwr_limit_throttle_ctr - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Reports the number of times the Power limiting algorithm had to clip the power 
       limit due to hitting the lowest power state available. 
       
       Accumulated PACKAGE throttled time
     */
  } Bits;
  UINT32 Data;
} PACKAGE_RAPL_PERF_STATUS_PCU_FUN2_STRUCT;


/* PKG_CST_ENTRY_CRITERIA_MASK_PCU_FUN2_REG supported on:                     */
/*       SKX_A0 (0x401F208C)                                                  */
/*       SKX (0x401F208C)                                                     */
/* Register default value:              0x00000000                            */
#define PKG_CST_ENTRY_CRITERIA_MASK_PCU_FUN2_REG 0x0402408C
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is used to configure which events will be used as a gate for PC3 
 * entry.  Expectation is that BIOS will write this register based on the system 
 * config and devices in the system. It is expected that disabled Intel UPI/PCIe 
 * links must report L1. 
 */
typedef union {
  struct {
    UINT32 pcie_in_l1 : 24;
    /* pcie_in_l1 - Bits[23:0], RWS, default = 24'b000000000000000000000000 
       PCIE port in L1
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PKG_CST_ENTRY_CRITERIA_MASK_PCU_FUN2_STRUCT;


/* PKG_CST_ENTRY_CRITERIA_MASK_CFG2_PCU_FUN2_REG supported on:                */
/*       SKX_A0 (0x401F2090)                                                  */
/*       SKX (0x401F2090)                                                     */
/* Register default value:              0x00000007                            */
#define PKG_CST_ENTRY_CRITERIA_MASK_CFG2_PCU_FUN2_REG 0x04024090
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is used to configure which events will be used as a gate for PC3 
 * entry.  Expectation is that IOS will write this register based on the system 
 * config and devices in the system. It is expected that disabled Intel UPI/PCIe 
 * links must report L1. 
 */
typedef union {
  struct {
    UINT32 kti_in_l1 : 3;
    /* kti_in_l1 - Bits[2:0], RWS, default = 3'b111 
       When set to 1, Intel UPI must be in L1
     */
    UINT32 rsvd_3 : 17;
    /* rsvd_3 - Bits[19:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dram_in_sr : 2;
    /* dram_in_sr - Bits[21:20], RWS, default = 2'b00 
       When set to 1, DRAM must be in SR.
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PKG_CST_ENTRY_CRITERIA_MASK_CFG2_PCU_FUN2_STRUCT;


/* DTS_CONFIG1_PCU_FUN2_REG supported on:                                     */
/*       SKX_A0 (0x401F2094)                                                  */
/*       SKX (0x401F2094)                                                     */
/* Register default value:              0x00000000                            */
#define DTS_CONFIG1_PCU_FUN2_REG 0x04024094
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * Location of correctionfactor F and scale factor C for 
 * DTS2.0
 *       bscherka: IVT HSD i1112231 DTS2.0 added this csr
 *                isteiner: Moved DTS registers for reconvergence
 */
typedef union {
  struct {
    UINT32 f : 13;
    /* f - Bits[12:0], RW_L, default = 13'b0000000000000 
       Correction Factor F
     */
    UINT32 rsvd_13 : 3;
    UINT32 c : 13;
    /* c - Bits[28:16], RW_L, default = 13'b0000000000000 
       Scale factor C
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DTS_CONFIG1_PCU_FUN2_STRUCT;


/* DTS_CONFIG2_PCU_FUN2_REG supported on:                                     */
/*       SKX_A0 (0x401F2098)                                                  */
/*       SKX (0x401F2098)                                                     */
/* Register default value:              0x00000000                            */
#define DTS_CONFIG2_PCU_FUN2_REG 0x04024098
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * Location of time constant coefficients for DTS2.0 
 * inplementation.
 *       bscherka: IVT HSD i1112231 added this csr
 *                isteiner: Moved DTS registers for reconvergence
 */
typedef union {
  struct {
    UINT32 af : 13;
    /* af - Bits[12:0], RW_L, default = 13'b0000000000000 
       Time constant coefficient AF
     */
    UINT32 rsvd_13 : 3;
    UINT32 as : 13;
    /* as - Bits[28:16], RW_L, default = 13'b0000000000000 
       Time constant coefficient AS
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DTS_CONFIG2_PCU_FUN2_STRUCT;




/* GLOBAL_PKG_C_S_CONTROL_REGISTER_PCU_FUN2_REG supported on:                 */
/*       SKX_A0 (0x401F20A0)                                                  */
/*       SKX (0x401F20A0)                                                     */
/* Register default value:              0x00000000                            */
#define GLOBAL_PKG_C_S_CONTROL_REGISTER_PCU_FUN2_REG 0x040240A0
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is in the PCU CR space.  It contains information pertinent to the 
 * master slave IPC protocol and global enable/disable for PK CST and SST.  
 * Expectation is that BIOS will write this register during the Reset/Init flow. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    UINT32 am_i_master : 1;
    /* am_i_master - Bits[2:2], RWS, default = 1'b0 
       If set, socket is master.  Master socket will be the leady socket.  BIOS will 
       set this bit in the legacy socket. 
     */
    UINT32 rsvd_3 : 5;
    UINT32 my_nid : 3;
    /* my_nid - Bits[10:8], RWS, default = 3'b000 
       NID of this socket.
     */
    UINT32 rsvd_11 : 1;
    UINT32 master_nid : 3;
    /* master_nid - Bits[14:12], RWS, default = 3'b0 
       Master socket NID.  Can also be determined from the Socket0 entry in the NID MAP 
       register. 
     */
    UINT32 rsvd_15 : 17;
  } Bits;
  UINT32 Data;
} GLOBAL_PKG_C_S_CONTROL_REGISTER_PCU_FUN2_STRUCT;


/* GLOBAL_NID_SOCKET_0_TO_3_MAP_PCU_FUN2_REG supported on:                    */
/*       SKX_A0 (0x401F20A4)                                                  */
/*       SKX (0x401F20A4)                                                     */
/* Register default value:              0x00000000                            */
#define GLOBAL_NID_SOCKET_0_TO_3_MAP_PCU_FUN2_REG 0x040240A4
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is in the PCU CR space.  It contains NID information for sockets 
 * 0-3 in the platform.  Expectation is that BIOS will write this register during 
 * the Reset/Init flow. 
 */
typedef union {
  struct {
    UINT32 skt0_nid : 3;
    /* skt0_nid - Bits[2:0], RW, default = 3'b000 
       Socket0 NID
     */
    UINT32 rsvd_3 : 1;
    UINT32 skt1_nid : 3;
    /* skt1_nid - Bits[6:4], RW, default = 3'b000 
       Socket1 NID
     */
    UINT32 rsvd_7 : 1;
    UINT32 skt2_nid : 3;
    /* skt2_nid - Bits[10:8], RW, default = 3'b000 
       Socket2 NID
     */
    UINT32 rsvd_11 : 1;
    UINT32 skt3_nid : 3;
    /* skt3_nid - Bits[14:12], RW, default = 3'b000 
       Socket3 NID
     */
    UINT32 rsvd_15 : 13;
    UINT32 skt_valid : 4;
    /* skt_valid - Bits[31:28], RW, default = 4'b0000 
       Valid bits indicating whether NID has been programmed by BIOS.  If bit is 0 
       after the CST/SST ready bit is set, then it implies that the socket is not 
       populated. 
     */
  } Bits;
  UINT32 Data;
} GLOBAL_NID_SOCKET_0_TO_3_MAP_PCU_FUN2_STRUCT;


/* DRAM_POWER_INFO_N0_PCU_FUN2_REG supported on:                              */
/*       SKX_A0 (0x401F20A8)                                                  */
/*       SKX (0x401F20A8)                                                     */
/* Register default value:              0x00780118                            */
#define DRAM_POWER_INFO_N0_PCU_FUN2_REG 0x040240A8
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 */
typedef union {
  struct {
    UINT32 dram_tdp : 15;
    /* dram_tdp - Bits[14:0], RW_L, default = 15'b000000100011000 
       The Spec power allowed for DRAM.  The TDP setting is typical (not guaranteed).
         
         
         The units for this value are defined in 
       DRAM_POWER_INFO_UNIT_MSR[PWR_UNIT].
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dram_min_pwr : 15;
    /* dram_min_pwr - Bits[30:16], RW_L, default = 15'b000000001111000 
       The minimal power setting allowed for DRAM.  Lower values will be clamped to 
       this value.  The minimum setting is typical (not guaranteed). 
         The units for this value are defined in DRAM_POWER_INFO_UNIT_MSR[PWR_UNIT].
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DRAM_POWER_INFO_N0_PCU_FUN2_STRUCT;


/* DRAM_POWER_INFO_N1_PCU_FUN2_REG supported on:                              */
/*       SKX_A0 (0x401F20AC)                                                  */
/*       SKX (0x401F20AC)                                                     */
/* Register default value:              0x00280258                            */
#define DRAM_POWER_INFO_N1_PCU_FUN2_REG 0x040240AC
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 */
typedef union {
  struct {
    UINT32 dram_max_pwr : 15;
    /* dram_max_pwr - Bits[14:0], RW_L, default = 15'b000001001011000 
       The maximal power setting allowed for DRAM.  Higher values will be clamped to 
       this value.  The maximum setting is typical (not guaranteed). 
         The units for this value are defined in DRAM_POWER_INFO_UNIT_MSR[PWR_UNIT].
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 dram_max_win : 7;
    /* dram_max_win - Bits[22:16], RW_L, default = 7'b0101000 
       The maximal time window allowed for the DRAM.  Higher 
       values will be clamped to this value.
         
         x = PKG_MAX_WIN[54:53]
         y = PKG_MAX_WIN[52:48]
         
         The timing interval window is Floating Point number given by 1.x * 
       power(2,y).
         
         The unit of measurement is defined in 
       DRAM_POWER_INFO_UNIT_MSR[TIME_UNIT].
     */
    UINT32 rsvd_23 : 8;
    /* rsvd_23 - Bits[30:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lock : 1;
    /* lock - Bits[31:31], RW_KL, default = 1'b0 
       Lock bit to lock the Register
     */
  } Bits;
  UINT32 Data;
} DRAM_POWER_INFO_N1_PCU_FUN2_STRUCT;


/* PROCHOT_RESPONSE_RATIO_PCU_FUN2_REG supported on:                          */
/*       SKX_A0 (0x401F20B0)                                                  */
/*       SKX (0x401F20B0)                                                     */
/* Register default value:              0x00000000                            */
#define PROCHOT_RESPONSE_RATIO_PCU_FUN2_REG 0x040240B0
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * Controls the CPU response to an inbound platform assertion of xxPROCHOT# or PMAX 
 * detector by capping to the programmed ratio. 
 */
typedef union {
  struct {
    UINT32 prochot_ratio : 8;
    /* prochot_ratio - Bits[7:0], RW_L, default = 8'b0 
       Controls the CPU response to an inbound platform assertion of xxPROCHOT# by 
       capping to the programmed ratio.  
       Default value is Pn. 
       The min allowed ratio is defined by PLATFORM_INFO[MIN_OPERATING_RATIO]
       Locked by FIRM_CONFIG.PROCHOT_LOCK
               
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PROCHOT_RESPONSE_RATIO_PCU_FUN2_STRUCT;


/* DRAM_RAPL_PERF_STATUS_PCU_FUN2_REG supported on:                           */
/*       SKX_A0 (0x401F20D8)                                                  */
/*       SKX (0x401F20D8)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RAPL_PERF_STATUS_PCU_FUN2_REG 0x040240D8
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is used by Pcode to report DRAM Plane Power limit violations in 
 * the Platform PBM. 
 * 
 * Provides information on the performance impact of the RAPL power limit. 
 * Provides the total time for which DRAM was throttled because of the RAPL power 
 * limit. Total time = Reg Value *1 s * 1/ (2^time_unit) 
 * Usage model: Enables the OS/Driver to learn about DRAM throttling as a result of 
 * RAPL limit. 
 * Can be used by other S/W components that control the DRAM power.
 * This register is mapped as an MSR with the name DRAM_RAPL_PERF_STATUS.
 */
typedef union {
  struct {
    UINT32 pwr_limit_throttle_ctr : 32;
    /* pwr_limit_throttle_ctr - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       Reports the number of times the Power limiting algorithm 
       had to clip the power limit due to hitting the lowest power state available.
       
       Accumulated DRAM throttled time
     */
  } Bits;
  UINT32 Data;
} DRAM_RAPL_PERF_STATUS_PCU_FUN2_STRUCT;


/* DYNAMIC_PERF_POWER_CTL_PCU_FUN2_REG supported on:                          */
/*       SKX_A0 (0x401F20DC)                                                  */
/*       SKX (0x401F20DC)                                                     */
/* Register default value:              0x00010000                            */
#define DYNAMIC_PERF_POWER_CTL_PCU_FUN2_REG 0x040240DC
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register effectively governs all major power saving 
 * engines and hueristics on the die.
 */
typedef union {
  struct {
    UINT32 kti_apm_override_enable : 1;
    /* kti_apm_override_enable - Bits[0:0], RW_V, default = 1'b0 
       0 disable over ride
       1 enable over ride
     */
    UINT32 rsvd_1 : 4;
    /* rsvd_1 - Bits[4:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 iom_apm_override_enable : 1;
    /* iom_apm_override_enable - Bits[5:5], RW_V, default = 1'b0 
       0 disable over ride
       1 enable over ride
     */
    UINT32 rsvd_6 : 4;
    /* rsvd_6 - Bits[9:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 imc_apm_override_enable : 1;
    /* imc_apm_override_enable - Bits[10:10], RW_V, default = 1'b0 
       0 disable over ride
       1 enable over ride
     */
    UINT32 rsvd_11 : 4;
    /* rsvd_11 - Bits[14:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 io_bw_plimit_override_enable : 1;
    /* io_bw_plimit_override_enable - Bits[15:15], RW_V, default = 1'b0 
       0 disable over ride
       1 enable over ride
     */
    UINT32 allow_peci_pcode_error_rsp : 1;
    /* allow_peci_pcode_error_rsp - Bits[16:16], RWS_V, default = 1'b1 
       allow pcode to attempt to respond to peci requests after a pcode MCA
     */
    UINT32 rsvd_17 : 3;
    UINT32 uncore_perf_plimit_override_enable : 1;
    /* uncore_perf_plimit_override_enable - Bits[20:20], RW_V, default = 1'b0 
       0 disable over ride
       1 enable over ride
     */
    UINT32 rsvd_21 : 1;
    UINT32 turbo_demotion_override_enable : 1;
    /* turbo_demotion_override_enable - Bits[22:22], RW_V, default = 1'b0 
       0 - Disable override
       1- Enable override
     */
    UINT32 cst_demotion_override_enable : 1;
    /* cst_demotion_override_enable - Bits[23:23], RW_V, default = 1'b0 
       0 - Disable override
       1- Enable override
     */
    UINT32 i_turbo_override_enable : 1;
    /* i_turbo_override_enable - Bits[24:24], RW_V, default = 1'b0 
       0 - Disable override
       1- Enable override
     */
    UINT32 eep_l_override_enable : 1;
    /* eep_l_override_enable - Bits[25:25], RW_V, default = 1'b0 
       0 disable over ride
       1 enable over ride
     */
    UINT32 eep_l_override : 4;
    /* eep_l_override - Bits[29:26], RW_V, default = 4'b0000 
       This indicates a EEP L override. Value 0-15
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], RW_V, default = 2'b00 
       Reserved
     */
  } Bits;
  UINT32 Data;
} DYNAMIC_PERF_POWER_CTL_PCU_FUN2_STRUCT;




/* PERF_P_LIMIT_CONTROL_PCU_FUN2_REG supported on:                            */
/*       SKX_A0 (0x401F20E4)                                                  */
/*       SKX (0x401F20E4)                                                     */
/* Register default value:              0x7D000000                            */
#define PERF_P_LIMIT_CONTROL_PCU_FUN2_REG 0x040240E4
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is BIOS configurable.  Dual mapping will prevent additional fast 
 * path events or polling needs from PCODE.  HW does not use the CSR input, it is 
 * primarily used by PCODE.  Note that PERF_P_LIMIT_CLIP must be nominally 
 * configured to guaranteed frequency + 1, if turbo related actions are needed in 
 * slave sockets. 
 */
typedef union {
  struct {
    UINT32 perf_plimit_enable : 1;
    /* perf_plimit_enable - Bits[0:0], RW_V, default = 1'b0 
       Enable Performance P-limit feature
     */
    UINT32 perf_plimit_threshold : 5;
    /* perf_plimit_threshold - Bits[5:1], RW_V, default = 5'b00000 
       Uncore frequency threshold above which this socket will trigger the feature and 
       start trying to raise frequency of other sockets. 
     */
    UINT32 rsvd_6 : 1;
    UINT32 perf_plimit_clip : 5;
    /* perf_plimit_clip - Bits[11:7], RW_V, default = 5'b00000 
       Maximum value the floor is allowed to be set to for perf P-limit.
     */
    UINT32 rsvd_12 : 3;
    UINT32 perf_plimit_differential : 3;
    /* perf_plimit_differential - Bits[17:15], RW_V, default = 3'b000 
       Parameter used to tune how far below local socket frequency remote socket 
       frequency is allowed to be. Also impacts rate at which frequency drops when 
       feature disengages. 
     */
    UINT32 rsvd_18 : 14;
  } Bits;
  UINT32 Data;
} PERF_P_LIMIT_CONTROL_PCU_FUN2_STRUCT;


/* IO_BANDWIDTH_P_LIMIT_CONTROL_PCU_FUN2_REG supported on:                    */
/*       SKX_A0 (0x401F20E8)                                                  */
/*       SKX (0x401F20E8)                                                     */
/* Register default value:              0x00018AB4                            */
#define IO_BANDWIDTH_P_LIMIT_CONTROL_PCU_FUN2_REG 0x040240E8
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * Various controls
 */
typedef union {
  struct {
    UINT32 io_floor_1 : 3;
    /* io_floor_1 - Bits[2:0], RWS_V, default = 3'b100 
       IO Floor 1
     */
    UINT32 io_thresh_1 : 3;
    /* io_thresh_1 - Bits[5:3], RWS_V, default = 3'b110 
       IO Threshold 1
     */
    UINT32 io_floor_2 : 3;
    /* io_floor_2 - Bits[8:6], RWS_V, default = 3'b010 
       IO Floor 2
     */
    UINT32 io_thresh_2 : 3;
    /* io_thresh_2 - Bits[11:9], RWS_V, default = 3'b101 
       IO threshold 2
     */
    UINT32 io_floor_3 : 3;
    /* io_floor_3 - Bits[14:12], RWS_V, default = 3'b000 
       IO floor 3
     */
    UINT32 io_thresh_3 : 3;
    /* io_thresh_3 - Bits[17:15], RWS_V, default = 3'b011 
       IO Threshold 3
     */
    UINT32 kti_floor_1 : 3;
    /* kti_floor_1 - Bits[20:18], RWS_V, default = 3'b000 
       Intel UPI Floor 1
     */
    UINT32 kti_thresh_1 : 3;
    /* kti_thresh_1 - Bits[23:21], RWS_V, default = 3'b000 
       Intel UPI Threshold 1
     */
    UINT32 kti_floor_2 : 3;
    /* kti_floor_2 - Bits[26:24], RWS_V, default = 3'b000 
       Intel UPI Floor 2
     */
    UINT32 kti_thresh_2 : 3;
    /* kti_thresh_2 - Bits[29:27], RWS_V, default = 3'b000 
       Intel UPI Threshold 2
     */
    UINT32 rsvd_30 : 1;
    UINT32 ovrd_enable : 1;
    /* ovrd_enable - Bits[31:31], RW_V, default = 1'b0 
       IO_BW_PLIMIT Override Bit.
       0 - Disable
       1 - Enable
     */
  } Bits;
  UINT32 Data;
} IO_BANDWIDTH_P_LIMIT_CONTROL_PCU_FUN2_STRUCT;


/* MCA_ERR_SRC_LOG_PCU_FUN2_REG supported on:                                 */
/*       SKX_A0 (0x401F20EC)                                                  */
/*       SKX (0x401F20EC)                                                     */
/* Register default value:              0x00000000                            */
#define MCA_ERR_SRC_LOG_PCU_FUN2_REG 0x040240EC
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * MCSourceLog is used by the PCU to log the error sources. This register is 
 * initialized to zeroes during reset. The PCU will set the relevant bits when the 
 * condition they represent appears. The PCU never clears the registers-the UBox or 
 * off-die entities should clear them when they are consumed, unless their 
 * processing involves taking down the platform. 
 */
typedef union {
  struct {
    UINT32 first_core_ierr : 5;
    /* first_core_ierr - Bits[4:0], RWS, default = 5'b00000 
       Reserved. SKX b302820.
     */
    UINT32 rsvd_5 : 13;
    /* rsvd_5 - Bits[17:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 msmi_mcerr_internal : 1;
    /* msmi_mcerr_internal - Bits[18:18], RWS_V, default = 1'b0 
       Internal error: This socket asserted MSMI_MCERR.
     */
    UINT32 msmi_ierr_internal : 1;
    /* msmi_ierr_internal - Bits[19:19], RWS_V, default = 1'b0 
       Internal error: This socket asserted MSMI_IERR.
     */
    UINT32 msmi_internal : 1;
    /* msmi_internal - Bits[20:20], RWS_V, default = 1'b0 
       Internal error: This socket asserted a MSMI#. This is OR (bit 19,18).
     */
    UINT32 msmi_mcerr : 1;
    /* msmi_mcerr - Bits[21:21], RWS_V, default = 1'b0 
       External error: The package observed MSMI_MCERR.
     */
    UINT32 msmi_ierr : 1;
    /* msmi_ierr - Bits[22:22], RWS_V, default = 1'b0 
       External error: The package observed MSMI_IERR.
     */
    UINT32 msmi : 1;
    /* msmi - Bits[23:23], RWS_V, default = 1'b0 
       External error: The package observed MSMI# (for any reason). 
       It is or(bit 22, bit21); functions as a Valid bit for the other two package 
       conditions. It has no effect when a local core is associated with the error. 
     */
    UINT32 rsvd_24 : 2;
    /* rsvd_24 - Bits[25:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mcerr_internal : 1;
    /* mcerr_internal - Bits[26:26], RWS_V, default = 1'b0 
       Internal error: This socket asserted MCERR.
     */
    UINT32 ierr_internal : 1;
    /* ierr_internal - Bits[27:27], RWS_V, default = 1'b0 
       Internal error: This socket asserted IERR.
     */
    UINT32 caterr_internal : 1;
    /* caterr_internal - Bits[28:28], RWS_V, default = 1'b0 
       Internal error: This socket asserted a CATERR#. This is OR (bit 27,26).
     */
    UINT32 mcerr : 1;
    /* mcerr - Bits[29:29], RWS_V, default = 1'b0 
       External error: The package observed MCERR.
     */
    UINT32 ierr : 1;
    /* ierr - Bits[30:30], RWS_V, default = 1'b0 
       External error: The package observed IERR.
     */
    UINT32 caterr : 1;
    /* caterr - Bits[31:31], RWS_V, default = 1'b0 
       External error: The package observed CATERR# (for any reason). 
       It is or(bit 30, bit29); functions as a Valid bit for the other two package 
       conditions. It has no effect when a local core is associated with the error. 
     */
  } Bits;
  UINT32 Data;
} MCA_ERR_SRC_LOG_PCU_FUN2_STRUCT;


/* DRAM_PLANE_POWER_LIMIT_N0_PCU_FUN2_REG supported on:                       */
/*       SKX_A0 (0x401F20F0)                                                  */
/*       SKX (0x401F20F0)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_PLANE_POWER_LIMIT_N0_PCU_FUN2_REG 0x040240F0
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is used by BIOS/OS/Integrated Graphics Driver/CPM Driver to limit 
 * the power budget of DRAM Plane. 
 *   
 *   The overall package turbo power limitation is controlled by 
 * DRAM_PLANE_POWER_LIMIT. 
 */
typedef union {
  struct {
    UINT32 dram_pp_pwr_lim : 15;
    /* dram_pp_pwr_lim - Bits[14:0], RW_L, default = 15'b000000000000000 
       This is the power limitation on the IA cores power plane.
         
         The unit of measurement is defined in DRAM_POWER_INFO_UNIT_MSR[PWR_UNIT].
     */
    UINT32 pwr_lim_ctrl_en : 1;
    /* pwr_lim_ctrl_en - Bits[15:15], RW_L, default = 1'b0 
       This bit must be set in order to limit the power of the DRAM power plane.
         
         0b     DRAM power plane power limitation is disabled
         1b     DRAM power plane power limitation is enabled
     */
    UINT32 reserved : 1;
    /* reserved - Bits[16:16], RO, default = 1'b0 
       Reserved
     */
    UINT32 ctrl_time_win : 7;
    /* ctrl_time_win - Bits[23:17], RW_L, default = 7'b0000000 
       x = CTRL_TIME_WIN[23:22]
         y = CTRL_TIME_WIN[21:17]
         
         The timing interval window is Floating Point number given by 1.x * power(2,y).
         
         The unit of measurement is defined in PACKAGE_POWER_SKU_UNIT_MSR[TIME_UNIT].
         
         The maximal time window is bounded by PACKAGE_POWER_SKU_MSR[PKG_MAX_WIN].  The 
       minimum time window is 1 unit of measurement (as defined above). 
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[30:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pp_pwr_lim_lock : 1;
    /* pp_pwr_lim_lock - Bits[31:31], RW_KL, default = 1'b0 
       When set, all settings in this register are locked and are treated as Read Only.
     */
  } Bits;
  UINT32 Data;
} DRAM_PLANE_POWER_LIMIT_N0_PCU_FUN2_STRUCT;




/* THERMTRIP_CONFIG_PCU_FUN2_REG supported on:                                */
/*       SKX_A0 (0x401F20F8)                                                  */
/*       SKX (0x401F20F8)                                                     */
/* Register default value:              0x00000000                            */
#define THERMTRIP_CONFIG_PCU_FUN2_REG 0x040240F8
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register is used to configure whether the Thermtrip signal only carries the 
 * processor Trip information, or does it carry the Mem trip information as well. 
 * The register will be used by HW to enable ORing of the memtrip info into the 
 * thermtrip OR tree. 
 */
typedef union {
  struct {
    UINT32 en_memtrip : 1;
    /* en_memtrip - Bits[0:0], RW_LB, default = 1'b0 
       If set to 1, PCU will OR in the MEMtrip information into the ThermTrip OR Tree
       If set to 0, PCU will ignore the MEMtrip information and ThermTrip will just 
       have the processor indication.  
       
       Expect BIOS to Enable this in Phase4
     */
    UINT32 rsvd_1 : 3;
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} THERMTRIP_CONFIG_PCU_FUN2_STRUCT;


/* PMONPCODEFILTER_PCU_FUN2_REG supported on:                                 */
/*       SKX_A0 (0x401F20FC)                                                  */
/*       SKX (0x401F20FC)                                                     */
/* Register default value:              0xFFFFFFFF                            */
#define PMONPCODEFILTER_PCU_FUN2_REG 0x040240FC
/* Struct format extracted from XML file SKX\1.30.2.CFG.xml.
 * This register has three mappings depending on the type of Perfmon Events that 
 * are being counted.  
 * This register is read by Pcode and communicates the Masking information from the 
 * BIOS/SW to Pcode.  
 */
typedef union {
  struct {
    UINT32 filter : 32;
    /* filter - Bits[31:0], RW_V, default = 32'b11111111111111111111111111111111 
       Pcode makes the decision on how to interpret the 32-bit field
       Interpretation2:
       15:0 ThreadID
       Interpretation1:
       7:0 CoreID
       
       Interpretation0:
       31:24 Voltage/Frequency Range 3
       23:16 Voltage/Frequency Range 2
       15:8 Voltage/Frequency Range 1
       7:0 Voltage/Frequency Range 0
     */
  } Bits;
  UINT32 Data;
} PMONPCODEFILTER_PCU_FUN2_STRUCT;


#endif /* PCU_FUN2_h */
