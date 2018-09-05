//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file DefaultFixedBootOrder.c

    Default FixedBootOrder Boot type setting.
*/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <efi.h>
#include <amidxelib.h>
#include <BootOptions.h>
#include <protocol/PciIo.h>
#if CSM_SUPPORT
#include <Protocol/LegacyBios.h>
#endif
#include <protocol/PciRootBridgeIo.h>
#include <protocol/DiskInfo.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include "../FixedBootOrder.h"
#include "DefaultFixedBootOrder.h"

//========== DEVICE MAP DATA ENTRIES ==========
//
//  UEFI DEVICE
//
#define UEFI_DEVICE {BoTagUefi, 0, 0xff, 0},
//
//  HDD
//
#if !FBO_MULTI_HARD_DISK_GROUPS
#define UEFI_HDD    {BoTagUefiHardDisk,  0, FBO_USELESS_PORT_NUMBER, F_HDD},
#else
#define UEFI_HDD    {BoTagUefiHardDisk,  0, 0x00, F_HDD},
#endif
#define UEFI_HDD1   {BoTagUefiHardDisk1, 0, 0x01, F_HDD},
#define UEFI_HDD2   {BoTagUefiHardDisk2, 0, 0x02, F_HDD},
#define UEFI_HDD3   {BoTagUefiHardDisk3, 0, 0x03, F_HDD},
#define UEFI_HDD4   {BoTagUefiHardDisk4, 0, 0x04, F_HDD},
#define UEFI_HDD5   {BoTagUefiHardDisk5, 0, 0x05, F_HDD},
//
//  UEFI  APPLICATION
//
#define UEFI_APPLICATION {BoTagUefiApplication, 0, FBO_USELESS_PORT_NUMBER, 0},
//
//  UEFI ODD
//
#define UEFI_ODD     {BoTagUefiCdrom,       0, FBO_USELESS_PORT_NUMBER, 0},
//
//  UEFI USB
//
#define UEFI_USB     {BoTagUefiUsb,         0, FBO_USELESS_PORT_NUMBER, 0},
#define UEFI_USBHDD  {BoTagUefiUsbHardDisk, 0, FBO_USELESS_PORT_NUMBER, 0},
#define UEFI_USBODD  {BoTagUefiUsbCdrom,    0, FBO_USELESS_PORT_NUMBER, 0},
#define UEFI_USBKEY  {BoTagUefiUsbKey,      0, FBO_USELESS_PORT_NUMBER, 0},
#define UEFI_USBFDD  {BoTagUefiUsbFloppy,   0, FBO_USELESS_PORT_NUMBER, 0},
#define UEFI_USBLAN  {BoTagUefiUsbLan,      0, FBO_USELESS_PORT_NUMBER, 0},
//
//  UEFI NETWORK
//
#define UEFI_NETWORK {BoTagUefiNetWork,     0, FBO_USELESS_PORT_NUMBER, 0},

//
//  LEGACY HDD
//
#if !FBO_MULTI_HARD_DISK_GROUPS
#define HDD     {BoTagLegacyHardDisk,     BBS_HARDDISK,      FBO_USELESS_PORT_NUMBER, 0},
#else
#define HDD     {BoTagLegacyHardDisk,     BBS_HARDDISK,      0x00, 0},
#endif
#define HDD1    {BoTagLegacyHardDisk1,    BBS_HARDDISK,      0x01, 0},
#define HDD2    {BoTagLegacyHardDisk2,    BBS_HARDDISK,      0x02, 0},
#define HDD3    {BoTagLegacyHardDisk3,    BBS_HARDDISK,      0x03, 0},
#define HDD4    {BoTagLegacyHardDisk4,    BBS_HARDDISK,      0x04, 0},
#define HDD5    {BoTagLegacyHardDisk5,    BBS_HARDDISK,      0x05, 0},
//
//  LEGACY ODD
//
#define ODD       {BoTagLegacyCdrom,        BBS_CDROM,         FBO_USELESS_PORT_NUMBER, 0},
//
//  LEGACY USB
//
#define USB       {BoTagLegacyUsb,          BBS_USB,           FBO_USELESS_PORT_NUMBER, F_USB | F_USBKEY},
#define USBHDD    {BoTagLegacyUSBHardDisk,  BBS_HARDDISK,      FBO_USELESS_PORT_NUMBER, F_USB},
#define USBODD    {BoTagLegacyUSBCdrom,     BBS_CDROM,         FBO_USELESS_PORT_NUMBER, F_USB},
#define USBKEY    {BoTagLegacyUSBKey,       BBS_HARDDISK,      FBO_USELESS_PORT_NUMBER, F_USB | F_USBKEY},
#define USBFDD    {BoTagLegacyUSBFloppy,    BBS_FLOPPY,        FBO_USELESS_PORT_NUMBER, F_USB},
#define USBLAN    {BoTagLegacyUSBLan,       BBS_EMBED_NETWORK, FBO_USELESS_PORT_NUMBER, F_USB},
//
//  LEGACY NETWORK
//
#define NETWORK   {BoTagLegacyEmbedNetwork, BBS_EMBED_NETWORK, FBO_USELESS_PORT_NUMBER, 0},

//---------------------------------------------------------------------------
// Global Variable declarations
//---------------------------------------------------------------------------
UINT8 gBdsSkipFBOModule = 0 ;
//---------------------------------------------------------------------------
// External functions
//---------------------------------------------------------------------------
EFI_HANDLE GetPhysicalBlockIoHandle(EFI_HANDLE BlockIoHandle);
BOOLEAN IsUSBkeyByHandle(EFI_HANDLE Handle);
BOOLEAN IsShellBootOption(BOOT_OPTION *Option);
UINT32 FBO_FindTagPriority(UINT16 Tag);
UINT16 GetDevMapDataCount(FBODevMap *pFBODevMap);
extern VOID SetBootOptionTags() ;
//---------------------------------------------------------------------------
// External Variables
//---------------------------------------------------------------------------
extern EFI_GUID gFixedBootOrderGuid;
//---------------------------------------------------------------------------
// Global Variable declarations
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Function Definitions
//---------------------------------------------------------------------------
UINT32 GetUefiBootOptionTag(IN BOOT_OPTION *Option) ;
EFI_STATUS SetDevNewDescription(EFI_DEVICE_PATH_PROTOCOL *DpHead, CHAR16 *Description) ;
BOOLEAN IsUsbDp(IN EFI_DEVICE_PATH_PROTOCOL *Dp, OUT UINT32 *AvailableNodes OPTIONAL);
//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
typedef VOID (FBO_CHANGE_DEVICE_NAME)(FBO_DEVICE_INFORM *Content);
extern FBO_CHANGE_DEVICE_NAME FBO_CHANGE_DEVICE_NAME_FUNCS EndOfFboChangeDeviceName ;
FBO_CHANGE_DEVICE_NAME *FboChangeDeviceNameFuncs[] = { FBO_CHANGE_DEVICE_NAME_FUNCS NULL } ;

