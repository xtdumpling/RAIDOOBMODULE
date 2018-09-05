//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//
//
//  Rev. 1.06
//   Bug Fixed:  Fixed system may occur IERR when injecting PCIE error.
//   Reason:     Remove workaround.
//   Auditor:    Chen Lin
//   Date:       May/08/2017
// 
//
//  Rev. 1.05
//   Bug Fixed:  Fixed system will hang up and no log when inject SERR/PERR with H0 CPU 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Mar/13/2017
//
//
//  Rev. 1.04
//   Bug Fixed:  Fixed that PERR could not work.. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Nov/11/2016
//
//  Rev. 1.03
//   Bug Fixed:  Fixe Memory Map out feature could not work. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//  
//  Rev. 1.02
//      Bug Fixed:  Fix cannot log SERR/PERR. 
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Sep/09/2016
// 
//  
//  Rev. 1.01
//    Bug Fix :  Let MECI (Multiple Event Count),METW (Multiple Event Time Window) can work with Memory correct error report and Pcie error report.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Aug/23/2016
//
//  Rev. 1.00
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
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
// Name:		RtErrorLogBoard.c
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorlogBoard.h"

#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
#include <SspTokens.h>
#include "SmcLibCommon.h" 
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Guid/SetupVariable.h>
#include <Protocol\SmmVariable.h>
#include <Guid/AmiGlobalVariable.h>
//BOOLEAN   mPERRlog = FALSE;
#endif

EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;
UINT8  				ErrorSource;
UINT8                           mErrorSeverity;

//---------------------------------------------------------------------------
// External (eLink) handler functions
//---------------------------------------------------------------------------

typedef EFI_STATUS (MEM_ERROR_HANDLER_FUNCTION) (
        AMI_MEMORY_ERROR_SECTION		*MemInfo
);

extern MEM_ERROR_HANDLER_FUNCTION MEM_ERROR_HANDLER_FUNCTION_LIST EndOfHandlerList;

MEM_ERROR_HANDLER_FUNCTION* MemErrorHandlerList[] = {
		MEM_ERROR_HANDLER_FUNCTION_LIST
	NULL
};

typedef EFI_STATUS (PROCESSOR_ERROR_HANDLER_FUNCTION) (
        AMI_PROCESSOR_GENERIC_ERROR_SECTION        *ProcInfo
);

extern PROCESSOR_ERROR_HANDLER_FUNCTION PROCESSOR_ERROR_HANDLER_FUNCTION_LIST EndOfProcessorHandlerList;

PROCESSOR_ERROR_HANDLER_FUNCTION* ProcessorErrorHandlerList[] = {
        PROCESSOR_ERROR_HANDLER_FUNCTION_LIST
    NULL
};

typedef EFI_STATUS (IOHQPI_ERROR_HANDLER_FUNCTION) (
    IN UINT16   ErrorType,
    IN UINT8    ErrorSubType,
    IN UINT8    ErrorSeverity,
    IN UINT8    Socket,
    IN UINT8    Port
);

extern IOHQPI_ERROR_HANDLER_FUNCTION IOHQPI_ERROR_HANDLER_FUNCTION_LIST IohQpiEndOfHandlerList;

IOHQPI_ERROR_HANDLER_FUNCTION* IohQpiErrorHandlerList[] = {
		IOHQPI_ERROR_HANDLER_FUNCTION_LIST
	NULL
};

typedef EFI_STATUS (IOHCORE_ERROR_HANDLER_FUNCTION) (
    IN UINT16   ErrorType,
    IN UINT8    ErrorSubType,
    IN UINT8    ErrorSeverity,
    IN UINT8    Socket    
);

extern IOHCORE_ERROR_HANDLER_FUNCTION IOHCORE_ERROR_HANDLER_FUNCTION_LIST IohCoreEndOfHandlerList;

