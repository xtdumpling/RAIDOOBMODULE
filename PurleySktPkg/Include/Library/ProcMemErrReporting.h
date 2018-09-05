/*
 * ProcMemErrReporting.h
 *
 *  Created on: Jun 3, 2014
 *      Author: mganesan
 */

#ifndef PROCMEMERRREPORTING_H_
#define PROCMEMERRREPORTING_H_

#include <Uefi.h>
#include <Protocol/CpuCsrAccess.h>
#include <PiDxe.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/MemRasProtocol.h>
#include <Library/DebugLib.h>
#include <CpuRegs.h>
#include <Protocol/IioUds.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/LocalApicLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/MpService.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/ErrorHandlingProtocol.h>
#include <Protocol/QuiesceProtocol.h>
#include <Library/SynchronizationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Protocol/MemRasProtocol.h>
#include <MemHost.h>
#include "ErrorRecords.h"
#include <mca_msr.h>
#include <WheaPlatformDefs.h> // RASCM_TODO - this should be removed later

//
// Defines
//
#define MC_SCOPE_TD    0x0
#define MC_SCOPE_CORE  0x1
#define MC_SCOPE_PKG   0x2
#define MCSCOPE_MASK  (BIT0 | BIT1)

#define EMCA_LOG_SUPPORT (BIT2)

#define EMCA_SECTYPE_NONE       0x0
#define EMCA_SECTYPE_PROCGEN    0x1
#define EMCA_SECTYPE_PROCIA32   0x2
#define EMCA_SECTYPE_PROCIPF    0x3
#define EMCA_SECTYPE_PLATMEM    0x4
#define EMCA_SECTYPE_PCIE       0x5
#define EMCA_SECTYPE_FWERR      0x6
#define EMCA_SECTYPE_PCIBUS     0x7
#define EMCA_SECTYPE_PCIDEV     0x8
#define EMCA_SECTYPE_DMAR       0x9
#define EMCA_SECTYPE_VTIO       0xA
#define EMCA_SECTYPE_IOMMU      0xB
#define EMCA_SECTYPE_RAW        0xC
#define EMCA_SECTYPE_IIO        0xD

#define EMCA_ENABLE_SWSMI       0x8E
#define EMCA_DISABLE_SWSMI      0x8D

#define MCA_UNIT_TYPE_IFU   (BIT0)
#define MCA_UNIT_TYPE_DCU   (BIT1)
#define MCA_UNIT_TYPE_DTLB  (BIT2)
#define MCA_UNIT_TYPE_MLC   (BIT3)
#define MCA_UNIT_TYPE_PCU   (BIT4)
#define MCA_UNIT_TYPE_KTI   (BIT5)
#define MCA_UNIT_TYPE_IIO   (BIT6)
#define MCA_UNIT_TYPE_CHA   (BIT7)
#define MCA_UNIT_TYPE_M2MEM (BIT8)
#define MCA_UNIT_TYPE_IMC   (BIT9)

#define MCA_UNIT_IFU0         (MCA_UNIT_TYPE_IFU)
#define MCA_UNIT_DCU0         (MCA_UNIT_TYPE_DCU)
#define MCA_UNIT_DTLB0        (MCA_UNIT_TYPE_DTLB)
#define MCA_UNIT_MLC0         (MCA_UNIT_TYPE_MLC)

#define MCA_UNIT_PCU0         (MCA_UNIT_TYPE_PCU)
#define MCA_UNIT_KTI0         (MCA_UNIT_TYPE_KTI)
#define MCA_UNIT_KTI1         (MCA_UNIT_TYPE_KTI)
#define MCA_UNIT_KTI2         (MCA_UNIT_TYPE_KTI)
#define MCA_UNIT_CHAM0        (MCA_UNIT_TYPE_CHA)
#define MCA_UNIT_CHAM1        (MCA_UNIT_TYPE_CHA)
#define MCA_UNIT_CHAM2        (MCA_UNIT_TYPE_CHA)

#define MCA_UNIT_IIO0         (MCA_UNIT_TYPE_IIO)
#define MCA_UNIT_M2M0         (MCA_UNIT_TYPE_M2MEM)
#define MCA_UNIT_M2M1         (MCA_UNIT_TYPE_M2MEM)
#define MCA_UNIT_IMC00        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC01        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC10        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC11        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC02        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC12        (MCA_UNIT_TYPE_IMC)

