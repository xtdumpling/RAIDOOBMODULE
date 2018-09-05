/** @file
  Core BIOS Messages

@copyright
  Copyright (c) 2008 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef CORE_BIOS_MSG_H
#define CORE_BIOS_MSG_H

#include <PiPei.h>
#include <MkhiMsgs.h>

#pragma pack(1)

#define CBM_RESET_REQ_CMD           0x0B
#define CBM_RESET_REQ_CMD_ACK       0x8B
#define GEN_END_OF_POST_CMD         0x0C
#define GEN_END_OF_POST_CMD_ACK     0x8C
#define GEN_SET_DEBUG_MEMORY_CMD    0x11
#define GEN_CORE_BIOS_DONE_CMD      0x05
#define GEN_CORE_BIOS_DONE_CMD_ACK  0x85

//
// HECI Client Address - Core Messages
//    Core messages to coordinate memory initialization and UMA allocation with ME
//    as well as to inform ME of the end of POST event
//
#define HECI_CLIENT_CORE_MSG_DISPATCHER 0x07
#define HOST_FIXED_ADDRESS              0x00

///
/// Header for all HECI message message placed in an Intel MEI circular buffer
///
typedef union _HECI_MESSAGE_HEADER {
  UINT32  Data;
  struct {
    /*
    This is the logical address of the Intel ME client of the message. This address is assigned 
    during ME firmware initialization. 
  */
    UINT32  MeAddress       : 8;
  /*
    This is the logical address of the Host client of the message. This address is assigned 
    when the host client registers itself with the Host MEI driver. 
  */
    UINT32  HostAddress     : 8;
    /*
    This is the message length in bytes, not including the MEI_MESSAGE_HEADER. A value of 0 
    indicates no additional bytes are part of the message. 
  */
    UINT32  Length          : 9;
    UINT32  Reserved        : 6;
    UINT32  MessageComplete : 1; /// < Indicates the last message of the list
  } Fields;
} HECI_MESSAGE_HEADER;

///
/// Reset request message data
///
typedef struct _CBM_RESET_REQ_DATA {
  UINT8 RequestOrigin;
  UINT8 ResetType;
} CBM_RESET_REQ_DATA;

///
/// Reset request MKHI message
///
typedef struct _CBM_RESET_REQ {
  MKHI_MESSAGE_HEADER MKHIHeader;
  CBM_RESET_REQ_DATA  Data;
} CBM_RESET_REQ;

///
/// Reset request message
///
typedef struct _MKHI_CBM_RESET_REQ {
  HECI_MESSAGE_HEADER Header;
  CBM_RESET_REQ       Msg;
} MKHI_CBM_RESET_REQ;

///
/// Reset request MKHI ack message
///
typedef struct _CBM_RESET_ACK {
  MKHI_MESSAGE_HEADER MKHIHeader;
} CBM_RESET_ACK;

///
/// Reset request ack message
///
typedef struct _MKHI_CBM_RESET_ACK {
  HECI_MESSAGE_HEADER Header;
  CBM_RESET_ACK       Msg;
} MKHI_CBM_RESET_ACK;

//
// ASF Watch Dog Timer
//
#define ASF_MANAGEMENT_CONTROL    0x02
#define ASF_SUB_COMMAND_START_WDT 0x13
#define ASF_SUB_COMMAND_STOP_WDT  0x14

//
// HECI ASF Command
//
#define ASF_START_WDT_BYTE_COUNT                      0x15
#define ASF_START_WDT_VERSION_NUMBER                  0x10
#define ASF_START_WDT_EVENT_SENSOR_TYPE               0x23
#define ASF_START_WDT_EVENT_TYPE                      0x6F
#define ASF_START_WDT_EVENT_OFFSET                    0x00
#define ASF_START_WDT_EVENT_SOURCE_TYPE_OS            0x48
#define ASF_START_WDT_EVENT_SOURCE_TYPE_BIOS          0x00
#define ASF_START_WDT_EVENT_SOURCE_TYPE               0x00  ///< 0x00 - BIOs, 0x48 - OS
#define ASF_START_WDT_EVENT_SEVERITY                  0x10  ///< critical
#define ASF_START_WDT_SENSOR_DEVICE                   0xFF  ///< unspecified
#define ASF_START_WDT_SENSOR_NUMBER                   0xFF  ///< unspecified
#define ASF_START_WDT_ENTITY                          0x00  ///< unspecified
#define ASF_START_WDT_ENTITY_INSTANCE                 0x00  ///< unspecified
#define ASF_START_WDT_EVENT_DATA_BYTE_0               0x40
#define ASF_START_WDT_EVENT_DATA_BYTE_1               0x02  ///< 0x02 BIOS POST WDT Timeout, 0x04 OS WDT timeout
#define ASF_START_WDT_EVENT_DATA_BYTE_1_BIOS_TIMEOUT  0x02
#define ASF_START_WDT_EVENT_DATA_BYTE_1_OS_TIMEOUT    0x04

#define ASF_STOP_WDT_VERSION_NUMBER                   0x10
#define ASF_STOP_WDT_BYTE_COUNT                       0x02

