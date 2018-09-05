//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file FastBootOptionBds.c
 *  Used to remove temp boot option.
 */

///----------------------------------------------------------------------
/// Include Files
///----------------------------------------------------------------------
#include <Efi.h>
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <BootOptions.h>
#include "FastBootOption.h"
#include <FastBoot2.h>
///---------------------------------------------------------------------------
/// Global Variable declarations
///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
/// External functions
///---------------------------------------------------------------------------
extern EFI_RUNTIME_SERVICES  *gRT;
extern EFI_BOOT_SERVICES     *gBS;
extern EFI_SYSTEM_TABLE      *gST;
///---------------------------------------------------------------------------
/// Function Definitions
///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
/// Type definitions
///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
/// Constant and Variables declarations
///---------------------------------------------------------------------------
 BOOLEAN TempDeviceFound = FALSE;
 UINT16 BootDeviceType = 0;

 /**
  * Function returns device type.
  *    
  * @param[in]   Dp                Boot option device path. 
  *
  * @retval    UINT32            Boot option device type.
  */
 
UINT32 
GetUefiDeviceType(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp
)
{
    EFI_DEVICE_PATH_PROTOCOL *DpPtr = Dp;
    BOOLEAN IsUsb = FALSE;

    if ( Dp == NULL )
    return 0xffff;

    for ( ; !(isEndNode(DpPtr)); DpPtr = NEXT_NODE(DpPtr))
    {
        if (DpPtr->Type == MESSAGING_DEVICE_PATH)
        {
            if (DpPtr->SubType == MSG_USB_DP)
                IsUsb = TRUE;

            if (DpPtr->SubType == MSG_MAC_ADDR_DP)
                return TempUefiNetDevice;

            continue;
        }

        if (DpPtr->Type == MEDIA_DEVICE_PATH)
        {
            if (!IsUsb && DpPtr->SubType == MEDIA_HARDDRIVE_DP)
                return TempUefiHddDevice;
        else
            if (!IsUsb && DpPtr->SubType == MEDIA_CDROM_DP)
                return TempUefiOddDevice;
        else
            if (IsUsb && DpPtr->SubType == MEDIA_HARDDRIVE_DP)
        {
        return TempUefiRemDevice;
        }
            else
            if (IsUsb && DpPtr->SubType == MEDIA_CDROM_DP)
                return TempUefiOddDevice;
        }
    }
    return 0xffff;
}

/**
 * Compare Boot device type and re-setting variable "BootNext".
 *    
 */

VOID 
EFIAPI 
SetBootNextBootOption()
{
    EFI_STATUS Status;
    DLINK *Link;
    BOOT_OPTION *Option;
    UINT16 BootNext;
    UINTN Size;

    Size = sizeof(BootNext);

    if ( TempDeviceFound )
    {
        FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option){

        if ( GetUefiDeviceType(Option->FilePathList) == BootDeviceType )
        {
            BootNext = Option->BootOptionNumber;
            Status = gRT->SetVariable(L"BootNext", &EfiVariableGuid, BOOT_VARIABLE_ATTRIBUTES, Size, &BootNext);                
            DEBUG((DEBUG_INFO,"FastBootOptionBds.c:: SetVariable Status=%r BootNext=%d\n", Status, BootNext));
        }

    }
    }
}

/**
 * Check BootNext device and find UEFI device, adjust BootNext number.
 *    
 */

VOID 
EFIAPI 
CheckBootNextTempBootOption()
{
    EFI_STATUS Status;
    DLINK *Link;
    BOOT_OPTION *Option;
    UINT16 BootNext;
    UINTN Size;

    Size = sizeof(BootNext);
    Status = gRT->GetVariable(L"BootNext", &EfiVariableGuid, NULL, &Size, &BootNext);

    DEBUG((DEBUG_INFO,"FastBootOptionBds.c:: Status=%r BootNext=%d\n", Status, BootNext));

    if ( !EFI_ERROR( Status ) )
    {
        FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        BBS_BBS_DEVICE_PATH *BbsDp;

        if ( BootNext == Option->BootOptionNumber )
        {
            BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
            
            if ( BbsDp->Header.Type == BBS_DEVICE_PATH )
            {
                BootDeviceType = BbsDp->DeviceType;
                switch ( BootDeviceType )
                {
                    case TempUefiHddDevice:
                    case TempUefiOddDevice:
                    case TempUefiRemDevice:
                    case TempUefiNetDevice:
                        TempDeviceFound=TRUE;
                        break;
                }
            }
        }
        }
    }
}

/**
 * Remove all TempUefiDevice BootOption.
 *    
 */

VOID 
EFIAPI 
RemoveFastBootOption()
{
    DLINK *Link;
    BOOT_OPTION *Option;

    CheckBootNextTempBootOption();

    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
    BBS_BBS_DEVICE_PATH *BbsDp;
    
    BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
    
    if ( BbsDp->Header.Type == BBS_DEVICE_PATH )
    {
        switch ( BbsDp->DeviceType )
        {
            case TempUefiHddDevice:
            case TempUefiOddDevice:
            case TempUefiRemDevice:
            case TempUefiNetDevice:
                Option->FwBootOption = TRUE;
                break;
        }
    }

    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
