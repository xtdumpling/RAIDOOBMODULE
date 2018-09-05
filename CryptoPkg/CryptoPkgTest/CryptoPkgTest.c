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
//<AMI_FHDR_START>
//
// Name:	CryptoPkgTest.C
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Token.h>
#include <Library/BaseCryptLib.h>
#include <openssl/crypto.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <CryptoPkgTest.h>

AMI_CRYPTOPKG_TEST_PROTOCOL  AmiCryptoPkgTestProtocol = {
    AMI_CRYPTOPKG_TEST_PROTOCOL_REVISION_02,
    GetOpenSslVersionString,
    GetOpenSslVersionNumber,
    ValidateCryptDigestFunc,
    ValidateCryptHmacFunc,
    ValidateCryptBlockCipherFunc,
    ValidateCryptRsaFunc,
    ValidateCryptRsa2Func,
    ValidateCryptPkcs7Func,
    ValidateAuthenticodeFunc,
    ValidateTscounterSignatureFunc,
    ValidateCryptDhFunc,
    ValidateCryptPrngFunc,
    ValidateCryptPkcs5Func   
};

EFI_STATUS ValidateCryptDigestFunc(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  RandomSeed (NULL, 0);
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptDigestFunc"));

  Status = ValidateCryptDigest();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateCryptHmacFunc(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptHmacFunc"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateCryptHmac();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateCryptBlockCipherFunc(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptBlockCipherFunc"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateCryptBlockCipher();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateCryptRsaFunc(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptRsaFunc"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateCryptRsa();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateCryptRsa2Func(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptRsa2Func"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateCryptRsa2();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateCryptPkcs5Func(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptPkcs5Func"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateCryptPkcs5Pbkdf2();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}


EFI_STATUS ValidateCryptPkcs7Func(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptPkcs7Func"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateCryptPkcs7();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateAuthenticodeFunc(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateAuthenticodeFunc"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateAuthenticode();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateTscounterSignatureFunc(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateTscounterSignatureFunc"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateTSCounterSignature();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateCryptDhFunc(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptDhFunc"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateCryptDh();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS ValidateCryptPrngFunc(VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  DEBUG ((DEBUG_ERROR, "ValidateCryptPrngFunc"));
  
  RandomSeed (NULL, 0);
  
  Status = ValidateCryptPrng();
  
  DEBUG ((DEBUG_ERROR, "  Status = %r\n",Status));
  
  return Status;	
}

EFI_STATUS GetOpenSslVersionString(const char **Buffer)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  *Buffer = SSLeay_version(SSLEAY_VERSION);
  
  return Status;
}

EFI_STATUS GetOpenSslVersionNumber(unsigned long *Buffer)
{
  EFI_STATUS    Status = EFI_SUCCESS;	
  
  *Buffer = SSLeay();
  
  return Status;
}	


EFI_STATUS
EFIAPI
CryptoPkgTestEntryPoint (
  IN EFI_HANDLE              ImageHandle,
  IN EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE	Handle;

  InitAmiLib(ImageHandle, SystemTable);

  Handle = NULL;
  Status = pBS->InstallProtocolInterface (
                 &Handle,
                 &gAmiCryptoPkgTestProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &AmiCryptoPkgTestProtocol
                 );
    
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
