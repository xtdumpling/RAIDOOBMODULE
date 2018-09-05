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
    Auth2Variable.c: Implement authentication services for the Time Based 
    authenticated variable service in UEFI2.2+
**/
/**
  Implement authentication services for the Time Based authenticated variables.
  
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
#include <CryptLib.h> // os_mktime

///
/// Global database array for scratch
/// 
extern UINT8 PublicKeyHashArray[HASH_SHA256_LEN];

extern AMI_DIGITAL_SIGNATURE_PROTOCOL *mDigitalSigProtocol;
extern EFI_GUID gEfiGlobalVariableGuid;

/**
  Process variable with platform key for verification.
  Called for PK or KEK Variables
  
  @param[in]   UINT8     *Pkcs7Cert - Pointer to Pkcs#7 cert.
  @param[in]   UINTN      Pkcs7Cert_len - Size of Pkcs7 cert
  @param[in]   UINT8     *pDigest - Digest of serialized data block
  @param[in]   UINTN      Digest_len - size of the digest (20 or 32 bytes)
  
  @retval EFI_SUCCESS             Variable passed validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SECURITY_VIOLATION  The variable does NOT pass the validation. 
                                  check carried out by the firmware. 

**/
EFI_STATUS
ProcessVarWithPk2 (
  IN  UINT8     *Pkcs7Cert,
  IN  UINTN      Pkcs7Cert_len,
  IN  UINT8     *pDigest,
  IN  UINTN      Digest_len
  )
{
    EFI_STATUS           Status;
    EFI_SIGNATURE_LIST   *OldPkList;
    EFI_SIGNATURE_DATA   *OldPkData;
    UINT8                *VarData;
    UINTN                 VarDataSize=0;
    UINT8                *x509_TrustCert, *TrustCert;
    UINTN                 x509_TrustCert_len, TrustCert_len;
    UINT8                 Pkcs7Operation;
    
    //
    // Get platform key from variable.
    //
    Status = AuthServiceInternalFindVariable (EFI_PLATFORM_KEY_NAME, &gEfiGlobalVariableGuid, (VOID **)&VarData, &VarDataSize);
// PK should have been set when we were in SETUP_MODE. This condition is INVALID.
    if (EFI_ERROR (Status) || VarData==NULL || !VarDataSize)
        return EFI_SECURITY_VIOLATION;

    OldPkList = (EFI_SIGNATURE_LIST *) VarData;
    OldPkData = (EFI_SIGNATURE_DATA *) ((UINT8 *) OldPkList + sizeof (EFI_SIGNATURE_LIST) + OldPkList->SignatureHeaderSize);

// Verify the PK SignatureType GUID
    if (!guidcmp ((EFI_GUID*) &(OldPkList->SignatureType), &gEfiCertRsa2048Guid))
    {
        //Process x509 cert and load Raw Key for Root cert comparison
        Pkcs7Operation = Pkcs7CertValidateGetSignerKey;
        x509_TrustCert = NULL;
        x509_TrustCert_len = 0;
    }
    else if(!guidcmp ((EFI_GUID*) &(OldPkList->SignatureType), &gEfiCertX509Guid)) 
        {
            Pkcs7Operation = Pkcs7CertValidateGetSignerCert;
            x509_TrustCert = (UINT8*)OldPkData->SignatureData;
            x509_TrustCert_len = (UINTN)OldPkList->SignatureSize-sizeof(EFI_GUID);
        } else
        //
        // Unsupported Sig Type, return EFI_SECURITY_VIOLATION.
        //
            return EFI_SECURITY_VIOLATION;

    TrustCert     = (UINT8*)OldPkData->SignatureData;
    TrustCert_len = (UINTN)OldPkList->SignatureSize-sizeof(EFI_GUID);

    Status = mDigitalSigProtocol->Pkcs7Verify (
             mDigitalSigProtocol,
             Pkcs7Cert, Pkcs7Cert_len,           // Pkcs7Cert
             x509_TrustCert, x509_TrustCert_len, // TrustCert
             &pDigest, &Digest_len,              // In/OutData
             Pkcs7Operation,
             RELEASE                             // Flags, mutex
             );
    if (!EFI_ERROR (Status)) {

        Status = ( TrustCert_len == Digest_len && 
                  !(MemCmp(TrustCert, pDigest, Digest_len))) ?
                  EFI_SUCCESS:EFI_SECURITY_VIOLATION;
    }

    return Status;
}