FIXED_BOOT_OPTION_TAG FixedLegacyBootOptionTags[] = {
    0,
    BoTagLegacyFloppy,
    BoTagLegacyHardDisk,
    BoTagLegacyCdrom,
    BoTagLegacyPcmcia,
    BoTagLegacyUsb,
    BoTagLegacyEmbedNetwork,
    BoTagLegacyBevDevice,
    BoTagLegacyUSBFloppy,
    BoTagLegacyUSBHardDisk,
    BoTagLegacyUSBCdrom,
    BoTagLegacyUSBKey,
    BoTagLegacyUSBLan,
    BoTagLegacyHardDisk1,
    BoTagLegacyHardDisk2,
    BoTagLegacyHardDisk3,
    BoTagLegacyHardDisk4,
    BoTagLegacyHardDisk5,
    UNASSIGNED_HIGHEST_TAG
};

FBODevMap FBOUefiDevMapData[]={
    //
    // Refer to Build/DefaultFixedBootOrder.h
    // and Device Map Data Entries
    //
    FBO_UEFI_DEVICE
    {  0, 0 }	//end of data
};

#if CSM_SUPPORT
FBODevMap FBOLegacyDevMapData[]={
    //
    // Refer to Build/DefaultFixedBootOrder.h
    // and Device Map Data Entries
    //
    FBO_LEGACY_DEVICE
    {  0, 0 }	//end of data
};

FBODevMap FBODualDevMapData[]={
    //
    // Refer to Build/DefaultFixedBootOrder.h
    // and Device Map Data Entries
    //
    FBO_DUAL_DEVICE
    {  0, 0 }	//end of data
};
#endif

#if (FBO_DUAL_MODE == 1) && (CSM_SUPPORT == 1)
FBOHiiMap FBOHiiMapData[]={
    {  BoTagLegacyHardDisk,     STR_BOOT_DUAL_HDD,               HDD_BOOT_FORM_ID,         HDD_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD)},
#if FBO_MULTI_HARD_DISK_GROUPS
    {  BoTagLegacyHardDisk1,    STR_BOOT_DUAL_HDD1,              HDD1_BOOT_FORM_ID,        HDD1_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD1)},
    {  BoTagLegacyHardDisk2,    STR_BOOT_DUAL_HDD2,              HDD2_BOOT_FORM_ID,        HDD2_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD2)},
    {  BoTagLegacyHardDisk3,    STR_BOOT_DUAL_HDD3,              HDD3_BOOT_FORM_ID,        HDD3_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD3)},
    {  BoTagLegacyHardDisk4,    STR_BOOT_DUAL_HDD4,              HDD4_BOOT_FORM_ID,        HDD4_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD4)},
    {  BoTagLegacyHardDisk5,    STR_BOOT_DUAL_HDD5,              HDD5_BOOT_FORM_ID,        HDD5_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD5)},
#endif
    {  BoTagLegacyCdrom,        STR_BOOT_DUAL_ODD,               ODD_BOOT_FORM_ID,         ODD_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyODD)},
    {  BoTagLegacyEmbedNetwork, STR_BOOT_DUAL_NETWORK,           NETWORK_BOOT_FORM_ID,     NETWORK_BOOT_FORM_LABEL,     EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyNETWORK)},
#if USB_GROUP == 2
    {  BoTagLegacyUSBFloppy,    STR_BOOT_DUAL_USBFDD,            USBFDD_BOOT_FORM_ID,      USBFDD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBFDD)},
    {  BoTagLegacyUSBHardDisk,  STR_BOOT_DUAL_USBHDD,            USBHDD_BOOT_FORM_ID,      USBHDD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBHDD)},
    {  BoTagLegacyUSBCdrom,     STR_BOOT_DUAL_USBODD,            USBODD_BOOT_FORM_ID,      USBODD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBODD)},
    {  BoTagLegacyUSBKey,       STR_BOOT_DUAL_USBKEY,            USBKEY_BOOT_FORM_ID,      USBKEY_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBKEY)},
    {  BoTagLegacyUSBLan,       STR_BOOT_DUAL_USBLAN,            USBLAN_BOOT_FORM_ID,      USBLAN_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBLAN)},
#endif
#if USB_GROUP == 1
    {  BoTagLegacyUsb,          STR_BOOT_DUAL_USB,               USB_BOOT_FORM_ID,         USB_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSB)},
#endif
#if EFI_DEVICE_IN_ONE_GROUP
	{  BoTagUefi,               STR_BOOT_DUAL_UEFI_DEVICE,       UEFI_DEVICE_BOOT_FORM_ID, UEFI_DEVICE_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiDEVICE)},
#else
    {  BoTagUefiHardDisk,	    STR_BOOT_DUAL_UEFI_HDD,     UEFI_HDD_BOOT_FORM_ID,    UEFI_HDD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD)},
#if FBO_MULTI_HARD_DISK_GROUPS
    {  BoTagUefiHardDisk1,      STR_BOOT_DUAL_UEFI_HDD1,    UEFI_HDD1_BOOT_FORM_ID,   UEFI_HDD1_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD1)},
    {  BoTagUefiHardDisk2,      STR_BOOT_DUAL_UEFI_HDD2,    UEFI_HDD2_BOOT_FORM_ID,   UEFI_HDD2_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD2)},
    {  BoTagUefiHardDisk3,      STR_BOOT_DUAL_UEFI_HDD3,    UEFI_HDD3_BOOT_FORM_ID,   UEFI_HDD3_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD3)},
    {  BoTagUefiHardDisk4,      STR_BOOT_DUAL_UEFI_HDD4,    UEFI_HDD4_BOOT_FORM_ID,   UEFI_HDD4_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD4)},
    {  BoTagUefiHardDisk5,      STR_BOOT_DUAL_UEFI_HDD5,    UEFI_HDD5_BOOT_FORM_ID,   UEFI_HDD5_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD5)},
