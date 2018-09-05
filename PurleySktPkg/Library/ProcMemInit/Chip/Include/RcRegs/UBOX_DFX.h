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

#ifndef UBOX_DFX_h
#define UBOX_DFX_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* UBOX_DFX_DEV 8                                                             */
/* UBOX_DFX_FUN 1                                                             */

/* VID_UBOX_DFX_REG supported on:                                             */
/*       SKX_A0 (0x20041000)                                                  */
/*       SKX (0x20041000)                                                     */
/* Register default value:              0x8086                                */
#define VID_UBOX_DFX_REG 0x13012000
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} VID_UBOX_DFX_STRUCT;


/* DID_UBOX_DFX_REG supported on:                                             */
/*       SKX_A0 (0x20041002)                                                  */
/*       SKX (0x20041002)                                                     */
/* Register default value:              0x2015                                */
#define DID_UBOX_DFX_REG 0x13012002
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h2015 
        
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
} DID_UBOX_DFX_STRUCT;


/* PCICMD_UBOX_DFX_REG supported on:                                          */
/*       SKX_A0 (0x20041004)                                                  */
/*       SKX (0x20041004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_UBOX_DFX_REG 0x13012004
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} PCICMD_UBOX_DFX_STRUCT;


/* PCISTS_UBOX_DFX_REG supported on:                                          */
/*       SKX_A0 (0x20041006)                                                  */
/*       SKX (0x20041006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_UBOX_DFX_REG 0x13012006
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} PCISTS_UBOX_DFX_STRUCT;


/* RID_UBOX_DFX_REG supported on:                                             */
/*       SKX_A0 (0x10041008)                                                  */
/*       SKX (0x10041008)                                                     */
/* Register default value:              0x00                                  */
#define RID_UBOX_DFX_REG 0x13011008
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} RID_UBOX_DFX_STRUCT;


/* CCR_N0_UBOX_DFX_REG supported on:                                          */
/*       SKX_A0 (0x10041009)                                                  */
/*       SKX (0x10041009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_UBOX_DFX_REG 0x13011009
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_UBOX_DFX_STRUCT;


/* CCR_N1_UBOX_DFX_REG supported on:                                          */
/*       SKX_A0 (0x2004100A)                                                  */
/*       SKX (0x2004100A)                                                     */
/* Register default value:              0x1101                                */
#define CCR_N1_UBOX_DFX_REG 0x1301200A
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT16 sub_class : 8;
    /* sub_class - Bits[7:0], RO_V, default = 8'b00000001 
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
    /* base_class - Bits[15:8], RO_V, default = 8'b00010001 
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
} CCR_N1_UBOX_DFX_STRUCT;


/* CLSR_UBOX_DFX_REG supported on:                                            */
/*       SKX_A0 (0x1004100C)                                                  */
/*       SKX (0x1004100C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_UBOX_DFX_REG 0x1301100C
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} CLSR_UBOX_DFX_STRUCT;


/* PLAT_UBOX_DFX_REG supported on:                                            */
/*       SKX_A0 (0x1004100D)                                                  */
/*       SKX (0x1004100D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_UBOX_DFX_REG 0x1301100D
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} PLAT_UBOX_DFX_STRUCT;


/* HDR_UBOX_DFX_REG supported on:                                             */
/*       SKX_A0 (0x1004100E)                                                  */
/*       SKX (0x1004100E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_UBOX_DFX_REG 0x1301100E
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} HDR_UBOX_DFX_STRUCT;


/* BIST_UBOX_DFX_REG supported on:                                            */
/*       SKX_A0 (0x1004100F)                                                  */
/*       SKX (0x1004100F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_UBOX_DFX_REG 0x1301100F
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} BIST_UBOX_DFX_STRUCT;


/* SVID_UBOX_DFX_REG supported on:                                            */
/*       SKX_A0 (0x2004102C)                                                  */
/*       SKX (0x2004102C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_UBOX_DFX_REG 0x1301202C
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_identification_number : 16;
    /* subsystem_vendor_identification_number - Bits[15:0], RW_O, default = 16'b1000000010000110 
       The default value specifies Intel but can be set to any value once after reset.
     */
  } Bits;
  UINT16 Data;
} SVID_UBOX_DFX_STRUCT;


/* SDID_UBOX_DFX_REG supported on:                                            */
/*       SKX_A0 (0x2004102E)                                                  */
/*       SKX (0x2004102E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_UBOX_DFX_REG 0x1301202E
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 */
typedef union {
  struct {
    UINT16 subsystem_device_identification_number : 16;
    /* subsystem_device_identification_number - Bits[15:0], RW_O, default = 16'b0000000000000000 
       Assigned by the subsystem vendor to uniquely identify the subsystem
     */
  } Bits;
  UINT16 Data;
} SDID_UBOX_DFX_STRUCT;


