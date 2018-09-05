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

#ifndef MCIO_DDRIOEXT_h
#define MCIO_DDRIOEXT_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* MCIO_DDRIOEXT_DEV 22                                                       */
/* MCIO_DDRIOEXT_FUN 1                                                        */

/* VID_MCIO_DDRIOEXT_REG supported on:                                        */
/*       SKX_A0 (0x202B1000)                                                  */
/*       SKX (0x202B1000)                                                     */
/* Register default value:              0x8086                                */
#define VID_MCIO_DDRIOEXT_REG 0x08012000
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} VID_MCIO_DDRIOEXT_STRUCT;


/* DID_MCIO_DDRIOEXT_REG supported on:                                        */
/*       SKX_A0 (0x202B1002)                                                  */
/*       SKX (0x202B1002)                                                     */
/* Register default value:              0x2069                                */
#define DID_MCIO_DDRIOEXT_REG 0x08012002
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2069 
        
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
} DID_MCIO_DDRIOEXT_STRUCT;


/* PCICMD_MCIO_DDRIOEXT_REG supported on:                                     */
/*       SKX_A0 (0x202B1004)                                                  */
/*       SKX (0x202B1004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_MCIO_DDRIOEXT_REG 0x08012004
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} PCICMD_MCIO_DDRIOEXT_STRUCT;


/* PCISTS_MCIO_DDRIOEXT_REG supported on:                                     */
/*       SKX_A0 (0x202B1006)                                                  */
/*       SKX (0x202B1006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_MCIO_DDRIOEXT_REG 0x08012006
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} PCISTS_MCIO_DDRIOEXT_STRUCT;


/* RID_MCIO_DDRIOEXT_REG supported on:                                        */
/*       SKX_A0 (0x102B1008)                                                  */
/*       SKX (0x102B1008)                                                     */
/* Register default value:              0x00                                  */
#define RID_MCIO_DDRIOEXT_REG 0x08011008
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} RID_MCIO_DDRIOEXT_STRUCT;


/* CCR_N0_MCIO_DDRIOEXT_REG supported on:                                     */
/*       SKX_A0 (0x102B1009)                                                  */
/*       SKX (0x102B1009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_MCIO_DDRIOEXT_REG 0x08011009
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_MCIO_DDRIOEXT_STRUCT;


/* CCR_N1_MCIO_DDRIOEXT_REG supported on:                                     */
/*       SKX_A0 (0x202B100A)                                                  */
/*       SKX (0x202B100A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_MCIO_DDRIOEXT_REG 0x0801200A
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} CCR_N1_MCIO_DDRIOEXT_STRUCT;


/* CLSR_MCIO_DDRIOEXT_REG supported on:                                       */
/*       SKX_A0 (0x102B100C)                                                  */
/*       SKX (0x102B100C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_MCIO_DDRIOEXT_REG 0x0801100C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} CLSR_MCIO_DDRIOEXT_STRUCT;


/* PLAT_MCIO_DDRIOEXT_REG supported on:                                       */
/*       SKX_A0 (0x102B100D)                                                  */
/*       SKX (0x102B100D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_MCIO_DDRIOEXT_REG 0x0801100D
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} PLAT_MCIO_DDRIOEXT_STRUCT;


/* HDR_MCIO_DDRIOEXT_REG supported on:                                        */
/*       SKX_A0 (0x102B100E)                                                  */
/*       SKX (0x102B100E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_MCIO_DDRIOEXT_REG 0x0801100E
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} HDR_MCIO_DDRIOEXT_STRUCT;


/* BIST_MCIO_DDRIOEXT_REG supported on:                                       */
/*       SKX_A0 (0x102B100F)                                                  */
/*       SKX (0x102B100F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_MCIO_DDRIOEXT_REG 0x0801100F
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} BIST_MCIO_DDRIOEXT_STRUCT;


/* CAPPTR_MCIO_DDRIOEXT_REG supported on:                                     */
/*       SKX_A0 (0x102B1034)                                                  */
/*       SKX (0x102B1034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_MCIO_DDRIOEXT_REG 0x08011034
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} CAPPTR_MCIO_DDRIOEXT_STRUCT;


/* INTL_MCIO_DDRIOEXT_REG supported on:                                       */
/*       SKX_A0 (0x102B103C)                                                  */
/*       SKX (0x102B103C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_MCIO_DDRIOEXT_REG 0x0801103C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} INTL_MCIO_DDRIOEXT_STRUCT;


/* INTPIN_MCIO_DDRIOEXT_REG supported on:                                     */
/*       SKX_A0 (0x102B103D)                                                  */
/*       SKX (0x102B103D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_MCIO_DDRIOEXT_REG 0x0801103D
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} INTPIN_MCIO_DDRIOEXT_STRUCT;


/* MINGNT_MCIO_DDRIOEXT_REG supported on:                                     */
/*       SKX_A0 (0x102B103E)                                                  */
/*       SKX (0x102B103E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_MCIO_DDRIOEXT_REG 0x0801103E
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} MINGNT_MCIO_DDRIOEXT_STRUCT;


/* MAXLAT_MCIO_DDRIOEXT_REG supported on:                                     */
/*       SKX_A0 (0x102B103F)                                                  */
/*       SKX (0x102B103F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_MCIO_DDRIOEXT_REG 0x0801103F
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
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
} MAXLAT_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1200)                                                  */
/*       SKX (0x402B1200)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK0_6_MCIO_DDRIOEXT_REG 0x08014200
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 or CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS (0: 1/16 UI 
       offset,  
                 1: 3/16 UI Offset, 2: 5/16 UI Offsets, 3: 7/16 UI Offsets, 4: 9/16 UI 
       Offsets, 5,6,7: 11/16 UI Offsets.) 
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1204)                                                  */
/*       SKX (0x402B1204)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK1_6_MCIO_DDRIOEXT_REG 0x08014204
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank1. Multicast read for CH012 or CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 5'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1208)                                                  */
/*       SKX (0x402B1208)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK2_6_MCIO_DDRIOEXT_REG 0x08014208
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank2. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 5'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B120C)                                                  */
/*       SKX (0x402B120C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK3_6_MCIO_DDRIOEXT_REG 0x0801420C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank3. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1210)                                                  */
/*       SKX (0x402B1210)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK4_6_MCIO_DDRIOEXT_REG 0x08014210
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank4. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1214)                                                  */
/*       SKX (0x402B1214)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK5_6_MCIO_DDRIOEXT_REG 0x08014214
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank5. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       mux select for crossover from ClkPiRef to CkPi4RcvEn, used in xover2to2 mode
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1218)                                                  */
/*       SKX (0x402B1218)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK6_6_MCIO_DDRIOEXT_REG 0x08014218
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank6. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B121C)                                                  */
/*       SKX (0x402B121C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK7_6_MCIO_DDRIOEXT_REG 0x0801421C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank7. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1220)                                                  */
/*       SKX (0x402B1220)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK0_6_MCIO_DDRIOEXT_REG 0x08014220
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1224)                                                  */
/*       SKX (0x402B1224)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK1_6_MCIO_DDRIOEXT_REG 0x08014224
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1228)                                                  */
/*       SKX (0x402B1228)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK2_6_MCIO_DDRIOEXT_REG 0x08014228
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B122C)                                                  */
/*       SKX (0x402B122C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK3_6_MCIO_DDRIOEXT_REG 0x0801422C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1230)                                                  */
/*       SKX (0x402B1230)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK4_6_MCIO_DDRIOEXT_REG 0x08014230
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1234)                                                  */
/*       SKX (0x402B1234)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK5_6_MCIO_DDRIOEXT_REG 0x08014234
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1238)                                                  */
/*       SKX (0x402B1238)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK6_6_MCIO_DDRIOEXT_REG 0x08014238
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B123C)                                                  */
/*       SKX (0x402B123C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK7_6_MCIO_DDRIOEXT_REG 0x0801423C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1240)                                                  */
/*       SKX (0x402B1240)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK0_6_MCIO_DDRIOEXT_REG 0x08014240
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       select one of the four DQ Clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1244)                                                  */
/*       SKX (0x402B1244)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK1_6_MCIO_DDRIOEXT_REG 0x08014244
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1248)                                                  */
/*       SKX (0x402B1248)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK2_6_MCIO_DDRIOEXT_REG 0x08014248
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B124C)                                                  */
/*       SKX (0x402B124C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK3_6_MCIO_DDRIOEXT_REG 0x0801424C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1250)                                                  */
/*       SKX (0x402B1250)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK4_6_MCIO_DDRIOEXT_REG 0x08014250
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1254)                                                  */
/*       SKX (0x402B1254)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK5_6_MCIO_DDRIOEXT_REG 0x08014254
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 1'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 3;
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1258)                                                  */
/*       SKX (0x402B1258)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK6_6_MCIO_DDRIOEXT_REG 0x08014258
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B125C)                                                  */
/*       SKX (0x402B125C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK7_6_MCIO_DDRIOEXT_REG 0x0801425C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1260)                                                  */
/*       SKX (0x402B1260)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK0_6_MCIO_DDRIOEXT_REG 0x08014260
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte. 0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1264)                                                  */
/*       SKX (0x402B1264)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK1_6_MCIO_DDRIOEXT_REG 0x08014264
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1268)                                                  */
/*       SKX (0x402B1268)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK2_6_MCIO_DDRIOEXT_REG 0x08014268
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B126C)                                                  */
/*       SKX (0x402B126C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK3_6_MCIO_DDRIOEXT_REG 0x0801426C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1270)                                                  */
/*       SKX (0x402B1270)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK4_6_MCIO_DDRIOEXT_REG 0x08014270
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1274)                                                  */
/*       SKX (0x402B1274)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK5_6_MCIO_DDRIOEXT_REG 0x08014274
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1278)                                                  */
/*       SKX (0x402B1278)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK6_6_MCIO_DDRIOEXT_REG 0x08014278
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B127C)                                                  */
/*       SKX (0x402B127C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK7_6_MCIO_DDRIOEXT_REG 0x0801427C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1280)                                                  */
/*       SKX (0x402B1280)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK0_6_MCIO_DDRIOEXT_REG 0x08014280
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       selects which skewed DQSP (to sample even bits) is used in the RX FIFO. For 
       proper operation, always pick latest DQSP 
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       selects which skewed DQSN (to sample odd bits) is used in the RX FIFO. For 
       proper operation, always pick latest DQSN 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1284)                                                  */
/*       SKX (0x402B1284)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK1_6_MCIO_DDRIOEXT_REG 0x08014284
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1288)                                                  */
/*       SKX (0x402B1288)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK2_6_MCIO_DDRIOEXT_REG 0x08014288
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B128C)                                                  */
/*       SKX (0x402B128C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK3_6_MCIO_DDRIOEXT_REG 0x0801428C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1290)                                                  */
/*       SKX (0x402B1290)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK4_6_MCIO_DDRIOEXT_REG 0x08014290
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1294)                                                  */
/*       SKX (0x402B1294)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK5_6_MCIO_DDRIOEXT_REG 0x08014294
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1298)                                                  */
/*       SKX (0x402B1298)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK6_6_MCIO_DDRIOEXT_REG 0x08014298
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B129C)                                                  */
/*       SKX (0x402B129C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK7_6_MCIO_DDRIOEXT_REG 0x0801429C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* TXXTALKN0_6_MCIO_DDRIOEXT_REG supported on:                                */
/*       SKX_A0 (0x402B12A0)                                                  */
/*       SKX (0x402B12A0)                                                     */
/* Register default value:              0x00000000                            */
#define TXXTALKN0_6_MCIO_DDRIOEXT_REG 0x080142A0
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * TX Crosstalk Cancellation Per Lane Write CoEfficents. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 twototwologicdelayrank0 : 4;
    /* twototwologicdelayrank0 - Bits[3:0], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 0
     */
    UINT32 twototwologicdelayrank1 : 4;
    /* twototwologicdelayrank1 - Bits[7:4], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 1 
     */
    UINT32 twototwologicdelayrank2 : 4;
    /* twototwologicdelayrank2 - Bits[11:8], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 2
     */
    UINT32 twototwologicdelayrank3 : 4;
    /* twototwologicdelayrank3 - Bits[15:12], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 3 
     */
    UINT32 twototwologicdelayrank4 : 4;
    /* twototwologicdelayrank4 - Bits[19:16], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 4
     */
    UINT32 twototwologicdelayrank5 : 4;
    /* twototwologicdelayrank5 - Bits[23:20], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 5
     */
    UINT32 twototwologicdelayrank6 : 4;
    /* twototwologicdelayrank6 - Bits[27:24], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 6
     */
    UINT32 twototwologicdelayrank7 : 4;
    /* twototwologicdelayrank7 - Bits[31:28], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 7
     */
  } Bits;
  UINT32 Data;
} TXXTALKN0_6_MCIO_DDRIOEXT_STRUCT;


/* RXVREFCTRLN0_6_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B12A4)                                                  */
/*       SKX (0x402B12A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN0_6_MCIO_DDRIOEXT_REG 0x080142A4
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * RxVrefCtrl. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vrefperbit0 : 7;
    /* vrefperbit0 - Bits[6:0], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben0 : 1;
    /* halflsben0 - Bits[7:7], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit1 : 7;
    /* vrefperbit1 - Bits[14:8], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben1 : 1;
    /* halflsben1 - Bits[15:15], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefperbit2 : 7;
    /* vrefperbit2 - Bits[22:16], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben2 : 1;
    /* halflsben2 - Bits[23:23], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefperbit3 : 7;
    /* vrefperbit3 - Bits[30:24], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben3 : 1;
    /* halflsben3 - Bits[31:31], RW, default = 1'b0 
       shift down the Vref voltage by ~0.5 stepsize
     */
  } Bits;
  UINT32 Data;
} RXVREFCTRLN0_6_MCIO_DDRIOEXT_STRUCT;


/* DATATRAINFEEDBACKN0_6_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B12A8)                                                  */
/*       SKX (0x402B12A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN0_6_MCIO_DDRIOEXT_REG 0x080142A8
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 18;
    /* datatrainfeedback - Bits[17:0], RW, default = 1'b0 
       Data Training Feedback Storage
       
               DataTrainFeedback Description:
                Training Step    Bits    Nibble    Description
               --------------    ----    -------  
       -------------------------------------------------------------------------------------------------------------------------------------------------------- 
               Cross Over Cal     0      0        Tx Dqs Phase Detection
               Cross Over Cal     1      0        Tx Dq Phase Detection
               Cross Over Cal     2      0        Rec En Phase Detection
               RcvEn              8:0    0        Byte detection
               Rx Dq-Dqs          0      0        Pass/fail
               Wr Leveling        8:0    0        Byte detection
       
               Cross Over Cal     3      1        Tx Dqs Phase Detection
               Cross Over Cal     4      1        Tx Dq Phase Detection
               Cross Over Cal     5      1        Rec En Phase Detection
               RcvEn             17:9    1        Byte detection
               Rx Dq-Dqs          1      1        Pass/fail
               Wr Leveling       17:9    1        Byte detection
               
     */
    UINT32 rsvd_18 : 14;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN0_6_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 9;
    /* datatrainfeedback - Bits[8:0], RW, default = 1'b0 
       Data Training Feedback Storage
       
               DataTrainFeedback Description:
                Training Step    Bits    Nibble    Description
               --------------    ----    -------  --------------------------
               Cross Over Cal     0      0        Tx Dqs Phase Detection
               Cross Over Cal     1      0        Tx Dq Phase Detection
               Cross Over Cal     2      0        Rec En Phase Detection
               RcvEn              8:0    0        Byte detection
               Wr Leveling        8:0    0        Byte detection
               Senseamp offset    3:0    0        Dq Senseamp offset Detection 
       
               
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN0_6_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL4N0_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B12CC)                                                  */
/*       SKX (0x402B12CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4N0_6_MCIO_DDRIOEXT_REG 0x080142CC
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 vsshiorvrefctl1 : 8;
    /* vsshiorvrefctl1 - Bits[8:1], RW, default = 8'b0 
        VssHi control bits
               Field Description: 
       
                Field             Bits    Description
               -------            ----    ---------------------------------
       	 disoverflow        24     Disable overflow logic from BDW
         fast panic enable  25     Bypass 2 flops in panic path for faster panic 
       response 
       	 vsshivccd2by3cfgen 26     Forces the driver DAC to get supply from 2/3 vccd
         vsshivccd2by3cfg   27:29  Current trim setting for 2/3 vccd mode, 001 fast, 
       011 typical, 111 slow silicon  
       	 ddrvssh2ndcompen   30     Enable second comparator (ZBB mode)
         comp high bwen     31     Improve comparator performance to support low 
       digital voltage range 
       	
     */
    UINT32 dqtxeqsel : 1;
    /* dqtxeqsel - Bits[9:9], RW, default = 1'b0 
       DQ TxEq setting selection to achieve constant EQ
     */
    UINT32 rsvd_10 : 2;
    UINT32 vrefpwrsave : 1;
    /* vrefpwrsave - Bits[12:12], RW, default = None 
        
                If set to 0 (default), Vref generators selected through RxVrefSel[3:0] 
       are always on. 
                If set to 1, Vref generators are switched off while in Self Refresh.
               
     */
    UINT32 vreftonblbus : 4;
    /* vreftonblbus - Bits[16:13], RW, default = 4'b1000 
       Per-bit Enable connection to share Vref Bus
     */
    UINT32 rxvrefsel : 4;
    /* rxvrefsel - Bits[20:17], RW, default = 4'b1111 
       Per Bit Verf generator Enable
     */
    UINT32 refpiclkdelay : 6;
    /* refpiclkdelay - Bits[26:21], RW, default = 6'b000000 
       RefPiClkDelay
     */
    UINT32 fnvcrsdlbypassen : 1;
    /* fnvcrsdlbypassen - Bits[27:27], RW, default = 1'b0 
       FNV Slave Delay Line Bypass Enable
     */
    UINT32 fnvcrdllbypassen : 1;
    /* fnvcrdllbypassen - Bits[28:28], RW, default = 1'b0 
       FNV DLL Bypass Enable 
     */
    UINT32 fnvcrvisaen : 1;
    /* fnvcrvisaen - Bits[29:29], RW, default = 1'b0 
       FNV Visa Fub Enable - data fub used for VISA if asserted 
     */
    UINT32 writecrcenable : 1;
    /* writecrcenable - Bits[30:30], RW, default = None 
       Enable write crc mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DATACONTROL4N0_6_MCIO_DDRIOEXT_STRUCT;


/* ATTACK1SELN0_6_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B12D0)                                                  */
/*       SKX (0x402B12D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SELN0_6_MCIO_DDRIOEXT_REG 0x080142D0
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr1d0 : 3;
    /* attackr1d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[0] in a byte
     */
    UINT32 attackr1d1 : 3;
    /* attackr1d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[1] in a byte
     */
    UINT32 attackr1d2 : 3;
    /* attackr1d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[2] in a byte
     */
    UINT32 attackr1d3 : 3;
    /* attackr1d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[3] in a byte
     */
    UINT32 attackr1d4 : 3;
    /* attackr1d4 - Bits[14:12], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[4] in a byte
     */
    UINT32 attackr1d5 : 3;
    /* attackr1d5 - Bits[17:15], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[5] in a byte
     */
    UINT32 attackr1d6 : 3;
    /* attackr1d6 - Bits[20:18], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[6] in a byte
     */
    UINT32 attackr1d7 : 3;
    /* attackr1d7 - Bits[23:21], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK1SELN0_6_MCIO_DDRIOEXT_STRUCT;






/* DATAOFFSETTRAINN0_6_MCIO_DDRIOEXT_REG supported on:                        */
/*       SKX_A0 (0x402B12DC)                                                  */
/*       SKX (0x402B12DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAINN0_6_MCIO_DDRIOEXT_REG 0x080142DC
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rcvenoffset : 6;
    /* rcvenoffset - Bits[5:0], RW, default = 1'b0 
       Offset added to Trained RxRcvEn Value.
     */
    UINT32 rxdqsoffset : 7;
    /* rxdqsoffset - Bits[12:6], RW, default = 1'b0 
       Offset added to Trained Rx DQS Value.
     */
    UINT32 txdqoffset : 6;
    /* txdqoffset - Bits[18:13], RW, default = 2'b00 
       Offset added to Trained Tx DQ Value.
     */
    UINT32 txdqsoffset : 6;
    /* txdqsoffset - Bits[24:19], RW, default = 6'b000000 
       Ooffset added to Trained Tx DQS Value.
     */
    UINT32 vrefoffset : 7;
    /* vrefoffset - Bits[31:25], RW, default = 7'b0000000 
       2s Compliment offset added to Trained Vref Value.  Positive number increases 
       Vref, and each step is Vdd/384. 
     */
  } Bits;
  UINT32 Data;
} DATAOFFSETTRAINN0_6_MCIO_DDRIOEXT_STRUCT;




/* ATTACK0SELN0_6_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B12E4)                                                  */
/*       SKX (0x402B12E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SELN0_6_MCIO_DDRIOEXT_REG 0x080142E4
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 attackr0d4 : 3;
    /* attackr0d4 - Bits[14:12], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[4] in a byte
     */
    UINT32 attackr0d5 : 3;
    /* attackr0d5 - Bits[17:15], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[5] in a byte
     */
    UINT32 attackr0d6 : 3;
    /* attackr0d6 - Bits[20:18], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[6] in a byte
     */
    UINT32 attackr0d7 : 3;
    /* attackr0d7 - Bits[23:21], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK0SELN0_6_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 txddjcen : 4;
    /* txddjcen - Bits[15:12], RW, default = 3'b000 
       TxddjcEn
     */
    UINT32 dfeenable : 1;
    /* dfeenable - Bits[16:16], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfedlysel : 1;
    /* dfedlysel - Bits[17:17], RW, default = 1'b0 
       Dfe Delay Selection
     */
    UINT32 dfecode : 4;
    /* dfecode - Bits[21:18], RW, default = 4'b0000 
       binary code 0000 to 1000 for DFE offset, mid-code code for zero offset is 0100
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} ATTACK0SELN0_6_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL0N0_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B12E8)                                                  */
/*       SKX (0x402B12E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0N0_6_MCIO_DDRIOEXT_REG 0x080142E8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxtrainingmode : 1;
    /* rxtrainingmode - Bits[0:0], RW, default = 1'b0 
       Enables Read DqDqs Training Mode
     */
    UINT32 wltrainingmode : 1;
    /* wltrainingmode - Bits[1:1], RW, default = 1'b0 
       Enables Write Leveling Training Mode
     */
    UINT32 rltrainingmode : 1;
    /* rltrainingmode - Bits[2:2], RW, default = 1'b0 
       Enables Read Leveling Training Mode.
     */
    UINT32 senseamptrainingmode : 1;
    /* senseamptrainingmode - Bits[3:3], RW, default = 1'b0 
       Enables SenseAmp offset cancellation training mode.
     */
    UINT32 txon : 1;
    /* txon - Bits[4:4], RW, default = 1'b0 
       When set, forces DQ/DQS drive enable to active.  Used in power measurements and 
       IO loopback mode  
     */
    UINT32 rfon : 1;
    /* rfon - Bits[5:5], RW, default = 1'b0 
       Power-down Disable:  Forces register file read, overriding the rank power down 
       logic. 
     */
    UINT32 rxpion : 1;
    /* rxpion - Bits[6:6], RW, default = 1'b0 
       Power-down Disable:  Forces RxDqs PI clocks on.
     */
    UINT32 txpion : 1;
    /* txpion - Bits[7:7], RW, default = 1'b0 
       Power-down Disable:  Forces TxDq and TxDqs PI clocks on.
     */
    UINT32 internalclockson : 1;
    /* internalclockson - Bits[8:8], RW, default = 1'b0 
       
                 Forces on master DLL and all PI enables ON, despite both channel in 
       SelfRefresh, etc. low power states. 
               
     */
    UINT32 saoffsettraintxon : 1;
    /* saoffsettraintxon - Bits[9:9], RW, default = 1'b0 
       SaOffsetTrainTxon
     */
    UINT32 txdisable : 1;
    /* txdisable - Bits[10:10], RW, default = 1'b0 
       Power-down:  All write commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 rxdisable : 1;
    /* rxdisable - Bits[11:11], RW, default = 1'b0 
       Power-down:  All read commands are ignored for this data-byte.  Used in 
       single-channel MCI mode.  
     */
    UINT32 txlong : 1;
    /* txlong - Bits[12:12], RW, default = 1'b0 
       Sets the duration of the first transmitted DQ bit of the burst. 0=1UI, 1=2UI.
     */
    UINT32 rsvd_13 : 5;
    UINT32 driversegmentenable : 1;
    /* driversegmentenable - Bits[18:18], RW, default = 1'b0 
       Controls which segements of the driver are enabled:  {0: All, 1: Only ODT}.
     */
    UINT32 rsvd_19 : 1;
    UINT32 readrfrd : 1;
    /* readrfrd - Bits[20:20], RW_V, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a read command to Rank = ReadRFRank 
     */
    UINT32 readrfwr : 1;
    /* readrfwr - Bits[21:21], RW_V, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a write command to Rank = ReadRFRank 
     */
    UINT32 readrfrank : 3;
    /* readrfrank - Bits[24:22], RW_V, default = 3'b000 
       Specifies the rank that ReadRFRd or ReadRFWr will manually download the CR 
       values from 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[25:25], RW, default = 1'b0 
       ODT is forced-on.
     */
    UINT32 odtsampoff : 1;
    /* odtsampoff - Bits[26:26], RW, default = 1'b0 
       ODT and Senseamp are forced-off.
     */
    UINT32 disableodtstatic : 1;
    /* disableodtstatic - Bits[27:27], RW, default = 1'b0 
       not used
     */
    UINT32 ddrcrforceodton : 1;
    /* ddrcrforceodton - Bits[28:28], RW, default = 1'b0 
       force ODT to the transmitter on, regardless of state of DriveEnable
     */
    UINT32 spare : 1;
    /* spare - Bits[29:29], RW, default = 1'b0 
       spare
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[30:30], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
    UINT32 longpreambleenable : 1;
    /* longpreambleenable - Bits[31:31], RW, default = 1'b0 
       Enables long preamble for DDR4 devices
     */
  } Bits;
  UINT32 Data;
} DATACONTROL0N0_6_MCIO_DDRIOEXT_STRUCT;