#endif
    {  BoTagUefiCdrom,          STR_BOOT_DUAL_UEFI_ODD,     UEFI_ODD_BOOT_FORM_ID,    UEFI_ODD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiODD)},
    {  BoTagUefiNetWork,        STR_BOOT_DUAL_UEFI_NETWORK, UEFI_NETWORK_BOOT_FORM_ID, UEFI_NETWORK_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiNETWORK)},
#if USB_GROUP == 2
    {  BoTagUefiUsbHardDisk,    STR_BOOT_DUAL_UEFI_USBHDD,  UEFI_USBHDD_BOOT_FORM_ID, UEFI_USBHDD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBHDD)},
    {  BoTagUefiUsbCdrom,	    STR_BOOT_DUAL_UEFI_USBODD,  UEFI_USBODD_BOOT_FORM_ID, UEFI_USBODD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBODD)},
    {  BoTagUefiUsbKey,	    	STR_BOOT_DUAL_UEFI_USBKEY,  UEFI_USBKEY_BOOT_FORM_ID, UEFI_USBKEY_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBKEY)},
    {  BoTagUefiUsbFloppy,	    STR_BOOT_DUAL_UEFI_USBFDD,  UEFI_USBFDD_BOOT_FORM_ID, UEFI_USBFDD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBFDD)},		//(EIP96232+)
    {  BoTagUefiUsbLan,	        STR_BOOT_DUAL_UEFI_USBLAN,  UEFI_USBLAN_BOOT_FORM_ID, UEFI_USBLAN_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBLAN)},
#endif
#if USB_GROUP == 1
    {  BoTagUefiUsb,            STR_BOOT_DUAL_UEFI_USB,     UEFI_USB_BOOT_FORM_ID,    UEFI_USB_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSB)},
#endif
#if SHELL_GROUP_SUPPORT
    {  BoTagUefiApplication,    STR_BOOT_DUAL_UEFI_APPLICATION,  UEFI_APPLICATION_BOOT_FORM_ID, UEFI_APPLICATION_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiAPPLICATION)},	//(EIP115686+)
#endif
#endif
    {  0, 0 }	//end of data
};
#else
FBOHiiMap FBOHiiMapData[]={
    {  BoTagLegacyHardDisk,     STR_BOOT_HDD,          HDD_BOOT_FORM_ID,         HDD_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD)},
#if FBO_MULTI_HARD_DISK_GROUPS
    {  BoTagLegacyHardDisk1,    STR_BOOT_HDD1,         HDD1_BOOT_FORM_ID,        HDD1_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD1)},
    {  BoTagLegacyHardDisk2,    STR_BOOT_HDD2,         HDD2_BOOT_FORM_ID,        HDD2_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD2)},
    {  BoTagLegacyHardDisk3,    STR_BOOT_HDD3,         HDD3_BOOT_FORM_ID,        HDD3_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD3)},
    {  BoTagLegacyHardDisk4,    STR_BOOT_HDD4,         HDD4_BOOT_FORM_ID,        HDD4_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD4)},
    {  BoTagLegacyHardDisk5,    STR_BOOT_HDD5,         HDD5_BOOT_FORM_ID,        HDD5_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD5)},
#endif
    {  BoTagLegacyCdrom,        STR_BOOT_ODD,          ODD_BOOT_FORM_ID,         ODD_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyODD)},
    {  BoTagLegacyEmbedNetwork, STR_BOOT_NETWORK,      NETWORK_BOOT_FORM_ID,     NETWORK_BOOT_FORM_LABEL,     EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyNETWORK)},
#if USB_GROUP == 2
    {  BoTagLegacyUSBFloppy,    STR_BOOT_USBFDD,       USBFDD_BOOT_FORM_ID,      USBFDD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBFDD)},
    {  BoTagLegacyUSBHardDisk,  STR_BOOT_USBHDD,       USBHDD_BOOT_FORM_ID,      USBHDD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBHDD)},
    {  BoTagLegacyUSBCdrom,     STR_BOOT_USBODD,       USBODD_BOOT_FORM_ID,      USBODD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBODD)},
    {  BoTagLegacyUSBKey,       STR_BOOT_USBKEY,       USBKEY_BOOT_FORM_ID,      USBKEY_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBKEY)},
    {  BoTagLegacyUSBLan,       STR_BOOT_USBLAN,       USBLAN_BOOT_FORM_ID,      USBLAN_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBLAN)},
#endif
#if USB_GROUP == 1
    {  BoTagLegacyUsb,          STR_BOOT_USB,          USB_BOOT_FORM_ID,         USB_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSB)},
#endif
#if EFI_DEVICE_IN_ONE_GROUP
	{  BoTagUefi,               STR_BOOT_UEFI_DEVICE,  UEFI_DEVICE_BOOT_FORM_ID, UEFI_DEVICE_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiDEVICE)},
#else
    {  BoTagUefiHardDisk,	    STR_BOOT_UEFI_HDD,     UEFI_HDD_BOOT_FORM_ID,    UEFI_HDD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD)},
#if FBO_MULTI_HARD_DISK_GROUPS
    {  BoTagUefiHardDisk1,      STR_BOOT_UEFI_HDD1,    UEFI_HDD1_BOOT_FORM_ID,   UEFI_HDD1_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD1)},
    {  BoTagUefiHardDisk2,      STR_BOOT_UEFI_HDD2,    UEFI_HDD2_BOOT_FORM_ID,   UEFI_HDD2_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD2)},
    {  BoTagUefiHardDisk3,      STR_BOOT_UEFI_HDD3,    UEFI_HDD3_BOOT_FORM_ID,   UEFI_HDD3_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD3)},
    {  BoTagUefiHardDisk4,      STR_BOOT_UEFI_HDD4,    UEFI_HDD4_BOOT_FORM_ID,   UEFI_HDD4_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD4)},
    {  BoTagUefiHardDisk5,      STR_BOOT_UEFI_HDD5,    UEFI_HDD5_BOOT_FORM_ID,   UEFI_HDD5_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD5)},
