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

/** @file AmiPcieSegBusPei.c
    Contains the PEI library functions to get the segment and bus number
    of the socket and stack.
*/

//---------------------------------------------------------------------------
#include "IioUniversalData.h"
#include <AmiPcieSegBusLib/AmiPcieSegBusLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

EFI_GUID  gUniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiUpdatePcieSegmentBusTable
//
// Description: 
//  function to update PCI segment bus PCD table from IIO UDS HOB .
//
// Input:
//  IN *PcieSegBusTable         - Pointer to a PCIe segment Bus table.
//
// Output:
//     EFI_STATUS
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
AmiUpdatePcieSegmentBusTable (
  PCIE_SEG_BUS_TABLE    *PcieSegBusTable
  )
{
  UINT8                   SocIndex;
  UINT8                   StackIndex;
  IIO_UDS                 *IioUds;
  EFI_HOB_GUID_TYPE       *GuidHob;
  
  ASSERT (sizeof (PCIE_SEG_BUS_TABLE) >= PcdGetSize (PcdPcieSegBusTablePtr));
  
  //If table is not valid and updated, Locate IIO UDS HOB to update the PCIe Segment Bus Table
  GuidHob    = GetFirstGuidHob (&gUniversalDataGuid);
  
  if (GuidHob == NULL) {
    DEBUG ((EFI_D_ERROR, "AmiUpdatePcieSegmentBusTable Can't locate the IIO UDS HOB\n"));
    return EFI_NOT_FOUND;
  }
  
  IioUds = GET_GUID_HOB_DATA(GuidHob); 
  
  PcieSegBusTable->ValidFlag = TRUE;
  PcieSegBusTable->SocketPresentBitMap = IioUds->SystemStatus.socketPresentBitMap;
  
  for (SocIndex = 0; SocIndex < MAX_SOCKET; SocIndex++) {
    PcieSegBusTable->SocketSegBus[SocIndex].SocketSegmentNumber = IioUds->PlatformData.CpuQpiInfo[SocIndex].segmentSocket;
    PcieSegBusTable->SocketSegBus[SocIndex].StackPresentBitMap = IioUds->PlatformData.CpuQpiInfo[SocIndex].stackPresentBitmap;
    
    for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) {
      PcieSegBusTable->SocketSegBus[SocIndex].StackBusNumber[StackIndex] = IioUds->PlatformData.CpuQpiInfo[SocIndex].StackBus[StackIndex];
    }
  }
  
  DEBUG ((EFI_D_INFO, "AmiUpdatePcieSegmentBusTable Updated SEG BUS table PCD with IIO UDS HOB\n"));
  
  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiIsSocketValid
//
// Description: 
//  Return Socket state.
//
// Input:
//  IN SocketId               - Socket number of a multi socket system
//
// Output:
//     BOOLEAN
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN
AmiIsSocketValid (
  IN UINT8          SocketId
  )
{
  EFI_STATUS              Status;
  PCIE_SEG_BUS_TABLE      *PcieSegBusTable;
  
  //Get the PcdPcieSegBusTablePtr
  PcieSegBusTable = (PCIE_SEG_BUS_TABLE *) PcdGetPtr (PcdPcieSegBusTablePtr);
  
  if (!PcieSegBusTable->ValidFlag) {
    Status = AmiUpdatePcieSegmentBusTable (PcieSegBusTable);
    if (EFI_ERROR(Status))
      return FALSE;  
  }
  
  return (PcieSegBusTable->SocketPresentBitMap & (1 << SocketId));
    
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiIsStackValid
//
// Description: 
//  Return Stack state.
//
// Input:
//  IN SocketId               - Socket number of a multi socket system
//  IN StackId                - Stack number in a Socket
//
// Output:
//     BOOLEAN
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN
AmiIsStackValid (
  IN UINT8          SocketId,
  IN UINT8          StackId
  )
{
  EFI_STATUS              Status;    
  PCIE_SEG_BUS_TABLE      *PcieSegBusTable;
    
  //Get the PcdPcieSegBusTablePtr
  PcieSegBusTable = (PCIE_SEG_BUS_TABLE *) PcdGetPtr (PcdPcieSegBusTablePtr);
  
  if (!PcieSegBusTable->ValidFlag) {
    Status = AmiUpdatePcieSegmentBusTable (PcieSegBusTable);
    if (EFI_ERROR(Status))
      return FALSE;  
  }
    
  return (PcieSegBusTable->SocketSegBus[SocketId].StackPresentBitMap & (1 << StackId));
}


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiGetPcieSegmentBus
//
// Description: 
//  Return the PCI segment and bus number of specified Socket and stack.
//
// Input:
//  IN SocketId               - Socket number of a multi socket system
//  IN StackId                - Stack number in a Socket
//  IN OUT *Seg               - PCI Segment number
//  IN OUT *Bus               - PCI Bus number
//
// Output:
//     EFI_STATUS
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
EFIAPI
AmiGetPcieSegmentBus (
  IN UINT8          SocketId,
  IN UINT8          StackId,
  IN OUT UINT8      *Seg,
  IN OUT UINT8      *Bus
  )
{
  EFI_STATUS              Status;
  PCIE_SEG_BUS_TABLE      *PcieSegBusTable;
    
  
  //Get the PcdPcieSegBusTablePtr
  PcieSegBusTable = (PCIE_SEG_BUS_TABLE *) PcdGetPtr (PcdPcieSegBusTablePtr);
  
  if (!PcieSegBusTable->ValidFlag) {
    Status = AmiUpdatePcieSegmentBusTable (PcieSegBusTable);
    if (EFI_ERROR(Status))
      return EFI_NOT_READY;  
  }  
  
  //check if requested Socket ID is valid
  if (!(PcieSegBusTable->SocketPresentBitMap & (1 << SocketId)))
    return EFI_UNSUPPORTED;
  
  //check if requested Stack ID is valid
  if (!(PcieSegBusTable->SocketSegBus[SocketId].StackPresentBitMap & (1 << StackId)))
    return EFI_UNSUPPORTED;
  
  *Seg = PcieSegBusTable->SocketSegBus[SocketId].SocketSegmentNumber;
  *Bus = PcieSegBusTable->SocketSegBus[SocketId].StackBusNumber[StackId];
  
  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiGetPcieSegment
//
// Description: 
//  Return the PCI segment number of specified Socket and stack.
//
// Input:
//  IN SocketId               - Socket number of a multi socket system
//  IN OUT *Seg               - PCI Segment number
//
// Output:
//     EFI_STATUS
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
AmiGetPcieSegment (
  IN UINT8          SocketId,
  IN OUT UINT8      *Seg
  )
{
  EFI_STATUS              Status;
  PCIE_SEG_BUS_TABLE      *PcieSegBusTable;
    
  //Get the PcdPcieSegBusTablePtr
  PcieSegBusTable = (PCIE_SEG_BUS_TABLE *) PcdGetPtr (PcdPcieSegBusTablePtr);
  
  if (!PcieSegBusTable->ValidFlag) {
    Status = AmiUpdatePcieSegmentBusTable (PcieSegBusTable);
    if (EFI_ERROR(Status))
      return EFI_NOT_READY;  
  }
      
  //check if requested Socket ID is valid
  if (!(PcieSegBusTable->SocketPresentBitMap & (1 << SocketId)))
    return EFI_UNSUPPORTED;
    
  *Seg = PcieSegBusTable->SocketSegBus[SocketId].SocketSegmentNumber;
    
  return EFI_SUCCESS;
}

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
