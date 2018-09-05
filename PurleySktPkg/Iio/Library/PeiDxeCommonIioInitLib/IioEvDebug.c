/*++
  This file contains  'PCIE debug routines' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2016   Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file  IioEvDebug.c


  Debug routines for dumping training states, values or margins.

--*/

#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <RcRegs.h>
#include "IioEvDebug.h"

/* Monbus load bus codes */
#define EVMONBUS          0x0
#define EVTRACEBUS        0x1

struct iio_monbusParameter {
    UINT8   busType;        // MonBus or Trace Bus
    UINT8   loadSel;        // Lookup code for monbus or tracebus
    UINT8   lsb;            // MSB for mask
    UINT8   msb;            // LSB for mask
};

struct iio_monbusParameter  EvParameterList[] = {
    {EVMONBUS,0x3D,11,13},     // 0 AFE SUMR OC 
    {EVMONBUS,0x3C,0,3},     // 1 RVREF
    {EVMONBUS,0x3B,0,13},     // 2 CTOC 
    {EVMONBUS,0x3A,0,3},     // 3 OC START 
    {EVMONBUS,0x39,4,5},     // 4 OC DONE 
    {EVMONBUS,0x37,0,5},     // 5 SUMR
    {EVMONBUS,0x36,0,7},     // 6 ESM1
    {EVMONBUS,0x35,0,7},     // 7 ESM0
    {EVMONBUS,0x34,0,7},     // 8 ESP1
    {EVMONBUS,0x33,0,7},     // 9 ESP0
    {EVMONBUS,0x32,0,7},     // 10 DS1 
    {EVMONBUS,0x31,0,7},     // 11 DS0 
    {EVMONBUS,0x30,0,4},     // 12 SQLCH 
    {EVMONBUS,0x2D,0,13},     // 13 VREF ADAPT CTRL
    {EVMONBUS,0x2C,7,13},     // 14 DFE1 STEP ACC 
    {EVMONBUS,0x2B,8,13},     // 15 DFE2 STEP ACC 
    {EVMONBUS,0x2A,9,13},     // 16 DFE3 STEP ACC 
    {EVMONBUS,0x29,9,13},     // 17 DFE4 STEP ACC 
    {EVMONBUS,0x28,7,12},     // 18 DFE1 
    {EVMONBUS,0x27,8,13},     // 19 DFE2 
    {EVMONBUS,0x26,9,13},     // 20 DFE3 
    {EVMONBUS,0x25,9,13},     // 21 DFE4 
    {EVMONBUS,0x24,4,13},     // 22 AGC STEP 
    {EVMONBUS,0x23,9,13},     // 23 AGC 
    {EVMONBUS,0x22,7,13},     // 24 CDR STEP 
    {EVMONBUS,0x21,7,13},     // 25 CDR 
    {EVMONBUS,0x20,7,13},     // 26 CDR_ACC 
    {EVMONBUS,0x19,7,13},     // 27 RXDCC
    {EVMONBUS,0x03,8,13},     // 28 ICOMP 
    {EVMONBUS,0x01,8,13},     // 29 TX ICOMP OVRD 
    {EVTRACEBUS,0x03,16,21},     // 30 ICOMP
    {EVTRACEBUS,0x21,25,31},     // 31 CDR
    {EVTRACEBUS,0x22,21,25},     // 32 AGC
    {EVTRACEBUS,0x23,25,30},     // 33 DFE1
    {EVTRACEBUS,0x24,26,31},     // 34 DFE2
    {EVTRACEBUS,0x25,27,31},     // 35 DFE3
    {EVTRACEBUS,0x26,27,31},     // 36 DFE4
    {EVTRACEBUS,0x27,24,31},     // 37 CTOC1
    {EVTRACEBUS,0x27,11,16},     // 38 CTOC2
    {EVTRACEBUS,0x3D,11,17},     // 39 TXDCC
    {EVTRACEBUS,0x3D,25,31},     // 40 RX0DCC
    {EVTRACEBUS,0x3D,18,24},     // 41 RX1DCC
};
UINT32 PortLinkSelect[] = { 0, 0, 1, 0, 1, 2, 3, 0, 1, 2, 3};

/**

    This routine is to Lock or unlock DFX hooks.

    @param IioGlobalData        - Pointer to IioGlobalData
    @param IioIndex             - Index to Iio
    @param Lock                 - Flag to lock or unlock DFX

    @retval None

**/
VOID
PcieUnlockDfx (
  IN IIO_GLOBALS     *IioGlobalData,
  IN UINT8            IioIndex,
  IN UINT8            Lock
  )
{
    IIO_DFX_LCK_CTL_CSR_IIO_DFX_GLOBAL_STRUCT   PcieLock;
    UINT8                                       PortIndex;
    UINT8                                       Stack;
    
    for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
        if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
          continue;
        }

        Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
        PcieLock.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, IIO_DFX_LCK_CTL_CSR_IIO_DFX_GLOBAL_REG);
        if (Lock == 0 ) {
            PcieLock.Bits.dfxuniphylck  = 0;
            PcieLock.Bits.reuteplck     = 0;
            PcieLock.Bits.reutenglck    = 0;
            PcieLock.Bits.dbgbuslck     = 0;
        } else {
            PcieLock.Bits.dfxuniphylck  = 1;
            PcieLock.Bits.reuteplck     = 1;
            PcieLock.Bits.reutenglck    = 1;
            PcieLock.Bits.dbgbuslck     = 1;
        }
        
       IioWriteCpuCsr16(IioGlobalData, IioIndex, Stack, IIO_DFX_LCK_CTL_CSR_IIO_DFX_GLOBAL_REG, PcieLock.Data);
    }
}

