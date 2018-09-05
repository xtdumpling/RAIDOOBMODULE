/**
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    @file ProcessIioErrors

    Contains Library Implementation file that supports PCIe error handling 
	and reporting in runtime.

---------------------------------------------------------------------------**/
#include <ProcessIioErrors.h>

extern EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccessLib;

/**

    Log VTD Errors.

    @param  IohVtdErr 
            Ioh
    @retval EFI_SUCCESS if the call is succeed.

**/
VOID
LogIohVtdError(
	IN UINT32      IohVtdErr,
	IN UINT8       IioStack,
	IN UINT8       SocketId)
{ 

    ERROR_DATA_TYPE1    ErrorData;
    UINT16              Sev = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;

    ErrorData.Header.ErrorType  = IohVtdSource;
    ErrorData.Socket = SocketId;
    ErrorData.Stack = IioStack;

    // Analyse and log errors based on 'VT Uncorrectable Error Status(VTUNCERRSTS)' register contents
    if( IohVtdErr & BIT0){ 
      ErrorData.ErrorSubType = IOH_VTD_90_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT1){ 
      ErrorData.ErrorSubType = IOH_VTD_91_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT2){
      ErrorData.ErrorSubType = IOH_VTD_92_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT3){
      ErrorData.ErrorSubType = IOH_VTD_93_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT4){
      ErrorData.ErrorSubType = IOH_VTD_94_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT5){
      ErrorData.ErrorSubType = IOH_VTD_95_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT6){
      ErrorData.ErrorSubType = IOH_VTD_96_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT7){
      ErrorData.ErrorSubType = IOH_VTD_97_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT8){
      ErrorData.ErrorSubType = IOH_VTD_98_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT31){
      ErrorData.ErrorSubType = IOH_VTD_A0_ERR;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	Sev = ErrorData.Header.ErrorSeverity;
	ErrorSeverity (Sev);
}
/**

    Log ITC Errors.

    @param  ItcErr 
            Ioh
    @retval EFI_SUCCESS if the call is succeed.

**/
VOID
LogItcError(
	IN UINT32      ItcErrSts,
	IN UINT8       IioStack ,
	IN UINT8       SocketId ) {

    ERROR_DATA_TYPE1    ErrorData;
    UINT16              Sev = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;

    ErrorData.Header.ErrorType  = IohItcSource;
    ErrorData.Socket = SocketId;
    ErrorData.Stack = IioStack;

    // Analyse and log errors based on 'Traffic Controller Error Status(TCERRSTS)' register contents
    if( ItcErrSts & BIT16){ 
      ErrorData.ErrorSubType = ITC_IRP_CRED_UF;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT17){ 
      ErrorData.ErrorSubType = ITC_IRP_CRED_OF;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT18){
      ErrorData.ErrorSubType = ITC_PAR_PCIE_DAT;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT19){
      ErrorData.ErrorSubType = ITC_PAR_HDR_RF;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT20){
      ErrorData.ErrorSubType = ITC_VTMISC_HDR_RF;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT21){
      ErrorData.ErrorSubType = ITC_PAR_ADDR_RF;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT22){
      ErrorData.ErrorSubType = ITC_ECC_COR_RF;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT23){
      ErrorData.ErrorSubType = ITC_ECC_UNCOR_RF;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT24){
      ErrorData.ErrorSubType = ITC_CABORT;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( ItcErrSts & BIT25){
      ErrorData.ErrorSubType = ITC_MABORT;
      ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
      CommonErrorHandling((VOID *)&ErrorData);
    }
    if( ItcErrSts & BIT26){
	ErrorData.ErrorSubType = ITC_MTC_TGT_ERR;
	ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
        CommonErrorHandling((VOID *)&ErrorData);
    }

	Sev = ErrorData.Header.ErrorSeverity;
	ErrorSeverity (Sev);
}

/**

    Log OTC Errors.

    @param  OtcErrSts 
            Ioh
    @retval EFI_SUCCESS if the call is succeed.

**/
VOID
LogOtcError(
	IN UINT32      OtcErrSts,
	IN UINT8       IioStack ,
	IN UINT8       SocketId ) {

	ERROR_DATA_TYPE1    ErrorData;
	UINT16              Sev = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;

	ErrorData.Header.ErrorType  = IohOtcSource;
	ErrorData.Socket = SocketId;
        ErrorData.Stack = IioStack;

	// Analyse and log errors based on 'Traffic Controller Error Status(TCERRSTS)' register contents
	if( OtcErrSts & BIT0){
	    ErrorData.ErrorSubType = OTC_OB_CRED_UF;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT1){
	    ErrorData.ErrorSubType = OTC_OB_CRED_OF;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT2){
	    ErrorData.ErrorSubType = OTC_PAR_HDR_RF;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT3){
	    ErrorData.ErrorSubType = OTC_PAR_ADDR_RF;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	 }
	if( OtcErrSts & BIT4){
	    ErrorData.ErrorSubType = OTC_ECC_UNCOR_RF;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT5){
            ErrorData.ErrorSubType = OTC_CABORT;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT6){
            ErrorData.ErrorSubType = OTC_MABORT;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT7){
	    ErrorData.ErrorSubType = OTC_MTC_TGT_ERR;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT8){
	    ErrorData.ErrorSubType = OTC_ECC_COR_RF;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT9){
	    ErrorData.ErrorSubType = OTC_MISC_REQ_OF;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	}
	if( OtcErrSts & BIT10){
	    ErrorData.ErrorSubType = OTC_PAR_RTE;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	 }
	 if( OtcErrSts & BIT11){
	     ErrorData.ErrorSubType = OTC_IRP_DAT_PAR;
	     ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	     CommonErrorHandling((VOID *)&ErrorData);
	 }
	 if( OtcErrSts & BIT12){
	     ErrorData.ErrorSubType = OTC_IRP_ADDR_PAR;
	     ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	     CommonErrorHandling((VOID *)&ErrorData);
	 }

	 Sev = ErrorData.Header.ErrorSeverity;
	 ErrorSeverity (Sev);
}

/**

    Log DMA Errors.

    @param  IohVtdErr 
            Ioh
    @retval EFI_SUCCESS if the call is succeed.

**/
VOID
LogDmaError(
	IN UINT32      DmaUnCerrSts,
	IN UINT32      *DmaChanErr,
	IN UINT8       IioStack ,
	IN UINT8       SocketId ) {

	ERROR_DATA_TYPE1    ErrorData;
    UINT8               Bit = 0, Func = 0;
    UINT32              DmaChanErrBitMap = 0;
    UINT16              Sev = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;

        ErrorData.Header.ErrorType  = IohDmaSource;
	ErrorData.Socket = SocketId;
	ErrorData.Stack = IioStack;

        // Analyse and log errors based on 'DMA Cluster Uncorrectable Error Status(DMAUNCERRSTS)' register contents
	if( DmaUnCerrSts & BIT2 ){
	    ErrorData.ErrorSubType = DMA_POI_DATA_DP;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE;
	    CommonErrorHandling((VOID *)&ErrorData);
	}

	if( DmaUnCerrSts & BIT3 ){
	    ErrorData.ErrorSubType = DMA_INT_HW_PAR;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	}

	if( DmaUnCerrSts & BIT4 ){
	    ErrorData.ErrorSubType = DMA_CFG_REG_PAR;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE;
	    CommonErrorHandling((VOID *)&ErrorData);
	}

	if( DmaUnCerrSts & BIT7 ){
	    ErrorData.ErrorSubType = DMA_RD_CMPL_HDR;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE;
	    CommonErrorHandling((VOID *)&ErrorData);
	}

	if( DmaUnCerrSts & BIT10 ){
	    ErrorData.ErrorSubType = DMA_RD_ADR_DEC;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE;
	    CommonErrorHandling((VOID *)&ErrorData);
	}

	if( DmaUnCerrSts & BIT12 ){
	    ErrorData.ErrorSubType = DMA_SYNDROME;
	    ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
	    CommonErrorHandling((VOID *)&ErrorData);
	}


	//Analyse and log errors based on 'Internal DMA Channel Error Status Registers(CHANERR_INT)' contents
        for ( Func = 0; Func < 8; Func++ ) {
           for ( Bit = 0; Bit < 24 ; Bit++ ) {
                switch ( (DmaChanErr[Func] & (1u << Bit))  ) {
                case BIT0:
                	DEBUG((-1, "CHANERR_INT BIT0 is set!.\n"));
                	ErrorData.ErrorSubType = DMA_TRA_SADDR_ERR;
                	ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
                	CommonErrorHandling((VOID *)&ErrorData);
                	break;
                case BIT1:
                	DEBUG((-1, "CHANERR_INT BIT1 is set!.\n"));
                	ErrorData.ErrorSubType = DMA_XFRER_DADDR_ERR;
                	ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
                	CommonErrorHandling((VOID *)&ErrorData);
                	break;
                case BIT2:
                	 ErrorData.ErrorSubType = DMA_NXT_DESC_ADDR_ERR;
                	 ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
                	 CommonErrorHandling((VOID *)&ErrorData);
                	 break;                	
                case BIT3:
                	 ErrorData.ErrorSubType = DMA_DESC_ERR;
                	 ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
                	 CommonErrorHandling((VOID *)&ErrorData);
                	 break;
                case BIT4:
        		ErrorData.ErrorSubType = DMA_CHN_ADDR_VALERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT5:
        		ErrorData.ErrorSubType = DMA_CHANCMD_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT6:
        		ErrorData.ErrorSubType = DMA_CDATA_PARERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT7:
               		ErrorData.ErrorSubType = DMA_DATA_PARERR;
                	ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
                	CommonErrorHandling((VOID *)&ErrorData);
                	break;
                case BIT8:
        		ErrorData.ErrorSubType = DMA_RD_DATA_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT9:
        		ErrorData.ErrorSubType = DMA_WR_DATA_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT10:
        		ErrorData.ErrorSubType = DMA_DESC_CTRL_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT11:
        		ErrorData.ErrorSubType = DMA_DESC_LEN_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT12:
        		ErrorData.ErrorSubType = DMA_CMP_ADDR_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT13:
        		ErrorData.ErrorSubType = DMA_INT_CFG_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT16:
        		ErrorData.ErrorSubType = DMA_CRC_XORP_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT17:
        		ErrorData.ErrorSubType = DMA_XORQERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT18:
        		ErrorData.ErrorSubType = DMA_DESCCNTERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT19:
        		ErrorData.ErrorSubType = DMA_DIF_ALLFTAG_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT20:
        		ErrorData.ErrorSubType = DMA_DIF_GUARDTAG_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT21:
        		ErrorData.ErrorSubType = DMA_DIF_APPTAG_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT22:
        		ErrorData.ErrorSubType = DMA_DIF_REFTAG_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                case BIT23:
        		ErrorData.ErrorSubType = DMA_DIF_BNDL_ERR;
        		ErrorData.Header.ErrorSeverity  = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
        		CommonErrorHandling((VOID *)&ErrorData);
        		break;
                default:
                	break;
                }
           }
        }

    Sev = ErrorData.Header.ErrorSeverity;
    ErrorSeverity (Sev);
}

/**

    Handle VTD Errors

    @param  BusNum   
            IioStack 

    @retval EFI_SUCCESS if the call is succeed.

**/
VOID
HandleVtdErrors(
IN UINT8      SocketId,
IN UINT8      IioStack ) {

	LFERRST_IIO_RAS_STRUCT     LFErrSt;
	LNERRST_IIO_RAS_STRUCT     LNErrSt;
	LCERRST_IIO_RAS_STRUCT     LCErrSt;
	VTUNCERRSTS_IIO_VTD_STRUCT VtUnCerrSts;

	//Read VT Uncorrectable Error Status register
    VtUnCerrSts.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, VTUNCERRSTS_IIO_VTD_REG); 
    DEBUG((-1, "Vtd Error detected.\n"));
    DEBUG((-1, "HandleVtdErrors: VtUnCerrSts reg 0x%08X, Socket No 0x%02X \n", VtUnCerrSts.Data,SocketId));
	
    //Log VTD Error
    LogIohVtdError (VtUnCerrSts.Data,IioStack,SocketId);

    //
    // Local IIO Error Status
    //
    LFErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LFERRST_IIO_RAS_REG); 
    LNErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LNERRST_IIO_RAS_REG); 
    LCErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LCERRST_IIO_RAS_REG);

    LFErrSt.Bits.vtd = 1;
    LNErrSt.Bits.vtd = 1;
    LCErrSt.Bits.vtd = 1;

    // Clear the Fatal and Non-Fatal FERR and NERR.
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LFERRST_IIO_RAS_REG, LFErrSt.Data);    
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LNERRST_IIO_RAS_REG, LNErrSt.Data);    
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LCERRST_IIO_RAS_REG, LCErrSt.Data);    
}

