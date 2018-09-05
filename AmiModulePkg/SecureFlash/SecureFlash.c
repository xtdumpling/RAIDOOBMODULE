//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <SecureFlash.h>
#include <FlashUpd.h>
#include <Protocol/AmiDigitalSignature.h>
#include "AmiCertificate.h"
#include <Library/PciLib.h>
#include <Library/DebugLib.h>

#include <Setup.h>
#include <AutoId.h>

//#include <Protocol/AMIPostMgr.h> 

static AMI_DIGITAL_SIGNATURE_PROTOCOL *mAmiSig = NULL;
//
// Global variables
//
extern EFI_BOOT_SERVICES    *pBS;

const UINT8 *FwBadKey = "$BAD";

VOID UpdateSecureFlashBrowserInfo (VOID);

// Ami Test Rsa2048 key
#if defined (AmiTestKey1_hash)
UINT8             AmiKey1Hash[SHA256_DIGEST_SIZE]=AmiTestKey1_hash;
#else
UINT8             AmiKey1Hash[SHA256_DIGEST_SIZE] = {
   0x56, 0xd2, 0xe8, 0x18, 0xf9, 0x6e, 0x9e, 0x2f, 0x3f, 0xa4, 0x80, 0xff, 0xc7, 0xb3, 0xb4, 0x41, 
   0x62, 0x6c, 0x10, 0xf7, 0xc1, 0xc8, 0xbb, 0xb1, 0xf4, 0x44, 0xba, 0x86, 0xce, 0x7d, 0xd2, 0x70
};
#endif
// Ami Test Key2 x509 Certificate
#if defined (AmiTestKey21_hash) && defined (AmiTestKey22_hash)
UINT8             AmiKey21Hash[SHA256_DIGEST_SIZE]=AmiTestKey21_hash;
UINT8             AmiKey22Hash[SHA256_DIGEST_SIZE]=AmiTestKey22_hash;    
#else    
 UINT8             AmiKey21Hash[SHA256_DIGEST_SIZE]={
  0x42, 0x8c, 0x2a, 0x22, 0x01, 0x91, 0x0d, 0xec, 0x7d, 0x54, 0x23, 0xf5, 0xd8, 0x33, 0x23, 0x3e, 
  0x91, 0x9e, 0x49, 0xb8, 0xb7, 0x09, 0x03, 0xcb, 0x46, 0x65, 0xe0, 0xaa, 0x33, 0xe8, 0x95, 0xcf
};
// Sha256 hash of Tbs cert             
UINT8             AmiKey22Hash[SHA256_DIGEST_SIZE]={
  0x7f, 0xc4, 0x28, 0xe4, 0x26, 0xb2, 0xa2, 0x30, 0x22, 0xad, 0x2f, 0x55, 0xd5, 0x22, 0xc6, 0x9b, 
  0x8f, 0x3d, 0x7e, 0x79, 0x11, 0x21, 0x9c, 0xd3, 0xe9, 0x72, 0xc1, 0xe1, 0x8b, 0xee, 0x21, 0x70
};
#endif

