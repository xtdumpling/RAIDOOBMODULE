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

#ifndef CHABC_SAD_h
#define CHABC_SAD_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* CHABC_SAD_DEV 29                                                           */
/* CHABC_SAD_FUN 0                                                            */

/* VID_CHABC_SAD_REG supported on:                                            */
/*       SKX_A0 (0x201E8000)                                                  */
/*       SKX (0x201E8000)                                                     */
/* Register default value:              0x8086                                */
#define VID_CHABC_SAD_REG 0x03002000
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} VID_CHABC_SAD_STRUCT;


/* DID_CHABC_SAD_REG supported on:                                            */
/*       SKX_A0 (0x201E8002)                                                  */
/*       SKX (0x201E8002)                                                     */
/* Register default value:              0x2054                                */
#define DID_CHABC_SAD_REG 0x03002002
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2054 
        
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
} DID_CHABC_SAD_STRUCT;


/* PCICMD_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x201E8004)                                                  */
/*       SKX (0x201E8004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_CHABC_SAD_REG 0x03002004
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} PCICMD_CHABC_SAD_STRUCT;


/* PCISTS_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x201E8006)                                                  */
/*       SKX (0x201E8006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_CHABC_SAD_REG 0x03002006
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} PCISTS_CHABC_SAD_STRUCT;


/* RID_CHABC_SAD_REG supported on:                                            */
/*       SKX_A0 (0x101E8008)                                                  */
/*       SKX (0x101E8008)                                                     */
/* Register default value:              0x00                                  */
#define RID_CHABC_SAD_REG 0x03001008
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} RID_CHABC_SAD_STRUCT;


/* CCR_N0_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x101E8009)                                                  */
/*       SKX (0x101E8009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_CHABC_SAD_REG 0x03001009
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_CHABC_SAD_STRUCT;


/* CCR_N1_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x201E800A)                                                  */
/*       SKX (0x201E800A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_CHABC_SAD_REG 0x0300200A
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} CCR_N1_CHABC_SAD_STRUCT;


/* CLSR_CHABC_SAD_REG supported on:                                           */
/*       SKX_A0 (0x101E800C)                                                  */
/*       SKX (0x101E800C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_CHABC_SAD_REG 0x0300100C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} CLSR_CHABC_SAD_STRUCT;


/* PLAT_CHABC_SAD_REG supported on:                                           */
/*       SKX_A0 (0x101E800D)                                                  */
/*       SKX (0x101E800D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_CHABC_SAD_REG 0x0300100D
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} PLAT_CHABC_SAD_STRUCT;


/* HDR_CHABC_SAD_REG supported on:                                            */
/*       SKX_A0 (0x101E800E)                                                  */
/*       SKX (0x101E800E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_CHABC_SAD_REG 0x0300100E
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} HDR_CHABC_SAD_STRUCT;


/* BIST_CHABC_SAD_REG supported on:                                           */
/*       SKX_A0 (0x101E800F)                                                  */
/*       SKX (0x101E800F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_CHABC_SAD_REG 0x0300100F
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} BIST_CHABC_SAD_STRUCT;


/* SVID_CHABC_SAD_REG supported on:                                           */
/*       SKX_A0 (0x201E802C)                                                  */
/*       SKX (0x201E802C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_CHABC_SAD_REG 0x0300202C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} SVID_CHABC_SAD_STRUCT;


/* SDID_CHABC_SAD_REG supported on:                                           */
/*       SKX_A0 (0x201E802E)                                                  */
/*       SKX (0x201E802E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_CHABC_SAD_REG 0x0300202E
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} SDID_CHABC_SAD_STRUCT;


/* CAPPTR_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x101E8034)                                                  */
/*       SKX (0x101E8034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_CHABC_SAD_REG 0x03001034
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} CAPPTR_CHABC_SAD_STRUCT;


/* INTL_CHABC_SAD_REG supported on:                                           */
/*       SKX_A0 (0x101E803C)                                                  */
/*       SKX (0x101E803C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_CHABC_SAD_REG 0x0300103C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} INTL_CHABC_SAD_STRUCT;


/* INTPIN_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x101E803D)                                                  */
/*       SKX (0x101E803D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_CHABC_SAD_REG 0x0300103D
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} INTPIN_CHABC_SAD_STRUCT;


/* MINGNT_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x101E803E)                                                  */
/*       SKX (0x101E803E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_CHABC_SAD_REG 0x0300103E
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} MINGNT_CHABC_SAD_STRUCT;


/* MAXLAT_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x101E803F)                                                  */
/*       SKX (0x101E803F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_CHABC_SAD_REG 0x0300103F
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
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
} MAXLAT_CHABC_SAD_STRUCT;


/* PAM0123_CHABC_SAD_REG supported on:                                        */
/*       SKX_A0 (0x401E8040)                                                  */
/*       SKX (0x401E8040)                                                     */
/* Register default value:              0x00000000                            */
#define PAM0123_CHABC_SAD_REG 0x03004040
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * PAM0123 Configuration register.  All fields of this register is locked with both 
 * LTCtrlSts.ConfigLock and PAM0123_CFG.Lock are set.  This lock is bypassable with 
 * SMM writes. 
 *       
 */
