/**
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2015 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file ChipsetErrorReporting

    Contains Library Implementation file that supports Chipset error handling

---------------------------------------------------------------------------**/
#include <RtErrorlogBoard.h>
#include <Library\ChipsetErrReporting\ChipsetErrReportingPrivate.h>
#include <RcRegs\IIO_RAS.h>
#include <RtErrorLog.h>

#define ITC_ERROR_STATUS_BIT   BIT23
#define OTC_ERROR_STATUS_BIT   BIT21

typedef enum {
    IOH_VTD_90_ERR = 0x90,  //PERR_context_cache - Data parity error while doing a context cache lookup status
    IOH_VTD_91_ERR = 0x91,  //PERR_L1_lookup - Data parity error while doing a L1 lookup status
    IOH_VTD_92_ERR = 0x92,  //PERR_L2_lookup -Data parity error while doing a L2 lookup status
    IOH_VTD_93_ERR = 0x93,  //PERR_L3_lookup - Data parity error while doing a L3 lookup status
    IOH_VTD_94_ERR = 0x94,  //PERR_TLB0 - TLB0 parity error status
    IOH_VTD_95_ERR = 0x95,  //PERR_TLB1 - TLB1 parity error status
    IOH_VTD_96_ERR = 0x96,  //UNSUCC_CI_RDCP - Unsuccessful status received in the coherent interface read completion status
    IOH_VTD_97_ERR = 0x97,  //miscerrs - Illegal request to 0xFEE
    IOH_VTD_98_ERR = 0x98,  //PROTMEMVIOL -Protected memory region space violated status
    IOH_VTD_A0_ERR = 0xA0   //VTDERR - Intel VT-d spec defined errors
} IOH_VTD_ERROR_TYPE;

typedef enum {
	ITC_IRP_CRED_UF   = 0x80,   //ITC IRP credit underflow
	ITC_IRP_CRED_OF   = 0x81,   //ITC IRP credit overflow
	ITC_PAR_PCIE_DAT  = 0x82,   //Parity error in the incoming data from PCIe
	ITC_PAR_HDR_RF    = 0x83,   //Parity error in the ITC hdr_q RF
	ITC_VTMISC_HDR_RF = 0x84,   //Parity error in the ITC vtd_misc_info RF
	ITC_PAR_ADDR_RF   = 0x85,   //Parity error in the ITC addr_q RF
	ITC_ECC_COR_RF    = 0x86,   //ECC corrected error in the ITC dat_dword RF
	ITC_ECC_UNCOR_RF  = 0x87,   //ECC uncorrected error in the ITC dat_dword RF
	ITC_CABORT        = 0x88,   //Completer abort
	ITC_MABORT        = 0x89,   //Master abort
	ITC_MTC_TGT_ERR   = 0x8A    //Multicast target error for ITC only
} IOH_ITC_ERROR_TYPE;

typedef enum {
	OTC_OB_CRED_UF    = 0x60,  //OTC OB credit underflow
	OTC_OB_CRED_OF    = 0x61,  //OTC OB credit overflow
	OTC_PAR_HDR_RF    = 0x62,  //Parity error in the OTC hdr_q RF
	OTC_PAR_ADDR_RF   = 0x63,  //Parity error in the OTC addr_q RF
	OTC_ECC_UNCOR_RF  = 0x64,  //ECC uncorrected error in the OTC dat_dword RF
	OTC_CABORT        = 0x65,  //Completer abort
	OTC_MABORT        = 0x66,  //Master abort
	OTC_MTC_TGT_ERR   = 0x67,  //Multicast target error for ITC
	OTC_ECC_COR_RF    = 0x68,  //ECC corrected error in the OTC dat_dword RF
	OTC_MISC_REQ_OF   = 0x69,  //Misc block request overflow
	OTC_PAR_RTE       = 0x6A,  //IOAPIC RTE parity error
	OTC_IRP_DAT_PAR   = 0x6B,  //parity error on incoming data from IRP
	OTC_IRP_ADDR_PAR  = 0x6C   //Parity error on incoming addr from IRP
} IOH_ITC_ERROR_TYPE;


