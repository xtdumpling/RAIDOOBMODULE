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

#ifndef KTI_CIOPHYDFX_h
#define KTI_CIOPHYDFX_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* KTI_CIOPHYDFX_DEV 14                                                       */
/* KTI_CIOPHYDFX_FUN 2                                                        */

/* VID_KTI_CIOPHYDFX_REG supported on:                                        */
/*       SKX_A0 (0x20372000)                                                  */
/*       SKX (0x20372000)                                                     */
/* Register default value:              0x8086                                */
#define VID_KTI_CIOPHYDFX_REG 0x09022000
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} VID_KTI_CIOPHYDFX_STRUCT;


/* DID_KTI_CIOPHYDFX_REG supported on:                                        */
/*       SKX_A0 (0x20372002)                                                  */
/*       SKX (0x20372002)                                                     */
/* Register default value:              0x205A                                */
#define DID_KTI_CIOPHYDFX_REG 0x09022002
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h205A 
        
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
} DID_KTI_CIOPHYDFX_STRUCT;


/* PCICMD_KTI_CIOPHYDFX_REG supported on:                                     */
/*       SKX_A0 (0x20372004)                                                  */
/*       SKX (0x20372004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_KTI_CIOPHYDFX_REG 0x09022004
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} PCICMD_KTI_CIOPHYDFX_STRUCT;


/* PCISTS_KTI_CIOPHYDFX_REG supported on:                                     */
/*       SKX_A0 (0x20372006)                                                  */
/*       SKX (0x20372006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_KTI_CIOPHYDFX_REG 0x09022006
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} PCISTS_KTI_CIOPHYDFX_STRUCT;


/* RID_KTI_CIOPHYDFX_REG supported on:                                        */
/*       SKX_A0 (0x10372008)                                                  */
/*       SKX (0x10372008)                                                     */
/* Register default value:              0x00                                  */
#define RID_KTI_CIOPHYDFX_REG 0x09021008
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} RID_KTI_CIOPHYDFX_STRUCT;


/* CCR_N0_KTI_CIOPHYDFX_REG supported on:                                     */
/*       SKX_A0 (0x10372009)                                                  */
/*       SKX (0x10372009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_KTI_CIOPHYDFX_REG 0x09021009
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_KTI_CIOPHYDFX_STRUCT;


/* CCR_N1_KTI_CIOPHYDFX_REG supported on:                                     */
/*       SKX_A0 (0x2037200A)                                                  */
/*       SKX (0x2037200A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_KTI_CIOPHYDFX_REG 0x0902200A
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} CCR_N1_KTI_CIOPHYDFX_STRUCT;


/* CLSR_KTI_CIOPHYDFX_REG supported on:                                       */
/*       SKX_A0 (0x1037200C)                                                  */
/*       SKX (0x1037200C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_KTI_CIOPHYDFX_REG 0x0902100C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} CLSR_KTI_CIOPHYDFX_STRUCT;


/* PLAT_KTI_CIOPHYDFX_REG supported on:                                       */
/*       SKX_A0 (0x1037200D)                                                  */
/*       SKX (0x1037200D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_KTI_CIOPHYDFX_REG 0x0902100D
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} PLAT_KTI_CIOPHYDFX_STRUCT;


/* HDR_KTI_CIOPHYDFX_REG supported on:                                        */
/*       SKX_A0 (0x1037200E)                                                  */
/*       SKX (0x1037200E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_KTI_CIOPHYDFX_REG 0x0902100E
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} HDR_KTI_CIOPHYDFX_STRUCT;


/* BIST_KTI_CIOPHYDFX_REG supported on:                                       */
/*       SKX_A0 (0x1037200F)                                                  */
/*       SKX (0x1037200F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_KTI_CIOPHYDFX_REG 0x0902100F
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} BIST_KTI_CIOPHYDFX_STRUCT;


/* SVID_F2_KTI_CIOPHYDFX_REG supported on:                                    */
/*       SKX_A0 (0x2037202C)                                                  */
/*       SKX (0x2037202C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_F2_KTI_CIOPHYDFX_REG 0x0902202C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * PCIE Subsystem vendor identification number
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'h8086 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_F2_KTI_CIOPHYDFX_STRUCT;


/* SDID_F2_KTI_CIOPHYDFX_REG supported on:                                    */
/*       SKX_A0 (0x2037202E)                                                  */
/*       SKX (0x2037202E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_F2_KTI_CIOPHYDFX_REG 0x0902202E
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} SDID_F2_KTI_CIOPHYDFX_STRUCT;


/* CAPPTR_KTI_CIOPHYDFX_REG supported on:                                     */
/*       SKX_A0 (0x10372034)                                                  */
/*       SKX (0x10372034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_KTI_CIOPHYDFX_REG 0x09021034
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} CAPPTR_KTI_CIOPHYDFX_STRUCT;


/* INTL_KTI_CIOPHYDFX_REG supported on:                                       */
/*       SKX_A0 (0x1037203C)                                                  */
/*       SKX (0x1037203C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_KTI_CIOPHYDFX_REG 0x0902103C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} INTL_KTI_CIOPHYDFX_STRUCT;


/* INTPIN_KTI_CIOPHYDFX_REG supported on:                                     */
/*       SKX_A0 (0x1037203D)                                                  */
/*       SKX (0x1037203D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_KTI_CIOPHYDFX_REG 0x0902103D
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} INTPIN_KTI_CIOPHYDFX_STRUCT;


/* MINGNT_KTI_CIOPHYDFX_REG supported on:                                     */
/*       SKX_A0 (0x1037203E)                                                  */
/*       SKX (0x1037203E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_KTI_CIOPHYDFX_REG 0x0902103E
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} MINGNT_KTI_CIOPHYDFX_STRUCT;


/* MAXLAT_KTI_CIOPHYDFX_REG supported on:                                     */
/*       SKX_A0 (0x1037203F)                                                  */
/*       SKX (0x1037203F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_KTI_CIOPHYDFX_REG 0x0902103F
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} MAXLAT_KTI_CIOPHYDFX_STRUCT;




/* KTIHMTXMAT0_KTI_CIOPHYDFX_REG supported on:                                */
/*       SKX_A0 (0x40372200)                                                  */
/*       SKX (0x40372200)                                                     */
/* Register default value:              0x00000000                            */
#define KTIHMTXMAT0_KTI_CIOPHYDFX_REG 0x09024200
/* Struct format extracted from XML file SKX_A0\3.14.2.CFG.xml.
 * Intel UPI Header Match Transmit 0
 * These bit fields indicate specifically the value to match against in the 
 * flit/VISA bus to produce a MatchOut assertion.  
 * It is expected that the assertion is valid only for the core clock cycle that 
 * the flit is valid.  
 * This register is used with the mask register to produced ranges of searches for 
 * particular types of transactions.  
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 24;
    UINT32 match_mode_and : 1;
    /* match_mode_and - Bits[24:24], RWS, default = 1'b0 
       Mode: 0 means OR the final results of the FC, S0, S1 and S2 match/masks.  1 
       means AND. 
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIHMTXMAT0_KTI_CIOPHYDFX_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Header Match Transmit 0
 * These bit fields indicate specifically the value to match against in the 
 * flit/VISA bus to produce a MatchOut assertion.  
 * It is expected that the assertion is valid only for the core clock cycle that 
 * the flit is valid.  
 * This register is used with the mask register to produced ranges of searches for 
 * particular types of transactions.  
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 23;
    UINT32 match_mode_and : 1;
    /* match_mode_and - Bits[23:23], RWS, default = 1'b0 
       Mode: 0 means OR the final results of the FC, S0, S1 and S2 match/masks.  1 
       means AND. 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} KTIHMTXMAT0_KTI_CIOPHYDFX_STRUCT;







































































/* KTIACKCNT_KTI_CIOPHYDFX_REG supported on:                                  */
/*       SKX_A0 (0x40372290)                                                  */
/*       SKX (0x40372290)                                                     */
/* Register default value:              0x00000000                            */
#define KTIACKCNT_KTI_CIOPHYDFX_REG 0x09024290
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Ack Counts
 * This register contains the ack counts and link layer read/write pointers. The 
 * values stored are either the current value or a snapshot captured value 
 * depending on the control bit. 
 */
