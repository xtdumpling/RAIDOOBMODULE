//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file  AmiTextOutStatusCodeList.h
    Declares structures, progress code , error code, progress code string and error code string
    used for by PEI and DXE status code driver 

**/

#ifndef __PROGREE_ERROR_CODE_H__
#define __PROGREE_ERROR_CODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Token.h>
#include <AmiStatusCodes.h>
#include <OemProgressErrorCode.h>

typedef struct {
    EFI_STATUS_CODE_VALUE   Value;   /// Status code value.
    UINT8                   PostCode;   /// Equivalent POST code
} STATUS_CODE_TO_CHECK_POINT;

typedef struct {
    EFI_STATUS_CODE_VALUE   Value;  /// Status code value.
    CHAR8                   *PostString; /// Equivalent String
} PROGRESS_CODE_STRING;

typedef struct {
    EFI_STATUS_CODE_VALUE   Value;  /// Status code value.
    CHAR8                   *ErrorString; /// Equivalent String
    CHAR8                   *PosibileCauseString;
    CHAR8                   *PosibibleSoutionString;
} ERROR_CODE_STRING;


//**********************************************************************
// Progress Code Checkpoints map
//**********************************************************************
//1    - 0xF : SEC execution
//0x10 - 0x2F: PEI CAR execution
//0x30 - 0x4F: PEI execution after memory detection
//0x60 - 0xCF: DXE execution
//0xE0 - 0xE8: S3 Resume
//0xF0 - 0xF8: Recovery

