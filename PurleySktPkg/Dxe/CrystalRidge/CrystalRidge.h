
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file CrystalRidge.h

--*/

#ifndef _CRYSTALRIDGE_H_
#define _CRYSTALRIDGE_H_

#include <PiDxe.h>
#include <Protocol/CrystalRidge.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/MpService.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
// HSD:4930336_BEGIN
#include <Library/TimerLib.h>
// HSD:4930336_END
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/LocalApicLib.h>
#include <Library/CpuConfigLib.h>
#include <Guid/MemoryMapData.h>
#include <Guid/HobList.h>
#include <Cpu/CpuRegs.h>
#include <Protocol/Smbios.h>
#include <Protocol/NgnAcpiSmmProtocol.h>
#include "RcRegs.h"
#include "FnvAccess.h"
#include "PlatformCfgData.h"

extern EFI_GUID gEfiVolatileMemModeVariableGuid;
extern EFI_GUID gEfiPrevBootNGNDimmCfgVariableGuid;
extern EFI_GUID gNfitBindingProtocolGuid;

#define ARRAY_SIZE(x)   (sizeof(x)/sizeof((x)[0]))

#define FNV_COMMANDS_TESTING      0
// Status codes of MB Commands
#define MB_CMD_SUCCESS            0
#define MB_CMD_INVALID_PARAM      0x01
#define MB_CMD_DATA_XFER_ERR      0x02
#define MB_CMD_INTERNAL_ERR       0x03
#define MB_CMD_UNSUPPORTED_CMD    0x04
#define MB_CMD_DEVICE_BUSY        0x05
#define MB_CMD_INCORRECT_NONCE    0x06
#define MB_CMD_SECURITY_CHK_FAIL  0x07
#define MB_CMD_INVALID_SEC_STATE  0x08
#define MB_CMD_SYS_TIME_NOT_SET   0x09
#define MB_CMD_DATA_NOT_SET       0x0A
#define MB_CMD_ABORTED            0x0B
#define MB_CMD_NO_NEW_FW          0x0C
#define MB_CMD_REVISION_FAIL      0x0D
#define MB_CMD_INJECT_NOT_ENABLED 0x0E
#define MB_CMD_CONFIG_LOCKED      0x0F
#define MB_CMD_INVALID_ARGUMENT   0x10
#define MB_CMD_INCOMPATIBLE_DIMM  0x11
#define MB_CMD_TIMEOUT            0x12

#define CR_IN_MB_SIZE             1 << 20    // Size of the OS mailbox large input payload
#define CR_OUT_MB_SIZE            1 << 20    // Size of the OS mailbox large output payload
#define CR_IN_PAYLOAD_SIZE        128        // Total size of the input payload registers
#define CR_OUT_PAYLOAD_SIZE       128        // Total size of the output payload registers

// Offset from the start of the CTRL region to the start of the BIOS mailbox
#define CR_BIOS_MB_OFFSET         0x101000
// Large Payload Region size
#define CR_BIOS_LARGE_PAYLOAD_SIZE 0x100000
// Offset to BIOS MB Large InPayload region
#define CR_BIOS_MB_LARGE_INPAYLOAD 0x400000
// Offset to BIOS MB Large OutPayload region
#define CR_BIOS_MB_LARGE_OUTPAYLOAD 0x500000
// Boot Status Reg Offset
#define CR_MB_BOOT_STATUS_OFFSET   0x20000
// Total Size of the Mailbox Region
#define MB_REGION_SIZE             0x8000000
// OS Mailbox Region Offsets below:
#define CR_OS_MB_OFFSET            0x100000
// Offset to OS Mailbox Large InPayload Region
#define CR_OS_MB_LARGE_INPAYLOAD   0x200000
// Offset to OS Mailbox Large OutPayload Region
#define CR_OS_MB_LARGE_OUTPAYLOAD  0x300000

#define CR_IN_MB_SIZE             1 << 20    // Size of the OS mailbox large input payload
#define CR_OUT_MB_SIZE            1 << 20    // Size of the OS mailbox large output payload
#define CR_REG_SIZE               8          // Size of a NVMDIMM Mailbox Register Bytes
#define CR_NONCE_SIZE             4          // Size of a NVMDIMM Nonce Register Bytes
#define CR_IN_PAYLOAD_SIZE        128        // Total size of the input payload registers
#define CR_IN_SINGLE_PAYLOAD_SIZE 4          // Size of the input payload register
#define CR_IN_PAYLOAD_NUM         32         // Total number of the input payload registers
#define CR_OUT_PAYLOAD_SIZE       128        // Total size of the output payload registers
#define CR_OUT_SINGLE_PAYLOAD_SIZE 4         // Size of the output payload register
#define CR_OUT_PAYLOAD_NUM        32         // Total number of the output payload registers
#define CR_REG_ALIGNMENT_SIZE     64         // NVMDIMM Registers are to be 64-byte aligned
#define ROOT_DEVICE_ACPI_HANDLE   0xFFFFFFFF
#define MAX_ROOT_DEVICE_FCNS      4
#define MAX_PMEM_RGN_PER_DIMM     2
#define MAX_PMEM_INTERLEAVE_SET   MAX_SOCKET * MAX_MC_CH * MAX_IMC * MAX_PMEM_RGN_PER_DIMM

