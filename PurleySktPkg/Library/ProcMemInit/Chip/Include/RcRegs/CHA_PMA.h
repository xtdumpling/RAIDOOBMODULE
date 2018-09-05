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

#ifndef CHA_PMA_h
#define CHA_PMA_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* CHA_PMA_DEV 14                                                             */
/* CHA_PMA_FUN 0                                                              */

/* VID_CHA_PMA_REG supported on:                                              */
/*       SKX_A0 (0x20170000)                                                  */
/*       SKX (0x20170000)                                                     */
/* Register default value:              0x8086                                */
#define VID_CHA_PMA_REG 0x01002000
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} VID_CHA_PMA_STRUCT;


/* DID_CHA_PMA_REG supported on:                                              */
/*       SKX_A0 (0x20170002)                                                  */
/*       SKX (0x20170002)                                                     */
/* Register default value:              0x208E                                */
#define DID_CHA_PMA_REG 0x01002002
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * PCI Device Identification Number
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h208E 
        
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
} DID_CHA_PMA_STRUCT;


/* PCICMD_CHA_PMA_REG supported on:                                           */
/*       SKX_A0 (0x20170004)                                                  */
/*       SKX (0x20170004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_CHA_PMA_REG 0x01002004
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} PCICMD_CHA_PMA_STRUCT;


/* PCISTS_CHA_PMA_REG supported on:                                           */
/*       SKX_A0 (0x20170006)                                                  */
/*       SKX (0x20170006)                                                     */
/* Register default value:              0x0000                                */
#define PCISTS_CHA_PMA_REG 0x01002006
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} PCISTS_CHA_PMA_STRUCT;


/* RID_CHA_PMA_REG supported on:                                              */
/*       SKX_A0 (0x10170008)                                                  */
/*       SKX (0x10170008)                                                     */
/* Register default value:              0x00                                  */
#define RID_CHA_PMA_REG 0x01001008
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} RID_CHA_PMA_STRUCT;


/* CCR_N0_CHA_PMA_REG supported on:                                           */
/*       SKX_A0 (0x10170009)                                                  */
/*       SKX (0x10170009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_CHA_PMA_REG 0x01001009
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * PCIe header ClassCode register
 */
typedef union {
  struct {
    UINT8 register_level_programming_interface : 8;
    /* register_level_programming_interface - Bits[7:0], RO_V, default = 8'b00000000  */
  } Bits;
  UINT8 Data;
} CCR_N0_CHA_PMA_STRUCT;


/* CCR_N1_CHA_PMA_REG supported on:                                           */
/*       SKX_A0 (0x2017000A)                                                  */
/*       SKX (0x2017000A)                                                     */
/* Register default value:              0x0880                                */
#define CCR_N1_CHA_PMA_REG 0x0100200A
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} CCR_N1_CHA_PMA_STRUCT;


/* CLSR_CHA_PMA_REG supported on:                                             */
/*       SKX_A0 (0x1017000C)                                                  */
/*       SKX (0x1017000C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_CHA_PMA_REG 0x0100100C
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} CLSR_CHA_PMA_STRUCT;


/* PLAT_CHA_PMA_REG supported on:                                             */
/*       SKX_A0 (0x1017000D)                                                  */
/*       SKX (0x1017000D)                                                     */
/* Register default value:              0x00                                  */
#define PLAT_CHA_PMA_REG 0x0100100D
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} PLAT_CHA_PMA_STRUCT;


/* HDR_CHA_PMA_REG supported on:                                              */
/*       SKX_A0 (0x1017000E)                                                  */
/*       SKX (0x1017000E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_CHA_PMA_REG 0x0100100E
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} HDR_CHA_PMA_STRUCT;


/* BIST_CHA_PMA_REG supported on:                                             */
/*       SKX_A0 (0x1017000F)                                                  */
/*       SKX (0x1017000F)                                                     */
/* Register default value:              0x00                                  */
#define BIST_CHA_PMA_REG 0x0100100F
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} BIST_CHA_PMA_STRUCT;


/* SVID_CHA_PMA_REG supported on:                                             */
/*       SKX_A0 (0x2017002C)                                                  */
/*       SKX (0x2017002C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_CHA_PMA_REG 0x0100202C
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} SVID_CHA_PMA_STRUCT;


/* SDID_CHA_PMA_REG supported on:                                             */
/*       SKX_A0 (0x2017002E)                                                  */
/*       SKX (0x2017002E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_CHA_PMA_REG 0x0100202E
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} SDID_CHA_PMA_STRUCT;


/* CAPPTR_CHA_PMA_REG supported on:                                           */
/*       SKX_A0 (0x10170034)                                                  */
/*       SKX (0x10170034)                                                     */
/* Register default value:              0x00                                  */
#define CAPPTR_CHA_PMA_REG 0x01001034
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} CAPPTR_CHA_PMA_STRUCT;


