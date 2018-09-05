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
// $Header: /Alaska/SOURCE/Modules/TcgNext/CommonHeaders/Tpm20.h 1     10/08/13 12:02p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:02p $
//*************************************************************************
// Revision History
// ----------------
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  Tpm20.h.h
//
// Description:
//  Contains Industry Standard #defines and Structures for TPM20
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _TPM20_H_
#define _TPM20_H_

#include <Uefi.h>

#pragma pack (push)
#pragma pack (1)


#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

// Table 189 -- Hash Algorithm Digest and Block Size Values
#define    SHA1_DIGEST_SIZE            20    // 20
#define    SHA1_BLOCK_SIZE             64    // 64
#define    SHA256_DIGEST_SIZE          32    // 32
#define    SHA256_BLOCK_SIZE           64    // 64
#define    SM3_256_DIGEST_SIZE         32    // 32
#define    SM3_256_BLOCK_SIZE          64    // 64
#define    SHA384_DIGEST_SIZE          48    // 48
#define    SHA384_BLOCK_SIZE           128    // 128
#define    SHA512_DIGEST_SIZE          64    // 64
#define    SHA512_BLOCK_SIZE           128    // 128
#define    WHIRLPOOL512_DIGEST_SIZE    64    // 64
#define    WHIRLPOOL512_BLOCK_SIZE     64    // 64


// Table 190 -- Logic Values
#define    YES      1    // 1
#define    NO       0    // 0
#define    TRUE     1    // 1
#define    FALSE    0    // 0
#define    SET      1    // 1
#define    CLEAR    0    // 0


// Table 191 -- Processor Values
#define    BIG_ENDIAN       0    // NO
#define    LITTLE_ENDIAN    1    // YES


// Table 192 -- Implemented Algorithms
#define    RSA                 1    // YES
#define    DES                 0    // NO
#define    _3DES               0    // NO
#define    SHA1                1    // YES
#define    SHA                 1    // SHA1
#define    HMAC                1    // YES
#define    AES                 1    // YES
#define    CFB                 1    // YES
#define    MGF1                1    // YES
#define    XOR                 1    // YES
#define    KEYEDHASH           1    // YES
#define    SHA256              1    // YES
#define    SHA384              1    // YES
#define    SHA512              1    // YES
#define    WHIRLPOOL512        0    // NO
#define    SM3_256             1    // YES
#define    SMS4                0    // NO
#define    RSASSA_PKCS1v1_5    1    // RSA
#define    RSAES_PKCS1v1_5     1    // RSA
#define    PSS                 1    // RSA
#define    OAEP                1    // RSA
#define    ECC                 1    // YES
#define    ECDSA               1    // ECC
#define    KDF1_SP800_56a      1    // ECC
#define    KDF2                0    // NO
#define    KDF1_SP800_108      1    // YES
#define    CTR                 1    // YES
#define    OFB                 1    // YES
#define    CBC                 1    // YES
#define    ECB                 1    // YES


// Table 193 -- Implemented Algorithm Constants
#define    MAX_RSA_KEY_BYTES    256    // 2048/8
#define    MAX_ECC_KEY          48    // 384/8
#define    MAX_CONTEXT_SIZE     4096    // 4096
#define    AES_KEY_SIZE_256     256    // 256
#define    MAX_AES_KEY          32    // 256/8
#define    RSA_KEY_SIZE_2048    2048    // 2048
#define    MAX_CC_LIST_SIZE     8    // 8
#define    MAX_HASH_BLOCK       128    // 128


// Table 194 -- Implementation Values
#define    IMPLEMENTATION_PCR             24    // 24
#define    PLATFORM_PCR                   24    // 24
#define    DRTM_PCR                       (TPM_RH_PCR0+17)    // (TPM_RH_PCR0+17)
#define    NUM_LOCALITIES                 5    // 5
#define    MAX_ACTIVE_SESSIONS            64    // 64
#define    CONTEXT_SLOT                   UINT16    // UINT16
#define    CONTEXT_COUNTER                UINT64    // UINT64
#define    MAX_LOADED_SESSIONS            3    // 3
#define    MAX_LOADED_OBJECTS             3    // 3
#define    MAX_EVICT_OBJECTS              16    // 16
#define    HASH_COUNT                     5    // (SHA1+SHA256+SHA384+SHA512+SM3_256)
#define    SYM_COUNT                      2    // (XOR+AES+SMS4)
#define    ASYM_COUNT                     2    // (RSA+ECC)
#define    PCR_SELECT_MIN                 3    // ((PLATFORM_PCR+7)/8)
#define    PCR_SELECT_MAX                 3    // ((IMPLEMENTATION_PCR+7)/8)
#define    MAX_DIGEST_BUFFER              1024    // 1024
#define    MAX_NV_INDEX_SIZE              1024    // 1024
#define    MAX_CAP_BUFFER                 1024    // 1024
#define    MAX_CAP_DATA                   (MAX_CAP_BUFFER-sizeof(TPM_CAP)-sizeof(UINT32))    // (MAX_CAP_BUFFER-sizeof(TPM_CAP)-sizeof(UINT32))
#define    MAX_CAP_ALGS                   (MAX_CAP_DATA/sizeof(TPMS_ALG_PROPERTY))    // (MAX_CAP_DATA/sizeof(TPMS_ALG_PROPERTY))
#define    MAX_CAP_HANDLES                (MAX_CAP_DATA/sizeof(TPM_HANDLE))    // (MAX_CAP_DATA/sizeof(TPM_HANDLE))
#define    MAX_CAP_CC                     (MAX_CAP_DATA/sizeof(TPM_CC))    // (MAX_CAP_DATA/sizeof(TPM_CC))
#define    MAX_TPM_PROPERTIES             (MAX_CAP_DATA/sizeof(TPMS_TAGGED_PROPERTY))    // (MAX_CAP_DATA/sizeof(TPMS_TAGGED_PROPERTY))
#define    MAX_PCR_PROPERTIES             (MAX_CAP_DATA/sizeof(TPMS_TAGGED_PCR_SELECT))    // (MAX_CAP_DATA/sizeof(TPMS_TAGGED_PCR_SELECT))
#define    MAX_ECC_CURVES                 (MAX_CAP_DATA/sizeof(TPM_ECC_CURVE_ID))    // (MAX_CAP_DATA/sizeof(TPM_ECC_CURVE_ID))
#define    NV_MEMORY_SIZE                 8192    // 8192
#define    NUM_STATIC_PCR                 16    // 16
#define    MAX_ALG_LIST_SIZE              128    // 128
#define    NV_CLOCK_UPDATE_RATE           12    // 12
#define    TIMER_PRESCALE                 100000    // 100000
#define    PP_COMMANDS_NUM                16    // 16
#define    PROOF_SIZE                     32    // 32
#define    PRIMARY_SEED_SIZE              64    // 64
#define    CONTEXT_ENCRYPT_ALG            TPM_ALG_AES    // TPM_ALG_AES
#define    CONTEXT_ENCRYPT_KEYSIZE        128    // 128
#define    CONTEXT_INTEGRITY_HASH_ALG     TPM_ALG_SHA256    // TPM_ALG_SHA256
#define    CONTEXT_INTEGRITY_HASH_SIZE    32    // 32
#define    NV_CLOCK_UPDATE_INTERVAL       12    // 12
#define    GENERATION_CTR                 UINT8    // UINT8
#define    NUM_POLICY_PCR                 1    // 1
#define    MAX_COMMAND_SIZE               4096    // 4096
#define    MAX_RESPONSE_SIZE              4096    // 4096
#define    MAX_ORDERLY_COUNT              256    // 256
#define    ALG_ID_FIRST                   TPM_ALG_RSA    // TPM_ALG_RSA
#define    ALG_ID_LAST                    TPM_ALG_ECB    // TPM_ALG_ECB
#define    MAX_SYM_DATA                   128    // 128
#define    MAX_HASH_STATE_SIZE            400    // 400
#define    MAX_HMAC_STATE_SIZE            800    // 800


// Table 2 -- BaseTypes from BaseTypes <I/O>
typedef unsigned char            BYTE;

// Table 3 -- DocumentationClarity from outputTypeDef <I/O>
typedef UINT32    TPM_ALGORITHM_ID;
typedef UINT32    TPM_MODIFIER_INDICATOR;
typedef UINT32    TPM_SESSION_OFFSET;
typedef UINT16    TPM_KEY_SIZE;
typedef UINT16    TPM_KEY_BITS;
typedef UINT64    TPM_SYSTEM_ADDRESS;

typedef UINT32    TPM2_PCRINDEX;

// Table 4 -- TPM_GENERATED from outputConstDef <O,S>
typedef UINT32 TPM_GENERATED;

#define    TPM_GENERATED_VALUE    (TPM_GENERATED)(0xff544347)




// Table 5 -- TPM2_ALG_ID from outputConstDef <I/O,S>
typedef UINT16 TPM2_ALG_ID;


#define    TPM2_ALG_ERROR               (TPM2_ALG_ID)(0x0000)
#define    TPM2_ALG_RSA                 (TPM2_ALG_ID)(0x0001)
#define    TPM2_ALG_DES                 (TPM2_ALG_ID)(0x0002)
#define    TPM2_ALG__3DES               (TPM2_ALG_ID)(0x0003)
#define    TPM2_ALG_SHA                 (TPM2_ALG_ID)(0x0004)
#define    TPM2_ALG_SHA1                (TPM2_ALG_ID)(0x0004)
#define    TPM2_ALG_HMAC                (TPM2_ALG_ID)(0x0005)
#define    TPM2_ALG_AES                 (TPM2_ALG_ID)(0x0006)
#define    TPM2_ALG_MGF1                (TPM2_ALG_ID)(0x0007)
#define    TPM2_ALG_XOR                 (TPM2_ALG_ID)(0x000A)
#define    TPM2_ALG_KEYEDHASH           (TPM2_ALG_ID)(0x0008)
#define    TPM2_ALG_SHA256              (TPM2_ALG_ID)(0x000B)
#define    TPM2_ALG_SHA384              (TPM2_ALG_ID)(0x000C)
#define    TPM2_ALG_SHA512              (TPM2_ALG_ID)(0x000D)
#define    TPM2_ALG_WHIRLPOOL512        (TPM2_ALG_ID)(0x000E)
#define    TPM2_ALG_NULL                (TPM2_ALG_ID)(0x0010)
#define    TPM2_ALG_SM3_256             (TPM2_ALG_ID)(0x0012)
#define    TPM2_ALG_SMS4                (TPM2_ALG_ID)(0x0013)
#define    TPM2_ALG_RSASSA_PKCS1v1_5    (TPM2_ALG_ID)(0x0014)
#define    TPM2_ALG_RSAES_PKCS1v1_5     (TPM2_ALG_ID)(0x0015)
#define    TPM2_ALG_PSS                 (TPM2_ALG_ID)(0x0016)
#define    TPM2_ALG_OAEP                (TPM2_ALG_ID)(0x0017)
#define    TPM2_ALG_ECDSA               (TPM2_ALG_ID)(0x0018)
#define    TPM2_ALG_SP800_56a_C1_1      (TPM2_ALG_ID)(0x0019)
#define    TPM2_ALG_KDF1_SP800_56a      (TPM2_ALG_ID)(0x0020)
#define    TPM2_ALG_KDF2                (TPM2_ALG_ID)(0x0021)
#define    TPM2_ALG_KDF1_SP800_108      (TPM2_ALG_ID)(0x0022)
#define    TPM2_ALG_ECC                 (TPM2_ALG_ID)(0x0023)
#define    TPM2_ALG_ECDAA               (TPM2_ALG_ID)(0x0024)
#define    TPM2_ALG_CTR                 (TPM2_ALG_ID)(0x0040)
#define    TPM2_ALG_OFB                 (TPM2_ALG_ID)(0x0041)
#define    TPM2_ALG_CBC                 (TPM2_ALG_ID)(0x0042)
#define    TPM2_ALG_CFB                 (TPM2_ALG_ID)(0x0043)
#define    TPM2_ALG_ECB                 (TPM2_ALG_ID)(0x0044)


// Table 7 -- TPM_CC from outputConstDef <I/O,S>
typedef UINT32 TPM_CC;

