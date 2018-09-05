//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file 
    This file contains the FW_VERSION data structure which is used by AMI 
    utilities to provide information about the BIOS
*/

#ifndef __FID_H__
#define __FID_H__
#ifdef __cplusplus
extern "C" {
#endif


#define FID_SIGNATURE   0x24464944      ///< '$FID' signature of the FW VERSION STRUCTURE
#pragma pack (1)


/**
    This data structure contains the information used by AMI utilities
    to provide information about the current BIOS 

    @note   
    Data structure changed for version 2 of the Firmware Version Structure
    Version 1 is no longer supported
*/
typedef struct{
CHAR8       FirmwareID[4];          ///< Signature '$FID'
UINT8       StructVersion;          ///< Version of the FW_VERSION Struct
UINT16      Size;                   ///< Size of this structure
CHAR8       BiosTag[9];             ///< BIOS Tag
EFI_GUID    FirmwareGuid;           ///< Firmware GUID
CHAR8       CoreMajorVersion[3];    ///< Core Major version
CHAR8       CoreMinorVersion[3];    ///< Core Minor Version
CHAR8       ProjectMajorVersion[3]; ///< Project Major Version
CHAR8       ProjectMinorVersion[3]; ///< Project Minor version
UINT16      Year;                   ///< Build Year of the current BIOS
UINT8       Month;                  ///< Build Month of the current BIOS
UINT8       Day;                    ///< Build Day of the current BIOS
UINT8       Hour;                   ///< Build Hour of the current BIOS
UINT8       Minute;                 ///< Build Minute of the current BIOS
UINT8       Second;                 ///< Build Second of the current BIOS
UINT16      SignOnStringId;         ///< ID of Sign on Message string in TSE string pack
UINT8       OemId[6];               ///< ACPI OEM ID
UINT8       OemTableId[8];          ///< ACPI OEM Table ID
UINT32      OemActivationKeyLength; ///< Length of the OEM Activation key
UINT8       OemActivationKey[49];   ///< OEM Activation key
}FW_VERSION;
#pragma pack ()



/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

