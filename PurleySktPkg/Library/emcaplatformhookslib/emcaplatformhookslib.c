/** @file
  Implementation of EMCA Platform hooks


  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

  Copyright (c) 2009-2016 Intel Corporation.  All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license.  Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


**/


#include <Library/emcaplatformhookslib.h>
#include <Library/mpsyncdatalib.h>
#include "emcaplatformhookslibprivate.h"

STATIC EMCA_MCBANK_DESC *gMcBankList = NULL;

STATIC EMCA_MCBANK_DESC gMcBankListSkxEp[] = { 
  { MSR_MC00_CTL, MSR_MC00_CTL2, MC_SCOPE_CORE| EMCA_LOG_SUPPORT , EMCA_SECTYPE_PROCIA32,    MCA_UNIT_IFU0       , ReadMcBankSignature, {NULL}},
  { MSR_MC01_CTL, MSR_MC01_CTL2, MC_SCOPE_CORE| EMCA_LOG_SUPPORT , EMCA_SECTYPE_PROCIA32,    MCA_UNIT_DCU0       , ReadMcBankSignature, {NULL}},
  { MSR_MC02_CTL, MSR_MC02_CTL2, MC_SCOPE_CORE| EMCA_LOG_SUPPORT , EMCA_SECTYPE_PROCIA32,    MCA_UNIT_DTLB0      , ReadMcBankSignature, {NULL}},
  { MSR_MC03_CTL, MSR_MC03_CTL2, MC_SCOPE_CORE| EMCA_LOG_SUPPORT , EMCA_SECTYPE_PROCIA32,    MCA_UNIT_MLC0       , ReadMcBankSignature, {NULL}},
  { MSR_MC04_CTL, MSR_MC04_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_PCU0       , ReadMcBankSignature, {NULL}},
  { MSR_MC05_CTL, MSR_MC05_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_KTI0       , ReadMcBankSignature, {NULL}},
  { MSR_MC06_CTL, MSR_MC06_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_IIO,       MCA_UNIT_IIO0       , ReadMcBankSignature, {NULL}},
  { MSR_MC07_CTL, MSR_MC07_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_M2M0       , ReadM2MemMcBankSignature, {NULL}},
  { MSR_MC08_CTL, MSR_MC08_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_M2M1       , ReadM2MemMcBankSignature, {NULL}},
  { MSR_MC09_CTL, MSR_MC09_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_CHAM0      , ReadMcBankSignature, {NULL}},
  { MSR_MC10_CTL, MSR_MC10_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_CHAM1      , ReadMcBankSignature, {NULL}},
  { MSR_MC11_CTL, MSR_MC11_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_CHAM2      , ReadMcBankSignature, {NULL}},
  { MSR_MC12_CTL, MSR_MC12_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_KTI1       , ReadMcBankSignature, {NULL}},
  { MSR_MC13_CTL, MSR_MC13_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC00      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC14_CTL, MSR_MC14_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC01      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC15_CTL, MSR_MC15_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC10      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC16_CTL, MSR_MC16_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC11      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC17_CTL, MSR_MC17_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC02      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC18_CTL, MSR_MC18_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC12      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC19_CTL, MSR_MC19_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_KTI2       , ReadMcBankSignature, {NULL}}
};

STATIC EMCA_MCBANK_DESC gMcBankListSkxEx[] = { 
  { MSR_MC00_CTL, MSR_MC00_CTL2, MC_SCOPE_CORE| EMCA_LOG_SUPPORT , EMCA_SECTYPE_PROCIA32,    MCA_UNIT_IFU0       , ReadMcBankSignature, {NULL}},
  { MSR_MC01_CTL, MSR_MC01_CTL2, MC_SCOPE_CORE| EMCA_LOG_SUPPORT , EMCA_SECTYPE_PROCIA32,    MCA_UNIT_DCU0       , ReadMcBankSignature, {NULL}},
  { MSR_MC02_CTL, MSR_MC02_CTL2, MC_SCOPE_CORE| EMCA_LOG_SUPPORT , EMCA_SECTYPE_PROCIA32,    MCA_UNIT_DTLB0      , ReadMcBankSignature, {NULL}},
  { MSR_MC03_CTL, MSR_MC03_CTL2, MC_SCOPE_CORE| EMCA_LOG_SUPPORT , EMCA_SECTYPE_PROCIA32,    MCA_UNIT_MLC0       , ReadMcBankSignature, {NULL}},
  { MSR_MC04_CTL, MSR_MC04_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_PCU0       , ReadMcBankSignature, {NULL}},
  { MSR_MC05_CTL, MSR_MC05_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_KTI0       , ReadMcBankSignature, {NULL}},
  { MSR_MC06_CTL, MSR_MC06_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_IIO,       MCA_UNIT_IIO0       , ReadMcBankSignature, {NULL}},
  { MSR_MC07_CTL, MSR_MC07_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_M2M0       , ReadM2MemMcBankSignature, {NULL}},
  { MSR_MC08_CTL, MSR_MC08_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_M2M1       , ReadM2MemMcBankSignature, {NULL}},
  { MSR_MC09_CTL, MSR_MC09_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_CHAM0      , ReadMcBankSignature, {NULL}},
  { MSR_MC10_CTL, MSR_MC10_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_CHAM1      , ReadMcBankSignature, {NULL}},
  { MSR_MC11_CTL, MSR_MC11_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_CHAM2      , ReadMcBankSignature, {NULL}},
  { MSR_MC12_CTL, MSR_MC12_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_KTI1       , ReadMcBankSignature, {NULL}},
  { MSR_MC13_CTL, MSR_MC13_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC00      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC14_CTL, MSR_MC14_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC01      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC15_CTL, MSR_MC15_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC10      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC16_CTL, MSR_MC16_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC11      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC17_CTL, MSR_MC17_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC02      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC18_CTL, MSR_MC18_CTL2, MC_SCOPE_PKG | EMCA_LOG_SUPPORT , EMCA_SECTYPE_PLATMEM,     MCA_UNIT_IMC12      , ReadMcBankSignature, {McaiMcCsrInfoHook}},
  { MSR_MC19_CTL, MSR_MC19_CTL2, MC_SCOPE_PKG                    , EMCA_SECTYPE_PROCGEN,     MCA_UNIT_KTI2       , ReadMcBankSignature, {NULL}}
};


STATIC M2MEM_LOOKUP *mM2MemLookup = NULL;
IMC_NODE_LOOKUP *mImcNodeIdLookup = NULL;

UINTN mImcNodeIdLookupEntries = 0;
IMC_NODE_LOOKUP mImcNodeIdLookupSkx[] = {
//McbankNum, Memoryid
  { 13, 0 },
  { 14, 0 },
  { 15, 1 },
  { 16, 1 },
  { 17, 0 },
  { 18, 1 }
};

STATIC M2MEM_LOOKUP mM2MemLookup2[] = {
//McbankNum, Instance 
  { 7, 0 },
  { 8, 1 }
};

STATIC EMCA_MCRECOVERY_DESC gMcaRecoverables[] = {
  // MCACOD , MCACODMASK, MSCOD, MSCODMASK,  Handler
  { MCA_PASCRUB_MCACOD, MCA_PASCRUB_MCACOD_MSK, MCA_PASCRUB_MSCOD, MCA_PASCRUB_MSCOD_MSK, RecoveryPatrolScrubDummy },
};

STATIC UINTN gMcaRecorevablesEntries = sizeof(gMcaRecoverables)/sizeof(EMCA_MCRECOVERY_DESC);

STATIC UINT32 mM2MemLookupEntries = 0;


STATIC EMCA_PHLIB_PRIV mEmcaPh = { 0 }; 

STATIC UINT64 mMcgCap = 0;
STATIC UINT64 mL1Address = 0;

/**

    Initialize the pointer to the gMcBankList with the correct table for processor family and skew 

    @param None

    @retval EFI_SUCCESS - Init was successful 

**/
EFI_STATUS
InitMcBankList(
VOID
)
{ UINT8 CpuType = 0;
  UINT8 SystemRasType = 0;
  UINT8 physical_chop = 0;
  UINT32 capid4;
  UINT8 Stepping;
  capid4 = mEmcaPh.CpuCsrAccess->ReadCpuCsr(0, 0, CAPID4_PCU_FUN3_REG);

  CpuType =  mEmcaPh.IioUds->SystemStatus.cpuType;
  Stepping = mEmcaPh.IioUds->SystemStatus.MinimumCpuStepping;
  SystemRasType= mEmcaPh.IioUds->SystemStatus.SystemRasType;
  physical_chop = (UINT8) ((capid4 >> 6) & 0x3); // physical_chop CAPID [7:6]
  
  switch(CpuType) {
    case CPU_SKX:
      switch(SystemRasType) {
      case ADVANCED_RAS:
      case FPGA_RAS:
          gMcBankList = &gMcBankListSkxEx[0];
          mM2MemLookup = &mM2MemLookup2[0];
          mM2MemLookupEntries = sizeof(mM2MemLookup2)/sizeof(M2MEM_LOOKUP);         
          mImcNodeIdLookup = &mImcNodeIdLookupSkx[0];
          mImcNodeIdLookupEntries = sizeof(mImcNodeIdLookupSkx)/sizeof(IMC_NODE_LOOKUP);
          break;          
        case STANDARD_RAS:
        case CORE_RAS:
          mM2MemLookup = &mM2MemLookup2[0];
          mM2MemLookupEntries = sizeof(mM2MemLookup2)/sizeof(M2MEM_LOOKUP); 
          mImcNodeIdLookup = &mImcNodeIdLookupSkx[0];
          mImcNodeIdLookupEntries = sizeof(mImcNodeIdLookupSkx)/sizeof(IMC_NODE_LOOKUP);
          switch(physical_chop) {
            case SKX_PHY_CHOP_MCC:
              gMcBankList = &gMcBankListSkxEp[0];
              break;
            case SKX_PHY_CHOP_HCC:
              gMcBankList = &gMcBankListSkxEp[0];
              break;
            default:
              gMcBankList = &gMcBankListSkxEp[0];
              break;
          }
          break;
      }
      break;
  }
  return EFI_SUCCESS;
}
  
                           

