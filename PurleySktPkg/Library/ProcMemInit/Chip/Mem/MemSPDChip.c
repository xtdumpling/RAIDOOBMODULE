//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.03
//   Bug Fix:  Log/Show MRC error/warning by major code (refer Intel Purley MRC Error Codes_draft_0.3.xlsx)
//   Reason:
//   Auditor:  Jimmy Chiu
//   Date:     Jun/05/2017
//
//  Rev. 1.02
//   Bug Fix:  Add tCCD_L Relaxation setup option for specific DIMMs.
//   Reason:   Requested by Micron, code reference from Jian.
//   Auditor:  Jacker Yeh
//   Date:     Mar/17/2017
//
//  Rev. 1.01
//   Bug Fix:  Add NVDIMM Vender ID.
//   Reason:  
//   Auditor:  Leon Xu
//   Date:     Feb/27/2017
//
//  Rev. 1.00
//      Bug Fixed:  add RC error log to BMC and report error DIMM To screen(refer to Grantley)
//      Reason:     
//      Auditor:    Timmy Wang
//      Date:       May/26/2016
//
//*****************************************************************************


/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains memory detection and initialization for
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysHostChip.h"
#include "MemApiSkx.h"
#include "FnvAccess.h"
#include "RcRegs.h"
#include "CpuCsrAccessDefine.h"
#include "CpuPciAccess.h"

extern  UINT8 primaryWidth[MAX_TECH]; // Primary SDRAM device width
extern  UINT8 rowBits[MAX_TECH];      // Number of row address bits
extern  UINT8 columnBits[MAX_TECH];   // Number of column address bits
extern  UINT8 internalBanks[MAX_TECH];// Number of internal SDRAM banks
extern  UINT8 primaryWidthDDR4[MAX_TECH];   // Primary SDRAM device width
extern  UINT8 rowBitsDDR4[MAX_TECH];        // Number of row address bits
extern  UINT8 columnBitsDDR4[MAX_TECH];     // Number of column address bits
extern  UINT8 bankGroupsDDR4[MAX_TECH];     // Number of internal SDRAM banks

// Prototypes
static UINT32 GatherSPDDataDDR4 (PSYSHOST host);
static UINT8  GetTechIndex(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);
static void   CheckPORMixingWithinSocket(PSYSHOST host);                                                       // LOCAL
static UINT8  PopulateDimmTypeMask(PSYSHOST host, UINT8 socket);                                                // LOCAL

#define UDIMM_SOCKET                BIT0
#define RDIMM_SOCKET                BIT1
#define LRDIMM_STANDARD_SOCKET      BIT2
#define LRDIMM_3DS_SOCKET           BIT3
#define LRDIMM_AEP_SOCKET           BIT4
#define RDIMM_3DS_SOCKET            BIT5

#define SPD_CRC_DATA_COUNT              126   // Except SPD CRC Data
#define SPD_BASE_CONFIGURATION_BASE     0
#define SPD_BASE_BLOCK1                 128
#define SPD_MANUFACTURING_INFO_BASE     320

BOOLEAN
IsDDR4CRCValid (
  PSYSHOST      host,
  UINT8         socket,
  UINT8         ch,
  UINT8         dimm
  )
{
  UINT8   SPDData[SPD_CRC_DATA_COUNT];
  UINT16  spdCRC;
  UINT16  calculatedCRC;
  UINT8   spdIndex;

  MemSetLocal ((VOID *) SPDData, 0x00, SPD_CRC_DATA_COUNT);

  //
  // Read SPD Base configuration and DRAM Parameter Data [0:125]
  //
  for (spdIndex = 0; spdIndex < SPD_CRC_DATA_COUNT; spdIndex++){
    ReadSpd(host, socket, ch, dimm, (SPD_BASE_CONFIGURATION_BASE + spdIndex), &SPDData[spdIndex]);
  } // spdIndex

  //
  // Read SPD Base configuration and DRAM Parameter CRC Data [126:127]
  //
  ReadSpd(host, socket, ch, dimm, SPD_CRC_LSB, (UINT8*)&spdCRC);
  ReadSpd(host, socket, ch, dimm, SPD_CRC_MSB, (UINT8*)&spdCRC + 1);

  calculatedCRC = Crc16 ((char*)&SPDData, SPD_CRC_DATA_COUNT);

  //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
  //              "IsDDR4CRCValid: Base configuration and DRAM Parameter SpdCRC = 0x%x, CalculatedCRC = 0x%x\n", SpdCRC, CalculatedCRC));

  if (calculatedCRC != spdCRC) {
    return FALSE;
  }

  //
  // Get ready for block 1
  //
  MemSetLocal ((VOID *) SPDData, 0x00, SPD_CRC_DATA_COUNT);

  //
  // Read SPD data from block 1
  //
  for (spdIndex = 0; spdIndex < SPD_CRC_DATA_COUNT; spdIndex++) {
    ReadSpd(host, socket, ch, dimm, (SPD_BASE_BLOCK1 + spdIndex), &SPDData[spdIndex]);
  } // spdIndex

  //
  // Read SPD Block 1 CRC
  //
  ReadSpd(host, socket, ch, dimm, SPD_CRC_LSB_BLOCK1, (UINT8*)&spdCRC);
  ReadSpd(host, socket, ch, dimm, SPD_CRC_MSB_BLOCK1, (UINT8*)&spdCRC + 1);

  calculatedCRC = Crc16 ((char*)&SPDData, SPD_CRC_DATA_COUNT);

  if (calculatedCRC != spdCRC) {
    return FALSE;
  }

  return TRUE;

} // IsDDR4CRCValid

#ifdef SERIAL_DBG_MSG
/**

  Displays the DRAM manufacturer

  @param host  - Pointer to sysHost
  @param MfgId - DRAM Manufacturer Id

  @retval N/A

**/
void
DisplayManf (
  PSYSHOST host,
  UINT16   MfgId
  )
{
  switch (MfgId) {
  //
  // Aeneon
  //
  case MFGID_AENEON:
    rcPrintf ((host, "Aeneon  "));
    break;

  //
  // Qimonda
  //
  case MFGID_QIMONDA:
    rcPrintf ((host, "Qimonda "));
    break;

  //
  // NEC
  //
  case MFGID_NEC:
    rcPrintf ((host, "NEC     "));
    break;

  //
  // IDT
  //
  case MFGID_IDT:
    rcPrintf ((host, "IDT     "));
    break;

  //
  // TI
  //
  case MFGID_TI:
    rcPrintf ((host, "TI      "));
    break;

  //
  // Hynix
  //
  case MFGID_HYNIX:
    rcPrintf ((host, "Hynix   "));
    break;

  //
  // Micron
  //
  case MFGID_MICRON:
    rcPrintf ((host, "Micron  "));
    break;

  //
  // Infineon
  //
  case MFGID_INFINEON:
    rcPrintf ((host, "Infineon"));
    break;

  //
  // Samsung
  //
  case MFGID_SAMSUNG:
    rcPrintf ((host, "Samsung "));
    break;

  //
  // Tek
  //
  case MFGID_TEK:
    rcPrintf ((host, "Tek     "));
    break;

  //
  // Kingston
  //
  case MFGID_KINGSTON:
    rcPrintf ((host, "Kingston"));
    break;

  //
  // Elpida
  //
  case MFGID_ELPIDA:
    rcPrintf ((host, "Elpida  "));
    break;

  //
  // Smart
  //
  case MFGID_SMART:
    rcPrintf ((host, "Smart   "));
    break;

  //
  // Agilent
  //
  case MFGID_AGILENT:
    rcPrintf ((host, "Agilent "));
    break;

  //
  // Nanya
  //
  case MFGID_NANYA:
    rcPrintf ((host, "Nanya   "));
    break;

  //
  // Inphi
  //
  case MFGID_INPHI:
    rcPrintf ((host, "Inphi   "));
    break;

  //
  // Montage
  //
  case MFGID_MONTAGE:
    rcPrintf ((host, "Montage "));
    break;

  //
  // RAMBUS
  //
  case MFGID_RAMBUS:
    rcPrintf ((host, "Rambus  "));
    break;

//SMCPKG_SUPPORT > +
   //
  // Viking
  //
  case MFGID_VIKING:
    rcPrintf ((host, "Viking  "));
    break;
  //
  // Netlist
  //
  case MFGID_NETLIST:
    rcPrintf ((host, "Netlist  "));
    break;
  //
  // AGIGA
  //
  case MFGID_AGIGA:
    rcPrintf ((host, "Agiga  "));
    break;
  //
  // INNODISK
  //
  case MFGID_INNODISK:
    rcPrintf ((host, "Innodisk  "));
    break;	
//SMCPKG_SUPPORT < +

  default:
    rcPrintf ((host, "%04x    ", MfgId));
    break;
  }
  EmptyBlock(host, 3);
} // DisplayManf
#endif // SERIAL_DBG_MSG

UINT32
DetectSpdTypeDIMMConfig (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     keyByte
  )
{
  UINT8                   SPDRegTemp;
  UINT8                   SPDRegTemp1;
  UINT8                   SPDRegTemp2;
  UINT32                  Status = 1;
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  struct channelNvram     (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);
  host->nvram.mem.dramType = SPD_TYPE_DDR4;

  if (keyByte == SPD_TYPE_DDR4) {

    //
    // Check DDR4 CRC before using SPD Data. Mapout the channel if calculated CRC is not matched with CRC Data offset [126:127]
    //
    if(host->setup.mem.optionsExt & SPD_CRC_CHECK) {
      if(!IsDDR4CRCValid(host, socket, ch, dimm)) {

        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                      "Error : Invalid DIMM SPD contents!\n"));
        OutputError (host, ERR_DIMM_COMPAT, ERR_INVALID_DDR4_SPD_CONTENT, socket, ch, dimm, NO_RANK);
        DisableChannel(host, socket, ch);

        return Status;
      }
    }

    // for RDIMM
    ReadSpd(host, socket, ch, dimm, SPD_KEY_BYTE2, &SPDRegTemp);
    // read for manufacturer register
    ReadSpd (host, socket, ch, dimm, SPD_REG_VEN_MSB_DDR4, &SPDRegTemp1);
    // read for register revision number
    ReadSpd (host, socket, ch, dimm, SPD_REG_REV_DDR4, &SPDRegTemp2);

    // check RDIMM and TI-A6 register, ignore the dimm and raise an error and move on
    if ((SPDRegTemp == SPD_RDIMM) && (SPDRegTemp1 == (MFGID_TI >> 8)) && (SPDRegTemp2 == TI_SPD_REGREV_A6)) {
      // clear this info for this dimm, as not needed
      (*dimmNvList)[dimm].keyByte = 0x0;
      //
      // clear this info for this dimm, as not needed
      //
      (*dimmNvList)[dimm].dimmPresent = 0;

      //
      // decrement this value for this dimm, as not needed
      //
      (*channelNvList)[ch].maxDimm--;

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "DIMM is NOT supported\n"));
      return Status;
    } else {
      Status = DetectDDR4DIMMConfig (host, socket, ch, dimm);
    }
  } else if (keyByte == SPD_TYPE_AEP) {
    //  Check if CPU sku supports NVMDIMM
    host->nvram.mem.aepDimmPresent = 1;
    (*dimmNvList)[dimm].aepDimmPresent = 1;
    Status = DetectAepDIMMConfig (host, socket, ch, dimm);
  }

  return Status;
} // DetectSpdTypeDIMMConfig


UINT32
GatherSPDData (
  PSYSHOST  host
  )
/*++

      Calls to gather device specific SPDData

    @param host  - Point to sysHost

    @retval SUCCESS

  --*/
{
  GatherSPDDataDDR4 (host);
  return SUCCESS;
}

