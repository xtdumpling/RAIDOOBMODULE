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

#ifndef MC_DDRIOBC_h
#define MC_DDRIOBC_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* MC_DDRIOBC_DEV 22                                                          */
/* MC_DDRIOBC_FUN 7                                                           */

/* VID_MC_DDRIOBC_REG supported on:                                           */
/*       SKX_A0 (0x202B7000)                                                  */
/*       SKX (0x202B7000)                                                     */
/* Register default value:              0x8086                                */
#define VID_MC_DDRIOBC_REG 0x08032000
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} VID_MC_DDRIOBC_STRUCT;


/* DID_MC_DDRIOBC_REG supported on:                                           */
/*       SKX_A0 (0x202B7002)                                                  */
/*       SKX (0x202B7002)                                                     */
/* Register default value:              0x206F                                */
#define DID_MC_DDRIOBC_REG 0x08032002
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h206F 
        
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
} DID_MC_DDRIOBC_STRUCT;


/* PCICMD_MC_DDRIOBC_REG supported on:                                        */
/*       SKX_A0 (0x202B7004)                                                  */
/*       SKX (0x202B7004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_MC_DDRIOBC_REG 0x08032004
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} PCICMD_MC_DDRIOBC_STRUCT;


/* PCISTS_MC_DDRIOBC_REG supported on:                                        */
/*       SKX_A0 (0x202B7006)                                                  */
/*       SKX (0x202B7006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_MC_DDRIOBC_REG 0x08032006
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} PCISTS_MC_DDRIOBC_STRUCT;


/* RID_MC_DDRIOBC_REG supported on:                                           */
/*       SKX_A0 (0x102B7008)                                                  */
/*       SKX (0x102B7008)                                                     */
/* Register default value:              0x00                                  */
#define RID_MC_DDRIOBC_REG 0x08031008
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RID_MC_DDRIOBC_STRUCT;


