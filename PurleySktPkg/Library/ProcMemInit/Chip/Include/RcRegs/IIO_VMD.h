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

#ifndef IIO_VMD_h
#define IIO_VMD_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* IIO_VMD_DEV 5                                                              */
/* IIO_VMD_FUN 5                                                              */

/* VID_IIO_VMD_REG supported on:                                              */
/*       SKX_A0 (0x2012D000)                                                  */
/*       SKX (0x2012D000)                                                     */
/* Register default value:              0x8086                                */
#define VID_IIO_VMD_REG 0x11042000
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Vendor ID
 */
typedef union {
  struct {
    UINT16 vendor_identification_number : 16;
    /* vendor_identification_number - Bits[15:0], RO, default = 16'h8086 
       
                   The value is assigned by PCI-SIG to Intel.
                
     */
  } Bits;
  UINT16 Data;
} VID_IIO_VMD_STRUCT;


/* DID_IIO_VMD_REG supported on:                                              */
/*       SKX_A0 (0x2012D002)                                                  */
/*       SKX (0x2012D002)                                                     */
/* Register default value:              0x201D                                */
#define DID_IIO_VMD_REG 0x11042002
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Device ID
 */
typedef union {
  struct {
    UINT16 device_identification_number : 16;
    /* device_identification_number - Bits[15:0], RO, default = 16'h201D 
       
                   The value in this register specifies the Device ID for the Volumne 
       Management Device. This value is the same for all instances of the VMD. 
                
     */
  } Bits;
  UINT16 Data;
} DID_IIO_VMD_STRUCT;


/* PCICMD_IIO_VMD_REG supported on:                                           */
/*       SKX_A0 (0x2012D004)                                                  */
/*       SKX (0x2012D004)                                                     */
/* Register default value:              0x0000                                */
#define PCICMD_IIO_VMD_REG 0x11042004
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * PCI Command
 */
