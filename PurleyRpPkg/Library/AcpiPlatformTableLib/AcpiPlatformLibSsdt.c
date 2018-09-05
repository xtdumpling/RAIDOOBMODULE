/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
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


  @file AcpiPlatformLibSsdt.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"

#pragma optimize("",off)

extern BIOS_ACPI_PARAM             *mAcpiParameter;
extern EFI_IIO_UDS_PROTOCOL        *mIioUds;

extern SOCKET_MEMORY_CONFIGURATION mSocketMemoryConfiguration;
extern SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;

extern BOOLEAN      mCpuOrderSorted;
extern const UINT32 *mApicIdMap;


//
// iASL generates an offset table .h file which contains offsets from the start of DSDT AML for the following items:
//   Name, OperationRegion, WORDBusNumber, DWORDMemory, QWORDMemory, WORDIO
// The last 4 items must populate the last field (DescriptorName) for iASL to put them in the offset table.
//
/*
typedef struct {
    char                  *Pathname;
    char                  *FixTag;
} BIOS_FIXUP_AML_OFFSET_TABLE_ENTRY;

//
// BIOS fixup table for Purley platform
//
BIOS_FIXUP_AML_OFFSET_TABLE_ENTRY BiosFixupAmlOffsetTable[] =
{
    //
    // Cast AML_RESOURCE_ADDRESS16 for WORDBusNumber operator so AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // Cast AML_RESOURCE_ADDRESS16 for WORDIO operator so AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // Cast AML_RESOURCE_ADDRESS32 for DWORDMemory operator so AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // Cast AML_RESOURCE_ADDRESS64 for QWORDMemory operator so AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // Name operator AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // OperationRegion AML_OFFSET_TABLE_ENTRY.operator offset is at value to change.
    //
    {"PSYS",                "FIX0"}, // OperationRegion() in Acpi\AcpiTables\Dsdt\CommonPlatform.asi
    {"_SB_.PCXX.FIX1",      "FIX1"}, // WORDBusNumber()   in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX2",      "FIX2"}, // WORDIO()          in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX3",      "FIX3"}, // DWORDMemory()     in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX4",      "FIX4"}, // QWORDMemory()     in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX5",      "FIX5"}, // DWORDMemory()     in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX6",      "FIX6"}, // WORDIO()          in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX7",      "FIX7"}, // WORDIO()          in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.MCTL",      "FIX8"}, // OperationRegion() in Acpi\AcpiTables\Dsdt\PcieHp.asi (for all root ports)
    {"_SB_.NVD_.FIX9",      "FIX9"}, // QWORDMemory()     in Acpi\AcpiTables\Dsdt\Ngn.asi
    {"_SB_.NVD_.CRCM",      "FIXA"}, // OperationRegion() in Acpi\AcpiTables\Dsdt\Ngn.asi
    {0, 0}                           // table terminator
};
*/