//
// Defines
#define SIMICS_OS_MB_OFFSET       0x800      // Offset from the start of the CTRL region to the start of the OS mailbox
#define SIMICS_MB_IN_PAYLOAD0_OFFSET  0x18
#define SIMICS_MB_STATUS_OFFSET   0x98
#define SIMICS_MB_OUT_PAYLOAD0_OFFSET  0xA0

//
// Offsets for NVMDIMM control region
//
#define CR_MB_COMMAND_OFFSET      0
#define CR_MB_NONCE0_OFFSET       0x40
#define CR_MB_NONCE1_OFFSET       0x80
#define CR_MB_IN_PAYLOAD0_OFFSET  0xC0
#define CR_MB_STATUS_OFFSET       0x4C0
#define CR_MB_OUT_PAYLOAD0_OFFSET 0x500
//
// Offsets for NVMDIMM control region (Simics environment)
//
#define CR_SIMICS_MB_COMMAND_OFFSET 0
#define CR_SIMICS_MB_NONCE0_OFFSET (CR_SIMICS_MB_COMMAND_OFFSET + CR_REG_SIZE)
#define CR_SIMICS_MB_NONCE1_OFFSET (CR_SIMICS_MB_NONCE0_OFFSET + CR_NONCE_SIZE)
#define CR_SIMICS_MB_IN_PAYLOAD0_OFFSET SIMICS_MB_IN_PAYLOAD0_OFFSET
#define CR_SIMICS_MB_STATUS_OFFSET SIMICS_MB_STATUS_OFFSET
#define CR_SIMICS_MB_OUT_PAYLOAD0_OFFSET SIMICS_MB_OUT_PAYLOAD0_OFFSET

#define FLUSH_HINT_ADDRESS_OFFSET      (512 * 1024)
#define CORE_FLUSH_HINT_ADDRESS_OFFSET (0 * 1024)
#define MC_FLUSH_HINT_ADDRESS_OFFSET   (256 * 1024)
#define CTRL_RGN_GRANULARITY           256
#define SMART_HEALTH_OUTPUT_SIZE       128
#define SMART_THRESHOLD_OUTPUT_SIZE    8
#define ARS_ALIGNMENT_SIZE             256

#define USE_SMALL_PAYLOAD              0
#define USE_LARGE_PAYLOAD              1
#define ERROR_UNCORRECTABLE            0
#define MAX_ERRORS_PER_DIMM            14

#define TOTAL_PERCENT_COMPLETE         100
#define INPUT_BUF_SIZE                 0x1000
#define OUTPUT_BUF_SIZE                0x1000
#define ACPI_SMM_BUF_SIZE              0x1000

#define PREV_BOOT_NGN_DIMM_CONFIG_VARIABLE_NAME L"PrevBootNGNDimmCfg"

#define LARGE_PAYLOAD_INPUT     1
#define LARGE_PAYLOAD_OUTPUT    0

#define BIOS_PARTITION          0x0
#define OS_PARTITION            0x01
#define OS_PARTITION_OTHER      0x02
#define FNV_SIZE                0x01
#define FNV_DATA                0x00
#define LARGE_PAYLOAD           0
#define SMALL_PAYLOAD           1
#define LARGE_PAYLOAD_MGMT      1
#define USE_BIOS_MAILBOX        FALSE
#define USE_OS_MAILBOX          TRUE

// DSM root functions
#define DSM_ROOT_QUERY_ARS_CAPABILITES      1
#define DSM_ROOT_START_ARS                  2
#define DSM_ROOT_QUERY_ARS_STATUS           3
#define DSM_ROOT_CLEAR_UNCORRECTABLE_ERROR  4

// DSM root fields definitions
#define VOLATILE_SCRUB_SUPPORTED        BIT0
#define PMEM_SCRUB_SUPPORTED            BIT1
#define OPTIMIZE_FOR_PERFORMANCE        BIT0
#define DSM_ROOT_FLAG_OVERFLOW          BIT0

// DSM additional definitions
#define DSM_CLEAR_UNCORRECTABLE_UNIT_SIZE_NOT_SUPPORTED 0
#define ARS_DEFAULT_TIME               0xFFFFFFFF

// DSM functions
#define DSM_GET_SMART_INFO      1
#define DSM_GET_SMART_THRESHOLD 2
#define DSM_GET_BLOCK_FLAGS     3
#define DSM_GET_LABEL_SIZE      4
#define DSM_GET_LABEL_DATA      5
#define DSM_SET_LABEL_DATA      6
#define DSM_GET_VENDOR_LOG_SIZE 7
#define DSM_GET_VENDOR_LOG      8
#define DSM_VENDOR_COMMAND      9

// DSM status codes
#define DSM_STS_SUCCESS             0x00
#define DSM_STS_NOT_SUPPORTED       0x01
#define DSM_STS_NO_MEM_DEVICE       0x02
#define DSM_STS_INVALID_PARAMS      0x03
#define DSM_STS_HW_ERROR            0x04
#define DSM_STS_RETRY_SUGGESTED     0x05
#define DSM_STS_UNKNOWN_FAIL        0x06
#define DSM_STS_VEN_SPECIFIC_ERROR  0x07

