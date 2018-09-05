/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c) 1996 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


Module Name:

  Nfit.c

Abstract:

  Implementation of NvDimm Firmware Interface Tables (NFIT)

--*/

#include "CrystalRidge.h"
#include "Nfit.h"
#pragma optimize("", off)

extern struct SystemMemoryMapHob  *mSystemMemoryMap;
extern CR_INFO                    mCrInfo;
extern UINT16                     mNumSMBiosHandles;
extern SMBIOS_HANDLE_INFO         mSMBiosHandleInfo[MAX_SOCKET * MAX_CH * MAX_DIMM];
extern UINT64                     mWpqFlushHintAddress[MAX_SOCKET * MAX_IMC];

extern CONST UINT32               Imc1Way4k_3ChWay256_LineMultipliers[3][16];
extern CONST UINT32               Imc2Way4k_3ChWay256_LineMultipliers[6][16];
extern CONST UINT32               Imc4Way4k_3ChWay256_LineMultipliers[12][16];
extern CONST UINT32               Imc8Way4k_3ChWay256_LineMultipliers[24][16];
extern CONST UINT32               Imc1Way4k_3ChWay4k_LineMultipliers[3][2];
extern CONST UINT32               Imc2Way4k_3ChWay4k_LineMultipliers[6][2];
extern CONST UINT32               Imc4Way4k_3ChWay4k_LineMultipliers[12][2];
extern CONST UINT32               Imc8Way4k_3ChWay4k_LineMultipliers[24][2];


//
// Module data
UINT32                            mNfitSize;
UINT16                            mSPARangeDescTblIndex;
UINT16                            mMemDevRegionalID;
UINT16                            mNumInterleaveDescTbls;
UINT16                            mCtrlRegionsLen;

//
// This struct is used by processing code to prevent redundant SADs
// getting entered into Nfit.
struct _SAD_INFO_ {
  UINT8       NumOfSADs;
  UINT32      Limits[MAX_SADS];
} mSAD_Info;

//
// Struct is used to store the list of control regions for all SADs.
struct _CTRL_INFO_ {
  UINT8       DimmIndex;
  UINT16      Ifc;
  BOOLEAN     TableAdded;
} mCtrlRegions[MAX_SOCKET * MAX_CH * MAX_CTRL_RGNS_PER_CHANNEL];


/**

This routine BuildNfitDevHandle basically builds Nfit Device
Handle as per the ACPI spec 6.1 as described in Memory Device
to System Physical Range Mapping Table section.

  @param socket       - socket # of the memory device of
                      interest
  @param iMC          - iMC # of the Mem Device
  @param Channel      - Channel # of the Mem device

  @retval NfitDevHanle - Build device handle based on socket,
          channel and iMC.

**/
NFIT_DEVICE_HANDLE
BuildNfitDevHandle(
   UINT8                          Socket,
   UINT8                          Ch
   )
{
  NFIT_DEVICE_HANDLE              DevHandle;
  EFI_STATUS                      Status;
  UINT8                           iMC;
  UINT8                           Dimm = 0;

  ZeroMem(&DevHandle, sizeof(DevHandle));

  if (Socket >= MAX_SOCKET || Ch >= MAX_CH) {
    return DevHandle;
  }

  Status = GetChannelDimm(Socket, Ch, &Dimm);
  if (EFI_ERROR(Status)) {
    return DevHandle;
  }

  if (Ch < MAX_MC_CH) {
    iMC = 0;
  } else {
    iMC = 1;
    Ch -= MAX_MC_CH;    // Nfit Handle should start Ch number from 0 for each MC
  }

  // SKX supports XNC, which means you can have multiple of 4S systems connected
  // together using node controller. The NodeID field allows to express the XNC node number.
  // If you have only one node that is either only 2S or 4S or 8S, system simply use NodeID as 0.
  // For the Nfit Device Handle, reserved and Node ID bitmaps should be all 0

  DevHandle.DIMMNumber = Dimm;
  DevHandle.MemoryChannelNumber = Ch;
  DevHandle.MemoryControllerID = iMC;
  DevHandle.SocketID = Socket;
  DevHandle.NodeControllerID = 0;

  return (DevHandle);
}

/**

This routine computes the checksum of a table of given size.

@param *TblStart    - Start of the table for which checksum needs to be computed
@param BytesCount   - Total size of the table for which checksum needs to be computed

@return UINT8       - Checksum computed

**/
UINT8
ChsumTbl(
   UINT8  *TblStart,
   UINT32 BytesCount
   )
{
  UINTN i;
  UINT8 res;

  if (TblStart == NULL || BytesCount == 0) {
    return 0;
  }

  res = *TblStart;
  for (i = 1; i < BytesCount; i++) {
    res = res + TblStart[i];
  }
  res = 0 - res;

  return (res);
} // ChsumTbl

/**

This routine generates list of control regions for all local SADs
for given memory region types.

@param MemTypes - memory types bit map (MEM_TYPE_PMEM, MEM_TYPE_PMEM_CACHE, MEM_TYPE_BLK_WINDOW, MEM_TYPE_CTRL).

@return UINT16  - Number of Ctrl Regions added to list

**/
UINT16
GenerateCtrlRegionsListForMemTypes(
  UINT8 MemTypes
  )
{
  EFI_STATUS Status;
  UINT8      Socket;
  UINT8      Ch;
  UINT8      Sad;
  UINT8      Index = 0;
  UINT32     ChannelMap;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Sad = 0; Sad < SAD_RULES; Sad++) {

      // Only local SADs should be taken into consideration
      if (!mSystemMemoryMap->Socket[Socket].SAD[Sad].local) {
        continue;
      }

      ChannelMap = GetChInterleaveBitmap(Socket, Sad);

      for (Ch = 0; Ch < MAX_CH; Ch++) {
        if (!((ChannelMap >> Ch) & 0x1)) {
          continue;
        }

        Status = GetDimmIndex(Socket, Ch, &mCtrlRegions[Index].DimmIndex);
        if (EFI_ERROR(Status)) {
          continue;
        }

        if (MemTypes & (MEM_TYPE_PMEM | MEM_TYPE_PMEM_CACHE) &&
            mSystemMemoryMap->Socket[Socket].SAD[Sad].type & (MEM_TYPE_PMEM | MEM_TYPE_PMEM_CACHE)) {
          mCtrlRegions[Index].Ifc = IFC_APPDIRECT_MODE;
        } else if (MemTypes & MEM_TYPE_BLK_WINDOW &&
                   mSystemMemoryMap->Socket[Socket].SAD[Sad].type & MEM_TYPE_BLK_WINDOW) {
          mCtrlRegions[Index].Ifc = IFC_STORAGE_MODE;
        } else if (MemTypes & MEM_TYPE_CTRL &&
                   mSystemMemoryMap->Socket[Socket].SAD[Sad].type & MEM_TYPE_CTRL) {
          mCtrlRegions[Index].Ifc = IFC_STORAGE_MODE;
        } else {
          continue;
        }
        mCtrlRegions[Index].TableAdded = FALSE;

        DEBUG((EFI_D_ERROR, "Nfit: Added ctrl region to list: Index = %x, dimm = %d, ifc = %x\n",
          Index+1,
          mCtrlRegions[Index].DimmIndex,
          mCtrlRegions[Index].Ifc
          ));

        Index++;
        ASSERT(Index < MAX_SOCKET * MAX_CH * MAX_CTRL_RGNS_PER_CHANNEL);
      }
    }
  }

  return Index;
}

/**

This routine generates list of control regions for all local SADs.

@return VOID

**/
VOID
GenerateCtrlRegionsList(
  VOID
  )
{
  ZeroMem(mCtrlRegions, sizeof(mCtrlRegions));
  mCtrlRegionsLen = 0;

  mCtrlRegionsLen = GenerateCtrlRegionsListForMemTypes(MEM_TYPE_PMEM | MEM_TYPE_PMEM_CACHE | MEM_TYPE_BLK_WINDOW);

  if (mCtrlRegionsLen == 0) {
    mCtrlRegionsLen = GenerateCtrlRegionsListForMemTypes(MEM_TYPE_CTRL);
  }
} // GenerateCtrlRegionsList

/**

This routine sets the given Control Region structure to being added.

@param CtrlTblIndex - index of control region structure

@return VOID

**/
VOID
SetCtrlTblAdded(
  UINT16  CtrlTblIndex
  )
{
  if (CtrlTblIndex > mCtrlRegionsLen || CtrlTblIndex == 0) {
    DEBUG((EFI_D_ERROR, "Nfit: SetCtrlTblAdded - param error!\n"));
    return;
  }

  // CtrlTblIndex is 1-based
  DEBUG((EFI_D_ERROR, "Nfit: SetCtrlTblAdded - table added, index = %d\n", CtrlTblIndex));
  mCtrlRegions[CtrlTblIndex-1].TableAdded = TRUE;
}

/**

This routine returns control region index for given socket, channel and ifc type.
Can returns one, which is not yet added to the NFIT or one, which already is.

@param Socket   - socket number
@param Ch       - channel number (on given socket)
@param Ifc      - IFC type (0x201: IFC_STORAGE_MODE or 0x301: IFC_APPDIRECT_MODE).
                  When IFC_AUTO is given, returns IFC_STORAGE_MODE or,
                  when not found, IFC_APPDIRECT_MODE control region index.
@param Unused   - TRUE - checks for tables, that are not in the NFIT,
                  FALSE - checks for any tables, that are or are not in the NFIT.

@return UINT16  - control region index, otherwise 0 - when not found

**/
UINT16
GetCtrlTblIndex(
  UINT8   Socket,
  UINT8   Ch,
  UINT16  Ifc,
  BOOLEAN Unused
  )
{
  UINT16      i = 0;
  UINT8       DimmIndex;
  BOOLEAN     Found = FALSE;
  EFI_STATUS  Status;

  if (Ifc == IFC_AUTO) {
    i = GetCtrlTblIndex(Socket, Ch, IFC_STORAGE_MODE, Unused);
    if (i == 0) {
      i = GetCtrlTblIndex(Socket, Ch, IFC_APPDIRECT_MODE, Unused);
    }

    if (i != 0) {
      Found = TRUE;
    }
  } else {
    Status = GetDimmIndex(Socket, Ch, &DimmIndex);
    if (EFI_ERROR(Status)) {
      return 0;
    }

    for (i = 0; i < mCtrlRegionsLen; i++) {
      if (mCtrlRegions[i].DimmIndex == DimmIndex &&
          mCtrlRegions[i].Ifc == Ifc &&
          !(Unused && mCtrlRegions[i].TableAdded == TRUE)) {
        Found = TRUE;
        break;
      }
    }
    // Ctrl region index is 1-based
    i++;
  }

  if (!Found) {
    return 0;
  }

  return i;
} // GetCtrlTblIndex