typedef union {
  struct {
    UINT32 ackcnt : 8;
    /* ackcnt - Bits[7:0], ROS_V, default = 8'b0 
       Acknowledge count[7:0]
     */
    UINT32 llrptr : 7;
    /* llrptr - Bits[14:8], ROS_V, default = 7'b0 
       Link layer retry read pointer[6:0]
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 llwptr : 7;
    /* llwptr - Bits[22:16], ROS_V, default = 7'b0 
       Link layer retry write pointer[6:0]
     */
    UINT32 rsvd_23 : 1;
    /* rsvd_23 - Bits[23:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_24 : 1;
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIACKCNT_KTI_CIOPHYDFX_STRUCT;










/* KTIRXEINJCTL0_KTI_CIOPHYDFX_REG supported on:                              */
/*       SKX_A0 (0x403722B0)                                                  */
/*       SKX (0x403722B0)                                                     */
/* Register default value:              0xCACA0000                            */
#define KTIRXEINJCTL0_KTI_CIOPHYDFX_REG 0x090242B0
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Rx Error Injection Control 0
 * This register contains the error injection control register that applies the 
 * mask registers across the width of the Intel UPI flit for error detection 
 * testing. 
 */
typedef union {
  struct {
    UINT32 superr_en : 1;
    /* superr_en - Bits[0:0], RWS_LB, default = 1'b0 
       0 - Do not suppress error injection in replay. Warning: using this setting makes 
       it more likely to hit the abort state. 
       1 - Suppress errors on the replay that reached the escalation counters according 
       to the bit settings in EinjRetryActiveMode, EinjNumRetry, and EinjNumPhyReInit 
     */
    UINT32 superr_hold_on_respfcn : 1;
    /* superr_hold_on_respfcn - Bits[1:1], RWS_LB, default = 1'b0 
       0 - allow suppression to stop when LRSM goes inactive (on successful retry with 
       Empty=1 or retriable flit) 
       1 - hold suppression even after LRSM goes inactive (on successful retry with 
       Empty=1 or retriable flit) and only stop suppression once the response function 
       deasserts injection (or when superr_en=0). 
     */
    UINT32 maten : 1;
    /* maten - Bits[2:2], RWS_LB, default = 1'b0 
       0 - No Header match, inject error on any header (IIB==1), except exclude NULLs 
       if bit[5] EXCLUDE_NULL==1. 
       1 - Inject error based on Header match MATF1 hit signal (ignores MATF2 and 
       HDmatOut hit signals) 
     */
    UINT32 crcdis : 1;
    /* crcdis - Bits[3:3], RWS_LB, default = 1'b0 
       0 - Enable CRC checking
       1 - Disable CRC checking
     */
    UINT32 suppress_st : 1;
    /* suppress_st - Bits[4:4], RW1CS_LBV, default = 1'b0 
       Error Injection suppression status
       0 - No suppression of injection has occurred.
       1 - Suppression of injection has occurred
     */
    UINT32 errinjst : 1;
    /* errinjst - Bits[5:5], RW1CS_LBV, default = 1'b0 
       0 - No error injection occurred. 
       1 - An error was injected
     */
    UINT32 einjretryactivemode : 2;
    /* einjretryactivemode - Bits[7:6], RWS_LB, default = 2'b0 
       This determines how the link will respond to error injection during replay. 
       00 - Retry mode 
       01 - Phy init mode 
       10 - Retry and Phy init mode 
       11 - Abort mode
     */
    UINT32 einjnumretry : 5;
    /* einjnumretry - Bits[12:8], RWS_LB, default = 5'b0 
       Number of retries [4:0]
     */
    UINT32 einjnumphyreinit : 3;
    /* einjnumphyreinit - Bits[15:13], RWS_LB, default = 2'b0 
       Number of phy retries before re-initialization [2:0]
     */
    UINT32 openloop_str_sel : 4;
    /* openloop_str_sel - Bits[19:16], RWS_LB, default = 4'hA 
       Start selection for injecting in open-loop response function. Open-loop means 
       that the response function will only watch for the stop condition and ignores 
       the response from the affected logic.  
       0000 - Micro Break Point 0 (MBP[0]) 
       0001 - Micro Break Point 1 (MBP[1]) 
       0010 - Cluster Trigger Out 0 (CTO[0]) 
       0011 - Cluster Trigger Out 1 (CTO[1]) 
       0100 - Artificial Starvation Counter 0 (ASC[0]) 
       0101 - Artificial Starvation Counter 1 (ASC[1]) 
       0110 - Common Trigger Sequencer TrigOut (CTS_Trig_Out)
       0111 - Reserved 
       1000 - MCA Correctable Error 
       1001 - MCA Uncorrectable Error 
       1010 - Response never occurs 
       1011 - Response occurs immediately 
       1100-1111 - Reserved
     */
    UINT32 openloop_stp_sel : 4;
    /* openloop_stp_sel - Bits[23:20], RWS_LB, default = 4'hC 
       Stop selection for injecting in open-loop response function. Open-loop means 
       that the response function will only watch for the stop condition and ignores 
       the response from the affected logic.  
       0000 - Micro Break Point 0 (MBP[0]) 
       0001 - Micro Break Point 1 (MBP[1]) 
       0010 - Cluster Trigger Out 0 (CTO[0]) 
       0011 - Cluster Trigger Out 1 (CTO[1]) 
       0100 - Artificial Starvation Counter 0 (ASC[0]) 
       0101 - Artificial Starvation Counter 1 (ASC[1]) 
       0110 - Common Trigger Sequencer TrigOut (CTS_Trig_Out)
       0111 - Reserved 
       1000 - MCA Correctable Error 
       1001 - MCA Uncorrectable Error 
       1010 - Deactivate one clock after activation. Error injection on one transaction 
       will occur on the interface that is enabled.  
       1011 - Response is never deactivated. 
       1100 - Deactivate the response when the activating signal becomes false. 
       1101-1111 - Reserved
     */
    UINT32 once_str_sel : 4;
    /* once_str_sel - Bits[27:24], RWS_LB, default = 4'hA 
       Start selection for injecting in standard closed-loop response function. 
       Closed-loop means that once activated,  
        the response function will not stop until both the stop condition has been met 
       *and* the logic affected by the response function  
        has been able to do the throttling/injecting on actual traffic. Closed-loop is 
       the most common response function.  
       0000 - Micro Break Point 0 (MBP[0]) 
       0001 - Micro Break Point 1 (MBP[1]) 
       0010 - Cluster Trigger Out 0 (CTO[0]) 
       0011 - Cluster Trigger Out 1 (CTO[1]) 
       0100 - Artificial Starvation Counter 0 (ASC[0]) 
       0101 - Artificial Starvation Counter 1 (ASC[1]) 
       0110 - Common Trigger Sequencer TrigOut (CTS_Trig_Out)
       0111 - Reserved 
       1000 - MCA Correctable Error 
       1001 - MCA Uncorrectable Error 
       1010 - Response never occurs 
       1011 - Response occurs immediately 
       1100-1111 - Reserved
     */
    UINT32 once_stp_sel : 4;
    /* once_stp_sel - Bits[31:28], RWS_LB, default = 4'hC 
       Stop selection for injecting in standard closed-loop response function. 
       Closed-loop means that once activated, the response function  
        will not stop until both the stop condition has been met *and* the logic 
       affected by the response function has been able to do the  
        throttling/injecting on actual traffic. Closed-loop is the most common response 
       function.  
       0000 - Micro Break Point 0 (MBP[0]) 
       0001 - Micro Break Point 1 (MBP[1]) 
       0010 - Cluster Trigger Out 0 (CTO[0]) 
       0011 - Cluster Trigger Out 1 (CTO[1]) 
       0100 - Artificial Starvation Counter 0 (ASC[0]) 
       0101 - Artificial Starvation Counter 1 (ASC[1]) 
       0110 - Common Trigger Sequencer TrigOut (CTS_Trig_Out)
       0111 - Reserved 
       1000 - MCA Correctable Error 
       1001 - MCA Uncorrectable Error 
       1010 - Deactivate one clock after activation. Error injection on one transaction 
       will occur on the interface that is enabled.  
       1011 - Response is never deactivated. 
       1100 - Deactivate the response when the activating signal becomes false. 
       1101-1111 - Reserved
     */
  } Bits;
  UINT32 Data;
} KTIRXEINJCTL0_KTI_CIOPHYDFX_STRUCT;


/* KTIRXEINJCTL1_KTI_CIOPHYDFX_REG supported on:                              */
/*       SKX_A0 (0x403722B4)                                                  */
/*       SKX (0x403722B4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIRXEINJCTL1_KTI_CIOPHYDFX_REG 0x090242B4
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Rx Error Injection Control 1
 * This register defines what types of flits to exclude from error injection Any 
 * bits that are prefixed with include, cause Inject a CRC error onto all flit-type 
 * flits that are received on the Intel UPI Link Layer RX.  
 * Any bit that is prefixed with Exclude results in exclusion of all flit-type 
 * flits from injection of CRC error that are received on the Intel UPI Link Layer 
 * RX.  
 * Note that KTIRXEINJCTL0 must be configured for actual error injection to occur. 
 * Note that the include enables take precedence over the exclude enables.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 exclude_data : 1;
    /* exclude_data - Bits[2:2], RWS_L, default = 1'b0 
       Exclude Data Flits
     */
    UINT32 exclude_llctrl_null : 1;
    /* exclude_llctrl_null - Bits[3:3], RWS_L, default = 1'b0 
       Exclude LLCTRL.NULL
     */
    UINT32 rsvd_4 : 1;
    /* rsvd_4 - Bits[4:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 exclude_llctrl_all : 1;
    /* exclude_llctrl_all - Bits[5:5], RWS_L, default = 1'b0 
       Exclude LLCTRL All
     */
    UINT32 exclude_llcrd : 1;
    /* exclude_llcrd - Bits[6:6], RWS_L, default = 1'b0 
       Exclude LLCRD
     */
    UINT32 exclude_llctrl_init_all : 1;
    /* exclude_llctrl_init_all - Bits[7:7], RWS_L, default = 1'b0 
       Exclude LLCTRL.INIT All
     */
    UINT32 exclude_llctrl_retry_req : 1;
    /* exclude_llctrl_retry_req - Bits[8:8], RWS_L, default = 1'b0 
       Exclude LLCTRL.RETRY_REQ
     */
    UINT32 exclude_llctrl_retry_ack : 1;
    /* exclude_llctrl_retry_ack - Bits[9:9], RWS_L, default = 1'b0 
       Exclude LLCTRL.RETRY_ACK
     */
    UINT32 spare13_10 : 4;
    /* spare13_10 - Bits[13:10], RWS_L, default = 4'b0 
        
     */
    UINT32 rsvd_14 : 6;
    /* rsvd_14 - Bits[19:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 include_llcrd : 1;
    /* include_llcrd - Bits[20:20], RWS_L, default = 1'b0 
       Include LLCRD
     */
    UINT32 rsvd_21 : 3;
    /* rsvd_21 - Bits[23:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 include_llctrl_retry_req : 1;
    /* include_llctrl_retry_req - Bits[24:24], RWS_L, default = 1'b0 
       Include LLCTRL.RETRY_REQ
     */
    UINT32 include_llctrl_retry_ack : 1;
    /* include_llctrl_retry_ack - Bits[25:25], RWS_L, default = 1'b0 
       Include LLCTRL.RETRY_ACK
     */
    UINT32 spare31_26 : 6;
    /* spare31_26 - Bits[31:26], RWS_L, default = 1'b0 
        
     */
  } Bits;
  UINT32 Data;
} KTIRXEINJCTL1_KTI_CIOPHYDFX_STRUCT;