// DSM status extended codes
#define DSM_STS_EXT_INVALID_MB_OFFSET           0x01
#define DSM_STS_EXT_INVALID_BYTES_TO_XFER       0x02
#define DSM_STS_EXT_ERROR_BUFFER_OVERRUN        0x03

// DSM status codes for root device
#define DSM_STS_ROOT_SUCCESS           0x00
#define DSM_STS_ROOT_NOT_SUPPORTED     0x01
#define DSM_STS_ROOT_INVALID_PARAMS    0x02
#define DSM_STS_ROOT_HW_ERROR          0x03
#define DSM_STS_ROOT_RETRY_SUGGESTED   0x04
#define DSM_STS_ROOT_UNKNOWN_FAIL      0x05
#define DSM_STS_ROOT_FUNCTION_SPECIFIC 0x06

// DSM status extended codes for root device
#define DSM_STS_EXT_ROOT_QUERY_ARS_COMPLETED    0x00
#define DSM_STS_EXT_ROOT_ARS_IN_PROGRESS        0x01
#define DSM_STS_EXT_ROOT_ARS_NOT_PERFORMED      0x02
#define DSM_STS_EXT_ROOT_ARS_STOPPED_PERMANETLY 0x03

#define CREATE_DSM_EXT_STATUS(sts, extSts) (((sts) & 0xFFFF) | (((extSts) & 0xFFFF)<<16))

#define ADMIN_FUNCTIONS     0x02
#define BIOS_EMULATED       BIT0
#define NON_EXISTING_MEM_DEV 0x02
#define ONE_IMC_WAY         1
#define TWO_IMC_WAY         2
#define FOUR_IMC_WAY        4
#define EIGHT_IMC_WAY       8
#define SIXTEEN_IMC_WAY     16

#define BIOS_EMULATED_COMMAND 0xFD
// BIOS Emulated Command SubOpcodes
#define GET_LP_MB_INFO      0
#define WRITE_LP_INPUT_MB   1
#define READ_LP_OUTPUT_MB   2
#define GET_BOOT_STATUS     3

// Options for prevBootNGNDimmCfg
#define ADD_VARIABLE        0
#define RMV_VARIABLE        1

// CR controller type (same present in MemAddrMap.h)
#define FNV 0x979
#define EKV 0x97A
#define BWV 0x97B

typedef enum {
  ACPI_DSM_ROOT,
  ACPI_DSM_DEVICE
} ACPI_DSM_TYPE;

typedef enum {
  MB_REG_COMMAND   = 0,
  MB_REG_NONCE0,
  MB_REG_NONCE1,
  MB_REG_STATUS,
} MB_REGS;

#pragma pack(1)

/*
 * Struct format extracted from XML file FNV\0.0.0.FNV.xml.
 * This register contains the Host Mailbox Command Register Bits definitions. This is a
 * 64-bit Register.
 */
typedef union {
  struct {
    UINT32 reserved : 32; // Bits[31:0] are reserved bits
    UINT32 opcode : 16;
    /* opcode - Bits[47:32], RW, default = 16'h0
       FW MB OPCODE
     */
    UINT32 doorbell : 1;
    /* doorbell - Bits[48:48], RW, default = 1'b0
       FW MB Doorbell
     */
    UINT32 spare : 14;
    /* spare - Bits[62:49], RW, default = 15'b0
       Spare for future use
     */
    UINT32 sequence_bit : 1;
    /* Sequence Bit*/
  } Bits;
  UINT64 Data;
} MB_SMM_CMD_FNV_SPA_MAPPED_0_STRUCT;

/*
 * Struct format extracted from XML file NVMCTLR\0.0.0.NVMCTLR.xml.
 * This register contains the FW Mailbox Status converted to 64-bit to be compatible with
 * 64-bit SPA addressing.
 */
typedef union {
  struct {
    UINT32 comp : 1;
    /* comp - Bits[0:0], RWV, default = 1'h1
       FW Mailbox Completion:[br]
                       0 = Not Complete[br]
                       1 = Complete[br]
                       HW will clear this bit when doorbell bit is set.
     */
    UINT32 nonce_fail : 1;
    /* fail - Bits[1:1], RW, default = 1'h0
       FW Mailbox Nonce Fail:[br]
                       0 = Success[br]
                       1 = Failure
     */
    UINT32 bos : 1;
    /*
    * Background Operation Status
    */
    UINT32 spare0 : 5;
    /* spare0 - Bits[7:2], RW, default = 6'h0
       Spare status bits reserved for future use.
     */
    UINT32 stat : 8;
    /* stat - Bits[15:8], RW, default = 8'h0
       FW Mailbox Status Code
     */
    UINT32 rsvd_16 : 16;
    UINT32 rsvd_32 : 32;
    /*
    Adding 32-bit of reserved field to be compatible with 64-bit
    SPA Addressing
    */
  } Bits;
  UINT64 Data;
} MB_SMM_STATUS_FNV_SPA_MAPPED_0_STRUCT;

//
// This struct is used by processing code to prevent redundant SADs
// being accounted for the computation of AepDimm Control Regions for CrystalRidge thru SAD interface.
typedef struct {
  UINT8        NumOfSADCntrlReg;
  UINT32       Limits[MAX_SAD_RULES];
} SAD_CONTROL_REGION;