#define    TPM_CC_FIRST                         (TPM_CC)(0x0000011F)
#define    TPM_CC_PP_FIRST                      (TPM_CC)(0x0000011F)
#define    TPM_CC_NV_UndefineSpaceSpecial       (TPM_CC)(0x0000011F)
#define    TPM_CC_EvictControl                        (TPM_CC)(0x00000120)
#define    TPM_CC_HierarchyControl                    (TPM_CC)(0x00000121)
#define    TPM_CC_NV_UndefineSpace                    (TPM_CC)(0x00000122)
#define    TPM_CC_ChangeEPS                           (TPM_CC)(0x00000124)
#define    TPM_CC_ChangePPS                           (TPM_CC)(0x00000125)
#define    TPM_CC_Clear                               (TPM_CC)(0x00000126)
#define    TPM_CC_ClearControl                        (TPM_CC)(0x00000127)
#define    TPM_CC_ClockSet                            (TPM_CC)(0x00000128)
#define    TPM_CC_HierarchyChangeAuth                 (TPM_CC)(0x00000129)
#define    TPM_CC_NV_DefineSpace                      (TPM_CC)(0x0000012A)
#define    TPM_CC_PCR_Allocate                        (TPM_CC)(0x0000012B)
#define    TPM_CC_PCR_SetAuthPolicy                   (TPM_CC)(0x0000012C)
#define    TPM_CC_PP_Commands                         (TPM_CC)(0x0000012D)
#define    TPM_CC_SetPrimaryPolicy                    (TPM_CC)(0x0000012E)
#define    TPM_CC_FieldUpgradeStart                   (TPM_CC)(0x0000012F)
#define    TPM_CC_ClockRateAdjust                     (TPM_CC)(0x00000130)
#define    TPM_CC_CreatePrimary                       (TPM_CC)(0x00000131)
#define    TPM_CC_NV_GlobalWriteLock                  (TPM_CC)(0x00000132)
#define    TPM_CC_PP_LAST                             (TPM_CC)(0x00000132)
#define    TPM_CC_GetCommandAuditDigest               (TPM_CC)(0x00000133)
#define    TPM_CC_NV_Increment                        (TPM_CC)(0x00000134)
#define    TPM_CC_NV_SetBits                          (TPM_CC)(0x00000135)
#define    TPM_CC_NV_Extend                           (TPM_CC)(0x00000136)
#define    TPM_CC_NV_Write                            (TPM_CC)(0x00000137)
#define    TPM_CC_NV_WriteLock                        (TPM_CC)(0x00000138)
#define    TPM_CC_DictionaryAttackLockReset           (TPM_CC)(0x00000139)
#define    TPM_CC_DictionaryAttackParameters          (TPM_CC)(0x0000013A)
#define    TPM_CC_NV_ChangeAuth                       (TPM_CC)(0x0000013B)
#define    TPM_CC_PCR_Event                           (TPM_CC)(0x0000013C)
#define    TPM_CC_PCR_Reset                           (TPM_CC)(0x0000013D)
#define    TPM_CC_SequenceComplete                    (TPM_CC)(0x0000013E)
#define    TPM_CC_SetAlgorithmSet               (TPM_CC)(0x0000013F)
#define    TPM_CC_SetCommandCodeAuditStatus           (TPM_CC)(0x00000140)
#define    TPM_CC_FieldUpgradeData                    (TPM_CC)(0x00000141)
#define    TPM_CC_IncrementalSelfTest                 (TPM_CC)(0x00000142)
#define    TPM_CC_SelfTest                            (TPM_CC)(0x00000143)
#define    TPM_CC_Startup                             (TPM_CC)(0x00000144)
#define    TPM_CC_Shutdown                            (TPM_CC)(0x00000145)
#define    TPM_CC_StirRandom                          (TPM_CC)(0x00000146)
#define    TPM_CC_ActivateCredential                  (TPM_CC)(0x00000147)
#define    TPM_CC_Certify                             (TPM_CC)(0x00000148)
#define    TPM_CC_PolicyNV                            (TPM_CC)(0x00000149)
#define    TPM_CC_CertifyCreation                     (TPM_CC)(0x0000014A)
#define    TPM_CC_Duplicate                           (TPM_CC)(0x0000014B)
#define    TPM_CC_GetTime                             (TPM_CC)(0x0000014C)
#define    TPM_CC_GetSessionAuditDigest               (TPM_CC)(0x0000014D)
#define    TPM_CC_NV_Read                             (TPM_CC)(0x0000014E)
#define    TPM_CC_NV_ReadLock                         (TPM_CC)(0x0000014F)
#define    TPM_CC_ObjectChangeAuth                    (TPM_CC)(0x00000150)
#define    TPM_CC_PolicySecret                        (TPM_CC)(0x00000151)
#define    TPM_CC_Rewrap                              (TPM_CC)(0x00000152)
#define    TPM_CC_Create                              (TPM_CC)(0x00000153)
#define    TPM_CC_ECDH_ZGen                           (TPM_CC)(0x00000154)
#define    TPM_CC_HMAC                                (TPM_CC)(0x00000155)
#define    TPM_CC_Import                              (TPM_CC)(0x00000156)
#define    TPM_CC_Load                                (TPM_CC)(0x00000157)
#define    TPM_CC_Quote                               (TPM_CC)(0x00000158)
#define    TPM_CC_RSA_Decrypt                   (TPM_CC)(0x00000159)

#define    TPM_CC_HMAC_Start                          (TPM_CC)(0x0000015B)
#define    TPM_CC_SequenceUpdate                      (TPM_CC)(0x0000015C)
#define    TPM_CC_Sign                                (TPM_CC)(0x0000015D)
#define    TPM_CC_Unseal                              (TPM_CC)(0x0000015E)
#define    TPM_CC_PolicySigned                        (TPM_CC)(0x00000160)
#define    TPM_CC_ContextLoad                         (TPM_CC)(0x00000161)
#define    TPM_CC_ContextSave                         (TPM_CC)(0x00000162)
#define    TPM_CC_ECDH_KeyGen                         (TPM_CC)(0x00000163)
#define    TPM_CC_EncryptDecrypt                      (TPM_CC)(0x00000164)
#define    TPM_CC_FlushContext                        (TPM_CC)(0x00000165)

#define    TPM_CC_LoadExternal                        (TPM_CC)(0x00000167)
#define    TPM_CC_MakeCredential                      (TPM_CC)(0x00000168)
#define    TPM_CC_NV_ReadPublic                       (TPM_CC)(0x00000169)
#define    TPM_CC_PolicyAuthorize                     (TPM_CC)(0x0000016A)
#define    TPM_CC_PolicyAuthValue                     (TPM_CC)(0x0000016B)
#define    TPM_CC_PolicyCommandCode                   (TPM_CC)(0x0000016C)
#define    TPM_CC_PolicyCounterTimer                  (TPM_CC)(0x0000016D)
#define    TPM_CC_PolicyCpHash                        (TPM_CC)(0x0000016E)
#define    TPM_CC_PolicyLocality                      (TPM_CC)(0x0000016F)
#define    TPM_CC_PolicyNameHash                      (TPM_CC)(0x00000170)
#define    TPM_CC_PolicyOR                            (TPM_CC)(0x00000171)
#define    TPM_CC_PolicyTicket                        (TPM_CC)(0x00000172)
#define    TPM_CC_ReadPublic                          (TPM_CC)(0x00000173)
#define    TPM_CC_RSA_Encrypt                   (TPM_CC)(0x00000174)

#define    TPM_CC_StartAuthSession                    (TPM_CC)(0x00000176)
#define    TPM_CC_VerifySignature                     (TPM_CC)(0x00000177)
#define    TPM_CC_ECC_Parameters                      (TPM_CC)(0x00000178)
#define    TPM_CC_FirmwareRead                        (TPM_CC)(0x00000179)
#define    TPM_CC_GetCapability                       (TPM_CC)(0x0000017A)
#define    TPM_CC_GetRandom                           (TPM_CC)(0x0000017B)
#define    TPM_CC_GetTestResult                       (TPM_CC)(0x0000017C)
#define    TPM_CC_Hash                                (TPM_CC)(0x0000017D)
#define    TPM_CC_PCR_Read                            (TPM_CC)(0x0000017E)
#define    TPM_CC_PolicyPCR                           (TPM_CC)(0x0000017F)
#define    TPM_CC_PolicyRestart                       (TPM_CC)(0x00000180)
#define    TPM_CC_ReadClock                           (TPM_CC)(0x00000181)
#define    TPM_CC_PCR_Extend                          (TPM_CC)(0x00000182)
#define    TPM_CC_PCR_SetAuthValue                    (TPM_CC)(0x00000183)
#define    TPM_CC_NV_Certify                          (TPM_CC)(0x00000184)
#define    TPM_CC_EventSequenceComplete               (TPM_CC)(0x00000185)
#define    TPM_CC_HashSequenceStart                   (TPM_CC)(0x00000186)
#define    TPM_CC_PolicyPhysicalPresence              (TPM_CC)(0x00000187)
#define    TPM_CC_PolicyDuplicationSelect             (TPM_CC)(0x00000188)
#define    TPM_CC_PolicyGetDigest                     (TPM_CC)(0x00000189)
#define    TPM_CC_TestParms                           (TPM_CC)(0x0000018A)
#define    TPM_CC_Commit                        (TPM_CC)(0x0000018B)
#define    TPM_CC_PolicyPassword                      (TPM_CC)(0x0000018C)
#define    TPM_CC_LAST                                (TPM_CC)(0x0000018C)




// Table 11 -- TPM_RC from outputConstDef <O,S>
typedef UINT32 TPM_RC;