/**
  Process variable with Db(r) for verification.
  Called for DB or DBx variables
  
  @param[in]   IN CHAR16 *VariableName - pointer to Var Name in Unicode( Dbr or Kek)
  @param[in]   IN EFI_GUID *VendorGuid - pointer to Var GUID
  @param[in]   UINT8     *Pkcs7Cert - Pointer to Pkcs#7 cert.
  @param[in]   UINTN      Pkcs7Cert_len - Size of Pkcs7 cert
  @param[in]   UINT8     *pDigest - Digest of serialized data block
  @param[in]   UINTN      Digest_len - size of the digest (20 or 32 bytes)
  
  @retval EFI_SUCCESS             Variable passed validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SECURITY_VIOLATION  The variable does NOT pass the validation. 
                                  check carried out by the firmware. 

**/
EFI_STATUS
ProcessVarWithKekDbr2 (
  IN  CHAR16    *VariableName,
  IN  EFI_GUID  *VendorGuid,
  IN  UINT8     *Pkcs7Cert,
  IN  UINTN      Pkcs7Cert_len,
  IN  UINT8     *pDigest,
  IN  UINTN      Digest_len
  )
{
    EFI_STATUS            Status;
    EFI_SIGNATURE_LIST    *SigList;
    EFI_SIGNATURE_DATA    *SigItem;
    UINT32                SigCount;
    BOOLEAN               IsFound;
    UINT32                Index;
    UINT8                 *VarData;
    UINTN                 VarDataSize=0;

    //
    // Get KEK database from variable.
    //
    Status = AuthServiceInternalFindVariable (VariableName, VendorGuid, (VOID **)&VarData, &VarDataSize);
    if (EFI_ERROR (Status) || VarData==NULL || !VarDataSize)
        return EFI_SECURITY_VIOLATION;

    SigList   = (EFI_SIGNATURE_LIST *) VarData;

    //
    // Enumerate all Sig items in this list to verify the variable certificate data.
    // If anyone is authenticated successfully, it means the variable is correct!
    //
    IsFound   = FALSE;
    //
    // scan thru multiple Sig Lists if exist. Add 1 more loop....
    //
    while (!IsFound && (VarDataSize > 0) && (VarDataSize >= SigList->SignatureListSize)) {
        if (!guidcmp ((EFI_GUID*) &(SigList->SignatureType), &gEfiCertX509Guid))
        {
            SigCount  = (SigList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - SigList->SignatureHeaderSize) / SigList->SignatureSize;
            SigItem   = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigList + sizeof (EFI_SIGNATURE_LIST) + SigList->SignatureHeaderSize);
            for (Index = 0; Index < SigCount; Index++) {
            // find x509 cert and compare against one from Kek
                Status = mDigitalSigProtocol->Pkcs7Verify (
                    mDigitalSigProtocol,
                    Pkcs7Cert, Pkcs7Cert_len,        // Pkcs7Cert
                    SigItem->SignatureData,          // TrustCert Cert
                    SigList->SignatureSize,
                    &pDigest, &Digest_len,            // In/OutData
                    Pkcs7CertValidate,
                    RELEASE
                );
                if (!EFI_ERROR(Status)) {
                    IsFound = TRUE;
                    break;  
                }
                SigItem = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigItem + SigList->SignatureSize);
            }
        }// else
    // Auth2 Vars must be signed with x509 Certs
//              if (!guidcmp ((EFI_GUID*) &(SigList->SignatureType), &gEfiCertRsa2048Guid))
//                 return EFI_SECURITY_VIOLATION;
         VarDataSize -= SigList->SignatureListSize;
         SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
    }

    if (!IsFound)
        return EFI_SECURITY_VIOLATION;

    return Status;
}