#endif
    {  BoTagUefiCdrom,          STR_BOOT_UEFI_ODD,     UEFI_ODD_BOOT_FORM_ID,    UEFI_ODD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiODD)},
    {  BoTagUefiNetWork,        STR_BOOT_UEFI_NETWORK, UEFI_NETWORK_BOOT_FORM_ID, UEFI_NETWORK_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiNETWORK)},
#if USB_GROUP == 2
    {  BoTagUefiUsbHardDisk,    STR_BOOT_UEFI_USBHDD,  UEFI_USBHDD_BOOT_FORM_ID, UEFI_USBHDD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBHDD)},
    {  BoTagUefiUsbCdrom,	    STR_BOOT_UEFI_USBODD,  UEFI_USBODD_BOOT_FORM_ID, UEFI_USBODD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBODD)},
    {  BoTagUefiUsbKey,	    	STR_BOOT_UEFI_USBKEY,  UEFI_USBKEY_BOOT_FORM_ID, UEFI_USBKEY_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBKEY)},
    {  BoTagUefiUsbFloppy,	    STR_BOOT_UEFI_USBFDD,  UEFI_USBFDD_BOOT_FORM_ID, UEFI_USBFDD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBFDD)},		//(EIP96232+)
    {  BoTagUefiUsbLan,	        STR_BOOT_UEFI_USBLAN,  UEFI_USBLAN_BOOT_FORM_ID, UEFI_USBLAN_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBLAN)},
#endif
#if USB_GROUP == 1
    {  BoTagUefiUsb,            STR_BOOT_UEFI_USB,     UEFI_USB_BOOT_FORM_ID,    UEFI_USB_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSB)},
#endif
#if SHELL_GROUP_SUPPORT
    {  BoTagUefiApplication,    STR_BOOT_UEFI_APPLICATION,  UEFI_APPLICATION_BOOT_FORM_ID, UEFI_APPLICATION_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiAPPLICATION)},	//(EIP115686+)
#endif
#endif
    {  0, 0 }	//end of data
};
#endif
//(EIP123284+)>
UINT16 BootOptionStringToken[]={
    STR_LEGACY_BOOT_ORDER_1,
    STR_LEGACY_BOOT_ORDER_2,
    STR_LEGACY_BOOT_ORDER_3,
    STR_LEGACY_BOOT_ORDER_4,
    STR_LEGACY_BOOT_ORDER_5,
    STR_LEGACY_BOOT_ORDER_6,
    STR_LEGACY_BOOT_ORDER_7,
    STR_LEGACY_BOOT_ORDER_8,
    STR_LEGACY_BOOT_ORDER_9,
    STR_LEGACY_BOOT_ORDER_10,
    STR_LEGACY_BOOT_ORDER_11,
    STR_LEGACY_BOOT_ORDER_12,
    STR_LEGACY_BOOT_ORDER_13,
    STR_LEGACY_BOOT_ORDER_14,
    STR_LEGACY_BOOT_ORDER_15,
    STR_LEGACY_BOOT_ORDER_16,
    STR_LEGACY_BOOT_ORDER_17,
    STR_LEGACY_BOOT_ORDER_18,
    STR_UEFI_BOOT_ORDER_1,
    STR_UEFI_BOOT_ORDER_2,
    STR_UEFI_BOOT_ORDER_3,
    STR_UEFI_BOOT_ORDER_4,
    STR_UEFI_BOOT_ORDER_5,
    STR_UEFI_BOOT_ORDER_6,
    STR_UEFI_BOOT_ORDER_7,
    STR_UEFI_BOOT_ORDER_8,
    STR_UEFI_BOOT_ORDER_9,
    STR_UEFI_BOOT_ORDER_10,
    STR_UEFI_BOOT_ORDER_11,
    STR_UEFI_BOOT_ORDER_12,
    STR_UEFI_BOOT_ORDER_13,
    STR_UEFI_BOOT_ORDER_14,
    STR_UEFI_BOOT_ORDER_15,
    STR_UEFI_BOOT_ORDER_16,
    STR_UEFI_BOOT_ORDER_17,
    STR_UEFI_BOOT_ORDER_18
#if FBO_DUAL_MODE
    ,
    STR_DUAL_BOOT_ORDER_1,
    STR_DUAL_BOOT_ORDER_2,
    STR_DUAL_BOOT_ORDER_3,
    STR_DUAL_BOOT_ORDER_4,
    STR_DUAL_BOOT_ORDER_5,
    STR_DUAL_BOOT_ORDER_6,
    STR_DUAL_BOOT_ORDER_7,
    STR_DUAL_BOOT_ORDER_8,
    STR_DUAL_BOOT_ORDER_9,
    STR_DUAL_BOOT_ORDER_10,
    STR_DUAL_BOOT_ORDER_11,
    STR_DUAL_BOOT_ORDER_12,
    STR_DUAL_BOOT_ORDER_13,
    STR_DUAL_BOOT_ORDER_14,
    STR_DUAL_BOOT_ORDER_15,
    STR_DUAL_BOOT_ORDER_16,
    STR_DUAL_BOOT_ORDER_17,
    STR_DUAL_BOOT_ORDER_18,
    STR_DUAL_BOOT_ORDER_19,
    STR_DUAL_BOOT_ORDER_20,
    STR_DUAL_BOOT_ORDER_21,
    STR_DUAL_BOOT_ORDER_22,
    STR_DUAL_BOOT_ORDER_23,
    STR_DUAL_BOOT_ORDER_24,
    STR_DUAL_BOOT_ORDER_25,
    STR_DUAL_BOOT_ORDER_26,
    STR_DUAL_BOOT_ORDER_27,
    STR_DUAL_BOOT_ORDER_28,
    STR_DUAL_BOOT_ORDER_29,
    STR_DUAL_BOOT_ORDER_30,
    STR_DUAL_BOOT_ORDER_31,
    STR_DUAL_BOOT_ORDER_32,
    STR_DUAL_BOOT_ORDER_33,
    STR_DUAL_BOOT_ORDER_34,
    STR_DUAL_BOOT_ORDER_35,
    STR_DUAL_BOOT_ORDER_36
#endif
};
//<(EIP123284+)