//
// This struct is used by processing code to prevent redundant SADs
// being accounted for the computation of AepDimm PMEM Regions for CrystalRidge thru SAD interface.
typedef struct {
  UINT8        NumOfSADPMemReg;
  UINT32       Limits[MAX_SAD_RULES];
} SAD_PMEM_REGION;

typedef struct _SMBIOS_HANDLE_INFO_
{
  UINT8        Socket;              // Socket ID (0 based)
  UINT8        Channel;             // Channel ID in socket (0 based)
  UINT8        Dimm;                // Dimm ID in channel (0 based)
  UINT16       SMBiosType17Handle;  // Handle in Type 17 SMBIOS table
} SMBIOS_HANDLE_INFO;

typedef struct {
  UINT64 pCommand;        // SPA of the command register for the NVMDIMM dimm
  UINT64 pNonce0;         // SPA of the nonce0 register for the NVMDIMM dimm
  UINT64 pNonce1;         // SPA of the nonce1 register for the AEP dimm
  UINT64 pInPayload[NUM_PAYLOAD_REG];  // SPA of the 32 input payload registers (write only)
  UINT64 pStatus;         // SPA of the status register for the AEP dimm
  UINT64 pOutPayload[NUM_PAYLOAD_REG]; // SPA of the 32 input payload registers (read only)
  UINT64 LargeInPayload;  //SPA of theLarge InPayload region for the AEP dimm
  UINT64 LargeOutPayload; // SPA of the Large OutPayload region for the AEP dimm
} CR_MAILBOX;

typedef struct _PEMEM_INFO {
  UINT64      SADPMemBase;         // SAD Rule PMEM Base address
  UINT64      DPABase;             // DPA start address for this PMEM
  UINT64      ARSDpaStart;         // Dpa Start address for ARS
  UINT64      ARSDpaEnd;           // Dpa End address for ARS
  UINT8       IntDimmNum;          // Dimm Number in the interleave set (not to be confused with dimm # of channel)
  UINT8       Socket;              // Socket number
  UINT8       Channel;             // Channel number
  UINT8       Dimm;                // Dimm number on channel
  UINT8       iMCWays;             // Number of iMC interleave ways that this dimm is participating for PMEM Rgn
  UINT8       ChWays;              // Number of Channel interleave ways that this dimm is participating for PMEM Rgn
  UINT8       SecondPMemRgn;       // To tell if this a second PMem Region of a dimm
  EFI_STATUS  ArsStatus;           // Status of the Ars Command Sent
} PMEM_INFO;

typedef struct _DIMM {
  UINT16      ImcId;               // iMC Num (either iMC0 or iMC1, i.e, o or 1)
  UINT16      SocketId;            // Socket Num
  UINT8       Dimm;                // Dimm #
  UINT8       IntDimmNum;          // Dimm Number in the interleave set (not to be confused with dimm # of channel)
  UINT8       Ch;                  // Channel # where this Dimm resides
  UINT8       iMCWays;             // Number of iMC interleave ways that this dimm is participating for Cntrl Rgn
  UINT8       ChWays;              // Number of Channel interleave ways that this dimm is participating for Cntrl Rgn
  UINT8       FMchannelInterBitmap;// FMchannelInterBitmap of the SAD Rule
  UINT64      SADSpaBase;          // SAD Rule Spa Base Addr for Control Region.
  UINT64      CntrlBase;           // Control Region Base Address
  UINT16      DeviceID;            // Device ID of this dimm
  UINT16      VendorID;            // Vendor ID of the dimm
  UINT16      RevisionID;          // Revision ID of the dimm
  UINT16      subSysVendorID;      // SubSystem vendor ID
  UINT16      subSysDeviceID;      // SubSystem Device ID
  UINT16      subSysRevisionID;    // SubSystem RID
  UINT8       ManufLocation;       // Manufacturing location for the NVDIMM
  UINT16      ManufDate;           // Date the NVDIMM was manufactured
  UINT16      FormatInterfaceCode; // FormatInterfaceCode
  UINT32      SerialNum;           // Serial Number of the Dimm
  CR_MAILBOX  MailboxSPA;          // BIOS Control Region Mapped SPA regiter addresses
  CR_MAILBOX  MailboxDPA;          // BIOS Control Region DPA register addresses
  CR_MAILBOX  OSMailboxSPA;        // OS Mailbox region for _DSM Command Interface
  CR_MAILBOX  OSMailboxDPA;        // OS Mailbox region for _DSM Command Interface
  UINT64      Nonce;               // Holds security Nonce generated
  UINT8       NonceSet;            // Flag to tell if the Nonce has been set for the Mailbox of this dimm.
} AEP_DIMM, *PAEP_DIMM;

typedef struct _CR_INFO
{
  UINT32       SktGran;            // Socket Granularity for the Control Region
  UINT32       ChGran;             // Channel Granularity for the Control Region
  AEP_DIMM     AepDimms[MAX_SYS_DIMM];
  PMEM_INFO    PMEMInfo[MAX_SYS_DIMM * MAX_PMEM_RGN_PER_DIMM]; // Information about 1st Persistent Memory Region
  UINT8        NumPMEMRgns;        // Number of PMEM Regions total
  UINT8        NumAepDimms;        // Number of Aep Dimms Installed/Found
} CR_INFO;