// Includes
//

#define IA32_MCG_STATUS 0x17A

//
// Defines
//
#define MCI_REG_BASE    0x400

#define IFU_CTL      (MCI_REG_BASE + 0x00)
#define DCU_CTL      (MCI_REG_BASE + 0x04)
#define DTLB_CTL     (MCI_REG_BASE + 0x08)
#define MLC_CTL      (MCI_REG_BASE + 0x0C)
#define PCU_CTL      (MCI_REG_BASE + 0x10)
#define QPI0_CTL     (MCI_REG_BASE + 0x14)
#define IIO_CTL      (MCI_REG_BASE + 0x18)
#define HA0_CTL      (MCI_REG_BASE + 0x1C)
#define HA1_CTL      (MCI_REG_BASE + 0x20)
#define IMC0_CTL     (MCI_REG_BASE + 0x24)
#define IMC1_CTL     (MCI_REG_BASE + 0x28)
#define IMC2_CTL     (MCI_REG_BASE + 0x2C)
#define IMC3_CTL     (MCI_REG_BASE + 0x30)
#define IMC4_CTL     (MCI_REG_BASE + 0x34)
#define IMC5_CTL     (MCI_REG_BASE + 0x38)
#define IMC6_CTL     (MCI_REG_BASE + 0x3C)
#define IMC7_CTL     (MCI_REG_BASE + 0x40)
#define CBO00_CTL    (MCI_REG_BASE + 0x44)
#define CBO01_CTL    (MCI_REG_BASE + 0x48)
#define CBO02_CTL    (MCI_REG_BASE + 0x4C)
#define CBO03_CTL    (MCI_REG_BASE + 0x50)
#define CBO04_CTL    (MCI_REG_BASE + 0x54)
#define CBO05_CTL    (MCI_REG_BASE + 0x58)
#define CBO06_CTL    (MCI_REG_BASE + 0x5C)
#define CBO07_CTL    (MCI_REG_BASE + 0x60)
#define CBO08_CTL    (MCI_REG_BASE + 0x64)
#define CBO09_CTL    (MCI_REG_BASE + 0x68)
#define CBO10_CTL    (MCI_REG_BASE + 0x6C)
#define CBO11_CTL    (MCI_REG_BASE + 0x70)
#define CBO12_CTL    (MCI_REG_BASE + 0x74)
#define CBO13_CTL    (MCI_REG_BASE + 0x78)
#define CBO14_CTL    (MCI_REG_BASE + 0x7C)

#define MSR_MC00_CTL IFU_CTL
#define MSR_MC01_CTL DCU_CTL
#define MSR_MC02_CTL DTLB_CTL
#define MSR_MC03_CTL MLC_CTL
#define MSR_MC04_CTL PCU_CTL
#define MSR_MC05_CTL QPI0_CTL
#define MSR_MC06_CTL IIO_CTL
#define MSR_MC07_CTL HA0_CTL
#define MSR_MC08_CTL HA1_CTL
#define MSR_MC09_CTL IMC0_CTL
#define MSR_MC10_CTL IMC1_CTL
#define MSR_MC11_CTL IMC2_CTL
#define MSR_MC12_CTL IMC3_CTL
#define MSR_MC13_CTL IMC4_CTL
#define MSR_MC14_CTL IMC5_CTL
#define MSR_MC15_CTL IMC6_CTL
#define MSR_MC16_CTL IMC7_CTL
#define MSR_MC17_CTL CBO00_CTL
#define MSR_MC18_CTL CBO01_CTL
#define MSR_MC19_CTL CBO02_CTL
#define MSR_MC20_CTL CBO03_CTL
#define MSR_MC21_CTL CBO04_CTL
#define MSR_MC22_CTL CBO05_CTL
#define MSR_MC23_CTL CBO06_CTL
#define MSR_MC24_CTL CBO07_CTL
#define MSR_MC25_CTL CBO08_CTL
#define MSR_MC26_CTL CBO09_CTL
#define MSR_MC27_CTL CBO10_CTL
#define MSR_MC28_CTL CBO11_CTL
#define MSR_MC29_CTL CBO12_CTL
#define MSR_MC30_CTL CBO13_CTL
#define MSR_MC31_CTL CBO14_CTL