static UINT32
GatherSPDDataDDR4 (
  PSYSHOST host
  )
/*++

  Collects data from SPD for DDR4

  @param host  - Point to sysHost

  @retval SUCCESS

--*/
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               SPDReg;
  UINT8               ftbVal;
  UINT8               i;
  UINT16              cycles;
  UINT8               pageSize = 0;
  UINT8               ti;
  UINT32              casSup = 0;
  UINT16              tRC;
  UINT16              tRAS;
  UINT16              tRFC;
  UINT8               tFAW;
  UINT8               tWTRL;
  UINT8               tWTR;
  UINT8               tWR;
  UINT8               tRTP;
  UINT8               VlpRdimmPresent = 0;
  UINT8               reservedBits_SPDReg;
  UINT16              dateCode;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
#ifdef SERIAL_DBG_MSG
  UINT32              serialNum;
  UINT32              hiDwordSerialNum;
  struct dimmDevice   (*chdimmList)[MAX_DIMM];
#endif  // SERIAL_DBG_MSG

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // Don't access the SPD if not necessary
  //
  if ((host->nvram.mem.DataGood != 1) || ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot))) {

    //
    // Only gather SPD info if we don't already have it
    //
    channelNvList = GetChannelNvList(host, socket);

    // Check strap addresses on each channel moving near to far
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
#ifdef SERIAL_DBG_MSG
      chdimmList  = &host->var.mem.socket[socket].channelList[ch].dimmList;
#endif  // SERIAL_DBG_MSG

      //
      // Initialize to all CAS Latencies supported
      //
      (*channelNvList)[ch].common.casSup = 0xFFFFFFFF;

      //Initialize dimmrevtype to 1
      (*channelNvList)[ch].dimmRevType = RCD_REV_2;

      //
      // Initialize to ASR supported. This bit will get cleared if any DIMM on the channel does not support ASR
      //
      (*channelNvList)[ch].features |= ASR_SUPPORT;

      (*channelNvList)[ch].tCCDAdder = 0;
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        //
        // Get SPD Spec revision
        //
        ReadSpd (host, socket, ch, dimm, SPD_REVISION, &SPDReg);
        (*dimmNvList)[dimm].SPDSpecRev = SPDReg;

        if ((*dimmNvList)[dimm].aepDimmPresent == 0) {
          //
          // Lookup geometry in DIMM technology table to determine page size

          ti = GetTechIndex(host, socket, ch, dimm);
          switch (ti) {
          case 0:
          case 3:
          case 6:
          case 9:
            pageSize = PAGE_SIZE_HALFK; // 1/2K Page Size
            break;
          case 1:
          case 4:
          case 7:
          case 10:
            pageSize = PAGE_SIZE_1K;    // 1K Page Size
            break;
          case 2:
          case 5:
          case 8:
          case 11:
            pageSize = PAGE_SIZE_2K;    // 2K Page Size
            break;
          }

          tFAW = (UINT8)GettFAW(host, socket, pageSize);

          if ((*channelNvList)[ch].common.nFAW < tFAW) {
            (*channelNvList)[ch].common.nFAW = tFAW;
          }

          //
          // Use lookup table for tWTR_L
          //
          tWTRL = (UINT8)GettWTR(host, socket);

          if ((*channelNvList)[ch].common.nWTR_L < tWTRL) {
            (*channelNvList)[ch].common.nWTR_L = tWTRL;
          }

          //
          // Use lookup table for tWTR_S
          //
          tWTR = (UINT8)GettWTR_S(host, socket);

          if ((*channelNvList)[ch].common.nWTR < tWTR) {
            (*channelNvList)[ch].common.nWTR = tWTR;
          }

          //
          // Use lookup table for tWR
          //
          tWR = (UINT8)GettWR(host, socket);
          if ((*channelNvList)[ch].common.nWR < tWR) {
            (*channelNvList)[ch].common.nWR = tWR;
          }

          //
          // Use lookup table for tRTP
          //
          tRTP = (UINT8)GettRTP(host, socket);
          if ((*channelNvList)[ch].common.nRTP < tRTP) {
            (*channelNvList)[ch].common.nRTP = tRTP;
          }

          //
          // Get tRRD_S
          //
          ReadSpd (host, socket, ch, dimm, SPD_MIN_TRRDS_DDR4, &SPDReg);
          ReadSpd (host, socket, ch, dimm, SPD_FTB_TRRDS_DDR4, &ftbVal);

          if ((*channelNvList)[ch].common.tRRD < SPDReg) {
            (*channelNvList)[ch].common.tRRD = SPDReg;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, (UINT16)SPDReg, ftbVal);
            if ((*channelNvList)[ch].common.nRRD < cycles) {
              (*channelNvList)[ch].common.nRRD = (UINT8)cycles;
            }
          }

          //
          // Get tRRD_L

          ReadSpd (host, socket, ch, dimm, SPD_MIN_TRRDL_DDR4, &SPDReg);
          ReadSpd (host, socket, ch, dimm, SPD_FTB_TRRDL_DDR4, &ftbVal);

          if ((*channelNvList)[ch].common.tRRD_L < SPDReg) {
            (*channelNvList)[ch].common.tRRD_L = SPDReg;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, (UINT16)SPDReg, ftbVal);
            if ((*channelNvList)[ch].common.nRRD_L < cycles) {
              (*channelNvList)[ch].common.nRRD_L = (UINT8)cycles;
            }
          }
#if SMCPKG_SUPPORT
           //
          // Get tCCD_L

          ReadSpd (host, socket, ch, dimm, SPD_MIN_TCCDL_DDR4, &SPDReg);
          ReadSpd (host, socket, ch, dimm, SPD_FTB_TCCDL_DDR4, &ftbVal);

          if ((*channelNvList)[ch].common.tCCD_L < SPDReg) {
            (*channelNvList)[ch].common.tCCD_L = SPDReg;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, (UINT16)SPDReg, ftbVal);
            if ((*channelNvList)[ch].common.nCCD_L < cycles) {
              (*channelNvList)[ch].common.nCCD_L = (UINT8)cycles;
            }
          }
#endif
        //
        // Get tRP
        //
          ReadSpd (host, socket, ch, dimm, SPD_MIN_TRP_DDR4, &SPDReg);
          ReadSpd (host, socket, ch, dimm, SPD_FTB_TRP_DDR4, &ftbVal);
#ifdef  SERIAL_DBG_MSG
          (*chdimmList)[dimm].tRP = SPDReg;
#endif  // SERIAL_DBG_MSG
          (*channelNvList)[ch].dimmList[dimm].tRP = SPDReg;
          if ((*channelNvList)[ch].common.tRP < SPDReg) {
            (*channelNvList)[ch].common.tRP = SPDReg;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, (UINT16)SPDReg, ftbVal);

            if ((*channelNvList)[ch].common.nRP < cycles) {
              (*channelNvList)[ch].common.nRP = (UINT8)cycles;
            }
          }

          //
          // Get upper nibble of tRC and tRAS
          //
          ReadSpd (host, socket, ch, dimm, SPD_EXT_TRC_TRAS_DDR4, &SPDReg);
          tRC = (UINT16) (SPDReg & 0xF0) << 4;
          tRAS = (UINT16) (SPDReg & 0x0F) << 8;

          //
          // Get lower byte of tRAS
          //

          ReadSpd (host, socket, ch, dimm, SPD_MIN_TRAS_DDR4, &SPDReg);
          tRAS = tRAS | (UINT16) SPDReg;
          if ((*channelNvList)[ch].common.tRAS < tRAS) {
            (*channelNvList)[ch].common.tRAS = tRAS;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, tRAS, 0);
            if ((*channelNvList)[ch].common.nRAS < cycles) {
              (*channelNvList)[ch].common.nRAS = (UINT8)cycles;
            }
          }

          //
          // Get lower byte of tRC
          //
          ReadSpd (host, socket, ch, dimm, SPD_MIN_TRC_DDR4, &SPDReg);
          ReadSpd (host, socket, ch, dimm, SPD_FTB_TRC_DDR4, &ftbVal);
          tRC = tRC | (UINT16) SPDReg;
          if ((*channelNvList)[ch].common.tRC < tRC) {
            (*channelNvList)[ch].common.tRC = tRC;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, tRC, ftbVal);
            if ((*channelNvList)[ch].common.nRC < cycles) {
              (*channelNvList)[ch].common.nRC = (UINT8)cycles;
            }
          }

          //
          // Get tRCD
          //
          ReadSpd (host, socket, ch, dimm, SPD_MIN_TRCD_DDR4, &SPDReg);
          ReadSpd (host, socket, ch, dimm, SPD_FTB_TRCD_DDR4, &ftbVal);
#ifdef  SERIAL_DBG_MSG
          (*chdimmList)[dimm].tRCD = SPDReg;
#endif  // SERIAL_DBG_MSG
          (*channelNvList)[ch].dimmList[dimm].tRCD = SPDReg;
          if ((*channelNvList)[ch].common.tRCD < SPDReg) {
            (*channelNvList)[ch].common.tRCD = SPDReg;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, (UINT16)SPDReg, ftbVal);

            if ((*channelNvList)[ch].common.nRCD < cycles) {
              (*channelNvList)[ch].common.nRCD = (UINT8)cycles;
            }
          }

          //
          // Get upper byte of tRFC1
          //
          ReadSpd (host, socket, ch, dimm, SPD_MIN_TRFC1_LSB_DDR4, &SPDReg);
          tRFC = (UINT16) SPDReg;

          //
          // Get lower byte of tRFC1
          //
          ReadSpd (host, socket, ch, dimm, SPD_MIN_TRFC1_MSB_DDR4, &SPDReg);
          tRFC = tRFC | ((UINT16) SPDReg << 8);

          if ((*channelNvList)[ch].common.tRFC < tRFC) {
            (*channelNvList)[ch].common.tRFC = tRFC;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, tRFC, 0);
            if ((*channelNvList)[ch].common.nRFC < cycles) {
              (*channelNvList)[ch].common.nRFC = cycles;
            }
          }
          //
          // nXS is the amount of time needed after CKE is asserted before
          // a refresh command can be sent to the DIMMs that are exiting
          // self refresh.
          //
          // nXS = ((common.tRFC  * MTB * 1000) + (10ns * 1000000)) /tCK
          //
          (*channelNvList)[ch].common.nXS = (*channelNvList)[ch].common.nRFC + (UINT16)GettXSOFFSET(host, socket);

          //
          // Get tAA
          //
          ReadSpd (host, socket, ch, dimm, SPD_MIN_TAA_DDR4, &SPDReg);
          ReadSpd (host, socket, ch, dimm, SPD_FTB_TAA_DDR4, &ftbVal);
#ifdef  SERIAL_DBG_MSG
          (*chdimmList)[dimm].tCL = SPDReg;