//
// Structure definitions for DSM
//

typedef struct _ARS_OP_STATUS {
  BOOLEAN   InProgress;         // is operation in progress
  BOOLEAN   PrematurelyEnded;   // is operation ended prematurely
  UINT8     StatusCode;         // FW status code of operation
  UINT16    PercentComplete;    // percent complete
  UINT32    EstimatedTime;      // estimated time in seconds to finish the operation
  UINT8     ErrorsFound;        // how many erroneous addresses has been found
  UINT64    DpaErrorAddress[MAX_ERRORS_PER_DIMM]; // list of erroneous addresses found during the scrub
} ARS_OP_STATUS;

typedef struct _ARS_ERROR_RECORD {
  UINT32    NfitHandle;       // Nfit handle that defines the dimm that is part of the error record
  UINT32    Reserved;
  UINT64    SpaOfErrLoc;      // start SPA of the error location
  UINT64    Length;           // length of the error location region
} ARS_ERROR_RECORD;

typedef struct _ARS_DSM_ROOT_CAPABILITY{
  UINT32      MaxArsQueryBuffersSize;   // Maximum size of buffer returned by query ARS in bytes
  UINT32      ClearUncorrectedUnitSize; // Clearing unit size in bytes
  UINT32      Reserved;
} ARS_DSM_ROOT_CAPABILITY;

typedef struct _ARS_DSM_ROOT_QUERY_HEADER {
  UINT32      OutputLength;        // Output size of the ARS Output Header
  UINT64      StartSpa;            // Start System Physical Address
  UINT64      Length;              // ARS Length
  UINT64      RestartSpa;          // Restart start address of SPA address of ARS
  UINT64      RestartLength;       // Length of region that need to scrubed after restart
  UINT16      Type;                // if Bit0 is set, it is Volatile & if Bit1 is set it is Non-Volatile, both if both the bits are set
  UINT16      Flags;               // Bit[0] – If set to 1, indicates an overflow condition has occurred
  UINT32      NumErrorRecords;     // Number of error records defined in this output
} ARS_DSM_ROOT_QUERY_HEADER;

typedef struct _ARS_DSM_CLEAR_UNCORR_ERROR {
  UINT32      Reserved;
  UINT64      ErrorRangeLength;    // The range of errors actually cleared by the platform
} ARS_DSM_CLEAR_UNCORR_ERROR;

//
// Payload definitions for NvmCtlr access functions. FIS 1.3 compatible
//

typedef struct _NVM_SET_ARS_PAYLOAD_
{
  UINT8  Enable;              // 1 to start the ARS, 0 to stop (if running)
  UINT8  Reserved[3];
  UINT64 DpaStartAddress;     // start DPA for the ARS
  UINT64 DpaEndAddress;       // end DPA for the ARS
} NVM_SET_ARS_PAYLOAD;

typedef struct _NVM_GET_ARS_PAYLOAD_
{
  UINT8  Enabled;             // 1 if ARS running, 0 otherwise
  UINT8  Reserved[3];
  UINT64 DpaStartAddress;     // start DPA for the ARS
  UINT64 DpaEndAddress;       // end DPA for the ARS
  UINT64 DpaCurrentAddress;   // last DPA, that was scrubbed
} NVM_GET_ARS_PAYLOAD;

typedef struct _NVM_GET_LONG_OPERATION_STATUS_ARS_PAYLOAD_
{
  UINT16  Command;                              // 7:0 - Opcode, 15:8 - Subopcode
  UINT16  PercentComplete;                      // Percent complete, BCD format = XXX
  UINT32  Etc;                                  // Estimated Time to Completion in seconds
  UINT8   StatusCode;                           // NvmCtlr status code of the last long operation
  UINT8   ErrorsFound;                          // number of errors found so far
  UINT8   MaxDpaErrorAddressReached : 1;        // if 1 - 14 error addressess found limit
                                                // reached and operation stopped prematurely
  UINT8   Rsvd1                     : 7;
  UINT64  DpaErrorAddress[MAX_ERRORS_PER_DIMM]; // list of erroneous addresses found durning the ARS
} NVM_GET_LONG_OPERATION_STATUS_ARS_PAYLOAD;

typedef struct _NVM_GET_ERROR_LOG_PARAMS_PAYLOAD_
{
  UINT8   LogLevel    : 1;    // 0 for LOW and 1 for HIGH level log
  UINT8   LogType     : 1;    // 0 for Media log, 1 for thermal
  UINT8   LogInfo     : 1;    // 0 for retrieving data and 1 for retrieving info
  UINT8   LogPayload  : 1;    // 0 for small payload, 1 for large payload
  UINT8   Rsvd        : 4;
  UINT16  SequenceNumber;     // sequence number from which to get logs, 0 for oldest available
  UINT16  RequestCount;       // how many logs to retrieve
} NVM_GET_ERROR_LOG_PARAMS_PAYLOAD;

#pragma pack()

//
// Function declarations being exposed by this module
//

/**

  Routine Description: Does this system have at least one NVMDIMM

  @return TRUE if NVMDIMM present

**/
BOOLEAN
IsSystemWithAepDimm (
  VOID
  );

