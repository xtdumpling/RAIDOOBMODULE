#ifndef AMI_TCG_GUID_INCLUDES_H
#define AMI_TCG_GUID_INCLUDES_H

#include <Token.h>

#define TCG_LOCK_DOWN_VAR_GUID\
  {0x6e605536, 0xa30a, 0x4d56, 0x93, 0x9e, 0x1c, 0x37, 0x3f, 0x79, 0x8d, 0x7b}

#define AMI_TPM_ROLLBACK_SMM_PROTOCOL_GUID \
  {0x79fbad19, 0xf9e1, 0x4d08, 0xb7, 0xbc, 0x35, 0x84, 0x53, 0x19, 0x5e, 0x41}

#define AMI_FLAGS_STATUS_GUID \
    {0x7d3dceee, 0xcbce, 0x4ea7, 0x87, 0x09, 0x6e, 0x55, 0x2f, 0x1e, 0xdb, 0xde}

#define AMI_TCG_EFI_OS_VARIABLE_GUID \
    {0xa8a2093b, 0xfefa, 0x43c1, 0x8e, 0x62, 0xce, 0x52, 0x68, 0x47, 0x26, 0x5e}

#define EFI_TCG_WAKE_EVENT_DATA_HOB_GUID \
    {0xBBB810BB, 0x5EF0, 0x4E8F, 0xB2, 0x98, 0xAD, 0x74, 0xAA, 0x50, 0xEF, 0x0A}

#define AMI_TCM_CALLBACK_GUID \
    { 0x91c74e50, 0x361d, 0x4cda, 0xa1, 0x6b, 0xc9, 0x2b, 0xe4, 0xbf, 0x16,0xee}

#define AMI_TPM_LEGACY_GUID \
    { 0xa1c85085, 0x3053, 0x4c4b, 0xa9, 0xf6, 0x72, 0x4d, 0x22, 0xa7, 0x6e,\
      0xf9}

#define EFI_TCG_MPDriver_HOB_GUID \
    { 0xbaaead09, 0x2a0,  0x4131, 0x9e, 0xd,  0xbc, 0x52, 0x9e, 0xf0, 0xff,\
      0x2a}

#define EFI_TCG_LOG_HOB_GUID \
    {0x5f7d4e0e, 0x3d6d, 0x42bc, 0xa9, 0x42, 0xe, 0x91, 0xe8, 0x3e, 0x3c, 0x31}

#define TPM20_HOB_GUID \
    { \
      0x7cea4f7e, 0x2052, 0x46ed, 0xbe, 0xfd, 0xe2, 0x2a, 0x44, 0xdc, 0x65, 0xe7 \
    }

#define AMI_FV_HOB_LIST_GUID \
    { 0x3a35ee58, 0x8286, 0x4f00, 0x9a, 0x5b, 0x1b, 0x6a, 0xb7, 0x68, 0x8, 0x15 }

#define AMI_TCG_INTERFACE_VAR_GUID \
    { 0x6e6ebc2d, 0x77ab, 0x46cf, 0xb2, 0xa7, 0xcc, 0x96, 0x8b, 0xe, 0x8a,\
      0xf3}

#define AMI_PPI_INTERNAL_VAR_GUID \
    { 0x78097bb6, 0x48cf, 0x449b, 0x9b, 0xdb, 0xf6, 0x38, 0x49, 0x85, 0x64,\
      0x60}

#define  AMI_PROCESS_TCG_SETUP_GUID\
    { 0xc77dd102, 0x1db4, 0x4997, 0xae, 0x37, 0x4e, 0x8c, 0x52, 0x1e, 0xf5,\
      0x67}

#define  AMI_MEASURE_CPU_MICROCODE_GUID\
    { 0x5cf308b5, 0xfa23, 0x4100, 0x8a, 0x76, 0xf3, 0x26, 0xc2, 0x81, 0x48,\
      0x80}

