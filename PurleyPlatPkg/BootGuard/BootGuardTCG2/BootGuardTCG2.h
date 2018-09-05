//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file BootGuardTCG2.h
    TPM Initialization Flow for Boot Guard

**/
//**********************************************************************

#ifndef _BOOT_GUARD_TCG2_H_
#define _BOOT_GUARD_TCG2_H_

//
// Define macros to build data structure signatures from characters.
//
#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#define EFI_SIGNATURE_64(A, B, C, D, E, F, G, H) \
    (EFI_SIGNATURE_32 (A, B, C, D) | ((UINT64) (EFI_SIGNATURE_32 (E, F, G, H)) << 32))

#define IBB_ENTRYPOINT_M  0xFFFFFFC0
#define ACM_STATUS (*(UINT32*)0xFED30328)
#define M_KEYHASH_ADDR      0xFED30400

// Define the FIT Entry Type
#define FIT_HEADER_TYPE             0x00
#define FIT_MICROCODE_TYPE          0x01
#define FIT_ACM_TYPE                0x02
#define FIT_KM_TYPE                 0x0B
#define FIT_BPM_TYPE                0x0C

//
// BtG related definitions
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
#define R_CPU_BOOT_GUARD_ACM_STATUS                                   0x328
#define V_CPU_BOOT_GUARD_LOAD_ACM_SUCCESS                             0x8000000000000000
#define B_BOOT_GUARD_ACM_ERRORCODE_MASK                               0x00007FF0
#define PURLEY_BOOTGUARD_POLICY_STS                                   0x032C

//
// Manifest definition
//
#define TPM_ALG_SHA1        0x4
#define TPM_ALG_SHA256      0xB
#define SHA1_DIGEST_SIZE    20
#define SHA256_DIGEST_SIZE  32
#define ACM_HEADER_FLAG_DEBUG_SIGNED     BIT15
#define ACM_NPW_SVN                      0x2

#pragma pack(push, 1)

typedef struct _BOOT_POLICY
{
    UINT8            RSTR;
    UINT8            TYPE;
    UINT16           ACM_SVN;
    UINT8            ACM_Signature[256];
    UINT8            Key_Manifest_Signature[256];
    UINT8            Boot_Policy_Manifest_Signature[256];
    UINT8            Digest_of_Hashed_IBB_Segment[32];
} BOOT_POLICY;

typedef struct _BOOT_POLICY_AUTHORITY
{
    UINT8           RSTR;
    UINT8           TYPE;
    UINT16          ACM_SVN;
    UINT8           BootGuardPkHash[32];
    UINT8           KMPkHash[32];
    UINT8           BPKeyHash[32];
}BOOT_POLICY_AUTHORITY;

typedef struct _FIT_ENTRY
{
    UINT64           TblAddress;
    UINT32           TblSIZE;
    UINT16           TblVer;
    UINT8            TblType;
    UINT8            TblChkSum;
} FIT_ENTRY;

typedef struct {
    UINT16   HashAlg;
    UINT16   Size;
    UINT8    HashBuffer[SHA256_DIGEST_SIZE];
} HASH_STRUCTURE;

#define RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT   (RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT/8)
#define RSA_PUBLIC_KEY_STRUCT_KEY_EXPONENT_DEFAULT  0x10001 // NOT 0x10001
typedef struct {
    UINT8    Version;
    UINT16   KeySize;
    UINT32   Exponent;
    UINT8    Modulus[RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT];
} RSA_PUBLIC_KEY_STRUCT;

#define RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT   (RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT/8)
typedef struct {
    UINT8    Version;
    UINT16   KeySize;
    UINT16   HashAlg;
    UINT8    Signature[RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT];
} RSASSA_SIGNATURE_STRUCT;

typedef struct {
    UINT8                    Version;
    UINT16                   KeyAlg;
    RSA_PUBLIC_KEY_STRUCT    Key;
    UINT16                   SigScheme;
    RSASSA_SIGNATURE_STRUCT  Signature;
} KEY_SIGNATURE_STRUCT;

