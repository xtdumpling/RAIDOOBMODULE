//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//  Rev. 1.09
//    Bug Fix : N/A
//    Reason  : Add Support SMC LSI OOB Module.
//    Auditor : Durant Lin
//    Date    : Sep/28/2018
//
//  Rev. 1.08
//    Bug Fix : N/A
//    Reason  : Move Variable_GUID to PlatformPolicy.
//    Auditor : Durant Lin
//    Date    : Aug/07/2018
//
//  Rev. 1.07
//    Bug Fix : N/A
//    Reason  : Hide Dynamic Page goto Advanced page. 
//    Auditor : Durant Lin
//    Date    : Jun/14/2018
//
//  Rev. 1.06
//    Bug Fix : N/A
//    Reason  : Isolate IPMI support for no IPMI module platform InBand. 
//    Auditor : Durant Lin
//    Date    : Feb/07/2018
//
//  Rev. 1.05
//    Bug Fix : N/A
//    Reason  : Create a signal guid for OOB use after BDS connect all driver,
//    			since original guid not all platform signaled.
//    Auditor : Durant Lin
//    Date    : Jan/04/2018
//
//  Rev. 1.04
//    Bug Fix : N/A
//    Reason  : Implement a SmcOobProtocol Interface for InBand and OutBand to
//              access Build time OobLibrary not OBJ OobLobrary. 
//    Auditor : Durant Lin
//    Date    : Dec/27/2017
//
//  Rev. 1.03
//    Bug Fix : N/A
//    Reason  : Updated SUM Feature Flag specification revision to 1.6 based 
//              on SUM_FeatureSupportFlagsSpecification_V1.6_rev20170930_1.docx 
//              for SMC OOB TPM provisioning via IPMI.
//    Auditor : Joe Jhang
//    Date    : Nov/07/2017
//
//  Rev. 1.02
//    Bug fixed: Update SmcPrepareDataToBmc function for new SMCBMCLIB function.
//    Reason   : 
//    Auditor  : Durant Lin
//    Date     : Dec/22/07
//
//  Rev. 1.01
//    Bug fixed: Hide "IPMI" and "Secure Boot" forms for SUM.
//    Reason   : 
//    Auditor  : Jason Hsu
//    Date     : Nov/16/07
//
//  Rev. 1.00
//    Bug fixed: Initial SMC_OOB_PLATFORM_POLICY for SMC OOB module binary release.
//    Reason   : 
//    Auditor  : Jason Hsu
//    Date     : Jun/09/07
//
//****************************************************************************

#include "SmcOobPlatformPolicyDxe.h"
#include <TOKEN.h>
#include <TimeStamp.h>
#include <SspTokens.h>
#include <ServerMgmtSetupVariable.h>
#include <SmcLibBmc.h>
#include <SmcRomHole.h>
#include "SmcFeatureFlag.h"
#include "AutoId.h"

SMC_OOB_PLATFORM_POLICY           *mSmcOobPlatformPolicy;
extern EFI_GUID                   gBdsAllDriversConnectedProtocolGuid;


SMC_IPMI_OEM_COMMAND_SET_PROTOCOL mSmcIpmiOemCommandSetProtocol = {
  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL_REVISION_1,
  0,
  SmcDownloadOobDataStart,
  SmcDownloadOobDataReady,
  SmcDownloadOobDataDone,
  SmcUploadOobDataStart,
  SmcUploadOobDataDone,
  SmcReadDataToBios,
  SmcPrepareDataToBmc,
  SmcGetSetOobIdentify,
  SmcSmibiosPreservation
};

SMC_ROM_HOLE_PROTOCOL             mSmcRomHoleProtocol = {
  SMC_ROM_HOLE_PROTOCOL_REVISION_1,
  0,
  RomHoleWriteRegion,
  RomHoleReadRegion,
  RomHoleSize,
  RomHoleOffset
};

