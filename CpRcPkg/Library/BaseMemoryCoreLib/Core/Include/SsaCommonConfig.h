/**
@copyright
  Copyright (c) 2014-2015 Intel Corporation. All rights reserved.
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

@file
  SsaCommonConfig.h

@brief
  This file contains the definition of the BIOS-SSA Common Configuration API.

  NOTE: This file is only intended to be included from MrcSsaServices.h.  It
  cannot be included separately.

**/
#ifndef _SsaCommonConfig_h_
#define _SsaCommonConfig_h_

#ifdef SSA_FLAG

// BIOS-SSA Common Configuration API revision
#define SSA_REVISION_COMMON ((0x01 << 24) | (0x00 << 16) | (0x00 << 8) | 0x00)

// maximum number of hook handlers
#define MAX_HOOK_HANDLER (10)

#ifndef MINIBIOS_BUILD
#ifdef MRC_SSA_SERVICES_SET
//#include <Library\ProcessorStartup\ProcessorStartup.h>
#include <Library/DebugLib.h>
#endif // MRC_SSA_SERVICES_SET
#endif // MINIBIOS_BUILD

#pragma pack (push, 1)

///
/// Register widths.
///
typedef enum {
  RegWidth8,                              ///< An 8-bit register width is selected.
  RegWidth16,                             ///< A 16-bit register width is selected.
  RegWidth32,                             ///< A 32-bit register width is selected.
  RegWidth64,                             ///< A 64-bit register width is selected.
  RegWidthMax,                            ///< REG_WIDTH enumeration maximum value.
  RegWidthDelim = INT32_MAX               ///< This value ensures the enum size is consistent on both sides of the PPI.
} REG_WIDTH;

///
/// Base address types.
///
typedef enum {
  MCH_BAR,                                ///< MCHBAR base address selection.
  BaseAddrTypeMax,                        ///< BASE_ADDR_TYPE enumeration maximum value.
  BaseAddrTypeDelim = INT32_MAX           ///< This value ensures the enum size is consistent on both sides of the PPI.
} BASE_ADDR_TYPE;

///
/// Memory Mapped I/O selectors.
///
typedef union {
  UINT64 Data64;                          ///< 64-bit MMIO buffer.
  UINT32 Data32;                          ///< 32-bit MMIO buffer.
  UINT16 Data16;                          ///< 16-bit MMIO buffer.
  UINT8  Data8;                           ///< 8-bit MMIO buffer.
} MMIO_BUFFER;

///
/// Port I/O selectors.
///
typedef union {
  UINT32 Data32;                          ///< 32-bit I/O buffer.
  UINT16 Data16;                          ///< 16-bit I/O buffer.
  UINT8  Data8;                           ///< 8-bit I/O buffer.
} IO_BUFFER;

///
/// PCI buffer sizes.
///
typedef union {
  UINT32 Data32;                          ///< 32-bit PCI buffer.
  UINT16 Data16;                          ///< 16-bit PCI buffer.
  UINT8  Data8;                           ///< 8-bit PCI buffer.
} PCI_BUFFER;

///
/// PCI configuration space bit definitions.
///
typedef union {
  struct {
    UINT32 Reserved0 : 2;           ///< PCI address pointer reserved value, range 0 to 3.
    UINT32 Offset    : 6;           ///< PCI address pointer offset value, range 0 to 63.
    UINT32 Function  : 3;           ///< PCI address pointer function value, range 0 to 7.
    UINT32 Device    : 5;           ///< PCI address pointer device value, range 0 to 31.
    UINT32 Bus       : 8;           ///< PCI address pointer bus value, range 0 to 255.
    UINT32 Reserved1 : 7;           ///< PCI address pointer reserved value, range 0 to 127.
    UINT32 Enable    : 1;           ///< PCI address pointer enable flag, 0 = disable, 1 = enable.
  } Bits;
  UINT32 Value;
} PCI_CONFIG_SPACE;

