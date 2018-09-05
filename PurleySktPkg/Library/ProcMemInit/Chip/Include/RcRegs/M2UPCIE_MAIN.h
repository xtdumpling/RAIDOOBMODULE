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

#ifndef M2UPCIE_MAIN_h
#define M2UPCIE_MAIN_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* M2UPCIE_MAIN_DEV 21                                                        */
/* M2UPCIE_MAIN_FUN 0                                                         */

/* VID_M2UPCIE_MAIN_REG supported on:                                         */
/*       SKX_A0 (0x203A8000)                                                  */
/*       SKX (0x203A8000)                                                     */
/* Register default value:              0x8086                                */
#define VID_M2UPCIE_MAIN_REG 0x0C002000
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} VID_M2UPCIE_MAIN_STRUCT;


/* DID_M2UPCIE_MAIN_REG supported on:                                         */
/*       SKX_A0 (0x203A8002)                                                  */
/*       SKX (0x203A8002)                                                     */
/* Register default value:              0x2018                                */
#define DID_M2UPCIE_MAIN_REG 0x0C002002
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2018 
        
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
} DID_M2UPCIE_MAIN_STRUCT;


/* PCICMD_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x203A8004)                                                  */
/*       SKX (0x203A8004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_M2UPCIE_MAIN_REG 0x0C002004
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} PCICMD_M2UPCIE_MAIN_STRUCT;


/* PCISTS_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x203A8006)                                                  */
/*       SKX (0x203A8006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_M2UPCIE_MAIN_REG 0x0C002006
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} PCISTS_M2UPCIE_MAIN_STRUCT;


/* RID_M2UPCIE_MAIN_REG supported on:                                         */
/*       SKX_A0 (0x103A8008)                                                  */
/*       SKX (0x103A8008)                                                     */
/* Register default value:              0x00                                  */
#define RID_M2UPCIE_MAIN_REG 0x0C001008
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} RID_M2UPCIE_MAIN_STRUCT;


/* CCR_N0_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x103A8009)                                                  */
/*       SKX (0x103A8009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_M2UPCIE_MAIN_REG 0x0C001009
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_M2UPCIE_MAIN_STRUCT;


/* CCR_N1_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x203A800A)                                                  */
/*       SKX (0x203A800A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_M2UPCIE_MAIN_REG 0x0C00200A
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} CCR_N1_M2UPCIE_MAIN_STRUCT;


/* CLSR_M2UPCIE_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x103A800C)                                                  */
/*       SKX (0x103A800C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_M2UPCIE_MAIN_REG 0x0C00100C
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} CLSR_M2UPCIE_MAIN_STRUCT;


/* HDR_M2UPCIE_MAIN_REG supported on:                                         */
/*       SKX_A0 (0x103A800E)                                                  */
/*       SKX (0x103A800E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_M2UPCIE_MAIN_REG 0x0C00100E
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} HDR_M2UPCIE_MAIN_STRUCT;


/* BIST_M2UPCIE_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x103A800F)                                                  */
/*       SKX (0x103A800F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_M2UPCIE_MAIN_REG 0x0C00100F
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} BIST_M2UPCIE_MAIN_STRUCT;


/* SVID_0_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x203A802C)                                                  */
/*       SKX (0x203A802C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_0_M2UPCIE_MAIN_REG 0x0C00202C
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * generated by critter 16_0_0x02c
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110  */
  } Bits;
  UINT16 Data;
} SVID_0_M2UPCIE_MAIN_STRUCT;


/* SDID_0_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x203A802E)                                                  */
/*       SKX (0x203A802E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_0_M2UPCIE_MAIN_REG 0x0C00202E
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * generated by critter 16_0_0x02e
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000  */
  } Bits;
  UINT16 Data;
} SDID_0_M2UPCIE_MAIN_STRUCT;


/* CAPPTR_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x103A8034)                                                  */
/*       SKX (0x103A8034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_M2UPCIE_MAIN_REG 0x0C001034
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} CAPPTR_M2UPCIE_MAIN_STRUCT;


/* INTL_M2UPCIE_MAIN_REG supported on:                                        */
/*       SKX_A0 (0x103A803C)                                                  */
/*       SKX (0x103A803C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_M2UPCIE_MAIN_REG 0x0C00103C
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} INTL_M2UPCIE_MAIN_STRUCT;


/* INTPIN_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x103A803D)                                                  */
/*       SKX (0x103A803D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_M2UPCIE_MAIN_REG 0x0C00103D
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} INTPIN_M2UPCIE_MAIN_STRUCT;


/* MINGNT_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x103A803E)                                                  */
/*       SKX (0x103A803E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_M2UPCIE_MAIN_REG 0x0C00103E
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} MINGNT_M2UPCIE_MAIN_STRUCT;


/* MAXLAT_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x103A803F)                                                  */
/*       SKX (0x103A803F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_M2UPCIE_MAIN_REG 0x0C00103F
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
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
} MAXLAT_M2UPCIE_MAIN_STRUCT;


/* QPI_RT_M2UPCIE_MAIN_REG supported on:                                      */
/*       SKX_A0 (0x203A8040)                                                  */
/*       SKX (0x203A8040)                                                     */
/* Register default value:              0x0000                                */
#define QPI_RT_M2UPCIE_MAIN_REG 0x0C002040
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * Intel UPI Routing Table
 */