IOHCORE_ERROR_HANDLER_FUNCTION* IohCoreErrorHandlerList[] = {
		IOHCORE_ERROR_HANDLER_FUNCTION_LIST
	NULL
};

typedef EFI_STATUS (IOHVTD_ERROR_HANDLER_FUNCTION) (
		    IN UINT8    Socket,
		    IN UINT8    Stack,
		    IN UINT16   ErrorType,
		    IN UINT8    ErrorSubType,
		    IN UINT8    ErrorSeverity
);

extern IOHVTD_ERROR_HANDLER_FUNCTION IOHVTD_ERROR_HANDLER_FUNCTION_LIST IohVtdEndOfHandlerList;

IOHVTD_ERROR_HANDLER_FUNCTION* IohVtdErrorHandlerList[] = {
	IOHVTD_ERROR_HANDLER_FUNCTION_LIST
	NULL
};

typedef EFI_STATUS (ITC_ERROR_HANDLER_FUNCTION) (
		    IN UINT8    Socket,
		    IN UINT8    Stack,
		    IN UINT16   ErrorType,
		    IN UINT8    ErrorSubType,
		    IN UINT8    ErrorSeverity
);
extern ITC_ERROR_HANDLER_FUNCTION ITC_ERROR_HANDLER_FUNCTION_LIST ItcEndOfErrorHandlerList;

ITC_ERROR_HANDLER_FUNCTION* ItcErrorHandlerList[] = {
	ITC_ERROR_HANDLER_FUNCTION_LIST
	NULL
};

typedef EFI_STATUS (OTC_ERROR_HANDLER_FUNCTION) (
		    IN UINT8    Socket,
		    IN UINT8    Stack,
		    IN UINT16   ErrorType,
		    IN UINT8    ErrorSubType,
		    IN UINT8    ErrorSeverity
);
extern OTC_ERROR_HANDLER_FUNCTION OTC_ERROR_HANDLER_FUNCTION_LIST OtcEndOfErrorHandlerList;

OTC_ERROR_HANDLER_FUNCTION* OtcErrorHandlerList[] = {
	OTC_ERROR_HANDLER_FUNCTION_LIST
	NULL
};

typedef EFI_STATUS (DMA_ERROR_HANDLER_FUNCTION) (
		    IN UINT8    Socket,
		    IN UINT8    Stack,
		    IN UINT16   ErrorType,
		    IN UINT8    ErrorSubType,
		    IN UINT8    ErrorSeverity
);
extern DMA_ERROR_HANDLER_FUNCTION DMA_ERROR_HANDLER_FUNCTION_LIST DmaEndOfErrorHandlerList;

DMA_ERROR_HANDLER_FUNCTION* DmaErrorHandlerList[] = {
	DMA_ERROR_HANDLER_FUNCTION_LIST
	NULL
};

typedef EFI_STATUS (PCI_ERROR_HANDLER_FUNCTION) (
        AMI_PCIE_ERROR_SECTION				*PcieInfo
);

extern PCI_ERROR_HANDLER_FUNCTION PCI_ERROR_HANDLER_FUNCTION_LIST PciEndOfHandlerList;