/**

    Allocates protocols and initialize internal structure required by the library.  

    @param ImageHandle - refers to the image handle of the driver.
    @param SystemTable - points to the EFI System Table.

    @retval EFI_SUCCESS - Library constructor executed correctly 

**/
EFI_STATUS
EFIAPI
EmcaPlatformHooksLibConstructor (
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS Status;
  BOOLEAN InSmm;
  EFI_IIO_UDS_PROTOCOL *iioUdsProtocol;

  if(mEmcaPh.SmmBase == NULL) {
    Status = gBS->LocateProtocol(
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &mEmcaPh.SmmBase
                  );
    ASSERT_EFI_ERROR(Status);
  }
  
  mEmcaPh.SmmBase->InSmm(mEmcaPh.SmmBase, &InSmm);

  if(mEmcaPh.IioUds == NULL) {
    //
    // Locate the IIO Protocol Interface
    //
    Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid,NULL, &iioUdsProtocol);
    ASSERT_EFI_ERROR (Status);
    mEmcaPh.IioUds = iioUdsProtocol->IioUdsPtr;
  }

  if(InSmm) {

    if(mEmcaPh.Smst == NULL) {
      Status = mEmcaPh.SmmBase->GetSmstLocation(
                                  mEmcaPh.SmmBase,
                                  &mEmcaPh.Smst
                                );
      ASSERT_EFI_ERROR(Status);
    }

    if(mEmcaPh.CpuCsrAccess == NULL) {
      Status = mEmcaPh.Smst->SmmLocateProtocol(
                               &gEfiCpuCsrAccessGuid, 
                               NULL, 
                               &mEmcaPh.CpuCsrAccess
                             );
      ASSERT_EFI_ERROR (Status);
    }
    
    if(gMcBankList == NULL) {
      InitMcBankList();
    }    
  }
  
  return EFI_SUCCESS;
}

/**
  Get the number of MCBANKS_PER_LP.
  
  MCBANKS_PER_LP = MCBANK_CORE + MCBANKS_UNCORE = MCG_CAP.Count
  Assumtion of implementation is that all sockets are symetric, they all have the same number of MCBanks Per LP
  
  @return MCBANKS_PER_LP

**/
UINT64
GetNumMcBanksPerLp(
  VOID
  )

{
  UINT32 McgCapAddr = 0;
  UINT64 McBanksNum = 0;
  
  McgCapAddr = EFI_IA32_MCG_CAP;
  McBanksNum = AsmReadMsr64(McgCapAddr);
  McBanksNum = BitFieldRead64(McBanksNum,N_MSR_MCG_COUNT_START, N_MSR_MCG_COUNT_STOP);
  return McBanksNum;
}

/**
  Get the number of L1 Dir Entries required based on the MC Banks available and number of sockets.

  Number of L1 Dir Entries = SOCKET_COUNT * ( 2 ** APICID_BITS_PER_SOCKET) * MCBANKS_PER_LP where: 
  
  SOCKET_COUNT is  the maximum number  of  processor  sockets in  the platform,
  APICID_BITS_PER_SOCKET is the number of APIC ID bits used up by each socket  
  MCBANKS_PER_LP = MCBANK_CORE + MCBANKS_UNCORE = MCG_CAP.Count
  
  @return number of L1 Dir Entries  

**/
UINT64
GetNumL1DirEntries(
  VOID
  )
{
  UINT64 Num = 0;
  UINT64 ApicIdBitsPerSkt = 0;
  UINTN  ThreadBits = 0;
  UINTN  CoreBits = 0;

  GetCoreAndSMTBits(&ThreadBits,&CoreBits);
  ApicIdBitsPerSkt = ThreadBits + CoreBits;
  
  Num = MAX_SOCKET * LShiftU64(1 , ApicIdBitsPerSkt) * GetNumMcBanksPerLp();
  return Num;
}

/**
  Get the number of Elog Dir(L2) Entries required based on the MC Banks in the system.

 NumElogentries = number of MC BANKS in the system
                      = NumSKTSupported * NumMCBanksPerSKT
                      = NumSKTSupported * (NumCoreMcBanks *  CORES_PER_SOCKET + NumUncoreMcBanks ) 
  
  @return Number of Elog Entries 

**/
UINT64 
GetNumElogEntries(
  VOID
  )
{
  UINT32 i;
  UINT64 McBanksNum;
  UINT64 PhyMcBankNum;
  UINT16  McScope;
  UINTN Corebits;
  UINTN Threadbits;
  
  McBanksNum = GetNumMcBanksPerLp();
  GetCoreAndSMTBits(&Threadbits,&Corebits);

  for (i = 0, PhyMcBankNum = 0; i < McBanksNum; i++) {
    if(isEMCALogSupported(i)) {
      McScope = GetMcBankScope(i);
      switch(McScope) {
        
        case MC_SCOPE_TD:
          PhyMcBankNum += (LShiftU64(1 , Threadbits)) * (LShiftU64(1 , Corebits)) * (MAX_SOCKET);
          break;
        case MC_SCOPE_CORE:
          PhyMcBankNum += (LShiftU64(1 , Corebits)) * (MAX_SOCKET);
          break;
        case MC_SCOPE_PKG:
          PhyMcBankNum += (MAX_SOCKET);
          break;
        default:
          ASSERT(FALSE); //should not enter here
          break;
      }         
    }
  }

  return PhyMcBankNum;
}

/**

    Get if emca logging support has been implemented in BIOS for particular Mc Bank. 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval  TRUE - logging is implemented in bios for McBank. 
    @retval FALSE - logging is not implemented in bios for McBank.

**/
BOOLEAN
isEMCALogSupported(
  UINT32 McBankIdx
  )
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){
    return ( (gMcBankList[McBankIdx].Attrib & EMCA_LOG_SUPPORT) != 0 );
  }else{
    return FALSE;
  }
}

/**

    Return whether the bank unit is Memory

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval TRUE  - Bank Unit is memory
    @retval FALSE - Bank Unit is not memory

**/
BOOLEAN
isEmcaMemoryBank(
  UINT32 McBankIdx
)
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){
    if(((gMcBankList[McBankIdx].UnitType & MCA_UNIT_TYPE_M2MEM) != 0) || 
     ((gMcBankList[McBankIdx].UnitType & MCA_UNIT_TYPE_IMC) != 0)) {
    return TRUE;
    }    
  }else{
    return FALSE;
  }
  return FALSE;
}

/**

    Return whether the bank unit is Memory

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval UNIT TYPE  - Silicon Specific Bank Unit Type

**/
UINT32
GetMcBankUnitType(
  UINT32 McBankIdx
  )
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){
    return ( gMcBankList[McBankIdx].UnitType );
  }else{
    return 0;
  }

}

/**

    Get Scope for the Bank. The scope can be thread level, core level and package level.

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval  0x0  - Thread Scope 
    @retval  0x1  - Core Scope 
    @retval  0x2  - Package Scope 

**/
UINT16
GetMcBankScope(
  UINT32 McBankIdx
  )
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){
    return ( gMcBankList[McBankIdx].Attrib & MCSCOPE_MASK );
  }else{
    return 0;
  }
}

/**

    Get Bank Index for Mci_status for McBank

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval Mci_status Index

**/
UINT32
GetAddrMcBankSts
(
  UINT32 McBankIdx
)
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){  
    return gMcBankList[McBankIdx].CtlMsr + 0x1; 
  }else{
    return 0;
  }
}

/**

    Get Bank Index for Mci_Ctl for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval Mci_Ctl Index

**/
UINT32
  GetAddrMcBankCtl
(
  UINT32 McBankIdx
)
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){
    return gMcBankList[McBankIdx].CtlMsr;
  }else{
    return 0;
  }
}

/**

    Get Bank Index for Mci_addr for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    

    @retval Mci_addr Index

**/
UINT32
GetAddrMcBankAddr
(
  UINT32 McBankIdx
)
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){
    return gMcBankList[McBankIdx].CtlMsr +  0x2;
  }else{
    return 0;
  }
}

/**

    Get Bank Index for Mci_Misc for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval Mci_Misc Index

**/
UINT32
GetAddrMcBankMisc
(
  UINT32 McBankIdx
)
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){  
    return gMcBankList[McBankIdx].CtlMsr + 0x3;
  }else{
    return 0;
  }
}

/**

    Get Bank Index for Mci_Misc2 for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval Mci_Misc2 Index

**/
UINT32
GetAddrMcBankCtl2
(
  UINT32 McBankIdx
)
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){  
    return gMcBankList[McBankIdx].Ctl2Msr;
  }else{
    return 0;
  }
}

/**

    Get Section Type for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.

    @retval SecType - Return the section type related to the McBank Unit

**/
UINT16
GetMcBankSecType
(
  UINT32 McBankIdx
)
{
  ASSERT(gMcBankList != NULL);

  if(gMcBankList != NULL){  
    return gMcBankList[McBankIdx].SecType;
  }else{
    return 0;
  }
}

