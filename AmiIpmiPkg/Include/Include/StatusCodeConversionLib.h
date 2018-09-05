//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  [Enhancements]Remove the " System Firmware Error(POST ERROR)" error log to BMC and "EFI 01030006" in BIOS event log.
//    Reason:   
//    Auditor:  Chen Lin
//    Date:     Jul/31/2017
//
//****************************************************************************
//****************************************************************************
//
//**********************************************************************
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

/** @file StatusCodeConversionLib.h
    Header for converting Status codes into equivalent
    IPMI format.

**/

#ifndef _PROGRESS_CODE_CONVERSION_LIB_H_
#define _PROGRESS_CODE_CONVERSION_LIB_H_

#include "AmiStatusCodes.h"

#define EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE     0x6F
#define SEL_RESERVE_ID_SUPPORT                  0x02

#define EFI_SOFTWARE_EFI_BMC                    (EFI_SOFTWARE | 0x00150000)

#define EFI_CU_EC0_DISABLED                     0x00000001
#define EFI_CU_EC1_DISABLED                     0x00000011
#define EFI_CU_EC2_DISABLED                     0x00000021
#define EFI_CU_EC3_DISABLED                     0x00000031
#define EFI_CU_EC4_DISABLED                     0x00000041
#define EFI_CU_EC5_DISABLED                     0x00000051
#define EFI_CU_EC6_DISABLED                     0x00000061
#define EFI_CU_EC7_DISABLED                     0x00000071

#define EFI_CU_EC0_SPARE                        0x00000005
#define EFI_CU_EC1_SPARE                        0x00000015
#define EFI_CU_EC2_SPARE                        0x00000025
#define EFI_CU_EC3_SPARE                        0x00000035
#define EFI_CU_EC4_SPARE                        0x00000045
#define EFI_CU_EC5_SPARE                        0x00000055
#define EFI_CU_EC6_SPARE                        0x00000065
#define EFI_CU_EC7_SPARE                        0x00000075

#pragma pack(1)
/**
 Status code Structure
 */
typedef struct {
  UINT32     StatusCode;
  UINT32     StatusCodeType; ///< Type of status code
  UINT8      SensorTypeCode; ///< Sensor Specific Offset
  UINT8      EventData1;     ///< ED1
  UINT8      EventData2;     ///< ED2
  UINT8      EventData3;     ///< ED3
} EFI_STATUS_CODE_TO_SEL_ENTRY;

#pragma pack()

