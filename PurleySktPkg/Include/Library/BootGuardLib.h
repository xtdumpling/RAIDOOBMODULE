/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c) 2014-2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file 
  BootGuardLibrary.h

  @brief 
  Header file for BootGuardLibrary Lib implementation. 
  
**/
#ifndef _BOOT_GUARD_LIBRARY_H_
#define _BOOT_GUARD_LIBRARY_H_

#include <Library/BaseLib.h>
#include <PiPei.h>

#define B_NEM_INIT                    BIT0
#define MSR_BC_PBEC                   0x139
#define B_STOP_PBET                   BIT0
#define R_CPU_BOOT_GUARD_ACM_STATUS   0x328
#define R_CPU_BOOT_GUARD_ACM_POLICY   0x32C

//
// Defines for Boot Guard
//
#define MSR_BOOT_GUARD_SACM_INFO                                      0x13A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0
#define V_TPM_PRESENT_MASK                                            0x06
#define V_TPM_PRESENT_NO_TPM                                          0
#define V_TPM_PRESENT_DTPM_12                                         1
#define V_TPM_PRESENT_DTPM_20                                         2
#define V_TPM_PRESENT_PTT                                             3
#define B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS                            BIT3
#define B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT                          BIT5
#define B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT                          BIT6
#define B_BOOT_GUARD_SACM_INFO_CAPABILITY                             BIT32
#define TXT_PUBLIC_BASE                                               0xFED30000
#define R_CPU_BOOT_GUARD_ERRORCODE                                    0x30
#define R_CPU_BOOT_GUARD_BOOTSTATUS                                   0xA0
#define R_CPU_BOOT_GUARD_ACM_ERROR                                    0x328
#define LT_EXTENDED_STATUS_REGISTER                                   0x08
#define V_CPU_BOOT_GUARD_LOAD_ACM_FAILED                              0x2000000000000000
#define B_BOOT_GUARD_ACM_ERRORCODE_MASK                               0x00007FF0

/**
   The TPM category, TPM 1.2, TPM 2.0 and PTT are defined.
**/
typedef enum {
  TpmNone = 0, ///< 0: No TPM device present on system
  dTpm12,      ///< 1: TPM 1.2 device present on system
  dTpm20,      ///< 2: TPM 2.0 device present on system
  Ptt,         ///< 3: PTT present on system
  TpmTypeMax   ///< 4: Unknown device
} TPM_TYPE;

/**
   Boot Guard ACM Error Class Types
**/ 
typedef enum
{
    BtgNoErrorClass,
    BtgInitErrorClass,
    BtgBootPolicyErrorClass,
    BtgFitErrorClass,
    BtgManifestErrorClass,
    BtgKeyManifestErrorClass,
    BtgTpmErrorClass,
    BtgNemErrorClass,
    BtgIbbErrorClass,
    BtgMeasureDetailPcrErrorClass,
    BtgMeErrorClass,
    BtgTpmStartupLoc0ErrorClass,
    BtgTpmStartupCmdErrorClass,
    BtgTpmStartupLoc3ErrorClass,
    BtgTpmExtendPcr18ErrorClass,
    BtgTpmExtendLoc0ErrorClass,
    BtgTpmExtendPcr0ErrorClass,
    BtgTpmExtendPcr7ErrorClass,
    BtgMeasureAuthPcrErrorClass,
    BtgDtpm20HierarchyErrorClass,
        
  //
  // Last error class, don't add after it
  //
    BtgMaxErrorClass                 = 0x3F,
} BTG_ERROR_CLASS;

/**
  Error code values for init errors class:BtgInitErrorClass
**/
typedef enum 
{
    BtgInitSuccess,
    BtgInitPbeNotSetError,
    BtgInitInvalidBootTypeError,
    BtgInitInvalidKmIdError,
    BtgInitAcmRevokedError,
    BtgInitInvalidBpKeyError,
    BtgInitInvalidBpProfileError,
    BtgInitInvalidCpuOrUcodePatchError,
    BtgInitAcmChipsetFlagsMismatchError,
    BtgInitInvalidChipsetError,
    BtgInitAcmSignatureCopyError,
    BtgInitAcmAddressRangeError,

  //
  // Last error, don't add after it
  //
    BtgInitMaxErrorCode                 = 0xF,
} BTG_INIT_ERROR_CODE;