/* DATACONTROL1N0_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B12EC)                                                  */
/*       SKX (0x402B12EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1N0_6_MCIO_DDRIOEXT_REG 0x080142EC
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 burstlength10 : 1;
    /* burstlength10 - Bits[5:5], RW, default = 1'b0 
       reserved
     */
    UINT32 rsvd_6 : 4;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 1'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 4'b0000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 4'b0000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N0_6_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 xtalkpienable : 1;
    /* xtalkpienable - Bits[0:0], RW, default = 1'b0 
       enable phase shift based crosstalk cancellation.
     */
    UINT32 xtalkpisign : 1;
    /* xtalkpisign - Bits[1:1], RW, default = 1'b0 
       select if 0: capacitive coupling; or 1: inductive coupling; xtalk dominant
     */
    UINT32 xtalkpidelta : 3;
    /* xtalkpidelta - Bits[4:2], RW, default = 3'b0 
       delta phase shift in one aggressor is in effect
     */
    UINT32 xtalkpideltarank5 : 2;
    /* xtalkpideltarank5 - Bits[6:5], RW, default = 2'b0 
       XtalkPiDeltaRank5
     */
    UINT32 rsvd_7 : 3;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 1'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 4'b0000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 4'b0000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N0_6_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL2N0_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B12F0)                                                  */
/*       SKX (0x402B12F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2N0_6_MCIO_DDRIOEXT_REG 0x080142F0
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier). 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency  
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = None 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = None 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = None 
       Enable Imode EQ per byte lane. 
                 0: Imode completely off 
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 spare : 4;
    /* spare - Bits[14:11], RW, default = None 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = None 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = None 
       Binary Imode Coefficient, sets Imode current strength.
                 (used for either Xtalk cancellation or swing boost)
                 Code   Imode Current (approx)
                 ------------------
                 0000        0
                 0001        1
                 0010        2
                   *
                   *
                   *
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = None 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N0_6_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier). 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency  
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = None 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = None 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = None 
       Enable Imode EQ per byte lane. 
                 0: Imode completely off 
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 enearlywriterankqnnnh : 1;
    /* enearlywriterankqnnnh - Bits[11:11], RW, default = None 
       Spare
     */
    UINT32 picodepipeqnnnh : 1;
    /* picodepipeqnnnh - Bits[12:12], RW, default = None 
       Spare
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW, default = None 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = None 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = None 
       Binary Imode Coefficient, sets Imode current strength.
                 (used for either Xtalk cancellation or swing boost)
                 Code   Imode Current (approx)
                 ------------------
                 0000        0
                 0001        1
                 0010        2
                   *
                   *
                   *
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = None 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N0_6_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL3N0_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B12F4)                                                  */
/*       SKX (0x402B12F4)                                                     */
/* Register default value on SKX_A0:    0x8184891FCF                          */
/* Register default value on SKX:       0x00000FCF                            */
#define DATACONTROL3N0_6_MCIO_DDRIOEXT_REG 0x080142F4
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 drvstaticlegcfg : 2;
    /* drvstaticlegcfg - Bits[1:0], RW, default = 2'b11 
       binary default setting for static legs in DRV mode
                  0    0       0 legs (EQ disabled)
                  0    1       3 legs (EQ max 3 legs)
                  1    0       6 legs (EQ max 6 legs)
                  1    1      12 legs (EQ max 12 legs)
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher  
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit  
                 the higher Ron target value.
     */
    UINT32 odtstaticlegcfg : 2;
    /* odtstaticlegcfg - Bits[3:2], RW, default = 2'b11 
       binary default setting for static legs in ODT mode
                  0    0       0 legs (EQ disabled)
                  0    1       3 legs (EQ max 3 legs)
                  1    0       6 legs (EQ max 6 legs)
                  1    1      12 legs (EQ max 12 legs)
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher  
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit  
                 the higher Ron target value.
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[4:4], RW, default = 1'b0 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rsvd_5 : 1;
    UINT32 odtsegmentenable : 3;
    /* odtsegmentenable - Bits[8:6], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.  
                            After that, a shift register will determine the enabled 
       segments 
     */
    UINT32 datasegmentenable : 3;
    /* datasegmentenable - Bits[11:9], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset. 
     */
    UINT32 longodtr2w : 1;
    /* longodtr2w - Bits[12:12], RW, default = 3'b000 
       When set, it will extends ODT by 1 qclk on read to write turnarounds. hsd 230773
     */
    UINT32 spare2 : 1;
    /* spare2 - Bits[13:13], RW, default = 1'b0 
       Spare2
     */
    UINT32 imodebiasvrefen : 1;
    /* imodebiasvrefen - Bits[14:14], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 imodebiasrxbiastrim : 3;
    /* imodebiasrxbiastrim - Bits[17:15], RW, default = 3'b000 
       imodebias control. Also used for rxbiasana 
     */
    UINT32 spare : 1;
    /* spare - Bits[18:18], RW, default = 1'b0 
       Spare
     */
    UINT32 imodebiasdfxddr4legup : 1;
    /* imodebiasdfxddr4legup - Bits[19:19], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 imodebiasdfxlegdn : 1;
    /* imodebiasdfxlegdn - Bits[20:20], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 ddrcrctlecapen : 2;
    /* ddrcrctlecapen - Bits[22:21], RW, default = 2'b00 
       ctle capacitor setting 
     */
    UINT32 ddrcrctleresen : 2;
    /* ddrcrctleresen - Bits[24:23], RW, default = 2'b00 
       ctle resistor setting 
     */
    UINT32 pcasbiasclosedloopen : 1;
    /* pcasbiasclosedloopen - Bits[25:25], RW, default = 1'b0 
       RxBias CR 
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[26:26], RW, default = 1'b0 
       RxBias CR 
     */
    UINT32 rxbiasfoldedlegtrim : 2;
    /* rxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b00 
       RxBias CR 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[30:29], RW, default = 2'b00 
       RxBias CR 
     */
    UINT32 postambleenable : 1;
    /* postambleenable - Bits[31:31], RW, default = None 
       Spare
     */
  } Bits;
  UINT32 Data;
} DATACONTROL3N0_6_MCIO_DDRIOEXT_STRUCT;


/* VSSHIORVREFCONTROLN0_6_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B12F8)                                                  */
/*       SKX (0x402B12F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROLN0_6_MCIO_DDRIOEXT_REG 0x080142F8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings. 
       
               VssHi Field Description: 
       
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode          23     Selects the code to output:  {0: Code, 1: TCode}
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode    
               PanicVoltage  21:18  Unsigned integer controlling the voltage error on 
       VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 117mV) 
               CloseLoop        17     Enables the VssHi close loop tracking 
               PanicEn          16     Enables the VssHi Panic Driver
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError) 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128} 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
       
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROLN0_6_MCIO_DDRIOEXT_STRUCT;




/* RXGROUP0N1RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1300)                                                  */
/*       SKX (0x402B1300)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK0_6_MCIO_DDRIOEXT_REG 0x08014300
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1304)                                                  */
/*       SKX (0x402B1304)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK1_6_MCIO_DDRIOEXT_REG 0x08014304
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1308)                                                  */
/*       SKX (0x402B1308)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK2_6_MCIO_DDRIOEXT_REG 0x08014308
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0.Multicast read for CH012 and CH345 versions 
 * are not supported.  
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B130C)                                                  */
/*       SKX (0x402B130C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK3_6_MCIO_DDRIOEXT_REG 0x0801430C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1310)                                                  */
/*       SKX (0x402B1310)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK4_6_MCIO_DDRIOEXT_REG 0x08014310
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1314)                                                  */
/*       SKX (0x402B1314)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK5_6_MCIO_DDRIOEXT_REG 0x08014314
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1318)                                                  */
/*       SKX (0x402B1318)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK6_6_MCIO_DDRIOEXT_REG 0x08014318
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B131C)                                                  */
/*       SKX (0x402B131C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK7_6_MCIO_DDRIOEXT_REG 0x0801431C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1320)                                                  */
/*       SKX (0x402B1320)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK0_6_MCIO_DDRIOEXT_REG 0x08014320
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1324)                                                  */
/*       SKX (0x402B1324)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK1_6_MCIO_DDRIOEXT_REG 0x08014324
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1328)                                                  */
/*       SKX (0x402B1328)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK2_6_MCIO_DDRIOEXT_REG 0x08014328
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B132C)                                                  */
/*       SKX (0x402B132C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK3_6_MCIO_DDRIOEXT_REG 0x0801432C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1330)                                                  */
/*       SKX (0x402B1330)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK4_6_MCIO_DDRIOEXT_REG 0x08014330
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1334)                                                  */
/*       SKX (0x402B1334)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK5_6_MCIO_DDRIOEXT_REG 0x08014334
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1338)                                                  */
/*       SKX (0x402B1338)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK6_6_MCIO_DDRIOEXT_REG 0x08014338
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B133C)                                                  */
/*       SKX (0x402B133C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK7_6_MCIO_DDRIOEXT_REG 0x0801433C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1340)                                                  */
/*       SKX (0x402B1340)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK0_6_MCIO_DDRIOEXT_REG 0x08014340
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the four clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1344)                                                  */
/*       SKX (0x402B1344)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK1_6_MCIO_DDRIOEXT_REG 0x08014344
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the four clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1348)                                                  */
/*       SKX (0x402B1348)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK2_6_MCIO_DDRIOEXT_REG 0x08014348
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B134C)                                                  */
/*       SKX (0x402B134C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK3_6_MCIO_DDRIOEXT_REG 0x0801434C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1350)                                                  */
/*       SKX (0x402B1350)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK4_6_MCIO_DDRIOEXT_REG 0x08014350
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       select one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1354)                                                  */
/*       SKX (0x402B1354)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK5_6_MCIO_DDRIOEXT_REG 0x08014354
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 1'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1358)                                                  */
/*       SKX (0x402B1358)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK6_6_MCIO_DDRIOEXT_REG 0x08014358
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 1'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B135C)                                                  */
/*       SKX (0x402B135C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK7_6_MCIO_DDRIOEXT_REG 0x0801435C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1360)                                                  */
/*       SKX (0x402B1360)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK0_6_MCIO_DDRIOEXT_REG 0x08014360
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1364)                                                  */
/*       SKX (0x402B1364)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK1_6_MCIO_DDRIOEXT_REG 0x08014364
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1368)                                                  */
/*       SKX (0x402B1368)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK2_6_MCIO_DDRIOEXT_REG 0x08014368
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B136C)                                                  */
/*       SKX (0x402B136C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK3_6_MCIO_DDRIOEXT_REG 0x0801436C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1370)                                                  */
/*       SKX (0x402B1370)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK4_6_MCIO_DDRIOEXT_REG 0x08014370
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1374)                                                  */
/*       SKX (0x402B1374)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK5_6_MCIO_DDRIOEXT_REG 0x08014374
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1378)                                                  */
/*       SKX (0x402B1378)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK6_6_MCIO_DDRIOEXT_REG 0x08014378
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B137C)                                                  */
/*       SKX (0x402B137C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK7_6_MCIO_DDRIOEXT_REG 0x0801437C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK7_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK0_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1380)                                                  */
/*       SKX (0x402B1380)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK0_6_MCIO_DDRIOEXT_REG 0x08014380
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK0_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK1_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1384)                                                  */
/*       SKX (0x402B1384)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK1_6_MCIO_DDRIOEXT_REG 0x08014384
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK1_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK2_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1388)                                                  */
/*       SKX (0x402B1388)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK2_6_MCIO_DDRIOEXT_REG 0x08014388
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK2_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK3_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B138C)                                                  */
/*       SKX (0x402B138C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK3_6_MCIO_DDRIOEXT_REG 0x0801438C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK3_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK4_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1390)                                                  */
/*       SKX (0x402B1390)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK4_6_MCIO_DDRIOEXT_REG 0x08014390
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK4_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK5_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1394)                                                  */
/*       SKX (0x402B1394)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK5_6_MCIO_DDRIOEXT_REG 0x08014394
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK5_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK6_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1398)                                                  */
/*       SKX (0x402B1398)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK6_6_MCIO_DDRIOEXT_REG 0x08014398
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK6_6_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK7_6_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B139C)                                                  */
/*       SKX (0x402B139C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK7_6_MCIO_DDRIOEXT_REG 0x0801439C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK7_6_MCIO_DDRIOEXT_STRUCT;




/* RXVREFCTRLN1_6_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B13A4)                                                  */
/*       SKX (0x402B13A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN1_6_MCIO_DDRIOEXT_REG 0x080143A4
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * RxVrefCtrl. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vrefperbit0 : 7;
    /* vrefperbit0 - Bits[6:0], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben0 : 1;
    /* halflsben0 - Bits[7:7], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit1 : 7;
    /* vrefperbit1 - Bits[14:8], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben1 : 1;
    /* halflsben1 - Bits[15:15], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit2 : 7;
    /* vrefperbit2 - Bits[22:16], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben2 : 1;
    /* halflsben2 - Bits[23:23], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit3 : 7;
    /* vrefperbit3 - Bits[30:24], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben3 : 1;
    /* halflsben3 - Bits[31:31], RW, default = 1'b0 
       shift down the Vref voltage by ~0.5 stepsize
     */
  } Bits;
  UINT32 Data;
} RXVREFCTRLN1_6_MCIO_DDRIOEXT_STRUCT;


/* DATATRAINFEEDBACKN1_6_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B13A8)                                                  */
/*       SKX (0x402B13A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN1_6_MCIO_DDRIOEXT_REG 0x080143A8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 9;
    /* datatrainfeedback - Bits[8:0], RW, default = 1'b0 
       Data Training Feedback Storage 
                Training Step    Bits    Nibble    Description
               --------------    ----    -------  ---------------
               Cross Over Cal     0      1        Tx Dqs Phase Detection
               Cross Over Cal     1      1        Tx Dq Phase Detection
               Cross Over Cal     2      1        Rec En Phase Detection
               RcvEn              8:0    1        Byte detection
               Wr Leveling        8:0    1        Byte detection
               Senseamp offset    3:0    1        Dq Senseamp offset Detection
       
               
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN1_6_MCIO_DDRIOEXT_STRUCT;


/* DATACONTROL4N1_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B13CC)                                                  */
/*       SKX (0x402B13CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4N1_6_MCIO_DDRIOEXT_REG 0x080143CC
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 vsshiorvrefctl1 : 8;
    /* vsshiorvrefctl1 - Bits[8:1], RW, default = 8'b0 
        VssHi control bits
               Field Description: 
       
                Field             Bits    Description
               -------            ----    ---------------------------------
       	 disoverflow        24     Disable overflow logic from BDW
         fast panic enable  25     Bypass 2 flops in panic path for faster panic 
       response 
       	 vsshivccd2by3cfgen 26     Forces the driver DAC to get supply from 2/3 vccd
         vsshivccd2by3cfg   27:29  Current trim setting for 2/3 vccd mode, 001 fast, 
       011 typical, 111 slow silicon  
       	 ddrvssh2ndcompen   30     Enable second comparator (ZBB mode)
         comp high bwen     31     Improve comparator performance to support low 
       digital voltage range 
       	
     */
    UINT32 dqtxeqsel : 1;
    /* dqtxeqsel - Bits[9:9], RW, default = 1'b0 
       DQ TxEq setting selection to achieve constant EQ
     */
    UINT32 rsvd_10 : 2;
    UINT32 vrefpwrsave : 1;
    /* vrefpwrsave - Bits[12:12], RW, default = None 
        
                If set to 0 (default), Vref generators selected through RxVrefSel[3:0] 
       are always on. 
                If set to 1, Vref generators are switched off while in Self Refresh.
               
     */
    UINT32 vreftonblbus : 4;
    /* vreftonblbus - Bits[16:13], RW, default = 4'b1000 
       Per-bit Enable connection to share Vref Bus
     */
    UINT32 rxvrefsel : 4;
    /* rxvrefsel - Bits[20:17], RW, default = 4'b1111 
       Per Bit Verf generator Enable
     */
    UINT32 refpiclkdelay : 6;
    /* refpiclkdelay - Bits[26:21], RW, default = 6'b000000 
       RefPiClkDelay
     */
    UINT32 fnvcrsdlbypassen : 1;
    /* fnvcrsdlbypassen - Bits[27:27], RW, default = 1'b0 
       FNV Slave Delay Line Bypass Enable
     */
    UINT32 fnvcrdllbypassen : 1;
    /* fnvcrdllbypassen - Bits[28:28], RW, default = 1'b0 
       FNV DLL Bypass Enable 
     */
    UINT32 fnvcrvisaen : 1;
    /* fnvcrvisaen - Bits[29:29], RW, default = 1'b0 
       FNV Visa Fub Enable - data fub used for VISA if asserted 
     */
    UINT32 writecrcenable : 1;
    /* writecrcenable - Bits[30:30], RW, default = None 
       Enable write crc mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DATACONTROL4N1_6_MCIO_DDRIOEXT_STRUCT;


/* ATTACK1SELN1_6_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B13D0)                                                  */
/*       SKX (0x402B13D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SELN1_6_MCIO_DDRIOEXT_REG 0x080143D0
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr1d0 : 3;
    /* attackr1d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[0] in a byte
     */
    UINT32 attackr1d1 : 3;
    /* attackr1d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[1] in a byte
     */
    UINT32 attackr1d2 : 3;
    /* attackr1d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[2] in a byte
     */
    UINT32 attackr1d3 : 3;
    /* attackr1d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[3] in a byte
     */
    UINT32 attackr1d4 : 3;
    /* attackr1d4 - Bits[14:12], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[4] in a byte
     */
    UINT32 attackr1d5 : 3;
    /* attackr1d5 - Bits[17:15], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[5] in a byte
     */
    UINT32 attackr1d6 : 3;
    /* attackr1d6 - Bits[20:18], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[6] in a byte
     */
    UINT32 attackr1d7 : 3;
    /* attackr1d7 - Bits[23:21], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK1SELN1_6_MCIO_DDRIOEXT_STRUCT;






/* DATAOFFSETTRAINN1_6_MCIO_DDRIOEXT_REG supported on:                        */
/*       SKX_A0 (0x402B13DC)                                                  */
/*       SKX (0x402B13DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAINN1_6_MCIO_DDRIOEXT_REG 0x080143DC
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rcvenoffset : 6;
    /* rcvenoffset - Bits[5:0], RW, default = 1'b0 
       Offset added to Trained RxRcvEn Value.
     */
    UINT32 rxdqsoffset : 7;
    /* rxdqsoffset - Bits[12:6], RW, default = 1'b0 
       Offset added to Trained Rx DQS Value.
     */
    UINT32 txdqoffset : 6;
    /* txdqoffset - Bits[18:13], RW, default = 2'b00 
       Offset added to Trained Tx DQ Value.
     */
    UINT32 txdqsoffset : 6;
    /* txdqsoffset - Bits[24:19], RW, default = 6'b000000 
       Ooffset added to Trained Tx DQS Value.
     */
    UINT32 vrefoffset : 7;
    /* vrefoffset - Bits[31:25], RW, default = 7'b0000000 
       2s Compliment offset added to Trained Vref Value.  Positive number increases 
       Vref, and each step is Vdd/384. 
     */
  } Bits;
  UINT32 Data;
} DATAOFFSETTRAINN1_6_MCIO_DDRIOEXT_STRUCT;




/* ATTACK0SELN1_6_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B13E4)                                                  */
/*       SKX (0x402B13E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SELN1_6_MCIO_DDRIOEXT_REG 0x080143E4
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 attackr0d4 : 3;
    /* attackr0d4 - Bits[14:12], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[4] in a byte
     */
    UINT32 attackr0d5 : 3;
    /* attackr0d5 - Bits[17:15], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[5] in a byte
     */
    UINT32 attackr0d6 : 3;
    /* attackr0d6 - Bits[20:18], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[6] in a byte
     */
    UINT32 attackr0d7 : 3;
    /* attackr0d7 - Bits[23:21], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK0SELN1_6_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 txddjcen : 4;
    /* txddjcen - Bits[15:12], RW, default = 3'b000 
       TxddjcEn
     */
    UINT32 dfeenable : 1;
    /* dfeenable - Bits[16:16], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfedlysel : 1;
    /* dfedlysel - Bits[17:17], RW, default = 1'b0 
       Dfe Delay Selection
     */
    UINT32 dfecode : 4;
    /* dfecode - Bits[21:18], RW, default = 4'b0000 
       binary code 0000 to 1000 for DFE offset, mid-code code for zero offset is 0100
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} ATTACK0SELN1_6_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL0N1_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B13E8)                                                  */
/*       SKX (0x402B13E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0N1_6_MCIO_DDRIOEXT_REG 0x080143E8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxtrainingmode : 1;
    /* rxtrainingmode - Bits[0:0], RW, default = 1'b0 
       Enables Read DqDqs Training Mode
     */
    UINT32 wltrainingmode : 1;
    /* wltrainingmode - Bits[1:1], RW, default = 1'b0 
       Enables Write Leveling Training Mode
     */
    UINT32 rltrainingmode : 1;
    /* rltrainingmode - Bits[2:2], RW, default = 1'b0 
       Enables Read Leveling Training Mode.
     */
    UINT32 senseamptrainingmode : 1;
    /* senseamptrainingmode - Bits[3:3], RW, default = 1'b0 
       Enables SenseAmp offset cancellation training mode.
     */
    UINT32 txon : 1;
    /* txon - Bits[4:4], RW, default = 1'b0 
       When set, forces DQ/DQS drive enable to active.  Used in power measurements and 
       IO loopback mode  
     */
    UINT32 rfon : 1;
    /* rfon - Bits[5:5], RW, default = 1'b0 
       Power-down Disable:  Forces register file read, overriding the rank power down 
       logic. 
     */
    UINT32 rxpion : 1;
    /* rxpion - Bits[6:6], RW, default = 1'b0 
       Power-down Disable:  Forces RxDqs PI clocks on.
     */
    UINT32 txpion : 1;
    /* txpion - Bits[7:7], RW, default = 1'b0 
       Power-down Disable:  Forces TxDq and TxDqs PI clocks on.
     */
    UINT32 internalclockson : 1;
    /* internalclockson - Bits[8:8], RW, default = 1'b0 
       
                 Forces on master DLL and all PI enables ON, despite both channel in 
       SelfRefresh, etc. low power states. 
               
     */
    UINT32 saoffsettraintxon : 1;
    /* saoffsettraintxon - Bits[9:9], RW, default = 1'b0 
       SaOffsetTrainTxon
     */
    UINT32 txdisable : 1;
    /* txdisable - Bits[10:10], RW, default = 1'b0 
       Power-down:  All write commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 rxdisable : 1;
    /* rxdisable - Bits[11:11], RW, default = 1'b0 
       Power-down:  All read commands are ignored for this data-byte.  Used in 
       single-channel MCI mode.  
     */
    UINT32 txlong : 1;
    /* txlong - Bits[12:12], RW, default = 1'b0 
       Sets the duration of the first transmitted DQ bit of the burst. 0=1UI, 1=2UI.
     */
    UINT32 rsvd_13 : 5;
    UINT32 driversegmentenable : 1;
    /* driversegmentenable - Bits[18:18], RW, default = 1'b0 
       Controls which segements of the driver are enabled:  {0: All, 1: Only ODT}.
     */
    UINT32 rsvd_19 : 1;
    UINT32 readrfrd : 1;
    /* readrfrd - Bits[20:20], RW_V, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a read command to Rank = ReadRFRank 
     */
    UINT32 readrfwr : 1;
    /* readrfwr - Bits[21:21], RW_V, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a write command to Rank = ReadRFRank 
     */
    UINT32 readrfrank : 3;
    /* readrfrank - Bits[24:22], RW_V, default = 3'b000 
       Specifies the rank that ReadRFRd or ReadRFWr will manually download the CR 
       values from 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[25:25], RW, default = 1'b0 
       ODT is forced-on.
     */
    UINT32 odtsampoff : 1;
    /* odtsampoff - Bits[26:26], RW, default = 1'b0 
       ODT and Senseamp are forced-off.
     */
    UINT32 disableodtstatic : 1;
    /* disableodtstatic - Bits[27:27], RW, default = 1'b0 
       not used
     */
    UINT32 ddrcrforceodton : 1;
    /* ddrcrforceodton - Bits[28:28], RW, default = 1'b0 
       force ODT to the transmitter on, regardless of state of DriveEnable
     */
    UINT32 spare : 1;
    /* spare - Bits[29:29], RW, default = 1'b0 
       spare
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[30:30], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
    UINT32 longpreambleenable : 1;
    /* longpreambleenable - Bits[31:31], RW, default = 1'b0 
       Enables long preamble for DDR4 devices
     */
  } Bits;
  UINT32 Data;
} DATACONTROL0N1_6_MCIO_DDRIOEXT_STRUCT;


/* DATACONTROL1N1_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B13EC)                                                  */
/*       SKX (0x402B13EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1N1_6_MCIO_DDRIOEXT_REG 0x080143EC
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 burstlength10 : 1;
    /* burstlength10 - Bits[5:5], RW, default = 1'b0 
       reserved
     */
    UINT32 rsvd_6 : 4;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 1'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 4'b0000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 4'b0000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N1_6_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 xtalkpienable : 1;
    /* xtalkpienable - Bits[0:0], RW, default = 1'b0 
       enable phase shift based crosstalk cancellation.
     */
    UINT32 xtalkpisign : 1;
    /* xtalkpisign - Bits[1:1], RW, default = 1'b0 
       select if 0: capacitive coupling; or 1: inductive coupling; xtalk dominant
     */
    UINT32 xtalkpidelta : 3;
    /* xtalkpidelta - Bits[4:2], RW, default = 3'b0 
       delta phase shift in one aggressor is in effect
     */
    UINT32 xtalkpideltarank5 : 2;
    /* xtalkpideltarank5 - Bits[6:5], RW, default = 2'b0 
       XtalkPiDeltaRank5
     */
    UINT32 rsvd_7 : 3;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 1'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 4'b0000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 4'b0000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N1_6_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL2N1_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B13F0)                                                  */
