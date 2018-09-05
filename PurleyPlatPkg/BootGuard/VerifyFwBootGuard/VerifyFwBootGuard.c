//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <AmiSmm.h>
#include <Protocol/SecSmiFlash.h>
#include "AmiCertificate.h"
#include <Library/MmPciBaseLib.h>
#include <Library/DebugLib.h>
#include <Uefi/UefiSpec.h>

extern EFI_GUID gSecureSMIFlashProtocolGuid;

EFI_SEC_SMI_FLASH_PROTOCOL		BootGuardSaveSetFlUpdMethodPointer;
EFI_SEC_SMI_FLASH_PROTOCOL              *SecSmiFlash;
UINT8                                   *BiosRom;
UINTN                                   BiosRomSize;

// ME
#define ME_BUS                               0
#define ME_DEVICE_NUMBER                     22
#define HECI_FUNCTION_NUMBER                 0

#define ME_VENDOR_ID                         0x00
// APTIOV_SERVER_OVERRIDE_RC_START : Changed as per TXT_BtG_BiosSpec
#define R_ME_HFSTS6                          0x6C
// APTIOV_SERVER_OVERRIDE_RC_END : Changed as per TXT_BtG_BiosSpec

#pragma pack (1)
#define  BASE_4GB                            0x0000000100000000ULL
//
// FIT definition
//
#define FIT_TABLE_TYPE_HEADER                0x0
#define FIT_TABLE_TYPE_MICROCODE             0x1
#define FIT_TABLE_TYPE_STARTUP_ACM           0x2
#define FIT_TABLE_TYPE_KEY_MANIFEST          0xB
#define FIT_TABLE_TYPE_BOOT_POLICY_MANIFEST  0xC

typedef struct {
    UINT64 Address;
    UINT8  Size[3];
    UINT8  Reserved;
    UINT16 Version;
    UINT8  Type : 7;
    UINT8  Cv   : 1;
    UINT8  Chksum;
} FIRMWARE_INTERFACE_TABLE_ENTRY;

//
// ACM definition
//
#define ACM_PKCS_1_5_RSA_SIGNATURE_SIZE  256
#define ACM_HEADER_FLAG_DEBUG_SIGNED     BIT15

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
// Manifest definition
//
#define SHA256_DIGEST_SIZE  32

typedef struct {
    UINT16 HashAlg;
    UINT16 Size;
    UINT8  HashBuffer[SHA256_DIGEST_SIZE];
} HASH_STRUCTURE;

#define RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT   (RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT/8)

typedef struct {
    UINT8  Version;
    UINT16 KeySize;
    UINT32 Exponent;
    UINT8  Modulus[RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT];
} RSA_PUBLIC_KEY_STRUCT;

#define RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT   (RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT/8)
typedef struct {
    UINT8  Version;
    UINT16 KeySize;
    UINT16 HashAlg;
    UINT8  Signature[RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT];
} RSASSA_SIGNATURE_STRUCT;

typedef struct {
    UINT8                   Version;
    UINT16                  KeyAlg;
    RSA_PUBLIC_KEY_STRUCT   Key;
    UINT16                  SigScheme;
    RSASSA_SIGNATURE_STRUCT Signature;
} KEY_SIGNATURE_STRUCT;

#define BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_ID  (*(UINT64 *)"__ACBP__")
typedef struct {
    UINT8  StructureId[8];
    UINT8  StructVersion;
    UINT8  HdrStructVersion;
    UINT8  PMBPMVersion;
    UINT8  BPSVN;
    UINT8  ACMSVN;
    UINT8  Reserved;
    UINT16 NEMDataStack;
} BOOT_POLICY_MANIFEST_HEADER;

#define IBB_SEGMENT_FLAG_IBB      0x0
#define IBB_SEGMENT_FLAG_NON_IBB  0x1
typedef struct {
    UINT8  Reserved[2];
    UINT16 Flags;
    UINT32 Base;
    UINT32 Size;
} IBB_SEGMENT_ELEMENT;

#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__IBBS__")
#define IBB_FLAG_AUTHORITY_MEASURE                     0x4