#endif  // SERIAL_DBG_MSG
          (*channelNvList)[ch].dimmList[dimm].tCL = SPDReg;
          if ((*channelNvList)[ch].common.tCL < SPDReg) {
            (*channelNvList)[ch].common.tCL = SPDReg;

            // Convert to number of cycles
            cycles = TimeBaseToDCLK(host, socket, ch, dimm, (UINT16)SPDReg, (UINT16)ftbVal);

            if ((*channelNvList)[ch].common.nCL < cycles) {
              (*channelNvList)[ch].common.nCL = (UINT8)cycles;
            }
          }

          //
          // Get byte 4 of casSup
          //
          ReadSpd (host, socket, ch, dimm, SPD_CAS_LT_SUP_4_DDR4, &SPDReg);
          SPDReg = SPDReg & 0xBF; //mask out reserved bit 6
          casSup = (UINT32)(SPDReg << 24);
          if (SPDReg & (1<<7)) (*channelNvList)[ch].common.casSupRange = HIGH_CL_RANGE;

          //
          // Get byte 3 of casSup
          //
          ReadSpd (host, socket, ch, dimm, SPD_CAS_LT_SUP_3_DDR4, &SPDReg);
          casSup |= (UINT32)(SPDReg << 16);

          //
          // Get byte 2 of casSup
          //
          ReadSpd (host, socket, ch, dimm, SPD_CAS_LT_SUP_2_DDR4, &SPDReg);
          casSup |= (UINT32)(SPDReg << 8);

          //
          // Get byte 1 of casSup
          //
          ReadSpd (host, socket, ch, dimm, SPD_CAS_LT_SUP_1_DDR4, &SPDReg);
          casSup |= (UINT32)SPDReg;
          //
          // Mask off the unsupported CLs
          //
          (*channelNvList)[ch].common.casSup &= casSup;

          //
          // Get DIMM thermal sensor
          //
          ReadSpd(host, socket, ch, dimm, SPD_DIMM_TS_DDR4, &reservedBits_SPDReg);
          (*dimmNvList)[dimm].dimmTs = reservedBits_SPDReg & 0x80; //mask out reserved bits 6-0

          //
          // Get SDRAM type
          //
          ReadSpd (host, socket, ch, dimm, SPD_SDRAM_TYPE_DDR4, &reservedBits_SPDReg);
          (*dimmNvList)[dimm].sdramType = reservedBits_SPDReg & 0xF3;  //mask out reserved bits 3-2

          //
          // Get SDRAM optional features
          //
          ReadSpd (host, socket, ch, dimm, SPD_OPT_FEAT_DDR4, &SPDReg);
          (*dimmNvList)[dimm].SPDOptionalFeature = SPDReg & 0x3F; //mask out reserved bits 6-7

          //if (((*dimmNvList)[dimm].SPDOptionalFeature & SPD_TRR_IMMUNE) == 0){
          //  rankNvList = GetRankNvList(host, socket, ch, dimm);
          //  for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          //    if ((*rankNvList)[rank].enabled == 0) continue;
          //    DisableRank(host, socket, ch, dimm, rank);
          //    OutputWarning (host, WARN_MEMTEST_DIMM_DISABLE, WARN_FPT_MINOR_MEM_TEST, socket, ch, dimm, rank);
          //  }
          //}
          ReadSpd (host, socket, ch, dimm, SPD_RFSH_OPT_DDR4, &SPDReg);
          (*dimmNvList)[dimm].SPDThermRefsh = SPDReg & 0x0; //mask out reserved bits 7-0

          ReadSpd (host, socket, ch, dimm, SPD_OTH_OP_FEAT_DDR4, &SPDReg);
          (*dimmNvList)[dimm].SPDOtherOptFeatures = SPDReg & 0xE0; //mask out reserved bits 4-0

          //Get VendorID, DeviceID, RID, SubSystemVendorID, SubSystemDeviceID, and SubSystem RID from SPD
          ReadSpd (host, socket, ch, dimm, SPD_VENDORID_BYTE1, &SPDReg);
          (*dimmNvList)[dimm].VendorID = SPDReg;
          (*dimmNvList)[dimm].VendorID = (*dimmNvList)[dimm].VendorID << 8;
          ReadSpd (host, socket, ch, dimm, SPD_VENDORID_BYTE0, &SPDReg);
          (*dimmNvList)[dimm].VendorID = (*dimmNvList)[dimm].VendorID | SPDReg;

          ReadSpd (host, socket, ch, dimm, SPD_DEVICEID_BYTE1, &SPDReg);
          (*dimmNvList)[dimm].DeviceID = SPDReg;
          (*dimmNvList)[dimm].DeviceID = (*dimmNvList)[dimm].DeviceID << 8;
          ReadSpd (host, socket, ch, dimm, SPD_DEVICEID_BYTE0, &SPDReg);
          (*dimmNvList)[dimm].DeviceID = (*dimmNvList)[dimm].DeviceID | SPDReg;

           ReadSpd (host, socket, ch, dimm, SPD_RID, &SPDReg);
          (*dimmNvList)[dimm].RevisionID = SPDReg;

          ReadSpd (host, socket, ch, dimm, SPD_SUBSYSTEM_VENDORID_BYTE1, &SPDReg);
          (*dimmNvList)[dimm].subSysVendorID = SPDReg;
          (*dimmNvList)[dimm].subSysVendorID = (*dimmNvList)[dimm].subSysVendorID << 8;
          ReadSpd (host, socket, ch, dimm, SPD_SUBSYSTEM_VENDORID_BYTE0, &SPDReg);
          (*dimmNvList)[dimm].subSysVendorID = (*dimmNvList)[dimm].subSysVendorID | SPDReg;
          ReadSpd (host, socket, ch, dimm, SPD_SUBSYSTEM_DEVICEID_BYTE1, &SPDReg);
          (*dimmNvList)[dimm].subSysDeviceID = SPDReg;
          (*dimmNvList)[dimm].subSysDeviceID = (*dimmNvList)[dimm].subSysDeviceID << 8;
          ReadSpd (host, socket, ch, dimm, SPD_SUBSYSTEM_DEVICEID_BYTE0, &SPDReg);
          (*dimmNvList)[dimm].subSysDeviceID = (*dimmNvList)[dimm].subSysDeviceID | SPDReg;

          ReadSpd (host, socket, ch, dimm, SPD_SUBSYSTEM_REVISIONID, &SPDReg);
          (*dimmNvList)[dimm].subSysRevisionID = SPDReg;

        } else { // NVMDIMM

          if ((*dimmNvList)[dimm].SPDSpecRev <= 1){
            ReadSpd (host, socket, ch, dimm, SPD_AEP_THERMAL_SENSOR_R1, &SPDReg);
            (*dimmNvList)[dimm].dimmTs = SPDReg;

            ReadSpd (host, socket, ch, dimm, SPD_AEP_DEV_TYPE_R1, &SPDReg);
            (*dimmNvList)[dimm].sdramType = SPDReg;

            if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1600) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_1333_1600_R1, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg >> 4);
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_1866_2133_R1, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg & 0x0F);
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2133) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_1866_2133_R1, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg >> 4);
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_2400_2666_R1, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg & 0x0F);
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2666) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_2400_2666_R1, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg >> 4);
            } else {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_3200_R1, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg & 0x0F);
            }
          } else {
            ReadSpd (host, socket, ch, dimm, SPD_AEP_THERMAL_SENSOR, &SPDReg);
            (*dimmNvList)[dimm].dimmTs = SPDReg;

            ReadSpd (host, socket, ch, dimm, SPD_AEP_DEV_TYPE, &SPDReg);
            (*dimmNvList)[dimm].sdramType = SPDReg;

            if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1600) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_1333_1600, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg >> 4);
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_1866_2133, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg & 0x0F);
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2133) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_1866_2133, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg >> 4);
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_2400_2666, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg & 0x0F);
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2666) {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_2400_2666, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg >> 4);
            } else {
              ReadSpd (host, socket, ch, dimm, SPD_AEP_GNT2ERID_3200, &SPDReg);
              (*dimmNvList)[dimm].gnt2ERID = (SPDReg & 0x0F);
            }
          }


        }
        //
        // Get module mfg id from SPD 320-321.
        //
        ReadSpd (host, socket, ch, dimm, SPD_MMID_MSB_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDMMfgId = (UINT16) (SPDReg << 8);
        ReadSpd (host, socket, ch, dimm, SPD_MMID_LSB_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDMMfgId |= (UINT16) (SPDReg &~BIT7);

        //
        // Get module module mfg location from SPD 322
        //
        ReadSpd (host, socket, ch, dimm,  SPD_MM_LOC_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDMMfLoc = SPDReg;

        //
        // Get module module date code from SPD 323-324.
        //
        ReadSpd (host, socket, ch, dimm, SPD_MM_DATE_YR_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDModDate = (UINT16) (SPDReg);
        ReadSpd (host, socket, ch, dimm, SPD_MM_DATE_WK_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDModDate |= (UINT16) (SPDReg << 8);

        //
        // Get module Serial Number from SPD 325-328
        //
        for (i = 0; i < 4; i++) {
          ReadSpd (host, socket, ch, dimm, SPD_MODULE_SN_DDR4 + i, &SPDReg);
          (*dimmNvList)[dimm].SPDModSN[i] = SPDReg;
        }

        //
        // Get module part number from SPD 329-348.
        //
        for (i = 0; i < SPD_MODULE_PART_DDR4; i++) {
          ReadSpd (host, socket, ch, dimm, SPD_MODULE_PN_DDR4 + i, &SPDReg);
          (*dimmNvList)[dimm].SPDModPartDDR4[i] = SPDReg;
        }

        // Module Revision Code from SPD 349.
        ReadSpd (host, socket, ch, dimm, SPD_MODULE_RC_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDModRevCode = SPDReg;

#ifdef  SERIAL_DBG_MSG
        //
        // Get Intel DIMM serial number from SPD 507-511.
        //
        for (i = 0; i < 5; i++) {
          ReadSpd (host, socket, ch, dimm, SPD_INTEL_ID_DDR4 + i, &SPDReg);
          (*dimmNvList)[dimm].SPDIntelSN[i] = SPDReg;
        }
#endif

        //
        // Get DRAM mfg id from SPD 350-351.
        //
        ReadSpd (host, socket, ch, dimm, SPD_DRAM_MIDC_MSB_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDDramMfgId = (UINT16) (SPDReg << 8);
        ReadSpd (host, socket, ch, dimm, SPD_DRAM_MIDC_LSB_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDDramMfgId |= (UINT16) (SPDReg &~BIT7);

        //
        // Get DRAM revision id from SPD 352.
        //
        ReadSpd (host, socket, ch, dimm, SPD_DRAM_REV_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDDramRev = SPDReg;

        //
        // Get MSB/LSB of 16-bit CRC from SPD 382-383
        //
        ReadSpd (host, socket, ch, dimm, SPD_CRC_MSB_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDCrcDDR4 = (UINT16) (SPDReg << 8);
        ReadSpd (host, socket, ch, dimm, SPD_CRC_LSB_DDR4, &SPDReg);
        (*dimmNvList)[dimm].SPDCrcDDR4 |= (UINT16) (SPDReg &~BIT7);

        //
        // Get SPD Spec revision
        //
        ReadSpd (host, socket, ch, dimm, SPD_REVISION, &SPDReg);
        (*dimmNvList)[dimm].SPDSpecRev = SPDReg;

        //
        // RDIMM specific bytes
        //
        if (host->nvram.mem.dimmTypePresent == RDIMM) {

          //
          // Register Output Drive Strength for Control
          //
          ReadSpd (host, socket, ch, dimm, SPD_REG_OD_CTL_DDR4, &SPDReg);
          (*dimmNvList)[dimm].SPDODCtl = SPDReg;

          //
          // Register Output Drive Strength for Clock
          //
          ReadSpd (host, socket, ch, dimm, SPD_REG_OD_CK_DDR4, &SPDReg);
          (*dimmNvList)[dimm].SPDODCk = SPDReg & 0x0F; //mask out reserved bits 7-4

          //
          // Get RDIMM/LRDIMM module attributes & check for mirrored rank addresses
          //
          if (!(*dimmNvList)[dimm].aepDimmPresent){
            ReadSpd (host, socket, ch, dimm, SPD_LRDIMM_ATTR_DDR4, &(*dimmNvList)[dimm].dimmAttrib);
            if ((*dimmNvList)[dimm].dimmAttrib & BIT4) {
              (*channelNvList)[ch].dimmRevType &= RCD_REV_2;
            } else {
              (*channelNvList)[ch].dimmRevType &= RCD_REV_1;
            }
            ReadSpd (host, socket, ch, dimm, SPD_ADD_MAPPING_DDR4, &SPDReg);
            (*dimmNvList)[dimm].SPDAddrMapp = SPDReg & 0x01; //mask out reserved bits 7-1
          } else {
            (*dimmNvList)[dimm].SPDAddrMapp = 0;
          }

          //for buffer generations
          (*dimmNvList)[dimm].lrbufGen = ((*dimmNvList)[dimm].dimmAttrib & 0xF0) >>4;

#ifdef  LRDIMM_SUPPORT
          //
          // LRDIMM specific bytes
          //
          if (IsLrdimmPresent(host, socket, ch, dimm)) {

            //
            // MSB of the register ID
            //
            ReadSpd (host, socket, ch, dimm, SPD_LRBUF_VEN_MSB_DDR4, &SPDReg);
            (*dimmNvList)[dimm].SPDRegVen = (UINT16) (SPDReg << 8);

            //
            // LSB of the register ID
            //
            ReadSpd (host, socket, ch, dimm, SPD_LRBUF_VEN_LSB_DDR4, &SPDReg);
            (*dimmNvList)[dimm].SPDRegVen |= (UINT16) (SPDReg &~BIT7);

            //
            // Register Revision
            //
            ReadSpd (host, socket, ch, dimm, SPD_LRBUF_REV_DDR4, &SPDReg);
            (*dimmNvList)[dimm].SPDRegRev = SPDReg;

            //
            // Data Buffer Revision
            //
            ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DB_REV_DDR4, &SPDReg);
            (*dimmNvList)[dimm].lrbufRid = SPDReg;

            if ((*dimmNvList)[dimm].aepDimmPresent) {
              //
              // LR Data Buffer VrefDQ for DRAM Interface
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DB_VREFDQ, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDbVrefdq = SPDReg + DB_DRAM_VREF_RANGE2_OFFSET;
              //
              // LR Data Buffer MDQ Drive Strength and RTT for data rate <= 1866
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DS_RTT_LE1866, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDbDsRttLe1866 = SPDReg;
              //
              // LR Data Buffer MDQ Drive Strength and RTT for data rate > 1866 and <= 2400
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DS_RTT_GT1866_LE2400, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDbDsRttGt1866Le2400 = SPDReg;
              //
              // LR Data Buffer MDQ Drive Strength and RTT for data rate > 2400 and <= 3200
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DS_RTT_GT2400_LE3200, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDbDsRttGt2400Le3200 = SPDReg;
              //
              // LR Buffer DRAM Drive Strength (for data rates <1866, 1866 < data rate < 2400, and 2400 < data rate < 3200)
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DRAM_DS, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramDs = SPDReg;
              //
              // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate <= 1866
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DRAM_ODT_WR_NOM_LE1866, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtWrNomLe1866 = SPDReg;
              //
              // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 1866 and <= 2400
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DRAM_ODT_WR_NOM_GT1866_LE2400, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtWrNomGt1866Le2400 = SPDReg;
              //
              // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 2400 and <= 3200
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DRAM_ODT_WR_NOM_GT2400_LE3200, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtWrNomGt2400Le3200 = SPDReg;
              //
              // LR Buffer DRAM ODT (RTT_PARK) for data rate <= 1866
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DRAM_ODT_PARK_LE1866, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtParkLe1866 = SPDReg;
              //
              // LR Buffer DRAM ODT (RTT_PARK) for data rate > 1866 and <= 2400
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DRAM_ODT_PARK_GT1866_LE2400, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtParkGt1866Le2400 = SPDReg;
              //
              // LR Buffer DRAM ODT (RTT_PARK) for data rate > 2400 and <= 3200
              //
              ReadSpd (host, socket, ch, dimm, SPD_AEP_LRBUF_DRAM_ODT_PARK_GT2400_LE3200, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtParkGt2400Le3200 = SPDReg;
            } else {

               //
              // Check if this LRDIMM is supported
              //
              if (!(host->var.common.emulation & SIMICS_FLAG)) { // W/A for simics HSD 5370385
                if (((*dimmNvList)[dimm].SPDSpecRev < 8) && (host->setup.mem.enforcePOR == ENFORCE_POR_EN) && ((*dimmNvList)[dimm].aepDimmPresent == 0)){
                  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                                "ERROR: You are using a non-production LRDIMM memory module that is not supported\n"));
                  DisableChannel(host, socket, ch);
                  OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_COMPAT_MINOR_NOT_SUPPORTED, socket, ch, dimm, 0);
                }
              }
              //
              // DRAM VrefDQ for Package Rank 0
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_VREFDQ_R0_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramVrefdqR0 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // DRAM VrefDQ for Package Rank 1
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_VREFDQ_R1_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramVrefdqR1 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // DRAM VrefDQ for Package Rank 2
               //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_VREFDQ_R2_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramVrefdqR2 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // DRAM VrefDQ for Package Rank 3
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_VREFDQ_R3_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramVrefdqR3 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // LR Data Buffer VrefDQ for DRAM Interface
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DB_VREFDQ_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDbVrefdq = SPDReg + DB_DRAM_VREF_RANGE2_OFFSET;
              //
              // LR Data Buffer MDQ Drive Strength and RTT for data rate <= 1866
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DB_DS_RTT_LE1866_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDbDsRttLe1866 = SPDReg;
              //
              // LR Data Buffer MDQ Drive Strength and RTT for data rate > 1866 and <= 2400
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DB_DS_RTT_GT1866_LE2400_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDbDsRttGt1866Le2400 = SPDReg;
              //
              // LR Data Buffer MDQ Drive Strength and RTT for data rate > 2400 and <= 3200
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DB_DS_RTT_GT2400_LE3200_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDbDsRttGt2400Le3200 = SPDReg;
              //
              // LR Buffer DRAM Drive Strength (for data rates <1866, 1866 < data rate < 2400, and 2400 < data rate < 3200)
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_DS_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramDs = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate <= 1866
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_ODT_WR_NOM_LE1866_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtWrNomLe1866 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 1866 and <= 2400
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_ODT_WR_NOM_GT1866_LE2400_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtWrNomGt1866Le2400 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 2400 and <= 3200
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_ODT_WR_NOM_GT2400_LE3200_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtWrNomGt2400Le3200 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // LR Buffer DRAM ODT (RTT_PARK) for data rate <= 1866
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_ODT_PARK_LE1866_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtParkLe1866 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // LR Buffer DRAM ODT (RTT_PARK) for data rate > 1866 and <= 2400
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_ODT_PARK_GT1866_LE2400_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtParkGt1866Le2400 = SPDReg & 0x3F; //mask out reserved bits 6-7
              //
              // LR Buffer DRAM ODT (RTT_PARK) for data rate > 2400 and <= 3200
              //
              ReadSpd (host, socket, ch, dimm, SPD_LRBUF_DRAM_ODT_PARK_GT2400_LE3200_DDR4, &SPDReg);
              (*dimmNvList)[dimm].SPDLrbufDramOdtParkGt2400Le3200 = SPDReg & 0x3F; //mask out reserved bits 6-7
            }

          } else
#endif
          // SPD bytes for RDIMM module
          {
            //
            // Get RDIMM module attributes
            //
            ReadSpd (host, socket, ch, dimm, SPD_RDIMM_ATTR_DDR4, &(*dimmNvList)[dimm].dimmAttrib);

            //
            // Get Heat spreader info
            //
            ReadSpd (host, socket, ch, dimm, SPD_DIMM_HS_DDR4, &(*dimmNvList)[dimm].dimmHs);

            //
            // MSB of the register ID
            //
            ReadSpd (host, socket, ch, dimm, SPD_REG_VEN_MSB_DDR4, &SPDReg);
            (*dimmNvList)[dimm].SPDRegVen = (UINT16) (SPDReg << 8);

            //
            // LSB of the register ID
            //
            ReadSpd (host, socket, ch, dimm, SPD_REG_VEN_LSB_DDR4, &SPDReg);
            (*dimmNvList)[dimm].SPDRegVen |= (UINT16) (SPDReg &~BIT7);

            //
            // Register Revision
            //
            ReadSpd (host, socket, ch, dimm, SPD_REG_REV_DDR4, &SPDReg);
            (*dimmNvList)[dimm].SPDRegRev = SPDReg;

            //
            // Check if this register is supported Gen1
            //
            if (((*dimmNvList)[dimm].lrbufGen == 0x0) && ((((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) && ((*dimmNvList)[dimm].SPDRegRev < IDT_SPD_REGREV_C0)) ||
                (((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) && ((*dimmNvList)[dimm].SPDRegRev < INPHI_SPD_REGREV_B0)) ||
                (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE) && ((*dimmNvList)[dimm].SPDRegRev < MONTAGE_SPD_REGREV_B1)) ||
                (((*dimmNvList)[dimm].SPDRegVen == MFGID_TI) && ((*dimmNvList)[dimm].SPDRegRev < TI_SPD_REGREV_A6)))) {

              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                            "ERROR: You are using a Gen1 non-production memory module that is not supported\n"));
              DisableChannel(host, socket, ch);
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x00    //SMCPKG_SUPPORT++         
              OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_COMPAT_MINOR_NOT_SUPPORTED, socket, ch, dimm, 0);
#endif    //SMCPKG_SUPPORT++           
            } // Supported register check

            //
            // Check if this register is supported Gen2
            //
            if (((*dimmNvList)[dimm].lrbufGen == 0x1) && ((((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) && ((*dimmNvList)[dimm].SPDRegRev < IDT_SPD_REGREV_GEN2_B0)) ||
                (((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) && ((*dimmNvList)[dimm].SPDRegRev < INPHI_SPD_REGREV_GEN2_C0)) ||
                (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE) && (((*dimmNvList)[dimm].SPDRegRev < MONTAGE_SPD_REGREV_GEN2_A0) && ((*dimmNvList)[dimm].SPDRegRev != MONTAGE_SPD_REGREV_GEN2_H0))) ||
                (((*dimmNvList)[dimm].SPDRegVen == MFGID_RAMBUS) && ((*dimmNvList)[dimm].SPDRegRev < RAMBUS_SPD_REGREV_GEN2_B1))
                )) {

              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                            "ERROR: You are using a Gen2 non-production memory module that is not supported\n"));
              DisableChannel(host, socket, ch);
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x00    //SMCPKG_SUPPORT++               
              OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_COMPAT_MINOR_NOT_SUPPORTED, socket, ch, dimm, 0);
#endif    //SMCPKG_SUPPORT++               
             } // Supported register check

          }

        } else {
          //
          // UDIMM/SODIMM specific bytes
          //
          //
          // Address Mapping from Edge connector to DRAM
          //
          if ((*dimmNvList)[dimm].aepDimmPresent){
            (*dimmNvList)[dimm].SPDAddrMapp = 0;
          } else {
            ReadSpd (host, socket, ch, dimm, SPD_ADDR_MAP_FECTD_DDR4, &SPDReg);
            SPDReg = SPDReg & 0x01; //mask out reserved bits 7-1
            (*dimmNvList)[dimm].SPDAddrMapp = SPDReg & BIT0;
          }
        }


        //
        // Set tCase for each DIMM based on temperature range supported
        //
        if ((*channelNvList)[ch].features & EXTENDED_TEMP) {
          (*dimmNvList)[dimm].dramTcaseMax = DRAM_TCASE_MAX;
        } else {
          (*dimmNvList)[dimm].dramTcaseMax = DRAM_TCASE_DEFAULT;
        }

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);

          //
          // Module part number
          //
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                         "DIMM Module Part Number: "));
          for (i = 0; i < 19; i++) {
            if ((*dimmNvList)[dimm].SPDModPartDDR4[i] >= 32 && (*dimmNvList)[dimm].SPDModPartDDR4[i] < 127) {
              rcPrintf ((host, "%c", (*dimmNvList)[dimm].SPDModPartDDR4[i]));
            } else {
              rcPrintf ((host, "(%d)", (*dimmNvList)[dimm].SPDModPartDDR4[i]));
            }
          }

          // New line
          rcPrintf ((host, "\n"));

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                         "Intel SN: "));
          //
          // Intel DIMM Serial Number
          //

          hiDwordSerialNum = 0;
          serialNum = 0;
          //
          // Intel DIMM Serial Number
          //
          hiDwordSerialNum = (UINT32)(*dimmNvList)[dimm].SPDIntelSN[0];
          for (i = 1; i < 5; i++) {
            serialNum |= (UINT32)(*dimmNvList)[dimm].SPDIntelSN[i] << ((4 - i) * 8);
          }
          rcPrintf ((host, "0x%08x%08x\n", hiDwordSerialNum, serialNum));

          releasePrintFControl (host);
        }