/**
  Extract Signer certificate and verify Pkcs#7 SignedData signature.
  Called for Private Authenticated variables

  @param[in]   UINT8     *Pkcs7Cert - Pointer to Pkcs#7 cert.
  @param[in]   UINTN      Pkcs7Cert_len - Size of Pkcs7 cert
  @param[in]   UINT8     *pDigest - Digest of serialized data block
  @param[in]   UINTN      Digest_len - size of the digest (20 or 32 bytes)
  @param[in]   UINT8      Operation - type of Pkcs operation:Pkcs7CertValidateGetSignerCert...
  
  @retval EFI_SUCCESS             Variable passed validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SECURITY_VIOLATION  The variable does NOT pass the validation. 
                                  check carried out by the firmware. 

**/
EFI_STATUS
ValidateSelfSigned (
    IN UINT8     *Pkcs7Cert,
    IN UINTN      Pkcs7Cert_len,
    IN OUT UINT8 **pDigest,
    IN OUT UINTN  *Digest_len,
    IN UINT8       Operation
  )
{
    EFI_STATUS   Status;
    UINT8       *x509_SignCert     = NULL;
    UINTN        x509_SignCert_len = 0;

#if SELF_SIGNED_CERTIFICATE_IN_PRIVATE_VARIABLES == 1
    //
    //  Extract x509 Signing Cert and validate self-signed data
    //
    x509_SignCert=(UINT8*)&x509_SignCert_len; // non-zero ptr
    Status = mDigitalSigProtocol->Pkcs7Verify (
             mDigitalSigProtocol,
             Pkcs7Cert, Pkcs7Cert_len,
             NULL, 0,                     // RootCert
             &x509_SignCert, &x509_SignCert_len,
             Pkcs7GetSignerCert, 
             KEEP
    );
    if (!EFI_ERROR (Status)) 
#endif
    //
    // Authenticate using internal Signer Cert, Extract Cert or Key Hash
    //
        Status = mDigitalSigProtocol->Pkcs7Verify (
                 mDigitalSigProtocol,
                 Pkcs7Cert, Pkcs7Cert_len,          // Pkcs7Cert
                 x509_SignCert, x509_SignCert_len,  // RootCert     
                 pDigest, Digest_len,               // Input->Digest, Output->DER Ptr to Signing Cert
                 Operation, 
                 RELEASE 
         );

    return Status;
}

/**
  Generates the Hash (SHA256) out of serialized data block 
  Hash(VariableName, VendorGuid, Attributes, TimeStamp, Data)

  @param[in]   CHAR16 *VariableName - pointer to Var Name in Unicode
  @param[in]   EFI_GUID *VendorGuid - pointer to Var GUID
  @param[in]   UINT32 Attributes - Attributes of the Var
  @param[in]   VOID *Data - pointer to the Var data
  @param[in]   UINTN DataSize - size of Var data
  
  @param[out]  UINT8  Digest - Hash of the serialized data block
  @param[out]  UINTN  Digest_len - size of data block
  
  @retval EFI_SUCCESS               The operation is success.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_UNSUPPORTED           Unsupported Digest type.


**/
EFI_STATUS ConstructDataParameter (
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32    Attributes,
    IN VOID     *Data,
    IN UINTN     DataSize, 
    OUT UINT8   *pDigest,
    OUT UINTN   *Digest_len,
    IN  UINT8    Mutex    
)
{
    EFI_STATUS Status;

    EFI_GUID *HashAlgorithm;
    UINT8 *Addr[5];
    UINTN   Len[5];
    EFI_VARIABLE_AUTHENTICATION_2 *CertData;
    UINTN   CertHdrSize, Pkcs7Cert_len;
    UINT8   *Pkcs7Cert;
    UINT8   HashType=SHA1;
    UINT8   *pHash=&HashType;

#ifdef EFI_DEBUG
    UINTN     j, i;
#endif

    CertData  = (EFI_VARIABLE_AUTHENTICATION_2 *)Data;
    CertHdrSize = AMI_AUTHINFO_2_SIZE(Data);

    Pkcs7Cert  = (UINT8*)&(CertData->AuthInfo.CertData);
    Pkcs7Cert_len = CertHdrSize - ((UINTN)Pkcs7Cert - (UINTN)Data);
    Status = mDigitalSigProtocol->Pkcs7Verify ( 
              mDigitalSigProtocol,
              Pkcs7Cert,
              Pkcs7Cert_len,
              NULL,
              0,
              &pHash,        // returns DER Ptr to Sign Cert
              &Len[0],
              Pkcs7GetDigestAlgorithm,
              Mutex
              );
    if (EFI_ERROR(Status)) 
        return Status;

    switch(*pHash) {
        case SHA1:
            *Digest_len = HASH_SHA1_LEN;
            HashAlgorithm = &gEfiHashAlgorithmSha1Guid;
            break;
        case SHA256:
            *Digest_len = HASH_SHA256_LEN;
            HashAlgorithm = &gEfiHashAlgorithmSha256Guid;
            break;
        default:
            return EFI_UNSUPPORTED;
            break;
    }
    //
    // Hash data payload with SHA.
    //
    Addr[0] = (UINT8*)VariableName;
    Len[0] = StrSize16(VariableName);
    Addr[1] = (UINT8*)VendorGuid;
    Len[1] = sizeof(EFI_GUID);
    Addr[2] = (UINT8*)&Attributes;
    Len[2] = sizeof(UINT32);
    Addr[3] = (UINT8*)&(CertData->TimeStamp);
    Len[3] = sizeof(EFI_TIME);
    Addr[4] = (UINT8*) Data + (CertHdrSize) ;
    Len[4] = DataSize - (CertHdrSize);
// zero out unused Time fields:
/*    CertData->TimeStamp.Pad1 = 0;
    CertData->TimeStamp.Pad2 = 0;
    CertData->TimeStamp.Nanosecond = 0;
    CertData->TimeStamp.TimeZone = 0;
    CertData->TimeStamp.Daylight = 0;*/
#ifdef EFI_DEBUG
AVAR_TRACE((TRACE_ALWAYS,"Hash Serialization"));
    for (j=0; j<5; j++) {
        AVAR_TRACE((TRACE_ALWAYS,"\nArg%d, Len=0x%x\n0000: ", j, Len[j]));
        for (i=0; i<Len[j]; i++) 
        {
            AVAR_TRACE((TRACE_ALWAYS,"%02X ", *(UINT8*)(Addr[j]+i) ));
            if(Len[j]>16 && i>=15){
                AVAR_TRACE((TRACE_ALWAYS,"\n....\n%04X: ",Len[j]-16));
                for (i=Len[j]-16; i<Len[j]; i++) 
                    AVAR_TRACE((TRACE_ALWAYS,"%02X ", *(UINT8*)(Addr[j]+i) ));
            }
        }
    }
#endif
    Status = mDigitalSigProtocol->Hash(mDigitalSigProtocol, HashAlgorithm, 5, Addr, Len, pDigest);
#ifdef EFI_DEBUG
AVAR_TRACE((TRACE_ALWAYS,"\nOutput Digest\n"));
    for (i=0; i<16; i++) 
        AVAR_TRACE((TRACE_ALWAYS,"%02X ", (pDigest[i]) ));
AVAR_TRACE((TRACE_ALWAYS,"...\n"));
#endif
    return Status;
}

