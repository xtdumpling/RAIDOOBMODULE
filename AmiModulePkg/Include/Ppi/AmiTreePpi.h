//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  Trusted Execution Environment (TrEE) PPI Definitions.
**/
#ifndef __AMI_TREE_PPI__H__
#define __AMI_TREE_PPI__H__

#pragma pack(push, 1)

#define AMI_TREE_PPI_GUID\
    {0x86f5680a, 0x155c, 0x4bc8,  { 0xac, 0x77, 0x57, 0x38, 0x48, 0xe2, 0xad, 0x3d } }

typedef struct _AMI_TREE_PPI AMI_TREE_PPI;

typedef struct _AMI_TREE_VERSION{
    UINT8 Major;
    UINT8 Minor;
} AMI_TREE_VERSION;

typedef UINT32 AMI_TREE_EVENT_LOG_BITMAP;
typedef UINT32 AMI_TREE_EVENT_LOG_FORMAT;

#define AMI_TREE_EVENT_LOG_FORMAT_TCG_1_2 0x00000001

#define AMI_TREE_STRUCTURE_VERSION_MAJOR  1
#define AMI_TREE_STRUCTURE_VERSION_MINOR  0

#define AMI_TREE_PROTOCOL_VERSION_MAJOR 1
#define AMI_TREE_PROTOCOL_VERSION_MINOR 0

typedef struct _AMI_TREE_BOOT_SERVICE_CAPABILITY {
    UINT8                       Size;
    AMI_TREE_VERSION            StructureVersion;
    AMI_TREE_VERSION            ProtocolVersion;
    UINT32                      HashAlgorithmBitmap;
    AMI_TREE_EVENT_LOG_BITMAP   SupportedEventLogs;
    BOOLEAN                     TreePresentFlag;
    UINT16                      MaxCommandSize;
    UINT16                      MaxResponseSize;
    UINT32                      ManufacturerID;
} AMI_TREE_BOOT_SERVICE_CAPABILITY;

#define AMI_TREE_BOOT_HASH_ALG_SHA1   0x00000001
#define AMI_TREE_BOOT_HASH_ALG_SHA256 0x00000002
#define AMI_TREE_BOOT_HASH_ALG_SHA384 0x00000004
#define AMI_TREE_BOOT_HASH_ALG_SHA512 0x00000008


typedef UINT32 AMI_TREE_PCRINDEX;
typedef UINT32 AMI_TREE_EVENTTYPE;
typedef UINT32 AMI_TREE_EVENTLOGTYPE;


typedef struct _AMI_TREE_EVENT_HEADER {
    UINT32              HeaderSize;
    UINT16              HeaderVersion;
    AMI_TREE_PCRINDEX   PcrIndex;
    AMI_TREE_EVENTTYPE  EventType;
} AMI_TREE_EVENT_HEADER;

typedef struct _AMI_TREE_EVENT {
    UINT32            Size;
    AMI_TREE_EVENT_HEADER Header;
    UINT8             Event[1];
} AMI_TREE_EVENT;

#pragma pack(pop)

#define AMI_SIZE_OF_TREE_EVENT OFFSET_OF(AMI_TREE_EVENT, Event)

typedef
EFI_STATUS
(EFIAPI *AMI_TREE_GET_CAPABILITY) ( 
    IN AMI_TREE_PPI *This,
    IN OUT AMI_TREE_BOOT_SERVICE_CAPABILITY *ProtocolCapability
);

typedef
EFI_STATUS
(EFIAPI *AMI_TREE_GET_EVENT_LOG) (
    IN AMI_TREE_PPI *This,
    IN AMI_TREE_EVENTLOGTYPE EventLogFormat,
    OUT EFI_PHYSICAL_ADDRESS *EventLogLocation,
    OUT EFI_PHYSICAL_ADDRESS *EventLogLastEntry,
    OUT BOOLEAN *EventLogTruncated
);

typedef
EFI_STATUS
(EFIAPI * AMI_TREE_HASH_LOG_EXTEND_EVENT) (
    IN AMI_TREE_PPI *This,
    IN UINT64 Flags,
    IN EFI_PHYSICAL_ADDRESS DataToHash,
    IN UINT64 DataToHashLen,
    IN AMI_TREE_EVENT *Event
);

typedef
EFI_STATUS
(EFIAPI *AMI_TREE_SUBMIT_COMMAND) (
    IN AMI_TREE_PPI *This,
    IN UINT32 InputParameterBlockSize,
    IN UINT8 *InputParameterBlock,
    IN UINT32 OutputParameterBlockSize,
    IN UINT8 *OutputParameterBlock
);

struct _AMI_TREE_PPI {
    AMI_TREE_GET_CAPABILITY         GetCapability;
    AMI_TREE_GET_EVENT_LOG          GetEventLog;
    AMI_TREE_HASH_LOG_EXTEND_EVENT  HashLogExtendEvent;
    AMI_TREE_SUBMIT_COMMAND         SubmitCommand;
};

extern EFI_GUID gAmiTreePpiGuid;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
