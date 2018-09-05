/** @file
  This file contains the BIOS implementation of the BIOS-SSA Memory Configuration API.

@copyright
  Copyright (c) 2015 - 2016 Intel Corporation. All rights
  reserved. This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "SysFunc.h"
#include "BiosSsaChipFunc.h"


#ifdef SSA_FLAG

/**
  Calculate the actual channel number with respect to the Socket

  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number in Memory Controller.

  @retval ChannelInSocket     - Channel Value with respect to the Socket.
**/
UINT8
SSAGetChannelInSocket (
  UINT8                 Controller,
  UINT8                 Channel
  )
{
  UINT8                 ChannelInSocket = 0;

  ChannelInSocket = Channel;

  //If Controller = 0, Ch = 0,1 OR 0,1,2,3
  if (Controller == 1) {
    ChannelInSocket += MAX_CH_IMC;  //If Controller = 1, Channels = 2,3 for BDX and 3,4,5 for SKX
  }

  return ChannelInSocket;
}

/**
  Calculate the minimum and maximum offset to the specified data byte, given the minimum and maximum values.
  So for 9/18 strobes/nibbles (72bits) this function will calculate the Min(of all max's) and Max(of all min's) offset possible that can be used for all strobes
  @param[in]      MrcData   - Pointer to the MRC global data area.
  @param[in]      DataByte  - The current data byte.
  @param[in]      MinValue  - The smallest value that the data byte can be.
  @param[in]      MaxValue  - The largest value that the data byte can be.
  @param[out]     MinOffset - Minimum offset supported by the given margin group.
  @param[out]     MaxOffset - Maximum offset supported by the given margin group.

  @retval Nothing.
**/
static
VOID
SSACalcMinMax (
  PSYSHOST             host,
  INT16                DataByte,
  INT16                MinValue,
  INT16                MaxValue,
  INT16 *const         MinOffset,
  INT16 *const         MaxOffset
  )
{
  INT16                Min = 0;
  INT16                Max = 0;

  Min = MinValue - DataByte;
  *MinOffset = MAX(*MinOffset, Min);

  Max = MaxValue - DataByte;
  *MaxOffset = MIN(*MaxOffset, Max);
 }

/**
  Find if the bit is present in the bitMask for that nibble under consideration for x4 and x8 DIMMs for LowDQ, HighDQ and Ecc Masks

  @retval TRUE/FALSE     FALSE: Bit not present in bitMask for that nibble under consideration
**/

static
BOOLEAN
SSAPerBitMaskCheck (
  PSYSHOST              host,
  UINT8                 nibble,
  UINT8                 bit,
  UINT8                 maxStrobe,
  UINT32                DqMask1Low,
  UINT32                DqMask1High,
  UINT8                 EccMask
  )
{
  //DQ bits: For nibble=0, we check, DqMask1Low(Bits 0-3) and DqMask1High(Bits 0:3->32-35). Nibble=1, DqMask1Low(Bits 4-7), DqMask1High(Bits 4-7->35-38)... so on
  //ECC Bits: For nibble=8, we check, 64-67 and 68-71
  //If BitMask='0' that means no bits/lanes in that bitMask are participating in the margin parameter offset test

  if (!((((DqMask1Low >> (nibble * 4)) >> bit) & BIT0) && (DqMask1Low != 0))) { //0-32,bits 0 - 4
    return FALSE; //bit not set in mask
  }

  if (!((((DqMask1High >> (nibble * 4)) >> bit) & BIT0) && (DqMask1High != 0))) { //0-32,bits 0 - 4
    return FALSE;
  }

  //Reach upper bits of ECCMask
  if ((maxStrobe == MAX_STROBE) && (nibble == 17)) {
    bit = bit + 4;
  }

  //For ECCMask, bit and bit+4 will be the I/Ps for each nibble
  if (!(((EccMask >> bit) & BIT0) && (EccMask != 0))) { //bit 0 - 8
    return FALSE;
  }

  return TRUE;
}

/**
  Helper function used to calculate the MaxMinOffset/NewValue values for SSAGetSetMarginMaxMinOffset (Not used for the CMD/CTL margin groups)

  @param[in]       host                       - Pointer to sysHost
  @param[in]       Socket                     - Zero based socket number.
  @param[in]       channelInSocket            - Channel number w.r.t. the socket
  @param[in]       Dimm                       - Zero based Dimm number.
  @param[in]       Rank                       - Zero based Rank number.
  @param[in]       SetMarginFunctionFlag      - Set to have SSAGetSetMarginMaxMinOffset() behave as Get or SetMargin
  @param[in]       IoLevel                    - IoLevel of MarginGroup
  @param[in]       MarginGroup                - MarginGroup
  @param[in]       MinOffsetPntr              - Pointer to MinOffset value to be programmed
  @param[in]       MaxOffsetPntr              - Pointer to MaxOffset value to be programmed
  @param[in]       MinOffsetDefault           - MinOffsetDefault value
  @param[in]       MaxOffsetDefault           - MaxOffsetDefault value
  @param[in]       maxStrobe                  - maxStrobe value
  @param[in]       DataBytePntr               - Value modified by GetSetDataGroup() used to get Max/Min offset used for GetMarginOffset() - The difference which will be saved into GetSetDataGroup and used as the piDelay
  @param[in]       newValuePntr               - Value modified by GetSetDataGroup() used to get Max/Min offset used for SetMarginOffset()
  @param[in]       DqMask1Low                 - Lower 32 bits of DQMask. Mask of the lanes/strobes that participate to the margin parameter offset limit calculation.
  @param[in]       DqMask1High                - Upper 32 bits of DQMask. Mask of the lanes/strobes that participate to the margin parameter offset limit calculation.
  @param[in]       EccMask                    - EccMask

  @retval  -       None
**/
static
VOID
SSAGetSetMarginMaxMinOffset (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 channelInSocket,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 SetMarginFunctionFlag,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  INT16                 *MinOffsetPntr,
  INT16                 *MaxOffsetPntr,
  UINT16                MinOffsetDefault,
  UINT16                MaxOffsetDefault,
  UINT8                 maxStrobe,
  INT16                 *DataBytePntr,
  INT16                 *newValuePntr,
  UINT32                DqMask1Low,
  UINT32                DqMask1High,
  UINT8                 EccMask
  )
{
  UINT8                       nibble;
  UINT8                       bit;

  if (maxStrobe == (MAX_STROBE/2)) { // 9 strobes, 8th is the ecc strobe
    for (nibble = 0; nibble < maxStrobe; nibble ++) {
      if ((nibble < 4) && (((DqMask1Low >> (nibble * 8)) & 0xFF) == 0)) continue;  // first 32bits are for first 4 strobes.. 0 to 3, if NOT enabled in mask, skip that strobe
      if (((nibble >= 4) && (nibble < 8)) && (((DqMask1High >> ((nibble - 4) * 8)) & 0xFF) == 0)) continue;  // second 32bits are for the next 4 strobes.. 4 to 7, if NOT enabled in mask, skip that strobe
      if ((!host->nvram.mem.eccEn) && (nibble == 8)) continue;  // Skip if this is an ECC strobe when ECC is disabled
      if (((EccMask & 0xFF) == 0) && (nibble == 8)) continue;  //Continue if EccMask disabled

      if (SetMarginFunctionFlag != 1) { //ie GetMargin()
        if ((MarginGroup == RxDqsBitDelay) || (MarginGroup == TxDqBitDelay)) {
          for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {
            //Testing bits 0-3 on nibbles/strobes. IO/MC: strobes 0,9 , 1,10 2,11 3,12... regardless of x4 or x8 DIMM. If 0 for a Mask, all bit disabled from this test

            //DqMask1Low: Check bits 0:3(nibble=0) in DqMask1Low.. so on
            if ((nibble < 4) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, DqMask1Low, 0, 0) != FALSE)) {
              //
              // Nibble + 0 returns the IO swizzled data for the lower nibble of the byte
              //
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
              rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
              SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
            }

            // DqMask1High: Check bits 0:3(nibble=0+9) in DqMask1High.. so on
            if ((nibble < 4) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, DqMask1High, 0) != FALSE)) {
              //
              // Nibble + 9 returns the IO swizzled data for the upper nibble of the byte
              //
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
              rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, *DataBytePntr));
#endif
              SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
            }

            //DqMask1Low: Check bits 16:19(nibble=4) in DqMask1Low.. so on
            if (((nibble > 3) && (nibble < 8)) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, DqMask1Low, 0, 0) != FALSE)) {
              //
              // Nibble + 0 returns the IO swizzled data for the lower nibble of the byte
              //
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
              rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
              SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
            }

            // DqMask1High: Check bits 48:51(nibble=13) in DqMask1High.. so on
            if (((nibble > 3) && (nibble < 8)) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, DqMask1High, 0) != FALSE)) {
              //
              // Nibble + 9 returns the IO swizzled data for the upper nibble of the byte
              //
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
              rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, *DataBytePntr));
#endif
              SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
            }

            // EccMask. Nibble 8, ECC Lower Nibble Bits 0,1,2,3
            if ((nibble == 8) && (SSAPerBitMaskCheck (host, 0, bit, maxStrobe, 0, 0, EccMask) != FALSE)) {
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
              rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
              SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
            }

            if ((nibble == 8) && (SSAPerBitMaskCheck (host, 0 , bit + 4, maxStrobe, 0, 0, EccMask) != FALSE)) { //bit + 4 for decoding the Upper Nibble Bits of EccMask
              // Nibble 17, Upper Nibble Bits 4,5,6,7
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
              rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, *DataBytePntr));
