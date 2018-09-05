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

/** @file
  @brief NVRAM GUID Definitions.

  The file contains definitions of the GUID associated with the NVRAM module.
**/

#ifndef __AMI_NVRAM_GUID__H__
#define __AMI_NVRAM_GUID__H__

/// The GUID of the main NVRAM area.
#define AMI_NVRAM_MAIN_ROM_AREA_GUID \
    { 0xfa4974fc, 0xaf1d, 0x4e5d, { 0xbd, 0xc5, 0xda, 0xcd, 0x6d, 0x27, 0xba, 0xec } }

/// The GUID of the backup NVRAM area.
#define AMI_NVRAM_BACKUP_ROM_AREA_GUID \
    { 0xa7edebd8, 0xa8d7, 0x48f8, { 0x81, 0xfb, 0x83, 0x76, 0x56, 0xb8, 0x20, 0x77 } }

/// The GUID of the NVRAM SMM Communication buffer.
#define AMI_NVRAM_SMM_COMMUNICATION_GUID \
	{ 0x45ac8863, 0xe3fa, 0x4a38, { 0xa2, 0x3c, 0x0, 0xbc, 0xd1, 0xc, 0xbd, 0x50 } }

/// see ::AMI_MAIN_NVRAM_ROM_AREA_GUID
extern EFI_GUID gAmiNvramMainRomAreaGuid;
/// see ::AMI_BACKUP_NVRAM_ROM_AREA_GUID
extern EFI_GUID gAmiNvramBackupRomAreaGuid;
/// see ::AMI_NVRAM_SMM_COMMUNICATION_GUID
extern EFI_GUID gAmiNvramSmmCommunicationGuid;

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