PCI_ERROR_HANDLER_FUNCTION* PciErrorHandlerList[] = {
		PCI_ERROR_HANDLER_FUNCTION_LIST
    NULL
};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogMemErrorRecoed
//
// Description: 
//
// Parameters:  
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogMemErrorRecord (
        AMI_MEMORY_ERROR_SECTION                  *MemInfo
  )
{

    // Call each handler function.
    {
       	UINTN i;
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
    UINT8	Socket, DdrChannel, Dimm;
    UINT8	MapOutDIMM = 0, MemoryControllerId;
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_SMM_VARIABLE_PROTOCOL         *mSmmVariablePtr;
    UINT32        AttributesRead;
    UINTN Size = sizeof(UINT32);
    UINT32 MapOutRTFlag;

    Status = gSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, &mSmmVariablePtr);
    DEBUG((EFI_D_ERROR, "MemInfo->Socket = %x.\n", NODE_TO_SKT(MemInfo->Node)));
    DEBUG((EFI_D_ERROR, "MemInfo->Node = %x.\n", MemInfo->Node));
    DEBUG((EFI_D_ERROR, "MemInfo->Channel = %x.\n", MemInfo->Card));
    DEBUG((EFI_D_ERROR, "MemInfo->Dimm = %x.\n", MemInfo->Module));
    DEBUG((EFI_D_ERROR, "LastBootErrorFlag = %x.\n", mPlatformRasPolicyProtocol->EfiRasSystemTopology->SystemInfo.LastBootErrorFlag));
    DEBUG((EFI_D_ERROR, "ErrorSeverity = %x.\n", MemInfo->Header.ErrorSeverity));
    
    // Only write map out cmos at runtme. 
    if(( MemInfo->Header.ErrorSeverity != EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) && !mPlatformRasPolicyProtocol->EfiRasSystemTopology->SystemInfo.LastBootErrorFlag ){   
    Status = mSmmVariablePtr->SmmGetVariable (
                                L"MemMapout",
                                &gSmcMemMapOutGuid,
                                &AttributesRead,
                                &Size,
                                &MapOutRTFlag
                                );                
	  Socket = (UINT8)(NODE_TO_SKT(MemInfo->Node));
	  DdrChannel = (UINT8)(NODE_TO_MC(MemInfo->Node)) * 3 + (UINT8)(MemInfo->Card);
	  Dimm = (UINT8)(MemInfo->Module);
	  
      DEBUG((EFI_D_ERROR, " Socket = %x.\n", Socket));
      DEBUG((EFI_D_ERROR, " DdrChannelID = %x.\n", DdrChannel));
      DEBUG((EFI_D_ERROR, " Dimm = %x.\n", Dimm));
       
	  MapOutDIMM = 0x80 | ((Socket & 0x03) << 4) | ((DdrChannel & 0x07) << 1) | (Dimm & 0x01);
      //set cmos
      SetCmosTokensValue (MEM_MAPOUT, MapOutDIMM);
  	  MapOutRTFlag = 0xAB;

      Status = mSmmVariablePtr->SmmSetVariable (
                                L"MemMapout",
                                &gSmcMemMapOutGuid,
                                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                Size,
                                &MapOutRTFlag
                                );                                      

    //ASSERT_EFI_ERROR (Status);
	            	     
   }//ifMemInfo->Header.ErrorSeverity != EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED)
   
   // Override MeInfo because this DIMM has been mapped out.
   if( mPlatformRasPolicyProtocol->EfiRasSystemTopology->SystemInfo.LastBootErrorFlag && (MemInfo->Header.ErrorSeverity != EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) ){
      MapOutDIMM = GetCmosTokensValue(MEM_MAPOUT);
      if ( 	MapOutDIMM & 0x80  ){    
        DEBUG((EFI_D_ERROR," MapOutDIMM = 0x%x.\n", MapOutDIMM));    
        Socket = (MapOutDIMM >> 4) & 0x03;    
        MemoryControllerId =  (MapOutDIMM >> 6) & 0x1;
        DEBUG((EFI_D_ERROR, "MCId = %x.\n", MemoryControllerId));
        MemInfo->Node =  SKTMC_TO_NODE(Socket,MemoryControllerId);
        MemInfo->Card = (UINT8)  ( ( (MapOutDIMM >> 1) & 0x07) - MemoryControllerId*3 );
        MemInfo->Module = MapOutDIMM & 0x01;
        
        DEBUG((EFI_D_ERROR, "Overrdie MemInfo->Node = 0x%x.\n", MemInfo->Node));
        DEBUG((EFI_D_ERROR, "Overrdie MemInfo->Channel = 0x%x.\n", MemInfo->Card));
        DEBUG((EFI_D_ERROR, "Overrdie MemInfo->Dimm = 0x%x.\n", MemInfo->Module));
      } else {
        // no DIMM has been mappedout.Don't log because MRC has UCE and mapout DIMM during POST
        DEBUG((EFI_D_ERROR," MRC has UCE during POST,ret\n"));    
        return EFI_SUCCESS;
      }
   }   
#endif  // #if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT

#if SMCPKG_SUPPORT
        for(i=0; MemErrorHandlerList[i]; i++) {
            
        }    
#endif 	  
	    for(i=0; MemErrorHandlerList[i]; i++) {
            Status = MemErrorHandlerList[i](MemInfo); //SMC
#if SMCPKG_SUPPORT 	 
            if (  Status == EFI_ABORTED && SmcLogMemErrorToGpnv ==  MemErrorHandlerList[i]){
                DEBUG((EFI_D_ERROR,"already log during METW, ret\n"));
                return EFI_SUCCESS;
            }      
#endif // #if SMCPKG_SUPPORT
        }
    }

  return EFI_SUCCESS;
}

