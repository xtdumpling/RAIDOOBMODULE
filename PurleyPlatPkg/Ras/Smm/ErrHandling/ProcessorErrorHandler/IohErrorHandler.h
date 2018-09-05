//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2016, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
//  
//  Rev. 1.00
//    Bug Fixed:  Support SMC Memory map-out function.
//    Reason  : 
//    Auditor : Ivern Yeh
//    Date    : Jul/07/2016
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  IohErrorHandler.h
//
//  Description:
//  Header file for IOH Error Logging code.
//
//<AMI_FHDR_END>
//*************************************************************************

#ifndef _IOH_ERROR_HANDLER_H
#define _IOH_ERROR_HANDLER_H

#include "ErrorRecords.h"
#include "UncoreCommonIncludes.h"
#include "CommonErrorHandlerDefs.h"
#include <Protocol\MemRasProtocol.h>
#include <Protocol\PlatformErrorHandlingProtocol.h>
#include <Library\emcaplatformhookslib.h>
#include <Library\emcalib.h>
#include <Library\PciExpressLib.h>
#include <IndustryStandard\Pci22.h>
#include "RtErrorLog.h"
#include "RtErrorLogBoard.h"

//extern UINT8	mResetRequired;

extern  EFI_GUID  gErrRecordNotifyPcieGuid;
extern  EFI_GUID  gErrRecordPcieErrGuid;
extern  EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;

#define EFI_PCI_BRIDGE_SUBTRACTIVE_DEV_CLASS_CODE   0x06040100
#define EFI_PCI_BRIDGE_DEV_CLASS_CODE               0x06040000

#define     IIO_GLBL_ERR_BITMASK            0x0000003F;     //<<SKXIIORAS-TODO: Update after Silicon DE provide


// As per Intel recommendation disabling the 0xC3 error.
// As per intel recommendation disabling the 0xC2 and 0xb4 errors.
#define QPI_P_ERRMSK 0x4404 // Bit 0 to 19 QPI Protocol errors

#define IOH_CORE_C4     BIT4 // C4 - Master abort address error
#define IOH_CORE_C5     BIT5 // C5 - Completor abort address error
#define IOH_CORE_C6     BIT6 // C6 - FIFO Overflow/Underflow error


#define MCA_BANK_IFU                           0
#define MCA_BANK_DCU                           1
#define MCA_BANK_DLTB                          2
#define MCA_BANK_MLC                           3
#define MCA_BANK_PCU                           4
#define MCA_BANK_UPI0                          5
#define MCA_BANK_IIO                           6
#define MCA_BANK_M2M0                          7
#define MCA_BANK_M2M1                          8
#define MCA_BANK_CHA0                          9
#define MCA_BANK_CHA1                          10
#define MCA_BANK_CHA2                          11
#define MCA_BANK_UPI1                          12
#define MCA_BANK_UPI2                          19

typedef enum {
    IOH_QPI_B4_ERR=0xB4,
    IOH_QPI_C1_ERR=0xC1,
    IOH_QPI_C2_ERR=0xC2,
    IOH_QPI_C3_ERR=0xC3,
    IOH_QPI_D7_ERR=0xD7,
    IOH_QPI_DA_ERR=0xDA,
    IOH_QPI_DB_ERR=0xDB,
} IOH_QPI_ERROR_TYPE;

typedef enum {
    IOH_CORE_C4_ERR = 0xc4,
    IOH_CORE_C5_ERR,
    IOH_CORE_C6_ERR
} IOH_CORE_ERROR_TYPE;

typedef enum {
    IOH_VTD_90_ERR = 0x90,
    IOH_VTD_91_ERR = 0x91,
    IOH_VTD_92_ERR = 0x92,
    IOH_VTD_93_ERR = 0x93,
    IOH_VTD_94_ERR = 0x94,
    IOH_VTD_95_ERR = 0x95,
    IOH_VTD_96_ERR = 0x96,
    IOH_VTD_97_ERR = 0x97,
    IOH_VTD_98_ERR = 0x98,
    IOH_VTD_A0_ERR = 0xA0
} IOH_VTD_ERROR_TYPE;


VOID
HandleIohQpiErrors (
	IN UINT8   BusNum,
	IN UINT8      Ioh);

VOID
HandleIohCoreErrors (
	IN UINT8   BusNum,
	IN UINT8      Ioh);

VOID
HandleIohVtdErrors (
	IN UINT8   BusNum,
	IN UINT8      Ioh);

VOID
LastBootIohErrorHandler(); 

VOID
LastBootErrorHandler();

extern 
EFI_STATUS
CommonErrorHandling (
  VOID  *ErrStruc
);

EFI_STATUS
ProcessSocketMcBankError (
  VOID
  );
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT 
VOID
LogMemorylastbooterror (
VOID
);
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
