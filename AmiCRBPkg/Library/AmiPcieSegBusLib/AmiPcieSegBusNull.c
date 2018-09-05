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

/** @file AmiPcieSegBusNull.c
    Contains the Base library functions to get the segment and bus.
*/

//---------------------------------------------------------------------------
#include <PiDxe.h>

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
  ASSERT (FALSE);
  return FALSE;
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
//  IN SocketId               - Socket number of a multiple socket system
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
  ASSERT (FALSE);
  return FALSE; 
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
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
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
  ASSERT (FALSE);	
  return EFI_UNSUPPORTED;
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
