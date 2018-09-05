//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017 American Megatrends, Inc.          **
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
  AmiCertificate.h
  AMI FW Certificate defines and data structures
**/

#ifndef __AMI_CERTIFICATE_H__
#define __AMI_CERTIFICATE_H__

#include <Efi.h>
// All are EDKII defined headers
//#include "WinCertificate.h"
#include <Guid/WinCertificate.h>
//#include "ImageAuthentication.h"
#include <Guid/ImageAuthentication.h>
#ifndef ROM_AREA
#include <RomLayout.h>
#endif

#pragma pack(1)

#ifndef offsetof
#define offsetof(st, m) \
     ((UINTN) ( (char *)&((st *)0)->m - (char *)0 ))
#endif

//**********************************************************************
// AMI FW Certificate defines
//**********************************************************************

///
/// Aptio FW Capsule with extended header fields.
///4A3CA68B-7723-48FB-803D-578CC1FEC44D
///
#define APTIO_FW_CAPSULE_GUID \
    { 0x4A3CA68B, 0x7723, 0x48FB, 0x80, 0x3d, 0x57, 0x8c, 0xc1, 0xfe, 0xc4, 0x4d }

///
/// APTIO Platform Fw Root Signing key identifier
/// {4E0F9BD4-E338-4B26-843E-BD3AD9B2837B}
///
#define PR_KEY_GUID \
    { 0x4E0F9BD4, 0xE338, 0x4B26, 0x84, 0x3E, 0xBD, 0x3A, 0xD9, 0xB2, 0x83, 0x7B }

///
/// AMI APTIO Signature Owner id.
/// An identifier which identifies the agent which added the signature to the list.
///
#define AMI_APTIO_SIG_OWNER_GUID \
    { 0x26DC4851, 0x195F, 0x4AE1, 0x9A, 0x19, 0xFB, 0xF8, 0x83, 0xBB, 0xB3, 0x5E }

///
///Platform FwKey GUIDed HOB
///Header HobGUID = PR_KEY_GUID
///
typedef struct {
    EFI_HOB_GUID_TYPE   Header;
    EFI_GUID            KeyGuid;
    EFI_PHYSICAL_ADDRESS KeyAddress;
    UINT32              KeySize;
} FW_KEY_HOB;

///
/// Modified version of Win Uefi Cert. CertData is not defined in here
///
typedef struct {
  WIN_CERTIFICATE   Hdr;
  EFI_GUID          CertType;
//  UINT8            CertData[1];
//    EFI_CERT_BLOCK_RSA_2048_SHA256  CertData;
} WIN_CERTIFICATE_UEFI_GUID_1;

///
/// Certificate which encapsulates the UEFI RSA2048_SHA256 digital signature.
///
/// The WIN_CERTIFICATE_UEFI_GUID structure is derived from
/// WIN_CERTIFICATE and encapsulate the information needed to  
/// implement the digital signature algorithm
/// Hdr.Hdr.wRevision = 0x0200
/// Hdr.Hdr.wCertificateType = 0x0EF1; WIN_CERT_TYPE_EFI_GUID
/// Hdr.CertType = EFI_CERT_TYPE_RSA2048_SHA256_GUID;
///
/// Fields: Name     Type        Description
///----------------------------------------------------------------------------
/// Hdr      WIN_CERTIFICATE_UEFI_GUID   encapsulate the information needed to implement the digital signature algorithm 
/// CertData  EFI_CERT_BLOCK_RSA_2048_SHA256    Signature and Public key
///----------------------------------------------------------------------------
///
typedef struct {
    WIN_CERTIFICATE_UEFI_GUID_1      Hdr;        // 24
    EFI_CERT_BLOCK_RSA_2048_SHA256   CertData;   // 16+256+256
} AMI_CERTIFICATE_RSA2048_SHA256;
//(sizeof(AMI_CERTIFICATE_RSA2048_SHA256) == 552);

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        FW_CERTIFICATE
//
// Description:    Aptio FW Capsule certificate block
//
// Fields: Name     Type        Description
//----------------------------------------------------------------------------
// SignCert      AMI_CERTIFICATE_RSA2048_SHA256    Certificate contains a signature of a Capsule's payload and Signer's public key
// RootCert      EFI_CERT_BLOCK_RSA_2048_SHA256    Certificate contains a signature of a Signer's public key signed by the 
//                                                   Root Key, whose public portion is also included into this certificate. 
//    For Aptio FW update process to succeed, either of these keys must match the instance of a Platform FW Key in the Flash.
//
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct {
    AMI_CERTIFICATE_RSA2048_SHA256   SignCert;   // 24+16+256+256
    EFI_CERT_BLOCK_RSA_2048_SHA256   RootCert;   // 16+256+256
} FW_CERTIFICATE;
//(sizeof(FW_CERTIFICATE) == 1112);

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        APTIO_FW_CAPSULE_HEADER
//
// Description: Enhanced APTIO FW CAPSULE with certificate and the map of the modules in the 
//              capsule's volume to be included into the signature calculation
// Fields: 
//  Name            Type                Description
//----------------------------------------------------------------------------
//  CapHdr           EFI_CAPSULE_HEADER  UEFI 2.0 Capsule Hdr. Use APTIO_FW_CAPSULE_2_GUID
//  RomImageOffset   UINT16              offset within the Fw Capsule header to the FW image
//  RomLayoutOffset  UINT16              offset to the FW descriptor table used for signature calculation
//  FWCert           FW_CERTIFICATE      Certificate block. Format may change based on WIN_CERTIFICATE header Type.
//                                       Hdr.CertType = EFI_CERT_TYPE_RSA2048_SHA256_GUID;
//                                       Hdr.CertType = EFI_CERT_TYPE_PKCS7_GUID; 
//  ....
//  RomAreaMap       ROM_AREA
//  ....
//  Payload    
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct {
    EFI_CAPSULE_HEADER    CapHdr;
    UINT16                RomImageOffset;
    UINT16                RomLayoutOffset;
    FW_CERTIFICATE        FWCert;
    ROM_AREA              RomAreaMap[1];
} APTIO_FW_CAPSULE_HEADER;

// CapHdr.Flags Extended FwCapsule GUID flags
// RSA Signature padding scheme:
// 0-PKCS1v5
// 1-PSS
#define CAPSULE_FLAGS_RSA_PSS_PADDING_SCHEME    0x00000001 
// Blocks used for signing by Signing Certificate:
// 0-RomMap + Payload
// 1-FwCapHdr + RomMap + Payload
#define CAPSULE_FLAGS_CAPHDR_IN_SIGNCERT      0x00000002 
// Blocks used for signing by Root Certificate:
// 0-FwCert.SignCert structure
// 1-FwCert.SignCert.PublicKey field
#define CAPSULE_FLAGS_SIGNKEY_IN_ROOTCERT      0x00000004 
// Validity of the Signatures within FwCert block
// 0-Capsule Payload signatures are current in FwCert block
// 1-Capsule Payload Signatures are invalid
#define CAPSULE_FLAGS_FWCERTBLOCK_VALID          0x00000008 

#pragma pack()
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017 American Megatrends, Inc.          **
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