/* CAPPTR_UBOX_DFX_REG supported on:                                          */
/*       SKX_A0 (0x10041034)                                                  */
/*       SKX (0x10041034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_UBOX_DFX_REG 0x13011034
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} CAPPTR_UBOX_DFX_STRUCT;


/* INTL_UBOX_DFX_REG supported on:                                            */
/*       SKX_A0 (0x1004103C)                                                  */
/*       SKX (0x1004103C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_UBOX_DFX_REG 0x1301103C
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} INTL_UBOX_DFX_STRUCT;


/* INTPIN_UBOX_DFX_REG supported on:                                          */
/*       SKX_A0 (0x1004103D)                                                  */
/*       SKX (0x1004103D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_UBOX_DFX_REG 0x1301103D
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} INTPIN_UBOX_DFX_STRUCT;


/* MINGNT_UBOX_DFX_REG supported on:                                          */
/*       SKX_A0 (0x1004103E)                                                  */
/*       SKX (0x1004103E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_UBOX_DFX_REG 0x1301103E
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} MINGNT_UBOX_DFX_STRUCT;


/* MAXLAT_UBOX_DFX_REG supported on:                                          */
/*       SKX_A0 (0x1004103F)                                                  */
/*       SKX (0x1004103F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_UBOX_DFX_REG 0x1301103F
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
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
} MAXLAT_UBOX_DFX_STRUCT;


/* SMM_DELAYED0_UBOX_DFX_REG supported on:                                    */
/*       SKX_A0 (0x40041040)                                                  */
/*       SKX (0x40041040)                                                     */
/* Register default value:              0x00000000                            */
#define SMM_DELAYED0_UBOX_DFX_REG 0x13014040
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * This CSR contains bit per logical processor indicating whether the logical 
 * processor is in the middle of long flow and hence will delay servicing of SMI. 
 * SMI will be serviced after the long flow completes execution. 
 * 
 *       16 cores will reside in instance 0 as such:
 *       Bit          function
 *       0            Core 0 Thread 0 is delayed
 *       1            Core 0 Thread 1 is delayed
 *       2            Core 1 Thread 0 is delayed
 *       3            ...
 *       31           Core 15 Thread 1 is delayed
 */
typedef union {
  struct {
    UINT32 log_proc : 32;
    /* log_proc - Bits[31:0], RO_V, default = 32'b00000000000000000000000000000000 
       
                 This bit will be set at the start of the flows listed below and
                 cleared at the end of the flows.
                    Patch load
                    C6 entry
                    C6 exit
                    WBINVD
                    Ratio Change/Throttle/S1
               
     */
  } Bits;
  UINT32 Data;
} SMM_DELAYED0_UBOX_DFX_STRUCT;


/* SMM_DELAYED1_UBOX_DFX_REG supported on:                                    */
/*       SKX_A0 (0x40041044)                                                  */
/*       SKX (0x40041044)                                                     */
/* Register default value:              0x00000000                            */
#define SMM_DELAYED1_UBOX_DFX_REG 0x13014044
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * This CSR contains bit per logical processor indicating
 *       whether the logical processor is in the middle of long flow and hence
 *       will delay servicing of SMI. SMI will be serviced after the long flow
 *       completes execution.
 * 
 *       12 cores will reside in instance 1 as such:
 *       Bit          function
 *       0            Core 16 Thread 0 is delayed
 *       1            Core 16 Thread 1 is delayed
 *       2            Core 17 Thread 0 is delayed
 *       3            Core 17 Thread 1 is delayed
 *       ... 	   ...
 *       23	   Core 27 Thread 1 is delayed
 * 	
 */
