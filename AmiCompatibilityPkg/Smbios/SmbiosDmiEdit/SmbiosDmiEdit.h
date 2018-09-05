//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

/** @file SmbiosDmiEdit.h
    SmbiosDmiEdit Header file.

**/

#ifndef _SmbiosDMIEdit_DRIVER_H
#define _SmbiosDMIEdit_DRIVER_H

#include <Efi.h>
#include <Token.h>

extern EFI_BOOT_SERVICES		*pBS;

#define WRITE_ONCE_ENTRIES  0x10                // Maximum number of WRITE_ONCE_STATUS entries

#pragma pack(1)

/**
    SMBIOS Entry Point structure
**/
typedef struct {
    UINT32    AnchorString;
    UINT8     EntryPointStructChecksum;
    UINT8     EntryPointLength;
    UINT8     MajorVersion;
    UINT8     MinorVersion;
    UINT16    MaximumStructSize;
    UINT8     EntryPointRevision;
    UINT8     FormattedArea[5];
    UINT8     IntermediateAnchor[5];
    UINT8     IntermediateChecksum;
    UINT16    StructTableLength;
    UINT32    StructTableAddress;
    UINT16    NumStructs;
    UINT8     SmbiosBCDRevision;
} SMBIOS_TABLE_ENTRY;

/**
    Structure Header
**/
typedef struct {
    UINT8		Type;
    UINT8		Length;
    UINT16		Handle;
} DMI_STRUC;

/**
    Set SMBIOS Structure Data
**/
typedef struct {
    UINT8     Command;
    UINT8     FieldOffset;
    UINT32    ChangeMask;
    UINT32    ChangeValue;
    UINT16    DataLength;
    DMI_STRUC StructureHeader;
    UINT8     StructureData[1];
} SET_SMBIOS_STRUCTURE_DATA;

//----------------------------------------------------------------------------
// 						For SMBIOS version 2.x
//----------------------------------------------------------------------------

/**
    Get SMBIOS Information
**/
typedef struct {
    UINT16		Function;
    UINT32		DmiBiosRevision32BitAddr;
    UINT32		NumStructures32BitAddr;
    UINT32		StructureSize32BitAddr;
    UINT32		DmiStorageBase32BitAddr;
    UINT32		DmiStorageSize32BitAddr;
    UINT16		BiosSelector; //Always 0.
    UINT16		RetStatus;
} GET_SMBIOS_INFO;

/**
    Get SMBIOS Structure
**/
typedef struct {
    UINT16		Function;
    UINT32		Handle32BitAddr;
    UINT32		Buffer32BitAddr;
    UINT16		DmiSelector;  //Always 0
    UINT16		BiosSelector; //Always 0
    UINT16		RetStatus;
} GET_SMBIOS_STRUCTURE;

/**
    Set SMBIOS Structure
**/
typedef struct {
    UINT16		Function;
    UINT32		Buffer32BitAddr;
    UINT32		DmiWorkBuffer32BitAddr;
    UINT8		Control;    //?
    UINT16		DmiSelector;  //Always 0
    UINT16		BiosSelector; //Always 0
    UINT16		RetStatus;
} SET_SMBIOS_STRUCTURE;

//----------------------------------------------------------------------------
// 						For SMBIOS version 3.x
//----------------------------------------------------------------------------

/**
    Get SMBIOS Information
**/
typedef struct {
    UINT16		Function;
    UINT64		DmiBiosRevision64BitAddr;
    UINT64		NumStructures64BitAddr;
    UINT64		StructureSize64BitAddr;
    UINT64		DmiStorageBase64BitAddr;
    UINT64		DmiStorageSize64BitAddr;
    UINT16		BiosSelector; //Always 0.
    UINT16		RetStatus;
} GET_SMBIOS_V3_INFO;

/**
    Get SMBIOS Structure
**/
typedef struct {
    UINT16		Function;
    UINT64		Handle64BitAddr;
    UINT64		Buffer64BitAddr;
    UINT16		DmiSelector;  //Always 0
    UINT16		BiosSelector; //Always 0
    UINT16		RetStatus;
} GET_SMBIOS_V3_STRUCTURE;