/**

    Handle Inbound Traffic Controller Errors

    @param  BusNum   
            IioStack 

    @retval EFI_SUCCESS if the call is succeed.

**/
VOID
HandleItcErrors(
IN UINT8      SocketId,
IN UINT8      IioStack ) {
	
	LFERRST_IIO_RAS_STRUCT    LFErrSt;
	LNERRST_IIO_RAS_STRUCT    LNErrSt;
	LCERRST_IIO_RAS_STRUCT    LCErrSt;
	TCERRST_IIO_RAS_STRUCT    TCerrSts;

    TCerrSts.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, TCERRST_IIO_RAS_REG); 
    DEBUG((-1, "ITC Error detected.\n"));
    DEBUG((-1, "HandleITCErrors: TCerrSts reg 0x%08X, SocketId No 0x%02X \n", TCerrSts.Data,SocketId));

    LogItcError (TCerrSts.Data,IioStack,SocketId);

    //
    // Local IIO Error Status
    //
    LFErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LFERRST_IIO_RAS_REG); 
    LNErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LNERRST_IIO_RAS_REG); 
    LCErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LCERRST_IIO_RAS_REG);

    LFErrSt.Bits.ioh = 1;
    LNErrSt.Bits.ioh = 1;
    LCErrSt.Bits.ioh = 1;

    // Clear the Fatal and Non-Fatal FERR and NERR.
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LFERRST_IIO_RAS_REG, LFErrSt.Data);    
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LNERRST_IIO_RAS_REG, LNErrSt.Data);    
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LCERRST_IIO_RAS_REG, LCErrSt.Data);    
}

