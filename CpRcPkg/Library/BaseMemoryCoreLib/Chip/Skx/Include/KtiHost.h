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
// *      This file contains the input parameter structure for KTI RC       *
// *      main entry point.                                                 *
// *                                                                        *
// **************************************************************************
**/

// Definition Flag:
//                  1. KTI_HW_PLATFORM   -> run with real hardware or SoftIVT
//                  2. KTI_SW_SIMULATION -> run with KTIRC Simulation
//                  3. IA32              -> run with IA32 mode


#ifndef _KTI_HOST_H_
#define _KTI_HOST_H_
#ifdef _MSC_VER
#pragma warning (disable: 4127 4214 4100)     // disable C4127: constant conditional expression
#endif
#include "DataTypes.h"
#include "PlatformHost.h"
#include "KtiSi.h"
#include "KtiDisc.h"

#pragma pack(1)

typedef INT32  KTI_STATUS;
#ifndef NULL
#define NULL  0
#endif
#define CONST       const
#define STATIC      static
#define VOID        void
#define VOLATILE    volatile
#define KTI_SUCCESS 0
#define KTI_REBOOT  1
#define KTI_SNC_CHANGED 2
#define KTI_FAILURE -1

//
// Warning log
//
#define MAX_WARNING_LOGS      16
#define WARNING_LOG_OVERFLOW  - 1

#define SR_LEGACY_BUSCFG_SYNC_CSR BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG
#define BUSCFGSYNC_PBSP_READY     0x0ABCDEF01 // Indicate PBSP ready for programming
#define BUSCFGSYNC_PBSP_GO        0x0ABCDEF02 // Indicate SBSP done, and PBSP should finish enabling
#define BUSCFGSYNC_PBSP_DONE      0x0ABCDEF03 // Indicate PBSP completed programming
#define BUSCFGSYNC_PBSP_CPL       0x000000000 // Release PBSP from Bus programming

typedef enum {
  NORMAL_OPERATION = 0,
  RECOVERY_OPERATION
} SNC_COLDRESET_REGISTER_OPERATION_TYPE;

typedef enum {
  KTI_GROUP = 0,
} GROUP_TYPE;

/*********************************************************
                KTIRC Host Structure Related
*********************************************************/

typedef enum {
  KTI_LINK0 =  0x0,
  KTI_LINK1,
  KTI_LINK2
} KTI_LOGIC_LINK;

typedef enum {
  FULL_SPEED = 0,
  HALF_SPEED
} KTI_LINK_SPEED_TYPE;

//
// LL credit allocation
//
#define  KTI_LL_VNA_NORMAL_SKX       66 // if KTI_LL_VNA_SETUP_MAX requested and Vn1=0
#define  KTI_LL_VNA_WITH_VN1_SKX     48 // if KTI_LL_VNA_SETUP_MAX requested and Vn1=1
#define  KTI_LL_VNA_MIN              0  // if KTI_LL_VNA_SETUP_MIN requested
#define  KTI_LL_VNA_FGPA_SKX         75 // KTI FPGA LINK VNA credits Maxmize (VN1 disable, VN0.NCS, VN0.NCB and VN0.SNP are disabled)

#define MSR_IRQ_VMSR_64H             0x0064
#define   MASK_IRQ_VMSR_IRQ_TH       0x0000001F  // bits [4:0] are R/W

#define IRQ_THRESHOLD_DIS             0x1F
#define IRQ_THRESHOLD_4SRING_DEFAULT  0xA
#define IRQ_THRESHOLD_8S_DEFAULT      0xC
#define RRQ_IRQ_THRESHOLD_DIS         0x1F1F

#define RRQ_IRQ_2S4SFC_LOW            0x061F                //RRQ=6, IRQ=0x1F
#define RRQ_IRQ_2S4SFC_MEDIUM         0x071F                //RRQ=7, IRQ=0x1F
#define RRQ_IRQ_2S4SFC_HIGH           0x0A1F                //RRQ=6, IRQ=0x1F
#define RRQ_IRQ_2S4SFC_AUTO           RRQ_IRQ_2S4SFC_MEDIUM