/*       SKX (0x402B13F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2N1_6_MCIO_DDRIOEXT_REG 0x080143F0
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier). 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency  
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = None 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = None 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = None 
       Enable Imode EQ per byte lane. 
                 0: Imode completely off 
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 spare : 4;
    /* spare - Bits[14:11], RW, default = None 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = None 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = None 
       Binary Imode Coefficient, sets Imode current strength.
                 (used for either Xtalk cancellation or swing boost)
                 Code   Imode Current (approx)
                 ------------------
                 0000        0
                 0001        1
                 0010        2
                   *
                   *
                   *
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = None 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N1_6_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier). 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency  
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = None 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = None 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = None 
       Enable Imode EQ per byte lane. 
                 0: Imode completely off 
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 enearlywriterankqnnnh : 1;
    /* enearlywriterankqnnnh - Bits[11:11], RW, default = None 
       Spare
     */
    UINT32 picodepipeqnnnh : 1;
    /* picodepipeqnnnh - Bits[12:12], RW, default = None 
       Spare
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW, default = None 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = None 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = None 
       Binary Imode Coefficient, sets Imode current strength.
                 (used for either Xtalk cancellation or swing boost)
                 Code   Imode Current (approx)
                 ------------------
                 0000        0
                 0001        1
                 0010        2
                   *
                   *
                   *
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = None 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N1_6_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL3N1_6_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B13F4)                                                  */
/*       SKX (0x402B13F4)                                                     */
/* Register default value on SKX_A0:    0x8184891FCF                          */
/* Register default value on SKX:       0x00000FCF                            */
#define DATACONTROL3N1_6_MCIO_DDRIOEXT_REG 0x080143F4
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 drvstaticlegcfg : 2;
    /* drvstaticlegcfg - Bits[1:0], RW, default = 2'b11 
       binary default setting for static legs in DRV mode
                  0    0       0 legs (EQ disabled)
                  0    1       3 legs (EQ max 3 legs)
                  1    0       6 legs (EQ max 6 legs)
                  1    1      12 legs (EQ max 12 legs)
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher  
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit  
                 the higher Ron target value.
     */
    UINT32 odtstaticlegcfg : 2;
    /* odtstaticlegcfg - Bits[3:2], RW, default = 2'b11 
       binary default setting for static legs in ODT mode
                  0    0       0 legs (EQ disabled)
                  0    1       3 legs (EQ max 3 legs)
                  1    0       6 legs (EQ max 6 legs)
                  1    1      12 legs (EQ max 12 legs)
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher  
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit  
                 the higher Ron target value.
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[4:4], RW, default = 1'b0 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rsvd_5 : 1;
    UINT32 odtsegmentenable : 3;
    /* odtsegmentenable - Bits[8:6], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.  
                            After that, a shift register will determine the enabled 
       segments 
     */
    UINT32 datasegmentenable : 3;
    /* datasegmentenable - Bits[11:9], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset. 
     */
    UINT32 longodtr2w : 1;
    /* longodtr2w - Bits[12:12], RW, default = 3'b000 
       When set, it will extends ODT by 1 qclk on read to write turnarounds. hsd 230773
     */
    UINT32 spare2 : 1;
    /* spare2 - Bits[13:13], RW, default = 1'b0 
       Spare2
     */
    UINT32 imodebiasvrefen : 1;
    /* imodebiasvrefen - Bits[14:14], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 imodebiasrxbiastrim : 3;
    /* imodebiasrxbiastrim - Bits[17:15], RW, default = 3'b000 
       imodebias control. Also used for rxbiasana 
     */
    UINT32 spare : 1;
    /* spare - Bits[18:18], RW, default = 1'b0 
       Spare
     */
    UINT32 imodebiasdfxddr4legup : 1;
    /* imodebiasdfxddr4legup - Bits[19:19], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 imodebiasdfxlegdn : 1;
    /* imodebiasdfxlegdn - Bits[20:20], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 ddrcrctlecapen : 2;
    /* ddrcrctlecapen - Bits[22:21], RW, default = 2'b00 
       ctle capacitor setting 
     */
    UINT32 ddrcrctleresen : 2;
    /* ddrcrctleresen - Bits[24:23], RW, default = 2'b00 
       ctle resistor setting 
     */
    UINT32 pcasbiasclosedloopen : 1;
    /* pcasbiasclosedloopen - Bits[25:25], RW, default = 1'b0 
       RxBias CR 
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[26:26], RW, default = 1'b0 
       RxBias CR 
     */
    UINT32 rxbiasfoldedlegtrim : 2;
    /* rxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b00 
       RxBias CR 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[30:29], RW, default = 2'b00 
       RxBias CR 
     */
    UINT32 postambleenable : 1;
    /* postambleenable - Bits[31:31], RW, default = None 
       Spare
     */
  } Bits;
  UINT32 Data;
} DATACONTROL3N1_6_MCIO_DDRIOEXT_STRUCT;


/* VSSHIORVREFCONTROLN1_6_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B13F8)                                                  */
/*       SKX (0x402B13F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROLN1_6_MCIO_DDRIOEXT_REG 0x080143F8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings. 
       
               VssHi Field Description: 
       
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode          23     Selects the code to output:  {0: Code, 1: TCode}
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode    
               PanicVoltage  21:18  Unsigned integer controlling the voltage error on 
       VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 117mV) 
               CloseLoop        17     Enables the VssHi close loop tracking 
               PanicEn          16     Enables the VssHi Panic Driver
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError) 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128} 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
       
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROLN1_6_MCIO_DDRIOEXT_STRUCT;




/* RXGROUP0N0RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1400)                                                  */
/*       SKX (0x402B1400)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK0_7_MCIO_DDRIOEXT_REG 0x08014400
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 or CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS (0: 1/16 UI 
       offset,  
                 1: 3/16 UI Offset, 2: 5/16 UI Offsets, 3: 7/16 UI Offsets, 4: 9/16 UI 
       Offsets, 5,6,7: 11/16 UI Offsets.) 
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1404)                                                  */
/*       SKX (0x402B1404)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK1_7_MCIO_DDRIOEXT_REG 0x08014404
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank1. Multicast read for CH012 or CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 5'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1408)                                                  */
/*       SKX (0x402B1408)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK2_7_MCIO_DDRIOEXT_REG 0x08014408
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank2. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 5'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B140C)                                                  */
/*       SKX (0x402B140C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK3_7_MCIO_DDRIOEXT_REG 0x0801440C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank3. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1410)                                                  */
/*       SKX (0x402B1410)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK4_7_MCIO_DDRIOEXT_REG 0x08014410
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank4. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1414)                                                  */
/*       SKX (0x402B1414)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK5_7_MCIO_DDRIOEXT_REG 0x08014414
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank5. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       mux select for crossover from ClkPiRef to CkPi4RcvEn, used in xover2to2 mode
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1418)                                                  */
/*       SKX (0x402B1418)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK6_7_MCIO_DDRIOEXT_REG 0x08014418
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank6. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N0RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B141C)                                                  */
/*       SKX (0x402B141C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK7_7_MCIO_DDRIOEXT_REG 0x0801441C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank7. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms) 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
               
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1420)                                                  */
/*       SKX (0x402B1420)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK0_7_MCIO_DDRIOEXT_REG 0x08014420
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       per bit READ timing control,
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
               
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1424)                                                  */
/*       SKX (0x402B1424)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK1_7_MCIO_DDRIOEXT_REG 0x08014424
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1428)                                                  */
/*       SKX (0x402B1428)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK2_7_MCIO_DDRIOEXT_REG 0x08014428
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B142C)                                                  */
/*       SKX (0x402B142C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK3_7_MCIO_DDRIOEXT_REG 0x0801442C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1430)                                                  */
/*       SKX (0x402B1430)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK4_7_MCIO_DDRIOEXT_REG 0x08014430
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1434)                                                  */
/*       SKX (0x402B1434)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK5_7_MCIO_DDRIOEXT_REG 0x08014434
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1438)                                                  */
/*       SKX (0x402B1438)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK6_7_MCIO_DDRIOEXT_REG 0x08014438
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N0RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B143C)                                                  */
/*       SKX (0x402B143C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK7_7_MCIO_DDRIOEXT_REG 0x0801443C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1440)                                                  */
/*       SKX (0x402B1440)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK0_7_MCIO_DDRIOEXT_REG 0x08014440
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       select one of the four DQ Clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1444)                                                  */
/*       SKX (0x402B1444)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK1_7_MCIO_DDRIOEXT_REG 0x08014444
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1448)                                                  */
/*       SKX (0x402B1448)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK2_7_MCIO_DDRIOEXT_REG 0x08014448
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B144C)                                                  */
/*       SKX (0x402B144C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK3_7_MCIO_DDRIOEXT_REG 0x0801444C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1450)                                                  */
/*       SKX (0x402B1450)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK4_7_MCIO_DDRIOEXT_REG 0x08014450
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1454)                                                  */
/*       SKX (0x402B1454)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK5_7_MCIO_DDRIOEXT_REG 0x08014454
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 1'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 3;
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1458)                                                  */
/*       SKX (0x402B1458)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK6_7_MCIO_DDRIOEXT_REG 0x08014458
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N0RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B145C)                                                  */
/*       SKX (0x402B145C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK7_7_MCIO_DDRIOEXT_REG 0x0801445C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1460)                                                  */
/*       SKX (0x402B1460)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK0_7_MCIO_DDRIOEXT_REG 0x08014460
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte. 0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1464)                                                  */
/*       SKX (0x402B1464)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK1_7_MCIO_DDRIOEXT_REG 0x08014464
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1468)                                                  */
/*       SKX (0x402B1468)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK2_7_MCIO_DDRIOEXT_REG 0x08014468
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B146C)                                                  */
/*       SKX (0x402B146C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK3_7_MCIO_DDRIOEXT_REG 0x0801446C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1470)                                                  */
/*       SKX (0x402B1470)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK4_7_MCIO_DDRIOEXT_REG 0x08014470
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1474)                                                  */
/*       SKX (0x402B1474)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK5_7_MCIO_DDRIOEXT_REG 0x08014474
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1478)                                                  */
/*       SKX (0x402B1478)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK6_7_MCIO_DDRIOEXT_REG 0x08014478
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N0RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B147C)                                                  */
/*       SKX (0x402B147C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK7_7_MCIO_DDRIOEXT_REG 0x0801447C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1480)                                                  */
/*       SKX (0x402B1480)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK0_7_MCIO_DDRIOEXT_REG 0x08014480
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       selects which skewed DQSP (to sample even bits) is used in the RX FIFO. For 
       proper operation, always pick latest DQSP 
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       selects which skewed DQSN (to sample odd bits) is used in the RX FIFO. For 
       proper operation, always pick latest DQSN 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1484)                                                  */
/*       SKX (0x402B1484)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK1_7_MCIO_DDRIOEXT_REG 0x08014484
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1488)                                                  */
/*       SKX (0x402B1488)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK2_7_MCIO_DDRIOEXT_REG 0x08014488
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B148C)                                                  */
/*       SKX (0x402B148C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK3_7_MCIO_DDRIOEXT_REG 0x0801448C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1490)                                                  */
/*       SKX (0x402B1490)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK4_7_MCIO_DDRIOEXT_REG 0x08014490
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1494)                                                  */
/*       SKX (0x402B1494)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK5_7_MCIO_DDRIOEXT_REG 0x08014494
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1498)                                                  */
/*       SKX (0x402B1498)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK6_7_MCIO_DDRIOEXT_REG 0x08014498
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN0RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B149C)                                                  */
/*       SKX (0x402B149C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK7_7_MCIO_DDRIOEXT_REG 0x0801449C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* TXXTALKN0_7_MCIO_DDRIOEXT_REG supported on:                                */
/*       SKX_A0 (0x402B14A0)                                                  */
/*       SKX (0x402B14A0)                                                     */
/* Register default value:              0x00000000                            */
#define TXXTALKN0_7_MCIO_DDRIOEXT_REG 0x080144A0
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * TX Crosstalk Cancellation Per Lane Write CoEfficents. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 twototwologicdelayrank0 : 4;
    /* twototwologicdelayrank0 - Bits[3:0], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 0
     */
    UINT32 twototwologicdelayrank1 : 4;
    /* twototwologicdelayrank1 - Bits[7:4], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 1 
     */
    UINT32 twototwologicdelayrank2 : 4;
    /* twototwologicdelayrank2 - Bits[11:8], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 2
     */
    UINT32 twototwologicdelayrank3 : 4;
    /* twototwologicdelayrank3 - Bits[15:12], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 3 
     */
    UINT32 twototwologicdelayrank4 : 4;
    /* twototwologicdelayrank4 - Bits[19:16], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 4
     */
    UINT32 twototwologicdelayrank5 : 4;
    /* twototwologicdelayrank5 - Bits[23:20], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 5
     */
    UINT32 twototwologicdelayrank6 : 4;
    /* twototwologicdelayrank6 - Bits[27:24], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 6
     */
    UINT32 twototwologicdelayrank7 : 4;
    /* twototwologicdelayrank7 - Bits[31:28], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 7
     */
  } Bits;
  UINT32 Data;
} TXXTALKN0_7_MCIO_DDRIOEXT_STRUCT;


/* RXVREFCTRLN0_7_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B14A4)                                                  */
/*       SKX (0x402B14A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN0_7_MCIO_DDRIOEXT_REG 0x080144A4
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * RxVrefCtrl. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vrefperbit0 : 7;
    /* vrefperbit0 - Bits[6:0], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben0 : 1;
    /* halflsben0 - Bits[7:7], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit1 : 7;
    /* vrefperbit1 - Bits[14:8], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben1 : 1;
    /* halflsben1 - Bits[15:15], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefperbit2 : 7;
    /* vrefperbit2 - Bits[22:16], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben2 : 1;
    /* halflsben2 - Bits[23:23], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefperbit3 : 7;
    /* vrefperbit3 - Bits[30:24], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben3 : 1;
    /* halflsben3 - Bits[31:31], RW, default = 1'b0 
       shift down the Vref voltage by ~0.5 stepsize
     */
  } Bits;
  UINT32 Data;
} RXVREFCTRLN0_7_MCIO_DDRIOEXT_STRUCT;


/* DATATRAINFEEDBACKN0_7_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B14A8)                                                  */
/*       SKX (0x402B14A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN0_7_MCIO_DDRIOEXT_REG 0x080144A8
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 18;
    /* datatrainfeedback - Bits[17:0], RW, default = 1'b0 
       Data Training Feedback Storage
       
               DataTrainFeedback Description:
                Training Step    Bits    Nibble    Description
               --------------    ----    -------  
       -------------------------------------------------------------------------------------------------------------------------------------------------------- 
               Cross Over Cal     0      0        Tx Dqs Phase Detection
               Cross Over Cal     1      0        Tx Dq Phase Detection
               Cross Over Cal     2      0        Rec En Phase Detection
               RcvEn              8:0    0        Byte detection
               Rx Dq-Dqs          0      0        Pass/fail
               Wr Leveling        8:0    0        Byte detection
       
               Cross Over Cal     3      1        Tx Dqs Phase Detection
               Cross Over Cal     4      1        Tx Dq Phase Detection
               Cross Over Cal     5      1        Rec En Phase Detection
               RcvEn             17:9    1        Byte detection
               Rx Dq-Dqs          1      1        Pass/fail
               Wr Leveling       17:9    1        Byte detection
               
     */
    UINT32 rsvd_18 : 14;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN0_7_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 9;
    /* datatrainfeedback - Bits[8:0], RW, default = 1'b0 
       Data Training Feedback Storage
       
               DataTrainFeedback Description:
                Training Step    Bits    Nibble    Description
               --------------    ----    -------  --------------------------
               Cross Over Cal     0      0        Tx Dqs Phase Detection
               Cross Over Cal     1      0        Tx Dq Phase Detection
               Cross Over Cal     2      0        Rec En Phase Detection
               RcvEn              8:0    0        Byte detection
               Wr Leveling        8:0    0        Byte detection
               Senseamp offset    3:0    0        Dq Senseamp offset Detection 
       
               
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN0_7_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL4N0_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B14CC)                                                  */
/*       SKX (0x402B14CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4N0_7_MCIO_DDRIOEXT_REG 0x080144CC
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 vsshiorvrefctl1 : 8;
    /* vsshiorvrefctl1 - Bits[8:1], RW, default = 8'b0 
        VssHi control bits
               Field Description: 
       
                Field             Bits    Description
               -------            ----    ---------------------------------
       	 disoverflow        24     Disable overflow logic from BDW
         fast panic enable  25     Bypass 2 flops in panic path for faster panic 
       response 
       	 vsshivccd2by3cfgen 26     Forces the driver DAC to get supply from 2/3 vccd
         vsshivccd2by3cfg   27:29  Current trim setting for 2/3 vccd mode, 001 fast, 
       011 typical, 111 slow silicon  
       	 ddrvssh2ndcompen   30     Enable second comparator (ZBB mode)
         comp high bwen     31     Improve comparator performance to support low 
       digital voltage range 
       	
     */
    UINT32 dqtxeqsel : 1;
    /* dqtxeqsel - Bits[9:9], RW, default = 1'b0 
       DQ TxEq setting selection to achieve constant EQ
     */
    UINT32 rsvd_10 : 2;
    UINT32 vrefpwrsave : 1;
    /* vrefpwrsave - Bits[12:12], RW, default = None 
        
                If set to 0 (default), Vref generators selected through RxVrefSel[3:0] 
       are always on. 
                If set to 1, Vref generators are switched off while in Self Refresh.
               
     */
    UINT32 vreftonblbus : 4;
    /* vreftonblbus - Bits[16:13], RW, default = 4'b1000 
       Per-bit Enable connection to share Vref Bus
     */
    UINT32 rxvrefsel : 4;
    /* rxvrefsel - Bits[20:17], RW, default = 4'b1111 
       Per Bit Verf generator Enable
     */
    UINT32 refpiclkdelay : 6;
    /* refpiclkdelay - Bits[26:21], RW, default = 6'b000000 
       RefPiClkDelay
     */
    UINT32 fnvcrsdlbypassen : 1;
    /* fnvcrsdlbypassen - Bits[27:27], RW, default = 1'b0 
       FNV Slave Delay Line Bypass Enable
     */
    UINT32 fnvcrdllbypassen : 1;
    /* fnvcrdllbypassen - Bits[28:28], RW, default = 1'b0 
       FNV DLL Bypass Enable 
     */
    UINT32 fnvcrvisaen : 1;
    /* fnvcrvisaen - Bits[29:29], RW, default = 1'b0 
       FNV Visa Fub Enable - data fub used for VISA if asserted 
     */
    UINT32 writecrcenable : 1;
    /* writecrcenable - Bits[30:30], RW, default = None 
       Enable write crc mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DATACONTROL4N0_7_MCIO_DDRIOEXT_STRUCT;


/* ATTACK1SELN0_7_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B14D0)                                                  */
/*       SKX (0x402B14D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SELN0_7_MCIO_DDRIOEXT_REG 0x080144D0
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr1d0 : 3;
    /* attackr1d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[0] in a byte
     */
    UINT32 attackr1d1 : 3;
    /* attackr1d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[1] in a byte
     */
    UINT32 attackr1d2 : 3;
    /* attackr1d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[2] in a byte
     */
    UINT32 attackr1d3 : 3;
    /* attackr1d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[3] in a byte
     */
    UINT32 attackr1d4 : 3;
    /* attackr1d4 - Bits[14:12], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[4] in a byte
     */
    UINT32 attackr1d5 : 3;
    /* attackr1d5 - Bits[17:15], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[5] in a byte
     */
    UINT32 attackr1d6 : 3;
    /* attackr1d6 - Bits[20:18], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[6] in a byte
     */
    UINT32 attackr1d7 : 3;
    /* attackr1d7 - Bits[23:21], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK1SELN0_7_MCIO_DDRIOEXT_STRUCT;






/* DATAOFFSETTRAINN0_7_MCIO_DDRIOEXT_REG supported on:                        */
/*       SKX_A0 (0x402B14DC)                                                  */
/*       SKX (0x402B14DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAINN0_7_MCIO_DDRIOEXT_REG 0x080144DC
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rcvenoffset : 6;
    /* rcvenoffset - Bits[5:0], RW, default = 1'b0 
       Offset added to Trained RxRcvEn Value.
     */
    UINT32 rxdqsoffset : 7;
    /* rxdqsoffset - Bits[12:6], RW, default = 1'b0 
       Offset added to Trained Rx DQS Value.
     */
    UINT32 txdqoffset : 6;
    /* txdqoffset - Bits[18:13], RW, default = 2'b00 
       Offset added to Trained Tx DQ Value.
     */
    UINT32 txdqsoffset : 6;
    /* txdqsoffset - Bits[24:19], RW, default = 6'b000000 
       Ooffset added to Trained Tx DQS Value.
     */
    UINT32 vrefoffset : 7;
    /* vrefoffset - Bits[31:25], RW, default = 7'b0000000 
       2s Compliment offset added to Trained Vref Value.  Positive number increases 
       Vref, and each step is Vdd/384. 
     */
  } Bits;
  UINT32 Data;
} DATAOFFSETTRAINN0_7_MCIO_DDRIOEXT_STRUCT;




/* ATTACK0SELN0_7_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B14E4)                                                  */
/*       SKX (0x402B14E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SELN0_7_MCIO_DDRIOEXT_REG 0x080144E4
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 attackr0d4 : 3;
    /* attackr0d4 - Bits[14:12], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[4] in a byte
     */
    UINT32 attackr0d5 : 3;
    /* attackr0d5 - Bits[17:15], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[5] in a byte
     */
    UINT32 attackr0d6 : 3;
    /* attackr0d6 - Bits[20:18], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[6] in a byte
     */
    UINT32 attackr0d7 : 3;
    /* attackr0d7 - Bits[23:21], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK0SELN0_7_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 txddjcen : 4;
    /* txddjcen - Bits[15:12], RW, default = 3'b000 
       TxddjcEn
     */
    UINT32 dfeenable : 1;
    /* dfeenable - Bits[16:16], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfedlysel : 1;
    /* dfedlysel - Bits[17:17], RW, default = 1'b0 
       Dfe Delay Selection
     */
    UINT32 dfecode : 4;
    /* dfecode - Bits[21:18], RW, default = 4'b0000 
       binary code 0000 to 1000 for DFE offset, mid-code code for zero offset is 0100
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} ATTACK0SELN0_7_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL0N0_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B14E8)                                                  */
/*       SKX (0x402B14E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0N0_7_MCIO_DDRIOEXT_REG 0x080144E8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxtrainingmode : 1;
    /* rxtrainingmode - Bits[0:0], RW, default = 1'b0 
       Enables Read DqDqs Training Mode
     */
    UINT32 wltrainingmode : 1;
    /* wltrainingmode - Bits[1:1], RW, default = 1'b0 
       Enables Write Leveling Training Mode
     */
    UINT32 rltrainingmode : 1;
    /* rltrainingmode - Bits[2:2], RW, default = 1'b0 
       Enables Read Leveling Training Mode.
     */
    UINT32 senseamptrainingmode : 1;
    /* senseamptrainingmode - Bits[3:3], RW, default = 1'b0 
       Enables SenseAmp offset cancellation training mode.
     */
    UINT32 txon : 1;
    /* txon - Bits[4:4], RW, default = 1'b0 
       When set, forces DQ/DQS drive enable to active.  Used in power measurements and 
       IO loopback mode  
     */
    UINT32 rfon : 1;
    /* rfon - Bits[5:5], RW, default = 1'b0 
       Power-down Disable:  Forces register file read, overriding the rank power down 
       logic. 
     */
    UINT32 rxpion : 1;
    /* rxpion - Bits[6:6], RW, default = 1'b0 
       Power-down Disable:  Forces RxDqs PI clocks on.
     */
    UINT32 txpion : 1;
    /* txpion - Bits[7:7], RW, default = 1'b0 
       Power-down Disable:  Forces TxDq and TxDqs PI clocks on.
     */
    UINT32 internalclockson : 1;
    /* internalclockson - Bits[8:8], RW, default = 1'b0 
       
                 Forces on master DLL and all PI enables ON, despite both channel in 
       SelfRefresh, etc. low power states. 
               
     */
    UINT32 saoffsettraintxon : 1;
    /* saoffsettraintxon - Bits[9:9], RW, default = 1'b0 
       SaOffsetTrainTxon
     */
    UINT32 txdisable : 1;
    /* txdisable - Bits[10:10], RW, default = 1'b0 
       Power-down:  All write commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 rxdisable : 1;
    /* rxdisable - Bits[11:11], RW, default = 1'b0 
       Power-down:  All read commands are ignored for this data-byte.  Used in 
       single-channel MCI mode.  
     */
    UINT32 txlong : 1;
    /* txlong - Bits[12:12], RW, default = 1'b0 
       Sets the duration of the first transmitted DQ bit of the burst. 0=1UI, 1=2UI.
     */
    UINT32 rsvd_13 : 5;
    UINT32 driversegmentenable : 1;
    /* driversegmentenable - Bits[18:18], RW, default = 1'b0 
       Controls which segements of the driver are enabled:  {0: All, 1: Only ODT}.
     */
    UINT32 rsvd_19 : 1;
    UINT32 readrfrd : 1;
    /* readrfrd - Bits[20:20], RW_V, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a read command to Rank = ReadRFRank 
     */
    UINT32 readrfwr : 1;
    /* readrfwr - Bits[21:21], RW_V, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a write command to Rank = ReadRFRank 
     */
    UINT32 readrfrank : 3;
    /* readrfrank - Bits[24:22], RW_V, default = 3'b000 
       Specifies the rank that ReadRFRd or ReadRFWr will manually download the CR 
       values from 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[25:25], RW, default = 1'b0 
       ODT is forced-on.
     */
    UINT32 odtsampoff : 1;
    /* odtsampoff - Bits[26:26], RW, default = 1'b0 
       ODT and Senseamp are forced-off.
     */
    UINT32 disableodtstatic : 1;
    /* disableodtstatic - Bits[27:27], RW, default = 1'b0 
       not used
     */
    UINT32 ddrcrforceodton : 1;
    /* ddrcrforceodton - Bits[28:28], RW, default = 1'b0 
       force ODT to the transmitter on, regardless of state of DriveEnable
     */
    UINT32 spare : 1;
    /* spare - Bits[29:29], RW, default = 1'b0 
       spare
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[30:30], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
    UINT32 longpreambleenable : 1;
    /* longpreambleenable - Bits[31:31], RW, default = 1'b0 
       Enables long preamble for DDR4 devices
     */
  } Bits;
  UINT32 Data;
} DATACONTROL0N0_7_MCIO_DDRIOEXT_STRUCT;


/* DATACONTROL1N0_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B14EC)                                                  */
/*       SKX (0x402B14EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1N0_7_MCIO_DDRIOEXT_REG 0x080144EC
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 burstlength10 : 1;
    /* burstlength10 - Bits[5:5], RW, default = 1'b0 
       reserved
     */
    UINT32 rsvd_6 : 4;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 1'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 4'b0000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 4'b0000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N0_7_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 xtalkpienable : 1;
    /* xtalkpienable - Bits[0:0], RW, default = 1'b0 
       enable phase shift based crosstalk cancellation.
     */
    UINT32 xtalkpisign : 1;
    /* xtalkpisign - Bits[1:1], RW, default = 1'b0 
       select if 0: capacitive coupling; or 1: inductive coupling; xtalk dominant
     */
    UINT32 xtalkpidelta : 3;
    /* xtalkpidelta - Bits[4:2], RW, default = 3'b0 
       delta phase shift in one aggressor is in effect
     */
    UINT32 xtalkpideltarank5 : 2;
    /* xtalkpideltarank5 - Bits[6:5], RW, default = 2'b0 
       XtalkPiDeltaRank5
     */
    UINT32 rsvd_7 : 3;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 1'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 4'b0000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 4'b0000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N0_7_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL2N0_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B14F0)                                                  */
