//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/PTT/FastBoot.h 6     6/01/12 6:35a Bibbyyeh $
//
// $Revision: 6 $
//
// $Date: 6/01/12 6:35a $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  FastBoot.h
//
//  Description:
//  Fast boot functions definitions
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef __FASTBOOT__H__
#define __FASTBOOT__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/DevicePath.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>


#define FAST_BOOT_TYPE_EFI         0x00000001    //last boot was from UEFI device
#define FAST_BOOT_TYPE_LEGACY      0x00000000    //last boot was from Legacy device


// {B540A530-6978-4da7-91CB-7207D764D262}
#define FAST_BOOT_VARIABLE_GUID \
    { 0xb540a530, 0x6978, 0x4da7, 0x91, 0xcb, 0x72, 0x7, 0xd7, 0x64, 0xd2, 0x62 }

GUID_VARIABLE_DECLARATION(FastBootVariableGuid, FAST_BOOT_VARIABLE_GUID);

#pragma	pack(1)

typedef struct _FAST_BOOT {
    UINT32 BootOptionNumber : 16;
    UINT32 BootType         : 2;
    UINT32 Reserved         : 14;
    UINT32 BootCount;
    UINT16 DevStrCheckSum; 
} FAST_BOOT;
#pragma	pack()

//FastBoot.c

VOID FastBootWorker(
    VOID
);

VOID ResetFastBootVariable(
    VOID
);

BOOLEAN OemConfigurationChanged(
    VOID
);

EFI_STATUS CreateLegacyFastBootOption(
    FAST_BOOT *Var
);

EFI_STATUS CreateEfiFastBootOption(
    IN EFI_LOAD_OPTION *BootOption,
    IN UINTN           OptionSize,
    IN UINT16          OptionNumber
);

BOOLEAN IsSupportedDevice(
    IN  EFI_DEVICE_PATH_PROTOCOL *Dp,
    OUT BOOLEAN                  *UsbMassStorage,
    OUT BOOLEAN                  *HddFilePath
);

VOID FastBootClearBootFailFlag(
	IN EFI_EVENT Event, 
    IN VOID      *Context
);

BOOLEAN IsBootToShell(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp
);

VOID FastBootReadyToBootNotify(
	IN EFI_EVENT Event, 
    IN VOID      *Context
);

EFI_STATUS FastLegacyBoot(
    VOID
);

EFI_STATUS FastEfiBoot(
    VOID
);

EFI_STATUS CheckLoader(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp
);

EFI_STATUS TseHandShake();
VOID ReturnToNormalBoot(
    VOID
);

BOOLEAN IsRuntime(
    VOID
);

VOID FbConnectInputDevices(
    VOID
);


//FastBootTseHook.c
										//(EIP63924+)>
EFI_STATUS FastBootCheckForKey( 		
    IN  BOOLEAN *EnterSetup,
    IN  UINT32  *BootFlow);

EFI_STATUS FastBootStopCheckForKeyTimer();

										//<(EIP63924+)
//Hook
typedef BOOLEAN (IS_VALID_FASTBOOT_BOOT_OPTION_FUNC_PTR)(
    IN  UINTN BootOrderIndex,
    IN  EFI_LOAD_OPTION *BootOption
);
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
