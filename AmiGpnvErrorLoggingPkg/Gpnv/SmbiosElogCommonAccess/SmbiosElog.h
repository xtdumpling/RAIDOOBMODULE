//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev. 1.00
//      Bug Fixed:  Add code to log CMOS Battery Low event into SMBIOS(POST Error Type 08h).
//      Reason:    
//      Auditor:    Jimmy Chiu
//      Date:       Feb/23/2017
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file SmbiosElog.h
    SmbiosElog header file function declarations, structures, constants

**/


#ifndef _EFI_SMBIOS_ELOG_H_
#define _EFI_SMBIOS_ELOG_H_
#define VFRCOMPILE


// Adding all the header file's


//---------------------------------------------------------------------------

#include <Include/AmiLib.h>
#include "SmbiosElogFlashFuncs.h"
#include "SmbiosElogType15.h"
#include "DxeSmmRedirFuncs.h"
#include <Setup.h>

// Including all the library's

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

// Including all the protocol's

#include <Protocol/AmiSmbios.h>
#include <Protocol/SmbiosGetFlashDataProtocol.h>
#include <Protocol/SmbiosElogSupport.h>
#include <Protocol/GpnvRedirProtocol.h>
#include <Protocol/DataHub.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <AmiDxeLib.h>

//---------------------------------------------------------------------------

EFI_RUNTIME_SERVICES     *gRT;


#define MAX_EVENT_TYPES 0xFF


// To calculate buffer size divide total event log length with minimum event 
// size possible. This will give max no events can be stored.

#define MAX_NO_EVENTS_STORED \
       LOCAL_EVENT_LOG_LENGTH /(sizeof(HANDLE_DATA) + sizeof(EFI_GPNV_ERROR_EVENT))

// Macros For accessing time from RTC

#ifndef RTC_ADDRESS_REGISTER
#define RTC_ADDRESS_REGISTER        0x70
#endif

#ifndef RTC_DATA_REGISTER
#define RTC_DATA_REGISTER           0x71
#endif

#ifndef RTC_SECONDS_OFFSET
#define RTC_SECONDS_OFFSET           0   // R/W  Range 0..59
#endif

#ifndef RTC_MINUTES_OFFSET
#define RTC_MINUTES_OFFSET           2   // R/W  Range 0..59
#endif

#ifndef RTC_HOURS_OFFSET
#define RTC_HOURS_OFFSET             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#endif

#ifndef RTC_DAY_OFFSET
#define RTC_DAY_OFFSET               7   // R/W  Range 1..31
#endif

#ifndef RTC_MONTH_OFFSET
#define RTC_MONTH_OFFSET             8   // R/W  Range 1..12
#endif

#ifndef RTC_YEAR_OFFSET
#define RTC_YEAR_OFFSET              9   // R/W  Range 0..99
#endif

#pragma pack(1)

// Structure definition of the Private data


typedef struct {

// NOTE: These Two must not be changed in Position.For use in Locating the 
// beginning of this structure in memory in both Regular and SMM memory 

  EFI_SM_ELOG_REDIR_PROTOCOL        SmbiosElog;


  AMI_SMBIOS_ELOG_SUPPORT_PROTOCOL  SmbiosElogSupport;

  UINT8*                            StartAddress;
  UINT8*                            EndAddress;
  UINT8*                            NextAddress;
  UINT8                             MECI;
  UINT8                             METW;
  BOOLEAN                           EraseEventLogWhenFull;
  BOOLEAN                           SystemBootRecordLogging;
  BOOLEAN                           EfiStatusCodesEnabled;
  BOOLEAN                           ConvertEfiToLegacyFlag;
  EFI_ELOG_TYPE_DESCRIPTOR          ValidSmbiosTypes[NumOfSupportedSmbiosTypes];
  UINT8                             CopyOfFlashArea[LOCAL_EVENT_LOG_LENGTH];
  UINT32                            NumberOfMultipleEventRecords;
  UINT8                             BufferForErasing[BUFFER_SIZE_NEEDED_FOR_ERASING];
  UINT32                            MultiEventOffset[MAX_NO_EVENTS_STORED];
  UINT8                             InternalCounter[MAX_NO_EVENTS_STORED];
  UINT32                            RecordId;
} EFI_SMBIOS_ELOG_INSTANCE_DATA;

