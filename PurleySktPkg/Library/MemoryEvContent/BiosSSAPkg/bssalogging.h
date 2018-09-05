/*++
Description:
  BIOS SSA logging functions macros.
--*/
#ifndef __BSSA_LOGGING_H__
#define __BSSA_LOGGING_H__

#ifdef SSA_FLAG
#include "MrcSsaServices.h"
#include "PlatformHost.h"

#define BSSA_LOGGING_INTERNAL_DEBUG_LEVEL  50
#define BSSA_LOGGING_INTERNAL_NORMAL_LEVEL 25
#define BSSA_LOGGING_NORMAL_LEVEL          0

#ifndef BSSA_LOGGING_LEVEL
	#define BSSA_LOGGING_LEVEL BSSA_LOGGING_INTERNAL_NORMAL_LEVEL
#endif

//PrintLevels
#define BSSA_LOGGING_WARNING SSA_D_WARN  ///< Warnings
#define BSSA_LOGGING_LOAD    SSA_D_LOAD  ///< Load events
#define BSSA_LOGGING_INFO    SSA_D_INFO  ///< Informational debug messages
#define BSSA_LOGGING_EVENT   SSA_D_EVENT ///< Event messages
#define BSSA_LOGGING_ERROR   SSA_D_ERROR ///< Error

/*
Function used to change the log print level. This is for internal testing purposes only.

Parameters:
- SSA_BIOS_SERVICES_PPI *SsaBiosServices: Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
- PRINT_LEVEL newPrintLevel: Enum used to determine if the message should be output to debug print device.

Function doesn't return anything.
*/
#define BSSA_LOGGING_CHANGE_LEVEL(SsaBiosServices, NewPrintLevel) (SsaBiosServices)->SsaCommonConfig->CurrentPrintLevel = (NewPrintLevel)

#ifndef BSSA_LOGGING_SILENT_MODE
/*
Function used to log messages. Will be defined on all three logging levels(BSSA_LOGGING_INTERNAL_DEBUG_LEVEL, 
BSSA_LOGGING_INTERNAL_NORMAL_LEVEL or BSSA_LOGGING_NORMAL_LEVEL). 

Parameters:
- SSA_BIOS_SERVICES_PPI *SsaBiosServices: Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
- PRINT_LEVEL PrintLevel: Enum used to determine if the message should be output to debug print device.
- CHAR* formatString: format string for the message to be output to debug print device.

Function doesn't return anything.
*/
#if BSSA_LOGGING_LEVEL >= BSSA_LOGGING_INTERNAL_DEBUG_LEVEL
#define INTERNAL_DEBUG_LOG(SsaBiosServices, PrintLevel, formatString, ...) (SsaBiosServices)->SsaCommonConfig->DebugPrint((SsaBiosServices), (PrintLevel), (formatString), __VA_ARGS__)
#else
#define INTERNAL_DEBUG_LOG(SsaBiosServices, PrintLevel, formatString, ...)
#endif
/*
Function used to log messages. Will be defined on BSSA_LOGGING_INTERNAL_NORMAL_LEVEL and BSSA_LOGGING_NORMAL_LEVEL

Parameters:
- SSA_BIOS_SERVICES_PPI *SsaBiosServices: Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
- PRINT_LEVEL PrintLevel: Enum used to determine if the message should be output to debug print device.
- CHAR* formatString: format string for the message to be output to debug print device.

Function doesn't return anything.
*/
#if BSSA_LOGGING_LEVEL >= BSSA_LOGGING_INTERNAL_NORMAL_LEVEL
#define INTERNAL_NORMAL_LOG(SsaBiosServices, PrintLevel, formatString, ...) (SsaBiosServices)->SsaCommonConfig->DebugPrint((SsaBiosServices), (PrintLevel), (formatString), __VA_ARGS__)
#else
#define INTERNAL_NORMAL_LOG(SsaBiosServices, PrintLevel, formatString, ...)
#endif
/*
Function used to log messages. Will only be defined on BSSA_LOGGING_NORMAL_LEVEL

Parameters:
- SSA_BIOS_SERVICES_PPI *SsaBiosServices: Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
- PRINT_LEVEL PrintLevel: Enum used to determine if the message should be output to debug print device.
- CHAR* formatString: format string for the message to be output to debug print device.

Function doesn't return anything.
*/
#ifdef SERIAL_DBG_MSG
#if BSSA_LOGGING_LEVEL >= BSSA_LOGGING_NORMAL_LEVEL
#define NORMAL_LOG(SsaBiosServices, PrintLevel, formatString, ...) (SsaBiosServices)->SsaCommonConfig->DebugPrint((SsaBiosServices), (PrintLevel), ((UINT8 *)(formatString)), ##__VA_ARGS__)
#else
#define NORMAL_LOG(SsaBiosServices, PrintLevel, formatString, ...)
#endif
#else
#define NORMAL_LOG(SsaBiosServices, PrintLevel, formatString, ...)
#endif
#else
#define INTERNAL_DEBUG_LOG(SsaBiosServices, PrintLevel, formatString, ...)
#define INTERNAL_NORMAL_LOG(SsaBiosServices, PrintLevel, formatString, ...)
#define NORMAL_LOG(SsaBiosServices, PrintLevel, formatString, ...)
#endif

#endif //SSA_FLAG
#endif