#define MSR_MC00_STATUS (MSR_MC00_CTL + 0x1)
#define MSR_MC00_ADDR   (MSR_MC00_CTL + 0x2)
#define MSR_MC00_MISC   (MSR_MC00_CTL + 0x3)
#define MSR_MC01_STATUS (MSR_MC01_CTL + 0x1)
#define MSR_MC01_ADDR   (MSR_MC01_CTL + 0x2)
#define MSR_MC01_MISC   (MSR_MC01_CTL + 0x3)
#define MSR_MC02_STATUS (MSR_MC02_CTL + 0x1)
#define MSR_MC02_ADDR   (MSR_MC02_CTL + 0x2)
#define MSR_MC02_MISC   (MSR_MC02_CTL + 0x3)
#define MSR_MC03_STATUS (MSR_MC03_CTL + 0x1)
#define MSR_MC03_ADDR   (MSR_MC03_CTL + 0x2)
#define MSR_MC03_MISC   (MSR_MC03_CTL + 0x3)
#define MSR_MC04_STATUS (MSR_MC04_CTL + 0x1)
#define MSR_MC04_ADDR   (MSR_MC04_CTL + 0x2)
#define MSR_MC04_MISC   (MSR_MC04_CTL + 0x3)
#define MSR_MC05_STATUS (MSR_MC05_CTL + 0x1)
#define MSR_MC05_ADDR   (MSR_MC05_CTL + 0x2)
#define MSR_MC05_MISC   (MSR_MC05_CTL + 0x3)
#define MSR_MC06_STATUS (MSR_MC06_CTL + 0x1)
#define MSR_MC06_ADDR   (MSR_MC06_CTL + 0x2)
#define MSR_MC06_MISC   (MSR_MC06_CTL + 0x3)
#define MSR_MC07_STATUS (MSR_MC07_CTL + 0x1)
#define MSR_MC07_ADDR   (MSR_MC07_CTL + 0x2)
#define MSR_MC07_MISC   (MSR_MC07_CTL + 0x3)
#define MSR_MC08_STATUS (MSR_MC08_CTL + 0x1)
#define MSR_MC08_ADDR   (MSR_MC08_CTL + 0x2)
#define MSR_MC08_MISC   (MSR_MC08_CTL + 0x3)
#define MSR_MC09_STATUS (MSR_MC09_CTL + 0x1)
#define MSR_MC09_ADDR   (MSR_MC09_CTL + 0x2)
#define MSR_MC09_MISC   (MSR_MC09_CTL + 0x3)
#define MSR_MC10_STATUS (MSR_MC10_CTL + 0x1)
#define MSR_MC10_ADDR   (MSR_MC10_CTL + 0x2)
#define MSR_MC10_MISC   (MSR_MC10_CTL + 0x3)
#define MSR_MC11_STATUS (MSR_MC11_CTL + 0x1)
#define MSR_MC11_ADDR   (MSR_MC11_CTL + 0x2)
#define MSR_MC11_MISC   (MSR_MC11_CTL + 0x3)
#define MSR_MC12_STATUS (MSR_MC12_CTL + 0x1)
#define MSR_MC12_ADDR   (MSR_MC12_CTL + 0x2)
#define MSR_MC12_MISC   (MSR_MC12_CTL + 0x3)
#define MSR_MC13_STATUS (MSR_MC13_CTL + 0x1)
#define MSR_MC13_ADDR   (MSR_MC13_CTL + 0x2)
#define MSR_MC13_MISC   (MSR_MC13_CTL + 0x3)
#define MSR_MC14_STATUS (MSR_MC14_CTL + 0x1)
#define MSR_MC14_ADDR   (MSR_MC14_CTL + 0x2)
#define MSR_MC14_MISC   (MSR_MC14_CTL + 0x3)
#define MSR_MC15_STATUS (MSR_MC15_CTL + 0x1)
#define MSR_MC15_ADDR   (MSR_MC15_CTL + 0x2)
#define MSR_MC15_MISC   (MSR_MC15_CTL + 0x3)
#define MSR_MC16_STATUS (MSR_MC16_CTL + 0x1)
#define MSR_MC16_ADDR   (MSR_MC16_CTL + 0x2)
#define MSR_MC16_MISC   (MSR_MC16_CTL + 0x3)
#define MSR_MC17_STATUS (MSR_MC17_CTL + 0x1)
#define MSR_MC17_ADDR   (MSR_MC17_CTL + 0x2)
#define MSR_MC17_MISC   (MSR_MC17_CTL + 0x3)
#define MSR_MC18_STATUS (MSR_MC18_CTL + 0x1)
#define MSR_MC18_ADDR   (MSR_MC18_CTL + 0x2)
#define MSR_MC18_MISC   (MSR_MC18_CTL + 0x3)
#define MSR_MC19_STATUS (MSR_MC19_CTL + 0x1)
#define MSR_MC19_ADDR   (MSR_MC19_CTL + 0x2)
#define MSR_MC19_MISC   (MSR_MC19_CTL + 0x3)
#define MSR_MC20_STATUS (MSR_MC20_CTL + 0x1)
#define MSR_MC20_ADDR   (MSR_MC20_CTL + 0x2)
#define MSR_MC20_MISC   (MSR_MC20_CTL + 0x3)
#define MSR_MC21_STATUS (MSR_MC21_CTL + 0x1)
#define MSR_MC21_ADDR   (MSR_MC21_CTL + 0x2)
#define MSR_MC21_MISC   (MSR_MC21_CTL + 0x3)
#define MSR_MC22_STATUS (MSR_MC22_CTL + 0x1)
#define MSR_MC22_ADDR   (MSR_MC22_CTL + 0x2)
#define MSR_MC22_MISC   (MSR_MC22_CTL + 0x3)
#define MSR_MC23_STATUS (MSR_MC23_CTL + 0x1)
#define MSR_MC23_ADDR   (MSR_MC23_CTL + 0x2)
#define MSR_MC23_MISC   (MSR_MC23_CTL + 0x3)
#define MSR_MC24_STATUS (MSR_MC24_CTL + 0x1)
#define MSR_MC24_ADDR   (MSR_MC24_CTL + 0x2)
#define MSR_MC24_MISC   (MSR_MC24_CTL + 0x3)
#define MSR_MC25_STATUS (MSR_MC25_CTL + 0x1)
#define MSR_MC25_ADDR   (MSR_MC25_CTL + 0x2)
#define MSR_MC25_MISC   (MSR_MC25_CTL + 0x3)
#define MSR_MC26_STATUS (MSR_MC26_CTL + 0x1)
#define MSR_MC26_ADDR   (MSR_MC26_CTL + 0x2)
#define MSR_MC26_MISC   (MSR_MC26_CTL + 0x3)
#define MSR_MC27_STATUS (MSR_MC27_CTL + 0x1)
#define MSR_MC27_ADDR   (MSR_MC27_CTL + 0x2)
#define MSR_MC27_MISC   (MSR_MC27_CTL + 0x3)
#define MSR_MC28_STATUS (MSR_MC28_CTL + 0x1)
#define MSR_MC28_ADDR   (MSR_MC28_CTL + 0x2)
#define MSR_MC28_MISC   (MSR_MC28_CTL + 0x3)
#define MSR_MC29_STATUS (MSR_MC29_CTL + 0x1)
#define MSR_MC29_ADDR   (MSR_MC29_CTL + 0x2)
#define MSR_MC29_MISC   (MSR_MC29_CTL + 0x3)
#define MSR_MC30_STATUS (MSR_MC30_CTL + 0x1)
#define MSR_MC30_ADDR   (MSR_MC30_CTL + 0x2)
#define MSR_MC30_MISC   (MSR_MC30_CTL + 0x3)
#define MSR_MC31_STATUS (MSR_MC31_CTL + 0x1)
#define MSR_MC31_ADDR   (MSR_MC31_CTL + 0x2)
#define MSR_MC31_MISC   (MSR_MC31_CTL + 0x3)