#endif
              SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
            }

          } //for bit
        } else {
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, ALL_BITS, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
        rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, *DataBytePntr));
#endif
        SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
        }
      } else { //ie SetMargin
        // newValue = the difference which will be saved into GetSetDataGroup and used as the piDelay
        // GSM_UPDATE_CACHE caused the LRDIMM backside TxVref to malfunction because backside TxVref shares the same
        // training values for all byte groups and updating the cache for each group was erroneous.
       if ((MarginGroup == RxDqsBitDelay) || (MarginGroup == TxDqBitDelay)) {
         for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {

           //DqMask1Low
           if ((nibble < 4) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, DqMask1Low, 0, 0) != FALSE)) {
             //
             // Nibble + 0 returns the IO swizzled data for the lower nibble of the byte
             //
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u new_value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble,  bit, *newValuePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           //DqMask1High
           if ((nibble < 4) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, DqMask1High, 0) != FALSE)) {
             //
             // Nibble + 9 returns the IO swizzled data for the upper nibble of the byte
             //
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u new_value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, *newValuePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           //DqMask1Low
           if (((nibble > 3) && (nibble < 8)) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, DqMask1Low, 0, 0) != FALSE)) {
             //
             // Nibble + 0 returns the IO swizzled data for the lower nibble of the byte
             //
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u new_value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *newValuePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           // DqMask1High
           if (((nibble > 3) && (nibble < 8)) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, DqMask1High, 0) != FALSE)) {
             //
             // Nibble + 9 returns the IO swizzled data for the upper nibble of the byte
             //
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u new_value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, *newValuePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           //ECC. Nibble 8, ECC Lower Bits 0,1,2,3
           if ((nibble == 8) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, 0, EccMask) != FALSE)) {
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u new_value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *newValuePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           if ((nibble == 8) && (SSAPerBitMaskCheck (host, nibble, bit + 4, maxStrobe, 0, 0, EccMask) != FALSE)) {
             //ECC. Nibble 17, Upper Bits 4,5,6,7
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u new_value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble + 9, bit, *newValuePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

         } //for bit
       } else {
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, ALL_BITS, IoLevel, MarginGroup, GSM_FORCE_WRITE | GSM_WRITE_OFFSET, newValuePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
        rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u new_value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, *newValuePntr));
#endif
       }
     }
     }
  } else if (maxStrobe == MAX_STROBE) {  // 18 strobes, 8 and 17 are the ecc strobes -> x4 Dimm
    for (nibble = 0; nibble < maxStrobe; nibble ++) {
      // first 8 strobes (0-7) are for the lower nibble of a byte. Eg. lane 0-3, 8-11, 16-19, etc.
      // the strobes(8-17) are for the upper nibble of a byte. Eg. lane 4-7, 12-15, etc.
      if ((!host->nvram.mem.eccEn) && ((nibble == 8) || (nibble == 17))) continue;  // Skip if this is an ECC strobe when ECC is disabled.
      //ECC - Nibble8 is bits 64 - 67; Nibble 17 is bits 68-71
      if (((EccMask & 0xF) == 0) && (nibble == 8)) continue;  //Continue if EccMask disabled.
      if (((EccMask & 0xF0) == 0) && (nibble == 17)) continue;  //Continue if EccMask disabled.

      if ((nibble < 4) && (((DqMask1Low >> (nibble * 8)) & 0xF) == 0)) continue;
      if (((nibble >= 4) && (nibble < 8)) && (((DqMask1High >> ((nibble - 4) * 8)) & 0xF) == 0)) continue;
      if (((nibble >= 9 ) && (nibble < 13)) && (((DqMask1Low >> ((nibble - 9) * 8)) & 0xF0) == 0)) continue;
      if (((nibble >= 13) && (nibble < 17)) && (((DqMask1High >> ((nibble - 13) * 8)) & 0xF0) == 0)) continue;

      if (SetMarginFunctionFlag != 1) { //GetMargin()
        if ((MarginGroup == RxDqsBitDelay) || (MarginGroup == TxDqBitDelay)) {
          for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {

           //DqMask1Low
           if ((nibble < 8) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, DqMask1Low, 0, 0) != FALSE)) {
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           //DqMask1High
           if (((nibble > 8) && (nibble < 17)) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, DqMask1High, 0) != FALSE)) {
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           //EccMask
           if (((nibble == 8) || (nibble == 17)) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, 0, EccMask) != FALSE)) {
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

          } //for bit
        } else {
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, ALL_BITS, IoLevel, MarginGroup, GSM_READ_ONLY, DataBytePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
        rcPrintf ((host,"GetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, *DataBytePntr));
#endif
        SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
        }
      } else { //SetMargin
        // GSM_UPDATE_CACHE caused the LRDIMM backside TxVref to malfunction because backside TxVref shares the same
        // training values for all byte groups and updating the cache for each group was erroneous.
        if ((MarginGroup == RxDqsBitDelay) || (MarginGroup == TxDqBitDelay)) {
          for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {
           //DqMask1Low
           if ((nibble < 8) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, DqMask1Low, 0, 0) != FALSE)) {
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_FORCE_WRITE | GSM_WRITE_OFFSET, newValuePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           //DqMask1High
           if (((nibble > 8) && (nibble < 17)) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, DqMask1High, 0) != FALSE)) {
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_FORCE_WRITE | GSM_WRITE_OFFSET, newValuePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }

           //EccMask
           if (((nibble == 8) || (nibble == 17)) && (SSAPerBitMaskCheck (host, nibble, bit, maxStrobe, 0, 0, EccMask) != FALSE)) {
             CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, bit, IoLevel, MarginGroup, GSM_FORCE_WRITE | GSM_WRITE_OFFSET, newValuePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
             rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u bit:%u value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, bit, *DataBytePntr));
#endif
             SSACalcMinMax (host, *DataBytePntr, MinOffsetDefault, MaxOffsetDefault, MinOffsetPntr, MaxOffsetPntr);
           }
         } //for bit
       } else {
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, Socket, channelInSocket, Dimm, Rank, nibble, ALL_BITS, IoLevel, MarginGroup, GSM_FORCE_WRITE | GSM_WRITE_OFFSET, newValuePntr));
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
        rcPrintf ((host,"SetMarginParamOffset s:%u c:%u d:%u r:%u nibble:%u new_value:%d\n", Socket, channelInSocket, Dimm, Rank, nibble, *newValuePntr));
#endif
      }
    }
    } //for-nibble
  } // end else if
} //SSAGetSetMarginMaxMinOffset

/**
  Calculate and return the step size for the MarginGroup

  @param[in]       MarginGroup     - MarginGroup entered from GSM_GT_SSA.

  @retval MarginGroupStepSize                    - Step size for the MarginGroup
**/
//Create a helper function - SWITCH... I/P IoLevel/margingroup to return *StepSize for every margingroup --> TxVref/RxVref .. for other group values --> piStep  = (UINT8)16;
static
UINT16
SSAGetMarginGroupStepSize (
  GSM_GT            MarginGroup
  )
{
  UINT16            MarginGroupStepSize = 0;

  switch (MarginGroup) {
    case RxVref:
      MarginGroupStepSize = (UINT16) 520;
      break;
    case TxVref:
      MarginGroupStepSize = (UINT16) 533;
      break;
    default:  //All other groups default step size = piStep
      MarginGroupStepSize = (UINT16) 16; //piStep  = (UINT8)16;
      break;
  }

  return MarginGroupStepSize;
}


/**
  Function used to get information about the system.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     SystemInfo - Pointer to buffer to be filled with system information.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetSystemInfo) (
  SSA_BIOS_SERVICES_PPI *This,
  MRC_SYSTEM_INFO       *SystemInfo
  )
{
  UINT8                              socket = 0;
  struct sysHost                     *host;

  const MRC_SYSTEM_INFO SysInfoConst = {
    MAX_SOCKET,                    // MaxNumberSockets
    MAX_IMC,                       // MaxNumberControllers
    MAX_CH / MAX_IMC,              // MaxNumberChannels Per Controller
    MAX_DIMM,                      // MaxNumberDimms
    MAX_RANK_DIMM,                 // MaxNumberRanks
    CPU_SOCKET_BIT_MASK,           // SocketBitMask
    BUS_WIDTH,                     // BusWidth 72bits with Ecc else 64bits
    BUS_FREQ,                      // DDR Bus Frequency in MHz
    FALSE,                         // IsEccEnabled -> default is FALSE, sysHost is checked and the value updated in struct
  };

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetSystemInfo ()\n"));
#endif

  socket = host->var.mem.currentSocket;

  MemCopy((UINT8 *) SystemInfo, (UINT8 *) &SysInfoConst, sizeof (MRC_SYSTEM_INFO));

  SystemInfo->SocketBitMask = host->var.common.socketPresentBitMap;

  if (host->nvram.mem.eccEn) {
    SystemInfo->BusWidth = 72;        //Default = 64bits
    SystemInfo->IsEccEnabled = TRUE;  //Default = FALSE
  }

  SystemInfo->BusFreq = (UINT32)host->nvram.mem.socket[socket].ddrFreqMHz;      //Default = 1333

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosGetSystemInfo\n"));
#endif
}

/**
  Function used to get the platform memory voltage (VDD).

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Voltage - Pointer to were the platform’s memory voltage (in mV) will be written.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetMemVoltage) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                *Voltage
  )
{
  UINT8                      socket;
  struct sysHost             *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetMemVoltage ()\n"));
#endif

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    if (CHIP_FUNC_CALL(host, GetLvmode(host, socket)) == 0) {
      *Voltage = 1500;
    } else if (CHIP_FUNC_CALL(host, GetLvmode(host, socket)) == 1) {
      *Voltage = 1350;
    } else if (CHIP_FUNC_CALL(host, GetLvmode(host, socket)) == 3) {
      *Voltage = 1200;
    }

    break; //Get system volt. from any one socket
  }//for-loop

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosGetMemVoltage (Platform Voltage: 0x%x)\n", *Voltage));
#endif
}

/**
  Function used to set the platform memory voltage.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Voltage - The memory voltage (in mV) to be set on the platform.

  @retval Nothing.
**/
VOID
(EFIAPI BiosSetMemVoltage) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                Voltage
  )
{
  UINT8                      socket;
  struct sysHost             *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosSetMemVoltage (Voltage:%d)\n", Voltage));
#endif

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->nvram.mem.socket[socket].enabled) {
      if (Voltage == 1200) {
        host->nvram.mem.socket[socket].ddrVoltage = SPD_VDD_120;
      } else if (Voltage == 1350) {
        host->nvram.mem.socket[socket].ddrVoltage = SPD_VDD_135;
      } else if (Voltage == 1500) {
        host->nvram.mem.socket[socket].ddrVoltage = SPD_VDD_150;
      }

      CheckVdd(host); // To reset platform voltage
    }
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosSetMemVoltage\n"));
#endif
}