/**

  Routine Description: Is this Dimm the NVMDIMM.
    CR interface protocol function.

  @param[in] Socket      - Socket Number
  @param[in] Ch          - DDR Channel ID
  @param[in] Dimm        - DIMM number

  @return TRUE if NVMDIMM otherwise FALSE

**/
BOOLEAN
IsNvmDimm (
  IN UINT8 Socket,
  IN UINT8 Ch,
  IN UINT8 Dimm
  );

/**

  Routine Description: Is the given NVMDIMM Enabled

  @param[in] Socket   - Socket ID
  @param[in] Ch       - Channel ID on given Socket
  @param[in] Dimm     - Dimm ID on given channel

  @return TRUE if NVMDIMM is Enabled, FALSE otherwise

**/
BOOLEAN
IsNvmDimmEnabled (
  IN UINT8 Socket,
  IN UINT8 Ch,
  IN UINT8 Dimm
  );

/**

  Routine Description: This function checks if the given start address
  of a SAD rule belongs to a PMEM Region.

  @param[in] SpaBaseAddr - Spa Start Address of the SAD Rule

  @return TRUE if yes, else FALSE

**/
BOOLEAN
IsPmemRgn (
  IN UINT64 SpaBaseAddr
  );

/**

  Routine Description: This helper function returns index of the PMEMInfo array
  based on the SAD base address, Socket and Channel info passed in.

  @param[in] SpaBaseAddr - Spa Start Address of the SAD Rule
  @param[in] Socket       - Socket Number
  @param[in] Ch           - Channel

  @return Index of the array, -1 if no match found

**/
INT32
GetPmemIndexBasedOnSocCh (
  IN UINT64  SpaBaseAddr,
  IN UINT8   Socket,
  IN UINT8   Ch
  );

/**

  Routine Description: Gets the PMEM DPA base address for given NVDIMM using the
  SPA base address for the relevant SAD of type PMEM.

  @param[in] SadSpaBase  - Base Address of the SAD Rule found
  @param[in] Socket      - Socket Number
  @param[in] Ch          - Channel number
  @param[in] Dimm        - Dimm number

  @return DPA base address

**/
UINT64
GetPmemDpaBase (
  IN UINT64  SadSpaBase,
  IN UINT8   Socket,
  IN UINT8   Ch,
  IN UINT8   Dimm
  );

/**

  Routine Description: This helper function returns interleave ways for the
  given PMEM region.

  @param[in] SpaBaseAddr  - Spa Start Address of the SAD Rule

  @return Interleave ways for PMEM region or 0 if error

**/
UINT8
GetInterleaveWaysForPmemRgn (
  IN UINT64 SadBase
  );

/**

  Routine Description: This helper function returns the total number of Dimms
  in the Interleave set of the SAD Rule for a PMEM Region.

  @param[in] SpaBaseAddr - Spa Start Address of the SAD Rule

  @return Total number of dimms in the Interleave set.

**/
UINT8
GetNumDimmsForPmemRgn (
  IN UINT64 SpaBaseAddr
  );

/**

  Routine Description: Returns channel interleve for given socket, Imc, and Sad index.

  @param[in] Socket   - socket number
  @param[in] Imc      - memory controller number
  @param[in] SadInx   - SAD index

  @return Ways of channel interleave for socket/sad

**/
UINT8
GetChInterleave (
  IN UINT8 Socket,
  IN UINT8 Imc,
  IN UINT8 SadInx
  );

/**

  Routine Description: This function computes the
  number of ways iMC has been interleaved for a particular SAD in
  both the Numa and Uma cases.

  @param[in] Socket      - socket number (0 based)
  @param[in] SadIndex    - SAD index

  @return iMC interleave Ways

**/
UINT8
GetImcWays (
  IN UINT8       Socket,
  IN UINT8       SadIndex
  );


/**

  Routine Description: Returns interleave channel bitmap for given socket and SAD.

  @param[in] Socket   - socket number
  @param[in] SadInx   - SAD number

  @return Channel bitmap for given socket/SAD. Counting from BIT0 - channel 1 of first iMC,
          BIT1 - channel 2 of first iMC, etc.

**/
UINT32
GetChInterleaveBitmap (
  IN UINT8 Socket,
  IN UINT8 SadInx
  );

/**

  Routine Description: Find the MC index to use for calculating channel ways.

  @param[in] ImcInterBitmap  - bitmap of IMCs in the interleave.
                               ImcInterBitmap must be a non-zero value in input.
                               Routine assumes value has BIT0 and/or BIT1 set.

  @return IMC number to use for calculating channel ways

**/
UINT8
GetMcIndexFromBitmap (
  IN UINT8 ImcInterBitmap
  );

/**

  Routine Description: This function returns SAD Rule limits based on the
  address passed in.

  @param[in]  SpaAddress   - Spa address used to find containing SAD Rule
  @param[in]  Socket       - socket id, for which SAD limits are to be found
  @param[out] *SpaStart    - start address of SAD rule
  @param[out] *SpaEnd      - end address of SAD rule

  @retval EFI_SUCCESS           - limits found
  @retval EFI_INVALID_PARAMETER - SpaStart or SpaEnd are NULL
  @retval EFI_NOT_FOUND         - limits not found, output variables not changed

**/
EFI_STATUS
GetSadLimits (
  IN  UINT64  SpaAddress,
  IN  UINT8   Socket,
  OUT UINT64  *SpaStart,
  OUT UINT64  *SpaEnd
  );