UINT8 
AmiPciErrLibGetCapabilitiesOffset (
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function,
  IN      UINT8   Cap
  )
{
  UINT8   PciPrimaryStatus;
  UINT8   CapabilityOffset;
  UINT8   CapId;

  PciPrimaryStatus = PciExpressRead8 (PCI_PCIE_ADDR (Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if (PciPrimaryStatus & EFI_PCI_STATUS_CAPABILITY) {
    CapabilityOffset = PciExpressRead8 (PCI_PCIE_ADDR (Bus, Device, Function, EFI_PCI_CAPABILITY_PTR));

    do {
      CapId = PciExpressRead8 (PCI_PCIE_ADDR (Bus, Device, Function, CapabilityOffset));
      if (CapId == Cap) {
        break;
      }
      CapabilityOffset = PciExpressRead8 (PCI_PCIE_ADDR (Bus, Device, Function, CapabilityOffset + 1));
    } while (CapabilityOffset != 0);
  } else {
    CapabilityOffset = 0;
  }
  return CapabilityOffset;
}

#if SMCPKG_SUPPORT
EFI_STATUS
WorkAroundSerrPerr(
   AMI_PCIE_ERROR_SECTION   *PciErrorData
){


    UINT16    Data16;
    UINT32    Data32;
    UINT8	  SecBus,TmpBus,TmpDev,TmpFun;
    UINT16    SecStatus = 0;
    UINT16    PciStatus = 0;
    UINT8     Bus, Dev, Func; 
    EFI_STATUS          Status = EFI_SUCCESS;

   DEBUG((-1, "WorkAroundSerrPerr Entry \n"));
    
    Bus = PciErrorData->DeviceId.PriBus;
    Dev = PciErrorData->DeviceId.DevNum;
    Func= PciErrorData->DeviceId.FuncNum;
    
  
    // Determine what sort of error occurred.
    Data16 = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Dev, Func, PCI_PRIMARY_STATUS_OFFSET));
    DEBUG((-1, "Data16 = %04x, Bus = %02x, Device = %02x, Function = %02x\n", Data16, Bus, Dev, Func));
    Data16 &= (PCI_STS_MSTR_DATA_PERR | PCI_STS_SIG_SERR | PCI_STS_DET_PERR);

    
    // workaround for ASC 293920LPE 
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Dev, Func, 0));
    DEBUG((-1, "VID DID = %04x\n",Data32));
    if ( Data16 && ( (UINT32) ASC_293920LPE_VID_DID == Data32 )){
         SecStatus = 0;
         DEBUG((-1, "do workaround for ASC 293920LPE \n"));
         //Find the brigde for it
         for (TmpBus = Bus-1 ; TmpBus >= 0; TmpBus-- ){
             for (TmpDev =0;  TmpDev <= 31 ; TmpDev++ ){
                for (TmpFun =0;  TmpFun <= 7 ; TmpFun++ ){
                   if ( PciExpressRead16 (PCI_PCIE_ADDR(TmpBus, TmpDev, TmpFun,0 )) == 0xFFFF){
                     continue;
                   } 
                   if ( PciExpressRead16 ( PCI_PCIE_ADDR(TmpBus, TmpDev, TmpFun, PCI_SCC ) ) == PCI_BRIDGE_CLASS_CODE) {
                     SecBus = PciExpressRead8 ( PCI_PCIE_ADDR(TmpBus, TmpDev, TmpFun, PCI_SBUS));
                     if ( SecBus == Bus){
                      // find it
                         DEBUG((-1, "Find Bridge Bus = %02x, Device = %02x, Function = %02x\n", TmpBus, TmpDev, TmpFun));
                      //check secondary bus status 0x1E    
                         SecStatus = PciExpressRead16 ( PCI_PCIE_ADDR(TmpBus, TmpDev, TmpFun, PCI_SECSTATUS) ) ;  
                         PciStatus = PciExpressRead16 ( PCI_PCIE_ADDR(TmpBus, TmpDev, TmpFun, PCI_PRIMARY_STATUS_OFFSET) ) ;                          
                         DEBUG((-1, "SecStatus = %02x\n",SecStatus));
                         DEBUG((-1, "PciStatus = %02x\n",PciStatus));
                         if( ( SecStatus & PCI_STS_DET_PERR) || ( PciStatus & PCI_STS_MASTER_DATA_PARITY_ERROR) ){
                            //This is PERR
                             DEBUG((-1, "Override ErrorType to PERR \n"));
                             PciErrorData->ErrorType = PERROR_TYPE;
                         } else {
                             DEBUG((-1, "Overrride ErrorType to SERR \n"));
                             PciErrorData->ErrorType = SERROR_TYPE;
                         }
                         goto Exit;
                         break;
                     }                   
                   }  
                }     
             }    
         }
         
         
     } else {
        DEBUG((-1, "Not Support\n"));
        Status = EFI_UNSUPPORTED;
     }
   Exit:   
   DEBUG((-1, "WorkAroundSerrPerr Exit \n"));  
   return Status;
}
#endif

