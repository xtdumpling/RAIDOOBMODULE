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
/** @file OA3.h
    Header file for OA3 module.

**/
//*****************************************************************************

#ifndef __MS_OA3_H
#define __MS_OA3_H

//----------------------------------------------------------------------------

#include <Efi.h>
#include <Acpi.h>

//----------------------------------------------------------------------------

#define MSDM_SIG                0x4D44534D  // "MSDM" - M$ Digital Marker
#define MSDM_REVISION           0x03        // 
#define MSDM_VERSION            0x01        // 
#define MSDM_DATA_TYPE          0x01        // 
#define FID_SUPPORTED_VERSION   0x04        // Supported version of the FID table

#define FID_TABLE_GUID \
  { 0x3fd1d3a2, 0x99f7, 0x420b, 0xbc, 0x69, 0x8b, 0xb1, 0xd4, 0x92, 0xa3, 0x32 }

#define FID_TABLE_SECTION_GUID \
  { 0x2ebe0275, 0x6458, 0x4af9, 0x91, 0xed, 0xd3, 0xf4, 0xed, 0xb1, 0x00, 0xaa }

#ifndef AMI_GLOBAL_VARIABLE_GUID
#define AMI_GLOBAL_VARIABLE_GUID \
  { 0x01368881, 0xc4ad, 0x4b1d, 0xb6, 0x31, 0xd5, 0x7a, 0x8e, 0xc8, 0xdb, 0x6b }
#endif

#define EFI_OA3_MSDM_VARIABLE       L"OA3MSDMvariable"

// if Product Key located in the NBC
#if OEM_ACTIVATION_TABLE_LOCATION == 1
#define FLASH_DEVICE_BASE_ADDRESS       (0xFFFFFFFF-FLASH_SIZE+1)
#define OEM_ACTIVATION_BLOCK_ADDRESS    (OEM_ACTIVATION_TABLE_ADDRESS-FLASH_DEVICE_BASE_ADDRESS)
#define OEM_ACTIVATION_BLOCK_END        (OEM_ACTIVATION_BLOCK_ADDRESS+OEM_ACTIVATION_TABLE_SIZE)
#define OEM_ACTIVATION_TABLE_OFFSET     (0xFFFFFFFF-OEM_ACTIVATION_TABLE_ADDRESS+1)
#endif

#ifndef FV_BB_END_ADDR
#define FV_BB_END_ADDR                  (0xFFFFFFFF-16-sizeof(EFI_GUID)+1)
#endif

#ifndef FV_BB_SIZE
#define FV_BB_SIZE                      (0xFFFFFFFF-FV_BB_BASE+1)
#endif

//----------------------------------------------------------------------------

#pragma pack (1)

/**
    This structure comes out of the document "M$ Digital Marker Pilot"

 Fields:   
  Name            Length      Description
  ------------------------------------------------------------------
  Header          -           Standard ACPI header
  Version         4           0x1
  Reserved        4           0x0
  DataType        4           0x1
  DataReserved    4           0x0
  DataLength      4           0x1D
  Data            29          5x5 Product Key, including "-" (BH3RN-B7FDM-C7WGT-4CR4X-6CKHM) 

**/

typedef struct _EFI_ACPI_MSDM_TABLE {
    ACPI_HDR    Header;
    UINT32      Version;
    UINT32      Reserved;
    UINT32      DataType;
    UINT32      DataReserved;
    UINT32      DataLength;
    UINT8       Data[29];
} EFI_ACPI_MSDM_TABLE;


typedef struct _EFI_OA3_MSDM_STRUCTURE {
    EFI_PHYSICAL_ADDRESS  XsdtAddress;
    EFI_PHYSICAL_ADDRESS  MsdmAddress;
    EFI_PHYSICAL_ADDRESS  ProductKeyAddress;
} EFI_OA3_MSDM_STRUCTURE;


#pragma pack()

//---------------------------------------------------------------------------

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
