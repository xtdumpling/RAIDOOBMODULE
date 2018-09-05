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


/** @file SmbiosElogSupport.h
    SmbiosElogSupport Header file - structures,constants

**/


#ifndef _AMI_SMBIOS_ELOG_SUPPORT_H_
#define _AMI_SMBIOS_ELOG_SUPPORT_H_

#define AMI_SMBIOS_ELOG_SUPPORT_GUID { \
  0x0e5870e4, 0x0525, 0x40ad, { 0x95, 0xa8, 0x0f, 0xff, 0x15, 0x5b, 0x8f, 0xc0 } }

EFI_GUID gAmiSmbiosElogSupportGuid;

typedef struct _AMI_SMBIOS_ELOG_SUPPORT_PROTOCOL
    AMI_SMBIOS_ELOG_SUPPORT_PROTOCOL;

typedef struct _AMI_SMBIOS_ELOG_SUPPORT_PROTOCOL {
    UINT8        *Something;
} AMI_SMBIOS_ELOG_SUPPORT_PROTOCOL;

//---------------------------------------------------------------------------

// Header for all Event Log Entries

#pragma pack(1)


//  7.16.6 SMBIOS Specification Log Record format

typedef struct {
  UINT8    EventType;    ///< (Values from Table 1 Below).
  UINT8    Length;       ///< bits 0 to 6 : size, usually 10h.
                         ///< bit7 : 0 = event already read.
                         ///< bit7 : 1 = event not read.
  UINT8    Year;         ///< year of event.
  UINT8    Month;        ///< month of event.
  UINT8    Day;          ///< day of event.
  UINT8    Hour;         ///< hour of event.
  UINT8    Minute;       ///< minute of event.
  UINT8    Second;       ///< second of event.
} EFI_GPNV_ERROR_EVENT;

//---------------------------------------------------------------------------

typedef struct {
    UINT8   VariableDataType;
    UINT16  SmbiosHandle;
}HANDLE_DATA;

typedef struct {
    UINT8   VariableDataType;
    UINT32  Counter;
}MULTIPLE_EVENT_DATA;

typedef struct {
    UINT8   VariableDataType;
    UINT16  SmbiosHandle;
    UINT32  Counter;
}MULTIPLE_EVENT_HANDLE_DATA;

typedef struct {
    UINT8   VariableDataType;
    UINT32  Bitmap1;
    UINT32  Bitmap2;
}POST_RESULTS_BITMAP_DATA;

typedef struct {
    UINT8   VariableDataType;
    UINT32  Condition;
}SYSTEM_MANAGEMENT_DATA;

typedef struct {
    UINT8   VariableDataType;
    UINT32  Condition;
    UINT32  Counter;
}MULTIPLE_EVENT_SYSTEM_MANAGEMENT_DATA;

typedef struct {
    UINT8   VariableDataType;
    EFI_STATUS_CODE_TYPE  Type;
    EFI_STATUS_CODE_VALUE Value;
    UINT32                Counter;
}MULTIPLE_EVENT_OEM_STATUS_CODE_DATA;

typedef union {
    HANDLE_DATA                           Handle;
    MULTIPLE_EVENT_DATA                   Me;
    MULTIPLE_EVENT_HANDLE_DATA            MeHandle;
    POST_RESULTS_BITMAP_DATA              PostResults;
    SYSTEM_MANAGEMENT_DATA                SystemManagement;
    MULTIPLE_EVENT_SYSTEM_MANAGEMENT_DATA MeSystemManagement;
    MULTIPLE_EVENT_OEM_STATUS_CODE_DATA   MeStatusCode;
}ALL_EVENT_TYPE_DATA;

typedef struct {
    EFI_GPNV_ERROR_EVENT  Header;
    ALL_EVENT_TYPE_DATA   Data;
}ALL_GPNV_ERROR;

#pragma pack()

#pragma pack(1)

typedef struct {
    UINT8   RecordType;
    UINT8   EventLogType ;
}EFI_GPNV_RUNTIME_ERROR;

typedef struct {
    UINT8   EventType;
    UINT16  SmbiosHandle;
}GPNV_ERROR_HANDLE;

#pragma pack()

#pragma pack(1)
typedef struct {
    UINT8 RecordType;  // Always EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE
    EFI_STATUS_CODE_TYPE  StatusCodeType;
    EFI_STATUS_CODE_VALUE StatusCodeValue;
} EFI_STATUS_CODE_ELOG_INPUT;
#pragma pack()