#define MSR_MC00_CTL2   (0x280)
#define MSR_MC01_CTL2   (MSR_MC00_CTL2 + 0x1)
#define MSR_MC02_CTL2   (MSR_MC01_CTL2 + 0x1)
#define MSR_MC03_CTL2   (MSR_MC02_CTL2 + 0x1)
#define MSR_MC04_CTL2   (MSR_MC03_CTL2 + 0x1)
#define MSR_MC05_CTL2   (MSR_MC04_CTL2 + 0x1)
#define MSR_MC06_CTL2   (MSR_MC05_CTL2 + 0x1)
#define MSR_MC07_CTL2   (MSR_MC06_CTL2 + 0x1)
#define MSR_MC08_CTL2   (MSR_MC07_CTL2 + 0x1)
#define MSR_MC09_CTL2   (MSR_MC08_CTL2 + 0x1)
#define MSR_MC10_CTL2   (MSR_MC09_CTL2 + 0x1)
#define MSR_MC11_CTL2   (MSR_MC10_CTL2 + 0x1)
#define MSR_MC12_CTL2   (MSR_MC11_CTL2 + 0x1)
#define MSR_MC13_CTL2   (MSR_MC12_CTL2 + 0x1)
#define MSR_MC14_CTL2   (MSR_MC13_CTL2 + 0x1)
#define MSR_MC15_CTL2   (MSR_MC14_CTL2 + 0x1)
#define MSR_MC16_CTL2   (MSR_MC15_CTL2 + 0x1)
#define MSR_MC17_CTL2   (MSR_MC16_CTL2 + 0x1)
#define MSR_MC18_CTL2   (MSR_MC17_CTL2 + 0x1)
#define MSR_MC19_CTL2   (MSR_MC18_CTL2 + 0x1)
#define MSR_MC20_CTL2   (MSR_MC19_CTL2 + 0x1)
#define MSR_MC21_CTL2   (MSR_MC20_CTL2 + 0x1)
#define MSR_MC22_CTL2   (MSR_MC21_CTL2 + 0x1)
#define MSR_MC23_CTL2   (MSR_MC22_CTL2 + 0x1)
#define MSR_MC24_CTL2   (MSR_MC23_CTL2 + 0x1)
#define MSR_MC25_CTL2   (MSR_MC24_CTL2 + 0x1)
#define MSR_MC26_CTL2   (MSR_MC25_CTL2 + 0x1)
#define MSR_MC27_CTL2   (MSR_MC26_CTL2 + 0x1)
#define MSR_MC28_CTL2   (MSR_MC27_CTL2 + 0x1)
#define MSR_MC29_CTL2   (MSR_MC28_CTL2 + 0x1)
#define MSR_MC30_CTL2   (MSR_MC29_CTL2 + 0x1)
#define MSR_MC31_CTL2   (MSR_MC30_CTL2 + 0x1)