/**
  Function used to get the DIMM temperature.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[out]     Temperature - Pointer to where the DIMM's temperature will be written.  Units: Celsius with 1 degree precision.

  @retval NotAvailable if the DIMM does not support a temperature sensor.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetMemTemp) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  INT32                 *Temperature
  )
{
  UINT8                                channelInSocket;
  struct sysHost                       *host;
  struct dimmNvram                     (*dimmNvList)[MAX_DIMM];

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetMemTemp (Socket:%d, Controller:%d, Channel:%d, Dimm:%d)\n", Socket, Controller, Channel, Dimm));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  dimmNvList = GetDimmNvList(host, Socket, channelInSocket);

  if (((*dimmNvList)[Dimm].dimmTs & BIT7) != 0) { // if Bit7 (of byte 14) = 1, tsod present
    CHIP_FUNC_CALL(host, BiosGetMemTempChip (host, Socket, channelInSocket, Dimm, Temperature));
  } else {
    return NotAvailable; //TSOD absent; DIMM does not support a temperature sensor
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosGetMemTemp (Temperature: 0x%x)\n", *Temperature));
#endif
  return Success;
}

/**
  Function used to get the bitmask of populated memory controllers on a given CPU socket.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket  - Zero based CPU socket number.
  @param[out]     BitMask     - Pointer to where the memory controller bitmask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = memory controller 0; bit position 1 = memory controller 1, etc.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetControllerBitMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 *BitMask
  )
{
  UINT8                      mcId;
  UINT8                      controllerBitmask = 0;
  struct sysHost             *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetControllerBitMask (Socket:%d)\n", Socket));
#endif

  if ((host->nvram.mem.socket[Socket].enabled) && (host->nvram.mem.socket[Socket].maxDimmPop)) { //IsSocketPresent
    for (mcId = 0; mcId < MAX_IMC; mcId++) {
      if (host->var.mem.socket[Socket].imcEnabled[mcId] == 0) continue;
        controllerBitmask |= (1 << mcId);  //0001 OR 0011
    }
      *BitMask = controllerBitmask;
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosGetControllerBitMask (BitMask: 0x%x)\n", *BitMask));
#endif
}

/**
  Function used to get the bitmask of populated memory channels on a given memory controller.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[out]     BitMask     - Pointer to where the memory channel bit mask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = memory channel 0; bit position 1 = memory channel 1, etc.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetChannelBitMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 *BitMask
  )
{
  UINT8                       ch;
  UINT8                       chBitmask = 0;
  struct sysHost              *host;
  struct channelNvram         (*channelNvList)[MAX_CH];

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetChannelBitMask (Socket:%d, Controller:%d)\n", Socket, Controller));
#endif
  channelNvList = GetChannelNvList(host, Socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if (host->var.mem.socket[Socket].channelList[ch].mcId != Controller) continue;  //Skip if not the 'Controller' entered to be checked

    //Bit Mask of Channels on a given Controller
    if (Controller == 0) {
      chBitmask |= (1 << ch); //For BDX: chBitmask = 0011 or 1111 when all Channels/MC enabled; SKX: chBitmask = 0111
    } else {
      chBitmask |= (1 << (ch - MAX_CH_IMC)); //For BDX: chBitmask = 0011 when all Channels/MC enabled; SKX: chBitmask = 0111
    }
  }

  *BitMask = chBitmask;

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosGetChannelBitMask (BitMask: 0x%x)\n", *BitMask));
#endif
}

/**
  Function used to get the bitmask of populated DIMMs on a given memory channel.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     BitMask    - Pointer to where the DIMM bit mask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = DIMM 0; bit position 1 = DIMM 1, etc.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetDimmBitMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 *BitMask
  )
{
  UINT8                      dimm;
  UINT8                      channelInSocket;
  UINT8                      dimmBitmask = 0;
  struct sysHost             *host;
  struct socketNvram         *socketNvram;
  struct dimmNvram           (*dimmNvList)[MAX_DIMM];

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetDimmBitMask (Socket:%d, Controller:%d, Channel:%d)\n", Socket, Controller, Channel));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);
  socketNvram = &host->nvram.mem.socket[Socket];
  dimmNvList = GetDimmNvList(host, Socket, channelInSocket);

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    dimmBitmask |= (1 << dimm);
  }

  *BitMask = dimmBitmask;

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosGetDimmBitMask (BitMask: 0x%x)\n", *BitMask));
#endif
}

/**
  Function used to get the number of ranks in a given DIMM.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[out]     *RankCount  - Pointer to where the rank count will be stored.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetRankInDimm) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 *RankCount
  )
{
  UINT8                       channelInSocket;
  struct sysHost              *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetRankInDimm (Socket:%d, Controller:%d, Channel:%d, Dimm:%d)\n", Socket, Controller, Channel, Dimm));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  *RankCount = host->nvram.mem.socket[Socket].channelList[channelInSocket].dimmList[Dimm].numDramRanks;

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosGetRankInDimm (RankCount: 0x%x)\n", *RankCount));
#endif
}

/**
  Function used to get the MC logical rank associated with a given DIMM and rank.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[in]      Rank        - Zero based rank number.
  @param[out]     LogicalRank - Pointer to where the logical rank will be stored.

  @retval None.
**/
VOID
(EFIAPI BiosGetLogicalRank) (
  struct _SSA_BIOS_SERVICES_PPI *This,
  UINT8                         Socket,
  UINT8                         Controller,
  UINT8                         Channel,
  UINT8                         Dimm,
  UINT8                         Rank,
  UINT8                         *LogicalRank
  )
{
  UINT8                         channelInSocket;
  struct sysHost                *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetLogicalRank (Socket:%d, Controller:%d, Channel:%d, Dimm:%d, Rank:%d)\n", Socket, Controller, Channel, Dimm, Rank));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  *LogicalRank = GetLogicalRank(host, Socket, channelInSocket, Dimm , Rank);

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosGetLogicalRank (LogicalRank: 0x%x)\n", *LogicalRank));
#endif
}