/* CCR_N0_MC_DDRIOBC_REG supported on:                                        */
/*       SKX_A0 (0x102B7009)                                                  */
/*       SKX (0x102B7009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_MC_DDRIOBC_REG 0x08031009
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_MC_DDRIOBC_STRUCT;


/* CCR_N1_MC_DDRIOBC_REG supported on:                                        */
/*       SKX_A0 (0x202B700A)                                                  */
/*       SKX (0x202B700A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_MC_DDRIOBC_REG 0x0803200A
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} CCR_N1_MC_DDRIOBC_STRUCT;


/* CLSR_MC_DDRIOBC_REG supported on:                                          */
/*       SKX_A0 (0x102B700C)                                                  */
/*       SKX (0x102B700C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_MC_DDRIOBC_REG 0x0803100C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} CLSR_MC_DDRIOBC_STRUCT;


/* PLAT_MC_DDRIOBC_REG supported on:                                          */
/*       SKX_A0 (0x102B700D)                                                  */
/*       SKX (0x102B700D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_MC_DDRIOBC_REG 0x0803100D
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} PLAT_MC_DDRIOBC_STRUCT;


/* HDR_MC_DDRIOBC_REG supported on:                                           */
/*       SKX_A0 (0x102B700E)                                                  */
/*       SKX (0x102B700E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_MC_DDRIOBC_REG 0x0803100E
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} HDR_MC_DDRIOBC_STRUCT;


/* BIST_MC_DDRIOBC_REG supported on:                                          */
/*       SKX_A0 (0x102B700F)                                                  */
/*       SKX (0x102B700F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_MC_DDRIOBC_REG 0x0803100F
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} BIST_MC_DDRIOBC_STRUCT;


/* CAPPTR_MC_DDRIOBC_REG supported on:                                        */
/*       SKX_A0 (0x102B7034)                                                  */
/*       SKX (0x102B7034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_MC_DDRIOBC_REG 0x08031034
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} CAPPTR_MC_DDRIOBC_STRUCT;


/* INTL_MC_DDRIOBC_REG supported on:                                          */
/*       SKX_A0 (0x102B703C)                                                  */
/*       SKX (0x102B703C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_MC_DDRIOBC_REG 0x0803103C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} INTL_MC_DDRIOBC_STRUCT;


/* INTPIN_MC_DDRIOBC_REG supported on:                                        */
/*       SKX_A0 (0x102B703D)                                                  */
/*       SKX (0x102B703D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_MC_DDRIOBC_REG 0x0803103D
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} INTPIN_MC_DDRIOBC_STRUCT;


/* MINGNT_MC_DDRIOBC_REG supported on:                                        */
/*       SKX_A0 (0x102B703E)                                                  */
/*       SKX (0x102B703E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_MC_DDRIOBC_REG 0x0803103E
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} MINGNT_MC_DDRIOBC_STRUCT;


/* MAXLAT_MC_DDRIOBC_REG supported on:                                        */
/*       SKX_A0 (0x102B703F)                                                  */
/*       SKX (0x102B703F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_MC_DDRIOBC_REG 0x0803103F
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} MAXLAT_MC_DDRIOBC_STRUCT;


/* RXGROUP0N0RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7200)                                                  */
/*       SKX (0x402B7200)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK0_MC_DDRIOBC_REG 0x08034200
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N0RANK0_MC_DDRIOBC_STRUCT;


/* RXGROUP0N0RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7204)                                                  */
/*       SKX (0x402B7204)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK1_MC_DDRIOBC_REG 0x08034204
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N0RANK1_MC_DDRIOBC_STRUCT;


/* RXGROUP0N0RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7208)                                                  */
/*       SKX (0x402B7208)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK2_MC_DDRIOBC_REG 0x08034208
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N0RANK2_MC_DDRIOBC_STRUCT;


/* RXGROUP0N0RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B720C)                                                  */
/*       SKX (0x402B720C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK3_MC_DDRIOBC_REG 0x0803420C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N0RANK3_MC_DDRIOBC_STRUCT;


/* RXGROUP0N0RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7210)                                                  */
/*       SKX (0x402B7210)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK4_MC_DDRIOBC_REG 0x08034210
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N0RANK4_MC_DDRIOBC_STRUCT;


/* RXGROUP0N0RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7214)                                                  */
/*       SKX (0x402B7214)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK5_MC_DDRIOBC_REG 0x08034214
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N0RANK5_MC_DDRIOBC_STRUCT;


/* RXGROUP0N0RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7218)                                                  */
/*       SKX (0x402B7218)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK6_MC_DDRIOBC_REG 0x08034218
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N0RANK6_MC_DDRIOBC_STRUCT;


/* RXGROUP0N0RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B721C)                                                  */
/*       SKX (0x402B721C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK7_MC_DDRIOBC_REG 0x0803421C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N0RANK7_MC_DDRIOBC_STRUCT;


/* RXGROUP1N0RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7220)                                                  */
/*       SKX (0x402B7220)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK0_MC_DDRIOBC_REG 0x08034220
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N0RANK0_MC_DDRIOBC_STRUCT;


/* RXGROUP1N0RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7224)                                                  */
/*       SKX (0x402B7224)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK1_MC_DDRIOBC_REG 0x08034224
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N0RANK1_MC_DDRIOBC_STRUCT;


/* RXGROUP1N0RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7228)                                                  */
/*       SKX (0x402B7228)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK2_MC_DDRIOBC_REG 0x08034228
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N0RANK2_MC_DDRIOBC_STRUCT;


/* RXGROUP1N0RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B722C)                                                  */
/*       SKX (0x402B722C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK3_MC_DDRIOBC_REG 0x0803422C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N0RANK3_MC_DDRIOBC_STRUCT;


/* RXGROUP1N0RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7230)                                                  */
/*       SKX (0x402B7230)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK4_MC_DDRIOBC_REG 0x08034230
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N0RANK4_MC_DDRIOBC_STRUCT;


/* RXGROUP1N0RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7234)                                                  */
/*       SKX (0x402B7234)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK5_MC_DDRIOBC_REG 0x08034234
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N0RANK5_MC_DDRIOBC_STRUCT;


/* RXGROUP1N0RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7238)                                                  */
/*       SKX (0x402B7238)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK6_MC_DDRIOBC_REG 0x08034238
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N0RANK6_MC_DDRIOBC_STRUCT;


/* RXGROUP1N0RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B723C)                                                  */
/*       SKX (0x402B723C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK7_MC_DDRIOBC_REG 0x0803423C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N0RANK7_MC_DDRIOBC_STRUCT;


/* TXGROUP0N0RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7240)                                                  */
/*       SKX (0x402B7240)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK0_MC_DDRIOBC_REG 0x08034240
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N0RANK0_MC_DDRIOBC_STRUCT;


/* TXGROUP0N0RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7244)                                                  */
/*       SKX (0x402B7244)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK1_MC_DDRIOBC_REG 0x08034244
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N0RANK1_MC_DDRIOBC_STRUCT;


/* TXGROUP0N0RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7248)                                                  */
/*       SKX (0x402B7248)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK2_MC_DDRIOBC_REG 0x08034248
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N0RANK2_MC_DDRIOBC_STRUCT;


/* TXGROUP0N0RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B724C)                                                  */
/*       SKX (0x402B724C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK3_MC_DDRIOBC_REG 0x0803424C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N0RANK3_MC_DDRIOBC_STRUCT;


/* TXGROUP0N0RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7250)                                                  */
/*       SKX (0x402B7250)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK4_MC_DDRIOBC_REG 0x08034250
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N0RANK4_MC_DDRIOBC_STRUCT;


/* TXGROUP0N0RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7254)                                                  */
/*       SKX (0x402B7254)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK5_MC_DDRIOBC_REG 0x08034254
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N0RANK5_MC_DDRIOBC_STRUCT;


/* TXGROUP0N0RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7258)                                                  */
/*       SKX (0x402B7258)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK6_MC_DDRIOBC_REG 0x08034258
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N0RANK6_MC_DDRIOBC_STRUCT;


/* TXGROUP0N0RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B725C)                                                  */
/*       SKX (0x402B725C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK7_MC_DDRIOBC_REG 0x0803425C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N0RANK7_MC_DDRIOBC_STRUCT;


/* TXGROUP1N0RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7260)                                                  */
/*       SKX (0x402B7260)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK0_MC_DDRIOBC_REG 0x08034260
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N0RANK0_MC_DDRIOBC_STRUCT;


/* TXGROUP1N0RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7264)                                                  */
/*       SKX (0x402B7264)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK1_MC_DDRIOBC_REG 0x08034264
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N0RANK1_MC_DDRIOBC_STRUCT;


/* TXGROUP1N0RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7268)                                                  */
/*       SKX (0x402B7268)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK2_MC_DDRIOBC_REG 0x08034268
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N0RANK2_MC_DDRIOBC_STRUCT;


/* TXGROUP1N0RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B726C)                                                  */
/*       SKX (0x402B726C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK3_MC_DDRIOBC_REG 0x0803426C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N0RANK3_MC_DDRIOBC_STRUCT;


/* TXGROUP1N0RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7270)                                                  */
/*       SKX (0x402B7270)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK4_MC_DDRIOBC_REG 0x08034270
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N0RANK4_MC_DDRIOBC_STRUCT;


/* TXGROUP1N0RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7274)                                                  */
/*       SKX (0x402B7274)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK5_MC_DDRIOBC_REG 0x08034274
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N0RANK5_MC_DDRIOBC_STRUCT;


/* TXGROUP1N0RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7278)                                                  */
/*       SKX (0x402B7278)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK6_MC_DDRIOBC_REG 0x08034278
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N0RANK6_MC_DDRIOBC_STRUCT;


/* TXGROUP1N0RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B727C)                                                  */
/*       SKX (0x402B727C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK7_MC_DDRIOBC_REG 0x0803427C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N0RANK7_MC_DDRIOBC_STRUCT;


/* RXOFFSETN0RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7280)                                                  */
/*       SKX (0x402B7280)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK0_MC_DDRIOBC_REG 0x08034280
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN0RANK0_MC_DDRIOBC_STRUCT;


/* RXOFFSETN0RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7284)                                                  */
/*       SKX (0x402B7284)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK1_MC_DDRIOBC_REG 0x08034284
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN0RANK1_MC_DDRIOBC_STRUCT;


/* RXOFFSETN0RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7288)                                                  */
/*       SKX (0x402B7288)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK2_MC_DDRIOBC_REG 0x08034288
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN0RANK2_MC_DDRIOBC_STRUCT;


/* RXOFFSETN0RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B728C)                                                  */
/*       SKX (0x402B728C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK3_MC_DDRIOBC_REG 0x0803428C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN0RANK3_MC_DDRIOBC_STRUCT;


/* RXOFFSETN0RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7290)                                                  */
/*       SKX (0x402B7290)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK4_MC_DDRIOBC_REG 0x08034290
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN0RANK4_MC_DDRIOBC_STRUCT;


/* RXOFFSETN0RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7294)                                                  */
/*       SKX (0x402B7294)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK5_MC_DDRIOBC_REG 0x08034294
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN0RANK5_MC_DDRIOBC_STRUCT;


/* RXOFFSETN0RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7298)                                                  */
/*       SKX (0x402B7298)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK6_MC_DDRIOBC_REG 0x08034298
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN0RANK6_MC_DDRIOBC_STRUCT;


/* RXOFFSETN0RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B729C)                                                  */
/*       SKX (0x402B729C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK7_MC_DDRIOBC_REG 0x0803429C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN0RANK7_MC_DDRIOBC_STRUCT;


/* TXXTALKN0_MC_DDRIOBC_REG supported on:                                     */
/*       SKX_A0 (0x402B72A0)                                                  */
/*       SKX (0x402B72A0)                                                     */
/* Register default value:              0x00000000                            */
#define TXXTALKN0_MC_DDRIOBC_REG 0x080342A0
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXXTALKN0_MC_DDRIOBC_STRUCT;