/* INTL_CHA_PMA_REG supported on:                                             */
/*       SKX_A0 (0x1017003C)                                                  */
/*       SKX (0x1017003C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_CHA_PMA_REG 0x0100103C
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} INTL_CHA_PMA_STRUCT;


/* INTPIN_CHA_PMA_REG supported on:                                           */
/*       SKX_A0 (0x1017003D)                                                  */
/*       SKX (0x1017003D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_CHA_PMA_REG 0x0100103D
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} INTPIN_CHA_PMA_STRUCT;


/* MINGNT_CHA_PMA_REG supported on:                                           */
/*       SKX_A0 (0x1017003E)                                                  */
/*       SKX (0x1017003E)                                                     */
/* Register default value:              0x00                                  */
#define MINGNT_CHA_PMA_REG 0x0100103E
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} MINGNT_CHA_PMA_STRUCT;


/* MAXLAT_CHA_PMA_REG supported on:                                           */
/*       SKX_A0 (0x1017003F)                                                  */
/*       SKX (0x1017003F)                                                     */
/* Register default value:              0x00                                  */
#define MAXLAT_CHA_PMA_REG 0x0100103F
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
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
} MAXLAT_CHA_PMA_STRUCT;








/* QPI_XNC_MASK_CHA_PMA_REG supported on:                                     */
/*       SKX_A0 (0x40170088)                                                  */
/*       SKX (0x40170088)                                                     */
/* Register default value:              0x00000000                            */
#define QPI_XNC_MASK_CHA_PMA_REG 0x01004088
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * Contains info about the XNCs in the system.
 */
typedef union {
  struct {
    UINT32 xncnodemask : 16;
    /* xncnodemask - Bits[15:0], RWS, default = 24'h000000 
       Denote which nodes in a system are XNCs. xNCNodeMask[node_id]=1 if the node is 
       an XNC, and 0 otherwise. 
     */
    UINT32 evctclnen : 1;
    /* evctclnen - Bits[16:16], RWS, default = 1'h0 
       Send EvctCln message when victimizing an E-state line from the cache, unless the 
       line is being monitored 
     */
    UINT32 rsvd : 15;
    /* rsvd - Bits[31:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} QPI_XNC_MASK_CHA_PMA_STRUCT;












/* SADDBGMM0_CHA_PMA_REG supported on:                                        */
/*       SKX_A0 (0x401700A0)                                                  */
/*       SKX (0x401700A0)                                                     */
/* Register default value:              0x00000000                            */
#define SADDBGMM0_CHA_PMA_REG 0x010040A0
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 mask_37_6 : 32;
    /* mask_37_6 - Bits[31:0], RWS_LB, default = 32'b00000000000000000000000000000000 
       Mask field for addres[37:6]
     */
  } Bits;
  UINT32 Data;
} SADDBGMM0_CHA_PMA_STRUCT;


/* SADDBGMM1_CHA_PMA_REG supported on:                                        */
/*       SKX_A0 (0x401700A4)                                                  */
/*       SKX (0x401700A4)                                                     */
/* Register default value:              0x00000000                            */
#define SADDBGMM1_CHA_PMA_REG 0x010040A4
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 mask_45_38 : 8;
    /* mask_45_38 - Bits[7:0], RWS_LB, default = 8'b00000000 
       Mask field for address[45:38].  Mask bits that are 1 correspond to address bits 
       that are not matched. 
     */
    UINT32 match_29_6 : 24;
    /* match_29_6 - Bits[31:8], RWS_LB, default = 24'b000000000000000000000000 
       Match field for address[29:6]. If bit i in the mask is 1, the corresponding bit 
       in the match field should be 0.  
     */
  } Bits;
  UINT32 Data;
} SADDBGMM1_CHA_PMA_STRUCT;


/* SADDBGMM2_CHA_PMA_REG supported on:                                        */
/*       SKX_A0 (0x401700A8)                                                  */
/*       SKX (0x401700A8)                                                     */
/* Register default value:              0x00000000                            */
#define SADDBGMM2_CHA_PMA_REG 0x010040A8
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * Controls debug functions in csad.
 */