/*       SKX (0x402B14F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2N0_7_MCIO_DDRIOEXT_REG 0x080144F0
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier). 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency  
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = None 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = None 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = None 
       Enable Imode EQ per byte lane. 
                 0: Imode completely off 
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 spare : 4;
    /* spare - Bits[14:11], RW, default = None 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = None 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = None 
       Binary Imode Coefficient, sets Imode current strength.
                 (used for either Xtalk cancellation or swing boost)
                 Code   Imode Current (approx)
                 ------------------
                 0000        0
                 0001        1
                 0010        2
                   *
                   *
                   *
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = None 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N0_7_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier). 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency  
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = None 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = None 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = None 
       Enable Imode EQ per byte lane. 
                 0: Imode completely off 
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 enearlywriterankqnnnh : 1;
    /* enearlywriterankqnnnh - Bits[11:11], RW, default = None 
       Spare
     */
    UINT32 picodepipeqnnnh : 1;
    /* picodepipeqnnnh - Bits[12:12], RW, default = None 
       Spare
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW, default = None 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = None 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = None 
       Binary Imode Coefficient, sets Imode current strength.
                 (used for either Xtalk cancellation or swing boost)
                 Code   Imode Current (approx)
                 ------------------
                 0000        0
                 0001        1
                 0010        2
                   *
                   *
                   *
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = None 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N0_7_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL3N0_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B14F4)                                                  */
/*       SKX (0x402B14F4)                                                     */
/* Register default value on SKX_A0:    0x8184891FCF                          */
/* Register default value on SKX:       0x00000FCF                            */
#define DATACONTROL3N0_7_MCIO_DDRIOEXT_REG 0x080144F4
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 drvstaticlegcfg : 2;
    /* drvstaticlegcfg - Bits[1:0], RW, default = 2'b11 
       binary default setting for static legs in DRV mode
                  0    0       0 legs (EQ disabled)
                  0    1       3 legs (EQ max 3 legs)
                  1    0       6 legs (EQ max 6 legs)
                  1    1      12 legs (EQ max 12 legs)
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher  
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit  
                 the higher Ron target value.
     */
    UINT32 odtstaticlegcfg : 2;
    /* odtstaticlegcfg - Bits[3:2], RW, default = 2'b11 
       binary default setting for static legs in ODT mode
                  0    0       0 legs (EQ disabled)
                  0    1       3 legs (EQ max 3 legs)
                  1    0       6 legs (EQ max 6 legs)
                  1    1      12 legs (EQ max 12 legs)
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher  
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit  
                 the higher Ron target value.
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[4:4], RW, default = 1'b0 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rsvd_5 : 1;
    UINT32 odtsegmentenable : 3;
    /* odtsegmentenable - Bits[8:6], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.  
                            After that, a shift register will determine the enabled 
       segments 
     */
    UINT32 datasegmentenable : 3;
    /* datasegmentenable - Bits[11:9], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset. 
     */
    UINT32 longodtr2w : 1;
    /* longodtr2w - Bits[12:12], RW, default = 3'b000 
       When set, it will extends ODT by 1 qclk on read to write turnarounds. hsd 230773
     */
    UINT32 spare2 : 1;
    /* spare2 - Bits[13:13], RW, default = 1'b0 
       Spare2
     */
    UINT32 imodebiasvrefen : 1;
    /* imodebiasvrefen - Bits[14:14], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 imodebiasrxbiastrim : 3;
    /* imodebiasrxbiastrim - Bits[17:15], RW, default = 3'b000 
       imodebias control. Also used for rxbiasana 
     */
    UINT32 spare : 1;
    /* spare - Bits[18:18], RW, default = 1'b0 
       Spare
     */
    UINT32 imodebiasdfxddr4legup : 1;
    /* imodebiasdfxddr4legup - Bits[19:19], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 imodebiasdfxlegdn : 1;
    /* imodebiasdfxlegdn - Bits[20:20], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 ddrcrctlecapen : 2;
    /* ddrcrctlecapen - Bits[22:21], RW, default = 2'b00 
       ctle capacitor setting 
     */
    UINT32 ddrcrctleresen : 2;
    /* ddrcrctleresen - Bits[24:23], RW, default = 2'b00 
       ctle resistor setting 
     */
    UINT32 pcasbiasclosedloopen : 1;
    /* pcasbiasclosedloopen - Bits[25:25], RW, default = 1'b0 
       RxBias CR 
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[26:26], RW, default = 1'b0 
       RxBias CR 
     */
    UINT32 rxbiasfoldedlegtrim : 2;
    /* rxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b00 
       RxBias CR 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[30:29], RW, default = 2'b00 
       RxBias CR 
     */
    UINT32 postambleenable : 1;
    /* postambleenable - Bits[31:31], RW, default = None 
       Spare
     */
  } Bits;
  UINT32 Data;
} DATACONTROL3N0_7_MCIO_DDRIOEXT_STRUCT;


/* VSSHIORVREFCONTROLN0_7_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B14F8)                                                  */
/*       SKX (0x402B14F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROLN0_7_MCIO_DDRIOEXT_REG 0x080144F8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings. 
       
               VssHi Field Description: 
       
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode          23     Selects the code to output:  {0: Code, 1: TCode}
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode    
               PanicVoltage  21:18  Unsigned integer controlling the voltage error on 
       VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 117mV) 
               CloseLoop        17     Enables the VssHi close loop tracking 
               PanicEn          16     Enables the VssHi Panic Driver
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError) 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128} 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
       
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROLN0_7_MCIO_DDRIOEXT_STRUCT;




/* RXGROUP0N1RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1500)                                                  */
/*       SKX (0x402B1500)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK0_7_MCIO_DDRIOEXT_REG 0x08014500
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1504)                                                  */
/*       SKX (0x402B1504)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK1_7_MCIO_DDRIOEXT_REG 0x08014504
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1508)                                                  */
/*       SKX (0x402B1508)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK2_7_MCIO_DDRIOEXT_REG 0x08014508
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0.Multicast read for CH012 and CH345 versions 
 * are not supported.  
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B150C)                                                  */
/*       SKX (0x402B150C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK3_7_MCIO_DDRIOEXT_REG 0x0801450C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1510)                                                  */
/*       SKX (0x402B1510)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK4_7_MCIO_DDRIOEXT_REG 0x08014510
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1514)                                                  */
/*       SKX (0x402B1514)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK5_7_MCIO_DDRIOEXT_REG 0x08014514
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1518)                                                  */
/*       SKX (0x402B1518)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK6_7_MCIO_DDRIOEXT_REG 0x08014518
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP0N1RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B151C)                                                  */
/*       SKX (0x402B151C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK7_7_MCIO_DDRIOEXT_REG 0x0801451C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 1'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 1'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 1'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 1'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 1'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff,  
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3). 
        
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1520)                                                  */
/*       SKX (0x402B1520)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK0_7_MCIO_DDRIOEXT_REG 0x08014520
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1524)                                                  */
/*       SKX (0x402B1524)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK1_7_MCIO_DDRIOEXT_REG 0x08014524
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1528)                                                  */
/*       SKX (0x402B1528)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK2_7_MCIO_DDRIOEXT_REG 0x08014528
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B152C)                                                  */
/*       SKX (0x402B152C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK3_7_MCIO_DDRIOEXT_REG 0x0801452C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1530)                                                  */
/*       SKX (0x402B1530)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK4_7_MCIO_DDRIOEXT_REG 0x08014530
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1534)                                                  */
/*       SKX (0x402B1534)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK5_7_MCIO_DDRIOEXT_REG 0x08014534
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1538)                                                  */
/*       SKX (0x402B1538)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK6_7_MCIO_DDRIOEXT_REG 0x08014538
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* RXGROUP1N1RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B153C)                                                  */
/*       SKX (0x402B153C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK7_7_MCIO_DDRIOEXT_REG 0x0801453C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 1'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1540)                                                  */
/*       SKX (0x402B1540)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK0_7_MCIO_DDRIOEXT_REG 0x08014540
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the four clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1544)                                                  */
/*       SKX (0x402B1544)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK1_7_MCIO_DDRIOEXT_REG 0x08014544
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the four clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1548)                                                  */
/*       SKX (0x402B1548)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK2_7_MCIO_DDRIOEXT_REG 0x08014548
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B154C)                                                  */
/*       SKX (0x402B154C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK3_7_MCIO_DDRIOEXT_REG 0x0801454C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1550)                                                  */
/*       SKX (0x402B1550)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK4_7_MCIO_DDRIOEXT_REG 0x08014550
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       select one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1554)                                                  */
/*       SKX (0x402B1554)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK5_7_MCIO_DDRIOEXT_REG 0x08014554
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 1'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1558)                                                  */
/*       SKX (0x402B1558)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK6_7_MCIO_DDRIOEXT_REG 0x08014558
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 1'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP0N1RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B155C)                                                  */
/*       SKX (0x402B155C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK7_7_MCIO_DDRIOEXT_REG 0x0801455C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 1'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 1'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful.
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled for deemphasized, and so on. 
                    
               
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 4'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1560)                                                  */
/*       SKX (0x402B1560)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK0_7_MCIO_DDRIOEXT_REG 0x08014560
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1564)                                                  */
/*       SKX (0x402B1564)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK1_7_MCIO_DDRIOEXT_REG 0x08014564
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1568)                                                  */
/*       SKX (0x402B1568)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK2_7_MCIO_DDRIOEXT_REG 0x08014568
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B156C)                                                  */
/*       SKX (0x402B156C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK3_7_MCIO_DDRIOEXT_REG 0x0801456C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1570)                                                  */
/*       SKX (0x402B1570)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK4_7_MCIO_DDRIOEXT_REG 0x08014570
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1574)                                                  */
/*       SKX (0x402B1574)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK5_7_MCIO_DDRIOEXT_REG 0x08014574
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1578)                                                  */
/*       SKX (0x402B1578)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK6_7_MCIO_DDRIOEXT_REG 0x08014578
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* TXGROUP1N1RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B157C)                                                  */
/*       SKX (0x402B157C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK7_7_MCIO_DDRIOEXT_REG 0x0801457C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Timing control for each lane of the data byte on Rank
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 1'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 1'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 1'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 1'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK7_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK0_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1580)                                                  */
/*       SKX (0x402B1580)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK0_7_MCIO_DDRIOEXT_REG 0x08014580
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK0_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK1_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1584)                                                  */
/*       SKX (0x402B1584)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK1_7_MCIO_DDRIOEXT_REG 0x08014584
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK1_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK2_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1588)                                                  */
/*       SKX (0x402B1588)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK2_7_MCIO_DDRIOEXT_REG 0x08014588
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK2_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK3_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B158C)                                                  */
/*       SKX (0x402B158C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK3_7_MCIO_DDRIOEXT_REG 0x0801458C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK3_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK4_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1590)                                                  */
/*       SKX (0x402B1590)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK4_7_MCIO_DDRIOEXT_REG 0x08014590
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK4_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK5_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1594)                                                  */
/*       SKX (0x402B1594)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK5_7_MCIO_DDRIOEXT_REG 0x08014594
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK5_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK6_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1598)                                                  */
/*       SKX (0x402B1598)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK6_7_MCIO_DDRIOEXT_REG 0x08014598
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK6_7_MCIO_DDRIOEXT_STRUCT;


/* RXOFFSETN1RANK7_7_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B159C)                                                  */
/*       SKX (0x402B159C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK7_7_MCIO_DDRIOEXT_REG 0x0801459C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 1'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 1'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 1'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 1'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset}  
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 1'b0 
       x4/x8 DQS mux select
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode;  
                   [0]=0 select nibble1 amplifier output in x4 mode; 
                   [1] is not used.
               
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 1'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK7_7_MCIO_DDRIOEXT_STRUCT;




/* RXVREFCTRLN1_7_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B15A4)                                                  */
/*       SKX (0x402B15A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN1_7_MCIO_DDRIOEXT_REG 0x080145A4
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * RxVrefCtrl. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vrefperbit0 : 7;
    /* vrefperbit0 - Bits[6:0], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben0 : 1;
    /* halflsben0 - Bits[7:7], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit1 : 7;
    /* vrefperbit1 - Bits[14:8], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben1 : 1;
    /* halflsben1 - Bits[15:15], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit2 : 7;
    /* vrefperbit2 - Bits[22:16], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben2 : 1;
    /* halflsben2 - Bits[23:23], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit3 : 7;
    /* vrefperbit3 - Bits[30:24], RW, default = 1'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben3 : 1;
    /* halflsben3 - Bits[31:31], RW, default = 1'b0 
       shift down the Vref voltage by ~0.5 stepsize
     */
  } Bits;
  UINT32 Data;
} RXVREFCTRLN1_7_MCIO_DDRIOEXT_STRUCT;


/* DATATRAINFEEDBACKN1_7_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B15A8)                                                  */
/*       SKX (0x402B15A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN1_7_MCIO_DDRIOEXT_REG 0x080145A8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 9;
    /* datatrainfeedback - Bits[8:0], RW, default = 1'b0 
       Data Training Feedback Storage 
                Training Step    Bits    Nibble    Description
               --------------    ----    -------  ---------------
               Cross Over Cal     0      1        Tx Dqs Phase Detection
               Cross Over Cal     1      1        Tx Dq Phase Detection
               Cross Over Cal     2      1        Rec En Phase Detection
               RcvEn              8:0    1        Byte detection
               Wr Leveling        8:0    1        Byte detection
               Senseamp offset    3:0    1        Dq Senseamp offset Detection
       
               
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN1_7_MCIO_DDRIOEXT_STRUCT;


/* DATACONTROL4N1_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B15CC)                                                  */
/*       SKX (0x402B15CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4N1_7_MCIO_DDRIOEXT_REG 0x080145CC
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 vsshiorvrefctl1 : 8;
    /* vsshiorvrefctl1 - Bits[8:1], RW, default = 8'b0 
        VssHi control bits
               Field Description: 
       
                Field             Bits    Description
               -------            ----    ---------------------------------
       	 disoverflow        24     Disable overflow logic from BDW
         fast panic enable  25     Bypass 2 flops in panic path for faster panic 
       response 
       	 vsshivccd2by3cfgen 26     Forces the driver DAC to get supply from 2/3 vccd
         vsshivccd2by3cfg   27:29  Current trim setting for 2/3 vccd mode, 001 fast, 
       011 typical, 111 slow silicon  
       	 ddrvssh2ndcompen   30     Enable second comparator (ZBB mode)
         comp high bwen     31     Improve comparator performance to support low 
       digital voltage range 
       	
     */
    UINT32 dqtxeqsel : 1;
    /* dqtxeqsel - Bits[9:9], RW, default = 1'b0 
       DQ TxEq setting selection to achieve constant EQ
     */
    UINT32 rsvd_10 : 2;
    UINT32 vrefpwrsave : 1;
    /* vrefpwrsave - Bits[12:12], RW, default = None 
        
                If set to 0 (default), Vref generators selected through RxVrefSel[3:0] 
       are always on. 
                If set to 1, Vref generators are switched off while in Self Refresh.
               
     */
    UINT32 vreftonblbus : 4;
    /* vreftonblbus - Bits[16:13], RW, default = 4'b1000 
       Per-bit Enable connection to share Vref Bus
     */
    UINT32 rxvrefsel : 4;
    /* rxvrefsel - Bits[20:17], RW, default = 4'b1111 
       Per Bit Verf generator Enable
     */
    UINT32 refpiclkdelay : 6;
    /* refpiclkdelay - Bits[26:21], RW, default = 6'b000000 
       RefPiClkDelay
     */
    UINT32 fnvcrsdlbypassen : 1;
    /* fnvcrsdlbypassen - Bits[27:27], RW, default = 1'b0 
       FNV Slave Delay Line Bypass Enable
     */
    UINT32 fnvcrdllbypassen : 1;
    /* fnvcrdllbypassen - Bits[28:28], RW, default = 1'b0 
       FNV DLL Bypass Enable 
     */
    UINT32 fnvcrvisaen : 1;
    /* fnvcrvisaen - Bits[29:29], RW, default = 1'b0 
       FNV Visa Fub Enable - data fub used for VISA if asserted 
     */
    UINT32 writecrcenable : 1;
    /* writecrcenable - Bits[30:30], RW, default = None 
       Enable write crc mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DATACONTROL4N1_7_MCIO_DDRIOEXT_STRUCT;


/* ATTACK1SELN1_7_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B15D0)                                                  */
/*       SKX (0x402B15D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SELN1_7_MCIO_DDRIOEXT_REG 0x080145D0
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr1d0 : 3;
    /* attackr1d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[0] in a byte
     */
    UINT32 attackr1d1 : 3;
    /* attackr1d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[1] in a byte
     */
    UINT32 attackr1d2 : 3;
    /* attackr1d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[2] in a byte
     */
    UINT32 attackr1d3 : 3;
    /* attackr1d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[3] in a byte
     */
    UINT32 attackr1d4 : 3;
    /* attackr1d4 - Bits[14:12], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[4] in a byte
     */
    UINT32 attackr1d5 : 3;
    /* attackr1d5 - Bits[17:15], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[5] in a byte
     */
    UINT32 attackr1d6 : 3;
    /* attackr1d6 - Bits[20:18], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[6] in a byte
     */
    UINT32 attackr1d7 : 3;
    /* attackr1d7 - Bits[23:21], RW, default = 1'b0 
       xtalk aggressor1 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK1SELN1_7_MCIO_DDRIOEXT_STRUCT;






/* DATAOFFSETTRAINN1_7_MCIO_DDRIOEXT_REG supported on:                        */
/*       SKX_A0 (0x402B15DC)                                                  */
/*       SKX (0x402B15DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAINN1_7_MCIO_DDRIOEXT_REG 0x080145DC
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rcvenoffset : 6;
    /* rcvenoffset - Bits[5:0], RW, default = 1'b0 
       Offset added to Trained RxRcvEn Value.
     */
    UINT32 rxdqsoffset : 7;
    /* rxdqsoffset - Bits[12:6], RW, default = 1'b0 
       Offset added to Trained Rx DQS Value.
     */
    UINT32 txdqoffset : 6;
    /* txdqoffset - Bits[18:13], RW, default = 2'b00 
       Offset added to Trained Tx DQ Value.
     */
    UINT32 txdqsoffset : 6;
    /* txdqsoffset - Bits[24:19], RW, default = 6'b000000 
       Ooffset added to Trained Tx DQS Value.
     */
    UINT32 vrefoffset : 7;
    /* vrefoffset - Bits[31:25], RW, default = 7'b0000000 
       2s Compliment offset added to Trained Vref Value.  Positive number increases 
       Vref, and each step is Vdd/384. 
     */
  } Bits;
  UINT32 Data;
} DATAOFFSETTRAINN1_7_MCIO_DDRIOEXT_STRUCT;




/* ATTACK0SELN1_7_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B15E4)                                                  */
/*       SKX (0x402B15E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SELN1_7_MCIO_DDRIOEXT_REG 0x080145E4
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 attackr0d4 : 3;
    /* attackr0d4 - Bits[14:12], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[4] in a byte
     */
    UINT32 attackr0d5 : 3;
    /* attackr0d5 - Bits[17:15], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[5] in a byte
     */
    UINT32 attackr0d6 : 3;
    /* attackr0d6 - Bits[20:18], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[6] in a byte
     */
    UINT32 attackr0d7 : 3;
    /* attackr0d7 - Bits[23:21], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK0SELN1_7_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 1'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 txddjcen : 4;
    /* txddjcen - Bits[15:12], RW, default = 3'b000 
       TxddjcEn
     */
    UINT32 dfeenable : 1;
    /* dfeenable - Bits[16:16], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfedlysel : 1;
    /* dfedlysel - Bits[17:17], RW, default = 1'b0 
       Dfe Delay Selection
     */
    UINT32 dfecode : 4;
    /* dfecode - Bits[21:18], RW, default = 4'b0000 
       binary code 0000 to 1000 for DFE offset, mid-code code for zero offset is 0100
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} ATTACK0SELN1_7_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL0N1_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B15E8)                                                  */
/*       SKX (0x402B15E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0N1_7_MCIO_DDRIOEXT_REG 0x080145E8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxtrainingmode : 1;
    /* rxtrainingmode - Bits[0:0], RW, default = 1'b0 
       Enables Read DqDqs Training Mode
     */
    UINT32 wltrainingmode : 1;
    /* wltrainingmode - Bits[1:1], RW, default = 1'b0 
       Enables Write Leveling Training Mode
     */
    UINT32 rltrainingmode : 1;
    /* rltrainingmode - Bits[2:2], RW, default = 1'b0 
       Enables Read Leveling Training Mode.
     */
    UINT32 senseamptrainingmode : 1;
    /* senseamptrainingmode - Bits[3:3], RW, default = 1'b0 
       Enables SenseAmp offset cancellation training mode.
     */
    UINT32 txon : 1;
    /* txon - Bits[4:4], RW, default = 1'b0 
       When set, forces DQ/DQS drive enable to active.  Used in power measurements and 
       IO loopback mode  
     */
    UINT32 rfon : 1;
    /* rfon - Bits[5:5], RW, default = 1'b0 
       Power-down Disable:  Forces register file read, overriding the rank power down 
       logic. 
     */
    UINT32 rxpion : 1;
    /* rxpion - Bits[6:6], RW, default = 1'b0 
       Power-down Disable:  Forces RxDqs PI clocks on.
     */
    UINT32 txpion : 1;
    /* txpion - Bits[7:7], RW, default = 1'b0 
       Power-down Disable:  Forces TxDq and TxDqs PI clocks on.
     */
    UINT32 internalclockson : 1;
    /* internalclockson - Bits[8:8], RW, default = 1'b0 
       
                 Forces on master DLL and all PI enables ON, despite both channel in 
       SelfRefresh, etc. low power states. 
               
     */
    UINT32 saoffsettraintxon : 1;
    /* saoffsettraintxon - Bits[9:9], RW, default = 1'b0 
       SaOffsetTrainTxon
     */
    UINT32 txdisable : 1;
    /* txdisable - Bits[10:10], RW, default = 1'b0 
       Power-down:  All write commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 rxdisable : 1;
    /* rxdisable - Bits[11:11], RW, default = 1'b0 
       Power-down:  All read commands are ignored for this data-byte.  Used in 
       single-channel MCI mode.  
     */
    UINT32 txlong : 1;
    /* txlong - Bits[12:12], RW, default = 1'b0 
       Sets the duration of the first transmitted DQ bit of the burst. 0=1UI, 1=2UI.
     */
    UINT32 rsvd_13 : 5;
    UINT32 driversegmentenable : 1;
    /* driversegmentenable - Bits[18:18], RW, default = 1'b0 
       Controls which segements of the driver are enabled:  {0: All, 1: Only ODT}.
     */
    UINT32 rsvd_19 : 1;
    UINT32 readrfrd : 1;
    /* readrfrd - Bits[20:20], RW_V, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a read command to Rank = ReadRFRank 
     */
    UINT32 readrfwr : 1;
    /* readrfwr - Bits[21:21], RW_V, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a write command to Rank = ReadRFRank 
     */
    UINT32 readrfrank : 3;
    /* readrfrank - Bits[24:22], RW_V, default = 3'b000 
       Specifies the rank that ReadRFRd or ReadRFWr will manually download the CR 
       values from 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[25:25], RW, default = 1'b0 
       ODT is forced-on.
     */
    UINT32 odtsampoff : 1;
    /* odtsampoff - Bits[26:26], RW, default = 1'b0 
       ODT and Senseamp are forced-off.
     */
    UINT32 disableodtstatic : 1;
    /* disableodtstatic - Bits[27:27], RW, default = 1'b0 
       not used
     */
    UINT32 ddrcrforceodton : 1;
    /* ddrcrforceodton - Bits[28:28], RW, default = 1'b0 
       force ODT to the transmitter on, regardless of state of DriveEnable
     */
    UINT32 spare : 1;
    /* spare - Bits[29:29], RW, default = 1'b0 
       spare
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[30:30], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
    UINT32 longpreambleenable : 1;
    /* longpreambleenable - Bits[31:31], RW, default = 1'b0 
       Enables long preamble for DDR4 devices
     */
  } Bits;
  UINT32 Data;
} DATACONTROL0N1_7_MCIO_DDRIOEXT_STRUCT;


/* DATACONTROL1N1_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B15EC)                                                  */
/*       SKX (0x402B15EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1N1_7_MCIO_DDRIOEXT_REG 0x080145EC
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 burstlength10 : 1;
    /* burstlength10 - Bits[5:5], RW, default = 1'b0 
       reserved
     */
    UINT32 rsvd_6 : 4;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 1'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 4'b0000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 4'b0000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N1_7_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 xtalkpienable : 1;
    /* xtalkpienable - Bits[0:0], RW, default = 1'b0 
       enable phase shift based crosstalk cancellation.
     */
    UINT32 xtalkpisign : 1;
    /* xtalkpisign - Bits[1:1], RW, default = 1'b0 
       select if 0: capacitive coupling; or 1: inductive coupling; xtalk dominant
     */
    UINT32 xtalkpidelta : 3;
    /* xtalkpidelta - Bits[4:2], RW, default = 3'b0 
       delta phase shift in one aggressor is in effect
     */
    UINT32 xtalkpideltarank5 : 2;
    /* xtalkpideltarank5 - Bits[6:5], RW, default = 2'b0 
       XtalkPiDeltaRank5
     */
    UINT32 rsvd_7 : 3;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 1'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 4'b0000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 4'b0000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N1_7_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL2N1_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B15F0)                                                  */