/**

    Agregate Information for iMC McBank using internal CSRs specific to silicon. 

    @param This      -Pointer to the hook
    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param Skt       - Socket Id 
    @param McSig     - A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
    @param Section   - A pointer to UEFI error section. 

    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.  
    @retval EFI_SUCCESS           - Function executed Correctly 

**/

EFI_STATUS
EFIAPI 
McaiMcCsrInfoHook
(
  IN MCA_BANK_HOOKS *This,
  IN UINT32 McBankIdx,
  IN UINT8 *Skt,
  IN EMCA_MC_SIGNATURE_ERR_RECORD *McSig,
  IN OUT VOID *Section 
) 
{
  UINTN i;
  UINT8 ChannelId;
  UINT8 DimmBitMap;
  UINT8 dimm;
  UINT16  Node;
  UINT8 SktCh;
  IA32_MCI_STATUS_MSR_STRUCT MsrIa32MCiStatus;
  PLATFORM_MEMORY_ERROR_DATA_UEFI_231 *MemSection;
  UINT16 MemoryControllerId;
  RETRY_RD_ERR_LOG_MCDDC_DP_STRUCT RetryRdErrLog;
  BOOLEAN CsrInfoErrors = FALSE;
  CORRERRORSTATUS_MCDDC_DP_STRUCT corrErrStsReg;
  EMCA_CSR_INFO_DESC iMcCsrInfoErrorLookup[] = {
    // MCACOD , MCACODMASK, MSCOD, MSCODMASK
    { IMC_SPARING_ERR_MCACOD, IMC_SPARING_ERR_MCACOD_MSK, IMC_UNC_SPARING_ERR_MSCOD, IMC_SPARING_ERR_MSCOD_MSK,PLATFORM_MEM_ERROR_SPARING},
    { IMC_SPARING_ERR_MCACOD, IMC_SPARING_ERR_MCACOD_MSK, IMC_COR_SPARING_ERR_MSCOD, IMC_SPARING_ERR_MSCOD_MSK,PLATFORM_MEM_ERROR_SPARING},
    { IMC_DDR4_CAP_ERR_MCACOD, IMC_DDR4_CAP_ERR_MCACOD_MSK, IMC_DDR4_CAP_ERR_MSCOD, IMC_DDR4_CAP_ERR_MSCOD_MSK,PLATFORM_MEM_ERROR_PARITY},
    { IMC_DDRT_CAP_ERR_MCACOD, IMC_DDRT_CAP_ERR_MCACOD_MSK, IMC_DDRT_CAP_ERR_MSCOD, IMC_DDRT_CAP_ERR_MSCOD_MSK,PLATFORM_MEM_ERROR_PARITY}
  };
  UINTN CsrInfoEntries = sizeof(iMcCsrInfoErrorLookup)/sizeof(EMCA_CSR_INFO_DESC);

  if(This == NULL || 
     Skt == NULL ||
     *Skt >= GetNumMcBanksPerLp() ||
     McSig == NULL ||
     Section == NULL
     ) {
    return EFI_INVALID_PARAMETER;
  }

  if( mImcNodeIdLookup == NULL ){
    return EFI_NOT_READY;
  }

  MemSection = (PLATFORM_MEMORY_ERROR_DATA_UEFI_231 *)Section;

  MsrIa32MCiStatus.Data = McSig->Signature.McSts;

  if(MsrIa32MCiStatus.Bits.val != 1) {
    return EFI_NOT_FOUND;
  }

  for(i=0; i < CsrInfoEntries; i++) {
    if( ((MsrIa32MCiStatus.Bits.mca_code & iMcCsrInfoErrorLookup[i].McaCodMsk) == iMcCsrInfoErrorLookup[i].McaCod) &&
          ((MsrIa32MCiStatus.Bits.ms_code & iMcCsrInfoErrorLookup[i].MsCodMsk) == iMcCsrInfoErrorLookup[i].MsCod)
        ) {
        CsrInfoErrors = TRUE;
        MemSection->ErrorType = iMcCsrInfoErrorLookup[i].ErrorType;
        MemSection->ValidFields |= PLATFORM_MEM_ERROR_TYPE_VALID;
    }
  }

  if(CsrInfoErrors == FALSE) {
    return EFI_NOT_FOUND;
  }

  for(i=0;i < mImcNodeIdLookupEntries; i++) {
    if(mImcNodeIdLookup[i].McBankNum == McBankIdx) {
      MemoryControllerId = mImcNodeIdLookup[i].MemoryControllerId;
      ChannelId = MsrIa32MCiStatus.Bits.mca_code & IMC_MCACOD_CHANNEL_MSK;
      Node = SKTMC_TO_NODE(*Skt, MemoryControllerId);
      SktCh = NODECH_TO_SKTCH(Node, ChannelId);

      if (MemSection->ErrorType == PLATFORM_MEM_ERROR_PARITY) {
        //
        // Command/Address Parity (CAP) error.
        //
        MemSection->Node = Node;
        MemSection->ValidFields |= PLATFORM_MEM_NODE_VALID;
        if (ChannelId != 0xF) {
          MemSection->Card = ChannelId;
          MemSection->ValidFields |= PLATFORM_MEM_CARD_VALID;
        }

        corrErrStsReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (*Skt, SktCh, CORRERRORSTATUS_MCDDC_DP_REG);
        DimmBitMap = (UINT8) corrErrStsReg.Bits.dimm_alert;
        if (DimmBitMap != 0) {
          dimm = 0;
          while ((DimmBitMap & 0x1) == 0 && dimm < MAX_DIMM) {
            DimmBitMap >>= 1;
            dimm++;
          }
          MemSection->ModuleRank = dimm;
          MemSection->ValidFields |= PLATFORM_MEM_MODULE_VALID;
        }

        RASDEBUG((DEBUG_INFO, "McaiMcCsrInfoHook (DDR4 Cmd/Adr Parity Error): Node = %d", MemSection->Node));
        if (MemSection->ValidFields & PLATFORM_MEM_CARD_VALID) {
          RASDEBUG((DEBUG_INFO, ", Card = %d", MemSection->Card));
        } else {
          RASDEBUG((DEBUG_INFO, ", Card = ?"));
        }
        if (MemSection->ValidFields & PLATFORM_MEM_MODULE_VALID) {
          RASDEBUG((DEBUG_INFO, ", ModuleRank = %d\n", MemSection->ModuleRank));
        } else {
          RASDEBUG((DEBUG_INFO, ", ModuleRank = ?\n"));
        }

      } else { // if (MemSection->ErrorType

        if(ChannelId != 0xf) {
          RetryRdErrLog.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(*Skt, SktCh, RETRY_RD_ERR_LOG_MCDDC_DP_REG);
          if(RetryRdErrLog.Bits.v == 1 && RetryRdErrLog.Bits.patspr == 1) {
            MemSection->Node = SKTMC_TO_NODE(*Skt, MemoryControllerId);
            MemSection->Card = ChannelId;
            // SKXTODO: Removed in 14ww31b CSR XML:
            // MemSection->RankNumber = (UINT16)RetryRdErrLog.Bits.rank;
            // DimmBitMap = (UINT8)RetryRdErrLog.Bits.dimm;
            dimm = 0;
            // while((DimmBitMap & 0x1) == 0 && dimm < MAX_DIMM) {
            //   DimmBitMap >>= 1;
            //   dimm++;
            // }
            MemSection->ModuleRank = dimm;
            MemSection->ValidFields |= PLATFORM_MEM_MODULE_VALID  |
                                       PLATFORM_RANK_NUMBER_VALID |
                                       PLATFORM_MEM_CARD_VALID    |
                                       PLATFORM_MEM_NODE_VALID    ;
            //clear CSR after log
            RetryRdErrLog.Data = 0;
            RetryRdErrLog.Bits.en = 1;
            RetryRdErrLog.Bits.en_patspr = 1;
            mEmcaPh.CpuCsrAccess->WriteCpuCsr(*Skt, SktCh, RETRY_RD_ERR_LOG_MCDDC_DP_REG, RetryRdErrLog.Data);
          } // if(RetryRdErrLog.Bits.v == 1 && RetryRdErrLog.Bits.patspr == 1)
        } // if (ChannelId)

      } // if (MemSection->ErrorType
    } // if(mImcNodeIdLookup[i].McBankNum)
  } // for (i)
  return EFI_SUCCESS;
}

/**

    Access Mc Bank MSRs to fill Mc signature as defined in the EMCA SAS. 

    @param McBankIdx -  The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    @param skt       - Socket Id
    @param McSig     -  A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS.
    
    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.   
    @retval EFI_SUCCESS - McSig contains valid information  

**/
EFI_STATUS
EFIAPI 
ReadMcBankSignature(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
)
{
  UINT32 msrAddr;
  IA32_MCI_STATUS_MSR_STRUCT MciStsMsr;

  if((McSig == NULL) || (McBankIdx >= GetNumMcBanksPerLp())) {
    return EFI_INVALID_PARAMETER;
  }
  
  msrAddr = GetAddrMcBankSts(McBankIdx);
  MciStsMsr.Data = AsmReadMsr64 (msrAddr);

  //
  // Fill Error Signature
  //
  McSig->Signature.McSts = MciStsMsr.Data;

  if(MciStsMsr.Bits.addrv == 1) {
    msrAddr = GetAddrMcBankAddr(McBankIdx);
    McSig->Signature.McAddr = AsmReadMsr64 (msrAddr);
  } else {
    McSig->Signature.McAddr = 0;
  }
  
  if(MciStsMsr.Bits.miscv == 1) {
    msrAddr = GetAddrMcBankMisc(McBankIdx);
    McSig->Signature.McMisc = AsmReadMsr64 (msrAddr);
  } else {
    McSig->Signature.McMisc = 0;
  }
  McSig->ApicId = GetApicID();
  
  return EFI_SUCCESS;
}