typedef union {
  struct {
    UINT16 iose : 1;
    /* iose - Bits[0:0], RO, default = 'b0 
       
                   I/O Space Enable
                   Not supported by VMD. VMD driver must not enable I/O regions in 
       VMD-Owned Root Port or Endpoint devices. 
                
     */
    UINT16 mse : 1;
    /* mse - Bits[1:1], RW, default = 'b0 
       
                   Memory Space Enable
                   Virtually, this bit is meant to enable the VMD memory BAR's. In 
       hardware, this bit will enable CFGBAR and the MEMBAR2 MSI-X table. It has no 
       effect on MEMBAR1 and the rest of MEMBAR2 (which are decoded by the VMD-Owned 
       Root Ports). 
                   The VMD driver reads this bit to decide how to set corresponding MSE 
       bits in the VMD-Owned Root Ports and/or Endpoint devices. 
                   1: If this bit is set, CFGBAR and MSI-X tables are enabled for 
       access. The VMD driver may enable VMD-Owned Root Port and Endpoint device BAR 
       regions. 
                   0: If this bit is clear, CFGBAR and MSI-X tables are disabled and 
       inaccessible. The VMD driver must disable VMD-Owned Root Port and Endpoint 
       device BAR regions. 
                   Notes: A write to this register will trigger an interrupt to the VMD 
       driver using the MSI table entry 0. 
                
     */
    UINT16 bme : 1;
    /* bme - Bits[2:2], RW, default = 'b0 
       
                   Bus Master Enable
                   Virtually, this bit is meant to enable the VMD to master requests to 
       the system. This bit has no effect in hardware. 
                   The VMD driver reads this bit to decide how to set corresponding BME 
       bits in the VMD-Owned Root Ports and Endpoint devices. 
                   1: If this bit is set, the VMD driver may allow VMD-Owned Root Ports 
       and Endpoint devices to master requests to the Root Complex. 
                   0: If this bit is clear, the VMD driver must prevent VMD-Owned Root 
       Ports and Endpoint devices from mastering requests to the Root Complex. 
                   Notes: A write to this register will trigger an interrupt to the VMD 
       driver using the MSI table entry 0. 
                
     */
    UINT16 sce : 1;
    /* sce - Bits[3:3], RO, default = 'b0 
       
                   Special Cycle Enable
                   Not applicable to DMI/PCI Express devices. Hardwired to 0
                
     */
    UINT16 mwie : 1;
    /* mwie - Bits[4:4], RO, default = 'b0 
       
                   Memory Write and Invalidate Enable
                   Not applicable to internal IIO devices. Hardwired to 0.
                
     */
    UINT16 vga_palette_snoop_enable : 1;
    /* vga_palette_snoop_enable - Bits[5:5], RO, default = 'b0 
       
                   Not applicable to internal IIO devices. Hardwired to 0.
                
     */
    UINT16 perre : 1;
    /* perre - Bits[6:6], RO, default = 'b0 
       
                   Parity Error Reporting Enable
                   Not supported for VMD. VMD-Owned Root Ports still report parity 
       errors separately. 
                
     */
    UINT16 idsel_stepping_wait_cycle_control : 1;
    /* idsel_stepping_wait_cycle_control - Bits[7:7], RO, default = 'b0 
       
                   Not applicable to internal IIO devices. Hardwired to 0.
                
     */
    UINT16 serre : 1;
    /* serre - Bits[8:8], RO, default = 'b0 
       
                   SERR Reporting Enable
                   Not supported for VMD. VMD-Owned Root Ports may be programmed by the 
       VMD driver to signal a system error. 
                
     */
    UINT16 fast_back_to_back_enable : 1;
    /* fast_back_to_back_enable - Bits[9:9], RO, default = 'b0 
       
                   Not applicable to PCI Express and is hardwired to 0
                
     */
    UINT16 interrupt_disable : 1;
    /* interrupt_disable - Bits[10:10], RW, default = 'b0 
       
                   INTx Interrupt Disable
                   VMD does not support the generation of INTx, but VMD-owned devices 
       may. This bit has no effect in hardware. 
                   1: INTx Legacy Interrupt generation is disabled
                   0: INTx Legacy Interrupt generation is enabled
                   Notes: 
                   a) INTx message received from VMD-owned Root Ports will be routed to 
       the system using the same rules defined in the Root Ports as though they were 
       not VMD-owned. If the VMD driver expects INTx, then the INTPIN registers in the 
       VMD-owned Root Ports and Switches must be programmed by the VMD driver. 
                   b) A write to this register will trigger an interrupt to the VMD 
       driver using the MSI table entry 0. 
                
     */
    UINT16 rsvd : 5;
    /* rsvd - Bits[15:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT16 Data;
} PCICMD_IIO_VMD_STRUCT;


/* PCISTS_IIO_VMD_REG supported on:                                           */
/*       SKX_A0 (0x2012D006)                                                  */
/*       SKX (0x2012D006)                                                     */
/* Register default value:              0x0010                                */
#define PCISTS_IIO_VMD_REG 0x11042006
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * PCI Status
 */
typedef union {
  struct {
    UINT16 rsvd_0 : 3;
    /* rsvd_0 - Bits[2:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT16 intx_status : 1;
    /* intx_status - Bits[3:3], RO, default = 'b0 
       
                   Indicates a pending INTx interrupt.
                   Not used by VMD.
                
     */
    UINT16 capabilities_list : 1;
    /* capabilities_list - Bits[4:4], RO, default = 1'b1 
       
                   This bit indicates the presence of a capabilities list structure.
                
     */
    UINT16 pci66mhz_capable : 1;
    /* pci66mhz_capable - Bits[5:5], RO, default = 'b0 
       
                   Not applicable to VMD. Hardwired to 0.
                
     */
    UINT16 rsvd_6 : 1;
    /* rsvd_6 - Bits[6:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT16 fast_back_to_back : 1;
    /* fast_back_to_back - Bits[7:7], RO, default = 'b0 
       
                   Not applicable to VMD. Hardwired to 0.
                
     */
    UINT16 mdpe : 1;
    /* mdpe - Bits[8:8], RO, default = 'b0 
       
                   Master Data Parity Error
                   Not used by VMD.
                
     */
    UINT16 devsel_timing : 2;
    /* devsel_timing - Bits[10:9], RO, default = 'h0 
       
                   Not applicable to PCI Express. Hardwired to 0.
                
     */
    UINT16 sta : 1;
    /* sta - Bits[11:11], RO, default = 'b0 
       
                   Signalled Target Abort
                   Not used by VMD.
                
     */
    UINT16 rta : 1;
    /* rta - Bits[12:12], RO, default = 'b0 
       
                   Received Target Abort
                   Not used by VMD.
                
     */
    UINT16 rma : 1;
    /* rma - Bits[13:13], RO, default = 'b0 
       
                   Received Master Abort
                   Not used by VMD.
                
     */
    UINT16 sse : 1;
    /* sse - Bits[14:14], RO, default = 'b0 
       
                   Signalled System Error
                   Not used by VMD.
                
     */
    UINT16 dpe : 1;
    /* dpe - Bits[15:15], RO, default = 'b0 
       
                   Detected Parity Error
                   Not used by VMD.
                
     */
  } Bits;
  UINT16 Data;
} PCISTS_IIO_VMD_STRUCT;


/* RID_IIO_VMD_REG supported on:                                              */
/*       SKX_A0 (0x1012D008)                                                  */
/*       SKX (0x1012D008)                                                     */
/* Register default value:              0x00                                  */
#define RID_IIO_VMD_REG 0x11041008
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Revision ID
 */
typedef union {
  struct {
    UINT8 revision_id : 8;
    /* revision_id - Bits[7:0], RO, default = 8'h0 
       
       	  Revision ID.
       	
     */
  } Bits;
  UINT8 Data;
} RID_IIO_VMD_STRUCT;


/* CCR_N0_IIO_VMD_REG supported on:                                           */
/*       SKX_A0 (0x1012D009)                                                  */
/*       SKX (0x1012D009)                                                     */
/* Register default value:              0x00                                  */
#define CCR_N0_IIO_VMD_REG 0x11041009
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Class Code Register
 */
typedef union {
  struct {
    UINT8 interface : 8;
    /* interface - Bits[7:0], RW_O, default = 8'h0 
       
       	  Base revision.
       	
     */
  } Bits;
  UINT8 Data;
} CCR_N0_IIO_VMD_STRUCT;


/* CCR_N1_IIO_VMD_REG supported on:                                           */
/*       SKX_A0 (0x2012D00A)                                                  */
/*       SKX (0x2012D00A)                                                     */
/* Register default value:              0x0104                                */
#define CCR_N1_IIO_VMD_REG 0x1104200A
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Class Code Register
 */
typedef union {
  struct {
    UINT16 sub_class : 8;
    /* sub_class - Bits[7:0], RW_O, default = 8'h4 
       
       	  Base revision.
       	
     */
    UINT16 base_class : 8;
    /* base_class - Bits[15:8], RW_O, default = 8'h1 
       
       	  Base revision.
       	
     */
  } Bits;
  UINT16 Data;
} CCR_N1_IIO_VMD_STRUCT;


/* CLSR_IIO_VMD_REG supported on:                                             */
/*       SKX_A0 (0x1012D00C)                                                  */
/*       SKX (0x1012D00C)                                                     */
/* Register default value:              0x00                                  */
#define CLSR_IIO_VMD_REG 0x1104100C
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Cacheline Size Register
 */
typedef union {
  struct {
    UINT8 cacheline_size : 8;
    /* cacheline_size - Bits[7:0], RW, default = 'h00 
       
                   This register is set as RW for compatibility reasons only. Cacheline 
       size is 64B. 
                
     */
  } Bits;
  UINT8 Data;
} CLSR_IIO_VMD_STRUCT;


/* HDR_IIO_VMD_REG supported on:                                              */
/*       SKX_A0 (0x1012D00E)                                                  */
/*       SKX (0x1012D00E)                                                     */
/* Register default value:              0x80                                  */
#define HDR_IIO_VMD_REG 0x1104100E
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Header Type
 */
typedef union {
  struct {
    UINT8 configuration_layout : 7;
    /* configuration_layout - Bits[6:0], RO, default = 'h00 
       
                   This field identifies the format of the configuration header layout. 
       It is Type 0 for all this device. The default is 00h, indicating a 'endpoint 
       device'. 
                
     */
    UINT8 multi_function_device : 1;
    /* multi_function_device - Bits[7:7], RO, default = 1'b1 
       
                   Set to 1b to indicate functions 1-7 may exist for the device
                
     */
  } Bits;
  UINT8 Data;
} HDR_IIO_VMD_STRUCT;


/* CFGBAR_N0_IIO_VMD_REG supported on:                                        */
/*       SKX_A0 (0x4012D010)                                                  */
/*       SKX (0x4012D010)                                                     */
/* Register default value:              0x0000000C                            */
#define CFGBAR_N0_IIO_VMD_REG 0x11044010
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Configuration Base Address
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 type : 2;
    /* type - Bits[2:1], RW_L, default = 2'b10 
       
                   Memory type claimed by this BAR is 64-bit addressable
                   Memory_Space_Indicator
                   BAR resource is memory (as opposed to I/O).
                
     */
    UINT32 prefetchable : 1;
    /* prefetchable - Bits[3:3], RW_L, default = 1'b1 
       
                   BAR points to Prefetchable memory.
                
     */
    UINT32 rsvd_4 : 16;
    /* rsvd_4 - Bits[19:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 memory_base_address : 12;
    /* memory_base_address - Bits[31:20], RW_V, default = None 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
                   Sets the location of the CFGBAR in memory space. The size is 
       programmed in CFGBARSZ by BIOS. CFGBARSZ specifies the lowest order address bit 
       that is writeable. The minimum granularity is 1MB. 
                   If CFGBAR.Type = 10b, then bits 63:32 are writeable.
                   If CFGBAR.Type = 00b, then bits 63:32 are read-only.
                
     */
  } Bits;
  UINT32 Data;
} CFGBAR_N0_IIO_VMD_STRUCT;