/* KTILKPRIVC0_KTI_CIOPHYDFX_REG supported on:                                */
/*       SKX_A0 (0x403722D8)                                                  */
/*       SKX (0x403722D8)                                                     */
/* Register default value:              0x00000000                            */
#define KTILKPRIVC0_KTI_CIOPHYDFX_REG 0x090242D8
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Link Private Control 0
 * Private Control Bits for Debug defined by design team. Bit level details are 
 * TBD. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 ctl_ck_gate_dis : 1;
    /* ctl_ck_gate_dis - Bits[1:1], RWS_LB, default = 1'b0 
       Disable clock gating on selected LL Control  flops
     */
    UINT32 rsvd_2 : 30;
  } Bits;
  UINT32 Data;
} KTILKPRIVC0_KTI_CIOPHYDFX_STRUCT;






/* KTILKPRIVC3_KTI_CIOPHYDFX_REG supported on:                                */
/*       SKX_A0 (0x403722F4)                                                  */
/*       SKX (0x403722F4)                                                     */
/* Register default value on SKX_A0:    0x00000045                            */
/* Register default value on SKX:       0x00280045                            */
#define KTILKPRIVC3_KTI_CIOPHYDFX_REG 0x090242F4
/* Struct format extracted from XML file SKX_A0\3.14.2.CFG.xml.
 * Intel UPI Link Private Control 3
 * Private Control Bits for Debug defined by design team. Bit level details are 
 * TBD. 
 */