/**

    Fill Mc signature with as if the MSRs access return 0. 

    @param McBankIdx -  The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    @param skt       - Socket Id
    @param McSig     -  A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS.
    
    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.   
    @retval EFI_SUCCESS - McSig contains valid information  

**/
EFI_STATUS
EFIAPI 
ReadNullSignature(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
)
{
  if((McSig == NULL) || (McBankIdx >= GetNumMcBanksPerLp())) {
    return EFI_INVALID_PARAMETER;
  }
  
  McSig->ApicId = GetApicID();
  McSig->Signature.McSts = 0; 
  McSig->Signature.McAddr = 0;
  McSig->Signature.McMisc = 0;
  return EFI_SUCCESS;
}


/**

    Access Mc Bank MSRs to fill Mc signature as defined in the EMCA SAS. If McBank information is invalid use the Shadow CSRs.

    @param McBankIdx -  The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    @param skt       - Socket Id
    @param McSig     -  A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS.
    
    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.   
    @retval EFI_SUCCESS - McSig contains valid information  

**/
EFI_STATUS
EFIAPI 
ReadM2MemMcBankSignature(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD * McSig
)
{
  MCI_STATUS_SHADOW_N0_M2MEM_MAIN_STRUCT Mc5Shadow0Status;
  MCI_STATUS_SHADOW_N1_M2MEM_MAIN_STRUCT Mc5Shadow1Status;
  MCI_MISC_SHADOW_N0_M2MEM_MAIN_STRUCT   Mc5Shadow0Misc;
  MCI_MISC_SHADOW_N1_M2MEM_MAIN_STRUCT   Mc5Shadow1Misc;
  
  MCI_ADDR_SHADOW0_M2MEM_MAIN_STRUCT         Mc5Shadow0Addr; 
  MCI_ADDR_SHADOW1_M2MEM_MAIN_STRUCT         Mc5Shadow1Addr; 
  UINT8                                   Instance;
  UINT8                                   m2mem;
  IA32_MCI_STATUS_MSR_STRUCT MciStsMsr;

  if((McSig == NULL) || (McBankIdx >= GetNumMcBanksPerLp())) {
    return EFI_INVALID_PARAMETER;
  }
  ASSERT(mM2MemLookup != NULL);

  for(m2mem=0, Instance=0; m2mem < mM2MemLookupEntries; m2mem++) {
    if(mM2MemLookup != NULL){
      if(McBankIdx == mM2MemLookup[m2mem].McBankNum) {
        Instance = mM2MemLookup[m2mem].Instance;
      }
    }
  }

  ReadMcBankSignature(McBankIdx,skt,McSig);
  MciStsMsr.Data = McSig->Signature.McSts;

  if(MciStsMsr.Bits.val == 0) {

    Mc5Shadow1Status.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(skt,Instance,MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG);
    Mc5Shadow0Status.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(skt,Instance,MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG);
    
    McSig->Signature.McSts = ((UINT64)Mc5Shadow1Status.Data << 32) | Mc5Shadow0Status.Data;
    MciStsMsr.Data = McSig->Signature.McSts;

    if( MciStsMsr.Bits.val && MciStsMsr.Bits.addrv) {
      Mc5Shadow0Addr.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(skt,Instance, MCI_ADDR_SHADOW0_M2MEM_MAIN_REG);
      Mc5Shadow1Addr.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(skt,Instance, MCI_ADDR_SHADOW1_M2MEM_MAIN_REG);
      
      McSig->Signature.McAddr = ((UINT64)Mc5Shadow1Addr.Data << 32) | (Mc5Shadow0Addr.Data );
      
    } else {
       McSig->Signature.McAddr = 0;
    }

     if( MciStsMsr.Bits.val && MciStsMsr.Bits.miscv) {
      Mc5Shadow0Misc.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(skt,Instance, MCI_MISC_SHADOW_N0_M2MEM_MAIN_REG);
      Mc5Shadow1Misc.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(skt,Instance, MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG);
      
      McSig->Signature.McMisc= ((UINT64)Mc5Shadow1Misc.Data << 32) | (Mc5Shadow0Misc.Data );
      
    } else {
       McSig->Signature.McMisc = 0;
    }
    McSig->ApicId = GetApicID();
  }
  return EFI_SUCCESS;  
}

/**

    Call Bank specific Signature hook. 

    @param McBankIdx -  The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    @param skt       - Socket Id
    @param McSig     -  A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS.
    
    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.   
    @retval EFI_SUCCESS - McSig contains valid information  

**/
EFI_STATUS
EFIAPI 
ReadMcBankSigHook(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD * McSig
)
{
  EFI_STATUS status = EFI_SUCCESS;
  
  if((McSig == NULL) || (McBankIdx >= GetNumMcBanksPerLp()) || skt >= MAX_SOCKET) {
    return EFI_INVALID_PARAMETER;
  }
  
  ASSERT(gMcBankList != NULL);
  
  if(gMcBankList != NULL){
    if(gMcBankList[McBankIdx].ReadMcBankSig != NULL) {
      status = gMcBankList[McBankIdx].ReadMcBankSig(McBankIdx,skt,McSig);  
    } else {
      SetMem(McSig,sizeof(EMCA_MC_SIGNATURE_ERR_RECORD),0);
    }
  }
  return status; 
  
}


/**

    Call Bank Specific CSRs Hook to aggregate Memory Section Information  

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param Skt       - Socket Id 
    @param McSig     - A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
    @param Section   - A pointer to UEFI error section. 

    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.  
    @retval EFI_NOT_FOUND - Hook is not available for the bank 
    @retval EFI_SUCCESS           - Function executed Correctly

**/
EFI_STATUS
MCBankCsrInfoHook(
  IN UINT32 McBankIdx,
  IN UINT8 *Skt,
  IN EMCA_MC_SIGNATURE_ERR_RECORD *McSig,
  IN OUT VOID *Section 
)
{
  EFI_STATUS status;
  status = EFI_NOT_FOUND;
  
  ASSERT(gMcBankList != NULL);
  
  if (gMcBankList != NULL) {
    if(gMcBankList[McBankIdx].Hooks.CsrInfoHook != NULL) {
      status = gMcBankList[McBankIdx].Hooks.CsrInfoHook(&gMcBankList[McBankIdx].Hooks, McBankIdx, Skt, McSig, Section);  
    }
  }
  return status;  
}




/**

    Enable MCE->SMI morphing for McBankIdx 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.

    @retval None

**/
VOID
EmcaEnableMSMI(
  IN UINT32 *McBankIdx
)
{
  UINT64   McBankSupp;
  UINT32   McBankAddr;
  UINT64 McCtl2;

  if(McBankIdx != NULL) {    
    McBankSupp = GetEMcaBankSupp();

    if(((McBankSupp >> *McBankIdx) & 0x1) != 0) {
            
      McBankAddr = GetAddrMcBankCtl2(*McBankIdx);
      McCtl2 = AsmReadMsr64(McBankAddr);
      McCtl2 |= BIT34; //MCE to SMI CTL
      AsmWriteMsr64(McBankAddr,McCtl2);
    }
  }
}

VOID
EnableMsmiBanks(
  IN        VOID  *Buffer
)
{
  UINT64   McBankNum;
  UINT32 i;
  
  McBankNum = GetNumMcBanksPerLp();

  for(i = 0; i < (UINT32)McBankNum; i++) {
    EmcaEnableMSMI(&i);
  }
}

/**

     Enable CMCI->SMI morphing for all the banks 

    @param Params - Used to pass parameters 

    @retval None

**/
VOID
EnableCSMIGen2(
CMCI_GEN2_PARAMS *Params
)
{
  UINT64 McBankNum;
  UINT64 McBankSupp;
  UINT32 i;
  UINT64 McCtl2;
  UINT32 McBankIdx;

  McBankNum = GetNumMcBanksPerLp();
  McBankSupp = GetEMcaBankSupp();
      
  for(i = 0; i < McBankNum; i++) {
    if(((McBankSupp >> i) & 0x1) != 0) {

      McBankIdx = GetAddrMcBankCtl2(i);
      // APTIOV_SERVER_OVERRIDE_RC_START : Skipping CSMI BIT Setting for Memory Bank MSR's M2M0,M2M1, IMC CH 0-5 (00,01,02,10,11,12)
            if ( McBankIdx == MSR_MC07_CTL2 || McBankIdx == MSR_MC08_CTL2 || (McBankIdx >= MSR_MC13_CTL2 && McBankIdx <= MSR_MC18_CTL2))
                continue;
      // APTIOV_SERVER_OVERRIDE_RC_END : Skipping CSMI BIT Setting for Memory Bank MSR's M2M0,M2M1,IMC CH 0-5 (00,01,02,10,11,12)
      McCtl2 = AsmReadMsr64(McBankIdx);
      McCtl2 |= BIT32; //CMCI to SMI CTL
      AsmWriteMsr64(McBankIdx,McCtl2);    
    }
  }
  ReleaseSpinLock(&Params->Busy);  
}

