/**
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
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


  @file TxtDxeProtocol.h

  This file contains definitions so that the functions in TXT DXE
  Driver can be used by other modules.

**/

#ifndef _TXT_DXE_PROTOCOL_H_
#define _TXT_DXE_PROTOCOL_H_

#include <Uefi.h>

//----------------------------------------------------------------------------------------------
#pragma pack (1)
//
EFI_STATUS
InstallTxtDxeProtocol (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );
//
//----------------------------------------------------------------------------------------------
// TXT_DXE_PROTOCOL_RESET_ESTABLISHMENT
// This function resets establishment bit.
// Arguments:	None
// Result:	EFI_SUCCESS		Success
//		Other			Error
//----------------------------------------------------------------------------------------------
//
typedef
EFI_STATUS
(EFIAPI *TXT_DXE_PROTOCOL_RESET_ESTABLISHMENT)(
  );
//
//----------------------------------------------------------------------------------------------
// TXT_DXE_PROTOCOL_RESET_AUX_INDEX
// This function resets aux index.
// Arguments:	None
// Result:	EFI_SUCCESS		Success
//		Other			Error
//----------------------------------------------------------------------------------------------
//
typedef
EFI_STATUS
(EFIAPI *TXT_DXE_PROTOCOL_RESET_AUX_INDEX)(
  );
//
//----------------------------------------------------------------------------------------------
// TXT_DXE_PROTOCOL: Used to provide an interface to TXT DXE Driver so that different functions
//		in TXT DXE Driver can be used by other modules.
//----------------------------------------------------------------------------------------------
//
typedef	struct	_TXT_DXE_PROTOCOL	TXT_DXE_PROTOCOL;
//
struct _TXT_DXE_PROTOCOL {
  TXT_DXE_PROTOCOL_RESET_ESTABLISHMENT	BiosAcResetEstablishmentBit;	// Reset Establishment Bit
  TXT_DXE_PROTOCOL_RESET_AUX_INDEX	BiosAcResetAuxIndex;		// Reset AUX Index
};
//
//----------------------------------------------------------------------------------------------
//
extern	EFI_GUID	gTxtDxeProtocolGuid;		// TXT DXE Protocol GUID
//
//----------------------------------------------------------------------------------------------
#pragma pack ()
//----------------------------------------------------------------------------------------------

#endif
