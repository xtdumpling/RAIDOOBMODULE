//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//
//  Rev. 1.06
//    Bug Fix: Fix SMBIOS Type 17 Manufacturer is incorrect when install 
//             Memory that has no manufacturer.
//    Reason : 
//    Auditor: William Hsu
//    Date   : Jun/06/2017
//
//
//  Rev. 1.05
//   Bug Fix:  Fixed cburn MEM Slt number is incorrect issue.
//   Reason:   Cburn checked MemoryFormFactor information to calculate Slt numbers.
//   Auditor:  Stephen Chen
//   Date:     Apr/05/2017
//
//  Rev. 1.04
//   Bug Fix:  Add NVDIMM Vender ID.
//   Reason:  
//   Auditor:  Leon Xu
//   Date:     Feb/25/2017
//
//  Rev. 1.03
//   Bug Fix:  Correct Bank Locator strings.
//   Reason:  
//   Auditor:  Jacker Yeh
//   Date:     Nov/03/2016
//
//  Rev. 1.02
//   Bug Fix:  1.Update SMBIOS Type 17 Bank Locator as "P0_Node0_Channel0_Dimm0"
//             2.Move Manufacturing Dat to AssetTag field's tail (date :Year/Week) of Smbios Type 17.  
//   Reason:  
//   Auditor:  Jimmy Chiu
//   Date:     Oct/21/2016
//
//  Rev. 1.01
//   Bug Fix:  Support fixed SMBIOS type 17 handle number.
//   Reason:  
//   Auditor:  Ivern Yeh
//   Date:     Jul/11/2016
//
//  Rev. 1.00
//   Bug Fix:  1.Add Manufacturing Date in Part number field 's tail (date :Year/Week) of Smbios Type 17.  
//   Reason:   
//   Auditor:  Chen Lin
//   Date:     Jul/01/2016
//
//***************************************************************************
//
/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

  Module Description:

  This is the driver that locates the MemoryConfigurationData Variable, if it
  exists, add the related SMBIOS tables by PI SMBIOS protocol.

  Copyright (c) 1999 - 2016 Intel Corporation.  All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "MemorySubClass.h"
#include "Platform.h"
#include "UncoreCommonIncludes.h"
#include "MemHost.h"
#include "EnhancedWarningLog.h"
#include "Token.h" // Supermicro

EFI_STRING_ID mDimmToDevLocator[] = {
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_0), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_1),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_2), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_3),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_4), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_5),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_6), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_7),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_8), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_9),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_A), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_B),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_C), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_D),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_E), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_F),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_10), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_11),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_12), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_13),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_14), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_15),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_16), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_17),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_18), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_19),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_1A), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_1B),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_1C), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_1D),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_1E), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_1F),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_20), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_21),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_22), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_23),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_24), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_25),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_26), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_27),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_28), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_29),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_2A), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_2B),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_2C), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_2D),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_2E), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_2F),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_30),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_31), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_32),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_33), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_33),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_35), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_35),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_37), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_37),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_39), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_39),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_3A), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_3B),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_3C), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_3D),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_3E), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_3F),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_40), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_41),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_42), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_43),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_44), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_45),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_46), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_47),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_48), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_49),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_4A), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_4B),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_4C), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_4D),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_4E), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_4F),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_50), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_51),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_52), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_53),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_54), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_55),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_56), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_57),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_58), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_59),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_5A), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_5B),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_5C), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_5D),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_5E), STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_5F),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_DIMM_60)
};

#if SMCPKG_SUPPORT == 0
EFI_STRING_ID mDimmToBankLocator[] = {
  STRING_TOKEN(STR_MEMORY_SUBCLASS_NODE_1), STRING_TOKEN(STR_MEMORY_SUBCLASS_NODE_2),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_NODE_3), STRING_TOKEN(STR_MEMORY_SUBCLASS_NODE_4),
//APTIOV_SERVER_OVERRIDE_START: Incorrect node numbers
  STRING_TOKEN(STR_MEMORY_SUBCLASS_NODE_5), STRING_TOKEN(STR_MEMORY_SUBCLASS_NODE_6),
  STRING_TOKEN(STR_MEMORY_SUBCLASS_NODE_7), STRING_TOKEN(STR_MEMORY_SUBCLASS_NODE_8),
//APTIOV_SERVER_OVERRIDE_END: Incorrect node numbers
};
#else
EFI_STRING_ID mDimmToBankLocator[] = {
//P0
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_0_CH0_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_0_CH0_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_0_CH1_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_0_CH1_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_0_CH2_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_0_CH2_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_1_CH0_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_1_CH0_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_1_CH1_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_1_CH1_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_1_CH2_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P0_NODE_1_CH2_DIMM1),
//P1
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_0_CH0_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_0_CH0_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_0_CH1_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_0_CH1_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_0_CH2_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_0_CH2_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_1_CH0_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_1_CH0_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_1_CH1_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_1_CH1_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_1_CH2_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P1_NODE_1_CH2_DIMM1),
//P2
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_0_CH0_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_0_CH0_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_0_CH1_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_0_CH1_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_0_CH2_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_0_CH2_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_1_CH0_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_1_CH0_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_1_CH1_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_1_CH1_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_1_CH2_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P2_NODE_1_CH2_DIMM1),
//P3  
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_0_CH0_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_0_CH0_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_0_CH1_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_0_CH1_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_0_CH2_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_0_CH2_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_1_CH0_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_1_CH0_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_1_CH1_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_1_CH1_DIMM1),
  STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_1_CH2_DIMM0), STRING_TOKEN(STR_SMC_MEMORY_SUBCLASS_P3_NODE_1_CH2_DIMM1),
};
#endif

EFI_GUID mEfiMemorySubClassDriverGuid = EFI_MEMORY_SUBCLASS_DRIVER_GUID;
typedef struct {
  UINT16  MfgId;
  CHAR16  String[16];
} MEMORY_MODULE_MANUFACTURER_LIST;

MEMORY_MODULE_MANUFACTURER_LIST MemoryModuleManufacturerList[] =
{
// SMCPKG_SUPPORT > +
  {MFGID_INNODISK,  L"Innodisk"},
  {MFGID_NETLIST,   L"Netlist"},
  {MFGID_AGIGA,     L"Agiga"},
// SMCPKG_SUPPORT < +
  {MFGID_MICRON,    L"Micron"},
  {MFGID_SAMSUNG,   L"Samsung"},
  {MFGID_MONTAGE,   L"Montage"},
  {MFGID_KINGSTON,  L"Kinston"},
  {MFGID_ELPIDA,    L"Elpida"},
  {MFGID_HYNIX,     L"Hynix"},
  {MFGID_INFINEON,  L"Infineon"},
  {MFGID_SMART,     L"Smart"},
  {MFGID_AENEON,    L"Aeneon"},
  {MFGID_QIMONDA,   L"Qimonda"},
  {MFGID_NEC,       L"NEC"},
  {MFGID_NANYA,     L"Nanya"},
  {MFGID_TI,        L"TI"},
  {MFGID_IDT,       L"IDT"},
  {MFGID_TEK,       L"TEK"},
  {MFGID_AGILENT,   L"Agilent"},
  {MFGID_INPHI,     L"Inphi"},
  {MFGID_INTEL,     L"Intel"},
  {MFGID_VIKING,    L"Viking"}
};

CHAR8
FilterCharacter (
  IN CHAR8    _char
)
{

  if( (_char >= 0x20 && _char <= 0x7E) ) {
    return _char;
  }

    return ' ';
}

#ifdef PC_HOOK
/**

Routine Description:
   Get Logical to Physical rank ID

Arguments:
    NodeId
    DdrCh
    Logical Rank

Returns:
  Physical Rank Id

--*/
UINT8
GetLogicalToPhysicalRankId(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  LogicalRank
 )
{
  UINT8   csMapping2DPC[] = {0, 1, 2, 3, 4, 5, 6, 7};
  UINT8   phyRank = 0xFF;

  if (LogicalRank < 8) {
    phyRank = csMapping2DPC[LogicalRank];
  }

  return phyRank;
}


