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
//  Rev. 1.03
//    Bug Fix :fixed Coding error.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Dec/21/2016
//  
//  
//  Rev. 1.02
//      Bug Fixed:  Fix cannot log SERR/PERR. 
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Sep/09/2016
// 
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
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
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
// Name:  RtErrorLogGpnv.c
//
// Description: Runtime error log GPNV library function definitions
//
//<AMI_FHDR_END>
//**********************************************************************

#include "RtErrorLogGpnv.h"


EFI_SM_ELOG_PROTOCOL      *gGenericElogProtocol     = NULL;
#if SMCPKG_SUPPORT
#include <Library/MmPciBaseLib.h>
BOOLEAN  LogGpnvUncorrectableError = FALSE;
#define PCI_STS  0x0006        // PCI Status Register
#define PCI_BRIDGE_CLASS_CODE               0x0604
#endif // #if SMCPKG_SUPPORT


//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Name:   EfiSmSetEventLogData
//
// Description:
//  This function sends event log data to the destination such as LAN, ICMB,
// BMC etc.  
//
// Input:
//  IN  UINT8                             *ElogData,
//  IN  EFI_SM_ELOG_TYPE                  DataType,
//  IN  BOOLEAN                           AlertEvent,
//  IN  UINTN                             DataSize,
//  OUT UINT64                            *RecordId
//
// Output:
//     EFI_STATUS
//
//--------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EfiSmSetEventLogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId  )
{
    EFI_STATUS              Status;

    if (gGenericElogProtocol == NULL) {

        Status = gSmst->SmmLocateProtocol (
                &gEfiGenericElogProtocolGuid,
                NULL,
                &gGenericElogProtocol);
        if ((!EFI_ERROR(Status)) && (gGenericElogProtocol != NULL)) {
            return gGenericElogProtocol->SetEventLogData (
                    gGenericElogProtocol,
                    ElogData,
                    DataType,
                    AlertEvent,
                    DataSize,
                    RecordId );
        }
    }else {  
        return gGenericElogProtocol->SetEventLogData (
                gGenericElogProtocol,
                ElogData,
                DataType,
                AlertEvent,
                DataSize,
                RecordId );
    }

    return Status;

}

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Name:   LogMemErrorToGpnv
//
// Description:
//  This function log memory errors to GPNV.  
//
// Input:
//  IN AMI_MEMORY_ERROR_SECTION     *MemInfo
//
// Output:
//     EFI_STATUS
//
//--------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
LogMemErrorToGpnv(
  IN AMI_MEMORY_ERROR_SECTION     *MemInfo )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINT64                  RecordId;
    EFI_GPNV_RUNTIME_ERROR  ToWrite;


    ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME;
    ToWrite.EventLogType = EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR;
    ToWrite.EventLogType = (MemInfo->Header.ErrorSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED ?
                        EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR :
                        EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR);

    Status = EfiSmSetEventLogData (
                (UINT8*)&ToWrite,
                EfiElogSmSMBIOS,
                FALSE,
                sizeof (EFI_GPNV_RUNTIME_ERROR),
                &RecordId );

    return Status;
}
#if SMCPKG_SUPPORT

EFI_STATUS
SmcLogMemErrorToGpnv (
	IN AMI_MEMORY_ERROR_SECTION		*MemInfo
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    UINT64          RecordId;
    ERROR_INFO      ErrorInfo;
    
    DEBUG ((-1, "SmcLogMemErrorToGpnv \n"));

    ErrorInfo.Header.RecordType = EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME;
   
    ErrorInfo.Header.EventLogType = (MemInfo->Header.ErrorSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED ?
                          EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR :
                          EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR);

    if (LogGpnvUncorrectableError) return Status;
    if (MemInfo->Header.ErrorSeverity != EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) LogGpnvUncorrectableError = TRUE;
    
    ErrorInfo.HardwareId = ECC_ERROR;
    if(MemInfo->Header.ErrorSeverity != EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED ) {
      ErrorInfo.Msg.EccMsg.UnCorrectable = 1;
    } 
    else{
      ErrorInfo.Msg.EccMsg.Correctable = 1;
    }
    
    ErrorInfo.Msg.EccMsg.DimmNum = (UINT8)(MemInfo->Module);
    ErrorInfo.Msg.EccMsg.Socket = (UINT8)(NODE_TO_SKT(MemInfo->Node));
    ErrorInfo.Msg.EccMsg.MemoryController = (UINT8)(NODE_TO_MC(MemInfo->Node));
    ErrorInfo.Msg.EccMsg.Channel = (UINT8)(MemInfo->Card);
    
    DEBUG((DEBUG_INFO,"Error Dimm Info: Socket = %02x, MemoryController = %02x, Channel = %02x, Dimm = %02x, EventLogType = %2x \n",
     NODE_TO_SKT(MemInfo->Node), NODE_TO_MC(MemInfo->Node), MemInfo->Card, MemInfo->Module, MemInfo->Header.ErrorSeverity));
    
    Status = EfiSmSetEventLogData ((UINT8*)&ErrorInfo, 
                                   EfiElogSmSMBIOS,
                                   FALSE,
                                   sizeof (ERROR_INFO),
                                   &RecordId );
    DEBUG ((-1, "SmcLogMemErrorToGpnv status: %x \n", Status));

    return Status;
}

