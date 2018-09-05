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

/** @file AmiPcieSegBusDxe.c
    Contains the DXE library functions to get the segment and bus number
    of the socket and stack.
*/

//---------------------------------------------------------------------------
#include "IioUniversalData.h"
#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <AmiPcieSegBusLib/AmiPcieSegBusLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

//Global Variables
PCIE_SEG_BUS_TABLE      gPcieSegBusTable = {0};

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiUpdatePcieSegBusDxeSmmConstructor
//
// Description: 
//  updates local PCIE_SEG_BUS_TABLE structure and update PCI segment bus table from SEG BUS Table PCD Pointer.
//  if not initialized.
// Input:
//  ImageHandle - refers to the image handle of the driver.
//  SystemTable - points to the EFI System Table.
// Output:
//     EFI_STATUS
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
AmiUpdatePcieSegBusDxeSmmConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINTN                     TableSize;
  PCIE_SEG_BUS_TABLE        *PcieSegBusTable;
  
  PcieSegBusTable = (PCIE_SEG_BUS_TABLE *) PcdGetPtr (PcdPcieSegBusTablePtr);
  
  ASSERT (sizeof (PCIE_SEG_BUS_TABLE) >= PcdGetSize (PcdPcieSegBusTablePtr));
  
  TableSize = PcdGetSize (PcdPcieSegBusTablePtr);
  
  CopyMem (&gPcieSegBusTable, PcieSegBusTable, TableSize);
  
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
//  IN SocketId               - Socket number of a multiple socket system
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
  if (!gPcieSegBusTable.ValidFlag)
    return FALSE;
  
  return (gPcieSegBusTable.SocketPresentBitMap & (1 << SocketId));
    
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
  if (!gPcieSegBusTable.ValidFlag)
    return FALSE; 
  
  return (gPcieSegBusTable.SocketSegBus[SocketId].StackPresentBitMap & (1 << StackId));
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
  if (!gPcieSegBusTable.ValidFlag)
    return EFI_NOT_READY;
  
  //check if requested Socket ID is valid
  if (!(gPcieSegBusTable.SocketPresentBitMap & (1 << SocketId)))
    return EFI_UNSUPPORTED;
  
  //check if requested Stack ID is valid
  if (!(gPcieSegBusTable.SocketSegBus[SocketId].StackPresentBitMap & (1 << StackId)))
    return EFI_UNSUPPORTED;
  
  *Seg = gPcieSegBusTable.SocketSegBus[SocketId].SocketSegmentNumber;
  *Bus = gPcieSegBusTable.SocketSegBus[SocketId].StackBusNumber[StackId];
  
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
  if (!gPcieSegBusTable.ValidFlag)
    return EFI_NOT_READY;  
  
  //check if requested Socket ID is valid
  if (!(gPcieSegBusTable.SocketPresentBitMap & (1 << SocketId)))
    return EFI_UNSUPPORTED;
    
  *Seg = gPcieSegBusTable.SocketSegBus[SocketId].SocketSegmentNumber;
    
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