SMC_ROOT_FORM_DATA RootForm[] =
{
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Main"       		 , 0},
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Advanced"   		 , 0},
  {{0x8BEB8C19, 0x3FEC, 0x4FAB, {0xA3, 0x78, 0xC9, 0x03, 0xE8, 0x90, 0xFC, 0xAE}}, 0, "Event Logs" 		 , 0},
//  {{0x01239999, 0xFC0E, 0x4B6E, {0x9E, 0x79, 0xD5, 0x4D, 0x5D, 0xB6, 0xCD, 0x20}}, 0, "IPMI"       , 0},
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Security"   		 , 0},
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Boot"       		 , 0},
  {{0xca113bd9, 0x21cc, 0xaa1b, {0x33, 0x58, 0xa9, 0x73, 0x78, 0xdd, 0x3c, 0x5e}}, 0, "SMC LSI OOB Form" , 0},
//{{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Save & Exit", 0}
};

SMC_ROOT_FORM_DATA HideFormForSum[] =
{
//Formset GUID,                                                                    0, Form Title,              0
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Secure Boot"          , 0},
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Add New Boot Option"  , 0},
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Delete Boot Option"   , 0},
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Add New Driver Option", 0},
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, 0, "Delete Driver Option" , 0},
//Dynmic pages tag, patch SUM wrong behavior.
  {{0x7B59104A, 0xC00D, 0x4158, {0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}}, ADVANCED_MAIN, ""   , 0}
};

//For Update Bios Cfg Use, specific to outsite Platform Var.
Variable_GUID VarGUID[] = { //"Setup" must be first, can't change it.
    {L"Setup", 							{0xec87d643, 0xeba4, 0x4bb5, 0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0x0d, 0xa9}},
    {L"ServerSetup", 					{0x1239999,  0xfc0e, 0x4b6e, 0x9e, 0x79, 0xd5, 0x4d, 0x5d, 0xb6, 0xcd, 0x20}},
    {L"PNP0501_0_NV", 					{0x560bf58a, 0x1e0d, 0x4d7e, 0x95, 0x3f, 0x29, 0x80, 0xa2, 0x61, 0xe0, 0x31}},
    {L"PNP0501_1_NV", 					{0x560bf58a, 0x1e0d, 0x4d7e, 0x95, 0x3f, 0x29, 0x80, 0xa2, 0x61, 0xe0, 0x31}},
    {L"FixedBoot", 						{0xde8ab926, 0xefda, 0x4c23, 0xbb, 0xc4, 0x98, 0xfd, 0x29, 0xaa, 0x00, 0x69}},
    {L"FixedBootGroup", 				{0xec87d643, 0xeba4, 0x4bb5, 0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0x0d, 0xa9}},
    {L"AMITSESetup", 					{0xc811fa38, 0x42c8, 0x4579, 0xa9, 0xbb, 0x60, 0xe9, 0x4e, 0xdd, 0xfb, 0x34}},
    {L"UsbSupport", 					{0xec87d643, 0xeba4, 0x4bb5, 0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0x0d, 0xa9}},
    {L"NetworkStackVar", 				{0xD1405D16, 0x7AFC, 0x4695, 0xBB, 0x12, 0x41, 0x45, 0x9D, 0x36, 0x95, 0xA2}},
    {L"Timeout", 						{0x8be4df61, 0x93ca, 0x11d2, 0xaa, 0x0d, 0x00, 0xe0, 0x98, 0x03, 0x2b, 0x8c}},
    {L"IntelSetup", 					{0xec87d643, 0xeba4, 0x4bb5, 0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0x0d, 0xa9}},
    {L"SocketIioConfig", 				{0xdd84017e, 0x7f52, 0x48f9, 0xb1, 0x6e, 0x50, 0xed, 0x9e, 0x0d, 0xbe, 0x27}},
    {L"SocketMemoryConfig", 			{0x98cf19ed, 0x4109, 0x4681, 0xb7, 0x9d, 0x91, 0x96, 0x75, 0x7c, 0x78, 0x24}},
    {L"FpgaSocketConfig", 				{0x75839b0b, 0x0a99, 0x4233, 0x8a, 0xa4, 0x38, 0x66, 0xf6, 0xce, 0xf4, 0xb3}},
    {L"SocketCommonRcConfig", 			{0x4402ca38, 0x808f, 0x4279, 0xbc, 0xec, 0x5b, 0xaf, 0x8d, 0x59, 0x09, 0x2f}},
    {L"SocketMpLinkConfig", 			{0x2b9b22de, 0x2ad4, 0x4abc, 0x95, 0x7d, 0x5f, 0x18, 0xc5, 0x04, 0xa0, 0x5c}},
    {L"SocketPowerManagementConfig", 	{0xa1047342, 0xbdba, 0x4dae, 0xa6, 0x7a, 0x40, 0x97, 0x9b, 0x65, 0xc7, 0xf8}},
    {L"SocketProcessorCoreConfig", 		{0x07013588, 0xc789, 0x4e12, 0xa7, 0xc3, 0x88, 0xfa, 0xfa, 0xe7, 0x9f, 0x7c}},
    {L"PchRcConfiguration", 			{0xd19a26a3, 0x17f1, 0x48c3, 0x8a, 0x1e, 0x11, 0xeb, 0x0a, 0x7f, 0x6e, 0x4e}},
    {L"MeRcConfiguration", 				{0x2b26358d, 0xf899, 0x41c4, 0x9b, 0xc2, 0x82, 0xa3, 0x38, 0xb9, 0x93, 0xd8}},
    {L"SecureBootSetup", 				{0x7B59104A, 0xC00D, 0x4158, 0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15}},
    {L"SMC_BOARD_INFO_NAME", 			{0x262b2e3f, 0xda2d, 0x4b81, 0x8d, 0x1d, 0xf9, 0x38, 0xe8, 0x51, 0xfd, 0xed}},
	// Always keep below line in the end and don't change it.
	{L"SMC_VAR_END",                    {0xFFFFFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}} 
};

