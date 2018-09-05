//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/Tpm20PlatformDxe/Tpm20PlatformDxe.h 1     10/08/13 12:06p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:06p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/Tpm20PlatformDxe/Tpm20PlatformDxe.h $
//
// 1     10/08/13 12:06p Fredericko
// Initial Check-In for Tpm-Next module
//
// 3     8/30/13 11:04p Fredericko
//
// 2     7/11/13 6:17p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20).
//
// 1     7/10/13 5:58p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 11    1/20/12 9:20p Fredericko
//
// 10    12/30/11 5:06p Fredericko
// [TAG]        EIP78141
// [Category]   New Feature
// [Description]    Added hooks to override generic TPM platform hash
// functions.
// [Files]          1. AmiTcgPlatform.sdl
// 2. AmiTcgPlatformPei.h
// 3. AmiTcgPlatformPeiLib.c
// 4. AmiTcgPlatformPeiAfterMem.c
// 5. AmiTcgPlatformDxe.c
// 6. AmiTcgPlatformDxe.h
//
// 9     11/28/11 7:31p Fredericko
// [TAG]        EIP63922
// [Category]   Improvement
// [Description]    Support for Smbios label 30 and up. Callback on
// publishing of Smbios tables
// [Files]      AmiTcgPlatformDxe.c
//
// 8     10/10/11 11:36a Fredericko
// [TAG]        EIP70220
// [Category]   Improvement
// [Description]    Remove dependency on CSM
// [Files]          TcgLegacy.sdl
// AmiTcgPlatformDxe.c
// AmiTcgPlatformDxe.h
// xTcgDxe.c
//
// 7     10/10/11 12:12a Fredericko
// [TAG]        EIP70220
// [Category]   Improvement
// [Description]    Remove dependency on CSM
// [Files]          TcgLegacy.sdl
// AmiTcgPlatformDxe.c
// AmiTcgPlatformDxe.h
// xTcgDxe.c
//
// 6     7/25/11 3:43a Fredericko
// [TAG]        EIP65177
// [Category]   Spec Update
// [Severity]   Minor
// [Description]    Tcg Ppi Spec ver 1.2 update
//
// 5     4/28/11 6:34p Fredericko
// Removed VFR compile
//
// 4     4/26/11 1:54p Fredericko
// Added support for function level override of specific functions. GUID
// definitions added
//
// 3     4/06/11 10:40a Fredericko
// Core 4.6.5.0 build error changes
//
// 2     3/29/11 2:27p Fredericko
// [TAG]        EIP 54642
// [Category] Improvement
// [Description] 1. Checkin Files related to TCG function override
// 2. Include TCM and TPM auto detection
// [Files] Affects all TCG files
//
//
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    AmiTcgPlatformDxe.h
//
// Description: Header file for AmiTcgPlatformDxe
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiTcg/TCGMisc.h>
#include <Token.h>
#include <AmiTcg/TrEEProtocol.h>
#include "Protocol/TpmDevice.h"
#include <AmiTcg/Tpm20.h>
#include <Protocol/HiiFont.h>

//--------------------------------------------------------------------------
//GUID Definitions
//--------------------------------------------------------------------------
#define EFI_SMBIOS_TABLE_GUID \
  { \
    0xeb9d2d31, 0x2d88, 0x11d3, 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d \
  }

#define MBR_SIGNATURE               0xaa55

#define EXTENDED_DOS_PARTITION      0x05
#define EXTENDED_WINDOWS_PARTITION  0x0F

#define MAX_MBR_PARTITIONS          4

#define PMBR_GPT_PARTITION          0xEE
#define EFI_PARTITION               0xEF

#define MBR_SIZE                    512


//---------------------------------------------------------------------------
//      Structure Definitions
//---------------------------------------------------------------------------
#pragma pack(push, 1)

//
// ------------------------------------------------------------------
// Name: Data_Structures
//
// Description:
//      Data Structures needed for MBR
//
//      MASTER_BOOT_RECORD - Data Structure definition of the
//          MBR located in the first block on an MBR drive
//
//      MBR_PARTITION - Data Structure definition for each
//          partition in the MBR
//
// ------------------------------------------------------------------
//
#pragma pack(1)

///
/// Data Structure definition for each partition in the MBR
///
typedef struct _MBR_PARTITION
{
    UINT8 BootIndicator;
    UINT8 StartHead;
    UINT8 StartSector;
    UINT8 StartTrack;
    UINT8 OSIndicator;
    UINT8 EndHead;
    UINT8 EndSector;
    UINT8 EndTrack;
    UINT8 StartingLBA[4];
    UINT8 SizeInLBA[4];
} MBR_PARTITION_RECORD;

///
/// Data Structure definition of the MBR located in the first block on an MBR drive
///
typedef struct _MASTER_BOOT_RECORD
{
    UINT8                 BootStrapCode[440];
    UINT8                 UniqueMbrSignature[4];
    UINT8                 Unknown[2];
    MBR_PARTITION_RECORD  Partition[MAX_MBR_PARTITIONS];
    UINT16                Signature;
} MASTER_BOOT_RECORD;


#define EFI_GPT_HEADER_ID  "EFI PART"