#define RRQ_IRQ_4SRING_LOW            0x070C                //RRQ=7, IRQ=0x0C
#define RRQ_IRQ_4SRING_MEDIUM         0x070E                //RRQ=7, IRQ=0x0E
#define RRQ_IRQ_4SRING_HIGH           0x090E                //RRQ=7, IRQ=0x0C
#define RRQ_IRQ_4SRING_AUTO           RRQ_IRQ_4SRING_MEDIUM

#define RRQ_IRQ_8S_LOW                0x070A                //RRQ=7, IRQ=0x0A
#define RRQ_IRQ_8S_MEDIUM             0x070C                //RRQ=7, IRQ=0x0C
#define RRQ_IRQ_8S_HIGH               0x080E                //RRQ=8, IRQ=0x0E
#define RRQ_IRQ_8S_AUTO               RRQ_IRQ_4SRING_MEDIUM


//
// Definitions to be used in Eparam tables:
//
typedef enum {
  PER_LANES_TXEQ_ENABLED   = 0,         // each lane use different TXEQ value
  ALL_LANES_TXEQ_ENABLED                // all lanes use same TXEQ value
} LANE_TXEQ_TYPE;

//
// Number of Clusters.
//
typedef enum {
  CLUSTER_MODE_1,
  CLUSTER_MODE_2,
} CLUSTER_MODE;

typedef struct {
   UINT8      Enabled;
   UINT16     McMemSize;
   UINT16     ChannelMemSize[MAX_MC_CH];
   UINT32     MemBase;
   UINT32     MemLimit;
   UINT32     ChannelInterBitmap;
   UINT32     FirstPfTadID;
   UINT32     SecondPfTadID;
} MC_INFO;

typedef struct {
   UINT8      NumOfMcEnabled;
   UINT8      NumOfChaPerCluster;
   UINT8      NumOfCluster;         //0: N/A       1: 1 cluster  2: 2 clusters
   UINT8      InterleaveEn;         //0: NUMA-non-interleave-cross-MC 1: NUMA interleave-cross-MC (for 1LM/XPT_Prefetch only)
                                    //   If 2LM, it must be NUMA
   UINT8      Interleave_256B;      //0: interleave 64B   1: Interleave 256B  if InterleaveEn = 1
   UINT8      PrefetchEn;           //0: KTI and XPT capability Disabled  1: Enabled
} SNC_INFO;

typedef struct {
  MC_INFO    McInfo[MAX_SOCKET][MAX_IMC];
  SNC_INFO   SncInfo[MAX_SOCKET];
  UINT32     ClusterMemBase[MAX_SOCKET][MAX_CLUSTERS+1];
  UINT8      SocketIdWithFirst4G;
  UINT8      PrefetchAll;
} MEM_INFO;

typedef enum {
  LCC = 0, // 10c
  MCC,     // 14c
  HCC,     // 22c
  XCC,     // 28c
  MAX_CHOP_TYPES
} PHYSICAL_CHOP;