#define    TPM_RC_SUCCESS              (TPM_RC)(0x000)
#define    TPM_RC_BAD_TAG              (TPM_RC)(0x030)
#define    RC_VER1                     (TPM_RC)(0x100)
#define    TPM_RC_PRIVATE              (TPM_RC)(RC_VER1 + 0x00B)
#define    TPM_RC_INITIALIZE           (TPM_RC)(RC_VER1 + 0x000)
#define    TPM_RC_FAILURE              (TPM_RC)(RC_VER1 + 0x001)
#define    TPM_RC_SEQUENCE             (TPM_RC)(RC_VER1 + 0x003)
#define    TPM_RC_HMAC                 (TPM_RC)(RC_VER1 + 0x019)
#define    TPM_RC_DISABLED             (TPM_RC)(RC_VER1 + 0x020)
#define    TPM_RC_EXCLUSIVE            (TPM_RC)(RC_VER1 + 0x021)
#define    TPM_RC_ECC_CURVE            (TPM_RC)(RC_VER1 + 0x023)
#define    TPM_RC_AUTH_TYPE            (TPM_RC)(RC_VER1 + 0x024)
#define    TPM_RC_AUTH_MISSING         (TPM_RC)(RC_VER1 + 0x025)
#define    TPM_RC_POLICY               (TPM_RC)(RC_VER1 + 0x026)
#define    TPM_RC_PCR                  (TPM_RC)(RC_VER1 + 0x027)
#define    TPM_RC_PCR_CHANGED          (TPM_RC)(RC_VER1 + 0x028)
#define    TPM_RC_ECC_POINT            (TPM_RC)(RC_VER1 + 0x02C)
#define    TPM_RC_UPGRADE              (TPM_RC)(RC_VER1 + 0x02D)
#define    TPM_RC_TOO_MANY_CONTEXTS    (TPM_RC)(RC_VER1 + 0x02E)
#define    TPM_RC_AUTH_UNAVAILABLE     (TPM_RC)(RC_VER1 + 0x02F)
#define    TPM_RC_REBOOT               (TPM_RC)(RC_VER1 + 0x030)
#define    TPM_RC_UNBALANCED           (TPM_RC)(RC_VER1 + 0x031)
#define    TPM_RC_COMMAND_SIZE         (TPM_RC)(RC_VER1 + 0x042)
#define    TPM_RC_COMMAND_CODE         (TPM_RC)(RC_VER1 + 0x043)
#define    TPM_RC_AUTHSIZE             (TPM_RC)(RC_VER1 + 0x044)
#define    TPM_RC_AUTH_CONTEXT         (TPM_RC)(RC_VER1 + 0x045)
#define    TPM_RC_NV_RANGE             (TPM_RC)(RC_VER1 + 0x046)
#define    TPM_RC_NV_SIZE              (TPM_RC)(RC_VER1 + 0x047)
#define    TPM_RC_NV_LOCKED            (TPM_RC)(RC_VER1 + 0x048)
#define    TPM_RC_NV_AUTHORIZATION     (TPM_RC)(RC_VER1 + 0x049)
#define    TPM_RC_NV_UNINITIALIZED     (TPM_RC)(RC_VER1 + 0x04A)
#define    TPM_RC_NV_SPACE             (TPM_RC)(RC_VER1 + 0x04B)
#define    TPM_RC_NV_DEFINED           (TPM_RC)(RC_VER1 + 0x04C)
#define    TPM_RC_BAD_CONTEXT          (TPM_RC)(RC_VER1 + 0x050)
#define    TPM_RC_CPHASH               (TPM_RC)(RC_VER1 + 0x051)
#define    TPM_RC_PARENT               (TPM_RC)(RC_VER1 + 0x052)
#define    TPM_RC_NEEDS_TEST           (TPM_RC)(RC_VER1 + 0x053)
#define    TPM_RC_NO_RESULT            (TPM_RC)(RC_VER1 + 0x054)
#define    TPM_RC_SENSITIVE            (TPM_RC)(RC_VER1 + 0x055)
#define    RC_MAX_FM0                  (TPM_RC)(RC_VER1 + 0x07F)
#define    RC_FMT1                     (TPM_RC)(0x080)
#define    TPM_RC_ASYMMETRIC           (TPM_RC)(RC_FMT1 + 0x001)
#define    TPM_RC_ATTRIBUTES           (TPM_RC)(RC_FMT1 + 0x002)
#define    TPM_RC_HASH                 (TPM_RC)(RC_FMT1 + 0x003)
#define    TPM_RC_VALUE                (TPM_RC)(RC_FMT1 + 0x004)
#define    TPM_RC_HIERARCHY            (TPM_RC)(RC_FMT1 + 0x005)
#define    TPM_RC_KEY_SIZE             (TPM_RC)(RC_FMT1 + 0x007)
#define    TPM_RC_MGF                  (TPM_RC)(RC_FMT1 + 0x008)
#define    TPM_RC_MODE                 (TPM_RC)(RC_FMT1 + 0x009)
#define    TPM_RC_TYPE                 (TPM_RC)(RC_FMT1 + 0x00A)
#define    TPM_RC_HANDLE               (TPM_RC)(RC_FMT1 + 0x00B)
#define    TPM_RC_KDF                  (TPM_RC)(RC_FMT1 + 0x00C)
#define    TPM_RC_RANGE                (TPM_RC)(RC_FMT1 + 0x00D)
#define    TPM_RC_AUTH_FAIL            (TPM_RC)(RC_FMT1 + 0x00E)
#define    TPM_RC_NONCE                (TPM_RC)(RC_FMT1 + 0x00F)
#define    TPM_RC_PP                   (TPM_RC)(RC_FMT1 + 0x010)
#define    TPM_RC_SCHEME               (TPM_RC)(RC_FMT1 + 0x012)
#define    TPM_RC_SIZE                 (TPM_RC)(RC_FMT1 + 0x015)
#define    TPM_RC_SYMMETRIC            (TPM_RC)(RC_FMT1 + 0x016)
#define    TPM_RC_TAG                  (TPM_RC)(RC_FMT1 + 0x017)
#define    TPM_RC_SELECTOR             (TPM_RC)(RC_FMT1 + 0x018)
#define    TPM_RC_INSUFFICIENT         (TPM_RC)(RC_FMT1 + 0x01A)
#define    TPM_RC_SIGNATURE            (TPM_RC)(RC_FMT1 + 0x01B)
#define    TPM_RC_KEY                  (TPM_RC)(RC_FMT1 + 0x01C)
#define    TPM_RC_POLICY_FAIL          (TPM_RC)(RC_FMT1 + 0x01D)
#define    TPM_RC_INTEGRITY            (TPM_RC)(RC_FMT1 + 0x01F)
#define    TPM_RC_TICKET               (TPM_RC)(RC_FMT1 + 0x020)
#define    TPM_RC_RESERVED_BITS        (TPM_RC)(RC_FMT1 + 0x021)
#define    TPM_RC_BAD_AUTH             (TPM_RC)(RC_FMT1 + 0x022)
#define    TPM_RC_EXPIRED              (TPM_RC)(RC_FMT1 + 0x023)
#define    TPM_RC_POLICY_CC            (TPM_RC)(RC_FMT1 + 0x024 )
#define    TPM_RC_BINDING              (TPM_RC)(RC_FMT1 + 0x025)
#define    TPM_RC_CURVE                (TPM_RC)(RC_FMT1 + 0x026)
#define    RC_WARN                     (TPM_RC)(0x900)
#define    TPM_RC_CONTEXT_GAP          (TPM_RC)(RC_WARN + 0x001)
#define    TPM_RC_OBJECT_MEMORY        (TPM_RC)(RC_WARN + 0x002)
#define    TPM_RC_SESSION_MEMORY       (TPM_RC)(RC_WARN + 0x003)
#define    TPM_RC_MEMORY               (TPM_RC)(RC_WARN + 0x004)
#define    TPM_RC_SESSION_HANDLES      (TPM_RC)(RC_WARN + 0x005)
#define    TPM_RC_OBJECT_HANDLES       (TPM_RC)(RC_WARN + 0x006)
#define    TPM_RC_LOCALITY             (TPM_RC)(RC_WARN + 0x007)
#define    TPM_RC_YIELDED              (TPM_RC)(RC_WARN + 0x008)
#define    TPM_RC_CANCELLED            (TPM_RC)(RC_WARN + 0x009)
#define    TPM_RC_TESTING              (TPM_RC)(RC_WARN + 0x00A)
#define    TPM_RC_REFERENCE_H0         (TPM_RC)(RC_WARN + 0x010)
#define    TPM_RC_REFERENCE_H1         (TPM_RC)(RC_WARN + 0x011)
#define    TPM_RC_REFERENCE_H2         (TPM_RC)(RC_WARN + 0x012)
#define    TPM_RC_REFERENCE_H3         (TPM_RC)(RC_WARN + 0x013)
#define    TPM_RC_REFERENCE_H4         (TPM_RC)(RC_WARN + 0x014)
#define    TPM_RC_REFERENCE_H5         (TPM_RC)(RC_WARN + 0x015)
#define    TPM_RC_REFERENCE_H6         (TPM_RC)(RC_WARN + 0x016)
#define    TPM_RC_REFERENCE_S0         (TPM_RC)(RC_WARN + 0x018)
#define    TPM_RC_REFERENCE_S1         (TPM_RC)(RC_WARN + 0x019)
#define    TPM_RC_REFERENCE_S2         (TPM_RC)(RC_WARN + 0x01A)
#define    TPM_RC_REFERENCE_S3         (TPM_RC)(RC_WARN + 0x01B)
#define    TPM_RC_REFERENCE_S4         (TPM_RC)(RC_WARN + 0x01C)
#define    TPM_RC_REFERENCE_S5         (TPM_RC)(RC_WARN + 0x01D)
#define    TPM_RC_REFERENCE_S6         (TPM_RC)(RC_WARN + 0x01E)
#define    TPM_RC_NV_RATE              (TPM_RC)(RC_WARN + 0x020)
#define    TPM_RC_LOCKOUT              (TPM_RC)(RC_WARN + 0x021)
#define    TPM_RC_RETRY                (TPM_RC)(RC_WARN + 0x022)
#define    TPM_RC_NV_UNAVAILABLE       (TPM_RC)(RC_WARN + 0x023)
#define    TPM_RC_NOT_USED             (TPM_RC)(RC_WARN + 0x7F)
#define    TPM_RC_H                    (TPM_RC)(0x000)
#define    TPM_RC_P                    (TPM_RC)(0x040)
#define    TPM_RC_S                    (TPM_RC)(0x800)
#define    TPM_RC_1                    (TPM_RC)(0x100)
#define    TPM_RC_2                    (TPM_RC)(0x200)
#define    TPM_RC_3                    (TPM_RC)(0x300)
#define    TPM_RC_4                    (TPM_RC)(0x400)
#define    TPM_RC_5                    (TPM_RC)(0x500)
#define    TPM_RC_6                    (TPM_RC)(0x600)
#define    TPM_RC_7                    (TPM_RC)(0x700)
#define    TPM_RC_8                    (TPM_RC)(0x800)
#define    TPM_RC_9                    (TPM_RC)(0x900)
#define    TPM_RC_A                    (TPM_RC)(0xA00)
#define    TPM_RC_B                    (TPM_RC)(0xB00)
#define    TPM_RC_C                    (TPM_RC)(0xC00)
#define    TPM_RC_D                    (TPM_RC)(0xD00)
#define    TPM_RC_E                    (TPM_RC)(0xE00)
#define    TPM_RC_F                    (TPM_RC)(0xF00)
#define    TPM_RC_N_MASK               (TPM_RC)(0xF00)




// Table 12 -- TPM_CLOCK_ADJUST from outputConstDef <I,S>
typedef INT8 TPM_CLOCK_ADJUST;

#define    TPM_CLOCK_COARSE_SLOWER    (TPM_CLOCK_ADJUST)(-3)
#define    TPM_CLOCK_MEDIUM_SLOWER    (TPM_CLOCK_ADJUST)(-2)
#define    TPM_CLOCK_FINE_SLOWER      (TPM_CLOCK_ADJUST)(-1)
#define    TPM_CLOCK_NO_CHANGE        (TPM_CLOCK_ADJUST)(0)
#define    TPM_CLOCK_FINE_FASTER      (TPM_CLOCK_ADJUST)(1)
#define    TPM_CLOCK_MEDIUM_FASTER    (TPM_CLOCK_ADJUST)(2)
#define    TPM_CLOCK_COARSE_FASTER    (TPM_CLOCK_ADJUST)(3)




// Table 13 -- TPM_EO from outputConstDef <I/O,S>
typedef UINT16 TPM_EO;

#define    TPM_EO_EQ             (TPM_EO)(0x0000)
#define    TPM_EO_NEQ            (TPM_EO)(0x0001)
#define    TPM_EO_SIGNED_GT      (TPM_EO)(0x0002)
#define    TPM_EO_UNSIGNED_GT    (TPM_EO)(0x0003)
#define    TPM_EO_SIGNED_LT      (TPM_EO)(0x0004)
#define    TPM_EO_UNSIGNED_LT    (TPM_EO)(0x0005)
#define    TPM_EO_SIGNED_GE      (TPM_EO)(0x0006)
#define    TPM_EO_UNSIGNED_GE    (TPM_EO)(0x0007)
#define    TPM_EO_SIGNED_LE      (TPM_EO)(0x0008)
#define    TPM_EO_UNSIGNED_LE    (TPM_EO)(0x0009)
#define    TPM_EO_BITSET         (TPM_EO)(0x000A)
#define    TPM_EO_BITCLEAR       (TPM_EO)(0x000B)




// Table 14 -- TPM_ST from outputConstDef <I/O,S>
typedef UINT16 TPM_ST;

#define    TPM_ST_RSP_COMMAND             (TPM_ST)(0x00C4)
#define    TPM_ST_NULL                    (TPM_ST)(0X8000)
#define    TPM_ST_NO_SESSIONS             (TPM_ST)(0x8001)
#define    TPM_ST_SESSIONS                (TPM_ST)(0x8002)
#define    TPM_ST_ATTEST_COMMAND_AUDIT    (TPM_ST)(0x8015)
#define    TPM_ST_ATTEST_SESSION_AUDIT    (TPM_ST)(0x8016)
#define    TPM_ST_ATTEST_CERTIFY          (TPM_ST)(0x8017)
#define    TPM_ST_ATTEST_QUOTE            (TPM_ST)(0x8018)
#define    TPM_ST_ATTEST_TIME             (TPM_ST)(0x8019)
#define    TPM_ST_ATTEST_CREATION         (TPM_ST)(0x801A)
#define    TPM_ST_ATTEST_NV               (TPM_ST)(0x801B)
#define    TPM_ST_CREATION                (TPM_ST)(0x8021)
#define    TPM_ST_VERIFIED                (TPM_ST)(0x8022)
#define    TPM_ST_AUTH_SECRET             (TPM_ST)(0x8023)
#define    TPM_ST_HASHCHECK               (TPM_ST)(0x8024)
#define    TPM_ST_AUTH_SIGNED             (TPM_ST)(0x8025)
#define    TPM_ST_FU_MANIFEST             (TPM_ST)(0x8029)


// Table 15 -- TPM_SU from outputConstDef <I>
typedef UINT16 TPM_SU;