/* RXVREFCTRLN0_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B72A4)                                                  */
/*       SKX (0x402B72A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN0_MC_DDRIOBC_REG 0x080342A4
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXVREFCTRLN0_MC_DDRIOBC_STRUCT;


/* DATATRAINFEEDBACKN0_MC_DDRIOBC_REG supported on:                           */
/*       SKX_A0 (0x402B72A8)                                                  */
/*       SKX (0x402B72A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN0_MC_DDRIOBC_REG 0x080342A8
/* Struct format extracted from XML file SKX_A0\2.22.7.CFG.xml.
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
} DATATRAINFEEDBACKN0_MC_DDRIOBC_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DATATRAINFEEDBACKN0_MC_DDRIOBC_STRUCT;



/* DATACONTROL4_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B72CC)                                                  */
/*       SKX (0x402B72CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4_MC_DDRIOBC_REG 0x080342CC
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DATACONTROL4_MC_DDRIOBC_STRUCT;


/* ATTACK1SEL_MC_DDRIOBC_REG supported on:                                    */
/*       SKX_A0 (0x402B72D0)                                                  */
/*       SKX (0x402B72D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SEL_MC_DDRIOBC_REG 0x080342D0
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} ATTACK1SEL_MC_DDRIOBC_STRUCT;






/* DATAOFFSETTRAIN_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B72DC)                                                  */
/*       SKX (0x402B72DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAIN_MC_DDRIOBC_REG 0x080342DC
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DATAOFFSETTRAIN_MC_DDRIOBC_STRUCT;




/* ATTACK0SEL_MC_DDRIOBC_REG supported on:                                    */
/*       SKX_A0 (0x402B72E4)                                                  */
/*       SKX (0x402B72E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SEL_MC_DDRIOBC_REG 0x080342E4
/* Struct format extracted from XML file SKX_A0\2.22.7.CFG.xml.
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
} ATTACK0SEL_MC_DDRIOBC_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} ATTACK0SEL_MC_DDRIOBC_STRUCT;



/* DATACONTROL0_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B72E8)                                                  */
/*       SKX (0x402B72E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0_MC_DDRIOBC_REG 0x080342E8
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DATACONTROL0_MC_DDRIOBC_STRUCT;


/* DATACONTROL1_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B72EC)                                                  */
/*       SKX (0x402B72EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1_MC_DDRIOBC_REG 0x080342EC
/* Struct format extracted from XML file SKX_A0\2.22.7.CFG.xml.
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
} DATACONTROL1_MC_DDRIOBC_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DATACONTROL1_MC_DDRIOBC_STRUCT;



/* DATACONTROL2_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B72F0)                                                  */
/*       SKX (0x402B72F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2_MC_DDRIOBC_REG 0x080342F0
/* Struct format extracted from XML file SKX_A0\2.22.7.CFG.xml.
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
} DATACONTROL2_MC_DDRIOBC_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DATACONTROL2_MC_DDRIOBC_STRUCT;



/* DATACONTROL3_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B72F4)                                                  */
/*       SKX (0x402B72F4)                                                     */
/* Register default value on SKX_A0:    0x8184891FCF                          */
/* Register default value on SKX:       0x00000FCF                            */
#define DATACONTROL3_MC_DDRIOBC_REG 0x080342F4
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DATACONTROL3_MC_DDRIOBC_STRUCT;