typedef union {
  struct {
    UINT32 lock : 1;
    /* lock - Bits[0:0], RW_LB, default = 2'b00 
       If this bit is set and LTCtrlSts.ConfigLock is set, all fields in PAM0123_CFG 
       and PAM456_CFG are locked.  The lock is bypassed by SMM writes. 
               
     */
    UINT32 rsvd_1 : 3;
    /* rsvd_1 - Bits[3:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam0_hienable : 2;
    /* pam0_hienable - Bits[5:4], RW_LB, default = 2'b00 
       0F0000-0FFFFF Attribute (HIENABLE). This field controls the steering of read and 
       write cycles that address the BIOS area from 0F0000 to 0FFFFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
       
       Note that PAM0 covers BIOS regions (F0000-FFFFF range). An INIT may send the 
       processor to jump to FFFF0 address. Therefore, it is a good idea to protect PAM0 
       range. Generally, BIOS does not need to change F region after it has locked 
       configuration. 
       
       Other PAM regions are used for shadowing option ROMs. That process happens after 
       config locking and requires changes to these PAM regs.  
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam1_loenable : 2;
    /* pam1_loenable - Bits[9:8], RW_LB, default = 2'b00 
       0C0000-0C3FFF Attribute (LOENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0C0000 to 0C3FFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam1_hienable : 2;
    /* pam1_hienable - Bits[13:12], RW_LB, default = 2'b00 
       0C4000-0C7FFF Attribute (HIENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0C4000 to 0C7FFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam2_loenable : 2;
    /* pam2_loenable - Bits[17:16], RW_LB, default = 2'b00 
       0C8000-0CBFFF Attribute (LOENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0C8000 to 0CBFFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_18 : 2;
    /* rsvd_18 - Bits[19:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam2_hienable : 2;
    /* pam2_hienable - Bits[21:20], RW_LB, default = 2'b00 
       0CC000-0CFFFF Attribute (HIENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0CC000 to 0CFFFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_22 : 2;
    /* rsvd_22 - Bits[23:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam3_loenable : 2;
    /* pam3_loenable - Bits[25:24], RW_LB, default = 2'b00 
       0D0000-0D3FFF Attribute (LOENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0D0000 to 0D3FFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_26 : 2;
    /* rsvd_26 - Bits[27:26], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam3_hienable : 2;
    /* pam3_hienable - Bits[29:28], RW_LB, default = 2'b00 
       0D4000-0D7FFF Attribute (HIENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0D4000 to 0D7FFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_30 : 2;
    /* rsvd_30 - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PAM0123_CHABC_SAD_STRUCT;


/* PAM456_CHABC_SAD_REG supported on:                                         */
/*       SKX_A0 (0x401E8044)                                                  */
/*       SKX (0x401E8044)                                                     */
/* Register default value:              0x00000000                            */
#define PAM456_CHABC_SAD_REG 0x03004044
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * PAM456 Configuration register.  All fields of this register is locked with both 
 * LTCtrlSts.ConfigLock and PAM0123_CFG.Lock are set.  This lock is bypassable with 
 * SMM writes. 
 *       
 */
typedef union {
  struct {
    UINT32 pam4_loenable : 2;
    /* pam4_loenable - Bits[1:0], RW_LB, default = 2'b00 
       0D8000-0DBFFF Attribute (LOENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0D8000 to 0DBFFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_2 : 2;
    /* rsvd_2 - Bits[3:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam4_hienable : 2;
    /* pam4_hienable - Bits[5:4], RW_LB, default = 2'b00 
       0DC000-0DFFFF Attribute (HIENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0DC000 to 0DFFFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_6 : 2;
    /* rsvd_6 - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam5_loenable : 2;
    /* pam5_loenable - Bits[9:8], RW_LB, default = 2'b00 
       0E0000-0E3FFF Attribute (LOENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0E0000 to 0E3FFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_10 : 2;
    /* rsvd_10 - Bits[11:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam5_hienable : 2;
    /* pam5_hienable - Bits[13:12], RW_LB, default = 2'b00 
       0E4000-0E7FFF Attribute (HIENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0E4000 to 0E7FFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_14 : 2;
    /* rsvd_14 - Bits[15:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam6_loenable : 2;
    /* pam6_loenable - Bits[17:16], RW_LB, default = 2'b00 
       0E8000-0EBFFF Attribute (LOENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0E8000 to 0EBFFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_18 : 2;
    /* rsvd_18 - Bits[19:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pam6_hienable : 2;
    /* pam6_hienable - Bits[21:20], RW_LB, default = 2'b00 
       0EC000-0EFFFF Attribute (HIENABLE) This field controls the steering of read and 
       write cycles that address the BIOS area from 0EC000 to 0EFFFF.  
        00: DRAM Disabled: All accesses are directed to DMI. 
        01: Read Only: All reads are sent to DRAM. All writes are forwarded to DMI. 
        10: Write Only: All writes are send to DRAM. Reads are serviced by DMI. 
        11: Normal DRAM Operation: All reads and writes are serviced by DRAM.
     */
    UINT32 rsvd_22 : 10;
    /* rsvd_22 - Bits[31:22], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} PAM456_CHABC_SAD_STRUCT;


/* EDC_CACHE_RULE_CHABC_SAD_REG supported on:                                 */
/*       SKX_A0 (0x401E8048)                                                  */
/*       SKX (0x401E8048)                                                     */
/* Register default value:              0x00000000                            */
#define EDC_CACHE_RULE_CHABC_SAD_REG 0x03004048
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 mod3 : 1;
    /* mod3 - Bits[0:0], RW_LB, default = 1'b0 
       Use mod3 in target idx calculation. Number of targets is multiple of 3
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[2:1], RW_LB, default = 2'b00 
       Mod3 Mode. FIXME: Add longer description
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} EDC_CACHE_RULE_CHABC_SAD_STRUCT;


/* MESEG_BASE_N0_CHABC_SAD_REG supported on:                                  */
/*       SKX_A0 (0x401E8050)                                                  */
/*       SKX (0x401E8050)                                                     */
/* Register default value:              0x00000000                            */
#define MESEG_BASE_N0_CHABC_SAD_REG 0x03004050
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd : 19;
    /* rsvd - Bits[18:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mebase : 13;
    /* mebase - Bits[31:19], RW_LB, default = 27'b000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Corresponds to A[45:19] of the base address memory range that is allocated to 
       the ME. 
     */
  } Bits;
  UINT32 Data;
} MESEG_BASE_N0_CHABC_SAD_STRUCT;


/* MESEG_BASE_N1_CHABC_SAD_REG supported on:                                  */
/*       SKX_A0 (0x401E8054)                                                  */
/*       SKX (0x401E8054)                                                     */
/* Register default value:              0x00000000                            */
#define MESEG_BASE_N1_CHABC_SAD_REG 0x03004054
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 mebase : 14;
    /* mebase - Bits[13:0], RW_LB, default = 27'b000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Corresponds to A[45:19] of the base address memory range that is allocated to 
       the ME. 
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MESEG_BASE_N1_CHABC_SAD_STRUCT;


/* MESEG_LIMIT_N0_CHABC_SAD_REG supported on:                                 */
/*       SKX_A0 (0x401E8058)                                                  */
/*       SKX (0x401E8058)                                                     */
/* Register default value:              0x00000000                            */
#define MESEG_LIMIT_N0_CHABC_SAD_REG 0x03004058
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 11;
    /* rsvd_0 - Bits[10:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 en : 1;
    /* en - Bits[11:11], RW_LB, default = 1'b0 
       Indicates whether the ME Stolen Memory range is enabled or not. When enabled, 
       all IA access to this range must be aborted. 
     */
    UINT32 rsvd_12 : 7;
    /* rsvd_12 - Bits[18:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 melimit : 13;
    /* melimit - Bits[31:19], RW_LB, default = 27'b000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Corresponds to A[45:19] of the limit address memory range that is allocated to 
       the ME. Minimum granularity is 1MB for this region. 
     */
  } Bits;
  UINT32 Data;
} MESEG_LIMIT_N0_CHABC_SAD_STRUCT;