/**
  Function used to get DIMM information.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Dimm           - Zero based DIMM number.
  @param[out]     DimmInfoBuffer - Pointer to buffer to be filled with DIMM information.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetDimmInfo) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  MRC_DIMM_INFO         *DimmInfoBuffer
  )
{
  UINT8                            channelInSocket;
  struct sysHost                   *host;
  struct dimmNvram                 (*dimmNvList)[MAX_DIMM];
  struct channelNvram              (*channelNvList)[MAX_CH];

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetDimmInfo (Socket:%d, Controller:%d, Channel:%d, Dimm:%d)\n", Socket, Controller, Channel, Dimm));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  dimmNvList  = GetDimmNvList(host, Socket, channelInSocket);
  channelNvList = GetChannelNvList(host, Socket);

  if ((*dimmNvList)[Dimm].SPDMemBusWidth & BIT3) { //Checks EccEn on that Dimm
    DimmInfoBuffer->EccSupport = TRUE;
  } else {
    DimmInfoBuffer->EccSupport = FALSE;
  }

  DimmInfoBuffer->DimmCapacity = (host->nvram.mem.socket[Socket].channelList[channelInSocket].dimmList[Dimm].sdramCapacity << 6); //memSize of dimm in MB, 1unit =64mb
  DimmInfoBuffer->BankCount    = host->nvram.mem.socket[Socket].channelList[channelInSocket].dimmList[Dimm].numBanks;
  DimmInfoBuffer->RowSize      = 0x1 << (host->nvram.mem.socket[Socket].channelList[channelInSocket].dimmList[Dimm].numRowBits);
  DimmInfoBuffer->ColumnSize   = 0x1 << (host->nvram.mem.socket[Socket].channelList[channelInSocket].dimmList[Dimm].numColBits);
  DimmInfoBuffer->DeviceWidth  = (UINT8) (4 << ((*dimmNvList)[Dimm].SPDModuleOrg & 0x7));

  switch ((*dimmNvList)[Dimm].keyByte) { //BYTE2
    case SPD_TYPE_DDR3:
      DimmInfoBuffer->MemoryTech = SsaMemoryDdr3;
      break;
    case SPD_TYPE_DDR4:
      DimmInfoBuffer->MemoryTech = SsaMemoryDdr4;
      break;
    case SPD_TYPE_AEP:
      DimmInfoBuffer->MemoryTech = SsaMemoryDdrT;
      break;
    default:
      break;
  }

  //Fill in the MEMORY_PACKAGE Values - First 3 values are the same for DDR3 and DDR4
  switch ((*dimmNvList)[Dimm].actKeyByte2) { //To get the Actual Module Type - BYTE3
    case SPD_RDIMM:
      DimmInfoBuffer->MemoryPackage = RDimmMemoryPackage;
      break;
    case SPD_UDIMM:
      DimmInfoBuffer->MemoryPackage = UDimmMemoryPackage;
      break;
    case SPD_SODIMM:
      DimmInfoBuffer->MemoryPackage = SoDimmMemoryPackage;
      break;
#ifdef  LRDIMM_SUPPORT
    case SPD_LRDIMM:
      DimmInfoBuffer->MemoryPackage = LrDimmMemoryPackage;
      break;
#endif
    case SPD_MICRO_DIMM:
      DimmInfoBuffer->MemoryPackage = MicroDimmMemoryPackage;
      break;
    case SPD_MINI_RDIMM:
      DimmInfoBuffer->MemoryPackage = MiniRDimmMemoryPackage;
      break;
    case SPD_MINI_UDIMM:
      DimmInfoBuffer->MemoryPackage = MiniUDimmMemoryPackage;
      break;
    case SPD_MINI_CDIMM:
      DimmInfoBuffer->MemoryPackage = MiniCDimmMemoryPackage;
      break;
    case SPD_ECC_SO_UDIMM:
      DimmInfoBuffer->MemoryPackage = SoUDimmEccMemoryPackage;
      break;
    case SPD_ECC_SO_RDIMM:
      DimmInfoBuffer->MemoryPackage = SoRDimmEccMemoryPackage;
      break;
    case SPD_16b_SO_DIMM:
      DimmInfoBuffer->MemoryPackage = SoDimm16bMemoryPackage;
      break;
    case SPD_32b_SO_DIMM:
      DimmInfoBuffer->MemoryPackage = SoDimm32bMemoryPackage;
      break;
    default:
      break;
  }

  //Overwrite for DDR4 the SPD_MICRO_DIMM value which is also equal 4
  if ((((host->nvram.mem.dramType) == SPD_TYPE_DDR4) && (*dimmNvList)[Dimm].actKeyByte2 == SPD_LRDIMM_DDR4)) {
#ifdef  LRDIMM_SUPPORT
    DimmInfoBuffer->MemoryPackage = LrDimmMemoryPackage;
#endif
  }
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosGetDimmInfo\n"));
#endif
}

/**
  Function used to get DIMM unique module ID.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Dimm           - Zero based DIMM number.
  @param[out]     UniqueModuleId - Pointer to buffer to be filled with DIMM unique module ID.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetDimmUniqueModuleId) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  SPD_UNIQUE_MODULE_ID  *UniqueModuleId
  )
{
  UINT8                            i;
  UINT8                            channelInSocket;
  struct sysHost                   *host;
  struct dimmNvram                 (*dimmNvList)[MAX_DIMM];

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetDimmUniqueModuleId (Socket:%d, Controller:%d, Channel:%d, Dimm:%d)\n", Socket, Controller, Channel, Dimm));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  dimmNvList  = GetDimmNvList(host, Socket, channelInSocket);

  UniqueModuleId->Location  = (*dimmNvList)[Dimm].SPDMMfLoc;                         //1 byte location
  UniqueModuleId->IdCode    = (*dimmNvList)[Dimm].SPDMMfgId;
  UniqueModuleId->Date.Year = (UINT8)((*dimmNvList)[Dimm].SPDModDate & 0x00FF);      //SPDModDate- 0:7 = Year, 8-15 = Week
  UniqueModuleId->Date.Week = (UINT8)((*dimmNvList)[Dimm].SPDModDate >> 8);          //Masking bits 8-15 for year, and right shifting bits 8:15 for week

  for (i = 0; i < 4; i++) {
    UniqueModuleId->SerialNumber.SerialNumber8[i] = (*dimmNvList)[Dimm].SPDModSN[i];  // Module Serial Number: 4 bytes
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosGetDimmUniqueModuleId\n"));
#endif
}

/**
  Function used to get DIMM SPD data.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      Dimm         - Zero based DIMM number.
  @param[in]      ByteOffset   - The byte offset in the SPD.
  @param[in]      ByteCount    - The number of bytes to read starting from the offset location specified by the "ByteOffset".
  @param[in]      ReadFromBus  - Flag to determine where to retrieve the SPD value.  TRUE = read from bus.  FALSE = return from MRC cache value.
  @param[out]     Data         - Pointer to buffer to be filled with  DIMM SPD data.

  @retval UnsupportedValue if (ByteOffset + ByteCnt) is larger than the SPD size.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetSpdData) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT16                ByteOffset,
  UINT16                ByteCount,
  BOOLEAN               ReadFromBus,
  UINT8                 *Data
  )
{
  UINT8                                             arrayByteCount = 0;
  UINT8                                             channelInSocket;
  UINT16                                            currentByte = 0;
  struct sysHost                                    *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetSpdData (Socket:%d, Controller:%d, Channel:%d, Dimm:%d, ByteOffset:%d, ByteCount:%d, ReadFromBus:%d)\n", Socket, Controller, Channel, Dimm, ByteOffset, ByteCount, ReadFromBus));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  //if ((((ByteOffset + ByteCount) > MAX_SPD_BYTE_DDR4) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) || (((ByteOffset + ByteCount) > MAX_SPD_BYTE)  && (host->nvram.mem.dramType == SPD_TYPE_DDR3))) {
  if (((ByteOffset + ByteCount) > MAX_SPD_BYTE_DDR4) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
    return UnsupportedValue;
  }

  //For "ReadFromBus" = True we will always read from the SPD
  for (currentByte = ByteOffset; currentByte < (ByteOffset + ByteCount); currentByte++) {
    ReadSpd (host, Socket, channelInSocket, Dimm, currentByte, (Data + arrayByteCount));
    arrayByteCount++;
  }
  //if ReadFromBus = FALSE read the Cached Value for SpdData.. is there a host struct in NVRAM (MRC)?

  #ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosGetSpdData (Data: 0x%x)\n", *Data));
#endif
  return Success;
}

/**
  Function used to perform a JEDEC reset for all the DIMMs on all channels of a given memory controller.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.

  @retval Nothing.
**/
VOID
(EFIAPI BiosJedecReset) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller
  )
{
  UINT8                   ch;
  UINT8                   chBitMask = 0;
  struct sysHost          *host;
  struct channelNvram     (*channelNvList)[MAX_CH];

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosJedecReset (Socket:%d, Controller:%d)\n", Socket, Controller));
#endif
  channelNvList = GetChannelNvList(host, Socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    if (Controller == CHIP_FUNC_CALL(host, GetMCID(host, Socket, ch))) {
      chBitMask |= (1 << ch);
    }
  } // ch loop

  disableAllMsg(host); // Turn off message to avoid printing the "N0.C2.D0.R0: Write RC00 = 0x00" messages during JedecReset
  JedecInitSequence (host, Socket, chBitMask);
  restoreMsg(host);

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosJedecReset\n"));
#endif
}

/**
  Function used to reset the I/O for a given memory controller.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.

  @retval Nothing.
**/
VOID
(EFIAPI BiosIoReset) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller
  )
{
  //Similar to IO_Reset, but this is written to reset a single Controller
  UINT8                      resetIo = 0;
  struct sysHost             *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosIoReset (Socket:%d, Controller:%d)\n", Socket, Controller));
#endif

  //
  // Reset DDR IO
  //
  resetIo = 1;
  CHIP_FUNC_CALL(host, BiosIoResetChip (host, Socket, Controller, resetIo));


  //
  // Wait 20 QCLK with reset high.
  //
  FixedQClkDelay (host, 20);

  //
  // Clear DDR IO Reset
  //
  resetIo = 0;
  CHIP_FUNC_CALL(host, BiosIoResetChip (host, Socket, Controller, resetIo));

  //
  // Wait again 20 QCLK for stable IO.
  //
  FixedQClkDelay (host, 20);

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosIoReset\n"));
#endif
}