/* VSSHIORVREFCONTROL_MC_DDRIOBC_REG supported on:                            */
/*       SKX_A0 (0x402B72F8)                                                  */
/*       SKX (0x402B72F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_MC_DDRIOBC_REG 0x080342F8
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} VSSHIORVREFCONTROL_MC_DDRIOBC_STRUCT;




/* RXGROUP0N1RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7300)                                                  */
/*       SKX (0x402B7300)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK0_MC_DDRIOBC_REG 0x08034300
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N1RANK0_MC_DDRIOBC_STRUCT;


/* RXGROUP0N1RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7304)                                                  */
/*       SKX (0x402B7304)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK1_MC_DDRIOBC_REG 0x08034304
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N1RANK1_MC_DDRIOBC_STRUCT;


/* RXGROUP0N1RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7308)                                                  */
/*       SKX (0x402B7308)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK2_MC_DDRIOBC_REG 0x08034308
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N1RANK2_MC_DDRIOBC_STRUCT;


/* RXGROUP0N1RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B730C)                                                  */
/*       SKX (0x402B730C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK3_MC_DDRIOBC_REG 0x0803430C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N1RANK3_MC_DDRIOBC_STRUCT;


/* RXGROUP0N1RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7310)                                                  */
/*       SKX (0x402B7310)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK4_MC_DDRIOBC_REG 0x08034310
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N1RANK4_MC_DDRIOBC_STRUCT;


/* RXGROUP0N1RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7314)                                                  */
/*       SKX (0x402B7314)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK5_MC_DDRIOBC_REG 0x08034314
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N1RANK5_MC_DDRIOBC_STRUCT;


/* RXGROUP0N1RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7318)                                                  */
/*       SKX (0x402B7318)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK6_MC_DDRIOBC_REG 0x08034318
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N1RANK6_MC_DDRIOBC_STRUCT;


/* RXGROUP0N1RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B731C)                                                  */
/*       SKX (0x402B731C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK7_MC_DDRIOBC_REG 0x0803431C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP0N1RANK7_MC_DDRIOBC_STRUCT;


/* RXGROUP1N1RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7320)                                                  */
/*       SKX (0x402B7320)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK0_MC_DDRIOBC_REG 0x08034320
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N1RANK0_MC_DDRIOBC_STRUCT;


/* RXGROUP1N1RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7324)                                                  */
/*       SKX (0x402B7324)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK1_MC_DDRIOBC_REG 0x08034324
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N1RANK1_MC_DDRIOBC_STRUCT;


/* RXGROUP1N1RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7328)                                                  */
/*       SKX (0x402B7328)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK2_MC_DDRIOBC_REG 0x08034328
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N1RANK2_MC_DDRIOBC_STRUCT;


/* RXGROUP1N1RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B732C)                                                  */
/*       SKX (0x402B732C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK3_MC_DDRIOBC_REG 0x0803432C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N1RANK3_MC_DDRIOBC_STRUCT;


/* RXGROUP1N1RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7330)                                                  */
/*       SKX (0x402B7330)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK4_MC_DDRIOBC_REG 0x08034330
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N1RANK4_MC_DDRIOBC_STRUCT;


/* RXGROUP1N1RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7334)                                                  */
/*       SKX (0x402B7334)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK5_MC_DDRIOBC_REG 0x08034334
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N1RANK5_MC_DDRIOBC_STRUCT;


/* RXGROUP1N1RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7338)                                                  */
/*       SKX (0x402B7338)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK6_MC_DDRIOBC_REG 0x08034338
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N1RANK6_MC_DDRIOBC_STRUCT;


/* RXGROUP1N1RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B733C)                                                  */
/*       SKX (0x402B733C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK7_MC_DDRIOBC_REG 0x0803433C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXGROUP1N1RANK7_MC_DDRIOBC_STRUCT;


/* TXGROUP0N1RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7340)                                                  */
/*       SKX (0x402B7340)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK0_MC_DDRIOBC_REG 0x08034340
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N1RANK0_MC_DDRIOBC_STRUCT;


/* TXGROUP0N1RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7344)                                                  */
/*       SKX (0x402B7344)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK1_MC_DDRIOBC_REG 0x08034344
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N1RANK1_MC_DDRIOBC_STRUCT;


/* TXGROUP0N1RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7348)                                                  */
/*       SKX (0x402B7348)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK2_MC_DDRIOBC_REG 0x08034348
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N1RANK2_MC_DDRIOBC_STRUCT;


/* TXGROUP0N1RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B734C)                                                  */
/*       SKX (0x402B734C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK3_MC_DDRIOBC_REG 0x0803434C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N1RANK3_MC_DDRIOBC_STRUCT;


/* TXGROUP0N1RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7350)                                                  */
/*       SKX (0x402B7350)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK4_MC_DDRIOBC_REG 0x08034350
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N1RANK4_MC_DDRIOBC_STRUCT;


/* TXGROUP0N1RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7354)                                                  */
/*       SKX (0x402B7354)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK5_MC_DDRIOBC_REG 0x08034354
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N1RANK5_MC_DDRIOBC_STRUCT;


/* TXGROUP0N1RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7358)                                                  */
/*       SKX (0x402B7358)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK6_MC_DDRIOBC_REG 0x08034358
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N1RANK6_MC_DDRIOBC_STRUCT;


/* TXGROUP0N1RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B735C)                                                  */
/*       SKX (0x402B735C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK7_MC_DDRIOBC_REG 0x0803435C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP0N1RANK7_MC_DDRIOBC_STRUCT;


/* TXGROUP1N1RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7360)                                                  */
/*       SKX (0x402B7360)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK0_MC_DDRIOBC_REG 0x08034360
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N1RANK0_MC_DDRIOBC_STRUCT;


/* TXGROUP1N1RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7364)                                                  */
/*       SKX (0x402B7364)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK1_MC_DDRIOBC_REG 0x08034364
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N1RANK1_MC_DDRIOBC_STRUCT;


/* TXGROUP1N1RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7368)                                                  */
/*       SKX (0x402B7368)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK2_MC_DDRIOBC_REG 0x08034368
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N1RANK2_MC_DDRIOBC_STRUCT;


/* TXGROUP1N1RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B736C)                                                  */
/*       SKX (0x402B736C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK3_MC_DDRIOBC_REG 0x0803436C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N1RANK3_MC_DDRIOBC_STRUCT;


/* TXGROUP1N1RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7370)                                                  */
/*       SKX (0x402B7370)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK4_MC_DDRIOBC_REG 0x08034370
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N1RANK4_MC_DDRIOBC_STRUCT;


/* TXGROUP1N1RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7374)                                                  */
/*       SKX (0x402B7374)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK5_MC_DDRIOBC_REG 0x08034374
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N1RANK5_MC_DDRIOBC_STRUCT;


/* TXGROUP1N1RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7378)                                                  */
/*       SKX (0x402B7378)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK6_MC_DDRIOBC_REG 0x08034378
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N1RANK6_MC_DDRIOBC_STRUCT;


/* TXGROUP1N1RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B737C)                                                  */
/*       SKX (0x402B737C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK7_MC_DDRIOBC_REG 0x0803437C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} TXGROUP1N1RANK7_MC_DDRIOBC_STRUCT;


/* RXOFFSETN1RANK0_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7380)                                                  */
/*       SKX (0x402B7380)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK0_MC_DDRIOBC_REG 0x08034380
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN1RANK0_MC_DDRIOBC_STRUCT;


/* RXOFFSETN1RANK1_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7384)                                                  */
/*       SKX (0x402B7384)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK1_MC_DDRIOBC_REG 0x08034384
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN1RANK1_MC_DDRIOBC_STRUCT;


/* RXOFFSETN1RANK2_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7388)                                                  */
/*       SKX (0x402B7388)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK2_MC_DDRIOBC_REG 0x08034388
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN1RANK2_MC_DDRIOBC_STRUCT;


/* RXOFFSETN1RANK3_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B738C)                                                  */
/*       SKX (0x402B738C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK3_MC_DDRIOBC_REG 0x0803438C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN1RANK3_MC_DDRIOBC_STRUCT;


/* RXOFFSETN1RANK4_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7390)                                                  */
/*       SKX (0x402B7390)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK4_MC_DDRIOBC_REG 0x08034390
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN1RANK4_MC_DDRIOBC_STRUCT;


/* RXOFFSETN1RANK5_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7394)                                                  */
/*       SKX (0x402B7394)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK5_MC_DDRIOBC_REG 0x08034394
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN1RANK5_MC_DDRIOBC_STRUCT;


/* RXOFFSETN1RANK6_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B7398)                                                  */
/*       SKX (0x402B7398)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK6_MC_DDRIOBC_REG 0x08034398
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN1RANK6_MC_DDRIOBC_STRUCT;


/* RXOFFSETN1RANK7_MC_DDRIOBC_REG supported on:                               */
/*       SKX_A0 (0x402B739C)                                                  */
/*       SKX (0x402B739C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK7_MC_DDRIOBC_REG 0x0803439C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXOFFSETN1RANK7_MC_DDRIOBC_STRUCT;




/* RXVREFCTRLN1_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B73A4)                                                  */
/*       SKX (0x402B73A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN1_MC_DDRIOBC_REG 0x080343A4
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} RXVREFCTRLN1_MC_DDRIOBC_STRUCT;


/* DATATRAINFEEDBACKN1_MC_DDRIOBC_REG supported on:                           */
/*       SKX_A0 (0x402B73A8)                                                  */
/*       SKX (0x402B73A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN1_MC_DDRIOBC_REG 0x080343A8
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DATATRAINFEEDBACKN1_MC_DDRIOBC_STRUCT;




/* DDRCRCMDCOMPOFFSET_CMD_MC_DDRIOBC_REG supported on:                        */
/*       SKX_A0 (0x402B7604)                                                  */
/*       SKX (0x402B7604)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_CMD_MC_DDRIOBC_REG 0x08034604
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDCOMPOFFSET_CMD_MC_DDRIOBC_STRUCT;


/* DDRCRCMDPICODING_CMD_MC_DDRIOBC_REG supported on:                          */
/*       SKX_A0 (0x402B7608)                                                  */
/*       SKX (0x402B7608)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_CMD_MC_DDRIOBC_REG 0x08034608
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDPICODING_CMD_MC_DDRIOBC_STRUCT;


/* DDRCRCMDCONTROLS_CMD_MC_DDRIOBC_REG supported on:                          */
/*       SKX_A0 (0x402B760C)                                                  */
/*       SKX (0x402B760C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_CMD_MC_DDRIOBC_REG 0x0803460C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDCONTROLS_CMD_MC_DDRIOBC_STRUCT;


/* DDRCRCMDCONTROLS2_CMD_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B7610)                                                  */
/*       SKX (0x402B7610)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_CMD_MC_DDRIOBC_REG 0x08034610
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDCONTROLS2_CMD_MC_DDRIOBC_STRUCT;




/* DDRCRCTLCOMPOFFSET_CMD_MC_DDRIOBC_REG supported on:                        */
/*       SKX_A0 (0x402B7618)                                                  */
/*       SKX (0x402B7618)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_CMD_MC_DDRIOBC_REG 0x08034618
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCTLCOMPOFFSET_CMD_MC_DDRIOBC_STRUCT;


/* DDRCRCMDCONTROLS3_CMD_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B761C)                                                  */
/*       SKX (0x402B761C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_CMD_MC_DDRIOBC_REG 0x0803461C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDCONTROLS3_CMD_MC_DDRIOBC_STRUCT;


/* DDRCRCTLRANKSUSED_CMD_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B7620)                                                  */
/*       SKX (0x402B7620)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_CMD_MC_DDRIOBC_REG 0x08034620
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCTLRANKSUSED_CMD_MC_DDRIOBC_STRUCT;






/* DDRCRCMDTRAINING_CMD_MC_DDRIOBC_REG supported on:                          */
/*       SKX_A0 (0x402B762C)                                                  */
/*       SKX (0x402B762C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_CMD_MC_DDRIOBC_REG 0x0803462C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDTRAINING_CMD_MC_DDRIOBC_STRUCT;


/* DDRCRCMDCONTROLS1_CMD_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B7630)                                                  */
/*       SKX (0x402B7630)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_CMD_MC_DDRIOBC_REG 0x08034630
/* Struct format extracted from XML file SKX_A0\2.22.7.CFG.xml.
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
} DDRCRCMDCONTROLS1_CMD_MC_DDRIOBC_A0_STRUCT;

/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDCONTROLS1_CMD_MC_DDRIOBC_STRUCT;



/* DDRCRCMDPICODING2_CMD_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B7634)                                                  */
/*       SKX (0x402B7634)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_CMD_MC_DDRIOBC_REG 0x08034634
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDPICODING2_CMD_MC_DDRIOBC_STRUCT;


/* DDRCRCMDPICODING3_CMD_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B7638)                                                  */
/*       SKX (0x402B7638)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_CMD_MC_DDRIOBC_REG 0x08034638
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCMDPICODING3_CMD_MC_DDRIOBC_STRUCT;




/* VSSHIORVREFCONTROL_CMD_MC_DDRIOBC_REG supported on:                        */
/*       SKX_A0 (0x402B7640)                                                  */
/*       SKX (0x402B7640)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_CMD_MC_DDRIOBC_REG 0x08034640
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} VSSHIORVREFCONTROL_CMD_MC_DDRIOBC_STRUCT;




/* VSSHIORVREFCONTROL1_CMD_MC_DDRIOBC_REG supported on:                       */
/*       SKX_A0 (0x402B7648)                                                  */
/*       SKX (0x402B7648)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_CMD_MC_DDRIOBC_REG 0x08034648
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} VSSHIORVREFCONTROL1_CMD_MC_DDRIOBC_STRUCT;


/* DDRCRCLKRANKSUSED_MC_DDRIOBC_REG supported on:                             */
/*       SKX_A0 (0x402B7A00)                                                  */
/*       SKX (0x402B7A00)                                                     */
/* Register default value:              0x0000E00F                            */
#define DDRCRCLKRANKSUSED_MC_DDRIOBC_REG 0x08034A00
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
 * CLK Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 4;
    /* ranken - Bits[3:0], RW, default = 4'b1111 
       Enables output buffers, PI clocks and output datapaths for this rank
     */
    UINT32 rsvd_4 : 4;
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
} DDRCRCLKRANKSUSED_MC_DDRIOBC_STRUCT;