/* CFGBAR_N1_IIO_VMD_REG supported on:                                        */
/*       SKX_A0 (0x4012D014)                                                  */
/*       SKX (0x4012D014)                                                     */
/* Register default value:              0x00000000                            */
#define CFGBAR_N1_IIO_VMD_REG 0x11044014
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Configuration Base Address
 */
typedef union {
  struct {
    UINT32 memory_base_address : 32;
    /* memory_base_address - Bits[31:0], RW_V, default = None 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
                   Sets the location of the CFGBAR in memory space. The size is 
       programmed in CFGBARSZ by BIOS. CFGBARSZ specifies the lowest order address bit 
       that is writeable. The minimum granularity is 1MB. 
                   If CFGBAR.Type = 10b, then bits 63:32 are writeable.
                   If CFGBAR.Type = 00b, then bits 63:32 are read-only.
                
     */
  } Bits;
  UINT32 Data;
} CFGBAR_N1_IIO_VMD_STRUCT;


/* MEMBAR1_N0_IIO_VMD_REG supported on:                                       */
/*       SKX_A0 (0x4012D018)                                                  */
/*       SKX (0x4012D018)                                                     */
/* Register default value:              0x0000000C                            */
#define MEMBAR1_N0_IIO_VMD_REG 0x11044018
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Memory Base Address Range 1
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 type : 2;
    /* type - Bits[2:1], RW_L, default = 2'b10 
       
                   Memory type claimed by this BAR is 64-bit addressable
                   Memory_Space_Indicator
                   BAR resource is memory (as opposed to I/O).
                
     */
    UINT32 prefetchable : 1;
    /* prefetchable - Bits[3:3], RW_L, default = 1'b1 
       
                   BAR points to Prefetchable memory.
                
     */
    UINT32 rsvd_4 : 8;
    /* rsvd_4 - Bits[11:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 memory_base_address : 20;
    /* memory_base_address - Bits[31:12], RW_V, default = None 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
                   Sets the location of the MEMBAR1 in memory space. The size is 
       programmed in MEMBAR1SZ by BIOS. MEMBAR1SZ specifies the lowest order address 
       bit that is writeable. The minimum granularity is 4kB. 
                   If MEMBAR1.Type = 10b, then bits 63:32 are writeable.
                   If MEMBAR1.Type = 00b, then bits 63:32 are read-only.
                   If MEMBAR1SZ = 0, then all address bits are read-only.
                   If MEMBAR1SZ < 12, then the effective value is 12.
                
     */
  } Bits;
  UINT32 Data;
} MEMBAR1_N0_IIO_VMD_STRUCT;


