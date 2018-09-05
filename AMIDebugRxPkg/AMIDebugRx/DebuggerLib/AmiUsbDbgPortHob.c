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
// $Header: /AptioV/BIN/AMIDebugRx/DebuggerLib/AmiUsbDbgPortHob.c 1     3/27/15 3:20a Jekekumarg $
//
// $Revision: 1 $
//
// $Date: 3/27/15 3:20a $
//**********************************************************************
//**********************************************************************
// Revision History
// ----------------
// $Log: /AptioV/BIN/AMIDebugRx/DebuggerLib/AmiUsbDbgPortHob.c $
// 
// 1     3/27/15 3:20a Jekekumarg
// 
// 6     6/26/14 3:00p Sudhirv
// Updated after 3.01.0010 INT label
// 
// 5     6/24/14 6:01p Sudhirv
// Updated for AMI Debug for UEFI 3.01.0010 Label
// 
// 4     2/20/14 5:01p Sudhirv
// Updated for AMI Debug for UEFI 3.01.0008 INT Label
// 
// 3     9/21/13 4:23a Sudhirv
// Updated for AMI Debug for UEFI 3.01.0006 Label.
// 
// 2     5/15/13 2:51p Sudhirv
// Updated for x86 DXE support patch binary generation.
// 
// 1     11/02/12 10:04a Sudhirv
// AMIDebugRx eModule for AMI Debug for UEFI 3.0
// 
// 2     8/03/12 6:32p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//**********************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	AmiUsbDbgPortHob.c
//
// Description:	File containing the elink or Hook to patch up the USB
//				Debug Port Hob to support PCI Bus driver in EFIx64 Env..
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Efi.h"
//#include "EfiImage.h"
#include "AmiDxeLib.h"
#include "Hob.h"
#include "Pei.h"


#include "Token.h"

//#include EFI_GUID_DEFINITION (HOB)
#include <Library/BaseLib.h>
#ifndef	EFIx64
#include <Library/AMIpeidebug.h>	//contents from PEI debugger copied
#else
#include <Library/AMIpeidebugx64.h>
#endif
//#include <Library\xportcmd.h>
#include <Library/AMIPeiGUIDs.h>
#include <AmiDebugPort.h>
#include <Protocol/AMITransport.h>
#include <Protocol/PciRootBridgeIo.h>
//#include "AsmDbgrx64Libs.h"

#define	DBG_PRT_CAP_ID					0xA

#define GET_HOB_TYPE(Hob)          ((Hob).Header->HobType)
#define GET_HOB_LENGTH(Hob)        ((Hob).Header->HobLength)
#define GET_NEXT_HOB(Hob)          ((Hob).Raw + GET_HOB_LENGTH(Hob))
#define END_OF_HOB_LIST(Hob)       (GET_HOB_TYPE(Hob) == EFI_HOB_TYPE_END_OF_HOB_LIST)
#define EfiCompareGuid	!guidcmp

#ifdef EFIx64
#define	PEI_DEBUG_DATA_BASEADDRESS	GetDbgDataBaseAddress()
#endif
//-------------------------------------------------------------------------
EFI_STATUS
GetDebugPortResourceInfo(
	IN AMI_DEBUGPORT_INFORMATION_HOB *This,
	OUT DEBUG_PORT_INFO *pDeviceInfo);

EFI_STATUS
ConfigureDebugPortResources(
	IN AMI_DEBUGPORT_INFORMATION_HOB *This,
	IN EFI_BOOT_SERVICES *pBS,
	IN DEBUG_PORT_INFO *pDeviceInfo);

BOOLEAN
static GetPEIDataSectionBaseAddress (
#ifndef	EFIx64
  PEIDebugData_T	**pPEIDS
#else
  PEIDebugData_Tx64	**pPEIDS
#endif
);

VOID *
GetHob1 (
  IN UINT16  Type,
  IN VOID    *HobStart
  );

void	DisableDebugFeature(UINT32 *pStoreDr7);
void 	EnableDebugFeature(UINT32 *pStoreDr7);
//
//GUID HOB for the PEI debugger data section in memory
// (defined in PeiDebugSupport PEIM)
//
static EFI_GUID  mPeiDebugDataGuid = PEI_DBGSUPPORT_DATA_GUID;