#define ASF_START_BIOS_WDT  0
#define ASF_START_OS_WDT    1

///
/// ASF Start Watch Dog
///
typedef struct _ASF_START_WDT {
  UINT8 Command;
  UINT8 ByteCount;
  UINT8 SubCommand;
  UINT8 VersionNumber;
  UINT8 TimeoutLow;
  UINT8 TimeoutHigh;
  UINT8 EventSensorType;
  UINT8 EventType;
  UINT8 EventOffset;
  UINT8 EventSourceType;
  UINT8 EventSeverity;
  UINT8 SensorDevice;
  UINT8 SensorNumber;
  UINT8 Entity;
  UINT8 EntityInstance;
  UINT8 EventData[8];
} ASF_START_WDT;

#define ASF_START_WDT_LENGTH  0x17

///
/// ASF Stop Watch Dog
///
typedef struct _ASF_STOP_WDT {
  UINT8 Command;
  UINT8 ByteCount;
  UINT8 SubCommand;
  UINT8 VersionNumber;
} ASF_STOP_WDT;

#define ASF_STOP_WDT_LENGTH 0x04

//
// HECI Header Definitions for Core BIOS Messages
//
#define CBM_END_OF_POST_HECI_HDR          0x80080001
#define CBM_END_OF_POST_RESPONSE_HECI_HDR 0x80010001
#define CBM_RESET_REQUEST_HECI_HDR        0x80030001
#define CBM_RESET_RESPONSE_HECI_HDR       0x80020001

//
// Enumerations used in Core BIOS Messages
//
// End Of Post Codes.
//
#define CBM_EOP_EXITING_G3  0x01
#define CBM_EOP_RESERVED    0x02
#define CBM_EOP_EXITING_S3  0x03
#define CBM_EOP_EXITING_S4  0x04
#define CBM_EOP_EXITING_S5  0x05

//
// Reset Request Origin Codes.
//
#define CBM_RR_REQ_ORIGIN_BIOS_MEMORY_INIT  0x01
#define CBM_RR_REQ_ORIGIN_BIOS_POST         0x02
#define CBM_RR_REQ_ORIGIN_MEBX              0x03

//
// Reset Type Codes.
//
#define CBM_HRR_GLOBAL_RESET  0x01

//
// Reset Response Codes.
//
#define CBM_HRR_RES_REQ_NOT_ACCEPTED  0x01

//
// definitions for ICC MEI Messages
//
#define IBEX_PEAK_PLATFORM    0x00010000
#define COUGAR_POINT_PLATFORM 0x00020000
#define LYNX_POINT_PLATFORM   0x00030000
#define SKYLAKE_PCH_PLATFORM  0x00040000

typedef enum {
  ICC_STATUS_SUCCESS                           = 0x0,
  ICC_STATUS_FAILURE,
  ICC_STATUS_INCORRECT_API_VERSION,
  ICC_STATUS_INVALID_FUNCTION,
  ICC_STATUS_INVALID_BUFFER_LENGTH,
  ICC_STATUS_INVALID_PARAMS,
  ICC_STATUS_FLASH_WEAR_OUT_VIOLATION,
  ICC_STATUS_FLASH_CORRUPTION,
  ICC_STATUS_PROFILE_NOT_SELECTABLE_BY_BIOS,
  ICC_STATUS_TOO_LARGE_PROFILE_INDEX,
  ICC_STATUS_NO_SUCH_PROFILE_IN_FLASH,
  ICC_STATUS_CMD_NOT_SUPPORTED_AFTER_END_OF_POST,
  ICC_STATUS_NO_SUCH_RECORD,
  ICC_STATUS_RESERVED13,
  ICC_STATUS_RESERVED14,
  ICC_STATUS_TOO_LARGE_UOB_RECORD,
  ICC_STATUS_RESERVED16,
  ICC_STATUS_REGISTER_IS_LOCKED,
  ICC_STATUS_RESERVED18,
  ICC_STATUS_RESERVED19,
  ICC_STATUS_RESERVED20,
  ICC_STATUS_RESERVED21,
  ICC_STATUS_RESERVED22,
  ICC_STATUS_RESERVED23,
  ICC_STATUS_UOB_RECORD_IS_ALREADY_INVALID,
  ICC_STATUS_RESERVED25,   
  ICC_STATUS_RESERVED26,
  ICC_STATUS_RESERVED27,
  ICC_STATUS_RANGE_VIOLATION_FREQ_TOO_HIGH,   
  ICC_STATUS_RESERVED29,
  ICC_STATUS_RESERVED30,
  ICC_STATUS_RESERVED31,
  ICC_STATUS_RESERVED32,
  ICC_STATUS_RESERVED33,
  ICC_STATUS_RESERVED34,
  ICC_STATUS_RESERVED35,
  ICC_STATUS_RANGE_VIOLATION_FREQ_TOO_LOW,
  ICC_STATUS_RESERVED37,
  ICC_STATUS_RESERVED38,
  ICC_STATUS_RESERVED39,
  ICC_STATUS_RESERVED40,
  ICC_STATUS_RESERVED41,
  ICC_STATUS_RESERVED42,
  ICC_STATUS_RESERVED43,
  ICC_STATUS_SSC_MODE_CHANGE_NOT_SUPPORTED,
  ICC_STATUS_RESERVED45,
  ICC_STATUS_RESERVED46,
  ICC_STATUS_RESERVED47,
  ICC_STATUS_RESERVED48,
  ICC_STATUS_RESERVED49,
  ICC_STATUS_RESERVED50,
  ICC_STATUS_RESERVED51,
  ICC_STATUS_NO_SUCH_TARGET_ID,
  ICC_STATUS_NO_SUCH_REGISTER,
  ICC_STATUS_RESERVED54,
  ICC_STATUS_BUFFER_TOO_SMALL,
  ICC_STATUS_RESERVED56,
  ICC_STATUS_WAITING_FOR_POWER_CYCLE,
  ICC_STATUS_SURVIVABILITY_TABLE_ACCESS_VIOLATION,
  ICC_STATUS_SURVIVABILITY_TABLE_TOO_LARGE,
  ICC_STATUS_NO_SUCH_EID,
  ICC_STATUS_RESERVED61,
  ICC_STATUS_UNSPECIFIED_ERROR               = 0xFFFF
} ICC_MEI_CMD_STATUS;

