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
// $Header: /Alaska/SOURCE/Modules/TCG/TpmClearOnRollBack/TpmClearOnRollbackWrapperLib.h 1     5/23/13 7:49p Fredericko $
//
// $Revision: 1 $
//
// $Date: 5/23/13 7:49p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TCG/TpmClearOnRollBack/TpmClearOnRollbackWrapperLib.h $
//
// 1     5/23/13 7:49p Fredericko
// [TAG]        EIP121823
// [Category]   New Feature
// [Description]    Clear TPM on BIOS rollback
// [Files]          TpmClearOnRollback.cif
// TpmClearOnRollback.c
// TpmClearOnRollback.h
// TpmClearOnRollback.sdl
// TpmClearOnRollback.mak
// TpmClearOnRollback.dxs
// TpmClearOnRollbackSmiFlash.c
// TpmClearOnRollbackWrapperLib.c
// TpmClearOnRollbackWrapperLib.h
//
//
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:
//
// Description:
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef _TPM_CLEAR_ON_ROLLBACK_WRAPPER_H
#define _TPM_CLEAR_ON_ROLLBACK_WRAPPER_H

//#include <AmiDxeLib.h>
#include <Protocol/TcgService.h>
#include <AmiTcg/TCGMisc.h>
//#include <Protocol\SmmBase2.h>
#include <Protocol/AmiTpmClearOnRollbackProtocol.h>
#include <Ppi/FwVersion.h>
#include <Hob.h>
#include <Token.h>
//#include <AmiHobs.h>

#define FID_FFS_FILE_NAME_GUID \
    { 0x3fd1d3a2, 0x99f7, 0x420b, 0xbc, 0x69, 0x8b, 0xb1, 0xd4, 0x92, 0xa3, 0x32 }

#define FID_FFS_FILE_SECTION_GUID \
    { 0x2EBE0275, 0x6458, 0x4AF9, 0x91, 0xED, 0xD3, 0xF4, 0xED, 0xB1, 0x00, 0xAA }

#define TPM_BASE_ADDRESS   0xfed40000
#define BASE 0x0
#define INVALID_POSTINIT   ((TPM_RESULT) (BASE + 38))
#define DISABLED       ((TPM_RESULT) (BASE + 7))
#define DEACTIVATED    ((TPM_RESULT) (BASE + 6))

#define TPM_H2NS( x ) \
    (((UINT16)(x) << 8) | ((UINT16)(x) >> 8))
#define TPM_H2NL( x ) \
    (UINT32)((((UINT32)(x)) >> 24)    \
             | (((x) >> 8) & 0xff00)  \
             | ((x) << 24)            \
             | (((x) & 0xff00) << 8))

#define TPM_N2HS( x ) TPM_H2NS( x )
#define TPM_N2HL( x ) TPM_H2NL( x )

#pragma pack(1)

typedef struct _TPM_1_2_REGISTERS
{
    UINT8                             Access;             // 0
    UINT8                             Reserved1[7];       // 1
    UINT32                            IntEnable;          // 8
    UINT8                             IntVector;          // 0ch
    UINT8                             Reserved2[3];       // 0dh
    UINT32                            IntSts;             // 10h
    UINT32                            IntfCapability;     // 14h
    UINT8                             Sts;                // 18h
    UINT16                            BurstCount;         // 19h
    UINT8                             Reserved3[9];
    UINT32                            DataFifo;           // 24h
    UINT8                             Reserved4[0xed8];   // 28h
    UINT16                            Vid;                // 0f00h
    UINT16                            Did;                // 0f02h
    UINT8                             Rid;                // 0f04h
    UINT8                             TcgDefined[0x7b];   // 0f05h
    UINT32                            LegacyAddress1;     // 0f80h
    UINT32                            LegacyAddress1Ex;   // 0f84h
    UINT32                            LegacyAddress2;     // 0f88h
    UINT32                            LegacyAddress2Ex;   // 0f8ch
    UINT8                             VendorDefined[0x70];// 0f90h
} TPM_1_2_REGISTERS;

typedef volatile TPM_1_2_REGISTERS *TPM_1_2_REGISTERS_PTR;

typedef struct _TPM_TRANSMIT_BUFFER
{
    VOID                              *Buffer;
    UINTN                             Size;
} TPM_TRANSMIT_BUFFER;

typedef struct _TPM_1_2_CMD_HEADER
{
    TPM_TAG          Tag;
    UINT32           ParamSize;
    TPM_COMMAND_CODE Ordinal;
} TPM_1_2_CMD_HEADER;

typedef struct _TPM_1_2_RET_HEADER
{
    TPM_TAG    Tag;
    UINT32     ParamSize;
    TPM_RESULT RetCode;
} TPM_1_2_RET_HEADER;

typedef struct _TPM_1_2_CMD_STARTUP
{
    TPM_1_2_CMD_HEADER Header;
    TPM_STARTUP_TYPE   StartupType;
} TPM_1_2_CMD_STARTUP;

typedef struct _PHYSICAL_PRESENCE_CMD
{
    TPM_1_2_CMD_HEADER  Hdr;
    UINT16              Data;
} PHYSICAL_PRESENCE_CMD;

typedef struct _DATA_CMD
{
    TPM_1_2_CMD_HEADER  Hdr;
    UINT8               Data;
} DATA_CMD;

#pragma pack()

EFI_STATUS
EFIAPI IsTpmPresent (
    IN TPM_1_2_REGISTERS_PTR TpmReg );

EFI_STATUS
EFIAPI TisReleaseLocality (
    IN TPM_1_2_REGISTERS_PTR TpmReg );

EFI_STATUS
EFIAPI TisRequestLocality (
    IN TPM_1_2_REGISTERS_PTR TpmReg );

EFI_STATUS
EFIAPI TpmLibPassThrough (
    IN TPM_1_2_REGISTERS_PTR   TpmReg,
    IN UINTN                   NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINTN                   NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers );

BOOLEAN
EFIAPI IsTcmSupportType ();

EFI_STATUS TpmRecoveryGetFidFromFv(
    OUT VOID *Fid
);

EFI_STATUS TpmGetFidFromBuffer(
    OUT VOID *Fid,
    IN VOID *Buffer
);

UINT8 ClearTpmBeforeFlash();

void
EFIAPI
FixedDelay(UINT32 dCount);

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
