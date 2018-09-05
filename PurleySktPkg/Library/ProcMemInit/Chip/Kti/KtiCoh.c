/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
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
// *                                                                        *
// **************************************************************************
**/
#include "DataTypes.h"
#include "PlatformHost.h"
#include "KtiLib.h"
#include "KtiMisc.h"
#include "SysHost.h"
#include "SysFunc.h"
#include "KtiCoh.h"
#include "RcRegs.h"
#include "KtiSetupDefinitions.h"
#include "SysHostChip.h"

/*  Here is IIO/M2PCIE index mapping in term of BDF
M2PCIE Index
                iio0    CSTACK             iiocstack120    M2PCIE1    m2pcie20      Include DMI/CBDMA    3       22      0
    0           iio1    PSTACK1            iiopstack020    M2PCIE1    m2pcie20      Include DMI/CBDMA    3       22      0
    1           iio2    PSTACK2            iiopstack010    M2PCIE0    m2pcie10      Include Ubox         3       21      0
    2           iio3    PSTACK3            iiopstack050    M2PCIE4    m2pcie50                           3       22      4
    3           iio4    MSTACK0            iiopstack030    M2PCIE2    m2pcie30                           3       23      0
    4           iio5    MSTACK1            iiopstack040    M2PCIE3    m2pcie40                           3       21      4
*/


//M2Pcie -> M3KTI KTI0 or 1( for VN0 AD Ingress)
STATIC UINT8   IioToKti0_Vn0AdCreditTable[MAX_SKX_M2PCIE]= {
    7, 4, 7, 5, 0                 //add CSTACK credit and PSTACK0 credit for M2D(PCIE1),
};

//M2Pcie -> M3KTI KTI2( for VN0 AD Ingress)  EX SKU should not have MCP links
STATIC UINT8   IioToKti2_Vn0AdCreditTable[MAX_SKX_M2PCIE]= {
    7, 6, 4, 5, 0                //add CSTACK credit and PSTACK0 credit for M2D(PCIE1),
};

//IIO -> M3KTI KTI0 or 1 ( for BL NCS)
STATIC UINT8   IioToKti0_BlCreditTable_For3M2Pcie[MAX_SKX_M2PCIE]= {
    4, 3, 5, 0, 0
};

STATIC UINT8   IioToKti0_BlCreditTable_For3M2PcieAnd8Socket[MAX_SKX_M2PCIE]= {
    3, 3, 4, 0, 0
};

STATIC UINT8   IioToKti0_BlCreditTable_For4M2Pcie[MAX_SKX_M2PCIE]= {
    4, 3, 4, 3, 0
};

STATIC UINT8   IioToKti2_BlCreditTable_For3M2Pcie[MAX_SKX_M2PCIE]= {
    4, 3, 5, 0, 0
};

STATIC UINT8   IioToKti2_BlCreditTable_For3M2PcieAnd8Socket[MAX_SKX_M2PCIE]= {
    3, 3, 4, 0, 0
};

STATIC UINT8   IioToKti2_BlCreditTable_For4M2Pcie[MAX_SKX_M2PCIE]= {
    4, 3, 4, 3, 0
};

//IIO -> IIO  ( for BL NCS/NCB )
// **Note**  NCS/NCB shared credits. Need to subtract 1 when programming from buffer credit excel sheet
STATIC UINT8   IioToIioBlCreditTableFor3M2Pcie_FullConnected[MAX_SKX_M2PCIE][MAX_SKX_M2PCIE] = {        // 3 M2Pcie
  {3, 5, 6, 0, 0 },       // pstack1 -> pstack 1,2,3,4,5
  {5, 0, 6, 0, 0 },       // pstack2 -> pstack 1,3,4,5
  {6, 6, 0, 0, 0 },       // pstack3 -> pstack 1,2,4,5
  {0, 0, 0, 0, 0 },       // pstack4 -> pstack 1,2,3,5
  {0, 0, 0, 0, 0 }        // pstack5 -> pstack 1,2,3,4
};

STATIC UINT8   IioToIioBlCreditTableFor3M2Pcie[MAX_SKX_M2PCIE][MAX_SKX_M2PCIE] = {        // 3 M2Pcie
  {4, 5, 6, 0, 0},       // pstack1 -> pstack 1,2,3,4,5
  {5, 0, 6, 0, 0},       // pstack2 -> pstack 1,3,4,5
  {6, 6, 0, 0, 0},       // pstack3 -> pstack 1,2,4,5
  {0, 0, 0, 0, 0},       // pstack4 -> pstack 1,2,3,5
  {0, 0, 0, 0, 0}        // pstack5 -> pstack 1,2,3,4
};

STATIC UINT8   IioToIioBlCreditTableFor4M2Pcie2Kti[MAX_SKX_M2PCIE][MAX_SKX_M2PCIE]= {     //EP 4 M2pcie Dual links
  {3, 5, 6, 5, 0},       // pstack1 -> pstack 1 2,3,4,5
  {5, 0, 6, 5, 0},       // pstack2 -> pstack 1,3,4,5
  {5, 6, 0, 5, 0},       // pstack3 -> pstack 1,2,4,5
  {5, 4, 5, 0, 0},       // pstack4 -> pstack 1,2,3,5
  {0, 0, 0, 0, 0}        // pstack5 -> pstack 1,2,3,4
};

STATIC UINT8   IioToIioBlCreditTableFor4M2Pcie0Kti[MAX_SKX_M2PCIE][MAX_SKX_M2PCIE]= {     //EP 4 M2pcie 0 KTI link (1S
  {4, 6, 6, 6, 0},       // pstack1 -> pstack 1,2,3,4,5
  {6, 0, 6, 6, 0},       // pstack2 -> pstack 1,3,4,5
  {6, 6, 0, 6, 0},       // pstack3 -> pstack 1,2,4,5
  {6, 6, 6, 0, 0},       // pstack4 -> pstack 1,2,3,5
  {0, 0, 0, 0, 0}        // pstack5 -> pstack 1,2,3,4
};

STATIC UINT8   IioToIioBlCreditTableFor4M2Pcie3Kti[MAX_SKX_M2PCIE][MAX_SKX_M2PCIE]= {    //EX 4 M2Pcie 3 Kti (full connected)
  {3, 3, 4, 3, 0},       // pstack1 -> pstack 1,2,3,4,5
  {3, 0, 4, 4, 0},       // pstack2 -> pstack 1,3,4,5
  {4, 4, 0, 3, 0},       // pstack3 -> pstack 1,2,4,5
  {3, 4, 4, 0, 0},       // pstack4 -> pstack 1,2,3,5
  {0, 0, 0, 0, 0}        // pstack5 -> pstack 1,2,3,4
};

typedef enum {
  KTI0_NCS_INDEX = 0,
  KTI1_NCS_INDEX,
  KTI2_NCS_INDEX,
  KTI0_NCB_INDEX,
  KTI1_NCB_INDEX,
  KTI2_NCB_INDEX,
  MAX_KTI_M2PCIE_ENTRY
} KTI_INDEX;

//KTI-> M2PCIE ( for BL NCB/NCS )
//Case: 3 M2PCIE/3KTI, 3 M2PCIE/2KTI, 4 M2PCIE/2KTI
STATIC UINT8   KtiToM2pcieCreditTable[MAX_KTI_M2PCIE_ENTRY][MAX_SKX_M2PCIE]= {
  {2, 2, 2, 2, 0},       // Kti 0   -> M2PCIE1, 2, 3. 4. 5 for BL NCS
  {1, 1, 1, 1, 0},       // Kti 1   -> M2PCIE1, 2, 3. 4. 5 for BL NCS
  {2, 2, 1, 0, 0},       // Kti 2   -> M2PCIE1, 2, 3, 4, 5 for BL NCS
  {3, 3, 4, 3, 0},       // Kti 0   -> M2PCIE1, 2, 3, 4, 5 for BL NCB
  {3, 3, 4, 3, 0},       // Kti 1   -> M2PCIE1, 2, 3, 4, 5 for BL NCB
  {3, 3, 2, 0, 0}        // Kti 2   -> M2PCIE1. 2, 3, 4. 5 for BL NCB
};