/* DDRCRCLKCOMPOFFSET_CLK_MC_DDRIOBC_REG supported on:                        */
/*       SKX_A0 (0x402B7A08)                                                  */
/*       SKX (0x402B7A08)                                                     */
/* Register default value:              0x82010000                            */
#define DDRCRCLKCOMPOFFSET_CLK_MC_DDRIOBC_REG 0x08034A08
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCLKCOMPOFFSET_CLK_MC_DDRIOBC_STRUCT;


/* DDRCRCLKPICODE_MC_DDRIOBC_REG supported on:                                */
/*       SKX_A0 (0x402B7A0C)                                                  */
/*       SKX (0x402B7A0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKPICODE_MC_DDRIOBC_REG 0x08034A0C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCLKPICODE_MC_DDRIOBC_STRUCT;


/* DDRCRCLKCONTROLS_MC_DDRIOBC_REG supported on:                              */
/*       SKX_A0 (0x402B7A10)                                                  */
/*       SKX (0x402B7A10)                                                     */
/* Register default value:              0x00066000                            */
#define DDRCRCLKCONTROLS_MC_DDRIOBC_REG 0x08034A10
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCLKCONTROLS_MC_DDRIOBC_STRUCT;




/* DDRCRCLKTRAINING_MC_DDRIOBC_REG supported on:                              */
/*       SKX_A0 (0x402B7A1C)                                                  */
/*       SKX (0x402B7A1C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKTRAINING_MC_DDRIOBC_REG 0x08034A1C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} DDRCRCLKTRAINING_MC_DDRIOBC_STRUCT;


/* DDRCRCLKACIOLB_MC_DDRIOBC_REG supported on:                                */
/*       SKX_A0 (0x402B7A20)                                                  */
/*       SKX (0x402B7A20)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKACIOLB_MC_DDRIOBC_REG 0x08034A20
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCLKACIOLB_MC_DDRIOBC_STRUCT;


/* DDRCRDDRTTRAINRESULT_MC_DDRIOBC_REG supported on:                          */
/*       SKX_A0 (0x402B7A24)                                                  */
/*       SKX (0x402B7A24)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRDDRTTRAINRESULT_MC_DDRIOBC_REG 0x08034A24
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRDDRTTRAINRESULT_MC_DDRIOBC_STRUCT;








/* DDRCRCTLCOMP_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B7E10)                                                  */
/*       SKX (0x402B7E10)                                                     */
/* Register default value:              0x34000000                            */
#define DDRCRCTLCOMP_MC_DDRIOBC_REG 0x08034E10
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCTLCOMP_MC_DDRIOBC_STRUCT;




