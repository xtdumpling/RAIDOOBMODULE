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
/** @file
  Header file for DXE_PERFORMANCE protocol
*/

#ifndef _EFI_PERFORMANCE_H_
#define _EFI_PERFORMANCE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>

#define EFI_PERFORMANCE_PROTOCOL_GUID \
  { 0xFFECFFFF, 0x923C, 0x14d2, 0x9E, 0x3F, 0x22, 0xA0, 0xC9, 0x69, 0x56, 0x3B }

#define EFI_PERF_TOKEN_LENGTH        32
#define EFI_PERF_HOST_LENGTH         32
#define EFI_PERF_PDBFILENAME_LENGTH  40

typedef struct _EFI_PERFORMANCE_PROTOCOL EFI_PERFORMANCE_PROTOCOL;

typedef struct {
  EFI_HANDLE        Handle; 
  UINT16            Token[EFI_PERF_TOKEN_LENGTH];
  UINT16            Host[EFI_PERF_HOST_LENGTH];
  UINT64            StartTick;
  UINT64            EndTick;
  EFI_GUID          GuidName;
  UINT8             PdbFileName[EFI_PERF_PDBFILENAME_LENGTH];
  UINT8             Phase;
} EFI_GAUGE_DATA ;


typedef 
EFI_STATUS
(EFIAPI * EFI_PERF_START_GAUGE) (
  IN EFI_PERFORMANCE_PROTOCOL *This,
  IN EFI_HANDLE        Handle,
  IN UINT16           *Token,
  IN UINT16           *Host,
  IN UINT64           Ticker
  );

typedef 
EFI_STATUS
(EFIAPI * EFI_PERF_END_GAUGE) (
  IN EFI_PERFORMANCE_PROTOCOL *This,
  IN EFI_HANDLE        Handle,
  IN UINT16           *Token,
  IN UINT16           *Host,
  IN UINT64           Ticker
  );


typedef 
EFI_GAUGE_DATA *
(EFIAPI * EFI_PERF_GET_GAUGE) (
  IN EFI_PERFORMANCE_PROTOCOL *This,
  IN EFI_HANDLE                       Handle,     
  IN UINT16                           *Token,     
  IN UINT16                           *Host,      
  IN EFI_GAUGE_DATA                   *PrevGauge      
  );


struct _EFI_PERFORMANCE_PROTOCOL{
  EFI_PERF_START_GAUGE  StartGauge;
  EFI_PERF_END_GAUGE    EndGauge;
  EFI_PERF_GET_GAUGE    GetGauge;
}; // EFI_PERFORMANCE_PROTOCOL;

GUID_VARIABLE_DECLARATION(gEfiPerformanceProtocolGuid, EFI_PERFORMANCE_PROTOCOL_GUID);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
