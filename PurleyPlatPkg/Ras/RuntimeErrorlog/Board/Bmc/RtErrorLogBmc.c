//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2017, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
//
//  Rev. 1.05
//    Bug Fix : Avoid too many memory UCE log in BMC event log at the same time.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Mar/06/2017
//
//  Rev. 1.04
//    Bug Fix : Remove unnessary delay.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Dec/21/2016
// 
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
//  Rev. 1.01
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
//  Rev. 1.00
//    Bug Fix : Update ECC/PCI SMBIOS event log. (From Grantley)
//    Reason  : 
//    Auditor : Ivern Yeh
//    Date    : Jun/16/2016
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
// Revision History
// ----------------
// $Log: $
// 
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		RtErrorLogBmc.c
//
// Description:	Runtime error log BMC library function definitions
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorLogBmc.h"

EFI_SEL_RECORD_DATA           mSelRecord;
#if SMCPKG_SUPPORT
#include <Library\IoLib.h>
#if USE_PLATFORM_EVENT_MESSAGE
EFI_EVENT_MESSAGE             mEventMessage;   
#endif
//BOOLEAN  LogUncorrectableError = FALSE; 
#define PCI_STS  0x0006        // PCI Status Register
#define PCI_BRIDGE_CLASS_CODE               0x0604
BOOLEAN  LogBmcUECC = FALSE;
#endif // #if SMCPKG_SUPPORT

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   UpdateGenericSelInfo
//
// Description: 
// This function will update generic IPMI info fields 
//
// Input:
//
//    SelRecord           - Pointer to SEL Record
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
UpdateGenericSelInfo (
  IN OUT  EFI_SEL_RECORD_DATA     *SelRecord
  )
{

  SelRecord->TimeStamp.Year = 0;
  SelRecord->RecordType = SEL_SYSTEM_RECORD;
  SelRecord->GeneratorId = (UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
  SelRecord->EvMRevision = EFI_EVM_REVISION;
  SelRecord->EventDirType = EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   UpdateSelInfoForMe
//
// Description: 
// Update the SEL Record information for Memory Errors
//
// Input:
//
//    MemorySensorOffset  - Memory offset to enum EFI_SEL_SENSOR_MEMORY_OFFSET
//    Channel          		- Channel, base 0
//    DimmNumber          - Dimm Number, base 0
//    SelRecord           - Pointer to SEL Record
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
UpdateSelInfoForMe (
  IN      EFI_SEL_SENSOR_MEMORY_OFFSET        MemorySensorOffset,
	IN      MEM_ERROR_LOCATION									*MemErrorLoc,
  IN OUT  EFI_SEL_RECORD_DATA                 *SelRecord
  )
{
  // Update Generic SEL related information

  UpdateGenericSelInfo (SelRecord);

  SelRecord->SensorType = EFI_MEMORY_SENSOR_TYPE;
  SelRecord->SensorNumber = (UINT8)0x8; // for UCE/CE

  if (MemorySensorOffset == EFI_MEMORY_SENSOR_OFFSET_ECC_UNCORRECTABLE) {
    // UCE error
    SelRecord->OEMEvData1 = 0xA0 | MEMORY_SENSOR_UCE_OFFSET; // Add error Id to identify the error
  } else if (MemorySensorOffset == EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE){
    // Single bit error
    SelRecord->OEMEvData1 = 0xA0 | MEMORY_SENSOR_CE_OFFSET; // Add error Id to identify the error
  } else if (MemorySensorOffset==EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE_MAX){
	// Single bit error threshold reached
    SelRecord->OEMEvData1 = 0xA0 | MEMORY_SENSOR_CEMAX_OFFSET; // Add error Id to identify the error
  }

  SelRecord->OEMEvData2 = (MemErrorLoc->MemoryController << 1); 
  if (MemErrorLoc->Dimm == 0xff) {
      // No info about the error is available
      SelRecord->OEMEvData3 = (UINT8)((MemErrorLoc->Socket << OEMEvData3_SOCKET_NUM_BITS) |(MemErrorLoc->DdrChannel << OEMEvData3_CHANNEL_NUM_BITS) | DIMM_NUM_BITS);
  } else {
      SelRecord->OEMEvData3 = (UINT8)((MemErrorLoc->Socket << OEMEvData3_SOCKET_NUM_BITS) |(MemErrorLoc->DdrChannel << OEMEvData3_CHANNEL_NUM_BITS) | MemErrorLoc->Dimm);
  }

  // Changes done for the LASTBOOT ERROR Support
  if (mPlatformRasPolicyProtocol->EfiRasSystemTopology->SystemInfo.LastBootErrorFlag)
     SelRecord->OEMEvData2 |= BIT0; //ED2[0]: 0 - Current Boot. 1 - Last boot

  return EFI_SUCCESS;
}


#if SMCPKG_SUPPORT
#if USE_PLATFORM_EVENT_MESSAGE
EFI_STATUS
UpdateEvenInfoForMem (
   IN      EFI_SEL_SENSOR_MEMORY_OFFSET   MemorySensorOffset,
   IN      MEM_ERROR_LOCATION             *MemErrorLoc,
   IN OUT  EFI_EVENT_MESSAGE              *EventMessage
)
{
    // Update Generic SEL related information
    EventMessage->GeneratorId = 0x01;
    EventMessage->EvMRev = 0x04;
    EventMessage->SensorType = EFI_MEMORY_SENSOR_TYPE;
    EventMessage->SensorNumber = 0x00;
    EventMessage->EventDirType = 0x6F;
            DEBUG((-1, "entry UpdateEvenInfoForMem \n"));

    if (MemorySensorOffset == EFI_MEMORY_SENSOR_OFFSET_ECC_UNCORRECTABLE) {
    
      // UCE error
      EventMessage->EventData1 = 0xA1;
      LogBmcUECC = TRUE;
    } else if (MemorySensorOffset == EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE) {
      EventMessage->EventData1 = 0xA0;
    }
    
    if (MemErrorLoc->Dimm == 0xFF) {
      // No info about the Dimm number
      EventMessage->EventData2 = 0xFF; 
    } else {
      EventMessage->EventData2 = (UINT8)((((MemErrorLoc->MemoryController * 3) + MemErrorLoc->DdrChannel + 1) << OEMEvData3_CHANNEL_NUM_BITS) | (MemErrorLoc->Dimm + 0x0A));
    }
    
    EventMessage->EventData3 = (UINT8)(MemErrorLoc->Socket | 0x80);
    
    return EFI_SUCCESS;
    
}
#endif
#endif // #if SMCPKG_SUPPORT

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AllocSelRecord
//
// Description: 
//     Allocate memory for SelRecord
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
AllocSelRecord(
  IN OUT EFI_SEL_RECORD_DATA **SelRecord
  )
{
  *SelRecord = &mSelRecord;
}

#if SMCPKG_SUPPORT
#if USE_PLATFORM_EVENT_MESSAGE
AllocEventMessage (
    IN OUT EFI_EVENT_MESSAGE **EventMessage
)
{
    *EventMessage = &mEventMessage;
}
#endif
#endif // #if SMCPKG_SUPPORT
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogInfoToSel
//
// Description: 
//
//	This function will log the data into SEL using Generic Log API

// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogInfoToSel (
  IN  VOID              *SelRecord
  )
{
  EFI_STATUS      				Status;
  EFI_IPMI_TRANSPORT			*IpmiTransport;
  UINT8      		  		    ResponseData[10];
  UINT8        	  				ResponseDataSize;
#if IPMI_COMPLETION_CODE_SUPPORT
  UINT8                         CompletionCode;
#endif
 ResponseDataSize = 10;

 Status = gSmst->SmmLocateProtocol (&gEfiSmmIpmiTransportProtocolGuid, NULL, &IpmiTransport);
  if (Status == EFI_SUCCESS) {
    Status = IpmiTransport-> SendIpmiCommand (IpmiTransport,
						EFI_SM_STORAGE,
						0,
						ADD_SEL_ENTRY,
						(UINT8 *) SelRecord,
						sizeof (EFI_SEL_RECORD_DATA),
						(UINT8 *) &ResponseData,
#if IPMI_COMPLETION_CODE_SUPPORT
						(UINT8 *) &ResponseDataSize,
						(UINT8 *) &CompletionCode
#else
						(UINT8 *) &ResponseDataSize
#endif
						); 
  }

  Status = EFI_SUCCESS;
  return Status;
}

#if SMCPKG_SUPPORT
#if USE_PLATFORM_EVENT_MESSAGE
EFI_STATUS
LogInfoToEvenMessage (
    IN  EFI_EVENT_MESSAGE     *EventMessage
)
{
    EFI_STATUS           Status;
    EFI_IPMI_TRANSPORT   *IpmiTransport;
    UINT8                ResponseData[10];
    UINT8                ResponseDataSize;
    
    
    ResponseDataSize = 10;
            DEBUG((-1, "ready to LogInfoToEvenMessage \n"));

    Status = gSmst->SmmLocateProtocol (&gEfiSmmIpmiTransportProtocolGuid, NULL, &IpmiTransport);
    
    if (Status == EFI_SUCCESS) {
                DEBUG((-1, "located  gEfiSmmIpmiTransportProtocolGuid \n"));

    Status = IpmiTransport-> SendIpmiCommand (IpmiTransport,
                                             EFI_SM_SENSOR_EVENT,
                                             0,
                                             PLATFORM_EVENT,
                                             (UINT8*) EventMessage,
                                             sizeof (EFI_EVENT_MESSAGE),
                                             (UINT8*) &ResponseData,
                                             (UINT8*) &ResponseDataSize);
    }

    return Status;
    
}
#endif
#endif // #if SMCPKG_SUPPORT
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogMemErrorToSel
//
// Description: 
//	Log the memory errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogMemErrorToSel (
        AMI_MEMORY_ERROR_SECTION		*MemInfo
 )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    EFI_SEL_RECORD_DATA             *SelRecord = NULL;