/**

This routine returns control region index for given socket, channel and ifc type.
Addidtionally it sets the returned index as added.

@param Socket   - socket number
@param Ch       - channel number (on given socket)
@param Ifc      - IFC type (0x201: IFC_STORAGE_MODE or 0x301: IFC_APPDIRECT_MODE).
                  When IFC_AUTO is given, returns IFC_STORAGE_MODE or,
                  when not found, IFC_APPDIRECT_MODE control region index.


@return UINT16  - control region index, otherwise 0 - when no unused ctrl table indices found

**/
UINT16
GetUnusedAndAddCtrlTblIndex(
  UINT8  Socket,
  UINT8  Ch,
  UINT16 Ifc
  )
{
  UINT16 index;

  index = GetCtrlTblIndex(Socket, Ch, Ifc, TRUE);
  if (index != 0) {
    SetCtrlTblAdded(index);
  }

  return index;
} // GetUnusedCtrlTblIndex

/**

This routine returns control region index for given socket, channel and ifc type.
First checks for Ctrl tables, that are already added, then for those that are not.

@param Socket   - socket number
@param Ch       - channel number (on given socket)
@param Ifc      - IFC type (0x201: IFC_STORAGE_MODE or 0x301: IFC_APPDIRECT_MODE).
                  When IFC_AUTO is given, returns IFC_STORAGE_MODE or,
                  when not found, IFC_APPDIRECT_MODE control region index.

@return UINT16  - control region index, otherwise 0 - when not found

**/
UINT16
GetNextCtrlTblIndex(
  UINT8  Socket,
  UINT8  Ch,
  UINT16 Ifc
  )
{
  return GetCtrlTblIndex(Socket, Ch, Ifc, FALSE);
} // GetNextCtrlTblIndex

/**

This routine returns IFC code for given Control Region structure.

@param CtrlTblIndex - index of control region structure

@return UINT16      - IFC code of given Control Region structure or 0, when error

**/
UINT16
GetIfcForCtrlTblIndex(
  UINT16  CtrlTblIndex
  )
{
  if (CtrlTblIndex > mCtrlRegionsLen || CtrlTblIndex == 0) {
    DEBUG((EFI_D_ERROR, "Nfit: GetIfcForCtrlTblIndex - param error!\n"));
    return 0;
  }

  // CtrlTblIndex is 1-based
  return mCtrlRegions[CtrlTblIndex - 1].Ifc;
}

/**

Routine Description:
This routine builds Interleave Description Tables for a 1 Way Channel Interleave.

  @param pNfit     -  pointer to Nvdimm f/w interface table
  @param SktGran   -  socket granularity
  @param ChGran    -  channel granularity
  @param iMCWays   -  iMC Ways

  @return VOID

**/
VOID
Build1ChWayIntDescTbls(
   NVDIMM_FW_INTERFACE_TABLE       *pNfit,
   UINT32                          SktGran,
   UINT32                          ChGran,
   UINT8                           iMCWays
   )
{
  INTERLEAVE_DESC_TABLE           *pIntDescTbl;
  UINT32                          LinesDesc, LineSize;
  UINT8                           i, j;
  UINT32                          *LineOffset;

  if (pNfit == NULL) {
    return;
  }

  //
  // No interleaving in this case
  if (iMCWays == ONE_IMC_WAY) {
    return;
  }

  ASSERT(SktGran >= ChGran);
  LinesDesc = SktGran / ChGran;
  LineSize = ChGran;

  for (i = 0; i < iMCWays; i++) {
    pIntDescTbl = (INTERLEAVE_DESC_TABLE *)(&pNfit->NfitTables[0] + pNfit->Length);
    pIntDescTbl->Type = TYPE_INTERLEAVE_DESCRIPTOR;
    pIntDescTbl->Length = (UINT16)(sizeof(*pIntDescTbl) + LinesDesc * sizeof(*LineOffset));
    // Index must be non-zero, i.e., 1 based.
    pIntDescTbl->InterleaveStructureIndex = mNumInterleaveDescTbls + 1;
    pIntDescTbl->NumberOfLines = LinesDesc;
    pIntDescTbl->LineSize = LineSize;

    LineOffset = (UINT32 *)((UINT8 *)pIntDescTbl + sizeof(*pIntDescTbl));
    for (j = 0; j < LinesDesc; j++) {
      LineOffset[j] = j;
    }

    mNumInterleaveDescTbls++;

    //
    // Update the Nfit lenth as appropriate
    ASSERT((pNfit->Length + pIntDescTbl->Length) <= mNfitSize);
    pNfit->Length += pIntDescTbl->Length;
  }
}

/**

Routine Description:
This routine builds Interleave Description Tables for a 2 Way Channel Interleave.

  @param pNfit     -  pointer to Nvdimm f/w interface table
  @param SktGran   -  socket granularity
  @param ChGran    -  channel granularity
  @param iMCWays   -  iMC Ways

  @return VOID

**/
VOID
Build2ChWayIntDescTbls(
   NVDIMM_FW_INTERFACE_TABLE       *pNfit,
   UINT32                          SktGran,
   UINT32                          ChGran,
   UINT8                           iMCWays
   )
{
  INTERLEAVE_DESC_TABLE           *pIntDescTbl;
  UINT32                          LinesDesc, LineSize;
  UINT8                           i, j;
  UINT32                          *LineOffset;

  if (pNfit == NULL) {
    return;
  }


  ASSERT(SktGran >= ChGran);
  LineSize = ChGran;

  // Compute the number of lines required for each channel and address range associated with each MC before the pattern repeats
  if((iMCWays == 1) || (SktGran == ChGran)) {
    LinesDesc = 2;
  } else {
    LinesDesc = SktGran / (ChGran * 2);
  }

  // Create one table for each MC and CH involved in this interleave
  for (i = 0; i < (iMCWays * 2); i++) {
    pIntDescTbl = (INTERLEAVE_DESC_TABLE *)(&pNfit->NfitTables[0] + pNfit->Length);
    pIntDescTbl->Type = TYPE_INTERLEAVE_DESCRIPTOR;
    pIntDescTbl->Length = (UINT16)(sizeof(*pIntDescTbl) + LinesDesc * sizeof(*LineOffset));
    // Index must be non-zero, i.e., 1 based.
    pIntDescTbl->InterleaveStructureIndex = mNumInterleaveDescTbls + 1;
    pIntDescTbl->NumberOfLines = LinesDesc;
    pIntDescTbl->LineSize = LineSize;

    LineOffset = (UINT32 *)((UINT8 *)pIntDescTbl + sizeof(*pIntDescTbl));
    for (j = 0; j < LinesDesc; j++)  {
      if(SktGran != ChGran) {
        // When SktGran is not equal to ChGran, the computed LineOffsets do not cross iMCs
        LineOffset[j] = (j * 2);
      } else {
        // When SktGran is equal to ChGran, the computed LineOffsets cross iMCs
        LineOffset[j] = (j * 2 * iMCWays);
      }
    }
    //DisplayInterleaveDescriptionTable(pIntDescTbl[i]);

    mNumInterleaveDescTbls++;

    //
    // Update the Nfit lenth as appropriate
    // DEBUG((EFI_D_ERROR, "Nfit Length = 0x%X\n", pNfit->Length));
    ASSERT((pNfit->Length + pIntDescTbl->Length) <= mNfitSize);
    pNfit->Length += pIntDescTbl->Length;
  }
}

/**

Routine Description:
This routine builds Interleave Description Tables for a 3 Way Channel Interleave.

  @param pNfit     -  pointer to Nvdimm f/w interface table
  @param SktGran   -  socket granularity
  @param ChGran    -  channel granularity
  @param iMCWays   -  iMC Ways

  @return VOID

**/
VOID
Build3ChWayIntDescTbls(
   NVDIMM_FW_INTERFACE_TABLE       *pNfit,
   UINT32                          SktGran,
   UINT32                          ChGran,
   UINT8                           iMCWays
   )
{
  INTERLEAVE_DESC_TABLE   *pIntDescTbl;
  UINT32                  LineSize;
  UINT32                  LinesDesc;
  UINT8                   imc, ch, line;
  UINT32                  IntDescTblMinValue;
  CONST UINT32            *InterleaveMultipliers = NULL;
  UINT32                  *LineOffset;
  UINT16                  InterleaveArrayIndex;

  if (pNfit == NULL) {
    return;
  }

  ASSERT(SktGran >= ChGran);

  //
  // Set Line Size and Lines Described
  LineSize = ChGran;

  if (SktGran == ChGran) {
    LinesDesc = 2;
  } else {
    LinesDesc = SktGran / ChGran;
  }

  for (imc = 0; imc < iMCWays; imc++) {
    for (ch = 0; ch < MAX_MC_CH; ch++) {
      //
      // Initialize the table headers
      pIntDescTbl = (INTERLEAVE_DESC_TABLE *)(&pNfit->NfitTables[0] + pNfit->Length);
      pIntDescTbl->Type = TYPE_INTERLEAVE_DESCRIPTOR;
      // Index must be non-zero, i.e., 1 based.
      pIntDescTbl->InterleaveStructureIndex = mNumInterleaveDescTbls + 1;
      pIntDescTbl->Length = (UINT16)(sizeof(*pIntDescTbl) + LinesDesc * sizeof(*LineOffset));
      pIntDescTbl->NumberOfLines = LinesDesc;
      pIntDescTbl->LineSize = LineSize;

      //
      // Point to the correct multiplier arrays
      InterleaveArrayIndex = ((imc * MAX_MC_CH) + ch) % (iMCWays * CH_3WAY);
      switch (iMCWays) {
        case ONE_IMC_WAY:
          if (SktGran == ChGran) {
            InterleaveMultipliers = &Imc1Way4k_3ChWay4k_LineMultipliers[InterleaveArrayIndex][0];
          } else {
            InterleaveMultipliers = &Imc1Way4k_3ChWay256_LineMultipliers[InterleaveArrayIndex][0];
          }
          break;
        case TWO_IMC_WAY:
          if (SktGran == ChGran) {
            InterleaveMultipliers = &Imc2Way4k_3ChWay4k_LineMultipliers[InterleaveArrayIndex][0];
          } else {
            InterleaveMultipliers = &Imc2Way4k_3ChWay256_LineMultipliers[InterleaveArrayIndex][0];
          }
          break;
        case FOUR_IMC_WAY:
          if (SktGran == ChGran) {
            InterleaveMultipliers = &Imc4Way4k_3ChWay4k_LineMultipliers[InterleaveArrayIndex][0];
          } else {
            InterleaveMultipliers = &Imc4Way4k_3ChWay256_LineMultipliers[InterleaveArrayIndex][0];
          }
          break;
        case EIGHT_IMC_WAY:
          if (SktGran == ChGran) {
            InterleaveMultipliers = &Imc8Way4k_3ChWay4k_LineMultipliers[InterleaveArrayIndex][0];
          } else {
            InterleaveMultipliers = &Imc8Way4k_3ChWay256_LineMultipliers[InterleaveArrayIndex][0];
          }
          break;
        default:
          ASSERT(iMCWays < EIGHT_IMC_WAY);
          return;
          break;
      }

      LineOffset = (UINT32 *)((UINT8 *)pIntDescTbl + sizeof(*pIntDescTbl));

      IntDescTblMinValue = 0xffffffff;
      //
      // Copy the known values from the table
      for (line = 0;  line < LinesDesc; line++) {
        LineOffset[line] = InterleaveMultipliers[line];
        if (LineOffset[line] < IntDescTblMinValue) {
          IntDescTblMinValue = LineOffset[line];
        }
      }

      //
      // Convert the table values to multiplier indices
      for (line = 0;  line < LinesDesc; line++) {
        LineOffset[line] -= IntDescTblMinValue;
      }

      mNumInterleaveDescTbls++;

      //
      // Update the Nfit lenth as appropriate
      ASSERT((pNfit->Length + pIntDescTbl->Length) <= mNfitSize);
      pNfit->Length += pIntDescTbl->Length;
    }
  }
}