/* DDRCRCOMPCTL0_MC_DDRIOBC_REG supported on:                                 */
/*       SKX_A0 (0x402B7E18)                                                  */
/*       SKX (0x402B7E18)                                                     */
/* Register default value:              0x40800007                            */
#define DDRCRCOMPCTL0_MC_DDRIOBC_REG 0x08034E18
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
 * This CR holds Compensation Controls like Vref Offset etc
 */
typedef union {
  struct {
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[0:0], RW, default = 1'b1  */
    UINT32 rsvd_1 : 2;
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
} DDRCRCOMPCTL0_MC_DDRIOBC_STRUCT;


/* DDRCRCOMPCTL1_MC_DDRIOBC_REG supported on:                                 */
/*       SKX_A0 (0x402B7E1C)                                                  */
/*       SKX (0x402B7E1C)                                                     */
/* Register default value:              0x08000C79                            */
#define DDRCRCOMPCTL1_MC_DDRIOBC_REG 0x08034E1C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCOMPCTL1_MC_DDRIOBC_STRUCT;


/* DDRCRCOMPCTL2_MC_DDRIOBC_REG supported on:                                 */
/*       SKX_A0 (0x402B7E20)                                                  */
/*       SKX (0x402B7E20)                                                     */
/* Register default value:              0x40808083F                           */
#define DDRCRCOMPCTL2_MC_DDRIOBC_REG 0x08034E20
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCOMPCTL2_MC_DDRIOBC_STRUCT;


/* DDRCRCOMPVSSHI_MC_DDRIOBC_REG supported on:                                */
/*       SKX_A0 (0x402B7E24)                                                  */
/*       SKX (0x402B7E24)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCOMPVSSHI_MC_DDRIOBC_REG 0x08034E24
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCOMPVSSHI_MC_DDRIOBC_STRUCT;


/* DDRCRCOMPOVR_MC_DDRIOBC_REG supported on:                                  */
/*       SKX_A0 (0x402B7E28)                                                  */
/*       SKX (0x402B7E28)                                                     */
/* Register default value:              0x010F0000                            */
#define DDRCRCOMPOVR_MC_DDRIOBC_REG 0x08034E28
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
    UINT32 rsvd_21 : 4;
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
} DDRCRCOMPOVR_MC_DDRIOBC_STRUCT;


