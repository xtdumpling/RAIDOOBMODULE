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

#ifndef CHA_MISC_h
#define CHA_MISC_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* CHA_MISC_DEV 8                                                             */
/* CHA_MISC_FUN 0                                                             */

/* VID_CHA_MISC_REG supported on:                                             */
/*       SKX_A0 (0x20140000)                                                  */
/*       SKX (0x20140000)                                                     */
/* Register default value:              0x8086                                */
#define VID_CHA_MISC_REG 0x00002000
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} VID_CHA_MISC_STRUCT;


/* DID_CHA_MISC_REG supported on:                                             */
/*       SKX_A0 (0x20140002)                                                  */
/*       SKX (0x20140002)                                                     */
/* Register default value:              0x208D                                */
#define DID_CHA_MISC_REG 0x00002002
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h208D 
        
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
} DID_CHA_MISC_STRUCT;


/* PCICMD_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x20140004)                                                  */
/*       SKX (0x20140004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_CHA_MISC_REG 0x00002004
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} PCICMD_CHA_MISC_STRUCT;


/* PCISTS_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x20140006)                                                  */
/*       SKX (0x20140006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_CHA_MISC_REG 0x00002006
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} PCISTS_CHA_MISC_STRUCT;


/* RID_CHA_MISC_REG supported on:                                             */
/*       SKX_A0 (0x10140008)                                                  */
/*       SKX (0x10140008)                                                     */
/* Register default value:              0x00                                  */
#define RID_CHA_MISC_REG 0x00001008
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} RID_CHA_MISC_STRUCT;


/* CCR_N0_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x10140009)                                                  */
/*       SKX (0x10140009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_CHA_MISC_REG 0x00001009
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_CHA_MISC_STRUCT;


/* CCR_N1_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x2014000A)                                                  */
/*       SKX (0x2014000A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_CHA_MISC_REG 0x0000200A
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} CCR_N1_CHA_MISC_STRUCT;


/* CLSR_CHA_MISC_REG supported on:                                            */
/*       SKX_A0 (0x1014000C)                                                  */
/*       SKX (0x1014000C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_CHA_MISC_REG 0x0000100C
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} CLSR_CHA_MISC_STRUCT;


/* PLAT_CHA_MISC_REG supported on:                                            */
/*       SKX_A0 (0x1014000D)                                                  */
/*       SKX (0x1014000D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_CHA_MISC_REG 0x0000100D
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} PLAT_CHA_MISC_STRUCT;


/* HDR_CHA_MISC_REG supported on:                                             */
/*       SKX_A0 (0x1014000E)                                                  */
/*       SKX (0x1014000E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_CHA_MISC_REG 0x0000100E
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} HDR_CHA_MISC_STRUCT;


/* BIST_CHA_MISC_REG supported on:                                            */
/*       SKX_A0 (0x1014000F)                                                  */
/*       SKX (0x1014000F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_CHA_MISC_REG 0x0000100F
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} BIST_CHA_MISC_STRUCT;


/* SVID_CHA_MISC_REG supported on:                                            */
/*       SKX_A0 (0x2014002C)                                                  */
/*       SKX (0x2014002C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_CHA_MISC_REG 0x0000202C
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} SVID_CHA_MISC_STRUCT;


/* SDID_CHA_MISC_REG supported on:                                            */
/*       SKX_A0 (0x2014002E)                                                  */
/*       SKX (0x2014002E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_CHA_MISC_REG 0x0000202E
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} SDID_CHA_MISC_STRUCT;


/* CAPPTR_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x10140034)                                                  */
/*       SKX (0x10140034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_CHA_MISC_REG 0x00001034
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} CAPPTR_CHA_MISC_STRUCT;


/* INTL_CHA_MISC_REG supported on:                                            */
/*       SKX_A0 (0x1014003C)                                                  */
/*       SKX (0x1014003C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_CHA_MISC_REG 0x0000103C
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} INTL_CHA_MISC_STRUCT;


/* INTPIN_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x1014003D)                                                  */
/*       SKX (0x1014003D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_CHA_MISC_REG 0x0000103D
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} INTPIN_CHA_MISC_STRUCT;


/* MINGNT_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x1014003E)                                                  */
/*       SKX (0x1014003E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_CHA_MISC_REG 0x0000103E
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} MINGNT_CHA_MISC_STRUCT;


/* MAXLAT_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x1014003F)                                                  */
/*       SKX (0x1014003F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_CHA_MISC_REG 0x0000103F
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
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
} MAXLAT_CHA_MISC_STRUCT;


/* HA_AD_CREDITS_CHA_MISC_REG supported on:                                   */
/*       SKX_A0 (0x4014009C)                                                  */
/*       SKX (0x4014009C)                                                     */
/* Register default value:              0x00111111                            */
#define HA_AD_CREDITS_CHA_MISC_REG 0x0000409C
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * This register contains HA AD Credit info.
 */
