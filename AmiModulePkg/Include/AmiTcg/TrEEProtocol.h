//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
// $Header: $
//
// $Revision:  $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: TrEEProtocol.h
//
// Description:
// Header file to TrEE TPM20 Structures
//
//<AMI_FHDR_END>
//*************************************************************************

#ifndef TREE_PROTOCOL_H

#define TREE_PROTOCOL_H
#include <AmiTcg/Tpm20.h>

#define EFI_TREE_SERVICE_BINDING_PROTOCOL_GUID \
{0x4cf01d0a, 0xc48c, 0x4271, 0xa2, 0x2a, 0xad, 0x8e, 0x55, 0x97,\
 0x81, 0x88}

#define EFI_TREE_PROTOCOL_GUID \
{0x607f766c, 0x7455, 0x42be, 0x93, 0x0b, 0xe4, 0xd7, 0x6d, 0xb2,\
 0x72, 0x0f}

// Add for TPM2 Final Event Table, Rev0.4/Rev0.9
#define EFI_TCG2_FINAL_EVENTS_TABLE_GUID \
{ 0x1e2ed096, 0x30e2, 0x4254, {0xbd, 0x89, 0x86, 0x3b, 0xbe, 0xf8, 0x23, 0x25} }

#define EFI_TCG2_FINAL_EVENTS_TABLE_VERSION     1


typedef struct _EFI_TREE_PROTOCOL EFI_TREE_PROTOCOL;

//#pragma pack(push,1) // Byte align structures

typedef struct _TREE_VERSION
{
    UINT8 Major;
    UINT8 Minor;
} TREE_VERSION;

typedef UINT32 TREE_EVENT_LOG_BITMAP;
typedef UINT32 TREE_EVENT_LOG_FORMAT;
#define TREE_EVENT_LOG_FORMAT_TCG_1_2 0x00000001
#define TREE_EVENT_LOG_FORMAT_TCG_2   0x00000002

#define TREE_STRUCTURE_VERSION_MAJOR  (1)
#define TREE_STRUCTURE_VERSION_MINOR  (0)

#define TREE_PROTOCOL_VERSION_MAJOR (1)
#define TREE_PROTOCOL_VERSION_MINOR (0)

typedef struct _TREE_BOOT_SERVICE_CAPABILITY_SHA1
{
    UINT8                   Size;
    TREE_VERSION            StructureVersion;
    TREE_VERSION            ProtocolVersion;
    UINT32                  HashAlgorithmBitmap;
    TREE_EVENT_LOG_BITMAP   SupportedEventLogs;
    BOOLEAN                 TrEEPresentFlag;
    UINT16                  MaxCommandSize;
    UINT16                  MaxResponseSize;
    UINT32                  ManufacturerID;
} TREE_BOOT_SERVICE_CAPABILITY_SHA1;

typedef struct _TREE_BOOT_SERVICE_CAPABILITY
{
    UINT8                   Size;
    TREE_VERSION            StructureVersion;
    TREE_VERSION            ProtocolVersion;
    UINT32                  HashAlgorithmBitmap;
    TREE_EVENT_LOG_BITMAP   SupportedEventLogs;
    BOOLEAN                 TrEEPresentFlag;
    UINT16                  MaxCommandSize;
    UINT16                  MaxResponseSize;
    UINT32                  ManufacturerID;
    UINT32                  NumberOfPcrBanks;
    UINT32                  ActivePcrBanks;
} TREE_BOOT_SERVICE_CAPABILITY;

#define TREE_BOOT_HASH_ALG_SHA1   0x00000001
#define TREE_BOOT_HASH_ALG_SHA256 0x00000002
#define TREE_BOOT_HASH_ALG_SHA384 0x00000004
#define TREE_BOOT_HASH_ALG_SHA512 0x00000008
#define TREE_BOOT_HASH_ALG_SM3    0x00000010

#define TCG2_PROTOCOL_SPEC_TCG_1_2  0x01
#define TCG2_PROTOCOL_SPEC_TCG_2    0x02

#define TREE_EXTEND_ONLY    0x0000000000000001
#define PE_COFF_IMAGE         0x0000000000000010

