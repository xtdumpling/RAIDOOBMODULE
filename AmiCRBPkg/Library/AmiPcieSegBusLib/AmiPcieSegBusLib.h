//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        AmiPcieSegBusLib.h
//
// Description: AmiPcieSegBusLib header file, define all the PCIe Segment and Bus
//              library specific equates and structures in this file.
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef  _AMI_PCIE_SEG_BUS_LIB_H_                         //To Avoid this header get compiled twice
#define  _AMI_PCIE_SEG_BUS_LIB_H_

#define AMI_PCIE_MAX_SOCKET                    8

typedef struct {
  UINT8         SocketSegmentNumber;
  UINT8         StackPresentBitMap;
  UINT8         StackBusNumber[MAX_IIO_STACK];
}PCIE_SOCKET_BUS;

typedef struct {
  UINT8             ValidFlag;
  UINT8             Reserved[3];
  UINT32            SocketPresentBitMap;
  PCIE_SOCKET_BUS   SocketSegBus[AMI_PCIE_MAX_SOCKET];
}PCIE_SEG_BUS_TABLE;

EFI_STATUS
EFIAPI
AmiUpdatePcieSegmentBusTable (
  PCIE_SEG_BUS_TABLE    *PcieSegBusTable
);

EFI_STATUS
EFIAPI
AmiGetPcieSegmentBus (
  IN UINT8          SocketId,
  IN UINT8          StackId,
  IN OUT UINT8      *Seg,
  IN OUT UINT8      *Bus
);

EFI_STATUS
EFIAPI
AmiGetPcieSegment (
  IN UINT8          SocketId,
  IN OUT UINT8      *Seg
);

BOOLEAN
AmiIsSocketValid (
  IN UINT8          SocketId
);

BOOLEAN
AmiIsStackValid (
  IN UINT8          SocketId,
  IN UINT8          StackId
);

#endif  // #ifndef  _AMI_PCIE_SEG_BUS_LIB_H_

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