typedef enum {
  LOCK_ICC_REGISTERS                        = 0x2,
  SET_CLOCK_ENABLES                         = 0x3,
  GET_ICC_PROFILE                           = 0x4,
  SET_ICC_PROFILE                           = 0x5,
  GET_ICC_CLOCKS_CAPABILITIES               = 0x6,
  GET_OEM_CLOCK_RANGE_DEFINITION_RECORD     = 0x7,
  GET_ICC_RECORD                            = 0x8,
  READ_ICC_REGISTER                         = 0x9,
  WRITE_ICC_REGISTER                        = 0xa,
  WRITE_UOB_RECORD                          = 0xb,
  READ_MPHY_SETTINGS                        = 0xe,
  WRITE_MPHY_SETTINGS                       = 0xf,
  ///
  /// Commands 0-15 not used on SKL PCH platform
  ///
  ICC_GET_PROFILE_CMD                       = 0x10,
  ICC_SET_PROFILE_CMD                       = 0x11,
  ICC_GET_REGISTERS_LOCK_CMD                = 0x12,
  ICC_SET_REGISTERS_LOCK_CMD                = 0x13,
  ICC_GET_CLOCK_CAPABILITIES_CMD            = 0x14,
  ICC_GET_CLOCK_RANGE_DEF_REC_CMD           = 0x15,
  ICC_GET_RECORD_CMD                        = 0x16,
  ICC_SET_RECORD_CMD                        = 0x17,
  ICC_GET_HSIO_SETTINGS_CMD                 = 0x18,
  ICC_SET_HSIO_SETTINGS_CMD                 = 0x19,
  ICC_GET_CLOCK_SETTINGS_CMD                = 0x1A,
  ICC_SET_CLOCK_SETTINGS_CMD                = 0x1B,
} ICC_MEI_COMMAND_ID;

typedef struct {
  UINT32              ApiVersion;
  ICC_MEI_COMMAND_ID  IccCommand;
  ICC_MEI_CMD_STATUS  IccResponse;
  UINT32              BufferLength;
  UINT32              Reserved;
} ICC_HEADER;

///
/// Get/Set ICC Register Lock
///
typedef struct {
  ICC_HEADER  Header;
} ICC_GET_REG_LOCK_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT8       LockState;
  UINT8       LockFlags;
  UINT16      Reserved;
} ICC_GET_REG_LOCK_RESPONSE;

typedef union {
  ICC_GET_REG_LOCK_MESSAGE   message;
  ICC_GET_REG_LOCK_RESPONSE  response;
} ICC_GET_REG_LOCK_BUFFER;

typedef struct {
  ICC_HEADER  Header;
  UINT8       LockState;
  UINT8       PaddingA;
  UINT16      PaddingB;
} ICC_SET_REG_LOCK_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
} ICC_SET_REG_LOCK_RESPONSE;

typedef union {
  ICC_SET_REG_LOCK_MESSAGE   message;
  ICC_SET_REG_LOCK_RESPONSE  response;
} ICC_SET_REG_LOCK_BUFFER;

///
/// Get/Set ICC Profile
///
#define MAX_NUM_ICC_PROFILES  16

typedef struct {
  ICC_HEADER  Header;
} ICC_GET_PROFILE_MESSAGE;

typedef struct _ICC_PROFILE_DESC
{
   UINT8       IccProfileName[24];
   UINT8       IccProfileBase[24];
} ICC_PROFILE_DESC;

typedef struct {
  ICC_HEADER    Header;
  UINT8         IccNumOfProfiles;
  union {
    UINT8 Data;
    struct {
      UINT8     IccFailsafeProfile  : 4;
      UINT8     Reserved            : 3;
      UINT8     IccRuntimeSelect    : 1;
    } Fields;
  } OemParams;
  UINT8             IccProfileIndex;   
  UINT8             Reserved;

  ICC_PROFILE_DESC  IccProfileDesc[MAX_NUM_ICC_PROFILES];
} ICC_GET_PROFILE_RESPONSE;

