//**********************************************************************
//**********************************************************************
//**                                                                  **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.           **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Draft template v0.1.
//    Reason:
//    Auditor:  Jimmy Chiu
//    Date:     Jan/17/2017
//
//**********************************************************************
/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2017, Intel Corporation.

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


  @file AcpiPlatformHooks.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"
#include <Library/PchCycleDecodingLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <PchAccess.h>            //SMCPKG_SUPPORT
#include <Register/PchRegsP2sb.h> //SMCPKG_SUPPORT
#include <Library/MmPciBaseLib.h> //SMCPKG_SUPPORT

#pragma optimize("",off)

EFI_PLATFORM_INFO           *mPlatformInfo;
BIOS_ACPI_PARAM             *mAcpiParameter;
EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;
EFI_IIO_UDS_PROTOCOL        *mIioUds;


UINT32                      mNumOfBitShift;
BOOLEAN                     mForceX2ApicId;
BOOLEAN                     mX2ApicEnabled;

struct SystemMemoryMapHob   *mSystemMemoryMap;

SOCKET_MEMORY_CONFIGURATION   mSocketMemoryConfiguration;
SOCKET_MP_LINK_CONFIGURATION  mSocketMpLinkConfiguration;
SOCKET_IIO_CONFIGURATION      mSocketIioConfiguration;
SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;
SOCKET_COMMONRC_CONFIGURATION mSocketCommonRcConfiguration;
SOCKET_PROCESSORCORE_CONFIGURATION   mSocketProcessorCoreConfiguration;


EFI_CRYSTAL_RIDGE_PROTOCOL *mCrystalRidgeProtocol = NULL;
EFI_JEDEC_NVDIMM_PROTOCOL *mJedecNvDimmProtocol = NULL;

EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE *mSpcrTable = NULL;


//Lib Private data
static SYSTEM_BOARD_PROTOCOL             *mSystemBoard;