/**

    Handle Outbound Traffic Controller Errors

    @param  BusNum   
            IioStack 

    @retval EFI_SUCCESS if the call is succeed.

**/
VOID
HandleOtcErrors(
IN UINT8      SocketId,
IN UINT8      IioStack ) {
	
	LFERRST_IIO_RAS_STRUCT    LFErrSt;
	LNERRST_IIO_RAS_STRUCT    LNErrSt;
	LCERRST_IIO_RAS_STRUCT    LCErrSt;
	TCERRST_IIO_RAS_STRUCT    TCerrSts;


    TCerrSts.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, TCERRST_IIO_RAS_REG); 
    DEBUG((-1, "OTC Error detected.\n"));
    DEBUG((-1, "HandleOTCErrors: TCerrSts reg 0x%08X, SocketId No 0x%02X \n", TCerrSts.Data,SocketId));


    LogOtcError (TCerrSts.Data,IioStack,SocketId);

    //
    // Local IIO Error Status
    //
    LFErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LFERRST_IIO_RAS_REG); 
    LNErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LNERRST_IIO_RAS_REG); 
    LCErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LCERRST_IIO_RAS_REG);

    LFErrSt.Bits.thermal = 1;
    LNErrSt.Bits.thermal = 1;
    LCErrSt.Bits.thermal = 1;

    // Clear the Fatal and Non-Fatal
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LFERRST_IIO_RAS_REG, LFErrSt.Data);    
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LNERRST_IIO_RAS_REG, LNErrSt.Data);    
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LCERRST_IIO_RAS_REG, LCErrSt.Data);  
}