/**
  Function used to get the specificity of a given margin parameter.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      IoLevel     - I/O level.
  @param[in]      MarginGroup - Margin group.
  @param[out]     Specificity - Pointer to where the margin parameter specificity mask will be stored.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetMarginParamSpecificity) (
  SSA_BIOS_SERVICES_PPI    *This,
  GSM_LT                   IoLevel,
  GSM_GT                   MarginGroup,
  MARGIN_PARAM_SPECIFICITY *Specificity
  )
{
  struct sysHost                 *host;
  SSA_STATUS                     Status = Success;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "\nStart: BiosGetMarginParamSpecificity (IoLevel: %d, MarginGroup: %d) \n", IoLevel, MarginGroup));
#endif

  Status = CHIP_FUNC_CALL(host, BiosGetMarginParamSpecificityChip (host, IoLevel, MarginGroup, Specificity));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosGetMarginParamSpecificity (Specificity: 0x%x)\n", *Specificity));
#endif

  return Status;
}

/**
  Function used to initialize the system before/after a margin parameter's use.
  Prior to calling the GetMarginParamLimits or SetMarginParamOffset functions
  for a margin parameter, this function should be called with the SetupCleanup
  input parameter set to Setup.  When finished with the margin parameter, this
  function should be called with the SetupCleanup input parameter set to
  Cleanup.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      IoLevel      - I/O level.
  @param[in]      MarginGroup  - Margin group.
  @param[in]      SetupCleanup - Specifies setup or cleanup action.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosInitMarginParam) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  SETUP_CLEANUP         SetupCleanup
  )
{
  struct sysHost                         *host;
  UINT8                                  channelInSocket;
  struct channelNvram                    (*channelNvList)[MAX_CH];
  MARGIN_PARAM_SPECIFICITY               Specificity;
  UINT8                                  CurrentSocketSaved;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;

  // Save the current socket.
  CurrentSocketSaved = host->var.mem.currentSocket;
  host->var.mem.currentSocket = Socket;

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosInitMarginParam (Socket:%d, IoLevel:%d, MarginGroup:%d, SetupCleanup:%d)\n", Socket, IoLevel, MarginGroup, SetupCleanup));
#endif

  //Check if the IoLevel and margin group is supported or not
  if (BiosGetMarginParamSpecificity (This, IoLevel, MarginGroup, &Specificity) == UnsupportedValue) {
    return UnsupportedValue;
  }

  switch (MarginGroup) {
    case CmdGrp0:
    case CmdGrp1:
    case CmdGrp2:
    case CmdAll:
    case CtlGrp0:
    case CtlGrp1:
    case CtlGrp2:
    case CtlGrp3:
    case CtlGrp4:
    case CtlGrp5:
    case CtlAll:
    case CmdVref:
    channelNvList = GetChannelNvList(host, Socket);
      // code to support additional shmoo direction. The bit0 defines - "setup"=1;  "clean"=0
      // The bits[2:1] define the margin direction: "low": 0, "high": 1, "high_and_low": 2,  "low_and_high": 3
      if (!(SetupCleanup & 0x1)) {
        // During the setup(BIT0=0x1), the order is to DENORMALIZE first, apply the cmd_oe_val, then backside
        if (IoLevel == DdrLevel) {  // only applicable to frontside
          disableAllMsg(host); // Turn off message to avoid printing the kind of "N0.C2.D0.R0: Write RC00 = 0x00" message
          DeNormalizeCCC (host, Socket, DENORMALIZE);
          restoreMsg(host);
        }

        for (channelInSocket = 0; channelInSocket < MAX_CH; channelInSocket++) {
          if ((*channelNvList)[channelInSocket].enabled == 0) continue;
          CHIP_FUNC_CALL(host, GetMarginsHook (host, Socket, channelInSocket, 1)); //sets 'cmd_oe_on' value
          //BiosInitMarginParamChip (host, Socket, channelInSocket, cmd_oe_val);        // Use the GetMarginHook()
        } // channelInSocket loop

        if ((IoLevel == LrbufLevel) && (((SetupCleanup >> 1) & 0x3) == 0)) { //reading BITS[2:1] for margin direction
          disableAllMsg(host);
          BacksideShift(host, Socket, RENORMALIZE);
          restoreMsg(host);
        }
      } else { //During the clean(BIT0=0x0)
        // during the cleanup, the order is backside first, then to apply the cmd_oe_val, finally RENORMALIZE.
        if ((IoLevel == LrbufLevel) && (((SetupCleanup >> 1) & 0x3) == 0)) {
          disableAllMsg(host);
          BacksideShift(host, Socket, DENORMALIZE);
          restoreMsg(host);
        }

        for (channelInSocket = 0; channelInSocket < MAX_CH; channelInSocket++) {
          if ((*channelNvList)[channelInSocket].enabled == 0) continue;
          CHIP_FUNC_CALL(host, GetMarginsHook (host, Socket, channelInSocket, 0)); //sets cmd_oe_on
        } // channelInSocket loop

        if (IoLevel == DdrLevel) {
          disableAllMsg(host);
          DeNormalizeCCC (host, Socket, RENORMALIZE);
          restoreMsg(host);
        }
      }

      break;
    default:
      break;
  }//MarginGroup

  // Restore the current socket
  host->var.mem.currentSocket = CurrentSocketSaved;

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosInitMarginParam actual margin group:%d \n", MarginGroup));
#endif

  return Success;
}

/**
  Function used to get the minimum and maximum offsets that can be applied to a given margin parameter and the time delay in micro seconds for the new value to take effect.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[in]      Rank        - Zero based physical rank number.
  @param[in]      LaneMasks   - Pointer to array of masks of the lanes/strobes that participate to the margin parameter offset limit calculation.  It is applicable only if margin parameter is per-strobe or per-bit/lane controllable.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  If a margin parameter is strobe specific and if any lane associated with that strobe is set in the mask then that strobe is selected.  For example, for a LaneMasks value of [0x00, … 0x00, 0x01], only the min/max offset of the first strobe group is returned.
  @param[in]      IoLevel     - I/O level.
  @param[in]      MarginGroup - Margin group.
  @param[out]     MinOffset   - Pointer to where the minimum offset from the current setting supported by the margin parameter will be stored.  This is a signed value.
  @param[out]     MaxOffset   - Pointer to where the maximum offset from the current setting supported by the margin parameter parameter will be stored.  This is a signed value.
  @param[out]     Delay       - Pointer to where the wait time in micro-seconds that is required for the new setting to take effect will be stored.
  @param[out]     StepUnit    - Pointer to where the margin parameter’s step size will be stored.  For timing parameters, the units are tCK / 2048.  For voltage parameters, the units are Vdd / 100.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetMarginParamLimits) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  CONST UINT8           *LaneMasks,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  INT16                 *MinOffset,
  INT16                 *MaxOffset,
  UINT16                *Delay,
  UINT16                *StepUnit
  )
{
  struct sysHost              *host;
  SSA_STATUS                  Status = Success;
  UINT8                       i;
  UINT8                       SetMarginFunctionFlag = 0;  //Set to have SSAGetSetMarginMaxMinOffset() behave as GetMarginParamLimits
  UINT8                       channelInSocket;
  UINT8                       arrayElements = 0;
  UINT8                       EccMask = 0;
  UINT8                       maxStrobe = 0;
  UINT8                       Loop = 0;
  UINT16                      MinDefaultDelayVal = 0;
  UINT16                      MaxDefaultDelayVal = 0;
  UINT16                      MinTrainingValOfGroup = 0;
  UINT16                      MaxTrainingValOfGroup = 0;
  INT16                       DataByte;
  INT16                       newValue = 0;
  INT16                       tempMaxOffsets[2] = {0,0};
  INT16                       tempMinOffsets[2] = {0,0};
  GSM_GT                      ActualMarginGroup;
  UINT64_STRUCT               DqMask1;
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];
  MARGIN_PARAM_SPECIFICITY    Specificity;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosGetMarginParamLimits (Socket:%d, Controller:%d, Channel:%d, Dimm:%d, Rank:%d, IoLevel:%d, MarginGroup:%d)\n", Socket, Controller, Channel, Dimm, Rank, IoLevel, MarginGroup));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);
  dimmNvList = GetDimmNvList(host, Socket, channelInSocket);

  //Check if the IoLevel and margin group is supported or not
  if (BiosGetMarginParamSpecificity (This, IoLevel, MarginGroup, &Specificity) == UnsupportedValue) {
    return UnsupportedValue;
  }

  //For 'LrbufLevel' a.) if margin groups CmdAll/CtlAll/CmdVref and "Rev02 register dimm" not present, Exit function. b.) LRDIMM not present, Exit.
  if (IoLevel == LrbufLevel) {
    // check if the margin group is support by Rev02 register or AEP DIMM
    if (((MarginGroup == CmdVref)) && ((!IsDdr4RegisterRev2(host, Socket, channelInSocket, Dimm)) && (!((*dimmNvList)[Dimm].aepDimmPresent)))) {
      return UnsupportedValue;
    }
    if (((MarginGroup == CmdAll) || (MarginGroup == CtlAll)) && ((!IsDdr4RegisterRev2(host, Socket, channelInSocket, Dimm)) && (!(((*dimmNvList)[Dimm].aepDimmPresent) && (host->setup.mem.enableNgnBcomMargining))))) {
      return UnsupportedValue;
    }
    // check if the margin group is supported by AEP DIMM
    if (((MarginGroup == CmdGrp3) || (MarginGroup == CmdGrp4)) && (!(((*dimmNvList)[Dimm].aepDimmPresent) && (host->setup.mem.enableNgnBcomMargining)))) {
      return UnsupportedValue;
    }
    // check if the dimm is LRDIMM
    if (((MarginGroup == RxDqsDelay) || (MarginGroup == TxDqDelay) || (MarginGroup == RxVref) || (MarginGroup == TxVref)) && (!CHIP_FUNC_CALL(host, IsLrdimmPresent(host, Socket, channelInSocket, Dimm)))){
      return UnsupportedValue;
    }
  }

  if (((MarginGroup == EridDelay) || (MarginGroup == EridVref)) && (!((*dimmNvList)[Dimm].aepDimmPresent))) {
    return UnsupportedValue;
  }

  DqMask1.lo = 0;
  DqMask1.hi = 0;

  *StepUnit = SSAGetMarginGroupStepSize(MarginGroup);  //Get the Step Size of the particular Margin Group

  //Getting the number of Array Elements
  if (host->nvram.mem.eccEn) { //BusWidth = 72(with Ecc), Array elements = 72/8 = 9
   arrayElements = 9;
  } else {
   arrayElements = 8;  //BusWidth = 64, Array elements = 64/8 = 8
  }

  //Separating the Array Elements into DQ Masks(lo, hi) and a ECC Mask
  for (i = 0; i < arrayElements; i++) {
    if (i < 4) {
      DqMask1.lo |= (UINT32) (((*(LaneMasks + i)) << i*8) & 0xFFFFFFFF);      // Saving of first 4 array elements into a 32 bit Mask (lo)
    } else if ((i >=4) && (i < 8)) {
      DqMask1.hi |= (UINT32) (((*(LaneMasks + i)) << (i-4)*8) & 0xFFFFFFFF);  // Saving of array elements 4-7 into a 32 bit Mask (hi)
    } else if (i == 8) {
      EccMask |= (UINT8)(*(LaneMasks + i) & 0xFF);
    }
  }

  switch (MarginGroup) {

    case RecEnDelay:
    case RxDqsDelay:
    case TxDqsDelay:
    case TxDqDelay:
    case RxDqsBitDelay:
    case TxDqBitDelay:
    case RxVref:
    case TxVref:
    case WrLvlDelay:
    case RxDqsPDelay:
    case RxDqsNDelay:
    case RxDqDelay:
    case RxEq:
    case TxEq:

      if ((MarginGroup == WrLvlDelay) || ((MarginGroup == RxDqsDelay) && (IoLevel == DdrLevel))) {
        Loop = 2;  //To get Limits for both (TxDqsDelay and TxDqDelay ~ WrLvlDelay) and (RxDqsPDelay and RxDqsNDelay) for the RxDqsDelay
      } else {
        Loop = 1;
      }

      while (Loop != 0) {
        if ((MarginGroup == WrLvlDelay) && (Loop == 2)) {
          ActualMarginGroup = TxDqsDelay;
        } else if ((MarginGroup == WrLvlDelay) && (Loop == 1)) {
          ActualMarginGroup = TxDqDelay;
        } else if ((MarginGroup == RxDqsDelay) && (IoLevel == DdrLevel) && (Loop == 2)) {
          ActualMarginGroup = RxDqsPDelay;
        }else if ((MarginGroup == RxDqsDelay) && (IoLevel == DdrLevel) && (Loop == 1)) {
          ActualMarginGroup = RxDqsNDelay;
        } else {
          ActualMarginGroup = MarginGroup;
        }

        CHIP_FUNC_CALL(host, GetDataGroupLimits(host, IoLevel, ActualMarginGroup, &MinDefaultDelayVal, &MaxDefaultDelayVal, Delay));  // Get Min/MaxOffsetDefault and Delay
        CHIP_FUNC_CALL(host, UpdateDdrtGroupLimits (host, Socket, channelInSocket, Dimm, IoLevel, ActualMarginGroup, &MinDefaultDelayVal, &MaxDefaultDelayVal));
        *MaxOffset = 0x7FFF; //MaxOffsetDefault; // Start with the largest value for a INT16
        *MinOffset = - (0x7FFF); //LARGE_MIN_OFFSET; // Start with the smallest value for a INT16

        maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, Socket, channelInSocket, Dimm, ActualMarginGroup, IoLevel));

        //Calling the SSA Helper function which calculates the Min/Max Offset values based of the DataByte value (calculated from GetSetDataGroup() in the helper)
        SSAGetSetMarginMaxMinOffset(host, Socket, channelInSocket, Dimm, Rank, SetMarginFunctionFlag, IoLevel, ActualMarginGroup, MinOffset, MaxOffset, MinDefaultDelayVal, MaxDefaultDelayVal, maxStrobe, &DataByte, &newValue, DqMask1.lo, DqMask1.hi, EccMask);

        if (((MarginGroup == WrLvlDelay) || ((MarginGroup == RxDqsDelay) && (IoLevel == DdrLevel))) && (Loop == 2)) {
          tempMaxOffsets[1] = *MaxOffset;
          tempMinOffsets[1] = *MinOffset;
        } else if (((MarginGroup == WrLvlDelay) || ((MarginGroup == RxDqsDelay) && (IoLevel == DdrLevel))) && (Loop == 1)) {
          tempMaxOffsets[0] = *MaxOffset;
          tempMinOffsets[0] = *MinOffset;
        }
        Loop = Loop - 1;
      } //while

      //WrLvlDelay margin parameter move both TxDqsDelay and TxDqDelay
      // RxDqsDelay is similar
      //WrLvlDelay limits: Take the Min of the 2 Max's and the Max of the two Min's
      if ((MarginGroup == WrLvlDelay) || ((MarginGroup == RxDqsDelay) && (IoLevel == DdrLevel))){
        *MaxOffset = MIN(tempMaxOffsets[1], tempMaxOffsets[0]);
        *MinOffset = MAX(tempMinOffsets[1], tempMinOffsets[0]);
      }

      //override timing parameter limit to within +/-32
      //override voltage parameter limit to within +/-64
      if ((MarginGroup == RxVref) || (MarginGroup == TxVref)) {
        if (*MaxOffset > MAX_VOLTAGE_MARGIN_PAMRAM_OFFSET ) {
          *MaxOffset = MAX_VOLTAGE_MARGIN_PAMRAM_OFFSET;
        }
        if (*MinOffset < MIN_VOLTAGE_MARGIN_PAMRAM_OFFSET) {
          *MinOffset = MIN_VOLTAGE_MARGIN_PAMRAM_OFFSET;
        }
      } else if ((MarginGroup == WrLvlDelay) || (MarginGroup == RecEnDelay)) {
        if (*MaxOffset > (2 * MAX_TIMING_MARGIN_PAMRAM_OFFSET + 1)) {
          *MaxOffset = (2 * MAX_TIMING_MARGIN_PAMRAM_OFFSET + 1);
        }
        if (*MinOffset < 2 * MIN_TIMING_MARGIN_PAMRAM_OFFSET) {
          *MinOffset = 2 * MIN_TIMING_MARGIN_PAMRAM_OFFSET;
        }
      } else {
        //timing margin parameter
        if (*MaxOffset > MAX_TIMING_MARGIN_PAMRAM_OFFSET) {
          *MaxOffset = MAX_TIMING_MARGIN_PAMRAM_OFFSET;
        }
        if (*MinOffset < MIN_TIMING_MARGIN_PAMRAM_OFFSET) {
          *MinOffset = MIN_TIMING_MARGIN_PAMRAM_OFFSET;
        }
      }  // end timing margin parameters

      break;

    case CmdGrp0:
    case CmdGrp1:
    case CmdGrp2:
    case CmdGrp3:
    case CmdGrp4:
    case CmdAll:
      //Calling GetCmdGroupLimits() to return the Limits for MinOffsetDefault/MaxOffsetDefault for the CMD Group
      CHIP_FUNC_CALL(host, GetCmdGroupLimits (host, Socket, IoLevel, MarginGroup, &MinDefaultDelayVal, &MaxDefaultDelayVal));

      //For backside REV2 margining different min/max values are used
      if (IoLevel == LrbufLevel) {
        if (!((*dimmNvList)[Dimm].aepDimmPresent)) {
          MaxDefaultDelayVal = 31;
          MinDefaultDelayVal = 0;
        } else {
          MaxDefaultDelayVal = 127;
          MinDefaultDelayVal = 0;
        }
      }

      //Initialize the min/max of all group value before calling GetSetCmdGroupDelay()
      //The GetSetCmdGroupDelay() iterate each groups, and compare it to the min/max, then update the min/max.
      MinTrainingValOfGroup = MaxDefaultDelayVal;
      MaxTrainingValOfGroup = MinDefaultDelayVal;

      //Calling GetSetCmdGroupDelay() to return the Limits for MinTrainingValOfGroup/MaxTrainingValOfGroup of that particular CMD Group entered
      GetSetCmdGroupDelayCore (host, Socket, channelInSocket, Dimm, IoLevel, MarginGroup, GSM_READ_ONLY, &DataByte, &MinTrainingValOfGroup, &MaxTrainingValOfGroup);
      *MaxOffset = MaxDefaultDelayVal - MaxTrainingValOfGroup;
      *MinOffset = MinDefaultDelayVal - MinTrainingValOfGroup;
      break;

    case CmdVref:
      GetSetCmdVrefCore (host, Socket, channelInSocket, Dimm, IoLevel, GSM_READ_ONLY, &DataByte);
      if (!((*dimmNvList)[Dimm].aepDimmPresent) && (IoLevel == LrbufLevel || (IoLevel == DdrLevel && !(CHIP_FUNC_CALL(host, CaVrefSelect(host)) & 0x8) && (host->nvram.mem.dimmTypePresent == RDIMM)))) {
        *MaxOffset = 20 - DataByte;      // based on the range defined in the GetSetCmdVrefCore() of the \Core\Mem\MemJedec.c
        *MinOffset = -20 - DataByte;
      } else {
        *MaxOffset = MEM_CHIP_POLICY_VALUE(host, maxCmdVref) - DataByte;
        *MinOffset = 0 - DataByte;
      }

      break;

    case CtlGrp0:
    case CtlGrp1:
    case CtlGrp2:
    case CtlGrp3:
    case CtlGrp4:
    case CtlGrp5:
    case CtlAll:
      //Calling GetCmdGroupLimits() to return the Limits for MinOffsetDefault/MaxOffsetDefault for the CTL Group - CTL/CMD group limits same
      CHIP_FUNC_CALL(host, GetCmdGroupLimits (host, Socket, IoLevel, MarginGroup, &MinDefaultDelayVal, &MaxDefaultDelayVal));

      //For backside REV2 margining different min/max values are used
      if (IoLevel == LrbufLevel) {
        if (!((*dimmNvList)[Dimm].aepDimmPresent)) {
          MaxDefaultDelayVal = 31;
          MinDefaultDelayVal = 0;
        } else {
          MaxDefaultDelayVal = 127;
          MinDefaultDelayVal = 0;
        }
      }

      //Initialize the min/max of all group value before calling GetSetCmdGroupDelay()
      //The GetSetCmdGroupDelay() iterate each groups, and compare it to the min/max, then update the min/max.
      MinTrainingValOfGroup = MaxDefaultDelayVal;
      MaxTrainingValOfGroup = MinDefaultDelayVal;

      //Calling GetSetCtlGroupDelay() to return the Limits for MinTrainingValOfGroup/MaxTrainingValOfGroup of that particular CTL Group entered
      GetSetCtlGroupDelayCore (host, Socket, channelInSocket, Dimm, IoLevel, MarginGroup, GSM_READ_ONLY, &DataByte, &MinTrainingValOfGroup, &MaxTrainingValOfGroup);
      *MaxOffset = MaxDefaultDelayVal - MaxTrainingValOfGroup;
      *MinOffset = MinDefaultDelayVal - MinTrainingValOfGroup;
      break;

    case EridDelay:
       MinDefaultDelayVal = 0;
       MaxDefaultDelayVal = 128 * 255 + 64 + 63; // cycle delay, logical delay, pi delay
       //Calling GetSetClkDelay() to return the current logic + PI delay
       //Dimm+2 since to move the 'clk'. ERID is mapped to CLK 2 and CLK 3 on the SKX side. So, DDRT on DIMM0 maps to ->CLK 2; 1 ->CLK3.
       CHIP_FUNC_CALL(host, GetSetClkDelay (host, Socket, channelInSocket, Dimm + 2, GSM_READ_ONLY, (INT16 *)&DataByte));
       GetSetSxpGnt2Erid(host, Socket, channelInSocket, Dimm, GSM_READ_ONLY, &newValue);
       *MaxOffset = MaxDefaultDelayVal - newValue * 128 - DataByte;
       *MinOffset = MinDefaultDelayVal - newValue * 128 - DataByte;

       // cap the range to +/-128
       if (*MaxOffset > (MAX_TIMING_MARGIN_PAMRAM_OFFSET * 4)) {
         *MaxOffset = MAX_TIMING_MARGIN_PAMRAM_OFFSET * 4 + 3; // 127
       }
       if (*MinOffset < (MIN_TIMING_MARGIN_PAMRAM_OFFSET * 4) ) {
         *MinOffset = MIN_TIMING_MARGIN_PAMRAM_OFFSET * 4;  // -128
       }
       break;

    case EridVref:
       CHIP_FUNC_CALL(host, GetDataGroupLimits(host, IoLevel, MarginGroup, &MinDefaultDelayVal, &MaxDefaultDelayVal, Delay));
       CHIP_FUNC_CALL(host, GetSetEridVref(host, Socket, channelInSocket, GSM_READ_ONLY, &DataByte));
       *MaxOffset = MaxDefaultDelayVal - DataByte;
       *MinOffset = MinDefaultDelayVal - DataByte;
       break;

    default:
      Status = UnsupportedValue;
      break;
  } // end of switch

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosGetMarginParamLimits (MinOffset: %d, MaxOffset: %d, Delay: %d, StepUnit: %d)\n", *MinOffset, *MaxOffset, *Delay, *StepUnit));
#endif
  return Status;
}

/**
  Function used to set the offset of a margin parameter.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      Dimm          - Zero based DIMM number.
  @param[in]      Rank          - Zero based physical rank number.
  @param[in]      LaneMasks     - Pointer to array of masks of the lanes/strobes that participate to the margin parameter offset limit calculation.  It is applicable only if margin parameter is per-strobe or per-bit/lane controllable.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  If a margin parameter is strobe specific and if any lane associated with that strobe is set in the mask then that strobe is selected.  For example, for a LaneMasks value of [0x00, … 0x00, 0x01], only the min/max offset of the first strobe group is returned.
  @param[in]      IoLevel       - Id of the I/O level to access. Can be VmseLevel(0), DdrLevel(1), LrbufLevel(2).
  @param[in]      MarginGroup   - Id of the margin group. Can be RcvEna(0), RdT(1), WrT(2), WrDqsT(3), RdV(4) or WrV(5).
  @param[in]      CurrentOffset - Signed value of the current offset setting.  The range of valid values is available  via the GetMarginParamLimits() function.
  @param[in]      NewOffset     - Signed value of the new offset setting.  The range of valid values is available  via the GetMarginParamLimits() function.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosSetMarginParamOffset) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  CONST UINT8           *LaneMasks,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  INT16                 CurrentOffset,
  INT16                 NewOffset
  )
{
  struct sysHost              *host;
  SSA_STATUS                  SsaStatus = Success;
  UINT8                       i;
  UINT8                       SetMarginFunctionFlag = 1;  //Set to have SSAGetSetMarginMaxMinOffset() behave as SetMarginParamOffset
  UINT8                       channelInSocket;
  UINT8                       arrayElements = 0;
  UINT8                       EccMask = 0;
  UINT8                       maxStrobe = 0;
  INT16                       MinOffset = 0;  //Used only by GetMarginParamLimits, so set to 0
  INT16                       MaxOffset = 0;  //Used only by GetMarginParamLimits, so set to 0
  UINT16                      MinOffsetDefault = 0;
  UINT16                      MaxOffsetDefault = 0;
  INT16                       DataByte = 0;
  INT16                       newValue = 0;
  INT16                       totalValue;
  UINT64_STRUCT               DqMask1;
  struct channelNvram         (*channelNvList)[MAX_CH];
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];
  BOOLEAN                     IoRegisterHasBeenChanged;
  MARGIN_PARAM_SPECIFICITY    Specificity;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosSetMarginParamOffset (Socket:%d, Controller:%d, Channel:%d, Dimm:%d, Rank:%d, IoLevel:%d, MarginGroup:%d, CurrentOffset:%d, NewOffset:%d)\n", Socket, Controller, Channel, Dimm, Rank, IoLevel, MarginGroup, CurrentOffset, NewOffset));
#endif
  channelNvList = GetChannelNvList(host, Socket);
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);
  dimmNvList = GetDimmNvList(host, Socket, channelInSocket);

  // check if the IoLevel and margin group is supported or not
  if (BiosGetMarginParamSpecificity (This, IoLevel, MarginGroup, &Specificity) == UnsupportedValue) {
    return UnsupportedValue;
  }

  if (IoLevel == LrbufLevel) {
    // check if the margin group is support by Rev02 register dimm
    if (((MarginGroup == CmdVref)) && ((!IsDdr4RegisterRev2(host, Socket, channelInSocket, Dimm)) && (!((*dimmNvList)[Dimm].aepDimmPresent)))) {
      return UnsupportedValue;
    }
    if (((MarginGroup == CmdAll) || (MarginGroup == CtlAll)) && ((!IsDdr4RegisterRev2(host, Socket, channelInSocket, Dimm)) && (!(((*dimmNvList)[Dimm].aepDimmPresent) && (host->setup.mem.enableNgnBcomMargining))))) {
      return UnsupportedValue;
    }
    // check if the margin group is supported by AEP DIMM
    if (((MarginGroup == CmdGrp3) || (MarginGroup == CmdGrp4)) && (!(((*dimmNvList)[Dimm].aepDimmPresent) && (host->setup.mem.enableNgnBcomMargining)))) {
      return UnsupportedValue;
    }
    // check if the dimm is LRDIMM
    if (((MarginGroup == RxDqsDelay) || (MarginGroup == TxDqDelay) || (MarginGroup == RxVref) || (MarginGroup == TxVref)) && (!CHIP_FUNC_CALL(host, IsLrdimmPresent(host, Socket, channelInSocket, Dimm)))){
      return UnsupportedValue;
    }
  }

  DqMask1.lo = 0;
  DqMask1.hi = 0;

  IoRegisterHasBeenChanged = FALSE;

  //Getting the number of Array Elements
  if (host->nvram.mem.eccEn) { //BusWidth = 72(with Ecc), Array elements = 72/8 = 9
   arrayElements = 9;
  } else {
   arrayElements = 8;  //BusWidth = 64, Array elements = 64/8 = 8
  }

  //Separating the Array Elements into DQ Masks(lo, hi) and a ECC Mask
  for (i=0; i < arrayElements; i++) {
    if (i < 4) {
      DqMask1.lo |= (UINT32) (((*(LaneMasks + i)) << i*8) & 0xFFFFFFFF);
    } else if ((i >=4) && (i < 8)) {
      DqMask1.hi |= (UINT32) (((*(LaneMasks + i)) << (i-4)*8) & 0xFFFFFFFF);
    } else if (i == 8) {
      EccMask |= (UINT8)(*(LaneMasks + i) & 0xFF);
    }
  }

  if (SsaStatus == Success) {
    newValue = NewOffset - CurrentOffset;  //CurrentOffset - NewOffset; //newValue --> the difference which will be applied to Cmd/Ctl/CmdVref

#ifndef ENBL_BIOS_SSA_DEBUG_MSGS
    disableAllMsg(host); // Turn off message to avoid printing the kind of "N0.C2.D0.R0: Write RC00 = 0x00" message
#endif

    //Saving newValue into concerned MarginGroup using the GSM_WRITE_OFFSET mode
    switch (MarginGroup) {
      case RecEnDelay:
      case RxDqsDelay:
      case TxDqsDelay:
      case TxDqDelay:
      case RxDqsBitDelay:
      case TxDqBitDelay:
      case RxVref:
      case TxVref:
      case WrLvlDelay:
      case RxDqsPDelay:
      case RxDqsNDelay:
      case RxDqDelay:
      case RxEq:
      case TxEq:

        maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, Socket, channelInSocket, Dimm, MarginGroup, IoLevel));

       //Calling the SSA Helper function which calculates/programs the "offset"  based of the newValue (calculated from GetSetDataGroup() in the helper)
       //WrLvlDelay: For this margin parameter we move TxDqsDelay and TxDqDelay; so the offset of both these parameters will be set when WrLvlDelay is called
       if (MarginGroup == WrLvlDelay) {  //To move WrLvlDelay we need to move TxDqsDelay  and TxDqDelay simultaneously
         SSAGetSetMarginMaxMinOffset(host, Socket, channelInSocket, Dimm, Rank, SetMarginFunctionFlag, IoLevel, TxDqsDelay, &MinOffset, &MaxOffset, MinOffsetDefault, MaxOffsetDefault, maxStrobe, &DataByte, &NewOffset, DqMask1.lo, DqMask1.hi, EccMask);
         SSAGetSetMarginMaxMinOffset(host, Socket, channelInSocket, Dimm, Rank, SetMarginFunctionFlag, IoLevel, TxDqDelay, &MinOffset, &MaxOffset, MinOffsetDefault, MaxOffsetDefault, maxStrobe, &DataByte, &NewOffset, DqMask1.lo, DqMask1.hi, EccMask);
       } else {
         SSAGetSetMarginMaxMinOffset(host, Socket, channelInSocket, Dimm, Rank, SetMarginFunctionFlag, IoLevel, MarginGroup, &MinOffset, &MaxOffset, MinOffsetDefault, MaxOffsetDefault, maxStrobe, &DataByte, &NewOffset, DqMask1.lo, DqMask1.hi, EccMask);
       }

        IoRegisterHasBeenChanged = TRUE;
        break;

      case CmdGrp0:
      case CmdGrp1:
      case CmdGrp2:
      case CmdGrp3:
      case CmdGrp4:
      case CmdAll:
        GetSetCmdGroupDelayCore (host, Socket, channelInSocket, Dimm, IoLevel, MarginGroup, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &newValue, &MinOffsetDefault, &MaxOffsetDefault); //Min, Max value gets updated on calling the function
        IoRegisterHasBeenChanged = TRUE;
        break;

      case CmdVref:
        GetSetCmdVrefCore (host, Socket, channelInSocket, Dimm, IoLevel, GSM_FORCE_WRITE |GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &newValue); 
        IoRegisterHasBeenChanged = TRUE;
        break;

      case CtlGrp0:
      case CtlGrp1:
      case CtlGrp2:
      case CtlGrp3:
      case CtlGrp4:
      case CtlGrp5:
      case CtlAll:
        GetSetCtlGroupDelayCore (host, Socket, channelInSocket, Dimm, IoLevel, MarginGroup, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &newValue, &MinOffsetDefault, &MaxOffsetDefault); //Min, Max value gets updated on calling the function
        IoRegisterHasBeenChanged = TRUE;
        break;

      case EridDelay:
        // read the current MC cycle delay value and IO logical/PI delay, calculate the new non-offset value
        CHIP_FUNC_CALL(host, GetSetClkDelay (host, Socket, channelInSocket, Dimm + 2, GSM_READ_ONLY, &DataByte));
        CHIP_FUNC_CALL(host, GetSetSxpGnt2Erid (host, Socket, channelInSocket, Dimm, GSM_READ_ONLY, &newValue));
        totalValue = (newValue * 128) + DataByte + NewOffset - CurrentOffset;
        newValue = totalValue / 128;
        CHIP_FUNC_CALL(host, GetSetSxpGnt2Erid (host, Socket, channelInSocket, Dimm, GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &newValue));
        DataByte = totalValue % 128;
        CHIP_FUNC_CALL(host, GetSetClkDelay (host, Socket, channelInSocket, Dimm + 2, GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &DataByte));
        IoRegisterHasBeenChanged = TRUE;
        break;

      case EridVref:
         CHIP_FUNC_CALL(host, GetSetEridVref(host, Socket, channelInSocket, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &newValue));
        IoRegisterHasBeenChanged = TRUE;
        break;

      default:
        SsaStatus = UnsupportedValue;
        break;
    }//MarginGroup

#ifndef ENBL_BIOS_SSA_DEBUG_MSGS
    restoreMsg(host);
#endif

    // need to reset IO
    if (IoRegisterHasBeenChanged == TRUE) {
     BiosIoReset(This, Socket, Controller);
    }
  }//if (SsaStatus == Success)

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosSetMarginParamOffset\n"));
#endif
  return SsaStatus;
} //BiosSetMarginParamOffset

/**
  Function used to set the ZQCal configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether ZQCal is enabled.  TRUE enables ZQCal; FALSE disables it.
  @param[out]     PrevEnable - Pointer to where the previous ZQCal enable setting will be stored.  This value may be NULL.

  @retval Nothing.
**/
VOID
(EFIAPI BiosSetZQCalConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  )
{
  struct sysHost                            *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosSetZQCalConfig (Socket:%d, Controller:%d, Enable:%d)\n", Socket, Controller, Enable));
#endif

  CHIP_FUNC_CALL(host, BiosSetZQCalConfigChip (host, Socket, Controller, Enable, PrevEnable));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosSetZQCalConfig:0x%x\n", *PrevEnable));
#endif
}

