//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file FastBoot2.h
 *  Fast boot functions definitions.
 */

#ifndef __FASTBOOT2__H__
#define __FASTBOOT2__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/DevicePath.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>
#include <Hob.h>
#include <Token.h>
#include <Library/DebugLib.h>


#define FAST_BOOT_TYPE_EFI         0x00000001    ///last boot was from UEFI device
#define FAST_BOOT_TYPE_LEGACY      0x00000000    ///last boot was from Legacy device


#if FASTBOOT_NEED_RESTART
#define FAST_BOOT_RESTART L"FastbootRestart" 
#define FAST_BOOT_RESTART_COUNT L"FastbootRestartCount"

///{39ac7cb1-789c-45f6-94f0-68873c5c1ff2}
#define FAST_BOOT_RESTART_COUNT_GUID \
    { 0x39ac7cb1, 0x789c, 0x45f6, {0x94, 0xf0, 0x68, 0x87, 0x3c, 0x5c, 0x1f, 0xf2 }}

///{800dabd7-ca8d-4611-bd77-3b200ebc275a}
#define FAST_BOOT_RESTART_GUID \
    { 0x800dabd7, 0xca8d, 0x4611, {0xbd, 0x77, 0x3b, 0x20, 0x0e, 0xbc, 0x27, 0x5a }}
#endif

/// {B540A530-6978-4da7-91CB-7207D764D262}
#define FAST_BOOT_VARIABLE_GUID \
    { 0xb540a530, 0x6978, 0x4da7, {0x91, 0xcb, 0x72, 0x7, 0xd7, 0x64, 0xd2, 0x62 }}

GUID_VARIABLE_DECLARATION(FastBootVariableGuid, FAST_BOOT_VARIABLE_GUID);

/// {50535FFD-1555-4329-A2C5-F3F44011A449}
#define AMI_SATA_PRESENT_HOB_GUID \
    {0x50535ffd, 0x1555, 0x4329, {0xa2, 0xc5, 0xf3, 0xf4, 0x40, 0x11, 0xa4, 0x49}}


#pragma    pack(1)
typedef struct _SATA_INFO {
    UINT16              ClassCode;      ///Base class and Sub class code
    UINT32              PresentPortBitMap;    //Upper word is BDF, Lower word is port bitmap
                                              /** Bit map description    
                                                    IDE Mode:
                                                    Bit0: Primary Master
                                                    Bit1: Secondary Master
                                                    Bit2: Primary Slave
                                                    Bit3: Secondary Slave
                                                
                                                    AHCI or RAID mode:
                                                    Bit0: Port0
                                                    Bit1: Port1
                                                    Bit2: Port2
                                                    Bit3: Port3
                                                    Bit4: Port4
                                                    Bit5: Port5
                                              **/      
} SATA_INFO;

#if SUPPORT_RAID_DRIVER
typedef struct _SATA_PRESENT_HOB {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    UINT8               ControllerCount;
    SATA_INFO           SataInfo[4];
} SATA_PRESENT_HOB;
#endif

#if SUPPORT_RAID_DRIVER
typedef struct _SATA_DEVICE_DATA{ 
    UINT16  Bdf;
    UINT16  ClassCode;
    UINT8   DevType;
    UINT8   PortNum; ///IDE MODE: bit0:channel, bit1:device
    UINT8    SerialNum[20];
} SATA_DEVICE_DATA;
#endif

typedef struct _FAST_BOOT2 {
    UINT32 BootOptionNumber : 16;
    UINT32 BootType         : 2;
    UINT32 Reserved         : 14;
    UINT32 BootCount;
    UINT16 DevStrCheckSum; 
///SATA device information
#if SUPPORT_RAID_DRIVER
    SATA_DEVICE_DATA    SataDevice[MAX_SATA_DEVICE_COUNT] ;
#endif
///SATA controller information   
    UINT8  ControllerCount;
    SATA_INFO SataInfo[4];
} FAST_BOOT2;
#pragma    pack()

///FastBoot.c

VOID 
EFIAPI 
FastBootWorker(
    VOID
);

VOID 
EFIAPI 
ResetFastBootVariable(
    VOID
);

BOOLEAN 
EFIAPI 
OemConfigurationChanged(
    VOID
);

EFI_STATUS 
EFIAPI 
CreateLegacyFastBootOption(
    FAST_BOOT2 *Var
);

EFI_STATUS 
EFIAPI 
CreateEfiFastBootOption(
    IN EFI_LOAD_OPTION *BootOption,
    IN UINTN           OptionSize,
    IN UINT16          OptionNumber
);

BOOLEAN 
EFIAPI 
IsSupportedDevice(
    IN  EFI_DEVICE_PATH_PROTOCOL *Dp,
    OUT BOOLEAN                  *UsbMassStorage,
    OUT BOOLEAN                  *HddFilePath
);

VOID 
EFIAPI 
FastBootClearBootFailFlag(
    IN EFI_EVENT Event, 
    IN VOID      *Context
);

BOOLEAN 
EFIAPI 
IsBootToShell(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp
);

VOID 
EFIAPI 
FastBootReadyToBootNotify(
    IN EFI_EVENT Event, 
    IN VOID      *Context
);

EFI_STATUS 
EFIAPI 
FastLegacyBoot(
    VOID
);

EFI_STATUS 
EFIAPI 
FastEfiBoot(
    VOID
);

EFI_STATUS 
EFIAPI 
TseHandshake();

VOID 
EFIAPI 
ReturnToNormalBoot(
    VOID
);

BOOLEAN 
EFIAPI 
IsRuntime(
    VOID
);

VOID 
EFIAPI 
FbConnectInputDevices(
    VOID
);


///FastBootTseHook.c
                                        ///(EIP63924+)>
EFI_STATUS 
EFIAPI 
FastBootCheckForKey(         
    IN  BOOLEAN *EnterSetup,
    IN  UINT32  *BootFlow);

EFI_STATUS 
EFIAPI 
FastBootStopCheckForKeyTimer();

                                        ///<(EIP63924+)
///Hook
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
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