//Case: 4 M2PCIE/3KTI
STATIC UINT8   KtiToM2pcieCreditTableFor4M2Pcie_3KTI[MAX_KTI_M2PCIE_ENTRY][MAX_SKX_M2PCIE]= {
  {2, 2, 2, 2, 0},       // Kti 0   -> M2PCIE1, 2, 3. 4. 5 for BL NCS
  {1, 1, 1, 1, 0},       // Kti 1   -> M2PCIE1, 2, 3. 4. 5 for BL NCS
  {2, 2, 1, 2, 0},       // Kti 2   -> M2PCIE1, 2, 3, 4, 5 for BL NCS
  {3, 2, 5, 3, 0},       // Kti 0   -> M2PCIE1, 2, 3, 4, 5 for BL NCB
  {3, 2, 5, 3, 0},       // Kti 1   -> M2PCIE1, 2, 3, 4, 5 for BL NCB
  {3, 3, 2, 3, 0}        // Kti 2   -> M2PCIE1. 2, 3, 4. 5 for BL NCB
};

//Case: 3 M2PCIE/3KTI as 2S3L
STATIC UINT8   KtiToM2pcieCreditTableFor2S_3M2Pcie_3Kti[MAX_KTI_M2PCIE_ENTRY][MAX_SKX_M2PCIE]= {
  {2, 2, 2, 0, 0},       // Kti 0   -> M2PCIE1, 2, 3. 4. 5 for BL NCS
  {1, 1, 1, 0, 0},       // Kti 1   -> M2PCIE1, 2, 3. 4. 5 for BL NCS
  {2, 2, 2, 0, 0},       // Kti 2   -> M2PCIE1, 2, 3, 4, 5 for BL NCS
  {3, 3, 4, 0, 0},       // Kti 0   -> M2PCIE1, 2, 3, 4, 5 for BL NCB
  {3, 3, 4, 0, 0},       // Kti 1   -> M2PCIE1, 2, 3, 4, 5 for BL NCB
  {2, 3, 4, 0, 0}        // Kti 2   -> M2PCIE1. 2, 3, 4. 5 for BL NCB
};


//start bit for each m2pcie stack on R2NCSTOIIOINITCRED register
STATIC UINT8   DestStartBitTable[MAX_SKX_M2PCIE] = {
  4,      //PSTACK1  M2PCIE20
  8,      //PSTACK2  M2PCIE10
  12,     //PSTACK3  M2PCIE50
  16,     //MSTACK0  M2PCIE30
  20      //MSTACK1  M2PCIE40
};

/**

  Program mesh credits.

  @param host              - pointer to the system host root structure
  @param SocketData        - pointer to socket data structure
  @param KtiInternalGlobal - pointer to KTIRC Internal Global data

  @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramMeshCredits (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{

  MCAErrWa(host, SocketData, TRUE);

  if ((host->var.kti.SysConfig == SYS_CONFIG_1S) && (KtiInternalGlobal->ProgramCreditFpga == 0)) {
    ProgramMeshCredits1S (host, KtiInternalGlobal);
  } else {
    ProgramMeshCreditsCommonSkx (host, SocketData, KtiInternalGlobal);
  }

  return KTI_SUCCESS;
}

/**

  Program CHA -> M2M credits

  @param host              - pointer to the system host root structure
  @param Soc               - socket Id
  @param NumOfCha          - number of cha
  @param *Data32_AD        - pointer to ad data
  @param *Data32_BL        - pointer to bl data

  @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
CalculateChaToM2mCredit(
  struct  sysHost            *host,
  UINT8   Soc,
  UINT8   NumOfCha,
  UINT32  *Data32_AD,
  UINT32  *Data32_BL
  )
{
  UINT8   port;
  UINT32  AdCreditsToM2mPerCha, BlCreditsToM2mPerCha;
  *Data32_AD = 0;
  *Data32_BL = 0;

  //
  // calculate CHA->M2M
  //
  BlCreditsToM2mPerCha = AdCreditsToM2mPerCha =  TOTAL_M2M_AD_BL_CREDIT / (NumOfCha);

  //
  // (only 4 bits for this field , max to 15)
  //
  if( AdCreditsToM2mPerCha > 15) {
    BlCreditsToM2mPerCha = AdCreditsToM2mPerCha = 15;
  }


  for(port = 0; port < 2; port++) {       //for mc0 and mc 1 only
     *Data32_AD |=  (AdCreditsToM2mPerCha << ( port * 4));
     *Data32_BL |=  (BlCreditsToM2mPerCha << ( port * 4));
  }

  //
  // W/A: 5331750: TOR TO with LLC victim caused 2-socket system hang when running max local read BW test
  //
  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
    if(host->var.kti.CpuInfo[Soc].TotCha > 16){
      *Data32_BL = 0x00000011;
    }
  }


  return KTI_SUCCESS;
}

/**

  Program CHA->M2M credits when SNC enabled

  @param  MemInfo          - Memory Information structure.
  @param  SocId            - Socket ID

  @retval KTI_SUCCESS - on successful completion

  Here is the example of 10 CHA setting:                                          cbo_0-4                                        cbo_5-9
                                                                                  bl_vna_port0 (for MC0) bl_vna_port1(for MC1)   bl_vna_port0 (for MC0) bl_vna_port1 (for MC1)
1. SNC Disable                                                                           5                          5                      5                    5
2. SNC Enabled/2 clusters(both MC have memory, NUMA with 1_WAY interleave)               9                          2                      2                    9
3. SNC Enabled/1 clusters(both MC have memory, NUMA with 2_WAY interleave)               5                          5                      5                    5
4. SNC Enabled/1 cluster (only Mc0 has memory)                                           5                          5                      5                    5
5. SNC Enabled/1 cluster (only Mc1 has memory)                                           5                          5                      5                    5

**/

KTI_STATUS
ReCalAndProgramChaToM2mCreditWhenSncEn(
  struct sysHost *host,
  MEM_INFO       *MemInfo,
  UINT8          SocId
)
{
  UINT8   Cha;
  UINT32  NonChusterChaCredit, ClusterChaCredit, Data32, Data32_reverse;
  HA_AD_CREDITS_CHA_MISC_STRUCT   HaAdCredit;
  HA_BL_CREDITS_CHA_MISC_STRUCT   HaBlCredit;

  NonChusterChaCredit = ClusterChaCredit = 0;
  //
  // calculate CHA->M2M
  //
  if( MemInfo->SncInfo[SocId].NumOfCluster == 2) {            // both MC have memory, NUMA with 1_WAY interleave
    NonChusterChaCredit =  (TOTAL_M2M_AD_BL_CREDIT /MemInfo->SncInfo[SocId].NumOfChaPerCluster) / 4;
    ClusterChaCredit =   (TOTAL_M2M_AD_BL_CREDIT /MemInfo->SncInfo[SocId].NumOfChaPerCluster) - NonChusterChaCredit;
  } else {
    return KTI_SUCCESS;
  }

  if( NonChusterChaCredit > 15) NonChusterChaCredit = 15;
  if( ClusterChaCredit > 15) ClusterChaCredit = 15;


  Data32 = (NonChusterChaCredit << 4) | ClusterChaCredit;
  Data32_reverse = (ClusterChaCredit << 4) | NonChusterChaCredit;

  // program into registers
  for (Cha = 0; Cha < host->var.kti.CpuInfo[SocId].TotCha; Cha++) {
    HaAdCredit.Data = KtiReadPciCfg (host, SocId, Cha, HA_AD_CREDITS_CHA_MISC_REG);
    HaAdCredit.Data &= 0xFFFFFF00;

    HaBlCredit.Data = KtiReadPciCfg (host, SocId, Cha, HA_BL_CREDITS_CHA_MISC_REG);
    HaBlCredit.Data &= 0xFFFFFF00;

    if (Cha < MemInfo->SncInfo[SocId].NumOfChaPerCluster) {

       //  1 cluster (only Mc1 has memory)
      if ( MemInfo->McInfo[SocId][0].Enabled == 0 && MemInfo->McInfo[SocId][1].Enabled == 1) {
         HaAdCredit.Data |= Data32_reverse;
         HaBlCredit.Data |= Data32_reverse;
      } else {
         HaAdCredit.Data |= Data32;
         HaBlCredit.Data |= Data32;
      }
    } else {
      HaAdCredit.Data |= Data32_reverse;
      HaBlCredit.Data |= Data32_reverse;
    }

    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "  Cha%d credit %x\n", Cha, HaAdCredit.Data));

    KtiWritePciCfg (host, SocId, Cha, HA_AD_CREDITS_CHA_MISC_REG, HaAdCredit.Data);

    //
    // W/A: 5331750: TOR TO with LLC victim caused 2-socket system hang when running max local read BW test
    //
    if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
      if(host->var.kti.CpuInfo[SocId].TotCha > 16){
        HaBlCredit.Bits.bl_vna_port0 = 1;
        HaBlCredit.Bits.bl_vna_port1 = 1;
      }
    }
    KtiWritePciCfg (host, SocId, Cha, HA_BL_CREDITS_CHA_MISC_REG, HaBlCredit.Data);
  }

  return KTI_SUCCESS;
}