typedef enum {
	DMA_POI_DATA_DP        = 0x40,  //Received_Poisoned_Data_from_DP_status
	DMA_INT_HW_PAR         = 0x41,  //DMA_internal_HW_parity_error_status
	DMA_CFG_REG_PAR        = 0x42,  //Cfg_Reg_Parity_Error_status
	DMA_RD_CMPL_HDR        = 0x43,  //RD_Cmpl_Header_Error_status
	DMA_RD_ADR_DEC         = 0x44,  //Read_address_decode_error_status
	DMA_SYNDROME           = 0x45,  //Multiple errors
	DMA_TRA_SADDR_ERR      = 0x46,  //DMA Transfer Source Address Error.
	DMA_XFRER_DADDR_ERR    = 0x47,  //DMA Transfer Destination Address Error.
	DMA_NXT_DESC_ADDR_ERR  = 0x48,  //Next Descriptor Address Error.
	DMA_DESC_ERR           = 0x49,  //Error when readign a DMA descriptor
	DMA_CHN_ADDR_VALERR    = 0x4A,  //Chain Address Value Error.
	DMA_CHANCMD_ERR        = 0x4B,  //CHANCMD Error
	DMA_CDATA_PARERR       = 0x4C,  //Data Parity Error.
	DMA_DATA_PARERR        = 0x4D,  //DMA Data Parity Error.
	DMA_RD_DATA_ERR        = 0x4E,  //Read Data Error.
	DMA_WR_DATA_ERR        = 0x4F,  //Write Data Error.
	DMA_DESC_CTRL_ERR      = 0x50,  //Descriptor Control Error.
	DMA_DESC_LEN_ERR       = 0x51,  //Descriptor Length Error.
	DMA_CMP_ADDR_ERR       = 0x52,  //Completion Address Error.
	DMA_INT_CFG_ERR        = 0x53,  //Interrupt Configuration Error.
	DMA_CRC_XORP_ERR       = 0x54,  //CRC or XOR P Error
	DMA_XORQERR            = 0x55,  //XOR Q Error
	DMA_DESCCNTERR         = 0x56,  //Descriptor Count Error
	DMA_DIF_ALLFTAG_ERR    = 0x57,  //DIF All F Detect Error
	DMA_DIF_GUARDTAG_ERR   = 0x58,  //DIF Guard Tag Error
	DMA_DIF_APPTAG_ERR     = 0x59,  //DIF Application Tag Error
	DMA_DIF_REFTAG_ERR     = 0x5A,  //DIF Reference Tag Error
	DMA_DIF_BNDL_ERR       = 0x5B   //DIF Bundle Error
} IOH_DMA_ERROR_TYPE;

VOID
HandleVtdErrors(
	IN UINT8      SocketId,
	IN UINT8      IioStack );

VOID
LogIohVtdError(
	IN UINT32      IohVtdErr,
	IN UINT8       IioStack ,
	IN UINT8       SocketId );

VOID
HandleItcErrors(
	IN UINT8      SocketId,
	IN UINT8      IioStack );

VOID
LogItcError(
	IN UINT32      ItcErr,
	IN UINT8       IioStack,
	IN UINT8       SocketId );

VOID
HandleOtcErrors(
	IN UINT8      SocketId,
	IN UINT8      IioStack );

VOID
LogOtcError(
	IN UINT32      OtcErr,
	IN UINT8       IioStack,
	IN UINT8       SocketId );
VOID
HandleDmaErrors(
	IN UINT8      SocketId,
	IN UINT8      IioStack );

VOID
LogDmaError(
	IN UINT32      DmaUnCerrSts,
	IN UINT32      *DmaChanErr,
	IN UINT8      IioStack,
	IN UINT8       SocketId );

VOID 
ErrorSeverity (
  IN      OUT   UINT16 Sev
    );