#endif

        //
        // Samsung SR RDIMMs older than 2013 WW01 are not supported
        //
        dateCode = (*dimmNvList)[dimm].SPDModDate >> 8;
        dateCode |= (*dimmNvList)[dimm].SPDModDate << 8;

        if (((*dimmNvList)[dimm].SPDDramMfgId == MFGID_SAMSUNG) && (host->nvram.mem.dimmTypePresent == RDIMM) &&
            ((*dimmNvList)[dimm].numRanks == 1) && (dateCode < 0x1301)) {

          for (i = 0; i < 10; i++) {
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                            "ERROR: Samsung SR RDIMM detected older than 2013 WW01. This DIMM is not supported!!!\n"));
          } // i loop
          DisableChannel(host, socket, ch);
          OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_COMPAT_MINOR_NOT_SUPPORTED, socket, ch, dimm, 0);
        }

        //SPD byte 128 check for VLP RDIMM
        if ((IsLrdimmPresent(host, socket, ch, dimm) == 0) && (host->nvram.mem.dimmTypePresent == RDIMM)) {
          ReadSpd (host, socket, ch, dimm, SPD_MODULE_NH_DDR4, &SPDReg);
          if ((SPDReg&0x1f) <= 0x4) {VlpRdimmPresent = 1;}
        }
        //Check for non-production DIMMs for 2666 and above
        if ((host->nvram.mem.socket[socket].ddrFreq >= DDR_2666) && ((*channelNvList)[ch].maxDimm == 2) && (!(*dimmNvList)[dimm].aepDimmPresent))  {
          if ((*dimmNvList)[dimm].sdramCapacity == SPD_4Gb)  {
            if (IsLrdimmPresent(host, socket, ch, dimm)) {
              if ((((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x43) && (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x21)) {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                                "WARNING: Non-production DIMMs detected!!!!\n"));
                OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP, socket, ch, dimm, 0);
              }
            }
            else if ((IsLrdimmPresent(host, socket, ch, dimm) == 0) && (host->nvram.mem.dimmTypePresent == RDIMM) && (VlpRdimmPresent ==0)) {
              if (((*dimmNvList)[dimm].SPDDramMfgId == MFGID_SAMSUNG) && (((*dimmNvList)[dimm].SPDRawCard & 0x1f) == RAW_CARD_D)) {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                                "WARNING: Non-production DIMMs detected!!!!\n"));
                OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP, socket, ch, dimm, 0);
              }
              if (((*dimmNvList)[dimm].SPDDramMfgId == MFGID_HYNIX) && ((*dimmNvList)[dimm].SPDModPartDDR4[8] != 0x41)) {
                if ((((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x41)) {
                  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                                  "WARNING: Non-production DIMMs detected!!!!\n"));
                  OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP, socket, ch, dimm, 0);
                }
              }
            }
          }
          else if ((*dimmNvList)[dimm].sdramCapacity == SPD_8Gb) {
            if (IsLrdimmPresent(host, socket, ch, dimm)) {
              if ((((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) != SPD_3DS_TYPE) && (((*dimmNvList)[dimm].SPDDramMfgId == MFGID_SAMSUNG) || ((*dimmNvList)[dimm].SPDDramMfgId == MFGID_HYNIX)) &&
                 ((((*dimmNvList)[dimm].SPDRawCard & 0x60) >> 5) < 0x2)) {
                if ((((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x1) && (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x21)) {
                  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                                "WARNING: Non-production DIMMs detected!!!!\n"));
                  OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP, socket, ch, dimm, 0);
                }
              }
              if ((((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) != SPD_3DS_TYPE) && ((*dimmNvList)[dimm].SPDDramMfgId == MFGID_MICRON) && (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x1) &&
                 (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x44) && (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x43) && (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x21)) {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "WARNING: Non-production DIMMs detected!!!!\n"));
                OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP, socket, ch, dimm, 0);
              }
            }
            else if ((IsLrdimmPresent(host, socket, ch, dimm) == 0) && (host->nvram.mem.dimmTypePresent == RDIMM) && (VlpRdimmPresent == 0)) {
              if ((((*dimmNvList)[dimm].SPDDramMfgId == MFGID_SAMSUNG) || ((*dimmNvList)[dimm].SPDDramMfgId == MFGID_MICRON)) && ((((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x23) &&
                 (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x42) && (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x22) && (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x24) && (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x21) &&
                 (((*dimmNvList)[dimm].SPDRawCard & 0x7f) != 0x41) && (((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) != SPD_3DS_TYPE))) {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                               "WARNING: Non-production DIMMs detected!!!!\n"));
                OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP, socket, ch, dimm, 0);
              }
              if (((*dimmNvList)[dimm].SPDDramMfgId == MFGID_HYNIX) && (((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) != SPD_3DS_TYPE) &&
                 ((*dimmNvList)[dimm].SPDModPartDDR4[8] != 0x41)) {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "WARNING: Non-production DIMMs detected!!!!\n"));
                OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP, socket, ch, dimm, 0);
              }
              if ((((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE) && ((((*dimmNvList)[dimm].SPDRawCard & 0x60) >> 5) < 0x2)) {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "WARNING: Non-production DIMMs detected!!!!\n"));
                OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP, socket, ch, dimm, 0);
              }
            }
          }
        }

        VlpRdimmPresent = 0;