//---------------------------------------------------------------------------

// Event Log Types
// 0x18-0x7F Unused
// 0x80-0xFE OEM Assigned
//
// IMPORTANT: IF A VALUE IS ADDED TO THIS LIST, CHANGE 
//            NumOfSupportedSmbiosTypes DEFINE TOO!

#define EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR      0x01
#define EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR       0x02
#define EFI_EVENT_LOG_TYPE_PARITY_MEMORY_ERROR              0x03
#define EFI_EVENT_LOG_TYPE_BUS_TIME_OUT                     0x04
#define EFI_EVENT_LOG_TYPE_IO_CHANNEL_CHECK                 0x05
#define EFI_EVENT_LOG_TYPE_SOFTWARE_NMI                     0x06
#define EFI_EVENT_LOG_TYPE_POST_MEMORY_RESIZE               0x07
#define EFI_EVENT_LOG_TYPE_POST_ERROR                       0x08
#define EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR                 0x09
#define EFI_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR                 0x0A
#define EFI_EVENT_LOG_TYPE_CPU_FAILURE                      0x0B
#define EFI_EVENT_LOG_TYPE_EISA_FAILSAFE_TIMER_TIME_OUT     0x0C
#define EFI_EVENT_LOG_TYPE_CORRECTABLE_MEMORY_LOG_DISABLED  0x0D
#define EFI_EVENT_LOG_TYPE_LOGGING_DISABLED_FOR_EVENT_TYPE  0x0E
#define EFI_EVENT_LOG_TYPE_SYSTEM_LIMIT_EXCEEDED            0x10
#define EFI_EVENT_LOG_TYPE_ASYN_HW_TIMER_EXPIRED            0x11
#define EFI_EVENT_LOG_TYPE_SYSTEM_CONFIG_INFORMATION        0x12
#define EFI_EVENT_LOG_TYPE_HARD_DISK_INFORMATION            0x13
#define EFI_EVENT_LOG_TYPE_SYSTEM_RECONFIGURED              0x14
#define EFI_EVENT_LOG_TYPE_UNCORRECTABLE_CPU_COMPLEX_ERROR  0x15
#define EFI_EVENT_LOG_TYPE_LOG_AREA_RESET                   0x16
#define EFI_EVENT_LOG_TYPE_SYSTEM_BOOT                      0x17
#define EFI_EVENT_LOG_TYPE_END_OF_LOG                       0xFF
//  ** New OEM Definitions Below **
#define EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE              0xE0
#define EFI_EVENT_LOG_TYPE_OEM_DUMMY                        0xE1
#define EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME                 0xE2


//---------------------------------------------------------------------------

// Event Log Variable Data Format Types and Size in bytes
// 0x07-0x7F Unused
// 0x80-0xFF OEM Assigned

#define EFI_EVENT_LOG_VAR_DATA_TYPE_NONE                              0x00
#define EFI_EVENT_LOG_VAR_DATA_TYPE_NONE_SIZE                         0x00

#define EFI_EVENT_LOG_VAR_DATA_TYPE_HANDLE                            0x01
#define EFI_EVENT_LOG_VAR_DATA_TYPE_HANDLE_SIZE                       0x02

#define EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT                       0x02
#define EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_SIZE                  0x04

#define EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE                0x03
#define EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE_SIZE           0x06

#define EFI_EVENT_LOG_VAR_DATA_TYPE_POST_RESULTS_BITMAP               0x04
#define EFI_EVENT_LOG_VAR_DATA_TYPE_POST_RESULTS_BITMAP_SIZE          0x08

#define EFI_EVENT_LOG_VAR_DATA_TYPE_SYS_MNGMT_TYPE                    0x05
#define EFI_EVENT_LOG_VAR_DATA_TYPE_SYS_MNGMT_TYPE_SIZE               0x04

#define EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_SYS_MNGMT_TYPE        0x06
#define EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_SYS_MNGMT_TYPE_SIZE   0x08

#define EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_STATUS_CODE                   0xE0
#define EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_STATUS_CODE_SIZE              0x0C

// No defined size, This should never be used
#define EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_DUMMY                         0xE1
#define EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_DUMMY_SIZE                    0xFF 


//---------------------------------------------------------------------------

// System Management Types 