#define    TPM_SU_CLEAR     (TPM_SU)(0x0000)
#define    TPM_SU_STATE     (TPM_SU)(0x0001)




// Table 16 -- TPM_SE from outputConstDef <I>
typedef UINT8 TPM_SE;

#define    TPM_SE_HMAC      (TPM_SE)(0x00)
#define    TPM_SE_POLICY    (TPM_SE)(0x01)
#define    TPM_SE_TRIAL     (TPM_SE)(0x03)




// Table 17 -- TPM_CAP from outputConstDef <I/O,S>
typedef UINT32 TPM_CAP;

#define    TPM_CAP_FIRST              (TPM_CAP)(0x00000000)
#define    TPM_CAP_ALGS               (TPM_CAP)(0x00000000)
#define    TPM_CAP_HANDLES            (TPM_CAP)(0x00000001)
#define    TPM_CAP_COMMANDS           (TPM_CAP)(0x00000002)
#define    TPM_CAP_PP_COMMANDS        (TPM_CAP)(0x00000003)
#define    TPM_CAP_AUDIT_COMMANDS     (TPM_CAP)(0x00000004)
#define    TPM_CAP_PCRS               (TPM_CAP)(0x00000005)
#define    TPM_CAP_TPM_PROPERTIES     (TPM_CAP)(0x00000006)
#define    TPM_CAP_PCR_PROPERTIES     (TPM_CAP)(0x00000007)
#define    TPM_CAP_ECC_CURVES         (TPM_CAP)(0x00000008)
#define    TPM_CAP_LAST               (TPM_CAP)(0x00000008)
#define    TPM_CAP_VENDOR_PROPERTY    (TPM_CAP)(0x00000100)




// Table 18 -- TPM_PT from outputConstDef <I/O,S>
typedef UINT32 TPM_PT;

#define    TPM_PT_NONE                   (TPM_PT)(0x00000000)
#define    PT_GROUP                      (TPM_PT)(0x00000100)
#define    PT_FIXED                      (TPM_PT)(PT_GROUP * 1)
#define    TPM_PT_FAMILY_INDICATOR       (TPM_PT)(PT_FIXED + 0)
#define    TPM_PT_LEVEL                  (TPM_PT)(PT_FIXED + 1)
#define    TPM_PT_REVISION               (TPM_PT)(PT_FIXED + 2)
#define    TPM_PT_DAY_OF_YEAR            (TPM_PT)(PT_FIXED + 3)
#define    TPM_PT_YEAR                   (TPM_PT)(PT_FIXED + 4)
#define    TPM_PT_MANUFACTURER           (TPM_PT)(PT_FIXED + 5)
#define    TPM_PT_VENDOR_STRING_1        (TPM_PT)(PT_FIXED + 6)
#define    TPM_PT_VENDOR_STRING_2        (TPM_PT)(PT_FIXED + 7)
#define    TPM_PT_VENDOR_STRING_3        (TPM_PT)(PT_FIXED + 8)
#define    TPM_PT_VENDOR_STRING_4        (TPM_PT)(PT_FIXED + 9)
#define    TPM_PT_VENDOR_TPM_TYPE        (TPM_PT)(PT_FIXED + 10)
#define    TPM_PT_FIRMWARE_VERSION_1     (TPM_PT)(PT_FIXED + 11)
#define    TPM_PT_FIRMWARE_VERSION_2     (TPM_PT)(PT_FIXED + 12)
#define    TPM_PT_INPUT_BUFFER           (TPM_PT)(PT_FIXED + 13)
#define    TPM_PT_HR_TRANSIENT_MIN       (TPM_PT)(PT_FIXED + 14)
#define    TPM_PT_HR_PERSISTENT_MIN      (TPM_PT)(PT_FIXED + 15)
#define    TPM_PT_HR_LOADED_MIN          (TPM_PT)(PT_FIXED + 16)
#define    TPM_PT_ACTIVE_SESSIONS_MAX    (TPM_PT)(PT_FIXED + 17)
#define    TPM_PT_PCR_COUNT              (TPM_PT)(PT_FIXED + 18)
#define    TPM_PT_PCR_SELECT_MIN         (TPM_PT)(PT_FIXED + 19)
#define    TPM_PT_CONTEXT_GAP_MAX        (TPM_PT)(PT_FIXED + 20)
#define    TPM_PT_NV_COUNTERS_MAX        (TPM_PT)(PT_FIXED + 22)
#define    TPM_PT_NV_INDEX_MAX           (TPM_PT)(PT_FIXED + 23)
#define    TPM_PT_MEMORY                 (TPM_PT)(PT_FIXED + 24)
#define    TPM_PT_CLOCK_UPDATE           (TPM_PT)(PT_FIXED + 25)
#define    TPM_PT_CONTEXT_HASH           (TPM_PT)(PT_FIXED + 26)
#define    TPM_PT_CONTEXT_SYM            (TPM_PT)(PT_FIXED + 27)
#define    TPM_PT_CONTEXT_SYM_SIZE       (TPM_PT)(PT_FIXED + 28)
#define    TPM_PT_ORDERLY_COUNT          (TPM_PT)(PT_FIXED + 29)
#define    TPM_PT_MAX_COMMAND_SIZE       (TPM_PT)(PT_FIXED + 30)
#define    TPM_PT_MAX_RESPONSE_SIZE      (TPM_PT)(PT_FIXED + 31)
#define    TPM_PT_MAX_DIGEST             (TPM_PT)(PT_FIXED + 32)
#define    TPM_PT_MAX_OBJECT_CONTEXT     (TPM_PT)(PT_FIXED + 33)
#define    TPM_PT_MAX_SESSION_CONTEXT    (TPM_PT)(PT_FIXED + 34)
#define    TPM_PT_PS_FAMILY_INDICATOR    (TPM_PT)(PT_FIXED + 35)
#define    TPM_PT_PS_LEVEL               (TPM_PT)(PT_FIXED + 36)
#define    TPM_PT_PS_REVISION            (TPM_PT)(PT_FIXED + 37)
#define    TPM_PT_PS_DAY_OF_YEAR         (TPM_PT)(PT_FIXED + 38)
#define    TPM_PT_PS_YEAR                (TPM_PT)(PT_FIXED + 39)
#define    TPM_PT_SPLIT_MAX              (TPM_PT)(PT_FIXED + 40)
#define    TPM_PT_TOTAL_COMMANDS         (TPM_PT)(PT_FIXED + 41)
#define    TPM_PT_LIBRARY_COMMANDS       (TPM_PT)(PT_FIXED + 42)
#define    TPM_PT_VENDOR_COMMANDS        (TPM_PT)(PT_FIXED + 43)
#define    PT_VAR                        (TPM_PT)(PT_GROUP * 2)
#define    TPM_PT_PERMANENT              (TPM_PT)(PT_VAR + 0)
#define    TPM_PT_STARTUP_CLEAR          (TPM_PT)(PT_VAR + 1)
#define    TPM_PT_HR_NV_INDEX            (TPM_PT)(PT_VAR + 2)
#define    TPM_PT_HR_LOADED              (TPM_PT)(PT_VAR + 3)
#define    TPM_PT_HR_LOADED_AVAIL        (TPM_PT)(PT_VAR + 4)
#define    TPM_PT_HR_ACTIVE              (TPM_PT)(PT_VAR + 5)
#define    TPM_PT_HR_ACTIVE_AVAIL        (TPM_PT)(PT_VAR + 6)
#define    TPM_PT_HR_TRANSIENT_AVAIL     (TPM_PT)(PT_VAR + 7)
#define    TPM_PT_HR_PERSISTENT          (TPM_PT)(PT_VAR + 8)
#define    TPM_PT_HR_PERSISTENT_AVAIL    (TPM_PT)(PT_VAR + 9)
#define    TPM_PT_NV_COUNTERS            (TPM_PT)(PT_VAR + 10)
#define    TPM_PT_NV_COUNTERS_AVAIL      (TPM_PT)(PT_VAR + 11)
#define    TPM_PT_ALGORITHM_SET          (TPM_PT)(PT_VAR + 12)
#define    TPM_PT_LOADED_CURVES          (TPM_PT)(PT_VAR + 13)
#define    TPM_PT_LOCKOUT_COUNTER        (TPM_PT)(PT_VAR + 14)
#define    TPM_PT_MAX_AUTH_FAIL          (TPM_PT)(PT_VAR + 15)
#define    TPM_PT_LOCKOUT_INTERVAL       (TPM_PT)(PT_VAR + 16)
#define    TPM_PT_LOCKOUT_RECOVERY       (TPM_PT)(PT_VAR + 17)
#define    TPM_PT_NV_WRITE_RECOVERY      (TPM_PT)(PT_VAR + 18)
#define    TPM_PT_AUDIT_COUNTER_0        (TPM_PT)(PT_VAR + 19)
#define    TPM_PT_AUDIT_COUNTER_1        (TPM_PT)(PT_VAR + 20)

// Table 19 -- TPM_PT_PCR from outputConstDef <I/O,S>
typedef UINT32 TPM_PT_PCR;

#define    TPM_PT_PCR_FIRST         (TPM_PT_PCR)(0x00000000)
#define    TPM_PT_PCR_SAVE          (TPM_PT_PCR)(0x00000000)
#define    TPM_PT_PCR_EXTEND_L0     (TPM_PT_PCR)(0x00000001)
#define    TPM_PT_PCR_RESET_L0      (TPM_PT_PCR)(0x00000002)
#define    TPM_PT_PCR_EXTEND_L1     (TPM_PT_PCR)(0x00000003)
#define    TPM_PT_PCR_RESET_L1      (TPM_PT_PCR)(0x00000004)
#define    TPM_PT_PCR_EXTEND_L2     (TPM_PT_PCR)(0x00000005)
#define    TPM_PT_PCR_RESET_L2      (TPM_PT_PCR)(0x00000006)
#define    TPM_PT_PCR_EXTEND_L3     (TPM_PT_PCR)(0x00000007)
#define    TPM_PT_PCR_RESET_L3      (TPM_PT_PCR)(0x00000008)
#define    TPM_PT_PCR_EXTEND_L4     (TPM_PT_PCR)(0x00000009)
#define    TPM_PT_PCR_RESET_L4      (TPM_PT_PCR)(0x0000000A)
#define    TPM_PT_PCR_DRTM_RESET    (TPM_PT_PCR)(0x0000000B)
#define    TPM_PT_PCR_POLICY        (TPM_PT_PCR)(0x0000000C)
#define    TPM_PT_PCR_AUTH          (TPM_PT_PCR)(0x0000000D)
#define    TPM_PT_PCR_LAST          (TPM_PT_PCR)(0x0000000D)

// Table 23 -- TPM_PS Constants <O,S>
typedef UINT32 TPM_PS;

#define    TPM_PS_MAIN              (TPM_PS)(0x00000000)
#define    TPM_PS_PC                (TPM_PS)(0x00000001)
#define    TPM_PS_PDA               (TPM_PS)(0x00000002)
#define    TPM_PS_CELL_PHONE        (TPM_PS)(0x00000003)
#define    TPM_PS_SERVER            (TPM_PS)(0x00000004)
#define    TPM_PS_PERIPHERAL        (TPM_PS)(0x00000005)
#define    TPM_PS_TSS               (TPM_PS)(0x00000006)
#define    TPM_PS_STORAGE           (TPM_PS)(0x00000007)
#define    TPM_PS_AUTHENTICATION    (TPM_PS)(0x00000008)
#define    TPM_PS_EMBEDDED          (TPM_PS)(0x00000009)
#define    TPM_PS_HARDCOPY          (TPM_PS)(0x0000000A)
#define    TPM_PS_INFRASTRUCTURE    (TPM_PS)(0x0000000B)
#define    TPM_PS_VIRTUALIZATION    (TPM_PS)(0x0000000C)
#define    TPM_PS_TNC               (TPM_PS)(0x0000000D)
#define    TPM_PS_MULTI_TENANT      (TPM_PS)(0x0000000E)
#define    TPM_PS_TC                (TPM_PS)(0x0000000F)

// Table 24 -- Handles Types <I/O>
typedef UINT32    TPM_HANDLE;

// Table 21 -- TPM_HT from outputConstDef <S>
typedef UINT8 TPM_HT;