/*       SKX (0x402B15F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2N1_7_MCIO_DDRIOEXT_REG 0x080145F0
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier). 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency  
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = None 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = None 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = None 
       Enable Imode EQ per byte lane. 
                 0: Imode completely off 
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 spare : 4;
    /* spare - Bits[14:11], RW, default = None 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = None 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = None 
       Binary Imode Coefficient, sets Imode current strength.
                 (used for either Xtalk cancellation or swing boost)
                 Code   Imode Current (approx)
                 ------------------
                 0000        0
                 0001        1
                 0010        2
                   *
                   *
                   *
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = None 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N1_7_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier). 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency  
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = None 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = None 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = None 
       Enable Imode EQ per byte lane. 
                 0: Imode completely off 
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 enearlywriterankqnnnh : 1;
    /* enearlywriterankqnnnh - Bits[11:11], RW, default = None 
       Spare
     */
    UINT32 picodepipeqnnnh : 1;
    /* picodepipeqnnnh - Bits[12:12], RW, default = None 
       Spare
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW, default = None 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = None 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = None 
       Binary Imode Coefficient, sets Imode current strength.
                 (used for either Xtalk cancellation or swing boost)
                 Code   Imode Current (approx)
                 ------------------
                 0000        0
                 0001        1
                 0010        2
                   *
                   *
                   *
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = None 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N1_7_MCIO_DDRIOEXT_STRUCT;



/* DATACONTROL3N1_7_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B15F4)                                                  */
/*       SKX (0x402B15F4)                                                     */
/* Register default value on SKX_A0:    0x8184891FCF                          */
/* Register default value on SKX:       0x00000FCF                            */
#define DATACONTROL3N1_7_MCIO_DDRIOEXT_REG 0x080145F4
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 drvstaticlegcfg : 2;
    /* drvstaticlegcfg - Bits[1:0], RW, default = 2'b11 
       binary default setting for static legs in DRV mode
                  0    0       0 legs (EQ disabled)
                  0    1       3 legs (EQ max 3 legs)
                  1    0       6 legs (EQ max 6 legs)
                  1    1      12 legs (EQ max 12 legs)
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher  
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit  
                 the higher Ron target value.
     */
    UINT32 odtstaticlegcfg : 2;
    /* odtstaticlegcfg - Bits[3:2], RW, default = 2'b11 
       binary default setting for static legs in ODT mode
                  0    0       0 legs (EQ disabled)
                  0    1       3 legs (EQ max 3 legs)
                  1    0       6 legs (EQ max 6 legs)
                  1    1      12 legs (EQ max 12 legs)
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher  
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit  
                 the higher Ron target value.
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[4:4], RW, default = 1'b0 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rsvd_5 : 1;
    UINT32 odtsegmentenable : 3;
    /* odtsegmentenable - Bits[8:6], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.  
                            After that, a shift register will determine the enabled 
       segments 
     */
    UINT32 datasegmentenable : 3;
    /* datasegmentenable - Bits[11:9], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset. 
     */
    UINT32 longodtr2w : 1;
    /* longodtr2w - Bits[12:12], RW, default = 3'b000 
       When set, it will extends ODT by 1 qclk on read to write turnarounds. hsd 230773
     */
    UINT32 spare2 : 1;
    /* spare2 - Bits[13:13], RW, default = 1'b0 
       Spare2
     */
    UINT32 imodebiasvrefen : 1;
    /* imodebiasvrefen - Bits[14:14], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 imodebiasrxbiastrim : 3;
    /* imodebiasrxbiastrim - Bits[17:15], RW, default = 3'b000 
       imodebias control. Also used for rxbiasana 
     */
    UINT32 spare : 1;
    /* spare - Bits[18:18], RW, default = 1'b0 
       Spare
     */
    UINT32 imodebiasdfxddr4legup : 1;
    /* imodebiasdfxddr4legup - Bits[19:19], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 imodebiasdfxlegdn : 1;
    /* imodebiasdfxlegdn - Bits[20:20], RW, default = 3'b000 
       imodebias control 
     */
    UINT32 ddrcrctlecapen : 2;
    /* ddrcrctlecapen - Bits[22:21], RW, default = 2'b00 
       ctle capacitor setting 
     */
    UINT32 ddrcrctleresen : 2;
    /* ddrcrctleresen - Bits[24:23], RW, default = 2'b00 
       ctle resistor setting 
     */
    UINT32 pcasbiasclosedloopen : 1;
    /* pcasbiasclosedloopen - Bits[25:25], RW, default = 1'b0 
       RxBias CR 
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[26:26], RW, default = 1'b0 
       RxBias CR 
     */
    UINT32 rxbiasfoldedlegtrim : 2;
    /* rxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b00 
       RxBias CR 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[30:29], RW, default = 2'b00 
       RxBias CR 
     */
    UINT32 postambleenable : 1;
    /* postambleenable - Bits[31:31], RW, default = None 
       Spare
     */
  } Bits;
  UINT32 Data;
} DATACONTROL3N1_7_MCIO_DDRIOEXT_STRUCT;


/* VSSHIORVREFCONTROLN1_7_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B15F8)                                                  */
/*       SKX (0x402B15F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROLN1_7_MCIO_DDRIOEXT_REG 0x080145F8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings. 
       
               VssHi Field Description: 
       
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode          23     Selects the code to output:  {0: Code, 1: TCode}
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode    
               PanicVoltage  21:18  Unsigned integer controlling the voltage error on 
       VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 117mV) 
               CloseLoop        17     Enables the VssHi close loop tracking 
               PanicEn          16     Enables the VssHi Panic Driver
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError) 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128} 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
       
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROLN1_7_MCIO_DDRIOEXT_STRUCT;




/* DDRCRCMDCOMP_CMDN_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1600)                                                  */
/*       SKX (0x402B1600)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_CMDN_MCIO_DDRIOEXT_REG 0x08014600
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RW, default = 1'b0 
       Scomp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RW, default = 1'b0 
       Tco Comp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RW, default = 1'b0 
       Rcomp Drv Up Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RW, default = 1'b0 
       Rcomp Drv Dn Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RW, default = 3'b010 
       not used
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = 1'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG supported on:                    */
/*       SKX_A0 (0x402B1604)                                                  */
/*       SKX (0x402B1604)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG 0x08014604
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 ddrcmdctlrx_en : 12;
    /* ddrcmdctlrx_en - Bits[28:17], RW, default = 1'b0 
       Enable DDRT Parity in CMDCTL
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMPOFFSET_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDPICODING_CMDN_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1608)                                                  */
/*       SKX (0x402B1608)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_CMDN_MCIO_DDRIOEXT_REG 0x08014608
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode0 : 6;
    /* cmdpicode0 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay0 : 2;
    /* cmdpilogicdelay0 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay - add 1 qclk cycle delay.
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode1 : 6;
    /* cmdpicode1 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay1 : 2;
    /* cmdpilogicdelay1 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode2 : 6;
    /* cmdpicode2 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay2 : 2;
    /* cmdpilogicdelay2 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B160C)                                                  */
/*       SKX (0x402B160C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG 0x0801460C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = None 
       Enable Visa debug bus LCB for CMD fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = None 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = None 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 dllmask : 2;
    /* dllmask - Bits[5:4], RW, default = 1'b0 
       On weak lock exit, controls how long the feedback to the phase detector is 
       masked: {0: 1 qclk (required for delayline latency), 1: 2 qclk, 2: 3 qclk, 3: 4 
       qclk} 
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[6:6], RW, default = 1'b0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CMD transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used 
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 1'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 cmdtxeq : 2;
    /* cmdtxeq - Bits[14:13], RW, default = 1'b0 
       not used
     */
    UINT32 earlyweakdrive : 2;
    /* earlyweakdrive - Bits[16:15], RW, default = 1'b0 
       Controls the number of segments enabled during the early warning weak drive: {0: 
       All Enabled, 1: Enable Seg[4], 2: Enable Seg[4:3], 3: Enable Seg[4:2]} 
     */
    UINT32 lvmode : 2;
    /* lvmode - Bits[18:17], RW, default = None 
       not used
               
     */
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[19:19], RW, default = None 
       not used
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[20:20], RW, default = None 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 6'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefdisable : 1;
    /* vrefdisable - Bits[29:29], RW, default = 1'b0 
        Vref Disable
     */
    UINT32 ddr4modeenable : 1;
    /* ddr4modeenable - Bits[30:30], RW, default = 1'b0 
       Ddr4 Mode Enable, set to 1 in CMD2 (Func:5.Addr:C00 and Func:7.Addr:C00) fub to 
       bypass extra delay on parity bit 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS2_CMDN_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1610)                                                  */
/*       SKX (0x402B1610)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_CMDN_MCIO_DDRIOEXT_REG 0x08014610
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 21;
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[21:21], RW, default = None 
       Enable finer vref resolution in ddr4
               
     */
    UINT32 rsvd_22 : 1;
    UINT32 cmdrxbiassel : 1;
    /* cmdrxbiassel - Bits[23:23], RW, default = 9'b0 
       
       	 CmdRxBiasSel for Cmd Receiver. 
         SKX b305822 - bit is floating and not connected to RxBiasSel in ddrcmdctl fub 
       design.  
          Refer to description for DdrCrCmdControls2.FnvCrCmdIsCke for RxBiasSel 
       control. 
       	
     */
    UINT32 forcecmdbiason : 1;
    /* forcecmdbiason - Bits[24:24], RW, default = 9'b1 
       ForceCmdBiason for Cmd Receiver
     */
    UINT32 rsvd_25 : 1;
    UINT32 cmdctlecapen : 2;
    /* cmdctlecapen - Bits[27:26], RW, default = 9'b0 
       CmdCtl Buffer CtleCapEn
     */
    UINT32 cmdctleresen : 2;
    /* cmdctleresen - Bits[29:28], RW, default = 9'b0 
       CmdCtl Buffer CtleResEn
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS2_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLCOMP_CMDN_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1614)                                                  */
/*       SKX (0x402B1614)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMP_CMDN_MCIO_DDRIOEXT_REG 0x08014614
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RW, default = 1'b0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RW, default = 1'b0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RW, default = 1'b0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RW, default = 1'b0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RW, default = 1'b0 
       LevelShift Comp Value
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = 1'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG supported on:                    */
/*       SKX_A0 (0x402B1618)                                                  */
/*       SKX (0x402B1618)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG 0x08014618
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 cmdctlbufsel : 6;
    /* cmdctlbufsel - Bits[23:18], RW, default = 1'b0 
       CmdBuf or CtlBuf Selection 
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[24:24], RW, default = 1'b0 
       Is the Direction (RX/TX Valid)
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B161C)                                                  */
/*       SKX (0x402B161C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG 0x0801461C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls 3. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 bufdrvsegen : 5;
    /* bufdrvsegen - Bits[4:0], RW, default = 5'b11111 
       Drive strength enable for CmdBuf/CtlBuf.
     */
    UINT32 cmdvrefpgen : 1;
    /* cmdvrefpgen - Bits[5:5], RW, default = 1'b1 
       control the DdrVrefSelDQQnnnH pin on the ddrvrefint generator
     */
    UINT32 odtstrength : 1;
    /* odtstrength - Bits[6:6], RW, default = 1'b1 
       OdtStrength 
     */
    UINT32 rsvd_7 : 2;
    UINT32 divby2alignctl : 1;
    /* divby2alignctl - Bits[9:9], RW, default = None 
       Invert DAlign 
     */
    UINT32 divby2disable : 1;
    /* divby2disable - Bits[10:10], RW, default = None 
       select qclk for cmdxover instead of Dclk 
     */
    UINT32 odten : 12;
    /* odten - Bits[22:11], RW, default = None 
       ODTDis for Cmd and Ctl buf
     */
    UINT32 cmdrefpiclkdelay : 6;
    /* cmdrefpiclkdelay - Bits[28:23], RW, default = None 
       CmdRefClk
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLRANKSUSED_CMDN_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1620)                                                  */
/*       SKX (0x402B1620)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_CMDN_MCIO_DDRIOEXT_REG 0x08014620
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 6;
    /* ranken - Bits[5:0], RW, default = 6'b111111 
       Enables output buffers for ctlbufpair 0 to ctlbufpair6 , PI clocks and output 
       datapaths for this rank 
     */
    UINT32 rsvd_6 : 6;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[12:12], RW, default = 1'b0 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[14:13], RW, default = 2'b00 
       used to compensate the GCN skew. The GCN skew information should be
                 extracted by reading and analyzing the final Rcomp code value. The 
       target spec 
                 is to categorize the GCN skew in 3 process condition (mainly slow, 
       fast, typical). 
                 (one example can be just reading the 2 MLS bit of Rcomp code and 
       determine if the 
                 GCN is slow, fast or typical). The gcncomp is used for CTLE.
                 11: gcn in slow corner
                 10: gcn in typical corner
                 01: gcn in fast corner
               
     */
    UINT32 rsvd_15 : 1;
    UINT32 imodebiashighcm : 1;
    /* imodebiashighcm - Bits[16:16], RW, default = 1'b0 
       Imode bias control
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[17:17], RW, default = 1'b0 
       
       	 Rx bias Sel.
         SKX b305822 - bit is floating and not connected to RxBiasSel in ddrcmdctl fub 
       design. 
          Refer to description for DdrCrCmdControls2.FnvCrCmdIsCke for RxBiasSel 
       control. 
       	
     */
    UINT32 cmdrxeqqnnnl : 3;
    /* cmdrxeqqnnnl - Bits[20:18], RW, default = 12'b0 
       CmdRxEqQnnnL for RX CmdCtl Buf 
     */
    UINT32 rsvd_21 : 3;
    UINT32 ddrcmdrxbiasctl : 3;
    /* ddrcmdrxbiasctl - Bits[26:24], RW, default = 2'b0 
       CmdRxBiasCtl for RX biasgen 
     */
    UINT32 cmdpcasbiasclosedloopen : 1;
    /* cmdpcasbiasclosedloopen - Bits[27:27], RW, default = 2'b0 
       CmdPcasBiasClosedLoopEnQnnnH for RX biasgen 
     */
    UINT32 ddrcmdrxbiasfoldedlegtrim : 2;
    /* ddrcmdrxbiasfoldedlegtrim - Bits[29:28], RW, default = 2'b0 
       DdrCmdRxBiasFoldedLegTrim for RX biasgen 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLRANKSUSED_CMDN_MCIO_DDRIOEXT_STRUCT;






/* DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B162C)                                                  */
/*       SKX (0x402B162C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_REG 0x0801462C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Cmd Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 spare0 : 2;
    /* spare0 - Bits[7:6], RO_V, default = None 
       Spare
               
     */
    UINT32 xoverphasedet : 14;
    /* xoverphasedet - Bits[21:8], RO_V, default = 14'b0 
       bits13:6 unused
                            bit5 output of phase detector for I/O Pair 0
                            bit4 output of phase detector for I/O Pair 1
                            bit3 output of phase detector for I/O Pair 2
                            bit2 output of phase detector for I/O Pair 3
                            bit1 output of phase detector for I/O Pair 4
                            bit0 output of phase detector for I/O Pair 5 (only on 
       cmdctla FUBs) 
               
     */
    UINT32 trainingoffset2 : 6;
    /* trainingoffset2 - Bits[27:22], RW, default = 6'b0 
       Offset added to PiCode. Obtained via xover calibration such that ClkPi aligns 
       with grid clock when PiCode=0. 
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS1_CMDN_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1630)                                                  */
/*       SKX (0x402B1630)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_CMDN_MCIO_DDRIOEXT_REG 0x08014630
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled  
                   for deemphasized, and so on.
               
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_CMDN_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled  
                   for deemphasized, and so on.
               
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_CMDN_MCIO_DDRIOEXT_STRUCT;



/* DDRCRCMDPICODING2_CMDN_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1634)                                                  */
/*       SKX (0x402B1634)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_CMDN_MCIO_DDRIOEXT_REG 0x08014634
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode3 : 6;
    /* cmdpicode3 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay3 : 2;
    /* cmdpilogicdelay3 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode4 : 6;
    /* cmdpicode4 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay4 : 2;
    /* cmdpilogicdelay4 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode5 : 6;
    /* cmdpicode5 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay5 : 2;
    /* cmdpilogicdelay5 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING2_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDPICODING3_CMDN_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1638)                                                  */
/*       SKX (0x402B1638)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_CMDN_MCIO_DDRIOEXT_REG 0x08014638
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode6 : 6;
    /* cmdpicode6 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay6 : 2;
    /* cmdpilogicdelay6 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING3_CMDN_MCIO_DDRIOEXT_STRUCT;




/* VSSHIORVREFCONTROL_CMDN_MCIO_DDRIOEXT_REG supported on:                    */
/*       SKX_A0 (0x402B1640)                                                  */
/*       SKX (0x402B1640)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_CMDN_MCIO_DDRIOEXT_REG 0x08014640
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings. 
       
               VssHi Field Description:
       
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode          23     Selects the code to output:  {0: Code, 1: TCode}
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode 
               PanicVoltage     21:18  Unsigned integer controlling the voltage error 
       on VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV) 
               CloseLoop        17     Enables the VssHi close loop tracking
               PanicEn          16     Enables the VssHi Panic Driver
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError) 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128} 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
       
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL_CMDN_MCIO_DDRIOEXT_STRUCT;




/* VSSHIORVREFCONTROL1_CMDN_MCIO_DDRIOEXT_REG supported on:                   */
/*       SKX_A0 (0x402B1648)                                                  */
/*       SKX (0x402B1648)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_CMDN_MCIO_DDRIOEXT_REG 0x08014648
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Extension for BDX. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 8;
    /* vsshiorvrefctl - Bits[7:0], RW, default = 8'b0 
        BDX-specific extended control signal. 
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL1_CMDN_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMP_CKE_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B1800)                                                  */
/*       SKX (0x402B1800)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_CKE_MCIO_DDRIOEXT_REG 0x08014800
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RW, default = 1'b0 
       Scomp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RW, default = 1'b0 
       Tco Comp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RW, default = 1'b0 
       Rcomp Drv Up Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RW, default = 1'b0 
       Rcomp Drv Dn Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RW, default = 3'b010 
       not used
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = 1'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1804)                                                  */
/*       SKX (0x402B1804)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG 0x08014804
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 ddrcmdctlrx_en : 12;
    /* ddrcmdctlrx_en - Bits[28:17], RW, default = 1'b0 
       Enable DDRT Parity in CMDCTL
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMPOFFSET_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDPICODING_CKE_MCIO_DDRIOEXT_REG supported on:                       */
/*       SKX_A0 (0x402B1808)                                                  */
/*       SKX (0x402B1808)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_CKE_MCIO_DDRIOEXT_REG 0x08014808
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode0 : 6;
    /* cmdpicode0 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay0 : 2;
    /* cmdpilogicdelay0 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay - add 1 qclk cycle delay.
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode1 : 6;
    /* cmdpicode1 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay1 : 2;
    /* cmdpilogicdelay1 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode2 : 6;
    /* cmdpicode2 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay2 : 2;
    /* cmdpilogicdelay2 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG supported on:                       */
/*       SKX_A0 (0x402B180C)                                                  */
/*       SKX (0x402B180C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG 0x0801480C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = None 
       Enable Visa debug bus LCB for CMD fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = None 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = None 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 dllmask : 2;
    /* dllmask - Bits[5:4], RW, default = 1'b0 
       On weak lock exit, controls how long the feedback to the phase detector is 
       masked: {0: 1 qclk (required for delayline latency), 1: 2 qclk, 2: 3 qclk, 3: 4 
       qclk} 
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[6:6], RW, default = 1'b0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CMD transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used 
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 1'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 cmdtxeq : 2;
    /* cmdtxeq - Bits[14:13], RW, default = 1'b0 
       not used
     */
    UINT32 earlyweakdrive : 2;
    /* earlyweakdrive - Bits[16:15], RW, default = 1'b0 
       Controls the number of segments enabled during the early warning weak drive: {0: 
       All Enabled, 1: Enable Seg[4], 2: Enable Seg[4:3], 3: Enable Seg[4:2]} 
     */
    UINT32 lvmode : 2;
    /* lvmode - Bits[18:17], RW, default = None 
       not used
               
     */
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[19:19], RW, default = None 
       not used
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[20:20], RW, default = None 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 6'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefdisable : 1;
    /* vrefdisable - Bits[29:29], RW, default = 1'b0 
        Vref Disable
     */
    UINT32 ddr4modeenable : 1;
    /* ddr4modeenable - Bits[30:30], RW, default = 1'b0 
       Ddr4 Mode Enable, set to 1 in CMD2 (Func:5.Addr:C00 and Func:7.Addr:C00) fub to 
       bypass extra delay on parity bit 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS2_CKE_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1810)                                                  */
/*       SKX (0x402B1810)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_CKE_MCIO_DDRIOEXT_REG 0x08014810
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 21;
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[21:21], RW, default = None 
       Enable finer vref resolution in ddr4
               
     */
    UINT32 rsvd_22 : 1;
    UINT32 cmdrxbiassel : 1;
    /* cmdrxbiassel - Bits[23:23], RW, default = 9'b0 
       
       	 CmdRxBiasSel for Cmd Receiver. 
         SKX b305822 - bit is floating and not connected to RxBiasSel in ddrcmdctl fub 
       design.  
          Refer to description for DdrCrCmdControls2.FnvCrCmdIsCke for RxBiasSel 
       control. 
       	
     */
    UINT32 forcecmdbiason : 1;
    /* forcecmdbiason - Bits[24:24], RW, default = 9'b1 
       ForceCmdBiason for Cmd Receiver
     */
    UINT32 rsvd_25 : 1;
    UINT32 cmdctlecapen : 2;
    /* cmdctlecapen - Bits[27:26], RW, default = 9'b0 
       CmdCtl Buffer CtleCapEn
     */
    UINT32 cmdctleresen : 2;
    /* cmdctleresen - Bits[29:28], RW, default = 9'b0 
       CmdCtl Buffer CtleResEn
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS2_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLCOMP_CKE_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B1814)                                                  */
/*       SKX (0x402B1814)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMP_CKE_MCIO_DDRIOEXT_REG 0x08014814
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RW, default = 1'b0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RW, default = 1'b0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RW, default = 1'b0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RW, default = 1'b0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RW, default = 1'b0 
       LevelShift Comp Value
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = 1'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1818)                                                  */
/*       SKX (0x402B1818)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG 0x08014818
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 cmdctlbufsel : 6;
    /* cmdctlbufsel - Bits[23:18], RW, default = 1'b0 
       CmdBuf or CtlBuf Selection 
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[24:24], RW, default = 1'b0 
       Is the Direction (RX/TX Valid)
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B181C)                                                  */
/*       SKX (0x402B181C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG 0x0801481C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls 3. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 bufdrvsegen : 5;
    /* bufdrvsegen - Bits[4:0], RW, default = 5'b11111 
       Drive strength enable for CmdBuf/CtlBuf.
     */
    UINT32 cmdvrefpgen : 1;
    /* cmdvrefpgen - Bits[5:5], RW, default = 1'b1 
       control the DdrVrefSelDQQnnnH pin on the ddrvrefint generator
     */
    UINT32 odtstrength : 1;
    /* odtstrength - Bits[6:6], RW, default = 1'b1 
       OdtStrength 
     */
    UINT32 rsvd_7 : 2;
    UINT32 divby2alignctl : 1;
    /* divby2alignctl - Bits[9:9], RW, default = None 
       Invert DAlign 
     */
    UINT32 divby2disable : 1;
    /* divby2disable - Bits[10:10], RW, default = None 
       select qclk for cmdxover instead of Dclk 
     */
    UINT32 odten : 12;
    /* odten - Bits[22:11], RW, default = None 
       ODTDis for Cmd and Ctl buf
     */
    UINT32 cmdrefpiclkdelay : 6;
    /* cmdrefpiclkdelay - Bits[28:23], RW, default = None 
       CmdRefClk
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLRANKSUSED_CKE_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1820)                                                  */
/*       SKX (0x402B1820)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_CKE_MCIO_DDRIOEXT_REG 0x08014820
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 6;
    /* ranken - Bits[5:0], RW, default = 6'b111111 
       Enables output buffers for ctlbufpair 0 to ctlbufpair6 , PI clocks and output 
       datapaths for this rank 
     */
    UINT32 rsvd_6 : 6;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[12:12], RW, default = 1'b0 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[14:13], RW, default = 2'b00 
       used to compensate the GCN skew. The GCN skew information should be
                 extracted by reading and analyzing the final Rcomp code value. The 
       target spec 
                 is to categorize the GCN skew in 3 process condition (mainly slow, 
       fast, typical). 
                 (one example can be just reading the 2 MLS bit of Rcomp code and 
       determine if the 
                 GCN is slow, fast or typical). The gcncomp is used for CTLE.
                 11: gcn in slow corner
                 10: gcn in typical corner
                 01: gcn in fast corner
               
     */
    UINT32 rsvd_15 : 1;
    UINT32 imodebiashighcm : 1;
    /* imodebiashighcm - Bits[16:16], RW, default = 1'b0 
       Imode bias control
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[17:17], RW, default = 1'b0 
       
       	 Rx bias Sel.
         SKX b305822 - bit is floating and not connected to RxBiasSel in ddrcmdctl fub 
       design. 
          Refer to description for DdrCrCmdControls2.FnvCrCmdIsCke for RxBiasSel 
       control. 
       	
     */
    UINT32 cmdrxeqqnnnl : 3;
    /* cmdrxeqqnnnl - Bits[20:18], RW, default = 12'b0 
       CmdRxEqQnnnL for RX CmdCtl Buf 
     */
    UINT32 rsvd_21 : 3;
    UINT32 ddrcmdrxbiasctl : 3;
    /* ddrcmdrxbiasctl - Bits[26:24], RW, default = 2'b0 
       CmdRxBiasCtl for RX biasgen 
     */
    UINT32 cmdpcasbiasclosedloopen : 1;
    /* cmdpcasbiasclosedloopen - Bits[27:27], RW, default = 2'b0 
       CmdPcasBiasClosedLoopEnQnnnH for RX biasgen 
     */
    UINT32 ddrcmdrxbiasfoldedlegtrim : 2;
    /* ddrcmdrxbiasfoldedlegtrim - Bits[29:28], RW, default = 2'b0 
       DdrCmdRxBiasFoldedLegTrim for RX biasgen 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLRANKSUSED_CKE_MCIO_DDRIOEXT_STRUCT;






/* DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_REG supported on:                       */
/*       SKX_A0 (0x402B182C)                                                  */
/*       SKX (0x402B182C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_REG 0x0801482C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Cmd Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 spare0 : 2;
    /* spare0 - Bits[7:6], RO_V, default = None 
       Spare
               
     */
    UINT32 xoverphasedet : 14;
    /* xoverphasedet - Bits[21:8], RO_V, default = 14'b0 
       bits13:6 unused
                            bit5 output of phase detector for I/O Pair 0
                            bit4 output of phase detector for I/O Pair 1
                            bit3 output of phase detector for I/O Pair 2
                            bit2 output of phase detector for I/O Pair 3
                            bit1 output of phase detector for I/O Pair 4
                            bit0 output of phase detector for I/O Pair 5 (only on 
       cmdctla FUBs) 
               
     */
    UINT32 trainingoffset2 : 6;
    /* trainingoffset2 - Bits[27:22], RW, default = 6'b0 
       Offset added to PiCode. Obtained via xover calibration such that ClkPi aligns 
       with grid clock when PiCode=0. 
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS1_CKE_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1830)                                                  */
/*       SKX (0x402B1830)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_CKE_MCIO_DDRIOEXT_REG 0x08014830
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled  
                   for deemphasized, and so on.
               
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_CKE_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled  
                   for deemphasized, and so on.
               
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_CKE_MCIO_DDRIOEXT_STRUCT;



/* DDRCRCMDPICODING2_CKE_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1834)                                                  */
/*       SKX (0x402B1834)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_CKE_MCIO_DDRIOEXT_REG 0x08014834
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode3 : 6;
    /* cmdpicode3 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay3 : 2;
    /* cmdpilogicdelay3 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode4 : 6;
    /* cmdpicode4 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay4 : 2;
    /* cmdpilogicdelay4 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode5 : 6;
    /* cmdpicode5 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay5 : 2;
    /* cmdpilogicdelay5 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING2_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDPICODING3_CKE_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1838)                                                  */