static EFI_GUID  mDxeDebugDataGuid = DXE_DBG_DATA_GUID;

static EFI_GUID gHobListGuid= HOB_LIST_GUID;

extern UINTN gDbgPerformanceRecords;
UINT32	BkpDR7 = 0;
BOOLEAN IntState = FALSE;

BOOLEAN IsAMIDebugRxConnected(UINT8);
//-------------------------------------------------------------------------

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	Init_AMI_USBDebugPort_HOB()
//
// Description:	An ELINK to re-publish (patch up) the
//				AMI_DEBUGPORT_INFORMATION_HOB for EFIx64 PCI Bus driver.
//
//
// Input:		IN  VOID* ,  IN VOID *
//
// Output:		EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
Init_AMI_USBDebugPort_HOB(
	IN VOID* ImageHandle,
	IN VOID *SystemTable)
{
	EFI_GUID 						DbgPortHobGuid = AMI_DEBUGPORT_HOB_GUID;
	AMI_DEBUGPORT_INFORMATION_HOB 	*AmiDebugPortHob=NULL;
	EFI_STATUS						Status = EFI_SUCCESS;
#ifndef EFIx64
	PEIDebugData_T *pData;
	DbgGblData_T *DbgData;
#else
	PEIDebugData_Tx64 *pData;
	DbgGblData_Tx64 *DbgData;
#endif
	
	if(gDbgPerformanceRecords) {
	    PERF_START (NULL, "DBG-HOB", NULL, 0);
	}
	
#ifndef EFIx64
	pData  = (PEIDebugData_T*)PEI_DEBUG_DATA_BASEADDRESS;
	DbgData  = (DbgGblData_T*)(pData->DbgrLocalVar);
#else
	pData  = (PEIDebugData_Tx64*)PEI_DEBUG_DATA_BASEADDRESS;
	DbgData  = (DbgGblData_Tx64*)(pData->DbgrLocalVar);
#endif

	//Update Dxe DbgPerf Data
	if(gDbgPerformanceRecords) {
		if(DbgData->m_DbgInitPending)
		{
			DbgData->m_DbgInitPending = 0;
			PERF_START (NULL, "DbgDxe", NULL, DbgData->m_DbgInitTimeStart);
			PERF_END (NULL, "DbgDxe", NULL, DbgData->m_DbgInitTimeEnd);
		}
	}
	
	DbgData->m_Services = ((EFI_SYSTEM_TABLE*)(SystemTable));
	DbgData->m_TargetState = IN_DXE_WITH_DXE_DBGR;

	AmiDebugPortHob = (AMI_DEBUGPORT_INFORMATION_HOB*)GetEfiConfigurationTable(pST,&gHobListGuid);

	if(AmiDebugPortHob)
		Status = FindNextHobByGuid(&DbgPortHobGuid,(VOID**)&AmiDebugPortHob);
	if(EFI_ERROR(Status))
		AmiDebugPortHob = NULL;
	else {
		if(AmiDebugPortHob->Reserved != 0x7fffffffffffffff){
			AmiDebugPortHob->GetDebugPortProperties = GetDebugPortResourceInfo;
			AmiDebugPortHob->SetDebugPortResources = ConfigureDebugPortResources;
			AmiDebugPortHob->Reserved = 0x7fffffffffffffff;
		}
	}

	if(gDbgPerformanceRecords)
	    PERF_END (NULL, "DBG-HOB", NULL, 0);
	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	DbgFindCapPtr
//
// Description: This function searches the PCI address space for the PCI
//				device specified for a particular capability ID and returns
//				the offset in the PCI address space if one found
//
// Input:		 IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL*,
//				 IN PCI_CFG_ADDR ,
//				 IN UINT8	
//
// Output:	UINT8	Capability ID location if one found
//				Otherwise returns 0
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8
DbgFindCapPtr(
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*PciRootBrdgIoProtocol,
	IN PCI_CFG_ADDR addr,
	IN UINT8	CapId
)
{
	UINT8  Value;
	UINT8  Reg;

//	Value = ReadPCIRegByte(6, Bus, DevFunc);
	addr.Addr.Register = 0x6;
	PciRootBrdgIoProtocol->Pci.Read(PciRootBrdgIoProtocol,EfiPciWidthUint8,addr.ADDR,1,(VOID*)&Value);

	if (Value == 0xff) return 0;			//No device.
	if (!(Value & (1 << 4))) return 0;		// Check if capabilities list.

	Reg = 0x34;			// Register to First capabilities pointer
											//if 0, then capabilities
	for(;;)
	{
		//Value = ReadPCIRegByte(Reg, Bus, DevFunc);
		addr.Addr.Register = Reg;
		PciRootBrdgIoProtocol->Pci.Read(PciRootBrdgIoProtocol,EfiPciWidthUint8,addr.ADDR,1,(VOID*)&Value);

		if (!Value) return 0;

		Reg = Value;		// ptr to CapID
		//Value = ReadPCIRegByte(Reg, Bus, DevFunc);		//If capablity ID, return register that points to it.
		addr.Addr.Register = Reg;
		PciRootBrdgIoProtocol->Pci.Read(PciRootBrdgIoProtocol,EfiPciWidthUint8,addr.ADDR,1,(VOID*)&Value);

		if (Value == CapId) return (UINT8)Reg;

		++Reg;			//equals to next capability pointer.
  	}
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	ReprogramECHI_BAR()
//
// Description:	Reprograms the ECHI_BAR
//
// Input:		IN  EFI_BOOT_SERVICES *,
//				IN PCI_CFG_ADDR ,
//				IN UINTN	,
//				OUT  UINT32	*
//
// Output:		EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ReprogramECHI_BAR(
	IN EFI_BOOT_SERVICES *pBS,
	IN PCI_CFG_ADDR addr,
	IN UINTN	BaseAddr,
	OUT	UINT32	*USBDebugPortStartAddr)
{
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*PciRootBrdgIoProtocol;
	EFI_GUID		PciRootBridgeIoProtocolGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
	EFI_STATUS		Status;
	EFI_HANDLE		*HandlePtr;
	UINTN			NumHandles;
	UINT16			CmdReg;
	PCI_CFG_ADDR	temp = addr;
	UINT8				DbgPrtCapReg;
	UINT16				DbgPrtBaseOffset;

	Status = pBS->LocateHandleBuffer( ByProtocol, &PciRootBridgeIoProtocolGuid,
			NULL, &NumHandles, &HandlePtr );

	if ( !EFI_ERROR( Status ) ){

		Status = pBS->OpenProtocol( HandlePtr[ 0 ],
				&PciRootBridgeIoProtocolGuid,
				&PciRootBrdgIoProtocol,
				NULL,			// ImageHandle,
				NULL,
				EFI_OPEN_PROTOCOL_GET_PROTOCOL );
		if ( EFI_ERROR( Status ) )
			return ( Status );

	//Disable the EHCI device
	addr.Addr.Register = 0x4;
	PciRootBrdgIoProtocol->Pci.Read(PciRootBrdgIoProtocol,EfiPciWidthUint16,addr.ADDR,1,(VOID*)&CmdReg);
	CmdReg = CmdReg & 0xfffC;
	PciRootBrdgIoProtocol->Pci.Write(PciRootBrdgIoProtocol,EfiPciWidthUint16,addr.ADDR,1,(VOID*)&CmdReg);

	addr = temp;
	PciRootBrdgIoProtocol->Pci.Write(PciRootBrdgIoProtocol,EfiPciWidthUint32,addr.ADDR,1,(VOID*)&BaseAddr);

	//Enable the EHCI device
	CmdReg = CmdReg | 0x06;
	addr.Addr.Register = 0x4;
	PciRootBrdgIoProtocol->Pci.Write(PciRootBrdgIoProtocol,EfiPciWidthUint16,addr.ADDR,1,(VOID*)&CmdReg);

	// Locate the Debug port register Interface location
	if(USBDebugPortStartAddr)
		{
			DbgPrtCapReg = DbgFindCapPtr(PciRootBrdgIoProtocol, addr,DBG_PRT_CAP_ID);
			addr.Addr.Register = DbgPrtCapReg + 2;
			PciRootBrdgIoProtocol->Pci.Read(PciRootBrdgIoProtocol,EfiPciWidthUint16,addr.ADDR,1,(VOID*)&DbgPrtBaseOffset);
			DbgPrtBaseOffset &= 0x1fff;

			*USBDebugPortStartAddr = (UINT32)(BaseAddr + DbgPrtBaseOffset);
		}
	}

	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetDebugPortResourceInfo()
//
// Description:	Gets the Debug Port Resource Information
//
// Input:		 IN AMI_DEBUGPORT_INFORMATION_HOB *,
//				 OUT DEBUG_PORT_INFO *
//
// Output:		EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetDebugPortResourceInfo(
	IN AMI_DEBUGPORT_INFORMATION_HOB *This,
	OUT DEBUG_PORT_INFO *pDeviceInfo)
{
	if(gDbgPerformanceRecords)
	    PERF_START (NULL, "DBG-RES", NULL, 0);

	//if Host is not connected and if not PMMode then return
	if(!IsAMIDebugRxConnected(1) && !IsAMIDebugRxConnected(0)) 
	{
		if(gDbgPerformanceRecords)
			PERF_END (NULL, "DBG-RES", NULL, 0);
		
		return EFI_SUCCESS;
	}
	
	pDeviceInfo->DebugPortType = This->DeviceInfo.DebugPortType;
	pDeviceInfo->BaseAddress = This->DeviceInfo.BaseAddress;
	pDeviceInfo->Length = This->DeviceInfo.Length;
	pDeviceInfo->IRQn = This->DeviceInfo.IRQn;
	pDeviceInfo->Address = This->DeviceInfo.Address;
	pDeviceInfo->BarOffset = This->DeviceInfo.BarOffset;
	pDeviceInfo->BarIndex = This->DeviceInfo.BarIndex;
	pDeviceInfo->BarType = This->DeviceInfo.BarType;
	
	if(gDbgPerformanceRecords)
	    PERF_END (NULL, "DBG-RES", NULL, 0);
	
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	ConfigureDebugPortResources()
//
// Description:	Configures the Debug Port Resources
//
// Input:		 IN AMI_DEBUGPORT_INFORMATION_HOB *,
//				 IN EFI_BOOT_SERVICES *,
//				IN  DEBUG_PORT_INFO *
//
// Output:		EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ConfigureDebugPortResources(
	IN AMI_DEBUGPORT_INFORMATION_HOB *This,
	IN EFI_BOOT_SERVICES *pBS,
	IN DEBUG_PORT_INFO *pDeviceInfo)
{
	AMI_TRANSPORT_PROTOCOL	*TransportProtocol;
	EFI_GUID			AMITransportProtocolGuid = AMI_TRANSPORT_PROTOCOL_GUID;
	EFI_STATUS			Status;
	EFI_HANDLE			*HandlePtr;
	UINTN				NumHandles;
	UINT32				BackupDR7;
#ifndef	EFIx64
	PEIDebugData_T		*pPEIDS=NULL;
	SerXPortGblVar_T		*SerVar=NULL;
	SerXPortGblData_T	*SerData = NULL;
#else
	PEIDebugData_Tx64		*pPEIDS=NULL;
	SerXPortGblVar_Tx64		*SerVar=NULL;
	SerXPortGblData_Tx64	*SerData = NULL;
#endif
	UINT32				USBDebugPortStartAddr;

	if(gDbgPerformanceRecords)
	    PERF_START (NULL, "DBG-CON", NULL, 0);
	
	//if Host is not connected and if not PMMode then return
	if(!IsAMIDebugRxConnected(1) && !IsAMIDebugRxConnected(0)) 
	{
		if(gDbgPerformanceRecords)
			PERF_END (NULL, "DBG-CON", NULL, 0);
		
		return EFI_SUCCESS;
	}
	//First find out whether DXE AMITransport is present or not
	Status = pBS->LocateHandleBuffer( ByProtocol, &AMITransportProtocolGuid,
			NULL, &NumHandles, &HandlePtr );

	if ( !EFI_ERROR( Status ) ){

		Status = pBS->OpenProtocol( HandlePtr[ 0 ],
				&AMITransportProtocolGuid,
				&TransportProtocol,
				NULL,			// ImageHandle,
				NULL,
				EFI_OPEN_PROTOCOL_GET_PROTOCOL );
		if ( EFI_ERROR( Status ) ) {
			if(gDbgPerformanceRecords)
				    PERF_END (NULL, "DBG-CON", NULL, 0);
			return ( Status );
		}

		if(pDeviceInfo->DebugPortType != SERIAL)
		{
			//_asm	pushfd;
//			DbgrDisableInterrupt();

			//Disable Hardware breakpoints including IRQs and Trap exceptions
			DisableDebugFeature(&BackupDR7);

			//First Reset the USB2 Debug Port
			TransportProtocol->Restore(TransportProtocol);
		}

		//Reprogram the EHCI BAR
		pDeviceInfo->Address.Addr.Register = pDeviceInfo->BarOffset;
		ReprogramECHI_BAR(pBS,pDeviceInfo->Address,(UINTN)pDeviceInfo->BaseAddress,NULL);

		if(pDeviceInfo->DebugPortType != SERIAL)
		{
			//Reinitialize the USB2 Debug Port and Device
			TransportProtocol->ReInitialize(TransportProtocol);

			//Eanble Hardware breakpoints
			EnableDebugFeature(&BackupDR7);

			//_asm	popfd;		//this should reenable INTs and Trap if previously set
//			DbgrRestoreInterrupt();
		}
	}
	else{
//		volatile int test=1;
		/*_asm{		//Clear Interrupt to avoid undetectable errors
			pushfd	// during enabling of EHC
			cli
		}*/
//		while(test);
//		DbgrDisableInterrupt();
		pDeviceInfo->Address.Addr.Register = pDeviceInfo->BarOffset;

		if(GetPEIDataSectionBaseAddress(&pPEIDS))
		{
#ifndef	EFIx64
			SerXPortGblData_T	*SerData = (SerXPortGblData_T *)(pPEIDS->XportLocalVar);

			SerVar = (SerXPortGblVar_T *)(pPEIDS->XportGlobalVar);
#else
			SerXPortGblData_Tx64	*SerData = (SerXPortGblData_Tx64 *)(pPEIDS->XportLocalVar);

			SerVar = (SerXPortGblVar_Tx64 *)(pPEIDS->XportGlobalVar);
#endif

			if(pDeviceInfo->DebugPortType != SERIAL)
			{
				//First Reset the USB2 Debug Port
				SerVar->gTransportProtocol->Restore(SerVar->gTransportProtocol);
			}

			ReprogramECHI_BAR(pBS,pDeviceInfo->Address,(UINTN)pDeviceInfo->BaseAddress,&USBDebugPortStartAddr);

			if(pDeviceInfo->DebugPortType != SERIAL)
			{
#if USB_DEBUG_TRANSPORT
				SerData->USBBASE = (UINT32)pDeviceInfo->BaseAddress;
				SerData->USB2_DEBUG_PORT_REGISTER_INTERFACE = USBDebugPortStartAddr;
				//Reinitialize the USB2 Debug Port and Device
				if(SerVar->gTransportProtocol->Initialize(SerVar->gTransportProtocol))
					Status = EFI_NOT_READY;
#endif
			}


		}
		else
		{
			ReprogramECHI_BAR(pBS,pDeviceInfo->Address,(UINTN)pDeviceInfo->BaseAddress,NULL);
		}

		//_asm popfd;
//		DbgrRestoreInterrupt();
	}

	if(gDbgPerformanceRecords)
	    PERF_END (NULL, "DBG-CON", NULL, 0);
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetNextMemoryTypeHob()
//
// Description:	Gets the Hob of Next Memory Type
//
// Input:		IN OUT  VOID      **,
// 				IN EFI_GUID  *Guid,
// 				OUT  VOID      **,
// 				OUT UINTN     *
//
// Output:		EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
static GetNextMemoryTypeHob (
  IN OUT VOID      **HobStart,
  IN     EFI_GUID  *Guid,
  OUT    VOID      **Buffer,
  OUT    UINTN     *BufferSize  OPTIONAL
  )

{
  EFI_STATUS        Status;
  EFI_PEI_HOB_POINTERS  GuidHob;

  for (Status = EFI_NOT_FOUND; EFI_ERROR (Status); ) {

    GuidHob.Raw = *HobStart;
    if (END_OF_HOB_LIST (GuidHob)) {
      return EFI_NOT_FOUND;
    }

    GuidHob.Raw = GetHob1 (EFI_HOB_TYPE_MEMORY_ALLOCATION, *HobStart);
    if (GuidHob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION) {
      if (EfiCompareGuid (Guid, &GuidHob.Guid->Name)) {
        Status = EFI_SUCCESS;
		*Buffer = (VOID *)((UINT8 *)(&GuidHob.Guid->Name));
        if (BufferSize) {
			*BufferSize = GuidHob.Header->HobLength - sizeof (EFI_HOB_GENERIC_HEADER);
        }
      }
    }

    *HobStart = GET_NEXT_HOB (GuidHob);
  }

  return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetPEIDataSectionBaseAddress()
//
// Description:	Gets the PEI Data Section Base Address
//
// Input:		PEIDebugData_Tx64	**pPEIDS
//
// Output:		BOOLEAN
//--------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN
static
GetPEIDataSectionBaseAddress (
#ifndef EFIx64
  PEIDebugData_T	**pPEIDS
#else
  PEIDebugData_Tx64	**pPEIDS
#endif
)
{
  EFI_STATUS							Status;
  EFI_HOB_GENERIC_HEADER				*HobList;
  EFI_PEI_HOB_POINTERS					Hob;
  EFI_HOB_MEMORY_ALLOCATION_HEADER      *MemoryHob;
  VOID									*DataInHob;
  UINTN									DataSize;
  EFI_PHYSICAL_ADDRESS 					DSMemoryBaseAddress = 0;
  //
  // Get Hob list
  //
  HobList = (EFI_HOB_GENERIC_HEADER*)GetEfiConfigurationTable(pST,&gHobListGuid);
  if (!HobList)
    return FALSE;

  //
  // Check for HtBist Data Hob.
  //
  Hob.Header = HobList;

  Status = GetNextMemoryTypeHob (&HobList, &mDxeDebugDataGuid, &DataInHob, &DataSize);

  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  else{
	  MemoryHob = (EFI_HOB_MEMORY_ALLOCATION_HEADER*) DataInHob;
	  (UINT32)DSMemoryBaseAddress = (UINT32)(MemoryHob->MemoryBaseAddress);
#ifndef EFIx64
	  *pPEIDS = (PEIDebugData_T*) DSMemoryBaseAddress;
#else
	  *pPEIDS = (PEIDebugData_Tx64*) DSMemoryBaseAddress;
#endif
  }
  return TRUE;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetHob1()
//
// Description:	This function returns the first instance of a HOB type in a HOB list.
//
// Input:		IN UINT16,
//  			IN VOID    *
//
// Output:		VOID *
//--------------------------------------------------------------------
//<AMI_PHDR_END>
VOID *
GetHob1 (
  IN UINT16  Type,
  IN VOID    *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  Hob;

  Hob.Raw = HobStart;
  //
  // Return input if not found
  //
  if (HobStart == NULL) {
    return HobStart;
  }

  //
  // Parse the HOB list, stop if end of list or matching type found.
  //
  while (!END_OF_HOB_LIST (Hob)) {

    if (Hob.Header->HobType == Type) {
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  //
  // Return input if not found
  //
  if (END_OF_HOB_LIST (Hob)) {
    return HobStart;
  }

  return (VOID *)(Hob.Raw);
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	EnableDisableDbgSupport()
//
// Description:	This function Links with the CSM Porting Hooks and
//				Enables\Disables the Debugger while handling Legacy Mode
//
// Input:		VOID  *Data,
//  			BOOLEAN Priority
//
// Output:		EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EnableDisableDbgSupport(VOID *Data, BOOLEAN Priority)
{
	if(Priority){ //CSM Legacy Mode Enter
		IntState = SaveAndDisableInterrupts();
		DisableDebugFeature(&BkpDR7);
	}
	else { //CSM Legacy Mode Exit
		EnableDebugFeature(&BkpDR7);
		//Clear the Global Var
		BkpDR7 = 0;
		if(IntState)
			EnableInterrupts();
		IntState = FALSE;
	}
	
	return EFI_SUCCESS;
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