STATUS_CODE_TO_CHECK_POINT ProgressCheckpointMap[] = 
{
//Regular boot
    OEM_PROGRESS_CODE_LIST		
    { PEI_CORE_STARTED, 0x10 },
    { PEI_CAR_CPU_INIT, 0x11 },
    //reserved for CPU 0x12 - 0x14
    { PEI_CAR_NB_INIT,  0x15 },
    //reserved for NB 0x16 - 0x18
    { PEI_CAR_SB_INIT,  0x19 },
    //reserved for SB 0x1A - 0x1C
    //reserved for OEM use: 0x1D - 0x2A
    { PEI_MEMORY_SPD_READ,  0x2B },
    { PEI_MEMORY_PRESENCE_DETECT,  0x2C },
    { PEI_MEMORY_TIMING, 0xB2},
    { PEI_MEMORY_CONFIGURING, 0xB7 },
    { PEI_MEMORY_OPTIMIZING, 0xB9 },
    { PEI_MEMORY_INIT, 0x2F },
    
    // reserved for AML use: 0x30
    
    { PEI_MEMORY_INSTALLED,  0x31 },
    { PEI_CPU_INIT,  0x32 },
    { PEI_CPU_CACHE_INIT, 0x33 },
    { PEI_CPU_AP_INIT,  0x34 },
    { PEI_CPU_BSP_SELECT,  0x35 },
    { PEI_CPU_SMM_INIT,  0x36 },
    { PEI_MEM_NB_INIT,  0x37 },
    //reserved for NB 0x38 - 0x3A
    
    { PEI_MEM_SB_INIT,    0x3B },
    
    //reserved for SB 0x3C - 0x3E
    //reserved for OEM use: 0x3F - 0x4E
    
    { PEI_DXE_IPL_STARTED,    0x4F },
    
//Recovery
    { PEI_RECOVERY_AUTO,    0xF0 },
    { PEI_RECOVERY_USER,    0xF1 },
    { PEI_RECOVERY_STARTED,    0xF2 },
    { PEI_RECOVERY_CAPSULE_FOUND,    0xF3 },
    { PEI_RECOVERY_CAPSULE_LOADED,    0xF4 },
    
//S3
    { PEI_S3_STARTED,    0xE0 },
    { PEI_S3_BOOT_SCRIPT,    0xE1 },
    { PEI_S3_VIDEO_REPOST,    0xE2 },
    { PEI_S3_OS_WAKE,    0xE3 },
    
//DXE    
    { DXE_CORE_STARTED,   0x60 },
    { DXE_NVRAM_INIT,    0x61 },
    { DXE_SBRUN_INIT,    0x62 },
    { DXE_CPU_INIT,    0x63 },
    //reserved for CPU 0x64 - 0x67
    { DXE_NB_HB_INIT,    0x68 },
    { DXE_NB_INIT,    0x69 },
    { DXE_NB_SMM_INIT,    0x6A },
    //reserved for NB 0x6B - 0x6F
    { DXE_SB_INIT,    0x70 },
    { DXE_SB_SMM_INIT,   0x71 },
    { DXE_SB_DEVICES_INIT,   0x72 },
    //reserved for SB 0x73 - 0x77
    { DXE_ACPI_INIT,   0x78 },
    { DXE_CSM_INIT,   0x79 },
    //reserved for AMI use: 0x7A - 0x7F
    //reserved for OEM use: 0x80 - 0x8F
    { DXE_BDS_STARTED,   0x90 },
    { DXE_BDS_CONNECT_DRIVERS,   0x91 },
    { DXE_PCI_BUS_BEGIN,   0x92 },
    { DXE_PCI_BUS_HPC_INIT,   0x93 },
    { DXE_PCI_BUS_ENUM,   0x94 },
    { DXE_PCI_BUS_REQUEST_RESOURCES,   0x95 },
    { DXE_PCI_BUS_ASSIGN_RESOURCES,   0x96 },
    { DXE_CON_OUT_CONNECT,   0x97 },
    { DXE_CON_IN_CONNECT,   0x98 },
    { DXE_SIO_INIT,   0x99 },
    { DXE_USB_BEGIN,   0x9A },
    { DXE_USB_RESET,   0x9B },
    { DXE_USB_DETECT,   0x9C },
    { DXE_USB_ENABLE,   0x9D },
    //reserved for AMI use: 0x9E - 0x9F
    { DXE_IDE_BEGIN,   0xA0 },
    { DXE_IDE_RESET,   0xA1 },
    { DXE_IDE_DETECT,   0xA2 },
    { DXE_IDE_ENABLE,   0xA3 },
    { DXE_SCSI_BEGIN,   0xA4 },
    { DXE_SCSI_RESET,   0xA5 },
    { DXE_SCSI_DETECT,   0xA6 },
    { DXE_SCSI_ENABLE,   0xA7 },
    { DXE_SETUP_VERIFYING_PASSWORD,   0xA8 },
    { DXE_SETUP_START,   0xA9 },
    //reserved for AML use: 0xAA
    { DXE_SETUP_INPUT_WAIT,   0xAB },
    //reserved for AML use: 0xAC
    { DXE_READY_TO_BOOT,   0xAD },
    { DXE_LEGACY_BOOT,   0xAE },
    { DXE_EXIT_BOOT_SERVICES,   0xAF },
    { RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN,   0xB0 },
    { RT_SET_VIRTUAL_ADDRESS_MAP_END,   0xB1 },
    { DXE_LEGACY_OPROM_INIT,   0xB2 },
    { DXE_RESET_SYSTEM,   0xB3 },
    { DXE_USB_HOTPLUG,   0xB4 },
    { DXE_PCI_BUS_HOTPLUG,   0xB5 },
    { DXE_NVRAM_CLEANUP,   0xB6 },
    { DXE_CONFIGURATION_RESET,   0xB7 },
    //reserved for AMI use: 0xB8 - 0xBF
    //reserved for OEM use: 0xC0 - 0xCF
    {0,0}
};

//**********************************************************************
// Error Code Checkpoints map
//**********************************************************************
//0x50 - 0x5F: PEI errors
//0xD0 - 0xDF: DXE errors
//0xE9 - 0xEF: S3 Resume errors
//0xF9 - 0xFF: Recovery errors