typedef struct {
    UINT8              StructureID[8];
    UINT8              StructVersion;
    UINT8              HdrStructVersion;
    UINT8              PMBPMVersion;
    UINT8              BPSVN_BPM;
    UINT8              ACMSVN_BPM;
    UINT8              Reserved;
    UINT16             NEMDataStack;
} BOOT_POLICY_MANIFEST_HEADER;

typedef struct {
    UINT16   Reserved;
    UINT16   Flags;
    UINT32   Base;
    UINT32   Size;
} IBB_SEGMENT_ELEMENT;

typedef struct {
    UINT8               StructureID[8];
    UINT8               StructVersion;
    UINT8               SetNumber;
    UINT8               Reserved;
    UINT8               PBETValue;
    UINT32              Flags;
    UINT64              IBB_MCHBAR;
    UINT64              VTD_BAR;
    UINT32              PMRL_Base;
    UINT32              PMRL_Limit;
    UINT64              PMRH_Base;
    UINT64              PMRH_Limit;
    HASH_STRUCTURE      PostIbbHash;
    UINT32              EntryPoint;
    HASH_STRUCTURE      Digest;
    UINT8               SegmentCount; // 1 ~ 8
//    IBB_SEGMENT_ELEMENT IBBSegment[1];    // reverse for dynamic offset
} IBB_ELEMENT;

typedef struct {
    UINT8    StructureID[8];
    UINT8    StructVersion;
    UINT16   PMDataSize;
//UINT8    PMData[PMDataSize];
} PLATFORM_MANUFACTURER_ELEMENT;

typedef struct {
    UINT8                 StructureID[8];
    UINT8                 StructVersion;
    KEY_SIGNATURE_STRUCT  KeySignature;
} BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT;

typedef struct {
    BOOT_POLICY_MANIFEST_HEADER                   Bpm_Header;
    IBB_ELEMENT                                   Ibb_Element;
    //PLATFORM_MANUFACTURER_ELEMENT                 Platform_Manufacture_Element;
    BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT        Bpm_Signature_Element;
} BpmStruct;

typedef struct {
    UINT8                  StructureID[8];
    UINT8                  StructVersion;
    UINT8                  KeyManifestVersion;
    UINT8                  KMSVN;
    UINT8                  KeyManifestID;
    HASH_STRUCTURE         BPKey;
    KEY_SIGNATURE_STRUCT   KeyManifestSignature;
} KEY_MANIFEST_STRAUCTURE;

typedef struct {
  UINT32 ModuleType;
  UINT32 HeaderLen;
  UINT32 HeaderVersion;
  UINT16 ChipsetId;
  UINT16 Flags;
  UINT32 ModuleVendor;
  UINT32 Date;
  UINT32 Size;
  UINT16 AcmSvn;
  UINT16 Reserved1;
  UINT32 CodeControl;
  UINT32 ErrorEntryPoint;
  UINT32 GdtLimit;
  UINT32 GdtBasePtr;
  UINT32 SegSel;
  UINT32 EntryPoint;
  UINT8  Reserved2[64];
  UINT32 KeySize;
  UINT32 ScratchSize;
  UINT8  RsaPubKey[64 * 4];
  UINT32 RsaPubExp;
  UINT8  RsaSig[256];
} ACM_FORMAT;

//
// Boot Policy Restrictions definition
//
typedef union {
  struct {
    UINT8 Facb     : 1;
    UINT8 Dcd      : 1;
    UINT8 Dbi      : 1;
    UINT8 Pbe      : 1;
    UINT8 Bbp      : 1;
    UINT8 Reserved : 2;
    UINT8 BpInvd   : 1;
  } Bits;
  UINT8 Data;
} BP_RSTR_LOW;

//
// Boot Policy Type definition
//
typedef union {
  struct {
    UINT8 MeasuredBoot : 1;
    UINT8 VerifiedBoot : 1;
    UINT8 Hap          : 1;
    UINT8 Reserved     : 5;
  } Bits;
  UINT8    Data;
} BP_TYPE_LOW;