#pragma pack()


extern  EFI_SMBIOS_ELOG_INSTANCE_DATA*  mRedirSmbiosPrivate;

// Entry Point declaration

EFI_STATUS
InitializeSmbiosElog (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
);

// GenericElog Redirect Functions declarations

EFI_STATUS
EfiSetSmbiosElogData (
  IN  EFI_SM_ELOG_REDIR_PROTOCOL        *This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_REDIR_TYPE            DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             Size,
  OUT UINT64                            *RecordId
);

EFI_STATUS
EfiGetSmbiosElogData (
  IN EFI_SM_ELOG_REDIR_PROTOCOL         *This,
  IN UINT8                              *ElogData,
  IN EFI_SM_ELOG_REDIR_TYPE             DataType,
  IN OUT UINTN                          *Size,
  IN OUT UINT64                         *RecordId
);

EFI_STATUS
EfiEraseSmbiosElogData (
  IN EFI_SM_ELOG_REDIR_PROTOCOL         *This,
  IN EFI_SM_ELOG_REDIR_TYPE             DataType,
  IN OUT UINT64                         *RecordId
);

EFI_STATUS
EfiActivateSmbiosElog (
  IN  EFI_SM_ELOG_REDIR_PROTOCOL         *This,
  IN  EFI_SM_ELOG_REDIR_TYPE             DataType,
  IN  BOOLEAN                            *EnableElog,
  OUT BOOLEAN                            *ElogStatus
);

// Local Function declarations


UINT8
TimeDifference (
  IN  EFI_GPNV_ERROR_EVENT            EventToUse1,
  IN  EFI_GPNV_ERROR_EVENT            EventToUse2
);

BOOLEAN
DuplicateRecordAndAlreadyReacted (
  IN  EFI_SMBIOS_ELOG_INSTANCE_DATA      *SmbiosElogPrivateData,
  IN  EFI_GPNV_ERROR_EVENT               *EventToUse,
  IN  UINT8                              *ExtendedData,
  OUT UINT64                             *RecordId,
  UINT8                                  VariableDataType
);

VOID SmbiosElogVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
);

EFI_STATUS
EfiEraseSmbiosElogDataLocal (
  EFI_SMBIOS_ELOG_INSTANCE_DATA *SmbiosElogPrivateData
);

BOOLEAN
IsValidEvent (
  IN  EFI_GPNV_ERROR_EVENT            *EventToUse,
  IN  EFI_SMBIOS_ELOG_INSTANCE_DATA   *SmbiosElogPrivateData
);

UINT8
RtcRead (
  IN  UINT8  Address
);

EFI_STATUS
FillEventWithTimeAndDefaults (
  IN OUT  EFI_GPNV_ERROR_EVENT    *EventPtr
);

EFI_STATUS
InsertSystemBootRecord (VOID);

EFI_STATUS
AddSMBIOSType15Structure (
  IN EFI_EVENT        Event,
  IN VOID             *Context
);

EFI_STATUS
CheckIfFull(
  IN EFI_SMBIOS_ELOG_INSTANCE_DATA    *SmbiosElogPrivateData,
  IN UINT8                             EventSize
);



EFI_STATUS
ErrorLoggingEraseCustomBlockRange (
  IN UINT8*                               StartAddress,
  IN UINT8*                               EndAddress,
  IN EFI_SMBIOS_ELOG_INSTANCE_DATA        *SmbiosElogPrivateData
  );

EFI_STATUS
ErrorLoggingReadFlashIntoMemoryCopy (VOID);

EFI_STATUS
RecordMemoryCopyDifferencesToFlash (
  IN EFI_SMBIOS_ELOG_INSTANCE_DATA  *SmbiosElogPrivateData,
  IN UINT32                         StartOffset,
  IN UINT32                         EndOffset
);

EFI_STATUS
ResetAllDataInMemoryCopy(
  EFI_SMBIOS_ELOG_INSTANCE_DATA *SmbiosElogPrivateData
);

UINT8
GetEventRecordSize (
  IN  ALL_GPNV_ERROR   *Event,
  IN  UINT8            VariableDataType
);

EFI_STATUS
GpnvStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
);

EFI_STATUS
GetEventVariableDataType (
  IN  UINT8    EventType,
  OUT UINT8    *VariableDataType
);