/**

Routine Description:GetSMBiosHandle finds from the previously
stored SMBIOS Type 17 Handles for all Aep Dimms, the SMBios Type
17 Handle for this particular NVMDIMM in this socket and
Channel.

  @param socket         - Socket Number
  @param ch             - DDR Channel ID
  @retval SMBiosHandle  - SMBios Type17 Handle for a given
                          socket and channel

**/
UINT16
GetSMBiosHandle(
   UINT8                          Socket,
   UINT8                          Channel
   )
{
  EFI_STATUS                      Status;
  UINT8                           i, Dimm;

  Status = GetChannelDimm(Socket, Channel, &Dimm);
  if (EFI_ERROR(Status)) {
    return 0;
  }

  for (i = 0; i < mNumSMBiosHandles; i++) {
    if ((mSMBiosHandleInfo[i].Socket  == Socket) &&
        (mSMBiosHandleInfo[i].Channel == Channel) &&
        (mSMBiosHandleInfo[i].Dimm    == Dimm)) {
      return (mSMBiosHandleInfo[i].SMBiosType17Handle);
    }
  }
  return 0;
}

/*++

Routine Description:

This routine basically builds Memory Device To SPA Range Map Table as it adds it to
the Nfit

Arguments:

pNfit         - Pointer to NFIT
socket        - socket
iMC           - iMC Index (0 for iMC0 & 1 for iMC1)
Ch            - Channel
pSPARDT       - Pointer to SPA Range Desc Table
IntIndex      - Interleave table Index associated with this Mem Dev to SPA Range Mapping table
RegionOffset  - Region offset
Ways          - Interleave ways
SadInx        - Sad Index for this SPA region

Returns:

VOID

--*/
VOID
BuildMemDevToSPARangeMapTbls(
   NVDIMM_FW_INTERFACE_TABLE       *pNfit,
   UINT8                           Socket,
   UINT8                           iMC,
   UINT8                           Ch,
   SPA_RANGE_DESC_TABLE            *pSPARDT,
   UINT16                          IntIndex,
   UINT64                          RegionOffset,
   UINT16                          Ways,
   UINT8                           SadInx
   )
{
  MEMDEV_TO_SPA_RANGE_MAP_TABLE   *pMDTSPARMT;
  UINT16                          CtrlTblIndex = 0;
  struct SADTable                 *pSAD;
  UINT8                           Dimm = 0;
  UINT8                           SmartBuffer[128];
  EFI_STATUS                      Status;

  DEBUG((EFI_D_ERROR, "Entering BldMemDevToSPARngMap!\n"));

  if (Socket >= MAX_SOCKET ||
      iMC >= MAX_IMC ||
      Ch >= MAX_CH ||
      SadInx >= SAD_RULES ||
      Ways == 0) {
    return;
  }

  if (pSPARDT == NULL || pNfit == NULL) {
    return;
  }

  pSAD = &mSystemMemoryMap->Socket[Socket].SAD[SadInx];
  ASSERT(pSAD != NULL);
  if (pSAD == NULL) {
    return;
  }

  if (pSAD->type & MEM_TYPE_BLK_WINDOW) {
    CtrlTblIndex = GetNextCtrlTblIndex(Socket, Ch, IFC_STORAGE_MODE);
  } else if (pSAD->type & (MEM_TYPE_PMEM | MEM_TYPE_PMEM_CACHE)) {
    CtrlTblIndex = GetNextCtrlTblIndex(Socket, Ch, IFC_APPDIRECT_MODE);
  } else if (pSAD->type & MEM_TYPE_CTRL) {
    CtrlTblIndex = GetNextCtrlTblIndex(Socket, Ch, IFC_AUTO);
  }

  //
  // At this point Ctrl region should be already created in the NFIT
  ASSERT(CtrlTblIndex != 0);

  // Get the pointer to the next Nfit table, which in this case is MDTSPARMT.
  pMDTSPARMT = (MEMDEV_TO_SPA_RANGE_MAP_TABLE*)(UINTN)(&pNfit->NfitTables[0] + pNfit->Length);
  // Set the Type for MDTSPARMT, which is 1.
  pMDTSPARMT->Type = TYPE_MEMDEV_TO_SPA_RANGE_MAP_DESCRIPTOR;
  pMDTSPARMT->Length = sizeof(MEMDEV_TO_SPA_RANGE_MAP_TABLE);
  pMDTSPARMT->NFITDeviceHandle = BuildNfitDevHandle(Socket, Ch);
  DEBUG((EFI_D_ERROR, "Nfit: NfitDevHandle = 0x%X\n", *((UINT32*)&pMDTSPARMT->NFITDeviceHandle)));
  // SMBIOS Type 17 handle
  pMDTSPARMT->MemoryDevicePhysicalID = GetSMBiosHandle(Socket, Ch);
  DEBUG((EFI_D_ERROR, "Nfit: SMBIOSHandle = 0x%X\n", pMDTSPARMT->MemoryDevicePhysicalID));
  pMDTSPARMT->MemoryDeviceRegionID = mMemDevRegionalID;
  mMemDevRegionalID++;
  pMDTSPARMT->SPARangeStructureIndex = pSPARDT->SPARangeStructureIndex;
  pMDTSPARMT->NVDIMMControlRegionStructureIndex = CtrlTblIndex;

  DEBUG((EFI_D_ERROR, "Nfit: CntrlRgnInx = 0x%X\n", pMDTSPARMT->NVDIMMControlRegionStructureIndex));
  DEBUG((EFI_D_ERROR, "Nfit: RegionOffset = 0x%llx\n", RegionOffset));

  Status = GetChannelDimm(Socket, Ch, &Dimm);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Nfit: can't find NVDIMM for socket = %d, channel = %d\n", Socket, Ch));
    ASSERT_EFI_ERROR(Status);
  }

  pMDTSPARMT->MemoryDeviceRegionSize = (pSPARDT->SystemPhysicalAddressRangeLength / Ways);
  if (pSAD->type & MEM_TYPE_CTRL) {
    // If this represents Control Region, DPA of this region in dimm is 0.
    pMDTSPARMT->MemoryDevicePhysicalAddressRegionBase = CTRL_REG_DPA_OFFSET;
  } else if (pSAD->type & MEM_TYPE_BLK_WINDOW) {
    // If this represents Block Window Region, DPA of this region in dimm is 128MB.
    pMDTSPARMT->MemoryDevicePhysicalAddressRegionBase = BLK_WINDOW_REGION_OFFSET;
  } else if ((pSAD->type & MEM_TYPE_PMEM) || (pSAD->type & MEM_TYPE_PMEM_CACHE)) {
    pMDTSPARMT->MemoryDevicePhysicalAddressRegionBase = GetPmemDpaBase(pSPARDT->SystemPhysicalAddressRangeBase, Socket, Ch, Dimm);
    DEBUG((EFI_D_ERROR, "Nfit: MemoryDevicePhysicalAddressRegionBase = 0x%llx\n", pMDTSPARMT->MemoryDevicePhysicalAddressRegionBase));
  }
  pMDTSPARMT->RegionOffset = RegionOffset;
  pMDTSPARMT->InterleaveStructureIndex = IntIndex;
  pMDTSPARMT->InterleaveWays = Ways;
  // This flag has the Bitmaps to indicate certain failure and SMART events.
  pMDTSPARMT->MemoryDeviceStateFlags = 0;

  // Update Flags using SMART and Health Info
  Status = GetSmartHealthData(Socket, Ch, Dimm, &SmartBuffer[0]);
  if (Status == EFI_SUCCESS) {
    if (SmartBuffer[1] & BIT3) {
      if (SmartBuffer[64] != 0x80) {
        // Not a clean shutdown
        pMDTSPARMT->MemoryDeviceStateFlags |= PREVIOUS_SAVE_FAIL;
      }

      if (((SmartBuffer[64] & 0x80) == 0) ||
          ((SmartBuffer[64] & 0x11) == 0x11) ||
          ((SmartBuffer[64] & 0x09) == 0x09) ||
           (SmartBuffer[64] & 0x40)) {
        // Data flush failed
        pMDTSPARMT->MemoryDeviceStateFlags |= PLATFORM_FLUSH_FAIL;
      }
    }

    if (((SmartBuffer[0] & BIT0) && (SmartBuffer[8])) ||
        ((SmartBuffer[1] & BIT1) && (SmartBuffer[11])) ||
        ((SmartBuffer[1] & BIT2) && (SmartBuffer[31]))) {
      // Device observed SMART and Health events
      pMDTSPARMT->MemoryDeviceStateFlags |= SMART_EVENTS_OBSERVED;
    }
  }

  // Update the Nfit lenth as appropriate
  ASSERT((pNfit->Length + pMDTSPARMT->Length) <= mNfitSize);
  pNfit->Length += pMDTSPARMT->Length;
}