//
// Efi Startup Locality Event Data
//
typedef struct {
  UINT8   Signature[16];
  UINT8   StartupLocality;
} TCG_EFI_STARTUP_LOCALITY_EVENT;

// For TPM20 Define +>
typedef struct _tdTpm2EventHeader {
    UINT32      HeaderSize;
    UINT16      HeaderVersion;
    UINT32      PCRIndex;
    UINT32      EventType;
} tdTpm2EventHeader;

#pragma pack(pop)

//
// ACM definition
//
#define MMIO_ACM_STATUS  (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS)

// For TCG Protocol Define

// For TPM12 Define +>

#define PEI_TCG_PPI_GUID  \
    {0x177d39d2, 0x43b8, 0x40c8, 0x9a, 0xe1, 0x3c, 0x51, 0x98, 0xd6, 0x94, 0x1e}

typedef
EFI_STATUS
(EFIAPI * PEI_TCG_HASH_LOG_EXTEND_EVENT)(
    IN VOID             *This,
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT8            *HashData,
    IN UINT32           HashDataLen,
    IN UINT8            *NewEvent,
    OUT UINT32          *EventNum
);

typedef struct _PEI_TCG_PPI
{
    PEI_TCG_HASH_LOG_EXTEND_EVENT   TCGHashLogExtendEvent;
    VOID                            *TCGLogEvent;
    VOID                            *TCGPassThroughToTpm;
} PEI_TCG_PPI;

typedef struct _tdTpm2Event {
    UINT32              Size;
    tdTpm2EventHeader   Header;
    UINT8               Event[1];
} tdTpm2Event;

typedef
EFI_STATUS
(EFIAPI * AMI_TREE_HASH_LOG_EXTEND_EVENT) (
    VOID                    *This,
    UINT64                  Flags,
    EFI_PHYSICAL_ADDRESS    DataToHash,
    UINT64                  DataToHashLen,
    tdTpm2Event             *Event
);

typedef struct _AMI_TREE_PPI {
    VOID*                           GetCapability;
    VOID*                           GetEventLog;
    AMI_TREE_HASH_LOG_EXTEND_EVENT  HashLogExtendEvent;
    VOID*                           SubmitCommand;
} AMI_TREE_PPI;

#define TREE_BOOT_HASH_ALG_SHA1   0x00000001
#define TREE_BOOT_HASH_ALG_SHA256 0x00000002
#define TREE_BOOT_HASH_ALG_SHA384 0x00000004
#define TREE_BOOT_HASH_ALG_SHA512 0x00000008
#define TREE_BOOT_HASH_ALG_SM3    0x00000010

#define TCG2_PROTOCOL_SPEC_VERSION_1_0  0x01
#define TCG2_PROTOCOL_SPEC_VERSION_1_X  0x02


#ifndef EFI_TCG_LOG_HOB_GUID
#define EFI_TCG_LOG_HOB_GUID \
    {0x5f7d4e0e, 0x3d6d, 0x42bc, 0xa9, 0x42, 0xe, 0x91, 0xe8, 0x3e, 0x3c, 0x31}
#endif

#ifndef EFI_TCG_TREE_LOG_HOB_GUID
#define EFI_TCG_TREE_LOG_HOB_GUID \
    {0x711e7d64, 0x1131, 0x494b, 0xa2, 0xd1, 0x84, 0x80, 0x6c, 0xd7, 0x2d, 0x53}
#endif

#ifndef TPM20_HOB_GUID
#define TPM20_HOB_GUID \
  { \
    0x7cea4f7e, 0x2052, 0x46ed, 0xbe, 0xfd, 0xe2, 0x2a, 0x44, 0xdc, 0x65, 0xe7 \
  }
#endif

#ifndef AMI_TREE_PPI_GUID
#define AMI_TREE_PPI_GUID\
    {0x86f5680a, 0x155c, 0x4bc8,  { 0xac, 0x77, 0x57, 0x38, 0x48, 0xe2, 0xad, 0x3d } }
#endif



#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
