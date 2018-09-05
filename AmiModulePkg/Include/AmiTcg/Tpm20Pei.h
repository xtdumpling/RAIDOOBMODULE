#ifndef TPM_2_0_PEI_H
#define TPM_2_0_PEI_H

#include <AmiTcg/Tpm20.h>
#include <Ppi/AmiTreePpi.h>
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER <= 0xb)
#include <AmiTcg/Tpm20.h>
#endif
#include <PiPei.h>

#pragma pack(push, 1)

typedef struct
{
    BYTE  sha1[SHA1_DIGEST_SIZE];
    BYTE  sha256[SHA256_DIGEST_SIZE];
    BYTE  sm3_256[SM3_256_DIGEST_SIZE];
    BYTE  sha384[SHA384_DIGEST_SIZE];
    BYTE  sha512[SHA512_DIGEST_SIZE];
} TPM2_HALG;

typedef struct
{
    TCG_PCRINDEX         PCRIndex;
    TCG_EVENTTYPE        EventType;
    TPML_DIGEST_VALUES   Digests;
    UINT32               EventSize; // UINT32 aligned
} TCG_PCR_EVENT2_HDR;

typedef struct _TPM2_Startup
{
    TPMI_ST_COMMAND_TAG tag;
    UINT32              CommandSize;
    TPM_CC              CommandCode;
    TPM_SU              StartupType;
} TPM2_Startup_Cmd;

typedef struct _TPM2_SelfTest
{
    TPMI_ST_COMMAND_TAG tag;
    UINT32              CommandSize;
    TPM_CC              CommandCode;
    TPMI_YES_NO         SelfTestType;
} TPM2_SelfTest;

typedef struct _TPM2_Startup_Resp
{
    TPM_ST              tag;
    UINT32              ResponsSize;
    TPM_RC              ResponseCode;
} TPM2_Common_RespHdr;

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

#pragma pack(pop)

#define AMI_TREE_BOOT_HASH_ALG_SHA1   0x00000001
#define AMI_TREE_BOOT_HASH_ALG_SHA256 0x00000002
#define AMI_TREE_BOOT_HASH_ALG_SHA384 0x00000004
#define AMI_TREE_BOOT_HASH_ALG_SHA512 0x00000008
#define AMI_TREE_BOOT_HASH_ALG_SM3    0x00000010


EFI_STATUS Tpm20PeiSendStartup(IN CONST EFI_PEI_SERVICES **PeiServices,
                               AMI_TREE_PPI *TrEEPeiPpi, EFI_BOOT_MODE BootMode);

typedef EFI_STATUS (TPM20_MEASURE_CRTM_VERSION_PEI_FUNC_PTR)(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN AMI_TREE_PPI *TrEEppi
);

extern TPM20_MEASURE_CRTM_VERSION_PEI_FUNC_PTR        *Tpm20MeasureCRTMVersionFuncPtr;
typedef struct _AMI_INTERNAL_HLXE_PPI  AMI_INTERNAL_HLXE_PPI;
typedef struct _AMI_PEI_HASH_INTERFACE_PPI  AMI_PEI_HASH_INTERFACE_PPI;

typedef
EFI_STATUS
(EFIAPI * INTERNAL_HASH_LOG_EXTEND_EVENT) (
    IN CONST EFI_PEI_SERVICES    **PeiServices,
    IN  AMI_TREE_PPI             *TrEEPpi,
    IN  UINT8                    *DataToHash,
    IN  UINT64                   Flags,
    IN  UINTN                    DataSize,
    IN  OUT  TCG_PCR_EVENT2_HDR  *NewEventHdr,
    IN  UINT8                    *NewEventData
);

typedef
EFI_STATUS
(EFIAPI * AMI_PEI_HASHALL_INTERFACE) (
    IN  AMI_TREE_PPI             *TrEEPpi,
    IN  UINT8                    *DataToHash,
    IN  UINTN                    DataSize,
    IN  TPML_DIGEST_VALUES       *HashResults,
    IN  UINT32                   *AlgBitMap
);

typedef
EFI_STATUS
(EFIAPI * AMI_PEI_HASH_INIT_INTERFACE) (
    IN  AMI_TREE_PPI             *TrEEPpi,
    IN  UINT32                   Alg,
    OUT VOID                     *Context,
    IN OUT UINTN                    *ContextSize
);

typedef
EFI_STATUS
(EFIAPI * AMI_PEI_HASH_UPDATE_INTERFACE) (
    IN  AMI_TREE_PPI             *TrEEPpi,
    IN  UINT8                    *Data,
    IN  UINTN                     Datasize,
    IN  UINT32                   Alg,
    IN  OUT  VOID                *Context
);

typedef
EFI_STATUS
(EFIAPI * AMI_PEI_HASH_COMPLETE_INTERFACE) (
    IN  AMI_TREE_PPI             *TrEEPpi,
    IN  VOID                     *Context,
    IN  UINT32                   Alg,
    IN  OUT  UINT8               *resultsbuffer,
    IN  OUT  UINTN               *resultsbuffersize
);



struct _AMI_INTERNAL_HLXE_PPI
{
    INTERNAL_HASH_LOG_EXTEND_EVENT  AmiHashLogExtendEx;
};

struct _AMI_PEI_HASH_INTERFACE_PPI
{
    AMI_PEI_HASHALL_INTERFACE  HashAll;
    AMI_PEI_HASH_INIT_INTERFACE Init;
    AMI_PEI_HASH_UPDATE_INTERFACE Update;
    AMI_PEI_HASH_COMPLETE_INTERFACE GetHashResults;
};


#endif