/**

    Enable emca gen 1 CSMI morphing 

    @param node - NodeId System Wide

    @retval None

**/
VOID
EnableCSMIGen1(
UINT8 *node
) 
{
 
  EXRAS_CONFIG_M2MEM_MAIN_STRUCT      ExRasCfg;
  EMCALTCTL_MC_MAIN_STRUCT  EmcaLtCtrl;
  
  UINT8   Socket;
  UINT8   Mc;

  Socket = NODE_TO_SKT(*node);
  Mc  = NODE_TO_MC(*node);

  ExRasCfg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(Socket, Mc, EXRAS_CONFIG_M2MEM_MAIN_REG);
  ExRasCfg.Bits.cfgmcasmioncorr = 1;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr( Socket, Mc, EXRAS_CONFIG_M2MEM_MAIN_REG, ExRasCfg.Data);

  EmcaLtCtrl.Data = (UINT8)mEmcaPh.CpuCsrAccess->ReadCpuCsr(Socket, Mc, EMCALTCTL_MC_MAIN_REG);
  EmcaLtCtrl.Bits.emcalt_sel_smi = 1;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr(Socket, Mc, EMCALTCTL_MC_MAIN_REG, EmcaLtCtrl.Data);
  
}

/**
  This function enables the SMI generation for Corrected errors in eMCA mode

  @param[in] Socket   The socket number

  @retval None.

**/
VOID
EnableEmcaSmiForCorrectedErrors (
    UINT8 Socket,
    UINT8 Mc
  )
{
    EXRAS_CONFIG_M2MEM_MAIN_STRUCT      ExRasCfg;

    ExRasCfg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(Socket, Mc, EXRAS_CONFIG_M2MEM_MAIN_REG);
    ExRasCfg.Bits.cfgmcacmcionfailover = 1;
    ExRasCfg.Bits.cfgmcacmcionmirrorcorr = 1;
    ExRasCfg.Bits.cfgmcasmionmirrorcorr = 0;
    ExRasCfg.Bits.cfgmcacmcioncorrcountthr = 0;
    ExRasCfg.Bits.cfgmcasmioncorrcountthr = 0;
    ExRasCfg.Bits.cfgmcacmcioncorr = 1;
    ExRasCfg.Bits.cfgmcasmioncorr = 0;
    ExRasCfg.Bits.cfgmcacmcionrecov = 1;
    ExRasCfg.Bits.cfgmcasmionrecov = 0;

    mEmcaPh.CpuCsrAccess->WriteCpuCsr( Socket, Mc, EXRAS_CONFIG_M2MEM_MAIN_REG, ExRasCfg.Data);

    return;
}

/**
  This function enables the SMI generation of uncorrected errors in eMCA mode

  @param[in] Socket   The socket number

  @retval None.

**/
VOID
EnableEmcaSmiForUncorrectedErrors (
  IN        UINT8                           Socket
  )
{
  VIRAL_CONTROL_PCU_FUN4_STRUCT     ViralControlPcuFun4;
  EMCA_EN_CORE_IERR_TO_MSMI_UBOX_CFG_STRUCT EmcaEnIERRMSMIUboxCfg;

  EmcaEnIERRMSMIUboxCfg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, 0, EMCA_EN_CORE_IERR_TO_MSMI_UBOX_CFG_REG);
  EmcaEnIERRMSMIUboxCfg.Bits.encoreierrtomsmi = 1;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, EMCA_EN_CORE_IERR_TO_MSMI_UBOX_CFG_REG, EmcaEnIERRMSMIUboxCfg.Data);

  ViralControlPcuFun4.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, 0, VIRAL_CONTROL_PCU_FUN4_REG);
  ViralControlPcuFun4.Bits.emca_mode = 1;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, VIRAL_CONTROL_PCU_FUN4_REG, ViralControlPcuFun4.Data);

    return;
}

/**
  This function enables the SMI generation of uncorrected errors using gen1

  @param[in] Buffer   unused

  @retval None.

**/
VOID
EnableEmcaSmiGen1ForUncorrectedErrors (
  VOID  *Buffer
)
{
    UINT64 Data64;
  // write to MSR MCA_ERROR_CONTROL bit 2
    Data64 = AsmReadMsr64(MCA_ERROR_CONTROL);
    Data64 |=  BIT2;  // enable cbo SMI triggering enable
    AsmWriteMsr64 (MCA_ERROR_CONTROL, Data64);
    
    return;
}


/**
  This function return if MCERR and IERR has been logged.

  @param[in] Socket   The socket number
  @param[in] UmcSmiEn   UMC is enabled

  @retval BOOLEAN.

**/
BOOLEAN
CheckErrLogReg (
  IN UINT8 Socket,
  IN UINT8 EmcaMsmiEn,
  IN UINT8 EmcaCsmiEn
  )
{
    MCERRLOGGINGREG_UBOX_CFG_STRUCT Mcerr;
    IERRLOGGINGREG_UBOX_CFG_STRUCT  Ierr;
    UBOXERRSTS_UBOX_CFG_STRUCT      uboxErrorStatusReg;
    BOOLEAN FoundError = FALSE;
    
  if(EmcaMsmiEn != 0 && mEmcaPh.IioUds->SystemStatus.cpuType != CPU_IVT) {
    Mcerr.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(Socket, 0, MCERRLOGGINGREG_UBOX_CFG_REG);
    Ierr.Data  = mEmcaPh.CpuCsrAccess->ReadCpuCsr(Socket, 0, IERRLOGGINGREG_UBOX_CFG_REG);

    if(Mcerr.Bits.firstmcerrsrcvalid == 1 || Ierr.Bits.firstierrsrcvalid == 1) {
      FoundError = TRUE;
    }
  }

  if(EmcaCsmiEn == 1) {
    uboxErrorStatusReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr( Socket, 0, UBOXERRSTS_UBOX_CFG_REG );
    if( uboxErrorStatusReg.Bits.smi_delivery_valid && uboxErrorStatusReg.Bits.smisrcimc)
        return TRUE;
  }
  return FoundError;
}

/**
  This function return UMC status.

  @param[in] Socket   The socket number
  @param[in] UmcSmiEn   UMC is enabled

  @retval BOOLEAN.

**/
VOID
ClearUmcStatus (
  IN UINT8 Socket
  )
{
  UBOXERRSTS_UBOX_CFG_STRUCT  UboxErrStsReg;

  //
  // Check for pending Uncorrected processor errors
  //
  UboxErrStsReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, 0, UBOXERRSTS_UBOX_CFG_REG);
  UboxErrStsReg.Bits.smisrcumc = 0;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr(Socket, 0, UBOXERRSTS_UBOX_CFG_REG,UboxErrStsReg.Data);
  
}

/**
  This function clears MCERRLOG and IERRLOG.

  @param[in] Socket   The socket number
  @param[in] EmcaMsmiEn   MCE morphing is enabled

  @retval VOID

**/
VOID
ClearErrLogReg(
IN UINT8 Socket,
IN UINT8 EmcaMsmiEn,
IN UINT8 EmcaCsmiEn
) 
{
  MCA_ERR_SRC_LOG_PCU_FUN2_STRUCT McaErrSrc;
  UBOXERRSTS_UBOX_CFG_STRUCT      uboxErrorStatusReg;
  
    if(EmcaMsmiEn != 0) {
      mEmcaPh.CpuCsrAccess->WriteCpuCsr(Socket, 0, MCERRLOGGINGREG_UBOX_CFG_REG,0);
      mEmcaPh.CpuCsrAccess->WriteCpuCsr(Socket, 0, IERRLOGGINGREG_UBOX_CFG_REG,0);
      McaErrSrc.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(Socket, 0, MCA_ERR_SRC_LOG_PCU_FUN2_REG); 
      if(!(McaErrSrc.Bits.msmi_ierr_internal || 
           McaErrSrc.Bits.ierr_internal || 
           McaErrSrc.Bits.ierr ||
           McaErrSrc.Bits.msmi_ierr)) {
        mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, MCA_ERR_SRC_LOG_PCU_FUN2_REG, 0);  
      }
    }
    if(EmcaCsmiEn == 1) {
      uboxErrorStatusReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr( Socket, 0, UBOXERRSTS_UBOX_CFG_REG );
      uboxErrorStatusReg.Bits.smi_delivery_valid  = 0;
      uboxErrorStatusReg.Bits.smisrcimc           = 0;
      mEmcaPh.CpuCsrAccess->WriteCpuCsr(Socket, 0, UBOXERRSTS_UBOX_CFG_REG,uboxErrorStatusReg.Data);
    }
}

/**
  This function clears Shadow Registers.

  @param[in] Socket   The socket number
  @param[in] Mc   Mc Index

  @retval VOID

**/
VOID
ClearMciShadows (
  IN UINT8 Socket, 
  IN UINT8 Mc
  )
{
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_ADDR_SHADOW0_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_ADDR_SHADOW1_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_MISC_SHADOW_N0_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG, 0);
}





/**

    Read Mcg Capability and return it in Buffer 

    @param Buffer - MCG_CAP MSR Value if return status is SUCCESS 

    @retval EFI_SUCCESS - Buffer contains MCG_CAP 

**/
STATIC
EFI_STATUS
ReadMCGCAP(
IN VOID *Buffer
) 
{

 UINT64 *Value;
 Value = (UINT64 *)Buffer;

 if(Value != NULL) {
  *Value = AsmReadMsr64(EFI_IA32_MCG_CAP);
 }
 return EFI_SUCCESS;
}

/**

    Read Core SMI Source MSR which contains bitmap of McBanks containing errors for CORE units. 

    @param None

    @retval retval - Value of CORE_SMI_SRC

**/
UINT64
ReadCoreSmiSrc(
VOID
)
{
  UINT64 retval;
  retval = 0;
  
  retval = AsmReadMsr64(MSR_CORE_SMI_ERR_SRC);
  return retval;
}