typedef union {
  ICC_GET_PROFILE_MESSAGE   message;
  ICC_GET_PROFILE_RESPONSE  response;
} ICC_GET_PROFILE_BUFFER;

typedef struct {
  ICC_HEADER  Header;
  UINT8       BiosIccProfile;
  UINT8       PaddingA;
  UINT16      PaddingB;
} ICC_SET_PROFILE_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
} ICC_SET_PROFILE_RESPONSE;

typedef union {
  ICC_SET_PROFILE_MESSAGE   message;
  ICC_SET_PROFILE_RESPONSE  response;
} ICC_SET_PROFILE_BUFFER;

///
/// Get/Set ICC Clock Capabilites
///
typedef struct {
  ICC_HEADER  Header;
} ICC_GET_CLK_CAPABILITIES_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT32      VersionNumber;
  UINT8       IccHwSku;
  UINT8       Reserved;
  UINT16      MaxSusramRecordSize;
  UINT64      IccSkuEnforcementTable;
  UINT32      IccBootStatusReport;
} ICC_GET_CLK_CAPABILITIES_RESPONSE;

typedef union {
  ICC_GET_CLK_CAPABILITIES_MESSAGE  message;
  ICC_GET_CLK_CAPABILITIES_RESPONSE response;
} ICC_GET_CLK_CAPABILITIES_BUFFER;

///
/// Get/Set Clock Settings
///
// Used for the FLAGS parameter of Get/Set Clock Settings
typedef union _ICC_CLOCK_SETTINGS_FLAGS
{
   UINT8 u;
   struct
   {
      UINT8 RoundDown   : 1;
      UINT8 Reserved    : 7;
   } Flags;
} ICC_CLOCK_SETTINGS_FLAGS;

///
/// Used for the CURRENT FLAGS parameter of Get/Set Clock Settings
///
typedef union _ICC_CLOCK_SETTINGS_CURRENT_FLAGS
{
   UINT16 u;
   struct
   {
      UINT16 Ratio              : 2;
      UINT16 Reserved           : 1;
      UINT16 AppliedOnWarmRst   : 1;
      UINT16 Type               : 2;
      UINT16 SetToDefault       : 1;
      UINT16 Disabled           : 1;
      UINT16 DynSettingsApplied : 1;
      UINT16 WarmRstPending     : 1;
      UINT16 PwrCyclePending    : 1;
      UINT16 Reserved1          : 5;
   } Flags;
} ICC_CLOCK_SETTINGS_CURRENT_FLAGS;

///
/// Used for the SUPPORTED FLAGS parameter of Get/Set Clock Settings
///
typedef union _ICC_CLOCK_SETTINGS_SUPPORTED_FLAGS
{
   UINT16 u;
   struct
   {
      UINT16 Ratio              : 1;
      UINT16 DownSpread         : 1;
      UINT16 UpSpread           : 1;
      UINT16 CenterSpread       : 1;
      UINT16 ChangeAllowed      : 1;
      UINT16 HaltAllowed        : 1;
      UINT16 PCIe               : 1;
      UINT16 Bclk               : 1;
      UINT16 Reserved           : 8;
   } Flags;
} ICC_CLOCK_SETTINGS_SUPPORTED_FLAGS;

///
/// Combined Request and Response structure for Get/Set Clock Settings
///
typedef struct _ICC_GETSET_CLOCK_SETTINGS_REQRSP
{
   ICC_HEADER                         Header;
   UINT8                              ReqClock;
   UINT8                              SettingType;
   ICC_CLOCK_SETTINGS_FLAGS           Flags;
   UINT8                              Reserved;
   UINT32                             Freq;
   UINT32                             UserFreq;
   UINT32                             MaxFreq;
   UINT32                             MinFreq;
   UINT8                              SscMode;
   UINT8                              SscPercent;
   UINT8                              MaxSscPercent;
   UINT8                              Reserved1;
   ICC_CLOCK_SETTINGS_CURRENT_FLAGS   CurrentFlags;
   ICC_CLOCK_SETTINGS_SUPPORTED_FLAGS SupportFlags;
} ICC_GETSET_CLOCK_SETTINGS_REQRSP;

typedef union {
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  message;
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  response;
} ICC_GET_CLOCK_SETTINGS_BUFFER;

typedef union {
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  message;
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  response;
} ICC_SET_CLOCK_SETTINGS_BUFFER;

///
/// Defines clock settings constraints for next-level user record.
///
typedef union _ICC_SINGLE_CLK_RANGE_DEF
{
   UINT32 Dword[2];
   struct 
   {
      UINT32 ClkFreqMax            :16;
      UINT32 ClkFreqMin            :16;
      UINT32 SscChangeAllowed      :1;
      UINT32 SscSprModeCtrlUp      :1;
      UINT32 SscSprModeCtrlCenter  :1;
      UINT32 SscSprModeCtrlDown    :1;
      UINT32 SscHaltAllowed        :1;
      UINT32 Reserved1             :3;
      UINT32 SscSprPercentMax      :8;
      UINT32 ClockUsage            :8;
      UINT32 Reserved2             :8;
   } Fields;
} ICC_SINGLE_CLK_RANGE_DEF;