/* MEMBAR1_N1_IIO_VMD_REG supported on:                                       */
/*       SKX_A0 (0x4012D01C)                                                  */
/*       SKX (0x4012D01C)                                                     */
/* Register default value:              0x00000000                            */
#define MEMBAR1_N1_IIO_VMD_REG 0x1104401C
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Memory Base Address Range 1
 */
typedef union {
  struct {
    UINT32 memory_base_address : 32;
    /* memory_base_address - Bits[31:0], RW_V, default = None 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
                   Sets the location of the MEMBAR1 in memory space. The size is 
       programmed in MEMBAR1SZ by BIOS. MEMBAR1SZ specifies the lowest order address 
       bit that is writeable. The minimum granularity is 4kB. 
                   If MEMBAR1.Type = 10b, then bits 63:32 are writeable.
                   If MEMBAR1.Type = 00b, then bits 63:32 are read-only.
                   If MEMBAR1SZ = 0, then all address bits are read-only.
                   If MEMBAR1SZ < 12, then the effective value is 12.
                
     */
  } Bits;
  UINT32 Data;
} MEMBAR1_N1_IIO_VMD_STRUCT;


/* MEMBAR2_N0_IIO_VMD_REG supported on:                                       */
/*       SKX_A0 (0x4012D020)                                                  */
/*       SKX (0x4012D020)                                                     */
/* Register default value:              0x0000000C                            */
#define MEMBAR2_N0_IIO_VMD_REG 0x11044020
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Memory Base Address Range 2
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    /* rsvd_0 - Bits[0:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 type : 2;
    /* type - Bits[2:1], RW_L, default = 2'b10 
       
                   Memory type claimed by this BAR is 64-bit addressable
                   Memory_Space_Indicator
                   BAR resource is memory (as opposed to I/O).
                
     */
    UINT32 prefetchable : 1;
    /* prefetchable - Bits[3:3], RW_L, default = 1'b1 
       
                   BAR points to Prefetchable memory.
                
     */
    UINT32 rsvd_4 : 9;
    /* rsvd_4 - Bits[12:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 memory_base_address : 19;
    /* memory_base_address - Bits[31:13], RW_V, default = None 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
                   Sets the location of the MEMBAR2 in memory space. The size is 
       programmed in MEMBAR2SZ by BIOS. MEMBAR2SZ specifies the lowest order address 
       bit that is writeable. The minimum granularity is 8kB. 
                   If MEMBAR2.Type = 10b, then bits 63:32 are writeable.
                   If MEMBAR2.Type = 00b, then bits 63:32 are read-only.
                   If MEMBAR2SZ = 0, then all address bits are read-only.
                   If MEMBAR2SZ < 13, then the effective value is 13.
                
     */
  } Bits;
  UINT32 Data;
} MEMBAR2_N0_IIO_VMD_STRUCT;


/* MEMBAR2_N1_IIO_VMD_REG supported on:                                       */
/*       SKX_A0 (0x4012D024)                                                  */
/*       SKX (0x4012D024)                                                     */
/* Register default value:              0x00000000                            */
#define MEMBAR2_N1_IIO_VMD_REG 0x11044024
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Memory Base Address Range 2
 */