/**

    This routine will dump Arc state of the link

    @param IioGlobalData        - Pointer to IioGlobalData
    @param L0ExitArc            - L0 Exit Arc value

    @retval None

**/
VOID 
DecodeLtssmArc (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT32       L0ExitArc
  )
{
    switch(L0ExitArc) {
    case 0:
       IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "RxL0s                  ");
       break;
    case 1:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "REC                   ");
        break;
    case 2:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "FERR                  ");
        break;
    case 3:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "REDOEQ                ");
        break;
    case 4:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "PSMI                  ");
        break;
    case 5:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "PROLOG                ");
        break;
    case 6:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SENT                  ");
        break;
    case 7:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SLD                   ");
        break;
    default:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN               ");
        break;
    }
}

/**

    This routine will dump LTSSM state of the link

    @param IioGlobalData        - Pointer to IioGlobalData
    @param MainState            - Value of the Main LTSSM State
    @param SubState             - Value of the Sub LTSSM State 

    @retval None

**/
VOID
DecodeLtssm (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT32        MainState,
  IN  UINT32        SubState
  )
{
  switch ( MainState ) {
    case 0:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "           DETECT.");
      switch ( SubState ) {
          case 0:
            IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "QUIET_RST          ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "QUIET_ENTER        ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "QUIET              ");
              break;
          case 3:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ACT_128US          ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ACTIVE             ");
              break;
          case 5:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ACT2_128US         ");
              break;
          case 6:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "POL                ");
              break;
          case 7:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EXIT_SQUELCH       ");
              break;
          case 8:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "RST_ASSERT         ");
              break;
          case 9:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "RECOMBINE          ");
              break;
          case 0xa:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "TRANS_GEN1         ");
              break;
          case 0xb:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "STGR               ");
              break;
          case 0xc:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SEND_TS1_POLL      ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN            ");
              break;
      }

      break;
    case 1:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "   POLLING/L0_EXT.");
      switch ( SubState ) {
          case 0:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ACTIVE             ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "CONFIG             ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G1/POL_COMPLIANCE      ");
              break;
          case 3:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Z_NO_EIOS          ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G2_ENTRY      ");
              break;
          case 5:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G2_PREP       ");
              break;
          case 6:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G2            ");
              break;
          case 7:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "PDP                ");
              break;
          case 8:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G2_EXIT       ");
              break;
          case 9:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G2_WAIT       ");
              break;
          case 0xa:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G2_PREP_EXIT  ");
              break;
          case 0xb:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G1_EIOS       ");
              break;
          case 0xc:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMP_G2_IDLE_WAIT  ");
              break;
          case 0xd:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "CHECK_COMPL        ");
              break;
          case 0xe:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SQ_EXIT            ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN            ");
              break;
      }
      break;
    case 2:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "           CONFIG.");
      switch ( SubState ) {
          case 0:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LNKWID_START       ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LNKWID_ACCEPT      ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LANENUM_WAIT       ");
              break;
          case 3:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LANENUM_ACCEPT     ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMPLETE           ");
              break;
          case 5:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "IDLE               ");
              break;
          case 6:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LNKWID_START_REC   ");
              break;
          case 7:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "IDLE_OLD           ");
              break;
          case 8:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LWS_WAIT_FOR_TS    ");
              break;
          case 9:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LNKWID_START_UPCFG ");
              break;
          case 0xa:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LNKWID_START_UPCFG_PREP_EXIT_MASTER");
              break;
          case 0xb:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LNKWID_START_UPCFG_PREP_EXIT_SLAVE ");
              break;
          case 0xc:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LWS_WAIT4_BITLOCK  ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN            ");
              break;
      }
      break;
    case 3:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "               L0.");
      switch ( SubState ) {
          case 0:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "L0                 ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "TXL0S_WAIT4EIOS    ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "TXL0S_IDLE         ");
              break;
          case 3:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "TXL0S_PREP_EXIT    ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "L1_EXIT            ");
              break;
          case 5:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "L1_IDLE            ");
              break;
          case 6:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "L2_IDLE            ");
              break;
          case 7:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "L1L2_ENTRY         ");
              break;
          case 8:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "TXL0S_FTS          ");
              break;
          case 9:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "L2_IDLE_WAIT       ");
              break;
          case 0xd:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UPSTREAM_L1L2_EIOS ");
              break;
          case 0xe:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "L1_IDLE_WAIT       ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN            ");
              break;
      }
      break;
    case 4:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "         RECOVERY.");
      switch ( SubState ) {
          case 0:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "COMPLETE           ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "RCVRCFG            ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "RCVRLOCK           ");
              break;
          case 3:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "IDLE               ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SPEED              ");
              break;
          case 5:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "RCVRCFG_SPEED      ");
              break;
          case 6:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "WAIT_FOR_GEN_TRANS ");
              break;
          case 7:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "W4RX2COMPLETE      ");
              break;
          case 0xa:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SPEED_IDLE         ");
              break;
          case 0xb:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SPEED_WAIT_1US_PREP");
              break;
          case 0xc:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SPEED_WAIT_1US     ");
              break;
          case 0xd:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "WAIT_EXIT_EI       ");
              break;
          case 0xe:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "WAIT_BITLOCK_TO    ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN            ");
              break;
      }
      break;
    case 5:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "         LOOPBACK.");
      switch ( SubState ) {
          case 0:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ENTRY_S            ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ACTIVE_S           ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EXIT_S             ");
              break;
          case 3:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EXIT1              ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ENTRY_M            ");
              break;
          case 5:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ACTIVE_M           ");
              break;
          case 6:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EXIT_M             ");
              break;
          case 8:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SPEED_CHANGE       ");
              break;
          case 9:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SEND_EIOS          ");
              break;
          case 0xa:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SPEED_PREP_EXIT    ");
              break;
          case 0xb:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SPEED_IDLE_WAIT    ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN            ");
              break;
      }
      break;
    case 6:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "HOTRESET/PREP_DET.");
      switch ( SubState ) {
          case 0:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "HR_ENTRY/HOTRESET  ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "MAS                ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SLV                ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "TS1                ");
              break;
          case 5:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "TS2                ");
              break;
          case 6:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EIOS               ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN            ");
              break;
      }
      break;
    case 7:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "         DISABLED.");
      switch ( SubState ) {
          case 0:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "TS1                ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EIOS               ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EI                 ");
              break;
          case 3:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SUCC               ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SUCC_WAIT          ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN            ");
              break;
      }
      break;
    case 8:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "       EQRECOVERY.");
      switch ( SubState ) {
          case 0:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "PH0                ");
              break;
          case 1:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "PH1                ");
              break;
          case 2:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ADAPTMYTX          ");
              break;
          case 3:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ADAPTMYRXS1_W4REQALLLANES   ");
              break;
          case 4:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ADAPTMYRXS2_W4500NSTO       ");
              break;
          case 5:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ADAPTMYRXS3_W464KUITO       ");
              break;
          case 6:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ADAPTMYRXS4_W416KUITO       ");
              break;
          case 7:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ADAPTMYRXS5_CHK4DONEALLLANES");
              break;
          case 8:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ADAPTMYRXS6_W4FINALRXTRAIN  ");
              break;
          case 9:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "ADAPTMYRXS7_FINALRXTRAINTO  ");
              break;
          default:
              IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "UNKNOWN          ");
              break;
      }
      break;
    default:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "          UNKNOWN.                   ");
      break;
  }
}     