typedef struct _ICC_CLK_RANGE_DEF_REC
{
   ICC_SINGLE_CLK_RANGE_DEF   SingleClkRangeDef[2];
} ICC_CLK_RANGE_DEF_REC;

typedef struct {
  ICC_HEADER  Header;
  UINT8       Mode;
  UINT8       RequiredRecord;
  UINT16      Reserved;
} ICC_GET_CLOCK_RANGE_MESSAGE;

typedef struct {
  ICC_HEADER             Header;
  ICC_CLK_RANGE_DEF_REC  Crdr;
} ICC_GET_CLOCK_RANGE_RESPONSE;

typedef union {
  ICC_GET_CLOCK_RANGE_MESSAGE  message;
  ICC_GET_CLOCK_RANGE_RESPONSE  response;
} ICC_GET_CLOCK_RANGE_BUFFER;

#define ICC_RESPONSE_MODE_WAIT      0
#define ICC_RESPONSE_MODE_SKIP      1
#define ICC_LOCK_ACCESS_MODE_SET    0
#define ICC_LOCK_ACCESS_MODE_GET    1
#define ICC_LOCK_MASK_COUNT         255
#define WRITE_ICC_REG_BUNDLE_COUNT  1           ///< 1 bundle for 1 DWORD register write
#define ADDRESS_MASK_FIXED_DATA     0x00017F01  ///< Target ID = 7F (Aux), MWM = 1(16 bits)
#define WRITE_ICC_RECORD_FLAGS      0x00012010  ///< 16 byte record length, No param section, valid bit

typedef struct _ICC_REG_BUNDLES
{
  UINT32 BundlesCnt :16; ///< Bundles Count - number of Address Mask entries
  UINT32 AU         :1;  ///< AU=1 -> All regisaters are Unlocked
  UINT32 Reserved   :15;
} ICC_REG_BUNDLES;

typedef struct {
  ICC_REG_BUNDLES RegBundles;
  UINT32          RegMask[ICC_LOCK_MASK_COUNT];
} ICC_LOCK_REGS_INFO;

typedef struct {
  ICC_HEADER          Header;
  UINT8               AccessMode;
  UINT8               Parameters;
  UINT8               Reserved[2];
  ICC_LOCK_REGS_INFO  LockRegInfo;
} ICC_LOCK_REGISTERS_MESSAGE;

typedef struct {
  ICC_HEADER          Header;
  UINT32              Reserved;
  ICC_LOCK_REGS_INFO  LockRegInfo;
} ICC_LOCK_REGISTERS_RESPONSE;

typedef union {
  ICC_LOCK_REGISTERS_MESSAGE  message;
  ICC_LOCK_REGISTERS_RESPONSE response;
} ICC_LOCK_REGISTERS_BUFFER;

typedef union _ICC_ADDRESS_MASK
{
  UINT32 AddressMaskData;
  struct
  {
    UINT32 MaskWidthModifier :2;
    UINT32 Offset            :6;
    UINT32 TargetId          :7;
    UINT32 Reserved          :1;
  }Fields;
} ICC_ADDRESS_MASK;

typedef struct _ICC_DWORD_RECORD
{
  UINT32            RecordFlags;
  ICC_REG_BUNDLES   BundleCount;
  ICC_ADDRESS_MASK  AddressMask;
  UINT32            RegValue;
} ICC_RECORD_DWORD;

typedef struct {
  ICC_HEADER        Header;
  UINT16            Reserved;
  UINT8             Params;
  UINT8             Reserved1;
  ICC_RECORD_DWORD  RecordDword;
} ICC_WRITE_ICC_REG_DWORD_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT16      BytesProcessed;
  UINT16      Reserved;
} ICC_WRITE_ICC_REG_DWORD_RESPONSE;

typedef union {
  ICC_WRITE_ICC_REG_DWORD_MESSAGE   message;
  ICC_WRITE_ICC_REG_DWORD_RESPONSE  response;
} ICC_WRITE_ICC_REG_BUFFER;

//
// SPI MEI Messages
//
#define HMRFPO_ENABLE_CMD_ID      0x01
#define HMRFPO_LOCK_CMD_ID        0x02
#define HMRFPO_GET_STATUS_CMD_ID  0x03

typedef enum {
  HMRFPO_ENABLE_SUCCESS                     = 0,
  HMRFPO_ENABLE_LOCKED,
  HMRFPO_NVAR_FAILURE,
  HMRFOP_ATP_POLICY,
  HMRFPO_ENABLE_UNKNOWN_FAILURE
} HMRFPO_ENABLE_STATUS;

///
/// The data for HMRFPO ENABLE message 
///
typedef struct _MKHI_HMRFPO_ENABLE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT64              Nonce; ///< this value should be set to 0
} MKHI_HMRFPO_ENABLE;

/*
  System BIOS sends this message to unlock the Intel(R) ME region temporarily for flash write. 
  The Intel(R) ME firmware will respond to HMRFPO ENABLE message even after the End of Post (EOP)
  if the firmware is in ME manufacturing mode. After firmware is out of ME manufacturing mode, 
  the firmware will not respond to HMRFPO ENABLE message after the End of Post (EOP).  A global
  reset is required right after HMRFPO ENABLE message.
*/
typedef struct _HMRFPO_ENABLE {
  HECI_MESSAGE_HEADER Header;
  MKHI_HMRFPO_ENABLE  Msg;
} HMRFPO_ENABLE;