/**

Routine Description: AddACntrlRgnToNfit creates One Control
Region Descriptor Table and adds it to Nfit.

  @param pNfit        - Pointer to Nfit Struct
  @param Socket       - Socket Number
  @param Ch           - Channel Mumber
  @param CtrlTblIndex - Index of control region to add (has to be > 0)

  @retrun VOID

**/
VOID
AddACtrlRgnToNfit(
   NVDIMM_FW_INTERFACE_TABLE     *pNfit,
   UINT8                         Socket,
   UINT8                         Ch,
   UINT16                        CtrlTblIndex
   )
{
  CTRL_REGION_TABLE             *pCntrlRgnTbl;
  AEP_DIMM                      *pDimm = NULL;
  UINT8                         Dimm;

  DEBUG((EFI_D_ERROR, "Entering AddCtrlRgnToNfit!\n"));

  if (pNfit == NULL || CtrlTblIndex == 0) {
    DEBUG((EFI_D_ERROR, "Nfit: Parameters error! AddCtrlRgnToNfit interrupted!\n"));
    ASSERT(pNfit != NULL);
    ASSERT(CtrlTblIndex != 0);
    return;
  }

  if (!EFI_ERROR(GetChannelDimm(Socket, Ch, &Dimm))) {
    pDimm = GetDimm(Socket, Ch, Dimm);
  }

  if (pDimm == NULL) {
    DEBUG((EFI_D_ERROR, "Nfit: dimm information structure not found! AddCtrlRgnToNfit interrupted!\n"));
    ASSERT (pDimm != NULL);
    return;
  }

  DEBUG((EFI_D_ERROR, "Nfit: Adding CntrlRgn for a NvmDimm on Socket = %d, Channel = %d, Dimm = %d\n", Socket, Ch, Dimm));

  // Get the pointer of Control Region Table to point to the right area of the Nfit.
  pCntrlRgnTbl = (CTRL_REGION_TABLE *)(UINTN)(&pNfit->NfitTables[0] + pNfit->Length);

  pCntrlRgnTbl->Type                              = TYPE_CNTRL_REGION_DESCRIPTOR;
  pCntrlRgnTbl->Length                            = sizeof(CTRL_REGION_TABLE);
  pCntrlRgnTbl->NVDIMMControlRegionStructureIndex = CtrlTblIndex;
  pCntrlRgnTbl->VendorID                          = pDimm->VendorID;
  pCntrlRgnTbl->DeviceID                          = pDimm->DeviceID;
  pCntrlRgnTbl->SubsystemVendorID                 = pDimm->subSysVendorID;
  pCntrlRgnTbl->SubsystemDeviceID                 = pDimm->subSysDeviceID;
  pCntrlRgnTbl->SubsystemRevisionID               = pDimm->subSysRevisionID;
  pCntrlRgnTbl->ValidFields                       = MANUFACTURING_VALID_FIELDS;
  pCntrlRgnTbl->ManufacturingLocation             = pDimm->ManufLocation;
  pCntrlRgnTbl->ManufacturingDate                 = pDimm->ManufDate;
  pCntrlRgnTbl->SerialNumber                      = pDimm->SerialNum;
  pCntrlRgnTbl->RegionFormatInterfaceCode         = GetIfcForCtrlTblIndex(CtrlTblIndex);

  DEBUG((EFI_D_ERROR, "Nfit: CntrlRgnIndex = 0x%X\n", pCntrlRgnTbl->NVDIMMControlRegionStructureIndex));
  DEBUG((EFI_D_ERROR, "Nfit: CntrlRgn VendorID = 0x%X\n", pDimm->VendorID));
  DEBUG((EFI_D_ERROR, "Nfit: CntrlRgn DeviceID = 0x%X\n", pDimm->DeviceID));
  DEBUG((EFI_D_ERROR, "Nfit: CntrlRgn ManufLocation = 0x%X\n", pDimm->ManufLocation));
  DEBUG((EFI_D_ERROR, "Nfit: CntrlRgn ManufDate = 0x%X\n", pDimm->ManufDate));
  DEBUG((EFI_D_ERROR, "Nfit: CntrlRgn RegionFormatInterfaceCode = 0x%X\n", pCntrlRgnTbl->RegionFormatInterfaceCode));

  if (pCntrlRgnTbl->RegionFormatInterfaceCode == IFC_STORAGE_MODE) {
    pCntrlRgnTbl->NumberOfBlockControlWindows                = NUM_BLOCK_CTRL_WINDOWS;
    pCntrlRgnTbl->SizeOfBlockControlWindow                   = BLOCK_CTRL_WIN_SIZE;
    // Blk Control offset is known
    pCntrlRgnTbl->CommandRegisterOffsetInBlockControlWindow  = BLK_CONTROL_OFFSET;
    pCntrlRgnTbl->SizeOfCommandRegisterInBlockControlWindows = CR_REG_SIZE;
    pCntrlRgnTbl->StatusRegisterOffsetInBlockControlWindow   = BLK_CONTROL_STATUS_REG_OFFSET;
    pCntrlRgnTbl->SizeOfStatusRegisterInBlockControlWindows  = STATUS_REG_SIZE;
  } else {
    pCntrlRgnTbl->NumberOfBlockControlWindows                = 0;

    pCntrlRgnTbl->Length -= CONTROL_REGION_SIZE_DIFF_NO_BLOCK_WINDOWS;
  }

  ASSERT((pNfit->Length + pCntrlRgnTbl->Length) <= mNfitSize);
  pNfit->Length += pCntrlRgnTbl->Length;
}

/**

Routine Description: AppendCtrlRgnsForChannels will build Control
Region structure for all channels in current interleave for socket.

  @param NfitPtr     - Pointer to Nfit Struct
  @param Socket      - Socket Number
  @param ChannelMap  - bitmap of all channels in current interleave
  @param Ifc         - IFC code

  @retval VOID       - None

**/
VOID
AppendCtrlRgnsForChannels (
   NVDIMM_FW_INTERFACE_TABLE     *NfitPtr,
   UINT8                         Socket,
   UINT32                        ChannelMap,
   UINT16                        Ifc
   )
{
  EFI_STATUS  Status;
  UINT8       Ch;
  UINT8       Dimm;
  UINT16      CtrlTblIndex;

  if (Socket >= MAX_SOCKET || NfitPtr == NULL) {
    return;
  }

  for (Ch = 0; Ch < MAX_CH; Ch++) {
    if ((ChannelMap >> Ch) & 0x1) {
      //
      // We are creating Control tables only for not yet used indexes
      CtrlTblIndex = GetUnusedAndAddCtrlTblIndex(Socket, Ch, Ifc);

      Status = GetChannelDimm(Socket, Ch, &Dimm);

      if (EFI_ERROR(Status)) {
        //
        // This condition shouldn't happen, since there have to be Dimm present on given channel
        ASSERT_EFI_ERROR(Status);
        return;
      }

      if (CtrlTblIndex && IsNvmDimmEnabled(Socket, Ch, Dimm)) {
        AddACtrlRgnToNfit (NfitPtr, Socket, Ch, CtrlTblIndex);
      }
    }
  }
}

/**

Routine Description: AddABlkDataWndToNfit creates One Block Data
Window Descriptor Table and adds it to Nfit.

  @param pNfit       - Pointer to Nfit Struct
  @param Socket      - Socket Number
  @param Ch          - Channel number of the Dimm
  @param DimmSPAStart- Start SPA for this Dimm
  @param RgnSize     - Control Region Size in bytes.
  @param Ch          - Memory Channel of the Dimm that contains
                     this Blk Wnd
  @retval VOID       - None

**/
VOID
AddABlkDataWndToNfit(
   NVDIMM_FW_INTERFACE_TABLE     *pNfit,
   UINT8                         Socket,
   UINT8                         Ch,
   UINT64                        DimmSPAStart,
   UINT64                        RgnSize
   )
{
  BLK_WINDOW_DESC_TABLE          *pBlkWinTbl;
  UINT8                          Dimm = 0;
  UINT64                         NvSize = 0;

  DEBUG((EFI_D_ERROR, "Entering AddABlkDataWndToNfit!\n"));

  if (pNfit == NULL || Socket >= MAX_SOCKET || Ch >= MAX_CH) {
    return;
  }

  // Get the pointer of Control Region Table to point to the right area of the Nfit.
  pBlkWinTbl = (BLK_WINDOW_DESC_TABLE *)(UINTN)(&pNfit->NfitTables[0] + pNfit->Length);
  pBlkWinTbl->Type = TYPE_BLK_WINDOW_DESCRIPTOR;
  pBlkWinTbl->Length = sizeof(BLK_WINDOW_DESC_TABLE);

  pBlkWinTbl->NVDIMMControlRegionStructureIndex = GetNextCtrlTblIndex(Socket, Ch, IFC_STORAGE_MODE);

  //
  // At this point Ctrl region for Block region should be already created in the NFIT
  ASSERT(pBlkWinTbl->NVDIMMControlRegionStructureIndex != 0);

  DEBUG((EFI_D_ERROR, "Nfit: Adding BlkWndRgn for a NvmDimm on Socket = %d, Channel = %d\n", Socket, Ch));
  DEBUG((EFI_D_ERROR, "Nfit: CntrlRgnIndex = 0x%X\n", pBlkWinTbl->NVDIMMControlRegionStructureIndex));
  pBlkWinTbl->NumberOfBlockDataWindows = NUM_BLOCK_CTRL_WINDOWS;
  // Blk Wnd Start is a Logical Offset, so should be 0.
  pBlkWinTbl->BlockDataWindowStartOffset = 0;
  pBlkWinTbl->SizeOfBlockDataWindow = BLOCK_CTRL_WIN_SIZE;

  for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
    if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].nonVolCap != 0) {
      NvSize = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].nonVolCap;
      break;
    }
  }
  NvSize <<= BITS_64MB_TO_BYTES;
  pBlkWinTbl->BlockAccessibleMemoryCapacity = NvSize;
  DEBUG((EFI_D_ERROR, "Nfit: AccBlkCapacity = 0x%llX\n", pBlkWinTbl->BlockAccessibleMemoryCapacity));
  if (Dimm < MAX_DIMM) {
    pBlkWinTbl->BeginningAddressOfFirstBlockInBlockAccessibleMemory = GetPmemDpaBase(DimmSPAStart, Socket, Ch, Dimm);
    DEBUG((EFI_D_ERROR, "Nfit: BlkWnd StartAddr = 0x%llX\n", pBlkWinTbl->BeginningAddressOfFirstBlockInBlockAccessibleMemory));
  }
  // DEBUG((EFI_D_ERROR, "Nfit Length = 0x%X\n", pNfit->Length));
  ASSERT((pNfit->Length + pBlkWinTbl->Length) <= mNfitSize);
  pNfit->Length += pBlkWinTbl->Length;
}