/**

    The purpose is dump information about the errors found in
    the receiver if any

    @param IioGlobalData        - Pointer to IioGlobalData
    @param IioIndex             - Index to Iio
    @param PortIndex            - Index to the Iio Port

    @retval None

**/
VOID
ReceiverErrors (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8 IioIndex,
  IN  UINT8 PortIndex
  )
{
  REUTENGLTRCON_IIO_DFX_DMI_STRUCT ReutenGltrCon_dmi;
  REUTERRRCV_IIO_DFX_DMI_STRUCT    ReutErrRcv_dmi;
  REUTERRRCV_IIO_DFX_STRUCT        ReutErrRcv_pxp;
  REUTENGLTRCON_IIO_DFX_STRUCT     ReutenGltrCon_pxp;
  UINT32 ErrCount = 0;
  UINT32 ErrOverFlow = 0;

  if(IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) {
    ReutenGltrCon_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_REG);
    ReutenGltrCon_pxp.Bits.linkselect =  PortLinkSelect[PortIndex];
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_REG, ReutenGltrCon_pxp.Data);

    ReutErrRcv_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTERRRCV_IIO_DFX_REG);
    ErrCount = ReutErrRcv_pxp.Bits.rcverrcount;
    ErrOverFlow = ReutErrRcv_pxp.Bits.rcverroverflow;
  } else {
    ReutenGltrCon_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_DMI_REG);
    ReutenGltrCon_dmi.Bits.linkselect =  PortLinkSelect[PortIndex];
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_DMI_REG, ReutenGltrCon_dmi.Data);

    ReutErrRcv_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTERRRCV_IIO_DFX_DMI_REG);
    ErrCount = ReutErrRcv_dmi.Bits.rcverrcount;
    ErrOverFlow = ReutErrRcv_dmi.Bits.rcverroverflow;
  }

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Reciver Errors: 0x%x\n", ErrCount);
  if (ErrOverFlow) {
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Receiver Error Overflow!");
  }

}
 
/**

    The purpose is to clear all the errors in the receiver 

    @param IioGlobalData        - Pointer to IioGlobalData
    @param IioIndex             - Index to Iio
    @param PortIndex            - Index to the Iio Port

    @retval None

**/
VOID
ReceiverErrrorsClear (
  IN  IIO_GLOBALS    *IioGlobalData,
  IN  UINT8          IioIndex,
  IN  UINT8          PortIndex
  )
{
  REUTENGLTRCON_IIO_DFX_DMI_STRUCT ReutenGltrCon_dmi;
  REUTERRRCV_IIO_DFX_DMI_STRUCT    ReutErrRcv_dmi;
  REUTERRRCV_IIO_DFX_STRUCT        ReutErrRcv_pxp;
  REUTENGLTRCON_IIO_DFX_STRUCT     ReutenGltrCon_pxp;

  if(IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) {
    ReutenGltrCon_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_REG);
    ReutenGltrCon_pxp.Bits.linkselect =  PortLinkSelect[PortIndex];
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_REG, ReutenGltrCon_pxp.Data);

    ReutErrRcv_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTERRRCV_IIO_DFX_REG);
    ReutErrRcv_pxp.Bits.rcverrcount = 0x7FF;
    ReutErrRcv_pxp.Bits.rcverroverflow = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTERRRCV_IIO_DFX_REG, ReutErrRcv_pxp.Data);
  } else {
    ReutenGltrCon_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_DMI_REG);
    ReutenGltrCon_dmi.Bits.linkselect =  PortLinkSelect[PortIndex];
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_DMI_REG, ReutenGltrCon_dmi.Data);

    ReutErrRcv_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTERRRCV_IIO_DFX_DMI_REG);
    ReutErrRcv_dmi.Bits.rcverrcount = 0x7FF;
    ReutErrRcv_dmi.Bits.rcverroverflow = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTERRRCV_IIO_DFX_DMI_REG, ReutErrRcv_dmi.Data);
  }
}
     