/*++
Routine Description:
  This function is invoked when calculating spare DIMM size.
  Assuming that all Logical Rank size is the same on this Channel,
  because of cannot get spare rank size from the SystemMemoryMapHob.

Arguments:
  SystemMemoryMap
  NodeId
  DdrCh
  EnabledLogicalRanks

Returns:
  Rank Size.

--*/
UINT16
GetRankSize(
    IN struct SystemMemoryMapHob   *SystemMemoryMap,
    IN UINT8                       NodeId,
    IN UINT8                       DdrCh,
    IN UINT8                       EnabledLogicalRanks
)
{
  UINT8           Dimm = 0;
  UINT16          ChnnelSize = 0;
  UINT8           Index;
  UINT8           SpareRank = 0xFF;
  UINT8           SpareRankNum = 0;
  UINT8           EnableRankNum = 0;
  UINT16          RankSize = 0;
//
//Calculate total effective size on this Channel
//
  for(Dimm=0; Dimm < MAX_DIMM; Dimm++){
    if(SystemMemoryMap->Socket[NodeId].ChannelInfo[DdrCh].DimmInfo[Dimm].Present != 0){
      ChnnelSize = ChnnelSize + (SystemMemoryMap->Socket[NodeId].ChannelInfo[DdrCh].DimmInfo[Dimm].DimmSize>>4);
    }
  }
//
//Calculate total spare rank number on this Channel
//
  for(Index=0; Index< MAX_SPARE_RANK; Index++){
    SpareRank = SystemMemoryMap->Socket[NodeId].ChannelInfo[DdrCh].SpareLogicalRank[Index];
    if(SpareRank == 0xff){
      continue;
    }
    SpareRankNum++;
  }
//
//Calculate total enable rank number on this Channel
//
  for(Index=0; Index<MAX_RANK_CH; Index++){
    if((EnabledLogicalRanks>>Index) == 0){
      break;
    }
    if(((EnabledLogicalRanks>>Index) & 0x01) == 0x01){
      EnableRankNum++;
    }
  }
//
//Calculate Rank size.
//
  if(EnableRankNum > SpareRankNum ){
    RankSize = ChnnelSize/(EnableRankNum-SpareRankNum);
  }
  return RankSize;
}


STATIC
BOOLEAN
CheckSpareDimm(
  IN struct SystemMemoryMapHob  *SystemMemoryMap,
  IN UINT8                       node,
  IN UINT8                       Chl,
  IN UINT8                       Dimm
)
{
   BOOLEAN    ReturnValue = FALSE;
   UINT8      i;
   UINT8      LogicalRank;
   UINT8      PhysicalRank;

   if((SystemMemoryMap->RasModesEnabled & RK_SPARE) == 0){
     return ReturnValue;
   }
   
   for(i = 0; i < MAX_SPARE_RANK; i++){
     LogicalRank = SystemMemoryMap->Socket[node].ChannelInfo[Chl].SpareLogicalRank[i];
     if(LogicalRank == 0xff){
       continue;
     }

     PhysicalRank = GetLogicalToPhysicalRankId(node,Chl,LogicalRank);
     if(Dimm == (PhysicalRank/4)){
       ReturnValue = TRUE;
       break;
     }     
   }
   

   return     ReturnValue; 
}

UINT8
GetDimmSpareRankNum(
    IN struct SystemMemoryMapHob   *SystemMemoryMap,
    IN UINT8                       NodeId,
    IN UINT8                       DdrCh,
    IN UINT8                       Dimm
)
{
   UINT8      Index;
   UINT8      LogicalRank = 0xFF;
   UINT8      PhysicalRank;
   UINT8      SpareRankNum = 0;
   for(Index=0; Index< MAX_SPARE_RANK; Index++){
     LogicalRank = SystemMemoryMap->Socket[NodeId].ChannelInfo[DdrCh].SpareLogicalRank[Index];
     if(LogicalRank == 0xFF){
       continue;
     }

     PhysicalRank = GetLogicalToPhysicalRankId(NodeId,DdrCh,LogicalRank);

     if(Dimm == (PhysicalRank/4)){
       SpareRankNum++;
     }
   }
   return SpareRankNum;
}
#endif

#ifdef PC_HOOK
/*
Check if DIMM is failed or Disabled

Input:
   socket: socket index
   Channel: channel number on skt
   Dimm: Dimm number

Output:
  0 -- not found in warnng message
  1 -- Disabled dimm
  2 -- Failed Dimm

*/
EFI_GUID             EWLDataGuid = EWL_ID_GUID;

STATIC
UINT8 
DetectDimmStatus(
  struct SystemMemoryMapHob  *SystemMemoryMap,
  IN UINT8     Socket,
  IN UINT8     Chan,
  IN UINT8     Dimm)
{

   UINT8                   ReturnValue = 0;
#ifdef ENHANCED_WARNING_LOG
   EFI_HOB_GUID_TYPE       *GuidHob;
   EWL_PRIVATE_DATA        *ewlPrivateData; 
   EWL_ENTRY_HEADER         *WarningHeader;
   EWL_ENTRY_TYPE1          *Type1Warning = NULL;
   EWL_ENTRY_TYPE2         *Type2Warning;
   EWL_ENTRY_TYPE3         *Type3Warning;
   EWL_ENTRY_TYPE4         *Type4Warning;
   EWL_ENTRY_TYPE5         *Type5Warning;
#else
  struct warningLog        *pStatus;
#endif
  UINT32                    MaxWarningMsg;
  UINT32                    i;
  UINT8                     SktMsg;
  UINT8                     ChOnSktMsg;
  UINT8                     DimmMsg;
  UINT8                     majorCode;
  UINT8                     minorCode;

#ifdef ENHANCED_WARNING_LOG
  GuidHob        = GetFirstGuidHob (&EWLDataGuid);
  ASSERT(GuidHob != NULL);
  ewlPrivateData = GET_GUID_HOB_DATA(GuidHob);
  MaxWarningMsg = ewlPrivateData->numEntries;
  DEBUG((EFI_D_ERROR,"MaxWarningMsg %x \n",MaxWarningMsg));
#else
   pStatus = NULL;
   pStatus = (struct warningLog   *)(VOID *)(&SystemMemoryMap->status);
   MaxWarningMsg = pStatus->index;
   if(MaxWarningMsg > MAX_LOG) MaxWarningMsg = MAX_LOG;  
#endif

#ifdef ENHANCED_WARNING_LOG
   i = 0;
   while (i < ewlPrivateData->status.Header.FreeOffset) {
    //Get next warning from EWL structure
    WarningHeader = (EWL_ENTRY_HEADER*) &(ewlPrivateData->status.Buffer[i]);
    DEBUG((EFI_D_ERROR,"WarningHeader Type %x Size %x Severity %x\n",WarningHeader->Type,WarningHeader->Size,WarningHeader->Severity));
    switch (WarningHeader->Type) {
    case EwlType1:
       Type1Warning = (EWL_ENTRY_TYPE1 *)WarningHeader;
       SktMsg = Type1Warning->MemoryLocation.Socket;
       ChOnSktMsg  = Type1Warning->MemoryLocation.Channel;
       DimmMsg = Type1Warning->MemoryLocation.Dimm;
       majorCode = Type1Warning->Context.MajorWarningCode;
       minorCode = Type1Warning->Context.MinorWarningCode;
       break;
    case EwlType2:
       Type2Warning = (EWL_ENTRY_TYPE2 *)WarningHeader;
       SktMsg = Type2Warning->MemoryLocation.Socket;
       ChOnSktMsg  = Type2Warning->MemoryLocation.Channel;
       DimmMsg = Type2Warning->MemoryLocation.Dimm;
       majorCode = Type2Warning->Context.MajorWarningCode;
       minorCode = Type2Warning->Context.MinorWarningCode;
       break;
    case EwlType3:
       Type3Warning = (EWL_ENTRY_TYPE3 *)WarningHeader;
       SktMsg = Type3Warning->MemoryLocation.Socket;
       ChOnSktMsg  = Type3Warning->MemoryLocation.Channel;
       DimmMsg = Type3Warning->MemoryLocation.Dimm;
       majorCode = Type3Warning->Context.MajorWarningCode;
       minorCode = Type3Warning->Context.MinorWarningCode;
       break;
    case EwlType4:
       Type4Warning = (EWL_ENTRY_TYPE4 *)WarningHeader;
       SktMsg = Type4Warning->MemoryLocation.Socket;
       ChOnSktMsg  = Type4Warning->MemoryLocation.Channel;
       DimmMsg = Type4Warning->MemoryLocation.Dimm;
       majorCode = Type4Warning->Context.MajorWarningCode;
       minorCode = Type4Warning->Context.MinorWarningCode;
       break;
    case EwlType5:
       Type5Warning = (EWL_ENTRY_TYPE5 *)WarningHeader;
       SktMsg = Type5Warning->MemoryLocation.Socket;
       ChOnSktMsg  = Type5Warning->MemoryLocation.Channel;
       DimmMsg = Type5Warning->MemoryLocation.Dimm;
       majorCode = Type5Warning->Context.MajorWarningCode;
       minorCode = Type5Warning->Context.MinorWarningCode;
      break;
    default:
       i += WarningHeader->Size;        
       continue;
    }

#else
   for(i=0;i< MaxWarningMsg; i++){
     SktMsg = (UINT8)(pStatus->log[i].data >> 24);
     ChOnSktMsg = (UINT8)(pStatus->log[i].data >> 16);
     DimmMsg = (UINT8)(pStatus->log[i].data >> 8);
     majorCode = (UINT8)(pStatus->log[i].code >> 8);
     minorCode = (UINT8)(pStatus->log[i].code);
#endif

     if((Socket == SktMsg) && (Chan == ChOnSktMsg)){
       if(Dimm == DimmMsg) {
         if(WARN_MEMTEST_DIMM_DISABLE == majorCode){
           ReturnValue = 2;
           break;
         }

       if(((majorCode == WARN_DIMM_COMPAT) && (minorCode == 0)) ||
          ((majorCode == WARN_DIMM_COMPAT) && (minorCode == WARN_DIMM_COMPAT_MINOR_MAX_RANKS)) ||
          ((majorCode == WARN_DIMM_COMPAT) && (minorCode == WARN_CHANNEL_MIX_ECC_NONECC))||
          ((majorCode == WARN_DIMM_COMPAT) && (minorCode == WARN_RANK_NUM)) ||
          ((majorCode == WARN_DIMM_COMPAT) && (minorCode == WARN_CHANNEL_CONFIG_NOT_SUPPORTED)) ||       
          ((majorCode == WARN_DIMM_COMPAT) && (minorCode == WARN_DIMM_COMPAT_MINOR_NOT_SUPPORTED)) ||       
          ((majorCode == WARN_DIMM_COMPAT) && (minorCode == WARN_DIMM_VOLTAGE_NOT_SUPPORTED)) ||     
          ((majorCode == WARN_RD_RCVEN) && (minorCode == WARN_ROUNDTRIP_EXCEEDED)) ||        
          ((majorCode == ERR_DIMM_COMPAT) && (minorCode == 0)) ||   
          ((majorCode == WARN_WR_DQ_DQS) && (minorCode == 0)) ||            
          ((majorCode == WARN_EARLY_RID) ) ||  
          ((majorCode == WARN_MEMORY_TRAINING) ) ||          
          ((majorCode == WARN_DQ_SWIZZLE_DISC) ) ||          
          ((majorCode == WARN_FPT_UNCORRECTABLE_ERROR) )       
         ){
            ReturnValue = 1;
            break;
         }

      if((majorCode == WARN_RDIMM_ON_UDIMM) ||
        (majorCode == WARN_UDIMM_ON_RDIMM) ||
        (majorCode == WARN_SODIMM_ON_RDIMM)) {
        ReturnValue = 1;
        break;
      }

      if((majorCode == WARN_DIMM_POP_RUL)&&(minorCode == WARN_DIMM_POP_RUL_MINOR_OUT_OF_ORDER)) {
        if(SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].Enabled){
          ReturnValue = 0;
          } else {
          ReturnValue = 1;
          }
        }
      }   
    }

#ifdef ENHANCED_WARNING_LOG
    i += WarningHeader->Size;
  }
#else
  }  