/**

Routine Description: BuildBlkWndRgnFor1WayChInt will build Block
Window Region Descriptor for 1 way channel interleave.

  @param pNfit       - Pointer to Nfit Struct
  @param socket      - Socket Number
  @param SadInx      - SAD Index
  @param iMCWays     - iMCWays
  @retval VOID       - None

**/
VOID
BuildBlkWndRgnFor1WayChInt(
   NVDIMM_FW_INTERFACE_TABLE     *pNfit,
   UINT8                         socket,
   UINT8                         SadInx,
   UINT8                         iMCWays
   )
{
  UINT64                         SADSpaBase, Size;
  UINT32                         ImcIncrement;
  UINT8                          Ch = 0;
  struct SADTable                *pSAD;
  UINT8                          McIndex;

  if (pNfit == NULL || socket >= MAX_SOCKET || SadInx >= SAD_RULES || iMCWays == 0) {
    return;
  }

  // If SAD Index is zero, start address is 0
  if (SadInx) {
    // SAD Limit is in 64MB chunks and it starts from the previous SAD's limit, convert to bytes
    SADSpaBase = (UINT64)((UINTN)mSystemMemoryMap->Socket[socket].SAD[SadInx - 1].Limit << BITS_64MB_TO_BYTES);
    Size = (UINT64)((UINTN)(mSystemMemoryMap->Socket[socket].SAD[SadInx].Limit - mSystemMemoryMap->Socket[socket].SAD[SadInx - 1].Limit) << BITS_64MB_TO_BYTES);
  } else {
    SADSpaBase = 0;
    Size = (UINT64)((UINTN)mSystemMemoryMap->Socket[socket].SAD[SadInx].Limit << BITS_64MB_TO_BYTES); // SAD in 64MB chunks, Convert it to bytes.
  }
  ImcIncrement = 0;

  pSAD = &mSystemMemoryMap->Socket[socket].SAD[SadInx];
  ASSERT(pSAD != NULL);
  if (pSAD == NULL) {
    return;
  }

  // Determine which IMC channel bitmap to use
  McIndex = GetMcIndexFromBitmap (pSAD->imcInterBitmap);

  if ((pSAD->FMchannelInterBitmap[McIndex] & 0x01) == 0x01) {
    Ch = 0;
  } else if ((pSAD->FMchannelInterBitmap[McIndex] & 0x02) == 0x02) {
    Ch = 1;
  } else if ((pSAD->FMchannelInterBitmap[McIndex] & 0x04) == 0x04) {
    Ch = 2;
  }

  if (socket < MAX_SOCKET) {
    if (mSystemMemoryMap->Socket[socket].SADintList[SadInx][socket * MAX_IMC]) {
      // iMC0 of this socket is involved in this interleave set,
      SADSpaBase += ImcIncrement;
      AddABlkDataWndToNfit (pNfit, socket, Ch, SADSpaBase, (Size / (iMCWays * CH_1WAY)));
      ImcIncrement = mCrInfo.SktGran;
    }

    if (mSystemMemoryMap->Socket[socket].SADintList[SadInx][(socket * MAX_IMC) + 1]) {
      Ch += MAX_MC_CH;
      // If iMC1 of this socket is involved in this interleave set,
      SADSpaBase += ImcIncrement;
      AddABlkDataWndToNfit (pNfit, socket, Ch, SADSpaBase, (Size / (iMCWays * CH_1WAY)));
      ImcIncrement = mCrInfo.SktGran;
    }
  }
}

/**

Routine Description: BuildBlkWndRgnFor2WayChInt will build Block
Window Region Descriptor for 2 way channel interleave.

  @param pNfit       - Pointer to Nfit Struct
  @param socket      - Socket Number
  @param SadInx      - SAD Index
  @param iMCWays     - iMCWays
  @retval VOID       - None

**/
VOID
BuildBlkWndRgnFor2WayChInt(
   NVDIMM_FW_INTERFACE_TABLE     *pNfit,
   UINT8                         socket,
   UINT8                         SadInx,
   UINT8                         iMCWays
   )
{
  UINT64                         SADSpaBase, Size;
  UINT32                         ImcIncrement;
  UINT8                          Ch1 = 0;
  UINT8                          Ch2 = 1;
  struct SADTable                *pSAD;
  UINT8                          McIndex;

  if (pNfit == NULL || socket >= MAX_SOCKET || SadInx >= SAD_RULES || iMCWays == 0) {
    return;
  }

  // If SAD Index is zero, start address is 0
  if (SadInx) {
    // SAD Limit is in 64MB chunks and it starts from the previous SAD's limit, convert to bytes
    SADSpaBase = (UINT64)((UINTN)mSystemMemoryMap->Socket[socket].SAD[SadInx - 1].Limit << BITS_64MB_TO_BYTES);
    Size = (UINT64)((UINTN)(mSystemMemoryMap->Socket[socket].SAD[SadInx].Limit - mSystemMemoryMap->Socket[socket].SAD[SadInx - 1].Limit) << BITS_64MB_TO_BYTES);
  } else {
    SADSpaBase = 0;
    Size = (UINT64)((UINTN)mSystemMemoryMap->Socket[socket].SAD[SadInx].Limit << BITS_64MB_TO_BYTES); // SAD in 64MB chunks, Convert it to bytes.
  }

  ImcIncrement = 0;
  pSAD = &mSystemMemoryMap->Socket[socket].SAD[SadInx];
  ASSERT(pSAD != NULL);
  if (pSAD == NULL) {
    return;
  }

  // Determine which IMC channel bitmap to use
  McIndex = GetMcIndexFromBitmap (pSAD->imcInterBitmap);

  if ((pSAD->FMchannelInterBitmap[McIndex] & 0x03) == 0x03) {
    Ch1 = 0;
    Ch2 = 1;
  } else if ((pSAD->FMchannelInterBitmap[McIndex] & 0x05) == 0x05) {
    Ch1 = 0;
    Ch2 = 2;
  } else if ((pSAD->FMchannelInterBitmap[McIndex] & 0x06) == 0x06) {
    Ch1 = 1;
    Ch2 = 2;
  }
  if (socket < MAX_SOCKET) {
    if (mSystemMemoryMap->Socket[socket].SADintList[SadInx][socket * MAX_IMC]) {
      // If iMC0 of this socket is involved in this interleave set,
      SADSpaBase += ImcIncrement;
      AddABlkDataWndToNfit (pNfit, socket, Ch1, SADSpaBase, (Size / (iMCWays * CH_2WAY)));
      SADSpaBase += mCrInfo.ChGran;
      AddABlkDataWndToNfit (pNfit, socket, Ch2, SADSpaBase, (Size / (iMCWays * CH_2WAY)));
      ImcIncrement = mCrInfo.SktGran;
    }

    if (mSystemMemoryMap->Socket[socket].SADintList[SadInx][(socket * MAX_IMC) + 1]) {
      // If iMC1 of this socket is involved in this interleave set,
      Ch1 += MAX_MC_CH;
      Ch2 += MAX_MC_CH;
      SADSpaBase += ImcIncrement;
      AddABlkDataWndToNfit (pNfit, socket, Ch1, SADSpaBase, (Size / (iMCWays * CH_2WAY)));
      SADSpaBase += mCrInfo.ChGran;
      AddABlkDataWndToNfit (pNfit, socket, Ch2, SADSpaBase, (Size / (iMCWays * CH_2WAY)));
      ImcIncrement = mCrInfo.SktGran;
    }
  }
}

/**

Routine Description: BuildBlkWndRgnFor3WayChInt will build Block
Window Region Descriptor for 3 way channel interleave.

  @param pNfit       - Pointer to Nfit Struct
  @param socket      - Socket Number
  @param SadInx      - SAD Index
  @param iMCWays     - iMCWays
  @retval VOID       - None

**/
VOID
BuildBlkWndRgnFor3WayChInt(
   NVDIMM_FW_INTERFACE_TABLE     *pNfit,
   UINT8                         socket,
   UINT8                         SadInx,
   UINT8                         iMCWays
   )
{
  UINT64                         Size;
  UINT8                          Ch, k, j;
  UINT64                         SADSpaBase, DimmSpaBase;
  struct SADTable                *pSAD;
  UINT32                         ImcIncrements[48] = { // MAX_SOCKET * MAX_MC_CH * MAX_IMC (for 8 sockets) -- SKX_TODO Update numbers for 8 sockets
                                   0, CTRL_RGN_GRANULARITY, (2 * CTRL_RGN_GRANULARITY), (18 * CTRL_RGN_GRANULARITY), (16 * CTRL_RGN_GRANULARITY), (17 * CTRL_RGN_GRANULARITY),
                                   (33 * CTRL_RGN_GRANULARITY), (34 * CTRL_RGN_GRANULARITY), (32 * CTRL_RGN_GRANULARITY), (48 * CTRL_RGN_GRANULARITY), (49 * CTRL_RGN_GRANULARITY), (50 * CTRL_RGN_GRANULARITY),
                                   (66 * CTRL_RGN_GRANULARITY), (64 * CTRL_RGN_GRANULARITY), (65 * CTRL_RGN_GRANULARITY), (81 * CTRL_RGN_GRANULARITY), (82 * CTRL_RGN_GRANULARITY), (80 * CTRL_RGN_GRANULARITY),
                                   (96 * CTRL_RGN_GRANULARITY), (97 * CTRL_RGN_GRANULARITY), (98 * CTRL_RGN_GRANULARITY), (114 * CTRL_RGN_GRANULARITY), (112 * CTRL_RGN_GRANULARITY), (113 * CTRL_RGN_GRANULARITY),
                                   0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0
                                 };

  if (pNfit == NULL || socket >= MAX_SOCKET || SadInx >= SAD_RULES || iMCWays == 0) {
    return;
  }

  // If SAD Index is zero, start address is 0
  if (SadInx) {
    // SAD Limit is in 64MB chunks and it starts from the previous SAD's limit, convert to bytes
    SADSpaBase = (UINT64)((UINTN)mSystemMemoryMap->Socket[socket].SAD[SadInx - 1].Limit << BITS_64MB_TO_BYTES);
    Size = (UINT64)((UINTN)(mSystemMemoryMap->Socket[socket].SAD[SadInx].Limit - mSystemMemoryMap->Socket[socket].SAD[SadInx - 1].Limit) << BITS_64MB_TO_BYTES);
  } else {
    SADSpaBase = 0;
    Size = (UINT64)((UINTN)mSystemMemoryMap->Socket[socket].SAD[SadInx].Limit << BITS_64MB_TO_BYTES); // SAD in 64MB chunks, Convert it to bytes.
  }
  j = 0;

  pSAD = &mSystemMemoryMap->Socket[socket].SAD[SadInx];
  ASSERT(pSAD != NULL);
  if (pSAD == NULL) {
    return;
  }

  if (socket < MAX_SOCKET) {
    if (mSystemMemoryMap->Socket[socket].SADintList[SadInx][socket * MAX_IMC]) {
      // iMC0 of this socket is involved in this interleave set,
      for (k = 0; k < CH_3WAY; k++) {
        DimmSpaBase = SADSpaBase + ImcIncrements[j];
        AddABlkDataWndToNfit (pNfit, socket, k, DimmSpaBase, (Size / (iMCWays * CH_3WAY)));
        j++;
      }
    }

    if (mSystemMemoryMap->Socket[socket].SADintList[SadInx][(socket * MAX_IMC) + 1]) {
      // If iMC1 of this socket is involved in this interleave set,
      for (k = 0; k < CH_3WAY; k++) {
        Ch = k + MAX_MC_CH;
        DimmSpaBase = SADSpaBase + ImcIncrements[j];
        AddABlkDataWndToNfit (pNfit, socket, Ch, DimmSpaBase, (Size / (iMCWays * CH_3WAY)));
        j++;
      }
    }
  }
}