/*       SKX (0x402B1838)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_CKE_MCIO_DDRIOEXT_REG 0x08014838
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode6 : 6;
    /* cmdpicode6 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay6 : 2;
    /* cmdpilogicdelay6 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING3_CKE_MCIO_DDRIOEXT_STRUCT;




/* VSSHIORVREFCONTROL_CKE_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1840)                                                  */
/*       SKX (0x402B1840)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_CKE_MCIO_DDRIOEXT_REG 0x08014840
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings. 
       
               VssHi Field Description:
       
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode          23     Selects the code to output:  {0: Code, 1: TCode}
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode 
               PanicVoltage     21:18  Unsigned integer controlling the voltage error 
       on VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV) 
               CloseLoop        17     Enables the VssHi close loop tracking
               PanicEn          16     Enables the VssHi Panic Driver
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError) 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128} 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
       
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMP1_CKE_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1844)                                                  */
/*       SKX (0x402B1844)                                                     */
/* Register default value on SKX_A0:    0x00080205                            */
/* Register default value on SKX:       0x00000000                            */
#define DDRCRCMDCOMP1_CKE_MCIO_DDRIOEXT_REG 0x08014844
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Cmd Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vtcomp : 5;
    /* vtcomp - Bits[4:0], RW, default = 5'b00000 
       VT Comp Values
     */
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[14:5], RW, default = 10'b0000000000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[24:15], RW, default = 10'b0000000000 
       Panic DrvUp RComp Comp Value.
     */
    UINT32 spare : 7;
    /* spare - Bits[31:25], RW, default = 7'h00 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP1_CKE_MCIO_DDRIOEXT_STRUCT;


/* VSSHIORVREFCONTROL1_CKE_MCIO_DDRIOEXT_REG supported on:                    */
/*       SKX_A0 (0x402B1848)                                                  */
/*       SKX (0x402B1848)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_CKE_MCIO_DDRIOEXT_REG 0x08014848
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Extension for BDX. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 8;
    /* vsshiorvrefctl - Bits[7:0], RW, default = 8'b0 
        BDX-specific extended control signal. 
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL1_CKE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1A00)                                                  */
/*       SKX (0x402B1A00)                                                     */
/* Register default value:              0x0000E00F                            */
#define DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG 0x08014A00
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CLK Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 4;
    /* ranken - Bits[3:0], RW, default = 4'b1111 
       Enables output buffers, PI clocks and output datapaths for this rank
     */
    UINT32 spare1 : 4;
    /* spare1 - Bits[7:4], RW, default = 4'b0 
       Spare
     */
    UINT32 ddrtclk1en : 1;
    /* ddrtclk1en - Bits[8:8], RW, default = 8'h0 
       DDRT Mode Clk1 Enable
     */
    UINT32 ddrtclk3en : 1;
    /* ddrtclk3en - Bits[9:9], RW, default = 8'h0 
       DDRT Mode Clk3 Enable
     */
    UINT32 ddrtclk0en : 1;
    /* ddrtclk0en - Bits[10:10], RW, default = 8'h0 
       DDRT Mode Clk0 Enable
     */
    UINT32 ddrtclk2en : 1;
    /* ddrtclk2en - Bits[11:11], RW, default = 8'h0 
       DDRT Mode Clk2 Enable
     */
    UINT32 ddrttraining : 1;
    /* ddrttraining - Bits[12:12], RW, default = 1'b0 
       Asserted during the Read ID fine training sub-step Causes the sampled value for 
       the Read ID signal to be placed in a results register within the DDRIO fub 
     */
    UINT32 drvsegen : 3;
    /* drvsegen - Bits[15:13], RW, default = 3'b111 
       Segments to enable for driving (inverse for ODT)
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[16:16], RW, default = None 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 clkdealignbitqnnnh : 1;
    /* clkdealignbitqnnnh - Bits[17:17], RW, default = 1'b0 
       XNOR with DAlign
     */
    UINT32 fifoptctlqnnnh : 1;
    /* fifoptctlqnnnh - Bits[18:18], RW, default = 1'b0 
       Fifo Point separation bit
     */
    UINT32 spare : 1;
    /* spare - Bits[19:19], RW, default = 1'b0 
       spare
     */
    UINT32 clkctlecapen : 2;
    /* clkctlecapen - Bits[21:20], RW, default = 6'h0 
       ClkCtleCapEn for Clk RX buffer
     */
    UINT32 clkctleresen : 2;
    /* clkctleresen - Bits[23:22], RW, default = 6'h0 
       ClkCtleResEn for Clk RX buffer
     */
    UINT32 rsvd_24 : 1;
    UINT32 refpiclk : 6;
    /* refpiclk - Bits[30:25], RW, default = 5'h0 
       RefD0Clk
     */
    UINT32 ddrclkverticalbumpplacmenten : 1;
    /* ddrclkverticalbumpplacmenten - Bits[31:31], RW, default = 1'h0 
       Ddr CLK fub vertical bump placement enable for RGT/FNV
     */
  } Bits;
  UINT32 Data;
} DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B1A04)                                                  */
/*       SKX (0x402B1A04)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_REG 0x08014A04
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CLK Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 5;
    /* scomp - Bits[4:0], RW, default = 5'h0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[11:5], RW, default = 7'h0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[17:12], RW, default = 6'h0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[23:18], RW, default = 6'h0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[26:24], RW, default = 3'b000 
       LevelShift Comp Value. Not unsed, controlled via MsgCh only.
     */
    UINT32 spare : 5;
    /* spare - Bits[31:27], RW, default = 5'h0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCLKCOMPOFFSET_CLK_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1A08)                                                  */
/*       SKX (0x402B1A08)                                                     */
/* Register default value:              0x82010000                            */
#define DDRCRCLKCOMPOFFSET_CLK_MCIO_DDRIOEXT_REG 0x08014A08
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CLK Comp Offset Values. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 clkpcasbiasclosedloopen : 1;
    /* clkpcasbiasclosedloopen - Bits[17:17], RW, default = 1'b0 
       ClkPcasBiasClosedLoopEn for Clk Rx buf
     */
    UINT32 clkrxbiasselqnnnh : 1;
    /* clkrxbiasselqnnnh - Bits[18:18], RW, default = 1'b0 
       ClkRxBiasSelQnnnH for Clk Rx Buf
     */
    UINT32 clkrxeq : 3;
    /* clkrxeq - Bits[21:19], RW, default = 1'b0 
       ClkRxEq for Clk Rx Buf
     */
    UINT32 ddrclkrxbiasctl : 3;
    /* ddrclkrxbiasctl - Bits[24:22], RW, default = 1'b0 
       DdrClkRxBiasCtl for Clk Rx Buf
     */
    UINT32 forceclkbiason : 1;
    /* forceclkbiason - Bits[25:25], RW, default = 1'b1 
       ForceClkBiason for Clk Rx Buf
     */
    UINT32 rsvd_26 : 1;
    UINT32 ddrclkrxbiasfoldedlegtrim : 2;
    /* ddrclkrxbiasfoldedlegtrim - Bits[28:27], RW, default = 1'b0 
       DdrClkRxBiasFoldedLegTrim for Clk Rx Buf
     */
    UINT32 ddrclkrxbiasgcncomp : 2;
    /* ddrclkrxbiasgcncomp - Bits[30:29], RW, default = 1'b0 
       DdrClkRxBiasGcnComp for Clk Rx Buf
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DDRCRCLKCOMPOFFSET_CLK_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCLKPICODE_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B1A0C)                                                  */
/*       SKX (0x402B1A0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKPICODE_MCIO_DDRIOEXT_REG 0x08014A0C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CLK Pi Codes. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 pisettingrank0 : 6;
    /* pisettingrank0 - Bits[5:0], RW, default = 1'b0 
       Pi Setting, Rank 0
                            [5:0] - Pi setting for xxDDR_CLK_P/N[0]
     */
    UINT32 pilogicdelayrank0 : 1;
    /* pilogicdelayrank0 - Bits[6:6], RW, default = 1'b0 
       Pi Logic Delay, Rank 0, delays CLK by an extra qclk cycle
     */
    UINT32 rsvd_7 : 1;
    UINT32 pisettingrank1 : 6;
    /* pisettingrank1 - Bits[13:8], RW, default = 1'b0 
       Pi Setting, Rank 1
                            [5:0] - Pi setting for xxDDR_CLK_P/N[1]
     */
    UINT32 pilogicdelayrank1 : 1;
    /* pilogicdelayrank1 - Bits[14:14], RW, default = 1'b0 
       Pi Logic Delay, Rank 1
     */
    UINT32 spare1 : 1;
    /* spare1 - Bits[15:15], RW, default = 1'b0 
       not used
     */
    UINT32 pisettingrank2 : 6;
    /* pisettingrank2 - Bits[21:16], RW, default = 1'b0 
       Pi Setting, Rank 2
                            [5:0] - Pi setting for xxDDR_CLK_P/N[2]
     */
    UINT32 pilogicdelayrank2 : 1;
    /* pilogicdelayrank2 - Bits[22:22], RW, default = 1'b0 
       Pi Logic Delay, Rank 2
     */
    UINT32 rsvd_23 : 1;
    UINT32 pisettingrank3 : 6;
    /* pisettingrank3 - Bits[29:24], RW, default = 1'b0 
       Pi Setting, Rank 3
                            [5:0] - Pi setting for xxDDR_CLK_P/N[3]
     */
    UINT32 pilogicdelayrank3 : 1;
    /* pilogicdelayrank3 - Bits[30:30], RW, default = 1'b0 
       Pi Logic Delay, Rank 3
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DDRCRCLKPICODE_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B1A10)                                                  */
/*       SKX (0x402B1A10)                                                     */
/* Register default value:              0x00066000                            */
#define DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG 0x08014A10
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CLK Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = None 
       Enable Visa debug bus LCB for CLK fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = None 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = None 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 clkodtpdnsegen : 3;
    /* clkodtpdnsegen - Bits[6:4], RW, default = 1'b0 
       not used
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CLK transmitter on, overriding any power downs 
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used 
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 2'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 odtpupsegen : 3;
    /* odtpupsegen - Bits[15:13], RW, default = 3'b011 
       not used
     */
    UINT32 is_rdimm : 1;
    /* is_rdimm - Bits[16:16], RW, default = None 
       set to drive DC on xxCLK during P6 and S3 if DDR3 + RDIMM is populated
     */
    UINT32 statlegen : 2;
    /* statlegen - Bits[18:17], RW, default = 2'b11 
       static leg enable
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[19:19], RW, default = None 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[20:20], RW, default = None 
       1: Center Tap Termination for DDR3; 0: Vddq Termination for DDR4/Intel SMI 2
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 7'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 rsvd_29 : 1;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[30:30], RW, default = 1'b0 
       level shift hold enable:
                            Holds the HV control values and power down the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCLKCONTROLS_MCIO_DDRIOEXT_STRUCT;




/* DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B1A1C)                                                  */
/*       SKX (0x402B1A1C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG 0x08014A1C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Clk Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 1'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[6:6], RW, default = 1'b0 
       IODirectionValid
               
     */
    UINT32 rsvd_7 : 1;
    UINT32 pclkxoverphasedet : 4;
    /* pclkxoverphasedet - Bits[11:8], RO_V, default = 1'b0 
       P Clock Buffer Xover Phase Detect
               
     */
    UINT32 nclkxoverphasedet : 4;
    /* nclkxoverphasedet - Bits[15:12], RO_V, default = 1'b0 
       N Clock Buffer Xover Phase Detect
               
     */
    UINT32 spare2 : 16;
    /* spare2 - Bits[31:16], RW, default = 1'b0 
       spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCLKTRAINING_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCLKACIOLB_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B1A20)                                                  */
/*       SKX (0x402B1A20)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKACIOLB_MCIO_DDRIOEXT_REG 0x08014A20
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * ACIO Loopback Control Register. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 rxvocpassgateen : 8;
    /* rxvocpassgateen - Bits[25:18], RW, default = None 
       Independent ClkRxVocSel or ClkRxVocPassGateEn for Full RX receiver 
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCLKACIOLB_MCIO_DDRIOEXT_STRUCT;


/* DDRCRDDRTTRAINRESULT_MCIO_DDRIOEXT_REG supported on:                       */
/*       SKX_A0 (0x402B1A24)                                                  */
/*       SKX (0x402B1A24)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRDDRTTRAINRESULT_MCIO_DDRIOEXT_REG 0x08014A24
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * DDRT Training Result
 */
