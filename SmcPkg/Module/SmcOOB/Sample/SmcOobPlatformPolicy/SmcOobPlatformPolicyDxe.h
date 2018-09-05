//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//  Rev. 1.02
//    Bug Fix : N/A
//    Reason  : Create a signal guid for OOB use after BDS connect all driver,
//    			since original guid not all platform signaled.
//    Auditor : Durant Lin
//    Date    : Jan/04/2018
//
//  Rev. 1.01
//    Bug Fix : N/A
//    Reason  : Implement a SmcOobProtocol Interface for InBand and OutBand to
//              access Build time OobLibrary not OBJ OobLobrary. 
//    Auditor : Durant Lin
//    Date    : Dec/27/2017
//
//  Rev. 1.00
//    Bug fixed: Initial SMC_OOB_PLATFORM_POLICY for SMC OOB module binary release.
//    Reason   : 
//    Auditor  : Jason Hsu
//    Date     : Jun/06/09
//
//****************************************************************************

#ifndef _SMC_OOB_PLATFORM_POLICY_DXE_H_
#define _SMC_OOB_PLATFORM_POLICY_DXE_H_

#include <Guid/SmcOobPlatformPolicy.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Protocol/SmcIpmiOemCommandSetProtocol.h>
#include <Protocol/SmcRomHoleProtocol.h>

#define OOB_SIGNAL_EVENT_AFTER_BDS_CONNECT \
  {0x1A88a3b3, 0xBA70, 0x18A7, {0xAC, 0x55, 0x9a, 0x9c, 0xba, 0x9c, 0x9c, 0xa9}}

extern EFI_GUID gOobSignalEventAfterBdsConnect;

EFI_STATUS
EFIAPI
SmcDownloadOobDataStart (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT32                            *FileSize,
  OUT UINT8                             *ResponseStatus
  );

EFI_STATUS
EFIAPI
SmcDownloadOobDataReady (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT8                             *ResponseStatus
  );

EFI_STATUS
EFIAPI
SmcDownloadOobDataDone (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  IN  UINT8                             UpdateStatus
  );

EFI_STATUS
EFIAPI
SmcUploadOobDataStart (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  IN  UINT32                            FileSize,
  OUT UINT8                             *ResponseStatus
  );

EFI_STATUS
EFIAPI
SmcUploadOobDataDone (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT8                             *ResponseStatus
  );

EFI_STATUS
EFIAPI
SmcReadDataToBios (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  OUT UINT8                             *Buffer,
  IN  UINT32                            BufferLength
  );

EFI_STATUS
EFIAPI
SmcPrepareDataToBmc (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             *Buffer,
  IN  UINT32                            BufferLength
  );

EFI_STATUS
EFIAPI
SmcGetSetOobIdentify (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT32                            BoardId,
  IN  UINT32                            Date,
  IN  UINT16                            Time
  );

EFI_STATUS
EFIAPI
SmcSmibiosPreservation (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             Operation,
  IN  UINT8                             FlashFlag,
  OUT UINT8                             *ResponseStatus
  );

EFI_STATUS
RomHoleWriteRegion (
  IN  UINT8                             Operation,
  IN  UINT8                             *pBuffer
  );

EFI_STATUS
RomHoleReadRegion (
  IN  UINT8                             Operation,
  OUT UINT8                             *pBuffer
  );

UINT32
RomHoleSize (
  IN  UINT8                             Operation
  );

UINT32
RomHoleOffset (
  IN  UINT8                             Operation
  );

VOID
EFIAPI
UpdateOobRefVariables (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );


VOID
InstallSmcOobLibraryProtocolInterface();

#endif

