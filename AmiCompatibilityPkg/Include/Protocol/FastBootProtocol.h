//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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
  Fast boot protocol definitions
*/

#ifndef __FASTBOOT_PROTOCOL__H__
#define __FASTBOOT_PROTOCOL__H__
#ifdef __cplusplus
extern "C" {
#endif
#pragma	pack(1)


// {3496A19A-2E99-41ba-833E-0FDE2EBF2A55}
#define AMI_FAST_BOOT_PROTOCOL_GUID \
   { 0x3496a19a, 0x2e99, 0x41ba, 0x83, 0x3e, 0xf, 0xde, 0x2e, 0xbf, 0x2a, 0x55 }

										//(EIP63924+)>
// {8fba43b8-acdb-4ee2-ab31-0fa4d5b6c3c5}
#define FAST_BOOT_TSE_PROTOCOL_GUID \
   { 0x0fba43b8, 0xacdb, 0x4ee2, 0xab, 0x31, 0x0f, 0xa4, 0xd5, 0xb6, 0xc3, 0xc5 }
										//<(EIP63924+)

#define FAST_BOOT_POLICY_PROTOCOL_GUID \
    { 0xb2a07be9, 0xc754, 0x4c36, 0x93, 0x02, 0x7c, 0xc7, 0x8a, 0xa0, 0xd8, 0x75 }

//GUID_VARIABLE_DECLARATION(gAmiFastBootProtocolGuid, AMI_FAST_BOOT_PROTOCOL_GUID);
extern EFI_GUID gAmiFastBootProtocolGuid;
typedef struct _SKIP_PCI_LIST{
	UINT8   Class;
    UINT8   SubClass;
} SKIP_PCI_LIST;

typedef struct _FAST_BOOT_POLICY {
    BOOLEAN                     FastBootEnable;
    BOOLEAN                     TestMode;
//pervious boot information
    BOOLEAN                     UEfiBoot;
    UINT16                      BootOptionNumber;
    UINT16                      DevStrCheckSum;
    EFI_DEVICE_PATH_PROTOCOL    *FastBootOption;
    BOOLEAN                     LastBootFailure;
    BOOLEAN                     LastBootVarPresence;
    UINT32                      BootCount;
//fast boot behavior para
    BOOLEAN                     FirstFastBootInS4;
    BOOLEAN                     CheckBootOptionNumber;
    BOOLEAN                     CheckDevStrCheckSum;
    BOOLEAN                     VGASupport;
    BOOLEAN                     UsbSupport;
    VOID                        *UsbSkipTable;
    UINTN                       UsbSkipTableSize;
    BOOLEAN                     Ps2Support;
    BOOLEAN                     NetWorkStackSupport;
    SKIP_PCI_LIST               *SkipPciList;
    UINTN                       SkipPciListSize;
    BOOLEAN                     CheckPassword;
    BOOLEAN                     SkipTSEHandshake;
    BOOLEAN                     ConnectAllSata;     //(EIP96276.3)+
    BOOLEAN                     InFastBootPath;
} FAST_BOOT_POLICY;


typedef
EFI_STATUS
(EFIAPI *AMI_FAST_BOOT_LAUNCH) (
	VOID
	);

typedef
VOID
(EFIAPI *AMI_FAST_BOOT_CONNECT_INPUT_DEVICE) (
	VOID
	);

typedef
BOOLEAN
(EFIAPI *AMI_FAST_BOOT_IS_RUNTIME) (
	VOID
	);

typedef struct _AMI_FAST_BOOT_PROTOCOL
{
	AMI_FAST_BOOT_LAUNCH Launch;
    AMI_FAST_BOOT_CONNECT_INPUT_DEVICE ConnectInputDevices;
    AMI_FAST_BOOT_IS_RUNTIME IsRuntime;
}AMI_FAST_BOOT_PROTOCOL;


										//(EIP63924+)>
typedef
EFI_STATUS
(EFIAPI *FAST_BOOT_CHECK_FOR_KEY) (
    BOOLEAN *EnterSetup,
    UINT32  *BootFlow
	);
                                        
typedef
EFI_STATUS
(EFIAPI *FAST_BOOT_STOP_CHECK_FOR_KEY_TIMER)();

										//(EIP68329+)>
typedef
VOID
(EFIAPI *FAST_BOOT_PASSWORD_FREEZE_DEVICES)();
										//<(EIP68329+)
typedef struct _FAST_BOOT_TSE_PROTOCOL
{
    FAST_BOOT_CHECK_FOR_KEY FastBootCheckForKey;
    FAST_BOOT_STOP_CHECK_FOR_KEY_TIMER FastBootStopCheckForKeyTimer;
    FAST_BOOT_PASSWORD_FREEZE_DEVICES FastBootPasswordFreezeDevices; //(EIP68329+)
}
FAST_BOOT_TSE_PROTOCOL; 
										//<(EIP63924+)
#pragma	pack()
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