typedef struct {

  UINT8   DfxSystemWideParmStart;           // This must be the first variable
  UINT8   DfxHaltLinkFailReset;             // 2 - Auto; 1 - Enable; 0 - Disable  (Auto);
  UINT8   DfxKtiMaxInitAbort;               // 0 - Disable; 1 - Enable; 2 - AUTO (default)
  UINT8   DfxLlcShareDrdCrd;                // Enable migration from SF to LLC and to leave shared lines in the LLC for Drd and Crd: 1 - Enable; 1 - Disable; 2 - Auto (default)
  UINT8   DfxBiasFwdMode;                   // 0 - Mode 0 (Fwd only when Hom != Req); 1 - Mode 1 (Fwd when Hom != Req & Hom != Local); 2 - Mode 2 (Disable Bias Fwd) ..  5 - Auto (Default)
  UINT8   DfxSnoopFanoutEn;                 // Snoop fanout enable 0: disable 1: enable 2 - Auto (default)
  UINT8   DfxHitMeEn;                       // CHA HitME$ Enable: 1 - Enable; 0 - Disable; 2 - Auto (Default)
  UINT8   DfxFrcfwdinv;                     // Enable alias all conflict flows to FwdInvItoE behaviour: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8   DfxDbpEnable;                     // Dbp Enable : 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8   DfxOsbEn;                         // OSB Enable: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8   DfxHitMeRfoDirsEn;                // Enable HitME DIR=S RFO optimization: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8   DfxGateOsbIodcAllocEn;            // When OSB indicates that there aren't enough snoop credits don't allocate IODC entry: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8   DfxDualLinksInterleavingMode;     // In 2S 2KTI can: 2 - Auto - do nothing (default); 1 - use legacy CHA interleaving (disable SNC, turn off XOR interleave); 0 - disable D2C  
  UINT8   DfxSystemDegradeMode;             // 0 - Degrade to 1S; 1 - Degarde to supported topology (default); 2 - Leave the topology as is.
  UINT8   DfxVn1En;                         // VN1 enable 0: disable 1: enable 2 - Auto (default) (8S parts only)
  UINT8   DfxD2cEn;                         // Direct To Core enable: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8   DfxD2kEn;                         // Direct To Kti enable: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8   DfxSystemWideParmEnd;             // This must be the last variable

} KTI_DFX_PARM;





//
// DFX Link layer Options (per Port)
//
typedef struct {
  UINT32  DfxCrcMode:2;           // CRC_MODE_16BIT, CRC_MODE_ROLLING_32BIT or CRC_MODE_AUTO
  UINT32  DfxL0pEnable:2;         // Value to determine if this port of the link may enter L0p, but both sides must agree to enter L0p.  Default uses system wide setting
  UINT32  DfxL1Enable:2;          // Value to determine if this port of the link may enter L1, but both sides must agree to enter L1.  Default uses system wide setting
  UINT32  Rsvd1 : 26;
} KTI_DFX_CPU_LINK_SETTING;

//
// DFX Phy Layer Options (per Port) - Currently unused; A place holder for future extension
//
typedef struct {
  UINT8  DfxKtiFailoverEn : 2; // 0 - Disable; 1 - Enable; 3 - Auto (Default)
  UINT8  Rsvd1 : 6;
} KTI_DFX_CPU_PHY_SETTING;

//
// Per CPU Port DFX settings
//
typedef struct {
  KTI_DFX_CPU_LINK_SETTING Link[MAX_KTI_PORTS];
  KTI_DFX_CPU_PHY_SETTING  Phy[MAX_KTI_PORTS];
} KTI_DFX_CPU_SETTING;

//
// PHY settings that are system dependent.   Need 1 of these for each socket/link/freq.
//

typedef struct {
  UINT8  SocketID;
  UINT8  AllLanesUseSameTxeq;
  UINT8  Freq;
  UINT32 Link;
  UINT32 TXEQL[20];
  UINT32 CTLEPEAK[5];
} PER_LANE_EPARAM_LINK_INFO;

//
// This is for full speed mode, all lanes have the same TXEQ setting
//
typedef struct {
  UINT8  SocketID;
  UINT8  Freq;
  UINT32 Link;
  UINT32 AllLanesTXEQ;
  UINT8  CTLEPEAK;
} ALL_LANES_EPARAM_LINK_INFO;

#define ADAPTIVE_CTLE 0x3f
#define PER_LANE_ADAPTIVE_CTLE 0X3f3f3f3f

