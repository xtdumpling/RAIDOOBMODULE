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

/** @file SmbiosDmiEditAfri.h
    SmbiosDmiEditAfri Header file.

**/

#ifndef _SmbiosDMIEditFri_DRIVER_H
#define _SmbiosDMIEditFri_DRIVER_H

#include <Token.h>
#include <Protocol/AmiSmbios.h>

extern EFI_BOOT_SERVICES    *pBS;

#define AFRI_DMIEDIT_GUID	{0x39cda386, 0x3988, 0x4073, {0xa6, 0x59, 0x3a, 0x26, 0x84, 0x8e, 0xe5, 0x6f}}

#pragma pack(1)

/**
    Get SMBIOS Information
**/
typedef struct {
    UINT8       Function;
    UINT16      RetStatus;
    UINT8       DmiBiosRevision;
    UINT16      NumberOfStructures;
    UINT16      LargestStructureSize;
    UINT64      DmiStorageBase;
    UINT16      DmiStorageSize;
} AFRI_GET_SMBIOS_INFO;

/**
    Get SMBIOS Structure
**/
typedef struct {
    UINT8       Function;
    UINT16      RetStatus;
    UINT16      Handle;
// Data section with size = largest structure size. This section will be filled with structure data by BIOS
} AFRI_GET_SMBIOS_STRUCTURE;

/**
    Set SMBIOS Structure
**/
typedef struct {
    UINT8       				Function;
    UINT16      				RetStatus;
    UINT8       				Control;
    UINT8       				Command;
    UINT8       				Offset;
    UINT32      				ChangeMask;
    UINT32      				ChangeValue;
    UINT16      				DataLength;
    SMBIOS_STRUCTURE_HEADER     StructureHeader;
// Data to be written to BIOS
} AFRI_SET_SMBIOS_STRUCTURE;

/**
    Get SMBIOS Table
**/
typedef struct {
    UINT8       Function;
    UINT16      RetStatus;
// Data section with size = maximum SMBIOS table size (should be equal to
// DmiStorageSize returned from GetSmbiosInfo (Fn60) call . This section
// will be filled with whole Smbios Table by BIOS
} AFRI_GET_SMBIOS_TABLE;

#pragma pack()

#endif                          // _SmbiosDMIEditFri_DRIVER_H

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
