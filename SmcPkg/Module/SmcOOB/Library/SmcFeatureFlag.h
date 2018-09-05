//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
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
//    Date:     May/31/2016
//
//****************************************************************************

#ifndef	_H_SmcFeatureFlag_
#define	_H_SmcFeatureFlag_

#include <Guid/SmcOobPlatformPolicy.h>

#define PAYLOAD_SIZE 24

#pragma pack(1)

typedef struct {
    UINT8	MajorVersion;
    UINT16	MinorVersion;
    UINT16	Length;
    UINT8	Reserved[9];
    UINT8	PayloadChecksum;
    UINT8	HeaderChecksum;
}PlatInfo_H;


VOID
FsfPayload(
    IN OUT	UINT8	*Payload,
    IN OUT	UINT16	*PayloadLength,
    IN      SMC_OOB_PLATFORM_POLICY *SmcOobPlatformPolicy
);

VOID
GenerateFeatureSupportFlags(
    IN	UINT8	*Buffer,
    IN OUT	UINT32	*TotalLength,
    IN      SMC_OOB_PLATFORM_POLICY *SmcOobPlatformPolicy
);

#pragma pack()

#endif