typedef struct {
  //
  //Credits to be programmed in KTIA2RCRCTRL
  //
  UINT8 egrndr_credits;
  UINT8 egrhom_credits;
  UINT8 egrsnp_credits;

  //
  // Credits to be programmed in M3KRTE0CR/M3KRTE1CR for KTI -> KTI communication.
  // Except AD/BL VNA credits, other credits are per VN (i.e VN0 & VN1). But both VNs
  // have same settings.
  //
  UINT8 advnakti;
  UINT8 snpkti;
  UINT8 homkti;
  UINT8 ndrkti;
  UINT8 blvnakti;
  UINT8 drskti;
  UINT8 ncskti;
  UINT8 ncbkti;

  //
  //Credits to be programmed in M3KPCIR0CR/M3KPCIR1CR for M3KTI -> M2PCIe communication
  //
  UINT8 pcicredit;
} M3KTI_RING_CREDIT_PARAM;


//
// Stack resource alloation info
//
typedef struct {
  UINT8         Personality;  // Ubox, ubox+iio, etc of this stack
  UINT8         BusBase;      // Bus base of each stack
  UINT8         BusLimit;     // Bus limit of each stack
  UINT16        IoBase;       // IO base of each stack
  UINT16        IoLimit;      // IO limit for each stack
  UINT32        IoApicBase;   // IoApic base of each stack
  UINT32        IoApicLimit;  // IoApic limit for each stack
  UINT32        MmiolBase;    // Mmiol base of each stack
  UINT32        MmiolLimit;   // Mmiol limit of each stack
  UINT64_STRUCT MmiohBase;    // Mmioh base of each stack
  UINT64_STRUCT MmiohLimit;   // Mmioh limit of each stack
} KTI_STACK_RESOURCE;

typedef enum {
  TYPE_UBOX = 0,
  TYPE_UBOX_IIO,
  TYPE_MCP,
  TYPE_FPGA,
  TYPE_DISABLED,              // This item must be prior to stack specific disable types
  TYPE_UBOX_IIO_DIS,
  TYPE_MCP_DIS,
  TYPE_FPGA_DIS,
  TYPE_NONE
} STACK_TYPE;

//
// Socket resource allocation info
//
typedef struct {
  UINT8               PciSegment;
  UINT8               TotEnabledStacks;
  UINT8               BusBase;
  UINT8               BusLimit;
  UINT16              IoBase;
  UINT16              IoLimit;
  UINT32              IoApicBase;
  UINT32              IoApicLimit;
  UINT32              MmiolBase;
  UINT32              MmiolLimit;
  UINT64_STRUCT       MmiohBase;
  UINT64_STRUCT       MmiohLimit;
  UINT64_STRUCT       SegMmcfgBase;
  UINT8               stackPresentBitmap;           // bitmap of present stacks per socket
  KTI_STACK_RESOURCE  StackRes[MAX_IIO_STACK];
} KTI_CPU_RESOURCE;

//M3KTI
typedef struct {
  UINT32           M3Egrerrlog0;
  UINT32           M3Egrerrlog1;
  UINT32           M3Ingerrlog0;
  UINT32           M3Ingerrlog1;
} KTI_CPU_M3KTI_ERRLOG;

//M2PCIe
typedef struct {
  UINT32           R2Egrerrlog;
  UINT32           R2Ingerrlog0;
  UINT32           R2Egrerrlog2;
} KTI_CPU_M2PCIE_ERRLOG;


typedef struct {
  KTI_CPU_M3KTI_ERRLOG    M3Kti;
  KTI_CPU_M2PCIE_ERRLOG   M2Pcie;
} KTI_CPU_ERRLOG;

// CPU socket KTI info
//
typedef struct {
#ifndef ASM_INC
  UINT32              Valid : 1;
  UINT32              SocId : 3;
  UINT32              SocType : 2;                    // Socket Type
  UINT32              Rsvd1 : 26;
#else
  UINT32              RawData;
#endif // ASM_INC
  KTI_LINK_DATA       LepInfo[MAX_KTI_PORTS];         // KTI LEP info
  UINT8               TotM2Pcie;                      // Tot M2Pcie Enabled
  UINT8               TotM3Kti;
  UINT8               TotCha;
  UINT32              ChaList;
  UINT8               CpuType;
  TOPOLOGY_TREE_NODE  TopologyInfo[MAX_SOCKET];       // Topology info describing how this soecket is connected to all other sockets
  KTI_CPU_RESOURCE    CpuRes;                         // System resources assigned per CPU
  KTI_CPU_ERRLOG      CpuErrLog;                      // Log of errors that occured during previous boot
} KTI_CPU_INFO;


