//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
// $Header: /AptioV/SRC/Usb3Statuscode/XhciHandler/XhciHandlerCommon.c 1     8/27/15 2:28p Sudhirv $
//
// $Revision: 1 $
//
// $Date: 8/27/15 2:28p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /AptioV/SRC/Usb3Statuscode/XhciHandler/XhciHandlerCommon.c $
// 
// 1     8/27/15 2:28p Sudhirv
// Usb3Statuscode eModule
// 
// $
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Token.h>
#include "XhciHandler.h"
//#include "Usb3StatuscodeELink.h"
#include <Library/SerialPortLib.h>
#ifndef EFIx64
#include <AmiPeiLib.h>
#else
#include <Guid/HobList.h>
#include <AmiDxeLib.h>
extern EFI_BOOT_SERVICES *gBS;
#endif

EFI_GUID gXhciHandlerHobGuid = AMI_XHCI_HANDLER_HOB_GUID;

#ifndef EFIx64
//extern PEI_INIT_FUNCTION PEI_DBG_INIT_LIST EndOfInitList;
//PEI_INIT_FUNCTION* PeiDebuggerInitList [] = {PEI_DBG_INIT_LIST NULL};
#else
//
// The global variable which can be used after memory is ready.
//
XHCI_HANDLER *gXhciHandler = NULL;
USB3_DEBUG_PORT_HANDLE *mDbgUsb3DebugPortHandle = NULL;
#endif

#ifdef EARLY_MEM_INIT
#if EARLY_MEM_INIT
volatile UINT8 gEarlyMemInit = 1;
#else
volatile UINT8 gEarlyMemInit = 0;
#endif
#endif

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	IoRead32
//
// Description:	Internal Helper function. Reads 32-bit value from IO port
//
// Input:		UINT16 Port
//
// Output:		UINT32 - port data
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
UINT32 DbgIoRead32(UINT16 Port)
{
#ifdef	EFIx64
	return IoRead32(Port);
#else
	_asm {
		mov dx, Port
		in eax, dx
	}
#endif
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	DBGWriteChkPort
//
// Description:	Outs the Checkpoint value in port 80
//
// Input:		UINT8	DbgChkPoint
//
// Output: 		VOID
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void WriteChkPort(UINT8	ChkPoint)
{	
#ifdef	EFIx64
	IoWrite8(0x80,ChkPoint);
#else
	_asm {
		mov al,ChkPoint
		mov dx, 0x80
		out dx,al
	}
#endif
return;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	Stall
//
// Description:	Internal Helper function.
//
// Input:		UINTN Usec (microseconds)
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void Stall (UINTN Usec)
{
    UINTN   Counter = (Usec * 7)/2; // 3.58 count per microsec
    UINTN   i;
    UINT32  Data32;
    UINT32  PrevData;

    PrevData = DbgIoRead32(PM_BASE_ADDRESS + 8);
    for (i=0; i < Counter; ) {
       Data32 = DbgIoRead32(PM_BASE_ADDRESS + 8);
        if (Data32 < PrevData) {        // Reset if there is a overlap
            PrevData=Data32;
            continue;
        }
        i += (Data32 - PrevData);
		PrevData=Data32; // FIX need to find out the real diff betweek different count.
    }
}

#ifndef EFIx64
//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	Initialize
//
// Description:	Internal Helper function.
//
// Input:		PEI_DBG_PORT_INFO *DebugPort,
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
// Procedure:	InitUsb3StatuscodePortingHooks()
//
// Description:	Initialize the Usb3Statuscode Porting Hooks.
//
// Input:		VOID
//
// Output:		VOID
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void InitUsb3StatuscodePortingHooks()
{
/*	PEI_DBG_PORT_INFO	DebugPort;
	UINTN i;
	
	Initialise(&DebugPort);
	
	if(!PeiDebuggerInitList) {
		SerialPortWrite("Usb3Statuscode: No Porting Hooks\r\n",0x24);
		return;
	}
	//Initialize the Porting Hooks if available
	for(i=0; PeiDebuggerInitList[i]; i++)
		PeiDebuggerInitList[i](&DebugPort);*/
	return;
}
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