/**
    Count the number of active links

    @param host              - pointer to the system host root structure

    @retval number of active links

**/
UINT8
CountActiveKtiLink (
  struct sysHost      *host,
  UINT8               Soc
 )
{
  UINT8 NumofActiveKtiLink, Port;

  if (host->var.kti.OutKtiCpuSktHotPlugEn == FALSE) {
    NumofActiveKtiLink = 0;
    for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
      if (host->var.kti.CpuInfo[Soc].LepInfo[Port].Valid == TRUE){
          NumofActiveKtiLink ++;
       }
    }
  } else {     //if hotplug enabled, need to reserved all credit for even unused credit
    NumofActiveKtiLink = host->var.common.KtiPortCnt;
  }

  return  NumofActiveKtiLink;
}

/**

    Program CHA credits

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param Soc               - Socket ID

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramChaCreditSetting (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Soc
  )
{
  AD_VNA_CREDIT_CONFIG_N0_CHA_PMA_STRUCT   AdVnaCreditConfig0;
  AD_VNA_CREDIT_CONFIG_N1_CHA_PMA_STRUCT   AdVnaCreditConfig1;
  BL_VNA_CREDIT_CONFIG_N0_CHA_PMA_STRUCT   BlVnaCreditConfig0;
  BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_STRUCT   BlVnaCreditConfig1;
  HA_AD_CREDITS_CHA_MISC_STRUCT            HaAdCredit;
  HA_BL_CREDITS_CHA_MISC_STRUCT            HaBlCredit;

  UINT8   Cha, NumofActiveKtiLink;
  UINT32  RequiredCredit, AdVnaCrditPerCha, BlVnaCrditPerCha;
  UINT32  BlCreditsToM2pciePerCha, Data32_AD, Data32_BL, RemainingBitMap, AddOneFlag;
  UINT8   RemainingCredit, NumOfCha, TotalIioStackToM3KtiAdVna, Src;

  AdVnaCrditPerCha = 0;
  BlVnaCrditPerCha = 0;
  RequiredCredit = 0;
  TotalIioStackToM3KtiAdVna = 0;
  AddOneFlag = 0;

  NumOfCha = KtiInternalGlobal->TotCha[Soc];

  // Force to min of 2CHA if detect 1CHA
  if( NumOfCha < 2) {
    NumOfCha  = 2;
  }

  NumofActiveKtiLink = CountActiveKtiLink(host, Soc);

  if ((host->var.kti.SysConfig != SYS_CONFIG_1S) || (KtiInternalGlobal->ProgramCreditFpga != 0)) {
    //
    // Calculate the AD VNA credit from CHA-> KTI if the topology is route through
    //
    // Total_credit >=  (#of CHA * AdVnaCrditPerCha) + (active_number_link - 1) ( KTI->KTI VNA + VN0 + VN1 if needed ) if route through +  CHA-KTI VN0  + TotalIioStackToM3KtiAdVna

    if (KtiInternalGlobal->IsRouteThrough == TRUE) {
     RequiredCredit = KTI_TO_KTI_VN0_AD_INGRESS;                       // VN0
     if (host->var.kti.OutVn1En == TRUE) {
        RequiredCredit = RequiredCredit + KTI_TO_KTI_VN1_AD_INGRESS;   // VN1
     }
     if(host->var.kti.SysConfig == SYS_CONFIG_8S || NumofActiveKtiLink == 3) {             //8S or 4S with route through (like 4S star)
       RequiredCredit = RequiredCredit + MIN_K2K_AD_VNA;
     } else {                                                         //4S, route-through (like 4s ring)
       RequiredCredit = RequiredCredit + TOTAL_K2K_AD_VNA;
     }
     RequiredCredit = RequiredCredit * (NumofActiveKtiLink - 1);
    }

    // sum of all IIO -> KTI0 credit
    for (Src = 0; Src < MAX_SKX_M2PCIE; Src ++){
      if(KtiInternalGlobal->M2PcieActiveBitmap[Soc] & (1 << Src)) {
         TotalIioStackToM3KtiAdVna = TotalIioStackToM3KtiAdVna + IioToKti0_Vn0AdCreditTable[Src];
      }
    }

    RequiredCredit = RequiredCredit + CHA_TO_KTI_VN0_AD_INGRESS + TotalIioStackToM3KtiAdVna;        // (CHA->KTI VN0) + (IIO stack -> M3KTI AD VN0)
    KTI_ASSERT(TOTAL_KTI_AD_CREDIT > RequiredCredit, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_2);
    AdVnaCrditPerCha = (TOTAL_KTI_AD_CREDIT - RequiredCredit) / NumOfCha;
    if( AdVnaCrditPerCha > 15) AdVnaCrditPerCha = 15;                   // because 4 bits only in this field, avoid overflow

    //
    // Calculate the BL VNA credit from CHA-> KTI based on number of CHAs
    //
    if (NumOfCha <= 14) {
      BlVnaCrditPerCha = 2;
    } else {
      BlVnaCrditPerCha = 1;
    }

  }  //if not SYS_CONFIG_1S

  //
  // calculate CHA-> M2m AD & BL Ingress (only 4 bits per , max to 15)
  // and later it will be re-calculate if SNC enable with 2 clusters after MRC in cold reset path
  //
  CalculateChaToM2mCredit(host, Soc, NumOfCha, &Data32_AD, &Data32_BL);

  //
  // calculate CHA-> M2PCIE BL VNA
  //
  BlCreditsToM2pciePerCha = (MAX_CHA_TO_M2PCIE_BL_VNA / NumOfCha);
  //get the remaining credit available & bitmap and will be assigned to the cha which is far from M2PCIE
  RemainingCredit = (MAX_CHA_TO_M2PCIE_BL_VNA % NumOfCha);
  if (NumOfCha == 22) {                           //get remaining bit map to tell which cha need to be used
    RemainingBitMap = 0x244444;                   //Cha 2, 6, 10, 14, 18, 21 for 22 CHA
  } else if (NumOfCha == 18) {
    RemainingBitMap = 0x3718e;                    //Cha 1, 2, 3, 7, 8, 12, 13, 14, 16, 17 for 18 CHA
  } else if (NumOfCha == 10) {
    RemainingBitMap = 0x3db;                      //Cha 0, 1, 3, 4, 6, 7, 8, 9 for 10 CHA
  } else {
    RemainingBitMap = 0;
  }

  //
  // Program VNA CREDIT CONFIG related registers
  //
  for (Cha = 0; Cha < KtiInternalGlobal->TotCha[Soc]; Cha++) {

    if (RemainingCredit) {                            //check if still have credit available
      AddOneFlag = ((RemainingBitMap & (1 << Cha)) >> Cha);
      if (AddOneFlag) {                               //check if this cha has extra credit
        RemainingCredit -= 1;
        BlCreditsToM2pciePerCha += 1;
      }
    }

    // overflow checking, 4-bit field -> max value is 15
    if (BlCreditsToM2pciePerCha > 15) {
      BlCreditsToM2pciePerCha = 15;
    }


    //Program AD VNA credit N1 and BL VNA credit N1
    AdVnaCreditConfig1.Data = KtiReadPciCfg (host, Soc, Cha, AD_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG);
    BlVnaCreditConfig1.Data = KtiReadPciCfg (host, Soc, Cha, BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG);

    //
    // Set disisocvnres field to disable reserving credits for isoch
    // If isoch is enabled, this will be reversed after WR
    //
    AdVnaCreditConfig1.Bits.disisocvnres = 1;
    BlVnaCreditConfig1.Bits.disisocvnres = 1;

    BlVnaCreditConfig1.Bits.bl_vna_port8 = BlCreditsToM2pciePerCha;
    KtiWritePciCfg (host, Soc, Cha, AD_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG, AdVnaCreditConfig1.Data);
    KtiWritePciCfg (host, Soc, Cha, BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG, BlVnaCreditConfig1.Data);

    AdVnaCreditConfig0.Data = KtiReadPciCfg (host, Soc, Cha, AD_VNA_CREDIT_CONFIG_N0_CHA_PMA_REG);
    //s5331153, s5370315
    //SKX  port2 -> kti0, port 3-> kti1, port 4 -> kti2 (check later for kti)
    AdVnaCreditConfig0.Bits.ad_vna_port2 = AdVnaCrditPerCha;
    AdVnaCreditConfig0.Bits.ad_vna_port3 = AdVnaCrditPerCha;
    AdVnaCreditConfig0.Bits.ad_vna_port4 = AdVnaCrditPerCha;
    KtiWritePciCfg (host, Soc, Cha, AD_VNA_CREDIT_CONFIG_N0_CHA_PMA_REG, AdVnaCreditConfig0.Data);
    BlVnaCreditConfig0.Data = KtiReadPciCfg (host, Soc, Cha, BL_VNA_CREDIT_CONFIG_N0_CHA_PMA_REG);
    BlVnaCreditConfig0.Bits.bl_vna_port3 = BlVnaCrditPerCha;
    BlVnaCreditConfig0.Bits.bl_vna_port4 = BlVnaCrditPerCha;
    BlVnaCreditConfig0.Bits.bl_vna_port5 = BlVnaCrditPerCha;
    BlVnaCreditConfig0.Bits.bl_vna_port0 = BlCreditsToM2pciePerCha;
    BlVnaCreditConfig0.Bits.bl_vna_port1 = BlCreditsToM2pciePerCha;
    BlVnaCreditConfig0.Bits.bl_vna_port2 = BlCreditsToM2pciePerCha;
    BlVnaCreditConfig0.Bits.bl_vna_port7 = BlCreditsToM2pciePerCha;
    KtiWritePciCfg (host, Soc, Cha, BL_VNA_CREDIT_CONFIG_N0_CHA_PMA_REG, BlVnaCreditConfig0.Data);

    //
    //SKX CHA->M2MEM AD OR BL credit
    //
    // CHA-> M2ME AD portion
    //
    HaAdCredit.Data =  Data32_AD;
    //
    // CHA-> M2ME BL portion
    //
    HaBlCredit.Data =  Data32_BL;

    //
    // Set disadvnaisocres and disblvnaisocres fields to 1 to disable reserving credits for isoch
    // If isoch is enabled, this will be reversed after WR
    //
    HaAdCredit.Bits.disadvnaisocres = 1;
    HaBlCredit.Bits.disblvnaisocres = 1;

    KtiWritePciCfg (host, Soc, Cha, HA_AD_CREDITS_CHA_MISC_REG, HaAdCredit.Data);
    KtiWritePciCfg (host, Soc, Cha, HA_BL_CREDITS_CHA_MISC_REG, HaBlCredit.Data);

    //
    // Once allocated one extra credit, need to decrement the BlCreditsToM2pciePerCha
    // back to initial value, such that each extra credit we allocate is not added to
    // every next CHA that we program
    //
    if (AddOneFlag) {
      BlCreditsToM2pciePerCha -= 1;
    }
  }

  return KTI_SUCCESS;
}


/*++

    Mask R2EGRPRQERRMSK0_M2UPCIE_MAIN_REG when program M2PCIE credits
    Unmaks after warm reset

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to thepointer to socket data structure
    @param Mask              - Mask or Unmask the R2EGRPRQERRMSK0_M2UPCIE_MAIN_REG

--*/
VOID
MCAErrWa(
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  BOOLEAN             Mask
)
{
  UINT8 Src, Idx1, Soc;

  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    if ((SocketData->Cpu[Soc].Valid == TRUE) && (SocketData->Cpu[Soc].SocType == SOCKET_TYPE_CPU)) {
      for (Idx1 = MAX_SKX_M2PCIE; Idx1 != 0; Idx1--) {
        Src = Idx1-1;
        if (host->var.common.M2PciePresentBitmap[Soc] & (1 << Src)) {
          if (Mask) {
            KtiWritePciCfg (host, Soc, Src, R2EGRPRQERRMSK0_M2UPCIE_MAIN_REG, 0xFFFFFFFF);
          } else {
            KtiWritePciCfg (host, Soc, Src, R2EGRPRQERRMSK0_M2UPCIE_MAIN_REG, 0);
          }
        }
      }
    }
  }
}


