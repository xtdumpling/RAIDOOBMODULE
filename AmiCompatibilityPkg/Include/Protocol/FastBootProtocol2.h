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

/** @file FastBootProtocol2.h
 *  Fast boot protocol definitions.
 */

#ifndef __FASTBOOT_PROTOCOL2__H__
#define __FASTBOOT_PROTOCOL2__H__
#ifdef __cplusplus
extern "C" {
#endif
#pragma    pack(1)


/// {3496A19A-2E99-41ba-833E-0FDE2EBF2A55}
///#define AMI_FAST_BOOT_PROTOCOL_GUID \
///   { 0x3496a19a, 0x2e99, 0x41ba, 0x83, 0x3e, 0xf, 0xde, 0x2e, 0xbf, 0x2a, 0x55 }
/// {30CB7768-ECDE-445a-BAAC-665445095522}
#define AMI_FAST_BOOT_PROTOCOL2_GUID \
         { 0x30cb7768, 0xecde, 0x445a, {0xba, 0xac, 0x66, 0x54, 0x45, 0x9, 0x55, 0x22}}

                                        ///<(EIP63924+)
/// {8fba43b8-acdb-4ee2-ab31-0fa4d5b6c3c5}
///#define FAST_BOOT_TSE_PROTOCOL_GUID \
///  { 0x0fba43b8, 0xacdb, 0x4ee2, 0xab, 0x31, 0x0f, 0xa4, 0xd5, 0xb6, 0xc3, 0xc5 }
                                        ///<(EIP63924+)
/// {42544F90-B0C2-42f2-884D-2769AE6599C5}
#define FAST_BOOT_TSE_PROTOCOL2_GUID \
        { 0x42544f90, 0xb0c2, 0x42f2, {0x88, 0x4d, 0x27, 0x69, 0xae, 0x65, 0x99, 0xc5 }}


///#define FAST_BOOT_POLICY_PROTOCOL_GUID \
///    { 0xb2a07be9, 0xc754, 0x4c36, 0x93, 0x02, 0x7c, 0xc7, 0x8a, 0xa0, 0xd8, 0x75 }
#define FAST_BOOT_POLICY_PROTOCOL2_GUID \
         { 0x302cb4f, 0xaf05, 0x4a16, {0x99, 0xa9, 0x31, 0xad, 0xcf, 0x87, 0x8, 0x90} }

///GUID_VARIABLE_DECLARATION(gAmiFastBootProtocolGuid, AMI_FAST_BOOT_PROTOCOL_GUID);
///extern EFI_GUID gAmiFastBootProtocolGuid;

#ifdef PTT_BUILD
EFI_GUID gAmiFastBootProtocol2Guid = AMI_FAST_BOOT_PROTOCOL2_GUID;
#else
extern EFI_GUID gAmiFastBootProtocol2Guid;
#endif

typedef struct _SKIP_PCI_LIST2{
    UINT8   Class;
    UINT8   SubClass;
} SKIP_PCI_LIST2;

typedef struct _FAST_BOOT_POLICY2 {
    BOOLEAN                     FastBootEnable;
///previous boot information
    BOOLEAN                     UefiBoot;
    UINT16                      BootOptionNumber;
    UINT16                      DevStrCheckSum;
    EFI_DEVICE_PATH_PROTOCOL    *FastBootOption;
    BOOLEAN                     LastBootFailure;
    BOOLEAN                     LastBootVarPresence;
    UINT32                      BootCount;
    VOID                        *SataDevice;
///fast boot behavior para
    BOOLEAN                     FirstFastBootInS4;
    BOOLEAN                     CheckBootOptionNumber;
    BOOLEAN                     CheckDevStrCheckSum;
    UINT8                       SataSupport;     
    BOOLEAN                     VgaSupport;
    UINT8                       UsbSupport; 
    VOID                        *UsbSkipTable;
    UINTN                       UsbSkipTableSize;    
    BOOLEAN                     Ps2Support;
    BOOLEAN                     RedirectionSupport;
    BOOLEAN                     NetworkStackSupport;
    SKIP_PCI_LIST2               *SkipPciList;
    UINTN                       SkipPciListSize;
    BOOLEAN                     CheckPassword;
    BOOLEAN                     SkipTseHandshake;
    BOOLEAN                     InFastBootPath;
} FAST_BOOT_POLICY2;



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

typedef struct _AMI_FAST_BOOT_PROTOCOL2
{
    AMI_FAST_BOOT_LAUNCH Launch;
    AMI_FAST_BOOT_CONNECT_INPUT_DEVICE ConnectInputDevices;
    AMI_FAST_BOOT_IS_RUNTIME IsRuntime;
}AMI_FAST_BOOT_PROTOCOL2;


                                        ///<(EIP63924+)
typedef
EFI_STATUS
(EFIAPI *FAST_BOOT_CHECK_FOR_KEY) (
    BOOLEAN *EnterSetup,
    UINT32  *BootFlow
    );
                                        
typedef
EFI_STATUS
(EFIAPI *FAST_BOOT_STOP_CHECK_FOR_KEY_TIMER)();

                                        ///<(EIP68329+)
typedef
VOID
(EFIAPI *FAST_BOOT_PASSWORD_FREEZE_DEVICES)();
                                        ///<(EIP68329+)
typedef struct _FAST_BOOT_TSE_PROTOCOL2
{
    FAST_BOOT_CHECK_FOR_KEY FastBootCheckForKey;
    FAST_BOOT_STOP_CHECK_FOR_KEY_TIMER FastBootStopCheckForKeyTimer;
    FAST_BOOT_PASSWORD_FREEZE_DEVICES FastBootPasswordFreezeDevices; ///<(EIP68329+)
}
FAST_BOOT_TSE_PROTOCOL2; 
                                        ///<(EIP63924+)
#pragma    pack()
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