STATUS_CODE_TO_CHECK_POINT ErrorCodeCheckpointMap[] = 
{
//PEI Error 
    OEM_ERROR_CODE_LIST		
    { PEI_MEMORY_INVALID_TYPE,  0x50 },
    { PEI_MEMORY_INVALID_SPEED,  0x50 },
    { PEI_MEMORY_SPD_FAIL,  0x51 },
    { PEI_MEMORY_INVALID_SIZE,  0x52 },
    { PEI_MEMORY_MISMATCH,  0x52 },
    { PEI_MEMORY_NOT_DETECTED,  0x53 },
    { PEI_MEMORY_NONE_USEFUL,  0x53 },
    { PEI_MEMORY_ERROR,  0x54 },
    { PEI_MEMORY_NOT_INSTALLED,  0x55 },
    { PEI_CPU_INVALID_TYPE,  0x56 },
    { PEI_CPU_INVALID_SPEED,  0x56 },
    { PEI_CPU_MISMATCH,  0x57 },
    { PEI_CPU_SELF_TEST_FAILED,  0x58 },
    { PEI_CPU_CACHE_ERROR,  0x58 },
    { PEI_CPU_MICROCODE_UPDATE_FAILED,  0x59 },
    { PEI_CPU_NO_MICROCODE,  0x59 },
    { PEI_CPU_INTERNAL_ERROR,  0x5A },
    { PEI_CPU_ERROR,  0x5A },
    { PEI_RESET_NOT_AVAILABLE,  0x5B },
    
//DXE Error     
    //reserved for AMI use: 0x5C - 0x5F
    { DXE_PCI_BUS_OUT_OF_RESOURCES, 0xD4 },
    
//Recovery Error
    { PEI_RECOVERY_PPI_NOT_FOUND,  0xF8 },
    { PEI_RECOVERY_NO_CAPSULE,  0xF9 },
    { PEI_RECOVERY_INVALID_CAPSULE,  0xFA },
    //reserved for AMI use: 0xFB - 0xFF
    
//S3 Resume Error
    { PEI_MEMORY_S3_RESUME_FAILED,  0xE8 },
    { PEI_S3_RESUME_PPI_NOT_FOUND,  0xE9 },
    { PEI_S3_BOOT_SCRIPT_ERROR,  0xEA },
    { PEI_S3_OS_WAKE_ERROR,  0xEB },
    //reserved for AMI use: 0xEC - 0xEF
    {0,0}
};