/**
  Function used to set the RComp update configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether RComp updates are enabled.  TRUE enables RComp updates; FALSE disables them.
  @param[out]     PrevEnable - Pointer to where the previous RComp update enable setting will be stored.  This value may be NULL.

  @retval Nothing.
**/
VOID
(EFIAPI BiosSetRCompUpdateConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  )
{
  struct sysHost                 *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosSetRCompUpdateConfig (Socket:%d, Controller:%d, Enable:%d)\n", Socket, Controller, Enable));
#endif

  CHIP_FUNC_CALL(host, BiosSetRCompUpdateConfigChip (host, Socket, Enable, PrevEnable));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosSetRCompUpdateConfig:0x%x\n", *PrevEnable));
#endif
}

/**
  Function used to set the page-open configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether page-open is enabled.  TRUE enables page-open; FALSE disables it.
  @param[out]     PrevEnable - Pointer to where the previous page-open enable setting will be stored.  This value may be NULL.

  @retval Nothing.
**/
VOID
(EFIAPI BiosSetPageOpenConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  )
{
  struct sysHost                            *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosSetPageOpenConfig (Socket:%d, Controller:%d, Enable:%d)\n", Socket, Controller, Enable));
#endif

  CHIP_FUNC_CALL(host, BiosSetPageOpenConfigChip (host, Socket, Controller, Enable, PrevEnable));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosSetPageOpenConfig:0x%x\n", *PrevEnable));
#endif
}