#if SMCPKG_SUPPORT
#if USE_PLATFORM_EVENT_MESSAGE
    EFI_EVENT_MESSAGE       *EventMessage = NULL;
#endif 
#endif // #if SMCPKG_SUPPORT
    EFI_SEL_SENSOR_MEMORY_OFFSET    MemorySensorOffset = EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE_MAX;
    MEM_ERROR_LOCATION              MemErrorLoc;

#if SMCPKG_SUPPORT
    DEBUG((-1, "LogMemErrorToSel entry \n"));
    if ( mPlatformRasPolicyProtocol->EfiRasSystemTopology->SystemInfo.LastBootErrorFlag &&
         (MemInfo->Header.ErrorSeverity != EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) ){
        DEBUG((EFI_D_ERROR, "RtBMC ret at last boot\n"));
        //return EFI_SUCCESS;   
    } 
#if USE_PLATFORM_EVENT_MESSAGE
    // Allocate memory for EventMessage 
    AllocEventMessage (&EventMessage);
#else
    // Allocate memory for SelRecord
    AllocSelRecord( &SelRecord );
#endif
#else
    AllocSelRecord( &SelRecord );
#endif // #if SMCPKG_SUPPORT

    // Set location
    MemErrorLoc.Socket = (UINT8)(NODE_TO_SKT(MemInfo->Node));
    MemErrorLoc.MemoryController = (UINT8)(NODE_TO_MC(MemInfo->Node));
    MemErrorLoc.DdrChannel = (UINT8)(MemInfo->Card);
    MemErrorLoc.Dimm = (UINT8)(MemInfo->Module);

    // Set type
    MemorySensorOffset = (MemInfo->Header.ErrorSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED ? 
        EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE_MAX :
        EFI_MEMORY_SENSOR_OFFSET_ECC_UNCORRECTABLE);