///
/// Debug print output level selectors.
///
typedef enum {
  SSA_D_WARN      = DEBUG_WARN,           ///< Warnings
  SSA_D_LOAD      = DEBUG_LOAD,           ///< Load events
  SSA_D_INFO      = DEBUG_INFO,           ///< Informational debug messages
  SSA_D_EVENT     = DEBUG_EVENT,          ///< Event messages
  SSA_D_ERROR     = DEBUG_ERROR,          ///< Error
  PrintLevelMax,                          ///< PRINT_LEVEL enumeration maximum value.
  PrintLevelDelim = INT32_MAX             ///< This value ensures the enum size is consistent on both sides of the PPI.
} PRINT_LEVEL;

/**
  BIOS hook handler function.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Parameters - Data passed from the invoker of the hook.

  @retval None
**/
typedef
VOID
(EFIAPI * SSA_BIOS_HOOK_HANDLER) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN VOID                      *Parameters
  );

// {ADF0A27B-61A6-4F18-9EAC-4687E79E6FBB}
#define SSA_POSTCODE_HOOK_GUID {0xADF0A27B, 0x61A6, 0x4F18, {0x9E, 0xAC, 0x46, 0x87, 0xE7, 0x9E, 0x6F, 0xBB}}

///
/// SSA post code hook data. For the post code hook guid type, the argument parameters is no longer optional and will be a pointer to a structure of type SSA_POSTCODE_HOOK_DATA.
///
typedef struct {
  UINT32 Postcode;      ///< PostCode value used to identify the current checkpoint. This is the same value shown on the postcode display.
  VOID   *PostcodeData; ///< Data passed from the caller at the postcode. It can be NULL if no data is expected at a given postcode.
} SSA_POSTCODE_HOOK_DATA;

/**
  Reads a variable-sized value from a memory mapped register using an absolute address.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width   - The size of the value to write.
  @param[in]      Address - Address of the register to be accessed.
  @param[out]     Buffer  - Value storage location.

  @retval None
**/
typedef
VOID
(EFIAPI * READ_MEM) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN REG_WIDTH                 Width,
  IN EFI_PHYSICAL_ADDRESS      Address,
  OUT VOID                     *Buffer
  );

/**
  Writes a variable sized value to a memory mapped register using an absolute address.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width   - The size of the value to write.
  @param[in]      Address - Address of the register to be accessed.
  @param[in]      Buffer  - Value to write.

  @retval None
**/
typedef
VOID
(EFIAPI * WRITE_MEM) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN REG_WIDTH                 Width,
  IN EFI_PHYSICAL_ADDRESS      Address,
  IN VOID                      *Buffer
  );

/**
  Reads a variable sized value from I/O.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width   - The size of the value to write.
  @param[in]      Address - Address of the I/O to be accessed.
  @param[out]     Buffer  - Value storage location.

  @retval None
**/
typedef
VOID
(EFIAPI * READ_IO) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN REG_WIDTH                 Width,
  IN UINT32                    Address,
  OUT VOID                     *Buffer
  );

/**
  Writes a variable sized value to I/O.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width   - The size of the value to write.
  @param[in]      Address - Address of the I/O to be accessed.
  @param[in]      Buffer  - Value to write.

  @retval None
**/
typedef
VOID
(EFIAPI * WRITE_IO) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN REG_WIDTH                 Width,
  IN UINT32                    Address,
  IN VOID                      *Buffer
  );

/**
  Reads a variable sized value from the PCI config space register.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width      - The size of the value to write.
  @param[in]      Address    - Address of the I/O to be accessed. Must be modulo 'Width'.
  @param[out]     Buffer     - Value storage location.

  @retval None
**/
typedef
VOID
(EFIAPI * READ_PCI) (
  IN OUT SSA_BIOS_SERVICES_PPI       *This,
  IN REG_WIDTH                       Width,
  IN EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS *Address,
  OUT VOID                           *Buffer
  );

/**
  Writes a variable sized value to the PCI config space register.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width      - The size of the value to write.
  @param[in]      Address    - Address of the I/O to be accessed. Must be modulo 'Width'.
  @param[in]      Buffer     - Value to write.

  @retval None
**/
typedef
VOID
(EFIAPI * WRITE_PCI) (
  IN OUT SSA_BIOS_SERVICES_PPI       *This,
  IN REG_WIDTH                       Width,
  IN EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS *Address,
  IN VOID                            *Buffer
  );

