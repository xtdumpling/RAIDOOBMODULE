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

#ifndef PCU_FUN4_h
#define PCU_FUN4_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* PCU_FUN4_DEV 30                                                            */
/* PCU_FUN4_FUN 4                                                             */

/* VID_PCU_FUN4_REG supported on:                                             */
/*       SKX_A0 (0x201F4000)                                                  */
/*       SKX (0x201F4000)                                                     */
/* Register default value:              0x8086                                */
#define VID_PCU_FUN4_REG 0x04042000
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} VID_PCU_FUN4_STRUCT;


/* DID_PCU_FUN4_REG supported on:                                             */
/*       SKX_A0 (0x201F4002)                                                  */
/*       SKX (0x201F4002)                                                     */
/* Register default value:              0x2084                                */
#define DID_PCU_FUN4_REG 0x04042002
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2084 
        
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
} DID_PCU_FUN4_STRUCT;


/* PCICMD_PCU_FUN4_REG supported on:                                          */
/*       SKX_A0 (0x201F4004)                                                  */
/*       SKX (0x201F4004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_PCU_FUN4_REG 0x04042004
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} PCICMD_PCU_FUN4_STRUCT;


/* PCISTS_PCU_FUN4_REG supported on:                                          */
/*       SKX_A0 (0x201F4006)                                                  */
/*       SKX (0x201F4006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_PCU_FUN4_REG 0x04042006
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} PCISTS_PCU_FUN4_STRUCT;


/* RID_PCU_FUN4_REG supported on:                                             */
/*       SKX_A0 (0x101F4008)                                                  */
/*       SKX (0x101F4008)                                                     */
/* Register default value:              0x00                                  */
#define RID_PCU_FUN4_REG 0x04041008
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} RID_PCU_FUN4_STRUCT;


/* CCR_N0_PCU_FUN4_REG supported on:                                          */
/*       SKX_A0 (0x101F4009)                                                  */
/*       SKX (0x101F4009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_PCU_FUN4_REG 0x04041009
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_PCU_FUN4_STRUCT;


/* CCR_N1_PCU_FUN4_REG supported on:                                          */
/*       SKX_A0 (0x201F400A)                                                  */
/*       SKX (0x201F400A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_PCU_FUN4_REG 0x0404200A
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} CCR_N1_PCU_FUN4_STRUCT;


/* CLSR_PCU_FUN4_REG supported on:                                            */
/*       SKX_A0 (0x101F400C)                                                  */
/*       SKX (0x101F400C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_PCU_FUN4_REG 0x0404100C
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} CLSR_PCU_FUN4_STRUCT;


/* PLAT_PCU_FUN4_REG supported on:                                            */
/*       SKX_A0 (0x101F400D)                                                  */
/*       SKX (0x101F400D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_PCU_FUN4_REG 0x0404100D
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} PLAT_PCU_FUN4_STRUCT;


/* HDR_PCU_FUN4_REG supported on:                                             */
/*       SKX_A0 (0x101F400E)                                                  */
/*       SKX (0x101F400E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_PCU_FUN4_REG 0x0404100E
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} HDR_PCU_FUN4_STRUCT;


/* BIST_PCU_FUN4_REG supported on:                                            */
/*       SKX_A0 (0x101F400F)                                                  */
/*       SKX (0x101F400F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_PCU_FUN4_REG 0x0404100F
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} BIST_PCU_FUN4_STRUCT;


/* SVID_PCU_FUN4_REG supported on:                                            */
/*       SKX_A0 (0x201F402C)                                                  */
/*       SKX (0x201F402C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_PCU_FUN4_REG 0x0404202C
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} SVID_PCU_FUN4_STRUCT;


/* SDID_PCU_FUN4_REG supported on:                                            */
/*       SKX_A0 (0x201F402E)                                                  */
/*       SKX (0x201F402E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_PCU_FUN4_REG 0x0404202E
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} SDID_PCU_FUN4_STRUCT;


/* CAPPTR_PCU_FUN4_REG supported on:                                          */
/*       SKX_A0 (0x101F4034)                                                  */
/*       SKX (0x101F4034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_PCU_FUN4_REG 0x04041034
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} CAPPTR_PCU_FUN4_STRUCT;


/* INTL_PCU_FUN4_REG supported on:                                            */
/*       SKX_A0 (0x101F403C)                                                  */
/*       SKX (0x101F403C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_PCU_FUN4_REG 0x0404103C
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} INTL_PCU_FUN4_STRUCT;


/* INTPIN_PCU_FUN4_REG supported on:                                          */
/*       SKX_A0 (0x101F403D)                                                  */
/*       SKX (0x101F403D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_PCU_FUN4_REG 0x0404103D
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} INTPIN_PCU_FUN4_STRUCT;


/* MINGNT_PCU_FUN4_REG supported on:                                          */
/*       SKX_A0 (0x101F403E)                                                  */
/*       SKX (0x101F403E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_PCU_FUN4_REG 0x0404103E
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} MINGNT_PCU_FUN4_STRUCT;


/* MAXLAT_PCU_FUN4_REG supported on:                                          */
/*       SKX_A0 (0x101F403F)                                                  */
/*       SKX (0x101F403F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_PCU_FUN4_REG 0x0404103F
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
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
} MAXLAT_PCU_FUN4_STRUCT;


/* GLOBAL_NID_SOCKET_4_TO_7_MAP_PCU_FUN4_REG supported on:                    */
/*       SKX_A0 (0x401F407C)                                                  */
/*       SKX (0x401F407C)                                                     */
/* Register default value:              0x00000000                            */
#define GLOBAL_NID_SOCKET_4_TO_7_MAP_PCU_FUN4_REG 0x0404407C
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register is in the PCU CR space.  It contains NID information for sockets 
 * 4-7 in the platform.  Expectation is that BIOS will write this register during 
 * the Reset/Init flow. 
 */