typedef union {
  struct {
    UINT32 log_proc : 24;
    /* log_proc - Bits[23:0], RO_V, default = 24'b000000000000000000000000 
       
                 This bit will be set at the start of the flows listed below and
                 cleared at the end of the flows.
                    Patch load
                    C6 entry
                    C6 exit
                    WBINVD
                    Ratio Change/Throttle/S1
               
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMM_DELAYED1_UBOX_DFX_STRUCT;


/* SMM_BLOCKED0_UBOX_DFX_REG supported on:                                    */
/*       SKX_A0 (0x40041048)                                                  */
/*       SKX (0x40041048)                                                     */
/* Register default value:              0xFFFFFFFF                            */
#define SMM_BLOCKED0_UBOX_DFX_REG 0x13014048
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * This CSR contains bit per logical processor indicating whether the logical 
 * processor is in state where SMIs are blocked and hence will not be able to 
 * service SMI. SMI can be serviced after the logical processor exits the state in 
 * which SMIs are blocked. 
 * 
 *       16 cores will reside in instance 0 as such:
 *       Bit          function
 *       0            Core 0 Thread 0 is blocked
 *       1            Core 0 Thread 1 is blocked
 *       2            Core 1 Thread 0 is blocked
 *       3            ...
 *       31           Core 15 Thread 1 is blocked
 */
typedef union {
  struct {
    UINT32 log_proc : 32;
    /* log_proc - Bits[31:0], RO_V, default = 32'b11111111111111111111111111111111 
       
                 This bit will be set when the logical processor is in the following
                 states
                    Wait For SIPI
                    SENTER Sleep
                    VMX Abort
                    Error Shutdown (Machine check in WFS state)
               
     */
  } Bits;
  UINT32 Data;
} SMM_BLOCKED0_UBOX_DFX_STRUCT;


/* SMM_BLOCKED1_UBOX_DFX_REG supported on:                                    */
/*       SKX_A0 (0x4004104C)                                                  */
/*       SKX (0x4004104C)                                                     */
/* Register default value:              0x00FFFFFF                            */
#define SMM_BLOCKED1_UBOX_DFX_REG 0x1301404C
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * This CSR contains bit per logical processor indicating whether the logical 
 * processor is in state where SMIs are blocked and hence will not be able to 
 * service SMI. SMI can be serviced after the logical processor exits the state in 
 * which SMIs are blocked. 
 * 
 *       12 cores will reside in instance 1 as such:
 *       Bit          function
 *       0            Core 16 Thread 0 is blocked
 *       1            Core 16 Thread 1 is blocked
 *       2            Core 17 Thread 0 is blocked
 *       3            Core 17 Thread 1 is blocked
 *       ...	   ...
 *       23	   Core 27 Thread 1 is blocked
 * 	
 */
typedef union {
  struct {
    UINT32 log_proc : 24;
    /* log_proc - Bits[23:0], RO_V, default = 24'b111111111111111111111111 
       
                 This bit will be set when the logical processor is in the following
                 states
                    Wait For SIPI
                    SENTER Sleep
                    VMX Abort
                    Error Shutdown (Machine check in WFS state)
               
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMM_BLOCKED1_UBOX_DFX_STRUCT;






/* SMM_FEATURE_CONTROL_UBOX_DFX_REG supported on:                             */
/*       SKX_A0 (0x4004107C)                                                  */
/*       SKX (0x4004107C)                                                     */
/* Register default value:              0x00000000                            */
#define SMM_FEATURE_CONTROL_UBOX_DFX_REG 0x1301407C
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * This CSR holds enable bits for on-chip storage of SMM and Code Access Check 
 * Violation features. This CSR can be written only from within SMM but can be read 
 * by SMM and non SMM code. This CSR is cleared only on cold reset. 
 */
typedef union {
  struct {
    UINT32 lock : 1;
    /* lock - Bits[0:0], RW_LB, default = 1'b0 
       
                 If Lock = 1, CPU will drop writes to the CSR
                 If Lock = 0, CPU will allow update to b[2:0]
               
     */
    UINT32 smm_cpu_save_en : 1;
    /* smm_cpu_save_en - Bits[1:1], RW_LB, default = 1'b0 
       
                 If 0, SMI/RSM will save/restore state in SMRAM
                 If 1, SMI/RSM will save/restore state from SRAM
               
     */
    UINT32 smm_code_chk_en : 1;
    /* smm_code_chk_en - Bits[2:2], RW_LB, default = 1'b0 
       
                 If 1, generate unrecoverable MCHK on code fetch outside SMRR
                 If 0, do not report any errors for code fetch outside SMRR
               
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} SMM_FEATURE_CONTROL_UBOX_DFX_STRUCT;


/* INVALIDBUSACCESS_UBOX_DFX_REG supported on:                                */
/*       SKX_A0 (0x40041080)                                                  */
/*       SKX (0x40041080)                                                     */
/* Register default value:              0x00000000                            */
#define INVALIDBUSACCESS_UBOX_DFX_REG 0x13014080
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * Logging of the first time invalid bus access on CFG. 
 */
typedef union {
  struct {
    UINT32 bus : 8;
    /* bus - Bits[7:0], RWS_V, default = 8'b00000000 
       Log the bus number when it is the first time of the invalid bus access on CFG
     */
    UINT32 invalidbusaccess : 1;
    /* invalidbusaccess - Bits[8:8], RWS_V, default = 1'b0 
       Set the bit when it is the first time of the invalid bus access on CFG
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} INVALIDBUSACCESS_UBOX_DFX_STRUCT;


/* BIST_RESULTS_UBOX_DFX_REG supported on:                                    */
/*       SKX_A0 (0x400410B4)                                                  */
/*       SKX (0x400410B4)                                                     */
/* Register default value:              0x00000000                            */
#define BIST_RESULTS_UBOX_DFX_REG 0x130140B4
/* Struct format extracted from XML file SKX\0.8.1.CFG.xml.
 * Config read-only access to core BIST results 
 */
typedef union {
  struct {
    UINT32 results : 28;
    /* results - Bits[27:0], RO_V, default = 28'd0 
       0 - BIST failure, 1 - BIST pass
     */
    UINT32 rsvd : 4;
    /* rsvd - Bits[31:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BIST_RESULTS_UBOX_DFX_STRUCT;




#endif /* UBOX_DFX_h */
