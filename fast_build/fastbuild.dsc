[Defines]
  PLATFORM_NAME                  = SmcOOB
  PLATFORM_GUID                  = 0458dade-8b6e-4e45-b773-1b27cbda3e06
  PLATFORM_VERSION               = 1
  DSC_SPECIFICATION              = 0x00010006
  OUTPUT_DIRECTORY               = Build\SmcOOB
  SUPPORTED_ARCHITECTURES        = X64
  BUILD_TARGETS                  = RELEASE|DEBUG
  SKUID_IDENTIFIER               = DEFAULT


[LibraryClasses]
  AmiDxeLib|AmiCompatibilityPkg/Library/AmiDxeLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  OpensslLib|Build/OpensslLib.inf
  OobLibrary|SmcPkg/Module/SmcOOB/Library/OobLibrary.inf
  SmcLsiRaidOOBLib|SmcPkg/Module/SmcOOB/SmcLsiRaidOOB/SmcLsiRaidOOBLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  OpensslTlsLib|CryptoPkg/Library/OpensslLib/openssl/OpensslTlsLib.inf
  PostCodeLib|MdePkg/Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf

[LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.DXE_RUNTIME_DRIVER,LibraryClasses.common.DXE_SAL_DRIVER,LibraryClasses.common.DXE_SMM_DRIVER,LibraryClasses.common.UEFI_APPLICATION,LibraryClasses.common.UEFI_DRIVER]
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf

[LibraryClasses.IA32.DXE_SMM_DRIVER,LibraryClasses.IA32.SMM_CORE,LibraryClasses.X64.DXE_SMM_DRIVER,LibraryClasses.X64.SMM_CORE,LibraryClasses.IPF.DXE_SMM_DRIVER,LibraryClasses.IPF.SMM_CORE,LibraryClasses.ARM.DXE_SMM_DRIVER,LibraryClasses.ARM.SMM_CORE]
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf

[LibraryClasses.IA32.DXE_RUNTIME_DRIVER,LibraryClasses.X64.DXE_RUNTIME_DRIVER,LibraryClasses.IPF.DXE_RUNTIME_DRIVER,LibraryClasses.ARM.DXE_RUNTIME_DRIVER]
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf

[LibraryClasses.IA32.DXE_DRIVER,LibraryClasses.IA32.DXE_CORE,LibraryClasses.IA32.UEFI_APPLICATION,LibraryClasses.IA32.UEFI_DRIVER,LibraryClasses.X64.DXE_DRIVER,LibraryClasses.X64.DXE_CORE,LibraryClasses.X64.UEFI_APPLICATION,LibraryClasses.X64.UEFI_DRIVER,LibraryClasses.IPF.DXE_DRIVER,LibraryClasses.IPF.DXE_CORE,LibraryClasses.IPF.UEFI_APPLICATION,LibraryClasses.IPF.UEFI_DRIVER,LibraryClasses.ARM.DXE_DRIVER,LibraryClasses.ARM.DXE_CORE,LibraryClasses.ARM.UEFI_APPLICATION,LibraryClasses.ARM.UEFI_DRIVER,LibraryClasses.AARCH64.DXE_DRIVER,LibraryClasses.AARCH64.DXE_CORE,LibraryClasses.AARCH64.UEFI_APPLICATION,LibraryClasses.AARCH64.UEFI_DRIVER]
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf

[LibraryClasses.common.SMM_CORE]
  AmiBufferValidationLib|AmiModulePkg/Library/SmmCoreAmiBufferValidationLib/SmmCoreAmiBufferValidationLib.inf
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf
  DebugLib|AmiModulePkg/Library/SmmCoreAmiDebugLib/SmmCoreAmiDebugLib.inf
  PerformanceLib|MdeModulePkg/Library/SmmCorePerformanceLib/SmmCorePerformanceLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  AmiBufferValidationLib|AmiModulePkg/Library/SmmAmiBufferValidationLib/SmmAmiBufferValidationLib.inf

[LibraryClasses.X64.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf

[LibraryClasses.common.DXE_CORE,LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.DXE_RUNTIME_DRIVER,LibraryClasses.common.DXE_SAL_DRIVER,LibraryClasses.common.DXE_SMM_DRIVER,LibraryClasses.common.SMM_CORE,LibraryClasses.common.UEFI_APPLICATION,LibraryClasses.common.UEFI_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf

[LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.DXE_RUNTIME_DRIVER,LibraryClasses.common.SMM_CORE,LibraryClasses.common.DXE_SMM_DRIVER,LibraryClasses.common.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf

[LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.DXE_RUNTIME_DRIVER,LibraryClasses.common.DXE_SAL_DRIVER,LibraryClasses.common.UEFI_APPLICATION,LibraryClasses.common.UEFI_DRIVER]
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf

[LibraryClasses.common.PEIM,LibraryClasses.common.PEI_CORE,LibraryClasses.common.SEC]
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf

[LibraryClasses.common.DXE_CORE]
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  DebugLib|AmiModulePkg/Library/DxeCoreAmiDebugLib/DxeCoreAmiDebugLib.inf
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf

[LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.DXE_RUNTIME_DRIVER,LibraryClasses.common.DXE_SAL_DRIVER,LibraryClasses.common.SMM_CORE,LibraryClasses.common.DXE_SMM_DRIVER,LibraryClasses.common.UEFI_APPLICATION,LibraryClasses.common.UEFI_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf

[LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.UEFI_APPLICATION,LibraryClasses.common.UEFI_DRIVER]
  DebugLib|AmiModulePkg/Library/DxeAmiDebugLib/DxeAmiDebugLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  DebugLib|AmiModulePkg/Library/RuntimeAmiDebugLib/RuntimeAmiDebugLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  DebugLib|AmiModulePkg/Library/SmmAmiDebugLib/SmmAmiDebugLib.inf
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/SmmPerformanceLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER,LibraryClasses.common.DXE_SAL_DRIVER]
  BaseCryptLib|CryptoPkg/Library/BaseCryptLibRuntimeCryptProtocol/BaseCryptLibRuntimeCryptProtocol.inf

[LibraryClasses.IA32,LibraryClasses.X64,LibraryClasses.IPF,LibraryClasses.AARCH64]
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  CrtWrapperLib|CryptoPkg/Library/BaseCryptLib/CrtWrapperLib.inf

[PcdsFixedAtBuild]
gEfiMdePkgTokenSpaceGuid.PcdPostCodePropertyMask|0x08
gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000006
gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F

[PcdsFixedAtBuild.common]
gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07

[PcdsPatchableInModule.common]
gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046

[Components.X64]
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf

  }
  SmcPkg/Module/SmcOOB/SmcInBand/SmcInBandDxe.inf
  SmcPkg/Module/SmcOOB/SmcInBand/SmcInBandSmm.inf
  SmcPkg/Module/SmcOOB/SmcOutBand/SmcOutBand.inf
  SmcPkg/Module/SmcOOB/SmcAssetInfo/SmcAssetInfo.inf
  SmcPkg/Module/SmcOOB/SmcLsiRaidOOB/SmcLsiRaidOOBSetup.inf {
    <LibraryClasses>
      NULL|SmcPkg/Module/SmcOOB/SmcLsiRaidOOB/SmcLsiRaidOOBLib.inf

  }
#  SmcPkg/Module/SmcOOB/Sample/SmcOobPlatformPolicy/SmcOobPlatformPolicyDxe.inf
#  SmcPkg/Module/SmcOOB/Sample/SmcOobPlatformPolicy/SmcOobPlatformPolicySmm.inf