#ifdef SERIAL_DBG_MSG
        if ((*dimmNvList)[dimm].aepDimmPresent){
            //prints for Buffer Gen, FM Controller, and Stepping to ease debugging
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Revision Code (Stepping) = 0x%x\n", (*dimmNvList)[dimm].fmcRev));
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "FMC Type: "));
            switch ((*dimmNvList)[dimm].fmcType) {
            case 0x79:
                MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "FNV\n"));
                break;
            case 0x7A:
                MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "EKV\n"));
                break;
            case 0x7B:
                MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "BWV\n"));
                break;
            default:
                MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "0x%x\n", (*dimmNvList)[dimm].fmcType));
            }
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Gen Buffer: Gen %d\n", ((*dimmNvList)[dimm].lrbufGen) + 1));
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Buffer Vendor: "));
            DisplayManf(host, (*dimmNvList)[dimm].SPDRegVen);
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n "));
        }
#endif // SERIAL_DBG_MSG
      } // dimm loop
       // MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
       //    "DIMM AEP=%d, LRDIMM=%d, 3DS=%d, DieCount=%d  \n",
       //   (*dimmNvList)[dimm].aepDimmPresent, IsLrdimmPresent(host, socket, ch, dimm), (*channelNvList)[ch].encodedCSMode, (*dimmNvList)[dimm].dieCount));
    } // ch loop
  }

  return SUCCESS;
} // GatherSPDDataDDR4

#ifdef SERIAL_DBG_MSG
static char *GetDimmTypeStr(UINT8 DimmType, char *strBuf) {

  switch (DimmType) {
  case SPD_UDIMM:
    StrCpyLocal(strBuf, "UDIMM");
    break;
  case SPD_RDIMM:
    StrCpyLocal(strBuf, "RDIMM");
    break;
  case SPD_LRDIMM_DDR4:
    StrCpyLocal(strBuf, "LRDIMM");
    break;
  case SPD_SODIMM:
    StrCpyLocal(strBuf, "SODIMM");
    break;
  default:
    StrCpyLocal(strBuf, "Unknown DIMM");
    break;
  }
  return strBuf;
}
#endif  // SERIAL_DBG_MSG

UINT32
CheckPORCompat (
  PSYSHOST host
  )