/**

    The purpose is to setup the LTSSM logger 

    @param IioGlobalData        - Pointer to IioGlobalData
    @param IioIndex             - Index to Iio
    @param PortIndex            - Index to the Iio Port
    @param Stop                 - Flag to Stop or Start logger

    @retval None

**/
VOID
LoggerStartStop (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex,
  IN  UINT8                               PortIndex,
  IN  UINT8                               Speed,
  IN  UINT8                               Stop,
  IN  UINT8                               Mask
  )
{
  LTSSMLOGCTRL0_IIO_DFX_DMI_STRUCT    LtssmLogCtrl0_dmi;
  LTSSMLOGCTRL1_IIO_DFX_DMI_STRUCT    LtssmLogCtrl1_dmi;
  LTSSMLOGCTRL0_IIO_DFX_STRUCT        LtssmLogCtrl0_pxp;
  LTSSMLOGCTRL1_IIO_DFX_STRUCT        LtssmLogCtrl1_pxp;

  UINT8                               PortSel=0;
  /* ltslog_portsel - Bits[1:0], RWS_L, default = 2'b00 
       
       2'b00 select PORT=0
       2'b01 select PORT=1
       2'b10 select PORT=2
       2'b11 select PORT=3
       Notes:
       Locked by DBGBUSLCK
  */
  if ( PortIndex >= 1 ) {
       PortSel = (PortIndex - 1)%4;
  } else {
       PortSel = PortIndex%4;
  }


  if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) {
     // Select the port 
     LtssmLogCtrl0_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL0_IIO_DFX_REG);
     LtssmLogCtrl0_pxp.Bits.ltslog_portsel = PortSel;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL0_IIO_DFX_REG, LtssmLogCtrl0_pxp.Data);
     // turn off previous logger
     LtssmLogCtrl1_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG);
     LtssmLogCtrl1_pxp.Bits.ltslog_start32log = STOP_LOGGER;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG, LtssmLogCtrl1_pxp.Data);
     
     Stop = Stop | (Speed << 8);
     LtssmLogCtrl0_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG);
     LtssmLogCtrl0_pxp.Bits.ltslog_cur_stoplog = Stop;
     LtssmLogCtrl0_pxp.Bits.ltslog_stoplog = Stop;
     LtssmLogCtrl0_pxp.Bits.ltslog_cur_stoplog_bypass = 1;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL0_IIO_DFX_REG, LtssmLogCtrl0_pxp.Data);

     //setup other logger stuff
     LtssmLogCtrl1_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG);
     LtssmLogCtrl1_pxp.Bits.ltslog_stoplog_mask = STOPLOG_MASK;
     LtssmLogCtrl1_pxp.Bits.ltslog_mask_ltstates = Mask;
     //set start bit
     LtssmLogCtrl1_pxp.Bits.ltslog_start32log = START_LOGGER;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG, LtssmLogCtrl1_pxp.Data);

  } else {

     // turn off previous logger
     LtssmLogCtrl1_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_DMI_REG);
     LtssmLogCtrl1_dmi.Bits.ltslog_start32log = STOP_LOGGER;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_DMI_REG, LtssmLogCtrl1_dmi.Data);

     Stop = Stop | (Speed << 8);
     LtssmLogCtrl0_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL0_IIO_DFX_DMI_REG);
     LtssmLogCtrl0_dmi.Bits.ltslog_cur_stoplog = Stop;
     LtssmLogCtrl0_dmi.Bits.ltslog_stoplog = Stop;
     LtssmLogCtrl0_dmi.Bits.ltslog_cur_stoplog_bypass = 1;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL0_IIO_DFX_DMI_REG, LtssmLogCtrl0_dmi.Data);

     //setup other logger stuff
     LtssmLogCtrl1_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_DMI_REG);
     LtssmLogCtrl1_dmi.Bits.ltslog_stoplog_mask = STOPLOG_MASK;
     LtssmLogCtrl1_dmi.Bits.ltslog_mask_ltstates = Mask;
     //set start bit
     LtssmLogCtrl1_dmi.Bits.ltslog_start32log = START_LOGGER;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG, LtssmLogCtrl1_dmi.Data);
  }

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Start Logger in PortIndex = %x\n", PortIndex);
}

