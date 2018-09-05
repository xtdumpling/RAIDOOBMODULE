//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
 ATA & ATAPI Specification defined Commands & structures 
 **/

#ifndef _AMI_ATA_ATAPI_H
#define _AMI_ATA_ATAPI_H

#define     IDE_HOB         0x80
#define     IDE_SRST        0x04
#define     IDE_NIEN        0x02

#define     IDE_ABRT        0x04

#define     IDE_BSY         0x80
#define     IDE_DRDY        0x40
#define     IDE_DF          0x20
#define     IDE_DRQ         0x08
#define     IDE_ERR         0x01
#define     IDE_CHK         0x01

#define     SPIN_UP_REQUIRED1               0x37C8
#define     SPIN_UP_REQUIRED2               0x738c

#define     ATAPI_SIGNATURE                 0xEB14
#define     ATAPI_FORMAT_UNIT               0x04
#define     ATAPI_INQUIRY                   0x12
#define     ATAPI_MODE_SELECT               0x55
#define     ATAPI_MODE_SENSE                0x5A
#define         READ_WRITE_ERROR            0x01    
#define         CACHING_PAGE                0x08    
#define         REMOVABLE_BLOCK_CAPABILITIES 0x1B   
#define         TIMER_PROTECT_PAGE          0x1C    
#define         RETURN_ALL_PAGES            0x3F    
#define     ATAPI_PREVENT_ALLOW_MEDIA_REMOVAL 0x1E
#define     ATAPI_READ_10                   0x28
#define     ATAPI_READ_12                   0xA8
#define     ATAPI_READ_CAPACITY             0x25
#define     ATAPI_READ_FORMAT_CAPACITIES    0x23
#define     ATAPI_REQUEST_SENSE             0x03
#define     ATAPI_SEEK                      0x2B
#define     ATAPI_START_STOP_UNIT           0x1B
#define         STOP_DISC                   0x00
#define         START_DISC                  0x01
#define         EJECT_DISC                  0x02
#define         LOAD_DISC                   0x03
#define     ATAPI_TEST_UNIT_READY           0x00
#define     ATAPI_VERIFY                    0x2F
#define     ATAPI_WRITE_10                  0x2A
#define     ATAPI_WRITE_12                  0xAA
#define     ATAPI_WRITE_AND_VERIFY          0x2E
#define     ATAPI_GET_CONFIGURATION         0x46
#define         GET_PROFILE_LIST            0x00
#define         FEATURE_DISCRIPTOR          0x02    
#define         GET_REMOVEABLE_MEDIUM_FEATURE   0x03


#define     MEDIUM_NOT_PRESENT              0x01
#define     BECOMING_READY                  0x02
#define     MEDIA_CHANGED                   0x03
#define     DEVICE_ERROR                    0x04
#define     WRITE_PROTECTED_MEDIA           0x05
#define     POWER_ON_OR_DEVICE_RESET        0x06
#define     ILLEGAL_REQUEST                 0x07


#define     DEVICE_RESET                    0x08
#define     PACKET_COMMAND                  0xA0
#define         BUSMASTER_DMA               0x01
#define         OVL                         0x02

#define     IDENTIFY_COMMAND                0xEC
#define     IDENTIFY_PACKET_COMMAND         0xA1

#define     READ_SECTORS                    0x20
#define     READ_SECTORS_EXT                0x24

#define     READ_MULTIPLE                   0xC4
#define     READ_MULTIPLE_EXT               0x29

#define     WRITE_SECTORS                   0x30
#define     WRITE_SECTORS_EXT               0x34

#define     WRITE_MULTIPLE                  0xC5
#define     WRITE_MULTIPLE_EXT              0x39

#define     READ_DMA                        0xC8
#define     READ_DMA_EXT                    0x25

#define     WRITE_DMA                       0xCA
#define     WRITE_DMA_EXT                   0x35

#define     FLUSH_CACHE                     0xE7
#define     FLUSH_CACHE_EXT                 0xEA

#define     SET_MULTIPLE_MODE               0xC6
#define     GET_MEDIA_STATUS                0xDA

