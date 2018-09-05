//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file SmmHddSecurity.h
    This file contains the Includes, Definitions, typedefs,
    Variable and External Declarations, Structure and
    function prototypes needed for the IDESMM Component

**/

#ifndef _HDD_SECURITY_SMM_H_
#define _HDD_SECURITY_SMM_H_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Token.h>
#include "HddSecurity.h"
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
#include "Nvme/NvmeController.h"
#include <Protocol/AmiNvmeController.h>
#include <Protocol/AmiNvmePassThru.h>
#endif
#include <Library/AmiBufferValidationLib.h>
#include <Library/S3BootScriptLib.h>

//---------------------------------------------------------------------------

extern EFI_BOOT_SERVICES             *pBS;
extern EFI_GUID                      gAmiSmmNvmePassThruProtocolGuid;
extern EFI_GUID                      gAmiSmmNvmeControllerProtocolGuid;
extern EFI_GUID                      gIdeSecurityInterfaceGuid;
extern EFI_GUID                      gEfiSmmSwDispatch2ProtocolGuid;
extern EFI_GUID                      gEfiSmmBase2ProtocolGuid;
extern EFI_SMM_BASE2_PROTOCOL        *pSmmBase;
extern EFI_SMM_SYSTEM_TABLE2         *pSmst;
DLIST                                gPasswordList;

#ifndef DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT
#define     DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT  16000           // 16Sec
#endif

#ifndef S3_BUSY_CLEAR_TIMEOUT
#define     S3_BUSY_CLEAR_TIMEOUT               10000           // 10Sec
#endif

#ifndef BUSY_CLEAR_TIMEOUT
#define     BUSY_CLEAR_TIMEOUT                  1000            // 1Sec
#endif

#ifndef DRDY_TIMEOUT
#define     DRDY_TIMEOUT                        1000            // 1Sec
#endif

#ifndef DRQ_TIMEOUT
#define     DRQ_TIMEOUT                         10              // 10msec
#endif

#ifndef HBA_CAP
#define     HBA_CAP                             0x0000
#endif

#ifndef HBA_CAP_NP_MASK
#define     HBA_CAP_NP_MASK                     0x1F
#endif

#ifndef HBA_PORTS_START
#define     HBA_PORTS_START                     0x0100
#endif

#ifndef HBA_PORTS_REG_WIDTH
#define     HBA_PORTS_REG_WIDTH                 0x0080
#endif

#pragma pack(1)

typedef struct
{
    UINT8 bFeature;
    UINT8 bSectorCount;
    UINT8 bLbaLow;
    UINT8 bLbaMid;
    UINT8 bLbaHigh;
    UINT8 bDevice;
    UINT8 bCommand;
} COMMAND_BUFFER;


#pragma pack()

EFI_STATUS
InSmmFunction (
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
);

EFI_STATUS
InstallSmiHandler (
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
);

EFI_STATUS
SMMSecurityUnlockCommand (
    HDD_PASSWORD            *pHddPassword
);

EFI_STATUS
SMMIdeNonDataCommand (
    HDD_PASSWORD    *pHddPassword,
    IN  UINT8       Features,
    IN  UINT8       SectorCount,
    IN  UINT8       LBALow,
    IN  UINT8       LBAMid,
    IN  UINT8       LBAHigh,
    IN  UINT8       Command
);

void
ZeroMemorySmm (
    void    *Buffer,
    UINTN   Size
);

static
UINT64 DivU64x32Local (
    IN  UINT64              Dividend,
    IN  UINTN               Divisor,
    OUT UINTN*Remainder     OPTIONAL
);

VOID *
EFIAPI
CopyMem (
    OUT VOID            *DestinationBuffer,
    IN  CONST VOID      *SourceBuffer,
    IN  UINTN           Length
);

EFI_STATUS
SMMNvmeSecurityUnlockCommand (
    HDD_PASSWORD    *pHddPassword
);

EFI_STATUS
SMMUnlockNvmeDevice (
    HDD_PASSWORD    *pHddPassword
);

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
EFI_STATUS
GetNvmeDeviceSecurityStatus(
    AMI_NVME_CONTROLLER_PROTOCOL  *gSmmNvmeController,
    UINT16                       *SecurityStatus 
);

EFI_STATUS
SendNvmeSecurityUnlockCommand (
    AMI_NVME_CONTROLLER_PROTOCOL    *gSmmNvmeController,
    HDD_PASSWORD                    *pHddPassword
);
#endif

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif  

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