#define    TPM_HT_PCR               (TPM_HT)(0x00)
#define    TPM_HT_NV_INDEX          (TPM_HT)(0x01)
#define    TPM_HT_HMAC_SESSION      (TPM_HT)(0x02)
#define    TPM_HT_LOADED_SESSION    (TPM_HT)(0x02)
#define    TPM_HT_POLICY_SESSION    (TPM_HT)(0x03)
#define    TPM_HT_ACTIVE_SESSION    (TPM_HT)(0x03)
#define    TPM_HT_PERMANENT         (TPM_HT)(0x40)
#define    TPM_HT_TRANSIENT         (TPM_HT)(0x80)
#define    TPM_HT_PERSISTENT        (TPM_HT)(0x81)




// Table 22 -- TPM_RH from outputConstDef <I,S>
typedef UINT32 TPM_RH;

#define    TPM_RH_FIRST          (TPM_RH)(0x40000000)
#define    TPM_RH_SRK            (TPM_RH)(0x40000000)
#define    TPM_RH_OWNER          (TPM_RH)(0x40000001)
#define    TPM_RH_REVOKE         (TPM_RH)(0x40000002)
#define    TPM_RH_TRANSPORT      (TPM_RH)(0x40000003)
#define    TPM_RH_OPERATOR       (TPM_RH)(0x40000004)
#define    TPM_RH_ADMIN          (TPM_RH)(0x40000005)
#define    TPM_RH_EK             (TPM_RH)(0x40000006)
#define    TPM_RH_NULL           (TPM_RH)(0x40000007)
#define    TPM_RH_UNASSIGNED     (TPM_RH)(0x40000008)
#define    TPM_RH_PW             (TPM_RH)(0x40000009)
#define    TPM_RS_PW             (TPM_RH)(0x40000009)
#define    TPM_RH_LOCKOUT        (TPM_RH)(0x4000000A)
#define    TPM_RH_ENDORSEMENT    (TPM_RH)(0x4000000B)
#define    TPM_RH_PLATFORM       (TPM_RH)(0x4000000C)
#define    TPM_RH_LAST           (TPM_RH)(0x4000000C)
#define    TPM_RH_PCR0           (TPM_RH)(0x00000000)


// Table 23 -- TPM_HC from outputConstDef <I,S>
typedef TPM_HANDLE TPM_HC;

#define    HR_HANDLE_MASK          (TPM_HC)(0x00FFFFFF)
#define    HR_RANGE_MASK           (TPM_HC)(0xFF000000)
#define    HR_SHIFT                (TPM_HC)(24)
#define    HR_PCR                  (TPM_HC)(TPM_HT_PCR << HR_SHIFT)
#define    HR_HMAC_SESSION         (TPM_HC)(TPM_HT_HMAC_SESSION << HR_SHIFT)
#define    HR_POLICY_SESSION       (TPM_HC)(TPM_HT_POLICY_SESSION << HR_SHIFT)
#define    HR_TRANSIENT            (TPM_HC)(TPM_HT_TRANSIENT << HR_SHIFT)
#define    HR_PERSISTENT           (TPM_HC)(TPM_HT_PERSISTENT << HR_SHIFT)
#define    HR_NV_INDEX             (TPM_HC)(TPM_HT_NV_INDEX << HR_SHIFT)
#define    HR_PERMANENT            (TPM_HC)(TPM_HT_PERMANENT << HR_SHIFT)
#define    PCR_FIRST               (TPM_HC)(TPM_RH_PCR0)
#define    PCR_LAST                (TPM_HC)(PCR_FIRST + IMPLEMENTATION_PCR-1)
#define    HMAC_SESSION_FIRST      (TPM_HC)(HR_HMAC_SESSION + 0)
#define    HMAC_SESSION_LAST       (TPM_HC)(HMAC_SESSION_FIRST+MAX_ACTIVE_SESSIONS-1)
#define    LOADED_SESSION_FIRST    (TPM_HC)(HMAC_SESSION_FIRST)
#define    LOADED_SESSION_LAST     (TPM_HC)(HMAC_SESSION_LAST)
#define    POLICY_SESSION_FIRST    (TPM_HC)(HR_POLICY_SESSION + 0)
#define    POLICY_SESSION_LAST     (TPM_HC)(POLICY_SESSION_FIRST + MAX_ACTIVE_SESSIONS-1)
#define    TRANSIENT_FIRST         (TPM_HC)(HR_TRANSIENT +  0)
#define    ACTIVE_SESSION_FIRST    (TPM_HC)(POLICY_SESSION_FIRST)
#define    ACTIVE_SESSION_LAST     (TPM_HC)(POLICY_SESSION_LAST)
#define    TRANSIENT_LAST          (TPM_HC)(TRANSIENT_FIRST+MAX_LOADED_OBJECTS-1)
#define    PERSISTENT_FIRST        (TPM_HC)(HR_PERSISTENT  +  0)
#define    PERSISTENT_LAST         (TPM_HC)(PERSISTENT_FIRST + 0x00FFFFFF)
#define    PLATFORM_PERSISTENT     (TPM_HC)(PERSISTENT_FIRST + 0x00800000)
#define    NV_INDEX_FIRST          (TPM_HC)(HR_NV_INDEX + 0)
#define    NV_INDEX_LAST           (TPM_HC)(NV_INDEX_FIRST + 0x00FFFFFF)
#define    PERMANENT_FIRST         (TPM_HC)(TPM_RH_FIRST)
#define    PERMANENT_LAST          (TPM_HC)(TPM_RH_LAST)


// Table 24 -- TPMA_ALGORITHM from outputBitDef <I/O>
typedef struct
{
    unsigned int asymmetric : 1;
    unsigned int symmetric  : 1;
    unsigned int hash       : 1;
    unsigned int object     : 1;
    unsigned int reserved5  : 4;
    unsigned int signing    : 1;
    unsigned int encrypting : 1;
    unsigned int method     : 1;
    unsigned int reserved9  : 21;
} TPMA_ALGORITHM ;

// Table 25 -- TPMA_OBJECT from outputBitDef <I/O>
typedef struct
{
    unsigned int reserved1           : 1;
    unsigned int fixedTPM            : 1;
    unsigned int stClear             : 1;
    unsigned int reserved4           : 1;
    unsigned int fixedParent         : 1;
    unsigned int sensitiveDataOrigin : 1;
    unsigned int userWithAuth        : 1;
    unsigned int adminWithPolicy     : 1;
    unsigned int Pad9                : 1;    //Inserted extra pad
    unsigned int reserved9           : 1;
    unsigned int noDA                : 1;
    unsigned int reserved11          : 5;
    unsigned int restricted          : 1;
    unsigned int decrypt             : 1;
    unsigned int sign                : 1;
    unsigned int Pad15               : 9;    //Inserted extra pad
    unsigned int softwareUse         : 4;
} TPMA_OBJECT ;

// Table 30 -- TPMA_SESSION Bits <I/O>
typedef struct
{
    unsigned int continueSession : 1;
    unsigned int auditExclusive  : 1;
    unsigned int auditReset      : 1;
    unsigned int reserved4       : 2;
    unsigned int decrypt         : 1;
    unsigned int encrypt         : 1;
    unsigned int audit           : 1;
} TPMA_SESSION ;

// Table 27 -- TPMA_LOCALITY from outputBitDef <I/O>
typedef struct
{
    unsigned int TPM2_LOC_ZERO  : 1;
    unsigned int TPM2_LOC_ONE   : 1;
    unsigned int TPM2_LOC_TWO   : 1;
    unsigned int TPM2_LOC_THREE : 1;
    unsigned int TPM2_LOC_FOUR  : 1;
    unsigned int reserved6     : 3;
} TPMA_LOCALITY ;

// Table 28 -- TPMA_PERMANENT from outputBitDef <O,S>
typedef struct
{
    unsigned int ownerAuthSet       : 1;
    unsigned int endorsementAuthSet : 1;
    unsigned int lockoutAuthSet     : 1;
    unsigned int reserved4          : 5;
    unsigned int disableClear       : 1;
    unsigned int inLockout          : 1;
    unsigned int tpmGeneratedEPS    : 1;
    unsigned int reserved8          : 21;
} TPMA_PERMANENT ;

// Table 29 -- TPMA_STARTUP_CLEAR from outputBitDef <O,S>
typedef struct
{
    unsigned int phEnable  : 1;
    unsigned int shEnable  : 1;
    unsigned int ehEnable  : 1;
    unsigned int reserved4 : 28;
    unsigned int orderly   : 1;
} TPMA_STARTUP_CLEAR ;

// Table 30 -- TPMA_MEMORY from outputBitDef <O,S>
typedef struct
{
    unsigned int sharedRAM         : 1;
    unsigned int sharedNV          : 1;
    unsigned int objectCopiedToRam : 1;
    unsigned int reserved4         : 29;
} TPMA_MEMORY ;

// Table 35 -- TPMA_CC Bits <O,S>
typedef struct
{
    unsigned int commandIndex : 16;
    unsigned int reserved2    : 6;
    unsigned int nv           : 1;
    unsigned int extensive    : 1;
    unsigned int flushed      : 1;
    unsigned int cHandles     : 3;
    unsigned int rHandle      : 1;
    unsigned int V            : 1;
    unsigned int reserved9    : 2;
} TPMA_CC ;

// Table 36 -- TPMI_YES_NO Type <I/O>
typedef BYTE TPMI_YES_NO;


// Table 32 -- TPMI_DH_OBJECT from outputType <I/O>
typedef TPM_HANDLE TPMI_DH_OBJECT;


// Table 33 -- TPMI_DH_PERSISTENT from outputType <I/O>
typedef TPM_HANDLE TPMI_DH_PERSISTENT;


// Table 34 -- TPMI_DH_ENTITY from outputType <I>
typedef TPM_HANDLE TPMI_DH_ENTITY;


// Table 35 -- TPMI_DH_PARENT from outputType <I>
typedef TPM_HANDLE TPMI_DH_PARENT;


// Table 36 -- TPMI_DH_PCR from outputType <I>
typedef TPM_HANDLE TPMI_DH_PCR;


// Table 37 -- TPMI_SH_AUTH_SESSION from outputType <I/O>
typedef TPM_HANDLE TPMI_SH_AUTH_SESSION;


// Table 38 -- TPMI_SH_HMAC from outputType <I/O>
typedef TPM_HANDLE TPMI_SH_HMAC;


// Table 39 -- TPMI_SH_POLICY from outputType <I/O>
typedef TPM_HANDLE TPMI_SH_POLICY;


// Table 40 -- TPMI_DH_CONTEXT from outputType <I/O>
typedef TPM_HANDLE TPMI_DH_CONTEXT;


// Table 41 -- TPMI_RH_HIERARCHY from outputType <I/O>
typedef TPM_HANDLE TPMI_RH_HIERARCHY;


// Table 42 -- TPMI_RH_HIERARCHY_AUTH from outputType <I>
typedef TPM_HANDLE TPMI_RH_HIERARCHY_AUTH;


// Table 43 -- TPMI_RH_PLATFORM from outputType <I>
typedef TPM_HANDLE TPMI_RH_PLATFORM;


// Table 44 -- TPMI_RH_OWNER from outputType <I>
typedef TPM_HANDLE TPMI_RH_OWNER;


// Table 45 -- TPMI_RH_ENDORSEMENT from outputType <I>
typedef TPM_HANDLE TPMI_RH_ENDORSEMENT;


// Table 46 -- TPMI_RH_PROVISION from outputType <I>
typedef TPM_HANDLE TPMI_RH_PROVISION;


// Table 47 -- TPMI_RH_CLEAR from outputType <I>
typedef TPM_HANDLE TPMI_RH_CLEAR;


// Table 48 -- TPMI_RH_NV_AUTH from outputType <I>
typedef TPM_HANDLE TPMI_RH_NV_AUTH;


// Table 49 -- TPMI_RH_LOCKOUT from outputType <I>
typedef TPM_HANDLE TPMI_RH_LOCKOUT;


// Table 50 -- TPMI_RH_NV_INDEX from outputType <I/O>
typedef TPM_HANDLE TPMI_RH_NV_INDEX;


// Table 51 -- TPMI_ALG_HASH from outputType <I/O>
typedef TPM2_ALG_ID TPMI_ALG_HASH;


// Table 52 -- TPMI_ALG_ASYM from outputType <I/O>
typedef TPM2_ALG_ID TPMI_ALG_ASYM;


// Table 53 -- TPMI_ALG_SYM from outputType <I/O>
typedef TPM2_ALG_ID TPMI_ALG_SYM;