#endif
  return ReturnValue;
}
#endif

// APTIOV_SERVER_OVERRIDE_RC_START: Get IMC index
UINT8
GetMcIndexFromBitmap (
  UINT8 imcInterBitmap
  )
{
  UINT8 mc;

  // if this entry is IMC0/IMC1 interleaved then use MC0 since number of channels have to be the same > 1 IMC way
  if ((imcInterBitmap & BIT0) && (imcInterBitmap & BIT1)) {
    mc = 0;
  } else if (imcInterBitmap & BIT1) { // if this entry for MC1 only
    mc = 1;
  } else { // if this entry for MC0 only
    mc = 0;
  }
  return mc;
}
// APTIOV_SERVER_OVERRIDE_RC_END

#if SMCPKG_SUPPORT
UINT32
BCDDatetoUNI(UINT8 Date)
{
    UINT16 UniDate1 = 0;
    UINT16 UniDate2 = 0;
    UniDate1 = Date;
    UniDate1 = UniDate1 >> 4;
    UniDate1 |= 0x30;
    UniDate1 &= 0x00FF;
    UniDate2 = Date;
    UniDate2 &= 0x000F; 		// Bit7~Bit4 is useless at this point
    UniDate2 |= 0x30;    
    return ((UniDate2 << 16) | UniDate1);    
}
#endif

