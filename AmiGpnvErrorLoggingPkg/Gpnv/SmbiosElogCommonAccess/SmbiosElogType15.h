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
/** @file SmbiosElogType15.h
    SmbiosElogType15 definitions

**/


#ifndef _EFI_SMBIOS_ELOG_TYPE_15_H_
#define _EFI_SMBIOS_ELOG_TYPE_15_H_

//---------------------------------------------------------------------------

#include "SmbiosElogFlashFuncs.h"
#include <Protocol/SmbiosElogSupport.h>

//---------------------------------------------------------------------------

// "Log Area Full" bit in LogStatus field of Type15 Structure.

#define SMBIOS_EVENT_LOG_FULL_BIT 0x00000002

// Structure for Event Log Flash Area Header (Type 1)

#pragma pack(1)
typedef struct {

// OEMReserved[5]
//    Used to store setup question values so that SMI is aware of them
//    Byte 0:  Set to 0xEE to indicate valid values in other Bytes
//    Byte 1:
//      bit 0, if set, logging of our OEM defined Status Code Type is
//         enabled
//      bit 1, if set, conversion of EFI Status Codes to Legacy Codes
//         (if available) is enabled
//      bit 2, if set, logging of Non-ECC Errors is enabled
//      bit 3, if set, logging of ECC Errors is enabled
//      bit 4, if set, enable errors ??? DON'T KNOW WHAT THIS SETUP
//         VALUE IS FOR
//      bit 5, if set, Enable & escalate fatal errors to MCERR
//      bit 6, if set, Enable & escalate non-fatal uncorrectable errors
//         to MCERR
//    Bytes 2-4, Reserved

  UINT8 OEMReserved[5];
  UINT8 MultipleEventTimeWindow;
  UINT8 MultipleEventCountIncrement;
  UINT8 PreBootEventLogResetCMOSAddress; // 00h for not supported
  UINT8 PreBootEventLogResetCMOSBit;
  UINT8 CMOSChecksumStartingOffset;
  UINT8 CMOSChecksumByteCount;
  UINT8 CMOSChecksumChecksumOffset;
  UINT8 RESERVED[3];
  UINT8 HeaderRevision; // Always 01h to signify Type 1 header
} EFI_GPNV_ERROR_HEADER_TYPE1;
#pragma pack()

// Data for Event Log Flash Area Header (Type 1)

static EFI_GPNV_ERROR_HEADER_TYPE1 GPNVErrorLogHeaderType1 = {
  0x00, 0x00, 0x00, 0x00, 0x00,         // OEM Reserved
  0x60,                                 // METW Default Values
  0x01,                                 // MECI Default Values
  0x00,                                 // CMOS Address
  0x00,                                 // CMOS Bit
  0x00,                                 // Cksum start offset
  0x00,                                 // Cksum Byte Count
  0x00,                                 // Cksum Offset
  0x00, 0x00, 0x00,                     // Reserved
  0x01                                  // Header Revision
};

// Structure for Smbios Event Log Types and Corresponding Variable Data Types

#pragma pack(1)
typedef struct {
  UINT8 LogType;
  UINT8 VariableDataFormatType;
} EFI_ELOG_TYPE_DESCRIPTOR;
#pragma pack()

// Data for Smbios Event Log Types and Corresponding Variable Data Types