EFI_STATUS
SmcLogPciErrorToGpnv (
        AMI_PCIE_ERROR_SECTION                      *PciErrorData
)
{
    EFI_STATUS     Status;
    UINT16         Data16;
    UINT64         RecordId;
    ERROR_INFO     ErrorInfo;
    UINT8   Bus, Dev, Func; 

    DEBUG ((-1, "SmcLogPciErrorToGpnv \n"));

    ErrorInfo.Header.RecordType = EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME;
    Bus = PciErrorData->DeviceId.PriBus;
    Dev = PciErrorData->DeviceId.DevNum;
    Func= PciErrorData->DeviceId.FuncNum;

    // Determine what sort of PCI error occurred.
    Data16 = *(UINT16*)MmPciAddress(0, Bus, Dev, Func, PCI_STS );
    Data16 &= (PCI_STS_MSTR_DATA_PERR | PCI_STS_SIG_SERR | PCI_STS_DET_PERR);
    
    DEBUG ((DEBUG_INFO, "Data16 = %04x, Bus = %02x, Device = %02x, Function = %02x\n", Data16, Bus, Dev, Func));
    
    if ( PciErrorData->ErrorType == SERROR_TYPE ) {
      ErrorInfo.Header.EventLogType = EFI_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR;
      ErrorInfo.Msg.PcieMsg.SystemErr = 1;        
    } 
    else {
      ErrorInfo.Header.EventLogType = EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR;
      ErrorInfo.Msg.PcieMsg.ParityErr = 1;        
    }    
    
   
      DEBUG ((-1, "Log Error Information to GPNV: Bus = %02x Device = %02x Function = %02x ErrorType = %02x\n", \
             Bus, Dev, Func, ErrorInfo.Header.EventLogType));     
      ErrorInfo.HardwareId = NB_PCIE_ERROR;
      ErrorInfo.Msg.PcieMsg.Bus = Bus;
      ErrorInfo.Msg.PcieMsg.Dev = Dev;
      ErrorInfo.Msg.PcieMsg.Fun = Func;
    
      Status = EfiSmSetEventLogData ((UINT8*)&ErrorInfo, 
                                     EfiElogSmSMBIOS,
                                     FALSE,
                                     sizeof (ERROR_INFO),
                                     &RecordId);    
     DEBUG ((-1, "SmcLogPciErrorToGpnv status: %r \n", Status));                                                                
  
     DEBUG ((-1, "SmcLogPciErrorToGpnv exit \n"));
    return Status;
}

#endif // #if SMCPKG_SUPPORT

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Name:   LogPciErrorToGpnv
//
// Description:
//  This function log PCI errors to GPNV.  
//
// Input:
//  IN AMI_PCIE_ERROR_SECTION    *PcieInfo
//
// Output:
//     EFI_STATUS
//
//--------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
LogPciErrorToGpnv (
		AMI_PCIE_ERROR_SECTION				*PcieInfo
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINT64                  RecordId;
    EFI_GPNV_RUNTIME_ERROR  ToWrite;


    ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME;
    ToWrite.EventLogType = PcieInfo->ErrorType;

    Status = EfiSmSetEventLogData (
                (UINT8*)&ToWrite,
                EfiElogSmSMBIOS,
                FALSE,
                sizeof (EFI_GPNV_RUNTIME_ERROR),
                &RecordId );

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Name:   LogProcessorErrorToGpnv
//
// Description:
//  This function log memory errors to GPNV.  
//
// Input:
//  IN AMI_MEMORY_ERROR_SECTION     *MemInfo
//
// Output:
//     EFI_STATUS
//
//--------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
LogProcessorErrorToGpnv(
  IN AMI_PROCESSOR_GENERIC_ERROR_SECTION     *ProcInfo )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINT64                  RecordId;
    EFI_GPNV_RUNTIME_ERROR  ToWrite;

    ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME;
    ToWrite.EventLogType = ProcInfo->ErrorType;

    Status = EfiSmSetEventLogData (
                (UINT8*)&ToWrite,
                EfiElogSmSMBIOS,
                FALSE,
                sizeof (EFI_GPNV_RUNTIME_ERROR),
                &RecordId );

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