typedef union {
  struct {
    UINT32 evenoddsamples : 8;
    /* evenoddsamples - Bits[7:0], RW, default = 16'h0000 
       Bit 7 CKP3 early Read ID training result (Even) 
                            Bit 6 CKP3 early Read ID training result (Odd)
                            Bit 5 CKN3 early Read ID training result (Even)
                            Bit 4 CKN3 early Read ID training result (Odd)
                            Bit 3 CKP1 early Read ID training result (Even)
                            Bit 2 CKP1 early Read ID training result (Odd)
                            Bit 1 CKN1 early Read ID training result (Even)
                            Bit 0 CKN1 early Read ID training result (Odd)
                            CKP3/CKN3 ï slot 1 read ID bits   
                            CKP1/CKN1   slot 0 read ID bits
                            Read ID training result (8 bits) Result of sampling on each 
       Read ID input signal, and for even and odd cycles If the even/odd results are 
       recorded in separate bitfields, no need to add logic to only capture every other 
       sample, since the data is available for the BIOS to make a decision. 
               
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRDDRTTRAINRESULT_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMP_CMDS_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1C00)                                                  */
/*       SKX (0x402B1C00)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_CMDS_MCIO_DDRIOEXT_REG 0x08014C00
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RW, default = 1'b0 
       Scomp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RW, default = 1'b0 
       Tco Comp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RW, default = 1'b0 
       Rcomp Drv Up Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RW, default = 1'b0 
       Rcomp Drv Dn Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RW, default = 3'b010 
       not used
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = 1'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG supported on:                    */
/*       SKX_A0 (0x402B1C04)                                                  */
/*       SKX (0x402B1C04)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG 0x08014C04
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 ddrcmdctlrx_en : 12;
    /* ddrcmdctlrx_en - Bits[28:17], RW, default = 1'b0 
       Enable DDRT Parity in CMDCTL
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMPOFFSET_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDPICODING_CMDS_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1C08)                                                  */
/*       SKX (0x402B1C08)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_CMDS_MCIO_DDRIOEXT_REG 0x08014C08
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode0 : 6;
    /* cmdpicode0 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay0 : 2;
    /* cmdpilogicdelay0 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay - add 1 qclk cycle delay.
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode1 : 6;
    /* cmdpicode1 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay1 : 2;
    /* cmdpilogicdelay1 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode2 : 6;
    /* cmdpicode2 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay2 : 2;
    /* cmdpilogicdelay2 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1C0C)                                                  */
/*       SKX (0x402B1C0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG 0x08014C0C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = None 
       Enable Visa debug bus LCB for CMD fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = None 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = None 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 dllmask : 2;
    /* dllmask - Bits[5:4], RW, default = 1'b0 
       On weak lock exit, controls how long the feedback to the phase detector is 
       masked: {0: 1 qclk (required for delayline latency), 1: 2 qclk, 2: 3 qclk, 3: 4 
       qclk} 
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[6:6], RW, default = 1'b0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CMD transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used 
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 1'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 cmdtxeq : 2;
    /* cmdtxeq - Bits[14:13], RW, default = 1'b0 
       not used
     */
    UINT32 earlyweakdrive : 2;
    /* earlyweakdrive - Bits[16:15], RW, default = 1'b0 
       Controls the number of segments enabled during the early warning weak drive: {0: 
       All Enabled, 1: Enable Seg[4], 2: Enable Seg[4:3], 3: Enable Seg[4:2]} 
     */
    UINT32 lvmode : 2;
    /* lvmode - Bits[18:17], RW, default = None 
       not used
               
     */
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[19:19], RW, default = None 
       not used
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[20:20], RW, default = None 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 6'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefdisable : 1;
    /* vrefdisable - Bits[29:29], RW, default = 1'b0 
        Vref Disable
     */
    UINT32 ddr4modeenable : 1;
    /* ddr4modeenable - Bits[30:30], RW, default = 1'b0 
       Ddr4 Mode Enable, set to 1 in CMD2 (Func:5.Addr:C00 and Func:7.Addr:C00) fub to 
       bypass extra delay on parity bit 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS2_CMDS_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1C10)                                                  */
/*       SKX (0x402B1C10)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_CMDS_MCIO_DDRIOEXT_REG 0x08014C10
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 21;
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[21:21], RW, default = None 
       Enable finer vref resolution in ddr4
               
     */
    UINT32 rsvd_22 : 1;
    UINT32 cmdrxbiassel : 1;
    /* cmdrxbiassel - Bits[23:23], RW, default = 9'b0 
       
       	 CmdRxBiasSel for Cmd Receiver. 
         SKX b305822 - bit is floating and not connected to RxBiasSel in ddrcmdctl fub 
       design.  
          Refer to description for DdrCrCmdControls2.FnvCrCmdIsCke for RxBiasSel 
       control. 
       	
     */
    UINT32 forcecmdbiason : 1;
    /* forcecmdbiason - Bits[24:24], RW, default = 9'b1 
       ForceCmdBiason for Cmd Receiver
     */
    UINT32 rsvd_25 : 1;
    UINT32 cmdctlecapen : 2;
    /* cmdctlecapen - Bits[27:26], RW, default = 9'b0 
       CmdCtl Buffer CtleCapEn
     */
    UINT32 cmdctleresen : 2;
    /* cmdctleresen - Bits[29:28], RW, default = 9'b0 
       CmdCtl Buffer CtleResEn
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS2_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLCOMP_CMDS_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1C14)                                                  */
/*       SKX (0x402B1C14)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMP_CMDS_MCIO_DDRIOEXT_REG 0x08014C14
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RW, default = 1'b0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RW, default = 1'b0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RW, default = 1'b0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RW, default = 1'b0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RW, default = 1'b0 
       LevelShift Comp Value
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = 1'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG supported on:                    */
/*       SKX_A0 (0x402B1C18)                                                  */
/*       SKX (0x402B1C18)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG 0x08014C18
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 cmdctlbufsel : 6;
    /* cmdctlbufsel - Bits[23:18], RW, default = 1'b0 
       CmdBuf or CtlBuf Selection 
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[24:24], RW, default = 1'b0 
       Is the Direction (RX/TX Valid)
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1C1C)                                                  */
/*       SKX (0x402B1C1C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG 0x08014C1C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls 3. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 bufdrvsegen : 5;
    /* bufdrvsegen - Bits[4:0], RW, default = 5'b11111 
       Drive strength enable for CmdBuf/CtlBuf.
     */
    UINT32 cmdvrefpgen : 1;
    /* cmdvrefpgen - Bits[5:5], RW, default = 1'b1 
       control the DdrVrefSelDQQnnnH pin on the ddrvrefint generator
     */
    UINT32 odtstrength : 1;
    /* odtstrength - Bits[6:6], RW, default = 1'b1 
       OdtStrength 
     */
    UINT32 rsvd_7 : 2;
    UINT32 divby2alignctl : 1;
    /* divby2alignctl - Bits[9:9], RW, default = None 
       Invert DAlign 
     */
    UINT32 divby2disable : 1;
    /* divby2disable - Bits[10:10], RW, default = None 
       select qclk for cmdxover instead of Dclk 
     */
    UINT32 odten : 12;
    /* odten - Bits[22:11], RW, default = None 
       ODTDis for Cmd and Ctl buf
     */
    UINT32 cmdrefpiclkdelay : 6;
    /* cmdrefpiclkdelay - Bits[28:23], RW, default = None 
       CmdRefClk
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLRANKSUSED_CMDS_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1C20)                                                  */
/*       SKX (0x402B1C20)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_CMDS_MCIO_DDRIOEXT_REG 0x08014C20
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 6;
    /* ranken - Bits[5:0], RW, default = 6'b111111 
       Enables output buffers for ctlbufpair 0 to ctlbufpair6 , PI clocks and output 
       datapaths for this rank 
     */
    UINT32 rsvd_6 : 6;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[12:12], RW, default = 1'b0 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[14:13], RW, default = 2'b00 
       used to compensate the GCN skew. The GCN skew information should be
                 extracted by reading and analyzing the final Rcomp code value. The 
       target spec 
                 is to categorize the GCN skew in 3 process condition (mainly slow, 
       fast, typical). 
                 (one example can be just reading the 2 MLS bit of Rcomp code and 
       determine if the 
                 GCN is slow, fast or typical). The gcncomp is used for CTLE.
                 11: gcn in slow corner
                 10: gcn in typical corner
                 01: gcn in fast corner
               
     */
    UINT32 rsvd_15 : 1;
    UINT32 imodebiashighcm : 1;
    /* imodebiashighcm - Bits[16:16], RW, default = 1'b0 
       Imode bias control
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[17:17], RW, default = 1'b0 
       
       	 Rx bias Sel.
         SKX b305822 - bit is floating and not connected to RxBiasSel in ddrcmdctl fub 
       design. 
          Refer to description for DdrCrCmdControls2.FnvCrCmdIsCke for RxBiasSel 
       control. 
       	
     */
    UINT32 cmdrxeqqnnnl : 3;
    /* cmdrxeqqnnnl - Bits[20:18], RW, default = 12'b0 
       CmdRxEqQnnnL for RX CmdCtl Buf 
     */
    UINT32 rsvd_21 : 3;
    UINT32 ddrcmdrxbiasctl : 3;
    /* ddrcmdrxbiasctl - Bits[26:24], RW, default = 2'b0 
       CmdRxBiasCtl for RX biasgen 
     */
    UINT32 cmdpcasbiasclosedloopen : 1;
    /* cmdpcasbiasclosedloopen - Bits[27:27], RW, default = 2'b0 
       CmdPcasBiasClosedLoopEnQnnnH for RX biasgen 
     */
    UINT32 ddrcmdrxbiasfoldedlegtrim : 2;
    /* ddrcmdrxbiasfoldedlegtrim - Bits[29:28], RW, default = 2'b0 
       DdrCmdRxBiasFoldedLegTrim for RX biasgen 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLRANKSUSED_CMDS_MCIO_DDRIOEXT_STRUCT;






/* DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1C2C)                                                  */
/*       SKX (0x402B1C2C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_REG 0x08014C2C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Cmd Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 spare0 : 2;
    /* spare0 - Bits[7:6], RO_V, default = None 
       Spare
               
     */
    UINT32 xoverphasedet : 14;
    /* xoverphasedet - Bits[21:8], RO_V, default = 14'b0 
       bits13:6 unused
                            bit5 output of phase detector for I/O Pair 0
                            bit4 output of phase detector for I/O Pair 1
                            bit3 output of phase detector for I/O Pair 2
                            bit2 output of phase detector for I/O Pair 3
                            bit1 output of phase detector for I/O Pair 4
                            bit0 output of phase detector for I/O Pair 5 (only on 
       cmdctla FUBs) 
               
     */
    UINT32 trainingoffset2 : 6;
    /* trainingoffset2 - Bits[27:22], RW, default = 6'b0 
       Offset added to PiCode. Obtained via xover calibration such that ClkPi aligns 
       with grid clock when PiCode=0. 
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1C30)                                                  */
/*       SKX (0x402B1C30)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_REG 0x08014C30
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled  
                   for deemphasized, and so on.
               
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */

    UINT32 spare1 : 3;
    /* spare1 - Bits[15:13], RW, default = 3'b0 
       Imode coef
     */
    UINT32 selectrefresh : 1;
    /* selectrefresh - Bits[16:16], RW, default = 1'b0 
       select refresh fall space 
     */

    UINT32 rsvd_13 : 5;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled  
                   for deemphasized, and so on.
               
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_STRUCT;



/* DDRCRCMDPICODING2_CMDS_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1C34)                                                  */
/*       SKX (0x402B1C34)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_CMDS_MCIO_DDRIOEXT_REG 0x08014C34
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode3 : 6;
    /* cmdpicode3 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay3 : 2;
    /* cmdpilogicdelay3 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode4 : 6;
    /* cmdpicode4 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay4 : 2;
    /* cmdpilogicdelay4 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode5 : 6;
    /* cmdpicode5 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay5 : 2;
    /* cmdpilogicdelay5 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING2_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDPICODING3_CMDS_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1C38)                                                  */
/*       SKX (0x402B1C38)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_CMDS_MCIO_DDRIOEXT_REG 0x08014C38
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode6 : 6;
    /* cmdpicode6 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay6 : 2;
    /* cmdpilogicdelay6 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING3_CMDS_MCIO_DDRIOEXT_STRUCT;




/* VSSHIORVREFCONTROL_CMDS_MCIO_DDRIOEXT_REG supported on:                    */
/*       SKX_A0 (0x402B1C40)                                                  */
/*       SKX (0x402B1C40)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_CMDS_MCIO_DDRIOEXT_REG 0x08014C40
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings. 
       
               VssHi Field Description:
       
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode          23     Selects the code to output:  {0: Code, 1: TCode}
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode 
               PanicVoltage     21:18  Unsigned integer controlling the voltage error 
       on VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV) 
               CloseLoop        17     Enables the VssHi close loop tracking
               PanicEn          16     Enables the VssHi Panic Driver
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError) 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128} 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
       
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL_CMDS_MCIO_DDRIOEXT_STRUCT;




/* VSSHIORVREFCONTROL1_CMDS_MCIO_DDRIOEXT_REG supported on:                   */
/*       SKX_A0 (0x402B1C48)                                                  */
/*       SKX (0x402B1C48)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_CMDS_MCIO_DDRIOEXT_REG 0x08014C48
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Extension for BDX. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 8;
    /* vsshiorvrefctl - Bits[7:0], RW, default = 8'b0 
        BDX-specific extended control signal. 
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL1_CMDS_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMP_CTL_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B1E00)                                                  */
/*       SKX (0x402B1E00)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_CTL_MCIO_DDRIOEXT_REG 0x08014E00
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RW, default = 1'b0 
       Scomp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RW, default = 1'b0 
       Tco Comp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RW, default = 1'b0 
       Rcomp Drv Up Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RW, default = 1'b0 
       Rcomp Drv Dn Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RW, default = 3'b010 
       not used
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = 1'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1E04)                                                  */
/*       SKX (0x402B1E04)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG 0x08014E04
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 ddrcmdctlrx_en : 12;
    /* ddrcmdctlrx_en - Bits[28:17], RW, default = 1'b0 
       Enable DDRT Parity in CMDCTL
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMPOFFSET_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDPICODING_CTL_MCIO_DDRIOEXT_REG supported on:                       */
/*       SKX_A0 (0x402B1E08)                                                  */
/*       SKX (0x402B1E08)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_CTL_MCIO_DDRIOEXT_REG 0x08014E08
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode0 : 6;
    /* cmdpicode0 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay0 : 2;
    /* cmdpilogicdelay0 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay - add 1 qclk cycle delay.
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode1 : 6;
    /* cmdpicode1 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay1 : 2;
    /* cmdpilogicdelay1 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode2 : 6;
    /* cmdpicode2 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay2 : 2;
    /* cmdpilogicdelay2 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG supported on:                       */
/*       SKX_A0 (0x402B1E0C)                                                  */
/*       SKX (0x402B1E0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG 0x08014E0C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = None 
       Enable Visa debug bus LCB for CMD fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = None 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = None 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 dllmask : 2;
    /* dllmask - Bits[5:4], RW, default = 1'b0 
       On weak lock exit, controls how long the feedback to the phase detector is 
       masked: {0: 1 qclk (required for delayline latency), 1: 2 qclk, 2: 3 qclk, 3: 4 
       qclk} 
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[6:6], RW, default = 1'b0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CMD transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used 
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 1'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 cmdtxeq : 2;
    /* cmdtxeq - Bits[14:13], RW, default = 1'b0 
       not used
     */
    UINT32 earlyweakdrive : 2;
    /* earlyweakdrive - Bits[16:15], RW, default = 1'b0 
       Controls the number of segments enabled during the early warning weak drive: {0: 
       All Enabled, 1: Enable Seg[4], 2: Enable Seg[4:3], 3: Enable Seg[4:2]} 
     */
    UINT32 lvmode : 2;
    /* lvmode - Bits[18:17], RW, default = None 
       not used
               
     */
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[19:19], RW, default = None 
       not used
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[20:20], RW, default = None 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 6'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefdisable : 1;
    /* vrefdisable - Bits[29:29], RW, default = 1'b0 
        Vref Disable
     */
    UINT32 ddr4modeenable : 1;
    /* ddr4modeenable - Bits[30:30], RW, default = 1'b0 
       Ddr4 Mode Enable, set to 1 in CMD2 (Func:5.Addr:C00 and Func:7.Addr:C00) fub to 
       bypass extra delay on parity bit 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS2_CTL_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1E10)                                                  */
/*       SKX (0x402B1E10)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_CTL_MCIO_DDRIOEXT_REG 0x08014E10
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 21;
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[21:21], RW, default = None 
       Enable finer vref resolution in ddr4
               
     */
    UINT32 rsvd_22 : 1;
    UINT32 cmdrxbiassel : 1;
    /* cmdrxbiassel - Bits[23:23], RW, default = 9'b0 
       
       	 CmdRxBiasSel for Cmd Receiver. 
         SKX b305822 - bit is floating and not connected to RxBiasSel in ddrcmdctl fub 
       design.  
          Refer to description for DdrCrCmdControls2.FnvCrCmdIsCke for RxBiasSel 
       control. 
       	
     */
    UINT32 forcecmdbiason : 1;
    /* forcecmdbiason - Bits[24:24], RW, default = 9'b1 
       ForceCmdBiason for Cmd Receiver
     */
    UINT32 rsvd_25 : 1;
    UINT32 cmdctlecapen : 2;
    /* cmdctlecapen - Bits[27:26], RW, default = 9'b0 
       CmdCtl Buffer CtleCapEn
     */
    UINT32 cmdctleresen : 2;
    /* cmdctleresen - Bits[29:28], RW, default = 9'b0 
       CmdCtl Buffer CtleResEn
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS2_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLCOMP_CTL_MCIO_DDRIOEXT_REG supported on:                           */
/*       SKX_A0 (0x402B1E14)                                                  */
/*       SKX (0x402B1E14)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMP_CTL_MCIO_DDRIOEXT_REG 0x08014E14
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RW, default = 1'b0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RW, default = 1'b0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RW, default = 1'b0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RW, default = 1'b0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RW, default = 1'b0 
       LevelShift Comp Value
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = 1'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1E18)                                                  */
/*       SKX (0x402B1E18)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG 0x08014E18
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 cmdctlbufsel : 6;
    /* cmdctlbufsel - Bits[23:18], RW, default = 1'b0 
       CmdBuf or CtlBuf Selection 
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[24:24], RW, default = 1'b0 
       Is the Direction (RX/TX Valid)
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1E1C)                                                  */
/*       SKX (0x402B1E1C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG 0x08014E1C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls 3. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 bufdrvsegen : 5;
    /* bufdrvsegen - Bits[4:0], RW, default = 5'b11111 
       Drive strength enable for CmdBuf/CtlBuf.
     */
    UINT32 cmdvrefpgen : 1;
    /* cmdvrefpgen - Bits[5:5], RW, default = 1'b1 
       control the DdrVrefSelDQQnnnH pin on the ddrvrefint generator
     */
    UINT32 odtstrength : 1;
    /* odtstrength - Bits[6:6], RW, default = 1'b1 
       OdtStrength 
     */
    UINT32 rsvd_7 : 2;
    UINT32 divby2alignctl : 1;
    /* divby2alignctl - Bits[9:9], RW, default = None 
       Invert DAlign 
     */
    UINT32 divby2disable : 1;
    /* divby2disable - Bits[10:10], RW, default = None 
       select qclk for cmdxover instead of Dclk 
     */
    UINT32 odten : 12;
    /* odten - Bits[22:11], RW, default = None 
       ODTDis for Cmd and Ctl buf
     */
    UINT32 cmdrefpiclkdelay : 6;
    /* cmdrefpiclkdelay - Bits[28:23], RW, default = None 
       CmdRefClk
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCTLRANKSUSED_CTL_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1E20)                                                  */
/*       SKX (0x402B1E20)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_CTL_MCIO_DDRIOEXT_REG 0x08014E20
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CTL Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 6;
    /* ranken - Bits[5:0], RW, default = 6'b111111 
       Enables output buffers for ctlbufpair 0 to ctlbufpair6 , PI clocks and output 
       datapaths for this rank 
     */
    UINT32 rsvd_6 : 6;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[12:12], RW, default = 1'b0 
       level shift hold enable
                            Holds the HV control values and power down mode the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[14:13], RW, default = 2'b00 
       used to compensate the GCN skew. The GCN skew information should be
                 extracted by reading and analyzing the final Rcomp code value. The 
       target spec 
                 is to categorize the GCN skew in 3 process condition (mainly slow, 
       fast, typical). 
                 (one example can be just reading the 2 MLS bit of Rcomp code and 
       determine if the 
                 GCN is slow, fast or typical). The gcncomp is used for CTLE.
                 11: gcn in slow corner
                 10: gcn in typical corner
                 01: gcn in fast corner
               
     */
    UINT32 rsvd_15 : 1;
    UINT32 imodebiashighcm : 1;
    /* imodebiashighcm - Bits[16:16], RW, default = 1'b0 
       Imode bias control
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[17:17], RW, default = 1'b0 
       
       	 Rx bias Sel.
         SKX b305822 - bit is floating and not connected to RxBiasSel in ddrcmdctl fub 
       design. 
          Refer to description for DdrCrCmdControls2.FnvCrCmdIsCke for RxBiasSel 
       control. 
       	
     */
    UINT32 cmdrxeqqnnnl : 3;
    /* cmdrxeqqnnnl - Bits[20:18], RW, default = 12'b0 
       CmdRxEqQnnnL for RX CmdCtl Buf 
     */
    UINT32 rsvd_21 : 3;
    UINT32 ddrcmdrxbiasctl : 3;
    /* ddrcmdrxbiasctl - Bits[26:24], RW, default = 2'b0 
       CmdRxBiasCtl for RX biasgen 
     */
    UINT32 cmdpcasbiasclosedloopen : 1;
    /* cmdpcasbiasclosedloopen - Bits[27:27], RW, default = 2'b0 
       CmdPcasBiasClosedLoopEnQnnnH for RX biasgen 
     */
    UINT32 ddrcmdrxbiasfoldedlegtrim : 2;
    /* ddrcmdrxbiasfoldedlegtrim - Bits[29:28], RW, default = 2'b0 
       DdrCmdRxBiasFoldedLegTrim for RX biasgen 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLRANKSUSED_CTL_MCIO_DDRIOEXT_STRUCT;






/* DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_REG supported on:                       */
/*       SKX_A0 (0x402B1E2C)                                                  */
/*       SKX (0x402B1E2C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_REG 0x08014E2C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Cmd Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 spare0 : 2;
    /* spare0 - Bits[7:6], RO_V, default = None 
       Spare
               
     */
    UINT32 xoverphasedet : 14;
    /* xoverphasedet - Bits[21:8], RO_V, default = 14'b0 
       bits13:6 unused
                            bit5 output of phase detector for I/O Pair 0
                            bit4 output of phase detector for I/O Pair 1
                            bit3 output of phase detector for I/O Pair 2
                            bit2 output of phase detector for I/O Pair 3
                            bit1 output of phase detector for I/O Pair 4
                            bit0 output of phase detector for I/O Pair 5 (only on 
       cmdctla FUBs) 
               
     */
    UINT32 trainingoffset2 : 6;
    /* trainingoffset2 - Bits[27:22], RW, default = 6'b0 
       Offset added to PiCode. Obtained via xover calibration such that ClkPi aligns 
       with grid clock when PiCode=0. 
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCONTROLS1_CTL_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1E30)                                                  */
/*       SKX (0x402B1E30)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_CTL_MCIO_DDRIOEXT_REG 0x08014E30
/* Struct format extracted from XML file SKX_A0\2.22.1.CFG.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled  
                   for deemphasized, and so on.
               
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_CTL_MCIO_DDRIOEXT_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 1'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled  
                   for deemphasized, and so on.
               
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_CTL_MCIO_DDRIOEXT_STRUCT;



/* DDRCRCMDPICODING2_CTL_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1E34)                                                  */
/*       SKX (0x402B1E34)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_CTL_MCIO_DDRIOEXT_REG 0x08014E34
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode3 : 6;
    /* cmdpicode3 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay3 : 2;
    /* cmdpilogicdelay3 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode4 : 6;
    /* cmdpicode4 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay4 : 2;
    /* cmdpilogicdelay4 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode5 : 6;
    /* cmdpicode5 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay5 : 2;
    /* cmdpilogicdelay5 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING2_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDPICODING3_CTL_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1E38)                                                  */
/*       SKX (0x402B1E38)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_CTL_MCIO_DDRIOEXT_REG 0x08014E38
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode6 : 6;
    /* cmdpicode6 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay6 : 2;
    /* cmdpilogicdelay6 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING3_CTL_MCIO_DDRIOEXT_STRUCT;




/* VSSHIORVREFCONTROL_CTL_MCIO_DDRIOEXT_REG supported on:                     */
/*       SKX_A0 (0x402B1E40)                                                  */
/*       SKX (0x402B1E40)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_CTL_MCIO_DDRIOEXT_REG 0x08014E40
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings. 
       
               VssHi Field Description:
       
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode          23     Selects the code to output:  {0: Code, 1: TCode}
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode 
               PanicVoltage     21:18  Unsigned integer controlling the voltage error 
       on VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV) 
               CloseLoop        17     Enables the VssHi close loop tracking
               PanicEn          16     Enables the VssHi Panic Driver
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError) 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128} 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples} 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
       
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL_CTL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCMDCOMP1_CTL_MCIO_DDRIOEXT_REG supported on:                          */
/*       SKX_A0 (0x402B1E44)                                                  */
/*       SKX (0x402B1E44)                                                     */
/* Register default value on SKX_A0:    0x00080205                            */
/* Register default value on SKX:       0x00000000                            */
#define DDRCRCMDCOMP1_CTL_MCIO_DDRIOEXT_REG 0x08014E44
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Cmd Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vtcomp : 5;
    /* vtcomp - Bits[4:0], RW, default = 5'b00000 
       VT Comp Values
     */
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[14:5], RW, default = 10'b0000000000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[24:15], RW, default = 10'b0000000000 
       Panic DrvUp RComp Comp Value.
     */
    UINT32 spare : 7;
    /* spare - Bits[31:25], RW, default = 7'h00 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP1_CTL_MCIO_DDRIOEXT_STRUCT;


/* VSSHIORVREFCONTROL1_CTL_MCIO_DDRIOEXT_REG supported on:                    */
/*       SKX_A0 (0x402B1E48)                                                  */
/*       SKX (0x402B1E48)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_CTL_MCIO_DDRIOEXT_REG 0x08014E48
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Extension for BDX. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 8;
    /* vsshiorvrefctl - Bits[7:0], RW, default = 8'b0 
        BDX-specific extended control signal. 
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL1_CTL_MCIO_DDRIOEXT_STRUCT;




/* DDRCRDATACOMP1_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B1F08)                                                  */
/*       SKX (0x402B1F08)                                                     */
/* Register default value:              0x0C030000                            */
#define DDRCRDATACOMP1_MCIO_DDRIOEXT_REG 0x08014F08
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds the second (of two) 32-bit register set for Data fub Compensation
 */
typedef union {
  struct {
    UINT32 rcompodtup : 6;
    /* rcompodtup - Bits[5:0], RW, default = None 
       Sets the resistive value of the pull-up block of on die termination 
     */
    UINT32 rcompodtdown : 6;
    /* rcompodtdown - Bits[11:6], RW, default = None 
       Sets the resistive value of the pull-dn block of on die termination
     */
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[21:12], RW, default = 10'b0000110000 
       Sets the resistive value of the VssHi Panic driver pulldown to move VssHi by a 
       precise dV when it gets out of spec 
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[31:22], RW, default = 10'b0000110000 
       Sets the resistive value of the VssHi Panic driver pullup to move VssHi by a 
       precise dV when it gets out of spec. 
     */
  } Bits;
  UINT32 Data;
} DDRCRDATACOMP1_MCIO_DDRIOEXT_STRUCT;




/* DDRCRCTLCOMP_MCIO_DDRIOEXT_REG supported on:                               */
/*       SKX_A0 (0x402B1F10)                                                  */
/*       SKX (0x402B1F10)                                                     */
/* Register default value:              0x34000000                            */
#define DDRCRCTLCOMP_MCIO_DDRIOEXT_REG 0x08014F10
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds the 32-bit register set for Ctl fub Compensation
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 28;
    UINT32 ctlstatlegen : 2;
    /* ctlstatlegen - Bits[29:28], RW, default = 2'b11 
       Ctl Comp Loop Static Leg Config
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_MCIO_DDRIOEXT_STRUCT;




/* DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG supported on:                              */
/*       SKX_A0 (0x402B1F18)                                                  */
/*       SKX (0x402B1F18)                                                     */
/* Register default value:              0x40800007                            */
#define DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG 0x08014F18
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds Compensation Controls like Vref Offset etc
 */
typedef union {
  struct {
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[0:0], RW, default = 1'b1  */
    UINT32 lvmode : 2;
    /* lvmode - Bits[2:1], RW, default = 2'b11 
       low voltage mode (DDR3LV, DDR3U, DDR4)
                   01: DDR3LV
                   10: DDR3U
                   11: DDR4
               DIMM support is processor type specific. 
               
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[3:3], RW, default = None 
       Enable Visa debug bus LCB for COMP fub
     */
    UINT32 dqodtupdnoff : 6;
    /* dqodtupdnoff - Bits[9:4], RW, default = None 
       Offset between DqOdt Up and Down
     */
    UINT32 fixodtd : 1;
    /* fixodtd - Bits[10:10], RW, default = None 
       Skip DqRcomOdtDn compensation and use DqOdtDn = DqOdtUp + DqOdtUpDnOff
     */
    UINT32 dqdrvpupvref : 7;
    /* dqdrvpupvref - Bits[17:11], RW, default = None 
       2's compliment analog offset for DqRcompDrvUp Vref with step size of Vccddq/64
     */
    UINT32 dqdrvpdnvref : 7;
    /* dqdrvpdnvref - Bits[24:18], RW, default = 7'b0100000 
       2's compliment analog offset for DqRcompDrvDown Vref with step size of Vccddq/96
     */
    UINT32 dqodtpupvref : 7;
    /* dqodtpupvref - Bits[31:25], RW, default = 7'b0100000 
       2's compliment analog offset for DqOdtcompDrvUp Vref with step size of Vccddq/96
     */
  } Bits;
  UINT32 Data;
} DDRCRCOMPCTL0_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCOMPCTL1_MCIO_DDRIOEXT_REG supported on:                              */
/*       SKX_A0 (0x402B1F1C)                                                  */
/*       SKX (0x402B1F1C)                                                     */
/* Register default value:              0x08000C79                            */
#define DDRCRCOMPCTL1_MCIO_DDRIOEXT_REG 0x08014F1C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds Compensation Controls like Scomp Offset etc
 */
typedef union {
  struct {
    UINT32 dqscompcells : 4;
    /* dqscompcells - Bits[3:0], RW, default = 4'b1001 
       # of delay cells in DqScomp delayline. (Min Setting > 3)
     */
    UINT32 dqscomppc : 1;
    /* dqscomppc - Bits[4:4], RW, default = 1'b1 
       When set (1'b1), SCOMP locks to a cycle, otherwise locks to a phase
     */
    UINT32 cmdscompcells : 4;
    /* cmdscompcells - Bits[8:5], RW, default = 3'b011 
       # of delay cells in CmdScomp delayline = CmdScompCells
     */
    UINT32 cmdscomppc : 1;
    /* cmdscomppc - Bits[9:9], RW, default = None 
       When set (1'b1), SCOMP locks to a cycle, otherwise locks to a phase
     */
    UINT32 ctlscompcells : 4;
    /* ctlscompcells - Bits[13:10], RW, default = 3'b011 
       # of delay cells in CtlScomp delayline = CtlScompCells
     */
    UINT32 ctlscomppc : 1;
    /* ctlscomppc - Bits[14:14], RW, default = None 
       When set (1'b1), SCOMP locks to a cycle, otherwise locks to a phase
     */
    UINT32 rsvd_15 : 6;
    UINT32 tcovref : 7;
    /* tcovref - Bits[27:21], RW, default = 8'b01000000 
       Vref control for TcoComp loop, can be used to offset FSM result
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCOMPCTL1_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCOMPCTL2_MCIO_DDRIOEXT_REG supported on:                              */
/*       SKX_A0 (0x402B1F20)                                                  */
/*       SKX (0x402B1F20)                                                     */
/* Register default value:              0x40808083F                           */
#define DDRCRCOMPCTL2_MCIO_DDRIOEXT_REG 0x08014F20
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds Compensation Controls like ODT static leg, Rxbias etc
 */
typedef union {
  struct {
    UINT32 odtstatlegen : 2;
    /* odtstatlegen - Bits[1:0], RW, default = 2'b11 
       ODT Comp Loop Staic Leg Config. 00: 0-leg; 01: 3-leg; 10: 6-leg; 11: 12-leg
     */
    UINT32 txdqstatlegen : 2;
    /* txdqstatlegen - Bits[3:2], RW, default = 2'b11 
       DQ Comp Loop Staic Leg Config. 00: 0-leg; 01: 3-leg; 10: 6-leg; 11: 12-leg
     */
    UINT32 cmdstatlegen : 2;
    /* cmdstatlegen - Bits[5:4], RW, default = 2'b11 
       Cmd Comp Loop Staic Leg Config. 00: 0-leg; 01: 3-leg; 10: 6-leg; 11: 12-leg
     */
    UINT32 dqodtpdnvref : 7;
    /* dqodtpdnvref - Bits[12:6], RW, default = 8'b00100000 
       unsiged analog offset for DqOdtRcompDrvDown Vref with step size of 0.75mW
     */
    UINT32 clkdrvpupvref : 7;
    /* clkdrvpupvref - Bits[19:13], RW, default = 8'b00100000 
       unsiged analog offset for ClkRcompDrvUp Vref with step size of 0.75mV
     */
    UINT32 clkdrvpdnvref : 7;
    /* clkdrvpdnvref - Bits[26:20], RW, default = 8'b00100000 
       unsigned analog offset for ClkRcompDrvDown Vref with step size of 0.75mV
     */
    UINT32 rsvd_27 : 1;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[28:28], RW, default = None 
       level shift hold enable
                            Holds the HV control values and power down the level 
       shifters. 
                            Apply "1" after training.  Slow timing - not advised for 
       fast enabling. 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[29:29], RW, default = None 
       Force ODT on used in dq/clk/ctl rcomp AIP
     */
    UINT32 ddr3nren : 1;
    /* ddr3nren - Bits[30:30], RW, default = 2'b0 
       DDR3 Narrow Range
     */
    UINT32 highbwen : 1;
    /* highbwen - Bits[31:31], RW, default = 1'b1 
       highbwen for panicvtcomp
     */
  } Bits;
  UINT32 Data;
} DDRCRCOMPCTL2_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCOMPVSSHI_MCIO_DDRIOEXT_REG supported on:                             */
/*       SKX_A0 (0x402B1F24)                                                  */
/*       SKX (0x402B1F24)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCOMPVSSHI_MCIO_DDRIOEXT_REG 0x08014F24
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds Compensation Controls for VssHi
 */
typedef union {
  struct {
    UINT32 panicdrvdnvref : 6;
    /* panicdrvdnvref - Bits[5:0], RW, default = None 
       Unsigned integer to select Vref for Panic DriverDn compensation. Step size of 
       VccIO/128 
     */
    UINT32 panicdrvupvref : 6;
    /* panicdrvupvref - Bits[11:6], RW, default = None 
       Unsigned integer to select Vref for Panic DriverUp compensation. Step size of 
       VccIO/128 
     */
    UINT32 vtoffset : 5;
    /* vtoffset - Bits[16:12], RW, default = None 
       Digital Offset to VtComp value.  Unsigned interger in the VccIO domain with a 
       step size of VccIO/128 
     */
    UINT32 vtslopea : 3;
    /* vtslopea - Bits[19:17], RW, default = None 
       Slope adjustment to the VtComp value.  {0: 0, 1: -1/4, 2: -1/8, 3: -1/16, 4: 
       +1/2, 5: +1/4, 6: +1/8, 7: +1/16} 
     */
    UINT32 vtslopeb : 3;
    /* vtslopeb - Bits[22:20], RW, default = None 
       Slope adjustment to the VtComp value.  {0: 0, 1: -1/4, 2: -1/8, 3: -1/16, 4: 
       +1/2, 5: +1/4, 6: +1/8, 7: +1/16} 
     */
    UINT32 clkdiv : 3;
    /* clkdiv - Bits[25:23], RW, default = None 
       divide by clk select for panicvt comp
     */
    UINT32 ddrdmvrfs3localpwrgoodoverride : 1;
    /* ddrdmvrfs3localpwrgoodoverride - Bits[26:26], RW, default = None 
       When this CSR bit is cleared,  S3LocalVcc in DIMM Vref is always on
     */
    UINT32 disablemcdfxclkqnnnh : 1;
    /* disablemcdfxclkqnnnh - Bits[27:27], RW, default = None 
       Spare
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RW, default = None 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCOMPVSSHI_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCOMPOVR_MCIO_DDRIOEXT_REG supported on:                               */
/*       SKX_A0 (0x402B1F28)                                                  */
/*       SKX (0x402B1F28)                                                     */
/* Register default value:              0x010F0000                            */
#define DDRCRCOMPOVR_MCIO_DDRIOEXT_REG 0x08014F28
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds bits for Override Enable for different Compensations
 */
typedef union {
  struct {
    UINT32 dqdrvu : 1;
    /* dqdrvu - Bits[0:0], RW, default = None 
       Override Data Rcomp DriveUp Value
     */
    UINT32 dqdrvd : 1;
    /* dqdrvd - Bits[1:1], RW, default = None 
       Override Data Rcomp DriveDn Value
     */
    UINT32 dqodtu : 1;
    /* dqodtu - Bits[2:2], RW, default = None 
       Override Data Rcomp OdtUp Value
     */
    UINT32 dqodtd : 1;
    /* dqodtd - Bits[3:3], RW, default = None 
       Override Data Rcomp OdtDn Value
     */
    UINT32 cmddrvu : 1;
    /* cmddrvu - Bits[4:4], RW, default = None 
       Override Cmd Rcomp DriveUp Value
     */
    UINT32 cmddrvd : 1;
    /* cmddrvd - Bits[5:5], RW, default = None 
       Override Cmd Rcomp DriveDn Value
     */
    UINT32 ctldrvu : 1;
    /* ctldrvu - Bits[6:6], RW, default = None 
       Override Ctl Rcomp DriveUp Value
     */
    UINT32 ctldrvd : 1;
    /* ctldrvd - Bits[7:7], RW, default = None 
       Override Ctl Rcomp DriveDn Value
     */
    UINT32 clkdrvu : 1;
    /* clkdrvu - Bits[8:8], RW, default = None 
       Override Clk Rcomp DriveUp Value
     */
    UINT32 clkdrvd : 1;
    /* clkdrvd - Bits[9:9], RW, default = None 
       Override Clk Rcomp DriveDn Value
     */
    UINT32 dqsr : 1;
    /* dqsr - Bits[10:10], RW, default = None 
       Override Data Scomp Value
     */
    UINT32 cmdsr : 1;
    /* cmdsr - Bits[11:11], RW, default = None 
       Override Cmd Scomp Value
     */
    UINT32 ctlsr : 1;
    /* ctlsr - Bits[12:12], RW, default = None 
       Override Ctl Scomp Value
     */
    UINT32 clksr : 1;
    /* clksr - Bits[13:13], RW, default = None 
       Override Clk Scomp Value
     */
    UINT32 dqtcooff : 1;
    /* dqtcooff - Bits[14:14], RW, default = None 
       Override Data TcoOffset Value
     */
    UINT32 cmdtcooff : 1;
    /* cmdtcooff - Bits[15:15], RW, default = None 
       Override Cmd TcoOffset Value
     */
    UINT32 dqtco : 1;
    /* dqtco - Bits[16:16], RW, default = 1'b1 
       Override Data Tco Value
     */
    UINT32 cmdtco : 1;
    /* cmdtco - Bits[17:17], RW, default = 1'b1 
       Override Cmd Tco Value
     */
    UINT32 ctltco : 1;
    /* ctltco - Bits[18:18], RW, default = 1'b1 
       Override Ctl Tco Value
     */
    UINT32 clktco : 1;
    /* clktco - Bits[19:19], RW, default = 1'b1 
       Override Clk Tco Value
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[20:20], RW, default = None 
       Enable finer vref resolution in ddr4
     */
    UINT32 panicdrvup : 1;
    /* panicdrvup - Bits[21:21], RW, default = None 
       Override Panic DriveUp Value
     */
    UINT32 panicdrvdn : 1;
    /* panicdrvdn - Bits[22:22], RW, default = None 
       Override Panic DriveDn Value
     */
    UINT32 vtcomp : 1;
    /* vtcomp - Bits[23:23], RW, default = None 
       Override VT Comp Value
     */
    UINT32 viewdigdrven : 1;
    /* viewdigdrven - Bits[24:24], RW, default = 1'b1 
       Override ViewDig DriveEn Value
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[25:25], RW, default = 1'b0 
        used to be VccddqHi in HSW, both are equivalent
                 1 - Nominal Vccddq voltage is 1.5v
                 0 - Nominal Vccddq voltage is 1.35v or 1.2v
               
     */
    UINT32 centerrptclkenovrd : 1;
    /* centerrptclkenovrd - Bits[26:26], RW, default = None 
       Override Clkgating signal for lcben in ifc
     */
    UINT32 rsvd_27 : 5;
  } Bits;
  UINT32 Data;
} DDRCRCOMPOVR_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCOMPVSSHICONTROL_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1F2C)                                                  */
/*       SKX (0x402B1F2C)                                                     */
/* Register default value:              0x004D8238                            */
#define DDRCRCOMPVSSHICONTROL_MCIO_DDRIOEXT_REG 0x08014F2C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds bits to control the VssHi Generation
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
       VssHi control bits
     */
    UINT32 outputcode : 8;
    /* outputcode - Bits[31:24], RO_V, default = None 
       Read the current code being generated by the VssHi feedback loop.  Use SelCode 
       field to control which instance is read.  
     */
  } Bits;
  UINT32 Data;
} DDRCRCOMPVSSHICONTROL_MCIO_DDRIOEXT_STRUCT;


/* DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1F30)                                                  */
/*       SKX (0x402B1F30)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG 0x08014F30
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * DimmVref Adjust Controls.
 */
typedef union {
  struct {
    UINT32 ch0cavrefctl : 8;
    /* ch0cavrefctl - Bits[7:0], RW, default = 8'b0 
        Ch0 CA DIMM Vref Control. 
                             2s compliment encoding with 0 = Vdd/2, range of +/- Vdd/6 
       and step size of Vdd/384 (3.9mV) 
               NOTE: LSB (bit 0) is a DFX feature (averaging between two codes) - only 
       use even codes so that LSB is '0'. 
               
     */
    UINT32 ch2cavrefctl : 8;
    /* ch2cavrefctl - Bits[15:8], RW, default = 8'b0 
        ch2 DQ DIMM Vref Control.
                             2s compliment encoding with 0 = Vdd/2, range of +/- Vdd/6 
       and step size of Vdd/384 (3.9mV) 
               NOTE: LSB (bit 7) is a DFX feature (averaging between two codes) - only 
       use even codes so that LSB is '0'. 
               
     */
    UINT32 ch1cavrefctl : 8;
    /* ch1cavrefctl - Bits[23:16], RW, default = 8'b0 
        ch1 DQ DIMM Vref Control.
                             2s compliment encoding with 0 = Vdd/2, range of +/- Vdd/6 
       and step size of Vdd/384 (3.9mV) 
               NOTE: LSB (bit 14) is a DFX feature - only use even codes so that LSB is 
       '0'. 
               
     */
    UINT32 ch0caendimmvref : 1;
    /* ch0caendimmvref - Bits[24:24], RW, default = 1'b0 
       Enables Ch0 CA DIMM Vref
     */
    UINT32 ch2caendimmvref : 1;
    /* ch2caendimmvref - Bits[25:25], RW, default = 1'b0 
       Enables Channel 2 DIMM Vref
     */
    UINT32 ch1caendimmvref : 1;
    /* ch1caendimmvref - Bits[26:26], RW, default = 1'b0 
       Enables Channel 1 DIMM Vref
     */
    UINT32 hiztimerctrl : 2;
    /* hiztimerctrl - Bits[28:27], RW, default = 2'b0 
        For the HiZ Timer, selects the number of clocks for the timer to expire 
                             {0: 4 cycles, 1: 8 cycles, 2: 16 cycles, 3: 32 cycles}
               
     */
    UINT32 ch0caslowbw : 1;
    /* ch0caslowbw - Bits[29:29], RO_V, default = 1'b0 
       Ch0 CA Vref controller has completed initial convergence and is in slow BW mode. 
        If used as a lock detector, add 5uS after slow BW mode is achieved for full 
       convergence 
     */
    UINT32 ch1caslowbw : 1;
    /* ch1caslowbw - Bits[30:30], RO_V, default = 1'b0 
       Ch1 Vref controller has completed initial convergence and is in slow BW mode.  
       If used as a lock detector, add 5uS after slow BW mode is achieved for full 
       convergence 
     */
    UINT32 ch2caslowbw : 1;
    /* ch2caslowbw - Bits[31:31], RO_V, default = 1'b0 
       Ch2 Vref controller has completed initial convergence and is in slow BW mode.  
       If used as a lock detector, add 5uS after slow BW mode is achieved for full 
       convergence 
     */
  } Bits;
  UINT32 Data;
} DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_STRUCT;


/* DDRCRDIMMVREFCONTROL2_MCIO_DDRIOEXT_REG supported on:                      */
/*       SKX_A0 (0x402B1F34)                                                  */
/*       SKX (0x402B1F34)                                                     */
/* Register default value:              0x000E4500                            */
#define DDRCRDIMMVREFCONTROL2_MCIO_DDRIOEXT_REG 0x08014F34
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. 
 */
typedef union {
  struct {
    UINT32 dimmvrefctl : 24;
    /* dimmvrefctl - Bits[23:0], RW, default = 24'h0E4500 
        DIMM VREF adjust settings.
                Field          Bits    Description
               -------         ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
               SelCode         23:21   Selects the code to output:  [0] = {0: Code, 1: 
       TCode}  [2:1] = {0: Select Ch0 CA Code, 1: Select DQCh1 Code, 2: Select DQCh2 
       Code}   
           VtOffset    20:18   Offset for VtComp to convert from VccIO to VccDDQ.  
       Signed integer in VccDDQ domain with a step size of VccDDQ/48 (32 mV) 
           VtSlopeA    17:15   Slope correction to convert between VtComp in VccIO to 
       VccDDQ: {0: 0, 1: -1/8, 2: -1/16, 3: -1/32. 4: +1/4, 5: +1/8, 6: +1/16. 7: 
       +1/32} 
           VtSlopeB    14:12   Slope correction to convert between VtComp in VccIO to 
       VccDDQ: {0: 0, 1: -1/8, 2: -1/16, 3: -1/32. 4: +1/4, 5: +1/8, 6: +1/16. 7: 
       +1/32} 
           Reserved        11      Reserved for future usage
           HiBWEnable  10  Enables high bandwidth mode after a code change to allow 
       fast loop convergence 
           SlowBWError 9:8 In slow BW mode, limits the loop gain (ie: difference 
       between code and tcode) to +/- 2^(SlowBWError) 
           OpenLoop    7   Operates the dimm vref in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved 
           SampleDivider   6:4 Controls the loop bandwidth by dividing down the input 
       clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 6: 
       Qclk/64, 7: Qclk/128} 
           LoBWDivider 3:2 For the slow integrator, selects the net number of samples 
       in a given direction to trip the filter.  {0: 4 samples, 1: 8 samples, 2: 16 
       samples, 3: 32 samples}  
           HiBWDivider 1:0 For the fast integrator, selects the net number of samples 
       in a given direction to trip the filter.  {0: 4 samples, 1: 8 samples, 2: 16 
       samples, 3: 32 samples}  
               
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} DDRCRDIMMVREFCONTROL2_MCIO_DDRIOEXT_STRUCT;


/* DDRCRSPDCFG1_MCIO_DDRIOEXT_REG supported on:                               */
/*       SKX_A0 (0x402B1F38)                                                  */
/*       SKX (0x402B1F38)                                                     */
/* Register default value:              0x014083F8                            */
#define DDRCRSPDCFG1_MCIO_DDRIOEXT_REG 0x08014F38
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * SPD Comp Config
 */
typedef union {
  struct {
    UINT32 crspdcfg1 : 22;
    /* crspdcfg1 - Bits[21:0], RW, default = 16'b1000001111111000 
         SPD config1
               Field Description: 
       
                Field             Bits    Description
               -------            ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
           
               spd_rcomp          21:15   view CComp
               spd_scomp_p        14:9   view SComp
               spd_scomp_n          8:3   view SComp
               spd_slowbuffer_ctl2    2   View Slow buffer control / Control 
       ddr_viewdig1  
               spd_slowbuffer_ctl1    1   View Slow buffer control / Control 
       ddr_viewdig0  
               spd_ddr_chdbg_sel      0   View Debug mux select in spd,  / 1'b1  - 
       select ch0   
               
     */
    UINT32 odtupsegen : 2;
    /* odtupsegen - Bits[23:22], RW, default = 2'b01 
       Odt up segment Enable
     */
    UINT32 odtdnsegen : 2;
    /* odtdnsegen - Bits[25:24], RW, default = 2'b01 
       Odt down segment Enable
     */
    UINT32 odtsegovrd : 1;
    /* odtsegovrd - Bits[26:26], RW, default = 1'b0 
       Dq segment Override
     */
    UINT32 rsvd : 5;
    /* rsvd - Bits[31:27], RW, default = 1'b0 
       Reserved for future usage
     */
  } Bits;
  UINT32 Data;
} DDRCRSPDCFG1_MCIO_DDRIOEXT_STRUCT;


/* DDRCRSPDCFG2_MCIO_DDRIOEXT_REG supported on:                               */
/*       SKX_A0 (0x402B1F3C)                                                  */
/*       SKX (0x402B1F3C)                                                     */
/* Register default value:              0x0A57C4F0                            */
#define DDRCRSPDCFG2_MCIO_DDRIOEXT_REG 0x08014F3C
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * SPD Comp Config
 */
typedef union {
  struct {
    UINT32 crspdcfg2 : 15;
    /* crspdcfg2 - Bits[14:0], RW, default = 15'b100010011110000 
         SPD config2
               Field Description: 
       
                Field             Bits    Description
               -------            ----    
       --------------------------------------------------------------------------------------------------------------------------------------------------------------- 
           
               spd_smbus1b           14   SPD/
               spd_hys_configp    13:12   SPD/Memhot/View
               spd_hys_confign_b  11:10   SPD/Memhot/View
               spd_rcompnctl        9:5   SPD/Memhot RComp
               spd_scompnctl        4:0   SPD/Memhot SComp
               
     */
    UINT32 dqupsegen : 2;
    /* dqupsegen - Bits[16:15], RW, default = 2'b11 
       Dq up segment Enable
     */
    UINT32 dqdnsegen : 2;
    /* dqdnsegen - Bits[18:17], RW, default = 2'b11 
       Dq down segment Enable
     */
    UINT32 dqsegovrd : 1;
    /* dqsegovrd - Bits[19:19], RW, default = 1'b0 
       Dq segment Override
     */
    UINT32 clkupsegen : 2;
    /* clkupsegen - Bits[21:20], RW, default = 2'b01 
       Clk up segment Enable
     */
    UINT32 clkdnsegen : 2;
    /* clkdnsegen - Bits[23:22], RW, default = 2'b01 
       Clk down segment Enable
     */
    UINT32 clksegovrd : 1;
    /* clksegovrd - Bits[24:24], RW, default = 1'b0 
       Clk segment Override
     */
    UINT32 ctlupsegen : 2;
    /* ctlupsegen - Bits[26:25], RW, default = 2'b01 
       Ctl up segment Enable
     */
    UINT32 ctldnsegen : 2;
    /* ctldnsegen - Bits[28:27], RW, default = 2'b01 
       Ctl down segment Enable
     */
    UINT32 ctlsegovrd : 1;
    /* ctlsegovrd - Bits[29:29], RW, default = 1'b0 
       Ctl segment Override
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], RW, default = 2'b0 
       Reserved for future usage
     */
  } Bits;
  UINT32 Data;
} DDRCRSPDCFG2_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCOMPCTL3_MCIO_DDRIOEXT_REG supported on:                              */
/*       SKX_A0 (0x402B1F40)                                                  */
/*       SKX (0x402B1F40)                                                     */
/* Register default value:              0x220202020                           */
#define DDRCRCOMPCTL3_MCIO_DDRIOEXT_REG 0x08014F40
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds Compensation Controls like cmd vref ctl, ctl vref ctl etc
 */
typedef union {
  struct {
    UINT32 cmddrvpupvref : 7;
    /* cmddrvpupvref - Bits[6:0], RW, default = 8'b00100000 
       2's compliment analog offset for CmdRcompDrvUp Vref with step size of Vccddq/96
     */
    UINT32 cmddrvpdnvref : 7;
    /* cmddrvpdnvref - Bits[13:7], RW, default = 8'b00100000 
       2's compliment analog offset for CmdRcompDrvDown Vref with step size of 
       Vccddq/96 
     */
    UINT32 ctldrvpupvref : 7;
    /* ctldrvpupvref - Bits[20:14], RW, default = 8'b00100000 
       2's compliment analog offset for CtlRcompDrvUp Vref with step size of Vccddq/96
     */
    UINT32 ctldrvpdnvref : 7;
    /* ctldrvpdnvref - Bits[27:21], RW, default = 8'b00100000 
       2's compliment analog offset for CtlRcompDrvDown Vref with step size of 
       Vccddq/96 
     */
    UINT32 swcapcmpclksel : 2;
    /* swcapcmpclksel - Bits[29:28], RW, default = 2'b10 
       Switch capacitor comparator clock select
     */
    UINT32 halflsbshiften : 1;
    /* halflsbshiften - Bits[30:30], RW, default = None 
       Half LSB Shift Enable
     */
    UINT32 dfxsel : 1;
    /* dfxsel - Bits[31:31], RW, default = 1'b0 
       dfxsel for cmd rcomp
     */
  } Bits;
  UINT32 Data;
} DDRCRCOMPCTL3_MCIO_DDRIOEXT_STRUCT;


/* DDRCRCOMPVSSHI1_MCIO_DDRIOEXT_REG supported on:                            */
/*       SKX_A0 (0x402B1F44)                                                  */
/*       SKX (0x402B1F44)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCOMPVSSHI1_MCIO_DDRIOEXT_REG 0x08014F44
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * This CR holds Compensation Controls for VssHi
 */
typedef union {
  struct {
    UINT32 spare0 : 18;
    /* spare0 - Bits[17:0], RW, default = None 
       Spare
     */
    UINT32 paniccompupmult : 7;
    /* paniccompupmult - Bits[24:18], RW, default = 7'b0 
       Multiplier to the Panic Comp Up code to expand to 10 bits
     */
    UINT32 paniccompdnmult : 7;
    /* paniccompdnmult - Bits[31:25], RW, default = 7'b0 
       Multiplier to the Panic Comp Dn code to expand to 10 bits
     */
  } Bits;
  UINT32 Data;
} DDRCRCOMPVSSHI1_MCIO_DDRIOEXT_STRUCT;


/* DDRCRFSMCALN0_6_MCIO_DDRIOEXT_REG supported on:                            */
/*       SKX (0x402B12C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCALN0_6_MCIO_DDRIOEXT_REG 0x080142C8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Fsm for  Voc and Xover caliberation, Voc caliberation is a procedure that cancel 
 * out intrinsic offset in analogue receivers and Xover training ,it aligns all the 
 * local dll to the global qclk   
 */
typedef union {
  struct {
    UINT32 saoffset0fsm : 5;
    /* saoffset0fsm - Bits[4:0], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of first receiver is done(read 
       only)   
     */
    UINT32 saoffset1fsm : 5;
    /* saoffset1fsm - Bits[9:5], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of second receiver is done(read 
       only)   
     */
    UINT32 saoffset2fsm : 5;
    /* saoffset2fsm - Bits[14:10], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of third receiver is done(read 
       only)   
     */
    UINT32 saoffset3fsm : 5;
    /* saoffset3fsm - Bits[19:15], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of fourth receiver is done(read 
       only)   
     */
    UINT32 fsmcalcomplete : 1;
    /* fsmcalcomplete - Bits[20:20], RO_V, default = 1'b0 
        when the caliberation is done
               
     */
    UINT32 refpiclkdelayfsm : 6;
    /* refpiclkdelayfsm - Bits[26:21], RO_V, default = 6'b0 
       Load FSM with a code at which , the transition is detected. 
               
     */
    UINT32 fsmcaldone : 1;
    /* fsmcaldone - Bits[27:27], RO_V, default = 1'b0 
       when the caliberation of voc or x-over is done
               
     */
    UINT32 fsmtimersel : 1;
    /* fsmtimersel - Bits[28:28], RW, default = 1'b0 
       Timer is set for either 8 cycles or 16 cycles    
               
     */
    UINT32 fsmpatternsel : 1;
    /* fsmpatternsel - Bits[29:29], RW, default = 1'b0 
        It selects whther the pattern of three bit or two bit is selected when the 
       transition happen from 1-->0 for pdout and receiver , to avoid glitch 
               
     */
    UINT32 fsmvoccal : 1;
    /* fsmvoccal - Bits[30:30], RW, default = 1'b0 
        This bit tells , wether the xover or voc caliberation will be done, if its zero 
       x-over caliberation is done and vice-versa 
               
     */
    UINT32 fsmcalen : 1;
    /* fsmcalen - Bits[31:31], RW, default = 1'b0 
       FsmCalEn starts the voc-Xover Fsm 
               
     */
  } Bits;
  UINT32 Data;
} DDRCRFSMCALN0_6_MCIO_DDRIOEXT_STRUCT;


/* DDRCRFSMCALN1_6_MCIO_DDRIOEXT_REG supported on:                            */
/*       SKX (0x402B13C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCALN1_6_MCIO_DDRIOEXT_REG 0x080143C8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Fsm for  Voc and Xover caliberation, Voc caliberation is a procedure that cancel 
 * out intrinsic offset in analogue receivers and Xover training ,it aligns all the 
 * local dll to the global qclk   
 */
typedef union {
  struct {
    UINT32 saoffset0fsm : 5;
    /* saoffset0fsm - Bits[4:0], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of first receiver is done(read 
       only)   
     */
    UINT32 saoffset1fsm : 5;
    /* saoffset1fsm - Bits[9:5], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of second receiver is done(read 
       only)   
     */
    UINT32 saoffset2fsm : 5;
    /* saoffset2fsm - Bits[14:10], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of third receiver is done(read 
       only)   
     */
    UINT32 saoffset3fsm : 5;
    /* saoffset3fsm - Bits[19:15], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of fourth receiver is done(read 
       only)   
     */
    UINT32 fsmcalcomplete : 1;
    /* fsmcalcomplete - Bits[20:20], RO_V, default = 1'b0 
        when the caliberation is done
               
     */
    UINT32 refpiclkdelayfsm : 6;
    /* refpiclkdelayfsm - Bits[26:21], RO_V, default = 6'b0 
       Load FSM with a code at which , the transition is detected. 
               
     */
    UINT32 fsmcaldone : 1;
    /* fsmcaldone - Bits[27:27], RO_V, default = 1'b0 
       when the caliberation of voc or x-over is done
               
     */
    UINT32 fsmtimersel : 1;
    /* fsmtimersel - Bits[28:28], RW, default = 1'b0 
       Timer is set for either 8 cycles or 16 cycles    
               
     */
    UINT32 fsmpatternsel : 1;
    /* fsmpatternsel - Bits[29:29], RW, default = 1'b0 
        It selects whther the pattern of three bit or two bit is selected when the 
       transition happen from 1-->0 for pdout and receiver , to avoid glitch 
               
     */
    UINT32 fsmvoccal : 1;
    /* fsmvoccal - Bits[30:30], RW, default = 1'b0 
        This bit tells , wether the xover or voc caliberation will be done, if its zero 
       x-over caliberation is done and vice-versa 
               
     */
    UINT32 fsmcalen : 1;
    /* fsmcalen - Bits[31:31], RW, default = 1'b0 
       FsmCalEn starts the voc-Xover Fsm 
               
     */
  } Bits;
  UINT32 Data;
} DDRCRFSMCALN1_6_MCIO_DDRIOEXT_STRUCT;


/* DDRCRFSMCALN0_7_MCIO_DDRIOEXT_REG supported on:                            */
/*       SKX (0x402B14C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCALN0_7_MCIO_DDRIOEXT_REG 0x080144C8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Fsm for  Voc and Xover caliberation, Voc caliberation is a procedure that cancel 
 * out intrinsic offset in analogue receivers and Xover training ,it aligns all the 
 * local dll to the global qclk   
 */
typedef union {
  struct {
    UINT32 saoffset0fsm : 5;
    /* saoffset0fsm - Bits[4:0], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of first receiver is done(read 
       only)   
     */
    UINT32 saoffset1fsm : 5;
    /* saoffset1fsm - Bits[9:5], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of second receiver is done(read 
       only)   
     */
    UINT32 saoffset2fsm : 5;
    /* saoffset2fsm - Bits[14:10], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of third receiver is done(read 
       only)   
     */
    UINT32 saoffset3fsm : 5;
    /* saoffset3fsm - Bits[19:15], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of fourth receiver is done(read 
       only)   
     */
    UINT32 fsmcalcomplete : 1;
    /* fsmcalcomplete - Bits[20:20], RO_V, default = 1'b0 
        when the caliberation is done
               
     */
    UINT32 refpiclkdelayfsm : 6;
    /* refpiclkdelayfsm - Bits[26:21], RO_V, default = 6'b0 
       Load FSM with a code at which , the transition is detected. 
               
     */
    UINT32 fsmcaldone : 1;
    /* fsmcaldone - Bits[27:27], RO_V, default = 1'b0 
       when the caliberation of voc or x-over is done
               
     */
    UINT32 fsmtimersel : 1;
    /* fsmtimersel - Bits[28:28], RW, default = 1'b0 
       Timer is set for either 8 cycles or 16 cycles    
               
     */
    UINT32 fsmpatternsel : 1;
    /* fsmpatternsel - Bits[29:29], RW, default = 1'b0 
        It selects whther the pattern of three bit or two bit is selected when the 
       transition happen from 1-->0 for pdout and receiver , to avoid glitch 
               
     */
    UINT32 fsmvoccal : 1;
    /* fsmvoccal - Bits[30:30], RW, default = 1'b0 
        This bit tells , wether the xover or voc caliberation will be done, if its zero 
       x-over caliberation is done and vice-versa 
               
     */
    UINT32 fsmcalen : 1;
    /* fsmcalen - Bits[31:31], RW, default = 1'b0 
       FsmCalEn starts the voc-Xover Fsm 
               
     */
  } Bits;
  UINT32 Data;
} DDRCRFSMCALN0_7_MCIO_DDRIOEXT_STRUCT;


/* DDRCRFSMCALN1_7_MCIO_DDRIOEXT_REG supported on:                            */
/*       SKX (0x402B15C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCALN1_7_MCIO_DDRIOEXT_REG 0x080145C8
/* Struct format extracted from XML file SKX\2.22.1.CFG.xml.
 * Fsm for  Voc and Xover caliberation, Voc caliberation is a procedure that cancel 
 * out intrinsic offset in analogue receivers and Xover training ,it aligns all the 
 * local dll to the global qclk   
 */
typedef union {
  struct {
    UINT32 saoffset0fsm : 5;
    /* saoffset0fsm - Bits[4:0], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of first receiver is done(read 
       only)   
     */
    UINT32 saoffset1fsm : 5;
    /* saoffset1fsm - Bits[9:5], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of second receiver is done(read 
       only)   
     */
    UINT32 saoffset2fsm : 5;
    /* saoffset2fsm - Bits[14:10], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of third receiver is done(read 
       only)   
     */
    UINT32 saoffset3fsm : 5;
    /* saoffset3fsm - Bits[19:15], RO_V, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of fourth receiver is done(read 
       only)   
     */
    UINT32 fsmcalcomplete : 1;
    /* fsmcalcomplete - Bits[20:20], RO_V, default = 1'b0 
        when the caliberation is done
               
     */
    UINT32 refpiclkdelayfsm : 6;
    /* refpiclkdelayfsm - Bits[26:21], RO_V, default = 6'b0 
       Load FSM with a code at which , the transition is detected. 
               
     */
    UINT32 fsmcaldone : 1;
    /* fsmcaldone - Bits[27:27], RO_V, default = 1'b0 
       when the caliberation of voc or x-over is done
               
     */
    UINT32 fsmtimersel : 1;
    /* fsmtimersel - Bits[28:28], RW, default = 1'b0 
       Timer is set for either 8 cycles or 16 cycles    
               
     */
    UINT32 fsmpatternsel : 1;
    /* fsmpatternsel - Bits[29:29], RW, default = 1'b0 
        It selects whther the pattern of three bit or two bit is selected when the 
       transition happen from 1-->0 for pdout and receiver , to avoid glitch 
               
     */
    UINT32 fsmvoccal : 1;
    /* fsmvoccal - Bits[30:30], RW, default = 1'b0 
        This bit tells , wether the xover or voc caliberation will be done, if its zero 
       x-over caliberation is done and vice-versa 
               
     */
    UINT32 fsmcalen : 1;
    /* fsmcalen - Bits[31:31], RW, default = 1'b0 
       FsmCalEn starts the voc-Xover Fsm 
               
     */
  } Bits;
  UINT32 Data;
} DDRCRFSMCALN1_7_MCIO_DDRIOEXT_STRUCT;


#endif /* MCIO_DDRIOEXT_h */
