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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Keymon.h $
//
// $Author: Madhans $
//
// $Revision: 3 $
//
// $Date: 2/19/10 12:58p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file Keymon.h
    header of keymon.c

**/
#if SETUP_SUPPORT_KEY_MONITORING

#define SETUP_ENTRY_KEY_BIT 0x00000001
#if SETUP_BBS_POPUP_ENABLE
#define POPUP_ENTRY_KEY_BIT 0x00000002
#endif
#if SETUP_OEM_KEY1_ENABLE
#define OEM_KEY1_BIT 0x00000004
#endif
#if SETUP_OEM_KEY2_ENABLE
#define OEM_KEY2_BIT 0x00000008
#endif
#if SETUP_OEM_KEY3_ENABLE
#define OEM_KEY3_BIT 0x00000100
#endif
#if SETUP_OEM_KEY4_ENABLE
#define OEM_KEY4_BIT 0x00000200
#endif

VOID InstallKeyMonProtocol(VOID);
VOID GetKeysFromKeyMonFilter(VOID);

extern UINT32 gLegacyKeys;

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
