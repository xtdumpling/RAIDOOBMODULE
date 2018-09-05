//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//
//*****************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		AmiErrReportingLib.h
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include <Uefi.h>
#include <Protocol/CpuCsrAccess.h>
#include <PiDxe.h>
#include <Protocol/SmmBase2.h>
#include <protocol/MemRasProtocol.h>
#include <UncoreCommonIncludes.h>
#include <CommonErrorHandlerDefs.h>
#include "CpuPciAccess.h"
#include "RcRegs.h"
#include <IndustryStandard\WheaDefs.h>
#include <Library/mpsyncdatalib.h>
#include <Library/emcaplatformhookslib.h>
#include <Protocol\ErrorHandlingProtocol.h>
#include <Library\emcalib.h>
#include <Library\ProcMemErrReporting.h>
#include <emca.h>
#include <OemRasLib\OemRasLib.h>


#define ERROR_EVENT_ADDDC           0
#define ERROR_EVENT_RANK_SPARING    1
#define ERROR_EVENT_SDDC            2
#define ERROR_EVENT_MIRR_FAILOVER   3
#define DEF_HANDLER                 4 
#define SKIP_CE_HANDLING            5

#define MSR_ERROR_CONTROL       0x17F

VOID FillMemRecordBeforeHandling (
	IN  UINT8 NodeId,
	OUT UEFI_MEM_ERROR_RECORD  *MemoryErrorRecord,
	IN  UINT8  ErrorEvent
  );
  
BOOLEAN
IsErrorExcdForThisRankBeforeHandling (
    IN UINT8 Node,
    IN UINT8 NodeCh,
    IN UINT8 Rank
 );
 
UINT8 
GetErrorValidMsrCount (
	);

VOID
FillOtherMcBankInfo (
	IN   UINT8         Skt,	
	OUT  MCA_BANK_INFO *OtherMcBankInfo,
	OUT  UINT8         *OtherMcBankCount
);

VOID
EnableMode1MemErrorReporting();

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//