/**
Routine Description:

This routine basically builds NvDIMM F/W Interface Table for 1 Way Channel Interleave, appending the sub-tables to Nfit
from the SAD struct passed in.

Arguments:

   @param pNfit           -  pointer to Nvdimm f/w interface table
   @param socket          - socket number
   @param Ch              - Interleave Channel number
   @param iMCWays         - Number of iMC ways this interleave should
   @param happen. SadInx  - SAD Index that needs to be added to
   @param Nfit pSPARDT    - Pointer to Range Desc Table
   @param SADSpaBase      - SPA Base Address


Returns:

   @retval VOID
**/
VOID
BuildMemDevFor1WayChInt(
   NVDIMM_FW_INTERFACE_TABLE     *pNfit,
   UINT8                         iMCWays,
   SPA_RANGE_DESC_TABLE          *pSPARDT,
   UINT64                        SADSpaBase
   )
{
  struct SADTable                 *pSAD;
  UINT16                          NumIntTbls;
  UINT8                           Soc, Ch = 0, i;
  UINT64                          SpaBase, RegionOffset;
  UINT32                          SktGran;

  if (pNfit == NULL || pSPARDT == NULL || iMCWays == 0) {
    return;
  }

  NumIntTbls = mNumInterleaveDescTbls + 1;
  // Region Offset starts as 0 for the 1st Dimm, then increments by Target Granularity.
  RegionOffset = 0;
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      ASSERT(pSAD != NULL);
      if (pSAD == NULL) {
        break;
      }

      SktGran = GetSocketGranularity(pSAD);
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        // iMC0 is participating in 1 Channel Way Interleave set
        // Knowing it is 1 way interleaved, get the channel
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          if ((pSAD->FMchannelInterBitmap[IMC0] & 0x01) == 0x01) {
            Ch = 0;
          } else if ((pSAD->FMchannelInterBitmap[IMC0] & 0x02) == 0x02) {
            Ch = 1;
          } else if ((pSAD->FMchannelInterBitmap[IMC0] & 0x04) == 0x04) {
            Ch = 2;
          }
          if (iMCWays == 1) {
            BuildMemDevToSPARangeMapTbls(pNfit, Soc, 0, Ch, pSPARDT, 0, RegionOffset, iMCWays, i);
          } else {
            BuildMemDevToSPARangeMapTbls(pNfit, Soc, 0, Ch, pSPARDT, NumIntTbls, RegionOffset, iMCWays, i);
            NumIntTbls++;
            RegionOffset += SktGran;
          }
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          if ((pSAD->FMchannelInterBitmap[IMC1] & 0x01) == 0x01) {
            Ch = 0;
          } else if ((pSAD->FMchannelInterBitmap[IMC1] & 0x02) == 0x02) {
            Ch = 1;
          } else if ((pSAD->FMchannelInterBitmap[IMC1] & 0x04) == 0x04) {
            Ch = 2;
          }
          Ch += MAX_MC_CH;
          if (iMCWays == 1) {
            BuildMemDevToSPARangeMapTbls(pNfit, Soc, 1, Ch, pSPARDT, 0, RegionOffset, iMCWays, i);
          } else {
            BuildMemDevToSPARangeMapTbls(pNfit, Soc, 1, Ch, pSPARDT, NumIntTbls, RegionOffset, iMCWays, i);
            NumIntTbls++;
            RegionOffset += SktGran;
          }
        }
      }
    }
  }
}

/**

Routine Description:

This routine basically builds NvDIMM F/W Interface Tables for 2 way Channel Interleaved, appending the sub-tables to Nfit
from the SAD struct passed in.

Arguments:

  @param pNfit   -  pointer to Nvdimm f/w interface table
  @param socket  - socket number
  @param iMCWays - Number of iMC ways this interleave should
  @param happen. SadInx  - SAD Index that needs to be added to
  @param Nfit pSPARDT - Pointer to Range Desc Table
  @param SADSpaBase - SPA Base Address


Returns:

  @retval VOID

**/
VOID
BuildMemDevFor2WayChInt(
  NVDIMM_FW_INTERFACE_TABLE     *pNfit,
  UINT8                         iMCWays,
  SPA_RANGE_DESC_TABLE          *pSPARDT,
  UINT64                        SADSpaBase
  )
{
  struct SADTable                 *pSAD;
  UINT64                          SpaBase, RegionOffset;
  UINT8                           Soc, i;
  UINT8                           Ch1 = 0;
  UINT8                           Ch2 = 1;
  UINT16                          NumIntTbls;
  UINT32                          SktGran, ChGran;

  if (pNfit == NULL || pSPARDT == NULL || iMCWays == 0) {
    return;
  }

  // Region Offset starts with 0 for the 1st Dimm and increments by Channel Granularity for dimms in the same
  // iMC but increments by Target Granularity for the Dimms in other iMCs.
  RegionOffset = 0;
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      ASSERT(pSAD != NULL);
      if (pSAD == NULL) {
        break;
      }

      SktGran = GetSocketGranularity(pSAD);
      ChGran = GetChannelGranularity(pSAD);
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        NumIntTbls = mNumInterleaveDescTbls + 1;
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          if ((pSAD->FMchannelInterBitmap[IMC0] & 0x03) == 0x03) {
            Ch1 = 0;
            Ch2 = 1;
          } else if ((pSAD->FMchannelInterBitmap[IMC0] & 0x05) == 0x05) {
            Ch1 = 0;
            Ch2 = 2;
          } else if ((pSAD->FMchannelInterBitmap[IMC0] & 0x06) == 0x06) {
            Ch1 = 1;
            Ch2 = 2;
          }
          // iMC0 is participating in the interleave and it is 2 Channel way interleave, so..
          BuildMemDevToSPARangeMapTbls(pNfit, Soc, 0, Ch1, pSPARDT, NumIntTbls, RegionOffset, (2 * iMCWays), i);
          NumIntTbls++;
          // We will need a actual VID & DID for Ch2, but for now it is hardcoded, so use the same VID & DID
          BuildMemDevToSPARangeMapTbls(pNfit, Soc, 0, Ch2, pSPARDT, NumIntTbls, (RegionOffset + ChGran), (2 * iMCWays), i);
          NumIntTbls++;
          if( SktGran != ChGran ) {
            // RegionOffset for next pair of channels is at the previous region offset plus memory controller interleave granularity
            RegionOffset += SktGran ;
          } else {
            // RegionOffset for next pair of channels is at the previous region offset plus the channel granularity for the two channels already processed
            RegionOffset = RegionOffset + ( ChGran * 2 );
          }
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          if ((pSAD->FMchannelInterBitmap[IMC1] & 0x03) == 0x03) {
            Ch1 = 0;
            Ch2 = 1;
          } else if ((pSAD->FMchannelInterBitmap[IMC1] & 0x05) == 0x05) {
            Ch1 = 0;
            Ch2 = 2;
          } else if ((pSAD->FMchannelInterBitmap[IMC1] & 0x06) == 0x06) {
            Ch1 = 1;
            Ch2 = 2;
          }
          Ch1 += MAX_MC_CH;
          // iMC1 is participating in the interleave and it is 2 Channel way interleave, so..
          BuildMemDevToSPARangeMapTbls(pNfit, Soc, 1, Ch1, pSPARDT, NumIntTbls, RegionOffset, (2 * iMCWays), i);
          NumIntTbls++;
          //
          Ch2 += MAX_MC_CH;
          BuildMemDevToSPARangeMapTbls(pNfit, Soc, 1, Ch2, pSPARDT, NumIntTbls, (RegionOffset + ChGran), (2 * iMCWays), i);
          NumIntTbls++;
          if (SktGran != ChGran) {
            // RegionOffset for next pair of channels is at the previous region offset plus memory controller interleave granularity
            RegionOffset += SktGran ;
          } else {
            // RegionOffset for next pair of channels is at the previous region offset plus the channel granularity for the two channels already processed
            RegionOffset = RegionOffset + (ChGran * 2);
          }
        }
      }
    }
  }
}