typedef struct {
    UINT8               StructureId[8];
    UINT8               StructVersion;
    UINT8               Reserved1[2];
    UINT8               PbetValue;
    UINT32              Flags;
    UINT64              IbbMchBar;
    UINT64              VtdBar;
    UINT32              PmrlBase;
    UINT32              PmrlLimit;
    UINT64              Reserved2[2];
    HASH_STRUCTURE      PostIbbHash;
    UINT32              EntryPoint;
    HASH_STRUCTURE      Digest;
    UINT8               SegmentCount;
    IBB_SEGMENT_ELEMENT IbbSegment[4];
} IBB_ELEMENT;

#define BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PMDA__")
typedef struct {
    UINT8  StructureId[8];
    UINT8  StructVersion;
    UINT16 PmDataSize;
} PLATFORM_MANUFACTURER_ELEMENT;

#define BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PMSG__")
typedef struct {
    UINT8                StructureId[8];
    UINT8                StructVersion;
    KEY_SIGNATURE_STRUCT KeySignature;
} BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT;

#define KEY_MANIFEST_STRUCTURE_ID  (*(UINT64 *)"__KEYM__")
typedef struct {
    UINT8                StructureId[8];
    UINT8                StructVersion;
    UINT8                KeyManifestVersion;
    UINT8                KmSvn;
    UINT8                KeyManifestId;
    HASH_STRUCTURE       BpKey;
    KEY_SIGNATURE_STRUCT KeyManifestSignature;
} KEY_MANIFEST_STRAUCTURE;

#pragma pack ()

UINT64
FindFitEntryData (
    IN UINT8 Type
)
{
    FIRMWARE_INTERFACE_TABLE_ENTRY *FitEntry;
    UINT32                         EntryNum, Index;
    UINT64                         FitTableEntry, FitTableOffset;
    UINT64                         BiosRomStaringAddress = 0xFFFFFFFF - FLASH_SIZE + 1;

    FitTableEntry = *(UINT64 *)(UINTN)(BiosRom + FLASH_SIZE - 0x40);
    if ( FitTableEntry < BiosRomStaringAddress ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] The Fit entry(%x) is incorrect\n", FitTableEntry));
        return 0; 
    }
    FitTableOffset = (UINT64)(UINTN)BiosRom + (FitTableEntry - BiosRomStaringAddress);    
    FitEntry = (FIRMWARE_INTERFACE_TABLE_ENTRY *)(UINTN)FitTableOffset;

    DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] FitTableOffset = %x\n", FitTableOffset));
    DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] FitEntry[0].Address = %x\n", FitEntry[0].Address));
    
    if (FitEntry[0].Address != *(UINT64 *)"_FIT_   ") {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] The signature of Fit table is incorrect\n"));
        return 0;
    }
    if (FitEntry[0].Type != FIT_TABLE_TYPE_HEADER) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] FitEntry[0].Type is incorrect\n"));
        return 0;
    }
    EntryNum = *(UINT32 *)(&FitEntry[0].Size[0]) & 0xFFFFFF;
    for (Index = 0; Index < EntryNum; Index++) {
        if (FitEntry[Index].Type == Type) {
            DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] The Fit type[%x] Physical Address = %x\n", Type, FitEntry[Index].Address ));
            return FitEntry[Index].Address;
        }
    }

    DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] The Fit type isn't found\n"));
    return 0;
}

UINT64
FindAcm (
    VOID
)
{
    return FindFitEntryData (FIT_TABLE_TYPE_STARTUP_ACM);
}

UINT64
FindBpm (
    VOID
)
{
    return FindFitEntryData (FIT_TABLE_TYPE_BOOT_POLICY_MANIFEST);
}

UINT64
FindKm (
    VOID
)
{
    return FindFitEntryData (FIT_TABLE_TYPE_KEY_MANIFEST);
}