typedef union {
  struct {
    UINT32 flowq_ad_vna_threshold : 6;
    /* flowq_ad_vna_threshold - Bits[5:0], RWS_LB, default = 6'd5 
       FlowQ AD VNA threshold
       - if all-FlowQ-VNA check is false (ie false condition of AD_VNA greater-than 2 
       -and- BL_VNA greater-than 0 -and- optional AK_VNA pre-allocated), then AD 
       packets may still consume AD_VNA FlowQ credits if AD_VNA greater-than threshold. 
        Default is 5 (aka (3+3)-1). 
       - program this field to all ones (6'h3f) to disable this feature
       Legal range: 6'h5 to 6'h3f (any value greater than the allocated AK_VNA credits 
       in KTIA2RCRCTRL will effectively disable this feature) 
     */
    UINT32 flowq_bl_vna_threshold : 5;
    /* flowq_bl_vna_threshold - Bits[10:6], RWS_LB, default = 5'd1 
       FlowQ BL VNA threshold
       - if all-FlowQ-VNA check is false (ie false condition of AD_VNA greater-than 2 
       -and- BL_VNA greater-than 0 -and- optional AK_VNA pre-allocated), then BL 
       packets may still consume BL_VNA FlowQ credits if BL_VNA greater-than threshold. 
        Default is 1 (aka (1+1)-1). 
       - program this field to all ones (5'h1f) to disable this feature
       Legal range: 5'h1 to 5'h1f (any value greater than the allocated BL_VNA credits 
       in KTIA2RCRCTRL will effectively disable this feature) 
     */
    UINT32 flowq_ak_vna_prealloc_all_en : 1;
    /* flowq_ak_vna_prealloc_all_en - Bits[11:11], RWS_LB, default = 1'b0 
       FlowQ AK VNA pre-allocation enable
       0 - do not require AK_VNA pre-allocated for AD/BK traffic to flow (except for WB 
       which require BL+AK) 
       1 - enable optional part of all-FlowQ-VNA check (ie AD_VNA greater-than 2 -and- 
       BL_VNA greater-than 0 -and- optional AK_VNA pre-allocated). 
     */
    UINT32 llrq_req_outofbounds_check_disable : 1;
    /* llrq_req_outofbounds_check_disable - Bits[12:12], RWS_LB, default = 1'b0 
       Link Layer Retry Queue Request Out-of-Bounds Check Disable
       This check monitors if an incoming LLCTRL-RETRY.Req has an ESeq that is 
       out-of-bounds with respect to the NumFreeBuf. 
       0: Delta check is enable
       1: Delta check is disabled
     */
    UINT32 spare13 : 1;
    /* spare13 - Bits[13:13], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare14 : 1;
    /* spare14 - Bits[14:14], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare15 : 1;
    /* spare15 - Bits[15:15], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare16 : 1;
    /* spare16 - Bits[16:16], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare17 : 1;
    /* spare17 - Bits[17:17], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare18 : 1;
    /* spare18 - Bits[18:18], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare19 : 1;
    /* spare19 - Bits[19:19], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare20 : 1;
    /* spare20 - Bits[20:20], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare21 : 1;
    /* spare21 - Bits[21:21], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare22 : 1;
    /* spare22 - Bits[22:22], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare23 : 1;
    /* spare23 - Bits[23:23], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare24 : 1;
    /* spare24 - Bits[24:24], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare25 : 1;
    /* spare25 - Bits[25:25], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare26 : 1;
    /* spare26 - Bits[26:26], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare27 : 1;
    /* spare27 - Bits[27:27], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare28 : 1;
    /* spare28 - Bits[28:28], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare29 : 1;
    /* spare29 - Bits[29:29], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare30 : 1;
    /* spare30 - Bits[30:30], RWS_LB, default = 1'b0 
       spare
     */
    UINT32 spare31 : 1;
    /* spare31 - Bits[31:31], RWS_LB, default = 1'b0 
       spare
     */
  } Bits;
  UINT32 Data;
} KTILKPRIVC3_KTI_CIOPHYDFX_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Link Private Control 3
 * Private Control Bits for Debug defined by design team. Bit level details are 
 * TBD. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 19;
    UINT32 rbt_wr_enable : 1;
    /* rbt_wr_enable - Bits[19:19], RW_LB, default = 1'b1  */
    UINT32 rsvd_20 : 7;
    UINT32 enable_p2p_routebyid_targeting_iio4 : 1;
    /* enable_p2p_routebyid_targeting_iio4 - Bits[27:27], RWS_LB, default = 1'b0 
       Enable P2P Route-by-ID (Tunnel DROUTE[4:3]=01 or 10) received in UPI link layer 
       to target IIO4 on the local socket. 
       0 - Force P2P Route-by-ID traffic to IIO0.  Use this setting if IIO4 is 
       devhidden or fuse disabled or disabled in any other manner. 
       1 - Enable P2P Route-by-ID traffic to IIO4.
     */
    UINT32 enable_p2p_routebyid_targeting_iio5 : 1;
    /* enable_p2p_routebyid_targeting_iio5 - Bits[28:28], RWS_LB, default = 1'b0 
       Enable P2P Route-by-ID (Tunnel DROUTE[4:3]=01 or 10) received in UPI link layer 
       to target IIO5 on the local socket. 
       0 - Force P2P Route-by-ID traffic to IIO0.  Use this setting if IIO5 is 
       devhidden or fuse disabled or disabled in any other manner. 
       1 - Enable P2P Route-by-ID traffic to IIO5.
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} KTILKPRIVC3_KTI_CIOPHYDFX_STRUCT;

























/* KTIDBGERRST0_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX_A0 (0x403723A0)                                                  */
/*       SKX (0x403723A0)                                                     */
/* Register default value:              0x00000000                            */
#define KTIDBGERRST0_KTI_CIOPHYDFX_REG 0x090243A0
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * ktidbgerrst0
 */
typedef union {
  struct {
    UINT32 param_excep : 3;
    /* param_excep - Bits[2:0], RW1CS, default = 3'b0  */
    UINT32 rxuuu : 10;
    /* rxuuu - Bits[12:3], RW1CS, default = 10'b0  */
    UINT32 ctrl_phy_ctrl_err : 1;
    /* ctrl_phy_ctrl_err - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 ctrl_unexp_retryack : 1;
    /* ctrl_unexp_retryack - Bits[14:14], RW1CS, default = 1'b0  */
    UINT32 ctrl_unexp_retryreq : 1;
    /* ctrl_unexp_retryreq - Bits[15:15], RW1CS, default = 1'b0  */
    UINT32 ctrl_rf_parity_err : 1;
    /* ctrl_rf_parity_err - Bits[16:16], RW1CS, default = 1'b0  */
    UINT32 ctrl_rbt_err : 2;
    /* ctrl_rbt_err - Bits[18:17], RW1CS, default = 2'b0  */
    UINT32 ctrl_unexp_txprot : 3;
    /* ctrl_unexp_txprot - Bits[21:19], RW1CS, default = 3'b0  */
    UINT32 ctrl_bgf : 6;
    /* ctrl_bgf - Bits[27:22], RW1CS, default = 6'b0  */
    UINT32 ctrl_llrst_phyl0 : 1;
    /* ctrl_llrst_phyl0 - Bits[28:28], RW1CS, default = 1'b0  */
    UINT32 ctrl_llrst_prot : 1;
    /* ctrl_llrst_prot - Bits[29:29], RW1CS, default = 1'b0  */
    UINT32 ctrl_txparerr : 2;
    /* ctrl_txparerr - Bits[31:30], RW1CS, default = 2'b0  */
  } Bits;
  UINT32 Data;
} KTIDBGERRST0_KTI_CIOPHYDFX_STRUCT;


/* KTIDBGERRSTDIS0_KTI_CIOPHYDFX_REG supported on:                            */
/*       SKX_A0 (0x403723A4)                                                  */
/*       SKX (0x403723A4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIDBGERRSTDIS0_KTI_CIOPHYDFX_REG 0x090243A4
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * ktidbgerrstdis0
 */
typedef union {
  struct {
    UINT32 disable_param_excep : 3;
    /* disable_param_excep - Bits[2:0], RWS, default = 3'b0  */
    UINT32 disable_rxuuu : 10;
    /* disable_rxuuu - Bits[12:3], RWS, default = 10'b0  */
    UINT32 disable_ctrl_phy_ctrl_err : 1;
    /* disable_ctrl_phy_ctrl_err - Bits[13:13], RWS, default = 1'b0  */
    UINT32 disable_ctrl_unexp_retryack : 1;
    /* disable_ctrl_unexp_retryack - Bits[14:14], RWS, default = 1'b0  */
    UINT32 disable_ctrl_unexp_retryreq : 1;
    /* disable_ctrl_unexp_retryreq - Bits[15:15], RWS, default = 1'b0  */
    UINT32 disable_ctrl_rf_parity_err : 1;
    /* disable_ctrl_rf_parity_err - Bits[16:16], RWS, default = 1'b0  */
    UINT32 disable_ctrl_rbt_err : 2;
    /* disable_ctrl_rbt_err - Bits[18:17], RWS, default = 2'b0  */
    UINT32 disable_ctrl_unexp_txprot : 3;
    /* disable_ctrl_unexp_txprot - Bits[21:19], RWS, default = 3'b0  */
    UINT32 disable_ctrl_bgf : 6;
    /* disable_ctrl_bgf - Bits[27:22], RWS, default = 6'b0  */
    UINT32 disable_ctrl_llrst_phyl0 : 1;
    /* disable_ctrl_llrst_phyl0 - Bits[28:28], RWS, default = 1'b0  */
    UINT32 disable_ctrl_llrst_prot : 1;
    /* disable_ctrl_llrst_prot - Bits[29:29], RWS, default = 1'b0  */
    UINT32 disable_ctrl_txparerr : 2;
    /* disable_ctrl_txparerr - Bits[31:30], RWS, default = 2'b0  */
  } Bits;
  UINT32 Data;
} KTIDBGERRSTDIS0_KTI_CIOPHYDFX_STRUCT;


/* KTIDBGERRST1_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX_A0 (0x403723A8)                                                  */
/*       SKX (0x403723A8)                                                     */
/* Register default value:              0x00000000                            */
#define KTIDBGERRST1_KTI_CIOPHYDFX_REG 0x090243A8
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * ktidbgerrst1
 */