EFI_STATUS
LogPciErrorRecord (
        AMI_PCIE_ERROR_SECTION   *PciErrorData
    )
{
#if SMCPKG_SUPPORT    
    EFI_STATUS    Status;
    UINT8         PciDevFun = 0;  
#endif // #if SMCPKG_SUPPORT
    UINT16                   ErrorMap;
    UINT16                   PrimaryStatus;
    UINT8                     CapabilityOffset;
    // APTIOV_SERVER_OVERRIDE_RC_START
    UINT16 					 Data16;
    // APTIOV_SERVER_OVERRIDE_RC_END

    if (PciErrorData->Header.ErrorSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) {
        PciErrorData->ErrorType = COR_ERROR_TYPE;
    }
    else if (PciErrorData->Header.ErrorSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE) {
        PciErrorData->ErrorType = NONFATAL_ERROR_TYPE;
    }
    else if (PciErrorData->Header.ErrorSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_FATAL) {
        ErrorMap = PCI_STS_MSTR_DATA_PERR | PCI_STS_SIG_SERR | PCI_STS_DET_PERR | PCI_STS_REC_TRGT_ABRT;
        PrimaryStatus = PciExpressRead16 (PCI_PCIE_ADDR (PciErrorData->DeviceId.PriBus, PciErrorData->DeviceId.DevNum, PciErrorData->DeviceId.FuncNum, PCI_PRIMARY_STATUS_OFFSET));
        if (PrimaryStatus & ErrorMap) {
            if (PrimaryStatus & PCI_STS_SIG_SERR) {
                PciErrorData->ErrorType = SERROR_TYPE;
            }
            else {
                PciErrorData->ErrorType = PERROR_TYPE;
            }
          #if SMCPKG_SUPPORT 
            WorkAroundSerrPerr(PciErrorData);
          #endif             
        }
       #if SMCPKG_SUPPORT == 0
        else {
            PciErrorData->ErrorType = FATAL_ERROR_TYPE;
        }
      #endif 
    }
    // APTIOV_SERVER_OVERRIDE_RC_START
    // 
    //  Co-processor Devices returns wrong Primary/Secondary/Sub Bus.
    //
    Data16 = PciExpressRead16 (PCI_PCIE_ADDR(PciErrorData->DeviceId.PriBus, PciErrorData->DeviceId.DevNum, PciErrorData->DeviceId.FuncNum, PCI_VENDOR_ID_OFFSET));
    if (Data16 == 0xFFFF) return EFI_SUCCESS;
    // APTIOV_SERVER_OVERRIDE_RC_END
    CapabilityOffset = AmiPciErrLibGetCapabilitiesOffset (PciErrorData->DeviceId.PriBus, PciErrorData->DeviceId.DevNum, PciErrorData->DeviceId.FuncNum, EFI_PCI_CAPABILITY_ID_PCIEXP);

    PciErrorData->PcieDeviceStatus = PciExpressRead16 (PCI_PCIE_ADDR (PciErrorData->DeviceId.PriBus, PciErrorData->DeviceId.DevNum, PciErrorData->DeviceId.FuncNum, CapabilityOffset + PCIE_DEVICE_STATUS_OFFSET));
    PciErrorData->XpCorErrSts = PciExpressRead32 (PCI_PCIE_ADDR (PciErrorData->DeviceId.PriBus, PciErrorData->DeviceId.DevNum, PciErrorData->DeviceId.FuncNum, ONLY_REGISTER_OFFSET (XPCORERRSTS_IIO_PCIE_REG)));
    PciErrorData->XpUncErrSts = PciExpressRead32 (PCI_PCIE_ADDR (PciErrorData->DeviceId.PriBus, PciErrorData->DeviceId.DevNum, PciErrorData->DeviceId.FuncNum, ONLY_REGISTER_OFFSET (XPUNCERRSTS_IIO_PCIE_REG)));
   

#if SMCPKG_SUPPORT
//    if (  PciErrorData->ErrorType == PERROR_TYPE && !mPERRlog){
//          DEBUG((EFI_D_ERROR,"WA for PERR\n"));
//          LogPciErrorToSel(PciErrorData);
//  
//          DEBUG((EFI_D_ERROR,"Set PERR_WA in CMOS\n"));
//          PciDevFun = (  PciErrorData->DeviceId.DevNum << 3 )| ( PciErrorData->DeviceId.FuncNum & 0x7 ) ;
//          SetCmosTokensValue (PERR_WA_H, PciErrorData->DeviceId.PriBus);
//          SetCmosTokensValue (PERR_WA_L, PciDevFun);
//
//          mPERRlog = TRUE;   
//          SmcLogPciErrorToGpnv(PciErrorData);  
//          DEBUG((EFI_D_ERROR,"Cls PERR_WA in CMOS\n"));   
//          SetCmosTokensValue (PERR_WA_H, 0);
//          SetCmosTokensValue (PERR_WA_L, 0);              
//    }
//    else{            
 
         // Call each handler function.
         {//local loop
            UINTN i;
            
            for(i=0; PciErrorHandlerList[i]; i++) {
                Status = PciErrorHandlerList[i] ( // SMC
                        PciErrorData
                 );
                if ( Status == EFI_ABORTED && SmcLogPciErrorToGpnv ==  PciErrorHandlerList[i]){
                    DEBUG((EFI_D_ERROR,"already log during METW, ret\n"));
                    return EFI_SUCCESS;
                }      
            }// for
         }//local loop
//       } //.........END of if (  PciErrorData->ErrorType == PERROR_TYPE && !mPERRlog){
   
#else 
       // Call each handler function.
        {//local loop
            UINTN i;
            
            for(i=0; PciErrorHandlerList[i]; i++) {
                PciErrorHandlerList[i] ( 
                        PciErrorData
                 );     
            }// for
        }//local loop
#endif  // .. END of ..//#if SMCPKG_SUPPORT

    return EFI_SUCCESS;

}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogProcessorErrorRecord
//
// Description: 
//
// Parameters:  
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogProcessorErrorRecord (
        AMI_PROCESSOR_GENERIC_ERROR_SECTION                  *ProcInfo
  )
{

    // Call each handler function.
    {
        UINTN i;
        for(i=0; ProcessorErrorHandlerList[i]; i++) {
            ProcessorErrorHandlerList[i] (
              ProcInfo
            );
        }
    }

  return EFI_SUCCESS;
}