#define  AMI_MEASURE_PCIOPROM_GUID\
    { 0x76f3992d, 0x529e, 0x4efe, 0x8b, 0xbe, 0x8e, 0x1e, 0xd4, 0x32, 0xc2,\
      0x23}

#define  AMI_TPM12_LOCK_PHYSICAL_PRESENCE_GUID\
    { 0x9c53ce0e, 0xe9f, 0x4f20, 0xb1, 0xa1, 0x15, 0xe, 0x43, 0x49, 0xd7,\
      0x77}

#define AMI_TPM12_MEASURE_CONFIGURATION_INFO_GUID \
    { 0x6da670e8, 0x3d73, 0x4eb2, 0xa7, 0x21, 0xa2, 0xdd, 0xf6, 0x82, 0xfd, 0xd8 }

#define  AMI_PROCESS_TCG_PPI_REQUEST_GUID\
    { 0x30ad2b83, 0xadd0, 0x414b, 0xb1, 0x1c, 0xf9, 0x3c, 0xc1, 0xd0, 0xb7,\
      0x9b}

#define  AMI_SET_TCG_READYTOBOOT_GUID\
    { 0xa4524a9c, 0xb5e, 0x492d, 0xae, 0xc9, 0x30, 0x86, 0x31, 0xb1, 0x89,\
      0xb4}

#define  TCGLEGX16_FILE_GUID\
    {0x142204e2, 0xc7b1, 0x4af9, 0xa7, 0x29, 0x92, 0x37, 0x58, 0xd9, 0x6d,\
    0x3}

#define  TPM32_FILE_GUID \
    { 0xaa31bc6, 0x3379, 0x41e8, 0x82, 0x5a, 0x53, 0xf8, 0x2c, 0xc0, 0xf2,\
    0x54}

#define  MPTPM_FILE_GUID \
    { 0x7d113aa9, 0x6280, 0x48c6, 0xba, 0xce, 0xdf, 0xe7, 0x66, 0x8e, 0x83,\
    0x7}

#define  TCMLEGX16_FILE_GUID \
    { 0x1E753E16, 0xDCEF, 0x47d0, 0x9A, 0x38, 0x7A, 0xDE, 0xCD, 0xB9, 0x83,\
    0xED}

#define  TCM32_FILE_GUID \
   { 0xB74E676E,0x3B2E, 0x483f, 0x94, 0x58, 0xC3, 0x78, 0xFE, 0x0A, 0xC6,\
    0x9F}

#define AMI_PPI_GUID \
    {0x1bb13967, 0x8b24, 0x411b, 0x98, 0x28, 0x18, 0xd2, 0x85, 0xa9, 0xcc, 0x4d}

// Add for TPM2 Final Event Table, Rev0.4/Rev0.9
#define EFI_TCG2_FINAL_EVENTS_TABLE_GUID \
{ 0x1e2ed096, 0x30e2, 0x4254, {0xbd, 0x89, 0x86, 0x3b, 0xbe, 0xf8, 0x23, 0x25} }

#ifdef AMI_MODULE_PKG_VERSION
#if AMI_MODULE_PKG_VERSION < 28
#define AMI_RESET_SYSTEN_EVENT_GUID { 0x62da6a56, 0x13fb, 0x485a, 0xa8, 0xda, 0xa3, 0xdd, 0x79, 0x12, 0xcb, 0x6b }
#endif
#endif

//------------------------------------------------------------------------------
// PORTING
//------------------------------------------------------------------------------
//This guid is from PttSsdt.inf(file guid)
#define PPT_SSDT_ACPI_TABLE_GUID \
  { \
    0xb733c141, 0xe88f, 0x4786, 0x94, 0xaf, 0x8b, 0x87, 0xbc, 0x48, 0x67, 0xfe \
  }




