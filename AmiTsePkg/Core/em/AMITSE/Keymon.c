//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Keymon.c $
//
// $Author: Arunsb $
//
// $Revision: 5 $
//
// $Date: 11/04/11 4:37a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file Keymon.c
    Contains key monitoring functions.

**/
//**********************************************************************

#ifdef TSE_FOR_APTIO_4_50

#include "Token.h"
#include <Efi.h>

#else //#ifdef TSE_FOR_APTIO_4_50

#include "minisetup.h"

#endif //#ifdef TSE_FOR_APTIO_4_50

#if SETUP_SUPPORT_KEY_MONITORING

#include <Protocol/KeyMonPlatform.h>
#include <Protocol/MonitorKeyFilter.h>

#include "KeyMon.h"


EFI_STATUS
GetPlatformMonitorKeyOptions(
  IN   EFI_KEYMON_PLATFORM_PROTOCOL *This,
  OUT  KEY_ELEMENT                    **KeyList,
  OUT  UINTN                          *KeyListSize
  );

static EFI_GUID EfiKeyMonPlatformProtocolGuid = EFI_KEYMON_PLATFORM_PROTOCOL_GUID;

static EFI_KEYMON_PLATFORM_PROTOCOL gKeyMonPlatformProtocol =
{
    GetPlatformMonitorKeyOptions
};

enum SETUP_KEYS
{
    SK_EntryKey
#if SETUP_ALT_ENTRY_ENABLE
    ,SK_AltEntryKey
#endif
#if SETUP_BBS_POPUP_ENABLE
    ,SK_PopupBootEntryKey
#endif
#if SETUP_OEM_KEY1_ENABLE
    ,SK_OemKey1
#endif
#if SETUP_OEM_KEY2_ENABLE
    ,SK_OemKey2
#endif
#if SETUP_OEM_KEY3_ENABLE
    ,SK_OemKey3
#endif
#if SETUP_OEM_KEY4_ENABLE
    ,SK_OemKey4
#endif
};

KEY_ELEMENT gLegacyKeyTable[]=
{
    {SETUP_ENTRY_LEGACY_CODE,{0,0,0,0}}
#if SETUP_ALT_ENTRY_ENABLE
    ,{SETUP_ALT_ENTRY_LEGACY_CODE,{0,0,0,0}}
#endif
#if SETUP_BBS_POPUP_ENABLE
    ,{POPUP_MENU_ENTRY_LEGACY_CODE,{0,0,0,0}}
#endif
#if SETUP_OEM_KEY1_ENABLE
    ,{SETUP_OEM_KEY1_LEGACY_CODE,{0,0,0,0}}
#endif
#if SETUP_OEM_KEY2_ENABLE
    ,{SETUP_OEM_KEY2_LEGACY_CODE,{0,0,0,0}}
#endif
#if SETUP_OEM_KEY3_ENABLE
    ,{SETUP_OEM_KEY3_LEGACY_CODE,{0,0,0,0}}
#endif
#if SETUP_OEM_KEY4_ENABLE
    ,{SETUP_OEM_KEY4_LEGACY_CODE,{0,0,0,0}}
#endif
};

UINT32 gLegacyKeys;
EFI_HANDLE KeyMonProtocolHandle;

extern EFI_BOOT_SERVICES *gBS;

VOID InstallKeyMonProtocol(VOID)
{
    gBS->InstallProtocolInterface(
                        &KeyMonProtocolHandle,
                        &EfiKeyMonPlatformProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        &gKeyMonPlatformProtocol
                        );
}

/**
    This function registers keys with key monitor driver
    for monitoring in the legacy mode.

    @param This 
    @param KeyList : List of keys to be monitored.
    @param KeyListSize : Size of data returned in KeyList.

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS
GetPlatformMonitorKeyOptions(
  IN   EFI_KEYMON_PLATFORM_PROTOCOL   *This,
  OUT  KEY_ELEMENT                    **KeyList,
  OUT  UINTN                          *KeyListSize
  )
{
    *KeyList = gLegacyKeyTable;
    *KeyListSize = sizeof(gLegacyKeyTable);

    return EFI_SUCCESS;
}

/**
    This function receives keys from key monitor filter

    @param VOID

    @retval VOID

**/
VOID GetKeysFromKeyMonFilter(VOID)
{
    UINT32  KeyDetected;
	EFI_MONITOR_KEY_FILTER_PROTOCOL *MonKey;
    EFI_STATUS  Status;
    EFI_GUID EfiMonitorKeyFilterProtocolGuid = EFI_MONITOR_KEY_FILTER_PROTOCOL_GUID;

    // Locate protocols
	Status = gBS->LocateProtocol (
					&EfiMonitorKeyFilterProtocolGuid,
					NULL, &MonKey);
    if ( !(EFI_ERROR(Status)) )
    {
        MonKey->GetMonitoredKeys(MonKey, &KeyDetected);

        if(KeyDetected & (1 << SK_EntryKey))
        {
            gLegacyKeys|=SETUP_ENTRY_KEY_BIT;
        }
#if SETUP_ALT_ENTRY_ENABLE
        if(KeyDetected & (1 << SK_AltEntryKey))
        {
            gLegacyKeys|=SETUP_ENTRY_KEY_BIT;
        }
#endif
#if SETUP_BBS_POPUP_ENABLE
        if(KeyDetected & (1 << SK_PopupBootEntryKey))
        {
            gLegacyKeys|=POPUP_ENTRY_KEY_BIT;
        }
#endif
#if SETUP_OEM_KEY1_ENABLE
        if(KeyDetected & (1 << SK_OemKey1))
        {
            gLegacyKeys|=OEM_KEY1_BIT;
        }
#endif
#if SETUP_OEM_KEY2_ENABLE
        if(KeyDetected & (1 << SK_OemKey2))
        {
            gLegacyKeys|=OEM_KEY2_BIT;
        }
#endif
#if SETUP_OEM_KEY3_ENABLE
        if(KeyDetected & (1 << SK_OemKey3))
        {
            gLegacyKeys|=OEM_KEY3_BIT;
        }
#endif
#if SETUP_OEM_KEY4_ENABLE
        if(KeyDetected & (1 << SK_OemKey4))
        {
            gLegacyKeys|=OEM_KEY4_BIT;
        }
#endif

    }
}

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
