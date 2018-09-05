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


/** @file StatusCodeMapPei.c
    This file contains several tables that map generic namespace
    of status code values into domain specific namespaces.
    In the current implementation the following tables are included:
    ProgressCheckpointMap - establishes mapping between progress code values and checkpoint values.
    ErrorCheckpointMap - establishes mapping between error code values and checkpoint values.
    ProgressBeepMap - establishes mapping between progress code values and number of beeps.
    ErrorBeepMap - establishes mapping between error code values and number of beeps.
    The mapping tables can be overriden using SDL tokens.
**/
//**********************************************************************
#include <AmiStatusCodes.h>
#include "StatusCodeInt.h"

//**********************************************************************
// Checkpoints map
//**********************************************************************
//1    - 0xF : SEC execution
//0x10 - 0x2F: PEI CAR execution
//0x30 - 0x4F: PEI execution after memory detection
//0x50 - 0x5F: PEI errors
//0x60 - 0xCF: DXE execution
//0xD0 - 0xDF: DXE errors
//0xE0 - 0xE8: S3 Resume
//0xE9 - 0xEF: S3 Resume errors
//0xF0 - 0xF8: Recovery
//0xF9 - 0xFF: Recovery errors

STATUS_CODE_TO_BYTE_MAP ProgressCheckpointMap[] =
{

//Regular boot
    { PEI_CORE_STARTED, 0x10 },
    { PEI_CAR_CPU_INIT, 0x11 },
    // reserved for CPU 0x12 - 0x14
    { PEI_CAR_NB_INIT, 0x15 },
    // reserved for NB 0x16 - 0x18
    { PEI_CAR_SB_INIT, 0x19 },
    // reserved for SB 0x1A - 0x1C
    // reserved for OEM use: 0x1D - 0x2A
    { PEI_MEMORY_SPD_READ, 0x2B },
    { PEI_MEMORY_PRESENCE_DETECT, 0x2C },
    { PEI_MEMORY_TIMING, 0x2D},
    { PEI_MEMORY_CONFIGURING, 0x2E },
    { PEI_MEMORY_INIT, 0x2F },
    // reserved for AML use: 0x30
    { PEI_MEMORY_INSTALLED, 0x31 },
    { PEI_CPU_INIT,  0x32 },
    { PEI_CPU_CACHE_INIT, 0x33 },
    { PEI_CPU_AP_INIT, 0x34 },
    { PEI_CPU_BSP_SELECT, 0x35 },
    { PEI_CPU_SMM_INIT, 0x36 },
    { PEI_MEM_NB_INIT, 0x37 },
    // reserved for NB 0x38 - 0x3A
    { PEI_MEM_SB_INIT, 0x3B },
    // reserved for SB 0x3C - 0x3E
    // reserved for OEM use: 0x3F - 0x4E
    { PEI_DXE_IPL_STARTED, 0x4F },
    { DXE_CORE_STARTED, 0x60 },
//Recovery
    { PEI_RECOVERY_AUTO, 0xF0 },
    { PEI_RECOVERY_USER, 0xF1 },
    { PEI_RECOVERY_STARTED, 0xF2 },
    { PEI_RECOVERY_CAPSULE_FOUND, 0xF3 },
    { PEI_RECOVERY_CAPSULE_LOADED, 0xF4 },
//S3
    { PEI_S3_STARTED, 0xE0 },
    { PEI_S3_BOOT_SCRIPT, 0xE1 },
    { PEI_S3_VIDEO_REPOST, 0xE2 },
    { PEI_S3_OS_WAKE, 0xE3 },

    {0,0}
};

STATUS_CODE_TO_BYTE_MAP ErrorCheckpointMap[] =
{

//Errors
//Regular boot
    { PEI_MEMORY_INVALID_TYPE, 0x50 },
    { PEI_MEMORY_INVALID_SPEED, 0x50 },
    { PEI_MEMORY_SPD_FAIL, 0x51 },
    { PEI_MEMORY_INVALID_SIZE, 0x52 },
    { PEI_MEMORY_MISMATCH, 0x52 },
    { PEI_MEMORY_NOT_DETECTED, 0x53 },
    { PEI_MEMORY_NONE_USEFUL, 0x53 },
    { PEI_MEMORY_ERROR, 0x54 },
    { PEI_MEMORY_NOT_INSTALLED, 0x55 },
    { PEI_CPU_INVALID_TYPE, 0x56 },
    { PEI_CPU_INVALID_SPEED, 0x56 },
    { PEI_CPU_MISMATCH, 0x57 },
    { PEI_CPU_SELF_TEST_FAILED, 0x58 },
    { PEI_CPU_CACHE_ERROR, 0x58 },
    { PEI_CPU_MICROCODE_UPDATE_FAILED, 0x59 },
    { PEI_CPU_NO_MICROCODE, 0x59 },
    { PEI_CPU_INTERNAL_ERROR, 0x5A },
    { PEI_CPU_ERROR, 0x5A },
    { PEI_RESET_NOT_AVAILABLE,0x5B },
    //reserved for AMI use: 0x5C - 0x5F
//Recovery
    { PEI_RECOVERY_PPI_NOT_FOUND, 0xF8 },
    { PEI_RECOVERY_NO_CAPSULE, 0xF9 },
    { PEI_RECOVERY_INVALID_CAPSULE, 0xFA },
    //reserved for AMI use: 0xFB - 0xFF
//S3 Resume
    { PEI_MEMORY_S3_RESUME_FAILED, 0xE8 },
    { PEI_S3_RESUME_PPI_NOT_FOUND, 0xE9 },
    { PEI_S3_BOOT_SCRIPT_ERROR, 0xEA },
    { PEI_S3_OS_WAKE_ERROR, 0xEB },
    //reserved for AMI use: 0xEC - 0xEF

    {0,0}
};

//**********************************************************************
// Beep codes map
//**********************************************************************
STATUS_CODE_TO_BYTE_MAP ProgressBeepMap[] =
{

//Recovery
    { PEI_RECOVERY_STARTED, 2 },

    {0,0}
};

STATUS_CODE_TO_BYTE_MAP ErrorBeepMap[] =
{

//Errors
//Regular boot
    { PEI_MEMORY_NOT_INSTALLED, 1 },
    { PEI_MEMORY_INSTALLED_TWICE, 1 },
    { PEI_DXEIPL_NOT_FOUND, 3 },
    { PEI_DXE_CORE_NOT_FOUND, 3 },
    { PEI_RESET_NOT_AVAILABLE, 7 },
//Recovery
    { PEI_RECOVERY_FAILED, 4 },
//S3 Resume
    { PEI_S3_RESUME_FAILED, 4 },

    {0,0}
};
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