typedef union {
  struct {
    UINT16 qpiroutingtable : 16;
    /* qpiroutingtable - Bits[15:0], RW_LB, default = 16'b0000000000000000 
       Bit 0 corresponds to NodeID0 
                            Bit 1 corresponds to NodeID1 
                            ... 
                            Value of 0 means Intel UPI 0 
                            Value of 1 means Intel UPI 1 
               
     */
  } Bits;
  UINT16 Data;
} QPI_RT_M2UPCIE_MAIN_STRUCT;


/* IIO_BW_COUNTER_M2UPCIE_MAIN_REG supported on:                              */
/*       SKX_A0 (0x403A8044)                                                  */
/*       SKX (0x403A8044)                                                     */
/* Register default value:              0x00000000                            */
#define IIO_BW_COUNTER_M2UPCIE_MAIN_REG 0x0C004044
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * IIO Bandwidth Counter
 */
typedef union {
  struct {
    UINT32 iio_bandwidth_counter : 30;
    /* iio_bandwidth_counter - Bits[29:0], RW1C, default = 30'b000000000000000000000000000000 
       Free running counter that increments on each AD request sent to the ring. Pcode 
       uses this for power metering and also for uncore P state related decisions. 
       Pcode can clear this counter by writing a '1' to all bits in this field, at 
       which time the counter starts from 0. 
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} IIO_BW_COUNTER_M2UPCIE_MAIN_STRUCT;


/* R2PGNCTRL_M2UPCIE_MAIN_REG supported on:                                   */
/*       SKX_A0 (0x403A8048)                                                  */
/*       SKX (0x403A8048)                                                     */
/* Register default value:              0x00000000                            */
#define R2PGNCTRL_M2UPCIE_MAIN_REG 0x0C004048
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * R2PCIe General Control Register
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 qpi_rt_sel_ndr_drs : 1;
    /* qpi_rt_sel_ndr_drs - Bits[1:1], RW_LB, default = 1'b0 
       When this bit is set, R2PCIe routes NDR and DRS packets destined to remote 
       sockets, using the QPI_RT. When this bit is clear, R2PCIe routes NDR and DRS 
       packets destined to remote sockets, using the Intel UPI Link_ID field sent by 
       IIO/Ubox along with these packet. 
     */
    UINT32 rsvd_2 : 30;
    /* rsvd_2 - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2PGNCTRL_M2UPCIE_MAIN_STRUCT;


/* R2PINGERRLOG0_M2UPCIE_MAIN_REG supported on:                               */
/*       SKX_A0 (0x403A804C)                                                  */
/*       SKX (0x403A804C)                                                     */
/* Register default value:              0x00000000                            */
#define R2PINGERRLOG0_M2UPCIE_MAIN_REG 0x0C00404C
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * R2PCIe Ingress error log 0
 */
typedef union {
  struct {
    UINT32 ak0bgfcrdoverflow : 1;
    /* ak0bgfcrdoverflow - Bits[0:0], RW1CS, default = 1'b0 
       AK BGF 0 BGF credit overflow
     */
    UINT32 ak1bgfcrdoverflow : 1;
    /* ak1bgfcrdoverflow - Bits[1:1], RW1CS, default = 1'b0 
       AK BGF 1 BGF credit overflow
     */
    UINT32 blbgfcrdoverflow : 1;
    /* blbgfcrdoverflow - Bits[2:2], RW1CS, default = 1'b0 
       BL BGF credit overflow
     */
    UINT32 ubxcboncbcrdoverflow : 1;
    /* ubxcboncbcrdoverflow - Bits[3:3], RW1CS, default = 1'b0 
       Ubox Cbo NCB credit overflow
     */
    UINT32 ubxcboncscrdoverflow : 1;
    /* ubxcboncscrdoverflow - Bits[4:4], RW1CS, default = 1'b0 
       Ubox Cbo NCS credit overflow
     */
    UINT32 ubxqpincbcrdoverflow : 1;
    /* ubxqpincbcrdoverflow - Bits[5:5], RW1CS, default = 1'b0 
       Ubox Intel UPI NCB credit overflow
     */
    UINT32 ubxqpincscrdoverflow : 1;
    /* ubxqpincscrdoverflow - Bits[6:6], RW1CS, default = 1'b0 
       Ubox Intel UPI NCS credit overflow
     */
    UINT32 iioidicrdoverflow : 1;
    /* iioidicrdoverflow - Bits[7:7], RW1CS, default = 1'b0 
       IIO IDI credit overflow
     */
    UINT32 iioncbcrdoverflow : 1;
    /* iioncbcrdoverflow - Bits[8:8], RW1CS, default = 1'b0 
       IIO NCB credit overflow
     */
    UINT32 iioncscrdoverflow : 1;
    /* iioncscrdoverflow - Bits[9:9], RW1CS, default = 1'b0 
       IIO NCS credit overflow
     */
    UINT32 parerring0 : 1;
    /* parerring0 - Bits[10:10], RW1CS, default = 1'b0 
       Ingress BL data parity error - Agent 0
     */
    UINT32 parerring0injdone : 1;
    /* parerring0injdone - Bits[11:11], RW1CS, default = 1'b0 
       Inidcating parity error injection is done. S/W
               need clear this bit if it wants to inject another error
     */
    UINT32 ubxiioncbcreditoverflow : 1;
    /* ubxiioncbcreditoverflow - Bits[12:12], RW1CS, default = 1'b0  */
    UINT32 ubxiioncscreditoverflow : 1;
    /* ubxiioncscreditoverflow - Bits[13:13], RW1CS, default = 1'b0  */
    UINT32 ivbgfcrdoverflow : 1;
    /* ivbgfcrdoverflow - Bits[14:14], RW1CS, default = 1'b0 
       IV BGF credit overflow
     */
    UINT32 ivsnpcrdoverflow : 1;
    /* ivsnpcrdoverflow - Bits[15:15], RW1CS, default = 1'b0 
       IV Snoop credit overflow
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2PINGERRLOG0_M2UPCIE_MAIN_STRUCT;


/* R2PINGERRMSK0_M2UPCIE_MAIN_REG supported on:                               */
/*       SKX_A0 (0x403A8050)                                                  */
/*       SKX (0x403A8050)                                                     */
/* Register default value:              0x00001800                            */
#define R2PINGERRMSK0_M2UPCIE_MAIN_REG 0x0C004050
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * R2PCIe Ingress error mask register 0
 */
typedef union {
  struct {
    UINT32 ak0bgfcrderrmsk : 1;
    /* ak0bgfcrderrmsk - Bits[0:0], RWS, default = 1'b0 
       Mask AK 0 BGF credit overflow error
     */
    UINT32 ak1bgfcrderrmsk : 1;
    /* ak1bgfcrderrmsk - Bits[1:1], RWS, default = 1'b0 
       Mask AK 1 BGF credit overflow error
     */
    UINT32 blbgfcrderrmsk : 1;
    /* blbgfcrderrmsk - Bits[2:2], RWS, default = 1'b0 
       Mask BL BGF credit overflow error
     */
    UINT32 ubxcboncbcrderrmsk : 1;
    /* ubxcboncbcrderrmsk - Bits[3:3], RWS, default = 1'b0 
       Mask Ubox Cbo NCB credit overflow error
     */
    UINT32 ubxcboncscrderrmsk : 1;
    /* ubxcboncscrderrmsk - Bits[4:4], RWS, default = 1'b0 
       Mask Ubox Cbo NCS credit overflow error
     */
    UINT32 ubxqpincbcrderrmsk : 1;
    /* ubxqpincbcrderrmsk - Bits[5:5], RWS, default = 1'b0 
       Mask Ubox Intel UPI NCB credit overflow error
     */
    UINT32 ubxqpincscrderrmsk : 1;
    /* ubxqpincscrderrmsk - Bits[6:6], RWS, default = 1'b0 
       Mask Ubox Intel UPI NCS credit overflow error
     */
    UINT32 iioidicrderrmsk : 1;
    /* iioidicrderrmsk - Bits[7:7], RWS, default = 1'b0 
       Mask IIO IDI credit overflow error
     */
    UINT32 iioncbcrderrmsk : 1;
    /* iioncbcrderrmsk - Bits[8:8], RWS, default = 1'b0 
       Mask IIO NCB credit overflow error
     */
    UINT32 iioncscrderrmsk : 1;
    /* iioncscrderrmsk - Bits[9:9], RWS, default = 1'b0 
       Mask IIO NCS credit overflow error
     */
    UINT32 parerring0msk : 1;
    /* parerring0msk - Bits[10:10], RWS, default = 1'b0 
       Mask BL ingress data parity error - Agent 0
     */
    UINT32 ubxiioncbcrediterrmsk : 1;
    /* ubxiioncbcrediterrmsk - Bits[11:11], RWS, default = 1'b1  */
    UINT32 ubxiioncscrediterrmsk : 1;
    /* ubxiioncscrediterrmsk - Bits[12:12], RWS, default = 1'b1  */
    UINT32 rsvd_13 : 1;
    /* rsvd_13 - Bits[13:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ivbgfcrderrmsk : 1;
    /* ivbgfcrderrmsk - Bits[14:14], RWS, default = 1'b0 
       IV BGF credit overflow error
     */
    UINT32 ivsnpcrderrmsk : 1;
    /* ivsnpcrderrmsk - Bits[15:15], RWS, default = 1'b0 
       IV Snoop credit overflow error
     */
    UINT32 ingparerrinjentry : 7;
    /* ingparerrinjentry - Bits[22:16], RWS, default = 7'b0 
       6'b111111: Par error injection happens to next
               Ingress Write; otherwise, Inject error the the ingress entry
               specified by this field
     */
    UINT32 ingparerrinj : 1;
    /* ingparerrinj - Bits[23:23], RWS_LB, default = 1'b0 
       When set, one single parity error injection will happen
               to an ingress entry write sepcified by
               R2PINGERRMSK0.IngParErrInjEntry. Once injection is done,
               R2PINGERRLOG0.ParErrIng0InjDone will be set. S/W need clear
               both bits if it wants to do another injection 
     */
    UINT32 rsvd_24 : 8;
    /* rsvd_24 - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2PINGERRMSK0_M2UPCIE_MAIN_STRUCT;








/* R2EGRISOERRLOG0_M2UPCIE_MAIN_REG supported on:                             */
/*       SKX_A0 (0x403A8060)                                                  */
/*       SKX (0x403A8060)                                                     */
/* Register default value:              0x00000000                            */
#define R2EGRISOERRLOG0_M2UPCIE_MAIN_REG 0x0C004060
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * ISO Credit Overflow Error for each Cbo. bit[31:0] maps to error for Cbo[31:0] 
 * (if applicable) 
 */
typedef union {
  struct {
    UINT32 cboisochcrdoverflow_31_00 : 32;
    /* cboisochcrdoverflow_31_00 - Bits[31:0], RW1CS, default = 32'b0  */
  } Bits;
  UINT32 Data;
} R2EGRISOERRLOG0_M2UPCIE_MAIN_STRUCT;




/* R2EGRPRQERRLOG0_M2UPCIE_MAIN_REG supported on:                             */
/*       SKX_A0 (0x403A8068)                                                  */
/*       SKX (0x403A8068)                                                     */
/* Register default value:              0x00000000                            */
#define R2EGRPRQERRLOG0_M2UPCIE_MAIN_REG 0x0C004068
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PRQ Credit Overflow Error for each Cbo. bit[31:0] maps to error for Cbo[31:0] 
 * (if applicable) 
 */
typedef union {
  struct {
    UINT32 cboprqcrdoverflow_31_00 : 32;
    /* cboprqcrdoverflow_31_00 - Bits[31:0], RW1CS, default = 32'b0  */
  } Bits;
  UINT32 Data;
} R2EGRPRQERRLOG0_M2UPCIE_MAIN_STRUCT;








/* R2EGRISOERRMSK0_M2UPCIE_MAIN_REG supported on:                             */
/*       SKX_A0 (0x403A8078)                                                  */
/*       SKX (0x403A8078)                                                     */
/* Register default value:              0x00000000                            */
#define R2EGRISOERRMSK0_M2UPCIE_MAIN_REG 0x0C004078
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * ISO Credit Error Mask for each Cbo. bit[31:0] maps to error for Cbo[31:0] (if 
 * applicable) 
 */
typedef union {
  struct {
    UINT32 cboisochcrdoverflowmask_31_00 : 32;
    /* cboisochcrdoverflowmask_31_00 - Bits[31:0], RWS, default = 32'b0  */
  } Bits;
  UINT32 Data;
} R2EGRISOERRMSK0_M2UPCIE_MAIN_STRUCT;




/* R2EGRPRQERRMSK0_M2UPCIE_MAIN_REG supported on:                             */
/*       SKX_A0 (0x403A8080)                                                  */
/*       SKX (0x403A8080)                                                     */
/* Register default value:              0x00000000                            */
#define R2EGRPRQERRMSK0_M2UPCIE_MAIN_REG 0x0C004080
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PRQ Credit Error Mask for each Cbo. bit[31:0] maps to error for Cbo[31:0] (if 
 * applicable) 
 */
typedef union {
  struct {
    UINT32 cboprqcrdoverflowmask_31_00 : 32;
    /* cboprqcrdoverflowmask_31_00 - Bits[31:0], RWS, default = 32'b0  */
  } Bits;
  UINT32 Data;
} R2EGRPRQERRMSK0_M2UPCIE_MAIN_STRUCT;








/* R2PISOINITCRDT0_M2UPCIE_MAIN_REG supported on:                             */
/*       SKX_A0 (0x403A8090)                                                  */
/*       SKX (0x403A8090)                                                     */
/* Register default value:              0x55555555                            */
#define R2PISOINITCRDT0_M2UPCIE_MAIN_REG 0x0C004090
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * ISO inital credit to each Cbo. bit[31:0] maps to Cbo[15:0](if applicable), 2-bit 
 * per Cbo. [1:0] for Cbo0 as so on 
 */
typedef union {
  struct {
    UINT32 cboisochinitialcred_15_00 : 32;
    /* cboisochinitialcred_15_00 - Bits[31:0], RWS_LB, default = 32'h55555555  */
  } Bits;
  UINT32 Data;
} R2PISOINITCRDT0_M2UPCIE_MAIN_STRUCT;


/* R2PISOINITCRDT1_M2UPCIE_MAIN_REG supported on:                             */
/*       SKX_A0 (0x403A8094)                                                  */
/*       SKX (0x403A8094)                                                     */
/* Register default value:              0x55555555                            */
#define R2PISOINITCRDT1_M2UPCIE_MAIN_REG 0x0C004094
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * ISO inital credit to each Cbo. bit[31:0] maps to Cbo[31:16](if applicable), 
 * 2-bit per Cbo. [1:0] for Cbo16 as so on 
 */
typedef union {
  struct {
    UINT32 cboisochinitialcred_31_16 : 32;
    /* cboisochinitialcred_31_16 - Bits[31:0], RWS_LB, default = 32'h55555555  */
  } Bits;
  UINT32 Data;
} R2PISOINITCRDT1_M2UPCIE_MAIN_STRUCT;


/* R2PISOINITCRDT2_M2UPCIE_MAIN_REG supported on:                             */
/*       SKX_A0 (0x403A8098)                                                  */
/*       SKX (0x403A8098)                                                     */
/* Register default value:              0x00000555                            */
#define R2PISOINITCRDT2_M2UPCIE_MAIN_REG 0x0C004098
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * ISO inital credit to each Cbo. bit[11:0] maps to Cbo[37:32](if applicable), 
 * 2-bit per Cbo. [1:0] for Cbo32 as so on 
 */
typedef union {
  struct {
    UINT32 cboisochinitialcred_37_32 : 12;
    /* cboisochinitialcred_37_32 - Bits[11:0], RWS_LB, default = 12'h555  */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2PISOINITCRDT2_M2UPCIE_MAIN_STRUCT;


/* R2PRQINITCRED0_M2UPCIE_MAIN_REG supported on:                              */
/*       SKX_A0 (0x403A809C)                                                  */
/*       SKX (0x403A809C)                                                     */
/* Register default value:              0x33333333                            */
#define R2PRQINITCRED0_M2UPCIE_MAIN_REG 0x0C00409C
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PRQ inital credit to each Cbo. bit[31:0] maps to Cbo[7:0](if applicable), 4-bit 
 * per Cbo. [3:0] for Cbo0 as so on 
 */
typedef union {
  struct {
    UINT32 cboprqinitialcred_07_00 : 32;
    /* cboprqinitialcred_07_00 - Bits[31:0], RWS_LB, default = 32'h33333333  */
  } Bits;
  UINT32 Data;
} R2PRQINITCRED0_M2UPCIE_MAIN_STRUCT;


/* R2PRQINITCRED1_M2UPCIE_MAIN_REG supported on:                              */
/*       SKX_A0 (0x403A80A0)                                                  */
/*       SKX (0x403A80A0)                                                     */
/* Register default value:              0x33333333                            */
#define R2PRQINITCRED1_M2UPCIE_MAIN_REG 0x0C0040A0
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PRQ inital credit to each Cbo. bit[31:0] maps to Cbo[15:8](if applicable), 4-bit 
 * per Cbo. [3:0] for Cbo8 as so on 
 */
typedef union {
  struct {
    UINT32 cboprqinitialcred_15_08 : 32;
    /* cboprqinitialcred_15_08 - Bits[31:0], RWS_LB, default = 32'h33333333  */
  } Bits;
  UINT32 Data;
} R2PRQINITCRED1_M2UPCIE_MAIN_STRUCT;


/* R2PRQINITCRED2_M2UPCIE_MAIN_REG supported on:                              */
/*       SKX_A0 (0x403A80A4)                                                  */
/*       SKX (0x403A80A4)                                                     */
/* Register default value:              0x33333333                            */
#define R2PRQINITCRED2_M2UPCIE_MAIN_REG 0x0C0040A4
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PRQ inital credit to each Cbo. bit[31:0] maps to Cbo[23:16](if applicable), 
 * 4-bit per Cbo. [3:0] for Cbo16 as so on 
 */
typedef union {
  struct {
    UINT32 cboprqinitialcred_23_16 : 32;
    /* cboprqinitialcred_23_16 - Bits[31:0], RWS_LB, default = 32'h33333333  */
  } Bits;
  UINT32 Data;
} R2PRQINITCRED2_M2UPCIE_MAIN_STRUCT;


/* R2GLERRCFG_M2UPCIE_MAIN_REG supported on:                                  */
/*       SKX_A0 (0x403A80A8)                                                  */
/*       SKX (0x403A80A8)                                                     */
/* Register default value:              0x000066E0                            */
#define R2GLERRCFG_M2UPCIE_MAIN_REG 0x0C0040A8
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * R2PCIe global viral/fatal error configuration
 */
typedef union {
  struct {
    UINT32 zero3 : 3;
    /* zero3 - Bits[2:0], RO, default = 3'b000  */
    UINT32 resetglobalfatalerror : 1;
    /* resetglobalfatalerror - Bits[3:3], RW, default = 1'b0 
       Set 1 to force clear global fatal error status.
     */
    UINT32 resetglobalviral : 1;
    /* resetglobalviral - Bits[4:4], RW, default = 1'b0 
       Set 1 to force clear global viral status.
     */
    UINT32 maskuboxviralin : 1;
    /* maskuboxviralin - Bits[5:5], RWS, default = 1'b1 
       Set 1 to block viral status of Ubox
     */
    UINT32 maskuboxfatalerrorin : 1;
    /* maskuboxfatalerrorin - Bits[6:6], RWS, default = 1'b1 
       Set 1 to block fatal error status of Ubox to global fatal status
     */
    UINT32 maskiiofatalerrorin : 1;
    /* maskiiofatalerrorin - Bits[7:7], RWS, default = 1'b1 
       Set 1 to block fatal error status from IIO to global fatal status
     */
    UINT32 zero1 : 1;
    /* zero1 - Bits[8:8], RO, default = 1'b0  */
    UINT32 maskiioviralout : 1;
    /* maskiioviralout - Bits[9:9], RWS, default = 1'b1 
       Set 1 to block Vrial status from global viral status to IIO
     */
    UINT32 maskiioviralin : 1;
    /* maskiioviralin - Bits[10:10], RWS, default = 1'b1 
       Set 1 to block Vrial status from IIO to global viral status
     */
    UINT32 zero2 : 2;
    /* zero2 - Bits[12:11], RO, default = 2'b00  */
    UINT32 maskr2fatalerror : 2;
    /* maskr2fatalerror - Bits[14:13], RWS, default = 2'b11 
       Set 1 to block Set 1 to block from R2PCIE Error to global fatal error status
     */
    UINT32 localerrorstatus : 1;
    /* localerrorstatus - Bits[15:15], RO_V, default = 1'b0 
       Read only R2PCIe error status
     */
    UINT32 viralstatustoiio : 1;
    /* viralstatustoiio - Bits[16:16], RO_V, default = 1'b0 
       Read only to IIO viral status
     */
    UINT32 viralstatusfromiio : 1;
    /* viralstatusfromiio - Bits[17:17], RO_V, default = 1'b0 
       Read only from IIO viral status
     */
    UINT32 fatalstatusfromiio : 1;
    /* fatalstatusfromiio - Bits[18:18], RO_V, default = 1'b0 
       Read only from IIO fatal status
     */
    UINT32 viralstatusfromubox : 1;
    /* viralstatusfromubox - Bits[19:19], RO_V, default = 1'b0 
       Read only from Ubox viral status
     */
    UINT32 fatalstatusfromubox : 1;
    /* fatalstatusfromubox - Bits[20:20], RO_V, default = 1'b0 
       Read only from Ubox fatal status
     */
    UINT32 rsvd : 11;
    /* rsvd - Bits[31:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2GLERRCFG_M2UPCIE_MAIN_STRUCT;


/* R2PRQINITCRED3_M2UPCIE_MAIN_REG supported on:                              */
/*       SKX_A0 (0x403A80AC)                                                  */
/*       SKX (0x403A80AC)                                                     */
/* Register default value:              0x33333333                            */
#define R2PRQINITCRED3_M2UPCIE_MAIN_REG 0x0C0040AC
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PRQ inital credit to each Cbo. bit[31:0] maps to Cbo[31:24](if applicable), 
 * 4-bit per Cbo. [3:0] for Cbo24 as so on 
 */
typedef union {
  struct {
    UINT32 cboprqinitialcred_31_24 : 32;
    /* cboprqinitialcred_31_24 - Bits[31:0], RWS_LB, default = 32'h33333333  */
  } Bits;
  UINT32 Data;
} R2PRQINITCRED3_M2UPCIE_MAIN_STRUCT;


/* R2EGRERRLOG_M2UPCIE_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x403A80B0)                                                  */
/*       SKX (0x403A80B0)                                                     */
/* Register default value:              0x00000000                            */
#define R2EGRERRLOG_M2UPCIE_MAIN_REG 0x0C0040B0
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * R2PCIe Egress error log
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    /* rsvd_0 - Bits[17:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 akegress0_write_to_valid_entry : 1;
    /* akegress0_write_to_valid_entry - Bits[18:18], RW1CS, default = 1'b0 
       AK Egress Agent 0 write to occupied entry
     */
    UINT32 akegress1_write_to_valid_entry : 1;
    /* akegress1_write_to_valid_entry - Bits[19:19], RW1CS, default = 1'b0 
       AK Egress Agent 1 write to occupied entry
     */
    UINT32 blegress0_write_to_valid_entry : 1;
    /* blegress0_write_to_valid_entry - Bits[20:20], RW1CS, default = 1'b0 
       BL Egress Agent 0 write to occupied entry
     */
    UINT32 blegress1_write_to_valid_entry : 1;
    /* blegress1_write_to_valid_entry - Bits[21:21], RW1CS, default = 1'b0 
       BL Egress Agent 1 write to occupied entry
     */
    UINT32 adegress0_write_to_valid_entry : 1;
    /* adegress0_write_to_valid_entry - Bits[22:22], RW1CS, default = 1'b0 
       AD Egress Agent 0 write to occupied entry
     */
    UINT32 adegress1_write_to_valid_entry : 1;
    /* adegress1_write_to_valid_entry - Bits[23:23], RW1CS, default = 1'b0 
       AD Egress Agent 1 write to occupied entry
     */
    UINT32 akegress0_overflow : 1;
    /* akegress0_overflow - Bits[24:24], RW1CS, default = 1'b0 
       AK Egress Agent 0 buffer overflow
     */
    UINT32 akegress1_overflow : 1;
    /* akegress1_overflow - Bits[25:25], RW1CS, default = 1'b0 
       AK Egress Agent 1 buffer overflow
     */
    UINT32 blegress0_overflow : 1;
    /* blegress0_overflow - Bits[26:26], RW1CS, default = 1'b0 
       BL Egress Agent 0 buffer overflow
     */
    UINT32 blegress1_overflow : 1;
    /* blegress1_overflow - Bits[27:27], RW1CS, default = 1'b0 
       BL Egress Agent 1 buffer overflow
     */
    UINT32 adegress0_overflow : 1;
    /* adegress0_overflow - Bits[28:28], RW1CS, default = 1'b0 
       AD Egress Agent 0 buffer overflow
     */
    UINT32 adegress1_overflow : 1;
    /* adegress1_overflow - Bits[29:29], RW1CS, default = 1'b0 
       AD Egress Agent 1 buffer overflow
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2EGRERRLOG_M2UPCIE_MAIN_STRUCT;


/* R2PRQINITCRED4_M2UPCIE_MAIN_REG supported on:                              */
/*       SKX_A0 (0x403A80B4)                                                  */
/*       SKX (0x403A80B4)                                                     */
/* Register default value:              0x00333333                            */
#define R2PRQINITCRED4_M2UPCIE_MAIN_REG 0x0C0040B4
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * PRQ inital credit to each Cbo. bit[23:0] maps to Cbo[37:32](if applicable), 
 * 4-bit per Cbo. [3:0] for Cbo32 as so on 
 */
typedef union {
  struct {
    UINT32 cboprqinitialcred_37_32 : 24;
    /* cboprqinitialcred_37_32 - Bits[23:0], RWS_LB, default = 24'h333333  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2PRQINITCRED4_M2UPCIE_MAIN_STRUCT;


/* R2EGRERRMSK_M2UPCIE_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x403A80B8)                                                  */
/*       SKX (0x403A80B8)                                                     */
/* Register default value:              0x00000000                            */
#define R2EGRERRMSK_M2UPCIE_MAIN_REG 0x0C0040B8
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * R2PCIe Error Mask Register
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    /* rsvd_0 - Bits[17:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 akegress0_write_to_valid_entry : 1;
    /* akegress0_write_to_valid_entry - Bits[18:18], RWS, default = 1'b0 
       Mask AK Egress Agent 0 write to occupied entry error
     */
    UINT32 akegress1_write_to_valid_entry : 1;
    /* akegress1_write_to_valid_entry - Bits[19:19], RWS, default = 1'b0 
       Mask AK Egress Agent 1 write to occupied entry error
     */
    UINT32 blegress0_write_to_valid_entry : 1;
    /* blegress0_write_to_valid_entry - Bits[20:20], RWS, default = 1'b0 
       Mask BL Egress Agent 0 write to occupied entry error
     */
    UINT32 blegress1_write_to_valid_entry : 1;
    /* blegress1_write_to_valid_entry - Bits[21:21], RWS, default = 1'b0 
       Mask BL Egress Agent 1 write to occupied entry error
     */
    UINT32 adegress0_write_to_valid_entry : 1;
    /* adegress0_write_to_valid_entry - Bits[22:22], RWS, default = 1'b0 
       Mask AD Egress Agent 0 write to occupied entry error
     */
    UINT32 adegress1_write_to_valid_entry : 1;
    /* adegress1_write_to_valid_entry - Bits[23:23], RWS, default = 1'b0 
       Mask AD Egress Agent 0 write to occupied entry error
     */
    UINT32 akegress0_overflow : 1;
    /* akegress0_overflow - Bits[24:24], RWS, default = 1'b0 
       Mask AK Egress Agent 0 overflow error
     */
    UINT32 akegress1_overflow : 1;
    /* akegress1_overflow - Bits[25:25], RWS, default = 1'b0 
       Mask AK Egress Agent 1 overflow error
     */
    UINT32 blegress0_overflow : 1;
    /* blegress0_overflow - Bits[26:26], RWS, default = 1'b0 
       Mask BL Egress Agent 0 overflow error
     */
    UINT32 blegress1_overflow : 1;
    /* blegress1_overflow - Bits[27:27], RWS, default = 1'b0 
       Mask AK Egress Agent 1 overflow error
     */
    UINT32 adegress0_overflow : 1;
    /* adegress0_overflow - Bits[28:28], RWS, default = 1'b0 
       Mask AD Egress Agent 0 overflow error
     */
    UINT32 adegress1_overflow : 1;
    /* adegress1_overflow - Bits[29:29], RWS, default = 1'b0 
       Mask AD Egress Agent 1 overflow error
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2EGRERRMSK_M2UPCIE_MAIN_STRUCT;


/* R2NCBTOIIOINITCRED_M2UPCIE_MAIN_REG supported on:                          */
/*       SKX_A0 (0x403A80BC)                                                  */
/*       SKX (0x403A80BC)                                                     */
/* Register default value:              0x00111111                            */
#define R2NCBTOIIOINITCRED_M2UPCIE_MAIN_REG 0x0C0040BC
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * 
 *         Inital dest IIO ingress credit assignment for NCB P2P. 4-bit per agent. 
 *          [ 3 : 0]: M2PCIE20 - Cstack 0; 
 *          [ 7 : 4]: M2PCIE20 - Pstack 1; 
 *          [11 : 8]: M2PCIE10 - Pstack 2;
 *          [15 :12]: M2PCIE50 - Pstack 3;
 *          [19 :16]: M2PCIE30 - Mstack 4;
 *          [23 :20]: M2PCIE40 - Mstack 5;
 *         There is no traffic to the same stack
 *       
 */
typedef union {
  struct {
    UINT32 ncbdstiioinitialcrd : 24;
    /* ncbdstiioinitialcrd - Bits[23:0], RWS_LB, default = 24'h111111  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2NCBTOIIOINITCRED_M2UPCIE_MAIN_STRUCT;


/* R2NCSTOIIOINITCRED_M2UPCIE_MAIN_REG supported on:                          */
/*       SKX_A0 (0x403A80C0)                                                  */
/*       SKX (0x403A80C0)                                                     */
/* Register default value:              0x00111111                            */
#define R2NCSTOIIOINITCRED_M2UPCIE_MAIN_REG 0x0C0040C0
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * 
 *         Inital dest IIO ingress credit assignment for NCB P2P. 4-bit per agent. 
 *          [ 3 : 0]: M2PCIE20 - Cstack 0; 
 *          [ 7 : 4]: M2PCIE20 - Pstack 1; 
 *          [11 : 8]: M2PCIE10 - Pstack 2;
 *          [15 :12]: M2PCIE50 - Pstack 3;
 *          [19 :16]: M2PCIE30 - Mstack 4;
 *          [23 :20]: M2PCIE40 - Mstack 5;
 *         There is no traffic to the same stack
 *       
 */
typedef union {
  struct {
    UINT32 ncsdstiioinitialcrd : 24;
    /* ncsdstiioinitialcrd - Bits[23:0], RWS_LB, default = 24'h111111  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2NCSTOIIOINITCRED_M2UPCIE_MAIN_STRUCT;


/* R2NCBTOKTIINITCRED_M2UPCIE_MAIN_REG supported on:                          */
/*       SKX_A0 (0x403A80C4)                                                  */
/*       SKX (0x403A80C4)                                                     */
/* Register default value:              0x00001111                            */
#define R2NCBTOKTIINITCRED_M2UPCIE_MAIN_REG 0x0C0040C4
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * 
 *         Inital dest Intel UPI ingress credit assignment for NCB P2P. 4-bit per 
 * agent.  
 *          [ 3 : 0]: Intel UPI 0
 *          [ 7 : 4]: Intel UPI 1 
 *          [11 : 8]: Intel UPI 2
 *          [15 :12]: Intel UPI 3
 *       
 */
typedef union {
  struct {
    UINT32 ncbdstktiinitialcrd : 16;
    /* ncbdstktiinitialcrd - Bits[15:0], RWS_LB, default = 16'h1111  */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2NCBTOKTIINITCRED_M2UPCIE_MAIN_STRUCT;


/* R2NCSTOKTIINITCRED_M2UPCIE_MAIN_REG supported on:                          */
/*       SKX_A0 (0x403A80C8)                                                  */
/*       SKX (0x403A80C8)                                                     */
/* Register default value:              0x00001111                            */
#define R2NCSTOKTIINITCRED_M2UPCIE_MAIN_REG 0x0C0040C8
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * 
 *         Inital dest Intel UPI ingress credit assignment for NCB . 4-bit per 
 * agent.  
 *          [ 3 : 0]: Intel UPI 0
 *          [ 7 : 4]: Intel UPI 1
 *          [11 : 8]: Intel UPI 2
 *          [15 :12]: Intel UPI 3
 *       
 */
typedef union {
  struct {
    UINT32 ncsdstktiinitialcrd : 16;
    /* ncsdstktiinitialcrd - Bits[15:0], RWS_LB, default = 16'h1111  */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2NCSTOKTIINITCRED_M2UPCIE_MAIN_STRUCT;


/* R2DRSTOIIOINITCRED_M2UPCIE_MAIN_REG supported on:                          */
/*       SKX_A0 (0x403A80CC)                                                  */
/*       SKX (0x403A80CC)                                                     */
/* Register default value:              0x00111111                            */
#define R2DRSTOIIOINITCRED_M2UPCIE_MAIN_REG 0x0C0040CC
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * 
 *         Inital dest IIO ingress credit assignment for DRS. 4-bit per agent. 
 *          [ 3 : 0]: M2PCIE20 - Cstack 0; 
 *          [ 7 : 4]: M2PCIE20 - Pstack 1; 
 *          [11 : 8]: M2PCIE10 - Pstack 2;
 *          [15 :12]: M2PCIE50 - Pstack 3;
 *          [19 :16]: M2PCIE30 - Mstack 4;
 *          [23 :20]: M2PCIE40 - Mstack 5;
 *         There is no traffic to the same stack
 *       
 */
typedef union {
  struct {
    UINT32 drsdstiioinitialcrd : 24;
    /* drsdstiioinitialcrd - Bits[23:0], RWS_LB, default = 24'h111111  */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2DRSTOIIOINITCRED_M2UPCIE_MAIN_STRUCT;










/* R2DRSTOKTIINITCRED_M2UPCIE_MAIN_REG supported on:                          */
/*       SKX_A0 (0x403A80E0)                                                  */
/*       SKX (0x403A80E0)                                                     */
/* Register default value:              0x00001111                            */
#define R2DRSTOKTIINITCRED_M2UPCIE_MAIN_REG 0x0C0040E0
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * 
 *         Inital dest Intel UPI ingress credit assignment for DRS. 4-bit per 
 * agent.  
 *          [ 3 : 0]: Intel UPI 0
 *          [ 7 : 4]: Intel UPI 1
 *          [11 : 8]: Intel UPI 2
 *          [15 :12]: Intel UPI 3
 *       
 */
typedef union {
  struct {
    UINT32 drsdstktiinitialcrd : 16;
    /* drsdstktiinitialcrd - Bits[15:0], RWS_LB, default = 16'h1111  */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2DRSTOKTIINITCRED_M2UPCIE_MAIN_STRUCT;








/* R2NDRTOKTIINITCRED_M2UPCIE_MAIN_REG supported on:                          */
/*       SKX_A0 (0x403A80F0)                                                  */
/*       SKX (0x403A80F0)                                                     */
/* Register default value:              0x00001111                            */
#define R2NDRTOKTIINITCRED_M2UPCIE_MAIN_REG 0x0C0040F0
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * 
 *         Inital dest Intel UPI ingress credit assignment for NDR. 4-bit per 
 * agent.  
 *          [ 3 : 0]: Intel UPI 0
 *          [ 7 : 4]: Intel UPI 1
 *          [11 : 8]: Intel UPI 2
 *          [15 :12]: Intel UPI 3
 *       
 */
typedef union {
  struct {
    UINT32 ndrdstktiinitialcrd : 16;
    /* ndrdstktiinitialcrd - Bits[15:0], RWS_LB, default = 16'h1111  */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2NDRTOKTIINITCRED_M2UPCIE_MAIN_STRUCT;


/* R2DEFEATURE_M2UPCIE_MAIN_REG supported on:                                 */
/*       SKX_A0 (0x403A80F4)                                                  */
/*       SKX (0x403A80F4)                                                     */
/* Register default value:              0x00000200                            */
#define R2DEFEATURE_M2UPCIE_MAIN_REG 0x0C0040F4
/* Struct format extracted from XML file SKX\3.21.0.CFG.xml.
 * R2PCIe defeature register
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 9;
    /* rsvd_0 - Bits[8:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 vn0ncbiio2ubxinitialcred : 2;
    /* vn0ncbiio2ubxinitialcred - Bits[10:9], RWS_LB, default = 2'b01 
       Initial credit for request from IIO to Ubox
     */
    UINT32 rsvd_11 : 21;
    /* rsvd_11 - Bits[31:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} R2DEFEATURE_M2UPCIE_MAIN_STRUCT;


#endif /* M2UPCIE_MAIN_h */