#define EFI_SYS_MNGMT_TYPE_25V_OUT_OF_RANGE_1               0x00000000
#define EFI_SYS_MNGMT_TYPE_25V_OUT_OF_RANGE_2               0x00000001
#define EFI_SYS_MNGMT_TYPE_33V_OUT_OF_RANGE                 0x00000002
#define EFI_SYS_MNGMT_TYPE_5V_OUT_OF_RANGE                  0x00000003
#define EFI_SYS_MNGMT_TYPE_NEG_5V_OUT_OF_RANGE              0x00000004
#define EFI_SYS_MNGMT_TYPE_12V_OUT_OF_RANGE                 0x00000005
#define EFI_SYS_MNGMT_TYPE_NEG_12V_OUT_OF_RANGE             0x00000006
#define EFI_SYS_MNGMT_TYPE_SYS_BOARD_TEMP_OUT_OF_RANGE      0x00000010
#define EFI_SYS_MNGMT_TYPE_PROCESSOR_1_TEMP_OUT_OF_RANGE    0x00000011
#define EFI_SYS_MNGMT_TYPE_PROCESSOR_2_TEMP_OUT_OF_RANGE    0x00000012
#define EFI_SYS_MNGMT_TYPE_PROCESSOR_3_TEMP_OUT_OF_RANGE    0x00000013
#define EFI_SYS_MNGMT_TYPE_PROCESSOR_4_TEMP_OUT_OF_RANGE    0x00000014
#define EFI_SYS_MNGMT_TYPE_FAN_0_OUT_OF_RANGE               0x00000020
#define EFI_SYS_MNGMT_TYPE_FAN_1_OUT_OF_RANGE               0x00000021
#define EFI_SYS_MNGMT_TYPE_FAN_2_OUT_OF_RANGE               0x00000022
#define EFI_SYS_MNGMT_TYPE_FAN_3_OUT_OF_RANGE               0x00000023
#define EFI_SYS_MNGMT_TYPE_FAN_4_OUT_OF_RANGE               0x00000024
#define EFI_SYS_MNGMT_TYPE_FAN_5_OUT_OF_RANGE               0x00000025
#define EFI_SYS_MNGMT_TYPE_FAN_6_OUT_OF_RANGE               0x00000026
#define EFI_SYS_MNGMT_TYPE_FAN_7_OUT_OF_RANGE               0x00000027
#define EFI_SYS_MNGMT_TYPE_CHASSIS_SECURE_SWITCH_ACTIVATED  0x00000030
#define EFI_SYS_MNGMT_TYPE_OTHER_MASK                       0x0001FFFF


//---------------------------------------------------------------------------

// POST RESULTS BITMAP
// (all reserved values = 0) 

#define EFI_POST_BITMAP_DW1_CHANNEL2_TIMER_ERROR        bit0
#define EFI_POST_BITMAP_DW1_MASTER_PIC_ERROR            bit1
#define EFI_POST_BITMAP_DW1_SLAVE_PIC_ERROR             bit2
#define EFI_POST_BITMAP_DW1_CMOS_BATTERY_FAILURE        bit3
#define EFI_POST_BITMAP_DW1_CMOS_SYS_OPTIONS_NOT_SET    bit4
#define EFI_POST_BITMAP_DW1_CMOS_CKSUM_ERROR            bit5
#define EFI_POST_BITMAP_DW1_CMOS_CONFIG_ERROR           bit6
#define EFI_POST_BITMAP_DW1_MOUSE_KEYBOARD_SWAP         bit7
#define EFI_POST_BITMAP_DW1_KBD_LOCKED                  bit8
#define EFI_POST_BITMAP_DW1_KBD_NOT_FUNC                bit9
#define EFI_POST_BITMAP_DW1_KBD_CONTR_NOT_FUNC          bit10
#define EFI_POST_BITMAP_DW1_CMOS_MEM_SIZE_DIFF          bit11
#define EFI_POST_BITMAP_DW1_MEM_DECREASED_IN_SIZE       bit12
#define EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR             bit13
#define EFI_POST_BITMAP_DW1_FLOPPY_DRV_0_ERROR          bit14
#define EFI_POST_BITMAP_DW1_FLOPPY_DRV_1_ERROR          bit15
#define EFI_POST_BITMAP_DW1_FLOPPY_CONTR_FAILURE        bit16
#define EFI_POST_BITMAP_DW1_NUM_ATA_DRVS_REDUCED_ERROR  bit17
#define EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET           bit18
#define EFI_POST_BITMAP_DW1_DDC_MONITOR_CONFIG_CHANGE   bit19
#define EFI_POST_BITMAP_DW1_SECOND_DWORD_VALID          bit24