/**

    The purpose is to dump the LTSSM logger results  

    @param IioGlobalData        - Pointer to IioGlobalData
    @param IioIndex             - Index to Iio
    @param PortIndex            - Index to Iio Port

    @retval None

**/
VOID
LogSnapShot (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex,
  IN  UINT8                               PortIndex
  )
{
  LTSSMLOGCTRL0_IIO_DFX_DMI_STRUCT    LtssmLogCtrl0_dmi;
  LTSSMLOGCTRL1_IIO_DFX_DMI_STRUCT    LtssmLogCtrl1_dmi;
  LTSSMLOGCTRL2_IIO_DFX_DMI_STRUCT    LtssmLogCtrl2_dmi;
  LTSSMLOGDATA1_IIO_DFX_DMI_STRUCT    LtssmLogData1_dmi;
  LTSSMLOGCTRL0_IIO_DFX_STRUCT        LtssmLogCtrl0_pxp;
  LTSSMLOGCTRL1_IIO_DFX_STRUCT        LtssmLogCtrl1_pxp;
  LTSSMLOGCTRL2_IIO_DFX_STRUCT        LtssmLogCtrl2_pxp;
  LTSSMLOGDATA1_IIO_DFX_STRUCT        LtssmLogData1_pxp;
  UINT8   Pointer, Index, ExitArchFlag;
  UINT32  ExtendedLog, MainState, SubState, TimeStmp, Speed, L0ExitArc;
  UINT32  Trigger =0; 
  UINT32  G3DfmErrSts = 0;
        
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n");
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n===================================================================");
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n Socket %d Port %d ", IioIndex, PortIndex, IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device);
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "B:%x D:%x F:%x", IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex], IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device, IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function);

  if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) {
      LtssmLogCtrl0_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL0_IIO_DFX_REG);
      Trigger = LtssmLogCtrl0_pxp.Bits.ltslog_stoplog;

      LtssmLogCtrl1_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex,  LTSSMLOGCTRL1_IIO_DFX_REG);
      LtssmLogCtrl1_pxp.Bits.ltslog_start32log = STOP_LOGGER;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG, LtssmLogCtrl1_pxp.Data);
      G3DfmErrSts = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, G3DFRMERRSTATUS_IIO_DFX_REG);
      G3DfmErrSts = G3DFRMERR_MASK;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, G3DFRMERRSTATUS_IIO_DFX_REG, G3DfmErrSts);

  } else {
      LtssmLogCtrl0_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL0_IIO_DFX_DMI_REG);
      Trigger = LtssmLogCtrl0_dmi.Bits.ltslog_stoplog;

      LtssmLogCtrl1_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_DMI_REG);
      LtssmLogCtrl1_dmi.Bits.ltslog_start32log = STOP_LOGGER;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_DMI_REG, LtssmLogCtrl1_dmi.Data);
     
      G3DfmErrSts = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, G3DFRMERRSTATUS_IIO_DFX_DMI_REG);
      G3DfmErrSts = G3DFRMERR_MASK;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, G3DFRMERRSTATUS_IIO_DFX_DMI_REG, G3DfmErrSts);
  }

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n Trigger: 0x%x\t", Trigger);
  ReceiverErrors(IioGlobalData,IioIndex, PortIndex);
  ReceiverErrrorsClear(IioGlobalData, IioIndex, PortIndex);
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n===================================================================");
  ExitArchFlag = 0;
  Speed = 0;
  ExtendedLog = 0;
  for (Pointer = 0; Pointer < LOGGER_DEPTH; Pointer++) {
     Index = (LOGGER_DEPTH - 1) - Pointer;       
    if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) {
        LtssmLogCtrl2_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL2_IIO_DFX_REG);
        LtssmLogCtrl2_pxp.Bits.ltslog_rd_index = Index;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL2_IIO_DFX_REG, LtssmLogCtrl2_pxp.Data);

        ExtendedLog = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGDATA0_IIO_DFX_REG);

        LtssmLogData1_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGDATA1_IIO_DFX_REG);
        Speed = (LtssmLogData1_pxp.Bits.ltslog_log_entry) & 3;
        L0ExitArc = (LtssmLogData1_pxp.Bits.ltslog_log_entry >> 2) & 0xF;

    } else{
        LtssmLogCtrl2_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL2_IIO_DFX_DMI_REG);
        LtssmLogCtrl2_dmi.Bits.ltslog_rd_index = Index;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL2_IIO_DFX_REG, LtssmLogCtrl2_dmi.Data);

        ExtendedLog = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGDATA0_IIO_DFX_DMI_REG);

        LtssmLogData1_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGDATA1_IIO_DFX_DMI_REG);
        Speed = (LtssmLogData1_dmi.Bits.ltslog_log_entry) &3;
        L0ExitArc = (LtssmLogData1_dmi.Bits.ltslog_log_entry >> 2) & 0xF;
    }
    //readout the logger info
    MainState = (ExtendedLog & 0xF0) >> 4;
    SubState = ExtendedLog & 0x0F;
    TimeStmp = (ExtendedLog >> 8);
    TimeStmp = (2 * TimeStmp) / (1000);
        
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n%-2d Speed: [Gen%d] LTSSM [%x%x] = ",Index,Speed,MainState,SubState);
    DecodeLtssm(IioGlobalData,MainState,SubState);
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\tTime: %f us ",TimeStmp);
    if (ExitArchFlag) {
       IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EXIT ARC : ");
       DecodeLtssmArc(IioGlobalData, L0ExitArc);
       ExitArchFlag = 0;
    }

    if (MainState == 3 && SubState == 0 && Index !=0) {
        ExitArchFlag = 1;
    }
  } // For

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n===================================================================");

  if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) {
      G3DfmErrSts = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, G3DFRMERRSTATUS_IIO_DFX_REG);
  } else {
      G3DfmErrSts = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, G3DFRMERRSTATUS_IIO_DFX_DMI_REG);
  }
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n FE: 0x%x \n", G3DfmErrSts);

  if(IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) {
    LtssmLogCtrl1_pxp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG);
    LtssmLogCtrl1_pxp.Bits.ltslog_start32log = START_LOGGER;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_REG, LtssmLogCtrl1_pxp.Data);
  }else {
    LtssmLogCtrl1_dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_DMI_REG);
    LtssmLogCtrl1_dmi.Bits.ltslog_start32log = START_LOGGER;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMLOGCTRL1_IIO_DFX_DMI_REG, LtssmLogCtrl1_dmi.Data);
  }
}
/**
    Find the actual lane incase of lane reversal


    @param IioGlobalData        - Pointer to IioGlobalData
    @param IioIndex             - Index to Iio
    @param Lane                 - Current Lane

**/
UINT8
GetAbsLane(
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT8           IioIndex,
  IN  UINT8           PortIndex,
  IN  UINT8           Lane
  )
{
    PHYLTSSMDBG0_IIO_DFX_STRUCT PhyLtssm;
    UINT8 Abslane;
    UINT8 LaneOffset = 0;
    UINT8 Bifurcation = 0;
    
    PhyLtssm.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, PHYLTSSMDBG0_IIO_DFX_REG); //LTSSMDBG0_IIO_DFX_REG
    if ( PortIndex == 1 ) {   // P1
        LaneOffset = 4; 
    }

    if (PortIndex > 1) { // P2 - P15
        LaneOffset = ((PortIndex - 1)%4)*4;
    }
    
    if (PhyLtssm.Bits.reversalmode == 1) {
        Bifurcation = GetAssignedPortIndexWidth(IioGlobalData, IioIndex, PortIndex);
        Abslane = LaneOffset + (Bifurcation - Lane)- 1;
        //IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n LANE %d  abs%d  bif%d  laneoff%d  Port%d", Lane, Abslane, Bifurcation, LaneOffset, PortIndex);
    } else {
        Abslane = LaneOffset + Lane;
    }
   // IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n LANE %d  abs%d  bif%d  laneoff%d  Port%d", Lane, Abslane, Bifurcation, LaneOffset, PortIndex);
    return Abslane;
}