//MdePkg
//  gEfiImageSecurityDatabaseguid = { 0xd719b2cb, 0x3d3a, 0x4596, {0xa3, 0xbc, 0xda, 0xd0,  0xe, 0x67, 0x65, 0x6f } }
//  ---------------------------------------------------------------------------------------------------------------------------
  extern EFI_GUID gSkipTpmStartupGuid;
  extern EFI_GUID TpmRollbackSmmGuid;
  extern EFI_GUID TcgPasswordAuthenticationGuid;
  extern EFI_GUID gEfiImageSecurityDatabaseguid;
  extern EFI_GUID FastBootVariableGuid;
  extern EFI_GUID FlagsStatusguid;
  extern EFI_GUID AmiTseSetupGuid;
  extern EFI_GUID AmitcgefiOsVariableGuid;
  extern EFI_GUID gSetupVariableGuid;
  extern EFI_GUID gEfiTcgWakeEventDataHobGuid;
  extern EFI_GUID gAmiTcmSignalguid;
  extern EFI_GUID gAmiLegacyTpmguid;
  extern EFI_GUID gEfiTcgMpDriverHobGuid;
  extern EFI_GUID gEfiPeiTcgLogHobGuid;
  extern EFI_GUID gPeiEfiTcgWakeEventDataHobGuid;
  extern EFI_GUID AmiMemoryPresentFunctionOverrideGuid;
  extern EFI_GUID AmiSetPhysicalPresenceGuid;
  extern EFI_GUID AmiVerifyTcgVariablesGuid;
  extern EFI_GUID gTpm20HobGuid;
  extern EFI_GUID amiFVhoblistguid;
  extern EFI_GUID mAmiTcgInterfaceVarGuid;
  extern EFI_GUID mAmiPpiInternalVarGuid;
  extern EFI_GUID AmiTcgResetVarHobGuid;
  extern EFI_GUID gEfiTcgTreeLogHobGuid;
  extern EFI_GUID gTcgInternalflagGuid;
  extern EFI_GUID gTcgPpiSyncFlagGuid;
  extern EFI_GUID gTcgEfiGlobalVariableGuid;
  extern EFI_GUID gAmiMeasureCPUMicrocodeGuid;
  extern EFI_GUID Tpm12PPILockOverrideguid;
  extern EFI_GUID Tpm12MeasureConfigurationInfoFuncGuid;
  extern EFI_GUID gEfiTcgMpDriverHobGuid;
  extern EFI_GUID AmiMeasurePciopromGuid;
  extern EFI_GUID AmiBiosPpiFlagsManagementGuid;
  extern EFI_GUID AmiProcessTcgSetupGuid;
  extern EFI_GUID AmiProcessTcgPpiRequestGuid;
  extern EFI_GUID gPeiTpmInitializedPpiGuid;
  extern EFI_GUID gAmiOsPpiConfirmationOverrideGuid;
  extern EFI_GUID gAmiTpm20MeasureConfigurationInfoGuid;
  extern EFI_GUID gPttSsdtAcpiTableGuid;
  extern EFI_GUID ExtendedDataGuid;
  extern EFI_GUID gTcgInternalflagGuid;
  extern EFI_GUID gAmiPpiGuid;
  extern EFI_GUID AmiValidBootImageCertTblGuid;
  extern EFI_GUID gTcg2FinalEventsTableGuid;
  extern EFI_GUID AmiSetTcgReadytobootGuid;
  extern EFI_GUID gMicrocodeGuid;
  extern EFI_GUID AmiMemoryAbsentOverrideGuid;
  extern EFI_GUID gEfiTcgMADriverHobGuid;
  extern EFI_GUID gEfiTcgMADriverGuid;
  extern EFI_GUID gTcgLegX16FileGuid;
  extern EFI_GUID gTpm32FileGuid;
  extern EFI_GUID gMpTpmFileGuid;
  extern EFI_GUID gTcmLegX16FileGuid;
  extern EFI_GUID gTcm32FileGuid;
  extern EFI_GUID gMpTcmFileGuid;
  extern EFI_GUID TcgPeiPolicyHobGuid;
  extern EFI_GUID RsdEventGuid;
  extern EFI_GUID RsdTcg2HobGuid;
#endif
