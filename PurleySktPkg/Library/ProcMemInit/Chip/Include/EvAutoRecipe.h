/*++
// **************************************************************************
// *                                                                        *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2016 Intel Corp.                             *
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
// *      This file contains EV related definitions.                        *
// *                                                                        *
// **************************************************************************
--*/

#ifndef __EV_AUTO_RECIPE_H__
#define __EV_AUTO_RECIPE_H__

#include <SysHostChip.h>
#define EV_AUTO_RECIPE_SIGNATURE  0x5F56455F

// Wild card definitions
//
#define WILDCARD_8     (0xFF)
#define WILDCARD_16    (0xFFFF)
#define WILDCARD_32    (0xFFFFFFFF)
#define WILDCARD_64    (0xFFFFFFFFFFFFFFFF)

typedef enum {
  DMI_CFG = 0,
  PCIE_CFG,
  WFR_CFG,
  SRIS_CFG,
  FPGA_CFG,
  KTI_CFG,
  LINK_CONFIG_TYPE_MAX
} LINK_CONFIG_TYPE;

typedef enum {
  TYPE_CSR = 0,
  TYPE_MSR,
  TYPE_IO,
  TYPE_KTI_IAR,
  TYPE_LBC_AFE,
  TYPE_LBC_COM,
  TYPE_LBC_COM_VCU,
  TYPE_CSR_VCU,
  ACCESS_TYPE_MAX,
} ACCESS_TYPE;

typedef enum {
  WINDOW1_IAR_IOVB = 1,
  WINDOW1_IAR_IOVB_BCAST,
  WINDOW1_IAR_IOVC,
  WINDOW1_SUB_TYPE_MAX,
} WINDOW1_SUB_TYPE;

#define A0_STEP A0_REV_SKX
#define A1_STEP A1_REV_SKX
#define A2_STEP A2_REV_SKX
#define B0_STEP B0_REV_SKX
#define B1_STEP B1_REV_SKX
#define L0_STEP L0_REV_SKX
#define H0_STEP H0_REV_SKX
#define M0_STEP M0_REV_SKX
#define U0_STEP U0_REV_SKX


#define J0_STEP H0_REV_SKX
#define V0_STEP U0_REV_SKX
#define H1_STEP H0_REV_SKX



typedef enum {
  KTI_RATE_96GT = 0,
  KTI_RATE_104GT,
  MAX_QPI_RATE,
} QPI_RATE_TYPE;

//Place holder for future use in PCIe
typedef enum {
  PCIE_RATE_25GT    = 0,
  PCIE_RATE_50GT,
  PCIE_RATE_80GT,
  MAX_PCIE_RATE,
} PCIE_RATE_TYPE;

typedef enum {
  SKX_SOCKET = 0,
  PROCESSOR_TYPE_MAX,
} PROCESSOR_TYPE;

//
// Steppings
//
#define Ax_REV  1 << A2_STEP | 1 << A1_STEP | 1 << A0_STEP
#define Bx_REV  1 << B1_STEP | 1 << B0_STEP | 1 << L0_STEP

#define Hx_REV  1 << H0_STEP | 1 << M0_STEP | 1 << U0_STEP

//
//  Macros to get Window 1 access's sub-type and address
//
#define WINDOW1_ACCESS_SUB_TYPE(x)	((x & 0xF0000000) >> 28)
#define WINDOW1_ACCESS_REG_ADDR(x)	(x & 0x3F)

//
//  Macros to form Window 1 based address from sub-type and reg addr
//
#define WINDOW1_ADDR(subtype, regaddr)  ((subtype << 28) | (regaddr & 0xFF))

#define REV_STR_CHARS                    8
#define HELP_STR_CHARS                   64

#pragma pack(1)
typedef struct {
  UINT32  Signature;                         // "_VE_"
  CHAR    Revision[REV_STR_CHARS];           // 1
  UINT16  NumberEntries;                     //
  UINT8   DebugMode;                         // 0 or 1
  CHAR    HelperString[HELP_STR_CHARS];      // fill in uniPhy, table rev and build date
} EV_RECIPE_HEADER;

typedef struct {
  UINT8           AccessType;            // One of ACCESS_TYPE
  UINT8           RegisterSize;          // size in bytes
  UINT16          Config;                 // 1 << one of LINK_CONFIG_TYPE, * = -1
  UINT32          RegisterAddress;       // 32 bit MMCFG encoded address | LBC load select
  UINT32          FieldMask;             // 32 bit field mask to AND field to zero
  UINT32          SocBitmap;             // * = -1, Else set bit for each valid socket to program
  UINT32          PortBitmap;            // * = -1, Else set bit for each port
  UINT32          LaneBitmap;            // * = -1, Else set bit for each lane (relevant to LBC only)
  UINT32          BitRates;              // Bitmask of rates. See Enum *_RATE_TYPE
  UINT64          Steppings;             // 1 << A0_REV_IVT, * = -1    //Will need to evaluate how we handle this dynamically
  UINT32          ProcessorType;         // SKX_SOCKET
  UINT16          Skus;                  // 1 << EP_SKU, * = -1
  UINT16          Chops;                 // Ignore for now
  UINT32          Data;                  // Value to write i.e data << start bit
} EV_RECIPE_ENTRY;

typedef union {
  struct {
    UINT32 LengthCode : 2;  // LengthCode - Bits[01:00]: 0 = 8-bits, 1 = 16-bits, 2 = 32-bits, 3 = 64-bits
    UINT32 Bar        : 2;  // BAR        - Bits[03:02]: Bar number (mmio only)
    UINT32 Bus        : 3;  // Bus        - Bits[06:04]: Bus Number (CSR), Instance number (mmio)
    UINT32 Reserved   : 1;  // RESERVED   - Bits[7]
    UINT32 Function   : 3;  // Function   - Bits[10:08]: Function Number
    UINT32 Device     : 5;  // Device     - Bits[15:11]: Device Number
    UINT32 Offset     :16;  // Offset     - Bits[31:16]: Register offset
  } Bits;
  UINT32 Data;
} VCU_API_DATA_FIELD;

#pragma pack()

#endif
