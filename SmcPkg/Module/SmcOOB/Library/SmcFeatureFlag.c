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
//  Rev. 1.04
//    Bug Fix : N/A
//    Reason  : Updated SUM Feature Flag specification revision to 1.6 based 
//              on SUM_FeatureSupportFlagsSpecification_V1.6_rev20170930_1.docx 
//              for SMC OOB TPM provisioning via IPMI.
//    Auditor : Joe Jhang
//    Date    : Nov/07/2017
//
//  Rev. 1.03
//    Bug Fix : N/A
//    Reason  : Updated to SUM Feature Flag specification revision 1.5.
//    Auditor : Joe Jhang
//    Date    : May/25/2017
//
//  Rev. 1.02
//    Bug Fix:  Update to SUM Feature Flag spec 1.4.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Sep/23/2016
//
//  Rev. 1.01
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/26/2016
//
//****************************************************************************

#include "Token.h"
#include "SmcRomHole.h"
#include "SmcFeatureFlag.h"

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  PlatformInfoForSum
//
// Description:
//   Platform Information
//
// Input:
//      IN OUT UINT8* Payload
//
// Output:
//      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
FsfPayload(
    IN OUT	UINT8	*Payload,
    IN OUT	UINT16	*PayloadLength,
    IN      SMC_OOB_PLATFORM_POLICY *SmcOobPlatformPolicy
)
{
    UINT32	ii;
    SUM_FEATURE_FLAGS *FeatureFlags;

    FeatureFlags = &(SmcOobPlatformPolicy->SumFeatureFlags);

    for(ii = 0; ii < PAYLOAD_SIZE; ii++)
	*(Payload + ii) = 0;

if (FeatureFlags->SumPlatformId)
    *(Payload + 0) |= (FeatureFlags->SumPlatformId & 0x7f);

if (FeatureFlags->SumWithWithoutBmc)
    *(Payload + 0) |= (FeatureFlags->SumWithWithoutBmc & 1) << 7;

if (FeatureFlags->SumInbandJumperlessUpdateBios)
    *(Payload + 4) |= (FeatureFlags->SumInbandJumperlessUpdateBios & 1) << 1;

if (FeatureFlags->SumBiosConfigurationSupportOutband)
    *(Payload + 4) |= (FeatureFlags->SumBiosConfigurationSupportOutband & 1) << 3;

if (FeatureFlags->SumBiosConfigurationSupportInband)
    *(Payload + 4) |= (FeatureFlags->SumBiosConfigurationSupportInband & 1) << 4;
    
if (FeatureFlags->SumMeDisablingUpdateBios)
    *(Payload + 4) |= (FeatureFlags->SumMeDisablingUpdateBios & 1) << 5;

if (FeatureFlags->SumEditDmiSupportOutband)
    *(Payload + 5) |= (FeatureFlags->SumEditDmiSupportOutband & 1) << 1;

if (FeatureFlags->SumEditDmiSupportInband)
    *(Payload + 5) |= (FeatureFlags->SumEditDmiSupportInband & 1) << 2;

if (FeatureFlags->SumPresistentDmiBiosCfgSupportOutband)
    *(Payload + 6) |= (FeatureFlags->SumPresistentDmiBiosCfgSupportOutband & 1) << 1;

if (FeatureFlags->SumFullSmbiosSupportOutband)
    *(Payload + 10) |= (FeatureFlags->SumFullSmbiosSupportOutband & 1) << 1;

if (FeatureFlags->SumFullSmbiosSupportInband)
    *(Payload + 10) |= (FeatureFlags->SumFullSmbiosSupportInband & 1) << 2;

if (FeatureFlags->SumVpdSupportOutband)
    *(Payload + 10) |= (FeatureFlags->SumVpdSupportOutband & 1) << 4;

if (FeatureFlags->SumVpdSupportInband)
    *(Payload + 10) |= (FeatureFlags->SumVpdSupportInband & 1) << 5;

if (FeatureFlags->SumHddInformationBiosSupport)
    *(Payload + 10) |= (FeatureFlags->SumHddInformationBiosSupport & 1) << 6;
    
if (FeatureFlags->SumMctpFwUpdateBiosSupport)
    *(Payload + 11) |= (FeatureFlags->SumMctpFwUpdateBiosSupport & 1) << 5;
    
if (FeatureFlags->SumVpdType40BiosSupport)
    *(Payload + 11) |= (FeatureFlags->SumVpdType40BiosSupport & 1) << 6;

if (FeatureFlags->SumFru1DataBiosSupport)
    *(Payload + 12) |= (FeatureFlags->SumFru1DataBiosSupport & 1) << 0;

if (FeatureFlags->SumIntelRcEventLogSupport)
    *(Payload + 14) |= (FeatureFlags->SumIntelRcEventLogSupport & 1) << 1;

if (FeatureFlags->SumTpmProvisionSupport)
    *(Payload + 16) |= (FeatureFlags->SumTpmProvisionSupport & 1) << 1;

if (FeatureFlags->SumAutoTpmProvisionSupport)
    *(Payload + 16) |= (FeatureFlags->SumAutoTpmProvisionSupport & 1) << 3;

if (FeatureFlags->SumTpmCustomProvisionTableSupport)
    *(Payload + 16) |= (FeatureFlags->SumTpmCustomProvisionTableSupport & 1) << 5;

if (FeatureFlags->SumTpmV12Support)
    *(Payload + 17) |= (FeatureFlags->SumTpmV12Support & 1) << 0;

if (FeatureFlags->SumTpmV20Support)
    *(Payload + 17) |= (FeatureFlags->SumTpmV20Support & 1) << 1;

if (FeatureFlags->SumDefaultPreserveSmbiosSupport)
    *(Payload + 18) |= (FeatureFlags->SumDefaultPreserveSmbiosSupport & 1) << 0;

if (FeatureFlags->SumDefaultProgramNvramSupport)
    *(Payload + 18) |= (FeatureFlags->SumDefaultProgramNvramSupport & 1) << 1;

if (FeatureFlags->SumDefaultUpdateMeSupport)
    *(Payload + 18) |= (FeatureFlags->SumDefaultUpdateMeSupport & 1) << 2;

    *PayloadLength = PAYLOAD_SIZE; // Follow spec.
}

