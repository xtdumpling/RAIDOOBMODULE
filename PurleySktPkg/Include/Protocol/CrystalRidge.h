//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2014 - 2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file CrystalRidge.h

    Header file for NVM tech Access protocol

---------------------------------------------------------------------------**/
#ifndef _CRYSTAL_RIDGE_PROTOCOL_H_
#define _CRYSTAL_RIDGE_PROTOCOL_H_

#define MAX_HOST_TRANSACTIONS    64
#define NUM_HOST_TRANSACTIONS    12
//
// NVM Tech Protocol GUID
//
// {626967C7-071B-4d9a-9D0C-F112CF0836E9}
#define EFI_CRYSTAL_RIDGE_GUID \
  { \
    0x626967c7, 0x71b, 0x4d9a, 0x9d, 0xc, 0xf1, 0x12, 0xcf, 0x8, 0x36, 0xe9 \
  }

//#define MAX_HOST_ALERTS          128 // 4 (num of bytes in a payload) * 32 ( num of payloads)
//#define NUM_HOST_ALERTS      	 16

typedef struct _HOST_ALERT_POLICY {
  UINT8   LogLevel:2;                       ///< 0 - do not log, 1 - low priority, 2 - high priority
  UINT8   AitErrViralEn:1;
  UINT8   AitErrInterruptEn:1;
  UINT8   DpaErrViralEn:1;
  UINT8   DpaErrInterruptEn:1;
  UINT8   UncErrViralEn:1;
  UINT8   UncErrInterruptEn:1;
  UINT8   DataPathErrInterruptEn:1;
  UINT8   DataPathErrViralEn:1;
  UINT8   IllegalAccessInterruptEn:1;
  UINT8   IllegalAccessViralEn:1;
  UINT8   Rsvd1:1;
  UINT8   Rsvd2:1;
  UINT8   Rsvd3:1;
  UINT8   Rsvd4:1;
} HOST_ALERT_POLICY;

#define MAX_PRIORITY            2
#define NO_LOG                  0
#define LOW_PRIORITY            1           // BIT0
#define HIGH_PRIORITY           2           // BIT1
#define NEW_ENTRIES_ONLY        ?


#define ENABLE                  1
#define DISABLE                 0

#define FNVERR_MASK                           0x00FFFFFF
#define FNVERR_NGNVM_ERROR_LOG                BIT0
#define FNVERR_THERMAL_SELF_THROTTLING        BIT1
#define FNVERR_ADDRESS_RANGE_SCRUB_COMPLETE   BIT2
#define FNVERR_FW_HANG                        BIT3
#define FNVERR_FATAL                          BIT4

//
// Signaling options on per priority (bucket) level. Strongly recommend to keep them mutually exclusive, aka set only one of them per bucket
//
#define SMI                     BIT0  // in-band SMI
#define ERROR0                  BIT1  // ERROR0# pin signaling
#define LOW_PRIORITY_EVENT      0
#define HIGH_PRIORITY_EVENT     1

#define CLEAR_ON_READ          TRUE
#define DO_NOT_CLEAR_ON_READ   FALSE

//
// FNV transaction types - delete these if these becomes available in FnvAccess.h
//
#define FNV_TT_2LM_READ         0
#define FNV_TT_2LM_WRITE        1
#define FNV_TT_PM_READ          2
#define FNV_TT_PM_WRITE         3
#define FNV_TT_BW_READ          4
#define FNV_TT_BW_WRITE         5
#define FNV_TT_AIT_READ         6
#define FNV_TT_AIT_WRITE        7
#define FNV_TT_WEAR_LEVEL_MOVE  8
#define FNV_TT_PATROL_SCRUB     9
#define FNV_TT_CSR_READ			10
#define FNV_TT_CSR_WRITE		11