EFI_STATUS
EFIAPI
SmcDownloadOobDataStart (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT32                            *FileSize,
  OUT UINT8                             *ResponseStatus
  )
{
  EFI_STATUS Status;

  if (This == NULL || FileSize == NULL || ResponseStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = DataToBIOSReadyNotify (FileType, ResponseStatus, FileSize);

  return Status;
}

EFI_STATUS
EFIAPI
SmcDownloadOobDataReady (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT8                             *ResponseStatus
  )
{
  EFI_STATUS Status;

  if (This == NULL || ResponseStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = DataToBIOSWait (FileType, ResponseStatus);

  return Status;
}

EFI_STATUS
EFIAPI
SmcDownloadOobDataDone (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  IN  UINT8                             UpdateStatus
  )
{
  EFI_STATUS Status;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = DataToBIOSDone (FileType, UpdateStatus);

  return Status;
}

EFI_STATUS
EFIAPI
SmcUploadOobDataStart (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  IN  UINT32                            FileSize,
  OUT UINT8                             *ResponseStatus
  )
{
  EFI_STATUS Status;

  if (This == NULL || ResponseStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = DataToBMCReadyNotify (FileType, FileSize, ResponseStatus);

  return Status;
}

EFI_STATUS
EFIAPI
SmcUploadOobDataDone (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT8                             *ResponseStatus
  )
{
  EFI_STATUS Status;

  if (This == NULL || ResponseStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = DataToBMCDone (FileType, ResponseStatus);

  return Status;
}

EFI_STATUS
EFIAPI
SmcReadDataToBios (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  OUT UINT8                             *Buffer,
  IN  UINT32                            BufferLength
  )
{
  EFI_STATUS Status;

  if (This == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = DataToBIOSReadData (Buffer, BufferLength);

  return Status;
}

EFI_STATUS
EFIAPI
SmcPrepareDataToBmc (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             *Buffer,
  IN  UINT32                            BufferLength
  )
{
  EFI_STATUS Status;
  UINT8 CheckSum = 0x0;

  if (This == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = DataToBMCPrepareData (Buffer, BufferLength,&CheckSum);

  return Status;
}

EFI_STATUS
EFIAPI
SmcGetSetOobIdentify (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT32                            BoardId,
  IN  UINT32                            Date,
  IN  UINT16                            Time
  )
{
  EFI_STATUS Status;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SMC_IPMICmd30_A0_12 (BoardId, Date, Time);

  return Status;
}

EFI_STATUS
EFIAPI
SmcSmibiosPreservation (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             Operation,
  IN  UINT8                             FlashFlag,
  OUT UINT8                             *ResponseStatus
  )
{
  EFI_STATUS Status;

  if (This == NULL || ResponseStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SMC_IPMICmd30_A0_15 (Operation, &FlashFlag);
  *ResponseStatus = FlashFlag;

  return Status;
}

EFI_STATUS
RomHoleWriteRegion (
  IN  UINT8                             Operation,
  IN  UINT8                             *pBuffer
  )
{
  EFI_STATUS Status;

  if (pBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  Status = SmcRomHoleWriteRegion (Operation, pBuffer);

  return Status;
}

EFI_STATUS
RomHoleReadRegion (
  IN  UINT8                             Operation,
  OUT UINT8                             *pBuffer
  )
{
  EFI_STATUS Status;

  if (pBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SmcRomHoleReadRegion (Operation, pBuffer);

  return Status;
}


UINT32
RomHoleSize (
  IN  UINT8                             Operation
  )
{
  return SmcRomHoleSize (Operation);
}


UINT32
RomHoleOffset (
  IN  UINT8                             Operation
  )
{
  return SmcRomHoleOffset (Operation);
}

EFI_STATUS
SmcOobPlatformPolicyDxeEntry (
  IN  EFI_HANDLE               ImageHandle,
  IN  EFI_SYSTEM_TABLE         *SystemTable
  )
{
  SMC_OOB_PLATFORM_POLICY          *SmcOobPlatformPolicy;
  EFI_STATUS                       Status;
  EFI_HANDLE                       NewHandle;
  EFI_EVENT                        Event;
  VOID                             *Registration;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (SMC_OOB_PLATFORM_POLICY),
                  &SmcOobPlatformPolicy
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }

  ZeroMem (SmcOobPlatformPolicy, sizeof (SMC_OOB_PLATFORM_POLICY));
  mSmcOobPlatformPolicy = SmcOobPlatformPolicy;

  SmcOobPlatformPolicy->PolicyRevision                                        = SMC_OOB_PLATFORM_POLICY_REVISION_1;
  SmcOobPlatformPolicy->PlatformId                                            = OobPurley;

  SmcOobPlatformPolicy->SumFeatureFlags.SumFeatureFlagMajorVersion            = SUM_FEATURE_FLAGS_MAJORVERSION;
  SmcOobPlatformPolicy->SumFeatureFlags.SumFeatureFlagMinorVersion            = SUM_FEATURE_FLAGS_MINORVERSION;
  SmcOobPlatformPolicy->SumFeatureFlags.SumPlatformId                         = SUM_PLATFORM_ID;
  SmcOobPlatformPolicy->SumFeatureFlags.SumWithWithoutBmc                     = SUM_WITH_WITHOUT_BMC;
  SmcOobPlatformPolicy->SumFeatureFlags.SumInbandJumperlessUpdateBios         = SUM_INBAND_JUMPERLESS_UPDATEBIOS;
  SmcOobPlatformPolicy->SumFeatureFlags.SumBiosConfigurationSupportOutband    = SUM_BIOS_CONFIGURATION_SUPPORT_OUTBAND;
  SmcOobPlatformPolicy->SumFeatureFlags.SumBiosConfigurationSupportInband     = SUM_BIOS_CONFIGURATION_SUPPORT_INBAND;
#ifdef SUM_ME_Disabling_UpdateBios
  SmcOobPlatformPolicy->SumFeatureFlags.SumMeDisablingUpdateBios              = SUM_ME_Disabling_UpdateBios;
#endif
  SmcOobPlatformPolicy->SumFeatureFlags.SumEditDmiSupportOutband              = SUM_EDIT_DMI_SUPPORT_OUTBAND;
  SmcOobPlatformPolicy->SumFeatureFlags.SumEditDmiSupportInband               = SUM_EDIT_DMI_SUPPORT_INBAND;
  SmcOobPlatformPolicy->SumFeatureFlags.SumPresistentDmiBiosCfgSupportOutband = SUM_PERSISTENT_DMI_BIOSCFG_SUPPORT_OUTBAND;
  SmcOobPlatformPolicy->SumFeatureFlags.SumFullSmbiosSupportOutband           = SUM_FULL_SMBIOS_SUPPORT_OUTBAND;
  SmcOobPlatformPolicy->SumFeatureFlags.SumFullSmbiosSupportInband            = SUM_FULL_SMBIOS_SUPPORT_INBAND;
  SmcOobPlatformPolicy->SumFeatureFlags.SumVpdSupportOutband                  = SUM_VPD_SUPPORT_OUTBAND;
  SmcOobPlatformPolicy->SumFeatureFlags.SumVpdSupportInband                   = SUM_VPD_SUPPORT_INBAND;
  SmcOobPlatformPolicy->SumFeatureFlags.SumHddInformationBiosSupport          = SUM_HDD_INFORMATION_BIOS_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumMctpFwUpdateBiosSupport            = SUM_MCTP_FW_UPDATE_BIOS_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumVpdType40BiosSupport               = SUM_VPD_TYPE40_BIOS_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumFru1DataBiosSupport                = SUM_FRU1_DATA_BIOS_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumIntelRcEventLogSupport             = SUM_INTEL_RC_EVENT_LOG_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumTpmProvisionSupport                = SUM_TPM_PROVISION_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumAutoTpmProvisionSupport            = SUM_AUTO_TPM_PROVISION_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumTpmCustomProvisionTableSupport     = SUM_TPM_CUSTOM_PROVISION_TABLE_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumTpmV12Support                      = SUM_TPM_V12_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumTpmV20Support                      = SUM_TPM_V20_SUPPORT;
  SmcOobPlatformPolicy->SumFeatureFlags.SumDefaultPreserveSmbiosSupport       = SUM_DEFAULT_PRESERVE_SMBIOS_SUPPORT;  
  SmcOobPlatformPolicy->SumFeatureFlags.SumDefaultProgramNvramSupport         = SUM_DEFAULT_PROGRAM_NVRAM_SUPPORT;  
  SmcOobPlatformPolicy->SumFeatureFlags.SumDefaultUpdateMeSupport             = SUM_DEFAULT_UPDATE_ME_SUPPORT;

  SmcOobPlatformPolicy->OobConfigurations.Md5Support         = SMC_OOB_MD5_SUPPORT;
  SmcOobPlatformPolicy->OobConfigurations.AssetInfoVersion   = SmcAssetInfoVersion;
  SmcOobPlatformPolicy->OobConfigurations.BiosCfgVersion     = SmcBiosCfgVersion;
  SmcOobPlatformPolicy->OobConfigurations.HiiVersion         = SmcHiiVersion;
  SmcOobPlatformPolicy->OobConfigurations.SmbiosVersion      = SmcSmbiosVersion;
  SmcOobPlatformPolicy->OobConfigurations.FullSmbiosVersion  = SmcFullSMVersion;
  SmcOobPlatformPolicy->OobConfigurations.SmbiosMajorVersion = SMBIOS_3X_MAJOR_VERSION;
  SmcOobPlatformPolicy->OobConfigurations.SmbiosMinorVersion = SMBIOS_3X_MINOR_VERSION;
  SmcOobPlatformPolicy->OobConfigurations.SmbiosDocVersion   = SMBIOS_DOC_VERSION;


  SmcOobPlatformPolicy->OobConfigurations.InBandSwSmiE6      = SWSMI_SMC_INBAND_E6;
  SmcOobPlatformPolicy->OobConfigurations.InBandSwSmiE8      = SWSMI_SMC_INBAND_E8;
  SmcOobPlatformPolicy->OobConfigurations.CmosSmcAddrHH      = Q_SMC_ADDR_HH;
  SmcOobPlatformPolicy->OobConfigurations.CmosSmcAddrH       = Q_SMC_ADDR_H;
  SmcOobPlatformPolicy->OobConfigurations.CmosSmcAddrL       = Q_SMC_ADDR_L;
  SmcOobPlatformPolicy->OobConfigurations.CmosSmcAddrLL      = Q_SMC_ADDR_LL;

  SmcOobPlatformPolicy->OobConfigurations.RootForm           = RootForm;
  SmcOobPlatformPolicy->OobConfigurations.RootFormCount      = sizeof (RootForm) / sizeof (SMC_ROOT_FORM_DATA);
  SmcOobPlatformPolicy->OobConfigurations.HideFormForSum     = HideFormForSum;
  SmcOobPlatformPolicy->OobConfigurations.HideFormCount      = sizeof (HideFormForSum) / sizeof (SMC_ROOT_FORM_DATA);

  SmcOobPlatformPolicy->OobConfigurations.BoardInfoSvid  = SMC_SSID;
  SmcOobPlatformPolicy->OobConfigurations.BoardInfoDate  = TWO_DIGIT_DAY_INT + (TWO_DIGIT_MONTH_INT << 8) + (FOUR_DIGIT_YEAR_INT << 16);
  SmcOobPlatformPolicy->OobConfigurations.BoardInfoTime  = TWO_DIGIT_MINUTE_INT + (TWO_DIGIT_HOUR_INT << 8);
  if (SmcOobPlatformPolicy->OobConfigurations.Md5Support) {
    SmcOobPlatformPolicy->OobConfigurations.OobFileStatus0 = PcdGet32 (PcdGetOobFileStatus0);
    SmcOobPlatformPolicy->OobConfigurations.OobFileStatus1 = PcdGet32 (PcdGetOobFileStatus1);
    SmcOobPlatformPolicy->OobConfigurations.OobFileStatus2 = PcdGet32 (PcdGetOobFileStatus2);
    SmcOobPlatformPolicy->OobConfigurations.OobFileStatus3 = PcdGet32 (PcdGetOobFileStatus3);
  }

  SmcOobPlatformPolicy->PlatformVar = VarGUID;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  UpdateOobRefVariables,
                  NULL,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->RegisterProtocolNotify (
                  &gOobSignalEventAfterBdsConnect,
                  Event,
                  &Registration
                  );
  ASSERT_EFI_ERROR (Status);

  NewHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gSmcOobPlatformPolicyGuid,
                  EFI_NATIVE_INTERFACE,
                  SmcOobPlatformPolicy
                  );
  ASSERT_EFI_ERROR (Status);

  NewHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gSmcIpmiOemCommandSetProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSmcIpmiOemCommandSetProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  NewHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gSmcRomHoleProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSmcRomHoleProtocol
                  );
  ASSERT_EFI_ERROR (Status);


  InstallSmcOobLibraryProtocolInterface();

  return EFI_SUCCESS;
}

VOID
EFIAPI
UpdateOobRefVariables (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       NewHandle;

#if defined(IPMI_SUPPORT) && (IPMI_SUPPORT == 1)  
  SERVER_MGMT_CONFIGURATION_DATA   ServerConfiguration;
  UINT32                           VariableAttributes;
  UINTN                            VariableSize;  

  VariableSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
  Status = gRT->GetVariable (
                  L"ServerSetup",
                  &gEfiServerMgmtSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &ServerConfiguration
                  );
  if (!EFI_ERROR (Status)) {
    mSmcOobPlatformPolicy->OobRefVariables.IpmiLanSelection = ServerConfiguration.IPMILANSelection;
  } else {
    mSmcOobPlatformPolicy->OobRefVariables.IpmiLanSelection = 0xFFFFFFFF;    
  }

#endif

  DEBUG((-1,"Oob UpdateOobRefVariables Entry!\n"));

  NewHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gSmcOobPlatformPolicyCallbackGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return;
}