EFI_STATUS
LogIohQpiErrorRecord (
        IOH_QPI_COMBINED_ERROR_RECORD *IohQpiErrorData
        ){

        // Call each handler function.
        {//local loop

            UINTN i;
            
            for(i=0; IohQpiErrorHandlerList[i]; i++) {
                IohQpiErrorHandlerList[i] (
                    IohQpiErrorData ->IohQpiErrorRecord.Socket,
                    IohQpiErrorData ->IohQpiErrorRecord.Port,
                    IohQpiErrorData ->IohQpiErrorRecord.Header.ErrorType,
                    IohQpiErrorData ->IohQpiErrorRecord.ErrorSubType,
                    IohQpiErrorData ->IohQpiErrorRecord.Header.ErrorSeverity
                );
            }// for
        }//local loop

    return EFI_SUCCESS;

}

EFI_STATUS
LogIohCoreErrorRecord (
        IOH_CORE_COMBINED_ERROR_RECORD *IohCoreErrorData
        ){

        // Call each handler function.
        {//local loop
            UINTN i;
            
            for(i=0; IohCoreErrorHandlerList[i]; i++) {
                IohCoreErrorHandlerList[i] (
                    IohCoreErrorData ->IohCoreErrorRecord.Socket,
                    IohCoreErrorData ->IohCoreErrorRecord.Header.ErrorType,
                    IohCoreErrorData ->IohCoreErrorRecord.ErrorSubType,
                    IohCoreErrorData ->IohCoreErrorRecord.Header.ErrorSeverity
                );
            }// for
        }//local loop

    return EFI_SUCCESS;
}