/*++


  Verifies current population does not validate POR restrictions

  @param host  - Pointer to sysHost

  @retval N/A

--*/
{
  UINT8                                   socket;
  UINT8                                   mcId;
  UINT8                                   ch;
  UINT8                                   dimm;
  INT32                                   minTCKcommon;
  UINT8                                   MemPresent;
  UINT8                                   chMemPresent;
  UINT8                                   mapOutCh;
  UINT8                                   DimmTypeStatus = 0;
  UINT8                                   DimmType;
  UINT8                                   emptySlot;
  UINT8                                   maxDimm;
  UINT8                                   ti;
#ifdef  LRDIMM_SUPPORT
  UINT8                                   numRanks;
  UINT8                                   numDramRanks;
#endif  //  LRDIMM_SUPPORT
  UINT8                                   ECCDimmSupport;
  struct dimmDevice                       (*chdimmList)[MAX_DIMM];
  struct dimmNvram                        (*dimmNvList)[MAX_DIMM];
  struct channelNvram                     (*channelNvList)[MAX_CH];
  struct smbDevice                        spd;
#ifdef SERIAL_DBG_MSG
  char                                    strBuf0[128];
  char                                    strBuf1[128];
#endif  // SERIAL_DBG_MSG
  IMC_FUSE_DOWNLOAD_SHADOW_MC_MAIN_STRUCT fuseShadow;
  DIMMMTR_0_MC_MAIN_STRUCT                dimmMtr;
  CAPID3_PCU_FUN3_STRUCT                  capId3;
  CAPID1_PCU_FUN3_STRUCT                  capId1;

  MemPresent      = 0;
  DimmType        = 0;

  //
  // Initialize common parts of the smbDevice structure for all SPD devices
  //
  spd.compId = SPD;
  spd.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  spd.address.deviceType = DTI_EEPROM;

  if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot) ) {
    //
    // Initialize ECC support to enabled
    //
    host->nvram.mem.eccEn = 1;
    //
    // Detect DIMMs on each Socket
    //
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      //
      // Loop for each CPU socket
      //
      if (host->nvram.mem.socket[socket].enabled == 0) continue;

      capId3.Data = ReadCpuPciCfgEx(host, socket, 0, CAPID3_PCU_FUN3_REG);
      capId1.Data = ReadCpuPciCfgEx(host, socket, PCU_INSTANCE_0, CAPID1_PCU_FUN3_REG);

      //
      // Disable ECC if option requires it
      //
      if (!(host->setup.mem.dfxMemSetup.dfxOptions & ECC_CHECK_EN)) {
        host->nvram.mem.eccEn = 0;
      }

      //
      // Initialize to 0
      //
      host->nvram.mem.socket[socket].maxDimmPop = 0;

      //
      // Save the original value
      //

      //
      // Initialize to 0
      //
      minTCKcommon  = 0;

      channelNvList = GetChannelNvList(host, socket);

      //
      // Detect DIMMs on each channel
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        //
        // Initialize to 0 for each channel
        //
        chMemPresent = 0;

        if ((*channelNvList)[ch].enabled == 0) continue;

        (*channelNvList)[ch].lrRankMultEnabled = 0;
        (*channelNvList)[ch].encodedCSMode = 0;
        (*channelNvList)[ch].cidBitMap = 0;

        //
        // Initialize the maximum number of DIMMs on this channel to 0
        //
        maxDimm = (*channelNvList)[ch].maxDimm;

        emptySlot = 0;

        //
        // Initialize flag used to map out the current channel to 0
        //
        mapOutCh    = 0;

        chdimmList  = &host->var.mem.socket[socket].channelList[ch].dimmList;

        //
        // Detect DIMMs in each slot
        //
        DimmTypeStatus = 0;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < host->var.mem.socket[socket].channelList[ch].numDimmSlots; dimm++) {
          dimmMtr.Data    = 0;
          GetSmbAddress(host, socket, ch, dimm, &spd);

          if ((*dimmNvList)[dimm].dimmPresent == 0) {
            emptySlot = 1;
            continue;
          }

          //
          // Log a warning and continue if a DIMM is populated out of order
          //
          if (emptySlot) {
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "DIMM out of order\nSMBUS Segment - %d, DIMM Address - 0x%x\n",
                           spd.address.busSegment, spd.address.strapAddress));
            OutputWarning (host, WARN_DIMM_POP_RUL, WARN_DIMM_POP_RUL_MINOR_OUT_OF_ORDER, socket, ch, dimm, 0xFF);

            mapOutCh = 1;
            continue;
          }

          if (dimm != 0) {
            if ((*dimmNvList)[dimm].aepDimmPresent && (*dimmNvList)[dimm-1].aepDimmPresent) {
              OutputWarning (host, WARN_DIMM_POP_RUL, WARN_DIMM_POP_RUL_2_AEP_FOUND_ON_SAME_CH, socket, ch, dimm, 0xFF);
            }

            if ((*dimmNvList)[0].aepDimmPresent == 1) {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "NON POR Config: NVMDIMM in slot 0\n"));
              OutputWarning (host, WARN_DIMM_POP_RUL, WARN_DIMM_POP_RUL_NVMDIMM_OUT_OF_ORDER, socket, ch, 0xFF, 0xFF);

              mapOutCh = 1;
              continue;
            }
          }
          // Change to addressing to 3DS support
          if (((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE) {
            (*channelNvList)[ch].encodedCSMode = 2;
          }


          // Determine DDR4 LRDIMM CS/ADDR mapping here
          if (IsLrdimmPresent(host, socket, ch, dimm) || ((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE) {

            // Save the number of physical ranks for later use
            numRanks = (((*dimmNvList)[dimm].actSPDModuleOrg >> 3) & 7);
            //
            // translate 0-based SPD number of ranks to 1-based
            numDramRanks = numRanks + 1;
            (*dimmNvList)[dimm].numDramRanks = numDramRanks;

            // Save die count per package

            //SPD
            //    000 = Single die
            //    001 = 2 die
            //    010 = 3 die
            //    011 = 4 die
            //    100 = 5 die
            //    101 = 6 die
            //    110 = 7 die
            //    111 = 8 die

            (*dimmNvList)[dimm].dieCount = (((*dimmNvList)[dimm].SPDDeviceType & SPD_DIE_COUNT) >> 4) + 1;

            // Reuse byte for Rank Multiplication factor as rank aliasing in DDR4
            (*dimmNvList)[dimm].lrRankMult = 1;
            //
            // if any dimm on this channel is in encoded mode, then all dimms must be in encoded mode
            if ((*channelNvList)[ch].encodedCSMode == 1) {
              (*dimmNvList)[dimm].lrRankMult = 2;
               numRanks = SPD_NUM_RANKS_2;
            }

            // Update logical ranks and DRAM row bits / density
            (*dimmNvList)[dimm].SPDModuleOrg = ((*dimmNvList)[dimm].actSPDModuleOrg & ~(BIT4 + BIT3)) | (numRanks << 3);
          }

          //
          // Comparison of DIMM's present on each slot to see if the DimmRank[slot] > DimmRank[slot-1]
          // Map out channel if Non POR Config detected (Ranks not placed in descending order in DIMM slots)
          //

          if (dimm != 0){
            if (DimmType != SPD_TYPE_AEP) {
              if (((*dimmNvList)[dimm].SPDModuleOrg >> 3) > ((*dimmNvList)[dimm - 1].SPDModuleOrg >> 3)) {
                MemDebugPrint((host, SDBG_MINMAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                               "DIMM config Not POR Config Slot%d SPD Num Rank= %d Slot%d SPD Num Rank= %d",
                               dimm - 1, ((*dimmNvList)[dimm - 1].SPDModuleOrg >> 3),
                               dimm, ((*dimmNvList)[dimm].SPDModuleOrg >> 3)));
                OutputWarning (host, WARN_DIMM_POP_RUL, WARN_DIMM_POP_RUL_MINOR_OUT_OF_ORDER, socket, ch, dimm, 0xFF);
                mapOutCh = 1;
                continue;
              }
            }
          }

          //
          // Lookup geometry in DIMM technology table
          //
          ti = GetTechIndex(host, socket, ch, dimm);

          //
          // Check if a valid tech index was found
          //
          if (ti == 0xFF) {
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                           "DIMM not supported!\n"));
            OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_COMPAT_MINOR_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
            mapOutCh = 1;
            continue;
          }

          //
          // 1, 2 or 4 Ranks
          //
          if ((*dimmNvList)[dimm].aepDimmPresent == 0) {
            if (!((((*dimmNvList)[dimm].SPDModuleOrg >> 3) == SPD_NUM_RANKS_1) ||
                  (((*dimmNvList)[dimm].SPDModuleOrg >> 3) == SPD_NUM_RANKS_2) ||
                  (((*dimmNvList)[dimm].SPDModuleOrg >> 3) == SPD_NUM_RANKS_4))) {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                             "Number of ranks on device not supported!\n"));
              OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_COMPAT_MINOR_MAX_RANKS, socket, ch, dimm, 0xFF);
              mapOutCh = 1;
              continue;
            }
          }
          //
          // Check if an RDIMM is plugged in to a UDIMM only board or connected to a cpu that does not support it
          //
          if (((*dimmNvList)[dimm].keyByte2 == SPD_RDIMM) &&
              ((host->setup.mem.dimmTypeSupport == UDIMM) )
             ) {
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                           "%s in UDIMM only board!\n",
                           GetDimmTypeStr((*dimmNvList)[dimm].actKeyByte2, (char *)&strBuf0)));
            OutputWarning (host, WARN_RDIMM_ON_UDIMM, WARN_MINOR_CONFIG_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
            mapOutCh = 1;
            continue;
          }

          //
          // Check if an UDIMM is plugged in to a RDIMM only board or connected to a cpu that does not support it
          //
          if (((*dimmNvList)[dimm].keyByte2 == SPD_UDIMM) &&
              ((host->setup.mem.dimmTypeSupport == RDIMM) || capId3.Bits.disable_udimm)
             ) {
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                           "UDIMM in RDIMM only board!\n"));
            OutputWarning (host, WARN_UDIMM_ON_RDIMM, WARN_MINOR_CONFIG_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
            mapOutCh = 1;
            continue;
          }

          //
          // Check if an SODIMM is plugged in to a RDIMM only board or connected to a cpu that does not support it
          //
          if (((*dimmNvList)[dimm].keyByte2 == SPD_SODIMM) &&
              ((host->setup.mem.dimmTypeSupport == RDIMM) || capId3.Bits.disable_udimm)
             ) {
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                           "SODIMM in RDIMM only board!\n"));
            OutputWarning (host, WARN_SODIMM_ON_RDIMM, WARN_MINOR_CONFIG_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
            mapOutCh = 1;
            continue;
          }

          //
          // Check if DIMM is supported for specific CPU SKU
          //
          fuseShadow.Data = MemReadPciCfgMC (host, socket, GetMCID(host, socket, ch), IMC_FUSE_DOWNLOAD_SHADOW_MC_MAIN_REG);

          // 3ds - SKU
          if (((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE) {
            if (fuseShadow.Bits.fuse_shadow_disable_3ds) {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "3DS not supported with this SKU!\n"));
              OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_3DS_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
              mapOutCh = 1;
              continue;
           }
          }

          // 16GB - SKU
          if ((*dimmNvList)[dimm].sdramCapacity == SPD_16Gb) {
            if (fuseShadow.Bits.fuse_shadow_disable_16gbit) {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "16Gb DRAMs not supported with this SKU!\n"));
              OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_16GB_SUPPORTED, socket, ch, dimm, 0xFF);
              mapOutCh = 1;
              continue;
            }
          }

          // Channel disable - SKU
          if(ch < MAX_MC_CH) {
            if (fuseShadow.Bits.fuse_shadow_chn_disable_mc0 & (1 << (ch % MAX_MC_CH))) {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Channel not supported with this SKU!\n"));
              OutputWarning (host, WARN_DIMM_COMPAT, WARN_CHANNEL_SKU_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
              mapOutCh = 1;
              continue;
            }
          } else {
              if (fuseShadow.Bits.fuse_shadow_chn_disable_mc1 & (1 << (ch % MAX_MC_CH))) {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Channel not supported with this SKU!\n"));
              OutputWarning (host, WARN_DIMM_COMPAT, WARN_CHANNEL_SKU_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
              mapOutCh = 1;
              continue;
            }
          }

          // Dimm disable - SKU
          if ((fuseShadow.Bits.fuse_shadow_disable_2_dpc) && ((*channelNvList)[ch].maxDimm == 2) && (dimm == 1)) {
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "2 Dimms per channel not supported with this SKU!\n"));
            OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_SKU_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
            mapOutCh = 1;
            continue;
          }

          // NVMDIMM - SKU
          if ((*dimmNvList)[dimm].aepDimmPresent) {
            if (capId1.Bits.disable_ddrt) {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "NVMDIMM not supported with this SKU!\n"));
              OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_NVMDIMM_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
              mapOutCh = 1;
              continue;
            }
          }


          //
          // Make sure RDIMMs and UDIMMs are not mixed. They are not electrically compatible
          // DimmTypeStatus is initialized to zero and will get set to the DIMM type of the DIMM
          // detected on the first pass. On subsequent passes keyByte2 must equal DimmTypeStatus or
          // else there is a mismatch and the system should halt to prevent electrical damage.
          // SODIMMs are not expected to be mixed with either RDIMM or UDIMM.
          //
          if (DimmTypeStatus != 0) {
            if (((*dimmNvList)[0].keyByte == SPD_TYPE_AEP) || ((*dimmNvList)[dimm].keyByte == SPD_TYPE_AEP)) {
              if (((*dimmNvList)[0].keyByte2 != SPD_RDIMM) || ((*dimmNvList)[dimm].keyByte2 != SPD_RDIMM)) {
                //
                // Rdimm and Lrdimm mixing allowed, checking dimm 1
                //
                MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                             "AEP is mixed with UDIMM!\n"));
#if SMCPKG_SUPPORT				//SMCPKG_SUPPORT++
                OutputError (host, ERR_DIMM_COMPAT, ERR_MIXED_MEM_AEP_AND_UDIMM, socket, ch, dimm, 0xFF);
#else            //SMCPKG_SUPPORT++   
                FatalError (host, ERR_DIMM_COMPAT, ERR_MIXED_MEM_AEP_AND_UDIMM);
#endif            //SMCPKG_SUPPORT++     
                break;
              }
            } else if (DimmTypeStatus != (*dimmNvList)[dimm].actKeyByte2) {

              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                             "%s is mixed with %s!\n",
                             GetDimmTypeStr((*dimmNvList)[dimm].actKeyByte2, (char *)&strBuf0),
                             GetDimmTypeStr(DimmTypeStatus, (char *)&strBuf1)));