/**
    Set SMBIOS Structure
**/
typedef struct {
    UINT16		Function;
    UINT64		Buffer64BitAddr;
    UINT64		DmiWorkBuffer64BitAddr;
    UINT8		Control;
    UINT16		DmiSelector;  //Always 0
    UINT16		BiosSelector; //Always 0
    UINT16		RetStatus;
} SET_SMBIOS_V3_STRUCTURE;

/**
    Get SMBIOS Table
**/
typedef struct {
    UINT16		Function;
    UINT8		*SmbiosTableBufferAddr;
    UINT16		SmbiosTableBufferSize;
    UINT16		RetStatus;
} GET_SMBIOS_V3_TABLE;

#define DMIEDIT_WRITE_ONCE      0x01
#define DMIEDIT_DELETE_STRUC    0x02
#define DMIEDIT_ADD_STRUC       0x04
#define DMIEDIT_EXTENDED_HDR    0x80

/**
    DMI data record
**/
typedef struct {
    UINT8   Type;       ///< Structure type
    UINT8   Offset;     ///< Structure field offset, or string number for Type 11 and 12
    UINT8   Reserved;   ///< Size of string including \0 or UUID (16)
    UINT8   Flags;      ///< Bit0 = Write Once
                        ///< Bit1 = Delete Structure
                        ///< Bit2 = Add structure
                        ///< Bit7 = Extended Header
    UINT8   HdrLength;  ///< Header size
    UINT16  Size;       ///< Structure size
    UINT16  Handle;     ///< Structure handle number
} TABLE_INFO;

/**
    Write Once structure
**/
typedef struct {
    UINT8   Type;       ///< Structure type
    UINT8   Offset;     ///< Offset in structure
  	BOOLEAN WriteOnce;  ///< TRUE = Data field can only be updated once
} WRITE_ONCE_TABLE;

/**
    Write Once status
**/
typedef struct {
    UINT8   Type;       ///< Structure type
    UINT8   Offset;     ///< Offset in structure
    UINT16  Handle;     ///< Structure handle number
} WRITE_ONCE_STATUS;

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2   // FV_BB or FV_MAIN

/**
    Flash Data Information
**/
typedef struct {
	UINT8		*Location;
	UINT16		Size;
	UINT8		*EndOfData;
} FLASH_DATA_INFO;

extern UINT8  					*gBlockSave;
extern VOID   					*gFlashData;
extern UINT32 					gFlashDataSize;

#endif                                          // SMBIOS_DMIEDIT_DATA_LOC

#if (defined(NonSmiDmiEdit_Support) && (NonSmiDmiEdit_Support == 1))
#define AMI_DMIEDIT_SMBIOS_GUID \
    { 0x74211cd7, 0x3d8e, 0x496f, { 0xba, 0x2, 0x91, 0x9c, 0x2e, 0x1f, 0x6, 0xcb } }

typedef UINT32  (EFIAPI *DMIEDIT_NONSMI_HANDLER) (
    IN UINT8                    Data,
    IN UINT64                   pCommBuff
);

/**
    Smbios DmiEdit Protocol (Non-SMI)
**/
typedef struct {
    DMIEDIT_NONSMI_HANDLER      DmiEditNonSmiHandler;
} EFI_SMBIOS_DMIEDIT_PROTOCOL;

UINT32 DmiEditNonSmiHandler(
    IN UINT8            Data,
    IN UINT64           pCommBuff
);
#endif                                          // NonSmiDmiEdit_Support

#pragma pack()

UINT16
GetSmbiosInfo(
);

UINT16
GetSmbiosStructure(
);

UINT16
SetSmbiosStructure(
    IN SET_SMBIOS_STRUCTURE_DATA    *Data,
    IN UINT8                        Control
);

UINT16
GetNumberOfStructures(
    IN UINT8    *Buffer
);

UINT16
GetLargestStructureSize(
    IN UINT8     *Buffer
);

UINT16
GetSmbiosV3Table(
    IN OUT  GET_SMBIOS_V3_TABLE   *p
);

#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