/**
Routine Description:

This routine basically builds NvDIMM F/W Interface Tables for 3 way Channel Interleaved, appending the sub-tables to Nfit
from the SAD struct passed in.

Arguments:

  @param NfitPtr   -  pointer to Nvdimm f/w interface table
  @param socket  - socket number
  @param ImcWays - Number of iMC ways this interleave should
  @param happen. SadInx  - SAD Index that needs to be added to
  @param Nfit SpaRdtPtr - Pointer to Range Desc Table
  @param SADSpaBase - SPA Base Address


Returns:

  @retval VOID

**/
VOID
BuildMemDevFor3WayChInt(
  NVDIMM_FW_INTERFACE_TABLE     *NfitPtr,
  UINT8                         ImcWays,
  SPA_RANGE_DESC_TABLE          *SpaRdtPtr,
  UINT64                        SADSpaBase
  )
{
  UINT16          NumIntTbls;
  UINT8           Soc;
  UINT8           i;
  UINT8           j;
  UINT8           Ch;
  struct SADTable *pSAD;
  UINT64          SpaBase;
  UINT32          ChGran;
  UINT32          RgnOffsetMultiplier[48] = { // MAX_SOCKET * MAX_MC_CH * MAX_IMC (for 8 sockets)
                                   0, 1, 2, 18, 16, 17,
                                   33, 34, 32, 48, 49, 50,
                                   66, 64, 65, 81, 82, 80,
                                   96, 97, 98, 114, 112, 113,
                                   129, 130, 128, 144, 145, 146,
                                   162, 160, 161, 177, 178, 176,
                                   192, 193, 194, 210, 208, 209,
                                   225, 226, 224, 240, 241, 242
                                  };

  if (NfitPtr == NULL || SpaRdtPtr == NULL || ImcWays == 0) {
    return;
  }

  // Region Offset starts with 0 for the 1st Dimm and increments by Channel Granularity for dimms in the same
  // iMC but increments by Target Granularity for the Dimms in other iMCs.
  j = 0;
  NumIntTbls = mNumInterleaveDescTbls + 1;
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      ASSERT(pSAD != NULL);
      if (pSAD == NULL) {
        break;
      }

      ChGran = GetChannelGranularity(pSAD);
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          // iMC0 is participating in the 3 channel way interleave
          // Determine starting channel number
          Ch = (SpaBase/ChGran) % CH_3WAY;
          BuildMemDevToSPARangeMapTbls(NfitPtr, Soc, 0, Ch, SpaRdtPtr, NumIntTbls, (RgnOffsetMultiplier[j] * ChGran), (3 * ImcWays), i);
          NumIntTbls++;
          j++;
          Ch = (Ch == CH_3WAY - 1) ? 0 : Ch + 1;
          BuildMemDevToSPARangeMapTbls(NfitPtr, Soc, 0, Ch, SpaRdtPtr, NumIntTbls, (RgnOffsetMultiplier[j] * ChGran), (3 * ImcWays), i);
          NumIntTbls++;
          j++;
          Ch = (Ch == CH_3WAY - 1) ? 0 : Ch + 1;
          BuildMemDevToSPARangeMapTbls(NfitPtr, Soc, 0, Ch, SpaRdtPtr, NumIntTbls, (RgnOffsetMultiplier[j] * ChGran), (3 * ImcWays), i);
          NumIntTbls++;
          j++;
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          // iMC1 is participating in the 3 channel way interleave for this socket
          // Determine starting channel number
          Ch = ((SpaBase/ChGran) % CH_3WAY) + MAX_MC_CH;
          BuildMemDevToSPARangeMapTbls(NfitPtr, Soc, 1, Ch, SpaRdtPtr, NumIntTbls, (RgnOffsetMultiplier[j] * ChGran), (3 * ImcWays), i);
          NumIntTbls++;
          j++;
          Ch = (Ch == MAX_MC_CH + CH_3WAY - 1) ? MAX_MC_CH : Ch + 1;
          BuildMemDevToSPARangeMapTbls(NfitPtr, Soc, 1, Ch, SpaRdtPtr, NumIntTbls, (RgnOffsetMultiplier[j] * ChGran), (3 * ImcWays), i);
          NumIntTbls++;
          j++;
          Ch = (Ch == MAX_MC_CH + CH_3WAY - 1) ? MAX_MC_CH : Ch + 1;
          BuildMemDevToSPARangeMapTbls(NfitPtr, Soc, 1, Ch, SpaRdtPtr, NumIntTbls, (RgnOffsetMultiplier[j] * ChGran), (3 * ImcWays), i);
          NumIntTbls++;
          j++;
        }
      }
    }
  }
}

/**

This routine adds Flush Hint Address Descriptor Tables to Nfit.

  @param pNfit        - Pointer to Nfit Table

  @retval VOID        - Nothing

**/
VOID
AddFlushHintAddrTblToNfit(
   NVDIMM_FW_INTERFACE_TABLE *pNfit
   )
{
  UINT8                   Socket, Mc, Dimm, Sch, Mch;
  FLUSH_HINT_ADDR_TABLE   *pFHA;
  UINT64                  *FlushHintAddress;

  if (pNfit == NULL) {
    return;
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Mc = 0; Mc < MAX_IMC; Mc++) {
      if (mWpqFlushHintAddress[Socket * MAX_IMC + Mc]) {
        for (Mch = 0; Mch < MAX_MC_CH; Mch++) {
          // convert the channel# per MC into channel# per socket
          Sch = Mc * MAX_MC_CH + Mch;

          for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
            if (!IsNvmDimm (Socket, Sch, Dimm)) {
              continue;
            }

            pFHA = (FLUSH_HINT_ADDR_TABLE *)(UINTN)(&pNfit->NfitTables[0] + pNfit->Length);
            pFHA->Type = TYPE_FLUSH_HINT_DESCRIPTOR;
            pFHA->NFITDeviceHandle = BuildNfitDevHandle(Socket, Sch);

            pFHA->NumberOfFlushHintAddresses = 1;
            pFHA->Length = sizeof(*pFHA) + pFHA->NumberOfFlushHintAddresses * sizeof(*FlushHintAddress);
            FlushHintAddress = (UINT64 *)((UINT8 *)pFHA + sizeof(*pFHA));
            *FlushHintAddress = mWpqFlushHintAddress[Socket * MAX_IMC + Mc];

            ASSERT ((pNfit->Length + pFHA->Length) <= mNfitSize);
            pNfit->Length += pFHA->Length;
            DEBUG ((EFI_D_INFO, "Flush Hint tbl for socket: %d, channel: %d, dimm: %d, dev_handle: 0x%X\n",
              Socket, Sch, Dimm, pFHA->NFITDeviceHandle));
          }
        }
      }
    }
  }
}

/**

This routine adds SMBIOS Management Information Descriptor Table
to Nfit. This Table essentiall has SMBios Type17 handles of all
NVMDimms installed in the system.

  @param pNfit        - Pointer to Nfit Table

  @retval VOID        - Nothing

**/
VOID
AddSMBiosMgmtInfoTblToNfit(
   NVDIMM_FW_INTERFACE_TABLE    *pNfit
   )
{
  SMBIOS_MGMT_INFO_TABLE     *pSMBiosTbl;
  UINT16                     *Handles;
  UINT8                      i;

  if (pNfit == NULL) {
    return;
  }

  pSMBiosTbl = (SMBIOS_MGMT_INFO_TABLE *)(UINTN)(&pNfit->NfitTables[0] + pNfit->Length);
  pSMBiosTbl->Type = TYPE_SMBIOS_MGMT_INFO_DESCRIPTOR;
  pSMBiosTbl->Length = (sizeof(*pSMBiosTbl) + (mNumSMBiosHandles * 2));

  Handles = (UINT16 *)((UINT8 *)pSMBiosTbl + sizeof(*pSMBiosTbl));
  for (i = 0; i < mNumSMBiosHandles; i++) {
    Handles[i] = mSMBiosHandleInfo[i].SMBiosType17Handle;
  }
  //
  // Update the Nfit length as appropriate

  ASSERT((pNfit->Length + pSMBiosTbl->Length) <= mNfitSize);
  pNfit->Length += pSMBiosTbl->Length;
}


/*++

Routine Description:

This routine checks to see if the Limit field of the current SAD Table has been previously
accounted for thru another SAD. The idea is to eliminate redundancy while adding SADs to the Nfit.
This is accomplished by adding the current SAD's limit to the Sad_Info struct when the current Limit is not found
in the Limits of Sad_Info and increment NumOfSads of Sad_Info struct.

Arguments:

pSAD      - Pointer to the current SAD

Returns:

TRUE if this SAD's limit has been found in another SAD that was added previously. Else FALSE

--*/
BOOLEAN
CheckIfRedundantSAD (
   struct SADTable                 *pSAD
   )
{
  UINT8 i;

  ASSERT(pSAD != NULL);
  if (pSAD == NULL) {
    return FALSE;
  }

  if (mSAD_Info.NumOfSADs == 0) {
    mSAD_Info.Limits[0] = pSAD->Limit;
    mSAD_Info.NumOfSADs++;
  } else {
    for (i = 0; i < mSAD_Info.NumOfSADs; i++) {
      if (mSAD_Info.Limits[i] == pSAD->Limit) {
        return TRUE;
      }
    }
    if (i == mSAD_Info.NumOfSADs) {
      mSAD_Info.Limits[i] = pSAD->Limit;
      mSAD_Info.NumOfSADs++;
    }
  }
  return FALSE;
}