typedef union {
  struct {
    UINT32 skt4_nid : 3;
    /* skt4_nid - Bits[2:0], RW, default = 3'b000 
       Socket4 NID
     */
    UINT32 rsvd_3 : 1;
    UINT32 skt5_nid : 3;
    /* skt5_nid - Bits[6:4], RW, default = 3'b000 
       Socket5 NID
     */
    UINT32 rsvd_7 : 1;
    UINT32 skt6_nid : 3;
    /* skt6_nid - Bits[10:8], RW, default = 3'b000 
       Socket6 NID
     */
    UINT32 rsvd_11 : 1;
    UINT32 skt7_nid : 3;
    /* skt7_nid - Bits[14:12], RW, default = 3'b000 
       Socket7 NID
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
} GLOBAL_NID_SOCKET_4_TO_7_MAP_PCU_FUN4_STRUCT;




/* VIRAL_CONTROL_PCU_FUN4_REG supported on:                                   */
/*       SKX_A0 (0x401F4084)                                                  */
/*       SKX (0x401F4084)                                                     */
/* Register default value:              0x00000000                            */
#define VIRAL_CONTROL_PCU_FUN4_REG 0x04044084
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * Controls how PCU responds to viral and eMCA signalling.
 *       Note: This register must be programmed for eMCA mode even if viral is not 
 * enabled. 
 */
typedef union {
  struct {
    UINT32 viral_reset : 1;
    /* viral_reset - Bits[0:0], RW1S, default = 1'b0 
       Clears the CATERR#/MSMI# pin after a viral is signalled.
     */
    UINT32 viral_fast_pin_dis : 1;
    /* viral_fast_pin_dis - Bits[1:1], RW, default = 1'b0 
       When set, disables the signaling of IERR/MSMI based on the global fatal signal.  
       CATERR# assertion dependent on message (MCERR/IERR) received over message 
       channel. MSMI# assertion dependent on message received over message channel.   
       Values:
               0 -> IERR signaling by global fatal is enabled. In viral condition, 
       CATERR#/MSMI# will assert always with IERR semantics. 
               1 -> IERR signaling by global fatal is disabled. CATERR#/MSMI# assertion 
       dependent on message (MCERR/IERR/MSMI) received over message channel. 
       
     */
    UINT32 emca_mode : 1;
    /* emca_mode - Bits[2:2], RW, default = 1'b0 
       When set to high, MSMI input is enabled, and viral will get propagated to MSMI# 
       pin, otherwise it will get propagated to CATERR# pin.   
     */
    UINT32 viral_log_disable : 1;
    /* viral_log_disable - Bits[3:3], RW, default = 1'b0 
       This will disable immediate logging of errors while in viral mode.
       
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VIRAL_CONTROL_PCU_FUN4_STRUCT;


/* SMB_TLOW_TIMEOUT_CFG_0_PCU_FUN4_REG supported on:                          */
/*       SKX_A0 (0x401F4088)                                                  */
/*       SKX (0x401F4088)                                                     */
/* Register default value:              0x00003568                            */
#define SMB_TLOW_TIMEOUT_CFG_0_PCU_FUN4_REG 0x04044088
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains configuration for timeout timer used to control how long 
 * to wait after BIOS sets clock override before hardware resets clock override. 
 * Note that instance 0 is shared between SPD SMBus 0 and SPD SMBus 1 and instance 
 * 2 controls MCP SMBus.  
 */
typedef union {
  struct {
    UINT32 smb_tlow_time_out : 16;
    /* smb_tlow_time_out - Bits[15:0], RWS, default = 16'h3568 
        Upper 16b of the 24b SMBus Time-Out Timer Configuration in unit of FXCLK. 
       Hardware will simply compare against the upper 16-bits of the 24-bit timeout 
       counter and will reset smb_ckovrd.  
       Assuming FXCLK period is at 10ns (no overclocking ref clock): For 35ms time out, 
       please configure this register to 3568. For 65ms time out, please configure this 
       register to 632F.  
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TLOW_TIMEOUT_CFG_0_PCU_FUN4_STRUCT;


/* SMB_TLOW_TIMEOUT_CFG_1_PCU_FUN4_REG supported on:                          */
/*       SKX_A0 (0x401F408C)                                                  */
/*       SKX (0x401F408C)                                                     */
/* Register default value:              0x00003568                            */
#define SMB_TLOW_TIMEOUT_CFG_1_PCU_FUN4_REG 0x0404408C
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains configuration for timeout timer used to control how long 
 * to wait after BIOS sets clock override before hardware resets clock override. 
 * Note that instance 0 is shared between SPD SMBus 0 and SPD SMBus 1 and instance 
 * 2 controls MCP SMBus.  
 */
typedef union {
  struct {
    UINT32 smb_tlow_time_out : 16;
    /* smb_tlow_time_out - Bits[15:0], RWS, default = 16'h3568 
        Upper 16b of the 24b SMBus Time-Out Timer Configuration in unit of FXCLK. 
       Hardware will simply compare against the upper 16-bits of the 24-bit timeout 
       counter and will reset smb_ckovrd.  
       Assuming FXCLK period is at 10ns (no overclocking ref clock): For 35ms time out, 
       please configure this register to 3568. For 65ms time out, please configure this 
       register to 632F.  
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMB_TLOW_TIMEOUT_CFG_1_PCU_FUN4_STRUCT;


/* CPUBUSNO_PCU_FUN4_REG supported on:                                        */
/*       SKX_A0 (0x401F4094)                                                  */
/*       SKX (0x401F4094)                                                     */
/* Register default value:              0x03020100                            */
#define CPUBUSNO_PCU_FUN4_REG 0x04044094
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register is used by BIOS to write the Bus number for 
 * the the socket. Pcode will use these values to determine whether PECI accesses 
 * are local or downstream. 
 * 
 */
typedef union {
  struct {
    UINT32 bus_no_0 : 8;
    /* bus_no_0 - Bits[7:0], RW_LB, default = 8'b00000000 
       Base bus number for stack 0
     */
    UINT32 bus_no_1 : 8;
    /* bus_no_1 - Bits[15:8], RW_LB, default = 8'b00000001 
       Top bus number for the range for stack 0 (IIO 0).  Stack 1 base bus number is 1 
       more than this value. 
     */
    UINT32 bus_no_2 : 8;
    /* bus_no_2 - Bits[23:16], RW_LB, default = 8'b00000010 
       Top bus number for the range for stack 1 (IIO 1).  Stack 2 base bus number is 1 
       more than this value. 
     */
    UINT32 bus_no_3 : 8;
    /* bus_no_3 - Bits[31:24], RW_LB, default = 8'b00000011 
       Top bus number for the range for stack 2 (IIO 2).  Stack 3 base bus number is 1 
       more than this value. 
     */
  } Bits;
  UINT32 Data;
} CPUBUSNO_PCU_FUN4_STRUCT;


/* CPUBUSNO1_PCU_FUN4_REG supported on:                                       */
/*       SKX_A0 (0x401F4098)                                                  */
/*       SKX (0x401F4098)                                                     */
/* Register default value:              0x07060504                            */
#define CPUBUSNO1_PCU_FUN4_REG 0x04044098
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register is used by BIOS to write the Bus number for the the socket. Pcode 
 * will use these values to determine whether PECI accesses are local or 
 * downstream. 
 * 
 */
typedef union {
  struct {
    UINT32 bus_no_4 : 8;
    /* bus_no_4 - Bits[7:0], RW_LB, default = 8'b00000100 
       Top bus number for the range for stack 3 (IIO 3).  Stack 4 base bus number is 1 
       more than this value. 
     */
    UINT32 bus_no_5 : 8;
    /* bus_no_5 - Bits[15:8], RW_LB, default = 8'b00000101 
       Top bus number for the range for stack 4 (MCP 0).  If this stack is not present, 
       then this value must equal entry 4 value.  Stack 5 (if present) base bus number 
       is 1 more than this value. 
     */
    UINT32 bus_no_6 : 8;
    /* bus_no_6 - Bits[23:16], RW_LB, default = 8'b00000110 
       Top bus number for the range for stack 5 (MCP 1).  If this stack is not present, 
       then this value must equal entry 5 value. 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} CPUBUSNO1_PCU_FUN4_STRUCT;


/* CPUBUSNO_VALID_PCU_FUN4_REG supported on:                                  */
/*       SKX_A0 (0x401F409C)                                                  */
/*       SKX (0x401F409C)                                                     */
/* Register default value:              0x0000003F                            */
#define CPUBUSNO_VALID_PCU_FUN4_REG 0x0404409C
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register is used by BIOS to write the Bus number for the the socket. Pcode 
 * will use these values to determine whether PECI accesses are local or 
 * downstream. 
 * 
 */
typedef union {
  struct {
    UINT32 bus_range_valid : 8;
    /* bus_range_valid - Bits[7:0], RW_LB, default = 8'b00111111 
       Indicates the corresponding range of bus numbers from other CSRs is valid.  Bit 
       n applies to range larger or equal to bus[n] and less than bus[n+1]. BIOS clears 
       a bit to disable a range (due to stack not present, power-gated, clock-gated).  
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[30:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 busno_programmed : 1;
    /* busno_programmed - Bits[31:31], RW_LB, default = 1'b0 
       Set by BIOS when the CSR is written with valid bus numbers. 
       or not
     */
  } Bits;
  UINT32 Data;
} CPUBUSNO_VALID_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_0_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40A0)                                                  */
/*       SKX (0x401F40A0)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_0_PCU_FUN4_REG 0x040440A0
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_0_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_1_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40A4)                                                  */
/*       SKX (0x401F40A4)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_1_PCU_FUN4_REG 0x040440A4
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_1_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_2_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40A8)                                                  */
/*       SKX (0x401F40A8)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_2_PCU_FUN4_REG 0x040440A8
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_2_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_3_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40AC)                                                  */
/*       SKX (0x401F40AC)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_3_PCU_FUN4_REG 0x040440AC
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_3_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_4_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40B0)                                                  */
/*       SKX (0x401F40B0)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_4_PCU_FUN4_REG 0x040440B0
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_4_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_5_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40B4)                                                  */
/*       SKX (0x401F40B4)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_5_PCU_FUN4_REG 0x040440B4
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_5_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_6_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40B8)                                                  */
/*       SKX (0x401F40B8)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_6_PCU_FUN4_REG 0x040440B8
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_6_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_7_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40BC)                                                  */
/*       SKX (0x401F40BC)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_7_PCU_FUN4_REG 0x040440BC
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_7_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_8_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40C0)                                                  */
/*       SKX (0x401F40C0)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_8_PCU_FUN4_REG 0x040440C0
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_8_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_9_PCU_FUN4_REG supported on:                     */
/*       SKX_A0 (0x401F40C4)                                                  */
/*       SKX (0x401F40C4)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_9_PCU_FUN4_REG 0x040440C4
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_9_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_10_PCU_FUN4_REG supported on:                    */
/*       SKX_A0 (0x401F40C8)                                                  */
/*       SKX (0x401F40C8)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_10_PCU_FUN4_REG 0x040440C8
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_10_PCU_FUN4_STRUCT;


/* MEM_ACCUMULATED_BW_CH_CFG_11_PCU_FUN4_REG supported on:                    */
/*       SKX_A0 (0x401F40CC)                                                  */
/*       SKX (0x401F40CC)                                                     */
/* Register default value:              0x00000000                            */
#define MEM_ACCUMULATED_BW_CH_CFG_11_PCU_FUN4_REG 0x040440CC
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register contains a measurement proportional to the weighted DRAM BW for 
 * the channel (including all ranks).  The weights are configured in the memory 
 * controller channel register PM_CMD_PWR. 
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       The weighted BW value is calculated by the memory controller based on the 
       following formula:Num_Precharge * PM_CMD_PWR[PWR_RAS_PRE] +Num_Reads * 
       PM_CMD_PWR[PWR_CAS_R] + Num_Writes * PM_CMD_PWR[PWR_CAS_W] 
     */
  } Bits;
  UINT32 Data;
} MEM_ACCUMULATED_BW_CH_CFG_11_PCU_FUN4_STRUCT;


/* MCP_THERMAL_REPORT_1_PCU_FUN4_REG supported on:                            */
/*       SKX_A0 (0x401F40D0)                                                  */
/*       SKX (0x401F40D0)                                                     */
/* Register default value:              0x00000000                            */
#define MCP_THERMAL_REPORT_1_PCU_FUN4_REG 0x040440D0
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * MCP Thermal reporting.
 *       This CSR is a mirror of MSR (1a3h) MCP_THERMAL_REPORT_1
 */
typedef union {
  struct {
    UINT32 margin_to_throttle : 16;
    /* margin_to_throttle - Bits[15:0], RO_V, default = 16'b0 
       Margin to Throttle represents the nomalized temperature margin until throttling 
       engages on any component.   
        It reports the minimum of the temperature margins on different dies.  Each 
       margin represents the current sensor temperature relative to the temperature 
       when the hotspot on the die is at its Tj,max. It is reported as a negative 
       number in signed, filtered 10.6 fixed-point format. Note: This is also reported 
       via PECS as the PCS index 2 function. 
       	
     */
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} MCP_THERMAL_REPORT_1_PCU_FUN4_STRUCT;


/* MCP_THERMAL_REPORT_2_PCU_FUN4_REG supported on:                            */
/*       SKX_A0 (0x401F40D8)                                                  */
/*       SKX (0x401F40D8)                                                     */
/* Register default value:              0x00000000                            */
#define MCP_THERMAL_REPORT_2_PCU_FUN4_REG 0x040440D8
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * MCP Thermal reporting.
 *       This CSR is a mirror of MSR (1A4h) MCP_THERMAL_REPORT_2.
 */
typedef union {
  struct {
    UINT32 package_absolute_max_temperature : 16;
    /* package_absolute_max_temperature - Bits[15:0], RO_V, default = 16'b0 
       Package Absolute Max Temperature reports the hottest absolute temperature of any 
       component in the multi-chip package.  
       	This field is reported in signed, absolute, filtered 10.6 fixed-point format. 
        Note: This is also reported via PECS as PCS index 9 command with parameter 
       0x80. 
       	
     */
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} MCP_THERMAL_REPORT_2_PCU_FUN4_STRUCT;


/* UNC_TSC_SNAPSHOT_N0_PCU_FUN4_REG supported on:                             */
/*       SKX_A0 (0x401F40E0)                                                  */
/*       SKX (0x401F40E0)                                                     */
/* Register default value:              0x00000000                            */
#define UNC_TSC_SNAPSHOT_N0_PCU_FUN4_REG 0x040440E0
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register will capture the value of Uncore TSC on TSC_SYNC assertion rising 
 * edge detect in PCU 
 */
typedef union {
  struct {
    UINT32 uncore_tsc_snapshot : 32;
    /* uncore_tsc_snapshot - Bits[31:0], RO_V, default = 63'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Value of the captured Uncore TSC on internal rising edge of TSC_SYNC
     */
  } Bits;
  UINT32 Data;
} UNC_TSC_SNAPSHOT_N0_PCU_FUN4_STRUCT;


/* UNC_TSC_SNAPSHOT_N1_PCU_FUN4_REG supported on:                             */
/*       SKX_A0 (0x401F40E4)                                                  */
/*       SKX (0x401F40E4)                                                     */
/* Register default value:              0x00000000                            */
#define UNC_TSC_SNAPSHOT_N1_PCU_FUN4_REG 0x040440E4
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This register will capture the value of Uncore TSC on TSC_SYNC assertion rising 
 * edge detect in PCU 
 */
typedef union {
  struct {
    UINT32 uncore_tsc_snapshot : 31;
    /* uncore_tsc_snapshot - Bits[30:0], RO_V, default = 63'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Value of the captured Uncore TSC on internal rising edge of TSC_SYNC
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} UNC_TSC_SNAPSHOT_N1_PCU_FUN4_STRUCT;


/* TSC_HP_OFFSET_N0_PCU_FUN4_REG supported on:                                */
/*       SKX_A0 (0x401F40E8)                                                  */
/*       SKX (0x401F40E8)                                                     */
/* Register default value:              0x00000000                            */
#define TSC_HP_OFFSET_N0_PCU_FUN4_REG 0x040440E8
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * BIOS may write this register to update the TSC in the hot plugged socket. This 
 * is a one time event.  
 */
typedef union {
  struct {
    UINT32 tsc_offset : 32;
    /* tsc_offset - Bits[31:0], RW_LB, default = 63'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       BIOS programmed value to do a one-time update to the Uncore TSC to align it for 
       Hot Plug 
     */
  } Bits;
  UINT32 Data;
} TSC_HP_OFFSET_N0_PCU_FUN4_STRUCT;


/* TSC_HP_OFFSET_N1_PCU_FUN4_REG supported on:                                */
/*       SKX_A0 (0x401F40EC)                                                  */
/*       SKX (0x401F40EC)                                                     */
/* Register default value:              0x00000000                            */
#define TSC_HP_OFFSET_N1_PCU_FUN4_REG 0x040440EC
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * BIOS may write this register to update the TSC in the hot plugged socket. This 
 * is a one time event.  
 */
typedef union {
  struct {
    UINT32 tsc_offset : 31;
    /* tsc_offset - Bits[30:0], RW_LB, default = 63'b0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       BIOS programmed value to do a one-time update to the Uncore TSC to align it for 
       Hot Plug 
     */
    UINT32 tsc_update : 1;
    /* tsc_update - Bits[31:31], RW_LB, default = 1'b0 
       When set, will result cause the TSC_OFFSET value to be added to the Uncore TSC 
       (one-shot) 
     */
  } Bits;
  UINT32 Data;
} TSC_HP_OFFSET_N1_PCU_FUN4_STRUCT;


/* PCU_FIRST_IERR_TSC_LO_PCU_FUN4_REG supported on:                           */
/*       SKX_A0 (0x401F40F0)                                                  */
/*       SKX (0x401F40F0)                                                     */
/* Register default value:              0x00000000                            */
#define PCU_FIRST_IERR_TSC_LO_PCU_FUN4_REG 0x040440F0
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This CSR stores the lower 4B of the TSC snapshot taken on the first internal 
 * IERR detected.  
 */
typedef union {
  struct {
    UINT32 snapshot : 32;
    /* snapshot - Bits[31:0], ROS_V, default = 32'b00000000000000000000000000000000 
       Low 4B of TSC snapshot taken on first internal IERR
     */
  } Bits;
  UINT32 Data;
} PCU_FIRST_IERR_TSC_LO_PCU_FUN4_STRUCT;


/* PCU_FIRST_IERR_TSC_HI_PCU_FUN4_REG supported on:                           */
/*       SKX_A0 (0x401F40F4)                                                  */
/*       SKX (0x401F40F4)                                                     */
/* Register default value:              0x00000000                            */
#define PCU_FIRST_IERR_TSC_HI_PCU_FUN4_REG 0x040440F4
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This CSR stores the high 4B of the TSC snapshot taken on the first internal IERR 
 * detected.  
 */
typedef union {
  struct {
    UINT32 snapshot : 32;
    /* snapshot - Bits[31:0], ROS_V, default = 32'b00000000000000000000000000000000 
       High 4B of TSC snapshot taken on first internal IERR
     */
  } Bits;
  UINT32 Data;
} PCU_FIRST_IERR_TSC_HI_PCU_FUN4_STRUCT;


/* PCU_FIRST_MCERR_TSC_LO_PCU_FUN4_REG supported on:                          */
/*       SKX_A0 (0x401F40F8)                                                  */
/*       SKX (0x401F40F8)                                                     */
/* Register default value:              0x00000000                            */
#define PCU_FIRST_MCERR_TSC_LO_PCU_FUN4_REG 0x040440F8
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This CSR stores the lower 4B of the TSC snapshot taken on the first internal 
 * MCERR detected.  
 */
typedef union {
  struct {
    UINT32 snapshot : 32;
    /* snapshot - Bits[31:0], ROS_V, default = 32'b00000000000000000000000000000000 
       Low 4B of TSC snapshot taken on first internal MCERR
     */
  } Bits;
  UINT32 Data;
} PCU_FIRST_MCERR_TSC_LO_PCU_FUN4_STRUCT;


/* PCU_FIRST_MCERR_TSC_HI_PCU_FUN4_REG supported on:                          */
/*       SKX_A0 (0x401F40FC)                                                  */
/*       SKX (0x401F40FC)                                                     */
/* Register default value:              0x00000000                            */
#define PCU_FIRST_MCERR_TSC_HI_PCU_FUN4_REG 0x040440FC
/* Struct format extracted from XML file SKX\1.30.4.CFG.xml.
 * This CSR stores the high 4B of the TSC snapshot taken on the first internal 
 * MCERR detected.  
 */
typedef union {
  struct {
    UINT32 snapshot : 32;
    /* snapshot - Bits[31:0], ROS_V, default = 32'b00000000000000000000000000000000 
       High 4B of TSC snapshot taken on first internal MCERR
     */
  } Bits;
  UINT32 Data;
} PCU_FIRST_MCERR_TSC_HI_PCU_FUN4_STRUCT;


#endif /* PCU_FUN4_h */