typedef union {
  struct {
    UINT32 ctrl_overunder : 13;
    /* ctrl_overunder - Bits[12:0], RW1CS, default = 13'b0  */
    UINT32 ctrl_flowqoverunder : 11;
    /* ctrl_flowqoverunder - Bits[23:13], RW1CS, default = 11'b0  */
    UINT32 ctrl_other0 : 1;
    /* ctrl_other0 - Bits[24:24], RW1CS, default = 1'b0  */
    UINT32 ctrl_other1 : 1;
    /* ctrl_other1 - Bits[25:25], RW1CS, default = 1'b0  */
    UINT32 ctrl_other2 : 1;
    /* ctrl_other2 - Bits[26:26], RW1CS, default = 1'b0  */
    UINT32 ctrl_other3 : 1;
    /* ctrl_other3 - Bits[27:27], RW1CS, default = 1'b0  */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIDBGERRST1_KTI_CIOPHYDFX_STRUCT;


/* KTIDBGERRSTDIS1_KTI_CIOPHYDFX_REG supported on:                            */
/*       SKX_A0 (0x403723AC)                                                  */
/*       SKX (0x403723AC)                                                     */
/* Register default value:              0x00000000                            */
#define KTIDBGERRSTDIS1_KTI_CIOPHYDFX_REG 0x090243AC
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * ktidbgerrstdis1
 */
typedef union {
  struct {
    UINT32 disable_ctrl_overunder : 13;
    /* disable_ctrl_overunder - Bits[12:0], RWS, default = 13'b0  */
    UINT32 disable_ctrl_flowqoverunder : 11;
    /* disable_ctrl_flowqoverunder - Bits[23:13], RWS, default = 11'b0  */
    UINT32 disable_ctrl_other0 : 1;
    /* disable_ctrl_other0 - Bits[24:24], RWS, default = 1'b0  */
    UINT32 disable_ctrl_other1 : 1;
    /* disable_ctrl_other1 - Bits[25:25], RWS, default = 1'b0  */
    UINT32 disable_ctrl_other2 : 1;
    /* disable_ctrl_other2 - Bits[26:26], RWS, default = 1'b0  */
    UINT32 disable_ctrl_other3 : 1;
    /* disable_ctrl_other3 - Bits[27:27], RWS, default = 1'b0  */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIDBGERRSTDIS1_KTI_CIOPHYDFX_STRUCT;


/* KTIDBGSMST_KTI_CIOPHYDFX_REG supported on:                                 */
/*       SKX_A0 (0x403723B0)                                                  */
/*       SKX (0x403723B0)                                                     */
/* Register default value:              0x00000000                            */
#define KTIDBGSMST_KTI_CIOPHYDFX_REG 0x090243B0
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Debug State Machine Status
 * This register contains various state machine encodings and other state variables 
 * to reflect whether the Intel UPI link layer is idle or not. 
 */
typedef union {
  struct {
    UINT32 llrstsm : 2;
    /* llrstsm - Bits[1:0], ROS_V, default = 2'b0 
       LL Reset State
       0x0 - Begin
       0x1 - Send to M3
       0x2 - Timer Exit
       0x3 - Done
     */
    UINT32 tism : 2;
    /* tism - Bits[3:2], ROS_V, default = 2'b0 
       LL Tx Init State
       0x0 - Not Ready For Init
       0x1 - Sending Parameter
       0x2 - Parameter Sent
     */
    UINT32 rism : 1;
    /* rism - Bits[4:4], ROS_V, default = 1'b0 
       LL Rx Init State
       0 - Waiting for Parameter
       1 - Parameter Received
     */
    UINT32 lrsm : 3;
    /* lrsm - Bits[7:5], ROS_V, default = 3'b0 
       LL Local Retry State
       0x0 - Retry Local Normal
       0x1 - Retry LLRReq
       0x2 - Retry Local Idle
       0x3 - Retry Phy Reinit
       0x4 - Retry Abort
     */
    UINT32 rrsm : 2;
    /* rrsm - Bits[9:8], ROS_V, default = 2'b0 
       LL Remote Retry State
       0 - Retry Remote Normal
       1 - Retry LLRAck
     */
    UINT32 dbgtsm : 3;
    /* dbgtsm - Bits[12:10], ROS_V, default = 3'b0 
       Debug State
       0x0 - Idle
       0x1 - Force Event
       0x2 - Force Relative Timing Packet (RTP)
       0x3 - Opportunistic Event
       0x4 - Opportunistic Relative Timing Packet (RTP)
     */
    UINT32 tpsnsm : 1;
    /* tpsnsm - Bits[13:13], ROS_V, default = 1'b0 
       Tx Poison State
       0 - Idle
       1 - Arb
     */
    UINT32 tvrlsm : 1;
    /* tvrlsm - Bits[14:14], ROS_V, default = 1'b0 
       Tx Viral State
       0x0 - Idle
       0x1 - Bad CRC
       0x2 - Deassert
     */
    UINT32 rx_active : 1;
    /* rx_active - Bits[15:15], ROS_V, default = 1'b0 
       LL Rx
       0 - inactive
       1 - active
     */
    UINT32 tx_active : 1;
    /* tx_active - Bits[16:16], ROS_V, default = 1'b0 
       LL Tx
       0 - inactive
       1 - active
     */
    UINT32 rx_normal_op : 1;
    /* rx_normal_op - Bits[17:17], ROS_V, default = 1'b0 
       Identical to KTILS.Rx_Normal_Op_State
       0 - Not Rx Normal Operation
       1 - Rx Normal Operation
     */
    UINT32 rsvd_18 : 6;
    /* rsvd_18 - Bits[23:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tx_llr_numfreebuf : 7;
    /* tx_llr_numfreebuf - Bits[30:24], ROS_V, default = 7'b0 
       Link Layer Retry NumFreeBuf.
       less than 83 : link still active
       equal to 83: either active or inactive, but there is no way to know for sure 
       because the Intel UPI spec mandates that remote device should not return the 
       last ACK if that is the only remaining thing to send (in order to avoid 
       potential ACK ping-pong issues). 
       equal to 84: link is inactive.
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIDBGSMST_KTI_CIOPHYDFX_STRUCT;


/* KTIA2RCRCTRL0_ACTIVE_KTI_CIOPHYDFX_REG supported on:                       */
/*       SKX_A0 (0x403723B8)                                                  */
/*       SKX (0x403723B8)                                                     */
/* Register default value:              0x00000000                            */
#define KTIA2RCRCTRL0_ACTIVE_KTI_CIOPHYDFX_REG 0x090243B8
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Agent to Ring Credit Control Active
 * This register holds a RO version of the currently active credit values in real 
 * time. These values were initialized by the values in KTIA2RCRCTRL register. 
 * These values are constantly changing based on credits currently consumed. 
 */
typedef union {
  struct {
    UINT32 bl_rsp0 : 3;
    /* bl_rsp0 - Bits[2:0], RO_V, default = 3'b0 
       BL VN0 RSP credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 bl_wb0 : 1;
    /* bl_wb0 - Bits[3:3], RO_V, default = 1'b0 
       BL VN0 WB credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 bl_ncs0 : 1;
    /* bl_ncs0 - Bits[4:4], RO_V, default = 1'b0 
       BL VN0 NCS credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 bl_ncb0 : 1;
    /* bl_ncb0 - Bits[5:5], RO_V, default = 1'b0 
       BL VN0 NCB credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_wb0 : 1;
    /* ad_wb0 - Bits[6:6], RO_V, default = 1'b0 
       AD VN0 WB credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_req0 : 1;
    /* ad_req0 - Bits[7:7], RO_V, default = 1'b0 
       AD VN0 REQ credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_rsp0 : 3;
    /* ad_rsp0 - Bits[10:8], RO_V, default = 3'b0 
       AD VN0 RSP credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_snp0 : 3;
    /* ad_snp0 - Bits[13:11], RO_V, default = 3'b0 
       AD VN0 SNP credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[14:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ak_vna : 6;
    /* ak_vna - Bits[20:15], RO_V, default = 6'b0 
       AK VNA credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 bl_vna : 5;
    /* bl_vna - Bits[25:21], RO_V, default = 5'b0 
       BL VNA credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_vna : 6;
    /* ad_vna - Bits[31:26], RO_V, default = 6'b0 
       AD VNA credits currently available to the FlowQ from the Intel UPI Agent
     */
  } Bits;
  UINT32 Data;
} KTIA2RCRCTRL0_ACTIVE_KTI_CIOPHYDFX_STRUCT;


/* KTIA2RCRCTRL1_ACTIVE_KTI_CIOPHYDFX_REG supported on:                       */
/*       SKX_A0 (0x403723BC)                                                  */
/*       SKX (0x403723BC)                                                     */
/* Register default value:              0x00000000                            */
#define KTIA2RCRCTRL1_ACTIVE_KTI_CIOPHYDFX_REG 0x090243BC
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Agent to Ring Credit Control Active
 * This register holds a RO version of the currently active credit values in real 
 * time. These values were initialized by the values in KTIA2RCRCTRL register. 
 * These values are constantly changing based on credits currently consumed. 
 */
typedef union {
  struct {
    UINT32 bl_rsp1 : 3;
    /* bl_rsp1 - Bits[2:0], RO_V, default = 3'b0 
       BL VN1 RSP credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 bl_wb1 : 1;
    /* bl_wb1 - Bits[3:3], RO_V, default = 1'b0 
       BL VN1 WB credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 bl_ncs1 : 1;
    /* bl_ncs1 - Bits[4:4], RO_V, default = 1'b0 
       BL VN1 NCS credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 bl_ncb1 : 1;
    /* bl_ncb1 - Bits[5:5], RO_V, default = 1'b0 
       BL VN1 NCB credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_wb1 : 1;
    /* ad_wb1 - Bits[6:6], RO_V, default = 1'b0 
       AD VN1 WB credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_req1 : 1;
    /* ad_req1 - Bits[7:7], RO_V, default = 1'b0 
       AD VN1 REQ credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_rsp1 : 3;
    /* ad_rsp1 - Bits[10:8], RO_V, default = 3'b0 
       AD VN1 RSP credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 ad_snp1 : 3;
    /* ad_snp1 - Bits[13:11], RO_V, default = 3'b0 
       AD VN1 SNP credits currently available to the FlowQ from the Intel UPI Agent
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIA2RCRCTRL1_ACTIVE_KTI_CIOPHYDFX_STRUCT;


/* DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_REG supported on:                            */
/*       SKX_A0 (0x40372800)                                                  */
/*       SKX (0x40372800)                                                     */
/* Register default value:              0x00000007                            */
#define DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_REG 0x09024800
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Register Lock Control for Physical Layer REUT
 */
typedef union {
  struct {
    UINT32 reutlck : 1;
    /* reutlck - Bits[0:0], RWS_KLB, default = 1'b1 
       reut eng ia spce csr lock
     */
    UINT32 reutenglck : 1;
    /* reutenglck - Bits[1:1], RWS_KLB, default = 1'b1 
       reut eng csr lock
     */
    UINT32 privdbg_phy_l0_sync_lck : 1;
    /* privdbg_phy_l0_sync_lck - Bits[2:2], RWS_KLB, default = 1'b1 
       phy L0 Sync csr lock
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_STRUCT;










/* PQ_CSR_IBTXLER_KTI_CIOPHYDFX_REG supported on:                             */
/*       SKX_A0 (0x40372818)                                                  */
/*       SKX (0x40372818)                                                     */
/* Register default value:              0x000FFFFF                            */
#define PQ_CSR_IBTXLER_KTI_CIOPHYDFX_REG 0x09024818
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI IBIST Transmit Lane Enable Register
 */
typedef union {
  struct {
    UINT32 tx_lane_en : 20;
    /* tx_lane_en - Bits[19:0], RWS_L, default = 20'hFFFFF 
       Lane having 0 in this bit-map will transmit a differential 0
       	This value may get modified by pattern inversion and PQ_CSR_TTDDC register
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PQ_CSR_IBTXLER_KTI_CIOPHYDFX_STRUCT;


/* PQ_CSR_PLLFCR_KTI_CIOPHYDFX_REG supported on:                              */
/*       SKX_A0 (0x4037281C)                                                  */
/*       SKX (0x4037281C)                                                     */
/* Register default value:              0x00000020                            */
#define PQ_CSR_PLLFCR_KTI_CIOPHYDFX_REG 0x0902481C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI PLL and Frequency Control Register
 */
typedef union {
  struct {
    UINT32 iopll_ratio : 6;
    /* iopll_ratio - Bits[5:0], ROS_V, default = 6'd32 
       IO PLL freq to Ref clock ratio. Default is 32. If Ref clock is 100MHz, IO PLL 
       clock is 3.2 GHz. Allow values are 48, 30, 32, and 24. 
               This is a read only status. The IO PLL ratio is set in PMA.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PQ_CSR_PLLFCR_KTI_CIOPHYDFX_STRUCT;


/* PQ_CSR_MCSR0_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX_A0 (0x40372820)                                                  */
/*       SKX (0x40372820)                                                     */
/* Register default value:              0x04008000                            */
#define PQ_CSR_MCSR0_KTI_CIOPHYDFX_REG 0x09024820
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Miscellaneous Control and Status Register 0
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 15;
    /* rsvd_0 - Bits[14:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_15 : 11;
    UINT32 assume_pa_valid : 1;
    /* assume_pa_valid - Bits[26:26], RWS_LB, default = 1'b1 
       1 - Assume PA is seen as soon as state machine enters Reset.Calibrate
       	This control has higher priority than pa_before_rstcal_exit
     */
    UINT32 rsvd_27 : 5;
  } Bits;
  UINT32 Data;
} PQ_CSR_MCSR0_KTI_CIOPHYDFX_STRUCT;






/* PQ_CSR_MCSR3_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX_A0 (0x4037282C)                                                  */
/*       SKX (0x4037282C)                                                     */
/* Register default value:              0x00FFFFFF                            */
#define PQ_CSR_MCSR3_KTI_CIOPHYDFX_REG 0x0902482C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Miscellaneous Control and Status Register 3
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 24;
    UINT32 lbuf_offset : 5;
    /* lbuf_offset - Bits[28:24], RWS_L, default = 5'b0 
       Decrease the measured observed latency to compensate for planetary alignment 
       skew. 
       8ui granularity.
     */
    UINT32 silent_err_indicator : 2;
    /* silent_err_indicator - Bits[30:29], RW1CS, default = 2'b0 
       Potential silent error reported by Intel UPI state machines as applicable:
       	00 - No inband reset
        01 - Inband reset received while port was in active (L0 or L0R) state (except 
       it is caused by L1 power state) 
        10 - Inband reset received while port was in non-active state (except it is 
       caused by L1 power state) 
        11 - Inband resets received while port was in active and non-active states (may 
       indicate the bit was not cleared since last inband reset 
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} PQ_CSR_MCSR3_KTI_CIOPHYDFX_STRUCT;


/* PQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG supported on:                              */
/*       SKX_A0 (0x40372838)                                                  */
/*       SKX (0x40372838)                                                     */
/* Register default value:              0x00000000                            */
#define PQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG 0x09024838
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI AFE Indirect Address Register
 */
typedef union {
  struct {
    UINT32 write_en : 1;
    /* write_en - Bits[0:0], RW_LBV, default = 1'b0 
       1 - Write enable
       Software sets this bit and hardware clears it once the write operation is 
       complete.  Software should never clear this bit. 
     */
    UINT32 hw_override_en : 1;
    /* hw_override_en - Bits[1:1], RW_LB, default = 1'b0 
        1 - Override enable
        Must be set by software for Read operations.  Software may choose to leave this 
       bit set. 
       
     */
    UINT32 inc_setting : 1;
    /* inc_setting - Bits[2:2], RW_LB, default = 1'b0 
       1 - increment setting of the IAR addressed by the unit and register IDs
     */
    UINT32 dec_setting : 1;
    /* dec_setting - Bits[3:3], RW_LB, default = 1'b0 
       1 - decrement setting of the IAR addressed by the unit and register IDs
     */
    UINT32 unit_id : 6;
    /* unit_id - Bits[9:4], RW_LB, default = 6'b0 
       Unit ID (example lane ID, Tx Rcomp etc)
     */
    UINT32 register_id : 6;
    /* register_id - Bits[15:10], RW_LB, default = 6'b0 
       Register ID within the unit addressed by the unit ID
     */
    UINT32 wr_data : 8;
    /* wr_data - Bits[23:16], RW_LB, default = 8'b0 
       Write data to be applied to the IAR addressed by unit and register IDs
     */
    UINT32 read_data : 8;
    /* read_data - Bits[31:24], RO_V, default = 8'b0 
       Read data from the IAR addressed by unit and register IDs
     */
  } Bits;
  UINT32 Data;
} PQ_CSR_AFEIAR_KTI_CIOPHYDFX_STRUCT;


/* KTIPHPTDS_KTI_CIOPHYDFX_REG supported on:                                  */
/*       SKX_A0 (0x40372840)                                                  */
/*       SKX (0x40372840)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPHPTDS_KTI_CIOPHYDFX_REG 0x09024840
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Physical Layer Previous Tx data lane status Register
 */
typedef union {
  struct {
    UINT32 tx_dlane_sts : 20;
    /* tx_dlane_sts - Bits[19:0], ROS_V, default = 20'h0 
       A bit value of 1 indicates the TX lane has been dropped
       	Bit 0: Status of lane 0.
       	Bit 1: Status of lane 1.
       	.. and so on..
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPHPTDS_KTI_CIOPHYDFX_STRUCT;


/* KTIPHPRDS_KTI_CIOPHYDFX_REG supported on:                                  */
/*       SKX_A0 (0x40372844)                                                  */
/*       SKX (0x40372844)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPHPRDS_KTI_CIOPHYDFX_REG 0x09024844
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Physical Layer Previous Rx data lane status Register
 */
typedef union {
  struct {
    UINT32 rx_dlane_sts : 20;
    /* rx_dlane_sts - Bits[19:0], ROS_V, default = 20'h0 
       A bit value of 1 indicates the RX lane has been dropped
       	Bit 0: Status of lane 0.
       	Bit 1: Status of lane 1.
       	.. and so on..
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPHPRDS_KTI_CIOPHYDFX_STRUCT;






/* RXSQ_DET_KTI_CIOPHYDFX_REG supported on:                                   */
/*       SKX_A0 (0x40372878)                                                  */
/*       SKX (0x40372878)                                                     */
/* Register default value:              0x00000000                            */
#define RXSQ_DET_KTI_CIOPHYDFX_REG 0x09024878
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * RXSQ Detect Status
 */
typedef union {
  struct {
    UINT32 rxsq_det : 20;
    /* rxsq_det - Bits[19:0], RO_V, default = 20'b0 
       Comparator output of squelch detect/ ntl / rterm_legchk_en.
       	In squelch detect mode:
       	0=exit not detected
       	1=exit detected
       	In ntl/rterm_legchk_en mode:
       	0=input ltreference voltage
       	1=input gtreference voltage
       
        AGENT NOTE: CSR will be updated based on the capt_mode bits of NTL_CTL. Will  
       contain lane based results of ntl, squelch, or rx detect based on other  
       configuration bits. 
       
       	Bit 19  to 0 is data lanes.
       	Bit 20 is rx primary clock lane N
       	Bit 21 is rx primary clock lane P
     */
    UINT32 rsvd : 11;
    /* rsvd - Bits[30:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rst_sqexitflop : 1;
    /* rst_sqexitflop - Bits[31:31], RW_V, default = 1'b0 
       Clears squelch detection flop.  Self-clears to zero.
     */
  } Bits;
  UINT32 Data;
} RXSQ_DET_KTI_CIOPHYDFX_STRUCT;


/* TX_RX_DET_KTI_CIOPHYDFX_REG supported on:                                  */
/*       SKX_A0 (0x4037287C)                                                  */
/*       SKX (0x4037287C)                                                     */
/* Register default value:              0x000FFFFF                            */
#define TX_RX_DET_KTI_CIOPHYDFX_REG 0x0902487C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * RXSQ Detect Status
 */
typedef union {
  struct {
    UINT32 tx_rx_det : 20;
    /* tx_rx_det - Bits[19:0], RO_V, default = 20'hFFFFF 
       TX RX Detect
       	1 = RX detected
       	0 = none.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TX_RX_DET_KTI_CIOPHYDFX_STRUCT;


/* LBC_PER_IOU_CONTROL_KTI_CIOPHYDFX_REG supported on:                        */
/*       SKX_A0 (0x40372900)                                                  */
/*       SKX (0x40372900)                                                     */
/* Register default value:              0x00000000                            */
#define LBC_PER_IOU_CONTROL_KTI_CIOPHYDFX_REG 0x09024900
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
       00: Reserved
       	01: read
       	10: write
       	11: read-modify-write
     */
    UINT32 lbc_ld_sel : 7;
    /* lbc_ld_sel - Bits[9:3], RW_LB, default = 7'b0 
       Selects the EP on the load bus or monitor bus.
     */
    UINT32 lbc_ln_sel : 22;
    /* lbc_ln_sel - Bits[31:10], RW_LB, default = 22'b0 
       For writes, reads, and read-modify-writes this acts as the selection of which 
       lanes have to go through the respective transaction.  
       Note that for read this has to be one hot.
       [29:10] - Data Lanes[19:0]
       [31:30] - Reserved
     */
  } Bits;
  UINT32 Data;
} LBC_PER_IOU_CONTROL_KTI_CIOPHYDFX_STRUCT;


/* LBC_PER_IOU_DATA_KTI_CIOPHYDFX_REG supported on:                           */
/*       SKX_A0 (0x40372904)                                                  */
/*       SKX (0x40372904)                                                     */
/* Register default value:              0x00000000                            */
#define LBC_PER_IOU_DATA_KTI_CIOPHYDFX_REG 0x09024904
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
} LBC_PER_IOU_DATA_KTI_CIOPHYDFX_STRUCT;




























/* KTIPH_RXL0C_ERR_STS_KTI_CIOPHYDFX_REG supported on:                        */
/*       SKX_A0 (0x403729F0)                                                  */
/*       SKX (0x403729F0)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPH_RXL0C_ERR_STS_KTI_CIOPHYDFX_REG 0x090249F0
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Rx L0c Error Log
 */
typedef union {
  struct {
    UINT32 lane_err : 8;
    /* lane_err - Bits[7:0], RW1CS, default = 8'b0 
       Lanes that had errors
     */
    UINT32 force_mismatch : 1;
    /* force_mismatch - Bits[8:8], RW1CS, default = 1'b0 
       L0c mismatch due to >= 4 lanes with error
     */
    UINT32 code_mismatch : 1;
    /* code_mismatch - Bits[9:9], RW1CS, default = 1'b0 
       L0c code did not match
     */
    UINT32 clm : 3;
    /* clm - Bits[12:10], RW1CS, default = 3'h0 
       CLM during l0c error
     */
    UINT32 lane_rev : 1;
    /* lane_rev - Bits[13:13], RW1CS, default = 1'b0 
       Lane reversal status during l0c error
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPH_RXL0C_ERR_STS_KTI_CIOPHYDFX_STRUCT;


/* KTIPH_RXL0C_ERR_LOG0_KTI_CIOPHYDFX_REG supported on:                       */
/*       SKX_A0 (0x403729F4)                                                  */
/*       SKX (0x403729F4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPH_RXL0C_ERR_LOG0_KTI_CIOPHYDFX_REG 0x090249F4
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Rx L0c codes for lower 4 lanes
 * Valid if RXL0C_ERR_STS.lane_err or RXL0C_ERR_STS.mismatch are non-zero.
 */
typedef union {
  struct {
    UINT32 lane0 : 8;
    /* lane0 - Bits[7:0], RW1CS, default = 8'b0 
       lane 0 l0c code
     */
    UINT32 lane1 : 8;
    /* lane1 - Bits[15:8], RW1CS, default = 8'b0 
       lane 1 l0c code
     */
    UINT32 lane2 : 8;
    /* lane2 - Bits[23:16], RW1CS, default = 8'b0 
       lane 2 l0c code
     */
    UINT32 lane3 : 8;
    /* lane3 - Bits[31:24], RW1CS, default = 8'b0 
       lane 3 l0c code
     */
  } Bits;
  UINT32 Data;
} KTIPH_RXL0C_ERR_LOG0_KTI_CIOPHYDFX_STRUCT;


/* KTIPH_RXL0C_ERR_LOG1_KTI_CIOPHYDFX_REG supported on:                       */
/*       SKX_A0 (0x403729F8)                                                  */
/*       SKX (0x403729F8)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPH_RXL0C_ERR_LOG1_KTI_CIOPHYDFX_REG 0x090249F8
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Rx L0c Error Log
 */
typedef union {
  struct {
    UINT32 lane4 : 8;
    /* lane4 - Bits[7:0], RW1CS, default = 8'b0 
       lane 4 l0c code
     */
    UINT32 lane5 : 8;
    /* lane5 - Bits[15:8], RW1CS, default = 8'b0 
       lane 5 l0c code
     */
    UINT32 lane6 : 8;
    /* lane6 - Bits[23:16], RW1CS, default = 8'b0 
       lane 6 l0c code
     */
    UINT32 lane7 : 8;
    /* lane7 - Bits[31:24], RW1CS, default = 8'b0 
       lane 7 l0c code
     */
  } Bits;
  UINT32 Data;
} KTIPH_RXL0C_ERR_LOG1_KTI_CIOPHYDFX_STRUCT;


/* KTIPH_RXRESEED_ERR_KTI_CIOPHYDFX_REG supported on:                         */
/*       SKX_A0 (0x403729FC)                                                  */
/*       SKX (0x403729FC)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPH_RXRESEED_ERR_KTI_CIOPHYDFX_REG 0x090249FC
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Rx Reseed Error
 */
typedef union {
  struct {
    UINT32 lane : 20;
    /* lane - Bits[19:0], RW1CS, default = 20'h0 
       Lane encoutered an error in sync bits after reseeding
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPH_RXRESEED_ERR_KTI_CIOPHYDFX_STRUCT;








/* KTIPH_L0PEXIT_ERR_KTI_CIOPHYDFX_REG supported on:                          */
/*       SKX_A0 (0x40372A10)                                                  */
/*       SKX (0x40372A10)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPH_L0PEXIT_ERR_KTI_CIOPHYDFX_REG 0x09024A10
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI L0p Exit Error
 */
typedef union {
  struct {
    UINT32 lane : 20;
    /* lane - Bits[19:0], RW1CS_LBV, default = 20'h0 
       Physical Rx lane associated with L0p Exit error
     */
    UINT32 rsvd_20 : 12;
  } Bits;
  UINT32 Data;
} KTIPH_L0PEXIT_ERR_KTI_CIOPHYDFX_STRUCT;




/* KTIPHYMODLCK_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX_A0 (0x40372B00)                                                  */
/*       SKX (0x40372B00)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPHYMODLCK_KTI_CIOPHYDFX_REG 0x09024B00
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Intel UPI Modified Lock State
 */
typedef union {
  struct {
    UINT32 privdbglck_errinjlock : 1;
    /* privdbglck_errinjlock - Bits[0:0], ROS_V, default = 1'b0 
       Privdbg modified lock status
     */
    UINT32 reutenglck_errinjlock : 1;
    /* reutenglck_errinjlock - Bits[1:1], ROS_V, default = 1'b0 
       Reuteng modified lock status (CTR1, CTR2, TDC, RDC)
     */
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPHYMODLCK_KTI_CIOPHYDFX_STRUCT;




/* PXPCAP_F2_KTI_CIOPHYDFX_REG supported on:                                  */
/*       SKX (0x40372040)                                                     */
/* Register default value:              0x00920010                            */
#define PXPCAP_F2_KTI_CIOPHYDFX_REG 0x09024040
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
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
    /* capability_version - Bits[19:16], RO, default = 4'h2 
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
} PXPCAP_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD0_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372044)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD0_F2_KTI_CIOPHYDFX_REG 0x09024044
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD0_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD1_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372048)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD1_F2_KTI_CIOPHYDFX_REG 0x09024048
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD1_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD2_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x4037204C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD2_F2_KTI_CIOPHYDFX_REG 0x0902404C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD2_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD3_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372050)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD3_F2_KTI_CIOPHYDFX_REG 0x09024050
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD3_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD4_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372054)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD4_F2_KTI_CIOPHYDFX_REG 0x09024054
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD4_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD5_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372058)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD5_F2_KTI_CIOPHYDFX_REG 0x09024058
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD5_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD6_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x4037205C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD6_F2_KTI_CIOPHYDFX_REG 0x0902405C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD6_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD7_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372060)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD7_F2_KTI_CIOPHYDFX_REG 0x09024060
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD7_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD8_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372064)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD8_F2_KTI_CIOPHYDFX_REG 0x09024064
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD8_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD9_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372068)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD9_F2_KTI_CIOPHYDFX_REG 0x09024068
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD9_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD10_F2_KTI_CIOPHYDFX_REG supported on:                              */
/*       SKX (0x4037206C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD10_F2_KTI_CIOPHYDFX_REG 0x0902406C
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD10_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD11_F2_KTI_CIOPHYDFX_REG supported on:                              */
/*       SKX (0x40372070)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD11_F2_KTI_CIOPHYDFX_REG 0x09024070
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD11_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD12_F2_KTI_CIOPHYDFX_REG supported on:                              */
/*       SKX (0x40372074)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD12_F2_KTI_CIOPHYDFX_REG 0x09024074
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD12_F2_KTI_CIOPHYDFX_STRUCT;


/* PXP_RSVD13_F2_KTI_CIOPHYDFX_REG supported on:                              */
/*       SKX (0x40372078)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD13_F2_KTI_CIOPHYDFX_REG 0x09024078
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * Reserved for PCI Express Capability
 */
typedef union {
  struct {
    UINT32 rsvd : 32;
    /* rsvd - Bits[31:0], RO, default = 32'b0 
       reserved
     */
  } Bits;
  UINT32 Data;
} PXP_RSVD13_F2_KTI_CIOPHYDFX_STRUCT;


/* PXPENHCAP_F2_KTI_CIOPHYDFX_REG supported on:                               */
/*       SKX (0x40372100)                                                     */
/* Register default value:              0x00000000                            */
#define PXPENHCAP_F2_KTI_CIOPHYDFX_REG 0x09024100
/* Struct format extracted from XML file SKX\3.14.2.CFG.xml.
 * PCI Express Enhanced Capability Register
 */
typedef union {
  struct {
    UINT32 capability_id : 16;
    /* capability_id - Bits[15:0], RO, default = 16'b0 
       There is no capability at this location.
     */
    UINT32 capability_version : 4;
    /* capability_version - Bits[19:16], RO, default = 4'b0 
       There is no capability at this location.
     */
    UINT32 next_capability_offset : 12;
    /* next_capability_offset - Bits[31:20], RO, default = 12'b0 
       Pointer to the next capability in the enhanced configuraton space.  Set to 0 to 
       indicate there are no more capability structures. 
     */
  } Bits;
  UINT32 Data;
} PXPENHCAP_F2_KTI_CIOPHYDFX_STRUCT;


#endif /* KTI_CIOPHYDFX_h */