/* MESEG_LIMIT_N1_CHABC_SAD_REG supported on:                                 */
/*       SKX_A0 (0x401E805C)                                                  */
/*       SKX (0x401E805C)                                                     */
/* Register default value:              0x00000000                            */
#define MESEG_LIMIT_N1_CHABC_SAD_REG 0x0300405C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 melimit : 14;
    /* melimit - Bits[13:0], RW_LB, default = 27'b000000000000000000000000000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Corresponds to A[45:19] of the limit address memory range that is allocated to 
       the ME. Minimum granularity is 1MB for this region. 
     */
    UINT32 rsvd : 18;
    /* rsvd - Bits[31:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MESEG_LIMIT_N1_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_0_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8060)                                                  */
/*       SKX (0x401E8060)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_0_CHABC_SAD_REG 0x03004060
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_0_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_0_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E8064)                                                  */
/*       SKX (0x401E8064)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_0_CHABC_SAD_REG 0x03004064
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_0_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_1_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8068)                                                  */
/*       SKX (0x401E8068)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_1_CHABC_SAD_REG 0x03004068
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_1_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_1_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E806C)                                                  */
/*       SKX (0x401E806C)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_1_CHABC_SAD_REG 0x0300406C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_1_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_2_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8070)                                                  */
/*       SKX (0x401E8070)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_2_CHABC_SAD_REG 0x03004070
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_2_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_2_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E8074)                                                  */
/*       SKX (0x401E8074)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_2_CHABC_SAD_REG 0x03004074
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_2_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_3_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8078)                                                  */
/*       SKX (0x401E8078)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_3_CHABC_SAD_REG 0x03004078
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_3_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_3_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E807C)                                                  */
/*       SKX (0x401E807C)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_3_CHABC_SAD_REG 0x0300407C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_3_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_4_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8080)                                                  */
/*       SKX (0x401E8080)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_4_CHABC_SAD_REG 0x03004080
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_4_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_4_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E8084)                                                  */
/*       SKX (0x401E8084)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_4_CHABC_SAD_REG 0x03004084
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_4_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_5_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8088)                                                  */
/*       SKX (0x401E8088)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_5_CHABC_SAD_REG 0x03004088
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_5_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_5_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E808C)                                                  */
/*       SKX (0x401E808C)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_5_CHABC_SAD_REG 0x0300408C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_5_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_6_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8090)                                                  */
/*       SKX (0x401E8090)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_6_CHABC_SAD_REG 0x03004090
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_6_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_6_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E8094)                                                  */
/*       SKX (0x401E8094)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_6_CHABC_SAD_REG 0x03004094
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_6_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_7_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8098)                                                  */
/*       SKX (0x401E8098)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_7_CHABC_SAD_REG 0x03004098
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_7_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_7_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E809C)                                                  */
/*       SKX (0x401E809C)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_7_CHABC_SAD_REG 0x0300409C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_7_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_8_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E80A0)                                                  */
/*       SKX (0x401E80A0)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_8_CHABC_SAD_REG 0x030040A0
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_8_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_8_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E80A4)                                                  */
/*       SKX (0x401E80A4)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_8_CHABC_SAD_REG 0x030040A4
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_8_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_9_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E80A8)                                                  */
/*       SKX (0x401E80A8)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_9_CHABC_SAD_REG 0x030040A8
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_9_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_9_CHABC_SAD_REG supported on:                          */
/*       SKX_A0 (0x401E80AC)                                                  */
/*       SKX (0x401E80AC)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_9_CHABC_SAD_REG 0x030040AC
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_9_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_10_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80B0)                                                  */
/*       SKX (0x401E80B0)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_10_CHABC_SAD_REG 0x030040B0
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_10_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_10_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80B4)                                                  */
/*       SKX (0x401E80B4)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_10_CHABC_SAD_REG 0x030040B4
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_10_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_11_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80B8)                                                  */
/*       SKX (0x401E80B8)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_11_CHABC_SAD_REG 0x030040B8
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_11_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_11_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80BC)                                                  */
/*       SKX (0x401E80BC)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_11_CHABC_SAD_REG 0x030040BC
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_11_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_12_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80C0)                                                  */
/*       SKX (0x401E80C0)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_12_CHABC_SAD_REG 0x030040C0
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_12_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_12_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80C4)                                                  */
/*       SKX (0x401E80C4)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_12_CHABC_SAD_REG 0x030040C4
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_12_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_13_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80C8)                                                  */
/*       SKX (0x401E80C8)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_13_CHABC_SAD_REG 0x030040C8
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_13_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_13_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80CC)                                                  */
/*       SKX (0x401E80CC)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_13_CHABC_SAD_REG 0x030040CC
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_13_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_14_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80D0)                                                  */
/*       SKX (0x401E80D0)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_14_CHABC_SAD_REG 0x030040D0
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_14_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_14_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80D4)                                                  */
/*       SKX (0x401E80D4)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_14_CHABC_SAD_REG 0x030040D4
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_14_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_15_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80D8)                                                  */
/*       SKX (0x401E80D8)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_15_CHABC_SAD_REG 0x030040D8
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_15_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_15_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80DC)                                                  */
/*       SKX (0x401E80DC)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_15_CHABC_SAD_REG 0x030040DC
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_15_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_16_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80E0)                                                  */
/*       SKX (0x401E80E0)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_16_CHABC_SAD_REG 0x030040E0
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_16_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_16_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80E4)                                                  */
/*       SKX (0x401E80E4)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_16_CHABC_SAD_REG 0x030040E4
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_16_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_17_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80E8)                                                  */
/*       SKX (0x401E80E8)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_17_CHABC_SAD_REG 0x030040E8
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_17_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_17_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80EC)                                                  */
/*       SKX (0x401E80EC)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_17_CHABC_SAD_REG 0x030040EC
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_17_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_18_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80F0)                                                  */
/*       SKX (0x401E80F0)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_18_CHABC_SAD_REG 0x030040F0
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_18_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_18_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80F4)                                                  */
/*       SKX (0x401E80F4)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_18_CHABC_SAD_REG 0x030040F4
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_18_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_19_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E80F8)                                                  */
/*       SKX (0x401E80F8)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_19_CHABC_SAD_REG 0x030040F8
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_19_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_19_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E80FC)                                                  */
/*       SKX (0x401E80FC)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_19_CHABC_SAD_REG 0x030040FC
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_19_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_20_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E8100)                                                  */
/*       SKX (0x401E8100)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_20_CHABC_SAD_REG 0x03004100
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_20_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_20_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E8104)                                                  */
/*       SKX (0x401E8104)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_20_CHABC_SAD_REG 0x03004104
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_20_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_21_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E8108)                                                  */
/*       SKX (0x401E8108)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_21_CHABC_SAD_REG 0x03004108
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_21_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_21_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E810C)                                                  */
/*       SKX (0x401E810C)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_21_CHABC_SAD_REG 0x0300410C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_21_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_22_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E8110)                                                  */
/*       SKX (0x401E8110)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_22_CHABC_SAD_REG 0x03004110
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_22_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_22_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E8114)                                                  */
/*       SKX (0x401E8114)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_22_CHABC_SAD_REG 0x03004114
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_22_CHABC_SAD_STRUCT;


/* DRAM_RULE_CFG_23_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E8118)                                                  */
/*       SKX (0x401E8118)                                                     */
/* Register default value:              0x00000000                            */
#define DRAM_RULE_CFG_23_CHABC_SAD_REG 0x03004118
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 
 *       24 instances of DRAM_RULE_CFG, one for each of the existing DRAM decoders. 
 * Each of the DRAM_RULE_CFG has a corresponding INTERLEAVE_LIST_CFG CR, denoting 
 * the targets for the specific decoder. 
 *       
 */