/**
  Gets a base address to be used in the different memory map or MMIO register access functions.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Index           - Additional index to locate the register.
  @param[in]      BaseAddressType - Value that indicates the type of base address to be retrieved.
  @param[out]     BaseAddress     - Where to write the base address

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if BaseAddressType is out of range.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * GET_BASE_ADDRESS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Index,
  IN BASE_ADDR_TYPE            BaseAddressType,
  OUT EFI_PHYSICAL_ADDRESS     *BaseAddress
  );

/**
  Function used to dynamically allocate memory.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Size - Amount of memory in bytes to allocate.

  @retval Returns a pointer to an allocated memory block on success or NULL on failure.
**/
typedef
VOID *
(EFIAPI * MALLOC) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT32                    Size
  );

/**
  Function used to release memory allocated using Malloc.

  @param[in, out] This   - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Buffer - The buffer to return to the free pool.

  @retval None
**/
typedef
VOID
(EFIAPI * FREE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN VOID                      *Buffer
  );

/**
  Function used to output debug messages to the output logging device.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      PrintLevel   - The severity level of the string.
  @param[in]      FormatString - The reduced set of printf style format specifiers.
  @param          %[flags][width]type
  @param          [flags] '-' left align
  @param          [flags] '+' prefix with sign (+ or -)
  @param          [flags] '0' zero pad numbers
  @param          [flags] ' ' prefix blank in front of positive numbers
  @param          [width] non negative decimal integer that specifies the width to print a value.
  @param          [width] '*' get the width from a int argument on the stack.
  @param          type    'd'|'i' signed decimal integer
  @param          type    'u' unsigned integer
  @param          type    'x'|'X' hexadecimal using "ABCDEF"
  @param          type    'c' print character
  @param          type    'p' print a pointer to void
  @param          type    's' print a null terminated string
  @param[in]      ...          - Variable list of output values.

  @retval None
**/
typedef
VOID
(EFIAPI * DEBUG_PRINT) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN PRINT_LEVEL               PrintLevel,
  IN UINT8                     *FormatString,
  ...
  );

/**
  Function used to stall the software progress for the given period of time.

  @param[in, out] This  - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Delay - The amount of time to wait, in microseconds.

  @retval None
**/
typedef
VOID
(EFIAPI * WAIT) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT32                    Delay
  );

/**
  Function used to add data to the BDAT v4 or later structure that will be created by BIOS in reserved physical memory.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      SchemaGuid - The Schema ID GUID.  This is a unique identifier that indicates the format of the data to be added to the BDAT structure.
  @param[in]      Data       - The location of the data block to add to the BDAT structure.
  @param[in]      DataSize   - The size of the data to be added to the BDAT structure in bytes.

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if SchemaGuid is NULL or invalid.  Else UnsupportedValue if Data is NULL or DataSize is 0.  Else NotAvailable if there is insufficient memory to add the data to the structure.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * SAVE_TO_BDAT) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN EFI_GUID                  *SchemaGuid,
  IN CONST UINT8               *Data,
  IN UINT32                    DataSize
  );

/**
  This function to register a new handler for a hook.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      HookGuid    - GUID defining the hook to invoke the handlers for.
  @param[in]      HookHandler - Function pointer to the new hook handler. This hook handler will be invoked whenever a corresponding call to RunHook() is made with the correct hook GUID.

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if HookGuid or HookHandler is NULL or HookGuid is invalid.  Else Failure if unable to allocate a buffer to store the hook.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * REGISTER_HOOK_HANDLER) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN VOID                      *HookGuid,
  IN SSA_BIOS_HOOK_HANDLER     HookHandler
  );

/**
  This function invokes all registered handlers for a specific hook.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      HookGuid   - GUID defining the hook to invoke the handlers for.
  @param[in]      Parameters - Data passed to the hook handlers. The type and format of this data differs depending on what hook is being invoked. The type and format of the data must be the same as all other cases where RunHook() is called with a specific hook GUID. This value can be NULL if the hook does not pass any additional data.

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if HookGuid is NULL or invalid.  Else Success
**/
typedef
SSA_STATUS
(EFIAPI * RUN_HOOK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN VOID                      *HookGuid,
  IN VOID                      *Parameters
  );