/**

 Gets and print rx stats per lane 
 
    @param IioGlobalData        - Pointer to IioGlobalData

    @retval                     - None
  

**/
VOID
ShowRxStats (
  IN  IIO_GLOBALS     *IioGlobalData
  )
{
    UINT8       IioIndex;
    UINT8       PortIndex;
    UINT8       k;
    UINT8       Lane;
    UINT8       Param;
    LNKSTS_IIO_PCIE_STRUCT    LnkSts;
    UINT32      Data32 = 0;
    UINT8       StatsList[19] = {32,31,37,38,33,34,35,36,11,10,7,6,9,8,30,1,27,12,5};
    
    for ( IioIndex = 0 ; IioIndex < MaxIIO ; IioIndex++ ) {
        if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
            continue;
        }
  
        //Unlock DFX hooks
        PcieUnlockDfx(IioGlobalData,IioIndex,0);    //0= unlock
          
        for ( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET ; PortIndex++) {
            if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
              continue;
            }

            if (PortIndex == 0 ) {
              LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIEDMI_REG);
            } else {
              LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG);
            }

            if ((LnkSts.Bits.negotiated_link_width == 1) || (LnkSts.Bits.negotiated_link_width == 2) || 
                (LnkSts.Bits.negotiated_link_width == 4) || (LnkSts.Bits.negotiated_link_width == 8) || 
                (LnkSts.Bits.negotiated_link_width == 16)){
            	IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n");
            	IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n Socket:%d, Port%d ", IioIndex, PortIndex);
            	IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, " Link up as x%d Gen%d", LnkSts.Bits.negotiated_link_width, LnkSts.Bits.current_link_speed);
            	IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n------------------------------------------");
                if ((LnkSts.Bits.negotiated_link_width > 0) & (PortIndex > 0)){ 
                   GetPcieJitterModeMargin(IioGlobalData, IioIndex, PortIndex, LnkSts.Bits.negotiated_link_width);
                }
                IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n Lane CTLEP   AGC   CDR CTOC1 CTOC2  DFE1  DFE2  DFE3  DFE4   ");
                IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DS0   DS1  ESM0  ESM1  ESP0  ESP1 ICOMP RVREF RXDCC SQLCH  SUMR");
               
                for (Lane = 0; Lane < LnkSts.Bits.negotiated_link_width; Lane++) {
                	IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n   %2d", Lane);
                    
                    Data32 = GetPcieCtle(IioGlobalData, IioIndex, PortIndex, Lane, LnkSts.Bits.current_link_speed);
                    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  %4d", Data32);
                    
                    for (k = 0; k < 19; ++k) {    
                        Param = StatsList[k];
                        if (EvParameterList[Param].busType == EVMONBUS) {
                            Data32 = GetPcieMonBus(IioGlobalData, IioIndex, PortIndex, Lane, Param);
                        } 
                        if (EvParameterList[Param].busType == EVTRACEBUS) {
                            Data32 = GetPcieTraceBus(IioGlobalData, IioIndex, PortIndex, Lane, Param);
                        }
                        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  %4d", Data32);
                    } // param loop for tracebus
                                            
                } //lane loop
                IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n");
            } //if link up check
            
        }
       
        // lock DFX 
        PcieUnlockDfx(IioGlobalData, IioIndex, 1);    //1= lock
    }
}