/**

  Routine Description: This function returns socket granularity for interleave
  (which is really iMC Granularity) as part of the setup options for the memory
  configuration.

  @param[in] SadPtr      - pointer to the SAD

  @return Socket Granularity

**/
UINT32
GetSocketGranularity (
  IN struct SADTable *SadPtr
  );

/**

  Routine Description: This function returns channel granularity for interleave
  as part of the setup options for the memory configuration.

  @param[in] SadPtr      - pointer to the SAD

  @return Channel granularity

**/
UINT32
GetChannelGranularity (
  IN struct SADTable *SadPtr
  );

/**

  Routine Description: Given a SAD base address, function returns channel
  granularity based on the SAD Rule.

  @param[in] SadPtr      - pointer to the SAD

  @return Channel granularity

**/
UINT32
GetChGranularityFromSadBaseAddr (
  IN UINT64 SadBaseAddr
  );

/**

  Routine Description: GetDimm will return the pointer to the Dimm
    structure if found based on socket, channel and dimm arguments.

  @param[in] Socket      - Socket Number
  @param[in] Ch          - DDR Channel ID
  @param[in] Dimm        - DIMM number

  @return AEP_DIMM structure for given Dimm or NULL if not found

**/
AEP_DIMM *
GetDimm (
  IN UINT8 Socket,
  IN UINT8 Ch,
  IN UINT8 Dimm
  );

/**

  Routine Description: Returns the pointer to the Dimm structure if found,
    based on the Index in mCrInfo.AepDimms array.

  @param[in] Index       - Index Number in mCrInfo.AepDimms array

  @return Pointer to AEP_DIMM structure or NULL if Index is out of bounds.

**/
AEP_DIMM *
GetDimmByIndex (
  IN UINT8 Index
  );

/**

  Routine Description: Returns the Index number of the NVMDIMM for a given
    Socket and Channel.

  @param[in]  Socket      - Socket Number
  @param[in]  Ch          - DDR Channel ID in socket
  @param[out] *Index      - Index in mCrInfo.AepDimms array.

  @retval EFI_SUCCESS           Dimm index found
  @retval EFI_NOT_FOUND         Dimm index not found
  @retval EFI_INVALID_PARAMETER Index is NULL or parameters are invalid

**/
EFI_STATUS
GetDimmIndex (
  IN  UINT8 Socket,
  IN  UINT8 Ch,
  OUT UINT8 *Index
  );

/**

  Routine Description: Returns dimm number for
    channel which has a Ngn Dimm.

  @param[in]  Socket      - Socket Number
  @param[in]  Ch          - DDR Channel ID
  @param[out] Dimm        - Dimm number of the channel

  @retval EFI_SUCCESS           Dimm found
  @retval EFI_INVALID_PARAMETER Dimm is NULL
  @retval EFI_NOT_FOUND         Dimm was not found

**/
EFI_STATUS
GetChannelDimm (
  IN  UINT8     Socket,
  IN  UINT8     Ch,
  OUT UINT8     *Dimm
  );

