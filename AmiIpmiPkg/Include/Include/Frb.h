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

/** @file Frb.h
    FRB common Enum structures

**/

#ifndef _FRB__H_
#define _FRB__H_

//
//    Enum data type used
//
typedef enum {
    EfiFrbReserved,
    EfiFrb2,
    EfiBiosPost,
    EfiOsBootWdt,
    EfiFrbReserved1,
    EfiFrbReserved2,
    EfiFrbReserved3,
    EfiFrbReserved4,
    EfiFrb1NotSupported,
    EfiFrb3NotSupported,
    EfiFrbReservedLast
} EFI_FRB_TYPE;

typedef enum {
    NoTimeoutInterrupt,
    SystemManagementInterrupt,
    DiagnosticInterrupt,
    MessagingInterrupt
} FRB_SYSTEM_TIMEOUT_INTERRUPT;

typedef enum {
    EfiNormalBoot,
    EfiHardReset,
    EfiPowerDown,
    EfiPowerCycle,
    EfiFrbSysReserved1,
    EfiFrbSysReserved2,
    EfiFrbSysReserved3,
    EfiFrbSysReserved4,
    EfiDiagnosticMode,
    EfiPlatformRecovery
} FRB_SYSTEM_BOOT_POLICY;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
