//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev. 1.01
//      Bug Fixed:  Only sleep to S5 will trigger ADR when the function is enabled.
//      Reason:     Function improvement.
//      Auditor:    Leon
//      Date:       Apr/14/2016
//
//    Rev. 1.00
//      Bug Fixed:  Support ACPI shutdown can trigger ADR.
//      Reason:     Function improvement.
//      Auditor:    Leon
//      Date:       Nov/02/2015
//
//****************************************************************************
//****************************************************************************

/** @file
  The GUID definition for PchOemSmmSxLib

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
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
**/

#include <token.h>	// SMCPKG_SUPPORT
#include <UEFI.h>
#include <PiDxe.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmVariable.h>
#include <Register/PchRegsPmc.h>
#include "PchOemSmmLibrary.h"

#if defined(MEM_NVDIMM_EN) && ( SMCPKG_SUPPORT == 1 )
typedef	VOID (EFIAPI *SMCPROJECTSMMCALLOUT)(VOID);
extern EFI_GUID gSmcNVDIMMSmmProtocolGuid;
extern UINT16 mAcpiBaseAddr;	// SMCPKG_SUPPORT Rev 1.01+
#endif

#if defined(MEM_NVDIMM_EN) && ( SMCPKG_SUPPORT == 1 )
VOID SmcGracefulShutdownTriggerADR(VOID)
{
	EFI_STATUS				Status = EFI_SUCCESS;
	VOID					*pSmcProjectSmmCallOut = NULL;
	
	UINT32      Pm1Cnt;		// SMCPKG_SUPPORT Rev 1.01+
	
	DEBUG((-1, "SmcGracefulShutdownTriggerADR Start\n"));

	Pm1Cnt = IoRead32 ((UINTN) (mAcpiBaseAddr + 0x04));
	DEBUG((-1, "Pm1Cnt=0x%x\n", Pm1Cnt));
	if ( (Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) != V_PCH_ACPI_PM1_CNT_S5 )
		return;

	Status = gSmst->SmmLocateProtocol(
					&gSmcNVDIMMSmmProtocolGuid, 
					NULL, 
					&pSmcProjectSmmCallOut
					);

	DEBUG((-1, "Get gSmcNVDIMMSmmProtocolGuid Status = %r\n", Status));
	DEBUG((-1, "pSmcProjectSmmCallOut address = 0x%x\n", pSmcProjectSmmCallOut));
	
	if ( !EFI_ERROR(Status) && NULL != pSmcProjectSmmCallOut ) {
		DEBUG((-1, "[OemSmmSxPrepareToSleep]1\n"));
		(*(SMCPROJECTSMMCALLOUT)pSmcProjectSmmCallOut)();
	}

	DEBUG((-1, "SmcGracefulShutdownTriggerADR End\n"));
}
#endif


VOID
OemSmmSxPrepareToSleep(
  VOID
  )
{

#if defined(MEM_NVDIMM_EN) && ( SMCPKG_SUPPORT == 1 )
	SmcGracefulShutdownTriggerADR();
#endif

  return;
}

