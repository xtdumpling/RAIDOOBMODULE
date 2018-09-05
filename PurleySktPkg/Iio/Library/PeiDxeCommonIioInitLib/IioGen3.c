
/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioGen3.c

  Iio Gen3 initialization.

**/
#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <IioSetupDefinitions.h>
#include <IioInitLib.h>
#include <RcRegs.h>
#include "IioWorkAround.h"
#include "IioGen3.h"
#include "IioEarlyInitialize.h"

static UINT8 MaxGen3EqRegs[] = {0, 16, 4, 8, 4, 16, 4, 8, 4, 16, 4, 8, 4};

static UINTN BifurcationMap[5][4] = {{ 4, 4, 4, 4},
                                     { 8, 0, 4, 4},
                                     { 4, 4, 8, 0},
                                     { 8, 0, 8, 0},
                                     {16, 0, 0, 0} };

#define TEMP_BUSBASE_OFFSET            1

/**

    This function will program GEN3PRIVTXEQ register
    with the required value.

    @param IioGlobalData - Pointer to IIO_GLOBLAS structure
    @param IioIndex      - Socket Index
    @param Port          - Port Index
    @param Precursor     - Precursor value to be program
    @param Cursor        - Cursor value to be program
    @param Postcursor    - Postcursor value to be program

    @retval GEN3_SUCCESS - Program was done successfully.

**/
IIO_STATUS
Gen3ProgPrivTxEqCoefficients (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8        IioIndex,
  IN  UINT8        PortIndex,
  IN  UINT32       Precursor,
  IN  UINT32       Cursor,
  IN  UINT32       Postcursor
  )
{
  UINT8                                Ctr;
  UINT8                                ClusterPort;
  UINT8                                Bifurcation;
  GEN3PRIVTXEQ_IIO_DFX_STRUCT          Gen3PrivTxEq;
  PXPPRTGEN3EQPRIV_IIO_DFX_STRUCT     PxpPrtGen3EqPriv;
  UINTN                                CtrEnd;


  ClusterPort = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
  Bifurcation = CheckBifurcationMapPort(IioGlobalData, IioIndex, PortIndex);
  CtrEnd = (BifurcationMap[Bifurcation][PortIndex - ClusterPort] / 4);
  
  for (Ctr = 0; Ctr < CtrEnd; Ctr++) {
    PxpPrtGen3EqPriv.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PXPPRTGEN3EQPRIV_IIO_DFX_REG);
   
    Gen3PrivTxEq.Data = 0;
    Gen3PrivTxEq.Bits.bndl0_g3ptxeq_precursor  = Precursor;
    Gen3PrivTxEq.Bits.bndl0_g3ptxeq_cursor     = Cursor;
    Gen3PrivTxEq.Bits.bndl0_g3ptxeq_postcursor = Postcursor;
    Gen3PrivTxEq.Bits.bndl1_g3ptxeq_precursor  = Precursor;
    Gen3PrivTxEq.Bits.bndl1_g3ptxeq_cursor     = Cursor;
    Gen3PrivTxEq.Bits.bndl1_g3ptxeq_postcursor = Postcursor;
    PxpPrtGen3EqPriv.Bits.use_g3eqprivatecsrvalues = 1;
    
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, PXPPRTGEN3EQPRIV_IIO_DFX_REG, PxpPrtGen3EqPriv.Data);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, GEN3PRIVTXEQ_IIO_DFX_REG, Gen3PrivTxEq.Data);
  }

  return GEN3_SUCCESS;
}

/**

    This routine will program GEN3PRIVRMTTXEQ register with the required
    values.


    @param IioGlobalData - Pointer to IIO_GLOBLAS structure
    @param IioIndex      - Socket Index
    @param Port          - Port Index
    @param Precursor     - Precursor value to be program
    @param Cursor        - Cursor value to be program
    @param Postcursor    - Postcursor value to be program

    @retval GEN3_SUCCESS - Program was done successfully.

**/
IIO_STATUS
Gen3ProgPrivRmtTxEqCoefficients (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8        IioIndex,
  IN  UINT8        PortIndex,
  IN  UINT32       Precursor,
  IN  UINT32       Cursor,
  IN  UINT32       Postcursor
  )
{
  UINT8                                Ctr;
  UINT8                                ClusterPort;
  UINT8                                Bifurcation;
  TX_PH3_CURSOR_IIO_DFX_STRUCT         TxPh3Cursor;
  GEN3PRIVRMTTXEQ_IIO_DFX_STRUCT       Gen3PrivRmtTxEq;
  UINTN                                CtrEnd;

  ClusterPort = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
  Bifurcation = CheckBifurcationMapPort(IioGlobalData, IioIndex, PortIndex);
  CtrEnd = (BifurcationMap[Bifurcation][PortIndex - ClusterPort] / 4);
  
  for (Ctr = 0; Ctr < CtrEnd; Ctr++) {
    Gen3PrivRmtTxEq.Data = 0;
    Gen3PrivRmtTxEq.Bits.bndl0_g3prmttxeq_precursor = Precursor;
    Gen3PrivRmtTxEq.Bits.bndl0_g3prmttxeq_cursor     = Cursor;
    Gen3PrivRmtTxEq.Bits.bndl0_g3prmttxeq_postcursor = Postcursor;
    Gen3PrivRmtTxEq.Bits.bndl1_g3prmttxeq_precursor  = Precursor;
    Gen3PrivRmtTxEq.Bits.bndl1_g3prmttxeq_cursor     = Cursor;
    Gen3PrivRmtTxEq.Bits.bndl1_g3prmttxeq_postcursor = Postcursor;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, GEN3PRIVRMTTXEQ_IIO_DFX_REG, Gen3PrivRmtTxEq.Data);
    TxPh3Cursor.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, TX_PH3_CURSOR_IIO_DFX_REG);
    TxPh3Cursor.Bits.bypass_endcard_coeff = 0;
	IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, TX_PH3_CURSOR_IIO_DFX_REG, TxPh3Cursor.Data);
  }

  return GEN3_SUCCESS;
}