#define EFI_POST_BITMAP_DW1_CHANNEL2_TIMER_ERROR_BIT        0
#define EFI_POST_BITMAP_DW1_MASTER_PIC_ERROR_BIT            1
#define EFI_POST_BITMAP_DW1_SLAVE_PIC_ERROR_BIT             2
#define EFI_POST_BITMAP_DW1_CMOS_BATTERY_FAILURE_BIT        3
#define EFI_POST_BITMAP_DW1_CMOS_SYS_OPTIONS_NOT_SET_BIT    4
#define EFI_POST_BITMAP_DW1_CMOS_CKSUM_ERROR_BIT            5
#define EFI_POST_BITMAP_DW1_CMOS_CONFIG_ERROR_BIT           6
#define EFI_POST_BITMAP_DW1_MOUSE_KEYBOARD_SWAP_BIT         7
#define EFI_POST_BITMAP_DW1_KBD_LOCKED_BIT                  8
#define EFI_POST_BITMAP_DW1_KBD_NOT_FUNC_BIT                9
#define EFI_POST_BITMAP_DW1_KBD_CONTR_NOT_FUNC_BIT          10
#define EFI_POST_BITMAP_DW1_CMOS_MEM_SIZE_DIFF_BIT          11
#define EFI_POST_BITMAP_DW1_MEM_DECREASED_IN_SIZE_BIT       12
#define EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT             13
#define EFI_POST_BITMAP_DW1_FLOPPY_DRV_0_ERROR_BIT          14
#define EFI_POST_BITMAP_DW1_FLOPPY_DRV_1_ERROR_BIT          15
#define EFI_POST_BITMAP_DW1_FLOPPY_CONTR_FAILURE_BIT        16
#define EFI_POST_BITMAP_DW1_NUM_ATA_DRVS_REDUCED_ERROR_BIT  17
#define EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT           18
#define EFI_POST_BITMAP_DW1_DDC_MONITOR_CONFIG_CHANGE_BIT   19
#define EFI_POST_BITMAP_DW1_SECOND_DWORD_VALID_BIT          24

#define EFI_POST_BITMAP_DW2_PCI_MEM_CONFLICT              bit7
#define EFI_POST_BITMAP_DW2_PCI_IO_CONFLICT               bit8
#define EFI_POST_BITMAP_DW2_PCI_IRQ_CONFLICT              bit9
#define EFI_POST_BITMAP_DW2_PNP_MEM_CONFLICT              bit10
#define EFI_POST_BITMAP_DW2_PNP_32BIT_MEM_CONFLICT        bit11
#define EFI_POST_BITMAP_DW2_PNP_IO_CONFLICT               bit12
#define EFI_POST_BITMAP_DW2_PNP_IRQ_CONFLICT              bit13
#define EFI_POST_BITMAP_DW2_PNP_DMA_CONFLICT              bit14
#define EFI_POST_BITMAP_DW2_BAD_PNP_SERIAL_ID             bit15
#define EFI_POST_BITMAP_DW2_BAD_PNP_RESOURCE_DATA_CKSUM   bit16
#define EFI_POST_BITMAP_DW2_STATIC_RESOURCE_CONFLICT      bit17
#define EFI_POST_BITMAP_DW2_NVRAM_CKSUM_ERROR             bit18
#define EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT        bit19
#define EFI_POST_BITMAP_DW2_PRIMARY_OUTPUT_DEV_NOT_FOUND  bit20
#define EFI_POST_BITMAP_DW2_PRIMARY_INPUT_DEV_NOT_FOUND   bit21
#define EFI_POST_BITMAP_DW2_PRIMARY_BOOT_DEV_NOT_FOUND    bit22
#define EFI_POST_BITMAP_DW2_NVRAM_CLEARED_BY_JUMPER       bit23
#define EFI_POST_BITMAP_DW2_NVRAM_DATA_INVALID            bit24
#define EFI_POST_BITMAP_DW2_FDC_RESOURCE_CONFLICT         bit25
#define EFI_POST_BITMAP_DW2_PRI_ATA_CNTR_RESOURCE_ERROR   bit26
#define EFI_POST_BITMAP_DW2_SEC_ATA_CNTR_RESOURCE_ERROR   bit27
#define EFI_POST_BITMAP_DW2_PARALLEL_PORT_RESOURCE_ERROR  bit28
#define EFI_POST_BITMAP_DW2_SERIAL_PORT_1_RESOURCE_ERROR  bit29
#define EFI_POST_BITMAP_DW2_SERIAL_PORT_2_RESOURCE_ERROR  bit30
#define EFI_POST_BITMAP_DW2_AUDIO_RESOURCE_CONFLICT       bit31