///
/// The data for HMRFPO ENABLE response message 
///
typedef struct _MKHI_HMRFPO_ENABLE_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              FactoryDefaultBase;   ///< this value should be set to 0
  UINT32              FactoryDefaultLimit;  ///< this value should be set to 0
  ///   
  /// 0x00      HMRFPO_ENABLE_SUCCESS
  /// 0x01      HMRFPO_ENABLE _LOCKED
  /// 0x02      HMRFPO_NVAR_FAILURE
  /// 0x03      HMRFPO_ATP_POLICY
  /// 0x04      HMRFPO_ENABLE _UNKNOWN_FAILURE
  /// 
  UINT8               Status;
  UINT8               Rsvd[3];
} MKHI_HMRFPO_ENABLE_RESPONSE;

///
/// This message is sent by the Intel(R) ME to the host in response to the HMRFPO ENABLE message. 
/// BIOS can proceed only after receiving this response.
///
typedef struct _HMRFPO_ENABLE_RESPONSE {
  HECI_MESSAGE_HEADER         Header;
  MKHI_HMRFPO_ENABLE_RESPONSE Msg;
} HMRFPO_ENABLE_RESPONSE;

typedef enum {
  HMRFPO_LOCK_SUCCESS                       = 0,
  HMRFPO_LOCK_FAILURE
} HMRFPO_LOCK_STATUS;