PROGRESS_CODE_STRING ProgressCodeVideoStrings[] = {
    OEM_PROGRESS_CODE_TO_STRING        
     // PEI Progress Code String 
    { PEI_CORE_STARTED, "PEI Core Started." },
    { PEI_CAR_CPU_INIT, "Pre-memory CPU initialization is started." },
    // reserved for CPU 0x12 - 0x14
    { PEI_CAR_NB_INIT, "Pre-memory NB Initialization." },
    // reserved for NB 0x16 - 0x18
    { PEI_CAR_SB_INIT, "Pre-memory SB Initialization." },
    // reserved for SB 0x1A - 0x1C
    // reserved for OEM use: 0x1D - 0x2A
    { PEI_MEMORY_SPD_READ, "Memory Initialization - SPD Read." },
    { PEI_MEMORY_PRESENCE_DETECT, "Memory presence detection" },
    { PEI_MEMORY_INIT, "Memory Initialization Start"     }, 
    { PEI_MEMORY_TIMING, "Gather Remaining SPD Data"  }, 
    { PEI_MEMORY_CONFIGURING, "Train DDR"   }, 
    { PEI_MEMORY_OPTIMIZING, "Hardware Memory Test and Initialization"   }, 
    
    // reserved for AML use: 0x30
    { PEI_MEMORY_INSTALLED, "Memory Initialization Complete"     }, 
    { PEI_CPU_INIT, "CPU POST-Memory Initialization"     }, 
    { PEI_CPU_CACHE_INIT, "CPU Cache initialization"     }, 
    { PEI_CPU_AP_INIT, "Application Processor(s) (AP) initialization"     },
    { PEI_CPU_BSP_SELECT, "BSP Selection"     },
    { PEI_CPU_SMM_INIT, "SMM Initialization."  },
    { PEI_MEM_NB_INIT, "POST-Memory NB Initialization."  },
    // reserved for NB 0x38 - 0x3A
    { PEI_MEM_SB_INIT,   "POST-Memory SB Initialization."  },
    // reserved for SB 0x3C - 0x3E
    // reserved for OEM use: 0x3F - 0x4E
    { PEI_DXE_IPL_STARTED,   "DXE IPL Start"     },
    
    //Recovery Progress Code
    { PEI_RECOVERY_AUTO,   "Recovery - By Firmware."  },
    { PEI_RECOVERY_USER,   "Recovery - By User."  },
    { PEI_RECOVERY_STARTED,   "Recovery Started." },
    { PEI_RECOVERY_CAPSULE_FOUND,   "Recovery Capsule Found."  },
    { PEI_RECOVERY_CAPSULE_LOADED,   "Recovery Capsule Loaded."  },
    
    //S3 Progress Code
    { PEI_S3_STARTED,   "S3 Resume Started."  },
    { PEI_S3_BOOT_SCRIPT,   "S3 Boot Script Execution."  },
    { PEI_S3_VIDEO_REPOST,   "Video Repost."  },
    { PEI_S3_OS_WAKE,   "OS S3 wake vector call."  },
    
    // DXE Progress Code 
    { DXE_CORE_STARTED,   "DXE Core Started."  },
    { DXE_NVRAM_INIT,   "NVRAM Initialization."  },
    { DXE_SBRUN_INIT,   "Install SB Runtime."  },
    { DXE_CPU_INIT,   "CPU DXE Initialization."  },
    //reserved for CPU 0x64 - 0x67
    { DXE_NB_HB_INIT,   "PCI HB Initialization."  },
    { DXE_NB_INIT,   "NB DXE Initialization."  },
    { DXE_NB_SMM_INIT,   "NB DXE SMM Initialization."  },
    //reserved for NB 0x6B - 0x6F
    { DXE_SB_INIT,   "SB DXE Initialization."  },
    { DXE_SB_SMM_INIT,   "SB DXE SMM Initialization."  },
    { DXE_SB_DEVICES_INIT,   "SB DEVICES Initialization."  },
    //reserved for SB 0x73 - 0x77
    { DXE_ACPI_INIT,   "ACPI Module Initialization."  },
    { DXE_CSM_INIT,   "CSM Driver Entry point"     },
    //reserved for AMI use: 0x7A - 0x7F
    //reserved for OEM use: 0x80 - 0x8F
    { DXE_BDS_STARTED,   "BDS Started."  },
    { DXE_BDS_CONNECT_DRIVERS,   "Connecting Drivers."  },
    { DXE_PCI_BUS_BEGIN,   "PCI Bus Initialization."  },
    { DXE_PCI_BUS_HPC_INIT,   "PCI Bus Hot Plug Controller Initialization."  },
    { DXE_PCI_BUS_ENUM,   "PCI Bus Enumeration."  },
    { DXE_PCI_BUS_REQUEST_RESOURCES,   "PCI Bus Request Resources."  },
    { DXE_PCI_BUS_ASSIGN_RESOURCES,   "PCI Bus Assign Resources."  },
    { DXE_CON_OUT_CONNECT,   "Console Output devices connect."  },
    { DXE_CON_IN_CONNECT,   "Console input devices connect."  },
    { DXE_SIO_INIT,   "SIO Initialization."  },
    { DXE_USB_BEGIN,   "USB Initialization."  },
    { DXE_USB_RESET,   "USB Reset."  },
    { DXE_USB_DETECT,   "USB Detect."  },
    { DXE_USB_ENABLE,   "USB Enable."  },
    //reserved for AMI use: 0x9E - 0x9F
    { DXE_IDE_BEGIN,   "IDE initialization is started."  },
    { DXE_IDE_RESET,   "IDE Reset."  },
    { DXE_IDE_DETECT,   "IDE Detect."  },
    { DXE_IDE_ENABLE,   "IDE Enable."  },
    { DXE_SCSI_BEGIN,   "SCSI Initialization."  },
    { DXE_SCSI_RESET,   "SCSI Reset."  },
    { DXE_SCSI_DETECT,   "SCSI Detect."  },
    { DXE_SCSI_ENABLE,   "SCSI Enable."  },
    { DXE_SETUP_VERIFYING_PASSWORD,    "Setup Verifying Password." },
    { DXE_SETUP_START,    "Start of Setup." },
    //reserved for AML use: 0xAA
    { DXE_SETUP_INPUT_WAIT,    "Setup Key Press Wait."  },
    //reserved for AML use: 0xAC
    { DXE_READY_TO_BOOT,   "Ready to Boot."  },
    { DXE_LEGACY_BOOT,     "Legacy Boot event." },
    { DXE_EXIT_BOOT_SERVICES,   "Exiting Boot Services. "  },
    { RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN,   "Virtual Address Begin."  },
    { RT_SET_VIRTUAL_ADDRESS_MAP_END,   "Virtual Address End."  },
    { DXE_LEGACY_OPROM_INIT,    "Legacy Option ROM Initialization." },
    { DXE_RESET_SYSTEM,    "System Reset Initiated."  },
    { DXE_USB_HOTPLUG,   "USB Hot Plug."  },
    { DXE_PCI_BUS_HOTPLUG,   "PCI Bus Hot Plug."  },
    { DXE_NVRAM_CLEANUP,    "NVRAM Clean Up."  },
    { DXE_CONFIGURATION_RESET,   "Configuration Reset."  },
    //reserved for AMI use: 0xB8 - 0xBF
    //reserved for OEM use: 0xC0 - 0xCF
    {0,0}

};