#define MSR_MCG_CONTAIN			  0x178
#define MSR_SMM_MCA_CAP           0x17D
#define MSR_CORE_SMI_ERR_SRC      0x17c
#define MSR_UNCORE_SMI_ERR_SRC    0x17e

#define MCA_ADDR_BIT_MASK (0x3fffffffffff) //46 bits
#define MCA_MISC_LSB_MASK (0x3f)

#define N_MSR_MCG_COUNT_START                       0
#define N_MSR_MCG_COUNT_STOP                        7
#define MCA_FIRMWARE_UPDATED_BIT                    BIT37

#define IMC_PATROL_SCRUB_ERR_MCACOD                 0xC0
#define IMC_PATROL_SCRUB_ERR_MCACOD_MSK             0xFFF0
#define IMC_UNC_PATROL_SCRUB_ERR_MSCOD              0x10
#define IMC_COR_PATROL_SCRUB_ERR_MSCOD              0x08
#define IMC_PATROL_SCRUB_ERR_MSCOD_MSK              0xFFFF

#define IMC_SPARING_ERR_MCACOD                      0xC0
#define IMC_SPARING_ERR_MCACOD_MSK                  0xFFF0
#define IMC_UNC_SPARING_ERR_MSCOD                   0x40
#define IMC_COR_SPARING_ERR_MSCOD                   0x20
#define IMC_SPARING_ERR_MSCOD_MSK                   0xFFFF

//
// Sparing interrupt select
//
#define SPARE_INT_NONE          0
#define SPARE_INT_SMI           1
#define SPARE_INT_PIN           2
#define SPARE_INT_CMCI          4

#define IMC_MCACOD_CHANNEL_MSK             0x000F


#define MCA_PASCRUB_MCACOD 0x80
#define MCA_PASCRUB_MCACOD_MSK 0xfff0
#define MCA_PASCRUB_MSCOD 0x10
#define MCA_PASCRUB_MSCOD_MSK 0xffff

// These may go away when entire RAS code is ported from Node/Ch to Socket/Ch
#define SKTCH_TO_NODE(Skt, Ch)      ((Skt * MAX_IMC) + (Ch % MAX_MC_CH))
#define SKTCH_TO_NODECH(Ch)         (Ch % MAX_MC_CH)

