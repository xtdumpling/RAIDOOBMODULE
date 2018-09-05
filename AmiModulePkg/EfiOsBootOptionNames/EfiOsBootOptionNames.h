//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file  EfiOsBootOptionNames.h
    Header file for EfiOsBootOptionNames module.
**/

#ifndef _EFI_OS_BOOT_OPTION_NAMES_H_
#define _EFI_OS_BOOT_OPTION_NAMES_H_
#ifdef __cplusplus
extern "C" {
#endif

#define EFI_BOOT_FILE_NAME EFI_REMOVABLE_MEDIA_FILE_NAME

#define EFI_OS_BOOT_OPTION_NAMES_GUID \
    {0x69ECC1BE, 0xA981, 0x446D, 0x8E, 0xB6, 0xAF, 0x0E, 0x53, 0xD0, 0x6C, 0xE8}

#pragma pack(1)
typedef struct _PARTITION_ENTRY {
    UINT8 ActiveFlag;               	///< Bootable or not
    UINT8 StartingTrack;            	///< Not used
    UINT8 StartingCylinderLsb;      	///< Not used
    UINT8 StartingCylinderMsb;      	///< Not used
    UINT8 PartitionType;            	///< 12 bit FAT, 16 bit FAT etc.
    UINT8 EndingTrack;              	///< Not used
    UINT8 EndingCylinderLsb;        	///< Not used
    UINT8 EndingCylinderMsb;        	///< Not used
    UINT32 StartSector;                 ///< Relative sectors
    UINT32 PartitionLength;         	///< Sectors in this partition
} PARTITION_ENTRY;

typedef struct
{
	CHAR16 *FilePath;            ///< Path to open the OS image.
	CHAR16 *BootOptionName;      ///< Or, OS Name.
}NAME_MAP;

#pragma pack()

typedef struct {
    BOOLEAN                   Invalid;
    EFI_LOAD_OPTION           *NvramOption;
    UINTN                     Size;
    CHAR16                    *Desc;
	EFI_DEVICE_PATH_PROTOCOL  *Dp;
    UINT16                    Num;
    BOOLEAN                   Fw;
}SIMPLE_BOOT_OPTION;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