/**

    GC_TODO: add routine description

    @param ThreadId      - In: Acpi thread number
    @param CpuMiscData - In/Out: Pointer to thread's CPU_MISC_DTAT struct

    @retval EFI_NOT_FOUND - GC_TODO: add retval description
    @retval EFI_SUCCESS   - GC_TODO: add retval description

**/
EFI_STATUS
LocateCpuEistInfo (
  IN   UINT32                        CpuIndex,
  OUT  CPU_MISC_DATA                 **CpuMiscData
  )
{
  UINTN                       Index;
  UINT32                      socket, ApicId;

  socket = CpuIndex / (MAX_CORE * MAX_THREAD);
  Index  = CpuIndex % (MAX_CORE * MAX_THREAD);

  ApicId = mAcpiParameter->ProcessorApicIdBase[socket] + mApicIdMap[Index];

  for (Index = 0; Index < mCpuConfigLibConfigContextBuffer->NumberOfProcessors; Index++) {
    if (mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[Index].CpuMiscData.ApicID == ApicId) {
      *CpuMiscData = &mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[Index].CpuMiscData;
      break;
    }
  }

  if(*CpuMiscData == NULL) {  //use SBSP's data
    *CpuMiscData = &mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[0].CpuMiscData;
  }

  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param None

    @retval Status - GC_TODO: add retval description

**/
BOOLEAN
IsTurboModeEnabled (
    VOID
  )
{
  EFI_CPUID_REGISTER     CpuidRegisters;
  BOOLEAN                Status;
  UINT64                 MsrValue;

  Status = FALSE;
  AsmCpuid (EFI_CPUID_POWER_MANAGEMENT_PARAMS, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  if ((CpuidRegisters.RegEax & EFI_CPUID_POWER_MANAGEMENT_EAX_TURBO) != 0) {
    //
    // Turbo mode is supported on this processor (Available and Visible)
    //
    MsrValue = AsmReadMsr64 (EFI_MSR_IA32_MISC_ENABLE);

    if (!(MsrValue & B_EFI_MSR_IA32_MISC_DISABLE_TURBO)) {
      //
      // Turbo mode is supported on this processor (Available but Hidden)
      //
      Status = TRUE;
    }
  }

  return Status;

}

/**

    Update the CPU PM SSDT table

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchCpuPmSsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  UINT8                       *CurrPtr;
  UINT8                       *SsdtPointer;
  UINT32                      Signature;
  UINT32                      CpuFixes;
  UINT32                      CpuSkt;
  UINT32                      CpuIndex, ThreadIndex;
  EFI_ACPI_NAMEPACK_DWORD     *NamePtr;
  UINT8                       DOMNValue;
  UINT8                       NCPUValue;
  UINT8                       HWALValue;
  EFI_ACPI_NAME_COMMAND       *PsdPackage;
  PSD_PACKAGE_LAYOUT          *PsdPackageItemPtr;

  //
  // Loop through the ASL looking for values that we must fix up.
  //
  DOMNValue = 0;
  NCPUValue = 0;
  HWALValue = 0;
  CpuFixes  = 0;
  CpuSkt    = 0;
  CpuIndex  = 0;
  ThreadIndex  = 0;
  CurrPtr   = (UINT8 *) Table;

  // PCPS - Update HWAL and NCPU
  HWALValue = (UINT8) mSocketPowermanagementConfiguration.ProcessorEistPsdFunc;

  if (mIioUds->IioUdsPtr->SystemStatus.CpuPCPSInfo & B_PCPS_SPD_ENABLE) {
    if (mSocketPowermanagementConfiguration.PStateDomain) {
        NCPUValue = (UINT8) mIioUds->IioUdsPtr->SystemStatus.CpuPCPSInfo & 0xFF;
    } else {
      if (mIioUds->IioUdsPtr->SystemStatus.CpuPCPSInfo & B_PCPS_HT_ENABLE) {
        NCPUValue = 2;
      } else {
        NCPUValue = 1;
      }
    }

    if ((mSocketPowermanagementConfiguration.ProcessorSinglePCTLEn) && (!mSocketPowermanagementConfiguration.PStateDomain)) {
        HWALValue = 2;  //force SW_ANY
    }

    if (mSocketPowermanagementConfiguration.ProcessorSinglePCTLEn == 2) {
        HWALValue = 1;  //force SW_ALL
    }
  } else {
    if (mSocketPowermanagementConfiguration.PStateDomain) {
	NCPUValue = (UINT8)(MAX_CORE * MAX_THREAD);
    } else {
        NCPUValue = 1;
    }
  }

  for (SsdtPointer = CurrPtr; SsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length); SsdtPointer++) {
    Signature = *(UINT32 *) SsdtPointer;
    switch (Signature) {
      case SIGNATURE_32 ('_', 'S', 'B', '_'):
        //SKTX
        CpuSkt = *(SsdtPointer + 7);
        if(CpuSkt >= '0' && CpuSkt <= '3') {
          CpuSkt -= '0';

          //
          // _PRXX ('00 to 013')
          //
          CpuIndex = *(SsdtPointer + 11);
          if( *(SsdtPointer + 10) == '0') {
            if ((CpuIndex >= '0') && (CpuIndex <= '9')) {
              CpuIndex -= '0';
            } else if ((CpuIndex >= 'A') && (CpuIndex <= 'F')) {
              CpuIndex = 10 + (CpuIndex - 'A');
            }
          } else if( *(SsdtPointer + 10) == '1')  { //is 0x10 - 0x1F
            if ((CpuIndex >= '0') && (CpuIndex <= '9')) {//must be 0x10 - 0x19
              CpuIndex = 0x10 + (CpuIndex - '0');
            } else if ((CpuIndex >= 'A') && (CpuIndex <= 'F')){ //must be 0x1A - 0x1F
              CpuIndex = 0x1A + (CpuIndex - 'A');
            }
          } else if( *(SsdtPointer + 10) == '2')  { //is 0x20 - 0x2F
            if ((CpuIndex >= '0') && (CpuIndex <= '9')) {//must be 0x20 - 0x29
              CpuIndex = 0x20 + (CpuIndex - '0');
            } else if ((CpuIndex >= 'A') && (CpuIndex <= 'F')){ //must be 0x2A - 0x2F
              CpuIndex = 0x2A + (CpuIndex - 'A');
            }
          } else { //must be 0x30 - 0x37
            CpuIndex -= '0';
            CpuIndex += 0x30;
          }
          ThreadIndex  = CpuIndex;

          CpuIndex = (CpuSkt << 6) + ThreadIndex;
        }

        // PCPS - Update DOMN
        DOMNValue = (UINT8) CpuSkt;

        if (mIioUds->IioUdsPtr->SystemStatus.CpuPCPSInfo & B_PCPS_SPD_ENABLE) {
	        if (mSocketPowermanagementConfiguration.PStateDomain == 0) {
	          DOMNValue = (UINT8)(((MAX_CORE * MAX_THREAD * CpuSkt) + ThreadIndex) >> 1);
	        }
	      } else {
	        if (mSocketPowermanagementConfiguration.PStateDomain == 0) {
	          DOMNValue = (UINT8)((MAX_CORE * MAX_THREAD * CpuSkt) + ThreadIndex);
          }
        }
        //DEBUG ((EFI_D_ERROR, "\n::ACPI:: CpuSkt: %d  CpuIndex: %x,   NCPUValue = %x,  HWALValue = %x,   DOMNValue = %x\n  ", \
        //                     CpuSkt, CpuIndex, NCPUValue, HWALValue, DOMNValue));
        CpuFixes++;
        break;

      case SIGNATURE_32 ('D', 'O', 'M', 'N'):

        NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (SsdtPointer);
        if (NamePtr->StartByte != AML_NAME_OP) {
          continue;
        }

        if (NamePtr->Size != AML_NAME_DWORD_SIZE) {
          continue;
        }

        NamePtr->Value = (UINT32) DOMNValue;
        break;

      case SIGNATURE_32 ('N', 'C', 'P', 'U'):

        NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (SsdtPointer);
        if (NamePtr->StartByte != AML_NAME_OP) {
          continue;
        }

        if (NamePtr->Size != AML_NAME_DWORD_SIZE) {
          continue;
        }

        NamePtr->Value = (UINT32) NCPUValue;
        break;

// APTIOV_SERVER_OVERRIDE_RC_START
#if 0
      case SIGNATURE_32 ('H', 'W', 'A', 'L'):

        NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (SsdtPointer);
        if (NamePtr->StartByte != AML_NAME_OP) {
          continue;
        }

        if (NamePtr->Size != AML_NAME_DWORD_SIZE) {
          continue;
        }

        NamePtr->Value = (UINT32) HWALValue;
        break;
#endif
// APTIOV_SERVER_OVERRIDE_RC_END

      case SIGNATURE_32 ('P', 'S', 'D', 'C'):
      case SIGNATURE_32 ('P', 'S', 'D', 'D'):
      case SIGNATURE_32 ('P', 'S', 'D', 'E'):

        PsdPackage = ACPI_NAME_COMMAND_FROM_NAME_STR (SsdtPointer);
        if (PsdPackage->StartByte != AML_NAME_OP) {
          continue;
        }

        PsdPackageItemPtr       = (PSD_PACKAGE_LAYOUT *) ((UINT8 *) PsdPackage);
        //DEBUG((EFI_D_ERROR, "\n:::: PRINT PsdPackageItemPtr table:     %x   is detected...\n", PsdPackage->NameStr));
        //DEBUG((EFI_D_ERROR, "::::                                   Domain = %x,    CoordType = %x,   NumProcessors = %x\n", PsdPackageItemPtr->Domain, PsdPackageItemPtr->CoordType, PsdPackageItemPtr->NumProcessors ));

        PsdPackageItemPtr->Domain = DOMNValue;
        PsdPackageItemPtr->NumProcessors = NCPUValue;
        //SsdtPointer = (UINT8 *) PsdPackage + sizeof (EFI_ACPI_NAME_COMMAND) + sizeof (EFI_PSD_PACKAGE);
        //DEBUG((EFI_D_ERROR, "::::  PsdPackage = %x,    PsdPackageItemPtr = %x,   SsdtPointer = %x\n", (UINT8 *)PsdPackage, (UINT8 *)PsdPackageItemPtr, (UINT8 *)SsdtPointer ));
        //DEBUG((EFI_D_ERROR, "::::      Updated  Domain = %x,    CoordType = %x,   NumProcessors = %x\n", PsdPackageItemPtr->Domain, PsdPackageItemPtr->CoordType, PsdPackageItemPtr->NumProcessors ));

        break;
    } // switch
  } // for

  //
  // N fixes together currently
  //
  ASSERT (CpuFixes == (UINT32) MAX_CPU_NUM);

  return EFI_SUCCESS;
}

/**

    Update the OEM1 P-State SSDT table (EIST)

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchOem1SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS                  Status;
  UINT8                       *CurrPtr;
  UINT8                       *SsdtPointer;
  UINT32                      Signature;
  UINT32                      CpuFixes;
  UINT32                      NpssFixes;
  UINT32                      CpuSkt;
  UINT32                      CpuIndex, ThreadIndex;
  UINT32                      PackageSize;
  UINT32                      NewPackageSize;
  UINT32                      AdjustSize;
  UINTN                       TableIndex;
  EFI_ACPI_NAME_COMMAND       *PssTable;
  EFI_PSS_PACKAGE             *PssTableItemPtr;
  CPU_MISC_DATA               *CpuMiscData;
  FVID_ENTRY                  *PssState;

  //
  // Loop through the ASL looking for values that we must fix up.
  //
  NpssFixes = 0;
  CpuFixes  = 0;
  CpuSkt    = 0;
  CpuIndex  = 0;
  ThreadIndex  = 0;
  CurrPtr   = (UINT8 *) Table;
  CpuMiscData = NULL;

  Status = LocateCpuEistInfo (0, &CpuMiscData);   //get BSP's data
  if( (EFI_ERROR (Status)) || (CpuMiscData == NULL ) ){
    DEBUG ((EFI_D_ERROR, " PatchGv3SsdtTable - EIST info for BSP index not found \n"));
    return Status;
  }

  for (SsdtPointer = CurrPtr; SsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length); SsdtPointer++) {
    Signature = *(UINT32 *) SsdtPointer;
    switch (Signature) {
      case SIGNATURE_32 ('_', 'S', 'B', '_'):
        //SKTX
        CpuSkt = *(SsdtPointer + 7);
        if(CpuSkt >= '0' && CpuSkt <= '3') {
          CpuSkt -= '0';

          //
          // _PRXX ('00 to 013')
          //
          CpuIndex = *(SsdtPointer + 11);
          if( *(SsdtPointer + 10) == '0') {
            if ((CpuIndex >= '0') && (CpuIndex <= '9')) {
              CpuIndex -= '0';
            } else if ((CpuIndex >= 'A') && (CpuIndex <= 'F')) {
              CpuIndex = 10 + (CpuIndex - 'A');
            }
          } else if( *(SsdtPointer + 10) == '1')  { //is 0x10 - 0x1F
            if ((CpuIndex >= '0') && (CpuIndex <= '9')) {//must be 0x10 - 0x19
              CpuIndex = 0x10 + (CpuIndex - '0');
            } else if ((CpuIndex >= 'A') && (CpuIndex <= 'F')){ //must be 0x1A - 0x1F
              CpuIndex = 0x1A + (CpuIndex - 'A');
            }
          } else if( *(SsdtPointer + 10) == '2')  { //is 0x20 - 0x2F
            if ((CpuIndex >= '0') && (CpuIndex <= '9')) {//must be 0x20 - 0x29
              CpuIndex = 0x20 + (CpuIndex - '0');
            } else if ((CpuIndex >= 'A') && (CpuIndex <= 'F')){ //must be 0x2A - 0x2F
              CpuIndex = 0x2A + (CpuIndex - 'A');
            }
          } else { //must be 0x30 - 0x37
            CpuIndex -= '0';
            CpuIndex += 0x30;
          }
          ThreadIndex  = CpuIndex;

          CpuIndex = (CpuSkt << 6) + ThreadIndex;
        }

        CpuFixes++;
        CpuMiscData = NULL;
        LocateCpuEistInfo (0, &CpuMiscData);  //use CPU0 for update NPSS and SPSS
        break;

      case SIGNATURE_32 ('N', 'P', 'S', 'S'):
      case SIGNATURE_32 ('S', 'P', 'S', 'S'):

        PssTable = ACPI_NAME_COMMAND_FROM_NAME_STR (SsdtPointer);
        if (PssTable->StartByte != AML_NAME_OP) {
          continue;
        }

        ASSERT (CpuMiscData != NULL);
        PssState = CpuMiscData->FvidTable;

        AdjustSize  = PssTable->NumEntries * sizeof (EFI_PSS_PACKAGE);
        AdjustSize -= (UINT32)(CpuMiscData->NumberOfPStates * sizeof (EFI_PSS_PACKAGE));
        PackageSize     = (PssTable->Size & 0xF) + ((PssTable->Size & 0xFF00) >> 4);
        NewPackageSize  = PackageSize - AdjustSize;
        PssTable->Size  = (UINT16) ((NewPackageSize & 0xF) + ((NewPackageSize & 0x0FF0) << 4));

        //
        // Set most significant two bits of byte zero to 01, meaning two bytes used
        //
        PssTable->Size |= 0x40;

        //
        // Set unused table to Noop Code
        //
        SetMem (
          (UINT8 *) PssTable + NewPackageSize + AML_NAME_PREFIX_SIZE,
          AdjustSize,
          AML_NOOP_OP
          );
        PssTable->NumEntries  = (UINT8) CpuMiscData->NumberOfPStates;
        PssTableItemPtr       = (EFI_PSS_PACKAGE *) ((UINT8 *) PssTable + sizeof (EFI_ACPI_NAME_COMMAND));

        //
        // Update the size
        //
        //DEBUG ((EFI_D_ERROR, "Frequecy   Power   Latency  MsterLat  Control   Status\n"));
        for (TableIndex = 0; TableIndex < CpuMiscData->NumberOfPStates; TableIndex++) {
          PssTableItemPtr->CoreFreq = (UINT32) (CpuMiscData->IntendedFsbFrequency * PssState[TableIndex].Ratio);
          if (mSocketPowermanagementConfiguration.TurboMode && (TableIndex == 0) && IsTurboModeEnabled()) {
            PssTableItemPtr->CoreFreq = (UINT32)((CpuMiscData->IntendedFsbFrequency * PssState[TableIndex + 1].Ratio) + 1);
          }

          //
          // If Turbo mode is supported, add one to the Max Non-Turbo frequency
          //
//          if ((mSocketPowermanagementConfiguration.TurboMode && mSocketPowermanagementConfiguration.TurboP01) && (TableIndex == 0)) {
//            PssTableItemPtr->CoreFreq = (UINT32)((CpuMiscData->IntendedFsbFrequency * PssState[TableIndex + 1].Ratio) + 1);
//          } else {
//            PssTableItemPtr->CoreFreq = (UINT32)(CpuMiscData->IntendedFsbFrequency * PssState[TableIndex].Ratio);
//          }

          //if (CpuFixes == 1) {
          //  DEBUG ((EFI_D_ERROR, "[%d] %d\n", TableIndex, PssTableItemPtr->CoreFreq));
          //}

          PssTableItemPtr->Power    =  (UINT32)(PssState[TableIndex].Power); // when calulate Tdp already make it mW;
          if (PssTable->NameStr == SIGNATURE_32 ('N', 'P', 'S', 'S')) {
            PssTableItemPtr->TransLatency = (UINT32)(PssState[TableIndex].TransitionLatency);
              PssTableItemPtr->Control  = (UINT32)(PssState[TableIndex].Ratio << 8);
              PssTableItemPtr->Status   = (UINT32)(PssState[TableIndex].Ratio << 8);
          } else {
            //
            // This method should be supported by SMM PPM Handler
            //
            // Status is simply the state number.
            // Use the state number w/ OS command value so that the
            // legacy interface may be used.  Latency for SMM is 100 + BM latency.
            PssTableItemPtr->Status   = (UINT32)TableIndex;
            PssTableItemPtr->TransLatency = (UINT32)(100 + PssState[TableIndex].TransitionLatency);
            PssTableItemPtr->Control  = (UINT32)(SW_SMI_OS_REQUEST | (TableIndex << 8));
          }

          PssTableItemPtr->BMLatency    = (UINT32)(PssState[TableIndex].BusMasterLatency);
/*
          if (CpuFixes == 1) {
            DEBUG ( (EFI_D_ERROR, "::ACPI TBL:: P%d:  PssTableItemPtr->CoreFreq:%x, Power:%x, TransLatency:%x\n", \
              TableIndex, \
              PssTableItemPtr->CoreFreq, \
              PssTableItemPtr->Power, \
              PssTableItemPtr->TransLatency
            ) );
            DEBUG ( (EFI_D_ERROR, "                   BMLatency:%x, Control:%x, Status:%x\n", \
              PssTableItemPtr->BMLatency, \
              PssTableItemPtr->Control, \
              PssTableItemPtr->Status
            ) );
          }
*/

          PssTableItemPtr++;
        }

        if (PssTable->NameStr == SIGNATURE_32 ('N', 'P', 'S', 'S')) {
          NpssFixes++;
        } //else {
          //SpssFixes++;
        //}

        SsdtPointer = (UINT8 *) PssTable + PackageSize;
        break;

    } // switch
  } // for

  //
  // N fixes together currently
  //
  // APTIOV_SERVER_OVERRIDE_RC_START
  //ASSERT (CpuFixes == (UINT32) MAX_CPU_NUM); 
  // APTIOV_SERVER_OVERRIDE_RC_END

  if (!mSocketPowermanagementConfiguration.ProcessorEistEnable || (mSocketPowermanagementConfiguration.ProcessorHWPMEnable > 1) ) {
      Status = EFI_UNSUPPORTED;  //CPU EIST
      return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PatchSsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  ,IN OUT EFI_ACPI_TABLE_VERSION     *Version
  )
{
  EFI_STATUS                    Status;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;

  Status = EFI_SUCCESS;
  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER   *)Table;

    //
    // Do not load the xHCI table. It is handled by separate function.
    //
    TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
    if (CompareMem (&TableHeader->OemTableId, "xh_", 3) == 0) {
      DEBUG((DEBUG_ERROR,"Xhci TableHeader->OemTableId = %x\n ", TableHeader->OemTableId));
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
    }
    if (TableHeader->OemTableId == SIGNATURE_64 ('S', 'S', 'D', 'T', ' ', ' ', 'P', 'M')) {
      PatchCpuPmSsdtTable(Table);  //CPU PM
    }

  return Status;
}