EFI_STATUS
IsAcmCorrect (
    VOID
)
{
    ACM_FORMAT                        *Acm;
    UINT32                            HFS, TargetACMSVN;
    UINT64                            PhysicalAcmAddress, RomAcmAddress;

    PhysicalAcmAddress = FindAcm ();
    if ( PhysicalAcmAddress == 0 ) return EFI_SECURITY_VIOLATION;

    RomAcmAddress = (UINT64)BiosRom + (PhysicalAcmAddress - (BASE_4GB - BiosRomSize));

    Acm = (ACM_FORMAT*)RomAcmAddress;
    if ( Acm == NULL ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] Acm isn't found\n"));
        return EFI_SECURITY_VIOLATION;
    }

    //
    // APTIOV_SERVER_OVERRIDE_RC_START : As per Table 2-2 Server ACM Sizes, ACM should be 256 KB aligned for purley
    //
    if ( (PhysicalAcmAddress & 0x0003FFFF) != 0 ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] Acm isn't 256KB alignment\n"));
        return EFI_SECURITY_VIOLATION;
    }
    //
    // APTIOV_SERVER_OVERRIDE_RC_END : Below limitation is not mentioned in TXT BtG Spec & this condition does not statify for Purley
    //
    /*if ( PhysicalAcmAddress < (UINT32)BASE_4GB - 0x200000 ) {
        DEBUG((EFI-D_ERROR, "[VerifyFwBootGuard.c] Boot Guard ACM must be within the range of 4GB to 4GB minus 2MB\n"));
        return EFI_SECURITY_VIOLATION;
    }*/

    //
    // 1. Check that ACM is correct for the platform and it will not be revoked on the target platform
    //    a. This is to make sure ACMSVN from the ACM Header is greater or equal to
    //       Host Firmware Status Register #6 Bit[13:10]
    //
    if ( MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + ME_VENDOR_ID) == 0xFFFFFFFF  ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] ME is disabled on the target platform, we can't verify the Boot Guard ACM\n"));
        return EFI_SUCCESS;
    }
// APTIOV_SERVER_OVERRIDE_RC_START : Changed as per TXT_BtG_BiosSpec
    HFS = MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_ME_HFSTS6);
// APTIOV_SERVER_OVERRIDE_RC_END : Changed as per TXT_BtG_BiosSpec

    TargetACMSVN = (HFS & (BIT10 | BIT11 | BIT12 | BIT13)) >> 10;
    if ( Acm->AcmSvn < TargetACMSVN ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] ACMSVN is incorrect for target platform %x, %x\n", Acm->AcmSvn,TargetACMSVN));
        return EFI_SECURITY_VIOLATION;
    }

    DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] ACM is correct for the platform\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
IsKmCorrect (
    VOID
)
{
    KEY_MANIFEST_STRAUCTURE           *Km;
    UINT32                            HFS, TargetKMSVN;
    UINT64                            PhysicalKmAddress, RomKmAddress;

    PhysicalKmAddress = FindKm ();
    if ( PhysicalKmAddress == 0 ) return EFI_SECURITY_VIOLATION;

    RomKmAddress = (UINT64)BiosRom + (PhysicalKmAddress - (BASE_4GB - BiosRomSize));

    Km = (KEY_MANIFEST_STRAUCTURE*)RomKmAddress;
    if ( Km == NULL ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] Km isn't found\n"));
        return EFI_SECURITY_VIOLATION;
    }

    //
    // 2.6.1 Key Manifest Integration Requirement
    // KM must be uncompressed and reside within 4GB to 4GB minus 4MB region. It also
    // must be aligned to 16 bytes boundary.
    //
    if ( (PhysicalKmAddress & 0xF) != 0 ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] KM must be aligned to 16 bytes boundary.\n"));
        return EFI_SECURITY_VIOLATION;
    }

    if ( PhysicalKmAddress < (UINT32)(BASE_4GB - 0x400000) ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] KM must be uncompressed and reside within 4GB to 4GB minus 4MB region.\n"));
        return EFI_SECURITY_VIOLATION;
    }

    //
    // 2. Check that KM is correct for the platform and it will not be revoked on the target platform
    //    a. This is to make sure KMSVN from the KM Header is greater or equal to
    //       Host Firmware Status Register #6 Bit[17:14]
    //

    if ( MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + ME_VENDOR_ID) == 0xFFFFFFFF  ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] ME is disabled on the target platform, we can't verify the Boot Guard Km\n"));
        return EFI_SUCCESS;
    }