EFI_STATUS
LogIohVtdErrorRecord (
        IOH_VTD_COMBINED_ERROR_RECORD *IohVtdErrorData
        ){

        // Call each handler function.
        {//local loop
            UINTN i;
            
            for(i=0; IohVtdErrorHandlerList[i]; i++) {
                IohVtdErrorHandlerList[i] (
                    IohVtdErrorData ->IohVtdErrorRecord.Socket,
                    IohVtdErrorData ->IohVtdErrorRecord.Stack,
                    IohVtdErrorData ->IohVtdErrorRecord.Header.ErrorType,
                    IohVtdErrorData ->IohVtdErrorRecord.ErrorSubType,
                    IohVtdErrorData ->IohVtdErrorRecord.Header.ErrorSeverity
                );
            }// for
        }//local loop

    return EFI_SUCCESS;
}

EFI_STATUS
LogItcErrorRecord (
		ITC_COMBINED_ERROR_RECORD *ItcErrorData
        ){

        // Call each handler function.
        {//local loop
            UINTN i;
            
            for(i=0; ItcErrorHandlerList[i]; i++) {
        	    ItcErrorHandlerList[i] (
                		ItcErrorData->ItcErrorRecord.Socket,
                		ItcErrorData->ItcErrorRecord.Stack,
                		ItcErrorData->ItcErrorRecord.Header.ErrorType,
                		ItcErrorData->ItcErrorRecord.ErrorSubType,
                		ItcErrorData->ItcErrorRecord.Header.ErrorSeverity
                );
            }// for
        }//local loop

    return EFI_SUCCESS;
}