typedef union {
  struct {
    UINT32 match_45_30 : 16;
    /* match_45_30 - Bits[15:0], RWS_LB, default = 16'b0000000000000000 
       Match field for address[45:30]. If bit i in the mask is 1, the corresponding bit 
       in the match field should be 0. 
     */
    UINT32 rsvd_16 : 5;
    UINT32 errinjaddrvalid : 1;
    /* errinjaddrvalid - Bits[21:21], RWS_LB, default = 1'b0 
       Set to 1 for CLFLUSH address match along with PA Addr match on ERR Injection.
     */
    UINT32 errinjopcodevalid : 1;
    /* errinjopcodevalid - Bits[22:22], RWS_LB, default = 1'b0 
       Set to 1 for CLFLUSH opcode match along with PA Addr match on ERR Injection.
     */
    UINT32 rsvd_23 : 9;
  } Bits;
  UINT32 Data;
} SADDBGMM2_CHA_PMA_STRUCT;






/* MC_ROUTE_TABLE_CHA_PMA_REG supported on:                                   */
/*       SKX_A0 (0x401700B4)                                                  */
/*       SKX (0x401700B4)                                                     */
/* Register default value:              0x00000000                            */
#define MC_ROUTE_TABLE_CHA_PMA_REG 0x010040B4
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * Controls the physical location targetted by the logical IMC Channel results 
 * computed by the SAD, when the SAD decodes a local coherent target for an 
 * address.  The logical result produced by the SAD will be a number from 0 to 5.  
 * For a logical result of N, RingIDN will be used to determine the target memory 
 * controller, and ChannelIDN will be used to determine the target DDR channel at 
 * that memory controller. 
 */