/**
  The constructor function of AcpiPlatormL Library.

  The constructor function caches the value of PCD entry

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
AcpiPlatformLibConstructor (
  VOID //IN EFI_HANDLE        ImageHandle,
  //IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_HOB_GUID_TYPE             *GuidHob;

  GuidHob    = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
      return EFI_NOT_FOUND;
  }
  mPlatformInfo = GET_GUID_HOB_DATA(GuidHob); 

  //
  // Locate System Board Protocol
  //
  Status = gBS->LocateProtocol (&gEfiDxeSystemBoardProtocolGuid, NULL, (VOID**)&mSystemBoard);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate the IIO Protocol Interface
  //
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid,NULL,&mIioUds);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  GuidHob    = GetFirstGuidHob (&gEfiMemoryMapGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
      return EFI_NOT_FOUND;
  }
  mSystemMemoryMap = GET_GUID_HOB_DATA(GuidHob); 

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &mMpService);
  ASSERT_EFI_ERROR (Status);
  //
  // Determine the number of processors
  //
  mMpService->GetNumberOfProcessors (
              mMpService,
              &mNumberOfCPUs,
              &mNumberOfEnabledCPUs
              );
  ASSERT (mNumberOfCPUs <= MAX_CPU_NUM && mNumberOfEnabledCPUs >= 1);

  return EFI_SUCCESS;
}

EFI_STATUS
AcpiPlatformHooksIsActiveTable (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  )
{
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader;
  EFI_STATUS                      Status;
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                SetupData;
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  SYSTEM_CONFIGURATION            mSystemConfiguration;
#if SMCPKG_SUPPORT && HPET_SUPPORT
  UINTN                           P2sbBase;
#endif
  
  Status = GetEntireConfig (&SetupData);
  ASSERT_EFI_ERROR (Status);
  CopyMem (&mSystemConfiguration, &(SetupData.SystemConfig), sizeof(SYSTEM_CONFIGURATION));

#if SMCPKG_SUPPORT && HPET_SUPPORT
  P2sbBase        = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_P2SB,
                      PCI_FUNCTION_NUMBER_PCH_P2SB
                      );
#endif

  TableHeader   = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
  if (TableHeader->Signature == EFI_ACPI_2_0_PMTT_TABLE_SIGNATURE) { 
    if ((mSystemMemoryMap == NULL) || (mSystemMemoryMap->AepDimmPresent) || (mSystemConfiguration.Pmtt == FALSE)) 
      return EFI_NOT_FOUND; 
  } 

#ifdef BDAT_SUPPORT
  if (TableHeader->Signature == EFI_BDAT_TABLE_SIGNATURE) {
    if (mSocketMemoryConfiguration.bdatEn == 0)
      return EFI_NOT_FOUND;
  }
#endif
  
  // If No AepDimmPresent then we shouldn't publish Nfit while Pcat is to be published regardless
//  if (TableHeader->Signature == NVDIMM_FW_INTERFACE_TABLE_SIGNATURE) {
//    if (mSystemMemoryMap->AepDimmPresent == 0)
//      return EFI_NOT_FOUND;
//  }

  if (TableHeader->Signature  ==  EFI_ACPI_2_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE) {
    //
    // Initialize the SPCR table pointer.
    // SPCR table is not ready yet, update it before booting.
    //
    mSpcrTable = (EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE *) Table;
    return EFI_NOT_READY;
  }

#if SMCPKG_SUPPORT && HPET_SUPPORT
  if (TableHeader->Signature == EFI_ACPI_3_0_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE) {
    //
    // Only publishes HPET table, if Hpet is enabled in Setup.
    //
    if (!mSystemConfiguration.Hpet) {
      //
      // Disables the HPET decode.
      //
      MmioAnd8 ((P2sbBase + R_PCH_P2SB_HPTC), (UINT8)~B_PCH_P2SB_HPTC_AE);
      return EFI_NOT_FOUND;
    }
  }
#endif

  if ((TableHeader->Signature == EFI_ACPI_3_0_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE) ||
      (TableHeader->Signature == EFI_ACPI_4_0_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE_SIGNATURE) ||
      (TableHeader->Signature == EFI_ACPI_4_0_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE)) {
    // Only publish SRAT/SLIT/MSCT if systememorymap available and Numa is enabled in Setup.
    if ((mSystemMemoryMap == NULL) || (mSocketCommonRcConfiguration.NumaEn == 0))
      return EFI_NOT_FOUND;
  }

  if (TableHeader->Signature == EFI_ACPI_5_1_DEBUG_PORT_2_TABLE_SIGNATURE) {
      return EFI_NOT_FOUND;
  }

  if (TableHeader->Signature == EFI_ACPI_5_1_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_SIGNATURE) {
      return EFI_NOT_FOUND;
  }

  if (TableHeader->Signature == EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    Status = mSystemBoard->UpdateSystemAcpiTable (TableHeader, &mAmlOffsetTablePointer );
    return Status;

  }

  return EFI_SUCCESS;
}

/**

  This function will update any runtime platform specific information.
  This currently includes:
    Setting OEM table values, ID, table ID, creator ID and creator revision.
    Enabling the proper processor entries in the APIC tables.

  @param Table  -  The table to update

  @retval EFI_SUCCESS  -  The function completed successfully.

**/
EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  ,IN OUT EFI_ACPI_TABLE_VERSION     *Version
  )
{
  EFI_ACPI_DESCRIPTION_HEADER               *TableHeader;
  EFI_STATUS                                Status;
  ACPI_APIC_STRUCTURE_PTR                   *ProcessorLocalApicEntry;
  UINT64                                    TempOemTableId;
  UINT32                                    Data32;

  TableHeader             = NULL;

  ProcessorLocalApicEntry = NULL;
  Status = EFI_SUCCESS;

  //
  // By default, a table belongs in all ACPI table versions published.
  // Some tables will override this because they have different versions of the table.
  //
  *Version = EFI_ACPI_TABLE_VERSION_2_0;

  if (Table->Signature != EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE &&
      Table->Signature != EFI_ACPI_2_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE && 
      Table->Signature != SIGNATURE_32('N', 'F', 'I', 'T') && 
      Table->Signature != SIGNATURE_32('P', 'C', 'A', 'T') &&
      Table->Signature != SIGNATURE_32('O', 'E', 'M', '1') &&
      Table->Signature != SIGNATURE_32('O', 'E', 'M', '2') && 
      Table->Signature != SIGNATURE_32('O', 'E', 'M', '3') && 
      Table->Signature != SIGNATURE_32('O', 'E', 'M', '4')) 
  {
    TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
    //
    // Update the OEMID and OEM Table ID.
    //
    TempOemTableId = PcdGet64(PcdAcpiDefaultOemTableId);

    CopyMem (TableHeader->OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(TableHeader->OemId));
    CopyMem (&TableHeader->OemTableId, &TempOemTableId, sizeof(TableHeader->OemTableId));

    //
    // Update the creator ID
    //
    TableHeader->CreatorId = EFI_ACPI_CREATOR_ID;

    //
    // Update the creator revision
    //
    TableHeader->CreatorRevision = EFI_ACPI_CREATOR_REVISION;
  }
  //
  // Complete this function
  //

  //ASSERT (mMaxNumberOfCPUs <= MAX_CPU_NUM && mNumberOfEnabledCPUs >= 1);

  //
  // Assign a invalid intial value for update
  //
  //
  // Update the processors in the APIC table
  //
  switch (Table->Signature) {

  case EFI_ACPI_4_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchMadtTable(Table);
    break;

  case EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchFadtTable(Table);
    break;

  case EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    //
    // Patch the memory resource
    //
    Status = PatchDsdtTable(Table);
    break;

  case EFI_ACPI_3_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchSsdtTable(
                      Table
                      ,Version
    );
    break;
  case EFI_ACPI_3_0_PCCT_SIGNATURE:
    //
    // Expand PCCT table to update RAST support
    //
    PatchPlatformCommunicationsChannelTable(Table);
    break;

  case EFI_ACPI_3_0_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE:
    //
    // Adjust HPET Table to correct the Base Address
    // Get the address bits in RCRB that configure HPET MMIO space
    //  and create an offset to the pre-defined HEPT base address
    //
    PchHpetBaseGet(&Data32);
    //
    // Add the offset to the base address and copy into HPET DSDT table
    //
    ((EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER *) Table)->BaseAddressLower32Bit.Address = Data32;
    break;

  case EFI_ACPI_3_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchMcfgAcpiTable(Table);
    break;

  case EFI_ACPI_2_0_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE:
    Status = PatchSLitTable(Table);
    break;

  case EFI_ACPI_4_0_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE:
    Status = PatchSratTable(Table);
    break;

  case EFI_ACPI_2_0_PMTT_TABLE_SIGNATURE: 
    Status = PatchPlatformMemoryTopologyTable(Table); 
    break; 

  case EFI_ACPI_4_0_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE_SIGNATURE:
    Status = PatchMsctAcpiTable(Table);
    break;

  case EFI_ACPI_2_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE:
      PatchSpcrAcpiTable(Table);
      break;

  case EFI_SVOS_ACPI_TABLE_SIGNATURE:
    if (PcdGetBool(PcdDfxAdvDebugJumper)) {
      Status = PatchSvosAcpiTable(Table);
    }
    break;

  case EFI_MIGT_ACPI_TABLE_SIGNATURE:
    if (PcdGetBool(PcdDfxAdvDebugJumper)) {
      Status = PatchMigtAcpiTable(Table);
    }
    break;

  case NVDIMM_FW_INTERFACE_TABLE_SIGNATURE:
    Status = UpdateNfitTable(Table);
    break;
  case NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_SIGNATURE:
    Status = UpdatePcatTable(Table);
    break;

  case ACPI_WSMT_SIGNATURE:
    (((ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_TABLE *)Table)->ProtectionFlags.Flags) = (UINT32 ) (WSMT_PROTECTION_FLAG & PcdGet32(PcdWsmtProtectionFlags));
    break;

#ifdef BDAT_SUPPORT
  case EFI_BDAT_TABLE_SIGNATURE:
    Status = PatchBdatAcpiTable(Table);
    break;
#endif

//Patch Dynamic OEM SSDT table
  case OEM1_SSDT_TABLE_SIGNATURE:
      Status = PatchOem1SsdtTable(Table);  //CPU EIST
    break;

  case OEM2_SSDT_TABLE_SIGNATURE:
      Status = PatchOem2SsdtTable(Table);  //CPU HWP
    break;

  case OEM3_SSDT_TABLE_SIGNATURE:
      Status = PatchOem3SsdtTable(Table);  //CPU TST
    break;

  case OEM4_SSDT_TABLE_SIGNATURE:
      Status = PatchOem4SsdtTable(Table);  //CPU CST
    break;

  default:
    break;
  }
  //
  //
  // Update the hardware signature in the FACS structure
  //
  //
  //
  return Status;
}