/**
  Function used to clear all memory.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.

  @retval NotAvailable if function is not supported by the BIOS.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosScrubMemory) (
  SSA_BIOS_SERVICES_PPI *This
  )
{
  struct sysHost                     *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData; // wipeMemory -> Write 0 to all memory
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosScrubMemory ()\n"));
#endif

  if (0 != MemInit(host)) { //If successfull MemInit() return 0 (Success = 0)
    return Failure;
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosScrubMemory\n"));
#endif
  return Success;
}

/**
  Function used to initialize CPGC engine(s) for all channels of a given memory
  controller.  Prior to configuring/using the CPGC engine, this function should
  be called with the SetupCleanup input parameter set to Setup.  When finished
  with the CPGC engine, this function should be called with the SetupCleanup
  input parameter set to Cleanup.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      SetupCleanup - Specifies setup or cleanup action.

  @retval Nothing.
**/
VOID
(EFIAPI BiosInitCpgc) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  SETUP_CLEANUP         SetupCleanup
  )
{
  struct sysHost                               *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosInitCpgc (Socket:%d, Controller:%d, SetupCleanup:%d)\n", Socket, Controller, SetupCleanup));
#endif

  CHIP_FUNC_CALL(host, BiosInitCpgcChip (host, Socket, Controller, SetupCleanup));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosInitCpgc\n"));
#endif
}

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
// end file BiosSsaMemoryConfig.c