typedef union {
  struct {
    UINT32 ringid0 : 3;
    /* ringid0 - Bits[2:0], RW_LB, default = 3'b000 
       Determines the MC target for Logical IMC Channel 0.  b'000 means target MC0.  
       b'001 means target MC1.  Any other falue in this field is not supported. 
     */
    UINT32 ringid1 : 3;
    /* ringid1 - Bits[5:3], RW_LB, default = 3'b000 
       Determines the MC target for Logical IMC Channel 1.  b'000 means target MC0.  
       b'001 means target MC1.  Any other falue in this field is not supported. 
     */
    UINT32 ringid2 : 3;
    /* ringid2 - Bits[8:6], RW_LB, default = 3'b000 
       Determines the MC target for Logical IMC Channel 2.  b'000 means target MC0.  
       b'001 means target MC1.  Any other falue in this field is not supported. 
     */
    UINT32 ringid3 : 3;
    /* ringid3 - Bits[11:9], RW_LB, default = 3'b000 
       Determines the MC target for Logical IMC Channel 3.  b'000 means target MC0.  
       b'001 means target MC1.  Any other falue in this field is not supported. 
     */
    UINT32 ringid4 : 3;
    /* ringid4 - Bits[14:12], RW_LB, default = 3'b000 
       Determines the MC target for Logical IMC Channel 4.  b'000 means target MC0.  
       b'001 means target MC1.  Any other falue in this field is not supported. 
     */
    UINT32 ringid5 : 3;
    /* ringid5 - Bits[17:15], RW_LB, default = 3'b000 
       Determines the MC target for Logical IMC Channel 5.  b'000 means target MC0.  
       b'001 means target MC1.  Any other falue in this field is not supported. 
     */
    UINT32 channelid0 : 2;
    /* channelid0 - Bits[19:18], RW_LB, default = 2'b00 
       Physical Channel ID for Locigal IMC Channel 0.  Legal values are b'00, b'01, and 
       b'10.  b'11 is not supported. 
     */
    UINT32 channelid1 : 2;
    /* channelid1 - Bits[21:20], RW_LB, default = 2'b00 
       Physical Channel ID for Locigal IMC Channel 1.  Legal values are b'00, b'01, and 
       b'10.  b'11 is not supported. 
     */
    UINT32 channelid2 : 2;
    /* channelid2 - Bits[23:22], RW_LB, default = 2'b00 
       Physical Channel ID for Locigal IMC Channel 2.  Legal values are b'00, b'01, and 
       b'10.  b'11 is not supported. 
     */
    UINT32 channelid3 : 2;
    /* channelid3 - Bits[25:24], RW_LB, default = 2'b00 
       Physical Channel ID for Locigal IMC Channel 3.  Legal values are b'00, b'01, and 
       b'10.  b'11 is not supported. 
     */
    UINT32 channelid4 : 2;
    /* channelid4 - Bits[27:26], RW_LB, default = 2'b00 
       Physical Channel ID for Locigal IMC Channel 4.  Legal values are b'00, b'01, and 
       b'10.  b'11 is not supported. 
     */
    UINT32 channelid5 : 2;
    /* channelid5 - Bits[29:28], RW_LB, default = 2'b00 
       Physical Channel ID for Locigal IMC Channel 5.  Legal values are b'00, b'01, and 
       b'10.  b'11 is not supported. 
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[31:30], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MC_ROUTE_TABLE_CHA_PMA_STRUCT;


/* EDC_ROUTE_TABLE_CHA_PMA_REG supported on:                                  */
/*       SKX_A0 (0x401700B8)                                                  */
/*       SKX (0x401700B8)                                                     */
/* Register default value:              0x00000000                            */
#define EDC_ROUTE_TABLE_CHA_PMA_REG 0x010040B8
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 edctarget0 : 3;
    /* edctarget0 - Bits[2:0], RW_LB, default = 3'b000 
       Ring Identifier for Logical EDC Target 0
     */
    UINT32 edctarget1 : 3;
    /* edctarget1 - Bits[5:3], RW_LB, default = 3'b000 
       Ring Identifier for Logical EDC Target 1
     */
    UINT32 edctarget2 : 3;
    /* edctarget2 - Bits[8:6], RW_LB, default = 3'b000 
       Ring Identifier for Logical EDC Target 2
     */
    UINT32 edctarget3 : 3;
    /* edctarget3 - Bits[11:9], RW_LB, default = 3'b000 
       Ring Identifier for Logical EDC Target 3
     */
    UINT32 edctarget4 : 3;
    /* edctarget4 - Bits[14:12], RW_LB, default = 3'b000 
       Ring Identifier for Logical EDC Target 4
     */
    UINT32 edctarget5 : 3;
    /* edctarget5 - Bits[17:15], RW_LB, default = 3'b000 
       Ring Identifier for Logical EDC Target 5
     */
    UINT32 edctarget6 : 3;
    /* edctarget6 - Bits[20:18], RW_LB, default = 3'b000 
       Ring Identifier for Logical EDC Target 6
     */
    UINT32 edctarget7 : 3;
    /* edctarget7 - Bits[23:21], RW_LB, default = 3'b000 
       Ring Identifier for Logical EDC Target 7
     */
    UINT32 rsvd : 8;
    /* rsvd - Bits[31:24], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} EDC_ROUTE_TABLE_CHA_PMA_STRUCT;














/* AD_VNA_CREDIT_CONFIG_N0_CHA_PMA_REG supported on:                          */
/*       SKX_A0 (0x401700D8)                                                  */
/*       SKX (0x401700D8)                                                     */
/* Register default value:              0x11111111                            */
#define AD_VNA_CREDIT_CONFIG_N0_CHA_PMA_REG 0x010040D8
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * Register related to VNA Credit Configuration
 */
typedef union {
  struct {
    UINT32 ad_vna_port0 : 4;
    /* ad_vna_port0 - Bits[3:0], RWS, default = 4'b0001 
       AD VNA count for PORT1. PCIE0/1
     */
    UINT32 ad_vna_port1 : 4;
    /* ad_vna_port1 - Bits[7:4], RWS, default = 4'b0001 
       AD VNA count for PORT2. PCIE3
     */
    UINT32 ad_vna_port2 : 4;
    /* ad_vna_port2 - Bits[11:8], RWS, default = 4'b0001 
       AD VNA count for PORT3. Intel UPI 0
     */
    UINT32 ad_vna_port3 : 4;
    /* ad_vna_port3 - Bits[15:12], RWS, default = 4'b0001 
       AD VNA count for PORT4. Intel UPI 1
     */
    UINT32 ad_vna_port4 : 4;
    /* ad_vna_port4 - Bits[19:16], RWS, default = 4'b0001 
       AD VNA count for PORT5. Intel UPI 2
     */
    UINT32 ad_vna_port5 : 4;
    /* ad_vna_port5 - Bits[23:20], RWS, default = 4'b0001 
       AD VNA count for PORT6. Intel UPI 3
     */
    UINT32 rsvd_24 : 4;
    UINT32 ad_vna_port7 : 4;
    /* ad_vna_port7 - Bits[31:28], RWS, default = 4'b0001 
       AD VNA count for PORT8. PCIE5
     */
  } Bits;
  UINT32 Data;
} AD_VNA_CREDIT_CONFIG_N0_CHA_PMA_STRUCT;


/* AD_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG supported on:                          */
/*       SKX_A0 (0x401700DC)                                                  */
/*       SKX (0x401700DC)                                                     */
/* Register default value:              0x00001111                            */
#define AD_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG 0x010040DC
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * Register related to VNA Credit Configuration
 */
typedef union {
  struct {
    UINT32 ad_vna_port8 : 4;
    /* ad_vna_port8 - Bits[3:0], RWS, default = 4'b0001 
       AD VNA count for PORT9. 
     */
    UINT32 ad_vna_port9 : 4;
    /* ad_vna_port9 - Bits[7:4], RWS, default = 4'b0001 
       AD VNA count for PORT10. 
     */
    UINT32 ad_vna_port10 : 4;
    /* ad_vna_port10 - Bits[11:8], RWS, default = 4'b0001 
       AD VNA count for PORT11.  
     */
    UINT32 ad_vna_port11 : 4;
    /* ad_vna_port11 - Bits[15:12], RWS, default = 4'b0001 
       AD VNA count for PORT12. 
     */
    UINT32 vnacreditchange : 1;
    /* vnacreditchange - Bits[16:16], RWS, default = 1'b0 
       Change VNA credits (Load from Shadow)
     */
    UINT32 disisocvnres : 1;
    /* disisocvnres - Bits[17:17], RWS, default = 1'b0 
       Disable ISOC VN credit reservation
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} AD_VNA_CREDIT_CONFIG_N1_CHA_PMA_STRUCT;


/* BL_VNA_CREDIT_CONFIG_N0_CHA_PMA_REG supported on:                          */
/*       SKX_A0 (0x401700E0)                                                  */
/*       SKX (0x401700E0)                                                     */
/* Register default value:              0x11111111                            */
#define BL_VNA_CREDIT_CONFIG_N0_CHA_PMA_REG 0x010040E0
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * Register related to VNA Credit Configuration
 */
typedef union {
  struct {
    UINT32 bl_vna_port0 : 4;
    /* bl_vna_port0 - Bits[3:0], RWS, default = 4'b0001 
       BL VNA count for PORT0. PCIE2(UBOX)
     */
    UINT32 bl_vna_port1 : 4;
    /* bl_vna_port1 - Bits[7:4], RWS, default = 4'b0001 
       BL VNA count for PORT1. PCIE0/PCIE1
     */
    UINT32 bl_vna_port2 : 4;
    /* bl_vna_port2 - Bits[11:8], RWS, default = 4'b0001 
       BL VNA count for PORT2. PCIE3
     */
    UINT32 bl_vna_port3 : 4;
    /* bl_vna_port3 - Bits[15:12], RWS, default = 4'b0001 
       BL VNA count for PORT3. Intel UPI 0
     */
    UINT32 bl_vna_port4 : 4;
    /* bl_vna_port4 - Bits[19:16], RWS, default = 4'b0001 
       BL VNA count for PORT4. Intel UPI 1
     */
    UINT32 bl_vna_port5 : 4;
    /* bl_vna_port5 - Bits[23:20], RWS, default = 4'b0001 
       BL VNA count for PORT5. Intel UPI 2
     */
    UINT32 rsvd_24 : 4;
    UINT32 bl_vna_port7 : 4;
    /* bl_vna_port7 - Bits[31:28], RWS, default = 4'b0001 
       BL VNA count for PORT7. PCIE4
     */
  } Bits;
  UINT32 Data;
} BL_VNA_CREDIT_CONFIG_N0_CHA_PMA_STRUCT;


/* BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG supported on:                          */
/*       SKX_A0 (0x401700E4)                                                  */
/*       SKX (0x401700E4)                                                     */
/* Register default value:              0x00001111                            */
#define BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG 0x010040E4
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * Register related to VNA Credit Configuration
 */
typedef union {
  struct {
    UINT32 bl_vna_port8 : 4;
    /* bl_vna_port8 - Bits[3:0], RWS, default = 4'b0001 
       BL VNA count for PORT8. PCIE5
     */
    UINT32 bl_vna_port9 : 4;
    /* bl_vna_port9 - Bits[7:4], RWS, default = 4'b0001 
       BL VNA count for PORT9
     */
    UINT32 bl_vna_port10 : 4;
    /* bl_vna_port10 - Bits[11:8], RWS, default = 4'b0001 
       BL VNA count for PORT10
     */
    UINT32 bl_vna_port11 : 4;
    /* bl_vna_port11 - Bits[15:12], RWS, default = 4'b0001 
       BL VNA count for PORT11
     */
    UINT32 vnacreditchange : 1;
    /* vnacreditchange - Bits[16:16], RWS, default = 1'b0 
       Change VNA credits (Load from Shadow)
     */
    UINT32 disisocvnres : 1;
    /* disisocvnres - Bits[17:17], RWS, default = 1'b0 
       Disable ISOC VN credit reservation
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_STRUCT;


/* HA_COH_CFG_TOR_CHA_PMA_REG supported on:                                   */
/*       SKX_A0 (0x401700EC)                                                  */
/*       SKX (0x401700EC)                                                     */
/* Register default value:              0x01380031                            */
#define HA_COH_CFG_TOR_CHA_PMA_REG 0x010040EC
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 * HA Coherency Config Register
 */
typedef union {
  struct {
    UINT32 hitme_enable : 1;
    /* hitme_enable - Bits[0:0], RW_LB, default = 1'b1 
       1'b1 - Enable HitME cache overloading. HitME$ should be enabled only when 
       directory is enabled. 
     */
    UINT32 hitme_rfo_dirs : 1;
    /* hitme_rfo_dirs - Bits[1:1], RW_LB, default = 1'b0 
       1'b1 - Enable HitME DIR=S RFO optimization. This optimization allows snoops to 
       be broadcast in parallel with memory lookup for RFO HitS in the local CA. 
     */
    UINT32 hitme_shar : 1;
    /* hitme_shar - Bits[2:2], RW_LB, default = 1'b0 
       HitME cache allows caching of SHARed lines
     */
    UINT32 twolm : 1;
    /* twolm - Bits[3:3], RW_LB, default = 1'b0 
       1'b1 - TwoLM enabled
     */
    UINT32 hitme_ways : 12;
    /* hitme_ways - Bits[15:4], RW_LB, default = 12'b000000000011 
       Identifies which ways of the SF can be used for HitME cache entries
     */
    UINT32 hacreditcnt : 4;
    /* hacreditcnt - Bits[19:16], RW_LB, default = 4'b1000 
       Number of credits in the TOR to the HApipe for the TORReqQ
     */
    UINT32 haegrcreditthreshold : 4;
    /* haegrcreditthreshold - Bits[23:20], RW_LB, default = 4'b0011 
       Threshold for the AD/BL egress credits for the HApipe. When the number of 
       credits available is higher than this number, the HA is allowed to acquire a 
       credit. This can never be set to less than 3. 
     */
    UINT32 hitme_alloconlocalme : 1;
    /* hitme_alloconlocalme - Bits[24:24], RW_LB, default = 1'b1 
       Convert SF entry to HitME$ entry when a remote request hits the local CA in M/E 
       state and results in Data_M/E transfer. 
     */
    UINT32 rsvd : 7;
    /* rsvd - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} HA_COH_CFG_TOR_CHA_PMA_STRUCT;


/* KTI_ROUTING_TABLE_SHADOW_CHA_PMA_REG supported on:                         */
/*       SKX_A0 (0x401700F0)                                                  */
/*       SKX (0x401700F0)                                                     */
/* Register default value:              0x00000000                            */
#define KTI_ROUTING_TABLE_SHADOW_CHA_PMA_REG 0x010040F0
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti_routing_vector : 32;
    /* kti_routing_vector - Bits[31:0], RW_LB, default = 32'h00000000 
       2 bits indicating target Intel UPI port (4 possible targets)
     */
  } Bits;
  UINT32 Data;
} KTI_ROUTING_TABLE_SHADOW_CHA_PMA_STRUCT;


/* KTI_ROUTING_TABLE_CHA_PMA_REG supported on:                                */
/*       SKX_A0 (0x401700F4)                                                  */
/*       SKX (0x401700F4)                                                     */
/* Register default value:              0x00000000                            */
#define KTI_ROUTING_TABLE_CHA_PMA_REG 0x010040F4
/* Struct format extracted from XML file SKX\1.14.0.CFG.xml.
 */
typedef union {
  struct {
    UINT32 kti_routing_vector : 32;
    /* kti_routing_vector - Bits[31:0], RW_LBV, default = 32'h00000000 
       Indicates Target Intel UPI Port [1:0] (4 possible targets)
     */
  } Bits;
  UINT32 Data;
} KTI_ROUTING_TABLE_CHA_PMA_STRUCT;




#endif /* CHA_PMA_h */