/**

    This routine will update TX_PH3_CURSOR and FSLVAL registers

    @param IioGlobalData - Pointer to the IO_GLOBALS stucture
    @param IioIndex      - Socket Index
    @param PortIndex     - Port Index

    @retval GEN3_SUCCESS - Update was done successfully.

**/
IIO_STATUS
Gen3AlternateTxPh3Coefficients(
  IIO_GLOBALS  *IioGlobalData,
  UINT8        IioIndex,
  UINT8        PortIndex
  )
{
  UINT8                                Ctr;
  UINT8                                ClusterPort;
  UINT8                                Bifurcation;
  TX_PH3_CURSOR_IIO_DFX_STRUCT         TxPh3Cursor;
  FSLFVAL_REG_IIO_DFX_STRUCT           FslFval;
  UINTN                                CtrEnd;

  ClusterPort = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
  Bifurcation = CheckBifurcationMapPort(IioGlobalData, IioIndex, PortIndex);
  CtrEnd = (BifurcationMap[Bifurcation][PortIndex - ClusterPort] / 4);

  for (Ctr = 0; Ctr < CtrEnd; Ctr++) {
     TxPh3Cursor.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, TX_PH3_CURSOR_IIO_DFX_REG);
     TxPh3Cursor.Bits.b0sub_post_cursor = 4;
     TxPh3Cursor.Bits.b1sub_post_cursor = 4;
     TxPh3Cursor.Bits.b0sub_pre_cursor = 4;
     TxPh3Cursor.Bits.b1sub_pre_cursor = 4;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, TX_PH3_CURSOR_IIO_DFX_REG, TxPh3Cursor.Data);
  }

  FslFval.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, FSLFVAL_REG_IIO_DFX_REG);
  FslFval.Bits.fsthreshold = 39;
  IioWriteCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_REG, FslFval.Data);

  return GEN3_SUCCESS;
}

/**

    This routine will update TX_PH3_CURSOR register

    @param IioGlobalData - Pointer to the IO_GLOBALS stucture
    @param IioIndex      - Socket Index
    @param PortIndex     - Port Index

    @retval GEN3_SUCCESS - Update was done successfully.

**/
IIO_STATUS
Gen3ProgTxPh3Coefficients (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8        IioIndex,
  IN  UINT8        PortIndex
  )
{
  UINT8                                Ctr;
  UINT8                                ClusterPort;
  UINT8                                Bifurcation;
  TX_PH3_CURSOR_IIO_DFX_STRUCT         TxPh3Cursor;
  UINTN                                CtrEnd;

  ClusterPort = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
  Bifurcation = CheckBifurcationMapPort(IioGlobalData, IioIndex, PortIndex);
  CtrEnd = (BifurcationMap[Bifurcation][PortIndex - ClusterPort] / 4);

  for (Ctr = 0; Ctr < CtrEnd; Ctr++) {
     TxPh3Cursor.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, TX_PH3_CURSOR_IIO_DFX_REG);
     TxPh3Cursor.Bits.b0sub_post_cursor = 6;
     TxPh3Cursor.Bits.b1sub_post_cursor = 6;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex+Ctr, TX_PH3_CURSOR_IIO_DFX_REG, TxPh3Cursor.Data);
  }

  return GEN3_SUCCESS;
}

