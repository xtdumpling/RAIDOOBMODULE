/*++
   Module Name:

   TcgDxe.c

   Abstract:

   DXE Driver that provides TCG services

   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgDxe/TcgDxe20.c 2     10/09/13 6:30p Fredericko $
//
// $Revision: 2 $
//
// $Date: 10/09/13 6:30p $
//*************************************************************************
// Revision History
// ----------------
// $Log:
#ifndef TrEE_DXE_H
#define TrEE_DXE_H

#include <Uefi.h>
#include <Token.h>
#include <AmiTcg/Tpm20.h>
#include <AmiTcg/TrEEProtocol.h>

#pragma pack(push,1)

#define AMI_TREE_BOOT_HASH_ALG_SHA1   0x00000001
#define AMI_TREE_BOOT_HASH_ALG_SHA256 0x00000002
#define AMI_TREE_BOOT_HASH_ALG_SHA384 0x00000004
#define AMI_TREE_BOOT_HASH_ALG_SHA512 0x00000008
#define AMI_TREE_BOOT_HASH_ALG_SM3    0x00000010

typedef struct
{
    TPM2_COMMAND_HEADER       Header;
    TPM_RH                    Authhandle;
    UINT32                    AuthorizationSize;
    TPMS_AUTH_COMMAND         AuthInfo;
    TPML_PCR_SELECTION        PcrSelect;
} TPM2_ALLOCATE_PCR_COMMAND;

typedef struct
{
    TPM2_RESPONSE_HEADER       Header;
    TPMS_AUTH_RESPONSE        RspSession;
    TPMI_YES_NO               Success;
    UINT32                    MaxPcr;
    UINT32                    RequiredSize;
    UINT32                    AvailableSize;
} TPM2_ALLOCATE_PCR_RESPONSE;

typedef struct
{
    TPM2_COMMAND_HEADER       Header;
    TPM_CAP                   Capability;
    UINT32                    Property;
    UINT32                    PropertyCount;
} TPM2_GET_CAPABILITY_COMMAND;

typedef struct
{
    TPM2_RESPONSE_HEADER      Header;
    TPMI_YES_NO               MoreData;
    TPMS_CAPABILITY_DATA      CapabilityData;
} TPM2_GET_CAPABILITY_RESPONSE;

typedef struct
{
    BYTE  sha1[SHA1_DIGEST_SIZE];
    BYTE  sha256[SHA256_DIGEST_SIZE];
    BYTE  sha384[SHA384_DIGEST_SIZE];
    BYTE  sha512[SHA512_DIGEST_SIZE];
    BYTE  sm3256[SM3_256_DIGEST_SIZE];
} TPM2_HALG;


typedef struct
{
    TCG_PCRINDEX         PCRIndex;
    TCG_EVENTTYPE        EventType;
    TPML_DIGEST_VALUES   Digests;
    UINT32               EventSize; // UINT32 aligned
} TCG_PCR_EVENT2_HDR;

#if MDE_PKG_VERSION < 10
typedef struct
{
    TCG_PCR_EVENT2_HDR   hdr;
    UINT8                Event[1];
} TCG_PCR_EVENT2;
#endif

typedef struct {
  TPM2_COMMAND_HEADER  Header;
  TPM_SU               ShutdownType;
} TPM2_SHUTDOWN_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER Header;
} TPM2_SHUTDOWN_RESPONSE;

#pragma pack(pop)
typedef struct tdEFI_TCG2_FINAL_EVENTS_TABLE
{
    UINT64              Version;
    UINT64              NumberOfEvents;
    TCG_PCR_EVENT2      Event[1];
} EFI_TCG2_FINAL_EVENTS_TABLE;


typedef struct _AMI_INTERNAL_HLXE_PROTOCOL  AMI_INTERNAL_HLXE_PROTOCOL;
typedef struct _AMI_DXE_HASH_INTERFACE_PROTOCOL  AMI_DXE_HASH_INTERFACE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI * AMI_DXE_HASHALL_INTERFACE) (
    IN  EFI_TREE_PROTOCOL        *TrEEPpi,
    IN  UINT8                    *DataToHash,
    IN  UINTN                    DataSize,
    IN  TPML_DIGEST_VALUES       *HashResults,
    IN  UINT32                   *AlgBitMap
);

typedef
EFI_STATUS
(EFIAPI * AMI_DXE_HASH_INIT_INTERFACE) (
    IN  EFI_TREE_PROTOCOL               *TrEEPpi,
    IN  UINT32                          Alg,
    OUT VOID                            *Context,
    IN OUT UINTN                        *ContextSize
);

typedef
EFI_STATUS
(EFIAPI * AMI_DXE_HASH_UPDATE_INTERFACE) (
    IN  EFI_TREE_PROTOCOL        *TrEEPpi,
    IN  UINT8                    *Data,
    IN  UINTN                     Datasize,
    IN  UINT32                   Alg,
    IN  OUT  VOID                *Context
);

typedef
EFI_STATUS
(EFIAPI * AMI_DXE_HASH_COMPLETE_INTERFACE) (
    IN  EFI_TREE_PROTOCOL             *TrEEPpi,
    IN  VOID                     *Context,
    IN  UINT32                   Alg,
    IN  OUT  UINT8               *resultsbuffer,
    IN  OUT  UINTN               *resultsbuffersize
);

typedef
EFI_STATUS
(EFIAPI * INTERNAL_HASH_LOG_EXTEND_EVENT) (
    IN  EFI_TREE_PROTOCOL        *This,
    IN  UINT8                    *DataToHash,
    IN  UINT64                   Flags,
    IN  UINTN                    DataSize,
    IN  OUT  TCG_PCR_EVENT2_HDR  *NewEventHdr,
    IN  UINT8                    *NewEventData
);

typedef
EFI_STATUS
(EFIAPI * INTERNAL_ALLOCATE_PCR_BANK) (
    IN  UINT32 RequestedBank,
    IN  UINT32 ActivePCRBanks,
    IN  UINT32 Supported,
    IN  OUT UINT32 *TpmErrorCode
);

struct _AMI_INTERNAL_HLXE_PROTOCOL
{
    INTERNAL_HASH_LOG_EXTEND_EVENT  AmiHashLogExtend2;
    INTERNAL_ALLOCATE_PCR_BANK      AmiAllocatePcrBank;
};


struct _AMI_DXE_HASH_INTERFACE_PROTOCOL
{
    AMI_DXE_HASHALL_INTERFACE  HashAll;
    AMI_DXE_HASH_INIT_INTERFACE Init;
    AMI_DXE_HASH_UPDATE_INTERFACE Update;
    AMI_DXE_HASH_COMPLETE_INTERFACE GetHashResults;
};
#endif

