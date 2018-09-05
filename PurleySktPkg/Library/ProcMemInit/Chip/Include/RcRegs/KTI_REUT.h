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

#ifndef KTI_REUT_h
#define KTI_REUT_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* KTI_REUT_DEV 14                                                            */
/* KTI_REUT_FUN 1                                                             */

/* VID_KTI_REUT_REG supported on:                                             */
/*       SKX_A0 (0x20371000)                                                  */
/*       SKX (0x20371000)                                                     */
/* Register default value:              0x8086                                */
#define VID_KTI_REUT_REG 0x09012000
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} VID_KTI_REUT_STRUCT;


/* DID_KTI_REUT_REG supported on:                                             */
/*       SKX_A0 (0x20371002)                                                  */
/*       SKX (0x20371002)                                                     */
/* Register default value:              0x2059                                */
#define DID_KTI_REUT_REG 0x09012002
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2059 
        
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
} DID_KTI_REUT_STRUCT;


/* PCICMD_KTI_REUT_REG supported on:                                          */
/*       SKX_A0 (0x20371004)                                                  */
/*       SKX (0x20371004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_KTI_REUT_REG 0x09012004
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PCICMD_KTI_REUT_STRUCT;


/* PCISTS_KTI_REUT_REG supported on:                                          */
/*       SKX_A0 (0x20371006)                                                  */
/*       SKX (0x20371006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_KTI_REUT_REG 0x09012006
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PCISTS_KTI_REUT_STRUCT;


/* RID_KTI_REUT_REG supported on:                                             */
/*       SKX_A0 (0x10371008)                                                  */
/*       SKX (0x10371008)                                                     */
/* Register default value:              0x00                                  */
#define RID_KTI_REUT_REG 0x09011008
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} RID_KTI_REUT_STRUCT;


/* CCR_N0_KTI_REUT_REG supported on:                                          */
/*       SKX_A0 (0x10371009)                                                  */
/*       SKX (0x10371009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_KTI_REUT_REG 0x09011009
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_KTI_REUT_STRUCT;


/* CCR_N1_KTI_REUT_REG supported on:                                          */
/*       SKX_A0 (0x2037100A)                                                  */
/*       SKX (0x2037100A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_KTI_REUT_REG 0x0901200A
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} CCR_N1_KTI_REUT_STRUCT;


/* CLSR_KTI_REUT_REG supported on:                                            */
/*       SKX_A0 (0x1037100C)                                                  */
/*       SKX (0x1037100C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_KTI_REUT_REG 0x0901100C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} CLSR_KTI_REUT_STRUCT;


/* PLAT_KTI_REUT_REG supported on:                                            */
/*       SKX_A0 (0x1037100D)                                                  */
/*       SKX (0x1037100D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_KTI_REUT_REG 0x0901100D
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PLAT_KTI_REUT_STRUCT;


/* HDR_KTI_REUT_REG supported on:                                             */
/*       SKX_A0 (0x1037100E)                                                  */
/*       SKX (0x1037100E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_KTI_REUT_REG 0x0901100E
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} HDR_KTI_REUT_STRUCT;


/* BIST_KTI_REUT_REG supported on:                                            */
/*       SKX_A0 (0x1037100F)                                                  */
/*       SKX (0x1037100F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_KTI_REUT_REG 0x0901100F
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} BIST_KTI_REUT_STRUCT;


/* SVID_F1_KTI_REUT_REG supported on:                                         */
/*       SKX_A0 (0x2037102C)                                                  */
/*       SKX (0x2037102C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_F1_KTI_REUT_REG 0x0901202C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} SVID_F1_KTI_REUT_STRUCT;


/* SDID_F1_KTI_REUT_REG supported on:                                         */
/*       SKX_A0 (0x2037102E)                                                  */
/*       SKX (0x2037102E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_F1_KTI_REUT_REG 0x0901202E
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} SDID_F1_KTI_REUT_STRUCT;


/* CAPPTR_KTI_REUT_REG supported on:                                          */
/*       SKX_A0 (0x10371034)                                                  */
/*       SKX (0x10371034)                                                     */
/* Register default value:              0x40                                  */
#define CAPPTR_KTI_REUT_REG 0x09011034
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} CAPPTR_KTI_REUT_STRUCT;


/* INTL_KTI_REUT_REG supported on:                                            */
/*       SKX_A0 (0x1037103C)                                                  */
/*       SKX (0x1037103C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_KTI_REUT_REG 0x0901103C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} INTL_KTI_REUT_STRUCT;


/* INTPIN_KTI_REUT_REG supported on:                                          */
/*       SKX_A0 (0x1037103D)                                                  */
/*       SKX (0x1037103D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_KTI_REUT_REG 0x0901103D
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} INTPIN_KTI_REUT_STRUCT;


/* MINGNT_KTI_REUT_REG supported on:                                          */
/*       SKX_A0 (0x1037103E)                                                  */
/*       SKX (0x1037103E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_KTI_REUT_REG 0x0901103E
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} MINGNT_KTI_REUT_STRUCT;


/* MAXLAT_KTI_REUT_REG supported on:                                          */
/*       SKX_A0 (0x1037103F)                                                  */
/*       SKX (0x1037103F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_KTI_REUT_REG 0x0901103F
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} MAXLAT_KTI_REUT_STRUCT;


/* KTITGTO_KTI_REUT_REG supported on:                                         */
/*       SKX_A0 (0x403710DC)                                                  */
/*       SKX (0x403710DC)                                                     */
/* Register default value:              0x00426800                            */
#define KTITGTO_KTI_REUT_REG 0x090140DC
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Register used by PCode to determine timeout on phy layer training
 */