typedef struct
{
    EFI_TABLE_HEADER    Header;
    EFI_LBA             MyLba;
    EFI_LBA             AlternateLba;
    EFI_LBA             FirstUsableLba;
    EFI_LBA             LastUsableLba;
    EFI_GUID            DiskGuid;
    EFI_LBA             PartitionEntryLba;
    UINT32              NumberOfPartitionEntries;
    UINT32              SizeOfPartitionEntry;
    UINT32              Crc32;
} GPT_HEADER;




#define TPM20_PP_NO_ACTION                               0
#define TPM20_PP_CLEAR_CONTROL_CLEAR                     5
#define TPM20_PP_CLEAR_CONTROL_CLEAR_2                   14
#define TPM20_PP_SET_NO_PPI_CLEAR_FALSE                  17
#define TPM20_PP_SET_NO_PPI_CLEAR_TRUE                   18
#define TPM20_PP_CLEAR_CONTROL_CLEAR_3                   21
#define TPM20_PP_CLEAR_CONTROL_CLEAR_4                   22
#define TCPA_PPIOP_SET_PCR_BANKS                         23
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_PCR_FALSE   25
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_PCR_TRUE    26
#define TCPA_PPIOP_VENDOR   128

#define TPM20_PP_NO_ACTION_MAX                           20


typedef UINT8  TPMI_YES_NO;
typedef UINT32 ClearControl_In;
typedef UINT32 Clear_In;

#pragma pack(push, 1)

typedef struct
{
    TPM_ST tag;
    UINT32 paramSize;
    TPM_CC commandCode;
} INT_TPM2_COMMAND_HEADER;

typedef struct
{
    TPM_ST tag;
    UINT32 paramSize;
    TPM_RC responseCode;
} INT_TPM2_RESPONSE_HEADER;

typedef struct
{
    INT_TPM2_COMMAND_HEADER   Header;
    TPMI_RH_CLEAR             Auth;
    UINT32                    AuthorizationSize;
    TPMS_AUTH_COMMAND PwapAuth;
    TPMI_YES_NO               Disable;
} TPM2_CLEAR_CONTROL_COMMAND;

typedef struct
{
    INT_TPM2_RESPONSE_HEADER  Header;
    UINT32                    ParameterSize;
    TPMS_AUTH_COMMAND PwapAuthSeq;
} TPM2_CLEAR_CONTROL_RESPONSE;

typedef struct
{
    INT_TPM2_COMMAND_HEADER   Header;
    TPMI_RH_CLEAR             Auth;
    UINT32                    AuthorizationSize;
    TPMS_AUTH_COMMAND PwapAuth;
} TPM2_CLEAR_COMMAND;

typedef struct
{
    INT_TPM2_RESPONSE_HEADER  Header;
    UINT32                    ParameterSize;
    TPMS_AUTH_COMMAND PwapAuthSeq;
} TPM2_CLEAR_RESPONSE;


typedef struct
{
    INT_TPM2_COMMAND_HEADER     Header;
    UINT16                      bytesRequested;
} TPM2_GetRandom_COMMAND;

typedef struct
{
    INT_TPM2_RESPONSE_HEADER      Header;
    TPM2B_DIGEST                 randomBytes;
} TPM2_GetRandom_RESPONSE;

typedef struct
{
    INT_TPM2_COMMAND_HEADER       Header;
    TPMI_RH_HIERARCHY             AuthHandle;
    UINT32                        AuthorizationSize;
    TPMS_AUTH_COMMAND             AuthSession;
    TPMI_RH_HIERARCHY             Hierarchy;
    TPMI_YES_NO                   State;
} TPM2_HIERARCHY_CONTROL_COMMAND;

typedef struct
{
    INT_TPM2_RESPONSE_HEADER      Header;
    UINT32                        ParameterSize;
    TPMS_AUTH_RESPONSE            AuthSession;
} TPM2_HIERARCHY_CONTROL_RESPONSE;


typedef struct
{
    INT_TPM2_COMMAND_HEADER       Header;
    TPMI_RH_HIERARCHY_AUTH    AuthHandle;
    UINT32                    AuthorizationSize;
    TPMS_AUTH_COMMAND         AuthSession;
    TPM2B_AUTH                NewAuth;
} TPM2_HIERARCHY_CHANGE_AUTH_COMMAND;

typedef struct
{
    INT_TPM2_RESPONSE_HEADER       Header;
    UINT32                     ParameterSize;
    TPMS_AUTH_RESPONSE         AuthSession;
} TPM2_HIERARCHY_CHANGE_AUTH_RESPONSE;


typedef struct _PCI_OPROM_TAG
{
    UINT32 EventID;
    UINT32 EventSize;
    UINT16 Reserved;
    UINT16 PFA;
} PCI_OPROM_TAG;

typedef struct tdTPM2SHA2DIGEST
{
    UINT8        digest[TPM_SHA2_256_HASH_LEN];
} TPM2SHA2DIGEST;

#pragma pack(pop)
#pragma pack(pop)

//**********************************************************************
//              Function Declarations
//**********************************************************************

typedef EFI_STATUS (MEASURE_BOOT_VAR_FUNC_PTR)(

);

extern MEASURE_BOOT_VAR_FUNC_PTR        *MeasureAllBootVariablesFuncPtr;

typedef EFI_STATUS (TPM20_MEASURE_PLATFORM_CONFIG_FUNC_PTR)(

);

extern TPM20_MEASURE_PLATFORM_CONFIG_FUNC_PTR *Tpm20MeasureConfigurationInfoFuncPtr;

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