/**

 Gets and returns CTLE peak value
 
  @param IioGlobalData  - Pointer to IIO_GLOBALS
  @param IioIndex       - Index to IIO
  @param PortIndex      - Index to IIO Port
  @param Lane           - Lane
  @param Speed          - PortSpeed

  @retval CTLE peaking code

**/
UINT32
GetPcieCtle (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex,
  IN  UINT8         Lane,
  IN  UINT16        Speed
  )
{
    UINT8       AbsLane;
    UINT32      Ctlep = 0;

    AbsLane = GetAbsLane(IioGlobalData, IioIndex, PortIndex, Lane);
    
    //Gen1 CTLE
    switch (Speed){
        case 1:
            if (AbsLane < 12) { //bundles 0-5
                Ctlep = (IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_CTLE_PEAK_GEN1_0_IIO_DFX_REG) >> ((AbsLane/2)*5)) & 0x1f;
            } else {
                Ctlep = (IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_CTLE_PEAK_GEN1_1_IIO_DFX_REG) >> ((AbsLane - 12/2)*5)) & 0x1f;
            }
            break;
        case 2:
        //Gen2 CTLE
            if (AbsLane < 12) { //bundles 0-5
               Ctlep = (IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_CTLE_PEAK_GEN2_0_IIO_DFX_REG) >> ((AbsLane/2)*5)) & 0x1f;
            } else {
               Ctlep = (IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_CTLE_PEAK_GEN2_1_IIO_DFX_REG) >> ((AbsLane - 12/2)*5)) & 0x1f;
            }
            break;
        case 3:
        //Gen3 CTLE
            if (AbsLane < 12) { //bundles 0-5
                Ctlep = (IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_CTLE_PEAK_GEN3_0_IIO_DFX_REG) >> ((AbsLane/2)*5)) & 0x1f;
            } else {
                Ctlep = (IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_CTLE_PEAK_GEN3_1_IIO_DFX_REG) >> ((AbsLane - 12/2)*5)) & 0x1f;
            }
           break;
        default:
            Ctlep = 0;
            break;
    }
    
    //place holder for reading CTLE peaking code
    return Ctlep;
}


/**

 Gets and returns specified monbus parameter

  @param IoGlobaData            - POinter to IIO_GLOBALS
  @param IioIndex               - Index to Iio
  @param PortIndex              - Index to IIO port
  @param LinkWidth              - Link Width of current Port

  @retval  - ReadData

**/
VOID
GetPcieJitterModeMargin (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex,
  IN  UINT8               PortIndex,
  IN  UINT16              LinkWidth
  )
{
    UINT16      Peak=0;
    UINT16      Step=0;
    UINT16      Loop=0;
    UINT8       Lane;
    UINT8       StopOnError;
    UINT32      ErrCnt;
    UINT32      LinkSelect;
    UINT8      Stack;

    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n Jitter Margins");
 
    Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
    //setup link select for non DMI 
    if ( PortIndex > 0 ) {
    	LinkSelect = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, REUTENGLTRCON_IIO_DFX_REG) & 0xf9ffffff;
        LinkSelect |= (PortIndex % 4)<<25;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex,  REUTENGLTRCON_IIO_DFX_REG, LinkSelect);
    }
       
    for (Lane = 0; Lane < LinkWidth; Lane++) {
        Peak = 0;
        StopOnError = 0;
        while (( Peak < 64 ) & ( StopOnError == 0 ) ) {
            Peak += 1;
            //set peak/step/enable
            Step = Peak*8;
            if ( Step > 255 ) {
               Step = 255;
            }    

            SetCdrPmodPeakStep(IioGlobalData, IioIndex, PortIndex, Lane, Peak, Step);

            //clear errors

            IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack,  REUTERRCED_IIO_DFX_REG, 0xffff);
            IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack,  REUTERRRCV_IIO_DFX_REG, 0xffffffff);
            for (Loop = 0; Loop < 10; Loop++) {
                IioStall (IioGlobalData, 1000);   // dwell time 
                
                ErrCnt = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, REUTERRCED_IIO_DFX_REG) & 0x7fff;
                ErrCnt += IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, REUTERRRCV_IIO_DFX_REG) & 0x7fff;
                if (ErrCnt > 0){
                    //clear jitter enable
                    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CDR_PMOD_EN_IIO_DFX_REG, 0);
                    
                    //set jitters back to 0
                    SetCdrPmodPeakStep(IioGlobalData, IioIndex, PortIndex, Lane, 0, 0);

                    StopOnError = 1;
                    break;
                } //ErrCnt check
            } // Loopcount
         } // Peak loop
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n S%d:P%d:Lane%2d, Peak = %2d, Step = %3d, Loop= %2d", IioIndex, PortIndex, Lane, Peak, Step, Loop-100);
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack,  REUTERRCED_IIO_DFX_REG, 0xffff);
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack,  REUTERRRCV_IIO_DFX_REG, 0xffffffff);
    } //Lane loop
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\n ");
}


/**

 Gets and returns specified monbus parameter 
 
  @param IoGlobaData            - Pointer to IIO_GLOBALS
  @param IioIndex               - Index to Iio
  @param PortIndex              - Index to IIO port
  @param Lane
  @param peak
  @param step

  @retval                       - None
  

--*/
VOID
SetCdrPmodPeakStep (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT8           IioIndex,
  IN  UINT8           PortIndex,
  IN  UINT8           Lane,
  IN  UINT16          Peak,
  IN  UINT16          Step
  )
{
    UINT8   AbsLane;
    UINT8   Stack;

    Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
    AbsLane = GetAbsLane(IioGlobalData, IioIndex, PortIndex, Lane);

    //set peak
    if (AbsLane < 10){
       IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CDR_PMOD_PEAK_0_IIO_DFX_REG, (Peak <<(6*(AbsLane/2))));
    } else {
       IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CDR_PMOD_PEAK_1_IIO_DFX_REG, (Peak <<(6*((AbsLane - 10)/2))));
    }
    
    //set step
    if (AbsLane < 8){
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CDR_PMOD_STEP_0_IIO_DFX_REG, (Step << (8*(AbsLane/2))));
    } else {
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, CDR_PMOD_STEP_1_IIO_DFX_REG, (Step << (8*((AbsLane-8)/2))));
    }

    //set enable bits
    if (( Peak > 0 ) & ( Step > 0 )) {
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, CDR_PMOD_EN_IIO_DFX_REG,(1 << AbsLane));
    }        
}

