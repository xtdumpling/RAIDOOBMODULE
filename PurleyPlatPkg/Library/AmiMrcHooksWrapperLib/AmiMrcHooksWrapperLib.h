//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiMrcHooksWrapperLib.h
    MRC Hooks wrapper methods header
**/

#ifndef __AMI_MRC_HOOKS_WRAPPER_LIB_H__
#define __AMI_MRC_HOOKS_WRAPPER_LIB_H__

#include "SysFunc.h"
#include <Ppi/ReadOnlyVariable2.h>
#include <PlatformHooksElinkAutoGen.h>
#include "CoreApi.h"

VOID
AmiPlatformLogWarningWrapper (
   PSYSHOST host,
   UINT8    majorCode,
   UINT8    minorCode,
   UINT32   logData
   );
 
VOID
 AmiPlatformInitializeDataWrapper (
   PSYSHOST host,
   UINT8 socket
   );

VOID
 AmiOemInitializePlatformData (
	struct sysHost *host
    );

VOID
 AmiOemLookupFreqTableWrapper (
   PSYSHOST host,
   struct DimmPOREntryStruct **freqTablePtr,
   UINT16 *freqTableLength
   );
 
 VOID
 AmiOemKtiChangeParameterWrapper (
   IN OUT struct sysHost *host,
   IN KTI_SOCKET_DATA *SocketData,
   IN KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
   );

 VOID
 AmiPlatformFatalErrorWrapper (
   IN PSYSHOST host,
   IN UINT8    majorCode,
   IN UINT8    minorCode,
   IN UINT8    *haltOnError
   );
 
 VOID
 AmiKtiLogWarningWrapper (
   struct sysHost             *host,
   UINT8                      SocId,
   UINT8                      SocType,
   UINT8                      KtiLink,
   UINT8                      WarnCode
   ); 
 
 VOID
 AmiOemPublishHobDataWrapper(
    struct sysHost             *host
    );

 VOID
 OemKtiGetEParamsOverride(
    struct sysHost             *host,
    ALL_LANES_EPARAM_LINK_INFO **Ptr,
    UINT32                     *SizeOfTable
    );

 VOID
 OemGetBoardTypeBitmaskOverride(
    struct sysHost             *host,
    UINT32                     *BoardTypeBitmask
    );

 VOID
 AmiKtiCheckPointWrapper (
   UINT8                      SocId,
   UINT8                      SocType,
   UINT8                      KtiLink,
   UINT8                      MajorCode,
   UINT8                      MinorCode,
   struct sysHost             *host
   );
 
 
 VOID
 AmiPlatformOutputErrorWrapper (
   PSYSHOST host,
   UINT8    ErrorCode,
   UINT8    minorErrorCode,
   UINT8    socket,
   UINT8    ch,
   UINT8    dimm,
   UINT8    rank
   );

 VOID
 AmiPlatformOutputWarningWrapper (
   PSYSHOST host,
   UINT8    warningCode,
   UINT8    minorWarningCode,
   UINT8    socket,
   UINT8    ch,
   UINT8    dimm,
   UINT8    rank
   );

 VOID
 OemGetResourceMapUpdateOverride(
   IN EFI_STATUS              Status,
   IN struct sysHost          *host,
   IN EFI_PEI_READ_ONLY_VARIABLE2_PPI   *PeiVariable,
   IN OUT KTI_CPU_RESOURCE    *CpuRes,
   IN KTI_OEMCALLOUT          SystemParams
   );
 
  VOID
  AmiTurnAroundTimingsWrapper(
     PSYSHOST host,
     UINT8    socket		
     );

#endif
 
 //**********************************************************************
 //**********************************************************************
 //**                                                                  **
 //**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
 //**                                                                  **
 //**                       All Rights Reserved.                       **
 //**                                                                  **
 //**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
 //**                                                                  **
 //**                       Phone: (770)-246-8600                      **
 //**                                                                  **
 //**********************************************************************
 //**********************************************************************