// FOR ERROR TRANSLATION


//----------------------------------------------------------------------------
//  Extended Data Descriptions
//----------------------------------------------------------------------------
//
//  if  System Management (Type 0x10)
//      bit  15     = bit 31 of dword (bits 30 to 15 of dword are ignored)
//      bits 14-0   = bits 14-0 of dword
//
//
//  if  Post Error (Type 0x08)
//      bits 15-0   = bit number in POST?s 2 DWORDS
//              1st DWORD Starts at bit 0, 2nd DWORD starts at bit 32
//
//  if  Other Error w/ Handle
//      bits 15-0	= Handle
//
//
//  else bits 15-0 = 0xFFFF
//----------------------------------------------------------------------------
#pragma pack(1)
typedef struct {
  UINT8  Type;
  UINT16 ExtendedData;
} LEGACY_CODE_STRUCTURE;
#pragma pack ()


EFI_STATUS
ConvertEfiToLegacy (
  IN     UINT32                       EfiStatusCodeValue,
  IN OUT LEGACY_CODE_STRUCTURE        *LegacyCode
);


typedef struct {
  UINT32 EfiValue;
  LEGACY_CODE_STRUCTURE LegacyErrorCode; 
} STATUS_CODE_TO_SMBIOS_TYPE_TABLE;

typedef UINT32 EFI_STATUS_CODE_TYPE;
typedef UINT32 EFI_STATUS_CODE_VALUE;

EFI_GUID gEfiStatusCodeGuid;

// Transition table to convert EFI status code to legacy status code


static STATUS_CODE_TO_SMBIOS_TYPE_TABLE TranslationTable1[] = {
//      { EFI_COMPUTING_UNIT_UNSPECIFIED | EFI_CU_EC_NON_SPECIFIC,         {0,0} },
//      { EFI_COMPUTING_UNIT_UNSPECIFIED | EFI_CU_EC_DISABLED,         {0,0} },
//      { EFI_COMPUTING_UNIT_UNSPECIFIED | EFI_CU_EC_NOT_SUPPORTED,         {0,0} },
//      { EFI_COMPUTING_UNIT_UNSPECIFIED | EFI_CU_EC_NOT_DETECTED,         {0,0} },
//      { EFI_COMPUTING_UNIT_UNSPECIFIED | EFI_CU_EC_NOT_CONFIGURED,         {0,0} },
    
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_NON_SPECIFIC,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_DISABLED,         {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_NOT_SUPPORTED,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_NOT_DETECTED,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_NOT_CONFIGURED,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_INVALID_TYPE,      {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_INVALID_SPEED,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_MISMATCH,      {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_TIMER_EXPIRED,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_SELF_TEST,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_INTERNAL,         {0,0} },
      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_THERMAL,         {EFI_EVENT_LOG_TYPE_SYSTEM_LIMIT_EXCEEDED,0x0011} },
      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_LOW_VOLTAGE,     {EFI_EVENT_LOG_TYPE_SYSTEM_LIMIT_EXCEEDED,0x0000} }, // ???
      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_HIGH_VOLTAGE,     {EFI_EVENT_LOG_TYPE_SYSTEM_LIMIT_EXCEEDED,0x0005} }, // ???
      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_CACHE,         {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_MICROCODE_UPDATE,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_CORRECTABLE,     {0,0} },
//      { EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_UNCORRECTABLE,     {0,0} },
    
//      { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_EC_NON_SPECIFIC,     {0,0} },
//      { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_EC_DISABLED,     {0,0} },
//      { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_EC_NOT_SUPPORTED,     {0,0} },
//      { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_EC_NOT_DETECTED,     {0,0} },
//      { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_HARD_FAIL,     {0,0} },
//      { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SOFT_FAIL,     {0,0} },
//      { EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_COMM_ERROR,     {0,0} },
    
