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

#ifndef PCU_FUN1_h
#define PCU_FUN1_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* PCU_FUN1_DEV 30                                                            */
/* PCU_FUN1_FUN 1                                                             */

/* VID_PCU_FUN1_REG supported on:                                             */
/*       SKX_A0 (0x201F1000)                                                  */
/*       SKX (0x201F1000)                                                     */
/* Register default value:              0x8086                                */
#define VID_PCU_FUN1_REG 0x04012000
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} VID_PCU_FUN1_STRUCT;


/* DID_PCU_FUN1_REG supported on:                                             */
/*       SKX_A0 (0x201F1002)                                                  */
/*       SKX (0x201F1002)                                                     */
/* Register default value:              0x2081                                */
#define DID_PCU_FUN1_REG 0x04012002
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2081 
        
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
} DID_PCU_FUN1_STRUCT;


/* PCICMD_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x201F1004)                                                  */
/*       SKX (0x201F1004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_PCU_FUN1_REG 0x04012004
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} PCICMD_PCU_FUN1_STRUCT;


/* PCISTS_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x201F1006)                                                  */
/*       SKX (0x201F1006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_PCU_FUN1_REG 0x04012006
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} PCISTS_PCU_FUN1_STRUCT;


/* RID_PCU_FUN1_REG supported on:                                             */
/*       SKX_A0 (0x101F1008)                                                  */
/*       SKX (0x101F1008)                                                     */
/* Register default value:              0x00                                  */
#define RID_PCU_FUN1_REG 0x04011008
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} RID_PCU_FUN1_STRUCT;


/* CCR_N0_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x101F1009)                                                  */
/*       SKX (0x101F1009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_PCU_FUN1_REG 0x04011009
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_PCU_FUN1_STRUCT;


/* CCR_N1_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x201F100A)                                                  */
/*       SKX (0x201F100A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_PCU_FUN1_REG 0x0401200A
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} CCR_N1_PCU_FUN1_STRUCT;


/* CLSR_PCU_FUN1_REG supported on:                                            */
/*       SKX_A0 (0x101F100C)                                                  */
/*       SKX (0x101F100C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_PCU_FUN1_REG 0x0401100C
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} CLSR_PCU_FUN1_STRUCT;


/* PLAT_PCU_FUN1_REG supported on:                                            */
/*       SKX_A0 (0x101F100D)                                                  */
/*       SKX (0x101F100D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_PCU_FUN1_REG 0x0401100D
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} PLAT_PCU_FUN1_STRUCT;


/* HDR_PCU_FUN1_REG supported on:                                             */
/*       SKX_A0 (0x101F100E)                                                  */
/*       SKX (0x101F100E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_PCU_FUN1_REG 0x0401100E
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} HDR_PCU_FUN1_STRUCT;


/* BIST_PCU_FUN1_REG supported on:                                            */
/*       SKX_A0 (0x101F100F)                                                  */
/*       SKX (0x101F100F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_PCU_FUN1_REG 0x0401100F
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} BIST_PCU_FUN1_STRUCT;


/* SVID_PCU_FUN1_REG supported on:                                            */
/*       SKX_A0 (0x201F102C)                                                  */
/*       SKX (0x201F102C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_PCU_FUN1_REG 0x0401202C
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} SVID_PCU_FUN1_STRUCT;


/* SDID_PCU_FUN1_REG supported on:                                            */
/*       SKX_A0 (0x201F102E)                                                  */
/*       SKX (0x201F102E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_PCU_FUN1_REG 0x0401202E
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} SDID_PCU_FUN1_STRUCT;


/* CAPPTR_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x101F1034)                                                  */
/*       SKX (0x101F1034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_PCU_FUN1_REG 0x04011034
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} CAPPTR_PCU_FUN1_STRUCT;


/* INTL_PCU_FUN1_REG supported on:                                            */
/*       SKX_A0 (0x101F103C)                                                  */
/*       SKX (0x101F103C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_PCU_FUN1_REG 0x0401103C
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} INTL_PCU_FUN1_STRUCT;


/* INTPIN_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x101F103D)                                                  */
/*       SKX (0x101F103D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_PCU_FUN1_REG 0x0401103D
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} INTPIN_PCU_FUN1_STRUCT;


/* MINGNT_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x101F103E)                                                  */
/*       SKX (0x101F103E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_PCU_FUN1_REG 0x0401103E
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} MINGNT_PCU_FUN1_STRUCT;


/* MAXLAT_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x101F103F)                                                  */
/*       SKX (0x101F103F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_PCU_FUN1_REG 0x0401103F
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
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
} MAXLAT_PCU_FUN1_STRUCT;










/* BIOS_MAILBOX_DATA_PCU_FUN1_REG supported on:                               */
/*       SKX_A0 (0x401F108C)                                                  */
/*       SKX (0x401F108C)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_MAILBOX_DATA_PCU_FUN1_REG 0x0401408C
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * Data register for the BIOS-to-PCODE mailbox.  This mailbox is implemented as a 
 * means for accessing statistics and implementing PCODE patches. 
 * This register is used in conjunction with BIOS_MAILBOX_INTERFACE.
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW_V, default = 32'b00000000000000000000000000000000 
       This field contains the data associated with specific commands.
     */
  } Bits;
  UINT32 Data;
} BIOS_MAILBOX_DATA_PCU_FUN1_STRUCT;


/* BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG supported on:                          */
/*       SKX_A0 (0x401F1090)                                                  */
/*       SKX (0x401F1090)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG 0x04014090
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * Control and Status register for the BIOS-to-PCODE mailbox.  This mailbox is 
 * implemented as a means for accessing statistics and implementing PCODE patches. 
 * This register is used in conjunction with BIOS_MAILBOX_DATA.
 */
typedef union {
  struct {
    UINT32 command : 8;
    /* command - Bits[7:0], RW_V, default = 8'b00000000 
       This field contains the SW request command or the PCODE response code, depending 
       on the setting of RUN_BUSY. 
     */
    UINT32 addr : 21;
    /* addr - Bits[28:8], RW_V, default = 21'b000000000000000000000 
       This field contains the address associated with specific commands.
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[30:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 run_busy : 1;
    /* run_busy - Bits[31:31], RW1S, default = 1'b0 
       SW may write to the two mailbox registers only when RUN_BUSY is cleared (0b).  
       Setting RUN_BUSY to 1b will create a Fast Path event.  After setting this bit, 
       SW will poll this bit until it is cleared. 
       PCODE will clear RUN_BUSY after updating the mailbox registers with the result 
       and error code. 
     */
  } Bits;
  UINT32 Data;
} BIOS_MAILBOX_INTERFACE_PCU_FUN1_STRUCT;