/**

    The purpose of this function is to apply all the
    GEn3 override required in the link

    @param IioGlobalData - Pointer to IIO_GLOBALS structure
    @param IioIndex      - Socket index
    @param PortIndex     - Port index

    @retval Status - GEN3_SUCCESS - Override was done successfully.

**/
GEN3_STATUS
Gen3PrelinkOverride (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             IioIndex,
  IN  UINT8                             PortIndex
  )
{
    GEN3_STATUS                             Status;
    PXPPRTGEN3EQPRIV_IIO_DFX_STRUCT         PxpPrtGen3EqPriv;
    FSLFVAL_REG_IIO_DFX_STRUCT              FslFval;
    GEN3TXEQPRESETTABLE_11_IIO_DFX_STRUCT   Gen3TxeqPresettable;
    UINT8                             Dfx_Gen3_Mode;

    Status = GEN3_SUCCESS;

    Dfx_Gen3_Mode = IioGlobalData->SetupData.DfxGen3OverrideMode[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex];
    if((IioGlobalData->SetupData.TXEQ[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) &&
       (Dfx_Gen3_Mode == DFX_GEN3_OVERRIDE_MODE_UNIPHY)){
       Dfx_Gen3_Mode = DFX_GEN3_OVERRIDE_MODE_ALTERNATE_TXEQ;
    }
    //
    // 5372887: Change PCIe Gen3 Preset 10 default postcursor to 21
    //
    Gen3TxeqPresettable.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, GEN3TXEQPRESETTABLE_11_IIO_DFX_REG);
    Gen3TxeqPresettable.Bits.presettable_postcursor11 = 21;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, GEN3TXEQPRESETTABLE_11_IIO_DFX_REG, Gen3TxeqPresettable.Data);

    switch (Dfx_Gen3_Mode) {
      case DFX_GEN3_OVERRIDE_MODE_MANUAL:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO,"Gen3: Manual override mode \n",
                          IioIndex, IIO_PORT_LABEL(PortIndex), PortIndex);
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Gen3: Manual override mode \nDfxDnTxPreset: %d\n RxPreset: %d\n DfxUpTxPreset: %d\n",
                          IioGlobalData->SetupData.DfxGen3ManualPh2_Precursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],
                          IioGlobalData->SetupData.DfxGen3ManualPh2_Cursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],
                          IioGlobalData->SetupData.DfxGen3ManualPh2_Postcursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]);

        PxpPrtGen3EqPriv.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PXPPRTGEN3EQPRIV_IIO_DFX_REG);
        PxpPrtGen3EqPriv.Bits.use_g3eqprivatecsrvalues = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PXPPRTGEN3EQPRIV_IIO_DFX_REG, PxpPrtGen3EqPriv.Data);

        Gen3ProgPrivTxEqCoefficients(IioGlobalData,IioIndex,PortIndex,
             IioGlobalData->SetupData.DfxGen3ManualPh2_Precursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],  //precursor
             IioGlobalData->SetupData.DfxGen3ManualPh2_Cursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],  //cursor
             IioGlobalData->SetupData.DfxGen3ManualPh2_Postcursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]); //postcursor

        PxpPrtGen3EqPriv.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PXPPRTGEN3EQPRIV_IIO_DFX_REG);
        PxpPrtGen3EqPriv.Bits.use_g3eqprivcsr4adaptsm = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PXPPRTGEN3EQPRIV_IIO_DFX_REG, PxpPrtGen3EqPriv.Data);

        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Gen3: Manual override mode \nDfxDnTxPreset: %d\n RxPreset: %d\n DfxUpTxPreset: %d\n",
                          IioGlobalData->SetupData.DfxGen3ManualPh3_Precursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],
                          IioGlobalData->SetupData.DfxGen3ManualPh3_Cursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],
                         IioGlobalData->SetupData.DfxGen3ManualPh3_Postcursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]);
        Gen3ProgPrivRmtTxEqCoefficients(IioGlobalData,IioIndex,PortIndex,
            IioGlobalData->SetupData.DfxGen3ManualPh3_Precursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],  //precursor
            IioGlobalData->SetupData.DfxGen3ManualPh3_Cursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],  //cursor
            IioGlobalData->SetupData.DfxGen3ManualPh3_Postcursor[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]); //postcursor
      break;
      case DFX_GEN3_OVERRIDE_MODE_TEST_CARD:
         if (IioGlobalData->SetupData.DfxGen3TestCard[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] == DFX_GEN3_TEST_CARD_LAGUNA){
           IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO,"Gen3: Gen3PrelinkOverride for Gen3 Laguna Test Card(SKT=%d, PORT=%s(%d))\n",
                  IioIndex, IIO_PORT_LABEL(PortIndex), PortIndex);
           PxpPrtGen3EqPriv.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PXPPRTGEN3EQPRIV_IIO_DFX_REG);
           PxpPrtGen3EqPriv.Bits.use_g3eqprivatecsrvalues = 1;
           IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PXPPRTGEN3EQPRIV_IIO_DFX_REG, PxpPrtGen3EqPriv.Data);

           Gen3ProgPrivTxEqCoefficients(IioGlobalData,IioIndex,PortIndex,
                                        11,  //precursor
                                         41,  //cursor
                                         11); //postcursor

        } else if (IioGlobalData->SetupData.DfxGen3TestCard[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] == DFX_GEN3_TEST_CARD_NTB){
           IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO,"Gen3: Gen3PrelinkOverride for Gen3 NTB Test Card(SKT=%d, PORT=%s(%d))\n",
                  IioIndex, IIO_PORT_LABEL(PortIndex), PortIndex);
           // Program pxpX_tx_ph3_cursor.b1sub_post_cursor, this is a per-bundle field.
           // So this means it must be set for all ports. If, for example, we are looking at
           // IOU2 and it is bifurcated as a x8, then pxp2_tx_ph3_cursor (port 2 (1a))
           // as well as pxpX_tx_ph3_cursor (port 3 (1b)).
           Gen3ProgTxPh3Coefficients(IioGlobalData,IioIndex,PortIndex);
           FslFval.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, FSLFVAL_REG_IIO_DFX_REG);
           FslFval.Bits.fsthreshold = 39;
           IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, FSLFVAL_REG_IIO_DFX_REG, FslFval.Data);
        }
      break;
      case DFX_GEN3_OVERRIDE_MODE_ALTERNATE_TXEQ:
        //
        // 5370996: PCIe Gen 3 Alternate TxEq
        //
          Gen3AlternateTxPh3Coefficients(IioGlobalData,IioIndex,PortIndex);
      break;
      case DFX_GEN3_OVERRIDE_MODE_UNIPHY:
      default:
          IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Gen3: Gen3PreLinkOverride Uniphy mode (SKT=%d, PORT=%s(%d))\n",
             IioIndex, IIO_PORT_LABEL(PortIndex), PortIndex);
          //
          //Do not Override original values  given by UNIPHY recipe
          //
       break;
    }

    PresetOverride(IioGlobalData, IioIndex, PortIndex);
    return Status;
}

