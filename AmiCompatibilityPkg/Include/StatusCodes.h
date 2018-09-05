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
  Status Code Definitions, according to Intel Platform
		Innovation Framework for EFI Status Codes Specification Revision 0.90

  The code where logically separated into file is parts for a sensible 
  grouping of the values
  Part:    Contents:
    1           General Status Code Definitions --> moved to EFI.h
    2           Class definitions
    3           Computing Unit Subclasses, Progress and Error Codes
    4           Peripheral Subclasses, Progress and Error Codes.
    5           IO Bus Subclasses, Progress and Error Codes.
    6           Software Subclasses, Progress and Error Codes.
    7           Debug Codes
*/

#ifndef _EFI_STATUS_CODE_H_
#define _EFI_STATUS_CODE_H_
#include <Efi.h>
#include <UefiHii.h>
#include <DataHubSubClass.h>

// Include from MdePkg
#include <Guid/StatusCodeDataTypeId.h>
#include <Pi/PiStatusCode.h>
// IntelFrameworkModulePkg
#include <Guid/DataHubStatusCodeRecord.h>
// IntelFrameworkPkg
#include <Framework/StatusCode.h>

// IO Bus Class PCI Subclass Progress Code definitions.
#define EFI_IOB_PCI_PC_BUS_ENUM            (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_IOB_PCI_PC_RES_ALLOC           (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#define EFI_IOB_PCI_PC_HPC_INIT            (EFI_SUBCLASS_SPECIFIC | 0x00000002)

// Software Class PEI Module Subclass Error Code definitions.
#define EFI_SW_PEIM_EC_NO_RECOVERY_CAPSULE (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_SW_PEIM_EC_INVALID_CAPSULE_DESCRIPTOR   \
										   (EFI_SUBCLASS_SPECIFIC | 0x00000001)


///////////////////////////////////////////////////////////////////////////////
// Section 7
///////////////////////////////////////////////////////////////////////////////
//Status Code Data Structures
#pragma pack(push,1)

///////////////////////////////////////////////////////////////////////////////
// AMI-specific operations
///////////////////////////////////////////////////////////////////////////////
//AMI Status Code definitions
#define AMI_STATUS_CODE_CLASS EFI_OEM_SPECIFIC //0x8000
#define AMI_STATUS_CODE_BEEP_CLASS 0x8100

// Host Processor AMI-specific codes
#define AMI_CU_HP_PC_DXE_CPU_INIT (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_CU_HP_EC_DXE_CPU_ERROR (AMI_STATUS_CODE_CLASS | 0x00000001)

//Chipset AMI-specific codes
#define AMI_CHIPSET_PC_PEI_CAR_SB_INIT (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_CHIPSET_PC_PEI_CAR_NB_INIT (AMI_STATUS_CODE_CLASS | 0x00000001)
#define AMI_CHIPSET_PC_PEI_MEM_SB_INIT (AMI_STATUS_CODE_CLASS | 0x00000002)
#define AMI_CHIPSET_PC_PEI_MEM_NB_INIT (AMI_STATUS_CODE_CLASS | 0x00000003)
#define AMI_CHIPSET_PC_DXE_HB_INIT (AMI_STATUS_CODE_CLASS | 0x00000004)
#define AMI_CHIPSET_PC_DXE_NB_INIT (AMI_STATUS_CODE_CLASS | 0x00000005)
#define AMI_CHIPSET_PC_DXE_NB_SMM_INIT (AMI_STATUS_CODE_CLASS | 0x00000006)
#define AMI_CHIPSET_PC_DXE_SB_RT_INIT (AMI_STATUS_CODE_CLASS | 0x00000007)
#define AMI_CHIPSET_PC_DXE_SB_INIT (AMI_STATUS_CODE_CLASS | 0x00000008)
#define AMI_CHIPSET_PC_DXE_SB_SMM_INIT (AMI_STATUS_CODE_CLASS | 0x00000009)
#define AMI_CHIPSET_PC_DXE_SB_DEVICES_INIT (AMI_STATUS_CODE_CLASS | 0x0000000A)
#define AMI_CHIPSET_EC_BAD_BATTERY     (AMI_STATUS_CODE_CLASS | 0x0000000B)
#define AMI_CHIPSET_EC_INTRUDER_DETECT (AMI_STATUS_CODE_CLASS | 0x0000000C)
#define AMI_CHIPSET_EC_DXE_NB_ERROR (AMI_STATUS_CODE_CLASS | 0x0000000D)
#define AMI_CHIPSET_EC_DXE_SB_ERROR (AMI_STATUS_CODE_CLASS | 0x0000000E)
#define AMI_CHIPSET_PC_PEI_CAR_NB2_INIT (AMI_STATUS_CODE_CLASS | 0x0000000F)
#define AMI_CHIPSET_PC_PEI_MEM_NB2_INIT (AMI_STATUS_CODE_CLASS | 0x00000010)
#define AMI_CHIPSET_PC_DXE_NB2_INIT (AMI_STATUS_CODE_CLASS | 0x00000011)