#if SMCPKG_SUPPORT
              OutputError (host, ERR_DIMM_COMPAT, ERR_MIXED_MEM_TYPE, socket, ch, dimm, 0xFF);
#else
              FatalError (host, ERR_DIMM_COMPAT, ERR_MIXED_MEM_TYPE);
#endif
              break;
              //
              // Break incase the user does not want to halt
              //
            }
          }

          //
          // Disable ECC for the entire system if one DIMM is found that does not support it
          // TODO: Support multiple Bus Width Extension
          //

          if (dimm == 0) {
            ECCDimmSupport = ((*dimmNvList)[dimm].SPDMemBusWidth & BIT3);
            if (ECCDimmSupport == 0) {
              host->nvram.mem.eccEn = 0;
            }
          } else {
            ECCDimmSupport =((*dimmNvList)[dimm-1].SPDMemBusWidth & BIT3);
          }

          if (((*dimmNvList)[dimm].SPDMemBusWidth & BIT3) != ECCDimmSupport) {

            OutputWarning (host, WARN_DIMM_COMPAT,  WARN_CHANNEL_MIX_ECC_NONECC, socket, ch, dimm, 0xFF);

            if (!(host->setup.mem.options & ECC_MIX_EN)) {
              host->nvram.mem.eccEn = 0;
            } else {
              host->nvram.mem.eccEn = 1;
              DisableChannel(host, socket, ch);
            }
          }

          //
          // Set DIMM Type status to the first type of DIMM we find. If a different type is detect later
          // a fatal error will be logged.  Mixed DIMM types are not supported.
          // DimmTypeStatus = SPD_X definition = keyByte2 (1,2 & 3 = RDIMM, UDIMM & SODIMM)
          // dimmTypePresent is zero based so = keyByte2 -1
          // Set DRAM type to first type found
          //
          if (DimmTypeStatus == 0) {
            host->nvram.mem.dimmTypePresent = ((*dimmNvList)[dimm].keyByte2 - 1);
            DimmTypeStatus                  = (*dimmNvList)[dimm].actKeyByte2;
            DimmType                        = (*dimmNvList)[dimm].keyByte;
          }

          //
          // Get number of ranks
          //
          (*dimmNvList)[dimm].numRanks = ((*dimmNvList)[dimm].SPDModuleOrg >> 3) & 7;
          (*dimmNvList)[dimm].numRanks++;

          if (IsLrdimmPresent(host, socket, ch, dimm) == 0) {
            (*dimmNvList)[dimm].numDramRanks = (*dimmNvList)[dimm].numRanks;
          }
#ifdef QR_DIMM_SUPPORT
          //
          // Check for quad rank DIMM
          //
          if ((*dimmNvList)[dimm].numRanks == 4) {
            //
            // Update number of QR DIMMs
            //
            (*channelNvList)[ch].numQuadRanks++;

            host->nvram.mem.socket[socket].qrPresent = 1;
          }
#endif // QR_DIMM_SUPPORT

          // Check if number of ranks on this channel has been exceeded
          //
          if (((*channelNvList)[ch].numRanks + (*dimmNvList)[dimm].numRanks) > MAX_RANK_CH) {
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "Max ranks per ch exceeded\n"));
            OutputWarning (host, WARN_DIMM_COMPAT, WARN_RANK_NUM, socket, ch, 0xFF, 0xFF);

            //
            // Indicate this channel needs to be mapped out after detecting all populated DIMMs
            //
            mapOutCh = 1;
            continue;
          } else {
            //
            // Update ranks on this channel
            //
            (*channelNvList)[ch].numRanks = (*channelNvList)[ch].numRanks + (*dimmNvList)[dimm].numRanks;

            //
            // Check if this is a x4 DIMM
            //
            if (((*dimmNvList)[dimm].SPDModuleOrg & 7) == 0) {
              if (((*channelNvList)[ch].features & X8_PRESENT)) {
                //
                // Set flag to enable workaround
                //
                (*channelNvList)[ch].features |= X4_AND_X8_PRESENT;
              }
              (*dimmNvList)[dimm].x4Present = 1;
              (*channelNvList)[ch].features |= X4_PRESENT;
            }
            //
            // Check if this is a x8 DIMM
            //
            if (((*dimmNvList)[dimm].SPDModuleOrg & 7) == 1) {
              if (((*channelNvList)[ch].features & X4_PRESENT)) {
                //
                // Set flag to enable workaround
                //
                (*channelNvList)[ch].features |= X4_AND_X8_PRESENT;
              }
              (*channelNvList)[ch].features |= X8_PRESENT;
            }

            // Check if this is a x16 DIMM
            if (((*dimmNvList)[dimm].SPDModuleOrg & 7) == 2) {
              (*channelNvList)[ch].features |= X16_PRESENT;
            }

            //
            // Save DIMM technology
            //
            (*dimmNvList)[dimm].techIndex = ti;

            //
            // Update the fastest common tCK
            //
            if (minTCKcommon < (*chdimmList)[dimm].minTCK) {
              minTCKcommon = (*chdimmList)[dimm].minTCK;
            }

            //
            // Indicate there is memory present on this channel
            //
            chMemPresent = 1;

            //
            // Check that DDR4 DIMM supports 1.2V.  If 1.2v not supported
            // map out channel so voltage level does not get set.
            //
            if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4) {
              if (host->nvram.mem.socket[socket].channelList[ch].v120NotSupported) {
                MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                               "DDR4 DIMM does not support 1.2V!\n"));
                OutputWarning (host, WARN_DIMM_COMPAT, WARN_DIMM_VOLTAGE_NOT_SUPPORTED, socket, ch, dimm, 0xFF);
                mapOutCh = 1;
              }
            }

            if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4){
              //
              // Indicate this DIMM is populated
              //
              dimmMtr.Bits.dimm_pop = 1;
            }

            //
            // Set rank count
            //
            dimmMtr.Bits.rank_cnt = ((*dimmNvList)[dimm].SPDModuleOrg >> 3) & 7;

            //
            // Get Num of Ranks
            //
            if (dimmMtr.Bits.rank_cnt == 3) {
              dimmMtr.Bits.rank_cnt--;
            }

            //
            // 3DS and non-3DS lrdimms cannot be mixed.  If a one type was previously detected
            // on the current channel and the current dimm is not the same the system should halt.
            //
            if (dimm > 0) {
              if (((((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE ) && (((*dimmNvList)[dimm-1].SPDDeviceType & SPD_SIGNAL_LOADING) != SPD_3DS_TYPE)) ||
                        ((((*dimmNvList)[dimm-1].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE ) && (((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) != SPD_3DS_TYPE))) {
                if (((*dimmNvList)[dimm].keyByte != SPD_TYPE_AEP) && ((*dimmNvList)[dimm-1].keyByte != SPD_TYPE_AEP)) {
                  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                  "Invalid mix of DIMMs on channel!\n"));
#if SMCPKG_SUPPORT
                  OutputError (host, ERR_DIMM_COMPAT, ERR_MISMATCH_DIMM_TYPE, socket, ch, dimm, 0xFF);
#else
                  FatalError (host, ERR_DIMM_COMPAT, ERR_MISMATCH_DIMM_TYPE);
#endif
                  mapOutCh = 1;
                  break;
                  //
                  // Map out the channel and break in case the user does not want to halt
                  //
                }
              }
            }

            //
            // Check if we are in direct or encoded mode
            //
             if (((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE) {
              switch ((*dimmNvList)[dimm].dieCount) {
                case 2:
                  dimmMtr.Bits.ddr4_3dsnumranks_cs = 1; //2**1 = 2 ranks
                  (*channelNvList)[ch].cidBitMap |= BIT0;
                  break;

                case 3:
                case 4:
                  dimmMtr.Bits.ddr4_3dsnumranks_cs = 2; //2**2 = 4 ranks
                  (*channelNvList)[ch].cidBitMap |= (BIT0 + BIT1);
                  break;

                case 5:
                case 6:
                case 7:
                case 8:
                  dimmMtr.Bits.ddr4_3dsnumranks_cs = 3; //2**3 = 8 ranks
                  (*channelNvList)[ch].cidBitMap |= (BIT0 + BIT1 + BIT2);
                  break;

                default:
                  dimmMtr.Bits.ddr4_3dsnumranks_cs = 0; // no CIDs
                  break;
              }
            } else if ((*dimmNvList)[dimm].lrRankMult > 1) {
              dimmMtr.Bits.ddr4_3dsnumranks_cs = 1;
            } else {
              dimmMtr.Bits.ddr4_3dsnumranks_cs = 0;
            }


            // Is this necessary?
            // TODO: SKX removal

            //
            // Set DIMM width (not used by the hardware)
            //
            dimmMtr.Bits.ddr3_width = (*dimmNvList)[dimm].SPDModuleOrg & 3;

            //
            // Set DRAM density (not used by the hardware)
            //
            dimmMtr.Bits.ddr3_dnsty =  (*dimmNvList)[dimm].sdramCapacity - 2;

            //
            // Set Row Address width
            //
            dimmMtr.Bits.ra_width = (*dimmNvList)[dimm].numRowBits - 12;

            //
            // Set Column Address width
            //
            dimmMtr.Bits.ca_width = (*dimmNvList)[dimm].numColBits - 10;

            //
            // Save for later
            //
            (*dimmNvList)[dimm].dimmMemTech = dimmMtr.Data;

          }
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                "%s population\n", GetDimmTypeStr((*dimmNvList)[dimm].actKeyByte2, (char *)&strBuf0)));
        } // dimm loop

        //
        // Map out this channel if an invalid DIMM or configuration was detected
        //
        if (mapOutCh) {
          //
          // Disable this channel
          //
          (*channelNvList)[ch].enabled = 0;

          //
          // Indicate no memory is available on this channel
          //
          chMemPresent = 0;

          //
          // Mapping out DIMM 0 maps out all other DIMMs on this channel
          // MapOutDIMM(host, socket, ch, 0, ch);
          //
          maxDimm = 0;
        }

        //
        // Save the max number of DIMMs on this channel to enforce population rules
        //
        (*channelNvList)[ch].maxDimm = maxDimm;

        //
        // Disable this channel if there is no memory installed
        //
        if (maxDimm == 0) {
          (*channelNvList)[ch].enabled = 0;

          //TODO: This needs to be enabled for power reasons!!
          //mcMtr.Data = MemReadPciCfgMC (host, socket, GetMCID(host, socket, ch), MCMTR_MC_MAIN_REG);
          //mcMtr.Bits.chn_disable |= 1 << GetMCCh(GetMCID(host, socket, ch), ch);
          //MemWritePciCfgMC (host, socket, GetMCID(host, socket, ch), MCMTR_MC_MAIN_REG, mcMtr.Data);
          MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Channel disabled in MemSPD: ch = %d\n", ch));
        }
        //
        // Keep track of the largest number of DIMMs in a channel on this socket
        //
        if (host->nvram.mem.socket[socket].maxDimmPop < maxDimm) {
          host->nvram.mem.socket[socket].maxDimmPop = maxDimm;
        }
        //
        // Or into the global MemPresent status
        //
        MemPresent |= chMemPresent;

      } // ch loop

      if (host->nvram.mem.socket[socket].minTCK < minTCKcommon) {
        host->nvram.mem.socket[socket].minTCK = minTCKcommon;
      }

      if (!(host->nvram.mem.socket[socket].ddrVoltage & SPD_VDD_135) && host->nvram.mem.socket[socket].lvDimmPresent) {
        MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "DDR3L DIMMs operating at 1.50V!\n"));
      }
    } // socket loop

    CheckPORMixingWithinSocket(host);
    //
    // Check for no memory error
    //
    if (MemPresent == 0) {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "No memory found!\n"));