/**

    Update the OEM2 HWP SSDT table

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchOem2SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS                  Status;
  UINT8                       *CurrPtr;
  UINT8                       *SsdtPointer;
  EFI_ACPI_NAME_COMMAND       *CpcTable;
  EFI_CPC_PACKAGE             *CpcTableItemPtr;
  UINT32                      Signature;
  UINT32                      NominalPerfP1;
  UINT64                      MsrValue;

  if ( (mSocketPowermanagementConfiguration.ProcessorHWPMEnable == 0) || (mSocketPowermanagementConfiguration.ProcessorHWPMEnable == 2)) {
      return EFI_UNSUPPORTED;
  }

  Status = EFI_SUCCESS;
  CurrPtr   = (UINT8 *) Table;

  MsrValue = AsmReadMsr64 (EFI_PLATFORM_INFORMATION);

  //NominalPerformance = MSR_PLATFORM_INFO[15:8]
  NominalPerfP1 = (UINT32) ( (MsrValue >> 8) & (UINT64) B_EFI_PLATFORM_INFO_RATIO_MASK );
  if (mIioUds->IioUdsPtr->SystemStatus.MinimumCpuStepping <  B0_REV_SKX) {
    // Multiply NominalPerformance value by 4 for Astep
    NominalPerfP1 = NominalPerfP1 << 2;
  }
  //DEBUG((EFI_D_ERROR, "\n:::: Read MSR EFI_PLATFORM_INFORMATION = %x %x,  NominalPerfP1 = %x\n", (MsrValue >> 32), MsrValue, NominalPerfP1));

  for (SsdtPointer = CurrPtr; SsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length); SsdtPointer++) {
    Signature = *(UINT32 *) SsdtPointer;
    switch (Signature) {

      case SIGNATURE_32 ('C', 'P', 'C', 'P'):

        CpcTable = ACPI_NAME_COMMAND_FROM_NAME_STR (SsdtPointer);
        if (CpcTable->StartByte != AML_NAME_OP) {
          continue;
        }

        CpcTableItemPtr       = (EFI_CPC_PACKAGE *) ((UINT8 *) CpcTable + sizeof (EFI_ACPI_NAME_COMMAND));
        CpcTableItemPtr->NominalPerf = NominalPerfP1;

        //DEBUG((EFI_D_ERROR, "\n::::         CpcTable table NominalPerf:           %x\n", CpcTableItemPtr->NominalPerf));

        break;

    } // switch
  } // for

  return EFI_SUCCESS;
}

/**

    Update the OEM3 T-State SSDT table (TST)

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchOem3SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  if (!mSocketPowermanagementConfiguration.TStateEnable || (mSocketPowermanagementConfiguration.ProcessorHWPMEnable > 1) ) {
      return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**

    Update the OEM4 C State SSDT table (CST)

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchOem4SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  if (mSocketPowermanagementConfiguration.ProcessorAutonomousCstateEnable) {
      return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}