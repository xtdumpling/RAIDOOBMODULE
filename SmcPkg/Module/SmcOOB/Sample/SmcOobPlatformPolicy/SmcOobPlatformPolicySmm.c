//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Add a SMM Oob Library interface. 
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Feb/12/2018
//
//  Rev. 1.00
//    Bug fixed: Initial SMC_OOB_PLATFORM_POLICY for SMC OOB module binary release.
//    Reason   : 
//    Auditor  : Jason Hsu
//    Date     : Jun/09/07
//
//****************************************************************************

#include <TOKEN.h>
#include <Protocol/SmcRomHoleProtocol.h>
#include <SmcRomHole.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/SmcOobLibraryProtocol.h>
#include "OobLibrary.h"
#include "SmcFeatureFlag.h"

EFI_STATUS
SmmRomHoleWriteRegion (
  IN  UINT8                             Operation,
  IN  UINT8                             *pBuffer
  )
{
  EFI_STATUS Status;

  if (pBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  Status = SmcRomHoleWriteRegion (Operation, pBuffer);

  return Status;
}

EFI_STATUS
SmmRomHoleReadRegion (
  IN  UINT8                             Operation,
  OUT UINT8                             *pBuffer
  )
{
  EFI_STATUS Status;

  if (pBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SmcRomHoleReadRegion (Operation, pBuffer);

  return Status;
}


UINT32
SmmRomHoleSize (
  IN  UINT8                             Operation
  )
{
  return SmcRomHoleSize (Operation);
}


UINT32
SmmRomHoleOffset (
  IN  UINT8                             Operation
  )
{
  return SmcRomHoleOffset (Operation);
}

SMC_SMM_ROM_HOLE_PROTOCOL             mSmcSmmRomHoleProtocol = {
  SMC_ROM_HOLE_PROTOCOL_REVISION_1,
  0,
  SmmRomHoleWriteRegion,
  SmmRomHoleReadRegion,
  SmmRomHoleSize,
  SmmRomHoleOffset
};
SMC_SMM_OOB_LIBRARY_PROTOCOL		 mSmcSmmOobLibraryProtocol = {
  GenerateFeatureSupportFlags,
  NULL,
  NULL,
  NULL,
  OobGetCmosTokensValue,
  OobSetCmosTokensValue

};

EFI_STATUS InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                    Status;
    EFI_HANDLE                   NewHandle;
	
	DEBUG((-1,"OOB PlatformPolicy SMM InSmmFunction!\n"));

    Status = InitAmiSmmLib (ImageHandle, SystemTable);
    if (EFI_ERROR(Status)) return Status;    

	NewHandle = NULL;

	Status = pSmst->SmmInstallProtocolInterface (
                  &NewHandle,
                  &gSmcSmmRomHoleProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSmcSmmRomHoleProtocol
                  );

	DEBUG((-1," Install Status = %r\n",Status));


	NewHandle = NULL;

	Status = pSmst->SmmInstallProtocolInterface (
                  &NewHandle,
                  &gSmcSmmOobLibraryProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSmcSmmOobLibraryProtocol
                  );

	DEBUG((-1," Install Status = %r\n",Status));

    return EFI_SUCCESS;
}

EFI_STATUS SmcOobPlatformPolicySmmEntry (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib(ImageHandle, SystemTable);

	DEBUG((-1,"SmcOobPlatformPolicySmmEntry Enter!\n"));

    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}