#if SMCPKG_SUPPORT
#if USE_PLATFORM_EVENT_MESSAGE

    // Log Event Message to report
    if ( LogBmcUECC ) {
        DEBUG((EFI_D_ERROR, "RtBMC has logged UCE ,ret\n"));
        return EFI_SUCCESS;
    }

    Status = UpdateEvenInfoForMem (MemorySensorOffset, &MemErrorLoc, EventMessage);
    Status = LogInfoToEvenMessage( EventMessage );
#else
    // Log sel entry to report
    Status = UpdateSelInfoForMe (MemorySensorOffset, &MemErrorLoc, SelRecord);
    Status = LogInfoToSel( (VOID*)SelRecord );
#endif
#else
    // Log sel entry to report
    Status = UpdateSelInfoForMe (MemorySensorOffset, &MemErrorLoc, SelRecord);
    Status = LogInfoToSel( (VOID*)SelRecord );
#endif // #if SMCPKG_SUPPORT

    return	Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogProcErrorToSel
//
// Description: 
//  Log the Processor errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogProcErrorToSel (
        AMI_PROCESSOR_GENERIC_ERROR_SECTION                  *ProcErrorData
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_SEL_RECORD_DATA             SelRecord;
  EFI_CPU_PHYSICAL_LOCATION       Loc;
  UINT8                           Socket = 0;
  EFI_SEL_PROCESSOR_BANK_TYPE     BankType = 0;
     // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );

    // Initialize specific parts of SEL record.
    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE ; 
    SelRecord.SensorNumber = (UINT8)0x0; 

    SelRecord.OEMEvData1 = 0xA0 | EFI_PROCESSOR_SENSOR_OFFSET_CONFIGURATION_ERROR; // Add error Id to identify the error

    ExtractProcessorLocation( (UINT32)ProcErrorData->ApicId,&Loc );
    Socket = Loc.Package;

    switch (ProcErrorData->McaBankInfo.BankType) {

    case MCA_UNIT_TYPE_IFU:
	    BankType =  MCA_UNIT_IFU;
		break;
    case MCA_UNIT_TYPE_DCU:
	    BankType =  MCA_UNIT_DCU;
		break;
    case MCA_UNIT_TYPE_DTLB:
	    BankType =  MCA_UNIT_DTLB;
		break;
    case MCA_UNIT_TYPE_MLC:
	    BankType =  MCA_UNIT_MLC;
		break;
    case MCA_UNIT_TYPE_PCU:
	    BankType =  MCA_UNIT_PCU;
		break;
    case MCA_UNIT_TYPE_IIO:
	    BankType =  MCA_UNIT_IIO;
		break;
    case MCA_UNIT_TYPE_CHA:
	    BankType =  MCA_UNIT_CHA;
		break;
    case MCA_UNIT_TYPE_KTI:
	    BankType =  MCA_UNIT_KTI;
		break;
	default:
		BankType = MCA_UNIT_NONE;

    }
    SelRecord.OEMEvData2 = ( Socket << 4 ) | BankType; 
    SelRecord.OEMEvData3 = (ProcErrorData->ErrorType << 4) | (ProcErrorData->Header.ErrorSeverity);

    // Report the error.
    Status = LogInfoToSel( (VOID*)&SelRecord ); 
    
    return  Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LogPciErrorToSel