#define LOW_LEVEL_LOG       0
#define HIGH_LEVEL_LOG      1
#define RETRIEVE_LOG_DATA   0
#define RETRIEVE_LOG_INFO   1
#define MEDIA_LOG_REQ       0
#define THERMAL_LOG_REQ     1
#define MAX_MEDIA_LOGS      3
#define MAX_THERMAL_LOGS    7

#define ERROR_LOG_HEAD_SIZE 2

#pragma pack(1)

typedef struct _MEDIA_ERR_LOG_ {
  UINT64      SysTimeStamp;        // Unix Epoch time of the log entry
  UINT64      Dpa;                 // Specifies the Dpa address of the error
  UINT64      Pda;                 // Specifies the PDA address of the failure
  UINT8       Range;               // Specifies the length in address space of the error
  UINT8       ErrorType;           // Indicates the kind of error that was logged.
  UINT8       ErrorFlags;          // Indicates the error flags for this entry
  UINT8       TransactionType;     // Indicates what transaction caused the error
  UINT16      SequenceNumber;      // Incremented for each log entry added to the selected log
  UINT16      Rsvd;
} MEDIA_ERR_LOG;

typedef struct _THERMAL_ERR_LOG_ {
  UINT64      SysTimestamp;         // Unix Epoch time of the log entry
  UINT32      Temperature   : 15;   // Host reported temperature (0.0625 C resolution)
  UINT32      Sign          : 1;    // Temperature sign, 0 - positive, 1 - negative
  UINT32      Reported      : 3;    // Temperature being reported
  UINT32      TempType      : 2;    // This denotes which device the temperature is for
  UINT32      Rsvd1         : 11;
  UINT16      SequenceNumber;       // Incremented for each log entry added to the selected log
  UINT16      Rsvd2;
} THERMAL_ERR_LOG;

typedef struct _MEDIA_ERR_LOG_DATA_ {
  UINT16         ReturnCount;     // Total number of entries in the log
  MEDIA_ERR_LOG  MediaLogs[MAX_MEDIA_LOGS];
} MEDIA_ERR_LOG_DATA;

typedef struct _THERMAL_ERR_LOG_DATA_ {
  UINT16          ReturnCount;    // Total number of entries in the log
  THERMAL_ERR_LOG ThermalLogs[MAX_THERMAL_LOGS];
} THERMAL_ERR_LOG_DATA;

typedef struct _MEDIA_ERR_LOG_INFO_ {
  UINT16      MaxLogEntries;          // Total number of log entries FW has abitily to log before overflow
  UINT16      CurrentSequenceNumber;  // Last assigned Sequence Number
  UINT16      OldestSequenceNumber;   // Oldest Sequence Number currently stored
  UINT64      OldestEntryTimestamp;   // Unix Epoch time of the oldest log entry
  UINT64      NewestEntryTimestamp;   // Unix Epoch time of the newest log entry
} MEDIA_ERR_LOG_INFO;

#pragma pack()

/**

  Routine Description: Is this Dimm the NVMDIMM

  @param Socket      - Socket Number
  @param Ch          - DDR Channel ID
  @param Dimm        - DIMM number

  @return BOOLEAN     - TRUE if NVMDIMM otherwise false

**/
typedef
BOOLEAN
(EFIAPI *IS_NVM_DIMM) (
  IN UINT8 Socket,
  IN UINT8 Ch,
  IN UINT8 Dimm
  );

/**

  Routine Description: This function returns the total number of Ngn Dimms
  installed in the system.

  @return UINT32 - Number of Ngn Dimms installed in the system.

**/
typedef
UINT32
(EFIAPI *GET_NUM_NVM_DIMMS) (
  VOID
  );

/**

  Routine Description: This function checks if the given address falls into
  NVMDIMM space or not.

  @param SystemAddress - System Physical Address

  @return BOOLEAN      - True if the address is NVMDIMM address else false

**/
typedef
BOOLEAN
(EFIAPI *IS_NVM_ADDRESS) (
  IN UINT64 Spa
  );