/**
    Set Bds Skip FBO Module Flag, 

    @param VOID

    @retval VOID

    @note  None

**/
VOID 
BdsSetSkipFBOModuleFlag()
{
    EFI_STATUS Status;
    UINTN  Size = 0;
    UINT32 Attr;
    UINT8 *Flag = NULL ;
    EFI_GUID FixedBootOrderGuid=FIXED_BOOT_ORDER_GUID;
    Status = GetEfiVariable(L"CurrentSkipFboModule", &FixedBootOrderGuid, &Attr, &Size, &Flag);
    if (!EFI_ERROR(Status))
    {
        if (*Flag) gBdsSkipFBOModule = 1 ;
        else gBdsSkipFBOModule = 0 ;
        pBS->FreePool(Flag) ;
    }
    else
        gBdsSkipFBOModule = 0 ;
}

/**
    Initialize the group map 

    @param VOID

    @retval VOID

    @retval EFI_STATUS Status of this Procedure

**/
    
EFI_STATUS DefaultFixedBootOrder_Init(IN void)
{
    EFI_STATUS Status;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *pFBO=NULL;

    BdsSetSkipFBOModuleFlag() ;
    if (gBdsSkipFBOModule) return EFI_SUCCESS ;
    
    Status = pBS->LocateProtocol( &gFixedBootOrderGuid, NULL, &pFBO );
    if ( !EFI_ERROR(Status) )
    {
        pFBO->SetUefiDevMap( FBOUefiDevMapData );
#if CSM_SUPPORT
        pFBO->SetLegacyDevMap( FBOLegacyDevMapData );
        pFBO->SetDualDevMap( FBODualDevMapData );
#endif
        pFBO->SetHiiMap( FBOHiiMapData );
        pFBO->SetBootOptionTokenMap( BootOptionStringToken );  //(EIP123284+)
    }
    return Status;
}

//reference CRB SbSetup.c

UINT16 gSATA[3][2] = {
    { 0, 1 },
    { 2, 3 },
    { 4, 5 }
};

//*************************************************************************
/**
    Use handle and EFI_DISK_INFO_PROTOCOL to find sata port.

    @param 
        IN EFI_HANDLE

    @retval 
        None.

**/
//*************************************************************************
UINT16 GetHddPort( IN EFI_HANDLE Handle )
{
    EFI_STATUS Status;
    EFI_GUID gEfiDiskInfoProtocolGuid = EFI_DISK_INFO_PROTOCOL_GUID;
    EFI_DISK_INFO_PROTOCOL *DiskInfo;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath;
    UINT32 IdeChannel;
    UINT32 IdeDevice;

    Status = pBS->HandleProtocol (
                 Handle,
                 &gEfiDevicePathProtocolGuid,
                 (VOID *) &DevicePath );

    if ( !EFI_ERROR( Status ))
    {
        EFI_DEVICE_PATH_PROTOCOL *DevicePathNode;
        EFI_DEVICE_PATH_PROTOCOL *MessagingDevicePath;
        PCI_DEVICE_PATH *PciDevicePath;

        DevicePathNode = DevicePath;
        while (!isEndNode (DevicePathNode))
        {
            if ((DevicePathNode->Type == HARDWARE_DEVICE_PATH)
                    && (DevicePathNode->SubType == HW_PCI_DP))
                PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
            else if (DevicePathNode->Type == MESSAGING_DEVICE_PATH)
                MessagingDevicePath = DevicePathNode;

            DevicePathNode = NEXT_NODE (DevicePathNode);
        }

        Status = pBS->HandleProtocol ( Handle, &gEfiDiskInfoProtocolGuid, &DiskInfo );
        if ( !EFI_ERROR(Status) )
        {
            Status = DiskInfo->WhichIde ( DiskInfo, &IdeChannel, &IdeDevice );
            if ( !EFI_ERROR(Status) )
            {
                if ( MessagingDevicePath->SubType == MSG_ATAPI_DP ) //IDE mode?
                {
                    if (PciDevicePath->Function == 5)
                        return gSATA[IdeDevice+2][IdeChannel];
                    else
                        return gSATA[IdeDevice][IdeChannel];
                }
                else
                    return IdeChannel;	//AHCI Port Number
            }
        }
    }
    return FBO_USELESS_PORT_NUMBER;
}

#if CSM_SUPPORT
/**
    Get the device port number according to bbs table

    @param BBS_TABLE *BbsTable - bbs table

    @retval UINT16 Port number

**/
UINT16 GetDevicePortNumber(BBS_TABLE *BbsTable)
{
    EFI_HANDLE Handle = *(VOID**)(&BbsTable->IBV1);

    if ( BbsTable->DeviceType == BBS_HARDDISK )
        return GetHddPort(Handle);

    return FBO_USELESS_PORT_NUMBER;
}
#endif	//#if CSM_SUPPORT