#define     EXECUTE_DEVICE_DIAGNOSTIC       0x90

#define     SET_FEATURE_COMMAND             0xEF
#define         SET_TRANSFER_MODE           0x03
#define         ADV_POWER_MANAGEMENT        0x05
#define         ENABLE_POWERUP_IN_STANDBY    0x06
#define         SET_DEVICE_SPINUP           0x07
#define         DISABLE_MEDIA_STATUS_NOTIFICATION 0x31
#define         ENABLE_MEDIA_STATUS_NOTIFICATION  0x95
#define         ACOUSTIC_MANAGEMENT_ENABLE  0x42
#define         DISABLE_ADV_POWER_MANAGEMENT 0x85
#define         DISABLE_POWERUP_IN_STANDBY   0x86
#define         ACOUSTIC_MANAGEMENT_DISABLE 0xC2
#define         DISABLE_SATA2_SOFTPREV      0x90

#define     PIO_FLOW_CONTROL                0x08            
#define     MWDMA_MODE                      0x20            
#define     UDMA_MODE                       0x40            

#define     ATA_SECTOR_BYTES                512
#define     MAX_SECTOR_COUNT_PIO            256 
#define     MAX_SECTOR_COUNT_PIO_48BIT      65536
#define     CDROM_BLOCK_SIZE                2048
#define     LS120_BLOCK_SIZE                512

#define     STANDBY_IMMEDIATE               0xE0
#define     IDLE_IMMEDIATE                  0xE1
#define     STANDBY_CMD                     0xE2
#define     IDLE_CMD                        0xE3
#define     CHECK_POWER_MODE                0xE5
#define     SLEEP_CMD                       0xE6

#define     READ_NATIVE_MAX_ADDRESS         0xF8
#define     READ_NATIVE_MAX_ADDRESS_EXT     0x27
#define     SET_MAX_ADDRESS                 0xF9
#define         SET_MAX_SET_PASSWORD        0x01
#define         SET_MAX_LOCK                0x02
#define         SET_MAX_UNLOCK              0x03
#define         SET_MAX_FREEZE_LOCK         0x04
#define     SET_MAX_ADDRESS_EXT             0x37

#define     READ_LOG_EXT                    0x2F
#define     IDENTIFY_DEVICE_DATA_LOG        0x30
#define     SERIAL_ATA_SETTINGS_PAGE        0x08
#define     DEVSLP_TIMING_VARIABLES_OFFSET  0x30
#define     DEVSLEEP_EXIT_TIMEOUT           20
#define     MINIMUM_DEVSLP_ASSERTION_TIME   10


#define     DIRECT_ACCESS_DEVICE            0
#define     SEQUENTIAL_ACCESS_DEVICE        1
#define     WORM                            4
#define     CDROM_DEVICE                    5
#define     OPTICAL_MEMORY_DEVICE           7


///  Equates used for Acoustic Flags
#define     ACOUSTIC_SUPPORT_DISABLE            0x00
#define     ACOUSTIC_SUPPORT_ENABLE             0x01
#define     ACOUSTIC_LEVEL_BYPASS               0xFF
#define     ACOUSTIC_LEVEL_MAXIMUM_PERFORMANCE  0xFE
#define     ACOUSTIC_LEVEL_QUIET                0x80

///  Equates used for DiPM Support
#define     DIPM_SUB_COMMAND                            0x03  // Count value in SetFeature identification : 03h  Device-initiated interface power state transitions 
#define     DIPM_ENABLE                                 0x10
#define     DIPM_DISABLE                                0x90
#define     IDENTIFY_DIPM_ENABLED                       0x08  // Identify Data Word 79 Bit 3 : Device initiating interface power management Enabled
#define     IDENTIFY_DIPM_SUPPORT                       0x08  // Identify Data Word 78 Bit 3 : Supports Device initiating interface power management
#define     IDENTIFY_DIPM_HIIPM_REQUESTS_CAPABLE        0x200 // Identify Data Word 76 Bit 9 : Receipt of host-initiated interface power management requests