/**

  Routine Description: This function converts given Dpa
  to Spa and also populates the socket channel and dimm information with
  only system phy address as the input.
  CR protocol function.

  @param[in]  Spa        - System Physical address SPA to be translated
  @param[out] *Skt       - socket number of the given SPA
  @param[out] *Ch        - channel number of the given SPA
  @param[out] *Dimm      - dimm corresponding to the given SPA
  @param[out] *Dpa       - Device physical address

  @return EFI_STATUS - status of the conversion effort

**/
typedef
EFI_STATUS
(EFIAPI *SPA_TO_NVM_DPA) (
  IN UINT64  Spa,
  OUT UINT8  *Skt,
  OUT UINT8  *Ch,
  OUT UINT8  *Dimm,
  OUT UINT64 *Dpa
  );

/**

  Routine Description: Update Nvm Tech ACPI tables

  @param *CrystalRidgeTablePtr  - pointer to CR ACPI tables

  @retval EFI_SUCCESS           - operation completed successfully
  @retval EFI_LOAD_ERROR        - error during ACPI table update

**/
typedef
EFI_STATUS
(EFIAPI *UPDATE_NVM_ACPI_TABLES) (
  IN UINT64 *CrystalRidgeTablePtr
  );

/**

  Routine Description: Update Nvm Tech ACPI PCAT table

  @param CrystalRidgeTablePtr * - pointer to CR ACPI tables

  @return EFI_STATUS            - Status of operation

**/
typedef
EFI_STATUS
(EFIAPI *UPDATE_NVM_ACPI_PCAT_TABLE) (
  IN UINT64 *CrystalRidgeTablePtr
  );

// Support for NvmController's config mem access

