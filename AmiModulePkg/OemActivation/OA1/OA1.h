//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

//*****************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*****************************************************************************
/** @file OA1.h
    Header file for OA1 module.

**/
//*****************************************************************************


#ifndef _AMI_OA1_MODULE_H
#define _AMI_OA1_MODULE_H

#include <Efi.h>
#include <Token.h>


#ifdef __cplusplus
extern "C" {
#endif

// {2EBE0275-6458-4af9-91ED-D3F4EDB100AA}
#define BIOS_FEATURES_SECTION_GUID \
    {0x2ebe0275, 0x6458, 0x4af9, 0x91, 0xed, 0xd3, 0xf4, 0xed, 0xb1, 0x00, 0xaa}

#define OA10_FILE_GUID \
    {0x8E477676, 0x55FD, 0x48cf, 0x92, 0x10, 0x15, 0xA9, 0x9B, 0x27, 0xD7, 0x40}

#define OEM_DATA_LOCATION_BIT   0x80


#pragma pack(1)


/**
    This structure comes out of the AMI Utility Specification.

 Fields:   
  Name        Type        Description
 ---------------------------------------------------------------------------
  Guid        EFI_GUID    Firmware Section GUID 
  Identifier  CHAR8       Identifier String "$ODB$"
  Flag        UINT16      Data Flag
  Size        UINT16      Data Size
  Data        UINT8       Data Buffer (OEM OA 1.0 String)

**/

typedef struct _BIOS_OEM_DATA {
    EFI_GUID Guid;
    CHAR8    Identifier[6];
    UINT16   Flag;
    UINT16   Size;
    UINT8    Data[OEM_DATA_SPACE_SIZE];
} BIOS_OEM_DATA;


typedef struct _OA1_STRUC {
    CHAR8  Signature[6];
    UINT16 Version;
    UINT16 Size;
    CHAR8  String[sizeof(CONVERT_TO_STRING(OA1_STRING))];
    UINT8  Padding[OEM_DATA_SPACE_SIZE - sizeof(CONVERT_TO_STRING(OA1_STRING))];
} OA1_STRUC;

#pragma pack()


#ifdef __cplusplus
}
#endif
#endif


//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
