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

/** @file NvmeSmm.h
    Header file for the NvmeSmm

**/

#ifndef _AMI_NVME_SMM_DRIVER_H_
#define _AMI_NVME_SMM_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include "Nvme/NvmeIncludes.h"
#include "Nvme/NvmeBus.h"
#include "NvmeDef.h"

typedef VOID        (*API_FUNC)(NVME_STRUC*);

//-----------------------------------------------
//      ERROR CODE REPORTED TO CALLER
//-----------------------------------------------
#define NVME_WRITE_PROTECT_ERR          0x003   // Write protect error
#define NVME_TIME_OUT_ERR               0x080   // Command timed out error
#define NVME_DRIVE_NOT_READY_ERR        0x0AA   // Drive not ready error
#define NVME_DATA_CORRECTED_ERR         0x011   // Data corrected error
#define NVME_PARAMETER_FAILED           0x007   // Bad parameter error
#define NVME_MARK_NOT_FOUND_ERR         0x002   // Address mark not found error
#define NVME_NO_MEDIA_ERR               0x031   // No media in drive
#define NVME_READ_ERR                   0x004   // Read error
#define NVME_WRITE_ERR                  0x005   // Write error
#define NVME_UNCORRECTABLE_ERR          0x010   // Uncorrectable data error
#define NVME_BAD_SECTOR_ERR             0x00A   // Bad sector error
#define NVME_GENERAL_FAILURE            0x020   // Controller general failure


//-----------------------------------------------------------------------;
// NVME_GLOBAL_DATA
//-----------------------------------------------------------------------;
typedef struct{
    UINT32                              TransferBufferAddress;
    UINT8                               NvmeMassEmulationOptionTable[NVMEDEVS_MAX_ENTRIES];
    ACTIVE_NAMESPACE_DATA               NvmeDev[NVMEDEVS_MAX_ENTRIES];
} NVME_GLOBAL_DATA;

//
//NVME Setup fields
//

typedef struct {
    UINT8   NvmeMode;
    UINT8   NvmeEmu1;
    UINT8   NvmeEmu2;
    UINT8   NvmeEmu3;
    UINT8   NvmeEmu4;
    UINT8   NvmeEmu5;
    UINT8   NvmeEmu6;
    UINT8   NvmeEmu7;
    UINT8   NvmeEmu8;
	UINT8	NvmeMassDevNum;
} NVME_DEV_CONFIGURATION;


EFI_STATUS
NotInSmmFunction (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
);

EFI_STATUS
NvmeInSmmFunction (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
);

EFI_STATUS
EFIAPI
NvmeSWSMIHandler (
    IN  EFI_HANDLE      DispatchHandle,
    IN  CONST   VOID    *DispatchContext OPTIONAL,
    IN  OUT     VOID    *CommBuffer OPTIONAL,
    IN  OUT     UINTN   *CommBufferSize OPTIONAL
);

VOID
ZeroMemorySmm (
    VOID    *Buffer,
    UINTN   Size
 );

VOID
NvmeMassAPIGetDeviceInformation (
    NVME_STRUC  *NvmeURP
);

VOID
NvmeMassAPIGetDeviceGeometry (
    NVME_STRUC  *NvmeURP
);

VOID
NvmeMassAPIResetDevice (
    NVME_STRUC  *NvmeURP
);

VOID
NvmeMassAPIReadDevice (
    NVME_STRUC  *NvmeURP
);

VOID
NvmeMassAPIWriteDevice (
    NVME_STRUC  *NvmeURP
);

VOID
NvmeMassAPINotSupported (
    NVME_STRUC  *NvmeURP
);

VOID
NvmeMassAPIPass(
    NVME_STRUC *NvmeURP
);

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