/**

Routine Description:

This routine basically builds NvDIMM F/W Interface Table, appending the sub-tables to Nfit
using the information from the SAD struct passed in.

Arguments:

  @param pNfit -  pointer to Nvdimm f/w interface table
  @param socket - socket number
  @param SadInx - SAD Index that needs to be added to Nfit

Returns:

  @retval VOID

**/
VOID
AppendToNFIT(
   NVDIMM_FW_INTERFACE_TABLE     *pNfit,
   UINT8                         socket,
   UINT8                         SadInx
   )
{
  struct SADTable       *pSAD;
  SPA_RANGE_DESC_TABLE  *pSPARDT = NULL;
  UINT64                SpaBase = 0;
  UINT8                 ChInterleave;
  UINT8                 iMCWays;
  UINT32                InterBitmap;
  UINT8                                 iMC = 0;
  BOOLEAN               IsRedundantSAD;
  UINT32                SktGran, ChGran;

  EFI_GUID PMEMRegionGuid = PMEM_REGION_GUID;
  EFI_GUID ControlRgnGuid = CONTROL_REGION_GUID;
  EFI_GUID BlkRegionGuid  = BLOCK_DATA_WINDOW_REGION_GUID;

  // If we would go outside bounds for SADintList[Sadinx], return.
  if (pNfit == NULL || socket >= MAX_SOCKET || SadInx >= SAD_RULES) {
    return;
  }

  pSAD = &mSystemMemoryMap->Socket[socket].SAD[SadInx];

  ASSERT(pSAD != NULL);
  if (pSAD == NULL) {
    return;
  }
  //
  // This SAD is to be added to the Nfit only when this memory range hasn't been added before thru another SAD previously
  //
  IsRedundantSAD = CheckIfRedundantSAD (pSAD);
  if (!IsRedundantSAD) {
    DEBUG((EFI_D_ERROR, "Nfit: NfitTables = 0x%X\n", &pNfit->NfitTables[0]));

    // Get the pointer to the next Nfit table, which in this case is SPARDT.
    pSPARDT = (SPA_RANGE_DESC_TABLE*)(UINTN)(&pNfit->NfitTables[0] + pNfit->Length);
    DEBUG((EFI_D_ERROR, "Nfit: pSPARDT = 0x%X\n", pSPARDT));

    pSPARDT->Type = TYPE_SPA_RANGE_DESCRIPTOR;

    // Right attributes will be set further down below, start with 0.
    pSPARDT->AddressRangeMemoryMappingAttribute = 0;

    if ((pSAD->type & MEM_TYPE_PMEM) || (pSAD->type & MEM_TYPE_PMEM_CACHE)) {
      DEBUG((EFI_D_ERROR, "Nfit: Type is either PMem or PMem_Cache\n"));
      CopyGuid(&pSPARDT->AddressRangeTypeGUID,  &PMEMRegionGuid);
      pSPARDT->AddressRangeMemoryMappingAttribute = EFI_MEMORY_NV | EFI_MEMORY_WB;
    } else if (pSAD->type & MEM_TYPE_CTRL) {
      DEBUG((EFI_D_ERROR, "Nfit: Type is Control Region\n"));
      CopyGuid(&pSPARDT->AddressRangeTypeGUID, &ControlRgnGuid);
      // Mem type uncacheable as its attribute for control region, per spec
      pSPARDT->AddressRangeMemoryMappingAttribute = EFI_MEMORY_UC;
    } else if (pSAD->type & MEM_TYPE_BLK_WINDOW) {
      DEBUG((EFI_D_ERROR, "Nfit: Type is Block Window\n"));
      CopyGuid(&pSPARDT->AddressRangeTypeGUID, &BlkRegionGuid);
      // Mem type write back cacheable for Blk Window region
      pSPARDT->AddressRangeMemoryMappingAttribute = EFI_MEMORY_WB;
    }

    if (pSAD->mirrored != 0) {
      pSPARDT->AddressRangeMemoryMappingAttribute |=  EFI_MEMORY_MORE_RELIABLE;
    }

    pSPARDT->SPARangeStructureIndex = mSPARangeDescTblIndex;
    mSPARangeDescTblIndex++;

    //
    // SPARDT Proximity Domain is the socket # (at least for now),
    // When SNC (Sub-Numa Cluster is enabled, this could change), we need to check when it is enabled.
    //
    if (mSystemMemoryMap->sncEnabled && (mSystemMemoryMap->NumOfCluster == 2)) {
      if (mSystemMemoryMap->Socket[socket].SADintList[SadInx][socket * MAX_IMC]) {
        // iMC0 of this socket is involved in this interleave set,
        iMC = 0;
      } else {
        iMC = 1;
      }
      pSPARDT->ProximityDomain = (socket * 2) + iMC;
    } else {
      // Determine which IMC channel bitmap to use
      iMC = GetMcIndexFromBitmap (pSAD->imcInterBitmap);
      pSPARDT->ProximityDomain = socket;
    }

    //
    // Update the Flag of the SPARDT
    if ((pSAD->type & MEM_TYPE_PMEM) || (pSAD->type & MEM_TYPE_PMEM_CACHE)) {
      pSPARDT->Flags = PROX_DOMAIN_VALID;
    } else {
      pSPARDT->Flags = 0;
    }

    // If SAD Index is zero, start address is 0
    if (SadInx) {
      // SAD Limit is in 64MB chunks and it starts from the previous SAD's limit, convert to bytes
      SpaBase = pSPARDT->SystemPhysicalAddressRangeBase = (UINT64)((UINTN)mSystemMemoryMap->Socket[socket].SAD[SadInx - 1].Limit << BITS_64MB_TO_BYTES);
      pSPARDT->SystemPhysicalAddressRangeLength = (UINT64)((UINTN)(mSystemMemoryMap->Socket[socket].SAD[SadInx].Limit - mSystemMemoryMap->Socket[socket].SAD[SadInx - 1].Limit) << BITS_64MB_TO_BYTES);
    } else {
      SpaBase = pSPARDT->SystemPhysicalAddressRangeBase = 0;
      pSPARDT->SystemPhysicalAddressRangeLength = (UINT64)((UINTN)mSystemMemoryMap->Socket[socket].SAD[SadInx].Limit << BITS_64MB_TO_BYTES); // SAD in 64MB chunks, Convert it to bytes.
    }
    DEBUG((EFI_D_ERROR, "Nfit: StartAddr = 0x%16lx\n", pSPARDT->SystemPhysicalAddressRangeBase));
    DEBUG((EFI_D_ERROR, "Nfit: Size = 0x%16lx\n", pSPARDT->SystemPhysicalAddressRangeLength));

    pSPARDT->Length = sizeof(SPA_RANGE_DESC_TABLE);
    //
    // At this point we've updated the SPARDT table, include its length into Nfit Length
    ASSERT((pNfit->Length + pSPARDT->Length) <= mNfitSize);
    pNfit->Length += pSPARDT->Length;
  }

  ChInterleave =  GetChInterleave(socket, iMC, SadInx);
  iMCWays =       GetImcWays(socket, SadInx);
  InterBitmap =   GetChInterleaveBitmap(socket, SadInx);
  SktGran =       GetSocketGranularity(pSAD);
  ChGran =        GetChannelGranularity(pSAD);

  if (pSAD->type & MEM_TYPE_CTRL) {
    AppendCtrlRgnsForChannels(pNfit, socket, InterBitmap, IFC_APPDIRECT_MODE);
    AppendCtrlRgnsForChannels(pNfit, socket, InterBitmap, IFC_STORAGE_MODE);
  }

  switch (ChInterleave) {
  case CH_1WAY:
    {
      if (pSAD->type & MEM_TYPE_BLK_WINDOW) {
        BuildBlkWndRgnFor1WayChInt(pNfit, socket, SadInx, iMCWays);
      }
      // 1st Case - if channel is interleaved across both IMCs
      // 1 or 2 iMC, 1 Way Channel Interleaved, but which channel is Interleaved?
      if (!IsRedundantSAD) {
        BuildMemDevFor1WayChInt(pNfit, iMCWays, pSPARDT, SpaBase);
        Build1ChWayIntDescTbls(pNfit, SktGran, ChGran, iMCWays);
      }
    }
    break;
  case CH_2WAY:
    {
      if (pSAD->type & MEM_TYPE_BLK_WINDOW) {
        BuildBlkWndRgnFor2WayChInt (pNfit, socket, SadInx, iMCWays);
      }
      // Build Nvdimm F/W Interface Table 2 way Channel Interleaved
      if (!IsRedundantSAD) {
        BuildMemDevFor2WayChInt(pNfit, iMCWays, pSPARDT, SpaBase);
        Build2ChWayIntDescTbls(pNfit, SktGran, ChGran, iMCWays);
      }
    }
    break;
  case CH_3WAY:
    {
      if (pSAD->type & MEM_TYPE_BLK_WINDOW) {
        BuildBlkWndRgnFor3WayChInt (pNfit, socket, SadInx, iMCWays);
      }
      // Build Nvdimm F/W Interface Table 3 way Channel Interleaved
      if (!IsRedundantSAD) {
        BuildMemDevFor3WayChInt(pNfit, iMCWays, pSPARDT, SpaBase);
        Build3ChWayIntDescTbls(pNfit, SktGran, ChGran, iMCWays);
      }
    }
    break;
  }
}

/**

Routine Description:

This routine updates the checksum field of already initialized NFIT table.

Arguments:

  @param CrystalRidgeTablePtr - pointer to NFIT table address allocated
                                in platform ACPI code

Returns:

  @retval EFI_SUCCESS - checksum updated successfully
  @retval EFI_INVALID_PARAMETER - CrystalRidgeTablePtr is NULL

**/
EFI_STATUS
UpdateNfitTableChecksum(
  UINT64 *CrystalRidgeTablePtr
  )
{
  NVDIMM_FW_INTERFACE_TABLE *pNfit;

  if (CrystalRidgeTablePtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  pNfit = (NVDIMM_FW_INTERFACE_TABLE *)CrystalRidgeTablePtr;
  pNfit->Checksum = ChsumTbl((UINT8 *)pNfit, pNfit->Length);
  return EFI_SUCCESS;
}

/**

Routine Description:

This routine basically builds NvDIMM F/W Interface Table,
appending the needed sub-tables.

Arguments:

  @param CrystalRidgeTablePtr - pointer to NFIT table address allocated
                                in platform ACPI code

Returns:

  @retval EFI_SUCCESS - NFIT table created successfully.
  @retval EFI_INVALID_PARAMETER - CrystalRidgeTablePtr is NULL

**/
EFI_STATUS
BuildNfitTable(
  UINT64 *CrystalRidgeTablePtr,
  UINTN  CrystalRidgeTableSize
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  NVDIMM_FW_INTERFACE_TABLE   *pNfit;
  UINT8                       socket;
  UINT8                       i;
  UINT64                      TempOemTableId;

  if (CrystalRidgeTablePtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Start building the Nfit based on the current memory configuration,
  // before it can be published as part of the Dsdt. Nfit is required ONLY
  // when AEP Dimms are installed
  //

  // Initialize Sad Info, particularly Number of SADs participating in Nfit
  mSAD_Info.NumOfSADs = 0;

  // Also initialize others that are for Unique ID numbers.
  // This Index is 1 based.
  mSPARangeDescTblIndex = 1;
  mMemDevRegionalID = mNumInterleaveDescTbls = 0;

  GenerateCtrlRegionsList();

  //
  // Adding support for Nfit, Adding NvDimm F/W Interface Table as part of the DsDt
  //
  pNfit = (NVDIMM_FW_INTERFACE_TABLE *)CrystalRidgeTablePtr;
  DEBUG((EFI_D_ERROR, "Nfit: main table address = 0x%p\n", pNfit));

  mNfitSize = (UINT32)CrystalRidgeTableSize;
  ZeroMem(pNfit, mNfitSize);

  TempOemTableId  = PcdGet64(PcdAcpiDefaultOemTableId);
  //
  // Create NFIT header
  //
  pNfit->Signature      = NVDIMM_FW_INTERFACE_TABLE_SIGNATURE;
  pNfit->Length         = 0;
  pNfit->Revision       = NVDIMM_FW_INTERFACE_TABLE_REVISION;
  CopyMem(pNfit->OemID,     PcdGetPtr (PcdAcpiDefaultOemId), sizeof(pNfit->OemID));
  CopyMem(pNfit->OemTblID,  &TempOemTableId, sizeof(pNfit->OemTblID));
  pNfit->OemRevision    = EFI_ACPI_OEM_REVISION;
  pNfit->CreatorID      = EFI_ACPI_CREATOR_ID;
  pNfit->CreatorRev     = EFI_ACPI_CREATOR_REVISION;

  //
  // Create NFIT sub-tables
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for (i = 0; i < SAD_RULES; i++) {
      // Only local SADs with type of PMEM (CACHE), BLK, CTRL should be taken into consideration
      if (mSystemMemoryMap->Socket[socket].SAD[i].local &&
          (mSystemMemoryMap->Socket[socket].SAD[i].type &
          (MEM_TYPE_PMEM | MEM_TYPE_PMEM_CACHE | MEM_TYPE_BLK_WINDOW | MEM_TYPE_CTRL)
          )) {
        AppendToNFIT(pNfit, socket, i);
      }
    }
  }

  //
  // Add Flush Hint Address to Nfit
  //
  AddFlushHintAddrTblToNfit(pNfit);

  //
  // Add SMBIOS Management Information Table to Nfit
  //
  AddSMBiosMgmtInfoTblToNfit(pNfit);

  pNfit->Length += sizeof(NFIT_HEADER);
  DEBUG((EFI_D_ERROR, "Nfit: After adding all tables, Nfit Length = 0x%X\n", pNfit->Length));

  Status = UpdateNfitTableChecksum(CrystalRidgeTablePtr);

  return Status;
}