static EFI_ELOG_TYPE_DESCRIPTOR  SupportedEventLogTypesList[] =
{
  {EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR,
      EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE },

  {EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR,
      EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE },

  {EFI_EVENT_LOG_TYPE_PARITY_MEMORY_ERROR,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_BUS_TIME_OUT,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_IO_CHANNEL_CHECK,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_SOFTWARE_NMI,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_POST_MEMORY_RESIZE,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_POST_ERROR,
      EFI_EVENT_LOG_VAR_DATA_TYPE_POST_RESULTS_BITMAP },

  {EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR,
      EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE },

  {EFI_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR,
      EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE },

  {EFI_EVENT_LOG_TYPE_CPU_FAILURE,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_EISA_FAILSAFE_TIMER_TIME_OUT,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_CORRECTABLE_MEMORY_LOG_DISABLED,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_LOGGING_DISABLED_FOR_EVENT_TYPE,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_SYSTEM_LIMIT_EXCEEDED,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_ASYN_HW_TIMER_EXPIRED,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_SYSTEM_CONFIG_INFORMATION,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_HARD_DISK_INFORMATION,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_SYSTEM_RECONFIGURED,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_UNCORRECTABLE_CPU_COMPLEX_ERROR,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_LOG_AREA_RESET,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_SYSTEM_BOOT,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_END_OF_LOG,
      EFI_EVENT_LOG_VAR_DATA_TYPE_NONE },

  {EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE,
      EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_STATUS_CODE },

  {EFI_EVENT_LOG_TYPE_OEM_DUMMY,
      EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_DUMMY }
};
#define NumOfSupportedSmbiosTypes \
  ( sizeof(SupportedEventLogTypesList) / sizeof(EFI_ELOG_TYPE_DESCRIPTOR) )

// Structure for Smbios Type 15 Table Entry (without Supported Types at end)

#pragma pack(1)
typedef struct {
  UINT8   Type;                         // Always 15
  UINT8   Length;                       // Length of Structure 2.1+ : 17h + 
                                        // (LengthOfDescriptor * 
                                        // NumSupportedLogTypes)
  UINT16  Handle;                       // Handle or instance number
  UINT16  LogAreaLength;                // Length, in bytes, of the overall  
                                        // event log area, 1st byte of header
                                        // to last of data
  UINT16  LogHeaderStartOffset;         // offset of event log header from  
                                        // access method address
  UINT16  LogDataStartOffset;           // offset of event log data from  
                                        // access method address
  UINT8   AccessMethod;                 // defines method of higher-level  
                                        // software accessing table
  UINT8   LogStatus;                    // current status of the system event 
                                        // log
                                        //   bit 2-7: Reserved (set to 0)
                                        //   bit 1: log area full if 1
                                        //   bit 0: log area valid if 1
  UINT32  LogChangeToken;               // unique token changed every time  
                                        // log changes
  UINT32  AccessMethodAddress;          // address associated with access  
                                        // method
  UINT8   LogHeaderFormat;              // identifies the format of the log  
                                        // header area
  UINT8   NumSupportedLogTypes;         // number of supported event log type
                                        // descriptors that follow
  UINT8   LengthOfDescriptor;           // Always 2

// SupportedEventLogTypesList[] to be added here to 
// build final type 15 entry

} EFI_SYSTEM_EVENT_LOG_TYPE_15;
#pragma pack()

// Data for Smbios Type 15 Table Entry (without Supported Types at end)

static EFI_SYSTEM_EVENT_LOG_TYPE_15 SystemEventLogType15Table = {
  15,                                   // Type
  0x17+(2*NumOfSupportedSmbiosTypes),   // Length
  0x0000,                               // Handle or instance number
  (UINT16)LOCAL_EVENT_LOG_LENGTH,       // Length, in bytes, of the overall 
                                        // event log area, 1st byte of header
                                        // to last of data
  0x0000,                               // offset of event log header from 
                                        // access method address
  sizeof(EFI_GPNV_ERROR_HEADER_TYPE1),  // offset of event log data from 
                                        // access method address
  0x03,                                 // defines method of higher-level 
                                        // software accessing table
  0x01,                                 // current status of the system event
                                        // log
  0x00000001,                           // unique token changed every time log
                                        // changes
  LOCAL_EVENT_LOG_PHY_ADDR,             // address associated with access 
                                        // method
  0x01,                                 // identifies the format of the log 
                                        // header area
  NumOfSupportedSmbiosTypes,            // number of supported event log type
                                        // descriptors that follow
  0x02                                  // Always 2

// SupportedEventLogTypesList[] to be added here 
// to build final type 15 entry
};

#if GPNV_LISTENER
EFI_STATUS LogInfoToGpnv (
  IN  UINT8 EventType);
#endif

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