/**

    The purpose of this function is to verify if an override is required
    before issue a Gen3 Link intialization

    @param IioGlobalData - Pointer to IIO_GLOBALS structure
    @param IioIndex      - Socket Index

    @retval None

**/
VOID
CheckGen3PreLinkOverride (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex
  )
{
  IIO_STATUS          Status;
  UINT8               PortIndex;

  for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {

    if ((IioGlobalData->IioVar.IioOutData.PciePortPresent[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] == 0 ) ||
        (IioGlobalData->IioVar.IioOutData.PciePortConfig[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] == 0 )){
        continue;
    }

    //if port is set to disable (Disable Link = TRUE) than skip
    if(CheckVarPortEnable(IioGlobalData, IioIndex, PortIndex) == PCIE_PORT_DISABLE){
      continue;
    }

    Status = Gen3PrelinkOverride(IioGlobalData, IioIndex, PortIndex);
    IioStall (IioGlobalData, UNIPHY_RECIPE_DELAY);   //Intended fix for s4030382: give a delay of 180us after Pre-link Gen3 override
  }
  return;
}

VOID
PresetOverride(
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8       IioIndex,
  IN  UINT8       PortIndex
  )
{
  LN0EQ_IIO_PCIE_STRUCT           LnEq;
  UINT8                           Ctrl;

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Gen3: Preset: DnTx: %d\nRxPreset: %d\nUpTxPreset: %d\n",
  IioGlobalData->SetupData.DfxDnTxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],
  IioGlobalData->SetupData.DfxRxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex],
  IioGlobalData->SetupData.DfxUpTxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]);

  for (Ctrl = 0; Ctrl < MaxGen3EqRegs[PortIndex]; Ctrl++) {
    LnEq.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, ( LN0EQ_IIO_PCIE_REG+(Ctrl*2) ));

    if (IioGlobalData->SetupData.DfxDnTxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] != GEN3_TX_PRESET_AUTO) {
      LnEq.Bits.dntxpreset = IioGlobalData->SetupData.DfxDnTxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex];
    } else {
      LnEq.Bits.dntxpreset = 7;
    }

    if (IioGlobalData->SetupData.DfxRxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] != GEN3_RX_PRESET_AUTO) {
      LnEq.Bits.dnrxpreset = IioGlobalData->SetupData.DfxRxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex];
    }

    if (IioGlobalData->SetupData.DfxUpTxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] != GEN3_TX_PRESET_AUTO) {
      LnEq.Bits.uptxpreset = IioGlobalData->SetupData.DfxUpTxPreset[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex];
    }
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, ( LN0EQ_IIO_PCIE_REG+(Ctrl*2) ), LnEq.Data);
  }
}