/* DDRCRCOMPVSSHICONTROL_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B7E2C)                                                  */
/*       SKX (0x402B7E2C)                                                     */
/* Register default value:              0x004D8238                            */
#define DDRCRCOMPVSSHICONTROL_MC_DDRIOBC_REG 0x08034E2C
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRCOMPVSSHICONTROL_MC_DDRIOBC_STRUCT;


/* DDRCRDIMMVREFCONTROL1_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B7E30)                                                  */
/*       SKX (0x402B7E30)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRDIMMVREFCONTROL1_MC_DDRIOBC_REG 0x08034E30
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRDIMMVREFCONTROL1_MC_DDRIOBC_STRUCT;


/* DDRCRDIMMVREFCONTROL2_MC_DDRIOBC_REG supported on:                         */
/*       SKX_A0 (0x402B7E34)                                                  */
/*       SKX (0x402B7E34)                                                     */
/* Register default value:              0x000E4500                            */
#define DDRCRDIMMVREFCONTROL2_MC_DDRIOBC_REG 0x08034E34
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRDIMMVREFCONTROL2_MC_DDRIOBC_STRUCT;










/* DDRCRFSMCAL_MC_DDRIOBC_REG supported on:                                   */
/*       SKX (0x402B72C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCAL_MC_DDRIOBC_REG 0x080342C8
/* Struct format extracted from XML file SKX\2.22.7.CFG.xml.
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
} DDRCRFSMCAL_MC_DDRIOBC_STRUCT;


#endif /* MC_DDRIOBC_h */
