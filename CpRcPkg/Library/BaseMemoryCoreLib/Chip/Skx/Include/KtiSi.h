//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      KTI Reference Code                                                *
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
// *      This file contains all the Si specific KTI information.           *
// *                                                                        *
// **************************************************************************
**/

#ifndef _KTI_SI_H_
#define _KTI_SI_H_
#ifdef _MSC_VER
#pragma warning (disable: 4127 4214 4100)     // disable C4127: constant conditional expression
#endif
#include "DataTypes.h"
#include "PlatformHost.h"

/*********************************************************
                KTI Topology Related
*********************************************************/
#define SI_MAX_CPU_SOCKETS  8    // Maximum CPU sockets  supported by Si
#define SI_MAX_NIDS         8    // Maximum number of NIDs supported by SI
#define SI_MAX_KTI_PORTS    3    // Maximum KTI ports supported by Si
#define SOCKET_TYPE_CPU     0    // Type Cpu
#define SOCKET_TYPE_FPGA    1    // Type Fpga
#define SOCKET_LOCAL_NID    8    // Bit3 indicates local vs remote socket when programming NID

#ifndef MAX_FPGA_NID
#define MAX_FPGA_NID        3    // Max possible NID for system FPGA
#endif

#define SEGMENT_TYPE_FPGA   3    // capid0.segment identifier for FPGA sku
#define XEON_FPGA_PORT      2    // Xeon port used for FPGA
#define FPGA_PORT           0    // Port used by FPGA
#define FPGA_NID_OFFSET     2    // NID of connected FPGA is SID+2

/*********************************************************
                      IIO Stacks
*********************************************************/
#define IIO_CSTACK          0
#define IIO_PSTACK0         1
#define IIO_PSTACK1         2
#define IIO_PSTACK2         3
#define IIO_PSTACK3         4
#define IIO_PSTACK4         5
#define MAX_IIO_STACK       6

#define BUS_PER_MCP         2
#define BUS_PER_FPGA        6
#define BUS_PER_UBOX        1
#define BUS_PER_LEG_STK     20
#define BUS_PER_NONLEG_STK  2


/*********************************************************
                      CHA
*********************************************************/
#define MAX_CHA             28


#define TOTAL_KTI_AD_CREDIT        96
#define CHA_TO_KTI_VN0_AD_INGRESS  3
#define KTI_TO_KTI_VN0_AD_INGRESS  3
#define KTI_TO_KTI_VN1_AD_INGRESS  3
#define MAX_M2PCIE_TO_M3KTI_BL_VNA 8
#define TOTAL_K2K_AD_VNA           12
#define MIN_K2K_AD_VNA             9

#define TOTAL_KTI_BL_CREDIT       64
#define BL_VNA_SHARED_BY_IIO      4

#define CHA_TO_KTI_VN0_BL_INGRESS 4
#define KTI_TO_KTI_VN0_BL_INGRESS 4
#define KTI_TO_KTI_VN1_BL_INGRESS 4
#define MAX_BL_VNA_CREDIT_PER_CHA 2
#define TOTAL_K2K_BL_VNA           4
#define MIN_K2K_BL_VNA             7

// for CHA->M2M
#define TOTAL_M2M_AD_BL_CREDIT    56

// for CHA-> M2PCIE
#define MAX_CHA_TO_M2PCIE_BL_VNA  28


#define MAX_KTI_TO_M2PCIE_BL_VNA  9

// for M2PCIE-> CHA
#define TOTAL_PRQ                 18

//
// SAD/System Address Map related
//
#define MAX_SAD_TARGETS     32              // 32 targets for IO, IOAPIC, MMIOH
#define SADTGT_SIZE         4               // SAD target is 4 bits in size
#define SAD_BUS_GRANTY      32              // 32 if mmcfg <=256
#define SAD_BUS_TARGETS     8               // 8 mmcfg targets
#define SAD_LEG_IO_GRANTY   0x800           // 2KB
#define SAD_LEG_IO_PCH_SIZE 8               // Reserve 16KB for Legacy_Pch
#define SAD_IOAPIC_GRANTY   0x8000          // 32KB
#define SAD_MMIOL_GRANTY    0x400000        // 4MB
#define MAX_ADDR_LINE       46              // 46 Address bits are supported
#define IOAPIC_BASE         0xFEC00000
#define MMIOL_LIMIT         0xFBFFFFFF      // (4GB - 64MB - 1)
#define MMIOH_BASE          0x3800          // address above 4GB, so the 46 bits of physical address [45:0] is 0x3800_0000_0000  (65536G - 8192G)
#define MMIOH_MAX_GRANTY    0x5             // 2**(size*2) GB. Range is 1GB to 32TB
#define MAX_ADDR_LINES      46              // 2**46 is top of Core physical addressability limit


/*********************************************************
                      MC
*********************************************************/
#define MAX_MC          2
#define MAX_CLUSTERS             4          // Maximum number of clusters supported