/**
  Error code values for FIT errors class:BtgFitErrorClass
**/
typedef enum
{
    BtgFitSuccess,
    BtgFitHashCalcFailError,
    BtgFitInvalidHeaderError,
    BtgFitHeaderCopyError,
    BtgFitCopyError,
    BtgFitKmEntryError,
    BtgFitManifestEntryError,

  //
  // Last error, don't add after it
  //
    BtgFitMaxErrorCode,
} BTG_FIT_ERROR_CODE;

/**
  Error code values for crypto errors will match both KM and manifest error classes:
  BtgKeyManifestErrorClass or BtgManifestErrorClass
**/
typedef enum
{
    BtgCryptoSuccess,
    BtgCryptoHashCalcFailError,
    BtgCryptoHashNotValidError,
    BtgCryptoRsaInitFailError,
    BtgCryptoExpInitFailError,
    BtgCryptoModulusInitFailError,
    BtgCryptoRsaPairFailError,
    BtgCryptoVerifySigFailError,
    BtgCryptoSigNotValidError,
    
  //
  // Last error, don't add after it
  //
    BtgCryptoMaxErrorCode,
} BTG_CRYPTO_ERROR_CODE;

/**
  Error code values for KM errors class:BtgKeyManifestErrorClass
**/
typedef enum
{
    BtgKmSuccess,
    BtgKmHashCalcFailError,
    BtgKmHashNotValidError,
    BtgKmRsaInitFailError,
    BtgKmExpInitFailError,
    BtgKmModulusInitFailError,
    BtgKmRsaPairFailError,
    BtgKmVerifySigFailError,
    BtgKmSigNotValidError,
    BtgKmCopyFailError,
    BtgKmSigCopyFailError,
    BtgKmInvalidStructError,
    BtgKmInvalidKmIdError,
    BtgKmRevokedError,

  //
  // Last error, don't add after it
  //
    BtgKmMaxErrorCode,
} BTG_KEY_MANIFEST_ERROR_CODE;

/**
  Error code values for manifest errors class:BtgManifestErrorClass
**/
typedef enum {
    BtgManifestSuccess,
    BtgManifestHashCalcFailError,
    BtgManifestHashNotValidError,
    BtgManifestRsaInitFailError,
    BtgManifestExpInitFailError,
    BtgManifestModulusInitFailError,
    BtgManifestRsaPairFailError,
    BtgManifestVerifySigFailError,
    BtgManifestSigNotValidError,
    BtgManifestCopyFailError,
    BtgManifestSigCopyFailError,
    BtgManifestInvalidHeaderStructError,
    BtgManifestInvalidIbbStructError,
    BtgManifestInvalidPmDataStructError,
    BtgManifestInvalidSignatureStructError,
    BtgManifestRevokedError,
    BtgManifestInvalidSizeError,

  //
  // Last error, don't add after it
  //
    BtgManifestMaxErrorCode,
} BTG_MANIFEST_ERROR_CODE;

/**
  Error code values for IBB errors class:BtgIbbErrorClass
**/
typedef enum
{
    BtgIbbSuccess,
    BtgIbbShaInitFailError,
    BtgIbbShaUpdateFailError,
    BtgIbbShaFinalFailError,
    BtgIbbHashNotValidError,
    
  //
  // Last error, don't add after it
  //
    BtgIbbMaxErrorCode,
} BTG_IBB_ERROR_CODE;

/**
  Error code values for MEASURE errors classes (both authority PCR and detail PCR):
  BtgMeasureDetailPcrErrorClass or BtgMeasureAuthPcrErrorClass
**/
typedef enum
{
    BtgMeasureSuccess,
    BtgMeasureShaInitFailError,
    BtgMeasureShaUpdateFailError,
    BtgMeasureShaFinalFailError,
    BtgMeasureHashCopyError,
    BtgMeasureTpmExtendError,
    BtgMeasureTpmExtendFailError,
    BtgMeasureHashCalcFailError,

  //
  // Last error, don't add after it
  //
    BtgMeasureMaxErrorCode,
} BTG_MEASURE_ERROR_CODE;

/**
  Error code values for NEM errors class:BtgNemErrorClass
**/
typedef enum
{
    BtgNemSuccess,
    BtgNemHashCalcFailError,
    BtgNemMtrrCountError,
    BtgNemInsufficientNumofMtrrsError,
    BtgNemIbbSegmentError,
    BtgNemIbbSegmentOverlapError,
    BtgNemIbbOutOfMtrrRangeError,
    BtgNemIbbEntryPointOutOfIbbError,
    BtgNemCantReadLlcSizeError,
    BtgNemLlcSizeTooSmallError,
    BtgNemIllegalDmaValueError,
    BtgNemManifestFlashCramCompareError,
    BtgNemFitFlashCramCompareError,
    BtgNemVtdEnableFailedError,

  //
  // Last error, don't add after it
  //
    BtgNemMaxErrorCode,
} BTG_NEM_ERROR_CODE;