// APTIOV_SERVER_OVERRIDE_RC_START : Changed as per TXT_BtG_BiosSpec
    HFS = MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_ME_HFSTS6);
// APTIOV_SERVER_OVERRIDE_RC_END : Changed as per TXT_BtG_BiosSpec

    TargetKMSVN = (HFS & (BIT14 | BIT15 | BIT16 | BIT17)) >> 14;
    if ( Km->KmSvn < TargetKMSVN ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] KMSVN is incorrect for target platform\n"));
        return EFI_SECURITY_VIOLATION;
    }

    DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] KM is correct for the platform \n"));
    return EFI_SUCCESS;
}

EFI_STATUS
IsBpmCorrect (
    VOID
)
{
    BOOT_POLICY_MANIFEST_HEADER       *Bpm;
    UINT32                            HFS, TargetBPMSVN;
    UINT64                            PhysicalBpmAddress, RomBpmAddress;

    PhysicalBpmAddress = FindBpm ();
    if ( PhysicalBpmAddress == 0 ) return EFI_SECURITY_VIOLATION;

    RomBpmAddress = (UINT64)BiosRom + (PhysicalBpmAddress - (BASE_4GB - BiosRomSize));

    Bpm = (BOOT_POLICY_MANIFEST_HEADER*)RomBpmAddress;
    if ( Bpm == NULL ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] Bpm isn't found\n"));
        return EFI_SECURITY_VIOLATION;
    }

    //
    // 2.5.1 Boot Policy Manifest Integration Requirement
    // BPM must be uncompressed and reside within the range of 4GB to 4GB Minus 4MB. It
    // also must be aligned to 16 bytes boundary.
    //
    if ( (PhysicalBpmAddress & 0xF) != 0 ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] BPM must be aligned to 16 bytes boundary.\n"));
        return EFI_SECURITY_VIOLATION;
    }

    if ( PhysicalBpmAddress < (UINT32)(BASE_4GB - 0x400000) ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] BPM must be uncompressed and reside within 4GB to 4GB minus 4MB region.\n"));
        return EFI_SECURITY_VIOLATION;
    }

    //
    // 3. Check that BPM is correct for the platform and it will not be revoked on the target platform
    //    a. This is to make sure BPSVN from the BPM Header is greater or equal to
    //       Host Firmware Status Register #6 Bit[21:18]
    //

    if ( MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + ME_VENDOR_ID) == 0xFFFFFFFF  ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] ME is disabled on the target platform, we can't verify the Boot Guard Bpm\n"));
        return EFI_SUCCESS;
    }
// APTIOV_SERVER_OVERRIDE_RC_START : Changed as per TXT_BtG_BiosSpec
    HFS = MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_ME_HFSTS6);
// APTIOV_SERVER_OVERRIDE_RC_END : Changed as per TXT_BtG_BiosSpec

    TargetBPMSVN = (HFS & (BIT18 | BIT19 | BIT20 | BIT21)) >> 18;
    if ( Bpm->BPSVN < TargetBPMSVN ) {
        DEBUG((EFI_D_ERROR, "[VerifyFwBootGuard.c] BPMSVN is incorrect for target platform\n"));
        return EFI_SECURITY_VIOLATION;
    }

    DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] BPM is correct for the platform\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