/* BIOS_RESET_CPL_PCU_FUN1_REG supported on:                                  */
/*       SKX_A0 (0x401F1094)                                                  */
/*       SKX (0x401F1094)                                                     */
/* Register default value:              0x00000000                            */
#define BIOS_RESET_CPL_PCU_FUN1_REG 0x04014094
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * This register is used as interface between BIOS and PCODE.  It is written by 
 * BIOS and read by PCODE. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 rst_cpl1 : 1;
    /* rst_cpl1 - Bits[1:1], RW1S, default = 1'b0 
       BIOS sets this bit when BIOS has completed NID MAP and C-state/Sstate 
       configurations at package level. Setting this bit allows the processore to 
       receive a Warm-RESET. 
              0 = Not Ready
              1 = NID MAP, package C-state and S-state BIOS configurations are 
       completed.   
     */
    UINT32 rst_cpl2 : 1;
    /* rst_cpl2 - Bits[2:2], RW1S, default = 1'b0 
       RST_CPL2 - Used to Facilitate handshake between BIOS and Pcode.
              Periodic RCOMP Start - Pcode starts issuing periodic RCOMPs from this 
       point forward. 
              BIOS set this bit when BIOS has completed the memory configurations and 
       indicates the processor to start issuing periodic RCOMPs from this point 
       forward. 
              0 = Not Ready
              1 = Ready for Periodic RCOMP Starts
     */
    UINT32 rst_cpl3 : 1;
    /* rst_cpl3 - Bits[3:3], RW1S, default = 1'b0 
       Used to Facilitate handshake between BIOS and Pcode Node ID Configuration is 
       Complete - allows pcode to get ready to receive a Reset Warn; No power mgmt 
       features running at all till this point. 
              BIOS set this bit when BIOS has completed all the power/thermal 
       management configurations platform wide (including core/package/DRAM PBM, EDP, 
       etc) and ready for the processor to perform base on the given parameters. 
              0 = Not Ready
              1 = BIOS Power and Thermal Management Configure Completed
              Once this bit is set, the VR12_WORKAROUND_ENABLE mailbox command becomes 
       invalid. 
     */
    UINT32 rst_cpl4 : 1;
    /* rst_cpl4 - Bits[4:4], RW1S, default = 1'b0 
       Used to Facilitate handshake between BIOS and Pcode.
              BIOS set this bit when BIOS has completed the DRAM parameter calibration 
       and is ready for the processor to start the DRAM power meter using the 
       coefficients. Once this bit is set, the BIOS mailbox will be locked until next 
       reset. 
              0 = Not Ready
              1 = Memory Calibration Completed
              DRAM PBM and DRAM power meter features will not work until this bit is 
       set. 
     */
    UINT32 rsvd_5 : 4;
    UINT32 pcode_init_done1 : 1;
    /* pcode_init_done1 - Bits[9:9], RO_FW, default = 1'b0 
       Used to Facilitate handshake between Pcode and BIOS.
              This bit is set as an acknowledgement of bit 1 in this register.
              0 = Not Ready
              1 = Ready 
     */
    UINT32 pcode_init_done2 : 1;
    /* pcode_init_done2 - Bits[10:10], RO_FW, default = 1'b0 
       Used to Facilitate handshake between Pcode and BIOS.
              This bit is set as an acknowledgement of bit 2 in this register.
              0 = Not Ready
              1 = Ready	
     */
    UINT32 pcode_init_done3 : 1;
    /* pcode_init_done3 - Bits[11:11], RO_FW, default = 1'b0 
       Used to Facilitate handshake between Pcode and BIOS.
              This bit is set as an acknowledgement of bit 3 in this register.
              0 = Not Ready
              1 = Ready	
     */
    UINT32 pcode_init_done4 : 1;
    /* pcode_init_done4 - Bits[12:12], RO_FW, default = 1'b0 
       Used to Facilitate handshake between Pcode and BIOS.
              This bit is set as an acknowledgement of bit 4 in this register.
              0 = Not Ready
              1 = Ready	
     */
    UINT32 rsvd_13 : 19;
  } Bits;
  UINT32 Data;
} BIOS_RESET_CPL_PCU_FUN1_STRUCT;


/* MC_BIOS_REQ_PCU_FUN1_REG supported on:                                     */
/*       SKX_A0 (0x401F1098)                                                  */
/*       SKX (0x401F1098)                                                     */
/* Register default value:              0x00000000                            */
#define MC_BIOS_REQ_PCU_FUN1_REG 0x04014098
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * This register allows BIOS to request Memory Controller clock frequency.
 */
typedef union {
  struct {
    UINT32 req_data : 6;
    /* req_data - Bits[5:0], RWS, default = 6'b000000 
       These 6 bits are the data for the request.  The only possible request type is MC 
       frequency request.  The encoding of this field is the 133 MHz multiplier for 
       QCLK: 
       Binary    Dec       QCLK Equation        QCLK Freq  
       0000b     0d        -------- MC PLL - shutdown ----------
       ...
       0011b     6d        6 * 133.33 MHz        800.00 MHz
       0100b     8d        8 * 133.33 MHz       1066.67 MHz
       0101b     10d      10 * 133.33 MHz       1333.33 MHz
       0110b     12d      12 * 133.33 MHz       1600.00 MHz
       0111b     14d      14 * 133.33 MHz       1866.67 MHz
       1000b     16d      16 * 133.33 MHz       2133.33 MHz
       ...
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 req_type : 4;
    /* req_type - Bits[11:8], RWS, default = 4'b0000 
       Request Type:
       0h     MC frequency request for 133 Mhz
       1h     MC frequency request for 100 Mhz
       All other values are reserved.
     */
    UINT32 rsvd_12 : 19;
    /* rsvd_12 - Bits[30:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 run_busy : 1;
    /* run_busy - Bits[31:31], RWS, default = 1'b0 
       This bit indicates that the BIOS request is pending.  BIOS sets this bit 
       together with a command in the lower bits of this register.  Firmware may only 
       clear this bit after the BIOS request has been served or observed. 
     */
  } Bits;
  UINT32 Data;
} MC_BIOS_REQ_PCU_FUN1_STRUCT;




/* CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG supported on:                          */
/*       SKX_A0 (0x401F10A0)                                                  */
/*       SKX (0x401F10A0)                                                     */
/* Register default value:              0x00000000                            */
#define CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG 0x040140A0
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * Single bits that were part of CSR_DESIRED_CORES_CFG broken out to this register 
 * to keep CSR_DESIRED_CORES_CFG to have a core mask only. 
 */
typedef union {
  struct {
    UINT32 rsvd : 30;
    /* rsvd - Bits[29:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 smt_disable : 1;
    /* smt_disable - Bits[30:30], RWS_L, default = 1'b0 
       Disable simultaneous multithreading in all cores if this bit is set to '1'.
     */
    UINT32 lock : 1;
    /* lock - Bits[31:31], RWS_KL, default = 1'b0 
       Lock:  once written to a '1', changes to this register cannot be done. Cleared 
       only by a power-on reset 
     */
  } Bits;
  UINT32 Data;
} CSR_DESIRED_CORES_CFG2_PCU_FUN1_STRUCT;


/* CSR_DESIRED_CORES_PCU_FUN1_REG supported on:                               */
/*       SKX_A0 (0x401F10A4)                                                  */
/*       SKX (0x401F10A4)                                                     */
/* Register default value:              0x00000000                            */
#define CSR_DESIRED_CORES_PCU_FUN1_REG 0x040140A4
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * Number of cores/threads BIOS wants to exist on the next reset.  A processor 
 * reset must be used for this register to take effect.  Note, programming this 
 * register to a value higher than the product has cores should not be done. This 
 * register is reset only by PWRGOOD. 
 */
