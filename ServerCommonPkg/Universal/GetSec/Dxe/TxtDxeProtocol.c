/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

  Copyright (c) 2007 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


    @file TxtDxeProtocol.c

**/

#include "TxtDxeLib.h"
#include "TxtDxeProtocol.h"

//----------------------------------------------------------------------------
extern	LT_DXE_LIB_CONTEXT          mLtDxeCtx;

//----------------------------------------------------------------------------
///////////////////////	TXT_DXE_PROTOCOL /////////////////////////////////////
//----------------------------------------------------------------------------
//
TXT_DXE_PROTOCOL		mTxtDxeProtocol;	// TXT DXE Protocol
//
//----------------------------------------------------------------------------
/**
  BiosAcResetEstablishmentBit(): Reset Establishment Bit.

  @param[in] None

  @retval EFI_SUCCESS            Successful
  @retval Other                  Error

**/
//----------------------------------------------------------------------------
EFI_STATUS
BiosAcResetEstablishmentBit (
  )
{
  return (ResetTpmEstBit (&mLtDxeCtx));
}

//----------------------------------------------------------------------------
/**
  BiosAcResetAuxIndex(): Reset Aux Index.

  @param[in] None

  @retval EFI_SUCCESS            Successful
  @retval Other                  Error

**/
//----------------------------------------------------------------------------
EFI_STATUS
BiosAcResetAuxIndex (
  )
{
  return (ClearTpmAuxIndex (&mLtDxeCtx));
}

//----------------------------------------------------------------------------
/**
  InstallTxtDxeProtocol(): Install TXT DXE PROTOCOL.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS            Successful
  @retval Other                  Error

**/
//----------------------------------------------------------------------------
EFI_STATUS
InstallTxtDxeProtocol (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  // Local data
  EFI_STATUS		Status;
  TXT_DXE_PROTOCOL	*Tdp;

  // Check if TXT_DXE_PROTOCOL is already installed
  Tdp = NULL;
  Status = SystemTable->BootServices->LocateProtocol (			// Locate protocol
		&gTxtDxeProtocolGuid,
		NULL,
		(VOID**) &Tdp);
  if (Status == EFI_SUCCESS) { return EFI_SUCCESS; }			// If already installed -> Success
  if (Tdp != NULL) { return EFI_DEVICE_ERROR; }				// If not installed but protocol non-NULL -> Error

  // Init and Install TXT DXE Protocol
  mTxtDxeProtocol.BiosAcResetEstablishmentBit =	BiosAcResetEstablishmentBit; // Ptr to USB_CORE_INFO_STRUC
  mTxtDxeProtocol.BiosAcResetAuxIndex = BiosAcResetAuxIndex;		// Size of available USB Core Information
  Status = SystemTable->BootServices->InstallProtocolInterface (
		&ImageHandle,
		&gTxtDxeProtocolGuid,
		EFI_NATIVE_INTERFACE,
		&mTxtDxeProtocol);
  if (Status != EFI_SUCCESS) { return Status; }				// If not successful -> Error

  // Ensure protocol is installed
  Tdp = NULL;
  Status = SystemTable->BootServices->LocateProtocol (			// Locate protocol
		&gTxtDxeProtocolGuid,
		NULL,
		(VOID**) &Tdp);
  if (Status == EFI_SUCCESS) {						// If success
    if (Tdp != &mTxtDxeProtocol) { return EFI_DEVICE_ERROR; }		//   If not match -> Error
  }

  return Status;							// Status
}

//----------------------------------------------------------------------------