//      { EFI_COMPUTING_UNIT_IO_PROCESSOR | EFI_CU_EC_NON_SPECIFIC,         {0,0} },
//      { EFI_COMPUTING_UNIT_IO_PROCESSOR | EFI_CU_EC_DISABLED,          {0,0} },
//      { EFI_COMPUTING_UNIT_IO_PROCESSOR | EFI_CU_EC_NOT_SUPPORTED,         {0,0} },
//      { EFI_COMPUTING_UNIT_IO_PROCESSOR | EFI_CU_EC_NOT_DETECTED,         {0,0} },
//      { EFI_COMPUTING_UNIT_IO_PROCESSOR | EFI_CU_EC_NOT_CONFIGURED,     {0,0} },
    
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_EC_NON_SPECIFIC,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_EC_DISABLED,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_EC_NOT_SUPPORTED,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_EC_NOT_DETECTED,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_EC_NOT_CONFIGURED,        {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_CACHE_EC_INVALID_TYPE,        {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_CACHE_EC_INVALID_SPEED,        {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_CACHE_EC_INVALID_SIZE,        {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
      { EFI_COMPUTING_UNIT_CACHE | EFI_CU_CACHE_EC_MISMATCH,        {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR_BIT} },
    
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_EC_DISABLED,             {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_EC_NOT_SUPPORTED,         {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_EC_NOT_DETECTED,         {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_EC_NOT_CONFIGURED,         {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_INVALID_TYPE,      {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_INVALID_SPEED,     {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_CORRECTABLE,         {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_UNCORRECTABLE,     {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_SPD_FAIL,         {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_INVALID_SIZE,         {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_MISMATCH,         {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_S3_RESUME_FAIL,     {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_UPDATE_FAIL,         {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED,     {0,0} },
//      { EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_USEFUL,         {0,0} },
      
//      { EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_EC_NON_SPECIFIC,         {0,0} },
//      { EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_EC_DISABLED,             {0,0} },
//      { EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_EC_NOT_SUPPORTED,          {0,0} },
//      { EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_EC_NOT_DETECTED,         {0,0} },
//      { EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_EC_NOT_CONFIGURED,         {0,0} },
    
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_NON_SPECIFIC,         {0,0} },
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_DISABLED,             {0,0} },
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_NOT_SUPPORTED,          {0,0} },
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_NOT_DETECTED,         {0,0} },
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_NOT_CONFIGURED,         {0,0} },
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_INTERFACE_ERROR,         {0,0} },
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_CONTROLLER_ERROR,         {0,0} },
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_INPUT_ERROR,             {0,0} },
//      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_OUTPUT_ERROR,         {0,0} },
      { EFI_PERIPHERAL_UNSPECIFIED | EFI_P_EC_RESOURCE_CONFLICT,         {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NON_SPECIFIC,             {0,0} },
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_DISABLED,             {0,0} },
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NOT_SUPPORTED,             {0,0} },
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NOT_DETECTED,             {0,0} },
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NOT_CONFIGURED,             {0,0} },
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_INTERFACE_ERROR,         {0,0} },
      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR,         {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_KBD_CONTR_NOT_FUNC_BIT} },
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_INPUT_ERROR,             {0,0} },
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_OUTPUT_ERROR,             {0,0} },
//      { EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_RESOURCE_CONFLICT,          {0,0} },
      { EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_EC_LOCKED,         {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_KBD_LOCKED_BIT} },
      { EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_EC_STUCK_KEY,          {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_KBD_NOT_FUNC_BIT} },
    
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_NON_SPECIFIC,         {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_DISABLED,             {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_NOT_SUPPORTED,         {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_NOT_DETECTED,         {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_NOT_CONFIGURED,          {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_INTERFACE_ERROR,         {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_CONTROLLER_ERROR,        {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_INPUT_ERROR,         {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_OUTPUT_ERROR,         {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_EC_RESOURCE_CONFLICT,         {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
      { EFI_PERIPHERAL_MOUSE | EFI_P_MOUSE_EC_LOCKED,         {EFI_EVENT_LOG_TYPE_POST_ERROR,10} },  // FOR TESTING ONLY!!!!!!!!!
    
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NON_SPECIFIC,     {0,0} },
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_DISABLED,         {0,0} },
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_SUPPORTED,     {0,0} },
      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_DETECTED,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_PRIMARY_OUTPUT_DEV_NOT_FOUND_BIT} },
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_CONFIGURED,       {0,0} },
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_INPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_RESOURCE_CONFLICT,  {0,0} },
    
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_DISABLED,      {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_NOT_SUPPORTED,      {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_NOT_DETECTED,      {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_NOT_CONFIGURED,   {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_INTERFACE_ERROR,  {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_CONTROLLER_ERROR, {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_INPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_REMOTE_CONSOLE  | EFI_P_EC_RESOURCE_CONFLICT,{0,0} },
    
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_DISABLED,          {0,0} },
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_NOT_DETECTED,      {0,0} },
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_NOT_CONFIGURED,       {0,0} },
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_INPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
      { EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SERIAL_PORT_1_RESOURCE_ERROR_BIT} },
     
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_DISABLED,          {0,0} },
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_NOT_DETECTED,      {0,0} },
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_NOT_CONFIGURED,       {0,0} },
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_INPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
      { EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_RESOURCE_CONFLICT,  {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_PARALLEL_PORT_RESOURCE_ERROR_BIT} },
    
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_DISABLED,          {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NOT_DETECTED,      {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NOT_CONFIGURED,       {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_INPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_RESOURCE_CONFLICT,      {0,0} },
     
//      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_DISABLED,      {0,0} },
//      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_NOT_DETECTED,      {0,0} },
//      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_NOT_CONFIGURED,   {0,0} },
//      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_INTERFACE_ERROR,  {0,0} },
      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_CONTROLLER_ERROR, {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_FLOPPY_CONTR_FAILURE_BIT} },
//      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_INPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
      { EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_RESOURCE_CONFLICT,{EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_FDC_RESOURCE_CONFLICT_BIT} },
    
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_DISABLED,          {0,0} },
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_NOT_DETECTED,      {0,0} },
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_NOT_CONFIGURED,       {0,0} },
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_INPUT_ERROR,         {0,0} },
//      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
      { EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_EC_RESOURCE_CONFLICT,     {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_AUDIO_RESOURCE_CONFLICT_BIT} },
    
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_DISABLED,         {0,0} },
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_NOT_DETECTED,      {0,0} },
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_NOT_CONFIGURED,       {0,0} },
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_CONTROLLER_ERROR,     {0,0} },
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_INPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
      { EFI_PERIPHERAL_AUDIO_OUTPUT | EFI_P_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_AUDIO_RESOURCE_CONFLICT_BIT} },
    
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_DISABLED,          {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_NOT_DETECTED,         {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_INPUT_ERROR,          {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_OUTPUT_ERROR,      {0,0} },
//      { EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_RESOURCE_CONFLICT,      {0,0} },
    
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_NOT_DETECTED,      {0,0} },
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_INTERFACE_ERROR,    {0,0} },
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_UNSPECIFIED  | EFI_IOB_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_NON_SPECIFIC,       {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_NOT_DETECTED,       {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_PCI | EFI_IOB_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_PCI_MEM_CONFLICT_BIT} },
//      { EFI_IO_BUS_PCI | EFI_IOB_PCI_EC_PERR,          {0,0} },
//      { EFI_IO_BUS_PCI | EFI_IOB_PCI_EC_SERR,          {0,0} },
    