//
// Link layer settings, per link
//
typedef struct {
  UINT8 KtiPortDisable:1;     // TRUE - Port disabled;    FALSE- Port enabled (default)
  UINT8 KtiLinkVnaOverride:7; // Numeric value 0x00-0x7f
  UINT8 Rsvd:8;
} KTI_CPU_LINK_SETTING;


//
// Phy general setting, per link
//
typedef struct {
  UINT32  KtiLinkSpeed:3;
  UINT32  Rsvd:29;
} KTI_CPU_PHY_SETTING;

//
// Per CPU setting
//
typedef struct {
  KTI_CPU_LINK_SETTING  Link[MAX_KTI_PORTS];
  KTI_CPU_PHY_SETTING   Phy[MAX_KTI_PORTS];
} KTI_CPU_SETTING;


//
// KTIRC input structure
//
typedef struct {
  //
  // Protocol layer and other general options; note that "Auto" is provided only options whose value will change depending
  // on the topology, not for all options.
  //

  //
  // Indicates the ratio of Bus/MMIOL/IO resource to be allocated for each CPU's IIO.
  // Value 0 indicates, that CPU is not relevant for the system. If resource is
  // requested for an CPU that is not currently populated, KTIRC will assume
  // that the ratio is 0 for that CPU and won't allocate any resources for it.
  // If resource is not requested for an CPU that is populated, KTIRC will force
  // the ratio for that CPU to 1.
  //


  UINT8               BusRatio[MAX_SOCKET];

  UINT8               LegacyVgaSoc;       // Socket that claims the legacy VGA range; valid values are 0-7; 0 is default.
  UINT8               LegacyVgaStack;     // Stack that claims the legacy VGA range; valid values are 0-3; 0 is default.
  UINT8               ColdResetRequestStart;
  UINT8               MmioP2pDis;         // 1 - Disable (default); 0 - Enable
  UINT8               DebugPrintLevel;    // Bit 0 - Fatal, Bit1 - Warning, Bit2 - Info Summary; Bit 3 - Info detailed. 1 - Enable; 0 - Disable
  UINT8               SncEn;              // 0 - Disable, (default) 1 - Enable
  UINT8               IoDcMode;           // 0 - Disable IODC,  1 - AUTO (default), 2 - IODC_EN_REM_INVITOM_PUSH, 3 - IODC_EN_REM_INVITOM_ALLOCFLOW
                                          // 4 - IODC_EN_REM_INVITOM_ALLOC_NONALLOC, 5 - IODC_EN_REM_INVITOM_AND_WCILF
  UINT8               DegradePrecedence;  // Use DEGRADE_PRECEDENCE definition; TOPOLOGY_PRECEDENCE is default
  UINT8               DirectoryModeEn;    // 1 - Disable; 0 - Enable (default)
  UINT8               XptPrefetchEn;      // Xpt Prefetch :  1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8               KtiPrefetchEn;      // Kti Prefetch :  1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8               RdCurForXptPrefetchEn;   // RdCur for XPT Prefetch :  0 - Disable, 1 - Enable, 2- Auto (default)
  UINT8               KtiFpgaEnable[MAX_SOCKET];  // Indicate if should enable Fpga device found in this socket :  0 - Disable, 1 - Enable, 2- Auto

  //
  // Phy/Link Layer Options (System-wide and per socket)
  //
  UINT8               KtiLinkSpeedMode;   // Link speed mode selection; 0 - Slow Speed; 1- Full Speed (default)
  UINT8               KtiLinkSpeed;       // Use KTI_LINKSPEED definition
  UINT8               KtiLinkL0pEn;       // 0 - Disable, 1 - Enable, 2- Auto  (default)
  UINT8               KtiLinkL1En;        // 0 - Disable, 1 - Enable, 2- Auto  (default)
  UINT8               KtiFailoverEn;      // 0 - Disable, 1 - Enable, 2- Auto (default)
  UINT8               KtiLbEn;            // 0 - Disable(default), 1 - Enable
  UINT8               KtiCrcMode;         // CRC_MODE_16BIT, CRC_MODE_ROLLING_32BIT, CRC_MODE_AUTO or CRC_MODE_PER_LINK

  UINT8               KtiCpuSktHotPlugEn;       // 0 - Disable (default), 1 - Enable
  UINT8               KtiCpuSktHotPlugTopology; // 0 - 4S Topology (default), 1 - 8S Topology
  UINT8               KtiSkuMismatchCheck;      // 0 - No, 1 - Yes (default)
  UINT8               IrqThreshold;             // IRQ Threshold setting
  UINT8               TscSyncEn;                // TSC sync in sockets: 0 - Disable, 1 - Enable, 2 - AUTO (Default)
  UINT8               StaleAtoSOptEn;           // HA A to S directory optimization: 1 - Enable; 0 - Disable; 2 - Auto (Default)
  UINT8               LLCDeadLineAlloc;         // LLC dead line alloc: 1 - Enable(Default); 0 - Disable
  UINT8               ColdResetRequestEnd;

  //
  // Phy/Link Layer Options (per Port)
  //
  KTI_CPU_SETTING     PhyLinkPerPortSetting[MAX_SOCKET];

  KTI_DFX_PARM        DfxParm;

  KTI_DFX_CPU_SETTING  DfxCpuCfg[MAX_SOCKET];  // Per CPU socket specific config info
} KTI_HOST_IN;

