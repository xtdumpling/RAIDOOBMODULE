/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Code to log processor subclass data with Smbios protocol.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "Cpu.h"
#include "Processor.h"

UINTN mCpuSocketStrNumber = 1;
UINTN mCpuAssetTagStrNumber = 4;

/**
  Add Type 4 SMBIOS Record for Socket Unpopulated.
**/
VOID
AddUnpopulatedSmbiosProcessorTypeTable (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_SMBIOS_HANDLE     SmbiosHandle;
  UINTN                 TotalSize;
  SMBIOS_TABLE_TYPE4    *SmbiosRecord;
  CHAR8                 *OptionalStrStart;
  EFI_STRING_ID         Token;
  EFI_STRING            CpuSocketStr;
  UINTN                 CpuSocketStrLen;
  UINT32                FamilyId;
  UINT32                ModelId;

  //
  // Get CPU Socket string, it will be updated when PcdPlatformCpuSocketNames is set.
  //
  Token = STRING_TOKEN (STR_UNKNOWN);
  CpuSocketStr = HiiGetPackageString (&gEfiCallerIdGuid, Token ,NULL);
  ASSERT (CpuSocketStr != NULL);
  CpuSocketStrLen = StrLen (CpuSocketStr);
  ASSERT (CpuSocketStrLen <= SMBIOS_STRING_MAX_LENGTH);

  //
  // Report Processor Information to Type 4 SMBIOS Record.
  //

  TotalSize = sizeof (SMBIOS_TABLE_TYPE4) + CpuSocketStrLen + 1 + 1;
  SmbiosRecord = AllocatePool (TotalSize);
  ASSERT (SmbiosRecord != NULL);
  ZeroMem (SmbiosRecord, TotalSize);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosRecord->Hdr.Length = (UINT8) sizeof (SMBIOS_TABLE_TYPE4);
  //
  // Make handle chosen by smbios protocol.add automatically.
  //
  SmbiosRecord->Hdr.Handle = 0;
  //
  // Socket will be the 1st optional string following the formatted structure.
  //
  SmbiosRecord->Socket = (UINT8) mCpuSocketStrNumber;
  SmbiosRecord->ProcessorType = CentralProcessor;

  //
  // Just indicate CPU Socket Unpopulated.
  // CPU_PROCESSOR_STATUS_DATA.SocketPopulated: 1- CPU Socket populated and 0 - CPU Socket Unpopulated
  //
//
// PURLEY_OVERRIDE_BEGIN
//
  SmbiosRecord->Status &= ~SMBIOS_TYPE4_CPU_SOCKET_POPULATED;
//
// PURLEY_OVERRIDE_END
//

  SmbiosRecord->MaxSpeed = (UINT16) PcdGet32 (PcdPlatformCpuMaxCoreFrequency);

  GetProcessorVersionInfo (0, &FamilyId, &ModelId, NULL, NULL);

  if ((FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_SERVER_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_SERVER_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_SERVER_PROC (FamilyId, ModelId))
      ) {
    //
    // The upgrade depends on processor family id and model id.
    //
// APTIOV_SERVER_OVERRIDE_RC_START
//    SmbiosRecord->ProcessorUpgrade = ProcessorUpgradeSocketLGA2011_3;
    SmbiosRecord->ProcessorUpgrade = ProcessorUpgradeOther;
// APTIOV_SERVER_OVERRIDE_RC_END
  } else if ((FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))) {
    SmbiosRecord->ProcessorUpgrade = ProcessorUpgradeOther;
  } else {
    SmbiosRecord->ProcessorUpgrade = ProcessorUpgradeSocketLGA775; 
  }

  SmbiosRecord->L1CacheHandle = 0xFFFF;
  SmbiosRecord->L2CacheHandle = 0xFFFF;
  SmbiosRecord->L3CacheHandle = 0xFFFF;

  OptionalStrStart = (CHAR8 *) (SmbiosRecord + 1);
  UnicodeStrToAsciiStr (CpuSocketStr, OptionalStrStart);
  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mSmbios->Add (mSmbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);
  FreePool (SmbiosRecord);
  FreePool (CpuSocketStr);
  ASSERT_EFI_ERROR (Status);
}