typedef union {
  struct {
    UINT32 ad_vna_port0 : 4;
    /* ad_vna_port0 - Bits[3:0], RWS, default = 4'b0001 
       AD VNA count for PORT0. MC0.
     */
    UINT32 ad_vna_port1 : 4;
    /* ad_vna_port1 - Bits[7:4], RWS, default = 4'b0001 
       AD VNA count for PORT1. MC1.
     */
    UINT32 rsvd_8 : 16;
    UINT32 disadvnaisocres : 1;
    /* disadvnaisocres - Bits[24:24], RWS, default = 1'b0 
       Disable AD VNA Isoc credit reservation. One AD VNA credit is reserved for ISOC 
       in ISOC enabled configurations if this bit is not set. 
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} HA_AD_CREDITS_CHA_MISC_STRUCT;


/* EXRAS_MEMMIGRATION_CHA_MISC_REG supported on:                              */
/*       SKX_A0 (0x401400BC)                                                  */
/*       SKX (0x401400BC)                                                     */
/* Register default value:              0x00000022                            */
#define EXRAS_MEMMIGRATION_CHA_MISC_REG 0x000040BC
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * Register for Memory Migration
 */
typedef union {
  struct {
    UINT32 migrationmodeen : 1;
    /* migrationmodeen - Bits[0:0], RW_LB, default = 1'b0 
       When Set, migration is enabled from master socket to the slave socket.
     */
    UINT32 ismaster : 1;
    /* ismaster - Bits[1:1], RW_LB, default = 1'b1 
       In Migration mode, if set it implies the local socket is the master and if clear 
       it is the slave socket. 
     */
    UINT32 mastersocketid : 3;
    /* mastersocketid - Bits[4:2], RW_LB, default = 3'b000 
       In Migration mode, if local=slave, fwd mem writes to master socketid.
     */
    UINT32 slavesocketid : 3;
    /* slavesocketid - Bits[7:5], RW_LB, default = 3'b001 
       In Migration mode, if local=master, fwd mem writes to slave socketid.
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} EXRAS_MEMMIGRATION_CHA_MISC_STRUCT;


/* CHA_RTR_SNP_CTL0_CHA_MISC_REG supported on:                                */
/*       SKX_A0 (0x401400D0)                                                  */
/*       SKX (0x401400D0)                                                     */
/* Register default value:              0x00000000                            */
#define CHA_RTR_SNP_CTL0_CHA_MISC_REG 0x000040D0
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * SnoopFanoutRouter UPI-Port-Selection Control.  If a request comes in from 
 * NodeID=i, a fanout snoop is sent to the respective Intel UPI for which the bits 
 * in Send2KtiPort_i are set, if the ith bit in CHA_RTR_SNP_CTL1.Enable_Fanout is 
 * set to 1. 
 */
typedef union {
  struct {
    UINT32 send2ktiport_0 : 4;
    /* send2ktiport_0 - Bits[3:0], RW_LB, default = 4'b0  */
    UINT32 send2ktiport_1 : 4;
    /* send2ktiport_1 - Bits[7:4], RW_LB, default = 4'b0  */
    UINT32 send2ktiport_2 : 4;
    /* send2ktiport_2 - Bits[11:8], RW_LB, default = 4'b0  */
    UINT32 send2ktiport_3 : 4;
    /* send2ktiport_3 - Bits[15:12], RW_LB, default = 4'b0  */
    UINT32 send2ktiport_4 : 4;
    /* send2ktiport_4 - Bits[19:16], RW_LB, default = 4'b0  */
    UINT32 send2ktiport_5 : 4;
    /* send2ktiport_5 - Bits[23:20], RW_LB, default = 4'b0  */
    UINT32 send2ktiport_6 : 4;
    /* send2ktiport_6 - Bits[27:24], RW_LB, default = 4'b0  */
    UINT32 send2ktiport_7 : 4;
    /* send2ktiport_7 - Bits[31:28], RW_LB, default = 4'b0  */
  } Bits;
  UINT32 Data;
} CHA_RTR_SNP_CTL0_CHA_MISC_STRUCT;


/* CHA_RTR_SNP_CTL1_CHA_MISC_REG supported on:                                */
/*       SKX_A0 (0x401400D4)                                                  */
/*       SKX (0x401400D4)                                                     */
/* Register default value:              0x00000000                            */
#define CHA_RTR_SNP_CTL1_CHA_MISC_REG 0x000040D4
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * SnoopFanoutRouter Enable Control Register
 */
typedef union {
  struct {
    UINT32 enable_fanout : 8;
    /* enable_fanout - Bits[7:0], RW_LB, default = 8'b0  */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CHA_RTR_SNP_CTL1_CHA_MISC_STRUCT;










/* HA_THRESHOLDS_CHA_MISC_REG supported on:                                   */
/*       SKX_A0 (0x4014010C)                                                  */
/*       SKX (0x4014010C)                                                     */
/* Register default value:              0x0000137F                            */
#define HA_THRESHOLDS_CHA_MISC_REG 0x0000410C
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * HA register containing HA thresholds
 */
typedef union {
  struct {
    UINT32 ak_fifo_threshold : 3;
    /* ak_fifo_threshold - Bits[2:0], RWS, default = 3'b111 
       Defines the number of credits to release back to CMS for AK bouncing.  Since 
       this can be a value of 0-7, 1 will 
                                    be added to this value to make the possible range 
       of credits 1-8.  The AK fifo in HA is 8 entries deep. 
     */
    UINT32 mem_rd_credits : 2;
    /* mem_rd_credits - Bits[4:3], RWS, default = 2'b11 
       Mem Rd Egress Staging buffer (ESB) Credits
     */
    UINT32 mem_wr_credits : 2;
    /* mem_wr_credits - Bits[6:5], RWS, default = 2'b11 
       Mem Wr Egress Staging buffer (ESB) Credits
     */
    UINT32 snoop_credits : 2;
    /* snoop_credits - Bits[8:7], RWS, default = 2'b10 
       Snoop Egress Staging buffer (ESB) Credits
     */
    UINT32 fwd_credits : 2;
    /* fwd_credits - Bits[10:9], RWS, default = 2'b01 
       Forward Egress Staging buffer (ESB) Credits
     */
    UINT32 mig_wr_credits : 2;
    /* mig_wr_credits - Bits[12:11], RWS, default = 2'b10 
       Memory Migration Wr Egress Staging buffer (ESB) Credits
     */
    UINT32 hacreditchange : 1;
    /* hacreditchange - Bits[13:13], RWS, default = 1'b0 
       Toggle this bit to load credit values
     */
    UINT32 resp_func_bypass_sel : 1;
    /* resp_func_bypass_sel - Bits[14:14], RWS, default = 1'b0 
       Response Function Select for Bypass Disable
     */
    UINT32 resp_func_bypass_en : 1;
    /* resp_func_bypass_en - Bits[15:15], RWS, default = 1'b0 
       Response Function Enable for Bypass Disable
     */
    UINT32 resp_func_retry_sel : 1;
    /* resp_func_retry_sel - Bits[16:16], RWS, default = 1'b0 
       Response Function Select for Retry Disable
     */
    UINT32 resp_func_retry_en : 1;
    /* resp_func_retry_en - Bits[17:17], RWS, default = 1'b0 
       Response Function Enable for Retry Select
     */
    UINT32 resp_func_ak_sel : 1;
    /* resp_func_ak_sel - Bits[18:18], RWS, default = 1'b0 
       Response Function Select for Ak Disable
     */
    UINT32 resp_func_ak_en : 1;
    /* resp_func_ak_en - Bits[19:19], RWS, default = 1'b0 
       Response Function Enable for Ak Select
     */
    UINT32 resp_func_tor_sel : 1;
    /* resp_func_tor_sel - Bits[20:20], RWS, default = 1'b0 
       Response Function Select for Tor Disable
     */
    UINT32 resp_func_tor_en : 1;
    /* resp_func_tor_en - Bits[21:21], RWS, default = 1'b0 
       Response Function Enable for Tor Select
     */
    UINT32 resp_func_osb_sel : 1;
    /* resp_func_osb_sel - Bits[22:22], RWS, default = 1'b0 
       Response Function Select for OSB Disable
     */
    UINT32 resp_func_osb_en : 1;
    /* resp_func_osb_en - Bits[23:23], RWS, default = 1'b0 
       Response Function Enable for OSB Select
     */
    UINT32 disegressisocres : 1;
    /* disegressisocres - Bits[24:24], RWS, default = 1'b0 
       Disable Egress Isoc credit reservation
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} HA_THRESHOLDS_CHA_MISC_STRUCT;


/* HA_COH_CHA_MISC_REG supported on:                                          */
/*       SKX_A0 (0x40140110)                                                  */
/*       SKX (0x40140110)                                                     */
/* Register default value:              0x06040301                            */
#define HA_COH_CHA_MISC_REG 0x00004110
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * HA Coherency Config Register
 */
typedef union {
  struct {
    UINT32 active_node_mask : 8;
    /* active_node_mask - Bits[7:0], RW_LB, default = 8'b00000001 
       Active Nodes In System. One bit per Node ID of caching agents in the system. It 
       is used to control which node snoops are spawned to and responses are expected 
       from. Default value is NodeID_0 and NodeID_1 active. 
     */
    UINT32 dis_directory : 1;
    /* dis_directory - Bits[8:8], RW_LB, default = 1'b1 
       1'b1 - Disable Directory ORd with Fuse_Dis_Directory. Directory should be 
       disabled for single socket configurations. Directory should be enabled for all 
       multi-socket configurations. 
     */
    UINT32 dis_spec_rd : 1;
    /* dis_spec_rd - Bits[9:9], RW_LB, default = 1'b1 
       1'b1- Disable Early Read (Wait for snoop Response) on HitME$ hit. This bit 
       should be set to 1 when HitME$ is enabled AND 2LM mode is disabled.  
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[10:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 frc_osb_loc_invitoe : 1;
    /* frc_osb_loc_invitoe - Bits[11:11], RW_LB, default = 1'b0 
       Force enablement of InvItoE OSB
     */
    UINT32 en_osb_loc_invitoe : 1;
    /* en_osb_loc_invitoe - Bits[12:12], RW_LB, default = 1'b0 
       Threshold Based Enablement of InvItoE OSB
     */
    UINT32 osb_ad_vna_thr : 5;
    /* osb_ad_vna_thr - Bits[17:13], RW_LB, default = 5'b00000 
       Intel UPI AD VNA Threshold for local InvIToE OSB. If Intel UPI VNA credits 
       available is >= OSB_AD_VNA_Thr in each of the Intel UPI ports, then allow OSB 
     */
    UINT32 osb_snp_thr : 3;
    /* osb_snp_thr - Bits[20:18], RW_LB, default = 3'b001 
       SNP Staging buffer Threshold for local InvIToE OSB. If snoop staging buffers 
       available is >= OSB_SNP_Thr, then allow OSB 
     */
    UINT32 dis_ha_idle_bypass : 1;
    /* dis_ha_idle_bypass - Bits[21:21], RW_LB, default = 1'b0 
       Disable bypassing HApipe under idle conditions for latency savings
     */
    UINT32 iodc_enable : 1;
    /* iodc_enable - Bits[22:22], RW_LB, default = 1'b0 
       Enabled IODC. IODC can be enabled only in DIRECTORY enabled configurations.
     */
    UINT32 iodc_morph_invitoe : 1;
    /* iodc_morph_invitoe - Bits[23:23], RW_LB, default = 1'b0 
       Enable generation of InvIToM for IIO InvIToE (writes). This should be set to 
       utilize the IODC feature. This bit is ineffective if IODC is not enabled.  
     */
    UINT32 rsvd_24 : 1;
    UINT32 kti0enabled : 1;
    /* kti0enabled - Bits[25:25], RW_LB, default = 1'b1 
       Intel UPI 0 is enabled. This is used by the OSB logic to determine if Intel UPI 
       0 is enabled. 
     */
    UINT32 kti1enabled : 1;
    /* kti1enabled - Bits[26:26], RW_LB, default = 1'b1 
       Intel UPI 1 is enabled. This is used by the OSB logic to determine if Intel UPI 
       1 is enabled. 
     */
    UINT32 kti2enabled : 1;
    /* kti2enabled - Bits[27:27], RW_LB, default = 1'b0 
       Intel UPI 2 is enabled. This is used by the OSB logic to determine if Intel UPI 
       2 is enabled. 
     */
    UINT32 rsvd_28 : 1;
    UINT32 senddirsonlocalwbs : 1;
    /* senddirsonlocalwbs - Bits[29:29], RW_LB, default = 1'b0 
       For a local WbSData send Dir=S otherwise Dir=I will be sent. Only relevant if 
       directory is enabled. Expect to be set to 0, experiment post-Si. 
     */
    UINT32 rdinvownacceptsm : 1;
    /* rdinvownacceptsm - Bits[30:30], RW_LB, default = 1'b0 
       Allows Data_M in the local CA without WB to memory for local RdInvOwn receiving 
       Rsp*IWb. Expect to be set to 1 in multi-socket systems. 
     */
    UINT32 rddatamigacceptsm : 1;
    /* rddatamigacceptsm - Bits[31:31], RW_LB, default = 1'b0 
       Allows Data_M in the local CA without WB to memory for local RdDataMig receiving 
       Rsp*IWb. Expect to be set to 1 in multi-socket systems. 
     */
  } Bits;
  UINT32 Data;
} HA_COH_CHA_MISC_STRUCT;


/* HA_BL_CREDITS_CHA_MISC_REG supported on:                                   */
/*       SKX_A0 (0x40140114)                                                  */
/*       SKX (0x40140114)                                                     */
/* Register default value:              0x00111111                            */
#define HA_BL_CREDITS_CHA_MISC_REG 0x00004114
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * This register contains HA AD Credit info.
 */
typedef union {
  struct {
    UINT32 bl_vna_port0 : 4;
    /* bl_vna_port0 - Bits[3:0], RWS, default = 4'b0001 
       BL VNA count for PORT0. MC0.
     */
    UINT32 bl_vna_port1 : 4;
    /* bl_vna_port1 - Bits[7:4], RWS, default = 4'b0001 
       BL VNA count for PORT1. MC1.
     */
    UINT32 rsvd_8 : 16;
    UINT32 disblvnaisocres : 1;
    /* disblvnaisocres - Bits[24:24], RWS, default = 1'b0 
       Disable BL VNA Isoc credit reservation
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} HA_BL_CREDITS_CHA_MISC_STRUCT;


/* LLCERRINJ_CHA_MISC_REG supported on:                                       */
/*       SKX_A0 (0x40140118)                                                  */
/*       SKX (0x40140118)                                                     */
/* Register default value:              0x00000000                            */
#define LLCERRINJ_CHA_MISC_REG 0x00004118
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * This CSR is for read-side DATA Array Error Injection
 */
typedef union {
  struct {
    UINT32 inj3bdataerr : 1;
    /* inj3bdataerr - Bits[0:0], RWS_LBV, default = 1'b0 
       This is used to flip the triple bit data error indication.
     */
    UINT32 errinjmode : 1;
    /* errinjmode - Bits[1:1], RWS_LB, default = 1'b0 
       if set to 1, it will be once-only injection mode.
     */
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} LLCERRINJ_CHA_MISC_STRUCT;


/* HA_COH_CFG_1_CHA_MISC_REG supported on:                                    */
/*       SKX_A0 (0x4014011C)                                                  */
/*       SKX (0x4014011C)                                                     */
/* Register default value:              0x000009FF                            */
#define HA_COH_CFG_1_CHA_MISC_REG 0x0000411C
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * HA Coherency Config Register
 */
typedef union {
  struct {
    UINT32 iodc_enabled_entries : 8;
    /* iodc_enabled_entries - Bits[7:0], RW_LB, default = 8'b11111111 
       Bit vector to show which of the 8 IODC entries are enabled.
     */
    UINT32 rsvd_8: 2;
    UINT32 ha_mini_bypass_dis : 1;
    /* ha_mini_bypass_dis - Bits[10:10], RW_LB, default = 1'b0 
       HA Mini Bypass mode. When a Read request gets read out of the TORReqQ and there 
       is not anything ahead of it in the HApipe, the read will be issued right away 
       saving 2 cycles. 
     */
    UINT32 dissadnmcachebit : 1;
    /* dissadnmcachebit - Bits[11:11], RW_LB, default = 1'b1 
       When this is set, we will fall back and send a MemInvXtoI when OSBLocItoE=1, 
       2LMEn=1, NM$=1, WCilF=1, NM_SetConf=0. i.e. we will disable the generation of 
       MemInvItoX. 
     */
    UINT32 staleatosopten : 1;
    /* staleatosopten - Bits[12:12], RW_LB, default = 1'b0 
       A to S directory optimization. When RdData* finds DIR=A and all snoop responses 
       received are RspI, then directory is moved to S and data is returned in S-state. 
       This optimization will not be effective in xNC configuration where BuriedM is 
       possible. 
     */
    UINT32 buriedca : 1;
    /* buriedca - Bits[13:13], RW_LB, default = 1'b0 
       Buried Caching Agent. This should be set in xNC configurations where buriedM is 
       possible. i.e. a requestor can issue a request while holding the line in 
       modified state. Processor caching agent will never have buriedM. 
     */
    UINT32 rsvd_14 : 1;
    UINT32 gateosbiodcalloc : 1;
    /* gateosbiodcalloc - Bits[15:15], RW_LB, default = 1'b0 
       When OSB indicates that there aren't enough snoop credits don't allocate IODC 
       entry.  
     */
    UINT32 sparehacsrunnnh : 16;
    /* sparehacsrunnnh - Bits[31:16], RW_LB, default = 16'b0000000000000000 
       
       SpareHACSRUnnnH[0]   - chicken bit for b303737/b304192
       SpareHACSRUnnnH[1]   - chicken bit for b305235
       SpareHACSRUnnnH[2]   - chicken bit for b305840
       SpareHACSRUnnnH[3]   - chicken bit for b305464
       SpareHACSRUnnnH[4]   - chicken bit for b305927
       SpareHACSRUnnnH[5]   - chicken bit for b305941
       SpareHACSRUnnnH[6]   - chicken bit for b306620
                   
     */
  } Bits;
  UINT32 Data;
} HA_COH_CFG_1_CHA_MISC_STRUCT;








/* REMOTE_FPGA_CHA_MISC_REG supported on:                                     */
/*       SKX (0x40140280)                                                     */
/* Register default value:              0x00000000                            */
#define REMOTE_FPGA_CHA_MISC_REG 0x00004280
/* Struct format extracted from XML file SKX\1.8.0.CFG.xml.
 * KTI-Connected FPGA Config Register
 */
typedef union {
  struct {
    UINT32 fpga_0_nodeid : 3;
    /* fpga_0_nodeid - Bits[2:0], RWS_LB, default = 3'b000 
       Node ID for FPGA0
     */
    UINT32 fpga_0_valid : 1;
    /* fpga_0_valid - Bits[3:3], RWS_LB, default = 1'b0 
       Valid for FPGA0
     */
    UINT32 fpga_1_nodeid : 3;
    /* fpga_1_nodeid - Bits[6:4], RWS_LB, default = 3'b000 
       Node ID for FPGA1
     */
    UINT32 fpga_1_valid : 1;
    /* fpga_1_valid - Bits[7:7], RWS_LB, default = 1'b0 
       Valid for FPGA1
     */
    UINT32 snoop_filter_active : 1;
    /* snoop_filter_active - Bits[8:8], RWS_LB, default = 1'b0 
       FPGA Snoop Filter enable
     */
    UINT32 snoop_filter_size : 2;
    /* snoop_filter_size - Bits[10:9], RWS_LB, default = 2'b0 
       FPGA Snoop Filter size encoding
     */
    UINT32 rsvd : 21;
    /* rsvd - Bits[31:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} REMOTE_FPGA_CHA_MISC_STRUCT;


#endif /* CHA_MISC_h */