//#define EV_EFI_VARIABLE_DRIVER_CONFIG 0x80000001
//#define EV_EFI_VARIABLE_AUTHORITY 0x800000E0
typedef UINT32                     TCG_EVENTTYPE;
typedef UINT32                     TCG_PCRINDEX;

#pragma pack(push, 1) // Byte align structures

typedef UINT32 TrEE_PCRINDEX;
typedef UINT32 TrEE_EVENTTYPE;
typedef UINT32 TREE_EVENTLOGTYPE;

typedef struct _TrEE_EVENT_HEADER
{
    UINT32          HeaderSize;
    UINT16          HeaderVersion;
    TrEE_PCRINDEX   PCRIndex;
    TrEE_EVENTTYPE  EventType;
} TrEE_EVENT_HEADER;

typedef struct _TrEE_EVENT
{
    UINT32            Size;
    TrEE_EVENT_HEADER Header;
    UINT8             Event[1];
} TrEE_EVENT;

typedef struct tdTCG_PCClient_PCR_Event2_Hdr
{
    UINT32              pcrIndex;
    UINT32              eventType;
    TPML_DIGEST_VALUES  digests;
    UINT32              eventSize;//UINT32 aligned
} TCG_PCClient_PCR_Event2_Hdr;

typedef struct tdTCG_PCClient_PCR_Event2
{
    UINT32              pcrIndex;
    UINT32              eventType;
    TPML_DIGEST_VALUES  digests;
    UINT32              eventSize;//UINT32 aligned
    BYTE                event[1];
} TCG_PCClient_PCR_Event2;

#pragma pack(pop) // Byte align structures

#define SIZE_OF_TrEE_EVENT OFFSET_OF (TrEE_EVENT, Event)

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_CAPABILITY) (
    IN EFI_TREE_PROTOCOL *This,
    IN OUT TREE_BOOT_SERVICE_CAPABILITY *ProtocolCapability
);

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_EVENT_LOG) (
    IN EFI_TREE_PROTOCOL *This,
    IN TREE_EVENTLOGTYPE EventLogFormat,
    OUT EFI_PHYSICAL_ADDRESS *EventLogLocation,
    OUT EFI_PHYSICAL_ADDRESS *EventLogLastEntry,
    OUT BOOLEAN *EventLogTruncated
);

typedef
EFI_STATUS
(EFIAPI * EFI_TREE_HASH_LOG_EXTEND_EVENT) (
    IN EFI_TREE_PROTOCOL *This,
    IN UINT64 Flags,
    IN EFI_PHYSICAL_ADDRESS DataToHash,
    IN UINT64 DataToHashLen,
    IN TrEE_EVENT *Event
);

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_SUBMIT_COMMAND) (
    IN EFI_TREE_PROTOCOL *This,
    IN UINT32 InputParameterBlockSize,
    IN UINT8 *InputParameterBlock,
    IN UINT32 OutputParameterBlockSize,
    IN UINT8 *OutputParameterBlock
);

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_ACTIVATE_PCR_BANKS)(
    IN  EFI_TREE_PROTOCOL   *This,
    OUT UINT32              *ActivatePcrBanks
);

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_SET_ACTIVATE_PCR_BANKS)(
    IN  EFI_TREE_PROTOCOL   *This,
    OUT UINT32              ActivatePcrBanks
);


typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_RESULT_OF_SET_ACTIVATE_PCR_BANKS)(
    IN  EFI_TREE_PROTOCOL   *This,
    OUT UINT32              *OperationPresent,
    OUT UINT32              *Response
);


struct _EFI_TREE_PROTOCOL
{
    EFI_TREE_GET_CAPABILITY         GetCapability;
    EFI_TREE_GET_EVENT_LOG          GetEventLog;
    EFI_TREE_HASH_LOG_EXTEND_EVENT  HashLogExtendEvent;
    EFI_TREE_SUBMIT_COMMAND         SubmitCommand;
    EFI_TREE_GET_ACTIVATE_PCR_BANKS               GetActivePcrBanks;
    EFI_TREE_SET_ACTIVATE_PCR_BANKS               SetActivePcrBanks;
    EFI_TREE_GET_RESULT_OF_SET_ACTIVATE_PCR_BANKS GetResultOfSetActivePcrBank;
};

extern EFI_GUID gEfiTrEEProtocolGuid;

#endif //TREE_PROTOCOL_H
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