//      { EFI_IO_BUS_USB | EFI_IOB_EC_NON_SPECIFIC,       {0,0} },
//      { EFI_IO_BUS_USB | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_USB | EFI_IOB_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_IO_BUS_USB | EFI_IOB_EC_NOT_DETECTED,       {0,0} },
//      { EFI_IO_BUS_USB | EFI_IOB_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_IO_BUS_USB | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_USB | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_USB | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_USB | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_USB | EFI_IOB_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_NON_SPECIFIC,       {0,0} },
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_NOT_DETECTED,          {0,0} },
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_IBA | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_IBA | EFI_IOB_EC_RESOURCE_CONFLICT,     {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_NON_SPECIFIC,           {0,0} },
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_NOT_DETECTED,       {0,0} },
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_AGP | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_AGP | EFI_IOB_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_NON_SPECIFIC,       {0,0} },
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_NOT_SUPPORTED,      {0,0} },
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_NOT_DETECTED,      {0,0} },
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_READ_ERROR,      {0,0} },
//      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_WRITE_ERROR,      {0,0} },
      { EFI_IO_BUS_PC_CARD | EFI_IOB_EC_RESOURCE_CONFLICT,  {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_NON_SPECIFIC,       {0,0} },
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_NOT_SUPPORTED,      {0,0} },
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_NOT_DETECTED,       {0,0} },
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_CONTROLLER_ERROR,     {0,0} },
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_LPC | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_LPC | EFI_IOB_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_NON_SPECIFIC,       {0,0} },
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_NOT_DETECTED,       {0,0} },
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_NOT_CONFIGURED,     {0,0} },
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_READ_ERROR,         {0,0} },
//      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_WRITE_ERROR,      {0,0} },
      { EFI_IO_BUS_SCSI  | EFI_IOB_EC_RESOURCE_CONFLICT,     {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_PRI_ATA_CNTR_RESOURCE_ERROR_BIT} },
     
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_DISABLED,      {0,0} },
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_NOT_SUPPORTED,      {0,0} },
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_NOT_DETECTED,      {0,0} },
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_NOT_CONFIGURED,      {0,0} },
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_INTERFACE_ERROR,     {0,0} },
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_CONTROLLER_ERROR,    {0,0} },
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_READ_ERROR,    {0,0} },
//      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_WRITE_ERROR,      {0,0} },
      { EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_RESOURCE_CONFLICT,{EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_PRI_ATA_CNTR_RESOURCE_ERROR_BIT} },
     
