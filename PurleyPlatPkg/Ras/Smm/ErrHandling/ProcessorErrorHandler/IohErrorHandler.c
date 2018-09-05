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
//
//
//  Rev. 1.02
//   Bug Fixed:  Fixe Memory Map out feature could not work. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//  
//  Rev. 1.01
//      Bug Fixed:  Fix cannot log SERR/PERR. 
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Sep/09/2016
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
// Name:  IohErrorHandler.c
//
//  Description:
//  Initialization and handler code for IOH Errors.
//
//<AMI_FHDR_END>
//*************************************************************************

//---------------------------------------------------------------------------
// Includes

#include <Token.h>
#include "IohErrorHandler.h"

//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//

#pragma optimize( "", off )
//UINT8						mResetRequired;

extern  EFI_MEM_RAS_PROTOCOL                  *mMemRas;
extern  EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccess;
extern EFI_RAS_SYSTEM_TOPOLOGY              *mRasTopology;

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HandleIohCoreErrors
//
// Description:
//  Event handler for Ioh Core errors. 
// Input:
//
// Output: 
//  VOID
//
// Modified:
//  Nothing
//
// Referrals:
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
/*
VOID
HandleIohCoreErrors(
IN UINT8      BusNum,
IN UINT8      Ioh)
{ 
    UINT32  ErrMask;
    UINT32	Data32 = 0;
    UINT32  IohCoreErr;
    ERROR_DATA_TYPE1    ErrorData;

    IohCoreErr = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IIOERRST));
    ErrorData.Header.ErrorType  = IohCoreSource;
    ErrorData.Socket = Ioh;
#if RUNTIME_ERROR_DEBUG_MESSAGE
	DEBUG((-1, "IOH Core Error detected.\n"));
    DEBUG((-1, "HandleIohCoreErrors: IohCoreErr reg 0x%08X, Ioh No 0x%02X \n", IohCoreErr,Ioh));
#endif
    if( IohCoreErr & IOH_CORE_C4){ 
      ErrorData.ErrorSubType = IOH_CORE_C4_ERR;
      ErrorData.Header.ErrorSeverity  = Correctable;
      CommonErrorHandling((VOID *)&ErrorData);
    }
    
    if( IohCoreErr & IOH_CORE_C5){ 
      ErrorData.ErrorSubType = IOH_CORE_C5_ERR;
      ErrorData.Header.ErrorSeverity  = Correctable;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohCoreErr & IOH_CORE_C6){ 
      ErrorData.ErrorSubType = IOH_CORE_C6_ERR;
      ErrorData.Header.ErrorSeverity  = Correctable;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohCoreErr & (IOH_CORE_C4 | IOH_CORE_C5 | IOH_CORE_C6)){
        // TO_DO : need to do Power good reset, handle mResetRequired variable
        mResetRequired = PowerGood;
    }

    ErrMask = BIT23;


    // Clear the Fatal and Non-Fatal FERR and NERR.
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFFERRST));
    PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFFERRST), Data32 | ErrMask);
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFNERRST));
    PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFNERRST), Data32 | ErrMask);
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNFERRST));
    PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNFERRST), Data32 | ErrMask);
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNNERRST));
    PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNNERRST), Data32 | ErrMask);

    return;
  }

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ClearIohQpiErrors
//
// Description: Clear IOH QPI Errors
//
// Parameters:  
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
ClearIohQpiErrors(
  UINT8 Bus)
{

  UINT32  Data32;

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0FFERRST));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0FFERRST), Data32);

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1FFERRST));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1FFERRST), Data32);

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0FNERRST));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0FNERRST), Data32);

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1FNERRST));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1FNERRST), Data32);
 
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0NFERRST));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0NFERRST), Data32);

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1NFERRST));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1NFERRST), Data32);

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0NNERRST));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0NNERRST), Data32);

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1NNERRST));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1NNERRST), Data32);

}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HandleIohQpiErrors
//
// Description:
//  Event handler for Ioh QPI errors. 
// Input:
//
// Output: 
//  VOID
//
// Modified:
//  Nothing
//
// Referrals:
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
HandleIohQpiErrors(
IN UINT8      BusNum,
IN UINT8      Ioh)
{ 
  
  BOOLEAN FatalError = FALSE; 
  UINT32  ErrMask;
  UINT32	Data32 = 0;
  UINT16	Data16 = 0;
  UINT8	  Data8 = 0;
  UINT32  IohQpiPErr;
  UINT32  IohQpiP0Err;
  UINT32  IohQpiP1Err;
  ERROR_DATA_TYPE1    ErrorData;

#if RUNTIME_ERROR_DEBUG_MESSAGE
   DEBUG((-1, "IOH QPI Error detected.\n"));
#endif

    // First, read the global system status register.
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(CORE05_BUS_NUM, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GSYSST));
    
    // Severity 2 fatal error 
    if (Data32 & V_IIO_GSYSCTL_S2STS) {
      FatalError = TRUE;
    }
      
    IohQpiP0Err = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0ERRST));
    IohQpiP1Err = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1ERRST));

    ErrorData.Header.ErrorType  = IohQpiSource;
    ErrorData.Socket = Ioh;
        
    if( IohQpiP0Err & QPI_P_ERRMSK){
      ErrorData.Port = 0;
    }

    if( IohQpiP0Err & QPI_P_ERRMSK){
      ErrorData.Port = 1;
    }

    IohQpiPErr = (IohQpiP0Err | IohQpiP1Err);

#if RUNTIME_ERROR_DEBUG_MESSAGE
   DEBUG((-1, "HandleIohQpiErrors: IohQpiPErr reg 0x%08X, Ioh No 0x%02X \n", IohQpiPErr,Ioh));
#endif

    // Protocol errors 

      if( IohQpiPErr & B_IIO_IRPPCTL_WRI_CACHE_COR_EN){
      ErrorData.ErrorSubType = IOH_QPI_B4_ERR;
      ErrorData.Header.ErrorSeverity  = Correctable;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohQpiPErr & B_IIO_IRPPCTL_LAY_EN){
      ErrorData.ErrorSubType = IOH_QPI_C1_ERR;
      ErrorData.Header.ErrorSeverity  = Recoverable;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohQpiPErr & B_IIO_IPRERR_WRI_CAC_UNCOR_ECC_EN){
      ErrorData.ErrorSubType = IOH_QPI_C2_ERR;
      ErrorData.Header.ErrorSeverity  = Recoverable;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohQpiPErr & B_IIO_IPRERR_CSR_CRO32BIT_BOUN_EN){
      ErrorData.ErrorSubType = IOH_QPI_C3_ERR;
      ErrorData.Header.ErrorSeverity  = Recoverable;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohQpiPErr & B_IIO_IPRERR_PRO_REC_UNRES_EN){
      ErrorData.ErrorSubType = IOH_QPI_D7_ERR;
      ErrorData.Header.ErrorSeverity  = Fatal;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohQpiPErr & B_IIO_IPRERR_PRO_QUETAB_FLOW_EN){
      ErrorData.ErrorSubType = IOH_QPI_DA_ERR;
      ErrorData.Header.ErrorSeverity  = Fatal;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohQpiPErr & B_IIO_IRPERR_PRO_PAR_ERR_EN){
      ErrorData.ErrorSubType = IOH_QPI_DB_ERR;
      ErrorData.Header.ErrorSeverity  = Fatal;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    // Clear IOH QPI Errors

      ClearIohQpiErrors(BusNum);

     // Set up the IOH QPI Error Mask.

      ErrMask = (BIT0 | BIT1);


      // Clear the Fatal and Non-Fatal FERR and NERR.
      Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFFERRST));
      PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFFERRST), Data32 | ErrMask);
      Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFNERRST));
      PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFNERRST), Data32 | ErrMask);
      Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNFERRST));
      PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNFERRST), Data32 | ErrMask);
      Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNNERRST));
      PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNNERRST), Data32 | ErrMask);


    if (FatalError) {
      // Do the System reset on fatal errors, need to check all kind of errors before reset.
	  //	SystemResetOnFe ();
    }
    return;
  }

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LogIohVtdError
//
// Description:
//  Detect type of VTd error
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
LogIohVtdError(
	IN UINT32      IohVtdErr,
	IN UINT8      Ioh)
{ 

	ERROR_DATA_TYPE1    ErrorData;

    ErrorData.Header.ErrorType  = IohVtdSource;
    ErrorData.Socket = Ioh;


	if( IohVtdErr & BIT0){ 
      ErrorData.ErrorSubType = IOH_VTD_90_ERR;
      ErrorData.Header.ErrorSeverity  = FATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

    if( IohVtdErr & BIT1){ 
      ErrorData.ErrorSubType = IOH_VTD_91_ERR;
      ErrorData.Header.ErrorSeverity  = FATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	if( IohVtdErr & BIT2){
      ErrorData.ErrorSubType = IOH_VTD_92_ERR;
      ErrorData.Header.ErrorSeverity  = FATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	if( IohVtdErr & BIT3){
      ErrorData.ErrorSubType = IOH_VTD_93_ERR;
      ErrorData.Header.ErrorSeverity  = FATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	if( IohVtdErr & BIT4){
      ErrorData.ErrorSubType = IOH_VTD_94_ERR;
      ErrorData.Header.ErrorSeverity  = FATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	if( IohVtdErr & BIT5){
      ErrorData.ErrorSubType = IOH_VTD_95_ERR;
      ErrorData.Header.ErrorSeverity  = FATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	if( IohVtdErr & BIT6){
      ErrorData.ErrorSubType = IOH_VTD_96_ERR;
      ErrorData.Header.ErrorSeverity  = NONFATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	if( IohVtdErr & BIT7){
      ErrorData.ErrorSubType = IOH_VTD_97_ERR;
      ErrorData.Header.ErrorSeverity  = FATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	if( IohVtdErr & BIT8){
      ErrorData.ErrorSubType = IOH_VTD_98_ERR;
      ErrorData.Header.ErrorSeverity  = FATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }

	if( IohVtdErr & BIT31){
      ErrorData.ErrorSubType = IOH_VTD_A0_ERR;
      ErrorData.Header.ErrorSeverity  = NONFATAL_ERROR_TYPE;
      CommonErrorHandling((VOID *)&ErrorData);
    }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HandleIohVtdErrors
//
// Description:
//  Event handler for Ioh Vtd errors. 
// Input:
//
// Output: 
//  VOID
//
// Modified:
//  Nothing
//
// Referrals:
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
HandleIohVtdErrors(
IN UINT8      BusNum,
IN UINT8      Ioh)
{ 
    UINT32  ErrMask;
	UINT32	Data32 = 0;
    UINT32  IohVtdErr;

    IohVtdErr = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC_NUM, R_IIO_VTUNCERRSTS));

#if RUNTIME_ERROR_DEBUG_MESSAGE
    DEBUG((-1, "IOH Vtd Error detected.\n"));
    DEBUG((-1, "HandleIohVtdErrors: IohVtdErr reg 0x%08X, Ioh No 0x%02X \n", IohVtdErr,Ioh));
#endif

    LogIohVtdError (IohVtdErr,Ioh);


      ErrMask = BIT25;

      // Clear the Fatal and Non-Fatal FERR and NERR.
      Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFFERRST));
      PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFFERRST), Data32 | ErrMask);
      Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFNERRST));
      PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GFNERRST), Data32 | ErrMask);
      Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNFERRST));
      PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNFERRST), Data32 | ErrMask);
      Data32 = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNNERRST));
      PciExpressWrite32(PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GNNERRST), Data32 | ErrMask);


    return;
  }
*/
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HandleLastBootIohQpiErrors
//
// Description:
//  Event handler for Ioh QPI errors. 
// Input:
//
// Output: 
//  VOID
//
// Modified:
//  Nothing
//
// Referrals:
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
/*
VOID
HandleLastBootIohQpiErrors(
        IN UINT8      BusNum,
        IN UINT8      Ioh)
{ 
  
  BOOLEAN FatalError = FALSE; 
  UINT32	Data32 = 0;
  UINT32  IohQpiPErr;
  UINT32  IohQpiP0Err;
  UINT32  IohQpiP1Err;
  ERROR_DATA_TYPE1    LastbootErrorData;

    DEBUG((-1, "IOH QPI Error detected.\n"));


    // First, read the global system status register.
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(CORE05_BUS_NUM, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_GSYSST));

    // Severity 2 fatal error 
    if (Data32 & V_IIO_GSYSCTL_S2STS) {
      FatalError = TRUE;
    }

    IohQpiP0Err = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP0ERRST));
    IohQpiP1Err = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IRPP1ERRST));

    LastbootErrorData.Header.ErrorType  = IohQpiSource;
    LastbootErrorData.Socket = Ioh;

    if( IohQpiP0Err & QPI_P_ERRMSK){
      LastbootErrorData.Port = 0;
    }

    if( IohQpiP1Err & QPI_P_ERRMSK){
      LastbootErrorData.Port = 1;
    }

    IohQpiPErr = (IohQpiP0Err | IohQpiP1Err);

    // Protocol errors 
    if( IohQpiPErr & B_IIO_IRPPCTL_WRI_CACHE_COR_EN){
      LastbootErrorData.ErrorSubType = IOH_QPI_B4_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Correctable;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }

    if( IohQpiPErr & B_IIO_IRPPCTL_LAY_EN){
      LastbootErrorData.ErrorSubType = IOH_QPI_C1_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Recoverable;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }

    if( IohQpiPErr & B_IIO_IPRERR_WRI_CAC_UNCOR_ECC_EN){
      LastbootErrorData.ErrorSubType = IOH_QPI_C2_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Recoverable;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }

    if( IohQpiPErr & B_IIO_IPRERR_CSR_CRO32BIT_BOUN_EN){
      LastbootErrorData.ErrorSubType = IOH_QPI_C3_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Recoverable;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }

    if( IohQpiPErr & B_IIO_IPRERR_PRO_REC_UNRES_EN){
      LastbootErrorData.ErrorSubType = IOH_QPI_D7_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Fatal;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }

    if( IohQpiPErr & B_IIO_IPRERR_PRO_QUETAB_FLOW_EN){
      LastbootErrorData.ErrorSubType = IOH_QPI_DA_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Fatal;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }

    if( IohQpiPErr & B_IIO_IRPERR_PRO_PAR_ERR_EN){
      LastbootErrorData.ErrorSubType = IOH_QPI_DB_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Fatal;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }  
   if (FatalError) {
      // Do the System reset on fatal errors, need to check all kind of errors before reset.
	  //	SystemResetOnFe ();
    }
    return;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HandleLastBootIohCoreErrors
//
// Description:
//  Event handler for Ioh Core errors. 
// Input:
//
// Output: 
//  VOID
//
// Modified:
//  Nothing
//
// Referrals:
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
HandleLastBootIohCoreErrors(
    IN UINT8      BusNum,
    IN UINT8      Ioh

)
{ 
    UINT32  IohCoreErr;
    ERROR_DATA_TYPE1    LastbootErrorData;

    DEBUG((-1, "IOH Core Error detected.\n"));

    IohCoreErr = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC2_NUM, R_IIO_IIOERRST));
    LastbootErrorData.Header.ErrorType  = IohCoreSource;
    LastbootErrorData.Socket = Ioh;

    if( IohCoreErr & IOH_CORE_C4){ 
      LastbootErrorData.ErrorSubType = IOH_CORE_C4_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Correctable;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }

    if( IohCoreErr & IOH_CORE_C5){ 
      LastbootErrorData.ErrorSubType = IOH_CORE_C5_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Correctable;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }

    if( IohCoreErr & IOH_CORE_C6){ 
      LastbootErrorData.ErrorSubType = IOH_CORE_C6_ERR;
      LastbootErrorData.Header.ErrorSeverity  = Correctable;
      CommonErrorHandling((VOID *)&LastbootErrorData);
    }
    return;
  }

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HandleLastBootIohVtdErrors
//
// Description:
//  Event handler for Ioh Vtd errors. 
// Input:
//
// Output: 
//  VOID
//
// Modified:
//  Nothing
//
// Referrals:
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
HandleLastBootIohVtdErrors(
IN UINT8      BusNum,
IN UINT8      Ioh
)
{ 

    UINT32  IohVtdErr;

    DEBUG((-1, "IOH Vtd Error detected.\n"));

    IohVtdErr = PciExpressRead32 (PCI_PCIE_ADDR(BusNum, CORE05_DEV_NUM, CORE05_FUNC_NUM, R_IIO_VTUNCERRSTS));

    LogIohVtdError (IohVtdErr,Ioh);

    return;
  }
*/
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LastBootPcieRootInformation
//
// Description:
//  Event handler for LastBoot Pcie Errors. 
// Input:
//
// Output: 
//  VOID
//
// Modified:
//  Nothing
//
// Referrals:
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
LastBootPcieRootInformation(
IN  UINT8  Bus,		
IN	UINT16 ErrorPortNumber
)
{
    AMI_PCIE_ERROR_SECTION            LastBootPciErrStruc;
	LastBootPciErrStruc.Header.ErrorType  = PciSource;
	LastBootPciErrStruc.Header.ErrorSeverity    = 0x0A;   // To Represent the Last boot Error in SELVIEW
	LastBootPciErrStruc.DeviceId.SegNum      = 0;
    LastBootPciErrStruc.DeviceId.PriBus         = Bus;

	switch (ErrorPortNumber)
    {
    	case 0 :   // DMI or Por 0
    		LastBootPciErrStruc.DeviceId.DevNum       = 0;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 0;
    		break;
    		
    	case 1 :  // PORT 1A
    	    LastBootPciErrStruc.DeviceId.DevNum       = 1;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 0;
    		break;
    		
    	case 2 :  // PORT 1B
    		LastBootPciErrStruc.DeviceId.DevNum       = 1;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 1;
    		break;
    		    		
    	case 3 :  // PORT 2A
    		LastBootPciErrStruc.DeviceId.DevNum       = 2;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 0;
    		break;
    		    		
    	case 4 :  // PORT 2B
    		LastBootPciErrStruc.DeviceId.DevNum       = 2;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 1;
    		break;
    		    		
    	case 5 :  // PORT 2C
    		LastBootPciErrStruc.DeviceId.DevNum       = 2;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 2;
    		break;
    		    		
    	case 6 :  // PORT 2D
    		LastBootPciErrStruc.DeviceId.DevNum       = 2;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 3;
    		break;
    		    		
    	case 7 :  // PORT 3A
    		LastBootPciErrStruc.DeviceId.DevNum       = 3;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 0;
    		break;
    		    		
    	case 8 :  // PORT 3B
    		LastBootPciErrStruc.DeviceId.DevNum       = 3;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 1;
    		break;
    		    		
    	case 9 :  // PORT 3C
    		LastBootPciErrStruc.DeviceId.DevNum       = 3;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 2;
    		break;
    		    		
    	case 10 : // PORT 3D
    		LastBootPciErrStruc.DeviceId.DevNum       = 3;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 3;
    		break;
    		    		
      	default :
    		LastBootPciErrStruc.DeviceId.DevNum       = 0xFF;
    		LastBootPciErrStruc.DeviceId.FuncNum     = 0xFF;
    		break;
    }
    CommonErrorHandling((VOID *)&LastBootPciErrStruc);          
    return ;
}
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HandlelastbootPcieErrors
//
// Description:
//  Primary event handler for PCI errors. Commences search for PCI error and
//  clears SMI status bits when complete. // 
//
// Input:

//
// Output: 

//
// Modified:
//  Nothing
//
// Referrals:

//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
HandlelastbootLocalErrors(
    IN  UINT8      SocketIndex
)
{
    UINT8                     SocketId;
    UINT8                           IioStack;
    LSYSST_IIO_RAS_STRUCT     LSysSt;
    LSYSCTL_IIO_RAS_STRUCT    LSysCtl;
    LFERRST_IIO_RAS_STRUCT    LFErrSt;
    LNERRST_IIO_RAS_STRUCT    LNErrSt;
    UINT8		  Bus;
    UINT16      ErrorPortNumber;
  
    if(SocketIndex >= MAX_SOCKET) {
      return;
    }

    SocketId = mRasTopology->SystemInfo.SocketInfo[SocketIndex].UncoreIioInfo.SocketId;
    Bus = mRasTopology->SystemInfo.SocketInfo[SocketIndex].UncoreIioInfo.IIOBusBase;

    for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
        
        LSysSt.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, IioStack, LSYSST_IIO_RAS_REG);
        LSysCtl.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, IioStack, LSYSCTL_IIO_RAS_REG);
        if (LSysCtl.Data & LSysSt.Data) {

      //
      // Local IIO Error Status
      //
      LFErrSt.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, IioStack, LFERRST_IIO_RAS_REG); 
      LNErrSt.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, IioStack, LNERRST_IIO_RAS_REG); 

	
	if ((LFErrSt.Bits.dmi)  || (LNErrSt.Bits.dmi)) { // DMI Port
		ErrorPortNumber = 0 ;
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
	
	if ((LFErrSt.Bits.pcie0)  || (LNErrSt.Bits.pcie0)) {
		ErrorPortNumber = 0 ;  // Port 0 
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
		
	if ((LFErrSt.Bits.pcie1)  || (LNErrSt.Bits.pcie1)) {
		ErrorPortNumber = 1 ; //  Port 1A 
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
			
	if ((LFErrSt.Bits.pcie2)  || (LNErrSt.Bits.pcie2)) {
		ErrorPortNumber = 2 ;  // Port 1B
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}

	if ((LFErrSt.Bits.pcie3)  || (LNErrSt.Bits.pcie3)) {
		ErrorPortNumber = 3 ;  // Port 2A
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
			
	if ((LFErrSt.Bits.pcie4)  || (LNErrSt.Bits.pcie4)) {
		ErrorPortNumber = 4 ;  // Port 2B
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
				
	if ((LFErrSt.Bits.pcie5)  || (LNErrSt.Bits.pcie5)) {
		ErrorPortNumber = 5 ;  // Port 2C
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}

	if ((LFErrSt.Bits.pcie6)  || (LNErrSt.Bits.pcie6)) {
		ErrorPortNumber = 6 ; // Port 2D
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
						
	if ((LFErrSt.Bits.pcie7)  || (LNErrSt.Bits.pcie7)) {
		ErrorPortNumber = 7 ; // Port 3A
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
							
	if ((LFErrSt.Bits.pcie8)  || (LNErrSt.Bits.pcie8)) {
		ErrorPortNumber = 8 ; // Port 3B
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
								
	if ((LFErrSt.Bits.pcie9)  || (LNErrSt.Bits.pcie9)) {
		ErrorPortNumber = 9 ;  // Port 3C
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}
		
	if ((LFErrSt.Bits.pcie10)  || (LNErrSt.Bits.pcie10)) {
		ErrorPortNumber = 10 ;  // Port 3D
		LastBootPcieRootInformation(Bus,ErrorPortNumber);
	}

// TO DO: Add the support
//    if( LFErrSt.Bits.tras_csi0 | LFErrSt.Bits.tras_csi1 | LNErrSt.Bits.csi0_err | LNErrSt.Bits.csi1_err ) {
//                        HandleLastBootIohQpiErrors(Bus, SocketIndex );
//    }
//    if( LFErrSt.Bits.ioh | LNErrSt.Bits.ioh ) {
//                        HandleLastBootIohCoreErrors(Bus, SocketIndex );
//    }
//    if( LFErrSt.Bits.vtd | LNErrSt.Bits.vtd ) {
//        HandleLastBootIohVtdErrors(Bus, SocketIndex );
//    }
    }
  }
  return ;
	
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HandleLastBootIohErrors
//
// Description:
//  Handler for IOH Errors.
//
// Input:

// Output:
//  VOID
//
// Modified:
//  Nothing

// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
LastBootIohErrorHandler()
{
    UINT8                           SocketIndex;
    UINT8                           SocketId;
    IIO_GSYSST_UBOX_CFG_STRUCT      GSysSt;
    IIO_GF_ERRST_UBOX_CFG_STRUCT    GFErrSt;
    IIO_GNF_ERRST_UBOX_CFG_STRUCT   GNErrSt;
    IIO_GSYSCTL_UBOX_CFG_STRUCT     GSysCtl;

    for ( SocketIndex =0; SocketIndex < MAX_SOCKET; SocketIndex++) {
      if(mRasTopology->SystemInfo.SocketInfo[SocketIndex].UncoreIioInfo.Valid) {
        SocketId = mRasTopology->SystemInfo.SocketInfo[SocketIndex].UncoreIioInfo.SocketId;

        GSysSt.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, IIO_GSYSST_UBOX_CFG_REG);
        GSysCtl.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, IIO_GSYSCTL_UBOX_CFG_REG);
        if (GSysCtl.Data & GSysSt.Data) {
            //
              // Check Global IIO Error Status
            //
              GFErrSt.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, IIO_GF_ERRST_UBOX_CFG_REG) & IIO_GLBL_ERR_BITMASK;
              GNErrSt.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, IIO_GNF_ERRST_UBOX_CFG_REG) & IIO_GLBL_ERR_BITMASK;
              if ( GFErrSt.Data | GNErrSt.Data ) {
                      //
                      // Local IIO Error Status
                      //
                      HandlelastbootLocalErrors(SocketIndex);
              }
        }
    } //if
  } // for

}

VOID ClearMSR (VOID *Buffer)
{
	UINT32  Ia32McgCap = (UINT32) AsmReadMsr64(EFI_IA32_MCG_CAP);
	UINT8   Count = (UINT8) Ia32McgCap;
	UINT8   i;
	
	// Clear all MC bank registers.
	if (1) { //Power-on reset detected
	        //Clear all errors
		for(i = 0; i < Count; ++i) AsmWriteMsr64(EFI_IA32_MC0_STATUS + 4 * i, 0); //Write 0 to IA32_MCi_STATUS
	} 
	
}
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LastBootErrorHandler
//
// Description:
//  LastBoot Error Handler.
//
// Input:

// Output:
//  VOID
//
// Modified:
//  Nothing

// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
LastBootErrorHandler()
{
	// ****** Porting  ************

	
	UINTN       thread;
    UINT8                           node;

    mRasTopology->SystemInfo.LastBootErrorFlag = TRUE;
    mRasTopology->SystemInfo.SkipFillOtherMcBankInfoFlag = FALSE;
//#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
    RASDEBUG ((DEBUG_ERROR, "Ioherror L1011\n"));
    LogMemorylastbooterror();
    RASDEBUG ((DEBUG_ERROR, "Ioherror L1013\n"));
//#endif
	ProcessSocketMcBankError ();
	mRasTopology->SystemInfo.LastBootErrorFlag = FALSE;

	 for (thread = 0; thread < gSmst->NumberOfCpus; thread++) {
	     gSmst->SmmStartupThisAp (ClearMSR,
		                             thread, 
		                             NULL);
	 }
	 ClearMSR(&thread);

	  for (node = 0; node < MC_MAX_NODE; node++) {
	    if (mMemRas->SystemMemInfo->Socket[NODE_TO_SKT(node)].imcEnabled[NODE_TO_MC(node)] == 0 ) continue;
			//
			// Clear MC5_STATUS_SHADOW
			//
	        mCpuCsrAccess->WriteCpuCsr(NODE_TO_SKT(node), NODE_TO_MC(node), MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG, 0);
	        mCpuCsrAccess->WriteCpuCsr(NODE_TO_SKT(node), NODE_TO_MC(node), MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG, 0);
	  }

	// Handle Last Boot IOH errors 
	LastBootIohErrorHandler();

	// ****** Porting  ************

	return ;
}



#pragma optimize( "", on )

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