/**

 Gets and returns specified monbus parameter
 
  @param IoGlobaData            - Pointer to IIO_GLOBALS
  @param IioIndex               - Index to Iio
  @param PortIndex              - Index to IIO port
  @param Lane                   - Current Lane
  @param Param

  @retval ReadData

**/
UINT32
GetPcieMonBus (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex,
  IN  UINT8         Lane,
  IN  UINT32        Param
  )
{
    UINT32      ReadData = 0;
    UINT32      LbCtrl = 0;
    UINT8       LoadSel,Lsb,Msb,RegMask;
     
    LoadSel = EvParameterList[Param].loadSel;
    Lsb = EvParameterList[Param].lsb;
    Msb = EvParameterList[Param].msb;
    RegMask = (1<<(Msb-Lsb+1))-1;
    
    Lane = GetAbsLane(IioGlobalData, IioIndex, PortIndex, Lane);
    

    //set
    LbCtrl = LbCtrl | BIT0 | BIT1;      // set lbc_req, and lbc_start
    LbCtrl = LbCtrl | LoadSel << 3;       // sets the loadsel
    
    IioWriteCpuCsr32(IioGlobalData,IioIndex,PortIndex, LBC_PER_IOU_DATA_IIO_DFX_REG, 0);
    LbCtrl = LbCtrl | 1<<(11 + (Lane^1));
    IioWriteCpuCsr32(IioGlobalData,IioIndex,PortIndex, LBC_PER_IOU_CONTROL_IIO_DFX_REG, 0);
    IioStall (IioGlobalData, 500); 
    ReadData = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LBC_PER_IOU_DATA_IIO_DFX_REG);
     
    //AGC, CDR, DFE1, DFE2, DFE3, DFE4, DS0, DS1, ESM0, ESM1, ESP0, ESP1, ICOMP, RVREF, RXDCC, SQLCH, SUMR 
    if ((LoadSel != 0x23) & (LoadSel != 0x21)) {
        ReadData = ReadData>>1;
    }
   
    ReadData = ReadData >> Lsb & RegMask;
    // need to adjust DFXs here
  
    return ReadData; 
}

/**

 Gets and returns specified tracebus parameter
 
  @param IoGlobaData            - Pointer to IIO_GLOBALS
  @param IioIndex               - Index to Iio
  @param PortIndex              - Index to IIO port
  @param Lane                   - Current Lane
  @param Param

  @retval ReadData

**/
UINT32
GetPcieTraceBus (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex,
  IN  UINT8         Lane,
  IN  UINT32        Param
  )
{
    UINT32      ReadData = 0;
    UINT8       TraceEn = 0;
    UINT32      TraceCtl = 0;
    UINT32      ModuleSel;
    UINT32      BndlSel = 0;
    UINT32      LaneSel = 0;
    UINT8       LoadSel,Lsb,Msb,RegMask; 
    UINT8       Stack;
       
    LoadSel = EvParameterList[Param].loadSel;
    Lsb = EvParameterList[Param].lsb;
    Msb = EvParameterList[Param].msb;
    RegMask = (1<<(Msb - Lsb + 1))-1;
    
     
    Lane = GetAbsLane(IioGlobalData, IioIndex, PortIndex, Lane);

    
    ModuleSel = 2*((PortIndex/4)%2);   
    Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
    IioWriteCpuCsr8(IioGlobalData, IioIndex, Stack, IIO_IIOS_CLK_GATE_OVR_IIO_DFX_GLOBAL_REG, 0x1);
    
    //zero out traceSel and modulesel
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CC_DFX_MON1_IIO_DFX_GLOBAL_REG, 0);
    
    //set DFX lane sel
    LaneSel = (Lane+1)%2;        
    LaneSel = LaneSel<<30;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CC_DFX_MON0_IIO_DFX_GLOBAL_REG, LaneSel);
    
    BndlSel = 1<<(Lane/2);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFX_BDL_SEL_IIO_DFX_REG, BndlSel);
    
    //setup tracebus sel
    TraceCtl = ModuleSel<<6;
    TraceCtl |= LoadSel;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CC_DFX_MON1_IIO_DFX_GLOBAL_REG, TraceCtl);

    // toggle the traceselect enable bit
    TraceEn =IioReadCpuCsr8(IioGlobalData, IioIndex, Stack, IIO_TRACE_DATA_CNTL_IIO_DFX_GLOBAL_REG);
    IioWriteCpuCsr8(IioGlobalData, IioIndex, Stack, IIO_TRACE_DATA_CNTL_IIO_DFX_GLOBAL_REG, (TraceEn | BIT2));
    IioStall (IioGlobalData, 500); 
    IioWriteCpuCsr8(IioGlobalData, IioIndex, Stack, IIO_TRACE_DATA_CNTL_IIO_DFX_GLOBAL_REG, (TraceEn & ~BIT2));
    IioStall (IioGlobalData, 500);     
   
   //read the data out of tracebus    
    ReadData = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIO_TRACE_DATA_0_IIO_DFX_GLOBAL_REG);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, DFX_BDL_SEL_IIO_DFX_REG, 0);
    
    ReadData = ReadData>>Lsb & RegMask;
    
    // need to adjust DFEs here

    if ((Param == 18) | (Param == 33)){
        ReadData = -1*ReadData;
    }
    if ((Param == 19) | (Param == 34)){
        ReadData = 31 - ReadData;
    }
    if ((Param == 20) | (Param == 35) | (Param == 21) | (Param == 36)){
        ReadData = 15 - ReadData;
    }
   
    //massage and mask data before return here
     
    
    return ReadData; 
}