//      { EFI_IO_BUS_FC | EFI_IOB_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_IO_BUS_FC | EFI_IOB_EC_DISABLED,         {0,0} },
//      { EFI_IO_BUS_FC | EFI_IOB_EC_NOT_SUPPORTED,       {0,0} },
//      { EFI_IO_BUS_FC | EFI_IOB_EC_NOT_DETECTED,           {0,0} },
//      { EFI_IO_BUS_FC | EFI_IOB_EC_NOT_CONFIGURED,        {0,0} },
//      { EFI_IO_BUS_FC | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_FC | EFI_IOB_EC_CONTROLLER_ERROR,     {0,0} },
//      { EFI_IO_BUS_FC | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_FC | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_FC | EFI_IOB_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_NON_SPECIFIC,         {0,0} },
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_DISABLED,         {0,0} },
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_NOT_SUPPORTED,          {0,0} },
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_NOT_DETECTED,         {0,0} },
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_NOT_CONFIGURED,         {0,0} },
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_INTERFACE_ERROR,        {0,0} },
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_CONTROLLER_ERROR,    {0,0} },
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_READ_ERROR,         {0,0} },
//      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_IP_NETWORK | EFI_IOB_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
     
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_DISABLED,          {0,0} },
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_NOT_SUPPORTED,      {0,0} },
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_NOT_DETECTED,       {0,0} },
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_NOT_CONFIGURED,     {0,0} },
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_INTERFACE_ERROR,      {0,0} },
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_WRITE_ERROR,      {0,0} },
      { EFI_IO_BUS_SMBUS | EFI_IOB_EC_RESOURCE_CONFLICT,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
    
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_DISABLED,        {0,0} },
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_NOT_SUPPORTED,      {0,0} },
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_NOT_DETECTED,         {0,0} },
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_NOT_CONFIGURED,     {0,0} },
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_INTERFACE_ERROR,     {0,0} },
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_CONTROLLER_ERROR,      {0,0} },
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_READ_ERROR,          {0,0} },
//      { EFI_IO_BUS_I2C | EFI_IOB_EC_WRITE_ERROR,          {0,0} },
      { EFI_IO_BUS_I2C | EFI_IOB_EC_RESOURCE_CONFLICT,     {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT_BIT} },
      
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_INVALID_PARAMETER,     {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_UNSUPPORTED,       {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_INVALID_BUFFER,      {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_OUT_OF_RESOURCES,      {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_ABORTED,          {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,{0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,{0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_BAD_DATE_TIME,        {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_CFG_INVALID,          {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_CFG_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_CFG_DEFAULT,          {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_PWD_INVALID,          {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_PWD_CLR_REQUEST,     {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_PWD_CLEARED,          {0,0} },
//      { EFI_SOFTWARE_UNSPECIFIED | EFI_SW_EC_EVENT_LOG_FULL,     {0,0} },

//      { EFI_SOFTWARE_SEC | EFI_SW_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_INVALID_PARAMETER,      {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_UNSUPPORTED,         {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_INVALID_BUFFER,      {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_OUT_OF_RESOURCES,      {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_ABORTED,         {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,{0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,{0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_SEC | EFI_SW_EC_BAD_DATE_TIME,      {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_CFG_INVALID,         {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_CFG_CLR_REQUEST,     {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_CFG_DEFAULT,          {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_PWD_INVALID,          {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_PWD_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_PWD_CLEARED,          {0,0} },
//      { EFI_SOFTWARE_SEC | EFI_SW_EC_EVENT_LOG_FULL,      {0,0} },
      
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_LOAD_ERROR,         {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_INVALID_PARAMETER,      {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_UNSUPPORTED,           {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_INVALID_BUFFER,          {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_OUT_OF_RESOURCES,      {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_ABORTED,             {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_START_ERROR,         {0,0} },
      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_BAD_DATE_TIME,           {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_CFG_INVALID,          {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_CFG_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_CFG_DEFAULT,         {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_PWD_INVALID,         {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_PWD_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_PWD_CLEARED,          {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_EC_EVENT_LOG_FULL,          {0,0} },
//      { EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_CORE_EC_DXE_CORRUPT,      {0,0} },
    
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_NON_SPECIFIC,         {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_INVALID_PARAMETER,      {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_UNSUPPORTED,           {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_INVALID_BUFFER,      {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_OUT_OF_RESOURCES,      {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_ABORTED,         {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE, {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_ILLEGAL_HARDWARE_STATE, {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_BAD_DATE_TIME,       {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_CFG_INVALID,          {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_CFG_CLR_REQUEST,     {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_CFG_DEFAULT,         {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_PWD_INVALID,          {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_PWD_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_PWD_CLEARED,          {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_EC_EVENT_LOG_FULL,      {0,0} },
//      { EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_EC_NO_RECOVERY_CAPSULE,{0,0} },
    
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_INVALID_PARAMETER,      {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_UNSUPPORTED,           {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_INVALID_BUFFER,          {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_OUT_OF_RESOURCES,      {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_ABORTED,          {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_BAD_DATE_TIME,           {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_CFG_INVALID,          {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_CFG_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_CFG_DEFAULT,          {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_PWD_INVALID,          {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_PWD_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_PWD_CLEARED,          {0,0} },
//      { EFI_SOFTWARE_DXE_CORE | EFI_SW_EC_EVENT_LOG_FULL,          {0,0} },  
      
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_INVALID_PARAMETER,      {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_UNSUPPORTED,          {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_INVALID_BUFFER,      {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_OUT_OF_RESOURCES,      {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_ABORTED,          {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,  {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,  {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_BAD_DATE_TIME,       {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_CFG_INVALID,          {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_CFG_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_CFG_DEFAULT,          {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_PWD_INVALID,         {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_PWD_CLR_REQUEST,      {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_PWD_CLEARED,          {0,0} },
//      { EFI_SOFTWARE_EFI_BOOT_SERVICE  | EFI_SW_EC_EVENT_LOG_FULL,      {0,0} },  
      
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_NON_SPECIFIC,      {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_INVALID_PARAMETER,      {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_UNSUPPORTED,          {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_INVALID_BUFFER,      {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_OUT_OF_RESOURCES,      {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_ABORTED,         {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,{0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,{0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_BAD_DATE_TIME,       {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_CFG_INVALID,          {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_CFG_CLR_REQUEST,     {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_CFG_DEFAULT,          {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_PWD_INVALID,          {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_PWD_CLR_REQUEST,     {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_PWD_CLEARED,          {0,0} },
//      { EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_EVENT_LOG_FULL,      {0,0} },
      
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_NON_SPECIFIC,         {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_LOAD_ERROR,             {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_INVALID_PARAMETER,          {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_UNSUPPORTED,           {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_INVALID_BUFFER,          {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_OUT_OF_RESOURCES,          {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_ABORTED,              {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_BAD_DATE_TIME,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_CFG_INVALID,          {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_CFG_CLR_REQUEST,          {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_CFG_DEFAULT,          {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_PWD_INVALID,          {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_PWD_CLR_REQUEST,         {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_PWD_CLEARED,          {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_EVENT_LOG_FULL,         {0,0} },
//      { EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_LEGACY_OPROM_NO_SPACE,{0,0} },
    
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_INVALID_PARAMETER,     {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_UNSUPPORTED,           {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_INVALID_BUFFER,      {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_OUT_OF_RESOURCES,      {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_ABORTED,          {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,    {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,    {0,0} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_BAD_DATE_TIME,        {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_CFG_INVALID,          {0,0} },  
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_CFG_CLR_REQUEST,      {0,0} },  
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_CFG_DEFAULT,         {0,0} },  
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_PWD_INVALID,          {0,0} },  
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_PWD_CLR_REQUEST,      {0,0} },  
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_PWD_CLEARED,          {0,0} },  
//      { EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_EVENT_LOG_FULL,      {0,0} },  
    
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_NON_SPECIFIC,             {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_LOAD_ERROR,              {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_INVALID_PARAMETER,          {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_UNSUPPORTED,              {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_INVALID_BUFFER,         {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_OUT_OF_RESOURCES,         {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_ABORTED,              {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,    {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_START_ERROR,             {0,0} },
      { EFI_SOFTWARE_RT  | EFI_SW_EC_BAD_DATE_TIME,           {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_CFG_INVALID,             {0,0} },  
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_CFG_CLR_REQUEST,          {0,0} },  
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_CFG_DEFAULT,             {0,0} },  
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_PWD_INVALID,              {0,0} },  
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_PWD_CLR_REQUEST,          {0,0} },  
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_PWD_CLEARED,              {0,0} },  
//      { EFI_SOFTWARE_RT  | EFI_SW_EC_EVENT_LOG_FULL,          {0,0} },  
    
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_NON_SPECIFIC,            {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_LOAD_ERROR,              {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_INVALID_PARAMETER,          {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_UNSUPPORTED,           {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_INVALID_BUFFER,         {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_OUT_OF_RESOURCES,          {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_ABORTED,              {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,     {0,0} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_START_ERROR,             {0,0} },
      { EFI_SOFTWARE_AL  | EFI_SW_EC_BAD_DATE_TIME,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_CFG_INVALID,              {0,0} },  
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_CFG_CLR_REQUEST,          {0,0} },  
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_CFG_DEFAULT,             {0,0} },  
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_PWD_INVALID,             {0,0} },  
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_PWD_CLR_REQUEST,          {0,0} },  
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_PWD_CLEARED,              {0,0} },  
//      { EFI_SOFTWARE_AL  | EFI_SW_EC_EVENT_LOG_FULL,          {0,0} },  
    
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_INVALID_PARAMETER,      {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_UNSUPPORTED,           {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_INVALID_BUFFER,          {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_OUT_OF_RESOURCES,         {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_ABORTED,              {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_BAD_DATE_TIME,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_CFG_INVALID,          {0,0} },  
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_CFG_CLR_REQUEST,          {0,0} },  
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_CFG_DEFAULT,          {0,0} },  
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_PWD_INVALID,          {0,0} },  
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_PWD_CLR_REQUEST,          {0,0} },  
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_PWD_CLEARED,          {0,0} },  
//      { EFI_SOFTWARE_EFI_APPLICATION | EFI_SW_EC_EVENT_LOG_FULL,          {0,0} },  
    
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_NON_SPECIFIC,          {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_LOAD_ERROR,          {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_INVALID_PARAMETER,         {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_UNSUPPORTED,           {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_INVALID_BUFFER,          {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_OUT_OF_RESOURCES,          {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_ABORTED,              {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_ILLEGAL_HARDWARE_STATE,      {0,0} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_START_ERROR,          {0,0} },
      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_BAD_DATE_TIME,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET_BIT} },
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_CFG_INVALID,          {0,0} },  
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_CFG_CLR_REQUEST,          {0,0} },  
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_CFG_DEFAULT,          {0,0} },  
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_PWD_INVALID,          {0,0} },  
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_PWD_CLR_REQUEST,          {0,0} },  
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_PWD_CLEARED,          {0,0} },  
//      { EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_EC_EVENT_LOG_FULL,          {0,0} }
#if SMCPKG_SUPPORT
      { DXE_SB_BAD_BATTERY,            {EFI_EVENT_LOG_TYPE_POST_ERROR, EFI_POST_BITMAP_DW1_CMOS_BATTERY_FAILURE_BIT} },
#endif
};

#if OEMRtc_Enabled
//
// Hook list
//
typedef EFI_STATUS (GPNV_RTC_READ_HOOK)(
		EFI_TIME *Time
);
#endif

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
