/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file FnvAccess.h
**/


#ifndef _fnvaccess_h
#define _fnvaccess_h

#include "NGNDimmPlatformCfgData.h"
#include "SysHost.h"
#include "MemHostChip.h"
#include "FnvOpcode.h"

#define OPCODE_INDEX             0
#define INPUT_PAYLOAD_INDEX      1
#define OUTPUT_PAYLOAD_INDEX     2
#define IMPLEMENTED_INDEX        3
#define RW_INDEX                 4
#define BUSY                     1
#define NUM_PAYLOAD_REG          32
#define SMALL_PAYLOAD            1
#define RETRIEVE_PARTITION_DATA  0
#define RETRIEVE_PARTITION_SIZE  1
#define NUM_SPARABLE_RANKS       4
#define DOORBELL_TIMEOUT         700 // 700 us for simics 190 us actual
#define COMP_TIMEOUT             700000 // 700 us for simics 190 us actual
#define SECURITY_ENABLED         0x02
#define SECURITY_LOCKED          0x04
#define SECURITY_FROZEN          0x08
#define SECURITY_EXPIRED         4
#define DIMM_CONFIG_BIOS_MAILBOX 0
#define DIMM_CONFIG_OEM_MAILBOX  1
#define DIMM_CONFIG_NAMESPACE    2
#define MAX_HOST_TRANSACTIONS    64
#define NUM_HOST_TRANSACTIONS    12
#define COMMIT_ID                40
#define MAX_LOG_ENTRY            5
#define UNC_ERROR_INTERRUPT      BIT7
#define UNC_ERROR_VIRAL          BIT6
#define DPA_ERROR_INTERRUPT      BIT5
#define DPA_ERROR_VIRAL          BIT4
#define AIT_ERROR_INTERRUPT      BIT3
#define AIT_ERROR_VIRAL          BIT2
#define NO_LOG                   0
#define LOW_PRIORITY             1
#define HIGH_PRIORITY            2
#define FNV_IO_READ              0
#define FNV_IO_WRITE             1
#define WINDOW_TIMEOUT 100000
#define BYTE0    0x1
#define BYTE1    0x2
#define BYTE2    0x4
#define BYTE3    0x8
#define ALL_BYTE 0xF
#define MAJOR_CHECKPOINT 0x00FF
#define MINOR_CHECKPOINT 0xFF00
#define COMMAND_LATENCY_DEFAULT 0x02
#define PASSPHRASE_COUNTER  8
#define PASSPHRASE_LENGTH 32
#define NEW_PASSPHRASE_START 16   //new passphrase start in 4 bytes unit
#define  FNV_MB_READY_TIMEOUT     1000000    // 500 msec
#define  FNV_DDRTIO_INIT_TIMEOUT  500000    // 500 msec
#define  FNV_MEDIA_READY_TIMEOUT  500000    // 500 msec
#define  FNV_CREDIT_READY_TIMEOUT 500000    // 500 msec
#define  FNV_MB_READY     BIT20             // Mailbox Ready         BIT 20    -  0 - Not ready,  1 - Ready
#define  FNV_DT_DONE      BIT18             // NVMDIMM IO Init Complete BIT 18 - 0 - Not ready, 1 - Ready
#define  FNV_CSR_UNLOCK   BIT19
#define  FNV_MEDIA_DONE   (BIT17 | BIT16)     // Media Ready           BIT 17:16 - 00 - Not ready, 01 - Ready, 10 - Error, 11 - Reserved
#define  FNV_MEDIA_READY  BIT16
#define  FNV_MEDIA_ERROR  BIT17
#define  FNV_CREDIT_READY BIT23             // Cready Ready          BIT 23    -  0 - Not ready,  1 - Ready
#define PLL_LOCK                 1
#define DDRT_FREQ                2
#define DDRT_TRAINING_COMPLETE   3

#define MAJOR_VERSION_INDEX           0
#define MINOR_VERSION_INDEX           1
#define HOTFIX_INDEX                  2
#define BUILD_INDEX                   3
#define NUM_FW_REVISION_FIELDS        4
#define NUM_FW_REVISION               2

#define CLK_MOVEMENT_FW               0
#define MAJOR_VERSION_CLK_MOVEMENT    1
#define MINOR_VERSION_CLK_MOVEMENT    0
#define HOTFIX_CLK_MOVEMENT           0
#define BUILD_CLK_MOVEMENT       0x2651
#define BCOM_MARGINING_FW             1
#define MAJOR_VERSION_BCOM_MARGINING  1
#define MINOR_VERSION_BCOM_MARGINING  0
#define HOTFIX_BCOM_MARGINING         0
#define BUILD_BCOM_MARGINING     0x3044

#define MEDIA_READY     0x1 
#define MEDIA_NOTREADY  0x0
#define MEDIA_INERROR   0x2
#define MEDIA_RESERVED  0x3
#define MEDIA_UNKNOWN   0xFF

