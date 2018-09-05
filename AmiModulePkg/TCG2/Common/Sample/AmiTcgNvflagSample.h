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
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgNvflagSample/AmiTcgNvflagSample.h 1     10/08/13 12:03p Fredericko $Revision:
//
// $Date:
//*************************************************************************
// Revision History
// ----------------
//
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: AmiTcgNvflagSample
//
// Description: This is a sample file for support TCG Ppi. It creates and installed
//      a protocal to access the persistent bios tpm flags in Tpm NV 0x50010000.
//      See Ppi Spec 1.2
//
//<AMI_FHDR_END>
//************************************************************************
//#include <AmiDxeLib.h>
#include "Token.h"
#include <Protocol/TcgService.h>
#include <Protocol/TpmDevice.h>
#include <AmiTcg/TCGMisc.h>



#pragma pack(1)

typedef struct
{
    UINT8  Interface : 1; //0=TpmNv, 1= BIOSNv
    UINT8  Reserved1  : 1;
    UINT8  Reserved2  : 1;
    UINT8  Reserved3  : 1;
    UINT8  Reserved4  : 1;
    UINT8  Reserved5  : 1;
    UINT8  Reserved6  : 1;
    UINT8  Reserved7  : 1;
} INTERNAL_BIOS_TPM_PPI_INTERFACE;


typedef struct TPM_NV_READ_CMD
{
    TPM_TAG           tag;
    UINT32            paramSize;
    TPM_COMMAND_CODE  ordinal;
    TPM_NV_INDEX      nvIndex;
    UINT32            offset;
    UINT32            dataSize;
} TPM_NV_READ_CMD;

typedef struct TPM_NV_READ_RET
{
    TPM_TAG           tag;
    UINT32            paramSize;
    TPM_RESULT        returnCode;
    UINT32            dataSize;
    UINT8             data[NV_DATA_SIZE];
    UINT8             nonceEven[20];
    UINT8             nonceOdd[20];
    BOOLEAN           ContinueAuthSession;
    UINT8             ownerAuth[20];
} TPM_NV_READ_RET;

typedef struct TPM_NV_WRITE_CMD_HDR
{
    TPM_TAG            tag;
    UINT32            paramSize;
    TPM_COMMAND_CODE  ordinal;
    TPM_NV_INDEX      nvIndex;
    UINT32            offset;
    UINT32            dataSize;
} TPM_NV_WRITE_CMD_HDR;

typedef struct TPM_NV_WRITE_CMD
{
    TPM_NV_WRITE_CMD_HDR  writeHeader;
    UINT8                 data[NV_DATA_SIZE];
} TPM_NV_WRITE_CMD;


typedef struct TPM_NV_WRITE_RET
{
    TPM_TAG           tag;
    UINT32            paramSize;
    TPM_RESULT        returnCode;
    UINT8             nonceEven[20];
    UINT8             nonceOdd[20];
    BOOLEAN           ContinueAuthSession;
    UINT8             ownerAuth[20];
} TPM_NV_WRITE_RET;


typedef struct _TCPA_DIGEST
{
    UINT8           digest[20];
} TCPA_DIGEST;

typedef struct _TPM_PCR_SELECTION
{
    UINT16          sizeOfSelect;
    UINT8           pcrSelect[3];
} NV_PCR_SELECTION;

typedef struct _TPM_PCR_INFO_SHORT
{
    UINT16          sizeOfSelect;
    UINT8           pcrSelect[3];
    UINT8           localityAtRelease;
    TCPA_DIGEST     digestAtRelease;
} NV_PCR_INFO_SHORT;

typedef struct _TPM_NV_ATTRIBUTES
{
    UINT16          tag;
    UINT32          attributes;
} NV_ATTRIBUTES;

typedef struct _TPM_NV_DATA_PUBLIC1
{
    UINT16          Tag_;
    UINT32          NvIndex;
    NV_PCR_INFO_SHORT  PcrInfoRead;
} NV_DATA_PUBLIC1;

typedef struct _TPM_NV_DATA_PUBLIC2
{
    NV_PCR_INFO_SHORT  PcrInfoWrite;
    NV_ATTRIBUTES   PerMission;
    UINT8           bReadSTClear;
    UINT8           bWriteSTClear;
    UINT8           bWriteDefine;
    UINT32          DataSize;
} NV_DATA_PUBLIC2;

typedef struct _TPM_DEF_NV_DATA
{
    NV_DATA_PUBLIC1 TpmDefineSpaceIn_pubinfo1;
    NV_DATA_PUBLIC2 TpmDefineSpaceIn_pubinfo2;
    TCPA_DIGEST     TpmDefineSpaceIn_encAuth;
} TPM_DEF_NV_DATA;


typedef struct TPM_NV_PUBLIC_DATA
{
    UINT16    tag;
    UINT32    Index;
    UINT16    InfoReadsizeOfSelect;
    UINT16    InfoReadpcrSelect;
    UINT8     InfoReadlocalityAtRelease;
    UINT8         InfoReaddigestAtRelease[20];
    UINT16    InfoWritesizeOfSelect;
    UINT16    InfoWritepcrSelect;
    UINT8     InfoWritelocalityAtRelease;
    UINT8         InfoWritedigestAtRelease[20];
    UINT16    AttribTag;
    UINT32    Attributes;
    BOOLEAN       bReadSTClear;
    BOOLEAN       bWriteSTClear;
    BOOLEAN       bWriteDefine;
    UINT32        dataSize;
} TPM_NV_PUBLIC_DATA;


typedef struct TPM_NV_DEFINESPCE_CMD
{
    TPM_TAG             tag;
    UINT32              paramSize;
    TPM_COMMAND_CODE    ordinal;
    TPM_NV_PUBLIC_DATA  pubInfo;
    UINT8               encAuth[20];
} TPM_NV_DEFINESPCE_CMD;


typedef struct TPM_NV_DEFINESPCE_RET
{
    TPM_TAG             tag;
    UINT32              paramSize;
    TPM_RESULT          returnCode;
    UINT8               nonceEven[20];
    UINT8               nonceOdd[20];
    BOOLEAN             ContinueAuthSession;
    UINT8               ownerAuth[20];
} TPM_NV_DEFINESPCE_RET;


#define INTERNAL_TPM_BADINDEX 0x00000002
#define INTERNAL_TPM_DOING_SELFTEST 0x00000802

#pragma pack()

#define EFI_MAX_BIT       0x80000000
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