typedef union {
  struct {
    UINT32 memory_base_address : 32;
    /* memory_base_address - Bits[31:0], RW_V, default = None 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       
                   Sets the location of the MEMBAR2 in memory space. The size is 
       programmed in MEMBAR2SZ by BIOS. MEMBAR2SZ specifies the lowest order address 
       bit that is writeable. The minimum granularity is 8kB. 
                   If MEMBAR2.Type = 10b, then bits 63:32 are writeable.
                   If MEMBAR2.Type = 00b, then bits 63:32 are read-only.
                   If MEMBAR2SZ = 0, then all address bits are read-only.
                   If MEMBAR2SZ < 13, then the effective value is 13.
                
     */
  } Bits;
  UINT32 Data;
} MEMBAR2_N1_IIO_VMD_STRUCT;


/* SVID_IIO_VMD_REG supported on:                                             */
/*       SKX_A0 (0x2012D02C)                                                  */
/*       SKX (0x2012D02C)                                                     */
/* Register default value:              0x8086                                */
#define SVID_IIO_VMD_REG 0x1104202C
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Subsystem Vendor ID
 */
typedef union {
  struct {
    UINT16 subsystem_vendor_id : 16;
    /* subsystem_vendor_id - Bits[15:0], RW_O, default = 16'h8086 
       
                   The default value specifies Intel but can be set to any value once 
       after reset. 
                
     */
  } Bits;
  UINT16 Data;
} SVID_IIO_VMD_STRUCT;


/* SDID_IIO_VMD_REG supported on:                                             */
/*       SKX_A0 (0x2012D02E)                                                  */
/*       SKX (0x2012D02E)                                                     */
/* Register default value:              0x0000                                */
#define SDID_IIO_VMD_REG 0x1104202E
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Subsystem ID
 */
typedef union {
  struct {
    UINT16 subsystem_id : 16;
    /* subsystem_id - Bits[15:0], RW_O, default = 'h0000 
       
                   The default value specifies Intel but can be set to any value once 
       after reset. 
                
     */
  } Bits;
  UINT16 Data;
} SDID_IIO_VMD_STRUCT;


/* CAPPTR_IIO_VMD_REG supported on:                                           */
/*       SKX_A0 (0x1012D034)                                                  */
/*       SKX (0x1012D034)                                                     */
/* Register default value:              0x80                                  */
#define CAPPTR_IIO_VMD_REG 0x11041034
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Capability Pointer
 */
typedef union {
  struct {
    UINT8 capability_pointer : 8;
    /* capability_pointer - Bits[7:0], RO, default = 8'h80 
       
                   Points to the first capability structure for the device which is the 
       PCIe capability (for devices that support 4kB extended configuration space). 
                   A value of zero indicates there are no capability structures (and no 
       extended configuration space). 
                
     */
  } Bits;
  UINT8 Data;
} CAPPTR_IIO_VMD_STRUCT;


/* INTL_IIO_VMD_REG supported on:                                             */
/*       SKX_A0 (0x1012D03C)                                                  */
/*       SKX (0x1012D03C)                                                     */
/* Register default value:              0x00                                  */
#define INTL_IIO_VMD_REG 0x1104103C
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Interrupt Line Register
 */
typedef union {
  struct {
    UINT8 interrupt_line : 8;
    /* interrupt_line - Bits[7:0], RO, default = 'h00 
       
                   N/A for these devices
                
     */
  } Bits;
  UINT8 Data;
} INTL_IIO_VMD_STRUCT;


/* INTPIN_IIO_VMD_REG supported on:                                           */
/*       SKX_A0 (0x1012D03D)                                                  */
/*       SKX (0x1012D03D)                                                     */
/* Register default value:              0x00                                  */
#define INTPIN_IIO_VMD_REG 0x1104103D
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Interrupt Pin Register
 */
typedef union {
  struct {
    UINT8 intp : 8;
    /* intp - Bits[7:0], RO, default = 'h00 
       
                   Interrupt Pin
                   N/A since these devices do not generate any interrupt on their own
                
     */
  } Bits;
  UINT8 Data;
} INTPIN_IIO_VMD_STRUCT;


/* VMCONFIG_IIO_VMD_REG supported on:                                         */
/*       SKX_A0 (0x4012D044)                                                  */
/*       SKX (0x4012D044)                                                     */
/* Register default value:              0x00000000                            */
#define VMCONFIG_IIO_VMD_REG 0x11044044
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Volume Management Configuration
 */