/**
  Verify data payload with AuthInfo in EFI_VARIABLE_AUTHENTICATION_2 type.
  Follow the steps in UEFI2.3.1. This function is called every time variable with 
  EFI_VARIABLE_AUTHENTICATED_TIME_BASED_ACCESS attribute is 
  created, updated or deleted. This function does all necessary 
  authentication checks and based on the results returns Status.
  Also it updates the ExtFlags.KeyHash with the hash the Signer's
  certificate from Variable's AuthInfo Hdr

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
EFI_STATUS VerifyVariable2 (
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
    EFI_VARIABLE_AUTHENTICATION_2 *CertData;
    UINTN       CertHdrSize, Pkcs7Cert_len, i;
    UINT8       *Pkcs7Cert;
    os_time_t   TimeStamp, NonZeroTime;
    UINT8       *PubKeyHash;
    UINTN       PubKeyHashLen;
    UINT8       AuthVarType;
    UINT64      OldMC;

/*
    common algo for all Auth2 Vars

1. Verify that the correct AuthInfo.CertType (EFI_CERT_TYPE_PKCS7_GUID) has been
used and that the AuthInfo.CertData value parses correctly as a PKCS #7 SignedData
value

2. Verify the signature by:
-extracting the EFI_VARIABLE_AUTHENTICATION_2 descriptor from the Data buffer;
-using the descriptor contents and other parameters to
- construct the input to the digest algorithm;
-computing the digest; 
    digest = hash (VariableName, VendorGuid, Attributes, TimeStamp, DataNew_variable_content).
 and
-comparing the digest with the result of applying the signer's public key to the signature
!!!!Signer must have at least one Cert!!!!

3. Verify that the signer's certificate chains to a certificate in the Key Exchange Key database (or
the Platform Key)
!!!!
!!!! branch off to handle special Vars for PK or KEK-> Use PK Cert to verify Signer's key
!!!! all other vars including DB and DBx should be looking for Cert chains in KEK db!!!!
!!!! 

4. Unless the EFI_VARIABLE_APPEND_WRITE attribute is set, verify that the TimeStamp value
is later than the current timestamp value associated with the variable.

5. For variables with GUID EFI_IMAGE_SECURITY_DATABASE_GUID (i.e. where the data buffer is formatted 
   as EFI_SIGNATURE_LIST), the driver shall not perform an append of EFI_SIGNATURE_DATA values that 
   are already part of the existing variable value
*/
// must have Auth attribute to go deeper
    if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)==0)
        return EFI_INVALID_PARAMETER;

    RealData = *Data;

    CertData  = (EFI_VARIABLE_AUTHENTICATION_2 *) *Data;
    CertHdrSize = AMI_AUTHINFO_2_SIZE(CertData);//(CertData->AuthInfo.Hdr.Hdr.dwLength + sizeof(EFI_TIME));
    if(*DataSize < CertHdrSize)
        return EFI_SECURITY_VIOLATION;

    // CertBlock->CerData is a begining of Pkcs7 Cert
    Pkcs7Cert  = (UINT8*)&(CertData->AuthInfo.CertData);
    Pkcs7Cert_len = CertHdrSize - ((UINTN)Pkcs7Cert - (UINTN)*Data);

    //
    // wCertificateType should be WIN_CERT_TYPE_EFI_GUID.
    // Cert type should be EFI_CERT_TYPE_PKCS7_GUID.
    //
    if ((CertData->AuthInfo.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) ||  
        guidcmp ((EFI_GUID*) &(CertData->AuthInfo.CertType), &gEfiCertPkcs7Guid)
        ) {
    //
    // Invalid AuthInfo type, return EFI_SECURITY_VIOLATION.
    //
        return EFI_SECURITY_VIOLATION;
    }

    //
    // Time check fail, suspicious replay attack, return EFI_SECURITY_VIOLATION.
    //
    TimeStamp = 0;
    if(os_mktime( CertData->TimeStamp.Year, CertData->TimeStamp.Month, 
                CertData->TimeStamp.Day, CertData->TimeStamp.Hour, 
                CertData->TimeStamp.Minute, CertData->TimeStamp.Second, &TimeStamp ))
    {
        NonZeroTime = 0;
        for(i=0;i < sizeof(EFI_TIME); i++)
            NonZeroTime+=(os_time_t)((UINT8*)CertData)[i];

        if(NonZeroTime || ((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0)) {
              AVAR_TRACE((TRACE_ALWAYS,"Invalid Time Stamp\n"));
              return EFI_SECURITY_VIOLATION;
        }
    }        
    AVAR_TRACE((TRACE_ALWAYS,"Update Time Stamp\nOrg=%lx\nNew=%lx\n", ExtFlags->Mc, TimeStamp));
    if( (Attributes & EFI_VARIABLE_APPEND_WRITE) == EFI_VARIABLE_APPEND_WRITE) {
    // AppendWrite: Only update Timestamp if New one is greater then the current one
        if(OldData && (os_time_t)ExtFlags->Mc > TimeStamp)
            TimeStamp = (os_time_t)ExtFlags->Mc;
    } else {
        //Unless the EFI_VARIABLE_APPEND_WRITE attribute is set, verify that the TimeStamp value
        //is later than the current timestamp value associated with the variable.
        // and reserved TimeStamp fields must be set to 0
        if(  CertData->TimeStamp.Pad1 ||
             CertData->TimeStamp.Pad2 ||
             CertData->TimeStamp.Nanosecond ||
             CertData->TimeStamp.TimeZone ||
             CertData->TimeStamp.Daylight ||
            (OldData && (os_time_t)ExtFlags->Mc >= TimeStamp)
        ) {
            AVAR_TRACE((TRACE_ALWAYS,"Failed\n"));
            return EFI_SECURITY_VIOLATION;
        }
    } 
    OldMC = ExtFlags->Mc;
    ExtFlags->Mc = TimeStamp;
    AVAR_TRACE((TRACE_ALWAYS,"Upd=%x\n", ExtFlags->Mc));
    //
    // Process PK, KEK,... separately
    //
    AuthVarType = IsPrivateVarType;

    if (IsDbVar(VariableName, VendorGuid)) 
        AuthVarType = IsDbVarType;
    else
        if (IsPkVar(VariableName, VendorGuid))
            AuthVarType = IsPkVarType;
        else
            if (IsKekVar(VariableName, VendorGuid))
                AuthVarType = IsKekVarType;
            else
                if (IsDbrVar(VariableName, VendorGuid))
                    AuthVarType = IsDbrVarType;

    if(AuthVarType != IsPrivateVarType) {

        //
        // PK, KEK and db/dbx should set EFI_VARIABLE_NON_VOLATILE attribute
        // authenticated variable.
        //
        if ((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
            return EFI_INVALID_PARAMETER;
        }
        if (!GetPkPresent() || InCustomSecureBootMode() ) {
            Status = EFI_SUCCESS;
            goto Exit_SetupMode;
        }
    }

    PubKeyHash = &PublicKeyHashArray[0];
    Status = ConstructDataParameter (
             VariableName, VendorGuid, Attributes, 
             *Data, *DataSize, 
             PublicKeyHashArray, &PubKeyHashLen,
             LOCK // first time Pkcs7 cert process.
    );
    if (EFI_ERROR(Status)) 
        return EFI_SECURITY_VIOLATION;

//
// Process UEFI Private TimeBased Authenticated Variables
//
    switch (AuthVarType) {

    case IsPrivateVarType: 
        //
        // Validate self signed certificate and extract a SHA256 hash of x509 Signing Cert
        //
        Status = ValidateSelfSigned (
                 Pkcs7Cert, Pkcs7Cert_len,    // Pkcs7Cert
                 &PubKeyHash, &PubKeyHashLen, // Input->Digest, Output->DER Ptr to Signing Cert
                 Pkcs7CertValidateGetSignerKeyHash
         );
        if (!EFI_ERROR(Status)) {
             // Updating Existing Variable
                if (OldData && MemCmp(&ExtFlags->KeyHash[0], PubKeyHash, PubKeyHashLen)){
                    AVAR_TRACE((TRACE_ALWAYS,"Private Var Key Compare FAILED!\n"));
                    return EFI_SECURITY_VIOLATION;
                }
            // Setting Hash for self signed variables
            MemCpy(&ExtFlags->KeyHash[0], PubKeyHash, PubKeyHashLen);
        }
        break;
// 
// Process OsRecoveryOrder & OsRecoveryXXXX with one of dbr,KEK or if not found - with PK keys
//        
    case IsDbrVarType:
        Status = ProcessVarWithKekDbr2 (EFI_IMAGE_SECURITY_DATABASE3, &gEfiImageSecurityDatabaseGuid, Pkcs7Cert, Pkcs7Cert_len, PubKeyHash, PubKeyHashLen);
        AVAR_TRACE((TRACE_ALWAYS,"dbr for '%S' check %r\n", VariableName, Status));
        if (!EFI_ERROR(Status))
            break;
//
// Process db(x,t,r) variables
//
    case IsDbVarType:
        Status = ProcessVarWithKekDbr2 (EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid, Pkcs7Cert, Pkcs7Cert_len, PubKeyHash, PubKeyHashLen);
        // verify process db(t,x) with one of KEK keys or if not found within KEK - with PK
        AVAR_TRACE((TRACE_ALWAYS,"KEK for '%S' check %r\n", VariableName, Status));
        if (!EFI_ERROR(Status))
            break;
//
// Process PK, KEK separately
//
    case IsKekVarType:
    case IsPkVarType:
         Status = ProcessVarWithPk2 (Pkcs7Cert, Pkcs7Cert_len, PubKeyHash, PubKeyHashLen);
         AVAR_TRACE((TRACE_ALWAYS,"PK for '%S' check %r\n", VariableName, Status));
         break;
    }
    if (EFI_ERROR(Status))  
        return EFI_SECURITY_VIOLATION;

Exit_SetupMode:

    *DataSize   =  *DataSize - CertHdrSize;
    *Data       = (UINT8*)RealData + CertHdrSize;

    if( *DataSize != 0 && AuthVarType != IsPrivateVarType && AuthVarType != IsDbrVarType ) 
    {
        if ( AuthVarType == IsDbVarType && 
            ((Attributes & EFI_VARIABLE_APPEND_WRITE)==EFI_VARIABLE_APPEND_WRITE)
        ) {
            // Find out if New Var is a Signature Db and the Sig already present in current Sig DB Variable
            // bail out SetVar if present - nothing to change
              Status = IsAppendToSignatureDb(*Data, DataSize, OldData, OldDataSize);
              if (EFI_ERROR(Status)) {
                  // Only update the Timestamp if new Sig found in OldSig list
                  if(OldMC != ExtFlags->Mc)
                  {
                     *DataSize = 0;
                      Status = EFI_SUCCESS;
                      
                  }
                  // else  Variable not changed, abort the SetVar
              } 
        } else {
            // Validate Signature List integrity 
            Status = ValidateSignatureList (*Data, *DataSize);
        }
    }
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