/**

    GC_TODO: add routine description

    @param ImageHandle - GC_TODO: add arg description
    @param SystemTable - GC_TODO: add arg description

    @retval EFI_NOT_FOUND - GC_TODO: add retval description
    @retval Status        - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
MemorySubClassEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_HII_HANDLE                HiiHandle;
  EFI_STATUS                    Status;
  EFI_HOB_GUID_TYPE             *GuidHob;
  struct SystemMemoryMapHob     *SystemMemoryMap;
  UINT8                         Socket, Ch, Dimm;
  UINT8                         Chan;
  BOOLEAN                       DimmPresent;

  EFI_SMBIOS_PROTOCOL           *Smbios;
  SMBIOS_TABLE_TYPE16           *Type16Record;
  SMBIOS_TABLE_TYPE17           *Type17Record;
  SMBIOS_TABLE_TYPE19           *Type19Record;
  //SMBIOS_TABLE_TYPE18           *Type18Record;
  SMBIOS_TABLE_TYPE20           *Type20Record;
  EFI_SMBIOS_HANDLE             MemArraySmbiosHandle;
  EFI_SMBIOS_HANDLE             MemArrayMappedAddrSmbiosHandle;
  EFI_SMBIOS_HANDLE             MemDevSmbiosHandle;
  EFI_SMBIOS_HANDLE             MemDevMappedAddrSmbiosHandle;
  UINTN                         TableSize;
  UINT8                         NumSlots;
  UINT64                        TotalMemorySize;
  UINT64                        DimmStartingAddressMb;
  UINT64                        DimmEndingAddressMb;
  UINT32                        MemoryCapacity;
  UINT32                        MemoryDeviceSize;
  UINT16                        MemorySpeed;
  UINT16                        MemoryTotalWidth;
  UINT16                        MemoryDataWidth;
  CHAR8                         *OptionalStrStart;
  UINTN                         DeviceLocatorStrLen;
  UINTN                         BankLocatorStrLen;
  UINTN                         ManufactureStrLen;
  UINTN                         SerialNumberStrLen;
  UINTN                         AssertTagStrLen;
  UINTN                         PartNumberStrLen;
  EFI_STRING_ID                  DeviceLocator;
  EFI_STRING_ID                  BankLocator;
  EFI_STRING_ID                  Manufacturer;
  EFI_STRING_ID                  SerialNumber;
  EFI_STRING_ID                  AssetTag;
  EFI_STRING_ID                  PartNumber;
  EFI_STRING                    DeviceLocatorStr;
  EFI_STRING                    BankLocatorStr;
  EFI_STRING                    ManufactureStr;
  EFI_STRING                    SerialNumberStr;
  EFI_STRING                    AssertTagStr;
  EFI_STRING                    PartNumberStr;
  UINTN                         StringBufferSize;
  EFI_STRING                    StringBuffer;
  UINT32                        MemInfoTotalMem;
  UINT16                        MemInfoMemFreq;
  UINT8                         Index = 0;
  UINT64                        MemoryAddress;
  UINT64                        RowLength;
  UINT32                        BspCpuidSignature;
  UINT32                        RegEax, RegEbx, RegEcx, RegEdx;
  UINT16                        MinimumVoltage;
  UINT16                        MaximumVoltage;
  UINT16                        ConfiguredVoltage;
  UINT8                         RasMode;
  UINT8                         MemoryErrorCorrection;
  UINT8                         Ranks;
  UINT8                         MemoryType;
#ifdef PC_HOOK
  UINT8                         DimmStatus;
#endif
  // APTIOV_SERVER_OVERRIDE_RC_START
  UINT8                         MemoryFormFactor;  // Added changes for NO DIMM populated.
  //
  // Update  Type 19,17 Structure.
  //
  UINT8				            PartitionWidth = 0;
  EFI_STRING                    	    StringBufferAsset;
  UINT8				            Type;
  EFI_SMBIOS_TABLE_HEADER       	    *Record;
  EFI_SMBIOS_HANDLE                         SearchType19Handle = SMBIOS_HANDLE_PI_RESERVED;
#if SMCPKG_SUPPORT && FixedType17Handle_SUPPORT == 0x00  
  UINT8                         SadIndex;
  UINT8                         ChannelInterBitmap;
  UINT8                         ChInterWays;
#endif // #if FixedType17Handle_SUPPORT == 0x00
  // APTIOV_SERVER_OVERRIDE_RC_END
#if SMCPKG_SUPPORT && FixedType17Handle_SUPPORT == 0x01
  EFI_SMBIOS_HANDLE             HandleArray[24];
  EFI_SMBIOS_HANDLE             PhysicalMemoryHandle[8];
  UINT8                         HandleCount = 0;
  UINT8                         PhysicalMemoryHandleCount = 0;
  UINT16                        DateWY = 0;
  UINT32                        DateTemp = 0; 
#endif // #if FixedType17Handle_SUPPORT == 0x01

  //SMBIOS TYPE 17
  MEMORY_MODULE_MANUFACTURER_LIST *IdTblPtr;
  UINT8                         PlatformMaxSocket;
  UINT8                         PlatformMaxHaCh;
  UINT8                         PlatformMaxDimm;
  UINT8                         MfdSize,MfdIndex;
  CHAR16                        *TempString=NULL;
  UINT16                        MAX_MemorySpeed;
  UINT8                         SPDPartnum,i;
  CHAR16                        StringBuffer2[SMBIOS_STRING_MAX_LENGTH];
  UINT64                        ExtendedMemoryCapacity = 0;
  UINT32                        MemInfoTotalNodeMem = 0;
  UINT64                        Type19StartingAddress = 0;
  UINT64                        Type19EndingAddress = 0;
  UINT8                         Node;
  UINT8                         StrIndex;
#ifdef PC_HOOK
  UINT32                        InfoTotalNodeMem = 0;
  UINT32                        SpareDimmSize;
#endif
  MemArrayMappedAddrSmbiosHandle = 0;
  //
  // Allocate Buffers
  //
  StringBufferSize = (sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH;
  StringBuffer = AllocateZeroPool (StringBufferSize);
  ASSERT (StringBuffer != NULL);
  if (StringBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TempString = AllocatePool (StringBufferSize);
  ASSERT (TempString != NULL);
  if (TempString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Allocate for Asset tag
  //
  StringBufferAsset = AllocatePool (StringBufferSize);
  ASSERT (StringBufferAsset != NULL);
  //
 // APTIOV_SERVER_OVERRIDE_RC_END : Allocate for Asset tag
  //

  GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
  if (GuidHob != NULL) {
    SystemMemoryMap = (struct SystemMemoryMapHob *) GET_GUID_HOB_DATA (GuidHob);
    RasMode = SystemMemoryMap->RasModesEnabled;
  } else {
    SystemMemoryMap = NULL;
    Status = EFI_DEVICE_ERROR;
    RasMode = 0xFF;
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  PlatformMaxSocket = (UINT8) PcdGet32(PcdOemSkuBoardSocketCount);
#ifdef SKXD_EN
  PlatformMaxHaCh = 4;
#else
  PlatformMaxHaCh   = (UINT8) PcdGet32(PcdOemSkuMaxChannel);
#endif
  PlatformMaxDimm   = (UINT8) PcdGet32(PcdOemSkuMaxDimmPerChannel);

  //
  // Check if MRC code has set the PCDs for platform or not
  //
  MemInfoTotalMem = (UINT32)(SystemMemoryMap->memSize << 6);
  MemInfoMemFreq  = SystemMemoryMap->memFreq;

  if ((MemInfoTotalMem == 0) || (MemInfoMemFreq == 0)) {
    return EFI_NOT_FOUND;
  }

  //
  // Locate dependent protocols
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, &Smbios);
  ASSERT (!EFI_ERROR (Status));

  //
  // Add our default strings to the HII database. They will be modified later.
  //
  HiiHandle = HiiAddPackages (
                &mEfiMemorySubClassDriverGuid,
                NULL,
                MemorySubClassStrings,
                NULL,
                NULL
                );

  ASSERT (!EFI_ERROR (Status));

  //
  // Get BSP CPU ID
  //
  AsmCpuid (0x01, &RegEax, &RegEbx, &RegEcx, &RegEdx);
  BspCpuidSignature = RegEax;

  //
  // Convert Total Memory size from MB to Bytes
  //
  TotalMemorySize = (UINT64) LShiftU64 (MemInfoTotalMem, 20);
  DEBUG ((EFI_D_INFO, "Total Memory Size = %dGB\n", MemInfoTotalMem >> 10));
  DEBUG ((EFI_D_INFO, "Memory Speed = %dMHz\n", MemInfoMemFreq));

  NumSlots = PlatformMaxHaCh * PlatformMaxDimm;

  // There can only be one Apache Pass DIMM per channel so the max capacity is 1 DDRT DIMM per channel plus
  // (n - 1) DDR4 DIMMs per channel (where n is the total number of DIMM slots per channel).  In the typical
  // case of 3 channels per iMC and 2 slots per channel, the max capacity per node (iMC) would be (x3 256GB
  // 16Gb 3DS DDR4 LRDIMM) and (x3 512GB Apache Pass mapped as memory).
  MemoryCapacity = (UINT32) ((PlatformMaxHaCh / MAX_IMC) * MAX_APACHEPASS_DIMM_SIZE +
                             ((NumSlots - PlatformMaxHaCh) / MAX_IMC) * MAX_DIMM_SIZE); // in GB
  
  //
  // Convert DDR Voltage to millivolts
  //
  switch (SystemMemoryMap->DdrVoltage) {
    // Currently DDR4 only supports 1.2v
    case SPD_VDD_120 :
      MinimumVoltage = 1200;
      MaximumVoltage = 1200;
      ConfiguredVoltage = 1200;
      break;
    default :
      MinimumVoltage = 0;
      MaximumVoltage = 0;
      ConfiguredVoltage = 0;
  }
  DEBUG ((EFI_D_INFO, "Configured DDR Voltage = %dmV\n", ConfiguredVoltage));

  //
  // Report top level Physical Memory Array to Type 16 SMBIOS Record
  //
  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //

  //
  // Get memory RAS mode
  //
  RasMode = SystemMemoryMap->RasModesEnabled;
  switch (RasMode) {
    case CH_INDEPENDENT:
      MemoryErrorCorrection = MemoryErrorCorrectionSingleBitEcc;
      break;
    case CH_MIRROR:
    case FULL_MIRROR_1LM:
    case PARTIAL_MIRROR_1LM:
    case FULL_MIRROR_2LM:
    case PARTIAL_MIRROR_2LM:
      MemoryErrorCorrection = MemoryErrorCorrectionMultiBitEcc;;
      break;
    case RK_SPARE:
      MemoryErrorCorrection = MemoryErrorCorrectionSingleBitEcc;
      break;
    case CH_LOCKSTEP:
      MemoryErrorCorrection = MemoryErrorCorrectionSingleBitEcc;
      break;
    default:
      MemoryErrorCorrection = MemoryErrorCorrectionMultiBitEcc;
  }
  DEBUG ((EFI_D_INFO, "RasMode:0x%x MemoryErrorCorrection:0x%x\n", RasMode, MemoryErrorCorrection));

  //
  // Accumulators for Type 20 structures
  //
  DimmStartingAddressMb = DimmEndingAddressMb = 0;

  for (Socket = 0; Socket < PlatformMaxSocket; Socket++) { // Loop for each CPU socket
    for(Node = 0; Node < MAX_IMC; Node++){
      Type16Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE16) + 1 + 1);
      ASSERT (Type16Record != NULL);
      if (Type16Record == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      ZeroMem(Type16Record, sizeof (SMBIOS_TABLE_TYPE16) + 1 + 1);

      Type16Record->Hdr.Type = EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY;
      Type16Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE16);
      Type16Record->Hdr.Handle = 0;
      Type16Record->Location = MemoryArrayLocationSystemBoard;
      Type16Record->Use = MemoryArrayUseSystemMemory;
      Type16Record->MemoryErrorCorrection = MemoryErrorCorrection;
      // APTIOV_SERVER_OVERRIDE_RC_START : Max Memory Capacity compliant with SMBIOS spec
      // If Max Capacity less than 2T, use MaximumCapacity store the value, else use ExtendedMaximumCapacity store it.
      if ( MemoryCapacity < 2048) {
      // APTIOV_SERVER_OVERRIDE_RC_END : Max Memory Capacity compliant with SMBIOS spec
        Type16Record->MaximumCapacity = MemoryCapacity << 20; // in KB
      } else {
        Type16Record->MaximumCapacity = 0x80000000;
        ExtendedMemoryCapacity = (UINT64) LShiftU64 (MemoryCapacity, 30); // in byte
        Type16Record->ExtendedMaximumCapacity = ExtendedMemoryCapacity;
      }
      Type16Record->MemoryErrorInformationHandle = 0xFFFE;
      Type16Record->NumberOfMemoryDevices = (UINT16) (NumSlots / MAX_IMC);

      //
      // Don't change it. This handle will be referenced by Type 17 and Type 19 SMBIOS Records
      //
      MemArraySmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
      Status = Smbios->Add (Smbios, NULL, &MemArraySmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type16Record);
#if SMCPKG_SUPPORT && FixedType17Handle_SUPPORT == 0x01  
      if (!EFI_ERROR(Status)) {
       PhysicalMemoryHandle[PhysicalMemoryHandleCount] = MemArraySmbiosHandle;
       PhysicalMemoryHandleCount++;
      }
#endif // #if FixedType17Handle_SUPPORT == 0x01

      FreePool (Type16Record);
      ASSERT_EFI_ERROR (Status);

      //
      // Report Memory Array Mapped Address to Type 19 SMBIOS Record
      //

      // Calculate the total memory size of current node.
#if SMCPKG_SUPPORT && FixedType17Handle_SUPPORT == 0x00  
#ifdef PC_HOOK
        for (Ch = 0; Ch < MAX_MC_CH; Ch++) { // Detect DIMMs on each channel
           for (Dimm = 0; Dimm < PlatformMaxDimm; Dimm++) {
              DimmPresent = TRUE;
              Chan = MAX_MC_CH * Node + Ch;
              DimmStatus = DetectDimmStatus(SystemMemoryMap,Socket,Chan,Dimm);
  
             if ( DimmStatus != 0 ||
                  SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].Enabled == 0 ||
                  SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].Present== 0 ) {
  
                  DimmPresent = FALSE;
             }
              if(DimmPresent) {
                 MemoryDeviceSize = SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].DimmSize; //in 64MB
                 MemoryDeviceSize = MemoryDeviceSize << 6; // in MB
              } else {
                 MemoryDeviceSize = 0;
              }
              InfoTotalNodeMem += (UINT32)MemoryDeviceSize;
 
          }
      }

      RasMode = SystemMemoryMap ->RasModesEnabled;
      if(RasMode & CH_ALL_MIRROR){
        InfoTotalNodeMem= InfoTotalNodeMem>>1;
      }
      MemInfoTotalNodeMem = MemInfoTotalNodeMem + InfoTotalNodeMem;
      InfoTotalNodeMem = 0;
      Type19EndingAddress = (UINT64)MemInfoTotalNodeMem << 10;  //in KB   
      if (Type19EndingAddress > Type19StartingAddress) {
#else

      MemInfoTotalNodeMem = (UINT32)(SystemMemoryMap->Socket[Socket].imc[Node].MemSize << 6);
      Type19EndingAddress += (UINT64) LShiftU64 (MemInfoTotalNodeMem, 10);
      if (MemInfoTotalNodeMem) {
#endif
        Type19Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE19) + 1 + 1);
        ASSERT (Type19Record != NULL);
        if (Type19Record == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        ZeroMem (Type19Record, sizeof (SMBIOS_TABLE_TYPE19) + 1 + 1);
        Type19Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS;
        Type19Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE19);
        Type19Record->Hdr.Handle = 0;
        Type19Record->MemoryArrayHandle = MemArraySmbiosHandle;
        Type19Record->PartitionWidth = NumSlots/2;

        // If the address is overflow, use ExtendedStartingAddress/ExtendedEndingAddress store it.
        if ((Type19StartingAddress < 0xFFFFFFFF) && (Type19EndingAddress < 0xFFFFFFFF)) {
          Type19Record->StartingAddress = (UINT32) Type19StartingAddress;
          Type19Record->EndingAddress = (UINT32)(Type19EndingAddress - 1); // In kilobytes
        } else {
          Type19Record->StartingAddress = 0xFFFFFFFF;
          Type19Record->EndingAddress = 0xFFFFFFFF;
          Type19Record->ExtendedStartingAddress = LShiftU64 (Type19StartingAddress, 10); // In bytes.
          Type19Record->ExtendedEndingAddress = LShiftU64 (Type19EndingAddress, 10) - 1; // In bytes.
        }
        Type19StartingAddress = Type19EndingAddress;

        //
        // Don't change it. This handle will be referenced by Type 20 SMBIOS Records
        //
        MemArrayMappedAddrSmbiosHandle= SMBIOS_HANDLE_PI_RESERVED;
        Status = Smbios->Add (Smbios, NULL, &MemArrayMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type19Record);
        FreePool (Type19Record);
        ASSERT_EFI_ERROR (Status);
      }
#endif // #if FixedType17Handle_SUPPORT == 0x00  


    //
    // Building Type 17 structures
    // Loop for each memory node
    //
    MfdSize = sizeof (MemoryModuleManufacturerList) / sizeof (MemoryModuleManufacturerList[0]);


    MemorySpeed = 0;
    MAX_MemorySpeed = 0;
#ifdef SKXD_EN
	for (Ch = 0; Ch < (PlatformMaxHaCh / MAX_IMC); Ch++) { // Detect DIMMs on each channel
#else
    for (Ch = 0; Ch < MAX_MC_CH; Ch++) { // Detect DIMMs on each channel
#endif
      for (Dimm = 0; Dimm < PlatformMaxDimm; Dimm++) {
        DimmPresent = TRUE;
        Chan = MAX_MC_CH * Node + Ch;

#ifdef PC_HOOK
        DimmStatus = DetectDimmStatus(SystemMemoryMap,Socket,Chan,Dimm);

        if ( DimmStatus != 0 ||
            SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].Enabled == 0 ||
            SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].Present== 0 ) {
            DimmPresent = FALSE;
        }
#else
       if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].Enabled == 0 ||
            SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].Present== 0 ) {
          DimmPresent = FALSE;
        }
#endif

        DeviceLocator = mDimmToDevLocator[Socket * PlatformMaxHaCh * PlatformMaxDimm + Chan * MAX_DIMM + Dimm]; //Zero based index
		// APTIOV_SERVER_OVERRIDE_RC_START : Fixed Type 17 doesn't match memory arrary in Type 16.
		//BankLocator = mDimmToBankLocator[Socket]; //Zero based index
#if SMCPKG_SUPPORT == 0
        BankLocator = mDimmToBankLocator[Socket*2+Node]; //Zero based index
#else
        BankLocator = mDimmToBankLocator[Socket*PlatformMaxHaCh*PlatformMaxDimm + Chan*MAX_DIMM + Dimm]; //Zero based index
#endif
		// APTIOV_SERVER_OVERRIDE_RC_END : Fixed Type 17 doesn't match memory arrary in Type 16.
        AssetTag = STRING_TOKEN (STR_MEMORY_SUBCLASS_SPACE);
        SerialNumber = STRING_TOKEN (STR_MEMORY_SUBCLASS_UNKNOWN);
        PartNumber = STRING_TOKEN (STR_MEMORY_SUBCLASS_UNKNOWN);

        if (DimmPresent) {
        //	
        //  APTIOV_SERVER_OVERRIDE_RC_START : Partition width is no of Dimms that forms the address so increment for every Dimm present.
        //  This will be added in Type 19 structure 
        //
            PartitionWidth++;
        //  APTIOV_SERVER_OVERRIDE_RC_END : Partition width is no of Dimms that forms the address so increment for every Dimm present.
          // Update Manufacturer
          IdTblPtr = MemoryModuleManufacturerList;
          ZeroMem(StringBuffer,StringBufferSize);
          for (MfdIndex = 0; MfdIndex < MfdSize; MfdIndex++) {
            if ((IdTblPtr[MfdIndex].MfgId == SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].SPDMMfgId)){
              CopyMem (StringBuffer, IdTblPtr[MfdIndex].String,(2*StrLen(IdTblPtr[MfdIndex].String)));
              break;
            }
          }
#if SMCPKG_SUPPORT
          if (MfdIndex == MfdSize)
          {
             CHAR16  UndefinedString[16] =  L"Undefined";
             CopyMem (StringBuffer, UndefinedString,(2*StrLen(UndefinedString))); 
          }   
#endif           
          Manufacturer = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
          // Update SerialNumber
          ZeroMem(StringBuffer2,StringBufferSize);
          ZeroMem(StringBuffer,StringBufferSize);
          for (i = 0; i <  4; i++) {
                  ZeroMem(TempString, StringBufferSize);
                  if ( SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].serialNumber[i] <= 0x0F) {
                    StrCpyS(TempString, SMBIOS_STRING_MAX_LENGTH, L"0");
                  }
                  UnicodeValueToString (
                          StringBuffer2,
                          RADIX_HEX,
                          SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].serialNumber[i],
                          2
                  );

                  StrCatS (TempString, SMBIOS_STRING_MAX_LENGTH, StringBuffer2);
                  StrCatS (StringBuffer, SMBIOS_STRING_MAX_LENGTH, TempString);
          }
          SerialNumber = HiiSetString (HiiHandle, 0, StringBuffer, NULL);

          // Update PartNumber
          SPDPartnum = 0;
          ZeroMem(StringBuffer,StringBufferSize);
          ZeroMem(TempString, StringBufferSize);
          for (i = 0; i < 20; i++) {
            SPDPartnum = SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].modelNumber[i];
            if (SPDPartnum == 0) SPDPartnum = 0x20;
            TempString[0] = FilterCharacter(SPDPartnum);
            StringBuffer[i] = TempString[0];
          }
          PartNumber = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
          #ifdef PC_HOOK


          if(CheckSpareDimm(SystemMemoryMap, Socket, Chan, Dimm)) {
            SpareDimmSize = GetDimmSpareRankNum(SystemMemoryMap, Socket, Chan, Dimm) * GetRankSize(SystemMemoryMap, Socket, Chan, SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].EnabledLogicalRanks);
            MemoryDeviceSize = (UINT32)((UINT16)SpareDimmSize << 4) + (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].DimmSize);
         }else{
          #endif
            MemoryDeviceSize = SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].DimmSize; //in 64MB
          #ifdef PC_HOOK
          }
          #endif
          MemoryDeviceSize = MemoryDeviceSize << 6; // in MB
          DimmEndingAddressMb = DimmStartingAddressMb + MemoryDeviceSize;
          Ranks = SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].NumRanks;
          MemoryType = MemoryTypeDdr4;
          // APTIOV_SERVER_OVERRIDE_RC_START : Added changes for NO DIMM populated.
          MemoryFormFactor = MemoryFormFactorDimm;
         // APTIOV_SERVER_OVERRIDE_RC_END : Added changes for NO DIMM populated.
          DEBUG ((EFI_D_INFO, "Socket - 0x%x Channel - 0x%x Dimm - 0x%x Dimm Size = %dGB\n", Socket, Chan, Dimm, MemoryDeviceSize >> 10));
          MemorySpeed = MemInfoMemFreq;
#ifdef PC_HOOK
          if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_4400_TCK_MIN) {
            MAX_MemorySpeed = 4400;   
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_4266_TCK_MIN) {
            MAX_MemorySpeed = 4266;       
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_4200_TCK_MIN) {
            MAX_MemorySpeed = 4200;    
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_4000_TCK_MIN) {
            MAX_MemorySpeed = 4000;        
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_3800_TCK_MIN) {
            MAX_MemorySpeed = 3800;    
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_3733_TCK_MIN) {
            MAX_MemorySpeed = 3733;       
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_3600_TCK_MIN) {
            MAX_MemorySpeed = 3600;    
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_3466_TCK_MIN) {
            MAX_MemorySpeed = 3466;      
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_3400_TCK_MIN) {
            MAX_MemorySpeed = 3400;             
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_3200_TCK_MIN) {
            MAX_MemorySpeed = 3200;            
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_3000_TCK_MIN) {
            MAX_MemorySpeed = 3000;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_2933_TCK_MIN) {
            MAX_MemorySpeed = 2933;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_2800_TCK_MIN) {
            MAX_MemorySpeed = 2800;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_2666_TCK_MIN) {
            MAX_MemorySpeed = 2666;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_2600_TCK_MIN) {
            MAX_MemorySpeed = 2600;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_2400_TCK_MIN) {
            MAX_MemorySpeed = 2400;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_2200_TCK_MIN) {
            MAX_MemorySpeed = 2200;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_2133_TCK_MIN) {
            MAX_MemorySpeed = 2133;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_2000_TCK_MIN) {
            MAX_MemorySpeed = 2000;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_1866_TCK_MIN) {
            MAX_MemorySpeed = 1866;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_1800_TCK_MIN) {
            MAX_MemorySpeed = 1800;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_1600_TCK_MIN) {
            MAX_MemorySpeed = 1600;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_1400_TCK_MIN) {
            MAX_MemorySpeed = 1400;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_1333_TCK_MIN) {
            MAX_MemorySpeed = 1333;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_1200_TCK_MIN) {
            MAX_MemorySpeed = 1200;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_1066_TCK_MIN) {
            MAX_MemorySpeed = 1066;
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck <= DDR_1000_TCK_MIN) {
            MAX_MemorySpeed = 1000;
          } else {
            MAX_MemorySpeed = 800;
          }
#else
          switch (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].commonTck) {
              case DDR_800_TCK_MIN:
                  MAX_MemorySpeed = 800;
                  break;
              case DDR_1000_TCK_MIN:
                  MAX_MemorySpeed = 1000;
                  break;
              case DDR_1066_TCK_MIN:
                  MAX_MemorySpeed = 1066;
                  break;
              case DDR_1200_TCK_MIN:
                  MAX_MemorySpeed = 1200;
                  break;
              case DDR_1333_TCK_MIN:
                  MAX_MemorySpeed = 1333;
                  break;
              case DDR_1400_TCK_MIN:
                  MAX_MemorySpeed = 1400;
                  break;
              case DDR_1600_TCK_MIN:
                  MAX_MemorySpeed = 1600;
                  break;
              case DDR_1800_TCK_MIN:
                  MAX_MemorySpeed = 1800;
                  break;
              case DDR_1866_TCK_MIN:
                  MAX_MemorySpeed = 1866;
                  break;
              case DDR_2000_TCK_MIN:
                  MAX_MemorySpeed = 2000;
                  break;
              case DDR_2133_TCK_MIN:
                  MAX_MemorySpeed = 2133;
                  break;
              case DDR_2200_TCK_MIN:
                  MAX_MemorySpeed = 2200;
                  break;
              case DDR_2400_TCK_MIN:
                  MAX_MemorySpeed = 2400;
                  break;
              case DDR_2600_TCK_MIN:
                  MAX_MemorySpeed = 2600;
                  break;
              case DDR_2666_TCK_MIN:
                  MAX_MemorySpeed = 2666;
                  break;
              case DDR_2800_TCK_MIN:
                  MAX_MemorySpeed = 2800;
                  break;
              case DDR_2933_TCK_MIN:
                  MAX_MemorySpeed = 2933;
                  break;
              case DDR_3000_TCK_MIN:
                  MAX_MemorySpeed = 3000;
                  break;
              case DDR_3200_TCK_MIN:
                  MAX_MemorySpeed = 3200;
                  break;
          }
#endif
          ///
          /// @todo Need a non-hard-coded way to get these
          ///
          MemoryTotalWidth = 72;
          MemoryDataWidth = MemoryTotalWidth - 8;
        } else {
          StrCpyS (StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"NO DIMM");
          Manufacturer = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
          SerialNumber = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
          PartNumber = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
          MemoryDeviceSize = 0;
          MemorySpeed = 0;
          MemoryTotalWidth = 0;
          MemoryDataWidth = 0;
          Ranks = 0;
          MAX_MemorySpeed = 0;
          MemoryType = MemoryTypeUnknown;
          // APTIOV_SERVER_OVERRIDE_RC_START : Added changes for NO DIMM populated.
#if SMCPKG_SUPPORT
          MemoryFormFactor = MemoryFormFactorDimm;
#else
          MemoryFormFactor = MemoryFormFactorUnknown;
#endif
          // APTIOV_SERVER_OVERRIDE_RC_END : Added changes for NO DIMM populated.
        }

        //
        // Get strings from HII database
        //

        DeviceLocatorStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, DeviceLocator, NULL);
        //
        // APTIOV_SERVER_OVERRIDE_RC_START : Asserting if string is NULL
        //
        if (DeviceLocatorStr == NULL) {
            DeviceLocatorStr = AllocatePool (sizeof (CHAR16) * SMBIOS_STRING_MAX_LENGTH);
            ASSERT (DeviceLocatorStr != NULL);
            StrCpy (DeviceLocatorStr, L"Details could not be retrieved");
        }
        //
        // APTIOV_SERVER_OVERRIDE_RC_END : Asserting if string is NULL
        //
        DeviceLocatorStrLen = ((DeviceLocatorStr == NULL) ? 0 : (StrLen (DeviceLocatorStr) + 1));
        ASSERT (DeviceLocatorStrLen <= SMBIOS_STRING_MAX_LENGTH);
        
        //
        // APTIOV_SERVER_OVERRIDE_RC_START : Update Asset Tag
        //
         if (DimmPresent) {
#if SMCPKG_SUPPORT == 0
           StrCpy (TempString, L"_AssetTag");
#else
           StrCpy (TempString, L"_AssetTag (date:YY/WW)");
           DateWY = SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].SPDModDate;
           DateTemp = BCDDatetoUNI((UINT8)(DateWY >> 8)); 
           CopyMem(&TempString[19], &DateTemp, 4);
           DateTemp = BCDDatetoUNI((UINT8)(DateWY));
           CopyMem(&TempString[16], &DateTemp, 4);
#endif   
           ZeroMem(StringBufferAsset,StringBufferSize);
           StrCat (StringBufferAsset, DeviceLocatorStr);
           StrCat (StringBufferAsset, TempString);
           AssetTag = HiiSetString (HiiHandle, 0, StringBufferAsset, NULL);
         } else {     
           StrCpy (StringBuffer, L"NO DIMM");  
           AssetTag = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
         }
        //
        // APTIOV_SERVER_OVERRIDE_RC_END : Update Asset Tag
        //

        BankLocatorStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, BankLocator, NULL);
        //
        // APTIOV_SERVER_OVERRIDE_RC_START : Asserting if string is NULL
        //
           if (BankLocatorStr == NULL) {
               BankLocatorStr = AllocatePool (sizeof (CHAR16) * SMBIOS_STRING_MAX_LENGTH);
               ASSERT (BankLocatorStr != NULL);
               StrCpy (BankLocatorStr, L"Details could not be retrieved");
           }
       //
       // APTIOV_SERVER_OVERRIDE_RC_END : Asserting if string is NULL
       //
        BankLocatorStrLen = ((BankLocatorStr == NULL) ? 0 : (StrLen (BankLocatorStr) + 1));
        ASSERT (BankLocatorStrLen <= SMBIOS_STRING_MAX_LENGTH);

        ManufactureStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, Manufacturer, NULL);
        //
        // APTIOV_SERVER_OVERRIDE_RC_START : Asserting if string is NULL
        //
          if (ManufactureStr == NULL) {
              ManufactureStr = AllocatePool (sizeof (CHAR16) * SMBIOS_STRING_MAX_LENGTH);
              ASSERT (ManufactureStr != NULL);
              StrCpy (ManufactureStr, L"Details could not be retrieved");
          }
        //
        // APTIOV_SERVER_OVERRIDE_RC_END : Asserting if string is NULL
        //
        ManufactureStrLen = ((ManufactureStr == NULL) ? 0 : (StrLen (ManufactureStr) + 1));
        ASSERT (ManufactureStrLen <= SMBIOS_STRING_MAX_LENGTH);

        SerialNumberStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, SerialNumber, NULL);
        //
        // APTIOV_SERVER_OVERRIDE_RC_START : Asserting if string is NULL
        //
          if (SerialNumberStr == NULL) {
              SerialNumberStr = AllocatePool (sizeof (CHAR16) * SMBIOS_STRING_MAX_LENGTH);
              ASSERT (SerialNumberStr != NULL);
              StrCpy (SerialNumberStr, L"Details could not be retrieved");
           }
        //
        // APTIOV_SERVER_OVERRIDE_RC_END : Asserting if string is NULL
        //
        SerialNumberStrLen = ((SerialNumberStr == NULL) ? 0 : (StrLen (SerialNumberStr) + 1));
        ASSERT (SerialNumberStrLen <= SMBIOS_STRING_MAX_LENGTH);

        AssertTagStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, AssetTag, NULL);
        //
        // APTIOV_SERVER_OVERRIDE_RC_START : Asserting if string is NULL
        //
         if (AssertTagStr == NULL) {
             AssertTagStr = AllocatePool (sizeof (CHAR16) * SMBIOS_STRING_MAX_LENGTH);
             ASSERT (AssertTagStr != NULL);
             StrCpy (AssertTagStr, L"Details could not be retrieved");
         }
         //
         // APTIOV_SERVER_OVERRIDE_RC_END : Asserting if string is NULL
         //
        AssertTagStrLen = ((AssertTagStr == NULL) ? 0 : (StrLen (AssertTagStr) + 1));
        ASSERT (AssertTagStrLen <= SMBIOS_STRING_MAX_LENGTH);

        PartNumberStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, PartNumber, NULL);
        //
        // APTIOV_SERVER_OVERRIDE_RC_START :Asserting if string is NULL
        //
            if (PartNumberStr == NULL) {
                PartNumberStr = AllocatePool (sizeof (CHAR16) * SMBIOS_STRING_MAX_LENGTH);
                ASSERT (PartNumberStr != NULL);
                StrCpy (PartNumberStr, L"Details could not be retrieved");
            }
        //
        // APTIOV_SERVER_OVERRIDE_RC_END : Asserting if string is NULL
       //
        PartNumberStrLen = ((PartNumberStr == NULL) ? 0 : (StrLen (PartNumberStr) + 1));
        ASSERT (PartNumberStrLen <= SMBIOS_STRING_MAX_LENGTH);

        //
        // Report Memory Device to Type 17 SMBIOS Record
        //

        TableSize = sizeof (SMBIOS_TABLE_TYPE17) + DeviceLocatorStrLen + BankLocatorStrLen + ManufactureStrLen + SerialNumberStrLen + AssertTagStrLen + PartNumberStrLen + 1;
        Type17Record = AllocatePool (TableSize);
        ASSERT (Type17Record != NULL);
        if (Type17Record == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        ZeroMem (Type17Record, TableSize);
        Type17Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
        Type17Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE17);
        Type17Record->Hdr.Handle = 0;
        Type17Record->MemoryArrayHandle = MemArraySmbiosHandle;
        Type17Record->MemoryErrorInformationHandle = 0xFFFE;
        Type17Record->TotalWidth = MemoryTotalWidth;
        Type17Record->DataWidth = MemoryDataWidth;
        if (MemoryDeviceSize < 0x7fff) {
          Type17Record->Size = (UINT16)MemoryDeviceSize; // in MB
          Type17Record->ExtendedSize = 0;
        } else {
          Type17Record->Size = 0x7fff; // Specifies size is in ExtendedSize field
          Type17Record->ExtendedSize = MemoryDeviceSize; // in MB
        }
        // APTIOV_SERVER_OVERRIDE_RC_START : Added changes for NO DIMM populated.
        Type17Record->FormFactor = MemoryFormFactor;
        // APTIOV_SERVER_OVERRIDE_RC_END : Added changes for NO DIMM populated.
        Type17Record->MemoryType = MemoryType;      // MemoryTypeDdr4 to be defined in Smbios.h
        Type17Record->TypeDetail.Synchronous = 1;
        Type17Record->Speed = MAX_MemorySpeed; // in MHZ
        Type17Record->ConfiguredMemoryClockSpeed = ((MemorySpeed == 2132) ? 2133 : MemorySpeed); // in MHZ
        Type17Record->Attributes = Ranks;
        Type17Record->MinimumVoltage = MinimumVoltage;
        Type17Record->MaximumVoltage = MaximumVoltage;
        Type17Record->ConfiguredVoltage = ConfiguredVoltage;
        // APTIOV_SERVER_OVERRIDE_RC_START : Added changes for NO DIMM populated.
         if (!DimmPresent) {
            Type17Record->TypeDetail.Unknown = 1;
            Type17Record->TypeDetail.Synchronous = 0;
        }
       // APTIOV_SERVER_OVERRIDE_RC_END : Added changes for NO DIMM populated.
        OptionalStrStart = (CHAR8 *) (Type17Record + 1);
        StrIndex = 1;
        
        Type17Record->DeviceLocator = ((DeviceLocatorStr == NULL) ? 0 : StrIndex);
        if (Type17Record->DeviceLocator != 0) {
          UnicodeStrToAsciiStr (DeviceLocatorStr, OptionalStrStart);
          FreePool (DeviceLocatorStr);
          StrIndex++;
        }
        
        Type17Record->BankLocator = ((BankLocatorStr == NULL) ? 0 : StrIndex);
        if (Type17Record->BankLocator != 0) {
          UnicodeStrToAsciiStr (BankLocatorStr, OptionalStrStart + DeviceLocatorStrLen);
          FreePool (BankLocatorStr);
          StrIndex++;
        }

        Type17Record->Manufacturer = ((ManufactureStr == NULL) ? 0 : StrIndex);
        if (Type17Record->Manufacturer != 0) {
          UnicodeStrToAsciiStr (ManufactureStr, OptionalStrStart + DeviceLocatorStrLen + BankLocatorStrLen);
          FreePool (ManufactureStr);
          StrIndex++;
        }

        Type17Record->SerialNumber = ((SerialNumberStr == NULL) ? 0 : StrIndex);
        if (Type17Record->SerialNumber != 0) {
          UnicodeStrToAsciiStr (SerialNumberStr, OptionalStrStart + DeviceLocatorStrLen + BankLocatorStrLen + ManufactureStrLen);
          FreePool (SerialNumberStr);
          StrIndex++;
        }

        Type17Record->AssetTag = ((AssertTagStr == NULL) ? 0 : StrIndex);
        if (Type17Record->AssetTag != 0) {
          UnicodeStrToAsciiStr (AssertTagStr, OptionalStrStart + DeviceLocatorStrLen + BankLocatorStrLen + ManufactureStrLen + SerialNumberStrLen);
          FreePool (AssertTagStr);
          StrIndex++;
        }

        Type17Record->PartNumber = ((PartNumberStr == NULL) ? 0 : StrIndex);
        if (Type17Record->PartNumber != 0) {
          UnicodeStrToAsciiStr (PartNumberStr, OptionalStrStart + DeviceLocatorStrLen + BankLocatorStrLen + ManufactureStrLen + SerialNumberStrLen + AssertTagStrLen);
          FreePool (PartNumberStr);
          StrIndex++;
        }

        //
        // Don't change it. This handle will be referenced by Type 20 SMBIOS Records
        //
        MemDevSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Status = Smbios->Add (Smbios, NULL, &MemDevSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type17Record);
        FreePool (Type17Record);
        ASSERT_EFI_ERROR (Status);
#if SMCPKG_SUPPORT && FixedType17Handle_SUPPORT == 0x01
        if (!EFI_ERROR(Status)) {
          if (DimmPresent) {
            HandleArray[HandleCount] = MemDevSmbiosHandle;
            HandleCount++;
          }
        }
#endif // #if FixedType17Handle_SUPPORT == 0x01

#if SMCPKG_SUPPORT && FixedType17Handle_SUPPORT == 0x00
        if (DimmPresent) {

          if ((MAX_SOCKET * MAX_CH * MAX_DIMM) <= Index) {
            DEBUG ((EFI_D_INFO, "Asserting the System Index Bufferflow"));

            ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
          }

          MemoryAddress = LShiftU64(SystemMemoryMap->Element[Index].BaseAddress, MEM_ADDR_SHFT_VAL);
          RowLength = LShiftU64(MemoryDeviceSize, 20);

          //
          // Report Memory Device Mapped address to Type 20 SMBIOS Record
          //
          Type20Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE20) + 1 + 1);
          ASSERT (Type20Record != NULL);
          if (Type20Record == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }
          ZeroMem(Type20Record, sizeof (SMBIOS_TABLE_TYPE20) + 1 + 1);

          Type20Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
          Type20Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE20);
          Type20Record->Hdr.Handle = 0;
          Type20Record->MemoryDeviceHandle = MemDevSmbiosHandle;
          Type20Record->MemoryArrayMappedAddressHandle = MemArrayMappedAddrSmbiosHandle;
          Type20Record->PartitionRowPosition = 0x01;
          // APTIOV_SERVER_OVERRIDE_RC_START: To indicate channel interleave position and ways.
          for (SadIndex = 0; SadIndex < SAD_RULES; SadIndex++) {
            if (SystemMemoryMap->Socket[Socket].SAD[SadIndex].type == MEM_TYPE_1LM) {
              ChannelInterBitmap = SystemMemoryMap->Socket[Socket].SAD[SadIndex].channelInterBitmap[GetMcIndexFromBitmap(SystemMemoryMap->Socket[Socket].SAD[SadIndex].imcInterBitmap)];
            }
          }
          ChInterWays = 0;
          for (i = 0; i < MAX_MC_CH; i++) {
            if (((ChannelInterBitmap >> i) & 0x1) ==1) {
              ChInterWays++;
            }
          }
          Type20Record->InterleavePosition = (ChannelInterBitmap >> Ch?Ch+1:0);
          Type20Record->InterleavedDataDepth = (ChannelInterBitmap >> Ch?ChInterWays:0);
          // APTIOV_SERVER_OVERRIDE_RC_END

          // If the address is overflow, use ExtendedStartingAddress/ExtendedEndingAddress store it.
          if ((LShiftU64(DimmStartingAddressMb, 10) < 0xFFFFFFFF) && ((LShiftU64(DimmEndingAddressMb, 10) - 1) < 0xFFFFFFFF)) {
            Type20Record->StartingAddress = (UINT32)LShiftU64(DimmStartingAddressMb, 10);    // in kB
            Type20Record->EndingAddress = (UINT32)(LShiftU64(DimmEndingAddressMb, 10) - 1);  // in kB
          } else {
            Type20Record->StartingAddress = 0xFFFFFFFF;
            Type20Record->EndingAddress = 0xFFFFFFFF;
            Type20Record->ExtendedStartingAddress = LShiftU64(DimmStartingAddressMb, 20); // in bytes
            Type20Record->ExtendedEndingAddress = LShiftU64(DimmEndingAddressMb, 20) - 1; // in bytes
          }
          DimmStartingAddressMb = DimmEndingAddressMb;

          MemDevMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
          Status = Smbios->Add (Smbios, NULL, &MemDevMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type20Record);
          FreePool (Type20Record);
          Index++;
        }
#endif FixedType17Handle_SUPPORT == 0x00  
      }
    }
  //
  // APTIOV_SERVER_OVERRIDE_RC_START
  //  Update Partition Width in Type 19 structure
#if SMCPKG_SUPPORT && FixedType17Handle_SUPPORT == 0x00     
   if ( MemInfoTotalNodeMem ) {
       Type = 19;
       Status = Smbios->GetNext (Smbios, &SearchType19Handle, &Type, &Record, NULL);  
       Type19Record = (SMBIOS_TABLE_TYPE19 *)Record;
       Type19Record->PartitionWidth = PartitionWidth;
       PartitionWidth = 0;
   }
#endif // #if FixedType17Handle_SUPPORT == 0x00  
 // Fixed Type 17 doesn't match memory arrary in Type 16.
  } //Node loop
  // APTIOV_SERVER_OVERRIDE_RC_END

  }
  /* Type 18 is related to run time errors may need to be updated during runtime if needed.
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) { // Loop for each CPU socket
    for (Ch = 0; Ch < MAX_CH; Ch++) { // Detect DIMMs on each channel
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        //
        // Report Memory Errors on Type 18 SMBIOS record
        //
        Type18Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE18) + 1 + 1);
        ZeroMem(Type18Record, sizeof (SMBIOS_TABLE_TYPE18) + 1 + 1);
        Type18Record->Hdr.Type = EFI_SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION;
        Type18Record->ErrorType            = 3;
        Type18Record->ErrorGranularity     = 2;
        Type18Record->ErrorOperation       = 2;
        Type18Record->MemArrayErrorAddress = 0x80000000;
        Type18Record->DeviceErrorAddress   = 0x80000000;
        Type18Record->ErrorResolution      = 0x80000000;
        Status = Smbios->Add (Smbios, NULL, &MemArraySmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type18Record);
        FreePool (Type18Record);
      }
    }
  }*/