// Table 54 -- TPMI_ALG_SYM_OBJECT from outputType <I/O>
typedef TPM2_ALG_ID TPMI_ALG_SYM_OBJECT;


// Table 55 -- TPMI_ALG_SYM_MODE from outputType <I/O>
typedef TPM2_ALG_ID TPMI_ALG_SYM_MODE;


// Table 56 -- TPMI_ALG_KDF from outputType <I/O>
typedef TPM2_ALG_ID TPMI_ALG_KDF;


// Table 57 -- TPMI_ALG_SIG_SCHEME from outputType <I/O>
typedef TPM2_ALG_ID TPMI_ALG_SIG_SCHEME;


// Table 58 -- TPMI_ST_COMMAND_TAG from outputType <I/O>
typedef TPM_ST TPMI_ST_COMMAND_TAG;


// Table 59 -- TPMS_ALGORITHM_DESCRIPTION from outputStructDef <O,S>
typedef struct
{
    TPM2_ALG_ID        alg;
    TPMA_ALGORITHM    attributes;
} TPMS_ALGORITHM_DESCRIPTION;

// Table 60 -- TPMU_HA from outputUnionDef <I/O,S>
typedef union
{
#if SHA1 == YES
    BYTE  sha1[SHA1_DIGEST_SIZE];
#endif
#if SHA256 == YES
    BYTE  sha256[SHA256_DIGEST_SIZE];
#endif
#if SM3_256 == YES
    BYTE  sm3_256[SM3_256_DIGEST_SIZE];
#endif
#if SHA384 == YES
    BYTE  sha384[SHA384_DIGEST_SIZE];
#endif
#if SHA512 == YES
    BYTE  sha512[SHA512_DIGEST_SIZE];
#endif
#if WHIRLPOOL512 == YES
    BYTE  whirlpool[WHIRLPOOL512_DIGEST_SIZE];
#endif

} TPMU_HA ;


// Table 61 -- TPMT_HA from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
    TPMU_HA          digest;
} TPMT_HA;

// Table 62 -- TPMS_HASH_DEF from outputStructDef <O,S>
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
    UINT32           digestSize;
    UINT32           blockSize;
} TPMS_HASH_DEF;

// Table 63 -- TPM2B_DIGEST from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      buffer[sizeof(TPMU_HA)];
} TPM2B_DIGEST;

// Table 64 -- TPM2B_DATA from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      buffer[sizeof(TPMT_HA)];
} TPM2B_DATA;

// Table 65 -- TPM2B_NONCE from outputTypeDef <I/O>
typedef TPM2B_DIGEST    TPM2B_NONCE;

// Table 66 -- TPM2B_AUTH from outputTypeDef <I/O>
typedef TPM2B_DIGEST    TPM2B_AUTH;

// Table 67 -- TPM2B_OPERAND from outputTypeDef <I/O>
typedef TPM2B_DIGEST    TPM2B_OPERAND;

// Table 68 -- TPM2B_EVENT from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      buffer[1024];
} TPM2B_EVENT;

// Table 69 -- TPM2B_MAX_BUFFER from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      buffer[MAX_DIGEST_BUFFER];
} TPM2B_MAX_BUFFER;

// Table 70 -- TPM2B_TIMEOUT from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      buffer[sizeof(UINT64)];
} TPM2B_TIMEOUT;

// Table 71 -- TPMU_NAME from outputUnionDef <S>
typedef union
{
    TPMU_HA  digest;
    TPM_HANDLE  handle;

} TPMU_NAME ;


// Table 72 -- TPM2B_NAME from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      name[sizeof(TPMU_NAME)];
} TPM2B_NAME;

// Table 73 -- TPMS_PCR_SELECT from outputStructDef <I/O>
typedef struct
{
    UINT8    sizeofSelect;
    BYTE     pcrSelect[PCR_SELECT_MAX];
} TPMS_PCR_SELECT;

// Table 74 -- TPMS_PCR_SELECTION from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_HASH    hash;
    UINT8            sizeofSelect;
    BYTE             pcrSelect[PCR_SELECT_MAX];
} TPMS_PCR_SELECTION;

// Table 78 -- TPMT_TK_CREATION from outputStructDef <I/O>
typedef struct
{
    TPM_ST               tag;
    TPMI_RH_HIERARCHY    hierarchy;
    TPM2B_DIGEST         digest;
} TPMT_TK_CREATION;

// Table 79 -- TPMT_TK_VERIFIED from outputStructDef <I/O>
typedef struct
{
    TPM_ST               tag;
    TPMI_RH_HIERARCHY    hierarchy;
    TPM2B_DIGEST         digest;
} TPMT_TK_VERIFIED;

// Table 80 -- TPMT_TK_AUTH from outputStructDef <I/O>
typedef struct
{
    TPM_ST               tag;
    TPMI_RH_HIERARCHY    hierarchy;
    TPM2B_DIGEST         digest;
} TPMT_TK_AUTH;

// Table 81 -- TPMT_TK_HASHCHECK from outputStructDef <I/O>
typedef struct
{
    TPM_ST               tag;
    TPMI_RH_HIERARCHY    hierarchy;
    TPM2B_DIGEST         digest;
} TPMT_TK_HASHCHECK;

// Table 82 -- TPMS_ALG_PROPERTY from outputStructDef <O,S>
typedef struct
{
    TPM2_ALG_ID        alg;
    TPMA_ALGORITHM    algProperties;
} TPMS_ALG_PROPERTY;

// Table 83 -- TPMS_TAGGED_PROPERTY from outputStructDef <O,S>
typedef struct
{
    TPM_PT    property;
    UINT32    value;
} TPMS_TAGGED_PROPERTY;

// Table 84 -- TPMS_TAGGED_PCR_SELECT from outputStructDef <O,S>
typedef struct
{
    TPM_PT    tag;
    UINT8     sizeofSelect;
    BYTE      pcrSelect[PCR_SELECT_MAX];
} TPMS_TAGGED_PCR_SELECT;

// Table 85 -- TPML_CC from outputStructDef <I/O>
typedef struct
{
    UINT32    count;
    TPM_CC    commandCodes[MAX_CAP_CC];
} TPML_CC;

// Table 86 -- TPML_ALG from outputStructDef <I/O>
typedef struct
{
    UINT32        count;
    TPM2_ALG_ID    alorithms[MAX_ALG_LIST_SIZE];
} TPML_ALG;

// Table 87 -- TPML_HANDLE from outputStructDef <O,S>
typedef struct
{
    UINT32        count;
    TPM_HANDLE    handle[MAX_CAP_HANDLES];
} TPML_HANDLE;

// Table 88 -- TPML_DIGEST from outputStructDef <I/O>
typedef struct
{
    UINT32          count;
    TPM2B_DIGEST    digests[8];
} TPML_DIGEST;

// Table 89 -- TPML_DIGEST_VALUES from outputStructDef <I/O>
typedef struct
{
    UINT32     count;
    TPMT_HA    digests[HASH_COUNT];
} TPML_DIGEST_VALUES;

// Table 90 -- TPM2B_DIGEST_VALUES from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      buffer[sizeof(TPML_DIGEST_VALUES)];
} TPM2B_DIGEST_VALUES;

// Table 91 -- TPML_PCR_SELECTION from outputStructDef <I/O>
typedef struct
{
    UINT32                count;
    TPMS_PCR_SELECTION    pcrSelections[HASH_COUNT];
} TPML_PCR_SELECTION;

// Table 92 -- TPML_ALG_PROPERTY from outputStructDef <O,S>
typedef struct
{
    UINT32               count;
    TPMS_ALG_PROPERTY    algProperties[MAX_CAP_ALGS];
} TPML_ALG_PROPERTY;

// Table 93 -- TPML_TAGGED_TPM_PROPERTY from outputStructDef <O,S>
typedef struct
{
    UINT32                  count;
    TPMS_TAGGED_PROPERTY    tpmProperty[MAX_TPM_PROPERTIES];
} TPML_TAGGED_TPM_PROPERTY;

// Table 94 -- TPML_TAGGED_PCR_PROPERTY from outputStructDef <O,S>
typedef struct
{
    UINT32                    count;
    TPMS_TAGGED_PCR_SELECT    pcrProperty[MAX_PCR_PROPERTIES];
} TPML_TAGGED_PCR_PROPERTY;

// Table 95 -- ECCCurve from outputTypeDef <I/O>
#if ECC == YES
typedef UINT16    TPM_ECC_CURVE_ID;
#endif

// Table 96 -- TPML_ECC_CURVE from outputStructDef <O,S>
#if ECC == YES
typedef struct
{
    UINT32              count;
    TPM_ECC_CURVE_ID    eccCurves[MAX_ECC_CURVES];
} TPML_ECC_CURVE;
#endif

// Table 97 -- TPMU_CAPABILITIES from outputUnionDef <O,S>
typedef union
{
    TPML_ALG_PROPERTY  algorithms;
    TPML_HANDLE  handles;
    TPML_CC  command;
    TPML_CC  ppCommands;
    TPML_CC  auditCommands;
    TPML_PCR_SELECTION  assignedPCR;
    TPML_TAGGED_TPM_PROPERTY  tpmProperties;
    TPML_TAGGED_PCR_PROPERTY  pcrProperties;
    TPML_ECC_CURVE  eccCurves;

} TPMU_CAPABILITIES ;


// Table 98 -- TPMS_CAPABILITY_DATA from outputStructDef <O,S>
typedef struct
{
    TPM_CAP              capability;
    TPMU_CAPABILITIES    data;
} TPMS_CAPABILITY_DATA;

// Table 99 -- TPMS_CLOCK_INFO from outputStructDef <I/O>
typedef struct
{
    UINT64         clock;
    TPMI_YES_NO    safe;
    UINT32         resetCount;
    UINT32         restartCount;
} TPMS_CLOCK_INFO;

// Table 100 -- TPMS_TIME_INFO from outputStructDef <I/O>
typedef struct
{
    UINT64             time;
    TPMS_CLOCK_INFO    clockInfo;
} TPMS_TIME_INFO;

// Table 101 -- TPMS_TIME_ATTEST_INFO from outputStructDef <O,S>
typedef struct
{
    TPMS_TIME_INFO    time;
    UINT64            firmwareVersion;
} TPMS_TIME_ATTEST_INFO;

// Table 102 -- TPMS_CERTIFY_INFO from outputStructDef <O,S>
typedef struct
{
    TPM2B_NAME    name;
    TPM2B_NAME    qualifiedName;
} TPMS_CERTIFY_INFO;

// Table 103 -- TPMS_QUOTE_INFO from outputStructDef <O,S>
typedef struct
{
    TPML_PCR_SELECTION    pcrSelect;
    TPM2B_DIGEST          pcrDigest;
} TPMS_QUOTE_INFO;

// Table 104 -- TPMS_COMMAND_AUDIT_INFO from outputStructDef <O,S>
typedef struct
{
    UINT32          auditCounter;
    TPM2_ALG_ID      digestAlg;
    TPM2B_DIGEST    auditDigest;
    TPM2B_DIGEST    commandDigest;
} TPMS_COMMAND_AUDIT_INFO;

// Table 105 -- TPMS_SESSION_AUDIT_INFO from outputStructDef <O,S>
typedef struct
{
    UINT8           exclusiveSesstion;
    TPM2B_DIGEST    sessionDigest;
} TPMS_SESSION_AUDIT_INFO;

// Table 106 -- TPMS_CREATION_INFO from outputStructDef <O,S>
typedef struct
{
    TPM2B_NAME    objectName;
} TPMS_CREATION_INFO;

// Table 107 -- TPMS_NV_CERTIFY_INFO from outputStructDef <O,S>
typedef struct
{
    TPM2B_MAX_BUFFER    nvContents;
} TPMS_NV_CERTIFY_INFO;

// Table 108 -- TPMI_ST_ATTEST from outputType <O,S>
typedef TPM_ST TPMI_ST_ATTEST;


// Table 109 -- TPMU_ATTEST from outputUnionDef <O,S>
typedef union
{
    TPMS_CERTIFY_INFO  certify;
    TPMS_CREATION_INFO  creation;
    TPMS_QUOTE_INFO  quote;
    TPMS_COMMAND_AUDIT_INFO  commandAudit;
    TPMS_SESSION_AUDIT_INFO  sessionAudit;
    TPMS_TIME_ATTEST_INFO  time;
    TPMS_NV_CERTIFY_INFO  nv;

} TPMU_ATTEST ;


