/** @file
  Include file for ASF Driver

@copyright
 Copyright (c) 2010 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _ALERT_STANDARD_FORMAT_H
#define _ALERT_STANDARD_FORMAT_H

#define ASF_VERSION 0x10

//
// ASF SMBUS Messages
//
#define ASF_MESSAGE_COMMAND_SENSE_DEVICE_SYSTEM_STATE 0x01
#define ASF_MESSAGE_COMMAND_MANAGEMENT_CONTROL        0x02
#define ASF_MESSAGE_COMMAND_ASF_CONFIGURATION         0x03
#define ASF_MESSAGE_COMMAND_MESSAGE                   0x04
#define ASF_MESSAGE_COMMAND_GETUUID                   0xC7
#define ASF_MESSAGE_COMMAND_REPORT_BIOS_STATUS        0x03

#define ASF_MESSAGE_BYTECOUNT_RETURN_BOOT_OPT         0x0D
#define ASF_MESSAGE_BYTECOUNT_CLEAR_BOOT_OPT          0x02
#define ASF_MESSAGE_BYTEOUNTT_GET_BOOT_OPT            0x02
#define ASF_MESSAGE_BYTECOUNT_DEVICE_TYPE_POLL        0x03
#define ASF_MESSAGE_BYTECOUNT_STOP_WATCH_DOG_TIMER    0x02
#define ASF_MESSAGE_BYTECOUNT_START_WATCH_DOG_TIMER   0x0D
#define ASF_MESSAGE_BYTECOUNT_GET_RSE_PASSWORD        0x02
#define ASF_MESSAGE_BYTECOUNT_REPORT_BIOS_STATUS      0x06

#define ASF_MESSAGE_SUBCOMMAND_RETURN_BOOT_OPT        0x16
#define ASF_MESSAGE_SUBCOMMAND_CLEAR_BOOT_OPT         0x15
#define ASF_MESSAGE_SUBCOMMAND_DEVICE_TYPE_POLL       0x13
#define ASF_MESSAGE_SUBCOMMAND_STOP_WATCH_DOG_TIMER   0x14
#define ASF_MESSAGE_SUBCOMMAND_START_WATCH_DOG_TIMER  0x13
#define ASF_MESSAGE_SUBCOMMAND_RETRANSMIT             0x15
#define ASF_MESSAGE_SUBCOMMAND_NORETRANSMIT           0x16
#define ASF_MESSAGE_SUBCOMMAND_SYSTEM_STATE           0x18
#define ASF_MESSAGE_SUBCOMMAND_GET_RSE_PASSWORD       0x18
#define ASF_MESSAGE_SUBCOMMAND_REQUEST                0x19

#define ASF_MESSAGE_VERSIONNUMBER                     0x10

//
// ASF Event Sensor Type Codes
//
#define ASF_EVENT_SENSOR_TYPE_SYS_FW_ERR_PROG     0x0F
#define ASF_EVENT_SENSOR_TYPE_POST_ERR_PROG       0x0F
#define ASF_EVENT_SENSOR_TYPE_BOOT_ERROR          0x1E
#define ASF_EVENT_SENSOR_TYPE_OS_BOOT             0x1F
#define ASF_EVENT_SENSOR_TYPE_ENTITY_PRESENCE     0x25
#define ASF_EVENT_SENSOR_TYPE_WATCHDOG2           0x23

#define ASF_EVENT_SENSOR_TYPE_TEMPERATURE         0x01
#define ASF_EVENT_SENSOR_TYPE_VOLTAGE             0x02
#define ASF_EVENT_SENSOR_TYPE_FAN                 0x04
#define ASF_EVENT_SENSOR_TYPE_CHASSIS_INTRUSION   0x05
#define ASF_EVENT_SENSOR_TYPE_SECURITY_VIOLATION  0x06
#define ASF_EVENT_SENSOR_TYPE_PROCESSOR           0x07
#define ASF_EVENT_SENSOR_TYPE_POWER_SUPPLY        0x08
#define ASF_EVENT_SENSOR_TYPE_POWER_UNIT          0x09
#define ASF_EVENT_SENSOR_TYPE_CHIPSET             0x19
#define ASF_EVENT_SENSOR_TYPE_BATTERY             0x29

//
// ASF Event Type Codes
// IPMI/PET related stuff
//
#define ASF_EVENT_TYPE_SENSOR_SPECIFIC                0x6F

#define ASF_EVENT_TYPE_THRESHOLD_BASED                0x01
#define ASF_EVENT_TYPE_USAGE_STATE                    0x02
#define ASF_EVENT_TYPE_DISCRETE_EVENT_STATE           0x03
#define ASF_EVENT_TYPE_DISCRETE_EVENT_PREDICTIVE_FAIL 0x04
#define ASF_EVENT_TYPE_DISCRETE_EVENT_LIMIT           0x05
#define ASF_EVENT_TYPE_DISCRETE_EVENT_PERFORMANCE     0x06
#define ASF_EVENT_TYPE_SEVERITY_STATE                 0x07
#define ASF_EVENT_TYPE_AVAILABILITY_STATUS_PRESENT    0x08
#define ASF_EVENT_TYPE_AVAILABILITY_STATUS_ENABLE     0x09
#define ASF_EVENT_TYPE_AVAILABILITY_STATUS_RUNNING    0x0A
#define ASF_EVENT_TYPE_AVAILABILITY_STATUS_OTHER      0x0B
#define ASF_EVENT_TYPE_DEVICE_POWER_STATES            0x0C

//
// ASF Event Offset Codes
// IPMI/PET related stuff
//
#define ASF_EVENT_OFFSET_ENTITY_PRESENT                           0x00
#define ASF_EVENT_OFFSET_TIMER_EXPIRED                            0x00
#define ASF_EVENT_OFFSET_SYS_FW_ERROR                             0x00
#define ASF_EVENT_OFFSET_SYS_FW_HANG                              0x01
#define ASF_EVENT_OFFSET_SYS_FW_PROGRESS                          0x02

#define ASF_EVENT_OFFSET_THRESHOLD_LOWER_NONCRITICAL              0x00
#define ASF_EVENT_OFFSET_THRESHOLD_LOWER_NONCRITICAL_GO_HIGH      0x01
#define ASF_EVENT_OFFSET_THRESHOLD_LOWER_CRITICAL                 0x02
#define ASF_EVENT_OFFSET_THRESHOLD_LOWER_CRITICAL_GO_HIGH         0x03
#define ASF_EVENT_OFFSET_THRESHOLD_LOWER_NONRECOVERABLE           0x04
#define ASF_EVENT_OFFSET_THRESHOLD_LOWER_NONRECOVERABLE_GO_HIGH   0x05
#define ASF_EVENT_OFFSET_THRESHOLD_UPPER_NONCRITICAL_GO_LOW       0x06
#define ASF_EVENT_OFFSET_THRESHOLD_UPPER_NONCRITICAL              0x07
#define ASF_EVENT_OFFSET_THRESHOLD_UPPER_CRITICAL_GO_LOW          0x08
#define ASF_EVENT_OFFSET_THRESHOLD_UPPER_CRITICAL                 0x09
#define ASF_EVENT_OFFSET_THRESHOLD_UPPER_NONRECOVERABLE_GO_LOW    0x0A
#define ASF_EVENT_OFFSET_THRESHOLD_UPPER_NONRECOVERABLE           0x0B

#define ASF_EVENT_OFFSET_DISCRETE_EVENT_PREDICTIVE_FAIL_DEASSERT  0x00
#define ASF_EVENT_OFFSET_DISCRETE_EVENT_PREDICTIVE_FAIL_ASSERT    0x01

#define ASF_EVENT_OFFSET_SEVERITY_TO_OK                           0x00
#define ASF_EVENT_OFFSET_SEVERITY_MORE_NONCRITICAL                0x01
#define ASF_EVENT_OFFSET_SEVERITY_MORE_CRITICAL                   0x02
#define ASF_EVENT_OFFSET_SEVERITY_MORE_NONRECOVERABLE             0x03
#define ASF_EVENT_OFFSET_SEVERITY_LESS_NONCRITICAL                0x04
#define ASF_EVENT_OFFSET_SEVERITY_LESS_CRITICAL                   0x05
#define ASF_EVENT_OFFSET_SEVERITY_TO_NONRECOVERABLE               0x06
#define ASF_EVENT_OFFSET_SEVERITY_MONITOR                         0x07
#define ASF_EVENT_OFFSET_SEVERITY_INFORMATIONAL                   0x08

#define ASF_EVENT_OFFSET_CHASSIS_INTRUSION_GENERIC                0x00

#define ASF_EVENT_OFFSET_PROCESSOR_IERR                           0x00

#define ASF_EVENT_OFFSET_SECURITY_VIOLATION_USER_PASSWORD         0x01
#define ASF_EVENT_OFFSET_SECURITY_VIOLATION_SUPERVISOR_PASSWORD   0x02
#define ASF_EVENT_OFFSET_SECURITY_VIOLATION_NETWORK_PASSWORD      0x03
#define ASF_EVENT_OFFSET_SECURITY_VIOLATION_OTHER_PASSWORD        0x04
#define ASF_EVENT_OFFSET_SECURITY_VIOLATION_OUTOFBAND_PASSWORD    0x05

#define ASF_EVENT_OFFSET_BATTERY_LOW_WARNING                      0x00
#define ASF_EVENT_OFFSET_BATTERY_LOW_CRITICAL                     0x01
#define ASF_EVENT_OFFSET_BATTERY_PRESENCE_DETECTED                0x02

#define ASF_EVENT_OFFSET_POWER_SUPPLY_PRESENCE                    0x00
#define ASF_EVENT_OFFSET_POWER_SUPPLY_FAILURE                     0x01
#define ASF_EVENT_OFFSET_POWER_SUPPLY_PREDICTIVE_FAILURE          0x02

#define ASF_EVENT_OFFSET_POWER_UNIT_DOWN                          0x00
#define ASF_EVENT_OFFSET_POWER_UNIT_CYCLE                         0x01
#define ASF_EVENT_OFFSET_POWER_UNIT_240VA_DOWN                    0x02
#define ASF_EVENT_OFFSET_POWER_UNIT_INTERLOCK_DOWN                0x03
#define ASF_EVENT_OFFSET_POWER_UNIT_AC_LOST                       0x04
#define ASF_EVENT_OFFSET_POWER_UNIT_SOFT_CONTROL_FAILURE          0x05
#define ASF_EVENT_OFFSET_POWER_UNIT_FAILURE                       0x06

//
// ASF Event Source Type Code
// IPMI/PET related stuff
//
#define ASF_EVENT_SOURCE_TYPE_ASF10             0x68
#define ASF_EVENT_SOURCE_TYPE_PLATFORM_FIRMWARE 0x00
#define ASF_EVENT_SOURCE_TYPE_OS                0x48

//
// ASF Event Severity Codes
// IPMI/PET related stuff
//
#define ASF_EVENT_SEVERITY_CODE_MONITOR         0x01
#define ASF_EVENT_SEVERITY_CODE_INFORMATION     0x02
#define ASF_EVENT_SEVERITY_CODE_OK              0x04
#define ASF_EVENT_SEVERITY_CODE_NONCRITICAL     0x08
#define ASF_EVENT_SEVERITY_CODE_CRITICAL        0x10
#define ASF_EVENT_SEVERITY_CODE_NONRECOVERABLE  0x20

///
/// ASF Sensor Device Codes
/// IPMI/PET related stuff
///
#define ASF_SENSOR_DEVICE 0xFF

///
/// ASF Sensor Number Codes
/// IPMI/PET related stuff
///
#define ASF_SENSOR_NUMBER 0xFF

//
// ASF Entity Codes
// IPMI/PET related stuff
//
#define ASF_ENTITY_UNSPECIFIED              0x00
#define ASF_ENTITY_OTHER                    0x01
#define ASF_ENTITY_UNKNOWN                  0x02
#define ASF_ENTITY_PROCESSOR                0x03
#define ASF_ENTITY_DISK                     0x04
#define ASF_ENTITY_PERIPHERAL               0x05
#define ASF_ENTITY_SYSTEM_MGMT_MOD          0x06
#define ASF_ENTITY_SYSTEM_BOARD             0x07
#define ASF_ENTITY_POWER_SUPPLY             0x0A
#define ASF_ENTITY_ADDIN_CARD               0x0B
#define ASF_ENTITY_CHASIS_BACK_PANEL_BOARD  0x17
#define ASF_ENTITY_MEMORY                   0x20
#define ASF_ENTITY_SYSTEM_MGMT_SW           0x21
#define ASF_ENTITY_BIOS                     0x22
#define ASF_ENTITY_OS                       0x23
#define ASF_ENTITY_REMOTE_OOB_MGMT          0x26
#define ASF_ENTITY_INTEL_AMT                0x26

///
/// ASF Event Data Codes for System Firmware Progress Events
/// IPMI/PET related stuff
///
#define ASF_FP_EVENT_DATA0          0x40

#define ASF_FP_UNSPECIFIED          0x00
#define ASF_FP_MEMORY_INIT          0x01
#define ASF_FP_HDD_INIT             0x02
#define ASF_FP_AP_INIT              0x03
#define ASF_FP_USER_AUTHENTICATION  0x04
#define ASF_FP_USER_SETUP           0x05
#define ASF_FP_USB_RESOURCE_CONFIG  0x06
#define ASF_FP_PCI_RESOURCE_CONFIG  0x07
#define ASF_FP_OPTION_ROM_INIT      0x08
#define ASF_FP_VIDEO_INIT           0x09
#define ASF_FP_CACHE_INIT           0x0A
#define ASF_FP_SMBUS_INIT           0x0B
#define ASF_FP_KBC_INIT             0x0C
#define ASF_FP_EC_INIT              0x0D
#define ASF_FP_AMTBX_INIT           0x0D
#define ASF_FP_ATTACH_DOCK          0x0E
#define ASF_FP_ENABLE_DOCK          0x0F
#define ASF_FP_EJECT_DOCK           0x10
#define ASF_FP_DISABLE_DOCK         0x11
#define ASF_FP_WAKE_OS              0x12
#define ASF_FP_BOOT_OS              0x13
#define ASF_FP_BOARD_INIT           0x14
#define ASF_FP_RESERVED             0x15
#define ASF_FP_FDC_INIT             0x16
#define ASF_FP_KBD_TEST             0x17
#define ASF_FP_MOUSE_TEST           0x18
#define ASF_FP_BSP_INIT             0x19

///
/// ASF Event Data Codes for System Firmware Error Events
/// IPMI/PET related stuff
///
#define ASF_FE_EVENT_DATA0                    0x40

#define ASF_FE_UNSPECIFIED                    0x00
#define ASF_FE_AMTBX_FAILURE                  0x00
#define ASF_FE_NO_MEMORY                      0x01
#define ASF_FE_MEMORY_FAILURE                 0x02
#define ASF_FE_HDD_FAILURE                    0x03
#define ASF_FE_BOARD_FAILURE                  0x04
#define ASF_FE_FDC_FAILURE                    0x05
#define ASF_FE_HDC_FAILURE                    0x06
#define ASF_FE_KBD_FAILURE                    0x07
#define ASF_FE_REMOVABLE_BOOT_MEDIA_NOT_FOUND 0x08
#define ASF_FE_VIDEO_CONTROLLER_FAILURE       0x09
#define ASF_FE_VIDEO_DEVICE_NOT_FOUND         0x0A
#define ASF_FE_ROM_CORRUPTED                  0x0B
#define ASF_FE_CPU_VOLTAGE_FAILURE            0x0C
#define ASF_FE_CPU_MISMATCH_FAILURE           0x0D

//
// AMT BX Software Class DXE Subclass Progress Code definitions.
//
#define EFI_SW_DXE_AMTBX_OPROM_DONE               (EFI_OEM_SPECIFIC | 0x00000000)
#define EFI_SW_DXE_AMTBX_OPROM_NOT_STARTED        (EFI_OEM_SPECIFIC | 0x00000001)
#define EFI_SW_DXE_AMTBX_SLEEP_S4_NOT_TAKEN       (EFI_OEM_SPECIFIC | 0x00000002)
#define EFI_SW_DXE_AMTBX_SLEEP_S5_NOT_TAKEN       (EFI_OEM_SPECIFIC | 0x00000003)
#define EFI_SW_DXE_AMTBX_SLEEP_UNSPECD_NOT_TAKEN  (EFI_OEM_SPECIFIC | 0x00000004)
#define EFI_SW_DXE_AMTBX_ERROR_ACTION_INTF        (EFI_OEM_SPECIFIC | 0x00000005)
#define EFI_SW_DXE_AMTBX_ERROR_ACTION_INV         (EFI_OEM_SPECIFIC | 0x00000006)
#define EFI_SW_DXE_AMTBX_ERROR_ACTION_SETUP       (EFI_OEM_SPECIFIC | 0x00000007)
#define EFI_SW_DXE_AMTBX_ERROR_ACTION_FAIL        (EFI_OEM_SPECIFIC | 0x00000008)
#define EFI_SW_DXE_AMTBX_ERROR_ACTION_UNSPECD     (EFI_OEM_SPECIFIC | 0x00000009)
#define EFI_SW_DXE_AMTBX_OTHER_UNSPECD            (EFI_OEM_SPECIFIC | 0x00000009)

typedef enum {
  AsfMessageBiosPresent,
  AsfMessageMemInit,
  AsfMessageMemInitDone,
  AsfMessageHddInit,
  AsfMessageApInit,
  AsfMessageUserAuthentication,
  AsfMessageUserInitSetup,
  AsfMessageUsbResourceConfig,
  AsfMessagePciResourceConfig,
  AsfMessageOptionRomInit,
  AsfMessageVideoInit,
  AsfMessageCacheInit,
  AsfMessageSmbusInit,
  AsfMessageKbcInit,
  AsfMessageEmControllerInit,
  AsfMessageDockAttached,
  AsfMessageEnableDock,
  AsfMessageDockEject,
  AsfMessageDisableDock,
  AsfMessageOSWakeVector,
  AsfMessageMotherBoardInit,
  AsfMessageFdcInit,
  AsfMessageKeyboardTest,
  AsfMessageMouseTest,
  AsfMessageBspInit,
  AsfMessageAmtBxDone,
  AsfMessageAmtBxNotStarted,
  AsfMessageAmtBxSleepS4ReportedNotTaken,
  AsfMessageAmtBxSleepS5ReportedNotTaken,
  AsfMessageAmtBxSleepUnspecifiedReportedNotTaken,
  AsfMessageAmtBxErrActionIntF,
  AsfMessageAmtBxErrActionInv,
  AsfMessageAmtBxErrActionSetup,
  AsfMessageAmtBxErrActionFailure,
  AsfMessageAmtBxErrActionUnspecified,
  AsfMessageAmtBxOtherUnspecified,
  AsfMessageNoMemory,
  AsfMessageMemoryFailure,
  AsfMessageHddFailure,
  AsfMessageBoardFailure,
  AsfMessageFdcFailure,
  AsfMessageHdcFailure,
  AsfMessageKbdFailure,
  AsfMessageNoFdd,
  AsfMessageVideoControllerFailure,
  AsfMessageNoVideo,
  AsfMessageFirmwareCorruption,
  AsfMessageCpuVidMismatch,
  AsfMessageCpuSpeedMatchingFailure,
  AsfMessageUserAuthenticationInvalid,
  AsfMessageHddAuthenticationInvalid,
  AsfMessageNoBootMedia,
  AsfMessageTemperatureGenericCritical,
  AsfMessageTemperatureGenericWarning,
  AsfMessageTemperatureOverCritical,
  AsfMessageTemperatureOverWarning,
  AsfMessageTemperatureUnderCritical,
  AsfMessageTemperatureUnderWarning,
  AsfMessageVoltageGenericCritical,
  AsfMessageVoltageOverCritical,
  AsfMessageVoltageUnderCritical,
  AsfMessageFanGenericCritical,
  AsfMessageFanPredictiveFailure,
  AsfMessageFanLowSpeedCritical,
  AsfMessageFanLowSpeedWarning,
  AsfMessageBatteryLowWarning,
  AsfMessageBatteryLowCritical,
  AsfMessageBatteryPresenceDetected,
  AsfMessagePowerFailure,
  AsfMessageChassisIntrusion,
} ASF_FRAMEWORK_MESSAGE_TYPE;

//
// StatusCode extension for ASF -- start
//
#define EFI_CU_HP_EC_VID_MISMATCH                   (EFI_SUBCLASS_SPECIFIC | 0x0000000E)
#define EFI_CU_HP_EC_SPEED_MATCHING_FAILURE         (EFI_SUBCLASS_SPECIFIC | 0x0000000F)

#define EFI_PERIPHERAL_EMBEDDED_CONTROLLER          (EFI_PERIPHERAL | 0x000D0000)
#define EFI_PERIPHERAL_DOCK                         (EFI_PERIPHERAL | 0x000E0000)

#define EFI_PERIPHERAL_BATTERY                      (EFI_PERIPHERAL | 0x000F0000)
#define EFI_P_BATTERY_PC_LOW_WARNING                (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_P_BATTERY_EC_LOW_CRITICAL               (EFI_SUBCLASS_SPECIFIC | 0x00000000)

#define EFI_PERIPHERAL_FAN                          (EFI_PERIPHERAL | 0x00100000)
#define EFI_P_FAN_EC_GENERIC_CRITICAL               (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_P_FAN_EC_PREDICTIVE_FAILURE             (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#define EFI_P_FAN_EC_LOW_SPEED_CRITICAL             (EFI_SUBCLASS_SPECIFIC | 0x00000002)
#define EFI_P_FAN_PC_LOW_SPEED_WARNING              (EFI_SUBCLASS_SPECIFIC | 0x00000002)

#define EFI_PERIPHERAL_TEMPERATURE                  (EFI_PERIPHERAL | 0x00110000)
#define EFI_P_TEMERATURE_EC_GENERIC_CRITICAL        (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_P_TEMERATURE_PC_GENERIC_WARNING         (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_P_TEMERATURE_EC_OVER_CRITICAL           (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#define EFI_P_TEMERATURE_PC_OVER_WARNING            (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#define EFI_P_TEMERATURE_EC_UNDER_CRITICAL          (EFI_SUBCLASS_SPECIFIC | 0x00000002)
#define EFI_P_TEMERATURE_PC_UNDER_WARNING           (EFI_SUBCLASS_SPECIFIC | 0x00000002)

#define EFI_PERIPHERAL_VOLTAGE                      (EFI_PERIPHERAL | 0x00120000)
#define EFI_P_VOLTAGE_EC_GENERIC_CRITICAL           (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_P_VOLTAGE_EC_OVER_CRITICAL              (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#define EFI_P_VOLTAGE_EC_UNDER_CRITICAL             (EFI_SUBCLASS_SPECIFIC | 0x00000002)

#define EFI_PERIPHERAL_POWER_SUPPLY                 (EFI_PERIPHERAL | 0x00130000)
#define EFI_P_POWER_SUPPLY_EC_FAILURE               (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_P_POWER_SUPPLY_EC_PREDICTIVE_FAILURE    (EFI_SUBCLASS_SPECIFIC | 0x00000001)

#define EFI_CU_CHIPSET_PC_INTRUDER_DETECT           (EFI_SUBCLASS_SPECIFIC | 0x00000000)

#define EFI_SW_DXE_BS_EC_NO_BOOT_MEDIA              (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#define EFI_SW_DXE_BS_EC_AUTHENTICATE_USER_INVALID  (EFI_SUBCLASS_SPECIFIC | 0x00000002)
#define EFI_SW_DXE_BS_EC_AUTHENTICATE_HDD_INVALID   (EFI_SUBCLASS_SPECIFIC | 0x00000003)

///
/// ASF Over HECI
///
#pragma pack(1)
typedef struct {
  UINT8 SubCommand;
  UINT8 Version;
  UINT8 EventSensorType;
  UINT8 EventType;
  UINT8 EventOffset;
  UINT8 EventSourceType;
  UINT8 EventSeverity;
  UINT8 SensorDevice;
  UINT8 SensorNumber;
  UINT8 Entity;
  UINT8 EntityInstance;
  UINT8 Data0;
  UINT8 Data1;
} ASF_MESSAGE;
#pragma pack()

typedef struct {
  ASF_FRAMEWORK_MESSAGE_TYPE      MessageType;
  ASF_MESSAGE                     Message;
} ASF_FRAMEWORK_MESSAGE;

typedef struct {
  ASF_FRAMEWORK_MESSAGE_TYPE      MessageType;
  EFI_STATUS_CODE_VALUE           StatusCodeValue;
} ASF_DATA_HUB_MAP;

//
// StatusCode extension for ASF -- end
//
#endif