/**

    Write Value to SMI SRC to Clear it. 

    @param Buffer - Value to Write to SMI_SRC 

    @retval None

**/
VOID
ClearCoreSmiSrc(
IN VOID *Buffer
)
{
  UINT64 *Value;
  Value = (UINT64 *)Buffer;
  
  AsmWriteMsr64(MSR_CORE_SMI_ERR_SRC,*Value);
}

/**

    Read Uncore SMI Source MSR which contains bitmap of McBanks containing errors for UNCORE units. 

    @param None

    @retval retval - Value of UNCORE_SMI_SRC

**/
UINT64
ReadUncoreSmiSrc(
VOID
)
{
  UINT64 retval;
  retval = 0;
  
  retval = AsmReadMsr64(MSR_UNCORE_SMI_ERR_SRC);
  return retval;
}

/**

    Write Value to UNCORE_SMI SRC to Clear it. 

    @param Buffer - Value to Write to UNCORE_SMI SRC 

    @retval None

**/
VOID
ClearUncoreSmiSrc(
IN VOID *Buffer
)
{
  UINT64 *Value;
  Value = (UINT64 *)Buffer;
  
  AsmWriteMsr64(MSR_UNCORE_SMI_ERR_SRC,*Value);
}



/**

    If successful Write Emca Logging Capability to the input buffer 

    @param EMcaLiteCap - Buffer to write the Emca Logging Capability 

    @retval EFI_INVALID_PARAMETER - EmcaLiteCap pointer is NULL
    @retval EFI_NOT_READY         - Function is executed outside SMM 
    @retval EFI_SUCCESS           - EMcaLiteCap contains valid data

**/
EFI_STATUS
GetEMCALogProcCap(
IN OUT BOOLEAN *EMcaLiteCap
)
{
  BOOLEAN InSmm;
  UINT8 RetVal = 0;

  if(EMcaLiteCap == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if( mEmcaPh.SmmBase == NULL ||  mEmcaPh.Smst == NULL) {
    return EFI_NOT_READY;
  }
  
  mEmcaPh.SmmBase->InSmm(mEmcaPh.SmmBase, &InSmm);

  if(InSmm) {

    ReadMCGCAP(&mMcgCap);
    RetVal = ((mMcgCap & BIT26) >> 26); //Check for EMCA log support capabality bit 
    
  } else {
    return EFI_NOT_READY;
  }

  *EMcaLiteCap = (RetVal == 1);

  return EFI_SUCCESS;
}

/**

    Return TRUE if silicon Supports EMCA Gen 2 and False otherwise 

    @param None

    @retval TRUE - Emca Gen 2 is supported
    @retval FALSE - Emca Gen 2 is not supported

**/
BOOLEAN
isEMCAGen2Cap(
VOID
)
{
  UINT64 McgCap;
  
  if(ReadMCGCAP(&McgCap) != EFI_SUCCESS) {
    McgCap = 0;
  }
    
  return ((McgCap & BIT25) != 0); 
}

/**

    Return bimap of MC Banks that support EMCA gen 2 morphing 

    @param None

    @retval BitMap - Bitmap of McBanks which support morphing 

**/
UINT64
GetEMcaBankSupp(
VOID
)
{
  UINT64 SmmMcgCap;

  SmmMcgCap = AsmReadMsr64(MSR_SMM_MCA_CAP);
  
  return SmmMcgCap & (~((UINT32)0));
}

/**

    Initialize L1 Directory pointers according to eMCA spec. This assumes the McBank Scope is Package Level so all the threads within the socket will share the elog pointer. 

    @param L1DirHdr - L1 directory base pointer 
    @param McBank   - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param NxtElog  - Pointer to Next Free Elog memory 

    @retval EFI_INVALID_PARAMETER - At least of the input pointers is NULL
    @retval EFI_OUT_OF_RESOURCES  - Elog directory memory is insufficient  
    @retval EFI_SUCCESS           - The initialization was successful

**/
EFI_STATUS
InitL1PtrForSockets(
  IN EMCA_L1_HEADER *L1DirHdr,
  IN UINT32 McBank, 
  IN OUT VOID **NxtElog
  )
{
  UINT8 s;
  UINT8 c;
  UINT8 t;
  UINT8 MaxThread;
  EFI_CPU_PHYSICAL_LOCATION loc;
  UINT8 *L1DirBase = NULL;
  UINT8 *L1DirEnd = NULL;
  UINT8 *ElogEnd = NULL;
  UINT8 *NxtElogIter = NULL;
  UINT64 *L1DirIter = NULL;
  UINTN Corebits;
  UINTN Threadbits;  

  if(L1DirHdr == NULL || NxtElog == NULL || McBank >= L1DirHdr->NumL1EntryLp) {
    return EFI_INVALID_PARAMETER;
  }

  GetCoreAndSMTBits(&Threadbits,&Corebits);
  MaxThread = (1<<Threadbits);

  NxtElogIter = (UINT8 *) *NxtElog;
  ElogEnd = (UINT8 *)(L1DirHdr->ElogDirBase + L1DirHdr->ElogDirLen); 
  L1DirBase = (UINT8 *)((UINT64)L1DirHdr + (UINT64)L1DirHdr->HdrLen);
  L1DirEnd = (UINT8 *)((UINT64)L1DirHdr + L1DirHdr->L1DirLen); 

  for(s = 0; s < MAX_SOCKET; s++, NxtElogIter += ELOG_ENTRY_LEN) {
    for(c = 0; c < LShiftU64(1 , Corebits); c++) {
      for(t = 0; t < MaxThread; t++) {
        loc.Package = s;
        loc.Core = c; 
        loc.Thread = t;         
        L1DirIter = (UINT64 *)((UINT64)L1DirBase + (GetApicIdByLoc(&loc) * (L1DirHdr->NumL1EntryLp * sizeof(UINT64))) + (McBank * sizeof(UINT64)));
        
        if(NxtElogIter < ElogEnd && (UINT8 *)L1DirIter < L1DirEnd) {
          *L1DirIter = BIT63 | (UINT64)NxtElogIter;    
        } else {
          return EFI_OUT_OF_RESOURCES; 
        }
      }
    }
  }
  *NxtElog = (VOID*)NxtElogIter;
  return EFI_SUCCESS; 
}

/**

    Initialize L1 Directory pointers according to eMCA spec. This assumes the McBank Scope is Thread Level so each thread will have its own elog pointer. 

    @param L1DirHdr - L1 directory base pointer 
    @param McBank   - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param NxtElog  - Pointer to Next Free Elog memory 

    @retval EFI_INVALID_PARAMETER - At least of the input pointers is NULL
    @retval EFI_OUT_OF_RESOURCES  - Elog directory memory is insufficient  
    @retval EFI_SUCCESS           - The initialization was successful

**/
EFI_STATUS
InitL1PtrForThreads(
  IN EMCA_L1_HEADER *L1DirHdr,
  IN UINT32 McBank, 
  IN OUT VOID **NxtElog
  )
{
  UINT8 s;
  UINT8 c;
  UINT8 t;
  UINT8 MaxThread;
  EFI_CPU_PHYSICAL_LOCATION loc;
  UINT8 *L1DirBase = NULL;
  UINT8 *L1DirEnd = NULL;
  UINT8 *ElogEnd = NULL;
  UINT8 *NxtElogIter = NULL;
  UINT64 *L1DirIter = NULL;
  UINTN Corebits;
  UINTN Threadbits;
  

  if(L1DirHdr == NULL || NxtElog == NULL || McBank >= L1DirHdr->NumL1EntryLp) {
    return EFI_INVALID_PARAMETER;
  }

  GetCoreAndSMTBits(&Threadbits,&Corebits);
  MaxThread =  1 << Threadbits;

  NxtElogIter = (UINT8 *) *NxtElog;
  ElogEnd = (UINT8 *)(L1DirHdr->ElogDirBase + L1DirHdr->ElogDirLen); 
  L1DirBase = (UINT8 *)((UINT64)L1DirHdr + (UINT64)L1DirHdr->HdrLen);
  L1DirEnd = (UINT8 *)((UINT64)L1DirHdr + L1DirHdr->L1DirLen); 

  for(s = 0; s < MAX_SOCKET; s++) {
    for(c = 0; c < LShiftU64(1 , Corebits); c++) {
      for(t = 0; t < MaxThread; t++, NxtElogIter += ELOG_ENTRY_LEN) {
        loc.Package = s;
        loc.Core = c; 
        loc.Thread = t;         
        L1DirIter = (UINT64 *)((UINT64)L1DirBase + (GetApicIdByLoc(&loc) * (L1DirHdr->NumL1EntryLp * sizeof(UINT64))) + (McBank * sizeof(UINT64)));
        
        if(NxtElogIter < ElogEnd && (UINT8 *)L1DirIter < L1DirEnd) {
          *L1DirIter = BIT63 | (UINT64)NxtElogIter;    
        } else {
          return EFI_OUT_OF_RESOURCES; 
        }
      }
    }
  }
  
  *NxtElog = (VOID*)NxtElogIter;
  return EFI_SUCCESS; 
}

/**

    Initialize L1 Directory pointers according to eMCA spec. This assumes the McBank Scope is Core Level so each Thread within the Core will share elog pointer. 

    @param L1DirHdr - L1 directory base pointer 
    @param McBank   - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param NxtElog  - Pointer to Next Free Elog memory 

    @retval EFI_INVALID_PARAMETER - At least of the input pointers is NULL
    @retval EFI_OUT_OF_RESOURCES  - Elog directory memory is insufficient  
    @retval EFI_SUCCESS           - The initialization was successful

**/
EFI_STATUS
InitL1PtrForCores(
  IN EMCA_L1_HEADER *L1DirHdr,
  IN UINT32 McBank,
  IN OUT VOID **NxtElog
  )
{  
  UINT8 s;
  UINT8 c;
  UINT8 t;
  UINT8 MaxThread;
  EFI_CPU_PHYSICAL_LOCATION loc;
  UINT8 *L1DirBase = NULL;
  UINT8 *L1DirEnd = NULL;
  UINT8 *ElogEnd = NULL;
  UINT8 *NxtElogIter = NULL;
  UINT64 *L1DirIter = NULL;
  UINTN Corebits;
  UINTN Threadbits;
  

  if(L1DirHdr == NULL || NxtElog == NULL || McBank >= L1DirHdr->NumL1EntryLp) {
    return EFI_INVALID_PARAMETER;
  }

  GetCoreAndSMTBits(&Threadbits,&Corebits);
  MaxThread =  1 << Threadbits;

  NxtElogIter = (UINT8 *) *NxtElog;
  ElogEnd = (UINT8 *)(L1DirHdr->ElogDirBase + L1DirHdr->ElogDirLen); 
  L1DirBase = (UINT8 *)((UINT64)L1DirHdr + (UINT64)L1DirHdr->HdrLen);
  L1DirEnd = (UINT8 *)((UINT64)L1DirHdr + L1DirHdr->L1DirLen);  

  for(s = 0; s < MAX_SOCKET; s++) {
    for(c = 0; c < LShiftU64(1 , Corebits); c++, NxtElogIter += ELOG_ENTRY_LEN) {
      for(t = 0; t < MaxThread; t++) {
        loc.Package = s;
        loc.Core = c; 
        loc.Thread = t;         
        L1DirIter = (UINT64 *)((UINT64)L1DirBase + (GetApicIdByLoc(&loc) * (L1DirHdr->NumL1EntryLp * sizeof(UINT64))) + (McBank * sizeof(UINT64)));
        
        if(NxtElogIter < ElogEnd && (UINT8 *)L1DirIter < L1DirEnd) {
          *L1DirIter = BIT63 | (UINT64)NxtElogIter;    
        } else {
          return EFI_OUT_OF_RESOURCES; 
        }
      }
    }
  }
  
  *NxtElog = (VOID*)NxtElogIter;
  return EFI_SUCCESS; 
}

/**

    Private Function used as placeholder for Recovery Function

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param McSig     - A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 

    @retval FALSE - Recovery was not successful 
    @retval TRUE - Recovery was successful 

**/
BOOLEAN
RecoveryPatrolScrubDummy(
IN UINT32 McBankIdx,
IN OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
)
{
  //Dummy function until we identify a recoverable error
  return FALSE;
}

/**

    Iterate within Silicon Specific Hooks to identify and perform recovery of errors.  

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param Skt       - Socket Id
    @param McSig     -  A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
                                This function should update the values in the McSig if the Recovery is successful.     

    @retval FALSE - Recovery was not successful 
    @retval TRUE - Recovery was successful 

**/
BOOLEAN
AttemptRecovery(
IN UINT32 McBankIdx,
IN UINT8 Skt,
IN OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
)
{
  UINTN i;
  IA32_MCI_STATUS_MSR_STRUCT McSts;
  
  
  if(isEMCAGen2Cap() == FALSE){
    return FALSE;
  }

  if((McSig == NULL) || 
     (McBankIdx >= GetNumMcBanksPerLp()) ||
     Skt >= MAX_SOCKET) {
    return FALSE;
  }

  McSts.Data = McSig->Signature.McSts;

  if(McSts.Bits.val == 0  || 
     McSts.Bits.over == 1 ||
     McSts.Bits.en == 0
    ) {
    return FALSE;
  }
     

  for(i = 0; i < gMcaRecorevablesEntries;i++) {
    
    if( ((McSts.Bits.mca_code & gMcaRecoverables[i].McaCodMsk) == gMcaRecoverables[i].McaCod ) &&
        ((McSts.Bits.ms_code & gMcaRecoverables[i].MsCodMsk) == gMcaRecoverables[i].MsCod )
      ) {

      return (gMcaRecoverables[i].RecoveryHdr != NULL) ? gMcaRecoverables[i].RecoveryHdr(McBankIdx, McSig):FALSE;
        
    }
  }

  return FALSE;
}

/**

    Write updated values for corrected error in the McBank. The Firmware updated bit must be set. 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param McSig     - A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
                               The values in the signature will be written to the banks. 

    @retval None

**/
VOID
WriteMcBank
(
  IN UINT32 McBankIdx,
  IN OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
) 
{
  IA32_MCI_STATUS_MSR_STRUCT McSts;
  UINT32 msrAddr;

  if(isEMCAGen2Cap() == FALSE) {
    return;
  }
  
  if((McSig == NULL) || 
     (McBankIdx >= GetNumMcBanksPerLp())
     ) {
    return;
  }
  
  McSts.Data = McSig->Signature.McSts;

  if(McSts.Bits.val != 1 && (McSts.Bits.other_info & MCA_FIRMWARE_UPDATED_BIT) != 0) {
    msrAddr = GetAddrMcBankSts(McBankIdx);
    AsmWriteMsr64(msrAddr, McSts.Data);

    if(McSts.Bits.miscv == 1) {
      msrAddr = GetAddrMcBankMisc(McBankIdx);
      AsmWriteMsr64(msrAddr, McSig->Signature.McMisc);
    }

    if(McSts.Bits.addrv == 1) {
      msrAddr = GetAddrMcBankAddr(McBankIdx);
      AsmWriteMsr64(msrAddr, McSig->Signature.McAddr);
    }
  }  
}

/**

    Clear the value in a given McBank.

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.

    @retval None
**/
VOID
ClearMcBank
(
  IN UINT32 McBankIdx
)
{
  UINT32 MsrAddr;

  MsrAddr = GetAddrMcBankSts(McBankIdx);
  AsmWriteMsr64(McBankIdx, 0);

  MsrAddr = GetAddrMcBankMisc(McBankIdx);
  AsmWriteMsr64(McBankIdx, 0);

  MsrAddr = GetAddrMcBankAddr(McBankIdx);
  AsmWriteMsr64(McBankIdx, 0);

}


/**
    

    Disables CSMI at MC bank

    @param McBank - McBank number

    @retval NONE

**/
VOID
DisableCSMIAtMcBank(
UINT8 McBank
)
{
  UINT64 McCtl2;
  UINT32 McBankIdx;
  
  McBankIdx = GetAddrMcBankCtl2(McBank);
  McCtl2 = AsmReadMsr64(McBankIdx);
  McCtl2 &= ~BIT32; // disable CMCI to SMI CTL
  AsmWriteMsr64(McBankIdx,McCtl2);    
  
}

/**
  Signal an MCE via MMIO

  This function signals an MCE using the MMIO mechanism

  @param[in] LogicalCpu     The CPU number of the CPU that generated the error

  @retval Status.

**/
EFI_STATUS
SignalMceMmio ( 
  IN      UINTN LogicalCpu
)
{
  //
  //write 0x1 at the offset pointed by Events control field
  //
  /* Events control field is a 2-byte field in at offset 0xff04 of SMRAM
    if on chip save state is not enabled */
  UINTN     SmramSmbase;
  UINTN     EventsCtrl;
  UINT16    *EventsCtrlPtr;

  if (LogicalCpu >= gSmst->NumberOfCpus) {
    return EFI_INVALID_PARAMETER;
  }
  SmramSmbase = ((UINTN *)gSmst->CpuSaveState)[LogicalCpu] - SIZE_64KB + ((UINTN *)gSmst->CpuSaveStateSize)[LogicalCpu];
  EventsCtrl = (UINTN)SmramSmbase + EVENTS_CTRL_OFFSET;
  EventsCtrlPtr = (UINT16 *) EventsCtrl;
  *EventsCtrlPtr = 0x0001;

  return EFI_SUCCESS;
}

/**
  Signal an MCE via MSR

  This function signals an MCE using the MSR mechanism

  @param[in] MceArgs     Structure for passing variables in smmstartthisapp across thread

**/
VOID
SignalMceMsr ( 
  IN      MCEMSR_ARGS_STRUCT *MceArgs
)
{
  /* Events control is 2-byte field in MSR 0xC1F (b[47:32])
      if on-chip state save is enabled*/
  UINT64    EventsCtrlMsr;
  UINT64    EventsCtrlMsrVal;

  if (MceArgs == NULL) {
    return;
  }

  EventsCtrlMsr = AsmReadMsr64 (UCODE_CR_SMRAM_EVENT_CTL_HLT_IO); // SKX_TODO - register needs porting
  EventsCtrlMsrVal = EventsCtrlMsr | 0x100000000;
  AsmWriteMsr64 (UCODE_CR_SMRAM_EVENT_CTL_HLT_IO,EventsCtrlMsrVal);
  if (MceArgs->Sync == TRUE) {
    MpSyncReleaseAp (MceArgs->LogicalCpu);
  }
}

/**
  Determine MCE Mechanism

  This function returns the mechanism to use to generate the MCE

  @retval UINT32    0 MMIO
                    1 MSR

**/
UINT32
GetSaveStateType ( 
  UINT16 SocketBitMap
)
{
  UINT8                                 Socket;
  SMM_FEATURE_CONTROL_UBOX_DFX_STRUCT   SmmCsrVal;
  UINT32                                RetVal;

  RetVal = 0;

  for (Socket=0; Socket < MAX_SOCKET; Socket++) {
    if ((SocketBitMap & (1 << Socket)) == 0) {
      continue;
    }
    SmmCsrVal.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(
                                    Socket,
                                    0,
                                    SMM_FEATURE_CONTROL_UBOX_DFX_REG
                                    );
    if (SmmCsrVal.Bits.smm_cpu_save_en == 0) {
      RetVal |= 0;
    } else {
      RetVal |= 1;
    }
  }

  return RetVal;
}

/**
  Read Local APIC Reg (copy)

  This function reads a Local Apic Register


  @param[in] MmioOffset  Offset to read

  @retval UINT32         Register contents

**/

UINT32
EFIAPI
ReadLocalApicRegrep ( 
  IN      UINTN  MmioOffset
  )
{
  UINT32    MsrIndex;

  ASSERT ((MmioOffset & 0xf) == 0);

  if (GetApicMode () == LOCAL_APIC_MODE_XAPIC) {
    return MmioRead32 (PcdGet32 (PcdCpuLocalApicBaseAddress) + MmioOffset);
  } else {
    //
    // DFR is not supported in x2APIC mode.
    //
    ASSERT (MmioOffset != XAPIC_ICR_DFR_OFFSET);
    //
    // Note that in x2APIC mode, ICR is a 64-bit MSR that needs special treatment. It
    // is not supported in this function for simplicity.
    //
    ASSERT (MmioOffset != XAPIC_ICR_HIGH_OFFSET);

    MsrIndex = (UINT32)(MmioOffset >> 4) + X2APIC_MSR_BASE_ADDRESS;
    return AsmReadMsr32 (MsrIndex);
  }
}

/**
  Signal a CMCI

  This function will signal a CMCI

  @param[in] CmciVarPtr  Structure for passing variables in smmstartthisapp across thread

**/
VOID SignalCmci ( 
  CMCI_ARGS_STRUCT *CmciVarPtr
)
{
  UINT32                      BankIndex;
  UINT32                      ApicId;
  LOCAL_APIC_LVT_CMCI         CmciLvt;
  UINT32                      CmciVec;
  UINT32                      MsrIdx;
  IA32_MCI_CTL2_MSR_STRUCT    Ctl2;

  BankIndex = CmciVarPtr->BankIndex;
  ApicId = CmciVarPtr->ApicId;

  // Find if CMCI enabled
  MsrIdx = GetAddrMcBankCtl2 (BankIndex);
  Ctl2.Data = AsmReadMsr64 (MsrIdx);
  if (Ctl2.Bits.CMCI_EN) {
    CmciLvt.Uint32 = ReadLocalApicRegrep (CMCI_LVT_OFFSET);
    if (!CmciLvt.Bits.Mask) {
      CmciVec = CmciLvt.Bits.Vector;
      SendFixedIpi (ApicId,(UINT8)CmciVec);
    }
  }
  ReleaseSpinLock (&CmciVarPtr->ApBusy);
  return ;
}

/**
  Is Lmce supported by the firmware
  
  @param[in] LmceFwrEn True if Lmce is supported by the firmware
  @retval NONE  

**/
VOID
IsLmceFwrEnabled(
  BOOLEAN *LmceFwrEn
  )
{
  UINT64                            FtControl;

  if(LmceFwrEn != NULL) {
    FtControl = AsmReadMsr64(MSR_IA32_FEATURE_CONTROL);
    if(BitFieldRead64(FtControl, N_MSR_LMCE_ON_BIT, N_MSR_LMCE_ON_BIT) == 0) {
      *LmceFwrEn = FALSE;
    } else {
      *LmceFwrEn = TRUE;
    }
  }  
}

/**
  Is Lmce supported by the processor
  
  @param[in] Supported True if Lmce is supported by the processor 
  @retval NONE  

**/
VOID
IsLmceSupported (
  BOOLEAN * Supported
  )
{
  UINT64                            McgCap;

  if(Supported != NULL) {
    McgCap = AsmReadMsr64(EFI_IA32_MCG_CAP);
    if ((McgCap & N_MSR_MCG_LMCE_P_BIT) == 0) {
      *Supported = FALSE;
    } else {
      *Supported = TRUE;
    } 
  }
}

/**
  Has OS enabled Lmce in the mcg_ext_ctl
  
  @param[in] LmceEn Return whether OS has enabled lmce in the mcg_ext_ctl 
  @retval NONE  

**/
VOID
IsLmceOsEnabled (
  OUT BOOLEAN * LmceEn
  )
{
  BOOLEAN                           LmceSupp;
  BOOLEAN                           LmceFwrEn;

  if(LmceEn != NULL) {
    IsLmceSupported(&LmceSupp);
    IsLmceFwrEnabled(&LmceFwrEn);
    
    if ( LmceSupp && LmceFwrEn && ((AsmReadMsr64(IA32_MCG_EXT_CTL) & N_MSR_IA32_LMCE_EN) != 0)) {
      *LmceEn = TRUE;
    } else {
      *LmceEn = FALSE;
    }
  }
}

/**
  Return if Local machine check exception signaled. Indicates (when set) that a local machine check
exception was generated. This indicates that the current machine-check event was delivered to only the
logical processor.

  @retval TRUE  LMCE was signaled.
  @retval FALSE LMCE was not signaled.

**/
VOID
IsLmceSignaled (
  IN BOOLEAN *Signaled
  )
{
  IA32_MCG_STATUS_MSR_STRUCT McgStatus;

  if(Signaled != NULL) {
    McgStatus.Data = AsmReadMsr32 (IA32_MCG_STATUS);
    *Signaled = (McgStatus.Bits.lmce == 1);
  } 
}

/**

    Enable LMCE

    @param Buffer 

    @retval None

**/
VOID
EnableLMCE(
  IN BOOLEAN  *Enabled
)
{
  UINT64                           IA32FeatureControl = 0;
  BOOLEAN                          LmceSupp = FALSE;
  BOOLEAN                          LmceEn = FALSE;

  if(Enabled != NULL) {
    LmceEn = *Enabled;
  }

  IA32FeatureControl = AsmReadMsr64(EFI_MSR_IA32_FEATURE_CONTROL);
  IsLmceSupported(&LmceSupp);

  if(BitFieldRead64(IA32FeatureControl,N_MSR_FEATURE_CONTROL_LOCK,N_MSR_FEATURE_CONTROL_LOCK) == 0) {
    if(LmceSupp && LmceEn) {
      IA32FeatureControl |= (1 << N_MSR_LMCE_ON_BIT);
      AsmWriteMsr64(EFI_MSR_IA32_FEATURE_CONTROL, IA32FeatureControl);
    }
    //WriteRegisterTable(ProcessorNumber, Msr, EFI_MSR_IA32_FEATURE_CONTROL, 0, 64, IA32FeatureControl);
  }
}

/**
  Clear Lmce if simultaneous BMCE and LMCE happen. 
  
**/
VOID
ClearLmceStatus (
  IN VOID *Buffer
  )
{
  IA32_MCG_STATUS_MSR_STRUCT McgStatus;

  McgStatus.Data = AsmReadMsr32 (IA32_MCG_STATUS);
  McgStatus.Bits.lmce = 0; 
  AsmWriteMsr32(IA32_MCG_STATUS,McgStatus.Data); 
}

/**
  return whether BMCE happen. 

  @retval TRUE  BMCE was signaled.
  @retval FALSE BMCE was not signaled.

**/
BOOLEAN
isBMCE(
  UINT16 SocketBitMap
  )
{
  UINT8                                 Socket;
  MCA_ERR_SRC_LOG_PCU_FUN2_STRUCT     errSrcLog;
  BOOLEAN                                RetVal;

  RetVal = FALSE;

  for (Socket=0; Socket < MAX_SOCKET; Socket++) {
    if ((SocketBitMap & (1 << Socket)) == 0) {
      RASDEBUG ((DEBUG_ERROR, "isbmce continue socket %lx\n",Socket));
      continue;
    }
    errSrcLog.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr(
                                    Socket,
                                    0,
                                    MCA_ERR_SRC_LOG_PCU_FUN2_REG
                                    );
    RASDEBUG ((DEBUG_ERROR, "MCA_ERR_SRC %lx\n",errSrcLog.Data));
    if (errSrcLog.Bits.msmi_internal == 1 ||
        errSrcLog.Bits.msmi ==  1
        ) {
      RetVal |= TRUE;
    } else {
      RetVal |= FALSE;
    }
  }

  return RetVal;
}

/**

  Populate Silicon Capability Structure of Platform RAS Protocol

  @param *CapabilityPtr  - Pointer to the Silcon RAS Capability Structure

  @retval VOID

**/
VOID
PopulateSiliconCapabilityStructure (
      IN OUT   SILICON_RAS_CAPABILITY   *CapabilityPtr
) 
{

  UINT64        McgCap;
  BOOLEAN       EmcaLog;

  McgCap = AsmReadMsr64 (MSR_IA32_MCG_CAP);
  CapabilityPtr->PoisonCap = (McgCap & 0x1000000) ? TRUE : FALSE;

  CapabilityPtr->EmcaGen1Cap = FALSE;
  CapabilityPtr->EmcaGen2Cap = FALSE;
  if(GetEMCALogProcCap(&EmcaLog) == EFI_SUCCESS) {
      CapabilityPtr->EmcaGen1Cap = EmcaLog;
  }
  CapabilityPtr->EmcaGen2Cap = isEMCAGen2Cap();
  IsLmceSupported(&CapabilityPtr->LmceCap);

  return;
}





