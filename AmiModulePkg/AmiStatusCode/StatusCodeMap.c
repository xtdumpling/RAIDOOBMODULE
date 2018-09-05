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


/** @file StatusCodeMap.c
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
    { DXE_CORE_STARTED, 0x60 },
    { DXE_NVRAM_INIT, 0x61 },
    { DXE_SBRUN_INIT, 0x62 },
    { DXE_CPU_INIT, 0x63 },
    //reserved for CPU 0x64 - 0x67
    { DXE_NB_HB_INIT, 0x68 },
    { DXE_NB_INIT, 0x69 },
    { DXE_NB_SMM_INIT, 0x6A },
    //reserved for NB 0x6B - 0x6F
    { DXE_SB_INIT, 0x70 },
    { DXE_SB_SMM_INIT, 0x71 },
    { DXE_SB_DEVICES_INIT, 0x72 },
    //reserved for SB 0x73 - 0x77
    { DXE_ACPI_INIT, 0x78 },
    { DXE_CSM_INIT, 0x79 },
    //reserved for AMI use: 0x7A - 0x7F
    //reserved for OEM use: 0x80 - 0x8F
    { DXE_BDS_STARTED, 0x90 },
    { DXE_BDS_CONNECT_DRIVERS, 0x91 },
    { DXE_PCI_BUS_BEGIN, 0x92 },
    { DXE_PCI_BUS_HPC_INIT, 0x93 },
    { DXE_PCI_BUS_ENUM, 0x94 },
    { DXE_PCI_BUS_REQUEST_RESOURCES, 0x95 },
    { DXE_PCI_BUS_ASSIGN_RESOURCES, 0x96 },
    { DXE_CON_OUT_CONNECT, 0x97 },
    { DXE_CON_IN_CONNECT, 0x98 },
    { DXE_SIO_INIT, 0x99 },
    { DXE_USB_BEGIN, 0x9A },
    { DXE_USB_RESET, 0x9B },
    { DXE_USB_DETECT, 0x9C },
    { DXE_USB_ENABLE, 0x9D },
    //reserved for AMI use: 0x9E - 0x9F
    { DXE_IDE_BEGIN, 0xA0 },
    { DXE_IDE_RESET, 0xA1 },
    { DXE_IDE_DETECT, 0xA2 },
    { DXE_IDE_ENABLE, 0xA3 },
    { DXE_SCSI_BEGIN, 0xA4 },
    { DXE_SCSI_RESET, 0xA5 },
    { DXE_SCSI_DETECT, 0xA6 },
    { DXE_SCSI_ENABLE, 0xA7 },
    { DXE_SETUP_VERIFYING_PASSWORD, 0xA8 },
    { DXE_SETUP_START, 0xA9 },
    //reserved for AML use: 0xAA
    { DXE_SETUP_INPUT_WAIT, 0xAB },
    //reserved for AML use: 0xAC
    { DXE_READY_TO_BOOT, 0xAD },
    { DXE_LEGACY_BOOT, 0xAE },
    { DXE_EXIT_BOOT_SERVICES, 0xAF },
    { RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN, 0xB0 },
    { RT_SET_VIRTUAL_ADDRESS_MAP_END, 0xB1 },
    { DXE_LEGACY_OPROM_INIT, 0xB2 },
    { DXE_RESET_SYSTEM, 0xB3 },
    { DXE_USB_HOTPLUG, 0xB4 },
    { DXE_PCI_BUS_HOTPLUG, 0xB5 },
    { DXE_NVRAM_CLEANUP, 0xB6 },
    { DXE_CONFIGURATION_RESET, 0xB7 },
    //reserved for AMI use: 0xB8 - 0xBF
    //reserved for OEM use: 0xC0 - 0xCF

    {0,0}
};

STATUS_CODE_TO_BYTE_MAP ErrorCheckpointMap[] =
{
    { DXE_CPU_ERROR, 0xD0 },
    { DXE_NB_ERROR, 0xD1 },
    { DXE_SB_ERROR, 0xD2 },
    { DXE_ARCH_PROTOCOL_NOT_AVAILABLE, 0xD3 },
    { DXE_PCI_BUS_OUT_OF_RESOURCES, 0xD4 },
    { DXE_LEGACY_OPROM_NO_SPACE, 0xD5 },
    { DXE_NO_CON_OUT, 0xD6 },
    { DXE_NO_CON_IN, 0xD7 },
    { DXE_INVALID_PASSWORD, 0xD8 },
    { DXE_BOOT_OPTION_LOAD_ERROR, 0xD9 },
    { DXE_BOOT_OPTION_FAILED, 0xDA },
    { DXE_FLASH_UPDATE_FAILED, 0xDB },
    { DXE_RESET_NOT_AVAILABLE, 0xDC },
    //reserved for AMI use: 0xDE - 0xDF

    {0,0}
};

//**********************************************************************
// Beep codes map
//**********************************************************************
STATUS_CODE_TO_BYTE_MAP ProgressBeepMap[] =
{
    {0,0}
};

STATUS_CODE_TO_BYTE_MAP ErrorBeepMap[] =
{
    { DXE_ARCH_PROTOCOL_NOT_AVAILABLE, 4 },
    { DXE_NO_CON_OUT, 5 },
    { DXE_NO_CON_IN, 5 },
    { DXE_INVALID_PASSWORD, 1 },
    { DXE_FLASH_UPDATE_FAILED, 6 },
    { DXE_RESET_NOT_AVAILABLE, 7 },
    { DXE_PCI_BUS_OUT_OF_RESOURCES, 8},

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