typedef struct{
  //
  // These variables are initialized and used like global variable by KTIRC. Not meant for outside KTIRC usage.
  //
  UINT8         CurrentReset;                  // Reset type witnessed by the system (viz PWRGD, WARM)
  UINT8         SbspSoc;                       // System BSP socket ID
  UINT8         SocketWays[MAX_SOCKET];        // Indicates NIDs that are allowed
  UINT8         RasType[MAX_SOCKET];           // Ras type supported by the socket
  UINT8         TotCha[MAX_SOCKET];            // Total Chas/LLCs; All CPUs must have same number of Cbo/LLCs
  UINT32        ChaList[MAX_SOCKET];           // List of Chas/LLCs
  UINT8         TotIio[MAX_SOCKET];            // Total Iio; All CPUs must have same number of IioLink
  UINT8         TotM2Mem[MAX_SOCKET];          // Total M2Mem; All CPUs must have same number of M2Mem
  UINT8         TotKtiAgent[MAX_SOCKET];       // Total KTIAgents; All CPUs must have same number of KtiAgents
  UINT8         M2PcieActiveBitmap[MAX_SOCKET]; //bitmap of activet m2pcie stop with IIO stack per socket
  UINT8         TotActiveM2Pcie[MAX_SOCKET];   // Total active M2PCIE with IIO stack enabled
  UINT8         TotSadTargets;                 // Count of all SAD targets in the system
  UINT32        MmcfgLocalRules[MAX_SOCKET][3];// Var to store programming to reuse in KTI/IIO BusId programming
  UINT8         TotCpu;                        // Total CPUs populated in the system
  UINT32        CpuList;                       // List of CPUs populated in the system
  UINT32        FpgaList;                      // List of Fpga populated in the system
  UINT8         ProgramCreditFpga;
  UINT8         OrigTotCpu;                    // Total CPUs populated in the system before topology reduction
  UINT32        OrigCpuList;                   // List of CPUs populated in the system before topology reduction
  UINT8         SysOsbEn;                      // OSB Enable: 1 - Enable; 0 - Disable
  UINT8         SysDirectoryModeEn;            // Directory Mode Enable: 1 - Enable; 0 - Disable
  UINT8         SnoopFanoutEn;                 // Enable snoop fanout
  UINT8         D2cEn;                         // Direct To Core Enable: 1 - Enable; 0 - Disable
  UINT8         D2kEn;                         // Direct To KTI Enable:  1 - Enable; 0 - Disable
  BOOLEAN       ForceColdResetFlow;            // Force cold reset flow
  BOOLEAN       TwistedKtiLinks;               // Indicates if the links are twisted in 2S config
  BOOLEAN       Is4SRing;                      // Indicates if 4S ring
  BOOLEAN       IsRouteThrough;                // Indicates if 3S chain, 4s-chain, 4S-ring, 8S topology
  UINT8         MmiolGranularity;              // min mmiol granualarity to each stack
  UINT8         MmiohGranularity;              // min mmioh granualarity to each stack
  UINT16        MmiohTargetSize;               // mmioh target granularity in use  0-5

  UINT8         KtiCpuSktHotPlugEn;            // hot plug enabled or not.  Only valid for hotplug capable parts

  UINT32        MmcfgBase;                     // Local storage of mmcfgbase
  BOOLEAN       IsMmcfgAboveMmiol;             // Track relative location of mmcfgbase
  BOOLEAN       UseNonInterleavedMmioh;        // Indicate if Mmioh allocation requires use of both decoders
  UINT8         DualLinksInterleavingMode;     // In 2S 2KTI can: 2 - Auto - do nothing (default); 1 - use legacy CHA interleaving (disable SNC, turn off XOR interleave); 0 - disable D2C
  UINT8         FpgaEnable[MAX_SOCKET];        // Indicate if should enable Fpga device found in this socket :  0 - Disable, 1 - Enable
  BOOLEAN       ProgramPhyLink;                // TRUE to indicate to Phy/Link code to program Phy/Link
  UINT8         SnoopFilter;                   // Enable:Disable snoopfilter
  KTI_CPU_SETTING PhyLinkPerPortSetting[MAX_SOCKET];  //Local LinkSpeed
  UINT8         TSCSyncEn;                     // TSC sync in sockets: 0 - Disable, 1 - Enable, 2 - AUTO (Default, enable by LR board)
} KTI_HOST_INTERNAL_GLOBAL;