#if SMCPKG_SUPPORT		//SMCPKG_SUPPORT++
      OutputError (host, ERR_NO_MEMORY, ERR_NO_MEMORY_MINOR_NO_MEMORY, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK);
#else //SMCPKG_SUPPORT++
      OutputWarning (host, WARN_NO_DDR_MEMORY, WARN_NO_MEMORY_MINOR_NO_MEMORY, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK);
#endif //SMCPKG_SUPPORT++      
    }

  } // if (host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Loop for each CPU socket
    //
    if (host->nvram.mem.socket[socket].enabled == 0) continue;
    //SKX change
    channelNvList = &host->nvram.mem.socket[socket].channelList;
    for (mcId = 0; mcId < MAX_IMC; mcId++) {
      host->var.mem.socket[socket].imcEnabled[mcId] = 0;
      for (ch = 0; ch < MAX_MC_CH; ch++) {
        if ((*channelNvList)[ch + mcId*(MAX_MC_CH)].enabled) {
          host->var.mem.socket[socket].imcEnabled[mcId] = 1;
        }
      } //ch loop
    } //mcId loop

    //HSD 5371349 (IMC0 not populated)
    if ((host->var.mem.socket[socket].imcEnabled[0] == 0)){
      MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "POR non-validated config - NO memory on IMC0!\n"));
    }

  }  //socket loop
  GetCpuCsrAccessVar_RC (host, &host->var.CpuCsrAccessVarHost);

  CoreCheckPORCompat(host);

  return SUCCESS;
}

static UINT8
GetTechIndex (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     dimm
             )
/*++

  Returns the tech index for the current dimm

--*/
{
  UINT8               ti;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_AEP) {
    // SKX TODO: verify number of row, column, bank settings
    ti = 0;
  } else {
    //
    // DDR4
    //
    for (ti = 0; ti < MAX_TECH_DDR4; ti++) {

      if (((primaryWidthDDR4[ti] >> 3) == ((*dimmNvList)[dimm].SPDModuleOrg & 7)) &&
          (rowBitsDDR4[ti] == (*dimmNvList)[dimm].numRowBits) &&
          (columnBitsDDR4[ti] == (*dimmNvList)[dimm].numColBits) &&
          (bankGroupsDDR4[ti] == (*dimmNvList)[dimm].numBankGroups)
         ) {
        break;
      }
    } // ti loop

    if (ti >= MAX_TECH_DDR4) {
      ti = 0xFF;
    }
  }
  return ti;
} // GetTechIndex

/**

    GetDdr4SpdPageEn - Detect DDR4 vs. DDR3 SPD devices on SMBus

    @param host       - Pointer to sysHost
    @param socket     - Socket ID

    @retval ddr4SpdPageEn = 1 for DDR4 SPD, 0 for DDR3 SPD

**/
UINT8
GetDdr4SpdPageEn (
  PSYSHOST         host,
  UINT8            socket
  )
{
  return 1;
}

void
UpdateAccessMode(PSYSHOST host, UINT8 socket)
{
  UINT8               ch;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //
    // Update NVMCTLR access mode
    //
    (*channelNvList)[ch].fnvAccessMode = SMBUS;
  }
}

void
WaitForMailboxReady (PSYSHOST host, UINT8 socket)
{
  // Wait for NVMCTLR to indicate that Mailbox interface is ready
  // SKX TODO: handle errors
  FnvPollingBootStatusRegister (host, socket, FNV_MB_READY);
}

void
GetModuleSerialNumber(
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT8     ch,
                     UINT8     dimm,
                     UINT8     i,
                     UINT8     *SPDReg
                     )
{
  ReadSpd (host, socket, ch, dimm, SPD_MODULE_SN_DDR4 + i, SPDReg);
}

void
MemRankGeometry (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     cBits[MAX_RANK_CH][MAX_CH],
  UINT8     rBits[MAX_RANK_CH][MAX_CH],
  UINT8     bBits[MAX_RANK_CH][MAX_CH],
  UINT8     rankEnabled[MAX_RANK_CH][MAX_CH],
  UINT8     dimmRank[MAX_RANK_CH][MAX_CH],
  UINT8     rankChEnabled[MAX_RANK_CH],
  UINT8     *maxEnabledRank,
  UINT8     logical2Physical[MAX_RANK_CH][MAX_CH]
  )
/*++

  Lookup geometry information for all logical ranks

  @param host                - Pointer to sysHost
  @param socket              - Socket ID
  @param cBits               - number of column bits
  @param rBits               - number of row bits
  @param bBits               - number of bank bits
  @param rankEnabled         - logical rank enabled
  @param dimmRank            - dimm containing logicalRank
  @param rankChEnabled       - at least one logical rank on channels enabled
                               (OR of rankEnabled for all channels)
  @param maxEnabledRank      - max logical rank enabled on node

  @retval n/a

--*/
{
  UINT8               dimm;
  UINT8               rank;
  UINT8               logicalRank;
  UINT8               ch;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  *maxEnabledRank = 0;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    rankChEnabled[ch] = 0;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
#ifdef MEM_NVDIMM_EN
      if (host->var.mem.subBootMode == NvDimmResume && ((*dimmNvList)[dimm].nvDimmType) && ((*dimmNvList)[dimm].nvDimmStatus & STATUS_RESTORE_SUCCESSFUL)) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                       "Excluded from dimm map.\n"));
         continue;
      }
#endif  //MEM_NVDIMM_EN

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {

        //
        // Update logical rank geometry information
        //
        if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

        logicalRank = (*rankList)[rank].logicalRank;
        if (logicalRank > *maxEnabledRank) {
          *maxEnabledRank = logicalRank;
        }
        cBits[logicalRank][ch] = columnBitsDDR4[(*dimmNvList)[dimm].techIndex];
        rBits[logicalRank][ch] = rowBitsDDR4[(*dimmNvList)[dimm].techIndex];
        if (((*dimmNvList)[dimm].SPDSecondaryDeviceType & SPD_HALF_SIZE_SECOND_RANK) && ((rank) % 2)) {
          rBits[logicalRank][ch] = rBits[logicalRank][ch] - 1;
        }
        if (((*dimmNvList)[dimm].SPDSecondaryDeviceType & SPD_QUARTER_SIZE_SECOND_RANK) && ((rank) % 2)) {
          rBits[logicalRank][ch] = rBits[logicalRank][ch] - 2;
        }
        bBits[logicalRank][ch] = 4;
        dimmRank[logicalRank][ch] = dimm;
        logical2Physical[logicalRank][ch] = rank;
        if ((*dimmNvList)[dimm].aepDimmPresent) {
          rankEnabled[logicalRank][ch] = 0;
        } else {
          rankEnabled[logicalRank][ch] = 1;
        }
        rankChEnabled[ch] = 1;
      } // for rank
    } // for dimm
  } // for ch

  return;
} // MemRankGeometry

/**

  A hook into chip-level code that is called during DIMM detect
  phase in MRC before warm reset. This function is called once per
  socket, after the DIMM detect is complete.

  @param host  - Pointer to the system host (root) structure
  @param socket - socket ID

**/
VOID
PostDimmDetectChipHook (
  PSYSHOST host,
  UINT8    socket
  )
{
  UINT32              BiosScratchPad7;
  UINT8               sbspSktId;
  UINT8               ch;
  UINT8               dimm;
  UINT8               AepDimmPresentOnSocket;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  AepDimmPresentOnSocket = 0;

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < host->var.mem.socket[socket].channelList[ch].numDimmSlots; dimm++){
      if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
        AepDimmPresentOnSocket = 1;
      } // NVM DIMM present
    } // dimm loop
  } // ch loop

  if (AepDimmPresentOnSocket == 1) {
    sbspSktId = GetSbspSktId(host);
    BiosScratchPad7 = ReadCpuPciCfgEx (host, sbspSktId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
    if (!(BiosScratchPad7 & BIT31)) {
      BiosScratchPad7 |= BIT31;
      WriteCpuPciCfgEx (host, sbspSktId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, BiosScratchPad7);
    }
  } // NVMDIMM flag set
}

static void
CheckPORMixingWithinSocket(
                           PSYSHOST host
)
{
  UINT8 socket;
  UINT8 dimmTypeMask;
  UINT8 minorCode;
  dimmTypeMask = 0;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->nvram.mem.socket[socket].enabled == 0) continue;
    dimmTypeMask |= PopulateDimmTypeMask(host, socket);
  }

  //
  // Population Rules
  //
  minorCode = 0;

  //
  // 1. UDIMM cannot be mixed with any other dimm type
  //
  if (dimmTypeMask & UDIMM_SOCKET) {
    if (dimmTypeMask & (~UDIMM_SOCKET)) minorCode |= WARN_DIMM_POP_RUL_UDIMM_POPULATION;
  }

  //
  // 2. RDIMM can only be mixed with NVMDIMM
  //
  if (dimmTypeMask & RDIMM_SOCKET) {
    if (dimmTypeMask & (~(RDIMM_SOCKET | LRDIMM_AEP_SOCKET))) minorCode |= WARN_DIMM_POP_RUL_RDIMM_POPULATION;
  }

  //
  // 3. LRDIMM Dual Die Package can only be mixed with NVMDIMM
  //
  if (dimmTypeMask & LRDIMM_STANDARD_SOCKET) {
    if (dimmTypeMask & (~(LRDIMM_STANDARD_SOCKET | LRDIMM_AEP_SOCKET))) minorCode |= WARN_DIMM_POP_RUL_LRDIMM_DUAL_DIE_POPULATION;
  }

  //
  // 4. LRDIMM 3DS can only be mixed with NVMDIMM
  //
  if (dimmTypeMask & LRDIMM_3DS_SOCKET) {
    if (dimmTypeMask & (~(LRDIMM_3DS_SOCKET | LRDIMM_AEP_SOCKET))) minorCode |= WARN_DIMM_POP_RUL_LRDIMM_3DS_POPULATION;
  }

  //
  // 5. RDIMM 3DS can only be mixed with NVMDIMM
  //
  if (dimmTypeMask & RDIMM_3DS_SOCKET) {
    if (dimmTypeMask & (~(RDIMM_3DS_SOCKET | LRDIMM_AEP_SOCKET))) minorCode |= WARN_DIMM_POP_RUL_RDIMM_3DS_POPULATION;
  }

  if (minorCode != 0) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Mixing Population Rules across Sockets\n"));
    OutputWarning(host, WARN_DIMM_POP_RUL, minorCode, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK);
  }
}

static UINT8
PopulateDimmTypeMask(
                     PSYSHOST host,
                     UINT8    socket
)
{
  UINT8                ch;
  UINT8                dimm;
  UINT8                dimmTypeMask;
  struct dimmNvram     (*dimmNvList)[MAX_DIMM];
  struct channelNvram  (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  dimmTypeMask = 0;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if ((*dimmNvList)[dimm].actKeyByte2 == SPD_UDIMM) dimmTypeMask |= UDIMM_SOCKET;
      if ((*dimmNvList)[dimm].actKeyByte2 == SPD_RDIMM) {
        if (((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE) {
          dimmTypeMask |= RDIMM_3DS_SOCKET;
        } else {
          dimmTypeMask |= RDIMM_SOCKET;
        }
        // Check for 3DS
      }
      if ((*dimmNvList)[dimm].lrDimmPresent) {
        if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
          dimmTypeMask |= LRDIMM_AEP_SOCKET;
        } else if (((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE) {
          dimmTypeMask |= LRDIMM_3DS_SOCKET;
        } else {
          dimmTypeMask |= LRDIMM_STANDARD_SOCKET;
        }
      } //LRDIMM
    } // dimm
  } // ch
  return dimmTypeMask;
}