/**
  This function saves a pointer into a table that can later be retrieved.

  @param[in, out] This      - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      PointerId - GUID that uniquely identifies the pointer.
  @param[in]      Pointer   - The pointer to save.

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if PointerId or Pointer is NULL or PointerId is invalid.  Else Failure if unable to allocate a buffer to store the pointer.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * SAVE_DATA_POINTER) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN EFI_GUID                  *PointerId,
  IN VOID                      *Pointer
  );

/**
  This function returns a pointer that was earlier saved using SaveDataPointer function.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      PointerId   - GUID that uniquely identifies the pointer.

  @retval The value of the pointer or NULL if the pointer for the specified GUID does not exist.
**/
typedef
VOID *
(EFIAPI * RESTORE_DATA_POINTER) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN EFI_GUID                  *PointerId
  );

/**
  Function used to disable the watchdog timer.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.

  @retval Success when the watchdog timer is successfully disabled or if the platform does not have a watchdog timer.  Else Failure.
**/
typedef
VOID
(EFIAPI * DISABLE_WATCHDOG_TIMER) (
  IN OUT SSA_BIOS_SERVICES_PPI *This
  );

/**
  Function used to get the elapsed time since the last CPU reset in milliseconds.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Timestamp - Pointer to were the elapsed time since the last CPU reset in milliseconds will be written.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_TIMESTAMP) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT UINT32                   *Timestamp
  );

///
/// BIOS-SSA Common Configuration
///
typedef struct _SSA_COMMON_CONFIG {
  UINT32                 Revision;             ///< Incremented when a backwards compatible binary change is made to the PPI.
  VOID                   *BiosData;            ///< Pointer to the BIOS data buffer.
  VOID                   *HookBuffer;          ///< Starting pointer to the hook handler buffer.
  READ_MEM               ReadMem;              ///< Reads a variable-sized value from a memory mapped register using an absolute address. This function takes advantage of any caching implemented by BIOS.
  WRITE_MEM              WriteMem;             ///< Writes a variable-sized value to a memory mapped register using an absolute address. This function takes advantage of any caching implemented by BIOS.
  READ_IO                ReadIo;               ///< Reads a variable-sized value from IO. This function takes advantage of any caching implemented by BIOS.
  WRITE_IO               WriteIo;              ///< Writes a variable-sized value to IO.
  READ_PCI               ReadPci;              ///< Reads a variable-sized value from a PCI config space register. This function takes advantage of any caching implemented by BIOS.
  WRITE_PCI              WritePci;             ///< Writes a variable-sized value to a PCI config space register. This function takes advantage of any caching implemented by BIOS.
  GET_BASE_ADDRESS       GetBaseAddress;       ///< Gets a base address to be used in the different memory map or IO register access functions.
  MALLOC                 Malloc;               ///< Function used to dynamically allocate memory.
  FREE                   Free;                 ///< Function used to release memory allocated using Malloc.
  PRINT_LEVEL            CurrentPrintLevel;    ///< Variable used to determine if a print done using DebugPrint should be shown in the system.
  DEBUG_PRINT            DebugPrint;           ///< Function used to output debug messages to the output logging device.
  WAIT                   Wait;                 ///< Function used to stall the software progress for the given period of time.
  SAVE_TO_BDAT           SaveToBdat;           ///< Function used to add data to the BDAT v4 or later structure that will be created by BIOS in reserved physical memory.
  REGISTER_HOOK_HANDLER  RegisterHookHandler;  ///< This function to register a new handler for a hook.
  RUN_HOOK               RunHook;              ///< This function invokes all registered handlers for a specific hook.
  SAVE_DATA_POINTER      SaveDataPointer;      ///< Saves a pointer into a table that can later be retrieved.
  RESTORE_DATA_POINTER   RestoreDataPointer;   ///< Restores a pointer that was earlier saved using SaveDataPointer function.
  DISABLE_WATCHDOG_TIMER DisableWatchdogTimer; ///< Function used to disable the watchdog timer.
  GET_TIMESTAMP          GetTimestamp;         ///< Function used to get the elapsed time since the last CPU reset in milliseconds.
} SSA_COMMON_CONFIG;

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // _SsaCommonConfig_h_