typedef union {
  struct {
    UINT32 kti_0_disable_timeout : 1;
    /* kti_0_disable_timeout - Bits[0:0], RWS, default = 1'b0 
       Disable timer in kti_physical_layer_timeout.
     */
    UINT32 rsvd_1 : 1;
    UINT32 rsvd_2 : 6;
    /* rsvd_2 - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 kti_physical_layer_timeout : 16;
    /* kti_physical_layer_timeout - Bits[23:8], RWS, default = 16'h4268 
       Value used by PCode to timeout on Intel UPI training. Default value of about 
       17000uS (0x4268) will work for slow boot. And BIOS will adjust the value for 
       full speed initialization. Value defined in micro-seconds (uS) 
       0x0-0xFFFF uSec
     */
    UINT32 rsvd_24 : 4;
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTITGTO_KTI_REUT_STRUCT;


/* KTIREUT_HDR_EXT_KTI_REUT_REG supported on:                                 */
/*       SKX_A0 (0x40371100)                                                  */
/*       SKX (0x40371100)                                                     */
/* Register default value:              0x0000000B                            */
#define KTIREUT_HDR_EXT_KTI_REUT_REG 0x09014100
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT PCIE Header Extended Register
 */
typedef union {
  struct {
    UINT32 pciecapid : 16;
    /* pciecapid - Bits[15:0], RO, default = 16'hB 
       PCIE Extended CAP ID
     */
    UINT32 pciecapversion : 4;
    /* pciecapversion - Bits[19:16], RO, default = 4'b0 
       Capability version
     */
    UINT32 pcienextptr : 12;
    /* pcienextptr - Bits[31:20], RO, default = 12'b0 
       Next Capability offset
     */
  } Bits;
  UINT32 Data;
} KTIREUT_HDR_EXT_KTI_REUT_STRUCT;


/* KTIREUT_HDR_CAP_KTI_REUT_REG supported on:                                 */
/*       SKX_A0 (0x40371104)                                                  */
/*       SKX (0x40371104)                                                     */
/* Register default value:              0x0F000001                            */
#define KTIREUT_HDR_CAP_KTI_REUT_REG 0x09014104
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Header Capability Register
 */
typedef union {
  struct {
    UINT32 vsecid : 16;
    /* vsecid - Bits[15:0], RO, default = 16'h1 
       This field is a Intel-defined ID number that indicates the nature and format of 
       the VSEC structure.  
        '01h' is the ID Council defined for REUT engines. Software must qualify the 
       Vendor ID before interpreting this field. 
     */
    UINT32 vsecidrev : 4;
    /* vsecidrev - Bits[19:16], RO, default = 4'b0 
       This field is defined as the version number that indicates the nature and format 
       of the VSEC structure.  
       	Software must quality the Vendor ID before interpreting this field.
     */
    UINT32 vseclength : 12;
    /* vseclength - Bits[31:20], RO, default = 12'hF0 
       This field defines the length of the REUT 'capability body'. 
       	Currently the size of the body is 184 bytes or 46 DWORDs.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_HDR_CAP_KTI_REUT_STRUCT;


/* KTIREUT_ENG_OVR_KTI_REUT_REG supported on:                                 */
/*       SKX_A0 (0x40371108)                                                  */
/*       SKX (0x40371108)                                                     */
/* Register default value:              0x00400000                            */
#define KTIREUT_ENG_OVR_KTI_REUT_REG 0x09014108
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Engine Overview
 */
typedef union {
  struct {
    UINT32 txrxonmultiplelinks : 1;
    /* txrxonmultiplelinks - Bits[0:0], RO, default = 1'b0 
       1 : REUT Engine can transmit and/or receive on multiple Links simultaneously.
     */
    UINT32 canconrlpartialsellinks : 1;
    /* canconrlpartialsellinks - Bits[1:1], RO, default = 1'b0 
       1 : REUT engine can control any possible subset of multiple Links 
       simultaneously. 
     */
    UINT32 separatesmctrl : 1;
    /* separatesmctrl - Bits[2:2], RO, default = 1'b0 
       Separate PHY state machine control0: Only one LTSSM for each link.
       1: There is more than one LTSSM for each link.
     */
    UINT32 rsvd_3 : 8;
    /* rsvd_3 - Bits[10:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reutengineid : 6;
    /* reutengineid - Bits[16:11], RO, default = 6'b0 
       REUT Engine ID[5:0]
     */
    UINT32 reutspecrev : 8;
    /* reutspecrev - Bits[24:17], RO, default = 8'h20 
       REUT revision[5:0][7:5]: major revision = 001b
       	[4:0]: minor revision = 0 0000b
     */
    UINT32 numlinks : 5;
    /* numlinks - Bits[29:25], RO, default = 5'b0 
       Number of links supported. 
        The number of links supported +1 is the total number of separate Links that 
       this REUT Engine is designed to support. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_ENG_OVR_KTI_REUT_STRUCT;


/* KTIREUT_ENG_LD_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x4037110C)                                                  */
/*       SKX (0x4037110C)                                                     */
/* Register default value:              0x00086273                            */
#define KTIREUT_ENG_LD_KTI_REUT_REG 0x0901410C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Engine Link Description
 */
typedef union {
  struct {
    UINT32 txlinkwidth : 5;
    /* txlinkwidth - Bits[4:0], RO, default = 5'h13 
       Tx Link Width Width of the Link indicated by LinkSelect in REUT-ENG-LS. Actual 
       width is N+1. 13h = 19dec.  
        Actual width is 19 + 1 = 20 lanes. If link is asymmetric then only the RX width 
       is reported here. 
     */
    UINT32 rxlinkwidth : 5;
    /* rxlinkwidth - Bits[9:5], RO, default = 5'h13 
       Rx Link Width Width of the Link indicated by LinkSelect in REUT-ENG-LS. Actual 
       width is N+1. 
        13h = 19dec. Actual width is 19 + 1 = 20 lanes. If link is asymmetric then only 
       the RX width is reported here. 
     */
    UINT32 rsvd_10 : 3;
    /* rsvd_10 - Bits[12:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reutdevid : 8;
    /* reutdevid - Bits[20:13], RO, default = 8'h43 
       Device/Function number. Describes the unique Device/Fn number for the selected 
       Link (indicated by the LinkSelect setting). 
        Device/Fn number is a unique identifier that is component specific that is not 
       repeated for any two Links of within the  component (i.e. every Intel UPI Link 
       must have a unique device number but a Intel UPI link can share a similar device 
       number.  
        Intel UPI port 0 located at: Device8  = 01000b, function3 = 011b. ReutDevID = 
       0100 0011b 
        Intel UPI port 1 located at: Device9  = 01000b, function3 = 011b. ReutDevID = 
       0100 1011b 
        Intel UPI port 2 located at: Device10 = 01010b, function3 = 011b. ReutDevID = 
       0101 0011b 
     */
    UINT32 rsvd_21 : 2;
    /* rsvd_21 - Bits[22:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 linkinactive : 1;
    /* linkinactive - Bits[23:23], RO_V, default = 1'b0 
       Link Inactive: Indicates whether the current Link (indicated by the Link Select) 
       is in use. 
        In general this bit is used to indicate that this link is disabled and no 
       testing can be performed. 
       	Intel UPI Link Behavior: Indicates whether the selected Link is disabled.
     */
    UINT32 linktype : 6;
    /* linktype - Bits[29:24], RO, default = 6'b0 
       Link TypeIndicates the type of Link selected by Link Select.
       000000: Intel UPI
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_ENG_LD_KTI_REUT_STRUCT;


/* KTIREUT_ENG_LSCAP_KTI_REUT_REG supported on:                               */
/*       SKX_A0 (0x40371110)                                                  */
/*       SKX (0x40371110)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_ENG_LSCAP_KTI_REUT_REG 0x09014110
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Link Speed Capability
 */
typedef union {
  struct {
    UINT32 currpllratio : 7;
    /* currpllratio - Bits[6:0], RO, default = 7'b0 
       Current PLL Ratio This is current PLL ratio.
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pllratiomask : 6;
    /* pllratiomask - Bits[13:8], RO, default = 6'b0 
       This indicates the range of options for programming the I/O PLL ratio.
       	Examples:
        001100 - Indicates that only ratio multiples of four and eight can be 
       programmed. 
        010001 - Indicates that only ratio multiples of sixteen and one can be 
       programmed. 
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 minclkratio : 6;
    /* minclkratio - Bits[21:16], RO, default = 6'b0 
       Minimum Clock Ratio This is the minimum value usable for the PLL ratio.
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 maxclkratio : 6;
    /* maxclkratio - Bits[29:24], RO, default = 6'b0 
       Maximum Clock Ratio This is the maximum value usable for the PLL ratio.
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_ENG_LSCAP_KTI_REUT_STRUCT;


/* KTIREUT_ENG_LSCTR_KTI_REUT_REG supported on:                               */
/*       SKX_A0 (0x40371114)                                                  */
/*       SKX (0x40371114)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_ENG_LSCTR_KTI_REUT_REG 0x09014114
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Link Speed Control
 */
typedef union {
  struct {
    UINT32 nextpllratio : 7;
    /* nextpllratio - Bits[6:0], RO, default = 7'b0 
       Next PLL Ratio after link reset
        Allows for controlling the speed of the Link by setting the next PLL ratio to 
       be used after an Inband Reset. 
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_ENG_LSCTR_KTI_REUT_STRUCT;


/* KTIREUT_ENG_LTRCAP_KTI_REUT_REG supported on:                              */
/*       SKX_A0 (0x40371118)                                                  */
/*       SKX (0x40371118)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_ENG_LTRCAP_KTI_REUT_REG 0x09014118
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Engine Link Transmit and Receive Control
 */
typedef union {
  struct {
    UINT32 linkcapability : 8;
    /* linkcapability - Bits[7:0], RO, default = 8'b0 
       Link Capability - Link Capability represents each of the Links indicated by # of 
       Links 
       	Supported by using a one hot it for each of the Links.
       	Intel UPI link Behavior
       	Every even bit represents a Link.
       	[0]: Link 0
       	[1]: Reserved
       	[2]: Link 1
       	[3]: Reserved
       	[4]: Link 2
       	Etc..
       	Odd bits are reserved.
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_ENG_LTRCAP_KTI_REUT_STRUCT;


/* KTIREUT_ENG_LTRCON_KTI_REUT_REG supported on:                              */
/*       SKX_A0 (0x4037111C)                                                  */
/*       SKX (0x4037111C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_ENG_LTRCON_KTI_REUT_REG 0x0901411C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Engine Link Transmit and Receive Configuration
 */
typedef union {
  struct {
    UINT32 linkstocontrol : 16;
    /* linkstocontrol - Bits[15:0], RO, default = 16'b0 
       Link Control Dependency on Link Control is called out explicitly throughout 
       various REUT registers definition when it occurs. 
       	Every even bit in Link Control represents a Link.
       	Bit 0 = Link 0
       	Bit 2 = Link 1
       	Bit 4 = Link 2 Etc.
        The following Rules apply to the Even bits of Link Control. If Can Transmit or 
       Receive on Multiple Links? = 0 then the even Bits of Link Control are not 
       writable and must always be equal to Link Capability. Link Select  must be the 
       sole method to determine which Link is being accessed and controlled. 
        If Can Transmit or Receive on Multiple Links? = 1 then the REUT engine can 
       transmit and/or receive on multiple links based on the following rules. 
        If Can Select a Partial Set Of Link Control is set to 0 then the even Bits of 
       Link Control are not writable and must always be equal to Link Capability  
       	Else,
        If Can Select a Partial Set Of Link Control is set to 1 then the even bits of 
       Link Control can be set to 0 or 1 for any combination of Links which are set to 
       ?1? in the Link Capability. Writing to register dependent on Link Control will 
       cause only the selected Links to be written to while leaving non selected Link 
       register values unchanged. 
       	The following Rules apply to the odd bits of Link Control.
       Intel 	UPI link Behavior: Odd bits are reserved.
     */
    UINT32 rsvd_16 : 7;
    /* rsvd_16 - Bits[22:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 linkautoconfiguration : 1;
    /* linkautoconfiguration - Bits[23:23], RO, default = 1'b0 
       Link Auto Configuration:  
        If Link Auto Configuration Capability is set to 0 then Link Auto Configuration 
       is always reserved as 0.  
       	Else,
        If Link Auto Configuration is set to 0 then all odd Link Control bits that are 
       set to 1 will condition how the Pre Configuration Lane ID values are set.  
        If Link Auto Configuration is set to 1 then all odd Link Control bits are 
       ignored and are assumed to be 0. 
       	See Link Control for more details.
     */
    UINT32 reutdisable : 1;
    /* reutdisable - Bits[24:24], RO, default = 1'b0 
       REUT disable
       	0: REUT enable
       	1: REUT disable
        By writing this bit the whole REUT PCI Function will disappear from the 
       perspective of software (i.e. PCI CSR space). 
        The REUT engine will only be made visible again upon the next Cold Reset or 
       some other implementation specific means. 
        This is intended to act as a security mechanism to prevent any unwanted usage 
       of the REUT registers. 
       	Note: Support for this feature is optional. ( It was RW-O).
     */
    UINT32 linkselect : 4;
    /* linkselect - Bits[28:25], RO, default = 4'b0 
       Link Select -This bit is used in conjunction with many other REUT register bits 
       which are explicitly called out within the description field of the particular 
       bit where it has a potential affect. 
       	Note: We have only one Intel UPI link so this bit field cannot be written.
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_ENG_LTRCON_KTI_REUT_STRUCT;


/* KTIREUT_PH_CPR_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371128)                                                  */
/*       SKX (0x40371128)                                                     */
/* Register default value:              0x00020070                            */
#define KTIREUT_PH_CPR_KTI_REUT_REG 0x09014128
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Physical Layer Capability
 */
typedef union {
  struct {
    UINT32 kti_version : 4;
    /* kti_version - Bits[3:0], RO, default = 4'b0 
       Intel UPI Version
       	0h: Intel UPI v1.0
       	all other encoding are reserved.
     */
    UINT32 ccl0p : 1;
    /* ccl0p - Bits[4:4], RO, default = 1'b1 
       L0p Support
       1:Supports L0p
     */
    UINT32 ccl1 : 1;
    /* ccl1 - Bits[5:5], RO, default = 1'b1 
       L1 Support
       1: supports L1
     */
    UINT32 ccdeterminism : 1;
    /* ccdeterminism - Bits[6:6], RO, default = 1'b1 
       Determinism Support
        'b1 1: supports latency fixing and alignment of exit from init on a clean flit 
       boundary and aligned to external signal. 
     */
    UINT32 cctxadaptsettings : 8;
    /* cctxadaptsettings - Bits[14:7], RO, default = 8'b0 
       Number of Tx adapt settings that can be  tried is one iteration. 
       Note: No plan to support Tx Adapt in processor.
     */
    UINT32 rsvd_15 : 1;
    UINT32 ccfailover : 1;
    /* ccfailover - Bits[16:16], RO_V, default = 1'b0 
       0: not capable of failover on fault.  
       1: capabable of failover on fault
     */
    UINT32 rsvd_17 : 1;
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_CPR_KTI_REUT_STRUCT;


/* KTIREUT_PH_CTR1_KTI_REUT_REG supported on:                                 */
/*       SKX_A0 (0x4037112C)                                                  */
/*       SKX (0x4037112C)                                                     */
/* Register default value:              0x20000020                            */
#define KTIREUT_PH_CTR1_KTI_REUT_REG 0x0901412C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Physical Layer Control 1
 */
typedef union {
  struct {
    UINT32 cp_reset : 1;
    /* cp_reset - Bits[0:0], RWS_LBV, default = 1'b0 
       1: initiates reset (resulting hw actions restore all status RO-V bits to default 
       values); cleared by hardware on entering ResetC 
     */
    UINT32 c_single_step : 1;
    /* c_single_step - Bits[1:1], RWS_LB, default = 1'b0 
       1: enable Single step; forward progress on clearing of SPSSHold.
       Setting this bit also sets SPSSHold.
     */
    UINT32 c_fia : 1;
    /* c_fia - Bits[2:2], RWS_LB, default = 1'b0 
       1: enable Freeze on init abort; backward progress on clearing of SPSSHold
     */
    UINT32 c_ate : 1;
    /* c_ate - Bits[3:3], RWS_LB, default = 1'b0 
       Not supported on processor.
       1: enable ATE mode; forward progress on time-out
     */
    UINT32 c_op_speed : 1;
    /* c_op_speed - Bits[4:4], RWS_LB, default = 1'b0 
       1: operational speed
       0: slow speed
       Note: The operational speed is determined by BIOS setup of the PLL using 
       BIOS-to-PCode Mailbox 
     */
    UINT32 c_init_begin : 1;
    /* c_init_begin - Bits[5:5], RWS_LB, default = 1'b1 
       0: blocks exit from ResetC
     */
    UINT32 c_init : 2;
    /* c_init - Bits[7:6], RWS_LB, default = 2'b0 
       0: Init to L0
       1: CLocalLBM
       2: CLocalComplianceSlave
       3: CLocalComplianceMaster
     */
    UINT32 c_det_en : 1;
    /* c_det_en - Bits[8:8], RWS_LB, default = 1'b0 
       0: no determinism
       1: determinism enabled
     */
    UINT32 c_det_slave : 1;
    /* c_det_slave - Bits[9:9], RWS_LB, default = 1'b0 
       0: Determinism master
       1: Determinism slave
     */
    UINT32 c_reinit_prbs : 2;
    /* c_reinit_prbs - Bits[11:10], RWS_LB, default = 2'b0 
       0: no reinit of PRBS generating LFSRs
       1: reinit after every EIEOS
       2: reserved
       3: reserved
     */
    UINT32 c_la_load_disable : 1;
    /* c_la_load_disable - Bits[12:12], RWS_LB, default = 1'b0 
       1: disable loading of effective values from late action registers in reset
     */
    UINT32 c_numinit : 8;
    /* c_numinit - Bits[20:13], RWS_LB, default = 8'b0 
       0: no limit on consecutive init iterations
       Others: number of init iterations attempted before coming to rest in ResetC.
     */
    UINT32 c_enable_phy_reset : 1;
    /* c_enable_phy_reset - Bits[21:21], RWS_LB, default = 1'b0 
       1: drift alarm initiates a PHY reset; late action
     */
    UINT32 c_l0p_en : 1;
    /* c_l0p_en - Bits[22:22], RWS_LB, default = 1'b0 
       1: enables L0p
     */
    UINT32 c_l1_en : 1;
    /* c_l1_en - Bits[23:23], RWS_LB, default = 1'b0 
       1: enables L1
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[24:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 c_enh_qual_en : 1;
    /* c_enh_qual_en - Bits[25:25], RWS_LB, default = 1'b0 
       1: Enables enhanced lane qualification
     */
    UINT32 c_comp_byp_en : 1;
    /* c_comp_byp_en - Bits[26:26], RWS_LB, default = 1'b0 
       1: Ignore wake detection, adaptation etc prior to bitlock. Used by compliance 
       slave. 
     */
    UINT32 c_failover_en : 1;
    /* c_failover_en - Bits[27:27], RWS_LB, default = 1'b0 
       1: Enable failover
     */
    UINT32 c_enable_phy_reset_w : 1;
    /* c_enable_phy_reset_w - Bits[28:28], RWS_LB, default = 1'b0 
       1: Loss of acquired wake detect during init initiates phy reset
     */
    UINT32 c_rc_terms_off : 1;
    /* c_rc_terms_off - Bits[29:29], RWS_LB, default = 1'b1 
       1: Rx pulldowns are off in ResetC
     */
    UINT32 c_rx_reset_dis : 1;
    /* c_rx_reset_dis - Bits[30:30], RWS_LB, default = 1'b0 
       1: Disables RxReset for DFx purposes
     */
    UINT32 c_lat_fix_en : 1;
    /* c_lat_fix_en - Bits[31:31], RWS_LB, default = 1'b0 
       1: Enables latency fixing
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_CTR1_KTI_REUT_STRUCT;


/* KTIREUT_PH_CTR2_KTI_REUT_REG supported on:                                 */
/*       SKX_A0 (0x40371130)                                                  */
/*       SKX (0x40371130)                                                     */
/* Register default value:              0x00000002                            */
#define KTIREUT_PH_CTR2_KTI_REUT_REG 0x09014130
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Physical Layer Control 2
 */
typedef union {
  struct {
    UINT32 c_tx_adapt_enable : 1;
    /* c_tx_adapt_enable - Bits[0:0], RO, default = 1'b0 
       Enables Tx Adapt Flow
       Note: unsupported in processor
     */
    UINT32 c_tx_adapt_settings : 8;
    /* c_tx_adapt_settings - Bits[8:1], RO, default = 8'h01 
       Number of TX adapt settings that are tried in one iteration
       Note: unsupported in processor
     */
    UINT32 c_tx_adapt_pat : 1;
    /* c_tx_adapt_pat - Bits[9:9], RO, default = 1'b0 
       1: use loopback master pattern based on pat gen mux
       0: use L0 PRBS
       Note: unsupported in processor
     */
    UINT32 c_sb_back_channel : 1;
    /* c_sb_back_channel - Bits[10:10], RO, default = 1'b0 
       0: use slow mode backchannel
       1: use sideband backchannel
       Note: Unsupported in processor.
     */
    UINT32 rsvd : 21;
    /* rsvd - Bits[31:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_CTR2_KTI_REUT_STRUCT;


/* KTIREUT_PH_LCS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371134)                                                  */
/*       SKX (0x40371134)                                                     */
/* Register default value:              0x00001210                            */
#define KTIREUT_PH_LCS_KTI_REUT_REG 0x09014134
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Physical Layer Late Action Current Status
 */
typedef union {
  struct {
    UINT32 s_enable_phy_reset : 1;
    /* s_enable_phy_reset - Bits[0:0], ROS_V, default = 1'b0 
       Updated from CEnablePhyReset on exit from ResetC
     */
    UINT32 rsvd_1 : 2;
    /* rsvd_1 - Bits[2:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 s_include_dropped_lanes : 1;
    /* s_include_dropped_lanes - Bits[3:3], ROS_V, default = 1'b0 
       Updated from CIncludeDroppedLanes on exit from ResetC
     */
    UINT32 s_tl0c : 8;
    /* s_tl0c - Bits[11:4], ROS_V, default = 8'h21 
       L0c interval.  Updated from PHTL0C_SL.t_l0c or PHTL0C_OP.t_l0c depending on slow 
       mode 
     */
    UINT32 s_tl0cq : 4;
    /* s_tl0cq - Bits[15:12], ROS_V, default = 4'h1 
       L0c duration.  Always 1
     */
    UINT32 rsvd_16 : 16;
    /* rsvd_16 - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_LCS_KTI_REUT_STRUCT;


/* KTIREUT_PH_TDC_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371138)                                                  */
/*       SKX (0x40371138)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TDC_KTI_REUT_REG 0x09014138
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Tx Data Lane Control
 */
typedef union {
  struct {
    UINT32 txdatalanedisable : 20;
    /* txdatalanedisable - Bits[19:0], RWS_L, default = 20'b0 
       Tx Data Lane Disable
       	A bit mask used for selectively enabling/disabling data TX Lanes.
       	Used for debug and validation purposes.
       	A bit value of 1 indicates the corresponding lane is disabled. 
       	Bit 0: Controls Lane 0.
       	Bit 1: Controls Lane 1.
       	.. and so on.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TDC_KTI_REUT_STRUCT;


/* KTIREUT_PH_TDS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x4037113C)                                                  */
/*       SKX (0x4037113C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TDS_KTI_REUT_REG 0x0901413C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Tx Data Lane Ready Status
 */
typedef union {
  struct {
    UINT32 txlanedetectstat : 20;
    /* txlanedetectstat - Bits[19:0], RO_V, default = 20'b0 
       Tx Data Lane Detection Status
       	A bit value of 1 indicates the TX lane is dropped.
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
} KTIREUT_PH_TDS_KTI_REUT_STRUCT;


/* KTIREUT_PH_RDS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371140)                                                  */
/*       SKX (0x40371140)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_RDS_KTI_REUT_REG 0x09014140
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Rx Data Lane Ready Status
 */
typedef union {
  struct {
    UINT32 rxdatareadystat : 20;
    /* rxdatareadystat - Bits[19:0], RO_V, default = 20'b0 
       Rx Data Lane Ready Status
       	A bit value of 1 indicates Rx lane is dropped.
       	Bit 0: Status of Lane 0
       	Bit 1: Status of Lane 1.
       	.. and so on.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_RDS_KTI_REUT_STRUCT;


/* KTIREUT_PH_RDC_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371144)                                                  */
/*       SKX (0x40371144)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_RDC_KTI_REUT_REG 0x09014144
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Rx Data Lane Ready Control
 */
typedef union {
  struct {
    UINT32 rxdatalanedisable : 20;
    /* rxdatalanedisable - Bits[19:0], RWS_L, default = 20'b0 
       Rx Data Lane Disable
       	A bit mask used for selectively enabling/disabling data RX Lanes.
       	Used for debug and validation purposes.
       	A bit value of 1 indicates the corresponding RX lane is disabled.
       	Bit 0: Controls RX on Lane 0.
       	Bit 1: Controls RX on Lane 1.
       	.. and so on.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_RDC_KTI_REUT_STRUCT;


/* KTIREUT_PH_CIS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371148)                                                  */
/*       SKX (0x40371148)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_CIS_KTI_REUT_REG 0x09014148
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Physical Layer Initialization Status
 */
typedef union {
  struct {
    UINT32 s_op_speed : 1;
    /* s_op_speed - Bits[0:0], ROS_V, default = 1'b0 
       0: slow speed
       1: op speed
       Updated at entry and exit from ResetC
     */
    UINT32 sp_rx_calib : 1;
    /* sp_rx_calib - Bits[1:1], RW1C, default = 1'b0 
       1: Rx calib done; sw clears by writing 1 prior to speed change to force 
       re-calib. 
     */
    UINT32 sp_detect : 1;
    /* sp_detect - Bits[2:2], RW1C, default = 1'b0 
       1: remote Rx detected by Tx
     */
    UINT32 sp_tx_calib : 1;
    /* sp_tx_calib - Bits[3:3], RW1C, default = 1'b0 
       1: Tx calib done; sw clears by writing 1 prior to speed change to force 
       re-calib. 
     */
    UINT32 s_wake_detected : 1;
    /* s_wake_detected - Bits[4:4], RO_V, default = 1'b0 
       1: Wake detected on one or more lanes.
     */
    UINT32 rsvd_5 : 1;
    /* rsvd_5 - Bits[5:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sp_use_back_channel : 1;
    /* sp_use_back_channel - Bits[6:6], RO, default = 1'b0 
       1: backchannel in use after Tx Adapt done; sw clears by writing 1 after 
       backchannel operations completed. 
       Note: unsupported in processor
     */
    UINT32 sp_tx_adapt : 8;
    /* sp_tx_adapt - Bits[14:7], RO, default = 8'b0 
       Tx Adapt Index
       Note: unsupported in processor
     */
    UINT32 s_link_up : 2;
    /* s_link_up - Bits[16:15], RO_V, default = 2'b0 
       00: STxLinkUp FALSE, SRxLinkUp FALSE
       01: STxLinkUp FALSE, SRxLinkUp TRUE
       10: STxLinkUp TRUE, SRxLinkUp FALSE
       11: STxLinkUp TRUE, SRxLinkUp TRUE
     */
    UINT32 s_init_count : 8;
    /* s_init_count - Bits[24:17], RO_V, default = 8'b0 
       Init iterations count (no rollover)
     */
    UINT32 rsvd_25 : 2;
    /* rsvd_25 - Bits[26:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 s_remote_lbm : 1;
    /* s_remote_lbm - Bits[27:27], RO_V, default = 1'b0 
       Set if received TS destination state is loopback master.
     */
    UINT32 s_rx_lane_rev : 1;
    /* s_rx_lane_rev - Bits[28:28], RO_V, default = 1'b0 
       1: lane reversal at Rx
     */
    UINT32 s_pss_hold : 1;
    /* s_pss_hold - Bits[29:29], RW1C, default = 1'b0 
       State Machine Hold
       1 - Physical layer state machine is holding on single step or freezing on 
       initialization abort.   
       Clearing this bit will cause state machine to advance. sw must also clear this 
       bit when CSingleStep or CFIA is cleared. 
     */
    UINT32 s_init_fail : 1;
    /* s_init_fail - Bits[30:30], RO_V, default = 1'b0 
       0: init progress
       1: secondary timeout and init fail
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_CIS_KTI_REUT_STRUCT;


/* KTIREUT_PH_PIS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x4037114C)                                                  */
/*       SKX (0x4037114C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_PIS_KTI_REUT_REG 0x0901414C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Layer Previous Init Status
 */
typedef union {
  struct {
    UINT32 s_op_speed_p : 1;
    /* s_op_speed_p - Bits[0:0], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 sp_rx_calib_p : 1;
    /* sp_rx_calib_p - Bits[1:1], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 sp_detect_p : 1;
    /* sp_detect_p - Bits[2:2], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 sp_tx_calib_p : 1;
    /* sp_tx_calib_p - Bits[3:3], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 s_wake_detected_p : 1;
    /* s_wake_detected_p - Bits[4:4], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 rsvd_5 : 1;
    /* rsvd_5 - Bits[5:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sp_use_back_channel_p : 1;
    /* sp_use_back_channel_p - Bits[6:6], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 sp_tx_adapt_p : 8;
    /* sp_tx_adapt_p - Bits[14:7], RW1CS, default = 8'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 s_link_up_p : 2;
    /* s_link_up_p - Bits[16:15], RW1CS, default = 2'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 s_init_count_p : 8;
    /* s_init_count_p - Bits[24:17], RW1CS, default = 8'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 rsvd_25 : 2;
    /* rsvd_25 - Bits[26:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 s_remote_lbm_p : 1;
    /* s_remote_lbm_p - Bits[27:27], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 s_rx_lane_rev_p : 1;
    /* s_rx_lane_rev_p - Bits[28:28], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 s_pss_hold_p : 1;
    /* s_pss_hold_p - Bits[29:29], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 s_init_fail_p : 1;
    /* s_init_fail_p - Bits[30:30], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_CIS register, but this is copy of 
       prior to last init abort. 
     */
    UINT32 rsvd_31 : 1;
    /* rsvd_31 - Bits[31:31], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_PIS_KTI_REUT_STRUCT;


/* KTIREUT_PH_CSS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371150)                                                  */
/*       SKX (0x40371150)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_CSS_KTI_REUT_REG 0x09014150
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Layer Current State Status Register
 */
typedef union {
  struct {
    UINT32 s_tx_state : 8;
    /* s_tx_state - Bits[7:0], RO_V, default = 8'b0 
       Tx State (encoding defined in Intel UPI specification)
     */
    UINT32 s_rx_state : 8;
    /* s_rx_state - Bits[15:8], RO_V, default = 8'b0 
       Rx State (encoding defined in Intel UPI specification)
     */
    UINT32 s_clm : 3;
    /* s_clm - Bits[18:16], RO_V, default = 3'b0 
       lane map code determined at Rx and sent in TX.Config for remote Tx; also updated 
       on L0p entry/exit. Encoded as follows: 
       0b111 - lanes 0..19
       0b001 - lanes 0..7
       0b100 - lanes 12..19
     */
    UINT32 rsvd : 13;
    /* rsvd - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_CSS_KTI_REUT_STRUCT;


/* KTIREUT_PH_PSS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371154)                                                  */
/*       SKX (0x40371154)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_PSS_KTI_REUT_REG 0x09014154
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Layer Previous State Status Register
 */
typedef union {
  struct {
    UINT32 s_tx_state_p : 8;
    /* s_tx_state_p - Bits[7:0], RW1CS, default = 8'b0 
       Same description as bit field in KTIREUT_PH_CSS register, but this is copy of 
       prior to last phy reset 
     */
    UINT32 s_rx_state_p : 8;
    /* s_rx_state_p - Bits[15:8], RW1CS, default = 8'b0 
       Same description as bit field in KTIREUT_PH_CSS register, but this is copy of 
       prior to last phy reset 
     */
    UINT32 s_clm_p : 3;
    /* s_clm_p - Bits[18:16], RW1CS, default = 3'b0 
       Same description as bit field in KTIREUT_PH_CSS register, but this is copy of 
       prior to last phy reset 
     */
    UINT32 rsvd : 13;
    /* rsvd - Bits[31:19], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_PSS_KTI_REUT_STRUCT;


/* KTIREUT_PH_YIS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371158)                                                  */
/*       SKX (0x40371158)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_YIS_KTI_REUT_REG 0x09014158
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Rx Data Lane Polarity Inversion Status
 */
typedef union {
  struct {
    UINT32 rx_pol_inv : 20;
    /* rx_pol_inv - Bits[19:0], RO_V, default = 20'b0 
       A bit value of 1 indicates Rx lane has detected polarity inversion.
       Bit 0: Status of Physical Lane 0
       Bit 1: Status of Physical Lane 1.
       .. and so on.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_YIS_KTI_REUT_STRUCT;


/* KTIREUT_PH_DIS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x4037115C)                                                  */
/*       SKX (0x4037115C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_DIS_KTI_REUT_REG 0x0901415C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Determinism Init Status
 */
typedef union {
  struct {
    UINT32 s_lat_added : 8;
    /* s_lat_added - Bits[7:0], RO_V, default = 8'b0 
       8:7 - Reserved
       6:0 - Latency added after deskew div 8 UI, i.e., 8
       UI granularity
     */
    UINT32 s_rx_obs_latency : 12;
    /* s_rx_obs_latency - Bits[19:8], RO_V, default = 12'b0 
       Rx sync counter value  when EIEOS of TS.Polling received at latency buffer.
     */
    UINT32 rsvd_20 : 4;
    /* rsvd_20 - Bits[23:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 s_lat_buff_error : 1;
    /* s_lat_buff_error - Bits[24:24], RO_V, default = 1'b0 
       1 - latency not fixable i.e. fix value larger  than latency buffer depth; init 
       continues without latency fixing. 
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_DIS_KTI_REUT_STRUCT;


/* KTIREUT_PH_CLS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371160)                                                  */
/*       SKX (0x40371160)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_CLS_KTI_REUT_REG 0x09014160
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Physical Link Status
 */
typedef union {
  struct {
    UINT32 s_l1r_err : 1;
    /* s_l1r_err - Bits[0:0], RW1C, default = 1'b0 
       L1r received at wrong time
     */
    UINT32 rsvd_1 : 1;
    /* rsvd_1 - Bits[1:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sp_drift_alarm : 1;
    /* sp_drift_alarm - Bits[2:2], RW1C, default = 1'b0 
       1: Indicates drift buffer underflow or overflow alarm
     */
    UINT32 s_drift_alarm_lane_address : 5;
    /* s_drift_alarm_lane_address - Bits[7:3], RO_V, default = 5'b0 
       The lane ID of first lane that caused Drift Buffer Alarm to set. Valid only when 
       alarm is set. 
     */
    UINT32 s_l0pe_fail : 1;
    /* s_l0pe_fail - Bits[8:8], RO_V, default = 1'b0 
       1: L0p exit failed at Rx
     */
    UINT32 s_l0pe_lat_sub : 1;
    /* s_l0pe_lat_sub - Bits[9:9], RO_V, default = 1'b0 
       1: Latency subtracted on at least one idle lane during L0p exit
     */
    UINT32 s_l0pe_lat_add : 1;
    /* s_l0pe_lat_add - Bits[10:10], RO_V, default = 1'b0 
       1: Latency added on at least one idle lane during L0p exit
     */
    UINT32 s_l1r_out : 1;
    /* s_l1r_out - Bits[11:11], RO_V, default = 1'b0 
       Set when QL1r sent; cleared when QL1n received
     */
    UINT32 s_l1n_pend : 1;
    /* s_l1n_pend - Bits[12:12], RO_V, default = 1'b0 
       Set when QL1r received; cleared when QL1n sent
     */
    UINT32 s_rst_rcvd : 1;
    /* s_rst_rcvd - Bits[13:13], RO_V, default = 1'b0 
       1: QRst code received
     */
    UINT32 s_mm_l0c : 8;
    /* s_mm_l0c - Bits[21:14], RO_V, default = 8'b0 
       Mismatched pattern by lane when L0c mismatch causes a reset. will be b'1 for bad 
       lane, b'0 for good lane 
     */
    UINT32 s_sent_l0c : 4;
    /* s_sent_l0c - Bits[25:22], RO_V, default = 4'b0 
       Sent L0c code
     */
    UINT32 s_rcvd_l0c : 4;
    /* s_rcvd_l0c - Bits[29:26], RO_V, default = 4'b0 
       Received L0c code
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_CLS_KTI_REUT_STRUCT;


/* KTIREUT_PH_PLS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371164)                                                  */
/*       SKX (0x40371164)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_PLS_KTI_REUT_REG 0x09014164
/* Struct format extracted from XML file SKX_A0\3.14.1.CFG.xml.
 * REUT Physical Link Status Previous
 */
typedef union {
  struct {
    UINT32 s_l1r_err_p : 1;
    /* s_l1r_err_p - Bits[0:0], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[1:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sp_drift_alarm_p : 1;
    /* sp_drift_alarm_p - Bits[2:2], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_drift_alarm_lane_address_p : 5;
    /* s_drift_alarm_lane_address_p - Bits[7:3], RW1CS, default = 5'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l0pe_fail_p : 1;
    /* s_l0pe_fail_p - Bits[8:8], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l0pe_lat_sub_p : 1;
    /* s_l0pe_lat_sub_p - Bits[9:9], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l0pe_lat_add_p : 1;
    /* s_l0pe_lat_add_p - Bits[10:10], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l1r_out_p : 1;
    /* s_l1r_out_p - Bits[11:11], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l1n_pend_p : 1;
    /* s_l1n_pend_p - Bits[12:12], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_rst_rcvd_p : 1;
    /* s_rst_rcvd_p - Bits[13:13], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset 
     */
    UINT32 s_mm_l0c_p : 8;
    /* s_mm_l0c_p - Bits[21:14], RW1CS, default = 8'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_sent_l0c_p : 5;
    /* s_sent_l0c_p - Bits[26:22], RW1CS, default = 5'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_rcvd_l0c_p : 5;
    /* s_rcvd_l0c_p - Bits[31:27], RW1CS, default = 5'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_PLS_KTI_REUT_A0_STRUCT;

/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Physical Link Status Previous
 */
typedef union {
  struct {
    UINT32 s_l1r_err_p : 1;
    /* s_l1r_err_p - Bits[0:0], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 rsvd_1 : 1;
    /* rsvd_1 - Bits[1:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sp_drift_alarm_p : 1;
    /* sp_drift_alarm_p - Bits[2:2], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_drift_alarm_lane_address_p : 5;
    /* s_drift_alarm_lane_address_p - Bits[7:3], RW1CS, default = 5'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l0pe_fail_p : 1;
    /* s_l0pe_fail_p - Bits[8:8], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l0pe_lat_sub_p : 1;
    /* s_l0pe_lat_sub_p - Bits[9:9], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l0pe_lat_add_p : 1;
    /* s_l0pe_lat_add_p - Bits[10:10], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l1r_out_p : 1;
    /* s_l1r_out_p - Bits[11:11], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_l1n_pend_p : 1;
    /* s_l1n_pend_p - Bits[12:12], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_rst_rcvd_p : 1;
    /* s_rst_rcvd_p - Bits[13:13], RW1CS, default = 1'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset 
     */
    UINT32 s_mm_l0c_p : 8;
    /* s_mm_l0c_p - Bits[21:14], RW1CS, default = 8'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_sent_l0c_p : 4;
    /* s_sent_l0c_p - Bits[25:22], RW1CS, default = 4'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 s_rcvd_l0c_p : 4;
    /* s_rcvd_l0c_p - Bits[29:26], RW1CS, default = 4'b0 
       Same description as bit field in KTIREUT_PH_PLS register, but this is copy of 
       prior to last phy reset. 
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_PLS_KTI_REUT_STRUCT;



/* KTIREUT_PH_WES_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371168)                                                  */
/*       SKX (0x40371168)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_WES_KTI_REUT_REG 0x09014168
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Rx Data Lane Wake Error Status
 */
typedef union {
  struct {
    UINT32 s_rx_lane_wake_err : 20;
    /* s_rx_lane_wake_err - Bits[19:0], RO_V, default = 20'b0 
       A bit value of 1 indicates Rx lane wake detect failed.
       Bit 0: Status of Physical Lane 0
       Bit 1: Status of Physical Lane 1
       ... and so on
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_WES_KTI_REUT_STRUCT;


/* KTIREUT_PH_KES_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x4037116C)                                                  */
/*       SKX (0x4037116C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_KES_KTI_REUT_REG 0x0901416C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Rx Data Lane Deskew Error Status
 */
typedef union {
  struct {
    UINT32 s_rx_lane_deskew_err : 20;
    /* s_rx_lane_deskew_err - Bits[19:0], RO_V, default = 20'b0 
       A bit value of 1 indicates Rx lane deskew failed.
       Bit 0: Status of Physical Lane 0
       Bit 1: Status of Physical Lane 1
       ... and so on
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_KES_KTI_REUT_STRUCT;


/* KTIREUT_PH_DAS_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371170)                                                  */
/*       SKX (0x40371170)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_DAS_KTI_REUT_REG 0x09014170
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Rx Data Lane Drift Alarm Status
 */
typedef union {
  struct {
    UINT32 s_rx_lane_drift_alarm : 20;
    /* s_rx_lane_drift_alarm - Bits[19:0], RO_V, default = 20'b0 
       A bit value of 1 indicates Rx lane drift alarm.
       Bit 0: Status of Physical Lane 0
       Bit 1: Status of Physical Lane 1
       ... and so on
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_DAS_KTI_REUT_STRUCT;


/* KTIREUT_PH_TES_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371174)                                                  */
/*       SKX (0x40371174)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TES_KTI_REUT_REG 0x09014174
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Phy Best Rx Metric Memory Status
 * Note: this register is unimplemented as tx adaptation is not supported.
 */
typedef union {
  struct {
    UINT32 s_opt_index : 16;
    /* s_opt_index - Bits[15:0], RO_V, default = 16'b0 
       Optimal Tx Adapt index for lane addressed in PH_TEC
     */
    UINT32 s_al_metric : 8;
    /* s_al_metric - Bits[23:16], RO_V, default = 8'b0 
       Rx metric for addressed lane
     */
    UINT32 s_al_metric_max : 8;
    /* s_al_metric_max - Bits[31:24], RO_V, default = 8'b0 
       Rx metric max for addressed lane (for normalization); 0 means metric not 
       implemented. 
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TES_KTI_REUT_STRUCT;


/* KTIREUT_PH_LDC_KTI_REUT_REG supported on:                                  */
/*       SKX_A0 (0x40371178)                                                  */
/*       SKX (0x40371178)                                                     */
/* Register default value:              0x00000804                            */
#define KTIREUT_PH_LDC_KTI_REUT_REG 0x09014178
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Physical Determinism Control
 */
typedef union {
  struct {
    UINT32 driftbufalarmthr : 6;
    /* driftbufalarmthr - Bits[5:0], RWS_L, default = 6'h4 
       Drift Buffer Alarm Threshold
        When the difference (both underflow and overflow) between drift buffer read and 
       write pointers (depth)  is less than the value in this field, drift buffer alarm 
       status will be set. 
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 inidriftbufflatency : 6;
    /* inidriftbufflatency - Bits[13:8], RWS_L, default = 6'h8 
       Initial Drift Buffer Latency
        Drift Buffer Depth refers to the initialized difference between read and writer 
       pointers in RX drift buffer. 
        The field indicates the difference between the drift buffer read and write 
       pointers in UI, to be set during Polling.BitLock, in all lanes. 
       
     */
    UINT32 targetlinklatency : 9;
    /* targetlinklatency - Bits[22:14], RWS_L, default = 9'b0 
       Target latency at Rx in 8 UI granularity
     */
    UINT32 rsvd_23 : 9;
    /* rsvd_23 - Bits[31:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_LDC_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTCAP_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x40371188)                                                  */
/*       SKX (0x40371188)                                                     */
/* Register default value:              0x04A40000                            */
#define KTIREUT_PH_TTCAP_KTI_REUT_REG 0x09014188
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Test Pattern Capability Register
 */
typedef union {
  struct {
    UINT32 test_exe_control_cap : 1;
    /* test_exe_control_cap - Bits[0:0], RO, default = 1'b0 
       If 0, this field indicates that the KTIREUT_PH_TTPGC register is used for the 
       Start Test and Stop Test control bits. 
        If 1, this field indicates that the KTIREUT_PH_TTTEC register is used for the 
       Start Test and Stop Test control bits. 
       
     */
    UINT32 test_exe_delay_cap : 1;
    /* test_exe_delay_cap - Bits[1:1], RO, default = 1'b0 
       If set, this field indicates that the Test Execution Delay register is available 
       to set the delay between  setting Start Test and the actual start of the test. 
       This bit MUST be a 1 when the Start Coordination Capability is 1. 
       	0 - Test Execution Delay register (KTIREUT_PH_TTTED) is not implemented
       	1 - Test Execution Delay register is implemente
       
     */
    UINT32 test_exe_status : 1;
    /* test_exe_status - Bits[2:2], RO, default = 1'b0 
       If set, this field indicates that the Test Execution Status register is 
       implemented. 
       	0 - Test Execution Status register (KTIREUT_PH_TTTES) is not implemented
       	1 - Test Execution Status register is implemented
     */
    UINT32 selerrmasktype : 1;
    /* selerrmasktype - Bits[3:3], RO, default = 1'b0 
       1 - Symbol/Bit and Masking Modes Single bit checks are specified in terms of a 
       pattern buffer symbol number and the bit position within that symbol. For 
       interval testing, the mask logic provides a bit mask that is always aligned to 
       the receive interpolator for isolation. 
       	0 - Start_bit/Interval mode 
        Single bit checks are specified in terms of the pattern buffer bit to check. 
       Interval checking specifies the starting pattern buffer bit and the interval 
       between bit checks. 
     */
    UINT32 rsvd_4 : 1;
    /* rsvd_4 - Bits[4:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 slave_ib_para_extract : 1;
    /* slave_ib_para_extract - Bits[5:5], RO, default = 1'b0 
       If set, slave is capable of extracting electrical parameter from master's 
       TS.Loopback and applying during the test. 
     */
    UINT32 startcoordcap : 1;
    /* startcoordcap - Bits[6:6], RO, default = 1'b0 
       Start Coordination Capability
       If set, this field indicates that the Start Test field has the ability to be 
       further qualified by an implementation specific control signal from a component 
       wide Global Event. 
     */
    UINT32 txerrorinjcap : 1;
    /* txerrorinjcap - Bits[7:7], RO, default = 1'b0 
       Only valid if Selective Error Mask Type is 1. If set, this field indicates that 
       the transmit side of the Loopback Master can inject bit errors. 
     */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 flatpatbufcap : 1;
    /* flatpatbufcap - Bits[9:9], RO, default = 1'b0 
       If set, this field indicates that the Pattern Buffer Access registers are not 
       indexed, but are flattened. 
     */
    UINT32 rsvd_10 : 6;
    /* rsvd_10 - Bits[15:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 loop_cnt_inhibit_cap : 1;
    /* loop_cnt_inhibit_cap - Bits[16:16], RO, default = 1'b0 
       If set, this field indicates that the Loop Counter Inhibit is implemented and 
       the user can stall the Loop Counter. 
       	0 - Loop Counter Inhibit is not implemented
       	1 - Loop Counter Inhibit is implemented
     */
    UINT32 rsvd_17 : 1;
    /* rsvd_17 - Bits[17:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lfsrstaggercap : 1;
    /* lfsrstaggercap - Bits[18:18], RO, default = 1'b1 
       If set, this field indicates that the LFSR1 Stagger Enable and LFSR2 Stagger 
       Enable fields are implemented and the user can choose between staggereing of 
       each LFSR or not. 
       	1 - LRSR1 Stagger Enable and LFSR2 Stagger Enable fields exist
       	0 - LFSR1 Stagger Enable and LFSR2 Stagger Enable fields are missing   
     */
    UINT32 dcpolarityselcap : 1;
    /* dcpolarityselcap - Bits[19:19], RO, default = 1'b0 
       If set, this field indicates that the Drive DC Zero or DC One field is 
       implemented and the user can choose between driving a 0 or a 1 on those lanes 
       set to drive a DC value in. 
       	1 - Drive DC Zero or DC One field exists
       	0 - Drive DC Zero or DC One field is missing 
       
       To force 0 on lanes during PATTERN use PQ_CS_IBTXLER.tx_lane_en [19:0]
       To force 1 on lanes during  PATTERN use PH_TTDDC.txdatlanctr[19:0]
     */
    UINT32 rsvd_20 : 1;
    /* rsvd_20 - Bits[20:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 err_cnt_inhibit : 1;
    /* err_cnt_inhibit - Bits[21:21], RO, default = 1'b1 
       If set, design is capable of inhibing error counters.
     */
    UINT32 polyselectcap : 1;
    /* polyselectcap - Bits[22:22], RO, default = 1'b0 
       If set, this field indicates that the LFSR Polynomial Select field is 
       implemented and the user can choose between the two LFSR polynomials. 
        1 - LFSR Polynomial can be selected between the ITU polynomial and the 
       historical Intel UPI polynomial (reciprocal of ITU polynomial). 
       	0 - Only the spec polynomial is available
     */
    UINT32 patbufselrotcap : 1;
    /* patbufselrotcap - Bits[23:23], RO, default = 1'b1 
       If set, user can enable rotation of the KTIREUT_PH_TTPG2S register selects 
       during testing. 
       	1 - Buffer Select can rotate
       	0 - Buffer Select is static
     */
    UINT32 rsvd_24 : 2;
    /* rsvd_24 - Bits[25:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ext_err_cnt_lane_reassign_cap : 1;
    /* ext_err_cnt_lane_reassign_cap - Bits[26:26], RO, default = 1'b1 
       This field indicates that the Extended Error Counter Lane Assign field is 
       implemented and the user can update the assigned lane for the Extended Error 
       Counters(s). 
       	0 - Feature not implemented
       	1 - Extended Error Counter can be reassigned
     */
    UINT32 extcntalllanescap : 1;
    /* extcntalllanescap - Bits[27:27], RO, default = 1'b0 
       If set, this field indicates that the Extended Error Count All Lanes field is 
       implemented and the user can enable counting of all bit errors for the complete 
       set of lanes for the tested lanes, starting from the lane selected in Extended 
       Error Counter Lane Select and including any consecutive higher lanes included in 
       the testing. 
       	1 - Extended Error Counter can accumulate all errors
       	0 - Feature not implemented
     */
    UINT32 numextcntcap : 4;
    /* numextcntcap - Bits[31:28], RO, default = 4'b0 
       This field is used to enumerate the total number of Extended Error Counters. 
       Each will have a Each will have a PH_TTPCC and PH_TTLECR. If the quantity of 
       Extended Error Counters is other than 1 then that number is recorded in this 
       field as a N-1 value. 
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTCAP_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTPGC_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x4037118C)                                                  */
/*       SKX (0x4037118C)                                                     */
/* Register default value:              0x06060000                            */
#define KTIREUT_PH_TTPGC_KTI_REUT_REG 0x0901418C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Pattern Generator Control
 */
typedef union {
  struct {
    UINT32 strttst : 1;
    /* strttst - Bits[0:0], RW1SS, default = 1'b0 
       Start test
       KTIRUET_PH_TTCAP(0) is 0, test starts if this bit is set.
        1 - Start the test - Initiates transition from Loopback.Marker to 
       Loopback.Pattern. 
       	The bit is cleared when Loopback.Pattern is exited.
     */
    UINT32 stponerr : 1;
    /* stponerr - Bits[1:1], RWS_L, default = 1'b0 
       Stop test on Error
       	Exit Loopback.Pattern to Loopback.Marker on error - Flag
       	0 - Do not stop test on error
       	1 - Stop test and latch Loop Status on first error in pattern checker.
     */
    UINT32 autoinv : 1;
    /* autoinv - Bits[2:2], RWS_L, default = 1'b0 
       Auto inversion rotate enable
        If set, this bit will enable logical rotation of the KTIREUT_PH_TTPIS register 
       during the test. 
        Each lane begins by selecting the appropriate bit in the Pattern Invert Select 
       field, and sequentially the  next higher bit, wrapping back to bit 0, following 
       each loop. 
       	1 - Logical rotation of Buffer Invert Select
       	0 - No logical rotation of Buffer INvert Select
     */
    UINT32 psuedorandomen1 : 1;
    /* psuedorandomen1 - Bits[3:3], RWS_L, default = 1'b0 
       Pseudo-random Enable1
       	Enable Pseudo random pattern generation in Pattern Generator Buffer 1
       	0 - Use Pattern Buffer 1 as 128 bit pattern.
       	1 - Use Pattern Buffer 1 XORed with the LFSR.
     */
    UINT32 psuedorandomen2 : 1;
    /* psuedorandomen2 - Bits[4:4], RWS_L, default = 1'b0 
       Pseudo-random Enable2
       	Enable Pseudo random pattern generation in Pattern Generator Buffer 2
       	0 - Use Pattern Buffer 2 as 128 bit pattern.
       	1 - Use Pattern Buffer 2 XORed with the LFSR.
     */
    UINT32 slaveignorremoteparam : 1;
    /* slaveignorremoteparam - Bits[5:5], RWS_L, default = 1'b0 
       Disable application of electrical parameters.  
       If set override electrical parameters will not be applied applied in 
       Loopback.Pattern. 
     */
    UINT32 rsvd_6 : 1;
    UINT32 incldroppedlanes : 1;
    /* incldroppedlanes - Bits[7:7], RWS_L, default = 1'b0 
       If set, include Tx lanes marked TxLaneDropped and Rx lanes marked RxLaneDropped 
       in Loopback.Pattern with deskew setting of 0. Late action. 
     */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pattbuffselect : 5;
    /* pattbuffselect - Bits[13:9], RWS_L, default = 2'b0 
       Pattern buffer selection used to index pattern buffers through the PH_TTPGB 
       register (see PH_TTPGB: Pattern Generator Buffer Access Register). Since 
       PH_TTPGB is a through register, target registers must be provided for each 
       address below. 
       x00-x03 - Access pattern buffer 1 LS-Dword (lsdw) to MS-Dword (msfw)
       x04-x07 - Access pattern buffer 2 LD-Dword to MSDword.
       x08 - Access Loopback LFSR1 seed
       x09- Access Loopback LFSR3 seed
       x0a Access Loopback LFSR5 seed
       x0b- Access Loopback LFSR7 seed
       x0c-x1f: reserved
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 loop_cnt_inhibit : 1;
    /* loop_cnt_inhibit - Bits[16:16], ROS_V, default = 1'b0 
       Valid only if the Loop Counter Inhibit Capability is 1. This field may be used 
       to inhibit the counting in the  Loop Counter when it is set. Normally this would 
       be used when setting Error Counter Inhibit, so there is  an accurate reading of 
       the Loop Counter Status.  
       	0 - Normal counting of pattern check loops
       	1 - Inhibit counting of pattern check loops
     */
    UINT32 lfsr1_stagger_en : 1;
    /* lfsr1_stagger_en - Bits[17:17], ROS_V, default = 1'b1 
       Selects a staggered scrambling pattern to be generated when LFSR1 is selected 
       for a pseudorandom  pattern. 
       	0 - Do not stagger the LFSR1 pattern(not supported)
       	1 - Enable staggering of the LFSR1 pattern(default)
     */
    UINT32 lfsr2_stagger_en : 1;
    /* lfsr2_stagger_en - Bits[18:18], ROS_V, default = 1'b1 
       Selects a staggered scrambling pattern to be generated when LFSR2 is selected 
       for a pseudorandom  pattern. 
       	0 - Do not stagger the LFSR2 pattern(not supported)
       	1 - Enable staggering of the LFSR2 pattern(default)
     */
    UINT32 drive0or1 : 1;
    /* drive0or1 - Bits[19:19], ROS_V, default = 1'b0 
       Drive DC Zero Instead of One
        Selects the DC Logic Value to use when KTIREUT_PH_TTDDC selects a DC pattern 
       override. 
       	0 - Drive a logic 1 for all lanes selected in KTIREUT_PH_TTDDC
       	1 - Drive a logic 0 for all lanes selected in KTIREUT_PH_TTDDC
     */
    UINT32 stoptest : 1;
    /* stoptest - Bits[20:20], RW1SS, default = 1'b0 
       if KTIREUT_PH_TTCAP(0) is 0, setting this bit stops the test. 
       	1 - Stops the test - forces an exit from Loopback.Pattern.
       	The bit is cleared when Loopback.Pattern is exited
     */
    UINT32 errcntstall : 1;
    /* errcntstall - Bits[21:21], RWS_L, default = 1'b0 
       Error Counters Inhibit
       	Inhibits the counting of errors when in Loopback.Pattern.
       	0 - Normal Error counting
       	1 - Inhibit Error counting
     */
    UINT32 lfsrpolynomialsel : 1;
    /* lfsrpolynomialsel - Bits[22:22], ROS_V, default = 1'b0 
       Polynomial Select
       	0 - Use the spec polynomial
       1- RSVD
     */
    UINT32 autobufselenable : 1;
    /* autobufselenable - Bits[23:23], RWS_L, default = 1'b0 
       Rotate Pattern Buffer Selection
        If set, this bit will enable rotation of the KTIREUT_PH_TTPG2S register during 
       the test. The actual register is not 
        rotated, but an operating copy is made at the beginning of the test, and then 
       the copy is rotated 
       	across all lanes of the interface.
       	0 - Rotation is disabled
       	1 - Rotation is enabled
     */
    UINT32 exploopcnt : 5;
    /* exploopcnt - Bits[28:24], RWS_L, default = 5'h6 
       Exponential Loop Count
       	Exponential Test Loop Count.
       	Loop count is 2^(count in this field)
        Each increment in loop counter indicates 128 UI of Pattern Buffer Register 
       content. 
       	0 - Infinite count. Use Soft Reset to exit test.
     */
    UINT32 rsvd_29 : 3;
    /* rsvd_29 - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTPGC_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTTEC_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x40371190)                                                  */
/*       SKX (0x40371190)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTTEC_KTI_REUT_REG 0x09014190
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Test Execution Control
 * Register not supported.
 */
typedef union {
  struct {
    UINT32 start_test : 1;
    /* start_test - Bits[0:0], RO, default = 1'b0 
       Test started on change from 0 to 1 (edge detect) if in Compliance or 
       Loopback.Marker. 
       Note: Writing Start Test from a 0 to 1 while also writing Stop Test to a 1 will 
       result in no test being executed. Clearing Start Test while a test is in process 
       will corrupt (clear) the test progress status bits in the KTIREUT_PH_TTTES 
       register. Start Test should only be cleared after all test results have been 
       examined. 
       Register not supported in processor.
     */
    UINT32 stop_test : 1;
    /* stop_test - Bits[1:1], RO, default = 1'b0 
       Test stopped on change from 0 to 1 (edge detect) if in Loopback.Pattern.
       Register not supported in processor.
     */
    UINT32 start_delay_sel_index : 2;
    /* start_delay_sel_index - Bits[3:2], RO, default = 2'b0 
       Start Delay Select Index into PH_TTTED
       Selects the execution delay in PH_TTTED.
       Register not supported in processor.
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTTEC_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTTED_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x40371194)                                                  */
/*       SKX (0x40371194)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTTED_KTI_REUT_REG 0x09014194
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Test Execution Delay
 * Register not supported.
 */
typedef union {
  struct {
    UINT32 t_test_delay_ctrl0 : 16;
    /* t_test_delay_ctrl0 - Bits[15:0], RO, default = 1'b0 
       Test execution delay Value number 0.
       Following the setting of the Start Test bit, the selected delay value as indexed 
       in Start Delay Select is used to delay the actual start of the test by the 
       programmed number of TSL. If Start Coordination is set then the timer will not 
       begin until the input trigger has happened. 
       Register not supported in processor.
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTTED_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTPCC_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x40371198)                                                  */
/*       SKX (0x40371198)                                                     */
/* Register default value:              0x04000002                            */
#define KTIREUT_PH_TTPCC_KTI_REUT_REG 0x09014198
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Pattern Checker Control
 */
typedef union {
  struct {
    UINT32 init_check_mask : 4;
    /* init_check_mask - Bits[3:0], RWS_L, default = 4'h2 
       InitialCheckMask
        The Initial Check Mask is used for a settling time for any Electrical Parameter 
       offsets that are applied upon  entry to Loopback.Pattern and for settling time 
       for any Electrical Parameter Global values applied upon re-entry to 
       Loopback.Marker.  
        Upon entry to Loopback.Pattern, no RX lane errors will be logged during the 
       Initial Check Mask period. 
       	Initial check mask period is 2^(Count in this field) * 1TSL.
       	0 - Indicates no masking.
     */
    UINT32 rsvd_4 : 1;
    /* rsvd_4 - Bits[4:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 sel_err_check_int : 3;
    /* sel_err_check_int - Bits[7:5], RWS_L, default = 3'b0 
       Selective Error Check Interval = 2^Count in this Field.
       Checks error for 1UI at every Error Check Interval starting from Error Check 
       Start. 
       
     */
    UINT32 sel_err_check_start : 7;
    /* sel_err_check_start - Bits[14:8], RWS_L, default = 7'h0 
       The initial UI in the pattern buffer to start checking errors in the arriving 
       pattern 
     */
    UINT32 rsvd_15 : 1;
    /* rsvd_15 - Bits[15:15], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rx_err_counter_lane_sel : 5;
    /* rx_err_counter_lane_sel - Bits[20:16], RWS_L, default = 5'b0 
       The value of RX Error Counter Lane Select is used to indicate which Lane will 
       have the full 32 bit error counter and the other Lanes will have only 8 bit 
       error counters that are observable using RX Error Counter Lane Select. 
       	0: Rx Lane 0 Selected.
       	1: Rx Lane 1 Selected.
       	Etc..
     */
    UINT32 rsvd_21 : 5;
    /* rsvd_21 - Bits[25:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ext_err_ctr_lane_assign : 1;
    /* ext_err_ctr_lane_assign - Bits[26:26], ROS_V, default = 1'b1 
       This field is valid only if Extended Count Lane Assign Capability is set. This 
       bit is used to force a reassignment of the 31-bit error counter. Entry into 
       Loopback.Pattern continues to also assign the long error counter lane. 
        1 - Use the current RxErrCntLaneSel value for assigning the lane that will use 
       the 31-bit error counter  
        0 - Only normal entry to Loopback.Pattern will cause the lane assignment to use 
       the 31-bit error counter 
     */
    UINT32 ext_err_ctr_all_lanes : 1;
    /* ext_err_ctr_all_lanes - Bits[27:27], ROS_V, default = 1'b0 
       This field is only valid if Extended Count All Lanes Capability is 1. The user 
       can enable counting of all bit  errors for the complete set of lanes for the 
       tested lanes, starting from the lane selected in Extended 
        Error Counter Lane Select and including any  consecutive higher lanes included 
       in the testing. 
        These errors are accumulated using the Extended Error Counter. Any per-lane 
       error counters are invalid  for the tested lanes. 
       	1 - Accumulate all errors for all lanes
       	0 - Per-lane error counters are used
     */
    UINT32 rsvd_28 : 4;
    /* rsvd_28 - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTPCC_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTPGB_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x4037119C)                                                  */
/*       SKX (0x4037119C)                                                     */
/* Register default value:              0x4DAE1577                            */
#define KTIREUT_PH_TTPGB_KTI_REUT_REG 0x0901419C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Pattern Buffer Access Register
 */
typedef union {
  struct {
    UINT32 reutpattbuff : 32;
    /* reutpattbuff - Bits[31:0], RWS_LV, default = 32'h4DAE1577 
       All words of Pattern generator buffers are accessed through this register.
        A particular pattern buffer is indexed using Pattern buffer selection bits in 
       the KTIREUT_PH_TTPGC register  
        Patterns larger than 32 bits are indexed using pattern buffer word selection 
       field in the KTIREUT_PH_TTPGC register. 
        Each pattern buffer is 128 bit, accessed 32 bit at a time. Least significant 
       bit is sent out first but pattern is not shifted. 
        PRBS pattern is generated by the 23 bit LFSR using ITU-T PRBS23 polynomial x^23 
       + x^18 + 1 and the initial seed is programmed using bits 0:23  
       	PatBuf1 Default
       	(msdw = most significant Dword)
       	msdw (D3): ACE3_C3E0h
       	msdw-1 (D2): 100F_F7FFh
       	msdw-2 (D1): ACBC_B211h
       	lsdw (D0): 4DAE_1577h
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTPGB_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTPPG_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711A0)                                                  */
/*       SKX (0x403711A0)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTPPG_KTI_REUT_REG 0x090141A0
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Pattern Generator Buffer Selection Register
 */
typedef union {
  struct {
    UINT32 c_reinit_lfsr : 16;
    /* c_reinit_lfsr - Bits[15:0], ROS_V, default = 16'b0 
       Reinit LFSR with seed after CReinitLFSR TSL. (This implies that LFSR seed 
       registers are distinct from the LFSR). 
       0: do not re-init LFSR
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTPPG_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTPG2S_KTI_REUT_REG supported on:                               */
/*       SKX_A0 (0x403711A4)                                                  */
/*       SKX (0x403711A4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTPG2S_KTI_REUT_REG 0x090141A4
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Pattern Generator Buffer 2 Selection Register
 */
typedef union {
  struct {
    UINT32 pattbuflanesel : 20;
    /* pattbuflanesel - Bits[19:0], RWS_L, default = 20'b0 
       Lane selects Pattern buffer 2 for its testing.
       	One bit per each lane
       	Bit 0 - Controls Lane 0
       	Bit 1 - Controls Lane 1
       	..., and so on.
       	1 - Corresponding lane will use Pattern Buffer 2 for test.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTPG2S_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTREC_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711A8)                                                  */
/*       SKX (0x403711A8)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTREC_KTI_REUT_REG 0x090141A8
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Rx Lane Error Check Disable Register
 */
typedef union {
  struct {
    UINT32 rxerrchdisable : 20;
    /* rxerrchdisable - Bits[19:0], RWS_L, default = 20'b0 
       Rx Lane Error Check Disable One bit for each Rx lane, if set, will disable error 
       check and accumulation during loop back test. 
       	Bit 0 - Rx Lane 0.
       	Bit 1 - Rx Lane 1.
       	...
       	Bit 19 - Rx Lane 19.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTREC_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTPIS_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711AC)                                                  */
/*       SKX (0x403711AC)                                                     */
/* Register default value:              0x00000001                            */
#define KTIREUT_PH_TTPIS_KTI_REUT_REG 0x090141AC
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Pattern Invert Selection
 */
typedef union {
  struct {
    UINT32 patterninvert : 20;
    /* patterninvert - Bits[19:0], RWS_L, default = 20'b1 
       Pattern Invert Selection
       	One bit for each Tx lane.
       	Bit 0 - Tx Lane 0
       	Bit 1 - Tx Lane 1
       	.., and so on.
       	1 - Invert the pattern in corresponding Tx lane.
        This register sets the initial inversion phase and is not dynamically updated 
       as the pattern progresses. 
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTPIS_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTDDC_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711B0)                                                  */
/*       SKX (0x403711B0)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTDDC_KTI_REUT_REG 0x090141B0
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Tx testability DC Data Lane Control Register
 */
typedef union {
  struct {
    UINT32 txdatlanctr : 20;
    /* txdatlanctr - Bits[19:0], RWS_L, default = 20'b0 
       TX DC Data Lane Control
        A bit mask used for selectively overriding the data TX to output a DC logical 
       ""1"" condition during Loopback.Pattern. 
        Used for debug and validation purposes to quiet neighboring crosstalk lines, 
       and is only applied during Loopback.Pattern. 
        Only the KTIPHTDC register will override the behavior of this register.
       	A bit value of 1 indicates the corresponding lane is transmitting a logical 1.
       	Bit 0: Controls Lane 0.
       	Bit 1: Controls Lane 1.
       	.. and so on.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTDDC_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTLECR_KTI_REUT_REG supported on:                               */
/*       SKX_A0 (0x403711B4)                                                  */
/*       SKX (0x403711B4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTLECR_KTI_REUT_REG 0x090141B4
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Lane Error Counter
 */
typedef union {
  struct {
    UINT32 laneerrcnt : 8;
    /* laneerrcnt - Bits[7:0], RW1CS, default = 8'b0 
       Lane Error Counter
       This field will contain the lower bits of the Extended Lane Error Counter, or 
       the error count bits of the short error counter for the lane selected by the 
       current RX Error Counter Lane Select.  
       To properly read all 32 bits of the Extended Lane Error Counter, the current 
       value in the RX Error Counter Lane Select field must match the assigned lane 
       designated on entry to Pattern. If Extended Error Count All Lanes is set, then 
       the value of RX Error Counter Lane Select should not be changed from the 
       assigned lane designated upon entry to Pattern. 
       Any per-lane Error Counters will be cleared by software writing 1 to the 
       appropriate bit in the Receive Error Status register, writing 1 to the 
       appropriate bit in the Receive Error Check Disable field, or by a Cold Reset. 
       Writing to this field will have no affect on any per-Lane Error Counters. If in 
       Extended Error Count All Lanes mode, this field can be cleared through writing 
       1s to this field. 
       (NOTE: While field type is RW1CS, field effectively behaves as read-only (RO).
     */
    UINT32 ex_laneerrcnt : 23;
    /* ex_laneerrcnt - Bits[30:8], RW1CS, default = 23'b0 
       Extended Lane Error Counter
       Field is assigned using the RxError Counter Lane Select on entry to 
       Loopback.Pattern.  
       It may also be reassigned at any time by setting the Extended Error Counter Lane 
       Assign.  
       The Extended Lane Error Counter can be cleared through software by writing 1 to 
       bit 0 of this register, or a Default Reset. 
       Note: Only the lower bit of this register controls clearing of the entire field 
       as shown below. Reading this register while the RX Error Counter Lane Select is 
       not set to the assigned Lane will continue to return the upper bits from the 
       Extended Lane Error Counter. This may be done to read the short Lane Error 
       Counters. Software must be aware of this and clear this field before reporting 
       to the user. 
       (NOTE: Clear operation on write of matching lane's bit in TTRES is not 
       implemented. Field does clear, however, whenever selected lane in TTPCC[20:16] 
       is changed.) 
     */
    UINT32 errovrflow : 1;
    /* errovrflow - Bits[31:31], RW1CS, default = 1'b0 
       Error Overflow
       	If set, indicates that the error counter has overflown in selected lane.
       	Cleared with writing 1 or PowerGood Reset.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTLECR_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTTLS_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711B8)                                                  */
/*       SKX (0x403711B8)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTTLS_KTI_REUT_REG 0x090141B8
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Tx Loop Counter Status
 */
typedef union {
  struct {
    UINT32 s_tx_loopcntstat : 32;
    /* s_tx_loopcntstat - Bits[31:0], RO_V, default = 32'b0 
       Tx Loop Count 
       The Loop Count Status register represents the current received Loop Count while 
       in Loopback.Pattern or the final transmitted Loop Count after a test. The Loop 
       Count Status is continuously incremented by 1 on the transmission of a complete 
       pattern buffer boundary while in Loopback.Pattern and rolls over to all 0s upon 
       hitting the maximum value. 
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTTLS_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTLCS_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711BC)                                                  */
/*       SKX (0x403711BC)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTLCS_KTI_REUT_REG 0x090141BC
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Rx Loop Counter Status
 */
typedef union {
  struct {
    UINT32 s_rx_loopcntstat : 32;
    /* s_rx_loopcntstat - Bits[31:0], RW1CS, default = 32'b0 
       Rx Loop Count
       The Loop Count Status register represents the current received Loop Count while 
       in Loopback.Pattern or the final checked Loop Count after a test. The Loop Count 
       Status is continuously incremented by 1 on the reception of a complete pattern 
       buffer boundary while in Loopback.Pattern and rolls over to all 0s upon hitting 
       the maximum value. 
       When Stop on Error is set to 1 and a specific LFSR is used to scramble either 
       Pattern Buffer 1 or 2, the Loop Count Status can be used to recreate the region 
       of the failing pattern by software replicating the LFSR using the resultant Loop 
       Count Status. The Loop Count Status can be cleared through software writing 1s 
       to this register, or a Cold Reset. 
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTLCS_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTRES_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711C0)                                                  */
/*       SKX (0x403711C0)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTRES_KTI_REUT_REG 0x090141C0
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Rx Error Status
 */
typedef union {
  struct {
    UINT32 rx_lane_error_status : 20;
    /* rx_lane_error_status - Bits[19:0], RW1CS, default = 20'b0 
       Rx Lane Error Status
       One bit per each Rx lane
       Bit 0 - Status of Lane 0
       Bit 1 - Status of Lane 1
       ..., and so on.
       Cleared with writing 1. Clearing a lane status also clears the lane error 
       counter for that lane (but not the extended error lane counter/overflow). 
       1 - Corresponding lane has received error.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTRES_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTLEO_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711C4)                                                  */
/*       SKX (0x403711C4)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTLEO_KTI_REUT_REG 0x090141C4
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Rx Lane Error Overflow Status
 */
typedef union {
  struct {
    UINT32 rxlaneerrsoverstat : 20;
    /* rxlaneerrsoverstat - Bits[19:0], RW1CS, default = 20'b0 
       Rx Lane Error Overflow Status
       Indicates that a particular lane encountered a sufficient number of errors 
       during Loopback.Pattern to overflow any per-lane error counter. The counter may 
       be as simple as 1 bit, in which case this can be interpreted as ?multiple 
       errors?. 
       A value of 1 indicates that the particular lane has received an overflow of 
       errors during Loopback.Pattern. 
       One bit per Rx lane.
       Bit 0 - Status of Lane 0
       Bit 1 - Status of Lane 1
       ... and so on.
       Cleared by writing 1.
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTLEO_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTTES_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711C8)                                                  */
/*       SKX (0x403711C8)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTTES_KTI_REUT_REG 0x090141C8
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Test Execution Status
 */
typedef union {
  struct {
    UINT32 test_initiated : 1;
    /* test_initiated - Bits[0:0], ROS_V, default = 1'b0 
       Master Tx ""SDS"" sent
     */
    UINT32 error_check_start : 1;
    /* error_check_start - Bits[1:1], ROS_V, default = 1'b0 
       Master Rx ""SDS"" received, error checking has begun.
       Bit is automatically reset to default state, '0',upon ""Start Test"" being set 
       to '0'. 
     */
    UINT32 test_completed : 1;
    /* test_completed - Bits[2:2], ROS_V, default = 1'b0 
       Full test has been transmitted and checked. Bit is automatically reset to 
       default state, '0', upon ""Start Test"" being set to '0'. 
     */
    UINT32 marker_relock_achieved : 1;
    /* marker_relock_achieved - Bits[3:3], ROS_V, default = 1'b0 
       Post-test execution transition back to Marker and bit/byte lock achieved.
       Remains persistent until the TX SDS is sent at which time it is reset to its 
       default state of 0. 
     */
    UINT32 any_lane_fails : 1;
    /* any_lane_fails - Bits[4:4], ROS_V, default = 1'b0 
       Any lane fails.
       Bit is automatically reset to default state, 0,upon [Start Test] being set to 0.
     */
    UINT32 all_lanes_fail : 1;
    /* all_lanes_fail - Bits[5:5], ROS_V, default = 1'b0 
       All lanes fail.
       Bit is automatically reset to default state, 0,upon [Start Test] being set to 0.
     */
    UINT32 any_lane_overflows : 1;
    /* any_lane_overflows - Bits[6:6], ROS_V, default = 1'b0 
       Any lane error counter overflow.
       Bit is automatically reset to default state, 0,upon [Start Test] being set to 0.
     */
    UINT32 all_lanes_overflow : 1;
    /* all_lanes_overflow - Bits[7:7], ROS_V, default = 1'b0 
       All lanes overflow.
       Bit is automatically reset to default state, 0,upon [Start Test] being set to 0.
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTTES_KTI_REUT_STRUCT;


/* KTIREUT_PH_TTSNL_KTI_REUT_REG supported on:                                */
/*       SKX_A0 (0x403711CC)                                                  */
/*       SKX (0x403711CC)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TTSNL_KTI_REUT_REG 0x090141CC
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * REUT Slave Loopback Path Select
 */
typedef union {
  struct {
    UINT32 lpbkpath : 20;
    /* lpbkpath - Bits[19:0], RWS_L, default = 20'b0 
       Lane selects for loopback path from Rx
       	Bit 0 - Controls Lane 0
       	Bit 1 - Controls Lane 1
       	..., and so on.
       	0 - Corresponding lane will use RX loopback path
       	1 - Corresponding lane will use path from pattern generator
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TTSNL_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL0PENTRYTX_KTI_REUT_REG supported on:                          */
/*       SKX_A0 (0x403711E0)                                                  */
/*       SKX (0x403711E0)                                                     */
/* Register default value:              0x06000664                            */
#define KTIREUT_PH_TL0PENTRYTX_KTI_REUT_REG 0x090141E0
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Power Management Tx Timers for L0p Entry
 */
typedef union {
  struct {
    UINT32 t_l0p_tx_tran2done : 8;
    /* t_l0p_tx_tran2done - Bits[7:0], RWS_L, default = 8'd100 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_tx_enter2tran : 8;
    /* t_l0p_tx_enter2tran - Bits[15:8], RWS_L, default = 8'h06 
       Linear time value, with granularity of 8UI; due to an implementation-specific 
       restriction, the value of this register field must be greater than or equal to 
       6. 
     */
    UINT32 t_l0p_tx_nullstart2nullstop : 8;
    /* t_l0p_tx_nullstart2nullstop - Bits[23:16], RWS_L, default = 8'h00 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_tx_enter2null : 8;
    /* t_l0p_tx_enter2null - Bits[31:24], RWS_L, default = 8'h06 
       Linear time value, with granularity of 8UI; due to an implementation-specific 
       restriction, the value of this register field must be greater than or equal to 
       6. 
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL0PENTRYTX_KTI_REUT_STRUCT;


/* KTIREUT_PH_L0PENTRYRX_KTI_REUT_REG supported on:                           */
/*       SKX_A0 (0x403711E4)                                                  */
/*       SKX (0x403711E4)                                                     */
/* Register default value:              0x06000664                            */
#define KTIREUT_PH_L0PENTRYRX_KTI_REUT_REG 0x090141E4
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Power Management Rx Timers for L0p Entry
 */
typedef union {
  struct {
    UINT32 t_l0p_rx_tran2done : 8;
    /* t_l0p_rx_tran2done - Bits[7:0], RWS_L, default = 8'd100 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_rx_enter2tran : 8;
    /* t_l0p_rx_enter2tran - Bits[15:8], RWS_L, default = 8'h06 
       Linear time value, with granularity of 8UI; due to an implementation-specific 
       restriction, the value of this regsiter field must be greater than or equal to 
       3. 
     */
    UINT32 t_l0p_rx_tran2unblock : 8;
    /* t_l0p_rx_tran2unblock - Bits[23:16], RWS_L, default = 8'h00 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_rx_enter2block : 8;
    /* t_l0p_rx_enter2block - Bits[31:24], RWS_L, default = 8'h06 
       Linear time value, with granularity of 8UI; due to an implementation-specific 
       restriction, the value of this register field must be greater than or equal to 
       1. 
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_L0PENTRYRX_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL0PEXITTXACT_KTI_REUT_REG supported on:                        */
/*       SKX_A0 (0x403711E8)                                                  */
/*       SKX (0x403711E8)                                                     */
/* Register default value:              0x00001E00                            */
#define KTIREUT_PH_TL0PEXITTXACT_KTI_REUT_REG 0x090141E8
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Power Management Rx Act Timers for L0p Exit
 */
typedef union {
  struct {
    UINT32 t_l0p_tx_nullsexit2tran : 8;
    /* t_l0p_tx_nullsexit2tran - Bits[7:0], RWS_L, default = 8'h00 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_tx_sqend2sds : 8;
    /* t_l0p_tx_sqend2sds - Bits[15:8], RWS_L, default = 8'd30 
       Linear time value, with granularity of 128UI
     */
    UINT32 t_l0p_tx_nullsexit_sq : 8;
    /* t_l0p_tx_nullsexit_sq - Bits[23:16], RWS_L, default = 8'h00 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_tx_exitend2l0c : 8;
    /* t_l0p_tx_exitend2l0c - Bits[31:24], RWS_L, default = 8'h00 
       Linear time value, with granularity of 8UI
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL0PEXITTXACT_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL0PEXITTXIDL_KTI_REUT_REG supported on:                        */
/*       SKX_A0 (0x403711EC)                                                  */
/*       SKX (0x403711EC)                                                     */
/* Register default value:              0x04990620                            */
#define KTIREUT_PH_TL0PEXITTXIDL_KTI_REUT_REG 0x090141EC
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Power Management Rx Idl Timers for L0p Exit
 */
typedef union {
  struct {
    UINT32 t_l0p_tx_exit2next : 8;
    /* t_l0p_tx_exit2next - Bits[7:0], RWS_L, default = 8'd32 
       Linear time value, with granularity of 128UI
     */
    UINT32 t_l0p_tx_sds2widthchg : 8;
    /* t_l0p_tx_sds2widthchg - Bits[15:8], RWS_L, default = 8'd6 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_tx_sds : 2;
    /* t_l0p_tx_sds - Bits[17:16], ROS_V, default = 2'b01 
       Linear time value, with granularity of 128UI
     */
    UINT32 t_l0p_tx_pat : 8;
    /* t_l0p_tx_pat - Bits[25:18], RWS_L, default = 8'd38 
       Linear time value, with granularity of 128UI
     */
    UINT32 t_l0p_tx_sbpa : 2;
    /* t_l0p_tx_sbpa - Bits[27:26], RWS_L, default = 2'b01 
       Linear time value, with granularity of 128UI
     */
    UINT32 t_l0p_tx_null2firston : 4;
    /* t_l0p_tx_null2firston - Bits[31:28], RWS_L, default = 4'h0 
       Linear time value, with granularity of 8UI
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL0PEXITTXIDL_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL0PEXITRX_KTI_REUT_REG supported on:                           */
/*       SKX_A0 (0x403711F4)                                                  */
/*       SKX (0x403711F4)                                                     */
/* Register default value:              0x00200600                            */
#define KTIREUT_PH_TL0PEXITRX_KTI_REUT_REG 0x090141F4
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Power Management Rx Timers for L0p Exit
 */
typedef union {
  struct {
    UINT32 t_l0p_rx_nulls_widthchg : 8;
    /* t_l0p_rx_nulls_widthchg - Bits[7:0], RWS_L, default = 8'h00 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_rx_sds2widthchg : 8;
    /* t_l0p_rx_sds2widthchg - Bits[15:8], RWS_L, default = 8'd6 
       Linear time value, with granularity of 8UI
     */
    UINT32 t_l0p_est : 8;
    /* t_l0p_est - Bits[23:16], RWS_L, default = 8'd32 
       Linear time value, with granularity of 128UI
     */
    UINT32 t_l0p_rx_nullsexit_sq : 8;
    /* t_l0p_rx_nullsexit_sq - Bits[31:24], RWS_L, default = 8'h00 
       Linear time value, with granularity of 8UI
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL0PEXITRX_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL1ENTRY_KTI_REUT_REG supported on:                             */
/*       SKX_A0 (0x403711F8)                                                  */
/*       SKX (0x403711F8)                                                     */
/* Register default value:              0x0F00004A                            */
#define KTIREUT_PH_TL1ENTRY_KTI_REUT_REG 0x090141F8
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Power Management Timers for L1
 */
typedef union {
  struct {
    UINT32 t_l1_minres : 12;
    /* t_l1_minres - Bits[11:0], RWS_L, default = 12'd74 
       Linear time value, with granularity of 128UI
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[19:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 t_l1_tx_firstoff2lastoff : 4;
    /* t_l1_tx_firstoff2lastoff - Bits[23:20], RWS_L, default = 4'h0 
       Linear time value, with granularity of 128UI
     */
    UINT32 t_l1_tx_enter2firstoff : 8;
    /* t_l1_tx_enter2firstoff - Bits[31:24], RWS_L, default = 8'd15 
       Linear time value, with granularity of 8UI
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL1ENTRY_KTI_REUT_STRUCT;


/* KTIREUT_PH_TPRI_SL_KTI_REUT_REG supported on:                              */
/*       SKX_A0 (0x40371200)                                                  */
/*       SKX (0x40371200)                                                     */
/* Register default value:              0x00C13344                            */
#define KTIREUT_PH_TPRI_SL_KTI_REUT_REG 0x09014200
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Primary Exponential Timer Primary Slow Speed
 */
typedef union {
  struct {
    UINT32 t_tc : 4;
    /* t_tc - Bits[3:0], RWS_L, default = 4'h4 
       Tc calib min time; for stagger on;
       exponential time value, with granularity of 128UI
     */
    UINT32 t_rc : 4;
    /* t_rc - Bits[7:4], RWS_L, default = 4'h4 
       Rx calib min time; for stagger Tx off;
       exponential time value, with granularity of 128UI
     */
    UINT32 t_rs : 4;
    /* t_rs - Bits[11:8], RWS_L, default = 4'h3 
       ResetS residency for driving EIOS;
       exponential time value, with granularity of 128UI
     */
    UINT32 t_rdb : 4;
    /* t_rdb - Bits[15:12], RWS_L, default = 4'h3 
       RxDetect debounce time per leg. Should be programmed to same value on both sides 
       of link. 
       Exponential time value, with granularity of 128UI
     */
    UINT32 n_tdr : 4;
    /* n_tdr - Bits[19:16], RWS_L, default = 4'h1 
       TxDetect Rx wake occurance window count for handshakei; value*granluarity (1) = 
       number of consecutive 4K UI windows during which wake must be detected for 
       debounce success in Tx Detect 
     */
    UINT32 n_tdt : 8;
    /* n_tdt - Bits[27:20], RWS_L, default = 8'd12 
       TxDetect wake supersequence (PatDetect) count for handshake; value*granularity 
       (1) = number of TxDetect wake supersequence (PatDetect) count for handshake 
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TPRI_SL_KTI_REUT_STRUCT;


/* KTIREUT_PH_TSEC_SL_KTI_REUT_REG supported on:                              */
/*       SKX_A0 (0x40371204)                                                  */
/*       SKX (0x40371204)                                                     */
/* Register default value:              0x33405440                            */
#define KTIREUT_PH_TSEC_SL_KTI_REUT_REG 0x09014204
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Primary Exponential Timer Secondary Slow Speed
 */
typedef union {
  struct {
    UINT32 t_lmst : 4;
    /* t_lmst - Bits[3:0], RWS_L, default = 4'h0 
       For Tx adaptation in loopback master-master mode non-zero value should be used 
       for fast speed (e.g., value used for Tclst). Tlmst is typically 0. For debug, 
       CSingleStep and CFIA (with non-zero Tlmst) can be used. 
       Exponential time value, with granularity of 4096UI
     */
    UINT32 t_cfst : 4;
    /* t_cfst - Bits[7:4], RWS_L, default = 4'h4 
       Wait for planetary aligned clean flit boundary. If determinism is enabled, Tcfst 
       must be greater than EPhyL0Sync period. 
       Exponential time value, with granularity of 4096UI
     */
    UINT32 t_clst : 4;
    /* t_clst - Bits[11:8], RWS_L, default = 4'h4 
       Configure link widths;
       exponential time value, with granularity of 4096UI
     */
    UINT32 t_plst : 4;
    /* t_plst - Bits[15:12], RWS_L, default = 4'h5 
       Adapt+bitlock/bytelock/deskew;
       exponential time value, with granularity of 4096UI
     */
    UINT32 t_rdst : 4;
    /* t_rdst - Bits[19:16], RWS_L, default = 4'h0 
       Remote Rx detect/debounce; 0 or no timeout for hot plug; non-0 value to debug 
       hangs; 
       exponential time value, with granularity of 4096UI
     */
    UINT32 t_tdst : 4;
    /* t_tdst - Bits[23:20], RWS_L, default = 4'h4 
       Squelch exit detect/debounce. This value (in UI) should be larger than TL0c.
       Exponential time value, with granularity of 4096UI
     */
    UINT32 t_tcst : 4;
    /* t_tcst - Bits[27:24], RWS_L, default = 4'h3 
       Timed Tx calib, e.g. icomp;
       exponential time value, with granularity of 4096UI
     */
    UINT32 t_rcst : 4;
    /* t_rcst - Bits[31:28], RWS_L, default = 4'h3 
       Timed Rx calibration, e.g. oc;
       exponential time value, with granularity of 4096UI
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TSEC_SL_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL0CENTRY_SL_KTI_REUT_REG supported on:                         */
/*       SKX_A0 (0x4037120C)                                                  */
/*       SKX (0x4037120C)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TL0CENTRY_SL_KTI_REUT_REG 0x0901420C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Primary Exponential Timer L0c Entry Slow Speed
 */
typedef union {
  struct {
    UINT32 t_ieo : 12;
    /* t_ieo - Bits[11:0], RWS_L, default = 12'h0 
       Init exit delay; used in debug to delay exit from init;
       linear time value, with granularity of 8UI
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL0CENTRY_SL_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL0C_SL_KTI_REUT_REG supported on:                              */
/*       SKX_A0 (0x40371210)                                                  */
/*       SKX (0x40371210)                                                     */
/* Register default value:              0x00010021                            */
#define KTIREUT_PH_TL0C_SL_KTI_REUT_REG 0x09014210
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Primary Exponential Timer L0c Slow Speed
 */
typedef union {
  struct {
    UINT32 t_l0c : 8;
    /* t_l0c - Bits[7:0], RWS_L, default = 8'h21 
       L0c clean flit interval.
       This value is loaded into the local TL0ct and TL0cr timers under PHY LTSSM 
       control. 
       Timeout is disabled if value is 0.
       Linear time value, with granularity of 128UI
     */
    UINT32 t_l1_reissue_delay : 8;
    /* t_l1_reissue_delay - Bits[15:8], RWS_L, default = 8'h0 
       L1 reissue delayed until after timeout (512UI granularity)
     */
    UINT32 t_l0cq : 4;
    /* t_l0cq - Bits[19:16], ROS_V, default = 4'h1 
       L0c duration. Value must be 1. Linear  time value, with granularity of 8UI
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL0C_SL_KTI_REUT_STRUCT;


/* KTIREUT_PH_TPRI_OP_KTI_REUT_REG supported on:                              */
/*       SKX_A0 (0x40371214)                                                  */
/*       SKX (0x40371214)                                                     */
/* Register default value:              0x00C13344                            */
#define KTIREUT_PH_TPRI_OP_KTI_REUT_REG 0x09014214
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Primary Exponential Timer Primary Operational Speed
 */
typedef union {
  struct {
    UINT32 t_tc : 4;
    /* t_tc - Bits[3:0], RWS_L, default = 4'h4 
       Tc calib min time; for stagger on;
       exponential time value, with granularity of 128UI
     */
    UINT32 t_rc : 4;
    /* t_rc - Bits[7:4], RWS_L, default = 4'h4 
       Rx calib min time; for stagger Tx off;
       exponential time value, with granularity of 128UI
     */
    UINT32 t_rs : 4;
    /* t_rs - Bits[11:8], RWS_L, default = 4'h3 
       ResetS residency for driving EIOS;
       exponential time value, with granularity of 128UI
     */
    UINT32 t_rdb : 4;
    /* t_rdb - Bits[15:12], RWS_L, default = 4'h3 
       RxDetect debounce time per leg. Should be programmed to same value on both sides 
       of link. 
       Exponential time value, with granularity of 128UI
     */
    UINT32 n_tdr : 4;
    /* n_tdr - Bits[19:16], RWS_L, default = 4'h1 
       TxDetect Rx wake occurance window count for handshake; value*granularity (1) = 
       number of consecutive 4K UI windows during which wake must be detected for 
       debounce success in Tx Detect 
     */
    UINT32 n_tdt : 8;
    /* n_tdt - Bits[27:20], RWS_L, default = 8'd12 
       TxDetect wake supersequence (PatDetect) count for handshake; value*granularity 
       (1) = number of TxDetect wake supersequence (PatDetect) count for handshake 
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TPRI_OP_KTI_REUT_STRUCT;


/* KTIREUT_PH_TSEC_OP_KTI_REUT_REG supported on:                              */
/*       SKX_A0 (0x40371218)                                                  */
/*       SKX (0x40371218)                                                     */
/* Register default value:              0xA9409440                            */
#define KTIREUT_PH_TSEC_OP_KTI_REUT_REG 0x09014218
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Primary Exponential Timer Secondary Operational Speed
 */
typedef union {
  struct {
    UINT32 t_lmst : 4;
    /* t_lmst - Bits[3:0], RWS_L, default = 4'h0 
       For Tx adaptation in loopback master-master mode non-zero value should be used 
       for fast speed (e.g., value used for Tclst). Tlmst is typically 0. For debug, 
       CSingleStep and CFIA (with non-zero Tlmst) can be used. 
       Exponential time value, with granularity of 4096UI
     */
    UINT32 t_cfst : 4;
    /* t_cfst - Bits[7:4], RWS_L, default = 4'h4 
       Wait for planetary aligned clean flit boundary. If determinism is enabled, Tcfst 
       must be greater than EPhyL0Sync period. 
       Exponential time value, with granularity of 4096UI
     */
    UINT32 t_clst : 4;
    /* t_clst - Bits[11:8], RWS_L, default = 4'h4 
       Configure link widths;
       exponential time value, with granularity of 4096UI
     */
    UINT32 t_plst : 4;
    /* t_plst - Bits[15:12], RWS_L, default = 4'h9 
       Adapt+bitlock/bytelock/deskew;
       exponential time value, with granularity of 4096UI
     */
    UINT32 t_rdst : 4;
    /* t_rdst - Bits[19:16], RWS_L, default = 4'h0 
       Remote Rx detect/debounce; 0 or no timeout for hot plug; non-0 value to debug 
       hangs 
       exponential time value, with granularity of 4096UI
     */
    UINT32 t_tdst : 4;
    /* t_tdst - Bits[23:20], RWS_L, default = 4'h4 
       Squelch exit detect/debounce. This value (in UI) should be larger than Tl0c.
       Exponential time value, with granularity of 4096UI
     */
    UINT32 t_tcst : 4;
    /* t_tcst - Bits[27:24], RWS_L, default = 4'h9 
       Timed Tx calib, e.g. icomp;
       exponential time value, with granularity of 4096UI
     */
    UINT32 t_rcst : 4;
    /* t_rcst - Bits[31:28], RWS_L, default = 4'hA 
       Timed Rx calibration, e.g. oc;
       exponential time value, with granularity of 4096UI
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TSEC_OP_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL0CENTRY_OP_KTI_REUT_REG supported on:                         */
/*       SKX_A0 (0x40371220)                                                  */
/*       SKX (0x40371220)                                                     */
/* Register default value:              0x00000000                            */
#define KTIREUT_PH_TL0CENTRY_OP_KTI_REUT_REG 0x09014220
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Primary Exponential Timer L0c Entry Operational Speed
 */
typedef union {
  struct {
    UINT32 t_ieo : 12;
    /* t_ieo - Bits[11:0], RWS_L, default = 12'h0 
       Init exit delay; used in debug to delay exit from init;
       linear time value, with granularity of 8UI
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL0CENTRY_OP_KTI_REUT_STRUCT;


/* KTIREUT_PH_TL0C_OP_KTI_REUT_REG supported on:                              */
/*       SKX_A0 (0x40371224)                                                  */
/*       SKX (0x40371224)                                                     */
/* Register default value:              0x00010021                            */
#define KTIREUT_PH_TL0C_OP_KTI_REUT_REG 0x09014224
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Primary Exponential Timer L0c Operational Speed
 */
typedef union {
  struct {
    UINT32 t_l0c : 8;
    /* t_l0c - Bits[7:0], RWS_L, default = 8'h21 
       L0c clean flit interval.
       This value is loaded into the local TL0ct and TL0cr timers under PHY LTSSM 
       control. 
       Linear time value, with granularity of 128UI.
       Can only be legally adjust in steps of 3 to keep alignment to 48UI
     */
    UINT32 t_l1_reissue_delay : 8;
    /* t_l1_reissue_delay - Bits[15:8], RWS_L, default = 8'h0 
       L1 reissue delayed until after timeout (512UI Granularity)
     */
    UINT32 t_l0cq : 4;
    /* t_l0cq - Bits[19:16], ROS_V, default = 4'h1 
       L0c duration. Value must be 1. Linear time value, with granularity of 8UI
     */
    UINT32 rsvd : 12;
    /* rsvd - Bits[31:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIREUT_PH_TL0C_OP_KTI_REUT_STRUCT;


/* KTIPHYPWRCTRL_KTI_REUT_REG supported on:                                   */
/*       SKX_A0 (0x40371228)                                                  */
/*       SKX (0x40371228)                                                     */
/* Register default value:              0x00000000                            */
#define KTIPHYPWRCTRL_KTI_REUT_REG 0x09014228
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
 * Intel UPI Phy power control
 */
typedef union {
  struct {
    UINT32 fake_l1 : 1;
    /* fake_l1 - Bits[0:0], RWS_LB, default = 1'b0 
       Overrides the signal the tells the PCU the link is eligible to enter Pkg-C.
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} KTIPHYPWRCTRL_KTI_REUT_STRUCT;


/* PXPCAP_F1_KTI_REUT_REG supported on:                                       */
/*       SKX (0x40371040)                                                     */
/* Register default value:              0x00920010                            */
#define PXPCAP_F1_KTI_REUT_REG 0x09014040
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXPCAP_F1_KTI_REUT_STRUCT;


/* PXP_RSVD0_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x40371044)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD0_F1_KTI_REUT_REG 0x09014044
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD0_F1_KTI_REUT_STRUCT;


/* PXP_RSVD1_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x40371048)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD1_F1_KTI_REUT_REG 0x09014048
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD1_F1_KTI_REUT_STRUCT;


/* PXP_RSVD2_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x4037104C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD2_F1_KTI_REUT_REG 0x0901404C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD2_F1_KTI_REUT_STRUCT;


/* PXP_RSVD3_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x40371050)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD3_F1_KTI_REUT_REG 0x09014050
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD3_F1_KTI_REUT_STRUCT;


/* PXP_RSVD4_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x40371054)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD4_F1_KTI_REUT_REG 0x09014054
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD4_F1_KTI_REUT_STRUCT;


/* PXP_RSVD5_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x40371058)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD5_F1_KTI_REUT_REG 0x09014058
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD5_F1_KTI_REUT_STRUCT;


/* PXP_RSVD6_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x4037105C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD6_F1_KTI_REUT_REG 0x0901405C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD6_F1_KTI_REUT_STRUCT;


/* PXP_RSVD7_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x40371060)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD7_F1_KTI_REUT_REG 0x09014060
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD7_F1_KTI_REUT_STRUCT;


/* PXP_RSVD8_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x40371064)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD8_F1_KTI_REUT_REG 0x09014064
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD8_F1_KTI_REUT_STRUCT;


/* PXP_RSVD9_F1_KTI_REUT_REG supported on:                                    */
/*       SKX (0x40371068)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD9_F1_KTI_REUT_REG 0x09014068
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD9_F1_KTI_REUT_STRUCT;


/* PXP_RSVD10_F1_KTI_REUT_REG supported on:                                   */
/*       SKX (0x4037106C)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD10_F1_KTI_REUT_REG 0x0901406C
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD10_F1_KTI_REUT_STRUCT;


/* PXP_RSVD11_F1_KTI_REUT_REG supported on:                                   */
/*       SKX (0x40371070)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD11_F1_KTI_REUT_REG 0x09014070
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD11_F1_KTI_REUT_STRUCT;


/* PXP_RSVD12_F1_KTI_REUT_REG supported on:                                   */
/*       SKX (0x40371074)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD12_F1_KTI_REUT_REG 0x09014074
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD12_F1_KTI_REUT_STRUCT;


/* PXP_RSVD13_F1_KTI_REUT_REG supported on:                                   */
/*       SKX (0x40371078)                                                     */
/* Register default value:              0x00000000                            */
#define PXP_RSVD13_F1_KTI_REUT_REG 0x09014078
/* Struct format extracted from XML file SKX\3.14.1.CFG.xml.
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
} PXP_RSVD13_F1_KTI_REUT_STRUCT;


#endif /* KTI_REUT_h */