/**

  Routine Description: this function will read a NVMCTLR register via
  SADs interface.

  @param Socket      - Socket Number
  @param Ch          - DDR Channel ID
  @param Dimm        - DIMM number
  @param Reg         - NVMCTLR register address
  @param *Data       - pointer containing the data after reading the register

  @retval EFI_SUCCESS - operation completed successfully
  @retval EFI_INVALID_PARAMETER - invalid DIMM

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_READ_CFG_MEM) (
  IN UINT8    Socket,
  IN UINT8    Ch,
  IN UINT8    Dimm,
  IN UINT32   Reg,
  OUT UINT32   *Data
  );

/**

  Routine Description: this function will write 32 bits of data
  into a NVMCTLR register using SAD interface.

  @param Socket      - Socket Number
  @param Ch          - DDR Channel ID
  @param Dimm        - DIMM number
  @param Reg         - NVMCTLR register Address
  @param Data        - Data to be written to NVMCTLR register

  @retval EFI_SUCCESS - operation completed successfully
  @retval EFI_INVALID_PARAMETER - invalid DIMM

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_WRITE_CFG_MEM) (
  IN UINT8    Socket,
  IN UINT8    Ch,
  IN UINT8    Dimm,
  IN UINT32   Reg,
  IN UINT32   Data
  );

// Support for Error Logging

/**
  Routine Description: this function allows the user to setup host alerts
  This function sends NVMCTLR Command to acoomplish this.

  @param socket       - Socket Number
  @param ch           - DDR Channel ID
  @param dimm         - DIMM Number
  @param hostAlerts   - New Host Alert Settings

  @return EFI_STATUS  - Status of the command sent

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_SET_HOST_ALERT) (
  IN UINT8     socket,
  IN UINT8     ch,
  IN UINT8     dimm,
  IN UINT16    *hostAlerts
  );

/**
  Routine Description: this function allows the user to retrieve
  host alerts. This function accomplishes this by sending this
  command to the NVMCTLR Controller.

  @param socket       - Socket Number
  @param ch           - DDR Channel ID
  @param dimm         - DIMM Number
  @param hostAlerts   - structure containing host alerts from NVMCTLR

  @return EFI_STATUS  - Status of the command sent
**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_GET_HOST_ALERT) (
  IN UINT8     socket,
  IN UINT8     ch,
  IN UINT8     dimm,
  OUT UINT16   *hostAlerts
  );

/**

  Routine Description: this function will send
  Set Address Range Scrub command to NVMCTLR.
  CR protocol function.

  @param[in] Socket             - Socket Number
  @param[in] Ch                 - DDR Channel ID
  @param[in] Dimm               - DIMM number
  @param[in] DpaStartAddress    - Start address for scrubbing
  @param[in] DpaEndAddress      - End address for scrubbing
  @param[in] Enable             - Enable\Disable scrubbing
  @param[in] UseOsMailbox       - TRUE to use OS mailbox

  @return EFI_STATUS            - Status of the command sent

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_SET_ADDRESS_RANGE_SCRUB) (
  IN UINT8     Socket,
  IN UINT8     Ch,
  IN UINT8     Dimm,
  IN UINT64    DpaStartAddress,
  IN UINT64    DpaEndAddress,
  IN BOOLEAN   Enable,
  IN BOOLEAN   UseOsMailbox
  );

/**

  Routine Description: this function will send
  Get Address Range Scrub command to NVMCTLR.
  CR protocol function.

  @param[in]  Socket                 - Socket Number
  @param[in]  Ch                     - DDR Channel ID
  @param[in]  Dimm                   - DIMM number
  @param[out] *DpaStartAddress       - Start address for scrubbing
  @param[out] *DpaCurrentAddress     - Current address of scrub
  @param[out] *DpaEndAddress         - End address for scrubbing
  @param[out] *Enabled               - TRUE if scrub is enabled
  @param[in]  UseOsMailbox           - TRUE to use OS mailbox

  @return EFI_STATUS            - Status of the command sent

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_GET_ADDRESS_RANGE_SCRUB) (
  IN  UINT8     Socket,
  IN  UINT8     Ch,
  IN  UINT8     Dimm,
  OUT UINT64    *DpaStartAddress,
  OUT UINT64    *DpaEndAddress,
  OUT UINT64    *DpaCurrentAddress,
  OUT BOOLEAN   *Enabled,
  IN  BOOLEAN   UseOsMailbox
  );

/**

  Routine Description: Programs Error Signalling for NVMCTLR Errors
  Note that DDRT_FNV_INTR is used to select error singaling not DDRT_FNV0/1_EVENT.
  CR protocol function.

  @param[in] Socket      - Socket index at system level
  @param[in] Ch          - Channel index at socket level
  @param[in] Dimm        - DIMM slot index within DDR channel
  @param[in] Priority    - 0:Low Priotity, 1:High Priority.
                           There are two buckets of errors. Each bucket can be
                           programmed to a particular signal scheme.
  @param[in] Signal      - BIT0 - SMI, BIT1 - ERROR0#, BIT2 - MSMI#
                           (all mutually exclusive)

  @retval EFI_SUCCESS           - signal programmed successfully
  @retval EFI_INVALID_PARAMETER - given DIMM is not NGN DIMM

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_ERROR_LOG_PROGRAM_SIGNAL) (
  IN UINT8 Socket,
  IN UINT8 Ch,
  IN UINT8 Dimm,
  IN UINT8 Priority,
  IN UINT8 Signal
  );

/**

  Routine Description: Detects the NGN DIMM error and returns 24-bit log.
  CR protocol function.

  @param[in] Socket         - Socket index at system level
  @param[in] Ch             - Channel index at socket level
  @param[in] Dimm           - DIMM slot index within DDR channel
  @param[in] Priority       - 0 for low priority event, 1 for high
  @param[in] ClearOnRead    - when set, clears the error when a NGN DIMM
                              is found and reported
  @param[out] *Log          - 24-bit log outcome

  @retval EFI_SUCCESS             - A NGN DIMM with error was found, pLog updated
  @retval EFI_INVALID_PARAMETER   - Given DIMM is not NGN DIMM or Log is NULL
  @retval EFI_NOT_FOUND           - No error found on NGN DIMM, Log = 0

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_ERROR_LOG_DETECT_ERROR) (
  IN  UINT8   Socket,
  IN  UINT8   Ch,
  IN  UINT8   Dimm,
  IN  UINT8   Priority,
  IN  BOOLEAN ClearOnRead,
  OUT UINT32  *Log
  );

/**

  Routine Description: this function gets latest error logs form NVMCTLR.

  @param Socket            - Socket Number
  @param Ch                - DDR Channel ID
  @param Dimm              - DIMM Number
  @param LogLevel          - Log Level 0 - low priority 1 - high priority
  @param LogType           - Log type
  @param Count             - Max number of log entries requested
  @param *LogBuffer        - Pointer to data buffer

  @return EFI_STATUS       - Status of the Command Sent

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_GET_LATEST_ERROR_LOG) (
  IN  UINT8    Socket,
  IN  UINT8    Ch,
  IN  UINT8    Dimm,
  IN  UINT8    LogLevel,
  IN  UINT8    LogType,
  IN  UINT8    Count,
  OUT VOID     *LogBuffer
  );

// Support for Error Injection

/**

  Routine Description: this function will enable
  injection within the part

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number

  @return EFI_STATUS - success if command sent

**/
typedef
EFI_STATUS
(EFIAPI  *NVMCTLR_ENABLE_ERR_INJECTION) (
  IN UINT8     socket,
  IN UINT8     ch,
  IN UINT8     dimm
  );

