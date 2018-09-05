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
// $Header: /Alaska/SOURCE/Modules/TCG/TcgPlatformSetupPolicy/TcgPlatformSetupPolicy.h 1     9/27/11 10:11p Fredericko $
//
// $Revision: 1 $
//
// $Date: 9/27/11 10:11p $
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name: TcgPlatformpolicy.h
//
// Description: Header file for TcgPlatformpolicy
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef _TCG_PLATFORM_SETUP_POLICY_H_
#define _TCG_PLATFORM_SETUP_POLICY_H_

#include <Efi.h>
#include <Token.h>
#include <Setup.h>

#define TCG_PLATFORM_SETUP_POLICY_GUID \
  { \
    0xbb6cbeff, 0xe072, 0x40d2, 0xa6, 0xeb, 0xba, 0xb7, 0x5b, 0xde, 0x87, 0xe7 \
  }

#define TCG_PPI_SYNC_FLAG_GUID \
  {\
    0xf3ed95df, 0x828e, 0x41c7, 0xbc, 0xa0, 0x16, 0xc4, 0x19, 0x65, 0xa6, 0x34 \
  }

#define TCG_INTERNAL_FLAGS_GUID \
  {\
    0x70fff0ff, 0xa543, 0x45b9, 0x8b, 0xe3, 0x1b, 0xdb, 0x90, 0x41, 0x20, 0x80 \
  }


//
// Protocol revision number
// Any backwards compatible changes to this protocol will result in an update in the revision number
// Major changes will require publication of a new protocol
//
#define TCG_PLATFORM_SETUP_PROTOCOL_REVISION_1 3

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
//  UINT32   HashPolicy;
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
    UINT8   TcmSupportEnabled;     // 0: Disabled; 1: Enabled
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
    UINT8 Tpm20Device;
    UINT8 PlatformHierarchy;
    UINT8 StorageHierarchy;
    UINT8 EndorsementHierarchy;
    UINT8 InterfaceSel;
    UINT32 PcrBanks;
} TCG_CONFIGURATION;

#pragma pack()

typedef
EFI_STATUS
(EFIAPI * UPDATE_AMI_TCG_STATUS_FLAGS)(
    TCG_CONFIGURATION   *StatusFlags,
    BOOLEAN             UpdateNvram
);


//
// AMT DXE Platform Policiy ====================================================
//
typedef struct _TCG_PLATFORM_SETUP_PROTOCOL
{
    UINT8                 Revision;
    TCG_CONFIGURATION     ConfigFlags;
    UPDATE_AMI_TCG_STATUS_FLAGS  UpdateStatusFlags;
} TCG_PLATFORM_SETUP_PROTOCOL;

extern EFI_GUID gTcgPlatformSetupPolicyGuid;

#endif

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