typedef union {
  struct {
    UINT32 core_off_mask : 28;
    /* core_off_mask - Bits[27:0], RWS_L, default = 28'b0000000000000000000000000000 
       BIOS will set this bit to request that the matching core should not be activated 
       coming out of reset.  The default value of this registers means that all cores 
       are enabled.  Restrictions: At least one core needs to be left active.  
       Otherwise, FW will ignore the setting altogether. 
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CSR_DESIRED_CORES_PCU_FUN1_STRUCT;






/* SAPMCTL_PCU_FUN1_REG supported on:                                         */
/*       SKX_A0 (0x401F10B0)                                                  */
/*       SKX (0x401F10B0)                                                     */
/* Register default value:              0x38FF0100                            */
#define SAPMCTL_PCU_FUN1_REG 0x040140B0
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * PCODE will sample this register at the end of Phase 4.
 */
typedef union {
  struct {
    UINT32 iio0_pkgc_clock_gate_disable : 1;
    /* iio0_pkgc_clock_gate_disable - Bits[0:0], RW_L, default = 1'b0 
       If set to 1, disables clock gating of IIO during package-c.
     */
    UINT32 iio1_pkgc_clock_gate_disable : 1;
    /* iio1_pkgc_clock_gate_disable - Bits[1:1], RW_L, default = 1'b0 
       If set to 1, disables clock gating of IIO during package-c.
     */
    UINT32 iio2_pkgc_clock_gate_disable : 1;
    /* iio2_pkgc_clock_gate_disable - Bits[2:2], RW_L, default = 1'b0 
       If set to 1, disables clock gating of IIO during package-c.
     */
    UINT32 kti01_pkgc_clock_gate_disable : 1;
    /* kti01_pkgc_clock_gate_disable - Bits[3:3], RW_L, default = 1'b0 
       If set to 1, disables clock gating of Intel UPI 0/1 during package-c.
     */
    UINT32 kti23_pkgc_clock_gate_disable : 1;
    /* kti23_pkgc_clock_gate_disable - Bits[4:4], RW_L, default = 1'b0 
       If set to 1, disables clock gating of Intel UPI 2/3 during package-c.
     */
    UINT32 rsvd_5 : 1;
    UINT32 mc0_pkgc_clk_gate_disable : 1;
    /* mc0_pkgc_clk_gate_disable - Bits[6:6], RW_L, default = 1'b0 
       If set to 1,  tells pcode to disallow clock gating in MC0 when in the PKG-C6 
       state 
     */
    UINT32 mc1_pkgc_clk_gate_disable : 1;
    /* mc1_pkgc_clk_gate_disable - Bits[7:7], RW_L, default = 1'b0 
       If set to 1, tells pcode to disallow clock gating in MC1 when in the PKG-C6 
       state 
     */
    UINT32 rsvd_8 : 22;
    UINT32 setvid_decay_disable : 1;
    /* setvid_decay_disable - Bits[30:30], RW_L, default = 1'b0 
       This bit is used by BIOS to disable SETVID Decay to enable use of VR12 designs 
       that do not support decay function.  
       0 - Enable Decay (Default)
       1 - Disable Decay
     */
    UINT32 lock : 1;
    /* lock - Bits[31:31], RW_KL, default = 1'b0 
       When set to 1b this bit locks various PM registers.
     */
  } Bits;
  UINT32 Data;
} SAPMCTL_PCU_FUN1_STRUCT;




/* M_COMP_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x401F10B8)                                                  */
/*       SKX (0x401F10B8)                                                     */
/* Register default value:              0x0000001A                            */
#define M_COMP_PCU_FUN1_REG 0x040140B8
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * Memory COMP control
 */
typedef union {
  struct {
    UINT32 comp_disable : 1;
    /* comp_disable - Bits[0:0], RW, default = 1'b0 
       Disable periodic COMP cycles
       0b     Enabled
       1b     Disabled
     */
    UINT32 comp_interval : 4;
    /* comp_interval - Bits[4:1], RW, default = 4'b1101 
       This field indicates the period of RCOMP.  The time is indicated by 
       power(2,COMP_INTERVAL) * 10.24 usec. 
       The default value of Dh corresponds to ~84 ms.
     */
    UINT32 rsvd_5 : 3;
    /* rsvd_5 - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 comp_force : 1;
    /* comp_force - Bits[8:8], RW1S, default = 1'b0 
       Writing '1' to this field triggers a COMP cycle.  HW will reset this bit when 
       the COMP cycle starts. 
     */
    UINT32 rsvd_9 : 23;
    /* rsvd_9 - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} M_COMP_PCU_FUN1_STRUCT;








/* SSKPD_N0_PCU_FUN1_REG supported on:                                        */
/*       SKX_A0 (0x401F10D0)                                                  */
/*       SKX (0x401F10D0)                                                     */
/* Register default value:              0x00000000                            */
#define SSKPD_N0_PCU_FUN1_REG 0x040140D0
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * This register holds 64 writable bits with no functionality behind them. It is 
 * for the convenience of BIOS. 
 */
typedef union {
  struct {
    UINT32 skpd : 32;
    /* skpd - Bits[31:0], RWS, default = 64'b0000000000000000000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       4 WORDs of data storage.
     */
  } Bits;
  UINT32 Data;
} SSKPD_N0_PCU_FUN1_STRUCT;


/* SSKPD_N1_PCU_FUN1_REG supported on:                                        */
/*       SKX_A0 (0x401F10D4)                                                  */
/*       SKX (0x401F10D4)                                                     */
/* Register default value:              0x00000000                            */
#define SSKPD_N1_PCU_FUN1_REG 0x040140D4
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * This register holds 64 writable bits with no functionality behind them. It is 
 * for the convenience of BIOS. 
 */
typedef union {
  struct {
    UINT32 skpd : 32;
    /* skpd - Bits[31:0], RWS, default = 64'b0000000000000000000000000000000000000000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       4 WORDs of data storage.
     */
  } Bits;
  UINT32 Data;
} SSKPD_N1_PCU_FUN1_STRUCT;


/* C2_DDR_TT_PCU_FUN1_REG supported on:                                       */
/*       SKX_A0 (0x401F10D8)                                                  */
/*       SKX (0x401F10D8)                                                     */
/* Register default value:              0x000001F4                            */
#define C2_DDR_TT_PCU_FUN1_REG 0x040140D8
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * This register contains the initial DDR timer value.  BIOS can update this value 
 * during run-time.   
 * PCODE will sample this register at slow loop.  If the value has changed since 
 * the previous sample and in addition there is no valid Hystereris parameter (HYS) 
 * from a previous PM_DMD or PM_RSP message, then PCODE will configure 
 * DDR_TRANS_TIMER_VALUE with this value. 
 */
typedef union {
  struct {
    UINT32 ddr_timer_value : 13;
    /* ddr_timer_value - Bits[12:0], RW, default = 13'b0000111110100 
       DDR Transition Pop Down Value in 10ns increments
     */
    UINT32 rsvd : 19;
    /* rsvd - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} C2_DDR_TT_PCU_FUN1_STRUCT;


/* C2C3TT_PCU_FUN1_REG supported on:                                          */
/*       SKX_A0 (0x401F10DC)                                                  */
/*       SKX (0x401F10DC)                                                     */
/* Register default value:              0x00000032                            */
#define C2C3TT_PCU_FUN1_REG 0x040140DC
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * This register contains the initial snoop timer (pop-down) value.  BIOS can 
 * update this value during run-time.   
 * PCODE will sample this register at slow loop.  If the value has changed since 
 * the previous sample and in addition there is no valid Hystereris parameter (HYS) 
 * from a previous PM_DMD or PM_RSP message, then PCODE will configure 
 * IMPH_CR_SNP_RELOAD[LIM] with this value. 
 */
typedef union {
  struct {
    UINT32 ppdn_init : 12;
    /* ppdn_init - Bits[11:0], RW, default = 12'b000000110010 
       Value
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} C2C3TT_PCU_FUN1_STRUCT;














/* PCIE_ILTR_OVRD_PCU_FUN1_REG supported on:                                  */
/*       SKX_A0 (0x401F10FC)                                                  */
/*       SKX (0x401F10FC)                                                     */
/* Register default value:              0x00000000                            */
#define PCIE_ILTR_OVRD_PCU_FUN1_REG 0x040140FC
/* Struct format extracted from XML file SKX\1.30.1.CFG.xml.
 * This register includes parameters that PCODE will use to override information 
 * received from PCI Express via LTR messages. 
 * PCODE will sample this register at slow loop.
 */
typedef union {
  struct {
    UINT32 nstl : 10;
    /* nstl - Bits[9:0], RW, default = 10'b0000000000 
       Latency requirement for Non-Snoop requests.  This value is multiplied by the 
       MULTIPLIER field to yield a time value, yielding an expressible range from 1ns 
       to 34,326.183,936 ns. 
       Setting this field and the MULTIPLIER to all 0s indicates that the device will 
       be impacted by any delay and that the best possible service is requested. 
     */
    UINT32 multiplier : 3;
    /* multiplier - Bits[12:10], RW, default = 3'b000 
       This field indicates the scale that the NSTL value is multipled by to yield a 
       time value. 
     */
    UINT32 rsvd_13 : 1;
    /* rsvd_13 - Bits[13:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 force_nl : 1;
    /* force_nl - Bits[14:14], RW, default = 1'b0 
       When this bit is set, PCODE will choose the non-snoop latency requirement from 
       this register, regardless of the LTR messages that are recieved by any of the 
       PCI Express controllers. 
       When this bit is clear, PCODE will choose the non-snoop latency requirement as 
       the minimum value taken between this register and each of the LTR messages that 
       were received by the PCI Express controllers with the Requirement bit set to 1b. 
     */
    UINT32 nl_v : 1;
    /* nl_v - Bits[15:15], RW, default = 1'b0 
       When this bit is set to 0b, PCODE will ignore the Non-Snoop Latency override 
       value. 
     */
    UINT32 sxl : 10;
    /* sxl - Bits[25:16], RW, default = 10'b0000000000 
       Latency requirement for Snoop requests.  This value is multiplied by the 
       SXL_MULTIPLIER field to yield a time value, yielding an expressible range from 
       1ns to 34,326.183,936 ns. 
       Setting this field and the SXL_MULTIPLIER to all 0s indicates that the device 
       will be impacted by any delay and that the best possible service is requested. 
     */
    UINT32 sxlm : 3;
    /* sxlm - Bits[28:26], RW, default = 3'b000 
       This field indicates the scale that the SXL value is multipled by to yield a 
       time value. 
     */
    UINT32 rsvd_29 : 1;
    /* rsvd_29 - Bits[29:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 force_sxl : 1;
    /* force_sxl - Bits[30:30], RW, default = 2'b00 
       When this bit is set, PCODE will choose the snoop latency requirement from this 
       register, regardless of the LTR messages that are recieved by any of the PCI 
       Express controllers. 
       When this bit is clear, PCODE will choose the snoop latency requirement as the 
       minimum value taken between this register and each of the LTR messages that were 
       received by the PCI Express controllers with the Requirement bit set to 1b. 
     */
    UINT32 sxl_v : 1;
    /* sxl_v - Bits[31:31], RW, default = 1'b0 
       When this bit is set to 0b, PCODE will ignore the Snoop Latency override value.
     */
  } Bits;
  UINT32 Data;
} PCIE_ILTR_OVRD_PCU_FUN1_STRUCT;


#endif /* PCU_FUN1_h */
