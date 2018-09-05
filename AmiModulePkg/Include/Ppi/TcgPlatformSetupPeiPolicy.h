//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgSetupPolicy/TcgPlatformSetupPeiPolicy.h 1     10/08/13 12:05p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:05p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/TcgSetupPolicy/TcgPlatformSetupPeiPolicy.h $
//
// 1     10/08/13 12:05p Fredericko
// Initial Check-In for Tpm-Next module
//
// 1     7/10/13 5:56p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 2     12/18/11 10:25p Fredericko
// Changes to support TcgplatformPeiPolicy in relation to O.S. requests.
//
// 1     9/27/11 10:10p Fredericko
// [TAG]        EIP67286
// [Category]   Improvement
// [Description]    Initial check-in for Tcg Setup policy for pei
// [Files]          TcgPlatformSetupPeiPolicy.cif
// TcgPlatformSetupPeiPolicy.c
// TcgPlatformSetupPeiPolicy.h
// TcgPlatformSetupPeiPolicy.sdl
// TcgPlatformSetupPeiPolicy.mak
// TcgPlatformSetupPeiPolicy.dxs
//
//
//**********************************************************************
//<AMI_FHDR_START>
//---------------------------------------------------------------------------
// Name: TcgPlatformpeipolicy.h
//
// Description: Header file for TcgPlatformpeipolicy
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _TCG_PLATFORM_SETUP_PEI_POLICY_H_
#define _TCG_PLATFORM_SETUP_PEI_POLICY_H_

#include <Uefi.h>
#include <Ppi/ReadOnlyVariable2.h>



#define TCG_PLATFORM_SETUP_PEI_POLICY_GUID \
  { \
    0xa76b4e22, 0xb50a, 0x401d, 0x8b, 0x35, 0x51, 0x24, 0xb0, 0xba, 0x41, 0x4 \
  }

#define TCG_PPI_SYNC_FLAG_GUID \
  {\
    0xf3ed95df, 0x828e, 0x41c7, 0xbc, 0xa0, 0x16, 0xc4, 0x19, 0x65, 0xa6, 0x34 \
  }

#define PEI_TCG_INTERNAL_FLAGS_GUID \
  {\
    0x70fff0ff, 0xa543, 0x45b9, 0x8b, 0xe3, 0x1b, 0xdb, 0x90, 0x41, 0x20, 0x80 \
  }

//
// Protocol revision number
// Any backwards compatible changes to this protocol will result in an update in the revision number
// Major changes will require publication of a new protocol
//
#define TCG_PLATFORM_SETUP_PEI_PROTOCOL_REVISION_1 3

#pragma pack(1)
typedef struct
{
    //
    // Byte 0, bit definition for functionality enable/disable
    //
    UINT8   TpmSupport;           // 0: Disabled; 1: Enabled

    UINT8   TpmEnable;            // 0: Disabled; 1: Enabled
    UINT8   TpmAuthenticate;
    UINT8   TpmOperation;           // 0: Disabled; 1: Enabled
    UINT8   DisallowTpm;           // 0: Disabled; 1: Enabled
//  UINT32  HashPolicy;
    UINT8   Tcg2SpecVersion;
    UINT8   DeviceType;

    //
    // Byte 1, bit definition for Status Information
    //
    UINT8   TpmHardware;     // 0: Disabled; 1: Enabled
    UINT8   TpmEnaDisable;
    UINT8   TpmActDeact;
    UINT8   TpmOwnedUnowned;
    UINT8   TcgSupportEnabled;     // 0: Disabled; 1: Enabled
    UINT8   TcmSupportEnabled;
    UINT8   TpmError;
    UINT8   PpiSetupSyncFlag;
    UINT8   Reserved3;

    //
    // Byte 2, Reserved bytes
    //
    UINT8   Reserved4;

    //
    // Byte 3, Reserved bytes
    //
    UINT8   Reserved5;

    //TPM 20 Configuration
    UINT8   Tpm20Device;
    UINT8   PlatformHierarchy;
    UINT8   StorageHierarchy;
    UINT8   EndorsementHierarchy;
    UINT8   InterfaceSel;
    UINT32  PcrBanks;
} TCG_CONFIGURATION;


typedef struct _AMI_TCG_PEI_POLICY_HOB
{
    EFI_HOB_GUID_TYPE EfiHobGuidType;
    TCG_CONFIGURATION ConfigInfo;
} AMI_TCG_PEI_POLICY_HOB;

#pragma pack()


typedef
EFI_STATUS
(EFIAPI * GET_TCG_PEI_POLICY)(
    IN EFI_PEI_SERVICES     **PeiServices ,
    IN TCG_CONFIGURATION    *ConfigFlags
);


//
// AMT DXE Platform Policiy ====================================================
//
typedef struct _TCG_PLATFORM_SETUP_INTERFACE
{
    UINT8                 Revision;
    GET_TCG_PEI_POLICY    getTcgPeiPolicy;
} TCG_PLATFORM_SETUP_INTERFACE;


#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