extern UINT32 FnvPayloadLookupTable [FNV_OPCODE_ARRAY_SIZE][5];
// time is in bcd format
typedef struct fnvSystemTime{
  UINT8  second;
  UINT8  minute;
  UINT8  hour;
  UINT8  day;
  UINT8  month;
  UINT16 year;
}*PFNVSYSTEMTIME;

typedef struct errorLogEntry {
  UINT64_STRUCT     timeStamp;       // Last system time set by BIOS using the set admin feature
  UINT32            timeOffset;      // Power on time in seconds since last system time update
  UINT64_STRUCT     dpaError;        // Specifies the DPA Address of the error
  UINT64_STRUCT     pdaError;        // Specifies the PDA address of the error
  UINT8             range;           // Specifies the length in address space of this error
  UINT8             errorID;         // Indicates what kind of error was logged
  UINT8             transactionType; // Indicates what transaction caused the error
}*ERROR_LOG_ENTRY;

typedef struct errorPageLog {
  UINT8                  totalEntries;    //Total Number of valid entries in the log new or old
  UINT8                  newEntries;      //Specifies the total number of new entries in the log
  UINT8                  overrunFlag;     // flag to indicate that the log fifo had an overrun condition
  UINT8                  returnCount;     // number of log entries returned
  struct errorLogEntry   errorEntry[MAX_LOG_ENTRY]; // Log Entry
}*ERROR_PAGE_LOG;

struct dimmPartition {
  UINT32           volatileCap;     // Volatile Capacity (2LM)
  UINT64_STRUCT    volatileStart;   // Volatile Start Address
  UINT32           persistentCap;   // Persistent Capcity (PM)
  UINT64_STRUCT    persistentStart; // Persistent Start Address
  UINT32           twoLMRegion;     // DPA start address of 2LM Region
  UINT32           PMRegion;        // DPA start address of PM Region
  UINT32           rc;              // Raw Capacity
  UINT32           pending2LM;      // Pending Volatile taking effect on next boot
  UINT32           pendingPM;       // Pending Persistent taking effect on next boot
  UINT64_STRUCT    nonceSecurity;   // BIOS Security Nonce
  UINT32           partitionSize;   // Size of Partition in Bytes
  UINT32           totalPlatform;   // Total Size of Platform Config Area
};

typedef struct powerManagement {

  UINT8             enable;     //0-disable;1-enable set power management policy
  UINT8             TDP;        //(default: 18) The TDP DIMM power limit in W (watts) (range 10-18W).
  UINT16            peakPowerBudget;  // (default: 100) Power budget in mW used for instantaneous power.
  UINT16            averagePowerBudget; // (default: 100) Power budget in mW used for averaged power.

}*POWERMANAGEMENT;

typedef struct dimmInformation {
  UINT16              vendorID;                           // Vendor Identification
  UINT16              deviceID;                           // Device Identification
  UINT16              revisionID;                         // Revision Identification
  UINT16              ifc;                                // Interface Format Code
  struct firmwareRev  fwr;                                // Firmware Revision
  UINT8               api;                                // API Version
  UINT8               fswr;                               // Featured SW Required Mask
  UINT8               ssid;                               // Security SKU Id
  UINT16              nbw;                                // Number of Block Windows
  UINT16              nwfa;                               // Number of Write Flush Addresses
  UINT32              ombcr;                              // Offset of Block Mode Control Region
  UINT32              rc;                                 // Raw Capacity Usabled size (volatile + persistent)
  UINT8               mf[NGN_MAX_MANUFACTURER_STRLEN];    // Manufacturer
  UINT8               sn[NGN_MAX_SERIALNUMBER_STRLEN];    // Serial Number
  UINT8               mn[NGN_MAX_MODELNUMBER_STRLEN];     // Model Number
  UINT8               dimmSku;                            // Dimm SKU info Bit 0:MemorymodeEnabled 1: StoragemodeEnabled 2:App-DirectmodeEnabled 3:DieSparingCapable 4:SoftProgrammableSKU 5:MemorymodeEncryptionEnabled 6:AppDirectmodeEncryptionEnabled 7:StoragemodeEncrytionEnabled
  UINT64_STRUCT       wfas;                               // Starting Address for Write Flush Addresses
}*DIMMINFORMATION;

typedef struct fnvInformation {
  struct firmwareRev  fwr;                                // Firmware Revision
  UINT8               fwrType;                            // Firmware Type
  UINT8               sfwStatus;
  struct firmwareRev  sfwr;
  UINT8               sfwrType;
  UINT8               commitId[COMMIT_ID];
}*FNVINFORMATION;

struct platformConfiguration {
  UINT32              currentSize;
  UINT32              totalSize;
};

