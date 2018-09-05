//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeIncludes.h
    Common header file for the Nvme Driver

**/

#ifndef _NVME_INCLUDES_H_
#define _NVME_INCLUDES_H_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#include <Token.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/ComponentName2.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/SmmCommunication.h>
#include "NvmeInt13/NvmeInt13.h"
#include <Library/TimerLib.h>
#include <Protocol/StorageSecurityCommand.h>
#include <Protocol/AmiBlockIoWriteProtection.h>

//---------------------------------------------------------------------------
#define LANGUAGE_CODE_ENGLISH           "en-US"
#define MSG_NVME_DP                     23
#define MAX_NVME_READ_FAIL_RETRY_COUNT  0x3

#if defined ( MDE_PKG_VERSION ) && (MDE_PKG_VERSION >= 10) \
       && ((defined NVME_DRIVER_VERSION && NVME_DRIVER_VERSION >= 10))
    #define NVME_PASSTHRU_HEADER_FILE "MdePkg/Include/Protocol/NvmExpressPassthru.h"
    #define USE_MDE_PKG_NVME_PASSTHRU_CHANGES 1
#elif (defined NVME_DRIVER_VERSION && NVME_DRIVER_VERSION >= 9)
    #define NVME_PASSTHRU_HEADER_FILE "Protocol/AmiNvmExpressPassThru.h"
#else
    #define NVME_PASSTHRU_HEADER_FILE "AmiModulePkg/Include/Protocol/NvmExpressPassThru.h"
#endif

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