/**
  Add Processor Information to Type 4 SMBIOS Record for Socket Populated.

  @param[in]    ProcessorNumber     Processor number of specified processor.
  @param[in]    L1CacheHandle       The handle of the L1 Cache SMBIOS record.
  @param[in]    L2CacheHandle       The handle of the L2 Cache SMBIOS record.
  @param[in]    L3CacheHandle       The handle of the L3 Cache SMBIOS record.

**/
VOID
AddSmbiosProcessorTypeTable (
  IN UINTN              ProcessorNumber,
  IN EFI_SMBIOS_HANDLE  L1CacheHandle,
  IN EFI_SMBIOS_HANDLE  L2CacheHandle,
  IN EFI_SMBIOS_HANDLE  L3CacheHandle
  )
{
  EFI_STATUS                    Status;
  EFI_SMBIOS_HANDLE             SmbiosHandle;
  UINTN                         TotalSize;
  EFI_STRING_ID                 Token;
  CHAR8                         *OptionalStrStart;
  EFI_STRING                    CpuManuStr;
  EFI_STRING                    CpuVerStr;
  EFI_STRING                    CpuSocketStr;
  EFI_STRING                    CpuAssetTagStr;
  UINTN                         CpuManuStrLen;
  UINTN                         CpuVerStrLen;
  UINTN                         CpuSocketStrLen;
  UINTN                         CpuAssetTagStrLen;
  SMBIOS_TABLE_TYPE4            *SmbiosRecord;
  EFI_CPUID_REGISTER            *CpuidRegister;
  UINT16                        ProcessorVoltage;
  CPU_PROCESSOR_VERSION_INFORMATION     Version;
  CPU_PROCESSOR_STATUS_DATA     ProcessorStatus;
  CPU_PROCESSOR_CHARACTERISTICS_DATA    ProcessorCharacteristics;
  UINT16                        PackageThreadCount;
  UINT16                        CoreThreadCount;
  UINT16                        CoreCount;
  UINT32                        FamilyId;
  UINT32                        ModelId;
  UINT64                        MsrValue;

  CoreCount = 0;
  PackageThreadCount = 0;

  //
  // Get CPU Socket string, it will be updated when PcdPlatformCpuSocketNames is set.
  //
  Token = STRING_TOKEN (STR_UNKNOWN);
  CpuSocketStr = HiiGetPackageString (&gEfiCallerIdGuid, Token ,NULL);
  ASSERT (CpuSocketStr != NULL);
  CpuSocketStrLen = StrLen (CpuSocketStr);
  ASSERT (CpuSocketStrLen <= SMBIOS_STRING_MAX_LENGTH);

  //
  // Get CPU Manufacture string.
  //
  Token = GetProcessorManufacturer (ProcessorNumber);
  CpuManuStr = HiiGetPackageString (&gEfiCallerIdGuid, Token, NULL);
  ASSERT (CpuManuStr != NULL);
  CpuManuStrLen = StrLen (CpuManuStr);
  ASSERT (CpuManuStrLen <= SMBIOS_STRING_MAX_LENGTH);

  //
  // Get CPU Version string.
  //
  GetProcessorVersion (ProcessorNumber, &Version);
  if (Version.StringValid) {
    Token = HiiSetString (mStringHandle, 0, Version.BrandString, NULL);
    if (Token == 0) {
      Token = Version.StringRef;
    }
  } else {
    Token = Version.StringRef;
  }
  CpuVerStr = HiiGetPackageString (&gEfiCallerIdGuid, Token, NULL);
  ASSERT (CpuVerStr != NULL);
  CpuVerStrLen = StrLen (CpuVerStr);
  ASSERT (CpuVerStrLen <= SMBIOS_STRING_MAX_LENGTH);

  //
  // Get CPU Asset Tag string, it will be updated when PcdPlatformCpuAssetTags is set.
  //
  Token = STRING_TOKEN (STR_UNKNOWN);
  CpuAssetTagStr = HiiGetPackageString (&gEfiCallerIdGuid, Token ,NULL);
  ASSERT (CpuAssetTagStr != NULL);
  CpuAssetTagStrLen = StrLen (CpuAssetTagStr);
  ASSERT (CpuAssetTagStrLen <= SMBIOS_STRING_MAX_LENGTH);

  //
  // Get CPU core count.
  //
  if (GetNumberOfCpuidLeafs (ProcessorNumber, BasicCpuidLeaf) > EFI_CPUID_CORE_TOPOLOGY) {
    CpuidRegister = GetExtendedTopologyEnumerationCpuidLeafs (ProcessorNumber, 1);
    PackageThreadCount = (UINT16) (CpuidRegister->RegEbx);
    CpuidRegister = GetExtendedTopologyEnumerationCpuidLeafs (ProcessorNumber, 0);
    CoreThreadCount = (UINT16) (CpuidRegister->RegEbx);
    CoreCount = PackageThreadCount / CoreThreadCount;
  }

  //
  // Report Processor Information to Type 4 SMBIOS Record.
  //

  TotalSize = sizeof (SMBIOS_TABLE_TYPE4) + CpuSocketStrLen + 1 + CpuManuStrLen + 1 + CpuVerStrLen + 1 + CpuAssetTagStrLen + 1 + 1;
  SmbiosRecord = AllocatePool (TotalSize);
  ASSERT (SmbiosRecord != NULL);
  ZeroMem (SmbiosRecord, TotalSize);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosRecord->Hdr.Length = (UINT8) sizeof (SMBIOS_TABLE_TYPE4);
  //
  // Make handle chosen by smbios protocol.add automatically.
  //
  SmbiosRecord->Hdr.Handle = 0;
  //
  // Socket will be the 1st optional string following the formatted structure.
  //
  SmbiosRecord->Socket = (UINT8) mCpuSocketStrNumber;
  SmbiosRecord->ProcessorType = CentralProcessor;
  SmbiosRecord->ProcessorFamily2 = (UINT8) GetProcessorFamily (ProcessorNumber); 
  if (SmbiosRecord->ProcessorFamily2 > ProcessorFamilyIndicatorFamily2) {
    SmbiosRecord->ProcessorFamily = ProcessorFamilyIndicatorFamily2;
  } else {
    SmbiosRecord->ProcessorFamily = (UINT8) SmbiosRecord->ProcessorFamily2;
  }
  
  //
  // Manu will be the 2nd optional string following the formatted structure.
  //
  SmbiosRecord->ProcessorManufacture = 2;   

  CpuidRegister = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegister != NULL);
  *(UINT32 *) &SmbiosRecord->ProcessorId.Signature = CpuidRegister->RegEax;
  *(UINT32 *) &SmbiosRecord->ProcessorId.FeatureFlags = CpuidRegister->RegEdx,

  //
  // Version will be the 3rd optional string following the formatted structure.
  // 
  SmbiosRecord->ProcessorVersion = 3;   

  ProcessorVoltage = GetProcessorVoltage (ProcessorNumber); // mV unit
  ProcessorVoltage = (UINT16) ((ProcessorVoltage * 10) / 1000);
  *(UINT8 *) &SmbiosRecord->Voltage = (UINT8) ProcessorVoltage;
  SmbiosRecord->Voltage.ProcessorVoltageIndicateLegacy = 1;

  SmbiosRecord->ExternalClock = (UINT16) (GET_CPU_MISC_DATA (ProcessorNumber, IntendedFsbFrequency));
  SmbiosRecord->MaxSpeed = (UINT16) PcdGet32 (PcdPlatformCpuMaxCoreFrequency);
  SmbiosRecord->CurrentSpeed = (UINT16) (GET_CPU_MISC_DATA (ProcessorNumber, IntendedFsbFrequency) * GET_CPU_MISC_DATA (ProcessorNumber, MaxCoreToBusRatio));

