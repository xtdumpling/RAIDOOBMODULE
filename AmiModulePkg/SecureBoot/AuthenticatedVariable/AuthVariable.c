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

/** @file
    AuthVariable.c: Implement authentication services for the Monotonic Counter based 
    authenticated variable service in UEFI2.2+
**/
/**
  Implement generic authentication services for the authenticated variables.
  
  Caution: This module requires additional review when modified.
  It takes an external input - variable data. It may be input in SMM mode.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.
  Variable attribute should also be checked to avoid authentication bypass.
     The whole SMM authentication variable design relies on the integrity of flash part and SMM.
  which is assumed to be protected by platform.  All variable code and metadata in flash/SMM Memory
  may not be modified without authorization. If platform fails to protect these resources,
  the authentication service provided in this driver will be broken, and the behavior is undefined.
**/  
#include "AuthVariable.h"

//
// Global defines and variables
// 
extern AMI_DIGITAL_SIGNATURE_PROTOCOL *mDigitalSigProtocol;
UINT8  PublicKeyHashArray[HASH_SHA256_LEN];


/**
  Verify data payload with AuthInfo in EFI_CERT_TYPE_RSA2048_SHA256 type.
  Follow the steps in UEFI2.3.1+. This function does signature 
  Authentication and based on the results returns Status.

  @param[in]   VOID *Data - pointer to the Var data
  @param[in]   UINTN DataSize - size of Var data
  @param[in]   UINT8 *PubKey - PublicKey used for Sig verification.
  
  @retval EFI_SUCCESS               The operation is success.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_UNSUPPORTED           Unsupported Digest type.


**/
EFI_STATUS VerifyDataPayload (
    IN VOID *Data,
    IN UINTN DataSize, 
    IN UINT8 *PubKey
)
{
    EFI_STATUS Status;
    EFI_VARIABLE_AUTHENTICATION     *CertData;
    EFI_CERT_BLOCK_RSA_2048_SHA256  *CertBlock;
    UINT8 Digest[HASH_SHA256_LEN];
    UINT8 *Addr[2];
    UINTN   Len[2];
    UINTN   CertHdrSize;

    CRYPT_HANDLE   PublicKey;
    CRYPT_HANDLE   HashToVerify;

    CertData  = (EFI_VARIABLE_AUTHENTICATION *)Data;
//    CertHdrSize = AMI_AUTHINFO_SIZE(Data);
// SCT 2.3.1. TW UEFI Plugfest BUG with listing of a Cert Size in Cert->AuthInfo.Hdr.dwLength
    CertHdrSize = 0x230;

    CertBlock = (EFI_CERT_BLOCK_RSA_2048_SHA256*)&(CertData->AuthInfo.CertData);

    //
    // Hash data payload with SHA256.
    //
    Addr[0] = (UINT8*) Data + (CertHdrSize) ;
    Len[0] = DataSize - (CertHdrSize);
    Addr[1] = (UINT8*)&(CertData->MonotonicCount);
    Len[1] = sizeof(UINT64);
    Status = mDigitalSigProtocol->Hash(mDigitalSigProtocol, 
                            &gEfiHashAlgorithmSha256Guid,
                            2, Addr, Len, (UINT8*)&Digest[0]);
    if (EFI_ERROR(Status)) 
        return Status;

    PublicKey.AlgGuid = gEfiCertRsa2048Guid;
    PublicKey.BlobSize = EFI_CERT_TYPE_RSA2048_SIZE;
    PublicKey.Blob = PubKey;//(UINT8*)&(CertData->AuthInfo.CertData.PublicKey);

    HashToVerify.AlgGuid = gEfiHashAlgorithmSha256Guid;
    HashToVerify.BlobSize = SHA256_DIGEST_SIZE;
    HashToVerify.Blob = &Digest[0];

    Status = mDigitalSigProtocol->Pkcs1Verify(
            mDigitalSigProtocol,
            &PublicKey,
            &HashToVerify,
            (UINT8*)&(CertBlock->Signature), 
            EFI_CERT_TYPE_RSA2048_SHA256_SIZE, EFI_CRYPT_RSASSA_PKCS1V15/*EFI_CRYPT_RSASSA_PSS*/);

    return Status;
}

/**
  Verify data payload with AuthInfo in EFI_CERT_TYPE_RSA2048_SHA256 type.
  Follow the steps in UEFI2.3.1+
  This function is called every time variable with 
  EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS attribute is 
  created, updated or deleted. This function does all necessary 
  Authentication checks and based on the results returns Status.
  Also it returns the Mc Hash of PublicKey from Variable's AuthInfo Hdr

  @param[in]   IN CHAR16 *VariableName - pointer to Var Name in Unicode
  @param[in]   IN EFI_GUID *VendorGuid - pointer to Var GUID
  @param[in]   UINT32 Attributes - Attributes of the Var
  @param[in]   VOID **Data - pointer to the Var data
  @param[in]   UINTN *DataSize - size of Var data
  @param[in]   VOID  *OldData - pointer to Existing in NVRAM data block 
  @param[in]   UINTN  OldDataSize - size of data block
  @param[in]   UINT64 ExtFlags.MonotonicCount - value of MC or TIME stamp 
  @param[in]   UINT8  ExtFlags.KeyHash[32] - pointer to memory, allocated by caller, 
                      where Hash of Public Key will be returned.
  
  @retval EFI_SUCCESS               The operation is success.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_SECURITY_VIOLATION    Variable fail Authentication

**/
EFI_STATUS VerifyVariable1 (
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32    Attributes,
    IN VOID    **Data,
    IN UINTN    *DataSize, 
    IN VOID     *OldData,
    IN UINTN     OldDataSize,
    IN OUT EXT_SEC_FLAGS *ExtFlags
){
    EFI_STATUS Status;

    VOID        *RealData;
    EFI_VARIABLE_AUTHENTICATION *CertData;
    EFI_CERT_BLOCK_RSA_2048_SHA256   *CertBlock;
    UINT8       *PubKey, *PubKeyHash;
    UINTN       CertHdrSize;
    UINTN       Size;

    RealData = *Data;

// must have Auth attribute to go deeper
    if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == 0)
        return EFI_INVALID_PARAMETER;

    if(*DataSize < sizeof(EFI_VARIABLE_AUTHENTICATION))
        return EFI_SECURITY_VIOLATION;

