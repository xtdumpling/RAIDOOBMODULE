//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/bbs.h $
//
// $Author: Mallikarjunanv $
//
// $Revision: 5 $
//
// $Date: 12/21/10 11:46a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**********************************************************************
/** @file bbs.h
    Header file for bbs/legacy boot management.

**/
//**********************************************************************

#ifndef _BBS_H_
#define _BBS_H_

UINT16	gBBSHardDiskOffset ;
UINT16	gBBSCDOffset ;
UINT16	gBBSFloppyOffset ;
UINT16	gBBSNetworkOffset ;
UINT16	gBBSBevOffset ;

#ifndef MAX_DRIVE_NAME
#define MAX_DRIVE_NAME TSE_MAX_DRIVE_NAME 
#endif //MAX_DRIVE_NAME

typedef struct
{
	UINT16	HardDiskCount;
	UINT16	CDROMCount;
	UINT16	FloppyCount;
	UINT16	NetworkCount;
	UINT16	BevCount;
}
BBS_DEVICE_COUNT;

typedef struct
{
	UINT32	Type;
	UINT16	Length;
	UINT16	Data[1];
}
BBS_ORDER_TABLE;

#define BBS_ORDER_DISABLE_MASK  0xFF00

typedef struct _BBS_ORDER_LIST
{
    UINT16	Index;
    CHAR16	Name[MAX_DRIVE_NAME];
}
BBS_ORDER_LIST;

//exported variables
extern EFI_GUID gLegacyDevGuid;

//exported functions
VOID BBSGetDeviceList( VOID );
VOID BBSSetBootPriorities( BOOT_DATA *pBootData, UINT16 *pOrder, UINTN u16OrderCount);
EFI_STATUS BBSSetBootNowPriority( BOOT_DATA *BootData,UINTN uiPrefferedDevice,BOOLEAN ShowAllBbsDev);
BOOLEAN BBSValidDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath );
EFI_STATUS BBSLaunchDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath );

VOID BBSUpdateBootData(UINT16 GroupNo);
CHAR16 *BBSGetOptionName( UINT16 *Option, UINT16 Pos  );
//VOID BBSUpdateOrder(UINT16 newOption,UINT32 *offset,UINTN *size, VOID **buffer);
VOID BBSSetDisabled(UINT16 Index, VOID **DisDPs, UINTN *DPSize);

EFI_STATUS BbsItkBoot();

#endif /* _BBS_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