typedef union {
  struct {
    UINT32 vmdlock : 1;
    /* vmdlock - Bits[0:0], RW_O, default = None 
       
                   Locks registers in the Volume Management Device. This must be set by 
       BIOS after programming VMD registers. 
                
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VMCONFIG_IIO_VMD_STRUCT;


/* VMASSIGN_IIO_VMD_REG supported on:                                         */
/*       SKX_A0 (0x4012D048)                                                  */
/*       SKX (0x4012D048)                                                     */
/* Register default value:              0x00000000                            */
#define VMASSIGN_IIO_VMD_REG 0x11044048
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Volume Management Assignment
 */
typedef union {
  struct {
    UINT32 port_assign : 4;
    /* port_assign - Bits[3:0], RWS_L, default = 'b0000 
       
                   Each bit corresponds to a device number on the same bus number as 
       VMD. For each bit set, the associated device will be assigned to the VMD and 
       hidden from the OS. 
                   This VMD supports owning Devices 0-3 only, so only bits 0-3 are 
       writeable. 
                
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} VMASSIGN_IIO_VMD_STRUCT;


/* CFGBARSZ_IIO_VMD_REG supported on:                                         */
/*       SKX_A0 (0x1012D04C)                                                  */
/*       SKX (0x1012D04C)                                                     */
/* Register default value:              0x00                                  */
#define CFGBARSZ_IIO_VMD_REG 0x1104104C
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Configuration BAR Size
 */
typedef union {
  struct {
    UINT8 size : 8;
    /* size - Bits[7:0], RWS_L, default = 'h00 
       
                   Value indicating the size of CFGBAR. This value is written by BIOS 
       prior to enumeration. The value indicates the lowest order address bit that is 
       writeable in CFGBAR. The only legal settings are 20 to 28 (decimal), 
       representing BAR sizes of 1MB through 256MB. 
                   If CFGBARSZ = 0, then all address bits are read-only.
                   If CFGBARSZ < 20, then the effective value is 20.
                   If CFGBARSZ > 28, then the effective value is 28.
                
     */
  } Bits;
  UINT8 Data;
} CFGBARSZ_IIO_VMD_STRUCT;


/* MEMBAR1SZ_IIO_VMD_REG supported on:                                        */
/*       SKX_A0 (0x1012D04D)                                                  */
/*       SKX (0x1012D04D)                                                     */
/* Register default value:              0x00                                  */
#define MEMBAR1SZ_IIO_VMD_REG 0x1104104D
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Memory BAR 1 Size
 */
typedef union {
  struct {
    UINT8 size : 8;
    /* size - Bits[7:0], RWS_L, default = 'h00 
       
                   Value indicating the size of MEMBAR1SZ. This value is written by 
       BIOS prior to enumeration. The value indicates the lowest order address bit that 
       is writeable in MEMBAR1SZ. The only legal settings are 12 to 47 (decimal), 
       representing BAR sizes of 4kB through 256TB. 
                   If MEMBAR1SZ = 0, then all address bits are read-only.
                   If MEMBAR1SZ < 12, then the effective value is 12.
                   If MEMBAR1SZ > 47, then the effective value is 47.
                
     */
  } Bits;
  UINT8 Data;
} MEMBAR1SZ_IIO_VMD_STRUCT;


/* MEMBAR2SZ_IIO_VMD_REG supported on:                                        */
/*       SKX_A0 (0x1012D04E)                                                  */
/*       SKX (0x1012D04E)                                                     */
/* Register default value:              0x00                                  */
#define MEMBAR2SZ_IIO_VMD_REG 0x1104104E
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * VMD Memory BAR 2 Size
 */
typedef union {
  struct {
    UINT8 size : 8;
    /* size - Bits[7:0], RWS_L, default = 'h00 
       
                   Value indicating the size of MEMBAR2SZ. This value is written by 
       BIOS prior to enumeration. The value indicates the lowest order address bit that 
       is writeable in MEMBAR2SZ. The only legal settings are 13 to 47 (decimal), 
       representing BAR sizes of 4kB through 256TB. 
                   If MEMBAR2SZ = 0, then all address bits are read-only.
                   If MEMBAR2SZ < 13, then the effective value is 13.
                   If MEMBAR2SZ > 47, then the effective value is 47.
                
     */
  } Bits;
  UINT8 Data;
} MEMBAR2SZ_IIO_VMD_STRUCT;


/* MSIXCAPID_IIO_VMD_REG supported on:                                        */
/*       SKX_A0 (0x1012D080)                                                  */
/*       SKX (0x1012D080)                                                     */
/* Register default value:              0x11                                  */
#define MSIXCAPID_IIO_VMD_REG 0x11041080
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * MSI-X Capability ID
 */
typedef union {
  struct {
    UINT8 capability_id : 8;
    /* capability_id - Bits[7:0], RO, default = 8'h11 
       
                   Assigned by PCI-SIG for MSI-X.
                
     */
  } Bits;
  UINT8 Data;
} MSIXCAPID_IIO_VMD_STRUCT;


/* MSIXNXTPTR_IIO_VMD_REG supported on:                                       */
/*       SKX_A0 (0x1012D081)                                                  */
/*       SKX (0x1012D081)                                                     */
/* Register default value:              0xE0                                  */
#define MSIXNXTPTR_IIO_VMD_REG 0x11041081
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * MSI-X Next Pointer
 */
typedef union {
  struct {
    UINT8 next_ptr : 8;
    /* next_ptr - Bits[7:0], RO, default = 8'hE0 
       
                   This field is set to E0h for the next capability list (Power 
       Management capability structure) in the chain. 
                
     */
  } Bits;
  UINT8 Data;
} MSIXNXTPTR_IIO_VMD_STRUCT;


/* MSIXMSGCTL_IIO_VMD_REG supported on:                                       */
/*       SKX_A0 (0x2012D082)                                                  */
/*       SKX (0x2012D082)                                                     */
/* Register default value:              0x0020                                */
#define MSIXMSGCTL_IIO_VMD_REG 0x11042082
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * MSI-X Message Control
 */
typedef union {
  struct {
    UINT16 table_size : 11;
    /* table_size - Bits[10:0], RO, default = 11'h020 
       
                   System software reads this field to determine the MSI-X Table Size 
       N, which is encoded as N-1. 
                   VMD table size is 33, encoded as a value of 020h.
                
     */
    UINT16 rsvd : 3;
    /* rsvd - Bits[13:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT16 function_mask : 1;
    /* function_mask - Bits[14:14], RW, default = 'b0 
       
                   1: all the vectors associated with the VMD are masked, regardless of 
       the per vector mask bit state. 
                   0: each vector's mask bit determines whether the vector is masked or 
       not. 
                   Note: Since VMD shares the MSI-X table with the NTB iEP, it is 
       required to disable the MSI-X table at the sources for VMD and for NTB. For VMD, 
       the source of interrupts is from MSI's generated by VMD-Owned devices. For NTB, 
       the source of interrupts is from doorbell registers. 
                
     */
    UINT16 msi_x_enable : 1;
    /* msi_x_enable - Bits[15:15], RW, default = 'b0 
       
                   Software uses this bit to select between INTx or MSI or MSI-X method 
       for signaling interrupts from the VMD 
                   0: VMD is prohibited from using MSI-X to request service
                   1: MSI-X method is chosen for VMD interrupts
                   Note: Since VMD shares the MSI-X table with the NTB iEP, it is 
       required to disable the MSI-X table at the sources for VMD and for NTB. For VMD, 
       the source of interrupts is from MSI's generated by VMD-Owned devices. For NTB, 
       the source of interrupts is from doorbell registers. 
                
     */
  } Bits;
  UINT16 Data;
} MSIXMSGCTL_IIO_VMD_STRUCT;


/* TABLEOFF_BIR_IIO_VMD_REG supported on:                                     */
/*       SKX_A0 (0x4012D084)                                                  */
/*       SKX (0x4012D084)                                                     */
/* Register default value:              0x00000004                            */
#define TABLEOFF_BIR_IIO_VMD_REG 0x11044084
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * MSI-X Table Offset and BAR Indicator
 */
typedef union {
  struct {
    UINT32 table_bir : 3;
    /* table_bir - Bits[2:0], RO, default = 3'h4 
       
                   Indicates which one of a function's Base Address registers, located 
       beginning at 10h in Configuration Space, is used to map the function's MSI-X 
       Table intoMemory Space. 
                   BIR Value Base Address register
                   0: 10h
                   1: 14h
                   2: 18h
                   3: 1Ch
                   4: 20h
                   5: 24h
                   6: Reserved
                   7: Reserved
                   For a 64-bit Base Address register, the Table BIR indicates the 
       lower DWORD. 
                
     */
    UINT32 table_offset : 29;
    /* table_offset - Bits[31:3], RO, default = 'h00000000 
       
                   MSI-X Table Structure is at offset 0 from the MEMBASE2 address.
                
     */
  } Bits;
  UINT32 Data;
} TABLEOFF_BIR_IIO_VMD_STRUCT;


/* PBAOFF_BIR_IIO_VMD_REG supported on:                                       */
/*       SKX_A0 (0x4012D088)                                                  */
/*       SKX (0x4012D088)                                                     */
/* Register default value:              0x00001004                            */
#define PBAOFF_BIR_IIO_VMD_REG 0x11044088
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * MSI-X Pending Array Offset and BAR Indicator
 */
typedef union {
  struct {
    UINT32 pba_bir : 3;
    /* pba_bir - Bits[2:0], RO, default = 3'h4 
       
                   Indicates which one of a function's Base Address registers, located 
       beginning at 10h in Configuration Space, is used to map the function's MSI-X 
       Table intoMemory Space. 
                   BIR Value Base Address register
                   0: 10h
                   1: 14h
                   2: 18h
                   3: 1Ch
                   4: 20h
                   5: 24h
                   6: Reserved
                   7: Reserved
                   For a 64-bit Base Address register, the Table BIR indicates the 
       lower DWORD. 
                
     */
    UINT32 table_offset : 29;
    /* table_offset - Bits[31:3], RO, default = 29'h00000200 
       
                   MSI-X PBA Structure is at offset 4K (1K QWs) from the MEMBAR2 
       address. 
                
     */
  } Bits;
  UINT32 Data;
} PBAOFF_BIR_IIO_VMD_STRUCT;


/* PMCAP_IIO_VMD_REG supported on:                                            */
/*       SKX_A0 (0x4012D0E0)                                                  */
/*       SKX (0x4012D0E0)                                                     */
/* Register default value:              0x00030001                            */
#define PMCAP_IIO_VMD_REG 0x110440E0
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Power Management Capability
 */
typedef union {
  struct {
    UINT32 capability_id : 8;
    /* capability_id - Bits[7:0], RO, default = 8'h01 
       
                   Provides the PM capability ID assigned by PCI-SIG.
                
     */
    UINT32 next_capability_pointer : 8;
    /* next_capability_pointer - Bits[15:8], RO, default = 'h00 
       
                   This is the last capability in the chain and hence set to 0.
                
     */
    UINT32 version : 3;
    /* version - Bits[18:16], RO, default = 3'h3 
       
                   This field is set to 3h (PM 1.2 compliant) as version number for all 
       PCI Express ports. 
                
     */
    UINT32 pme_clock : 1;
    /* pme_clock - Bits[19:19], RO, default = 'b0 
       
                   Not applicable.
                
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[20:20], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 device_specific_initialization : 1;
    /* device_specific_initialization - Bits[21:21], RO, default = 'b0 
       
                   Not applicable.
                
     */
    UINT32 aux_current : 3;
    /* aux_current - Bits[24:22], RO, default = 'h0 
       
                   Not applicable.
                
     */
    UINT32 d1_support : 1;
    /* d1_support - Bits[25:25], RO, default = 'b0 
       
                   VMD does not support power management state D1.
                
     */
    UINT32 d2_support : 1;
    /* d2_support - Bits[26:26], RO, default = 'b0 
       
                   VMD does not support power management state D2.
                
     */
    UINT32 pme_support : 5;
    /* pme_support - Bits[31:27], RO, default = 'h00 
       
                   The Volume Management Device does not support generating PME#.
                
     */
  } Bits;
  UINT32 Data;
} PMCAP_IIO_VMD_STRUCT;


/* PMCSR_IIO_VMD_REG supported on:                                            */
/*       SKX_A0 (0x4012D0E4)                                                  */
/*       SKX (0x4012D0E4)                                                     */
/* Register default value:              0x00000008                            */
#define PMCSR_IIO_VMD_REG 0x110440E4
/* Struct format extracted from XML file SKX\1.5.5.CFG.xml.
 * Power Management Control and Status Register
 */
typedef union {
  struct {
    UINT32 power_state : 2;
    /* power_state - Bits[1:0], RW, default = 'h0 
       
                   This 2-bit field is used to determine the current power state of the 
       function and to set a new power state as well. 
                   00: D0
                   01: D1 (not supported by VMD)
                   10: D2 (not supported by VMD)
                   11: D3_hot
                   If Software tries to write 01 or 10 to this field, the power state 
       does not change from the existing power state (which is either D0 or D3hot) and 
       nor do these bits 1:0 change value. 
                   In the D3hot state, VMD will not respond to memory/IO transactions, 
       which is equivalent to MSE bit being clear. Also, VMD will not generate any 
       memory transactions as initiator. There is no hardware to enforce this. The VMD 
       driver will enforce this by programming all of the VMD-Owned devices to D3hot 
       when the OS writes 11b to this field. 
                   Notes: A write to this register will trigger an interrupt to the VMD 
       driver using the MSI table entry 0. 
                
     */
    UINT32 rsvd_2 : 1;
    /* rsvd_2 - Bits[2:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 no_soft_reset : 1;
    /* no_soft_reset - Bits[3:3], RO, default = 1'b1 
       
                   Indicates VMD does not reset its registers when transitioning from 
       D3hot to D0. 
                
     */
    UINT32 rsvd_4 : 4;
    /* rsvd_4 - Bits[7:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pme_enable : 1;
    /* pme_enable - Bits[8:8], RO, default = 'b0 
       
                   Not supported.
                
     */
    UINT32 data_select : 4;
    /* data_select - Bits[12:9], RO, default = 'h00 
       
                   Not relevant for IIO
                
     */
    UINT32 data_scale : 2;
    /* data_scale - Bits[14:13], RO, default = 'h00 
       
                   Not relevant for IIO
                
     */
    UINT32 pme_status : 1;
    /* pme_status - Bits[15:15], RO, default = 'b0 
       
                   Not supported.
                
     */
    UINT32 rsvd_16 : 6;
    /* rsvd_16 - Bits[21:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 b2_b3_support : 1;
    /* b2_b3_support - Bits[22:22], RO, default = 'b0 
       
                   This field is hardwired to 0h as it does not apply to PCI Express.
                
     */
    UINT32 bus_power_clock_control_enable : 1;
    /* bus_power_clock_control_enable - Bits[23:23], RO, default = 'b0 
       
                   This field is hardwired to 0h as it does not apply to PCI Express.
                
     */
    UINT32 data : 8;
    /* data - Bits[31:24], RO, default = 'h00 
       
                   Not relevant for IIO
                
     */
  } Bits;
  UINT32 Data;
} PMCSR_IIO_VMD_STRUCT;


#endif /* IIO_VMD_h */