// Table 110 -- TPMS_ATTEST from outputStructDef <O,S>
typedef struct
{
    TPM_GENERATED      magic;
    TPMI_ST_ATTEST     type;
    TPM2B_NAME         qualifiedSigner;
    TPM2B_DATA         extraData;
    TPMS_CLOCK_INFO    clockInfo;
    UINT64             firmwareVersion;
    TPMU_ATTEST        attested;
} TPMS_ATTEST;

// Table 111 -- TPM2B_ATTEST from outputStructDef <O,S>
typedef struct
{
    UINT16         size;
    TPMS_ATTEST    attestationData;
} TPM2B_ATTEST;

// Table 112 -- TPMS_AUTH_SESSION_COMMAND from outputStructDef <I>
/*typedef struct {
    TPMI_SH_AUTH_SESSION    sessionHandle;
    TPM2B_NONCE             nonce;
    TPMA_SESSION            sessionAttributes;
    TPM2B_AUTH              auth;
} TPMS_AUTH_SESSION_COMMAND;

// Table 113 -- TPMS_AUTH_SESSION_RESPONSE from outputStructDef <I/O>
typedef struct {
    TPM2B_NONCE     nonce;
    TPMA_SESSION    sessionAttributes;
    TPM2B_AUTH      auth;
} TPMS_AUTH_SESSION_RESPONSE;*/

// Table 118 - TPMS_AUTH_COMMAND Structure
typedef struct
{
    TPMI_SH_AUTH_SESSION sessionHandle;
    TPM2B_NONCE          nonce;
    TPMA_SESSION         sessionAttributes;
    TPM2B_AUTH           hmac;
} TPMS_AUTH_COMMAND;

// Table 119 - TPMS_AUTH_RESPONSE Structure
typedef struct
{
    TPM2B_NONCE  nonce;
    TPMA_SESSION sessionAttributes;
    TPM2B_AUTH   hmac;
} TPMS_AUTH_RESPONSE;

// Table 114 -- TPMS_AUTH_COMPUTE_NOT_BOUND from outputStructDef <S>
typedef struct
{
    TPM2B_AUTH      sessionKey;
    TPM2B_AUTH      authValue;
    TPM2B_DIGEST    pHash;
    TPM2B_NONCE     nonceNewer;
    TPM2B_NONCE     nonceOlder;
    TPMA_SESSION    sessionFlags;
} TPMS_AUTH_COMPUTE_NOT_BOUND;

// Table 115 -- TPMS_AUTH_COMPUTE_BOUND from outputStructDef <S>
typedef struct
{
    TPM2B_DIGEST    sessionKey;
    TPM2B_DIGEST    pHash;
    TPM2B_NONCE     nonceNewer;
    TPM2B_NONCE     nonceOlder;
    TPMA_SESSION    sessionFlags;
} TPMS_AUTH_COMPUTE_BOUND;

// Table 116 -- TPMS_XOR_PARMS from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_HASH    hash;
} TPMS_XOR_PARMS;

// Table 117 -- TPMS_AES_PARMS from outputStructDef <I/O>
#if AES == YES
typedef struct
{
    TPM_KEY_BITS         keyBits;
    TPMI_ALG_SYM_MODE    mode;
} TPMS_AES_PARMS;
#endif

// Table 118 -- TPMS_SMS4_PARMS from outputStructDef <I/O>
#if SMS4 == YES
typedef struct
{
    TPM_KEY_BITS         keyBits;
    TPMI_ALG_SYM_MODE    mode;
} TPMS_SMS4_PARMS;
#endif

// Table 119 -- TPMS_KEYEDHASH_PARMS from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_HASH    sign;
    TPMI_ALG_KDF     encrypt;
} TPMS_KEYEDHASH_PARMS;

// Table 120 -- TPMU_SYM_DEF from outputUnionDef <I/O>
typedef union
{
#if AES == YES
    TPMS_AES_PARMS  aes;
#endif
#if SMS4 == YES
    TPMS_SMS4_PARMS  sms4;
#endif
#if XOR == YES
    TPMS_XOR_PARMS  xor;
#endif

} TPMU_SYM_DEF ;


// Table 121 -- TPMT_SYM_DEF from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_SYM    algorithm;
    TPMU_SYM_DEF    details;
} TPMT_SYM_DEF;

// Table 122 -- TPMT_SYM_DEF_OBJECT from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_SYM_OBJECT    algorithm;
    TPMU_SYM_DEF           details;
} TPMT_SYM_DEF_OBJECT;

// Table 123 -- TPM2B_SYM_KEY from outputStructDef <I/O>
typedef struct
{
    UINT16    keySize;
    BYTE      key[MAX_SYM_DATA];
} TPM2B_SYM_KEY;

// Table 124 -- TPMS_SENSITIVE_CREATE from outputStructDef <I>
typedef struct
{
    TPM2B_AUTH       userAuth;
    TPM2B_SYM_KEY    data;
} TPMS_SENSITIVE_CREATE;

// Table 125 -- TPM2B_SENSITIVE_CREATE from outputStructDef <I,S>
typedef struct
{
    UINT16                   size;
    TPMS_SENSITIVE_CREATE    sensitive;
} TPM2B_SENSITIVE_CREATE;

// Table 126 -- TPMS_SCHEME_PKCS1v1_5 from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_PKCS1v1_5;
#endif

// Table 127 -- TPMS_SCHEME_PSS from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
    UINT16           saltSize;
} TPMS_SCHEME_PSS;
#endif

// Table 128 -- TPMS_SCHEME_ECDSA from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_ECDSA;
#endif

// Table 129 -- TPMS_SCHEME_ECDAA from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_ECDAA;
#endif

// Table 130 -- TPMS_SCHEME_HMAC from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_HMAC;
#endif

// Table 131 -- TPMU_SIG_SCHEME from outputUnionDef <I/O,S>
typedef union
{
#if RSASSA_PKCS1v1_5 == YES
    TPMS_SCHEME_PKCS1v1_5  pkcs1v1_5;
#endif
#if PSS == YES
    TPMS_SCHEME_PSS  pkcspss;
#endif
#if ECDSA == YES
    TPMS_SCHEME_ECDSA  ecdsa;
#endif
#if ECDAA == YES
    TPMS_SCHEME_ECDAA  ecdaa;
#endif
#if HMAC == YES
    TPMS_SCHEME_HMAC  hmac;
#endif

} TPMU_SIG_SCHEME ;


// Table 132 -- TPMT_SIG_SCHEME from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_SIG_SCHEME    scheme;
    TPMU_SIG_SCHEME        details;
} TPMT_SIG_SCHEME;

// Table 133 -- TPMS_SCHEME_MGF1 from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_MGF1;

// Table 134 -- TPMS_SCHEME_KDF1_SP800_56a from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_KDF1_SP800_56a;
#endif

// Table 135 -- TPMS_SCHEME_KDF2 from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_KDF2;

// Table 136 -- TPMS_SCHEME_KDF1_SP800_108 from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_KDF1_SP800_108;

// Table 137 -- TPMU_KDF_SCHEME from outputUnionDef <I/O,S>
typedef union
{
#if MGF1 == YES
    TPMS_SCHEME_MGF1  mgf1;
#endif
#if KDF1_SP800_56a == YES
    TPMS_SCHEME_KDF1_SP800_56a  kdf1_SP800_56a;
#endif
#if KDF2 == YES
    TPMS_SCHEME_KDF2  kdf2;
#endif
#if KDF1_SP800_108 == YES
    TPMS_SCHEME_KDF1_SP800_108  kdf1_sp800_108;
#endif

} TPMU_KDF_SCHEME ;


// Table 138 -- TPMT_KDF_SCHEME from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_KDF       scheme;
    TPMU_KDF_SCHEME    details;
} TPMT_KDF_SCHEME;

// Table 139 -- TPMI_ALG_RSA_SIG_SCHEME from outputType <I/O>
#if RSA == YES
typedef TPM2_ALG_ID TPMI_ALG_RSA_SIG_SCHEME;

#endif

// Table 140 -- TPMT_RSA_SIG_SCHEME from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    TPMI_ALG_RSA_SIG_SCHEME    scheme;
    TPMU_SIG_SCHEME            details;
} TPMT_RSA_SIG_SCHEME;
#endif

// Table 141 -- TPMS_RSA_PARMS from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    TPM_KEY_BITS           keyBits;
    UINT32                 exponent;
    TPMT_RSA_SIG_SCHEME    sign;
    TPMT_SYM_DEF_OBJECT    symmetric;
} TPMS_RSA_PARMS;
#endif

// Table 142 -- TPM2B_RSA_PUBLIC_KEY from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    UINT16    size;
    BYTE      buffer[MAX_RSA_KEY_BYTES];
} TPM2B_RSA_PUBLIC_KEY;
#endif

// Table 143 -- TPM2B_RSA_PRIVATE_KEY from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    UINT16    size;
    BYTE      buffer[MAX_RSA_KEY_BYTES/2];
} TPM2B_RSA_PRIVATE_KEY;
#endif

// Table 144 -- TPM_ECC_CURVE from outputConstDef <I/O>
#if ECC == YES
typedef TPM_ECC_CURVE_ID TPM_ECC_CURVE;

#define    TPM_ECC_NIST_P256    (TPM_ECC_CURVE)(0x0000)
#define    TPM_ECC_NIST_P384    (TPM_ECC_CURVE)(0x0001)
#define    TPM_ECC_NIST_P521    (TPM_ECC_CURVE)(0x0002)



#endif

// Table 145 -- TPM2B_ECC_PARAMETER from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    UINT16    size;
    BYTE      value[MAX_ECC_KEY];
} TPM2B_ECC_PARAMETER;
#endif

// Table 146 -- TPMS_ECC_POINT from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPM2B_ECC_PARAMETER    pointX;
    TPM2B_ECC_PARAMETER    pointY;
} TPMS_ECC_POINT;
#endif

// Table 147 -- TPM2B_ECC_POINT from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    UINT16            size;
    TPMS_ECC_POINT    point;
} TPM2B_ECC_POINT;
#endif

// Table 148 -- TPMI_ALG_ECC_SIG_SCHEME from outputType <I/O>
#if ECC == YES
typedef TPM2_ALG_ID TPMI_ALG_ECC_SIG_SCHEME;

#endif

// Table 149 -- TPMT_ECC_SIG_SCHEME from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPMI_ALG_ECC_SIG_SCHEME    scheme;
    TPMU_SIG_SCHEME            details;
} TPMT_ECC_SIG_SCHEME;
#endif

// Table 150 -- TPMS_ECC_PARMS from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPM_ECC_CURVE          curveID;
    TPMT_ECC_SIG_SCHEME    sign;
    TPMT_SYM_DEF_OBJECT    symmetric;
    TPMT_KDF_SCHEME        kdf;
} TPMS_ECC_PARMS;
#endif

// Table 151 -- TPMS_ALGORITHM_DETAIL_ECC from outputStructDef <O,S>
typedef struct
{
    TPM_ECC_CURVE_ID       curveID;
    UINT16                 keySize;
    TPMT_KDF_SCHEME        kdf;
    TPMT_ECC_SIG_SCHEME    sign;
    TPM2B_ECC_PARAMETER    p;
    TPM2B_ECC_PARAMETER    a;
    TPM2B_ECC_PARAMETER    b;
    TPM2B_ECC_PARAMETER    gX;
    TPM2B_ECC_PARAMETER    gY;
    TPM2B_ECC_PARAMETER    n;
    TPM2B_ECC_PARAMETER    h;
} TPMS_ALGORITHM_DETAIL_ECC;

// Table 152 -- TPMS_SIGNATURE_PKCS1v1_5 from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    TPMI_ALG_HASH           hash;
    TPM2B_RSA_PUBLIC_KEY    sig;
} TPMS_SIGNATURE_PKCS1v1_5;
#endif

// Table 153 -- TPMS_SIGNATURE_PSS from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    TPMI_ALG_HASH           hash;
    TPM2B_RSA_PUBLIC_KEY    sig;
    UINT16                  saltSize;
} TPMS_SIGNATURE_PSS;
#endif

// Table 154 -- TPMS_SIGNATURE_ECDSA from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPMI_ALG_HASH          hash;
    TPM2B_ECC_PARAMETER    signatureR;
    TPM2B_ECC_PARAMETER    signatureS;
} TPMS_SIGNATURE_ECDSA;
#endif