//
// PURLEY_OVERRIDE_BEGIN
//
  //
  // Just indicate CPU Socket populated.
  // CPU_PROCESSOR_STATUS_DATA.SocketPopulated: BIT6 - CPU Socket populated and 0 - CPU Socket Unpopulated
  //
  SmbiosRecord->Status |= SMBIOS_TYPE4_CPU_SOCKET_POPULATED;
//
// PURLEY_OVERRIDE_END
//

  ProcessorStatus.CpuStatus = 1;        // CPU Enabled
  ProcessorStatus.Reserved1 = 0;
  ProcessorStatus.SocketPopulated = 1;  // CPU Socket Populated
  ProcessorStatus.Reserved2 = 0;
  CopyMem (&SmbiosRecord->Status, &ProcessorStatus, 1);

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

  if ((FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_SERVER_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_SERVER_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_SERVER_PROC (FamilyId, ModelId))
      ) {
    //
    // The upgrade depends on processor family id and model id.
    //
// APTIOV_SERVER_OVERRIDE_RC_START
//    SmbiosRecord->ProcessorUpgrade = ProcessorUpgradeSocketLGA2011_3;
    SmbiosRecord->ProcessorUpgrade = ProcessorUpgradeOther;
// APTIOV_SERVER_OVERRIDE_RC_END
  } else if ((FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))) {
    SmbiosRecord->ProcessorUpgrade = ProcessorUpgradeOther;
  } else {
    SmbiosRecord->ProcessorUpgrade = ProcessorUpgradeSocketLGA775; 
  }

  SmbiosRecord->L1CacheHandle = L1CacheHandle;
  SmbiosRecord->L2CacheHandle = L2CacheHandle;
  SmbiosRecord->L3CacheHandle = L3CacheHandle;

  //
  // AssetTag will be the 4th optional string following the formatted structure.
  //
  SmbiosRecord->AssetTag = (UINT8) mCpuAssetTagStrNumber;

  SmbiosRecord->CoreCount2 = CoreCount;
  SmbiosRecord->CoreCount = (CoreCount > 255) ? 0xFF : (UINT8) (CoreCount);
  
  if ((FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))
      ) {
    //
    // This MSR register depends on processor family id and model id.
    //
    MsrValue = GET_CPU_MISC_DATA (ProcessorNumber, EnabledThreadCountMsr); // PURLEY_OVERRIDE_5371439
    SmbiosRecord->EnabledCoreCount2 = (UINT16)(RShiftU64 (MsrValue, 16) & 0xFFFF);
    SmbiosRecord->ThreadCount2 = (UINT16)(MsrValue & 0xFFFF);
  } else {
    SmbiosRecord->EnabledCoreCount2 = CoreCount;
    SmbiosRecord->ThreadCount2 = PackageThreadCount;
  }
  
  SmbiosRecord->EnabledCoreCount = (SmbiosRecord->EnabledCoreCount2 > 255) ? 0xFF : (UINT8) (SmbiosRecord->EnabledCoreCount2);
  SmbiosRecord->ThreadCount = (SmbiosRecord->ThreadCount2 > 255) ? 0xFF : (UINT8) (SmbiosRecord->ThreadCount2);    

  ProcessorCharacteristics.Reserved = 0;
  ProcessorCharacteristics.Capable64Bit = 1; // 64-bit Capable
  ProcessorCharacteristics.Unknown = 0;
  ProcessorCharacteristics.MultiCore = 1;
  ProcessorCharacteristics.HardwareThread = 1;
  ProcessorCharacteristics.EnhancedVirtualization = 0;
  if (BitFieldRead32 (CpuidRegister->RegEcx, N_CPUID_VMX_SUPPORT, N_CPUID_VMX_SUPPORT) == 1) {
    ProcessorCharacteristics.EnhancedVirtualization = 1;
  }
  ProcessorCharacteristics.ExecuteProtection = 0;
  if (GetNumberOfCpuidLeafs (ProcessorNumber, ExtendedCpuidLeaf) > 2) {
    //
    // Check CPUID(0x80000001).EDX[20]
    //
    CpuidRegister = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_EXTENDED_CPU_SIG);
    ASSERT (CpuidRegister != NULL);

    if (BitFieldRead32 (CpuidRegister->RegEdx, N_CPUID_XD_BIT_AVAILABLE, N_CPUID_XD_BIT_AVAILABLE) == 1) {
      ProcessorCharacteristics.ExecuteProtection = 1;
    }
  }
  ProcessorCharacteristics.PowerPerformanceControl = 1;
  ProcessorCharacteristics.Reserved2 = 0;
  CopyMem (&SmbiosRecord->ProcessorCharacteristics, &ProcessorCharacteristics, 2);

  OptionalStrStart = (CHAR8 *) (SmbiosRecord + 1);
  UnicodeStrToAsciiStr (CpuSocketStr, OptionalStrStart);
  UnicodeStrToAsciiStr (CpuManuStr, OptionalStrStart + CpuSocketStrLen + 1);
  UnicodeStrToAsciiStr (CpuVerStr, OptionalStrStart + CpuSocketStrLen + 1 + CpuManuStrLen + 1);
  UnicodeStrToAsciiStr (CpuAssetTagStr, OptionalStrStart + CpuSocketStrLen + 1 + CpuManuStrLen + 1 + CpuVerStrLen + 1);
  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mSmbios->Add (mSmbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);
  FreePool (SmbiosRecord);
  FreePool (CpuSocketStr);
  FreePool (CpuManuStr);
  FreePool (CpuVerStr);
  FreePool (CpuAssetTagStr);
  ASSERT_EFI_ERROR (Status);
}