#if SMCPKG_SUPPORT == 0x01 && FixedType17Handle_SUPPORT == 0x01
  HandleCount = 0;
  PhysicalMemoryHandleCount = 0;
  PartitionWidth = 0;
  for (Socket = 0; Socket < PlatformMaxSocket; Socket++) { // Loop for each CPU socket
    for(Node = 0; Node < MAX_IMC; Node++){

    // update SMBIOS type 19
    //
    // Calculate the memory size in this node
    //
    MemInfoTotalNodeMem = (UINT32)(SystemMemoryMap->Socket[Socket].imc[Node].MemSize << 6);
    Type19EndingAddress += (UINT64) LShiftU64 (MemInfoTotalNodeMem, 10);
    if (MemInfoTotalNodeMem) {
        Type19Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE19) + 1 + 1);
        ZeroMem (Type19Record, sizeof (SMBIOS_TABLE_TYPE19) + 1 + 1);
        Type19Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS;
        Type19Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE19);
        Type19Record->Hdr.Handle = 0;
        Type19Record->StartingAddress = (UINT32) Type19StartingAddress;
        Type19Record->EndingAddress = (UINT32)(Type19EndingAddress - 1);
        Type19StartingAddress = Type19EndingAddress;
        Type19Record->MemoryArrayHandle = PhysicalMemoryHandle[PhysicalMemoryHandleCount];
        MemArrayMappedAddrSmbiosHandle= SMBIOS_HANDLE_PI_RESERVED;
        Status = Smbios->Add (Smbios, NULL, &MemArrayMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type19Record);
        if (!EFI_ERROR(Status)) PhysicalMemoryHandleCount++;
        FreePool (Type19Record);
        ASSERT_EFI_ERROR (Status);
    }
    DimmStartingAddressMb = DimmEndingAddressMb = 0;
    for (Ch = 0; Ch < MAX_MC_CH; Ch++) { // Detect DIMMs on each channel
      for (Dimm = 0; Dimm < PlatformMaxDimm; Dimm++) {
        DimmPresent = TRUE;
        Chan = MAX_MC_CH * Node + Ch;
        if (SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].Enabled == 0 ||
            SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].Present== 0) {
          DimmPresent = FALSE;
        }
        if (DimmPresent) {
   	      if ((MAX_SOCKET * MAX_CH * MAX_DIMM) <= Index) {
       	        DEBUG ((EFI_D_INFO, "Asserting the System Index Bufferflow"));
       	        ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
       	      }
        MemoryAddress = LShiftU64(SystemMemoryMap->Element[Index].BaseAddress, MEM_ADDR_SHFT_VAL);
        RowLength = LShiftU64(MemoryDeviceSize, 20);
	    
        //	
        //  AptioV server override: Partition width is no of Dimms that forms the address so increment for every Dimm present.
        //  This will be added in Type 19 structure 
        //
        PartitionWidth++;
        MemoryDeviceSize = SystemMemoryMap->Socket[Socket].ChannelInfo[Chan].DimmInfo[Dimm].DimmSize; //in 64MB
        MemoryDeviceSize = MemoryDeviceSize << 6; // in MB
        DimmEndingAddressMb = DimmStartingAddressMb + MemoryDeviceSize;        
        //
        // Report Memory Device Mapped address to Type 20 SMBIOS Record
        //
        Type20Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE20) + 1 + 1);
        ZeroMem(Type20Record, sizeof (SMBIOS_TABLE_TYPE20) + 1 + 1);
        Type20Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
        Type20Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE20);
        Type20Record->Hdr.Handle = 0;
        Type20Record->StartingAddress = (UINT32) (DimmStartingAddressMb << 10);    // in kB
        Type20Record->EndingAddress = (UINT32) ((DimmEndingAddressMb << 10) - 1);  // in kB
        Type20Record->MemoryDeviceHandle = HandleArray[HandleCount];
        Type20Record->MemoryArrayMappedAddressHandle = MemArrayMappedAddrSmbiosHandle;
        Type20Record->PartitionRowPosition = 0x01;
        Type20Record->InterleavePosition = 0x0;
        Type20Record->InterleavedDataDepth = 0x0;
        DimmStartingAddressMb = DimmEndingAddressMb;

        MemDevMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Status = Smbios->Add (Smbios, NULL, &MemDevMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type20Record);
        FreePool (Type20Record);
        Index++;
        HandleCount++;            
      } //if (DimmPresent)
     } //Dimm
    }  // Ch
    if ( MemInfoTotalNodeMem ) {
        Type = 19;
        Status = Smbios->GetNext (Smbios, &SearchType19Handle, &Type, &Record, NULL);  //AptioV server override
        Type19Record = (SMBIOS_TABLE_TYPE19 *)Record;
        Type19Record->PartitionWidth = PartitionWidth;
        PartitionWidth = 0;
    }
   } // Node
  } //Socket
#endif #if SMCPKG_SUPPORT == 0x01 && FixedType17Handle_SUPPORT == 0x01

  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Filling Type17 info
  //
     gBS->FreePool (StringBufferAsset);
     gBS->FreePool (TempString);
   //
   // APTIOV_SERVER_OVERRIDE_RC_END : Filling Type17 info
   //
  gBS->FreePool (StringBuffer);

  return Status;
}