/**

    Handle DMA Errors

    @param  BusNum   
            IioStack 

    @retval EFI_SUCCESS if the call is succeed.

**/
VOID
HandleDmaErrors(
IN UINT8      SocketId,
IN UINT8      IioStack ) {
	
    LFERRST_IIO_RAS_STRUCT             LFErrSt;
    LNERRST_IIO_RAS_STRUCT             LNErrSt;
    LCERRST_IIO_RAS_STRUCT             LCErrSt;
    DMAUNCERRSTS_IIOCB_FUNC0_STRUCT    DmaUnCerrSts;
    UINT32                             ChanerrIntRegFunc_0_7[8];
    UINTN                              Func = 0;
    IIO_ERRSTS_REG                     LogDmaErrorRegList[] = {
    //
    // CBDMA Error Status Registers
    //
    { CHANERR_INT_IIOCB_FUNC0_REG  },
    { CHANERR_INT_IIOCB_FUNC1_REG  },
    { CHANERR_INT_IIOCB_FUNC2_REG  },
    { CHANERR_INT_IIOCB_FUNC3_REG  },
    { CHANERR_INT_IIOCB_FUNC4_REG  },
    { CHANERR_INT_IIOCB_FUNC5_REG  },
    { CHANERR_INT_IIOCB_FUNC6_REG  },
    { CHANERR_INT_IIOCB_FUNC7_REG  }
    };

    DmaUnCerrSts.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, DMAUNCERRSTS_IIOCB_FUNC0_REG); 
    DEBUG((-1, "DMA Error detected.\n"));
    DEBUG((-1, "HandleDmaErrors: DmaUnCerrSts reg 0x%08X, SocketId No 0x%02X \n", DmaUnCerrSts.Data,SocketId));

    for ( Func = 0 ; Func < 8 ; Func++ ) {
	  ChanerrIntRegFunc_0_7[Func] = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LogDmaErrorRegList[Func].RegDef);
    }

    LogDmaError (DmaUnCerrSts.Data,ChanerrIntRegFunc_0_7,IioStack,SocketId);

    //
    // Local IIO Error Status
    //
    LFErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LFERRST_IIO_RAS_REG); 
    LNErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LNERRST_IIO_RAS_REG); 
    LCErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LCERRST_IIO_RAS_REG);

    LFErrSt.Bits.dma = 1;
    LNErrSt.Bits.dma = 1;
    LCErrSt.Bits.dma = 1;

    // Clear the Fatal and Non-Fatal errors.
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LFERRST_IIO_RAS_REG, LFErrSt.Data);    
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LNERRST_IIO_RAS_REG, LNErrSt.Data);    
    mCpuCsrAccessLib->WriteCpuCsr (SocketId, IioStack, LCERRST_IIO_RAS_REG, LCErrSt.Data);  
}
