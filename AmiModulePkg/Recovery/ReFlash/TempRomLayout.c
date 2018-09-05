//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
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
  This is temporary file to describe ROM layout
  It will be removed once ROM layout wizard is upgraded
**/
#include <Token.h>
#include <AmiRomLayout.h>

#ifndef FtRecovery_SUPPORT
#define FtRecovery_SUPPORT 0
#endif

#ifndef BUILD_TIME_BACKUP
#define BUILD_TIME_BACKUP 0
#endif

#ifndef EXTRA_FW_VOLUME
#define EXTRA_FW_VOLUME
#endif

#define AMI_ROM_LAYOUT_FV_BB_GUID \
{ 0x61C0F511, 0xA691, 0x4F54, 0x97, 0x4F, 0xB9, 0xA4, 0x21, 0x72, 0xCE, 0x53 }

#define AMI_ROM_LAYOUT_FV_RECOVERY_GUID \
{ 0x764D41AC, 0x282D, 0x4777, 0xBF, 0x9F, 0xDC, 0x16, 0x71, 0xC1, 0x0F, 0x36 }

#define AMI_ROM_LAYOUT_FV_MAIN_GUID \
{ 0x5C60F367, 0xA505, 0x419A, 0x85, 0x9E, 0x2A, 0x4F, 0xF6, 0xCA, 0x6F, 0xE5 }

#define AMI_ROM_LAYOUT_NVRAM_GUID \
{ 0xfa4974fc, 0xaf1d, 0x4e5d, { 0xbd, 0xc5, 0xda, 0xcd, 0x6d, 0x27, 0xba, 0xec } }

static AMI_ROM_AREA TempRomLayout[] = {
#if FtRecovery_SUPPORT
//FV_BB
    { 
        AMI_ROM_LAYOUT_FV_BB_GUID,
        AMI_ROM_LAYOUT_FV_BB_ADDRESS,
        AMI_ROM_LAYOUT_FV_BB_OFFSET,
        AMI_ROM_LAYOUT_FV_BB_SIZE,
        0,								//AmiRomAreaTypeFv
        AMI_ROM_AREA_SIGNED | AMI_ROM_AREA_VITAL | AMI_ROM_AREA_FV_PEI
    },
//FV_RECOVERY
    { 
        AMI_ROM_LAYOUT_FV_RECOVERY_GUID,
        AMI_ROM_LAYOUT_FV_RECOVERY_ADDRESS,
        AMI_ROM_LAYOUT_FV_RECOVERY_OFFSET,
        AMI_ROM_LAYOUT_FV_RECOVERY_SIZE,
        0,								//AmiRomAreaTypeFv
        AMI_ROM_AREA_SIGNED | AMI_ROM_AREA_VITAL | AMI_ROM_AREA_FV_PEI
    },
#if BUILD_TIME_BACKUP
//FV_BB_BACKUP
    { 
        AMI_ROM_LAYOUT_FV_BB_GUID,
        AMI_ROM_LAYOUT_FV_BB_BACKUP_ADDRESS,
        AMI_ROM_LAYOUT_FV_BB_BACKUP_OFFSET,
        AMI_ROM_LAYOUT_FV_BB_SIZE,
        0,								//AmiRomAreaTypeFv
        AMI_ROM_AREA_SIGNED | AMI_ROM_AREA_VITAL
    },
//FV_RECOVERY_BACKUP
    { 
        AMI_ROM_LAYOUT_FV_RECOVERY_GUID,
        AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_ADDRESS,
        AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_OFFSET,
        AMI_ROM_LAYOUT_FV_RECOVERY_SIZE,
        0,								//AmiRomAreaTypeFv
        AMI_ROM_AREA_SIGNED | AMI_ROM_AREA_VITAL
    },
#endif  //BUILD_TIME_BACKUP
#else   //FtRecovery_SUPPORT
//FV_BB
    { 
        AMI_ROM_LAYOUT_FV_BB_GUID,
        AMI_ROM_LAYOUT_FV_BB_ADDRESS,
        AMI_ROM_LAYOUT_FV_BB_OFFSET,
        AMI_ROM_LAYOUT_FV_BB_SIZE,
        0,								//AmiRomAreaTypeFv
        AMI_ROM_AREA_SIGNED | AMI_ROM_AREA_VITAL | AMI_ROM_AREA_FV_PEI
    },
#endif  //FtRecovery_SUPPORT
//FV_MAIN
    { 
        AMI_ROM_LAYOUT_FV_MAIN_GUID,
        AMI_ROM_LAYOUT_FV_MAIN_ADDRESS,
        AMI_ROM_LAYOUT_FV_MAIN_OFFSET,
        AMI_ROM_LAYOUT_FV_MAIN_SIZE,
        0,								//AmiRomAreaTypeFv
        AMI_ROM_AREA_SIGNED | AMI_ROM_AREA_FV_DXE
    },
//NVRAM
    { 
        AMI_ROM_LAYOUT_NVRAM_GUID,
        AMI_ROM_LAYOUT_NVRAM_ADDRESS,
        AMI_ROM_LAYOUT_NVRAM_OFFSET,
        AMI_ROM_LAYOUT_NVRAM_SIZE,
        0,								//AmiRomAreaTypeFv
        AMI_ROM_AREA_SIGNED
    }
//NVRAM_BACKUP
#if FAULT_TOLERANT_NVRAM_UPDATE
    ,{ 
        AMI_ROM_LAYOUT_NVRAM_GUID,
        AMI_ROM_LAYOUT_NVRAM_BACKUP_ADDRESS,
        AMI_ROM_LAYOUT_NVRAM_BACKUP_OFFSET,
        AMI_ROM_LAYOUT_NVRAM_SIZE,
        1,								//AmiRomAreaTypeRaw
        AMI_ROM_AREA_SIGNED
    }
#endif
    EXTRA_FW_VOLUME
};

static UINT32 TempLayoutSize = sizeof (TempRomLayout);
static UINT32 TempAreaSize = sizeof (AMI_ROM_AREA);

/**
  This function returns current ROM layout
  
  @param Layout     Pointer where to store pointer to ROM layout
  @param AreaSize   Pointer where to store area size
  @param LayoutSize Pointer where to store layout size

  @retval EFI_SUCCESS Layout returned successfully
  @retval other       error occured during execution

**/
EFI_STATUS TempGetRomLayout (
    AMI_ROM_AREA **Layout,
    UINT32 *AreaSize,
    UINT32 *LayoutSize
    )
{
    *Layout = TempRomLayout;
    *AreaSize = TempAreaSize;
    *LayoutSize = TempLayoutSize;
    return EFI_SUCCESS;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