/**

    Program M2PCIE credits.

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param Soc               - Socket ID

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramM2PcieCreditSetting (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Soc
  )
{
  UINT8   Idx1, Src, Dest, Port, PrqPerIoStack, RemainingPrq, Cha, IioToIioBlCredit, DestStartBit, NumofActiveKtiLink, SubtractOneForAStepping;
  UINT32  Vn0AdCredit, BlNcsCredit;
  UINT32  VN0_AD_Data32, BL_NCS_Data32, Credit, Credit0, Credit1, Credit2, Credit3;
  R2DEFEATURE_M2UPCIE_MAIN_STRUCT      R2Defeature;

  RemainingPrq = TOTAL_PRQ % (KtiInternalGlobal->TotActiveM2Pcie[Soc] + 1);    // add 1 because M2PCIE1 is shared by CSTACK and PSTACK1

  NumofActiveKtiLink = CountActiveKtiLink(host, Soc);
  SubtractOneForAStepping = 0;

  //Handle a0 stepping workaround
  // all cases except the following case, need to subtract 1
  // if number of M2CPIE <=3,and not 8S, number of CHA less than or equal to 18, don't need subtract 1

  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
    SubtractOneForAStepping = 1;
    if ((KtiInternalGlobal->TotActiveM2Pcie[Soc] <= 3) && (host->var.kti.SysConfig != SYS_CONFIG_8S) && (KtiInternalGlobal->TotCha[Soc] <= 18)){
       SubtractOneForAStepping = 0;
    }
  }

  for (Idx1 = MAX_SKX_M2PCIE; Idx1 != 0; Idx1--){                          //start from far distance M2PCIE first
    Src = Idx1-1;                                                         //get source M2Pcie number
    if (KtiInternalGlobal->M2PcieActiveBitmap[Soc] & (1 << Src)) {

      PrqPerIoStack =  TOTAL_PRQ / (KtiInternalGlobal->TotActiveM2Pcie[Soc] + 1);   //distubute equally to all existing M2Pcie

      VN0_AD_Data32 = BL_NCS_Data32 = 0;

      for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
        Vn0AdCredit = BlNcsCredit = 0;
        if ((host->var.kti.CpuInfo[Soc].LepInfo[Port].Valid == TRUE) || (host->var.kti.OutKtiCpuSktHotPlugEn == TRUE)){
          if (Port != 2){                                                           //KTI 0 or 1
             Vn0AdCredit = IioToKti0_Vn0AdCreditTable[Src];                         //  IIO -> KTI VN0 AD Ingress
             if( KtiInternalGlobal->TotActiveM2Pcie[Soc] <= 3){                        //  IIO stack -> KTI BL NCS
               if ((host->var.kti.SysConfig == SYS_CONFIG_8S)){
                 BlNcsCredit = IioToKti0_BlCreditTable_For3M2PcieAnd8Socket[Src];
               } else {
                 BlNcsCredit = IioToKti0_BlCreditTable_For3M2Pcie[Src];
               }
             } else {
                BlNcsCredit = IioToKti0_BlCreditTable_For4M2Pcie[Src];
             }
          } else {                                                                  //KTI 2
             Vn0AdCredit = IioToKti2_Vn0AdCreditTable[Src];                         //  IIO -> KTI VN0 AD Ingress
             if( KtiInternalGlobal->TotActiveM2Pcie[Soc] <= 3){                        //  IIO stack -> KTI BL NCS
               if ((host->var.kti.SysConfig == SYS_CONFIG_8S)){
                 BlNcsCredit = IioToKti2_BlCreditTable_For3M2PcieAnd8Socket[Src];
               } else {
                 BlNcsCredit = IioToKti2_BlCreditTable_For3M2Pcie[Src];
               }
             } else {
                BlNcsCredit = IioToKti2_BlCreditTable_For4M2Pcie[Src];
             }
          }

          //This is NCB/NCS/DRS shared register
          // A0-bug work-around, so we need to subtract another 1 credit
          // For A0- please program *SS value minus 2*, for B0- please program *SS value minus 1*
          BlNcsCredit = BlNcsCredit - 1;
          if (SubtractOneForAStepping) {
            BlNcsCredit = BlNcsCredit - 1;
          }
        }  // link valid or hotplug enabled

        VN0_AD_Data32 = VN0_AD_Data32 | (Vn0AdCredit << (4 * Port));
        BL_NCS_Data32 = BL_NCS_Data32 | (BlNcsCredit << (4 * Port));
      }

      KtiWritePciCfg (host, Soc, Src, R2NDRTOKTIINITCRED_M2UPCIE_MAIN_REG, VN0_AD_Data32);
      KtiWritePciCfg (host, Soc, Src, R2NCSTOKTIINITCRED_M2UPCIE_MAIN_REG, BL_NCS_Data32);

      //IIO-> IIO stack credit setting
      Credit = 0;                                                        //initialize with 0
      for (Dest = 0; Dest < MAX_SKX_M2PCIE; Dest ++){
        DestStartBit = DestStartBitTable[Dest];                          //get dest start bit
        if (KtiInternalGlobal->M2PcieActiveBitmap[Soc] & (1 << Dest)) {
           Credit = Credit & ~( 0xF << DestStartBit );
           if (KtiInternalGlobal->TotActiveM2Pcie[Soc] <= 3){                //3 M2pcie case
               if (NumofActiveKtiLink == 3 && host->var.kti.SysConfig >= SYS_CONFIG_4S){
                   IioToIioBlCredit = IioToIioBlCreditTableFor3M2Pcie_FullConnected[Src][Dest];
               } else {
             IioToIioBlCredit = IioToIioBlCreditTableFor3M2Pcie[Src][Dest];
               }
           } else {                                                       //4 M2pcie case
              if ((NumofActiveKtiLink == 0)) {                              //  1S
                IioToIioBlCredit = IioToIioBlCreditTableFor4M2Pcie0Kti[Src][Dest];
              } else if (NumofActiveKtiLink <= 2) {                         //  dual links
                IioToIioBlCredit = IioToIioBlCreditTableFor4M2Pcie2Kti[Src][Dest];
              } else {                                                      //  3 kti links
                IioToIioBlCredit = IioToIioBlCreditTableFor4M2Pcie3Kti[Src][Dest];
              }
           }
           Credit = Credit | (IioToIioBlCredit <<  DestStartBit );
           if (Dest == 0){                                 //check if Dest is PSTACK1
              Credit = Credit & 0xFFFFFFF0;                //program CSTACK fields with the value of PSTACK1
              Credit = Credit | IioToIioBlCredit;
           }
        }
      }

      KtiWritePciCfg (host, Soc, Src, R2NCSTOIIOINITCRED_M2UPCIE_MAIN_REG, Credit);
      KtiWritePciCfg (host, Soc, Src, R2NCBTOIIOINITCRED_M2UPCIE_MAIN_REG, Credit);

      //M2PCIE-> CHA PRQ credit setting
      Credit0 = Credit1 = Credit2 = Credit3 = 0;
      if (RemainingPrq) {            //if still has remaining PRQ left, then assign one to current M2Pcie
        PrqPerIoStack++;
        RemainingPrq--;
      }

      for (Cha = 0; Cha < KtiInternalGlobal->TotCha[Soc]; Cha ++){

          if (Cha < 8){
            Credit0 = Credit0 | (PrqPerIoStack << (4 *(Cha % 8)));
          } else if (Cha < 16){
            Credit1 = Credit1 | (PrqPerIoStack << (4 *(Cha % 8)));
          } else if (Cha < 24){
            Credit2 = Credit2 | (PrqPerIoStack << (4 *(Cha % 8)));
          } else {
            Credit3 = Credit3 | (PrqPerIoStack << (4 *(Cha % 8)));
          }
        }

      KtiWritePciCfg (host, Soc, Src, R2PRQINITCRED0_M2UPCIE_MAIN_REG, Credit0);         //for CHA 0-7
      KtiWritePciCfg (host, Soc, Src, R2PRQINITCRED1_M2UPCIE_MAIN_REG, Credit1);         //for CHA 8-15
      KtiWritePciCfg (host, Soc, Src, R2PRQINITCRED2_M2UPCIE_MAIN_REG, Credit2);         //for CHA 16-23
      KtiWritePciCfg (host, Soc, Src, R2PRQINITCRED3_M2UPCIE_MAIN_REG, Credit3);         //for CHA 24-27

      KtiWritePciCfg (host, Soc, Src, R2PISOINITCRDT0_M2UPCIE_MAIN_REG, 0);
      KtiWritePciCfg (host, Soc, Src, R2PISOINITCRDT1_M2UPCIE_MAIN_REG, 0);
      KtiWritePciCfg (host, Soc, Src, R2PISOINITCRDT2_M2UPCIE_MAIN_REG, 0);


      // IIO to UBOX credit setting
      R2Defeature.Data = KtiReadPciCfg (host, Soc, Src, R2DEFEATURE_M2UPCIE_MAIN_REG);
      R2Defeature.Bits.vn0ncbiio2ubxinitialcred = 1;
      KtiWritePciCfg (host, Soc, Src, R2DEFEATURE_M2UPCIE_MAIN_REG, R2Defeature.Data);

    }  //  M2PcieActiveBitmap valid??
  }  //for loop

  return KTI_SUCCESS;
}

/**

    Program M3KTI credits

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param Soc               - Socket ID

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramM3KtiCreditSetting (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Soc
  )
{
  UINT8    M3Kti, Ctr, NumofActiveKtiLink, IioToIioBlCredit, Src, TotalBlCredit, KtiBlArray[MAX_KTI_M2PCIE_ENTRY];
  UINT8    *KtiToM2Pcie, Index;
  UINT32   Data32;
  M3KPCIR0CR_M3KTI_MAIN_STRUCT  M3KPciRingCrdtCtrl_Port0, M3KPciRingCrdtCtrl_Port1,M3KPciRingCrdtCtrl_Port2;
  M3KEGRCTRL0_M3KTI_MAIN_STRUCT M3KErgCtrl;
  M3KRTE0CR_M3KTI_MAIN_STRUCT   M3KRtEgressCrdtCtrl;
  M3KRTE0CR_M3KTI_MAIN_STRUCT   M3KRtEgressCrdtCtrlTemp;

  M3KPciRingCrdtCtrl_Port0.Data = M3KPciRingCrdtCtrl_Port1.Data = M3KPciRingCrdtCtrl_Port2.Data = 0;
  M3KRtEgressCrdtCtrl.Data = 0;

  NumofActiveKtiLink = CountActiveKtiLink(host, Soc);

  for (Ctr = 0; Ctr < MAX_SKX_M2PCIE; Ctr ++){
    // check if this M2PCIE exists or IIO0 or UBOX
    if (KtiInternalGlobal->M2PcieActiveBitmap[Soc] & (1 << Ctr) || (Ctr == 0) || (Ctr == 1)) {
       //find which table is for this configuration
       if ((KtiInternalGlobal->TotActiveM2Pcie[Soc] == 3) && (host->var.kti.SysConfig == SYS_CONFIG_2S3L)) {
         KtiToM2Pcie = &KtiToM2pcieCreditTableFor2S_3M2Pcie_3Kti[0][0];
       } else if (KtiInternalGlobal->TotActiveM2Pcie[Soc] > 3 && NumofActiveKtiLink == 3) {
         KtiToM2Pcie = &KtiToM2pcieCreditTableFor4M2Pcie_3KTI[0][0];
       } else {
         KtiToM2Pcie = &KtiToM2pcieCreditTable[0][0];
       }

       //get credit values for this dest M2PCIE (Ctr)
       for (Index = 0; Index < MAX_KTI_M2PCIE_ENTRY; Index++){
           KtiBlArray[Index] = *(KtiToM2Pcie + Index * MAX_SKX_M2PCIE + Ctr);
       }

       //check if need to do adjust for kti1 if total credit greater than 63
       for (Index = 0; Index < host->var.common.KtiPortCnt; Index++) {
           if ((host->var.kti.CpuInfo[Soc].LepInfo[Index].Valid == FALSE) && (host->var.kti.OutKtiCpuSktHotPlugEn == FALSE)){
               if (Index == 0){
                  KtiBlArray[KTI0_NCS_INDEX] = KtiBlArray[KTI0_NCB_INDEX] = 0;
               } else if (Index == 1){
                  KtiBlArray[KTI1_NCS_INDEX] = KtiBlArray[KTI1_NCB_INDEX] = 0;
              } else {
                  KtiBlArray[KTI2_NCS_INDEX] = KtiBlArray[KTI2_NCB_INDEX] = 0;
              }
           }
       }

       //if silicon only 2 kti port, clear port2 portion to 0
       if (host->var.common.KtiPortCnt == 2){
          KtiBlArray[KTI2_NCS_INDEX] = KtiBlArray[KTI2_NCB_INDEX] = 0;
       }

       TotalBlCredit = MAX_CHA_TO_M2PCIE_BL_VNA + 4;

       for (Src = 0; Src < MAX_SKX_M2PCIE; Src++){
           if (KtiInternalGlobal->M2PcieActiveBitmap[Soc] & (1 << Src)) {
               if (KtiInternalGlobal->TotActiveM2Pcie[Soc] <= 3){                //3 M2pcie case
                   if (NumofActiveKtiLink == 3 && host->var.kti.SysConfig >= SYS_CONFIG_4S){
                       IioToIioBlCredit = IioToIioBlCreditTableFor3M2Pcie_FullConnected[Src][Ctr];
                   } else {
                       IioToIioBlCredit = IioToIioBlCreditTableFor3M2Pcie[Src][Ctr];
                   }
               } else {                                                       //4 M2pcie case
                   if ((NumofActiveKtiLink == 0)) {                              //  1S
                       IioToIioBlCredit = IioToIioBlCreditTableFor4M2Pcie0Kti[Src][Ctr];
                   } else if (NumofActiveKtiLink <= 2) {                         //  dual links
                       IioToIioBlCredit = IioToIioBlCreditTableFor4M2Pcie2Kti[Src][Ctr];
                   } else {                                                      //  3 kti links
                       IioToIioBlCredit = IioToIioBlCreditTableFor4M2Pcie3Kti[Src][Ctr];
                   }
               }
               if (IioToIioBlCredit != 0){
                 // because value in table is actual credit - 1, we need to add 1 to get actual value
                 TotalBlCredit = TotalBlCredit + IioToIioBlCredit + 1;
               }
           }
       }

       //add all Kti-> M2CPIE credit
       for (Index = 0; Index < MAX_KTI_M2PCIE_ENTRY; Index++){
           TotalBlCredit = TotalBlCredit + KtiBlArray[Index];
       }
       //if dest is Pstack2 (Ctr = 1), then add IIO to ubox credit
       if (Ctr ==  1){
           TotalBlCredit = TotalBlCredit + KtiInternalGlobal->TotActiveM2Pcie[Soc];
       }
       KTI_ASSERT(TOTAL_KTI_BL_CREDIT >= TotalBlCredit, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_54);
       //s5331454 Cloned From SKX Si Bug Eco: M2PCIE BL Ingress Allocation of Last Free Entry Causes Pointer Corruption
       //         check if need to do adjust for kti1 if total credit is 64
       if (TotalBlCredit == TOTAL_KTI_BL_CREDIT){
           // check kti 1 first, and then kti 2, kti 0
           if (KtiBlArray[KTI1_NCS_INDEX] >= 2) {
              KtiBlArray[KTI1_NCS_INDEX] = KtiBlArray[KTI1_NCS_INDEX] - 1;
           } else if (KtiBlArray[KTI2_NCS_INDEX] >= 2) {
              KtiBlArray[KTI2_NCS_INDEX] = KtiBlArray[KTI2_NCS_INDEX] - 1;
           } else if (KtiBlArray[KTI0_NCS_INDEX] >= 2) {
              KtiBlArray[KTI0_NCS_INDEX] = KtiBlArray[KTI0_NCS_INDEX] - 1;
           }
       }


       switch (Ctr){
       case 0:
         M3KPciRingCrdtCtrl_Port0.Bits.iio0_iio1_ncb = KtiBlArray[KTI0_NCB_INDEX];       //Kti0   -> M2PCIE1 NCB
         M3KPciRingCrdtCtrl_Port0.Bits.iio0_iio1_ncs = KtiBlArray[KTI0_NCS_INDEX];       //Kti0   -> M2PCIE1 NCS
         M3KPciRingCrdtCtrl_Port1.Bits.iio0_iio1_ncb = KtiBlArray[KTI1_NCB_INDEX];       //Kti1   -> M2PCIE1 NCB
         M3KPciRingCrdtCtrl_Port1.Bits.iio0_iio1_ncs = KtiBlArray[KTI1_NCS_INDEX];       //Kti1   -> M2PCIE1 NCS
         M3KPciRingCrdtCtrl_Port2.Bits.iio0_iio1_ncb = KtiBlArray[KTI2_NCB_INDEX];       //Kti2   -> M2PCIE1 NCB
         M3KPciRingCrdtCtrl_Port2.Bits.iio0_iio1_ncs = KtiBlArray[KTI2_NCS_INDEX];       //Kti2   -> M2PCIE1 NCS
         break;
       case 1:
         M3KPciRingCrdtCtrl_Port0.Bits.iio2_ubox_ncb = KtiBlArray[KTI0_NCB_INDEX];       //Kti0   -> M2PCIE2 NCB
         M3KPciRingCrdtCtrl_Port0.Bits.iio2_ubox_ncs = KtiBlArray[KTI0_NCS_INDEX];       //Kti0   -> M2PCIE2 NCS
         M3KPciRingCrdtCtrl_Port1.Bits.iio2_ubox_ncb = KtiBlArray[KTI1_NCB_INDEX];       //Kti1   -> M2PCIE2 NCB
         M3KPciRingCrdtCtrl_Port1.Bits.iio2_ubox_ncs = KtiBlArray[KTI1_NCS_INDEX];       //Kti1   -> M2PCIE2 NCS
         M3KPciRingCrdtCtrl_Port2.Bits.iio2_ubox_ncb = KtiBlArray[KTI2_NCB_INDEX];       //Kti2   -> M2PCIE2 NCB
         M3KPciRingCrdtCtrl_Port2.Bits.iio2_ubox_ncs = KtiBlArray[KTI2_NCS_INDEX];       //Kti2   -> M2PCIE2 NCS
         break;
       case 2:
         M3KPciRingCrdtCtrl_Port0.Bits.iio3_ncb = KtiBlArray[KTI0_NCB_INDEX];            //Kti0   -> M2PCIE3 NCB
         M3KPciRingCrdtCtrl_Port0.Bits.iio3_ncs = KtiBlArray[KTI0_NCS_INDEX];            //Kti0   -> M2PCIE3 NCS
         M3KPciRingCrdtCtrl_Port1.Bits.iio3_ncb = KtiBlArray[KTI1_NCB_INDEX];            //Kti1   -> M2PCIE3 NCB
         M3KPciRingCrdtCtrl_Port1.Bits.iio3_ncs = KtiBlArray[KTI1_NCS_INDEX];            //Kti1   -> M2PCIE3 NCS
         M3KPciRingCrdtCtrl_Port2.Bits.iio3_ncb = KtiBlArray[KTI2_NCB_INDEX];            //Kti2   -> M2PCIE3 NCB
         M3KPciRingCrdtCtrl_Port2.Bits.iio3_ncs = KtiBlArray[KTI2_NCS_INDEX];            //Kti2   -> M2PCIE3 NCS
         break;
       case 3:
         M3KPciRingCrdtCtrl_Port0.Bits.iio4_ncb = KtiBlArray[KTI0_NCB_INDEX];            //Kti0   -> M2PCIE4 NCB
         M3KPciRingCrdtCtrl_Port0.Bits.iio4_ncs = KtiBlArray[KTI0_NCS_INDEX];            //Kti0   -> M2PCIE4 NCS
         M3KPciRingCrdtCtrl_Port1.Bits.iio4_ncb = KtiBlArray[KTI1_NCB_INDEX];            //Kti1   -> M2PCIE4 NCB
         M3KPciRingCrdtCtrl_Port1.Bits.iio4_ncs = KtiBlArray[KTI1_NCS_INDEX];            //Kti1   -> M2PCIE4 NCS
         M3KPciRingCrdtCtrl_Port2.Bits.iio4_ncb = KtiBlArray[KTI2_NCB_INDEX];            //Kti2   -> M2PCIE4 NCB
         M3KPciRingCrdtCtrl_Port2.Bits.iio4_ncs = KtiBlArray[KTI2_NCS_INDEX];            //Kti2   -> M2PCIE4 NCS
         break;
       case 4:
         M3KPciRingCrdtCtrl_Port0.Bits.iio5_ncb = KtiBlArray[KTI0_NCB_INDEX];            //Kti0   -> M2PCIE5 NCB
         M3KPciRingCrdtCtrl_Port0.Bits.iio5_ncs = KtiBlArray[KTI0_NCS_INDEX];            //Kti0   -> M2PCIE5 NCS
         M3KPciRingCrdtCtrl_Port1.Bits.iio5_ncb = KtiBlArray[KTI1_NCB_INDEX];            //Kti1   -> M2PCIE5 NCB
         M3KPciRingCrdtCtrl_Port1.Bits.iio5_ncs = KtiBlArray[KTI1_NCS_INDEX];            //Kti1   -> M2PCIE5 NCS
         M3KPciRingCrdtCtrl_Port2.Bits.iio5_ncb = KtiBlArray[KTI2_NCB_INDEX];            //Kti2   -> M2PCIE5 NCB
         M3KPciRingCrdtCtrl_Port2.Bits.iio5_ncs = KtiBlArray[KTI2_NCS_INDEX];            //Kti2   -> M2PCIE5 NCS
         break;
       default:
         break;
      }  //switch
    } // valid
  }

  //
  // M3KRTE0CR or M3KRTE1CR - used for route-through traffic
  // Configure KTI->KTI VNA AD Ingress & KTI->KTI  VNA BL Ingress
  //
  if (KtiInternalGlobal->IsRouteThrough == TRUE ) {
     if(host->var.kti.SysConfig == SYS_CONFIG_8S || NumofActiveKtiLink == 3) {             //8S or 4S with route through (like 4S star)
       M3KRtEgressCrdtCtrl.Bits.vna_rt_ad = MIN_K2K_AD_VNA;
       M3KRtEgressCrdtCtrl.Bits.vna_rt_bl = MIN_K2K_BL_VNA;
     } else {                                                   //4S, route-through (like 4s ring)
       M3KRtEgressCrdtCtrl.Bits.vna_rt_ad = TOTAL_K2K_AD_VNA;
       M3KRtEgressCrdtCtrl.Bits.vna_rt_bl = TOTAL_K2K_BL_VNA;
     }
  }

  //
  // KTI->M3KPCIR NCS/NCB credits
  //
  if (host->var.kti.CpuInfo[Soc].SocType == SOCKET_TYPE_CPU) {
    for (Ctr = 0; Ctr < host->var.common.KtiPortCnt; Ctr++) {
      if ((host->var.kti.CpuInfo[Soc].LepInfo[Ctr].Valid == TRUE) || (host->var.kti.OutKtiCpuSktHotPlugEn == TRUE)) {
        if (Ctr == 0){
          KtiWritePciCfg (host, Soc, 0, M3KPCIR0CR_M3KTI_MAIN_REG, M3KPciRingCrdtCtrl_Port0.Data);     // M3KTI 0 Port 0 (KTI port0)
        } else if (Ctr == 1){
          KtiWritePciCfg (host, Soc, 0, M3KPCIR1CR_M3KTI_MAIN_REG, M3KPciRingCrdtCtrl_Port1.Data);     // M3KTI 0 Port 1 (KTI port1)
        } else {
          KtiWritePciCfg (host, Soc, 1, M3KPCIR0CR_M3KTI_MAIN_REG, M3KPciRingCrdtCtrl_Port2.Data);        // M3KTI 1 Port 0 (KTI port2)
        }
      }
    }
  }
  for (M3Kti = 0; M3Kti < host->var.kti.CpuInfo[Soc].TotM3Kti; M3Kti++) {
    //
    // M3KEGRCTRL0/M3KEGRCTRL1 PRQ_VNA (KTI-> CHA)
    //
    if( NumofActiveKtiLink !=0 ){
      Data32  = (24 -  NumofActiveKtiLink *3) / NumofActiveKtiLink;
      M3KErgCtrl.Data = KtiReadPciCfg (host, Soc, M3Kti, M3KEGRCTRL0_M3KTI_MAIN_REG);
      M3KErgCtrl.Bits.rpq_vna = Data32;
      KtiWritePciCfg (host, Soc, M3Kti, M3KEGRCTRL0_M3KTI_MAIN_REG, M3KErgCtrl.Data);

      M3KErgCtrl.Data = KtiReadPciCfg (host, Soc, M3Kti, M3KEGRCTRL1_M3KTI_MAIN_REG);
      M3KErgCtrl.Bits.rpq_vna = Data32;
      KtiWritePciCfg (host, Soc, M3Kti, M3KEGRCTRL1_M3KTI_MAIN_REG, M3KErgCtrl.Data);
    }

    //
    // M3KRTE0CR/M3KRTE1CR - shared (AD/BL VNA) and VN0 credits. VN1 uses the same value as VN0.
    //
    M3KRtEgressCrdtCtrlTemp.Data = KtiReadPciCfg (host, Soc, M3Kti, M3KRTE0CR_M3KTI_MAIN_REG);
    M3KRtEgressCrdtCtrlTemp.Bits.vna_rt_ad = M3KRtEgressCrdtCtrl.Bits.vna_rt_ad;
    M3KRtEgressCrdtCtrlTemp.Bits.vna_rt_bl = M3KRtEgressCrdtCtrl.Bits.vna_rt_bl;
    KtiWritePciCfg (host, Soc, M3Kti, M3KRTE0CR_M3KTI_MAIN_REG, M3KRtEgressCrdtCtrlTemp.Data);

    M3KRtEgressCrdtCtrlTemp.Data = KtiReadPciCfg (host, Soc, M3Kti, M3KRTE1CR_M3KTI_MAIN_REG);
    M3KRtEgressCrdtCtrlTemp.Bits.vna_rt_ad = M3KRtEgressCrdtCtrl.Bits.vna_rt_ad;
    M3KRtEgressCrdtCtrlTemp.Bits.vna_rt_bl = M3KRtEgressCrdtCtrl.Bits.vna_rt_bl;
    KtiWritePciCfg (host, Soc, M3Kti, M3KRTE1CR_M3KTI_MAIN_REG, M3KRtEgressCrdtCtrlTemp.Data);

  } //M3Kti

  return KTI_SUCCESS;
}

/**

    Program KTI link credits

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param Soc               - Socket ID

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramKtiCreditSetting (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Soc
  )
{
  // KTI-> M3KTI
  KTIA2RCRCTRL_KTI_LLPMON_STRUCT       KtiA2RcrCtrl;
  KTIDBGERRSTDIS1_KTI_CIOPHYDFX_STRUCT KtiDbgErrStDis1;
  UINT8  Port;

  for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
    if ((host->var.kti.CpuInfo[Soc].LepInfo[Port].Valid == TRUE) && (host->var.kti.CpuInfo[Soc].SocType == SOCKET_TYPE_CPU)) {
      KtiDbgErrStDis1.Data = KtiReadPciCfg (host, Soc, Port, KTIDBGERRSTDIS1_KTI_CIOPHYDFX_REG);
      if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
        //
        // A0 Stepping workaround: s4929014
        // Before programming KTIA2RCRCTRL need to set all bits in disable_ctrl_flowqoverunder
        // This field needs to be cleared after WR
        //
        KtiDbgErrStDis1.Bits.disable_ctrl_flowqoverunder = 0x7ff;
      } else {
        //
        // s5371481: KTI LL is incorrectly flagging AK VNA overflow/underflow error indication when VN1 is enabled
        //
        KtiDbgErrStDis1.Bits.disable_ctrl_flowqoverunder = 1;
      }
      KtiWritePciCfg (host, Soc, Port, KTIDBGERRSTDIS1_KTI_CIOPHYDFX_REG, KtiDbgErrStDis1.Data);

      //
      // KTIA2RCRCTRL (port x, vn0) credits,
      //
      KtiA2RcrCtrl.Data = KtiReadPciCfg (host, Soc, Port, KTIA2RCRCTRL_KTI_LLPMON_REG);
      KtiA2RcrCtrl.Bits.ad_rsp = 0;       //VN0/1 CRD encodings are ""value + 1"" (0=1CRD, 1=2CRD...)
      KtiA2RcrCtrl.Bits.ad_snp = 0;       //VN0/1 CRD encodings are ""value + 1"" (0=1CRD, 1=2CRD...)
      KtiA2RcrCtrl.Bits.bl_rsp = 0;       //VN0/1 CRD encodings are ""value + 1"" (0=1CRD, 1=2CRD...)

      if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
        KtiA2RcrCtrl.Bits.ak_vna = 0x1f;    //s4929464, program the AK flowQ credit value in LL to a value less than the max AK flowQ entries (31 or less)
      } else {
        //s5370383
        KtiA2RcrCtrl.Bits.ak_vna = 0x20;
      }

      if (host->var.kti.OutVn1En == TRUE) {
        //s4929979: Cloned From SKX Si Bug Eco: M3KTI AD message corruption in egress/FlowQ when free list is down to two entries
        if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
          KtiA2RcrCtrl.Bits.ad_vna = 32;
        } else {
          KtiA2RcrCtrl.Bits.ad_vna = 33;
        }
        KtiA2RcrCtrl.Bits.bl_vna = 14;
      } else {
        if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
          KtiA2RcrCtrl.Bits.ad_vna = 36;
        } else {
          KtiA2RcrCtrl.Bits.ad_vna = 37;
        }
        KtiA2RcrCtrl.Bits.bl_vna = 18;
      }
      KtiWritePciCfg (host, Soc, Port, KTIA2RCRCTRL_KTI_LLPMON_REG, KtiA2RcrCtrl.Data);
    }
  }

  return KTI_SUCCESS;
}

/**

    Program M2Mem credits.

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param Soc               - Socket ID
    @param M2mem             - M2mem
    @param Port              - Port
    @param CrdtType          - Credit type
    @param CrdtCnt           - Credit count
    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramM2mCreditReg (
  struct sysHost      *host,
  UINT8               Soc,
  UINT8               M2mem,
  UINT8               Port,
  UINT8               CrdtType,
  UINT8               CrdtCnt
  )
{
  CREDITS_M2MEM_MAIN_STRUCT     CreditsM2mem;
  CreditsM2mem.Data = 0;
  CreditsM2mem.Bits.crdttype = (UINT32)(CrdtType | ( Port << 4));
  CreditsM2mem.Bits.crdtcnt = (UINT32)CrdtCnt;      // Credit count
  CreditsM2mem.Bits.crdtwren = 0x0;                 // clear write enable bit
  KtiWritePciCfg (host, (UINT8)Soc, M2mem, CREDITS_M2MEM_MAIN_REG, CreditsM2mem.Data);
  CreditsM2mem.Bits.crdtwren = 0x1;                 // Credit Write Enable
  KtiWritePciCfg (host, (UINT8)Soc, M2mem, CREDITS_M2MEM_MAIN_REG, CreditsM2mem.Data);
  CreditsM2mem.Bits.crdtwren = 0x0;                 // clear write enable bit
  KtiWritePciCfg (host, (UINT8)Soc, M2mem, CREDITS_M2MEM_MAIN_REG, CreditsM2mem.Data);

  return KTI_SUCCESS;
}

/**

    Program M2Mem credits.

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param Soc               - Socket ID

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramM2mCreditSetting (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Soc
  )
{
  UINT8   Port, Ctr, BlCreditPerM2m, RequiredCredit, NumOfCha;
  UINT8   TotalBlCredit, BlNcsCredit;
  UINT8   Ch;

  // Total_credit >=  (#of CHA * BldVnaCrditPerCha) + (active_number_link - 1) ( KTI->KTI VNA + VN0 + VN1 if needed ) if route through +  CHA-KTI VN0 + MAX_M2PCIE_TO_M3KTI_BL_VNA
  RequiredCredit = 0;

  if (host->var.kti.OutVn1En == FALSE) {
    if(KtiInternalGlobal->IsRouteThrough == TRUE) {
      RequiredCredit = KTI_TO_KTI_VN0_BL_INGRESS + TOTAL_K2K_BL_VNA;    // VN0 + VNA
    }

    // add cha vn0 credit
    RequiredCredit = RequiredCredit +  CHA_TO_KTI_VN0_BL_INGRESS;       // CHA-KTI VN0

    // sum of all IIO -> KTI BL credit
    TotalBlCredit = 0;
    for (Ctr = 0; Ctr < MAX_SKX_M2PCIE; Ctr ++){
      if (KtiInternalGlobal->M2PcieActiveBitmap[Soc] & (1 << Ctr)) {
        if( KtiInternalGlobal->TotActiveM2Pcie[Soc] <= 3){                        //  IIO stack -> KTI BL NCS
           if ((host->var.kti.SysConfig == SYS_CONFIG_8S)){
             BlNcsCredit = IioToKti0_BlCreditTable_For3M2PcieAnd8Socket[Ctr];
           } else {
             BlNcsCredit = IioToKti0_BlCreditTable_For3M2Pcie[Ctr];
           }
        } else {
           BlNcsCredit = IioToKti0_BlCreditTable_For4M2Pcie[Ctr];
        }
        TotalBlCredit = TotalBlCredit + BlNcsCredit;
      }
    }

    RequiredCredit = RequiredCredit + TotalBlCredit;

    // add credit from CHA vna credit
    NumOfCha = KtiInternalGlobal->TotCha[Soc];
    if (NumOfCha > 14) {
       RequiredCredit += NumOfCha;
    } else {
       RequiredCredit += (2 * NumOfCha);
    }

    KTI_ASSERT(TOTAL_KTI_BL_CREDIT > RequiredCredit, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_3);
    BlCreditPerM2m = (TOTAL_KTI_BL_CREDIT - RequiredCredit) / (UINT8) (KtiInternalGlobal->TotM2Mem[Soc]);
    //
    // s5332305: Maximum D2K credits is 0x0F
    //
    if (BlCreditPerM2m >= 0x10){
      BlCreditPerM2m = 0x0F;
    }
    //
    // s5372239: D2K credit setting for 2S3UPI
    //
    if (host->var.kti.SysConfig == SYS_CONFIG_2S3L) {
      BlCreditPerM2m = 0x03;
    }
    
  } else {
    BlCreditPerM2m = 0;               // VN1 enabled (D2K disabled), then M2M->KTI credits are not required
  }

  // program into m2mem credit register
  for (Ctr = 0; Ctr < KtiInternalGlobal->TotM2Mem[Soc]; Ctr++) {
    //Credit Type 0xd : BL Ingress TransgressedQ (per Mc)
    ProgramM2mCreditReg(host, Soc, Ctr, 0, 0xd, 8);

    for(Ch=0; Ch < 3; Ch++){
      //
      // s4929272: Cloned From SKX Si Bug Eco: patrol deallocated by SbDeallocAll
      //   Set M2Mem's SB credits to 126 for each MC channel:
      //   - Chnl0: Credits.CrdtWrEn = 1, Credits.CrdtType = 0x0A, Credits.CrdtCnt = 126
      //   - Chnl1: Credits.CrdtWrEn = 1, Credits.CrdtType = 0x1A, Credits.CrdtCnt = 126
      //   - Chnl2: Credits.CrdtWrEn = 1, Credits.CrdtType = 0x2A, Credits.CrdtCnt = 126
      //
      ProgramM2mCreditReg(host, Soc, Ctr, Ch, 0xa, 126);

      //
      // 5331281: mesh2mem BGF credit counter is set too low for performance
      //
      ProgramM2mCreditReg(host, Soc, Ctr, Ch, 0xe, 0x0a);
    }
    for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
      if (host->var.kti.CpuInfo[Soc].LepInfo[Port].Valid == TRUE) {
        if (Port == 2 && host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
          //
          // 4929519: Cloned From SKX Si Bug Eco: Mesh ID encoding for KTI Half tile is incorrect
          // For SKX A0, always zero out M2M 0xF type credits for Kti Port 2.
          //
          ProgramM2mCreditReg(host, Soc, Ctr, Port, 0xf, 0);
        } else {
          //Credit Type 0xf : Intel UPI (D2K)
          ProgramM2mCreditReg(host, Soc, Ctr, Port, 0xf, BlCreditPerM2m);
        }
      }
    }
  }

  return KTI_SUCCESS;
}


/**

  Program mesh credits that require reset to take effect.
  For 1S topologies only.

  @param host              - pointer to the system host (root) structure
  @param SocketData        - pointer to socket data structure
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramMeshCredits1S (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{

  UINT8                 Sbsp;
  Sbsp = KtiInternalGlobal->SbspSoc;

  // Cha
  ProgramChaCreditSetting(host, KtiInternalGlobal, Sbsp);
  //M2PCIE:
  ProgramM2PcieCreditSetting(host, KtiInternalGlobal, Sbsp);

  return KTI_SUCCESS;
}

/**

  Program the mesh credits that require reset for it to take effect.

  @param host              - Pointer to the system host (root) structure
  @param SocketData        - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramMeshCreditsCommonSkx (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8    Soc;
  //
  // Allocate credits for all the mesh agents communication.
  //

  //
  // Program the values into each socket.
  //
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    if ((SocketData->Cpu[Soc].Valid == TRUE) && (SocketData->Cpu[Soc].SocType == SOCKET_TYPE_CPU)) {
      //M2PCIE:
      ProgramM2PcieCreditSetting(host, KtiInternalGlobal, Soc);

      //CHA:
      ProgramChaCreditSetting(host, KtiInternalGlobal, Soc);

      //M3KTI:
      ProgramM3KtiCreditSetting (host, KtiInternalGlobal, Soc);

      //KTI Agent:
      ProgramKtiCreditSetting (host, KtiInternalGlobal, Soc);
    }
  }

  return KTI_SUCCESS;
}