// specific for EFI_VARIABLE_AUTHENTICATION mode variables
//--->>>
    CertData  = (EFI_VARIABLE_AUTHENTICATION *) *Data;
    CertBlock = (EFI_CERT_BLOCK_RSA_2048_SHA256*)&(CertData->AuthInfo.CertData);
// SCT 2.3.1 TW Plugfest BUG with listing of a Cert Size in Cert->AuthInfo.Hdr.dwLength
// 0x22b vs 0x230
//    CertHdrSize = AMI_AUTHINFO_SIZE(CertData);//(CertData->AuthInfo.Hdr.Hdr.dwLength + sizeof(CertData->MonotonicCount));
//AVAR_TRACE((TRACE_ALWAYS,"VerifyVariable CertHdr Size 0x%x (expected(0x230)\n",AMI_AMI_AUTHINFO_SIZE(CertData)));

    CertHdrSize = 0x230;

    //
    // wCertificateType should be WIN_CERT_TYPE_EFI_GUID.
    // Cert type should be EFI_CERT_TYPE_RSA2048_SHA256.
    //
    if ((CertData->AuthInfo.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) ||  
        guidcmp ((EFI_GUID*) &(CertData->AuthInfo.CertType), &gEfiCertTypeRsa2048Sha256Guid)
    ) {
        //
        // Invalid AuthInfo type, return EFI_SECURITY_VIOLATION.
        //
//AVAR_TRACE((TRACE_ALWAYS,"VerifyVariable AuthHdr GUID test Fails\nWinCert_Type\nExpected  %x\nReceived %x\nGUID\nExpected  %g\nReceived %g\n",
//                               WIN_CERT_TYPE_EFI_GUID, CertData->AuthInfo.Hdr.wCertificateType, &gEfiCertTypeRsa2048Sha256Guid, &(CertData->AuthInfo.CertType)));
// SCT 2.3.1 TW Plugfest uses wrong GUID
            return EFI_SECURITY_VIOLATION;
    } 

    if(*DataSize < CertHdrSize)
    {
//AVAR_TRACE((TRACE_ALWAYS,"VerifyVariable DataSize test fails: DataSize(%x) < AuthHdrSize (%x)\n", *DataSize, CertHdrSize));
        return EFI_SECURITY_VIOLATION;
    }
    //
    // UEFI 2.5: PK, KEK, Sig db variables are not supported with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS type
    // similar blocking logic already in place in NvramDxe driver
     if (IsPkVar(VariableName, VendorGuid) ||
         IsKekVar(VariableName, VendorGuid) ||
         IsDbVar(VariableName, VendorGuid) ||
         IsDbrVar(VariableName, VendorGuid) 
        )
         return EFI_INVALID_PARAMETER;

    //
    // Monotonic count check fail, suspicious replay attack, return EFI_SECURITY_VIOLATION.
    //
//    AVAR_TRACE((TRACE_ALWAYS,"Check MC:\nOld=%x\nNew=%x\n",ExtFlags->Mc, CertData->MonotonicCount));
    //Unless the EFI_VARIABLE_APPEND_WRITE attribute is set, 
    // verify that the Counter value is later than the 
    // current one associated with the variable.
    if (((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) &&
        (OldData && ExtFlags->Mc >= CertData->MonotonicCount)
    ) {
//        AVAR_TRACE((TRACE_ALWAYS,"Failed\n"));
        return EFI_SECURITY_VIOLATION;
    }
    // AppendWrite: Only update Counter if New one is greater then current
    if( (Attributes & EFI_VARIABLE_APPEND_WRITE) && 
        (OldData && ExtFlags->Mc > CertData->MonotonicCount)
       );
    else
        ExtFlags->Mc = CertData->MonotonicCount;
//AVAR_TRACE((TRACE_ALWAYS,"Upd=%x\n", ExtFlags->Mc));

    PubKey = &(CertBlock->PublicKey[0]);
    PubKeyHash = &PublicKeyHashArray[0];
    // Verify SelfSigned variable is signed with a valid Key
    Status = VerifyDataPayload (*Data, *DataSize, PubKey);
    if (EFI_ERROR(Status))
        return EFI_SECURITY_VIOLATION;

    Size = RSA2048_PUB_KEY_LEN;
    Status = mDigitalSigProtocol->Hash(mDigitalSigProtocol, 
                &gEfiHashAlgorithmSha256Guid,
                1, &PubKey, (const UINTN*)&Size, PubKeyHash); 

    if (OldData && MemCmp(&ExtFlags->KeyHash[0], PubKeyHash, HASH_SHA256_LEN)){
//AVAR_TRACE((TRACE_ALWAYS,"Self Signed MC Var Key Compare FAILED!\n"));
        return EFI_SECURITY_VIOLATION;
    }
    // Setting key Hash for self signed variables
    MemCpy(&ExtFlags->KeyHash[0], PubKeyHash, HASH_SHA256_LEN);
    
    *DataSize   =  *DataSize - CertHdrSize;
    *Data       = (UINT8*)RealData + CertHdrSize;

    return Status;
}

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