/**

  Routine Description: this function populates the payload registers with
  the value of CSR on success allows setting Poison on a particular DPA address.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param DPAAddr     - DPA of the device
  @param Enable      - Enable poison
  @param *EinjCsr    - value of CSR

  @return EFI_STATUS - success if command sent

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_INJECT_POISON_ERROR) (
  IN  UINT8     socket,
  IN  UINT8     ch,
  IN  UINT8     dimm,
  IN  UINT64    DPAAddr,
  IN  UINT8     Enable,
  OUT UINT32    *EinjCsr
  );

/**

  Routine Description: this routine sets the config data policy

  @param[in]  Socket  The socket
  @param[in]  Ch      The channel
  @param[in]  Dimm    The dimm number
  @param[in]  Data    The data to write

  @return Status

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_SET_CFG_DATA_POLICY) (
  IN UINT8     Socket,
  IN UINT8     Ch,
  IN UINT8     Dimm,
  IN UINT32    Data
  );

/**

  Routine Description: this routine gets the config data policy

  @param[in]    Socket  The socket
  @param[in]    Ch      The channel
  @param[in]    Dimm    The dimm number
  @param[out]   *Data   The data that is read

  @return Status

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_GET_CFG_DATA_POLICY) (
  IN    UINT8     Socket,
  IN    UINT8     Ch,
  IN    UINT8     Dimm,
  OUT   UINT32    *Data
  );

/**

  Routine Description: This function Sends Set Platform Config Data Area
  size to the NVMCTLR Controller. This function also copies the Input Buffer
  to the Dimm's Large Payload Input area.

  @param[in] Socket           - Socket index at system level
  @param[in] Ch               - Channel index at socket level
  @param[in] Dimm             - DIMM slot index within DDR channel
  @param[in] InputBuffer      - Pointer to the Input Buffer.
  @param[in] InputSize        - Input length of the Config Data buffer
  @param[in] Offset           - PCD offset to which to write the Config data
  @param[in] PartitionID      - Partition ID to use for this command
  @param[in] UseLargePayload  - Boolean to tell whether to use
                                LargePayload or SmallPayload registers
                                for this PCD command.
  @param[in] UseOSMailbox     - If TRUE use OS mailbox. BIOS mailbox otherwise.

  @return Status - Status of the operation

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_SET_PCD) (
  IN    UINT8     Socket,
  IN    UINT8     Ch,
  IN    UINT8     Dimm,
  IN    UINT8     *InputBuffer,
  IN    UINT32    InputSize,
  IN    UINT32    Offset,
  IN    UINT8     PartitionId,
  IN    BOOLEAN   UseLargePayload,
  IN    BOOLEAN   UseOsMailbox
  );

/**

  Routine Description: This function returns Platform Config Data by sending
  this command to the NVMCTLR Controller.

  @param[in]  Socket          - Socket index at system level
  @param[in]  Ch              - Channel index at socket level
  @param[in]  Dimm            - DIMM slot index within DDR channel
  @param[out] OutputBuffer    - pointer to the buffer to which PCD needs to be
                                copied after successful command completion
  @param[in,out] OutputSize   - As input: size of the buffer, output:
                                size of the PCD read.
  @param[in]  Offset          - Offset from which the PCD needs to be read
  @param[in]  PartitionID     - ID of the partition to be read
  @param[in]  UseLargePayload - Boolean to tell whether to use
                                LargePayload or SmallPayload registers
                                for this PCD command.
  @param[in]  UseOSMailbox    - If TRUE use OS mailbox. BIOS mailbox otherwise.

  @return Status - Status of the operation

**/
typedef
EFI_STATUS
(EFIAPI *NVMCTLR_GET_PCD) (
  IN      UINT8     Socket,
  IN      UINT8     Ch,
  IN      UINT8     Dimm,
  IN OUT  UINT8     *OutputBuffer,
  IN      UINT32    *OutputSize,
  IN      UINT32    Offset,
  IN      UINT8     PartitionId,
  IN      BOOLEAN   UseLargePayload,
  IN      BOOLEAN   UseOsMailbox
  );