//
// Description:
//  Takes in an error type and the address of a PCI device. Correctly fills
//  in a SEL record and logs to the SEL.
//
// Input:
//  IN  UINT8     ErrorType - Type of PCI error (SERR, PERR, etc.)
//  IN  UINT8     Bus
//  IN  UINT8     Dev
//  IN  UINT8     Func
//
// Output: 
//  EFI_STATUS
//      EFI_SUCCESS - Error successfully logged.
//      Others - A problem occurred while trying to log error.
//
// Modified:
//  Nothing
//
// Referrals:
//  UpdateGenericSelInfo()
//  LogInfoToSel()
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogPciErrorToSel (
        AMI_PCIE_ERROR_SECTION                      *PcieInfo
)
{
#if SMCPKG_SUPPORT
#if USE_PLATFORM_EVENT_MESSAGE
    EFI_EVENT_MESSAGE   EventMessage;
    UINT8   Bus, Dev, Func; 
    UINT8   ErrorType = SEL_SENS_PCI_PERR;
#else //#if USE_PLATFORM_EVENT_MESSAGE
    EFI_SEL_RECORD_DATA SelRecord;
#endif
#else
    EFI_SEL_RECORD_DATA SelRecord;
#endif // #if SMCPKG_SUPPORT
    EFI_STATUS          Status;

#if SMCPKG_SUPPORT
#if USE_PLATFORM_EVENT_MESSAGE
    // Initialize Event Message.
    EventMessage.GeneratorId = 0x01;
    EventMessage.EvMRev = 0x04;
    EventMessage.SensorType = SEL_SENS_TYPE_CRIT_ERR;
    EventMessage.SensorNumber = 0x00;
    EventMessage.EventDirType = 0x6F;

    Bus = PcieInfo->DeviceId.PriBus;
    Dev = PcieInfo->DeviceId.DevNum;
    Func= PcieInfo->DeviceId.FuncNum;
    
    if ( PcieInfo->ErrorType == SERROR_TYPE ) {
       ErrorType = SEL_SENS_PCI_SERR;
    } else {
       ErrorType = SEL_SENS_PCI_PERR;  
    }


    DEBUG ((-1, "Log Error Information to BMC: Bus = %02x Device = %02x Function = %02x ErrorType = %02x\n", \
           Bus, Dev, Func, ErrorType));               
    EventMessage.EventData1 = 0xA0 | ErrorType; 
    EventMessage.EventData2 = Bus;
    EventMessage.EventData3 = (Dev << 3) | Func;
    
    Status = LogInfoToEvenMessage (&EventMessage);	  
   	
#else
    // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );

    // Initialize specific parts of SEL record.
    SelRecord.SensorType = SEL_SENS_TYPE_CRIT_ERR;
    SelRecord.SensorNumber = 0x00;
    if (mPlatformRasPolicyProtocol->EfiRasSystemTopology->SystemInfo.LastBootErrorFlag) {
        SelRecord.OEMEvData1 = 0xA0 | LASTBOOT_PCIE_ERROR;    // OEM Data for byte 2, 3 + Last Boot PCIe Error
    }
    else {
      if (PcieInfo->ErrorType == PERROR_TYPE) {
        SelRecord.OEMEvData1 = 0xA0 | CRITICAL_INTERRUPT_PCI_PERR;    // OEM Data for byte 2, 3.
      }
      else if (PcieInfo->ErrorType == SERROR_TYPE) {
        SelRecord.OEMEvData1 = 0xA0 | CRITICAL_INTERRUPT_PCI_SERR;    // OEM Data for byte 2, 3.
      }
      else if (PcieInfo->ErrorType == COR_ERROR_TYPE) {
        SelRecord.OEMEvData1 = 0xA0 | CRITICAL_INTERRUPT_BUS_CORERR;    // OEM Data for byte 2, 3.
      }
      else if (PcieInfo->ErrorType == NONFATAL_ERROR_TYPE) {
        SelRecord.OEMEvData1 = 0xA0 | CRITICAL_INTERRUPT_BUS_UNCERR;    // OEM Data for byte 2, 3.
      }
      else if (PcieInfo->ErrorType == FATAL_ERROR_TYPE) {
        SelRecord.OEMEvData1 = 0xA0 | CRITICAL_INTERRUPT_BUS_FATERR;    // OEM Data for byte 2, 3.
      }
    }
    SelRecord.OEMEvData2 = PcieInfo->DeviceId.PriBus;                 // Fill with Bus.
    SelRecord.OEMEvData3 = (PcieInfo->DeviceId.DevNum << 3) | PcieInfo->DeviceId.FuncNum;   // Fill with Dev and Func.

    // Report the error.
    Status = LogInfoToSel( (VOID*)&SelRecord ); 
#endif //#if USE_PLATFORM_EVENT_MESSAGE
#endif //#if SMCPKG_SUPPORT
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogIohQpiToSel
//
// Description: 
//	Log the Ioh Qpi errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogIohQpiToSel (
    IN UINT8    Socket,
    IN UINT8    Port,
    IN UINT16   ErrorType,
    IN UINT8    ErrorSubType,
    IN UINT8    ErrorSeverity
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_SEL_RECORD_DATA             SelRecord;

     // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );
    // Initialize specific parts of SEL record.
    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE ; 
    SelRecord.SensorNumber = (UINT8)0x0; 

    SelRecord.OEMEvData1 = 0xA0 | EFI_PROCESSOR_SENSOR_OFFSET_CONFIGURATION_ERROR; // Add error Id to identify the error
    SelRecord.OEMEvData2 = (Socket << 4) | (Port); 
    SelRecord.OEMEvData3 = ErrorSubType ;

    // Report the error.
    Status = LogInfoToSel( (VOID*)&SelRecord ); 
    
    return	Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogIohCoreToSel
//
// Description: 
//	Log the Ioh Core errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogIohCoreToSel (
    IN UINT8    Socket,
    IN UINT16   ErrorType,
    IN UINT8    ErrorSubType,
    IN UINT8    ErrorSeverity
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_SEL_RECORD_DATA             SelRecord;

     // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );
    // Initialize specific parts of SEL record.
    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE ; 
    SelRecord.SensorNumber = (UINT8)0x0; 

    SelRecord.OEMEvData1 = 0xA0 | EFI_PROCESSOR_SENSOR_OFFSET_CONFIGURATION_ERROR; // Add error Id to identify the error
    SelRecord.OEMEvData2 = (Socket << 4); 
    SelRecord.OEMEvData3 = ErrorSubType ;

    // Report the error.
    Status = LogInfoToSel( (VOID*)&SelRecord ); 
    
    return	Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogIohVtdToSel
//
// Description: 
//	Log the Ioh Vtd errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogIohVtdToSel (
    IN UINT8    Socket,
    IN UINT8    Stack,
    IN UINT16   ErrorType,
    IN UINT8    ErrorSubType,
    IN UINT8    ErrorSeverity
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_SEL_RECORD_DATA             SelRecord;
     // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );
    // Initialize specific parts of SEL record.
    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE ; 
    SelRecord.SensorNumber = (UINT8)0x0; 

    SelRecord.OEMEvData1 = 0xA0 | EFI_PROCESSOR_SENSOR_OFFSET_CONFIGURATION_ERROR; // Add error Id to identify the error
    SelRecord.OEMEvData2 = (Socket << 4) | (Stack); 
    SelRecord.OEMEvData3 = ErrorSubType ;

    // Report the error.
    Status = LogInfoToSel( (VOID*)&SelRecord ); 
    
    return	Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogItcToSel
//
// Description: 
//	Log the Ioh ITC errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogItcToSel (
    IN UINT8    Socket,
    IN UINT8    Stack,
    IN UINT16   ErrorType,
    IN UINT8    ErrorSubType,
    IN UINT8    ErrorSeverity
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_SEL_RECORD_DATA             SelRecord;

     // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );
    // Initialize specific parts of SEL record.
    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE ; 
    SelRecord.SensorNumber = (UINT8)0x0; 

    SelRecord.OEMEvData1 = 0xA0 | EFI_PROCESSOR_SENSOR_OFFSET_CONFIGURATION_ERROR; // Add error Id to identify the error
    SelRecord.OEMEvData2 = (Socket << 4) | (Stack); 
    SelRecord.OEMEvData3 = ErrorSubType ;

    // Report the error.
    Status = LogInfoToSel( (VOID*)&SelRecord ); 
    
    return	Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogOtcToSel
//
// Description: 
//	Log the Ioh OTC errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogOtcToSel (
    IN UINT8    Socket,
    IN UINT8    Stack,
    IN UINT16   ErrorType,
    IN UINT8    ErrorSubType,
    IN UINT8    ErrorSeverity
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_SEL_RECORD_DATA             SelRecord;

     // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );
    // Initialize specific parts of SEL record.
    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE ; 
    SelRecord.SensorNumber = (UINT8)0x0; 

    SelRecord.OEMEvData1 = 0xA0 | EFI_PROCESSOR_SENSOR_OFFSET_CONFIGURATION_ERROR; // Add error Id to identify the error
    SelRecord.OEMEvData2 = (Socket << 4) | (Stack); 
    SelRecord.OEMEvData3 = ErrorSubType ;

    // Report the error.
    Status = LogInfoToSel( (VOID*)&SelRecord ); 
    
    return	Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogDmaToSel
//
// Description: 
//	Log the Ioh DMA errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogDmaToSel (
    IN UINT8    Socket,
    IN UINT8    Stack,
    IN UINT16   ErrorType,
    IN UINT8    ErrorSubType,
    IN UINT8    ErrorSeverity
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_SEL_RECORD_DATA             SelRecord;

     // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );
    // Initialize specific parts of SEL record.
    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE ; 
    SelRecord.SensorNumber = (UINT8)0x0; 

    SelRecord.OEMEvData1 = 0xA0 | EFI_PROCESSOR_SENSOR_OFFSET_CONFIGURATION_ERROR; // Add error Id to identify the error
    SelRecord.OEMEvData2 = (Socket << 4) | (Stack); 
    SelRecord.OEMEvData3 = ErrorSubType ;

    // Report the error.
    Status = LogInfoToSel( (VOID*)&SelRecord ); 
    
    return	Status;
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
