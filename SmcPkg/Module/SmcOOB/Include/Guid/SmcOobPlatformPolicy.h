//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.03
//    Bug Fix : N/A
//    Reason  : Add a guid to inform InBand prepare collect data.
//    Auditor : Durant Lin
//    Date    : Aug/14/2018
//
//  Rev. 1.02
//    Bug Fix : N/A
//    Reason  : Move Variable_GUID to PlatformPolicy.
//    Auditor : Durant Lin
//    Date    : Aug/07/2018
//
//  Rev. 1.01
//    Bug Fix : N/A
//    Reason  : Updated SUM Feature Flag specification revision to 1.6 based 
//              on SUM_FeatureSupportFlagsSpecification_V1.6_rev20170930_1.docx 
//              for SMC OOB TPM provisioning via IPMI.
//    Auditor : Joe Jhang
//    Date    : Nov/07/2017
//
//  Rev. 1.00
//    Bug fixed: Initial SMC_OOB_PLATFORM_POLICY for SMC OOB module binary release.
//    Reason   :
//    Auditor  : Jason Hsu
//    Date     : Jun/06/09
//
//****************************************************************************

#ifndef _SMC_OOB_PLATFORM_POLICY_H_
#define _SMC_OOB_PLATFORM_POLICY_H_
        
#define SMC_OOB_PLATFORM_POLICY_GUID \
  { \
    0x27723F8B, 0x25A8, 0x4DA1, {0xA3, 0xFC, 0x7B, 0x30, 0xE9, 0x87, 0x1D, 0xC7} \
  }

#define SMC_OOB_PLATFORM_POLICY_CALLBACK_GUID \
  { \
    0x501C5B87, 0x8181, 0x4AE5, {0xA6, 0xB2, 0xD0, 0x5A, 0xCE, 0x0D, 0x83, 0xDE} \
  }

//
// Forward Declaration
//
typedef struct _SMC_OOB_PLATFORM_POLICY SMC_OOB_PLATFORM_POLICY;

//
// Revision 1: First Version
//
#define SMC_OOB_PLATFORM_POLICY_REVISION_1     1

//
// PlatformId: OOB supported platform ID
//
typedef enum {
  OobDefault,
  OobPurley,
  OobBakerville
} OOB_PLATFORM_ID;

typedef struct _SMC_ROOT_FORM_DATA {
  EFI_GUID           Guid;
  UINT16             FormId;
  CHAR8              FormTitle[64];
  EFI_IFR_OP_HEADER* OpPointer;
} SMC_ROOT_FORM_DATA;

#pragma pack(1)
typedef struct {
  UINT8              SumFeatureFlagMajorVersion;
  UINT16             SumFeatureFlagMinorVersion;
  UINT8              SumPlatformId;
  UINT8              SumWithWithoutBmc;
  UINT8              SumInbandJumperlessUpdateBios;
  UINT8              SumBiosConfigurationSupportOutband;
  UINT8              SumBiosConfigurationSupportInband;
  UINT8              SumMeDisablingUpdateBios;
  UINT8              SumEditDmiSupportOutband;
  UINT8              SumEditDmiSupportInband;
  UINT8              SumPresistentDmiBiosCfgSupportOutband;
  UINT8              SumFullSmbiosSupportOutband;
  UINT8              SumFullSmbiosSupportInband;
  UINT8              SumVpdSupportOutband;
  UINT8              SumVpdSupportInband;
  UINT8              SumHddInformationBiosSupport;
  UINT8              SumMctpFwUpdateBiosSupport;  
  UINT8              SumVpdType40BiosSupport;
  UINT8              SumFru1DataBiosSupport;
  UINT8              SumIntelRcEventLogSupport;
  UINT8              SumTpmProvisionSupport;
  UINT8              SumAutoTpmProvisionSupport;
  UINT8              SumTpmV12Support;
  UINT8              SumTpmV20Support;
  UINT8              SumDefaultPreserveSmbiosSupport;
  UINT8              SumDefaultProgramNvramSupport;
  UINT8              SumDefaultUpdateMeSupport;
  UINT8              SumTpmCustomProvisionTableSupport;
  UINT8              Reserved[71];
} SUM_FEATURE_FLAGS;

typedef struct {
  UINT8              Md5Support;
  UINT16             AssetInfoVersion;
  UINT16             BiosCfgVersion;
  UINT16             HiiVersion;
  UINT16             SmbiosVersion;
  UINT16             FullSmbiosVersion;
  UINT8              SmbiosMajorVersion;
  UINT8              SmbiosMinorVersion;
  UINT8              SmbiosDocVersion;
  UINT8              InBandSwSmiE6;
  UINT8              InBandSwSmiE7;
  UINT8              InBandSwSmiE8;
  UINT32             CmosSmcAddrHH;
  UINT32             CmosSmcAddrH;
  UINT32             CmosSmcAddrL;
  UINT32             CmosSmcAddrLL;
  SMC_ROOT_FORM_DATA *RootForm;
  UINT8              RootFormCount;
  SMC_ROOT_FORM_DATA *HideFormForSum;
  UINT8              HideFormCount;
  UINT32             BoardInfoSvid;
  UINT32             BoardInfoDate;
  UINT16             BoardInfoTime;
  UINT32             OobFileStatus0; // For Md5Support
  UINT32             OobFileStatus1; // For Md5Support
  UINT32             OobFileStatus2; // For Md5Support
  UINT32             OobFileStatus3; // For Md5Support
  UINT8              Reserved[691];
} OOB_CONFIGURATIONS;

typedef struct {
  UINT32             IpmiLanSelection;
  UINT8              Reserved[508];
} OOB_REF_VARIABLES;

typedef struct {
    CHAR16		*Var;
    EFI_GUID	Guid;
}Variable_GUID;


//
// SMC OOB Platform Policy
//
typedef struct _SMC_OOB_PLATFORM_POLICY {
  UINT32                PolicyRevision;
  UINT32                PlatformId;
  SUM_FEATURE_FLAGS     SumFeatureFlags;
  OOB_CONFIGURATIONS    OobConfigurations;
  OOB_REF_VARIABLES     OobRefVariables;
  Variable_GUID*		PlatformVar;
};



#pragma pack()

extern EFI_GUID gSmcOobPlatformPolicyGuid;
extern EFI_GUID gSmcOobPlatformPolicyCallbackGuid;
extern EFI_GUID gSmcOobInBandCombineDataNotify;

#endif