/**

  Routine Description: This function is responsible for getting Socket,
  Channel and Dimm information based on the given System Physical Address.
  That is, to figure out the Nvm Dimm that is mapped to the passed in Spa.

  @param[in]  Spa          - Spa
  @param[out] *SadSpaBase  - Spa base address
  @param[out] *Soc         - Socket that this function will return
  @param[out] *Channel     - Channel that this function will return
  @param[out] *Dimm        - Dimm that this function will return

  @return Return Status

**/
EFI_STATUS
GetDimmInfoFromSpa (
  IN  UINT64      Spa,
  OUT UINT64      *SadSpaBase,
  OUT UINT8       *Soc,
  OUT UINT8       *Channel,
  OUT UINT8       *Dimm
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
EFI_STATUS
SpaToNvmDpa (
  IN  UINT64       Spa,
  OUT UINT8        *Skt,
  OUT UINT8        *Ch,
  OUT UINT8        *Dimm,
  OUT UINT64       *Dpa
  );

/**

  Routine Description: Get/Retrieve the Media Status via Boot status register value.

  @param[in] NvmDimmPtr - Pointer to Dimm structure

  @retval MEDIA_READY                 - media is ready
  @retval WARN_NVMCTRL_MEDIA_NOTREADY - media not ready
  @retval WARN_NVMCTRL_MEDIA_INERROR  - media is in error state
  @retval WARN_NVMCTRL_MEDIA_RESERVED - media status is reserved
  @retval MEDIA_UNKNOWN               - can't get media status

**/
UINT8
GetMediaStatus (
  IN AEP_DIMM *NvmDimmPtr
  );

//
// NvmCtlr access functions
//

/**

  Routine Description: This function will send Log Page Get Long Operation
  Command to the NVMCTLR Controller and return Status of the command sent.

  @param[in] Socket       - Socket Number
  @param[in] Ch           - DDR Channel ID
  @param[in] Dimm         - DIMM number

  @return Status of command sent.

**/
EFI_STATUS
SendFnvGetLongOperationStatus (
  IN UINT8     Socket,
  IN UINT8     Ch,
  IN UINT8     Dimm
  );

/**

  Routine description: returns long operation status for ARS operation.

  @param[in]  Socket    - socket id (0 based)
  @param[in]  Channel   - channel id in socket (0 based)
  @param[in]  Dimm      - dimm id in channel (0 based)
  @param[out] ArsStatus - status for long operation

  @return Status returned by firmware, EFI_INVALID_PARAMETER when dimm not found
          or ArsStatus is NULL.

**/
EFI_STATUS
GetLongOperationStatusDataForArs (
  IN  UINT8         Socket,
  IN  UINT8         Channel,
  IN  UINT8         Dimm,
  OUT ARS_OP_STATUS *ArsStatus
  );

/**

  Routine Description: Gets error logs from NVMCTLR. Uses small payload, so
  request count shouldn't be more than 3 for media logs and 7 for thermal logs.
  FIS 1.3 compatible.

  @param[in] Socket            - Socket Number
  @param[in] Ch                - DDR Channel ID
  @param[in] Dimm              - DIMM Number
  @param[in] LogLevel          - Log Level 0 - low priority 1 - high priority
  @param[in] LogType           - 0 - for media logs, 1 for thermal
  @param[in] LogInfo           - 0 - retrieve log data, 1 - retrieve log info
  @param[in] Sequence          - Reads log from specified sequence number
  @param[in] RequestCount      - Max number of log entries requested for this access
  @param[out] *LogData         - pointer to LogData structure.
                                 Caller must assure this data struct is big enough.

  @retval EFI_INVALID_PARAMETER  - one of parameters is not valid
  @retval EFI_OUT_OF_RESOURCES   - requested number of log entries does not fit in small payload
  @retval EFI_SUCCESS            - success
  @retval other                  - see return codes of SendFnvCommand

**/
EFI_STATUS
SendFnvGetErrorLog (
  IN  UINT8     Socket,
  IN  UINT8     Ch,
  IN  UINT8     Dimm,
  IN  UINT8     LogLevel,
  IN  UINT8     LogType,
  IN  UINT8     LogInfo,
  IN  UINT16    Sequence,
  IN  UINT16    RequestCount,
  OUT VOID      *LogData
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
EFI_STATUS
NvmCtlrSetAddressRangeScrub (
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
EFI_STATUS
NvmCtlrGetAddressRangeScrub (
  IN  UINT8     Socket,
  IN  UINT8     Ch,
  IN  UINT8     Dimm,
  OUT UINT64    *DpaStartAddress,
  OUT UINT64    *DpaEndAddress,
  OUT UINT64    *DpaCurrentAddress,
  OUT BOOLEAN   *Enabled,
  IN  BOOLEAN   UseOsMailbox
  );

EFI_STATUS NvmCtlrReadCfgMem(UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 reg, UINT32 *data);
EFI_STATUS NvmCtlrWriteCfgMem(UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 reg, UINT32 data);
EFI_STATUS NvmCtlrGetHostAlert(UINT8 socket, UINT8 ch, UINT8 dimm, UINT16 hostAlerts[NUM_HOST_TRANSACTIONS]);
EFI_STATUS NvmCtlrSetHostAlert(UINT8 socket, UINT8 ch, UINT8 dimm, UINT16 hostAlerts[NUM_HOST_TRANSACTIONS]);
EFI_STATUS NvmCtlrGetLatestErrorLog(UINT8 Socket, UINT8 Ch, UINT8 Dimm, UINT8 LogLevel, UINT8 LogType, UINT8 Count, VOID * LogBuffer);
EFI_STATUS SendFnvSecurityNonce(UINT8 Socket, UINT8 Ch, UINT8 Dimm);
EFI_STATUS SendFnvSetConfigLockdown(UINT8 socket, UINT8 ch, UINT8 dimm);
EFI_STATUS GetSmartHealthData(UINT8 Socket, UINT8 Ch, UINT8 Dimm, UINT8 *SmartBuffer);

EFI_STATUS UpdateNvmAcpiTable(UINT64 *CrystalRidgeTablePtr);
EFI_STATUS UpdateCrystalRidgeSmBiosTable(VOID);

// OemCrystalRidgeHooks.c
VOID OemGetMemTopologyInfo(UINT8 *skt, UINT8 *ch, UINT8 *dimm);
VOID OemBeforeUpdatePCDBIOSPartition(UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 **cfgCurRecBuffer, UINT32* cfgCurRecLength);
VOID OemBeforeUpdatePCDOSPartition(	UINT8 skt,UINT8 ch, UINT8 dimm, UINT8 **cfgOutRecBuffer, UINT32* cfgOutRecLength);
EFI_STATUS OemCreateNfitTable(UINT64  *Table, UINT32 TableBufferSize);
EFI_STATUS OemCreatePcatTable(UINT64 *Table, UINT64 TableBufferSize);
VOID OemUpdateNfitTable (UINT64  *Table);
VOID OemUpdatePcatTable (UINT64  *Table);
#endif // _CRYSTALRIDGE_H_