VOID
GenerateFeatureSupportFlags(
    IN	UINT8	*Buffer,
    IN OUT	UINT32	*TotalLength,
    IN      SMC_OOB_PLATFORM_POLICY *SmcOobPlatformPolicy
)
{
    PlatInfo_H	*PlatInfoHeader;
    UINT16	PayloadLength = 0;
    UINT8	*pData = NULL;
    UINT32	Checksum = 0, ii = 0;

    PlatInfoHeader = (PlatInfo_H*)Buffer;
    PlatInfoHeader->MajorVersion = SmcOobPlatformPolicy->SumFeatureFlags.SumFeatureFlagMajorVersion;
    PlatInfoHeader->MinorVersion = SmcOobPlatformPolicy->SumFeatureFlags.SumFeatureFlagMinorVersion;

    pData = Buffer + sizeof(PlatInfo_H);
    FsfPayload(pData, &PayloadLength, SmcOobPlatformPolicy);

    PlatInfoHeader->Length = PayloadLength;

    Checksum = 0;
    for (ii = 0; ii < PAYLOAD_SIZE; ii++)
        Checksum += *(pData + ii);

    PlatInfoHeader->PayloadChecksum = (UINT8)Checksum;

    Checksum = 0;
    pData = ((UINT8*)PlatInfoHeader);
    for (ii = 0; ii < sizeof (PlatInfo_H) - 1; ii++)
        Checksum += *(pData + ii);

    PlatInfoHeader->HeaderChecksum = (UINT8)Checksum;

    *TotalLength = PayloadLength + sizeof(PlatInfo_H);
}