//
//===============================================================
// Uncore definitions (Should be public include from Socket pkg)
//
#ifndef	MAX_SKX_M2PCIE
#define  MAX_SKX_M2PCIE      5
#endif


typedef struct {
  EFI_MEM_RAS_PROTOCOL   *MemRas;
  EFI_SMM_BASE2_PROTOCOL *SmmBase;
  EFI_SMM_SYSTEM_TABLE2  *Smst;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;
  IIO_UDS *IioUds;
} EMCA_PHLIB_PRIV;

typedef struct {
  UINT32 McBankNum;
  UINT8 Instance;
} M2MEM_LOOKUP;

typedef
EFI_STATUS (*MEM_RAS_EVENT_HNDLR)(
    IN UINT8        NodeId,
    IN EVENT_TYPE   EventType,
    IN OUT BOOLEAN  *IsEventHandled
 );

typedef volatile UINTN              SPIN_LOCK;

//
// Functions prototype declarations
//

// >> need to go to emcalib


// << need to go to emcalib

// >>
// >> below functions should go to emcaplatformhookslib.c ???
// >>

// <<
// << above functions should go to emcaplatformhookslib.c ???
// <<


// >>>
// >>> below functions need to go to common processor error handler files
// >>>

VOID
LogCorrectedMemError (
  IN        UINT8               Type,
  IN        UINT8               SubType,
  IN        UINT8               Node,
  IN        UINT8               Ch,
  IN        UINT8               Rank,
  IN        UINT8               Dimm,
  IN        UINT8               McaCode
  );

/**
  Checks to see if pending processor errors exist. 

  @param[out] UmcSockets            Sockets with UMC pending
  @param[out] ImcSockets            Sockets with IMC
  @param[out] ImcNodesEventProgress Bitmap of nodes with events in progress
  @param[out] ImcNodesNewEvents     Bitmap of nodes with new errors

  @retval Status.

**/
BOOLEAN
PendingProcessorErrorExists (
      OUT   UINT32            *UmcSockets,
      OUT   UINT32            *ImcSockets,
      OUT   UINT32            *ImcNodesEventProgress,
      OUT   UINT32            *ImcNodesNewEvents
  );


// <<<
// <<< above functions need to go to common processor error handler files
// <<<

//
// Interfaces between Silicon and Common Module
//

VOID
FirstCheckToHalt(
  VOID
  );

EFI_STATUS
ProcMemEnableReporting (
  VOID
  );

EFI_STATUS
ProcMemDisableReporting (
  IN UINT32 DeviceType,
  IN UINT32 EventType,
  IN UINT32 *ExtData
  );
        //VOID DisableSMIForCorrectedMemErrorThreshold (IN UINT8 Socket, IN UINT8 Mc);

BOOLEAN
ProcMemCheckStatus (
    VOID
    );

EFI_STATUS
ProcMemDetectAndHandle (
  UINT32    *CurrentSev,
  ClearOption      Clear     
  );

VOID
MirrorFailoverHandler (
  IN        UINT8   Socket,
  IN        UINT8   Mc
  );

BOOLEAN
GetErrInfo (
  IN        UINT8   Socket,
  IN        UINT8   *DdrCh,
  IN        UINT8   *Dimm,
  IN        UINT8   *Rank
  );

static
EFI_STATUS
HandleAdddcSparing (
  IN        UINT8       NodeId,
  IN        EVENT_TYPE  EventType,
  IN  OUT   BOOLEAN     *IsEventHandled
  );

static
EFI_STATUS
HandleRankSparing (
  IN        UINT8       NodeId,
  IN        EVENT_TYPE  EventType,
  IN  OUT   BOOLEAN     *IsEventHandled
  );

static
EFI_STATUS
HandleSddcPlusOneSparing (
  IN        UINT8       NodeId,
  IN        EVENT_TYPE  EventType,
  IN  OUT   BOOLEAN     *IsEventHandled
  );

EFI_STATUS ProcessKtiFatalAndViralError (VOID );
BOOLEAN SocketInKtiViralState (VOID);

EFI_STATUS
ProcMemClearEvent (
  VOID
  );


#endif /* PROCMEMERRREPORTING_H_ */
