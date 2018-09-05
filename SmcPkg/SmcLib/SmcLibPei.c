//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/26/2016
//
//****************************************************************************
//****************************************************************************
#include "Setup.h"
#include "Token.h"
#include "EFI.h"
#include <Pei.h>
#include "SmcLibBmc.h"
#include  "SmcLibBmcPrivate.h"
#include <Ppi/IPMITransportPpi.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#ifndef EFI_SM_INTEL_OEM
#define EFI_SM_INTEL_OEM    0x30
#endif


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  SMC_IPMICmd
//
// Description :  Send command to IPMI
//
// Parameters:    IN  UINT8          NetFunction,
//                IN  UINT8          Lun,
//                IN  UINT8          Command,
//                IN  UINT8          *CommandData,
//                IN  UINT8          CommandDataSize,
//                OUT UINT8         *ResponseData,
//                OUT UINT8         *ResponseDataSize
//
// Returns     :  EFI_STATUS Status
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>

EFI_STATUS
SMC_IPMICmd(
  IN  UINT8          NetFunction,
  IN  UINT8          Lun,
  IN  UINT8          Command,
  IN  UINT8          *CommandData,
  IN  UINT8          CommandDataSize,
  OUT UINT8         *ResponseData,
  OUT UINT8         *ResponseDataSize
)	
{
    EFI_STATUS	Status;  
    EFI_PEI_SERVICES       **PeiServices;  
    PEI_IPMI_TRANSPORT_PPI *IpmiTransportPpi;
 
    DEBUG((-1, "SMC_IPMICmd pei entry.\n"));
  
    PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
    //
     //
    //Locate IPMI Transport protocol to send commands to BMC.
    //
    Status = (*PeiServices)->LocatePpi (
        (CONST EFI_PEI_SERVICES**)PeiServices,
        &gEfiPeiIpmiTransportPpiGuid,
        0,
        NULL,
        (VOID **)&IpmiTransportPpi );
    if (EFI_ERROR(Status)){
       DEBUG((-1, "Can't locate Ipmi ret.\n"));
      return Status;
    }       
    
    DEBUG((-1, "Netfun0x%x,Lun=0x%x cmd=0x%x\n", NetFunction,Lun,Command));
    
    Status = IpmiTransportPpi->SendIpmiCommand(
		         IpmiTransportPpi,
		         NetFunction,		// EFI_SM_INTEL_OEM, 
		         Lun,				// LUN
		         Command,			
		         CommandData,	    // *CommandData
		         CommandDataSize,	// Size of CommandData
		         ResponseData,		// *ResponseData
		         ResponseDataSize);		// ResponseDataSize
        
    
    DEBUG((-1, "SMC_IPMICmd pei end.\n"));
    return Status;
}



//****************************************************************************