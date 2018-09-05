/**
Copyright (c) 2007 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file LastBootErrorLogHob.h

---------------------------------------------------------------------*/

#ifndef _LAST_BOOT_ERROR_LOG_HOB_H_
#define _LAST_BOOT_ERROR_LOG_HOB_H_

extern EFI_GUID  gPrevBootErrSrcHobGuid;

#define PREV_BOOT_ERR_SRC_HOB_SIZE	1000

typedef struct {
UINT16	Length;		// Actual size of the error sources  used in the HOB
UINT8	HobData[PREV_BOOT_ERR_SRC_HOB_SIZE -2];	// List of Error source structures of format //MCBANK_ERR_INFO  or CSR_ERR_INFO
}PREV_BOOT_ERR_SRC_HOB;

typedef struct{
UINT8	Type;		// McBankType = 1;
UINT8	Segment;
UINT8	Socket;
UINT16	ApicId;		// ApicId  is Needed only if it a core McBank.
UINT16	McBankNum;
UINT64	McBankStatus;
UINT64	McbankAddr;
UINT64	McBankMisc;
} MCBANK_ERR_INFO;

typedef struct {
UINT8	Type;		// PciExType =2 ;
UINT8	Segment;
UINT8	Bus;
UINT8	Device;
UINT8	Function;
UINT32	AerUncErrSts;
UINT8	AerHdrLogData[16];
} PCI_EX_ERR_INFO;

typedef struct {
UINT8	Type;		// Other Csr error type =3 ;
UINT8	Segment;
UINT8	Bus;
UINT8	Device;
UINT8	Function;
UINT16	offset;
UINT32	Value;
} CSR_ERR_INFO;

typedef enum {
  McBankType = 1,
  PciExType,
  CsrOtherType
} ERROR_ACCESS_TYPE;

#endif