/*********************************************************
                      M3KTI
*********************************************************/
#define MAX_M3KTI             2
#define MAX_PORT_IN_M3KTI     2
#define PORT_TO_M3KTI(PORT)   (PORT >> 1)
#define D2C_THLD_DFLT_SETTING 2

/*********************************************************
                      KTI Agent Link Layer
*********************************************************/

#define KTI_INVALID_PORT      0xFF

//
// Link Layer Init State Machine
//
typedef enum {
  LINK_SM_NOT_RDY_FOR_INIT           = 0,
  LINK_SM_PARAM_EX,
  LINK_SM_CRD_RETURN_STALL,
  LINK_SM_INIT_DONE,
  LINK_SM_MAX
} LINK_SM;

//
// KTI Agent Type Definition
//
typedef enum {
  KTI_CONFIG_AGENT                  = 0,
  KTI_FIRMWARE_AGENT,
  KTI_ROUTER_AGENT,
  KTI_AGENT_RSVD,
  KTI_LT_AGENT,
  KTI_IO_PROXY_AGENT,
  KTI_HOME_AGENT,
  KTI_CACHING_AGENT,
  KTI_MAX_AGENT
} KTI_AGENT_TYPE;


/*********************************************************
                      KTI Agent Phy Layer
*********************************************************/
#define CSIPHYPORT_STATE_RESET_CALIB        0x1
#define CSIPHYPORT_STATE_DETECT_CLKTERM     0x2
#define CSIPHYPORT_STATE_L0                 0xf
#define CSIPHYPORT_STATE_LOOPBACK_MASTER    0x12


#define CSI_LINK_DEFAULT_RESET_MODE         0x1
#define CSI_LINK_SOFT_RESET_MODE            0x0

#define MAX_KTI_BUNDLES      10
#define MAX_KTI_LANES        (2*MAX_KTI_BUNDLES)

/*********************************************************
                      UBOX
*********************************************************/
//
// Supported System configuration. This clasification is based on Credit setting, coherency setting
// and not based on topology alone. For example, a 3S topology will be treated as 4S config so that it
// can have same Credit and other coherency setting as 4S topology.
//
typedef enum {
  SYS_CONFIG_1S = 0,  // Single socket
  SYS_CONFIG_2S1L,
  SYS_CONFIG_2S2L,
  SYS_CONFIG_2S3L,    // 2S 3 LINKs  (TwoSkt3Link)
  SYS_CONFIG_4S,
  SYS_CONFIG_8S,
  SYS_CONFIG_MAX
} SYS_CONFIG;

//
// Macro to create mask. Ex. FLD_MASK(3, 5) --> 0000 0000 1110 0000
//
#define FLD_MASK(Size, Pos) ((~((~0) << (Size))) << (Pos))


//
// Should be removed once the ConfigDB issue with LEP0/1/2/3 is fixed and use the definition from auto generated header file
//
#pragma pack(1)
typedef union {
    struct {
    UINT32  KTIVersion : 4; // [3:0]   - 0x0 indicates KTI mode
    UINT32  Rsvd1 : 4;      // [7:4]   - Reserved but allow RW for late changes
    UINT32  SendingPort: 5; // [12:8]  - Corresponds to the port# on SKX. KTI Agent 0 encodes Port 0.
                            //           KTI Agent 1 encodes port 1. KTI Agent 2 encodes port 2.
    UINT32  Rsvd2 : 1;      // [13]    - Reserved but allow RW for late changes
    UINT32  LtAgent : 1;    // [14]    - LT Agent setting depends on LT-Strap (LTEnable).
    UINT32  FwAgent : 1;    // [15]    - Firmware Agent Present
    UINT32  NodeId  : 4;    // [19:16] - Based NodeID for the Socket. Not expected to need support for >8 NodeIDs.
    UINT32  Rsvd3   : 4;    // [23:20] - Reserved but allow RW for late changes
    UINT32  LlrWrap : 8;    // [31:24] - Equals the max LLR queue size - 1. Value does not change if LLRQ is reduced via configuration bits.  0x53=LLRQ size - 1 (decimal value is 83).
  } Fields;
  UINT32  RawData;
} KTI_LEP0;

typedef union {
  struct {
    UINT32  Rsvd1 : 32;     // [31:0]   - Reserved but allow RW for late changes
  } Fields;
  UINT32 RawData;
} KTI_LEP2;
#pragma pack()

/*********************************************************
                           PCU
*********************************************************/
//
// Ras types defined in ascending order of feature
//

#ifndef UNDEFINED_RAS
typedef enum {
  HEDT_RAS = 0x00,
  BASIC_RAS_AX,
  S1WS_RAS,
  CORE_RAS,
  STANDARD_RAS,
  FPGA_RAS,
  ADVANCED_RAS,
  UNDEFINED_RAS
}
RAS_TYPE;
#endif