///  Equates used for DevSleep Support
#define     DEVSLEEP_SUB_COMMAND                        0x09  // Count value in SetFeature identification : 09h  Device Sleep 
#define     DEVSLEEP_ENABLE                             0x10
#define     DEVSLEEP_DISABLE                            0x90
#define     IDENTIFY_DEVSLEEP_ENABLED                   0x100 // Identify Data Word 79 Bit 8 : Device Sleep Enabled
#define     IDENTIFY_DEVSLEEP_SUPPORT                   0x100 // Identify Data Word 78 Bit 8 : Supports Device Sleep
#define     IDENTIFY_DEVSLP_TO_REDUCED_PWRSTATE_CAPABLE 0x80  // Identify Data Word 77 Bit 7 : Device Sleep in reduced Power state capable


#pragma pack(1)

typedef struct {
    UINT32  DataLength;
    UINT8   Reserved[2];
    UINT16  CurrentProfile;
} GET_CONFIGURATION_HEADER;

/// Based on ATA/ATAPI-6
typedef struct _IDENTIFY_DATA{ 

    UINT16  General_Config_0;
    UINT16  Reserved_1;
    UINT16  Special_Config_2;
    UINT16  Reserved_3;
    UINT16  Reserved_4;
    UINT16  Reserved_5;
    UINT16  Reserved_6;
    UINT16  Reserved_7;
    UINT16  Reserved_8;
    UINT16  Reserved_9;
    UINT8   Serial_Number_10[20];
    UINT16  Reserved_20;
    UINT16  Reserved_21;
    UINT16  Reserved_22;
    UINT8   Firmware_Revision_23[8];
    UINT8   Model_Number_27[40];
    UINT16  Maximum_Sector_Multiple_Command_47;
    UINT16  Trusted_Computing_Support;
    UINT16  Capabilities_49;
    UINT16  Capabilities_50;
    UINT16  PIO_Mode_51;
    UINT16  Reserved_52;
    UINT16  Valid_Bits_53;
    UINT16  Reserved_54_58[5];
    UINT16  Valid_Bits_59;
    UINT32  Addressable_Sector_60;
    UINT16  SingleWord_DMA_62;
    UINT16  MultiWord_DMA_63;
    UINT16  PIO_Mode_64;
    UINT16  Min_Multiword_DMA_timing_65;
    UINT16  Manuf_Multiword_DMA_timing_66;
    UINT16  Min_PIO_Mode_timing_67;
    UINT16  Min_PIO_Mode_timing_68;
    UINT16  Reserved_69_74[6];
    UINT16  Queue_Depth_75;
    UINT16  Reserved_76_79[4];
    UINT16  Major_Revision_80;
    UINT16  Minor_Revision_81;
    UINT16  Command_Set_Supported_82;
    UINT16  Command_Set_Supported_83;
    UINT16  Command_Set_Supported_84;
    UINT16  Command_Set_Enabled_85;
    UINT16  Command_Set_Enabled_86;
    UINT16  Command_Set_Enabled_87;
    UINT16  UDMA_Mode_88;
    UINT16  Time_security_Earse_89;
    UINT16  Time_Esecurity_Earse_90;
    UINT16  Current_Power_Level_91;
    UINT16  Master_Password_Rev_92;
    UINT16  Hard_Reset_Value_93;
    UINT16  Acoustic_Level_94;
    UINT16  Reserved_95_99[5];
    UINT64  LBA_48;
    UINT16  Reserved_104_126[23];
    UINT16  Status_Notification_127;
    UINT16  Security_Status_128;
    UINT16  Reserved_129_159[31];
    UINT16  CFA_Power_Mode_160;
    UINT16  Reserved_161_175[15];
    UINT16  Media_Serial_Number_176_205[30];
    UINT16  Reserved_206_254[49];
    UINT16  Checksum_255;
} IDENTIFY_DATA;

#pragma pack()

typedef enum {  
  ATA = 0,
  ATAPI = 1,
  PMPORT = 2
} DEVICE_TYPE;

