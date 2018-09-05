//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /AptioV/BIN/AMIDebugRx/DbgRxPortLib/PeiDbgPortUSB.c 1     11/02/12 10:14a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:14a $
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRx/DbgRxPortLib/PeiDbgPortUSB.c $
// 
//
//*****************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:		PeiDbgPortUSB.c
//
// Description:This file contains code to intialize PeiDebugPortInterface
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef EFIx64
#include <Library/AMIpeidebug.h>
#else
#include <Library/AMIpeidebugX64.h>
#endif
#include <AmiDebugPort.h>

void DBGWriteChkPort(UINT8	DbgChkPoint);
//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	Initialise
//
// Description:	Internal Helper function.
//
// Input:		PEI_DBG_PORT_INFO *
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void Initialise(PEI_DBG_PORT_INFO *DebugPort)
{
	DebugPort->UsbDebugPort.USBBaseAddr = 0;
	DebugPort->UsbDebugPort.USBDebugPortStartAddr = 0;
	DebugPort->UsbDebugPort.MemoryMappedIoSpaceSize = 0;
	DebugPort->UsbDebugPort.PciBusNumber = 0;
	DebugPort->UsbDebugPort.PciDeviceNumber = 0;
	DebugPort->UsbDebugPort.PciFunctionNumber = 0;
	DebugPort->UsbDebugPort.PciBAROffset = 0;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	InitPeiDebugPortInterface
//
// Description:	Internal Helper function.
//
// Input:		PEI_DBG_PORT_INFO *,
//				UINTN ,
//				UINTN 
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void InitPeiDebugPortInterface(PEI_DBG_PORT_INFO *DebugPort,UINTN SetupFlag, UINTN IDTBaseAddress)
{
#ifndef EFIx64
	PEIDebugData_T		*pData = (PEIDebugData_T *)IDTBaseAddress;
	SerXPortGblData_T	*PeiXportData = (SerXPortGblData_T *)pData->XportLocalVar;
	DbgGblData_T		*DbgData = (DbgGblData_T *)(pData->DbgrLocalVar);
#else
	PEIDebugData_Tx64		*pData = (PEIDebugData_Tx64 *)IDTBaseAddress;
	SerXPortGblData_Tx64	*PeiXportData = (SerXPortGblData_Tx64 *)pData->XportLocalVar;
	DbgGblData_Tx64		*DbgData = (DbgGblData_Tx64 *)(pData->DbgrLocalVar);
#endif

#if USB_DEBUG_TRANSPORT
	DbgData->m_FlagToDebugAfterMemory = (UINT8)SetupFlag;
#ifndef EFIx64
	// PORTING for ALASKA: For PciBus DXE driver HOB interface
	((SerXPortGblVar_T*)pData->XportGlobalVar)->PeiDbgSIO_Init_USB_EHCI =
							DebugPort->UsbDebugPort.InitUSBEHCI;
#else
	((SerXPortGblVar_Tx64*)pData->XportGlobalVar)->PeiDbgSIO_Init_USB_EHCI =
								DebugPort->UsbDebugPort.InitUSBEHCI;
#endif

	PeiXportData->USBBASE = DebugPort->UsbDebugPort.USBBaseAddr;
	PeiXportData->USB2_DEBUG_PORT_REGISTER_INTERFACE = DebugPort->UsbDebugPort.USBDebugPortStartAddr;
	PeiXportData->m_EHCI_MEMORY_SIZE = DebugPort->UsbDebugPort.MemoryMappedIoSpaceSize;
	PeiXportData->m_PCI_EHCI_BUS_NUMBER = DebugPort->UsbDebugPort.PciBusNumber;
	PeiXportData->m_PCI_EHCI_DEVICE_NUMBER = DebugPort->UsbDebugPort.PciDeviceNumber;
	PeiXportData->m_PCI_EHCI_FUNCTION_NUMBER = DebugPort->UsbDebugPort.PciFunctionNumber;
	PeiXportData->m_PCI_EHCI_BAR_OFFSET = DebugPort->UsbDebugPort.PciBAROffset;

	switch(PeiXportData->USBBASE){
		case 0:
			DBGWriteChkPort(0xBF);	//err code for invalid USB base address register
			while(1);
		case 0xffffffff:
			DBGWriteChkPort(0xD1);	//err code for invalid CAR
			while(1);
		default:
			break;
	}
	if(PeiXportData->USBBASE == PeiXportData->USB2_DEBUG_PORT_REGISTER_INTERFACE){
		DBGWriteChkPort(0xBE);	//err code for invalid USB Debug Port base address
		while(1);
	}
#endif
}

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