//
// Wayness types
//
typedef enum {
  ONE_WAY_SOCKET,
  TWO_WAY_SOCKET,
  FOUR_WAY_SOCKET,
  EIGHT_WAY_SOCKET,
  UNDEFINED_WAYS
}
WAYS_TYPE;

//
// CPU type: Standard (no MCP), -F, etc
//
typedef enum {
  CPU_TYPE_STD,
  CPU_TYPE_F,
  CPU_TYPE_P,
  CPU_TYPE_MAX
} CPU_TYPE;

#define CPU_TYPE_STD_MASK (1 << CPU_TYPE_STD)
#define CPU_TYPE_F_MASK   (1 << CPU_TYPE_F)
#define CPU_TYPE_P_MASK   (1 << CPU_TYPE_P)

#define BGF_POINTER_OVERRIDE_MEM_NON_GV   0  // Uncore to Memory  SKX.U2D
#define BGF_POINTER_OVERRIDE_MEM_GV       1  // Memory to Uncore  SKX.D2U
#define BGF_POINTER_OVERRIDE_KTI_NON_GV   0  // Uncore to KTI     SKX.U2K
#define BGF_POINTER_OVERRIDE_KTI_GV       1  // KTI to Uncore     SKX.K2U
#define BGF_POINTER_OVERRIDE_IIO_NON_GV   0  // Uncore to IIO     SKX.U2I
#define BGF_POINTER_OVERRIDE_IIO_GV       1  // IIO to Uncore     SKX.I2U

//
// legacy PCI address for CSRs
//

//
// Sync the SBSP BUSNO to LEGACY_BUS_NO in ProcessorStartupUncore.inc
//
#define LEGACY_BUS0  0
#define LEGACY_BUS1  0x11


#define LEGACY_CSR_MMCFG_RULE_N0                  (0x80000000 | (LEGACY_BUS1 << 16) | (29 << 11) | (1 << 8) | 0xC0)  // CSR 1:29:1:0xc0
#define LEGACY_CSR_MMCFG_RULE_N1                  (0x80000000 | (LEGACY_BUS1 << 16) | (29 << 11) | (1 << 8) | 0xC4)  // CSR 1:29:1:0xC4
#define LEGACY_CSR_MMCFG_TARGET_LIST              (0x80000000 | (LEGACY_BUS1 << 16) | (29 << 11) | (1 << 8) | 0xEC)  // CSR 1:29:1:0xec
#define LEGACY_CSR_MMCFG_LOCAL_RULE_ADDRESS_CFG_0 (0x80000000 | (LEGACY_BUS1 << 16) | (29 << 11) | (1 << 8) | 0xC8)  // CSR 1:29:1:0xc8
#define LEGACY_CSR_MMCFG_LOCAL_RULE_ADDRESS_CFG_1 (0x80000000 | (LEGACY_BUS1 << 16) | (29 << 11) | (1 << 8) | 0xCC)  // CSR 1:29:1:0xcc
#define LEGACY_CSR_MMCFG_LOCAL_RULE               (0x80000000 | (LEGACY_BUS1 << 16) | (29 << 11) | (1 << 8) | 0xE4)  // CSR 1:29:1:0xE4

#define LEGACY_CSR_CPUBUSNO_UBOX                  (0x80000000 | (LEGACY_BUS0 << 16) | (8  << 11) | (2 << 8) | 0xCC)  // CSR 0:8:2:0xCC
#define LEGACY_CSR_CPUBUSNO1_UBOX                 (0x80000000 | (LEGACY_BUS0 << 16) | (8  << 11) | (2 << 8) | 0xD0)  // CSR 0:8:2:0xD0
#define LEGACY_CSR_CPUBUSNO_VALID_UBOX            (0x80000000 | (LEGACY_BUS0 << 16) | (8  << 11) | (2 << 8) | 0xD4)  // CSR 0:8:2:0xD4

#define LEGACY_CSR_CPUBUSNO_PCU                   (0x80000000 | (LEGACY_BUS1 << 16) | (30 << 11) | (4 << 8) | 0x94)  // CSR 1:30:4:0x94
#define LEGACY_CSR_CPUBUSNO1_PCU                  (0x80000000 | (LEGACY_BUS1 << 16) | (30 << 11) | (4 << 8) | 0x98)  // CSR 1:30:4:0x98

#define LEGACY_CSR_CPUBUSNO_VCU                   (0x80000000 | (LEGACY_BUS1 << 16) | (31 << 11) | (0 << 8) | 0xA0)  // CSR 1:31:0:0xA0
#define LEGACY_CSR_CPUBUSNO1_VCU                  (0x80000000 | (LEGACY_BUS1 << 16) | (31 << 11) | (0 << 8) | 0xA4)  // CSR 1:31:0:0xA4
#define LEGACY_CSR_CPUBUSNO_VALID_VCU             (0x80000000 | (LEGACY_BUS1 << 16) | (31 << 11) | (0 << 8) | 0xA8)  // CSR 1:31:0:0xA8

#endif // _KTI_SI_H_