/**
  Error code values for TPM errors class:BtgTpmErrorClass
**/
typedef enum
{
    BtgTpmSuccess,
    BtgFtpmStartupError,
    BtgFtpmStartupNotEnabledError,
    BtgFtpmStartupNotReadyError,
    BtgFtpmStartupInternalError,
    BtgTpmInitUnsupportedTpmType,
    BtgTpm12StsNotReady,
    BtgTpm12StartupWrongRspTag,
    BtgTpm12StartupWrongParamSize,
    BtgTpm12StartupWrongResult,
    BtgTpm12VendorNone,
    BtgTpm12ExtendWrongRspTag,
    BtgTpm12ExtendWrongParamSize,
    BtgTpm12ExtendWrongResult,
    BtgTpm12AccessActiveLocalityTimeOut,
    BtgTpm12BurstCountBeforeReadTimeOut,
    BtgTpm12BurstCountBeforeWriteTimeOut,
    BtgTpm12StsExpectOrDataAvail0AtFifoRead,
    BtgTpm12StsExpectOrDataAvail0AtFifoWrite,
    BtgTpm12StsExpectOrDataAvail1AfterFifoRead,
    BtgTpm12StsExpectOrDataAvail1AfterFifoWrite,
    BtgTpmStsValidAfterFifoReadTimeOut,
    BtgTpmStsValidAfterFifoWriteTimeOut,
    BtgTpmMeasureUnsupportedTpmType,
    BtgDtpm20StartupWrongRspTag,
    BtgDtpm20StartupWrongParamSize,
    BtgDtpm20StartupWrongResult,
    BtgTpm20ExtendWrongRspTag,
    BtgTpm20ExtendWrongParamSize,
    BtgTpm20ExtendWrongResult,
    BtgDtpm20HierarchyControlError,

  //
  // Last error, don't add after it
  //
    BtgTpmMaxErrorCode,
} BTG_TPM_ERROR_CODE;

/**
  Error code values for ME errors class:BtgMeErrorClass
**/
typedef enum
{
    BtgMeSuccess,
    BtgMeNotOkProceedError,

  //
  // Last error, don't add after it
  //
    BtgMeMaxErrorCode,
} BTG_ME_ERROR_CODE;

/**
    Information related to Boot Guard Configuration.
**/
typedef struct {
  BOOLEAN  MeasuredBoot;        ///< Report Measured Boot setting in Boot Guard profile. 0: Disable; 1: Enable.
  /**
  ByPassTpmInit is set to 1 if Boot Guard ACM does TPM initialization successfully.
  - 0: No TPM initialization happen in Boot Guard ACM.
  - 1: TPM initialization is done by Boot Guard ACM.
  **/
  BOOLEAN  BypassTpmInit;
  TPM_TYPE TpmType;             ///< Report what TPM device is available on system. 
  BOOLEAN  BootGuardCapability; ///< Value is set to 1 if chipset is Boot Guard capable.
  /**
  Value is set to 1 if microcode failed to load Boot Guard ACM or ENF Shutdown path is taken by ME FW.
  - 0: BIOS TPM code continue with TPM initization based on MeasuredBoot.
  - 1: BIOS TPM code is not to do any futher TPM initization and extends.
  **/
  BOOLEAN  DisconnectAllTpms;
  /**
  It is indicated BIOS TPM code is not to create DetailPCR or AuthorityPCR event log if Sx resume type is S3, 
  Deep-S3, or iFFS Resume.
  - 0: Create TPM event log if not Sx Resume Type.
  - 1: Bypass TPM Event Log if Sx Resume Type is identified.
  **/
  BOOLEAN  ByPassTpmEventLog;
} BOOT_GUARD_INFO;

/**
  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
BOOLEAN
IsBootGuardSupported (
  VOID
  );

/**
  Stop PBE timer if system is in Boot Guard boot

  @retval EFI_SUCCESS        - Stop PBE timer
  @retval EFI_UNSUPPORTED    - Not in Boot GuardSupport mode.
**/
EFI_STATUS
StopPbeTimer (
  VOID
  );

/**
Report platform specific Boot Guard information.

@param[out] *BootGuardInfo - Pointer to BootGuardInfo.
**/

VOID
GetBootGuardInfo(
  OUT BOOT_GUARD_INFO *BootGuardInfo
);

#endif