ERROR_CODE_STRING ErrorCodeVideoStrings[] = {
    // PEI Error     
    OEM_ERROR_CODE_TO_STRING        
    { PEI_MEMORY_SPD_FAIL,  "SPD Reading Failed.", NULL, NULL },
    { PEI_MEMORY_NOT_DETECTED,  "Memory Not Detected / Not Use Full.", "Possible Cause: No Memory Found", "Solution: Insert dimms or replace the existing ones" },
    { PEI_MEMORY_ERROR,  "Hardware Memory test Failure (SIMULATED)", "Cause: Memory Test Failure", "Solution: Replace DIMM in the slot 6 under CPU 0." },
    { PEI_MEMORY_NOT_INSTALLED,  "Memory Not Installed.", NULL, NULL },
    { PEI_CPU_INVALID_TYPE,  "Invalid CPU Type / Speed.", NULL, NULL },
    { PEI_CPU_MISMATCH,  "CPU Mismatch (SIMULATED)", "Possible Cause: Cbo Count/List mismatch.", "Solution: Populate CPUs with same Cbo Count." },
    { PEI_CPU_SELF_TEST_FAILED,  "CPU Self Test Failed / Cache Error.", NULL, NULL },
    { PEI_CPU_MICROCODE_UPDATE_FAILED,  "No MicroCode / MicroCode Update Failed.", NULL, NULL },
    { PEI_CPU_INTERNAL_ERROR,  "CPU Internal Error.", NULL, NULL },
    { PEI_RESET_NOT_AVAILABLE, "Reset PPI not Available.", NULL, NULL },
    //reserved for AMI use: 0x5C - 0x5F
    
    //Recovery Error    
    { DXE_PCI_BUS_OUT_OF_RESOURCES,  "Insufficient PCI Resources error", "Cause: Insufficient PCI MMIO Resource", "Solution: Need to remove PCI card" },
    { PEI_RECOVERY_PPI_NOT_FOUND,  "Recover PPI Not Found.", NULL, NULL },
    { PEI_RECOVERY_NO_CAPSULE,  "Recovery Capsule Not Found.", NULL, NULL },
    { PEI_RECOVERY_INVALID_CAPSULE,  "Invalid Recovery Capsule.", NULL, NULL },
    //reserved for AMI use: 0xFB - 0xFF
    
    //S3 Resume Error
    { PEI_MEMORY_S3_RESUME_FAILED,  "S3 Resume Failed.", NULL, NULL },
    { PEI_S3_RESUME_PPI_NOT_FOUND,  "S3 Resume PPI Not Found.", NULL, NULL },
    { PEI_S3_BOOT_SCRIPT_ERROR,  "S3 Resume Boot Script Error.", NULL, NULL },
    { PEI_S3_OS_WAKE_ERROR,   "OS S3 Wake Up Error.", NULL, NULL },
    {0,0,0,0}
};

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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