typedef struct{
  UINT32            SizeOfCpuRes;
  UINT32            CpuList;
  UINT8             Sbsp;
  UINT32            mmcfgSize;
  UINT64_STRUCT     mmiohGranularity;
} KTI_OEMCALLOUT;

//
// KTIRC output structure
//
typedef struct {


  //
  // All CPUs are assigned pre-determined array element, viz, element 0 is for CPU0,
  // element 1 is for CPU1 etc. There is valid bit to track if the CPU is populated or not.
  //
  KTI_CPU_INFO  CpuInfo[MAX_SOCKET];           // KTI related info per CPU
  UINT8         SysConfig;                     // System Topology
  UINT32        WarningLog[MAX_WARNING_LOGS];  // Warning Logs

  //
  // There is a corresponding input option for these options. If the input option can not be met,
  // due to the current system topology not allowing it, the options will be forced to supported
  // value and platform BIOS can use these variables to know if any such overriding has happened.
  //
  UINT8         OutLegacyVgaSoc;                // Socket that claims the legacy VGA range
  UINT8         OutLegacyVgaStack;              // Stack that claims the legacy VGA range
  UINT8         OutIsocEn;                      // 1 - Enable; 0 - Disable (BIOS will force this for 4S)
  UINT8         KtiCurrentLinkSpeedMode;        // Current link speed mode; 0 - Slow Speed; 1- Full Speed.
  UINT8         OutKtiLinkSpeed;                // Link speed/freq
  UINT8         OutPerLinkSpeed[MAX_SOCKET][MAX_KTI_PORTS]; // Per link speed
  UINT8         OutKtiLinkL0pEn;                // 0 - Disabled, 1 - Enabled
  UINT8         OutKtiLinkL1En;                 // 0 - Disabled, 1 - Enabled
  UINT8         OutKtiFailoverEn;               // 0 - Disabled, 1 - Enabled
  UINT8         OutKtiCpuSktHotPlugEn;          // 0 - Disabled, 1 - Enabled
  UINT8         OutSystemRasType;               // See RAS_TYPE enum for defintions

  UINT8         OutKtiCrcMode;                  // CRC_MODE_16BIT, CRC_MODE_ROLLING_32BIT
  UINT8         OutVn1En;                       // 0 - Disabled, 1 - Enabled
  UINT8         OutIsRouteThrough;              // 0 - Disabled, 1 - Enabled
  UINT8         OutHitMeEn;                     // 0 - Disabled, 1 - Enabled
  UINT8         OutIoDcMode;                    // 0 - Disable IODC,  1 - AUTO (default), 2 - IODC_EN_REM_INVITOM_PUSH, 3 - IODC_EN_REM_INVITOM_ALLOCFLOW
                                              // 4 - IODC_EN_REM_INVITOM_ALLOC_NONALLOC, 5 - IODC_EN_REM_INVITOM_AND_WCILF
  UINT8         OutSncEn;                       // 0 - Disabled, 1 - Enabled
  UINT8         OutNumOfClusterPerSystem;       // 0 - N/A, 1: 1 cluster, 2: 2 clusters ( per system) Number of Cluster per socket for SNC enable
  UINT8         OutNumOfCluster[MAX_SOCKET];    // Number of Cluster per socket for SNC enable
  UINT8         OutBoardVsCpuConflict;          // 8-bit bitmask of CPUs that have a conflict detected with a board
  UINT16        OutIoGranularity;               // Io Granularity
  UINT32        OutMmiolGranularity;            // Mmiol Granularity
  UINT64_STRUCT OutMmiohGranularity;            // Mmioh Granularity
  BOOLEAN       OutKtiPerLinkL1En[SI_MAX_CPU_SOCKETS][SI_MAX_KTI_PORTS]; // output kti link enabled status for PM
  UINT8         OutKtiPrefetch;                 // 0 - Disabled; 1 - Enabled
  UINT8         OutKtiFpgaEnable[MAX_SOCKET];   // Indicate fpga enabled in this socket - 0 - Disable, 1 - Enable
  UINT8         OutKtiFpgaPresent[MAX_SOCKET];  // Indicate fpga present in this socket - 0 - Disable, 1 - Enable
  UINT8         OutRrqThreshold;                 // RRQ threshold
  UINT8         OutIRQThreshold;                 // IRQ RRQ threshold
  UINT8         OutSncGbAlignRequired;           // 0 - no GB Alignment required, 1 - GB Alignment required
  UINT8         OutSncPrefetchEn;                // OR the SNC and XPT KTI prefetch

  //
  // These variables are initialized and used like global variable by KTIRC.
  // Not meant for outside KTIRC consumption.
  //
  UINT8         RasInProgress;                   // Set to TRUE when the KTIRC is called to handle a CPU/IOH O*L event
  UINT8         RasEvtType;                      // 0 - Online; 1 - Offline
  UINT8         RasSocId;                        // RAS event socket ID
  BOOLEAN       ProgramNonBC;                    //Flag to indicate if the Non BC RTA entries to be programmed
  BOOLEAN       ProgramBC;                       //Flag to indicate if the BC RTA entries to be programmed
  BOOLEAN       Is4SRing;                        // Indicates if 4S ring
} KTI_HOST_OUT;


typedef struct {
  UINT32            Cpus;           // List of CPUs populated in the system
  UINT32            Chas;           // List of Chas/LLCs in SBSP
  KTI_LINK_DATA     LepInfo[MAX_SOCKET][MAX_KTI_PORTS];  // KTI LEP info
  KTI_HOST_IN       SaveSetupData;                       // Kti Setup save in variable
} KTI_HOST_NVRAM;


#pragma pack()

#endif // _KTI_HOST_H_
