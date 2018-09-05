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
//  Rev. 1.01
//    Bug Fix : Update the DIMM number of Socket.
//    Reason  : 
//    Auditor : Ivern Yeh
//    Date    : Jul/07/2016
//
//  Rev. 1.00
//    Bug Fix : Update ECC/PCI SMBIOS event log. (From Grantley)
//    Reason  : 
//    Auditor : Ivern Yeh
//    Date    : Jun/16/2016
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

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
// Revision History
// ----------------
// $Log:  $
// 
// 
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  RtErrorLogGpnv.h
//
// Description: Runtime error log GPNV includes and declarations
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef _RT_ERROR_LOG_GPNV_H
#define _RT_ERROR_LOG_GPNV_H

//
// Include files
//
#include "token.h"
#include "RtErrorLogBoard.h"
#include <Protocol/SmbiosElogSupport.h>
#include <Protocol/GenericElogProtocol.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Include/PCI.h>


//
// Function Declaration
//
EFI_STATUS
LogMemErrorToGpnv(
  IN AMI_MEMORY_ERROR_SECTION     *MemInfo
 );

EFI_STATUS
LogPciErrorToGpnv (
		AMI_PCIE_ERROR_SECTION				*PcieInfo
 );

#pragma pack(push, 1)
typedef struct {
    UINT32 Correctable:1;
    UINT32 UnCorrectable:1;
    UINT32 DimmNum:2;       // DIMM 0-3
    UINT32 Socket:2;        // Socket 0-3
    UINT32 MemoryController:1; // MemoryController 0-1
    UINT32 Channel:2;       // Channel 0-3
    UINT32 Reserved:23;
} NB_ECC_ERR;

typedef struct {
    UINT8 Bus;
    UINT8 Dev;
    UINT8 Fun;
    UINT16 DeviceStatus;
    UINT32 Correctable:1;
    UINT32 NonFatal:1;
    UINT32 Fatal:1;
    UINT32 ParityErr:1;  // PERR
    UINT32 SystemErr:1;  // SERR
    UINT32 Reserved:27;
} NB_PCIE_ERR;

typedef union {
    NB_ECC_ERR   EccMsg;
    NB_PCIE_ERR  PcieMsg;
} ERROR_MSG;

typedef enum {
    NO_ERROR = 0,
    ECC_ERROR,
    NB_PCIE_ERROR,
    SB_PCIE_ERROR,
    SB_PCI_ERROR,
    ERROR_MAX
} HARDWARE_ID;

typedef struct _NB_ERROR_INFO {
    EFI_GPNV_RUNTIME_ERROR  Header;     // standard header ...error type
    HARDWARE_ID             HardwareId; // identify the hardware
    ERROR_MSG               Msg;        // hardware-specific error information
} ERROR_INFO;
#pragma pack(pop)
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