BootGuardValidate (
    IN OUT UINT8     **pFwCapsule
) {
    APTIO_FW_CAPSULE_HEADER       *FWCapsuleHdr;
    EFI_GUID                      gFWCapsuleGuid = APTIO_FW_CAPSULE_GUID;
    EFI_STATUS	                  Status;

    FWCapsuleHdr = (APTIO_FW_CAPSULE_HEADER*)*pFwCapsule;
    BiosRom = (UINT8*)*(pFwCapsule);
    BiosRomSize = FLASH_SIZE;

    if(!guidcmp((EFI_GUID*)&FWCapsuleHdr->CapHdr.CapsuleGuid, &gFWCapsuleGuid)) {
        DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] CapsuleGuid OK\n"));
    	BiosRom = (UINT8*)(SecSmiFlash->pFwCapsule);
        BiosRomSize = (FWCapsuleHdr->CapHdr.CapsuleImageSize - FWCapsuleHdr->RomImageOffset);
    }
    {
        // Skip BootGuard Validation if ME FW Capsule only.
        EFI_GUID                      gMeFwCapsuleGuid = \
        { 0x9F8B1DEF, 0xB62B, 0x45F3, 0x82, 0x82, 0xBF, 0xD7, 0xEA, 0x19, 0x80, 0x1B };
        if (!guidcmp((EFI_GUID*)(*pFwCapsule + 0x60), &gMeFwCapsuleGuid)) return EFI_SUCCESS;
    }    
    
    DEBUG((EFI_D_INFO, "[VerifyFwBootGuard.c] BiosRom = %x, BiosRomSize = %x\n", BiosRom, BiosRomSize));    
    
    //
    // 1. Verify that ACM is correct.
    //
    Status = IsAcmCorrect();
    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    //
    // 2. Verify that KM is correct.
    //
    Status = IsKmCorrect();
    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    //
    // 3. Verify that BPM is correct.
    //
    Status = IsBpmCorrect();
    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
BootGuardSetFlUpdMethod (
    IN OUT FUNC_FLASH_SESSION_BLOCK    *pSessionBlock
) {
    EFI_STATUS	Status;
    EFI_CAPSULE_BLOCK_DESCRIPTOR *pCapsulePtr;
    UINT8                        *DataBuff;

    Status = BootGuardSaveSetFlUpdMethodPointer.SetFlUpdMethod( pSessionBlock );
    if ( EFI_ERROR(Status) ) {
        DEBUG((EFI_D_ERROR,"[VerifyFwBootGuard.c] Secure Flash verifies the BIOS ROM is fail\n\n"));
        return Status;
    }
    
    if ( pSessionBlock->FlUpdBlock.FlashOpType == FlRecovery ) return EFI_SUCCESS;

    pCapsulePtr = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)(UINTN)pSessionBlock->FlUpdBlock.FwImage.CapsuleMailboxPtr[0];
    if (pCapsulePtr){
        DataBuff = (UINT8*)(pCapsulePtr[0].Union.DataBlock);
    } else {
        DataBuff = (UINT8*)(SecSmiFlash->pFwCapsule);
    }
	Status = BootGuardValidate( (UINT8**)&DataBuff );    
    if ( EFI_ERROR(Status) ) {
        DEBUG((EFI_D_ERROR,"[VerifyFwBootGuard.c] Boot Guard verifies the BIOS ROM is fail\n\n"));
        return Status;
    }

    DEBUG((EFI_D_INFO,"[VerifyFwBootGuard.c] Verify ROM is Pass\n"));

    return Status;
}

EFI_STATUS
InSmmFunction (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS          Status;

    InitAmiSmmLib( ImageHandle, SystemTable );

    DEBUG((EFI_D_INFO,"[VerifyFwBootGuard.c] "__FUNCTION__" - Start\n"));

    Status = pSmst->SmmLocateProtocol(&gSecureSMIFlashProtocolGuid, NULL, &SecSmiFlash);
    if ( EFI_ERROR(Status) ) {
        DEBUG((EFI_D_ERROR,"[VerifyFwBootGuard.c] Locate gSecureSMIFlashProtocolGuid is fail\n"));
        return Status;
    }

    BootGuardSaveSetFlUpdMethodPointer.SetFlUpdMethod = SecSmiFlash->SetFlUpdMethod;
    SecSmiFlash->SetFlUpdMethod = BootGuardSetFlUpdMethod;

    DEBUG((EFI_D_INFO,"[VerifyFwBootGuard.c] "__FUNCTION__" - End\n"));
    return Status;
}

EFI_STATUS
VerifyFwBootGuardEntryPoint (
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
    InitAmiLib(ImageHandle, SystemTable);

    return InitSmmHandlerEx(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