typedef struct _EFI_CRYSTAL_RIDGE_PROTOCOL {
  IS_NVM_DIMM                      IsNvmDimm;
  GET_NUM_NVM_DIMMS                GetNumNvmDimms;
  IS_NVM_ADDRESS                   IsNvmAddress;
  SPA_TO_NVM_DPA                   SpaToNvmDpa;
  UPDATE_NVM_ACPI_TABLES           UpdateNvmAcpiTables;
  UPDATE_NVM_ACPI_PCAT_TABLE       UpdateNvmAcpiPcatTable;
  NVMCTLR_READ_CFG_MEM             NvmCtlrReadCfgMem;
  NVMCTLR_WRITE_CFG_MEM            NvmCtlrWriteCfgMem;
  NVMCTLR_SET_HOST_ALERT           NvmCtlrSetHostAlert;
  NVMCTLR_GET_HOST_ALERT           NvmCtlrGetHostAlert;
  NVMCTLR_SET_ADDRESS_RANGE_SCRUB  NvmCtlrSetAddressRangeScrub;
  NVMCTLR_GET_ADDRESS_RANGE_SCRUB  NvmCtlrGetAddressRangeScrub;
  NVMCTLR_ERROR_LOG_PROGRAM_SIGNAL NvmCtlrErrorLogProgramSignal;
  NVMCTLR_ERROR_LOG_DETECT_ERROR   NvmCtlrErrorLogDetectError;
  NVMCTLR_GET_LATEST_ERROR_LOG     NvmCtlrGetLatestErrorLog;
  NVMCTLR_ENABLE_ERR_INJECTION     NvmCtlrEnableErrInjection;
  NVMCTLR_INJECT_POISON_ERROR      NvmCtlrInjectPoisonError;
  NVMCTLR_SET_CFG_DATA_POLICY      NvmCtlrSetCfgDataPolicy;
  NVMCTLR_GET_CFG_DATA_POLICY      NvmCtlrGetCfgDataPolicy;
  NVMCTLR_SET_PCD                  NvmCtlrSetPcd;
  NVMCTLR_GET_PCD                  NvmCtlrGetPcd;
} EFI_CRYSTAL_RIDGE_PROTOCOL;

extern EFI_GUID         gEfiCrystalRidgeGuid;
extern EFI_GUID         gEfiCrystalRidgeSmmGuid;

#endif

