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

/** @file BiosGuardPeiApRecoveryCapsule.h
    This file is used for BIOS Guard flash tool executed "/recovery" and "/capsule".

**/

#ifndef _BIOS_GUARD_PEI_AP_RECOVERY_CAPSULE_H_
#define _BIOS_GUARD_PEI_AP_RECOVERY_CAPSULE_H_

#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <AmiHobs.h>
#include <Capsule.h>
#include <Guid/CapsuleVendor.h>
#include <BiosGuardDefinitions.h>
#include "AmiCertificate.h"
#include <FlashUpd.h>
#include <CpuRegs.h>
#include <Guid/SetupVariable.h>
#include <token.h>

#pragma pack (1)
typedef struct {
    //
    // AmiBiosGuardRecoveryAddress for Tool Interface
    //
    UINT64                      Signature;                  // _AMIBSGD
    UINT16                      Version;                    // 0
    UINT16                      ReoveryMode;                // 1: Recovery, 2: Capsule
    UINT32                      ReoveryAddress;             // AP pointer;
} AMI_BIOS_GUARD_RECOVERY;
#pragma pack ()

typedef struct {
    EFI_FFS_FILE_HEADER         FfsHdr;
    EFI_COMMON_SECTION_HEADER   SecHdr;
    EFI_GUID                    SectionGuid;
    UINT8                       FwCapHdr[0];
} AMI_FFS_COMMON_SECTION_HEADER;

typedef enum
{
    BiosGuardRecoveryMode = 1,
    BiosGuardCapsuleMode = 2
} BIOS_GUARD_RECOVERY_TYPE;

#define AMI_PEI_AFTER_MRC_GUID \
 {0x64c96700, 0x6b4c, 0x480c, 0xa3, 0xe1, 0xb8, 0xbd, 0xe8, 0xf6, 0x2, 0xb2}
EFI_GUID gAmiPeiAfterMrcGuid      = AMI_PEI_AFTER_MRC_GUID;
/*#define AMI_PEI_END_OF_MEM_DETECT_GUID \
 {0x633194be, 0x1697, 0x11e1, 0xb5, 0xf0, 0x2c, 0xb2, 0x48, 0x24, 0x01, 0x9b}
EFI_GUID gAmiPeiEndOfMemDetectGuid      = AMI_PEI_END_OF_MEM_DETECT_GUID;*/
EFI_GUID FWCapsuleGuid                     = APTIO_FW_CAPSULE_GUID;
EFI_GUID BiosGuardRecoveryGuid             = BIOS_GUARD_RECOVERY_GUID;
static EFI_GUID FwCapFfsGuid               = AMI_FW_CAPSULE_FFS_GUID;
static EFI_GUID FwCapSectionGuid           = AMI_FW_CAPSULE_SECTION_GUID;
extern EFI_GUID gFlashUpdBootModePpiGuid;

#define BIOSGUARD_PACKAGE_CERT   524
#define Disable  0
#define Enable   1

EFI_STATUS
BiosGuardCapsuleChangeBootModeAfterEndofMrc (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
    IN VOID                               *InvokePpi
);

EFI_STATUS
BiosGuardGetCapsuleImage (
    IN EFI_PEI_SERVICES                  **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
    IN VOID                              *InvokePpi
);

static EFI_PEI_PPI_DESCRIPTOR FlashUpdateModePpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gFlashUpdBootModePpiGuid, NULL
};

static EFI_PEI_PPI_DESCRIPTOR RecoveryModePpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiBootInRecoveryModePpiGuid, NULL
};

static EFI_PEI_NOTIFY_DESCRIPTOR BiosGuardCapsuleChangeBootModeAfterEndofMrcNotifyList[] = {
    {   EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gAmiPeiAfterMrcGuid,
        BiosGuardCapsuleChangeBootModeAfterEndofMrc
    },
};

/*static EFI_PEI_NOTIFY_DESCRIPTOR BiosGuardGetCapsuleImageNotifyList[] = {
    {   EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gAmiPeiEndOfMemDetectGuid,
        BiosGuardGetCapsuleImage
    },
};*/

static EFI_PEI_NOTIFY_DESCRIPTOR BiosGuardGetCapsuleImageNotifyList[] = {
  {    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
       &gMemoryInitDoneSentGuid,
       BiosGuardGetCapsuleImage
  },
};

#endif