//----------------------------------------------------------------------------
// Function definitions
//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   UpdateSecureFlashBrowserInfo
//
// Description: 
//
// Input:       none
//
// Output:      none
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UpdateSecureFlashBrowserInfo ()
{
    EFI_STATUS  Status;
    UINTN       Size;
    UINT32      Attributes;

    SECURE_FLASH_SETUP_VAR SecureFlashSetupVar = {0,0,0,0,0};
    EFI_GUID gSecureFlashSetupVarGuid = AMI_SECURE_FLASH_SETUP_VAR_GUID;  

    UINT8            FlUpdatePolicy = FlashUpdatePolicy;
    CRYPT_HANDLE     pPubKeyHndl;
    EFI_GUID         PRKeyGuid  = PR_KEY_GUID;

//    UINT8            Byte;
    UINT8            Digest[SHA256_DIGEST_SIZE]={0};
    UINT8            *pDigest = Digest;
    INTN             err;

    ///////////////////////////////////////////////////////////////////////////////
    //
    // Updated Secure Flash Setup Var
    //
    ///////////////////////////////////////////////////////////////////////////////
    // Key Store ffs
    // 0 - Disabled, No Key
    // 1 - Enabled, RSA2048
    // 2 - Enabled, SHA256
    // 3 - Enabled, x509
    // Key Type
    // 0 - Disabled, No Key
    // 1 - Dummy (0xFF...)
    // 1 - AMI Test Key
    // 2 - OEM Key
    // Mode
    // 0 - Flash Updates disabled
    // 1 - Flash Update: Runtime, Recovery, Capsule
    // 2 - Flash Update: Runtime, Recovery
    // 3 - Flash Update: Runtime
    // Rollback
    // 0 - Fw revision rollback not implemented
    // 1 - Rollback fully implemented
    // 2 - Rollback is disabled
    // Lock 
    // 0 - Flash Lock information N/A
    // 1 - Flash Lock Disabled
    // 2 - Flash Lock Enabled    
    //
    // Look up for FwKey 
    //
    if(mAmiSig == NULL)
    {
        Status = pBS->LocateProtocol(&gAmiDigitalSignatureProtocolGuid, NULL, &mAmiSig); 
        if(EFI_ERROR(Status) || !mAmiSig) {
            return;
        }
    }

    Size = sizeof(SECURE_FLASH_SETUP_VAR);
    pRS->GetVariable (AMI_SECURE_FLASH_SETUP_VAR, 
            &gSecureFlashSetupVarGuid,  
            &Attributes,
            &Size,
            &SecureFlashSetupVar);
// Get PRKey ptr
    pPubKeyHndl.Blob = NULL;
    pPubKeyHndl.BlobSize = 0;
    Status = mAmiSig->GetKey(mAmiSig, &pPubKeyHndl, &PRKeyGuid, pPubKeyHndl.BlobSize, 0);
DEBUG ((SecureMod_DEBUG_LEVEL,"GetKey %r (%x, %d bytes)\n",Status, pPubKeyHndl.Blob,pPubKeyHndl.BlobSize));
    if (!EFI_ERROR(Status) || Status == EFI_BUFFER_TOO_SMALL) {
        if(!guidcmp(&pPubKeyHndl.AlgGuid, &gEfiCertRsa2048Guid)) 
            SecureFlashSetupVar.Key = 1;
        if(!guidcmp(&pPubKeyHndl.AlgGuid, &gEfiCertSha256Guid))
            SecureFlashSetupVar.Key = 2;
        if(!guidcmp(&pPubKeyHndl.AlgGuid, &gEfiCertX509Guid))
            SecureFlashSetupVar.Key = 3;
        // check If dummy key - return Mode = 0
//        Byte = pPubKeyHndl.Blob[0];
//        for(Size = 1; Size < pPubKeyHndl.BlobSize && (Byte == pPubKeyHndl.Blob[Size]); Size++);
//        if(Size == pPubKeyHndl.BlobSize) {
        if(*((UINT32*)(pPubKeyHndl.Blob)) == *(UINT32*)FwBadKey) {
            SecureFlashSetupVar.Type = 1; // dummy key
            FlUpdatePolicy &= ~(FlCapsule | FlRuntime);
        } else {
// hash the Rsa or x509 key    
// x509 hash of full Cert file         
            SecureFlashSetupVar.Type = 2; // Ami test Key
            if(SecureFlashSetupVar.Key == 2)
                pDigest = pPubKeyHndl.Blob;
            else 
                Status = mAmiSig->Hash(mAmiSig, 
                        &gEfiHashAlgorithmSha256Guid, 
                        1, 
                        &pPubKeyHndl.Blob,
                        (const UINTN*)&pPubKeyHndl.BlobSize, 
                        Digest);
            err = MemCmp(AmiKey1Hash, pDigest, SHA256_DIGEST_SIZE);
            DEBUG ((SecureMod_DEBUG_LEVEL, "Compare Key1 == FW Key : %a\n", (!err)?"Yes":"No"));
            if(err) {
                err = MemCmp(AmiKey21Hash, pDigest, SHA256_DIGEST_SIZE);
                DEBUG ((SecureMod_DEBUG_LEVEL, "Compare Key21 == FW Key : %a\n", (!err)?"Yes":"No"));
                if(err) {
                    err = MemCmp(AmiKey22Hash, pDigest, SHA256_DIGEST_SIZE);
                    DEBUG ((SecureMod_DEBUG_LEVEL, "Compare Key22 == FW Key : %a\n", (!err)?"Yes":"No"));
                }
            }
            if(err) 
                SecureFlashSetupVar.Type = 3; // OEM Key
        }
    }
    //
    // Flash Upd Mode
    //
#if FWCAPSULE_RECOVERY_SUPPORT == 0
    FlUpdatePolicy &=~FlCapsule;
#endif    
    switch(FlUpdatePolicy & (FlCapsule | FlRecovery | FlRuntime))
    {
        case (FlCapsule | FlRecovery | FlRuntime):
             SecureFlashSetupVar.Mode = 1;
             break;
        case (FlRecovery | FlRuntime):
             SecureFlashSetupVar.Mode = 2;
             break;
        case (FlCapsule | FlRecovery):
             SecureFlashSetupVar.Mode = 3;
             break;
        case (FlRuntime):
             SecureFlashSetupVar.Mode = 4;
             break;
        case (FlRecovery):
             SecureFlashSetupVar.Mode = 5;
             break;
        default:
            SecureFlashSetupVar.Mode = 0;
            break;
    }
    //
    // Rollback protection
    //
    SecureFlashSetupVar.Rollback = 0; // rollback protection undefined
    if(SecureFlashSetupVar.Mode) {
        SecureFlashSetupVar.Rollback = 1;
#if IGNORE_IMAGE_ROLLBACK == 1
        SecureFlashSetupVar.Rollback = 2; // rollback protection fully disabled
#endif
#if IGNORE_RUNTIME_UPDATE_IMAGE_REVISION_CHECK == 1
        if(SecureFlashSetupVar.Mode == 3) 
            SecureFlashSetupVar.Rollback = 1; // rollback disabled for Runtime
#endif
    }
    //
    // Set Flash Upd Setup Var
    //
    Size = sizeof(SECURE_FLASH_SETUP_VAR);
    Status = pRS->SetVariable (AMI_SECURE_FLASH_SETUP_VAR, 
            &gSecureFlashSetupVarGuid,  
            EFI_VARIABLE_BOOTSERVICE_ACCESS,
            Size,
            &SecureFlashSetupVar);

    DEBUG ((SecureMod_DEBUG_LEVEL,"SecureFlash Setup update %r\n", Status));

    HiiLibSetBrowserData( Size, &SecureFlashSetupVar, &gSecureFlashSetupVarGuid, AMI_SECURE_FLASH_SETUP_VAR);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SecureFlashModeCallback
//
// Description: 
//
// Input:       none
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SecureFlashModeCallback(EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key)
{
    CALLBACK_PARAMETERS *Callback;

    Callback = GetCallbackParameters();
    if(!Callback) {
        return EFI_SUCCESS;
    }
    DEBUG ((SecureMod_DEBUG_LEVEL,"\n====SecureFlashCallback==== Key=%d, Action=%0X\n",  Key, Callback->Action));
    if( Callback->Action != EFI_BROWSER_ACTION_CHANGING) {
//        if(Callback->Action==EFI_BROWSER_ACTION_RETRIEVE)
//            UpdateSecureFlashBrowserInfo();
        return EFI_UNSUPPORTED;
    }
    if(Key == SECURE_FLASH_MENU_REFRESH)
        UpdateSecureFlashBrowserInfo();

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
