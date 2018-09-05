//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file TerminalBds.c
    This function will connect the UART devices as soon as Video is 
	available. This is done to make sure redirection starts before the 
	Post messages are displayed especially in first boot where Redirection
	starts in connecteverything.

**/

//---------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/SuperIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/TerminalAmiSerial.h> 

#define UART_PnpID 0x501


extern  VOID ConnectDevicePath(
    IN EFI_DEVICE_PATH_PROTOCOL *pPath
);

extern EFI_STATUS GetPciHandlesByClass(
    UINT8           Class,
    UINT8           SubClass,
    UINTN           *NumberOfHandles,
    EFI_HANDLE      **HandleBuffer
);



/**
    This function will connect Ami Serial UART devices for the Terminal driver
    to be started early. 

    @param VOID

    @retval VOID

**/

VOID
ConnectAmiSerialDevices (
) 
{
    EFI_HANDLE                  *Handle = NULL;
    UINTN                       Count;
    UINTN                       i;
    EFI_STATUS                  Status;
	
    // Locate the Firmware volume protocol
    Status = pBS->LocateHandleBuffer(
    							ByProtocol,
    							&gAmiSerialProtocolGuid,
    							NULL,
    							&Count,
    							&Handle
    							);    
    
    if(!EFI_ERROR(Status) && Count != 0) {

    	// Connect the AmiSerial devices.
    	for(i=0; i<Count; i++) {
    		pBS->ConnectController(Handle[i],NULL,NULL,TRUE);
    	}    
        pBS->FreePool(Handle);
    }

    return;
}

/**
    This function will connect PCI UART devices for the Terminal driver
    to be started early. 

    @param VOID

    @retval VOID

**/

VOID
ConnectPciUartDevices (
) 
{
    EFI_HANDLE                  *Handle = NULL;
    UINTN                       Count;
    UINTN                       i;
    EFI_STATUS                  Status;
	
    //Get a list of PCI Serial devices 
    Status = GetPciHandlesByClass(
    					PCI_CLASS_SCC, 
    					PCI_SUBCLASS_SERIAL, 
                        &Count, 
                        &Handle
                        );

    if(!EFI_ERROR(Status) && Count != 0) {

    	// Connect the PCI UART devices.
    	for(i=0; i<Count; i++) {
    		pBS->ConnectController(Handle[i],NULL,NULL,TRUE);
    	}    
        pBS->FreePool(Handle);
    }

    //Get a list of Communication and Other type devices 
    Status = GetPciHandlesByClass(
    					PCI_CLASS_SCC, 
    					PCI_SUBCLASS_SCC_OTHER, 
                        &Count, 
                        &Handle
                        );

    if(!EFI_ERROR(Status) && Count != 0) {

    	// Connect the PCI UART devices.
    	for(i=0; i<Count; i++) {
    		pBS->ConnectController(Handle[i],NULL,NULL,TRUE);
    	}    
        pBS->FreePool(Handle);
    }

    return;
}

/**
    This function will connect SIO UART devices for the Terminal driver
    to be started early. 

    @param VOID

    @retval VOID

**/

VOID
ConnectSioUartDevices (
) 
{

    EFI_HANDLE                  *Handle = NULL;
    UINTN                       Count;
    UINTN                       i;
    EFI_STATUS                  Status;
    EFI_DEVICE_PATH_PROTOCOL    *TempDevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath = NULL;
    ACPI_HID_DEVICE_PATH        *AcpiPrevDPNodePtr = NULL;
    EFI_SIO_PROTOCOL            *EfiSio=NULL;
    
    //Get a list of all PCI to ISA Bridges
    Status = GetPciHandlesByClass(
                        PCI_CLASS_BRIDGE, 
                        PCI_CLASS_BRIDGE_ISA, 
                        &Count, 
                        &Handle
                        );
    
    if (!EFI_ERROR(Status)) {      
	    // Connect the SIO devices.
	    for(i=0; i<Count; i++) {
	        Status=pBS->HandleProtocol(Handle[i],&gEfiDevicePathProtocolGuid,(VOID**)&DevicePath);
	        if (EFI_ERROR(Status)) { 
	            continue;
	        }
	        pBS->ConnectController(Handle[i],NULL,NULL,FALSE);
	    }
		pBS->FreePool(Handle);
	}    
    
    Status = pBS->LocateHandleBuffer(ByProtocol,
                      &gEfiSioProtocolGuid, 
                      NULL,
                      &Count, 
                      &Handle);

    if(EFI_ERROR(Status)) { 
        return;
    }

    for (i = 0; i < Count; i++) {

        Status = pBS->HandleProtocol(Handle[i],
                                   &gEfiDevicePathProtocolGuid,
                                   &DevicePath);

        if(EFI_ERROR(Status) || (DevicePath == NULL)) {
            continue;
        }

        TempDevicePath=DevicePath;
        
        for ( ;!isEndNode(TempDevicePath); TempDevicePath = NEXT_NODE(TempDevicePath)) {
            AcpiPrevDPNodePtr = (ACPI_HID_DEVICE_PATH *)(TempDevicePath);

            if ((AcpiPrevDPNodePtr->Header.Type == ACPI_DEVICE_PATH) && 
                    (AcpiPrevDPNodePtr->Header.SubType == ACPI_DP) && 
                    (AcpiPrevDPNodePtr->HID == EISA_PNP_ID(UART_PnpID))) {

                    ConnectDevicePath(DevicePath);
            }
        }
    }

    pBS->FreePool(Handle);
    
    return;
}

/**
    This function will connect UART devices for the Terminal driver
    to be started early. 

    @param VOID

    @retval VOID

**/

VOID
ConnectUartDevices (
) 
{
	ConnectSioUartDevices();
	ConnectPciUartDevices();
	ConnectAmiSerialDevices();
	return;
}

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