/**
  Get Type 4 SMBIOS Record table.

  @param[in, out]   SmbiosHandle    On entry, points to the previous handle of the SMBIOS record. On exit, points to the
                                    next SMBIOS record handle. If it is zero on entry, then the first SMBIOS record
                                    handle will be returned. If it returns zero on exit, then there are no more SMBIOS records.
  @param[out]       Record          Returned pointer to record buffer .

**/
VOID
GetSmbiosProcessorTypeTable (
  IN OUT EFI_SMBIOS_HANDLE      *SmbiosHandle,
  OUT EFI_SMBIOS_TABLE_HEADER   *Record OPTIONAL
  )
{
  EFI_STATUS                 Status;
  EFI_SMBIOS_TYPE            RecordType;
  EFI_SMBIOS_TABLE_HEADER    *Buffer;

  RecordType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  do {
    Status = mSmbios->GetNext (
                        mSmbios,
                        SmbiosHandle,
                        &RecordType,
                        &Buffer, 
                        NULL
                        );
    if (!EFI_ERROR(Status)) {
      if (Record != NULL) {
        Record = Buffer;
      }
      return;
    }
  } while (!EFI_ERROR(Status));

}

/**
  Notification function when PcdPlatformCpuSocketCount is set.

**/
VOID
CallbackOnPcdPlatformCpuSocketCount (
  VOID
  )
{
  UINT32 CpuSocketCount;
  UINT32 SocketIndex;

  CpuSocketCount = PcdGet32 (PcdPlatformCpuSocketCount);
  if (CpuSocketCount <= mPopulatedSocketCount) {
    return;
  }

  //
  // Add Type 4 SMBIOS Record for Socket Unpopulated.
  //
  for (SocketIndex = mPopulatedSocketCount; SocketIndex < CpuSocketCount; SocketIndex++) {
    AddUnpopulatedSmbiosProcessorTypeTable ();
  }

}