//
//  EFI Status Code to SEL Conversion table.
//  Entries should be added to this table to convert from EFI to SEL entry.
//
EFI_STATUS_CODE_TO_SEL_ENTRY EfiStatusCodeToSelTable[] =
{
   //StatusCode                                 StatusCodeType    SensorTypeCode
                                                                        //ED1 //ED2 //ED3
  { PEI_MEMORY_INIT,                            EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x01, 0xFF },
  { DXE_IDE_BEGIN,                              EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x02, 0xFF },
  { PEI_CPU_AP_INIT,                            EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x03, 0xFF },
  { DXE_SETUP_VERIFYING_PASSWORD,               EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x04, 0xFF },
  { DXE_SETUP_START ,                           EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x05, 0xFF },
//{ EFI_IO_BUS_USB |  EFI_IOB_PC_INIT,          EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x06, 0xFF },
  { DXE_PCI_BUS_REQUEST_RESOURCES,              EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x07, 0xFF },
  { DXE_LEGACY_OPROM_INIT,                      EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x08, 0xFF },
  { DXE_CON_OUT_CONNECT,                        EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x09, 0xFF },
  { PEI_CPU_CACHE_INIT,                         EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x0A, 0xFF },
  { EFI_IO_BUS_SMBUS | EFI_IOB_PC_INIT,         EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x0B, 0xFF }, // SM Bus Initialization
  { DXE_CON_IN_CONNECT,                         EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x0C, 0xFF },
  { PEI_S3_OS_WAKE,                             EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x12, 0xFF },
  { DXE_LEGACY_BOOT,                            EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x13, 0xFF },
  { DXE_KEYBOARD_SELF_TEST,                     EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x17, 0xFF },

  { EFI_PERIPHERAL_MOUSE                 | \
    EFI_P_MOUSE_PC_SELF_TEST,                   EFI_PROGRESS_CODE, 0x0F, 0xC2, 0x18, 0xFF }, // Pointing device test

  { PEI_CAR_CPU_INIT,                           EFI_PROGRESS_CODE, 0x0F, 0XC2, 0x19, 0xFF }, // Primary processor initialization

  { EFI_SOFTWARE_EFI_BOOT_SERVICE         | \
    EFI_SW_BS_PC_EXIT_BOOT_SERVICES,            EFI_PROGRESS_CODE, 0x0F, 0xC2, 0x13, 0xFF }, // Starting operating system boot process, e.g. calling Int 19h

  { PEI_MEMORY_NOT_DETECTED,                    EFI_ERROR_CODE,    0x0F, 0XC0, 0x01, 0xFF },
  { PEI_MEMORY_NONE_USEFUL,                     EFI_ERROR_CODE,    0x0F, 0XC0, 0x02, 0xFF },
  { DXE_IDE_DEVICE_FAILURE,                     EFI_ERROR_CODE,    0x0F, 0XC0, 0x03, 0xFF }, // Unrecoverable hard dik/ATAPI/IDE device failure
  { DXE_IDE_CONTROLLER_ERROR,                   EFI_ERROR_CODE,    0x0F, 0XC0, 0x06, 0xFF }, // Unrecoverable hard disk controller failure
  { EFI_PERIPHERAL_KEYBOARD               | \
    EFI_P_EC_CONTROLLER_ERROR,                  EFI_ERROR_CODE,    0x0F, 0XC0, 0x07, 0xFF },
  { EFI_PERIPHERAL_REMOVABLE_MEDIA        | \
    EFI_P_EC_CONTROLLER_ERROR,                  EFI_ERROR_CODE,    0x0F, 0XC0, 0x08, 0xFF },
//SMCPKG  { EFI_PERIPHERAL_LOCAL_CONSOLE          | \
//SMCPKg    EFI_P_EC_CONTROLLER_ERROR,                  EFI_ERROR_CODE,    0x0F, 0XC0, 0x09, 0xFF }, // Unrecoverable Media control failure
  { DXE_NO_CON_OUT,                             EFI_ERROR_CODE,    0x0F, 0XC0, 0x0A, 0xFF },
  { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | \
    EFI_IOB_EC_CONTROLLER_ERROR,                EFI_ERROR_CODE,    0x0F, 0XC0, 0x0B, 0xFF },

  { (EFI_COMPUTING_UNIT_HOST_PROCESSOR    | \
   EFI_CU_EC_DISABLED) ,                        EFI_ERROR_CODE,    0x07, 0x08, 0xFF, 0xFF }, // Processor Disabled

  { (EFI_COMPUTING_UNIT_HOST_PROCESSOR    | \
   EFI_CU_HP_EC_MISMATCH) ,                     EFI_ERROR_CODE,    0x07, 0x05, 0xFF, 0xFF }, // Configuration Error

  { (EFI_COMPUTING_UNIT_HOST_PROCESSOR    | \
   EFI_CU_HP_EC_THERMAL ) ,                     EFI_ERROR_CODE,    0x07, 0x01, 0xFF, 0xFF }, // Thermal Trip

  { (EFI_COMPUTING_UNIT_HOST_PROCESSOR    | \
   EFI_CU_HP_EC_SELF_TEST) ,                    EFI_ERROR_CODE,    0x07, 0x02, 0xFF, 0xFF }, // BIST failure

  { (EFI_COMPUTING_UNIT_HOST_PROCESSOR    | \
   EFI_CU_HP_EC_INTERNAL ) ,                    EFI_ERROR_CODE,    0x07, 0x00, 0xFF, 0xFF }, // IERR

  { EFI_SOFTWARE_EFI_BMC                  | \
    EFI_SW_EC_EVENT_LOG_FULL,                   EFI_ERROR_CODE,    0x10, 0x04, 0XFF, 0XFF },

 //
 // Sensor Type: 0x13 - Critical Interrupt Sensor Type
 //
  { EFI_IO_BUS_PCI                        | \
    EFI_IOB_PCI_EC_PERR,                        EFI_ERROR_CODE,    0x13, 0x04, 0XFF, 0XFF }, // PCI PERR

  { EFI_IO_BUS_PCI                        | \
    EFI_IOB_PCI_EC_SERR,                        EFI_ERROR_CODE,    0x13, 0x05, 0XFF, 0XFF }, // PCI SERR

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC_NOT_CONFIGURED,                   EFI_ERROR_CODE,    0x0C, 0x07, 0XFF, 0XFF },
  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC0_DISABLED,                        EFI_ERROR_CODE,    0x0C, 0X24, 0XFF, 0x00 },
  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC1_DISABLED,                        EFI_ERROR_CODE,    0x0C, 0X24, 0XFF, 0x01 },
  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC2_DISABLED,                        EFI_ERROR_CODE,    0x0C, 0X24, 0XFF, 0x02 },
  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC3_DISABLED,                        EFI_ERROR_CODE,    0x0C, 0X24, 0XFF, 0x03 },
  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC4_DISABLED,                        EFI_ERROR_CODE,    0x0C, 0X24, 0XFF, 0x04 },
  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC5_DISABLED,                        EFI_ERROR_CODE,    0x0C, 0X24, 0XFF, 0x05 },
  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC6_DISABLED,                        EFI_ERROR_CODE,    0x0C, 0X24, 0XFF, 0x06 },
  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC7_DISABLED,                        EFI_ERROR_CODE,    0x0C, 0X24, 0XFF, 0x07 },

  { GENERIC_MEMORY_CORRECTABLE_ERROR,           EFI_ERROR_CODE,    0x0C, 0x00, 0xFF, 0xFF }, // Correctable ECC/other correctable memory error

  { GENERIC_MEMORY_UNCORRECTABLE_ERROR,         EFI_ERROR_CODE,    0x0C, 0x01, 0xFF, 0xFF }, // Uncorrectable ECC/other uncorrectable memory error

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC0_SPARE,                           EFI_PROGRESS_CODE, 0x0C, 0X28, 0XFF, 0x00 },

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC1_SPARE,                           EFI_PROGRESS_CODE, 0x0C, 0X28, 0XFF, 0x01 },

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC2_SPARE,                           EFI_PROGRESS_CODE, 0x0C, 0X28, 0XFF, 0x02 },

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC3_SPARE,                           EFI_PROGRESS_CODE, 0x0C, 0X28, 0XFF, 0x03 },

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC4_SPARE,                           EFI_PROGRESS_CODE, 0x0C, 0X28, 0XFF, 0x04 },

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC5_SPARE,                           EFI_PROGRESS_CODE, 0x0C, 0X28, 0XFF, 0x05 },

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC6_SPARE,                           EFI_PROGRESS_CODE, 0x0C, 0X28, 0XFF, 0x06 },

  { EFI_COMPUTING_UNIT_MEMORY             | \
    EFI_CU_EC7_SPARE,                           EFI_PROGRESS_CODE, 0x0C, 0X28, 0XFF, 0x07 },

  { PEI_MEMORY_PRESENCE_DETECT,                 EFI_PROGRESS_CODE, 0x0C, 0x06, 0xFF, 0xFF } // Presence detected. Indicate presence of entity
                                                                                            // associated with the sensor.

// SEL Table End
};

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