typedef union {
  struct {
    UINT32 rule_enable : 1;
    /* rule_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable for this DRAM rule.
     */
    UINT32 interleave_mode : 2;
    /* interleave_mode - Bits[2:1], RW_LB, default = 1'b0 
       DRAM rule interleave mode. If a dram_rule hits a 3 bit number is used to index 
       into the corresponding interleave_list to determain which package the DRAM 
       belongs to. This mode selects how that number is computed.  
        00: Address bits {8,7,6}. 
        01: Address bits {10,9,8}. 
        10: Address bits {14,13,12}. 
        11: Address bits {32,31,30}
     */
    UINT32 attr : 2;
    /* attr - Bits[4:3], RW_LB, default = 2'b00 
       00 - DRAM, 01 - MMCFG , 10 - NXM 
     */
    UINT32 mod3_asamod2 : 2;
    /* mod3_asamod2 - Bits[6:5], RW_LB, default = 1'b0 
       2 bit field used to indicate if mod3 or mod2 has to be used. For the mod2 case, 
       several options available, depending on how the mod2 result is mapped to 2 of 
       the 3 possible channels.  
               00->use mod3;
               01->use mod2, channels 0,1;
               10->use mod2, channels 1,2;
               11->use mod2, channels 0,2;
               
     */
    UINT32 limit : 20;
    /* limit - Bits[26:7], RW_LB, default = 20'b00000000000000000000 
       This correspond to Addr[45:26] of the DRAM rule top limit address. Must be 
       strickly greater then previous rule, even if this rule is disabled, unless this 
       rule and all following rules are disabled. Lower limit is the previous rule (or 
       0 if this is the first rule) 
     */
    UINT32 mod3 : 1;
    /* mod3 - Bits[27:27], RW_LB, default = 1'b0 
       Use mod3/mod2 in target idx calculation.
     */
    UINT32 cacheable : 1;
    /* cacheable - Bits[28:28], RW_LB, default = 1'b0 
       KNL only - address range can be cached at the EDC. Used in MSC and Hybrid modes
     */
    UINT32 edram : 1;
    /* edram - Bits[29:29], RW_LB, default = 1'b0 
       KNL only - specifies if the local target is eDRAM rather than an iMC.  That will 
       be set when eDRAM is used as flat-memory. 
     */
    UINT32 mod3_mode : 2;
    /* mod3_mode - Bits[31:30], RW_LB, default = 2'b00 
       Define the range of bits used for the mod3/mod2 calculation.
               00->Compute mod3/mod2 over PA[45:6]
               01->Compute mod3/mod2 over PA[45:8]
               10->Compute mod3/mod2 over PA[45:12]
               11->Unused
               
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE_CFG_23_CHABC_SAD_STRUCT;


/* INTERLEAVE_LIST_CFG_23_CHABC_SAD_REG supported on:                         */
/*       SKX_A0 (0x401E811C)                                                  */
/*       SKX (0x401E811C)                                                     */
/* Register default value:              0x88888888                            */
#define INTERLEAVE_LIST_CFG_23_CHABC_SAD_REG 0x0300411C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       Interleave list target.  Encoding is as follows:
                   4b'0xyz  -  target is a remote socket with NodeId=3b'xyz
                   4b'1xyz  -  target is a local memory controller which maps to 
       MC_ROUTE_TABLE index 3b'xyz 
                   Note:  in the 4b'1xyz case, if mod3==1, the MC_ROUTE_TABLE index is 
       actually {mod3[1:0],z} (i.e., xy is ignored) 
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST_CFG_23_CHABC_SAD_STRUCT;


/* MIGRATION_LIST_CFG_0_CHABC_SAD_REG supported on:                           */
/*       SKX_A0 (0x401E8204)                                                  */
/*       SKX (0x401E8204)                                                     */
/* Register default value:              0x00000000                            */
#define MIGRATION_LIST_CFG_0_CHABC_SAD_REG 0x03004204
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 1-bit per SAD target per DRAM rule to indicate if the memory range is a 
 * migration target. With 8 SAD targets per DRAM rule, and a total of 24 DRAM 
 * rules, there are 192-bits to indicate if the memory range is a migration target. 
 * These are packed into 6 32-bit CSRs. 8 consecutive bits in each CSR represents 8 
 * SAD targets per DRAM rule. Note that these bits have to be programmed 
 * correspondingly between the master and slave to accomplish migration.  
 */
typedef union {
  struct {
    UINT32 migration0 : 8;
    /* migration0 - Bits[7:0], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 0, 4, 8, 12, 16, 20
     */
    UINT32 migration1 : 8;
    /* migration1 - Bits[15:8], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 1, 5, 9, 13, 17, 21
     */
    UINT32 migration2 : 8;
    /* migration2 - Bits[23:16], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 2, 6, 10, 14, 18, 22
     */
    UINT32 migration3 : 8;
    /* migration3 - Bits[31:24], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 3, 7, 11, 15, 19, 23
     */
  } Bits;
  UINT32 Data;
} MIGRATION_LIST_CFG_0_CHABC_SAD_STRUCT;


/* MIGRATION_LIST_CFG_1_CHABC_SAD_REG supported on:                           */
/*       SKX_A0 (0x401E8208)                                                  */
/*       SKX (0x401E8208)                                                     */
/* Register default value:              0x00000000                            */
#define MIGRATION_LIST_CFG_1_CHABC_SAD_REG 0x03004208
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 1-bit per SAD target per DRAM rule to indicate if the memory range is a 
 * migration target. With 8 SAD targets per DRAM rule, and a total of 24 DRAM 
 * rules, there are 192-bits to indicate if the memory range is a migration target. 
 * These are packed into 6 32-bit CSRs. 8 consecutive bits in each CSR represents 8 
 * SAD targets per DRAM rule. Note that these bits have to be programmed 
 * correspondingly between the master and slave to accomplish migration.  
 */
typedef union {
  struct {
    UINT32 migration0 : 8;
    /* migration0 - Bits[7:0], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 0, 4, 8, 12, 16, 20
     */
    UINT32 migration1 : 8;
    /* migration1 - Bits[15:8], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 1, 5, 9, 13, 17, 21
     */
    UINT32 migration2 : 8;
    /* migration2 - Bits[23:16], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 2, 6, 10, 14, 18, 22
     */
    UINT32 migration3 : 8;
    /* migration3 - Bits[31:24], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 3, 7, 11, 15, 19, 23
     */
  } Bits;
  UINT32 Data;
} MIGRATION_LIST_CFG_1_CHABC_SAD_STRUCT;


/* MIGRATION_LIST_CFG_2_CHABC_SAD_REG supported on:                           */
/*       SKX_A0 (0x401E820C)                                                  */
/*       SKX (0x401E820C)                                                     */
/* Register default value:              0x00000000                            */
#define MIGRATION_LIST_CFG_2_CHABC_SAD_REG 0x0300420C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 1-bit per SAD target per DRAM rule to indicate if the memory range is a 
 * migration target. With 8 SAD targets per DRAM rule, and a total of 24 DRAM 
 * rules, there are 192-bits to indicate if the memory range is a migration target. 
 * These are packed into 6 32-bit CSRs. 8 consecutive bits in each CSR represents 8 
 * SAD targets per DRAM rule. Note that these bits have to be programmed 
 * correspondingly between the master and slave to accomplish migration.  
 */
typedef union {
  struct {
    UINT32 migration0 : 8;
    /* migration0 - Bits[7:0], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 0, 4, 8, 12, 16, 20
     */
    UINT32 migration1 : 8;
    /* migration1 - Bits[15:8], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 1, 5, 9, 13, 17, 21
     */
    UINT32 migration2 : 8;
    /* migration2 - Bits[23:16], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 2, 6, 10, 14, 18, 22
     */
    UINT32 migration3 : 8;
    /* migration3 - Bits[31:24], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 3, 7, 11, 15, 19, 23
     */
  } Bits;
  UINT32 Data;
} MIGRATION_LIST_CFG_2_CHABC_SAD_STRUCT;


/* MIGRATION_LIST_CFG_3_CHABC_SAD_REG supported on:                           */
/*       SKX_A0 (0x401E8210)                                                  */
/*       SKX (0x401E8210)                                                     */
/* Register default value:              0x00000000                            */
#define MIGRATION_LIST_CFG_3_CHABC_SAD_REG 0x03004210
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 1-bit per SAD target per DRAM rule to indicate if the memory range is a 
 * migration target. With 8 SAD targets per DRAM rule, and a total of 24 DRAM 
 * rules, there are 192-bits to indicate if the memory range is a migration target. 
 * These are packed into 6 32-bit CSRs. 8 consecutive bits in each CSR represents 8 
 * SAD targets per DRAM rule. Note that these bits have to be programmed 
 * correspondingly between the master and slave to accomplish migration.  
 */
typedef union {
  struct {
    UINT32 migration0 : 8;
    /* migration0 - Bits[7:0], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 0, 4, 8, 12, 16, 20
     */
    UINT32 migration1 : 8;
    /* migration1 - Bits[15:8], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 1, 5, 9, 13, 17, 21
     */
    UINT32 migration2 : 8;
    /* migration2 - Bits[23:16], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 2, 6, 10, 14, 18, 22
     */
    UINT32 migration3 : 8;
    /* migration3 - Bits[31:24], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 3, 7, 11, 15, 19, 23
     */
  } Bits;
  UINT32 Data;
} MIGRATION_LIST_CFG_3_CHABC_SAD_STRUCT;


/* MIGRATION_LIST_CFG_4_CHABC_SAD_REG supported on:                           */
/*       SKX_A0 (0x401E8214)                                                  */
/*       SKX (0x401E8214)                                                     */
/* Register default value:              0x00000000                            */
#define MIGRATION_LIST_CFG_4_CHABC_SAD_REG 0x03004214
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 1-bit per SAD target per DRAM rule to indicate if the memory range is a 
 * migration target. With 8 SAD targets per DRAM rule, and a total of 24 DRAM 
 * rules, there are 192-bits to indicate if the memory range is a migration target. 
 * These are packed into 6 32-bit CSRs. 8 consecutive bits in each CSR represents 8 
 * SAD targets per DRAM rule. Note that these bits have to be programmed 
 * correspondingly between the master and slave to accomplish migration.  
 */
typedef union {
  struct {
    UINT32 migration0 : 8;
    /* migration0 - Bits[7:0], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 0, 4, 8, 12, 16, 20
     */
    UINT32 migration1 : 8;
    /* migration1 - Bits[15:8], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 1, 5, 9, 13, 17, 21
     */
    UINT32 migration2 : 8;
    /* migration2 - Bits[23:16], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 2, 6, 10, 14, 18, 22
     */
    UINT32 migration3 : 8;
    /* migration3 - Bits[31:24], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 3, 7, 11, 15, 19, 23
     */
  } Bits;
  UINT32 Data;
} MIGRATION_LIST_CFG_4_CHABC_SAD_STRUCT;


/* MIGRATION_LIST_CFG_5_CHABC_SAD_REG supported on:                           */
/*       SKX_A0 (0x401E8218)                                                  */
/*       SKX (0x401E8218)                                                     */
/* Register default value:              0x00000000                            */
#define MIGRATION_LIST_CFG_5_CHABC_SAD_REG 0x03004218
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * 1-bit per SAD target per DRAM rule to indicate if the memory range is a 
 * migration target. With 8 SAD targets per DRAM rule, and a total of 24 DRAM 
 * rules, there are 192-bits to indicate if the memory range is a migration target. 
 * These are packed into 6 32-bit CSRs. 8 consecutive bits in each CSR represents 8 
 * SAD targets per DRAM rule. Note that these bits have to be programmed 
 * correspondingly between the master and slave to accomplish migration.  
 */
typedef union {
  struct {
    UINT32 migration0 : 8;
    /* migration0 - Bits[7:0], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 0, 4, 8, 12, 16, 20
     */
    UINT32 migration1 : 8;
    /* migration1 - Bits[15:8], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 1, 5, 9, 13, 17, 21
     */
    UINT32 migration2 : 8;
    /* migration2 - Bits[23:16], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 2, 6, 10, 14, 18, 22
     */
    UINT32 migration3 : 8;
    /* migration3 - Bits[31:24], RW_LB, default = 8'b00000000 
       IsMigrationTarget bits for DRAM rules 3, 7, 11, 15, 19, 23
     */
  } Bits;
  UINT32 Data;
} MIGRATION_LIST_CFG_5_CHABC_SAD_STRUCT;


/* CHA_SNC_CONFIG_CHABC_SAD_REG supported on:                                 */
/*       SKX_A0 (0x401E8220)                                                  */
/*       SKX (0x401E8220)                                                     */
/* Register default value:              0x00000000                            */
#define CHA_SNC_CONFIG_CHABC_SAD_REG 0x03004220
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * SNC Configuration, used for PRE/PCLS.  Relevant values should match those in 
 * SNC_CONFIG register. 
 */
typedef union {
  struct {
    UINT32 full_snc_en : 1;
    /* full_snc_en - Bits[0:0], RWS, default = 1'b0 
       full snc enable
     */
    UINT32 snc_ind_en : 1;
    /* snc_ind_en - Bits[1:1], RWS, default = 1'b0 
       SNC IND enable
     */
    UINT32 num_snc_clu : 2;
    /* num_snc_clu - Bits[3:2], RWS, default = 2'b00 
       Num of SNC Clusters
     */
    UINT32 sncmidpointid : 6;
    /* sncmidpointid - Bits[9:4], RWS, default = 2'b00 
       Last ring ID that is in the lower SNC cluster.
     */
    UINT32 rsvd : 22;
    /* rsvd - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CHA_SNC_CONFIG_CHABC_SAD_STRUCT;


/* CBO_ISOC_CONFIG_CHABC_SAD_REG supported on:                                */
/*       SKX_A0 (0x401E8244)                                                  */
/*       SKX (0x401E8244)                                                     */
/* Register default value:              0x00000000                            */
#define CBO_ISOC_CONFIG_CHABC_SAD_REG 0x03004244
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 isoc_enable : 1;
    /* isoc_enable - Bits[0:0], RW_LB, default = 1'b0 
       Enable ISOC mode. This will be used for TOR pipeline to reserve TOR entries for 
       ISOC. 
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} CBO_ISOC_CONFIG_CHABC_SAD_STRUCT;


/* CBO_COH_CONFIG_CHABC_SAD_REG supported on:                                 */
/*       SKX_A0 (0x401E8250)                                                  */
/*       SKX (0x401E8250)                                                     */
/* Register default value:              0x0009ACA0                            */
#define CBO_COH_CONFIG_CHABC_SAD_REG 0x03004250
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 ego : 1;
    /* ego - Bits[0:0], RW_LB, default = 1'b0 
       Enable CHA Early GO mode for RFO
     */
    UINT32 llc_disable : 1;
    /* llc_disable - Bits[1:1], RW_LB, default = 1'b0 
       Disable LLC
     */
    UINT32 haonclockpipeopt : 1;
    /* haonclockpipeopt - Bits[2:2], RW_LB, default = 1'b0 
       Enables ON clock pipeline optimization for HA
     */
    UINT32 disablecdf_rem : 1;
    /* disablecdf_rem - Bits[3:3], RW_LB, default = 1'b0 
       Disable CDF snoops (alias to non-CDF snoop) for Remote requests
     */
    UINT32 disablecdf_loc : 1;
    /* disablecdf_loc - Bits[4:4], RW_LB, default = 1'b0 
       Disable CDF snoops (alias to non-CDF snoop) for Local requests
     */
    UINT32 enableearlyqpireq : 1;
    /* enableearlyqpireq - Bits[5:5], RW_LB, default = 1'b1 
       Enable sending RdInvOwn/InvItoE in parallel with core snoops
     */
    UINT32 disllctosfmigdrdcrd : 1;
    /* disllctosfmigdrdcrd - Bits[6:6], RW_LB, default = 1'b0 
       Disable LLC to SF migration for DRd and CRd flow
     */
    UINT32 llcsharedrdcrd : 1;
    /* llcsharedrdcrd - Bits[7:7], RW_LB, default = 1'b1 
       Enable migration from SF to LLC and to leave shared lines in the LLC for Drd and 
       Crd 
     */
    UINT32 drdgosonem : 1;
    /* drdgosonem - Bits[8:8], RW_LB, default = 1'b0 
       Enable GOS on E/M state for DRD
     */
    UINT32 sendwbedata : 1;
    /* sendwbedata - Bits[9:9], RW_LB, default = 1'b0 
       For DRd, CRd, and PCIRdCur with LLC disabled send a WbEData instead of WbSData 
       when writing back M data 
     */
    UINT32 mtoibias : 1;
    /* mtoibias - Bits[10:10], RW_LB, default = 1'b1 
       Use MtoI policy as opposed to MtoS policy
     */
    UINT32 downgradeftos : 1;
    /* downgradeftos - Bits[11:11], RW_LB, default = 1'b1 
       Downgrade all F state to S state
     */
    UINT32 biasfwd : 1;
    /* biasfwd - Bits[12:12], RW_LB, default = 1'b0 
       Enable RspFwdIWB mode, BiasFwdDoubleData &amp; BiasFwdLocalHome are used for 
       further qualifications. Table below shows the qualifications and 
       behaviors:BiasFwd   BiasFwdDoubleData   BiasFwdLocalHome   Behaviour for 
       SnpData/Code with LLC=M    
          0   x   x   Original behaviour. No C2C. Implicit writeback to HA   
          1   0   0   Fwd only when Home!=Requestor AND Home!=Local   
          1   0   1   Fwd only when Home!=Requestor   
          1   1   0   Fwd only when Home!=Local   
          1   1   1   Fwd for all cases
     */
    UINT32 waitfordatacmp : 1;
    /* waitfordatacmp - Bits[13:13], RW_LB, default = 1'b1 
       Wait for Data+Cmp before sending through Cpipe. if 0h, will do it separately.
     */
    UINT32 morphpcommittowcil : 1;
    /* morphpcommittowcil - Bits[14:14], RW_LB, default = 1'b0 
       Defeature bit to morph PCommit request to a WCiL 
     */
    UINT32 biasfwddoubledata : 1;
    /* biasfwddoubledata - Bits[15:15], RW_LB, default = 1'b1 
       RspFwdIWB when HOME!=Local (BiasFwd must be enabled)
     */
    UINT32 biasfwdlocalhome : 1;
    /* biasfwdlocalhome - Bits[16:16], RW_LB, default = 1'b1 
       RspFwdIWB when HOME!=Requestor (BiasFwd must be enabled).
     */
    UINT32 suppressinvitoeonwrite : 1;
    /* suppressinvitoeonwrite - Bits[17:17], RW_LB, default = 1'b0 
       Do not send and InvItoE for WCiL, WiL, WCiLF
     */
    UINT32 llcmissendrd : 1;
    /* llcmissendrd - Bits[18:18], RW_LB, default = 1'b0 
       Enable LLC Miss message for DRd
     */
    UINT32 llcmissendrdpte : 1;
    /* llcmissendrdpte - Bits[19:19], RW_LB, default = 1'b1 
       Enable LLC Miss message for DRdPTE
     */
    UINT32 disisoctorres : 1;
    /* disisoctorres - Bits[20:20], RW_LB, default = 1'b0 
       Disable TOR ISOC reservation
     */
    UINT32 disisocegrres : 1;
    /* disisocegrres - Bits[21:21], RW_LB, default = 1'b0 
       Disable ISOC Egress Reservation
     */
    UINT32 disselfsnpindtocore : 1;
    /* disselfsnpindtocore - Bits[22:22], RW_LB, default = 1'b0 
       When it is set, the self-snoop indication to core is disabled for the core DUE 
       improvement feature 
     */
    UINT32 enablexorbasedktilink : 1;
    /* enablexorbasedktilink - Bits[23:23], RW_LB, default = 1'b0 
       Use PA6 xor PA12 value to decide the Intel UPI link to use in 2S-2K case
     */
    UINT32 swapktilinkintlv : 1;
    /* swapktilinkintlv - Bits[24:24], RW_LB, default = 1'b0 
       This is required to support twisted board topologies where link0 on one socket 
       is connected to link1 on the other socket in 2S-2Link configurations and with 
       EnableXorBasedKTILink set. 
     */
    UINT32 rsvd_25 : 1;
    /* rsvd_25 - Bits[25:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 frcfwdinv : 1;
    /* frcfwdinv - Bits[26:26], RW_LB, default = 1'b0 
       This bit is used to alias all conflict flows to FwdInvItoE behaviour 
     */
    UINT32 conddisablef : 1;
    /* conddisablef - Bits[27:27], RW_LB, default = 1'b0 
       When set, downgrade from F to S without forwarding on 
       SnpData/Code/Cur/DataMigratory from a remote home 
     */
    UINT32 localkeepffwds : 1;
    /* localkeepffwds - Bits[28:28], RW_LB, default = 1'b0 
       When set, keep the line in F state and forward S state on 
       SnpData/Code/Cur/DataMigratory from a local home 
     */
    UINT32 rsvd_29 : 1;
    /* rsvd_29 - Bits[29:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 micwcilfen : 1;
    /* micwcilfen - Bits[30:30], RW_LB, default = 1'b0 
       When it is set, InvItoM will be issued for remote WCiLF
     */
    UINT32 ha_disable : 1;
    /* ha_disable - Bits[31:31], RW_LB, default = 1'b0 
       When it is set, implies local HA is never active and associated reserved TOR 
       entries could be reclaimed 
     */
  } Bits;
  UINT32 Data;
} CBO_COH_CONFIG_CHABC_SAD_STRUCT;


/* QPI_TIMEOUT_CTRL_CHABC_SAD_REG supported on:                               */
/*       SKX_A0 (0x401E825C)                                                  */
/*       SKX (0x401E825C)                                                     */
/* Register default value:              0x1FFFFFFE                            */
#define QPI_TIMEOUT_CTRL_CHABC_SAD_REG 0x0300425C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * Intel UPI/TOR Timeout Configuration
 */
typedef union {
  struct {
    UINT32 enabletortimeout : 1;
    /* enabletortimeout - Bits[0:0], RWS_LB, default = 1'b0 
       Enable Intel UPI Timeout for certain classes of Intel UPI transactions.
                   A unique CHA timeout range can be programmed based on the timeout 
       level of the transaction (refer to Intel UPI specifications for further details 
       of the mapping of levels to the transaction type): 
                   Actual CHA timeout value varies and could be any value between lower 
       bound and a upper bound and is defined as follows: 
                   Upper bound of the CHA timeout:- [2^(N+11)*Level_M_offset]/F
                   Lower bound of the CHA timeout:- [2^(N+11)*(Level_M_offset-1)]/F
                   Where:
                      - N is the number of bits in the upper base counter (defined by 
       entry_ctr_inc_ctl). 
                      - Level_M_offset (here M=0,1,3,4,6) is the value programed in 
       levelM_offset bit-field. 
                      - F is the CPU clock frequency.  
     */
    UINT32 entry_ctr_inc_ctl : 3;
    /* entry_ctr_inc_ctl - Bits[3:1], RWS_LB, default = 3'b111 
       Indication of which bit of upper base counter increments entry counter.  Value 
       of the upper base counter maps to actual timeout intervals as follows (assuming 
       a 1.0 GHz base frequency 
                   000: no upper-base counter is used (~ 2.1*offset us timeout @1.0Ghz)
                   001: 2-bit upper-base counter is used (~ 8.2*offset us timeout 
       @1.0Ghz) 
                   010: 5-bit upper-base counter is used (~ 66*offset us timeout 
       @1.0Ghz) 
                   011: 8-bit upper-base counter is used (~ 524*offset us timeout 
       @1.0Ghz) 
                   100: 11-bit upper-base counter is used (~ 4.2*offset ms timeout 
       @1.0Ghz) 
                   101: 15-bit upper-base counter is used (~ 67*offset ms timeout 
       @1.0Ghz) 
                   110: 18-bit upper-base counter is used (~ 537*offset ms timeout 
       @1.0Ghz) 
                   111: 20-bit upper-base counter is used (~ 2.2*offset second timeout 
       @1.0Ghz)   
     */
    UINT32 level0_offset : 5;
    /* level0_offset - Bits[8:4], RWS_LB, default = 5'd31 
        Encoded Intel UPI Level 0 Timeout offset. Offsets must be > 1. 
     */
    UINT32 level1_offset : 5;
    /* level1_offset - Bits[13:9], RWS_LB, default = 5'd31 
        Encoded Intel UPI Level 1 Timeout offset. Offsets must be > 1. 
               Level1-Level6 offsets must be >= Level0 offset. 
     */
    UINT32 level2_offset : 5;
    /* level2_offset - Bits[18:14], RWS_LB, default = 5'd31 
        Encoded Intel UPI Level 2 Timeout offset. Offsets must be > 1. 
               Level1-Level6 offsets must be >= Level0 offset. 
     */
    UINT32 level3_offset : 5;
    /* level3_offset - Bits[23:19], RWS_LB, default = 5'd31 
        Encoded Intel UPI Level 3 Timeout offset. Offsets must be > 1. 
               Level1-Level6 offsets must be >= Level0 offset. 
     */
    UINT32 level4_offset : 5;
    /* level4_offset - Bits[28:24], RWS_LB, default = 5'd31 
        Encoded Intel UPI Level 4 Timeout offset. Offsets must be > 1. 
               Level1-Level6 offsets must be >= Level0 offset. 
     */
    UINT32 rsvd : 3;
    /* rsvd - Bits[31:29], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} QPI_TIMEOUT_CTRL_CHABC_SAD_STRUCT;


/* QPI_TIMEOUT_CTRL2_CHABC_SAD_REG supported on:                              */
/*       SKX_A0 (0x401E8260)                                                  */
/*       SKX (0x401E8260)                                                     */
/* Register default value:              0x0000001F                            */
#define QPI_TIMEOUT_CTRL2_CHABC_SAD_REG 0x03004260
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 * Intel UPI/TOR Timeout Configuration 2
 */
typedef union {
  struct {
    UINT32 level6_offset : 5;
    /* level6_offset - Bits[4:0], RWS_LB, default = 5'd31 
        Encoded Intel UPI Level 6 Timeout offset. Offsets must be > 1. 
               Level1-Level6 offsets must be >= Level0 offset. 
     */
    UINT32 disableretrytracking : 1;
    /* disableretrytracking - Bits[5:5], RWS_LB, default = 1'b0 
        Disable TOR timeout tracking on retry requests (before they have been acked) 
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} QPI_TIMEOUT_CTRL2_CHABC_SAD_STRUCT;


/* NUM_OF_HOPS_CHABC_SAD_REG supported on:                                    */
/*       SKX_A0 (0x401E8280)                                                  */
/*       SKX (0x401E8280)                                                     */
/* Register default value:              0x00005555                            */
#define NUM_OF_HOPS_CHABC_SAD_REG 0x03004280
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 *  Core responses use the number of Hops
 */
typedef union {
  struct {
    UINT32 ca0 : 2;
    /* ca0 - Bits[1:0], RW, default = 2'b01 
       HOPS for SID[2:0]=0
     */
    UINT32 ca1 : 2;
    /* ca1 - Bits[3:2], RW, default = 2'b01 
       HOPS for SID[2:0]=1
     */
    UINT32 ca2 : 2;
    /* ca2 - Bits[5:4], RW, default = 2'b01 
       HOPS for SID[2:0]=2
     */
    UINT32 ca3 : 2;
    /* ca3 - Bits[7:6], RW, default = 2'b01 
       HOPS for SID[2:0]=3
     */
    UINT32 ca4 : 2;
    /* ca4 - Bits[9:8], RW, default = 2'b01 
       HOPS for SID[2:0]=4
     */
    UINT32 ca5 : 2;
    /* ca5 - Bits[11:10], RW, default = 2'b01 
       HOPS for SID[2:0]=5
     */
    UINT32 ca6 : 2;
    /* ca6 - Bits[13:12], RW, default = 2'b01 
       HOPS for SID[2:0]=6
     */
    UINT32 ca7 : 2;
    /* ca7 - Bits[15:14], RW, default = 2'b01 
       HOPS for SID[2:0]=7
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} NUM_OF_HOPS_CHABC_SAD_STRUCT;


/* TWO_LM_CONFIG_CHABC_SAD_REG supported on:                                  */
/*       SKX_A0 (0x401E8288)                                                  */
/*       SKX (0x401E8288)                                                     */
/* Register default value:              0x00003FFF                            */
#define TWO_LM_CONFIG_CHABC_SAD_REG 0x03004288
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 mask : 14;
    /* mask - Bits[13:0], RW_LB, default = 14'h3FFF 
       Two Level Memory Address Mask. This allows masking out address bits [45:32] for 
       Near memory set conflict detection. 
     */
    UINT32 rsvd_14 : 6;
    /* rsvd_14 - Bits[19:14], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 enable : 1;
    /* enable - Bits[20:20], RW_LB, default = 1'b0 
       Overall Two LM Enable
     */
    UINT32 rsvd_21 : 11;
    /* rsvd_21 - Bits[31:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} TWO_LM_CONFIG_CHABC_SAD_STRUCT;


/* DBP_CONFIG_CHABC_SAD_REG supported on:                                     */
/*       SKX_A0 (0x401E828C)                                                  */
/*       SKX (0x401E828C)                                                     */
/* Register default value:              0x00000040                            */
#define DBP_CONFIG_CHABC_SAD_REG 0x0300428C
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 enable : 1;
    /* enable - Bits[0:0], RW, default = 1'b0 
       Overall DBP enable
     */
    UINT32 cleanevictalwaysdead : 1;
    /* cleanevictalwaysdead - Bits[1:1], RW, default = 1'b0 
       CleanEvictAlwaysDead
     */
    UINT32 modifiedevictalwaysdead : 1;
    /* modifiedevictalwaysdead - Bits[2:2], RW, default = 1'b0 
       ModifiedEvictAlwaysDead
     */
    UINT32 cleanevictalwayslive : 1;
    /* cleanevictalwayslive - Bits[3:3], RW, default = 1'b0 
       CleanEvictAlwaysLive
     */
    UINT32 modifiedevictalwayslive : 1;
    /* modifiedevictalwayslive - Bits[4:4], RW, default = 1'b0 
       ModifiedEvictAlwaysLive
     */
    UINT32 disablecorecheck : 1;
    /* disablecorecheck - Bits[5:5], RW, default = 1'b0 
       DisableCoreCheck
     */
    UINT32 deadonvalidllc : 1;
    /* deadonvalidllc - Bits[6:6], RW, default = 1'b1 
       1'b1 - If Downgrade is set on follower do not fill in LLC regardless of 
       available LLC I-state ways 
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DBP_CONFIG_CHABC_SAD_STRUCT;


/* IOPORT_TARGET_LIST_CFG_0_CHABC_SAD_REG supported on:                       */
/*       SKX_A0 (0x401E82B0)                                                  */
/*       SKX (0x401E82B0)                                                     */
/* Register default value:              0x88888888                            */
#define IOPORT_TARGET_LIST_CFG_0_CHABC_SAD_REG 0x030042B0
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} IOPORT_TARGET_LIST_CFG_0_CHABC_SAD_STRUCT;


/* IOPORT_TARGET_LIST_CFG_1_CHABC_SAD_REG supported on:                       */
/*       SKX_A0 (0x401E82B4)                                                  */
/*       SKX (0x401E82B4)                                                     */
/* Register default value:              0x88888888                            */
#define IOPORT_TARGET_LIST_CFG_1_CHABC_SAD_REG 0x030042B4
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} IOPORT_TARGET_LIST_CFG_1_CHABC_SAD_STRUCT;


/* IOPORT_TARGET_LIST_CFG_2_CHABC_SAD_REG supported on:                       */
/*       SKX_A0 (0x401E82B8)                                                  */
/*       SKX (0x401E82B8)                                                     */
/* Register default value:              0x88888888                            */
#define IOPORT_TARGET_LIST_CFG_2_CHABC_SAD_REG 0x030042B8
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} IOPORT_TARGET_LIST_CFG_2_CHABC_SAD_STRUCT;


/* IOPORT_TARGET_LIST_CFG_3_CHABC_SAD_REG supported on:                       */
/*       SKX_A0 (0x401E82BC)                                                  */
/*       SKX (0x401E82BC)                                                     */
/* Register default value:              0x88888888                            */
#define IOPORT_TARGET_LIST_CFG_3_CHABC_SAD_REG 0x030042BC
/* Struct format extracted from XML file SKX\1.29.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW_LB, default = 4'b1000 
       IOPORT target.
       		  b'1xxx  Target is local IIO Stack xxx
       		  b'0xxx  Target is remote socket with NodeID xxx
     */
  } Bits;
  UINT32 Data;
} IOPORT_TARGET_LIST_CFG_3_CHABC_SAD_STRUCT;


#endif /* CHABC_SAD_h */