typedef enum {
    Obsolete,
    Removabledisk,
    CDROM,
    CDR,
    CDRW,
    DVDROM,
    DVDRSequentialrecording,
    DVDRAM,
    DVDRWRestrictedOverwrite,
    DVDRWSequentialrecording,
    DVDRDualLayerSequential,
    DVDRDualLayerJumprecording,
    DVDRWDualLayer,
    DVDDownloaddiscrecording,
    DVDRW,
    DVDR,
    BDROM,
    BDRSequentialRecording,
    BDRRandomRecordingMode,
    BDRE,
    HDDVDROM,
    HDDVDR,
    HDDVDRAM,
    HDDVDRW,
    HDDVDRDualLayer,
    HDDVDRWDualLayer,
    UnknownType
} ODD_TYPE;

typedef enum {
    CaddySlotType,
    TrayType,
    PopupType,
    ReservedType,
    EmbeddedChangerWithIndividuallyChangeablediscs,
    EmbeddedChangerUsingCartridgeMmechanism,
    ReservedLoadingType
} ODD_LOADING_TYPE;

typedef struct { 
    UINT8                               *PacketBuffer;
    UINT8                               DeviceType;
    UINT8                               Lun;
    UINT8                               PacketSize;
    UINT16                              BlockSize;
    BOOLEAN                             ReadOnly;
    UINT8                               Atapi_Status;
    UINT8                               *InquiryData;                           
    UINT32                              InquiryDataSize;
    ODD_TYPE                            OddType;                            
    ODD_LOADING_TYPE                    OddLoadingType;                         
} ATAPI_DEVICE;

/**
    * HDD Security commands
*/

#define     SECURITY_SET_PASSWORD           0xF1
#define     SECURITY_UNLOCK                 0xF2
#define     SECURITY_ERASE_PREPARE          0xF3
#define     SECURITY_ERASE_UNIT             0xF4
#define     SECURITY_FREEZE_LOCK            0xF5
#define     SECURITY_DISABLE_PASSWORD       0xF6
#define         SECURITY_BUFFER_LENGTH      512     
#define     DEV_CONFIG_FREEZE_LOCK          0xB1
#define         DEV_CONFIG_FREEZE_LOCK_FEATURES 0xC1



/**
     * Hdd Smart Commands
 */

#define     SMART_COMMAND                   0xB0    
#define         SMART_READ_DATA             0xD0
#define         SMART_READ_THRESHOLD        0xD1    
#define         SMART_AUTOSAVE              0xD2
#define         SMART_EXECUTE_OFFLINE_IMMEDIATE 0xD4
#define         SMART_READLOG               0xD5
#define         SMART_WRITELOG              0xD6
#define         SMART_ENABLE_CMD            0xD8
#define         SMART_DISABLE_CMD           0xD9
#define         SMART_RETURN_STATUS         0xDA
#define         THRESHOLD_NOT_EXCEEDED      0xC24F
#define         THRESHOLD_EXCEEDED          0x2CF4
#define         SMART_ABORT_SELF_TEST_SUBROUTINE 0x7F

/**
     * Equates used for Smart command support 
 */

#define     LOG_DIRECTORY                0x00
#define     SUM_SMART_ERROR_LOG          0x01 
#define     COMP_SMART_ERROR_LOG         0x02 
#define     SMART_SELFTEST_LOG           0x06
#define     HOST_VENDOR_SPECIFIC         0x80 
#define     DEVICE_VENDOR_SPECIFIC       0xA0 

#define     DISABLE_AUTOSAVE       0x00
#define     ENABLE_AUTOSAVE        0xF1


/**
 * Opal Security Spec defined Commands
 */

#define     TRUSTED_NON_DATA            0x5B
#define     TRUSTED_RECEIVE             0x5C
#define     TRUSTED_SEND                0x5E

#define     TPER_FEATURE_CODE           0x0100
#define     LOCKING_FEATURE_CODE        0x0200
#define     OPAL_SSC_FEATURE_CODE       0x0002

#endif // #ifndef _AMI_ATA_ATAPI_H

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