// Keyboard AMI-specific codes
#define AMI_P_KEYBOARD_EC_BUFFER_FULL (AMI_STATUS_CODE_CLASS | 0x00000000)

//PEI Core AMI-specific codes
#define AMI_PEI_CORE_EC_DXEIPL_NOT_FOUND (AMI_STATUS_CODE_CLASS | 0x00000000)

//PEI Services AMI-specific codes
#define AMI_PS_PC_RESET_SYSTEM (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_PS_EC_RESET_NOT_AVAILABLE (AMI_STATUS_CODE_CLASS | 0x00000001)
#define AMI_PS_EC_MEMORY_NOT_INSTALLED (AMI_STATUS_CODE_CLASS | 0x00000002)
#define AMI_PS_EC_MEMORY_INSTALLED_TWICE (AMI_STATUS_CODE_CLASS | 0x00000003)

//PEIM AMI-specific codes
#define AMI_PEIM_PC_S3_STARTED (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_PEIM_PC_S3_BOOT_SCRIPT (AMI_STATUS_CODE_CLASS | 0x00000001)
#define AMI_PEIM_PC_VIDEO_REPOST (AMI_STATUS_CODE_CLASS | 0x00000003)
#define AMI_PEIM_PC_OS_WAKE (AMI_STATUS_CODE_CLASS | 0x00000004)
#define AMI_PEIM_EC_S3_RESUME_PPI_NOT_FOUND (AMI_STATUS_CODE_CLASS | 0x00000005)
#define AMI_PEIM_EC_S3_BOOT_SCRIPT_ERROR (AMI_STATUS_CODE_CLASS | 0x00000006)
#define AMI_PEIM_EC_S3_OS_WAKE_ERROR (AMI_STATUS_CODE_CLASS | 0x00000007)
#define AMI_PEIM_EC_S3_RESUME_ERROR (AMI_STATUS_CODE_CLASS | 0x00000008)
#define AMI_PEIM_EC_RECOVERY_PPI_NOT_FOUND (AMI_STATUS_CODE_CLASS | 0x00000009)
#define AMI_PEIM_EC_RECOVERY_FAILED (AMI_STATUS_CODE_CLASS | 0x0000000A)
#define AMI_PEIM_EC_S3_RESUME_FAILED (AMI_STATUS_CODE_CLASS | 0x0000000B)

//DXE Core AMI-specific codes
#define AMI_DXE_CORE_PC_ARCH_READY (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_DXE_CORE_EC_NO_ARCH (AMI_STATUS_CODE_CLASS | 0x00000001)

//Boot Services AMI-specific codes
#define AMI_BS_PC_NVRAM_INIT (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_BS_PC_NVRAM_GC (AMI_STATUS_CODE_CLASS | 0x00000001)
#define AMI_BS_PC_CONFIG_RESET (AMI_STATUS_CODE_CLASS | 0x00000002)

//DXE Driver AMI-specific codes
#define AMI_DXE_BS_PC_ACPI_INIT (AMI_STATUS_CODE_CLASS | 0x00000000)
#define AMI_DXE_BS_PC_CSM_INIT (AMI_STATUS_CODE_CLASS | 0x00000001)
#define AMI_DXE_BS_EC_INVALID_PASSWORD (AMI_STATUS_CODE_CLASS | 0x00000002)
#define AMI_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR (AMI_STATUS_CODE_CLASS | 0x00000003)
#define AMI_DXE_BS_EC_BOOT_OPTION_FAILED (AMI_STATUS_CODE_CLASS | 0x00000004)
#define AMI_DXE_BS_EC_INVALID_IDE_PASSWORD (AMI_STATUS_CODE_CLASS | 0x00000005)
#define AMI_DXE_BS_EC_MBR_GPT_WRITE_PROTECTED (AMI_STATUS_CODE_CLASS | 0x00000006)

#pragma pack(pop)
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