/**
    Change Boot Option Group Name, OEM have two way to modify Group Name in run time, such as USB Key - > OEM USB Key
    1.Provide StrToken(Define in .uni), New Group Name.
    FboChangeGroupName( STR_BOOT_HDD, NULL, NewGroupName) ;
    2.Provide Original Group Name(ex:Hard Disk), New Group Name.
    FboChangeGroupName( 0, OrgGroupName, NewGroupName) ;  
          
    @note  EIP 143657

**/
EFI_STATUS
FboChangeGroupName(
    IN OPTIONAL UINT16 StrToken,
    IN OPTIONAL CHAR16* OrgGroupName, 
    IN CHAR16* NewGroupName )
{
    EFI_STATUS Status ;
    UINT32 Attr ;
    UINTN Size = 0 ,*GroupPtrAddr = NULL ;
    FBO_GROUP_OPTION *Group = NULL ;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID ;
    
    TRACE((-1,"DefaultFixedBootOrder.c FboChangeGroupName\n")) ;
    if (StrToken == 0 && OrgGroupName == NULL )
        return EFI_INVALID_PARAMETER ;
    
    Status = GetEfiVariable(L"FboGroupNameData", &FixedBootOrderGuid, &Attr, &Size, &GroupPtrAddr);
    if(EFI_ERROR(Status))
    {
        Group = MallocZ(sizeof(FBO_GROUP_OPTION)) ;
        // Firsrt time, Restore the group pointer address to variable.
        Status = pRS->SetVariable ( L"FboGroupNameData",
                                    &FixedBootOrderGuid,
                                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                    sizeof(UINTN),
                                    &Group );
    }
    else
    {
        // Go To the last node.
        Group = (FBO_GROUP_OPTION*)*GroupPtrAddr ;
        while (Group->Next)
            Group = Group->Next ;
        // Create the new node.
        Group->Next = MallocZ(sizeof(FBO_GROUP_OPTION)) ;
        Group = Group->Next ;
    }

    Group->NewGroupName = MallocZ( StrLen(NewGroupName)*sizeof(CHAR16)+1) ;
    StrCpy( Group->NewGroupName, NewGroupName) ;
    if (StrToken) Group->StrToken = StrToken ;
    else
    {
        Group->OrgGroupName = MallocZ( StrLen(OrgGroupName)*sizeof(CHAR16)+1) ;
        StrCpy( Group->OrgGroupName, OrgGroupName) ;
    }
    pBS->FreePool(GroupPtrAddr) ; // Avoid memory leak
    return EFI_SUCCESS ;
}

/**
    Get Device Information(Description, Type, Path) by BootOptionList

    @param FBO_DEVICE_INFORM **DevHead

    @retval FBO_DEVICE_INFORM **DevHead
 
    @note  EIP 143657

**/
VOID
GetDevInformByBootOptionList(
    FBO_DEVICE_INFORM **DevHead)
{
    FBO_DEVICE_INFORM *DevInform = NULL;
    EFI_DEVICE_PATH_PROTOCOL *Dp = NULL ;
    DLINK *Link;
    BOOT_OPTION *Option;
    
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        BOOLEAN UsbPortFlag = FALSE ;
        if (Option->GroupHeader) continue ;
        if (!DevInform)
            *DevHead = DevInform = MallocZ(sizeof(FBO_DEVICE_INFORM)) ;
        else
        {
            DevInform->Next = MallocZ(sizeof(FBO_DEVICE_INFORM)) ;
            DevInform = DevInform->Next ;
        }
        
        DevInform->DevName = Option->Description ;
        // Set Default value.
        DevInform->DevPortNum = 0xff ;
        DevInform->DevBDF = 0xffffffff ; 
        
#if CSM_SUPPORT
        if (IsLegacyBootOption(Option))
        {
            EFI_STATUS Status;
            
            // Legacy Option need use Handle Protocol to get correct Device Path.
            Status = pBS->HandleProtocol((EFI_HANDLE)Option->BbsEntry->IBV1, &gEfiDevicePathProtocolGuid, &Dp);
            if (EFI_ERROR(Status)) continue ;
            DevInform->DevPath = Dp ;
            DevInform->DevType = (UINT16)(0xff&Option->Tag) ;
            DevInform->DevBDF = (UINT32) (Option->BbsEntry->Bus << 16)|
                                                                  (Option->BbsEntry->Device << 11) |
                                                                  (Option->BbsEntry->Function << 8);
        }
        else 
#endif
        {
            UINT32 Bus = 0 ;   
            // Uefi Option just need get device path by Option->FilePathList.
            DevInform->DevPath = Option->FilePathList ;
            DevInform->DevType = (UINT16)GetUefiBootOptionTag(Option) ;
            
            // Get Uefi Option BDF
            Dp = DevInform->DevPath ;
            while(!(isEndNode(Dp)))
            {
                if ( Dp->Type == ACPI_DEVICE_PATH && Dp->SubType == ACPI_DP )
                {
                    Bus = ((ACPI_HID_DEVICE_PATH*)Dp)->UID ;
                }
                else if ( Dp->Type == HARDWARE_DEVICE_PATH && Dp->SubType == HW_PCI_DP )
                {
                    DevInform->DevBDF = (UINT32) (Bus << 16) |
                                                                          (((PCI_DEVICE_PATH*)Dp)->Device << 11) |
                                                                          (((PCI_DEVICE_PATH*)Dp)->Function << 8);        
                    break ;
                }
                Dp=NEXT_NODE(Dp) ;
            }
        }
        
        // Get USB and SATA Port Number
        Dp = DevInform->DevPath ;
        while (!(isEndNode(Dp)))
        {
            if ( Dp->Type == MESSAGING_DEVICE_PATH && Dp->SubType == MSG_SATA_DP )
            {
                DevInform->DevPortNum = ((SATA_DEVICE_PATH*)Dp)->HBAPortNumber ;
                break ;
            }
            else if ( Dp->Type == MESSAGING_DEVICE_PATH && Dp->SubType == MSG_USB_DP )
            {
                if (!UsbPortFlag) UsbPortFlag = TRUE ;
                else
                {
                    DevInform->DevPortNum = (UINT16)((USB_DEVICE_PATH*)Dp)->ParentPortNumber ;
                    break ;
                }
            }
            Dp=NEXT_NODE(Dp) ;
        }
    }
}

/**
    Set Device New Description

    @param EFI_DEVICE_PATH_PROTOCOL *DpHead
        CHAR16 *Description
    @retval 

    @note  OEM can use this function to update new device description.
            example: SetDevNewDescription(Dp, L"OEM USB Key")
**/
EFI_STATUS
SetDevNewDescription(
    EFI_DEVICE_PATH_PROTOCOL *DpHead, 
    CHAR16 *Description)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FIXED_BOOT_ORDER_PROTOCOL*FBOProtocol ;
    EFI_GUID FixedBootOrderGuid=FIXED_BOOT_ORDER_GUID;
    Status = pBS->LocateProtocol (&FixedBootOrderGuid, NULL, &FBOProtocol);
    if (!EFI_ERROR(Status))
    {
        // Prepare FBO_DEVICE_INFORM data
        FBO_DEVICE_INFORM *SetData = MallocZ(sizeof(FBO_DEVICE_INFORM));
        EFI_DEVICE_PATH_PROTOCOL *Dp = NULL ;
        UINTN DpSize = 0 ;
        Dp = DpHead ;
        for ( ; !(isEndNode(Dp)) ; Dp=NEXT_NODE(Dp))
            DpSize+=NODE_LENGTH(Dp) ;
        DpSize += sizeof(EFI_DEVICE_PATH_PROTOCOL) ; //End Node Size
        
        SetData->DevName = MallocZ(StrLen(Description)*sizeof(CHAR16)+1) ;
        SetData->DevPath = MallocZ(DpSize) ;
        StrCpy( SetData->DevName, Description);
        MemCpy( SetData->DevPath, DpHead, DpSize);
        // Set to new description.
        Status = FBOProtocol->SetNewDescription( SetData ) ;
    }
    return Status ;
}