// Table 155 -- TPMU_SIGNATURE from outputUnionDef <I/O,S>
typedef union
{
#if RSASSA_PKCS1v1_5 == YES
    TPMS_SIGNATURE_PKCS1v1_5  pkcs1v1_5;
#endif
#if PSS == YES
    TPMS_SIGNATURE_PSS  pkcspss;
#endif
#if ECDSA == YES
    TPMS_SIGNATURE_ECDSA  ecdsa;
#endif
#if HMAC == YES
    TPMT_HA  hmac;
#endif

} TPMU_SIGNATURE ;


// Table 156 -- TPMT_SIGNATURE from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_SIG_SCHEME    sigAlg;
    TPMU_SIGNATURE         digest;
} TPMT_SIGNATURE;

// Table 157 -- TPMU_ENCRYPTED_SECRET from outputUnionDef <S>
typedef union
{
#if ECC == YES
    BYTE  ecc[sizeof(TPMS_ECC_POINT)];
#endif
#if RSA == YES
    BYTE  rsa[MAX_RSA_KEY_BYTES];
#endif
#if AES == YES
    BYTE  aes[sizeof(TPM2B_DIGEST)];
#endif
#if SMS4 == YES
    BYTE  sms4[sizeof(TPM2B_DIGEST)];
#endif
#if KEYEDHASH == YES
    BYTE  keyedHash[sizeof(TPM2B_DIGEST)];
#endif

} TPMU_ENCRYPTED_SECRET ;


// Table 158 -- TPM2B_ENCRYPTED_SECRET from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      secret[sizeof(TPMU_ENCRYPTED_SECRET)];
} TPM2B_ENCRYPTED_SECRET;

// Table 159 -- TPMI_ALG_PUBLIC from outputType <I/O>
typedef TPM2_ALG_ID TPMI_ALG_PUBLIC;


// Table 160 -- TPMS_PUBLIC_ID_SYM from outputStructDef <I/O>
typedef struct
{
    TPM2B_DIGEST    unique;
} TPMS_PUBLIC_ID_SYM;

// Table 161 -- PublicIDStructures from outputTypeDef <I/O>
typedef TPMS_PUBLIC_ID_SYM    TPMS_PUBLIC_ID_AES;
typedef TPMS_PUBLIC_ID_SYM    TPMS_PUBLIC_ID_SMS4;
typedef TPMS_PUBLIC_ID_SYM    TPMS_PUBLIC_ID_KEYEDHASH;

// Table 162 -- TPM2B_PUBLIC_ID_RSA from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    UINT16    keySize;
    BYTE      key[MAX_RSA_KEY_BYTES];
} TPM2B_PUBLIC_ID_RSA;
#endif

// Table 163 -- TPMS_PUBLIC_ID_ECC from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPM2B_ECC_PARAMETER    qX;
    TPM2B_ECC_PARAMETER    qY;
} TPMS_PUBLIC_ID_ECC;
#endif

// Table 164 -- TPMU_PUBLIC_PARMS from outputUnionDef <I/O,S>
typedef union
{
#if RSA == YES
    TPMS_RSA_PARMS  rsaDetail;
#endif
#if ECC == YES
    TPMS_ECC_PARMS  eccDetail;
#endif
#if KEYEDHASH == YES
    TPMS_KEYEDHASH_PARMS  keyedHash;
#endif
#if AES == YES
    TPMS_AES_PARMS  aesDetail;
#endif
#if SMS4 == YES
    TPMS_SMS4_PARMS  sms4Detail;
#endif

} TPMU_PUBLIC_PARMS ;


// Table 165 -- TPMU_PUBLIC_ID from outputUnionDef <I/O,S>
typedef union
{
#if AES == YES
    TPMS_PUBLIC_ID_AES  aes;
#endif
#if SMS4 == YES
    TPMS_PUBLIC_ID_SMS4  sms4;
#endif
#if KEYEDHASH == YES
    TPMS_PUBLIC_ID_KEYEDHASH  keyedHash;
#endif
#if RSA == YES
    TPM2B_PUBLIC_ID_RSA  rsa;
#endif
#if ECC == YES
    TPMS_PUBLIC_ID_ECC  ecc;
#endif

} TPMU_PUBLIC_ID ;


// Table 166 -- TPMT_PUBLIC_PARMS from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_PUBLIC      type;
    TPMU_PUBLIC_PARMS    parameters;
} TPMT_PUBLIC_PARMS;

// Table 167 -- TPMT_PUBLIC from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_PUBLIC      type;
    TPMI_ALG_HASH        nameAlg;
    TPMA_OBJECT          objectAttributes;
    TPM2B_DIGEST         authPolicy;
    TPMU_PUBLIC_PARMS    parameters;
    TPMU_PUBLIC_ID       unique;
} TPMT_PUBLIC;

// Table 168 -- TPM2B_PUBLIC from outputStructDef <I/O>
typedef struct
{
    UINT16         size;
    TPMT_PUBLIC    publicArea;
} TPM2B_PUBLIC;

// Table 169 -- TPMS_SENSITIVE_SYM from outputStructDef <I/O>
typedef struct
{
    TPM2B_DIGEST     obfuscate;
    TPM2B_SYM_KEY    symKey;
} TPMS_SENSITIVE_SYM;

// Table 170 -- PrivateSymmetricStructures from outputTypeDef <I/O>
typedef TPMS_SENSITIVE_SYM    TPMS_SENSITIVE_AES;
typedef TPMS_SENSITIVE_SYM    TPMS_SENSITIVE_SMS4;
typedef TPMS_SENSITIVE_SYM    TPMS_SENSITIVE_KEYEDHASH;

// Table 171 -- TPMS_SENSITIVE_RSA from outputStructDef <I/O>
#if RSA == YES
typedef struct
{
    UINT16           keySize;
    BYTE             key[(MAX_RSA_KEY_BYTES+1)/2];
    TPM2B_SYM_KEY    symKey;
} TPMS_SENSITIVE_RSA;
#endif

// Table 172 -- TPMS_SENSITIVE_ECC from outputStructDef <I/O>
#if ECC == YES
typedef struct
{
    TPM2B_ECC_PARAMETER    key;
    TPM2B_SYM_KEY          symKey;
} TPMS_SENSITIVE_ECC;
#endif

// Table 173 -- TPMU_SENSITIVE_COMPOSITE from outputUnionDef <I/O,S>
typedef union
{
#if RSA == YES
    TPMS_SENSITIVE_RSA  rsaDetail;
#endif
#if ECC == YES
    TPMS_SENSITIVE_ECC  eccDetail;
#endif
#if KEYEDHASH == YES
    TPMS_SENSITIVE_SYM  bitsDetail;
#endif
#if AES == YES
    TPMS_SENSITIVE_SYM  aesDetail;
#endif
#if SMS4 == YES
    TPMS_SENSITIVE_SYM  sms4Detail;
#endif

} TPMU_SENSITIVE_COMPOSITE ;


// Table 174 -- TPMT_SENSITIVE from outputStructDef <I/O>
typedef struct
{
    TPMI_ALG_PUBLIC             sensitiveType;
    TPM2B_AUTH                  authValue;
    TPMU_SENSITIVE_COMPOSITE    sensitive;
} TPMT_SENSITIVE;

// Table 175 -- TPM2B_SENSITIVE from outputStructDef <I/O>
typedef struct
{
    UINT16            size;
    TPMT_SENSITIVE    sensitiveArea;
} TPM2B_SENSITIVE;

// Table 176 -- TPMS_ENCRYPT from outputStructDef <I/O,S>
typedef struct
{
    TPM2B_DIGEST      integrity;
    TPMT_SENSITIVE    sensitive;
} TPMS_ENCRYPT;

// Table 177 -- TPM2B_PRIVATE from outputStructDef <I/O,S>
typedef struct
{
    UINT16    size;
    BYTE      data[sizeof(TPMS_ENCRYPT)];
} TPM2B_PRIVATE;

// Table 178 -- TPMS_ID_OBJECT from outputStructDef <I/O,S>
typedef struct
{
    TPM2B_DIGEST    integrity;
    TPM2B_DIGEST    protector;
} TPMS_ID_OBJECT;

// Table 179 -- TPM2B_ID_OBJECT from outputStructDef <I/O,S>
typedef struct
{
    UINT16    size;
    BYTE      credential[sizeof(TPMS_ID_OBJECT)];
} TPM2B_ID_OBJECT;

// Table 180 -- TPM_NV_INDEX from outputBitDef <S>
typedef struct
{
    unsigned int index : 21;
    unsigned int space : 3;
    unsigned int RH_NV : 8;
} TPM2_NV_INDEX ;

// Table 182 -- TPMA_NV from outputBitDef <I/O>
typedef struct
{
    unsigned int TPMA_NV_PPWRITE        : 1;
    unsigned int TPMA_NV_OWNERWRITE     : 1;
    unsigned int TPMA_NV_AUTHWRITE      : 1;
    unsigned int TPMA_NV_POLICYWRITE    : 1;
    unsigned int TPMA_NV_COUNTER        : 1;
    unsigned int TPMA_NV_BITS           : 1;
    unsigned int TPMA_NV_EXTEND         : 1;
    unsigned int reserved8              : 4;
    unsigned int TPMA_NV_WRITELOCKED    : 1;
    unsigned int TPMA_NV_WRITEALL       : 1;
    unsigned int TPMA_NV_WRITEDEFINE    : 1;
    unsigned int TPMA_NV_WRITE_STCLEAR  : 1;
    unsigned int TPMA_NV_GLOBALLOCK     : 1;
    unsigned int TPMA_NV_PPREAD         : 1;
    unsigned int TPMA_NV_OWNERREAD      : 1;
    unsigned int TPMA_NV_AUTHREAD       : 1;
    unsigned int TPMA_NV_POLICYREAD     : 1;
    unsigned int reserved18             : 5;
    unsigned int TPMA_NV_NO_DA          : 1;
    unsigned int TPMA_NV_ORDERLY        : 1;
    unsigned int TPMA_NV_CLEAR_STCLEAR  : 1;
    unsigned int TPMA_NV_READLOCKED     : 1;
    unsigned int TPMA_NV_WRITTEN        : 1;
    unsigned int TPMA_NV_PLATFORMCREATE : 1;
    unsigned int TPMA_NV_READ_STCLEAR   : 1;
} TPMA_NV ;

// Table 183 -- TPMS_NV_PUBLIC from outputStructDef <I/O>
typedef struct
{
    TPMI_RH_NV_INDEX    nvIndex;
    TPMI_ALG_HASH       nameAlg;
    TPMA_NV             attributes;
    TPM2B_DIGEST        authPolicy;
    UINT16              dataSize;
} TPMS_NV_PUBLIC;

// Table 184 -- TPM2B_NV_PUBLIC from outputStructDef <I/O>
typedef struct
{
    UINT16            size;
    TPMS_NV_PUBLIC    nvPublic;
} TPM2B_NV_PUBLIC;

// Table 185 -- TPM2B_CONTEXT from outputStructDef <I/O>
typedef struct
{
    UINT16    size;
    BYTE      contextData[MAX_CONTEXT_SIZE];
} TPM2B_CONTEXT;

// Table 186 -- TPMS_CONTEXT from outputStructDef <I/O>
typedef struct
{
    UINT32             sequence;
    TPMI_DH_CONTEXT    savedHandle;
    TPM2B_CONTEXT      contextBlob;
} TPMS_CONTEXT;

// Table 187 -- TPMS_CREATION_DATA from outputStructDef <O,S>
typedef struct
{
    TPML_PCR_SELECTION    pcrSelect;
    TPM2B_DIGEST          pcrDigest;
    TPMA_LOCALITY         locality;
    TPM2_ALG_ID            parentNameAlg;
    TPM2B_NAME            parentName;
    TPM2B_NAME            parentQualifiedName;
    TPM2B_DATA            outsideInfo;
} TPMS_CREATION_DATA;

// Table 188 -- TPM2B_CREATION_DATA from outputStructDef <O,S>
typedef struct
{
    UINT16                size;
    TPMS_CREATION_DATA    creationData;
} TPM2B_CREATION_DATA;


typedef struct
{
    TPM_ST tag;
    UINT32 paramSize;
    TPM_CC commandCode;
} TPM2_COMMAND_HEADER;

typedef struct
{
    TPM_ST tag;
    UINT32 paramSize;
    TPM_RC responseCode;
} TPM2_RESPONSE_HEADER;

#pragma pack (pop)
#endif