typedef struct fnvCISBuffer {
  struct dimmInformation        dimmInfo[MAX_SOCKET][MAX_CH][MAX_DIMM];
  struct dimmPartition          dimmPart[MAX_SOCKET][MAX_CH][MAX_DIMM];
  struct platformConfiguration  platformConfig[MAX_SOCKET][MAX_CH][MAX_DIMM];
  UINT32                        platformData[NUM_PAYLOAD_REG];
  UINT32                        platformSize;
  UINT64_STRUCT                 securityNonce;
  UINT16                        hostAlerts[MAX_HOST_TRANSACTIONS];
  struct errorPageLog           errorLog[MAX_SOCKET][MAX_CH][MAX_DIMM];
  struct fnvSystemTime          time;
}*FNVCISBUFFER;

UINT32 ReadFnvCfg (PSYSHOST host,UINT8 socket,UINT8 ch, UINT8 dimm, UINT32 reg);
UINT32 WriteFnvCfg (PSYSHOST host,UINT8 socket,UINT8 ch, UINT8 dimm, UINT32 reg, UINT32 data);
UINT32 IdentifyDimm (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, DIMMINFORMATION dimmInformation, UINT8 *mbStatus);
UINT32 GetDimmPartitionInfo (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, FNVCISBUFFER fnvBuffer, UINT8 *mbStatus);
UINT32 SetDimmPartitionInfo (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 twoLM, UINT32 PM, FNVCISBUFFER fnvBuffer, UINT8 *mbStatus);
UINT32 SetPowerManagementPolicy (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, POWERMANAGEMENT powerManagement, UINT8 *mbStatus);
UINT32 GetPowerManagementPolicy (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, POWERMANAGEMENT powerManagement, UINT8 *mbStatus);
UINT32 GetPlatformConfigData (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 partitionID, UINT8 retrieve_option, UINT32 offset, FNVCISBUFFER fnvBuffer, UINT8 *mbStatus);
UINT32 StatusDecoder (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 status);
UINT32 SetPlatformConfigData(PSYSHOST host, UINT8 socket,UINT8 ch, UINT8 dimm, UINT8 partitionID, UINT32 offset, FNVCISBUFFER fnvBuffer, UINT8 *mbStatus);
UINT32 SetSecurityNonce(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT64_STRUCT nonce, FNVCISBUFFER fnvBuffer, UINT8 *mbStatus);
UINT32 GetSecurityState(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 *security, UINT8 *mbStatus);
UINT32 SecureEraseUnit(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 *passphrase, UINT8 *mbStatus);
UINT32 SetPassphrase(PSYSHOST host,UINT8 socket,UINT8 ch,UINT8 dimm,UINT8 security, UINT8 *currentPassphrase, UINT8 *newPassphrase, UINT8 *mbStatus); 
UINT32 SetDDRTAlert(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT16 hostAlerts[MAX_HOST_TRANSACTIONS], UINT8 *mbStatus);
UINT32 SetConfigLockdown(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 *mbStatus);
UINT32 SetFnvSystemTime(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 *mbStatus);
UINT32 FnvSimicsSim(PSYSHOST host);
UINT32 FnvDdrtIoInit(PSYSHOST host, UINT8 socket, UINT8 payload);
UINT32 SetDieSparingPolicy(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 Enable, UINT8 Aggressiveness, UINT8 *mbStatus);
void EarlyFnvConfigAccess (PSYSHOST host, UINT8 socket);
void LateFnvConfigAccess (PSYSHOST host, UINT8 socket);
void EmulationReadFnvCfg (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 reg, UINT32 mask, UINT8 logic, UINT32 offset);
void EmulationReadFnvCfgSmbus (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 reg, UINT32 mask, UINT8 logic);
UINT32 GetSetFnvIO (PSYSHOST host,UINT8 socket,UINT8 ch,UINT8 dimm,UINT32 reg,UINT32 *data,UINT8 IA,UINT8 mode, UINT8 byteMask);
UINT32 GetFnvInfo (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, FNVINFORMATION fnvInformation, UINT8 *mbStatus);
void CheckValidFirmwareVersion(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, FNVINFORMATION FnvInformation);
#ifdef MEMMAPSIM_BUILD
extern UINT32 MMS_FnvMailboxInterface ( PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 inputPayload[NUM_PAYLOAD_REG], UINT32 outputPayload[NUM_PAYLOAD_REG], UINT32 opcode);
#endif

struct dimmPartition (*GetFnvCisBufferDimmPartition (FNVCISBUFFER  fnvBuffer)) [MAX_SOCKET][MAX_CH][MAX_DIMM];

VOID FnvBcwRead (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 dpa, UINT8 data[MAX_STROBE/2]);

UINT32 ReadFnvCfgSmb(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 reg, UINT32 *data);          // CHIP
UINT32 WriteFnvCfgSmb (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 reg, UINT32 data);         // CHIP
UINT32 WriteFnvCfgEmrs (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 reg, UINT32 data);        // CHIP
void   EmulationPoll (PSYSHOST host, UINT8 socket, UINT8 BoxInst, UINT32 Offset, UINT32 mask, UINT8 logic); // CHIP

#endif // _fnvaccess_h