#define EFI_POST_BITMAP_DW2_PCI_MEM_CONFLICT_BIT              (7 + 32)
#define EFI_POST_BITMAP_DW2_PCI_IO_CONFLICT_BIT               (8 + 32)
#define EFI_POST_BITMAP_DW2_PCI_IRQ_CONFLICT_BIT              (9 + 32)
#define EFI_POST_BITMAP_DW2_PNP_MEM_CONFLICT_BIT              (10 + 32)
#define EFI_POST_BITMAP_DW2_PNP_32BIT_MEM_CONFLICT_BIT        (11 + 32)
#define EFI_POST_BITMAP_DW2_PNP_IO_CONFLICT_BIT               (12 + 32)
#define EFI_POST_BITMAP_DW2_PNP_IRQ_CONFLICT_BIT              (13 + 32)
#define EFI_POST_BITMAP_DW2_PNP_DMA_CONFLICT_BIT              (14 + 32)
#define EFI_POST_BITMAP_DW2_BAD_PNP_SERIAL_ID_BIT             (15 + 32)
#define EFI_POST_BITMAP_DW2_BAD_PNP_RESOURCE_DATA_CKSUM_BIT   (16 + 32)
#define EFI_POST_BITMAP_DW2_STATIC_RESOURCE_CONFLICT_BIT      (17 + 32)
#define EFI_POST_BITMAP_DW2_NVRAM_CKSUM_ERROR_BIT             (18 + 32)
#define EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT        (19 + 32)
#define EFI_POST_BITMAP_DW2_PRIMARY_OUTPUT_DEV_NOT_FOUND_BIT  (20 + 32)
#define EFI_POST_BITMAP_DW2_PRIMARY_INPUT_DEV_NOT_FOUND_BIT   (21 + 32)
#define EFI_POST_BITMAP_DW2_PRIMARY_BOOT_DEV_NOT_FOUND_BIT    (22 + 32)
#define EFI_POST_BITMAP_DW2_NVRAM_CLEARED_BY_JUMPER_BIT       (23 + 32)
#define EFI_POST_BITMAP_DW2_NVRAM_DATA_INVALID_BIT            (24 + 32)
#define EFI_POST_BITMAP_DW2_FDC_RESOURCE_CONFLICT_BIT         (25 + 32)
#define EFI_POST_BITMAP_DW2_PRI_ATA_CNTR_RESOURCE_ERROR_BIT   (26 + 32)
#define EFI_POST_BITMAP_DW2_SEC_ATA_CNTR_RESOURCE_ERROR_BIT   (27 + 32)
#define EFI_POST_BITMAP_DW2_PARALLEL_PORT_RESOURCE_ERROR_BIT  (28 + 32)
#define EFI_POST_BITMAP_DW2_SERIAL_PORT_1_RESOURCE_ERROR_BIT  (29 + 32)
#define EFI_POST_BITMAP_DW2_SERIAL_PORT_2_RESOURCE_ERROR_BIT  (30 + 32)
#define EFI_POST_BITMAP_DW2_AUDIO_RESOURCE_CONFLICT_BIT       (31 + 32)

#define bit0  1 << 0
#define bit1  1 << 1
#define bit2  1 << 2
#define bit3  1 << 3
#define bit4  1 << 4
#define bit5  1 << 5
#define bit6  1 << 6
#define bit7  1 << 7
#define bit8  1 << 8
#define bit9  1 << 9
#define bit10 1 << 10
#define bit11 1 << 11
#define bit12 1 << 12
#define bit13 1 << 13
#define bit14 1 << 14
#define bit15 1 << 15
#define bit16 1 << 16
#define bit17 1 << 17
#define bit18 1 << 18
#define bit19 1 << 19
#define bit20 1 << 20
#define bit21 1 << 21
#define bit22 1 << 22
#define bit23 1 << 23
#define bit24 1 << 24
#define bit25 1 << 25
#define bit26 1 << 26
#define bit27 1 << 27
#define bit28 1 << 28
#define bit29 1 << 29
#define bit30 1 << 30
#define bit31 1 << 31


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