/**
    Change FBO Device Name

    @param 

    @retval 

    @note  EIP 143657

**/
VOID
FBO_AdjustDeviceName()
{
    EFI_STATUS Status = EFI_SUCCESS;
    FBO_DEVICE_INFORM *Node = NULL, *PreNode = NULL ;
    UINT16 x ;
    GetDevInformByBootOptionList(&Node) ;
    for ( x=0 ; FboChangeDeviceNameFuncs[x] ; x++)
        FboChangeDeviceNameFuncs[x](Node) ;
    
    // Free Linked list data ;
    while (Node)
    {
        PreNode = Node;
        Node = Node->Next;
        pBS->FreePool(PreNode);
    }
}

/**
    Go through the entire boot option list and Apply priorities for
    each entry in the list.

    @param DLIST *BootOptionList - the entire Boot Option List

    @retval VOID

    @note  To change boot order priorities

**/
VOID FBO_SetBootOptionTags() {
    DLINK *Link;
    BOOT_OPTION *Option;
    UINT32 UefiBootOptionsInc = DEFAULT_PRIORITY_INCREMENT;

    if (gBdsSkipFBOModule) 
    {
        SetBootOptionTags() ;
        return ;
    }
    
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option) {
        UINT32 TagPriority;
        UINT32 BaseTag = UNASSIGNED_HIGHEST_TAG;
#if CSM_SUPPORT
        if (IsLegacyBootOption(Option)) {
            UINT16 DeviceType;
            DeviceType = ((BBS_BBS_DEVICE_PATH*)Option->FilePathList)->DeviceType;
            if ( DeviceType == BoTagLegacyBevDevice )
                BaseTag = DeviceType;
            else if ( DeviceType != BBS_UNKNOWN )
                BaseTag = FixedLegacyBootOptionTags[DeviceType];
        } else
#endif
            if (IsShellBootOption(Option)) BaseTag = BoTagEmbeddedShell;
            else BaseTag = BoTagUefi;
        if (BaseTag == UNASSIGNED_HIGHEST_TAG) continue;
        TagPriority = FBO_FindTagPriority(BaseTag);
        //UEFI boot options must have unique tags, otherwise then will be groupped when
        //GROUP_BOOT_OPTIONS_BY_TAG tokens is enabled
        if (BaseTag == BoTagUefi) BaseTag += UefiBootOptionsInc++;
        Option->Tag = BootOptionTag(BaseTag, TagPriority);
    }
}

/**
    Get the Hard Disk's tag (type) according to its port number

    @param BOOT_OPTION *Option - Boot option to find the port number

    @retval UINT32 device tag (type)

**/
UINT32 GetUefiHardDiskTagByPort(
    IN BOOT_OPTION *Option
)
{
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FixedBootOrderProtocol;
    EFI_STATUS Status;

    Status = pBS->LocateProtocol(
                &gFixedBootOrderGuid,
                NULL,
                &FixedBootOrderProtocol
            );

    //
    // Get Uefi Map and compare Port with it.
    //
    if(!EFI_ERROR(Status))
    {
        FBODevMap *UefiDevMap = FixedBootOrderProtocol->GetUefiDevMap();
        if(NULL != UefiDevMap)
        {
            UINT16 DevMapCount = GetDevMapDataCount(UefiDevMap);
            UINT16 DevMapIdx;

            for(DevMapIdx = 0; DevMapIdx < DevMapCount; DevMapIdx++)
            {
                //
                // Only Compare with HDD
                //
                if(UefiDevMap[DevMapIdx].Attr != F_HDD) continue;

                if(UefiDevMap[DevMapIdx].Port != FBO_USELESS_PORT_NUMBER)
                {
                    //
                    // Get Port and Compare it with table's
                    //
                    EFI_HANDLE Handle = GetPhysicalBlockIoHandle(Option->DeviceHandle);
                    UINT32 Port = GetHddPort(Handle);

                    if(UefiDevMap[DevMapIdx].Port == Port)
                        return UefiDevMap[DevMapIdx].DevType;
                }
                else
                {
                    return UefiDevMap[DevMapIdx].DevType;
                }
            }
        }
        else
        {
            TRACE((-1, "Get UefiDevMap fail\n"));
        }
    }
    else
    {
        TRACE((-1, "Get Fixed Boot Order Protocol fail\n"));
    }

    return BoTagUefi;
}

/**
    Check whether Uefi Hdd with sata to usb adapter

    @param BOOT_OPTION *Option - boot option

    @retval TRUE   Is Uefi Hdd with sata to usb adapter
	@retval FALSE  Isn't Uefi Hdd With sata to usb adapter

**/
BOOLEAN IsSataToUsbHdd(BOOT_OPTION *Option)
{
    EFI_STATUS Status;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    EFI_BLOCK_IO_PROTOCOL *BlkIo;

    if(Option && Option->DeviceHandle)
    {
        EFI_GUID EfiBlockIoProtocolGuid = EFI_BLOCK_IO_PROTOCOL_GUID;

        Status = pBS->HandleProtocol(
                    Option->DeviceHandle,
                    &EfiBlockIoProtocolGuid,
                    (VOID*)&BlkIo);

        if(!EFI_ERROR(Status))
        {
            if(!BlkIo->Media->RemovableMedia)
            {
                Status = pBS->HandleProtocol(
                            Option->DeviceHandle,
                            &gEfiDevicePathProtocolGuid,
                            (VOID**)&Dp);

                if(!EFI_ERROR(Status))
                {
                    return IsUsbDp(Dp, NULL);
                }
            }
        }
    }

    return FALSE;
}