EFI_STATUS
LogOtcErrorRecord (
		OTC_COMBINED_ERROR_RECORD *OtcErrorData
        ){

        // Call each handler function.
        {//local loop
            UINTN i;
            
            for(i=0; OtcErrorHandlerList[i]; i++) {
        	    OtcErrorHandlerList[i] (
                		OtcErrorData->OtcErrorRecord.Socket,
                		OtcErrorData->OtcErrorRecord.Stack,
                		OtcErrorData->OtcErrorRecord.Header.ErrorType,
                		OtcErrorData->OtcErrorRecord.ErrorSubType,
                		OtcErrorData->OtcErrorRecord.Header.ErrorSeverity
                );
            }// for
        }//local loop

    return EFI_SUCCESS;
}

EFI_STATUS
LogDmaErrorRecord (
		DMA_COMBINED_ERROR_RECORD *DmaErrorData
        ){

        // Call each handler function.
        {//local loop
            UINTN i;
            
            for(i=0; DmaErrorHandlerList[i]; i++) {
        	    DmaErrorHandlerList[i] (
                		DmaErrorData->DmaErrorRecord.Socket,
                		DmaErrorData->DmaErrorRecord.Stack,
                		DmaErrorData->DmaErrorRecord.Header.ErrorType,
                		DmaErrorData->DmaErrorRecord.ErrorSubType,
                		DmaErrorData->DmaErrorRecord.Header.ErrorSeverity
                );
            }// for
        }//local loop

    return EFI_SUCCESS;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   CommonErrorHandling
//
// Description: 
//
// Parameters:  
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
CommonErrorHandling (
  VOID  *ErrorDataStructure
  )
{

  EFI_STATUS  	Status = EFI_SUCCESS;

  Status = gSmst->SmmLocateProtocol (
                  &gEfiPlatformRasPolicyProtocolGuid,
                  NULL,
                  &mPlatformRasPolicyProtocol
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
      mPlatformRasPolicyProtocol->SystemRasSetup->PoisonEn = 0;
      mPlatformRasPolicyProtocol->EfiRasSystemTopology->SystemInfo.LastBootErrorFlag = FALSE;
  }

  ErrorSource = *(UINT8 *)ErrorDataStructure;
  mErrorSeverity = *((UINT8 *)ErrorDataStructure + 1);

  switch (ErrorSource)  {
  case MemSource:
    LogMemErrorRecord ((AMI_MEMORY_ERROR_SECTION *)ErrorDataStructure);
    break;

  case PciSource:
    LogPciErrorRecord ((AMI_PCIE_ERROR_SECTION *)ErrorDataStructure);
    break;

  case ProcSource:
    LogProcessorErrorRecord ((AMI_PROCESSOR_GENERIC_ERROR_SECTION *)ErrorDataStructure);
    break;
    
  case IohQpiSource:
    LogIohQpiErrorRecord ((IOH_QPI_COMBINED_ERROR_RECORD *)ErrorDataStructure);
    break;

  case IohCoreSource:
    LogIohCoreErrorRecord ((IOH_CORE_COMBINED_ERROR_RECORD *)ErrorDataStructure);
    break;

  case IohVtdSource:
    LogIohVtdErrorRecord ((IOH_VTD_COMBINED_ERROR_RECORD *)ErrorDataStructure);
    break;

  case IohItcSource:
    LogItcErrorRecord  ((ITC_COMBINED_ERROR_RECORD *)ErrorDataStructure);
    break;

  case IohOtcSource:
    LogOtcErrorRecord  ((OTC_COMBINED_ERROR_RECORD *)ErrorDataStructure);
    break;

  case IohDmaSource:
    LogDmaErrorRecord  ((DMA_COMBINED_ERROR_RECORD *)ErrorDataStructure);
    break;

  default:
    break;

  }
  return Status;
}

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
