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
// $Header: /AptioV/BIN/AMIDebugRx/DbgRxDebugSupportLib/DebugPortUsb.c 1     3/27/15 3:20a Jekekumarg $
//
// $Revision: 1 $
//
// $Date: 3/27/15 3:20a $
//**********************************************************************
// Revision History
// ----------------
// $Log: /AptioV/BIN/AMIDebugRx/DbgRxDebugSupportLib/DebugPortUsb.c $
// 
// 1     3/27/15 3:20a Jekekumarg
// 
// 4     6/26/14 3:00p Sudhirv
// Updated after 3.01.0010 INT label
// 
// 3     2/20/14 5:01p Sudhirv
// Updated for AMI Debug for UEFI 3.01.0008 INT Label
// 
// 2     7/03/13 11:57a Sudhirv
// [TAG]	EIP123305
// [Category]	Improvement
// [Description]	5.004_Capsule_02 will cause build error when add
// AmiDebugRx module
// [Files]	PeiDebugSupport.c
// relocdbg.c
// AmiDebugPort.c
// DebugPortUsb.c
// DbgRxDebugSupportLib.cif
// 
// 1     11/02/12 10:04a Sudhirv
// AMIDebugRx eModule for AMI Debug for UEFI 3.0
// 
// 2     8/08/12 3:58p Sudhirv
// Updated after Binaries prepared.
// 
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
// 
//
//**********************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	DebugPortUsb.c
//
// Description:This file contains code to fill debug port information
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Efi.h"
#include "AmiLib.h"
#include "Pei.h"

#include "AmiDebugPort.h"
#include <Library/AMIpeidebug.h>
#include <Library/AMIpeidebugX64.h>

void DBGWriteChkPort(UINT8);
#ifndef EFIx64
//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	FillDebugPortInfo
//
// Description:	Fills the Debug Port Informations
//
// Input:		SerXPortGblData_T *	,
//				AMI_DEBUGPORT_INFORMATION_HOB *
//
// Output:		VOID
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void FillDebugPortInfo(	SerXPortGblData_T *	PeiData,
						AMI_DEBUGPORT_INFORMATION_HOB *AmiDebugPortHob)
{

  AmiDebugPortHob->DeviceInfo.DebugPortType = USB2_EHCI;
  AmiDebugPortHob->DeviceInfo.BaseAddress = PeiData->USBBASE;
  AmiDebugPortHob->DeviceInfo.Length = PeiData->m_EHCI_MEMORY_SIZE;
  AmiDebugPortHob->DeviceInfo.Address.Addr.Bus = PeiData->m_PCI_EHCI_BUS_NUMBER;
  AmiDebugPortHob->DeviceInfo.Address.Addr.Device = PeiData->m_PCI_EHCI_DEVICE_NUMBER;
  AmiDebugPortHob->DeviceInfo.Address.Addr.Function = PeiData->m_PCI_EHCI_FUNCTION_NUMBER;
  AmiDebugPortHob->DeviceInfo.Address.Addr.Register = 0;
  AmiDebugPortHob->DeviceInfo.Address.Addr.ExtendedRegister = 0;
  AmiDebugPortHob->DeviceInfo.IRQn = 0;			//Timer IRQ is used for reading USB2 debug port
  AmiDebugPortHob->DeviceInfo.BarOffset = PeiData->m_PCI_EHCI_BAR_OFFSET;
  AmiDebugPortHob->DeviceInfo.BarIndex = 0;

  AmiDebugPortHob->DeviceInfo.BarType = tBarMmio32;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	ProgramDebugPort()
//
// Description:	
//
// Input:		SerXPortGblVar_T *, UINTN ,UINTN 
//
// Output:		VOID
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void ProgramDebugPort(SerXPortGblVar_T *pXportGblVar, UINTN PeiDbgDataBaseAddr, UINTN BaseAddr)
{
	PEIDebugData_T		*pData = (PEIDebugData_T *)PeiDbgDataBaseAddr;
	SerXPortGblData_T	*PeiUsbData = (SerXPortGblData_T *)pData->XportLocalVar; 
	PEI_DBG_PORT_INFO   DebugPort;

	DebugPort.UsbDebugPort.USBBaseAddr = BaseAddr;
	if(pXportGblVar->PeiDbgSIO_Init_USB_EHCI != NULL){
		pXportGblVar->PeiDbgSIO_Init_USB_EHCI(&DebugPort);
		PeiUsbData->USBBASE = DebugPort.UsbDebugPort.USBBaseAddr;	
		PeiUsbData->USB2_DEBUG_PORT_REGISTER_INTERFACE = DebugPort.UsbDebugPort.USBDebugPortStartAddr;
	}
	else{
		//if chipset init for USB2 EHCI is not done properly then halt
		DBGWriteChkPort(0xBB);
		_asm {
			jmp $		//debugging purpose - remove later
		}
	}
}

#endif
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