/**
    Function returns device type for given handle

    @param BOOT_OPTION *Option - boot option

    @retval UINT32 device type

**/
UINT32 GetUefiBootOptionTag(
    IN BOOT_OPTION *Option
)
{
#if !EFI_DEVICE_IN_ONE_GROUP
    EFI_STATUS Status;
    EFI_HANDLE Handle, BlockHandle;
    EFI_DEVICE_PATH_PROTOCOL *Dp = Option->FilePathList;
    EFI_DEVICE_PATH_PROTOCOL *DpPtr = Dp;
    BOOLEAN IsUSB = FALSE;

    if ( Dp == NULL )
        return UNASSIGNED_HIGHEST_TAG;

#if SHELL_GROUP_SUPPORT                             //(EIP115686+)>
    if (IsShellBootOption(Option))
        return BoTagUefiApplication;
#endif                                              //<(EIP115686+)

    for ( ; !(isEndNode(DpPtr)); DpPtr = NEXT_NODE(DpPtr))
    {
        if (DpPtr->Type == MESSAGING_DEVICE_PATH)
        {
            if (DpPtr->SubType == MSG_USB_DP)
#if USB_GROUP == 1
                return BoTagUefiUsb;
#elif USB_GROUP == 2
                IsUSB = TRUE;
#endif
            //Check Windows To Go USB Hard Disk boot option. (EIP101213+)>
            if (DpPtr->SubType == MSG_USB_CLASS_DP)
#if USB_GROUP == 1
                return BoTagUefiUsb;
#elif USB_GROUP == 0
                return BoTagUefiHardDisk;
#else
                return BoTagUefiUsbHardDisk;
#endif
            else					     //<(EIP101213+)
                if (DpPtr->SubType == MSG_MAC_ADDR_DP)
                {
                    if ( IsUSB ) return BoTagUefiUsbLan ;
                    return BoTagUefiNetWork;
                }
                else if ( (DpPtr->SubType == MSG_IPv4_DP) || (DpPtr->SubType == MSG_IPv6_DP) )
                {   
                    if ( IsUSB ) return BoTagUefiUsbLan ;
                    else  return BoTagUefiNetWork;
                }

            continue;
        }

        if (DpPtr->Type == MEDIA_DEVICE_PATH)
        {
#if PUT_SATA_TO_USB_HDD_INTO_UEFI_USB_HDD_GROUP
            //
            //  Uefi Hdd with sata to usb adapter may be treated as fixed device so 
            //  EfiOsBootOptionNames creates boot option for it without usb dp 
            //  in device path.
            //
            if(!IsUSB && IsSataToUsbHdd(Option))
                return BoTagUefiUsbHardDisk;
#endif

            if (!IsUSB && DpPtr->SubType == MEDIA_HARDDRIVE_DP)
                return GetUefiHardDiskTagByPort(Option);
            else if (!IsUSB && DpPtr->SubType == MEDIA_CDROM_DP)
                return BoTagUefiCdrom;
            else if (IsUSB && DpPtr->SubType == MEDIA_HARDDRIVE_DP)
            {
                Status=pBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid, &Dp, &Handle);
                if (!EFI_ERROR(Status))
                {
                    BlockHandle = GetPhysicalBlockIoHandle(Handle);
                    if ( IsUSBkeyByHandle( BlockHandle ) )
                        return BoTagUefiUsbKey;
                    else
                        return BoTagUefiUsbHardDisk;
                }
            }
            else if (IsUSB && DpPtr->SubType == MEDIA_CDROM_DP)
                return BoTagUefiUsbCdrom;
        }
    }
    //(EIP96232+)>

    if ( IsUSB )
    {
        Status=pBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid, &Dp, &Handle);
        if (!EFI_ERROR(Status))
        {
            EFI_GUID gEfiBlockIoProtocolGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
            EFI_BLOCK_IO_PROTOCOL *BlkIo;

            BlockHandle = GetPhysicalBlockIoHandle(Handle);

            if ( !EFI_ERROR( pBS->HandleProtocol(
                                 BlockHandle,
                                 &gEfiBlockIoProtocolGuid,
                                 &BlkIo)))
            {
//(EIP133551+)>
                UINT64 SizeInMb;
        	
                SizeInMb = MultU64x32(BlkIo->Media->LastBlock + 1, BlkIo->Media->BlockSize);
                SizeInMb = DivU64x32(SizeInMb, 1000000);

                if( SizeInMb > USBKEY_RANGE_SIZE )  //size > 32G? (Token default).
                    return BoTagUefiUsbHardDisk;
                else
                if( SizeInMb > MAX_SIZE_FOR_USB_FLOPPY_EMULATION )  //size > 530M? (Token default).
                    return BoTagUefiUsbKey;
//<(EIP133551+)
                return BoTagUefiUsbFloppy;
            }
        }
    }
    //<(EIP96232+)
#endif  //#if !EFI_DEVICE_IN_ONE_GROUP
    return BoTagUefi;
}
/**
    Since FixedBootOrder may creates the legacy types that are not
    in starandard specification, the group name will be constructed
    as "Unknown Device" by default, we need to add the new
    construction rule to change the name by inserting this eLink.     

    @param BOOT_OPTION *Option - boot option
        CHAR16 *Name - buffer to save the name
        UINTN NameSize - maximum of name size

    @retval VOID
**/
UINTN ConstructGroupNameByFBO(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
)
{
    FBO_GROUP_NAME      DevTypeToName[] = {FBO_LEGACY_TYPE_TO_NAME,{0,NULL}};
    BBS_BBS_DEVICE_PATH *Dp = (BBS_BBS_DEVICE_PATH*)(Option->FilePathList);
    UINTN               i;
    
    if (gBdsSkipFBOModule) return 0 ;
    if(!Option->GroupHeader) return 0;

    if (!IsLegacyBootOption(Option)) return 0;
    
    // Find Match Device Type and Update Name
    for(i = 0 ; DevTypeToName[i].LegacyDevType != 0 ; i++)
        if(DevTypeToName[i].LegacyDevType == Dp->DeviceType)
            return Swprintf_s(Name, NameSize, L"%s",DevTypeToName[i].DeviceName);

    return 0;  
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