/**
  Notification function when PcdPlatformCpuSocketNames is set.

**/
VOID
CallbackOnPcdPlatformCpuSocketNames (
  VOID
  )
{
  CHAR16            **CpuSocketNames;
  CHAR8             *CpuSocketStr;
  UINTN             CpuSocketStrLen;
  UINT32            SocketIndex;
  EFI_SMBIOS_HANDLE SmbiosHandle;
  UINT32            CpuSocketCount;

  CpuSocketNames = (CHAR16 **) (UINTN) (PcdGet64 (PcdPlatformCpuSocketNames));

  if (CpuSocketNames == NULL) {
    return;
  }

  CpuSocketCount = PcdGet32 (PcdPlatformCpuSocketCount);
  if (CpuSocketCount < mPopulatedSocketCount) {
    CpuSocketCount = mPopulatedSocketCount;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SocketIndex = 0;

  //
  // Update CPU Socket string for Socket Populated.
  //
  do {
    GetSmbiosProcessorTypeTable (&SmbiosHandle, NULL);
    if (SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED) {
      return;
    }
    CpuSocketStrLen = StrLen (CpuSocketNames[SocketIndex]);
    ASSERT (CpuSocketStrLen <= SMBIOS_STRING_MAX_LENGTH);
    CpuSocketStr = AllocatePool (CpuSocketStrLen + 1);
    UnicodeStrToAsciiStr (CpuSocketNames[SocketIndex], CpuSocketStr);
    mSmbios->UpdateString (mSmbios, &SmbiosHandle, &mCpuSocketStrNumber, CpuSocketStr);
    FreePool (CpuSocketStr);
  } while ((++SocketIndex) < CpuSocketCount);

}

/**
  Notification function when PcdPlatformCpuAssetTags is set.

**/
VOID
CallbackOnPcdPlatformCpuAssetTags (
  VOID
  )
{
  CHAR16            **CpuAssetTags;
  CHAR8             *CpuAssetTagStr;
  UINTN             CpuAssetTagStrLen;
  UINT32            SocketIndex;
  EFI_SMBIOS_HANDLE SmbiosHandle;

  CpuAssetTags = (CHAR16 **) (UINTN) (PcdGet64 (PcdPlatformCpuAssetTags));
  if (CpuAssetTags == NULL) {
    return;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SocketIndex = 0;

  //
  // Update CPU Asset Tag string for Socket Populated.
  //
  do {
    GetSmbiosProcessorTypeTable (&SmbiosHandle, NULL);
    if (SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED) {
      return;
    }
    CpuAssetTagStrLen = StrLen (CpuAssetTags[SocketIndex]);
    ASSERT (CpuAssetTagStrLen <= SMBIOS_STRING_MAX_LENGTH);
    CpuAssetTagStr = AllocatePool (CpuAssetTagStrLen + 1);
    UnicodeStrToAsciiStr (CpuAssetTags[SocketIndex], CpuAssetTagStr);
    mSmbios->UpdateString (mSmbios, &SmbiosHandle, &mCpuAssetTagStrNumber, CpuAssetTagStr);
    FreePool (CpuAssetTagStr);
  } while ((++SocketIndex) < mPopulatedSocketCount);

}

/**
  Register notification functions for Pcds related to Smbios Processor Type.
**/
VOID
SmbiosProcessorTypeTableCallback (
  VOID
  )
{
  //
  // PcdPlatformCpuSocketCount, PcdPlatformCpuSocketNames and PcdPlatformCpuAssetTags
  // should be already set by platform before
  //
  CallbackOnPcdPlatformCpuSocketCount ();
  CallbackOnPcdPlatformCpuSocketNames ();
  CallbackOnPcdPlatformCpuAssetTags ();
}