///
/// MKHI message of HMRFPO LOCK command
///
typedef struct _MKHI_HMRFPO_LOCK {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MKHI_HMRFPO_LOCK;

/*
  System BIOS sends this message to Intel(R) ME to prevent a SPI Flash ME region from unlocking.
  BIOS has to check host firmware status to make sure ME finished initialization before sending 
  out this message. After sending HMRFPO LOCK command, BIOS can make sure that Intel(R) ME is in 
  HMRFPO_LOCKED state by sending HMRFPO GET STATUS command. It is recommended that this message
  is sent prior to any OROM initialization. The firmware will ignore HMRFPO LOCK message if the 
  firmware is in ME manufacturing mode. After firmware is out of ME manufacturing mode, the 
  firmware will not respond to HMRFPO LOCK message after the End of Post (EOP).
*/
typedef struct _HMRFPO_LOCK {
  HECI_MESSAGE_HEADER Header;
  MKHI_HMRFPO_LOCK    Msg;
} HMRFPO_LOCK;

///
/// MKHI message of HMRFPO LOCK response
///
typedef struct _MKHI_HMRFPO_LOCK_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT64              Nonce;  ///< this value should be set to 0
  UINT32              FactoryDefaultBase;  ///< this value should be set to 0
  UINT32              FactoryDefaultLimit; ///< this value should be set to 0
  ///   
  /// 0x00    HMRFPO_LOCK_SUCCESS
  /// 0x01    HMRFPO_LOCK_FAIL
  /// 
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_HMRFPO_LOCK_RESPONSE;

///
/// This message is sent by the ME to the host in response to the HMRFPO LOCK message. BIOS can 
/// proceed only after receiving this response.
///
typedef struct _HMRFPO_LOCK_RESPONSE {
  HECI_MESSAGE_HEADER       Header;
  MKHI_HMRFPO_LOCK_RESPONSE Data;
} HMRFPO_LOCK_RESPONSE;

///
/// Data of the HMRFPO GET STATUS message. 
/// 
typedef struct _MKHI_HMRFPO_GET_STATUS {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MKHI_HMRFPO_GET_STATUS;

///
/// System BIOS sends this message to get the current "Host ME Region Flash Protection Override"
/// status. The firmware will respond to HMRFPO GET STATUS message even after the End of Post (EOP). 
///
typedef struct _HMRFPO_GET_STATUS {
  HECI_MESSAGE_HEADER     Header;
  MKHI_HMRFPO_GET_STATUS  Msg;
} HMRFPO_GET_STATUS;

///
/// Data of response to the HMRFPO GET STATUS message. 
/// 
typedef struct _MKHI_HMRFPO_GET_STATUS_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  /// 
  /// 0x00    HMRFPO_DISABLED
  /// 0x01    HMRFPO_LOCKED
  /// 0x02    HMRFPO_ENABLED
  ///   
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_HMRFPO_GET_STATUS_RESPONSE;

///
/// This message is sent by the ME to the host in response to the HMRFPO GET STATUS message. 
///
typedef struct _HMRFPO_GET_STATUS_RESPONSE {
  HECI_MESSAGE_HEADER             Header;
  MKHI_HMRFPO_GET_STATUS_RESPONSE Data;
} HMRFPO_GET_STATUS_RESPONSE;

#define HMRFPO_LOCKED   1
#define HMRFPO_ENABLED  2

//
// ME State Control
//
#define ME_STATE_STALL_1_SECOND 1000000
#define ME_STATE_MAX_TIMEOUT    20
//
// KVM support
//
#define KVM_MESSAGE_COMMAND 0x08
#define KVM_BYTE_COUNT      0x06
#define KVM_QUERY_REQUES    0x01
#define KVM_QUERY_RESPONSE  0x02
#define KVM_VERSION         0x10

#define KVM_STALL_1_SECOND  1000000   ///< Stall 1 second
#define KVM_MAX_WAIT_TIME   (60 * 8)  ///< 8 Mins

typedef enum {
  QUERY_REQUEST                             = 0,
  CANCEL_REQUEST
} QUERY_TYPE;

typedef struct _AMT_QUERY_KVM_REQUEST {
  UINT8   Command;        ///< KVM
  UINT8   ByteCount;      ///< Number of bytes in AMT/ ASF message beyond this byte
  UINT8   SubCommand;     ///< Query KVM session request
  UINT8   VersionNumber;  ///< Version number
  ///   
  /// 0x00 - Query the KVM session request
  /// 0x01 - Cancel the KVM session request
  /// 
  UINT32  QueryType;
} AMT_QUERY_KVM_REQUEST;

typedef enum {
  KVM_SESSION_ESTABLISHED                   = 1,
  KVM_SESSION_CANCELLED
} RESPONSE_CODE;

typedef struct _AMT_QUERY_KVM_RESPONSE {
  UINT8   Command;       ///< KVM
  UINT8   ByteCount;     ///< Number of bytes in AMT/ ASF message beyond this byte
  UINT8   SubCommand;    ///< Query KVM session request response
  UINT8   VersionNumber; ///< Version number
  /// 
  /// 0x01 - the KVM session established
  /// 0x02 - the KVM session cancelled
  /// 
  UINT32  ResponseCode;
} AMT_QUERY_KVM_RESPONSE;

//
// ME Memory Debug support
//
typedef struct _SET_DEBUG_MEMORY_DATA {
  UINT32  BiosDebugMemoryAddress;
  UINT32  BiosDebugMemorySize;
  UINT32  MeVeDebugMemoryAddress;
  UINT32  MeVeDebugMemorySize;
} SET_DEBUG_MEMORY_DATA;

typedef struct _GEN_SET_DEBUG_MEMORY {
  MKHI_MESSAGE_HEADER   MKHIHeader;
  SET_DEBUG_MEMORY_DATA Data;
} GEN_SET_DEBUG_MEMORY;

typedef struct _GEN_SET_DEBUG_MEMORY_ACK {
  MKHI_MESSAGE_HEADER Header;
} GEN_SET_DEBUG_MEMORY_ACK;

//
// BIOS MDES messaging
//
#define MDES_BIOS_MSG_LOG_REQ_CMD     0x0B
#define MDES_BIOS_MSG_GET_CONFIG_CMD  0x0C

///   
/// [1:0] Severity 
///   0= No error
///   1= Low error
///   2= High error
///   3= Critical error
/// [7:2] Payload Data Type
///   1= EFI status code
///   2= Raw buffer
///   3= ASCII data
/// [15:8] Reserved
///   
typedef struct _MDES_ATTR {
  UINT16  Severity : 2;
  UINT16  PayLoadType : 6;
  UINT16  Reserved : 8;
} MDES_ATTR;

/// 
/// [0] Caller ID Data Present - this bit is set when Caller ID data is present for status code that is being reported.
/// [1] Extended Data Present - this bit is set when Extended data is present for status code that is being reported
/// [15:2] Reserved
/// 
typedef struct _BIOS_ATTR {
  UINT16  CallerIdData       : 1;
  UINT16  ExtendedDataHeader : 1;
  UINT16  Reserved           : 14;
} BIOS_ATTR;

typedef struct {
  UINT16    HeaderSize;
  UINT16    Size;
  EFI_GUID  Type;
} MDES_EXTENDED_DATA_HEADER;

#define SIZE_OF_MDES_EXTENDED_DATA  (2048 - \
                                      ( \
                                        16 + sizeof (MDES_ATTR) + \
                                        sizeof (BIOS_ATTR) + \
                                        sizeof (EFI_GUID) + \
                                        sizeof (MDES_EXTENDED_DATA_HEADER) + \
                                        sizeof (MKHI_MESSAGE_HEADER) \
                                      ) \
                                    )

///
/// The data format of MDES BIOS STATUS CODE message
///		  
typedef struct _CBM_BIOS_MDES_MSG_DATA {
  ///   
  /// [1:0] Severity 
  ///   0= No error
  ///   1= Low error
  ///   2= High error
  ///   3= Critical error
  /// [7:2] Payload Data Type
  ///   1= EFI status code
  ///   2= Raw buffer
  ///   3= ASCII data
  /// [15:8] Reserved
  ///   
  MDES_ATTR                 MdesAttr;
  /// 
  /// [0] Caller ID Data Present - this bit is set when Caller ID data is present for status code that is being reported.
  /// [1] Extended Data Present - this bit is set when Extended data is present for status code that is being reported
  /// [15:2] Reserved
  /// 
  BIOS_ATTR                 BiosAttr;
  /*
    BIOS assigns the individual sequencing number in this field based on the order of BIOS status codes
	being reported
  */
  UINT32                    Serial;
  UINT32                    StatusType; ///< Indicate the type of status code being reported
  UINT32                    StatusCode; ///< Describe the current status of a hardware or software entity.
  UINT32                    Instance;   ///< The enumeration of a hardware or software entity within the system.
  EFI_GUID                  CallerId;   ///< GUID type of data. This optional parameter may be used to identify the caller/ producer.
  MDES_EXTENDED_DATA_HEADER ExtendedDataHeader;  ///< This optional parameter may be used to pass additional data.
  UINT8                     ExtendedData[SIZE_OF_MDES_EXTENDED_DATA];///< This optional parameter may be used to pass additional data.
} CBM_BIOS_MDES_MSG_DATA;

///
/// The MKHI MDES BIOS STATUS CODE message
///
typedef struct _CBM_BIOS_MDES_MSG_REQ {
  MKHI_MESSAGE_HEADER     MKHIHeader;
  CBM_BIOS_MDES_MSG_DATA  Data;
} CBM_BIOS_MDES_MSG_REQ;

/*
  The MDES BIOS STATUS CODE message is based on MKHI interface. This command is used by BIOS to 
  send the BIOS status code to MEI interface. It's the same concept as BIOS to report the BIOS
  status code to port 80h or serial port. The debugger can use the Intel(R) ME Debug Viewer tool
  to collect the BIOS status code thorugh either Intel(R)  LAN or SMBUS.
*/
typedef struct _MKHI_CBM_BIOS_MDES_MSG_REQ {
  HECI_MESSAGE_HEADER   Header;
  CBM_BIOS_MDES_MSG_REQ Msg;
} MKHI_CBM_BIOS_MDES_MSG_REQ;

/*
  The MDES BIOS STATUS CODE GET CONFIGURATION message is based on MKHI interface. This command 
  is used by BIOS to get query about MDES BIOS STATUS CODE feature capabilities. BIOS can 
  consider MDES BIOS STATUS CODE feature enabled if "MDES Enabled" bit set to 1 and "BIOS
  Event Filters" is non-zero.
*/
typedef struct _MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_REQ {
  MKHI_MESSAGE_HEADER MKHIHeader;
} MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_REQ;

///
/// BIOS flag returned in MDES BIOS STATUS CODE GET CONFIGURATION ACK message
///
typedef union _MDES_BIOS_FLAGS {
  VOLATILE UINT32 ul;
  struct {
    UINT32  MdesEnabled            : 1; /// High level indication if MDES is enabled
    UINT32  MdesLogPaused          : 1; /// A caller paused MDES logging as part of the log retrieval process
    UINT32  LanInterfaceEnabled    : 1; /// The user configuration has enabled the LAN debug interface
    UINT32  SmbusInterfaceEnabled  : 1; /// The user configuration has enabled the SMBus debug interface
    UINT32  PramLogEnabled         : 1; /// The user configuration has enabled the PRAM debug log
    UINT32  FlashLogEnabled        : 1; /// The user configuration has enabled the flash error log
    UINT32  MdesBlockingModeEn     : 1; /// Set to 0 when operating in buffered mode, set to 1 when MDES blocks the caller until the event is dispatched.
    UINT32  Reserved7              : 2;
    UINT32  SensitiveMsgEnabled    : 1; /// Set to 1 to indicate Sensitive messages are enabled
    UINT32  DescriptorUnlocked     : 1; /// Set when FW reads all regions are unlocked.  Enables 'none' sensitivity messages.
    UINT32  MdesPolicyEnabled      : 1; /// Set to 1 to indicate MDES is enabled due to BIOS sending the enable message (on current or previous boot)
    UINT32  MdesEnableRcvdFromBios : 1; /// BIOS has sent the MDES policy to 'enabled' via a MKHI message on this current boot
    UINT32  Reserved13             : 19;
  } fl;
} MDES_BIOS_FLAGS;

///
/// Data format for MDES BIOS STATUS CODE GET CONFIGURATION ACK message
///
typedef struct _CBM_BIOS_MDES_MSG_GET_CONFIG_DATA {
  ///
  /// [0] MDES enabled
  /// [1] MDES log paused
  /// [2] LAN interface enabled
  /// [3] SMBUS interface enabled
  /// [4] PRAM log enabled
  /// [5] Flash log enabled
  /// [6] MDES blocking mode enabled
  /// [8-7] Reserved
  /// [9] Sensitive message enabled
  /// [10] Descriptor unlocked 
  /// [11] MDES policy enabled
  /// [12] MDES enabled received from BIOS
  /// [31-13] Reserved 
  ///
  MDES_BIOS_FLAGS Flags;
  UINT32          BiosEventFilters;
} CBM_BIOS_MDES_MSG_GET_CONFIG_DATA;

///
/// Response (ACK) MKHI message for MDES BIOS STATUS CODE GET CONFIGURATION message
///
typedef struct _MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_ACK {
  MKHI_MESSAGE_HEADER               Header;
  CBM_BIOS_MDES_MSG_GET_CONFIG_DATA Data;
} MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_ACK;

#pragma pack()

#endif
